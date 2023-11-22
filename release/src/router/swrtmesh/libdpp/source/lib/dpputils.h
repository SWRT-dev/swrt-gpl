/*
 * dpputils.h - defines DPP utility functions and macros.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#ifndef DPPUTIL_H
#define DPPUTIL_H

/* TODO: use defines from dpp_api.h once moved */
#define DPP_PUB_AF_ACTION		0x09
#define DPP_PUB_AF_ACTION_OUI_TYPE	0x1A
#define DPP_PUB_AF_GAS_INITIAL_REQ	0x0A
#define DPP_PUB_AF_GAS_INITIAL_RESP	0x0B

#define FRAME_IS_DPP_PUB_AF(frame) \
		((*(frame) == DPP_PUB_AF_ACTION) && \
		(memcmp((frame + 1), "\x50\x6F\x9A", 3) == 0) && \
		((*(frame + 4)) == DPP_PUB_AF_ACTION_OUI_TYPE))

#define FRAME_IS_DPP_GAS_FRAME(frame) \
		((*(frame) == DPP_PUB_AF_GAS_INITIAL_REQ) || \
		(*(frame) == DPP_PUB_AF_GAS_INITIAL_RESP))

const char *dpp_akm_str(enum dpp_akm akm);
const char *dpp_akm_selector_str(enum dpp_akm akm);
const char *dpp_netrole_str(enum dpp_netrole netrole);
enum dpp_netrole dpp_netrole_from_str(char *netrole);

void dpp_build_attr_r_bootstrap_key_hash(uint8_t *msg, const uint8_t *hash);
void dpp_build_attr_status(uint8_t *msg, enum dpp_status status);
int dpp_check_attrs(uint8_t *msg, uint16_t msglen);
uint8_t *dpp_get_attr(uint8_t *msg, uint16_t msglen, uint16_t attr, uint16_t *olen);
uint8_t * dpp_get_attr_next(uint8_t *prev, uint8_t *buf, size_t len,
				    uint16_t req_id, uint16_t *ret_len);

enum dpp_akm dpp_akm_from_str(const char *akm);
int dpp_akm_psk(enum dpp_akm akm);
int dpp_akm_sae(enum dpp_akm akm);
int dpp_akm_legacy(enum dpp_akm akm);
int dpp_akm_dpp(enum dpp_akm akm);
int dpp_akm_ver2(enum dpp_akm akm);
#if 0
bool dpp_supports_curve(const char *curve, struct dpp_bootstrap_info *bi);
#endif
int dpp_uri_valid_info(const char *info);
int dpp_parse_chan_list(char *chanlist);
int dpp_chan_list_to_opclass(char *chanlist);
enum wifi_band dpp_chan_list_to_band(char *chanlist);

uint8_t dpp_get_pub_af_type(const uint8_t *frame, uint16_t framelen);
uint8_t dpp_get_gas_frame_type(const uint8_t *frame, uint16_t framelen);
uint8_t dpp_get_frame_type(const uint8_t *frame, uint16_t framelen);

#endif