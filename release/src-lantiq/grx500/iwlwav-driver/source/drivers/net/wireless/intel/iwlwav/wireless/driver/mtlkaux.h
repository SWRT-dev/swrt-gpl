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
 * $Id:
 *
 * 
 *
 * Shared Auxiliary routines
 *
 */

#ifndef _MTLK_AUX_H
#define _MTLK_AUX_H

#include "mhi_umi.h"

/*
 * These magic numbers we got from WEB team.
 * Should be replaced by meaningful protocol names.
 */
enum {
  NETWORK_MODE_11AN = 14,
  NETWORK_MODE_11A = 10,
  NETWORK_MODE_11N5 = 12,
  NETWORK_MODE_11BGN = 23,
  NETWORK_MODE_11BG = 19,
  NETWORK_MODE_11B = 17,
  NETWORK_MODE_11G = 18,
  NETWORK_MODE_11N2 = 20,
  NETWORK_MODE_11GN = 22,
  NETWORK_MODE_11ABGN = 30,
  NETWORK_MODE_11ABG = 0,
};

typedef enum _MTLK_NETWORK_MODES
{
  MTLK_NETWORK_11B_ONLY,
  MTLK_NETWORK_11G_ONLY,
  MTLK_NETWORK_11N_2_4_ONLY,
  MTLK_NETWORK_11BG_MIXED,
  MTLK_NETWORK_11GN_MIXED,
  MTLK_NETWORK_11BGN_MIXED,
  MTLK_NETWORK_11A_ONLY,
  MTLK_NETWORK_11N_5_ONLY,
  MTLK_NETWORK_11AN_MIXED,
  MTLK_NETWORK_11AX_6G_ONLY,
  MTLK_NETWORK_11ABG_MIXED,
  MTLK_NETWORK_11ABGN_MIXED,
  /* this is not "driver-only for sta", it's for the iwconfig support when in 80 MHz */
  MTLK_NETWORK_11ANAC_MIXED,
  MTLK_NETWORK_11ANACAX_MIXED,
  MTLK_NETWORK_11BGNAX_MIXED,
  MTLK_NETWORK_11GNAX_MIXED,
  MTLK_NETWORK_NONE
} MTLK_NETWORK_MODES_E;

#define LM_PHY_11A_RATE_MSK             0x000000FF
#define LM_PHY_11B_RATE_MSK             0x00007F00
#define LM_PHY_11B_SHORT_RATE_MSK       0x00000700
#define LM_PHY_11B_LONG_RATE_MSK        (LM_PHY_11B_RATE_MSK & (~LM_PHY_11B_SHORT_RATE_MSK))
#define LM_PHY_11N_RATE_MSK             0xFFFF8000
#define LM_PHY_11N_MIMO_RATE_MSK        0x7F800000
#define LM_PHY_11N_NO_MIMO_RATE_MSK     0x007F8000
#define LM_PHY_11G_RATE_MSK             (LM_PHY_11A_RATE_MSK|LM_PHY_11B_RATE_MSK)

#define LM_PHY_11A_RATE_OFFS        0
#define LM_PHY_11B_RATE_OFFS        8
#define LM_PHY_11N_RATE_OFFS        15

#define LM_PHY_11N_RATE_6_5      15// ,  11N    15     0x40
#define LM_PHY_11N_RATE_6_DUP    31//    11N    31     0x50

static __INLINE int
mtlk_aux_is_11n_rate (uint8 rate)
{
  return (rate >= LM_PHY_11N_RATE_6_5 &&
          rate <= LM_PHY_11N_RATE_6_DUP);
}

uint32 __MTLK_IFUNC get_operate_rate_set (uint8 net_mode);

uint8 __MTLK_IFUNC get_net_mode (uint8 band, uint8 is_ht);
uint8 __MTLK_IFUNC net_mode_to_band (uint8 net_mode);
BOOL __MTLK_IFUNC is_ht_net_mode (uint8 net_mode);
const char * __MTLK_IFUNC net_mode_to_string (uint8 net_mode);

#endif // _MTLK_AUX_H
