/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __BITRATE_H__
#define __BITRATE_H__

#include "mtlkidefs.h"
#include "mhi_umi.h"
#include "wave_hal_stats.h"

/* Rate Power Offset */
typedef struct {
  uint8              phy_mode;
  uint8              mcs;
  uint8              nss;
  int8               pw_offs;
} mtlk_rate_pw_entry_t;

#define BITRATE_FIRST         0
#define BITRATE_LAST          31

#define BITRATES_MAX          32

#define BITRATE_MCS32         32 /* BPSK */

/* PHY modes */
#define PHY_MODE_AG         0
#define PHY_MODE_B          1
#define PHY_MODE_N          2
#define PHY_MODE_AC         3
#define PHY_MODE_AX         4
#define PHY_MODE_AX_SU_EXT  5
#define PHY_MODE_11AX_MU_TRIG  6
#define PHY_MODE_11AX_MU_DL    7

/* Number of rate MCS in the table */
#define NUM_RATE_MCS_AG     8   /* 0 ... 7 */
#define NUM_RATE_MCS_B      8   /* 0 ... 7 */
#define NUM_RATE_MCS_N     33   /* 0 .. 32 */
#define NUM_RATE_MCS_AC    10   /* 0 ... 9 */
#define NUM_RATE_MCS_AX    12   /* 0 .. 11 */
#define NUM_RATE_MCS_AX_SU_EXT 3 /* 0 .. 2 */
#define NUM_RATES_N_PER_NSS 8

#define PHY_STAT_RATE_NSS_MIN   1

#define BITRATE_MCS_TO_NSS(mcs) ((BITRATE_MCS32 == (mcs)) ? (1) : ((mcs) / NUM_RATES_N_PER_NSS + 1))
#define IS_QAM256_OR_HIGHER(phy_mode,mcs) (((PHY_MODE_AC == (phy_mode)) || (PHY_MODE_AX == (phy_mode))) && ((mcs) >= 8))

#define PHY_STAT_RATE_MODE_MAX  PHY_MODE_AX_SU_EXT
#define PHY_STAT_RATE_CBW_MAX   3 /* CW_160 */
#define PHY_STAT_RATE_CBW_NUM   (PHY_STAT_RATE_CBW_MAX + 1)
#define PHY_STAT_RATE_SCP_MAX   1 /* 0 or 1 */
#define PHY_STAT_RATE_NSS_MAX   4
#define PHY_STAT_RATE_MCS_MAX   (NUM_RATE_MCS_N - 1)

/* Bitrate values representation is fixed point 10*Mbps, e.g. 60 means 6.0 Mbit/s */
#define MTLK_BITRATE_INVALID    (0) /* use zero value if rate is unknown/invalid */
#define MTLK_BPS_PER_KBPS       1000
#define MTLK_KBPS_PER_MBPS      1000
#define MTLK_BPS_PER_MBPS       1000000
#define MTLK_KBPS_TO_MBPS(kbps) ((kbps) / MTLK_KBPS_PER_MBPS)

#define MTLK_BITRATE_IDX_INVALID (0)


#define MTLK_BITRATE_FACTOR     10
#define MTLK_BITRATE_TO_BPS(x)  ((x) * (MTLK_KBPS_PER_MBPS  / MTLK_BITRATE_FACTOR) * MTLK_BPS_PER_KBPS)
#define MTLK_BITRATE_TO_MBPS(x) (((x) + (MTLK_BITRATE_FACTOR / 2)) / MTLK_BITRATE_FACTOR) /* with rounding */
#define MTLK_BITRATE_TO_KBPS(x) MTLK_BITRATE_TO_MBPS((x) * MTLK_KBPS_PER_MBPS)

/* Supported rates are in 0.5 MHz units */
#define MTLK_SUPP_RATE_TO_BITRATE(x)  ((x) * MTLK_BITRATE_FACTOR / 2)

typedef uint16  mtlk_bitrate_info16_t;
#define MTLK_BITRATE_PLUME_HT_MCS              MTLK_BFIELD_INFO(0, 3)  /* 3 bits [MCS index] */
#define MTLK_BITRATE_PLUME_HT_NSS              MTLK_BFIELD_INFO(3, 2)  /* 2 bits [NSS]       */

