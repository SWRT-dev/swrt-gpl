/*
 * pp_common.h
 * Description:
 * PP bm regs Definitions
 *
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

#ifndef _PP_BMGR_REGS_H_
#define _PP_BMGR_REGS_H_

/*! \def PP_BMGR_MAX_POOLS_IN_POLICY
 *       Max pools in policy
 */
#define PP_BMGR_MAX_POOLS_IN_POLICY					(4)


#define BMGR_OFFSET_OF_POLICY(policy)										((policy) * (int)0x4)
#define BMGR_OFFSET_OF_POOL(pool)											((pool) * (int)0x4)
#define BMGR_OFFSET_OF_GROUP(group)											((group) * (int)0x4)
#define BMGR_OFFSET_OF_POOL_IN_POLICY(pool, policy)							((BMGR_OFFSET_OF_POLICY(policy) * PP_BMGR_MAX_POOLS_IN_POLICY) + BMGR_OFFSET_OF_POOL(pool))
// Regs
#define BMGR_CTRL_REG_ADDR(BMGR_REG_BASE)									(BMGR_REG_BASE + 0x000)
#define BMGR_POOL_MIN_GRNT_MASK_REG_ADDR(BMGR_REG_BASE)						(BMGR_REG_BASE + 0x004)
#define BMGR_POOL_ENABLE_REG_ADDR(BMGR_REG_BASE)							(BMGR_REG_BASE + 0x008)
#define BMGR_POOL_FIFO_RESET_REG_ADDR(BMGR_REG_BASE)						(BMGR_REG_BASE + 0x00C)
#define BMGR_OCPM_BURST_SIZE_REG_ADDR(BMGR_REG_BASE)						(BMGR_REG_BASE + 0x010)
#define BMGR_OCPM_NUM_OF_BURSTS_REG_ADDR(BMGR_REG_BASE)						(BMGR_REG_BASE + 0x014)
#define BMGR_STATUS_REG_ADDR(BMGR_REG_BASE)									(BMGR_REG_BASE + 0x018)
#define BMGR_POOL_SIZE_REG_ADDR(BMGR_REG_BASE, pool)						(BMGR_REG_BASE + 0x020 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_GROUP_AVAILABLE_BUFF_REG_ADDR(BMGR_REG_BASE, group)			(BMGR_REG_BASE + 0x100 + BMGR_OFFSET_OF_GROUP(group))
#define BMGR_GROUP_RESERVED_BUFF_REG_ADDR(BMGR_REG_BASE, group)				(BMGR_REG_BASE + 0x200 + BMGR_OFFSET_OF_GROUP(group))
#define BMGR_POOL_PCU_FIFO_BASE_ADDR_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x400 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_PCU_FIFO_SIZE_REG_ADDR(BMGR_REG_BASE, pool)				(BMGR_REG_BASE + 0x440 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_PCU_FIFO_OCCUPANCY_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x480 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_PCU_FIFO_PROG_EMPTY_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x4C0 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_PCU_FIFO_PROG_FULL_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x500 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_EXT_FIFO_OCC_REG_ADDR(BMGR_REG_BASE, pool)				(BMGR_REG_BASE + 0x5C0 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_EXT_FIFO_BASE_ADDR_LOW_REG_ADDR(BMGR_REG_BASE, pool)		(BMGR_REG_BASE + 0x540 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_EXT_FIFO_BASE_ADDR_HIGH_REG_ADDR(BMGR_REG_BASE, pool)		(BMGR_REG_BASE + 0x580 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_ALLOCATED_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x600 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_POP_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)					(BMGR_REG_BASE + 0x640 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_PUSH_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)				(BMGR_REG_BASE + 0x680 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_DDR_BURST_WRITE_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x6C0 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_DDR_BURST_READ_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)			(BMGR_REG_BASE + 0x700 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_WATERMARK_LOW_THRESHOLD_REG_ADDR(BMGR_REG_BASE, pool)		(BMGR_REG_BASE + 0x740 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POOL_WATERMARK_LOW_COUNTER_REG_ADDR(BMGR_REG_BASE, pool)		(BMGR_REG_BASE + 0x780 + BMGR_OFFSET_OF_POOL(pool))
#define BMGR_POLICY_NULL_COUNTER_REG_ADDR(BMGR_REG_BASE, policy)			(BMGR_REG_BASE + 0x800 + BMGR_OFFSET_OF_POLICY(policy))
// Ram
#define BMGR_POLICY_MAX_ALLOWED_ADDR(BMGR_RAM_BASE, policy)					(BMGR_RAM_BASE + 0x0000 + BMGR_OFFSET_OF_POLICY(policy))
#define BMGR_POLICY_MIN_GUARANTEED_ADDR(BMGR_RAM_BASE, policy)				(BMGR_RAM_BASE + 0x1000 + BMGR_OFFSET_OF_POLICY(policy))
#define BMGR_POLICY_GROUP_ASSOCIATED_ADDR(BMGR_RAM_BASE, policy)			(BMGR_RAM_BASE + 0x2000 + BMGR_OFFSET_OF_POLICY(policy))
#define BMGR_POLICY_POOLS_MAPPING_ADDR(BMGR_RAM_BASE, policy)				(BMGR_RAM_BASE + 0x3000 + BMGR_OFFSET_OF_POLICY(policy))
#define BMGR_POLICY_MAX_ALLOWED_PER_POOL_ADDR(BMGR_RAM_BASE, policy, pool)	(BMGR_RAM_BASE + 0x4000 + BMGR_OFFSET_OF_POLICY(policy) + (pool * 0x1000))
#define BMGR_POLICY_ALLOC_BUFF_COUNTER_ADDR(BMGR_RAM_BASE, policy)			(BMGR_RAM_BASE + 0xA000 + BMGR_OFFSET_OF_POLICY(policy))
#define BMGR_POLICY_ALLOC_BUFF_PER_POOL_COUNTER_ADDR(BMGR_RAM_BASE, pool, policy)	(BMGR_RAM_BASE + 0xB000 + BMGR_OFFSET_OF_POOL_IN_POLICY(pool, policy))

// Data
#define BMGR_DATAPATH_BASE_HOST						(0x18BC0000)
#define BMGR_DATAPATH_BASE						(0x3C0000)

#endif // _PP_BMGR_REGS_H_

