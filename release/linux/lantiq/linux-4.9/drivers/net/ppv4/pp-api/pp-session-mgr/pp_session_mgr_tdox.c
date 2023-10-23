/*
 * Description: PP session manager turbodox module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SESS_MGR_TDOX]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>     /* For pr_err/pr_debug/...   */
#include <linux/types.h>      /* For standard types        */
#include <linux/spinlock.h>   /* For DB lock               */
#include <linux/errno.h>      /* For the EINVAL/EEXIST/... */
#include <linux/bug.h>        /* For WARN()                */
#include <linux/list.h>       /* For free/act lists        */
#include <linux/timer.h>      /* For sync/done timers      */
#include <linux/jiffies.h>    /* For jiffies               */
#include <linux/pp_qos_api.h> /* For QoS manager APIs      */
#include <linux/pktprs.h>

#include "uc.h"
#include "classifier.h"
#include "checker.h"
#include "pp_session_mgr.h"
#include "pp_session_mgr_internal.h"

#define TDOX_BYPASS 1

/**
 * @define Tdox timer interval
 */
#define SMGR_TDOX_TIMER_INTERVAL_MS  (3000)

/**
 * @define Minimum packet length for qualification
 */
#define TDOX_DEFAULT_AVG_PKT_SIZE_THR 122

/**
 * @define Minimum packet length for tunnel qualification
 */
#define TDOX_DEFAULT_AVG_PKT_SIZE_TUNN_THR 204

/**
 * @define Number of required entries is doubled due to candidates
 */
#define SMGR_TDOX_DB_NUM_ENTRIES (UC_MAX_TDOX_SESSIONS * 2)

#define DB_TDOX_FREE_LIST_GET_NODE(_db)                                        \
	list_first_entry(&(_db)->tdox_free_list, struct tdox_db_entry,         \
			 tdox_node)
#define DB_TDOX_FREE_LIST_ADD_NODE(_db, _ent)                                  \
	list_add_tail(&(_ent)->tdox_node, &(_db)->tdox_free_list)
#define DB_TDOX_MOVE_TO_FREE_LIST(_db, _ent)                                   \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_free_list)
#define DB_TDOX_MOVE_TO_BUSY_LIST(_db, _ent)                                   \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_busy_list)
#define DB_TDOX_MOVE_TO_OBSOLETE_LIST(_db, _ent)                               \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_obsolete_list)
#define DB_TDOX_CANDIDATE_FREE_LIST_GET_NODE(_db)                              \
	list_first_entry(&(_db)->tdox_candidate_free_list,                     \
			 struct tdox_db_entry, tdox_node)
#define DB_TDOX_CANDIDATE_FREE_LIST_ADD_NODE(_db, _ent)                        \
	list_add_tail(&(_ent)->tdox_node, &(_db)->tdox_candidate_free_list)
#define DB_TDOX_MOVE_TO_CANDIDATE_FREE_LIST(_db, _ent)                         \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_candidate_free_list)
#define DB_TDOX_MOVE_TO_CANDIDATE_BUSY_LIST(_db, _ent)                         \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_candidate_busy_list)
#define DB_TDOX_SESS_UPDATE_FREE_LIST_GET_NODE(_db)                            \
	list_first_entry(&(_db)->tdox_sess_update_free_list,                   \
			 struct tdox_sess_update_entry, tdox_node)
#define DB_TDOX_SESS_UPDATE_FREE_LIST_ADD_NODE(_db, _ent)                      \
	list_add_tail(&(_ent)->tdox_node, &(_db)->tdox_sess_update_free_list)
#define DB_TDOX_MOVE_TO_SESS_UPDATE_FREE_LIST(_db, _ent)                       \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_sess_update_free_list)
#define DB_TDOX_MOVE_TO_SESS_UPDATE_BUSY_LIST(_db, _ent)                       \
	list_move_tail(&(_ent)->tdox_node, &(_db)->tdox_sess_update_busy_list)

