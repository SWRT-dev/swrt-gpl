// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP or GPHY
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

#include <linux/scatterlist.h>
#include <crypto/aes.h>
#include <crypto/hash.h>
#include <crypto/aead.h>
#include <crypto/skcipher.h>

#include "macsec.h"

struct crypto_wait {
	struct completion completion;
	int err;
};

#define DECLARE_CRYPTO_WAIT(_wait) \
	struct crypto_wait _wait = { \
		COMPLETION_INITIALIZER_ONSTACK((_wait).completion), 0 }

static int crypto_wait_req(int err, struct crypto_wait *wait)
{
	switch (err) {
	case -EINPROGRESS:
	case -EBUSY:
		wait_for_completion(&wait->completion);
		reinit_completion(&wait->completion);
		err = wait->err;
		break;
	}

	return err;
}

static void crypto_req_done(struct crypto_async_request *req, int err)
{
	struct crypto_wait *wait = req->data;

	if (err == -EINPROGRESS)
		return;

	wait->err = err;
	complete(&wait->completion);
}

int derive_aes_hkey(const u8 *key, u8 *output, u32 keylen)
{
	struct crypto_skcipher *tfm;
	struct skcipher_request *req = NULL;
	struct scatterlist src, dst;
	DECLARE_CRYPTO_WAIT(wait);
	u8 input[MACSEC_HKEY_LEN_BYTE] = {0};
	int ret;

	tfm = crypto_alloc_skcipher("ecb(aes)", 0, 0);
	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	req = skcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req) {
		ret = -ENOMEM;
		goto OUT;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
						CRYPTO_TFM_REQ_MAY_SLEEP, crypto_req_done,
						&wait);

	ret = crypto_skcipher_setkey(tfm, key, keylen);
	if (ret < 0)
		goto OUT;

	sg_init_one(&src, input, MACSEC_HKEY_LEN_BYTE);
	sg_init_one(&dst, output, MACSEC_HKEY_LEN_BYTE);
	skcipher_request_set_crypt(req, &src, &dst, MACSEC_HKEY_LEN_BYTE, NULL);

	ret = crypto_wait_req(crypto_skcipher_encrypt(req), &wait);

OUT:
	skcipher_request_free(req);
	crypto_free_skcipher(tfm);
	return ret;
}
