/*
 * Description: PP micro-controller internal definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef __PP_UC_INTERNAL_H__
#define __PP_UC_INTERNAL_H__

#include <linux/types.h>
#include "uc_regs.h"

#define UC_VER_MAJOR(t)               (t ## _VER_MAJOR)
#define UC_VER_MINOR(t)               (t ## _VER_MINOR)

/* Ingress configuration. Must be aligned with the gdb structure in uc */
#define INGRESS_GDB_SSRAM_OFF                    (0x4000)
#define INGRESS_VER_SSRAM_OFF                    (INGRESS_GDB_SSRAM_OFF)
#define INGRESS_HOST_INFO_PORT_SSRAM_OFF         (INGRESS_GDB_SSRAM_OFF + 0x4)
#define INGRESS_HOST_INFO_QUEUE_SSRAM_OFF        (INGRESS_GDB_SSRAM_OFF + 0x8)
#define INGRESS_HOST_INFO_BASE_POLICY_SSRAM_OFF  (INGRESS_GDB_SSRAM_OFF + 0xc)
#define INGRESS_HOST_INFO_POLICY_BMAP_SSRAM_OFF  (INGRESS_GDB_SSRAM_OFF + 0x10)

/* UC Egress SW implementation for A-STEP */
/* CPU 3 DCCM address for the port credit inc address */
#define UC_SW_WRITER_TX_PORT_CRED(p)  (UC_DCCM(EGRESS, 3, (p) << 2))
/* CPU 3 DCCM address for the port ring address */
#define UC_SW_WRITER_TX_PORT_RING(p)  (0x50 + UC_DCCM(EGRESS, 3, (p) << 5))
/* DCCM address for triggering the command */
#define UC_SW_WRITER_MBOX_CH_TRIGGER  UC_SW_WRITER_TX_PORT_CRED(16)

/**
 * @brief for B-step the tx manager uc ports starts from port 16
 */
#define UC_B_TXMGR_PORT_OFF 4

#define UC_TASK_REGS_NUM    32U

/**
 * @enum uc_egr_port
 * @brief uc egress port type
 */
enum uc_egr_port {
	/*! mailbox port is valid only for B-step */
	UC_MBOX_PORT = 0,
	/*! tx manager ports starting from port 0 */
	/*! in B-step required UC_B_TXMGR_PORT_OFF offset */
	UC_MCAST_PORT = 0,
	UC_REASS_PORT,
	UC_FRAG_PORT,
	UC_TDOX_PORT,
	UC_IPSEC_PORT,
	UC_EGR_MAX_PORT,
};

/**
 * @brief init the uc egress mailbox module
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_mbox_init(void);

/**
 * @brief exit the uc egress mailbox module
 */
void uc_egr_mbox_exit(void);

/**
 * @brief Get egress writter port enable
 * @param port port id
 * @param en
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_writer_port_en_get(u32 port, bool *en);

/**
 * @brief Get the writer port and queue mapping
 *        for a specific CPU
 * @param cid cpu id
 * @param port port id
 * @param queue the mapped queue
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_writer_map_get(u32 cid, u32 port, u32 *queue);

/**
 * @brief Get the writer credits for a the queue
 *        which is mapped to 'port' on CPU 'cid'
 * @param cid cpu id
 * @param port port id
 * @param credit the credits count
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_writer_queue_credit_get(u32 cid, u32 port, u32 *credit);

/**
 * @brief Get the writer credits for a specific port
 * @param port port id
 * @param credit the credits count
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_writer_port_credit_get(u32 port, u32 *credit);

/**
 * @brief Get the program counter register of CPU's task
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param tid task index
 * @param pc the program counter
 * @return s32 0 on success, error code otherwise
 */
s32 uc_task_pc_get(bool uc_is_egr, u32 cid, u32 tid, u32 *pc);

/**
 * @brief Get the status32 register of CPU's task
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param tid task index
 * @param status the status32
 * @return s32 0 on success, error code otherwise
 */
s32 uc_task_status32_get(bool uc_is_egr, u32 cid, u32 tid, u32 *status);

/**
 * @brief Get the ready to run bit of CPU's task
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param tid task index
 * @param ready the ready bit
 * @return s32 0 on success, error code otherwise
 */
s32 uc_task_ready_get(bool uc_is_egr, u32 cid, u32 tid, bool *ready);

/**
 * @brief Get the status and queue of POPA for a CPU's task
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param tid task index
 * @param active if the task is waiting for POPA
 * @param queue for which queue the task is waiting
 * @return s32 0 on success, error code otherwise
 */
