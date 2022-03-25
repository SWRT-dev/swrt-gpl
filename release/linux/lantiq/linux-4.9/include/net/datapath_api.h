/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_API_H
#define DATAPATH_API_H

#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/atmdev.h>

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
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30) /*GRX500 GSWIP30*/
#include <net/datapath_api_umt.h>
#endif
#include <net/datapath_api_vlan.h>
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/gsw_flow_ops.h>
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
#define DP_TX_CAL_CHKSUM 1 /*!< @brief Need calculate PMAC. \n
			    *  Note, make sure pmac place holder already have\n
			    *   or set flag DP_TX_INSERT_PMAC to insert it
			    */
#define DP_TX_DSL_FCS        2	/*!< @brief For DSL FCS Checksum calculation */
#define DP_TX_INSERT_PMAC    4	/*!< @brief Oly for special test purpose */
#define DP_TX_OAM            8	/*!< @brief OAM packet */
#define DP_TX_TO_DL_MPEFW    0x10/*!< @brief Send Pkt directly to DL FW */
#define DP_TX_INSERT	     0x20 /*!< @brief GSWIP Insertion Support */
#define DP_TX_INSERT_POINT   0x40 /*!< For GSWIP Insertion Point 0 or 1 */
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

#define DP_RX_RING_NUM 2  /*!< maximum number of ACA RX ring
			   *   For GRX500/PRX300, only support 1 ring
			   *   For LGM, maximum up to 2 rings
			   */
#define DP_TX_RING_NUM 8  /*!< maximum number of ACA RXOUT ring
			   *   For GRX500/PRX300, only support 1 ring
			   *   For 5G, it needs to support up to 16 ring.
			   */
#define DP_MAX_UMT DP_RX_RING_NUM /*!< maximum number of UMT port per DC
				   *   For GRX500/PRX300, only support 1
				   *      umt port
				   *   For LGM docsis, it can support up to 2
				   */
#define DP_MAX_POLICY_GPID 4     /*!< maximum number of policy per GPID
				  *   for HW automatically free BM buffer
				  *   In LGM, it is 4. Not valid for all other
				  *   existing products
				  */
#define DP_TS_HDRLEN	10    /*!< Befault Timestamp Header Length to strip */
#define DP_DFL_SESS_NUM 16    /*!< Maximum default egress session per subif */
/* Note: In LGM kernel (4.19) branch,
 * DMA driver already adapted to linux kernel
 * DMA framework and some DMA macro (like
 * _DMA_CONTROLLER/_DMA_PORT/_DMA_CHANNEL)
 * was removed. In order to keep the interface between DP and CQM/UMT driver
 * same, DP has defined DMA macro accordingly
 */
#define _DMA_CHANBITS   16
#define _DMA_PORTBITS   8
#define _DMA_CTRLBITS   8

#define _DMA_CHANMASK   ((1 << _DMA_CHANBITS) - 1)
#define _DMA_PORTMASK   ((1 << _DMA_PORTBITS) - 1)
#define _DMA_CTRLMASK   ((1 << _DMA_CTRLBITS) - 1)

#define _DMA_CHANSHIFT  0
#define _DMA_PORTSHIFT  (_DMA_CHANSHIFT + _DMA_CHANBITS)
#define _DMA_CTRLSHIFT  (_DMA_PORTSHIFT + _DMA_PORTBITS)

#define _DMA_CONTROLLER(nr)     (((nr) >> _DMA_CTRLSHIFT) & _DMA_CTRLMASK)
#define _DMA_PORT(nr)           (((nr) >> _DMA_PORTSHIFT) & _DMA_PORTMASK)
#define _DMA_CHANNEL(nr)        (((nr) >> _DMA_CHANSHIFT) & _DMA_CHANMASK)

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

	DP_F_VUNI     = BIT(21), /*!< For vUNI device */
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
	DP_F_ENUM_OR_STRING(DP_F_VUNI,         "VUNI") \
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

/*! @brief DP Special Connectivity Type*/
enum DP_SPL_TYPE {
	DP_SPL_LRO = 0, /*!< Special connectivity type: LRO */
	DP_SPL_TSO, /*!< Special connectivity type: TSO */
	DP_SPL_VOICE, /*!< Special connectivity type: VOICE */
	DP_SPL_VPN, /*!< Special connectivity type: VPN Adaptor */
	DP_SPL_PP_2ND_PATH, /*!< Special connectivity type: a second pass via
			     *   PPV4
			     */
	DP_SPL_APP_LITEPATH, /*!< Special connectivity type: For Application
			      *   Litepath
			      */
	DP_SPL_PPV4_NFS, /*!< Special connectivity type:For PPV4 uCs-
			  *   Fragmenter, Reassembler, Multicast, TurboDox
			  */
	DP_SPL_MAX /*!< Special connectivity type: NOT VALID */
};

