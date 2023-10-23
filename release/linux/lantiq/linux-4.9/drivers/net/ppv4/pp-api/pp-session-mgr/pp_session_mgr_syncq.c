/*
 * Description: PP session manager synchronization queues module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SESS_MGR_SQ]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>     /* For pr_err/pr_debug/...   */
#include <linux/types.h>      /* For standard types        */
#include <linux/spinlock.h>   /* For DB lock               */
#include <linux/errno.h>      /* For the EINVAL/EEXIST/... */
#include <linux/bug.h>        /* For WARN()                */
#include <linux/list.h>       /* For free/act lists        */
#include <linux/timer.h>      /* For sync/done timers      */
#include <linux/jiffies.h>    /* For jiffies               */
#include <linux/pp_qos_api.h> /* For QoS manager APIs      */

#include "checker.h"          /* For syncq HW table        */
#include "pp_qos_utils.h"     /* For QoS syncq APIs        */
#include "pp_common.h"
#include "pp_session_mgr.h"
#include "pp_session_mgr_internal.h"

/**
 * @define SQ_SYNC_TO
 * @brief timeout in micro seconds
 */
#define SQ_SYNC_TO   100000

/**
 * @define SQ_LSPP_TO
 * @brief timeout in micro seconds
 * @note LSPP: Last Slow Path Packet
 */
#define SQ_LSPP_TO   10000

/**
 * @define SQ_DONE_TO
 * @brief timeout in micro seconds
 */
#define SQ_DONE_TO   300000

/**
 * @define SQ_DFLT_QLEN
 * @brief maximum sync queue length
 */
#define SQ_DFLT_QLEN 0x1000

/**
 * @define SQ_PR
 * @brief printing a synch queue entry
 */
#define SQ_PR(pr_func, sq) \
	pr_func("SQ[%02hhu] SESS[%05u] Q_ID[%04u] DST_Q[%04u] STATE[%s]\n", \
		(sq)->id, (sq)->session, (sq)->queue_id, (sq)->dst_queue_id, \
		__sq_state_str((sq)->state))

/**
 * @define SQ_FOREACH_ACTIVE_ENTRY
 * @brief go over all the active sq entries from db
 */
#define SQ_FOREACH_ACTIVE_ENTRY(db, sq) \
	list_for_each_entry((sq), &(db)->act_list, node)

/**
 * @define SQ_FREE_ENTRY_SET
 * @brief move the sq entry to the free list
 */
#define SQ_FREE_ENTRY_SET(db, sq) \
	list_move_tail(&(sq)->node, &(db)->free_list)

/**
 * @define SQ_IS_DB_FULL
 * @brief check if the sq free list is emty
 */
#define SQ_IS_DB_FULL(db) \
	list_empty(&(db)->free_list)

/**
 * @define SQ_FREE_ENTRY_GET
 * @brief get the first free sq entry
 */
#define SQ_FREE_ENTRY_GET(db) \
	list_first_entry(&(db)->free_list, struct sq_entry, node)

/**
 * @define SQ_ACTIVE_ENTRY_SET
 * @brief move the sq entry to the active list
 */
#define SQ_ACTIVE_ENTRY_SET(db, sq) \
	list_move(&(sq)->node, &(db)->act_list)

/**
 * @define SQ_FREE_ENTRY_INIT
 * @brief add the sq entry to the free list
 */
#define SQ_FREE_ENTRY_INIT(db, sq) \
	list_add_tail(&(sq)->node, &(db)->free_list)

/**
 * @enum sq_state
 * @brief this is the sync queue state machine, this enum
 *        described the syncq entry life cyncle
 */
enum sq_state {
	/*! syncq is not attached to any session, located in the free list */
	SQ_STATE_IDLE,

	/*! syncq was allocated but the timer not activated */
	SQ_STATE_ALLOCATED,

	/*! syncq is pending on lsp packet or "sync timer" */
	SQ_STATE_STARTED,

