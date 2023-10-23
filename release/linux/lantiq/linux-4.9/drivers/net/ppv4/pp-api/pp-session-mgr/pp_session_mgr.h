/*
 * Description: PP session manager H file
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_SESSION_MGR_H__
#define __PP_SESSION_MGR_H__

#include <linux/types.h>
#include <linux/init.h>
#include <linux/debugfs.h> /* struct dentry */
#include <linux/pp_api.h> /* For PP APIs */

/**
 * enum smgr_frag_mode - Fragmentation mode
 * @SMGR_FRAG_MODE_CHECK_MTU_IN_UC: MTU check is done in UC
 * @SMGR_FRAG_MODE_CHECK_MTU_IN_CHECKER: MTU check is done in Checker
 *
 * Fragmnetation mode. in case mode is SMGR_FRAG_MODE_CHECK_MTU_IN_CHECKER
 * only packets exceed the MTU will go through the UC.
 * The default mode is SMGR_FRAG_MODE_CHECK_MTU_IN_UC
 */
enum smgr_frag_mode {
	SMGR_FRAG_MODE_CHECK_MTU_IN_UC,
	SMGR_FRAG_MODE_CHECK_MTU_IN_CHECKER
};

/**
 * struct pp_smgr_init_param - Session manager initial configuration parameters
 * @valid: params valid
 * @num_sessions: number of session to support
 * @num_syncqs: number of syncq to support (user defined)
 * @dbgfs: debugfs parent folder
 * @frag_mode: Defines whether mtu violation is checked in uc or in checker
 */
struct pp_smgr_init_param {
	bool valid;
	u32 num_sessions;
	u32 num_syncqs;
	struct dentry *dbgfs;
	enum smgr_frag_mode frag_mode;
};

#ifdef CONFIG_PPV4_LGM

/**
 * @define Invalid tdox session id
 */
#define SMGR_INVALID_TDOX_SESS_ID  (1023)
/**
 * @struct Session manager statistics definitions
 * @sess_create_req total number of requests to create sessions
 * @sess_delete_req total number of requests to delete sessions
 * @port_flush_req total number of requests to flush port's sessions
 * @flush_all_req total number of requests to flush all sessions
 * @inactive_req total number of requests to get inactive sessions
 * @sess_created total number of sessions successfully created
 * @sess_deleted total number of sessions successfully deleted
 * @port_flushed total number of ports successfully flushed
 * @flush_all_done total number of flush all sessions successfully
 *        done
 * @inactive_done total number of requests to get inactive sessions
 *                successfully done
 * @sess_free number of free sessions
 * @sess_open number of opened sessions
 * @sess_open_hi_wm highest number of concurrent opened sessions
 *        ever
 * @sess_lu_fail number of session lookup failure, occur when
 *        session is not found, this is not an error
 * @sess_lu_succ number of successful sessions lookup, occur when a
 *        session is found
 * @ipv4_sess_opened number of opened ipv4 sessions
 * @ipv6_sess_opened number of opened ipv6 sessions
 * @tcp_sess_opened number of opened tcp sessions
 * @udp_sess_opened number of opened udp sessions
 * @cwlr_idle_wait number of times session manager had to wait for
 *                 crawler HW to be idle
 * @sess_create_fail number of session create requests that fails
 * @sess_delete_fail number of session delete requests that fails
 * @port_flush_fail number of port flush requests that fails
 * @flush_all_fail number of flush all sessions requests that fails
 * @inactive_fail total number of requests to get inactive sessions
 *                that fails
 * @invalid_args number of requests that fails due to invalid
 *                 parameters
 * @sess_lookup_err number of session create requests that fails due
 *                  to lookup failures
 * @sess_not_supported number of session create requests that fails
 *                     due to not supported ingress or egress packet
 * @sess_si_create_err number of session create requests that fails
 *                     due to si creation error
 * @work_args_cache_err number of work arguments cache allocation
 *                      errors
 * @hw_err number of requests that fails due to HW error
 * @resource_busy number of requests that fails due resources busy
 * @note USE only atomic64_t type, there are macro which depend on that
 */
struct smgr_stats {
	atomic_t sess_create_req;
	atomic_t sess_delete_req;
	atomic_t port_flush_req;
	atomic_t flush_all_req;
	atomic_t inactive_req;
	atomic_t sess_created;
	atomic_t sess_deleted;
	atomic_t port_flushed;
	atomic_t flush_all_done;
	atomic_t inactive_done;
	atomic_t sess_free;
	atomic_t sess_open;
	atomic_t sess_open_hi_wm;
	atomic_t sess_lu_fail;
	atomic_t sess_lu_succ;
	atomic_t ipv4_sess_opened;
	atomic_t ipv6_sess_opened;
	atomic_t tcp_sess_opened;
	atomic_t udp_sess_opened;
	atomic_t crwlr_idle_wait;
	/* errors */
	atomic_t sess_create_fail;
	atomic_t sess_delete_fail;
	atomic_t sess_update_fail;
	atomic_t port_flush_fail;
	atomic_t flush_all_fail;
	atomic_t inactive_fail;
	/* error cause */
	atomic_t invalid_args;
	atomic_t sess_lookup_err;
	atomic_t sess_not_supported;
	atomic_t sess_si_create_err;
	atomic_t work_args_cache_err;
	atomic_t hw_err;
	atomic_t resource_busy;
};

/**
 * @struct smgr_sq_stats
 * @brief syncq statistics
 */
struct smgr_sq_stats {
	/*! packets accepted */
	u32 packets_accepted;

	/*! packet dropped */
	u32 packets_dropped;

