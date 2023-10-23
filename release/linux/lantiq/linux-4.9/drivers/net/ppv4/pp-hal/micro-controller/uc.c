/*
 * Description: PP micro-controllers
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_UC]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/pp_api.h>

#include "elf_parser.h"
#include "pp_regs.h"
#include "pp_common.h"
#include "infra.h"
#include "classifier.h"
#include "checker.h"
#include "rx_dma.h"
#include "uc.h"
#include "uc_regs.h"
#include "uc_internal.h"

/**
 * @define egress elf file name
 */
#define UC_A_EGR_ELF       "egress_uc_v1.elf"
#define UC_B_EGR_ELF       "egress_uc_v2.elf"
/**
 * @define ingress elf file name
 */
#define UC_A_ING_ELF       "ingress_uc_v1.elf"
#define UC_B_ING_ELF       "ingress_uc_v2.elf"
/**
 * @define maximum number of elf sections
 */
#define UC_ELF_SEC_MAX     32
/**
 * @define convert elf section address to offset
 */
#define UC_ELF_ADDR2OFF(a) ((a) & 0xFFFF)
/**
 * @define boot done value is written by the uc
 */
#define UC_BOOT_DONE_VAL   0x20
/**
 * @define ccu done value is written by the uc
 */
#define UC_CCU_DONE_VAL    1
/**
 * @define turn on value
 */
#define UC_TURN_ON_VAL     1
/**
 * @define MSB nibble of the PP mem-space when accessing from uc
 */
#define UC_PP_ADDR_MSB     UC_A_PP_FAT_ENT
/**
 * @define convert the phy-address from host perspective to uc
 *         perspective
 */
#define UC_PP_ADDR(a)      ((UC_PP_ADDR_MSB << 28) | (0x0FFFFFFF & (a)))

#define UC_LOGGER_BUFF_SZ  (sizeof(struct uc_log_msg) * 512)

/**
 * @brief egress uc HW base address
 */
u64 uc_egr_base_addr;
/**
 * @brief ingress uc HW base address
 */
u64 uc_ing_base_addr;

/**
 * @brief TX-Manager credit base address (phy)
 */
static u32 txm_cred_base;

/**
 * @brief checker physical base address
 */
static phys_addr_t chk_base_addr;

/**
 * @brief classifier physical base address
 */
static phys_addr_t cls_base_addr;

/**
 * @brief checker counter cache physical base address
 */
static phys_addr_t chk_ctrs_cache_base_addr;

/**
 * @brief checker physical base address
 */
static phys_addr_t rxdma_base_addr;

/**
 * @brief UC logger buffer cpu address
 */
static void      *logger_buff_virt;

/**
 * @brief UC logger buffer phys address
 */
static dma_addr_t logger_buff_phys;

/**
 * @brief UC logger buffer size in bytes
 */
static size_t     logger_buff_sz;

/**
 * @brief UC Tdox state offset
 */
static u32 tdox_state_arr_offset;

/**
 * @define base address for the tx manager add packet credit
 *         register per port.
 */
#define UC_TXMGR_CRED_INC_ADDR(p) (txm_cred_base + 8 + ((p) << 4))

/**
 * @brief valid bitmap for the egress cluster, pit per cpu
 */
static u8 eg_cpu_valid_bmap;
/**
 * @brief active bitmap for the egress cluster, pit per cpu
 */
static u8 eg_cpu_act_bmap;
/**
 * @brief valid bitmap for the ingress cluster, pit per cpu
 */
static u8 in_cpu_valid_bmap;
/**
 * @brief active bitmap for the ingress cluster, pit per cpu
 */
static u8 in_cpu_act_bmap;

static inline void __uc_cpu_active_set(bool uc_is_egr, u32 cid, bool enable);

bool uc_is_cluster_valid(bool uc_is_egr)
{
	u8 valid_bmap = uc_is_egr ? eg_cpu_valid_bmap : in_cpu_valid_bmap;

	if (unlikely(!valid_bmap)) {
		pr_err("invalid cluster %s\n",
		       uc_is_egr ? "EGRESS" : "INGRESS");
		return false;
	}

	return true;
}

bool uc_is_cpu_valid(bool uc_is_egr, u32 cid)
{
	u8 valid_bmap = uc_is_egr ? eg_cpu_valid_bmap : in_cpu_valid_bmap;

	if (unlikely(cid >= (BITS_PER_BYTE * sizeof(valid_bmap)) - 1)) {
		pr_err("invalid uc cpu id %u\n", cid);
		return false;
	}

	if (likely(valid_bmap & BIT(cid)))
		return true;

	pr_err("invalid %s cpu %u\n", uc_is_egr ? "EGRESS" : "INGRESS", cid);
	return false;
}

/**
 * @brief Test whether a task id is valid or not
 * @param uc_is_egr select cluster
 * @param tid task id
 * @return bool true in case it is valid, false otherwise
 */
static bool uc_is_task_id_valid(bool uc_is_egr, u32 tid)
{
	u32 num_tasks = uc_is_egr ? UC_EGR_MAXTASK : UC_ING_MAXTASK;

	if (tid < num_tasks)
		return true;

	pr_err("invalid %s task id %u\n", uc_is_egr ? "EGRESS" : "INGRESS",
	       tid);
	return false;
}

/**
 * @brief Test whether egress port id is valid or not
 * @param port the port id
 * @return bool true in case it is valid, false otherwise
 */
static bool uc_is_egr_port_valid(u32 port)
{
	if (port < UC_EGR_MAX_PORT)
		return true;

	pr_err("invalid egress port id %u\n", port);
	return false;
}

s32 uc_egr_writer_port_en_get(u32 port, bool *en)
{
	u64 addr;

	if (unlikely(ptr_is_null(en)))
		return -EINVAL;

	if (!uc_is_egr_port_valid(port))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		addr = UC_EGR_A_WRITER_PORT_ENABLE(port);
	else
		addr = UC_EGR_B_WRITER_PORT_ENABLE(port + UC_B_TXMGR_PORT_OFF);

	*en = !!PP_REG_RD32(addr);

	return 0;
}

s32 uc_egr_writer_map_get(u32 cid, u32 port, u32 *queue)
{
	u64 addr;

	if (unlikely(ptr_is_null(queue)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid) || !uc_is_egr_port_valid(port))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		addr = UC_EGR_A_WRITER_MAP(cid, port);
	else
		addr = UC_EGR_A_WRITER_MAP(cid, port + UC_B_TXMGR_PORT_OFF);

	*queue = PP_REG_RD32(addr);

	return 0;
}

s32 uc_egr_writer_queue_credit_get(u32 cid, u32 port, u32 *credit)
{
	u64 addr;

	if (unlikely(ptr_is_null(credit)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid) || !uc_is_egr_port_valid(port))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		addr = UC_EGR_A_WRITER_QUEUE_CREDIT(cid, port);
	else
		addr = UC_EGR_B_WRITER_QUEUE_CREDIT(cid,
						    port + UC_B_TXMGR_PORT_OFF);

	*credit = PP_REG_RD32(addr);

	return 0;
}

s32 uc_egr_writer_port_credit_get(u32 port, u32 *credit)
{
	u64 addr;

	if (unlikely(ptr_is_null(credit)))
		return -EINVAL;

	if (!uc_is_egr_port_valid(port))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		addr = UC_EGR_A_WRITER_PORT_CREDIT(port);
	else
		addr = UC_EGR_B_WRITER_PORT_CREDIT(port + UC_B_TXMGR_PORT_OFF);

	*credit = PP_REG_RD32(addr);

	return 0;
}

void uc_stats_reset(void)
{
	PP_REG_WR32(UC_SR(EGRESS, UC_SR_RESET_OFF), 1);
}