/**
 * @brief Module database
 */
static struct smgr_tdox_db *db;

/**
 * @brief Tdox database statistics
 */
struct tdox_db_stats {
	/*! Free tdox sessions counter */
	atomic_t tdox_sess_free;
	/*! Free tdox session candidates counter */
	atomic_t tdox_free_candidates;
	/*! Tdox full counter */
	atomic_t tdox_sess_full_fail;
	/*! Candidate full counter */
	atomic_t tdox_candidate_full_fail;
	/*! Tdox session create args error */
	atomic_t tdox_create_args_err;
};

/**
 * @brief Tdox database definitions
 */
struct smgr_tdox_db {
	/*! turbodox enable */
	bool                  enable;
	/*! turbodox state machine array sram offset */
	u32                   tdox_state_arr_offset;
	/*! turbodox free sessions list */
	struct list_head      tdox_free_list;
	/*! turbodox busy sessions list */
	struct list_head      tdox_busy_list;
	/*! turbodox obsolete sessions list */
	struct list_head      tdox_obsolete_list;
	/*! turbodox free candidate sessions list */
	struct list_head      tdox_candidate_free_list;
	/*! turbodox busy candidate sessions list */
	struct list_head      tdox_candidate_busy_list;
	/*! turbodox free session update list */
	struct list_head      tdox_sess_update_free_list;
	/*! turbodox busy session update list */
	struct list_head      tdox_sess_update_busy_list;
	/*! tdox session update array */
	struct tdox_sess_update_entry  *tdox_sess_update;
	/*! tdox sessions array */
	struct tdox_db_entry  *tdox_sessions;
	/*! tdox host timer */
	struct timer_list     tdox_timer;
	/*! tdox host timer interval milliseconds  */
	u32                   tdox_timer_interval;
	/*! tdox db stats */
	struct tdox_db_stats  stats;
	/*! Tdox db lock */
	spinlock_t            tdox_lock;
};

s32 smgr_tdox_init(struct device *dev)
{
	struct tdox_sess_update_entry *tdox_sess_update_ent;
	struct tdox_db_entry *tdox_ent;
	u32 i;

	/* allocate database structure */
	db = devm_kzalloc(dev, sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("Failed to allocate database memory\n");
		return -ENOMEM;
	}

	spin_lock_init(&db->tdox_lock);

	/* init free sessions list */
	INIT_LIST_HEAD(&db->tdox_free_list);
	INIT_LIST_HEAD(&db->tdox_busy_list);
	INIT_LIST_HEAD(&db->tdox_obsolete_list);
	INIT_LIST_HEAD(&db->tdox_candidate_free_list);
	INIT_LIST_HEAD(&db->tdox_candidate_busy_list);
	INIT_LIST_HEAD(&db->tdox_sess_update_free_list);
	INIT_LIST_HEAD(&db->tdox_sess_update_busy_list);

	/* allocate sessions db entries */
	db->tdox_sessions =
		pp_devm_kvcalloc(dev, SMGR_TDOX_DB_NUM_ENTRIES,
				 sizeof(*db->tdox_sessions),
				 GFP_KERNEL);

	if (!db->tdox_sessions) {
		pr_err("Failed to allocate %u tdox sessions memory\n",
		       SMGR_TDOX_DB_NUM_ENTRIES);
		return -ENOMEM;
	}

	/* allocate sessions db entries */
	db->tdox_sess_update =
		pp_devm_kvcalloc(dev, SMGR_TDOX_DB_NUM_ENTRIES,
				 sizeof(*db->tdox_sess_update),
				 GFP_KERNEL);

	if (!db->tdox_sess_update) {
		pr_err("Failed to allocate %u tdox session update memory\n",
		       SMGR_TDOX_DB_NUM_ENTRIES);
		return -ENOMEM;
	}

	for (i = 0; i < SMGR_TDOX_DB_NUM_ENTRIES; i++) {
		tdox_ent = &db->tdox_sessions[i];
		tdox_sess_update_ent = &db->tdox_sess_update[i];
		/* init sessions lists nodes */
		INIT_LIST_HEAD(&tdox_ent->tdox_node);
		INIT_LIST_HEAD(&tdox_sess_update_ent->tdox_node);
		/* add the session to the free list */
		if (i < UC_MAX_TDOX_SESSIONS) {
			/* Free entries */
			tdox_ent->tdox_id = i;
			DB_TDOX_FREE_LIST_ADD_NODE(db, tdox_ent);
		} else {
			/* Candidates */
			tdox_ent->tdox_id = SMGR_INVALID_TDOX_SESS_ID;
			DB_TDOX_CANDIDATE_FREE_LIST_ADD_NODE(db, tdox_ent);
		}
		tdox_ent->sess_ent = NULL;
		DB_TDOX_SESS_UPDATE_FREE_LIST_ADD_NODE(db,
						       tdox_sess_update_ent);
	}
	atomic_set(&db->stats.tdox_sess_free, UC_MAX_TDOX_SESSIONS);
	atomic_set(&db->stats.tdox_free_candidates, UC_MAX_TDOX_SESSIONS);

	if (IS_ENABLED(TDOX_BYPASS))
		db->enable = false;
	else
		db->enable = true;

	return 0;
}

