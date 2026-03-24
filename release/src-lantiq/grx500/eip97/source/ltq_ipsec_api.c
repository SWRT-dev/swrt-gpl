/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/******************************************************************************
**
** FILE NAME    : ltq_ipsec_api.c
** PROJECT      : GRX500
** MODULES      : crypto hardware
**
** DATE         : 10 May 2016
** AUTHOR       : Mohammad Firdaus B Alias Thani
** DESCRIPTION  : Hardware accelerated Crypto IPsec Drivers
**
*******************************************************************************/

#include <linux/crypto.h>
#include <linux/completion.h>
#include <linux/kernel.h>
#include <linux/rtnetlink.h>
#include <linux/gfp.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/clk.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <linux/atomic.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/irq.h>             // IRQ_TYPE_LEVEL_HIGH
#include <linux/irqreturn.h>       // irqreturn_t
#include <linux/dma-mapping.h>  // dma_sync_single_for_cpu, dma_alloc_coherent,
#include <linux/proc_fs.h>

#include <net/xfrm.h>
//#if defined(CONFIG_LTQ_PPA_API) || defined(CONFIG_LTQ_PPA_API_MODULE)
#include <net/ppa/ppa_api.h>
//#endif
#include <crypto/ctr.h>
#include <crypto/des.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <crypto/md5.h>
#include <crypto/hash.h>
#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/internal/hash.h>
#include <crypto/ltq_ipsec_ins.h>
#include <crypto/authenc.h>
#include <crypto/internal/hash.h>

#include <lantiq_soc.h>

#include "ltq_crypto_core.h"
#include "ltq_ipsec_api.h"

#include "device_rw.h"
#include "dmares_types.h"
#include "api_dmabuf.h"
#include "api_pec.h"
#include "api_pec_sg.h"
#include "device_mgmt.h"
#include "sa_builder.h"
#include "sa_builder_ipsec.h"
#include "token_builder.h"


#ifdef DRIVER_PERFORMANCE
#define DEBUG_INFO				pr_debug
//#define IPSEC_DEBUG				0
#else
#define IPSEC_DEBUG				1
#define DEBUG_INFO				pr_info
#endif

#define IOCU_MODE				1
//#define SYNC_CRYPT				1
#define LTQ_ALIGN               4
#define DA_PEC_BANK_SA          0
#define DA_PEC_BANK_TOKEN       0
#define DA_PEC_BANK_PACKET      0
#define AES_MAX_KEYSIZE			32
#define CTR_RFC3686_NONCE_SIZE	4

#define INS_MASK				0xF0000000
#define INS_DIRECTION			0
#define INS_INSERT				0x20000000
#define RETRIEVE_INS			0x40000000
#define ORIGIN_MASK				0x00F80000
#define ORIGIN_IPSEC_PAD		0x00200000
#define HASH_ICVLEN				0x00E00000	
#define ICVLEN_MASK				0x0000FFFF
#define STAT_MASK				0x00060000
#define STAT_PAD				0x00020000
#define BLK_CNT_ESP 			4
#define BLK_CNT_AES 			16
#define MASK_BLK_CNT_ESP		0xfffffffc // ~(MASK_BLK_CNT_ESP - 1)
#define MASK_BLK_CNT_AES 		0xfffffff0  // ~(MASK_BLK_CNT_AES - 1)
#define LTQ_PROC_DIR            "driver/ltq_eip97"
#define MAX_QUEUE_LEN			500


#ifndef SYNC_CRYPT
#define CDR_DESC_SIZE			0x8
#define RDR_DESC_SIZE			0x8
#define MAX_DESC_LIST_SIZE		1000
#else
#define CDR_DESC_SIZE			0x6
#define RDR_DESC_SIZE			0x6
#define MAX_DESC_LIST_SIZE		1
#endif

static int cur_irq = -1;
static spinlock_t ipsec_lock;
static spinlock_t list_lock;
static Device_Handle_t Global_Base;
static int cur_ring = -1;
static unsigned int cur_max_pkt = 0;
static unsigned int desc_offset;
static unsigned int rdr_desc_offset;
static unsigned int max_queue_size;
static struct proc_dir_entry *ipsec_root_dir;
atomic_t fifo_queue;
atomic_t pkt_err;

struct ipsec_list {
	struct list_head list;
	struct ltq_crypto_ipsec_params *params;
	struct ltq_crypto_ipsec_params  ref;
	u32 spi;
};

struct ltq_ipsec_complete *ipsec_done_ptr = NULL;
u32 *cdr_fifo_ptr = NULL;
u32 *rdr_fifo_ptr = NULL;

LIST_HEAD(ipsec_list);

#ifdef IPSEC_DEBUG
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
#endif /* IPSEC_DEBUG */
int EIP97_notify(struct xfrm_state *x, const struct km_event *c);
int ltq_ipsec_construct_token(struct xfrm_state *xfrm_sa,int dir);
int32_t get_auth_algo(char *algo);
int32_t get_crypto_algo(char *algo);
int32_t get_crypto_mode_algo(char *alg, char *mode, char *algo);
int32_t get_crypto_mode(char *mode);

static int EIP97_policy(struct xfrm_policy *xp, int dir, const struct km_event *c)
{

	return 0;

}
static int EIP97_acquire(struct xfrm_state *x, struct xfrm_tmpl *xt,
			     struct xfrm_policy *xp)
{
	return 0;
}

static struct xfrm_policy *EIP97_compile(struct sock *sk, int opt,
					       u8 *data, int len, int *dir)
{
	return 0;
}

static int EIP97_report(struct net *net, u8 proto,
			    struct xfrm_selector *sel, xfrm_address_t *addr)
{
	return 0;
}


static int EIP97_migrate(const struct xfrm_selector *sel, u8 dir, u8 type,
			     const struct xfrm_migrate *m, int num_migrate,
			     const struct xfrm_kmaddress *k)
{
	return 0;
}

static int EIP97_mapping(struct xfrm_state *x, xfrm_address_t *ipaddr,
			     __be16 sport)
{
	return 0;
}

int32_t get_auth_algo(char *algo)
{
	if(!strcmp(algo, "digest_null"))
		return LTQ_SAB_AUTH_NULL;
	if(!strcmp(algo, "hash(md5)"))
		return LTQ_SAB_AUTH_HASH_MD5;
	if(!strcmp(algo, "hmac(md5)"))
		return LTQ_SAB_AUTH_HMAC_MD5;
	if(!strcmp(algo, "hmac(sha1)"))
		return LTQ_SAB_AUTH_HMAC_SHA1;
	if(!strcmp(algo, "hmac(sha256)"))
		return LTQ_SAB_AUTH_HMAC_SHA2_256;
	if(!strcmp(algo, "hmac(sha384)"))
		return LTQ_SAB_AUTH_HMAC_SHA2_384;
	if(!strcmp(algo, "hmac(sha512)"))
		return LTQ_SAB_AUTH_HMAC_SHA2_512;
	if(!strcmp(algo, "xcbc(aes)"))
		return LTQ_SAB_AUTH_AES_XCBC_MAC;
	if(!strcmp(algo, "gcm"))
		return LTQ_SAB_AUTH_AES_GCM;
	if(!strcmp(algo, "gmac"))
		return LTQ_SAB_AUTH_AES_GMAC;

	return -EINVAL;
}
int32_t get_crypto_algo(char *algo)
{
	if(!strcmp(algo, "cipher_null"))
		return LTQ_SAB_CRYPTO_NULL;
	if(!strcmp(algo, "des"))
		return LTQ_SAB_CRYPTO_DES;
	if(!strcmp(algo, "3des"))
		return LTQ_SAB_CRYPTO_3DES;
	if(!strcmp(algo, "aes"))
		return LTQ_SAB_CRYPTO_AES;
	if(!strcmp(algo, "arcfour"))
		return LTQ_SAB_CRYPTO_ARCFOUR;

	return -EINVAL;
}
int32_t get_crypto_mode(char *mode)
{
	if(!strcmp(mode, "ecb"))
		return LTQ_SAB_CRYPTO_MODE_ECB;
	if(!strcmp(mode, "cbc"))
		return LTQ_SAB_CRYPTO_MODE_CBC;
	if(!strcmp(mode, "ofb"))
		return LTQ_SAB_CRYPTO_MODE_OFB;
	if(!strcmp(mode, "cfb"))
		return LTQ_SAB_CRYPTO_MODE_CFB;
	if(!strcmp(mode, "cfb1"))
		return LTQ_SAB_CRYPTO_MODE_CFB1;
	if(!strcmp(mode, "cfb8"))
		return LTQ_SAB_CRYPTO_MODE_CFB8;
	if(!strcmp(mode, "ctr"))
		return LTQ_SAB_CRYPTO_MODE_CTR;
	if(!strcmp(mode, "icm"))
		return LTQ_SAB_CRYPTO_MODE_ICM;
	if(!strcmp(mode, "ccm"))
		return LTQ_SAB_CRYPTO_MODE_CCM;
	if(!strcmp(mode, "gcm"))
		return LTQ_SAB_CRYPTO_MODE_GCM;
	if(!strcmp(mode, "gmac"))
		return LTQ_SAB_CRYPTO_MODE_GMAC;

	return -EINVAL;

}
int32_t get_crypto_mode_algo(char *alg, char *mode, char *algo )
{
        int32_t i=0;
        uint8_t alg1[64] = "";
	char tmp[10];
        int slen=0;

        uint8_t *dash = strchr(alg, '(');
	
	if(dash == NULL)
		return -EINVAL;

        strncpy(tmp, alg, 3 );
        tmp[3] = '\0';
        slen = strlen(dash +1 ) -1;
        if(!strcmp(tmp, "rfc")) {
                alg = dash+1;
                dash = strchr(alg, '(');
                slen = strlen(dash+1) -2;
        }

        for(i=0; *(alg+i) != '('; i++)
        {
                alg1[i] = *(alg+i);
        }

        alg1[i] = '\0';
	strcpy(mode, alg1);
        strncpy(algo, dash+1, slen);

	return 0;
}
static unsigned int get_pad_blksize(u32 crypto_alg, u32 crypto_mode)
{
	unsigned int pad_blksize = 4;

	switch (crypto_alg) {
	case SAB_CRYPTO_3DES:
	case SAB_CRYPTO_DES:
		pad_blksize = 8;
		break;
	case SAB_CRYPTO_AES:
		if (crypto_mode == SAB_CRYPTO_MODE_CBC)
			pad_blksize = 16;
		break;
	}

	return pad_blksize;
}

