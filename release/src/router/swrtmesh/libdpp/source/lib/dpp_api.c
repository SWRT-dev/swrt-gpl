/*
 * dpp_api.c - implements DPP frame generation and processing APIs.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/dh.h>
#include <openssl/hmac.h>
#include <openssl/cmac.h>
#include <openssl/x509.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#if defined(RTCONFIG_SWRTMESH)
#include <openssl/aes.h>
#endif

#include <libubox/list.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "debug.h"
#include "util.h"

#include "utf8.h"
#include "json_helper.h"
#include "dpp_api.h"
#include "dpputils.h"	//TODO: deprecate
#include "dpp_sm.h"

int dpp_configuration_valid(const struct dpp_configuration *conf)
{
	if (conf->ssid_len == 0)
		return 0;
	if (dpp_akm_psk(conf->akm) && !conf->passphrase)
		return 0;
	if (dpp_akm_sae(conf->akm) && !conf->passphrase)
		return 0;
	return 1;
}

struct dpp_configurator *dpp_get_configurator(struct dpp_context *ctx, int id)
{
	struct dpp_configurator *conf = NULL;

	list_for_each_entry(conf, &ctx->configuratorlist, list) {
		if (conf->id == id)
			return conf;
	}

	return NULL;
}

int dpp_next_configurator_id(struct dpp_context *ctx)
{
	struct dpp_configurator *conf = NULL;
	int max_id = 0;

	list_for_each_entry(conf, &ctx->configuratorlist, list) {
		if (conf->id > max_id)
			max_id = conf->id;
	}
	return max_id + 1;
}

static struct dpp_configuration *dpp_config_get(struct dpp_context *ctx,
			     enum dpp_netrole netrole, enum wifi_band band)
{
	struct dpp_configuration *conf = NULL;

	list_for_each_entry(conf, &ctx->conflist, list) {
		if (netrole == conf->netrole && band == conf->band)
			return conf;
	}

	return NULL;
}

static int dpp_config_parser(struct dpp_context *ctx,
			     enum dpp_netrole netrole, enum dpp_akm akm,
			     char *ssid, char *pass, enum wifi_band band)
{
	struct dpp_configuration *conf = NULL;

	conf = calloc(1, sizeof(*conf));
	if (!conf)
		return -1;

	conf->netrole = netrole;
	conf->akm = akm;
	if (ssid) {
		conf->ssid = str_to_utf8(ssid, &conf->ssid_len);
		if (conf->ssid_len > 32 /* MAX_SSID_LEN */ || conf->ssid_len == 0)
			goto fail;
	} else
		goto fail;

	if (pass) {
		conf->passphrase = str_to_utf8(pass, &conf->pass_len);
		if (conf->pass_len > 64 /* MAX_pass_len */ || conf->pass_len == 0)
			goto fail;
	}

	if (!dpp_configuration_valid(conf))
		goto fail;

	conf->band = band;

	list_add(&conf->list, &ctx->conflist);
	return 0;
fail:
	free(conf);
	return -1;
}

/* clear all stored configuration */
int dpp_del_configuration(struct dpp_context *ctx)
{
	struct dpp_configuration *conf = NULL, *tmp = NULL;

	list_for_each_entry_safe(conf, tmp, &ctx->conflist, list) {
		if (conf->ssid)
			free(conf->ssid);
		if (conf->passphrase)
			free(conf->passphrase);
		list_del(&conf->list);
		free(conf);
	}
	return 0;
}

int dpp_set_configuration(struct dpp_context *ctx, enum dpp_netrole netrole,
			  enum dpp_akm akm, char *ssid, char *pass,
			  enum wifi_band band)
{
	int ret = -1;

	pr_debug("DPP: Set configurator parameters:\n" \
		"\tnetrole:%s\n" \
		"\takm:%s\n" \
		"\tssid:%s\n" \
		"\tpass:%s\n", dpp_netrole_str(netrole), dpp_akm_str(akm),
		ssid, pass);

	ret = dpp_config_parser(ctx, netrole, akm, ssid, pass, band);
	return ret;
}




int dpp_build_jwk(uint8_t *buf, const char *name, void *key, const char *kid)
{
	uint8_t *pub;
	size_t pub_len;
	const uint8_t *pos;
	int ret = -1;

	ret = ecc_key_get_pubkey_point(key, 0, &pub, &pub_len);
	if (ret)
		goto fail;

	json_start_object(buf, name);
	json_add_string(buf, "kty", "EC");
	json_value_sep(buf);
	json_add_string(buf, "crv", "P-256" /* curve->jwk_crv */);
	json_value_sep(buf);
	pos = pub;
	if (json_add_base64url(buf, "x", pos, 32 /* curve->prime_len */) < 0)
		goto fail;

	json_value_sep(buf);
	pos += 32 /* curve->prime_len */;
	if (json_add_base64url(buf, "y", pos, 32 /* curve->prime_len */) < 0)
		goto fail;

	if (kid) {
		json_value_sep(buf);
		json_add_string(buf, "kid", kid);
	}
	json_end_object(buf);
	ret = 0;
fail:
	free(pub);
	return ret;
}

static char *dpp_build_jws_prot_hdr(struct dpp_configurator *conf, size_t *signed1_len)
{
	uint8_t *jws_prot_hdr;
	char *signed1;

	jws_prot_hdr = calloc(1, 128 * sizeof(uint8_t));
	if (!jws_prot_hdr)
		return NULL;

	pr_debug("kid:%s\n", conf->kid);

	json_start_object(jws_prot_hdr, NULL);
	json_add_string(jws_prot_hdr, "typ", "dppCon");
	json_value_sep(jws_prot_hdr);
	json_add_string(jws_prot_hdr, "kid", conf->kid);
	json_value_sep(jws_prot_hdr);
	json_add_string(jws_prot_hdr, "alg", "ES256" /* conf->curve->jws_alg */);
	json_end_object(jws_prot_hdr);

	signed1 = calloc(1, ((strlen((char *)jws_prot_hdr) +1) / 3) * 4 + 64);
	if (!signed1)
		return NULL;

	*signed1_len = base64url_encode((unsigned char *)signed1, (unsigned char *)jws_prot_hdr, strlen((char *)jws_prot_hdr));
	free(jws_prot_hdr);

	return signed1;
}

static char *dpp_build_conn_signature(struct dpp_configurator *conf,
				      const char *signed1, size_t signed1_len,
				      const char *signed2, size_t signed2_len,
				      size_t *signed3_len)
{
	uint8_t *r = NULL, *s = NULL, *rs = NULL;
	size_t hash_len = SHA256_MAC_LEN;
	size_t rlen = 0, slen = 0;
	uint8_t *sig = NULL;
	size_t sig_len = 0;
	char *signed3 = NULL;
	char *dot = ".";
	const uint8_t *addr[3];
	size_t addrlen[3];
	uint8_t *hash;
	int ret = -1;


	addr[0] = (const uint8_t *) signed1;
	addr[1] = (const uint8_t *) dot;
	addr[2] = (const uint8_t *) signed2;
	addrlen[0] = signed1_len;
	addrlen[1] = 1;
	addrlen[2] = signed2_len;

	hash = calloc(1, hash_len);
	if (!hash)
		goto fail;

	if (hash_len == SHA256_MAC_LEN) {
		ret = PLATFORM_SHA256(3, addr, addrlen, hash);
	} else {
		pr_debug("DPP: Unknown signature algorithm\n");
		goto fail;
	}

	if (ret) {
		pr_debug("DPP: Hash computation failed\n");
		goto fail;
	}

	bufprintf("DPP: Hash value for Connector signature", hash, hash_len);

	sig = ecdsa_sign(conf->csign_key, hash, hash_len, &sig_len);
	if (!sig) {
		pr_debug("DPP: Signature computation failed\n");
		goto fail;
	}

	ret = ecdsa_signature_extract_r_s(sig, sig_len, &r, &rlen, &s, &slen);
	if (ret) {
		pr_debug("DPP: Extract r,s from sig failed\n");
		goto fail;
	}

	rs = calloc(1, rlen + slen);
	if (!rs) {
		pr_debug("DPP: alloc(rs) failed\n");
		goto fail;
	}

	memcpy(rs, r, rlen);
	memcpy(rs + rlen, s, slen);

	bufprintf("DPP: signedConnector ECDSA signature (raw r,s)\n", rs, rlen + slen);
	signed3 = calloc(1, ((rlen + slen + 1) / 3) * 4 + 64);
	if (!signed3)
		goto fail;

	*signed3_len = base64url_encode((unsigned char *)signed3, (unsigned char *)rs, rlen + slen);

fail:
	free(hash);
	free(sig);
	free(rs);

	return signed3;
}

char *dpp_sign_connector(struct dpp_configurator *conf, const uint8_t *dppcon, size_t dppcon_len)
{
	char *signed1 = NULL, *signed2 = NULL, *signed3 = NULL;
	char *signed_conn = NULL, *pos;
	size_t signed1_len, signed2_len, signed3_len;

	signed1 = dpp_build_jws_prot_hdr(conf, &signed1_len);
	signed2 = calloc(1, ((dppcon_len +1) / 3) * 4 + 64);
	if (!signed2)
		return NULL;

	signed2_len = (size_t) base64url_encode((unsigned char *)signed2, (unsigned char *)dppcon, dppcon_len);
	if (!signed1 || !signed2)
		goto fail;

	signed3 = dpp_build_conn_signature(conf, signed1, signed1_len,
					   signed2, signed2_len, &signed3_len);
	if (!signed3)
		goto fail;

	signed_conn = calloc(1, signed1_len + signed2_len + signed3_len + 3);
	if (!signed_conn)
		goto fail;

	pos = signed_conn;
	memcpy(pos, signed1, signed1_len);
	pos += signed1_len;
	*pos++ = '.';
	memcpy(pos, signed2, signed2_len);
	pos += signed2_len;
	*pos++ = '.';
	memcpy(pos, signed3, signed3_len);
	pos += signed3_len;
	*pos = '\0';

fail:
	free(signed1);
	free(signed2);
	free(signed3);

	return signed_conn;
}

uint8_t *dpp_build_conf_start_map(struct dpp_peer *peer,
		     struct dpp_configuration *conf, size_t tailroom)
{
	uint8_t *buf;

	buf = calloc(1, 200 + tailroom);
	if (!buf)
		return NULL;

	json_start_object(buf, NULL);
	json_add_string(buf, "wi-fi_tech", "map");
	json_value_sep(buf);

	if (conf->netrole == DPP_NETROLE_MAPAGENT) {
		json_add_int(buf, "dfCounterThreshold", 15);
		json_value_sep(buf);
	}

	if ((conf->netrole == DPP_NETROLE_MAP_BH_STA) ||
		(conf->netrole == DPP_NETROLE_MAP_BH_BSS)) {
		json_start_object(buf, "discovery");
		json_add_string_escape(buf, "ssid", conf->ssid, conf->ssid_len);
		json_value_sep(buf);
		json_add_string(buf, "BSSID", "000000000000");
		json_end_object(buf);
		json_value_sep(buf);
	}

	return buf;
}


static uint8_t *dpp_build_conf_start(struct dpp_peer *peer,
				     struct dpp_configuration *conf,
				     size_t buf_len)
{
	uint8_t *buf;

	buf = calloc(1, 200 + buf_len * sizeof(uint8_t));
	if (!buf)
		return NULL;

	json_start_object(buf, NULL);
	json_add_string(buf, "wi-fi_tech", "infra");
	json_value_sep(buf);

	json_start_object(buf, "discovery");

	if ((/*( !conf->ssid_charset ||*/ peer->peer_version < 2) &&
	     json_add_string_escape(buf, "ssid", conf->ssid, conf->ssid_len) < 0) // ||
	    /* ((conf->ssid_charset && peer->peer_version >= 2) &&
	     json_add_base64url(buf, "ssid64", conf->ssid, conf->ssid_len) < 0)) */{
		free(buf);
		return NULL;
	}

//	if (conf->ssid_charset > 0) {
//		json_value_sep(buf);
//		json_add_int(buf, "ssid_charset", conf->ssid_charset);
//	}
	json_end_object(buf);
	json_value_sep(buf);

	return buf;
}

static void dpp_build_legacy_cred_params(uint8_t *buf, struct dpp_configuration *conf)
{
	if (conf->passphrase && strlen(conf->passphrase) < 64) {
		json_add_string_escape(buf, "pass", conf->passphrase, strlen(conf->passphrase));
	} /* else if (conf->psk_set) {
		char pskstr[2 * sizeof(conf->psk) + 1] = {0};

		btostr(conf->psk, sizeof(conf->psk), pskstr);
		json_add_string(buf, "psk_hex", pskstr);
		memset(pskstr, 0, sizeof(pskstr));
	} */
}

static uint8_t *dpp_build_conf_obj_dpp(struct dpp_context *ctx,
				       struct dpp_peer *peer,
				       struct dpp_configuration *conf,
				       size_t *outlen)
{
	uint8_t *buf = NULL;
	size_t buf_len = 0;
	char *signed_conn = NULL;
	size_t tailroom;
	uint8_t *dppcon = NULL;
	size_t dppcon_len = 0;
	size_t extra_len = 1000;
	int incl_legacy;
	enum dpp_akm akm;
	const char *akm_str;
	struct dpp_configurator *c;

	c = dpp_get_configurator(ctx, 1); /* TODO: what ID? */
	if (!c) {
		pr_debug("DPP: No configurator specified"
			" - cannot generate DPP config object\n");
		goto fail;
	}

	akm = conf->akm;

	/* Connector (JSON dppCon object) */
	dppcon_len = extra_len + 2 * 32 * 4 / 3;
	dppcon = calloc(1, dppcon_len);
	if (!dppcon)
		goto fail;

	json_start_object(dppcon, NULL);
	json_start_array(dppcon, "groups");
	json_start_object(dppcon, NULL);
	json_add_string(dppcon, "groupId", "*");
	json_value_sep(dppcon);
	json_add_string(dppcon, "netRole", dpp_netrole_str(conf->netrole));
	json_end_object(dppcon);
	json_end_array(dppcon);
	json_value_sep(dppcon);

	if (!peer->peer_protocol_key) {
		pr_debug("DPP: No peer protocol key available to build netAccessKey JWK\n");
		goto fail;
	}

	if (dpp_build_jwk(dppcon, "netAccessKey", peer->peer_protocol_key, NULL) < 0) {
		pr_debug("DPP: Failed to build netAccessKey JWK\n");
		goto fail;
	}

#if 0	//TODO
	if (conf->netaccesskey_expiry) {
		time_t tm;
		char expiry[30];

		if (gmtime(conf->netaccesskey_expiry, &tm) < 0) {
			pr_debug("DPP: Failed to generate expiry string\n");
			goto fail;
		}

		snprintf(expiry, sizeof(expiry),
			    "%04u-%02u-%02uT%02u:%02u:%02uZ",
			    tm.year, tm.month, tm.day,
			    tm.hour, tm.min, tm.sec);
		json_value_sep(dppcon);
		json_add_string(dppcon, "expiry", expiry);
	}
#endif
	json_end_object(dppcon);
	pr_debug("DPP: dppCon: %s\n", (const char *)dppcon);

	signed_conn = dpp_sign_connector(c, dppcon, strlen((char *)dppcon));
	if (!signed_conn)
		goto fail;

	incl_legacy = dpp_akm_psk(akm) || dpp_akm_sae(akm);
	tailroom = 1000;
	tailroom += 2 * /* curve->prime_len */ 32 * 4 / 3 + strlen(c->kid);

	tailroom += strlen(signed_conn);
	if (incl_legacy)
		tailroom += 1000;

//	if (conf->extra_name && conf->extra_value)
//		tailroom += 10 + strlen(conf->extra_name) + strlen(conf->extra_value);

	buf_len = 128 + tailroom;


	if (conf->netrole == DPP_NETROLE_MAP_BH_STA ||
		conf->netrole == DPP_NETROLE_MAP_BH_BSS ||
		conf->netrole == DPP_NETROLE_MAPAGENT ||
		conf->netrole == DPP_NETROLE_MAPCONTROLLER)
		buf = dpp_build_conf_start_map(peer, conf, tailroom);
	else
		buf = dpp_build_conf_start(peer, conf, tailroom);
	if (!buf)
		goto fail;


	if (peer->akm_use_selector && dpp_akm_ver2(akm))
		akm_str = dpp_akm_selector_str(akm);
	else
		akm_str = dpp_akm_str(akm);

	json_start_object(buf, "cred");
	json_add_string(buf, "akm", akm_str);
	json_value_sep(buf);
	if (incl_legacy) {
		dpp_build_legacy_cred_params(buf, conf);
		json_value_sep(buf);
	}

	snprintf((char *)(buf + strlen((char *)buf)), buf_len, "%s", "\"signedConnector\":\"");
	snprintf((char *)(buf + strlen((char *)buf)), buf_len, "%s", signed_conn);
	snprintf((char *)(buf + strlen((char *)buf)), buf_len, "%s", "\"");
	json_value_sep(buf);
	if (dpp_build_jwk(buf, "csign", c->csign_key, c->kid) < 0) {
		pr_debug("DPP: Failed to build csign JWK\n");
		goto fail;
	}

	if (peer->peer_version >= 2 && c->pp_key) {
		json_value_sep(buf);
		if (dpp_build_jwk(buf, "ppKey", c->pp_key, NULL) < 0) {
			pr_debug("DPP: Failed to build ppKey JWK\n");
			goto fail;
		}
	}

	json_end_object(buf);
//	if (conf->extra_name && conf->extra_value) {
//		json_value_sep(buf);
//		sprintf((char *)(buf + strlen((char *)buf)), "\"%s\":%s", conf->extra_name, conf->extra_value);
//	}
	json_end_object(buf);

	bufprintf("DPP: Configuration Object", buf, strlen((char *)buf));
	*outlen = strlen((char *)buf) + 1;

out:
	free(signed_conn);
	free(dppcon);
	return buf;
fail:
	pr_debug("DPP: Failed to build configuration object\n");
	free(buf);
	buf = NULL;
	goto out;
}

static uint8_t *dpp_build_conf_obj(struct dpp_context *ctx,
				   struct dpp_peer *peer,
				   enum dpp_netrole netrole,
				   enum dpp_config_obj_type type,
				   enum wifi_band band,
				   size_t *outlen)
{
	struct dpp_configuration *conf = NULL;
	struct dpp_configuration conf_1905 = {
		.netrole = netrole,
		.akm = DPP_AKM_DPP
	};

	if (type == DPP_CONFIG_TYPE_1905 && netrole == DPP_NETROLE_MAPAGENT)
		conf = &conf_1905;
	else
		conf = dpp_config_get(ctx, netrole, band);

	if (!conf) {
		pr_debug("DPP: No configuration available for Enrollee(%s) - reject configuration request\n",
				dpp_netrole_str(netrole));
		return NULL;
	}

	return dpp_build_conf_obj_dpp(ctx, peer, conf, outlen);
}

static int dpp_conf_req_rx_map(struct dpp_context *ctx, struct dpp_peer *peer,
			       struct json_token *root)
{
	struct json_token  *token, *list;

	token = json_get_member(root, "netRole");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No Config Attributes - netRole");
		return -1;
	}
	pr_debug("DPP: netRole = '%s'\n", token->string);
	peer->e_req.netrole = dpp_netrole_from_str(token->string);
	if (peer->e_req.netrole == DPP_NETROLE_INVALID) {
		pr_debug("DPP: Unsupported netRole '%s'\n", token->string);
		return -1;
	}

	list = json_get_member(root, "bSTAList");
	if (list && list->type == JSON_ARRAY) {
		int array_size = 0;
		for (token = list->child; token; token = token->sibling) {
			struct json_token *akm, *role, *chan_list;

			role = json_get_member(token, "netRole");
			if (!role || role->type != JSON_STRING) {
				pr_debug("DPP: Missing netRole string\n");
				return -1;
			}

			pr_debug("DPP: L2 netRole = '%s'\n", role->string);
			if (strcmp(role->string, "mapBackhaulSta") != 0) {
				pr_debug("DPP: Unsupported netRole '%s'\n",
					role->string);
				return -1;
			}

			peer->e_req.blist[array_size].l2_netrole = dpp_netrole_from_str(role->string);
			if (peer->e_req.blist[array_size].l2_netrole == DPP_NETROLE_INVALID) {
				pr_debug("DPP: Unsupported netRole '%s'\n",
					role->string);
				return -1;
			}
			akm = json_get_member(token, "akm");
			if (!akm || akm->type != JSON_STRING) {
				pr_debug("DPP: Missing akm string\n");
				return -1;
			}

			peer->e_req.blist[array_size].akm = dpp_akm_from_str(akm->string);
			if (peer->e_req.blist[array_size].akm == DPP_AKM_UNKNOWN) {
				pr_debug("DPP: Invalid AKM:%s\n", akm->string);
				return -1;
			}

			//TODO: check if any of the akm is supported before proceeding
			//eunm dpp_akm akm_val = dpp_akm_from_str(akm->string);
			//pr_debug("DPP: akm supported = '%s'configurator akm '%s\n",
			//		akm->string, dpp_akm_str(conf->akm));
			//TODO: handle channel list

			chan_list = json_get_member(token, "channelList");
			if (!chan_list || chan_list->type != JSON_STRING) {
				pr_debug("DPP: Missing channelList string\n");
				return -1;
			}

			peer->e_req.blist[array_size].chan_list = strdup(chan_list->string);
			dpp_parse_chan_list(peer->e_req.blist[array_size].chan_list);

			array_size++;

			peer->e_req.num_bsta++;
			if (peer->e_req.num_bsta >= sizeof(peer->e_req.blist))
				break;
		}
		pr_debug("DPP: BSTAList count = %d\n", array_size);
	}
	return 0;
}

int dpp_attr_ok(struct dpp_attribute *a, int rem)
{
	uint16_t l;

	if (rem < sizeof(struct dpp_attribute))
		return 0;

	l = buf_get_le16(&((uint8_t *)a)[2]);
	if (l + 4 > rem)
		return 0;

	return 1;
}

struct dpp_attribute *dpp_attr_next(struct dpp_attribute *a, int *rem)
{
	uint16_t l = buf_get_le16(&((uint8_t *)a)[2]);

	*rem -= (l + 4);
	return (struct dpp_attribute *)((uint8_t *)a + l + 4);
}


enum dpp_role {
	DPP_ENROLLEE = 0x1,
	DPP_CONFIGURATOR = 0x2,
};

struct dpp_peer *dpp_lookup_peer(struct dpp_context *ctx, uint8_t *macaddr);

/* read eckey from file */
int dpp_read_key_from_file(const char *keyfile, void **eckey)
{
	return 0;
}

