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
#include "mtlkinc.h"

#include "mtlkaux.h"
#include "eeprom.h"

#define LOG_LOCAL_GID   GID_AUX
#define LOG_LOCAL_FID   1

static const uint32 OperateRateSet[] = {
  0x00007f00, /* 11b@2.4GHz */
  0x000000ff, /* 11g@2.4GHz */
  0xffffffff, /* 11n@2.4GHz */
  0x00007fff, /* 11bg@2.4GHz */
  0xffff80ff, /* 11gn@2.4GHz */
  0xffffffff, /* 11bgn@2.4GHz */
  0x000000ff, /* 11a@5.2GHz */
  0xffff80ff, /* 11n@5.2GHz */
  0xffff80ff, /* 11an@5.2GHz */
  /*TODO: Review this for 6GHz Band support */
  0xffff80ff, /* 11ax@6.0GHz */

  /* these ARE NOT real network modes. they are for
     unconfigured dual-band STA which may be MTLK_NETWORK_11ABG_MIXED
     or MTLK_NETWORK_11ABGN_MIXED */
  0x00007fff, /* 11abg */
  0xffffffff, /* 11abgn */
  0x01ff80ff, /* 11anac@5.2GHz MCS [0...9] */
  0x01ff80ff, /* 11anacax@5.2GHz MCS [0...9] */
  0xffffffff, /* 11bgnax@2.4Hz */
  0xffff80ff, /* 11gnax@2.4Hz */
};

uint32 __MTLK_IFUNC
get_operate_rate_set (uint8 net_mode)
{
  MTLK_STATIC_ASSERT(MTLK_NETWORK_NONE == ARRAY_SIZE(OperateRateSet));
  ASSERT(net_mode < MTLK_NETWORK_NONE);
  return OperateRateSet[net_mode];
}

/*
 * It should be noted, that
 * get_net_mode(net_mode_to_band(net_mode), is_ht_net_mode(net_mode)) != net_mode
 * because there are {ht, !ht}x{2.4GHz, 5.2GHz, both} == 6 combinations,
 * and there are 11 Network Modes.
 */
uint8 __MTLK_IFUNC get_net_mode (uint8 band, uint8 is_ht)
{
  switch (band) {
  case MTLK_HW_BAND_2_4_GHZ:
    if (is_ht)
      return MTLK_NETWORK_11BGN_MIXED;
    else
      return MTLK_NETWORK_11BG_MIXED;
  case MTLK_HW_BAND_5_2_GHZ:
    if (is_ht)
      return MTLK_NETWORK_11AN_MIXED;
    else
      return MTLK_NETWORK_11A_ONLY;
  case MTLK_HW_BAND_2_4_5_2_GHZ:
    if (is_ht)
      return MTLK_NETWORK_11ABGN_MIXED;
    else
      return MTLK_NETWORK_11ABG_MIXED;

  case MTLK_HW_BAND_6_GHZ:
  case MTLK_HW_BAND_2_4_6_GHZ:
  case MTLK_HW_BAND_5_2_6_GHZ:
      return MTLK_NETWORK_11AX_6G_ONLY;
  default:
    break;
  }

  ASSERT(0);

  return 0; /* just fake cc */
}