static unsigned int get_IVsize(u32 crypto_alg, u32 crypto_mode)
{
	int IVWords = 0;
	unsigned int IVBytes = 0;

	if (crypto_alg == SAB_CRYPTO_AES) {
		IVWords = 4;
	} else if (crypto_alg == SAB_CRYPTO_DES ||
				crypto_alg == SAB_CRYPTO_3DES) {
		IVWords = 2;
	}

	switch (crypto_mode) {
	case SAB_CRYPTO_MODE_CTR:
	case SAB_CRYPTO_MODE_CCM:
	case SAB_CRYPTO_MODE_GMAC:
	case SAB_CRYPTO_MODE_GCM:
		IVBytes = IVWords * 2;
		break;
	case SAB_CRYPTO_MODE_ECB:
	case SAB_CRYPTO_MODE_CBC:
	case SAB_CRYPTO_MODE_ICM:
	case SAB_CRYPTO_MODE_CFB:
	case SAB_CRYPTO_MODE_CFB1:
	case SAB_CRYPTO_MODE_CFB8:
	case SAB_CRYPTO_MODE_OFB:
		IVBytes = IVWords * 4;
		break;
	default:
		IVBytes = 0;
	}

	return IVBytes;
}

static unsigned int get_ICV_length(struct ltq_crypto_ipsec_params *req)
{
	int i = 0;
	unsigned int token_words = req->tokenwords;
	u32 *token_p = req->token_buffer;
	
	for (i = 0; i < token_words; i++) {
		if ((token_p[i] & INS_MASK) == INS_INSERT &&
				req->direction == SAB_DIRECTION_OUTBOUND) {
			if ((token_p[i] & ORIGIN_MASK) == HASH_ICVLEN)
				return (token_p[i] & ICVLEN_MASK);
		} else if ((token_p[i] & INS_MASK) == RETRIEVE_INS &&
				req->direction == SAB_DIRECTION_INBOUND) {
			if ((token_p[i] & ORIGIN_MASK) == HASH_ICVLEN)
				return (token_p[i] & ICVLEN_MASK);
		}
	}

	return 0;
}

static void ltq_get_offset(struct ltq_crypto_ipsec_params *req)
{
	int i;
	unsigned int token_words = req->tokenwords;
	u32 crypto_mode = req->crypto_mode;
	u32 *token_p = req->token_buffer;

	req->msg_len_offs = 0;
	req->hash_pad_offs = 0;
	req->crypto_offs = 0;

	if (crypto_mode != SAB_CRYPTO_MODE_CCM) {
		/* non-AEAD crypto ipsec mode */
		for (i = 0; i < token_words; i++) {
			if ((token_p[i] & INS_MASK) == INS_DIRECTION &&
				(token_p[i] & ORIGIN_MASK) == 0) {
				req->crypto_offs = i;
			} 

			/* We are done for INBOUND traffic */
			if (req->crypto_offs != 0 &&
				req->direction == SAB_DIRECTION_INBOUND)
				break;
			
			if ((token_p[i] & ORIGIN_MASK) == ORIGIN_IPSEC_PAD &&
				(crypto_mode == SAB_CRYPTO_MODE_ECB || 
				crypto_mode == SAB_CRYPTO_MODE_CBC ||
				crypto_mode == SAB_CRYPTO_MODE_GCM)) {
				/* padding offset */
				req->total_pad_offs = i;
			}
		}
	} else {
		if(req->direction == SAB_DIRECTION_OUTBOUND)
			req->msg_len_offs = 4;
		else
			req->msg_len_offs = 5;

		for (i = 0; i < token_words; i++) {
            if ((token_p[i] & INS_MASK) == INS_DIRECTION &&
                (token_p[i] & ORIGIN_MASK) == 0) {
                req->crypto_offs = i;
            } else if (((token_p[i] & INS_MASK) == INS_INSERT) &&
					((token_p[i] & ORIGIN_MASK)== ORIGIN_IPSEC_PAD) &&
					req->direction == SAB_DIRECTION_OUTBOUND) {
				req->total_pad_offs = i;
			} else if (((token_p[i] & INS_MASK) == INS_INSERT) &&
					((token_p[i] & ORIGIN_MASK) == 0) &&
					((token_p[i] & STAT_MASK) == STAT_PAD)) {
				req->hash_pad_offs = i;
				break;
			}
		}
	}
}

int ltq_ipsec_setkey(struct ltq_crypto_ipsec_params *ipsec_params)
{
	int ret = 0;
	int bs;
	unsigned int base_hash_ds;
	u8 keys[AES_MAX_KEYSIZE];
	u8 nonce[16];
	u32 crypto_mode = ipsec_params->crypto_mode; 
	u32 hash_flag = ipsec_params->auth_algo;
	struct crypto_shash *shash = NULL;
	struct crypto_tfm *tfm = NULL; 		/* dummy, not used */
    	void (*func)(u32 *ipad, u32 *opad,
        void *istate,
        void *ostate,
        unsigned int len);	
	
	if (hash_flag == SAB_AUTH_AES_CCM) {
		/* Minimum key size */
		if (ipsec_params->crypto_keysize <= 3)
			return -EINVAL;
		if (ipsec_params->crypto_keysize > 32)
			return -EINVAL;

		memcpy(keys, ipsec_params->crypto_key, 
		ipsec_params->crypto_keysize);
		ipsec_params->crypto_keysize -= 3;
    		memcpy(ipsec_params->crypto_key, keys, 
			ipsec_params->crypto_keysize);
	    	memcpy(ipsec_params->nonce, keys + ipsec_params->crypto_keysize, 3);
		return 0;

	} else if (hash_flag == SAB_AUTH_AES_XCBC_MAC) {
			ret = ltq_xcbc_precompute(ipsec_params->ipad,
								ipsec_params->opad,
								ipsec_params->authkey3,
								ipsec_params->auth_key,
								ipsec_params->auth_keysize);
		if (ret) {
			pr_err("Error precomputing GCM keys\n");
			return -EINVAL;
		}

		return 0;

	} else if (hash_flag == SAB_AUTH_AES_GCM ||
				hash_flag == SAB_AUTH_AES_GMAC) {
		/* Minimum key size */
		if (ipsec_params->crypto_keysize <= 4)
			return -EINVAL;
		if (ipsec_params->crypto_keysize > 32)
			return -EINVAL;
	
		memcpy(keys, ipsec_params->crypto_key, 
		/* Copy original keys to dummy buffer */
				ipsec_params->crypto_keysize);
		
		ipsec_params->crypto_keysize -= 4;
		/* Separate keys to keys and nonce */
		memcpy(ipsec_params->crypto_key, keys, 
				ipsec_params->crypto_keysize);
		memcpy(nonce, keys + ipsec_params->crypto_keysize, 4);

		/* Compute H */
		ret = ltq_gcm_precompute_keys(ipsec_params->ipad, 
									keys, 
									ipsec_params->crypto_keysize);
		if (ret) {
			pr_err("Error precomputing GCM keys\n");
			return -EINVAL;
		}
		
		memcpy(ipsec_params->nonce,  nonce, 4);
		return 0;

	} else if (crypto_mode == SAB_CRYPTO_MODE_CTR) {
		memcpy(nonce, 
		ipsec_params->crypto_key + (ipsec_params->crypto_keysize - CTR_RFC3686_NONCE_SIZE), 
		CTR_RFC3686_NONCE_SIZE);

		ipsec_params->crypto_keysize -= CTR_RFC3686_NONCE_SIZE;
		memcpy(ipsec_params->nonce,  nonce, CTR_RFC3686_NONCE_SIZE);
		return 0;

	} else if (hash_flag == SAB_AUTH_HMAC_SHA1) {
       		func = ltq_sha1_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "sha1");
	} else if (hash_flag == SAB_AUTH_HMAC_MD5) {
        	func = ltq_md5_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "md5");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_224) {
        	func = ltq_sha224_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "sha224");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_256) {
        	func = ltq_sha256_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "sha256");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_384) {
        	func = ltq_sha512_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "sha384");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_512) {
        	func = ltq_sha512_hmac_copy_keys;
		shash = ltq_init_hmac(tfm, "sha512");
	} else
		return 0;

	if (!shash) {
		pr_err("Error allocating base hash in %s\n", 
				__func__);
		return -EINVAL;
	}

	if (SAB_AUTH_HMAC_SHA2_224)
        	base_hash_ds = SHA256_DIGEST_SIZE;
    	else
        	base_hash_ds = crypto_shash_digestsize(shash);

	bs = crypto_shash_blocksize(shash);

    	ret = ltq_hmac_precompute((u32 *)ipsec_params->ipad, (u32 *)ipsec_params->opad,
                            shash, base_hash_ds,
                            ipsec_params->auth_key, 
							ipsec_params->auth_keysize,
                            (ipsec_params->auth_keysize > bs) ? 1 : 0,
                            func);
    	if (ret) {
        	pr_err("Error Precomputing ipad/opad\n");
        return ret;
    }

	return 0;
}
EXPORT_SYMBOL(ltq_ipsec_setkey);

