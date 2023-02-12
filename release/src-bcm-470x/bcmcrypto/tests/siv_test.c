/*
 * siv_test.c
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: siv_test.c 453301 2014-02-04 19:49:09Z $
 */

/* test ref - http://tools.ietf.org/search/rfc5297 */

#include <bcmcrypto/aessiv.h>
#include <bcmutils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SZ 1024

/* test 0 data */
const size_t key0_len = AES_BLOCK_SZ;
const uint8 iv0_key[] = {
	0xff, 0xfe, 0xfd, 0xfc,
	0xfb, 0xfa, 0xf9, 0xf8,
	0xf7, 0xf6, 0xf5, 0xf4,
	0xf3, 0xf2, 0xf1, 0xf0
};

const uint8 ctr0_key[] = {
	0xf0, 0xf1, 0xf2, 0xf3,
	0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0xfa, 0xfb,
	0xfc, 0xfd, 0xfe, 0xff
};

const uint8 ad0[] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
	0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23,
	0x24, 0x25, 0x26, 0x27
};
const size_t ad0_len = sizeof(ad0);

uint8 p0[] = {
	0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
	0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee
};
const size_t p0_len = sizeof(p0);

uint8 e0[] = {
	0x40, 0xc0, 0x2b, 0x96, 0x90, 0xc4, 0xdc, 0x04,
	0xda, 0xef, 0x7f, 0x6a, 0xfe, 0x5c
};
const size_t e0_len = sizeof(e0);

const uint8 iv0[] = {
	0x85, 0x63, 0x2d, 0x07,
	0xc6, 0xe8, 0xf3, 0x7f,
	0x95, 0x0a, 0xcd, 0x32,
	0x0a, 0x2e, 0xcc, 0x93
};
const size_t iv0_len = sizeof(iv0);

/* test 1 data */

const uint8 iv1_key[] = {
	0x7f, 0x7e, 0x7d, 0x7c, 0x7b, 0x7a, 0x79, 0x78,
	0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70
};

const uint8 ctr1_key[] = {
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
};
const size_t key1_len = AES_BLOCK_SZ;

const uint8 ad1a[] = {
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
	0xde, 0xad, 0xda, 0xda, 0xde, 0xad, 0xda, 0xda,
	0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,
	0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00
};
const size_t ad1a_len = sizeof(ad1a);

const uint8 ad1b[] = {
	0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
	0x90, 0xa0
};
const size_t ad1b_len = sizeof(ad1b);

const uint8 ad1c[] = {
	0x09, 0xf9, 0x11, 0x02, 0x9d, 0x74, 0xe3, 0x5b,
	0xd8, 0x41, 0x56, 0xc5, 0x63, 0x56, 0x88, 0xc0
};
const size_t ad1c_len = sizeof(ad1c);

const uint8 p1[] = {
	0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
	0x73, 0x6f, 0x6d, 0x65, 0x20, 0x70, 0x6c, 0x61,
	0x69, 0x6e, 0x74, 0x65, 0x78, 0x74, 0x20, 0x74,
	0x6f, 0x20, 0x65, 0x6e, 0x63, 0x72, 0x79, 0x70,
	0x74, 0x20, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20,
	0x53, 0x49, 0x56, 0x2d, 0x41, 0x45, 0x53
};
const size_t p1_len = sizeof(p1);

const uint8 iv1[] = {
	0x7b, 0xdb, 0x6e, 0x3b, 0x43, 0x26, 0x67, 0xeb,
	0x06, 0xf4, 0xd1, 0x4b, 0xff, 0x2f, 0xbd, 0x0f
};
const size_t iv1_len = sizeof(iv1);

const uint8 e1[] = {
	0xcb, 0x90, 0x0f, 0x2f, 0xdd, 0xbe, 0x40, 0x43,
	0x26, 0x60, 0x19, 0x65, 0xc8, 0x89, 0xbf, 0x17,
	0xdb, 0xa7, 0x7c, 0xeb, 0x09, 0x4f, 0xa6, 0x63,
	0xb7, 0xa3, 0xf7, 0x48, 0xba, 0x8a, 0xf8, 0x29,
	0xea, 0x64, 0xad, 0x54, 0x4a, 0x27, 0x2e, 0x9c,
	0x48, 0x5b, 0x62, 0xa3, 0xfd, 0x5c, 0x0d
};
const size_t e1_len = sizeof(e1);

