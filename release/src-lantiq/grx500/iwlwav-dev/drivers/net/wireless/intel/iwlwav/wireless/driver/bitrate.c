/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "bitrate.h"
#include "mtlkaux.h"
#include "mtlk_coreui.h"
#include "mhi_ieee_address.h"

#define LOG_LOCAL_GID   GID_BITRATE
#define LOG_LOCAL_FID   0

/*****************************************************************************
**
** NAME         mtlk_bitrate_get_value
**
** PARAMETERS   index  requested bitrate index
**              sm     SpectrumMode
**              scp    ShortCyclicPrefix
**
** RETURNS      Value of bitrate in bits/second
**
** DESCRIPTION  Function returns value of bitrate 
**              depend on current HW parameters and bitrate index
**
******************************************************************************/

/*
 * Tables below (per Network mode) contain all available bitrates
 * Network mode: 0 - 11a/g, 1 - 11b, 2 - 11an, 3 - 11ac
 *
 * Values representation is fixed point, i.e. 60 means 6.0 Mbit/s.
 *
 * Index in table composed of several parameters:
 * bits | param
 * ======================
 *    0 | SCP ShortCyclicPrefix
 *  1,2 | CBW ChannelBandWidth (from 0 to 3)
 *  3-7 | MCS index (from 0 to 32)
 *
 * See _hw_rate_params_to_rate() for details
 *
 */

#define HE_CP_MODES_NUM     6

typedef enum {
  HE_CP_TYPE_SHORT,
  HE_CP_TYPE_MEDIUM,
  HE_CP_TYPE_LONG,
  HE_CP_TYPES_NUM
} he_cp_types_e;

static uint8 he_cp_mode_to_cp_type_table[HE_CP_MODES_NUM] = {
  HE_CP_TYPE_SHORT,
  HE_CP_TYPE_MEDIUM,
  HE_CP_TYPE_SHORT,
  HE_CP_TYPE_MEDIUM,
  HE_CP_TYPE_SHORT,
  HE_CP_TYPE_LONG
};


#define _NV_            MTLK_BITRATE_INVALID

/* Mode 0 (11a/g)   MCS 0...7, CBW 0, SCP 0 */
static const uint16 bitrates_11ag[NUM_RATE_MCS_AG] = {
    60,  90, 120, 180, 240, 360, 480, 540
};

/* Mode 1 (11b)     MCS 1...7, CBW 0, SCP 0 */
static const uint16 bitrates_11b[NUM_RATE_MCS_B] = {
    _NV_,  20,  55, 110,  10,  20,  55, 110
};

/* Mode 2 (11n) HT, MCS 0...32, CBW 0/1, SCP 0/1 */
/* Bit0 - SCP, Bit1 - CBW, Bits2345 - MCS */
static const uint16 bitrates_11n[NUM_RATE_MCS_N * 2 * 2] = {
      65,     72,    135,    150, /*  0 */
     130,    144,    270,    300, /*  1 */
     195,    217,    405,    450, /*  2 */
     260,    289,    540,    600, /*  3 */
     390,    433,    810,    900, /*  4 */
     520,    578,   1080,   1200, /*  5 */
     585,    650,   1215,   1350, /*  6 */
     650,    722,   1350,   1500, /*  7 */

     130,    144,    270,    300, /*  8 */
     260,    289,    540,    600, /*  9 */
     390,    433,    810,    900, /* 10 */
     520,    578,   1080,   1200, /* 11 */
     780,    867,   1620,   1800, /* 12 */
    1040,   1156,   2160,   2400, /* 13 */
    1170,   1300,   2430,   2700, /* 14 */
    1300,   1444,   2700,   3000, /* 15 */

     195,    217,    405,    450, /* 16 */
     390,    433,    810,    900, /* 17 */
     585,    650,   1215,   1350, /* 18 */
     780,    867,   1620,   1800, /* 19 */
    1170,   1300,   2430,   2700, /* 20 */
    1560,   1733,   3240,   3600, /* 21 */
    1755,   1950,   3645,   4050, /* 22 */
    1950,   2167,   4050,   4500, /* 23 */

     260,    289,    540,    600, /* 24 */
     520,    578,   1080,   1200, /* 25 */
     780,    867,   1620,   1800, /* 26 */
    1040,   1156,   2160,   2400, /* 27 */
    1560,   1733,   3240,   3600, /* 28 */
    2080,   2311,   4320,   4800, /* 29 */
    2340,   2600,   4860,   5400, /* 30 */
    2600,   2889,   5400,   6000, /* 31 */

    _NV_,   _NV_,     60,     67  /* 32 */ /* BPSK */
};