static u32 tdox_list_count(struct list_head *list)
{
	u32 cnt = 0;
	struct tdox_db_entry *ent;

	list_for_each_entry(ent, list, tdox_node)
		cnt++;

	return cnt;
}

bool smgr_tdox_enable_get(void)
{
	return db->enable;
}

void smgr_tdox_enable_set(bool enable)
{
	db->enable = enable;
	if (enable)
		smgr_tdox_late_init();
}

s32 smgr_tdox_record_create(struct sess_db_entry *sess_ent)
{
	s32 ret = 0;
	struct tdox_record_create_cmd cmd;
	struct tdox_db_entry *tdox_ent;

	if (unlikely(ptr_is_null(sess_ent)))
		return -EINVAL;

	tdox_ent = sess_ent->info.tdox_ent;

	if (SESS_IS_FLAG_OFF(sess_ent, SESS_FLAG_TDOX))
		return 0;

	cmd.info = tdox_ent->info;
	cmd.sess_id = tdox_ent->sess_ent->info.sess_id;
	cmd.tdox_id = tdox_ent->tdox_id;

	/* send command */
	ret = uc_egr_mbox_cmd_send(UC_CMD_TDOX_CREATE, 0, (const void *)&cmd,
				   sizeof(cmd), NULL, 0);
	if (ret) {
		pr_err("UC_CMD_TDOX_CREATE %u command failed\n",
		       tdox_ent->tdox_id);
		return ret;
	}

	pr_debug("Creating tdox record %u for session %u\n", cmd.tdox_id,
		 cmd.sess_id);
	return ret;
}

bool smgr_tdox_is_info_valid(struct pp_sess_create_args *args)
{
	u16 queue_phy;

	/* If not Tdox return */
	if (!test_bit(PP_SESS_FLAG_TDOX_BIT, &args->flags))
		return true;

	if (test_bit(PP_SESS_FLAG_LRO_INFO_BIT, &args->flags)) {
		pr_debug("LRO & TDOX flags cannot co-exist\n");
		atomic_inc(&db->stats.tdox_create_args_err);
		return false;
	}

	if (!args->rx || !args->tx) {
		pr_debug("TDOX requires in/eg packets\n");
		atomic_inc(&db->stats.tdox_create_args_err);
		return false;
	}

	if (!PKTPRS_IS_TCP(args->rx, HDR_OUTER)) {
		pr_debug("TDOX supports only TCP\n");
		atomic_inc(&db->stats.tdox_create_args_err);
		return false;
	}

	if (pktprs_first_frag(args->rx, HDR_OUTER) ||
	    pktprs_first_frag(args->rx, HDR_INNER) ||
	    pktprs_first_frag(args->tx, HDR_OUTER) ||
	    pktprs_first_frag(args->rx, HDR_INNER)) {
		pr_debug("TDOX does not support fragmented packets\n");
		atomic_inc(&db->stats.tdox_create_args_err);
		return false;
	}

	if (smgr_get_queue_phy_id(args->dst_q_high, &queue_phy)) {
		atomic_inc(&db->stats.tdox_create_args_err);
		return false;
	}

	return true;
}

