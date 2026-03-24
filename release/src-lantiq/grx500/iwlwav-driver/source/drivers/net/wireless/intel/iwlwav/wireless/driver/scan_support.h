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
 * Scan support header
 *
 */
#ifndef __SCAN_SUPPORT_H__
#define __SCAN_SUPPORT_H__

#include "mtlk_coreui.h"
#include "mhi_umi_propr.h"
#include "progmodel.h"
#include <linux/ktime.h>

#define LOG_LOCAL_GID GID_SCAN_SUPPORT
#define LOG_LOCAL_FID 1

#define BEACON_FRAME_INTERVAL_US   102400       /* 102400us = 100*TU,TU is 1024 microseconds */
#define MAX_SCAN_TIME_WO_SLICE_US  (28 * 1000)  /* in us, 32ms CTS-to-SELF minus context switch time */

/* mimic the linux enum nl80211_scan_flags */
enum mtlk_scan_flags {
  MTLK_SCAN_FLAG_LOW_PRIORITY = 1<<0,
  MTLK_SCAN_FLAG_FLUSH = 1<<1,
  MTLK_SCAN_FLAG_AP = 1<<2,
};

/* mimic the linux enum ieee80211_channel_flags with our own */
enum mtlk_channel_flags
{
  MTLK_CHAN_DISABLED = 1<<0,
  MTLK_CHAN_NO_IR = 1<<1,
  MTLK_CHAN_RADAR = 1<<3,
  MTLK_CHAN_NO_HT40PLUS = 1<<4,
  MTLK_CHAN_NO_HT40MINUS = 1<<5,
  MTLK_CHAN_NO_OFDM = 1<<6,
  MTLK_CHAN_NO_80MHZ = 1<<7,
  MTLK_CHAN_NO_160MHZ = 1<<8,
  MTLK_CHAN_INDOOR_ONLY = 1<<9,
  MTLK_CHAN_GO_CONCURRENT = 1<<10,
  MTLK_CHAN_NO_20MHZ = 1<<11,
  MTLK_CHAN_NO_10MHZ = 1<<12,
};

/* mimic enum nl80211_dfs_state */
enum mtlk_dfs_state
{
  MTLK_DFS_USABLE,
  MTLK_DFS_UNAVAILABLE,
  MTLK_DFS_AVAILABLE,
};

/* These are values for parameter "is_bg_scan" of
   the function "mtlk_scan_recovery_continue_scan" */
enum {
  MTLK_SCAN_SUPPORT_NO_BG_SCAN,
  MTLK_SCAN_SUPPORT_BG_SCAN
};

#define MHZS_PER_20MHZ 20     /* I can't believe it's 20---it's a miracle! */
#define CHANNUMS_PER_20MHZ 4
#define CHANS_IN_80MHZ 4
#define CHANS_IN_160MHZ 8

#define MAX_NUM_SURVEYS    NUM_TOTAL_CHANS
#define MAX_NUM_SURVEYS_6G NUM_TOTAL_CHANS_6G

#define CHAN_IDX_ILLEGAL 0

/* Conversion from 2.4 and 5 GHz channel index in our array of channel scan support data to channel number */
extern const uint8 chanidx2num_2_5[];
/* Conversion from 5 GHz channel number to its index in our array of extra_chan_data */
extern const uint8 channum2idx_2_5[256]; /* FIXME: size should be taken from scan_support.c */

/* Conversion from our channel-scan-support index to channel number */
static __INLINE
unsigned cssidx2channum(unsigned idx)
{
  MTLK_ASSERT(idx <= NUM_TOTAL_CHANS);
  return chanidx2num_2_5[idx];
}

/* Conversion from channel number to our channel-scan-support index */
static __INLINE
unsigned channum2cssidx(int num)
{
  unsigned idx;

  if ((num < 0) || ((size_t)num >= MTLK_ARRAY_SIZE(channum2idx_2_5)))
    return CHAN_IDX_ILLEGAL;

  idx = channum2idx_2_5[(u8) num];
  /* Note: size of array css of struct channel_scan_support must fit to idx value */
  return (idx > NUM_TOTAL_CHANS) ? CHAN_IDX_ILLEGAL : idx;
}