/* Mode 3 (11ac) VHT, MCS 0...9, CBW 0/1/2/3, SCP 0/1 */
/* Bit0 - SCP, Bits12 - CBW, Bits3456 - MCS */
static const uint16 bitrates_11ac[PHY_STAT_RATE_NSS_MAX][NUM_RATE_MCS_AC * 2 * 4] = {
{  /* NSS = 1 */
     65,     72,    135,    150,    293,    325,    585,    650, /*  0 */
    130,    144,    270,    300,    585,    650,   1170,   1300, /*  1 */
    195,    217,    405,    450,    878,    975,   1755,   1950, /*  2 */
    260,    289,    540,    600,   1170,   1300,   2340,   2600, /*  3 */
    390,    433,    810,    900,   1755,   1950,   3510,   3900, /*  4 */
    520,    578,   1080,   1200,   2340,   2600,   4680,   5200, /*  5 */
    585,    650,   1215,   1350,   2633,   2925,   5265,   5850, /*  6 */
    650,    722,   1350,   1500,   2925,   3250,   5850,   6500, /*  7 */
    780,    867,   1620,   1800,   3510,   3900,   7020,   7800, /*  8 */
   _NV_,   _NV_,   1800,   2000,   3900,   4333,   7800,   8667  /*  9 */
},
{  /* NSS = 2 */
    130,    144,    270,    300,    585,    650,   1170,   1300, /*  0 */
    260,    289,    540,    600,   1170,   1300,   2340,   2600, /*  1 */
    390,    433,    810,    900,   1755,   1950,   3510,   3900, /*  2 */
    520,    578,   1080,   1200,   2340,   2600,   4680,   5200, /*  3 */
    780,    867,   1620,   1800,   3510,   3900,   7020,   7800, /*  4 */
   1040,   1156,   2160,   2400,   4680,   5200,   9360,  10400, /*  5 */
   1170,   1303,   2430,   2700,   5265,   5850,  10530,  11700, /*  6 */
   1300,   1444,   2700,   3000,   5850,   6500,  11700,  13000, /*  7 */
   1560,   1733,   3240,   3600,   7020,   7800,  14040,  15600, /*  8 */
   _NV_,   _NV_,   3600,   4000,   7800,   8667,  15600,  17333  /*  9 */
},
{  /* NSS = 3 */
    195,    217,    405,    450,    878,    975,   1755,   1950, /*  0 */
    390,    433,    810,    900,   1755,   1950,   3510,   3900, /*  1 */
    585,    650,   1215,   1350,   2633,   2925,   5265,   5850, /*  2 */
    780,    867,   1620,   1800,   3510,   3900,   7020,   7800, /*  3 */
   1170,   1300,   2430,   2700,   5265,   5850,  10530,  11700, /*  4 */
   1560,   1733,   3240,   3600,   7020,   7800,  14040,  15600, /*  5 */
   1755,   1950,   3645,   4050,   _NV_,   _NV_,  15795,  17550, /*  6 */
   1950,   2167,   4050,   4500,   8775,   9750,  17550,  19500, /*  7 */
   2340,   2600,   4860,   5400,  10530,  11700,  21060,  23400, /*  8 */
   2600,   2889,   5400,   6000,  11700,  13000,   _NV_,   _NV_  /*  9 */
},
{  /* NSS = 4 */
    260,    289,    540,    600,   1170,   1300,   2340,   2600, /*  0 */
    520,    578,   1080,   1200,   2340,   2600,   4680,   5200, /*  1 */
    780,    867,   1620,   1800,   3510,   3900,   7020,   7800, /*  2 */
   1040,   1156,   2160,   2400,   4680,   5200,   9360,  10400, /*  3 */
   1560,   1733,   3240,   3600,   7020,   7800,  14040,  15600, /*  4 */
   2080,   2311,   4320,   4800,   9360,  10400,  18720,  20800, /*  5 */
   2340,   2600,   4860,   5400,  10530,  11700,  21060,  23400, /*  6 */
   2600,   2889,   5400,   6000,  11700,  13000,  23400,  26000, /*  7 */
   3120,   3467,   6480,   7200,  14040,  15600,  28080,  31200, /*  8 */
   _NV_,   _NV_,   7200,   8000,  15600,  17333,  31200,  34667  /*  9 */
} };

