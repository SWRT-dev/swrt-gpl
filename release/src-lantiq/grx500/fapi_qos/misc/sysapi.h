/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "commondefs.h"
int32_t sysapi_iface_rate_get(char *ifname,iftype_t iftype, uint32_t *rate);
int32_t sysapi_set_ppefp(char *ifname, iftype_t iftype, uint32_t enable);
int32_t sysapi_set_pkts_redirection(char *from, char *to, uint32_t enable);
int32_t sysapi_no_of_lan_ports(void);
int32_t qos_call_ioctl(const char *dev_node, int ioctl_cmd, void *data);
#define MAX_IF_RATE		1000000 /* 1000Mbps */
