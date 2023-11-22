/*
 * ecc_cryptutil.c - implements EC crypto functions using Openssl-3x.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <openssl/core_names.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <openssl/dh.h>
#include <openssl/hmac.h>
#include <openssl/cmac.h>
#include <openssl/x509.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/encoder.h>
#include <openssl/decoder.h>

#include "easy.h"

#if OPENSSL_VERSION_NUMBER < 0x30000000L
#error "libeasy: ECC utility functions need atleast Openssl-3.0"
#endif

#ifdef ECC_UTILS_SELF_TEST
void bufprintf(const char *label, uint8_t *buf, size_t len)
{
	if (label)
		printf("%s ", label);
	for (int i = 0; i < len; i++)
		printf("0x%02x, ", buf[i] & 0xff);
	printf("\n");
}
#else
#define bufprintf(l,b,s)
#endif

/* get curve name from EC key */
LIBEASY_API char *ecc_key_get_curve_name(void *key, char *curve_name)
{
	char name[64];
	size_t len;

	if (!curve_name)
		return NULL;

	if (!key || !EVP_PKEY_is_a(key, "EC"))
		return NULL;

	if (!EVP_PKEY_get_group_name(key, name, sizeof(name), &len))
		return NULL;

	strncpy(curve_name, name, len);
	return curve_name;
}

/* get NID from EC key */
int LIBEASY_API ecc_key_get_nid(void *key)
{
	char curve_name[64];
	int curve_nid;
	size_t len;

	if (!key || !EVP_PKEY_is_a(key, "EC"))
		return -1;

	if (!EVP_PKEY_get_group_name(key, curve_name, sizeof(curve_name), &len))
		return -1;

	curve_nid = OBJ_txt2nid(curve_name);
	return curve_nid == NID_undef ? -1 : curve_nid;
}

/* get private key of EC key */
LIBEASY_API uint8_t *ecc_key_get_private(void *key, size_t *outlen)
{
	BIGNUM *bn_priv = NULL;
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_bn_param((EVP_PKEY *)key, OSSL_PKEY_PARAM_PRIV_KEY, &bn_priv);
	if (!ret)
		return NULL;


	olen = BN_num_bytes(bn_priv);
	out = calloc(1, olen);
	if (!out) {
		BN_clear_free(bn_priv);
		return NULL;
	}

	BN_bn2bin(bn_priv, out);
	*outlen = olen;

	BN_clear_free(bn_priv);
	return out;
}

/* get encoded-public-key of EC key */
LIBEASY_API uint8_t *ecc_key_get_encoded_pubkey(void *key, size_t *outlen)
{
	uint8_t pub[256];
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_octet_string_param((EVP_PKEY *)key,
					      OSSL_PKEY_PARAM_ENCODED_PUBLIC_KEY,
					      pub, sizeof(pub), &olen);
	if (!ret)
		return NULL;

	out = calloc(1, olen);
	if (!out)
		return NULL;

	memcpy(out, pub, olen);
	*outlen = olen;

	return out;
}

/* get pub-key of EC key */
LIBEASY_API uint8_t *ecc_key_get_pubkey(void *key, size_t *outlen)
{
	uint8_t pub[256];
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_octet_string_param((EVP_PKEY *)key,
					      OSSL_PKEY_PARAM_PUB_KEY,
					      pub, sizeof(pub), &olen);
	if (!ret)
		return NULL;

	out = calloc(1, olen);
	if (!out)
		return NULL;

	memcpy(out, pub, olen);
	*outlen = olen;
	return out;
}


/* get DER-encoded SubjectPublicKeyInfo structure of pubkey */
LIBEASY_API uint8_t *ecc_key_get_spki(void *key, size_t *olen)
{
	const char *output_structure = "SubjectPublicKeyInfo";
	const char *outformat = "DER";
	OSSL_ENCODER_CTX *ectx;
	uint8_t *spki = NULL;
	size_t conv_len;
	int selection;
	char conv[80];


	*olen = 0;

	if (!EVP_PKEY_get_utf8_string_param(key,
					    OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
					    conv, sizeof(conv), &conv_len)) {
		return NULL;
	}

	if (strcmp(conv, "compressed")) {
		//printf("%s: change key to point compressed format\n", __func__);
		if (EVP_PKEY_set_utf8_string_param(key,
				OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
				OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_COMPRESSED) != 1) {

			printf("OpenSSL: Failed to set compressed format\n");
			return NULL;
		}
	}

	selection = OSSL_KEYMGMT_SELECT_ALL_PARAMETERS |
		    OSSL_KEYMGMT_SELECT_PUBLIC_KEY;

	ectx = OSSL_ENCODER_CTX_new_for_pkey(key, selection, outformat,
					     output_structure, NULL);
	if (ectx) {
		OSSL_ENCODER_to_data(ectx, &spki, olen);
		OSSL_ENCODER_CTX_free(ectx);
	}

	return spki;
}

