/*
 * cryptutil.c - crypto utility functions
 *
 * Copyright (C) 2021 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/hmac.h>
#include <openssl/cmac.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/params.h>
#include <openssl/param_build.h>
#else
#include <openssl/dh.h>
#endif

#include "easy.h"

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE	16
#endif

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
int LIBEASY_API PLATFORM_GENERATE_DH_KEY_PAIR(uint8_t **priv, size_t *privlen,
				  uint8_t **pub, size_t *publen)
{
	EVP_PKEY_CTX *ctx = NULL;
	uint8_t *privkey = NULL;
	uint8_t *pubkey = NULL;
	EVP_PKEY *pkey = NULL;
	OSSL_PARAM params[2];
	BIGNUM *bn = NULL;
	//void *ret = NULL;
	int ret = -1;

	if (!priv || !privlen || !pub || !publen) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	*privlen = 0;
	*publen = 0;
	*priv = NULL;
	*pub = NULL;

	params[0] = OSSL_PARAM_construct_utf8_string("group", "modp_1536", 0);
	params[1] = OSSL_PARAM_construct_end();

	ctx = EVP_PKEY_CTX_new_from_name(NULL, "DH", NULL);
	if (!ctx)
		goto err;

	if (EVP_PKEY_keygen_init(ctx) != 1)
		goto err;

	if (EVP_PKEY_CTX_set_params(ctx, params) != 1)
		goto err;

	if (EVP_PKEY_generate(ctx, &pkey) != 1)
		goto err;

	if (EVP_PKEY_get_bn_param(pkey, "priv", &bn) != 1)
		goto err;

	*privlen = BN_num_bytes(bn);
	if (*privlen == 0)
		goto err;

	if (EVP_PKEY_get_octet_string_param(pkey, "encoded-pub-key", NULL, 0, publen) < 0)
		goto err;

	if (*publen == OSSL_PARAM_UNMODIFIED)
		goto err;

	privkey = calloc(1, *privlen);
	if (!privkey) {
		goto err;
	}

	if (EVP_PKEY_get1_encoded_public_key(pkey, &pubkey) != *publen) {
		printf("OpenSSL: failed: %s", ERR_error_string(ERR_get_error(), NULL));
		free(pubkey);
		free(privkey);
		goto err;
	}

	BN_bn2bin(bn, privkey);
	*priv = privkey;
	*pub = pubkey;
	//ret = pkey;
	ret = 0;
err:
	BN_clear_free(bn);
	//EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	return ret;
}

int LIBEASY_API PLATFORM_COMPUTE_DH_SHARED_SECRET(uint8_t **shkey, size_t *shkeylen,
				      uint8_t *peer_pub, size_t peer_publen,
				      uint8_t *local_priv, size_t local_privlen,
				      uint8_t *local_pub, size_t local_publen)
{
	EVP_PKEY *pkey = NULL, *peerkey = NULL;
	EVP_PKEY_CTX *ctx, *fctx;
	OSSL_PARAM *params = NULL;
	OSSL_PARAM_BLD *pbld;
	BIGNUM *pbn, *xbn;
	size_t skeylen;
	uint8_t *skey;
	int ret = -1;


	pbn = BN_bin2bn(local_priv, local_privlen, NULL);
	if (!pbn)
		return -1;

	xbn = BN_bin2bn(local_pub, local_publen, NULL);
	if (!xbn) {
		BN_clear_free(pbn);
		return -1;
	}

	pbld = OSSL_PARAM_BLD_new();
	OSSL_PARAM_BLD_push_utf8_string(pbld, "group", "modp_1536", 0);
	OSSL_PARAM_BLD_push_BN(pbld, "priv", pbn);
	OSSL_PARAM_BLD_push_BN(pbld, "pub", xbn);
	params = OSSL_PARAM_BLD_to_param(pbld);

	fctx = EVP_PKEY_CTX_new_from_name(NULL, "DH", NULL);
	if (!fctx)
		goto err;

	if (EVP_PKEY_fromdata_init(fctx) != 1)
		goto err;

	if (EVP_PKEY_fromdata(fctx, &pkey, EVP_PKEY_KEYPAIR, params) != 1) {
		printf("Error in fromdata\n");
		goto err;
	}

	peerkey = EVP_PKEY_new();
	if (!peerkey)
		goto err;

	if (EVP_PKEY_copy_parameters(peerkey, pkey) != 1)
		goto err;

	if (EVP_PKEY_set1_encoded_public_key(peerkey, peer_pub, peer_publen) <= 0)
		goto err;

	ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!ctx)
		goto err;

	if (EVP_PKEY_derive_init(ctx) <= 0)
		goto err;

	if (EVP_PKEY_derive_set_peer(ctx, peerkey) <= 0)
		goto err;

	if (EVP_PKEY_derive(ctx, NULL, &skeylen) <= 0)
		goto err;

	skey = OPENSSL_malloc(skeylen);
	if (!skey)
		goto err;

	if (EVP_PKEY_derive(ctx, skey, &skeylen) <= 0) {
		OPENSSL_free(skey);
		goto err;
	}

	printf("SharedKey (len = %zu): ", skeylen);
	for (int i = 0; i < skeylen; i++)
		printf("%02x", skey[i]);
	printf("\n");

	*shkey = skey;
	*shkeylen = skeylen;
	ret = 0;
err:
	BN_clear_free(pbn);
	BN_clear_free(xbn);
	return ret;
}

int LIBEASY_API PLATFORM_SHA256(int num, const uint8_t *addr[], const size_t *len, uint8_t *digest)
{
	EVP_MD_CTX *ctx = NULL;
	EVP_MD *md = NULL;
	unsigned int olen;
	int ret = -1;
	int i;

	ctx = EVP_MD_CTX_new();
	if (!ctx)
		goto err;

	md = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (!md)
		goto err;

	if (!EVP_DigestInit_ex(ctx, md, NULL))
		goto err;

	for (i = 0; i < num; i++) {
		if (!EVP_DigestUpdate(ctx, addr[i], len[i]))
			goto err;
	}

	if (!EVP_DigestFinal_ex(ctx, digest, &olen))
		goto err;

#ifdef CRYPT_DEBUG
	BIO_dump_fp(stdout, digest, olen);
#endif
	ret = 0;

err:
	EVP_MD_free(md);
	EVP_MD_CTX_free(ctx);
	return ret;
}


int LIBEASY_API PLATFORM_HMAC_SHA256(const uint8_t *key, size_t keylen, int num,
			 const uint8_t *addr[], const size_t *len, uint8_t *hmac)
{
	EVP_MAC_CTX *ctx = NULL;
	OSSL_PARAM params[2];
	EVP_MAC *mac =NULL;
	int ret = -1;
	size_t olen;
	int i;


	mac = EVP_MAC_fetch(NULL, "HMAC", NULL);
	if (!mac)
		goto err;

	ctx = EVP_MAC_CTX_new(mac);
	if (!ctx)
		goto err;

	params[0] = OSSL_PARAM_construct_utf8_string("digest", "SHA256", 0);
	params[1] = OSSL_PARAM_construct_end();

	if (!EVP_MAC_init(ctx, (const unsigned char *)key, keylen, params))
		goto err;

	for (i = 0; i < num; i++) {
		if (!EVP_MAC_update(ctx, addr[i], len[i]))
			goto err;
	}

	if (!EVP_MAC_final(ctx, hmac, &olen, 32))
		goto err;

	ret = 0;

err:
	EVP_MAC_CTX_free(ctx);
	EVP_MAC_free(mac);
	return ret;
}
#else	/* ! OPENSSL_VERSION_NUMBER >= 0x30000000L */

