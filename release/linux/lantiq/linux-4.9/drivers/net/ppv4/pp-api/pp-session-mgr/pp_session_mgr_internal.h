/*
 * Description: Packet Processor Session Manager Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_SESSION_MGR_INTERNAL_H__
#define __PP_SESSION_MGR_INTERNAL_H__

#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/bitops.h>
#include <linux/pp_api.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/pktprs.h>
#endif

#include "pp_si.h"
#include "pp_fv.h"
#include "uc.h"
#include "pp_session_mgr.h"

/**
 * @define DDR cache line size in bytes
 */
#define CACHE_LINE_SIZE 64

/**
 * @define Maximum number of sessions supported by the PP HW, derived
 *        from the SI session index field which is 3 bytes where
 *        0xffffff cannot be used
 */
#define SMGR_MAX_HW_SESSIONS    (0xFFFFFE)
/**
 * @define Invalid session id
 */
#define SMGR_INVALID_SESS_ID    (0xFFFFFF)

/**
 * @enum session manager sessions flags, these flags are set in the
 *       session db entry
 * @ROUTED specifying if session is a routed session, session is
 *         considered to be a routed session if the mac destination
 *         address has changed from ingress to egress
 * @SYNCQ specifying if session has a sync q attached
 * @MTU_CHCK specifying if MTU should be checked on the session
 * @MCAST_GRP specifying if session is a multicast group session
 *            (first cycle)
 * @MCAST_DST specifying if session is a multicast dst session
 *            (second cycle)
 */
enum smgr_sess_flags {
	SESS_FLAG_ROUTED,
	SESS_FLAG_SYNCQ,
	SESS_FLAG_MTU_CHCK,
	SESS_FLAG_MCAST_GRP,
	SESS_FLAG_MCAST_DST,
	SESS_FLAG_TDOX,
	SESS_FLAG_TDOX_SUPP,
	SMGR_FLAGS_NUM,
};

/**
 * @brief Shortcuts for session entry flags operations
 * @param e session db entry pointer (struct sess_db_entry)
 * @param f flag
 */
#define SESS_FLAG_SET(e, f)                         set_bit(f, &(e)->info.flags)
#define SESS_FLAG_CLR(e, f)                       clear_bit(f, &(e)->info.flags)

/**
 * @brief Shortcuts for session entry flags testing
 * @param s session db entry pointer (struct sess_db_entry)
 * @param f flag
 */
#define SESS_IS_FLAG_ON(e, f)                      test_bit(f, &(e)->info.flags)
#define SESS_IS_FLAG_OFF(e, f)                    !test_bit(f, &(e)->info.flags)

/**
 * @brief Shortcuts for session args flags testing
 * @param s session info pointer
 * @param f flag
 */
#define SESS_ARGS_IS_FLAG_ON(s, f)                test_bit(f, &(s)->args->flags)
#define SESS_ARGS_IS_FLAG_OFF(s, f)              !test_bit(f, &(s)->args->flags)

#define HDR_OUTER                                              PKTPRS_HDR_LEVEL0
#define HDR_INNER                                              PKTPRS_HDR_LEVEL1

/**
 * @brief Shortcut for accessing the session's rx packet header
 * @param s session info pointer
 */
#define SESS_RX_PKT(s)                      ((struct pktprs_hdr *)(s)->args->rx)

/**
 * @brief Shortcut for accessing the session's tx packet header
 * @param s session info pointer
 */
#define SESS_TX_PKT(s)                      ((struct pktprs_hdr *)(s)->args->tx)

