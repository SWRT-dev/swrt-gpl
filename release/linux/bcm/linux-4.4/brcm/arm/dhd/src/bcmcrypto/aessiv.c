/*
 * aessiv.c
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: aessiv.c 453301 2014-02-04 19:49:09Z $
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
#include <bcmcrypto/aessiv.h>

static int
cmac_cb(void *ctx_in, const uint8* data, size_t data_len, uint8* mic, const size_t mic_len)
{
	aessiv_ctx_t *ctx = (aessiv_ctx_t *)ctx_in;
	aes_cmac(ctx->nrounds, ctx->iv_rkey, ctx->iv_subk1, ctx->iv_subk2, data_len,
		data, mic, mic_len);
	return BCME_OK;
}

static int ctr_cb(void *ctx_in, const uint8 *iv, uint8* data, size_t data_len)
{
	int err;

	aessiv_ctx_t *ctx = (aessiv_ctx_t *)ctx_in;
	err = aes_ctr_crypt(ctx->ctr_rkey, ctx->key_len, iv, data_len, data, data);
	return err ? BCME_DECERR : BCME_OK;
}

int
aessiv_init(aessiv_ctx_t *ctx, siv_op_type_t op_type,
	size_t key_len, const uint8 *iv_key, const uint8 *ctr_key)
{
	int err;

	memset(ctx, 0, sizeof(*ctx));

	ctx->key_len = key_len;
	ctx->nrounds = (int)AES_ROUNDS(key_len);
	rijndaelKeySetupEnc(ctx->iv_rkey, iv_key, (int)AES_KEY_BITLEN(key_len));
	aes_cmac_gen_subkeys(ctx->nrounds, ctx->iv_rkey, ctx->iv_subk1, ctx->iv_subk2);
	rijndaelKeySetupEnc(ctx->ctr_rkey, ctr_key, (int)AES_KEY_BITLEN(key_len));

	err = siv_init(&ctx->siv_ctx, op_type, cmac_cb, ctx, ctr_cb, ctx);
	if (err != BCME_OK)
		goto done;

done:
	return err;
}


int aessiv_update(aessiv_ctx_t *ctx, const uint8 *hdr, size_t hdr_len)
{
	ASSERT(ctx != NULL);
	return siv_update(&ctx->siv_ctx, hdr, hdr_len);
}

/* Finalize aessiv context. */
int
aessiv_final(aessiv_ctx_t *ctx, uint8 *iv, uint8 *data, size_t data_len)
{
	int err;

	ASSERT(ctx != NULL);
	err =  siv_final(&ctx->siv_ctx, iv, data, data_len);

	memset(ctx, 0, sizeof(*ctx));
	return err;
}