int dpp_load_bootstrap_uri(void *context, const char *file)
{
	struct dpp_context *ctx = (struct dpp_context *)context;
	char uri[512] = {0};
	int ret = 0;
	FILE *f;

	f = fopen(file, "r");
	if (!f)
		return -1;

	while (fgets(uri, sizeof(uri), f)) {
		struct dpp_bootstrap_info *b = NULL;
		char *lf = strstr(uri, "\n");

		if (lf)
			lf[0] = '\0';

		if (!strlen(uri))
			continue;

		b = calloc(1, sizeof(*b));
		if (!b) {
			ret = -1;
			continue;
		}

		ret = dpp_build_bootstrap_info_from_uri(uri, b);
		if (!ret) {
			ret = dpp_bootstrap_add(ctx, b);
			dpp_print_bootstrap_info(b);
		} else {
			dpp_bootstrap_free(b);
		}
	}

	fclose(f);
	return ret;
}

int dpp_store_bootstrap_keys(const char *file, struct dpp_bootstrap_info *bs)
{
	return 0;
}

static uint8_t *dpp_alloc_frame(enum dpp_public_action_frame_type type, size_t len)
{
	uint8_t *msg, *head;

	msg = calloc(1, len * sizeof(uint8_t));
	if (!msg)
		return NULL;

	head = msg;
	bufptr_put_u8(msg, WLAN_ACTION_PUBLIC);
	bufptr_put_u8(msg, WLAN_PA_VENDOR_SPECIFIC);
	bufptr_put(msg, "\x50\x6f\x9a", 3);	// WFA OUI
	bufptr_put_u8(msg, DPP_OUI_TYPE);
	bufptr_put_u8(msg, 1); /* Crypto Suite */
	bufptr_put_u8(msg, type);

	pr_debug("%s: head = %p   attr_start = %p\n", __func__, head, msg);

	bufprintf("Msg-header", head, 8);

	return head;
}

int dpp_gen_key(const char *curve, void **key)
{
	void *k;

	*key = NULL;
	if (!curve || strncmp(curve, "P-256", 5))
		return -1;

	k = ecc_key_gen(curve);
	if (k)
		ecc_key_print("Generated key", k);

	*key = k;
	return 0;
}

int dpp_pubkey_hash(const uint8_t *der, size_t der_len, uint8_t *digest)
{
	const uint8_t *addr[2];
	size_t len[2];

	if (!digest || !der)
		return -1;

	addr[0] = der;
	len[0] = der_len;
	if (PLATFORM_SHA256(1, addr, len, digest) < 0)
		return -1;

	bufprintf("DPP: Public key hash", digest, SHA256_MAC_LEN);
	return 0;
}

int dpp_pubkey_hash_with_chirp(const uint8_t *der, size_t der_len, uint8_t *digest)
{
	const uint8_t *addr[2];
	size_t len[2];

	if (!digest || !der)
		return -1;

	addr[0] = (const uint8_t *)"chirp";
	len[0] = 5;
	addr[1] = der;
	len[1] = der_len;
	if (PLATFORM_SHA256(2, addr, len, digest) < 0)
		return -1;

	bufprintf("DPP: Public key hash (chirp)", digest, SHA256_MAC_LEN);
	return 0;
}

int dpp_gen_uri(int argc, char **argv, char **out)
{
	struct getopt_option opt = {0};
	char chanstr[16] = {0};
	char macstr[32] = {0};
	char verstr[32] = {0};
	char keystr[256] = {0};
	char infostr[256] = {0};
	size_t len;
	char *uri;
	int ch;

	*out = NULL;
	len = 4; /* "DPP:" */

	while ((ch = getopt_r(argc, argv, "C:M:V:K:B:H:I:", &opt)) != -1) {
		switch (ch) {
		case 'C':
			//channel = opt.value;
			len += 3 + strlen(opt.value);	/* C:...; */
			snprintf(chanstr, sizeof(chanstr), "C:%s;", opt.value);
			pr_debug("chanstr: %s\n", chanstr);
			break;
		case 'M':
			//macaddr = opt.value;
			len += 3 + strlen(opt.value);	/* M:...; */
			snprintf(macstr, sizeof(macstr), "M:%12s;", opt.value);
			pr_debug("macstr: %s\n", macstr);
			break;
		case 'V':
			//version = opt.value;
			len += 3 + strlen(opt.value);	/* V:...; */
			snprintf(verstr, sizeof(verstr), "V:%s;", opt.value);
			pr_debug("verstr: %s\n", verstr);
			break;
		case 'K':
			//key = opt.value;
			len += 3 + strlen(opt.value);	/* K:...; */
			snprintf(keystr, sizeof(keystr), "K:%s;", opt.value);
			pr_debug("keystr: %s\n", keystr);
			break;
		case 'I':
			//info = opt.value;
			len += 3 + strlen(opt.value);	/* I:...; */
			snprintf(infostr, sizeof(infostr), "I:%s;", opt.value);
			pr_debug("Infostr: %s\n", infostr);
			break;
		default:
			break;
		}
	}

	len += 2;	/* ; and nul terminate */

	uri = calloc(1, len * sizeof(char));
	if (!uri)
		return -1;

	snprintf(uri, len, "DPP:%s%s%s%s%s;",
		 chanstr,
		 macstr,
		 infostr,
		 verstr,
		 keystr);

	*out = uri;
	return 0;
}

int dpp_gen_bootstrap_info(const char *curve, char *channel_list, char *mac,
			   char *info, struct dpp_bootstrap_info **bi)
{
	struct dpp_bootstrap_info *b;
	size_t base64_len = 0;
	char *argv[16] = {0};
	char *base64 = NULL;
	uint8_t *der = NULL;
	size_t der_len = 0;
	char *pos, *end;
	int i = 0;
	int argc;
	int ret;


	*bi = NULL;
	b = calloc(1, sizeof(*b));
	if (!b)
		return -1;

	b->own = 1;
	b->type = 0;			/* qr code */
	b->curve_ike_grp = 19;		/* prime256v1 */
	ret = dpp_gen_key(curve, &b->key);
	if (ret) {
		free(b);
		return -1;
	}

	der = ecc_key_get_spki(b->key, &der_len);
	if (!der)
		goto fail_1;

	b->pubkey = der;
	b->pubkeylen = der_len;

	bufprintf("DPP: bootstrap: DER encoded SubjectPublicKeyInfo", der, der_len);

	ret = dpp_pubkey_hash(der, der_len, b->pubkey_hash);
	if (ret)
		goto fail_1;

	ret = dpp_pubkey_hash_with_chirp(der, der_len, b->pubkey_hash_chirp);
	if (ret)
		goto fail_1;

	base64_len = der_len * 2;
	base64 = calloc(1, base64_len * sizeof(char));
	if (!base64)
		goto fail_2;
#if defined(RTCONFIG_SWRTMESH)
	ret = easy_base64_encode(der, der_len, (unsigned char *)base64, &base64_len);
#else
	ret = base64_encode(der, der_len, (unsigned char *)base64, &base64_len);
#endif
	if (ret)
		goto fail_2;

	pos = base64;
	end = pos + base64_len;
	for (;;) {
		pos = strchr(pos, '\n');
		if (!pos)
			break;

		memmove(pos, pos + 1, end - pos);
	}

	b->pk = base64;
	pr_debug("pk = %s\n", b->pk);

	/* prepare uri */
	if (channel_list) {
		argv[i++] = "-C";
		argv[i++] = channel_list;
	}

	if (mac) {
		argv[i++] = "-M";
		argv[i++] = mac;
	}

	if (info) {
		argv[i++] = "-I";
		argv[i++] = info;
	}

	argv[i++] = "-V";
	argv[i++] = "2";

	argv[i++] = "-B";
	argv[i++] = "1";	/* supp-curves = prime256v1 */

	argv[i++] = "-K";
	argv[i++] = b->pk;

	argc = i;

	ret = dpp_gen_uri(argc, argv, &b->uri);
	if (ret)
		goto fail_2;

	*bi = b;
	return 0;

fail_2:
	memset(der, 0, der_len);
	free(der);
	der = NULL;
fail_1:
	EVP_PKEY_free(b->key);
	b->key = NULL;
	b->own = 0;
	return -1;
}

int dpp_parse_uri(const char *uri, int *argc, char **argv)
{
	char *s, *tmp = strdup(uri);
	const char *tokens[] = {"C", "M", "I", "K", "V", "B", "H"};
	int num_tokens = 7;
	int lim = *argc;
	int i, k = 0;

	if (!(s = strstr(tmp, "DPP:")) || strncmp(tmp, s, strlen(tmp))) {
		pr_debug("Invalid URI: doesn't start with 'DPP:'\n");
		errno = -EINVAL;
		return -1;
	}

	tmp += strlen("DPP:");
	*argc = 0;

	foreach_token(s, tmp, ";") {
		int invalid = 1;

		if (!strncmp(s, ";", 1) || !(strncmp(s, "\n", 1)))
			continue;

		pr_debug("Token: '%s'\n", s);
		for (i = 0; i < num_tokens; i++) {
			if (!strncmp(s, tokens[i], 1)) {
				invalid = 0;
				break;
			}
		}

		if (invalid) {
			errno = -EINVAL;
			return -1;
		}

		argv[k++] = strdup(s);
		if (k >= lim) {
			errno = -E2BIG;
			return -1;
		}
	}

	*argc = k;
	pr_debug("num-tokens = %d\n", *argc);
	return 0;
}

int dpp_build_bootstrap_info_from_uri(const char *uri,
				      struct dpp_bootstrap_info *bo)
{
	char *ret_argv[12] = {0};
	int ret_argc = 12;
	int ret = 0;
	int i;

	ret = dpp_parse_uri(uri, &ret_argc, ret_argv);
	if (ret)
		return -1;

	pr_debug("URI is valid\n");

	memset(bo, 0, sizeof(struct dpp_bootstrap_info));
	bo->type = 0;			/* qr-code */
	bo->curve_ike_grp = 19;		/* prime256v1 */

	for (i = 0; i < ret_argc; i++) {
		char *tmp = ret_argv[i];

		//pr_debug("argv[%d] = '%s'\n", i, ret_argv[i]);
		if (!strncmp(tmp, "C:", 2)) {
			char *c;

			tmp += 2;
			foreach_token(c, tmp, "/") {
				if (bo->num_channel >= 2 * NUM_OPCLASS_CHANNEL_PAIR)
					break;

				bo->opclass_channel[bo->num_channel++] = atoi(c);
			}
			pr_debug("opclass,channel: ");
			for (int n = 0; n < bo->num_channel; n += 2) {
				pr_debug("(%d, %d) ", bo->opclass_channel[n], bo->opclass_channel[n+1]);
			}
			pr_debug("\n");

		} else if (!strncmp(tmp, "M:", 2)) {
			tmp += 2;
			strtob(tmp, 6, bo->macaddr);
			pr_debug("macaddr: " MACFMT"\n", MAC2STR(bo->macaddr));
		} else if (!strncmp(tmp, "V:", 2)) {
			tmp += 2;
			bo->version = atoi(tmp);
			pr_debug("version: %d\n", bo->version);
		} else if (!strncmp(tmp, "K:", 2)) {
			uint8_t out[512] = {0};
			size_t olen = 512;

			tmp += 2;
			bo->pk = strdup(tmp);
			pr_debug("pk: %s\n", bo->pk);
#if defined(RTCONFIG_SWRTMESH)
			if (easy_base64_decode((unsigned char *)bo->pk, strlen(bo->pk), out, &olen)) {
#else
			if (base64_decode((unsigned char *)bo->pk, strlen(bo->pk), out, &olen)) {
#endif
				pr_debug("%s: Invalid 'K:'\n", __func__);
				ret = -1;
				break;
			}

			bo->pubkey = calloc(1, olen);
			if (!bo->pubkey) {
				ret = -1;
				break;
			}

			memcpy(bo->pubkey, out, olen);
			bo->pubkeylen = olen;

			ret = dpp_pubkey_hash(bo->pubkey, bo->pubkeylen, bo->pubkey_hash);
			if (ret)
				pr_debug("%s: Failed: sha256(pubkey)\n", __func__);

			ret = dpp_pubkey_hash_with_chirp(bo->pubkey, bo->pubkeylen, bo->pubkey_hash_chirp);
			if (ret)
				pr_debug("%s: Failed: sha256(chirp|pubkey)\n", __func__);

			pr_debug("pubkey: %s\n", bo->pubkey);

			bo->key = ecc_key_gen_from_spki(bo->pubkey, bo->pubkeylen);
			if (!bo->key) {
				pr_debug("%s: Failed: to create key from SPKI\n", __func__);
				ret = -1;
				break;
			}
		} else if (!strncmp(tmp, "I:", 2)) {
			tmp += 2;
			bo->info = strdup(tmp);
			pr_debug("info: %s\n", bo->info);
		} else if (!strncmp(tmp, "B:", 2)) {
			tmp += 2;
			bo->supported_curves = atoi(tmp);
			pr_debug("supported-curves: 0x%02x\n", bo->supported_curves);
		}
	}

	for (i = 0; i < ret_argc; i++)
		free(ret_argv[i]);

	return ret;
}

uint8_t *dpp_get_bootstrap_pubkey_hash(struct dpp_bootstrap_info *b, uint16_t *outlen)
{
	if (!b)
		return NULL;

	if (!b->uri)
		return NULL;

	*outlen = SHA256_MAC_LEN;
	return b->pubkey_hash_chirp;
}

void dpp_print_bootstrap_info(struct dpp_bootstrap_info *b)
{
	if (!b)
		return;

	if (b->uri)
		pr_debug("URI: %s\n", b->uri);

	pr_debug("Own: %d\n", b->own);
	pr_debug("Type: %d\n", b->type);
	pr_debug("Curve-ikegroup: %hd\n", b->curve_ike_grp);
	pr_debug("Macaddress: " MACFMT "\n", MAC2STR(b->macaddr));
	pr_debug("Verion: %u\n", b->version);
	pr_debug("Supported-curves: %u\n", b->supported_curves);
	if (b->info)
		pr_debug("Info: %s\n", b->info);

	pr_debug("Pubkey: ");
	if (b->pubkey && b->pubkeylen > 0) {
		for (int i = 0; i < b->pubkeylen; i++)
			pr_debug("%02x ", b->pubkey[i] & 0xff);
	}
	pr_debug("\n");

	pr_debug("SHA256(Pubkey): ");
	for (int i = 0; i < SHA256_MAC_LEN; i++)
		pr_debug("%02x ", b->pubkey_hash[i] & 0xff);
	pr_debug("\n");

	pr_debug("SHA256(\"chirp\"|Pubkey): ");
	for (int i = 0; i < SHA256_MAC_LEN; i++)
		pr_debug("%02x ", b->pubkey_hash_chirp[i] & 0xff);
	pr_debug("\n");
}

char *dpp_get_bootstrap_uri(struct dpp_bootstrap_info *b)
{
	if (!b)
		return NULL;

	return b->uri;
}


static int dpp_derive_kid(void *key, char **kid)
{
	uint8_t kid_hash[SHA256_MAC_LEN];
	const uint8_t *addr[1];
	uint8_t *pub = NULL;
	size_t kidlen = 0;
	size_t publen;
	size_t len[1];
	char *k;
	int ret;


	*kid = NULL;
	ret = ecc_key_get_pubkey_point(key, 1, &pub, &publen);
	if (ret) {
		pr_debug("%s: Failed to derive kid\n", __func__);
		return -1;
	}

	/* kid = base64url(SHA256(ANSI X9.63 uncompressed key)) */
	addr[0] = pub;
	len[0] = publen;
	ret = PLATFORM_SHA256(1, addr, len, kid_hash);
	free(pub);
	if (ret) {
		pr_debug("%s: Failed to derive kid\n", __func__);
		return -1;
	}

	k = calloc(1, 128);
	if (!k)
		return -1;

	kidlen = base64url_encode((uint8_t *)k, kid_hash, sizeof(kid_hash));
	if (kidlen) {
		*kid = k;
		pr_debug("%s: kid = %s\n", __func__, *kid);
	}

	return 0;
}

int dpp_derive_k1(const uint8_t *Mx, size_t Mx_len, uint8_t *k1, unsigned int hash_len)
{
	uint8_t salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "first intermediate key";
	const uint8_t *addr[1];
	size_t addrlen[1];
	int res;


	if (hash_len != 32)	/* hashlen for prime256v1 */
		return -1;

	/* k1 = HKDF(<>, "first intermediate key", M.x) */

	/* HKDF-Extract(<>, M.x) */
	memset(salt, 0, hash_len);
	addr[0] = (const uint8_t *)Mx;
	addrlen[0] = Mx_len;

	res = PLATFORM_HMAC_SHA256(salt, hash_len, 1, addr, addrlen, prk);
	if (res < 0)
		return -1;

	bufprintf("DPP: PRK = HKDF-Extract(<>, IKM=M.x)", prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = hmac_sha256_kdf(prk, hash_len, NULL, (const uint8_t *)info, strlen(info), k1, hash_len);
	memset(prk, 0, hash_len);
	if (res < 0)
		return -1;

	bufprintf("DPP: k1 = HKDF-Expand(PRK, info, L)", k1, hash_len);
	return 0;
}

int dpp_derive_k2(const uint8_t *Nx, size_t Nx_len, uint8_t *k2, unsigned int hash_len)
{
	uint8_t salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "second intermediate key";
	const uint8_t *addr[1];
	size_t addrlen[1];
	int res;


	if (hash_len != 32)	/* hashlen for prime256v1 */
		return -1;

	pr_debug("%s: %d\n", __func__, __LINE__);
	/* k2 = HKDF(<>, "second intermediate key", N.x) */

	/* HKDF-Extract(<>, N.x) */
	memset(salt, 0, hash_len);
	addr[0] = (const uint8_t *)Nx;
	addrlen[0] = Nx_len;

	pr_debug("%s: %d\n", __func__, __LINE__);
	res = PLATFORM_HMAC_SHA256(salt, hash_len, 1, addr, addrlen, prk);
	//res = dpp_hmac(hash_len, salt, hash_len, Nx, Nx_len, prk);
	if (res < 0) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		return -1;
	}

	bufprintf("DPP: PRK = HKDF-Extract(<>, IKM=N.x)", prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = hmac_sha256_kdf(prk, hash_len, NULL, (const uint8_t *)info, strlen(info), k2, hash_len);
	memset(prk, 0, hash_len);
	if (res < 0)
		return -1;

	bufprintf("DPP: k2 = HKDF-Expand(PRK, info, L)", k2, hash_len);
	return 0;
}

int dpp_derive_pmk(const uint8_t *Nx, size_t Nx_len, uint8_t *pmk, unsigned int hash_len)
{
	uint8_t salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "DPP PMK";
	const uint8_t *addr[1];
	size_t addrlen[1];
	int res;

	if (hash_len != 32)	/* hashlen for prime256v1 */
		return -1;

	pr_debug("%s: %d\n", __func__, __LINE__);
	/* PMK = HKDF(<>, "DPP PMK", N.x) */

	/* HKDF-Extract(<>, N.x) */
	memset(salt, 0, hash_len);
	addr[0] = (const uint8_t *)Nx;
	addrlen[0] = Nx_len;

	res = PLATFORM_HMAC_SHA256(salt, hash_len, 1, addr, addrlen, prk);
	if (res < 0) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		return -1;
	}

	bufprintf("DPP: PRK = HKDF-Extract(<>, IKM=N.x)", prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = hmac_sha256_kdf(prk, hash_len, NULL, (const uint8_t *)info, strlen(info), pmk, hash_len);
	memset(prk, 0, hash_len);
	if (res < 0)
		return -1;

	bufprintf("DPP: PMK = HKDF-Expand(PRK, info, L)", pmk, hash_len);
	return 0;
}

int dpp_derive_pmkid(const struct dpp_curve_params *curve,
		     void *own_key,
		     void *peer_key, uint8_t *pmkid)
{
	uint8_t hash[SHA256_MAC_LEN];
	size_t nkx_len, pkx_len;
	const uint8_t *addr[2];
	uint8_t *nkx, *pkx;
	size_t len[2];
	int ret;

	/* PMKID = Truncate-128(H(min(NK.x, PK.x) | max(NK.x, PK.x))) */
	ret = ecc_key_get_pubkey_point(own_key, 0, &nkx, &nkx_len);
	ret |= ecc_key_get_pubkey_point(peer_key, 0, &pkx, &pkx_len);
	if (ret)
		goto fail;

	addr[0] = nkx;
	len[0] = nkx_len / 2;
	addr[1] = pkx;
	len[1] = pkx_len / 2;
	if (len[0] != len[1])
		goto fail;

	if (memcmp(addr[0], addr[1], len[0]) > 0) {
		addr[0] = pkx;
		addr[1] = nkx;
	}

	//bufprintf("DPP: PMKID hash payload 1", addr[0], len[0]);
	//bufprintf("DPP: PMKID hash payload 2", addr[1], len[1]);
	ret = PLATFORM_SHA256(2, addr, len, hash);
	if (ret)
		goto fail;

	bufprintf("DPP: PMKID hash output", hash, SHA256_MAC_LEN);
	memcpy(pmkid, hash, PMKID_LEN);
	bufprintf("DPP: PMKID", pmkid, PMKID_LEN);
	ret = 0;
fail:
	free(nkx);
	free(pkx);
	return ret;
}

int dpp_derive_bk_ke(struct dpp_peer *peer)
{
	unsigned int hash_len = peer->curve_hash_len;
	size_t nonce_len = peer->curve_nonce_len;
	uint8_t nonces[2 * DPP_NONCE_LEN];
	const char *info_ke = "DPP Key";
	const uint8_t *addr[3];
	size_t num_elem = 0;
	size_t len[3];
	int res;

	if (!peer->Mx_len || !peer->Nx_len) {
		pr_debug("DPP: Mx/Nx not available - cannot derive ke\n");
		return -1;
	}

	/* bk = HKDF-Extract(I-nonce | R-nonce, M.x | N.x [| L.x]) */
	memcpy(nonces, peer->i_nonce, nonce_len);
	memcpy(&nonces[nonce_len], peer->r_nonce, nonce_len);
	addr[num_elem] = peer->Mx;
	len[num_elem] = peer->Mx_len;
	num_elem++;
	addr[num_elem] = peer->Nx;
	len[num_elem] = peer->Nx_len;
	num_elem++;
	if (peer->peer_bi && peer->own_bi) {
		if (!peer->Lx_len) {
			pr_debug("DPP: Lx not available - cannot derive ke\n");
			return -1;
		}
		addr[num_elem] = peer->Lx;
		len[num_elem] = peer->Lx_len;
		num_elem++;
	}

	res = PLATFORM_HMAC_SHA256(nonces, 2 * nonce_len, num_elem, addr, len, peer->bk);
	if (res < 0)
		return -1;

	bufprintf("DPP: bk = HKDF-Extract(I-nonce | R-nonce, M.x | N.x [| L.x])",
		  peer->bk, hash_len);

	/* ke = HKDF-Expand(bk, "DPP Key", length) */
	res = hmac_sha256_kdf(peer->bk, hash_len, NULL, (const uint8_t *)info_ke,
			      strlen(info_ke), peer->ke, hash_len);
	if (res < 0)
		return -1;

	bufprintf("DPP: ke = HKDF-Expand(bk, \"DPP Key\", length)", peer->ke, hash_len);

	return 0;
}

int dpp_gen_r_auth(struct dpp_peer *peer, uint8_t *r_auth, int is_initiator)
{
	uint8_t *pix = NULL, *prx = NULL, *bix = NULL, *brx = NULL;
	size_t pix_len = 0, prx_len = 0, bix_len = 0, brx_len = 0;
	const uint8_t *addr[7];
	size_t num_elem = 0;
	size_t nonce_len;
	uint8_t zero = 0;
	size_t len[7];
	int ret = -1;
	size_t i;


	/* R-auth = H(I-nonce | R-nonce | PI.x | PR.x | [BI.x |] BR.x | 0) */
	nonce_len = peer->curve_nonce_len;

	pr_debug("%s: %d peer->initiator = %d\n", __func__, __LINE__, peer->initiator);
	if (peer->initiator) {
		ret = ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &pix, &pix_len) ||
		      ecc_key_get_pubkey_point(peer->peer_protocol_key, 0, &prx, &prx_len);

		if (ret)
			goto fail;

		if (peer->own_bi)
			ret = ecc_key_get_pubkey_point(peer->own_bi->key, 0, &bix, &bix_len);
		else
			bix = NULL;

		if (ret)
			goto fail;

		ret = ecc_key_get_pubkey_point(peer->peer_bi->key, 0, &brx, &brx_len);
		if (ret)
			goto fail;
	} else {
		ret = ecc_key_get_pubkey_point(peer->peer_protocol_key, 0, &pix, &pix_len) ||
		      ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &prx, &prx_len);

		if (ret)
			goto fail;

		if (peer->peer_bi)
			ret = ecc_key_get_pubkey_point(peer->peer_bi->key, 0, &bix, &bix_len);
		else
			bix = NULL;

		if (ret)
			goto fail;

		ret = ecc_key_get_pubkey_point(peer->own_bi->key, 0, &brx, &brx_len);
		if (ret)
			goto fail;
	}