	/*! lspp sent from CPU, syncq is pending on "sync timer" */
	SQ_STATE_LSPP_SENT,

	/*! syncq process was done and pending on "done timer" */
	SQ_STATE_SYNC_DONE,

	SQ_STATE_NUM,
};

/**
 * @struct sq_entry
 * @brief sync queue entry
 */
struct sq_entry {
	/*! entry id */
	u8                id;

	/*! entry state (enum sq_state) */
	enum sq_state     state;

	/*! attached session id */
	u32               session;

	/*! the synchronization queue id */
	u32               queue_id;

	/*! the synchronization queue (phy) */
	u32               queue_phy;

	/*! the original destination queue id */
	u32               dst_queue_id;

	/*! timer for "sync" event*/
	struct timer_list sync_timer;

	/*! timer for "done" event*/
	struct timer_list done_timer;

	/*! entry list node */
	struct list_head  node;
};

/**
 * @struct sq_database
 * @brief sync queue module database
 */
struct sq_database {
	/*! number of synch queues supported */
	u32                  n_sq;

	/*! locks database and HW accesses */
	spinlock_t           lock;

	/*! free entries list */
	struct list_head     free_list;

	/*! active entries list */
	struct list_head     act_list;

	/*! sync queues array */
	struct sq_entry      *sq;

	/*! sync queues stats */
	struct smgr_sq_stats stats;

	/*! QoS device for using the QoS APIs */
	struct pp_qos_dev    *qdev;

	/*! sync event timeout */
	u32                  sq_sync_to;

	/*! last slow path packet event timeout */
	u32                  sq_lspp_to;

	/*! done event timeout */
	u32                  sq_done_to;

	/*! maximum sync queue length (max packets allowed) */
	u32                  sq_qlen;
};

static const char *sq_state_str[SQ_STATE_NUM + 1] = {
	[SQ_STATE_IDLE]      = "IDLE",
	[SQ_STATE_ALLOCATED] = "ALLOCATED",
	[SQ_STATE_STARTED]   = "STARTED",
	[SQ_STATE_LSPP_SENT] = "LSPP_SENT",
	[SQ_STATE_SYNC_DONE] = "SYNC_DONE",
	[SQ_STATE_NUM]       = "INVALID_STATE",
};

/**
 * @brief Module database
 */
static struct sq_database *db;

static struct sq_entry *__sq_get_by_session(u32 session);
static struct sq_entry *__sq_get_by_dstq(u32 dst_queue_id);
static inline void      __sq_free(struct sq_entry *sq);
static void             __sq_del(struct sq_entry *sq);
static s32              __sq_prepare(u32 id);
static void             __sq_stats_update(struct sq_entry *sq);
static void             __sq_next_state(struct sq_entry *sq);
static void             __sq_timers_setup(struct sq_entry *sq);
static void             __sq_queue_connect(struct sq_entry *sq);
static s32              __sq_queue_disconnect(struct sq_entry *sq);
static s32              __sq_queue_alloc(struct sq_entry *sq);

/*****************************************************************************/
/*                      internal sq operations                               */
/*****************************************************************************/
/**
 * @brief Acquire syncq lock
 */
static inline void __sq_lock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	/* we are not allowed to run in irq context */
	WARN(in_irq(), "smgr syncq shouldn't be used from irq context\n");
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release syncq lock
 */
static inline void __sq_unlock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	spin_unlock_bh(&db->lock);
}

/**
 * @brief Get the syncq state string
 * @param s syncq state
 * @return const char* syncq state string
 */
static inline const char *__sq_state_str(enum sq_state s)
{
	if (s < SQ_STATE_NUM)
		return sq_state_str[s];

	return sq_state_str[SQ_STATE_NUM];
}

/**
 * @brief printing a synch queue entry for debug
 * @param sq syncq entry
 */
static inline void __sq_debug(struct sq_entry *sq)
{
	SQ_PR(pr_debug, sq);
}

