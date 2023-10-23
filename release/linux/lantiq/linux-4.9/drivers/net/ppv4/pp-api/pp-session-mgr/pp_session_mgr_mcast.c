/*
 * Description: PP session manager multicast extension
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SESS_MGR_MCAST]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/bug.h>
#include <linux/pp_api.h>

#include "pp_common.h"
#include "uc.h"
#include "pp_session_mgr_internal.h"

/**
 * @brief Multicast database definitions
 * @invalid_sess invalid session index
 * @lock spin lock
 * @mcast multicast groups array
 */
struct smgr_mcast_database {
	u32 invalid_sess;
	spinlock_t lock; /*! locks database and HW accesses */
	struct pp_mcast_grp_info grp[PP_MCAST_GRP_MAX];
};

/**
 * @brief Module database
 */
static struct smgr_mcast_database *db;

/**
 * @brief Acquire multicast lock
 */
static inline void __mcast_lock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));

	/* we are not allowed to run in irq context */
	WARN(in_irq(),
	     "session manager mcast shouldn't be used from irq context\n");
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release multicast lock
 */
static inline void __mcast_unlock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));

	spin_unlock_bh(&db->lock);
}

/**
 * @brief Test whether multicast group id is valid or not
 * @param id multicast group id to test
 * @return bool true if the id is a valid multicast group id
 */
static inline bool __mcast_is_group_valid(u16 id)
{
	if (likely(id < PP_MCAST_GRP_MAX))
		return true;

	pr_err("invalid multicast group id %hu, max id %u\n",
	       id, PP_MCAST_GRP_MAX - 1);
	return false;
}

/**
 * @brief Test whether multicast dst id is valid or not
 * @param id multicast dst id to test
 * @return bool true if the id is a valid multicast dst id
 */
static inline bool __mcast_is_dst_valid(u8 id)
{
	if (likely(id < PP_MCAST_DST_MAX))
		return true;

	pr_err("invalid multicast dst id %hhu, max id %u\n",
	       id, PP_MCAST_DST_MAX - 1);
	return false;
}

/**
 * @brief Get multicast group database entry reference
 * @param id multicast group id
 * @return struct pp_mcast_grp_info* entry reference on success,
 *         NULL otherwise
 */
static inline struct pp_mcast_grp_info *__mcast_db_ent(u16 id)
{
	if (unlikely(!__mcast_is_group_valid(id)))
		return NULL;

	return &db->grp[id];
}

static s32 __mcast_dst_set(u16 grp, ulong dst_bmap)
{
	u32 cmd_data;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR)))
		return -EPERM;

	if (unlikely(grp >= PP_MCAST_GRP_MAX)) {
		pr_err("invalid mcast group id %hu\n", grp);
		return -EINVAL;
	}

	cmd_data = (u16)dst_bmap << 16 | grp;
	return uc_egr_mbox_cmd_send(UC_CMD_MCAST_DST, cmd_data,
				    NULL, 0, NULL, 0);
}

void smgr_mcast_sess_track(struct sess_info *sess)
{
	struct pp_mcast_grp_info *grp_db;

	/* if the session is a multicast base session - set the id
	 * if the session is a multicast dst session - set the id
	 */
	if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_GRP_BIT)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(sess->args->mcast.grp_idx);
		grp_db->base_sess = sess->db_ent->info.sess_id;
		__mcast_unlock();
	} else if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_DST_BIT)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(sess->args->mcast.grp_idx);
		grp_db->dst_sess[sess->args->mcast.dst_idx] =
			sess->db_ent->info.sess_id;
		__mcast_unlock();
	}
}

void smgr_mcast_sess_untrack(struct sess_db_entry *ent)
{
	struct pp_mcast_grp_info *grp_db;

	/* if the session is a multicast session - clean the id */
	if (smgr_is_sess_mcast_grp(&ent->info)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(ent->info.mcast.grp_idx);
		grp_db->base_sess = db->invalid_sess;
		__mcast_unlock();
	} else if (smgr_is_sess_mcast_dst(&ent->info)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(ent->info.mcast.grp_idx);
		grp_db->dst_sess[ent->info.mcast.dst_idx] = db->invalid_sess;
		__mcast_unlock();
	}
}