	if (!pix || !prx || !brx)
		goto fail;

	addr[num_elem] = peer->i_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = peer->r_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = pix;
	len[num_elem] = pix_len / 2;
	num_elem++;

	addr[num_elem] = prx;
	len[num_elem] = prx_len / 2;
	num_elem++;

	if (bix) {
		addr[num_elem] = bix;
		len[num_elem] = bix_len / 2;
		num_elem++;
	}

	addr[num_elem] = brx;
	len[num_elem] = brx_len / 2;
	num_elem++;

	addr[num_elem] = &zero;
	len[num_elem] = 1;
	num_elem++;

	pr_debug("DPP: R-auth hash components\n");
	for (i = 0; i < num_elem; i++)
		bufprintf("DPP: hash component", (uint8_t *)addr[i], len[i]);

	ret = PLATFORM_SHA256(num_elem, addr, len, r_auth);
	if (!ret)
		bufprintf("DPP: R-auth", (uint8_t *)r_auth, peer->curve_hash_len);
fail:
	if (pix)
		free(pix);
	if (prx)
		free(prx);
	if (bix)
		free(bix);
	if (brx)
		free(brx);

	return ret;
}

int dpp_gen_i_auth(struct dpp_peer *peer, uint8_t *i_auth)
{
	uint8_t *pix = NULL, *prx = NULL, *bix = NULL, *brx = NULL;
	size_t pix_len = 0, prx_len = 0, bix_len = 0, brx_len = 0;
	const uint8_t *addr[7];
	size_t len[7];
	size_t i, num_elem = 0;
	size_t nonce_len;
	uint8_t one = 1;
	int ret = -1;

	/* I-auth = H(R-nonce | I-nonce | PR.x | PI.x | BR.x | [BI.x |] 1) */
	nonce_len = peer->curve_nonce_len;

	if (peer->initiator) {
		ret = ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &pix, &pix_len) ||
		      ecc_key_get_pubkey_point(peer->peer_protocol_key, 0, &prx, &prx_len);

		if (ret)
			goto fail;

		if (peer->own_bi)
			ret = ecc_key_get_pubkey_point(peer->own_bi->key, 0, &bix, &bix_len);
		else
			bix = NULL;

		if (!peer->peer_bi)
			goto fail;

		ret = ecc_key_get_pubkey_point(peer->peer_bi->key, 0, &brx, &brx_len);
		if (ret)
			goto fail;
	} else {
		ret = ecc_key_get_pubkey_point(peer->peer_protocol_key, 0, &pix, &pix_len) ||
		      ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &prx, &prx_len);

		if (ret)
			goto fail;

		if (peer->peer_bi)
			ret = ecc_key_get_pubkey_point(peer->peer_bi->key, 0, &bix, &bix_len);
		else
			bix = NULL;

		if (ret)
			goto fail;

		ret = ecc_key_get_pubkey_point(peer->own_bi->key, 0, &brx, &brx_len);
		if (ret)
			goto fail;
	}

	if (!pix || !prx || !brx)
		goto fail;

	addr[num_elem] = peer->r_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = peer->i_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = prx;
	len[num_elem] = prx_len / 2;
	num_elem++;

	addr[num_elem] = pix;
	len[num_elem] = pix_len / 2;
	num_elem++;

	addr[num_elem] = brx;
	len[num_elem] = brx_len / 2;
	num_elem++;

	if (bix) {
		addr[num_elem] = bix;
		len[num_elem] = bix_len / 2;
		num_elem++;
	}

	addr[num_elem] = &one;
	len[num_elem] = 1;
	num_elem++;

	pr_debug("DPP: I-auth hash components\n");
	for (i = 0; i < num_elem; i++)
		bufprintf("DPP: hash component", (uint8_t *)addr[i], len[i]);

	ret = PLATFORM_SHA256(num_elem, addr, len, i_auth);
	if (!ret)
		bufprintf("DPP: I-auth", (uint8_t *)i_auth, peer->curve_hash_len);
fail:
	if (pix)
		free(pix);
	if (prx)
		free(prx);
	if (bix)
		free(bix);
	if (brx)
		free(brx);

	return ret;
}


void dpp_configurator_free(struct dpp_configurator *c)
{
	if (!c)
		return;

	ecc_key_free(c->csign_key);
	ecc_key_free(c->connector_key);
	ecc_key_free(c->pp_key);
	free(c->kid);
	free(c->connector);
	free(c);
}

int dpp_configurator_init(struct dpp_context *ctx, void **configurator,
			  int argc, char **argv)
{
	struct getopt_option opt = {0};
	const char *csign_keyfile = NULL;
	const char *pp_keyfile = NULL;
	const char *curve = "P-256";
	struct dpp_configurator *c;
	int ret;
	int ch;

	while ((ch = getopt_r(argc, argv, "c:k:p:", &opt)) != -1) {
		switch (ch) {
		case 'c':
			curve = opt.value;
			break;
		case 'k':
			csign_keyfile = opt.value;
			break;
		case 'p':
			pp_keyfile = opt.value;
			break;
		default:
			break;
		}
	}

	*configurator = NULL;
	c = calloc(1, sizeof(*c));
	if (!c)
		return -1;

	if (csign_keyfile)
		dpp_read_key_from_file(csign_keyfile, &c->csign_key);
	else
		dpp_gen_key(curve, &c->csign_key);

	if (pp_keyfile)
		dpp_read_key_from_file(pp_keyfile, &c->pp_key);
	else
		dpp_gen_key(curve, &c->pp_key);

	if (!c->csign_key || !c->pp_key)
		goto fail;

	ret = dpp_derive_kid(c->csign_key, &c->kid);
	if (ret)
		goto fail;

	c->id = dpp_next_configurator_id(ctx);

	*configurator = c;
	return 0;

fail:
	dpp_configurator_free(c);
	return -1;
}

int dpp_configurator_sign_config(void *configurator, void *config)
{
	return 0;
}

int dpp_config_add(void **config, int argc, char **argv)
{
	//struct dpp_config_obj *new;
	//TODO
	return 0;
}

int dpp_build_configuration_object(void *ctx, void *peer, int netrole, uint8_t **obj, size_t *objlen)
{
	//TODO
	return 0;
}

/* When argc = 0, default init as responder, enrollee with ver2 */
int dpp_init(void **inst, int argc, char *argv[])
{
	struct getopt_option opt = {0};
	struct dpp_context *ctx;
	char *urifile = NULL;
	int ch, v;


	*inst = NULL;
	ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		return -1;

	ctx->version = 2;
	INIT_LIST_HEAD(&ctx->own_bilist);
	INIT_LIST_HEAD(&ctx->peer_bilist);
	//INIT_LIST_HEAD(&ctx->bslist);
	INIT_LIST_HEAD(&ctx->conflist);
	INIT_LIST_HEAD(&ctx->configuratorlist);
	ctx->num_peers = 0;
	INIT_LIST_HEAD(&ctx->peerlist);

	while ((ch = getopt_r(argc, argv, "IECR:V:K:U:", &opt)) != -1) {
		switch (ch) {
		case 'I':
			ctx->initiator = true;
			break;
		case 'C': {
			struct dpp_configurator *c;
			int ret;

			ctx->role |= DPP_CONFIGURATOR;
			ret = dpp_configurator_init(ctx, (void **)&c, 0, NULL);
			if (ret) {
				/* TODO: goto out;? */
				break;
			}

			list_add(&c->list, &ctx->configuratorlist);

			break;
		}
		case 'E':
			ctx->role |= DPP_ENROLLEE;
			break;
		case 'V':
			v = atoi(opt.value);
			if (v >= 2 && v <= 3)
				ctx->version = v;
			break;
		case 'K':
			ctx->keyfile = strdup(opt.value);
			break;
		case 'U':
			urifile = strdup(opt.value);
			break;
		default:
			break;
		}
	}

	if (ctx->role == 0)
		ctx->role |= DPP_ENROLLEE;	/* default enrollee */

	if (urifile) {
		dpp_load_bootstrap_uri(ctx, urifile);
		free(urifile);
	}

	*inst = ctx;
	return 0;
}

int dpp_register_cb(void *ctx, int (*cb)(void *ctx, uint8_t *smac, enum dpp_event e, uint8_t *data, size_t len))
{
	struct dpp_context *c = (struct dpp_context *)ctx;

	if (!c)
		return -1;

	if (cb)
		c->cb = cb;

	return 0;
}

int dpp_trigger(void *ctx, uint8_t *peer_macaddr, void *event)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_peer *peer;
	struct dpp_sm *mc;
	int ret;

	UNUSED(mc);

	if (!c || !peer_macaddr)
		return -1;

	peer = dpp_lookup_peer(c, peer_macaddr);
	if (!peer)
		return -1;

	ret = dpp_sm_process(c, peer->sm, event);

	return ret;
}

void dpp_sm_free_event(void *ev)
{
	struct dpp_sm_event *e = (struct dpp_sm_event *)ev;

	if (e->len)
		free(e->data);
	free(e);
}

void *dpp_sm_create_event(void *ctx, uint8_t *peer_macaddr, int event_type,
			size_t event_len, uint8_t *event_data)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_peer *peer;
	struct dpp_sm *mc;
	struct dpp_sm_event *e;

	UNUSED(mc);

	if (!c || !peer_macaddr)
		return NULL;

	peer = dpp_lookup_peer(c, peer_macaddr);
	if (!peer)
		return NULL;

	e = calloc(1, sizeof(*e));
	if (!e)
		return NULL;

	e->type = event_type;
	e->len = event_len;
	if (event_len) {
		e->data = calloc(1, event_len);
		if (!e->data) {
			free(e);
			return NULL;
		}
		memcpy(e->data, event_data, event_len);
	}

	return e;
}

struct dpp_peer *dpp_alloc_peer(struct dpp_context *ctx)
{
	struct dpp_peer *peer;

	peer = calloc(1, sizeof(*peer));
	if (!peer)
		return NULL;

	peer->ctx = ctx;
	peer->curve_hash_len = 32;
	peer->curve_nonce_len = 16;

	INIT_LIST_HEAD(&peer->conf_objlist);

	return peer;
}

int dpp_set_peer_private_data(void *ctx, uint8_t *peer_macaddr, void *priv)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_peer *peer;

	if (!c || !peer_macaddr)
		return -1;

	peer = dpp_lookup_peer(c, peer_macaddr);
	if (!peer)
		return -1;

	peer->priv = priv;
	return 0;
}

void *dpp_get_peer_private_data(void *ctx, uint8_t *peer_macaddr)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_peer *peer;

	if (!c || !peer_macaddr)
		return NULL;

	peer = dpp_lookup_peer(c, peer_macaddr);
	if (!peer)
		return NULL;

	return peer->priv;
}

int dpp_set_ctx_private_data(void *ctx, void *priv)
{
	struct dpp_context *c = (struct dpp_context *)ctx;

	c->priv = priv;
	return 0;
}

void *dpp_get_ctx_private_data(void *ctx)
{
	struct dpp_context *c = (struct dpp_context *)ctx;

	return c->priv;
}

struct dpp_peer *dpp_create_initiator_instance(struct dpp_context *ctx)
{
	struct dpp_peer *p;
	int ret;

	p = dpp_alloc_peer(ctx);
	if (!p)
		return NULL;

	p->initiator = 1;
	p->i_capab |= DPP_CAPAB_CONFIGURATOR;
	ret = dpp_sm_create(1, &p->sm);
	if (!ret)
		dpp_sm_set_data(p->sm, p);

	list_add(&p->list, &ctx->peerlist);
	return p;
}

struct dpp_peer *dpp_create_responder_instance(struct dpp_context *ctx)
{
	uint8_t bcast_macaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	struct dpp_peer *p;
	int ret;

	p = dpp_alloc_peer(ctx);
	if (!p)
		return NULL;

	p->initiator = 0;
	p->r_capab |= DPP_CAPAB_ENROLLEE;
	memcpy(p->peer_macaddr, bcast_macaddr, 6);

	/* TODO: if bootstrap_info already available, use that instead */
	dpp_gen_bootstrap_info("P-256", "81/1,115/36", NULL, NULL, &p->own_bi);

	list_add(&p->list, &ctx->peerlist);
	/* TODO: initialize other values in dpp_peer for enrollee side */

	ret = dpp_sm_create(0, &p->sm);
	if (!ret)
		dpp_sm_set_data(p->sm, p);
	return p;
}

void dpp_free_peer(struct dpp_context *ctx, struct dpp_peer *peer)
{
	struct dpp_sm *sm = peer->sm;

	if (peer->ctx) {
		peer->ctx = NULL;
		if (peer->own_bi)
			dpp_bootstrap_free(peer->own_bi);

		timer_del(&sm->t);
		free(sm);
		free(peer);
	}
}

struct dpp_peer *dpp_lookup_peer(struct dpp_context *ctx, uint8_t *macaddr)
{
	struct dpp_peer *p = NULL;

	if (!ctx || !macaddr || hwaddr_is_zero(macaddr))
		return NULL;

	list_for_each_entry(p, &ctx->peerlist, list) {
		if (!memcmp(p->peer_macaddr, macaddr, 6))
			return p;
	}

	return NULL;
}

struct dpp_bootstrap_info *dpp_lookup_chirp(struct dpp_context *ctx, uint8_t *chirp)
{
	struct dpp_bootstrap_info *b = NULL;

	if (!ctx || !chirp)
		return NULL;

	list_for_each_entry(b, &ctx->peer_bilist, list) {
		if (!memcmp(b->pubkey_hash_chirp, chirp, SHA256_MAC_LEN))
			return b;
	}

	return NULL;
}

struct dpp_peer *dpp_lookup_peer_by_chirp(struct dpp_context *ctx, uint8_t *chirp)
{
	struct dpp_bootstrap_info *b;
	struct dpp_peer *p = NULL;

	UNUSED(b);

	if (!ctx || !chirp)
		return NULL;

	list_for_each_entry(p, &ctx->peerlist, list) {
		if (!p->peer_bi)
			continue;

		if (!memcmp(p->peer_bi->pubkey_hash_chirp, chirp, SHA256_MAC_LEN))
			return p;
	}

	return NULL;
}

//TODO: remove
int dpp_create_peer_context(struct dpp_context *ctx,
				struct dpp_bootstrap_info *peer_bi,
				struct dpp_bootstrap_info *own_bi,
				enum dpp_role role,
				void **x)
{
	struct dpp_peer *peer;

	peer = calloc(1, sizeof(*peer));
	if (!peer)
		return -1;

	if (own_bi)
		peer->own_bi = own_bi;
	else
		dpp_gen_bootstrap_info("prime256v1", "81/1,115/36", NULL, NULL, &peer->own_bi);

	peer->peer_bi = peer_bi;
	peer->role = role;
	peer->ctx = ctx;
	*x = peer;

	return 0;
}

/* lookup peer's bootstrap hash, if known return 1 */
int dpp_is_peer_bootstrap_hash_known(void *context,
				     const uint8_t *bootstrap_hash_chirp,
				     uint8_t *peer_macaddr)
{
	struct dpp_context *ctx = context;
	struct dpp_bootstrap_info *bh;

	if (!context)
		return -1;

	list_for_each_entry(bh, &ctx->peer_bilist, list) {
		bufprintf("Bootstrap Hash", bh->pubkey_hash, 32);

		if (!memcmp(bh->pubkey_hash_chirp, bootstrap_hash_chirp, SHA256_MAC_LEN)) {
			if (peer_macaddr) {
				if (hwaddr_equal(peer_macaddr, bh->macaddr))
					return 1;
				else
					return 0;
			}

			return 1;
		}
	}

	return 0;
}

void dpp_bootstrap_free(struct dpp_bootstrap_info *b)
{
	if (b) {
		if (b->key)
			EVP_PKEY_free(b->key);
		if (b->uri)
			free(b->uri);
		if (b->info)
			free(b->info);
		if (b->pubkey) {
			memset(b->pubkey, 0, b->pubkeylen);
			free(b->pubkey);
			b->pubkey = NULL;
		}
		if (b->pk)
			free(b->pk);
		free(b);
	}
}

int dpp_bootstrap_add(void *context, struct dpp_bootstrap_info *b)
{
	struct dpp_context *ctx = (struct dpp_context *)context;
	struct dpp_bootstrap_info *bi = NULL;

	if (!ctx)
		return -1;

	list_for_each_entry(bi, &ctx->peer_bilist, list) {
		if (!memcmp(bi->pubkey_hash, b->pubkey_hash, SHA256_MAC_LEN)) {
			pr_debug("%s: Peer already bootstrapped\n", __func__);
			errno = -EEXIST;
			return -1;
		}
	}

	list_add_tail(&b->list, &ctx->peer_bilist);
	return 0;
}

int dpp_bootstrap_find_pair(void *context,
			    const uint8_t *own_bootstrap,
			    const uint8_t *peer_bootstrap,
			    struct dpp_bootstrap_info **own_bi,
			    struct dpp_bootstrap_info **peer_bi)
{
	struct dpp_context *ctx = context;
	struct dpp_bootstrap_info *bi = NULL;

	*own_bi = NULL;
	*peer_bi = NULL;

	if (!context)
		return -1;

	list_for_each_entry(bi, &ctx->own_bilist, list) {
		bufprintf("bi bootstrap", bi->pubkey_hash, 32);

		if (!memcmp(bi->pubkey_hash, own_bootstrap, SHA256_MAC_LEN)) {
			pr_debug("DPP: Found matching own bootstrapping information\n");
			*own_bi = bi;
			break;
		}
	}

	if (!*own_bi)
		return -1;

	list_for_each_entry(bi, &ctx->peer_bilist, list) {
		bufprintf("bi bootstrap", bi->pubkey_hash, 32);

		if (!memcmp(bi->pubkey_hash, peer_bootstrap, SHA256_MAC_LEN)) {
			pr_debug("DPP: Found matching peer bootstrapping information\n");
			*peer_bi = bi;
			return 0;
		}
	}

	return -1;
}

/* TODO: move to ec_utils.c */
int ecc_key_group(void *key)
{
	return 19;	//TODO
}
///////////////////////////////////////////////////


