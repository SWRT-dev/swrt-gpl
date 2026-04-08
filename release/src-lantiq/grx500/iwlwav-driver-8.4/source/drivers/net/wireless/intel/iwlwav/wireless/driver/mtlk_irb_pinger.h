/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_IRB_PINGER_H__
#define __MTLK_IRB_PINGER_H__

#include "mtlk_osal.h"
#include "mtlkstartup.h"
#include "mtlkirbd.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

struct mtlk_irb_pinger_stats
{
  uint64 peak_delay;
  uint64 all_delay;
  uint32 nof_recvd_pongs;
  uint32 nof_missed_pongs;
  uint32 nof_ooo_pongs;
};

typedef struct _mtlk_irb_pinger_t
{
  uint32                       ping_period_ms; /* also used as is_started flag */
  mtlk_irbd_handle_t           *irb_handle;
  mtlk_osal_timer_t            timer;
  mtlk_osal_spinlock_t         lock;
  uint32                       id;
  struct mtlk_irb_pinger_stats stats;
  uint32                       last_pong_id;
  mtlk_irbd_t                  *irbd;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
} __MTLK_IDATA mtlk_irb_pinger_t;

int __MTLK_IFUNC
mtlk_irb_pinger_init(mtlk_irb_pinger_t *pinger, mtlk_irbd_t *irbd);
int __MTLK_IFUNC
mtlk_irb_pinger_start(mtlk_irb_pinger_t *pinger, uint32 ping_period_ms);
void __MTLK_IFUNC
mtlk_irb_pinger_stop(mtlk_irb_pinger_t *pinger);
void __MTLK_IFUNC
mtlk_irb_pinger_cleanup(mtlk_irb_pinger_t *pinger);

BOOL __MTLK_IFUNC
mtlk_irb_pinger_is_started(mtlk_irb_pinger_t *pinger);
uint32 __MTLK_IFUNC
mtlk_irb_pinger_get_ping_period_ms (mtlk_irb_pinger_t *pinger);

#ifdef CPTCFG_IWLWAV_IRB_DEBUG
int __MTLK_IFUNC
mtlk_irb_pinger_restart(mtlk_irb_pinger_t *pinger, uint32 ping_period_ms);
#endif

void __MTLK_IFUNC
mtlk_irb_pinger_get_stats(mtlk_irb_pinger_t *pinger, struct mtlk_irb_pinger_stats *stats);
void __MTLK_IFUNC
mtlk_irb_pinger_zero_stats(mtlk_irb_pinger_t *pinger);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRB_PINGER_H__ */
