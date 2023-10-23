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
 * $$
 *
 * 
 *
 * Tasks serializer based on "command" design pattern
 * ideology
 *
 */
#include "mtlkinc.h"

#include "mtlk_serializer.h"

#define LOG_LOCAL_GID   GID_SERIALIZER
#define LOG_LOCAL_FID   1

MTLK_INIT_STEPS_LIST_BEGIN(serializer)
  MTLK_INIT_STEPS_LIST_ENTRY(serializer, LISTS_ALLOC)
  MTLK_INIT_STEPS_LIST_ENTRY(serializer, LISTS_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(serializer, SZR_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(serializer, SZR_OSDEP)
MTLK_INIT_INNER_STEPS_BEGIN(serializer)
MTLK_INIT_STEPS_LIST_END(serializer);

MTLK_START_STEPS_LIST_BEGIN(serializer)
  MTLK_START_STEPS_LIST_ENTRY(serializer, TASKS_PENDING_LOCK)
  MTLK_START_STEPS_LIST_ENTRY(serializer, SZR_OSDEP)
  MTLK_START_STEPS_LIST_ENTRY(serializer, START_PROCESSING)
MTLK_START_INNER_STEPS_BEGIN(serializer)
MTLK_START_STEPS_LIST_END(serializer);

int  __MTLK_IFUNC
mtlk_serializer_init(mtlk_serializer_t *szr, const char *name, uint32 num_priorities)
{
  unsigned i;

  MTLK_ASSERT(NULL != szr);
  MTLK_ASSERT(0 != num_priorities);

  memset(szr, 0, sizeof(*szr));
  wave_strcopy(szr->name, name, sizeof(szr->name));

  szr->num_priorities = num_priorities;
  szr->is_running = FALSE;

  MTLK_INIT_TRY(serializer, MTLK_OBJ_PTR(szr))
    MTLK_INIT_STEP_EX(serializer, LISTS_ALLOC, MTLK_OBJ_PTR(szr), mtlk_osal_mem_alloc,
                      (sizeof(*szr->commands)*num_priorities, MTLK_MEM_TAG_SERIALIZER),
                      szr->commands, NULL != szr->commands, MTLK_ERR_NO_MEM);
    for(i = 0; i < num_priorities; i++)
    {
      MTLK_INIT_STEP_VOID_LOOP(serializer, LISTS_INIT, MTLK_OBJ_PTR(szr), 
                               mtlk_dlist_init, (&szr->commands[i]));
    }
    MTLK_INIT_STEP(serializer, SZR_LOCK, MTLK_OBJ_PTR(szr), 
                   mtlk_osal_lock_init, (&szr->lock));
    MTLK_INIT_STEP(serializer, SZR_OSDEP, MTLK_OBJ_PTR(szr), 
                   _mtlk_serializer_osdep_init, (&szr->osdep));
  MTLK_INIT_FINALLY(serializer, MTLK_OBJ_PTR(szr))
  MTLK_INIT_RETURN(serializer, MTLK_OBJ_PTR(szr), mtlk_serializer_cleanup, (szr));
}

static void
_mtlk_serializer_lock_cleanup(mtlk_serializer_t *szr)
{
  /*
  * For some corner cases make sure the lock is released
  * before serializer cleanup!
  */
  mtlk_osal_lock_acquire(&szr->lock);
  mtlk_osal_lock_release(&szr->lock);
  mtlk_osal_lock_cleanup(&szr->lock);
}

void __MTLK_IFUNC
mtlk_serializer_cleanup(mtlk_serializer_t *szr)
{
  int i;
  MTLK_ASSERT(NULL != szr);

  MTLK_CLEANUP_BEGIN(serializer, MTLK_OBJ_PTR(szr))
    MTLK_CLEANUP_STEP(serializer, SZR_OSDEP, MTLK_OBJ_PTR(szr),
                      _mtlk_serializer_osdep_cleanup, (&szr->osdep));
    MTLK_CLEANUP_STEP(serializer, SZR_LOCK, MTLK_OBJ_PTR(szr),
                      _mtlk_serializer_lock_cleanup, (szr));
    for (i = 0; MTLK_CLEANUP_ITERATONS_LEFT(MTLK_OBJ_PTR(szr), LISTS_INIT) > 0; i++) {
      MTLK_CLEANUP_STEP_LOOP(serializer, LISTS_INIT, MTLK_OBJ_PTR(szr), 
                             mtlk_dlist_cleanup, (&szr->commands[i]));
    }
    MTLK_CLEANUP_STEP(serializer, LISTS_ALLOC, MTLK_OBJ_PTR(szr),
                      mtlk_osal_mem_free, (szr->commands));
  MTLK_CLEANUP_END(serializer, MTLK_OBJ_PTR(szr));
}

static void _mtlk_serializer_process_commands(mtlk_handle_t ctx);

static void
_mtlk_serializer_start_processing(mtlk_serializer_t *szr)
{
  mtlk_rmlock_acquire(&szr->tasks_pending_lock);

  mtlk_osal_lock_acquire(&szr->lock);
  ILOG3_V("Starting the serializer");
  szr->is_running = TRUE;
  mtlk_osal_lock_release(&szr->lock);
}

static void
_mtlk_serializer_stop_processing(mtlk_serializer_t *szr)
{
  int res;

  mtlk_osal_lock_acquire(&szr->lock);
  ILOG3_V("Stopping the serializer");  
  szr->is_running = FALSE;
  mtlk_osal_lock_release(&szr->lock);

  mtlk_rmlock_release(&szr->tasks_pending_lock);
  res = mtlk_rmlock_wait(&szr->tasks_pending_lock);
  if(MTLK_ERR_OK != res)
  {
    ELOG_D("Serializer wait failed, error %d", res);
  }
}

int  __MTLK_IFUNC
mtlk_serializer_start(mtlk_serializer_t *szr)
{
  MTLK_ASSERT(NULL != szr);

  MTLK_START_TRY(serializer, MTLK_OBJ_PTR(szr))
    MTLK_START_STEP(serializer, TASKS_PENDING_LOCK, MTLK_OBJ_PTR(szr), 
                    mtlk_rmlock_init, (&szr->tasks_pending_lock));
    MTLK_START_STEP(serializer, SZR_OSDEP, MTLK_OBJ_PTR(szr), 
                    _mtlk_serializer_osdep_start, 
                    (&szr->osdep, szr->name, _mtlk_serializer_process_commands, HANDLE_T(szr)));
    MTLK_START_STEP_VOID(serializer, START_PROCESSING, MTLK_OBJ_PTR(szr), 
                         _mtlk_serializer_start_processing,(szr));
  MTLK_START_FINALLY(serializer, MTLK_OBJ_PTR(szr))
  MTLK_START_RETURN(serializer, MTLK_OBJ_PTR(szr), mtlk_serializer_stop, (szr));
}

void  __MTLK_IFUNC
mtlk_serializer_stop(mtlk_serializer_t *szr)
{
  MTLK_ASSERT(NULL != szr);

  MTLK_STOP_BEGIN(serializer, MTLK_OBJ_PTR(szr))
    MTLK_STOP_STEP(serializer, START_PROCESSING, MTLK_OBJ_PTR(szr),
                   _mtlk_serializer_stop_processing, (szr));
    MTLK_STOP_STEP(serializer, SZR_OSDEP, MTLK_OBJ_PTR(szr),
                   _mtlk_serializer_osdep_stop, (&szr->osdep));
    MTLK_STOP_STEP(serializer, TASKS_PENDING_LOCK, MTLK_OBJ_PTR(szr), 
                   mtlk_rmlock_cleanup, (&szr->tasks_pending_lock));
  MTLK_STOP_END(serializer, MTLK_OBJ_PTR(szr));
}

MTLK_INIT_STEPS_LIST_BEGIN(command)

MTLK_INIT_INNER_STEPS_BEGIN(command)
MTLK_INIT_STEPS_LIST_END(command);

int __MTLK_IFUNC
mtlk_command_init(mtlk_command_t* command, 
                  mtlk_cmd_clb_t callback, 
                  mtlk_handle_t callback_context,
                  mtlk_slid_t issuer_slid)
{
  MTLK_ASSERT(NULL != command);
  MTLK_ASSERT(NULL != callback);

  MTLK_INIT_TRY(command, MTLK_OBJ_PTR(command))

  MTLK_INIT_FINALLY(command, MTLK_OBJ_PTR(command))
    command->cmd_callback = callback;
    command->user_context = callback_context;
    command->is_in_queue = FALSE;
    command->issuer_slid = issuer_slid;
    command->is_cancelled = FALSE;
  MTLK_INIT_RETURN(command, MTLK_OBJ_PTR(command), mtlk_command_cleanup, (command));
}

void __MTLK_IFUNC
mtlk_command_cleanup(mtlk_command_t* command)
{
  MTLK_ASSERT(NULL != command);
  MTLK_ASSERT(!command->is_in_queue);

  MTLK_CLEANUP_BEGIN(command, MTLK_OBJ_PTR(command))

  MTLK_CLEANUP_END(command, MTLK_OBJ_PTR(command));
}

int __MTLK_IFUNC
mtlk_serializer_enqueue(mtlk_serializer_t *szr, 
                        uint32 priority,
                        mtlk_command_t *command,
                        mtlk_cmd_completion_clb_t completion_callback,
                        mtlk_handle_t completion_callback_context)
{
  int res = MTLK_ERR_UNKNOWN;
  
  MTLK_ASSERT(NULL != szr);
  MTLK_ASSERT(priority < szr->num_priorities);
  MTLK_ASSERT(NULL != command);
  MTLK_ASSERT(NULL != completion_callback);

  mtlk_osal_lock_acquire(&szr->lock);

  if(!szr->is_running)
  {
    ELOG_V("Serializer not yet running");
    res = MTLK_ERR_NOT_READY;
  }
  else if(command->is_in_queue)
  {
    ELOG_V("Command already in the queue");    
    res = MTLK_ERR_BUSY;
  }
  else
  {
    mtlk_dlist_push_back(&szr->commands[priority], &command->list_entry);
    command->is_in_queue = TRUE;
    command->priority = priority;
    command->post_exec_callback = completion_callback;
    command->post_exec_callback_ctx = completion_callback_context;

    mtlk_rmlock_acquire(&szr->tasks_pending_lock);
    _mtlk_serializer_osdep_notify(&szr->osdep);
    res = MTLK_ERR_OK;
  }

  mtlk_osal_lock_release(&szr->lock);

  return res;
}

BOOL __MTLK_IFUNC
mtlk_serializer_cancel(mtlk_serializer_t *szr, mtlk_command_t *command)
{
  BOOL was_removed = FALSE;

  MTLK_ASSERT(NULL != szr);
  MTLK_ASSERT(NULL != command);

  mtlk_osal_lock_acquire(&szr->lock);
  if(command->is_in_queue)
  {
    mtlk_dlist_remove(&szr->commands[command->priority], &command->list_entry);
    command->is_in_queue = FALSE;
    mtlk_rmlock_release(&szr->tasks_pending_lock);
    was_removed = TRUE;
  }
  mtlk_osal_lock_release(&szr->lock);

  if(was_removed)
  {
    command->post_exec_callback(MTLK_CMD_CANCELED, command,
                                command->post_exec_callback_ctx);
    return TRUE;
  }

  return FALSE;
}

static void _mtlk_serializer_invoke_command(mtlk_command_t* command)
{
  command->cmd_callback(command->user_context);
  command->post_exec_callback(MTLK_CMD_EXECUTED, command,
                              command->post_exec_callback_ctx);
}

static void _mtlk_serializer_process_commands(mtlk_handle_t ctx)
{
  mtlk_serializer_t *szr = (mtlk_serializer_t *) ctx;
  BOOL queues_are_empty;
  int i;

  MTLK_ASSERT(NULL != szr);
  MTLK_ASSERT(NULL == szr->current_command);
  
  do {
    queues_are_empty = TRUE;

    for(i = szr->num_priorities - 1; i >= 0; i--)
    {
      mtlk_dlist_entry_t* pentry;

      mtlk_osal_lock_acquire(&szr->lock);

      if(NULL != (pentry = mtlk_dlist_pop_front(&szr->commands[i])))
      {
        szr->current_command = 
          MTLK_LIST_GET_CONTAINING_RECORD(pentry, mtlk_command_t, list_entry);
        szr->current_command->is_in_queue = FALSE;
        mtlk_rmlock_release(&szr->tasks_pending_lock);
        queues_are_empty = FALSE;
      }

      mtlk_osal_lock_release(&szr->lock);

      if(szr->current_command)
      {
        _mtlk_serializer_invoke_command(szr->current_command);
        mtlk_osal_lock_acquire(&szr->lock);
        szr->current_command = NULL;
        mtlk_osal_lock_release(&szr->lock);
        break;
      }
    }

  } while(!queues_are_empty);
}

typedef struct _szr_execute_blocked_cmpl_ctx_t
{
  mtlk_osal_event_t completion_event;
  volatile serializer_result_t szr_result;
} szr_execute_blocked_cmpl_ctx_t;
#ifdef UNUSED_CODE
static void 
_szr_execute_blocked_cmpl_clb(serializer_result_t res, 
                              mtlk_command_t* command, 
                              mtlk_handle_t completion_ctx)
{
  szr_execute_blocked_cmpl_ctx_t* ctx 
    = (szr_execute_blocked_cmpl_ctx_t*) completion_ctx;

  MTLK_ASSERT(NULL != ctx);
  MTLK_UNREFERENCED_PARAM(command);

  ctx->szr_result = res;
  mtlk_osal_event_set(&ctx->completion_event);
}

int __MTLK_IFUNC
mtlk_serializer_execute_blocked(mtlk_serializer_t *szr,
                                uint32 priority,
                                mtlk_command_t *command,
                                uint32 ttl)
{
  int res;
  szr_execute_blocked_cmpl_ctx_t completion_ctx;

  MTLK_ASSERT(NULL != szr);
  MTLK_ASSERT(NULL != command);

  completion_ctx.szr_result = MTLK_CMD_NONE;
  res = mtlk_osal_event_init(&completion_ctx.completion_event);
  if(MTLK_ERR_OK != res)
    return res;

  res = mtlk_serializer_enqueue(szr, priority, command, 
                                _szr_execute_blocked_cmpl_clb, HANDLE_T(&completion_ctx));

  if(MTLK_ERR_OK != res)
  {
    mtlk_osal_event_cleanup(&completion_ctx.completion_event);
    return res;
  }

  res = mtlk_osal_event_wait(&completion_ctx.completion_event, ttl);

  if(MTLK_ERR_OK == res)
  {
    /* Command processing finished */
    mtlk_osal_event_cleanup(&completion_ctx.completion_event);
    return (MTLK_CMD_EXECUTED == completion_ctx.szr_result) ? MTLK_ERR_OK : MTLK_ERR_CANCELED;
  }

  if(MTLK_ERR_TIMEOUT == res)
  {
    /* Timeout occurred */
    if(mtlk_serializer_cancel(szr, command))
    {
      /* Command canceled successfully */
      mtlk_osal_event_cleanup(&completion_ctx.completion_event);
      return MTLK_ERR_TIMEOUT;
    }
    
    /* Failed to cancel the command, i.e. command is being processed by serializer. */
    /* Wait for completion.                                                         */
    res = mtlk_osal_event_wait(&completion_ctx.completion_event, MTLK_OSAL_EVENT_INFINITE);
  }

  mtlk_osal_event_cleanup(&completion_ctx.completion_event);

  if(MTLK_ERR_OK == res)
    return (MTLK_CMD_EXECUTED == completion_ctx.szr_result) ? MTLK_ERR_OK : MTLK_ERR_CANCELED;
  else
    return res;
}
#endif /* UNUSED_CODE */
#ifdef CONFIG_WAVE_DEBUG
void __MTLK_IFUNC
mtlk_serializer_enum_commands (mtlk_serializer_t               *szr,
                               mtlk_serializer_enum_commands_f  enum_clb,
                               mtlk_handle_t                    enum_ctx)
{
  BOOL continue_enum = TRUE;
  int  i;

  MTLK_ASSERT(szr != NULL);
  MTLK_ASSERT(enum_clb != NULL);

  mtlk_osal_lock_acquire(&szr->lock);
  if (szr->current_command) {
    continue_enum = enum_clb(szr, szr->current_command, TRUE, enum_ctx);
  }

  for(i = szr->num_priorities - 1; continue_enum && i >= 0; i--) {
    mtlk_dlist_entry_t* head;
    mtlk_dlist_entry_t* pentry;
    mtlk_dlist_foreach(&szr->commands[i], pentry, head) {
      mtlk_command_t *command = 
        MTLK_LIST_GET_CONTAINING_RECORD(pentry, mtlk_command_t, list_entry);
      continue_enum = enum_clb(szr, command, FALSE, enum_ctx);
      if (!continue_enum) {
        break;
      }
    }
  }
  mtlk_osal_lock_release(&szr->lock);
}
#endif

void __MTLK_IFUNC
mtlk_serializer_cancel_commands (mtlk_serializer_t *szr)
{
  int i;

  MTLK_ASSERT(szr != NULL);

  mtlk_osal_lock_acquire(&szr->lock);

  /* Cancel all commands except currently executing command */
  for(i = szr->num_priorities - 1; i >= 0; i--) {
    mtlk_dlist_entry_t* head;
    mtlk_dlist_entry_t* pentry;
    mtlk_dlist_foreach(&szr->commands[i], pentry, head) {
      mtlk_command_t *command =
        MTLK_LIST_GET_CONTAINING_RECORD(pentry, mtlk_command_t, list_entry);

      MTLK_ASSERT(NULL != command);
      MTLK_ASSERT(command != szr->current_command);

      command->is_cancelled = TRUE;
    }
  }
  mtlk_osal_lock_release(&szr->lock);
}

