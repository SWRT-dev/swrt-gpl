/*
 * cryptutil.h
 * utility functions for crypt and hashing.
 *
 * Copyright (C) 2021 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 *
 */


#ifndef CRYPT_UTIL_H
#define CRYPT_UTIL_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_MAC_LEN	32
#define AES_BLOCK_SIZE	16

int PLATFORM_GENERATE_DH_KEY_PAIR(uint8_t **priv, size_t *privlen,
				  uint8_t **pub, size_t *publen);

int PLATFORM_COMPUTE_DH_SHARED_SECRET(uint8_t **shkey, size_t *shkeylen,
				      uint8_t *peer_pub, size_t peer_publen,
				      uint8_t *local_priv, size_t local_privlen,
				      uint8_t *local_pub, size_t local_publen);

int PLATFORM_SHA256(int num, const uint8_t *addr[], const size_t *len,
		    uint8_t *digest);

int PLATFORM_HMAC_SHA256(const uint8_t *key, size_t keylen, int num,
			 const uint8_t *addr[], const size_t *len, uint8_t *hmac);


int PLATFORM_AES_ENCRYPT(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len);
int PLATFORM_AES_DECRYPT(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t data_len);


int AES_WRAP_128(uint8_t *key, uint8_t *plain, size_t plen,
		 uint8_t *cipher, size_t *clen);

int AES_UNWRAP_128(uint8_t *key, uint8_t *cipher, size_t clen,
		   uint8_t *plain, size_t *plen);


int SHA256_PRF(const uint8_t *key, size_t key_len, const char *label,
	       const uint8_t *data, size_t data_len, uint8_t *buf, size_t buf_len);

int omac1_aes_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		     const uint8_t *addr[], const size_t *len, uint8_t *mac);

int aes_ctr_encrypt(const uint8_t *key, size_t key_len, const uint8_t *nonce,
		    uint8_t *data, size_t data_len);

int AES_SIV_ENCRYPT(const uint8_t *key, size_t key_len,
		    const uint8_t *pw, size_t pwlen,
		    size_t num_elem, const uint8_t *addr[], const size_t *len,
		    uint8_t *out);
int AES_SIV_DECRYPT(const uint8_t *key, size_t key_len,
		    const uint8_t *iv_crypt, size_t iv_c_len,
		    size_t num_elem, const uint8_t *addr[], const size_t *len,
		    uint8_t *out);

int hmac_sha256_kdf(const uint8_t *secret, size_t secret_len,
		    const char *label, const uint8_t *seed, size_t seed_len,
		    uint8_t *out, size_t outlen);

#ifdef __cplusplus
}
#endif

#endif /* CRYPT_UTIL_H */
