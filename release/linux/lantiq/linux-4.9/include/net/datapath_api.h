// SPDX-License-Identifier: GPL-2.0
/*******************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 * Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Copyright 2012 - 2014 Lantiq Deutschland GmbH
 *
*******************************************************************************/

#ifndef DATAPATH_API_H
#define DATAPATH_API_H

#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/atmdev.h>
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
#include <net/datapath_api_grx500.h>
#else
#ifndef DATAPATH_HAL_LAYER
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32) || \
	IS_ENABLED(CONFIG_SOC_LGM) || \
	IS_ENABLED(CONFIG_X86_INTEL_LGM)
#include <net/datapath_api_gswip32.h>
#include <net/datapath_api_ppv4.h>
#elif IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
#include <net/datapath_api_gswip31.h>
#elif IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
#include <net/datapath_api_gswip30.h>
#else
#error "wrong DP HAL selected"
#endif
#endif /*DATAPATH_HAL_LAYER */
#include <net/datapath_api_tx.h>
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
#include <net/datapath_api_umt.h>
#endif
#include <net/datapath_api_qos.h>
#include <net/datapath_api_vlan.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/gsw_flow_ops.h>
#include <net/datapath_dp_cqm.h>
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
#include <linux/cpufreq.h>
#endif /*CONFIG_INTEL_DATAPATH_CPUFREQ*/

/*! @mainpage Datapath Manager API
 * @section Basic Datapath Registration API
 * @section Datapath QOS HAL
 *
 * @file datapath_api.h
 * @brief This file contains all the API for datapath manager on the GRX500 and
 *future system. This will actually be split into different header files,
 *but collected together for understanding here.
 */
/*! @defgroup Datapath_MGR Datapath Manager Basic API
 *@brief All API and defines exported by Datapath Manager
 */
/*! @{ */
/*! @defgroup Datapath_Driver_Defines Datapath Driver Defines
 *@brief Defines used in the Datapath Driver
 */
/*! @defgroup Datapath_Driver_Structures Datapath Driver Structures
 *@brief Datapath Configuration Structures
 */
/*! @defgroup Datapath_Driver_API Datapath Driver Manager API
 *@brief  Datapath Driver Manager API
 *@brief  Datapath Driver Manager API
 */
/*! @defgroup PPA_Accel_API PPA Acceleration Driver API
 *@brief PPA Acceleration Driver API used for learning and getting
 *the information necessary to accelerate a flow
 */
/*! @defgroup Datapath_API_QOS Datapath QOS Manager API
 *@brief  Datapath QOS Manager API
 */
/*! @} */
#define DP_INTERFACE_ID   1 /*!< @brief DP Interface ID:
			     *   If there is interface change between DP and
			     *   upper drivers, like DCDP/PON ethernet driver,
			     *   this macro will increase
			     */
#define DP_MAX_INST  1  /*!< @brief maximum DP instance to support. It can be
			 *  change as as needed
			 */
#define DP_MAX_ETH_ALEN 6  /*!< @brief MAC Header Size */
#define DP_MAX_PMAC_LEN     8  /*!< @brief Maximum PMAC Header Size */

#define DP_TXIN_RING_SIZE_DEF  4000 /*!< Default TXIN Ring size, upper driver
				     *   can tune down via dp_register_dev_ext
				     */
#define DP_RXOUT_RING_SIZE_DEF 4000 /*!< Default RXOUT Ring size, upper driver
				     *   can tune down via dp_register_dev_ext
				     */
#define DP_RXOUT_PKT_SIZE_DEF 2048 /*!< Default size of RXOUT normal pkt */
#define DP_RXOUT_PKt_SIZE_DEF_JUMBO 10240 /*!< Default size of RXOUT jumbo pkt*/

#define DP_MAX_UMT DP_RX_RING_NUM /*!< maximum number of UMT port per DC
				   *   For GRX500/PRX300, only support 1
				   *      umt port
				   *   For LGM docsis, it can support up to 2
				   */

#define DP_MAX_POOL_SUBIF 4     /*!< maximum number of pool per subif
				 */
#define DP_SPLPATH_IGP_NUM 2   /*!< maximum number of IGP number for CPU special
				*   path
				*/
#define DP_SPLPATH_EGP_NUM 1   /*!< maximum number of EGP number for CPU special
				*   path
				*/
#define DP_TS_HDRLEN	10    /*!< Befault Timestamp Header Length to strip */
#define DP_DFL_SESS_NUM 16    /*!< Maximum default egress session per subif */
#define DP_MAX_DEF_Q_PER_SUBIF 8 /*!< Maximum default queue per subif */
#define DP_CPU_LPID		0 /*!< Default CPU port LPID */
/*!< get vap or subif group */
#define GET_VAP(subif, bit_shift, mask) (((subif) >> (bit_shift)) & (mask))
/*!< set vap or subif group */
#define SET_VAP(vap, bit_shift, mask) ((((u32)vap) & (mask)) << (bit_shift))

/*! @addtogroup Datapath_Driver_Structures */
/*! @brief  DP Sub-interface Data structure
 *@param port_id  Datapath Port Id corresponds to PMAC Port Id
 *@param subif    Sub-interface Id info. In GRX500, this 15 bits,
 *		only 13 bits in PAE are handled [14, 11:0]
 *\note
 */
enum DP_API_STATUS {
	DP_FAILURE = -1,  /*!< failure */
	DP_SUCCESS = 0, /*!< succeed */
};

#define DP_F_ENUM_OR_STRING(name, value, short_name) {name = value} /*!< @brief
								     *  macro
								     *  for
								     *  enum
								     */

/*! @brief Enumerator DP_F_FLAG */
enum DP_F_FLAG {
	DP_F_DEREGISTER   = BIT(0), /*!< For de-allocate port only */
	DP_F_FAST_ETH_LAN = BIT(1), /*!< For Ethernet LAN device */
	DP_F_FAST_ETH_WAN = BIT(2), /*!< For Ethernet WAN device */
	DP_F_FAST_WLAN =    BIT(3), /*!< For DirectConnect WLAN device*/
	DP_F_FAST_DSL =     BIT(4), /*!< For DSL device */
	DP_F_DIRECT =       BIT(5), /*!< For PPA Directpath/LitePath*/
	DP_F_LOOPBACK =     BIT(6), /*!< For packet redirect back to GSWIP
				     *! For bridging/routing acceleration after
				     *de-tunnel or others, like ipsec case
				     */
	DP_F_DIRECTLINK =  BIT(7), /*!< For DirectLink/QCA device */
	DP_F_SUBIF_LOGICAL = BIT(8), /*!< For logical device, like VLAN device
				      *It is used by dp_register_subif
				      */
	DP_F_5G       = BIT(9), /*!< For 5G modem support */
	DP_F_ALLOC_EXPLICIT_SUBIFID = BIT(10), /*!< For logical device which
						* need explicit subif/VAP
						* request.
						*! For GRX350, seems need this
						*flag becase of VLAN talbe
						*handling inside PPA.
						*! For PRX300, normally no
						*need this flag.
						*Used by dp_register_subif
						*/
	DP_F_FAST_WLAN_EXT = BIT(11), /*!< 9-bit WLAN stations's device */
	DP_F_GPON     = BIT(12), /*!< For GPON device */
	DP_F_EPON     = BIT(13), /*!< For EPON device */
	DP_F_GINT     = BIT(14), /*!< For GINT device */
	DP_F_DOCSIS   = BIT(15), /*!< For DOCSIS device support */
	DP_F_CPU      = BIT(16), /*!< For CPU */
	DP_F_VUNI     = BIT(17), /*!< For vUNI device */
#define	DP_F_DEV_END  (DP_F_VUNI << 1) /*!< end of device type for
					*looping in dp_xmit
					*/

	DP_F_SHARE_RES = BIT(22), /*!< Wave600 multiple radio share same ACA */
	DP_F_ACA       = BIT(23), /*!< peripheral PCI device via ACA*/
	DP_F_NON_ACA_PORT = BIT(24), /*!< ACA device but without ACA PORT */
	/*Note Below Flags are ued by CBM/CQE driver only */
	DP_F_MPE_ACCEL =   BIT(25), /*!< For MPE path config, used by CBM only*/
	DP_F_CHECKSUM =    BIT(26), /*!< For HW chksum offload path config.
				     *Used by CBM only
				     */
	DP_F_DIRECTPATH_RX = BIT(27), /*!< For PPA Directpath/LitePath's RX PATH
				       *Used by CBM only
				       */
	DP_F_DONTCARE =    BIT(28), /*!< ??? Used by CBM only */
	DP_F_LRO =         BIT(29), /*!< For LRO path config. used by CBM only*/
	DP_F_FAST_DSL_DOWNSTREAM = BIT(30), /*!< For VRX318/518 downstream path.
					     *used by CBM only
					     */
	DP_F_DSL_BONDING =         BIT(31), /*!< For DSL BONDING path config
					     *to configu two UMT
					     *used by CBM only
					     */
	/*Note, once add a new entry here int the enum,
	 *need to add new item in below macro DP_F_FLAG_LIST
	 */
};

/*! @brief DP_F_FLAG_LIST Note:per bit one variable */
#define DP_F_FLAG_LIST  { \
	DP_F_ENUM_OR_STRING(DP_F_DEREGISTER,   "De-Register"), \
	DP_F_ENUM_OR_STRING(DP_F_FAST_ETH_LAN, "ETH_LAN"), \
	DP_F_ENUM_OR_STRING(DP_F_FAST_ETH_WAN, "ETH_WAN"),\
	DP_F_ENUM_OR_STRING(DP_F_FAST_WLAN,    "FAST_WLAN"),\
	DP_F_ENUM_OR_STRING(DP_F_FAST_DSL,     "DSL"),\
	DP_F_ENUM_OR_STRING(DP_F_DIRECT,        "DirectPath"), \
	DP_F_ENUM_OR_STRING(DP_F_LOOPBACK,      "Tunne_loop"),\
	DP_F_ENUM_OR_STRING(DP_F_DIRECTLINK,    "DirectLink"),\
	DP_F_ENUM_OR_STRING(DP_F_SUBIF_LOGICAL, "LogicalDev"), \
	DP_F_ENUM_OR_STRING(DP_F_5G,            "5G"), \
	DP_F_ENUM_OR_STRING(DP_F_ALLOC_EXPLICIT_SUBIFID, "Explicit_subif"), \
	DP_F_ENUM_OR_STRING(DP_F_FAST_WLAN_EXT,       "EXT_WLAN"),\
	DP_F_ENUM_OR_STRING(DP_F_GPON,                "GPON"),\
	DP_F_ENUM_OR_STRING(DP_F_EPON,                "EPON"),\
	DP_F_ENUM_OR_STRING(DP_F_GINT,                "GINT"),\
	DP_F_ENUM_OR_STRING(DP_F_DOCSIS,              "DOCSIS"),\
	DP_F_ENUM_OR_STRING(DP_F_SHARE_RES,           "SHARE_ACA"),\
	DP_F_ENUM_OR_STRING(DP_F_ACA,                 "ACA"),\
	DP_F_ENUM_OR_STRING(DP_F_MPE_ACCEL,     "MPE_FW"), \
	DP_F_ENUM_OR_STRING(DP_F_CHECKSUM,      "HW Chksum"),\
	DP_F_ENUM_OR_STRING(DP_F_DIRECTPATH_RX, "Directpath_RX"),\
	DP_F_ENUM_OR_STRING(DP_F_DONTCARE,      "DontCare"),\
	DP_F_ENUM_OR_STRING(DP_F_LRO,           "LRO"), \
	DP_F_ENUM_OR_STRING(DP_F_FAST_DSL_DOWNSTREAM, "DSL_Down"),\
	DP_F_ENUM_OR_STRING(DP_F_DSL_BONDING,         "DSL_Bonding"),\
	DP_F_ENUM_OR_STRING(DP_F_VUNI,         "VUNI"), \
	DP_F_ENUM_OR_STRING(DP_F_CPU,         "CPU") \
}

#define DP_F_PORT_TUNNEL_DECAP  DP_F_LOOPBACK /*!< @brief Just for
					       *  back-compatible since
					       *  CBM is using old macro
					       *  DP_F_PORT_TUNNEL_DECAP
					       */
#define DP_COC_REQ_DP	1 /*!< @brief COC request from Datapath itself */
#define DP_COC_REQ_ETHERNET	2 /*!< @brief COC request from ethernet */
#define DP_COC_REQ_VRX318	4 /*!< @brief COC request from vrx318 */

enum PMAC_TCP_TYPE {
	TCP_OVER_IPV4 = 0,
	UDP_OVER_IPV4,
	TCP_OVER_IPV6,
	UDP_OVER_IPV6,
	TCP_OVER_IPV6_IPV4,
	UDP_OVER_IPV6_IPV4,
	TCP_OVER_IPV4_IPV6,
	UDP_OVER_IPV4_IPV6
};

/*! @brief pmapper mode */
enum DP_PMAP_MODE {
	DP_PMAP_PCP = 1,  /*!< PCP Mapper:with omci unmark frame option 1
			   *    ie, derive pcp fields from default
			   */
	DP_PMAP_DSCP,     /*!< PCP Mapper with omci unmark frame option 0,
			   *    ie, derive pcp fields from dscp bits
			   */
	DP_PMAP_DSCP_ONLY, /*!< DSCP mapper only: PON not using it */
	DP_PMAP_MAX       /*!< Not valid */
};

enum DP_IO_PORT_TYPE {
	DP_IO_PORT_LINUX = BIT(0), /*!< Linux IO Type */
	DP_IO_PORT_DPDK = BIT(1),  /*!< DPDK IO Type */
};

#define DP_MAX_CPU 4 /*!< @brief Max number of CPUs */
#define DP_PMAP_PCP_NUM 8  /*!< @brief  Max pcp entries supported per pmapper*/
#define DP_PMAP_DSCP_NUM 64 /*!<@brief  Max dscp entries supported per pmapper*/
#define DP_MAX_CTP_PER_DEV  8   /*!< @brief  max CTP per dev:
				 *  Note: its value should be like
				 *     max(DP_PMAP_DSCP_NUM, DP_PMAP_PCP_NUM)
				 *   ? Maybe we can reduce from 64 to 8 since
				 *     PON currently only needs 8 CTP per
				 *     pmapper althogh GSIWP support 64
				 *     in pcp mode
				 */
#define DP_MAX_DEQ_PER_SUBIF 8 /*!<@brief the maximum number of dequeue port per
				* subif Note : one subif, it can create more
				* than 8 queues.
				* for example of epon, one subif can use up to 8
				* dequeue port, each dequeue port will use 1 Q
				* only
				*/
#define DP_PMAPPER_DISCARD_CTP 0xFFFF  /*!<@brief Discard ctp flag for pmapper*/
/*! @brief structure for pmapper */
struct dp_pmapper {
	u32 pmapper_id;  /*!<pmapper_id : pmapper id*/
	enum DP_PMAP_MODE mode;  /*!< mode: pcp or dscp mapper*/
	u16 def_ctp;  /*!< default map: used for below cases:
		       *  pcp mode: for non-vlan packet case
		       *  dscp mode: for non-ip packet case
		       *  but according to PON OMCI requirement, in fact, it
		       *  should drop.
		       */
	u16 pcp_map[DP_PMAP_PCP_NUM];  /*!< For pcp mapper.  Should be
					*non-zero since CTP 0 reserved
					*/
	u16 dscp_map[DP_PMAP_DSCP_NUM]; /*!< For dscp mapper*/
};

/*! @brief structure for dp_subif_port_common */
struct dp_subif_port_common { /* shared by struct dp_subif and pmac_port_info */
	int port_id;
	int alloc_flag; /*!< [out] the flag value is from the top level driver
			 *    during calling dp_alloc_port_ext
			 *   output for dp_get_netif_subifid
			 *   no use for dp_register_subif_ext
			 *   This is requested by PPA/DCDP to get original flag
			 *   the caller provided to DP during
			 *   dp_alloc_port
			 */
	u8  cqe_lu_mode; /* cqe lookup mode */
	u32 gsw_mode; /* gswip mode for subif */
	s16 gpid_spl;  /* special GPID:
			* alloc it at dp_alloc_port
			* config it at dp_register_dev for policy setting
			*/
};

struct dp_subif_common {  /* shared by struct dp_subif and dp_subif_info */
	u16 bport;  /*!< output: valid only for API dp_get_netif_subifid in the
		     *  GSWIP 3.1 or above
		     *  bridge port ID
		     */
	u16 gpid; /*!< [out] gpid which is mapped from dpid + subif
		   *   normally one GPID per subif for non PON device.
		   *   For PON case, one GPID per bridge port
		   */
	u32 data_flag; /*!< [out] return the caller provided data->flag_ops
			* during dp_register_subif
			*/
	u8 num_q; /*!< number of queue id*/
	union {
		u16 def_qid;/* physical queue id Still keep it to be
			     * back-compatible for legacy platform and legacy
			     * integration
			     */
		u16 def_qlist[DP_MAX_DEQ_PER_SUBIF];/* physical queue id */
	};
	u32 dfl_eg_sess[DP_DFL_SESS_NUM]; /*! default CPU egress session ID
					   * Valid only if f_dfl_eg_sess set
					   * one sesson per class[4 bits]
					   */
	u32 subif_groupid; /*!< [out] subif group id or vap */
};