/* 32-bits internal data structure for PHY Metrics rate info */
/* Data types:  1) Flag=0 + PHY rate value - ready value
                2) Flag=1 + STAT rate info encoded (PHY_STAT_RATE_INFO)
                          + STAT rate value (can be PHY_RATE_INVALID)
 */
typedef uint32  mtlk_bitrate_info_t;
#define MTLK_BITRATE_INFO_FLAG      MTLK_BFIELD_INFO(31,  1) /*  1 bit  [ 0 - ready PHY rate value, 1 - stat (rate + info) ] */

#define MTLK_BITRATE_INFO_PHY_RATE  MTLK_BFIELD_INFO( 0, 20) /* 20 bits PHY rate value */

#define MTLK_BITRATE_INFO_STAT_RATE MTLK_BFIELD_INFO( 0, 16) /* 16 bits STAT rate value */
#define MTLK_BITRATE_INFO_STAT_INFO MTLK_BFIELD_INFO(16, 11) /* 11 bits STAT info (PHY_STAT_RATE_INFO) */

/* Bitfields for MTLK_BITRATE_INFO_STAT_INFO (11 bits PHY_STAT_RATE_INFO) */
#define MTLK_BITRATE_STAT_PHYMODE   MTLK_BFIELD_INFO(16 +  0, 2)  /* 2 bits [ 00 - 11a/g     01 - 11b    10 - 11n (HT mode)   11 - 11ac (VHT mode) ]*/
#define MTLK_BITRATE_STAT_MCS       MTLK_BFIELD_INFO(16 +  2, 6)  /* 6 bits [MCS index ]*/
#define MTLK_BITRATE_STAT_CBW       MTLK_BFIELD_INFO(16 +  8, 2)  /* 2 bits [ 00 - 20MHz     01 - 40MHz    10 - 80MHz    11 - 160MHz ]*/
#define MTLK_BITRATE_STAT_SCP       MTLK_BFIELD_INFO(16 + 10, 1) /*  1 bit  [Short CP]*/

#define MTLK_BITRATE_INFO_BY_RATE_INFO(rate, info) \
        MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_FLAG,           1, mtlk_bitrate_info_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_STAT_RATE, (rate), mtlk_bitrate_info_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_STAT_INFO, (info), mtlk_bitrate_info_t)

#define MTLK_BITRATE_INFO_BY_PHY_RATE(phy_rate) \
        /* MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_FLAG, 0, mtlk_bitrate_info_t) | */\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_PHY_RATE, (phy_rate), mtlk_bitrate_info_t)

#define MTLK_BITRATE_INFO_INVALID   MTLK_BITRATE_INFO_BY_PHY_RATE(MTLK_BITRATE_INVALID)

/* 32-bits internal data structure for PHY rate parameters */
typedef uint32  mtlk_bitrate_params_t;
#define MTLK_BITRATE_PARAMS_MCS   MTLK_BFIELD_INFO( 0,  8) /*  6 bits [MCS index ] */
#define MTLK_BITRATE_PARAMS_NSS   MTLK_BFIELD_INFO( 8,  4) /*  4 bits [NSS] */
#define MTLK_BITRATE_PARAMS_SCP   MTLK_BFIELD_INFO(16,  4) /*  1 bits [Short CP] OR 3 bits cpMode */
#define MTLK_BITRATE_PARAMS_CBW   MTLK_BFIELD_INFO(20,  4) /*  2 bits [ 00 - 20MHz     01 - 40MHz    10 - 80MHz    11 - 160MHz ] */
#define MTLK_BITRATE_PARAMS_MODE  MTLK_BFIELD_INFO(24,  8) /*  3 bits [ 000 - 11a/g, 001 - 11b, 010 - 11n (HT), 011 - 11ac (VHT), 100 - 11ax (HE)] */

#define MTLK_BITRATE_PARAMS_BY_RATE_PARAMS(mode, cbw, scp, mcs, nss) \
        MTLK_BFIELD_VALUE(MTLK_BITRATE_PARAMS_MODE, mode, mtlk_bitrate_params_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_PARAMS_CBW,  cbw,  mtlk_bitrate_params_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_PARAMS_SCP,  scp,  mtlk_bitrate_params_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_PARAMS_MCS,  mcs,  mtlk_bitrate_params_t) |\
        MTLK_BFIELD_VALUE(MTLK_BITRATE_PARAMS_NSS,  nss,  mtlk_bitrate_params_t)

