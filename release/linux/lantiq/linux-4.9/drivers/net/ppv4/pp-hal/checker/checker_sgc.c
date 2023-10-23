/*
 * Description: PP Checker SGC sub-module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#define pr_fmt(fmt) "[CHK_SGC]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_regs.h"
#include "checker_regs.h"
#include "checker_internal.h"
#include "checker.h"

/**
 * @brief Checker SGC RAMs definitions
 */
enum sgc_ram {
	SGC_RAM0,
	SGC_RAM1,
	SGC_RAM_CNT,
};

/**
 * @brief SGC owner info
 */
struct owner_db_info {
	struct list_head node;
	u32 idx;
	u8  grp;
};

/**
 * @brief SGC counter database info
 */
struct cntr_db_info {
	struct list_head node; /*! list node to use with free list */
	u32 n_sessions;        /*! number of associated sessions,
				*  counter cannot be freed if this
				*  counter is not 0
				*/
	u8  grp_idx;           /*! counter's group index           */
	u16 idx;               /*! counter index within the group  */
	u16 in_ram_idx;        /*! index within the RAM            */
	u64 addr;              /*! virtual address                 */
};

/**
 * @brief SGC group database info
 */
struct grp_db_info {
	struct list_head     free;  /*! free counters list                 */
	struct cntr_db_info *cntrs; /*! counters entries                   */
	u8  ram;                    /*! RAM index                          */
	u8  in_ram_idx;             /*! index within the RAM               */
	u8  idx;                    /*! group index                        */
	u16 n_cntrs;                /*! number of counters in the group    */
	u16 used_cntrs;             /*! number of used counters            */
	u32 owner;                  /*! the owner of the group             */
	u64 wr_reg0;                /*! reg address to write counter value */
	u64 wr_reg1;                /*! reg address to write counter value */
	u64 wr_reg2;                /*! reg address to write counter value */
	u64 rd_reg0;                /*! reg address to read counter value  */
	u64 rd_reg1;                /*! reg address to read counter value  */
	u64 rd_reg2;                /*! reg address to read counter value  */
};

/**
 * @brief SGC module database
 */
struct sgc_database {
	struct grp_db_info   groups[PP_SI_SGC_MAX];
	struct owner_db_info owners[PP_SI_SGC_MAX];
	struct list_head     free_owners; /*! free owners list */
	spinlock_t           lock;        /*! lock for protecting the db */
};

/**
 * @enum chk_sgc_fld
 * @brief SGC fields enumeration
 */
enum chk_sgc_fld {
	CHK_SGC_FLDS_FIRST,
	CHK_SGC_FLD_BYTES = CHK_SGC_FLDS_FIRST,
	CHK_SGC_FLD_PKTS,
	CHK_SGC_FLDS_LAST = CHK_SGC_FLD_PKTS,
	CHK_SGC_FLDS_NUM,
};

/**
 * @brief SGC fields masks
 */
#define CHK_SGC_FLD_BYTES_LSB                      (0)
#define CHK_SGC_FLD_BYTES_MSB                      (51)
#define CHK_SGC_FLD_PKTS_LSB                       (54)
#define CHK_SGC_FLD_PKTS_MSB                       (95)

/* Group counters RAM 0 groups, below values describes the HW, DO NOT CHANGE */
#define SGC_GRP0_RAM_IDX                           (SGC_RAM0)
#define SGC_GRP0_IN_RAM_IDX                        (0)

#define SGC_GRP3_RAM_IDX                           (SGC_RAM0)
#define SGC_GRP3_IN_RAM_IDX                        (1)

#define SGC_GRP2_RAM_IDX                           (SGC_RAM0)
#define SGC_GRP2_IN_RAM_IDX                        (2)

#define SGC_GRP5_RAM_IDX                           (SGC_RAM0)
#define SGC_GRP5_IN_RAM_IDX                        (3)

/* Group counters RAM 1 groups, below values describes the HW, DO NOT CHANGE */
#define SGC_GRP1_RAM_IDX                           (SGC_RAM1)
#define SGC_GRP1_IN_RAM_IDX                        (0)

#define SGC_GRP4_RAM_IDX                           (SGC_RAM1)
#define SGC_GRP4_IN_RAM_IDX                        (1)

#define SGC_GRP6_RAM_IDX                           (SGC_RAM1)
#define SGC_GRP6_IN_RAM_IDX                        (2)