s32 uc_mcast_cpu_stats_get(u32 cid, struct mcast_stats *stats)
{
	u32 off;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid))
		return -EINVAL;

	off = offsetof(struct egress_stats, mcast) +
	      cid * sizeof(struct egress_stats);

	PP_REG_RD_REP(UC_SR(EGRESS, off), stats, sizeof(*stats));

	return 0;
}

s32 uc_ipsec_cpu_stats_get(u32 cid, struct ipsec_stats *stats)
{
	u32 off;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid))
		return -EINVAL;

	off = offsetof(struct egress_stats, ipsec) +
	      cid * sizeof(struct egress_stats);

	PP_REG_RD_REP(UC_SR(EGRESS, off), stats, sizeof(*stats));

	return 0;
}

s32 uc_reass_cpu_stats_get(u32 cid, struct reassembly_stats *stats)
{
	u32 off;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid))
		return -EINVAL;

	off = offsetof(struct egress_stats, reass) +
	      cid * sizeof(struct egress_stats);

	PP_REG_RD_REP(UC_SR(EGRESS, off), stats, sizeof(*stats));

	return 0;
}

s32 uc_frag_cpu_stats_get(u32 cid, struct frag_stats *stats)
{
	u32 off;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	if (!uc_is_cpu_valid(UC_IS_EGR, cid))
		return -EINVAL;

	off = offsetof(struct egress_stats, frag) +
	      cid * sizeof(struct egress_stats);

	PP_REG_RD_REP(UC_SR(EGRESS, off), stats, sizeof(*stats));

	return 0;
}

s32 uc_mcast_stats_get(struct mcast_stats *stats)
{
	struct mcast_stats cpu_stats;
	s32 ret;
	u32 i;

	for (i = 0; i < UC_CPUS_MAX; i++) {
		if (!uc_is_cpu_active(UC_IS_EGR, i))
			continue;

		ret = uc_mcast_cpu_stats_get(i, &cpu_stats);
		if (unlikely(ret)) {
			pr_err("failed to get mcast uc cpu %u counters\n", i);
			return ret;
		}
		stats->rx_pkt          += cpu_stats.rx_pkt;
		stats->tx_pkt          += cpu_stats.tx_pkt;
		stats->drop_pkt        += cpu_stats.drop_pkt;
		stats->mirror_tx_pkt   += cpu_stats.mirror_tx_pkt;
		stats->mirror_drop_pkt += cpu_stats.mirror_drop_pkt;
	}

	return 0;
}

s32 uc_reass_stats_get(struct reassembly_stats *stats)
{
	struct reassembly_stats cpu_stats;
	s32 ret;
	u32 i;
	u64 *it1, *it2;

	for (i = 0; i < UC_CPUS_MAX; i++) {
		if (!uc_is_cpu_active(UC_IS_EGR, i))
			continue;

		ret = uc_reass_cpu_stats_get(i, &cpu_stats);
		if (unlikely(ret)) {
			pr_err("failed to get reassembly uc cpu %u counters\n",
			       i);
			return ret;
		}

		/* add all counters to stats */
		it2 = (u64 *)(&cpu_stats);
		for_each_struct_mem(stats, it1, it2++)
			*it1 += *it2;
	}


	return 0;
}

s32 uc_frag_stats_get(struct frag_stats *stats)
{
	struct frag_stats cpu_stats;
	s32 ret;
	u32 i;

	for (i = 0; i < UC_CPUS_MAX; i++) {
		if (!uc_is_cpu_active(UC_IS_EGR, i))
			continue;

		ret = uc_frag_cpu_stats_get(i, &cpu_stats);
		if (unlikely(ret)) {
			pr_err("failed to get frag uc cpu %u counters\n", i);
			return ret;
		}
		stats->rx_pkt            += cpu_stats.rx_pkt;
		stats->tx_pkt            += cpu_stats.tx_pkt;
		stats->total_drops       += cpu_stats.total_drops;
		stats->bmgr_drops        += cpu_stats.bmgr_drops;
		stats->df_drops          += cpu_stats.df_drops;
		stats->max_frags_drops   += cpu_stats.max_frags_drops;
	}

	return 0;
}

s32 uc_task_pc_get(bool uc_is_egr, u32 cid, u32 tid, u32 *pc)
{
	u64 addr;

	if (ptr_is_null(pc))
		return -EINVAL;

	if (!uc_is_cpu_active(uc_is_egr, cid))
		return -EINVAL;

	if (!uc_is_task_id_valid(uc_is_egr, tid))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			addr = UC_A_AUX_PC_REG(EGRESS, cid, tid);
		else
			addr = UC_A_AUX_PC_REG(INGRESS, cid, tid);
	} else {
		if (uc_is_egr)
			addr = UC_B_AUX_PC_REG(EGRESS, cid, tid);
		else
			addr = UC_B_AUX_PC_REG(INGRESS, cid, tid);
	}

	*pc = PP_REG_RD32(addr);

	return 0;
}

s32 uc_task_status32_get(bool uc_is_egr, u32 cid, u32 tid, u32 *status)
{
	u64 addr;

	if (ptr_is_null(status))
		return -EINVAL;

	if (!uc_is_cpu_active(uc_is_egr, cid))
		return -EINVAL;

	if (!uc_is_task_id_valid(uc_is_egr, tid))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			addr = UC_A_AUX_S32_REG(EGRESS, cid, tid);
		else
			addr = UC_A_AUX_S32_REG(INGRESS, cid, tid);
	} else {
		if (uc_is_egr)
			addr = UC_B_AUX_S32_REG(EGRESS, cid, tid);
		else
			addr = UC_B_AUX_S32_REG(INGRESS, cid, tid);
	}
	*status = PP_REG_RD32(addr);

	return 0;
}

s32 uc_task_ready_get(bool uc_is_egr, u32 cid, u32 tid, bool *ready)
{
	u64 addr;

	if (ptr_is_null(ready))
		return -EINVAL;

	if (!uc_is_cpu_active(uc_is_egr, cid))
		return -EINVAL;

	if (!uc_is_task_id_valid(uc_is_egr, tid))
		return -EINVAL;

	if (uc_is_egr)
		addr = UC_MTSR_READY_REG(EGRESS, cid, tid);
	else
		addr = UC_MTSR_READY_REG(INGRESS, cid, tid);

	*ready = !!PP_REG_RD32(addr);

	return 0;
}

s32 uc_task_popa_active_get(bool uc_is_egr, u32 cid, u32 tid, bool *active,
			    u32 *queue)
{
	u32 num_queues, i;
	ulong active_map;
	u64 addr;

	if (ptr_is_null(active) || ptr_is_null(queue))
		return -EINVAL;

	if (!uc_is_cpu_active(uc_is_egr, cid))
		return -EINVAL;

	if (!uc_is_task_id_valid(uc_is_egr, tid))
		return -EINVAL;

	if (uc_is_egr)
		num_queues = UC_EGR_MAXQUEUE;
	else
		num_queues = UC_ING_MAXQUEUE;

	*active = false;
	for (i = 0; i < num_queues; i++) {
		if (pp_silicon_step_get() == PP_SSTEP_A) {
			if (uc_is_egr)
				addr = UC_A_POPA_ACT_LO_REG(EGRESS, cid, i);
			else
				addr = UC_A_POPA_ACT_LO_REG(INGRESS, cid, i);
		} else {
			if (uc_is_egr)
				addr = UC_B_POPA_ACT_LO_REG(EGRESS, cid, i);
			else
				addr = UC_B_POPA_ACT_LO_REG(INGRESS, cid, i);
		}

		active_map = (ulong)PP_REG_RD32(addr);

		if (!test_bit(tid, &active_map))
			continue;

		if (!*active) {
			*active = true;
			*queue = i;
		} else {
			pr_debug("%s task %u.%u is already active on queue %u\n",
				 uc_is_egr ? "EGRESS" : "INGRESS", cid, tid,
				 *queue);
		}
	}

	return 0;
}

