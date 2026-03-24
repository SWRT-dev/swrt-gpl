/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_IRBD_H__
#define __MTLK_IRBD_H__

#include "mtlkguid.h"

/*! \file  mtlkirbd.h
    \brief IRB Driver part

    The part of IRB API that available in driver (i.e. master-part).
*/

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \typedef struct mtlk_irbd_private mtlk_irbd_t
    \brief   "Black box" Driver IRB object. 
*/
typedef struct mtlk_irbd_private mtlk_irbd_t;

/*! \typedef struct mtlk_irbd_handle_private mtlk_irbd_handle_t
    \brief   "Black box" IRB's client object. 
*/
typedef struct mtlk_irbd_handle_private mtlk_irbd_handle_t;

/*! \typedef void (*mtlk_irbd_evt_handler_f)(mtlk_irbd_t       *irbd,
                                             mtlk_handle_t      context,
                                             const mtlk_guid_t *evt,
                                             void              *buffer,
                                             uint32            *size)
    \brief   IRB event handler function prototype.

    This function could be registered as an event handler using the is mtlk_irbd_register API.
    Then it will be called on a desired event(s) arrival.

    \param   irbd    IRBD object that received the event.
    \param   context user context passed to mtlk_irbd_register.
    \param   evt     Arrived event ID (GUID).
    \param   buffer  Buffer arived with the event. 
    \param   size    Size of the buffer.
*/
typedef void (__MTLK_IFUNC * mtlk_irbd_evt_handler_f)(mtlk_irbd_t       *irbd,
                                                      mtlk_handle_t      context,
                                                      const mtlk_guid_t *evt,
                                                      void              *buffer,
                                                      uint32            *size);

/*! \fn      int mtlk_irbd_root_init(void)
    \brief   Initiates driver-wide MTLK IRBD data.

    \return  MTLK_ERR... code

    \warning This function should be called once a driver, 
             prior to any other mtlk_irbd_...() call.
*/
int __MTLK_IFUNC
mtlk_irbd_root_init(void);

/*! \fn     void mtlk_irbd_root_cleanup(void)
    \brief  Cleans up the driver-wide MTLK IRB data.

    \warning This function should be called once a driver, 
             no mtlk_irbd_...() calls are allowed after this.
*/
void __MTLK_IFUNC 
mtlk_irbd_root_cleanup(void);

/*! \fn     mtlk_irbd_t *mtlk_irbd_alloc(void)
    \brief  Allocates an IRB object.

    \return Pointer to allocated IRB object. NULL if failed.
*/
mtlk_irbd_t *__MTLK_IFUNC
mtlk_irbd_alloc(void);

/*! \fn     void mtlk_irbd_free(mtlk_irbd_t *irbd)
    \brief  Frees IRB object.

    \param  irbd Pointer to the IRB object to free.
*/
void __MTLK_IFUNC
mtlk_irbd_free(mtlk_irbd_t *irbd);

/*! \fn      int mtlk_irbd_init(mtlk_irbd_t *irbd, 
                                mtlk_irbd_t *parent, 
                                const char  *unique_desc)
    \brief   Initiates IRBD object.

    This function initializes IRBD object.

    \param   irbd        IRBD object to initialize.
    \param   parent      parent IRBD object (root for 1st level (NICs))
    \param   unique_desc Unique string descriptor (usually started with 
                         MTLK_IRB_NIC(WLAN)_NAME)

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_irbd_init(mtlk_irbd_t *irbd, 
               mtlk_irbd_t *parent, 
               const char  *unique_desc);

/*! \fn      void mtlk_irbd_cleanup(mtlk_irbd_t *irbd)
    \brief   Cleans up the IRBD object.

    This function cleans up the IRBD object.

    \param   irbd IRBD object to clean.
*/
void __MTLK_IFUNC
mtlk_irbd_cleanup(mtlk_irbd_t *irbd);

/*! \fn      mtlk_irbd_handle_t *mtlk_irbd_register(mtlk_irbd_t            *irbd,
                                                    const mtlk_guid_t      *evts,
                                                    uint32                  nof_evts,
                                                    mtlk_irbd_evt_handler_f handler,
                                                    mtlk_handle_t           context)

    \brief   Registers an event(s) handler with a specific IRBD object.

    This function registers event handler to be called on desired event(s) arrival.
    It is called by driver's modules that wants to be aware of an event arrived from
    applications.

    \param   irbd     IRBD object.
    \param   evts     array of desired events.
    \param   nof_evts number of events in evts array.
    \param   handler  handler function to be called once a desired event arrives.
    \param   context  context to be passed to handler function.

    \return  pointer to IRBD client object associated with the requested handler. NULL
             if registration fails.
*/
mtlk_irbd_handle_t *__MTLK_IFUNC
mtlk_irbd_register(mtlk_irbd_t            *irbd,
                   const mtlk_guid_t      *evts,
                   uint32                  nof_evts,
                   mtlk_irbd_evt_handler_f handler,
                   mtlk_handle_t           context);

/*! \fn     void mtlk_irbd_unregister (mtlk_irbd_t        *irbd,
                                       mtlk_irbd_handle_t *irbdh)

    \brief   Unregisters an previously registered event(s) handler with a 
             specific IRBD object.

    \param   irbd     IRBD object.
    \param   irbdh    IRBD client object returned from mtlk_irbd_register call.
*/
void __MTLK_IFUNC
mtlk_irbd_unregister(mtlk_irbd_t        *irbd,
                     mtlk_irbd_handle_t *irbdh);

/*! \fn     int mtlk_irbd_notify_app(mtlk_irbd_t       *irbd,
                                     const mtlk_guid_t *evt,
                                     void              *buffer,
                                     uint32             size)

    \brief Posts a notification to application(s). 

    This functions posts a event to application IRB (IRBA) instances that 
    are registered to handle it. The notification is \b asynchronous.
    It is called by driver modules to notify application(s).

    \param   irbd     IRBD object.
    \param   evt      event ID to send (GUID)
    \param   buffer   event data to send
    \param   size     event data size

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_irbd_notify_app(mtlk_irbd_t       *irbd,
                     const mtlk_guid_t *evt,
                     void              *buffer,
                     uint32             size);

/*! \fn     int mtlk_irbd_get_root(void)

    \brief Returns pointer to the root IRB node.
*/
mtlk_irbd_t * __MTLK_IFUNC
mtlk_irbd_get_root (void);

BOOL __MTLK_IFUNC
mtlk_irbd_hanlder_ctx_evt_valid (mtlk_irbd_t       *irbd,
                                 mtlk_handle_t      ctx,
                                 const mtlk_guid_t *evt,
                                 void              *buff,
                                 uint32            *size,
                                 const mtlk_guid_t *evt_exp,
                                 mtlk_slid_t        caller_slid);

#define MTLK_IRBD_CHECK_CTX_EVT(_irbd, _ctx, _evt, _buff, _size, _evt_exp) { \
  if (!mtlk_irbd_hanlder_ctx_evt_valid(_irbd, _ctx, _evt, _buff, _size, _evt_exp, MTLK_SLID)) { \
    return; \
  } \
}

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRBD_H__ */
