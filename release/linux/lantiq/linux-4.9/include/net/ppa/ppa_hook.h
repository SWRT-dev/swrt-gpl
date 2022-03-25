#ifndef __PPA_HOOK_H__20081103_1736__
#define __PPA_HOOK_H__20081103_1736__
/******************************************************************************
 **
 ** FILE NAME    : ppa_hook.h
 ** PROJECT      : PPA
 ** MODULES      : PPA Protocol Stack Hooks
 **
 ** DATE         : 3 NOV 2008
 ** AUTHOR       : Xu Liang
 ** DESCRIPTION  : PPA Protocol Stack Hook Pointers Header File
 ** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 ** HISTORY
 ** $Date        $Author         $Comment
 ** 03 NOV 2008  Xu Liang        Initiate Version
 *******************************************************************************/
/*! \file ppa_hook.h
  \brief This file contains all exported HOOK API to linux Kernel and user space via ioctl API.
 */
#include <net/ppa/ppa_api_common.h>
/** \addtogroup  PPA_HOOK_API */
/*@{*/
/*
 * ####################################
 *             Declaration
 * ####################################
 */
#ifdef __KERNEL__
/*!
  \brief Add a PPA routing session entry
  \param[in] skb Pointer to the packet buffer for which PPA session addition is to be done.
  \param[in] p_session Points to the connection tracking session to which this packet belongs. It maybe passed as NULL in which case PPA will try to determine it using the PPA stack adaptation layer.
  \param[in] flags  Flags as valid for the PPA session Valid \n
  values are one or more of: \n
  - PPA_F_SESSION_BIDIRECTIONAL \n
  - PPA_F_BRIDGED_SESSION \n
  - PPA_F_STATIC_ENTRY \n
  - PPA_F_DROP_PACKET \n
  - PPA_F_SESSION_ORG_DIR \n
  - PPA_F_SESSION_REPLY_DIR \n
  - PPA_F_BEFORE_NAT_TRANSFORM \n
  \return The return value can be any one of the following:  \n
  - PPA_SESSION_NOT_ADDED \n
  - PPA_SESSION_ADDED \n
  - PPA_SESSION_EXISTS \n
  \note   Linux 2.4 Hook-up recommendation \n
  Must be hooked into the stack before the PREROUTING and after the POSTROUTING hooks In ip_conntrack_in() function in file ip_conntrack_core.c, with the flag PPA_F_BEFORE_NAT_TRANSFORM specified. \n
  1) In ip_finish_output2(), the hook should be invoked after NAT transforms are applied and at the very beginning of function call. \n
  Linux 2.6 Hook-up recommendation \n
  Must be in netfilter IPV4 or IPV6 hook PREROUTRING and POSTROUTING hooks. In nf_conntrack_in for function which is common for both IPV4 and V6 connection tracking, PPA_F_BEFORE_NAT_TRANSFORM. In ip_finish_output2(), the hook should be invoked after NAT transforms are applied at netfilter POSTROUTING hooks and at the very beginning of function call.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_session_add_fn)(PPA_BUF *skb, PPA_SESSION *p_session, uint32_t flags);
#else
extern int32_t ppa_hook_session_add_fn(PPA_BUF *skb, PPA_SESSION *p_session, uint32_t flags);
#endif
extern int32_t (*ppa_hook_session_bradd_fn)(PPA_BUF *skb, PPA_SESSION *p_session, uint32_t flags);
/*!
  \brief If skb stores a p_item, increase ref count of the p_item->used
  \param[in] skb points to the skb pointer
 */
extern void (*ppa_hook_pitem_refcnt_inc_fn)(PPA_BUF *skb);
/*!
  \brief If skb stores a p_item, call ppa_session_put
  \param[in] skb points to the skb pointer
 */
extern void (*ppa_hook_pitem_refcnt_dec_fn)(PPA_BUF *skb);