#define MTLK_BITRATE_PARAMS_INVALID     ((mtlk_bitrate_params_t)(~0)) /* FIXME: TBD */

/* Getters for bitrate_info_t */
static __INLINE BOOL
mtlk_bitrate_info_get_flag (mtlk_bitrate_info_t rate_info)
{
  return (0 != MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_INFO_FLAG));
}

static __INLINE uint32
mtlk_bitrate_info_get_phy_rate (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_INFO_PHY_RATE); /* ready value if flag is FALSE */
}

static __INLINE uint32
mtlk_bitrate_info_get_mode (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_PHYMODE);
}

static __INLINE uint32
mtlk_bitrate_info_get_stat_rate (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_INFO_STAT_RATE);
}

static __INLINE uint32
mtlk_bitrate_info_get_cbw (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_CBW);
}

static __INLINE uint32
mtlk_bitrate_info_get_scp (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_SCP);
}

static __INLINE uint32
mtlk_bitrate_info_get_mcs (mtlk_bitrate_info_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_MCS);
}

static __INLINE uint32
mtlk_bitrate_info_get_nss (mtlk_bitrate_info_t rate_info)
{ /* Note: mtlk_bitrate_info_t does not contain NSS. So take it from MCS */
  return BITRATE_MCS_TO_NSS(mtlk_bitrate_info_get_mcs(rate_info));
}

static __INLINE BOOL
mtlk_bitrate_info_rate_ready (uint32 phy_mode, uint32 rate_value)
{
  return (PHY_RATE_INVALID != rate_value) && (PHY_MODE_B != phy_mode);
}

uint32 __MTLK_IFUNC
mtlk_bitrate_hw_params_to_rate (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss);

static __INLINE uint32
mtlk_bitrate_info_to_rate (mtlk_bitrate_info_t bitrate_info)
{
  if (__UNLIKELY(MTLK_BITRATE_INFO_INVALID == bitrate_info)) {
    return MTLK_BITRATE_INVALID;
  } else if (!mtlk_bitrate_info_get_flag(bitrate_info)) {
    return mtlk_bitrate_info_get_phy_rate(bitrate_info);
  } else {
    uint32 rate_val = mtlk_bitrate_info_get_stat_rate(bitrate_info);
    uint32 phy_mode = mtlk_bitrate_info_get_mode(bitrate_info);
    if ((PHY_RATE_INVALID == rate_val) || (PHY_MODE_B == phy_mode)) {
    rate_val = mtlk_bitrate_hw_params_to_rate(
                    phy_mode,
                    mtlk_bitrate_info_get_cbw(bitrate_info),
                    mtlk_bitrate_info_get_scp(bitrate_info),
                    mtlk_bitrate_info_get_mcs(bitrate_info),
                    mtlk_bitrate_info_get_nss(bitrate_info));
    }
    return rate_val;
  }
}
#ifdef UNUSED_CODE
uint8 __MTLK_IFUNC
mtlk_bitrate_hw_params_to_rate_idx (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss);
#endif /* UNUSED_CODE */
uint8 __MTLK_IFUNC
mtlk_bitrate_info_to_rate_idx(mtlk_bitrate_info_t rate_info);

/* Getters for bitrate_params_t */
static __INLINE uint32
mtlk_bitrate_params_get_mode (mtlk_bitrate_params_t bitrate_params)
{
  return MTLK_BFIELD_GET(bitrate_params, MTLK_BITRATE_PARAMS_MODE);
}

static __INLINE uint32
mtlk_bitrate_params_get_cbw (mtlk_bitrate_params_t bitrate_params)
{
  return MTLK_BFIELD_GET(bitrate_params, MTLK_BITRATE_PARAMS_CBW);
}

static __INLINE uint32
mtlk_bitrate_params_get_mcs (mtlk_bitrate_params_t bitrate_params)
{
  return MTLK_BFIELD_GET(bitrate_params, MTLK_BITRATE_PARAMS_MCS);
}

