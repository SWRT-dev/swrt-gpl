// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 * Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Copyright 2012 - 2014 Lantiq Deutschland GmbH
 *
 *****************************************************************************/

#ifndef DATAPATH_H
#define DATAPATH_H

#include <linux/skbuff.h>	/*skb */
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/platform_device.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#include <linux/percpu.h>

#if IS_ENABLED(CONFIG_PRX300_CQM) || \
	IS_ENABLED(CONFIG_GRX500_CBM)
	#include <net/lantiq_cbm_api.h>
#else
	#include <net/intel_cbm_api.h>
#endif

#define dp_vlan_dev_priv vlan_dev_priv
#include <linux/pp_qos_api.h>
#if IS_ENABLED(CONFIG_INTEL_CBM_SKB) || \
	LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
	#define DP_SKB_HACK
#endif
#include <net/datapath_api_qos.h>
#if IS_ENABLED(CONFIG_NET_SWITCHDEV)
#include <net/switchdev.h>
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
#include "datapath_swdev.h"
#endif
#include <net/datapath_inst.h>
#include <net/datapath_api_umt.h>

#define DP_DEBUGFS_PATH "/sys/kernel/debug/dp"

#define MAX_SUBIFS 256  /* Max subif per DPID */
#define MAX_DP_PORTS  16
#define PMAC_CPU_ID  0
#define DP_MAX_BP_NUM 128
#define DP_MAX_QUEUE_NUM 256
#define DP_MAX_SCHED_NUM 2048  /*In fact it should use max number of node*/
#define DP_MAX_CQM_DEQ 256 /* Max Deq for array definition from LGM and PRX */

#define DP_PLATFORM_INIT    1
#define DP_PLATFORM_DE_INIT 2

#define GSWIP_VER_REG_OFFSET 0x013

#define UP_STATS(atomic) atomic_add(1, &(atomic))

#define STATS_GET(atomic) atomic_read(&(atomic))
#define STATS_SET(atomic, val) atomic_set(&(atomic), val)
#define DP_CB(i, x) dp_port_prop[i].info.x

#define dp_set_val(reg, val, mask, offset) do {\
	(reg) &= ~(mask);\
	(reg) |= (((val) << (offset)) & (mask));\
} while (0)

#define dp_get_val(val, mask, offset) (((val) & (mask)) >> (offset))

#define DP_DEBUG_ASSERT(expr, fmt, arg...)  do { if (expr) \
	pr_err(fmt, ##arg); \
} while (0)

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
#define DP_DEBUG(flags, fmt, arg...)  do { \
	if (unlikely((dp_dbg_flag & (flags)) && \
		     (((dp_print_num_en) && \
		      (dp_max_print_num)) || (!dp_print_num_en)))) {\
	pr_info(fmt, ##arg); \
	if ((dp_print_num_en) && \
	    (dp_max_print_num)) \
		dp_max_print_num--; \
	} \
} while (0)

#define DP_ASSERT_SCOPE __func__

#else
#define DP_DEBUG(flags, fmt, arg...)
#endif				/* end of CONFIG_INTEL_DATAPATH_DBG */

extern u32 reinsert_deq_port;

static inline bool is_invalid_port(unsigned int port_id)
{
	if (port_id >= MAX_DP_PORTS) {
		pr_err("port_id(%u) should be 0~%u\n", port_id, MAX_DP_PORTS);
		return true;
	}
	return false;
}

static inline bool is_invalid_inst(unsigned int inst)
{
	if (inst >= DP_MAX_INST) {
		pr_err("inst(%u) should be 0~%u\n", inst, DP_MAX_INST);
		return true;
	}
	return false;
}

#define IFNAMSIZ 16
#define DP_MAX_HW_CAP 4

#ifdef DP_SPIN_LOCK
#define DP_LOCK_T spinlock_t
#define DP_LOCK_INIT(lock) spin_lock_init(lock)
#define DP_DEFINE_LOCK(lock) DEFINE_SPINLOCK(lock)
#define DP_LIB_LOCK    spin_lock_bh
#define DP_LIB_UNLOCK  spin_unlock_bh
#else
#define DP_LOCK_T struct mutex
#define DP_LOCK_INIT(lock) mutex_init(lock)
#define DP_DEFINE_LOCK(lock) DEFINE_MUTEX(lock)
#define DP_LIB_LOCK    mutex_lock
#define DP_LIB_UNLOCK  mutex_unlock
#endif

extern DP_LOCK_T dp_lock;

#define PARSER_FLAG_SIZE   40
#define PARSER_OFFSET_SIZE 8

#define PMAC_HDR_SIZE (sizeof(struct pmac_rx_hdr))
#define PKT_PASER_FLAG_OFFSET   0
#define PKT_PASER_OFFSET_OFFSET (PARSER_FLAG_SIZE)
#define PKT_PMAC_OFFSET         ((PARSER_FLAG_SIZE) + (PARSER_OFFSET_SIZE))
#define PKT_DATA_OFFSET         ((PKT_PMAC_OFFSET) + (PMAC_HDR_SIZE))

#define CHECK_BIT(var, pos) (((var) & (1 << (pos))) ? 1 : 0)

#define PASAR_OFFSETS_NUM 40	/*40 bytes offset */
#define PASAR_FLAGS_NUM 8	/*8 bytes */

/* maximum DMA port per controller */
#define DP_MAX_DMA_PORT 4
/* maximum dma channnels per port*/
#define DP_MAX_DMA_CHAN 64
/* maximum dma controller*/
#define DP_DMAMAX 7
#define DP_DEQ(p, q) (dp_deq_port_tbl[p][q])

enum dp_xmit_errors {
	DP_XMIT_ERR_DEFAULT = 0,
	DP_XMIT_ERR_NOT_INIT,
	DP_XMIT_ERR_IN_IRQ,
	DP_XMIT_ERR_NULL_SUBIF,
	DP_XMIT_ERR_PORT_TOO_BIG,
	DP_XMIT_ERR_NULL_SKB,
	DP_XMIT_ERR_NULL_IF,
	DP_XMIT_ERR_REALLOC_SKB,
	DP_XMIT_ERR_EP_ZERO,
	DP_XMIT_ERR_GSO_NOHEADROOM,
	DP_XMIT_ERR_CSM_NO_SUPPORT,
	DP_XMIT_PTP_ERR,
};

