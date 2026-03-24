/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_IRBA_H__
#define __MTLK_IRBA_H__

#include "mtlkguid.h"
#include "mtlkirb_osdep.h"

/*! \file  mtlkirba.h
    \brief IRB Application part

    The part of IRB API that available in application (i.e. slave-part).
    It also contains the MTLK topology (HWs, WLANs) discovery API (mtlk_node_...) 
    which can be used by applications to parse current MTLK objects topology.
*/

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/******************************************************************************
 * IRBA - IRB Application Layer
 ******************************************************************************/

/*! \typedef struct mtlk_irba_private mtlk_irba_t
    \brief   "Black box" IRBA object. 
*/
typedef struct mtlk_irba_private mtlk_irba_t;

/*! \typedef struct mtlk_irba_handle_private mtlk_irba_handle_t
    \brief   "Black box" IRB's client object. 
*/
typedef struct mtlk_irba_handle_private mtlk_irba_handle_t;

/*! \typedef void (*mtlk_irba_evt_handler_f)(mtlk_irbd_t       *irba,
                                             mtlk_handle_t      context,
                                             const mtlk_guid_t *evt,
                                             void              *buffer,
                                             uint32            size)
    \brief   IRB event handler function prototype.

    This function could be registered as an event handler using the is mtlk_irba_register API.
    Then it will be called on a desired event(s) arrival.

    \param   irba    IRBA object that received the event.
    \param   context user context passed to mtlk_irba_register.
    \param   evt     Arrived event ID (GUID).
    \param   buffer  Buffer arived with the event. 
    \param   size    Size of the buffer.
*/
typedef void (__MTLK_IFUNC * mtlk_irba_evt_handler_f)(mtlk_irba_t       *irba,
                                                      mtlk_handle_t      context,
                                                      const mtlk_guid_t *evt,
                                                      void              *buffer,
                                                      uint32             size);

/*! \typedef void (*mtlk_irba_rm_handler_f)(mtlk_irbd_t       *irba,
                                            mtlk_handle_t      context)
    \brief   "IRB object disappears" event handler function prototype.

    This function is a \b mandatory parameter for the mtlk_irba_app_init (for root IRBA object) 
    and mtlk_irba_init (for specific IRBA object) API.
    The "IRB object disappears" event handler will be called once the IRBA object will disappear 
    in the driver.

    \param   irba    IRBA object that received the event.
    \param   context user context passed to mtlk_irba_app_init/mtlk_irba_init.
*/
typedef void (__MTLK_IFUNC * mtlk_irba_rm_handler_f)(mtlk_irba_t       *irba,
                                                     mtlk_handle_t      context);

/*! \var     mtlk_irba_t *mtlk_irba_root
    \brief   \b private pointer to Root IRBA object

    \warning Must not be accessed directly. Use MTLK_IRBA_ROOT instead.
*/
extern mtlk_irba_t *mtlk_irba_root;

/*! \def   MTLK_IRBA_ROOT
    \brief Accessor for Root IRBA object.

    Can be used for any kind of client's IRBA access: 
    - mtlk_irba_register/mtlk_irba_unregister
    - mtlk_irba_call_drv

    \note  The Root IRBA object is present, initiated and accessible anywhere between 
           the mtlk_irba_app_init and mtlk_irba_app_cleanup calls.
*/
#define MTLK_IRBA_ROOT mtlk_irba_root

/*! \brief   Initiates application-wide MTLK IRBA data.

    \param  handler \b mandatory "IRB object disappears" event handler.
    \param  context user context to be passed to handler.

    \return  MTLK_ERR... code

    \warning This function should be called once by an application, 
             prior to any other mtlk_irba_...() call.
*/
mtlk_error_t __MTLK_IFUNC
mtlk_irba_app_init(mtlk_irba_rm_handler_f handler,
                   mtlk_handle_t          context);

/*! \brief   Starts application-wide MTLK IRBA functionality.

    \return  MTLK_ERR... code

    \warning This function should be called once by an application, 
             prior to any other mtlk_irba_...() call except of mtlk_irba_app_init.
*/
mtlk_error_t __MTLK_IFUNC
mtlk_irba_app_start(void);