/* get DER-encoded privkey */
LIBEASY_API uint8_t *ecc_key_get_privkey_der(void *key, size_t *olen)
{
	const char *output_structure = "type-specific";
	const char *outformat = "DER";
	uint8_t *der_pkey = NULL;
	BIGNUM *bn_priv = NULL;
	OSSL_ENCODER_CTX *ectx;
	int selection;


	*olen = 0;

	if (!EVP_PKEY_get_bn_param((EVP_PKEY *)key, OSSL_PKEY_PARAM_PRIV_KEY, &bn_priv)) {
		printf("No private key in EC key\n");
		return NULL;
	}

	selection = OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS |
		    OSSL_KEYMGMT_SELECT_PRIVATE_KEY;

	ectx = OSSL_ENCODER_CTX_new_for_pkey(key, selection, outformat,
					     output_structure, NULL);
	if (ectx) {
		OSSL_ENCODER_to_data(ectx, &der_pkey, olen);
		OSSL_ENCODER_CTX_free(ectx);
	}

	BN_clear_free(bn_priv);
	return der_pkey;
}

/* TODO: deprecate */
int LIBEASY_API ecc_key_get_pubkey_point(void *key, int prefix, uint8_t **pub,
					 size_t *publen)
{
	*pub = ecc_key_get_encoded_pubkey(key, publen);
	if (*pub) {
		if (!prefix) {
			uint8_t *pos = *pub;

			*publen -= 1;
			memmove(pos, pos + 1, *publen);
		}
	}

	return *pub ? 0 : -1;
}

/* get point-x */
LIBEASY_API uint8_t *ecc_key_get_x(void *key, size_t *outlen)
{
	BIGNUM *bn = NULL;
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_bn_param((EVP_PKEY *)key, OSSL_PKEY_PARAM_EC_PUB_X, &bn);
	if (!ret)
		return NULL;


	olen = BN_num_bytes(bn);
	out = calloc(1, olen);
	if (!out) {
		BN_clear_free(bn);
		return NULL;
	}

	BN_bn2bin(bn, out);
	*outlen = olen;

	BN_clear_free(bn);
	return out;
}

/* get point-y */
LIBEASY_API uint8_t *ecc_key_get_y(void *key, size_t *outlen)
{
	BIGNUM *bn = NULL;
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_bn_param((EVP_PKEY *)key, OSSL_PKEY_PARAM_EC_PUB_Y, &bn);
	if (!ret)
		return NULL;


	olen = BN_num_bytes(bn);
	out = calloc(1, olen);
	if (!out) {
		BN_clear_free(bn);
		return NULL;
	}

	BN_bn2bin(bn, out);
	*outlen = olen;

	BN_clear_free(bn);
	return out;
}

/* get EC point format (compressed/uncompressed) */
LIBEASY_API uint8_t *ecc_key_get_point_format(void *key, size_t *outlen)
{
	char buf[256] = {0};
	uint8_t *out;
	size_t olen;
	int ret;

	*outlen = 0;
	ret = EVP_PKEY_get_utf8_string_param((EVP_PKEY *)key,
					      OSSL_PKEY_PARAM_EC_POINT_CONVERSION_FORMAT,
					      buf, sizeof(buf), &olen);
	if (!ret)
		return NULL;

	out = calloc(1, olen);
	if (!out)
		return NULL;

	memcpy(out, buf, olen);
	*outlen = olen;

	return out;
}

/* dump EC key for debug */
void LIBEASY_API ecc_key_dump(const char *title, const void *key)
{
	BIO *out;
	size_t rlen;
	char *txt;
	int res;

	out = BIO_new(BIO_s_mem());
	if (!out)
		return;

	EVP_PKEY_print_private(out, (EVP_PKEY *)key, 0, NULL);
	rlen = BIO_ctrl_pending(out);
	txt = calloc(1, (rlen + 1) * sizeof(char));
	if (txt) {
		res = BIO_read(out, txt, rlen);
		if (res > 0) {
			txt[res] = '\0';
			fprintf(stderr, "%s: %s\n", title, txt);
		}
		free(txt);
	}
	BIO_free(out);
}