static int ltq_create_sa(struct ltq_crypto_ipsec_params *req)
{
	int rc = 0;
	unsigned int SAWords = 0;
	unsigned int TCRWords = 0;
	unsigned int ring = req->ring_no;
	unsigned int ivsize;
	u32 *TCRData;
    	SABuilder_Params_t params;
	SABuilder_Params_IPsec_t IPsecParams;
    	DMABuf_Status_t DMAStatus;
    	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
    	DMABuf_Handle_t SAHandle = {0};
    	DMABuf_HostAddress_t SAHostAddress;
	if (ring == 0 || ring > 3) {
		pr_err("Invalid ring no %d requested, only supports Ring 1 - 3", ring);
		return -EINVAL;
	}

	DEBUG_INFO("### DEBUG Params: ####\n");
	DEBUG_INFO("### IPsec Mode: %s, Tunnel/Transport: %s, IP mode: IPv%d\n",
			((req->ipsec_mode & SAB_IPSEC_ESP) ? "ESP" : "AH"),
			((req->ipsec_tunnel & SAB_IPSEC_TUNNEL) ? "Tunnel" : "Transport"),
			((req->ip_type & SAB_IPSEC_IPV4) ? 4 : 6));
	DEBUG_INFO("Crypto algo: 0x%08x, Crypto Mode: 0x%08x, Auth Algo: 0x%08x\n",
			req->crypto_algo, req->crypto_mode, req->auth_algo);
	DEBUG_INFO("### Direction : %s, spi 0x%08x\n", ((req->direction) ? "Inbound" : "Outbound"), req->spi);
	DEBUG_INFO("params: data length: %d, crypto len: %d, auth size: %d\n",
			req->data_len, req->cryptosize, req->authsize);
	DEBUG_INFO("crypto key size: %d, auth keysize: %d\n", req->crypto_keysize, req->auth_keysize);

	rc = SABuilder_Init_ESP(&params, &IPsecParams, req->spi, req->ipsec_tunnel,
		                   		SAB_IPSEC_IPV4, req->direction);
	if (rc) {
		pr_err("SA Builder Initialization failed\n");
		rc = -EINVAL;	
		return rc;
	}

	ivsize = get_IVsize(req->crypto_algo, req->crypto_mode);
	req->ivsize = ivsize;

	/* IV has to be populated in req->IV before calling API if LTQ_STATIC_IV is flag used 
	 * This LTQ_STATIC_IV flag is for debugging purposes only
	 */
	if (ivsize && ((req->flags & LTQ_STATIC_IV) == 0)) {
		get_random_bytes(req->IV, ivsize);
	}

	// Disable anti-replay for now
	IPsecParams.IPsecFlags |= SAB_IPSEC_NO_ANTI_REPLAY;
	IPsecParams.IPsecFlags |= (req->ipsec_tunnel | req->ipsec_mode | req->ip_type);

	/* Inbound packet */
	if (req->direction) {
		params.IVSrc |= SAB_IV_SRC_INPUT;
		IPsecParams.ICVByteCount = req->authsize;
	} else {
		/* Save info in SA so that we can reuse */
		params.flags = SAB_FLAG_IV_SAVE;
		params.IVSrc |= SAB_IV_SRC_SA;
		params.IV_p = req->IV;
	}

    // Add crypto key and parameters.
    	params.CryptoAlgo = req->crypto_algo;
    	params.CryptoMode = req->crypto_mode;
	if (params.CryptoAlgo != SAB_CRYPTO_NULL) {
	    	params.KeyByteCount = req->crypto_keysize;
    		params.Key_p = req->crypto_key;
	}

    // Add authentication key and paramters.
	// Assume IV is in the input packet
    params.AuthAlgo = req->auth_algo;
	switch (params.AuthAlgo) {
	case SAB_AUTH_HASH_MD5:
	case SAB_AUTH_HASH_SHA1:
	case SAB_AUTH_HASH_SHA2_224:
	case SAB_AUTH_HASH_SHA2_256:
	case SAB_AUTH_HASH_SHA2_384:
	case SAB_AUTH_HASH_SHA2_512:
	case SAB_AUTH_HMAC_MD5:
	case SAB_AUTH_HMAC_SHA1:
	case SAB_AUTH_HMAC_SHA2_224:
	case SAB_AUTH_HMAC_SHA2_256:
	case SAB_AUTH_HMAC_SHA2_384:
	case SAB_AUTH_HMAC_SHA2_512:
		params.AuthKey1_p = req->ipad;
   		params.AuthKey2_p = req->opad;
		break;
	case SAB_AUTH_AES_XCBC_MAC:
		params.AuthKey1_p = req->ipad;
   		params.AuthKey2_p = req->opad;
		params.AuthKey3_p = req->authkey3;	
		break;
	case SAB_AUTH_AES_CCM:
	case SAB_AUTH_AES_GCM:
	case SAB_AUTH_AES_GMAC:	
		params.AuthKey1_p = req->ipad;
		break;
	default:
		pr_debug("No Auth algo\n");
	}

	if (params.CryptoMode == SAB_CRYPTO_MODE_CCM ||
        params.CryptoMode == SAB_CRYPTO_MODE_GCM ||
        params.CryptoMode == SAB_CRYPTO_MODE_GMAC ||
        params.CryptoMode == SAB_CRYPTO_MODE_CTR) {
        params.Nonce_p = req->nonce;
    }
	
	rc = SABuilder_GetSizes(&params, &SAWords, NULL, NULL);	
	if (rc) {
		pr_err("SABuilder_GetSizes failed\n");
		rc = -EINVAL;	
		return rc;
	}
	
	dma_alloc_properties(&DMAProperties,
                    true, (4 * SAWords), LTQ_ALIGN,
                    DA_PEC_BANK_PACKET);
    
	DMAStatus = DMABuf_Alloc(DMAProperties, &SAHostAddress, &SAHandle);
    if (DMAStatus != DMABUF_STATUS_OK) {
        pr_err("%s: Error allocating DMA buffer for SA\n", __func__);
        return -EAGAIN;
    }
	
	rc = SABuilder_BuildSA(&params, (uint32_t *)SAHostAddress.p, NULL, NULL);
    if (rc != 0) {
        pr_err("%s: Error building SA\n", __func__);
        DMABuf_Release(SAHandle);
        return -EINVAL;
    }

    rc = TokenBuilder_GetContextSize(&params, &TCRWords);

    if (rc != 0) {
        pr_err("TokenBuilder_GetContextSize returned errors\n");
        DMABuf_Release(SAHandle);
		return -EINVAL;
    }

	TCRData = (u32 *) kmalloc(4 * TCRWords, GFP_KERNEL);
	if (!TCRData) {
		pr_err("Error allocating mem for context records\n");
		DMABuf_Release(SAHandle);
		return -ENOMEM;
	}

    rc = TokenBuilder_BuildContext(&params, TCRData);
    if (rc != 0) {
        pr_err("TokenBuilder_BuildContext failed\n");
		kfree(TCRData);
        DMABuf_Release(SAHandle);
		return -EINVAL;
    }

	// Register the SAs.
    rc = PEC_SA_Register(req->ring_no, SAHandle, DMABuf_NULLHandle,
                      DMABuf_NULLHandle);
    if (rc != PEC_STATUS_OK)
    {
        pr_err("PEC_SA_Register failed\n");
        kfree(TCRData);
        DMABuf_Release(SAHandle);
		return -EINVAL;
    }

	req->SAHandle = SAHandle;
	req->SA_buffer = (u32 *)SAHostAddress.p;
	req->SAWords = SAWords;
	req->context_data = TCRData;

	return 0;

}

