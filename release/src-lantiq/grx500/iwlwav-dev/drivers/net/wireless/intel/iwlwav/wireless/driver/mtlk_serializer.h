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
* $Id$
*
* 
*
*/

#ifndef _MTLK_SERIALIZER_H_
#define _MTLK_SERIALIZER_H_

#include "mtlklist.h"
#include "mtlkstartup.h"
#include "mtlk_serializer_osdep.h"

/** 
*\file mtlk_serializer.h 

*\brief Tasks serializer based on "command" design pattern ideology

*\defgroup SERIALIZER Tasks serializer
*\{

   Tasks serializer provides services for commands offloading to waitable 
   single-threaded context. Commands enqueued are being executed one by one
   with respect to command priority specified by caller. Commands are cancelable,
   each command enqueued may be canceled unless its processing was already started.
   The serializer is thread safe i.e. maintains reentrability internally.

   There are two main usage scenarious for the serializer:
     -# Blocked command invocation
       - Command object is declared on stack
       - Command is being executed by mtlk_serializer_execute_blocked() API
     -# Synchronous command invocation
        - Command object is declared on stack
        - Command is being initialized
        - Command is being enqueued
        - Thread is waiting for some specific event to be signaled by completion callback
     -# Asynchronous command invocation
        - Command object is a member of some modules's structure
        - Command is being initialized in object's modules function
        - Command is being enqueued
        - Completion callback optionally notifies thread that command execution finished

\see serializer_test.c Unit tests for serializer, may be used as usage example reference.
*/

/* \cond DOXYGEN_IGNORE */

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/* \endcond */

typedef struct _mtlk_command_t mtlk_command_t;

typedef struct _mtlk_serializer_t
{
  mtlk_dlist_t*      commands;       /*!< \private Pending commands lists, 
                                                   separate list for each priority */
  uint32             num_priorities; /*!< \private Number of priorities */
  mtlk_osal_spinlock_t lock;         /*!< \private Lock for lists protection */

  mtlk_serializer_osdep_t osdep;     /*!< \private OSDEP part implementing execution context */

  mtlk_rmlock_t tasks_pending_lock;  /*!< \private Remove lock for stop operation synchronization */
  BOOL          is_running;          /*!< \private Indicates whether serializer accepts new commands */

  mtlk_command_t *current_command;          /*!< \private Command is being execuded */
  char name[IFNAMSIZ + 8];

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
  MTLK_DECLARE_INIT_LOOP(LISTS_INIT);
} __MTLK_IDATA mtlk_serializer_t;

typedef enum _serializer_result_t
{
  MTLK_CMD_EXECUTED = 0xABCD,       /*!< Command executed */
  MTLK_CMD_CANCELED,                /*!< Command canceled, command callback not invoked */
  MTLK_CMD_NONE                     /*!< Command not executed */
} serializer_result_t;

/*! Command callback prototype

    \param   user_context   Context passed on command construction.
*/
typedef void (*mtlk_cmd_clb_t)(mtlk_handle_t user_context);

/*! Command completion callback prototype

    \param   res              Command execution status, one of serializer_result_t values
    \param   command          Pointer to the command
    \param   completion_ctx   Completion context passed to mtlk_serializer_enqueue

*/
typedef void (*mtlk_cmd_completion_clb_t)(serializer_result_t res, 
                                          mtlk_command_t* command, 
                                          mtlk_handle_t completion_ctx);

struct _mtlk_command_t
{
  mtlk_cmd_clb_t       cmd_callback;             /*!< \private Pointer to function to be called    */
  mtlk_handle_t        user_context;             /*!< \private Context to be provided to callback  */

  mtlk_cmd_completion_clb_t post_exec_callback;  /*!< \private Function to be called after command
                                                    processing (optional, may be NULL)  */
  mtlk_handle_t        post_exec_callback_ctx;   /*!< \private Context to be provided to post-exec
                                                    callback */
  mtlk_ldlist_entry_t  list_entry;               /*!< \private List entry for adding to the lists
                                                    of serializer */
  BOOL                 is_in_queue;              /*!< \private Whether the command is pending in
                                                    serializer queue */
  uint32               priority;                 /*!< \private Command priority assigned on last
                                                    enqueue */
  mtlk_slid_t          issuer_slid;              /*!< \private SLID of the command issuer */
  BOOL                 is_cancelled;             /*!< \private Whether the command was cancelled */
  MTLK_DECLARE_INIT_STATUS;
} /* \cond DOXYGEN_IGNORE */ __MTLK_IDATA /* \endcond */;

/*! Construct serializer object

    \param   szr            The serializer object.
    \param   num_priorities Number of priorities.

    \return  MTLK_ERR... code
*/
int  __MTLK_IFUNC 
mtlk_serializer_init(mtlk_serializer_t *szr, const char *name, uint32 num_priorities);

/*! Destroy serializer object

    \param   szr            The serializer object.
*/
void __MTLK_IFUNC
mtlk_serializer_cleanup(mtlk_serializer_t *szr);

/*! Start serializer object

    \param   szr            The serializer object.

    \return  MTLK_ERR... code
*/
int  __MTLK_IFUNC 
mtlk_serializer_start(mtlk_serializer_t *szr);