/**
 * @brief Shortcuts for checking if session is pppoe
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_PPPOE(s)     PKTPRS_IS_PPPOE(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_PPPOE(s)     PKTPRS_IS_PPPOE(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_PPPOE(s)     PKTPRS_IS_PPPOE(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_PPPOE(s)     PKTPRS_IS_PPPOE(SESS_TX_PKT(s), HDR_INNER)

/**
 * @brief Shortcuts for checking if session is ipv4
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_V4(s)         PKTPRS_IS_IPV4(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_V4(s)         PKTPRS_IS_IPV4(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_V4(s)         PKTPRS_IS_IPV4(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_V4(s)         PKTPRS_IS_IPV4(SESS_TX_PKT(s), HDR_INNER)
#define SESS_RX_IS_OUTER_V6(s)         PKTPRS_IS_IPV6(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_V6(s)         PKTPRS_IS_IPV6(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_V6(s)         PKTPRS_IS_IPV6(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_V6(s)         PKTPRS_IS_IPV6(SESS_TX_PKT(s), HDR_INNER)

#define SESS_TX_OUTER_V4_NEXT(s) \
PKTPRS_PROTO_NEXT(SESS_TX_PKT(s), PKTPRS_PROTO_IPV4, HDR_OUTER)

/**
 * @brief Shortcuts for checking if session is ip
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_IP(s) \
(SESS_RX_IS_OUTER_V4(s) || SESS_RX_IS_OUTER_V6(s))
#define SESS_TX_IS_OUTER_IP(s) \
(SESS_TX_IS_OUTER_V4(s) || SESS_TX_IS_OUTER_V6(s))
#define SESS_RX_IS_INNER_IP(s) \
(SESS_RX_IS_INNER_V4(s) || SESS_RX_IS_INNER_V6(s))
#define SESS_TX_IS_INNER_IP(s) \
(SESS_TX_IS_INNER_V4(s) || SESS_TX_IS_INNER_V6(s))

/**
 * @brief Shortcuts for checking if session is udp
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_UDP(s)         PKTPRS_IS_UDP(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_UDP(s)         PKTPRS_IS_UDP(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_UDP(s)         PKTPRS_IS_UDP(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_UDP(s)         PKTPRS_IS_UDP(SESS_TX_PKT(s), HDR_INNER)

/**
 * @brief Shortcuts for checking if session is tcp
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_TCP(s)         PKTPRS_IS_TCP(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_TCP(s)         PKTPRS_IS_TCP(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_TCP(s)         PKTPRS_IS_TCP(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_TCP(s)         PKTPRS_IS_TCP(SESS_TX_PKT(s), HDR_INNER)

/**
 * @brief Shortcuts for checking if session is esp
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_ESP(s)         PKTPRS_IS_ESP(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_OUTER_ESP(s)         PKTPRS_IS_ESP(SESS_TX_PKT(s), HDR_OUTER)
#define SESS_RX_IS_INNER_ESP(s)         PKTPRS_IS_ESP(SESS_RX_PKT(s), HDR_INNER)
#define SESS_TX_IS_INNER_ESP(s)         PKTPRS_IS_ESP(SESS_TX_PKT(s), HDR_INNER)

#define SESS_TX_ESP_NEXT(s) \
PKTPRS_PROTO_NEXT(SESS_TX_PKT(s), PKTPRS_PROTO_ESP, HDR_OUTER)

/**
 * @brief Shortcuts for checking if session is supported L4
 * @param s session info pointer
 */
#define SESS_RX_IS_OUTER_L4(s) \
(SESS_RX_IS_OUTER_UDP(s) || SESS_RX_IS_OUTER_TCP(s) || SESS_RX_IS_OUTER_ESP(s))
#define SESS_TX_IS_OUTER_L4(s) \
(SESS_TX_IS_OUTER_UDP(s) || SESS_TX_IS_OUTER_TCP(s) || SESS_TX_IS_OUTER_ESP(s))
#define SESS_RX_IS_INNER_L4(s) \
(SESS_RX_IS_INNER_UDP(s) || SESS_RX_IS_INNER_TCP(s) || SESS_RX_IS_INNER_ESP(s))
#define SESS_TX_IS_INNER_L4(s) \
(SESS_TX_IS_INNER_UDP(s) || SESS_TX_IS_INNER_TCP(s) || SESS_TX_IS_INNER_ESP(s))

/**
 * @brief Shortcuts for checking if session is l2 gre
 * @param s session info pointer
 */
#define SESS_RX_IS_L2_GRE(s)                    PKTPRS_IS_L2_GRE(SESS_RX_PKT(s))
#define SESS_TX_IS_L2_GRE(s)                    PKTPRS_IS_L2_GRE(SESS_TX_PKT(s))

