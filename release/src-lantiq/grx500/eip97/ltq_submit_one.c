/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/******************************************************************************
**
** FILE NAME    : ltq_submit_one.c
** PROJECT      : GRX500
** MODULES      : crypto hardware
**
** DATE         : 10 May 2016
** AUTHOR       : Mohammad Firdaus B Alias Thani
** DESCRIPTION  : IPSec encrypt/decrypt test application
**
*******************************************************************************/

#include <linux/err.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/delay.h>
#include <crypto/ltq_ipsec_ins.h>
#include <linux/slab.h>

#include "dmares_types.h"
#include "api_dmabuf.h"
#include "sa_builder.h"
#include "sa_builder_ipsec.h"
#include "token_builder.h"

#define ESP_HDR			8

#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT

static volatile int submit_done;
static unsigned int engine_out_pkt_len;
u8 *out_ptr;

struct test_vector {
	char *inpkt;
	unsigned int pktlen;
	char *iv;
	char *enc_key;
	unsigned int enc_keylen;
	char *auth_key;
	unsigned int auth_keylen;
	u32 spi;
	int direction;
	int enc_algo;
	int enc_mode;
	int auth_algo;
};

static struct test_vector enc_template [] = {
	{
		.inpkt = "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
        	    "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
            	"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
            	"\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd",
		.pktlen = 47,
		.enc_keylen = 0,
		.auth_key = "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
		.auth_keylen = 20,
		.spi = 0xbabeface,
		.direction = SAB_DIRECTION_OUTBOUND,
		.enc_algo = SAB_CRYPTO_NULL,
		.enc_mode = SAB_CRYPTO_MODE_ECB,
		.auth_algo = SAB_AUTH_HMAC_SHA1,
	},
	{
        .inpkt  = "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7"
              "\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
              "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7"
              "\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
              "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7"
              "\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
              "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7"
              "\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf",
		.pktlen = 64,
		.iv = "\x8c\xe8\x2e\xef\xbe\xa0\xda\x3c"
		      "\x44\x69\x9e\xd7\xdb\x51\xb7\xd9",
		.enc_keylen = 16, 
		.enc_key = "\x56\xe4\x7a\x38\xc5\x59\x89\x74"
              "\xbc\x46\x90\x3d\xba\x29\x03\x49",
        .auth_key = "\x11\x22\x33\x44\x55\x66\x77\x88"
              "\x99\xaa\xbb\xcc\xdd\xee\xff\x11"
              "\x22\x33\x44\x55",
        .auth_keylen = 20,
        .spi = 0xaa0011cc,
        .direction = SAB_DIRECTION_OUTBOUND,
        .enc_algo = SAB_CRYPTO_AES,
        .enc_mode = SAB_CRYPTO_MODE_CBC,
        .auth_algo = SAB_AUTH_HMAC_SHA1,
	}
};

static void populate_params(struct ltq_crypto_ipsec_params *req,
					u8 *enc_key, unsigned int enc_keylen, 
					u8 *input, unsigned int buflen,
					u8* auth_key, unsigned int auth_keylen,
					int enc_algo, int enc_mode, int auth_algo,
					int direction, int ring_no, u32 spi,
					u8 *iv)
{
	req->ipsec_mode = SAB_IPSEC_ESP;
	req->ipsec_tunnel = SAB_IPSEC_TUNNEL;
	req->ip_type = SAB_IPSEC_IPV4;
	req->direction = direction;
	req->ring_no = ring_no;
	req->spi = spi;

	req->data_len = buflen;
	req->cryptosize = buflen;
	req->authsize = 12;

	if (iv)
		memcpy(req->IV, iv, 32);

	req->crypto_algo = enc_algo;
	req->crypto_mode = enc_mode;
	req->crypto_keysize = enc_keylen;
	req->crypto_key = enc_key;

	req->auth_algo = auth_algo;
	req->auth_keysize = auth_keylen;
	req->auth_key = auth_key;

}

void submit_one(struct ltq_ipsec_complete *complete) 
{
	printk("IN-Callback function: err=%d\n", complete->err);
	if (complete->err)
		pr_err("Error processing packet\n");

	engine_out_pkt_len = complete->ret_pkt_len;
	submit_done = 1;
    
	pr_info("callback data @ 0x%08x, pktlen: %d\n", (u32) complete->data, engine_out_pkt_len);
	print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
                 16, 1,
                 complete->data, 16, false);
}

