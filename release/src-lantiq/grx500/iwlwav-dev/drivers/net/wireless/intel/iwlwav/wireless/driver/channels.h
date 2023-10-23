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
 * Written by: Roman Sikorskyy 
 *
 */

#ifndef __CHANNELS_H__
#define __CHANNELS_H__

#include "mhi_umi.h"
#include "eeprom.h"
#include "mtlk_clipboard.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID GID_SCAN_SUPPORT
#define LOG_LOCAL_FID 2

#define ALTERNATE_LOWER     1
#define ALTERNATE_UPPER     0
#define ALTERNATE_NONE      (-1)

typedef struct _tx_limit_t
{
  /* this is needed for HW limits - to load initial data */
  uint16 vendor_id;
  uint16 device_id;
  uint8 hw_type;
  uint8 hw_revision;
  uint8 num_tx_antennas;
  uint8 num_rx_antennas;
} __MTLK_IDATA tx_limit_t;

typedef struct _drv_params_t
{
    uint8 band;
    uint8 bandwidth;
    uint8 upper_lower;
    uint8 reg_domain;
    uint8 spectrum_mode;
} __MTLK_IDATA drv_params_t;

typedef struct {
  uint16             freq;
  uint8              tx_lim;
  uint8              spectrum;
  uint8              phy_mode;
  uint8              reg_domain;
} __MTLK_IDATA mtlk_hw_limits_stat_entry_t;

#define WAVE_BANDWIDTHS_NUM     MAXIMUM_BANDWIDTHS_GEN6

typedef struct {
    uint8  lower_chan[WAVE_BANDWIDTHS_NUM];
    uint8  center_chan[WAVE_BANDWIDTHS_NUM];
    uint8  upper_chan[WAVE_BANDWIDTHS_NUM];
} __MTLK_IDATA wave_chan_bonding_t;


static __INLINE uint16
mtlk_channels_get_secondary_channel_no_by_offset(uint16 primary_channel_no, uint8 secondary_channel_offset)
{
#define CB_CHANNEL_OFFSET 4

  uint16 res = 0;

  switch (secondary_channel_offset)
  {
  case UMI_CHANNEL_SW_MODE_SCA:
    res = primary_channel_no + CB_CHANNEL_OFFSET;
    break;
  case UMI_CHANNEL_SW_MODE_SCB:
    res = primary_channel_no - CB_CHANNEL_OFFSET;
    break;
  case UMI_CHANNEL_SW_MODE_SCN:
  default:
    break;
  }

  return res;

#undef CB_CHANNEL_OFFSET
}

#define WAVE_FIRST_5G_CHAN      (36)
#define WAVE_FIRST_5G_FREQ      (5000u + (5 * WAVE_FIRST_5G_CHAN))
#define WAVE_FREQ_IS_5G(freq)   (WAVE_FIRST_5G_FREQ <= (freq))