/*! @brief structure for dp_subif */
typedef struct dp_subif {
	int inst;  /*!< dp instance id */
	union {
		struct {
			int port_id;
			int alloc_flag; /*!< [out] the flag value is from the
					 * top level driver during calling
					 * dp_alloc_port_ext
					 *   output for dp_get_netif_subifid
					 *   no use for dp_register_subif_ext
					 * This is requested by PPA/DCDP to
					 * get original flag the caller provided
					 * to DP during dp_alloc_port
					 */
			u8  lookup_mode; /*!< [out] CQM lookup mode for this
					  * device (dp_port based)
					  * valid for dp_get_netif_subifid only
					  */
			u32 gsw_mode; /* gswip mode for subif */
			s16 gpid_spl;  /* special GPID:
					* alloc it at dp_alloc_port
					* config it at dp_register_dev
					* for policy setting
					*/

		};
		struct dp_subif_port_common subif_port_cmn; /*!<out]
							     * return the
							     * common fields
							     * stored during
							     * dp_alloc_port
							     */
	};
	union {
		struct {
			u16 bport;  /*!< output: valid only for API
				     * dp_get_netif_subifid in the
				     *  GSWIP 3.1 or above
				     *  bridge port ID
				     */
			u16 gpid; /*!< [out] gpid which is mapped from
				   * dpid + subif normally one GPID per subif
				   * for non PON device.
				   *   For PON case, one GPID per bridge port
				   */
			u32 data_flag; /*!< [out] return the caller provided
					* data->flag_ops
					* during dp_register_subif
					*/
			u8  num_q; /*!< [out] num of default number of queues
				    * allocated by DP
				    */
			union {
				u16 def_qid; /*!< [out] def physical queue id
					      * assigned by DP
					      */
				/*!< [in/out] default physical qid list assigned
				 * by DP
				 */
				u16 def_qlist[DP_MAX_DEQ_PER_SUBIF];
			};
			u32 dfl_eg_sess[DP_DFL_SESS_NUM]; /*!< [out]
							   * default egress
							   * session id. This is
							   * for CPU TX to DC
							   * only
							   */
			u32 subif_groupid; /*!< [out] subif group id or vap */
		};
		struct dp_subif_common subif_common; /*[out] return group of
						      * fields which is stored
						      * during dp_register_subif
						      */
	};
	int subif_num; /*!< valid subif/ctp num.
			*   output for dp_get_netif_subifid,
			*   no use for dp_register_subif_ext
			*/
	union {
		s32 subif; /*!< [in/out] Sub-interface Id as HW defined
			    */
		s32 subif_list[DP_MAX_CTP_PER_DEV]; /*!< [in/out] subif list
						     *   Normally 1 subif per
						     *   dev. But for PON
						     *   pmapper case, multiple
						     *   subif per pmapper
						     *   device
						     */
	};
	int subif_flag[DP_MAX_CTP_PER_DEV]; /*!< the flag is used during
					     *   dp_register_subif_ext
					     *   output for dp_get_netif_subifid
					     *   no use for
					     *   dp_register_subif_ext.
					     *   This is requested by PPA/DCDP
					     *   to get original flag the caller
					     *   provided to DP during
					     *   dp_register_subif_ext
					     */
	u32 flag_bp : 1; /*!< [out] flag to indicate whether this device is
			  *   bridge port device or GEM device
			  *   For PON CTP device under pmapper: 0.
			  *   For PON pmapper or normal registered subif: 1
			  *   For example: eth1, pmapper device and so on.
			  *   Valid only for API output of dp_get_netif_subifid
			  *   It will be used for asymmetric VLAN case
			  *   in case need call API dp_vlan_set to apply VLAN
			  *   rule to CTP or bridge port
			  */
	u32 flag_pmapper : 1; /*!< [out] flag to indicate whether this
			       *   device is pmapper device.
			       *   For PON pmappper device: 1
			       *   For PON CTP device or other non PON device:0
			       *   valid for dp_get_netif_subifid only
			       */
	enum DP_IO_PORT_TYPE type; /*!< [out] IO Port type */
	struct net_device *associate_netif; /*!< [out] return vUNI dev pointer,
					     * valid for VANI device only
					     */
	u32 peripheral_pvt; /*!< [out] peripheral private flag
			     * output for dp_get_netif_subifid
			     * no use for dp_register_subif_ext
			     * For example: DSL driver used to indicate Bonding
			     */
} dp_subif_t;

typedef dp_subif_t PPA_SUBIF; /*!< @brief structure type dp_subif PPA_SUBIF*/

struct dp_port_prop {
	u32 vap_offset; /*shift bits to get vap value */
	u32 vap_mask; /*get final vap after bit shift */
	int alloc_flags; /* alloc flag */
	struct module *owner; /* port owner */
	u32 num_subif;  /* number of subif registered already */
	u16 subif_max;  /* maximum number of subif supported for this port */
	int ctp_max;  /* maximum number of stp reserved for this port */
	int status;  /* port status, registered or not */
	int port_id; /* dp_port id */
	u32 lct_idx; /* lct_idx if applicable */
	u32 deq_port_num; /* number of CQM dequeue port for this dp_port */
};

struct dp_subif_prop {
	int flags;
	struct net_device *netif;
	struct net_device *ctp_dev;
};

struct vlan_prop {
	u8 num;
	u16 out_proto, out_vid;
	u16 in_proto, in_vid;
	struct net_device *base;
};

/*! @brief struct for dp_drv_mib */
typedef struct dp_drv_mib {
	u64 rx_drop_pkts;  /*!< rx drop pkts */
	u64 rx_error_pkts; /*!< rx error pkts */
	u64 tx_drop_pkts; /*!< tx drop pkts */
	u64 tx_error_pkts; /*!< tx error  pkts */
	u64 tx_pkts; /*!< tx pkts */
	u64 tx_bytes; /*!< tx bytes */
} dp_drv_mib_t;

/*! @brief struct for dp_buffer_info */
struct dp_buffer_info {
	phys_addr_t addr; /*!< [in] physical address of buffer to free */
	u32 policy_base;  /*!< [in] associated policy base*/
	int policy_num;   /*!< [in] associated policy number*/
};

/*! @brief struct for dp_buffer */
struct dp_dma_ch {
	u32 ch; /*!< DMA CH ID */
#define DP_DMA_IRQ_ENABLE  BIT(0)  /*!< enable DMA IRQ */
#define DP_DMA_IRQ_DISABLE BIT(1)  /*!< dsiable DMA IRQ */
	int flag; /*!< enable/disable flag */
};

/*! struct dp_aca_stop: dp stop ACA configuration */
struct dp_aca_stop {
	int inst; /*!< [in] DP instance ID */
};

typedef int32_t(*dp_rx_fn_t)(struct net_device *rxif, struct net_device *txif,
	struct sk_buff *skb, int32_t len);/*!< @brief   Device Receive
					   *   Function callback for packets
					   */
typedef int32_t(*dp_stop_tx_fn_t)(struct net_device *dev);/*!< @brief   The
							   * Driver Stop
							   *Tx function
							   *callback
							   */
typedef int32_t(*dp_restart_tx_fn_t)(struct net_device *dev); /*!< @brief Driver
							       * Restart Tx
							       * function
							       * callback
							       */
typedef int32_t(*dp_reset_mib_fn_t)(dp_subif_t *subif, int32_t flag);/*!< @brief
								      *Driver
								      *reset
								      *its mib
								      *counter
								      *callback
								      **/
typedef int32_t(*dp_get_mib_fn_t)(dp_subif_t *subif, dp_drv_mib_t *,
	int32_t flag); /*!< @brief   Driver get mib counter of the
			*specified subif interface.
			*/
typedef int32_t(*dp_get_netif_subifid_fn_t)(struct net_device *netif,
	struct sk_buff *skb, void *subif_data, uint8_t dst_mac[DP_MAX_ETH_ALEN],
	dp_subif_t *subif, uint32_t flags);	/*!< @brief   get subifid */
#if defined(CONFIG_INTEL_DATAPATH_CPUFREQ)
typedef int32_t(*dp_coc_confirm_stat)(int new_state,
	int old_st, uint32_t f); /*!< @brief Confirm state
				  *   by COC
				  */
#endif
/*!
 *@brief Datapath Manager Registration Callback
 *@param rx_fn  Rx function callback
 *@param stop_fn    Stop Tx function callback for flow control
 * *@param restart_fn    Start Tx function callback for flow control
 *@param get_subifid_fn    Get Sub Interface Id of netif
 *@note
 */
typedef struct dp_cb {
	dp_rx_fn_t rx_fn;	/*!< Rx function callback */
	dp_stop_tx_fn_t stop_fn;/*!< Stop Tx function callback for
				 *flow control
				 */
	dp_restart_tx_fn_t restart_fn;	/*!< Start Tx function callback
					 *! For flow control
					 */
	dp_get_netif_subifid_fn_t get_subifid_fn; /*!< Get Sub Interface Id
						   *of netif/netdevice
						   */
	dp_reset_mib_fn_t reset_mib_fn;  /*!< reset registered device's network
					  *mib counters
					  */
	dp_get_mib_fn_t get_mib_fn; /*!< reset registered device's
				     *network mib counters
				     */
	irqreturn_t (*dma_rx_irq)(int irq, void *dev_instance); /*!< DMA RX IRQ
								 *   handler.
								 *   For 5G only
								 *   now.
								 */
	int (*aca_fw_stop)(struct dp_aca_stop *cfg, int flags); /*!< callback to
								 *   stop ACA FW
								 */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
	dp_coc_confirm_stat dp_coc_confirm_stat_fn; /*!< once COC confirm the
						     *state changed, Datatpath
						     *will notify Ethernet/
						     *VRX318 driver and
						     *Ethernet/VRX318 driver
						     *need to enable/disable
						     *interrupt or change
						     *threshold accordingly
						     */
#endif
} dp_cb_t;

/*!
 *@brief Ingress PMAC port configuration from Datapath Manager
 *@param tx_dma_chan  Tx DMA channel Number for which PMAC
 *		configuration is to be done
 *@param err_disc     Is Discard Error Enable
 *@param pmac    Is Ingress PMAC Hdr Present
 *@param def_pmac   Is Default PMAC Header configured for Tx DMA Channel
 *@param def_pmac_pmap Is PortMap to be used from Default PMAC hdr (else use
 *		Ingress PMAC hdr)
 *@param def_pmac_en_pmap Is PortMap Enable to be used from Default PMAC hrd
 *		(else use Ingress PMAC hdr)
 *@param def_pmac_tc  Is TC (class) to be used from Default PMAC hdr
 *		(else use Ingress PMAC hdr)
 *@param def_pmac_en_tc  Are TC bits to be used for TC from Default PMAC hdr
 *		(else use Ingress PMAC hdr)
 *		Alternately, EN/DE/MPE1/MPE2 bits can be used for TC
 *@param def_pmac_subifid  Is Sub-interfaceId to be taken from Default PMAC hdr
 *		(else use Ingress PMAC hdr)
 *@param def_pmac_src_port Packet Source Port determined from Default PMAC hdr
 *		(else use Ingress PMAC hdr)
 *@param res_ing	Reserved bits
 *@param def_pmac_hdr Default PMAC header configuration for the Tx DMA channel
 *		Useful if Src Port does not send PMAC header with packet
 *@note
 */
typedef struct ingress_pmac {
	uint32_t tx_dma_chan:8;	/*!< Tx DMA channel Number for which PMAC
				 * configuration is to be done
				 */
	uint32_t err_disc:1;	/*!< Is Discard Error Enable */
	uint32_t pmac:1;	/*!< Is Ingress PMAC Hdr Present */
	uint32_t def_pmac:1;	/*!< Is Ingress PMAC Hdr Present */
	uint32_t def_pmac_pmap:1;	/*!< Is Default PMAC Header configured
					 * for Tx DMA Channel
					 */
	uint32_t def_pmac_en_pmap:1;	/*!< Is PortMap Enable to be used from
					 *  Default PMAC hrd (else use Ingress
					 * PMAC hdr)
					 */
	uint32_t def_pmac_tc:1;	/*!< Is TC (class) to be used from Default PMAC
				 *  hdr (else use Ingress PMAC hdr)
				 */
	uint32_t def_pmac_en_tc:1;	/*!< Are TC bits to be used for TC from
					 * Default PMAC hdr (else use Ingress
					 * PMAC hdr)
					 * Alternately, EN/DE/MPE1/MPE2 bits
					 * can be used for TC
					 */
	uint32_t def_pmac_subifid:2; /*!< Is Sub-interfaceId to be taken from
				      *Default PMAC hdr (else use Ingress PMAC
				      *hdr)
				      */
	uint32_t def_pmac_src_port:1; /*!< Packet Source Port determined from
				       *Default PMAC hdr (else use Ingress
				       *PMAC hdr)
				       */
	uint32_t res_ing:15;	/*!< Reserved bits */
	u8 def_pmac_hdr[DP_MAX_PMAC_LEN]; /*!< Default PMAC header config
					   *   For the Tx DMA channel.
					   *   Useful if Src Port does not
					   *   send PMAC header with
					   *   packet
					   */
} ingress_pmac_t;

/*!
 *@brief Egress PMAC port configuration from Datapath Manager
 *@param rx_dma_chan  Rx DMA channel Number for which PMAC configuration
 *		is to be done
 *@param rm_l2hdr	If Layer 2 Header is to be removed before Egress
 *		(for eg. for IP interfaces like LTE)
 *@param num_l2hdr_bytes_rm If rm_l2hdr=1,then number of L2 hdr bytes to be
 *		removed
 *@param fcs	If FCS is enabled on the port
 *@param pmac If PMAC header is enabled on the port
 *@param dst_port  Destination Port Identifier
 *@param res_eg  Reserved bits
 *@note
 */
typedef struct egress_pmac {
	uint32_t rx_dma_chan:8;	/*!< Rx DMA channel Number for which PMAC
				 *  configuration is to be done
				 */
	uint32_t rm_l2hdr:1;   /*!< If Layer 2 Header is to be removed
				*before Egress (for eg. for IP interfaces like
				*LTE)
				*/

	uint32_t num_l2hdr_bytes_rm:8;/*!< If rm_l2hdr=1,
				       *then number of L2 hdr bytes to be
				       *removed
				       */
	uint32_t fcs:1;		/*!< If FCS is enabled on the port */
	uint32_t pmac:1;	/*!< If PMAC header is enabled on the port */
	uint32_t redir:1;	/*!< Enable redirection flag. GSWIP-3.1 only.
				 *Overwritten by bRes1DW0Enable and nRes1DW0.
				 */
	uint32_t bsl_seg:1;	/*!< Allow (False) or not allow (True)
				 * segmentation during buffer selection.
				 * GSWIP-3.1 only. Overwritten by
				 * bResDW1Enable and nResDW1.
				 */
	uint32_t dst_port:8;	/*!< Destination Port Identifier */
	uint32_t res_endw1:1;	/*!< If false, nResDW1 is ignored*/
	uint32_t res_dw1:4;	/*!< reserved field in DMA descriptor - DW1*/
	uint32_t res1_endw0:1;	/*!< If false, nRes1DW0 is ignored.*/
	uint32_t res1_dw0:3;	/*!< reserved field in DMA descriptor - DW0*/
	uint32_t res2_endw0:1;	/*!< If false, nRes2DW0 is ignored.*/
	uint32_t res2_dw0:2;	/*!< reserved field in DMA descriptor - DW0*/
	uint32_t tc_enable:1;	/*!< Selector for traffic class bits */
	uint32_t traffic_class:8;/*!< If tc_enable=true,sets egress queue
				  *	traffic class.
				  */
	uint32_t flow_id:8; /*!< flow id msb*/
	uint32_t dec_flag:1; /*!< If tc_enable=false,sets decryption flag*/
	uint32_t enc_flag:1; /*!< If tc_enable=false,sets encryption flag*/
	uint32_t mpe1_flag:1; /*!< If tc_enable=false,mpe1 marked flag valid*/
	uint32_t mpe2_flag:1; /*!< If tc_enable=false,mpe1 marked flag valid*/
	uint32_t res_eg:5; /*!< Reserved bits */
} egress_pmac_t;

/*!
 *@brief struct dp_subif_stats_t
 */