enum PARSER_FLAGS {
	PASER_FLAGS_NO = 0,
	PASER_FLAGS_END,
	PASER_FLAGS_CAPWAP,
	PASER_FLAGS_GRE,
	PASER_FLAGS_LEN,
	PASER_FLAGS_GREK,
	PASER_FLAGS_NN1,
	PASER_FLAGS_NN2,

	PASER_FLAGS_ITAG,
	PASER_FLAGS_1VLAN,
	PASER_FLAGS_2VLAN,
	PASER_FLAGS_3VLAN,
	PASER_FLAGS_4VLAN,
	PASER_FLAGS_SNAP,
	PASER_FLAGS_PPPOES,
	PASER_FLAGS_1IPV4,

	PASER_FLAGS_1IPV6,
	PASER_FLAGS_2IPV4,
	PASER_FLAGS_2IPV6,
	PASER_FLAGS_ROUTEXP,
	PASER_FLAGS_TCP,
	PASER_FLAGS_1UDP,
	PASER_FLAGS_IGMP,
	PASER_FLAGS_IPV4OPT,

	PASER_FLAGS_IPV6EXT,
	PASER_FLAGS_TCPACK,
	PASER_FLAGS_IPFRAG,
	PASER_FLAGS_EAPOL,
	PASER_FLAGS_2IPV6EXT,
	PASER_FLAGS_2UDP,
	PASER_FLAGS_L2TPNEXP,
	PASER_FLAGS_LROEXP,

	PASER_FLAGS_L2TP,
	PASER_FLAGS_GRE_VLAN1,
	PASER_FLAGS_GRE_VLAN2,
	PASER_FLAGS_GRE_PPPOE,
	PASER_FLAGS_BYTE4_BIT4,
	PASER_FLAGS_BYTE4_BIT5,
	PASER_FLAGS_BYTE4_BIT6,
	PASER_FLAGS_BYTE4_BIT7,

	PASER_FLAGS_BYTE5_BIT0,
	PASER_FLAGS_BYTE5_BIT1,
	PASER_FLAGS_BYTE5_BIT2,
	PASER_FLAGS_BYTE5_BIT3,
	PASER_FLAGS_BYTE5_BIT4,
	PASER_FLAGS_BYTE5_BIT5,
	PASER_FLAGS_BYTE5_BIT6,
	PASER_FLAGS_BYTE5_BIT7,

	PASER_FLAGS_BYTE6_BIT0,
	PASER_FLAGS_BYTE6_BIT1,
	PASER_FLAGS_BYTE6_BIT2,
	PASER_FLAGS_BYTE6_BIT3,
	PASER_FLAGS_BYTE6_BIT4,
	PASER_FLAGS_BYTE6_BIT5,
	PASER_FLAGS_BYTE6_BIT6,
	PASER_FLAGS_BYTE6_BIT7,

	PASER_FLAGS_BYTE7_BIT0,
	PASER_FLAGS_BYTE7_BIT1,
	PASER_FLAGS_BYTE7_BIT2,
	PASER_FLAGS_BYTE7_BIT3,
	PASER_FLAGS_BYTE7_BIT4,
	PASER_FLAGS_BYTE7_BIT5,
	PASER_FLAGS_BYTE7_BIT6,
	PASER_FLAGS_BYTE7_BIT7,

	/*Must be put at the end of the enum */
	PASER_FLAGS_MAX
};

enum dp_message_errors {
	DP_ERR_SUBIF_NOT_FOUND = -7,
	DP_ERR_INIT_FAIL = -6,
	DP_ERR_INVALID_PORT_ID = -5,
	DP_ERR_MEM = -4,
	DP_ERR_NULL_DATA = -3,
	DP_ERR_INVALID_SUBIF = -2,
	DP_ERR_DEFAULT = -1,
};

/*! PMAC port flag */
enum PORT_FLAG {
	PORT_FREE = 0,		/*! The port is free */
	PORT_ALLOCATED,		/*! the port is already allocated to others,
				 * but not registered or no need to register.\n
				 * eg, LRO/CAPWA, only need to allocate,
				 * but no need to register
				 */
	PORT_DEV_REGISTERED,	/*! dev Registered already. */
	PORT_SUBIF_REGISTERED,	/*! subif Registered already. */

	PORT_FLAG_NO_VALID	/*! Not valid flag */
};

#define DP_DBG_ENUM_OR_STRING(name, value, short_name) {name = value}

enum DP_DBG_FLAG {
	DP_DBG_FLAG_DBG = BIT(0),
	DP_DBG_FLAG_DUMP_RX_DATA = BIT(1),
	DP_DBG_FLAG_DUMP_RX_DESCRIPTOR = BIT(2),
	DP_DBG_FLAG_DUMP_RX_PASER = BIT(3),
	DP_DBG_FLAG_DUMP_RX_PMAC = BIT(4),
	DP_DBG_FLAG_DUMP_RX = (BIT(1) | BIT(2) | BIT(3) | BIT(4)),
	DP_DBG_FLAG_DUMP_TX_DATA = BIT(5),
	DP_DBG_FLAG_DUMP_TX_DESCRIPTOR = BIT(6),
	DP_DBG_FLAG_DUMP_TX_PMAC = BIT(7),
	DP_DBG_FLAG_DUMP_TX_SUM = BIT(8),
	DP_DBG_FLAG_DUMP_TX = (BIT(5) | BIT(6) | BIT(7) | BIT(8)),
	DP_DBG_FLAG_COC = BIT(9),
	DP_DBG_FLAG_MIB = BIT(10),
	DP_DBG_FLAG_MIB_ALGO = BIT(11),
	DP_DBG_FLAG_CBM_BUF = BIT(12),
	DP_DBG_FLAG_PAE = BIT(13),
	DP_DBG_FLAG_INST = BIT(14),
	DP_DBG_FLAG_SWDEV = BIT(15),
	DP_DBG_FLAG_NOTIFY = BIT(16),
	DP_DBG_FLAG_LOGIC = BIT(17),
	DP_DBG_FLAG_GSWIP_API = BIT(18),
	DP_DBG_FLAG_QOS = BIT(19),
	DP_DBG_FLAG_QOS_DETAIL = BIT(20),
	DP_DBG_FLAG_LOOKUP = BIT(21),
	DP_DBG_FLAG_REG = BIT(22),
	DP_DBG_FLAG_BR_VLAN = BIT(23),
	DP_DBG_FLAG_PCE = BIT(24),
	DP_DBG_FLAG_OPS = BIT(25),
	/*Note, once add a new entry here int the enum,
	 *need to add new item in below macro DP_F_FLAG_LIST
	 */
	DP_DBG_FLAG_MAX = BIT(31)
};