static int ltq_build_token(struct ltq_crypto_ipsec_params *req)
{
	int rc = 0;
	unsigned int TokenMaxWords = 0;
	unsigned int TokenWords = 0;
	unsigned int TokenHeaderWord;
	unsigned int dst_size, src_size;
	u8 buf[128];
	TokenBuilder_Params_t TokenParams;
	u32 *TCRData = req->context_data;

	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	DMABuf_HostAddress_t TokenAddress;
	DMABuf_Handle_t SAHandle = req->SAHandle;
	DMABuf_Handle_t TokenHandle = {0};
	DMABuf_Handle_t SrcHandle = {0};
	DMABuf_Handle_t DstHandle = {0};

	DMABuf_Status_t DMAStatus;

    rc = TokenBuilder_GetSize(TCRData, &TokenMaxWords);
    if (rc != 0) {
        pr_err("TokenBuilder_GetSize failed\n");
		rc = -EINVAL;
        goto err_getsz;
    }

    dma_alloc_properties(&DMAProperties,
                    true, (4 * TokenMaxWords),
                    LTQ_ALIGN,
                    DA_PEC_BANK_PACKET);	

    DMAStatus = DMABuf_Alloc(DMAProperties, &TokenAddress, &TokenHandle);
    if (DMAStatus != DMABUF_STATUS_OK) {
        pr_err("%s: Failed to allocate Token DMA buf \n", __func__);
        rc = -EAGAIN;
		goto err_getsz;
    }

	if (req->direction == SAB_DIRECTION_OUTBOUND) {
		if (req->crypto_algo == SAB_CRYPTO_NULL) {
			src_size = req->data_len;
			dst_size = req->data_len + req->authsize;
		}
		else {
			src_size = req->cryptosize;
			dst_size = req->cryptosize + req->authsize;
		}
	} else {
		if (req->crypto_algo == SAB_CRYPTO_NULL) {
			src_size = req->data_len  + req->authsize;
			dst_size = req->data_len - req->authsize;
		}
		else {
			src_size = req->cryptosize + req->authsize;
			dst_size = req->cryptosize;
		}
	}

    /* Register src buffer */
    dma_alloc_properties(&DMAProperties,
                true, src_size,
                LTQ_ALIGN,
                DA_PEC_BANK_PACKET);

	DMABuf_Register(DMAProperties, buf, NULL, 'k', &SrcHandle);
    if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Failed to register Src DMA buf\n", __func__);
		rc = -EAGAIN;
        goto err_tkn;
	}

    /* Register dst buffer */
    dma_alloc_properties(&DMAProperties,
                true, dst_size,
                LTQ_ALIGN,
                DA_PEC_BANK_PACKET);
	
    DMABuf_Register(DMAProperties, buf, NULL, 'k', &DstHandle);
    if (DMAStatus != DMABUF_STATUS_OK) {
        pr_err("%s: Failed to register Src DMA buf\n", __func__);
        rc = -EAGAIN;
        goto err_src;
    }

    TokenParams.PacketFlags = 0;
    TokenParams.PadByte = 0x04;
    TokenParams.BypassByteCount = 0;
	TokenParams.AdditionalValue = 0;
	TokenParams.AAD_p = NULL;

    rc = TokenBuilder_BuildToken(TCRData,
                        (uint8_t *) SrcHandle.p,
                        src_size,
                        &TokenParams,
                        (uint32_t *) TokenAddress.p,
                        &TokenWords,
                        &TokenHeaderWord);
    if (rc != TKB_STATUS_OK) {
        if (rc == TKB_BAD_PACKET)
            pr_err("Token not created because packet size is invalid\n");
        else
            pr_err("Token builder failed\n");
		rc = -EINVAL;	
		goto err_dst;
	}

	req->token_buffer = (u32 *) TokenAddress.p;
	req->tokenwords = TokenWords;
	req->token_headerword = TokenHeaderWord;

	req->TokenHandle =  TokenHandle;
	req->SrcHandle = SrcHandle;
	req->DstHandle = DstHandle;

	return 0;

err_dst:	
	DMABuf_Release(DstHandle);
err_src:
	DMABuf_Release(SrcHandle);
err_tkn:
    DMABuf_Release(TokenHandle);
err_getsz:
	kfree(TCRData);
	DMABuf_Release(SAHandle);
	return rc;
}

int ltq_get_ipsec_token(struct ltq_crypto_ipsec_params *ipsec_params)
{
	int ret;
	struct ipsec_list *ipsec_req;

	ipsec_req = (struct ipsec_list *) kmalloc(sizeof(struct ipsec_list),
												GFP_KERNEL);
	if (!ipsec_req)
		return -ENOMEM;
	ipsec_req->params = &ipsec_req->ref;

	ret = ltq_create_sa(ipsec_params);
	if (ret)
		return ret;
		
	ret = ltq_build_token(ipsec_params);
	if (ret)
		return ret;

	ltq_get_offset(ipsec_params);
	
	ipsec_params->pad_blksize = get_pad_blksize(ipsec_params->crypto_algo, 
											ipsec_params->crypto_mode);
	ipsec_params->ICV_length = get_ICV_length(ipsec_params);
	if (!ipsec_params->ICV_length) {
		pr_err("ICV length returned zero\n");
		return -EINVAL;
	}

	memcpy(ipsec_req->params,ipsec_params,sizeof(struct ltq_crypto_ipsec_params));
	ipsec_req->spi = ipsec_params->spi;

	spin_lock_bh(&list_lock);
	list_add(&ipsec_req->list, &ipsec_list);
	spin_unlock_bh(&list_lock);

#ifdef IPSEC_DEBUG
	debug_params(ipsec_params);
#endif

	return 0;
	
}
EXPORT_SYMBOL(ltq_get_ipsec_token);

void debug_ipsec_list(void)
{
#ifdef IPSEC_DEBUG
	struct list_head *ptr;
	struct ipsec_list *entry;
	int i = 0;

	if (list_empty(&ipsec_list)) {
		DEBUG_INFO("List is empty\n");
		return;
	}

	list_for_each(ptr, &ipsec_list) {
		entry = list_entry(ptr, struct ipsec_list, list);
		if (!entry) 
			break;
		DEBUG_INFO("SPI entry @ %d is 0x%08x\n", i, entry->spi);
		i++;
	}
#endif
	return;
}

static void parse_err(int err_code)
{
	int rol;
	int code;
	
	pr_debug("Error code: 0x%08x - ", err_code);

	for (rol = 0; rol < 16; rol++) {
		code = (err_code >> rol) & 0x1;
		if (code) {
			switch (rol) {
			case 0:
				pr_debug("Packet Length Error\n");
				break;
			case 1:
				pr_debug("Token error, Unknown instructions\n");
				break;
			case 2:
				pr_debug("Token contains too many bypass data\n");
				break;
			case 3:
				pr_debug("Cryptographic blk error\n");
				break;
			case 4:
				pr_debug("Hash blksize error\n");
				break;
			case 5:
				pr_debug("Invalid command\n");
				break;
			case 6:
				pr_debug("Prohibited algorithm\n");
				break;
			case 7:
				pr_debug("Hash input overflow\n");
				break;
			case 8:
				pr_debug("TTL/Hop-linit Underflow\n");
				break;
			case 9:
				pr_debug("Authentication failed\n");
				break;
			case 10:
				pr_debug("Sequence number check failed\n");
				break;
			case 11:
				pr_debug("SPI check failed\n");
				break;
			case 12:
				pr_debug("Checksum incorrect\n");
				break;
			case 13:
				pr_debug("Pad verification failed\n");
				break;
			case 14:
				pr_debug("Timeout\n");
				break;
			default:
				pr_debug("E15 bit set\n");
			}
		}
	}
}