typedef struct dp_subif_stats_t {
	u64 rx_bytes; /*!< received bytes*/
	u64 rx_pkts; /*!< received packets*/
	u64 rx_disc_pkts; /*!< received discarded packets*/
	u64 rx_err_pkts; /*!< received errored packets*/
	u64 tx_bytes; /*!< transmitted bytes*/
	u64 tx_pkts; /*!< transmitted packets*/
	u64 tx_disc_pkts; /*!< transmitted discarded packets*/
	u64 tx_err_pkts; /*!< transmitted errored packets*/
} dp_subif_stats_t;

/*!
 *@brief enum EG_PMAC_F
 */
enum EG_PMAC_F {
	/*1 bit one flag */
	EG_PMAC_F_L2HDR_RM = 0x1, /*!< eg_pmac.numBytesRem/bRemL2Hdr valid*/
	EG_PMAC_F_FCS = 0x2, /*!< mean eg_pmac.bFcsEna valid*/
	EG_PMAC_F_PMAC = 0x4, /*!< mean eg_pmac.bPmacEna valid */
	EG_PMAC_F_RXID = 0x8, /*!< mean eg_pmac.nRxDmaChanId valid */
	EG_PMAC_F_RESDW1 = 0x10, /*!< mean eg_pmac.nResDW1 valid */
	EG_PMAC_F_RES1DW0 = 0x20, /*!< mean eg_pmac.nRes1DW0 valid */
	EG_PMAC_F_RES2DW0 = 0x40, /*!< mean eg_pmac.nRes2DW0 valid */
	EG_PMAC_F_TCENA = 0x80, /*!< mean eg_pmac.bTCEnable valid */
	EG_PMAC_F_DECFLG = 0x100, /*!< mean eg_pmac.bDecFlag valid */
	EG_PMAC_F_ENCFLG = 0x200, /*!< mean eg_pmac.bEncFlag valid */
	EG_PMAC_F_MPE1FLG = 0x400, /*!< mean eg_pmac.bMpe1Flag valid */
	EG_PMAC_F_MPE2FLG = 0x800, /*!< mean eg_pmac.bMpe2Flag valid */
	EG_PMAC_F_RESDW1EN = 0x1000, /*!< mean eg_pmac.res_endw1 valid */
	EG_PMAC_F_RES1DW0EN = 0x2000, /*!< mean eg_pmac.res1_endw0 valid */
	EG_PMAC_F_RES2DW0EN = 0x4000, /*!< mean eg_pmac.res2_endw0 valid */
	EG_PMAC_F_REDIREN = 0x8000, /*!< mean eg_pmac.redir valid */
	EG_PMAC_F_BSLSEG = 0x10000, /*!< mean eg_pmac.bsl_seg valid */
};

/*! @brief EG_PMAC Flags */
enum IG_PMAC_F {
	/*1 bit one flag */
	IG_PMAC_F_ERR_DISC = BIT(0), /*!< mean ig_pmac.bErrPktsDisc valid */
	IG_PMAC_F_PRESENT = BIT(1),  /*!< mean ig_pmac.bPmacPresent valid */
	IG_PMAC_F_SUBIF = BIT(2),  /*!< mean ig_pmac.bSubIdDefault valid */
	IG_PMAC_F_SPID = BIT(3),  /*!< mean ig_pmac.bSpIdDefault valid */
	IG_PMAC_F_CLASSENA = BIT(4),  /*!< mean ig_pmac.bClassEna valid */
	IG_PMAC_F_CLASS = BIT(5),  /*!< mean ig_pmac.bClassDefault valid */
	IG_PMAC_F_PMAPENA = BIT(6),  /*!< mean ig_pmac.bPmapEna valid */
	IG_PMAC_F_PMAP = BIT(7),  /*!< mean ig_pmac.bPmapDefault valid */
	IG_PMAC_F_PMACHDR1 = BIT(8),  /*!< mean ig_pmac.defPmacHdr[1] valid */
	IG_PMAC_F_PMACHDR2 = BIT(9),  /*!< mean ig_pmac.defPmacHdr[2] valid */
	IG_PMAC_F_PMACHDR3 = BIT(10),  /*!< mean ig_pmac.defPmacHdr[3] valid */
	IG_PMAC_F_PMACHDR4 = BIT(11),  /*!< mean ig_pmac.defPmacHdr[4] valid */
	IG_PMAC_F_PMACHDR5 = BIT(12),  /*!< mean ig_pmac.defPmacHdr[5] valid */
	IG_PMAC_F_PMACHDR6 = BIT(13),  /*!< mean ig_pmac.defPmacHdr[6] valid */
	IG_PMAC_F_PMACHDR7 = BIT(14),  /*!< mean ig_pmac.defPmacHdr[7] valid */
	IG_PMAC_F_PMACHDR8 = BIT(15),  /*!< mean ig_pmac.defPmacHdr[8] valid */
};

/*! @brief Paser Flags */
enum PASER_FLAG {
	F_MPE_NONE = 0x1, /*!< Need set MPE1=0 and MPE2=0*/
	F_MPE1_ONLY = 0x2, /*!< Need set MPE1=1 and MPE2=0 */
	F_MPE2_ONLY = 0x4, /*!< Need set MPE1=0 and MPE2=1 */
	F_MPE1_MPE2 = 0x8, /*!< Need set MPE1=1 and MPE2=1 */
};

/*! @brief PASER_VALUE Flags */
enum PASER_VALUE {
	DP_PARSER_F_DISABLE = 0,  /*!< Without Paser Header and Offset */
	DP_PARSER_F_HDR_ENABLE = 1,/*!< With Paser Header, but without Offset */
	DP_PARSER_F_HDR_OFFSETS_ENABLE = 2,  /*!< with Paser Header and Offset*/
};

/*!
 *@brief Datapath Manager Port PMAC configuration structure
 *@param ig_pmac  Ingress PMAC configuration
 *@param eg_pmac  Egress PMAC configuration
 *@note GSW_PMAC_Ig_Cfg_t/GSW_PMAC_Eg_Cfg_t defined in GSWIP driver:
 *	<xway/switch-api/lantiq_gsw_api.h>
 */
typedef struct dp_pmac_cfg {
	u32 ig_pmac_flags;	/*!< one bit for one ingress_pmac_t fields */
	u32 eg_pmac_flags;	/*!< one bit for one egress_pmac_t fields */
	ingress_pmac_t ig_pmac;	/*!< Ingress PMAC configuration */
	egress_pmac_t eg_pmac;	/*!< Egress PMAC configuration */
} dp_pmac_cfg_t;

/*! @brief struct pon_subif_d */
struct pon_subif_d {
	s32 tcont_idx; /*!< relative tconf_idx map to CQE PON dequeuer port */
	s8 pcp;/*!< 0~7:valid pcp
		*   -1: non valid pcp value
		*/
};

#define DP_F_DATA_LCT_SUBIF DP_SUBIF_LCT
/*! @brief enum DP_SUBIF_DATA_FLAG */
enum DP_SUBIF_DATA_FLAG {
	DP_SUBIF_AUTO_NEW_Q = BIT(0), /*!< create new queue for this subif */
	DP_SUBIF_SPECIFIC_Q = BIT(1), /*!< use the already configured queue as
				       *  specified by q_id in
				       *  \struct dp_subif_data. This queue can
				       *  be created by caller itself, or
				       *  by last call of dp_register_subif_ext
				       */
	DP_SUBIF_LCT = BIT(2), /*!< Register as LCT port */
	DP_SUBIF_VANI = BIT(3), /*!< Register as vANI Subif */
	DP_SUBIF_DEQPORT_NUM = BIT(4), /*!< Specify num of deq port per subif */
	DP_SUBIF_RX_FLAG = BIT(5), /*!< To Specify if Rx enable/disable during
				    * DP register subif using rx_en_flag
				    * specified under struct dp_subif_data
				    */
	DP_SUBIF_SWDEV = BIT(6), /*!< To specify subif level swdev
				  * enable/disable
				  */
	DP_SUBIF_PREL2 = BIT(7), /*!< Pre_L2 support */
	DP_SUBIF_CPU_QMAP = BIT(8),
	DP_SUBIF_NO_HOSTIF = BIT(9),
	DP_SUBIF_BP_CPU_DISABLE = BIT(10), /*!< To remove CPU bridge port
					   * from bridge port member list
					   */
	DP_SUBIF_SEG_EN = BIT(11), /*!< if this flag is set seg_en will be set to 1
								* to use FSQM buffer,
								*/
	DP_SUBIF_SEG_DIS = BIT(12), /*!< if this flag is set seg_en=0 to use
								 * BM buffer
								 */
	DP_SUBIF_NON_CPU_MAC = BIT(13), /*!< if this flag is set, dpm will not
					 *   set this device's mac address to
					 *   CPU bridge port
					 */
	DP_SUBIF_BR_DOMAIN = BIT(14), /*!< once this flag is set, it means
				       * domain_id and domain_members are valid
				       */
};

/*! @brief dp_subif_id struct for get_netif_subif */
struct dp_subif_cache {
	struct hlist_node hlist;
	dp_subif_t subif;
	struct net_device *dev;
	char name[IFNAMSIZ];
	dp_get_netif_subifid_fn_t subif_fn;  /*!< Get Sub Interface Id
					      * of netif/netdevice
					      */
	void *data;
	struct rcu_head rcu;
};

/*! @brief struct dp_subif_data */
struct dp_subif_data {
	s8 deq_port_idx;  /*!< [in] range: 0 ~ its max deq_port_num - 1
			   *  For PON, it is tcont_idx,
			   *  For other device, normally its value is zero
			   */
	enum DP_SUBIF_DATA_FLAG flag_ops; /*!< flags */
	int q_id; /*!< [in,out]:
		   * [in]: valid only if DP_SUBIF_SPECIFIC_Q set in
		   *       \ref flag_ops
		   * [out]: queue alloted or reused for this subif
		   * Note: this queue can be created by caller,
		   *         or by dp_register_subif_ext itself in Pmapper case
		   */
	struct net_device *ctp_dev; /*!<  Optional CTP device.
				     * Mainly for PON CTP device under pmapper.
				     */
	dp_rx_fn_t rx_fn;      /*!< [in] subif RX callback */
	dp_get_netif_subifid_fn_t get_subifid_fn; /*! [in] get subif ID cb */
	u8 f_policy;   /*!< [in] flag to indicate whether need new
			*   policy for this subif or not.
			*   if this flat is set, it needs new tx/rx policy
			*   otherwise DP will use its existing base policy
			*   which is created during dp_register_dev_ext
			*/
	u16 tx_pkt_size;  /*!< [in] maximum packet size required
			   *   to alloc new policy for different cqm dequeue
			   *   port
			   *   valid only if f_tx_policy set
			   */
	struct dp_gpid_tx_info gpid_tx_info; /*!< [in] for GPID tx information
					      *   Valid only if \ref
					      *   f_tx_policy set
					      */
	u16 tx_policy_base;   /*!< [out] txin_policy
			       *   if f_txin_policy set, this subif will need
			       *       create new policy
			       *   else DP will use its base policy which is
			       *       create during dp_register_dev_ext
			       */
	u8 tx_policy_num;	/*!< [out] */
	u16 rx_policy_base;	/*!< [out] */
	u8 rx_policy_num;	/*!< [out] */

	int txin_ring_size;  /*!< [in/out] ACA TXIN Ring size.
			      *   if input value is not zero, DP try to tune
			      *   down the pre-allocated TXIN ring buffer size.
			      *   Only allowed to tune down.
			      */
	void *txin_ring_phy_addr; /*!< [out] ACA TXIN Ring Buffer physical
				   *   address based on deq_port_idx
				   */
	void *credit_add_phy_addr; /*!< [out] PPv4 credit add physical address
				    *   which is valid only if flag
				    *   DP_F_NON_ACA_PORT is set during
				    *   dp_alloc_port_ext.
				    *   So far for 5G without using CQM DC port
				    */
	void *credit_left_phy_addr; /*!< [out] PPv4 credit left physical address
				     *   which is valid only if flag
				     *   DP_F_NON_ACA_PORT is set during
				     *   dp_alloc_port_ext
				     *   So far for 5G without using CQM DC port
				     */
	#define DP_MAC_LEARNING_EN 0
	#define DP_MAC_LEARNING_DIS 1
	u16 mac_learn_disable; /*!< [in] To enable or disable
				* mac learning for subif
				*/
	u16 num_deq_port; /*!< [in] To specify number of DEQ_PORT one subif */
	u32 rx_en_flag; /*!< [in] rx_en_flag = 1 - To enable dp_rx
			 * rx_en_flag = 0 - disable dp_rx
			 */
	u32 bm_policy_res_id; /*!< [in] buffer policy id */
	u16 swdev_en_flag; /*!< [in] swdev_en_flag =1 - Support h/w offload
			    * swdev_en_flag = 0 - no h/w offload support
			    */
	u8 domain_id:5;	/*!< [in] we support 32 domain per bridge only
			 * valid only DP_SUBIF_BR_DOMAIN is set in flag_ops
			 */
	#define DP_BR_DM_MEMBER(domain) (1 << (domain))
	u32 domain_members; /*!< [in] one bit for one domain_id:
			     * bit 0 for domain_id 0, bit 1 for domain 1 and so on.
			     * If one bit is set to 1, the traffic received
			     * from this dev can be forwarded to this domain
			     * as specified in domain_members
			     * valid only if DP_SUBIF_BR_DOMAIN is set in flag_ops
			     */
};

/*! @brief enum DP_F_DATA_RESV_CQM_PORT */
enum dp_port_data_flag {
	DP_F_DATA_RESV_CQM_PORT = BIT(0), /*!< need reserve cqm multiple ports*/
	DP_F_DATA_ALLOC = BIT(1),
	DP_F_DATA_EVEN_FIRST = BIT(2), /*!< reserve dp_port in even number*/
	DP_F_DATA_RESV_Q = BIT(3), /*!< reserve QOS queue */
	DP_F_DATA_RESV_SCH = BIT(4), /*!< reserve QOS scheduler */
	DP_F_DATA_FCS_DISABLE = BIT(5), /*!< Disable FCS for PON port on SOC */
	DP_F_DATA_NO_CQM_DEQ = BIT(6), /*!< No mapped CQM dequeue port needed,
					*   instead DC device directly dequeue
					*   packet from PP QOS port via credit
					*   left and credit add
					*/
	DP_F_DATA_CONTINUOUS_Q_RESV = BIT(7), /*!< reserve continuous physical
					       *   queue ID
					       */
	DP_F_DATA_NO_LOOP = BIT(8),	/*!< Once this flag is set,Bridge
					 * forwarding among subif which belongs
					 * to same DP port/llid is disabled.
					 * It can be used for PON application,
					 * but may not for others like WIFI
					 * bridging between diff subif/vap
					 */
	DP_F_DATA_PCE_PATH_EN = BIT(9), /*!< Once this flag is set,
					 * PCE path will be enabled for the
					 * corresponding LPID
					 */
	DP_F_DATA_PON_HGU_SEC_LOOP = BIT(10), /*!< Once this flag is set,
					       * GSWIP second loop redirect to
					       * PMAC 1 will be configured
					       */
};

/*! @brief typedef struct dp_port_data */
struct dp_port_data {
	int flag_ops; /*!< [in] flag operation, refer to enum dp_port_data_flag
		       */
	u32 resv_num_port; /*!< valid only if DP_F_DATA_RESV_CQM_PORT is set.
			    * the number of cqm dequeue port to reserve.
			    * Currently mainly for Wave600 multiple radio but
			    * sharing same cqm dequeue port
			    */
	u32 start_port_no; /*!< valid only if DP_F_DATA_RESV_CQM_PORT is set */

	int num_resv_q; /*!< [in] reserve the required number of queues. Valid
			 *   only if DP_F_DATA_RESV_Q bit valid in \ref flag_ops
			 */
	int num_resv_sched; /*!< [in] reserve required number of schedulers.
			     *   Valid only if DP_F_DATA_RESV_SCH bit valid in
			     *   \ref flag_ops
			     */
	int deq_port_base; /*!< [out] the CQM dequeue port base for the
			    *   traffic to this device.
			    */
	int deq_num;  /*!< [out] the number of dequeue port allocated for the
		       *         traffic to this device
		       */
};

