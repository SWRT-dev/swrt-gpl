/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*****************************************************************************/
/*! \file       mtlk_hs20_api.h
 *
 * \brief Hotspot 2.0 PPF interface.
 * Modules performing ARP proxy, L2 firewall and Disable DGAF functions
 * will register with us using this API.
 *
 * Public header file to be used by packet processing kernel modules.
 *
 */


#ifndef __MTLK_HS20_API_H__
#define __MTLK_HS20_API_H__

#define WAVE_WIFI_PARP_STA_ADD (1)
#define WAVE_WIFI_PARP_STA_DEL (2)

typedef int (*wave_wifi_parp_if_manage_t)(
  int op,
  unsigned char *if_name
);

typedef int (*wave_wifi_parp_client_manage_t)(
  int op,
  unsigned char *if_name,
  unsigned char *sta_mac,
  unsigned char ip_type,
  unsigned char *sta_ip
);

typedef int (*wave_wifi_parp_lookup_t)(
  unsigned char *sta_mac,
  unsigned char ip_type,
  unsigned char *sta_ip
);

typedef int (*wave_wifi_parp_dump_t)(
  unsigned char *buffer,
  int len
);

#define WAVE_WIFI_REG   (1)
#define WAVE_WIFI_UNREG (0)

/***********************************************************************/
/*!
 * \brief Enable/disable PARP (Proxy ARP) on an interface
 * \param[in]  p_wave_wifi_parp_if_manage - Module callback for interface management
 * \param[in]  p_wave_wifi_parp_client_manage - Module callback for client management
 * \param[in]  p_wave_wifi_parp_lookup Module callback for address lookup
 * \param[in]  p_wave_wifi_parp_dump Module callback for table PARP dump
 * \param[in]  flag - Action flag, register\deregister PARP module to the wireless driver
 *
 * \return Linux system error code
 */
int mtlk_parp_register(
  wave_wifi_parp_if_manage_t     p_wave_wifi_parp_if_manage,
  wave_wifi_parp_client_manage_t p_wave_wifi_parp_client_manage,
  wave_wifi_parp_lookup_t        p_wave_wifi_parp_lookup,
  wave_wifi_parp_dump_t          p_wave_wifi_parp_dump,
  int                            flag
);

typedef int (*wave_wifi_l2f_xmit_t)(
  struct sk_buff *skb,
  struct net_device *dev
);

typedef int (*wave_wifi_ppa_l2f_xmit_t)(
  struct net_device *rx_dev,
  struct net_device *tx_dev,
  struct sk_buff *skb,
  int len
);

/***********************************************************************/
/*!
 * \brief Enable/disable HS20 (Hot Spot 2.0) on an interface
 * \param[in]  p_wave_wifi_l2f_xmit - Module hook on WLAN TX (kernel) path
 * \param[in]  p_wave_wifi_ppa_l2f_xmit - Module hook on WLAN TX (PPA) path
 * \param[in]  flag - Action flag, register\deregister HS20 module to the wireless driver
 * \return Linux system error code
 */
int mtlk_hs2_register(
  wave_wifi_l2f_xmit_t     p_wave_wifi_l2f_xmit,
  wave_wifi_ppa_l2f_xmit_t p_wave_wifi_ppa_l2f_xmit,
  int                      flag
);

/************************************************************************/
/*! \brief A callback for ndo_start_xmit of linux netdevice API.
 * Called when a packet needs to be transmitted.
 * \param[in] skb - skbuff to transmit
 * \param[in] ndev - TX net device
 * \return Returns NETDEV_TX_OK.
 */
int _mtlk_df_user_linux_start_tx(struct sk_buff *skb, struct net_device *dev);

/***********************************************************************/
/*! \brief Transmit packet on an interface
 *  \param[in]  skb - skbuff to transmit
 *  \param[in]  dev - TX net device
 *  \return     netdev_tx_t value
 */
int _mtlk_df_user_start_tx(struct sk_buff *skb, struct net_device *dev);

/***********************************************************************/
/*! \brief Transmit packet on an interface
 *  \param[in]  rx_dev - RX net device
 *  \param[in]  tx_dev - TX net device
 *  \param[in]  skb - pointer to sk_buff of the packet
 *  \param[in]  len Length of the packet
 *  \return Linux system error code
 *  \note The actual transmission will be done using LitePath,
 *  PPA prefix is left only for compatibility with old names.
 */
int _mtlk_df_user_ppa_start_xmit (struct net_device *rx_dev,
                                  struct net_device *tx_dev,
                                  struct sk_buff *skb,
                                  int len);

#endif /* __MTLK_HS20_API_H__ */