static s32 tdox_set_db_entry(struct tdox_db_entry *tdox_ent,
			     struct sess_info *sess)
{
	u16 queue_phy;
	s32 ret;

	pr_debug("Setting tdox entry %u with queues [%u/%u]\n",
		 tdox_ent->tdox_id, sess->args->dst_q, sess->args->dst_q_high);

	/* Copy session parameters for future use */
	tdox_ent->info.supp_enable =
		SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_TDOX_SUPP_BIT);

	if (PKTPRS_IS_MULTI_LEVEL(SESS_RX_PKT(sess)))
		tdox_ent->info.threshold = TDOX_DEFAULT_AVG_PKT_SIZE_TUNN_THR;
	else
		tdox_ent->info.threshold = TDOX_DEFAULT_AVG_PKT_SIZE_THR;

	ret = smgr_get_queue_phy_id(sess->args->dst_q, &queue_phy);
	if (ret)
		return ret;
	tdox_ent->info.low_queue = queue_phy;

	ret = smgr_get_queue_phy_id(sess->args->dst_q_high, &queue_phy);
	if (ret)
		return ret;
	tdox_ent->info.high_queue = queue_phy;

	return 0;
}

static bool tdox_allocate_free_entry(struct tdox_db_entry **tdox_ent)
{
	/* If free list empty try inserting a candidate */
	if (list_empty(&db->tdox_free_list)) {
		atomic_inc(&db->stats.tdox_sess_full_fail);
		return false;
	}
	/* get the first entry from the tdox free list */
	*tdox_ent = DB_TDOX_FREE_LIST_GET_NODE(db);
	(*tdox_ent)->info.is_candidate = 0;
	DB_TDOX_MOVE_TO_BUSY_LIST(db, *tdox_ent);
	atomic_dec(&db->stats.tdox_sess_free);

	return true;
}

static bool tdox_allocate_candidate_entry(struct tdox_db_entry **tdox_ent)
{
	if (list_empty(&db->tdox_candidate_free_list)) {
		atomic_inc(&db->stats.tdox_candidate_full_fail);
		return false;
	}

	*tdox_ent = DB_TDOX_CANDIDATE_FREE_LIST_GET_NODE(db);
	(*tdox_ent)->info.is_candidate = 1;
	DB_TDOX_MOVE_TO_CANDIDATE_BUSY_LIST(db, *tdox_ent);
	atomic_dec(&db->stats.tdox_free_candidates);

	return true;
}

s32 smgr_tdox_sess_ent_prepare(struct sess_info *sess)
{
	struct tdox_db_entry *tdox_ent;
	struct sess_db_entry *sess_ent;
	s32 ret = 0;

	sess_ent = sess->db_ent;

	if (!db->enable) {
		sess_ent->info.tdox_ent = NULL;
		return 0;
	}

	if (SESS_ARGS_IS_FLAG_OFF(sess, PP_SESS_FLAG_TDOX_BIT)) {
		sess_ent->info.tdox_ent = NULL;
		return 0;
	}

	spin_lock_bh(&db->tdox_lock);

	if (tdox_allocate_free_entry(&tdox_ent)) {
		pr_debug("Allocated Tdox Entry %u. Session %u\n",
			 tdox_ent->tdox_id, sess_ent->info.sess_id);
		/* Set session db flags */
		SESS_FLAG_SET(sess_ent, SESS_FLAG_TDOX);
		if (SESS_ARGS_IS_FLAG_ON(sess, PP_SESS_FLAG_TDOX_SUPP_BIT))
			SESS_FLAG_SET(sess_ent, SESS_FLAG_TDOX_SUPP);
	} else {
		if (tdox_allocate_candidate_entry(&tdox_ent)) {
			pr_debug("Allocated Tdox Candidate. Session %u\n",
				 sess_ent->info.sess_id);
		} else {
			goto unlock;
		}
	}

	/* Link tdox entry with session entry and vice versa */
	sess_ent->info.tdox_ent = tdox_ent;
	tdox_ent->sess_ent = sess_ent;
	ret = tdox_set_db_entry(tdox_ent, sess);

	/* Use high destination queue */
	sess->args->dst_q = sess->args->dst_q_high;

unlock:
	spin_unlock_bh(&db->tdox_lock);

	return ret;
}

