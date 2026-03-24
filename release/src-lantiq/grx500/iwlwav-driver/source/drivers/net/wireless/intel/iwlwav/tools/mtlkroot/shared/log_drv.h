/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
******************************************************************************/

/*
 *
 * 
 *
 * Written by: Andriy Fidrya
 *
 */

#ifndef __LOG_DRV_H__
#define __LOG_DRV_H__

#include "mtlklist.h"
#include "mtlk_osal.h"
#include <linux/seq_file.h>

#ifdef FMT_NO_ATTR_CHK
#define __attribute__(x)
#endif

#define LOG_TARGET_CONSOLE (1 << 0)
#define LOG_TARGET_REMOTE  (1 << 1)
#define LOG_TARGET_CAPWAP  (1 << 2)

#define LOG_CONSOLE_ON(flags) (((flags) & LOG_TARGET_CONSOLE) != 0)
#define LOG_REMOTE_ON(flags)  (((flags) & LOG_TARGET_REMOTE) != 0)
#define LOG_CAPWAP_ON(flags)  (((flags) & LOG_TARGET_CAPWAP) != 0)

#define LOG_DLVL_DISABLE    IWLWAV_RTLOG_WARNING_DLEVEL

/* #define LOG_DEFAULT_CDEBUG  IWLWAV_RTLOG_WARNING_DLEVEL */
#define LOG_DEFAULT_CDEBUG    IWLWAV_RTLOG_INFO_DLEVEL
#define LOG_DEFAULT_RDEBUG    IWLWAV_RTLOG_INFO_DLEVEL
#define LOG_DEFAULT_CAPDEBUG  IWLWAV_RTLOG_INFO_DLEVEL

#define LOG_DLVL_MIN    IWLWAV_RTLOG_SILENT_DLEVEL
#define LOG_DLVL_MAX    (9)

#define LOG_MAX_DEBUG_OID   ((MAX_OID) - 1)
#define LOG_MAX_DEBUG_MODE  3 /* cdebug=1, rdebug=2, capdebug=3 */

// ---------------
// Data structures
// ---------------

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

extern int rdebug;
extern int cdebug;
extern int bufs_no;
extern int buf_size;
extern int buf_swap_tmout;

// --------------------
// Internal definitions
// --------------------

typedef struct _mtlk_log_level_t
{
    int8 console;
    int8 remote;
    int8 capwap;
} mtlk_log_level_t;

typedef struct _mtlk_log_data_t
{
  mtlk_osal_spinlock_t   data_lock;
  mtlk_log_level_t       dbg_level[MAX_OID][MAX_GID];

#ifdef CONFIG_WAVE_RTLOG_REMOTE
  // Never changes until shutdown, so requires no locking
  int total_buf_count;       // Total buffers count
  mtlk_osal_timer_t timer;   // Buffer swap timer

  // Implementation should use data_lock when accessing these fields
  mtlk_osal_spinlock_t   timer_lock;
  mtlk_log_buf_entry_t  *unsafe_active_buf; /* Driver's active buffer */

  mtlk_dlist_t           bufs_free;       // Free buffer pool
  mtlk_osal_spinlock_t   bufs_free_lock;
  mtlk_dlist_t           bufs_ready;      // Ready buffer pool
  mtlk_osal_spinlock_t   bufs_ready_lock;

  mtlk_rmlock_t          buff_rm_lock;    // Remove lock that tracks used buffers

  /* Logging statistics */
  uint32 log_buff_allocations_succeeded;
  uint32 log_buff_allocations_failed;
  uint32 log_pkt_reservations_succeeded;
  uint32 log_pkt_reservations_failed;

  MTLK_DECLARE_INIT_LOOP(FREE_BUFFS);
#endif

  MTLK_DECLARE_INIT_STATUS;

} __MTLK_IDATA mtlk_log_data_t;

// --------------------
// Functions and macros
// --------------------

/*!
 * \fn      int mtlk_log_init(void)
 * \brief   Initialize logger
 *
 * Call this function on startup before any logging is done.
 * Errors must be reported to user via CERROR. Errors aren't fatal.
 *
 * Implementation details:
 *  - Initializes log_data global variable.
 *  - Allocates buffers for Lower MAC, Upper MAC and Driver and
 *    adds them to free buffers pool.
 *  - Gets a free buffer from pool and makes it active.
 *  - Initializes buffer swap timer and schedules it for execution.
 *
 *  \return MTLK_ERR_OK     all buffers were succesfully allocated.
 *  \return MTLK_ERR_NO_MEM failed to allocate one or more buffers or assign
 *                          an active buffer.
 */