#define DP_PMAP_PCP_NUM 8  /*!< @brief  Max pcp entries supported per pmapper*/
#define DP_PMAP_DSCP_NUM 64 /*!<@brief  Max dscp entries supported per pmapper*/
#define DP_MAX_CTP_PER_DEV  64  /*!< @brief  max CTP per dev:
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

/*! @brief structure for dp_subif */
typedef struct dp_subif {
	s32 port_id; /*!< Datapath Port Id corresponds to PMAC Port Id */
	int inst;  /*!< dp instance id */
	int bport;  /*!< output: valid only for API dp_get_netif_subifid in the
		     *  GSWIP 3.1 or above
		     *  bridge port ID
		     */
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
	u16 gpid; /*!< [out] gpid which is mapped from dpid + subif
		   *   normally one GPID per subif for non PON device.
		   *   For PON case, one GPID per bridge port
		   */
	u8  num_q; /*!< [out] num of default number of queues allocated by DP */
	union {
		u16 def_qid; /*!< [out] def physical queue id assigned by DP */
		u16 def_qlist[DP_MAX_DEQ_PER_SUBIF]; /*!< [in/out] default
						      * physical qid list 
						      * assigned by DP */
	};
	int lookup_mode; /*!< [out] CQM lookup mode for this device
			  *   (dp_port based)
			  *   valid for dp_get_netif_subifid only
			  */
	int alloc_flag; /*!< [out] the flag value is from the top level driver
			 *    during calling dp_alloc_port_ext
			 *   output for dp_get_netif_subifid
			 *   no use for dp_register_subif_ext
			 *   This is requested by PPA/DCDP to get original flag
			 *   the caller provided to DP during
			 *   dp_alloc_port
			 */
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
	u16 dfl_eg_sess[DP_DFL_SESS_NUM]; /*!< [out] default egress session id
					   *   This is for CPU TX to DC only
					   */
	u32 data_flag; /*!< [out] return the caller provided data->flag_ops
			* during dp_register_subif
			*/
	struct net_device *associate_netif; /*!< [out] return vUNI dev pointer,
					     * valid for VANI device only
					     */
} dp_subif_t;

typedef dp_subif_t PPA_SUBIF; /*!< @brief structure type dp_subif PPA_SUBIF*/

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

struct dp_gpid_tx_info {
	/* Note for GPID max packet lengh setting, DP should get it from
	 *      struct dp_tx_ring's tx_pkt_size
	 */
	u32 f_min_pkt_len : 1; /*!< flag to indicate whether min_pkt_len is
				*   set or not by caller
				*   1: min_pkt_len is set by caller
				*   0: min_pkt_len is not set by caller.
				*/
	u32 seg_en : 1; /*!< support FSQM buffer or not
			 *   For stream port: should set to 1.
			 *   For non_stream port: shoudl set to 0
			 */

	u16 min_pkt_len; /*!< minimum packet length in bytes
			  *   valid if f_min_pkt_len is set
			  */

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
	dp_cb_t *dp_cb; /*!< [in] for subif level callback.
			 *   Mainly for docsis/voice special handling.
			 *   For wave/VRX618/518, just set to NULL.
			 */
	u16 f_tx_policy : 1;   /*!< [in] flag to indicate whether need new
				*   policy for this subif or not.
				*   if this flat is set, it needs new txin
				*       policy
				*   otherwise DP will use its existing base
				*       policy which is create during
				*       dp_register_dev_ext
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
	u16 txin_base_policy; /*!< [out] txin_policy
			       *   if f_txin_policy set, this subif will need
			       *       create new policy
			       *   else DP will use its base policy which is
			       *       create during dp_register_dev_ext
			       */

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
	DP_F_DATA_CONTINOUS_Q_RESV = BIT(7) /*!< reserve continous physical
					     *   queue ID
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
 *  4 Ring with Multipe QOS dequeue ports
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

/*! @brief enum DP_SUBIF_DATA_FLAG */
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
				 *   Note: this paramater will be forced to tune
				 *         down to HW capability
				 */
	u32   out_enq_port_id; /*!< [out] CQM enqueue port based ID */
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
	u32 out_dma_ch_to_gswip; /*!< [out] DMA TX channel base to GSWIP for
				  *   forwarding rxout packet to GSWIP
				  */
	u32 num_out_tx_dma_ch; /*!< [out] number of DMA tx channel assigned */
	u32 out_cqm_deq_port_id; /*!< [out] CQM dequeue port to GSWIP for rxout
				  *   packet
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
	struct dp_gpid_tx_info gpid_info; /*!< [in] for GPID tx information
					   *   Valid only if @f_gpid valid.
					   */

