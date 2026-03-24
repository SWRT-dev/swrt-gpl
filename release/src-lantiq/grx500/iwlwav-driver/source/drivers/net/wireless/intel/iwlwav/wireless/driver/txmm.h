/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_TXMM_H_
#define _MTLK_TXMM_H_

#include "mtlk_osal.h"
/* #include "mtlk_vap_manager.h" */
#include "mtlkdfdefs.h"
#include "mtlk_mmb_msg.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/** 
*\file txmm.h

*\brief TXMM module API
*\brief Cross platform API of messages serialization module.

*\defgroup AUX Auxilliary infrastructure CP modules
*\{
*/

/*! \def   TXMM_DEFAULT_TIMEOUT
    \brief Default message sending timeout.

    When used for UMI messages with healthy MAC, there are no messages
    that could be handled more then TXMM_DEFAULT_TIMEOUT.
    No confirmation during TXMM_DEFAULT_TIMEOUT usually means MAC
    crashed or entered a busy loop.
*/
#define TXMM_DEFAULT_TIMEOUT 30000 /* ms */

/*! \def   MTLK_MM_BLOCKED_SEND_TIMEOUT
    \brief Default management message sending timeout.
*/
#ifdef MTLK_LGM_PLATFORM_FPGA
/* LGM FPGA works very slow */
#define MTLK_MM_BLOCKED_SEND_TIMEOUT (30000/10) /* ms */
#else
#define MTLK_MM_BLOCKED_SEND_TIMEOUT 30000    /* ms */
#endif

/*! \typedef enum _tx_man_msg_block_e mtlk_txmm_block_e
    \brief   TXMM module's internals.

    \warning Irrelevant for public TXMM API.
*/
typedef enum _tx_man_msg_block_e
{
  MTLK_TXMMB_FREE,
  MTLK_TXMMB_QUEUED,
  MTLK_TXMMB_SENT,
  MTLK_TXMMB_TIMED_OUT,
  MTLK_TXMMB_NONE
} mtlk_txmm_block_e;

/*! \def   MTLK_TXMMB_LAST
    \brief TXMM module's internals.

    \warning Irrelevant for public TXMM API.
*/
#define MTLK_TXMMB_LAST  MTLK_TXMMB_NONE

/*! \typedef struct _mtlk_txmm_cfg_t mtlk_txmm_cfg_t
    \brief   TXMM module configuration.

    TXMM module configuration parameters.
*/
typedef struct _mtlk_txmm_cfg_t
{ 
  uint16 max_payload_size; /*!< maximal payload size */
  uint32 max_msgs;         /*!< maximal nof payloads to maintain */
  uint32 tmr_granularity;  /*!< timeout check granularity */
} __MTLK_IDATA mtlk_txmm_cfg_t;

/*! \typedef struct _mtlk_txmm_wrap_api_t mtlk_txmm_wrap_api_t
    \brief   TXMM module's outward API.

    The API is used by TXMM module to implement actual sending and 
    notification functionality.
*/
typedef struct _mtlk_txmm_wrap_api_t
{ 
  mtlk_handle_t   usr_data; /*!< user-defined context to be used in outward API calls */
  PMSG_OBJ        (__MTLK_IFUNC *msg_get_from_pool)(mtlk_handle_t usr_data); /*!< function is called to obtain a free real slot to 
                                                                                  be used for message sending
                                                                              */
  void            (__MTLK_IFUNC *msg_free_to_pool)(mtlk_handle_t   usr_data, /*!< function is called to free a real slot that is 
                                                                                  out of use
                                                                             */
                                                   PMSG_OBJ pmsg);
  void            (__MTLK_IFUNC *msg_send)(mtlk_handle_t    usr_data,         /*!< function is called to send a message via real slot
                                                                                  received from msg_get_from_pool
                                                                               */
                                           PMSG_OBJ          pmsg,
                                           mtlk_vap_handle_t vap_handle);
  void            (__MTLK_IFUNC *msg_timed_out)(mtlk_handle_t    usr_data,         /*!< function is called when message send timeout expired
                                                                                    */
                                                uint16 msg_id);
} __MTLK_IDATA mtlk_txmm_wrap_api_t;

/*! \typedef struct _mtlk_txmm_data_t mtlk_txmm_data_t
    \brief   TXMM payload/data structure.

    Initially, contains an actual data to send. 
    Upon the confirm arrived (in CFM callback or after blocked send) - contains 
    the CFM data.
*/
typedef struct _mtlk_txmm_data_t
{
  uint16               id;           /*!< message ID */
  void*                payload;      /*!< message payload buffer */
  uint16               payload_size; /*!< message payload size to send */
} __MTLK_IDATA mtlk_txmm_data_t;

/*! \typedef enum _mtlk_txmm_clb_reason_e mtlk_txmm_clb_reason_e
    \brief   TXMM CFM callback reason code.

    This code allows callback to understand a call reason and handle it properly.
*/
typedef enum _mtlk_txmm_clb_reason_e
{
  MTLK_TXMM_CLBR_CONFIRMED,      /*!< message confirmation has arrived */
  MTLK_TXMM_CLBR_TIMEOUT_UNSENT, /*!< message hasn't been even sent, 
                                      most likely due to a previous message stuck
                                 */
  MTLK_TXMM_CLBR_TIMEOUT_SENT,   /*!< message has been sent but not confirmed
                                      withing the timeout
                                 */
  MTLK_TXMM_CLBR_RCVRY_DISCARD,  /*!< message canceled by recovery procedure */
  MTLK_TXMM_CLBR_LAST
} mtlk_txmm_clb_reason_e;

/*! \typedef enum _mtlk_txmm_clb_action_e mtlk_txmm_clb_action_e
    \brief   TXMM CFM callback return code.

    This code tells the TXMM what to do with payload attached.
*/
typedef enum _mtlk_txmm_clb_action_e
{
  MTLK_TXMM_CLBA_FREE, /*!< detach the payload */
  MTLK_TXMM_CLBA_SEND, /*!< keep the payload attached, re-send the message */
  MTLK_TXMM_CLBA_KEEP, /*!< keep the payload attached */
  MTLK_TXMM_CLBA_LAST
} mtlk_txmm_clb_action_e;

/*! \typedef  mtlk_txmm_clb_action_e (*mtlk_txmm_clb_f)(mtlk_handle_t          clb_usr_data,
                                                       mtlk_txmm_data_t*      data, 
                                                       mtlk_txmm_clb_reason_e reason)
    \brief    TXMM CFM callback prototype.

    \param    clb_usr_data the same clb_usr_data as passed to mtlk_txmm_msg_send()
    \param    data         payload/data sent (and in case of real CFM - received)
    \param    reason       a callback reason

    \return   MTLK_TXMM_CLBA_... code

    \warning  calling mtlk_txmm_...() API from the callback is strictly prohibited!
              Return an appropriate mtlk_txmm_clb_action_e value instead.
*/
typedef mtlk_txmm_clb_action_e (__MTLK_IFUNC *mtlk_txmm_clb_f)(mtlk_handle_t          clb_usr_data,
                                                               mtlk_txmm_data_t*      data, 
                                                               mtlk_txmm_clb_reason_e reason); 

/*! \typedef struct _mtlk_txmm_block_t mtlk_txmm_block_t
    \brief   TXMM module's internals.

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_block_t mtlk_txmm_block_t;

/*! \typedef struct _mtlk_txmm_que_t mtlk_txmm_que_t
    \brief   TXMM module's internals.

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_que_t
{
  mtlk_txmm_block_t** ptr;
  uint32              from;
  uint32              to;
  uint32              total;
} __MTLK_IDATA mtlk_txmm_que_t;

/*! \typedef struct _mtlk_txmm_bsend_t mtlk_txmm_bsend_t
    \brief   TXMM module's internals (blocked send related).

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_bsend_t
{
  mtlk_rmlock_t     rmlock;
  mtlk_osal_mutex_t mutex;
  mtlk_osal_event_t evt;
} __MTLK_IDATA mtlk_txmm_bsend_t;

#if CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH
/*! \typedef enum mtlk_txmm_stats_history_entry_ts_e
    \brief   TXMM module's internals (statistics related).

    \warning Irrelevant for public TXMM API.
*/
typedef enum
{
  MTLK_TXMM_HE_TS_ARRIVED,
  MTLK_TXMM_HE_TS_SENT_TO_FW,
  MTLK_TXMM_HE_TS_CFM_CLB_CALLED,
  MTLK_TXMM_HE_TS_BWAIT_FINISHED,
  MTLK_TXMM_HE_TS_LAST
} mtlk_txmm_stats_history_entry_ts_e;

/*! \typedef struct _mtlk_txmm_stats_history_t mtlk_txmm_stats_history_t
    \brief   TXMM module's internals (statistics related).

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_stats_history_entry_t
{
  uint16                 msg_id;
  int                    blocked_res;
  mtlk_txmm_clb_reason_e clb_reason;
  uint32                 ts[MTLK_TXMM_HE_TS_LAST];
} __MTLK_IDATA mtlk_txmm_stats_history_entry_t;

/*! \typedef struct _mtlk_txmm_stats_history_t mtlk_txmm_stats_history_t
    \brief   TXMM module's internals (statistics related).

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_stats_history_t
{
  mtlk_txmm_stats_history_entry_t entries[CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH];
  uint32                          widx;
} __MTLK_IDATA mtlk_txmm_stats_history_t;
#endif

/*! \typedef struct _mtlk_txmm_stats_t mtlk_txmm_stats_t
    \brief   TXMM module's internals (statistics related).

    \warning Irrelevant for public TXMM API.
*/
typedef struct _mtlk_txmm_stats_t
{
  uint32                    nof_sent;   /*!< number of sent messages                  */
  uint32                    nof_cfmed;  /*!< number of confirmed messages             */
  uint32                    used_peak;  /*!< max number of used messages              */
#if CPTCFG_IWLWAV_TXMM_HISTORY_LENGTH
  mtlk_txmm_stats_history_t history;    /*!< messages advanced history                */
#endif
} __MTLK_IDATA mtlk_txmm_stats_t;

/*! \typedef struct _mtlk_txmm_t mtlk_txmm_t
    \brief   TXMM module's object - internals.

    \warning This structure can't be accessed directly, but only thru the TXMM API.
*/
typedef struct _mtlk_txmm_base_t
{
  mtlk_txmm_cfg_t           cfg;
  mtlk_txmm_wrap_api_t      api;
  mtlk_txmm_que_t           ques[MTLK_TXMMB_LAST];
  uint8*                    storage;
  mtlk_osal_spinlock_t      lock;
  mtlk_txmm_bsend_t         bsend; /* used for blocked send */
  mtlk_txmm_stats_t         stats;
  uint32                    flags;
  mtlk_osal_timer_t         timer;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_INIT_LOOP(TXMM_QUEUES);
  MTLK_DECLARE_START_STATUS;
} __MTLK_IDATA mtlk_txmm_base_t;

/*! \fn       int mtlk_txmm_init(mtlk_txmm_base_t           *obj, 
                                 const mtlk_txmm_cfg_t      *cfg, 
                                 const mtlk_txmm_wrap_api_t *api)
    \brief    initializes TXMM object.

    This function is called to initiate the object's internals.

    \param    obj          TXMM module object
    \param    cfg          desired TXMM module's configuration
    \param    api          an outward API to be used by TXMM module (VFT)

    \return   MTLK_ERR_... code
*/
int __MTLK_IFUNC
mtlk_txmm_init(mtlk_txmm_base_t           *obj, 
               const mtlk_txmm_cfg_t      *cfg, 
               const mtlk_txmm_wrap_api_t *api);

/*! \fn       int mtlk_txmm_start(mtlk_txmm_base_t *obj)
    \brief    starts TXMM object.

    This function is called to start the TXMM object functionality (timers etc.).

    \param    obj          TXMM module object

    \return   MTLK_ERR_... code
*/
int __MTLK_IFUNC
mtlk_txmm_start(mtlk_txmm_base_t *obj);

/*! \fn       int mtlk_txmm_pump(mtlk_txmm_base_t* obj, PMSG_OBJ* pmsg)
    \brief    pumps TXMM queues.

    This function is called to pump the TXMM queues and re-use an out-of-use
    message slot (typically, immediately after mtlk_txmm_on_cfm).
    If no slot is given, mtlk_txmm_pump will try to get it using the
    corresponding outward API.
    
    \param    obj          TXMM module object
    \param    pmsg         real message slot that has been confirmed

    \return   MTLK_ERR_... code
    
    \note     it there are no queued messages in TXMM queues and a message slot
              is passed then TXMM will free the message slot using the 
              corresponding outward API.
*/
int __MTLK_IFUNC
mtlk_txmm_pump(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg);

void __MTLK_IFUNC
mtlk_txmm_restore(mtlk_txmm_base_t *obj);

/*! \fn       int mtlk_txmm_on_cfm(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg)
    \brief    CFM handler

    This function is called when a CFM arrived to handle it by TXMM.
    
    \param    obj          TXMM module object
    \param    pmsg         real message slot that has been confirmed

    \return   MTLK_ERR_... code

    \note     TXMM handles only the messages it sends, any other message 
              could be passed to this function safely - TXMM will ignore it.

*/
int __MTLK_IFUNC
mtlk_txmm_on_cfm(mtlk_txmm_base_t* obj, PMSG_OBJ pmsg);

/*! \fn       void mtlk_txmm_stop(mtlk_txmm_base_t *obj)
    \brief    stops TXMM object.

    This function is called to stop the TXMM object functionality (timers etc.)

    \param    obj          TXMM module object
*/
void __MTLK_IFUNC
mtlk_txmm_stop(mtlk_txmm_base_t *obj);

/*! \fn       void mtlk_txmm_cleanup(mtlk_txmm_base_t *obj)
    \brief    cleans up TXMM object.

    This function is called to clean up the object's internals.

    \param    obj          TXMM module object
*/
void __MTLK_IFUNC
mtlk_txmm_cleanup(mtlk_txmm_base_t* obj);

/*! \fn       void mtlk_txmm_halt(mtlk_txmm_base_t *obj)
    \brief    marks the TXMM object as halted.

    This function is called to mark TXMM object as halted.

    \param    obj          TXMM module object
*/
void __MTLK_IFUNC
mtlk_txmm_halt(mtlk_txmm_base_t* obj);

/*! \fn       BOOL mtlk_txmm_is_running(mtlk_txmm_base_t *obj)
    \brief    queries TXMM object for running (not-halted) state.

    This function is called to queriy the TXMM object for running (not-halted) state.

    \param    obj          TXMM module object

    \return   TRUE if still running, FALSE - if halted
*/
BOOL __MTLK_IFUNC
mtlk_txmm_is_running(mtlk_txmm_base_t* obj);


/*! \typedef struct mtlk_txmm_t
    \brief   TXMM object, including txmm_base and vap_index. Functions called from the core should use this shadow object.

*/
typedef struct _mtlk_txmm_t
{
  mtlk_txmm_base_t *base;
  mtlk_vap_handle_t vap_handle;
  mtlk_hw_api_t    *hw_api;
} __MTLK_IDATA mtlk_txmm_t;

/*! \typedef struct _mtlk_txmm_msg_t mtlk_txmm_msg_t
    \brief   TXMM message object.

    \warning This structure is "Black box" and must not be accessed directly,
             but only through the mtlk_txmm_msg... API.
*/
typedef struct _mtlk_txmm_msg_t
{
  mtlk_osal_spinlock_t  lock;
  mtlk_txmm_data_t     *data;
  mtlk_txmm_t          *obj;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);
} __MTLK_IDATA mtlk_txmm_msg_t;

