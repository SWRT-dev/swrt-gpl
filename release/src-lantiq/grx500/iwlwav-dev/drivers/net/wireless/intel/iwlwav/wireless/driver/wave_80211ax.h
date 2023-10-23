/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 * 
 *
 * 802.11 AX D3.0 related definitions.
 *
 * Originally written by Andrei Grosmanis
 *
 */

#ifndef __WAVE_80211AX_H_
#define __WAVE_80211AX_H_

#define HE_EXTENSION_LEN 0
#define HE_MAC_LEN 6
#define HE_PHY_LEN 11
#define HE_MAC_PHY_LEN (HE_EXTENSION_LEN + HE_MAC_LEN + HE_PHY_LEN)
#define HE_MCS_NSS_80MHZ_LEN 4
#define HE_MCS_NSS_160MHZ_LEN (HE_MCS_NSS_80MHZ_LEN)
#define HE_MCS_NSS_8080MHZ_LEN (HE_MCS_NSS_160MHZ_LEN)
#define HE_MCS_NSS_MAX_LEN (HE_MCS_NSS_80MHZ_LEN + HE_MCS_NSS_160MHZ_LEN + HE_MCS_NSS_8080MHZ_LEN)
#define HE_PPE_TH_MAX_LEN 13
#define HE_OPERATION_PARAMETERS_LEN 11 /* "HE Operation Parameters" field */

#define HE_MAC_PHY_OFFSET 0
#define HE_MCS_NSS_OFFSET (HE_MAC_PHY_OFFSET + HE_MAC_PHY_LEN)
#define HE_PPE_TH_80_MHZ_OFFSET (HE_MCS_NSS_OFFSET + HE_MCS_NSS_80MHZ_LEN)
#define HE_PPE_TH_160_MHZ_OFFSET (HE_PPE_TH_80_MHZ_OFFSET + HE_MCS_NSS_160MHZ_LEN)
#define HE_PPE_TH_8080_MHZ_OFFSET (HE_PPE_TH_160_MHZ_OFFSET + HE_MCS_NSS_8080MHZ_LEN)
#define HE_OPERATION_PARAMETERS_OFFSET (HE_PPE_TH_8080_MHZ_OFFSET + HE_PPE_TH_MAX_LEN)
#define HE_MAC_PHY_CHAN_WIDTH_OFFSET 7
#define HE_MAC_PHY_CHAN_WIDTH_B2 3
#define HE_MAC_PHY_CHAN_WIDTH_B3 4

#define HE_MAC_TWT_RESPONDER_SUPPORT        2
#define HE_MAC_TWT_RESPONDER_SUPPORT_WIDTH  1
#define HE_MAC_TWT_RESPONDER_NOT_SUPPORTED  0
#define HE_MAC_TWT_RESPONDER_SUPPORTED      1

#define IE_HE_LENGTH_LEN 1
#define IE_HE_MAC_TWT_RESPONDER_OFFSET     (HE_EXTENSION_LEN + IE_HE_LENGTH_LEN + 1)
#define IE_HE_PHY_CHANNEL_WIDTH_SET_OFFSET (HE_EXTENSION_LEN + IE_HE_LENGTH_LEN + HE_MAC_LEN)
#define IE_LENGTH_ELEM_ID_LEN 1
#define IE_HE_MCS_NSS_OFFSET (IE_LENGTH_ELEM_ID_LEN + IE_HE_LENGTH_LEN + HE_MAC_PHY_LEN)

/* HE Operation element definitions */
#define HE_OPERATION_BSS_COLOR_DISABLED_FLAG    0x80 /* The BSS Color Disabled subfield is set to 1 to temporarily disable the use of color for the BSS */
#define HE_OPERATION_BSS_COLOR_MASK             0x3F /* Mask for BSS color value */
#define HE_OPERATION_BSS_COLOR_DFLT             0x4  /* Default BSS color value */
#define HE_OPERATION_BSS_COLOR_5GHZ_6GHZ_2STS   0x5  /* BSS color value for 5GHz/6GHz band and 2 STS */
#define HE_OPERATION_MAX_COHOSTED_BSS_IND_2STS  0x4
#define HE_OPERATION_MAX_COHOSTED_BSS_IND_4STS  0x5
#define HE_OPERATION_ER_SU_DISABLE_SET          0x1
#define HE_OPERATION_ER_SU_DISABLE_UNSET        0x0

#define HE_OPERATION_CAP0 0xf4
#define HE_OPERATION_CAP1 0x81
#define HE_OPERATION_CAP2 HE_OPERATION_ER_SU_DISABLE_SET
#define HE_OPERATION_CAP3 HE_OPERATION_BSS_COLOR_DFLT
#define HE_OPERATION_CAP4 0xfc
#define HE_OPERATION_CAP5 0xff
#define HE_OPERATION_CAP6 HE_OPERATION_MAX_COHOSTED_BSS_IND_2STS

/* HE non advertised capabilities definitions */
#define HE_NON_ADVERT_MAC_CAP0    0x01
#define HE_NON_ADVERT_MAC_CAP1    0x0b
#define HE_NON_ADVERT_MAC_CAP2    0x00
#define HE_NON_ADVERT_MAC_CAP3    0x12
#define HE_NON_ADVERT_MAC_CAP3_D2 0x18
#define HE_NON_ADVERT_MAC_CAP4    0x00
#define HE_NON_ADVERT_MAC_CAP5    0x08

#define HE_NON_ADVERT_PHY_CAP0    0x00
#define HE_NON_ADVERT_PHY_CAP1    0x10
#define HE_NON_ADVERT_PHY_CAP1_D2 0x1F
#define HE_NON_ADVERT_PHY_CAP2    0x02
#define HE_NON_ADVERT_PHY_CAP3    0x00
#define HE_NON_ADVERT_PHY_CAP3_D2 0x09
#define HE_NON_ADVERT_PHY_CAP4    0x00
#define HE_NON_ADVERT_PHY_CAP5    0xc0
#define HE_NON_ADVERT_PHY_CAP6    0x03
#define HE_NON_ADVERT_PHY_CAP7    0x1c
#define HE_NON_ADVERT_PHY_CAP8    0x08
#define HE_NON_ADVERT_PHY_CAP8_D2 0x09

#define HE_MAC_CAP0_IDX 0
#define HE_MAC_CAP1_IDX 1
#define HE_MAC_CAP2_IDX 2
#define HE_MAC_CAP3_IDX 3
#define HE_MAC_CAP4_IDX 4
#define HE_MAC_CAP5_IDX 5

#define HE_PHY_CAP0_IDX 0
#define HE_PHY_CAP1_IDX 1
#define HE_PHY_CAP2_IDX 2
#define HE_PHY_CAP3_IDX 3
#define HE_PHY_CAP4_IDX 4
#define HE_PHY_CAP5_IDX 5
#define HE_PHY_CAP6_IDX 6
#define HE_PHY_CAP7_IDX 7
#define HE_PHY_CAP8_IDX 8

#endif /* __WAVE_80211AX_H_ */