int __MTLK_IFUNC
mtlk_irba_app_create_stop_pipe(void);

/*! \brief  Stops the application-wide MTLK IRB functionality.

    \warning This function should be called once an application, 
             no mtlk_irbd_...() calls are allowed after this except of
             mtlk_irba_app_cleanup and mtlk_irba_app_start.
*/
void __MTLK_IFUNC
mtlk_irba_app_stop(void);

void __MTLK_IFUNC
mtlk_irba_app_close_pipe_handles(void);

/*! \brief  Cleans up the application-wide MTLK IRB data.

    \warning This function should be called once an application, 
             no mtlk_irba_...() calls are allowed after this.
*/
void __MTLK_IFUNC
mtlk_irba_app_cleanup(void);

/*! \brief  Allocates an IRB object.

    This function is called by application modules to get reference to driver's IRB instances

    \return Pointer to allocated IRB object. NULL if failed.
*/
mtlk_irba_t * __MTLK_IFUNC
mtlk_irba_alloc(void);

/*! \brief  Frees IRB object.

    \param  irba Pointer to the IRB object to free.
*/
void __MTLK_IFUNC
mtlk_irba_free(mtlk_irba_t *irba);

/*! \brief   Initiates IRBA object.

    This function initializes IRBA object (i.e. gets a reference to 
    the corresponding driver's IRB istance).

    \param   irba        IRB object to initialize.
    \param   unique_desc Unique string descriptor (usually started with 
                         MTLK_IRB_NIC(WLAN)_NAME)
    \param  handler      \b mandatory "IRB object disappears" event handler.
    \param  context      user context to be passed to handler.

    \return  MTLK_ERR... code
*/
mtlk_error_t __MTLK_IFUNC
mtlk_irba_init(mtlk_irba_t           *irba,
               const char            *unique_desc,
               mtlk_irba_rm_handler_f handler,
               mtlk_handle_t          context);

/*! \brief   Cleans up the IRBA object.

    This function cleans up the IRBA object.

    \param   irba IRBA object to clean.
*/
void __MTLK_IFUNC
mtlk_irba_cleanup(mtlk_irba_t *irba);


/*! \brief   Registers an event(s) handler with a specific IRBA object.

    This function registers event handler to be called on desired event(s) arrival.
    It is called by application's modules that wants to be aware of an event arrived
    from the driver.

    \param   irba     IRBA object.
    \param   evts     array of desired events.
    \param   nof_evts number of events in evts array.
    \param   handler  handler function to be called once a desired event arrives.
    \param   context  context to be passed to handler function.

    \return  pointer to IRBA client object associated with the requested handler. NULL
             if registration fails.
*/
mtlk_irba_handle_t *__MTLK_IFUNC
mtlk_irba_register(mtlk_irba_t            *irba,
                   const mtlk_guid_t      *evts,
                   uint32                  nof_evts,
                   mtlk_irba_evt_handler_f handler,
                   mtlk_handle_t           context);

/*! \brief   Unregisters an previously registered event(s) handler with a
             specific IRBA object.

    \param   irba     IRBA object.
    \param   irbah    IRBA client object returned from mtlk_irba_register call.
*/
void __MTLK_IFUNC
mtlk_irba_unregister(mtlk_irba_t        *irba,
                     mtlk_irba_handle_t *irbah);

/*! \brief Call the driver.

    This functions call the driver with specified event. Driver handlers registered to handle 
    it will be called. The call is \b synchronous.

    \param   irba     IRBA object.
    \param   evt      event ID to send (GUID)
    \param   buffer   event data to send
    \param   size     event data size

    \return  MTLK_ERR... code
*/
mtlk_error_t __MTLK_IFUNC
mtlk_irba_call_drv(mtlk_irba_t       *irba,
                   const mtlk_guid_t *evt,
                   void              *buffer,
                   uint32             size);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRBA_H__ */
