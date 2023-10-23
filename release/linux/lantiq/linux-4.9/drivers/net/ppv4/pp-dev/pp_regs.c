/*
 * pp_regs.c
 * Description: PP Registers Translation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/dma-mapping.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include "pp_regs.h"
#include "infra.h" /* For checking the Silicon step */

/**
 * @brief Add function name and line number for all pr_* prints
 */
#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_REGS]:%s:%d: " fmt, __func__, __LINE__
#endif

bool simulation_dbg;
ulong mod_reg_log_en[BITS_TO_LONGS(HW_MOD_CNT)];
static const char * const mod_str[HW_MOD_CNT + 1] = {
	[HW_MOD_INFRA]     = "Infra    : ",
	[HW_MOD_PARSER]    = "Parser   : ",
	[HW_MOD_RPB]       = "Rpb      : ",
	[HW_MOD_PORT_DIST] = "Port_dist: ",
	[HW_MOD_CLASSIFER] = "Classifer: ",
	[HW_MOD_CHECKER]   = "Checker  : ",
	[HW_MOD_MODIFIER]  = "Modifier : ",
	[HW_MOD_RXDMA]     = "Rxdma    : ",
	[HW_MOD_BMNGR]     = "Bmngr    : ",
	[HW_MOD_QOS]       = "Qos      : ",
	[HW_MOD_QOS]       = "Qos WRED : ",
	[HW_MOD_EG_UC]     = "Eg_uc    : ",
	[HW_MOD_IN_UC]     = "In_uc    : ",
	[HW_MOD_CNT]       = ""
};

s32 pp_raw_reg_poll(u64 addr, u32 mask, u32 val, u32 retries,
		    u32 *attempts, u32 *read_val, enum pp_poll_op op)
{
	u32 reg_val = 0;
	u32 fld_val = 0;
	s32 ret = 0;

	if (!mask || !retries)
		return -EINVAL;

	/* Always success in case of simulation environment */
	if (simulation_dbg)
		goto done;

	retries = min(retries, PP_REG_MAX_RETRIES);

	do {
		reg_val = PP_REG_RD32((unsigned long)addr);
		fld_val = PP_FIELD_GET(mask, reg_val);
		pr_debug("reg %#x, expected val %#x, actual val %#x\n",
			 reg_val, val, fld_val);

		if (op == PP_POLL_EQ && fld_val == val)
			goto done;
		if (op == PP_POLL_NE && fld_val != val)
			goto done;
		if (op == PP_POLL_LT && fld_val < val)
			goto done;
		if (op == PP_POLL_GT && fld_val > val)
			goto done;

		if (attempts)
			(*attempts)++;

	} while (--retries);

	ret = -EPERM;
done:
	if (read_val)
		*read_val = reg_val & mask;

	return ret;
}

/* TBD - reduce back to 30 once no need for ddr allocations bookeeping */
#define PPV4_MAX_IO_REGIONS (500)

static struct pp_io_region regions[PPV4_MAX_IO_REGIONS];
static u32                 regions_cnt;

static int __init simics_env(char *str)
{
	pr_info("Simics Simulation Enabled\n");
	simulation_dbg = true;

	return 0;
}

early_param("simics", simics_env);

s32 pp_region_add(struct pp_io_region *region)
{
	if (!region) {
		pr_err("Null Region\n");
		return -EINVAL;
	}

	if (regions_cnt == PPV4_MAX_IO_REGIONS) {
		pr_err("Regions count exceeds maximum of %u\n",
		       PPV4_MAX_IO_REGIONS);
		return -ENOMEM;
	}

	memcpy(&regions[regions_cnt], region, sizeof(*region));
	regions_cnt++;

	return 0;
}

void *pp_phys_to_virt(phys_addr_t addr)
{
	u32 idx;
	unsigned long offset;

	for (idx = 0 ; idx < regions_cnt ; idx++) {
		if ((addr >= regions[idx].phys) &&
		    (addr < (regions[idx].phys + regions[idx].sz))) {
			offset = addr - regions[idx].phys;
			return (void *)((unsigned long)regions[idx].virt +
					offset);
		}
	}

	return NULL;
}

phys_addr_t pp_virt_to_phys(void *addr)
{
	u32 idx;
	unsigned long offset;

	for (idx = 0 ; idx < regions_cnt ; idx++) {
		if ((addr >= regions[idx].virt) &&
		    (addr < (void *)((unsigned long)regions[idx].virt +
				     regions[idx].sz))) {
			offset = (unsigned long)addr -
				(unsigned long)regions[idx].virt;
			return regions[idx].phys + offset;
		}
	}

	return 0;
}

inline void pp_reg_wr32(void *addr, u32 data)
{
	enum pp_hw_module mod = HW_MOD_CNT;
	struct pp_io_region *region;
	u32 idx;

	if (!IS_ENABLED(CONFIG_PPV4_HW_MOD_REGS_LOGS))
		goto do_print;

	for (idx = 0; idx < regions_cnt; idx++) {
		region = &regions[idx];
		if (addr >= region->virt &&
		    addr < (region->virt + region->sz)) {
			mod = region->mod_id;
			break;
		}
	}

do_print:
#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
	if (mod == HW_MOD_CNT || bitmap_empty(mod_reg_log_en, HW_MOD_CNT) ||
	    test_bit(mod, mod_reg_log_en))
		pr_info("%s%#lx: %#x\n", mod_str[mod],
			(unsigned long)pp_virt_to_phys(addr), data);
#endif

	__raw_writel(data, addr);
}

