/*
 * docmac.c - caculate aes cmac over given key & input in hex 
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: $
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <openssl/aes.h>
#include <openssl/cmac.h>
#include <openssl/evp.h>

void aes_cmac_calc(
	unsigned char *key,
	size_t key_len,
	unsigned char *in,
	size_t in_len,
	unsigned char *mic,
	size_t mic_size)
{
	int err;
	CMAC_CTX *ctx;
	size_t mic_len;
	unsigned char mic_tmp[128];

	ctx = CMAC_CTX_new();
	if (!ctx) {
		fprintf(stderr, "Error allocating cmac context\n");
		goto done;
	}
	
	err = CMAC_Init(ctx, key, key_len, EVP_aes_128_cbc(), NULL);
	if (!err) {
		fprintf(stderr, "Error from cmac init\n");
		goto done;
	}

	err = CMAC_Update(ctx, in, in_len);
	if (!err) {
		fprintf(stderr, "Error from cmac update\n");
		goto done;
	}

	err = CMAC_Final(ctx, mic_tmp, &mic_len);
	if (!err) {
		fprintf(stderr, "Error from cmac update\n");
		goto done;
	}

	memset(mic, 0, mic_size);
	if (mic_len > mic_size)
		mic_len = mic_size;
	memcpy(mic, mic_tmp, mic_len);

done:;
}

void output(int ofd, unsigned char* out, size_t osz)
{
	if (!out)
		goto done;

	uint nw=0;
	while (osz--) {
		if (nw %16 == 0) {
			char lbuf[16];
			if (nw)
				write(ofd, "\n", 1);
			sprintf(lbuf, " %04d: ", nw);
			write(ofd, lbuf, strlen(lbuf));
		}
			
		char hbuf[4];
		sprintf(hbuf, "%02x ", *out++);
		write(ofd, hbuf, 3);
		nw++;
	}
	write(ofd, "\n", 1);

done:;
}

unsigned h2i(int c)
{
        if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
        else if (c >= '0' && c <= '9')
                return c - '0';
        else
                return 0;
}


void hex2bin(const char* in, uint isz, unsigned char* out, uint olim, uint* osz)
{
	*osz = 0;
	if (!in || !olim)
		return;

	int ch_hi;
	while (isz && olim) {
		int ch_lo;

		/* consume the first hex digit */
		ch_hi	= *in++; isz--;
		if (!ch_hi) break;
		if (!isxdigit(ch_hi)) continue;

		/* next digit */
		if (!isz) break;
		ch_lo = *in++; isz--;
		while (ch_lo && !isxdigit(ch_lo)) {
			ch_lo = *in++; isz--;
			if (!isz) break;
		}
		if (!ch_lo) break;
		*out++ =  h2i(ch_hi) << 4 | h2i(ch_lo); (*osz)++; olim--;
	}
}

int main(int argc, char **argv)
{
	int ret;
	const char *prog;
	unsigned char mic[AES_BLOCK_SIZE];
	unsigned char *key;
	unsigned char *in;
	int in_len;
	int key_len;
	
	ret = 0;
	prog = argv[0];
	if (argc != 3) {
		fprintf(stderr, "usage: %s <key> <input>\n", prog);
		goto done;
	}

	key = (unsigned char *)strdup(argv[1]);
	key_len = strlen((char *)key);
	if (!key) {
		fprintf(stderr, "strdup failed for key - errno %d\n", errno);
		goto done;
	}

	hex2bin((char *)key, key_len, key, key_len, (uint *)&key_len);

	in = (unsigned char *)strdup(argv[2]);
	in_len = strlen((char *)in);
	if (!in) {
		fprintf(stderr, "strdup failed for input - errno %d\n", errno);
		goto done;
	}

	hex2bin((char *)in, in_len, in, in_len, (uint *)&in_len);

	aes_cmac_calc(key, key_len, in, in_len, mic, sizeof(mic));
	write(1, "mic:\n", sizeof("mic:\n"));
	output(1, mic, sizeof(mic));

done:
	return 0;
}