/* 6GHz band definitions */
#define WAVE_FIRST_6G_CHAN      (1)
#define WAVE_LAST_6G_CHAN       (233) /* U-NII-5 to U-NII-8 bands, full 6GHz spectrum*/
#define WAVE_6G_STARTING_FREQ   (5950) /*Refer 802.11 D7.0 spec*/
#define WAVE_FIRST_6G_FREQ      (WAVE_6G_STARTING_FREQ + (5 * WAVE_FIRST_6G_CHAN))
#define WAVE_FREQ_IS_6G(freq)   (WAVE_FIRST_6G_FREQ <= (freq))
/* for CDB table */
#define WAVE_CHAN_6G_LOWER        WAVE_FIRST_6G_CHAN
#define WAVE_CHAN_6G_UPPER_UNII5  93
#define WAVE_CHAN_6G_UPPER        WAVE_LAST_6G_CHAN
/* for channel tables */
#define NUM_6GHZ_CHANS_UNII5            24  /* channels  1,5,9,..., 93 */
#define NUM_6GHZ_CHANS_UNII6_8          35  /* channels 97,101,...,233 */
#define NUM_TOTAL_CHANS_6G              (NUM_6GHZ_CHANS_UNII5 + NUM_6GHZ_CHANS_UNII6_8)
/* for calibration */
#define NUM_6GHZ_CENTRAL_FREQS_BW40     (NUM_TOTAL_CHANS_6G >> 1)
#define NUM_6GHZ_CENTRAL_FREQS_BW80     (NUM_6GHZ_CENTRAL_FREQS_BW40 >> 1)
#define NUM_6GHZ_CENTRAL_FREQS_BW160    (NUM_6GHZ_CENTRAL_FREQS_BW80 >> 1)
#define NUM_HOLES_CHAN_FREQS_6G         8 /* 8 holes: 31,63,95,...,223,231 */
#define NUM_TOTAL_CHAN_FREQS_6G         (NUM_TOTAL_CHANS_6G + NUM_6GHZ_CENTRAL_FREQS_BW40 +\
                                         NUM_6GHZ_CENTRAL_FREQS_BW80 + NUM_6GHZ_CENTRAL_FREQS_BW160 +\
                                         NUM_HOLES_CHAN_FREQS_6G)
/* for channel number to channel index conversions and vice-versa */
#define MAX_CHAN_IDX_6G_FULL     (117) /* idx 1(ch 1), idx 2(ch 3), idx 3(ch 5),..., idx 117(ch 233) */
#define MAX_CHAN_IDX_6G_UNII5    (47)  /* idx 1(ch 1), idx 2(ch 3), idx 3(ch 5),..., idx 47(ch 93)   */

static __INLINE uint8
channel_to_band (uint16 channel)
{
  return (channel < WAVE_FIRST_5G_CHAN)? MTLK_HW_BAND_2_4_GHZ : MTLK_HW_BAND_5_2_GHZ;
}

static __INLINE uint8
frequency_to_band (uint16 frequency)
{
  /* Tri band support */
  mtlk_hw_band_e band;
  if (frequency < WAVE_FIRST_5G_FREQ) {
     band = MTLK_HW_BAND_2_4_GHZ;
  }  else if(frequency < WAVE_FIRST_6G_FREQ) {
     band = MTLK_HW_BAND_5_2_GHZ;
  }  else {
     band = MTLK_HW_BAND_6_GHZ;
  }

  return band;
}

static __INLINE uint16
channel_to_frequency (uint16 channel, mtlk_hw_band_e band)
{
#define CHANNEL_THRESHOLD 180
  uint16 res;
  /* IEEE Std 802.11-2012: 20.3.15 Channel numbering and channelization
    Channel center frequency = starting frequency + 5 * ch
  */
  /* Tri band support */
  switch (band) {
    case MTLK_HW_BAND_2_4_GHZ:
    {
      MTLK_ASSERT(channel_to_band(channel) == MTLK_HW_BAND_2_4_GHZ);
      res = 2407 + 5*channel;
      if(channel == 14) /* IEEE Std 802.11-2012: 17.4.6.3 Channel Numbering of operating channels */
        res += 7;
      break;
    }
    case MTLK_HW_BAND_5_2_GHZ:
    {
      MTLK_ASSERT(channel_to_band(channel) == MTLK_HW_BAND_5_2_GHZ);
      if (channel < CHANNEL_THRESHOLD)
        res = 5000 + 5*channel;
      else
        res = 4000 + 5*channel;
      break;
    }
    case MTLK_HW_BAND_6_GHZ:
    {
      /* IEEE P802.11ax/D4.3, August 2019 Sec 27.3.22.2 */
      res = WAVE_6G_STARTING_FREQ + (5*channel);
      break;
    }
    default:
    {
      res = 4000 + 5*channel; 
      break;
    }
  }
#undef CHANNEL_THRESHOLD
  return res;
}

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* __CHANNELS_H__ */