int __MTLK_IFUNC mtlk_log_init(void);

/*!
 * \fn      void mtlk_log_cleanup(void)
 * \brief   Cleanup logger.
 *
 * Call this function on cleanup when logging is no longer required.
 *
 * Implementation details:
 *  - Waits until all allocated buffers are returned into free buffers pool
 *    then frees them.
 *  - Releases all logger resources.
 */
void __MTLK_IFUNC mtlk_log_cleanup(void);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/*!
 * \fn      void wave_log_before_cleanup(void)
 * \brief   Logger's pre-cleanup.
 *
 * Called to make preparation steps for logger's cleanup.
 */
void __MTLK_IFUNC wave_log_before_cleanup(void);


void __MTLK_IFUNC rtlog_before_cleanup(void);
void __MTLK_IFUNC rtlog_on_cleanup(void);
int  __MTLK_IFUNC rtlog_on_init(void);
void __MTLK_IFUNC mtlk_log_get_rtlog (struct seq_file *seq);
void __MTLK_IFUNC mtlk_log_set_rtlog (char *conf);
#endif

/*!
 * \fn      void mtlk_log_set_cdebug_conf(int oid, int cdebug)
 * \brief   Set debug levels for console logging for oid
 *
 * \param   oid      OID
 * \param   rdebug   default debug level for console
 *
 * Set debug levels for console logging
 *
 */
void __MTLK_IFUNC mtlk_log_set_cdebug_conf(int oid, int cdebug);

/*!
 * \fn      void mtlk_log_set_rdebug_conf(int oid, int rdebug)
 * \brief   Set debug levels for remote logging for oid
 *
 * \param   oid      OID
 * \param   rdebug   default debug level for remote
 *
 * Set debug levels for remote logging
 *
 */
void __MTLK_IFUNC mtlk_log_set_rdebug_conf(int oid, int rdebug);

/*!
 * \fn      void mtlk_log_set_defconf(int cdebug, int rdebug)
 * \brief   Set default debug levels for console and remote logging
 *
 * \param   cdebug   default debug level for console
 * \param   rdebug   default debug level for remote
 *
 * Set default debug levels for console and remote logging
 *
 */
void __MTLK_IFUNC mtlk_log_set_defconf(int cdebug, int rdebug);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/*!
 * \fn      mtlk_log_buf_entry_t *mtlk_log_get_free_buf(void)
 * \brief   Get buffer from free buffers pool.
 *
 * Caller is a new owner of the buffer returned and must return it to one
 * of the pools.
 *
 * If no buffers are available, caller must report this to user via CERROR.
 *
 * \return buffer from free buffers pool if available, NULL otherwise
 */
mtlk_log_buf_entry_t * __MTLK_IFUNC mtlk_log_get_free_buf(void);

/*!
 * \fn      mtlk_log_buf_entry_t *mtlk_log_get_ready_buf(void)
 * \brief   Get buffer from ready buffers pool.
 *
 * Caller is a new owner of the buffer returned and must return it to one
 * of the pools.
 *
 * \return buffer from ready buffers pool if available, NULL otherwise
 */
mtlk_log_buf_entry_t * __MTLK_IFUNC mtlk_log_get_ready_buf(void);

/*!
 * \fn      int mtlk_log_put_free_buf(mtlk_log_buf_entry_t *buf)
 * \brief   Puts buffer to free buffers pool.
 *
 * Caller's buffer ownership will be relinquished on success.
 *
 * Errors must be reported to user via normal logging methods.
 *
 * \return MTLK_ERR_OK      success.
 * \return MTLK_ERR_NO_MEM  error has occured while adding buffer to pool.
 */
int __MTLK_IFUNC mtlk_log_put_free_buf(mtlk_log_buf_entry_t *buf);

/*!
 * \fn      void mtlk_log_put_ready_buf(mtlk_log_buf_entry_t *buf)
 * \brief   Puts buffer to ready buffers pool.
 *
 * Caller's buffer ownership will be relinquished on success.
 *
 * Errors must be reported to user via normal logging methods.
 */