/*Note: per bit one variable */
#define DP_DBG_FLAG_LIST {\
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DBG, "dbg"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_RX_DATA, "rx_data"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "rx_desc"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_RX_PASER, "rx_parse"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_RX_PMAC, "rx_pmac"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_RX, "rx"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_TX_DATA, "tx_data"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_TX_DESCRIPTOR, "tx_desc"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_TX_PMAC, "tx_pmac"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_TX_SUM, "tx_sum"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_DUMP_TX, "tx"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_COC, "coc"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_MIB, "mib"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_MIB_ALGO, "mib_algo"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_CBM_BUF, "cbm_buf"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_PAE, "pae"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_INST, "inst"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_SWDEV, "swdev"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_NOTIFY, "notify"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_LOGIC, "logic"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_GSWIP_API, "gswip"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_QOS, "qos"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_QOS_DETAIL, "qos2"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_LOOKUP, "lookup"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_REG, "register"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_BR_VLAN, "br_vlan"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_PCE, "pce"), \
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_OPS, "ops"), \
	\
	\
	/*must be last one */\
	DP_DBG_ENUM_OR_STRING(DP_DBG_FLAG_MAX, "")\
}

enum QOS_FLAG {
	NODE_LINK_ADD = 0, /*add a link node */
	NODE_LINK_GET,     /*get a link node */
	NODE_LINK_EN_GET,  /*Get link status: enable/disable */
	NODE_LINK_EN_SET,  /*Set link status: enable/disable */
	NODE_UNLINK,       /*unlink a node: in fact, it is just flush now*/
	LINK_ADD,          /*add a link with multiple link nodes */
	LINK_GET,          /*get a link may with multiple link nodes */
	LINK_PRIO_SET,     /*set arbitrate/priority */
	LINK_PRIO_GET,     /*get arbitrate/priority */
	QUEUE_CFG_SET,     /*set queue configuration */
	QUEUE_CFG_GET,     /*get queue configuration */
	SHAPER_SET,        /*set shaper/bandwidth*/
	SHAPER_GET,        /*get shaper/bandwidth*/
	NODE_ALLOC,        /*allocate a node */
	NODE_FREE,         /*free a node */
	NODE_CHILDREN_FREE,  /*free all children under one specified parent:
			      *   scheduler/port
			      */
	DEQ_PORT_RES_GET,  /*get all full links under one specified port */
	COUNTER_MODE_SET,  /*set counter mode: may only for TMU now so far*/
	COUNTER_MODE_GET,  /*get counter mode: may only for TMU now so far*/
	QUEUE_MAP_GET,     /*get lookup entries based on the specified qid*/
	QUEUE_MAP_SET,     /*set lookup entries to the specified qid*/
	NODE_CHILDREN_GET, /*get direct children list of node*/
	QOS_LEVEL_GET,     /* get Max Scheduler level for Node */
	QOS_Q_LOGIC,       /* get logical queue ID based on physical queue ID */
	QOS_GLOBAL_CFG_GET, /* get global qos config info */
	QOS_PORT_CFG_SET, /* set qos port config info */
	QOS_BLOCK_FLUSH_PORT, /* Block and Flush all QiD's in the port */
	QOS_BLOCK_FLUSH_QUEUE, /* Block and Flush particular QiD */
};

struct dev_mib {
	atomic_t rx_fn_rxif_pkt; /*! received packet counter */
	atomic_t rx_fn_txif_pkt; /*! transmitted packet counter */
	atomic_t rx_fn_dropped; /*! transmitted packet counter */
	atomic_t tx_cbm_pkt; /*! transmitted packet counter */
	atomic_t tx_clone_pkt; /*! duplicate unicast packet for cloned flag */
	atomic_t tx_hdr_room_pkt; /*! duplicate pkt for no enough headerroom*/
	atomic_t tx_tso_pkt;	/*! transmitted packet counter */
	atomic_t tx_pkt_dropped;	/*! dropped packet counter */
};

struct mib_global_stats {
	u64 rx_rxif_pkts;
	u64 rx_txif_pkts;
	u64 rx_rxif_clone;
	u64 rx_drop;
	u64 tx_pkts;
	u64 tx_drop;
};

DECLARE_PER_CPU_SHARED_ALIGNED(struct mib_global_stats, mib_g_stats);

#define MIB_G_STATS_INC(member) do { \
			per_cpu(mib_g_stats, get_cpu()).member++; \
			put_cpu(); \
		} while(0)

#define MIB_G_STATS_RESET(member, cpu) do { \
			per_cpu(mib_g_stats, cpu).member = 0; \
		} while(0)

#define MIB_G_STATS_GET(member, cpu) \
			per_cpu(mib_g_stats, cpu).member


struct logic_dev {
	struct list_head list;
	struct net_device *dev;
	u16 bp; /*bridge port */
	u16 ep;
	u16 ctp;
	u32 subif_flag; /*save the flag used during dp_register_subif*/
};

struct dp_igp {
	u32 igp_id; /* CQM enqueue port based ID */
	u32 igp_dma_ch_to_gswip; /* DMA TX channel base to GSWIP */
	u32 num_out_cqm_deq_port; /* num of CQM dequeue port to GSWIP */
};

