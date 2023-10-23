/******************************************************************************
 **
 ** FILE NAME 	: ppa_hook.c
 ** PROJECT 	: PPA
 ** MODULES 	: PPA Protocol Stack Hooks
 **
 ** DATE 	: 3 NOV 2008
 ** AUTHOR 	: Xu Liang
 ** DESCRIPTION : PPA Protocol Stack Hook Pointers
 ** COPYRIGHT	: Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 ** HISTORY
 ** $Date $Author $Comment
 ** 03 NOV 2008 Xu Liang Initiate Version
 ** 20 SEP 2017 Eradath Kamal PPA leraning based on nf_hook is added
 *******************************************************************************/
/*
 * ####################################
 * Version No.
 * ####################################
 */
#define VER_FAMILY 0x60 /* bit 0: res*/
/* 1: Danube*/
/* 2: Twinpass*/
/* 3: Amazon-SE*/
/* 4: res*/
/* 5: AR9*/
/* 6: GR9*/
#define VER_DRTYPE 0x08 /* bit 0: Normal Data Path driver*/
/* 1: Indirect-Fast Path driver*/
/* 2: HAL driver*/
/* 3: Hook driver*/
/* 4: Stack/System Adaption Layer driver*/
/* 5: PPA API driver*/
#define VER_INTERFACE 0x00 /* bit 0: MII 0*/
/* 1: MII 1*/
/* 2: ATM WAN*/
/* 3: PTM WAN*/
#define VER_ACCMODE 0x03 /* bit 0: Routing*/
/* 1: Bridging*/
#define VER_MAJOR 0
#define VER_MID 0
#define VER_MINOR 2

/*
 * ####################################
 * Head File
 * ####################################
 */

/*
 * Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>
#include <asm/atomic.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/protocol.h>
#include <net/xfrm.h>
#include <linux/once.h>
#include <linux/jhash.h>
#include <linux/skbuff.h>
#include <net/netns/hash.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_bridge.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_conntrack.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_acct.h>
/*
 * Chip Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_stack_al.h>
#include <net/ppa/ppa_api_directpath.h>

/*
 * ####################################
 * Export PPA hook functions
 * ####################################
 */

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
/**********************************************************************************************
 * PPA unicast routing hook function:ppa_hook_session_add_fn
 * It it used to check unicast routing session and if necessary, add it to PPE FW to acceleration this session.
 * This hook will be invoked when a packet is handled before NAT and after NAT
 * input parameter PPA_BUF *: IP packet
 * input parameter PPA_SESSION *: routing session in network stack
 * input parameter uint32_t: this flag mainly to indicate the hook is invoked before NAT or after NAT
 * return: PPA_SESSION_ADDED: this session is added into PPE FW, ie, it is accelerated by PPE
 * PPA_SESSION_EXISTS: already added into PPE FW
 * PPA_SESSION_NOT_ADDED: not added int PPE FW for some reasons.
 * ...
 **********************************************************************************************/
int32_t (*ppa_hook_session_add_fn)(PPA_BUF *, PPA_SESSION *, uint32_t) = NULL;
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
int32_t (*ppa_hook_session_bradd_fn)(PPA_BUF *, PPA_SESSION *, uint32_t) = NULL;
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/

#endif /*CONFIG_PPA_RT_SESS_LEARNING*/
/**********************************************************************************************
 * PPA unicast routing hook function:ppa_hook_session_add_fn
 * It it used to delete a unicast routing session when it is timeout, reset or purposely.
 * input parameter PPA_SESSION *: the session pointer
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: this session is deleted sucessfully from PPE FW
 * ...
 **********************************************************************************************/
int32_t (*ppa_hook_session_del_fn)(PPA_SESSION *, uint32_t) = NULL;

void (*ppa_hook_pitem_refcnt_inc_fn)(PPA_BUF *) = NULL;
void (*ppa_hook_pitem_refcnt_dec_fn)(PPA_BUF *) = NULL;
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
int32_t (*ppa_hook_session_ipsec_add_fn)(PPA_XFRM_STATE *, sa_direction) = NULL;
int32_t (*ppa_hook_session_ipsec_del_fn)(PPA_XFRM_STATE *) = NULL;
#endif

/**********************************************************************************************
 * PPA session priority hook function:ppa_hook_session_prio_fn
 * It is used to get session priority of a ppa session.
 * input parameter PPA_SESSION *: the session pointer
 * input parameter uint32_t: for future purpose
 * return: session priority
 * ...
 **********************************************************************************************/