static __INLINE
unsigned channum2csysidx(int num)
{
  unsigned idx;

  if ((num < 0) || ((size_t)num >= MTLK_ARRAY_SIZE(channum2idx_2_5)))
    return CHAN_IDX_ILLEGAL;

  idx = channum2idx_2_5[(u8) num];
  return (idx >= MAX_NUM_SURVEYS) ? CHAN_IDX_ILLEGAL : idx;
}

/* Conversion from all 20MHz channel-freq-support index to
 * 20MHz channel number
 * idx 1=>chan 1, idx 2=>chan 5, idx 3=>chan 9 ...
 */
static __INLINE
unsigned cssidx6g2channum(unsigned idx)
{
#define FOUR_TIMES_IDX_PLUS_ONE(x) ((x << 2) + 1)

  MTLK_ASSERT(idx <= NUM_TOTAL_CHANS_6G);
  /* 20 MHz chan num = (4 * (idx - 1)) + 1 */
  return (idx ? FOUR_TIMES_IDX_PLUS_ONE((idx - 1)) : CHAN_IDX_ILLEGAL);
}

/* Conversion from all 20MHz channel number to 20MHz
 * channel-freq-support index
 * chan 1=>idx 1, chan 5=>idx 2, chan 9=>idx 3 ...
 */

extern unsigned channum6g2chanidx(int num);

static __INLINE
unsigned channum6g2cssidx(int num)
{
#define IDX_BY_TWO_PLUS_ONE(x) ((x >> 1) + 1)

  unsigned idx = channum6g2chanidx(num);
  return ((idx > 1)? IDX_BY_TWO_PLUS_ONE(idx) : idx);
}

static __INLINE
uint8 is_interf_det_needed(unsigned channel)
{
  MTLK_UNREFERENCED_PARAM(channel);
  /* enable on all bands and channels */
  return 1;
}

static __INLINE
uint8 is_interf_det_switch_needed(unsigned channel)
{
  return ((1 == channel) || (6 == channel) || (11 == channel));
}


enum scanDescriptionFlags
{
  SDF_RUNNING = 0x01,
  SDF_IGNORE_FRAMES = 0x02,
  SDF_PASSIVE = 0x04,
  SDF_ACTIVE = 0x08,
  SDF_SCHEDULED = 0x10,
  SDF_BACKGROUND = 0x20,
  SDF_BG_BREAK = 0x40,
  SDF_ABORT = 0x80,
  SDF_PAUSE_OR_PREVENT = 0x100,
  SDF_STAY_ON_CHAN = 0x200,
  SDF_MAC_RUNNING = 0x400,
};

/* The modification flags/modes apply to all VAPs equally.
 * However, depending on each VAP's type (AP or STA) it will use only the applicable modifications
 */
enum ScanModifFlags
{
  SMF_NONE = 0,
  // Passive scan modifications, i.e., typically for APs:
  SMF_ADD_ACTIVE = 1, // add active scanning to passive scanning
  SMF_REMOVE_PASSIVE = 2, // it's a flag, but not to be used without the previous flag
  SMF_ACTIVE_INSTEAD_OF_PASSIVE = 3, // both of the previous 2 flags simultaneously
  // Active scan modifications, i.e., typically for stations:
  SMF_ADD_PASSIVE = 4, // add passive scanning to active scanning
  SMF_REMOVE_ACTIVE = 8, // it's a flag, but not to be used without the previous flag
  SMF_PASSIVE_INSTEAD_OF_ACTIVE = 0x0c, // both of the previous 2 flags simultaneously
  SMF_AUTO_ENABLE_BG = 0x10, // switching to background mode may allow channel switching when VAPs are up
  SMF_AUTO_PASSIVE_ON_NO_IR = 0x20, // switch NO_IR chans to passive mode automatically
  // and for completeness, not a mode or a flag, just a terminator
  SMF_NUM_SMF = 0x40 // upper bound on the more-or-less sensible modification modes
};

struct channel_scan_support
{
  u64 last_passive_scan_time; /* in 64-bit jiffies */
  u64 last_active_scan_time;  /* in 64-bit jiffies */
  u8 calib_done_mask;
  u8 calib_failed_mask;
};

