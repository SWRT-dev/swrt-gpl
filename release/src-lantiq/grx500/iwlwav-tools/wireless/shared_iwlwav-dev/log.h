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
 *
 * 
 *
 * Written by: Andriy Fidrya
 *
 */

#ifndef __LOG_H__
#define __LOG_H__

/* RTLOGGER compilation time options (IWLWAV_RTLOG_FLAGS) */
#define RTLF_REMOTE_ENABLED  0x00000001
#define RTLF_CONSOLE_ENABLED 0x00000002

/* RTLOGGER special logs compilation control Debug Levels 
   Used to enable/disable logs by debug level in compile time 
   (IWLWAV_RTLOG_MAX_DLEVEL)
*/
#define IWLWAV_RTLOG_SILENT_DLEVEL  (-4)
#define IWLWAV_RTLOG_ERROR_DLEVEL   (-3)
#define IWLWAV_RTLOG_WARNING_DLEVEL (-2)
#define IWLWAV_RTLOG_RELEASE_DLEVEL (-1)
#define IWLWAV_RTLOG_INFO_DLEVEL    ( 0)

#ifndef IWLWAV_RTLOG_FLAGS /* combination of RTLF_... flags */
#error IWLWAV_RTLOG_FLAGS must be defined!
#endif

#ifndef IWLWAV_RTLOG_MAX_DLEVEL
#error IWLWAV_RTLOG_MAX_DLEVEL must be defined!
#endif

#ifdef FMT_NO_ATTR_CHK
#define __attribute__(x)
#endif

#define LOG_TARGET_CONSOLE (1 << 0)
#define LOG_TARGET_REMOTE  (1 << 1)
#define LOG_TARGET_CAPWAP  (1 << 2)

/*	Driver defined __MTLK_IFUNC outside the scope of shared files,
	so when it's not defined - as is the case in FW - we need to 
	define it as white space.
 */
#ifndef __MTLK_IFUNC
#define __MTLK_IFUNC
#endif

// ---------------
// Data structures
// ---------------

typedef struct _mtlk_log_buf_entry_t mtlk_log_buf_entry_t;

typedef struct _mtlk_log_multi_buffer_t
{
  mtlk_log_buf_entry_t *buf;
  uint8                *data;
  uint16                data_size;
} mtlk_log_multi_buffer_t;

// --------------------
// Functions and macros
// --------------------
#ifdef CONFIG_WAVE_RTLOG_REMOTE
/*!
 * \fn      BOOL mtlk_log_check_version(uint16 major, uint16 minor)
 * \brief   Check logger version.
 *
 * Checks logger version of a component that wants to log using this log driver.
 *
 * \return TRUE if verion is supported, FALSE otherwise.
 */
extern BOOL mtlk_log_check_version(uint16 major, uint16 minor);
#endif

/*!
 * \fn      int mtlk_log_get_flags(int level, int oid, int gid)
 * \brief   Query log settings.
 *
 * Queries log settings for level, gid and sid combination.
 *
 * \return bitmask specifying log targets.
 *         Bitmask is 0 if logger is not initialized or no log targets
 *         are set.
 *         LOG_TARGET_CONSOLE - console logging requested.
 *         LOG_TARGET_REMOTE - remote logging requested.
 *         LOG_TARGET_CAPWAP - capture sniffer requested.
 */
extern int mtlk_log_get_flags(int level, int oid, int gid);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/*!
 * \fn      mtlk_log_buf_entry_t *mtlk_log_new_pkt_reserve(uint32 pkt_size,
            uint8 **ppdata)
 * \brief   Reserve space for a new packet in active buffer.
 *
 * Returns a pointer to active buffer (as return value) and a pointer to
 * beginning of free space in this buffer (ppdata).
 *
 * Caller writes packet contents to ppdata, then releases the buffer by
 * calling mtlk_log_new_pkt_release.
 *
 * Caller must silently ignore errors; reporting is done by
 * mtlk_log_new_pkt_reserve function itself via CERROR.
 *
 * Implementation details:
 *  - If enough space is available in current active buffer, it's refcount
 *    is increased by 1 and size by pkt_size. Buffer is protected by refcount
 *    so it won't move to ready buffers pool before packet creation is
 *    complete.
 *  - If not enough space is available and active buffer's refcount is 0,
 *    sends active buffer to ready buffers pool and requests another free
 *    buffer.
 *  - If not enough space is available, but active buffer's refcount is
 *    greater than 0, requests another free buffer, then replaces pointer
 *    to active buffer with a pointer to the newly obtained buffer.
 *    Note: in this case old buffer will not be lost:
 *    mtlk_log_new_pkt_release will move it to ready buffers pool after
 *    refcount reaches zero.
 *
 * \return pointer to a buffer on success, ppdata points to beginning of
 *         data.
 * \return NULL if error has occured. Do not report this error to user.
 */
extern mtlk_log_buf_entry_t *mtlk_log_new_pkt_reserve(uint32 pkt_size,
    uint8 **ppdata);

/*!
 * \fn      void __MTLK_IFUNC mtlk_log_new_pkt_release(mtlk_log_buf_entry_t *buf)
 * \brief   Release a buffer captured with mtlk_log_new_pkt_reserve call.
 *
 * Implementation details:
 *  - Decreases buf's refcount. After this:
 *  - If refcount is zero and buf is active buffer, does nothing (this buffer
 *    still can accept data as active buffer).
 *  - If refcount is zero, but buf is not an active buffer, sends the buffer
 *    to ready buffers pool. Note that the buffer was already invalidated by
 *    mtlk_log_new_pkt_reserve.
 *  - If refcount is not zero, no more actions are taken.
 */
extern void mtlk_log_new_pkt_release(mtlk_log_buf_entry_t *buf);
#endif

#include "logdefs.h"
#include "formats.h"
#include "logmacro_mixins.h"
#include "loggroups.h"

#ifdef CONFIG_WAVE_RTLOG_REMOTE
extern BOOL mtlk_log_new_pkt_reserve_multi(uint32 pkt_size, mtlk_log_multi_buffer_t **ppdata);
extern void mtlk_log_new_pkt_release_multi(mtlk_log_multi_buffer_t *ppdata);

extern void __MTLK_IFUNC
mtlk_ndev_send_capwap_packet(uint8 hwid, void *data, size_t data_len, uint8 is_wifi, int rssi);
#endif
/* There is a strange behaviour of GCC compiler */
/* when it is used to generate source file      */
/* dependencies. It ignores compilation errors  */
/* like "unknown symbol/function/macros" but    */
/* does not ignore "incorrect number of         */
/* parameters passed to function". When user    */
/* changes number of parameters of printout     */
/* without changing the macro itself GCC fails  */
/* to generate list of dependencies and         */
/* and without list of dependencies LOGPREP can */
/* not update the MACRO used in printout being  */
/* changed. The workaround is to hide RTLOGGER  */
/* macro definitions during dependencies        */
/* generation, so GCC will be happy silently    */
/* ignoring "undefined function" error.         */

#ifndef __MTLK_DEPENDENCY_GENERATION_PATH

#include "logmacros.h"
#ifdef CPTCFG_IWLWAV_RT_LOGGER_INLINES
#include "logmacros.c"
#endif

#endif /* __MTLK_DEPENDENCY_GENERATION_PATH */

#endif /* __LOG_H__ */