/**
 * @brief printing a synch queue entry for debug
 * @param sq syncq entry
 */
static inline void __sq_print(struct sq_entry *sq)
{
	SQ_PR(pr_info, sq);
}

/**
 * @brief printing a timer callback event for debug
 */
static inline void __sq_timer_cb_debug(void)
{
	pr_debug("TIME[%u] timer callback\n", jiffies_to_usecs(jiffies));
}

static inline void __sq_timer_restart_debug(u32 tout)
{
	pr_debug("TIME[%u] start timer for %u uSEC\n",
		 jiffies_to_usecs(jiffies), tout);
}

/**
 * @brief restart the timer for synch timeout
 * @param sq syncq entry
 */
static inline void __sq_sync_tout_restart(struct sq_entry *sq)
{
	__sq_timer_restart_debug(db->sq_sync_to);
	mod_timer(&(sq)->sync_timer,
		  jiffies + usecs_to_jiffies(db->sq_sync_to));
}

/**
 * @brief restart the timer for lspp timeout
 * @param sq syncq entry
 */
static inline void __sq_lspp_tout_restart(struct sq_entry *sq)
{
	__sq_timer_restart_debug(db->sq_lspp_to);
	mod_timer(&(sq)->sync_timer,
		  jiffies + usecs_to_jiffies(db->sq_lspp_to));
}

/**
 * @brief restart the timer for done timeout
 * @param sq syncq entry
 */
static inline void __sq_done_tout_restart(struct sq_entry *sq)
{
	__sq_timer_restart_debug(db->sq_done_to);
	mod_timer(&(sq)->done_timer,
		  jiffies + usecs_to_jiffies(db->sq_done_to));
}

/**
 * @brief Get the syncq by the session id
 * @param session session id
 * @return struct sq_entry* syncq entry
 */
static struct sq_entry *__sq_get_by_session(u32 session)
{
	struct sq_entry *sq;

	pr_debug("session %u\n", session);
	SQ_FOREACH_ACTIVE_ENTRY(db, sq) {
		if (sq->session == session)
			return sq;
	}

	return NULL;
}

/**
 * @brief Get the syncq by the dst queue id
 * @param dst_queue_id dst queue id
 * @return struct sq_entry* syncq entry
 */
static struct sq_entry *__sq_get_by_dstq(u32 dst_queue_id)
{
	struct sq_entry *sq;

	pr_debug("dst_queue_id %u\n", dst_queue_id);
	SQ_FOREACH_ACTIVE_ENTRY(db, sq) {
		if (sq->dst_queue_id == dst_queue_id)
			return sq;
	}

	return NULL;
}

/**
 * @brief Release the syncq entry
 * @note called under lock
 * @param sq syncq entry
 */
static inline void __sq_free(struct sq_entry *sq)
{
	__sq_debug(sq);
	/* reset the sync queue properties */
	sq->state        = SQ_STATE_IDLE;
	sq->session      = 0;
	sq->dst_queue_id = 0;
	/* move the entry to the free list */
	SQ_FREE_ENTRY_SET(db, sq);
}

/**
 * @brief Delete the syncq entry
 * @note this API replacing the syncq entry to a FREE entry
 * @param sq syncq entry
 */