static s32 tdox_session_update(u32 tdox_id, u32 sess_id)
{
	struct pp_hw_si si;
	s32 ret = 0;

	pr_debug("Update session %u with tdox id %u\n", sess_id, tdox_id);

	/* Get session SI */
	if (unlikely(cls_session_si_get(sess_id, &si))) {
		pr_err("couldn't get si for session %u", sess_id);
		return -EINVAL;
	}
	ret = pp_si_fld_set(&si, SI_FLD_TDOX_FLOW, tdox_id);
	if (ret) {
		pr_err("couldn't set si fld tdox for session %u", sess_id);
		return ret;
	}
	ret = smgr_session_update(sess_id, &si);
	if (ret) {
		pr_err("couldn't set si fld tdox for session %u", sess_id);
		return ret;
	}

	return ret;
}
/**
 * @brief Helper function to detach tdox entry from an existing session
 */
static s32 tdox_detach(struct sess_db_entry *sess_ent, bool update_session,
		       bool remove_record)
{
	struct tdox_record_remove_cmd cmd;
	struct tdox_db_entry *tdox_ent;
	struct tdox_sess_update_entry *tdox_sess_update_ent;
	s32 ret = 0;

	SESS_FLAG_CLR(sess_ent, SESS_FLAG_TDOX);
	SESS_FLAG_CLR(sess_ent, SESS_FLAG_TDOX_SUPP);
	tdox_ent = sess_ent->info.tdox_ent;
	sess_ent->info.tdox_ent = NULL;
	tdox_ent->sess_ent = NULL;

	pr_debug("Detach tdox record %u from sess %u\n", tdox_ent->tdox_id,
		 sess_ent->info.sess_id);

	if (!update_session)
		goto remove_record;

	/* Add new session update entry to busy list */
	tdox_sess_update_ent = DB_TDOX_SESS_UPDATE_FREE_LIST_GET_NODE(db);
	tdox_sess_update_ent->sess_id = sess_ent->info.sess_id;
	tdox_sess_update_ent->tdox_id = SMGR_INVALID_TDOX_SESS_ID;
	DB_TDOX_MOVE_TO_SESS_UPDATE_BUSY_LIST(db, tdox_sess_update_ent);

remove_record:
	if (remove_record) {
		cmd.tdox_id = tdox_ent->tdox_id;
		/* send command */
		ret = uc_egr_mbox_cmd_send(UC_CMD_TDOX_REMOVE, 0,
					   (const void *)&cmd, sizeof(cmd),
					   NULL, 0);
		if (ret) {
			pr_err("UC_CMD_TDOX_REMOVE %u command failed\n",
			       tdox_ent->tdox_id);
			return ret;
		}
	}

	return ret;
}