/* Mode 4 (11ax) HE, MCS 0...11, CBW 0/1/2/3, CP 0/1/2 */
static const uint16 bitrates_11ax[PHY_STAT_RATE_NSS_MAX][NUM_RATE_MCS_AX * 4 * HE_CP_TYPES_NUM] = {
{ /* NSS = 1 */
  /*  20     20     20      40     40     40      80     80     80     160    160    160     CBW */
  /* SCP    MCP    LCP     SCP    MCP    LCP     SCP    MCP    LCP     SCP    MCP    LCP *//* MCS*/
      86,    81,    73,    172,   163,   146,    360,   340,   306,    721,   681,   613,  /*  0 */
     172,   163,   146,    344,   325,   293,    721,   681,   613,   1441,  1361,  1225,  /*  1 */
     258,   244,   219,    516,   488,   439,   1081,  1021,   919,   2162,  2042,  1838,  /*  2 */
     344,   325,   293,    688,   650,   585,   1441,  1361,  1225,   2882,  2722,  2450,  /*  3 */
     516,   488,   439,   1032,   975,   878,   2162,  2042,  1838,   4324,  4083,  3675,  /*  4 */
     688,   650,   585,   1376,  1300,  1170,   2882,  2722,  2450,   5765,  5444,  4900,  /*  5 */
     774,   731,   658,   1549,  1463,  1316,   3243,  3063,  2756,   6485,  6125,  5513,  /*  6 */
     860,   813,   731,   1721,  1625,  1463,   3603,  3403,  3063,   7206,  6806,  6125,  /*  7 */
    1032,   975,   878,   2065,  1950,  1755,   4324,  4083,  3675,   8647,  8167,  7350,  /*  8 */
    1147,  1083,   975,   2294,  2167,  1950,   4804,  4537,  4083,   9607,  9074,  8166,  /*  9 */
    1290,  1219,  1097,   2581,  2438,  2194,   5404,  5104,  4594,  10809, 10208,  9188,  /* 10 */
    1434,  1354,  1219,   2868,  2708,  2438,   6004,  5671,  5104,  12010, 11342, 10208,  /* 11 */
},
{ /* NSS = 2 */
     172,   163,   146,    344,   325,   293,    721,   681,   613,   1441,  1361,  1225,  /*  0 */
     344,   325,   293,    688,   650,   585,   1441,  1361,  1225,   2882,  2722,  2450,  /*  1 */
     516,   488,   439,   1032,   975,   878,   2162,  2042,  1838,   4324,  4083,  3675,  /*  2 */
     688,   650,   585,   1376,  1300,  1170,   2882,  2722,  2450,   5765,  5444,  4900,  /*  3 */
    1032,   975,   878,   2065,  1950,  1755,   4324,  4083,  3675,   8647,  8167,  7350,  /*  4 */
    1376,  1300,  1170,   2753,  2600,  2340,   5765,  5444,  4900,  11529, 10889,  9800,  /*  5 */
    1549,  1463,  1316,   3097,  2925,  2633,   6485,  6125,  5513,  12971, 12250, 11025,  /*  6 */
    1721,  1625,  1463,   3441,  3250,  2925,   7206,  6806,  6125,  14412, 13611, 12250,  /*  7 */
    2065,  1950,  1755,   4129,  3900,  3510,   8647,  8167,  7350,  17294, 16333, 14700,  /*  8 */
    2294,  2167,  1950,   4588,  4333,  3900,   9607,  9074,  8166,  19215, 18148, 16333,  /*  9 */
    2581,  2438,  2194,   5162,  4875,  4388,  10809, 10208,  9188,  21618, 20417, 18375,  /* 10 */
    2868,  2708,  2438,   5735,  5417,  4875,  12010, 11343, 10208,  24019, 22685, 20416,  /* 11 */
},
{ /* NSS = 3 */
     258,   244,   219,    516,   488,   439,   1081,  1021,   919,   2162,  2042,  1838,  /*  0 */
     516,   488,   439,   1032,   975,   878,   2162,  2042,  1838,   4324,  4083,  3675,  /*  1 */
     774,   731,   658,   1549,  1463,  1316,   3243,  3063,  2756,   6485,  6125,  5513,  /*  2 */
    1032,   975,   878,   2065,  1950,  1755,   4324,  4083,  3675,   8647,  8167,  7350,  /*  3 */
    1549,  1463,  1316,   3097,  2925,  2633,   6485,  6125,  5513,  12971, 12250, 11025,  /*  4 */
    2065,  1950,  1755,   4129,  3900,  3510,   8647,  8167,  7350,  17294, 16333, 14700,  /*  5 */
    2323,  2194,  1974,   4646,  4388,  3949,   9728,  9188,  8269,  19456, 18375, 16538,  /*  6 */
    2581,  2438,  2194,   5162,  4875,  4388,  10809, 10208,  9188,  21618, 20417, 18375,  /*  7 */
    3097,  2925,  2633,   6194,  5850,  5265,  12971, 12250, 11025,  25941, 24500, 22050,  /*  8 */
    3441,  3250,  2925,   6882,  6500,  5850,  14412, 13611, 12250,  28824, 27222, 24500,  /*  9 */
    3871,  3656,  3291,   7743,  7313,  6581,  16213, 15313, 13781,  32426, 30625, 27563,  /* 10 */
    4301,  4063,  3656,   8603,  8125,  7313,  18015, 17014, 15313,  36029, 34028, 30625,  /* 11 */
},
{ /* NSS = 4 */
     344,   325,   293,    688,   650,   585,   1441,  1361,  1225,   2882,  2722,  2450,  /*  0 */
     688,   650,   585,   1376,  1300,  1170,   2882,  2722,  2450,   5765,  5444,  4900,  /*  1 */
    1032,   975,   878,   2065,  1950,  1755,   4324,  4083,  3675,   8647,  8167,  7350,  /*  2 */
    1376,  1300,  1170,   2753,  2600,  2340,   5765,  5444,  4900,  11529, 10889,  9800,  /*  3 */
    2065,  1950,  1755,   4129,  3900,  3510,   8647,  8167,  7350,  17294, 16333, 14700,  /*  4 */
    2753,  2600,  2340,   5506,  5200,  4680,  11529, 10889,  9800,  23059, 21778, 19600,  /*  5 */
    3097,  2925,  2633,   6194,  5850,  5265,  12971, 12250, 11025,  25941, 24500, 22050,  /*  6 */
    3441,  3250,  2925,   6882,  6500,  5850,  14412, 13611, 12250,  28824, 27222, 24500,  /*  7 */
    4129,  3900,  3510,   8259,  7800,  7020,  17294, 16333, 14700,  34588, 32667, 29400,  /*  8 */
    4588,  4333,  3900,   9176,  8667,  7800,  19215, 18148, 16333,  38431, 36296, 32666,  /*  9 */
    5162,  4875,  4388,  10324,  9750,  8775,  21618, 20417, 18375,  43235, 40833, 36750,  /* 10 */
    5735,  5417,  4875,  11471, 10833,  9750,  24019, 22685, 20416,  48039, 45370, 40833,  /* 11 */
} };