/* DC/ACA Rings
 *  4 Ring with 1 DC dequeue port
 *  |-----| ------1 TXIN (Pkt+Desc)-----------> |-----------|
 *  |     |    (via UMT TX Msg Cnt to dequeue)  |           |
 *  |     |                                     |           |
 *  |     | <-----1 TXOUT(Pkt Free)-----------  |           |
 *  | Host|                                     | DC Device |
 *  |     | <-----1 RXOUT (PKt+Desc) ---------- | (via ACA) |
 *  |     |                                     |           |
 *  |     |  -----1 RXINT (Pkt Alloc)---------> |           |
 *  |     |    (via UMT RX Msg to Alloc )       |           |
 *  |-----| ----------------------------------> |-----------|
 *
 *  4 Ring with Multiple QOS dequeue ports
 *  |-----| ------Multiple TXIN (Pkt+Desc)----> |-----------|
 *  |     |    (Poll Each QOS port Credit_Add ) |           |
 *  |     |                                     |           |
 *  |     | <-----1 TXOUT(Pkt Free)-----------  |           |
 *  | Host|                                     | DC Device |
 *  |     | <-----1 RXOUT (PKt+Desc) ---------- | (via ACA) |
 *  |     |                                     |           |
 *  |     |  -----1 RXINT (Pkt Alloc)---------> |           |
 *  |     |    (via UMT RX Msg to Alloc )       |           |
 *  |-----| ----------------------------------> |-----------|
 *
 *  3 Ring with 1 DC dequeue port
 *  |-----| ------1 TXIN (Pkt+Desc)-----------> |-----------|
 *  |     |    (via UMT TX Msg Cnt to dequeue)  |           |
 *  |     |                                     |           |
 *  |     | <-----1 TXOUT(Pkt Free)-----------  |           |
 *  | Host|                                     | DC Device |
 *  |     | <-----1 RXOUT (PKt+Desc) ---------- | (via ACA) |
 *  |     |    (via UMT RX Msg to recycle )     |           |
 *  |-----| ----------------------------------> |-----------|
 *
 *  3 Ring with Multiple QOS dequeue ports
 *  |-----| -------Multiple TXIN (Pkt+Desc)---> |-----------|
 *  |     |   (Poll Each QOS port Credit_Add )  |           |
 *  |     |                                     |           |
 *  |     | <-----1 TXOUT(Pkt Free)-----------  |           |
 *  | Host|                                     | DC Device |
 *  |     | <-----1 RXOUT (PKt+Desc) ---------- | (via ACA) |
 *  |     |    (via UMT RX Msg to recycle )     |           |
 *  |-----| ----------------------------------> |-----------|
 */

/*! @brief struct dp_buf_type for free ACA/DC buffer */
struct dp_dc_buf {
	u16 f_policy_pool : 1; /*!< flag to indicate policy/pool valid or not */
	u16 policy; /*!< buffer policy, valid if f_policy_pool is set */
	u16 pool; /*!< buffer pool, valid if f_policy_pool is set.
		   *   For PRX300, it is a must to provide pool id or extract
		   *   from DP local table
		   */

	u16 num; /*!< number of buffers in the buffer list */
	void *buf; /*!< buffer list pointer */
};

/*! @brief struct dp_buf_info for free ACA/DC buffer */
struct dp_buf_info {
	int inst; /*!< [in] DP instance ID */
	int dp_port; /*!< [in] DP port ID */
	struct dp_dc_buf rx[DP_RX_RING_NUM]; /* [in] buffers in the rx ring
					      * to free
					      */
	struct dp_dc_buf tx; /* [in] buffers in the rx ring to free */
};

/*! @addtogroup Datapath_Driver_API */
/*! @brief  dp_free_dc_buf is used to free the buffer which is allocated by
 *          DP or from HW BM
 *  @note: DP will internal make assumption:
 *         a) For 3 ring case: tx is BM buffer and rx is linux system buffer
 *         b) For 4 ring case: both are using BM buffer
 *         c) in case caller does not provide policy/pool information, DP will
 *            use default policy to free it
 */
int dp_free_dc_buf(struct dp_buf_info *buf, int flag);

/*! @brief enum DP_DEV_DATA_FLAG */
enum DP_DEV_DATA_FLAG {
	DP_F_DEV_RESV_Q = BIT(0), /*!< Reserve queues for this dev
				   * based on num_resv_q
				   */
	DP_F_DEV_RESV_SCH = BIT(1), /*!< Reserve scheduler for this dev
				     * based on num_resv_sched
				     */
	DP_F_DEV_CONTINUOUS_Q = BIT(2), /*!< Reserve continuous Q for this dev
					 * DP_F_DEV_RESV_Q bit should also be
					 * set for this continuous Q alloc
					 */
	DP_F_DEV_NO_BM_DUMMY_READ = BIT(3), /*!< Disable dummy read */
};

/**
 * @brief dp_gpid_tx_info
 */
/* FIXME: ??? Not sure need or not */
#define DP_DFT_MAX_PKT_LEN 1600 /*!< Default maximal packet length */
/* FIXME: ??? Not sure need or not */
#define DP_DFT_MIN_PKT_LEN 60 /*!< Default minimal packet length */
/*! @addtogroup Datapath_Driver_Structures */
/*! @brief  PPA Sub-interface Data structure
 *@param port_id  Datapath Port Id corresponds to PMAC Port Id
 *@param subif    Sub-interface Id info. In GRX500, this 15 bits,
 *                only 13 bits in PAE are handled [14, 11:0]
 *\note
 */

enum DP_RXOUT_QOS_MODE {
	DP_RXOUT_BYPASS_QOS_ONLY = 0, /*!< bypass QOS but with FSQM */
	DP_RXOUT_QOS, /*!< with QOS */
	DP_RXOUT_BYPASS_QOS_FSQM, /*!< bypass QOS and FSQM */
	DP_RXOUT_QOS_MAX /*!< Not valid RXOUT qos mode */

};

/*! @brief struct dp_rx_ring, which is used for DirectConnected (DC)
 *  applications
 */
struct dp_rx_ring {
	int out_enq_ring_size;  /*!< [in/out]
				 *   rxout enqueue ring size, ie, burst size
				 *   for Peripheral device to enqueue packets to
				 *   Host side via CQM or DMA in burst.
				 *   If 0, then auto set by DP/CQM.
				 *   Note: this parameter will be forced to tune
				 *         down to HW capability
				 */
	u32 out_enq_port_id;   /*!<  [in/out] CQM enqueue port based ID
				* [out] for dev registration which is set by CQM
				* driver.
				* [in] for dev de-registration which is set by
				* DPM.
				*/
	void *out_enq_paddr;   /*!< [out] rxout ring physical address
				*   For GRX350/PRX300, it is DMA Descriptor base
				*      address
				*   For LGM: it is CQM enqueue register address
				*   If NULL, it means not valid
				*/
	void *out_enq_vaddr;   /*!< [out] rxout ring virtual address
				*   For software testing or debugging
				*   For GRX350/PRX300, it is DMA coherent
				*     virtual address of DMA Descriptor base
				*   For LGM, it is IO mapped virtual address
				*     of CQM enqueue register base
				*   If NULL, it means not valid
				*/
	u32 out_dma_ch_to_gswip; /*!< [in/out] DMA TX channel base to GSWIP for
				  *   forwarding rxout packet to GSWIP
				  * [out] for dev registration which is set by
				  * CQM driver.
				  * [in] for dev de-registration which is set by
				  * DPM.
				  */
	u32 num_out_tx_dma_ch; /*!< [out] number of DMA tx channel assigned */
	u32 out_cqm_deq_port_id; /*!< [in/out] CQM dequeue port to GSWIP for
				  *  rxout packet
				  * [out] for dev registration which is set by
				  * CQM driver.
				  * [in] for dev de-registration which is set by
				  * DPM.
				  */
	u32 num_out_cqm_deq_port; /*!< [out] number of CQM dequeue port to GSWIP
				   *   for rxout packet
				   */
	int in_alloc_ring_size; /*!< [out] rxin ring size, ie, maximum number of
				 *   burst alloc buffers size supported.
				 */
	void *in_alloc_paddr; /*!< [out] rxin ring buf allocation physical
			       *     address. It is for 4 ring case only
			       *   Note:
			       *   1. GRX350/PRX300: not support
			       */
	void *in_alloc_vaddr;  /*!< [out] rxin ring buf allocation virtual
				*     address. It is for 4 ring case only
				*   For software testing or debugging
				*   Note:
				*   1. GRX350/PRX300: not support
				*/
	u32 num_pkt; /*!< [in/out] nuber of packet for this policy */
	int rx_pkt_size; /*!< [in/out] rx packet buffer size
			  *   requirement rxout packets.
			  *   DP/CQM will adjust to most
			  *   matched BM pool
			  */
	u16 rx_policy_base; /*!< [out] the rx_policy based on @rx_pkt_size
			     *      requirement.
			     */
	u8 policy_num; /*!< [out] number of policy for this rx_ring */
	u16 rx_poolid; /*!< [out] Only for PRX300, it should be passed for
			*     device to free the packet buffer in rx ring
			*/
	int prefill_pkt_num; /*!< [in] the number of pre-fill packet buffer
			      *       required.
			      * For LGM, normally no need and caller just set it
			      *     to 0.
			      * For GRX350/PRX300, Normally needed to pre-fill
			      *     The buffer size should based on @rx_pkt_size
			      *     requirement
			      */
	void *pkt_base_paddr; /*!< [out] packet list base physical address,
			       *    which stored  @prefill_pkt_num of packet
			       *    physical addressin
			       * For LGM, normally no need and caller just
			       *     set it to 0.
			       * For GRX350/PRX300, Normally needed to pre-fill
			       *     The buffer size should based on
			       *     @rx_pkt_size requirement
			       */
	void *pkt_base_vaddr;  /*!< [out] packet list base virtual address,
				*    which stored @prefill_pkt_num of packet
				*    physical addresses
				*    For software testing or debugging
				*/
	void *pkt_list_vaddr;  /*!< [out] virtual address of 2nd pkt list,
				*    which stored @prefill_pkt_num of packet
				*    virtual addresses
				*    For software testing or debugging
				*/
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
	enum umt_rx_msg_mode out_msg_mode; /*!< [in] rxout msg mode */
#endif
	enum DP_RXOUT_QOS_MODE out_qos_mode; /*!< [in] rxout qos mode */
};

/*! @brief struct dp_tx_ring, which is used for DirectConnected (DC)
 *  applications
 */
struct dp_tx_ring {
	int in_deq_ring_size; /*!< [in/out] ACA TXIN Ring burst size, ie,
			       * size of maximum number of
			       * buffer can be dequeue in one time
			       * For GRX500/PRX300 CQM DC: maximum 32
			       * For 5G with PP QOS port only, ???
			       */
	void *in_deq_paddr; /*!< [out] txin ring/dequeue physical base
			     *   address
			     */
	void *in_deq_vaddr; /*!< [out] txin ring/dequeue virtual base address
			     * For software testing or debugging
			     */
	int out_free_ring_size;/*!< [out] txout ring/free buffer burst size,
				*     the number of buffer can be freed in
				*     in one free operation.
				*/
	void *out_free_paddr; /*!< [out] txout/free buffer
			       *     physica address
			       */
	void *out_free_vaddr; /*!< [out] txout/free buffer virtual base address
			       * For software testing or debugging
			       */
	u32 num_tx_pkt; /*!< [in] nuber of packet */
	int tx_pkt_size; /*!< [in] maximum packet size
			  *   requirement to the packet
			  *   buffer used in tx ring
			  *   For LGM, need provide up to
			  *      @DP_MAX_POLICY_GPID
			  *   For PRX300, only support 1
			  *   For GRX500, only support 1
			  */
	int txout_policy_base; /*!< [out] It should based on @tx_pkt_size and
				*          @dp_gpid_tx_info to get the proper
				*          BM pool.
				*   gpid_info to generate the policy
				*   Not valid for GRX500
				*   In case multiple policy is allocated, only
				*   first policy is really configured properly
				*   and can be used by, all other policy should
				*   be dummy one and cannot allcoate via it now.
				*   later peripheral drivce should call
				*   dp_register_subif_ext to create the
				*   remaining policies
				*   This is also used to configure CQM DQ port
				*   with the egress base policy if
				*   f_txout_auto_free is set to 1
				*/
	int policy_num;  /*!< [out] the number of policy create
			  *   a) For PRX300: it should be 1.
			  *   b) For LGM: it should always 4
			  *   c) For GRX500, not valid
			  */
	u16 tx_poolid; /*!< [out] Only for PRX300, it should be passed for
			*     device to free the packet buffer in tx ring
			*/
	u32 f_out_auto_free : 1; /*!< [in] flag to indicate whether need txout
				  *      base policy to auto free TXIN buffer.
				  * Once f_txout_auto_free is set, this
				  * device can only support maximum up to 4
				  * continuous policy and later not allowed to
				  * create new policy during
				  * dp_register_subif_ext. Otherwise auto free
				  * will cause problem since CQM in LGM only
				  * support 4 policy only.
				  * For those DC device which can keep
				  * policy/pool information, no need
				  * to enable this flag and it can support
				  * more than 4 policies
				  * Note, this setting is not valid for
				  *GRX500/FLM
				  */
	u32 tx_deq_port; /*!< [out] cqm deque port id for this txin ring */
};

/*! @brief struct dp_dev_data, which used for DirectConnected (DC)
 *  applications
 */
struct dp_dev_data {
	enum DP_DEV_DATA_FLAG flag_ops; /*!< flag operation, for subif
					 * registration
					 * refer to enum DP_DEV_DATA_FLAG
					 */
	u8 num_rx_ring;   /*!< [in] number of rx ring from DC device to Host.
			   *   num_rx_ring requirement:
			   *   @num_rings <= @DP_RX_RING_NUM
			   *   GRX350/PRX300:1 rx ring
			   *   LGM: up to 2 rx ring, like Docsis can use 2 rings
			   *   For two ring case:
			   *    1st rxout ring without qos
			   *    2nd rxout ring with qos
			   */
	u8 num_tx_ring;   /*!< [in] number of tx ring from Host to DC device
			   *   num_rx_ring requirement:
			   *   @num_rings <= @DP_TX_RING_NUM
			   *   Normally it is 1 TX ring only.
			   *   But for 5G, it can support up to 8 TX ring
			   *   For docsis, alhtough it is 16 dequeue port to
			   *   WIB.
			   *   But the final ring only 1, ie, WIB to Dcosis
			   */
	u8 num_umt_port;   /*!< [in] number of UMT port.
			    *    Normally is 1 only. But Docsis can use up to 2
			    */
	struct dp_rx_ring rx_ring[DP_RX_RING_NUM]; /*!< [in/out] DC rx ring info
						    */
	struct dp_tx_ring tx_ring[DP_TX_RING_NUM]; /*!< [in/out] DC tx ring info
						    */
	struct dp_gpid_tx_info gpid_info; /*!< [in] for GPID tx information
					   *   Valid only if @f_gpid valid.
					   */
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
	struct dp_umt_port umt[DP_MAX_UMT]; /*!< [in/out] DC umt information */
#endif
	u32 enable_cqm_meta : 1; /*!< enable CQM buffer meta data marking */

	u16 max_ctp;    /*!< [in] maximum subif required which will be mapped to
			 * GSWIP continuous CTP block.
			 * Since very limited CTP in GSWIP and it is already
			 * out of range, some drivers have to specify this
			 * parameter to save the system resource, for example
			 * of G.INIT in falcon_mx:
			 * 1) single LAN port only: it is value should be 16
			 * 2) two Lan Ports:
			 *      a) 1st lan port: 8 CPT with 8 subif only
			 *      b) 2nd lan port: 16 CTP with 16 subif
			 *         Note: first 8 subif/CTP will not be used
			 *         at all for the HW back-pressure design based
			 *         subif ID.
			 * Currently DP just add this parameter, but not really
			 * implemented yet since DP need to change the flow, ie,
			 * move the CTP allocation from dp_alloc_port to
			 * dp_register_dev
			 */
	u16 max_gpid;  /*!< [in] maximum subif required which will be mapped to
			* PP continuous GPID block. The continuous limitation is
			* from GSWIP (subif->GPID mapping design), not because
			* of PP itself.
			* since overall number of GPID < nubmer of CTP in HW,
			* DP need to add this parameter to fully use of shared
			* HW resource.
			*/
	int num_resv_q; /*!< [in] reserve the required number of queues. Valid
			 *   only if DP_F_DATA_RESV_Q bit valid in \ref flag_ops
			 */
	int num_resv_sched; /*!< [in] reserve required number of schedulers.
			     *   Valid only if DP_F_DATA_RESV_SCH bit valid in
			     *   \ref flag_ops
			     */
	u16 qos_resv_q_base; /*!< [out] PPv4 QoS reserved Q base */
	phys_addr_t wib_tx_phy_addr; /*!< [in] DOCSIS WIB ring address */
	u32 n_wib_credit_pkt; /*!< [in] credit size for CQM non-DC egress port
			       * related to WIB. If zero, set as HW capability,
			       * otherwise set as specified for all egress port
			       * linked to WIB
			       */
	u32 bm_policy_res_id; /*!< [in] buffer policy id */
	struct dp_dev_opt_param opt_param; /*!< [in] optional parameters */
};

/**
 * struct dp_reinsert_count - datapath re-insertion counters in TX
 */
