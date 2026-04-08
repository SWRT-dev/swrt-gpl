/******************************************************************************

                               Copyright (c) 2014
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 *
 * Hotspot 2.0 PPF interface.
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

int mtlk_hs2_register(
  wave_wifi_l2f_xmit_t     p_wave_wifi_l2f_xmit,
  wave_wifi_ppa_l2f_xmit_t p_wave_wifi_ppa_l2f_xmit,
  int                      flag
);

int _mtlk_df_user_linux_start_tx(struct sk_buff *skb, struct net_device *dev);
int _mtlk_df_user_start_tx(struct sk_buff *skb, struct net_device *dev);
int _mtlk_df_user_ppa_start_xmit (struct net_device *rx_dev,
                                  struct net_device *tx_dev,
                                  struct sk_buff *skb,
                                  int len);

#endif /* __MTLK_HS20_API_H__ */