static void __sq_del(struct sq_entry *sq)
{
	__sq_debug(sq);
	switch (sq->state) {
	case SQ_STATE_STARTED:
		sq->state = SQ_STATE_LSPP_SENT;
	case SQ_STATE_LSPP_SENT:
		__sq_next_state(sq);
	case SQ_STATE_SYNC_DONE:
		/* let the session handle to use new sq */
		sq->session = U32_MAX;
		/* nothing to do, let the handler to do the job */
		break;
	default:
		/* SQ_STATE_IDLE */
		/* SQ_STATE_ALLOCATED */
		pr_err("sq %hhu invalid state %s\n",
		       sq->id, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		break;
	}
}

/**
 * @brief Prepare syncq entry
 * @param id synchq entry id
 * @return s32 0 on success, error code otherwise
 */
static s32 __sq_prepare(u32 id)
{
	struct sq_entry *sq = &db->sq[id];
	s32 ret;

	pr_debug("id %u\n", id);
	/* set syncq id */
	sq->id = id;
	/* setup the timers */
	__sq_timers_setup(sq);
	/* init syncq list node */
	INIT_LIST_HEAD(&sq->node);
	/* allocate the QoS resources */
	ret = __sq_queue_alloc(sq);
	if (unlikely(ret)) {
		pr_err("failed to allocate sync queue %u\n", id);
		return ret;
	}
	/* sync queue is ready - initiate the state */
	sq->state = SQ_STATE_IDLE;
	/* add the syncq to the free list */
	SQ_FREE_ENTRY_INIT(db, sq);

	return 0;
}

static void __sq_stats_update(struct sq_entry *sq)
{
	struct pp_qos_queue_stat stat;

	__sq_debug(sq);
	stat.reset = 1;
	/* get the sync queue statistics */
	if (unlikely(pp_qos_queue_stat_get(db->qdev, sq->queue_id, &stat))) {
		pr_err("failed to get syncq (%u) statistics\n", sq->queue_id);
		return;
	}

	db->stats.packets_accepted  += stat.total_packets_accepted;
	db->stats.bytes_accepted    += stat.total_bytes_accepted;
	db->stats.packets_dropped   += stat.total_packets_dropped;
	db->stats.bytes_dropped     += stat.total_bytes_dropped;
}

/**
 * @brief Take the syncq entry to the next state
 * @param sq syncq entry
 */
static void __sq_next_state(struct sq_entry *sq)
{
	__sq_debug(sq);
	switch (sq->state) {
	case SQ_STATE_IDLE:
		/* the sq was allocated to a new session:
		 * update the new state
		 * mepped all the matched packets to the syncq by updating
		 * the checker synch queues table.
		 */
		sq->state = SQ_STATE_ALLOCATED;
		chk_sq_entry_enable(sq->id, sq->session, sq->queue_phy);
		db->stats.allocated++;
		db->stats.active++;
		break;

	case SQ_STATE_ALLOCATED:
		/* the session was created in HW:
		 * update the new state
		 * start the timer for letting the lspp (Last Slow Path Packet)
		 * the chance to be captured in the CPU tx
		 */
		sq->state = SQ_STATE_STARTED;
		__sq_sync_tout_restart(sq);
		break;

	case SQ_STATE_STARTED:
		/* lspp (Last Slow Path Packet) recieved:
		 * restart the timer for "sq_lspp_to" time,
		 * let the last packet the chance to be enqueued
		 * update the new state
		 */
		del_timer(&sq->sync_timer);
		__sq_lspp_tout_restart(sq);
		sq->state = SQ_STATE_LSPP_SENT;
		break;

	case SQ_STATE_LSPP_SENT:
		/* time to start the synchronization process:
		 * stop the "sync" timer,
		 * update the QoS tree to connect the syncq
		 * mepped all the matched packets to the original dst queue
		 * start the "done" timeout
		 * update the new state
		 */
		del_timer(&sq->sync_timer);
		__sq_queue_connect(sq);
		chk_sq_entry_disable(sq->id);
		sq->state = SQ_STATE_SYNC_DONE;
		__sq_done_tout_restart(sq);
		break;

	case SQ_STATE_SYNC_DONE:
		/* start the revert process:
		 * get sync queue statisics
		 * try to revert the the syncq
		 * if the operation failed / queue is not empty yet,
		 * then, restart the timer and try next time
		 * release the sq entry
		 */
		__sq_stats_update(sq);
		if (unlikely(__sq_queue_disconnect(sq))) {
			db->stats.err_q_disconnect_failed++;
			__sq_done_tout_restart(sq);
			break;
		}
		__sq_free(sq);
		db->stats.freed++;
		db->stats.active--;
		break;

	default:
		pr_err("sq %hhu state %s\n", sq->id, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		return;
	}
	__sq_debug(sq);
}

/*****************************************************************************/
/*                       QoS queues operations                               */
/*****************************************************************************/
/**
 * @brief Synchronized the syncq, connect the syncq to the QoS
 *        tree
 * @param sq syncq entry
 */
static void __sq_queue_connect(struct sq_entry *sq)
{
	__sq_debug(sq);
	if (unlikely(qos_sync_queue_connect(db->qdev, sq->queue_id,
					    sq->dst_queue_id))) {
		pr_crit("failed to synchronized the queues in QoS\n");
	}
}

/**
 * @brief Revert the syncq, disconnect the syncq from the QoS
 *        tree
 * @param sq syncq entry
 * @return s32 0 on success, error code otherwise
 */
static s32 __sq_queue_disconnect(struct sq_entry *sq)
{
	__sq_debug(sq);
	return qos_sync_queue_disconnect(db->qdev, sq->queue_id);
}

/**
 * @brief Allocate a QoS queue for the syncq entry
 * @param sq syncq entry
 * @return s32 0 on success, error code otherwise
 */
static s32 __sq_queue_alloc(struct sq_entry *sq)
{
	struct pp_qos_queue_conf conf;
	s32 ret;

	__sq_debug(sq);
	/* allocate the queue */
	ret = pp_qos_queue_allocate(db->qdev, &sq->queue_id);
	if (unlikely(ret)) {
		pr_err("failed to allocate QoS queue\n");
		return ret;
	}

	pp_qos_queue_conf_set_default(&conf);
	conf.wred_max_allowed = db->sq_qlen;
	ret = qos_sync_queue_add(db->qdev, sq->queue_id, &sq->queue_phy, &conf);
	if (unlikely(ret)) {
		pr_err("failed to add QoS synch queue %u\n", sq->queue_id);
		return ret;
	}

	pr_debug("sync queue added to QoS, node-id[%u], qid[%u]\n",
		 sq->queue_id, sq->queue_phy);

	return 0;
}

/*****************************************************************************/
/*                       timer callback routines                             */
/*****************************************************************************/

/**
 * @brief Done timer handler
 * @param timer the timer object
 */
static void __sq_done_cb(struct timer_list *timer)
{
	struct sq_entry *sq = from_timer(sq, timer, done_timer);

	__sq_timer_cb_debug();
	__sq_debug(sq);
	__sq_lock();
	if (sq->state != SQ_STATE_SYNC_DONE) {
		pr_err("sq %hhu invalid state %s for done event\n",
		       sq->id, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		goto unlock;
	}
	__sq_next_state(sq);

unlock:
	__sq_unlock();
}

/* called from timer only!! */
/**
 * @brief sync timer handler
 * @param t the timer object
 */
static void __sq_sync_cb(struct timer_list *timer)
{
	struct sq_entry *sq = from_timer(sq, timer, sync_timer);

	__sq_timer_cb_debug();
	__sq_debug(sq);
	__sq_lock();
	switch (sq->state) {
	case SQ_STATE_STARTED:
		/* skip the "started" state */
		sq->state = SQ_STATE_LSPP_SENT;
		db->stats.lspp_timeout_events++;
	case SQ_STATE_LSPP_SENT:
		__sq_next_state(sq);
		break;
	default:
		pr_err("sq %hhu invalid state %s for sync event\n",
		       sq->id, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		break;
	}
	__sq_unlock();
}

/**
 * @brief Setup the syncq timers
 * @param sq syncq entry
 */
static void __sq_timers_setup(struct sq_entry *sq)
{
	timer_setup(&sq->sync_timer, __sq_sync_cb, 0);
	timer_setup(&sq->done_timer, __sq_done_cb, 0);
}

/*****************************************************************************/
/*                           external APIs                                   */
/*****************************************************************************/
s32 sq_alloc(u32 session, u32 dst_queue_id)
{
	struct sq_entry *sq;
	s32 ret = 0;

	pr_debug("session %u, dst_queue_id %u\n", session, dst_queue_id);

	/* lock the syncq db */
	__sq_lock();

	/* if dst queue already hold an entry, abort */
	sq = __sq_get_by_dstq(dst_queue_id);
	if (sq) {
		pr_debug("syncq exist for dst queue %u, abort\n", dst_queue_id);
		ret = -EEXIST;
		goto unlock;
	}

	/* if no available entries, abort */
	if (SQ_IS_DB_FULL(db)) {
		pr_debug("no available sync queue, abort\n");
		ret = -ENOSPC;
		goto unlock;
	}
	/* get a free syncq entry */
	sq = SQ_FREE_ENTRY_GET(db);
	SQ_ACTIVE_ENTRY_SET(db, sq);
	/* set the session info */
	sq->session      = session;
	sq->dst_queue_id = dst_queue_id;
	/* go to next state --> SQ_STATE_ALLOCATED */
	__sq_next_state(sq);

unlock:
	__sq_unlock();
	return ret;
}

s32 smgr_sq_alloc(struct sess_info *sess)
{
	if (!test_bit(SESS_FLAG_SYNCQ, &sess->db_ent->info.flags))
		return 0;

	return sq_alloc(sess->db_ent->info.sess_id, sess->args->dst_q);
}

s32 sq_start(u32 session)
{
	struct sq_entry *sq;
	s32 ret = 0;

	pr_debug("session %u\n", session);

	__sq_lock();
	/* check if sq entry was allocated for this session */
	sq = __sq_get_by_session(session);
	if (!sq) {
		/* no attached sync queue, abort */
		ret = -ENOENT;
		goto unlock;
	}

	if (unlikely(sq->state != SQ_STATE_ALLOCATED)) {
		pr_err("invalid sq %hhu state for session %u, state %s\n",
		       sq->id, session, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		ret = -EPIPE;
		goto unlock;
	}
	/* go to next state --> SQ_STATE_STARTED */
	__sq_next_state(sq);

unlock:
	__sq_unlock();
	return ret;
}

s32 smgr_sq_start(struct sess_info *sess)
{
	if (!test_bit(SESS_FLAG_SYNCQ, &sess->db_ent->info.flags))
		return 0;

	return sq_start(sess->db_ent->info.sess_id);
}

s32 sq_del(u32 session)
{
	struct sq_entry *sq;
	s32 ret = 0;

	pr_debug("session %u\n", session);

	__sq_lock();
	sq = __sq_get_by_session(session);
	if (!sq) {
		/* no attached sync queue, abort */
		pr_debug("no sync queue for session %u, abort\n", session);
		ret = -ENOENT;
		goto unlock;
	}
	__sq_del(sq);
unlock:
	__sq_unlock();
	return ret;
}

s32 smgr_sq_del(struct sess_db_entry *ent)
{
	if (!test_bit(SESS_FLAG_SYNCQ, &ent->info.flags))
		return 0;

	return sq_del(ent->info.sess_id);
}

void smgr_sq_lspp_rcv(u32 session)
{
	struct sq_entry *sq;

	pr_debug("session %u\n", session);

	__sq_lock();
	/* check if sq entry exist for this session */
	sq = __sq_get_by_session(session);
	if (unlikely(!sq))
		goto unlock;

	switch (sq->state) {
	case SQ_STATE_ALLOCATED:
		sq->state = SQ_STATE_STARTED;
	case SQ_STATE_STARTED:
		/* this is the normal flow,
		 * go to next state --> SQ_STATE_LSPP_SENT
		 */
		__sq_next_state(sq);
		break;
	case SQ_STATE_SYNC_DONE:
		/* too late, wait for done event */
		break;
	default:
		pr_err("sq %hhu invalid state %s\n",
		       sq->id, __sq_state_str(sq->state));
		db->stats.err_invalid_state++;
		goto unlock;
	}

unlock:
	__sq_unlock();
}

s32 smgr_sq_dbg_stats_get(struct smgr_sq_stats *stats, bool reset)
{
	u32 active, total;

	if (stats)
		memcpy(stats, &db->stats, sizeof(*stats));
	if (reset) {
		active = db->stats.active;
		total  = db->stats.total;
		memset(&db->stats, 0, sizeof(db->stats));
		db->stats.active = active;
		db->stats.total  = total;
	}

	return 0;
}

void smgr_sq_dbg_dump(void)
{
	u32 i;

	for (i = 0; i < db->n_sq; i++)
		__sq_print(&db->sq[i]);
}

void smgr_sq_dbg_sync_tout_get(u32 *tout)
{
	if (unlikely(ptr_is_null(tout)))
		return;

	*tout = db->sq_sync_to;
}

void smgr_sq_dbg_sync_tout_set(u32 tout)
{
	db->sq_sync_to = tout;
}

void smgr_sq_dbg_done_tout_get(u32 *tout)
{
	if (unlikely(ptr_is_null(tout)))
		return;

	*tout = db->sq_done_to;
}

void smgr_sq_dbg_done_tout_set(u32 tout)
{
	db->sq_done_to = tout;
}

void smgr_sq_dbg_lspp_tout_get(u32 *tout)
{
	if (unlikely(ptr_is_null(tout)))
		return;

	*tout = db->sq_lspp_to;
}

void smgr_sq_dbg_lspp_tout_set(u32 tout)
{
	db->sq_lspp_to = tout;
}

void smgr_sq_dbg_qlen_get(u32 *qlen)
{
	if (unlikely(ptr_is_null(qlen)))
		return;

	*qlen = db->sq_qlen;
}

void smgr_sq_dbg_qlen_set(u32 qlen)
{
	db->sq_qlen = qlen;
}

s32 smgr_sq_init(struct pp_smgr_init_param *param)
{
	struct device *dev = pp_dev_get();
	u32 i, n_sq = param->num_syncqs;

	pr_debug("\n");
	/* Allocate syncq database */
	db = devm_kzalloc(dev, sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("Failed to allocate sync queues databse\n");
		return -ENOMEM;
	}

	if (n_sq) {
		db->sq =
			devm_kcalloc(dev, n_sq, sizeof(*db->sq), GFP_KERNEL);
		if (unlikely(!db->sq)) {
			pr_err("Failed to allocate %u sync queues memory\n", n_sq);
			return -ENOMEM;
		}
	}

	/* init lock */
	spin_lock_init(&db->lock);

	/* init syncq lists */
	INIT_LIST_HEAD(&db->free_list);
	INIT_LIST_HEAD(&db->act_list);

	/* get qos device */
	db->qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(db->qdev)))
		return -EPERM;

	/* init the timeout values */
	db->sq_sync_to = SQ_SYNC_TO;
	db->sq_lspp_to = SQ_LSPP_TO;
	db->sq_done_to = SQ_DONE_TO;
	db->sq_qlen    = SQ_DFLT_QLEN;

	/* prepare each syncq entry */
	for (i = 0; i < n_sq; i++) {
		if (likely(!__sq_prepare(i)))
			continue;
		pr_notice("failed to allocate %u sync queues, only %hu are supported\n",
			  n_sq, i);
		break;
	}

	/* set the number of sync queues supported */
	db->n_sq        = i;
	db->stats.total = i;

	return 0;
}

void smgr_sq_exit(void)
{
	u32 i;
	struct device *dev = pp_dev_get();

	pr_debug("\n");
	if (unlikely(!db))
		return;

	__sq_lock();
	for (i = 0; i < db->n_sq; i++) {
		/* kill all timers */
		del_timer(&db->sq[i].sync_timer);
		del_timer(&db->sq[i].done_timer);
	}
	__sq_unlock();
	if (db->sq)
		devm_kfree(dev, db->sq);
	devm_kfree(dev, db);
	db = NULL;
}
