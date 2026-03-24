/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __CPU_STAT_H__
#define __CPU_STAT_H__

#ifdef CPTCFG_IWLWAV_CPU_STAT
/*! 
  \file  cpu_stat.h
  \brief CPUStat module interface
*/

#include "mtlk_osal.h"
#include "mtlk_dbg.h"

#define LOG_LOCAL_GID   GID_CPU_STAT
#define LOG_LOCAL_FID   0

/*! 
  \enum   _cpu_stat_track_id_e
  \brief  IDs of existing code tracks.
*/
typedef enum
{
  CPU_STAT_ID_NONE = -1, /*!<  STAT disabled */
  CPU_STAT_ID_ISR,
  CPU_STAT_ID_TX_OS,
  CPU_STAT_ID_TX_FWD,
  CPU_STAT_ID_TX_SQ_FLUSH,
  CPU_STAT_ID_TX_FLUSH,
  CPU_STAT_ID_TX_HW,
  CPU_STAT_ID_RX_DAT,
  CPU_STAT_ID_RX_MM,
  CPU_STAT_ID_RX_EMPTY,
  CPU_STAT_ID_RX_HW,
  CPU_STAT_ID_TX_CFM,
  CPU_STAT_ID_RX_LOG,
  CPU_STAT_ID_TX_MM_CFM,
  CPU_STAT_ID_RX_BSS,
  CPU_STAT_ID_TX_BSS,
  CPU_STAT_ID_RX_MGMT_BEACON,
  CPU_STAT_ID_RX_MGMT_ACTION,
  CPU_STAT_ID_RX_MGMT_OTHER,
  CPU_STAT_ID_RX_CTL,
  CPU_STAT_ID_LAST
} cpu_stat_track_id_e;

/*! 
  \struct  cpu_stat_node_t
  \brief   Structure for collecting CPU statistics for one track.
*/
typedef struct _cpu_stat_node_t
{
  uint32          count;      /*!< counter of iterations done */
  uint64          total;      /*!< sum of measured per-iteration times */
  uint64          peak;       /*!< maximal iteration time */
  uint32          peak_sn;    /*!< peak's sequence number */
  uint32          usec_delay; /*!< delay in milliseconds to be emitted into track */
} cpu_stat_node_t;

/*! 
  \struct  cpu_stat_t
  \brief   Module's object. 

  \warning This structure is for internal usage only and must not be changed outside the module.
*/
typedef struct _cpu_stat_t
{
  cpu_stat_node_t      tracks[CPU_STAT_ID_LAST]; /*!< array of per-track statisticts */
  mtlk_dbg_hres_ts_t   current_start;            /*!< current track's iteration start time */
  BOOL                 timestamp_valid;          /*!< internal flag specifying whether timestamp field holds valid timestamp */
  cpu_stat_track_id_e  current_id;               /*!< ID of the track we are measuring now */
  cpu_stat_track_id_e  enabled_id;               /*!< ID of enabled track */
  mtlk_osal_spinlock_t lock;                     /*!< Object's spinlock */
} cpu_stat_t;

extern cpu_stat_t g_mtlk_cpu_stat_obj;

/******************************************************************************
 * Auxiliary internal defines
 ******************************************************************************/
#define __CPU_STAT_ID_VALID(id)         ((id) > CPU_STAT_ID_NONE && (id) < CPU_STAT_ID_LAST)
#define __CPU_STAT_ID_ENABLED(id)       (__CPU_STAT_ID_VALID(id) && g_mtlk_cpu_stat_obj.enabled_id == (id))

/*! 
  \fn      void __CPU_STAT_WAIT(uint32 usec_delay)
  \brief   Busy-wait for given number of microseconds based on hi-res timer.

  \param   usec-delay Number of milliseconds to wait

  \warning This function is for internal usage only and must not be used outside the module.
*/
static inline void
__CPU_STAT_WAIT(uint32 usec_delay)
{
  mtlk_dbg_hres_ts_t wait_start, now;

  if(0 == usec_delay)
    return;

  mtlk_dbg_hres_ts(&wait_start);

  do
  {
    mtlk_dbg_hres_ts(&now);
  }
  while( mtlk_dbg_hres_diff_uint64(&now, &wait_start) < mtlk_dbg_hres_us_to_uint64(usec_delay) );
}