#define SGC_GRP7_RAM_IDX                           (SGC_RAM1)
#define SGC_GRP7_IN_RAM_IDX                        (3)

/**
 * @brief Checker group counters fields
 */
struct buf_fld_info sgc_flds[CHK_SGC_FLDS_NUM];

/**
 * @brief module database
 */
static struct sgc_database *db;

/**
 * @brief Acquire session manager lock
 */
static inline void __sgc_lock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));

	/* we are not allowed to run in irq context */
	WARN(in_irq(), "cannot run from irq context\n");
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release session manager lock
 */
static inline void __sgc_unlock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	spin_unlock_bh(&db->lock);
}

/**
 * @brief Test whether SGC counter index is valid
 * @param grp counter's group
 * @return bool true in case the index is valid, false otherwise
 */
bool chk_sgc_is_group_valid(u8 grp)
{
	if (likely(grp < PP_SI_SGC_MAX))
		return true;

	pr_err("Invalid SGC group %u\n", grp);
	return false;
}

/**
 * @brief Test whether SGC counter index is valid
 * @param grp counter's group
 * @param cntr counter index
 * @return bool true in case the index is valid, false otherwise
 */
static inline bool __sgc_is_idx_valid(u8 grp, u16 cntr)
{
	if (unlikely(!chk_sgc_is_group_valid(grp)))
		return false;

	if (likely(cntr < db->groups[grp].n_cntrs))
		return true;

	pr_err("Invalid SGC [%u][%04u]\n", grp, cntr);
	return false;
}

/**
 * @brief Calculate sgc index in the ram, e.g.
 *        RAM 0 contain groups 1,2,4,5
 *        Group 1 has 1024 counters, meaning group 2 1st counter index
 *        in the ram is 1024, 2nd counter is 1025 and so on
 * @param grp counter group
 * @param cntr counter index
 * @return u16 counter index within the ram
 */
static inline u16 __sgc_in_ram_idx_calc(struct grp_db_info *grp,
					struct cntr_db_info *cntr)
{
	struct grp_db_info *other;
	u16 base = 0;

	for_each_arr_entry(other, db->groups, ARRAY_SIZE(db->groups)) {
		/* skip all second ram groups */
		if (grp->ram != other->ram)
			continue;
		/* add all counters of the groups prior to our group */
		if (other->in_ram_idx < grp->in_ram_idx)
			base += other->n_cntrs;
	}
	return base + cntr->idx;
}

/**
 * @brief Init all groups static info, info which is defined by HW
 */
static void __sgc_groups_static_init(void)
{
	db->groups[0].n_cntrs    = SGC_GRP0_CNTRS_NUM;
	db->groups[1].n_cntrs    = SGC_GRP1_CNTRS_NUM;
	db->groups[2].n_cntrs    = SGC_GRP2_CNTRS_NUM;
	db->groups[3].n_cntrs    = SGC_GRP3_CNTRS_NUM;
	db->groups[4].n_cntrs    = SGC_GRP4_CNTRS_NUM;
	db->groups[5].n_cntrs    = SGC_GRP5_CNTRS_NUM;
	db->groups[6].n_cntrs    = SGC_GRP6_CNTRS_NUM;
	db->groups[7].n_cntrs    = SGC_GRP7_CNTRS_NUM;

	db->groups[0].in_ram_idx = SGC_GRP0_IN_RAM_IDX;
	db->groups[1].in_ram_idx = SGC_GRP1_IN_RAM_IDX;
	db->groups[2].in_ram_idx = SGC_GRP2_IN_RAM_IDX;
	db->groups[3].in_ram_idx = SGC_GRP3_IN_RAM_IDX;
	db->groups[4].in_ram_idx = SGC_GRP4_IN_RAM_IDX;
	db->groups[5].in_ram_idx = SGC_GRP5_IN_RAM_IDX;
	db->groups[6].in_ram_idx = SGC_GRP6_IN_RAM_IDX;
	db->groups[7].in_ram_idx = SGC_GRP7_IN_RAM_IDX;

	db->groups[0].ram        = SGC_GRP0_RAM_IDX;
	db->groups[1].ram        = SGC_GRP1_RAM_IDX;
	db->groups[2].ram        = SGC_GRP2_RAM_IDX;
	db->groups[3].ram        = SGC_GRP3_RAM_IDX;
	db->groups[4].ram        = SGC_GRP4_RAM_IDX;
	db->groups[5].ram        = SGC_GRP5_RAM_IDX;
	db->groups[6].ram        = SGC_GRP6_RAM_IDX;
	db->groups[7].ram        = SGC_GRP7_RAM_IDX;
}