/* print EC key and params - also for debug */
void LIBEASY_API ecc_key_print(const char *title, void *key)
{
	char curve_name[64] = {0};
	uint8_t *out = NULL;
	size_t olen = 0;

	ecc_key_dump(title, key);

	fprintf(stderr, "Curve = %s\n", ecc_key_get_curve_name(key, curve_name));
	fprintf(stderr, "NID = %d\n", ecc_key_get_nid(key));

	out = ecc_key_get_private(key, &olen);
	if (out) {
		bufprintf("PrivateKey = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	} else {
		printf("No private key\n");
	}

	out = ecc_key_get_encoded_pubkey(key, &olen);
	if (out) {
		bufprintf("Encoded PUBKEY = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_pubkey(key, &olen);
	if (out) {
		bufprintf("PUBKEY = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_x(key, &olen);
	if (out) {
		bufprintf("X = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_y(key, &olen);
	if (out) {
		bufprintf("Y = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_point_format(key, &olen);
	if (out) {
		fprintf(stderr, "PUBKEY point-format = %s\n", out);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_spki(key, &olen);
	if (out) {
		bufprintf("DER-SubjectPublicKeyInfo = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}

	out = ecc_key_get_privkey_der(key, &olen);
	if (out) {
		bufprintf("DER-PrivateKey = ", out, olen);
		memset(out, 0, olen);
		free(out);
		out = NULL;
	}
}

/* generate EC key from curve name */
LIBEASY_API void *ecc_key_gen(const char *curve)
{
	return EVP_EC_gen(curve);
}

/* free EC key */
void LIBEASY_API ecc_key_free(void *key)
{
	EVP_PKEY_free((EVP_PKEY *)key);
}


/* check if key is a valid ECC key */
bool LIBEASY_API ecc_key_check(void *key)
{
	EVP_PKEY_CTX *pctx = NULL;
	bool chk = false;

	pctx = EVP_PKEY_CTX_new_from_pkey(NULL, key, NULL);
	if (!pctx) {
		printf("unable to check EC key\n");
		return false;
	}

	chk = EVP_PKEY_check(pctx) > 0 ? true : false;
	EVP_PKEY_CTX_free(pctx);

	return chk;
}

/* compare ECC keys */
int LIBEASY_API is_ecc_key_equal(void *key1, void *key2)
{
	return EVP_PKEY_eq((EVP_PKEY *)key1, (EVP_PKEY *)key2);
}

/* ECDH */
int LIBEASY_API ecc_ecdh(void *key, void *peerkey, uint8_t **out, size_t *secret_len)
{
	EVP_PKEY_CTX *ctx;
	uint8_t *secret;


	*secret_len = 0;
	*out = NULL;

	/* Create context for shared secret derivation */
	ctx = EVP_PKEY_CTX_new((EVP_PKEY *)key, NULL);
	if (!ctx)
		return -1;

	/* Initialize */
	if (1 != EVP_PKEY_derive_init(ctx))
		goto err;

	/* Provide the peer public key */
	if (1 != EVP_PKEY_derive_set_peer(ctx, (EVP_PKEY *)peerkey))
		goto err;

	/* Determine shared secret len */
	if (1 != EVP_PKEY_derive(ctx, NULL, secret_len))
		goto err;

	if (NULL == (secret = OPENSSL_malloc(*secret_len)))
		goto err;

	/* Derive the shared secret */
	if (1 != (EVP_PKEY_derive(ctx, secret, secret_len)))
		goto err;

	bufprintf("ECDH-secret = ", secret, *secret_len);
	*out = secret;

err:
	EVP_PKEY_CTX_free(ctx);
	return 0;
}

/* generate EC key for curve with points (x,y) */
LIBEASY_API void *ecc_key_gen_from_xy(char *curve, const uint8_t *bx, const uint8_t *by, size_t len)
{
	OSSL_PARAM params[3] = { 0 };
	EVP_PKEY_CTX *ctx = NULL;
	uint8_t bxy[1 + 2 * len];
	EVP_PKEY *key = NULL;
	int selection;


	bxy[0] = POINT_CONVERSION_UNCOMPRESSED;		/* 0x04 */
	memcpy(&bxy[1], bx, len);
	memcpy(&bxy[1+len], by, len);			/* 0x04|x|y */

	params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, curve, 0);
	params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PUB_KEY, bxy, sizeof(bxy));
	params[2] = OSSL_PARAM_construct_end();

	selection = EVP_PKEY_KEY_PARAMETERS | EVP_PKEY_PUBLIC_KEY;
	ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
	if (ctx
	    && EVP_PKEY_fromdata_init(ctx) > 0
	    && EVP_PKEY_fromdata(ctx, &key, selection, params) > 0) {
		ecc_key_print("Generated Key from XY", key);
	}

	EVP_PKEY_CTX_free(ctx);
	return key;
}

LIBEASY_API void *ecc_key_gen_from_spki(uint8_t *spki, size_t len)
{
	EVP_PKEY *key = NULL;

	key = d2i_PUBKEY(NULL, (const unsigned char **)&spki, len);
	if (!key)
		return NULL;

	return key;
}

/* generate EC key from data (data: input_type = DER) */
LIBEASY_API void *ecc_key_gen_from_data(uint8_t *data, size_t len, int is_public)
{
	OSSL_DECODER_CTX *dctx = NULL;
	const char *input_type = "DER";
	EVP_PKEY *key = NULL;
	int selection;
	const unsigned char *in = data;
	size_t inlen = len;

	selection = is_public ? EVP_PKEY_PUBLIC_KEY : EVP_PKEY_KEYPAIR;
	dctx = OSSL_DECODER_CTX_new_for_pkey(&key, input_type, NULL, "EC",
					     selection, NULL, "SHA256");

	(void)OSSL_DECODER_from_data(dctx, &in, &inlen);
	OSSL_DECODER_CTX_free(dctx);

	if (!key)
		fprintf(stderr, "Failed to load key from data\n");

	return key;
}

/* ECDSA sign data (data = SHA256 digest). Output signature is DER encoded */
LIBEASY_API uint8_t *ecdsa_sign(void *key, const uint8_t *data, size_t len, size_t *olen)
{
	EVP_MD_CTX *md_ctx = NULL;
	size_t out_len = 0;
	uint8_t *out = NULL;
	int ret;

	*olen = 0;
	md_ctx = EVP_MD_CTX_new();
	if (!md_ctx)
		return NULL;

	ret = EVP_DigestSignInit(md_ctx, NULL, EVP_sha256(), NULL, (EVP_PKEY *)key);
	if (ret <= 0)
		goto err;

	ret = EVP_DigestSignUpdate(md_ctx, data, len);
	if (ret <= 0)
		goto err;

	ret = EVP_DigestSignFinal(md_ctx, NULL, &out_len);
	if (ret <= 0)
		goto err;

	out = calloc(1, out_len);
	if (!out)
		goto err;

	ret = EVP_DigestSignFinal(md_ctx, out, &out_len);
	if (ret <= 0) {
		free(out);
		goto err;
	}

	*olen = out_len;
err:
	EVP_MD_CTX_free(md_ctx);
	return out;
}

int LIBEASY_API ecdsa_signature_extract_r_s(const uint8_t *sig, size_t siglen,
					    uint8_t **r, size_t *rlen,
					    uint8_t **s, size_t *slen)
{
	ECDSA_SIG *sign = NULL;
	uint8_t *rbuf = NULL;
	uint8_t *sbuf = NULL;
	size_t r1_len, s1_len;
	const BIGNUM *r1, *s1;


	*rlen = *slen = 0;
	*r = NULL;
	*s = NULL;

	sign = d2i_ECDSA_SIG(NULL, &sig, siglen);
	if (!sign)
		return -1;

	r1 = ECDSA_SIG_get0_r(sign);
	s1 = ECDSA_SIG_get0_s(sign);
	if (r1 == NULL || s1 == NULL)
		goto err;

	r1_len = BN_num_bytes(r1);
	s1_len = BN_num_bytes(s1);
	rbuf = OPENSSL_zalloc(r1_len);
	sbuf = OPENSSL_zalloc(s1_len);
	if (!rbuf || !sbuf)
		goto err;

	if (BN_bn2binpad(r1, rbuf, r1_len) <= 0)
		goto err;

	if (BN_bn2binpad(s1, sbuf, s1_len) <= 0)
		goto err;

	*r = rbuf;
	*s = sbuf;
	*rlen = r1_len;
	*slen = s1_len;
	ECDSA_SIG_free(sign);
	return 0;

err:
	OPENSSL_free(rbuf);
	OPENSSL_free(sbuf);
	ECDSA_SIG_free(sign);
	return -1;
}

/* ECDSA verify signature (r,s) for data (data = SHA256 digest) */
int LIBEASY_API ecdsa_verify_signature_r_s(void *key,
					   const uint8_t *data, size_t datalen,
					   const uint8_t *r, size_t r_len,
					   const uint8_t *s, size_t s_len)
{
	EVP_MD_CTX *md_ctx = NULL;
	ECDSA_SIG *sign = NULL;
	uint8_t *sig = NULL;
	size_t siglen = 0;
	BIGNUM *rbn, *sbn;
	int ret = -1;


	sign = ECDSA_SIG_new();
	if (!sign)
		return -1;

	rbn = BN_bin2bn(r, r_len, NULL);
	sbn = BN_bin2bn(s, s_len, NULL);
	if (!rbn || !sbn) {
		ECDSA_SIG_free(sign);
		return -1;
	}

	if (ECDSA_SIG_set0(sign, rbn, sbn) <= 0)
		goto err;

	siglen = i2d_ECDSA_SIG(sign, &sig);
	if (siglen <= 0)
		goto err;

	rbn = NULL;
	sbn = NULL;

	//bufprintf("(R)Signature = ", sig, siglen);

	md_ctx = EVP_MD_CTX_new();
	if (!md_ctx)
		return -1;

	ret = EVP_DigestVerifyInit(md_ctx, NULL, EVP_sha256(), NULL, (EVP_PKEY *)key);
	if (ret <= 0)
		goto err;

	ret = EVP_DigestVerify(md_ctx, sig, siglen, data, datalen);

err:
	BN_free(rbn);
	BN_free(sbn);
	OPENSSL_free(sig);
	ECDSA_SIG_free(sign);
	EVP_MD_CTX_free(md_ctx);
	return ret;
}


#ifdef ECC_UTILS_SELF_TEST
int test_ecdh()
{
	size_t shared_secret_own_len, shared_secret_peer_len;
	uint8_t *shared_secret_own, *shared_secret_peer;
	void *eckey_own, *eckey_peer;
	int ret = 0;


	printf(" Generate key (for self)\n");
	printf("=================================================\n");
	eckey_own = ecc_key_gen("prime256v1");
	if (eckey_own)
		ecc_key_print("Self: Generated Key", eckey_own);

	printf("\n");
	printf(" Generate key (for peer)\n");
	printf("=================================================\n");
	eckey_peer = ecc_key_gen("prime256v1");
	if (eckey_peer)
		ecc_key_print("Peer: Generated Key", eckey_peer);


	printf("\n");
	printf(" Perform ECDH (self, peer) \n");
	printf("=================================================\n");
	/* generate our side shared-secret */
	ret = ecc_ecdh(eckey_own, eckey_peer, &shared_secret_own, &shared_secret_own_len);
	if (ret) {
		printf("%s: own ecc_ecdh() ret = %d\n", __func__, ret);
		return -1;
	}

	/* generate peer side shared-secret */
	ret = ecc_ecdh(eckey_peer, eckey_own, &shared_secret_peer, &shared_secret_peer_len);
	if (ret) {
		printf("%s: peer ecc_ecdh() ret = %d\n", __func__, ret);
		return -1;
	}

	if (!memcmp(shared_secret_own, shared_secret_peer, shared_secret_own_len))
		printf("ECDH: OK\n");
	else
		printf("ECDH: NOK\n");

	assert(!memcmp(shared_secret_own, shared_secret_peer, shared_secret_own_len));
	return 0;
}

int test_ecdsa_sign_verify()
{
	const char *text = "Sample Text";
	uint8_t text_sha256[] = {	/* SHA256("Sample Text") */
				 0x35, 0xae, 0x88, 0x55,
				 0xdb, 0x11, 0x2a, 0x23,
				 0x4a, 0x69, 0xef, 0x68,
				 0xcf, 0x73, 0x72, 0x35,
				 0x71, 0x24, 0xb1, 0x8b,
				 0xe0, 0x29, 0xef, 0xcf,
				 0x43, 0x45, 0xc2, 0xbf,
				 0x1d, 0xa7, 0xb7, 0xb5
				};
	uint8_t *sign;
	size_t signlen;

	/* DER encoded pub EC key used for verify */
	uint8_t pub_signkey[] = {
			0x30, 0x39, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86,
			0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
			0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03,
			0x22, 0x00, 0x03, 0xd0, 0x1a, 0x4f, 0x23, 0x70,
			0x26, 0x4e, 0x54, 0xd8, 0xd0, 0x6a, 0x03, 0x25,
			0x24, 0xf9, 0x94, 0x00, 0x67, 0xef, 0xa1, 0x28,
			0xda, 0x53, 0xd2, 0xc0, 0x91, 0x8a, 0xfb, 0xf4,
			0x08, 0xd0, 0xc4 };

	/* DER encoded private EC keypair used for signing */
	uint8_t priv_signkey[] = {
			0x30, 0x57, 0x02, 0x01, 0x01, 0x04, 0x20, 0xd6,
			0x4f, 0xa9, 0xc8, 0x94, 0xa8, 0x47, 0x4f, 0xa6,
			0x88, 0xb1, 0x4a, 0x41, 0x95, 0x71, 0x68, 0xbf,
			0xc7, 0x79, 0x25, 0x3e, 0xa7, 0x0d, 0x57, 0xf3,
			0x4f, 0x1e, 0x16, 0xa2, 0xd8, 0x4c, 0x59, 0xa0,
			0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
			0x03, 0x01, 0x07, 0xa1, 0x24, 0x03, 0x22, 0x00,
			0x03, 0xd0, 0x1a, 0x4f, 0x23, 0x70, 0x26, 0x4e,
			0x54, 0xd8, 0xd0, 0x6a, 0x03, 0x25, 0x24, 0xf9,
			0x94, 0x00, 0x67, 0xef, 0xa1, 0x28, 0xda, 0x53,
			0xd2, 0xc0, 0x91, 0x8a, 0xfb, 0xf4, 0x08, 0xd0,
			0xc4 };

	void *priv_skey = ecc_key_gen_from_data(priv_signkey, sizeof(priv_signkey), 0);
	void *pub_skey = ecc_key_gen_from_data(pub_signkey, sizeof(pub_signkey), 1);
	size_t rlen, slen;
	uint8_t *r, *s;
	int verified = 0;
	int ret;

	if (!priv_skey || !pub_skey) {
		printf("%s: Error getting private or pub key\n", __func__);
		return -1;
	}

	sign = ecdsa_sign(priv_skey, text_sha256, 32, &signlen);
	if (sign) {
		printf("ECDSA signature (len = %zd)\n", signlen);
		bufprintf("Signature = ", sign, signlen);
	} else {
		printf("%s: Failed to sign\n", __func__);
		return -1;
	}

	ret = ecdsa_signature_extract_r_s(sign, signlen, &r, &rlen, &s, &slen);
	if (ret) {
		printf("%s: Failed to extract r,s from signature\n", __func__);
		return -1;
	}

	bufprintf("r = ", r, rlen);
	bufprintf("s = ", s, slen);
	verified = ecdsa_verify_signature_r_s(pub_skey, text_sha256, 32, r, rlen, s, slen);
	printf("Signature verified %s\n", verified == 1 ? "OK" : "NOK");

	assert(verified == 1);
	return 0;
}

int main()
{
	test_ecdh();
	test_ecdsa_sign_verify();

#if 0
	printf("\n");
	printf(" Build EC key for curve with x,y\n");
	printf("=================================================\n");

	/* generate key for curve from x,y */
	uint8_t pub_x[] = {
	    0xcf, 0x20, 0xfb, 0x9a, 0x1d, 0x11, 0x6c, 0x5e,
	    0x9f, 0xec, 0x38, 0x87, 0x6c, 0x1d, 0x2f, 0x58,
	    0x47, 0xab, 0xa3, 0x9b, 0x79, 0x23, 0xe6, 0xeb,
	    0x94, 0x6f, 0x97, 0xdb, 0xa3, 0x7d, 0xbd, 0xe5,
	};

	uint8_t pub_y[] = {
	    0x26, 0xca, 0x07, 0x17, 0x8d, 0x26, 0x75, 0xff,
	    0xcb, 0x8e, 0xb6, 0x84, 0xd0, 0x24, 0x02, 0x25,
	    0x8f, 0xb9, 0x33, 0x6e, 0xcf, 0x12, 0x16, 0x2f,
	    0x5c, 0xcd, 0x86, 0x71, 0xa8, 0xbf, 0x1a, 0x47
	};

	ecc_key_gen_from_xy("prime256v1", pub_x, pub_y, 32);

	printf("=====================DONE========================\n");
#endif

}
#endif