#undef  _NV_

static uint32
_hw_rate_params_to_rate (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  uint32    rate = MTLK_BITRATE_INVALID;
  uint32    idx;

  switch(mode) {
  case PHY_MODE_AG:
    if (mcs < NUM_RATE_MCS_AG) {
        rate = bitrates_11ag[mcs];
    }
    break;

  case PHY_MODE_B:
    if (mcs < NUM_RATE_MCS_B) {
        rate = bitrates_11b[mcs];
    }
    break;

  case PHY_MODE_N:
    if (cbw < 2) { /* CBW 0/1 */
        if (mcs < NUM_RATE_MCS_N) {
            idx  = scp | (cbw << 1) | (mcs << 2);
            rate = bitrates_11n[idx];
        }
    }
    break;

  case PHY_MODE_AC: /* CBW 0/1/2/3 */
    if (mcs < NUM_RATE_MCS_AC && nss <= PHY_STAT_RATE_NSS_MAX && nss >= PHY_STAT_RATE_NSS_MIN) {
        idx  = scp | (cbw << 1) | (mcs << 3);
        rate = bitrates_11ac[nss - 1][idx];
    }
    break;

  case PHY_MODE_AX_SU_EXT:
   if (mcs > (NUM_RATE_MCS_AX_SU_EXT - 1))
     break;
   FALLTHROUGH;

  case PHY_MODE_AX: /* CBW 0/1/2/3 */
    /* scp means cp_mode */
    if ((scp < HE_CP_MODES_NUM) && (mcs < NUM_RATE_MCS_AX) && (nss > 0) && (nss <= PHY_STAT_RATE_NSS_MAX)) {
      uint8 cp_type = he_cp_mode_to_cp_type_table[scp];
      idx = cp_type + (HE_CP_TYPES_NUM * (cbw + (PHY_STAT_RATE_CBW_NUM * mcs)));
      rate = bitrates_11ax[nss - 1][idx];
    }
    break;
  default:
    break;
  };

  ILOG2_DDDDDD("mcs %2d, mode %d, cbw %d, scp %d, nss %d, rate %d", mcs, mode, cbw, scp, nss, rate);

  return rate;
}