/**
 * @brief Init group database info
 * @param grp the group
 * @param the group id
 * @return s32 0 on success, errorcode otherwise
 */
static s32 __sgc_group_init(struct grp_db_info *grp, u8 idx)
{
	struct cntr_db_info *cntr;
	u64 ram_base;
	u16 i;

	grp->idx   = idx;
	grp->owner = PP_SGC_SHARED_OWNER;
	grp->cntrs = devm_kcalloc(pp_dev_get(), grp->n_cntrs,
				  sizeof(*grp->cntrs), GFP_KERNEL);
	if (unlikely(!grp->cntrs)) {
		pr_err("Failed to allocate memory for group %u\n", grp->idx);
		return -ENOMEM;
	}

	if (grp->ram == SGC_RAM0) {
		grp->rd_reg0 = PP_CHK_GCNTS_SRAM0_R0_REG;
		grp->rd_reg1 = PP_CHK_GCNTS_SRAM0_R1_REG;
		grp->rd_reg2 = PP_CHK_GCNTS_SRAM0_R2_REG;
		grp->wr_reg0 = PP_CHK_GCNTS_SRAM0_W0_REG;
		grp->wr_reg1 = PP_CHK_GCNTS_SRAM0_W1_REG;
		grp->wr_reg2 = PP_CHK_GCNTS_SRAM0_W2_REG;
	} else {
		grp->rd_reg0 = PP_CHK_GCNTS_SRAM1_R0_REG;
		grp->rd_reg1 = PP_CHK_GCNTS_SRAM1_R1_REG;
		grp->rd_reg2 = PP_CHK_GCNTS_SRAM1_R2_REG;
		grp->wr_reg0 = PP_CHK_GCNTS_SRAM1_W0_REG;
		grp->wr_reg1 = PP_CHK_GCNTS_SRAM1_W1_REG;
		grp->wr_reg2 = PP_CHK_GCNTS_SRAM1_W2_REG;
	}

	/* init free counters list */
	INIT_LIST_HEAD(&grp->free);
	if (grp->ram == SGC_RAM1)
		ram_base = CHK_GCNTS1_RAM_BASE;
	else
		ram_base = CHK_GCNTS0_RAM_BASE;

	i = 0;
	for_each_arr_entry(cntr, grp->cntrs, grp->n_cntrs, i++) {
		/* set indexes */
		cntr->idx        = i;
		cntr->grp_idx    = grp->idx;
		cntr->in_ram_idx = __sgc_in_ram_idx_calc(grp, cntr);
		/* Calculate group counter indirect address, for indirect
		 * address calculation, counter size is 4 bytes, counter
		 * real size is 12 bytes
		 */
		cntr->addr = ram_base + (cntr->in_ram_idx << 2);
		/* add the counter to the free list */
		INIT_LIST_HEAD(&cntr->node);            /* init list node */
		list_add_tail(&cntr->node, &grp->free); /* add to free list */

		pr_debug("counter [%u][%04u]: ram %u, in_ram_idx %u, addr %#llx\n",
			 grp->idx, cntr->idx, grp->ram, cntr->in_ram_idx,
			 cntr->addr);
	}

	return 0;
}

/**
 * @brief Init all groups dynamic info in database,
 *        memory allocations etc...
 */
static s32 __sgc_groups_init(void)
{
	struct grp_db_info *grp;
	s32 ret;
	u8 i = 0;

	for_each_arr_entry(grp, db->groups, ARRAY_SIZE(db->groups), i++) {
		ret = __sgc_group_init(grp, i);
		if (unlikely(ret))
			return ret;
	}

	return ret;
}

static bool __sgc_grp_is_destroyable(const void *_grp)
{
	const struct grp_db_info *grp = _grp;

	return !!grp->cntrs;
}

/**
 * @brief Free database groups resources
 */
static void __sgc_groups_destroy(void)
{
	struct grp_db_info *grp;
	u32 n = ARRAY_SIZE(db->groups);

	for_each_arr_entry_cond(grp, db->groups, n, __sgc_grp_is_destroyable)
		devm_kfree(pp_dev_get(), grp->cntrs);
}