#ifdef SYNC_CRYPT
static void get_packet(unsigned long data)
{
	int err;
	int no_pkt_read;
	unsigned int ret_pkt_len;
	u32 *rdr_ptr;
	u32 reg;
	struct device *DMADevice_p = NULL;
	struct ltq_ipsec_complete *done;
	dma_addr_t DMAaddr;

	reg = Device_Read32(Global_Base, RDR_STAT_OFFSET(cur_ring));
	while ((reg & 0x00000010) != 0x00000010) {
		reg = Device_Read32(Global_Base, RDR_STAT_OFFSET(cur_ring));
	};

	reg = Device_Read32(Global_Base, RDR_PROC_COUNT_OFFSET(cur_ring));

	no_pkt_read = ((reg & 0x00fffffc) >> 2);
	no_pkt_read = (no_pkt_read / (RDR_DESC_SIZE));

	done = &ipsec_done[rdr_desc_offset];
	DMAaddr = dma_map_single(DMADevice_p,
					&rdr_fifo[rdr_desc_offset * RDR_DESC_SIZE],
                    RDR_DESC_SIZE * 4,
                     DMA_FROM_DEVICE);

	rdr_ptr = __va(DMAaddr);
	ret_pkt_len = rdr_ptr[0];

	/* RDR packet seems to give more bytes */
	done->ret_pkt_len = (ret_pkt_len & 0x1ffff);

	err = ((rdr_ptr[2] >> RDR_ERR_E0_BIT) & 0x7fff);
	err |= ((rdr_ptr[3] & E15_MASK) << 11);

	Device_Write32(Global_Base, RDR_STAT_OFFSET(cur_ring), 0x10);
	Device_Write32(Global_Base, RDR_PROC_COUNT_OFFSET(cur_ring), 0x18);	
	Device_Write32(Global_Base, RDR_PREP_PNTR_OFFSET(cur_ring), 0x0); 
	Device_Write32(Global_Base, RDR_PROC_PNTR_OFFSET(cur_ring), 0x0); 
	Device_Write32(Global_Base, CDR_PREP_PNTR_OFFSET(cur_ring), 0x0); 
	Device_Write32(Global_Base, CDR_PROC_PNTR_OFFSET(cur_ring), 0x0); 

	if (err)
		err = -EBADMSG;
	
	done->err = err;
	done->callback_fn(done);
	atomic_dec(&fifo_queue);

	return;
}	
#else
static void get_packet(unsigned long data)
{
	int err;
	int i=0, no_pkt_read=0;
	unsigned int ret_pkt_len=0, max_read=0;
	u32 *rdr_ptr;
	u32 reg=0;
	struct ltq_ipsec_complete *done;
#ifndef IOCU_MODE
	struct device *DMADevice_p = NULL;
	dma_addr_t DMAaddr;
#endif /* IOCU_MODE */

	reg = Device_Read32(Global_Base, RDR_PROC_COUNT_OFFSET(cur_ring));
	no_pkt_read = ((reg & 0x00fffffc) >> 2);
	no_pkt_read = (no_pkt_read / (RDR_DESC_SIZE));
	max_read = min(no_pkt_read, MAX_QUEUE_LEN);

	if (max_read > cur_max_pkt)
		cur_max_pkt = max_read;

	Device_Write32(Global_Base, RDR_STAT_OFFSET(cur_ring), 0x10);

	for (i = 0; i < max_read; i++) {
		if (rdr_desc_offset > MAX_DESC_LIST_SIZE - 1)
			rdr_desc_offset = 0;

		done = ipsec_done_ptr + rdr_desc_offset;
		if( 0 == done->flag ) {
			pr_debug("\nERROR: EIP97 Descriptor is not set properly \n");
			continue;
		}
		done->flag = 0;	

#ifndef IOCU_MODE
		DMAaddr = dma_map_single(DMADevice_p,
							&rdr_fifo[rdr_desc_offset * RDR_DESC_SIZE],
                            RDR_DESC_SIZE * 4,
                            DMA_FROM_DEVICE);

		rdr_ptr = __va(DMAaddr);
#else
		rdr_ptr =  rdr_fifo_ptr + rdr_desc_offset * RDR_DESC_SIZE;
#endif

#ifdef IPSEC_DEBUG
		{
			int k;
	    	for (k = 0; k < 6; k++) {
        		DEBUG_INFO("** RDR[%d] = 0x%08x\n", k, rdr_ptr[k]);
			}
		}
#endif
		ret_pkt_len = rdr_ptr[0];
		done->ret_pkt_len = (ret_pkt_len & 0x1ffff);

		err = ((rdr_ptr[2] >> RDR_ERR_E0_BIT) & 0x7fff);
		err |= ((rdr_ptr[3] & E15_MASK) << 11);
		done->nexthdr = rdr_ptr[5] & 0xff;

		if (done->token_ptr)
			kfree(done->token_ptr);

		Device_Write32(Global_Base, RDR_PROC_COUNT_OFFSET(cur_ring), 0x01000020);	

		if (err) {
			parse_err(err);
		}

		if (err)
			err = -EBADMSG;

		rdr_desc_offset++;

		done->err = err;
		done->callback_fn(done);
		atomic_dec(&fifo_queue);
	}	

	return;
}

DECLARE_TASKLET(eip97_ipsec_tasklet, &get_packet, 0); 

static irqreturn_t ipsec_handler(int irq, void *data)
{
	DEBUG_INFO("In IRQ-Handler: irq %d\n", irq);
	if (irq != cur_irq)
		return IRQ_NONE;
	
	Device_Write32(Global_Base, HIA_AIC_CLEAR_REG, 1 << (16 + cur_ring)); 
	Device_Write32(Global_Base, HIA_AIC_RING_CLEAR(cur_ring), (0x3 << cur_ring * 2));
	tasklet_schedule(&eip97_ipsec_tasklet);
	return IRQ_HANDLED;
}

#endif /* SYNC_CRYPT */

static void delete_entry(u32 spi)
{
	struct list_head *ptr;
	struct ipsec_list *entry;

	if (list_empty(&ipsec_list)) {
		pr_err("%s: nothing to delete!\n", __func__);
		return;
	}

	spin_lock_bh(&list_lock);
	list_for_each(ptr, &ipsec_list) {
		entry = list_entry(ptr, struct ipsec_list, list);
		if (!entry)
			break;
		if (entry->spi == spi) {
			list_del(&entry->list);
			kfree(entry);
			spin_unlock_bh(&list_lock);
			return;
		}
	}
	spin_unlock_bh(&list_lock);

	pr_err("Cannot find SPI entry 0x%08x to delete\n", spi);
	return;
}

struct ltq_crypto_ipsec_params *ltq_ipsec_get_params(u32 spi) 
{
	struct list_head *ptr;
	struct ipsec_list *entry;

	DEBUG_INFO("debug ipsec entry of spi: 0x%08x\n", spi);
	debug_ipsec_list();

	if (list_empty(&ipsec_list)) {
		pr_err("%s: IPSec param list is empty!\n", __func__);
		return NULL;
	}

	spin_lock_bh(&list_lock);
	list_for_each(ptr, &ipsec_list) {
		entry = list_entry(ptr, struct ipsec_list, list);
		if (!entry)
			break;
		if (entry->spi == spi) {
			DEBUG_INFO("found entry: 0x%08x\n", entry->spi);
			spin_unlock_bh(&list_lock);
			return entry->params;
		}
	}
	spin_unlock_bh(&list_lock);

	return NULL;

}
EXPORT_SYMBOL(ltq_ipsec_get_params);

int ltq_get_length_params(u32 spi, unsigned int *ivsize, unsigned int *ICV_length,
 	           unsigned int *blksize)
{
	struct ltq_crypto_ipsec_params *params;

	params = ltq_ipsec_get_params(spi);
	if (!params) {
		pr_err("No entry found for spi=0x%08x, unable to get params length\n", spi);
		return -EINVAL;
	}

	*ivsize = params->ivsize;
	*ICV_length = params->ICV_length;
	*blksize = params->pad_blksize;

	return 0;
}
EXPORT_SYMBOL(ltq_get_length_params);

void debug_eip97_reg(unsigned int ring)
{
	int i;

	for (i = 0; i < 15; i++)
		Device_Read32(Global_Base, (0x1000 * ring) + i*4);

	for (i = 0; i < 15; i++)
		Device_Read32(Global_Base, (0x1800 * ring) + i*4);
}

void ltq_ipsec_enable_irq(unsigned int ring)
{
	u32 reg;

	reg = Device_Read32(Global_Base, HIA_AIC_GLOBAL_CTRL);
	Device_Write32(Global_Base, HIA_AIC_GLOBAL_CTRL, reg | (1 << (16 + ring))); 
	Device_Write32(Global_Base, HIA_AIC_RING_CTRL(ring), (0x3 << ring * 2));
}

static void eip97_send_cmd(unsigned int cd_size)
{
    /* Dwords of descriptor to send to HW */
	Device_Write32(Global_Base, RDR_PREP_COUNT_OFFSET(cur_ring), (RDR_DESC_SIZE << 2));
	Device_Write32(Global_Base, CDR_PREP_COUNT_OFFSET(cur_ring), (cd_size << 2));

	return;
}

/*
 * Paraemters for the eip97_crypto_api() function
 * *req        Pointer to structure which hold SPI and algorithm info for EIP97 engine
 * *tokenbuf   Token instruction word updated by eip97 api
 * *in         Input data buffer pointer
 * *out        Output buffer pointer for the encrypted/decrypted data
 * outBufLen   Maximum length of data that out buffer can hold.
 * *done       Call back function after encryption/decryption
 * *ip_data    skbuff pointer
 *  buflen     Length of input buffer data to be encrypted/decrypted
 *  padlen     Pad length value for ESP
 *  direction  Inbound/outbound
 */
