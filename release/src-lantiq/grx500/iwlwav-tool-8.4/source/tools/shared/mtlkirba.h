/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
 * Topology
 ******************************************************************************/

/*! \typedef mtlk_node_private mtlk_node_t
    \brief   "Black box" MTLK node object. 
*/
typedef struct mtlk_node_private mtlk_node_t;

/*! \typedef struct mtlk_node_son_private mtlk_node_son_t
    \brief   "Black box" MTLK node's son object used for enumeration. 
*/
typedef struct mtlk_node_son_private mtlk_node_son_t;

/*! \fn     mtlk_node_t *mtlk_node_alloc(void)
    \brief  Allocates an MTLK Node object.

    \return Pointer to allocated Node object. NULL if failed.
*/
mtlk_node_t *__MTLK_IFUNC
mtlk_node_alloc(void);

/*! \fn     void mtlk_node_free(mtlk_node_t *irbd)
    \brief  Frees Node object.

    \param  node Pointer to the Node object to free.
*/
void __MTLK_IFUNC
mtlk_node_free(mtlk_node_t *node);

/*! \fn      int mtlk_node_init(mtlk_node_t *node, const char* unique_desc)
    \brief   Attaches the Node object to corresponding kernel object.

    This function attaches the Node object to corresponding kernel object
    according to it's unique description.

    \param   node        Node object to initialize.
    \param   unique_desc Unique string descriptor (usually started with 
                         MTLK_IRB_NIC(WLAN)_NAME). 
                         NULL for root (the highest level) object.

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_node_attach(mtlk_node_t *node, const char* unique_desc);

/*! \fn      void mtlk_node_detach(mtlk_node_t *node)
    \brief   Detaches the Node object from kernel object.

    This function detaches the Node object from kernel object.

    \param   node Node object to clean.
*/
void __MTLK_IFUNC
mtlk_node_detach(mtlk_node_t *node);

/*! \fn      int mtlk_node_get_nof_sons(mtlk_node_t *node)
    \brief   Returns the number of node's sons.

    This function returnes the actual number of sons of the node.

    \param   node Node object.

    \return  Actual number of sons.
*/
int __MTLK_IFUNC
mtlk_node_get_nof_sons(mtlk_node_t *node);

/*! \fn      mtlk_node_son_t *mtlk_node_son_get_first(mtlk_node_t *node)
    \brief   Starts the sons enumeration.

    This function starts the enumeration of node's sons. 

    \param   node Node object

    \return  Pointer to 1st node's son object. NULL if there is the node has no sons.
*/
mtlk_node_son_t * __MTLK_IFUNC
mtlk_node_son_get_first(mtlk_node_t *node);

/*! \fn      mtlk_node_son_t *mtlk_node_son_get_next(mtlk_node_t    *node,
                                                    mtlk_node_son_t *prev_son)
    \brief   Continues the sons enumeration.

    This function continues the enumeration of node's sons. 

    \param   node     Node object
    \param   prev_son previous son object

    \return  Pointer to the next node's son object. NULL if there are no more sons.
*/
mtlk_node_son_t * __MTLK_IFUNC
mtlk_node_son_get_next(mtlk_node_t     *node,
                       mtlk_node_son_t *prev_son);

/*! \fn      const char *mtlk_node_son_get_desc(mtlk_node_t     *node,
                                                mtlk_node_son_t *son)
    \brief   Returnes son's unique descriptor.

    \param   node Node object
    \param   son  Node's son object

    \return  Son's unique descriptor which is valid until the mtlk_node_cleanup call.
*/
const char * __MTLK_IFUNC
mtlk_node_son_get_desc(mtlk_node_t     *node,
                       mtlk_node_son_t *son);

/*! \fn      const char *mtlk_node_parent_get_desc(mtlk_node_t *node)
    \brief   Returnes parent's unique descriptor.

    \param   node Node object

    \return  Parent's unique descriptor which is valid until the mtlk_node_cleanup call.
             NULL means this node's parent is the root node.

    \warning The call is not allowed for parent Node object (i.e. object that was 
             initialized with NULL unique descriptor).
*/
const char * __MTLK_IFUNC
mtlk_node_parent_get_desc(mtlk_node_t *node);

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

/*! \fn      int mtlk_irba_app_init(mtlk_irba_rm_handler_f handler,
                                    mtlk_handle_t          context)
    \brief   Initiates application-wide MTLK IRBA data.

    \param  handler \b mandatory "IRB object disappears" event handler.
    \param  context user context to be passed to handler.

    \return  MTLK_ERR... code

    \warning This function should be called once by an application, 
             prior to any other mtlk_irba_...() call.
*/
int           __MTLK_IFUNC
mtlk_irba_app_init(mtlk_irba_rm_handler_f handler,
                   mtlk_handle_t          context);