struct channel_survey_support
{
  struct mtlk_channel *channel;
  s8 noise; /* as reported by GET_CHAN_STATUS */
  u8 load;  /* as reported by GET_CHAN_STATUS */
};

#define CHDATA_NOISE_FLOOR            1
#define CHDATA_BUSY_TIME              2
#define CHDATA_TOTAL_TIME             4
#define CHDATA_CALIB                  8
#define CHDATA_NUM_BSS           0x0010
#define CHDATA_DYNBW             0x0020
#define CHDATA_RSSI              0x0040
#define CHDATA_SNR               0x0080
#define CHDATA_CWI_NOISE         0x0100
#define CHDATA_LOW_RSSI          0x0400
#define CHDATA_TX_POWER          0x0800
#define CHDATA_LOAD              0x1000
#define CHDATA_SCAN_MODE       0x800000

struct dfs_debug_params
{
#ifdef CONFIG_WAVE_DEBUG
  int beacon_count;
  uint32 nop; /* Non Occupancy Period */
#endif
  BOOL switch_in_progress;
  u8 debug_chan;
};

#define MAX_SUPPORTED_RATES 8
#define HT_CAPAB_LEN 26
#define MAX_SSID_LEN (MTLK_ESSID_MAX_SIZE - 1)
#define SSID_IE_TAG 0
#define SUPPORTED_RATES_IE_TAG 1
#define HT_CAPABILITIES_IE_TAG 45

typedef struct scan_support
{
  mtlk_core_t *master_core; /* this allows us to easily refer to related stuff, such as the current_chandef */
  mtlk_core_t *requester_core;
  volatile uint32 flags;        /* flags from scanDescriptionFlags */
  struct mtlk_chan_def orig_chandef; /* what to return to after scan, if any */
  struct mtlk_scan_request req; /* sched scan stuff can get saved here, too */
  mtlk_osal_event_t scan_end_event;
  mtlk_osal_hrtimer_t scan_timer;
  mtlk_txmm_msg_t set_chan_man_msg;
  UM_SET_CHAN_PARAMS *set_chan_req; /* points inside the msg above */
  uint32 ScanModifierFlags;
  struct iwpriv_scan_params iwpriv_scan_params;
  struct iwpriv_scan_params_bg iwpriv_scan_params_bg;
  struct dfs_debug_params dfs_debug_params; /*DFS debug params for radar simulation*/
  u32 PassiveValid; /* in jiffies */
  u32 ActiveValid;  /* in jiffies */
  u32 PassiveScanWaitTime; /* in us, copied PassiveScanTime from either fg or bg iwpriv_scan_params */
  u32 ActiveScanWaitTime;  /* in us, do it for copied ActiveScanTime */
  u16 NumProbeReqs;      /* number of probe requests to send for the same SSID */
  u32 ProbeReqInterval;  /* time in us, after which to fire the next round of probe reqs for the same SSIDs */
  u16 NumChansInChunk;   /* how many channels to probe in one "chunk" (deviation from the real channel in use) */
  u32 ChanChunkInterval; /* how often (in us) may we check the next chunk of channels in BG scan */
  u32 param_algomask;
  u32 param_oalgomask;
  u64 scan_start_time;
  ktime_t scan_next_ktime;
  /* stuff to deal with chunks in BG scans and repeating probes in active scans */
  int chan_in_chunk;
  int last_probe_num;
  u32 chan_in_chunk_time;   /* the current time which spent for channel scan in chunk in us*/
  u16 window_slice_num;     /* number of performed window slices */
  u32 window_slice;         /* window slices time in us*/
  u32 window_slice_overlap; /* overlap time between window slices in us*/
  u32 cts_to_self_duration;
  /* extra channel data */
  u32 chan_flags;
  int last_chan_idx; /* index in req */
  u32 last_chan_num;
  int last_chan_band;
  u32 last_chan_flags; /* channel flags from enum mtlk_channel_flags */
  struct channel_scan_support css[1 + NUM_TOTAL_CHANS]; /* 0-th entry not used */
  struct channel_scan_support css_6g[1 + NUM_TOTAL_CHANS_6G]; /* 0-th entry not used */
  // FIXME we'll have to tie these to a VAP, so that wpa_supplicant's scans don't screw up hostapd's surveys */
  int num_surveys;
  struct channel_survey_support csys[MAX_NUM_SURVEYS];
  struct channel_survey_support csys_6g[MAX_NUM_SURVEYS_6G];
  /* buffers to use when preparing probe requests for active scans */
  u8 supp_rates[NUM_SUPPORTED_BANDS][MAX_SUPPORTED_RATES + 2];
  u8 ht_capab[NUM_SUPPORTED_BANDS][HT_CAPAB_LEN + 2];
  u8 *probe_req; /* this will be alloced per scan, because we don't know how long the extra IE-s will be */
  size_t probe_req_len;
} mtlk_scan_support_t;