void dbgprint(const char* pfx, const uint8* buf, size_t len, int line)
{
	int i;
	printf("%d: %s: begin\n%s", line, pfx, (len ? "  " : ""));
	for (i = 0; i < len; ++i) {
		if (i != 0 && i%16 == 0)
			printf("  ");
		printf("%02x ", buf[i]);
		if ((i+1)%16 == 0)
			printf("\n");
	}
	if (len%16 != 0)
		printf("\n");
	printf("%d: %s: end\n", line, pfx);
}


int main(int argc,  char **argv)
{
	int err;
	siv_block_t iv;
	uint8 data[MAX_DATA_SZ];
	aessiv_ctx_t ctx;

	/* begin test 0 */

	err = aessiv_init(&ctx, SIV_ENCRYPT, key0_len, iv0_key, ctr0_key);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad0, ad0_len);
	if (err != BCME_OK)
		goto done;

	memcpy(data, p0, p0_len);
	err = aessiv_final(&ctx, iv, data, p0_len);
	if (err != BCME_OK)
		goto done;

	if (memcmp(data, e0, p0_len) || memcmp(iv, iv0, iv0_len)) {
		err = BCME_MICERR;
		dbgprint("test 0 - computed iv", iv, iv0_len, __LINE__);
		dbgprint("test 0 - computed data", data, p0_len, __LINE__);
		dbgprint("test 0 - expected iv", iv0, iv0_len, __LINE__);
		dbgprint("test 0 - expected data", e0, p0_len, __LINE__);
		goto done;
	}

	printf("test 0 encryption - success\n");

#ifdef XXDBG
	dbgprint("test 0 - iv key0", iv0_key, key0_len, __LINE__);
	dbgprint("test 0 - ctr key0", ctr0_key, key0_len, __LINE__);
	dbgprint("test 0 - ad0", ad0, ad0_len, __LINE__);
	dbgprint("test 0 - o0", p0, p0_len, __LINE__);
	dbgprint("test 0 - computed iv", iv, iv0_len, __LINE__);
	dbgprint("test 0 - computed data", data, p0_len, __LINE__);
	dbgprint("test 0 - expected iv", iv0, iv0_len, __LINE__);
	dbgprint("test 0 - expected data", e0, p0_len, __LINE__);
#endif

	err = aessiv_init(&ctx, SIV_DECRYPT, key0_len, iv0_key, ctr0_key);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad0, ad0_len);
	if (err != BCME_OK)
		goto done;

	memcpy(iv, iv0, iv0_len);
	err = aessiv_final(&ctx, iv, data, p0_len);
	if (err != BCME_OK)
		goto done;

	if (memcmp(data, p0, p0_len)) {
		dbgprint("test 0 - expected decrypted data", p0, p0_len, __LINE__);
		dbgprint("test 0 - decrypted data", data, p0_len, __LINE__);
		err = BCME_DECERR;
		goto done;
	}

	printf("test 0 decryption - success\n");

	/* end test 0 */

	/* begin test 1 */

	err = aessiv_init(&ctx, SIV_ENCRYPT, key1_len, iv1_key, ctr1_key);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1a, ad1a_len);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1b, ad1b_len);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1c, ad1c_len);
	if (err != BCME_OK)
		goto done;

	memcpy(data, p1, p1_len);
	err = aessiv_final(&ctx, iv, data, p1_len);
	if (err != BCME_OK)
		goto done;

	if (memcmp(data, e1, e1_len) || memcmp(iv, iv1, iv1_len)) {
		err = BCME_MICERR;
		dbgprint("test 0 - computed iv", iv, iv1_len, __LINE__);
		dbgprint("test 0 - computed data", data, p1_len, __LINE__);
		dbgprint("test 0 - expected iv", iv1, iv1_len, __LINE__);
		dbgprint("test 0 - expected data", e1, e1_len, __LINE__);
		goto done;
	}

	printf("test 1 encryption - success\n");

	err = aessiv_init(&ctx, SIV_DECRYPT, key1_len, iv1_key, ctr1_key);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1a, ad1a_len);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1b, ad1b_len);
	if (err != BCME_OK)
		goto done;

	err = aessiv_update(&ctx, ad1c, ad1c_len);
	if (err != BCME_OK)
		goto done;

	memcpy(iv, iv1, iv1_len);
	err = aessiv_final(&ctx, iv, data, e1_len);
	if (err != BCME_OK)
		goto done;

	if (memcmp(data, p1, p1_len)) {
		dbgprint("test 1 - expected decrypted data", p1, p1_len, __LINE__);
		dbgprint("test 1 - decrypted data", data, p1_len, __LINE__);
		err = BCME_DECERR;
		goto done;
	}

	printf("test 1 decryption - success\n");

	/* end test 1 */

done:
	if (err != BCME_OK)
		printf("test - failed\n");
	else
		printf("test - success\n");
	return err;
}