static int eip97_crypto_api(struct ltq_crypto_ipsec_params *req, u32 *tokenbuf,
				u8 *in, u8 *out, uint16_t outBufLen, void (*callback)(struct ltq_ipsec_complete *done),
				void *ip_data, unsigned int buflen, 
				unsigned int padlen, int direction)
{
	unsigned int cd_size = CDR_DESC_SIZE;
	u32 token_header = req->token_headerword;
	u32 *ctx_buffer = req->SA_buffer;
	u32 appid = 0x00000af0;
	u32 *cdr_ptr;
	u32 *rdr_ptr;
	struct ltq_ipsec_complete *done_ptr;
#ifndef IOCU_MODE
	struct device *DMADevice_p = NULL;
	dma_addr_t DMAaddr;
#else
	u32 DMAaddr;
#endif

	if (cur_ring < 0 || cur_ring > 3) {
		pr_err("Ring not initialized!\n");
		return -EINVAL;
	}

#ifndef SYNC_CRYPT
	ltq_ipsec_enable_irq(cur_ring);
#endif

	spin_lock_bh(&ipsec_lock);
	if (desc_offset > MAX_DESC_LIST_SIZE - 1)
		desc_offset = 0;

	cdr_ptr = cdr_fifo_ptr + desc_offset * CDR_DESC_SIZE;
	rdr_ptr = rdr_fifo_ptr + desc_offset * RDR_DESC_SIZE;

	done_ptr = ipsec_done_ptr + desc_offset;
	if( done_ptr->flag ) {
		pr_debug("\nERROR: EIP97 Descriptor is not free\n");
		spin_unlock_bh(&ipsec_lock);
		return -EINVAL;
	}
	done_ptr->flag = 1;
	atomic_inc(&fifo_queue);

	/* ACD is present as a pointer */
	cdr_ptr[0] = CDR_DW0_TEMPL | (req->tokenwords << 24) | buflen;


#ifndef IOCU_MODE
	DMAaddr = dma_map_single(DMADevice_p,
							in,
                            buflen,
                            DMA_BIDIRECTIONAL);
#else
	DMAaddr = VIRT_TO_IOCU((u32) in); 
#endif

	cdr_ptr[1] = DMAaddr;

#ifndef IOCU_MODE
	DMAaddr = dma_map_single(DMADevice_p,
							tokenbuf,
                            req->tokenwords * 4,
                            DMA_BIDIRECTIONAL);
#else
	DMAaddr = VIRT_TO_IOCU((u32) tokenbuf); 
#endif

	cdr_ptr[2] = DMAaddr;

	cdr_ptr[3] = ((token_header & 0xfffe000) | buflen);
	cdr_ptr[4] = appid;

#ifndef IOCU_MODE
	DMAaddr = dma_map_single(DMADevice_p,
							ctx_buffer,
                            req->SAWords * 4,
                            DMA_BIDIRECTIONAL);
#else
	DMAaddr = VIRT_TO_IOCU((u32) ctx_buffer); 
#endif

	cdr_ptr[5] = DMAaddr;

	/* RDR stuffs */
	rdr_ptr[0] = RDR_DW0_TEMPL | outBufLen;

#ifndef IOCU_MODE
	DMAaddr = dma_map_single(DMADevice_p,
							out,
                            buflen,
                            DMA_BIDIRECTIONAL);
#else
	DMAaddr = VIRT_TO_IOCU((u32) out); 
#endif

	rdr_ptr[1] = DMAaddr;

	done_ptr->data = ip_data;

#ifdef IPSEC_DEBUG
	{
		int i;
		u32 *tmp = ctx_buffer;

		for (i = 0; i < req->SAWords; i++) {
			DEBUG_INFO("@@ SA-DATA[%d] = 0x%08x\n", i, tmp[i]);
		}

		tmp = tokenbuf;

		for (i = 0; i < req->tokenwords; i++) {
			DEBUG_INFO("## Token[%d] = 0x%08x\n", i, tmp[i]);
		}

		for (i = 0; i < cd_size; i++) {
			DEBUG_INFO("** CDR[%d] = 0x%08x\n", i, cdr_ptr[i]);
		}

		for (i = 0; i < cd_size; i++) {
			DEBUG_INFO("** RDR[%d] = 0x%08x\n", i, rdr_ptr[i]);
		}
	}
#endif /* IPSEC_DEBUG */

#ifndef IOCU_MODE
	DMAaddr = dma_map_single(DMADevice_p,
							&cdr_fifo[desc_offset * CDR_DESC_SIZE],
                            cd_size * 4,
                            DMA_TO_DEVICE);

	DMAaddr = dma_map_single(DMADevice_p,
							&rdr_fifo[desc_offset * RDR_DESC_SIZE],
                            cd_size * 4,
                            DMA_TO_DEVICE);
#endif

	done_ptr->callback_fn = callback;
	done_ptr->token_ptr = tokenbuf;

	eip97_send_cmd(cd_size);

#ifndef SYNC_CRYPT
	desc_offset++;
#endif

	spin_unlock_bh(&ipsec_lock);

	/* trigger eip97 */
	Device_Write32(Global_Base, RDR_THRESH_OFFSET(cur_ring), RDR_DESC_SIZE);

#ifdef SYNC_CRYPT
	get_packet(0);
#endif

	return 0;
}

int ltq_ipsec_enc(u32 spi, u16 ip_prot, u8 *in, u8 *out, uint16_t outBufLen, void (*callback)(struct ltq_ipsec_complete *done),
			unsigned int buflen, void *ip_data) 
{
	struct ltq_crypto_ipsec_params *req;
	unsigned int tokensize, padlen;
	int ret = 0;
	u32 *token;
	u32 psize = 0, tplen, hplen;

	if (atomic_read(&fifo_queue) > max_queue_size) {
        	atomic_inc(&pkt_err);
		return -EAGAIN;
	}

	req = ltq_ipsec_get_params(spi);
	if (!req) {
		pr_err("No entry found for spi=0x%08x, unable to submit to engine for encryption\n", spi);
        	atomic_inc(&pkt_err);
		return -EINVAL;
	}

	tokensize = req->tokenwords * 4;
	token = (u32 *) kmalloc(tokensize, GFP_KERNEL);
	if (!token) {
        	atomic_inc(&pkt_err);
		return -ENOMEM;
	}

	/* copy to a local token buffer */
	memcpy((u32 *)token, (u32 *)req->token_buffer, tokensize);

	if (req->crypto_offs < req->tokenwords) {
		set_val(token[req->crypto_offs], buflen, DIR_LEN_MASK, DIR_LEN_OFFSET);
	}
 
	if (req->total_pad_offs && req->total_pad_offs < req->tokenwords) {
		padlen = (req->pad_blksize - (buflen & (req->pad_blksize - 1))); 

		if (padlen < 2)
			padlen += req->pad_blksize;

		set_val(token[req->total_pad_offs], padlen, INSERT_LEN_MASK, INSERT_LEN_OFFSET);
		set_val(token[req->total_pad_offs], ip_prot, INSERT_XLIV_MASK, INSERT_XLIV_OFFSET);
	}

	if (req->hash_pad_offs) {
		if (req->hash_pad_offs > req->tokenwords)
		{
			pr_debug("\n%s,%d\n",__FUNCTION__,__LINE__);
			return -EINVAL;
		}
        /*
            payload_size = pkt_byte_cnt - ICV_len - IV_len - 8;
            tmp_pad_len = (payload_size + block_cnt_aes - 1) & ~(block_cnt_aes - 1);
            hash_pad_len = tmp_pad_len - payload_size;
        */
        psize = (buflen + 2 + BLK_CNT_ESP - 1) & MASK_BLK_CNT_ESP;
        tplen = (psize + BLK_CNT_AES - 1) & MASK_BLK_CNT_AES;
        hplen = tplen - psize;

		set_val(token[req->hash_pad_offs], hplen, INSERT_LEN_MASK, INSERT_LEN_OFFSET);
		DEBUG_INFO("%s: psize: 0x%08x, tplen: 0x%08x, hplen: 0x%08x\n", 
									__func__, psize, tplen, hplen);
	}

	if (req->msg_len_offs) {
		if (req->msg_len_offs > req->tokenwords){
			return -EINVAL;
		}
		token[req->msg_len_offs] = endian_swap(psize); 
		DEBUG_INFO("%s: swapped psize: 0x%08x\n", __func__, token[req->msg_len_offs]);
	}

	ret = eip97_crypto_api(req, token, in, out, outBufLen, callback,
						ip_data, buflen, padlen,
						SAB_DIRECTION_OUTBOUND);
	if (ret) {
			pr_err("Unable to encrypt packet, err=%d\n", ret);
		return -EINVAL;
	}

	return -EINPROGRESS;
}
EXPORT_SYMBOL(ltq_ipsec_enc);