struct dp_egp {
	int egp_id; /* EGP port ID */
	enum DP_EGP_TYPE type; /* EGP port: DP_EGP_TO_DEV, DP_EGP_TO_GSWIP */
};

/*! Sub interface detail information */
struct dp_subif_info {
	s32 flags;
	u32 subif;
	struct net_device *netif; /*! pointer to  net_device */
	char device_name[IFNAMSIZ]; /*! devide name, like wlan0, */
	struct dev_mib mib; /*! mib */
	struct dp_reinsert_count reins_cnt;
	struct net_device *ctp_dev; /*CTP dev for PON pmapper case*/
	u16 fid; /* switch bridge id */
	struct list_head logic_dev; /*unexplicit logical dev*/
#if IS_ENABLED(CONFIG_NET_SWITCHDEV)
	void *swdev_priv; /*to store ext vlan info*/
#endif
	u8 deq_port_idx; /* To store deq port index from register_subif */
	union {
		struct {
			u16 bp;     /* bridge port */
			u16 gpid; /*!< [out] gpid which is mapped from
				   * dpid +subif normally one GPID per subif for
				   *   non PON device.
				   *   For PON case, one GPID per bridge port
				   */
			u32 data_flag_ops; /* To store original flag from caller
					    * during dp_register_subif
					    * under data->flag_ops
					    */
			u8 num_qid; /*!< number of queue id*/
			union {
				u16 qid;    /* physical queue id Still keep it
					     * to be back-compatible for legacy
					     * platform and legacy integration
					     */
				/* physical queue id */
				u16 qid_list[DP_MAX_DEQ_PER_SUBIF];
			};
			u32 dfl_sess[DP_DFL_SESS_NUM];	  /*! default CPU egress
							   * session ID Valid
							   * only if
							   * f_dfl_eg_sess set
							   * one sesson per
							   * class[4 bits]
							   */
			u32 subif_groupid;
		};
		struct dp_subif_common subif_common;
	};
	s16 sched_id; /* can be physical scheduler id or logical node id */
	s16 q_node[DP_MAX_DEQ_PER_SUBIF]; /* logical Q node Id if applicable */
	s16 qos_deq_port[DP_MAX_DEQ_PER_SUBIF]; /* qos port id */
	s16 cqm_deq_port[DP_MAX_DEQ_PER_SUBIF]; /* CQM physical dequeue port ID
						 * (absolute)
						 */
	s16 cqm_port_idx; /* CQM relative dequeue port index, like tconf id */
	u32 subif_flag; /* To store original flag from caller during
			 * dp_register_subif
			 */
	u16 mac_learn_dis; /* To store mac learning capability of subif from
			    * caller during dp_register_subif
			    */
	atomic_t rx_flag; /* To enable/disable DP rx */
	atomic_t f_dfl_sess[DP_DFL_SESS_NUM]; /*! flag to indicate whether
					       *  dfl_eg_sess valid or
					       *  not
					       */
	u16 max_pkt_size;
	u16 cqm_mtu_size;
	u16 headroom_size;
	u16 tailroom_size;
	u16 min_pkt_len;
	int min_pkt_len_cfg;
	u16 tx_policy_base;    /* TX policy base */
	u8 tx_policy_num;      /* TX policy number */
	u8 tx_policy_map;      /* TX policy map */
	u16 rx_policy_base; /* RX policy base */
	u8 rx_policy_num;   /* RX policy number */
	u8 rx_policy_map;   /* RX policy map */
	u8 pkt_only_en;
	u8 seg_en;
	u16 swdev_en;
	bool cpu_port_en;
	u8 prel2_len:2;	 /* 0 = disabled, 1 = 16 bytes, 2 = 32 bytes, 3 = 48 bytes */
	dp_rx_fn_t rx_fn;	/*!< Rx function callback */
	dp_get_netif_subifid_fn_t get_subifid_fn; /*! get subif ID callback */
	enum DP_DATA_PORT_TYPE type;
	enum DP_SPL_TYPE spl_conn_type; /* only for special path,
					 * otherwise set to DP_SPL_INVAL
					 */
	int igp_id;
	int tx_pkt_credit;
	u8 num_igp;
	u8 num_egp;
	struct dp_spl_cfg *spl_cfg;
};

struct vlan_info {
	u16 out_proto;
	u16 out_vid;
	u16 in_proto;
	u16 in_vid;
	int cnt;
};

enum DP_TEMP_DMA_PMAC {
	TEMPL_NORMAL = 0,
	TEMPL_CHECKSUM,
	TEMPL_PTP,
	TEMPL_INSERT,
	TEMPL_OTHERS,
	TEMPL_CHECKSUM_PTP,
	MAX_TEMPLATE
};

enum DP_PRIV_F {
	DP_PRIV_PER_CTP_QUEUE = BIT(0), /*Manage Queue per CTP/subif */
};