/*! 
  \fn      void __CPU_STAT_END(cpu_stat_track_id_e id,
                               struct timeval     *start)
  \brief   Finalizes the measurement: takes the final timestamp, calculates diff and 
           updates the suitable track's node.

  \param   id    ID of track to finalize
  \param   start pointer to start timestamp

  \warning This function is for internal usage only and must not be used outside the module.
*/
static inline void
__CPU_STAT_END (cpu_stat_track_id_e id,
                mtlk_dbg_hres_ts_t  *start)
{
  cpu_stat_node_t   *node;
  mtlk_dbg_hres_ts_t now;
  uint64 diff;
  mtlk_handle_t         lock_val;

  /* Important:                                                */
  /*   always get NOW timestamp before ifs and calculations to */
  /*   avoid affecting the result                              */
  mtlk_dbg_hres_ts(&now);

  node = &g_mtlk_cpu_stat_obj.tracks[id];

  if(!g_mtlk_cpu_stat_obj.timestamp_valid)
    return;

  g_mtlk_cpu_stat_obj.timestamp_valid = FALSE;

  diff = mtlk_dbg_hres_diff_uint64(&now, start);

  lock_val = mtlk_osal_lock_acquire_irq(&g_mtlk_cpu_stat_obj.lock);
  if (diff > node->peak) {
    node->peak    = diff;
    node->peak_sn = node->count;
  }

  node->total += diff;
  node->count ++;

  __CPU_STAT_WAIT(node->usec_delay);

  mtlk_osal_lock_release_irq(&g_mtlk_cpu_stat_obj.lock, lock_val);
}

/*! 
  \fn      void _CPU_STAT_GET_DATA(cpu_stat_track_id_e id,
                                   cpu_stat_node_t    *stat)
  \brief   Fills the stat structure with the suitable track's data.

  \param   id    track ID
  \param   stat  pointer to structure to fill

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   

*/
static inline void
_CPU_STAT_GET_DATA (cpu_stat_track_id_e id,
                    cpu_stat_node_t    *stat)
{
  mtlk_handle_t         lock_val;
  MTLK_ASSERT(__CPU_STAT_ID_VALID(id));

  lock_val = mtlk_osal_lock_acquire_irq(&g_mtlk_cpu_stat_obj.lock);
  *stat = g_mtlk_cpu_stat_obj.tracks[id];
  mtlk_osal_lock_release_irq(&g_mtlk_cpu_stat_obj.lock, lock_val);
}

/*! 
  \fn      const char* _CPU_STAT_GET_NAME(cpu_stat_track_id_e id);
  \brief   Returns track's symbolic name

  \param   id    track ID

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
const char*
_CPU_STAT_GET_NAME (cpu_stat_track_id_e id);

/*! 
  \fn      void _CPU_STAT_GET_NAME_EX(cpu_stat_track_id_e id,
                                      char               *buf,
                                      uint32              size)
  \brief   Fills buffer with the track's symbolic name or track's ID if no symbolic name found.

  \param   id    track ID
  \param   buf   buffer to fill/format
  \param   size  size of buffer

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
void
_CPU_STAT_GET_NAME_EX (cpu_stat_track_id_e id,
                       char               *buf,
                       uint32              size);

/*! 
  \fn      int _CPU_STAT_IS_ENABLED(cpu_stat_track_id_e id)
  \brief   Check if track ID is enabled.
           

  \param   id   track ID

  \return  1 - enabled, 0 - disabled

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
static inline int
_CPU_STAT_IS_ENABLED (cpu_stat_track_id_e id)
{
  return (id == g_mtlk_cpu_stat_obj.enabled_id);
}

/*! 
  \fn      uint32 _CPU_STAT_GET_DELAY(cpu_stat_track_id_e id)
  \brief   Get emitted delay for specified track.


  \param   id   track ID

  \return  delay in microseconds

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
static inline uint32
_CPU_STAT_GET_DELAY (cpu_stat_track_id_e id)
{
  return g_mtlk_cpu_stat_obj.tracks[id].usec_delay;
}

/*! 
  \fn      cpu_stat_track_id_e _CPU_STAT_GET_ENABLED_ID()
  \brief   Returnes ID of enabled track. 
           

  \return  track ID

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
static inline cpu_stat_track_id_e 
_CPU_STAT_GET_ENABLED_ID (void)
{
  return g_mtlk_cpu_stat_obj.enabled_id;
}

/*! 
  \def    _CPU_STAT_FOREACH_TRACK_IDX(id)

  Iterates track IDs. Puts id into \a id variable.

  \warning This function should not be used for the measurements. It provides functionality required
           for CPU_STAT UI exposing, for example via procfs   
*/
#define _CPU_STAT_FOREACH_TRACK_IDX(id) \
  for ((id) = CPU_STAT_ID_NONE + 1; (id) < CPU_STAT_ID_LAST; (id)++)