struct dp_reinsert_count {
	/* Size 64 Bytes Packet Count. */
	u64 dp_64BytePkts;
	/* Size 65-127 Bytes Packet Count. */
	u64 dp_127BytePkts;
	/* Size 128-255 Bytes Packet Count. */
	u64 dp_255BytePkts;
	/* Size 256-511 Bytes Packet Count. */
	u64 dp_511BytePkts;
	/* Size 512-1023 Bytes Packet Count. */
	u64 dp_1023BytePkts;
	/* Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u64 dp_MaxBytePkts;
	/* DP Oversize Good Packet Count. */
	u64 dp_OversizeGoodPkts;
	/* DP Unicast Packet Count. */
	u64 dp_UnicastPkts;
	/* DP Multicast Packet Count. */
	u64 dp_MulticastPkts;
	/* DP Broadcast Packet Count. */
	u64 dp_BroadcastPkts;
	/* DP Good Packet Count. */
	u64 dp_GoodPkts;
	/* DP Good Bytes Count. */
	u64 dp_GoodBytes;
};

/**
 * struct dp_lct_rx_cnt - datapath lct counters in RX
 */
struct dp_lct_rx_cnt {
	/* Size 64 Bytes Packet Count. */
	u64 dp_64BytePkts;
	/* Size 65-127 Bytes Packet Count. */
	u64 dp_127BytePkts;
	/* Size 128-255 Bytes Packet Count. */
	u64 dp_255BytePkts;
	/* Size 256-511 Bytes Packet Count. */
	u64 dp_511BytePkts;
	/* Size 512-1023 Bytes Packet Count. */
	u64 dp_1023BytePkts;
	/* Size 1024-1522 Bytes (or more, if configured) Packet Count. */
	u64 dp_MaxBytePkts;
	/* DP Unicast Packet Count. */
	u64 dp_UnicastPkts;
	/* DP Multicast Packet Count. */
	u64 dp_MulticastPkts;
	/* DP Broadcast Packet Count. */
	u64 dp_BroadcastPkts;
	/* DP Good Packet Count. */
	u64 dp_GoodPkts;
	/* DP Good Bytes Count. */
	u64 dp_GoodBytes;
};

/*! @brief enum DP_LATE_INIT_TYPE */
enum DP_LATE_INIT_TYPE
{
	DP_LATE_INIT_MIB_COUNTER = 0,
	DP_LATE_INIT_MAX,
};

/*! @addtogroup Datapath_Driver_API */
/*! @brief  Datapath Allocate Datapath Port aka PMAC port
 *	port may map to an exclusive netdevice like in the case of
 *	ethernet LAN ports. In other cases like WLAN, the physical port is a
 *	Radio port, while netdevices are Virtual Access Points (VAPs)
 *	In this case, the  AP netdevice can be passed
 *Alternately, driver_port & driver_id will be used to identify this port
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] dev pointer to Linux netdevice structure (optional), can be NULL
 *@param[in] dev_port Physical Port Number of this device managed by the driver
 *@param[in] port_id Optional port_id number requested. Usually, 0 and
 *	allocated by driver
 *@param[in] pmac_cfg PMAC related configuration parameters
 *@param[in] flags :Various special Port flags like WAVE500, VRX318 etc ...
 *	-  DP_F_DEALLOC_PORT :Deallocate the already allocated port
 *@return  Returns PMAC Port number, -1 on ERROR
 */
int32_t dp_alloc_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, int32_t port_id,
		      dp_pmac_cfg_t *pmac_cfg, uint32_t flags);

/*! @brief  Datapath Allocate Datapath Port aka PMAC port
 *	port may map to an exclusive netdevice like in the case of
 *	ethernet LAN ports. In other cases like WLAN, the physical port is a
 *	Radio port, while netdevices are Virtual Access Points (VAPs)
 *	In this case, the  AP netdevice can be passed
 *Alternately, driver_port & driver_id will be used to identify this port
 *@param[in] inst the DP instance, start from 0. At SOC side, it is always 0.
 *           For pherpheral device, normally it is non-zero.
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] dev pointer to Linux netdevice structure (optional), can be NULL
 *@param[in] dev_port Physical Port Number of this device managed by the driver
 *@param[in] port_id Optional port_id number requested. Usually, 0 and
 *	allocated by driver
 *@param[in] pmac_cfg PMAC related configuration parameters
 *@param[in,out] data to pass the peripheral information
 *@param[in] flags :Various special Port flags like WAVE500, VRX318 etc ...
 *	-  DP_F_DEALLOC_PORT :Deallocate the already allocated port
 *@return  Returns PMAC Port number, -1 on ERROR
 */
int32_t dp_alloc_port_ext(int inst, struct module *owner,
			  struct net_device *dev,
			  u32 dev_port, int32_t port_id,
			  dp_pmac_cfg_t *pmac_cfg,
			  struct dp_port_data *data,
			  uint32_t flags);

/*! @brief  Higher layer Driver Datapath registration API
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] port_id Port Id returned by alloc() function
 *@param[in] dp_cb  Datapath driver callback structure
 *@param[in] flags :Special input flags to alloc routine
 *		- F_DEREGISTER :Deregister the device
 *@return 0 - OK / -1 - Correct Return Value
 *@note
 */
int32_t dp_register_dev(struct module *owner, uint32_t port_id,
			dp_cb_t *dp_cb, uint32_t flags);

/*! @brief  Higher layer Driver Datapath registration API
 *@param[in] inst the DP instance, start from 0. At SOC side, it is always 0.
 *           For pherpheral device, normally it is non-zero.
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] port_id Port Id returned by alloc() function
 *@param[in] dp_cb  Datapath driver callback structure
 *@param[in,out] data to pass the peripheral information
 *@param[in] flags :Special input flags to alloc routine
 *		- F_DEREGISTER :Deregister the device
 *@return 0 - OK / -1 - Correct Return Value
 *@note
 */
int32_t dp_register_dev_ext(int inst, struct module *owner,
			    u32 port_id,
			    dp_cb_t *dp_cb,
			    struct dp_dev_data *data,
			    uint32_t flags);

/*! @brief  Allocates datapath subif number to a sub-interface netdevice
 *Sub-interface value must be passed to the driver
 *port may map to an exclusive netdevice like in the case of ethernet LAN ports
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] dev pointer to Linux netdevice structure, only for VRX318 driver,
 *it can be NULL. All other driver's, must provide valid dev pointer.
 *@param[in] subif_name pointer
 *@param[in,out] subif_id pointer to subif_id structure including port_id
 *@param[in] flags :
 *	DP_F_DEREGISTER - De-register already registered subif/vap
 *@return Port Id  / IFX_FAILURE
 *@note
 */
int32_t dp_register_subif(struct module *owner, struct net_device *dev,
			  char *subif_name, dp_subif_t *subif_id,
			  uint32_t flags);

/*! @brief  Allocates datapath subif number to a sub-interface netdevice
 *Sub-interface value must be passed to the driver
 *port may map to an exclusive netdevice like in the case of ethernet LAN ports
 *@param[in] inst the DP instance, start from 0. At SOC side, it is always 0.
 *           For pherpheral device, normally it is non-zero.
 *@param[in] owner  Kernel module pointer which owns the port
 *@param[in] dev pointer to Linux netdevice structure, only for VRX318 driver,
 *it can be NULL. All other driver's, must provide valid dev pointer.
 *@param[in] subif_name pointer
 *@param[in,out] subif_id pointer to subif_id structure including port_id
 *@param[in,out] data to pass the peripheral information
 *@param[in] flags :
 *	DP_F_DEREGISTER - De-register already registered subif/vap
 *@return Port Id  / IFX_FAILURE
 *@note
 */
int32_t dp_register_subif_ext(
	int inst,
	struct module *owner,
	struct net_device *dev,
	char *subif_name,
	dp_subif_t *subif_id,
	struct dp_subif_data *data,
	uint32_t flags);

/*! @brief  Transmit packet to low-level Datapath driver
 *@param[in] rx_if  Rx If netdevice pointer - optional
 *@param[in] rx_subif  Rx rx_subif netdevice pointer
 *@param[in] skb  pointer to packet buffer like sk_buff
 *@param[in] len    Length of packet to transmit
 *@param[in] flags :Reserved
 *@return 0 if OK  / -1 if error
 *@note
 */
int32_t dp_xmit(struct net_device *rx_if, dp_subif_t *rx_subif,
		struct sk_buff *skb, int32_t len, uint32_t flags);

/*! @brief  Check if network interface like WLAN is a fastpath interface
 *Sub-interface value must be passed to the driver
 *	port may map to an exclusive netdevice like in the case of
 *	ethernet LAN ports.
 *@param[in] netif  pointer to stack network interface structure
 *@param[in,out] subif pointer to subif_id structure including port_id
 *@param[in] ifname  Interface Name
 *@param[in] flags :Reserved
 *@return 1 if WLAN fastpath interface like WAVE500 / 0 otherwise
 *@note  Prototype of PPA_DP_CHECK_IF_NETIF_FASTPATH part of the callback
 *		structure. Such a function needs to be defined by client driver
 *		like the WAVE500 WLAN driver. This API is used by the PPA Stack
 *		AL to check during acceleration learning and configuration
 */

int32_t dp_check_if_netif_fastpath_fn(struct net_device *netif,
				      dp_subif_t *subif, char *ifname,
				      uint32_t flags);

/*! @brief  Get Pkt dst-if Sub-if value
 *	Sub-interface value must be passed to the driver
 *	port may map to an exclusive netdevice like in the case of ethernet
 *	LAN ports.
 *@param[in] netif  pointer to stack network interface structure through
 *		which packet to be Tx
 *@param[in] skb pointer to sk_buff structure that carries packet destination
 *		info
 *@param[in,out] subif_data pointer to subif_data structure including port_id
 *@param[in] dst_mac  Destiantion MAC address to which packet is addressed
 *@param[in,out] subif pointer to subif_id structure including port_id
 *@param[in] flags :Reserved
 *@return 0 if subifid found; -1 otherwise
 *@note  Prototype of PPA_DP_GET_NETIF_SUBIF function. Not implemented in PPA
 *	Datapath, but in client driver like WAVE500 WLAN driver
 *@note  Either skbuff parameters to be used  or dst_mac to determine subifid
 *	For WAVE500 driver, this will be the StationId + VAP on the basis of
 *	the dst mac. This function is only to be used by the PPA to program
 *	acceleration entries. The client driver is still expected to fill
 *	in Sub-interface id when transmitting to the underlying datapath driver
 */
int32_t dp_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
			     void *subif_data, uint8_t dst_mac[DP_MAX_ETH_ALEN],
			     dp_subif_t *subif, uint32_t flags);


/*! @brief  get port information
 *@param[in] inst instance id
 *@param[in] port_id dp port id
 *@param[out] port attribute
 *@return 0 if OK / -1 if error
 *@note: these two API is for dp_dbg and pon mib counter special use.
 */
int dp_get_port_prop(int inst, int port_id, struct dp_port_prop *prop);
int dp_get_subif_prop(int inst, int port_id, int vap, struct dp_subif_prop *prop);

/*! @brief  Check if given interface is a pmapper
 *@param[in] dev  pointer to stack network interface structure
 *@return true if the interface is a pmapper; false otherwise
 */
bool dp_is_pmapper_check(struct net_device *dev);

/*! @brief  The API is for CBM to send received packets(skb) to dp lib. Datapath
 *	lib will do basic packet parsing and forwards it to related drivers,\n
 *	like ethernet driver, wifi and lte and so on. Noted.
 *	It is a chained skb and dp lib will split it before send it to
 *	related drivers
 *@param[in] skb  pointer to packet buffer like sk_buffer
 *@param[in] flags  reserved for futures
 *@return 0 if OK / -1 if error
 */
int32_t dp_rx(struct sk_buff *skb, uint32_t flags);
/*!
 *@brief  The API is for configuing PMAC based on deque port
 *@param[in] port  Egress Port
 *@param[in] pmac_cfg Structure of ingress/egress parameters for setting PMAC
 *	   configuration
 *@return 0 if OK / -1 if error
 */

enum DP_F_STATS_ENUM {
	DP_F_STATS_SUBIF = 1 << 0, /*!< Flag to get network device subif
				    *   mib counter
				    */
	DP_F_STATS_PAE_CPU = 1 << 1 /*!< Flag to get CPU network mib counter*/
};

/*!
 *@brief  The API is for dp_get_netif_stats
 *@param[in] dev pointer to Linux netdevice structure, only for VRX318 driver,
 *@param[in,out] subif_id pointer to subif_id structure including port_id
 *@param[in] path_stats stats for path
 *@param[in] flags  reserved for futures
 *@return 0 if OK / -1 if error
 */
int dp_get_netif_stats(struct net_device *dev, dp_subif_t *subif_id,
		       struct rtnl_link_stats64 *path_stats, uint32_t flags);

/*!
 *@brief  The API is for dp_clear_netif_stats
 *@param[in] dev pointer to Linux netdevice structure, only for VRX318 driver,
 *@param[in,out] subif_id pointer to subif_id structure including port_id
 *@param[in] flag  reserved for futures
 *@return 0 if OK / -1 if error
 */
int dp_clear_netif_stats(struct net_device *dev, dp_subif_t *subif_id,
			 uint32_t flag);

/*!
 *@brief  The API is for dp_get_port_subitf_via_ifname
 *@param[in] ifname  Interface Name
 *@param[in,out] subif pointer to subif_id structure including port_id
 *@return 0 if OK / -1 if error
 */
int dp_get_port_subitf_via_ifname(char *ifname, dp_subif_t *subif);

/*!
 *@brief  The API is for dp_get_port_subitf_via_dev
 *@param[in] dev pointer to Linux netdevice structure, only for VRX318 driver,
 *@param[in,out] subif pointer to subif_id structure including port_id
 *@return 0 if OK / -1 if error
 */
int dp_get_port_subitf_via_dev(struct net_device *dev,
			       dp_subif_t *subif);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)

/*!
 *@brief  The API is for dp_get_port_subitf_via_dev
 *@param[in,out] new_state pointer to structure ltq_cpufreq_threshold,
 *@param[in] flag flag
 *@return 0 if OK / -1 if error
 */
int dp_coc_new_stat_req(int new_state, uint32_t flag);

/*!
 *@brief  The API is for dp_get_port_subitf_via_dev
 *@param[in,out] threshold pointer to structure ltq_cpufreq_threshold,
 *@param[in] flags flags
 *@return 0 if OK / -1 if error
 */
/*! DP's submodule to call it */
/*int dp_set_rmon_threshold(struct dp_coc_threshold *threshold,
 *			    uint32_t flags);
 */
#endif /*! CONFIG_INTEL_DATAPATH_CPUFREQ*/

/*! get port flag. for TMU proc file cat /proc/tmu/queue1 and /proc/tmu/eqt */
u32 get_dp_port_flag(int k);
char *get_dp_port_type_str(int k);
int get_dp_port_type_str_size(void);
u32 *get_port_flag(int inst, int index);

/*! @brief set parser: to enable/disable pmac header
 *@param[in] flag flag
 *@param[in] cpu cpu
 *@param[in] mpe1 mpe1
 *@param[in] mpe2 mpe2
 *@param[in] mpe3 mpe3
 *@return 0 if OK / -1 if error
 */
int dp_set_gsw_parser(u8 flag, u8 cpu, u8 mpe1, u8 mpe2, u8 mpe3);

/*! @brief get parser configuration
 *@param[in] cpu cpu
 *@param[in] mpe1 mpe1
 *@param[in] mpe2 mpe2
 *@param[in] mpe3 mpe3
 *@return 0 if OK / -1 if error
 */
int dp_get_gsw_parser(u8 *cpu, u8 *mpe1, u8 *mpe2, u8 *mpe3);

/*! @brief set pmac configuration
 *@param[in] inst DP instance ID
 *@param[in] port DP port ID
 *@param[in] pmac_cfg point of pmac configuration need to set
 *@return 0 if OK / -1 if error
 */
int dp_pmac_set(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg);

/*! struct dp_umt_cap_mode: dp UMT capability */
struct dp_umt_cap_mode {
	u32 enable:1;  /*!< UMT enable flag: 0-disable 1-eanbled */
	u32 rx_accumulate:1; /*!< rx accumulated supported flag: 1 supported */
	u32 rx_incremental:1; /*!< rx incremental supported flag: 1 supported */
	u32 tx_accumulate:1; /*!< tx accumulated supported flag: 1 supported */
	u32 tx_incremental:1; /*!< tx incremental supported flag: 1 supported */
};

/*! struct dp_umt_cap: dp UMT capability */
struct dp_umt_cap {
	struct dp_umt_cap_mode umt_hw_auto; /*!< HW UMT self mode */
	struct dp_umt_cap_mode umt_hw_user; /*!< HW UMT: user mode */
	struct dp_umt_cap_mode umt_sw; /*!< SW UMT mode */
};

#define DP_MAX_NAME  20 /*!< max name length in character */
#define DP_MAX_GSWIP_LOGICAL_MODE 3 /*!< Max GSWIP Logical Mode */