int dpp_process_auth_confirm(struct dpp_context *ctx,
			     uint8_t *src_macaddr,
			     uint8_t *frame, size_t framelen)
{
	const uint8_t *r_bootstrap, *i_bootstrap, *wrapped_data;
	const uint8_t *status, *i_auth;
	uint16_t r_bootstrap_len, i_bootstrap_len, wrapped_data_len;
	uint16_t status_len, i_auth_len;
	uint8_t i_auth2[DPP_MAX_HASH_LEN];
	uint8_t *unwrapped = NULL;
	size_t unwrapped_len = 0;
	const uint8_t *addr[2];
	struct dpp_peer *peer;
	size_t len[2];

	uint8_t *hdr = frame + 2;
	uint8_t *attr_start = frame + WLAN_DPP_HDR_LEN;
	size_t attr_len = framelen - WLAN_DPP_HDR_LEN;


	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (!peer)
		return -1;

	if (peer->initiator ||
	    !peer->own_bi ||
	    !peer->waiting_auth_confirm ||
	    peer->reconfig) {
		pr_debug("DPP: initiator=%d own_bi=%d  reconfig=%d\n",
			peer->initiator, !!peer->own_bi, peer->reconfig);
		pr_debug("Unexpected Authentication Confirm\n");
		return -1;
	}

	peer->waiting_auth_confirm = 0;

	wrapped_data = dpp_get_attr(attr_start, attr_len,
				    DPP_ATTR_WRAPPED_DATA,
				    &wrapped_data_len);
	if (!wrapped_data || wrapped_data_len < AES_BLOCK_SIZE) {
		pr_debug("%s: Missing or invalid 'wrapped_data'\n", __func__);
		return -1;
	}

	bufprintf("DPP: Wrapped data", (uint8_t *)wrapped_data, wrapped_data_len);

	attr_len = wrapped_data - 4 - attr_start;

	r_bootstrap = dpp_get_attr(attr_start, attr_len,
				   DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
				   &r_bootstrap_len);
	if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
		pr_debug("%s: Missing or invalid 'r_bootstrap'\n", __func__);
		return -1;
	}

	bufprintf("DPP: Responder Bootstrapping Key Hash", (uint8_t *) r_bootstrap, r_bootstrap_len);
	if (memcmp(r_bootstrap, peer->own_bi->pubkey_hash, SHA256_MAC_LEN)) {
		bufprintf("DPP: Expected Responder Bootstrapping Key Hash",
				peer->peer_bi->pubkey_hash, SHA256_MAC_LEN);
		pr_debug("%s: r_bootstrap key hash mismatch\n", __func__);
		return -1;
	}

	i_bootstrap = dpp_get_attr(attr_start, attr_len,
				   DPP_ATTR_I_BOOTSTRAP_KEY_HASH,
				   &i_bootstrap_len);
	if (i_bootstrap) {
		if (i_bootstrap_len != SHA256_MAC_LEN) {
			pr_debug("%s: Invalid 'i_bootstrap'\n", __func__);
			return -1;
		}

		bufprintf("DPP: Initiator Bootstrapping Key Hash",
				(uint8_t *)i_bootstrap, i_bootstrap_len);

		if (!peer->peer_bi ||
		    memcmp(i_bootstrap, peer->peer_bi->pubkey_hash, SHA256_MAC_LEN)) {
			pr_debug("Initiator Bootstrapping Key Hash mismatch\n");
			return -1;
		}
	} else if (peer->peer_bi) {
		/* Mutual authentication and peer did not include its
		 * Bootstrapping Key Hash attribute. */
		pr_debug("Missing Initiator Bootstrapping Key Hash attribute\n");
		return -1;
	}

	status = dpp_get_attr(attr_start, attr_len, DPP_ATTR_STATUS, &status_len);
	if (!status || status_len < 1) {
		pr_debug("Missing or invalid required DPP Status attribute\n");
		return -1;
	}

	pr_debug("DPP: Status %u\n", status[0]);
	peer->auth_status = status[0];
	if (status[0] != DPP_STATUS_OK) {
		pr_debug("Authentication failed\n");
		return -1;
	}

	addr[0] = hdr;
	len[0] = DPP_HDR_LEN;
	addr[1] = attr_start;
	len[1] = attr_len;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *) addr[0], len[0]);
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *) addr[1], len[1]);
	bufprintf("DPP: AES-SIV ciphertext", (uint8_t *) wrapped_data, wrapped_data_len);
	unwrapped_len = wrapped_data_len - AES_BLOCK_SIZE;
	unwrapped = calloc(1, unwrapped_len);
	if (!unwrapped)
		return -1;

	if (AES_SIV_DECRYPT(peer->ke, peer->curve_hash_len,
			    wrapped_data, wrapped_data_len,
			    2, addr, len, unwrapped) < 0) {
		pr_debug("AES-SIV decryption failed\n");
		goto fail;
	}

	bufprintf("DPP: AES-SIV cleartext", unwrapped, unwrapped_len);

	if (dpp_check_attrs(unwrapped, unwrapped_len) < 0) {
		pr_debug("Invalid attribute in unwrapped data\n");
		goto fail;
	}

	i_auth = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_I_AUTH_TAG,
			      &i_auth_len);
	if (!i_auth || i_auth_len != peer->curve_hash_len) {
		pr_debug("Missing or invalid Initiator Authenticating Tag\n");
		goto fail;
	}

	bufprintf("DPP: Received Initiator Authenticating Tag", (uint8_t *)i_auth, i_auth_len);
	/* I-auth' = H(R-nonce | I-nonce | PR.x | PI.x | BR.x | [BI.x |] 1) */
	if (dpp_gen_i_auth(peer, i_auth2) < 0) {
		pr_debug("%s: -- %d --\n", __func__, __LINE__);
		goto fail;
	}

	bufprintf("DPP: Calculated Initiator Authenticating Tag", i_auth2, i_auth_len);
	if (memcmp(i_auth, i_auth2, i_auth_len)) {
		pr_debug("Mismatching Initiator Authenticating Tag\n");
		goto fail;
	}

	memset(unwrapped, 0, unwrapped_len);
	free(unwrapped);
	unwrapped = NULL;

	pr_debug("DPP: Auth Success. Clear temporary keys\n");
	memset(peer->Mx, 0, peer->Mx_len);
	peer->Mx_len = 0;
	free(peer->Mx);
	peer->Mx = NULL;
	memset(peer->Nx, 0, peer->Nx_len);
	peer->Nx_len = 0;
	free(peer->Nx);
	peer->Nx = NULL;
	memset(peer->Lx, 0, peer->Lx_len);
	peer->Lx_len = 0;
	free(peer->Lx);
	peer->Lx = NULL;
	memset(peer->k1, 0, sizeof(peer->k1));
	memset(peer->k2, 0, sizeof(peer->k2));

	peer->auth_success = 1;

	return 0;
fail:
	memset(unwrapped, 0, unwrapped_len);
	free(unwrapped);
	unwrapped = NULL;
	return -1;
}

int dpp_build_auth_confirm(struct dpp_context *ctx,
			   uint8_t *peer_macaddr,
			   uint8_t **frame, size_t *framelen)
{
	uint8_t *msg, *msg_start;
	size_t msg_len;
	uint8_t i_auth[4 + DPP_MAX_HASH_LEN];
	size_t i_auth_len;
	uint8_t r_nonce[4 + DPP_MAX_NONCE_LEN];
	size_t r_nonce_len;
	const uint8_t *addr[2];
	size_t addrlen[2], attr_len;
	uint8_t *wrapped_i_auth;
	uint8_t *wrapped_r_nonce;
	uint8_t *attr_start, *attr_end;
	const uint8_t *r_pubkey_hash, *i_pubkey_hash;
	struct dpp_peer *peer;
	int ret;


	peer = dpp_lookup_peer(ctx, peer_macaddr);
	if (!peer)
		return -1;

	UNUSED(i_pubkey_hash);

	pr_debug("DPP: Build Authentication Confirmation\n");

	*frame = NULL;
	*framelen = 0;
	i_auth_len = 4 + peer->curve_hash_len;
	r_nonce_len = 4 + peer->curve_nonce_len;

	/* Build DPP Authentication Confirmation frame attributes */
	attr_len = 4 + 1 + 2 * (4 + SHA256_MAC_LEN) + 4 + i_auth_len +
			r_nonce_len + AES_BLOCK_SIZE;

	msg_len = attr_len + 8;

	msg = dpp_alloc_frame(DPP_PA_AUTHENTICATION_CONF, msg_len);
	if (!msg)
		goto fail;

	msg_start = msg;
	msg += 8;
	attr_start = msg;

	pr_debug("%s: msg-len = %zu   msg_start = %p   attr_start = %p\n",
		__func__, msg_len, msg_start, attr_start);

	r_pubkey_hash = peer->peer_bi->pubkey_hash;
	if (peer->own_bi)
		i_pubkey_hash = peer->own_bi->pubkey_hash;
	else
		i_pubkey_hash = NULL;

	/* DPP Status */
	bufptr_put_le16(msg, DPP_ATTR_STATUS);
	bufptr_put_le16(msg, 1);
	bufptr_put_u8(msg, peer->auth_status);

	/* Responder Bootstrapping Key Hash */
	if (r_pubkey_hash) {
		uint8_t *p = msg;
		size_t len = 0;

		bufptr_put_le16(msg, DPP_ATTR_R_BOOTSTRAP_KEY_HASH);
		bufptr_put_le16(msg, SHA256_MAC_LEN);
		bufptr_put(msg, r_pubkey_hash, SHA256_MAC_LEN);

		len += 2 + 2 + SHA256_MAC_LEN;
		bufprintf("R-bootstrap KeyHash", p, len);
	}

#if 0	//TODO:
	/* Initiator Bootstrapping Key Hash (mutual authentication) */
	dpp_build_attr_i_bootstrap_key_hash(msg, i_pubkey_hash);
#endif

	attr_end = msg;
	pr_debug("attr_end = %p (should match last msg)\n", attr_end);

	/* OUI, OUI type, Crypto Suite, DPP frame type */
	addr[0] = msg_start + 2;
	addrlen[0] = 3 + 1 + 1 + 1;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *)addr[0], addrlen[0]);

	/* Attributes before Wrapped Data */
	addr[1] = attr_start;
	addrlen[1] = attr_end - attr_start;
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *)addr[1], addrlen[1]);

	if (peer->auth_status == DPP_STATUS_OK) {
		/* I-auth wrapped with ke */
		bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
		bufptr_put_le16(msg, i_auth_len + AES_BLOCK_SIZE);
		wrapped_i_auth = msg;

		/* I-auth = H(R-nonce | I-nonce | PR.x | PI.x | BR.x | [BI.x |] 1) */
		buf_put_le16(i_auth, DPP_ATTR_I_AUTH_TAG);
		buf_put_le16(&i_auth[2], peer->curve_hash_len);
		if (dpp_gen_i_auth(peer, i_auth + 4) < 0)
			goto fail;

		ret = AES_SIV_ENCRYPT(peer->ke, peer->curve_hash_len,
				      i_auth, i_auth_len, 2, addr, addrlen,
				      wrapped_i_auth);
		if (ret)
			goto fail;

		bufprintf("DPP: {I-auth}ke", wrapped_i_auth, i_auth_len + AES_BLOCK_SIZE);
	} else {
		/* R-nonce wrapped with k2 */
		bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
		bufptr_put_le16(msg, r_nonce_len + AES_BLOCK_SIZE);
		wrapped_r_nonce = msg;

		buf_put_le16(r_nonce, DPP_ATTR_R_NONCE);
		buf_put_le16(&r_nonce[2], peer->curve_nonce_len);
		memcpy(r_nonce + 4, peer->r_nonce, peer->curve_nonce_len);

		ret = AES_SIV_ENCRYPT(peer->k2, peer->curve_hash_len,
				      r_nonce, r_nonce_len, 2, addr, addrlen,
				      wrapped_r_nonce);
		if (ret)
			goto fail;

		bufprintf("DPP: {R-nonce}k2", wrapped_r_nonce, r_nonce_len + AES_BLOCK_SIZE);
	}

	bufprintf("DPP: Authentication Confirmation frame attributes", msg_start, msg_len);
	peer->auth_confirm = msg_start;
	peer->auth_confirm_len = msg_len;

	if (peer->auth_status == DPP_STATUS_OK) {
		pr_debug("DPP: Auth Success. Clear temporary keys");
		memset(peer->Mx, 0, peer->Mx_len);
		peer->Mx_len = 0;
		free(peer->Mx);
		peer->Mx = NULL;
		memset(peer->Nx, 0, peer->Nx_len);
		peer->Nx_len = 0;
		free(peer->Nx);
		peer->Nx = NULL;
		memset(peer->Lx, 0, peer->Lx_len);
		peer->Lx_len = 0;
		free(peer->Lx);
		peer->Lx = NULL;
		memset(peer->k1, 0, sizeof(peer->k1));
		memset(peer->k2, 0, sizeof(peer->k2));

		peer->auth_success = 1;
	}

	*frame = msg_start;
	*framelen = msg_len;
	return 0;

fail:
	free(msg_start);
	return -1;

}