#if defined(CONFIG_PPA_MPE_IP97)
#ifdef NO_DOXY
extern int32_t (*ppa_hook_session_ipsec_add_fn)(PPA_XFRM_STATE *, sa_direction);
extern int32_t (*ppa_hook_session_ipsec_del_fn)(PPA_XFRM_STATE *);
#else
extern int32_t ppa_hook_session_ipsec_add_fn(PPA_XFRM_STATE *, sa_direction);
extern int32_t ppa_hook_session_ipsec_del_fn(PPA_XFRM_STATE *);
#endif
#endif
/*!
  \brief Del a PPA routing session entry
  \param[in] p_session Points to the connection tracking session to which this packet belongs. It maybe passed as NULL in which case PPA will try to determine it using the PPA stack adaptation layer.
  \param[in] flags Reserved currently.
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_session_del_fn)(PPA_SESSION *p_session, uint32_t flags);
#else
extern int32_t ppa_hook_session_del_fn(PPA_SESSION *p_session, uint32_t flags);
#endif
/*!
  \brief It is used to get session priority  of a ppa session
  \param[in] p_session Points to the connection tracking session to which this packet belongs. It maybe passed as NULL in which case PPA will try to determine it using the PPA stack adaptation layer.
  \param[in] flags Reserved currently.
  \return The return value is PPA session priority
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_session_prio_fn)(PPA_SESSION *p_session, uint32_t flags);
#else
extern int32_t ppa_hook_session_prio_fn(PPA_SESSION *p_session, uint32_t flags);
#endif
/*!
  \brief Returns session statistics
  \param[in]  p_session   Pointer to the session
  \param[in]  pCtCounter  Pointer to the statistics structure
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_ct_stats_fn)(PPA_SESSION *p_session, PPA_CT_COUNTER *pCtCounter);
#else
extern int32_t ppa_hook_get_ct_stats_fn(PPA_SESSION *p_session, PPA_CT_COUNTER *pCtCounter);
#endif
/*!
  \brief Checks if the "accelerated" PPA session should be timed out due to inactivity.
  \param[in]  p_session Pointer to PPA unicast or multicast session.
  \return The return value can be any one of the following:  \n
  - PPA_TIMEOUT if the PPA session inactivity timer has expired \n
  - PPA_HIT if the PPA session has been active
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_inactivity_status_fn)(PPA_U_SESSION *p_session);
#else
extern int32_t ppa_hook_inactivity_status_fn(PPA_U_SESSION *p_session);
#endif
/*!
  \brief Update the session inactivity timeout for a PPA session as per the session inactivity configuration in the protocol stack.
  \param[in] p_session Pointer to PPA unicast or multicast session.
  \param[in] timeout  Timeout value for session inactivity in  seconds.
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_inactivity_fn)(PPA_U_SESSION *p_session, int32_t timeout);
#else
extern int32_t ppa_hook_set_inactivity_fn(PPA_U_SESSION *p_session, int32_t timeout);
#endif
/*!
  \brief Add or update a MAC entry and its source ethernet port information in the PPA bridge table.
  \param[in]  mac_addr Pointer to MAC address to add to PPA bridge table.
  \param[in]  brif pointer to PPA net interface sttructure of the bridge.
  \param[in]  netif   Pointer to PPA net interface which is the source of the MAC address.
  \param[in]  flags Valid values are:
  - PPA_F_BRIDGE_LOCAL - traffic is destined for local termination.
  - PPA_F_STATIC_ENTRY - static MAC address entry in the PPA bridge table. It will not be aged out.
  - PPA_F_DROP_PACKET - firewall action. Always drop packets with this MAC destination.
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE \n
  \note Static MAC entry updates and MAC address drop filters can be configured from userspace.  For dynamic entries, the function must be hooked from bridging code where new entries are inserted into bridge mac table (or forwarding database, fdb).
  Linux 2.4 Hook-up recommendation \n
  Hook in kernel function br_fdb_insert() in net/bridge/br_fdb.c.  For Linux bridging code, the netif is given by fdb->dst->dev field where fdb points to a MAC entry.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_bridge_entry_add_fn)(uint8_t *mac_addr, PPA_NETIF *brif, PPA_NETIF *netif, uint32_t flags);
#else
extern int32_t ppa_hook_bridge_entry_add_fn(uint8_t *mac_addr, PPA_NETIF *brif, PPA_NETIF *netif, uint32_t flags);
#endif
/*!
  \brief Delete a MAC entry from PPA Bridge table since the MAC entry is aged out or administratively triggered.
  \param[in]  mac_addr Pointer to MAC address to delete from PPA bridge table.
  \param[in]  brif pointer to PPA net interface sttructure of the bridge.
  \param[in]  flags Reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_bridge_entry_delete_fn)(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t flags);
#else
extern int32_t ppa_hook_bridge_entry_delete_fn(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t flags);
#endif
/*!
  \brief Get latest packet arriving time for the specified MAC address entry. This is used for aging out decisions for the MAC entry.
  \param[in]  mac_addr Pointer to MAC address whose entry hit time is being queried
  \param[in]  brif pointer to PPA net interface sttructure of the bridge.
  \param[out] p_hit_time Provides the latest packet arriving time in seconds from system bootup.
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS \n
  - PPA_FAILURE
  \note Linux 2.4 Hook-up recommendation \n
  This API can be hooked in function br_fdb_cleanup(). In Linux, there is a kernel thread (br_fdb_cleanup) polling each entry in the MAC table and removes entries without traffic for a long time.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_bridge_entry_hit_time_fn)(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t *p_hit_time);
#else
extern int32_t ppa_hook_bridge_entry_hit_time_fn(uint8_t *mac_addr, PPA_NETIF *brif, uint32_t *p_hit_time);
#endif
#if defined(PPA_IF_MIB) && PPA_IF_MIB
/*!
  \brief Returns per interface statistics kept by the PPA which is a superset of the per Interface statistics above. This provides, for example, fastpath routed and bridged statistics.
  \param[in] ifname Pointer to the interface name
  \param[out] p_stats  Pointer to the Statistics data structure of the interface.
  \param[in] flags Reserved.
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
  \note  This function is only implemented for D4 firmware.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_netif_accel_stats_fn)(PPA_IFNAME *ifname, PPA_NETIF_ACCEL_STATS *p_stats, uint32_t flags);
#else
extern int32_t ppa_hook_get_netif_accel_stats_fn(PPA_IFNAME *ifname, PPA_NETIF_ACCEL_STATS *p_stats, uint32_t flags);
#endif
#endif
#if defined(CONFIG_PPA_API_DIRECTCONNECT) && CONFIG_PPA_API_DIRECTCONNECT
/*!
  \brief Check if network interafce like ACA is a fastpath interface
  \param[in] netif Pointer to the network interface structure in the protocol stack. For eg. pointer to a struct net_device
  \param[in] ifname Interface name
  \param[in] flags  Reserved for future use
  \return The return value can be any one of the following:  \n
  - 1 if ACA or WLAN fastpath interface \n
  - 0 otherwise
 */