/*! Wait for all pending commands completion and stop the serializer object

    \param   szr            The serializer object.
*/
void __MTLK_IFUNC
mtlk_serializer_stop(mtlk_serializer_t *szr);

/*! Send command to serializer for processing

    \param   szr                  Serializer object.
    \param   priority             Command priority.
    \param   command              Pointer to the command to be processed.
    \param   completion_callback  Callback to be called after command invocation 
                                  or cancellation.
    \param   completion_callback_context Context to be passed to the completion callback

    \return MTLK_ERR_NOT_READY  if serializer is stopped
    \return MTLK_ERR_BUSY       if message is already in queue
    \return MTLK_ERR_OK         in case of success
    
    \warning
    User is to ensure all enqueued commands are finished before object stop/cleanup.
    Failure to do this will lead to postmortem command execution which is guaranteed 
    kernel panic.

*/
int __MTLK_IFUNC
mtlk_serializer_enqueue(mtlk_serializer_t *szr, 
                        uint32 priority,
                        mtlk_command_t *command,
                        mtlk_cmd_completion_clb_t completion_callback,
                        mtlk_handle_t completion_callback_context);

/*! Cancel command enqueued in the serializer

    \param   szr                  Serializer object.
    \param   command              Pointer to the command to be cancelled.

    \return  TRUE  In case command was canceled successfully, command callback 
                   will never be called, completion callback will be called
                   with serializer result MTLK_CMD_CANCELED.
    \return  FALSE In case it is impossible to cancel the command because
                   it is already dequeued by the serializer for processing,
                   being processed or was processed earlier. In this case 
                   mtlk_serializer_cancel does not have any effect.

*/
BOOL __MTLK_IFUNC
mtlk_serializer_cancel(mtlk_serializer_t *szr, mtlk_command_t *command);

#define MTLK_CMD_TTL_INFINITE MTLK_OSAL_EVENT_INFINITE /*!< Specify infinite TTL for 
                                                            synchronous command execution */
#ifdef UNUSED_CODE
/*! Execute command synchronously

    \param   szr      Serializer object.
    \param   priority Command priority.
    \param   command  Pointer to the command to be executed.
    \param   ttl      Maximum time in milliseconds command may wait for processing or MTLK_CMD_TTL_INFINITE.

    \return  MTLK_ERR... code
    \return  MTLK_ERR_TIMEOUT in case of timeout (ttl expired)
    \return  MTLK_ERR_CANCELED in case command was canceled
    \return  MTLK_ERR_NOT_READY if message is already in enqueued
*/
int __MTLK_IFUNC
mtlk_serializer_execute_blocked(mtlk_serializer_t *szr,
                                uint32 priority,
                                mtlk_command_t *command,
                                uint32 ttl);
#endif /* UNUSED_CODE */
/*! Serializer enumeration callback

    \param   szr         Serializer object.
    \param   command     Command object.
    \param   is_current  TRUE if the command is currently being executed.
    \param   enum_ctx    Context to be passed to enum_clb.

    \return  TRUE to continue enumeration, FALSE otherwise.
*/
typedef BOOL (__MTLK_IFUNC * mtlk_serializer_enum_commands_f)(mtlk_serializer_t    *szr, 
                                                              const mtlk_command_t *command, 
                                                              BOOL                  is_current,
                                                              mtlk_handle_t         enum_ctx);

/*! Enumerates the currently serialized commands

    \param   szr      Serializer object.
    \param   enum_clb Callback to be called for each command serialized.
    \param   enum_ctx Context to be passed to enum_clb.

*/
#ifdef CONFIG_WAVE_DEBUG
void __MTLK_IFUNC
mtlk_serializer_enum_commands(mtlk_serializer_t               *szr,
                              mtlk_serializer_enum_commands_f  enum_clb,
                              mtlk_handle_t                    enum_ctx);
#endif

/*! Removes all serialized commands except currently executing command

    \param   szr      Serializer object.

*/
void __MTLK_IFUNC
mtlk_serializer_cancel_commands(mtlk_serializer_t *szr);

/*! Construct command object

    \param   command           Command object.
    \param   callback          Callback to be executed on command invocation.
    \param   callback_context  Context to be passed to command callback.
    \param   issuer_slid       Command's Issuer SLID

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC
mtlk_command_init(mtlk_command_t* command, 
                  mtlk_cmd_clb_t callback, 
                  mtlk_handle_t callback_context,
                  mtlk_slid_t issuer_slid);

/*! Destroy command object
    \param   command           The command object to be destroyed. */
void __MTLK_IFUNC
mtlk_command_cleanup(mtlk_command_t* command);

/*! Returns command object's priority
    \param   command           The command object 

    \return Command's Priority
*/
static __INLINE mtlk_slid_t
mtlk_command_get_priority (const mtlk_command_t* command)
{
  return command->priority;
}

/*! Returns command object's issuer SLID
    \param   command           The command object 

    \return Command's Issuer SLID
*/
static __INLINE mtlk_slid_t
mtlk_command_get_issuer_slid (const mtlk_command_t* command)
{
  return command->issuer_slid;
}

/* \cond DOXYGEN_IGNORE */

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

/* \endcond */

/*\}*/

#endif /* _MTLK_SERIALIZER_H_ */