int32_t (*ppa_hook_session_prio_fn)(PPA_SESSION *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA unicast routing hook function:ppa_hook_get_ct_stats_fn
 * It it used to read the statistics counters of specified session
 * input parameter PPA_SESSION *: the session pointer
 * input parameter PPA_CT_COUNTER *: the session stats counter structure pointer
 * return: PPA_SUCCESS: the session statistics read sucessfully
 **********************************************************************************************/

int32_t (*ppa_hook_get_ct_stats_fn)(PPA_SESSION *, PPA_CT_COUNTER*) = NULL;
/**********************************************************************************************
 * PPA unicast hook function:ppa_hook_inactivity_status_fn
 * It it used to check whether a unicast session is timeout or not.
 * Normally it is called by network stack which want to delete a session without any traffic for sometime.
 * input parameter PPA_U_SESSION *: the unicast session pointer
 * return: PPA_HIT: the session still hit and should keep it
 * PPA_TIMEOUT: timeout already
 **********************************************************************************************/
int32_t (*ppa_hook_inactivity_status_fn)(PPA_U_SESSION *) = NULL;

/**********************************************************************************************
 * PPA unicast hook function:ppa_hook_set_inactivity_fn
 * It it used to set one unicast session timeout value
 * Normally it is called to prolong one sessions duration.
 * input parameter PPA_U_SESSION *: the unicast session pointer
 * input parameter int32_t: the timeout value
 * return: PPA_SUCCESS: update timeout sucessfully
 * PPA_FAILURE: fail to update timeout value

 **********************************************************************************************/
int32_t (*ppa_hook_set_inactivity_fn)(PPA_U_SESSION*, int32_t) = NULL;

#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
/**********************************************************************************************
 * PPA briding learning hook function:ppa_hook_bridge_entry_add_fn
 * It it used to add a bridging mac addresss into PPE FW.
 * This hook will be invoked when bridging learned a mac address or purposely to filter one mac address.
 * input parameter uint8_t*: the mac address
 * input parameter PPA_NETIF *: the interface which learned the mac address
 * input parameter PPA_NETIF *: bridge interface
 * input parameter uint32_t: PPA_F_STATIC_ENTRY means static mode,
 * PPA_F_DROP_PACKET means drop the packet, otherwise accelerate the packet.
 * return: PPA_SESSION_ADDED: this session is added into PPE FW, ie, it is accelerated by PPE
 * PPA_SESSION_EXISTS: already added into PPE FW
 * PPA_SESSION_NOT_ADDED: not added int PPE FW for some reasons.
 **********************************************************************************************/
int32_t (*ppa_hook_bridge_entry_add_fn)(uint8_t *, PPA_NETIF *, PPA_NETIF *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA briding learning hook function:ppa_hook_bridge_entry_delete_fn
 * It it used to delete a bridging mac addresss from PPE FW.
 * Normally this hook will be invoked when mac address timeout.
 * input parameter uint8_t*: the mac address
 * input parameter PPA_NETIF *: bridge interface
 * input parameter uint32_t: flag for future purpose
 * return: PPA_SESSION_ADDED: this session is added into PPE FW, ie, it is accelerated by PPE
 * return: PPA_SUCCESS: set timeout value sucessfully.
 * PPA_FAILURE: failed to update
 **********************************************************************************************/
int32_t (*ppa_hook_bridge_entry_delete_fn)(uint8_t *, PPA_NETIF *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA bridge hook function:ppa_hook_bridge_entry_hit_time_fn
 * It it used to get last hit time
 * input parameter uint8_t *: the mac address
 * input parameter PPA_NETIF *: bridge interface
 * input parameter uint32_t *: the last hit time
 * return: PPA_HIT: sucessfully get the last hit time
 * PPA_SESSION_NOT_ADDED: the mac address not in PPE FW yet
 **********************************************************************************************/
int32_t (*ppa_hook_bridge_entry_hit_time_fn)(uint8_t *, PPA_NETIF *, uint32_t *) = NULL;

#endif /*CONFIG_PPA_BR_MAC_LEARNING*/

#if defined(PPA_IF_MIB) && PPA_IF_MIB
/**********************************************************************************************
 * PPA mibs hook function:ppa_hook_get_accel_stats_fn
 * It it used to port acclerated mib status, like accelerated rx/tx packet number and so on
 * input parameter PPA_IFNAME *: specify which port's acceleration mibs want to get
 * output parameter PPA_ACCEL_STATS *
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: get sucessfully
 * PPA_FAILURE: fail to get
 **********************************************************************************************/
int32_t (*ppa_hook_get_netif_accel_stats_fn)(PPA_IFNAME *, PPA_NETIF_ACCEL_STATS *, uint32_t) = NULL;
#endif

#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT) && CONFIG_PPA_API_DIRECTCONNECT
/**********************************************************************************************
 * PPA interface hook function:ppa_check_if_netif_fastpath_fn
 * It it used to check if network interface like, WAVE500, ACA is a fastpath interface
 * input parameter PPA_NETIF *: pointer to stack network interface structure
 * input parameter char *: interface name
 * input parameter uint32_t: for future purpose
 * return: 1: if ACA or WLAN fastpath interface
 * 0: otherwise
 **********************************************************************************************/
int32_t (*ppa_check_if_netif_fastpath_fn)(PPA_NETIF *, char *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA interface hook function:ppa_hook_disconn_if_fn
 * It is used to delete one WAVE500 STA from PPA
 * input parameter PPA_NETIF *: the linux interface name, like wlan0
 * input parameter PPA_DP_SUBIF *: WAVE500 port id and subif id including station id
 * input parameter uint8_t *: WAVE500 STA mac address
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: delete sucessfully
 * PPA_FAILURE: fail to delete
 **********************************************************************************************/
int32_t (*ppa_hook_disconn_if_fn)(PPA_NETIF *, PPA_DP_SUBIF *, uint8_t *, uint32_t) = NULL;

#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
static RAW_NOTIFIER_HEAD(ppa_event_chain);

int ppa_register_event_notifier(struct notifier_block *nb)
{
	return raw_notifier_chain_register(&ppa_event_chain, nb);
}
EXPORT_SYMBOL(ppa_register_event_notifier);

int ppa_unregister_event_notifier(struct notifier_block *nb)
{
	return raw_notifier_chain_unregister(&ppa_event_chain, nb);
}
EXPORT_SYMBOL(ppa_unregister_event_notifier);

int ppa_call_class2prio_notifiers(unsigned long val,
				  s32 port_id, PPA_NETIF *dev,
				  u8 class2prio[MAX_TC_NUM])
{
	struct ppa_class2prio_notifier_info info;

	info.port_id = port_id;
	info.dev = dev;
	memcpy(info.class2prio, class2prio, (sizeof(u8) * MAX_TC_NUM));

	return raw_notifier_call_chain(&ppa_event_chain, val, &info);
}
EXPORT_SYMBOL(ppa_call_class2prio_notifiers);
#endif /* WMM_QOS_CONFIG */
#endif /* CONFIG_PPA_API_DIRECTCONNECT */

#if IS_ENABLED(CONFIG_PPA_API_DIRECTPATH)
int32_t (*ppa_directpath_port_add_fn)(void) = NULL;
int32_t (*ppa_check_if_netif_directpath_fn)(PPA_NETIF *netif,  uint16_t flag) = NULL;
int32_t (*ppa_phys_port_add_hook_fn)(PPA_IFNAME *ifname, uint32_t port) = NULL;
int32_t (*ppa_phys_port_remove_hook_fn)(uint32_t port) = NULL;
#endif

/**********************************************************************************************
 * PPA Extra ethernet interface hook function :ppa_hook_addppa_hook_directpath_register_dev_fn_if_fn
 * it is used to register/de-register a device for direct path support
 * output parameter uint32_t *: return the virtual port id
 * input parameter PPA_NETIF *: the linux interface name, like wlan0
 * input parameter PPA_DIRECTPATH_CB *: mainly provide callback function, like start_tx_fn, stop_tx_fn, rx_fn
 * input parameter uint32_t: PPA_F_DIRECTPATH_REGISTER for register, otherwise for de-register
 * return: PPA_SUCCESS: register sucessfully
 * others: fail to register
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_register_dev_fn)(uint32_t *, PPA_NETIF *, PPA_DIRECTPATH_CB *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extended ethernet interface hook function :ppa_hook_addppa_hook_directpath_register_dev_fn_if_fn
 * it is used to register/de-register a device for direct path support
 * output parameter PPA_SUBIF *: return the virtual port id and sub interface id
 * input parameter PPA_NETIF *: the linux interface name, like wlan0
 * input parameter PPA_DIRECTPATH_CB *: mainly provide callback function, like start_tx_fn, stop_tx_fn, rx_fn
 * input parameter uint32_t: PPA_F_DIRECTPATH_REGISTER for register, otherwise for de-register
 * return: PPA_SUCCESS: register sucessfully
 * others: fail to register
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_ex_register_dev_fn)(PPA_SUBIF *, PPA_NETIF *, PPA_DIRECTPATH_CB *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_send_fn
 * it is used to send a packet to PPE FW when extra ethernet interface receive a packet
 * input parameter uint32_t: the virtual port id
 * input parameter PPA_SKBUF *: the packet to send
 * input parameter int32_t *: the packet length
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: send sucessfully
 * others: fail to send
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_send_fn)(uint32_t, PPA_SKBUF *, int32_t, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_ex_send_fn
 * it is used to send a packet to PPE FW when extra ethernet interface receive a packet
 * input parameter PPA_SUBIF: the virtual port id and sub interface id
 * input parameter int32_t *: the packet length
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: send sucessfully
 * others: fail to send
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_ex_send_fn)(PPA_SUBIF *, PPA_SKBUF *, int32_t, uint32_t) = NULL;

/*Note, ppa_hook_directpath_enqueue_to_imq_fn will be set by imq module during initialization */
int32_t (*ppa_hook_directpath_enqueue_to_imq_fn)(PPA_SKBUF *skb, uint32_t portID) = NULL;
/*it will be called by imq module */
int32_t (*ppa_hook_directpath_reinject_from_imq_fn)(int32_t rx_if_id, PPA_SKBUF *buf, int32_t len, uint32_t flags) = NULL;
int32_t ppa_directpath_imq_en_flag;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_rx_stop_fn
 * it is used to stop forward packet to extra ethernet interface
 * input parameter uint32_t: the virtual port id
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: stop sucessfully
 * others: fail to stop
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_rx_stop_fn)(uint32_t, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_ex_rx_stop_fn
 * it is used to stop forward packet to extra ethernet interface
 * input parameter PPA_SUBIF: the virtual port id and sub interface id
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: stop sucessfully
 * others: fail to stop
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_ex_rx_stop_fn)(PPA_SUBIF *, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_rx_restart_fn
 * it is used to restart forwarding packet to extra ethernet interface
 * input parameter uint32_t: the virtual port id
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: restart forwarding sucessfully
 * others: fail to restart forwarding
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_rx_restart_fn)(uint32_t, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_directpath_ex_rx_restart_fn
 * it is used to restart forwarding packet to extra ethernet interface
 * input parameter PPA_SUBIF: the virtual port id and sub interface id
 * input parameter uint32_t: for future purpose
 * return: PPA_SUCCESS: restart forwarding sucessfully
 * others: fail to restart forwarding
 **********************************************************************************************/
int32_t (*ppa_hook_directpath_ex_rx_restart_fn)(PPA_SUBIF *, uint32_t) = NULL;
int32_t (*ppa_hook_directpath_recycle_skb_fn)(PPA_SUBIF*, PPA_SKBUF*, uint32_t) = NULL;
PPA_SKBUF* (*ppa_hook_directpath_alloc_skb_fn)(PPA_SUBIF*, int32_t, uint32_t) = NULL;

/**********************************************************************************************
 * PPA Extra interface hook function :ppa_hook_get_ifid_for_netif_fn
 * it is used to get one extran ethernet interface virtual port id according to its interface pointer
 * input parameter uint32_t: network interface pointer
 * return: virtual port id: if sucessully, otherwise return -1, ie, PPA_FAILURE
 **********************************************************************************************/
int32_t (*ppa_hook_get_ifid_for_netif_fn)(PPA_NETIF *) = NULL;

uint32_t (*ppa_is_ipv4_gretap_fn)(struct net_device *dev) = NULL;
uint32_t (*ppa_is_ipv6_gretap_fn)(struct net_device *dev) = NULL;

uint32_t (*ppa_is_vxlan_fn)(struct net_device *dev) = NULL;

/*PPP related functions */
/* ppp_generic.c will register function for getting ppp info once the ppp.ko is insmoded */
int32_t (*ppa_ppp_get_chan_info_fn)(struct net_device *ppp_dev, struct ppp_channel **chan) = NULL;
int32_t (*ppa_check_pppoe_addr_valid_fn)(struct net_device *dev, struct pppoe_addr *pa) = NULL;
int32_t (*ppa_get_pppoa_info_fn)(struct net_device *dev, void *pvcc, uint32_t pppoa_id, void *value) = NULL;
int32_t (*ppa_get_pppol2tp_info_fn)(struct net_device *dev, void *po, uint32_t pppol2tp_id, void *value) = NULL;
int32_t (*ppa_if_is_ipoa_fn)(struct net_device *netdev, char *ifname) = NULL;
int32_t (*ppa_if_is_br2684_fn)(struct net_device *netdev, char *ifname) = NULL;
int32_t (*ppa_br2684_get_vcc_fn)(struct net_device *netdev, struct atm_vcc **pvcc) = NULL;
int32_t (*ppa_if_ops_veth_xmit_fn)(struct net_device *dev) = NULL;
struct net_device* (*ppa_get_bond_xmit_xor_intrf_hook)(struct sk_buff *skb, struct net_device *bond_dev) = NULL;

#if IS_ENABLED(CONFIG_PPA_QOS)
int32_t (*ppa_hook_get_qos_qnum)(uint32_t portid, uint32_t flag) = NULL;
int32_t (*ppa_hook_get_qos_mib)(uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag) = NULL;
int32_t (*ppa_hook_set_ctrl_qos_rate)(uint32_t portid, uint32_t enable, uint32_t flag) = NULL;
int32_t (*ppa_hook_get_ctrl_qos_rate)(uint32_t portid, uint32_t *enable, uint32_t flag) = NULL;
int32_t (*ppa_hook_set_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t rate, uint32_t burst, uint32_t flag) = NULL;
int32_t (*ppa_hook_get_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t *rate, uint32_t *burst, uint32_t flag) = NULL;
int32_t (*ppa_hook_reset_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t flag) = NULL;
int32_t (*ppa_hook_set_ctrl_qos_wfq)(uint32_t portid, uint32_t enable, uint32_t flag) = NULL;
int32_t (*ppa_hook_get_ctrl_qos_wfq)(uint32_t portid, uint32_t *enable, uint32_t flag) = NULL;
int32_t (*ppa_hook_set_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag) = NULL;
int32_t (*ppa_hook_get_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag) = NULL;
int32_t (*ppa_hook_reset_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t flag) = NULL;
#endif /*end of CONFIG_PPA_QOS*/

#ifdef CONFIG_INTEL_IPQOS_MARK_SKBPRIO
/*
 * Function to mark priority based on specific criteria
 */
static inline
int skb_mark_priority(struct sk_buff *skb)
{
	unsigned old_priority = skb->priority;
	/*
	 * IPQoS in UGW: added copy of nfmark set in classifier to skb->priority to be used in hardware queues.
	 * nfmark range = 1-8 if QoS is enabled; priority range = 0-7; else preserve original priority
	 */
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	if (skb->extmark) {
		unsigned value;
		GET_DATA_FROM_MARK_OPT(skb->extmark, QUEPRIO_MASK, QUEPRIO_START_BIT_POS, value);
		if (value)
			skb->priority = value - 1;
	}
#endif /* CONFIG_NETWORK_EXTMARK*/

	return old_priority;
}

#endif /* CONFIG_INTEL_IPQOS_MARK_SKBPRIO*/

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
/**************************************************************************************************
 * PPA based software acceleration function hook
 * gets called from netif_rx in dev.c
 * bye pass the linux sw stack and routes the packet based on the ppa session entry
 * input : sk_buff of incoming packet
 * output: PPA_SUCCESS if the packet is accelerated
 * 	 PPA_FAILURE if the packet is not accelerated
 **************************************************************************************************/
int32_t (*ppa_hook_sw_fastpath_send_fn)(struct sk_buff *skb) = NULL;
int32_t (*ppa_hook_set_sw_fastpath_enable_fn)(uint32_t flags) = NULL;
int32_t (*ppa_hook_get_sw_fastpath_status_fn)(uint32_t flags) = NULL;
#endif

#if IS_ENABLED(CONFIG_L2NAT_MODULE) || defined(CONFIG_L2NAT)
/**********************************************************************************************
 * PPA interface hook function:ppa_check_if_netif_l2nat_fn
 * It it used to check if network interface is an l2nat interface
 * input parameter PPA_NETIF *: pointer to stack network interface structure
 * input parameter char *: interface name
 * input parameter uint32_t: for future purpose
 * return: 1: if network interafce is an l2nat interface
 * 0: otherwise
 **********************************************************************************************/
int32_t (*ppa_check_if_netif_l2nat_fn)(PPA_NETIF *, char *, uint32_t) = NULL;
#endif

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
EXPORT_SYMBOL(ppa_hook_session_add_fn);
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
EXPORT_SYMBOL(ppa_hook_session_bradd_fn);
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/
EXPORT_SYMBOL(ppa_hook_pitem_refcnt_inc_fn);
EXPORT_SYMBOL(ppa_hook_pitem_refcnt_dec_fn);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
EXPORT_SYMBOL(ppa_hook_session_ipsec_add_fn);
EXPORT_SYMBOL(ppa_hook_session_ipsec_del_fn);
#endif
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/
EXPORT_SYMBOL(ppa_hook_session_del_fn);
EXPORT_SYMBOL(ppa_hook_get_ct_stats_fn);
EXPORT_SYMBOL(ppa_hook_session_prio_fn);
EXPORT_SYMBOL(ppa_hook_inactivity_status_fn);
EXPORT_SYMBOL(ppa_hook_set_inactivity_fn);

#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
EXPORT_SYMBOL(ppa_hook_bridge_entry_add_fn);
EXPORT_SYMBOL(ppa_hook_bridge_entry_delete_fn);
EXPORT_SYMBOL(ppa_hook_bridge_entry_hit_time_fn);
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/

#if defined(PPA_IF_MIB) && PPA_IF_MIB
EXPORT_SYMBOL(ppa_hook_get_netif_accel_stats_fn);
#endif

#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT)
EXPORT_SYMBOL(ppa_check_if_netif_fastpath_fn);
EXPORT_SYMBOL(ppa_hook_disconn_if_fn);
#endif

#if IS_ENABLED(CONFIG_PPA_API_DIRECTPATH)
EXPORT_SYMBOL(ppa_directpath_port_add_fn);
EXPORT_SYMBOL(ppa_check_if_netif_directpath_fn);
EXPORT_SYMBOL(ppa_phys_port_add_hook_fn);
EXPORT_SYMBOL(ppa_phys_port_remove_hook_fn);
#endif
EXPORT_SYMBOL(ppa_hook_directpath_register_dev_fn);
EXPORT_SYMBOL(ppa_hook_directpath_ex_register_dev_fn);
EXPORT_SYMBOL(ppa_hook_directpath_send_fn);
EXPORT_SYMBOL(ppa_hook_directpath_ex_send_fn);
EXPORT_SYMBOL(ppa_hook_directpath_rx_stop_fn);
EXPORT_SYMBOL(ppa_hook_directpath_ex_rx_stop_fn);
EXPORT_SYMBOL(ppa_hook_directpath_rx_restart_fn);
EXPORT_SYMBOL(ppa_hook_directpath_ex_rx_restart_fn);
EXPORT_SYMBOL(ppa_hook_directpath_alloc_skb_fn);
EXPORT_SYMBOL(ppa_hook_directpath_recycle_skb_fn);
EXPORT_SYMBOL(ppa_hook_get_ifid_for_netif_fn);
EXPORT_SYMBOL(ppa_hook_directpath_enqueue_to_imq_fn);
EXPORT_SYMBOL(ppa_hook_directpath_reinject_from_imq_fn);
EXPORT_SYMBOL(ppa_directpath_imq_en_flag);

EXPORT_SYMBOL(ppa_get_pppoa_info_fn);
EXPORT_SYMBOL(ppa_check_pppoe_addr_valid_fn);
EXPORT_SYMBOL(ppa_ppp_get_chan_info_fn);
EXPORT_SYMBOL(ppa_is_ipv4_gretap_fn);
EXPORT_SYMBOL(ppa_is_ipv6_gretap_fn);
EXPORT_SYMBOL(ppa_is_vxlan_fn);
EXPORT_SYMBOL(ppa_get_pppol2tp_info_fn);
EXPORT_SYMBOL(ppa_if_is_ipoa_fn);
EXPORT_SYMBOL(ppa_if_is_br2684_fn);
EXPORT_SYMBOL(ppa_br2684_get_vcc_fn);
EXPORT_SYMBOL(ppa_if_ops_veth_xmit_fn);
EXPORT_SYMBOL(ppa_get_bond_xmit_xor_intrf_hook);
#if IS_ENABLED(CONFIG_PPA_QOS)
EXPORT_SYMBOL(ppa_hook_get_qos_qnum);
EXPORT_SYMBOL(ppa_hook_get_qos_mib);
EXPORT_SYMBOL(ppa_hook_set_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_hook_get_ctrl_qos_rate);
EXPORT_SYMBOL(ppa_hook_set_qos_rate);
EXPORT_SYMBOL(ppa_hook_get_qos_rate);
EXPORT_SYMBOL(ppa_hook_reset_qos_rate);
EXPORT_SYMBOL(ppa_hook_set_ctrl_qos_wfq);
EXPORT_SYMBOL(ppa_hook_get_ctrl_qos_wfq);
EXPORT_SYMBOL(ppa_hook_set_qos_wfq);
EXPORT_SYMBOL(ppa_hook_get_qos_wfq);
EXPORT_SYMBOL(ppa_hook_reset_qos_wfq);
#endif /*end of CONFIG_PPA_QOS*/

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
EXPORT_SYMBOL(ppa_hook_set_sw_fastpath_enable_fn);
EXPORT_SYMBOL(ppa_hook_get_sw_fastpath_status_fn);
EXPORT_SYMBOL(ppa_hook_sw_fastpath_send_fn);
#endif
#if IS_ENABLED(CONFIG_L2NAT)
EXPORT_SYMBOL(ppa_check_if_netif_l2nat_fn);
#endif

/* Stack adaptation layer APIs that directly hooks 
with un exported kernel APIs needs to be in builtin */
int sysctl_ip_default_ttl __read_mostly = IPDEFTTL;
EXPORT_SYMBOL(sysctl_ip_default_ttl);

int32_t ppa_is_pkt_local(PPA_BUF *ppa_buf)
{
	struct dst_entry *dst = ppa_dst(ppa_buf);

	return dst != NULL && (uint32_t)(dst->input) == (uint32_t)ip_local_deliver ? 1 : 0;
}
EXPORT_SYMBOL(ppa_is_pkt_local);

int32_t ppa_is_pkt_routing(PPA_BUF *ppa_buf)
{
	struct dst_entry *dst = ppa_dst(ppa_buf);
#if IS_ENABLED(CONFIG_IPV6)
	if (ppa_is_pkt_ipv6(ppa_buf)) {
		return dst != NULL && (uint32_t)(dst->input) == (uint32_t)ip6_forward ? 1 : 0;
	}
#endif
	return dst != NULL && (uint32_t)(dst->input) == (uint32_t)ip_forward ? 1 : 0;
}
EXPORT_SYMBOL(ppa_is_pkt_routing);

int32_t ppa_is_pkt_mc_routing(PPA_BUF *ppa_buf)
{
	struct dst_entry *dst = ppa_dst(ppa_buf);
#if IS_ENABLED(CONFIG_IPV6)
	if (ppa_is_pkt_ipv6(ppa_buf)) {
		return 0;/*dst != NULL && (uint32_t)(dst->input) == (uint32_t)ip6_mc_input ? 1 : 0;*/
	}
#endif
	return dst != NULL && (uint32_t)(dst->output) == (uint32_t)ip_mc_output ? 1 : 0;
}
EXPORT_SYMBOL(ppa_is_pkt_mc_routing);

int ppa_dev_is_br(PPA_NETIF *netif)
{
	if (netif->netdev_ops)
		return (netif->netdev_ops->ndo_start_xmit == br_dev_xmit);

	return 0;
}
EXPORT_SYMBOL(ppa_dev_is_br);

struct net_bridge_fdb_entry *ppa_br_fdb_get(struct net_bridge *br,
		const unsigned char *addr,
		uint16_t vid)
{
	return __br_fdb_get(br, addr, vid);
}
EXPORT_SYMBOL(ppa_br_fdb_get);

struct rtable *ppa_ip_route_output_key(struct net *net, struct flowi4 *flp)
{
	return ip_route_output_key(net, flp);
}
EXPORT_SYMBOL(ppa_ip_route_output_key);

#if IS_ENABLED(CONFIG_IPV6)
struct dst_entry *ppa_ip6_route_output(struct net *net,
		const struct sock *sk,
		struct flowi6 *fl6)
{
	return ip6_route_output(net, sk, fl6);
}
EXPORT_SYMBOL(ppa_ip6_route_output);
#endif

/* calculate teh hash for the conntrack 			 */
/* copied from the nf_conntrack_core.c function hash_conntrack_raw*/
/* needs to be updated if the original function is updated */
static unsigned int ppa_nf_conntrack_hash_rnd;

u32 ppa_hash_conntrack_raw(const struct nf_conntrack_tuple *tuple,
		const struct net *net)
{
	unsigned int n;
	u32 seed;

	get_random_once(&ppa_nf_conntrack_hash_rnd, sizeof(ppa_nf_conntrack_hash_rnd));

	/* The direction must be ignored, so we hash everything up to the
	 * destination ports (which is a multiple of 4) and treat the last
	 * three bytes manually.
	 */
	seed = ppa_nf_conntrack_hash_rnd ^ net_hash_mix(net);
	n = (sizeof(tuple->src) + sizeof(tuple->dst.u3)) / sizeof(u32);
	return jhash2((u32 *)tuple, n, seed ^
			(((__force __u16)tuple->dst.u.all << 16) |
			 tuple->dst.protonum));
}
EXPORT_SYMBOL_GPL(ppa_hash_conntrack_raw);

#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
int32_t ppa_sw_litepath_local_deliver(struct sk_buff *skb)
{
	int ret = 0;
	const struct net_protocol *ipprot;
	int protocol = ip_hdr(skb)->protocol;
	struct net *net = dev_net(skb->dev);

	rcu_read_lock();
resubmit:
	ipprot = rcu_dereference(inet_protos[protocol]);
	if (ipprot != NULL) {
		if (!ipprot->no_policy) {
			if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
				kfree_skb(skb);
				printk("ppa_sw_litepath_local_deliver failed: %d\n", __LINE__);
				goto out;
			}
			nf_reset(skb);
		}
		ret = ipprot->handler(skb);
		if (ret < 0) {
			protocol = -ret;
			printk("ppa_sw_litepath_local_deliver failed: %d\n", __LINE__);
			goto resubmit;
		}
		__IP_INC_STATS(net, IPSTATS_MIB_INDELIVERS);
	} else {
		__IP_INC_STATS(net, IPSTATS_MIB_INDELIVERS);
		consume_skb(skb);
	}
out:
	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(ppa_sw_litepath_local_deliver);
#endif

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
/* session learning hooks */
static unsigned int ppa_prert_hook_fn (void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	struct nf_conn *ct = NULL;
	enum ip_conntrack_info ctinfo;

	uint32_t flags;

	if (ppa_hook_session_add_fn != NULL) {

		ct = nf_ct_get(skb, &ctinfo);

		if (ct) {
			ct = nf_ct_is_confirmed(ct) ? ct : NULL;
		}

		flags = PPA_F_BEFORE_NAT_TRANSFORM;
		flags |= CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL ? PPA_F_SESSION_ORG_DIR : PPA_F_SESSION_REPLY_DIR;

		ppa_hook_session_add_fn(skb, ct, flags);
	}

	return NF_ACCEPT;
}

static unsigned int ppa_postrt_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	struct nf_conn *ct = NULL;
	enum ip_conntrack_info ctinfo;
	uint32_t flags;
	/* Post routing hook gets invoked twice for IPSec Path in new PPA
	hook mechanism (similar to netfilter). So in the first call
	we skip the learning, since it will be a normal TCP/UDP session
	packet which will eventually take the IPSec Path */
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	struct iphdr *hdr = ip_hdr(skb);
	if (hdr->protocol == IPPROTO_ESP || skb_dst(skb)->flags & DST_XFRM_TUNNEL || !ppa_hook_session_add_fn)
		return NF_ACCEPT;
	/* In the Second call (After Encryption) we skip again for
	ESP type since we call this hook in xfrm_output_resume before
	old conntrack reset, based on which the p_item was created */
#else
	if (skb_dst(skb)->flags & DST_XFRM_TUNNEL || !ppa_hook_session_add_fn)
		return NF_ACCEPT;
#endif

#if IS_ENABLED(CONFIG_INTEL_IPQOS_ACCEL_DISABLE)
	/* check for 13th bit in NFMARK set by IPQOS classifier */
	/* If this bit is set, dont call PPA session add fn*/
	bool accel_st = 0;
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	GET_DATA_FROM_MARK_OPT(skb->extmark, ACCELSEL_MASK,
					 ACCELSEL_START_BIT_POS, accel_st);
#endif /* CONFIG_NETWORK_EXTMARK*/
	if (accel_st != 0)
		return NF_ACCEPT;
#endif /* CONFIG_INTEL_IPQOS_ACCEL_DISABLE*/

	ct = nf_ct_get(skb, &ctinfo);

	flags = 0; /* post routing */
	flags |= CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL ?
		 PPA_F_SESSION_ORG_DIR : PPA_F_SESSION_REPLY_DIR;

	ppa_hook_session_add_fn(skb, ct, flags);

	return NF_ACCEPT;
}

static unsigned int ppa_localin_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	struct nf_conn *ct = NULL;
	enum ip_conntrack_info ctinfo;
	uint32_t flags;
	
	if (ppa_hook_session_add_fn != NULL) {

		ct = nf_ct_get(skb, &ctinfo);
		flags = PPA_F_SESSION_LOCAL_IN;
		flags |= CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL ? PPA_F_SESSION_ORG_DIR : PPA_F_SESSION_REPLY_DIR;

		 	ppa_hook_session_add_fn(skb, ct, flags);
	}
 
	return NF_ACCEPT;
}

static unsigned int ppa_localout_hook_fn(void *priv,
					 struct sk_buff *skb,
					 const struct nf_hook_state *state)
{
	struct nf_conn *ct = NULL;
	enum ip_conntrack_info ctinfo;

	u32 flags;

	if (ppa_hook_session_add_fn) {
		ct = nf_ct_get(skb, &ctinfo);

		if (ct)
			ct = nf_ct_is_confirmed(ct) ? ct : NULL;

		flags = PPA_F_BEFORE_NAT_TRANSFORM;
		flags |= PPA_F_SESSION_LOCAL_OUT;
		flags |= (CTINFO2DIR(ctinfo) == IP_CT_DIR_ORIGINAL)
				? PPA_F_SESSION_ORG_DIR
				: PPA_F_SESSION_REPLY_DIR;

		ppa_hook_session_add_fn(skb, ct, flags);
	}

	return NF_ACCEPT;
}

static struct nf_hook_ops ipt_hook_ops[] __read_mostly = {
	/* hook for pre-routing ipv4 packets */
	{
		.hook 		= ppa_prert_hook_fn,
		.hooknum 	= 0, /*NF_IP_PRE_ROUTING*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_CONNTRACK,
	},
	/* hook for pre-routing ipv6 packets */
	{
		.hook 		= ppa_prert_hook_fn,
		.hooknum 	= 0, /*NF_IP_PRE_ROUTING*/
		.pf		= PF_INET6,
		.priority	= NF_IP6_PRI_CONNTRACK,
	},
	/* hook for post-routing ipv4 packets */
	{
		.hook 		= ppa_postrt_hook_fn,
		.hooknum 	= 4, /*NF_IP_POST_ROUTING*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for post-routing ipv6 packets */
	{
		.hook 		= ppa_postrt_hook_fn,
		.hooknum 	= 4, /*NF_IP_POST_ROUTING*/
		.pf		= PF_INET6,
		.priority	= NF_IP6_PRI_LAST,
	},
	/* hook for local in ipv4 packets */
	{
		.hook 		= ppa_localin_hook_fn,
		.hooknum 	= 1, /*NF_IP_LOCAL_IN*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for post-routing ipv6 packets */
	{
		.hook 		= ppa_localin_hook_fn,
		.hooknum 	= 1, /*NF_IP_LOCAL_IN*/
		.pf		= PF_INET6,
		.priority	= NF_IP6_PRI_LAST,
	},
	/* hook for local-out ipv4 packets */
	{
		.hook		= ppa_localout_hook_fn,
		.hooknum	= 3, /*NF_IP_LOCAL_OUT*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for local-out ipv6 packets */
	{
		.hook		= ppa_localout_hook_fn,
		.hooknum	= 3, /*NF_IP_LOCAL_OUT*/
		.pf		= PF_INET6,
		.priority	= NF_IP6_PRI_LAST,
	}
};
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

#if IS_ENABLED(CONFIG_INTEL_IPQOS)
static unsigned int ppa_qos_postrt_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MARK_SKBPRIO)
	skb_mark_priority(skb);
#endif
	return NF_ACCEPT;
}

static unsigned int ppa_qos_prert_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	uint32_t qos_priority;
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;

#if IS_ENABLED(CONFIG_INTEL_IPQOS_MARK_SKBPRIO)
	if (desc_1->field.classid != skb->priority) {
		skb->priority = desc_1->field.classid;
	}
	qos_priority = skb->priority + 1;
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	SET_DATA_FROM_MARK_OPT(skb->extmark, QUEPRIO_MASK, QUEPRIO_START_BIT_POS, qos_priority );
#endif
#endif
	return NF_ACCEPT;
}

static struct nf_hook_ops qos_hook_ops[] __read_mostly = {
	/* hook for pre-routing ipv4 packets */
	{
		.hook		= ppa_qos_prert_hook_fn,
		.hooknum	= 0, /*NF_IP_PRE_ROUTING*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for pre-routing ipv6 packets */
	{
		.hook		= ppa_qos_prert_hook_fn,
		.hooknum	= 0, /*NF_IP_PRE_ROUTING*/
		.pf		= PF_INET6,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for post-routing ipv4 packets */
	{
		.hook		= ppa_qos_postrt_hook_fn,
		.hooknum	= 4, /*NF_IP_POST_ROUTING*/
		.pf		= PF_INET,
		.priority	= NF_IP_PRI_LAST,
	},
	/* hook for post-routing ipv6 packets */
	{
		.hook		= ppa_qos_postrt_hook_fn,
		.hooknum	= 4, /*NF_IP_POST_ROUTING*/
		.pf		= PF_INET6,
		.priority	= NF_IP6_PRI_LAST,
	}
};
#endif /* CONFIG_INTEL_IPQOS*/

#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
static unsigned int ppa_br_prert_hook_fn (void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	if (ppa_hook_session_add_fn != NULL) {

		ppa_hook_session_add_fn(skb,
				NULL, PPA_F_BRIDGED_SESSION|PPA_F_BEFORE_NAT_TRANSFORM);
	}
	return NF_ACCEPT;
}

static unsigned int ppa_br_postrt_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
	if (ppa_hook_session_add_fn != NULL) {
		//printk(KERN_INFO"br Post-routing hook \n");
		ppa_hook_session_add_fn(skb,
				NULL,
				PPA_F_BRIDGED_SESSION);
	}
	return NF_ACCEPT;
}

static struct nf_hook_ops ebt_hook_ops[] __read_mostly = {
	/* hook for bridge pre-routing packets */
	{
		.hook 		= ppa_br_prert_hook_fn,
		.hooknum 	= 0, /*NF_IP_PRE_ROUTING*/
		.pf		= NFPROTO_BRIDGE,
		.priority	= NF_BR_PRI_NAT_DST_BRIDGED,
	},
	/* hook for bridge post-routing packets */
	{
		.hook 		= ppa_br_postrt_hook_fn,
		.hooknum 	= 4, /*NF_IP_POST_ROUTING*/
		.pf		= NFPROTO_BRIDGE,
		.priority	= NF_BR_PRI_NAT_SRC,
	}
};
#endif /* CONFIG_PPA_BR_SESS_LEARNING*/

#if IS_ENABLED(CONFIG_INTEL_IPQOS)
#if defined(CONFIG_VLAN_8021Q_COPY_TO_EXTMARK)        
static unsigned int ppa_qos_br_prert_hook_fn(void *priv,
					     struct sk_buff *skb,
					     const struct nf_hook_state *state)
{
	u16 vlan_id = 0;
	u16 vprio = 0;
	u16 vlan_tci = skb->vlan_tci;
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;

	/* In case of trunk vlan, vlan is stripped off before
	 * reaching this hook so get the vlan from netdevice
	 */
	if (skb->dev->priv_flags & IFF_802_1Q_VLAN) {
		/* In case of trunk VLAN, skb priority which has
		 * PCE traffic class is overwrittten by pcp-qos map
		 * so copy class back to skb priority
		 */
		if (desc_1->field.classid) {
			skb->priority = desc_1->field.classid;
		} else {
			vlan_id = vlan_dev_vlan_id(skb->dev);
			vprio = skb->priority;
			SET_DATA_FROM_MARK_OPT(skb->extmark, VLANID_MASK,
					       VLANID_START_BIT_POS, vlan_id);
			SET_DATA_FROM_MARK_OPT(skb->extmark, VPRIO_MASK,
					       VPRIO_START_BIT_POS, vprio);
			/* skb priority is used to identify PCE classified
			 * packets. since this prioirty is coming from pcp-qos
			 * mapping so clearing it.
			 */
			skb->extmark |= CPU_CLASSIFICATION_MASK;
		}
	} else {
		/* In case of PCE classification skb priority is set
		 * and that will be considered for final QoS. else
		 * PCP is copied into extmark, IPtables rule is used
		 * to do final QoS.
		 */
		vprio = (vlan_tci >> VLAN_PRIO_SHIFT);
		SET_DATA_FROM_MARK_OPT(skb->extmark, VPRIO_MASK,
				       VPRIO_START_BIT_POS, vprio);
	}
	return NF_ACCEPT;
}
#endif

static unsigned int ppa_qos_br_postrt_hook_fn(void *priv,
		struct sk_buff *skb,
		const struct nf_hook_state *state)
{
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MARK_SKBPRIO)
	skb_mark_priority(skb);
#endif
	return NF_ACCEPT;
}

static struct nf_hook_ops qos_ebt_hook_ops[] __read_mostly = {
#if defined(CONFIG_VLAN_8021Q_COPY_TO_EXTMARK) 
	/* hook for bridge pre-routing packets */
	{
		.hook           = ppa_qos_br_prert_hook_fn,
		.hooknum        = NF_BR_PRE_ROUTING,
		.pf             = NFPROTO_BRIDGE,
		.priority       = NF_BR_PRI_NAT_DST_BRIDGED,
	},
#endif
	/* hook for bridge post-routing packets */
	{
		.hook		= ppa_qos_br_postrt_hook_fn,
		.hooknum	= 4, /*NF_BR_POST_ROUTING*/
		.pf		= NFPROTO_BRIDGE,
		.priority	= NF_BR_PRI_NAT_SRC,
	}
};
#endif /* CONFIG_INTEL_IPQOS*/

/*Refresh conntrack for this many jiffies and do accounting */
/*This replictes the functionality of kernel api __nf_ct_refresh_acct*/
void ppa_nf_ct_refresh_acct(struct nf_conn *ct,
		enum ip_conntrack_info ctinfo,
		unsigned long extra_jiffies,
		unsigned long bytes,
		unsigned int pkts)
{
	struct nf_conn_acct *acct = NULL;
	unsigned long more_jiffies = 0;

	if (!ct)
		return;

	/* Only update if this is not a fixed timeout */
	if (test_bit(IPS_FIXED_TIMEOUT_BIT, &ct->status))
		goto do_acct;

	/* If not in hash table, timer will not be active yet */
	if (nf_ct_is_confirmed(ct)) {
		more_jiffies = nfct_time_stamp;

		/*	printk(KERN_INFO" function %s ct->timeout =%lu extra_jiffies,=%lu\n", __FUNCTION__, ct->timeout, extra_jiffies+more_jiffies);*/
		/* Update the timeout only if it is more than existing timeout value */
		if (ct->timeout < (more_jiffies+extra_jiffies))
			ct->timeout = (more_jiffies + 3*extra_jiffies); /* update with three times extra_jiffies*/
	}

do_acct:
	acct = nf_conn_acct_find(ct);
	if (acct) {
		struct nf_conn_counter *counter = acct->counter;

		atomic64_add(pkts, &counter[CTINFO2DIR(ctinfo)].packets);
		atomic64_add(bytes, &counter[CTINFO2DIR(ctinfo)].bytes);
	}
}
EXPORT_SYMBOL(ppa_nf_ct_refresh_acct);

void (*orig_nfct_destroy)(struct nf_conntrack *) = NULL;

void ppa_destroy_conntrack (struct nf_conntrack *nfct)
{

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	struct nf_conn *ct = (struct nf_conn *)nfct;
	if (ppa_hook_session_del_fn != NULL) {
		ppa_hook_session_del_fn(ct, PPA_F_SESSION_ORG_DIR | PPA_F_SESSION_REPLY_DIR);
	}
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

	orig_nfct_destroy(nfct);
}

void ppa_register_delhook(void)
{
	orig_nfct_destroy = nf_ct_destroy;
	RCU_INIT_POINTER(nf_ct_destroy, ppa_destroy_conntrack);
}

void ppa_unregister_delhook(void)
{
	RCU_INIT_POINTER(nf_ct_destroy, orig_nfct_destroy);
	orig_nfct_destroy = NULL;
}

int ppa_api_register_hooks(void)
{
	int ret = 0;
#if IS_ENABLED(CONFIG_INTEL_IPQOS)
	/*qos ipt hooks*/
	nf_register_hooks(qos_hook_ops, ARRAY_SIZE(qos_hook_ops));
	/*qos ebt hooks*/
	nf_register_hooks(qos_ebt_hook_ops, ARRAY_SIZE(qos_ebt_hook_ops));
#endif /* CONFIG_INTEL_IPQOS*/

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	/*ipt hooks*/
	nf_register_hooks(ipt_hook_ops, ARRAY_SIZE(ipt_hook_ops));
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	/*ebt hooks*/
	nf_register_hooks(ebt_hook_ops, ARRAY_SIZE(ebt_hook_ops));
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

	/*delete conntrack hook*/
	ppa_register_delhook();

	printk(KERN_INFO"Installed the ppa netfilter hooks\n");
	return ret;
}
EXPORT_SYMBOL(ppa_api_register_hooks);

void ppa_api_unregister_hooks(void)
{
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	/*ipt hooks*/
	nf_unregister_hooks(ipt_hook_ops, ARRAY_SIZE(ipt_hook_ops));
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	/*ebt hooks*/
	nf_unregister_hooks(ebt_hook_ops, ARRAY_SIZE(ebt_hook_ops));
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

#if IS_ENABLED(CONFIG_INTEL_IPQOS)
	/*qos ipt hooks*/
	nf_unregister_hooks(qos_hook_ops, ARRAY_SIZE(qos_hook_ops));
	/*qos ebt hooks*/
	nf_unregister_hooks(qos_ebt_hook_ops, ARRAY_SIZE(qos_ebt_hook_ops));
#endif /* CONFIG_INTEL_IPQOS*/

	/*delete conntrack hook*/
	ppa_unregister_delhook();

	printk(KERN_INFO"Uninstalled the ppa netfilter hooks\n");
}
EXPORT_SYMBOL(ppa_api_unregister_hooks);