#ifdef NO_DOXY
extern int32_t (*ppa_check_if_netif_fastpath_fn)(PPA_NETIF *netif, char *ifname, uint32_t flags);
#else
extern int32_t ppa_check_if_netif_fastpath_fn(PPA_NETIF *netif, char *ifname, uint32_t flags);
#endif
/*!
  \brief Deletes an WAVE500 STA session information from the PPA session list.
  \param[in] dev Pointer to the network device structure in the protocol stack. For eg. pointer to a struct netdevice
  \param[in] subif Pointer to the sub-interface structure in the datapath library.
  \param[in] mac station mac address
  \param[in] flags  Reserved for future use
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_disconn_if_fn)(PPA_NETIF *dev, PPA_DP_SUBIF *subif, uint8_t *mac, uint32_t flags);
#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
extern int32_t (*ppa_register_qos_class2prio_hook_fn)(int32_t port_id, PPA_NETIF *netif, PPA_QOS_CLASS2PRIO_CB qos_class2prio_cb, uint32_t flags);
#endif /* WMM*/
#else
extern int32_t ppa_hook_disconn_if_fn(PPA_NETIF *dev, PPA_DP_SUBIF *subif, uint8_t *mac, uint32_t flags);
#if defined(WMM_QOS_CONFIG) && WMM_QOS_CONFIG
extern int32_t ppa_register_qos_class2prio_hook_fn(int32_t port_id, PPA_NETIF *netif, PPA_QOS_CLASS2PRIO_CB qos_class2prio_cb, uint32_t flags);
#endif /* WMM*/
#endif
#endif
/*!
  \This function adds all the PPE_DIRECTPATH_DATA_ENTRY_VALID ports as Physical ports in PPA
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess
  - PPA_FAILURE on error
 */ 
extern int32_t (*ppa_directpath_port_add_fn)(void);
/*!
  \This function checks a netdevice is a directpath device or not
  \param[in] netif Pointer to the network device structure in  the protocol stack. For eg. pointer to a struct netdevice
  \param[in] flag  Directpath specific flags
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess
  - PPA_FAILURE on error
 */