/*! \fn       int mtlk_txmm_msg_init(mtlk_txmm_msg_t *msg)
    \brief    initializes TXMM message object

    This function is called to initialize TXMM message object.

    \param    msg          TXMM message object

    \return  MTLK_ERR_... code
*/
#ifndef CPTCFG_IWLWAV_ENABLE_OBJPOOL
int __MTLK_IFUNC
mtlk_txmm_msg_init(mtlk_txmm_msg_t *msg); 
#else
int __MTLK_IFUNC
_mtlk_txmm_msg_init_objpool(mtlk_txmm_msg_t *msg,
                            mtlk_slid_t caller_slid);
#define mtlk_txmm_msg_init(msg)                                 \
  _mtlk_txmm_msg_init_objpool((msg), MTLK_SLID)
#endif

/*! \fn       void mtlk_txmm_msg_cleanup(mtlk_txmm_msg_t *msg)
    \brief    cleans up TXMM message object

    This function is called to clean up TXMM message object. It also detaches
    a message payload/data if attached.

    \param    msg          TXMM message object
*/
void __MTLK_IFUNC
mtlk_txmm_msg_cleanup(mtlk_txmm_msg_t *msg); 

/*! \fn       mtlk_txmm_data_t* mtlk_txmm_msg_get_empty_data(mtlk_txmm_msg_t *msg, 
                                                             mtlk_txmm_t     *obj)
    \brief    attaches an empty message payload/data to message and returns it

    This function is called to attach an empty message payload/data from the 
    corresponding TXMM object pool and to get the attached payload/data object.

    \param    msg          TXMM message object
    \param    obj          TXMM object

    \return  attached payload/data object, NULL if failed (for exampl, because
             of lack of empty payload/data objects)
*/
mtlk_txmm_data_t* __MTLK_IFUNC
mtlk_txmm_msg_get_empty_data(mtlk_txmm_msg_t *msg, mtlk_txmm_t *obj);