s32 uc_task_regs_get(bool uc_is_egr, u32 cid, u32 tid, u32 *regs, u32 num)
{
	u64 addr;
	u32 i;

	if (ptr_is_null(regs))
		return -EINVAL;

	if (!uc_is_cpu_active(uc_is_egr, cid))
		return -EINVAL;

	if (!uc_is_task_id_valid(uc_is_egr, tid))
		return -EINVAL;

	for (i = 0; i < min(num, UC_TASK_REGS_NUM); i++) {
		if (pp_silicon_step_get() == PP_SSTEP_A) {
			if (uc_is_egr)
				addr = UC_A_PRIV_GP_REG(EGRESS, cid, i, tid);
			else
				addr = UC_A_PRIV_GP_REG(INGRESS, cid, i, tid);
		} else {
			if (uc_is_egr)
				addr = UC_B_PRIV_GP_REG(EGRESS, cid, i, tid);
			else
				addr = UC_B_PRIV_GP_REG(INGRESS, cid, i, tid);
		}
		regs[i] = PP_REG_RD32(addr);
	}

	return 0;
}

bool uc_is_cpu_active(bool uc_is_egr, u32 cid)
{
	u8 act_bmap = uc_is_egr ? eg_cpu_act_bmap : in_cpu_act_bmap;

	if (!uc_is_cpu_valid(uc_is_egr, cid))
		return false;

	if (likely(act_bmap & BIT(cid)))
		return true;

	pr_err("inactive %s cpu %u\n", uc_is_egr ? "EGRESS" : "INGRESS", cid);
	return false;
}

static void __uc_egr_reader_wred_addr_set(u32 addr)
{
	u64 reg;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		reg = UC_A_READER_WRED_BASE(EGRESS);
	else
		reg = UC_B_READER_WRED_BASE(EGRESS);

	PP_REG_WR32(reg, UC_PP_ADDR(addr));
}

static void __uc_egr_reader_client_addr_set(u32 addr)
{
	u64 reg;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		reg = UC_A_READER_CLIENT_BASE(EGRESS);
	else
		reg = UC_B_READER_CLIENT_BASE(EGRESS);

	PP_REG_WR32(reg, addr >> 20);
}

static void __uc_egr_reader_qm_addr_set(u32 addr)
{
	u64 reg;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		reg = UC_A_READER_QMNGR_BASE(EGRESS);
	else
		reg = UC_B_READER_QMNGR_BASE(EGRESS);

	PP_REG_WR32(reg, UC_PP_ADDR(addr));
}

static void __uc_egr_reader_bm_addr_set(u32 addr)
{
	u64 reg;
	u32 val = PP_FIELD_PREP(UC_READER_BMNGR_BASE_MSK,
				(UC_PP_ADDR(addr)) >> 16);

	if (pp_silicon_step_get() == PP_SSTEP_A)
		reg = UC_A_READER_BMNGR_BASE(EGRESS);
	else
		reg = UC_B_READER_BMNGR_BASE(EGRESS);

	PP_REG_WR32(reg, val);

}

s32 uc_ccu_maxcpus_get(bool uc_is_egr, u8 *max_cpus)
{
	u64 reg;

	if (unlikely(ptr_is_null(max_cpus)))
		return -EINVAL;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			reg = PP_REG_RD32(UC_A_CCU_MAXCPU_REG(EGRESS));
		else
			reg = PP_REG_RD32(UC_A_CCU_MAXCPU_REG(INGRESS));
	} else {
		if (uc_is_egr)
			reg = PP_REG_RD32(UC_B_CCU_MAXCPU_REG(EGRESS));
		else
			reg = PP_REG_RD32(UC_B_CCU_MAXCPU_REG(INGRESS));
	}

	*max_cpus = PP_FIELD_GET(UC_CCU_MAXCPU_MSK, reg);

	return 0;
}

s32 uc_ccu_gpreg_get(bool uc_is_egr, u32 idx, u32 *gpreg)
{
	u64 reg;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (unlikely(ptr_is_null(gpreg)))
		return -EINVAL;

	if (unlikely(idx >= UC_CCU_GPREG_MAX)) {
		pr_err("invalid %s gpreg index %u, max=%u\n",
		       uc_is_egr ? "EGRESS" : "INGRESS", idx, UC_CCU_GPREG_MAX);
		return -EINVAL;
	}

	/* read ccu gpreg */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			reg = UC_A_CCU_GPREG_REG(EGRESS, idx);
		else
			reg = UC_A_CCU_GPREG_REG(INGRESS, idx);
	} else {
		if (uc_is_egr)
			reg = UC_B_CCU_GPREG_REG(EGRESS, idx);
		else
			reg = UC_B_CCU_GPREG_REG(INGRESS, idx);
	}

	*gpreg = PP_REG_RD32(reg);

	return 0;
}

s32 uc_ccu_gpreg_set(bool uc_is_egr, u32 idx, u32 gpreg_val)
{
	u64 reg;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (unlikely(idx >= UC_CCU_GPREG_MAX)) {
		pr_err("invalid %s gpreg index %u, max=%u\n",
		       uc_is_egr ? "EGRESS" : "INGRESS", idx, UC_CCU_GPREG_MAX);
		return -EINVAL;
	}

	/* write ccu gpreg */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			reg = UC_A_CCU_GPREG_REG(EGRESS, idx);
		else
			reg = UC_A_CCU_GPREG_REG(INGRESS, idx);
	} else {
		if (uc_is_egr)
			reg = UC_B_CCU_GPREG_REG(EGRESS, idx);
		else
			reg = UC_B_CCU_GPREG_REG(INGRESS, idx);
	}

	PP_REG_WR32(reg, gpreg_val);

	return 0;
}

s32 uc_ccu_irr_get(bool uc_is_egr, u32 *irr_bitmap)
{
	u64 reg;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (unlikely(ptr_is_null(irr_bitmap)))
		return -EINVAL;

	/* read interrupt request register */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			reg = UC_A_CCU_IRR_REG(EGRESS);
		else
			reg = UC_A_CCU_IRR_REG(INGRESS);
	} else {
		if (uc_is_egr)
			reg = UC_B_CCU_IRR_REG(EGRESS);
		else
			reg = UC_B_CCU_IRR_REG(INGRESS);
	}

	*irr_bitmap = PP_REG_RD32(reg);

	return 0;
}

void uc_ccu_enable_set(bool uc_is_egr, u32 cid, bool enable)
{
	u32 val = enable ? UC_TURN_ON_VAL : 0;

	if (unlikely(!uc_is_cpu_valid(uc_is_egr, cid)))
		return;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr) {
			/* take the uc in/out of reset */
			PP_REG_WR32(UC_A_CCU_UC_UNRESET_REG(EGRESS, cid), val);
			/* enable/disable the uc clock */
			PP_REG_WR32(UC_A_CCU_UC_CLK_EN_REG(EGRESS, cid), val);
		} else {
			/* take the uc in/out of reset */
			PP_REG_WR32(UC_A_CCU_UC_UNRESET_REG(INGRESS, cid), val);
			/* enable/disable the uc clock */
			PP_REG_WR32(UC_A_CCU_UC_CLK_EN_REG(INGRESS, cid), val);
		}
	} else {
		if (uc_is_egr) {
			/* take the uc in/out of reset */
			PP_REG_WR32(UC_B_CCU_UC_UNRESET_REG(EGRESS, cid), val);
			/* enable/disable the uc clock */
			PP_REG_WR32(UC_B_CCU_UC_CLK_EN_REG(EGRESS, cid), val);
		} else {
			/* take the uc in/out of reset */
			PP_REG_WR32(UC_B_CCU_UC_UNRESET_REG(INGRESS, cid), val);
			/* enable/disable the uc clock */
			PP_REG_WR32(UC_B_CCU_UC_CLK_EN_REG(INGRESS, cid), val);
		}
	}

	__uc_cpu_active_set(uc_is_egr, cid, enable);
}