extern int32_t (*ppa_check_if_netif_directpath_fn)(PPA_NETIF *netif,  uint16_t flag);
/*!
  \This function allocates phys port for network interfaces
  \param[in] ifname Name of netdevice
  \param[in] port Port to be allocated
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess
  - PPA_FAILURE on error
 */
extern int32_t (*ppa_phys_port_add_hook_fn)(PPA_IFNAME *ifname, uint32_t port);
/*!
  \This function deallocates phys port for network interfaces
  \param[in] port Port to be deallocated
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess
  - PPA_FAILURE on error
 */
extern int32_t (*ppa_phys_port_remove_hook_fn)(uint32_t port);
/*!
  \brief This function allows a device driver to register or deregister a network device to the PPA
  \param[out] if_id  PPA specific Interface Identifier. It is currently a number between 0 to 7. This Id is returned by the PPA module
  \param[in] dev Pointer to the network device structure in  the protocol stack. For eg. pointer to a struct netdevice
  \param[in] pDirectpathCb   Pointer to the DirectPath callback structure which provides the driver callbacks for rx_fn, stop_tx_fn and restart_tx_fn.
  \param[in] flags Flag to indicate if device is being registered or deregisterd. Valid values are: \n
  PPA_F_DIRECTPATH_DEREGISTER, if de-registering, zero otherwise
  PPA_F_DIRECTPATH_CORE1 if the driver for the network interface is running on Core 1 (i.e. 2nd CPU)
  PPA_F_DIRECTPATH_ETH_IF if the network interface is an Ethernet-like interface
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_register_dev_fn)(uint32_t *if_id, PPA_NETIF *dev, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_register_dev_fn(uint32_t *if_id, PPA_NETIF *dev, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#endif
/*!
  \brief This function allows a device driver to register or deregister a network device to the PPA
  \param[out] subif  PPA specific Interface Identifier. It is currently a number between 0 to 16. This Id is returned by the PPA module
  \param[in] dev Pointer to the network device structure in  the protocol stack. For eg. pointer to a struct netdevice
  \param[in] pDirectpathCb   Pointer to the DirectPath callback structure which provides the driver callbacks for rx_fn, stop_tx_fn and restart_tx_fn.
  \param[in] flags Flag to indicate if device is being registered or deregisterd. Valid values are: \n
  PPA_F_DIRECTPATH_DEREGISTER, if de-registering, zero otherwise
  PPA_F_DIRECTPATH_CORE1 if the driver for the network interface is running on Core 1 (i.e. 2nd CPU)
  PPA_F_DIRECTPATH_ETH_IF if the network interface is an Ethernet-like interface
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_ex_register_dev_fn)(PPA_SUBIF *subif, PPA_NETIF *dev, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_ex_register_dev_fn(PPA_SUBIF *subif, PPA_NETIF *dev, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#endif
/*!
  \brief This function allows a device driver to register or deregister a network device to the PPA directlink
  \param[out] if_id  PPA virtual Interface Identifier. It is currently a number between 0 to 15. This Id is returned by the PPA module
  \param[in] dtlk Pointer to the directlink structure. inside the structure, it has some elements:\n
dev: pointer to network device
vap_id:  VAP index from wifi driver
flags:  flag to indicate the actions
\param[in] pDirectpathCb   Pointer to the DirectPath callback structure which provides the driver callbacks for rx_fn, stop_tx_fn and restart_tx_fn.
\param[in] flags Flag to indicate if device is being registered or deregisterd. Valid values are: \n
PPA_F_DIRECTPATH_DEREGISTER, if de-registering, zero otherwise
PPA_F_DIRECTPATH_CORE1 if the driver for the network interface is running on Core 1 (i.e. 2nd CPU)
PPA_F_DIRECTPATH_ETH_IF if the network interface is an Ethernet-like interface
\return The return value can be any one of the following:  \n
- PPA_SUCCESS on sucess \n
- PPA_FAILURE on error
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directlink_register_dev_fn)(int32_t *if_id, PPA_DTLK_T *dtlk, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#else
extern int32_t ppa_hook_directlink_register_dev_fn(int32_t *if_id, PPA_DTLK_T *dtlk, PPA_DIRECTPATH_CB *pDirectpathCb, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to transmit a packet using the PPA DirectPath interface. The packet buffer
  passed to the function must have its packet data pointer set to the IP header with the Ethernet
  header/Link layer header etc preceding the IP header (For eg., on Linux skb->data points to IP header, and
  doing the appropriate skb_push() will allow skb->data to move backwards and point to Ethernet header).
  In other words, PPA Directpath must be able to access the full frame even though the packet buffer points to the
  IP header as required by the Linux netif_rx() function.
  \param[in] rx_if_id  Receive interface Id in the protocol stack
  \param[in] buf     Pointer to the packet buffer structure of the stack for the packet which is to be transmitted
  \param[in] len     Size of packet in bytes
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
  \note The DirectPath Tx API can have internal shortcut path to the destination or fallback to passing the packet to the
  protocol stack. The aim is to insulate the device driver calling the API from such details. \n
  For Linux, the driver must call this function through the hook pointer where it passes packets to the network stack by calling the netif_rx() or netif_receive_skb() functions. \n
Note : The CPU-bound device driver is strongly recommended to call this API from tasklet mode (or equivalent non-interrupt context on non-Linux OS) and not from IRQ context for better system dynamics.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_send_fn)(uint32_t rx_if_id, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_send_fn(uint32_t rx_if_id, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to transmit a packet using the PPA DirectPath interface. The packet buffer
  passed to the function must have its packet data pointer set to the IP header with the Ethernet
  header/Link layer header etc preceding the IP header (For eg., on Linux skb->data points to IP header, and
  doing the appropriate skb_push() will allow skb->data to move backwards and point to Ethernet header).
  In other words, PPA Directpath must be able to access the full frame even though the packet buffer points to the
  IP header as required by the Linux netif_rx() function.
  \param[in] subif  Receive interface Id and sub interface id
  \param[in] buf     Pointer to the packet buffer structure of the stack for the packet which is to be transmitted
  \param[in] len     Size of packet in bytes
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess \n
  - PPA_FAILURE on error
  \note The DirectPath Tx API can have internal shortcut path to the destination or fallback to passing the packet to the
  protocol stack. The aim is to insulate the device driver calling the API from such details. \n
  For Linux, the driver must call this function through the hook pointer where it passes packets to the network stack by calling the netif_rx() or netif_receive_skb() functions. \n
Note : The CPU-bound device driver is strongly recommended to call this API from tasklet mode (or equivalent non-interrupt context on non-Linux OS) and not from IRQ context for better system dynamics.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_ex_send_fn)(PPA_SUBIF *subif, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_ex_send_fn(PPA_SUBIF *subif, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to indicate to the PPA that it cannot receive any further packets from the
  latter. The device driver can call this function for flow control.
  \param[in] if_id  interface Id for which to apply flow control
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess\n
  - PPA_FAILURE on fail\n
  - PPA_EINVAL if the interface not exist
  \note It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet processing.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_rx_stop_fn)(uint32_t if_id, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_rx_stop_fn(uint32_t if_id, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to indicate to the PPA that it cannot receive any further packets from the
  latter. The device driver can call this function for flow control.
  \param[in] subif  Receive interface Id and sub interface id
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess\n
  - PPA_FAILURE on fail\n
  - PPA_EINVAL if the interface not exist
  \note It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet processing.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_ex_rx_stop_fn)(PPA_SUBIF *subif, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_ex_rx_stop_fn(PPA_SUBIF *subif, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to indicate to the PPA that it can again receive packets from the latter. The
  device driver can call this function for flow control after it has called the Rx Stop function to halt supply of packets
  from the PPA.
  \param[in] if_id  interface Id for which the driver requests the flow control action to restart transmission.
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess\n
  - PPA_FAILURE on fail\n
  - PPA_EINVAL if the interface not exist
  \note   It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet processing. This function must be used in conjunction with the PPA_FP_STOP_TX_FN.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_rx_restart_fn)(uint32_t if_id, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_rx_restart_fn(uint32_t if_id, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to indicate to the PPA that it can again receive packets from the latter. The
  device driver can call this function for flow control after it has called the Rx Stop function to halt supply of packets
  from the PPA.
  \param[in] subif  Receive interface Id and sub interface id
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess\n
  - PPA_FAILURE on fail\n
  - PPA_EINVAL if the interface not exist
  \note   It is recommended for a device driver to use the PPA DirectPath flow control functions for efficient packet processing. This function must be used in conjunction with the PPA_FP_STOP_TX_FN.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_ex_rx_restart_fn)(PPA_SUBIF *subif, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_ex_rx_restart_fn(PPA_SUBIF *subif, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to allocate the skb buffer.
  \param[in] subif  Receive interface Id and sub interface id
  \param[in] len    Length of the skb buffer
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - Not-null PPA_SKBUF pointer on sucess\n
  - NULL or (void *)PPA_FAILURE or (void *)PPA_EINVAL on fail\n
  \note      It is recommended for a device driver to recycle this buffer using ppa_hook_directpath_recycle_skb_fn function.
 */