/**
 * @brief Shortcuts for checking if session is ip gre
 * @param s session info pointer
 */
#define SESS_RX_IS_IP_GRE(s)                    PKTPRS_IS_IP_GRE(SESS_RX_PKT(s))
#define SESS_TX_IS_IP_GRE(s)                    PKTPRS_IS_IP_GRE(SESS_TX_PKT(s))

/**
 * @brief Shortcuts for checking if session is dslite
 * @param s session info pointer
 */
#define SESS_RX_IS_DSLITE(s)                    PKTPRS_IS_DSLITE(SESS_RX_PKT(s))
#define SESS_TX_IS_DSLITE(s)                    PKTPRS_IS_DSLITE(SESS_TX_PKT(s))

/**
 * @brief Shortcuts for checking if session is sixrd
 * @param s session info pointer
 */
#define SESS_RX_IS_SIXRD(s)                      PKTPRS_IS_SIXRD(SESS_RX_PKT(s))
#define SESS_TX_IS_SIXRD(s)                      PKTPRS_IS_SIXRD(SESS_TX_PKT(s))

/**
 * @brief Shortcuts for checking if session is l2tp over udp
 * @param s session info pointer
 */
#define SESS_RX_IS_L2TP_OUDP(s)   PKTPRS_IS_L2TP_OUDP(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_L2TP_OUDP(s)   PKTPRS_IS_L2TP_OUDP(SESS_TX_PKT(s), HDR_OUTER)

/**
 * @brief Shortcuts for checking if session is ppp
 * @param s session info pointer
 */
#define SESS_RX_IS_PPP(s)               PKTPRS_IS_PPP(SESS_RX_PKT(s), HDR_OUTER)
#define SESS_TX_IS_PPP(s)               PKTPRS_IS_PPP(SESS_TX_PKT(s), HDR_OUTER)

/**
 * @enum Session manager mode
 * @disable disable mode, no sessions are created in this mode
 * @enable enable mode
 */
enum smgr_state {
	SMGR_DISABLE,
	SMGR_ENABLE,
	SMGR_STATES_NUM,
};

/**
 * @brief Session manager tdox session database entry definition
 * @tdox_id tdox id
 * @info relevant session info
 * @sess_ent corresponding session entry
 * @tdox_node list node for attaching the session to the tdox lists
 */
struct tdox_db_entry {
	u16 tdox_id;
	struct tdox_info info;
	struct sess_db_entry *sess_ent;
	struct list_head tdox_node;
} __aligned(CACHE_LINE_SIZE);

/**
 * @brief Session manager tdox update session database entry definition
 * @tdox_id tdox id
 * @sess_id corresponding session id
 * @tdox_node list node for attaching the session to the tdox lists
 */
struct tdox_sess_update_entry {
	u32 tdox_id;
	u32 sess_id;
	struct list_head tdox_node;
} __aligned(CACHE_LINE_SIZE);

/**
 * @brief Session manager session database entry definition
 * @info session info
 * @free_node list node for attaching the session to the free sessions
 *            list
 * @in_port_node list note for attaching the session to ingress port
 *               sessions list
 * @in_port_node list note for attaching the session to ingress port
 *               sessions list
 */
struct sess_db_entry {
	struct sess_db_info info;
	struct list_head free_node;
	struct list_head in_port_node;
	struct list_head eg_port_node;
} __aligned(CACHE_LINE_SIZE);

/**
 * @struct sess_info
 * @brief Session info definition, it defines the input
 *        information received by the user to create the session
 *        and all the information collected along the session
 *        create flow
 */
struct sess_info {
	/*! session create args received by the user */
	struct pp_sess_create_args *args;
	/*! session database entry */
	struct sess_db_entry       *db_ent;
	/*! host si structure which is being built along the flow */
	struct pp_si                si;
	/*! hw si structure which was encoded using the host si, this is
	 *  the si which is used to the create the actual session in the HW
	 */
	struct pp_hw_si             hw_si;
	/*! pointer for pointing to the right modification flags variable
	 *  along the session create flow
	 */
	ulong                      *mod_flags;
	/*! Struct defines the info required for nat modifications */
	struct {
		u8  rx_lvl;
		u8  tx_lvl;
		u16 res;
	} nat;
};