	/*! bytes accepted */
	u32 bytes_accepted;

	/*! bytes dropped */
	u32 bytes_dropped;

	/*! number of sync queue sync event w/o lspp */
	u32 lspp_timeout_events;

	/*! number of sync queue allocated */
	u32 allocated;

	/*! number of sync queue freed */
	u32 freed;

	/*! number of active sync queues */
	u32 active;

	/*! number of syncqs supported */
	u32 total;

	/*! number of invalid state errors */
	u32 err_invalid_state;

	/*! done events and failed to disconnect the queue (not empty) */
	u32 err_q_disconnect_failed;
};

/**
 * @struct smgr_tdox_stats
 * @brief tdox UC cpu statistics
 */
struct smgr_tdox_stats {
	u32 uc_rx_pkt;
	u32 uc_tx_pkt;
	u32 free_list_cnt;
	u32 busy_list_cnt;
	u32 obsolete_list_cnt;
	u32 cand_free_list_cnt;
	u32 cand_busy_list_cnt;
	u32 sess_update_free_list_cnt;
	u32 sess_update_busy_list_cnt;
	u32 tdox_sess_free;
	u32 tdox_free_candidates;
	u32 tdox_sess_full_fail;
	u32 tdox_candidate_full_fail;
	u32 tdox_create_args_err;
};

/**
 * @struct Session manager session database info definition
 * @flags session flags
 * @mod_flags modification flags, the flags which were used to
 *            determine the recipe
 * @sess_id session id
 * @in_port session ingress port
 * @eg_port session egress port
 * @fv_sz session field vector size for quick reference
 * @hash session hash results calculated by the HW
 * @mcast session mcast info
 * @tdox_ent Link to turbodox entry
 * @rx session ingress packet, needed for debug printing
 * @tx session egress packet, needed for debug printing
 */
struct sess_db_info {
	ulong flags;
	ulong mod_flags;
	u32 sess_id;
	u16 in_port;
	u16 eg_port;
	u8  fv_sz;
	struct pp_hash hash;
	struct pp_mcast mcast;
	struct tdox_db_entry *tdox_ent;
#ifdef CONFIG_DEBUG_FS
	struct pktprs_hdr rx;
	struct pktprs_hdr tx;
#endif
};

/**
 * @brief Get PP session info
 * @param id session id
 * @param info buffer to save the info
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_session_info_get(u32 id, struct sess_db_info *info);

/**
 * @brief Get session manager statistics
 * @param stats buffer
 * @return s32 0 on success, error code otherwise
 */
s32 pp_smgr_stats_get(struct smgr_stats *stats);

/**
 * @brief Reset session manager counters
 * @return s32 0 on success, error code otherwise
 */
s32 pp_smgr_stats_reset(void);

/**
 * @brief init the sync queue module
 * @note this API must be called from the late init routine
 *       since it uses the QoS APIs
 * @param param session manager init params
 * @return s32 0 on success
 */
s32 smgr_sq_init(struct pp_smgr_init_param *param);

/**
 * @brief this API should be called for sync queues mechanism
 *        when the lspp (Last Slow Path Packet) receive in the
 *        CPU path
 * @param session session id from the lspp
 */
void smgr_sq_lspp_rcv(u32 session);

/**
 * @brief get the sq statistics
 * @param stats sq statistics
 * @param reset reset statistics
 * @return s32 0 on success
 */
s32 smgr_sq_dbg_stats_get(struct smgr_sq_stats *stats, bool reset);

/**
 * @brief Set NF in session manager
 * @return s32 0 on success
 */
s32 smgr_nf_set(enum pp_nf_type type, struct pp_nf_info *nf);

/**
 * @brief Allocate sessions array
 * @note it is the user responsibility to free the memory
 * @param sessions pointer to return the array address
 * @param n_sessions number of sessions
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sessions_arr_alloc(u32 **sessions, u32 *n_sessions);

/**
 * @brief Initiate late parameters
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_tdox_late_init(void);

/**
 * @brief Get tdox stats
 * @param stats statistics structure
 * @return 0 if success
 */
s32 smgr_tdox_stats_get(struct smgr_tdox_stats *stats);

/**
 * @brief Get tdox enable state
 * @return true if tdox is enabled
 */
bool smgr_tdox_enable_get(void);

/**
 * @brief Allocate bitmap with bit per session
 * @note it is the user responsibility to free the memory
 * @param bmap bitmap pointer to return the bitmap address
 * @param n_bits bitmap size in bits, equal to number of sessions
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sessions_bmap_alloc(ulong **bmap, u32 *n_bits);

/**
 * @brief Get session manager busy sessions bitmap, set bit specify
 *        busy session, clear bit specify free session
 * @param bmap buffer to save sessions list
 * @param n_bits bitmap size in bits
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_open_sessions_bmap_get(unsigned long *bmap, u32 n_bits);

/**
 * @brief initialized PP session manager driver
 * @param init_param initial parameters
 * @return s32 0 on success
 */
s32 pp_smgr_init(struct pp_smgr_init_param *init_param);

/**
 * @brief Release all PP session manager resources/memory
 * @param void
 */
void pp_smgr_exit(void);
#else
static inline s32 pp_smgr_init(struct pp_smgr_init_param *cfg)
{
	if (cfg->valid)
		return -EINVAL;

	return 0;
}

static inline s32 smgr_sq_init(struct pp_smgr_init_param *cfg)
{
	if (cfg->valid)
		return -EINVAL;

	return 0;
}

static inline void pp_smgr_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PP_SESSION_MGR_H__ */
