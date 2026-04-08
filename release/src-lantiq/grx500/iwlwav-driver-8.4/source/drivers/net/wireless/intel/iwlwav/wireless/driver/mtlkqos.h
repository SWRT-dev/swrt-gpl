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

#ifndef _MTLK_QOS_H
#define _MTLK_QOS_H

#include "mtlk_osal.h"
#include "mtlkerr.h"
#include "mtlkdfdefs.h"
#include "mhi_umi.h"

#define LOG_LOCAL_GID   GID_QOS
#define LOG_LOCAL_FID   0

#define MTLK_WMM_ACI_DEFAULT_CLASS  (0)
#define NTS_TIDS                    (8)
#define NTS_TIDS_GEN6               (16)
#define NTS_TID_USE_DEFAULT         (~0)
#define DSCP_NUM                    (64)
#define DSCP_PER_UP                 (8)

typedef int (* mtlk_qos_do_classify_f)(mtlk_nbuf_t *skb);

struct _mtlk_vap_handle_t;
void __MTLK_IFUNC mtlk_qos_adjust_priority(uint8 *dscp_table, mtlk_nbuf_t *nbuf) __MTLK_INT_HANDLER_SECTION;
int  __MTLK_IFUNC mtlk_qos_classifier_register (mtlk_qos_do_classify_f classify_fn);
void __MTLK_IFUNC mtlk_qos_classifier_unregister (void);
void __MTLK_IFUNC mtlk_qos_dscp_table_init (uint8 *dscp_table);

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif // _MTLK_QOS_H
