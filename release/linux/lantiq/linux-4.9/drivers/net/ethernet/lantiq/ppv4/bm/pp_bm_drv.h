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

#ifndef _BMGR_H_
#define _BMGR_H_

/**********************************
 **********************************
 ************ DEFINES *************
 **********************************
 **********************************/

/*! \def PP_BMGR_MAX_POOLS
 *       Max supported pools. Real max defined in the DTS
 */
#define PP_BMGR_MAX_POOLS		(16)

/*! \def PP_BMGR_MAX_POOLS_IN_GROUP
 *       Max pools in group
 */
#define PP_BMGR_MAX_POOLS_IN_GROUP	(4)

/*! \def PP_BMGR_MAX_GROUPS
 *       Max supported groups. Real max defined in the DTS
 */
#define PP_BMGR_MAX_GROUPS		(16)

/*! \def PP_BMGR_MAX_POLICIES
 *       Max supported policies. Real max defined in the DTS
 */
#define PP_BMGR_MAX_POLICIES		(256)

/*! \def POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC
 *       bmgr pools flags (Used in bmgr_pool_params.pool_flags)
 *       When set pool will be take part in
 *       policy minimum guaranteed calculation
 */
#define POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC	BIT(0)

/**************************************************************************
 *! \struct	bmgr_pool_params
 **************************************************************************
 *
 * \brief This structure is used in bmgr_pool_configure API in parameter
 *
 **************************************************************************/
struct bmgr_pool_params {
	//!< Pool flags
	u16	flags;
	//!< Group index which the pool belong to
	u8	group_id;
	//!< Amount of buffers in pool
	u32	num_buffers;
	//!< Buffer size for this pool (in bytes). Minimum is 64 bytes
	u32	size_of_buffer;
	//!< Base address of the pool (low)
	u32	base_addr_low;
	//!< Base address of the pool (high)
	u32	base_addr_high;
	//!< Total number of pools
	u32	num_pools;
};

/**************************************************************************
 *! \struct	bmgr_group_params
 **************************************************************************
 *
 * \brief This structure is used for buffer manager database
 *
 **************************************************************************/
struct bmgr_group_params {
	//!< available buffers in group
	u32	available_buffers;
	//!< reserved buffers in this group
	u32	reserved_buffers;
	//!< Pools in policy
	u8	pools[PP_BMGR_MAX_POOLS];
};

/**************************************************************************
 *! \struct	bmgr_pool_in_policy_info
 **************************************************************************
 *
 * \brief	This structure is used in policy_params struct and holds
 *			the information about the pools in policy
 *
 **************************************************************************/
struct bmgr_pool_in_policy_info {
	//!< Pool id
	u8	pool_id;
	//!< Max allowed per pool per policy
	u32	max_allowed;
};

/*! \def POLICY_FLAG_RESERVED1
 *       bmgr policy flags (Used in bmgr_policy_param.policy_flags)
 */
#define POLICY_FLAG_RESERVED1	BIT(0)

/**************************************************************************
 *! \struct	bmgr_policy_params
 **************************************************************************
 *
 * \brief This structure is used in bmgr_policy_configure API in parameter
 *
 **************************************************************************/
struct bmgr_policy_params {
	//!< Policy flags
	u16		flags;
	//!< Group index
	u8		group_id;
	//!< Policy maximum allowed
	u32		max_allowed;
	//!< Policy minimum guaranteed
	u32		min_guaranteed;
	//!< Pools information. Sorted from high priority (index 0) to lowest
	struct bmgr_pool_in_policy_info
		pools_in_policy[4/*PP_BMGR_MAX_POOLS_IN_POLICY*/];
	//!< Number of pools in pools_in_policy
	u8		num_pools_in_policy;
};

#define PP_BMGR_MAX_BURST_IN_POP	(32)
/**************************************************************************
 *! \struct	bmgr_buff_info
 **************************************************************************
 *
 * \brief This structure is used for allocate and deallocate buffer
 *
 **************************************************************************/
struct bmgr_buff_info {
	//!< [Out] buffer pointer low
	u32	addr_low[PP_BMGR_MAX_BURST_IN_POP];
	//!< [Out] buffer pointer high
	u32	addr_high[PP_BMGR_MAX_BURST_IN_POP];
	//!< policy to allocate from
	u8	policy_id;
	//!< pool for deallocate buffer back
	u8	pool_id[PP_BMGR_MAX_BURST_IN_POP];
	//!< number of pointers to allocate (up to 32 pointers)
	u8	num_allocs;
};

/**********************************
 **********************************
 *********** PROTOTYPES ***********
 **********************************
 **********************************/

/**************************************************************************
 *! \fn	bmgr_driver_init
 **************************************************************************
 *
 *  \brief	Initializes the buffer manager driver.
 *			Must be the first driver's function to be called
 *
 *  \return	PP_RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_driver_init(void);

/**************************************************************************
 *! \fn	bmgr_pool_configure
 **************************************************************************
 *
 *  \brief	Configure a Buffer Manager pool
 *
 *  \param	pool_params:	Pool param from user
 *  \param	pool_id[OUT]:	Pool ID
 *
 *  \return	PP_RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_pool_configure(const struct bmgr_pool_params * const pool_params,
			u8 * const pool_id);

/**************************************************************************
 *! \fn	bmgr_policy_configure
 **************************************************************************
 *
 *  \brief	Configure a Buffer Manager policy
 *
 *  \param	policy_params:	Policy param from user
 *  \param	policy_id[OUT]:	Policy ID
 *
 *  \return	PP_RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_policy_configure(
		const struct bmgr_policy_params * const policy_params,
		u8 * const policy_id);

/**************************************************************************
 *! \fn	bmgr_pop_buffer
 **************************************************************************
 *
 *  \brief	Pops a buffer from the buffer manager
 *
 *  \param	buff_info:	Buffer information
 *
 *  \return	PP_RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_pop_buffer(struct bmgr_buff_info * const buff_info);

/**************************************************************************
 *! \fn	bmgr_push_buffer
 **************************************************************************
 *
 *  \brief	Pushes a buffer back to the buffer manager
 *
 *  \param	buff_info:	Buffer information
 *
 *  \return	PP_RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_push_buffer(struct bmgr_buff_info * const buff_info);

#endif /* _BMGR_H_ */