#ifdef NO_DOXY
extern PPA_SKBUF * (*ppa_hook_directpath_alloc_skb_fn)(PPA_SUBIF *psubif, int32_t len, uint32_t flags);
#else
extern PPA_SKBUF *ppa_hook_directpath_alloc_skb_fn(PPA_SUBIF *psubif, int32_t len, uint32_t flags);
#endif
/*!
  \brief This function allows the device driver to de-allocate the skb buffer.
  \param[in] subif  Receive interface Id and sub interface id
  \param[in] skb    Allocated skb pointer
  \param[in] flags  reserved
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on sucess\n
  - PPA_FAILURE on fail\n
  - PPA_EINVAL if the interface not exist
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_recycle_skb_fn)(PPA_SUBIF *psubif, PPA_SKBUF *skb, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_recycle_skb_fn(PPA_SUBIF *psubif, PPA_SKBUF *skb, uint32_t flags);
#endif
/*!
  \brief This function maps the Protocol stack interface structure to the PPA Interface Id.
  \param[in] netif Pointer to the protocol stack network interface structure for the device
  \return The return value can be any one of the following: \n
  - PPA Interface Identifier, if_id \n
  - PPA_FAILURE
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_ifid_for_netif_fn)(PPA_NETIF *netif);
#else
extern int32_t ppa_hook_get_ifid_for_netif_fn(PPA_NETIF *netif);
#endif
#ifdef CONFIG_PPA_QOS
/*!
  \brief This is to get the maximum queue number supported on specified port
  \param[in] portid the physical port id which support qos queue
  \param[in] flag reserved for future
  \return returns the queue number supported on this port.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_qos_qnum)(uint32_t portid, uint32_t flag);
#else
extern int32_t ppa_hook_get_qos_qnum(uint32_t portid, uint32_t flag);
#endif
/*!
  \brief This is to get the mib counter on specified port and queueid
  \param[in] portid the physical port id which support qos queue
  \param[in] queueid the queue id for the mib
  \param[out] mib the buffer to store qos mib
  \param[in] flag reserved for future
  \return returns the queue number supported on this port.
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_qos_mib)(uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
#else
extern int32_t ppa_hook_get_qos_mib(uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
#endif
#ifdef CONFIG_PPA_QOS_RATE_SHAPING
/*!
  \brief This is to eanble/disable Rate Shaping feature
  \param[in] portid the phisical port id which support qos queue
  \param[in] enable 1:enable 0: disable
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_ctrl_qos_rate)(uint32_t portid, uint32_t enable, uint32_t flag);
#else
extern int32_t ppa_hook_set_ctrl_qos_rate(uint32_t portid, uint32_t enable, uint32_t flag);
#endif
/*!
  \brief This is to get Rate Shaping feature status: enabled or disabled
  \param[in] portid the phisical port id which support qos queue
  \param[out] enable buffer to store status: 1:enable 0: disable
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_ctrl_qos_rate)(uint32_t portid, uint32_t *enable, uint32_t flag);
#else
extern int32_t ppa_hook_get_ctrl_qos_rate(uint32_t portid, uint32_t *enable, uint32_t flag);
#endif
/*!
  \brief This is to set Rate Shaping for one specified port and queue
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set rate shaping
  \param[in] rate the maximum rate limit in kbps
  \param[in] burst the maximun burst in bytes
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t rate, uint32_t burst, uint32_t flag);
#else
extern int32_t ppa_hook_set_qos_rate(uint32_t portid, uint32_t queueid, uint32_t rate, uint32_t burst, uint32_t flag);
#endif
/*!
  \brief This is to get Rate Shaping settings for one specified port and queue
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set rate shaping
  \param[out] rate the maximum rate limit in kbps
  \param[out] burst the maximun burst in bytes
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t *rate, uint32_t *burst, uint32_t flag);
#else
extern int32_t ppa_hook_get_qos_rate(uint32_t portid, uint32_t queueid, uint32_t *rate, uint32_t *burst, uint32_t flag);
#endif
/*!
  \brief This is to reset Rate Shaping for one specified port and queue (
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set rate shaping
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_reset_qos_rate)(uint32_t portid, uint32_t queueid, uint32_t flag);
#else
extern int32_t ppa_hook_reset_qos_rate(uint32_t portid, uint32_t queueid, uint32_t flag);
#endif
#endif /*end of CONFIG_PPA_QOS_RATE_SHAPING*/
#ifdef CONFIG_PPA_QOS_WFQ
/*!
  \brief This is to eanble/disable WFQ feature
  \param[in] portid the phisical port id which support qos queue
  \param[in] enable 1:enable 0: disable
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_ctrl_qos_wfq)(uint32_t portid, uint32_t enable, uint32_t flag);
#else
extern int32_t ppa_hook_set_ctrl_qos_wfq(uint32_t portid, uint32_t enable, uint32_t flag);
#endif
/*!
  \brief This is to get WFQ feature status: enabled or disabled
  \param[in] portid the phisical port id which support qos queue
  \param[out] enable 1:enable 0: disable
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_ctrl_qos_wfq)(uint32_t portid, uint32_t *enable, uint32_t flag);
#else
extern int32_t ppa_hook_get_ctrl_qos_wfq(uint32_t portid, uint32_t *enable, uint32_t flag);
#endif
/*!
  \brief This is to set WFQ weight level for one specified port and queue
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set WFQ
  \param[out] weight_level the value should be 0 ~ 100.
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag);
#else
extern int32_t ppa_hook_set_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t weight_level, uint32_t flag);
#endif
/*!
  \brief This is to get WFQ settings for one specified port and queue (default value should be 0xFFFF)
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set WFQ
  \param[out] weight_level the value should be 0 ~ 100.
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
#else
extern int32_t ppa_hook_get_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t *weight_level, uint32_t flag);
#endif
/*!
  \brief This is to reset WFQ for one specified port and queue (default value should be 0xFFFF)
  \param[in] portid the phisical port id which support qos queue
  \param[in] queueid the queue id need to set WFQ
  \param[in] flag reserved for future
  \return The return value can be any one of the following:  \n
  - PPA_SUCCESS on success \n
  - PPA_FAILURE on error \n
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_reset_qos_wfq)(uint32_t portid, uint32_t queueid, uint32_t flag);
#else
extern int32_t ppa_hook_reset_qos_wfq(uint32_t portid, uint32_t queueid, uint32_t flag);
#endif
#endif /*end of CONFIG_PPA_QOS_WFQ*/
#endif /*end of CONFIG_PPA_QOS*/
/*#ifdef CONFIG_PPA_GRE*/
extern uint32_t (*ppa_is_ipv4_gretap_fn)(struct net_device *dev);
extern uint32_t (*ppa_is_ipv6_gretap_fn)(struct net_device *dev);
/*#endif*/
extern int32_t (*ppa_ppp_get_chan_info_fn)(struct net_device *ppp_dev, struct ppp_channel **chan);
extern int32_t (*ppa_check_pppoe_addr_valid_fn)(struct net_device *dev, struct pppoe_addr *pa);
extern int32_t (*ppa_get_pppoa_info_fn)(struct net_device *dev, void *pvcc, uint32_t pppoa_id, void *value);
extern int32_t (*ppa_get_pppol2tp_info_fn)(struct net_device *dev, void *po, uint32_t pppol2tp_id, void *value);
extern int32_t (*ppa_if_is_ipoa_fn)(struct net_device *netdev, char *ifname);
extern int32_t (*ppa_if_is_br2684_fn)(struct net_device *netdev, char *ifname);
extern int32_t (*ppa_br2684_get_vcc_fn)(struct net_device *netdev, struct atm_vcc **pvcc);
extern int32_t (*ppa_if_ops_veth_xmit_fn)(struct net_device *dev);
/*!
  \brief This is to enqueue skb to linux imq for rateshaping
  \param[in] skb skb buffer
  \param[in] portID directpath port id
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_enqueue_to_imq_fn)(PPA_SKBUF *skb, uint32_t portID);
#else
extern int32_t ppa_hook_directpath_enqueue_to_imq_fn(PPA_SKBUF *skb, uint32_t portID);
#endif
/*!
  \brief This is to reinject skb from linux imq for rateshaping
  \param[in] rx_if_id Recive interface id
  \param[in] buf skb buffer
  \param[in] len
  \param[in] flags
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_directpath_reinject_from_imq_fn)(int32_t rx_if_id, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#else
extern int32_t ppa_hook_directpath_reinject_from_imq_fn(int32_t rx_if_id, PPA_SKBUF *buf, int32_t len, uint32_t flags);
#endif
/*!
  \brief This is to set the lan separation flag
  \param[in] flag Lan separation flag
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t(*ppa_hook_set_lan_seperate_flag_fn)(uint32_t flag);
#else
extern int32_t ppa_hook_set_lan_seperate_flag_fn(uint32_t flag);
#endif
/*!
  \brief This is to set the wan separation flag
  \param[in] flag Wan separation flag
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t(*ppa_hook_set_wan_seperate_flag_fn)(uint32_t flag);
#else
extern int32_t ppa_hook_set_wan_seperate_flag_fn (uint32_t flag);
#endif
/*!
  \brief This is to get the lan separation flag
  \param[in] flag Lan separation flag
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern uint32_t (*ppa_hook_get_lan_seperate_flag_fn)(uint32_t flag);
#else
extern uint32_t ppa_hook_get_lan_seperate_flag_fn(uint32_t flag);
#endif
/*!
  \brief This is to get the wan separation flag
  \param[in] flag Wan separation flag
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern uint32_t (*ppa_hook_get_wan_seperate_flag_fn)(uint32_t flag);
#else
extern uint32_t ppa_hook_get_wan_seperate_flag_fn(uint32_t flag);
#endif
#if defined(CONFIG_PPA_API_SW_FASTPATH)
/*!
  \brief This is to send skb through software fastpath
  \param[in] skb skb buffer
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_sw_fastpath_send_fn)(struct sk_buff *skb);
#else
extern int32_t ppa_hook_sw_fastpath_send_fn(struct sk_buff *skb);
#endif
/*!
  \brief This is to enable software fastpath
  \param[in] flags for software fp enable disable
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_set_sw_fastpath_enable_fn)(uint32_t flags);
#else
extern int32_t ppa_hook_set_sw_fastpath_enable_fn(uint32_t *flags);
#endif
/*!
  \brief This is to get software fastpath status
  \param[in] flags for software fp enable disable
  \return int32_t, The return value can be 0 or 1
 */