/**
 * @define FRAG_INFO_FLAG defines, to be used with struct si_ud_frag_info.flags
 */
#define FRAG_INFO_FLAG_INT_DF           (BIT(0))
#define FRAG_INFO_FLAG_EXT_DF           (BIT(1))
#define FRAG_INFO_FLAG_IGNORE_INT_DF    (BIT(2))
#define FRAG_INFO_FLAG_IGNORE_EXT_DF    (BIT(3))
#define FRAG_INFO_FLAG_FRAG_EXT         (BIT(4))
#define FRAG_INFO_FLAG_IPV4             (BIT(5))
#define FRAG_INFO_FLAG_PPPOE            (BIT(6))

/**
 * struct si_ud_frag_info - Fragmentation info
 *
 * This structure defines the fragmentation information saved in the si ud
 * right after the ps (Starts at si.ud + PP_PS_REGION_SZ).
 * Used in the fragmenter uc
 */
struct si_ud_frag_info {
	/*! Fragmentation flags */
	u8	flags;

	/*! l3 offset. In case of inner fragmentation,
	 *  the offset will be to the inner l3 hdr */
	u8	l3_off;

	/*! Destination queue */
	u16	dst_q;

	/*! Port's max_pkt_size */
	u16	max_pkt_size;
} __packed;

/**
 * struct si_ud_reass_info - Reassembly info
 *
 * This structure defines the reassembly information saved in the si ud
 * right after the ps (Starts at si.ud + PP_PS_REGION_SZ).
 * Used in the egress uc for reassembly
 */
struct si_ud_reass_info {
	/*! egress port headroom */
	u16 eg_port_hr;

	/*! egress port tailroom */
	u16 eg_port_tr;

	/*! flags */
	u8 flags;
} __packed;

/**
 * @brief Test whether a session is routed session.<br>
 *        Routed session is a session where the ingress destination
 *        mac address is different then the egress one.
 * @param s the session to test
 * @return bool true if the session is routed, false otherwise
 */
static inline bool smgr_is_sess_routed(const struct sess_db_info *s)
{
	if (ptr_is_null(s))
		return false;

	return test_bit(SESS_FLAG_ROUTED, &s->flags);
}

/**
 * @brief Test whether a session is bridged session.<br>
 *        Bridged session is a session where the ingress destination
 *        mac address is equal to the egress one
 * @param s the session to test
 * @return bool true if the session is bridged, false otherwise
 */
static inline bool smgr_is_sess_bridged(const struct sess_db_info *s)
{
	if (ptr_is_null(s))
		return false;

	return !test_bit(SESS_FLAG_ROUTED, &s->flags);
}

/**
 * @brief Test whether a session is multicast group session
 * @param s the session to test
 * @return bool true if the session is multicast group, false
 *         otherwise
 */
static inline bool smgr_is_sess_mcast_grp(const struct sess_db_info *s)
{
	if (ptr_is_null(s))
		return false;

	return test_bit(SESS_FLAG_MCAST_GRP, &s->flags);
}

/**
 * @brief Test whether a session is multicast dst session
 * @param s the session to test
 * @return bool true if the session is multicast dst, false
 *         otherwise
 */
static inline bool smgr_is_sess_mcast_dst(const struct sess_db_info *s)
{
	if (ptr_is_null(s))
		return false;

	return test_bit(SESS_FLAG_MCAST_DST, &s->flags);
}

/**
 * @brief Test whether session id is valid or not
 * @note Valid session id doesn't mean the session exist
 * @param id session id to test
 * @return bool true if the id is a valid session id
 */
bool smgr_is_sess_id_valid(u32 id);

/**
 * @brief Get inactive sessions scan state (enable/disable)
 * @return bool true in case it is enabled, false otherwise
 */
