/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * Copyright (C) 2020 iopsys Software Solutions AB.
 *
 * Modified to return encode/decode data in user passed buffer instead of
 * allocating one in the library functions.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "easy.h"

static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out: Pointer to output buffer where encoded data is written to
 * @out_len: Pointer to output length (initialized with sizeof out buffer)
 * Returns: 0 on success, else -1 or appropriate error codes.
 *
 * Returned buffer is '\0' terminated to make it easier to use as a C string.
 * The '\0' terminator is not included in out_len.
 */
#if defined(RTCONFIG_SWRTMESH)
int LIBEASY_API easy_base64_encode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len)
#else
int LIBEASY_API base64_encode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len)
#endif
{
	unsigned char *pos;
	const unsigned char *end, *in;
	size_t olen;
	int line_len;

	if (!out || !out_len)
		return -EINVAL;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	if (olen < len)
		return -ERANGE; /* integer overflow */

	if (*out_len < olen)
		return -EMSGSIZE;

	memset(out, 0, olen);
	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
		line_len += 4;
	}

	if (line_len)
		*pos++ = '\n';

	*pos = '\0';
	*out_len = (size_t)(pos - out);

	return 0;
}


/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out: Pointer to output buffer where decoded data is written to
 * @out_len: Pointer to output length (initialized with sizeof out buffer)
 * Returns: 0 on success, else -1 or appropriate error codes.
 */
#if defined(RTCONFIG_SWRTMESH)
int LIBEASY_API easy_base64_decode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len)
#else
int LIBEASY_API base64_decode(const unsigned char *src, size_t len,
				unsigned char *out, size_t *out_len)
#endif
{
	unsigned char dtable[256], *pos, block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

	if (!out || !out_len)
		return -EINVAL;

	memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

	if (count == 0 || count % 4)
		return -1;

	olen = count / 4 * 3;
	if (*out_len < olen)
		return -EMSGSIZE;

	memset(out, 0, olen);
	pos = out;

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (unsigned char)((block[0] << 2U) | (block[1] >> 4U));
			*pos++ = (unsigned char)((block[1] << 4U) | (block[2] >> 2U));
			*pos++ = (unsigned char)((block[2] << 6U) | block[3]);
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
					return -1;
				}
				break;
			}
		}
	}

	*out_len = (size_t)(pos - out);

	return 0;
}