void smgr_tdox_session_remove(struct tdox_db_entry *ent)
{
	enum tdox_uc_state state;
	s32 ret;

	/* Check if session is linked with tdox */
	if (!ent)
		return;

	spin_lock_bh(&db->tdox_lock);
	if (ent->info.is_candidate) {
		pr_debug("Removing candidate session %u\n",
			 ent->sess_ent->info.sess_id);
		/* Detach, no need to update session as it is a candidate */
		tdox_detach(ent->sess_ent, false, false);
		/* Update candidates db */
		DB_TDOX_MOVE_TO_CANDIDATE_FREE_LIST(db, ent);
		atomic_inc(&db->stats.tdox_free_candidates);
		goto unlock;
	} else if (SESS_IS_FLAG_OFF(ent->sess_ent, SESS_FLAG_TDOX)) {
		pr_err("Session %u with Tdox %u with no flag\n",
		       ent->sess_ent->info.sess_id, ent->tdox_id);
	}

	ret = uc_tdox_state_get(ent->tdox_id, &state);
	if (ret) {
		pr_err("Failed fetching tdox id %u state\n", ent->tdox_id);
		goto unlock;
	}

	switch (state) {
	case TDOX_NONQUALIFIED_UC_INACTIVE:
		pr_debug("Removing nonqualified tdox %u. session %u\n",
			 ent->tdox_id, ent->sess_ent->info.sess_id);
		/* Detach, do not update session, do not send delete cmd */
		tdox_detach(ent->sess_ent, false, false);
		DB_TDOX_MOVE_TO_FREE_LIST(db, ent);
		atomic_inc(&db->stats.tdox_sess_free);
		break;
	case TDOX_NEW:
	case TDOX_QUALIFIED_UC_INACTIVE:
	case TDOX_QUALIFIED_UC_ACTIVE:
		pr_debug("Obsolating qualified/new tdox %u. session %u\n",
			 ent->tdox_id, ent->sess_ent->info.sess_id);
		/* Detach, do not update session, send delete cmd */
		tdox_detach(ent->sess_ent, false, true);
		/* In case uc is active change to obsolete state */
		DB_TDOX_MOVE_TO_OBSOLETE_LIST(db, ent);
		break;
	case TDOX_NONQUALIFIED_UC_ACTIVE:
		pr_debug("Obsolating NONQUALIFIED ACTIVE tdox %u. session %u\n",
			 ent->tdox_id, ent->sess_ent->info.sess_id);
		/* Detach, do not update session,do not send delete cmd */
		tdox_detach(ent->sess_ent, false, false);
		/* In case uc is active change to obsolete state */
		DB_TDOX_MOVE_TO_OBSOLETE_LIST(db, ent);
		break;
	default:
		pr_err("Unknown Tdox state %u\n", state);
	}
unlock:
	spin_unlock_bh(&db->tdox_lock);
}

/**
 * @brief Helper function to attach tdox entry to an existing session
 */
static s32 tdox_attach(struct sess_db_entry *sess_ent,
		       struct tdox_db_entry *tdox_ent)
{
	struct tdox_sess_update_entry *tdox_sess_update_ent;
	s32 ret = 0;

	pr_debug("Attach tdox record %u to sess %u\n", tdox_ent->tdox_id,
		 sess_ent->info.sess_id);

	SESS_FLAG_SET(sess_ent, SESS_FLAG_TDOX);
	if (tdox_ent->info.supp_enable)
		SESS_FLAG_SET(sess_ent, SESS_FLAG_TDOX_SUPP);

	sess_ent->info.tdox_ent = tdox_ent;

	/* Add new session update entry to busy list */
	tdox_sess_update_ent = DB_TDOX_SESS_UPDATE_FREE_LIST_GET_NODE(db);
	tdox_sess_update_ent->sess_id = sess_ent->info.sess_id;
	tdox_sess_update_ent->tdox_id = tdox_ent->tdox_id;
	DB_TDOX_MOVE_TO_SESS_UPDATE_BUSY_LIST(db, tdox_sess_update_ent);

	/* Create tdox record */
	ret = smgr_tdox_record_create(sess_ent);
	if (ret) {
		pr_err("Error creating tdox record %u\n", tdox_ent->tdox_id);
		return -EINVAL;
	}

