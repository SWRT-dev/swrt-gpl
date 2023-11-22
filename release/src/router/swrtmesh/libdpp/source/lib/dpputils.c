/*
 * dpputils.c - includes DPP utility functions.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <libubox/list.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "debug.h"

#include "dpp_api.h"
#include "dpputils.h"
#include "util.h"


const char *dpp_akm_str(enum dpp_akm akm)
{
	switch (akm) {
	case DPP_AKM_DPP:
		return "dpp";
	case DPP_AKM_PSK:
		return "psk";
	case DPP_AKM_SAE:
		return "sae";
	case DPP_AKM_PSK_SAE:
		return "psk+sae";
	case DPP_AKM_SAE_DPP:
		return "dpp+sae";
	case DPP_AKM_PSK_SAE_DPP:
		return "dpp+psk+sae";
	case DPP_AKM_DOT1X:
		return "dot1x";
	default:
		return "??";
	}
}

const char *dpp_akm_selector_str(enum dpp_akm akm)
{
	switch (akm) {
	case DPP_AKM_DPP:
		return "506F9A02";
	case DPP_AKM_PSK:
		return "000FAC02+000FAC06";
	case DPP_AKM_SAE:
		return "000FAC08";
	case DPP_AKM_PSK_SAE:
		return "000FAC02+000FAC06+000FAC08";
	case DPP_AKM_SAE_DPP:
		return "506F9A02+000FAC08";
	case DPP_AKM_PSK_SAE_DPP:
		return "506F9A02+000FAC08+000FAC02+000FAC06";
	case DPP_AKM_DOT1X:
		return "000FAC01+000FAC05";
	default:
		return "??";
	}
}

const char *dpp_netrole_str(enum dpp_netrole netrole)
{
	switch (netrole) {
	case DPP_NETROLE_STA:
		return "sta";
	case DPP_NETROLE_AP:
		return "ap";
	case DPP_NETROLE_CONFIGURATOR:
		return "configurator";
	case DPP_NETROLE_MAP_BH_BSS:
		return "mapBackhaulBss";
	case DPP_NETROLE_MAP_BH_STA:
		return "mapBackhaulSta";
	case DPP_NETROLE_MAPAGENT:
		return "mapAgent";
	case DPP_NETROLE_MAPCONTROLLER:
		return "mapController";
	default:
		return "??";
	}
}

enum dpp_netrole dpp_netrole_from_str(char *netrole)
{
	enum dpp_netrole role = DPP_NETROLE_INVALID;

	if (!strcasecmp(netrole, "sta"))
		role = DPP_NETROLE_STA;
	else if (!strcasecmp(netrole, "ap"))
		role = DPP_NETROLE_AP;
	else if (!strcasecmp(netrole, "configurator"))
		role = DPP_NETROLE_CONFIGURATOR;
	else if (!strcasecmp(netrole, "mapBackhaulBss"))
		role = DPP_NETROLE_MAP_BH_BSS;
	else if (!strcasecmp(netrole, "mapBackhaulSta"))
		role = DPP_NETROLE_MAP_BH_STA;
	else if (!strcasecmp(netrole, "mapAgent"))
		role = DPP_NETROLE_MAPAGENT;
	else if (!strcasecmp(netrole, "mapController"))
		role = DPP_NETROLE_MAPCONTROLLER;

	return role;
}

void dpp_build_attr_r_bootstrap_key_hash(uint8_t *msg, const uint8_t *hash)
{
        if (hash) {
                pr_debug("DPP: R-Bootstrap Key Hash\n");
                bufptr_put_le16(msg, DPP_ATTR_R_BOOTSTRAP_KEY_HASH);
                bufptr_put_le16(msg, SHA256_MAC_LEN);
                bufptr_put(msg, hash, SHA256_MAC_LEN);
        }
}


void dpp_build_attr_status(uint8_t *msg, enum dpp_status status)
{
	pr_debug("DPP: Status %d\n", status);
	bufptr_put_le16(msg, DPP_ATTR_STATUS);
	bufptr_put_le16(msg, 1);
	*msg = status;
}

int dpp_check_attrs(uint8_t *msg, uint16_t msglen)
{
	uint8_t *p;
	uint8_t *msg_end;


	if (!msg || msglen == 0)
		return -1;

	p = msg;
	msg_end = msg + msglen;

	while (labs(p - msg) < msglen - 4) {
		uint16_t attr_type;
		uint16_t attr_len;

		attr_type = buf_get_le16(p);
		p += 2;
		attr_len = buf_get_le16(p);
		p += 2;

		if (p + attr_len > msg_end) {
			pr_debug("%s: Invalid attr = 0x%04x  len = %hu, p = %p  msg_end = %p diff = %d\n",
				__func__, attr_type, attr_len, p, msg_end, abs(msg_end - p));
			return -1;
		}


		p += attr_len;
	}

	pr_debug("%s: parsed valid dpp frame!\n", __func__);
	return 0;
}

uint8_t *dpp_get_attr(uint8_t *msg, uint16_t msglen, uint16_t attr, uint16_t *olen)
{
	uint8_t *p;
	uint8_t *msg_end;


	if (!msg || msglen == 0)
		return NULL;

	p = msg;
	msg_end = msg + msglen;

	while (labs(p - msg) < msglen - 4) {
		uint16_t attr_type;
		uint16_t attr_len;

		attr_type = buf_get_le16(p);
		p += 2;
		attr_len = buf_get_le16(p);
		p += 2;

		if (p + attr_len > msg_end) {
			pr_debug("%s: Invalid attr = 0x%04x  len = %hu, p = %p  msg_end = %p diff = %d msg_len:%d\n",
				__func__, attr_type, attr_len, p, msg_end, abs(msg_end - p), msglen);
			return NULL;
		}

		if (attr_type == attr) {
			pr_debug("MATCHED attr = 0x%04x\n", attr);
			if (olen)
				*olen = attr_len;

			return p;
		}

		p += attr_len;
	}

	pr_debug("%s: VALID msg, but attr 0x%04x not found!\n", __func__, attr);
	return NULL;
}

uint8_t *dpp_get_attr_next(uint8_t *prev, uint8_t *buf, size_t len,
			   uint16_t req_id, uint16_t *ret_len)
{
	uint16_t id, alen;
	uint8_t *pos, *end;

	end = (buf + len);

	if (!prev)
		pos = buf;
	else {
		uint8_t *pprev = prev - 2;
		pos = prev + buf_get_le16(pprev);
	}

	while (abs(end - pos) >= 4) {
		id = buf_get_le16(pos);
		pos += 2;
		alen = buf_get_le16(pos);
		pos += 2;
		if (alen > end - pos) {
			return NULL;
		}
		if (id == req_id) {
			*ret_len = alen;
			return pos;
		}
		pos += alen;
	}

	return NULL;
}

enum dpp_akm dpp_akm_from_str(const char *akm)
{
	const char *pos;
	int dpp = 0, psk = 0, sae = 0, dot1x = 0;

	if (strcmp(akm, "psk") == 0)
		return DPP_AKM_PSK;
	if (strcmp(akm, "sae") == 0)
		return DPP_AKM_SAE;
	if (strcmp(akm, "psk+sae") == 0)
		return DPP_AKM_PSK_SAE;
	if (strcmp(akm, "dpp") == 0)
		return DPP_AKM_DPP;
	if (strcmp(akm, "dpp+sae") == 0)
		return DPP_AKM_SAE_DPP;
	if (strcmp(akm, "dpp+psk+sae") == 0)
		return DPP_AKM_PSK_SAE_DPP;
	if (strcmp(akm, "dot1x") == 0)
		return DPP_AKM_DOT1X;

	pos = akm;
	while (*pos) {
		if (strlen(pos) < 8)
			break;
		if (strncasecmp(pos, "506F9A02", 8) == 0)
			dpp = 1;
		else if (strncasecmp(pos, "000FAC02", 8) == 0)
			psk = 1;
		else if (strncasecmp(pos, "000FAC06", 8) == 0)
			psk = 1;
		else if (strncasecmp(pos, "000FAC08", 8) == 0)
			sae = 1;
		else if (strncasecmp(pos, "000FAC01", 8) == 0)
			dot1x = 1;
		else if (strncasecmp(pos, "000FAC05", 8) == 0)
			dot1x = 1;
		pos += 8;
		if (*pos != '+')
			break;
		pos++;
	}

	if (dpp && psk && sae)
		return DPP_AKM_PSK_SAE_DPP;
	if (dpp && sae)
		return DPP_AKM_SAE_DPP;
	if (dpp)
		return DPP_AKM_DPP;
	if (psk && sae)
		return DPP_AKM_PSK_SAE;
	if (sae)
		return DPP_AKM_SAE;
	if (psk)
		return DPP_AKM_PSK;
	if (dot1x)
		return DPP_AKM_DOT1X;

	return DPP_AKM_UNKNOWN;
}

int dpp_akm_psk(enum dpp_akm akm)
{
	return akm == DPP_AKM_PSK || akm == DPP_AKM_PSK_SAE ||
		akm == DPP_AKM_PSK_SAE_DPP;
}


int dpp_akm_sae(enum dpp_akm akm)
{
	return akm == DPP_AKM_SAE || akm == DPP_AKM_PSK_SAE ||
		akm == DPP_AKM_SAE_DPP || akm == DPP_AKM_PSK_SAE_DPP;
}


int dpp_akm_legacy(enum dpp_akm akm)
{
	return akm == DPP_AKM_PSK || akm == DPP_AKM_PSK_SAE ||
		akm == DPP_AKM_SAE;
}


int dpp_akm_dpp(enum dpp_akm akm)
{
	return akm == DPP_AKM_DPP || akm == DPP_AKM_SAE_DPP ||
		akm == DPP_AKM_PSK_SAE_DPP;
}

int dpp_akm_ver2(enum dpp_akm akm)
{
	return akm == DPP_AKM_SAE_DPP || akm == DPP_AKM_PSK_SAE_DPP;
}


#if 0
bool dpp_supports_curve(const char *curve, struct dpp_bootstrap_info *bi)
{
	enum dpp_bootstrap_supported_curves idx;

	if (!bi || !bi->supported_curves)
		return true; /* no support indication available */

	if (strcmp(curve, "prime256v1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_P_256;
	else if (strcmp(curve, "secp384r1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_P_384;
	else if (strcmp(curve, "secp521r1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_P_521;
	else if (strcmp(curve, "brainpoolP256r1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_BP_256;
	else if (strcmp(curve, "brainpoolP384r1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_BP_384;
	else if (strcmp(curve, "brainpoolP512r1") == 0)
		idx = DPP_BOOTSTRAP_CURVE_BP_512;
	else
		return true;

	return bi->supported_curves & BIT(idx);
}
#endif

int dpp_uri_valid_info(const char *info)
{
	while (*info) {
		unsigned char val = *info++;

		if (val < 0x20 || val > 0x7e || val == 0x3b)
			return 0;
	}

	return 1;
}

int dpp_parse_chan_list(char *chanlist)
{
	char *pos, *pos2;
	int opclass = -1, channel, freq;

	pr_debug("%s: chanlist:%s\n", __func__, chanlist);

	pos = chanlist;

	while (pos && *pos) {
		pos2 = pos;
		while (*pos2 >= '0' && *pos2 <= '9')
			pos2++;
		if (*pos2 == '/') {
			opclass = atoi(pos);
			pos = pos2 + 1;
		}
		if (opclass <= 0)
			goto fail;
		channel = atoi(pos);
		if (channel <= 0)
			goto fail;
		while (*pos >= '0' && *pos <= '9')
			pos++;

		freq = ieee80211_chan_to_freq(opclass, channel);
		pr_debug("DPP: channel-list: opclass=%d channel=%d ==> freq=%d\n",
			   opclass, channel, freq);

		if (*pos == ';' || *pos == '\0')
			break;
		if (*pos != ',')
			break;
		pos++;
	}

fail:
	return 0;
}

int dpp_chan_list_to_opclass(char *chanlist)
{
	char *pos, *pos2;

	pr_debug("%s: chanlist:%s\n", __func__, chanlist);

	pos = chanlist;

	if (pos && *pos) {
		pos2 = pos;
		while (*pos2 >= '0' && *pos2 <= '9')
			pos2++;
		if (*pos2 == '/')
			return atoi(pos); /* return first opclass */
	}

	return -1;
}

enum wifi_band dpp_chan_list_to_band(char *chanlist)
{
	int opclass;

	opclass = dpp_chan_list_to_opclass(chanlist);
	if (opclass >= 80 && opclass < 90)
		return BAND_2;
	if (opclass >= 110 && opclass < 130)
		return BAND_5;
	if (opclass >= 130 && opclass < 140)
		return BAND_6;

	return BAND_UNKNOWN;
}

uint8_t dpp_get_pub_af_type(const uint8_t *frame, uint16_t framelen)
{
	if (framelen < 7)
		return 255;

	return *(frame + 6);
}

uint8_t dpp_get_gas_frame_type(const uint8_t *frame, uint16_t framelen)
{
	if (framelen < 1)
		return 255;

	return *frame;
}

uint8_t dpp_get_frame_type(const uint8_t *frame, uint16_t framelen)
{
	if (framelen < 7)
		return 255;

	if (FRAME_IS_DPP_PUB_AF(frame))
		return dpp_get_pub_af_type(frame, framelen);
	else if (FRAME_IS_DPP_GAS_FRAME(frame))
		return dpp_get_gas_frame_type(frame, framelen);

	return 255;
}