int dpp_process_auth_response(struct dpp_context *ctx,
			      uint8_t *src_macaddr,
			      uint8_t *frame, size_t framelen)
{
	uint16_t r_bootstrap_len, i_bootstrap_len, wrapped_data_len, status_len;
	const uint8_t *r_bootstrap, *i_bootstrap, *wrapped_data, *status;
	uint16_t r_proto_len, r_nonce_len, i_nonce_len, r_capab_len;
	const uint8_t *r_proto, *r_nonce, *i_nonce, *r_capab;
	uint8_t *unwrapped = NULL, *unwrapped2 = NULL;
	size_t unwrapped_len = 0, unwrapped2_len = 0;
	uint16_t wrapped2_len, r_auth_len;
	uint8_t r_auth2[DPP_MAX_HASH_LEN];
	const uint8_t *wrapped2, *r_auth;
	const uint8_t *hdr, *attr_start;
	const uint8_t *version;
	uint16_t version_len;
	const uint8_t *addr[2];
	struct dpp_peer *peer;
	size_t addrlen[2];
	size_t attr_len;
	uint8_t role;
	void *pr;
	int ret;


	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (!peer)
		return -1;

	hdr = frame + 2;
	attr_start = frame + WLAN_DPP_HDR_LEN;
	attr_len = framelen - WLAN_DPP_HDR_LEN;

	if (!peer->initiator || !peer->peer_bi || peer->reconfig) {
		//TODO
		pr_debug("%s: Unexpected Authentication Response\n", __func__);
		return -1;
	}

	peer->waiting_auth_resp = 0;

	wrapped_data = dpp_get_attr((uint8_t *)attr_start, attr_len,
				    DPP_ATTR_WRAPPED_DATA,
				    &wrapped_data_len);
	if (!wrapped_data || wrapped_data_len < AES_BLOCK_SIZE) {
		pr_debug("%s: Missing or invalid 'wrapped_data'\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	bufprintf("DPP: Wrapped data",  (uint8_t *)wrapped_data, wrapped_data_len);

	attr_len = wrapped_data - 4 - attr_start;

	r_bootstrap = dpp_get_attr((uint8_t *)attr_start, attr_len,
				   DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
				   &r_bootstrap_len);
	if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
		pr_debug("%s: Missing or invalid 'r_bootstrap'\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	bufprintf("DPP: Responder Bootstrapping Key Hash", (uint8_t *)r_bootstrap, r_bootstrap_len);
	if (memcmp(r_bootstrap, peer->peer_bi->pubkey_hash, SHA256_MAC_LEN)) {
		pr_debug("%s: r_bootstrap mismatch\n", __func__);
		bufprintf("DPP: Expected Responder Bootstrapping Key Hash",
			  peer->peer_bi->pubkey_hash, SHA256_MAC_LEN);

		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	i_bootstrap = dpp_get_attr((uint8_t *)attr_start, attr_len,
				   DPP_ATTR_I_BOOTSTRAP_KEY_HASH,
				   &i_bootstrap_len);
	if (i_bootstrap) {
		if (i_bootstrap_len != SHA256_MAC_LEN) {
			pr_debug("%s: Invalid 'i_bootstrap'\n", __func__);
			peer->auth_status = DPP_STATUS_AUTH_FAILURE;
			return -1;
		}

		bufprintf("DPP: Initiator Bootstrapping Key Hash",
			   (uint8_t *)i_bootstrap, i_bootstrap_len);

		if (!peer->own_bi ||
		    memcmp(i_bootstrap, peer->own_bi->pubkey_hash, SHA256_MAC_LEN) != 0) {
			pr_debug("%s: i_bootstrap mismatch\n", __func__);
			peer->auth_status = DPP_STATUS_AUTH_FAILURE;
			return -1;
		}
	}

	peer->peer_version = 1;
	version = dpp_get_attr((uint8_t *)attr_start, attr_len,
			       DPP_ATTR_PROTOCOL_VERSION,
			       &version_len);
	if (!version) {
		pr_debug("%s: 'version' missing!\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	if (version_len < 1 || version[0] == 0) {
		pr_debug("%s: Invalid 'version' %d\n", __func__, version[0]);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	peer->peer_version = version[0];
	pr_debug("DPP: Peer protocol version %u\n", peer->peer_version);

	status = dpp_get_attr((uint8_t *)attr_start, attr_len, DPP_ATTR_STATUS,
			      &status_len);
	if (!status || status_len < 1) {
		pr_debug("%s: Missing or invalid 'status'\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	pr_debug("DPP: Status %u\n", status[0]);
	peer->response_status = status[0];
	if (status[0] != DPP_STATUS_OK) {
		/* TODO: DPP_STATUS_RESPONSE_PENDING */
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	if (!i_bootstrap && peer->own_bi) {
		pr_debug("DPP: Responder decided not to use mutual authentication\n");
		dpp_bootstrap_free(peer->own_bi);
		peer->own_bi = NULL;
	}

	r_proto = dpp_get_attr((uint8_t *)attr_start, attr_len,
				DPP_ATTR_R_PROTOCOL_KEY,
				&r_proto_len);
	if (!r_proto) {
		pr_debug("%s: Missing 'r_protocol_key'\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	bufprintf("DPP: Responder Protocol Key\n", (uint8_t *)r_proto, r_proto_len);

	/* N = pI * PR */
	int ike_grp = ecc_key_group(peer->own_protocol_key);
	if (ike_grp < 0) {
		pr_debug("%s: Failed to get ike-group for own-protocol key!\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	//pr = dpp_set_pubkey_point(peer->own_protocol_key, r_proto, r_proto_len);
	pr = ecc_key_gen_from_xy("prime256v1", r_proto, r_proto + r_proto_len / 2, r_proto_len / 2);
	if (!pr) {
		pr_debug("%s: Invalid r_protocol_key\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		return -1;
	}

	ecc_key_print("Peer (Responder) Protocol Key", pr);

	if (ecc_ecdh(peer->own_protocol_key, pr, &peer->Nx, &peer->Nx_len) < 0) {
		pr_debug("%s: Failed to derive ECDH shared secret\n", __func__);
		goto fail;
	}

	ecc_key_free(peer->peer_protocol_key);
	peer->peer_protocol_key = pr;
	pr = NULL;

	bufprintf("DPP: ECDH shared secret (N.x)", peer->Nx, peer->Nx_len);

	if (dpp_derive_k2(peer->Nx, peer->Nx_len, peer->k2, peer->curve_hash_len) < 0)
		goto fail;

	addr[0] = hdr;
	addrlen[0] = DPP_HDR_LEN;
	addr[1] = attr_start;
	addrlen[1] = attr_len;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *)addr[0], addrlen[0]);
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *)addr[1], addrlen[1]);
	bufprintf("DPP: AES-SIV ciphertext", (uint8_t *)wrapped_data, wrapped_data_len);
	unwrapped_len = wrapped_data_len - AES_BLOCK_SIZE;
	unwrapped = calloc(1, unwrapped_len);
	if (!unwrapped)
		goto fail;

	ret = AES_SIV_DECRYPT(peer->k2, peer->curve_hash_len,
			      wrapped_data, wrapped_data_len,
			      2, addr, addrlen, unwrapped);
	if (ret) {
		pr_debug("%s: AES-SIV decryption failed\n", __func__);
		goto fail;
	}

	bufprintf("DPP: AES-SIV cleartext", (uint8_t *)unwrapped, unwrapped_len);

	if (dpp_check_attrs(unwrapped, unwrapped_len) < 0) {
		pr_debug("%s: Invalid attribute in unwrapped data\n", __func__);
		goto fail;
	}

	r_nonce = dpp_get_attr(unwrapped, unwrapped_len,
			       DPP_ATTR_R_NONCE,
			       &r_nonce_len);
	if (!r_nonce || r_nonce_len != peer->curve_nonce_len) {
		pr_debug("%s: Missing or invalid r_nonce\n", __func__);
		goto fail;
	}

	bufprintf("DPP: R-nonce", (uint8_t *)r_nonce, r_nonce_len);
	memcpy(peer->r_nonce, r_nonce, r_nonce_len);

	i_nonce = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_I_NONCE,
			       &i_nonce_len);
	if (!i_nonce || i_nonce_len != peer->curve_nonce_len)
		goto fail;

	bufprintf("DPP: I-nonce", (uint8_t *)i_nonce, i_nonce_len);
	if (memcmp(peer->i_nonce, i_nonce, i_nonce_len)) {
		pr_debug("%s: I-nonce mismatch\n", __func__);
		goto fail;
	}

#if 0	//TODO: /* Mutual authentication */
	if (peer->own_bi) {
		if (dpp_auth_derive_l_initiator(peer) < 0)
			goto fail;
	}
#endif

	r_capab = dpp_get_attr(unwrapped, unwrapped_len,
			       DPP_ATTR_R_CAPABILITIES,
			       &r_capab_len);
	if (!r_capab || r_capab_len < 1) {
		pr_debug("%s: Missing or invalid r_capabilities\n", __func__);
		goto fail;
	}

	peer->r_capab = r_capab[0];
	pr_debug("%s: DPP: R-capabilities: 0x%02x\n", __func__, peer->r_capab);
	role = peer->r_capab & DPP_CAPAB_ROLE_MASK;
	if (role != DPP_CAPAB_ENROLLEE) {
		pr_debug("DPP: Our role = Configurator; Peer role 0x%02x is incompatible\n", role);
		peer->auth_status = DPP_STATUS_NOT_COMPATIBLE;
		memset(unwrapped, 0, unwrapped_len);
		free(unwrapped);
		return -1;
	}

	wrapped2 = dpp_get_attr(unwrapped, unwrapped_len,
				DPP_ATTR_WRAPPED_DATA, &wrapped2_len);
	if (!wrapped2 || wrapped2_len < AES_BLOCK_SIZE) {
		pr_debug("%s: Missing or invalid Secondary Wrapped Data\n", __func__);
		goto fail;
	}

	bufprintf("DPP: AES-SIV ciphertext", (uint8_t *)wrapped2, wrapped2_len);

	if (dpp_derive_bk_ke(peer) < 0)
		goto fail;

	unwrapped2_len = wrapped2_len - AES_BLOCK_SIZE;
	unwrapped2 = calloc(1, unwrapped2_len);
	if (!unwrapped2)
		goto fail;

	ret = AES_SIV_DECRYPT(peer->ke, peer->curve_hash_len,
			      wrapped2, wrapped2_len,
			      0, NULL, NULL, unwrapped2);
	if (ret) {
		pr_debug("AES-SIV decryption failed\n");
		goto fail;
	}

	bufprintf("DPP: AES-SIV cleartext\n", unwrapped2, unwrapped2_len);

	if (dpp_check_attrs(unwrapped2, unwrapped2_len) < 0) {
		pr_debug("Invalid attribute in secondary unwrapped data\n");
		goto fail;
	}

	r_auth = dpp_get_attr(unwrapped2, unwrapped2_len, DPP_ATTR_R_AUTH_TAG,
			       &r_auth_len);
	if (!r_auth || r_auth_len != peer->curve_hash_len) {
		pr_debug("Missing or invalid Responder Authenticating Tag\n");
		goto fail;
	}

	bufprintf("DPP: Received Responder Authenticating Tag", (uint8_t *)r_auth, r_auth_len);
	/* R-auth' = H(I-nonce | R-nonce | PI.x | PR.x | [BI.x |] BR.x | 0) */
	if (dpp_gen_r_auth(peer, r_auth2, 1) < 0)
		goto fail;

	bufprintf("DPP: Calculated Responder Authenticating Tag", (uint8_t *)r_auth2, r_auth_len);
	if (memcmp(r_auth, r_auth2, r_auth_len) != 0) {
		pr_debug("%s: r_auth mismatch\n", __func__);
		peer->auth_status = DPP_STATUS_AUTH_FAILURE;
		memset(unwrapped, 0, unwrapped_len);
		memset(unwrapped2, 0, unwrapped2_len);
		free(unwrapped);
		free(unwrapped2);
		unwrapped = NULL;
		unwrapped2 = NULL;
		return -1;
	}

	memset(unwrapped, 0, unwrapped_len);
	memset(unwrapped2, 0, unwrapped2_len);
	free(unwrapped);
	free(unwrapped2);
	unwrapped = NULL;
	unwrapped2 = NULL;

	peer->auth_status = DPP_STATUS_OK;
	return 0;

fail:
	peer->auth_status = DPP_STATUS_AUTH_FAILURE;
	memset(unwrapped, 0, unwrapped_len);
	memset(unwrapped2, 0, unwrapped2_len);
	free(unwrapped);
	free(unwrapped2);
	unwrapped = NULL;
	unwrapped2 = NULL;
	ecc_key_free(pr);
	return -1;
}

int dpp_build_auth_response(struct dpp_context *ctx,
			    uint8_t *peer_macaddr,
			    uint8_t **frame, size_t *framelen)
{
#define DPP_AUTH_RESP_CLEAR_LEN		2 * (4 + DPP_MAX_NONCE_LEN) + 4 + 1 + \
					4 + 4 + DPP_MAX_HASH_LEN + AES_BLOCK_SIZE

	uint8_t wrapped_r_auth[4 + DPP_MAX_HASH_LEN + AES_BLOCK_SIZE], *w_r_auth;
	uint8_t wrapped_data[DPP_AUTH_RESP_CLEAR_LEN + AES_BLOCK_SIZE];
	const uint8_t *r_pubkey_hash, *i_pubkey_hash, *r_nonce, *i_nonce;
	uint8_t r_auth[4 + DPP_MAX_HASH_LEN];
	uint8_t clear[DPP_AUTH_RESP_CLEAR_LEN];
	uint8_t *attr_start, *attr_end, *pos;
	uint8_t *msg, *msg_start;
	size_t wrapped_r_auth_len;
	size_t siv_len, attr_len;
	const uint8_t *addr[2];
	struct dpp_peer *peer;
	size_t addrlen[2];
	uint8_t *pr = NULL;
	size_t pr_len = 0;
	size_t msg_len;
	int ret = -1;

	UNUSED(i_pubkey_hash);
	UNUSED(r_pubkey_hash);

	pr_debug("DPP: Build Authentication Response\n");
	*frame = NULL;
	*framelen = 0;

	peer = dpp_lookup_peer(ctx, peer_macaddr);
	if (!peer)
		return -1;

	if (peer->auth_status != DPP_STATUS_OK) {
		pr_debug("%s: DPP auth_status != OK\n", __func__); //TODO
		return -1;
	}

	if (!peer->own_bi)
		return -1;

	get_random_bytes(peer->curve_nonce_len, peer->r_nonce);
	bufprintf("DPP: R-nonce", peer->r_nonce, peer->curve_nonce_len);

	ecc_key_free(peer->own_protocol_key);
	ret = dpp_gen_key("P-256", &peer->own_protocol_key);
	if (ret)
		goto fail;

	ret = ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &pr, &pr_len);
	if (ret) {
		pr_debug("DPP: ecc_key_get_pubkey_point() failed\n");
		goto fail;
	}

	/* ECDH: N = pR * PI */
	if (ecc_ecdh(peer->own_protocol_key, peer->peer_protocol_key, &peer->Nx, &peer->Nx_len) < 0) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		goto fail;
	}

	bufprintf("DPP: ECDH shared secret (N.x)", peer->Nx, peer->Nx_len);

	if (dpp_derive_k2(peer->Nx, peer->Nx_len, peer->k2, peer->curve_hash_len) < 0) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		goto fail;
	}

#if 0	//TODO: check if peer_bi is valid in auth-req handler
	if (peer->own_bi && peer->peer_bi) {	//FIXME FIXME
		/* Mutual authentication */
		if (dpp_auth_derive_l_responder(peer) < 0)
			goto fail;
	}
#endif

	if (dpp_derive_bk_ke(peer) < 0) {
		goto fail;
	}

	/* R-auth = H(I-nonce | R-nonce | PI.x | PR.x | [BI.x |] BR.x | 0) */
	buf_put_le16(r_auth, DPP_ATTR_R_AUTH_TAG);
	buf_put_le16(&r_auth[2], peer->curve_hash_len);
	if (dpp_gen_r_auth(peer, r_auth + 4, 0) < 0) {
		goto fail;
	}

	ret = AES_SIV_ENCRYPT(peer->ke, peer->curve_hash_len,
			    r_auth, 4 + peer->curve_hash_len,
			    0, NULL, NULL, wrapped_r_auth);
	if (ret) {
		pr_debug("AES-SIV encryption failed\n");
		goto fail;
	}

	wrapped_r_auth_len = 4 + peer->curve_hash_len + AES_BLOCK_SIZE;
	bufprintf("DPP: {R-auth}ke", wrapped_r_auth, wrapped_r_auth_len);
	w_r_auth = wrapped_r_auth;

	r_pubkey_hash = peer->own_bi->pubkey_hash;
	i_pubkey_hash = NULL;	// no mutual auth

	i_nonce = peer->i_nonce;
	r_nonce = peer->r_nonce;

	peer->waiting_auth_confirm = 1;
	peer->response_status = peer->auth_status;
	//peer->peer_resp_tries = 0;

	/* Build DPP Authentication Response frame attributes */
	attr_len = 4 + 1 + 2 * (4 + SHA256_MAC_LEN) + 4 + (pr ? pr_len : 0) + 4 + sizeof(wrapped_data);
	attr_len += 5;	// for version 2
	msg_len = attr_len + 8;

	msg = dpp_alloc_frame(DPP_PA_AUTHENTICATION_RESP, attr_len);
	if (!msg) {
		ret = -1;
		goto fail;
	}

	msg_start = msg;
	msg += 8;
	attr_start = msg;

	pr_debug("%s: msg-len = %zu   msg_start = %p   attr_start = %p\n",
		__func__, msg_len, msg_start, attr_start);

	/* DPP Status */
	if (peer->auth_status != 255) {
		pr_debug("DPP: Status %d\n", peer->auth_status);
		bufptr_put_le16(msg, DPP_ATTR_STATUS);
		bufptr_put_le16(msg, 1);
		bufptr_put_u8(msg, peer->auth_status);
	}

	/* Responder Bootstrapping Key Hash */
	if (r_pubkey_hash) {
		uint8_t *p = msg;
		size_t alen = 0;

		bufptr_put_le16(msg, DPP_ATTR_R_BOOTSTRAP_KEY_HASH);
		bufptr_put_le16(msg, SHA256_MAC_LEN);
		bufptr_put(msg, r_pubkey_hash, SHA256_MAC_LEN);

		alen += 2 + 2 + SHA256_MAC_LEN;
		bufprintf("R-bootstrap KeyHash", p, alen);
	}

#if 0	//TODO:
	/* Initiator Bootstrapping Key Hash (mutual authentication) */
	dpp_build_attr_i_bootstrap_key_hash(msg, i_pubkey_hash);
#endif

	/* Responder Protocol Key */
	if (pr) {
		bufptr_put_le16(msg, DPP_ATTR_R_PROTOCOL_KEY);
		bufptr_put_le16(msg, pr_len);
		bufptr_put(msg, pr, pr_len);
	}

	/* Protocol Version */
	if (peer->peer_version >= 2) {
		bufptr_put_le16(msg, DPP_ATTR_PROTOCOL_VERSION);
		bufptr_put_le16(msg, 1);
		bufptr_put_u8(msg, DPP_VERSION);
	}

	attr_end = msg;

	/* Wrapped data ({R-nonce, I-nonce, R-capabilities, {R-auth}ke}k2) */
	pos = clear;
	if (r_nonce) {
		/* R-nonce */
		bufptr_put_le16(pos, DPP_ATTR_R_NONCE);
		bufptr_put_le16(pos, peer->curve_nonce_len);
		bufptr_put(pos, r_nonce, peer->curve_nonce_len);
	}

	if (i_nonce) {
		/* I-nonce */
		bufptr_put_le16(pos, DPP_ATTR_I_NONCE);
		bufptr_put_le16(pos, peer->curve_nonce_len);
		bufptr_put(pos, i_nonce, peer->curve_nonce_len);
	}

	/* R-capabilities */
	peer->r_capab = DPP_CAPAB_ENROLLEE;

	bufptr_put_le16(pos, DPP_ATTR_R_CAPABILITIES);
	bufptr_put_le16(pos, 1);
	bufptr_put_u8(pos, peer->r_capab);

	if (w_r_auth) {
		/* {R-auth}ke */
		bufptr_put_le16(pos, DPP_ATTR_WRAPPED_DATA);
		bufptr_put_le16(pos, wrapped_r_auth_len);
		bufptr_put(pos, w_r_auth, wrapped_r_auth_len);
	}

	/* OUI, OUI type, Crypto Suite, DPP frame type */
	addr[0] = msg_start + 2;
	addrlen[0] = 3 + 1 + 1 + 1;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *)addr[0], addrlen[0]);

	/* Attributes before Wrapped Data */
	addr[1] = attr_start;
	addrlen[1] = attr_end - attr_start;
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *)addr[1], addrlen[1]);

	siv_len = pos - clear;
	bufprintf("DPP: AES-SIV cleartext", clear, siv_len);
	ret = AES_SIV_ENCRYPT(peer->k2, peer->curve_hash_len, clear, siv_len,
				2, addr, addrlen, wrapped_data);
	if (ret) {
		free(msg);
		goto fail;
	}

	siv_len += AES_BLOCK_SIZE;
	bufprintf("DPP: AES-SIV ciphertext", wrapped_data, siv_len);

	bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
	bufptr_put_le16(msg, siv_len);
	bufptr_put(msg, wrapped_data, siv_len);

	bufprintf("DPP: Authentication Response frame attributes", msg_start, msg_len);

	if (peer->auth_response) {
		free(peer->auth_response);
		peer->auth_response = NULL;
	}

	peer->auth_response = msg_start;
	*frame = msg_start;
	*framelen = msg_len;

	ret = 0;
fail:
	free(pr);
	return ret;
}

int dpp_process_auth_request(struct dpp_context *ctx, uint8_t *src_macaddr,
			     uint8_t *frame, size_t framelen)
{
	uint8_t *hdr = (uint8_t *)frame + 2;	// 0x04 0x09 - PA-type, PA-vend-spec
	uint8_t *buf = (uint8_t *)frame + WLAN_DPP_HDR_LEN;
	size_t len = framelen - WLAN_DPP_HDR_LEN;
	uint16_t wrapped_data_len, i_proto_len, i_nonce_len;
	const uint8_t *wrapped_data, *i_proto, *i_nonce;
	uint16_t i_capab_len, i_bootstrap_len, r_bootstrap_len, channel_len;
	const uint8_t *i_capab, *i_bootstrap, *r_bootstrap, *channel;
	struct dpp_bootstrap_info *peer_bi = NULL;
	void *pi = NULL;
	struct dpp_peer *peer = NULL;
	uint8_t *unwrapped = NULL;
	size_t unwrapped_len = 0;
	const uint8_t *version;
	uint16_t version_len;
	uint8_t *attr_start;
	size_t attr_len;
	const uint8_t *addr[2];
	size_t addrlen[2];
	int ret;


	pr_debug("%s: from " MACFMT "\n", __func__, MAC2STR(src_macaddr));

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (peer && peer->state >= DPP_RESPONDER_STATE_AUTHENTICATING) {
		pr_debug("DPP_EVENT_FAIL: "
			"Already in DPP authentication exchange - ignore new one\n");
		return -1;
	}

	//dpp_chirp_stop(ctx);

	bufprintf("DPP Auth Request", buf, len);

	attr_start = buf;
	attr_len = len;
	r_bootstrap = dpp_get_attr(buf, len, DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
				   &r_bootstrap_len);
	if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
		pr_debug("%s: DPP_EVENT_FAIL: "
			"Missing or invalid Responder Bootstrapping Key Hash attribute\n",
			__func__);
		return -1;
	}

	bufprintf("DPP: Responder Bootstrapping Key Hash", (uint8_t *)r_bootstrap, r_bootstrap_len);

	i_bootstrap = dpp_get_attr(buf, len, DPP_ATTR_I_BOOTSTRAP_KEY_HASH,
				   &i_bootstrap_len);
	if (!i_bootstrap || i_bootstrap_len != SHA256_MAC_LEN) {
		pr_debug("%s: DPP_EVENT_FAIL: "
			"Missing or invalid Initiator Bootstrapping Key Hash attribute\n",
			__func__);
		return -1;
	}

	bufprintf("DPP: Initiator Bootstrapping Key Hash", (uint8_t *)i_bootstrap, i_bootstrap_len);

	//if (!dpp_is_peer_bootstrap_hash_known(ctx, i_bootstrap, src_macaddr)) {
	//	pr_debug("%s: DPP_EVENT_FAIL: "
	//		"Initiator bootstrap hash is NOT known. Can't do mutual auth\n",
	//		__func__);
	//	return -1;
	//}

	wrapped_data = dpp_get_attr(attr_start, attr_len,
				    DPP_ATTR_WRAPPED_DATA,
				    &wrapped_data_len);
	if (!wrapped_data || wrapped_data_len < AES_BLOCK_SIZE) {
		pr_debug("%s: DPP_EVENT_FAIL: "
			"Missing or invalid Wrapped Data attribute\n",
			__func__);
		return -1;
	}

	bufprintf("DPP: Wrapped Data", (uint8_t *)wrapped_data, wrapped_data_len);
	attr_len = wrapped_data - 4 - attr_start;

	if (!peer)
		peer = dpp_create_responder_instance(ctx);

	if (!peer)
		goto fail;

#if 0	//TODO
	if (peer_bi && peer_bi->configurator_params &&
	    dpp_set_configurator(peer, peer_bi->configurator_params) < 0)
		goto fail;
#endif

	//list_add_tail(&peer->list, &ctx->peerlist);
	ctx->num_peers++;

	peer->peer_bi = peer_bi;
	//peer->own_bi = own_bi;
	peer->curve_ike_grp = peer->own_bi->curve_ike_grp;
	//peer->curr_freq = freq;
	pr_debug("%s: Enrollee: peer = %p\n", __func__, peer);

	peer->peer_version = 1;
	version = dpp_get_attr(attr_start, attr_len, DPP_ATTR_PROTOCOL_VERSION,
			       &version_len);
	if (version && DPP_VERSION > 1) {
		if (version_len < 1 || version[0] == 0) {
			pr_debug("Invalid Protocol Version attribute\n");
			goto fail;
		}

		peer->peer_version = version[0];
		pr_debug("DPP: Peer protocol version %u", peer->peer_version);
	}

	if (peer->peer_version < 2) {
		pr_debug("DPP: version 2 and above supported\n");
		goto fail;
	}

	channel = dpp_get_attr(attr_start, attr_len, DPP_ATTR_CHANNEL,
			       &channel_len);
	if (channel) {
		//int neg_freq;

		if (channel_len < 2) {
			pr_debug("Too short Channel attribute\n");
			goto fail;
		}

		/* TODO
		neg_freq = ieee80211_chan_to_freq(NULL, channel[0], channel[1]);
		fprintf(stderr,
			"DPP: Initiator requested different channel for negotiation:"
			"op_class=%u channel=%u --> freq=%d",
			channel[0], channel[1], neg_freq);

		if (neg_freq < 0) {
			pr_debug("Unsupported Channel attribute value\n");
			goto fail;
		}

		if (peer->curr_freq != (unsigned int) neg_freq) {
			pr_debug("DPP: Changing negotiation channel from %u MHz to %u MHz",
				freq, neg_freq);

			peer->curr_freq = neg_freq;
		}
		*/
	}

	i_proto = dpp_get_attr(attr_start, attr_len, DPP_ATTR_I_PROTOCOL_KEY,
			       &i_proto_len);
	if (!i_proto) {
		pr_debug("Missing required Initiator Protocol Key attribute\n");
		goto fail;
	}

	bufprintf("DPP: Initiator Protocol Key", (uint8_t *)i_proto, i_proto_len);

	/* M = bR * PI */

	int ike_grp = ecc_key_group(peer->own_bi->pubkey);
	if (ike_grp < 0) {
		pr_debug("%s: Failed to get ike-group for own pubkey!\n", __func__);
		return -1;
	}

	pi = ecc_key_gen_from_xy("prime256v1", i_proto, i_proto + i_proto_len / 2, i_proto_len / 2);
	//pi = dpp_set_pubkey_point(peer->own_bi->pubkey, i_proto, i_proto_len);
	if (!pi) {
		pr_debug("%s: Invalid Initiator Protocol Key\n", __func__);
		goto fail;
	}

	ecc_key_print("Peer (Initiator) Protocol Key", pi);

	if (ecc_ecdh(peer->own_bi->key, pi, &peer->Mx, &peer->Mx_len) < 0) {
		pr_debug("Failed to derive Mx\n");
		goto fail;
	}

	bufprintf("DPP: ECDH shared secret (M.x)", peer->Mx, peer->Mx_len);

	if (dpp_derive_k1(peer->Mx, peer->Mx_len, peer->k1, peer->curve_hash_len) < 0)
		goto fail;

	addr[0] = hdr;
	addrlen[0] = DPP_HDR_LEN;
	addr[1] = attr_start;
	addrlen[1] = attr_len;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *)addr[0], addrlen[0]);
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *)addr[1], addrlen[1]);
	bufprintf("DPP: AES-SIV ciphertext", (uint8_t *)wrapped_data, wrapped_data_len);
	unwrapped_len = wrapped_data_len - AES_BLOCK_SIZE;
	unwrapped = calloc(1, unwrapped_len);
	if (!unwrapped)
		goto fail;

	ret = AES_SIV_DECRYPT(peer->k1, peer->curve_hash_len,
			      wrapped_data, wrapped_data_len,
			      2, addr, addrlen, unwrapped);
	if (ret) {
		pr_debug("AES-SIV decryption failed\n");
		goto fail;
	}

	bufprintf("DPP: AES-SIV cleartext", (uint8_t *)unwrapped, unwrapped_len);

	if (dpp_check_attrs(unwrapped, unwrapped_len) < 0) {
		pr_debug("Invalid attribute in unwrapped data\n");
		goto fail;
	}

	i_nonce = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_I_NONCE, &i_nonce_len);
	if (!i_nonce || i_nonce_len != peer->curve_nonce_len) {
		//dpp_send_auth_fail(peer, "Missing or invalid I-nonce"); //TODO
		pr_debug("Missing or invalid I-nonce\n");
		goto fail;
	}

	bufprintf("DPP: I-nonce", (uint8_t *)i_nonce, i_nonce_len);
	memcpy(peer->i_nonce, i_nonce, i_nonce_len);

	i_capab = dpp_get_attr(unwrapped, unwrapped_len,
			       DPP_ATTR_I_CAPABILITIES,
			       &i_capab_len);
	if (!i_capab || i_capab_len < 1) {
		//dpp_send_auth_fail(peer, "Missing or invalid I-capabilities"); //TODO
		pr_debug("Missing or invalid I-capabilities\n");
		goto fail;
	}

	peer->i_capab = i_capab[0];
	pr_debug("DPP: I-capabilities: 0x%02x\n", peer->i_capab);

	memset(unwrapped, 0, unwrapped_len);
	free(unwrapped);
	unwrapped = NULL;

	if ((peer->i_capab & DPP_CAPAB_ROLE_MASK) != DPP_CAPAB_CONFIGURATOR) {
		pr_debug("DPP: Unexpected or Incompatible role in I-capabilities\n");
		goto not_compatible;
	}

	peer->peer_protocol_key = pi;
	pi = NULL;
	peer->auth_status = DPP_STATUS_OK;

	return 0;

not_compatible:
	pr_debug("%s: DPP_EVENT_NOT_COMPATIBLE: i-capab=0x%02x\n",
		__func__, peer->i_capab);
	peer->peer_protocol_key = pi;
	pi = NULL;
	peer->auth_status = DPP_STATUS_NOT_COMPATIBLE;
fail:
	memset(unwrapped, 0, unwrapped_len);
	free(unwrapped);
	unwrapped = NULL;
	ecc_key_free(pi);
	dpp_free_peer(ctx, peer);

	return -1;
}

int dpp_build_auth_request(struct dpp_context *ctx,
			   uint8_t *peer_macaddr,
			   uint8_t **frame, size_t *framelen)
{
	uint8_t wrapped_data[4 + DPP_NONCE_LEN + 4 + 1 + AES_BLOCK_SIZE];
	const uint8_t *r_pubkey_hash, *i_pubkey_hash;
	uint8_t clear[4 + DPP_NONCE_LEN + 4 + 1];
	size_t len[2], siv_len, attr_len;
	uint8_t *attr_start, *attr_end;
	struct dpp_peer *peer = NULL;
	uint8_t *msg, *msg_start;
	const uint8_t *addr[2];
	uint8_t *pi = NULL;
	size_t pi_len = 0;
	size_t msg_len;
	uint8_t *pos;
	int ret;


	*frame = NULL;
	*framelen = 0;

	peer = dpp_lookup_peer(ctx, peer_macaddr);
	if (peer) {
		//TODO
	}

	if (!peer)
		peer = dpp_create_initiator_instance(ctx);

	if (!peer)
		return -1;

	memcpy(peer->peer_macaddr, peer_macaddr, 6);

	/* nonce */
	get_random_bytes(peer->curve_nonce_len, peer->i_nonce);

	dpp_gen_key("P-256", &peer->own_protocol_key);
	if (!peer->own_protocol_key) {
		pr_debug("Failed to generate protocol key\n");
		goto fail;
	}

	ret = ecc_key_get_pubkey_point(peer->own_protocol_key, 0, &pi, &pi_len);
	if (ret) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		goto fail;
	}

	/* ECDH: M = pI * BR */
	if (ecc_ecdh(peer->own_protocol_key, peer->peer_bi->key, &peer->Mx, &peer->Mx_len) < 0) {
		pr_debug("%s: %d\n", __func__, __LINE__);
		goto fail;
	}

	bufprintf("DPP: ECDH shared secret (M.x)", peer->Mx, peer->Mx_len);

	if (dpp_derive_k1(peer->Mx, peer->Mx_len, peer->k1, peer->curve_hash_len) < 0)
		goto fail;

	r_pubkey_hash = peer->peer_bi->pubkey_hash;
	i_pubkey_hash = peer->own_bi->pubkey_hash;

	/* DPP Auth Request attributes */
	attr_len = 2 * (4 + SHA256_MAC_LEN) + 4 + pi_len + 4 + sizeof(wrapped_data);

	attr_len += 5;		/* for DPP2 */
	msg_len = attr_len + 8;	/* for header */

	msg = dpp_alloc_frame(DPP_PA_AUTHENTICATION_REQ, msg_len);
	if (!msg)
		goto fail;

	msg_start = msg;
	msg += 8;
	attr_start = msg;

	pr_debug("%s: msg-len = %zu   msg_start = %p   attr_start = %p\n", __func__,
		msg_len, msg_start, attr_start);

	/* Responder Bootstrapping Key Hash */
	bufptr_put_le16(msg, DPP_ATTR_R_BOOTSTRAP_KEY_HASH);
	bufptr_put_le16(msg, SHA256_MAC_LEN);
	bufptr_put(msg, r_pubkey_hash, SHA256_MAC_LEN);

	/* Initiator Bootstrapping Key Hash */
	bufptr_put_le16(msg, DPP_ATTR_I_BOOTSTRAP_KEY_HASH);
	bufptr_put_le16(msg, SHA256_MAC_LEN);
	bufptr_put(msg, i_pubkey_hash, SHA256_MAC_LEN);

	/* Initiator Protocol Key */
	bufptr_put_le16(msg, DPP_ATTR_I_PROTOCOL_KEY);
	bufptr_put_le16(msg, pi_len);
	bufptr_put(msg, pi, pi_len);
#if 0
	/* Channel */
	if (freq > 0) {
		uint8_t op_class = 81, channel = 1;	//FIXME

		bufptr_put_le16(msg, DPP_ATTR_CHANNEL);
		bufptr_put_le16(msg, 2);
		bufptr_put_u8(msg, op_class);
		bufptr_put_u8(msg, channel);
	}