bool smgr_sessions_scan_state_get(void);

/**
 * @brief Set the inactive sessions scan state
 * @param en enable/disable scan
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sessions_scan_state_set(bool en);

/**
 * @brief Get session SI
 * @param id session id
 * @param si buffer to write the si
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_session_si_get(u32 id, struct pp_si *si);

/**
 * @brief Get session dynamic SI
 * @param id session id
 * @param dsi buffer to write the dsi
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_session_dsi_get(u32 id, struct pp_dsi *dsi);

/**
 * @brief Get bitmap indicating which sessions uses the specified SGC
 * @param grp SGC group index
 * @param cntr SGC index
 * @param bmap bitmap buffer to save the sessions list
 * @param n_bits bitmap size in bits
 */
s32 smgr_sgc_sessions_bmap_get(u8 grp, u16 cntr, ulong *bmap, u32 n_bits);

/**
 * @brief Shortcut for converting QoS logical ID to physical ID
 * @param logical the logical ID
 * @param physical result physical ID
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_get_queue_phy_id(u16 logical, u16 *physical);

/**
 * @brief Update a PP session
 * @param sess_id session id to update
 * @param si new si
 * @return s32 0 on success update, error code otherwise
 */
s32 smgr_session_update(u32 sess_id, struct pp_hw_si *hw_si);
/* ========================================================================== */
/*                       Synchronization queues                               */
/* ========================================================================== */
/**
 * @brief allocate new syncq
 * @note exported internally only for debufs
 * @param session session id
 * @param dst_queue_id dst queue id
 * @return s32 0 on success, error code otherwise
 */
s32 sq_alloc(u32 session, u32 dst_queue_id);

/**
 * @brief allocate new syncq
 * @note called before the session is added to the hw,
 *       after the allocation the queue is available to use
 * @param sess session entry
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sq_alloc(struct sess_info *sess);

/**
 * @brief start the sync queue state machine
 * @note exported internally only for debufs
 * @param session session id
 * @return s32 0 on success, error code otherwise
 */
s32 sq_start(u32 session);
/**
 * @brief start the sync queue state machine
 * @note called after the session was added in HW
 * @param sess session entry
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sq_start(struct sess_info *sess);

/**
 * @brief delete the synch queue
 * @note exported internally only for debufs
 * @param session session id
 * @return s32 0 on success, error code otherwise
 */
s32 sq_del(u32 session);

/**
 * @brief delete the synch queue
 * @note called when session is being deleted
 * @param ent session db entry
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_sq_del(struct sess_db_entry *ent);

/**
 * @brief print all the sync queue from database
 * @note debug only
 */
void smgr_sq_dbg_dump(void);

/**
 * @brief get the "synch timeout" in micro sec units
 * @note debug only
 * @param tout timeout
 */
void smgr_sq_dbg_sync_tout_get(u32 *tout);

/**
 * @brief set the "synch timeout" in micro sec units
 * @note debug only
 * @param tout timeout
 */
void smgr_sq_dbg_sync_tout_set(u32  tout);

/**
 * @brief get the "done timeout" in micro sec units
 * @note debug only
 * @param tout timeout
 */
void smgr_sq_dbg_done_tout_get(u32 *tout);

/**
 * @brief set the "done timeout" in micro sec units
 * @note debug only
 * @param tout timeout
 */
void smgr_sq_dbg_done_tout_set(u32  tout);

/**
 * @brief get the "lspp timeout" in micro sec units
 * @note debug only
 * @note lspp: Last Slow Path Packet
 * @param tout timeout
 */
void smgr_sq_dbg_lspp_tout_get(u32 *tout);

/**
 * @brief set the "lspp timeout" in micro sec units
 * @note debug only
 * @note lspp: Last Slow Path Packet
 * @param tout timeout
 */
void smgr_sq_dbg_lspp_tout_set(u32  tout);

/**
 * @brief get the max qlen
 * @param qlen queue length
 */
void smgr_sq_dbg_qlen_get(u32 *qlen);

/**
 * @brief set the max qlen
 * @param qlen queue length
 */