	int f_out_auto_free : 1; /*!< [in] flag to indicate whether need txout
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
			    *   For docsis, alhtough it is 16 dequeue port to WIB.
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
			* PP continious GPID block. The continuous limitation is
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

	char qos_eng_name[DP_MAX_NAME]; /*!< QOS engine name in string */
	char pkt_eng_name[DP_MAX_NAME]; /*!< Packet Engine Name String */
	int max_num_queues; /*!< max number of QOS queue supported */
	int max_num_scheds; /*!< max number of QOS scheduler supported */
	int max_num_deq_ports; /*!< max number of CQM dequeue port */
	int max_num_qos_ports; /*!< max number of QOS dequeue port */
	int max_num_dp_ports; /*!< max number of dp port */
	int max_num_subif_per_port; /*!< max number of subif per dp_port */
	int max_num_subif; /*!< max number of subif supported. Maybe no meaning?
			    */
	int max_num_bridge_port; /*!< max number of bridge port */
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
 *@brief Datapath Swdev API to get FID from bridge name
 *@param[in] dev: network device pointer
 *@param[out] inst: DP instance ID
 *@return Returns GSWIP bridge id i.e. FID
 */
int dp_get_fid_by_brname(struct net_device *dev, int *inst);

struct dp_spl_conn {
	enum DP_SPL_TYPE type; /*!< [in] Special Connectity Type */
	u32 f_subif : 1; /*!< [in] need allocate subif */
	u32 f_gpid : 1; /*!< [in] need allocate GPID */
	u32 f_gpid_policy : 1; /*!< [in] need allocate policy or not.
				*   valid only if f_gpid set
				*   For LGM:
				*     Required only for Voice CPU DQ port.
				*     TSO - Not required (No Dequeue )
				*     LRO - Not required (No GPID )
				*     VPN-A: Not required.(reuses input buffer)
				*     App Litepath: Not required
				*       (reuses the CPU DQ ports for the host).
				*     Fragmenter NF: Not required
				*       (GPID will not be used as Egress port
				*     Other NFs: Not required (No GPIDs).
				*/
	dp_cb_t *dp_cb; /*!< [in] for subif level callback.
			 *   Valid only if f_subif valid.
			 *   Mainly for voice related applicaton only
			 */
	int dp_port; /*!< [out] dp_port ID, normally it is CPU 0.
		      *   if -1, then not applicable for this special connect
		      */
	u8 num_rx_ring; /*!< [in] number of rx ring from DC device to Host.
			 *   num_rx_ring requirement:
			 *   @num_rings <= @DP_RX_RING_NUM
			 *   GRX350/PRX300:1 rx ring
			 *   LGM: up to 2 rx ring, like Docsis can use 2 rings
			 *   For two ring case:
			 *    1st rxout ring without qos
			 *    2nd rxout ring with qos
			 */
	u8 num_tx_ring; /*!< [in] number of tx ring from Host to DC device
			 *   num_rx_ring requirement:
			 *   @num_rings <= @DP_TX_RING_NUM
			 *   Normally it is 1 TX ring only.
			 *   But for 5G, it can support up to 8 TX ring
			 *   For docsis, alhtough it is 16 dequeue port to WIB.
			 *   But the final ring only 1, ie, WIB to Dcosis
			 */
	u8 num_umt_port; /*!< [in] number of UMT port.
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
};

/*!
 *@brief Datapath Manager Initialize Speical Connectivity API
 *@param[in] inst: DP instance ID
 *@param[in] conn: Special Connect information
 *@return Returns DP_SUCCESS on succeed and DP_FAILURE on failure
 */
int dp_connect_spl_path(int inst, struct dp_spl_conn *conn);

/*! @brief Enumerator DP_OPS_TYPE */
enum DP_OPS_TYPE {
	DP_OPS_BM, /*!< BM ops type */
	DP_OPS_QOS, /*!< QOS ops type */
	DP_OPS_GSW, /*!< GSWIP ops type */
	DP_OPS_UMT, /*!< UMT ops type */
	DP_OPS_LRO, /*!< LRO ops type */
	DP_OPS_CNT,  /*!< total ops type count */
};

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
int dp_get_mtu_size(struct net_device *dev, u32 *mtu_size);

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

enum DP_DATA_PORT_TYPE {
        DP_DATA_PORT_LINUX = 0, /*!< data port owned by Linux */
        DP_DATA_PORT_DPDK, /*!< data port owned by DKDP */
        DP_DATA_PORT_MPE, /*!< data port owned by MPE */
};

/*!
 *@brief parse dma ID
 *@param[in] inst     : DMA ID
 *@param[out] cid     : DMA Controller ID
 *@param[out] pid     : DMA Port ID
 *@param[out] chid    : DMA Channel ID
 */
static inline void dp_dma_parse_id(u32 dma_id, u8 *cid, u8 *pid, u16 *chid)
{
	if (cid)
		*cid = dma_id >> 24;
	if (pid)
		*pid = (dma_id >> 16) & 0xff;
	if (chid)
		*chid = dma_id & 0xffff;
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

#endif /*DATAPATH_API_H */
