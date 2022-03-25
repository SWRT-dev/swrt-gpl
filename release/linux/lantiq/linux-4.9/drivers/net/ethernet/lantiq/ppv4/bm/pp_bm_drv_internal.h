/*
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

#ifndef _BMGR_DRV_INTERNAL_H_
#define _BMGR_DRV_INTERNAL_H_

#include <linux/debugfs.h>
#include "pp_bm_drv.h"

/*! \def BMGR_DRIVER_VERSION */
#define BMGR_DRIVER_VERSION			"1.0.0"

/*! \def BMGR_MIN_POOL_BUFFER_SIZE
 *       Minimum buffer size in configured pool
 */
#define BMGR_MIN_POOL_BUFFER_SIZE		(64)

/*! \def BMGR_START_PCU_SRAM_ADDR
 *       Start PCU address in SRAM. Used in confiure pool
 */
#define BMGR_START_PCU_FIFO_SRAM_ADDR		(0)

/*! \def BMGR_DEFAULT_PCU_FIFO_SIZE
 *       PCU fifo size
 */
#define BMGR_DEFAULT_PCU_FIFO_SIZE		(0x400)

/*! \def BMGR_DEFAULT_PCU_FIFO_LOW_THRESHOLD
 *       PCU fifo low threshold
 */
#define BMGR_DEFAULT_PCU_FIFO_LOW_THRESHOLD	(0x100)

/*! \def BMGR_DEFAULT_PCU_FIFO_HIGH_THRESHOLD
 *       PCU fifo high threshold
 */
#define BMGR_DEFAULT_PCU_FIFO_HIGH_THRESHOLD	(0x300)

/*! \def BMGR_DEFAULT_WATERMARK_LOW_THRESHOLD
 *       Watermark low threshold
 */
#define BMGR_DEFAULT_WATERMARK_LOW_THRESHOLD	(0x200)

/**********************************
 **********************************
 *********** STRUCTURE ************
 **********************************
 **********************************/

/**************************************************************************
 *! \enum	bmgr_policy_pools_priority_e
 **************************************************************************
 *
 * \brief enum to describe the pool's priority
 *
 **************************************************************************/
enum bmgr_policy_pools_priority_e {
	bmgr_policy_pool_priority_high,		//!< High priority
	bmgr_policy_pool_priority_mid_high,	//!< Mid-High priority
	bmgr_policy_pool_priority_mid_low,	//!< Mid-Low priority
	bmgr_policy_pool_priority_low,		//!< Low priority
	bmgr_policy_pool_priority_max		//!< Last priority
};

/**************************************************************************
 *! \struct	bmgr_pool_db_entry
 **************************************************************************
 *
 * \brief This structure holds the pool database
 *
 **************************************************************************/
struct bmgr_pool_db_entry {
	//!< Pool params
	struct bmgr_pool_params	pool_params;
	//!< Number of allocated buffers
	u32						num_allocated_buffers;
	//!< Number of deallocated buffers
	u32						num_deallocated_buffers;
	//!< Is entry in used
	u8						is_busy;
	//!< Pointers table to be used in HW
	void					*internal_pointers_tables;
};

/**************************************************************************
 *! \struct	bmgr_group_db_entry
 **************************************************************************
 *
 * \brief This structure holds the group database
 *
 **************************************************************************/
struct bmgr_group_db_entry {
	//!< available buffers in group
	u32	available_buffers;
	//!< reserved buffers in this group
	u32	reserved_buffers;
	//!< Pools in policy (if set, pool is part of this group)
	u8	pools[PP_BMGR_MAX_POOLS_IN_GROUP];
	//!< Number of pools in group
	u8	num_pools_in_group;
};

/**************************************************************************
 *! \struct	bmgr_policy_db_entry
 **************************************************************************
 *
 * \brief This structure holds the policy database
 *
 **************************************************************************/
struct bmgr_policy_db_entry {
	//!< Policy params
	struct bmgr_policy_params	policy_params;
	//!< Number of allocated buffers
	u32		num_allocated_buffers;
	//!< Number of deallocated buffers
	u32		num_deallocated_buffers;
	//!< Is entry in used
	u8		is_busy;
};

/**************************************************************************
 *! \struct	bmgr_driver_db
 **************************************************************************
 *
 * \brief This structure holds the buffer manager database
 *
 **************************************************************************/
struct bmgr_driver_db {
	//!< Pools information
	struct bmgr_pool_db_entry	pools[PP_BMGR_MAX_POOLS];
	//!< Groups information
	struct bmgr_group_db_entry	groups[PP_BMGR_MAX_GROUPS];
	//!< Policies information
	struct bmgr_policy_db_entry	policies[PP_BMGR_MAX_POLICIES];

	// general counters
	//!< Number of active pools
	u32				num_pools;
	//!< Number of active groups
	u32				num_groups;
	//!< Number of active policies
	u32				num_policies;

	// per project constants
	//!< Max number of pools
	u32				max_pools;
	//!< Max number of groups
	u32				max_groups;
	//!< Max number of policies
	u32				max_policies;

	//!< spinlock
	spinlock_t		db_lock;
};

/**************************************************************************
 *! \struct	bmgr_driver_db
 **************************************************************************
 *
 * \brief This structure holds the buffer manager database
 *
 **************************************************************************/
struct bmgr_debugfs_info {
	//!< Debugfs dir
	struct dentry	*dir;
	u32		virt2phyoff;
	u32		ddrvirt2phyoff;
	u32		fpga_offset;
};

/**************************************************************************
 *! \struct	bmgr_driver_private
 **************************************************************************
 *
 * \brief This struct defines the driver's private data
 *
 **************************************************************************/
struct bmgr_driver_private {
	//!< Platform device pointer
	struct platform_device		*pdev;
	//!< Is driver enabled
	int				enabled;
	//!< Platform device DB
	struct bmgr_driver_db		driver_db;
	//!< Debugfs info
	struct bmgr_debugfs_info	debugfs_info;
};

s32 bmgr_test_dma(u32 num_bytes);
void print_hw_stats(void);

int bm_dbg_dev_init(struct platform_device *pdev);
void bm_dbg_dev_clean(struct platform_device *pdev);
int bm_dbg_module_init(void);
void bm_dbg_module_clean(void);

#endif /* _BMGR_DRV_INTERNAL_H_ */
