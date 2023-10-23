/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/******************************************************************************
**
** FILE NAME    : ltq_ipsec_test_api.c
** PROJECT      : GRX500
** MODULES      : crypto hardware
**
** DATE         : 10 May 2016
** AUTHOR       : Mohammad Firdaus B Alias Thani
** DESCRIPTION  : IPSec token/SA generator test application
**
*******************************************************************************/

#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/string.h>
#include <linux/moduleparam.h>
#include <crypto/ltq_ipsec_ins.h>
#include <linux/slab.h>

#include "ltq_crypto_core.h"

#include "dmares_types.h"
#include "api_dmabuf.h"
#include "sa_builder.h"
#include "sa_builder_ipsec.h"
#include "token_builder.h"

struct ltq_test_vector {
	int ipsec_mode;				/* AH/ESP */
	int ipsec_tunnel;			/* Tunnel or transport mode */
	int direction;				/* INBOUND or OUTBOUND */
	int ip_type;				/* IPv4 or IPv6*/
	unsigned int ring_no;		/* Ring number this SA is registered to */
	unsigned int data_len;		/* size of packet */
	u32	flags;					/* optional flag for future use */
	u32	spi;					/* SPI number */

	u8 *src;					/* pointer to source buffer */
	
	u32 crypto_algo;			/* crypto algo used */
	u32 crypto_mode;			/* crypto mode used */
	u32	cryptosize;				/* size of encrypted data */
	u32 crypto_keysize;			/* key size used for crypto algo */
	u8 *crypto_key;				/* pointer to the crypto key buffer */
	
	u32 auth_algo;				/* auth algo used */
	u32	authsize;				/* expected size of authentication data */
	u32 auth_keysize;			/* key size used for auth algo */
	u8 *auth_key;				/* pointer to the auth key buffer */

	u8 nonce[8];					/* pointer to nonce buffer */
	u8 IV[32];					/* Pointer to the IV buffer */
	u8 *AAD;					/* Additional additional authenticated data */
	unsigned int aad_len;		/* length of additonal authenticated data */		
};

static struct ltq_test_vector test_data [] = {
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_OUTBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 16,
		.flags = 0,
		.spi = 0x1234,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
		.cryptosize = 16,
		.crypto_key =  "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		              "\x51\x2e\x03\xd5\x34\x12\x00\x06",	
		.crypto_keysize = 16,
		.auth_algo = SAB_AUTH_HMAC_SHA1,
		.authsize = 20,
		.auth_keysize = 20,
		.auth_key = "\x00\x00\x00\x00\x00\x00\x00\x00"
					"\x00\x00\x00\x00\x00\x00\x00\x00"
    				"\x00\x00\x00\x00",
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 100,
		.flags = 0,
		.spi = 0x1234,
		.crypto_algo = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
		.cryptosize = 100,
		.crypto_key =  "\xE9\xC0\xFF\x2E\x76\x0B\x64\x24"
		              "\x44\x4D\x99\x5A\x12\xD6\x40\xC0"
		                "\xEA\xC2\x84\xE8\x14\x95\xDB\xE8",
		.crypto_keysize = 24,
		.auth_algo = SAB_AUTH_HMAC_MD5,
		.authsize = 16,
		.auth_keysize = 16,
		.auth_key = "\x02\x00\x00\x00\x00\x00\x00\x00"
					"\x01\x00\x00\x00\x00\x00\x00\x00"
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 80,
		.flags = 0,
		.spi = 0x1234,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
		.cryptosize = 80,
		.crypto_key =  "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		              "\x51\x2e\x03\xd5\x34\x12\x00\x06",	
		.crypto_keysize = 16,
		.auth_algo = SAB_AUTH_HMAC_SHA1,
		.authsize = 20,
		.auth_keysize = 20,
		.auth_key = "\x00\x00\x00\x00\x00\x00\x00\x00"
					"\x00\x00\x00\x00\x00\x00\x00\x00"
    				"\x00\x00\x00\x00",
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 84,
		.flags = 0,
		.spi = 0x1234,
		.crypto_algo = SAB_CRYPTO_NULL,
		.auth_algo = SAB_AUTH_HMAC_SHA1,
		.authsize = 20,
		.auth_keysize = 20,
		.auth_key = "\x11\x22\x33\x44\x55\x66\x77\x88"
     	            "\x99\xaa\xbb\xcc\xdd\xee\xff\x11"
	                "\x22\x33\x44\x55",
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_OUTBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 16,
		.flags = 0,
		.spi = 0x1234,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CTR,
		.cryptosize = 16,
		.crypto_key =  "\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		              "\x51\x2e\x03\xd5\x34\x12\x00\x06"
					  "\xba\xbe\xfa\xce",
		.crypto_keysize = 20,
		.auth_algo = SAB_AUTH_HMAC_SHA1,
		.authsize = 20,
		.auth_keysize = 20,
		.auth_key = "\x00\x00\x00\x00\x00\x00\x00\x00"
     	            "\x99\xaa\xbb\xcc\xdd\xee\xff\x11"
	                "\x22\x33\x44\x55",
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_OUTBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 16,
		.flags = 0,
		.spi = 0x8728,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_GCM,
		.cryptosize = 16,
		.crypto_keysize = 20,
		.crypto_key = "\xfe\xff\xe9\x92\x86\x65\x73\x1c"
		              "\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                       "\x00\x00\x00\x00",
		.auth_algo = SAB_AUTH_AES_GCM,
		.authsize = 16,
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 88,
		.flags = 0,
		.spi = 0xbabe,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_GCM,
		.cryptosize = 64,
		.crypto_keysize = 20,
		.crypto_key = "\xfe\xff\xe9\x92\x86\x65\x73\x1c"
		              "\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                       "\x00\x00\x00\x00",
		.auth_algo = SAB_AUTH_AES_GCM,
		.authsize = 16,
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 96,
		.flags = 0,
		.spi = 0xbabe,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_GMAC,
		.cryptosize = 96,
		.crypto_keysize = 20,
		.crypto_key = "\xfe\xff\xe9\x92\x86\x65\x73\x1c"
		              "\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                       "\x22\x10\x0d\xff",
		.auth_algo = SAB_AUTH_AES_GMAC,
		.authsize = 16,
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 116,
		.flags = 0,
		.spi = 0xd0d0,
		.crypto_algo = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CCM,
		.cryptosize = 116,
		.crypto_keysize = 19,
		.crypto_key = "\xfe\xff\xe9\x92\x86\x65\x73\x1c"
		              "\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                       "\xaa\xbb\xcc",
		.auth_algo = SAB_AUTH_AES_CCM,
		.authsize = 16,
	},
	{
		.ipsec_mode = SAB_IPSEC_ESP,
		.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL,
		.direction = SAB_DIRECTION_INBOUND,
		.ip_type = SAB_IPSEC_IPV4,
		.ring_no = 2,
		.data_len = 128,
		.flags = 0,
		.spi = 0xface,
		.crypto_algo = SAB_CRYPTO_NULL,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
		.cryptosize = 128,
		.crypto_keysize = 16,
		.crypto_key = "\xfe\xff\xe9\x92\x86\x65\x73\x1c"
		              "\x6d\x6a\x8f\x94\x67\x30\x83\x08",
		.auth_algo = SAB_AUTH_AES_XCBC_MAC,
		.authsize = 16,
	}
};