void smgr_sq_dbg_qlen_set(u32 qlen);

/**
 * @brief exit the sync queue module
 */
void smgr_sq_exit(void);

/* ========================================================================== */
/*                               Multicast                                    */
/* ========================================================================== */
/**
 * @brief Track session in the multicast group db
 * @param sess session to track
 */
void smgr_mcast_sess_track(struct sess_info *sess);

/**
 * @brief Untrack session from the multicast group db
 * @param sess session to untrack
 */
void smgr_mcast_sess_untrack(struct sess_db_entry *ent);

/**
 * @brief Test whether session args mcast info is valid, mcast
 *        info is considered to be invalid when the user set one
 *        of the mcast flags and the mcast info is
 *        invalid, another invalid option is to set more than
 *        one mcast flag
 * @param args the args to test
 * @return bool true in case mcast info is valid, false
 *         otherwise
 */
bool smgr_mcast_is_info_valid(struct pp_sess_create_args *args);

/**
 * @brief update the session args for multicast sessions
 *        oob information for mcast dst sessions
 *        ud information for mcast group sessions
 * @param sess session info
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_mcast_sess_args_update(struct sess_info *sess);

/**
 * @brief Prepare a session multicast database entry
 * @param sess session info to save the session db entry
 * @note Caller MUST acquire database lock
 */
void smgr_mcast_sess_ent_prepare(struct sess_info *sess);

/**
 * @brief Perform session multicast lookup based on the
 *        multicast information (group or dst)
 * @param sess session info
 * @return s32 -EEXIST in case session was found, 0 if session wasn't
 *         found, error code otherwise
 */
s32 smgr_mcast_sess_lookup(struct sess_info *sess);

/**
 * @brief Helper function to initialize multicast info
 * @param invalid_sess_id invalid session index
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_mcast_init(u32 invalid_sess_id);

/**
 * @brief Clean multicast resources
 */
void smgr_mcast_exit(void);

/* ========================================================================== */
/*                               Turbodox                                     */
/* ========================================================================== */

/**
 * @brief Helper function to initialize tdox manager
 * @return s32 0 on success, error code otherwise
 */
s32 smgr_tdox_init(struct device *dev);

/**
 * @brief Fetching tdox record info and sending mailbox to uC
 * @param ent session entry in db
 * @return 0 if success
 */
s32 smgr_tdox_record_create(struct sess_db_entry *sess_ent);

/**
 * @brief Validate args are valid for tdox session
 * @param ent session args
 * @return true if args are valid
 */
bool smgr_tdox_is_info_valid(struct pp_sess_create_args *args);

/**
 * @brief Prepare tdox db before seession creation
 * @param ent session info
 * @return 0 if success
 */
s32 smgr_tdox_sess_ent_prepare(struct sess_info *sess);

/**
 * @brief Remove tdox entry
 * @param ent tdox entry in db
 */
void smgr_tdox_session_remove(struct tdox_db_entry *ent);

/**
 * @brief Set tdox enable state
 */
void smgr_tdox_enable_set(bool enable);

/**
 * @brief Clean tdox resources
 */
void smgr_tdox_exit(void);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Set PP session manager state (enable/disable)
 * @param state new state
 * @return s32 0 on successful state change, error code otherwise
 */
s32 smgr_state_set(enum smgr_state state);

/**
 * @brief Get session manager current state
 * @return enum smgr_state
 */
enum smgr_state smgr_state_get(void);

/**
 * @brief Get nf status in smgr
 * @param nf nf type
 * @return true if nf enabled
 */
bool smgr_is_nf_en(enum pp_nf_type nf);

/**
 * @brief Session manager debug init
 * @return s32 0 for success, non-zero otherwise
 */
s32 smgr_dbg_init(struct dentry *parent);

/**
 * @brief Session manager debug cleanup
 * @return s32 0 for success, non-zero otherwise
 */
s32 smgr_dbg_clean(void);

#else /* !CONFIG_DEBUG_FS */
static inline s32 smgr_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 smgr_dbg_clean(void)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __PP_SESSION_MGR_INTERNAL_H__ */
