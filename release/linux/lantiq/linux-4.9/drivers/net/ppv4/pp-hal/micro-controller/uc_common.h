/*
 * Description: PP micro-controller common definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2020 Intel Corporation
 */
#ifndef __PP_UC_COMMON_H__
#define __PP_UC_COMMON_H__

/* UC version - version major and minor numbers must be aligned with
 * the fw version, if the fw was changed without any interface changes
 * only the build number should be changed and no need to update
 * the version here
 */
#define EGRESS_VER_MAJOR  1
#define EGRESS_VER_MINOR  20

#define INGRESS_VER_MAJOR 1
#define INGRESS_VER_MINOR 3

/**
 * @define CMD_DONE_REG_IDX
 * @brief CCU GP register index for the mailbox command done
 *        signal
 */
#define CMD_DONE_REG_IDX  2
/**
 * @define CLS_LOCK_REG_IDX
 * @brief CCU GP register index for A step classifier interface
 *        lock
 */
#define CLS_LOCK_REG_IDX  7

/**
 * @define BUFF_EX_FLAG, to be used with struct buffer_ex_info.flags si_ud_reass_info.flags
 */
#define BUFF_EX_FLAG_WR_DESC           (BIT(0))
#define BUFF_EX_FLAG_PREL2_EN          (BIT(1))
/**
 * @define BUFF_EX_FLAG, to be used with struct si_ud_reass_info.flags
 * NOTE - si_ud_reass_info.flags also includes buffer_ex_info.flags!
 */
#define REASS_INFO_FLAG_MEMORY_PORT    (BIT(2))

/**
 * @enum uc_mbox_cmd_type
 * @brief mailbox command type
 */
enum uc_mbox_cmd_type {
	UC_CMD_FIRST,
	/*! synchronized the host with the mbox queue and cpu id */
	UC_CMD_SYNC = UC_CMD_FIRST,
	/*! send init info to UC */
	UC_CMD_INIT,
	/*! get the egress uc version */
	UC_CMD_VERSION,
	/*! reset the UC logger */
	UC_CMD_LOGGER_RESET,
	/*! set UC logger level */
	UC_CMD_LOGGER_LEVEL_SET,
	/*! set the multicast pp port id */
	UC_CMD_MCAST_PID,
	/*! set the multicast pp rx queue id (second cycle) */
	UC_CMD_MCAST_QUEUE,
	/*! set the multicast group bitmap */
	UC_CMD_MCAST_DST,
	/*! Set the multicast uc to work in session mirroring mode */
	UC_CMD_MCAST_SESS_MIRRORING,
	/*! set the ipsec pp port id */
	UC_CMD_IPSEC_PID,
	/*! set the ipsec pp rx queue id (second cycle) */
	UC_CMD_IPSEC_QUEUE,
	/*! set the reassembly info */
	UC_CMD_REASSEMBLY_INFO,
	/*! set reassembly context timeout */
	UC_CMD_REASSEMBLY_TIMEOUT_THR,
	/* read UC AUX register value */
	UC_CMD_AUX_REG_RD,
	/* write UC AUX register value */
	UC_CMD_AUX_REG_WR,
	/*! Get Tdox configuration */
	UC_CMD_TDOX_CONF_GET,
	/*! Create Tdox record */
	UC_CMD_TDOX_CREATE,
	/*! Create Tdox record */
	UC_CMD_TDOX_REMOVE,
	/*! Get Tdox record */
	UC_CMD_TDOX_STATS,
	/*! Checker mailbox */
	UC_CMD_CHK_MBX,
	/*! Classifier mailbox */
	UC_CMD_CLS_MBX,

	UC_CMD_LAST = UC_CMD_CLS_MBX,
	UC_CMD_MAX  = U32_MAX,
};

/**
 * @enum mbox_cmd_err
 * @brief mailbox command error code
 */
enum mbox_cmd_err {
	MBOX_CMD_SUCCESS,
	MBOX_CMD_BUSY,
	MBOX_CMD_UNSUPPORTED,
	MBOX_CMD_INVALID_PARAM,
};

/**
 * @enum uc_log_level
 * @brief logger log level
 */
enum uc_log_level {
	UC_LOG_FATAL,
	UC_LOG_ERROR,
	UC_LOG_WARN,
	UC_LOG_INFO,
	UC_LOG_DEBUG,
	UC_LOG_LEVELS_NUM
};

/**
 * @struct uc_log_msg
 * @brief UC log message
 * @note keep the structure aligned to power of 2
 */
struct uc_log_msg {
	u32  val;
	u32  ts;
	u8   level;
	u8   cid:3,
	     tid:5;
	char str[38];
	char func[16];
};

#define _RX_DMA_MAX_POOLS (5)

/**
 * @struct eg_uc_init_info
 * @brief UC init info
 */
struct eg_uc_init_info {
	/*! logger buffer base address  */
	u32 logger_buff;
	/*! logger buffer size in bytes */
	u32 logger_buff_sz;
	/* checker base address */
	u32 chk_base;
	/* checker counters cache base */
	u32 chk_ctrs_cache_base;
	/* checker dsi ddr base */
	u32 chk_dsi_ddr_base;
	/* classifier base address */
	u32 cls_base;
	/*! Policy buffer sizes */
	u16 buffer_size[_RX_DMA_MAX_POOLS];
	/*! Reserved */
	u16 res;
};