inline u32 pp_reg_rd32(void *addr)
{
	u32 reg = 0;

	pr_debug("read %#lx\n",
		   (unsigned long)pp_virt_to_phys(addr));

	reg = __raw_readl(addr);

	pr_debug("%#lx: %#x\n",
		   (unsigned long)pp_virt_to_phys(addr), reg);
	return reg;
}

inline u16 pp_reg_rd16(void *addr)
{
	u16 reg = 0;

	pr_debug("read %#lx\n", (unsigned long)pp_virt_to_phys(addr));

	reg = __raw_readw(addr);

	pr_debug("%#lx: %#x\n", (unsigned long)pp_virt_to_phys(addr), reg);
	return reg;
}

inline u8 pp_reg_rd8(void *addr)
{
	u8 reg = 0;

	pr_debug("read %#lx\n", (unsigned long)pp_virt_to_phys(addr));

	reg = __raw_readb(addr);

	pr_debug("%#lx: %#x\n", (unsigned long)pp_virt_to_phys(addr), reg);
	return reg;
}

/**
 * @brief Read 'cnt' from Packet processor registers
 * @param src register's virtual address
 * @param dst buffer to save the data read
 * @param cnt number of bytes to read
 */
inline void pp_memcpy(void *dst, void *src, u32 cnt)
{
	u32 byte_reminder = cnt % 4;
	u32 word_cnt = cnt / 4;
	u32 word_idx, rem_idx;
	u32 *buf = (u32 *)dst;
	u32 byte_offset;
	u8 *byte_buf;

	for (word_idx = 0 ; word_idx < word_cnt ; word_idx++) {
		*buf = pp_reg_rd32((void *)((unsigned long)src +
					    (word_idx * 4)));
		buf++;
	}
	byte_buf = (u8 *)buf;

	for (rem_idx = 0 ; rem_idx < byte_reminder ; rem_idx++) {
		byte_offset = (word_cnt * 4) + rem_idx;
		*byte_buf = pp_reg_rd8((void *)((unsigned long)src +
						byte_offset));
		byte_buf++;
	}
}

void *pp_dma_alloc(size_t size, gfp_t flag, dma_addr_t *dma_handle, bool ioc)
{
	void *addr;
#ifndef CONFIG_PPV4_LGM
	struct pp_io_region region;
#endif

	/* For A-step and FLM (DMA mask 32) the default is none IOC memory
	 * For B-step (DMA mask 64) the default is IOC so we remap later
	 */
	addr = dmam_alloc_coherent(pp_dev_get(), size, dma_handle,
				   flag | __GFP_ZERO);
	if (!addr)
		return NULL;

#ifdef CONFIG_PPV4_LGM
	/* Mapping is done for B-step */
	if (!ioc && pp_silicon_step_get() == PP_SSTEP_B) {
		if (*dma_handle & BIT_ULL(34)) {
			*dma_handle &= ~BIT_ULL(34);
			*dma_handle |= BIT_ULL(32) | BIT_ULL(33) | BIT_ULL(35);
		} else {
			*dma_handle |= BIT_ULL(34) | BIT_ULL(35);
		}
	}
#else
	/* For FLM make sure to store original virtual address */
	region.virt = addr;
	region.phys = *dma_handle;
	region.sz = size;

	pp_region_add(&region);

	/* Overwrite virtual address */
	addr = phys_to_virt(*dma_handle);
#endif

	return addr;
}

void pp_dma_free(size_t size, void *address, dma_addr_t *dma_handle, bool ioc)
{
	void *virt_addr;

#ifdef CONFIG_PPV4_LGM
	if (!ioc && pp_silicon_step_get() == PP_SSTEP_B) {
		/* Reverse mapping */
		if (*dma_handle & BIT_ULL(34) &&
		    *dma_handle & BIT_ULL(35)) {
			*dma_handle &= ~BIT_ULL(34);
			*dma_handle &= ~BIT_ULL(35);
		} else if (*dma_handle & BIT_ULL(32) &&
			   *dma_handle & BIT_ULL(33) &&
			   *dma_handle & BIT_ULL(35)) {
			*dma_handle &= ~BIT_ULL(32);
			*dma_handle &= ~BIT_ULL(33);
			*dma_handle &= ~BIT_ULL(35);
			*dma_handle |= BIT_ULL(34);
		}
	}
	virt_addr = address;
#else
	/* In FLM use stored virtual address
	 * rather than the user's address
	 */
	virt_addr = pp_phys_to_virt(*dma_handle);
#endif
	dmam_free_coherent(pp_dev_get(), size, virt_addr, *dma_handle);
}

#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
s32 pp_regs_mod_log_en_set(ulong *bmap, s32 n_bits)
{
	if (ptr_is_null(bmap) || !n_bits)
		return -EINVAL;

	bitmap_copy(mod_reg_log_en, bmap, min_t(s32, HW_MOD_CNT, n_bits));
	return 0;
}

s32 pp_regs_mod_log_en_get(ulong *bmap, s32 n_bits)
{
	if (ptr_is_null(bmap) || !n_bits)
		return -EINVAL;

	bitmap_copy(bmap, mod_reg_log_en, min_t(s32, HW_MOD_CNT, n_bits));
	return 0;
}
#endif

s32 pp_regs_init(void)
{
#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
	ulong bmap = 0;

	bmap = CONFIG_PPV4_HW_MOD_REGS_LOGS_EN;
	bitmap_or(mod_reg_log_en, mod_reg_log_en, &bmap, HW_MOD_CNT);
	pr_info("regs init done, mod_reg_log_en %*pbl\n", HW_MOD_CNT,
		mod_reg_log_en);
#endif

	return 0;
}