static __INLINE const char *
_CPU_STAT_GET_UNIT_NAME (void)
{
  return mtlk_dbg_hres_get_unit_name();
}

/******************************************************************************
 * Interface
 ******************************************************************************/
/*! 
  \fn      void mtlk_cpu_stat_reset()
  \brief   Resets the CPU Statistics module including collected data and Enabled Track ID

  \warning This function is used to reset statistics upon user request, for example via pocfs.
*/
static inline void
mtlk_cpu_stat_reset (void)
{
  mtlk_handle_t         lock_val;
  lock_val = mtlk_osal_lock_acquire_irq(&g_mtlk_cpu_stat_obj.lock);
  memset(&g_mtlk_cpu_stat_obj.tracks, 0, sizeof(g_mtlk_cpu_stat_obj.tracks));
  g_mtlk_cpu_stat_obj.current_id = CPU_STAT_ID_NONE;
  g_mtlk_cpu_stat_obj.enabled_id = CPU_STAT_ID_NONE;
  g_mtlk_cpu_stat_obj.timestamp_valid = FALSE;
  mtlk_osal_lock_release_irq(&g_mtlk_cpu_stat_obj.lock, lock_val);
}

/*! 
  \fn      void mtlk_cpu_stat_init()
  \brief   Initializes the CPU Statistics module

  \warning This function is usually called by HW layer during the initialization phase. 
           Pointer to CPU STAT module's object then is passed to Core via HAL 
           (mtlk_core_set_prop).
*/
static inline void 
mtlk_cpu_stat_init (void)
{
  mtlk_osal_lock_init(&g_mtlk_cpu_stat_obj.lock);
  mtlk_cpu_stat_reset();
}

/*! 
  \def     mtlk_cpu_stat_cleanup()
  \brief   Cleans the CPU Statistics module up.
*/
static inline void 
mtlk_cpu_stat_cleanup (void)
{
  mtlk_osal_lock_cleanup(&g_mtlk_cpu_stat_obj.lock);
}

/*! 
  \fn      void mtlk_cpu_stat_enable(cpu_stat_track_id_e id)
  \brief   Enable measurements for specified track. Disables measurement if id == CPU_STAT_PT_NONE
           

  \param   id   track ID
*/
static inline void 
mtlk_cpu_stat_enable (cpu_stat_track_id_e id)
{
  if (__CPU_STAT_ID_VALID(id) || id == CPU_STAT_ID_NONE)
      g_mtlk_cpu_stat_obj.enabled_id = id;
}

/*! 
  \fn      void mtlk_cpu_stat_set_delay(cpu_stat_track_id_e id, uint32 delay)
  \brief   Sets emitted delay for the specified track

  \param   id     track ID
  \param   delay  delay in microseconds
*/
static inline void 
mtlk_cpu_stat_set_delay (cpu_stat_track_id_e id, uint32 delay)
{
  if (__CPU_STAT_ID_VALID(id))
  {
      mtlk_handle_t         lock_val;
      lock_val = mtlk_osal_lock_acquire_irq(&g_mtlk_cpu_stat_obj.lock);

      g_mtlk_cpu_stat_obj.tracks[id].usec_delay = delay;

      mtlk_osal_lock_release_irq(&g_mtlk_cpu_stat_obj.lock, lock_val);
  }
}

