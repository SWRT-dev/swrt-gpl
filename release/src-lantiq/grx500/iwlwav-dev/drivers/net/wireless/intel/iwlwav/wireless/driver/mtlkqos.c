/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id:
 *
 * 
 *
 * Shared QoS
 *
 */
#include "mtlkinc.h"

#include "mtlkqos.h"
#include "mtlk_packets.h"
#include "mtlk_core_iface.h"
#include "mtlk_coreui.h"
#include "mtlk_df_nbuf.h"
#include "mtlk_param_db.h"

#define LOG_LOCAL_GID   GID_QOS
#define LOG_LOCAL_FID   1


static __INLINE int
_mtlk_qos_get_priority_from_dscp (const uint8 *dscp_table, mtlk_nbuf_t* nbuf, uint16* ppriority)
{
  uint8 dscp_field, dscp_value;
  uint8 ether_type_and_tos[4];
  uint8 *p_type_tos = nbuf->data + (MTLK_ETH_HDR_SIZE - MTLK_ETH_SIZEOF_ETHERNET_TYPE);

  ASSERT (nbuf != NULL);  
  ASSERT (nbuf->len >= MTLK_ETH_HDR_SIZE);

  ether_type_and_tos[0] = *p_type_tos++;
  ether_type_and_tos[1] = *p_type_tos++;
  ether_type_and_tos[2] = *p_type_tos++;
  ether_type_and_tos[3] = *p_type_tos++;

  switch (NET_TO_HOST16(*(u16*)ether_type_and_tos))
  {
  case MTLK_ETH_DGRAM_TYPE_IP:
    dscp_field = get_ip4_tos(ether_type_and_tos + MTLK_ETH_SIZEOF_ETHERNET_TYPE);
    break;
  case MTLK_ETH_DGRAM_TYPE_IPV6:
    dscp_field = get_ip6_tos(ether_type_and_tos + MTLK_ETH_SIZEOF_ETHERNET_TYPE);
    break;
  default:
    //Unknown protocol
    *ppriority = MTLK_WMM_ACI_DEFAULT_CLASS;
    return MTLK_ERR_OK;
  }

  dscp_value = dscp_field >> 2;
  *ppriority = dscp_table[dscp_value];

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_qos_adjust_priority(uint8 *dscp_table, mtlk_nbuf_t *nbuf)
{
  uint16 priority;

  /* Get priority from DSCP bits (IP priority) */
  _mtlk_qos_get_priority_from_dscp(dscp_table, nbuf, &priority);

  /* update packet's priority */
  mtlk_df_nbuf_set_priority(nbuf, priority);
}

void __MTLK_IFUNC mtlk_qos_dscp_table_init (uint8 *dscp_table)
{
  uint32 up;

  for (up = 0; up < NTS_TIDS; up++)
  {
    memset(&dscp_table[up * DSCP_PER_UP], up, DSCP_PER_UP);
  }
}