void uc_run_set(bool uc_is_egr, u32 cid, bool enable)
{
	u32 val = enable ? UC_TURN_ON_VAL : 0;

	if (unlikely(!uc_is_cpu_active(uc_is_egr, cid)))
		return;

	/* run the uc core */
	if (uc_is_egr)
		PP_REG_WR32(UC_BOOT_CORE_RUN_REG(EGRESS, cid), val);
	else
		PP_REG_WR32(UC_BOOT_CORE_RUN_REG(INGRESS, cid), val);
}

s32 uc_reader_irr_get(bool uc_is_egr, u32 *irr_bitmap)
{
	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (unlikely(ptr_is_null(irr_bitmap)))
		return -EINVAL;

	/* read interrupt request register */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			*irr_bitmap = PP_REG_RD32(UC_A_READER_IRR(EGRESS));
		else
			*irr_bitmap = PP_REG_RD32(UC_A_READER_IRR(INGRESS));
	} else {
		if (uc_is_egr)
			*irr_bitmap = PP_REG_RD32(UC_B_READER_IRR(EGRESS));
		else
			*irr_bitmap = PP_REG_RD32(UC_B_READER_IRR(INGRESS));
	}

	return 0;
}

s32 uc_fat_get(bool uc_is_egr, u32 ent, u32 *val, bool *enable)
{
	u64 reg;
	u32 max_ent;

	if (unlikely(ptr_is_null(val) || ptr_is_null(enable)))
		return -EINVAL;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		max_ent = UC_A_FAT_MAX_ENT;
	else
		max_ent = UC_B_FAT_ALL_MAX_ENT;

	if (unlikely(ent >= max_ent)) {
		pr_err("invalid %s cluster fat entry %u\n",
		       uc_is_egr ? "EGRESS" : "INGRESS", ent);
		return -EINVAL;
	}

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		if (uc_is_egr)
			reg = UC_A_CLUSTERBRIDGE_FAT(EGRESS, ent);
		else
			reg = UC_A_CLUSTERBRIDGE_FAT(INGRESS, ent);
		*val = PP_FIELD_GET(UC_A_FAT_VAL_MSK, PP_REG_RD32(reg));
		*enable = PP_FIELD_GET(UC_A_FAT_EN_MSK, PP_REG_RD32(reg));
	} else {
		if (uc_is_egr) {
			reg = UC_B_CLUSTERBRIDGE_VALID(EGRESS, ent);
			*enable = PP_FIELD_GET(UC_B_FAT_EN_MSK,
					       PP_REG_RD32(reg));
			if (ent < UC_B_FAT_MAX_ENT) {
				reg = UC_B_CLUSTERBRIDGE_FAT(EGRESS, ent);
				*val = PP_FIELD_GET(UC_B_FAT_VAL_MSK,
						    PP_REG_RD32(reg));
			} else {
				ent -= UC_B_FAT_MAX_ENT;
				reg = UC_B_CLUSTERBRIDGE_BASE(EGRESS, ent);
				*val = PP_REG_RD32(reg);
			}
		} else {
			reg = UC_B_CLUSTERBRIDGE_VALID(INGRESS, ent);
			*enable = PP_FIELD_GET(UC_B_FAT_EN_MSK,
					       PP_REG_RD32(reg));
			if (ent < UC_B_FAT_MAX_ENT) {
				reg = UC_B_CLUSTERBRIDGE_FAT(INGRESS, ent);
				*val = PP_FIELD_GET(UC_B_FAT_VAL_MSK,
						    PP_REG_RD32(reg));
			} else {
				ent -= UC_B_FAT_MAX_ENT;
				reg = UC_B_CLUSTERBRIDGE_BASE(INGRESS, ent);
				*val = PP_REG_RD32(reg);
			}
		}
	}

	return 0;
}

s32 uc_fat_set(bool uc_is_egr, u8 ent, u32 val, bool enable)
{
	u32 max_ent;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		max_ent = UC_A_FAT_MAX_ENT;
		val &= UC_A_FAT_VAL_MSK;
	} else {
		max_ent = UC_B_FAT_ALL_MAX_ENT;
	}

	if (unlikely(ent >= max_ent)) {
		pr_err("invalid %s cluster fat entry %u\n",
		       uc_is_egr ? "EGRESS" : "INGRESS", ent);
		return -EINVAL;
	}

	/* enable/disable the cluster bridge fat */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		val |= enable << UC_A_FAT_EN_BIT_SHIFT;
		if (uc_is_egr)
			PP_REG_WR32(UC_A_CLUSTERBRIDGE_FAT(EGRESS, ent), val);
		else
			PP_REG_WR32(UC_A_CLUSTERBRIDGE_FAT(INGRESS, ent), val);
	} else {
		if (uc_is_egr) {
			PP_REG_WR32(UC_B_CLUSTERBRIDGE_VALID(EGRESS, ent),
				    enable);
			if (ent < UC_B_FAT_MAX_ENT) {
				PP_REG_WR32(UC_B_CLUSTERBRIDGE_FAT(EGRESS, ent),
					    val);
			} else {
				ent -= UC_B_FAT_MAX_ENT;
				PP_REG_WR32(UC_B_CLUSTERBRIDGE_BASE(EGRESS,
								    ent), val);
			}
		} else {
			PP_REG_WR32(UC_B_CLUSTERBRIDGE_VALID(INGRESS, ent),
				    enable);
			if (ent < UC_B_FAT_MAX_ENT) {
				PP_REG_WR32(UC_B_CLUSTERBRIDGE_FAT(INGRESS,
								   ent), val);
			} else {
				ent -= UC_B_FAT_MAX_ENT;
				PP_REG_WR32(UC_B_CLUSTERBRIDGE_BASE(INGRESS,
								    ent), val);
			}
		}
	}

	return 0;
}

s32 uc_ver_get(bool uc_is_egr, u32 *ver)
{
	s32 ret;

	if (unlikely(ptr_is_null(ver)))
		return -EINVAL;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	if (uc_is_egr) {
		/* send the mailbox command */
		ret = uc_egr_mbox_cmd_send(UC_CMD_VERSION, 0, NULL, 0,
					   ver, sizeof(*ver));
		if (unlikely(ret))
			return ret;
	} else {
		/* read version from SRAM */
		*ver = PP_REG_RD32(UC_SSRAM(INGRESS, INGRESS_VER_SSRAM_OFF));
	}

	pr_debug("pp uc %s version:MAJOR[%u], MINOR[%u], BUILD[%u]\n",
		 uc_is_egr ? "egress" : "ingress",
		 (*ver >> 16) & U8_MAX, (*ver >> 8) & U8_MAX, *ver & U8_MAX);

	return 0;
}

/**
 * @brief enable/disable the active bitmap for cpu
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param enable enable/disable
 */
static inline void __uc_cpu_active_set(bool uc_is_egr, u32 cid, bool enable)
{
	u8 *act_bmap = uc_is_egr ? &eg_cpu_act_bmap : &in_cpu_act_bmap;

	if (enable)
		*act_bmap |= BIT(cid);
	else
		*act_bmap &= ~BIT(cid);
}

/**
 * @brief check if the uc init was done
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @return bool true if done, fasle otherwise
 */
