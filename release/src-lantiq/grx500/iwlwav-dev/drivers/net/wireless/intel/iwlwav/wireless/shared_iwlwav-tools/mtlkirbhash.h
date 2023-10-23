/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*! \file   mtlkirbhash.h
 *  \brief  IRB HASH APIs, for IRBD/IRBA internal usage only
 */

#ifndef __MTLK_IRB_H__
#define __MTLK_IRB_H__

#include "mtlk_osal.h"
#include "mtlkhash.h"
#include "mtlkguid.h"
#include "mtlkstartup.h"

#include "mtlkirb_osdep.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define IRB_MAX_NOF_EVTS (16)

struct mtlk_irb_hash_private
{
  mtlk_mhash_t      hash;
  mtlk_atomic_t     owner_id;
  mtlk_osal_mutex_t lock;
  MTLK_DECLARE_INIT_STATUS;
};

typedef struct mtlk_irb_hash_private mtlk_irb_hash_t;

/*! \brief      Initializing an IRB HASH Object
 *
 *  \param[in]  irb_hash  Pointer to the IRB HASH Object
 *
 *  \return     error code on failure, MTLK_ERR_OK on success
 */
int           __MTLK_IFUNC _mtlk_irb_hash_init(mtlk_irb_hash_t *irb_hash);

/*! \brief      Cleanup of an IRB HASH Object
 *
 *  \param[in]  irb_hash  Pointer to the IRB HASH Object
 *
 *  \return     none
 */
void          __MTLK_IFUNC _mtlk_irb_hash_cleanup(mtlk_irb_hash_t *irb_hash);

/*! \brief      Registers an event(s) handler with a specific IRBD hash.
 *              The handler will be called on a desired event(s) arrival.
 *
 *  \param[in]  irb_hash  Pointer to the IRB HASH Object
 *  \param[in]  evts      Array of desired events to register
 *  \param[in]  nof_evts  Number of events in \a evts array
 *  \param[in]  handler   Handler function to be called once a desired event arrives
 *  \param[in]  context   Context to be passed to handler function
 *
 *  \return     Registered IRB owner's handle
 */
mtlk_handle_t __MTLK_IFUNC _mtlk_irb_hash_register(mtlk_irb_hash_t   *irb_hash,
                                                   const mtlk_guid_t *evts,
                                                   uint32             nof_evts,
                                                   void              *handler,
                                                   mtlk_handle_t      context);

/*! \brief      Unregisters an previously registered event(s) handler
 *
 *  \param[in]  irb_hash  Pointer to the IRB HASH Object
 *  \param[in]  owner     IRB owner's handle to unregister
 *
 *  \return     none
 */
void          __MTLK_IFUNC _mtlk_irb_hash_unregister(mtlk_irb_hash_t *irb_hash, 
                                                     mtlk_handle_t    owner);

/*! \brief          Handle to IRB event(s) received
 *
 *  \param[in]      irb_hash  Pointer to the IRB HASH Object
 *  \param[in]      evt       Pointer to IRB event received
 *  \param[in]      buffer    Pointer to data buffer passed to handler function
 *  \param[in,out]  size      Pointer to size of data in bytes
 *                             - As input, size of data passed to handler function
 *                             - As output, size of data received from handler function
 *
 *  \return     none
 */
void          __MTLK_IFUNC _mtlk_irb_hash_on_evt(mtlk_irb_hash_t   *irb_hash,
                                                 const mtlk_guid_t *evt,
                                                 void              *buffer,
                                                 uint32            *size,
                                                 mtlk_handle_t      contex);

/*! \brief          Call the registered handler function on IRB event received
 *
 *  \param[in]      evt_contex  IRB object
 *  \param[in]      handler     Pointer to the handler function
 *  \param[in]      context     Context passed to the handler
 *  \param[in]      evt         Pointer to IRB event received
 *  \param[in]      buffer      Pointer to data buffer passed to handler function
 *  \param[in,out]  size        Pointer to size of data in bytes
 *                               - As input, size of data passed to handler function
 *                               - As output, size of data received from handler function
 *
 *  \return     none
 */
void __MTLK_IFUNC _mtlk_irb_call_handler(mtlk_handle_t      evt_contex,
                                         void              *handler,
                                         mtlk_handle_t      handler_contex,
                                         const mtlk_guid_t *evt,
                                         void              *buffer,
                                         uint32            *size);


#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRB_H__ */
