/*
 * pp_regs.h
 * Description: PP registers common definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_REGS_H__
#define __PP_REGS_H__

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/byteorder/generic.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_qos_uc_defs.h"

#define PP_REG_DFLT_RETRIES             1000U
#define PP_REG_MAX_RETRIES              100000000U

/**
 * @brief Defines operations types for PP registers polling
 */
enum pp_poll_op {
	PP_POLL_EQ, /*! poll till register is equal to value     */
	PP_POLL_NE, /*! poll till register is not equal to value */
	PP_POLL_LT, /*! poll till register is lower than value   */
	PP_POLL_GT, /*! poll till register is greater than value */
};

/**
 * @brief Poll on a Packet Processor register bits field. <br>
 *        The polling stops if one of the following apply: <br>
 *          1. reached maximum number of tries <br>
 *          2. (*(addr) & (mask)) >> (ffs(mask) - 1) == val <br>
 *             if the extracted bits from the value in addr is
 *             equal to the specified value
 * @param addr Register address
 * @param mask Mask to apply on the register value
 * @param val value to wait for
 * @param retries number of polling tries
 * @param attempts if non-null ptr is provided, the number of attempts
 *                 the register was polled till expected value was
 *                 seen is returned
 * @param read_val if non-null ptr is provided, the masked
 *                 register value is returned
 * @param op compare operation
 * @return u32 0 on successful poll, non-zero value otherwise.
 */
s32 pp_raw_reg_poll(u64 addr, u32 mask, u32 val, u32 retries,
		    u32 *attempts, u32 *read_val, enum pp_poll_op op);

/**
 * @brief poll on PP register till it is equal to 'val' or
 *        default retries exceeds
 */
#define pp_reg_poll(addr, mask, val) \
	pp_reg_poll_x(addr, mask, val, PP_REG_DFLT_RETRIES)

/**
 * @brief poll on PP register till it is equal to 'val' or
 *        'x' retries exceeds
 */
#define pp_reg_poll_x(addr, mask, val, x) \
	pp_raw_reg_poll(addr, mask, val, x, NULL, NULL, PP_POLL_EQ)

/**
 * @brief poll on PP register till it is not equal to 'val' or
 *        default retries exceeds
 */
#define pp_reg_poll_ne(addr, mask, val) \
	pp_raw_reg_poll(addr, mask, val, PP_REG_DFLT_RETRIES, \
			NULL, NULL, PP_POLL_NE)

/**
 * @brief Defines the PP HW Modules which has registers access from the host
 *        as specified by the DTS
 * @note  Currently used only for printing modules registers writes logs
 */
enum pp_hw_module {
	HW_MOD_INFRA = UC_MOD_CNT,
	HW_MOD_PARSER,
	HW_MOD_RPB,
	HW_MOD_PORT_DIST,
	HW_MOD_CLASSIFER,
	HW_MOD_CHECKER,
	HW_MOD_MODIFIER,
	HW_MOD_RXDMA,
	HW_MOD_BMNGR,
	HW_MOD_QOS,
	HW_MOD_QOS_WRED,
	HW_MOD_EG_UC,
	HW_MOD_IN_UC,
	HW_MOD_CNT
};

struct pp_io_region {
	void        *virt;
	phys_addr_t phys;
	size_t      sz;
	enum pp_hw_module mod_id;
};

s32 pp_region_add(struct pp_io_region *region);
void *pp_phys_to_virt(phys_addr_t addr);
phys_addr_t pp_virt_to_phys(void *addr);
void pp_memcpy(void *dst, void *src, u32 cnt);
u8 pp_reg_rd8(void *addr);
u16 pp_reg_rd16(void *addr);
u32 pp_reg_rd32(void *addr);
void pp_reg_wr32(void *addr, u32 data);
#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
s32 pp_regs_mod_log_en_set(ulong *bmap, s32 n_bits);
s32 pp_regs_mod_log_en_get(ulong *bmap, s32 n_bits);
#endif
s32 pp_regs_init(void);

#define PP_REG_RD_REP(addr, buf, cnt)                           \
	pp_memcpy((void *)(ulong)buf, (void *)(ulong)addr, cnt)

/**
 * @brief Read 32bit data from Packet processor register
 * @param addr register's virtual address
 */
#ifdef CONFIG_PPV4_LGM
#define PP_REG_RD32(addr)                             \
	le32_to_cpu(pp_reg_rd32((void *)(ulong)addr))
#else
#define PP_REG_RD32(addr)                             \
	pp_reg_rd32((void *)(ulong)addr)
#endif

/**
 * @brief Read 16bit data from Packet processor register
 * @param addr register's virtual address
 */
#define PP_REG_RD16(addr)                             \
	le16_to_cpu(pp_reg_rd16((void *)(ulong)addr))

/**
 * @brief Read 8bit data from Packet processor register
 * @param addr register's virtual address
 */
#define PP_REG_RD8(addr)                \
	pp_reg_rd8((void *)(ulong)addr)

/**
 * @brief Write 32bit data to Packet Processor register without
 *        le swapping
 * @param addr register's virtual address
 * @param data data to write
 */
#define PP_REG_WR32_NO_SWAP(addr, data) \
	pp_reg_wr32((void *)(ulong)addr, data)

/**
 * @brief Write 32bit data to Packet Processor register
 * @param addr register's virtual address
 * @param data data to write
 */
#ifdef CONFIG_PPV4_LGM
#define PP_REG_WR32(addr, data)                        \
	PP_REG_WR32_NO_SWAP((addr), cpu_to_le32(data))
#else
#define PP_REG_WR32(addr, data)                        \
	PP_REG_WR32_NO_SWAP((addr), data)
#endif

#define PP_REG_WR32_INC(addr, data)                    \
do {                                                   \
	PP_REG_WR32(addr, data);                       \
	addr += 4;                                     \
} while (0)

#endif /* __PP_REGS_H__ */