static bool __uc_is_uc_init_done(bool uc_is_egr, u32 cid)
{
	u64 reg_addr;

	if (uc_is_egr)
		reg_addr = UC_BOOT_GPREG_REG(EGRESS, cid);
	else
		reg_addr = UC_BOOT_GPREG_REG(INGRESS, cid);

	pr_debug("start polling on uc %u init done reg\n", cid);
	/* wait until the uc boot init will be done */
	if (unlikely(pp_reg_poll_x(reg_addr, UC_BOOT_GPREG_MSK,
				   UC_BOOT_DONE_VAL, PP_REG_MAX_RETRIES))) {
		pr_err("Failed polling on uc %u init done reg\n", cid);
		return false;
	}
	pr_debug("uc boot done, cid %d\n", cid);

	return true;
}

/**
 * @brief enable/disable the valid bitmap for cpu
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param enable enable/disable
 */
static inline void __uc_cpu_valid_set(bool uc_is_egr, u32 cid, bool enable)
{
	u8 *valid_bmap = uc_is_egr ? &eg_cpu_valid_bmap : &in_cpu_valid_bmap;

	if (enable)
		*valid_bmap |= BIT(cid);
	else
		*valid_bmap &= ~BIT(cid);
}

/**
 * @brief copy elf section to uc shared SRAM
 * @param dst destination address
 * @param _src source address
 * @param size number of bytes to copy
 */
static void __uc_copy_sec(unsigned long dst, const void *_src, u32 size)
{
	u32 i;
	const u32 *src;

	src = (u32 *)_src;

	for (i = size; i > 3; i -= 4) {
		PP_REG_WR32(dst, *(src++));
		dst += 4;
	}

	/* Section size must be aligned to 2 */
	if (i == 1 || i == 3)
		pr_err("Size %u not aligned to 2 (i=%u)\n", size, i);

	/* Copy last u16 if exists */
	if (i == 2)
		PP_REG_WR32(dst, *(u16 *)src);
}

/**
 * @brief download the image to the uc shared SRAM
 * @param name elf file name
 * @param rst_vec reset vector array
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_download_image(const char *name, u32 *rst_vec)
{
	const struct firmware *fw;
	struct elf_sec *secs;
	u16 i, num_secs = 0;
	unsigned long dst;
	s32 ret = 0;

	secs = kcalloc(UC_ELF_SEC_MAX, sizeof(*secs), GFP_KERNEL);
	if (unlikely(!secs)) {
		pr_err("failed to allocate fw sections\n");
		return -ENOMEM;
	}

	ret = elf_request_fw_and_parse(pp_dev_get(), name, &fw, secs,
				       UC_ELF_SEC_MAX, &num_secs);
	if (unlikely(ret)) {
		pr_err("failed to parse the fw elf file %s\n", name);
		goto free_secs;
	}

	/* go over all sections */
	for (i = 0; i < num_secs; i++) {
		if (!secs[i].need_copy || !secs[i].size)
			continue;

		pr_debug("SEC[%hu] name %s, address %#llx, size %llu\n",
			 i, secs[i].name, secs[i].addr, secs[i].size);

		/* reset vector will be copied to the uc boot later */
		if (!strncmp(secs[i].name, ".reset_vector",
			     strlen(".reset_vector"))) {
			if (secs[i].size > UC_BOOT_RST_VEC_WORDS) {
				pr_err("uc reset vector section size %llu larger then %u\n",
				       secs[i].size, UC_BOOT_RST_VEC_WORDS);
				ret = -EFBIG;
				goto release_elf;
			}
			/* save the reset vector for later used */
			memcpy(rst_vec, secs[i].data, secs[i].size);
			continue;
		}
		/* local db (DCCM) will be reset by the uc at init */
		if (!strncmp(secs[i].name, ".local_db", strlen(".local_db")))
			continue;

		if (!strncmp(name, UC_A_EGR_ELF, strlen(UC_A_EGR_ELF)) ||
		    !strncmp(name, UC_B_EGR_ELF, strlen(UC_B_EGR_ELF)))
			dst = UC_SSRAM(EGRESS, UC_ELF_ADDR2OFF(secs[i].addr));
		else
			dst = UC_SSRAM(INGRESS, UC_ELF_ADDR2OFF(secs[i].addr));
		__uc_copy_sec(dst, secs[i].data, secs[i].size);
	}

release_elf:
	/* release the FW file */
	elf_release_fw(fw);
free_secs:
	kfree(secs);

	return ret;
}

/**
 * @brief verify the cluster by checking the number of cpus
 *        available in HW
 * @param uc_is_egr select the uc cluster
 * @param max_cpu number of cpus requested from the driver
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_cluster_verify(bool uc_is_egr, u32 max_cpu)
{
	u8 hw_max_cpu;
	s32 ret;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	ret = uc_ccu_maxcpus_get(uc_is_egr, &hw_max_cpu);
	if (unlikely(ret))
		return ret;

	if ((u32)hw_max_cpu < max_cpu) {
		pr_err("invalid %s max cpus parameter %u, max_hw %hhu\n",
		       uc_is_egr ? "egress" : "ingress", max_cpu, hw_max_cpu);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief set the default uc cluster bridge fat configuration
 *        for A-step
 * @param uc_is_egr select the uc cluster
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_cluster_bridge_a_default_set(bool uc_is_egr)
{
	s32 ret;
	u32 i;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	/* open all entries for 32bit access */
	for (i = 0; i < UC_A_FAT_MAX_ENT; i++) {
		switch (i) {
		case UC_A_PP_FAT_ENT:
			ret = uc_fat_set(uc_is_egr, i, UC_A_PP_FAT_VAL, 1);
			break;
		case UC_A_IOC_FAT_ENT:
			ret = uc_fat_set(uc_is_egr, i, UC_A_IOC_FAT_VAL, 1);
			break;
		default:
			ret = uc_fat_set(uc_is_egr, i, i, 1);
			break;
		}

		if (unlikely(ret))
			return ret;
	}

	return 0;
}

/**
 * @brief set the default uc cluster bridge fat configuration
 *        for B-step
 * @param uc_is_egr select the uc cluster
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_cluster_bridge_b_default_set(bool uc_is_egr)
{
	s32 ret;
	u32 i;

	if (unlikely(!uc_is_cluster_valid(uc_is_egr)))
		return -EPERM;

	/* open all fat entries for 32bit access */
	for (i = 0; i < UC_B_FAT_MAX_ENT; i++) {
		if (i >= UC_B_PP_FAT_ENT_BASE && i <
		    UC_B_PP_FAT_ENT_BASE + UC_B_PP_FAT_NUM_ENT) {
			/* open the ppv4 access */
			ret = uc_fat_set(uc_is_egr, i, UC_B_PP_FAT_VAL_BASE +
					 i - UC_B_PP_FAT_ENT_BASE, 1);
		} else if (i >= UC_B_IOC_FAT_ENT_BASE && i <
		    UC_B_IOC_FAT_ENT_BASE + UC_B_PP_FAT_NUM_ENT) {
			/* open the ppv4 access */
			ret = uc_fat_set(uc_is_egr, i,
					 UC_B_IOC_C5_FAT_VAL_BASE +
					 i - UC_B_IOC_FAT_ENT_BASE, 1);
		} else {
			ret = uc_fat_set(uc_is_egr, i, i, 1);
		}
		if (unlikely(ret))
			return ret;
	}

	/* open all spacial fat entries for 32bit access */
	for (; i < UC_B_FAT_ALL_MAX_ENT; i++) {
		if (i >= UC_B_IOC_C6_FAT_ENT_BASE && i <
		    UC_B_IOC_C6_FAT_ENT_BASE + UC_B_PP_FAT_NUM_ENT) {
			/* open the ppv4 access */
			ret = uc_fat_set(uc_is_egr, i,
					 UC_B_IOC_C6_FAT_VAL_BASE(
					 i - UC_B_IOC_C6_FAT_ENT_BASE), 1);
		} else {
			ret = uc_fat_set(uc_is_egr, i, i << 22, 1);
		}
		if (unlikely(ret))
			return ret;
	}

	return 0;
}