void __MTLK_IFUNC mtlk_log_put_ready_buf(mtlk_log_buf_entry_t *buf);
#endif

/*!
 * \fn      void mtlk_log_set_conf(const char *conf)
 * \brief   Configure log filters.
 *
 * \param   conf   Configuration string.
 *
 * Parses configuration string and configures log filters.
 *
 * Configuration string contains one or more tokens of form:
 * "<+/-><gid>[r<rd>][c<cd>]"
 * NB! blank configuration line switches all debug off
 * Space separators between tokens are optional.
 * '+' enables logging of messages with gid <gid>
 * '-' disables logging of messages with gid <gid>
 * gid is numeric value (GID).
 * 'r' specified remote logging.
 * 'rd' debug level for the remote target
 * 'c' specified console logging.
 * 'cd' debug level for the console logging
 * If neither 'r' nor 'c' is specified, both ("rc") are assumed.
 * For example:
 *   "-114 +115r3c1 +112c1"
 * Disables remote and console logging for GID 114.
 * Enables remote and console logging for GID 115: remote
 *   w/ debug level 5 and console w/ 1
 * Enables console logging for gid 112 w/ debug level 1
 *
 * Syntax errors are reported using normal logging methods so when
 * both console and remote logging is disabled user won't see anything
 * on a console in response to bad configuration string.
 *
 * By default both remote and console logging is enabled for all GIDs.
 */
void __MTLK_IFUNC mtlk_log_set_conf(const char *conf);

/*!
 * \fn      int mtlk_log_get_conf(char *buffer, int size)
 * \brief   Query all logger settings.
 *
 * \param   buffer   buffer to place configuration
 * \param   size     size of the buffer
 *
 * Queries all logger settings
 */
void __MTLK_IFUNC mtlk_log_get_conf(struct seq_file *seq);

/*!
 * \fn      int mtlk_log_on_init(void)
 * \brief   Platform-specific logger initialization.
 *
 * Implementation is platform-specific.
 * Called on logger initialization to setup communication with userspace if
 * needed (for example, open netlink sockets in Linux etc).
 *
 * \return MTLK_ERR_OK  success.
 * \return < 0          failure.
 */
int __MTLK_IFUNC mtlk_log_on_init(void);

/*!
 * \fn      void mtlk_log_on_cleanup(void)
 * \brief   Platform-specific logger cleanup.
 *
 * Implementation is platform-specific.
 * Called to abort any pending userspace operations and return buffers to
 * free buffers pool.
 *
 * \return MTLK_ERR_OK  success.
 * \return < 0          failure.
 */
void __MTLK_IFUNC mtlk_log_on_cleanup(void);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/*!
 * \fn      int mtlk_log_on_buf_ready(void)
 * \brief   Notification of new buffer in ready pool.
 *
 * Implementation is platform-specific.
 * Called when new buffer appears in ready pool.
 *
 * Implementation details:
 *  - take buffer from ready pool using mtlk_log_get_ready_buf.
 *  - after sending buffer contents, return buffer to free buffers pool.
 *
 * Log module will log all errors reported by this function using normal
 * logging methods.
 *
 * \return MTLK_ERR_OK      success.
 * \return MTLK_ERR_NO_MEM  error has occured during processing of buffer
 *                          data.
 */
int __MTLK_IFUNC mtlk_log_on_buf_ready(void);

/*!
 * \fn      void mtlk_log_parse_notification_msg(void *param, void *packet)
 * \brief   Parse notification message from user's space application.
 *
 * \param   param   pointer to parameters (optional)
 * \param   packet  pointer to packet payload
 *
 * Implementation is platform-specific.
 * Called when either Startup or Cleanup message is received.
 *
 * Implementation details:
 *  - three messages are expected from application - Startup, Cleanup and Log.
 *  - in case of Startup message:
 *      get OID using incoming application's PID,
 *      register application,
 *      send OID and actual LOG-configuration to application.
 *  - in case of Cleanup message:
 *      unregister application.
 *  - in case of Log message:
 *      forward message to network device.
 *
 * Log module will log all errors reported by this function using normal
 * logging methods.
 *
 * \return none.
 */
void __MTLK_IFUNC mtlk_log_parse_notification_msg(void *param, void *packet);
#endif

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __LOG_DRV_H__ */