/*! struct dp_cap: dp capability per instance */
struct dp_cap {
	int inst; /*!< Datapath instance id */

	u32 tx_hw_chksum:1;  /*!< output: HW checksum offloading support flag
			      *   for tx path
			      *   0 - not support
			      *   1: support
			      */
	u32 rx_hw_chksum:1;  /*!< output: HW checksum verification support flag
			      *   for rx path
			      *   0 - not support
			      *   1: support
			      */
	u32 hw_tso: 1; /*!< output: HW TSO offload support for TX path */
	u32 hw_gso: 1; /*!< output: HW GSO offload support for TX path */
	u32 hw_ptp: 1; /*!< HW PTP support */
	u32 max_cpu: 8;			/*!< Maximum number of CPU */
	u32 max_port_per_cpu: 8;	/*!< Maximum number of ports per CPU */
	char qos_eng_name[DP_MAX_NAME]; /*!< QOS engine name in string */
	char pkt_eng_name[DP_MAX_NAME]; /*!< Packet Engine Name String */
	int max_num_spl_conn; /*!< max number of special connection */
	int max_num_queues; /*!< max number of QOS queue supported */
	int max_num_scheds; /*!< max number of QOS scheduler supported */
	int max_num_deq_ports; /*!< max number of CQM dequeue port */
	int max_num_qos_ports; /*!< max number of QOS dequeue port */
	int max_num_dp_ports; /*!< max number of dp port */
	int max_num_subif_per_port; /*!< max number of subif per dp_port */
	int max_num_subif; /*!< max number of subif supported. Maybe no meaning?
			    */
	int max_num_bridge_port; /*!< max number of bridge port */
	int max_num_learn_limit_port;  /*!< max limit of mac addr per port */
	u32 max_eth_port; /*!< Number of MAC Ports */
	u32 max_virt_eth_port; /*!< Number of maximum logical ports over PMAC */
	u32 vap_offset[DP_MAX_GSWIP_LOGICAL_MODE]; /*!< Vap Offset */
	u32 vap_mask[DP_MAX_GSWIP_LOGICAL_MODE]; /*!< Vap Mask */
	struct dp_umt_cap umt; /*!< UMT cap */
};

/*!
 *@brief  The API is for dp_get_cap
 *@param[in,out] cap dp_cap pointer, caller must provide the buffer
 *@param[in] flag for future
 *@return 0 if OK / -1 if error
 */
int dp_get_cap(struct dp_cap *cap, int flag);

/*!
 *@brief  The API is for dp_get_module_owner
 *@param[in] ep dp_port ID
 *@return module owner pointer if success, otherwise NULL
 */
struct module *dp_get_module_owner(int ep);

/*!
 *@brief  Set the minimum frame length on the DP Port
 *@param[in] dp_port dp_port ID
 *@param[in] min_frame_len minimal frame size for this DP port ID
 *@param[in] flag Reserved
 *@return return 0 if OK / -1 if error
 */

int dp_set_min_frame_len(s32 dp_port,
			 s32 min_frame_len,
			 uint32_t flags);

/*!
 *@brief  Enable/Disable forwarding RX packet to specified netif or ifname
 *@param[in] netif netowrk device pointer. if NULL, then check ifname
 *@param[in] ifname if netif == NULL
 *@param[in] rx_enable: 1 enable rx for this device
 *@param[in] flag:
 *            DP_RX_ENABLE: enable rx, ie, allow forwarding rx pkt to this dev
 *            DP_RX_DISABLE: stop rx, ie, DP should drop rx pkt for this dev
 *@return return 0 if OK / -1 if error
 */
#define DP_RX_ENABLE  1
#define DP_RX_DISABLE 0
int dp_rx_enable(struct net_device *netif, char *ifname, uint32_t flags);

/*!
 *@brief Datapath Manager Pmapper Configuration Set
 *@param[in] dev: network device point to set pmapper
 *@param[in] mapper: buffer to get pmapper configuration
 *@param[in] flag: reserve for future
 *@return Returns 0 on succeed and -1 on failure
 *@note  for pcp mapper case, all 8 mapping must be configured properly
 *       for dscp mapper case, all 64 mapping must be configured properly
 *       def ctp will match non-vlan and non-ip case
 *	For drop case, assign CTP value == DP_PMAPPER_DISCARD_CTP
 */
int dp_set_pmapper(struct net_device *dev, struct dp_pmapper *mapper, u32 flag);

/*!
 *@brief Datapath Manager Pmapper Configuration Get
 *@param[in] dev: network device point to set pmapper
 *@param[out] mapper: buffer to get pmapper configuration
 *@param[in] flag: reserve for future
 *@return Returns 0 on succeed and -1 on failure
 *@note  for pcp mapper case, all 8 mapping must be configured properly
 *       for dscp mapper case, all 64 mapping must be configured properly
 *       def ctp will match non-vlan and non-ip case
 *	 For drop case, assign CTP value == DP_PMAPPER_DISCARD_CTP
 */
int dp_get_pmapper(struct net_device *dev, struct dp_pmapper *mapper, u32 flag);

/*!
 *@brief Datapath Manager DMA RX IRQ enable/disable API
 *@param[in] inst: DP instance ID
 *@param[in] ch: DMA RX Channel information
 *@param[in] flag: reserve for future
 *@return Returns DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_dma_chan_irq_rx_enable(int inst, struct dp_dma_ch *ch, int flag);

/*!
 *@brief Datapath Swdev API to get FID from device
 *@param[in] dev: network device pointer
 *@param[out] inst: DP instance ID
 *@return Returns GSWIP bridge id i.e. FID
 */
int dp_get_fid_by_dev(struct net_device *dev, int *inst);
/*!
 *@note dp_get_fid_by_brname will be remove after all other drivers change
 *      to dp_get_fid_by_dev
 */
#define dp_get_fid_by_brname(dev, inst) dp_get_fid_by_dev(dev, inst)

enum DP_EGP_TYPE {
	DP_EGP_TO_DEV = 0, /*!< EGP port type:  to device */
	DP_EGP_TO_GSWIP    /*!< intermediate Port to GSWIP */
};

/*! @brief struct dp_spl_egp, which is used for sepcial CPU path
 *  applications: CQM egress path to special application
 */
struct dp_spl_egp {
	enum DP_EGP_TYPE type; /*!< [out] EGP port type: DP_EGP_TO_DEV
				*         DP_EGP_TO_GSWIP
				*/
	int egp_id; /*!< EGP port ID */
	int pp_ring_size; /*!< [in/out] ACA TXIN Ring burst size, ie,
			   *         size of maximum number of
			   *         buffer can be dequeue in one time
			   *         LGM
			   *           Voice: 4 if input is 0
			   *           TOE(LRO/TSO): 2 if input is 0
			   *           VPN-A: 1-32. 32 if input is 0
			   *           App Litepath: NA
			   *           PP NF: 32 if input is 0 ?
			   */
	void *egp_paddr; /*!< [out] egp physical base address
			  *         LGM
			  *           Voice: [out] valid
			  *           TOE(LRO/TSO): [out] valid for LRO only
			  *           VPN-A: [out] valid
			  *           App Litepath: NA
			  *           PP NF: [out] NULL since PP NF use PP Port
			  */
	u32 tx_pkt_credit;  /*port tx packet credit */
	void *tx_push_paddr; /*!< [in/out] QOS TX push physical address
			      *         LGM
			      *           Voice: [out] valid
			      *           TOE(LRO/TSO): [out] valid for LRO only
			      *           VPN-A: [out] valid
			      *           App Litepath: NA
			      *           PP NF: [in] points to PP SRAM
			      */
	void *tx_push_paddr_qos; /*!< [in/out] QOS TX push physical address for
				  *   QOS Port configuration with masking
				  *   LGM
				  *     Voice: [out] valid
				  *     TOE(LRO/TSO): [out] valid for LRO only
				  *     VPN-A: [out] valid
				  *     App Litepath: NA
				  *     PP NF: [in] points to PP SRAM
				  */
	void *free_paddr; /*!< [out] register physical address to free
			   *  BM buffer
			   *    LGM
			   *      Voice: [out] valid
			   *      TOE(LRO/TSO): NA
			   *      VPN-A: NA
			   *      App Litepath: NA
			   *      PP NF: NA
			   *  Note: Not valid for dequeue port for IGP
			   */
	void *free_vaddr; /*!< For voice */
	int qid;  /*!< [out] physical qid to the EGP
		   *         LGM
		   *           Voice: valid
		   *           TOE(LRO/TSO): valid for LRO only
		   *           VPN-A: valid
		   *           App Litepath: NA
		   *           PP NF: valid
		   */
};

/*! @brief struct dp_spl_igp, which is used for sepcial CPU path
 *  applications: special applicaton to CQM ingress path, then to GSWIP and PP
 */
struct dp_spl_igp {
	u32 igp_id; /*!< [out] CQM enqueue port based ID
		     *         LGM
		     *           Voice: valid
		     *           TOE(LRO/TSO): valid
		     *           VPNA FW: valid
		     *           App Litepath: NA
		     *           PP NF: NA
		     */
	int igp_ring_size;  /*!< [in/out]
			     *   If 0, then auto set by DP/CQM.
			     *   Note: this parameter will be forced to tune
			     *         down to HW capability
			     *    LGM
			     *       Voice: 1
			     *       TOE(LRO/TSO): 1 for LRO_ACK and TSO
			     *       VPNA FW: 1-32. Driver need to decide for it
			     *       App Litepath: NA
			     *       PP NF: NA
			     */
	void *igp_paddr;   /*!< [out] CQM enqueue register address
			    *         LGM
			    *           Voice: valid
			    *           TOE(LRO/TSO): valid
			    *           VPNA FW: valid
			    *           App Litepath: NA
			    *           PP NF: NA
			    */
	void *alloc_paddr; /*!< [out] register physical address to allococate
			    *   BM buffer
			    *   LGM
			    *      Voice: [out] valid
			    *      TOE(LRO/TSO): NA
			    *      VPN-A: NA
			    *      App Litepath: NA
			    *      PP NF: NA
			    */
	void *alloc_vaddr; /*!< [out] For voice_testing */

	u32 igp_dma_ch_to_gswip; /*!< [out] DMA TX channel base to GSWIP for
				  *         forwarding rxout packet to GSWIP
				  *    LGM
				  *       Voice: valid
				  *       TOE(LRO/TSO): valid for LRO_ACK/TSO
				  *       VPNA FW: valid
				  *       App Litepath: NA
				  *       PP NF: valid with one shared common
				  *              path for all NFs
				  */
	u32 num_out_tx_dma_ch; /*!< [out] number of DMA tx channel assigned
				*    LGM
				*       Voice: 1
				*       TOE(LRO/TSO): 1 respectilvey per IGP
				*       VPNA FW: 0
				*       App Litepath: 0
				*       PP NF: 1(shared common path for all NFs)
				*/
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
	enum umt_rx_msg_mode out_msg_mode; /*!< [in] rxout msg mode
					    *    LGM
					    *       Voice: NA
					    *       TOE(LRO/TSO): NA
					    *       VPNA FW: valid
					    *       App Litepath: NA
					    *       PP NF: NA
					    */
#endif
	enum DP_RXOUT_QOS_MODE out_qos_mode; /*!< [in] rxout qos mode
					      *    LGM
					      *       Voice: NA
					      *       TOE(LRO/TSO): NA
					      *       VPNA FW: valid
					      *       App Litepath: NA
					      *       PP NF: NA
					      */
	u32 num_out_cqm_deq_port; /*!< [out] number of CQM dequeue port to GSWIP
				   *   for rxout packet: 1 or 0
				   *    LGM
				   *       Voice: 1
				   *       TOE(LRO/TSO): 1
				   *       VPNA FW: 1
				   *       App Litepath: 0
				   *       PP NF: 1(shared common path for
				   *                all NFs)
				   */
	struct dp_spl_egp *egp; /*!< [out] CQM dequeue port to GSWIP for
				 *   rxout/igp packet
				 */
};

/*! @brief struct dp_spl_policy, which is used for sepcial CPU path
 *  applications: policy related information
 */
struct dp_spl_policy {
	u32 num_pkt; /*!< [in] nuber of packet for this policy */
	u32 rx_pkt_size; /*!< [in/out] rx packet buffer size
			  *   requirement rxout packets.
			  *   DP/CQM will adjust to most
			  *   matched BM pool
			  */
	u16 tx_policy_base; /*!< [out] tx policy ID base */
	u8 tx_policy_num;   /*!< [out] number of continuous tx policy allowed */
	u16 rx_policy_base; /*!< [out] rx policy ID base */
	u8 rx_policy_num;   /*!< [out] number of continuous rx policy allowed */
	u16 pool_id[DP_MAX_POOL_SUBIF]; /* [out] pool id allocated. valid only
					 *   if num_policy is non-zero
					 */
	u16 policy_map;	/* [out] buffer size bit map for relevant policy
			 *      eg 0x4 is for size-2 buffer
			 *      eg 0x0 is for size-0 buffer
			 */
};

/*! @brief struct dp_spl_cfg, which is used for register/register special
 *  CPU path applications
 */
struct dp_spl_cfg {
	u32 flag; /*!< [in]DE_REGISTER for de-registration, otherwise for
		   *       registration
		   */
	enum DP_SPL_TYPE type; /*!< [in] Special Connectity Type */
	u32 spl_id; /*!< [out] for dp_spl_conn_get() only
		     *   [out/in] For registration, it is [out] parameter
		     *            For de-registration, it is [in] parameter
		     *            Note: for LGM, DP can support up to 8 special
		     *                  CPU connection path at present.
		     *            From CQM point of view, may from 16 better.
		     */
	u32 f_subif : 1; /*!< [out] for dp_spl_conn_get() only
			  *   [in] need allocate subif flag
			  *        LGM:
			  *         Even if caller disable it. DP internally
			  *         still assign one subif for it.
			  *         caller just skip its output parameters
			  *         For Voice: 1
			  *         For TOE(TSO/LRO): 0
			  *         For VPN-A Driver: 1
			  *         For APP Litepath: 1
			  *         For PP NF: Fragmenter: 1
			  */
	u32 f_gpid : 1; /*!< [out] for dp_spl_conn_get() only
			 *   [in] need allocate GPID flag
			 *        Even if caller disable it. DP internally will
			 *        assign one GPID for it. But DP will not
			 *        call PP API to really configure it in this
			 *        case. Valid only if @f_subif valid
			 *       LGM:
			 *         For Voice: 1
			 *         For TOE(TSO/LRO): 0
			 *         For VPN-A Driver: 1
			 *         For APP Litepath: 1
			 *         For PP NF: 1
			 */
	u32 f_policy : 1; /*!< [out] for dp_spl_conn_get() only
			   *   [in] need allocate policy or not
			   *   LGM:
			   *     voice: 1 Need allocate one special policy,
			   *     TOE(LRO/TSO): 0 CQM driver configure it to
			   *                  system policy for TSO/LRO_ACK
			   *     VPN-A Driver: 0, ie, reuses input buffer
			   *     App Litepath: 0, ie, reuses the CPU DQ
			   *                   ports for the host
			   *     PP NF: Fragmenter NF: 0
			   */
	u32 f_hostif : 1; /*!< [out] for dp_spl_conn_get() only
			   *   [in] need create PP hostif flag
			   *   LGM:
			   *     voice: 1
			   *     TOE(LRO/TSO): 0
			   *     VPN-A Driver: 1
			   *     App Litepath: 1
			   *     PP NF: Mutlicast : 1,
			   *            Others: 0
			   */
	u32 prel2_len:2; /*!< [out] for dp_spl_conn_get() only
			   *   [in] size of PMAC header in multiple of 16 bytes
			   *   LGM:
			   *     0: Disabled
			   *     1: 16 bytes
			   *     2: 32 bytes
			   *     3: 48 bytes
			   */
	int subif; /*!< [out] subif id. Valid only if f_subif is set */
	int gpid; /*!< [out] gpid. Valid only if f_gpid is set */
	int spl_gpid; /*!< [out] gpid for voice. Valid only if f_gpid is set */
	dp_cb_t *dp_cb; /*!< [out] for dp_spl_conn_get() only
			 *   [in] for subif level callback.
			 *        Not for GRX500/PRX300
			 *        LGM:
			 *         *)For rx_fn of dp_cb
			 *            voice: yes for rx_fn
			 *            TOE(LRO/TSO): no
			 *            VPN-A Driver: yes
			 *            App Litepath: yes
			 *            PP NF: no
			 *         *)For tx_fn of dp_cb
			 *            voice: no
			 *            TOE(LRO/TSO): yes for TSO
			 *            VPN-A Driver: no
			 *            App Litepath: no
			 *            PP NF: Multicast NF needs rx_cb
			 */
	struct net_device *dev; /*!< [out] for dp_spl_conn_get() only
				 *   [in] network device if need.
				 *   if rx_fn of dp_cb valid, dev should be
				 *      valid also
				 *   Not for GRX500/PRX300
				 *   LGM:
				 *     voice: yes
				 *     TOE(LRO/TSO)- Not required (No Dequeue )
				 *     VPN-A Driver: yes
				 *     App Litepath: yes
				 *     Fragmenter NF: Not required
				 */
	int dp_port; /*!< [out] dp_port ID, normally it is CPU 0.
		      *   if -1, then not applicable for this special connect
		      */
	u8 num_igp;   /*!< [out] number of IGP (ingress port) from device to CQM
		       *   num_igp requirement: @num_igp <= @DP_SPLPATH_IGP_NUM
		       *   LGM:
		       *     Voice: 1(CPU IGP without QOS) igp_info[0]
		       *     TOE(LRO/TSO): 2(DMA IGP)
		       *                     igp_info[0]-TSO,
		       *                     igp_info[1]-LRO_ACK
		       *     VPN-A Driver: 2(DC IGP)
		       *                     igp_info[0]-with QOS,
		       *                     igp_info[1]-without QOS,
		       *     App Litepath: 0
		       *     PP NF: 0
		       */
	u8 num_egp; /*!< [out] number of EGP from CQM/Host to device
		     *   num_egp requirement:@num_egps <= @DP_TX_RING_NUM
		     *   LGM:
		     *     Voice: 1(CPU) + 1 GSWIP EGP
		     *     TOE(LRO/TSO): 1(CPU) for LRO + 2 GSWIP EGP
		     *     VPN-A Driver: 1(DC)
		     *     App Litepath: 0 + 0
		     *     PP NF: 1 per NF + 1 GSWIP.
		     *            note, it is PP QOS port based only
		     */
	u8 num_umt_port;   /*!< [out] number of UMT port.
			    *        LGM
			    *           Voice: 0
			    *           TOE(LRO/TSO): 0
			    *           VPN-A Driver: 1
			    *           App Litepath: 0
			    *           PP NF: 0
			    */
	struct dp_spl_igp igp[DP_SPLPATH_IGP_NUM]; /*!< [out] for
						    *   dp_spl_conn_get() only
						    *   [in/out] IGP
						    *   ingress Port information
						    *   refer to the comment of
						    *   @num_igp
						    */
	struct dp_spl_egp egp[DP_SPLPATH_EGP_NUM + DP_SPLPATH_IGP_NUM];
						    /*!< [out]
						     * for dp_spl_conn_get only
						     * [in/out] EGP egress port
						     *   information
						     *   refer to the comment of
						     *   @num_egp
						     */
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
	struct dp_umt_port umt[1];  /*!< [out] for dp_spl_conn_get() only
				     *        [in/out] umt information
				     *        LGM
				     *           Voice: Not applicable
				     *           TOE(LRO/TSO): Not applicable
				     *           VPN-A Driver:Yes
				     *           App Litepath: NA
				     *           PP NF: NA
				     */
#endif
	struct dp_spl_policy policy[1];  /*!< [out] for dp_spl_conn_get() only
					  *   [in/out] Buffer policy information
					  *        LGM
					  *           Voice: Yes and need
					  *              allocate policy for it
					  *           TOE(LRO/TSO): fixed to
					  *              system policy.
					  *               For TSO: policy base 0
					  *               For LRO: no need
					  *           VPN-A Driver:NA
					  *           App Litepath: NA
					  *           PP NF: NA
					  */
};