#endif

	/* Protocol Version */
	bufptr_put_le16(msg, DPP_ATTR_PROTOCOL_VERSION);
	bufptr_put_le16(msg, 1);
	bufptr_put_u8(msg, DPP_VERSION);

	/* Wrapped data ({I-nonce, I-capabilities}k1) */
	pos = clear;

	/* I-nonce */
	bufptr_put_le16(pos, DPP_ATTR_I_NONCE);
	bufptr_put_le16(pos, peer->curve_nonce_len);
	bufptr_put(pos, peer->i_nonce, peer->curve_nonce_len);

	/* I-capabilities */
	bufptr_put_le16(pos, DPP_ATTR_I_CAPABILITIES);
	bufptr_put_le16(pos, 1);
	bufptr_put_u8(pos, peer->i_capab);

	attr_end = msg;

	/* OUI, OUI type, Crypto Suite, DPP frame type */
	addr[0] = msg_start + 2;
	len[0] = 3 + 1 + 1 + 1;
	bufprintf("DDP: AES-SIV AD[0]", (uint8_t *)addr[0], len[0]);

	/* Attributes before Wrapped Data */
	addr[1] = attr_start;
	len[1] = attr_end - attr_start;
	bufprintf("DDP: AES-SIV AD[1]", (uint8_t *)addr[1], len[1]);

	siv_len = pos - clear;
	bufprintf("DPP: AES-SIV cleartext", clear, siv_len);

	ret = AES_SIV_ENCRYPT(peer->k1, peer->curve_hash_len,
			      clear, siv_len, 2, addr, len, wrapped_data);
	if (ret) {
		free(msg);
		goto fail;
	}

	siv_len += AES_BLOCK_SIZE;
	bufprintf("DPP: AES-SIV ciphertext", wrapped_data, siv_len);

	pr_debug("msg = %p  (copying wrapped data to)\n", msg);
	bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
	bufptr_put_le16(msg, siv_len);
	bufptr_put(msg, wrapped_data, siv_len);

	bufprintf("DPP: Authentication Request", msg_start, msg_len);
	*frame = msg_start;
	*framelen = msg_len;

fail:
	if (pi)
		free(pi);
	return 0;
}

int dpp_process_presence_announcement(struct dpp_context *ctx, uint8_t *src_macaddr,
				      uint8_t *frame, size_t framelen)
{
	//uint8_t *hdr = (uint8_t *)frame + 2;	// 0x04 0x09 - PA-type, PA-vend-spec
	uint8_t *buf = (uint8_t *)frame + WLAN_DPP_HDR_LEN;
	size_t len = framelen - WLAN_DPP_HDR_LEN;
	struct dpp_bootstrap_info *peer_bi = NULL;
	struct dpp_peer *peer = NULL;
	uint8_t *r_bootstrap;
	uint16_t r_bootstrap_len;
	int ret;


	pr_debug("%s: from " MACFMT "\n", __func__, MAC2STR(src_macaddr));
	bufprintf("DPP Presence Announcement", buf, len);

	r_bootstrap = dpp_get_attr(buf, len, DPP_ATTR_R_BOOTSTRAP_KEY_HASH,
				   &r_bootstrap_len);
	if (!r_bootstrap || r_bootstrap_len != SHA256_MAC_LEN) {
		pr_debug("%s: DPP_EVENT_FAIL: "
			"Missing or invalid Responder Bootstrapping Key Hash attribute\n",
			__func__);
		return -1;
	}

	bufprintf("DPP: Responder Bootstrapping Key Hash", (uint8_t *)r_bootstrap, r_bootstrap_len);

	peer_bi = dpp_lookup_chirp(ctx, r_bootstrap);
	if (!peer_bi) {
		pr_debug("%s: DPP_EVENT_FAIL: Responder bootstrap hash is NOT known.\n",
			__func__);
		return -1;
	}

	//TODO: override peer_bi->macaddr with this src_macaddr

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (peer) {
		pr_debug("%s: DPP FAIL: Already in process\n", __func__);
		return -1;
	}

	peer = dpp_create_initiator_instance(ctx);
	if (!peer)
		goto fail;

	memcpy(peer->peer_macaddr, src_macaddr, 6);
	peer->peer_bi = peer_bi;
	ret = dpp_gen_bootstrap_info("P-256", "81/1,115/36", NULL, NULL, &peer->own_bi);
	if (ret) {
		pr_debug("%s: Failed to gen bootstrap info for self\n", __func__);
		goto fail;
	}

	peer->peer_version = 2;
	peer->curve_ike_grp = peer->own_bi->curve_ike_grp;
	//peer->curr_freq = freq;

	//list_add_tail(&peer->list, &ctx->peerlist);
	ctx->num_peers++;

	/* move to Bootstraped state and feed Trigger event to initiate Auth with peer*/
	dpp_sm_set_state(peer->sm, DPP_INITIATOR_STATE_BOOTSTRAPPED);

	struct dpp_sm_event *ev = dpp_sm_create_event(ctx, src_macaddr, DPP_EVENT_TRIGGER, 0, NULL);

	if (ev) {
		dpp_trigger(ctx, src_macaddr, ev);
		dpp_sm_free_event(ev);
	}

	return 0;

fail:
	dpp_free_peer(ctx, peer);
	return -1;
}

int dpp_process_virt_presence_announcement(struct dpp_context *ctx,
					   uint8_t *src_macaddr,
					   uint8_t *hash, size_t hashlen)
{
	struct dpp_bootstrap_info *peer_bi = NULL;
	struct dpp_peer *peer;
	int ret;

	peer_bi = dpp_lookup_chirp(ctx, hash);
	if (!peer_bi) {
		pr_debug("%s: DPP_EVENT_FAIL: Responder bootstrap hash is NOT known.\n",
			__func__);
		return -1;
	}

	//TODO: override peer_bi->macaddr with this src_macaddr

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (peer) {
		pr_debug("%s: DPP FAIL: Already in process\n", __func__);
		return -1;
	}

	peer = dpp_create_initiator_instance(ctx);
	if (!peer)
		goto fail;

	memcpy(peer->peer_macaddr, src_macaddr, 6);
	peer->peer_bi = peer_bi;
	ret = dpp_gen_bootstrap_info("P-256", "81/1,115/36", NULL, NULL, &peer->own_bi);
	if (ret) {
		pr_debug("%s: Failed to gen bootstrap info for self\n", __func__);
		goto fail;
	}

	peer->peer_version = 2;
	peer->curve_ike_grp = peer->own_bi->curve_ike_grp;

	//list_add_tail(&peer->list, &ctx->peerlist);
	ctx->num_peers++;

	/* move to Bootstraped state and feed Trigger event to initiate Auth with peer*/
	dpp_sm_set_state(peer->sm, DPP_INITIATOR_STATE_BOOTSTRAPPED);

	struct dpp_sm_event *ev = dpp_sm_create_event(ctx, src_macaddr, DPP_EVENT_TRIGGER, 0, NULL);

	if (ev) {
		dpp_trigger(ctx, src_macaddr, ev);
		dpp_sm_free_event(ev);
	}


	return 0;

fail:
	dpp_free_peer(ctx, peer);
	return -1;
}

int dpp_build_presence_announcement(struct dpp_context *ctx,
				    uint8_t *peer_macaddr,
				    uint8_t **frame, size_t *framelen)
{
	const uint8_t *r_pubkey_hash;
	struct dpp_peer *peer = NULL;
	uint8_t *msg, *msg_start;
	uint8_t *attr_start;
	size_t attr_len;
	size_t msg_len;
	//int ret;


	*frame = NULL;
	*framelen = 0;

	/* peer_macaddr = ff:ff:ff:ff:ff:ff */
	peer = dpp_lookup_peer(ctx, peer_macaddr);
	if (!peer) {
		pr_debug("%s: No peer! Create using dpp_create_responder_instance()\n", __func__);
		return -1;
	}

	r_pubkey_hash = peer->own_bi->pubkey_hash_chirp;

	/* DPP Auth Request attributes */
	attr_len = 4 + SHA256_MAC_LEN;
	msg_len = attr_len + 8;	/* for header */

	msg = dpp_alloc_frame(DPP_PA_PRESENCE_ANNOUNCEMENT, msg_len);
	if (!msg)
		return -1;

	msg_start = msg;
	msg += 8;
	attr_start = msg;

	pr_debug("%s: msg-len = %zu   msg_start = %p   attr_start = %p\n", __func__,
		msg_len, msg_start, attr_start);

	/* Responder Bootstrapping Key Hash */
	bufptr_put_le16(msg, DPP_ATTR_R_BOOTSTRAP_KEY_HASH);
	bufptr_put_le16(msg, SHA256_MAC_LEN);
	bufptr_put(msg, r_pubkey_hash, SHA256_MAC_LEN);

	bufprintf("DPP: Presence Announcement", msg_start, msg_len);
	*frame = msg_start;
	*framelen = msg_len;

	return 0;
}

int dpp_del_conf_req_bstalist(struct dpp_context *ctx, struct dpp_peer *peer)
{
	int i;
	struct dpp_conf_req *req = &peer->e_req;

	for (i = 0; i < req->num_bsta; i++) {
		free(req->blist[i].chan_list);
	}

	req->num_bsta = 0;
	return 0;
}

int dpp_del_conf_req(struct dpp_context *ctx, struct dpp_peer *peer)
{
	struct dpp_conf_req *req = &peer->e_req;

	dpp_del_conf_req_bstalist(ctx, peer);
	memset(req, 0, sizeof(*req));
	return 0;
}

int dpp_add_req(struct dpp_context *ctx, struct dpp_peer *peer,
		     enum dpp_netrole netrole, enum dpp_akm akm,
		     char *chan_list)
{
	int idx = peer->e_req.num_bsta;
	struct dpp_conf_req *req = &peer->e_req;

	if (idx >= DPP_CONF_REQ_BSTA_MAX) {
		pr_debug("%s: at most %d bstas can be a part of request object\n",
			__func__, DPP_CONF_REQ_BSTA_MAX);
		return -1;
	}

	req->blist[idx].l2_netrole = netrole;
	req->blist[idx].akm = akm;
	req->blist[idx].chan_list = strdup(chan_list);
	return 0;
}

int dpp_add_bsta_req(struct dpp_context *ctx, struct dpp_peer *peer,
		     enum dpp_netrole netrole, enum dpp_akm akm,
		     char *chan_list)
{
	int idx = peer->e_req.num_bsta;
	struct dpp_conf_req *req = &peer->e_req;

	if (idx >= DPP_CONF_REQ_BSTA_MAX) {
		pr_debug("%s: at most %d bstas can be a part of request object\n",
			__func__, DPP_CONF_REQ_BSTA_MAX);
		return -1;
	}

	req->blist[idx].l2_netrole = netrole;
	req->blist[idx].akm = akm;
	req->blist[idx].chan_list = strdup(chan_list);
	return 0;
}

#ifdef DEBUG
struct dpp_conf_req global_req = {0};
#endif

int dpp_build_config_request(struct dpp_context *ctx,
			     uint8_t *src_macaddr,
			     uint8_t **frame, size_t *framelen)
{
	uint8_t *clear_start, *msg_start, *buf_start;
	size_t json_len, clear_len, msg_len, buflen;
	uint8_t *json, *clear, *msg, *buf;
	struct dpp_peer *peer;
	size_t len;
	uint8_t *wrapped;
	int ret, i;
	struct dpp_conf_req *req;

	*framelen = 0;
	*frame = NULL;

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (!peer) {
		pr_debug("%s: No peer " MACFMT " found\n", __func__, MAC2STR(src_macaddr));
		return -1;
	}


/* want to be able to pass multiple object requests */
#ifdef DEBUG	// testing with sample data
	req = &global_req;

	strcpy(global_req.tech, "map");
	strcpy(global_req.dpp_name, "test_map_dpp");
	global_req.name_len = strlen(global_req.dpp_name);
	global_req.netrole = DPP_NETROLE_MAPAGENT;
	global_req.num_bsta = 2;
	global_req.blist[0].l2_netrole = DPP_NETROLE_MAP_BH_STA;
	global_req.blist[0].akm = DPP_AKM_SAE_DPP;
	global_req.blist[0].chan_list = strdup("115/36");
	global_req.blist[1].l2_netrole = DPP_NETROLE_MAP_BH_STA;
	global_req.blist[1].akm = DPP_AKM_SAE_DPP;
	global_req.blist[1].chan_list = strdup("81/1");
#else
	req = &peer->e_req;
#endif


	len = 100 + req->name_len * 6 + (req->num_bsta * 33 /* arbitrary */) + 1 /* + num_opclasses * 4 */;

	/*
	if (mud_url && mud_url[0])
		len += 10 + strlen(mud_url);
	*/

	if (req->extra_name && req->extra_value && req->extra_name[0] && req->extra_value[0])
		len += 10 + strlen(req->extra_name) + strlen(req->extra_value);


	json = calloc(1, len * sizeof(char));
	if (!json) {
		return -1;
	}

	json_start_object(json, NULL);
	if (json_add_string_escape(json, "name", req->dpp_name, req->name_len) < 0) {
		free(json);
		return -1;
	}

	json_value_sep(json);
	json_add_string(json, "wi-fi_tech", req->tech);
	json_value_sep(json);
	json_add_string(json, "netRole", dpp_netrole_str(req->netrole));
	json_value_sep(json);
	json_start_array(json, "bSTAList"); /* TODO: could be a list that is iterated, and if empty dont pass */
	if (req->netrole == DPP_NETROLE_MAPAGENT) { /* TODO: may not be desired to have additional list */
		for (i = 0; i < req->num_bsta; i++) {
			if (i > 0)
				json_value_sep(json); /* one object already added */

			json_start_object(json, NULL);
			json_add_string(json, "netRole", dpp_netrole_str(req->blist[i].l2_netrole));
			json_value_sep(json);
			json_add_string(json, "akm", dpp_akm_str(req->blist[i].akm));
			json_value_sep(json);
			if (req->blist[i].chan_list) {
				json_add_string(json, "channelList", req->blist[i].chan_list);
			}
			json_end_object(json);
		}
	}

	json_end_array(json);
	/*
	if (mud_url && mud_url[0]) {
		json_value_sep(json);
		json_add_string(json, "mudurl", mud_url);
	}

	if (opclasses && num_opclasses) {
		int i;

		json_value_sep(json);
		json_start_array(json, "bandSupport");
		for (i = 0; opclasses[i] && i < num_opclasses; i++)
			sprintf(json + strlen(json), "%s%u", i ? "," : "", opclasses[i]);

		json_end_array(json);
	}
	*/

	//if (extra_name && extra_value && extra_name[0] && extra_value[0]) {
	//	json_value_sep(json);
	//	sprintf((char *)json + strlen((char *)json), "\"%s\":%s", extra_name, extra_value);
	//}
	json_end_object(json);

	pr_debug("%s: json= %s\n", __func__, json);
	pr_debug("DPP: Build configuration request attributes\n");

	get_random_bytes(peer->curve_nonce_len, peer->e_nonce);
	bufprintf("DPP: E-nonce", peer->e_nonce, peer->curve_nonce_len);
	json_len = strlen((char *)json);
	bufprintf("DPP: configRequest JSON", json, json_len);

	/* { E-nonce, configAttrib }ke */
	clear_len = 4 + peer->curve_nonce_len + 4 + json_len;
	clear = calloc(1, clear_len);

	msg_len = 4 + clear_len + AES_BLOCK_SIZE;
	msg = calloc(1, msg_len);
	if (!clear || !msg) {
		free(clear);
		free(json);
		return -1;
	}

	clear_start = clear;
	msg_start = msg;

	pr_debug("%s: -- %d--\n", __func__, __LINE__);
	/* E-nonce */
	bufptr_put_le16(clear, DPP_ATTR_ENROLLEE_NONCE);
	bufptr_put_le16(clear, peer->curve_nonce_len);
	bufptr_put(clear, peer->e_nonce, peer->curve_nonce_len);

	/* configAttrib */
	bufptr_put_le16(clear, DPP_ATTR_CONFIG_ATTR_OBJ);
	bufptr_put_le16(clear, json_len);
	bufptr_put(clear, json, json_len);

	bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
	bufptr_put_le16(msg, clear_len + AES_BLOCK_SIZE);
	wrapped = msg;		//TODO: check

	pr_debug("%s: -- %d-- clearlen = %zu\n", __func__, __LINE__, clear_len);
	/* No AES-SIV AD */
	bufprintf("DPP: AES-SIV cleartext", clear_start, clear_len);
	ret = AES_SIV_ENCRYPT(peer->ke, peer->curve_hash_len,
			      clear_start, clear_len, 0, NULL, NULL, wrapped);
	if (ret) {
		pr_debug("%s: AES-SIV encrypt failed\n", __func__);
		free(msg_start);
		free(clear_start);
		free(json);
		return -1;
	}

	bufprintf("DPP: AES-SIV ciphertext", wrapped, clear_len + AES_BLOCK_SIZE);

	bufprintf("DPP: Configuration Request frame attributes", msg_start, msg_len);

	pr_debug("%s: -- %d-- attributes len = %zu\n", __func__, __LINE__, msg_len);

	/* build gas frame */
	buflen = 3 + 10 + 2 + msg_len;
	buflen += 128;

	buf = calloc(1, buflen);
	if (!buf) {
		free(msg_start);
		free(clear_start);
		free(json);
		return -1;
	}

	/* prepare GAS query containing DPP-config-request */
	buf_start = buf;
	bufptr_put_u8(buf, WLAN_ACTION_PUBLIC);
	bufptr_put_u8(buf, WLAN_PA_GAS_INITIAL_REQ);
	bufptr_put_u8(buf, 0);	/* dialog token */

	/* Advertisement Protocol IE */
	bufptr_put_u8(buf, WLAN_EID_ADV_PROTO);
	bufptr_put_u8(buf, 8); /* Length */
	bufptr_put_u8(buf, 0x7f);
	bufptr_put_u8(buf, WLAN_EID_VENDOR_SPECIFIC);
	bufptr_put_u8(buf, 5);
	bufptr_put(buf, "\x50\x6f\x9a", 3);
	bufptr_put_u8(buf, DPP_OUI_TYPE);
	bufptr_put_u8(buf, 0x01);

	bufptr_put_le16(buf, msg_len);
	bufptr_put(buf, msg_start, msg_len);

	free(msg_start);
	free(clear_start);
	free(json);

	*frame = buf_start;
	*framelen = buflen;
	bufprintf("DPP: GAS Config Request", *frame, *framelen);

	return 0;
}

int dpp_process_config_request(struct dpp_context *ctx,
			       uint8_t *src_macaddr,
			       uint8_t *frame, size_t framelen)
{
	const uint8_t *wrapped_data, *e_nonce, *config_attr;
	uint16_t wrapped_data_len = 0, e_nonce_len, config_attr_len;
	struct json_token *root = NULL, *token;
	uint8_t *unwrapped = NULL;
	size_t unwrapped_len = 0;
	struct dpp_peer *peer;
	uint8_t *attr_start;
	size_t attr_len;
	int ret = -1;

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (!peer) {
		pr_debug("%s: No peer " MACFMT " found\n", __func__, MAC2STR(src_macaddr));
		return -1;
	}

	bufprintf("DPP: Process Config Request", frame, framelen);
	/* TODO: validate received GAS-query header */
	if (framelen < 15)
		return -1;

	attr_start = frame + 15;	 /* PA-GAS-initial-req */
	attr_len = framelen - 15;

	pr_debug("%s: %d: attr_len = %zu\n", __func__, __LINE__, attr_len);
	bufprintf("DPP: TODO Process Config attrs", attr_start, attr_len);
	if (dpp_check_attrs(attr_start, attr_len) < 0) {
		pr_debug("Invalid attribute in config request\n");
		return -1;
	}

	wrapped_data = dpp_get_attr(attr_start, attr_len, DPP_ATTR_WRAPPED_DATA, &wrapped_data_len);
	if (!wrapped_data || wrapped_data_len < AES_BLOCK_SIZE) {
		pr_debug("Missing or invalid Wrapped Data attribute (len = %d)\n",
			wrapped_data_len);
		return -1;
	}

	bufprintf("DPP: AES-SIV ciphertext", (uint8_t *) wrapped_data, wrapped_data_len);
	unwrapped_len = wrapped_data_len - AES_BLOCK_SIZE;
	unwrapped = calloc(1, unwrapped_len);
	if (!unwrapped)
		return -1;

	ret = AES_SIV_DECRYPT(peer->ke, peer->curve_hash_len,
			      wrapped_data, wrapped_data_len, 0, NULL, NULL,
			      unwrapped);
	if (ret) {
		pr_debug("AES-SIV decryption failed\n");
		ret = -1;
		goto out;
	}

	bufprintf("DPP: AES-SIV cleartext", unwrapped, unwrapped_len);
	if (dpp_check_attrs(unwrapped, unwrapped_len) < 0) {
		pr_debug("Invalid attribute in unwrapped data\n");
		goto out;
	}

	e_nonce = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_ENROLLEE_NONCE, &e_nonce_len);
	if (!e_nonce || e_nonce_len != peer->curve_nonce_len) {
		pr_debug("Missing or invalid Enrollee Nonce attribute\n");
		goto out;
	}

	bufprintf("DPP: Enrollee Nonce", (uint8_t *) e_nonce, e_nonce_len);
	memcpy(peer->e_nonce, e_nonce, e_nonce_len);

	//TODO: dpp_attr_for_each(attr, unwrapped, unwrapped_len) {
	//		if (attr->type == DPP_ATTR_CONFIG_ATTR_OBJ) {
	//			//handle this config-obj request
	//			// check if we support this netrole request
	//			// put list of netroles in peer struct
	//		}
	//	}
	config_attr = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_CONFIG_ATTR_OBJ, &config_attr_len);
	if (!config_attr) {
		pr_debug("Missing or invalid Config Attributes attribute\n");
		goto out;
	}

	bufprintf("DPP: Config Attributes", (uint8_t *) config_attr, config_attr_len);

	root = json_parse((const char *)config_attr, config_attr_len);
	if (!root) {
		pr_debug("Could not parse Config Attributes\n");
		goto out;
	}

	token = json_get_member(root, "name");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No Config Attributes - name\n");
		goto out;
	}

	pr_debug("DPP: Enrollee name = '%s'\n", token->string);
	free(peer->e_name);
	peer->e_name = strdup(token->string);

	token = json_get_member(root, "wi-fi_tech");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No Config Attributes - wi-fi_tech\n");
		goto out;
	}

	pr_debug("DPP: wi-fi_tech = '%s'\n", token->string);
	if (strcmp(token->string, "map") == 0) {
		if (dpp_conf_req_rx_map(ctx, peer, root) < 0) {
			pr_debug("DPP: MAP configuration "
				"request parsing failed\n");
			goto out;
		}

		ret = 0;
		goto out;
	} else if (strcmp(token->string, "infra") != 0) {
		pr_debug("DPP: Unsupported wi-fi_tech '%s'\n", token->string);
		goto out;
	}

	token = json_get_member(root, "netRole");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No Config Attributes - netRole\n");
		goto out;
	}

	pr_debug("DPP: netRole = '%s'\n", token->string);
	peer->e_req.netrole = dpp_netrole_from_str(token->string);
	if (peer->e_req.netrole == DPP_NETROLE_INVALID) {
		pr_debug("DPP: Unsupported netRole '%s'\n", token->string);
		pr_debug("Unsupported netRole\n");
		goto out;
	}

	token = json_get_member(root, "mudurl");	//TODO
	if (token && token->type == JSON_STRING) {
		pr_debug("DPP: mudurl = '%s'\n", token->string);
		free(peer->e_mud_url);
		peer->e_mud_url = strdup(token->string);
	}