/**
 * @struct mbox_aux_reg_wr
 * @brief UC Mailbox command intended to aux register
 */
struct mbox_aux_reg_wr {
	/*! aux register */
	u32 reg;
	/*! register value */
	u32 val;
};

/**
 * @struct uc_chk_cmd
 * @brief UC Mailbox command intended to checker
 */
struct uc_chk_cmd {
	/*! session id */
	u32 id;
	/*! checker mailbox command */
	u32 cmd;
};

/**
 * @brief SI size in 32bit words granularity
 */
#define _PP_SI_WORDS_CNT                 (64)

/**
 * @struct uc_cls_cmd
 * @brief UC Mailbox command intended to classifier
 */
struct uc_cls_cmd {
	/*! classifier mailbox command */
	u32 cmd_id;
	/*! Hash1 */
	u32 h1;
	/*! hash2 */
	u32 h2;
	/*! sig */
	u32 sig;
	/*! session id + bucket offset + hash table + flags */
	u32 cmd_info;
	/*! DSI update info */
	u32 dsi_info;
	/*! SI (optional) */
	u32 si[_PP_SI_WORDS_CNT];
};

/**
 * @struct mcast_stats
 */
struct mcast_stats {
	/*! RX packet counter */
	u64 rx_pkt;
	/*! TX packet counter */
	u64 tx_pkt;
	/*! drop packet counter */
	u64 drop_pkt;
	/*! Mirror TX packet counter */
	u64 mirror_tx_pkt;
	/*! Mirror drop packet counter */
	u64 mirror_drop_pkt;
};

/**
 * @struct ipsec_stats
 */
struct ipsec_stats {
	/*! RX packet counter */
	u64 rx_pkt;
	/*! TX packet counter */
	u64 tx_pkt;
	/*! error packet counter */
	u64 error_pkt;
};

/**
 * @struct smgr_reass_uc_cpu_stats
 * @brief reassembly UC cpu statistics
 */
struct reassembly_stats {
	u64 rx_pkts;
	u64 tx_pkts;
	u64 reassembled;
	u64 accelerated;
	u64 diverted;
	u64 early_diverted;
	u64 matched;
	u64 not_matched;
	struct {
		u64 cntxs_starv;
		u64 cntxs_busy;
		u64 cntx_overflow;
		u64 timedout;
		u64 timeout_err;
		u64 frags_starv;
		u64 duplicates;
		u64 unsupported_frag;
		u64 unsupported_ext;
		u64 unsupported_proto;
		u64 unsupported_pmac;
		u64 unsupported_pre_l2;
		u64 ext_overflow;
		u64 invalid_len;
		u64 dropped;
		u64 bm_null_buff;
	} err;
};

/**
 * @struct frag_stats
 */
struct frag_stats {
	/*! RX packet counter */
	u64 rx_pkt;
	/*! TX packet counter */
	u64 tx_pkt;
	/*! Drop packet counter */
	u64 total_drops;
	/*! bmgr drops */
	u64 bmgr_drops;
	/*! don't frag drops */
	u64 df_drops;
	/*! More frags required than supported */
	u64 max_frags_drops;
};

/**
 * @struct consolidated statistics for egress uc
 */
struct egress_stats {
	struct mcast_stats mcast;
	struct ipsec_stats ipsec;
	struct reassembly_stats reass;
	struct frag_stats frag;
};

/**
 * @brief Reassembly network function parameters for the FW
 */
struct reassembly_info {
	/*! Host port headroom */
	u16 host_port_hr;
	/*! Host port tailroom */
	u16 host_port_tr;
	/*! host base policy */
	u8  host_base_policy;
	/*! host policies bitmap */
	u8  host_policies_bmap;
	/*! host pid */
	u8  host_pid;
	/*! host port flags. To be used with struct si_ud_reass_info.flags */
	u8  host_port_flags;
	/*! host queue to send packets to host upon failure */
	u16 host_q;
	/*! 2nd round queue */
	u16 pp_rx_q;
	/*! exception si base address */
	u32 excp_si_base;
	/*! rxdma regs base for calculating buffer size */
	u32 rxdma_base;
	/*! si base address */
	u64 si_base;
};

/**
 * @brief Tdox session information
 */
struct tdox_info {
	u32 supp_enable:1,
	    is_candidate:1,
	    low_queue:9,
	    high_queue:9,
	    threshold:8;
};

/**
 * @brief Config read from tdox at boot
 */
struct tdox_uc_config {
	u32 record_state_offset;
};

/**
 * @brief Tdox uc create command
 */
struct tdox_record_create_cmd {
	struct tdox_info info;
	u16 sess_id;
	u16 tdox_id;
};

/**
 * @brief Tdox uc remove command
 */
struct tdox_record_remove_cmd {
	u32 tdox_id;
};

/**
 * @brief mcast uc session mirroring
 */
struct mcast_sess_mirroring_cmd {
	u16 enable;
	u16 orig_q;
	u16 q;
	u16 gpid;
	u32 ps0;
	u16 ps_valid;
	u16 eg_port_hr;
	u16 eg_port_tr;
	u16 flags;
	u16 base_policy;
	u8  policies_map;
};

#endif /* __PP_UC_COMMON_H__ */