#ifdef NO_DOXY
extern int32_t (*ppa_hook_get_sw_fastpath_status_fn)(uint32_t flags);
#else
extern int32_t ppa_hook_get_sw_fastpath_status_fn(uint32_t *flags);
#endif
#endif
#if defined(CONFIG_L2NAT_MODULE) || defined(CONFIG_L2NAT)
/*!
  \brief Check if network interafce is a part of l2nat
  \param[in] netif Pointer to the network interface structure in the protocol stack. For eg. pointer to a struct net_device
  \param[in] ifname Interface name
  \param[in] flags  Reserved for future use
  \return The return value can be any one of the following:  \n
  - 1 if network interafce is a part of l2nat \n
  - 0 otherwise
 */
#ifdef NO_DOXY
extern int32_t (*ppa_check_if_netif_l2nat_fn)(PPA_NETIF *netif, char *ifname, uint32_t flags);
#else
extern int32_t ppa_check_if_netif_l2nat_fn(PPA_NETIF *netif, char *ifname, uint32_t flags);
#endif
#endif /* CONFIG_L2NAT*/
#ifdef NO_DOXY
extern struct net_device* (*ppa_get_bond_xmit_xor_intrf_hook)(struct sk_buff *skb, struct net_device *bond_dev);
#else
extern struct net_device* ppa_get_bond_xmit_xor_intrf_hook(struct sk_buff *skb, struct net_device *bond_dev);
#endif
#endif /*end of __KERNEL__*/
/* @} */
#endif  /*  __PPA_HOOK_H__20081103_1736__*/