/**
 * @brief Init owner database info
 * @param owner the owner db entry
 * @id the owner id
 */
static inline void __sgc_owner_init(struct owner_db_info *owner, u32 idx)
{
	/* init owner info */
	INIT_LIST_HEAD(&owner->node);
	owner->idx = idx;
	owner->grp = PP_SI_SGC_MAX;
	/* add to free list */
	list_add_tail(&owner->node, &db->free_owners);
}

/**
 * @brief Init all groups owners info
 */
static void __sgc_owners_init(void)
{
	struct owner_db_info *owner;
	u32 i = 0;

	INIT_LIST_HEAD(&db->free_owners);
	for_each_arr_entry(owner, db->owners, ARRAY_SIZE(db->owners), i++)
		__sgc_owner_init(owner, i);
}

void sgc_encode(struct hw_sgc *hw, u64 pkts, u64 bytes)
{
	buf_fld_set(&sgc_flds[CHK_SGC_FLD_BYTES], hw->buf, bytes);
	buf_fld_set(&sgc_flds[CHK_SGC_FLD_PKTS],  hw->buf, pkts);
}

void sgc_decode(struct hw_sgc *hw, u64 *pkts, u64 *bytes)
{
	*bytes = buf_fld_get(&sgc_flds[CHK_SGC_FLD_BYTES], hw->buf);
	*pkts  = buf_fld_get(&sgc_flds[CHK_SGC_FLD_PKTS],  hw->buf);
}

/**
 * @brief Test whether SGC group owner index is valid
 * @param owner owner index
 * @return bool true in case the index is valid, false otherwise
 */
static inline bool __sgc_is_owner_valid(u32 owner)
{
	if (likely(owner < PP_SGC_MAX_OWNERS))
		return true;

	pr_err("Invalid SGC group owner  %u\n", owner);
	return false;
}

bool __sgc_is_group_owner(u32 owner, struct grp_db_info *grp)
{
	if (unlikely(!__sgc_is_owner_valid(owner)))
		return false;

	if (likely(owner == grp->owner))
		return true;

	if (grp->owner != PP_SGC_SHARED_OWNER)
		pr_err("SGC group %u is owned by %u, not by %u\n", grp->idx,
		       grp->owner, owner);
	else
		pr_err("SGC group %u has no owner\n", grp->idx);

	return false;
}

s32 chk_sgc_get(u8 grp, u16 cntr, u64 *pkts, u64 *bytes,
		u32 *n_sessions)
{
	struct grp_db_info  *grp_info;
	struct cntr_db_info *cntr_info;
	struct hw_sgc sgc = { 0 };

	if (unlikely(!(!ptr_is_null(pkts) && !ptr_is_null(bytes))))
		return -EINVAL;
	if (unlikely(!__sgc_is_idx_valid(grp, cntr)))
		return -EINVAL;

	grp_info  = &db->groups[grp];
	cntr_info = &grp_info->cntrs[cntr];

	/* a dummy read to trigger the indirect mechanism */
	__sgc_lock();
	PP_REG_RD32(cntr_info->addr);
	sgc.buf[0] = PP_REG_RD32(grp_info->rd_reg0);
	sgc.buf[1] = PP_REG_RD32(grp_info->rd_reg1);
	sgc.buf[2] = PP_REG_RD32(grp_info->rd_reg2);
	if (n_sessions)
		*n_sessions = cntr_info->n_sessions;
	__sgc_unlock();

	sgc_decode(&sgc, pkts, bytes);
	pr_debug("buf[2] %#x, buf[1] %#x, buf[0] %#x, pkts = %llu, bytes = %llu\n",
		 sgc.buf[2], sgc.buf[1], sgc.buf[0], *pkts, *bytes);

	return 0;
}

/**
 * @brief Modify session group counter
 * @param grp counter group
 * @param cntr counter index within the group
 * @param add specify what modification should be done, true for
 *            add, false for substruct
 * @param pkts number of packets to modify
 * @param bytes number of bytes to modify
 * @return s32 0 on success, error code otherwise
 */