static int __init ltq_submit_one(void)
{
	int ret;
	int i;
	int ring_no = 1;
	unsigned int outpkt_len, buflen;
	u8 *pkt_buf;
	char dummy1[16];
	char dummy2[16];
	struct ltq_crypto_ipsec_params *req_enc;	
	struct ltq_crypto_ipsec_params *req_dec;
	struct test_vector *enc_t;
	
	memset(dummy1, 0xaa, 16);
	memset(dummy2, 0xfa, 16);

	pr_info("Testing ipsec encrypt/decrypt\n");

	for (i = 0; i < ARRAY_SIZE(enc_template); i++) {
		enc_t = &enc_template[i];
		submit_done = 0;

		req_enc = kmalloc(sizeof(struct ltq_crypto_ipsec_params), GFP_KERNEL);
		if (!req_enc)
			return -ENOMEM;
	
		req_dec = kmalloc(sizeof(struct ltq_crypto_ipsec_params), GFP_KERNEL);
		if (!req_dec)
			goto err;

		buflen = enc_t->pktlen;
		pr_info("### inpkt len: %d ### \n", buflen);
		if (enc_t->iv)
			req_enc->flags = LTQ_STATIC_IV;
		populate_params(req_enc, enc_t->enc_key, enc_t->enc_keylen, 
					enc_t->inpkt, 64, enc_t->auth_key, enc_t->auth_keylen,
					enc_t->enc_algo, enc_t->enc_mode, enc_t->auth_algo,
					0, ring_no, enc_t->spi, enc_t->iv);

		/* creating SA & Tokens */
		ret = ltq_ipsec_setkey(req_enc);
		if (ret) {
			pr_err("Error setting encrytion keys\n");
			goto err_out;
		}

		printk("Getting IPsec token\n");
		ret = ltq_get_ipsec_token(req_enc);
		if (ret) {
			pr_err("Error getting tokens enc direction\n");
			goto err_out;
		}
	
		/* larger buffer added for packet expansion */
		outpkt_len = buflen + req_enc->ivsize + req_enc->ICV_length + ESP_HDR + 8;
		printk("outpkt_len: %d, icvlen: %d, ivsize: %d\n", outpkt_len, 
					req_enc->ICV_length, req_enc->ivsize);
		pkt_buf = kmalloc(outpkt_len, GFP_KERNEL);
		if (!pkt_buf)
			goto des_enc;
	
	    print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
   	              16, 1,
   	              enc_t->inpkt, buflen, false);

		printk("Encrypting packet\n");
		ret = ltq_ipsec_enc(req_enc->spi, enc_t->inpkt, pkt_buf, submit_one, buflen,
							dummy1);
		if (ret && ret != -EINPROGRESS) {
			pr_err("Error submiting packet for encryption\n");
			goto des_enc;
		}

		while (submit_done == 0);

	    pr_info("Outbound Outpkt: \n");
   		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
	             16, 1,
    	         pkt_buf, engine_out_pkt_len, false);

		submit_done = 0;

		if (enc_t->iv)
			req_dec->flags = LTQ_STATIC_IV;

		populate_params(req_dec, enc_t->enc_key, enc_t->enc_keylen, 
						pkt_buf, 120, enc_t->auth_key, enc_t->auth_keylen,
						enc_t->enc_algo, enc_t->enc_mode, enc_t->auth_algo,
						1, ring_no, enc_t->spi, enc_t->iv);
	
		/* create SA & Token for decrypt direction */
		ret = ltq_ipsec_setkey(req_dec);
		if (ret) {
			pr_err("Error setting decrytion keys\n");
			goto des_enc;
		}

		ret = ltq_get_ipsec_token(req_dec);
		if (ret) {
			pr_err("Error getting tokens dec direction\n");
			goto des_enc;
		}

		outpkt_len = buflen;
		/* Decrypting the packet */
   		ret = ltq_ipsec_dec(req_dec->spi, pkt_buf, pkt_buf, submit_one, engine_out_pkt_len,
							dummy2);
	    if (ret && ret != -EINPROGRESS) {
   	    	pr_err("Error submiting packet for decryption\n");
        	goto des_dec;
    	}
		
		while (submit_done == 0);

		/* Compare original and decrypted packets are the same */
		if (memcmp(pkt_buf, enc_t->inpkt, outpkt_len)) {
			pr_err("Error decrypting pkt\n");
			pr_info("Expected data: \n");
   	 		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
           			      16, 1,
		                  enc_t->inpkt, buflen, false);
	    	pr_info("outbuf: \n");
    		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
                 			16, 1,
                 			pkt_buf, buflen, false);
		} else {
			pr_info("Inbound PASS!!!! Outpkt: \n");
    		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
        	         16, 1,
                 	pkt_buf,buflen, false);
		}

des_dec:
		ltq_destroy_ipsec_sa(req_dec);
des_enc:
		ltq_destroy_ipsec_sa(req_enc);
		if (pkt_buf)
			kfree(pkt_buf);
err_out:
		kfree(req_dec);
err:
		kfree(req_enc);
	}

	/* Don't keep module in the kernel */
	return -EAGAIN;
}


static void __exit ltq_submit_exit(void)
{
    return;
}

module_init(ltq_submit_one);
module_exit(ltq_submit_exit);

#endif /* CONFIG_LTQ_MPE_IPSEC_SUPPORT */