struct pmac_port_info {
	enum PORT_FLAG status;	/*! port status */
	struct dp_cb cb;	/*! Callback Pointer to DIRECTPATH_CB */
	struct module *owner;
	struct net_device *dev;
	u32 dev_port;
	u32 num_subif;
	u16 subif_max;
	union {
		struct {
			int port_id;
			int alloc_flags; /* the flags saved when calling
					  * dp_port_alloc
					  */
			u8  cqe_lu_mode; /* cqe lookup mode */
			u32 gsw_mode; /* gswip mode for subif */
			s16 gpid_spl;  /* special GPID:
					* alloc it at dp_alloc_port
					* config it at dp_register_dev
					* for policy setting
					*/

		};
		struct dp_subif_port_common subif_port_cmn;
	};
	atomic_t tx_err_drop;
	atomic_t rx_err_drop;
	struct gsw_itf *itf_info;  /*point to switch interface configuration */
	int ctp_max; /*maximum ctp */
	u32 vap_offset; /*shift bits to get vap value */
	u32 vap_mask; /*get final vap after bit shift */
	u32 flag_other; /*save flag from cbm_dp_port_alloc */
	u32 deq_port_base; /*CQE Dequeue Port */
	u32 deq_port_num;  /*for PON IP: 64 ports, others: 1 */
	u32 dma_chan; /*associated dma tx CH,-1 means no DMA CH*/
	u32 tx_pkt_credit;  /*PP port tx bytes credit */
	u32 tx_b_credit;  /*PP port tx bytes credit */
	void *txpush_addr_qos;  /*QoS push addr after shift or mask from
				 * PP QOS point of view
				 */
	void *txpush_addr;  /* QOS push address without any shift/mask */
	u32 tx_ring_size; /*PP ring size */
	u32 tx_ring_offset;  /*PP: next tx_ring_addr=
			      *   current tx_ring_addr + tx_ring_offset
			      */
	u16 gpid_base; /* gpid base
			* For CPU/DPDK:
			*   alloc it in dp_platform_set
			*   config it in dp_platform_set
			* For peripheral device
			*   alloc it at dp_alloc_port via gpid_port_assign
			*   config it at dp_register_subif
			*/
	u16 gpid_num; /* reserved nubmer of continuous of gpid */
	u16 tx_policy_base; /* TX policy base */
	u8 tx_policy_num;   /* TX policy number */
	u16 rx_policy_base; /* RX policy base */
	u8 rx_policy_num;   /* RX policy number */
	u16 spl_tx_policy_base; /* TX policy base   for spl GPID */
	u8 spl_tx_policy_num;   /* TX policy number for spl GPID */
	u8 spl_tx_policy_map;   /* TX policy map    for spl GPID */
	u16 spl_rx_policy_base; /* RX policy base   for spl GPID */
	u8 spl_rx_policy_num;   /* RX policy number for spl GPID */
	u8 spl_rx_policy_map;   /* RX policy map    for spl GPID */
	u32 num_dma_chan; /*For G.INT it's 8 or 16, for other 1*/
	u32 lct_idx; /* LCT subif register flag */
	struct dp_lct_rx_cnt *lct_rx_cnt; /* For lct counter corrections,
					   * only allocated in case of lct subif
					   */
	u32 dma_chan_tbl_idx; /*! Base entry index of dp_dma_chan_tbl */
	u32 res_qid_base; /* Base entry for the device's reserved Q */
	u32 num_resv_q; /* Num of reserved Q per device */
	u32 f_ptp: 1; /* PTP1588 support enablement */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	u32 swdev_en; /* switchdev enable/disable flag for port */
#endif
	u16 loop_dis; /*!< If set do not add this dp port's bp
		       * into other bp's member port map
		       * if source = dest dp port
		       */
	u16 bp_hairpin_cap; /*!< If set, then add BP's in this dev
			     * into its own bridgeport member list
			     * For PRX: Only WLAN/Wifi device need
			     * hair pin capability
			     */
	/*only valid for 1st dp instanace which need dp_xmit/dp_rx*/
	/*[0] for non-checksum case,
	 *[1] for checksum offload
	 *[2] two cases:
	 * a: only traffic directly to MPE DL FW
	 * b: DSL bonding FCS case
	 */
	struct pmac_tx_hdr pmac_template[MAX_TEMPLATE];
	u32 desc_dw_templ[4][MAX_TEMPLATE];
	u32 desc_dw_mask[4][MAX_TEMPLATE];
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	struct dma_tx_desc_0 dma0_template[MAX_TEMPLATE];
	struct dma_tx_desc_0 dma0_mask_template[MAX_TEMPLATE];
	struct dma_tx_desc_1 dma1_template[MAX_TEMPLATE];
	struct dma_tx_desc_1 dma1_mask_template[MAX_TEMPLATE];
	struct dma_tx_desc_3 dma3_template[MAX_TEMPLATE];
	struct dma_tx_desc_3 dma3_mask_template[MAX_TEMPLATE];
#endif
	u32 num_tx_ring; /* Num of Tx ring */
	u32 num_rx_ring; /* Num of Rx ring */
	struct dp_rx_ring rx_ring[DP_RX_RING_NUM]; /*!< DC rx ring info	*/
	struct dp_tx_ring tx_ring[DP_TX_RING_NUM]; /*!< DC tx ring info	*/
	u8 num_umt_port; /* Num of UMT port */
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	struct dp_umt_port umt[DP_MAX_UMT]; /*!< UMT Param */
#endif
	enum DP_DATA_PORT_TYPE type;
	u32 blk_size;	/*!< PCE Block Size */
	/* These members must be end. */
	u32 tail;
	struct dp_subif_info *subif_info;
	spinlock_t mib_cnt_lock; /* lock for updates from mib_counters module*/
};

struct ctp_dev {
	struct list_head list;
	struct net_device *dev; /* CTP device pointer */
	u16 bp; /* bridge port */
	u16 ctp; /* CTP port */
};

/*bridge port with pmapper supported dev structure */
struct bp_pmapper {
	int flag;/*0-FREE, 1-Used*/
	struct net_device *dev; /*bridge port device pointer */
	struct list_head ctp_dev; /* CTP dev list */
	int pcp[DP_PMAP_PCP_NUM];  /*PCP table */
	int dscp[DP_PMAP_DSCP_NUM]; /*DSCP table*/
	int def_ctp; /*Untag & nonip*/
	int mode; /*mode*/
	int ref_cnt; /*reference counter */
	int ctp_flow_index; /* to store CTP first flow entry index
			     * for vlan aware feature PCE rule
			     */
};

/*queue struct */
struct q_info {
	int flag;  /*0-FREE, 1-Used*/
	int need_free; /*if this queue is allocated by dp_register_subif,
			*   it needs free during de-register.
			*Otherwise, no free
			*/
	int q_node_id;
	int ref_cnt; /*subif_counter*/
	int cqm_dequeue_port; /*CQM dequeue port */
};

/*scheduler struct */
struct dp_sched_info {
	int flag;  /*0-FREE, 1-Used*/
	int ref_cnt; /*subif_counter*/
	int cqm_dequeue_port; /*CQM dequeue port */
};

struct dma_chan_info {
	atomic_t ref_cnt;
};