static s32 __sgc_mod(struct cntr_db_info *cntr, bool add, u32 pkts, u32 bytes)
{
	struct grp_db_info *grp = &db->groups[cntr->grp_idx];
	u32 _bytes, _pkts, cmd = 0;

	if (unlikely(!pkts && !bytes)) {
		pr_err("pkts %u, bytes %u, at least one should be non-zero\n",
		       pkts, bytes);
		return -EINVAL;
	}

	cmd = PP_FIELD_MOD(CHK_CMD_SGC_INC_MSK, add,              cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_SGC_RAM_MSK, grp->ram,         cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_SGC_IDX_MSK, cntr->in_ram_idx, cmd);

	/* iterate till pkts and bytes are zero and modify the maximum
	 * supported by the command every iteration
	 */
	while (pkts || bytes) {
		_bytes = min_t(u32, MASK2MAX(CHK_CMD_SGC_BYTES_MSK), bytes);
		_pkts  = min_t(u32, MASK2MAX(CHK_CMD_SGC_PKTS_MSK),  pkts);
		bytes -= _bytes;
		pkts  -= _pkts;

		/* FIELD_MOD is safe cause it clear the mask bits */
		cmd = PP_FIELD_MOD(CHK_CMD_SGC_BYTES_MSK, _bytes, cmd);
		cmd = PP_FIELD_MOD(CHK_CMD_SGC_PKTS_MSK, _pkts, cmd);
		pr_debug("%s %u packets and %u bytes %s SGC[%u][%u], cmd %#x\n",
			 add ? "add" : "sub", pkts, bytes, add ? "to" : "from",
			 cntr->grp_idx, cntr->idx, cmd);
		PP_REG_WR32(PP_CHK_HOST_CNTS_UPDATE_REG, cmd);
	}

	return 0;
}

/**
 * @brief Set value to a session group counter
 * @param grp counter group
 * @param cntr counter index within the group
 * @param pkts new number of packets
 * @param bytes new number of bytes
 * @return s32 0 on success, error code otherwise
 */
static s32 __sgc_set(struct cntr_db_info *cntr, u64 pkts, u64 bytes)
{
	struct grp_db_info *grp_info;
	struct hw_sgc sgc = { 0 };

	grp_info = &db->groups[cntr->grp_idx];
	sgc_encode(&sgc, pkts, bytes);

	PP_REG_WR32(grp_info->wr_reg0, sgc.buf[0]);
	PP_REG_WR32(grp_info->wr_reg1, sgc.buf[1]);
	PP_REG_WR32(grp_info->wr_reg2, sgc.buf[2]);
	/* a dummy write to trigger the indirect mechanism */
	PP_REG_WR32(cntr->addr, 0);

	return 0;
}

s32 chk_sgc_mod(u32 owner, u8 grp, u16 cntr, enum pp_stats_op act, u32 pkts,
		u32 bytes)
{
	struct cntr_db_info *cntr_info = &db->groups[grp].cntrs[cntr];
	struct grp_db_info  *grp_info = &db->groups[grp];
	s32 ret;

	if (unlikely(!__sgc_is_idx_valid(grp, cntr)))
		return -EINVAL;

	__sgc_lock();
	if (unlikely(!__sgc_is_group_owner(owner, grp_info))) {
		ret = -EPERM;
		goto done;
	}

	switch (act) {
	case PP_STATS_SUB:
		ret = __sgc_mod(cntr_info, false, pkts, bytes);
		break;
	case PP_STATS_ADD:
		ret = __sgc_mod(cntr_info, true, pkts, bytes);
		break;
	case PP_STATS_RESET:
		ret = __sgc_set(cntr_info, 0, 0);
		break;
	default:
		pr_err("Invalid stats operation %u\n", act);
		ret = -EINVAL;
		break;
	}

done:
	__sgc_unlock();
	return ret;
}

/**
 * @brief Test whether the specified counter is active.
 *        An active counter is one that was allocated
 * @param grp the group
 * @param cntr the counter
 * @return bool true if the counter is active, false otherwise
 */
static bool __sgc_is_active(struct cntr_db_info *cntr_info)
{
	/* if the counter isn't in the free list, he is active */
	if (likely(list_empty(&cntr_info->node)))
		return true;

	pr_err("counter [%u][%04u] isn't active\n", cntr_info->grp_idx,
	       cntr_info->idx);
	return false;
}

/**
 * @brief Test whether the specified counter is in use. A used counter
 *        is a counter which is attached to sessions
 * @param grp the group
 * @param cntr the counter
 * @return bool true if the counter is used, false otherwise
 */