/*! \fn       void mtlk_txmm_msg_cancel(mtlk_txmm_msg_t *msg)
    \brief    cancel TXMM message object sending

    This function is called to cancel previously sent TXMM message. It also detaches
    a message payload/data if attached.
    The function is safe and could also be called ob unsent (but initialized) messages.

    \param    msg          TXMM message object
*/
void __MTLK_IFUNC
mtlk_txmm_msg_cancel(mtlk_txmm_msg_t *msg);

/*! \fn       int mtlk_txmm_msg_send(mtlk_txmm_msg_t  *msg, 
                                    mtlk_txmm_clb_f   clb,
                                    mtlk_handle_t     clb_usr_data,
                                    uint32            timeout_ms)
    \brief    sends the TXMM message in asynchronous way

    This function is called to send the TXMM message in asynchronous way.
    An payload/data must be attached and filled prior to calling this function.

    \param    msg          TXMM message object
    \param    clb          TXMM CFM/timeout callback
    \param    clb_usr_data user data to be passed to TXMM CFM/timeout callback
    \param    timeout_ms   send timeout

    \return   MTLK_ERR_... code
*/
int __MTLK_IFUNC
mtlk_txmm_msg_send(mtlk_txmm_msg_t  *msg, 
                   mtlk_txmm_clb_f   clb,
                   mtlk_handle_t     clb_usr_data,
                   uint32            timeout_ms);