static __INLINE uint32
mtlk_bitrate_params_get_nss (mtlk_bitrate_params_t bitrate_params)
{
  uint32 nss;
  nss = MTLK_BFIELD_GET(bitrate_params, MTLK_BITRATE_PARAMS_NSS);
  if (0 == nss) {
    nss = PHY_STAT_RATE_NSS_MIN; /* minimal possible */
  }
  return nss;
}

static __INLINE uint32
mtlk_bitrate_params_get_scp (mtlk_bitrate_params_t bitrate_params)
{
  return MTLK_BFIELD_GET(bitrate_params, MTLK_BITRATE_PARAMS_SCP);
}

static __INLINE void
mtlk_bitrate_params_get_hw_params(mtlk_bitrate_params_t bitrate_params,
                    uint32 *mode, uint32 *cbw, uint32 *scp, uint32 *mcs, uint32 *nss)
{
  *mode = mtlk_bitrate_params_get_mode(bitrate_params);
  *cbw  = mtlk_bitrate_params_get_cbw(bitrate_params);
  *scp  = mtlk_bitrate_params_get_scp(bitrate_params);
  *mcs  = mtlk_bitrate_params_get_mcs(bitrate_params);
  *nss  = mtlk_bitrate_params_get_nss(bitrate_params);
}

static __INLINE uint32
mtlk_bitrate_params_to_rate(mtlk_bitrate_params_t bitrate_params)
{
  if (__UNLIKELY(MTLK_BITRATE_PARAMS_INVALID == bitrate_params))
    return MTLK_BITRATE_INVALID;

  return mtlk_bitrate_hw_params_to_rate(
            mtlk_bitrate_params_get_mode(bitrate_params),
            mtlk_bitrate_params_get_cbw(bitrate_params),
            mtlk_bitrate_params_get_scp(bitrate_params),
            mtlk_bitrate_params_get_mcs(bitrate_params),
            mtlk_bitrate_params_get_nss(bitrate_params));
}

BOOL __MTLK_IFUNC
mtlk_bitrate_hw_params_supported (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss);

BOOL __MTLK_IFUNC
mtlk_bitrate_hw_params_supported_rate (uint32 *rate, uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss);

int __MTLK_IFUNC mtlk_bitrate_get_value (int index, int sm, int scp);

static __INLINE void
mtlk_bitrate_params_get_11n_mcs_and_nss_for_plume (mtlk_bitrate_info16_t rate_info, uint8 *mcs, uint8 *nss)
{
  *mcs = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PLUME_HT_MCS);
  *nss = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PLUME_HT_NSS) + 1;
}

static __INLINE uint8
mtlk_bitrate_params_get_psdu_mode (mtlk_bitrate_info16_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_PSDU_PHYMODE);
}

static __INLINE uint8
mtlk_bitrate_params_get_psdu_cbw (mtlk_bitrate_info16_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_BW);
}

static __INLINE uint8
mtlk_bitrate_params_get_psdu_scp (mtlk_bitrate_info16_t rate_info)
{
  return MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_SCP) |
         (MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_SCP_HE) >> 1);
}

static __INLINE void
mtlk_bitrate_params_get_psdu_mcs_and_nss (mtlk_bitrate_info16_t rate_info, uint8 *mcs, uint8 *nss)
{
  uint8 mode;

  mode = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_STAT_PSDU_PHYMODE);
  *nss = 0;

  switch(mode) {
    case PHY_MODE_AG:
      *mcs = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_LEGACY_MCS);
      *nss = HAL_LEGACY_NSS;
    break;

    case PHY_MODE_B:
      *mcs = 0;
      *nss = HAL_LEGACY_NSS;
    break;

    case PHY_MODE_N:
      *mcs = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_HT_MCS);
      *nss = BITRATE_MCS_TO_NSS(*mcs);
    break;

    case PHY_MODE_AC:
      *mcs = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_VHT_MCS);
      *nss = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_VHT_NSS) + 1;
    break;

    case PHY_MODE_AX:
    case PHY_MODE_AX_SU_EXT:
    case PHY_MODE_11AX_MU_TRIG:
    case PHY_MODE_11AX_MU_DL:
      *mcs = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_HE_MCS);
      *nss = MTLK_BFIELD_GET(rate_info, MTLK_BITRATE_PSDU_HE_NSS) + 1;
    break;

    default:
    break;
  };

  return;
}
#endif
