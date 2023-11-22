/*
 * ecc_cryptutil.h - EC crypto functions.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 */

#ifndef ECC_CRYPTUTIL_H
#define ECC_CRYPTUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

int ecc_key_get_nid(void *key);
char *ecc_key_get_curve_name(void *key, char *curve_name);
uint8_t *ecc_key_get_private(void *key, size_t *outlen);
uint8_t *ecc_key_get_encoded_pubkey(void *key, size_t *outlen);
uint8_t *ecc_key_get_pubkey(void *key, size_t *outlen);
uint8_t *ecc_key_get_spki(void *key, size_t *olen);
uint8_t *ecc_key_get_privkey_der(void *key, size_t *olen);
uint8_t *ecc_key_get_x(void *key, size_t *outlen);
uint8_t *ecc_key_get_y(void *key, size_t *outlen);
uint8_t *ecc_key_get_point_format(void *key, size_t *outlen);

void *ecc_key_gen(const char *curve);
void *ecc_key_gen_from_xy(char *curve, const uint8_t *bx, const uint8_t *by, size_t len);
void *ecc_key_gen_from_spki(uint8_t *spki, size_t len);
void *ecc_key_gen_from_data(uint8_t *data, size_t len, int is_public);

void ecc_key_free(void *key);
bool ecc_key_check(void *key);
int is_ecc_key_equal(void *key1, void *key2);

void ecc_key_dump(const char *title, const void *key);
void ecc_key_print(const char *title, void *key);

int ecc_ecdh(void *key, void *peerkey, uint8_t **out, size_t *secret_len);

uint8_t *ecdsa_sign(void *key, const uint8_t *data, size_t len, size_t *olen);

int ecdsa_signature_extract_r_s(const uint8_t *sig, size_t siglen,
				uint8_t **r, size_t *rlen,
				uint8_t **s, size_t *slen);

int ecdsa_verify_signature_r_s(void *key, const uint8_t *data, size_t datalen,
			       const uint8_t *r, size_t r_len,
			       const uint8_t *s, size_t s_len);

/* deprecate */
int ecc_key_get_pubkey_point(void *key, int prefix, uint8_t **pub, size_t *publen);

#ifdef __cplusplus
}
#endif

#endif /* ECC_CRYPTUTIL_H */
