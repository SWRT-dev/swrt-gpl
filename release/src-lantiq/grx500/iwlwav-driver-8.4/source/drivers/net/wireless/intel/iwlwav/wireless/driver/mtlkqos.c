/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

/*************************************************************************************************
 * QOS local definitions
 *************************************************************************************************/

static mtlk_qos_do_classify_f do_classify = NULL;

/*****************************************************************************
**
** NAME         video_classifier_register / video_classifier_unregister
**
** DESCRIPTION  This functions are used for registration of Metalink's
**              video classifier module
**
******************************************************************************/
int mtlk_qos_classifier_register (mtlk_qos_do_classify_f classify_fn)
{
  do_classify = classify_fn;
  return MTLK_ERR_OK;
}

void mtlk_qos_classifier_unregister (void)
{
  do_classify = NULL;
}

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

  /* try to classify packet with external classifier, override default
   * skb's priority in case of success */
  if (do_classify != NULL) {
    (*do_classify)(nbuf);
    /* try packet's priority assigned by OS */
    priority = mtlk_df_nbuf_get_priority(nbuf);
  } else {
    /* Get priority from DSCP bits (IP priority) */
    _mtlk_qos_get_priority_from_dscp(dscp_table, nbuf, &priority);
  }

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