static s32 __uc_cluster_mem_access_set(bool uc_is_egr)
{
	if (pp_silicon_step_get() == PP_SSTEP_A)
		return __uc_cluster_bridge_a_default_set(uc_is_egr);
	else
		return __uc_cluster_bridge_b_default_set(uc_is_egr);
}

/**
 * @brief set the uc shared accelerators
 * @param cfg uc init parameters
 * @param max_cpu num of cpus
 * @param uc_is_egr select the uc cluster
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_shrd_acc_set(struct uc_egr_init_params *cfg, u32 max_cpu,
			     bool uc_is_egr)
{
	s32 ret, i;
	u8 hw_max_cpu;

	ret = uc_ccu_maxcpus_get(uc_is_egr, &hw_max_cpu);
	if (unlikely(ret))
		return ret;

	for (i = 0; i < hw_max_cpu; i++)
		__uc_cpu_valid_set(uc_is_egr, i, true);

	ret = __uc_cluster_verify(uc_is_egr, max_cpu);
	if (unlikely(ret))
		return ret;

	ret = __uc_cluster_mem_access_set(uc_is_egr);
	if (unlikely(ret))
		return ret;

	if (uc_is_egr == UC_IS_EGR) {
		__uc_egr_reader_wred_addr_set(cfg->wred_base);
		__uc_egr_reader_client_addr_set(cfg->client_base);
		__uc_egr_reader_qm_addr_set(cfg->qm_base);
		__uc_egr_reader_bm_addr_set(cfg->bm_base);
	}

	return 0;
}

/**
 * @brief set the uc profile
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param prof profile index
 */
static void __uc_profile_set(bool uc_is_egr, u32 cid, u32 prof)
{
	pr_debug("pp uc cpu %u profile %u\n", cid, prof);

	/* set the uc profile */
	if (uc_is_egr)
		PP_REG_WR32(UC_BOOT_GPREG_REG(EGRESS, cid), prof);
	else
		PP_REG_WR32(UC_BOOT_GPREG_REG(INGRESS, cid), prof);
}

/**
 * @brief set the uc reset vector
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param vec reset vector array
 */
static void __uc_rst_vec_set(bool uc_is_egr, u32 cid, u32 *vec)
{
	u32 i;

	/* set the uc reset vector */
	for (i = 0; i < UC_BOOT_RST_VEC_WORDS; i++) {
		if (uc_is_egr) {
			PP_REG_WR32(UC_BOOT_RST_VEC_REG(EGRESS, cid, i),
				    vec[i]);
		} else {
			PP_REG_WR32(UC_BOOT_RST_VEC_REG(INGRESS, cid, i),
				    vec[i]);
		}
	}
}

static s32 __uc_txmgr_port_update(u16 port, ulong ring_addr, u16 ring_sz,
				  u16 credit)
{
	struct pp_qos_dev *qdev;
	struct pp_qos_port_conf conf;
	u32 logical_id;
	s32 ret;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EPERM;

	logical_id = pp_qos_port_id_get(qdev, port);

	ret = pp_qos_port_conf_get(qdev, logical_id, &conf);
	if (unlikely(ret)) {
		pr_err("pp_qos_port_conf_get failed\n");
		goto done;
	}
	conf.ring_address = ring_addr;
	conf.ring_size    = ring_sz;
	conf.credit       = credit;
	ret = pp_qos_port_set(qdev, logical_id, &conf);
	if (unlikely(ret)) {
		pr_err("pp_qos_port_set failed\n");
		goto done;
	}

	pr_debug("set port done: phys: %d. logical: %d\n", port, logical_id);

done:
	return ret;
}

s32 uc_aux_reg_read(u32 reg, u32 *val)
{
	if (ptr_is_null(val))
		return -EINVAL;

	return uc_egr_mbox_cmd_send(UC_CMD_AUX_REG_RD, reg, NULL, 0,
				    val, sizeof(*val));
}

s32 uc_aux_reg_write(u32 reg, u32 val)
{
	struct mbox_aux_reg_wr args;

	args.reg = reg;
	args.val = val;

	return uc_egr_mbox_cmd_send(UC_CMD_AUX_REG_WR, 0, (const void *)&args,
				    sizeof(args), NULL, 0);
}

/* Converts IO coherent address to uc perspective */
static u32 __convert_36b_addr_to_32b(u64 addr)
{
	u32 msb = (addr >> 28) & 0xFF;
	u32 uc_addr = addr;

	/* Replace 0xC5XXX address to 0xBXXX */
	if (msb == 0xc5) {
		uc_addr &= 0x0FFFFFFF;
		uc_addr |= 0xB0000000;
	} else if (pp_silicon_step_get() == PP_SSTEP_B) {
		/* Replace 0xC6XXX address to 0xCXXX */
		if (msb == 0xc6) {
			uc_addr &= 0x0FFFFFFF;
			uc_addr |= 0xC0000000;
		}
	}

	return uc_addr;
}

s32 __nf_reass_set(u16 tx_queue, u16 dflt_hif)
{
	struct reassembly_info info;
	struct pp_si dflt_si;
	s32 ret;
	struct rx_dma_port_cfg port_cfg;

	memset(&info, 0, sizeof(info));

	ret = chk_exception_session_si_get(dflt_hif, &dflt_si);
	if (unlikely(ret)) {
		pr_err("failed to to get dflt host i/f %u si, ret %d\n",
		       dflt_hif, ret);
		return ret;
	}

	info.pp_rx_q            = tx_queue;
	info.host_q             = dflt_si.dst_q;
	info.host_base_policy   = dflt_si.base_policy;
	info.host_policies_bmap = dflt_si.policies_map;
	info.host_pid           = dflt_si.eg_port;
	info.rxdma_base         = UC_PP_ADDR(rxdma_base_addr);
	info.si_base = __convert_36b_addr_to_32b(cls_si_phys_base_get());
	if (pp_silicon_step_get() == PP_SSTEP_A)
		info.excp_si_base = __convert_36b_addr_to_32b(
			chk_sh_excp_si_phys_base_get());
	else
		info.excp_si_base = UC_PP_ADDR(chk_excp_si_phys_base_get());

	port_cfg.port_id = dflt_si.eg_port;
	ret = rx_dma_port_get(&port_cfg);
	if (unlikely(ret)) {
		pr_err("rx_dma_port_get err %d\n", ret);
		return ret;
	}

	info.host_port_hr = port_cfg.headroom_size;
	info.host_port_tr = port_cfg.tailroom_size;
	info.host_port_flags = 0;
	if (port_cfg.wr_desc)
		info.host_port_flags |= BUFF_EX_FLAG_WR_DESC;

	/* Write host info to SRAM to be used by ingress uc */
	PP_REG_WR32(UC_SSRAM(INGRESS, INGRESS_HOST_INFO_PORT_SSRAM_OFF),
		    dflt_si.eg_port);
	PP_REG_WR32(UC_SSRAM(INGRESS, INGRESS_HOST_INFO_QUEUE_SSRAM_OFF),
		    dflt_si.dst_q);
	PP_REG_WR32(UC_SSRAM(INGRESS, INGRESS_HOST_INFO_BASE_POLICY_SSRAM_OFF),
		    dflt_si.base_policy);
	PP_REG_WR32(UC_SSRAM(INGRESS, INGRESS_HOST_INFO_POLICY_BMAP_SSRAM_OFF),
		    dflt_si.policies_map);

	return uc_egr_mbox_cmd_send(UC_CMD_REASSEMBLY_INFO, 0,
				    (const void *)&info, sizeof(info), NULL, 0);
}