typedef struct mtlk_alter_scan
{
  BOOL abort_scan;
  BOOL pause_or_prevent;
  BOOL resume_or_allow;
} mtlk_alter_scan_t;

/* Checks whether some kind of scan is going on right now */
/* Won't tell about a SCAN_DO_SCAN sitting on master serializer's queue ready to start.
 * Such a thing can be identified by looking at rdev->scan_req
 * but it looks just like that also after a scan ends and before the kernel has reported the results...
 */
static __INLINE
unsigned is_scan_running(const struct scan_support *ss)
{
  return ss->flags & SDF_RUNNING;
}

static __INLINE
unsigned is_mac_scan_running(const struct scan_support *ss)
{
  return ss->flags & SDF_MAC_RUNNING;
}

/* This is an optimization, the scan will be "ignorant" (and won't report frames to the kernel)
 * at all those times when it's not explicitly sitting on the right channel waiting the timeout.
 * I.e., during processing, during breaks from a background scan, initial preparation, etc.
 * Things we would hear during those times are those on our regular channel
 * or duplicates of what we just heard or will hear on any explicitly scannable channel.
 */
static __INLINE
unsigned is_scan_ignorant(const struct scan_support *ss)
{
  return ss->flags & SDF_IGNORE_FRAMES;
}

int mtlk_scan_recovery_get_channel(mtlk_core_t *core, struct mtlk_chan_def *chandef);
int mtlk_scan_recovery_continue_scan(mtlk_core_t *core, BOOL is_bg_scan);

void abort_scan(struct scan_support *ss);
int abort_scan_sync(mtlk_core_t *core);
int pause_or_prevent_scan(mtlk_core_t *core);
int resume_or_allow_scan(mtlk_core_t *core);
int clean_up_after_this_scan(mtlk_core_t *core, struct scan_support *ss);

int scan_support_init(mtlk_core_t *core, mtlk_scan_support_t *ss);
void scan_support_cleanup(mtlk_scan_support_t *ss);
int _scan_do_scan(mtlk_core_t *core, struct mtlk_scan_request *request);
int scan_timeout_async_func(mtlk_handle_t hcore, const void* data, uint32 data_size);
int scan_do_scan(mtlk_handle_t hcore, const void* data, uint32 data_size);
int scan_dump_survey(mtlk_handle_t hcore, const void* data, uint32 data_size);

int __MTLK_IFUNC
mtlk_alter_scan(mtlk_handle_t hcore, const void* data, uint32 data_size);

int scan_get_aocs_info(mtlk_core_t *core, struct intel_vendor_channel_data *ch_data, struct channel_survey_support *csys);
void get_channel_data_hw_save_chan_stats_info(mtlk_core_t *core, struct intel_vendor_channel_data *ch_data);

int mtlk_send_chan_data(mtlk_vap_handle_t vap_handle, void *data, int data_len);

void __MTLK_IFUNC
wave_scan_support_fill_chan_bonding_by_chan(wave_chan_bonding_t *out_data, unsigned center_chan, unsigned primary_chan);

void __MTLK_IFUNC
wave_scan_support_fill_chan_bonding_by_freq(wave_chan_bonding_t *out_data, unsigned center_freq, unsigned primary_freq);

void scan_chan_survey_set_chan_load(struct channel_survey_support *csys, unsigned ch_load);
int scan_send_get_chan_load(const mtlk_core_t *core, uint32 *chan_load);

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* __SCAN_SUPPORT_H__ */