static void debug_params(struct ltq_crypto_ipsec_params *params)
{
	int i;
	int ds = params->authsize;

    pr_info("ipad: \n");
    print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
                 16, 1,
                 params->ipad, ds, false);

    pr_info("opad: \n");
    print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
                16, 1,
                params->opad, ds, false);

	pr_info("SA buffer dump @ addr 0x%08x: \n", (u32) params->SA_buffer);
	for (i = 0; i <  params->SAWords; i++) {
		pr_info("SA buffer[%d]: 0x%08x\n", i, (u32) params->SA_buffer[i]);
	}

	pr_info("Token header: 0x%08x\n", params->token_headerword);
	for (i = 0; i < params->tokenwords; i++) {
		pr_info("Token[%d]: 0x%08x\n", i, (u32) params->token_buffer[i]);
	}

	pr_info("crypto len offset: %d, IPsec len offset: %d, Alignment padsize: %d\n",
			params->crypto_offs, params->total_pad_offs, params->pad_blksize);
	
	if (params->msg_len_offs > 0)
		pr_info("AES-CCM msg len offset: %d\n", params->msg_len_offs);
	if (params->hash_pad_offs > 0)
		pr_info("AES-CCM hash pad offset: %d\n", params->hash_pad_offs);
	
	pr_info("\nICV Length: %d, ivsize: %d\n", params->ICV_length, params->ivsize);
	pr_info("\n ######### DEBUG DONE ##########\n");
}

static int __init ltq_test_ipsec_mod(void)
{
	int ret;
	int i, test_no = sizeof(test_data)/sizeof(test_data[0]);
	struct ltq_crypto_ipsec_params *params;

	params = (struct ltq_crypto_ipsec_params *)	kmalloc(sizeof(struct ltq_crypto_ipsec_params),
							GFP_KERNEL);
	if (!params) {
		pr_err("Error allocating test mem\n");
		return -ENOMEM;
	}

	for (i = 0; i < test_no; i++) {
		memset(params, 0, sizeof(struct ltq_crypto_ipsec_params));

		params->ipsec_mode = test_data[i].ipsec_mode;
		params->ipsec_tunnel = test_data[i].ipsec_tunnel;
		params->direction = test_data[i].direction;
		params->ip_type = test_data[i].ip_type;
		params->ring_no = test_data[i].ring_no;
	
		params->data_len = test_data[i].data_len;
		params->crypto_offs = 0;
		params->flags = test_data[i].flags;
		params->spi = test_data[i].spi;

		params->crypto_algo = test_data[i].crypto_algo;
		params->crypto_mode = test_data[i].crypto_mode;
		params->cryptosize = test_data[i].cryptosize;
		params->crypto_keysize = test_data[i].crypto_keysize;
		params->crypto_key = test_data[i].crypto_key;

		params->auth_algo = test_data[i].auth_algo;
		params->authsize = test_data[i].authsize;
		params->auth_keysize = test_data[i].auth_keysize;
		params->auth_key = test_data[i].auth_key;

		ret = ltq_ipsec_setkey(params);
		if (ret) {
			pr_err("Setkey failed\n");
			goto exit_err;
		}

		ret = ltq_get_ipsec_token(params);
		if (ret) { 
			ltq_destroy_ipsec_sa(params);
			goto exit_err;;
		}

		debug_params(params);

		ltq_destroy_ipsec_sa(params);
	}

	ret = -EAGAIN;

exit_err:
		kfree(params);
		return ret;;
}


static void __exit ltq_test_ipsec_exit(void)
{
	return;
}

module_init(ltq_test_ipsec_mod);
module_exit(ltq_test_ipsec_exit);