struct cqm_port_info {
	u32 f_first_qid : 1; /*0 not valid */
	u32 ref_cnt; /*reference counter: the number of CTP attached to it*/
	u32 ref_cnt_umt; /* ref counter of UMT */
	u32 tx_pkt_credit;  /*PP port tx bytes credit */
	void *txpush_addr; /* QOS push addr after shift or mask from
			    * PP QOS point of view
			    */
	void *txpush_addr_qos; /* QOS push address without any shift/mask */
	u32 tx_ring_size; /*PP port ring size */
	int qos_port; /*qos port id*/
	union {
		int first_qid; /*in order to auto sharing queue,
				*1st queue allocated by dp_register_subif_ext
				* for that cqm_dequeue_port will be stored here.
				* later it will be shared by other subif via
				* dp_register_subif_ext
				*/
		int qid[DP_MAX_DEQ_PER_SUBIF];
	};
	int q_node; /*first_qid's logical node id*/
	int dp_port; /* dp_port info */
	u32 dma_chan;
	u32 dma_ch_offset; /*! Offset of dp_dma_chan_tbl */
	u32 cpu_type;
};

struct parser_info {
	u8 v;
	s8 size;
};

struct subif_platform_data {
	struct net_device *dev;
	struct dp_subif_data *subif_data;  /*from dp_register_subif_ex */
#define TRIGGER_CQE_DP_ENABLE  1
	int act; /*Set by HAL subif_platform_set and used by DP lib */
};

struct vlan_info1 {
	/* Changed this TPID field to GSWIP API enum type.
	 * We do not have flexible for any TPID, only following are supported:
	 * 1. ignore (don't care)
	 * 2. 0x8100
	 * 3. Value configured int VTE Type register
	 */
	GSW_ExtendedVlanFilterTpid_t tpid;  /* TPID like 0x8100, 0x8800 */
	u16 vid ;  /*VLAN ID*/
	/*note: user priority/CFI both don't care */
	/* DSCP to Priority value mapping is possible */
};

struct vlan1 {
	int bp;  /*assigned bp for this single VLAN dev */
	struct vlan_info1 outer_vlan; /*out vlan info */
	/* Add Ethernet type with GSWIP API enum type.
	 * Following types are supported:
	 * 1. ignore	(don't care)
	 * 2. IPoE	(0x0800)
	 * 3. PPPoE	(0x8863 or 0x8864)
	 * 4. ARP	(0x0806)
	 * 5. IPv6 IPoE	(0x86DD)
	 */
	GSW_ExtendedVlanFilterEthertype_t ether_type;
};

struct vlan2 {
	int bp;  /*assigned bp for this double VLAN dev */
	struct vlan_info1 outer_vlan;  /*out vlan info */
	struct vlan_info1 inner_vlan;   /*in vlan info */
	/* Add Ethernet type with GSWIP API enum type.
	 * Following types are supported:
	 * 1. ignore	(don't care)
	 * 2. IPoE	(0x0800)
	 * 3. PPPoE	(0x8863 or 0x8864)
	 * 4. ARP	(0x0806)
	 * 5. IPv6 IPoE	(0x86DD)
	 */
	GSW_ExtendedVlanFilterEthertype_t ether_type;
};

struct ext_vlan_info {
	int subif_grp, logic_port; /* base subif group and logical port.
				    * In DP it is subif
				    */
	int bp; /*default bp for this ctp */
	int n_vlan1, n_vlan2; /*size of vlan1/2_list*/
	int n_vlan1_drop, n_vlan2_drop; /*size of vlan1/2_drop_list */
	struct vlan1 *vlan1_list; /*allow single vlan dev info list auto
				   * bp is for egress VLAN setting
				   */
	struct vlan2 *vlan2_list; /* allow double vlan dev info list auto
				   * bp is for egress VLAN setting
				   */
	struct vlan1 *vlan1_drop_list; /* drop single vlan list - manual
					*  bp no use
					*/
	struct vlan2 *vlan2_drop_list; /* drop double vlan list - manual
					* bp no use
					*/
	/* Need add other input / output information for deletion. ?? */
	/* private data stored by function set_gswip_ext_vlan */
	void *priv;
};

struct dp_tc_vlan_info {
	int dev_type; /* bit 0 - 1: apply VLAN to bp
		       *         0: apply VLAN to subix (subif group)
		       * bit 1 - 0: apply VLAN to non-multicast session on CTP
		       *         1: don't apply VLAN to non-multicast session
		       * bit 2 - 0: apply VLAN to multicast session on CTP
		       *         1: don't apply VLAN to multicast session
		       */
	int subix;  /*similar like GSWIP subif group*/
	int bp;  /*bridge port id */
	int dp_port; /*logical port */
	int inst;  /*DP instance */
};

#ifdef CONFIG_INTEL_DATAPATH_CPUFREQ
enum CPUFREQ_FLAG {
	PRE_CHANGE = 0,		/*! Cpufreq transition prechange */
	POST_CHANGE,		/*! Cpufreq transition postchange */
	POLICY_NOTIFY,		/*! Cpufreq policy notifier */
};
#endif

/*port 0 is reserved*/
extern int dp_inst_num;
extern int dp_print_len;
extern struct inst_property dp_port_prop[DP_MAX_INST];
extern struct pmac_port_info *dp_port_info[DP_MAX_INST];
extern struct q_info dp_q_tbl[DP_MAX_INST][DP_MAX_QUEUE_NUM];
extern struct dp_sched_info dp_sched_tbl[DP_MAX_INST][DP_MAX_SCHED_NUM];
extern struct cqm_port_info dp_deq_port_tbl[DP_MAX_INST][DP_MAX_CQM_DEQ];
extern struct bp_pmapper dp_bp_tbl[DP_MAX_INST][DP_MAX_BP_NUM];
extern struct dma_chan_info *dp_dma_chan_tbl[DP_MAX_INST];

static inline struct inst_property *get_dp_port_prop(int inst)
{
	return &dp_port_prop[inst];
}

static inline struct pmac_port_info *get_dp_port_info(int inst, int index)
{
	return &dp_port_info[inst][index];
}