/*!
 *@brief Datapath Manager Initialize Speical Connectivity API
 *@param[in] inst: DP instance ID
 *@param[in] conn: Special Connect information
 *@param[in] flag: for deregistration, it needs to use flag DP_F_DEREGISTER
 *                 otherwise it is for registration a new CPU special path
 *@return Returns DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_spl_conn(int inst, struct dp_spl_cfg *conn);

#define DP_SPL_CONN_MAX_CNT 4 /*!< maximum number of struct dp_spl_cfg */

/*!
 *@brief Datapath Manager get Speical Connectivity API
 *@param[in] inst: DP instance ID
 *@param[in] type: DP special connectivity type
 *@param[in/out] conns: user allocated buffer to store connectivity info
 *@param[in] cnt: number of dp_spl_cfg been allocated
 *@return returns number of results been wrote, 0 means not found
 */
int dp_spl_conn_get(int inst, enum DP_SPL_TYPE type,
		    struct dp_spl_cfg *conns, u8 cnt);

/*! @addtogroup Datapath_Driver_Structures */
/*! @brief  DP IO  information
 *@param port_id  Datapath Port Id corresponds to PMAC Port Id
 *@param subif    Sub-interface Id info. In GRX500, this 15 bits,
 *		only 13 bits in PAE are handled [14, 11:0]
 *\note
 */
#define DP_MAX_CORE  4 /*!< maximum CORE DP supported */
struct dp_dpdk_io_per_core {
	u8 core_id; /*!< [out] core ID: from 0 ~ (DP_MAX_CORE -1)  */
	int num_subif; /*!< [out] number of subif allocated for this IO type */
	int subif[2]; /*!< [out] subif ID for this core.
		       *   Two subifs are needed because DPDK needs 2 CQM
		       *   EQ ports (QOS bypass and QOS).
		       *   For the CQM DQ side, they currently require 1 CQM
		       *   DQ port - but this may change later.
		       */
	u16 gpid[2]; /*!< [out] gpid ID for this core */

	int num_egp; /*!< [out] number of egress port. Normally 1 */
	int egp_id[2]; /*!< [out] CQM EGP(Egress) port ID */

	int num_igp; /*!< [out] number of CQM ingress port. Normally 2 */
	int igp_id[2]; /*!< [out] IGP port ID.
			*   Note: DPDK will get igp/allo_buffer/return buffer
			*         register address based on igp_id as spec
			*         defined
			*/
	u8  f_igp_qos[2]; /*!< [out] flag to indicate whether this igp with or
			   *   without QOS
			   *   0 -- without QOS
			   *   1  -- with QOS
			   */
	int num_policy; /*!< [out] number of policy. Normally 1 */
	u16 policy_base; /*!< [out] the base policy assigned to DPDK */

	/* need further align for DPDK optimized path */
	int num_tx_push; /*!< [out] number of tx push used DPDK */
	void *credit_add_paddr[2]; /*!< [out] credit add physical address */
	void *credit_left_paddr[2]; /*!< [out] credit left physical address */
};

struct dp_dpdk_io {
	u8 num; /*!< [out] number of DPDK core */
	int dpid; /*!< [out] DPID, it is always zero in LGM */
	struct dp_dpdk_io_per_core info[DP_MAX_CORE]; /*!< [out] information per
						       *    core
						       */
};

/*! @brief struct dp_io_port_info, which is used for dp_get_io_port_info
 */
struct dp_io_port_info {
	int inst;  /*!< [in] dp instance id. For Host side, it is alwasy 0 */
	enum DP_IO_PORT_TYPE type; /*!< [in] io port type */
	union {
		struct dp_dpdk_io info; /*!< [out] dpdk io information */
		char dummy[0]; /*!< [out] reserve for future */
	} info; /*!< [out] union variable */
};

/*!
 *@brief dp_get_io_port_info API to retrieve system IO port information
 *@param[in/out] info: io port information
 *       1)DPDK: it needs set info->type to DP_IO_PORT_TYPE,
 *               in this case, DP will return DPDK IO port information per core
 *@param[in] flag: reserved for future
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 *@note: CQM driver no need to export new API, instead just enhance existing
 *       API cbm_cpu_port_get to add new varabile enum DP_IO_PORT_TYPE type
 */
int dp_get_io_port_info(struct dp_io_port_info *info, uint32_t flag);

enum DP_DATA_PORT_TYPE {
	DP_DATA_PORT_LINUX = 0, /*!< data port owned by Linux */
	DP_DATA_PORT_DPDK, /*!< data port owned by DPDK */
	DP_DATA_PORT_MPE, /*!< data port owned by MPE */
};

/*! @brief struct dp_dpdk_port_conf, which is used for DPDK to call
 *  dp_set_port/dp_get_port
 */
struct dp_dpdk_port_conf {
	int dpid;  /*!< [in] dp port ID */
	enum DP_DATA_PORT_TYPE type; /*!< [in/out] io port type:
				      * For dp_set_io_port, it is [in] parameter
				      *   if type == DP_DATA_PORT_LINUX
				      *      data port owner should set to Linux
				      *   if type == DP_DATA_PORT_DPDK
				      *      data port owner should set to DPDK
				      * For dp_get_io_port, it is [out]
				      *      parameter
				      */
};

/*! @brief struct dp_port_conf, which is used for dp_set_port/dp_get_port
 */
struct dp_port_conf {
	int inst;  /*!< [in] dp instance id. For Host side, it is alwasy 0 */
	enum DP_IO_PORT_TYPE type; /*!< [in] io port type */
	union {
		struct dp_dpdk_port_conf info; /*!< [out] dpdk io information */
		char dummy[0]; /*!< [out] reserve for future */
	} info; /*!< [out] union variable */
};

/*!
 *@brief dp_set_io_port API
 *@param[in/out] conf: port information
 *@param[in] flag: reserved for future
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 *@note:  1) For DPDK: it needs set info->type to DP_IO_PORT_TYPE,
 *                     a) This API is used to set data port owner: linux or DPDK
 *                     b) conf->dpid cannot be zero. CPU port type is fixed
 *                        and cannot be changed during runtime.
 *                     c) All subifs under same dpdk will be owned either DPDK
 *                        or linux, cannot partially by Linux, partially by DPDK
 */
int dp_set_datapath_io_port(struct dp_port_conf *conf, uint32_t flag);

/*!
 *@brief dp_get_io_port API
 *@param[in/out] conf: port information
 *@param[in] flag: reserved for future
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 *@note:  1) For DPDK: it needs set info->type to DP_IO_PORT_TYPE,
 *                     a) This API is used to get data port owner: linux or DPDK
 */
int dp_get_datapath_io_port(struct dp_port_conf *conf, uint32_t flag);

enum DP_EVENT_TYPE_BITS {
	DP_EVENT_INIT_BIT,
	DP_EVENT_ALLOC_PORT_BIT,
	DP_EVENT_DE_ALLOC_PORT_BIT,
	DP_EVENT_REGISTER_DEV_BIT,
	DP_EVENT_DE_REGISTER_DEV_BIT,
	DP_EVENT_REGISTER_SUBIF_BIT,
	DP_EVENT_DE_REGISTER_SUBIF_BIT,
	DP_EVENT_OWNER_SWITCH_BIT,
	DP_EVENT_MAX_BIT
};

#define EVENT(TYPE) BIT(DP_EVENT_##TYPE##_BIT)

/*! @brief enum DP_EVENT_TYPE to define the different kind of event type
 */
enum DP_EVENT_TYPE {
	DP_EVENT_INIT = EVENT(INIT), /*!< event callback for initiazation
					*   when first network driver register to dp via
					*   calling API dp_alloc_port_ext
					*   DP will call event callback before
					*   allocate port for this request
					*/
	DP_EVENT_ALLOC_PORT = EVENT(ALLOC_PORT), /*!< event callback for
					*   alloc port
					*/
	DP_EVENT_DE_ALLOC_PORT = EVENT(DE_ALLOC_PORT), /*!< event callback
					*   for de_alloc port
					*/
	DP_EVENT_REGISTER_DEV = EVENT(REGISTER_DEV), /*!< event callback for
					*   register dev
					*/
	DP_EVENT_DE_REGISTER_DEV = EVENT(DE_REGISTER_DEV), /*!< event callback
					*   for de_register dev
					*/
	DP_EVENT_REGISTER_SUBIF = EVENT(REGISTER_SUBIF), /*!< event callback
					*   for register subif
					*/
	DP_EVENT_DE_REGISTER_SUBIF = EVENT(DE_REGISTER_SUBIF), /*!< event
					*   callback for de_register subif
					*/
	DP_EVENT_OWNER_SWITCH = EVENT(OWNER_SWITCH),  /*!< event callback
					*   when data port owner switching between linux and
					*   DPDK
					*/
	DP_EVENT_MAX = EVENT(MAX)
};

/*! @brief struct dp_event_init_info, init event specific information
 */
struct dp_event_init_info {
	struct module *owner; /*!< Kernel module pointer which owns the port */
	struct net_device *dev; /*!< network device pointer. it can be NULL for
				 *   some device
				 */
	u32 dev_port; /*!< Physical Port Number of this device managed by the
		       *   driver
		       */
};

/*! @brief struct dp_event_alloc_info, alloc port event specific information
 */
struct dp_event_alloc_info {
	struct module *owner; /*!< Kernel module pointer which owns the port */
	struct net_device *dev; /*!< network device pointer. it can be NULL for
				 *   some device
				 */
	u32 dev_port; /*!< Physical Port Number of this device managed by the
		       *   driver
		       */
};

/*! @brief struct dp_event_reg_dev_info, register dev event specific
 *  information
 */
struct dp_event_reg_dev_info {
	struct module *owner; /*!< Kernel module pointer which owns the port */
	struct net_device *dev; /*!< network device pointer. it can be NULL for
				 *   some device
				 */
	int dpid;  /*!< dp port ID */
};

/*! @brief struct dp_event_reg_subif_info, register subif event specific
 *   information
 */
struct dp_event_reg_subif_info {
	int dpid;  /*!< dp port ID */
	s32 subif; /*!< device subif */
	struct net_device *dev; /*!< subif network device pointer.
				 *   It can be NULL for some case, like ATM DSL
				 */
};

/*! @brief struct dp_event_owner_info, data port owner change event specific
 *   information
 */
struct dp_event_dataport_owner_info {
	int dpid;  /*!< dp port ID */
	enum DP_DATA_PORT_TYPE new_owner; /*!<  owner dpid will switch to */
};

/*! @brief struct dp_dev_event, which is used for dp_register_event
 */
struct dp_event_info {
	int inst; /*!< [out] dp instance id */
	enum DP_EVENT_TYPE type; /*!< [out] event type */
	union {
		struct dp_event_init_info init_info; /*!< initialization info */
		struct dp_event_alloc_info alloc_info; /*!< alloc port info */
		struct dp_event_alloc_info de_alloc_info; /*!< de_alloc port
							   * info
							   */
		struct dp_event_reg_dev_info reg_dev_info; /*!< register
							    *   dev info
							    */
		struct dp_event_reg_dev_info dereg_dev_info; /*!< de_register
							      *   dev info
							      */
		struct dp_event_reg_subif_info reg_subif_info;/*!< register
							       *   subif
							       *   info
							       */
		struct dp_event_reg_subif_info de_reg_subif_info;/*!< register
								  *   subif
								  *   info
								  */
		struct dp_event_dataport_owner_info owner_info;/*!< data port
								*   owner info
								*/
	};
	void *data; /*!< [out] pass back the data to caller which was provied by
		     *   caller when calling dp_register_event_cb API
		     */

};

enum DP_EVENT_OWNER {
	DP_EVENT_OWNER_OTHERS = 0, /*!< Event owner type OTHERS */
	DP_EVENT_OWNER_DPDK, /*!< Event owner type DPDK */
	DP_EVENT_OWNER_PPA, /*!< Event owner type PPA */
	DP_EVENT_OWNER_MIB, /*!< Event owner type MIB */
	DP_EVENT_OWNER_MAX /*!< Maximum owners types */
};

/* Note:this callback should should support tasklet/softtimer related context */
struct dp_event {
	int inst; /*!< [in] dp instance id */
	enum DP_EVENT_OWNER owner; /*!< [in] owner who register to DP
				    *   for event status monitoring.
				    *   For debugging purpose only
				    */
	enum DP_EVENT_TYPE type; /*!< [in] type of event */
	int f_owner_only; /*!< [in] flag to indicate whether only limited event
			   *   will be trigger. It is mainly for DPDK if
			   *   DPDK only interted in those data port which owned
			   *   by DPDK only
			   */
	int32_t(*dp_event_cb)(struct dp_event_info *info);  /*!< [in] event
							     *   callback
							     *   function
							     *   callback contex
							     *   can be tasklet
							     *   or soft-timer
							     */
	void *data; /*!< [in] optinal data which will be passed backed to
		     *   callback function when event triggered
		     */
	void *id; /*!< [in/out] handle for dp_register_event_cb.
		   *   When set as NULL, it will be treated as [out] parameter.
		   *   Upon sucessful registration of the callback function, a
		   *   valid handle will be return.
		   *   The handle can be used for subsequent update by
		   *   registered apps. It will be used as [in] parameter.
		   *   All existing values will be overwritten by the new
		   *   values in dp_event_info.
		   */
};