int ltq_ipsec_dec(u32 spi, u8 *in, u8 *out, void (*callback)(struct ltq_ipsec_complete *done),
			unsigned int buflen, void *ip_data) 
{
	struct ltq_crypto_ipsec_params *req;
	unsigned int tokensize, adjust_len;
	int ret = 0;
	u32 *token;
	u32 psize = 0, tplen; // AES-CCM variables
	u32 hplen = 0; // AES-CCM variables
	uint16_t outbuflen = 0;

	if (atomic_read(&fifo_queue) > max_queue_size) {
        	atomic_inc(&pkt_err);
		return -EAGAIN;
	}

	req = ltq_ipsec_get_params(spi);
	if (!req) {
		pr_err("No entry found for spi=0x%08x, unable to submit to engine for decryption\n", spi);
        	atomic_inc(&pkt_err);
		pr_debug("\n%s,%d\n",__FUNCTION__,__LINE__);
		return -EINVAL;
	}
	
	tokensize = req->tokenwords * 4;
	token = (u32 *) kmalloc(tokensize, GFP_KERNEL);
	if (!token) {
        	atomic_inc(&pkt_err);
		return -ENOMEM;
	}

	/* copy to a local token buffer */
	memcpy((u32 *)token, (u32 *)req->token_buffer, tokensize);

	if (req->crypto_offs < req->tokenwords &&
			req->crypto_offs > 0) {
		adjust_len = req->ICV_length + ESP_HDR_LEN + req->ivsize;
		set_val(token[req->crypto_offs], buflen - adjust_len,
					DIR_LEN_MASK, DIR_LEN_OFFSET);
	}

	if (req->hash_pad_offs > 0 || req->msg_len_offs > 0) {
        /*
            payload_size = pkt_byte_cnt - ICV_len - IV_len - 8;
            tmp_pad_len = (payload_size + block_cnt_aes - 1) & ~(block_cnt_aes - 1);
            hash_pad_len = tmp_pad_len - payload_size;
        */
        psize = buflen - req->ICV_length - req->ivsize - 8;
        tplen = (psize + BLK_CNT_AES - 1) & MASK_BLK_CNT_AES;
        hplen = tplen - psize;
	}

	if (req->hash_pad_offs) {
		if (req->hash_pad_offs > req->tokenwords){
			return -EINVAL;
		}

        set_val(token[req->hash_pad_offs], hplen, INSERT_LEN_MASK, INSERT_LEN_OFFSET);
		DEBUG_INFO("%s: psize: 0x%08x, tplen: 0x%08x, hplen: 0x%08x\n", 
								__func__, psize, tplen, hplen);
	}

	if (req->msg_len_offs) {
		if (req->msg_len_offs > req->tokenwords){
			return -EINVAL;
		}

		token[req->msg_len_offs] = endian_swap(psize); 
		DEBUG_INFO("%s: swapped psize: 0x%08x\n", __func__, token[req->msg_len_offs]);
	}

	outbuflen = buflen;

	ret = eip97_crypto_api(req, token, in, out, outbuflen, callback,
						ip_data, buflen, adjust_len,
						SAB_DIRECTION_INBOUND);
	if (ret) {
		pr_err("Unable to decrypt packet, err=%d\n", ret);
		return -EINVAL;
	}
	return -EINPROGRESS;
}
EXPORT_SYMBOL(ltq_ipsec_dec);

static int ltq_ipsec_show(struct seq_file *file, void *data)
{
    seq_printf(file, "[ . Packets to be processed: %d . ]\n ",
            atomic_read(&fifo_queue));
    seq_printf(file, "[ . Packets dropped %d . ]\n",
            atomic_read(&pkt_err));
    seq_printf(file, "[ . Max pkt handled in tasklet %d . ]\n",
            cur_max_pkt);
    return 0;
}

static int ltq_ipsec_proc_open(struct inode *inode,
                                struct file *file)
{
     return single_open(file, ltq_ipsec_show, NULL);
}