/*! \fn      int mtlk_irba_app_start(void)
    \brief   Starts application-wide MTLK IRBA functionality.

    \return  MTLK_ERR... code

    \warning This function should be called once by an application, 
             prior to any other mtlk_irba_...() call except of mtlk_irba_app_init.
*/
int __MTLK_IFUNC
mtlk_irba_app_start(void);

int __MTLK_IFUNC
mtlk_irba_app_create_stop_pipe(void);

/*! \fn     void mtlk_irba_app_stop(void)
    \brief  Stops the application-wide MTLK IRB functionality.

    \warning This function should be called once an application, 
             no mtlk_irbd_...() calls are allowed after this except of
             mtlk_irba_app_cleanup and mtlk_irba_app_start.
*/
void __MTLK_IFUNC
mtlk_irba_app_stop(void);

void __MTLK_IFUNC
mtlk_irba_app_close_pipe_handles(void);

/*! \fn     void mtlk_irba_app_cleanup(void)
    \brief  Cleans up the application-wide MTLK IRB data.

    \warning This function should be called once an application, 
             no mtlk_irba_...() calls are allowed after this.
*/
void __MTLK_IFUNC
mtlk_irba_app_cleanup(void);

/* Called by application modules to get reference to driver's IRB istances */
/*! \fn     mtlk_irba_t *mtlk_irba_alloc(void)
    \brief  Allocates an IRB object.

    \return Pointer to allocated IRB object. NULL if failed.
*/
mtlk_irba_t * __MTLK_IFUNC
mtlk_irba_alloc(void);

/*! \fn     void mtlk_irba_free(mtlk_irba_t *irba)
    \brief  Frees IRB object.

    \param  irba Pointer to the IRB object to free.
*/
void __MTLK_IFUNC
mtlk_irba_free(mtlk_irba_t *irba);

/*! \fn      int mtlk_irba_init(mtlk_irba_t           *irba,
                                const char            *unique_desc,
                                mtlk_irba_rm_handler_f handler,
                                mtlk_handle_t          context)
    \brief   Initiates IRBA object.

    This function initializes IRBA object (i.e. gets a reference to 
    the corresponding driver's IRB istance).

    \param   irba        IRB object to initialize.
    \param   unique_desc Unique string descriptor (usually started with 
                         MTLK_IRB_NIC(WLAN)_NAME)
    \param  handler      \b mandatory "IRB object disappears" event handler.
    \param  context      user context to be passed to handler.

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_irba_init(mtlk_irba_t           *irba,
               const char            *unique_desc,
               mtlk_irba_rm_handler_f handler,
               mtlk_handle_t          context);

/*! \fn      void mtlk_irba_cleanup(mtlk_irba_t *irba)
    \brief   Cleans up the IRBA object.

    This function cleans up the IRBA object.

    \param   irba IRBA object to clean.
*/
void __MTLK_IFUNC
mtlk_irba_cleanup(mtlk_irba_t *irba);


/*! \fn      mtlk_irba_handle_t *mtlk_irba_register(mtlk_irba_t            *irba,
                                                    const mtlk_guid_t      *evts,
                                                    uint32                  nof_evts,
                                                    mtlk_irba_evt_handler_f handler,
                                                    mtlk_handle_t           context)
    \brief   Registers an event(s) handler with a specific IRBA object.

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

/*! \fn     mtlk_irba_unregister(mtlk_irba_t        *irba,
                                 mtlk_irba_handle_t *irbah)
    \brief   Unregisters an previously registered event(s) handler with a 
             specific IRBA object.

    \param   irba     IRBA object.
    \param   irbah    IRBA client object returned from mtlk_irba_register call.
*/
void __MTLK_IFUNC
mtlk_irba_unregister(mtlk_irba_t        *irba,
                     mtlk_irba_handle_t *irbah);

/*! \fn     int mtlk_irba_call_drv(mtlk_irba_t       *irba,
                                   const mtlk_guid_t *evt,
                                   void              *buffer,
                                   uint32             size)
    \brief Call the driver.

    This functions call the driver with specified event. Driver handlers registered to handle 
    it will be called. The call is \b synchronous.

    \param   irba     IRBA object.
    \param   evt      event ID to send (GUID)
    \param   buffer   event data to send
    \param   size     event data size

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_irba_call_drv(mtlk_irba_t       *irba,
                   const mtlk_guid_t *evt,
                   void              *buffer,
                   uint32             size);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_IRBA_H__ */