bool smgr_mcast_is_info_valid(struct pp_sess_create_args *args)
{
	struct pp_nf_info nf;

	if (test_bit(PP_SESS_FLAG_MCAST_GRP_BIT, &args->flags)) {
		/* dst flag must be clear and mcast group must be valid */
		if (test_bit(PP_SESS_FLAG_MCAST_DST_BIT, &args->flags) ||
		    !__mcast_is_group_valid(args->mcast.grp_idx))
			return false;
		/* get the multicast nf info */
		if (unlikely(pp_nf_get(PP_NF_MULTICAST, &nf))) {
			pr_err("failed to get the multicast nf info\n");
			return false;
		}
		/* session ingress port must be the multicast nf port */
		if (unlikely(nf.q != args->dst_q)) {
			pr_err("invalid multicast egress queue %hu\n",
			       args->dst_q);
			return false;
		}
	} else if (test_bit(PP_SESS_FLAG_MCAST_DST_BIT, &args->flags)) {
		/* group flag must be clear and mcast info must be valid */
		if (test_bit(PP_SESS_FLAG_MCAST_GRP_BIT, &args->flags) ||
		    !__mcast_is_dst_valid(args->mcast.dst_idx)         ||
		    !__mcast_is_group_valid(args->mcast.grp_idx))
			return false;
		/* get the multicast nf info */
		if (unlikely(pp_nf_get(PP_NF_MULTICAST, &nf))) {
			pr_err("failed to get the multicast nf info\n");
			return false;
		}
		/* session ingress port must be the multicast nf port */
		if (unlikely(nf.pid != args->in_port)) {
			pr_err("invalid multicast ingress port %hu\n",
			       args->in_port);
			return false;
		}
	}

	return true;
}

void smgr_mcast_sess_ent_prepare(struct sess_info *sess)
{
	if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_GRP_BIT)) {
		/* store the group index for later */
		sess->db_ent->info.mcast.grp_idx = sess->args->mcast.grp_idx;
	} else if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_DST_BIT)) {
		/* store the mcast info for later */
		sess->db_ent->info.mcast.grp_idx = sess->args->mcast.grp_idx;
		sess->db_ent->info.mcast.dst_idx = sess->args->mcast.dst_idx;
	}
}

s32 smgr_mcast_sess_lookup(struct sess_info *sess)
{
	struct pp_mcast_grp_info *grp_db;

	if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_GRP_BIT)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(sess->args->mcast.grp_idx);
		if (unlikely(grp_db->base_sess != db->invalid_sess)) {
			__mcast_unlock();
			return -EEXIST;
		}
		__mcast_unlock();
	} else if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_MCAST_DST_BIT)) {
		__mcast_lock();
		grp_db = __mcast_db_ent(sess->args->mcast.grp_idx);
		if (unlikely(grp_db->dst_sess[sess->args->mcast.dst_idx] !=
			     db->invalid_sess)) {
			__mcast_unlock();
			return -EEXIST;
		}
		__mcast_unlock();
	}

	return 0;
}

s32 pp_mcast_dst_set(u16 group, u8 dst, bool add_op)
{
	struct pp_mcast_grp_info *grp_db;
	s32 ret;

	if (unlikely(!pp_is_ready()))
		return -EPERM;

	if (unlikely(!__mcast_is_dst_valid(dst)))
		return -EINVAL;

	__mcast_lock();
	grp_db = __mcast_db_ent(group);
	if (unlikely(!grp_db)) {
		__mcast_unlock();
		return -EINVAL;
	}

	if (add_op)
		set_bit(dst, &grp_db->dst_bmap);
	else
		clear_bit(dst, &grp_db->dst_bmap);

	/* set the dst in hal layer */
	ret = __mcast_dst_set(group, grp_db->dst_bmap);
	if (unlikely(ret)) {
		if (add_op)
			clear_bit(dst, &grp_db->dst_bmap);
		else
			set_bit(dst, &grp_db->dst_bmap);
	}
	__mcast_unlock();

	return ret;
}
EXPORT_SYMBOL(pp_mcast_dst_set);

s32 pp_mcast_group_info_get(u16 group, struct pp_mcast_grp_info *info)
{
	struct pp_mcast_grp_info *grp_db;

	if (unlikely(!pp_is_ready()))
		return -EPERM;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__mcast_lock();
	grp_db = __mcast_db_ent(group);
	if (unlikely(!grp_db)) {
		__mcast_unlock();
		return -EINVAL;
	}

	memcpy(info, grp_db, sizeof(*grp_db));
	__mcast_unlock();

	return 0;
}
EXPORT_SYMBOL(pp_mcast_group_info_get);

s32 smgr_mcast_init(u32 invalid_sess_id)
{
	struct pp_mcast_grp_info *mcast;
	u32 grp_id, dst_id;

	/* allocate mcast db entries */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("Failed to alloc %u mcast memory\n", (u32)sizeof(*db));
		return -ENOMEM;
	}

	/* init lock */
	spin_lock_init(&db->lock);
	__mcast_lock();
	db->invalid_sess = invalid_sess_id;
	for (grp_id = 0; grp_id < PP_MCAST_GRP_MAX; grp_id++) {
		mcast = __mcast_db_ent(grp_id);
		/* set all session ids with invalid id */
		mcast->base_sess = db->invalid_sess;
		for (dst_id = 0; dst_id < PP_MCAST_DST_MAX; dst_id++)
			mcast->dst_sess[dst_id] = db->invalid_sess;
	}
	__mcast_unlock();

	return 0;
}

void smgr_mcast_exit(void)
{
	kfree(db);
	db = NULL;
}