#if 0	//TODO
	token = json_get_member(root, "bandSupport");
	if (token && token->type == JSON_ARRAY) {
		int *opclass = NULL;
		char txt[200], *pos, *end;
		int i, res;

		pr_debug("DPP: bandSupport\n");
		token = token->child;
		while (token) {
			if (token->type != JSON_NUMBER) {
				pr_debug("DPP: Invalid bandSupport array member type\n");
			} else {
				pr_debug("DPP: Supported global operating class: %d", token->number);
				int_array_add_unique(&opclass, token->number);
			}
			token = token->sibling;
		}

		txt[0] = '\0';
		pos = txt;
		end = txt + sizeof(txt);
		for (i = 0; opclass && opclass[i]; i++) {
			res = snprintf(pos, end - pos, "%s%d",
					  pos == txt ? "" : ",", opclass[i]);
			if (snprintf_error(end - pos, res)) {
				*pos = '\0';
				break;
			}
			pos += res;
		}
		free(peer->e_band_support);
		peer->e_band_support = opclass;
	}
#endif
out:
	if (unwrapped)
		free(unwrapped);
	if (root)
		json_free(root);
	return ret;
}

int dpp_build_config_response(struct dpp_context *ctx,
			      uint8_t *src_macaddr,
			      uint8_t **frame, size_t *framelen)
{
	size_t clear_len, attr_len, conf_len, conf2_len[3] = {0}, msglen;
	uint8_t *clear = NULL, *msg = NULL;
	uint8_t *clear_start, *msg_start;
	struct dpp_peer *peer;
	const uint8_t *addr[1];
	uint8_t *conf, *conf2[3] = {0};
	uint8_t *wrapped;
	size_t len[1];
	int ret, i;
	struct dpp_conf_req *req;

	*frame = NULL;
	*framelen = 0;

	peer = dpp_lookup_peer(ctx, src_macaddr);
	if (!peer) {
		pr_debug("%s: No peer " MACFMT " found\n", __func__, MAC2STR(src_macaddr));
		return -1;
	}

	req = &peer->e_req;

	if (req->netrole == DPP_NETROLE_CONFIGURATOR)
		return -1;	/* Not supported */

	conf = dpp_build_conf_obj(ctx, peer, req->netrole, DPP_CONFIG_TYPE_1905, BAND_UNKNOWN, &conf_len);
	if (!conf) {
		pr_debug("%s: Failed to build configuration object 1905\n", __func__);
		peer->conf_status = DPP_STATUS_CONFIGURE_FAILURE;
		return -1;
	}

	for (i = 0; i < req->num_bsta; i++) {
		enum wifi_band band = dpp_chan_list_to_band(req->blist[i].chan_list);

		conf2[i] = dpp_build_conf_obj(ctx, peer, req->blist[i].l2_netrole, DPP_CONFIG_TYPE_BSTA, band, &conf2_len[i]);
		if (!conf2[i]) {
			pr_debug("%s: Failed to build configuration object bsta\n", __func__);
			peer->conf_status = DPP_STATUS_CONFIGURE_FAILURE;
			goto error;
		}
	}

	bufprintf("DPP: configurationObject JSON", conf, conf_len);
	peer->conf_status = DPP_STATUS_OK;

	/* { E-nonce, configurationObject[, sendConnStatus]}ke */
	clear_len = 4 + peer->curve_nonce_len;
	if (conf)
		clear_len += 4 + conf_len;

	for (i = 0; i < req->num_bsta; i++)
		clear_len += 4 + conf2_len[i];

	if (peer->peer_version >= 2 && peer->send_conn_status && req->netrole == DPP_NETROLE_STA)
		clear_len += 4;

	clear = calloc(1, clear_len);
	if (!clear)
		return -1;

	attr_len = 4 + 1 + 4 + clear_len + AES_BLOCK_SIZE;
	msglen = 19 + attr_len;

	msg = calloc(1, msglen);
	if (!msg) {
		free(clear);
		return -1;
	}

	msg_start = msg;
	clear_start = clear;

	bufptr_put_u8(msg, WLAN_ACTION_PUBLIC);
	bufptr_put_u8(msg, WLAN_PA_GAS_INITIAL_RESP);
	bufptr_put_u8(msg, 0); /* TODO: ? */
	bufptr_put_le16(msg, WLAN_STATUS_SUCCESS);
	bufptr_put_le16(msg, 0);

	bufptr_put_u8(msg, WLAN_EID_ADV_PROTO);
	bufptr_put_u8(msg, 8); /* Length */
	bufptr_put_u8(msg, 0x7f);
	bufptr_put_u8(msg, WLAN_EID_VENDOR_SPECIFIC);
	bufptr_put_u8(msg, 5);
	bufptr_put(msg, "\x50\x6f\x9a", 3);
	bufptr_put_u8(msg, DPP_OUI_TYPE);
	bufptr_put_u8(msg, 0x01);

	bufptr_put_le16(msg, attr_len);

	/* E-nonce */
	bufptr_put_le16(clear, DPP_ATTR_ENROLLEE_NONCE);
	bufptr_put_le16(clear, peer->curve_nonce_len);
	bufptr_put(clear, peer->e_nonce, peer->curve_nonce_len);

	if (conf) {
		bufptr_put_le16(clear, DPP_ATTR_CONFIG_OBJ);
		bufptr_put_le16(clear, conf_len);
		bufptr_put(clear, conf, conf_len);
	}

	if (peer->peer_version >= 2 && conf2[0]) {
		for (i = 0; i < req->num_bsta; i++) {
			bufptr_put_le16(clear, DPP_ATTR_CONFIG_OBJ);
			bufptr_put_le16(clear, conf2_len[i]);
			bufptr_put(clear, conf2[i], conf2_len[i]);
		}
	}

	if (peer->peer_version >= 2 && peer->send_conn_status &&
	    req->netrole == DPP_NETROLE_STA &&
	    peer->conf_status == DPP_STATUS_OK) {
		pr_debug("DPP: sendConnStatus\n");
		bufptr_put_le16(clear, DPP_ATTR_SEND_CONN_STATUS);
		bufptr_put_le16(clear, 0);
	}

	/* DPP Status */
	pr_debug("DPP: Status %d\n", peer->conf_status);
	bufptr_put_le16(msg, DPP_ATTR_STATUS);
	bufptr_put_le16(msg, 1);
	bufptr_put_u8(msg, peer->conf_status);


	addr[0] = (msg_start + 19);
	len[0] = 5;
	bufprintf("DDP: AES-SIV AD", (uint8_t *) addr[0], len[0]);

	bufptr_put_le16(msg, DPP_ATTR_WRAPPED_DATA);
	bufptr_put_le16(msg, clear_len + AES_BLOCK_SIZE);
	wrapped = msg;

	bufprintf("DPP: AES-SIV cleartext", clear_start, clear_len);
	ret = AES_SIV_ENCRYPT(peer->ke, peer->curve_hash_len,
			      clear_start, clear_len,
			      1, addr, len, wrapped);
	if (ret) {
		pr_debug("%s: AES-SIV encrypt failed\n", __func__);
		free(clear_start);
		free(msg_start);
		return -1;
	}

	bufprintf("DPP: AES-SIV ciphertext", wrapped, clear_len + AES_BLOCK_SIZE);
	//bufprintf("DPP: Configuration Response attributes", msg_start, attr_len);

	memset(conf, 0, conf_len);
	free(conf);
	for (i = 0; i < req->num_bsta; i++) {
		memset(conf2[i], 0, conf2_len[i]);
		free(conf2[i]);
	}
	memset(clear_start, 0, clear_len);
	free(clear_start);

	*framelen = msglen;
	*frame = msg_start;

	return 0;
error:
	/* TODO: error handling!*/
	return -1;
}

static uint8_t * dpp_get_csr_attrs(uint8_t *attrs, size_t attrs_len, size_t *len)
{
	uint8_t *b64;
	uint16_t b64_len;
	uint8_t *data;
	*len = 258;

	data = calloc(1, 258);
	if (!data)
		return NULL;

	b64 = dpp_get_attr(attrs, attrs_len, DPP_ATTR_CSR_ATTR_REQ, &b64_len);
	if (!b64) {
		free(data);
		return NULL;
	}
#if defined(RTCONFIG_SWRTMESH)
	easy_base64_decode((const unsigned char *) b64, b64_len, data, len);
#else
	base64_decode((const unsigned char *) b64, b64_len, data, len);
#endif

	return data;
}

struct dpp_config_obj *dpp_config_obj_init(struct dpp_peer *peer)
{
	struct dpp_config_obj *conf;

	conf = calloc(1, sizeof(*conf));
	if (!conf) {
		pr_debug("DPP: Failure allocating dpp_config_obj\n");
		return NULL;
	}

	list_add_tail(&conf->list, &peer->conf_objlist);
	return conf;
}

struct dpp_config_obj *dpp_config_obj_get_band(struct dpp_peer *peer,
					       enum dpp_netrole netrole,
					       enum wifi_band band)
{
	struct dpp_config_obj *conf = NULL;

	list_for_each_entry(conf, &peer->conf_objlist, list) {
		if (conf->netrole == netrole)
			return conf;

	}

	return NULL;
}

struct dpp_config_obj *dpp_config_obj_get_netrole(struct dpp_peer *peer,
					  enum dpp_netrole netrole, int idx)
{
	struct dpp_config_obj *conf = NULL;
	int i = 0;

	list_for_each_entry(conf, &peer->conf_objlist, list) {
		if (conf->netrole == netrole) {
			if (idx == i++)
				return conf;
		}
	}

	return NULL;
}

struct dpp_config_obj *dpp_config_obj_get_akm(struct dpp_peer *peer,
					  enum dpp_akm akm)
{
	struct dpp_config_obj *conf = NULL;

	list_for_each_entry(conf, &peer->conf_objlist, list) {
		if (conf->akm == akm)
			return conf;
	}

	return NULL;
}

void dpp_config_obj_free(struct dpp_config_obj *obj)
{
	if (!obj)
		return;

	list_del(&obj->list);
	free(obj);
}

static int dpp_parse_cred_legacy(struct dpp_config_obj *conf,
				 struct json_token *cred)
{
	struct json_token *pass, *psk_hex;

	pr_debug("DPP: Legacy akm=psk credential\n");

	pass = json_get_member(cred, "pass");
	psk_hex = json_get_member(cred, "psk_hex");

	if (pass && pass->type == JSON_STRING) {
		size_t len = strlen(pass->string);

		if (len < 8 || len > 63)
			return -1;
		strncpy(conf->passphrase, pass->string,
			   sizeof(conf->passphrase));
	} else if (psk_hex && psk_hex->type == JSON_STRING) {
		if (dpp_akm_sae(conf->akm) && !dpp_akm_psk(conf->akm)) {
			pr_debug("DPP: Unexpected psk_hex with akm=sae\n");
			return -1;
		}
		if (strlen(psk_hex->string) != PMKID_LEN * 2 ||
		    strtob(psk_hex->string, PMKID_LEN, conf->psk) < 0) {
			pr_debug("DPP: Invalid psk_hex encoding\n");
			return -1;
		}
		bufprintf("DPP: Legacy PSK", conf->psk, PMKID_LEN);
		conf->psk_set = 1;
	} else {
		pr_debug("DPP: No pass or psk_hex strings found\n");
		return -1;
	}

	if (dpp_akm_sae(conf->akm) && !conf->passphrase[0]) {
		pr_debug("DPP: No pass for sae found\n");
		return -1;
	}

	return 0;
}

struct crypto_ec_key *dpp_parse_jwk(struct json_token *jwk)
{
	//const struct dpp_curve_params *curve;
	struct crypto_ec_key *key = NULL;
	uint8_t *x = NULL, *y = NULL;
	struct json_token *token;
	size_t x_len, y_len;


	token = json_get_member(jwk, "kty");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: No kty in JWK\n");
		return NULL;
	}

	if (strcmp(token->string, "EC")) {
		pr_debug("DPP: Unexpected JWK kty '%s'\n", token->string);
		return NULL;
	}

	token = json_get_member(jwk, "crv");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: No crv in JWK\n");
		return NULL;
	}

	if (strcmp(token->string, "P-256")) {
		pr_debug("DPP: Unexpected JWK crv '%s'\n", token->string);
		return NULL;
	}

	/*
	curve = dpp_get_curve_jwk_crv(token->string);
	if (!curve) {
		pr_debug("DPP: Unsupported JWK crv '%s'\n", token->string);
		return NULL;
	}
	*/

	token = json_get_member(jwk, "x");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: Invalid x in JWK\n");
		return NULL;
	}

	x = calloc(1, strlen(token->string) * 3 / 4);
	if (!x) {
		pr_debug("|%s:%d| DPP: Error allocating x!\n", __func__, __LINE__);
		return NULL;
	}

	pr_debug("|%s:%d| token:%s len:%zu\n", __func__, __LINE__, token->string, strlen(token->string));

	x_len = base64url_decode(x, (unsigned char *)token->string, strlen(token->string));
	if (x_len == 0) {
		pr_debug("DPP: No x in JWK\n");
		return NULL;
	}

	bufprintf("DPP: JWK x", x, x_len);

	if (x_len != 32 /* curve->prime_len */) {
		pr_debug("DPP: Invalid x length %zu in JWK\n", x_len);
		goto free_x;
	}

	token = json_get_member(jwk, "y");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: Invalid y in JWK\n");
		goto free_x;
	}

	y = calloc(1, strlen(token->string) * 3 / 4);
	if (!y) {
		pr_debug("|%s:%d| DPP: Error allocating y!\n", __func__, __LINE__);
		return NULL;
	}

	y_len = base64url_decode(y, (unsigned char *) token->string, strlen(token->string));
	if (!y_len) {
		pr_debug("DPP: No y in JWK\n");
		goto free_y;
	}

	bufprintf("DPP: JWK y", y, y_len);

	if (y_len != 32 /* curve->prime_len */) {
		pr_debug("DPP: Invalid y length %zu in JWK\n", y_len);
		goto free_y;
	}

	key = ecc_key_gen_from_xy("prime256v1", x, y, x_len);
free_y:
	free(y);
free_x:
	free(x);
	return key;
}

uint8_t json_get_member_base64url(struct json_token *json,
					  const char *name, uint8_t **buf)
{
	struct json_token *token;
	size_t buflen;

	token = json_get_member(json, name);
	if (!token || token->type != JSON_STRING)
		return 0;

	(*buf) = calloc(1, 33);
	if (!*buf)
		return 0;

	buflen = base64url_decode(*buf, (unsigned char *) token->string, strlen(token->string));
	return buflen;
}

enum crypto_hash_alg {
	CRYPTO_HASH_ALG_MD5, CRYPTO_HASH_ALG_SHA1,
	CRYPTO_HASH_ALG_HMAC_MD5, CRYPTO_HASH_ALG_HMAC_SHA1,
	CRYPTO_HASH_ALG_SHA256, CRYPTO_HASH_ALG_HMAC_SHA256,
	CRYPTO_HASH_ALG_SHA384, CRYPTO_HASH_ALG_SHA512
};


static uint8_t *dpp_parse_jws_prot_hdr(/* const struct dpp_curve_params *curve, */
		       const uint8_t *prot_hdr, uint16_t prot_hdr_len,
		       int *hash_func, uint8_t *kidlen)
{
	struct json_token *root, *token;
	uint8_t *kid = NULL;

	root = json_parse((const char *) prot_hdr, prot_hdr_len);
	if (!root) {
		fprintf(stderr,
			   "DPP: JSON parsing failed for JWS Protected Header\n");
		goto fail;
	}

	if (root->type != JSON_OBJECT) {
		fprintf(stderr,
			   "DPP: JWS Protected Header root is not an object\n");
		goto fail;
	}

	token = json_get_member(root, "typ");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: No typ string value found\n");
		goto fail;
	}
	pr_debug("DPP: JWS Protected Header typ=%s\n",
		   token->string);
	if (strcmp(token->string, "dppCon") != 0) {
		fprintf(stderr,
			   "DPP: Unsupported JWS Protected Header typ=%s\n",
			   token->string);
		goto fail;
	}

	token = json_get_member(root, "alg");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: No alg string value found\n");
		goto fail;
	}
	pr_debug("DPP: JWS Protected Header alg=%s\n",
		   token->string);
	if (strcmp(token->string, /* curve->jws_alg */ "ES256") != 0) {
		fprintf(stderr,
			   "DPP: Unexpected JWS Protected Header alg=%s (expected %s based on C-sign-key)\n",
			   token->string, /* curve->jws_alg */ "ES256");
		goto fail;
	}
	if (strcmp(token->string, "ES256") == 0 ||
	    strcmp(token->string, "BS256") == 0) {
		*hash_func = CRYPTO_HASH_ALG_SHA256;
	} else if (strcmp(token->string, "ES384") == 0 ||
		   strcmp(token->string, "BS384") == 0) {
		*hash_func = CRYPTO_HASH_ALG_SHA384;
	} else if (strcmp(token->string, "ES512") == 0 ||
		   strcmp(token->string, "BS512") == 0) {
		*hash_func = CRYPTO_HASH_ALG_SHA512;
	} else {
		*hash_func = -1;
		fprintf(stderr,
			   "DPP: Unsupported JWS Protected Header alg=%s\n",
			   token->string);
		goto fail;
	}

	*kidlen = json_get_member_base64url(root, "kid", &kid);
	if (!*kidlen) {
		pr_debug("DPP: No kid string value found\n");
		goto fail;
	}

fail:
	json_free(root);
	return kid;
}

static int dpp_check_pubkey_match(struct crypto_ec_key *pub,
				  uint8_t *r_hash, uint8_t rlen)
{
	uint8_t *uncomp;
	size_t unlen;
	int res;
	uint8_t hash[SHA256_MAC_LEN];
	const uint8_t *addr[1];
	size_t len[1];

	if (rlen != SHA256_MAC_LEN)
		return -1;

	res = ecc_key_get_pubkey_point(pub, 1, &uncomp, &unlen);
	if (res)
		return -1;

	addr[0] = uncomp;
	len[0] = unlen;
	bufprintf("DPP: Uncompressed public key", (uint8_t *)addr[0], len[0]);
	res = PLATFORM_SHA256(1, addr, len, hash);
	free(uncomp);
	if (res < 0)
		return -1;

	if (memcmp(hash, r_hash, SHA256_MAC_LEN) != 0) {
		pr_debug("DPP: Received hash value does not match calculated public key hash value\n");
		bufprintf("DPP: Calculated hash", hash, SHA256_MAC_LEN);
		return -1;
	}

	return 0;
}