static inline struct cqm_port_info *get_dp_deqport_info(int inst, int idx)
{
	return &dp_deq_port_tbl[inst][idx];
}

static inline struct bp_pmapper *get_dp_bp_info(int inst, int idx)
{
	return &dp_bp_tbl[inst][idx];
}

static inline struct q_info *get_dp_q_info(int inst, int idx)
{
	return &dp_q_tbl[inst][idx];
}

static inline struct inst_info *get_dp_prop_info(int inst)
{
	return &dp_port_prop[inst].info;
}

struct inst_property *dp_port_prop_dbg(int inst);
struct pmac_port_info *dp_port_info_dbg(int inst, int index);

static inline struct dp_subif_info *get_dp_port_subif(
	const struct pmac_port_info *port, u16 vap)
{
	return &port->subif_info[vap];
}

static inline struct dev_mib *get_dp_port_subif_mib(struct dp_subif_info *sif)
{
	return &sif->mib;
}

/* TODO: Need to improve this API later */
static inline bool is_soc_lgm(int inst)
{
	struct inst_info *info = get_dp_prop_info(inst);

	if (info->type == GSWIP32_TYPE)
		return true;
	return false;
}

/* TODO: Need to improve this API later */
static inline bool is_soc_prx(int inst)
{
	struct inst_info *info = get_dp_prop_info(inst);

	if (info->type == GSWIP31_TYPE)
		return true;
	return false;
}

extern u32 dp_dbg_flag;
extern u32 dp_dbg_err;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
extern u32 dp_max_print_num;
extern u32 dp_print_num_en;
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_ACA_CSUM_WORKAROUND)
extern int aca_portid;
#endif

void dp_loop_eth_dev_exit(void);

#ifdef CONFIG_DEBUG_FS
struct dentry *dp_proc_install(void);
#else
static inline struct dentry *dp_proc_install(void) { return NULL;}
#endif

extern char *dp_dbg_flag_str[];
extern u32 dp_dbg_flag_list[];
extern u32 dp_port_flag[];
extern char *dp_port_type_str[];
extern char *dp_port_status_str[];
extern struct parser_info pinfo[];

enum TEST_MODE {
	DP_RX_MODE_NORMAL = 0,
	DP_RX_MODE_LAN_WAN_BRIDGE,
	DPR_RX_MODE_MAX
};

extern struct platform_device *g_dp_dev;
extern u32 dp_rx_test_mode;
extern u32 rx_desc_mask[4];
extern u32 tx_desc_mask[4];
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
extern struct dma_rx_desc_1 dma_rx_desc_mask1;
extern struct dma_rx_desc_2 dma_rx_desc_mask2;
extern struct dma_rx_desc_3 dma_rx_desc_mask3;
extern struct dma_rx_desc_0 dma_tx_desc_mask0;
extern struct dma_rx_desc_1 dma_tx_desc_mask1;
#endif

ssize_t proc_print_mode_write(struct file *file, const char *buf,
			      size_t count, loff_t *ppos);
void proc_print_mode_read(struct seq_file *s);
int parser_size_via_index(u8 index);
struct pmac_port_info *get_port_info_via_dev(struct net_device *dev);
void dp_clear_mib(dp_subif_t *subif, uint32_t flag);
extern u32 dp_drop_all_tcp_err;
extern u32 dp_pkt_size_check;
void print_parser_status(struct seq_file *s);
void proc_mib_timer_read(struct seq_file *s);
int mpe_fh_netfiler_install(void);
#ifdef CONFIG_LTQ_DATAPATH_CPUFREQ
int dp_cpufreq_notify_init(int inst);
int dp_cpufreq_notify_exit(void);
#endif
int proc_qos_init(void);
int proc_qos_dump(struct seq_file *s, int pos);
ssize_t proc_qos_write(struct file *file, const char *buf,
		       size_t count, loff_t *ppos);
void dump_parser_flag(char *buf);

//int dp_reset_sys_mib(u32 flag);
void dp_clear_all_mib_inside(uint32_t flag);

extern int ip_offset_hw_adjust;
int register_notifier(u32 flag);
int unregister_notifier(u32 flag);
//int supported_logic_dev(int inst, struct net_device *dev, char *subif_name);
struct net_device *get_base_dev(struct net_device *dev, int level);
int add_logic_dev(int inst, int port_id, struct net_device *dev,
		  dp_subif_t *subif_id, struct dp_subif_data *data, u32 flags);
int del_logic_dev(int inst, struct list_head *head, struct net_device *dev,
		  u32 flags);
int get_vlan_via_dev(struct net_device *dev, struct vlan_prop *vlan_prop);
void dp_parser_info_refresh(u32 cpu, u32 mpe1, u32 mpe2, u32 mpe3, u32 verify);
int dp_inst_init(u32 flag);
void dp_inst_free(void);
int request_dp(u32 flag);
int dp_init_module(void);
void dp_cleanup_module(void);
int dp_probe(struct platform_device *pdev);
#define NS_INT16SZ	 2
#define NS_INADDRSZ	 4
#define NS_IN6ADDRSZ	16

int low_10dec(u64 x);
int high_10dec(u64 x);
int dp_atoi(unsigned char *str);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
int get_offset_clear_chksum(struct sk_buff *skb, u32 *ip_offset,
			    u32 *tcp_h_offset,
			    u32 *tcp_type);
#else
int get_offset_clear_chksum(struct sk_buff *skb, u32 *ip_offset,
			    u32 *tcp_h_offset, u16 *l3_csum_off,
			    u16 *l4_csum_off, u32 *tcp_type);
#endif
int dp_basic_proc(void);

struct pmac_port_info *get_port_info_via_dp_port(int inst, int dp_port);

void set_dp_dbg_flag(uint32_t flags);
uint32_t get_dp_dbg_flag(void);
void dp_dump_raw_data(const void *buf, int len, char *prefix_str);

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
/*! @brief  ltq_tso_xmit
 *@param[in] skb  pointer to packet buffer like sk_buff
 *@param[in] hdr  point to packet header, like pmac header
 *@param[in] len  packet header
 *@param[in] flags :Reserved
 *@return 0 if OK  / -1 if error
 *@note
 */