static s32 __writer_port_enable(u32 uc_port, u16 qos_port)
{
	ulong r_addr, p_enable_reg;
	u16 r_sz, r_cred;
	void *r_addr_virt;
	s32 ret;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		p_enable_reg = UC_EGR_A_WRITER_PORT_ENABLE(uc_port);
	else
		p_enable_reg = UC_EGR_B_WRITER_PORT_ENABLE(uc_port);

	if (PP_REG_RD32(p_enable_reg) == UC_TURN_ON_VAL) {
		pr_err("uc nf type %u already enabled\n", uc_port);
		return -EPERM;
	}

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		/* set the tx manager port address in CPU3 DCCM */
		PP_REG_WR32(UC_SW_WRITER_TX_PORT_CRED(uc_port),
			    UC_PP_ADDR(UC_TXMGR_CRED_INC_ADDR(qos_port)));
		r_sz = 2;
		r_cred = 2;
		r_addr_virt = (void *)(ulong)UC_SW_WRITER_TX_PORT_RING(uc_port);
	} else {
		/* set the tx manager port address */
		PP_REG_WR32(UC_EGR_B_WRITER_TX_PORT(uc_port),
			    UC_PP_ADDR(UC_TXMGR_CRED_INC_ADDR(qos_port)));
		r_sz = 1;
		r_cred = 4;
		r_addr_virt = (void *)(ulong)UC_EGR_B_WRITER_PORT_ADDR(uc_port);
	}
	r_addr = (ulong)pp_virt_to_phys(r_addr_virt);
	ret = __uc_txmgr_port_update(qos_port, r_addr, r_sz, r_cred);
	if (unlikely(ret))
		return ret;

	/* enable the port */
	PP_REG_WR32(p_enable_reg, UC_TURN_ON_VAL);

	return 0;
}

static s32 __nf_tdox_set(void)
{
	struct tdox_uc_config cfg;
	s32 ret = 0;

	ret = uc_egr_mbox_cmd_send(UC_CMD_TDOX_CONF_GET, 0, NULL, 0, &cfg,
				   sizeof(cfg));
	if (unlikely(ret)) {
		pr_err("failed to get tdox config\n");
		return ret;
	}

	tdox_state_arr_offset = cfg.record_state_offset;
	pr_info("tdox_state offset: %#x\n", tdox_state_arr_offset);

	return ret;
}

s32 uc_tdox_state_get(u16 tdox_id, enum tdox_uc_state *state)
{
	u32 offset;

	if (tdox_id >= UC_MAX_TDOX_SESSIONS)
		return -EINVAL;

	offset = tdox_state_arr_offset + tdox_id;
	*state = PP_REG_RD8(UC_SSRAM(EGRESS, offset));

	return 0;
}

s32 uc_nf_set(enum pp_nf_type nf, u16 pid, u16 qos_port, u16 tx_queue,
	      u16 dflt_hif)
{
	u32 uc_port;
	s32 ret;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR)))
		return -EPERM;

	switch (nf) {
	case PP_NF_MULTICAST:
		uc_port = UC_MCAST_PORT;
		/* set the mcast tx queue */
		ret = uc_egr_mbox_cmd_send(UC_CMD_MCAST_QUEUE, tx_queue, NULL,
					   0, NULL, 0);
		if (unlikely(ret))
			return ret;
		/* set the mcast pid */
		ret = uc_egr_mbox_cmd_send(UC_CMD_MCAST_PID, pid, NULL, 0,
					   NULL, 0);
		if (unlikely(ret))
			return ret;
		break;
	case PP_NF_IPSEC:
		uc_port = UC_IPSEC_PORT;
		/* set the ipsec tx queue */
		ret = uc_egr_mbox_cmd_send(UC_CMD_IPSEC_QUEUE, tx_queue, NULL,
					   0, NULL, 0);
		if (unlikely(ret))
			return ret;
		/* set the ipsec pid */
		ret = uc_egr_mbox_cmd_send(UC_CMD_IPSEC_PID, pid, NULL, 0,
					   NULL, 0);
		if (unlikely(ret))
			return ret;
		break;
	case PP_NF_REASSEMBLY:
		uc_port = UC_REASS_PORT;

		ret = __nf_reass_set(tx_queue, dflt_hif);
		if (unlikely(ret)) {
			pr_err("__nf_reass_set failed, ret %d\n", ret);
			return ret;
		}

		break;
	case PP_NF_TURBODOX:
		uc_port = UC_TDOX_PORT;
		ret = __nf_tdox_set();
		if (unlikely(ret)) {
			pr_err("__nf_tdox_set failed, ret %d\n", ret);
			return ret;
		}
		break;
	case PP_NF_FRAGMENTER:
		uc_port = UC_FRAG_PORT;
		break;
	default:
		pr_err("nf type is not supported\n");
		return -EINVAL;
	}

	if (pp_silicon_step_get() != PP_SSTEP_A)
		uc_port += UC_B_TXMGR_PORT_OFF;

	return __writer_port_enable(uc_port, qos_port);
}

/**
 * @brief set the uc private accelerators
 * @param uc_is_egr select the uc cluster
 * @param cfg cpus init parameters
 * @param rst_vec reset vector array
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_priv_acc_set(bool uc_is_egr, struct uc_cpu_params *cfg,
			     u32 *rst_vec)
{
	u32 cid;

	for (cid = 0; cid < cfg->max_cpu; cid++) {
		uc_ccu_enable_set(uc_is_egr, cid, true);
		__uc_profile_set(uc_is_egr, cid, cfg->cpu_prof[cid]);
		__uc_rst_vec_set(uc_is_egr, cid, rst_vec);
		uc_run_set(uc_is_egr, cid, true);
		/* in this stage, the uc is run and execute the
		 * cluster_init() and app_init() routines
		 */
		/* verify the uc is up and running */
		if (unlikely(!__uc_is_uc_init_done(uc_is_egr, cid)))
			return -EBUSY;
	}

	return 0;
}

/**
 * @brief verify the uc version major and minor numbers
 * @param uc_is_egr select the uc cluster
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_ver_verify(bool uc_is_egr)
{
	u32 ver;
	u8 major, minor;
	s32 ret;

	/* get fw version */
	ret = uc_ver_get(uc_is_egr, &ver);
	if (unlikely(ret))
		return ret;

	if (uc_is_egr) {
		major = UC_VER_MAJOR(EGRESS);
		minor = UC_VER_MINOR(EGRESS);
	} else {
		major = UC_VER_MAJOR(INGRESS);
		minor = UC_VER_MINOR(INGRESS);
	}
	if ((major != ((ver >> 16) & U8_MAX)) ||
	    (minor != ((ver >> 8)  & U8_MAX))) {
		pr_err("uc version mismatch error, major[%u] minor[%u]\n",
		       ((ver >> 16) & U8_MAX), ((ver >> 8)  & U8_MAX));
		return -EACCES;
	}

	pr_info("pp %s uc version: MAJOR[%u], MINOR[%u], BUILD[%u]\n",
		uc_is_egr ? "egress " : "ingress", (ver >> 16) & U8_MAX,
		(ver >> 8) & U8_MAX, ver & U8_MAX);

	return 0;
}