enum dpp_status
dpp_process_signed_connector(struct dpp_signed_connector_info *info,
			     struct crypto_ec_key *csign_pub,
			     const char *connector)
{
	enum dpp_status ret = 255;
	const char *pos, *end, *signed_start, *signed_end;
	uint8_t *kid = NULL, kid_len = 0;
	unsigned char *prot_hdr = NULL, *signature = NULL;
	size_t prot_hdr_len = 0, signature_len = 0, signed_len;
	int res, hash_func = -1;
	//const struct dpp_curve_params *curve;
	uint8_t *hash = NULL;

	//curve = dpp_get_curve_ike_group(crypto_ec_key_group(csign_pub));
	//if (!curve)
	//        goto fail;
	//pr_debug("DPP: C-sign-key group: %s\n", curve->jwk_crv);
	//memset(info, 0, sizeof(*info));

	signed_start = pos = connector;
	end = strchr(pos, '.');
	if (!end) {
		pr_debug("DPP: Missing dot(1) in signedConnector\n");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	prot_hdr = calloc(1, abs(end - pos) * 3 / 4);
	if (!prot_hdr) {
		pr_debug("|%s:%d| DPP: Error allocating!\n", __func__, __LINE__);
		goto fail;
	}
	prot_hdr_len = base64url_decode(prot_hdr, (unsigned char *)pos, abs(end - pos));
	if (prot_hdr_len == 0) {
		fprintf(stderr,
			   "DPP: Failed to base64url decode signedConnector JWS Protected Header\n");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	kid = dpp_parse_jws_prot_hdr(/* curve , */prot_hdr, prot_hdr_len, &hash_func, &kid_len);
	if (!kid) {
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	if (kid_len != SHA256_MAC_LEN) {
		fprintf(stderr,
			   "DPP: Unexpected signedConnector JWS Protected Header kid length: %u (expected %d)\n",
			   (unsigned int) kid_len, SHA256_MAC_LEN);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	pos = end + 1;
	end = strchr(pos, '.');
	if (!end) {
		fprintf(stderr,
			   "DPP: Missing dot(2) in signedConnector\n");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	signed_end = end - 1;


	info->payload = calloc(1, abs(end - pos) * 3 / 4);
	if (!info->payload) {
		pr_debug("|%s:%d| DPP: Error allocating!\n", __func__, __LINE__);
		goto fail;
	}
	info->payload_len = base64url_decode(info->payload, (unsigned char *) pos, abs(end - pos));
	if (info->payload_len == 0) {
		fprintf(stderr,
			   "DPP: Failed to base64url decode signedConnector JWS Protected Header\n");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	pos = end + 1;

	signature = calloc(1, strlen(pos) * 3 / 4);
	if (!signature) {
		pr_debug("|%s:%d| DPP: Error allocating!\n", __func__, __LINE__);
		goto fail;
	}
	signature_len = base64url_decode(signature, (unsigned char *) pos, strlen(pos));
	if (signature_len == 0) {
		fprintf(stderr,
			   "DPP: Failed to base64url decode signedConnector JWS Protected Header\n");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	bufprintf("DPP: signedConnector - signature", signature, signature_len);

	if (dpp_check_pubkey_match(csign_pub, kid, kid_len) < 0) {
		ret = DPP_STATUS_NO_MATCH;
		goto fail;
	}

	if (signature_len & 0x01) {
		fprintf(stderr,
			   "DPP: Unexpected signedConnector signature length (%d)\n",
			   (int) signature_len);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	hash = malloc(32/* curve->hash_len */);
	if (!hash)
		goto fail;

	signed_len = signed_end - signed_start + 1;
	if (hash_func == CRYPTO_HASH_ALG_SHA256)
		res = PLATFORM_SHA256(1, (const uint8_t **) &signed_start, &signed_len,
				    hash);
	else
		goto fail;

	if (res)
		goto fail;

	res = ecdsa_verify_signature_r_s(csign_pub, hash, 32 /* curve->hash_len */,
					 signature, signature_len / 2,
					 signature + signature_len / 2,
					 signature_len / 2);
	if (res != 1) {
		pr_debug("DPP: signedConnector signature mismatch (res=%d)\n", res);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	ret = DPP_STATUS_OK;
fail:
	free(hash);
	free(prot_hdr);
	free(kid);
	free(signature);
	return ret;
}

static int dpp_parse_connector(struct dpp_peer *peer,
			       struct dpp_config_obj *conf,
			       const unsigned char *payload,
			       uint16_t payload_len)
{
	struct json_token *root, *groups, *netkey, *token;
	int ret = -1;
	struct crypto_ec_key *key = NULL;
	//const struct dpp_curve_params *curve;
	unsigned int rules = 0;

	root = json_parse((const char *) payload, payload_len);
	if (!root) {
		pr_debug("DPP: JSON parsing of connector failed\n");
		goto fail;
	}

	groups = json_get_member(root, "groups");
	if (!groups || groups->type != JSON_ARRAY) {
		pr_debug("DPP: No groups array found\n");
		goto skip_groups;
	}
	for (token = groups->child; token; token = token->sibling) {
		struct json_token *id, *role;

		id = json_get_member(token, "groupId");
		if (!id || id->type != JSON_STRING) {
			pr_debug("DPP: Missing groupId string\n");
			goto fail;
		}

		role = json_get_member(token, "netRole");
		if (!role || role->type != JSON_STRING) {
			pr_debug("DPP: Missing netRole string\n");
			goto fail;
		}
		fprintf(stderr,
			   "DPP: connector group: groupId='%s' netRole='%s'\n",
			   id->string, role->string);
		conf->netrole = dpp_netrole_from_str(role->string);
		rules++;
	}
skip_groups:

	if (!rules) {
		fprintf(stderr,
			   "DPP: Connector includes no groups\n");
		goto fail;
	}

	token = json_get_member(root, "expiry");
	if (!token || token->type != JSON_STRING) {
		fprintf(stderr,
			   "DPP: No expiry string found - connector does not expire\n");
	} else {
		pr_debug("DPP: expiry = %s", token->string);
		//if (dpp_key_expired(token->string,
		//                    &peer->net_access_key_expiry)) {
		//        fprintf(stderr,
		//                   "DPP: Connector (netAccessKey) has expired\n");
		//        goto fail;
		//}
	}

	netkey = json_get_member(root, "netAccessKey");
	if (!netkey || netkey->type != JSON_OBJECT) {
		pr_debug("DPP: No netAccessKey object found\n");
		goto fail;
	}

	key = dpp_parse_jwk(netkey);
	if (!key)
		goto fail;
       pr_debug("DPP: Received netAccessKey\n");

	/* If we use peer's public key for generating netaccesskey while doing self sign,
	 * the key comparison fails as it compares against csign key. So, do not return
	 * failure for self sign when public key of peer is used
	 */
	if (!peer->map_peer_pub_key) {
		if (is_ecc_key_equal(key, peer->own_protocol_key) != 1) {
			pr_debug("DPP: netAccessKey in connector does not match own protocol key\n");
			goto fail;
		}
	}

	ret = 0;
fail:
	ecc_key_free(key);
	json_free(root);
	return ret;
}

static void dpp_copy_netaccesskey(struct dpp_peer *peer,
				  struct dpp_config_obj *conf)
{
	uint8_t *net_access_key;
	struct crypto_ec_key *own_key;
	size_t len;

	own_key = peer->own_protocol_key;
	if (peer->reconfig_connector_key == DPP_CONFIG_REUSEKEY &&
	    peer->reconfig_old_protocol_key)
		own_key = peer->reconfig_old_protocol_key;

	net_access_key = ecc_key_get_privkey_der(own_key, &len);
	if (!net_access_key)
		return;

	free(peer->net_access_key);
	peer->net_access_key = net_access_key;
	peer->net_access_key_len = len;
}

static void dpp_copy_csign(struct dpp_config_obj *conf,
			   struct crypto_ec_key *csign)
{
	uint8_t *c_sign_key;
	size_t len;

	c_sign_key = ecc_key_get_spki(csign, &len);
	if (!c_sign_key)
		return;

	free(conf->c_sign_key);
	conf->c_sign_key = c_sign_key;
	conf->csign_len = len;
}

static void dpp_copy_ppkey(struct dpp_config_obj *conf,
			   struct crypto_ec_key *ppkey)
{
	uint8_t *pp_key;
	size_t len = 0;

	pp_key = ecc_key_get_spki(ppkey, &len);
	if (!pp_key)
		return;

	free(conf->pp_key);
	conf->pp_key = pp_key;
	conf->pp_key_len = len;
}

static int dpp_parse_cred_dpp(struct dpp_peer *peer,
			      struct dpp_config_obj *conf,
			      struct json_token *cred)
{
	struct dpp_signed_connector_info info;
	struct json_token *token, *csign, *ppkey;
	int ret = -1;
	struct crypto_ec_key *csign_pub = NULL, *pp_pub = NULL;
	const struct dpp_curve_params *key_curve = NULL, *pp_curve = NULL;
	const char *signed_connector;

	memset(&info, 0, sizeof(info));

	if (dpp_akm_psk(conf->akm) || dpp_akm_sae(conf->akm)) {
		fprintf(stderr,
			   "DPP: Legacy credential included in Connector credential\n");
		if (dpp_parse_cred_legacy(conf, cred) < 0)
			return -1;
	}

	pr_debug("DPP: Connector credential\n");

	csign = json_get_member(cred, "csign");
	if (!csign || csign->type != JSON_OBJECT) {
		pr_debug("DPP: No csign JWK in JSON\n");
		goto fail;
	}

	csign_pub = dpp_parse_jwk(csign); //, &key_curve
	if (!csign_pub) {
		pr_debug("DPP: Failed to parse csign JWK\n");
		goto fail;
	}
	//pr_debug("DPP: Received C-sign-key\n", csign_pub);

	ppkey = json_get_member(cred, "ppKey");
	if (ppkey && ppkey->type == JSON_OBJECT) {
		pp_pub = dpp_parse_jwk(ppkey); //, &pp_curve
		if (!pp_pub) {
			pr_debug("DPP: Failed to parse ppKey JWK\n");
			goto fail;
		}
		//pr_debug("DPP: Received ppKey\n", pp_pub);
		if (key_curve != pp_curve) {
			fprintf(stderr,
				   "DPP: C-sign-key and ppKey do not use the same curve\n");
			goto fail;
		}
	}

	token = json_get_member(cred, "signedConnector");
	if (!token || token->type != JSON_STRING) {
		pr_debug("DPP: No signedConnector string found\n");
		goto fail;
	}

	signed_connector = token->string;

	if (strchr(signed_connector, '"') ||
	    strchr(signed_connector, '\n')) {
		fprintf(stderr,
			   "DPP: Unexpected character in signedConnector\n");
		goto fail;
	}

	if (dpp_process_signed_connector(&info, csign_pub,
					 signed_connector) != DPP_STATUS_OK)
		goto fail;

	if (dpp_parse_connector(peer, conf,
				info.payload, info.payload_len) < 0) {
		pr_debug("DPP: Failed to parse connector\n");
		goto fail;
	}

	free(conf->connector);
	conf->connector = strdup(signed_connector);

	dpp_copy_csign(conf, csign_pub);
	if (pp_pub)
		dpp_copy_ppkey(conf, pp_pub);
	if (dpp_akm_dpp(conf->akm) || peer->peer_version >= 2)
		dpp_copy_netaccesskey(peer, conf);

	ret = 0;
fail:
	ecc_key_free(csign_pub);
	ecc_key_free(pp_pub);
	free(info.payload);
	return ret;
}

static int dpp_parse_conf_obj(struct dpp_peer *peer, const uint8_t *conf_obj,
			      uint16_t conf_obj_len)
{
	int ret = -1;
	struct json_token *root, *token, *discovery, *cred;
	struct dpp_config_obj *conf;
	uint8_t *ssid64 = NULL;
	int ssid64_len;
	int legacy;

	pr_debug("DPP: Configuration object\n%s\n", conf_obj);

	root = json_parse((const char *) conf_obj, conf_obj_len);
	if (!root)
		return -1;
	pr_debug("|%s:%d|\n", __func__, __LINE__);
	if (root->type != JSON_OBJECT) {
		pr_debug("JSON root is not an object\n");
		goto fail;
	}

	token = json_get_member(root, "wi-fi_tech");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No wi-fi_tech string value found\n");
		goto fail;
	}
	if ((strcmp(token->string, "map") != 0) &&
		 (strcmp(token->string, "infra") != 0)) {
		pr_debug("DPP: Unsupported wi-fi_tech value: '%s' for netrole '%s\n",
			   token->string, dpp_netrole_str(peer->e_req.netrole));
		pr_debug("Unsupported wi-fi_tech value\n");
		goto fail;
	}
	if (peer->num_conf_obj == 10) {
		fprintf(stderr,
			   "DPP: No room for this many Config Objects - ignore this one\n");
		ret = 0;
		goto fail;
	}

	conf = dpp_config_obj_init(peer);
	if (!conf) {
		pr_debug("DPP: Fail initializing config object\n");
		goto fail;
	}

	cred = json_get_member(root, "cred");
	if (!cred || cred->type != JSON_OBJECT) {
		pr_debug("No cred object in JSON\n");
		goto fail;
	}
	pr_debug("|%s:%d|\n", __func__, __LINE__);

	token = json_get_member(cred, "akm");
	if (!token || token->type != JSON_STRING) {
		pr_debug("No cred::akm string value found\n");
		goto fail;
	}

	conf->akm = dpp_akm_from_str(token->string);

	legacy = dpp_akm_legacy(conf->akm);
	if (legacy && peer->peer_version >= 2) {
		struct json_token *csign, *s_conn;

		csign = json_get_member(cred, "csign");
		s_conn = json_get_member(cred, "signedConnector");
		if (csign && csign->type == JSON_OBJECT &&
		    s_conn && s_conn->type == JSON_STRING)
			legacy = 0;
	}
	if (legacy) {
		if (dpp_parse_cred_legacy(conf, cred) < 0)
			goto fail;
	} else if (dpp_akm_dpp(conf->akm) ||
		   (peer->peer_version >= 2 && dpp_akm_legacy(conf->akm))) {
		if (dpp_parse_cred_dpp(peer, conf, cred) < 0)
			goto fail;
	} else {
		pr_debug("DPP: Unsupported akm: %s\n",
			   token->string);
		goto fail;
	}

	if (conf->netrole == DPP_NETROLE_MAPAGENT ||
		conf->netrole == DPP_NETROLE_MAPCONTROLLER) {
		if (conf->akm != DPP_AKM_DPP) {
			pr_debug("MAP AKM not DPP, for ieee1905 security\n");
			pr_debug("DPP: AKM should be DPP, for netrole '%s'\n",
				dpp_netrole_str(conf->netrole));
			goto fail;
		}
		goto cont;
	}

	discovery = json_get_member(root, "discovery");
	if (!discovery || discovery->type != JSON_OBJECT) {
		pr_debug("No discovery object in JSON\n");
		goto fail;
	}

	ssid64_len = json_get_member_base64url(discovery, "ssid64", &ssid64);
	if (ssid64_len) {
		/* bufprintf("DPP: discovery::ssid64", ssid64, ssid64_len); */
		if (ssid64_len > 32) {
			pr_debug("Too long discovery::ssid64 value\n");
			goto fail;
		}
	} else {
		token = json_get_member(discovery, "ssid");
		if (!token || token->type != JSON_STRING) {
			pr_debug("No discovery::ssid value found\n");
			goto fail;
		}
		if (strlen(token->string) > 32) {
			pr_debug("Too long discovery::ssid value\n");
			goto fail;
		}
	}
	if (ssid64) {
		conf->ssid_len = ssid64_len;
		memcpy(conf->ssid, ssid64, conf->ssid_len);
	} else {
		conf->ssid_len = strlen(token->string);
		memcpy(conf->ssid, token->string, conf->ssid_len);
	}
	token = json_get_member(discovery, "ssid_charset");
	if (token && token->type == JSON_NUMBER) {
		conf->ssid_charset = token->number;
		pr_debug("DPP: ssid_charset=%d\n",
			   conf->ssid_charset);
	}


cont:
	pr_debug("DPP: JSON parsing completed successfully\n");
	ret = 0;
	free(ssid64);
	json_free(root);
	return ret;
fail:
	free(ssid64);
	json_free(root);
	dpp_config_obj_free(conf);
	return ret;
}

int dpp_conf_resp_rx(struct dpp_peer *peer,
		     uint8_t *resp, uint16_t resp_len)
{
	uint8_t *wrapped_data, *e_nonce, *status, *conf_obj;
	uint16_t wrapped_data_len, e_nonce_len, status_len, conf_obj_len;
	uint8_t *addr[1];
	size_t len[1];
	uint8_t *unwrapped = NULL;
	size_t unwrapped_len = 0;
	int ret = -1;

	peer->conf_status = 255;
	if (dpp_check_attrs(resp, resp_len) < 0) {
		pr_debug("Invalid attribute in config response\n");
		return -1;
	}
	wrapped_data = dpp_get_attr(resp, resp_len,
				    DPP_ATTR_WRAPPED_DATA,
				    &wrapped_data_len);
	if (!wrapped_data || wrapped_data_len < AES_BLOCK_SIZE) {
		fprintf(stderr,
			      "Missing or invalid required Wrapped Data attribute\n");
		return -1;
	}

	bufprintf("DPP: AES-SIV ciphertext", wrapped_data, wrapped_data_len);
	unwrapped_len = wrapped_data_len - AES_BLOCK_SIZE;
	unwrapped = malloc(unwrapped_len);
	if (!unwrapped)
		return -1;

	addr[0] = resp;
	len[0] = wrapped_data - 4 - (uint8_t *) resp;
	bufprintf("DDP: AES-SIV AD", addr[0], len[0]);

	if (AES_SIV_DECRYPT(peer->ke, 32/* peer->curve->hash_len */ ,
			    wrapped_data, wrapped_data_len,
			    1, (const uint8_t **) addr, len, unwrapped) < 0) {
		pr_debug("AES-SIV decryption failed\n");
		goto fail;
	}
	bufprintf("DPP: AES-SIV cleartext",
		    unwrapped, unwrapped_len);

	if (dpp_check_attrs(unwrapped, unwrapped_len) < 0) {
		pr_debug("Invalid attribute in unwrapped data\n");
		goto fail;
	}

	e_nonce = dpp_get_attr(unwrapped, unwrapped_len,
			       DPP_ATTR_ENROLLEE_NONCE,
			       &e_nonce_len);
	if (!e_nonce || e_nonce_len != 16 /* peer->curve->nonce_len */ ) {
		fprintf(stderr,
			      "Missing or invalid Enrollee Nonce attribute\n");
		goto fail;
	}
	pr_debug("e_nonce_len:%d\n", e_nonce_len);
	bufprintf("DPP: Enrollee Nonce", e_nonce, e_nonce_len);
	if (memcmp(e_nonce, peer->e_nonce, e_nonce_len) != 0) {
		pr_debug("Enrollee Nonce mismatch");
		goto fail;
	}

	status = dpp_get_attr(resp, resp_len,
			      DPP_ATTR_STATUS, &status_len);
	if (!status || status_len < 1) {
		fprintf(stderr,
			      "Missing or invalid required DPP Status attribute\n");
		goto fail;
	}
	peer->conf_status = status[0];
	pr_debug("DPP: Status %u\n", status[0]);

	if (status[0] == DPP_STATUS_CSR_NEEDED) {
		uint8_t *csrattrs;
		size_t csrattrs_len;

		pr_debug("DPP: Configurator requested CSR\n");

		csrattrs = dpp_get_csr_attrs(unwrapped, unwrapped_len,
					     &csrattrs_len);
		if (!csrattrs) {
			pr_debug("Missing or invalid CSR Attributes Request attribute\n");
			goto fail;
		}
		bufprintf("DPP: CsrAttrs", csrattrs, csrattrs_len);
		free(peer->csrattrs);
		peer->csrattrs = csrattrs;
		peer->csrattrs_len = csrattrs_len;
		ret = -2;
		goto fail;
	}

	if (status[0] != DPP_STATUS_OK) {
		pr_debug("Configurator rejected configuration\n");
		goto fail;
	}

	conf_obj = dpp_get_attr(unwrapped, unwrapped_len, DPP_ATTR_CONFIG_OBJ,
				&conf_obj_len);
	if (!conf_obj) {
		fprintf(stderr,
			      "Missing required Configuration Object attribute\n");
		goto fail;
	}
	while (conf_obj) {
		bufprintf("DPP: configurationObject JSON", conf_obj, conf_obj_len);
		if (dpp_parse_conf_obj(peer, conf_obj, conf_obj_len) < 0) {
			pr_debug("%s %d\n", __func__, __LINE__);
			goto fail;
		}
		conf_obj = dpp_get_attr_next(conf_obj, unwrapped, unwrapped_len,
					     DPP_ATTR_CONFIG_OBJ,
					     &conf_obj_len);
	}

	status = dpp_get_attr(unwrapped, unwrapped_len,
			      DPP_ATTR_SEND_CONN_STATUS, &status_len);
	if (status) {
		fprintf(stderr,
			   "DPP: Configurator requested connection status result");
		peer->conn_status_requested = 1;
	}

	ret = 0;

fail:
	free(unwrapped);
	return ret;
}

void dpp_handle_config_obj(struct dpp_peer *auth, struct dpp_config_obj *conf)
{
	pr_debug("DPP_EVENT_CONF_RECEIVED\n");
	pr_debug("DPP_EVENT_CONFOBJ_NETROLE %s\n", dpp_netrole_str(conf->netrole));
	pr_debug("DPP_EVENT_CONFOBJ_AKM %s\n", dpp_akm_str(conf->akm));
	if (conf->ssid_len)
		pr_debug("DPP_EVENT_CONFOBJ_SSID %s\n", ssid_to_str(conf->ssid, conf->ssid_len));
	if (conf->connector) {
		/* TODO: Save the Connector and consider using a command
		 * to fetch the value instead of sending an event with
		 * it. The Connector could end up being larger than what
		 * most clients are ready to receive as an event
		 * message. */
		pr_debug("DPP_EVENT_CONNECTOR %s\n", conf->connector);
	}

	if (conf->passphrase[0]) {
		char hex[64 * 2 + 1];

		snprintf_hex(hex, sizeof(hex),
				 (uint8_t *) conf->passphrase,
				 strlen(conf->passphrase));
		pr_debug("DPP_EVENT_CONFOBJ_PASS %s\n", hex);
	} else if (conf->psk_set) {
		char hex[PMKID_LEN * 2 + 1];

		snprintf_hex(hex, sizeof(hex), conf->psk, PMKID_LEN);
		pr_debug("DPP_EVENT_CONFOBJ_PSK %s\n", hex);
	}
	if (conf->c_sign_key) {
		char *hex;
		size_t hexlen;

		hexlen = 2 * conf->csign_len + 1;
		hex = malloc(hexlen);
		if (hex) {
			snprintf_hex(hex, hexlen,
					 conf->c_sign_key,
					 conf->csign_len);
			pr_debug("DPP_EVENT_C_SIGN_KEY %s\n", hex);
			free(hex);
		}
	}
	if (auth->net_access_key) {
		char *hex;
		size_t hexlen;

		hexlen = 2 * auth->net_access_key_len + 1;
		hex = malloc(hexlen);
		if (hex) {
			snprintf_hex(hex, hexlen,
					 auth->net_access_key,
					 auth->net_access_key_len);
			//if (auth->net_access_key_expiry)
			//        fprintf(stderr,
			//                "DPP_EVENT_NET_ACCESS_KEY %s %lu\n", hex,
			//                (unsigned long)
			//                auth->net_access_key_expiry);
			//else
			pr_debug("DPP_EVENT_NET_ACCESS_KEY %s\n", hex);
			free(hex);
		}
	}
}

int dpp_process_config_response(struct dpp_context *ctx,
				uint8_t *peer_macaddr,
				uint8_t *frame, size_t framelen)
{
	//const uint8_t *pos;
	enum dpp_status status = DPP_STATUS_CONFIG_REJECTED;
	struct dpp_peer *peer;
#ifdef DEBUG // testing with sample data
	struct dpp_conf_req *req = &global_req;
	struct dpp_config_obj *conf;
	int i;
#endif
       /* if (!auth || !auth->auth_success) {
		pr_debug("DPP: No matching exchange in progress");
		return;
	}
	if (result != GAS_QUERY_AP_SUCCESS ||
	    !resp || status_code != WLAN_STATUS_SUCCESS) {
		pr_debug("DPP: GAS query did not succeed");
		goto fail;
	}

	bufprintf("DPP: Configuration Response adv_proto",
			adv_proto, adv_proto_len);
	bufprintf("DPP: Configuration Response (GAS response)",
			resp, resp_len);
	uint8_t *pos5 = &pos[5];
	if (adv_proto_len != 10 ||
	    !(pos = adv_proto) ||
	    pos[0] != WLAN_EID_ADV_PROTO ||
	    pos[1] != 8 ||
	    pos[3] != WLAN_EID_VENDOR_SPECIFIC ||
	    pos[4] != 5 ||
	    BUF_GET_BE24(pos5) != OUI_WFA ||
	    pos[8] != 0x1a ||
	    pos[9] != 1) {
		pr_debug("DPP: Not a DPP Advertisement Protocol ID\n");
		goto fail;
	}*/

	peer = dpp_lookup_peer(ctx, peer_macaddr);
	if (!peer) {
		pr_debug("%s: No peer " MACFMT " found\n", __func__,
			MAC2STR(peer_macaddr));
		return -1;
	}

	if (dpp_conf_resp_rx(peer, frame + 19, framelen - 19) < 0) {
		pr_debug("DPP: Configuration attempt failed\n");
		goto fail;
	}

#ifdef DEBUG
	for (i = 0; i < req->num_bsta; i++) {
		enum wifi_band band = dpp_chan_list_to_band(req->blist[i].chan_list);
		int bandnum = 0;

		if (band == BAND_2)
			bandnum = 2;
		if (band == BAND_5)
			bandnum = 5;
		if (band == BAND_6)
			bandnum = 6;

		pr_debug("DPP: Trying to find creds for band:%d\n", bandnum);

		conf = dpp_config_obj_get_netrole(peer, DPP_NETROLE_MAP_BH_STA, i);
		if (!conf) {
			pr_debug("DPP: Did not find conf obj for netrole %s\n",
				dpp_netrole_str(DPP_NETROLE_MAP_BH_STA));
			return -1;
		}

		dpp_handle_config_obj(peer, conf);
	}
#endif

	status = DPP_STATUS_OK;
fail:
	if (status != DPP_STATUS_OK) {
		pr_debug("DPP CONF FAILED\n");
	}

	if (peer->peer_version >= 2 &&
	    peer->conf_status == DPP_STATUS_OK) {
		//pr_debug("DPP: Send DPP Configuration Result\n");
		pr_debug("DPP CONF SUCCESS VERSION >= 2\n");

		//pr_debug("DPP_EVENT_TX dst=" MACSTR " freq=%u type=%d\n",
		//	MAC2STR(peer_macaddr), peer->curr_freq,
		//	DPP_PA_CONFIGURATION_RESULT);


		/* This exchange will be terminated in the TX status handler */
		peer->connect_on_tx_status = 1;
		return 0;
	}

	return 0;
}