int ltq_tso_xmit(struct sk_buff *skb, void *hdr, int len, int flags);
#endif

char *dp_skb_csum_str(struct sk_buff *skb);
extern struct dentry *dp_proc_node;
int get_dp_dbg_flag_str_size(void);
int get_dp_port_status_str_size(void);

int dp_request_inst(struct dp_inst_info *info, u32 flag);
int register_dp_cap(u32 flag);
int bp_pmapper_get(int inst, struct net_device *dev);
extern int dp_init_ok;
extern int dp_cpu_init_ok;
void set_chksum(struct pmac_tx_hdr *pmac, u32 tcp_type,
		u32 ip_offset, int ip_off_hw_adjust, u32 tcp_h_offset);

#if IS_ENABLED(CONFIG_QOS_MGR)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev, u32 handle,
					__be16 protocol,
					struct tc_to_netdev *tc);
#else
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev,
					enum tc_setup_type type,
					void *type_data);
#endif
#define DP_ENABLE_TC_OFFLOADS
#endif

#define DP_SUBIF_LIST_HASH_SHIFT 8
#define DP_SUBIF_LIST_HASH_BIT_LENGTH 10
#define DP_SUBIF_LIST_HASH_SIZE ((1 << DP_SUBIF_LIST_HASH_BIT_LENGTH) - 1)

int dp_get_drv_mib(dp_subif_t *subif, dp_drv_mib_t *mib, uint32_t flag);
extern struct hlist_head dp_subif_list[DP_SUBIF_LIST_HASH_SIZE];
int32_t dp_sync_subifid(struct net_device *dev, char *subif_name,
			dp_subif_t *subif_id, struct dp_subif_data *data,
			u32 flags, int *f_subif_up);
int32_t dp_sync_subifid_priv(struct net_device *dev, char *subif_name,
			     dp_subif_t *subif_id, struct dp_subif_data *data,
			     u32 flags, dp_get_netif_subifid_fn_t subifid_fn,
			     int *f_subif_up, int f_notif);
int32_t	dp_update_subif(struct net_device *netif, void *data, dp_subif_t *subif,
			char *subif_name, u32 flags,
			dp_get_netif_subifid_fn_t subifid_fn);
int32_t	dp_del_subif(struct net_device *netif, void *data, dp_subif_t *subif,
		     char *subif_name, u32 flags);
struct dp_subif_cache *dp_subif_lookup_safe(struct hlist_head *head,
					    struct net_device *dev,
					    void *data);
int dp_subif_list_init(void);
void dp_subif_list_free(void);
int parser_enabled(int ep, struct dma_rx_desc_1 *desc_1);
int dp_lan_wan_bridging(int port_id, struct sk_buff *skb);
int get_dma_chan_idx(int inst, int num_dma_chan);
int dp_get_dma_ch_num(int inst, int ep, int num_deq_port);
u32 alloc_dma_chan_tbl(int inst);
void free_dma_chan_tbl(int inst);
u32 alloc_dp_port_subif_info(int inst);
void free_dp_port_subif_info(int inst);
u32 dp_subif_hash(struct net_device *dev);
int dp_cbm_deq_port_enable(struct module *owner, int inst, int port_id,
			   int deq_port_idx, int num_deq_port, int flags,
			   u32 dma_ch_off);
int32_t dp_get_subifid_for_update(int inst, struct net_device *netif,
				  dp_subif_t *subif, uint32_t flags);
int do_tx_hwtstamp(int inst, int dpid, struct sk_buff *skb);
struct hlist_head *get_dp_g_bridge_id_entry_hash_table_info(int index);
struct dp_evt_notif_info {
	struct notifier_block nb;
	struct dp_event evt_info;
	struct list_head list;
};

struct dp_evt_notif_data {
	enum DP_DATA_PORT_TYPE type;
	enum DP_EVENT_OWNER owner;
	struct net_device *dev;
	struct module *mod;
	u32 dev_port;
	s32 subif;
	int dpid;
	int inst;
};

extern struct blocking_notifier_head dp_evt_notif_list;
int register_dp_event_notifier(struct dp_event *info);
int unregister_dp_event_notifier(struct dp_event *info);

static inline bool is_directpath(struct pmac_port_info *port)
{
	return (port->alloc_flags & DP_F_DIRECT);
}

static inline bool is_dsl(struct pmac_port_info *port)
{
	return (port->alloc_flags & DP_F_FAST_DSL);
}

void dp_print_err_info(int res);
int dp_notifier_invoke(int inst,
		       struct net_device *dev, u32 port_id, u32 subif_id,
		       enum DP_EVENT_TYPE type);
int proc_dp_event_list_dump(struct seq_file *s, int pos);

int dp_dealloc_cqm_port(struct module *owner, u32 dev_port,
			struct pmac_port_info *port,
			struct cbm_dp_alloc_data *data, u32 flags);

int dp_alloc_cqm_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, s32 port_id,
		      struct cbm_dp_alloc_data *cbm_data, u32 flags);

int dp_enable_cqm_port(struct module *owner, struct pmac_port_info *port,
		       struct cbm_dp_en_data *data, u32 flags);

int dp_cqm_port_alloc_complete(struct module *owner,
			       struct pmac_port_info *port, s32 dp_port,
			       struct cbm_dp_alloc_complete_data *data,
			       u32 flags);

int dp_cqm_deq_port_res_get(u32 dp_port, cbm_dq_port_res_t *res,
			    u32 flags);

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
int dp_cqm_gpid_lpid_map(struct cbm_gpid_lpid *map);
#endif

int dp_init_pce(void);
int proc_pce_dump(struct seq_file *s, int pos);
ssize_t proc_pce_write(struct file *file, const char *buf, size_t count,
		       loff_t *ppos);
int dp_ctp_dev_list_add(struct list_head *head, struct net_device *dev, int bp,
			int vap);
int dp_ctp_dev_list_del(struct list_head *head, struct net_device *dev);

#endif /*DATAPATH_H */