s32 __uc_egr_info_set(void)
{
	struct eg_uc_init_info info;
	struct pp_rx_dma_init_params cfg;
	u8 idx;

	memset(&info, 0, sizeof(info));
	info.logger_buff    = __convert_36b_addr_to_32b(logger_buff_phys);
	info.logger_buff_sz = logger_buff_sz;
	info.chk_base       = UC_PP_ADDR(chk_base_addr);
	info.chk_ctrs_cache_base = UC_PP_ADDR(chk_ctrs_cache_base_addr);
	info.chk_dsi_ddr_base =
		__convert_36b_addr_to_32b(chk_dsi_phys_base_get());
	info.cls_base       = UC_PP_ADDR(cls_base_addr);

	rx_dma_config_get(&cfg);
	for (idx = 0 ; idx < RX_DMA_MAX_POOLS ; idx++)
		info.buffer_size[idx] = cfg.buffer_size[idx];

	return uc_egr_mbox_cmd_send(UC_CMD_INIT, 0, (const void *)&info,
				    sizeof(info), NULL, 0);
}

/**
 * @brief initialized the egress uc cluster
 * @param cfg egress uc init parameters
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_egr_init(struct uc_egr_init_params *cfg)
{
	s32 ret = 0;
	u32 rst_vec[UC_BOOT_RST_VEC_WORDS] = { 0 };

	pr_debug("pp uc egress max-cpu: %u\n", cfg->cpus.max_cpu);

	/* Save the tx manager address for NF configuration */
	txm_cred_base = (u32)cfg->txm_cred_base;

	/* copy the elf sections to the cluster SRAM */
	if (pp_silicon_step_get() == PP_SSTEP_A)
		ret = __uc_download_image(UC_A_EGR_ELF, rst_vec);
	else
		ret = __uc_download_image(UC_B_EGR_ELF, rst_vec);
	if (unlikely(ret))
		return ret;

	/* configure the shared accelerators */
	ret = __uc_shrd_acc_set(cfg, cfg->cpus.max_cpu, UC_IS_EGR);
	if (unlikely(ret))
		return ret;

	/* configure the private accelerators */
	ret = __uc_priv_acc_set(UC_IS_EGR, &cfg->cpus, rst_vec);
	if (unlikely(ret))
		return ret;

	/* init the uc egress mbox */
	ret = uc_egr_mbox_init();
	if (unlikely(ret))
		return ret;

	/* verify fw version */
	ret = __uc_ver_verify(UC_IS_EGR);
	if (unlikely(ret))
		return ret;

	ret = __uc_egr_info_set();
	if (unlikely(ret))
		return ret;

	pr_debug("pp uc egress init done\n");
	return 0;
}

/**
 * @brief exit the uc egress cluster
 */
static void __uc_egr_exit(void)
{
	uc_egr_mbox_exit();
	pr_debug("done\n");
}

/**
 * @brief initialized the ingress uc cluster
 * @param cfg ingress uc init parameters
 * @return s32 0 on success, error code otherwise
 */
static s32 __uc_ing_init(struct uc_ing_init_params *cfg)
{
	s32 ret = 0;
	u32 rst_vec[UC_BOOT_RST_VEC_WORDS] = { 0 };

	pr_debug("pp uc ingress max-cpu: %u\n", cfg->cpus.max_cpu);

	/* copy the elf sections to SRAM */
	if (pp_silicon_step_get() == PP_SSTEP_A)
		ret = __uc_download_image(UC_A_ING_ELF, rst_vec);
	else
		ret = __uc_download_image(UC_B_ING_ELF, rst_vec);
	if (unlikely(ret))
		return ret;

	/* configure the shared accelerators */
	ret = __uc_shrd_acc_set(NULL, cfg->cpus.max_cpu, UC_IS_ING);
	if (unlikely(ret))
		return ret;

	/* configure the private accelerators */
	ret = __uc_priv_acc_set(UC_IS_ING, &cfg->cpus, rst_vec);
	if (unlikely(ret))
		return ret;

	/* vriey fw version */
	ret = __uc_ver_verify(UC_IS_ING);
	if (unlikely(ret))
		return ret;

	pr_debug("pp uc ingress init done\n");
	return 0;
}

/**
 * @brief exit the uc ingress cluster
 */
static void __uc_ing_exit(void)
{
	pr_debug("done\n");
}

#if IS_ENABLED(PP_POWER_ON_REGS_TEST)
s32 uc_dbg_clk_set(struct uc_init_params *init_param, bool en)
{
	u32 cid;

	if (!init_param->ing.valid || !init_param->egr.valid)
		return -EINVAL;

	uc_egr_base_addr = init_param->egr.uc_base;
	uc_ing_base_addr = init_param->ing.uc_base;

	for (cid = 0; cid < init_param->egr.cpus.max_cpu; cid++) {
		__uc_cpu_valid_set(UC_IS_EGR, cid, true);
		uc_ccu_enable_set(UC_IS_EGR, cid, en);
		__uc_cpu_valid_set(UC_IS_EGR, cid, false);
	}
	for (cid = 0; cid < init_param->ing.cpus.max_cpu; cid++) {
		__uc_cpu_valid_set(UC_IS_ING, cid, true);
		uc_ccu_enable_set(UC_IS_ING, cid, en);
		__uc_cpu_valid_set(UC_IS_EGR, cid, false);
	}

	return 0;
}
#endif

s32 uc_log_buff_info_get(void **buff, size_t *sz)
{
	if (unlikely(ptr_is_null(buff) || ptr_is_null(sz)))
		return -EINVAL;

	*buff = logger_buff_virt;
	*sz   = logger_buff_sz;
	dma_map_single(pp_dev_get(), *buff, *sz, DMA_FROM_DEVICE);

	return 0;
}

s32 uc_log_reset(void)
{
	return uc_egr_mbox_cmd_send(UC_CMD_LOGGER_RESET, 0, NULL, 0, NULL, 0);
}

s32 uc_log_level_set(enum uc_log_level level)
{
	return uc_egr_mbox_cmd_send(UC_CMD_LOGGER_LEVEL_SET, level, NULL, 0,
				    NULL, 0);
}

static void __uc_logger_init(void)
{
	void *virt;
	dma_addr_t phys;
	size_t sz;

	sz   = UC_LOGGER_BUFF_SZ;
	virt = pp_dma_alloc(sz, GFP_KERNEL, &phys, false);
	if (unlikely(!virt)) {
		pr_err("Could not allocate %zu bytes for egress uc logger\n",
			sz);
		return;
	}

	memset(virt, 0, sz);
	logger_buff_phys = phys;
	logger_buff_virt = virt;
	logger_buff_sz   = sz;
}

static void __uc_logger_destroy(void)
{
	if (logger_buff_virt)
		pp_dma_free(logger_buff_sz, logger_buff_virt,
			    &logger_buff_phys, false);
}

s32 uc_init(struct uc_init_params *init_param)
{
	s32 ret = 0;

	if (!init_param->ing.valid || !init_param->egr.valid)
		return -EINVAL;

	__uc_logger_init();

	uc_egr_base_addr = init_param->egr.uc_base;
	uc_ing_base_addr = init_param->ing.uc_base;
	chk_base_addr    = init_param->egr.chk_base;
	chk_ctrs_cache_base_addr = init_param->egr.chk_ctrs_cache_base_addr;
	rxdma_base_addr  = init_param->egr.rxdma_base;
	cls_base_addr    = init_param->egr.cls_base;

	if (init_param->egr.cpus.max_cpu) {
		ret = __uc_egr_init(&init_param->egr);
		if (unlikely(ret))
			return ret;
	}

	if (init_param->ing.cpus.max_cpu) {
		ret = __uc_ing_init(&init_param->ing);
		if (unlikely(ret))
			return ret;
	}

	/* debugfs */
	ret = uc_dbg_init(init_param->dbgfs);
	if (unlikely(ret))
		return ret;

	pr_debug("done\n");
	return 0;
}

void uc_exit(void)
{
	__uc_egr_exit();
	__uc_ing_exit();
	uc_dbg_clean();
	__uc_logger_destroy();
}