static unsigned char dh1536_p[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2,
	0x21, 0x68, 0xC2, 0x34, 0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1,
	0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74, 0x02, 0x0B, 0xBE, 0xA6,
	0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
	0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
	0xF2, 0x5F, 0x14, 0x37, 0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45,
	0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6, 0xF4, 0x4C, 0x42, 0xE9,
	0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
	0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
	0x7C, 0x4B, 0x1F, 0xE6, 0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D,
	0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05, 0x98, 0xDA, 0x48, 0x36,
	0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
	0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
	0x20, 0x85, 0x52, 0xBB, 0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D,
	0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04, 0xF1, 0x74, 0x6C, 0x08,
	0xCA, 0x23, 0x73, 0x27, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static unsigned char dh1536_g[] = { 0x02 };


int LIBEASY_API PLATFORM_GENERATE_DH_KEY_PAIR(uint8_t **priv, size_t *priv_len,
				  uint8_t **pub, size_t *pub_len)
{
	DH *dh;

	if (priv == NULL || priv_len == NULL || pub == NULL || pub_len == NULL) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	dh = DH_new();
	if (dh == NULL) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	// Convert binary to BIGNUM format
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
	BIGNUM * dhp_bn, *dhg_bn;

	dhp_bn = BN_bin2bn(dh1536_p, sizeof(dh1536_p), NULL);
	dhg_bn = BN_bin2bn(dh1536_g, sizeof(dh1536_g), NULL);

	if (dhp_bn == NULL || dhg_bn == NULL || !DH_set0_pqg(dh, dhp_bn, NULL, dhg_bn)) {
		DH_free(dh);
		BN_free(dhp_bn);
		BN_free(dhg_bn);
		fprintf(stderr, "#### problem detected");
		return -1;
	}
#else
	dh->p = BN_bin2bn(dh1536_p, sizeof(dh1536_p), NULL);
	if (dh->p == NULL) {
		DH_free(dh);
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	dh->g = BN_bin2bn(dh1536_g, sizeof(dh1536_g), NULL);
	if (dh->g == NULL) {
		DH_free(dh);
		fprintf(stderr, "#### problem detected");
		return -1;
	}
#endif
	// Obtain key pair
	//
	if (DH_generate_key(dh) == 0) {
		DH_free(dh);
		fprintf(stderr, "#### problem detected");
		return -1;
	}
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
	const BIGNUM * pv = DH_get0_priv_key(dh);
	*priv_len = BN_num_bytes(pv);
	*priv = (uint8_t *) malloc(sizeof(uint8_t) * (*priv_len + 1));
	if (*priv == NULL) {
		fprintf(stderr, "Out of memory!");
		return -1;
	}
	BN_bn2bin(pv, *priv);

	const BIGNUM *pu = DH_get0_pub_key(dh);
	*pub_len = BN_num_bytes(pu);
	*pub = (uint8_t *) malloc(sizeof(uint8_t) * (*pub_len + 1));
	if (*pub == NULL) {
		fprintf(stderr, "Out of memory!");
		return -1;
	}

	BN_bn2bin(pu, *pub);
#else
	*priv_len = BN_num_bytes(dh->priv_key);
	*priv = (uint8_t *) malloc(*priv_len);
	if (*priv == NULL) {
		fprintf(stderr, "Out of memory!");
		return -1;
	}

	BN_bn2bin(dh->priv_key, *priv);

	*pub_len = BN_num_bytes(dh->pub_key);
	*pub = (uint8_t *) malloc(*pub_len);
	if (*pub == NULL) {
		fprintf(stderr, "Out of memory!");
		return -1;
	}

	BN_bn2bin(dh->pub_key, *pub);
#endif
	DH_free(dh);
	// NOTE: This internally frees "dh->p" and "dh->q", thus no need for us
	// to do anything else.

	return 0;
}

int LIBEASY_API PLATFORM_COMPUTE_DH_SHARED_SECRET(uint8_t **shared_secret, size_t *shared_secret_len,
					uint8_t *remote_pub, size_t remote_pub_len,
					uint8_t *local_priv, size_t local_priv_len,
					uint8_t *local_pub, size_t local_publen)
{
	BIGNUM *pub_key;
	size_t rlen;
	int keylen;
	DH *dh;


	(void)local_pub;
	(void)local_publen;

	if (!shared_secret || !shared_secret_len || !remote_pub || !local_priv) {
		fprintf(stderr, "Derive DH-sharedkey: invalid args\n");
		return -1;
	}

	dh = DH_new();
	if (!dh) {
		fprintf(stderr, "Derive DH-sharedkey: dh_new() failed\n");
		return -1;
	}

#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
	BIGNUM *dhp_bn, *dhg_bn, *priv_key;

	dhp_bn = BN_bin2bn(dh1536_p, sizeof(dh1536_p), NULL);
	dhg_bn = BN_bin2bn(dh1536_g, sizeof(dh1536_g), NULL);

	if (!dhp_bn || !dhg_bn || !DH_set0_pqg(dh, dhp_bn, NULL, dhg_bn)) {
		DH_free(dh);
		BN_free(dhp_bn);
		BN_free(dhg_bn);
		fprintf(stderr, "Derive DH-sharedkey: dh_set0_pqg() failed\n");
		return -1;
	}
	pub_key = BN_bin2bn(remote_pub, remote_pub_len, NULL);
	if (!pub_key) {
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: peer pubkey bin2bn() failed\n");
		return -1;
	}
	priv_key = BN_bin2bn(local_priv, local_priv_len, NULL);
	if (!priv_key) {
		BN_clear_free(priv_key);
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: own privkey bin2bn() failed\n");
		return -1;
	}
	DH_set0_key(dh, pub_key, priv_key);
#else
	dh->p = BN_bin2bn(dh1536_p, sizeof(dh1536_p), NULL);
	if (!dh->p) {
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: bin2bn(dh1536_p) failed\n");
		return -1;
	}
	dh->g = BN_bin2bn(dh1536_g, sizeof(dh1536_g), NULL);
	if (!dh->g) {
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: bin2bn(dh1536_g) failed\n");
		return -1;
	}
	pub_key = BN_bin2bn(remote_pub, remote_pub_len, NULL);
	if (!pub_key) {
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: peer pubkey bin2bn() failed\n");
		return -1;
	}
	dh->priv_key = BN_bin2bn(local_priv, local_priv_len, NULL);
	if (!dh->priv_key) {
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: own privkey bin2bn() failed\n");
		return -1;
	}
#endif
	rlen = DH_size(dh);
	*shared_secret = (uint8_t *) malloc(rlen);
	if (*shared_secret == NULL) {
		fprintf(stderr, "Derive DH-sharedkey: -ENOMEM\n");
		return -1;
	}


	// Compute the shared secret and save it in the output buffer
	//
	keylen = DH_compute_key(*shared_secret, pub_key, dh);
	if (keylen < 0) {
		*shared_secret_len = 0;
		free(*shared_secret);
		*shared_secret = NULL;
		BN_clear_free(pub_key);
		DH_free(dh);
		fprintf(stderr, "Derive DH-sharedkey: compute_key() failed\n");
		return -1;
	} else {
		*shared_secret_len = (uint16_t) keylen;
	}

#if OPENSSL_VERSION_NUMBER < 0x1010000fL
	BN_clear_free(pub_key);
#endif
	DH_free(dh);

	return 0;
}

int LIBEASY_API PLATFORM_SHA256(int num_elem, const uint8_t *addr[], const size_t *len, uint8_t *digest)
{
	int res;
	unsigned int mac_len;
	EVP_MD_CTX *ctx;

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	ctx = EVP_MD_CTX_new();
	if (!ctx) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
#else
	EVP_MD_CTX ctx_aux;

	ctx = &ctx_aux;

	EVP_MD_CTX_init(ctx);
#endif

	res = 1;

	if (!EVP_DigestInit_ex(ctx, EVP_sha256(), NULL)) {
		res = 0;
	}

	if (res == 1) {
		int i;

		for (i = 0; i < num_elem; i++) {
			if (!EVP_DigestUpdate(ctx, addr[i], len[i])) {
				res = 0;
				break;
			}
		}
	}

	if (res == 1) {
		if (!EVP_DigestFinal(ctx, digest, &mac_len)) {
			res = 0;
		}
	}
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	EVP_MD_CTX_free(ctx);
#endif

	return !res;
}

int LIBEASY_API PLATFORM_HMAC_SHA256(const uint8_t *key, size_t keylen, int num_elem,
			 const uint8_t *addr[], const size_t *len, uint8_t *hmac)
{
	HMAC_CTX *ctx;
	unsigned int mdlen = 32;
	int i;

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	ctx = HMAC_CTX_new();
	if (!ctx) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
#else
	HMAC_CTX ctx_aux;

	ctx = &ctx_aux;

	HMAC_CTX_init(ctx);
#endif

	HMAC_Init_ex(ctx, key, keylen, EVP_sha256(), NULL);

	for (i = 0; i < num_elem; i++) {
		HMAC_Update(ctx, addr[i], len[i]);
	}

	HMAC_Final(ctx, hmac, &mdlen);

#if OPENSSL_VERSION_NUMBER >= 0x10100000L
	HMAC_CTX_free(ctx);
#else
	HMAC_CTX_cleanup(ctx);
#endif

	return 0;
}
#endif	/* OPENSSL_VERSION_NUMBER < 0x30000000L */

int LIBEASY_API PLATFORM_AES_ENCRYPT(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len)
{
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
	EVP_CIPHER_CTX * ctx;

	int clen, len;
	uint8_t buf[AES_BLOCK_SIZE];

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	clen = data_len;
	if (EVP_EncryptUpdate(ctx, data, &clen, data, data_len) != 1 || clen != (int)data_len) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	len = sizeof(buf);
	if (EVP_EncryptFinal_ex(ctx, buf, &len) != 1 || len != 0) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_free(ctx);
#else
	EVP_CIPHER_CTX ctx;

	int clen, len;
	uint8_t buf[AES_BLOCK_SIZE];

	EVP_CIPHER_CTX_init(&ctx);

	if (EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	clen = data_len;
	if (EVP_EncryptUpdate(&ctx, data, &clen, data, data_len) != 1 || clen != (int)data_len) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	len = sizeof(buf);
	if (EVP_EncryptFinal_ex(&ctx, buf, &len) != 1 || len != 0) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
#endif

	return 0;
}

int LIBEASY_API PLATFORM_AES_DECRYPT(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len)
{
#if OPENSSL_VERSION_NUMBER >= 0x1010000fL
	EVP_CIPHER_CTX * ctx;

	int plen, len;
	uint8_t buf[AES_BLOCK_SIZE];

	ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	plen = data_len;
	if (EVP_DecryptUpdate(ctx, data, &plen, data, data_len) != 1 || plen != (int)data_len) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	len = sizeof(buf);
	if (EVP_DecryptFinal_ex(ctx, buf, &len) != 1 || len != 0) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_free(ctx);
#else
	EVP_CIPHER_CTX ctx;

	int plen, len;
	uint8_t buf[AES_BLOCK_SIZE];

	EVP_CIPHER_CTX_init(&ctx);

	if (EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_set_padding(&ctx, 0);

	plen = data_len;
	if (EVP_DecryptUpdate(&ctx, data, &plen, data, data_len) != 1 || plen != (int)data_len) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}

	len = sizeof(buf);
	if (EVP_DecryptFinal_ex(&ctx, buf, &len) != 1 || len != 0) {
		fprintf(stderr, "#### problem detected");
		return -1;
	}
	EVP_CIPHER_CTX_cleanup(&ctx);
#endif
	return 0;
}

int LIBEASY_API AES_WRAP_128(uint8_t *key, uint8_t *plain, size_t plen,
		 uint8_t *cipher, size_t *clen)
{
	EVP_CIPHER_CTX *ctx;
	int ret = -1;
	int len = 0;


	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		return -1;

	EVP_CIPHER_CTX_set_flags(ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);

	if (EVP_EncryptInit_ex(ctx, EVP_aes_128_wrap_pad(), NULL, key, NULL) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	if (EVP_EncryptUpdate(ctx, cipher, (int *)clen, plain, (int)plen) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	if (EVP_EncryptFinal_ex(ctx, cipher + *clen, &len) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	*clen += len;
	ret = 0;

out:
	EVP_CIPHER_CTX_free(ctx);
	return ret;
}

int LIBEASY_API AES_UNWRAP_128(uint8_t *key, uint8_t *cipher, size_t clen,
		   uint8_t *plain, size_t *plen)
{
	EVP_CIPHER_CTX *ctx;
	int ret = -1;
	int len;


	ctx = EVP_CIPHER_CTX_new();
	if (!ctx)
		return -1;

	EVP_CIPHER_CTX_set_flags(ctx, EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);

	if (EVP_DecryptInit_ex(ctx, EVP_aes_128_wrap_pad(), NULL, key, NULL) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	if (EVP_DecryptUpdate(ctx, plain, (int *)plen, cipher, (int)clen) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	if (EVP_DecryptFinal_ex(ctx, plain + *plen, &len) != 1) {
		fprintf(stderr, "%s: %d -- error --\n", __func__, __LINE__);
		goto out;
	}

	ret = 0;
	*plen += len;

out:
	EVP_CIPHER_CTX_free(ctx);
	return ret;
}

int LIBEASY_API omac1_aes_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		     const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	EVP_MAC_CTX *ctx = NULL;
	EVP_MAC *emac;
	int ret = -1;
	size_t outlen, i;
	OSSL_PARAM params[2];
	char *cipher = NULL;

	emac = EVP_MAC_fetch(NULL, "CMAC", NULL);

	if (key_len == 32)
		cipher = "aes-256-cbc";
	else if (key_len == 24)
		cipher = "aes-192-cbc";
	else if (key_len == 16)
		cipher = "aes-128-cbc";

	params[0] = OSSL_PARAM_construct_utf8_string("cipher", cipher, 0);
	params[1] = OSSL_PARAM_construct_end();

	if (!emac || !cipher ||
	    !(ctx = EVP_MAC_CTX_new(emac)) ||
	    EVP_MAC_init(ctx, key, key_len, params) != 1)
		goto fail;

	for (i = 0; i < num_elem; i++) {
		if (!EVP_MAC_update(ctx, addr[i], len[i]))
			goto fail;
	}
	if (EVP_MAC_final(ctx, mac, &outlen, 16) != 1 || outlen != 16)
		goto fail;

	ret = 0;
fail:
	EVP_MAC_CTX_free(ctx);
	return ret;
#else /* OpenSSL version >= 3.0 */
	CMAC_CTX *ctx;
	int ret = -1;
	size_t outlen, i;

	ctx = CMAC_CTX_new();
	if (ctx == NULL)
		return -1;

	if (key_len == 32) {
		if (!CMAC_Init(ctx, key, 32, EVP_aes_256_cbc(), NULL))
			goto fail;
	} else if (key_len == 24) {
		if (!CMAC_Init(ctx, key, 24, EVP_aes_192_cbc(), NULL))
			goto fail;
	} else if (key_len == 16) {
		if (!CMAC_Init(ctx, key, 16, EVP_aes_128_cbc(), NULL))
			goto fail;
	} else {
		goto fail;
	}
	for (i = 0; i < num_elem; i++) {
		if (!CMAC_Update(ctx, addr[i], len[i]))
			goto fail;
	}
	if (!CMAC_Final(ctx, mac, &outlen) || outlen != 16)
		goto fail;

	ret = 0;
fail:
	CMAC_CTX_free(ctx);
	return ret;
#endif /* OpenSSL version >= 3.0 */
}

static const EVP_CIPHER * aes_get_evp_cipher(size_t keylen)
{
	switch (keylen) {
	case 16:
		return EVP_aes_128_ecb();
	case 24:
		return EVP_aes_192_ecb();
	case 32:
		return EVP_aes_256_ecb();
	}

	return NULL;
}

void * aes_encrypt_init(const uint8_t *key, size_t len)
{
	EVP_CIPHER_CTX *ctx;
	const EVP_CIPHER *type;

	type = aes_get_evp_cipher(len);
	if (!type) {
		fprintf(stderr, "%s: Unsupported len = %zu\n", __func__, len);
		return NULL;
	}

	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
		return NULL;
	if (EVP_EncryptInit_ex(ctx, type, NULL, key, NULL) != 1) {
		EVP_CIPHER_CTX_free(ctx);
		return NULL;
	}
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	return ctx;
}

void aes_encrypt_deinit(void *ctx)
{
	EVP_CIPHER_CTX *c = ctx;
	uint8_t buf[16];
	int len = sizeof(buf);

	if (EVP_EncryptFinal_ex(c, buf, &len) != 1) {
		fprintf(stderr, "OpenSSL: EVP_EncryptFinal_ex failed: %s\n",
			ERR_error_string(ERR_get_error(), NULL));
	}

	if (len != 0) {
		fprintf(stderr, "OpenSSL: Unexpected padding length %d in AES encrypt\n", len);
	}

	EVP_CIPHER_CTX_free(c);
}

int aes_encrypt(void *ctx, const uint8_t *plain, uint8_t *crypt) /* Flawfinder: ignore */
{
	EVP_CIPHER_CTX *c = ctx;
	int clen = 16;

	if (EVP_EncryptUpdate(c, crypt, &clen, plain, 16) != 1) { /* Flawfinder: ignore */
		fprintf(stderr, "OpenSSL: EVP_EncryptUpdate failed: %s\n",
			ERR_error_string(ERR_get_error(), NULL));
		return -1;
	}
	return 0;
}



/**
 * aes_ctr_encrypt - AES-128/192/256 CTR mode encryption
 * @key: Key for encryption (key_len bytes)
 * @key_len: Length of the key (16, 24, or 32 bytes)
 * @nonce: Nonce for counter mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes
 * Returns: 0 on success, -1 on failure
 */
int LIBEASY_API aes_ctr_encrypt(const uint8_t *key, size_t key_len, const uint8_t *nonce,
		    uint8_t *data, size_t data_len)
{
	void *ctx;
	size_t j, len, left = data_len;
	int i;
	uint8_t *pos = data;
	uint8_t counter[AES_BLOCK_SIZE], buf[AES_BLOCK_SIZE];

	ctx = aes_encrypt_init(key, key_len);
	if (ctx == NULL)
		return -1;
	memcpy(counter, nonce, AES_BLOCK_SIZE);

	while (left > 0) {
		aes_encrypt(ctx, counter, buf);

		len = (left < AES_BLOCK_SIZE) ? left : AES_BLOCK_SIZE;
		for (j = 0; j < len; j++)
			pos[j] ^= buf[j];
		pos += len;
		left -= len;

		for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
			counter[i]++;
			if (counter[i])
				break;
		}
	}
	aes_encrypt_deinit(ctx);
	return 0;
}

/**
 * sha256_prf_bits - IEEE Std 802.11-2012, 11.6.1.7.2 Key derivation function
 * @key: Key for KDF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bits of key to generate
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key. If the requested buf_len is not divisible by eight, the least
 * significant 1-7 bits of the last octet in the output are not part of the
 * requested output.
 */
int sha256_prf_bits(const uint8_t *key, size_t key_len, const char *label,
		    const uint8_t *data, size_t data_len, uint8_t *buf,
		    size_t buf_len_bits)
{
	uint16_t counter = 1;
	size_t pos, plen;
	uint8_t hash[SHA256_MAC_LEN];
	const uint8_t *addr[4];
	size_t len[4];
	uint8_t counter_le[2], length_le[2];
	size_t buf_len = (buf_len_bits + 7) / 8;

	addr[0] = counter_le;
	len[0] = 2;
	addr[1] = (uint8_t *) label;
	len[1] = strlen(label);
	addr[2] = data;
	len[2] = data_len;
	addr[3] = length_le;
	len[3] = sizeof(length_le);

	buf_put_le16(length_le, buf_len_bits);
	pos = 0;
	while (pos < buf_len) {
		plen = buf_len - pos;
		buf_put_le16(counter_le, counter);
		if (plen >= SHA256_MAC_LEN) {
			if (PLATFORM_HMAC_SHA256(key, key_len, 4, addr, len, &buf[pos]) < 0)
				return -1;

			pos += SHA256_MAC_LEN;
		} else {
			if (PLATFORM_HMAC_SHA256(key, key_len, 4, addr, len, hash) < 0)
				return -1;
			memcpy(&buf[pos], hash, plen);
			pos += plen;
			break;
		}
		counter++;
	}

	/*
	 * Mask out unused bits in the last octet if it does not use all the
	 * bits.
	 */
	if (buf_len_bits % 8) {
		uint8_t mask = 0xff << (8 - buf_len_bits % 8);
		buf[pos - 1] &= mask;
	}

	memset(hash, 0, sizeof(hash));

	return 0;
}

/**
 * sha256_prf - SHA256-based Pseudo-Random Function
 * @key: Key for PRF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bytes of key to generate
 * Returns: 0 on success, -1 on failure
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key.
 */
int LIBEASY_API SHA256_PRF(const uint8_t *key, size_t key_len, const char *label,
		const uint8_t *data, size_t data_len, uint8_t *buf, size_t buf_len)
{
	return sha256_prf_bits(key, key_len, label, data, data_len, buf,
			       buf_len * 8);
}

/**
 * hmac_sha256_kdf - HMAC-SHA256 based KDF (RFC 5295)
 * @secret: Key for KDF
 * @secret_len: Length of the key in bytes
 * @label: A unique label for each purpose of the KDF or %NULL to select
 *	RFC 5869 HKDF-Expand() with arbitrary seed (= info)
 * @seed: Seed value to bind into the key
 * @seed_len: Length of the seed
 * @out: Buffer for the generated pseudo-random key
 * @outlen: Number of bytes of key to generate
 * Returns: 0 on success, -1 on failure.
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key in ERP. This KDF is defined in RFC 5295, Chapter 3.1.2. When used
 * with label = NULL and seed = info, this matches HKDF-Expand() defined in
 * RFC 5869, Chapter 2.3.
 */
int LIBEASY_API hmac_sha256_kdf(const uint8_t *secret, size_t secret_len,
		    const char *label, const uint8_t *seed, size_t seed_len,
		    uint8_t *out, size_t outlen)
{
	uint8_t T[SHA256_MAC_LEN];
	uint8_t iter = 1;
	const unsigned char *addr[4];
	size_t len[4];
	size_t pos, clen;

	addr[0] = T;
	len[0] = SHA256_MAC_LEN;
	if (label) {
		addr[1] = (const unsigned char *) label;
		len[1] = strlen(label) + 1;
	} else {
		addr[1] = (const uint8_t *) "";
		len[1] = 0;
	}
	addr[2] = seed;
	len[2] = seed_len;
	addr[3] = &iter;
	len[3] = 1;


	if (PLATFORM_HMAC_SHA256(secret, secret_len, 3, &addr[1], &len[1], T) < 0)
		return -1;

	pos = 0;
	for (;;) {
		clen = outlen - pos;
		if (clen > SHA256_MAC_LEN)
			clen = SHA256_MAC_LEN;
		memcpy(out + pos, T, clen);
		pos += clen;

		if (pos == outlen)
			break;

		if (iter == 255) {
			memset(out, 0, outlen);
			memset(T, 0, SHA256_MAC_LEN);	//forced_memzero
			return -1;
		}
		iter++;

		if (PLATFORM_HMAC_SHA256(secret, secret_len, 4, addr, len, T) < 0) {
			memset(out, 0, outlen);
			memset(T, 0, SHA256_MAC_LEN);	//forced_memzero
			return -1;
		}
	}

	memset(T, 0, SHA256_MAC_LEN);	//forced_memzero
	return 0;
}
