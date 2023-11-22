/*
 * AES SIV (RFC 5297)
 * Copyright (c) 2013 Cozybit, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

/* https://github.com/cryptotronix/yacl/blob/master/src/aes/aes-siv.c */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "easy.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#endif

static const uint8_t zero[AES_BLOCK_SIZE];


static void dbl(uint8_t *pad)
{
	int i, carry;

	carry = pad[0] & 0x80;
	for (i = 0; i < AES_BLOCK_SIZE - 1; i++)
		pad[i] = (pad[i] << 1) | (pad[i + 1] >> 7);
	pad[AES_BLOCK_SIZE - 1] <<= 1;
	if (carry)
		pad[AES_BLOCK_SIZE - 1] ^= 0x87;
}


static void xor(uint8_t *a, const uint8_t *b)
{
	int i;

	for (i = 0; i < AES_BLOCK_SIZE; i++)
		*a++ ^= *b++;
}


static void xorend(uint8_t *a, int alen, const uint8_t *b, int blen)
{
	int i;

	if (alen < blen)
		return;

	for (i = 0; i < blen; i++)
		a[alen - blen + i] ^= b[i];
}


static void pad_block(uint8_t *pad, const uint8_t *addr, size_t len)
{
	memset(pad, 0, AES_BLOCK_SIZE);
	memcpy(pad, addr, len);

	if (len < AES_BLOCK_SIZE)
		pad[len] = 0x80;
}


static int aes_s2v(const uint8_t *key, size_t key_len,
		   size_t num_elem, const uint8_t *addr[], size_t *len, uint8_t *mac)
{
	uint8_t tmp[AES_BLOCK_SIZE], tmp2[AES_BLOCK_SIZE];
	uint8_t *buf = NULL;
	int ret;
	size_t i;
	const uint8_t *data[1];
	size_t data_len[1];

	if (!num_elem) {
		memcpy(tmp, zero, sizeof(zero));
		tmp[AES_BLOCK_SIZE - 1] = 1;
		data[0] = tmp;
		data_len[0] = sizeof(tmp);
		return omac1_aes_vector(key, key_len, 1, data, data_len, mac);
	}

	data[0] = zero;
	data_len[0] = sizeof(zero);
	ret = omac1_aes_vector(key, key_len, 1, data, data_len, tmp);
	if (ret)
		return ret;

	for (i = 0; i < num_elem - 1; i++) {
		ret = omac1_aes_vector(key, key_len, 1, &addr[i], &len[i],
				       tmp2);
		if (ret)
			return ret;

		dbl(tmp);
		xor(tmp, tmp2);
	}
	if (len[i] >= AES_BLOCK_SIZE) {
		buf = calloc(1, sizeof(uint8_t) * len[i]);
		if (!buf)
			return -ENOMEM;

		memcpy(buf, addr[i], len[i]);
		xorend(buf, len[i], tmp, AES_BLOCK_SIZE);
		data[0] = buf;
		ret = omac1_aes_vector(key, key_len, 1, data, &len[i], mac);
		memset(buf, 0, len[i]);
		free(buf);
		return ret;
	}

	dbl(tmp);
	pad_block(tmp2, addr[i], len[i]);
	xor(tmp, tmp2);

	data[0] = tmp;
	data_len[0] = sizeof(tmp);
	return omac1_aes_vector(key, key_len, 1, data, data_len, mac);
}


int LIBEASY_API AES_SIV_ENCRYPT(const uint8_t *key, size_t key_len,
		    const uint8_t *pw, size_t pwlen,
		    size_t num_elem, const uint8_t *addr[], const size_t *len,
		    uint8_t *out)
{
	const uint8_t *_addr[6];
	size_t _len[6];
	const uint8_t *k1, *k2;
	uint8_t v[AES_BLOCK_SIZE];
	size_t i;
	uint8_t *iv, *crypt_pw;

	if (num_elem > ARRAY_SIZE(_addr) - 1 ||
	    (key_len != 32 && key_len != 48 && key_len != 64))
		return -1;

	key_len /= 2;
	k1 = key;
	k2 = key + key_len;

	for (i = 0; i < num_elem; i++) {
		_addr[i] = addr[i];
		_len[i] = len[i];
	}
	_addr[num_elem] = pw;
	_len[num_elem] = pwlen;

	if (aes_s2v(k1, key_len, num_elem + 1, _addr, _len, v))
		return -1;

	iv = out;
	crypt_pw = out + AES_BLOCK_SIZE;

	memcpy(iv, v, AES_BLOCK_SIZE);
	memcpy(crypt_pw, pw, pwlen);

	/* zero out 63rd and 31st bits of ctr (from right) */
	v[8] &= 0x7f;
	v[12] &= 0x7f;
	return aes_ctr_encrypt(k2, key_len, v, crypt_pw, pwlen);
}


int LIBEASY_API AES_SIV_DECRYPT(const uint8_t *key, size_t key_len,
		    const uint8_t *iv_crypt, size_t iv_c_len,
		    size_t num_elem, const uint8_t *addr[], const size_t *len,
		    uint8_t *out)
{
	const uint8_t *_addr[6];
	size_t _len[6];
	const uint8_t *k1, *k2;
	size_t crypt_len;
	size_t i;
	int ret;
	uint8_t iv[AES_BLOCK_SIZE];
	uint8_t check[AES_BLOCK_SIZE];

	if (iv_c_len < AES_BLOCK_SIZE || num_elem > ARRAY_SIZE(_addr) - 1 ||
	    (key_len != 32 && key_len != 48 && key_len != 64))
		return -1;
	crypt_len = iv_c_len - AES_BLOCK_SIZE;
	key_len /= 2;
	k1 = key;
	k2 = key + key_len;

	for (i = 0; i < num_elem; i++) {
		_addr[i] = addr[i];
		_len[i] = len[i];
	}
	_addr[num_elem] = out;
	_len[num_elem] = crypt_len;

	memcpy(iv, iv_crypt, AES_BLOCK_SIZE);
	memcpy(out, iv_crypt + AES_BLOCK_SIZE, crypt_len);

	iv[8] &= 0x7f;
	iv[12] &= 0x7f;

	ret = aes_ctr_encrypt(k2, key_len, iv, out, crypt_len);
	if (ret)
		return ret;

	ret = aes_s2v(k1, key_len, num_elem + 1, _addr, _len, check);
	if (ret)
		return ret;
	if (memcmp(check, iv_crypt, AES_BLOCK_SIZE) == 0)
		return 0;

	return -1;
}