	return ret;
}

static void smgr_tdox_nonqualified_process(void)
{
	struct tdox_db_entry *ent, *tmp;
	enum tdox_uc_state state;
	s32 ret;

	/* Iterate through busy list and manage any non qualified entry */
	list_for_each_entry_safe(ent, tmp, &db->tdox_busy_list, tdox_node) {
		ret = uc_tdox_state_get(ent->tdox_id, &state);
		if (ret) {
			pr_err("Failed fetching tdox id %u state\n",
			       ent->tdox_id);
			return;
		}

		if (state == TDOX_NONQUALIFIED_UC_INACTIVE) {
			pr_debug("Detach NONQUALIFIED INACT tdox %u. sess %u\n",
				 ent->tdox_id, ent->sess_ent->info.sess_id);
			/* Detach Tdox from session */
			tdox_detach(ent->sess_ent, true, false);
			/* Update Tdox db */
			DB_TDOX_MOVE_TO_FREE_LIST(db, ent);
			atomic_inc(&db->stats.tdox_sess_free);
		}
	}
}

static void smgr_tdox_obsolete_process(void)
{
	struct tdox_db_entry *obsolete_ent, *tmp;
	u32 cnt = 0;

	/* Iterate through obsolete list and free all entries */
	list_for_each_entry_safe(obsolete_ent, tmp,
				 &db->tdox_obsolete_list, tdox_node) {
		/* Remove Tdox from session if active */
		if (obsolete_ent->sess_ent)
			tdox_detach(obsolete_ent->sess_ent, true, false);

		cnt++;
		/* Update Tdox db */
		DB_TDOX_MOVE_TO_FREE_LIST(db, obsolete_ent);
		atomic_inc(&db->stats.tdox_sess_free);
	}
	if (cnt)
		pr_debug("Obsoletes Freed: %u\n", cnt);
}

static void smgr_tdox_candidate_process(void)
{
	s32 ret;
	struct tdox_db_entry *cand_ent, *new_ent, *tmp;
	u32 cnt = 0;

	/* Iterate through busy candidate list and move them to busy list */
	list_for_each_entry_safe(cand_ent, tmp,
				 &db->tdox_candidate_busy_list,
				 tdox_node) {
		if (list_empty(&db->tdox_free_list))
			return;

		cnt++;

		/* Get a free entry and attach candidate session */
		new_ent = DB_TDOX_FREE_LIST_GET_NODE(db);
		/* Add new entry to busy list */
		DB_TDOX_MOVE_TO_BUSY_LIST(db, new_ent);
		atomic_dec(&db->stats.tdox_sess_free);

		/* Copy session info from candidate entry */
		new_ent->info = cand_ent->info;
		new_ent->sess_ent = cand_ent->sess_ent;

		/* Update candidates db */
		cand_ent->sess_ent = NULL;
		DB_TDOX_MOVE_TO_CANDIDATE_FREE_LIST(db, cand_ent);
		atomic_inc(&db->stats.tdox_free_candidates);

		/* Attach new tdox entry with existing session */
		ret = tdox_attach(new_ent->sess_ent, new_ent);
		if (ret) {
			pr_err("Error attaching tdox record %u\n",
			       new_ent->tdox_id);
			tdox_detach(new_ent->sess_ent, true, false);
			DB_TDOX_MOVE_TO_FREE_LIST(db, new_ent);
			atomic_inc(&db->stats.tdox_sess_free);
		}
	}

	if (cnt)
		pr_debug("Candidates Activated: %u\n", cnt);
}

/**
 * @brief tdox timer handler
 * @param t the timer object
 */
