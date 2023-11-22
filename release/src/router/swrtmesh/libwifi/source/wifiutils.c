/*
 * util.c - wifi helper functions
 *
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <endian.h>
#include <arpa/inet.h>

#include <easy/easy.h>
#include "wifiutils.h"

unsigned char microsoft_oui[] = { 0x00, 0x50, 0xf2 };

bool __is_any_bit_set(unsigned int v, int sizeof_v, ...)
{
	bool ret = false;
	va_list args;
	uint32_t b;
	int i = 0;

	va_start(args, sizeof_v);

	while (i++ < sizeof_v) {
		b = (uint32_t)va_arg(args, int);
		if (b > sizeof_v - 1)
			goto out;

		if (!!(v & BIT(b))) {
			ret = true;
			goto out;
		}
	}

out:
	va_end(args);
	return ret;
}

bool __is_all_bits_set(unsigned int v, int sizeof_v, ...)
{
	bool ret = false;
	va_list args;
	uint32_t b;
	int i = 0;

	va_start(args, sizeof_v);

	while (i++ < sizeof_v) {
		b = (uint32_t)va_arg(args, int);
		if (b > sizeof_v - 1)
			goto out;

		if (!(v & BIT(b))) {
			ret = false;
			goto out;
		}

		ret = true;
	}

out:
	va_end(args);
	return ret;
}

int wifi_channel_to_freq(int chan)
{
	if (chan <= 0)
		return -EINVAL;

	if (chan <= 14)
		return (chan < 14) ?
			2407 + chan * 5 :
			2484;

	if (chan > 32 && chan <= 196)
		return (chan >= 182) ? 4000 + chan * 5 : 5000 + chan * 5;

	return -EINVAL;
}

int wifi_channel_to_freq_ex(int chan, enum wifi_band band)
{
	if (chan <= 0)
		return -EINVAL;

	switch (band) {
	case BAND_2:
		if (chan <= 14)
			return (chan < 14) ?
				2407 + chan * 5 :
				2484;
		break;
	case BAND_5:
		if (chan > 32 && chan <= 196)
			return (chan >= 182) ?
				4000 + chan * 5 :
				5000 + chan * 5;
		break;
	case BAND_6:
		if (chan >= 1 && chan <= 233)
			return 5950 + chan * 5;
		break;
	default:
		break;
	}

	return -EINVAL;
}

int wifi_freq_to_channel(int freq)
{
	if (freq == 2484)
		return 14;

	if (freq < 2484)
		return (freq - 2407) / 5;

	if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;

	if (freq >= 5150 && freq <= 5865)
		return (freq - 5000) / 5;

	if (freq > 5950 && freq <= 7115)
		return (freq - 5950) / 5;

	return -EINVAL;
}

uint8_t *wifi_find_ie(uint8_t *ies, size_t len, uint8_t eid)
{
	uint8_t *end;

	if (!ies || len < 2)
		return NULL;

	end = ies + len;
	while (end - ies > 1) {
		if (ies + ies[1] + 2 > end)
			return NULL;

		if (ies[0] == eid)
			return ies;

		ies += ies[1] + 2;
	}

	return NULL;
}

uint8_t *wifi_find_ie_ext(uint8_t *ies, size_t len, uint8_t ext_id)
{
	uint8_t *end;

	if (!ies || len < 2)
		return NULL;

	end = ies + len;
	while (end - ies > 1) {
		if (ies + ies[1] + 2 > end)
			return NULL;

		if (ies[0] == 0xff && ies[2] == ext_id)
			return ies;

		ies += ies[1] + 2;
	}

	return NULL;
}

uint8_t *wifi_find_vsie(uint8_t *ies, size_t len, uint8_t *oui,
					uint8_t type, uint8_t stype)
{
	uint8_t *vsie;
	uint8_t *end;


	if (!ies || !oui || len < 2)
		return NULL;

	end = ies + len;
	while (end - ies > 1) {
		if (ies + ies[1] + 2 > end)
			return NULL;

		vsie = ies;
		ies += ies[1] + 2;

		if (vsie[0] == 0xdd && !memcmp(&vsie[2], oui, 3)) {
			if (type != 0xff && vsie[5] != type)
				continue;

			if (stype == 0xff || vsie[6] == stype)
				return vsie;
		}
	}

	return NULL;
}

void wifi_cap_set_from_capability_information(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	if (!bitmap || !ie || ie_len < 2)
		return;
	if (!!(ie[0] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_ESS);
	if (!!(ie[0] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_IBSS);
	if (!!(ie[0] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_SHORT_PREAMBLE);

	if (!!(ie[1] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_SPECTRUM_MGMT);
	if (!!(ie[1] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_WMM);
	if (!!(ie[1] & bit(2)))
		wifi_cap_set(bitmap, WIFI_CAP_SHORT_SLOT);
	if (!!(ie[1] & bit(3)))
		wifi_cap_set(bitmap, WIFI_CAP_APSD);
	if (!!(ie[1] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_RADIO_MEAS);
	if (!!(ie[1] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_DELAYED_BA);
	if (!!(ie[1] & bit(7)))
		wifi_cap_set(bitmap, WIFI_CAP_IMMEDIATE_BA);
}

void wifi_cap_set_from_ht_capabilities_info(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	if (!bitmap || !ie || ie_len < 2)
		return;
	if (!!(ie[0] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_HT_LDPC);

	if (!!(ie[0] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_2040);

	if (!!(ie[0] & 0xc))
		wifi_cap_set(bitmap, WIFI_CAP_HT_SMPS);

	if (!!(ie[0] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_HT_GREENFIELD);

	if (!!(ie[0] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_SGI20);

	if (!!(ie[0] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_SGI40);

	if (!!(ie[0] & bit(7)))
		wifi_cap_set(bitmap, WIFI_CAP_HT_TX_STBC);

	if (!!(ie[1] & 0x3))
		wifi_cap_set(bitmap, WIFI_CAP_HT_RX_STBC);
}

void wifi_cap_set_from_vht_capabilities_info(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	int vht_mpdu = 0;
	int vht_rx_stbc = 0;
	int vht_chw = 0;
	int vht_nss = 0;

	if (!bitmap || !ie || ie_len < 4)
		return;

	/* vht */
	vht_mpdu = ie[0] & 0x3;
	if (vht_mpdu == 0)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_MPDU_3895);
	else if (vht_mpdu == 1)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_MPDU_7991);
	else if (vht_mpdu == 2)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_MPDU_11454);

	vht_chw = ie[0] & 0xc;

	if (!!(ie[0] & bit(4))) {
		wifi_cap_set(bitmap, WIFI_CAP_VHT_RX_LDPC);
	}

	if (!!(ie[0] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_SGI80);

	if (!!(ie[0] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_SGI160);

	if (!!(ie[0] & bit(7)))
		wifi_cap_set(bitmap, WIFI_CAP_VHT_TX_STBC);

	vht_rx_stbc = ie[1] & 0x7;
	if (vht_rx_stbc == 1)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_RX_STBC_1SS);
	else if (vht_rx_stbc == 2)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_RX_STBC_2SS);
	else if (vht_rx_stbc == 3)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_RX_STBC_3SS);
	else if (vht_rx_stbc == 4)
		wifi_cap_set(bitmap, WIFI_CAP_VHT_RX_STBC_4SS);

	if (!!(ie[1] & bit(3)))
		wifi_cap_set(bitmap, WIFI_CAP_VHT_SU_BFR);

	if (!!(ie[1] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_VHT_SU_BFE);

	if (!!(ie[2] & bit(3)))
		wifi_cap_set(bitmap, WIFI_CAP_VHT_MU_BFR);

	if (!!(ie[2] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_VHT_MU_BFE);

	vht_nss = ie[3] & 0xc;
	if (vht_chw >= 1)
		wifi_cap_set(bitmap, WIFI_CAP_160);
	if (vht_chw >= 2 || (vht_chw == 1 && vht_nss >= 3))
		wifi_cap_set(bitmap, WIFI_CAP_8080);
}

void wifi_cap_set_from_extended_capabilities(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	if (!bitmap || !ie || ie_len < 1)
		return;
	if (!!(ie[0] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_2040_COEX);

	if (!!(ie[0] & bit(2)))
		wifi_cap_set(bitmap, WIFI_CAP_EXT_CHSWITCH);

	if (!!(ie[0] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_PSMP);

	if (!!(ie[1] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_PROXY_ARP);

	if (!!(ie[2] & bit(3)))
		wifi_cap_set(bitmap, WIFI_CAP_11V_BSS_TRANS);

	if (!!(ie[2] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_MULTI_BSSID);

	if (!!(ie[3] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_SSID_LIST);

	if (!!(ie[3] & bit(7)))
		wifi_cap_set(bitmap, WIFI_CAP_INTERWORKING);

	if (ie_len < 5)
		return;

	if (!!(ie[4] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_QOSMAP);

	if (!!(ie[4] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_TDLS);

	if (ie_len < 7)
		return;

	if (!!(ie[6] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_SCS);

	if (!!(ie[6] & bit(7)))
		wifi_cap_set(bitmap, WIFI_CAP_QLOAD_REPORT);

	if (ie_len < 8)
		return;

	if (!!(ie[7] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_OMI);

	if (ie_len < 10)
		return;

	if (!!(ie[9] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_TWT_REQ);

	if (!!(ie[9] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_TWT_RSP);

	if (ie_len < 11)
		return;

	if (!!(ie[10] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_MSCS);

}

void wifi_cap_set_from_rm_enabled_capabilities(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	if (!bitmap || !ie || ie_len < 3)
		return;
	if (!!(ie[0] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_LINK);
	if (!!(ie[0] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_NBR_REPORT);
	if (!!(ie[0] & bit(4)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_BCN_PASSIVE);
	if (!!(ie[0] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_BCN_ACTIVE);
	if (!!(ie[0] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_BCN_TABLE);
	if (!!(ie[3] & bit(5)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_RCPI);
	if (!!(ie[3] & bit(6)))
		wifi_cap_set(bitmap, WIFI_CAP_RM_RSNI);
}

void wifi_cap_set_from_fast_bss_transition(uint8_t *bitmap, const uint8_t *ie, size_t ie_len)
{
	if (!bitmap || !ie || ie_len < 3)
		return;

	if (!!(ie[2] & bit(0)))
		wifi_cap_set(bitmap, WIFI_CAP_FT_BSS);
	if (!!(ie[2] & bit(1)))
		wifi_cap_set(bitmap, WIFI_CAP_FT_RRP);
}

void wifi_cap_set_from_he(uint8_t *bmp, const uint8_t *ie, size_t ielen)
{
	UNUSED(ielen);

	if (!!(ie[0] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TWT_REQ);

	if (!!(ie[0] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TWT_RSP);

	if (!!(ie[2] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_ALL_ACK);

	if (!!(ie[2] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TRS);

	if (!!(ie[2] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_BSR);

	if (!!(ie[2] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_BCAST_TWT);

	if (!!(ie[2] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_32BIT_BA_BMP);

	if (!!(ie[3] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_OM_CONTROL);

	if (!!(ie[3] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_OFDMA_RA);

	if (!!(ie[3] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_AMSDU_FRAG);

	if (!!(ie[3] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_FLEX_TWT);

	if (!!(ie[4] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_QTP);

	if (!!(ie[4] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_BQR);

	if (!!(ie[4] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_SRP_RSP);

	if (!!(ie[4] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_OPS);

	if (!!(ie[4] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_AMSDU_IN_AMPDU);

	if (!!(ie[5] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_DYN_SMPS);

	/* phy caps */
	if (!!(ie[6] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_40_BAND2);

	if (!!(ie[6] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_4080_BAND5);

	if (!!(ie[6] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_160_BAND5);

	if (!!(ie[6] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_160_8080_BAND5);

	if (!!(ie[6] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_242RU_BAND2);

	if (!!(ie[6] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_242RU_BAND5);

	if (!!(ie[7] & bit(0)) || !!(ie[7] & bit(1)) || !!(ie[7] & bit(2)) || !!(ie[7] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PREAMBLE_PUNC_RX);

	if (!!(ie[7] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_LDPC_PAYLOAD);

	if (!!(ie[8] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_STBC_TX_80);

	if (!!(ie[8] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_STBC_RX_80);

	if (!!(ie[8] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_DOPPLER_TX);

	if (!!(ie[8] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_DOPPLER_RX);

	if (!!(ie[8] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_FULL_BW_UL_MUMIMO);

	if (!!(ie[8] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PART_BW_UL_MUMIMO);

	if (!!(ie[9] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_SU_BFR);

	if (!!(ie[10] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_HE_SU_BFE);

	if (!!(ie[10] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_MU_BFR);

	if (!!(ie[10] & bit(2)) || !!(ie[10] & bit(3)) || !!(ie[10] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_BFE_STS_LE_80MHZ);

	if (!!(ie[10] & bit(5)) || !!(ie[10] & bit(6)) || !!(ie[10] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_BFE_STS_GT_80MHZ);

	if (!!(ie[11] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_NG16_SU_FEEDBACK);

	if (!!(ie[11] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_NG16_MU_FEEDBACK);

	if (!!(ie[12] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_HE_CODEBOOK_42_SU_FEEDBACK);

	if (!!(ie[12] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_CODEBOOK_75_MU_FEEDBACK);

	if (!!(ie[12] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TRIGGERED_SU_BF_FEEDBACK);

	if (!!(ie[12] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TRIGGERED_MU_BF_PARTIAL_BW_FEEDBACK);

	if (!!(ie[12] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_HE_TRIGGERED_CQI_FEEDBACK);

	if (!!(ie[12] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PARTIAL_BW_ER);

	if (!!(ie[12] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PARTIAL_BW_DL_MUMIMO);

	if (!!(ie[12] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PPE_THRESHOLDS_PRESENT);

	if (!!(ie[13] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_HE_PSR_SR);

	if (!!(ie[13] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_HE_POWER_BOOST_FACTOR);

	if (!!(ie[13] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_HE_STBC_TX_GT_80MHZ);

	if (!!(ie[13] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_HE_STBC_RX_GT_80MHZ);
}

void wifi_cap_set_from_eht(uint8_t *bmp, const uint8_t *ie, size_t ielen)
{
	uint8_t mpdu_len = 0;

	UNUSED(ielen);

	if (!!(ie[0] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_EPCS);

	if (!!(ie[0] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_OM_CONTROL);

	if (!!(ie[0] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_1);

	if (!!(ie[0] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_2);

	if (!!(ie[0] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_RTWT);

	if (!!(ie[0] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_SCS_TDESC);

	mpdu_len = ie[0] & (bit(6) | bit(7));
	if (mpdu_len == 0)
		wifi_cap_set(bmp, WIFI_CAP_EHT_MPDU_3895);
	else if (mpdu_len == 0x40)
		wifi_cap_set(bmp, WIFI_CAP_EHT_MPDU_7991);
	else if (mpdu_len == 0x80)
		wifi_cap_set(bmp, WIFI_CAP_EHT_MPDU_11454);

	if (!!(ie[1] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRS);

	if (!!(ie[1] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TXOP_RETURN_IN_MODE_2);

	if (!!(ie[1] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TWO_BQR);

	if ((ie[1] & (bit(4) | bit(5))) == 0x30 || (ie[1] & (bit(4) | bit(5))) == 0x20)
		wifi_cap_set(bmp, WIFI_CAP_EHT_LINK_ADAPT);

	/* phy caps */
	if (!!(ie[2] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_320_BAND6);

	if (!!(ie[2] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_242RU_IN_BW_GT_20MHZ);

	if (!!(ie[2] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NDP_WITH_4xEHT_LTF_AND_3_2_GI);

	if (!!(ie[2] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_PARTIAL_BW_UL_MUMIMO);

	if (!!(ie[2] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_SU_BFR);

	if (!!(ie[2] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_SU_BFE);

	if (!!(ie[5] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NG16_SU_FEEDBACK);

	if (!!(ie[5] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NG16_MU_FEEDBACK);

	if (!!(ie[5] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_CODEBOOK_42_SU_FEEDBACK);

	if (!!(ie[5] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_CODEBOOK_75_MU_FEEDBACK);

	if (!!(ie[5] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRIGGERED_SU_BF_FEEDBACK);

	if (!!(ie[5] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRIGGERED_MU_BF_PARTIAL_BW_FEEDBACK);

	if (!!(ie[5] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TRIGGERED_CQI_FEEDBACK);

	if (!!(ie[6] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_PARTIAL_BW_DL_MUMIMO);

	if (!!(ie[6] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_PSR_SR);

	if (!!(ie[6] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_POWER_BOOST_FACTOR);

	if (!!(ie[6] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_MU_PPDU_WITH_4xEHT_LTF_AND_0_8_GI);

	if (!!(ie[7] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NON_TRIGGERED_CQI_FEEDBACK);

	if (!!(ie[7] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TX_1K_4K_QAM_WITH_LT_242RU);

	if (!!(ie[7] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_RX_1K_4K_QAM_WITH_LT_242RU);

	if (!!(ie[7] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_PPE_THRESHOLDS_PRESENT);

	//TODO: mcs15-mru

	if (!!(ie[8] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_DUP_IN_BAND6);

	if (!!(ie[9] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_80MHZ);

	if (!!(ie[9] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_160MHZ);

	if (!!(ie[9] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_320MHZ);

	if (!!(ie[9] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_MU_BFR_80MHZ);

	if (!!(ie[9] & bit(5)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_MU_BFR_160MHZ);

	if (!!(ie[9] & bit(6)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_MU_BFR_320MHZ);

	if (!!(ie[9] & bit(7)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_TB_SOUND_FEEDBACK_RATELIMIT);

	if (!!(ie[10] & bit(0)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_RX_1K_QAM_DL_OFDMA_WIDER_BW);

	if (!!(ie[10] & bit(1)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_RX_4K_QAM_DL_OFDMA_WIDER_BW);

	if (!!(ie[10] & bit(2)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_20MHZ_ONLY);

	if (!!(ie[10] & bit(3)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_20MHZ_ONLY_TRIGGERED_MU_BF_FULL_BW_FEEDBACK);

	if (!!(ie[10] & bit(4)))
		wifi_cap_set(bmp, WIFI_CAP_EHT_20MHZ_ONLY_MRU);

}

void wifi_cap_set_from_ie_ext(uint8_t *bitmap, uint8_t *xe, size_t xe_len)
{
	if (!bitmap || !xe)
		return;

	switch (xe[0]) {
	case IE_EXT_HE_CAP:
		/* he caps */
		if (xe_len >= 17)
			wifi_cap_set_from_he(bitmap, xe + 1, xe_len - 1);
		break;
	case IE_EXT_MU_EDCA:
		wifi_cap_set(bitmap, WIFI_CAP_MU_EDCA);
		break;

	case IE_EXT_EHT_CAP:
		if (xe_len >= 11)
			wifi_cap_set_from_eht(bitmap, xe + 1, xe_len - 1);
		break;
	default:
		break;
	}
}

void wifi_cap_set_from_ie_vendor(uint8_t *bitmap, uint8_t *ie, size_t ielen)
{
	/* Microsoft OUI type 2 */
	uint8_t wmm_ie[4] = { 0x00, 0x50, 0xf2, 0x02 };

	if (!bitmap || !ie)
		return;

	if (ielen < sizeof(wmm_ie) + 3)
		return;

	/* Check WMM */
	if (!memcmp(ie, wmm_ie, sizeof(wmm_ie))) {
		wifi_cap_set(bitmap, WIFI_CAP_WMM);

		/* U-APSD */
		if (ie[6] & 0x80)
			wifi_cap_set(bitmap, WIFI_CAP_APSD);
	}
}

void wifi_cap_set_from_ie(uint8_t *bitmap, uint8_t *ie, size_t len)
{
	uint8_t eid;

	if (len < 2)
		return;

	if (len == 2) {
		/* assume capability */
		wifi_cap_set_from_capability_information(bitmap, ie, len);
		return;
	}

	if (ie[1] != len - 2)
		return;   /* bad request */

	eid = ie[0];
	ie += 2;
	len -= 2;

	switch (eid) {
	case IE_HT_CAP:
		/* ht */
		wifi_cap_set_from_ht_capabilities_info(bitmap, ie, len);
		break;
	case IE_VHT_CAP:
		wifi_cap_set_from_vht_capabilities_info(bitmap, ie, len);
		break;
	case IE_EXT_CAP:
		/* ext */
		wifi_cap_set_from_extended_capabilities(bitmap, ie, len);
		break;
	case IE_RRM:
		/* rrm */
		wifi_cap_set_from_rm_enabled_capabilities(bitmap, ie, len);
		break;
	case IE_MDE:
		/* ft-mde */
		wifi_cap_set_from_fast_bss_transition(bitmap, ie, len);
		break;
	case IE_EXT:
		/* ie-ext */
		wifi_cap_set_from_ie_ext(bitmap, ie, len);
		break;
	case IE_VEND_SPEC:
		/* vendor specific */
		wifi_cap_set_from_ie_vendor(bitmap, ie, len);
		break;
	default:
		break;
	}
}

#define EDCF_AIFSN_MASK		0x0f	/* AIFSN mask */
#define EDCF_ACM_MASK		0x10	/* ACM mask */
#define EDCF_ACI_MASK		0x60	/* ACI mask */
#define EDCF_ACI_SHIFT		5	/* ACI shift */

#define EDCF_ECWMIN_MASK	0x0f	/* cwmin exponent form mask */
#define EDCF_ECWMAX_MASK	0xf0	/* cwmax exponent form mask */
#define EDCF_ECWMAX_SHIFT	4	/* cwmax exponent form shift */

struct edcf_ac_param {
	uint8_t aci_aifsn;		/* AIFSN, ACM, ACI */
	uint8_t cw;			/* ECWmin, ECWmax (CW = 2^ECW - 1) */
	uint16_t txop_limit;		/* LE16 */
} __attribute__ ((packed));

void wifi_wmm_set_from_ie(struct wifi_ap_wmm_ac *ac, uint8_t *ie, size_t len)
{
	uint8_t wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };
	struct edcf_ac_param *ie_ac;
	int i;

	if (!ac || !ie)
		return;

	if (len < 26)
		return;

	if (memcmp(&ie[2], wmm_oui, sizeof(wmm_oui)))
		return;

	ie_ac = (struct edcf_ac_param *) &ie[10];
	for (i = 0; i < WIFI_NUM_AC; i++) {
		ac[i].ac = (ie_ac->aci_aifsn & EDCF_ACI_MASK) >> EDCF_ACI_SHIFT;
		ac[i].aifsn = ie_ac->aci_aifsn & EDCF_AIFSN_MASK;
		ac[i].cwmin = ie_ac->cw & EDCF_ECWMIN_MASK;
		ac[i].cwmax = (ie_ac->cw & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
		ac[i].txop = (uint8_t)le16toh(ie_ac->txop_limit);

		ie_ac++;
	}
}

struct mcs_struct {
	uint8_t num_bpscs;
	float coding_rate;
};

static const struct mcs_struct mcstab[] = {
	{ 1, 1.0f / 2.0f },
	{ 2, 1.0f / 2.0f },
	{ 2, 3.0f / 4.0f },
	{ 4, 1.0f / 2.0f },
	{ 4, 3.0f / 4.0f },
	{ 6, 2.0f / 3.0f },
	{ 6, 3.0f / 4.0f },
	{ 6, 5.0f / 6.0f },
	{ 8, 3.0f / 4.0f },
	{ 8, 5.0f / 6.0f },
	{ 10, 3.0f / 4.0f },
	{ 10, 5.0f / 6.0f },
};

static int get_num_sd(uint32_t bw)
{
	int num_sd;
	switch(bw) {
		case 20:
			num_sd = 52;
			break;
		case 40:
			num_sd = 108;
			break;
		case 80:
			num_sd = 234;
			break;
		case 160:
			num_sd = 468;
			break;
		default:
			num_sd = 0;
			break;
	}
	return num_sd;
}

static int get_he_num_sd(uint32_t bw)
{
	int num_sd;

	switch(bw) {
		case 20:
			num_sd = 234;
			break;
		case 40:
			num_sd = 468;
			break;
		case 80:
			num_sd = 980;
			break;
		case 160:
			num_sd = 1960;
			break;
		default:
			num_sd = 0;
			break;
	}
	return num_sd;
}

unsigned long wifi_mcs2rate(uint32_t mcs,
                            uint32_t bw,
                            uint32_t nss,
                            enum wifi_guard gi)
{
	int num_sd;
	float num_cbps;
	float num_dbps;
	float rate = 0.0;
	float duration;

	if (mcs > 11)
		return 0;

	switch(gi) {
		case WIFI_SGI:
			duration = 3.2f + 0.4f;
			num_sd = get_num_sd(bw);
			break;
		case WIFI_LGI:
			duration = 3.2f + 0.8f;
			num_sd = get_num_sd(bw);
			break;
		case WIFI_1xLTF_GI800:
		case WIFI_2xLTF_GI800:
		case WIFI_4xLTF_GI800:
			duration = 12.8f + 0.8f;
			num_sd = get_he_num_sd(bw);
			break;
		case WIFI_1xLTF_GI1600:
		case WIFI_2xLTF_GI1600:
			duration = 12.8f + 1.6f;
			num_sd = get_he_num_sd(bw);
			break;
		case WIFI_4xLTF_GI3200:
			duration = 12.8f + 3.2f;
			num_sd = get_he_num_sd(bw);
			break;
		default:
			duration = 0;
			num_sd = 0;
			break;
	}

	if (num_sd == 0)
			return 0;

	num_cbps = (float)num_sd * mcstab[mcs].num_bpscs * (float)nss;
	num_dbps = num_cbps * mcstab[mcs].coding_rate;
	rate = num_dbps / duration;

    return (unsigned long)rate;
}

uint32_t wifi_bw_enum2MHz(enum wifi_bw bw)
{
	uint32_t bandwidth = 0;
	switch(bw) {
	case BW80:
		bandwidth = 80;
		break;
	case BW40:
		bandwidth = 40;
		break;
	case BW20:
		bandwidth = 20;
		break;
	case BW160:
	case BW8080:
		bandwidth = 160;
		break;
	default:
		break;
	}

	return bandwidth;
}

int wifi_parse_wpa_ie(uint8_t *iebuf, size_t len, struct wifi_rsne *r)
{
	size_t ielen = (size_t)(iebuf[1] & 0xff) + 2;
	unsigned char wpa1_oui[3] = {0x00, 0x50, 0xf2};
	unsigned char wpa2_oui[3] = {0x00, 0x0f, 0xac};
	unsigned char *wpa_oui;
	uint16_t cnt = 0;
	int wpa_version;
	int offset = 2;
	int i;


	wpa_version = (iebuf[0] == 0xdd) ? 1 : 2;

	if (ielen > len)
		ielen = len;

	wpa_oui = wpa_version == 2 ? wpa2_oui : wpa1_oui;

	if (wpa_version == 2 && ielen < 4)
		return -1;

	if (wpa_version == 1) {
		if ((ielen < 8)
			|| (memcmp(&iebuf[2], wpa_oui, 3) != 0)
			|| (iebuf[5] != 0x01)) {
			return -1;
		}
		offset += 4;

		r->wpa_versions |= WPA_VERSION1;
	} else {
		r->wpa_versions |= WPA_VERSION2;
	}

	offset += 2;

	if (ielen < (offset + 4)) {
		/* insufficient ie - assume PSK */
		r->akms |= BIT(WIFI_AKM_PSK);
		r->pair_ciphers |= BIT(WIFI_CIPHER_TKIP);
		r->group_cipher = WIFI_CIPHER_TKIP;

		return 0;
	}
	/* group cipher */
	if (!memcmp(&iebuf[offset], wpa_oui, 3))
		r->group_cipher = iebuf[offset + 3];

	offset += 4;
	if (ielen < (offset + 2)) {
		/* no pairwise cipher or akm present.. assume PSK */
		r->pair_ciphers |= BIT(WIFI_CIPHER_TKIP);
		r->akms |= BIT(WIFI_AKM_PSK);

		return 0;
	}

	/* pairwise ciphers */
	cnt = (uint16_t)(iebuf[offset] | (iebuf[offset + 1] << 8));
	offset += 2;

	if (ielen < (offset + 4 * cnt))
		return -1;

	for (i = 0; i < cnt; i++) {
		if (!memcmp(&iebuf[offset], wpa_oui, 3))
			r->pair_ciphers |= BIT(iebuf[offset+3]);

		offset += 4;
	}

	if (ielen < (offset + 2))
		return -1;

	/* akm suites */
	cnt = (uint16_t)(iebuf[offset] | (iebuf[offset + 1] << 8));
	offset += 2;

	if (ielen < (offset + 4 * cnt))
		return -1;

	for (i = 0; i < cnt; i++) {
		if (!memcmp(&iebuf[offset], wpa_oui, 3))
			r->akms |= BIT(iebuf[offset + 3]);

		offset += 4;
	}

	if (ielen < (offset + 1))
		return 0;

	/* rsnie caps */
	r->rsn_caps = (uint16_t)(iebuf[offset] | (iebuf[offset + 1] << 8));

	return 0;
}

void wifi_rsne_to_security(struct wifi_rsne *r, uint32_t *s)
{
	if (!s)
		return;
	*s = 0;

	if (IS_AKM_PSK(r->akms)) {
		if (!!(r->wpa_versions & WPA_VERSION2)) {
			if (IS_AKM_WPA3_FT_PSK(r->akms) &&
				IS_MFPC(r->rsn_caps) && IS_MFPR(r->rsn_caps)) {

				*s |= BIT(WIFI_SECURITY_FT_WPA3PSK);
			} else if (IS_AKM_WPA3PSK(r->akms) &&
				IS_MFPC(r->rsn_caps) && IS_MFPR(r->rsn_caps)) {

				*s |= BIT(WIFI_SECURITY_WPA3PSK);
			} else if (IS_AKM_WPA3PSK_T(r->akms) &&
				IS_MFPC(r->rsn_caps) && !IS_MFPR(r->rsn_caps)) {

				*s |= BIT(WIFI_SECURITY_WPA3PSK_T);
			} else if (IS_AKM_WPA2_FT_PSK(r->akms)) {
				*s |= BIT(WIFI_SECURITY_FT_WPA2PSK);
			} else {
				*s |= BIT(WIFI_SECURITY_WPA2PSK);
			}
		}

	}

	if (IS_AKM_EAP(r->akms)) {
		if (!!(r->wpa_versions & WPA_VERSION2)) {
			/* FIXME */
			if (IS_MFPC(r->rsn_caps) && IS_MFPR(r->rsn_caps))
				*s |= BIT(WIFI_SECURITY_WPA3);
			else
				*s |= BIT(WIFI_SECURITY_WPA2);
		}
	}

	if (!!(r->wpa_versions & WPA_VERSION1)) {
		*s |= BIT(WIFI_SECURITY_WPAPSK);
	}
}

static int is_wpa_ie(const uint8_t *ie, size_t len)
{
	if (ie[0] == IE_RSN)
		return 1;
	if (ie[0] == IE_VEND_SPEC && ie[1] >= 4 && ie[2] == 0x00 && ie[3] == 0x50 && ie[4] == 0x0f2 && ie[5] == 0x01)
		return 1;

	return 0;
}

int wifi_get_bss_security_from_ies(struct wifi_bss *e, uint8_t *ies, size_t len)
{
	uint8_t *ie;

	memset(&e->rsn, 0, sizeof(e->rsn));

	wifi_foreach_ie(ie, ies, len) {
		if (!is_wpa_ie(ie, len))
			continue;
		/* wpa ie */
		if (wifi_parse_wpa_ie(ie, len, &e->rsn))
				return -1;
		wifi_rsne_to_security(&e->rsn, &e->security);
	}

	/* pass through all ies, no wpa ie found */
	if (e->rsn.wpa_versions == 0 && e->security == 0) {
		e->security |= BIT(WIFI_SECURITY_NONE);
	}

	return 0;
}

static int wifi_is_bg_rate(uint8_t rate, uint8_t* brate, uint8_t* grate)
{
	int i;
	uint8_t b_rates[4] = {0x2, 0x04, 0x0b, 0x16};
	uint8_t g_rates[8] = {0x0c, 0x12, 0x18, 0x24, 0x30, 0x48, 0x60, 0x6c};

	if (!brate || !grate)
		return -EINVAL;

	*brate = 0;
	*grate = 0;

	for (i = 0; i < sizeof(b_rates); i++) {
		if (rate != b_rates[i])
			continue;
		*brate = 1;
		break;
	}
	for (i = 0; i < sizeof(g_rates); i++) {
		if (rate != g_rates[i])
			continue;
		*grate = 1;
		break;
	}
	return 0;
}

int wifi_oper_stds_set_from_ie(uint8_t *ies, size_t ies_len, uint8_t *std)
{
	uint8_t *ht_cap, *vht_cap;
	uint8_t *ds, *supp_rates;
	uint8_t *he_cap;
	uint8_t *eht_cap;
	uint8_t *band6_cap;
	uint8_t channel = 0;
	uint8_t brate = 0;
	uint8_t grate = 0;

	if (!ies || ies_len == 0 || !std)
		return -EINVAL;

	ds = wifi_find_ie(ies, ies_len, IE_DS_PARAM);
	ht_cap = wifi_find_ie(ies, ies_len, IE_HT_CAP);
	vht_cap = wifi_find_ie(ies, ies_len, IE_VHT_CAP);
	supp_rates = wifi_find_ie(ies, ies_len, IE_SUPP_RATES);
	he_cap = wifi_find_ie_ext(ies, ies_len, IE_EXT_HE_CAP);
	eht_cap = wifi_find_ie_ext(ies, ies_len, IE_EXT_EHT_CAP);
	band6_cap = wifi_find_ie_ext(ies, ies_len, IE_EXT_6G_CAPS);

	if (supp_rates) {
		uint8_t b = 0;
		uint8_t g = 0;
		uint8_t *pos, *end;
		pos = supp_rates + 2;
		end = pos + supp_rates[1];
		while(pos < end) {
			wifi_is_bg_rate(*pos & 0x7F, &b, &g);
			pos++;
			brate |= b;
			grate |= g;
		}
	}

	*std = 0;

	if (eht_cap)
		*std |= WIFI_BE;

	if (he_cap)
		*std |= WIFI_AX;

	if (vht_cap)
		*std |= WIFI_AC;

	if (ht_cap)
		*std |= WIFI_N;

	if (brate)
		*std |= WIFI_B;

	if (grate)
		*std |= WIFI_G;

	if (ds && ds[1] == 1 && !band6_cap) {
		channel = ds[2];

		if (channel >= 1 && channel <= 14) {
			/* 2.4GHz */
			*std &= ~(WIFI_A | WIFI_AC);

			if (grate)
				*std |= WIFI_G;
		 } else {
			/* 5GHz */
			*std &= ~(WIFI_G | WIFI_B);

			if (grate)
				*std |= WIFI_A;
		}
	}

	if (band6_cap)
		*std &= ~(WIFI_G | WIFI_A | WIFI_B | WIFI_AC);

	return 0;
}

int wifi_ssid_advertised_set_from_ie(uint8_t *ies, size_t ies_len, bool *ssid_advertised)
{
	uint8_t *ssid_ie;

	if (!ies || ies_len == 0 || !ssid_advertised)
		return -EINVAL;

	*ssid_advertised = true;
	ssid_ie = wifi_find_ie(ies, ies_len, IE_SSID);
	if (ssid_ie && ssid_ie[0] == 0 && ssid_ie[1] == 0) {
		*ssid_advertised = false;
	}
	return 0;
}

int wifi_apload_set_from_ie(uint8_t *ies, size_t ies_len, struct wifi_ap_load *load)
{
	uint8_t *bs;

	if (!ies || ies_len == 0 || !load)
		return -EINVAL;

	bs = wifi_find_ie(ies, ies_len, IE_BSS_LOAD);
	if (bs && bs[1] == 5) {
		load->sta_count = (uint16_t)(bs[2] | (bs[3] << 8));
		load->utilization = bs[4];
		load->available = (uint16_t)(bs[5] | (bs[6] << 8));
	}
	return 0;
}

#define C2S(x) case x: return #x
const char * wifi_band_to_str(enum wifi_band band)
{
        switch (band) {
        C2S(BAND_2);
        C2S(BAND_5);
        C2S(BAND_DUAL);
        C2S(BAND_6);
        C2S(BAND_60);
        C2S(BAND_UNII_1);
        C2S(BAND_UNII_2);
        C2S(BAND_UNII_3);
        C2S(BAND_UNII_4);
        C2S(BAND_UNII_5);
        C2S(BAND_UNII_6);
        C2S(BAND_UNII_7);
        C2S(BAND_UNII_8);
        C2S(BAND_UNKNOWN);
        C2S(BAND_ANY);
	default:
		return "unknown";
	}
}

const char * wifi_bw_to_str(enum wifi_bw bw)
{
	switch (bw) {
	C2S(BW20);
	C2S(BW40);
	C2S(BW80);
	C2S(BW160);
	C2S(BW_AUTO);
	C2S(BW8080);
	C2S(BW320);
	C2S(BW_UNKNOWN);
	default:
		return "unknown";
	}
}
#undef C2S

void correct_oper_std_by_band(enum wifi_band band, uint8_t *std)
{
	switch (band) {
	case BAND_2:
		*std &= ~(WIFI_A | WIFI_AC);
		break;
	case BAND_5:
		if (*std & WIFI_G)
			*std |= WIFI_A;
		*std &= ~(WIFI_G | WIFI_B);
		break;
	case BAND_6:
		*std &= ~(WIFI_B | WIFI_G | WIFI_A | WIFI_AC);
		break;
	default:
		break;
	}
}