uint32 __MTLK_IFUNC
mtlk_bitrate_hw_params_to_rate (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  uint32 rate = _hw_rate_params_to_rate(mode, cbw, scp, mcs, nss);
  if (MTLK_BITRATE_INVALID == rate) {
    WLOG_DDDDD("Rate is not found: mcs %2d, mode %d, cbw %d, scp %d, nss %d", mcs, mode, cbw, scp, nss);
  }
  return rate;
}

BOOL __MTLK_IFUNC
mtlk_bitrate_hw_params_supported (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  return (MTLK_BITRATE_INVALID != _hw_rate_params_to_rate (mode, cbw, scp, mcs, nss));
}

BOOL __MTLK_IFUNC
mtlk_bitrate_hw_params_supported_rate (uint32 *rate, uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  uint32 rate_value   = _hw_rate_params_to_rate (mode, cbw, scp, mcs, nss);
  BOOL   is_supported = (MTLK_BITRATE_INVALID != rate_value);
  if (is_supported && (rate != NULL)) {
    *rate = rate_value;
  }
  return is_supported;
}

/* Input:   index - bit number of OperateRateSet or BasicRateSet
 *          sm    - CBW [ 0 - 20MHz, 1 - 40MHz, 2 - 80MHz, 3 - 160MHz ]
*/
int __MTLK_IFUNC
mtlk_bitrate_get_value (int index, int sm, int scp)
{
  int phy; /* PHY mode */
  int mcs;
  int value;

  ASSERT((BITRATE_FIRST <= index) && (index <= BITRATE_LAST));
  ASSERT((0 <= sm) || (sm <= 3));
  ASSERT((0 == scp) || (1 == scp));

  if (LM_PHY_11N_RATE_6_DUP == index) {
      index -= 1;       /* dup of previous */
  }

  /* Calculate PHY mode and MCS by index and sm */
  if (index >= LM_PHY_11N_RATE_OFFS) {
      mcs = index - LM_PHY_11N_RATE_OFFS;
      /* select AC mode in case of sm > 1, i.e. BW > 40 MHz */
      phy =  (sm > 1) ? PHY_MODE_AC : PHY_MODE_N;
  } else if (index >= LM_PHY_11B_RATE_OFFS) {
      mcs = index - LM_PHY_11B_RATE_OFFS;
      mcs += 1;         /* remap index 0...7 to MCS 1...8 */
      phy = PHY_MODE_B;
  } else { /* LM_PHY_11A_RATE_OFFS = 0 */
      mcs = index;
      phy = PHY_MODE_AG;
  }

  value = _hw_rate_params_to_rate (phy, sm, scp, mcs, 1 /* nss */);
  if (value != MTLK_BITRATE_INVALID) {
    /* In table values in fixed-point representation: 60 -> 6.0 Mbit/s */
    value = 100 * 1000 * value;
  }

  return value;
}
static uint8
_hw_rate_params_to_rate_idx (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  uint8 idx = MTLK_BITRATE_IDX_INVALID;

  switch (mode)
  {
  case PHY_MODE_AG:
    if (mcs < NUM_RATE_MCS_AG)
    {
      idx = mcs;
    }
    break;

  case PHY_MODE_B:
    if (mcs < NUM_RATE_MCS_B)
    {
      idx = mcs;
    }
    break;

  case PHY_MODE_N:
    if (cbw < 2)
    { /* CBW 0/1 */
      if (mcs < NUM_RATE_MCS_N)
      {
        idx = scp | (cbw << 1) | (mcs << 2);
      }
    }
    break;

  case PHY_MODE_AC: /* CBW 0/1/2/3 */
    if (mcs < NUM_RATE_MCS_AC && nss <= PHY_STAT_RATE_NSS_MAX && nss >= PHY_STAT_RATE_NSS_MIN)
    {
      idx = scp | (cbw << 1) | (mcs << 3);
    }
    break;

  default:
    break;
  };

  if (idx == MTLK_BITRATE_IDX_INVALID) {
    ILOG1_DDDDD("Rate idx is not found: mcs %2d, mode %d, cbw %d, scp %d, nss %d", mcs, mode, cbw, scp, nss);
  }

  return idx;
}

#ifdef UNUSED_CODE

uint8 __MTLK_IFUNC
mtlk_bitrate_hw_params_to_rate_idx (uint32 mode, uint32 cbw, uint32 scp, uint32 mcs, uint32 nss)
{
  return _hw_rate_params_to_rate_idx(mode, cbw, scp, mcs, nss);
}

#endif /* UNUSED_CODE */

uint8 __MTLK_IFUNC
mtlk_bitrate_info_to_rate_idx (mtlk_bitrate_info_t bitrate_info)
{
  if (__UNLIKELY(MTLK_BITRATE_INFO_INVALID == bitrate_info)) {
    return MTLK_BITRATE_IDX_INVALID;
  } else if (!mtlk_bitrate_info_get_flag(bitrate_info)) {
    return MTLK_BITRATE_IDX_INVALID;
  } else {
    return _hw_rate_params_to_rate_idx(
                    mtlk_bitrate_info_get_mode(bitrate_info),
                    mtlk_bitrate_info_get_cbw(bitrate_info),
                    mtlk_bitrate_info_get_scp(bitrate_info),
                    mtlk_bitrate_info_get_mcs(bitrate_info),
                    mtlk_bitrate_info_get_nss(bitrate_info));
  }
}