/*!
 *@brief dp_register_event_cb API to register event callback functions
 *@param[in/out] info: registration event info
 *@param[in] flag: DP_F_DEREGISTER is for de-registration,
 *                 otherwise it is for registration
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_register_event_cb(struct dp_event *info, uint32_t flag);

/*!
 *@brief dp_set_ethtool_stats_fn API to register callback function which
 * retrieves counters for ethtool statistics
 *@param[in] inst: dp instance id
 *@param[in] cb: pointer to function called to retrieve counters for
 *		 ethtool statistics
 */
void dp_set_ethtool_stats_fn(int inst,
			     void (*cb)(struct net_device *dev,
					struct ethtool_stats *stats,
					u64 *data));

/*!
 *@brief dp_set_ethtool_stats_strings_cnt_fn API to register callback function which
 * retrieves retrieve number of counters
 *@param[in] inst: dp instance id
 *@param[in] cb: pointer to function called to retrieve number of counters for
 *		 ethtool statistics
 */
void dp_set_ethtool_stats_strings_cnt_fn(int inst,
					 int (*cb)(struct net_device *dev));

/*!
 *@brief dp_set_ethtool_stats_strings_fn API to register callback function which
 * retrieves set of counters' names
 *@param[in] inst: dp instance id
 *@param[in] cb: pointer to function called to retrieve set of counters' names
 *		 for ethtool statistics
 */
void dp_set_ethtool_stats_strings_fn(int inst,
				     void (*cb)(struct net_device *dev, u8 *data));

/*! @brief Enumerator DP_OPS_TYPE */
enum DP_OPS_TYPE {
	DP_OPS_BM, /*!< BM ops type */
	DP_OPS_QOS, /*!< QOS ops type */
	DP_OPS_GSW, /*!< GSWIP ops type */
	DP_OPS_UMT, /*!< UMT ops type */
	DP_OPS_LRO, /*!< LRO ops type */
	DP_OPS_VPN, /*!< VPN ops type */
	DP_OPS_CQM, /*!< CQM ops type */
	DP_OPS_CNT,  /*!< total ops type count */
};

enum DP_SUBIF_UPD {
	/*!< Once this flag is set, qmap will not be changed */
	DP_F_UPDATE_NO_Q_MAP = BIT(0),
};

/*! @brief struct dp_subif_upd_info, which is used for dp_update_subif
 */
struct dp_subif_upd_info {
	int inst; /*!< [in] dp instance id */
	int dp_port; /*!< [in] Datapath Port Id corresponds to PMAC Port Id */
	int subif; /*!< [in] Sub-interface Id as HW defined */
	struct net_device *new_dev; /*!< [in] master dev */
	struct net_device *new_ctp_dev; /*!< [in] slave dev */
	int new_cqm_deq_idx; /*!< [in] New deq port index */
	int new_num_cqm_deq; /*!< [in] number of CQM dequeue port used
			      * for this subif
			      * for GPON, always 1, for EPON, 1-8
			      */
	u32 flags; /*!< [in] caller provided during dp_update_subif_info */
	u8 new_domain_flag : 1; /*!< [in] if set, then use domain
				 * information, otherwise keep original
				 * domain information
				 */
	u8 new_domain_id : 5; /*!< [in] we support 32 domain per bridge only.
			       * valid only DP_SUBIF_BR_DOMAIN is set in flag_ops
			       */
	u32 domain_members; /*!< [in] one bit for one domain_id:
			     * bit 0 for domain_id 0, bit 1 for domain 1,so on.
			     * If one bit is set to 1, the traffic received
			     * from this dev can be forwarded to this domain
			     * as specified in domain_members
			     * valid only if DP_SUBIF_BR_DOMAIN is set in flag_ops
			     */
};

/*!
 *@brief Datapath Manager subif registration update
 *@param[in] info: subif_update_information
 *@return DP_SUCCESS on success and DP_FAILURE on failure
 */
int dp_update_subif_info(struct dp_subif_upd_info *info);

/*!
 *@brief Datapath Manager ops registration
 *@param[in] inst: DP instance ID
 *@param[in] type: ops type
 *@param[in] ops: pointer to ops structure
 *@note  set to NULL to deregister
 *@return return 0 if OK / -1 if error
 */
int dp_register_ops(int inst, enum DP_OPS_TYPE type, void *ops);

/*!
 *@brief get ops registration
 *@param[in] inst: DP instance ID
 *@param[in] type: ops type
 *@return ops pointer if registered, or NULL if not registered
 */
void *dp_get_ops(int inst, enum DP_OPS_TYPE type);

/*!
 * @brief get network device's MTU
 * @param[in] dev: network device pointer
 * @param[out] mtu_size: return the maximum MTU can be supported
 *                       for this device based on current HW configuration
 * @return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_get_mtu_size(struct net_device *dev, u32 *mtu_size);

/*!
 * @brief set network device's MTU
 * @param[in] dev: network device pointer
 * @param[in] mtu_size: new Linux network MTU requested
 * @return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_set_mtu_size(struct net_device *dev, u32 mtu_size);

/*!
 *@brief free Rx/Tx buffer
 *@param[in] info: buffer info
 *@param[in] flag: reserved
 *@return DP_SUCCESS on success and DP_FAILURE on failure
 */
int dp_free_buffer_by_policy(struct dp_buffer_info *info, u32 flag);

/*!
 *@brief get UMT ops registration
 *@param[in] inst: DP instance ID
 *@return UMT ops pointer if registered, or NULL if not registered
 */
static inline struct umt_ops *dp_get_umt_ops(int inst)
{
	return (struct umt_ops *)dp_get_ops(inst, DP_OPS_UMT);
}

#define DP_DMA_PORT_BIT_POS 16
#define DP_DMA_CTRL_BIT_POS 24

/*!
 *@brief parse DMA ID
 *@param[in] dma_id   : DMA ID
 *@param[out] cid     : DMA Controller ID
 *@param[out] pid     : DMA Port ID
 *@param[out] chid    : DMA Channel ID
 */
static inline void dp_dma_parse_id(u32 dma_id, u8 *cid, u8 *pid, u16 *chid)
{
	if (cid)
		*cid = dma_id >> DP_DMA_CTRL_BIT_POS;
	if (pid)
		*pid = (dma_id >> DP_DMA_PORT_BIT_POS) & 0xff;
	if (chid)
		*chid = dma_id & 0xffff;
}

/*!
 *@brief set DMA ID
 *@param[in] cid     : DMA Controller ID
 *@param[in] pid     : DMA Port ID
 *@param[in] chid    : DMA Channel ID
 *@return DMA ID
 */
static inline u32 dp_dma_set_id(int cid, int pid, int chid)
{
	return cid << DP_DMA_CTRL_BIT_POS |
			(pid & 0xff) << DP_DMA_PORT_BIT_POS | (chid & 0xffff);
}

/*!
 *@brief is_stream_port
 *@param[in] inst     : alloc flags
 *@param[out] true    : if stream port
 *@param[out] false   : if not stream port
 */
static inline bool is_stream_port(int flags)
{
	return !!(flags & (DP_F_FAST_ETH_LAN | DP_F_FAST_ETH_WAN | DP_F_GINT |
			   DP_F_GPON | DP_F_EPON | DP_F_VUNI));
}

/*!
 *@brief is_dc_port
 *@param[in] inst     : alloc flags
 *@param[out] true    : if dc port
 *@param[out] false   : if not dc port
 */
static inline bool is_dc_port(int flags)
{
	return !!(flags & DP_F_ACA);
}

/*!
 *@brief is_spl_conn
 *@param[in] lpid    : port LPID
 *@param[in] vap	 : VAP id
 *@retrurn bool      : true if CPU Special connection
 */
static inline bool is_spl_conn(int lpid, int vap)
{
	return (lpid == DP_CPU_LPID && vap >= DP_MAX_CPU * 2);
}

/*!
 *@brief struct dp_bp_attr enable/disable CPU port in the BP member list
 */
struct dp_bp_attr {
	int inst; /*!< [in] DP instance ID */
	struct net_device *dev; /*!< [in] Network device pointer */
	bool en; /*!< [in] DP enable/disable CPU port in the BP member list.*/
};

/*!
 *@brief dp_set_bp_attr API
 *@param[in/out] conf: bp attribute
 *@param[in] flag: reserved for future
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_set_bp_attr(struct dp_bp_attr *conf, uint32_t flag);

#define DP_CQM_LU_MODE_SET BIT(0)
#define DP_CQM_LU_MODE_GET BIT(1)

/*!
 *@brief dp_lookup_mode_cfg API
 *@param[in] inst	: Instance
 *@param[in/out] lu_mode: lu_mode set/get
 *@param[in] map	: qmap values, to set/get lookup mode
 *@param[in] flag	: DP_CQM_LU_MODE_SET/DP_CQM_LU_MODE_GET
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_lookup_mode_cfg(int inst, u32 lu_mode, struct dp_q_map *map,
		       u32 flag);

/*!
 *@brief dp_net_dev_get_ss_stat_strings API - function used by ethtool
 *	 to retrieve set of counters' names
 *@param[in] dev: net device
 *@param[out] data: buffer pointer where the function will copy counters' names
 */
void dp_net_dev_get_ss_stat_strings(struct net_device *dev, u8 *data);

/*!
 *@brief dp_net_dev_get_ss_stat_strings_count API - function used by ethtool
 *	 to retrieve number of counters
 *@param[in] dev: net device
 *@return number of counters
 */
int dp_net_dev_get_ss_stat_strings_count(struct net_device *dev);

/*!
 *@brief dp_net_dev_get_ethtool_stats API - function used by ethtool
 *	 to retrieve counters' values
 *@param[in] dev: net device
 *@param[out] stats: for dumping NIC-specific statistics
 *@param[out] data: for dumping counters values sequence
 */
void dp_net_dev_get_ethtool_stats(struct net_device *dev,
				  struct ethtool_stats *stats, u64 *data);

#define DP_OPS_RESET		BIT(0) /*!< set ops to original ops */
#define DP_OPS_NETDEV		BIT(1) /*!< set ops for net_device_ops */
#define DP_OPS_ETHTOOL		BIT(2) /*!< set ops for ethtool_ops */
#define DP_OPS_SWITCHDEV	BIT(3) /*!< set ops for switchdev_ops */
#define DP_OPS_XFRMDEV		BIT(4) /*!< set ops for xfrmdev_ops */

/*!
 *@brief dp_set_net_dev_ops API
 *@param[in] dev: pointer to net_device structure
 *@param[in] ops_callback: pointer to new callback,
 *                         will be ignore if DP_OPS_RESET is set in flag
 *@param[in] ops_offset: offset of the new callback,
 *                       e.g. offsetof(const struct net_device_ops, ndo_xxx)
 *@param[in] flag: ops to change - DP_OPS_x
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 *@note: Set to new ops - dp_set_net_dev_ops(dev, ops, ops_offset, DP_OPS_NETDEV
 *       Reset to orginal ops - dp_set_net_dev_ops(dev, NULL, ops_offset,
 *                                                 DP_OPS_NETDEV|DP_OPS_RESET);
 */
int dp_set_net_dev_ops(struct net_device *dev, void *ops_cb, int ops_offset,
		       u32 flag);

struct dp_dc_res {
	int inst;    /*! <in> dp instance */
	int dp_port; /*! <in> dp port id/lpid/ep */
	int res_id;  /*! <in> resource ID */
	struct dp_dc_rx_res rx_res;  /*! <out> rx resource */
	struct dp_dc_tx_res tx_res;  /*! <out> tx resource */
};

int dp_get_dc_config(struct dp_dc_res *res, int flag);

/*!
 *@brief dp_set_br_vlan_limit API, will initialize Bridge VLAN
 *with how many vlanid and bridgeid supported per bridge
 *@param[in] br_dev: Specifies bridge device to set the limit
 *@param[in] maxvlanid: Specifies Maximum VLAN ID supported
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_set_br_vlan_limit(struct net_device *br_dev, u32 maxvlanid);

/* Sub-Blocks user data which will be used to manipulate rule creation */
struct dp_pce_user_data {
	bool idx_en;	/* Enable user to request index within range */
	u32 idx_s;	/* Requested start index */
	u32 idx_e;	/* Requested end index */
};

/*!
 *@brief struct dp_subblk_info to pass PCE Sub-Block Information
 */
struct dp_subblk_info {
	int portid;   		/*!< [in] LPID, Used only for CTP Region */
	int subif;    		/*!< [in] SubifIdGroup/VAP, Used only
				 * for CTP Region
				 */
	bool subblk_protected;	/*!< [in] 1 - Sub-block is protected
			         * delete will be done by user
			         * 0 - Sub-block is Un-Protected,
			         * delete will be done once last rule gets
			         * deleted
			         */
	int subblk_id;		/*!< [in/out] Sub-Block Type ID
				 *   while add ID will be returned back to user
				 *   While del, ID need to be passed to API
				 */
	int subblk_firstidx;	/*!< [in] If not <=0, use this as first Idx to
				 *   allocate
				 */
	int subblk_size;	/*!< [in] Sub-Block Size */
	char subblk_name[32];  /*!< [in] Name for sub-block */
	struct dp_pce_user_data *user_data; /*!< [in] Optional user data */
};

/*!
 *@brief struct dp_pce_blk_info which contains all information needed to create
 * block and Sub-Block
 */
struct dp_pce_blk_info {
	int inst;    /*! <in> dp instance */
	GSW_PCE_RuleRegion_t region; /*!< [in] Region CTP/Global for PCE rule */
	/*!< different block type will have different information */
	union {
		struct dp_subblk_info info;	/*!< DP Sub-Block Information */
		int dummy; /*!< For Future use */
	};
};

/*!
 *@brief enum DP_PCE_UPDATE which tells DPM to enable/disable or update the 
 * PCE rule
 */
enum DP_PCE_RULE_UPDATE {
	DP_PCE_RULE_NOCHANGE = 0, /*!< pce rule no change */
	DP_PCE_RULE_EN, /*!< pce rule enable */
	DP_PCE_RULE_DIS, /*!< pce rule disable */
	DP_PCE_RULE_REWRITE, /*!< pce rule rewrite any pattern or action */
};

/*!
 *@brief dp_pce_blk_create API, will add Blk, Sub-Blk and reserve
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_blk_create(struct dp_pce_blk_info *pce_blk_info);

/*!
 *@brief dp_pce_rule_add API, will add Blk, Sub-Blk, if not present and Pce-Rule
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@param[in] pce: PCE Rule to add
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_rule_add(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_rule_t *pce);

/*!
 *@brief dp_pce_rule_del API, will delete pce-rule,
 *Blk and Sub-Blk deleted if un-protected
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@param[in] pce: PCE Rule to Delete
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_rule_del(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_ruleDelete_t *pce);

/*!
 *@brief dp_pce_rule_get Get the PCE rule inside Sub-Block
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@param[in] pce: PCE Rule from sub-block will be returned back
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_rule_get(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_rule_t *pce);

/*!
 *@brief dp_pce_rule_update Update the PCE rule inside Sub-Block
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@param[in] pce: PCE Rule to update
 *@param[in] update: PCE Rule update operation Enable/Disable/ReWrite
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_rule_update(struct dp_pce_blk_info *pce_blk_info,
		       GSW_PCE_rule_t *pce, enum DP_PCE_RULE_UPDATE update);

/*!
 *@brief dp_pce_blk_del API, will delete Blk, Sub-Blk and all pce rules inside
 *@param[in] pce_blk_info: pointer to dp_pce_blk_info structure
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_pce_blk_del(struct dp_pce_blk_info *pce_blk_info);


/*!
 *@brief dp_get_tx_cbm_pkt API, get CPU CQM TX counter via port_id and
 *       subifid group
 *@param[in] inst: dp instance
 *@param[in] dp_port: dp port id
 *@param[subif_id_grp] subifid group or vap
 *@return CPU TX counter on succeed, otherise return 0
 */
u32 dp_get_tx_cbm_pkt(int inst, int dp_port, int subif_id_grp);

/*!
 *@brief dp_get_reinsert_cnt API, get reinsertion counter
 *@param[in] inst: dp instance
 *@param[in] dp_port dp port id
 *@param[in] vap subif group or vap
 *@param[in] flag 
 *@param[out] dp_reins_count output of reinsertion counter
 *@return DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_get_reinsert_cnt(int inst, int dp_port, int vap,
				   int flag, 
				   struct dp_reinsert_count *dp_reins_count);
int dp_get_lct_cnt(int inst, int dp_port, int flag,
		   u32 *lct_idx, struct dp_lct_rx_cnt *dp_lct_rx_count);
/*!
 *@brief dp_is_ready API, check whether DPM initialize is done or not
 *@return true if DPM initialization done, otherwise return false;
 */
bool dp_is_ready(void);

/* for paritial dpm feed workaround */
int dp_late_register_ops(void);
int dp_late_register_event_cb(void);
int dp_late_register_tx(void);
#endif /*CONFIG_INTEL_DATAPATH_HAL_GSWIP30*/
#endif /*DATAPATH_API_H */