uint8 __MTLK_IFUNC net_mode_to_band (uint8 net_mode)
{
  switch (net_mode) {
  case MTLK_NETWORK_11BG_MIXED: /* walk through */
  case MTLK_NETWORK_11BGN_MIXED: /* walk through */
  case MTLK_NETWORK_11B_ONLY: /* walk through */
  case MTLK_NETWORK_11GN_MIXED: /* walk through */
  case MTLK_NETWORK_11G_ONLY: /* walk through */
  case MTLK_NETWORK_11N_2_4_ONLY:
  case MTLK_NETWORK_11GNAX_MIXED:
  case MTLK_NETWORK_11BGNAX_MIXED:
    return MTLK_HW_BAND_2_4_GHZ;

  case MTLK_NETWORK_11ANAC_MIXED: /* walk through */
  case MTLK_NETWORK_11ANACAX_MIXED: /* walk through */
  case MTLK_NETWORK_11AN_MIXED:  /* walk through */
  case MTLK_NETWORK_11A_ONLY:    /* walk through */
  case MTLK_NETWORK_11N_5_ONLY:
    return MTLK_HW_BAND_5_2_GHZ;

  case MTLK_NETWORK_11ABG_MIXED: /* walk through */
  case MTLK_NETWORK_11ABGN_MIXED:
    return MTLK_HW_BAND_2_4_5_2_GHZ;

/* TODO: To handle the multi radio configuration with 6GHz */
  case MTLK_NETWORK_11AX_6G_ONLY:
    return MTLK_HW_BAND_6_GHZ;

  default:
    break;
  }

  ASSERT(0);

  return 0; /* just fake cc */
}

BOOL __MTLK_IFUNC is_ht_net_mode (uint8 net_mode)
{
  switch (net_mode) {
  case MTLK_NETWORK_11ABG_MIXED: /* walk through */
  case MTLK_NETWORK_11A_ONLY: /* walk through */
  case MTLK_NETWORK_11BG_MIXED: /* walk through */
  case MTLK_NETWORK_11B_ONLY: /* walk through */
  case MTLK_NETWORK_11G_ONLY:
    return FALSE;
  case MTLK_NETWORK_11ABGN_MIXED: /* walk through */
  case MTLK_NETWORK_11ANAC_MIXED: /* walk through */
  case MTLK_NETWORK_11ANACAX_MIXED: /* walk through */
  case MTLK_NETWORK_11AN_MIXED: /* walk through */
  case MTLK_NETWORK_11BGN_MIXED: /* walk through */
  case MTLK_NETWORK_11GN_MIXED: /* walk through */
  case MTLK_NETWORK_11N_2_4_ONLY: /* walk through */
  case MTLK_NETWORK_11GNAX_MIXED:
  case MTLK_NETWORK_11BGNAX_MIXED:
  case MTLK_NETWORK_11N_5_ONLY:
  case MTLK_NETWORK_11AX_6G_ONLY:
    return TRUE;
  default:
    break;
  }

  ASSERT(0);

  return FALSE; /* just fake cc */
}

const char * __MTLK_IFUNC
net_mode_to_string (uint8 net_mode)
{
  switch (net_mode) {
  case MTLK_NETWORK_11B_ONLY:
    return "802.11b";
  case MTLK_NETWORK_11G_ONLY:
    return "802.11g";
  case MTLK_NETWORK_11N_2_4_ONLY:
    return "802.11n(2.4)";
  case MTLK_NETWORK_11BG_MIXED:
    return "802.11bg";
  case MTLK_NETWORK_11GN_MIXED:
    return "802.11gn";
  case MTLK_NETWORK_11BGN_MIXED:
    return "802.11bgn"; 
  case MTLK_NETWORK_11GNAX_MIXED:
    return "802.11gnax";
  case MTLK_NETWORK_11BGNAX_MIXED:
    return "802.11bgnax";
  case MTLK_NETWORK_11A_ONLY:
    return "802.11a";
  case MTLK_NETWORK_11N_5_ONLY:
    return "802.11n(5.2)";
  case MTLK_NETWORK_11AN_MIXED:
    return "802.11an";
  case MTLK_NETWORK_11ABG_MIXED:
    return "802.11abg";
  case MTLK_NETWORK_11ABGN_MIXED:
    return "802.11abgn";
  case MTLK_NETWORK_11ANAC_MIXED:
    return "802.11anac";
  case MTLK_NETWORK_11ANACAX_MIXED:
    return "802.11anacax";
  case MTLK_NETWORK_11AX_6G_ONLY:
    return "802.11ax";
  }

  return "invalid mode";
}