/*! \fn       int mtlk_txmm_msg_send_blocked(mtlk_txmm_msg_t  *msg, 
                                             uint32            timeout_ms)
    \brief    sends the TXMM message in synchronous way

    This function is called to send the TXMM message in synchronous (blocked) way.
    An payload/data must be attached and filled prior to calling this function.

    \param    msg          TXMM message object
    \param    timeout_ms   send timeout

    \return   MTLK_ERR_... code, MTLK_ERR_OK if succeded (a CFM received within the timeout_ms),
              MTLK_ERR_TIMEOUT if timeout reached with no CFM received.
*/
int __MTLK_IFUNC
mtlk_txmm_msg_send_blocked(mtlk_txmm_msg_t  *msg, 
                           uint32            timeout_ms);

/*! \fn       mtlk_txmm_data_t* mtlk_txmm_msg_init_with_empty_data(mtlk_txmm_msg_t *msg, 
                                                                   mtlk_txmm_t     *obj,
                                                                   int             *err)
    \brief    initializes TXMM message object with an empty message payload/data and 
              returns the attached payload/data

    This function is called to both initiate TXMM message object, attach an empty message 
    payload/data from the corresponding TXMM object pool and get the attached payload/data object.

    \param    msg          TXMM message object
    \param    obj          TXMM object
    \param    err          optional, initialization error

    \return  attached payload/data object, NULL if failed (for exampl, because
             of lack of empty payload/data objects)
*/
mtlk_txmm_data_t* __MTLK_IFUNC
mtlk_txmm_msg_init_with_empty_data(mtlk_txmm_msg_t *msg, 
                                   mtlk_txmm_t     *obj,
                                   int             *err);

/*! \fn       void mtlk_txmm_get_stats(mtlk_txmm_t       *obj, 
                                       mtlk_txmm_stats_t *stats)
    \brief    retrieves TXMM object statistics

    This function is called to retrieve the TXMM module internal statistics.

    \param    obj          TXMM object
    \param    stats        statistics structure to fill
*/
void __MTLK_IFUNC
mtlk_txmm_get_stats(mtlk_txmm_t       *obj, 
                    mtlk_txmm_stats_t *stats);

void __MTLK_IFUNC
mtlk_txmm_terminate_waiting_msg(mtlk_txmm_base_t *base);

/*! \fn       void mtlk_txmm_base_get_stats(mtlk_txmm_base_t  *base,
                                            mtlk_txmm_stats_t *stats)
    \brief    retrieves TXMM base object statistics

    This function is called to retrieve the TXMM module internal statistics.

    \param    base         TXMM base object
    \param    stats        statistics structure to fill
*/
void __MTLK_IFUNC
mtlk_txmm_base_get_stats(mtlk_txmm_base_t  *base,
                         mtlk_txmm_stats_t *stats);

/*\}*/

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* !_MTLK_TXMM_H_ */