static void tdox_cb(struct timer_list *timer)
{
	struct tdox_sess_update_entry *ent, *tmp;
	unsigned long timer_delay_jiffies;

	spin_lock_bh(&db->tdox_lock);

	/* Busy => Free */
	if (!list_empty(&db->tdox_busy_list))
		smgr_tdox_nonqualified_process();

	/* Candidate => Busy */
	if (!list_empty(&db->tdox_candidate_busy_list) &&
	    !list_empty(&db->tdox_free_list))
		smgr_tdox_candidate_process();

	/* Obsolete => Free
	 * Obsolete process is located last in order to make
	 * sure we use the freed entry only on the next timer tick
	 */
	if (!list_empty(&db->tdox_obsolete_list))
		smgr_tdox_obsolete_process();

	spin_unlock_bh(&db->tdox_lock);

	/* Iterate through busy list and update sessions with correct tdox id */
	list_for_each_entry_safe(ent, tmp, &db->tdox_sess_update_busy_list,
				 tdox_node) {
		tdox_session_update(ent->tdox_id, ent->sess_id);
		DB_TDOX_MOVE_TO_SESS_UPDATE_FREE_LIST(db, ent);
	}

	/* Restart timer */
	timer_delay_jiffies =
		jiffies + msecs_to_jiffies(db->tdox_timer_interval);
	mod_timer(&db->tdox_timer, timer_delay_jiffies);
}

s32 smgr_tdox_late_init(void)
{
	unsigned long timer_delay_jiffies;

	if (!db->enable)
		return -EPERM;

	spin_lock_bh(&db->tdox_lock);
	timer_setup(&db->tdox_timer, tdox_cb, 0);
	db->tdox_timer_interval = SMGR_TDOX_TIMER_INTERVAL_MS;

	timer_delay_jiffies = jiffies +
		msecs_to_jiffies(db->tdox_timer_interval);
	mod_timer(&db->tdox_timer, timer_delay_jiffies);

	spin_unlock_bh(&db->tdox_lock);

	return 0;
}

s32 smgr_tdox_stats_get(struct smgr_tdox_stats *stats)
{
	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	spin_lock_bh(&db->tdox_lock);
	/** TBD add command
	 * ret = uc_egr_mbox_cmd_send(UC_CMD_TDOX_STATS,0, NULL, 0,
	 *                            stats, sizeof(*stats));
	 * if (unlikely(ret)) {
	 *      pr_err("failed to get tdox uc cpu %u  counters\n", cid);
	 *      stats->uc_rx_pkt = 0;
	 *      stats->uc_tx_pkt = 0;
	 * }
	 **/

	stats->uc_rx_pkt = 0;
	stats->uc_tx_pkt = 0;

	stats->free_list_cnt = tdox_list_count(&db->tdox_free_list);
	stats->busy_list_cnt = tdox_list_count(&db->tdox_busy_list);
	stats->obsolete_list_cnt = tdox_list_count(&db->tdox_obsolete_list);
	stats->cand_free_list_cnt =
		tdox_list_count(&db->tdox_candidate_free_list);
	stats->cand_busy_list_cnt =
		tdox_list_count(&db->tdox_candidate_busy_list);
	stats->sess_update_free_list_cnt =
		tdox_list_count(&db->tdox_sess_update_free_list);
	stats->sess_update_busy_list_cnt =
		tdox_list_count(&db->tdox_sess_update_busy_list);
	stats->tdox_sess_free = atomic_read(&db->stats.tdox_sess_free);
	stats->tdox_free_candidates =
		atomic_read(&db->stats.tdox_free_candidates);
	stats->tdox_sess_full_fail =
		atomic_read(&db->stats.tdox_sess_full_fail);
	stats->tdox_candidate_full_fail =
		atomic_read(&db->stats.tdox_candidate_full_fail);
	stats->tdox_create_args_err =
		atomic_read(&db->stats.tdox_create_args_err);

	spin_unlock_bh(&db->tdox_lock);

	return 0;
}

void smgr_tdox_exit(void)
{
	spin_lock_bh(&db->tdox_lock);
	del_timer(&db->tdox_timer);
	spin_unlock_bh(&db->tdox_lock);

	kfree(db);
	db = NULL;
}