/*! 
  \fn      int CPU_STAT_BEGIN_TRACK(cpu_stat_track_id_e id)
  \brief   Begins measurement of specified track. 
           Note, this will do nothing (1 'if') if the specified track is not enabled 
           prior to this call.

  \param   id    track ID
*/
static inline int 
CPU_STAT_BEGIN_TRACK (cpu_stat_track_id_e id)
{
  if (__CPU_STAT_ID_ENABLED(id)) {
    mtlk_dbg_hres_ts(&g_mtlk_cpu_stat_obj.current_start);
    g_mtlk_cpu_stat_obj.timestamp_valid = TRUE;
    return 1;
  }

  return 0;
}

/*! 
  \fn       void CPU_STAT_END_TRACK(cpu_stat_track_id_e id)
  \brief   Finalizes measurement of specified track. 
           Note, this will do nothing (2 'if's) if the specified track is not enabled 
           or not started prior to this call.

  \param   id    track ID
*/
static inline void 
CPU_STAT_END_TRACK (cpu_stat_track_id_e id)
{
  if (__CPU_STAT_ID_ENABLED(id)) {

    __CPU_STAT_END(id, &g_mtlk_cpu_stat_obj.current_start);
    g_mtlk_cpu_stat_obj.current_id = CPU_STAT_ID_NONE;
  }
}

/*! 
  \fn      void CPU_STAT_BEGIN_TRACK_SET(const cpu_stat_track_id_e *ids,
                                         uint32                     nof_ids,
                                         cpu_stat_track_id_e       *ts_handle)
  \brief   Begins measurement of one of specified tracks. 
           Note, this will do nothing (1 'if' per track ID) if no one of specified tracks 
           is enabled prior to this call. Current track ID can be specified after this call.
           

  \param   ids       array of track IDs
  \param   nof_ids   array size
  \param   ts_handle Filled with value to be passed to CPU_STAT_END_TRACK_SET.
*/
static inline void
CPU_STAT_BEGIN_TRACK_SET (const cpu_stat_track_id_e *ids,
                          uint32                     nof_ids,
                          cpu_stat_track_id_e       *ts_handle)
{
  uint32              i   = 0;

  for (; i < nof_ids; i++) {
    if (CPU_STAT_BEGIN_TRACK(ids[i])) {
      *ts_handle = ids[i];
      return;
    }
  }

  *ts_handle = CPU_STAT_ID_NONE;
}

/*! 
  \fn      void CPU_STAT_SPECIFY_TRACK(cpu_stat_track_id_e id)
  \brief   Specifies current track ID. 

  \param   id    track ID
*/
static inline void 
CPU_STAT_SPECIFY_TRACK (cpu_stat_track_id_e id) 
{
  if (__CPU_STAT_ID_ENABLED(id))
    g_mtlk_cpu_stat_obj.current_id = id;
}

/*! 
  \fn      void CPU_STAT_END_TRACK_SET (cpu_stat_track_id_e begin_ret_val)
  \brief   Finalizes measurement of the current track previously specified by 
           CPU_STAT_SPECIFY_TRACK(). 
           Note, this will do nothing (1 'if') if the specified track is not started or
           it is not enabled prior to this call.
           

  \param   begin_ret_val value returned by CPU_STAT_BEGIN_TRACK_SET
*/
static inline void
CPU_STAT_END_TRACK_SET (cpu_stat_track_id_e begin_ret_val)
{
  if (begin_ret_val == g_mtlk_cpu_stat_obj.current_id) /* current TRACK is started */
    CPU_STAT_END_TRACK(begin_ret_val);
}

#else /* CPTCFG_IWLWAV_CPU_STAT */

#define CPU_STAT_BEGIN_TRACK(x)
#define CPU_STAT_END_TRACK(x)
#define CPU_STAT_BEGIN_TRACK_SET(x, y, z)
#define CPU_STAT_SPECIFY_TRACK(x)
#define CPU_STAT_END_TRACK_SET(x)

#endif /* CPTCFG_IWLWAV_CPU_STAT */

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* __CPU_STAT_H__ */
