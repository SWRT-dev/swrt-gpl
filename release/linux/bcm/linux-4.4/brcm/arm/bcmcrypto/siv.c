/*
 * siv.c
 *
	Broadcom Proprietary and Confidential. Copyright (C) 2016,
	All Rights Reserved.
	
	This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
	the contents of this file may not be disclosed to third parties, copied
	or duplicated in any form, in whole or in part, without the prior
	written permission of Broadcom.
 *
 * $Id: siv.c 453301 2014-02-04 19:49:09Z $
 */

#include <bcm_cfg.h>
#include <typedefs.h>

#ifdef BCMDRIVER
#include <osl.h>
#else
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ASSERT assert
#endif  /* BCMDRIVER */

#include <bcmendian.h>
#include <bcmutils.h>
#include <bcmcrypto/siv.h>


/* non-zero lsb byte of dbl operation */
static const uint8 siv_r15 = 0x87;

/* _b & 1**n-64.01**31.01**31, n = 128 */
#define SIV_CTR_MASK(_b) do {\
	(_b)[8] &= 0x7f; \
	(_b)[12] &= 0x7f; \
} while (0)

static void
siv_xor_block(const siv_block_t s1, const siv_block_t s2, siv_block_t d)
{
	int i;
	for (i = 0; i < SIV_BLOCK_SZ; ++i)
		d[i] = s1[i] ^ s2[i];
}

static void siv_lshift(siv_block_t b)
{
	int i;
	int oc;

	oc = 0;
	for (i = SIV_BLOCK_SZ - 1; i >= 0; --i) {
		const int nc = (b[i] & 0x80) >> 7;
		b[i] = ((b[i] << 1) | oc) & 0xff;
		oc = nc;
	}
}

static void siv_dbl(siv_block_t b)
{
	int msb = b[0] & 0x80;
	siv_lshift(b);
	if (msb)
		b[SIV_BLOCK_SZ - 1] ^= siv_r15;
}

int
siv_init(siv_ctx_t *ctx, siv_op_type_t op_type,
	siv_cmac_fn_t cmac_cb, void *cmac_ctx,
	siv_ctr_fn_t ctr_cb, void *ctr_ctx)
{
	int err = BCME_OK;

	ASSERT(ctx != NULL);
	ASSERT(op_type == SIV_ENCRYPT || op_type == SIV_DECRYPT);
	ASSERT(cmac_cb != NULL && ctr_cb != NULL);

	memset(ctx, 0, sizeof(*ctx));
	ctx->op_type = op_type;
	ctx->cmac_cb = cmac_cb;
	ctx->cmac_cb_ctx = cmac_ctx;
	ctx->ctr_cb = ctr_cb;
	ctx->ctr_cb_ctx = ctr_ctx;

	/* init iv w/ cmac of zero block */
	err = (*cmac_cb)(ctx->cmac_cb_ctx, ctx->iv, sizeof(ctx->iv),
		ctx->iv, sizeof(ctx->iv));
	return err;
}

int
siv_update(siv_ctx_t *ctx, const uint8 *hdr, size_t hdr_len)
{
	siv_block_t b;
	int err = BCME_OK;

	ASSERT(ctx != NULL);

	if (ctx->num_hdr >= (SIV_BLOCK_NBITS - 2)) {
		err = BCME_BADLEN;
		goto done;
	}

	(ctx->num_hdr)++;
	err = (*(ctx->cmac_cb))(ctx->cmac_cb_ctx, hdr, hdr_len, b, sizeof(b));
	if (err != BCME_OK)
		goto done;

	siv_dbl(ctx->iv);
	siv_xor_block(ctx->iv, b, ctx->iv);

done:
	return err;
}

int
siv_final(siv_ctx_t *ctx, uint8 *iv_in, uint8 *data, size_t data_len)
{
	int err = BCME_OK;
	siv_block_t b;

	ASSERT(ctx != NULL);
	ASSERT(ctx->op_type == SIV_ENCRYPT || ctx->op_type == SIV_DECRYPT);

	if (ctx->op_type == SIV_DECRYPT && data_len != 0) {
		memcpy(b, iv_in, sizeof(b));
		SIV_CTR_MASK(b);
		err = (*(ctx->ctr_cb))(ctx->ctr_cb_ctx, b, data, data_len);
		if (err != BCME_OK)
			goto done;
	}

	/* we now have plain text in data, compute expected/transmitted iv
	 * note that MSB is byte 0, and LSB is byte SIV_BLOCK_SZ-1
	 */
	if (data_len < SIV_BLOCK_SZ) { /* dbl(S) ^ Xm.10* */
		size_t len;

		siv_dbl(ctx->iv);
		memcpy(b, data, data_len);
		len = data_len;

		b[len++] = 0x80;
		if (len < SIV_BLOCK_SZ)
			memset(&b[len], 0, SIV_BLOCK_SZ - len);

		siv_xor_block(b, ctx->iv, ctx->iv);
		err = (*(ctx->cmac_cb))(ctx->cmac_cb_ctx, ctx->iv, sizeof(ctx->iv),
			ctx->iv, sizeof(ctx->iv));
		if (err != BCME_OK)
			goto done;
	} else { /* S ^end Xm */
		uint8 *endp;

		/* save lsb bytes of input as we need to append ctx->iv at end (lsb) of data
		 * to compute the final iv
		 */
		endp = &data[data_len - SIV_BLOCK_SZ];
		memcpy(b, endp, sizeof(b));
		siv_xor_block(endp, ctx->iv, endp);
		err = (*(ctx->cmac_cb))(ctx->cmac_cb_ctx, data, data_len, ctx->iv, sizeof(ctx->iv));
		if (err != BCME_OK)
			goto done;
		memcpy(endp, b, sizeof(b));
	}

	if (ctx->op_type == SIV_ENCRYPT) {
		memcpy(iv_in, ctx->iv, sizeof(ctx->iv));
		if (data_len != 0) {
			memcpy(b, ctx->iv, sizeof(b));
			SIV_CTR_MASK(b);
			err = (*(ctx->ctr_cb))(ctx->ctr_cb_ctx, b, data, data_len);
			if (err != BCME_OK)
				goto done;
		}
	} else {
		ASSERT(ctx->op_type == SIV_DECRYPT);
		/* check expected iv.  perhaps need to clear data on failure, but
		 * that's not good for debugging
		 */
		if (memcmp(iv_in, ctx->iv, sizeof(ctx->iv))) {
			err = BCME_MICERR;
			goto done;
		}
	}

done:
	memset(ctx, 0, sizeof(*ctx));
	return err;
}