static bool __sgc_is_used(struct cntr_db_info *cntr_info)
{
	/* if the counter have sessions attached, he is used */
	if (likely(cntr_info->n_sessions > 0))
		return true;

	return false;
}

static inline void *__must_check __owner_alloc(void)
{
	struct owner_db_info *owner = NULL;

	if (unlikely(list_empty(&db->free_owners))) {
		pr_err("no free owner available\n");
		return NULL;
	}
	/* take the first counter */
	owner = list_first_entry(&db->free_owners, struct owner_db_info, node);
	/* remove the counter from the free list */
	list_del_init(&owner->node);

	return owner;
}

static inline bool __sgc_is_group_reserved(struct grp_db_info *grp)
{
	if (likely(grp->owner == PP_SGC_SHARED_OWNER))
		return false;

	pr_err("SGC group %u already reserved by %u\n", grp->idx, grp->owner);
	return true;
}

s32 chk_sgc_group_unreserve(u8 grp, u32 owner)
{
	struct owner_db_info *owner_info;
	struct grp_db_info   *grp_info;
	s32 ret = 0;

	if (unlikely(!chk_sgc_is_group_valid(grp)))
		return -EINVAL;

	grp_info   = &db->groups[grp];
	owner_info = &db->owners[owner];

	__sgc_lock();
	/* check if reserved by this owner */
	if (unlikely(!__sgc_is_group_owner(owner, grp_info))) {
		ret = -EPERM;
		goto done;
	}

	/* set group as unreserved and return the owner to the free list */
	owner_info->grp = PP_SI_SGC_MAX;
	grp_info->owner = PP_SGC_SHARED_OWNER;
	list_add_tail(&owner_info->node, &db->free_owners);

done:
	__sgc_unlock();
	return ret;
}

s32 chk_sgc_group_reserve(u8 grp, u32 *owner)
{
	struct owner_db_info *owner_info;
	struct grp_db_info *grp_info;
	s32 ret = 0;

	if (unlikely(ptr_is_null(owner)))
		return -EINVAL;
	if (unlikely(!chk_sgc_is_group_valid(grp)))
		return -EINVAL;

	grp_info = &db->groups[grp];
	__sgc_lock();
	/* if the group was already reserved or some counters were
	 * already allocated on the group is cannot be reserved
	 */
	if (unlikely(__sgc_is_group_reserved(grp_info) ||
		     grp_info->used_cntrs)) {
		ret = -EPERM;
		goto done;
	}

	/* allocated owner id */
	owner_info = __owner_alloc();
	if (unlikely(!owner_info)) {
		/* this should not happened EVER */
		ret = -ENOSPC;
		goto done;
	}
	*owner = db->groups[grp].owner = owner_info->idx;
	owner_info->grp = grp;

done:
	__sgc_unlock();
	return ret;
}

s32 chk_sgc_group_info_get(u8 grp, u32 *n_cntrs, u32 *owner)
{
	if (unlikely(ptr_is_null(owner)))
		return -EINVAL;
	if (unlikely(ptr_is_null(n_cntrs)))
		return -EINVAL;
	if (unlikely(!chk_sgc_is_group_valid(grp)))
		return -EINVAL;

	__sgc_lock();
	*owner   = db->groups[grp].owner;
	*n_cntrs = db->groups[grp].n_cntrs;
	__sgc_unlock();

	return 0;
}

static inline void *__must_check __grp_free_cntr_alloc(struct grp_db_info *grp)
{
	struct cntr_db_info *cntr = NULL;

	if (unlikely(list_empty(&grp->free))) {
		pr_err("no free counter available on group %u\n", grp->idx);
		return NULL;
	}
	/* take the first counter */
	cntr = list_first_entry(&grp->free, struct cntr_db_info, node);
	/* remove the counter from the free list */
	list_del_init(&cntr->node);
	grp->used_cntrs++;

	return cntr;
}

s32 chk_sgc_alloc(u32 owner, u8 grp, u16 *cntr)
{
	struct grp_db_info  *grp_info;
	struct cntr_db_info *cntr_info;

	if (unlikely(ptr_is_null(cntr) || !chk_sgc_is_group_valid(grp)))
		return -EINVAL;

	grp_info = &db->groups[grp];
	__sgc_lock();
	if (unlikely(!__sgc_is_group_owner(owner, grp_info))) {
		__sgc_unlock();
		return -EPERM;
	}

	cntr_info = __grp_free_cntr_alloc(grp_info);
	__sgc_unlock();

	if (unlikely(!cntr_info))
		return -ENOSPC;

	*cntr = cntr_info->idx;
	pr_debug("counter [%u][%04u] allocated\n", grp, *cntr);

	return 0;
}

