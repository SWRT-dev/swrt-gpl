/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_IRB_H__
#define __MTLK_IRB_H__

#include "mtlk_osal.h"
#include "mtlkhash.h"
#include "mtlkguid.h"
#include "mtlkstartup.h"

#include "mtlkirb_osdep.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

void __MTLK_IFUNC _mtlk_irb_call_handler(mtlk_handle_t      evt_contex, 
                                         void              *handler, 
                                         mtlk_handle_t      handler_contex, 
                                         const mtlk_guid_t *evt,
                                         void              *buffer,
                                         uint32            *size);

struct mtlk_irb_hash_private
{
  mtlk_mhash_t      hash;
  mtlk_atomic_t     owner_id;
  mtlk_osal_mutex_t lock;
  MTLK_DECLARE_INIT_STATUS;
};

typedef struct mtlk_irb_hash_private mtlk_irb_hash_t;

/************************************************************************
 * IRB HASH - CP, but for IRBD/A internal usage only
 ************************************************************************/

int           __MTLK_IFUNC _mtlk_irb_hash_init(mtlk_irb_hash_t *irb_hash);
void          __MTLK_IFUNC _mtlk_irb_hash_cleanup(mtlk_irb_hash_t *irb_hash);

mtlk_handle_t __MTLK_IFUNC _mtlk_irb_hash_register(mtlk_irb_hash_t   *irb_hash,
                                                   const mtlk_guid_t *evts,
                                                   uint32             nof_evts,
                                                   void              *handler,
                                                   mtlk_handle_t      context);
void          __MTLK_IFUNC _mtlk_irb_hash_unregister(mtlk_irb_hash_t *irb_hash, 
                                                     mtlk_handle_t    owner);

void          __MTLK_IFUNC _mtlk_irb_hash_on_evt(mtlk_irb_hash_t   *irb_hash,
                                                 const mtlk_guid_t *evt,
                                                 void              *buffer,
                                                 uint32            *size,
                                                 mtlk_handle_t      contex);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRB_H__ */