s32 uc_task_popa_active_get(bool uc_is_egr, u32 cid, u32 tid, bool *active,
			    u32 *queue);

/**
 * @brief Get CPU's task registers
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param tid task index
 * @param regs registers array
 * @param num number of registers to get, 0 till 'num'
 * @return s32 0 on success, error code otherwise
 */
s32 uc_task_regs_get(bool uc_is_egr, u32 cid, u32 tid, u32 *regs, u32 num);

/**
 * @brief check if cpu is valid
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @return bool true if valid, fasle otherwise
 */
bool uc_is_cpu_valid(bool uc_is_egr, u32 cid);

/**
 * @brief run/halt cpu
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param enable run/halt
 */
void uc_run_set(bool uc_is_egr, u32 cid, bool enable);

/**
 * @brief enable/disable the cpu in ccu (take the CPU out of
 *        reset and turn on the clock)
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @param enable enable/disable
 */
void uc_ccu_enable_set(bool uc_is_egr, u32 cid, bool enable);

/**
 * @brief get the ccu interrupt request register
 * @param uc_is_egr select the uc cluster
 * @param ier_bitmap interrupts bitmap
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ccu_irr_get(bool uc_is_egr, u32 *irr_bitmap);

/**
 * @brief get the maximum supported cpus in cluster
 * @param uc_is_egr select the uc cluster
 * @param max_cpus max supported cpus
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ccu_maxcpus_get(bool uc_is_egr, u8 *max_cpus);

/**
 * @brief get the cluster bridge fat entry value
 * @param uc_is_egr select the uc cluster
 * @param ent fat entry index
 * @param val fat entry value
 * @param enable fat entry state
 * @return s32 0 on success, error code otherwise
 */
s32 uc_fat_get(bool uc_is_egr, u32 ent, u32 *val, bool *enable);

/**
 * @brief set the cluster bridge fat entry value
 * @param uc_is_egr select the uc cluster
 * @param ent fat entry index
 * @param val fat entry value
 * @param enable enable/disable the entry
 * @return s32 0 on success, error code otherwise
 */
s32 uc_fat_set(bool uc_is_egr, u8 ent, u32 val, bool enable);

/**
 * @brief get the cluster reader interrupt request register
 * @param uc_is_egr select the uc cluster
 * @param irr_bitmap interrupt request register value
 * @return s32 0 on success, error code otherwise
 */
s32 uc_reader_irr_get(bool uc_is_egr, u32 *irr_bitmap);

/**
 * @brief get the uc fw version
 * @param uc_is_egr select the uc cluster
 * @param ver version
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ver_get(bool uc_is_egr, u32 *ver);

/**
 * @brief Get uC logger buffer pointer and size
 * @param buff pointer to get the buffer pointer back
 * @param sz pointer to get the size back
 * @return s32 0 on success, error code otherwise
 */
s32 uc_log_buff_info_get(void **buff, size_t *sz);

/**
 * @brief Reset uC log buffer, this will delete all log messages
 * @return s32 0 on success, error code otherwise
 */
s32 uc_log_reset(void);

/**
 * @brief Set the uC logger level
 * @param level the level to set
 * @return s32 0 on success, error code otherwise
 */
s32 uc_log_level_set(enum uc_log_level level);

/**
 * @brief Read uC auxiliary register, for full register list, see
 *        Figure 3-5 in the ARCv2 Programmer's Reference Manual
 * @note this will read the register from the CPU which
 *       handles the mailbox messages
 * @param reg the register to read
 * @param val pointer to get the value back
 * @return s32 0 on success, error code otherwise
 */
s32 uc_aux_reg_read(u32 reg, u32 *val);

/**
 * @brief Write uC auxiliary register value, for full register list, see
 *        Figure 3-5 in the ARCv2 Programmer's Reference Manual
 * @note this will write to a register in the CPU which
 *       handles the mailbox messages
 * @param reg the register to read
 * @param val the value to write
 * @return s32 0 on success, error code otherwise
 */
s32 uc_aux_reg_write(u32 reg, u32 val);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief uc debug init
 * @return s32 0 on success, error code otherwise
 */
s32 uc_dbg_init(struct dentry *parent);

/**
 * @brief uc debug cleanup
 */
void uc_dbg_clean(void);
#else /* !CONFIG_DEBUG_FS */
	static inline s32 uc_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline void uc_dbg_clean(void)
{
}
#endif /* CONFIG_DEBUG_FS */

#endif /* __PP_UC_INTERNAL_H__ */