s32 chk_sgc_free(u32 owner, u8 grp, u16 cntr)
{
	struct grp_db_info  *grp_info;
	struct cntr_db_info *cntr_info;

	if (unlikely(!__sgc_is_idx_valid(grp, cntr)))
		return -EINVAL;

	grp_info  = &db->groups[grp];
	cntr_info = &grp_info->cntrs[cntr];

	__sgc_lock();
	if (unlikely(!__sgc_is_group_owner(owner, grp_info))) {
		__sgc_unlock();
		return -EPERM;
	}

	if (unlikely(!__sgc_is_active(cntr_info))) {
		__sgc_unlock();
		return -EINVAL;
	}

	if (unlikely(__sgc_is_used(cntr_info))) {
		pr_err("cannot free, sgc[%u][%04u] is used by %u sessions\n",
		       cntr_info->grp_idx, cntr_info->idx,
		       cntr_info->n_sessions);
		__sgc_unlock();
		return -EBUSY;
	}

	/* reset the counter */
	__sgc_set(cntr_info, 0, 0);
	/* return the counter back the free list tail */
	list_add_tail(&cntr_info->node, &grp_info->free);
	grp_info->used_cntrs--;
	__sgc_unlock();

	pr_debug("counter [%u][%04u] freed\n", grp, cntr);
	return 0;
}

s32 chk_sgc_session_attach(u8 grp, u16 cntr)
{
	struct cntr_db_info *cntr_info;

	if (unlikely(!__sgc_is_idx_valid(grp, cntr)))
		return -EINVAL;

	cntr_info = &db->groups[grp].cntrs[cntr];
	__sgc_lock();
	if (unlikely(!__sgc_is_active(cntr_info))) {
		__sgc_unlock();
		return -EINVAL;
	}

	cntr_info->n_sessions++;
	__sgc_unlock();

	pr_debug("session attached to sgc[%u][%04u], n_sessions %u\n",
		 cntr_info->grp_idx, cntr_info->idx, cntr_info->n_sessions);

	return 0;
}

s32 chk_sgc_session_detach(u8 grp, u16 cntr)
{
	struct cntr_db_info *cntr_info;

	if (unlikely(!__sgc_is_idx_valid(grp, cntr)))
		return -EINVAL;

	cntr_info = &db->groups[grp].cntrs[cntr];
	__sgc_lock();
	if (unlikely(!__sgc_is_active(cntr_info))) {
		__sgc_unlock();
		return -EINVAL;
	}

	if (unlikely(!__sgc_is_used(cntr_info))) {
		pr_err("cannot detach, sgc[%u][%04u] is not used by any sessions\n",
		       cntr_info->grp_idx, cntr_info->idx);
		__sgc_unlock();
		return -EINVAL;
	}

	cntr_info->n_sessions--;
	__sgc_unlock();

	pr_debug("session detached from sgc[%u][%04u], n_sessions %u\n",
		 cntr_info->grp_idx, cntr_info->idx, cntr_info->n_sessions);
	return 0;
}

/**
 * @brief Init sgc database
 */
s32 __sgc_db_init(void)
{
	/* Allocate db */
	db = devm_kzalloc(pp_dev_get(), sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("failed to allocate database\n");
		return -ENOMEM;
	};

	spin_lock_init(&db->lock);
	__sgc_groups_static_init();
	__sgc_groups_init();
	__sgc_owners_init();

	return 0;
}

/**
 * @brief Destroy sgc database
 */
void __sgc_db_destroy(void)
{
	if (unlikely(!db))
		return;

	__sgc_groups_destroy();
	devm_kfree(pp_dev_get(), db);
	db = NULL;
}

s32 chk_sgc_init(void)
{
	s32 ret;

	ret = __sgc_db_init();
	if (unlikely(ret))
		return ret;

	/* init SGC fields */
	BUF_FLD_INIT_SIMPLE(sgc_flds, CHK_SGC_FLD_BYTES);
	BUF_FLD_INIT_SIMPLE(sgc_flds, CHK_SGC_FLD_PKTS);

	return 0;
}

void chk_sgc_exit(void)
{
	__sgc_db_destroy();
}