static const struct file_operations ltq_ipsec_proc_ops = {
    .owner = THIS_MODULE,
    .open = ltq_ipsec_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

void ltq_destroy_ipsec_sa(struct ltq_crypto_ipsec_params *req)
{
	DMABuf_Handle_t SAHandle = req->SAHandle;
	DMABuf_Handle_t TokenHandle = req->TokenHandle;
	DMABuf_Handle_t SrcHandle = req->SrcHandle;
	DMABuf_Handle_t DstHandle = req->DstHandle;

	DMABuf_Release(SAHandle);
	DMABuf_Release(TokenHandle);
	DMABuf_Release(SrcHandle);
	DMABuf_Release(DstHandle);

	if (req->context_data) {
		kfree(req->context_data);
		req->context_data = NULL;
	}

	delete_entry(req->spi);

}
EXPORT_SYMBOL(ltq_destroy_ipsec_sa);

struct xfrm_mgr eip97_xfrm_mgr = {
	.id		= "EIP97",
	.notify		= EIP97_notify,
	.acquire	= EIP97_acquire,
	.compile_policy	= EIP97_compile,
	.notify_policy	= EIP97_policy,
	.report		= EIP97_report,
	.migrate	= EIP97_migrate,
	.new_mapping	= EIP97_mapping,
};
/* XFRM Event handler for EIP97 Token creation and CDR configuration for MPE */

int EIP97_notify(struct xfrm_state *x, const struct km_event *c)
{
	struct ltq_crypto_ipsec_params *params = NULL;
	switch (c->event) {

	case XFRM_MSG_DELSA:
	case XFRM_MSG_FLUSHSA:
#if defined(CONFIG_PPA_MPE_IP97)
		if ( ppa_hook_session_ipsec_del_fn != NULL ){
			pr_debug("\n%s Del SA,%d\n",__FUNCTION__,__LINE__);
			ppa_hook_session_ipsec_del_fn(x);
		}
#endif
		params = ltq_ipsec_get_params(x->id.spi);
		if (!params) {
			pr_err("No entry found for spi=0x%08x, unable to delete SA\n", x->id.spi);
			return -EINVAL;
		}
		ltq_destroy_ipsec_sa(params);
		break;

	case XFRM_MSG_UPDSA:
		if( ltq_ipsec_construct_token(x,LTQ_SAB_DIRECTION_INBOUND) != 0) {
			pr_err("Token creation failed for INBOUND \n");
			return -EINVAL;
		}
		break;

	case XFRM_MSG_NEWSA:
		if (ltq_ipsec_construct_token(x,LTQ_SAB_DIRECTION_OUTBOUND) != 0) {
			pr_err("Token creation failed for OUTBOUND \n");
			return -EINVAL;
		}
		break;

	default:
		pr_debug(KERN_NOTICE "xfrm_user: Unknown SA event %d\n",
		       c->event);
		break;
	}

	return 0;
}

int ltq_ipsec_construct_token(struct xfrm_state *xfrm_sa,int dir)
{
	struct ltq_crypto_ipsec_params ipsec_eip97_params;
	char emode[64] = "", ealgo[64]= "";

	memset(&ipsec_eip97_params, 0, sizeof(struct ltq_crypto_ipsec_params));

	ipsec_eip97_params.ipsec_mode = LTQ_SAB_IPSEC_ESP; //xfrm_sa->id.proto;
	ipsec_eip97_params.ipsec_tunnel = LTQ_SAB_IPSEC_TUNNEL;
	ipsec_eip97_params.direction = dir;
	ipsec_eip97_params.spi = xfrm_sa->id.spi;
	ipsec_eip97_params.ip_type = LTQ_SAB_IPSEC_IPV4;
	ipsec_eip97_params.ring_no = 1;

	if(xfrm_sa->aead->alg_name != NULL){
		pr_debug("<%s> Aead Algo %s\n",__func__,xfrm_sa->aead->alg_name);
		get_crypto_mode_algo(xfrm_sa->aead->alg_name, emode, ealgo);
		ipsec_eip97_params.crypto_mode = get_crypto_mode(emode);
		/* crypto 256ctr algorithm not supported */
		if (ipsec_eip97_params.crypto_mode == LTQ_SAB_CRYPTO_MODE_CTR){
			pr_debug("crypto algo %s not supported\n", xfrm_sa->aead->alg_name);
			return -EINVAL;
		}
		ipsec_eip97_params.crypto_algo = get_crypto_algo(ealgo);
		ipsec_eip97_params.crypto_keysize = (xfrm_sa->aead->alg_key_len + 7)/8;
		ipsec_eip97_params.crypto_key = &xfrm_sa->aead->alg_key[0];
		ipsec_eip97_params.auth_algo = SAB_AUTH_AES_GCM;
		ipsec_eip97_params.authsize = (xfrm_sa->aead->alg_key_len + 7)/8;
		ipsec_eip97_params.auth_keysize = (xfrm_sa->aead->alg_key_len + 7)/8;
		ipsec_eip97_params.auth_key = &xfrm_sa->aead->alg_key[0];
	}
	if(xfrm_sa->ealg->alg_name != NULL){
		get_crypto_mode_algo(xfrm_sa->ealg->alg_name, emode, ealgo);
		ipsec_eip97_params.crypto_mode = get_crypto_mode(emode);
		ipsec_eip97_params.crypto_algo = get_crypto_algo(ealgo);
		ipsec_eip97_params.crypto_keysize = (xfrm_sa->ealg->alg_key_len + 7)/8;
		ipsec_eip97_params.crypto_key = &xfrm_sa->ealg->alg_key[0];
	}

	if(xfrm_sa->aalg->alg_name != NULL){
		ipsec_eip97_params.auth_algo =get_auth_algo(xfrm_sa->aalg->alg_name);
		ipsec_eip97_params.authsize =(xfrm_sa->aalg->alg_key_len + 7)/8;
		ipsec_eip97_params.auth_keysize = (xfrm_sa->aalg->alg_key_len + 7)/8;
		ipsec_eip97_params.auth_key = &xfrm_sa->aalg->alg_key[0];
	}
	if(dir == LTQ_SAB_DIRECTION_INBOUND ){
                /* Packet Payload on on inbound packets should be multiple of the pad block size,
                   More info refer JIRA-29926 */
                if((xfrm_sa->aalg->alg_name != NULL) &&
                   (! strncmp (xfrm_sa->aalg->alg_name, "hmac(sha256)", 12)))
                        ipsec_eip97_params.cryptosize = 72;
                else if((xfrm_sa->aalg->alg_name != NULL) &&
                   (! strncmp (xfrm_sa->aalg->alg_name, "xcbc(aes)", 9)))
                        ipsec_eip97_params.cryptosize = 84;
                else
                        ipsec_eip97_params.cryptosize = 80;
			ipsec_eip97_params.data_len = 80;
	} else if(dir == LTQ_SAB_DIRECTION_OUTBOUND ) {
		ipsec_eip97_params.cryptosize = 16;
		ipsec_eip97_params.data_len = 16;
	}

	if(ltq_ipsec_setkey(&ipsec_eip97_params) != 0 ) {
		pr_debug("<%s> SetKey failed!!!\n",__FUNCTION__);
		return -EINVAL;
	}

	if(ltq_get_ipsec_token(&ipsec_eip97_params) != 0) {
		pr_debug("<%s> Get Ipsec Token failed !!!\n",__FUNCTION__);
		return -EINVAL;
	}

#if defined(CONFIG_PPA_MPE_IP97)
	if ( ppa_hook_session_ipsec_add_fn != NULL ) {
		pr_debug("\n%s added to ppa ,%d\n",__FUNCTION__,__LINE__);
		ppa_hook_session_ipsec_add_fn(xfrm_sa, dir);
        }
#endif
	return 0;
}

int ltq_ipsec_init(unsigned int ring, int irq)
{
	int err;
	unsigned int desc_size;
	u32 reg = 0;
	DEBUG_INFO("Registering %s with ring %d, irq %d\n", __func__, ring, irq);

	ipsec_done_ptr = (struct ltq_ipsec_complete*) kmalloc((sizeof(struct ltq_ipsec_complete) * MAX_DESC_LIST_SIZE), GFP_KERNEL);
	cdr_fifo_ptr = (u32*) kmalloc((sizeof(u32) * MAX_DESC_LIST_SIZE * CDR_DESC_SIZE), GFP_KERNEL);
	rdr_fifo_ptr = (u32*) kmalloc((sizeof(u32) * MAX_DESC_LIST_SIZE * RDR_DESC_SIZE), GFP_KERNEL);

	memset(cdr_fifo_ptr, 0, sizeof(u32) * MAX_DESC_LIST_SIZE * CDR_DESC_SIZE);
        memset(rdr_fifo_ptr, 0, sizeof(u32) * MAX_DESC_LIST_SIZE * RDR_DESC_SIZE);
	memset(ipsec_done_ptr, 0, sizeof(struct ltq_ipsec_complete) * MAX_DESC_LIST_SIZE);

	u32 *cdr_ptr = cdr_fifo_ptr;
	u32 *rdr_ptr = rdr_fifo_ptr;

	Global_Base = Device_Find("EIP97_GLOBAL");
	if (Global_Base == NULL) {
		pr_err("Unable to find global base device info\n");
		return -EINVAL;
	}

#ifndef SYNC_CRYPT
	err = request_irq(irq, ipsec_handler, 0,
						"EIP97_IPSec", NULL);
	if (err)
		return -EINVAL;
	
	cur_irq = irq;
#endif

	desc_size = CDR_DESC_SIZE;

	spin_lock_init(&ipsec_lock);
	spin_lock_init(&list_lock);

	/* CDR init */
	Device_Write32(Global_Base, CDR_CFG_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, CDR_PREP_COUNT_OFFSET(ring), 0x80000000);
	Device_Write32(Global_Base, CDR_PROC_COUNT_OFFSET(ring), 0x80000000);
	Device_Write32(Global_Base, CDR_PREP_PNTR_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, CDR_PROC_PNTR_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, CDR_RING_SIZE_OFFSET(ring), ((desc_size * MAX_DESC_LIST_SIZE) << 2));
#ifndef IOCU_MODE
	Device_Write32(Global_Base, CDR_RING_BASE_ADDR_LO_OFFSET(ring), __pa((u32) cdr_ptr));
#else
	Device_Write32(Global_Base, CDR_RING_BASE_ADDR_LO_OFFSET(ring), VIRT_TO_IOCU((u32) cdr_ptr));
#endif
	Device_Write32(Global_Base, CDR_DMA_CFG_OFFSET(ring), 0x01000100);
#ifdef SYNC_CRYPT
	Device_Write32(Global_Base, CDR_DESC_SIZE_OFFSET(ring), 0x40060006);
	Device_Write32(Global_Base, CDR_CFG_OFFSET(ring), 0x00060006);
#else
	Device_Write32(Global_Base, CDR_DESC_SIZE_OFFSET(ring), 0x40080008);
	Device_Write32(Global_Base, CDR_CFG_OFFSET(ring), 0x00080008);
#endif
	Device_Write32(Global_Base, CDR_THRESH_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, CDR_STAT_OFFSET(ring), 0x0000003f);

	reg = Device_Read32(Global_Base, HIA_DFE_PRIO_0_OFFSET);
	Device_Write32(Global_Base, HIA_DFE_PRIO_0_OFFSET, (reg | (0xf << (ring * 8))));
	reg = Device_Read32(Global_Base, HIA_DFE_THR_CTRL_OFFSET);
	Device_Write32(Global_Base, HIA_DFE_THR_CTRL_OFFSET, (reg | (1 << 30 | 1 << ring)));

	/* RDR Init */
	Device_Write32(Global_Base, RDR_DMA_CFG_OFFSET(ring), 0x01000100);
#ifndef IOCU_MODE
	Device_Write32(Global_Base, RDR_RING_BASE_ADDR_LO_OFFSET(ring), __pa((u32) rdr_ptr));
#else
	Device_Write32(Global_Base, RDR_RING_BASE_ADDR_LO_OFFSET(ring), VIRT_TO_IOCU((u32) rdr_ptr));
#endif
	Device_Write32(Global_Base, RDR_RING_SIZE_OFFSET(ring), ((desc_size * MAX_DESC_LIST_SIZE) << 2));	
#ifdef SYNC_CRYPT
	Device_Write32(Global_Base, RDR_DESC_SIZE_OFFSET(ring), 0x00060006);
	Device_Write32(Global_Base, RDR_CFG_OFFSET(ring), 0x00060012);
#else
	Device_Write32(Global_Base, RDR_DESC_SIZE_OFFSET(ring), 0x00080008);
	Device_Write32(Global_Base, RDR_CFG_OFFSET(ring), 0x00080018);
#endif
	Device_Write32(Global_Base, RDR_PREP_COUNT_OFFSET(ring), 0x80000000);
	Device_Write32(Global_Base, RDR_PROC_COUNT_OFFSET(ring), 0x80000000);
	Device_Write32(Global_Base, RDR_PREP_PNTR_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, RDR_PROC_PNTR_OFFSET(ring), 0x00000000);
	Device_Write32(Global_Base, RDR_THRESH_OFFSET(ring), 0x00000000);	
	Device_Write32(Global_Base, RDR_STAT_OFFSET(ring), 0x000000ff);

	reg = Device_Read32(Global_Base, HIA_DSE_PRIO_0_OFFSET);
	Device_Write32(Global_Base, HIA_DSE_PRIO_0_OFFSET, (reg | (0xf << (ring * 8))));
	reg = Device_Read32(Global_Base, HIA_DSE_THR_CTRL_OFFSET);
	Device_Write32(Global_Base, HIA_DSE_THR_CTRL_OFFSET, (reg | (1 << 30 | 1 << ring)));
	/* Init EIP97 IRQ */
	reg = Device_Read32(Global_Base, HIA_AIC_GLOBAL_CTRL);
	Device_Write32(Global_Base, HIA_AIC_GLOBAL_CTRL, reg | (1 << (16 + ring))); 
	Device_Write32(Global_Base, HIA_AIC_RING_CTRL(ring), (0x3 << ring * 2));

	cur_ring = ring;
	desc_offset = 0;
 	rdr_desc_offset = 0;
	max_queue_size = MAX_QUEUE_LEN;
 	
	atomic_set(&fifo_queue, 0);
    atomic_set(&pkt_err, 0);

    ipsec_root_dir = proc_mkdir(LTQ_PROC_DIR, NULL);
    if (!ipsec_root_dir) {
        pr_err("Error creating proc entry\n");
        return -EINVAL;
    }

    proc_create("debug",
                S_IRUGO| S_IWUSR,
                ipsec_root_dir,
                &ltq_ipsec_proc_ops);	

#ifdef IOCU_MODE
	ltq_w32(0x5, GCR_CCA_IC_MREQ(24));
	ltq_w32(0x5, GCR_CCA_IC_MREQ(25));
#endif

	xfrm_register_km(&eip97_xfrm_mgr);
	DEBUG_INFO("Done initializing IPSec path\n");
	ltq_ipsec_dec_hook = ltq_ipsec_dec;
	ltq_ipsec_enc_hook = ltq_ipsec_enc;
	ltq_get_len_param_hook = ltq_get_length_params;
	ltq_ipsec_get_param_hook = ltq_ipsec_get_params;
	ltq_destroy_ipsec_sa_hook = ltq_destroy_ipsec_sa;

	return 0;
}

void ltq_ipsec_exit(unsigned int ring) 
{
	xfrm_unregister_km(&eip97_xfrm_mgr);
#ifndef SYNC_CRYPT
	free_irq(cur_irq, NULL);
#endif
	ltq_ipsec_dec_hook = NULL;
	ltq_ipsec_enc_hook = NULL;
	ltq_get_len_param_hook = NULL;
	ltq_ipsec_get_param_hook = NULL;
	ltq_destroy_ipsec_sa_hook = NULL;

	return;
}














