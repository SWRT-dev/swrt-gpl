/******************************************************************************

						 Copyright (c) 2012, 2014, 2015
							Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/******************************************************************************
**
** FILE NAME	: ltq_crypto_core.c
** PROJECT		: GRX500
** MODULES		: crypto hardware
**
** DATE			: 02 September 2014
** AUTHOR		: Mohammad Firdaus B Alias Thani
** DESCRIPTION	: Hardware accelerated Crypto drivers Linux glue layer
**
*******************************************************************************/

#include <linux/crypto.h>
#include <linux/kernel.h>
#include <linux/rtnetlink.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/atomic.h>
#include <linux/gfp.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/device.h>
#include <linux/clk.h>

#include <crypto/ctr.h>
#include <crypto/des.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <crypto/md5.h>
#include <crypto/hash.h>
#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/scatterwalk.h>
#include <crypto/internal/aead.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>

#include "ltq_crypto_core.h"

#include "dmares_types.h"
#include "api_driver97_init.h"
#include "api_dmabuf.h"
#include "api_pec.h"
#include "api_pec_sg.h"
#include "device_mgmt.h"
#include "sa_builder.h"
#include "sa_builder_basic.h"
#include "token_builder.h"

#define MD5_BLOCK_SIZE			64
#define LTQ_CRYPTO_PRIORITY		300

#define ARC4_MIN_KEY_SIZE		5
#define ARC4_MAX_KEY_SIZE		256
#define ARC4_BLOCK_SIZE			1
#define MAX_FETCH_SIZE			100
#define MAX_AEAD_TKN_SIZE		26
#define MAX_HASH_TKN_SIZE		23
#define MAX_CTX_SIZE			36
#define MAX_IVLEN				16
#define MAX_OPAD_SIZE			64
#define MAX_IPAD_SIZE			64
#define MAX_DIGEST_SIZE			64
#define MAX_KEY_LEN				64
#define MAX_IV_LEN				32
#define DA_PEC_BANK_SA			0
#define DA_PEC_BANK_TOKEN		0
#define DA_PEC_BANK_PACKET		0
#define LTQ_MAX_KEYLEN			256
#define MAX_SCATTERGATHER_PKT	ADAPTER_PEC_MAX_FRAGMENTS_PER_PACKET
#define LTQ_ENC					0
#define LTQ_DEC					1
#define LTQ_ALIGN				4
#define LTQ_TEST_BIT			0x12
#define V_MAJOR					1
#define V_MINOR					4
#define REVISION				13

#ifdef ADAPTER_PEC_ENABLE_SCATTERGATHER
#define PACKET_SCATTERGATHER
#endif

/* If.f IPSec support is enabled in MPE, we will use only one ring */
#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT
#define EIP97_RING_ENC			0
#define EIP97_RING_DEC			0
#define EIP97_ENC_CALLBACK		eip97_ring_irq0
#define EIP97_DEC_CALLBACK		eip97_ring_irq0
#define LTQ_RINGS_USED_DRIVER	1
#define FASTPATH_RING			1
#else
#define EIP97_RING_ENC			0
#define EIP97_RING_DEC			1
#define EIP97_ENC_CALLBACK		eip97_ring_irq0
#define EIP97_DEC_CALLBACK		eip97_ring_irq1
#define LTQ_RINGS_USED_DRIVER	2
#endif /* CONFIG_LTQ_MPE_IPSEC_SUPPORT */

void __iomem *ltq_crypto_membase;
unsigned int PEC_ResultNotificationCount;
atomic_t packets_queue;
PEC_ResultDescriptor_t Result_R0[MAX_FETCH_SIZE];
PEC_ResultDescriptor_t Result_R1[MAX_FETCH_SIZE];

struct ltq_aes_result {
	int err;
	struct completion completion;
};

struct ltq_crypto_template {
	u32 type;
	union {
		struct ahash_alg hash;
		struct crypto_alg crypto;
		struct aead_alg aead;
	} alg;
	const u8 *zero_msg;
	u16 auth_flag;
	u16 crypto_flag;
	u16 crypto_mode;
};

struct ltq_crypt_ctrl {
	int SA_flag;
	int no_of_particles;
	int dir;

	DMABuf_Handle_t TokenHandle;		/* Token Handle */
	DMABuf_Handle_t SrcPktHandle;
	DMABuf_Handle_t DstPktHandle;

	DMABuf_Handle_t SrcScatter_Handle[MAX_SCATTERGATHER_PKT];
	DMABuf_Handle_t DstGather_Handle[MAX_SCATTERGATHER_PKT];

	u8 *copybuf;
	union {
		struct ablkcipher_request *ablk_req;
		struct aead_request *aead_req;
		struct ahash_request *req;
	} data;

	struct scatterlist *sg_cpy;
	int unaligned;
	unsigned int copy_bytes;

	TokenBuilder_Params_t TokenParams;
};

struct ltq_crypto_ctx {
	u8 enckeylen;
	u8 nonce[CTR_RFC3686_NONCE_SIZE];
	u8 iv[MAX_IV_LEN];
	u8 key[MAX_KEY_LEN];

	unsigned int registered;
	unsigned int SAWords;
	unsigned int config_sa_done;
	unsigned int SA_flag;

	u32 *TCRDataOut;

	SABuilder_Params_t params;			/* SA returned params */
	DMABuf_Handle_t SAHandle;			/* SA Handle */
};

struct ltq_ahash_req_ctx {
	struct ahash_request req;
};
struct ltq_ahash_ctx {
	unsigned int base_hash_ds;
	unsigned int ds;
	unsigned int SAWords;
	unsigned int config_sa_done;
	unsigned int registered;

	struct crypto_shash *shash;
	struct crypto_ahash *fallback;

	SABuilder_Params_t params;			/* SA returned params */
	DMABuf_Handle_t SAHandle;			/* SA Handle */

	u32 TokenFlags;
	u32 *TCRDataOut;

	u32 ipad[MAX_IPAD_SIZE];
	u32 opad[MAX_OPAD_SIZE];
	u32 auth_key3[AES_BLOCK_SIZE];
};

struct ltq_aead_ctx {
	struct crypto_shash *shash;
	unsigned int direction;
	unsigned int base_hash_ds;
	unsigned int blksize;
	unsigned int TokenFlags;
	unsigned int authsize;

	unsigned int SAWords;
	unsigned int SA_flag;
	unsigned int config_sa_done;
	unsigned int registered;

	SABuilder_Params_t params;			/* SA returned params */
	DMABuf_Handle_t SAHandle;			/* SA Handle */

	u8 authkeylen;
	u8 enckeylen;

	u8 enckey[MAX_KEY_LEN];
	u8 authkey[MAX_KEY_LEN];
	u8 nonce[16];
	u8 iv[16];

	u32 *TCRDataOut;
	u32 ipad[MAX_IPAD_SIZE];
	u32 opad[MAX_OPAD_SIZE];

	struct crypto_aead	*aead_fb;
};

static int count_sg(struct scatterlist *sg)
{
	int sg_nents = 1;

	while (!sg_is_last(sg)) {
		sg_nents++;
		sg = sg_next(sg);
		if (!sg)
			break;
	}
	return sg_nents;
}

#ifdef LTQ_CRYPTO_DEBUG
static void dump_sg(struct scatterlist *sg)
{
	int i;
	int count = count_sg(sg);
	struct scatterlist *tmp = sg;

	for (i = 0; i < count; i++) {
		if (!sg)
			break;
		pr_info("sg # %d ", i);
		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
						16, 1,
						sg_virt(tmp), tmp->length, false);

		tmp = sg_next(tmp);
	}
}
#endif /* LTQ_CRYPTO_DEBUG */

u32 endian_swap(u32 input)
{
	u8 *ptr = (u8 *)&input;
	return (ptr[3] << 24) | (ptr[2] << 16) | (ptr[1] << 8) | ptr[0];
}

/* Get the mode of the crypto transform */
static inline u32 cryto_mode_get(struct crypto_tfm *tfm, int tfm_type)
{
	if (tfm_type == CRYPTO_ALG_TYPE_AHASH)
		return container_of(__crypto_ahash_alg(tfm->__crt_alg),
				struct ltq_crypto_template, alg.hash)->crypto_mode;
	else if (tfm_type == CRYPTO_ALG_TYPE_ABLKCIPHER)
		return container_of(tfm->__crt_alg,
				struct ltq_crypto_template, alg.crypto)->crypto_mode;
	else
		return container_of(crypto_aead_alg(__crypto_aead_cast(tfm)),
				struct ltq_crypto_template, alg.aead)->crypto_mode;
}

/* Get the crypto type */
static inline u32 crypto_flag_get(struct crypto_tfm *tfm, int tfm_type)
{
	if (tfm_type == CRYPTO_ALG_TYPE_AHASH)
		return container_of(__crypto_ahash_alg(tfm->__crt_alg),
				struct ltq_crypto_template, alg.hash)->crypto_flag;
	else if (tfm_type == CRYPTO_ALG_TYPE_ABLKCIPHER)
		return container_of(tfm->__crt_alg,
				struct ltq_crypto_template, alg.crypto)->crypto_flag;
	else
		return container_of(crypto_aead_alg(__crypto_aead_cast(tfm)),
				struct ltq_crypto_template, alg.aead)->crypto_flag;
}

/* Get the hash type */
static inline u32 hash_mode_get(struct crypto_tfm *tfm, int tfm_type)
{
	if (tfm_type == CRYPTO_ALG_TYPE_AHASH)
		return container_of(__crypto_ahash_alg(tfm->__crt_alg),
				struct ltq_crypto_template, alg.hash)->auth_flag;
	else if (tfm_type == CRYPTO_ALG_TYPE_ABLKCIPHER)
		return container_of(tfm->__crt_alg,
				struct ltq_crypto_template, alg.crypto)->auth_flag;
	else
		return container_of(crypto_aead_alg(__crypto_aead_cast(tfm)),
				struct ltq_crypto_template, alg.aead)->auth_flag;
}

/* Since we don't support hw 0 byte hash, we fix them to a pattern */
static inline const u8 *zero_msg_get(struct crypto_tfm *tfm)
{
	return container_of(__crypto_ahash_alg(tfm->__crt_alg),
				struct ltq_crypto_template, alg.hash)->zero_msg;
}

static inline void cpu_to_le32_array(u32 *buf, unsigned int words)
{
	while (words--) {
		__cpu_to_le32s(buf);
		buf++;
	}
}

static int check_unaligned_buffer(struct scatterlist *sg)
{
	u8 *buf;

	while (sg) {
		buf = sg_virt(sg);
		if (!(IS_ALIGNED((u32)buf, sizeof(u32))))
			return true;
		sg = sg_next(sg);
	}

	return false;
}

static void ltq_free_sg_copy(struct scatterlist **sg, unsigned int len)
{
	if (!*sg)
		return;

	free_pages((unsigned long)sg_virt(*sg), get_order(len));
	kfree(*sg);
}

static void ltq_copybuf(void *buf, struct scatterlist *sg,
				unsigned int nbytes, int out)
{
	struct scatter_walk walk;

	if (!nbytes)
		return;

	scatterwalk_start(&walk, sg);
	scatterwalk_copychunks(buf, &walk, nbytes, out);
	scatterwalk_done(&walk, out, 0);
}

static int ltq_unaligned_cpy(struct scatterlist *src,
				struct scatterlist **dst, unsigned int len,
				int dir)
{
	void *pages;

	*dst = kmalloc(sizeof(**dst), GFP_ATOMIC);
	if (!*dst)
		return -ENOMEM;

	pages = (void *)__get_free_pages(GFP_ATOMIC, get_order(len));
	if (!pages) {
		kfree(*dst);
		*dst = NULL;
		return -ENOMEM;
	}

	ltq_copybuf(pages, src, len, dir);

	sg_init_table(*dst, 1);
	sg_set_buf(*dst, pages, len);

	return 0;
}

static void free_resources(struct ltq_crypt_ctrl *ctrl,
						DMABuf_Handle_t SAHandle, int dev)
{
	int i;

	DMABuf_Release(ctrl->TokenHandle);

	if (ctrl->no_of_particles == 0) {
		DMABuf_Release(ctrl->SrcPktHandle);
		DMABuf_Release(ctrl->DstPktHandle);
	} else {
		PEC_SGList_Destroy(ctrl->SrcPktHandle);
		for (i = 0; i < ctrl->no_of_particles; i++)
			DMABuf_Release(ctrl->SrcScatter_Handle[i]);

		if ((ctrl->SA_flag & CRYPTO_ALG_TYPE_MASK) !=
					CRYPTO_ALG_TYPE_AHASH) {
			PEC_SGList_Destroy(ctrl->DstPktHandle);
			for (i = 0; i < ctrl->no_of_particles; i++)
				DMABuf_Release(ctrl->DstGather_Handle[i]);
		} else {
			DMABuf_Release(ctrl->DstPktHandle);
		}
	}

	/* Free sglist */
	if (ctrl->unaligned)
		ltq_free_sg_copy(&ctrl->sg_cpy, ctrl->copy_bytes);

	kfree(ctrl);
}

static void ltq_pkt_done(PEC_ResultDescriptor_t *res, int result, int dev)
{
	struct ltq_crypt_ctrl *ctrl = res->User_p;
	unsigned int nbytes;
	int i;

	switch (ctrl->SA_flag & CRYPTO_ALG_TYPE_MASK) {
	case CRYPTO_ALG_TYPE_AHASH: {
		struct ahash_request *req =	(struct ahash_request *)ctrl->data.req;
		nbytes = req->nbytes;

		req->base.complete(&req->base, result);
		break;
		}
	case CRYPTO_ALG_TYPE_ABLKCIPHER: {
		struct ablkcipher_request *areq =
						(struct ablkcipher_request *)ctrl->data.ablk_req;
		struct crypto_ablkcipher *ablk_tfm =
						crypto_ablkcipher_reqtfm(areq);
		struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
		nbytes = areq->nbytes;

		if (ctx->params.IVSrc == SAB_IV_SRC_TOKEN)
			areq->info = ctrl->TokenParams.IV_p;

		if (ctrl->unaligned)
			ltq_copybuf(sg_virt(ctrl->sg_cpy), areq->dst,
						areq->nbytes, 1);

		areq->base.complete(&areq->base, result);
		break;
		}
	case CRYPTO_ALG_TYPE_AEAD: {
		struct aead_request *aead =
				(struct aead_request *)ctrl->data.aead_req;
		struct crypto_aead *tfm = crypto_aead_reqtfm(aead);
		struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);
		unsigned int authsize = (ctrl->dir == 1) ? 0 : ctx->authsize;
		unsigned int iv_offset = 0;

		if (ctrl->dir == LTQ_ENC)
			nbytes = aead->cryptlen + aead->assoclen + authsize;
		else
			nbytes = aead->cryptlen + aead->assoclen;

		if (ctx->params.IVSrc != SAB_IV_SRC_TOKEN)
			aead->iv = ctx->params.IV_p;
		else
			aead->iv = ctrl->TokenParams.IV_p;

		/* Nasty work around for AES-CCM function due to HW workings */
		/* CCM includes IV in dst buffer for outbound transform */
		if ((ctx->params.CryptoMode == SAB_CRYPTO_MODE_CCM) ||
			(ctx->params.CryptoMode == SAB_CRYPTO_MODE_GMAC)) {
				if (ctrl->dir == 0)
					iv_offset = crypto_aead_ivsize(tfm);
		}

		/* Handle cases of unaligned address and extern buffers
		 * copybuf = IV | data | auth
		 * dst = data | auth
		 */
		if (ctrl->copybuf && !ctrl->unaligned) {
			memcpy(sg_virt(aead->dst), ctrl->copybuf + iv_offset,
					aead->cryptlen + authsize);
			kfree(ctrl->copybuf);
		} else if (ctrl->copybuf && ctrl->unaligned) {
			/* copy to intermediate aligned buffer */
			memcpy((u8 *)sg_virt(ctrl->sg_cpy) + aead->assoclen,
					ctrl->copybuf + iv_offset,
					aead->cryptlen + authsize);
			/* scatter copy to unaligned buffer */
			ltq_copybuf(sg_virt(ctrl->sg_cpy), aead->dst,
						aead->cryptlen + aead->assoclen +
						authsize, 1);
			kfree(ctrl->copybuf);
		} else if (ctrl->unaligned) {
			/* scatter copy to unaligned buffer */
			ltq_copybuf(sg_virt(ctrl->sg_cpy), aead->dst,
						aead->cryptlen + aead->assoclen +
						authsize, 1);
		}

		aead->base.complete(&aead->base, result);
		break;
		}
	default:
		pr_err("Unkown crypto transform\n");
	}

	DMABuf_Release(ctrl->TokenHandle);

	if (ctrl->no_of_particles == 0) {
		DMABuf_Release(ctrl->SrcPktHandle);
		DMABuf_Release(ctrl->DstPktHandle);
	} else {
		PEC_SGList_Destroy(ctrl->SrcPktHandle);
		for (i = 0; i < ctrl->no_of_particles; i++)
			DMABuf_Release(ctrl->SrcScatter_Handle[i]);

		if ((ctrl->SA_flag & CRYPTO_ALG_TYPE_MASK) !=
			CRYPTO_ALG_TYPE_AHASH) {
			PEC_SGList_Destroy(ctrl->DstPktHandle);
			for (i = 0; i < ctrl->no_of_particles; i++)
				DMABuf_Release(ctrl->DstGather_Handle[i]);
		} else {
			DMABuf_Release(ctrl->DstPktHandle);
		}
	}

	/* Free sglist */
	if (ctrl->unaligned)
		ltq_free_sg_copy(&ctrl->sg_cpy, ctrl->copy_bytes);

	kfree(ctrl);
}

#ifndef CONFIG_LTQ_MPE_IPSEC_SUPPORT
/* Tasklet to process eip97 irq */
static void eip97_ring_irq1(void *data)
{
	unsigned int Pkt_Count = 0;
	int i, LoopCount = 10;
	PEC_Status_t ret;
	PEC_NotifyFunction_t CBFunc = (PEC_NotifyFunction_t) eip97_ring_irq1;

	ret = PEC_Packet_Get(EIP97_RING_DEC, Result_R1, MAX_FETCH_SIZE,
							&Pkt_Count);

	if (ret == PEC_STATUS_BUSY) {
		while (LoopCount--) {
			ret = PEC_Packet_Get(EIP97_RING_DEC, Result_R1, MAX_FETCH_SIZE,
							 &Pkt_Count);
			if (ret == PEC_STATUS_OK)
				break;
		}
	}

	if (!Pkt_Count) {
		PEC_ResultNotificationCount++;
		if (PEC_ResultNotificationCount < 2) {
			ret = PEC_ResultNotify_Request(EIP97_RING_DEC,
											CBFunc,
											1);
			if (ret != PEC_STATUS_OK) {
				PEC_ResultNotificationCount = 0;
				pr_err("%s: Err setting notify request fn\n", __func__);
				ltq_pkt_done(&Result_R1[0], -EINVAL, EIP97_RING_DEC);
				return;
			}
		} else {
			PEC_ResultNotificationCount = 0;
			pr_err("Exceeded irq request\n");
			ltq_pkt_done(&Result_R1[0], -EBUSY, EIP97_RING_DEC);
			return;
		}
	} else {
		/* Send notify request here */
		ret = PEC_ResultNotify_Request(EIP97_RING_DEC, CBFunc, 1);
		if (ret != PEC_STATUS_OK)
			pr_err("%s: Err setting notify request fn\n", __func__);

		atomic_sub(Pkt_Count, &packets_queue);
		for (i = 0; i < Pkt_Count; i++) {
			ltq_pkt_done(&Result_R1[i], (int) ret, EIP97_RING_DEC);
			PEC_ResultNotificationCount = 0;
		}
	}

	return;
}

#endif /* CONFIG_LTQ_MPE_IPSEC_SUPPORT */

/* Tasklet to process eip97 irq */
static void eip97_ring_irq0(void *data)
{
	unsigned int Pkt_Count = 0;
	int i, LoopCount = 10;
	PEC_Status_t ret;
	PEC_NotifyFunction_t CBFunc = (PEC_NotifyFunction_t) eip97_ring_irq0;

	ret = PEC_Packet_Get(EIP97_RING_ENC, Result_R0, MAX_FETCH_SIZE,
							&Pkt_Count);

	if (ret == PEC_STATUS_BUSY) {
		while (LoopCount--) {
			ret = PEC_Packet_Get(EIP97_RING_ENC, Result_R0, MAX_FETCH_SIZE,
							 &Pkt_Count);
			if (ret == PEC_STATUS_OK)
				break;
		}
	}

	if (!Pkt_Count) {
		PEC_ResultNotificationCount++;
		if (PEC_ResultNotificationCount < 2) {
			ret = PEC_ResultNotify_Request(EIP97_RING_ENC,
											CBFunc,
											1);
			if (ret != PEC_STATUS_OK) {
				PEC_ResultNotificationCount = 0;
				pr_err("%s: Err setting notify request fn\n", __func__);
				ltq_pkt_done(&Result_R0[0], -EINVAL, EIP97_RING_ENC);
				return;
			}
		} else {
			PEC_ResultNotificationCount = 0;
			pr_err("Exceeded irq request\n");
			ltq_pkt_done(&Result_R0[0], -EBUSY, EIP97_RING_ENC);
			return;
		}
	} else {
		/* Send notify request here */
		ret = PEC_ResultNotify_Request(EIP97_RING_ENC, CBFunc, 1);
		if (ret != PEC_STATUS_OK)
			pr_err("%s: Err setting notify request fn\n", __func__);

		atomic_sub(Pkt_Count, &packets_queue);
		for (i = 0; i < Pkt_Count; i++) {
			ltq_pkt_done(&Result_R0[i], (int) ret, EIP97_RING_ENC);
			PEC_ResultNotificationCount = 0;
		}
	}

	return;
}

void dma_alloc_properties(DMABuf_Properties_t *dma,
								int fcached, int size,
								int alignment,
								int bank)
{
	dma->fCached = fcached;
	dma->Alignment = alignment;
	dma->Bank = bank;
	dma->Size = size;
}
EXPORT_SYMBOL(dma_alloc_properties);

/* Handle scatter gather particles more than 3B */
static int ltq_buffers_chaining(struct ltq_crypt_ctrl *ctrl,
								struct scatterlist *sg,
								DMABuf_Handle_t *Handle,
								DMABuf_Handle_t *Scatter_Handle,
								unsigned int nbytes,
								unsigned int particles)
{
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	DMABuf_HostAddress_t Address[MAX_SCATTERGATHER_PKT];
	DMABuf_Status_t DMAStatus;
	PEC_Status_t ret;
	struct scatterlist *tmp_sg;
	int i, cnt = 0;
	u8 *buf;

	/* scatter cases*/
	ret = PEC_SGList_Create(particles,
							Handle);
	if (ret != PEC_STATUS_OK) {
		pr_err("Error creating SG list for scatter/gather\n");
		return -EINVAL;
	}

	tmp_sg = sg;
	for (i = 0; i < particles; i++) {
		dma_alloc_properties(&DMAProperties,
							true, tmp_sg->length, LTQ_ALIGN,
							DA_PEC_BANK_PACKET);

		buf = sg_virt(tmp_sg);
		if (!buf) {
			pr_err("sg particle has no valid virtual address!\n");
			return -EFAULT;
		}

#ifdef LTQ_CRYPTO_DEBUG
		pr_info("input:\n");
		print_hex_dump(KERN_CONT, "", DUMP_PREFIX_OFFSET,
						16, 1,
						buf, tmp_sg->length, false);
#endif
		if (((int)buf & 0x3)) {
			DMAStatus = DMABuf_Alloc(DMAProperties, &Address[cnt],
									&Scatter_Handle[i]);
			if (DMAStatus != DMABUF_STATUS_OK) {
				pr_err("DMA error allocting buffer\n");
				return -ENOMEM;
			}

			memcpy(Address[cnt].p, buf, tmp_sg->length);
			cnt++;
		} else {
			DMAStatus = DMABuf_Register(DMAProperties, buf,
									NULL, 'k',
									&Scatter_Handle[i]);
			if (DMAStatus != DMABUF_STATUS_OK) {
				pr_err("Error allocating src buffer in iter %d\n", i);
				return -ENOMEM;
			}
		}

		ret = PEC_SGList_Write(*Handle,
								i,
								Scatter_Handle[i],
								DMAProperties.Size);
		if (ret != PEC_STATUS_OK) {
			pr_err("Error writing to SG List\n");
			return -EINVAL;
		}

		tmp_sg = sg_next(tmp_sg);
		if (!tmp_sg)
			break;
	}

	ctrl->no_of_particles = particles;
	return 0;
}

static int ltq_setup_ahash_assoc(struct crypto_tfm *tfm,
						u32 *ipad, u32 *opad, u32 *auth_key3)
{
	int rc;
	unsigned int SAWords = 0;
	unsigned int TCRWords = 0;
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 auth_flag, crypto_mode, crypto_flag;
	u32 *TCRDataOut;
	DMABuf_Handle_t SAHandle = {0};
	DMABuf_HostAddress_t SAHostAddress;
	DMABuf_Status_t DMAStatus;

	SABuilder_Params_t params;
	SABuilder_Params_Basic_t SAParamsBasic_p;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};

	memset(&params, 0, sizeof(SABuilder_Params_t));
	memset(&SAParamsBasic_p, 0, sizeof(SABuilder_Params_Basic_t));

	rc = SABuilder_Init_Basic(&params, &SAParamsBasic_p, 0);
	if (rc) {
		pr_err("%s: Init failed\n", __func__);
		return -EINVAL;
	}

	crypto_flag = crypto_flag_get(tfm, CRYPTO_ALG_TYPE_AHASH);
	crypto_mode = cryto_mode_get(tfm, CRYPTO_ALG_TYPE_AHASH);
	auth_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AHASH);

	params.CryptoAlgo = crypto_flag;
	params.CryptoMode = crypto_mode;
	params.AuthAlgo = auth_flag;

	if (auth_flag == SAB_AUTH_HMAC_MD5 ||
		auth_flag == SAB_AUTH_HMAC_SHA1 ||
		auth_flag == SAB_AUTH_HMAC_SHA2_224 ||
		auth_flag == SAB_AUTH_HMAC_SHA2_256 ||
		auth_flag == SAB_AUTH_HMAC_SHA2_384 ||
		auth_flag == SAB_AUTH_HMAC_SHA2_512 ||
		auth_flag == SAB_AUTH_AES_CCM ||
		auth_flag == SAB_AUTH_AES_GCM) {

		if (ipad == NULL || opad == NULL) {
			pr_err("opad / ipad keys needed"
				"for HMAC transforms\n");
			return -EINVAL;
		}

		params.AuthKey1_p =  (u8 *)ipad;
		params.AuthKey2_p =  (u8 *)opad;

	} else if (auth_flag == SAB_AUTH_AES_XCBC_MAC) {
		params.AuthKey1_p =  (u8 *)ipad;
		params.AuthKey2_p =  (u8 *)opad;
		params.AuthKey3_p =  (u8 *)auth_key3;
	}

	SAParamsBasic_p.BasicFlags = 0;
	params.flags = 0;
	params.flags |= SAB_FLAG_SUPPRESS_PAYLOAD;

	rc = SABuilder_GetSizes(&params, &SAWords, NULL, NULL);
	if (rc) {
		pr_err("%s: SABuilder_GetSizes returned error\n", __func__);
		return -EINVAL;
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
	if (rc) {
		pr_err("%s: TokenBuilder_GetContextSize return error\n", __func__);
		return -EINVAL;
	}

	TCRDataOut = (u32 *)kmalloc(4*TCRWords, GFP_KERNEL);
	if (!TCRDataOut) {
		pr_err("%s Error allocating memory for token data\n", __func__);
		return -ENOMEM;
	}

	rc = TokenBuilder_BuildContext(&params, TCRDataOut);
	if (rc) {
		pr_err("%s: TokenBuilder_BuildContext return error\n", __func__);
		return -EINVAL;
	}

	rc = PEC_SA_Register(EIP97_RING_ENC, SAHandle, DMABuf_NULLHandle,
						DMABuf_NULLHandle);
	if (rc != PEC_STATUS_OK) {
		pr_err("%s: Failed to register SA\n", __func__);
		return -EINVAL;
	}

	ctx->config_sa_done = 1;
	ctx->SAWords = SAWords;
	ctx->SAHandle = SAHandle;
	ctx->params = (SABuilder_Params_t) params;
	ctx->TCRDataOut = TCRDataOut;

	return 0;
}

static int ltq_setup_aead_assoc(struct crypto_aead *authenc,
						struct ltq_crypt_ctrl *ctrl,
						struct ltq_aead_ctx *ctx,
						int dir, int enckeylen,
						u8 *key, u8 *iv,
						u32 *ipad, u32 *opad)
{
	struct crypto_tfm *tfm = crypto_aead_tfm(authenc);
	int rc, ring;
	unsigned int SAWords = 0;
	unsigned int TCRWords = 0;
	unsigned int authsize = crypto_aead_authsize(authenc);
	struct crypto_alg *alg = tfm->__crt_alg;
	u32 auth_flag, crypto_mode, crypto_flag;
	DMABuf_Handle_t SAHandle = {0};
	DMABuf_HostAddress_t SAHostAddress;
	DMABuf_Status_t DMAStatus;

	SABuilder_Params_t params;
	SABuilder_Params_Basic_t SAParamsBasic_p;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	u32 *TCRDataOut;

	memset(&params, 0, sizeof(SABuilder_Params_t));
	memset(&SAParamsBasic_p, 0, sizeof(SABuilder_Params_Basic_t));

	if (dir == LTQ_ENC)
		ring = EIP97_RING_ENC;
	else
		ring = EIP97_RING_DEC;

	ctx->SA_flag = alg->cra_flags;

	rc = SABuilder_Init_Basic(&params, &SAParamsBasic_p, dir);
	if (rc) {
		pr_err("%s: Init failed\n", __func__);
		return -EINVAL;
	}

	crypto_flag = crypto_flag_get(tfm, CRYPTO_ALG_TYPE_AEAD);
	crypto_mode = cryto_mode_get(tfm, CRYPTO_ALG_TYPE_AEAD);
	auth_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AEAD);

	pr_debug("crypto flg: %d crypto mode: %d auth flag: %d\n", crypto_flag,
			crypto_mode, auth_flag);

	/* Authenc hash setup */
	params.CryptoAlgo = crypto_flag;
	params.CryptoMode = crypto_mode;
	params.AuthAlgo = auth_flag;
	if (auth_flag != SAB_AUTH_NULL &&
		auth_flag != SAB_AUTH_AES_CCM &&
		auth_flag != SAB_AUTH_AES_GCM &&
		auth_flag != SAB_AUTH_AES_GMAC) {
		/* precompute keys needed*/
		if (auth_flag == SAB_AUTH_HMAC_MD5 ||
			auth_flag == SAB_AUTH_HMAC_SHA1 ||
			auth_flag == SAB_AUTH_HMAC_SHA2_224 ||
			auth_flag == SAB_AUTH_HMAC_SHA2_256 ||
			auth_flag == SAB_AUTH_HMAC_SHA2_384 ||
			auth_flag == SAB_AUTH_HMAC_SHA2_512) {

			if (ipad == NULL || opad == NULL) {
				pr_err("opad / ipad keys needed"
						"for HMAC transforms\n");
				return -EINVAL;
			}

			params.AuthKey1_p =  (u8 *)ipad;
			params.AuthKey2_p =  (u8 *)opad;
		}
		/* do not copy hash to output for basic hash tfm */
		if (crypto_flag == SAB_CRYPTO_NULL)
			params.flags |= SAB_FLAG_SUPPRESS_PAYLOAD;

	} else if (auth_flag == SAB_AUTH_AES_GCM ||
		auth_flag == SAB_AUTH_AES_GMAC) {
		/* Pre-calculated H value of GCM */
		params.AuthKey1_p = (u8 *)ipad;
	}

	if (crypto_flag != SAB_CRYPTO_NULL) {
		params.KeyByteCount = enckeylen;
		params.Key_p = key;
	}

	SAParamsBasic_p.BasicFlags = SAB_BASIC_FLAG_EXTRACT_ICV;

	if (crypto_flag != SAB_CRYPTO_NULL &&
		auth_flag != SAB_AUTH_NULL &&
		auth_flag != SAB_AUTH_AES_CCM &&
		auth_flag != SAB_AUTH_AES_GCM &&
		auth_flag != SAB_AUTH_AES_GMAC) {
		/* Prevent the copy of ICV */
		params.flags |= SAB_FLAG_SUPPRESS_HEADER;
		params.IVSrc = SAB_IV_SRC_TOKEN;
		params.IV_p = iv; /* So that IV will also be hashed */
		if (dir) {
			/* Incoming packets */
			SAParamsBasic_p.ICVByteCount = authsize;
		} else {
			/* outgoing packets */
			SAParamsBasic_p.ICVByteCount = authsize;
			params.flags &= ~(SAB_FLAG_COPY_IV);
		}
	} else if (auth_flag == SAB_AUTH_AES_GCM ||
				auth_flag == SAB_AUTH_AES_CCM ||
				auth_flag == SAB_AUTH_AES_GMAC) {
		if (!dir)
			params.flags &= ~(SAB_FLAG_COPY_IV);
		/* Prevent the copy of ICV & IV */
		params.flags |= SAB_FLAG_SUPPRESS_HEADER;
		params.IV_p = iv; /* So that IV will also be hashed */
		params.IVSrc = SAB_IV_SRC_TOKEN;
		SAParamsBasic_p.ICVByteCount = authsize;
	}

	if (crypto_mode == SAB_CRYPTO_MODE_CCM ||
		crypto_mode == SAB_CRYPTO_MODE_GCM ||
		crypto_mode == SAB_CRYPTO_MODE_GMAC ||
		crypto_mode == SAB_CRYPTO_MODE_CTR) {
		params.Nonce_p = ctx->nonce;
	}

	rc = SABuilder_GetSizes(&params, &SAWords, NULL, NULL);
	if (rc) {
		pr_err("%s: SABuilder_GetSizes returned error\n", __func__);
		return -EINVAL;
	}

	dma_alloc_properties(&DMAProperties,
					true, (4 * SAWords), LTQ_ALIGN,
					DA_PEC_BANK_PACKET);

	DMAStatus = DMABuf_Alloc(DMAProperties, &SAHostAddress, &SAHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Error allocating DMA buffer for SA\n", __func__);
		return -EAGAIN;
	}

	rc = SABuilder_BuildSA(&params,
				(uint32_t *)SAHostAddress.p, NULL, NULL);
	if (rc != 0) {
		pr_err("%s: Error building SA\n", __func__);
		DMABuf_Release(SAHandle);
		return -EINVAL;
	}

	rc = TokenBuilder_GetContextSize(&params, &TCRWords);
	if (rc) {
		pr_err("%s: TokenBuilder_GetContextSize return error\n", __func__);
		return -EINVAL;
	}

	TCRDataOut = (u32 *)kmalloc(4*TCRWords, GFP_KERNEL);
	if (!TCRDataOut) {
		pr_err("%s Error allocating memory for token data\n", __func__);
		return -ENOMEM;
	}

	rc = TokenBuilder_BuildContext(&params, TCRDataOut);
	if (rc) {
		pr_err("%s: TokenBuilder_BuildContext return error\n", __func__);
		return -EINVAL;
	}

	rc = PEC_SA_Register(ring, SAHandle, DMABuf_NULLHandle,
						DMABuf_NULLHandle);
	if (rc != PEC_STATUS_OK) {
		pr_err("%s: Failed to register SA\n", __func__);
		return -EINVAL;
	}

	ctx->SAWords = SAWords;
	ctx->SAHandle = SAHandle;
	ctx->params = (SABuilder_Params_t) params;
	ctx->TCRDataOut = TCRDataOut;
	ctx->direction = dir;

	return 0;
}

/* Setup SA records */
static int ltq_setup_ablk_assoc(struct crypto_tfm *tfm,
						struct ltq_crypt_ctrl *ctrl,
						struct ltq_crypto_ctx *ctx,
						int dir, int enckeylen,
						u8 *key, u8 *iv)
{
	int rc;
	unsigned int SAWords = 0;
	unsigned int TCRWords = 0;
	struct crypto_alg *alg = tfm->__crt_alg;
	u32 auth_flag, crypto_mode, crypto_flag;
	u32 *TCRDataOut;
	DMABuf_Handle_t SAHandle = {0};
	DMABuf_HostAddress_t SAHostAddress;
	DMABuf_Status_t DMAStatus;

	SABuilder_Params_t params;
	SABuilder_Params_Basic_t SAParamsBasic_p;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};

	memset(&params, 0, sizeof(SABuilder_Params_t));

	ctx->SA_flag = alg->cra_flags;
	ctrl->dir = dir;

	rc = SABuilder_Init_Basic(&params, &SAParamsBasic_p, dir);
	if (rc) {
		pr_err("%s: Init failed\n", __func__);
		return -EINVAL;
	}

	crypto_flag = crypto_flag_get(tfm, CRYPTO_ALG_TYPE_ABLKCIPHER);
	crypto_mode = cryto_mode_get(tfm, CRYPTO_ALG_TYPE_ABLKCIPHER);
	auth_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_ABLKCIPHER);

	pr_debug("crypto flg: %d crypto mode: %d auth flag: %d\n",
			crypto_flag, crypto_mode, auth_flag);

	/* Authenc hash setup */
	params.CryptoAlgo = crypto_flag;
	params.CryptoMode = crypto_mode;
	params.AuthAlgo = auth_flag;

	if (crypto_flag != SAB_CRYPTO_NULL) {
		params.KeyByteCount = enckeylen;
		params.Key_p = key;
	}

	params.IVSrc = SAB_IV_SRC_TOKEN;
	SAParamsBasic_p.BasicFlags = 0;

	rc = SABuilder_GetSizes(&params, &SAWords, NULL, NULL);
	if (rc) {
		pr_err("%s: SABuilder_GetSizes returned error\n", __func__);
		return -EINVAL;
	}

	dma_alloc_properties(&DMAProperties,
					true, (4 * SAWords), LTQ_ALIGN,
					DA_PEC_BANK_PACKET);

	DMAStatus = DMABuf_Alloc(DMAProperties, &SAHostAddress, &SAHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Error allocating DMA buffer for SA\n", __func__);
		return -EAGAIN;
	}

	rc = SABuilder_BuildSA(&params,
					(uint32_t *)SAHostAddress.p, NULL, NULL);
	if (rc != 0) {
		pr_err("%s: Error building SA\n", __func__);
		DMABuf_Release(SAHandle);
		return -EINVAL;
	}

	rc = TokenBuilder_GetContextSize(&params, &TCRWords);
	if (rc) {
		pr_err("%s: TokenBuilder_GetContextSize return error\n", __func__);
		return -EINVAL;
	}

	TCRDataOut = (u32 *)kmalloc(4*TCRWords, GFP_KERNEL);
	if (!TCRDataOut) {
		pr_err("%s Error allocating memory for token data\n", __func__);
		return -ENOMEM;
	}

	rc = TokenBuilder_BuildContext(&params, TCRDataOut);
	if (rc) {
		pr_err("%s: TokenBuilder_BuildContext return error\n", __func__);
		return -EINVAL;
	}

	rc = PEC_SA_Register(EIP97_RING_ENC, SAHandle, DMABuf_NULLHandle,
						DMABuf_NULLHandle);
	if (rc != PEC_STATUS_OK) {
		pr_err("%s: Failed to register SA\n", __func__);
		return -EINVAL;
	}

	ctx->config_sa_done = 1;
	ctx->SAWords = SAWords;
	ctx->SAHandle = SAHandle;
	ctx->params = (SABuilder_Params_t) params;
	ctx->TCRDataOut = TCRDataOut;

	return 0;
}

/* ABLKCIPHER functions */
static int ltq_process_cipher(struct ablkcipher_request *req,
						struct ltq_crypt_ctrl *ctrl,
						struct scatterlist *src_sg,
						struct scatterlist *dst_sg,
						int decrypt,
						void (*callback) (void *data))
{
	int rc, ring = 0;
	unsigned int count;
	unsigned int nbytes = req->nbytes;
	unsigned int TokenWords = 0;
	unsigned int TokenHeaderWord;
	unsigned int TokenMaxWordsOut = 0;
	unsigned int tries = 5;
	u8 *src, *dst;
	struct crypto_ablkcipher *ablk_tfm = crypto_ablkcipher_reqtfm(req);
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
	void *TCRDataOut = (void *)ctx->TCRDataOut;

	DMABuf_Status_t DMAStatus;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	DMABuf_Handle_t SAHandle = ctx->SAHandle;
	DMABuf_Handle_t TokenHandle = {0};
	DMABuf_HostAddress_t TokenHostAddress;
	TokenBuilder_Params_t TokenParams;
	PEC_CommandDescriptor_t Cmd;
	PEC_NotifyFunction_t CBFunc;

	/* This value can be tweaked. From observation,
	 * performance of the driver drops at around
	 * this value
	 */
	if (atomic_read(&packets_queue) > 500) {
		kfree(ctrl);
		return -EBUSY;
	}

	rc = TokenBuilder_GetSize(TCRDataOut, &TokenMaxWordsOut);
	if (rc) {
		pr_err("%s: TokenBuilder_GetSize return error\n", __func__);
		rc = -EINVAL;
		goto cipher_err;
	}

	dma_alloc_properties(&DMAProperties,
						true, (4 * TokenMaxWordsOut),
						LTQ_ALIGN,
						DA_PEC_BANK_PACKET);

	DMAStatus = DMABuf_Alloc(DMAProperties,
						&TokenHostAddress, &TokenHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Failed to allocate Token DMA buf\n", __func__);
		rc = -ENOMEM;
		goto cipher_err;
	}

	/* src buffer */
	if (count_sg(src_sg) == 1) {
		ctrl->no_of_particles = 0;
		dma_alloc_properties(&DMAProperties,
						true, nbytes, LTQ_ALIGN,
						DA_PEC_BANK_PACKET);

		src = sg_virt(src_sg);
		DMABuf_Register(DMAProperties, src, NULL, 'k', &ctrl->SrcPktHandle);
		if (DMAStatus != DMABUF_STATUS_OK) {
			pr_err("%s: Failed to register Src DMA buf\n", __func__);
			rc = -ENOMEM;
			goto cipher_err;
		}

		dst = sg_virt(dst_sg);
		dma_alloc_properties(&DMAProperties,
						true, nbytes,
						LTQ_ALIGN,
						DA_PEC_BANK_PACKET);

		DMABuf_Register(DMAProperties, dst,  NULL, 'k', &ctrl->DstPktHandle);
		if (DMAStatus != DMABUF_STATUS_OK) {
			pr_err("%s: Failed to register dst DMA buf\n", __func__);
			rc = -ENOMEM;
			goto cipher_err;
		}
	} else {
		rc = ltq_buffers_chaining(ctrl, src_sg,
						&ctrl->SrcPktHandle,
						ctrl->SrcScatter_Handle,
						nbytes,
						count_sg(src_sg));
		if (rc) {
			pr_err("Error allocating sg buffers\n");
			rc = -ENOMEM;
			goto cipher_err;
		}
		rc = ltq_buffers_chaining(ctrl, dst_sg,
						&ctrl->DstPktHandle,
						ctrl->DstGather_Handle,
						nbytes,
						count_sg(dst_sg));
		if (rc) {
			pr_err("Error allocating sg buffers\n");
			rc = -ENOMEM;
			goto cipher_err;
		}
	}

	ctrl->TokenHandle = TokenHandle;
	TokenParams.PacketFlags = 0;
	TokenParams.PadByte = 0;
	TokenParams.BypassByteCount = 0;
	TokenParams.AdditionalValue = 0;

	TokenParams.IV_p = req->info;

	rc = TokenBuilder_BuildToken(TCRDataOut,
						(uint8_t *)ctrl->SrcPktHandle.p,
						nbytes,
						&TokenParams,
						(uint32_t *)TokenHostAddress.p,
						&TokenWords,
						&TokenHeaderWord);
	if (rc != TKB_STATUS_OK) {
		if (rc == TKB_BAD_PACKET)
			pr_err("Token not created because packet size is invalid\n");
		else
			pr_err("Token builder failed\n");
		rc = -EINVAL;
		goto cipher_err;
	}

	ctrl->TokenParams = TokenParams;

	Cmd.User_p = ctrl;
	Cmd.Token_Handle = TokenHandle;
	Cmd.Token_WordCount = TokenWords;
	Cmd.SrcPkt_Handle = ctrl->SrcPktHandle;
	Cmd.SrcPkt_ByteCount = nbytes;
	Cmd.DstPkt_Handle = ctrl->DstPktHandle;
	Cmd.Bypass_WordCount = 0;
	Cmd.SA_Handle1 = SAHandle;
	Cmd.SA_Handle2 = DMABuf_NULLHandle;
	Cmd.SA_WordCount = 0;
	Cmd.Control1 = TokenHeaderWord;
	Cmd.Control2 = 0;

	rc = PEC_Packet_Put(EIP97_RING_ENC,
							&Cmd,
							1,
							&count);

	if (rc == PEC_STATUS_BUSY) {
		while (tries > 0) {
			udelay(300);
			rc = PEC_Packet_Put(EIP97_RING_ENC,
						&Cmd, 1, &count);
			if (rc == PEC_STATUS_OK)
				break;

			tries--;
		}
	}
	if ((rc != PEC_STATUS_OK) || (count != 1)) {
		rc = -EAGAIN;
		pr_debug("Packet Put error\n");
		goto cipher_err;
	}

	atomic_inc(&packets_queue);
	return -EINPROGRESS;

cipher_err:
	free_resources(ctrl, SAHandle, ring);
	return rc;

}

static int ltq_ablk_cra_init(struct crypto_tfm *tfm)
{
	struct ltq_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	ctx->config_sa_done = 0;
	ctx->TCRDataOut = NULL;
	ctx->registered = 0;

	return 0;
}

static void ltq_ablk_cra_exit(struct crypto_tfm *tfm)
{
	int dev = 0;
	struct ltq_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	u32 *TCRDataOut = ctx->TCRDataOut;

	ctx->config_sa_done = 0;
	if (TCRDataOut)
		kfree(TCRDataOut);

	PEC_SA_UnRegister(dev, ctx->SAHandle,
					DMABuf_NULLHandle,
					DMABuf_NULLHandle);

	DMABuf_Release(ctx->SAHandle);
}

static int ltq_ablk_setkey(struct crypto_ablkcipher *tfm, const u8 *key,
				  unsigned int keylen)
{
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(tfm);
	u32 tmp[DES_EXPKEY_WORDS];

	ctx->config_sa_done = 0;
	if (keylen > LTQ_MAX_KEYLEN) {
		pr_err("Bad key provided\n");
		return -EBADMSG;
	}

	if (unlikely(crypto_ablkcipher_get_flags(tfm) &
		     CRYPTO_TFM_REQ_WEAK_KEY) &&
	    !des_ekey(tmp, key)) {
		crypto_ablkcipher_set_flags(tfm, CRYPTO_TFM_RES_WEAK_KEY);
		return -EINVAL;
	}

	memcpy(ctx->key, key, keylen);
	ctx->enckeylen = keylen;

	return 0;
}

static int ltq_rfc3686_setkey(struct crypto_ablkcipher *tfm, const u8 *key,
				  unsigned int keylen)
{
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(tfm);

	/* the nonce is stored in bytes at end of key */
	if (keylen < CTR_RFC3686_NONCE_SIZE)
		return -EINVAL;

	memcpy(ctx->nonce, key + (keylen - CTR_RFC3686_NONCE_SIZE),
		CTR_RFC3686_NONCE_SIZE);

	keylen -= CTR_RFC3686_NONCE_SIZE;
	return ltq_ablk_setkey(tfm, key, keylen);
}

static int ltq_rfc3686_encrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *ablk_tfm = crypto_ablkcipher_reqtfm(req);
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(ablk_tfm);
	u8 iv[CTR_RFC3686_BLOCK_SIZE];
	u8 *info = req->info;
	u32 *TCRDataOut = ctx->TCRDataOut;
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src, *dst;
	int ret, dev = 0;

	memcpy(iv, ctx->nonce, CTR_RFC3686_NONCE_SIZE);
	memcpy(iv + CTR_RFC3686_NONCE_SIZE, info, CTR_RFC3686_IV_SIZE);

	/* initialize counter portion of counter block */
	*(__be32 *)(iv + CTR_RFC3686_NONCE_SIZE + CTR_RFC3686_IV_SIZE) =
		cpu_to_be32(1);

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->unaligned = 0;
	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy, req->nbytes, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->copy_bytes = req->nbytes;
		ctrl->unaligned = 1;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	req->info = iv;
	if ((ctx->config_sa_done == 0)) {
		/* workaround for tcrypt test cases */
		if (TCRDataOut) {
			kfree(TCRDataOut);
			PEC_SA_UnRegister(dev, ctx->SAHandle,
						DMABuf_NULLHandle,
						DMABuf_NULLHandle);
			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_ablk_assoc(tfm, ctrl, ctx, LTQ_ENC,
							ctx->enckeylen,
							ctx->key,
							req->info);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
	}

	ctrl->SA_flag = ctx->SA_flag;
	ctrl->data.ablk_req = req;
	ret = ltq_process_cipher(req, ctrl, src, dst,
				LTQ_ENC, EIP97_ENC_CALLBACK);
	req->info = info;
	return ret;
}

static int ltq_rfc3686_decrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *ablk_tfm = crypto_ablkcipher_reqtfm(req);
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(ablk_tfm);
	u8 iv[CTR_RFC3686_BLOCK_SIZE];
	u8 *info = req->info;
	u32 *TCRDataOut = ctx->TCRDataOut;
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src, *dst;
	int ret, dev = 0;

	memcpy(iv, ctx->nonce, CTR_RFC3686_NONCE_SIZE);
	memcpy(iv + CTR_RFC3686_NONCE_SIZE, info, CTR_RFC3686_IV_SIZE);

	/* initialize counter portion of counter block */
	*(__be32 *)(iv + CTR_RFC3686_NONCE_SIZE + CTR_RFC3686_IV_SIZE) =
		cpu_to_be32(1);

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->unaligned = 0;
	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy, req->nbytes, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->copy_bytes = req->nbytes;
		ctrl->unaligned = 1;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	req->info = iv;
	if ((ctx->config_sa_done == 0)) {
		/* workaround for tcrypt test cases */
		if (TCRDataOut) {
			kfree(TCRDataOut);
			PEC_SA_UnRegister(dev, ctx->SAHandle,
							DMABuf_NULLHandle,
							DMABuf_NULLHandle);
			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_ablk_assoc(tfm, ctrl, ctx, LTQ_DEC,
								ctx->enckeylen,
								ctx->key,
								req->info);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
	}

	ctrl->SA_flag = ctx->SA_flag;
	ctrl->data.ablk_req = req;
	ret = ltq_process_cipher(req, ctrl, src, dst,
							LTQ_DEC, EIP97_ENC_CALLBACK);
	req->info = info;
	return ret;
}

static int ltq_ablk_encrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *ablk_tfm = crypto_ablkcipher_reqtfm(req);
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(ablk_tfm);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src, *dst;
	u32 *TCRDataOut = ctx->TCRDataOut;
	int ret, dev = 0;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->unaligned = 0;
	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy, req->nbytes, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->copy_bytes = req->nbytes;
		ctrl->unaligned = 1;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if ((ctx->config_sa_done == 0)) {
		/* workaround for tcrypt test cases */
		if (TCRDataOut) {
			kfree(TCRDataOut);
			PEC_SA_UnRegister(dev, ctx->SAHandle,
							DMABuf_NULLHandle,
							DMABuf_NULLHandle);
			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_ablk_assoc(tfm, ctrl, ctx, LTQ_ENC,
								ctx->enckeylen,
								ctx->key,
								req->info);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
	}

	ctrl->SA_flag = ctx->SA_flag;
	ctrl->data.ablk_req = req;
	return ltq_process_cipher(req, ctrl, src, dst,
							LTQ_ENC, EIP97_ENC_CALLBACK);
}

static int ltq_ablk_decrypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *ablk_tfm = crypto_ablkcipher_reqtfm(req);
	struct ltq_crypto_ctx *ctx = crypto_ablkcipher_ctx(ablk_tfm);
	struct crypto_tfm *tfm = crypto_ablkcipher_tfm(ablk_tfm);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src, *dst;
	u32 *TCRDataOut = ctx->TCRDataOut;
	int ret, dev = 0;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->unaligned = 0;
	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy, req->nbytes, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->copy_bytes = req->nbytes;
		ctrl->unaligned = 1;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if ((ctx->config_sa_done == 0)) {
		/* workaround for tcrypt test cases */
		if (TCRDataOut) {
			kfree(TCRDataOut);
			PEC_SA_UnRegister(dev, ctx->SAHandle,
							DMABuf_NULLHandle,
							DMABuf_NULLHandle);
			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_ablk_assoc(tfm, ctrl, ctx, LTQ_DEC,
								ctx->enckeylen,
								ctx->key,
								req->info);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
	}

	ctrl->SA_flag = ctx->SA_flag;
	ctrl->data.ablk_req = req;
	return ltq_process_cipher(req, ctrl, src, dst,
							LTQ_DEC, EIP97_ENC_CALLBACK);
}

/* AHASH functions */
static int ltq_process_ahash(struct ltq_ahash_ctx *ctx,
							struct ltq_crypt_ctrl *ctrl,
							struct ahash_request *req,
							struct scatterlist *src_sg,
							void (*callback) (void *data))
{
	int sg_cnt, rc, count, ring = 0;
	unsigned int nbytes = req->nbytes;
	unsigned int ds = ctx->ds;
	unsigned int TokenWords = 0;
	unsigned int TokenHeaderWord;
	unsigned int tries = 5;
	void *TCRDataOut = (void *)ctx->TCRDataOut;
	u8 *src;

	DMABuf_Status_t DMAStatus;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	DMABuf_Handle_t SAHandle = ctx->SAHandle;
	DMABuf_Handle_t TokenHandle = {0};

	DMABuf_HostAddress_t TokenHostAddress;
	TokenBuilder_Params_t TokenParams;

	PEC_CommandDescriptor_t Cmd;
	PEC_NotifyFunction_t CBFunc;

	memset(&TokenParams, 0, sizeof(TokenBuilder_Params_t));

	/* This value can be tweaked. From observation,
	 * performance of the driver drops at around
	 * this value
	 */
	if (atomic_read(&packets_queue) > 500) {
		kfree(ctrl);
		return -EBUSY;
	}

	dma_alloc_properties(&DMAProperties,
						true, (4 * MAX_HASH_TKN_SIZE),
						LTQ_ALIGN,
						DA_PEC_BANK_PACKET);

	DMAStatus = DMABuf_Alloc(DMAProperties, &TokenHostAddress, &TokenHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Failed to allocate Token DMA buf\n", __func__);
		rc = -EAGAIN;
		goto ahash_err;
	}

	/* Register dst buffer */
	dma_alloc_properties(&DMAProperties,
						true, ds,
						LTQ_ALIGN,
						DA_PEC_BANK_PACKET);

	DMABuf_Register(DMAProperties, req->result,
					NULL, 'k', &ctrl->DstPktHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Failed to register dst DMA buf\n", __func__);
		rc = -EAGAIN;
		goto ahash_err;
	}

	/* Check for scattergather, register src buffers */
	sg_cnt = count_sg(src_sg);
	if (sg_cnt == 1) {
		ctrl->no_of_particles = 0;
		src = sg_virt(src_sg);
		dma_alloc_properties(&DMAProperties,
							true, nbytes,
							LTQ_ALIGN,
							DA_PEC_BANK_PACKET);

		DMABuf_Register(DMAProperties, src,
						NULL, 'k', &ctrl->SrcPktHandle);
		if (DMAStatus != DMABUF_STATUS_OK) {
			pr_err("%s: Failed to register Src DMA buf\n", __func__);
			rc = -ENOMEM;
			goto ahash_err;
		}
	} else {
		rc = ltq_buffers_chaining(ctrl, src_sg,
								&ctrl->SrcPktHandle,
								ctrl->SrcScatter_Handle,
								nbytes,
								count_sg(src_sg));
		if (rc) {
			pr_err("Error allocating sg buffers\n");
			rc = -ENOMEM;
			goto ahash_err;
		}
	}

	ctrl->TokenHandle = TokenHandle;
	TokenParams.PacketFlags = 0;
	TokenParams.PacketFlags |= ctx->TokenFlags;
	TokenParams.PadByte = 0;
	TokenParams.IV_p = NULL;
	TokenParams.BypassByteCount = 0;
	TokenParams.AdditionalValue = 0;
	TokenParams.AAD_p = NULL;

	rc = TokenBuilder_BuildToken((void *)TCRDataOut,
						(uint8_t *)ctrl->SrcPktHandle.p,
						nbytes,
						&TokenParams,
						(uint32_t *)TokenHostAddress.p,
						&TokenWords,
						&TokenHeaderWord);
	if (rc != TKB_STATUS_OK) {
		if (rc == TKB_BAD_PACKET)
			pr_err("Token not created because packet size is invalid\n");
		else
			pr_err("Token builder failed\n");

		rc = -EINVAL;
		goto ahash_err;
	}

#ifdef LTQ_CRYPTO_DEBUG
	{
		u8 *tmp = (u8 *)TokenHostAddress.p;
		int i;
		pr_info("AHASH TokenHeaderWord: %08x\n", TokenHeaderWord);
		for (i = 0; i < TokenWords; i++) {
			if (i == 0) {
				tmp[i] &= 0x00ffffff;
				tmp[i] |= (0x2 << 24);
			}
			pr_info("Token[%d]: %08x\n", i, (u32) tmp[i]);
		}
	}
#endif

	Cmd.User_p = ctrl;
	Cmd.Token_Handle = TokenHandle;
	Cmd.Token_WordCount = TokenWords;
	Cmd.SrcPkt_Handle = ctrl->SrcPktHandle;
	Cmd.SrcPkt_ByteCount = nbytes;
	Cmd.DstPkt_Handle = ctrl->DstPktHandle;
	Cmd.Bypass_WordCount = 0;
	Cmd.SA_Handle1 = SAHandle;
	Cmd.SA_Handle2 = DMABuf_NULLHandle;
	Cmd.SA_WordCount = 0;
	Cmd.Control1 = TokenHeaderWord;
	Cmd.Control2 = 0;

	rc = PEC_Packet_Put(EIP97_RING_ENC,
							&Cmd,
							1,
							&count);

	if (rc == PEC_STATUS_BUSY) {
		while (tries > 0) {
			udelay(300);
			rc = PEC_Packet_Put(EIP97_RING_ENC,
						&Cmd, 1, &count);
			if (rc == PEC_STATUS_OK)
				break;

			tries--;
		}
	}
	if ((rc != PEC_STATUS_OK) || (count != 1)) {
		pr_debug("Packet Put error\n");
		rc = -EAGAIN;
		goto ahash_err;
	}

	atomic_inc(&packets_queue);
	return -EINPROGRESS;

ahash_err:
	free_resources(ctrl, SAHandle, ring);
	return rc;
}

void ltq_aes_setkey_done(struct crypto_async_request *req, int err)
{
	struct ltq_aes_result *res = req->data;

	if (err == -EINPROGRESS)
		return;

	res->err = err;
	complete(&res->completion);
}

/* Pre-compute keys using aes algorithm */
static int software_aes_precompute(char *algo, u8 *input, u8 *output,
								const u8 *key, unsigned int keylen)
{
	int err = 0;
	struct crypto_skcipher *ablk_tfm;
	struct {
		u8 hash_res[16];
		u8 iv[8];

		/* ablkcipher callback */
		struct ltq_aes_result result;

		struct scatterlist sg[1];
		struct skcipher_request *req;
	} *data;

	ablk_tfm = crypto_alloc_skcipher(algo, 0, 0);
	if (IS_ERR(ablk_tfm)) {
		pr_err("Error allocating skcipher transform\n");
		return PTR_ERR(ablk_tfm);
	}

	data = kzalloc(sizeof(*data) + crypto_skcipher_reqsize(ablk_tfm),
			   GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->req = skcipher_request_alloc(ablk_tfm, GFP_KERNEL);
	if (!data->req) {
		pr_err("Error allocating request for ablkcipher tfm\n");
		goto out;
	}
	crypto_skcipher_clear_flags(ablk_tfm, CRYPTO_TFM_REQ_MASK);

	err = crypto_skcipher_setkey(ablk_tfm, key, keylen);
	if (err)
		return err;

	memset(data->iv, 0, 16);
	memcpy(data->hash_res, input, 16);

	init_completion(&data->result.completion);
	sg_init_one(data->sg, &data->hash_res, sizeof(data->hash_res));
	skcipher_request_set_tfm(data->req, ablk_tfm);
	skcipher_request_set_callback(data->req, CRYPTO_TFM_REQ_MAY_SLEEP |
							CRYPTO_TFM_REQ_MAY_BACKLOG,
							ltq_aes_setkey_done,
							&data->result);
	skcipher_request_set_crypt(data->req, data->sg, data->sg,
					 sizeof(data->hash_res), data->iv);

	err = crypto_skcipher_encrypt(data->req);
	if (err == -EINPROGRESS || err == -EBUSY) {
		err = wait_for_completion_interruptible(
			&data->result.completion);
		if (!err)
			err = data->result.err;
	}

	memcpy(output, data->hash_res, AES_BLOCK_SIZE);

out:
	kfree(data);
	crypto_free_skcipher(ablk_tfm);
	return 0;
}

int ltq_xcbc_precompute(u8 *key1, u8 *key2, u8 *key3, const u8 *key,
				unsigned int keylen)
{
	int err = 0;
	unsigned char algo[] = "ecb(aes)";

	static uint8_t K1_In[] = {
			0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
			0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
	static uint8_t K2_In[] = {
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
			0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};
	static uint8_t K3_In[] = {
			0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
			0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};

	err = software_aes_precompute(algo, K1_In, key1, key, keylen);
	if (err) {
		pr_err("Compute key1 err\n");
		goto out;
	}
	err = software_aes_precompute(algo, K2_In, key2, key, keylen);
	if (err) {
		pr_err("Compute key2 err\n");
		goto out;
	}
	err = software_aes_precompute(algo, K3_In, key3, key, keylen);
	if (err) {
		pr_err("Compute key3 err\n");
		goto out;
	}

out:
	return err;
}
EXPORT_SYMBOL(ltq_xcbc_precompute);

/* Need to calculate the ipad/opad from the hmac keys */
static int ltq_xcbc_ahash_setkey(struct crypto_ahash *ahash, const u8 *key,
			  unsigned int keylen)
{
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(crypto_ahash_tfm(ahash));
	int ret = 0;

	/* fallback keys just in case we need them for init/update/final */
	ret = crypto_ahash_setkey(ctx->fallback, key, keylen);
	if (ret)
		return ret;

	ret = ltq_xcbc_precompute((u8 *)ctx->ipad, (u8 *)ctx->opad,
						(u8 *)ctx->auth_key3,
						key, keylen);
	if (ret) {
		pr_err("Error preprocessing keys for AES-XCBC, err=%d\n", ret);
		return -EINVAL;
	}

	return 0;
}

void ltq_sha1_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len)
{
	const struct sha1_state *isha1_state = istate, *osha1_state = ostate;
	int i;

	for (i = 0; i < (len / 4); i++) {
		ipad[i] = cpu_to_be32(isha1_state->state[i]);
		opad[i] = cpu_to_be32(osha1_state->state[i]);
	}
}

void ltq_md5_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len)
{
	cpu_to_le32_array(istate, len);
	cpu_to_le32_array(ostate, len);

	memcpy(ipad, istate, len);
	memcpy(opad, ostate, len);
}

void ltq_sha224_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len)
{
	const struct sha256_state *isha256_state = istate;
	const struct sha256_state *osha256_state = ostate;
	int i;

	for (i = 0; i < (len / 4); i++) {
		ipad[i] = cpu_to_be32(isha256_state->state[i]);
		opad[i] = cpu_to_be32(osha256_state->state[i]);
	}
}

void ltq_sha256_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len)
{
	const struct sha256_state *isha256_state = istate;
	const struct sha256_state *osha256_state = ostate;
	int i;

	for (i = 0; i < (len / 4); i++) {
		ipad[i] = cpu_to_be32(isha256_state->state[i]);
		opad[i] = cpu_to_be32(osha256_state->state[i]);
	}
}

void ltq_sha512_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len)
{
	const struct sha512_state *isha512_state = istate;
	const struct sha512_state *osha512_state = ostate;
	__be64 *hash512_out, *hash512_in;
	__be32 *hash_state_out, *hash_state_in;
	int i;

	hash_state_in = (__be32 *)ipad;
	hash_state_out = (__be32 *)opad;

	hash512_in = (__be64 *)hash_state_in;
	hash512_out = (__be64 *)hash_state_out;

	for (i = 0; i < (len / 4); i++, hash512_in++)
		*hash512_in = cpu_to_be64(isha512_state->state[i]);

	for (i = 0; i < (len / 4); i++, hash512_out++)
		*hash512_out = cpu_to_be64(osha512_state->state[i]);
}

/* Need to precompute ipad/opad for hmac keys */
int ltq_hmac_precompute(u32 *inner, u32 *outer,
							struct crypto_shash *hash,
							unsigned int base_hash_ds,
							const u8 *key, unsigned int keylen,
							bool need_hash,
							void (*copykeys) (u32 *inner, u32 *outer,
									void *istate,
									void *ostate,
									unsigned int len))
{
	int err, i;
	int bs = crypto_shash_blocksize(hash);
	int ds = crypto_shash_digestsize(hash);
	int ss = crypto_shash_statesize(hash);
	struct {
		struct shash_desc shash;
		char ctx[crypto_shash_descsize(hash)];
	} desc;
	char opad[ss];
	char ipad[ss];

	desc.shash.tfm = hash;
	desc.shash.flags = crypto_shash_get_flags(hash) &
				CRYPTO_TFM_REQ_MAY_SLEEP;

	if (need_hash) {
		err = crypto_shash_digest(&desc.shash, key, keylen, ipad);
		if (err)
			return err;

		keylen = ds;
	} else {
		memcpy(ipad, key, keylen);
	}

	memset(ipad + keylen, 0, bs - keylen);
	memcpy(opad, ipad, bs);

	for (i = 0; i < bs; i++) {
		ipad[i] ^= 0x36;
		opad[i] ^= 0x5c;
	}

	err = crypto_shash_init(&desc.shash) ?:
		   crypto_shash_update(&desc.shash, ipad, bs) ?:
		   crypto_shash_export(&desc.shash, ipad) ?:
		   crypto_shash_init(&desc.shash) ?:
		   crypto_shash_update(&desc.shash, opad, bs) ?:
		   crypto_shash_export(&desc.shash, opad);
	if (!err)
		copykeys(inner, outer, ipad, opad, base_hash_ds);

	return err;
}
EXPORT_SYMBOL(ltq_hmac_precompute);

/* Need to calculate the ipad/opad from the hmac keys */
static int ltq_ahash_setkey(struct crypto_ahash *ahash, const u8 *key,
			  unsigned int keylen)
{
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(crypto_ahash_tfm(ahash));
	struct crypto_tfm *tfm = crypto_ahash_tfm(ahash);
	int bs = crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash));
	int ret;
	u32 hash_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AHASH);
	void (*func)(u32 *ipad, u32 *opad,
				void *istate,
				void *ostate,
				unsigned int len);

	if (!ctx->shash) {
		pr_err("Software hash was not initialized properly\n");
		return -EINVAL;
	}

	/* fallback keys just in case we need them for init/update/final */
	ret = crypto_ahash_setkey(ctx->fallback, key, keylen);
	if (ret)
		return ret;

	if (hash_flag == SAB_AUTH_HMAC_SHA1) {
		func = ltq_sha1_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_MD5) {
		func = ltq_md5_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_224) {
		func = ltq_sha224_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_256) {
		func = ltq_sha256_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_512 ||
						SAB_AUTH_HMAC_SHA2_384) {
		func = ltq_sha512_hmac_copy_keys;
	} else {
		pr_err("driver does not support this hmac extension\n");
		return -EINVAL;
	}

	ret = ltq_hmac_precompute(ctx->ipad, ctx->opad,
							ctx->shash, ctx->base_hash_ds,
							key, keylen,
							(keylen > bs) ? 1 : 0,
							func);
	if (ret) {
		pr_err("Error Precomputing ipad/opad\n");
		return ret;
	}

	ret = ltq_setup_ahash_assoc(tfm, ctx->ipad,
						ctx->opad, ctx->auth_key3);

	return ret;
}

/* We use a software hash to calculate the ipad/opad
 * Here we find the base hash algo and store it in
 * context
 */
struct crypto_shash *ltq_init_hmac(
					struct crypto_tfm *tfm,
					char *base_hash_name)
{
	struct crypto_shash *shash = NULL;
	int ret = 0;

	shash = crypto_alloc_shash(base_hash_name, 0,
				CRYPTO_ALG_NEED_FALLBACK);
	if (IS_ERR(shash)) {
		pr_err("Error initializing base hash for hmac\n");
		ret = PTR_ERR(shash);
		return NULL;
	}

	return shash;
}
EXPORT_SYMBOL(ltq_init_hmac);

static int cra_hash_init(struct crypto_tfm *tfm,
			const char *alg_name)
{
	int err;
	struct crypto_ahash *fallback;
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(tfm);

	fallback = crypto_alloc_ahash(alg_name, 0,
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_ASYNC);

	if (IS_ERR(fallback)) {
		pr_err("Cannot add fallback driver for driver %s\n", alg_name);
		err = PTR_ERR(ctx->fallback);
		goto out;
	}

	ctx->fallback = fallback;
	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				sizeof(struct ltq_ahash_req_ctx) +
				crypto_ahash_reqsize(ctx->fallback));

	return 0;
out:
	return err;
}

/* Init hmac transforms */
static int ltq_ahash_cra_init(struct crypto_tfm *tfm)
{
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_shash *shash = NULL;
	u32 hash_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AHASH);

	ctx->SAWords = 0;

	/* For hmac algos, we find the hash algo
	 * and for hash algos, we find fallback
	 * algos
	 */
	if (hash_flag == SAB_AUTH_HMAC_SHA1)
		shash = ltq_init_hmac(tfm, "sha1");
	else if (hash_flag == SAB_AUTH_HMAC_MD5)
		shash = ltq_init_hmac(tfm, "md5");
	else if (hash_flag == SAB_AUTH_HMAC_SHA2_224)
		shash = ltq_init_hmac(tfm, "sha224");
	else if (hash_flag == SAB_AUTH_HMAC_SHA2_256)
		shash = ltq_init_hmac(tfm, "sha256");
	else if (hash_flag == SAB_AUTH_HMAC_SHA2_384)
		shash = ltq_init_hmac(tfm, "sha384");
	else if (hash_flag == SAB_AUTH_HMAC_SHA2_512)
		shash = ltq_init_hmac(tfm, "sha512");
	else
		return cra_hash_init(tfm,
				crypto_tfm_alg_name(tfm)); /* normal hash */

	if (!shash)
		return -EINVAL;
	if (hash_flag == SAB_AUTH_HMAC_SHA2_224)
		ctx->base_hash_ds = SHA256_DIGEST_SIZE;
	else
		ctx->base_hash_ds = crypto_shash_digestsize(shash);

	ctx->shash = shash;
	ctx->config_sa_done = 0;
	ctx->registered = 0;

	return cra_hash_init(tfm,
				crypto_tfm_alg_name(tfm)); /* normal hash */;
}

static void ltq_ahash_cra_exit(struct crypto_tfm *tfm)
{
	struct ltq_ahash_ctx *ctx = crypto_tfm_ctx(tfm);
	int dev = 0;
	u32 *TCRDataOut = ctx->TCRDataOut;

	if (ctx->fallback)
		crypto_free_ahash(ctx->fallback);

	if (TCRDataOut)
		kfree(TCRDataOut);

	PEC_SA_UnRegister(dev, ctx->SAHandle,
					DMABuf_NULLHandle,
					DMABuf_NULLHandle);
}

static int ltq_ahash_init(struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);

	if (!ctx->fallback)
		return -EINVAL;

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
				CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_ahash_init(&rctx->req);
}

static int ltq_ahash_update(struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
			CRYPTO_TFM_REQ_MAY_SLEEP;
	rctx->req.nbytes = req->nbytes;
	rctx->req.src = req->src;

	return crypto_ahash_update(&rctx->req);
}

static int ltq_ahash_export(struct ahash_request *req, void *out)
{
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
			CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_ahash_export(&rctx->req, out);
}

static int ltq_ahash_import(struct ahash_request *req, const void *in)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
			CRYPTO_TFM_REQ_MAY_SLEEP;

	return crypto_ahash_import(&rctx->req, in);
}

static int ltq_ahash_final(struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
			CRYPTO_TFM_REQ_MAY_SLEEP;
	rctx->req.result = req->result;

	return crypto_ahash_final(&rctx->req);
}

static int ltq_ahash_finup(struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct ltq_ahash_req_ctx *rctx = ahash_request_ctx(req);

	ahash_request_set_tfm(&rctx->req, ctx->fallback);
	rctx->req.base.flags = req->base.flags &
			CRYPTO_TFM_REQ_MAY_SLEEP;
	rctx->req.nbytes = req->nbytes;
	rctx->req.src = req->src;
	rctx->req.result = req->result;

	return crypto_ahash_finup(&rctx->req);
}

static int ltq_ahash_digest(struct ahash_request *req)
{
	struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
	struct ltq_ahash_ctx *ctx = crypto_ahash_ctx(ahash);
	struct crypto_tfm *tfm = crypto_ahash_tfm(ahash);
	struct ltq_crypt_ctrl *ctrl;
	struct crypto_alg *alg = tfm->__crt_alg;
	struct scatterlist *src = req->src;
	u32 hash_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AHASH);
	unsigned int ds = crypto_ahash_digestsize(crypto_ahash_reqtfm
												(req));
	int ret;
	const u8 *zero_msg;

	if (req->nbytes == 0) {
		zero_msg = zero_msg_get(tfm);
		memcpy(req->result, zero_msg, ds);
		return 0;
	}

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ctrl->unaligned = 0;
	/* check for unaligned buffer in SG list */
	if (check_unaligned_buffer(req->src)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
								req->nbytes, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		src = ctrl->sg_cpy;
		ctrl->copy_bytes = req->nbytes;
		ctrl->unaligned = 1;
	}

	ctx->ds = ds;
	ctx->TokenFlags = 0;
	ctx->TokenFlags |= (TKB_PACKET_FLAG_HASHFIRST |
						TKB_PACKET_FLAG_HASHFINAL);
	ctrl->SA_flag = alg->cra_flags;
	ctrl->data.req = req;

	if ((hash_flag == SAB_AUTH_HASH_SHA2_256 ||
			hash_flag == SAB_AUTH_HASH_SHA2_224 ||
			hash_flag == SAB_AUTH_HASH_SHA1 ||
			hash_flag == SAB_AUTH_HASH_SHA2_512 ||
			hash_flag == SAB_AUTH_HASH_SHA2_384 ||
			hash_flag == SAB_AUTH_AES_XCBC_MAC ||
			hash_flag == SAB_AUTH_HASH_MD5)) {
		ret = ltq_setup_ahash_assoc(tfm, ctx->ipad,
									ctx->opad, ctx->auth_key3);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
	}

	return ltq_process_ahash(ctx, ctrl, req,
							src, EIP97_ENC_CALLBACK);
}

/* AEAD functions */
static int ltq_process_aead(struct aead_request *req,
					struct ltq_crypt_ctrl *ctrl,
					struct scatterlist *assoc_data,
					struct scatterlist *src_data,
					struct scatterlist *dst_data,
					int direction,
					u8 *iv,
					void (*callback)(void *data))
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct crypto_tfm *cryptfm = crypto_aead_tfm(tfm);
	int rc, count, sg_cnt;
	int dst_size, src_size, cur_ring;
	unsigned int authsize = crypto_aead_authsize(tfm);
	unsigned int cryptlen = req->cryptlen;
	unsigned int assoclen = req->assoclen;
	unsigned int TokenWords = 0;
	unsigned int TokenHeaderWord;
	unsigned int tries = 5;
	unsigned ivsize;
	u8 *src_buf, *dst_buf;
	u32 hash_alg = hash_mode_get(cryptfm, CRYPTO_ALG_TYPE_AEAD);
	void *TCRDataOut = (void *)ctx->TCRDataOut;

	DMABuf_Status_t DMAStatus;
	DMABuf_Properties_t DMAProperties = {0, 0, 0, 0};
	DMABuf_Handle_t SAHandle = ctx->SAHandle;
	DMABuf_Handle_t TokenHandle = {0};

	DMABuf_HostAddress_t TokenHostAddress;
	TokenBuilder_Params_t TokenParams;

	PEC_CommandDescriptor_t Cmd;
	PEC_NotifyFunction_t CBFunc;

	if (direction)
		cur_ring = EIP97_RING_DEC;
	else
		cur_ring = EIP97_RING_ENC;

	/* This value can be tweaked. From observation,
	 * performance of the driver drops at around
	 * this value
	 */
	if (atomic_read(&packets_queue) > 500) {
		kfree(ctrl);
		return -EBUSY;
	}

	/* we use MAX_AEAD_TKN_SIZE or else we need TokenBuilder_GetSize */
	dma_alloc_properties(&DMAProperties,
					true, (4 * MAX_AEAD_TKN_SIZE),
					LTQ_ALIGN,
					DA_PEC_BANK_PACKET);

	DMAStatus = DMABuf_Alloc(DMAProperties, &TokenHostAddress, &TokenHandle);
	if (DMAStatus != DMABUF_STATUS_OK) {
		pr_err("%s: Failed to allocate Token DMA buf\n", __func__);
		rc = -EAGAIN;
		goto aead_err;
	}

	/* for encrypt direction:
	 *		src_size = cryptlen
	 * for decrypt direction:
	 *		src_size = cryptlen + ivsize - authsize
	 */
	ivsize = crypto_aead_ivsize(tfm);
	if (direction) {
		/* decrypt dir */
		src_size = cryptlen + assoclen;
		src_buf = (u8 *)sg_virt(src_data) + assoclen;
		if (cryptlen - authsize <= 0)
			dst_size = cryptlen;
		else
			dst_size = assoclen + cryptlen - authsize;
		dst_buf = (u8 *)sg_virt(dst_data) + assoclen;
		ctrl->copybuf = NULL;
	} else {
		/* encrypt dir */
		src_size = cryptlen;
		/* the AAD data resides in the beginning of the packet */
		src_buf = (u8 *)sg_virt(src_data) + assoclen;
		/* Nasty workaround because of the HW working procedure */
		/* Affects SAB_AUTH_AES_CCM  && SAB_AUTH_AES_GMAC */
		if (hash_alg == SAB_AUTH_AES_CCM) {
			dst_size = cryptlen + ivsize + authsize;
			dst_buf = kmalloc(dst_size, GFP_KERNEL);
			if (!dst_buf)
				return -ENOMEM;
			ctrl->copybuf = dst_buf;
		} else if (hash_alg == SAB_AUTH_AES_GMAC) {
			dst_size = cryptlen + (ivsize * 2) + authsize;
			dst_buf = kmalloc(dst_size, GFP_KERNEL);
			ctrl->copybuf = dst_buf;
		} else {
			dst_size = assoclen + cryptlen + authsize;
			dst_buf = (u8 *)sg_virt(dst_data) + assoclen;
			ctrl->copybuf = NULL;
		}
	}

	pr_debug("debug: ivsize: %d, authsize: %d, dst_size: %d, src_size: %d\n",
					ivsize, authsize, dst_size, src_size);

	/* ASSOC | sg0 | sg 1 | ... | sg n */
	sg_cnt = count_sg(src_data);
	if ((sg_cnt == 1)) {
		ctrl->no_of_particles = 0;

		dma_alloc_properties(&DMAProperties,
					true, src_size,
					LTQ_ALIGN,
					DA_PEC_BANK_PACKET);

		DMABuf_Register(DMAProperties, src_buf,
							NULL, 'k', &ctrl->SrcPktHandle);
		if (DMAStatus != DMABUF_STATUS_OK) {
			pr_err("%s: Failed to register Src DMA buf\n", __func__);
			rc = -EAGAIN;
			goto aead_err;
		}

		dma_alloc_properties(&DMAProperties,
							true, dst_size,
							LTQ_ALIGN,
							DA_PEC_BANK_PACKET);

		DMABuf_Register(DMAProperties, dst_buf,
						NULL, 'k', &ctrl->DstPktHandle);
		if (DMAStatus != DMABUF_STATUS_OK) {
			pr_err("%s: Failed to register dst DMA buf\n", __func__);
			rc = -EAGAIN;
			goto aead_err;
		}
	} else {
		rc = ltq_buffers_chaining(ctrl, src_data,
								&ctrl->SrcPktHandle,
								ctrl->SrcScatter_Handle,
								src_size,
								sg_cnt);
		if (rc) {
			pr_err("Error allocating sg buffers\n");
			rc = -ENOMEM;
			goto aead_err;
		}

		rc = ltq_buffers_chaining(ctrl, dst_data,
							&ctrl->DstPktHandle,
							ctrl->DstGather_Handle,
							dst_size,
							sg_cnt);
		if (rc) {
			pr_err("Error allocating sg buffers\n");
			rc = -ENOMEM;
			goto aead_err;
		}
	}

	ctrl->TokenHandle = TokenHandle;
	TokenParams.PacketFlags = ctx->TokenFlags;
	TokenParams.PadByte = 0;
	TokenParams.BypassByteCount = 0;

	/* AEAD algos have IV in SA, hence IV is automatically
	 * calculated in the ICV tags
	 */
	TokenParams.IV_p = iv;
	if (hash_alg == SAB_AUTH_AES_GCM ||
		(hash_alg == SAB_AUTH_AES_CCM)) {
		/* The assoc data is just the assoc data minus IV,
		 * IV is the SA buffer
		 */
		assoclen = assoclen - 8;
		if (assoclen > 0) {
			TokenParams.AAD_p = sg_virt(assoc_data);
			TokenParams.AdditionalValue = assoclen;
		}
	} else {
		/* assoc data is AAD | IV */
		TokenParams.AAD_p = sg_virt(assoc_data);
		TokenParams.AdditionalValue = assoclen;
	}

	rc = TokenBuilder_BuildToken(TCRDataOut,
						(uint8_t *)ctrl->SrcPktHandle.p,
						src_size,
						&TokenParams,
						(uint32_t *)TokenHostAddress.p,
						&TokenWords,
						&TokenHeaderWord);
	if (rc != TKB_STATUS_OK) {
		if (rc == TKB_BAD_PACKET)
			pr_err("Token not created because packet size is invalid\n");
		else
			pr_err("Token builder failed\n");

		rc = -EINVAL;
		goto aead_err;
	}

#ifdef LTQ_CRYPTO_DEBUG
	{
		int i;
		u32 *tmp = (u32 *)TokenHostAddress.p;
		pr_info("AEAD: TokenHeaderWord: %08x\n", TokenHeaderWord);
		for (i = 0; i < TokenWords; i++)
			pr_info("Token[%d]: %08x\n", i, tmp[i]);
	}
#endif

	ctrl->TokenParams = TokenParams;
	Cmd.User_p = ctrl;
	Cmd.Token_Handle = TokenHandle;
	Cmd.Token_WordCount = TokenWords;
	Cmd.SrcPkt_Handle = ctrl->SrcPktHandle;
	Cmd.SrcPkt_ByteCount = cryptlen;
	Cmd.DstPkt_Handle = ctrl->DstPktHandle;
	Cmd.Bypass_WordCount = 0;
	Cmd.SA_Handle1 = SAHandle;
	Cmd.SA_Handle2 = DMABuf_NULLHandle;
	Cmd.SA_WordCount = 0;
	Cmd.Control1 = TokenHeaderWord;
	Cmd.Control2 = 0;

	rc = PEC_Packet_Put(cur_ring,
						&Cmd, 1, &count);
	if (rc == PEC_STATUS_BUSY) {
		while (tries > 0) {
			udelay(300);
			rc = PEC_Packet_Put(cur_ring,
						&Cmd, 1, &count);
			if (rc == PEC_STATUS_OK)
				break;

			tries--;
		}
	}
	if ((rc != PEC_STATUS_OK) || (count != 1)) {
		pr_debug("Packet Put error\n");
		rc = -EAGAIN;
		goto aead_err;
	}

	atomic_inc(&packets_queue);
	return -EINPROGRESS;

aead_err:
	free_resources(ctrl, SAHandle, cur_ring);
	return rc;
}

static int ltq_aead_setkey(struct crypto_aead *authenc, const u8 *key,
				  unsigned int keylen)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct crypto_tfm *tfm = crypto_aead_tfm(authenc);
	int bs = ctx->blksize;
	int ret;
	u32 hash_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AEAD);
	struct crypto_authenc_keys keys;
	void (*func)(u32 *ipad, u32 *opad,
				void *istate,
				void *ostate,
				unsigned int len);

	if (crypto_authenc_extractkeys(&keys, key, keylen) != 0)
		goto badkey;

	if (keys.authkeylen > sizeof(ctx->authkey))
		goto badkey;

	if (keys.enckeylen > sizeof(ctx->enckey))
		goto badkey;

	memcpy(ctx->authkey, keys.authkey, keys.authkeylen);
	memcpy(ctx->enckey, keys.enckey, keys.enckeylen);

	ctx->enckeylen = keys.enckeylen;
	ctx->authkeylen = keys.authkeylen;
	ctx->config_sa_done = 0;

	if (hash_flag == SAB_AUTH_HMAC_SHA1) {
		func = ltq_sha1_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_MD5) {
		func = ltq_md5_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_224) {
		func = ltq_sha224_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_256) {
		func = ltq_sha256_hmac_copy_keys;
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_512 ||
						SAB_AUTH_HMAC_SHA2_384) {
		func = ltq_sha512_hmac_copy_keys;
	} else {
		pr_err("driver does not support this hmac extension\n");
		return -EINVAL;
	}

	ret = ltq_hmac_precompute(ctx->ipad, ctx->opad,
							ctx->shash, ctx->base_hash_ds,
							ctx->authkey,
							keys.authkeylen,
							(keys.authkeylen > bs) ? 1 : 0,
							func);
	if (ret)
		goto badkey;

	return 0;

badkey:
	ctx->enckeylen = 0;
	crypto_aead_set_flags(authenc, CRYPTO_TFM_RES_BAD_KEY_LEN);
	return -EINVAL;
}

static int ltq_aead_rfc4309_setkey(struct crypto_aead *authenc, const u8 *key,
							 unsigned int keylen)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);

	if (keylen <= 3)
		return -EINVAL;

	memset(ctx->nonce, 0, 8);

	keylen -= 3;
	ctx->enckeylen = keylen;
	memcpy(ctx->enckey, key, keylen);
	memcpy(ctx->nonce, key + keylen, 3);

	/* prepare fallback keys */
	if (ctx->aead_fb) {
		ctx->aead_fb->base.crt_flags |= CRYPTO_TFM_REQ_MASK;
		crypto_aead_setkey(ctx->aead_fb, key, keylen + 3);
	}

	ctx->config_sa_done = 0;
	return 0;
}

int ltq_gcm_precompute_keys(u8 *out, const u8 *key, unsigned int keylen)
{
	int i, err = 0;
	u8 buf[AES_BLOCK_SIZE];

	memset(buf, 0 , AES_BLOCK_SIZE);
	err = software_aes_precompute("ctr(aes)", buf, buf, key, keylen);
	if (err) {
		pr_err("Error pre-processing keys for GCM\n");
		err = -EINVAL;
		goto out;
	}

	/* expected H should be swapped */
	for (i = 0; i < 4; i++)
		*((u32 *)out + i) = endian_swap(*((u32 *)buf + i));

	err = 0;
out:
	return err;
}
EXPORT_SYMBOL(ltq_gcm_precompute_keys);

static int ltq_aead_rfc4xxx_gcm_setkey(struct crypto_aead *authenc, const u8 *key,
							 unsigned int keylen)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	int err;

	if (keylen < 4)
		return -EINVAL;

	keylen -= 4;

	ctx->enckeylen = keylen;
	memcpy(ctx->enckey, key, keylen);
	memcpy(ctx->nonce, key + keylen, 4);

	err = ltq_gcm_precompute_keys((u8 *)ctx->ipad, key, keylen);
	if (err) {
		pr_err("Error calculating the precompute keys for GCM\n");
		return err;
	}

	/* if setkey in invoked, most likely the ctx needs a refresh */
	ctx->config_sa_done = 0;

	return 0;
}

static int ltq_aead_cra_init(struct crypto_aead *aead)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_tfm *tfm = crypto_aead_tfm(aead);
	struct crypto_shash *shash = NULL;
	u32 hash_flag = hash_mode_get(tfm, CRYPTO_ALG_TYPE_AEAD);
	const char *fallback_name = tfm->__crt_alg->cra_name;

	if (hash_flag == SAB_AUTH_HMAC_SHA1) {
		shash = ltq_init_hmac(tfm, "sha1");
	} else if (hash_flag == SAB_AUTH_HMAC_MD5) {
		shash = ltq_init_hmac(tfm, "md5");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_224) {
		shash = ltq_init_hmac(tfm, "sha224");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_256) {
		shash = ltq_init_hmac(tfm, "sha256");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_384) {
		shash = ltq_init_hmac(tfm, "sha384");
	} else if (hash_flag == SAB_AUTH_HMAC_SHA2_512) {
		shash = ltq_init_hmac(tfm, "sha512");
	} else if (hash_flag == SAB_AUTH_AES_CCM) {
		ctx->aead_fb = crypto_alloc_aead(fallback_name, 0,
									CRYPTO_ALG_ASYNC |
									CRYPTO_ALG_NEED_FALLBACK);
		if (IS_ERR(ctx->aead_fb)) {
			pr_err("Error allocating %s fallback\n", fallback_name);
			return -EINVAL;
		}

		return 0;
	} else {
		return 0; /* non-HMAC case */
	}

	if (!shash)
		return -EINVAL;
	if (hash_flag == SAB_AUTH_HMAC_SHA2_224)
		ctx->base_hash_ds = SHA256_DIGEST_SIZE;
	else
		ctx->base_hash_ds = crypto_shash_digestsize(shash);

	ctx->shash = shash;
	ctx->blksize = crypto_shash_blocksize(shash);
	ctx->registered = 0;
	ctx->config_sa_done = 0;
	return 0;
}

static void ltq_aead_cra_exit(struct crypto_aead *aead)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(aead);
	int ring = ctx->direction;
	u32 *TCRDataOut = ctx->TCRDataOut;

	if (TCRDataOut)
		kfree(TCRDataOut);

	PEC_SA_UnRegister(ring, ctx->SAHandle,
					DMABuf_NULLHandle,
					DMABuf_NULLHandle);

	DMABuf_Release(ctx->SAHandle);
}

static int ltq_aead_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src;
	struct scatterlist *dst;
	struct scatterlist *assoc = req->src;
	int ret;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	/* unaligned buffer have to be copied to one that is aligned
	 * for tcrypt test cases, this helps to pass the unaligned
	 * buffer test cases
	 */
	if (check_unaligned_buffer(req->src) ||
			(count_sg(req->src) > 0)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
								req->cryptlen + req->assoclen, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->copy_bytes = req->cryptlen + req->assoclen;
		ctrl->unaligned = 1;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if (!ctx->config_sa_done) {
		ret = ltq_setup_aead_assoc(authenc, ctrl, ctx,
					LTQ_ENC, ctx->enckeylen,
					ctx->enckey, req->iv,
					ctx->ipad, ctx->opad);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		ctx->config_sa_done = 1;
	}

	ctx->TokenFlags = TKB_PACKET_FLAG_HASHAPPEND;
	ctrl->dir = LTQ_ENC;
	ctrl->data.aead_req = req;
	ctrl->SA_flag = ctx->SA_flag;
	return ltq_process_aead(req, ctrl, assoc, src,
					dst, LTQ_ENC, req->iv,
					EIP97_ENC_CALLBACK);
}

static int ltq_aead_decrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src;
	struct scatterlist *dst;
	struct scatterlist *assoc = req->src;
	int ret;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src) ||
			(count_sg(req->src) > 0)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
					req->cryptlen + req->assoclen, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->unaligned = 1;
		ctrl->copy_bytes = req->cryptlen + req->assoclen;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if (!ctx->config_sa_done) {
		ret = ltq_setup_aead_assoc(authenc, ctrl, ctx,
				LTQ_DEC, ctx->enckeylen,
				ctx->enckey, req->iv,
				ctx->ipad, ctx->opad);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		ctx->TokenFlags = 0;
		ctx->config_sa_done = 1;
	}

	ctrl->dir = LTQ_DEC;
	ctrl->data.aead_req = req;
	ctrl->SA_flag = ctx->SA_flag;
	return ltq_process_aead(req, ctrl, assoc, src,
					dst, LTQ_DEC, req->iv,
					EIP97_DEC_CALLBACK);
}

static int ltq_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);
	ctx->authsize = authsize;
	return 0;
}

static int ltq_aead_gcm_rfc4543_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);

	switch (authsize) {
	case 16:
		break;
	default:
		return -EINVAL;
	}

	ctx->authsize = authsize;
	return 0;
}

static int ltq_aead_gcm_rfc4106_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);

	switch (authsize) {
	case 8:
	case 12:
	case 16:
		break;
	default:
		return -EINVAL;
	}

	ctx->authsize = authsize;
	return 0;
}

static int ltq_aead_gcm_transform(struct aead_request *req, int direction)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct ltq_crypt_ctrl *ctrl;
	void (*callback)(void *data);
	struct scatterlist *src;
	struct scatterlist *dst;
	struct scatterlist *assoc = req->src;
	int ret;
	u8 *iv = ctx->iv;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src) ||
			(count_sg(req->src) > 0)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
								req->cryptlen + req->assoclen, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->unaligned = 1;
		ctrl->copy_bytes = req->cryptlen + req->assoclen;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if (!ctx->config_sa_done) {
		ret = ltq_setup_aead_assoc(authenc, ctrl, ctx,
								direction, ctx->enckeylen,
								ctx->enckey, iv,
								ctx->ipad, ctx->opad);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		ctx->TokenFlags = 0;
		ctx->config_sa_done = 1;
	}

	if (direction)
		callback = EIP97_DEC_CALLBACK;
	else
		callback = EIP97_ENC_CALLBACK;

	ctrl->dir = direction;
	ctrl->data.aead_req = req;
	ctrl->SA_flag = ctx->SA_flag;
	return ltq_process_aead(req, ctrl, assoc, src,
							dst, direction, iv,
							callback);

}


static int ltq_gcm_rfc4xxx_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	int ctx_mem_dir_to_ring_no;
	__be32 counter = cpu_to_be32(1);

	u8 *iv = ctx->iv;
	memcpy(iv, ctx->nonce, 4);
	memcpy(iv + 4, req->iv, 8);
	*((__be32 *)(iv + 12)) = counter;

	/* Tcrypt workaround */
	if (ctx->config_sa_done == 0) {
		if (ctx->TCRDataOut) {
			kfree(ctx->TCRDataOut);

			ctx_mem_dir_to_ring_no = ctx->params.direction;
			ctx->registered = 0;

			PEC_SA_UnRegister(ctx_mem_dir_to_ring_no,
						ctx->SAHandle,
						DMABuf_NULLHandle,
						DMABuf_NULLHandle);

			DMABuf_Release(ctx->SAHandle);
		}
	}

	return ltq_aead_gcm_transform(req, LTQ_ENC);
}

static int ltq_gcm_rfc4xxx_decrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	int ctx_mem_dir_to_ring_no;
	__be32 counter = cpu_to_be32(1);

	u8 *iv = ctx->iv;
	memcpy(iv, ctx->nonce, 4);
	memcpy(iv + 4, req->iv, 8);
	*((__be32 *)(iv + 12)) = counter;

	/* Tcrypt workaround */
	if (ctx->config_sa_done == 0) {
		if (ctx->TCRDataOut) {
			kfree(ctx->TCRDataOut);

			ctx_mem_dir_to_ring_no = ctx->params.direction;
			ctx->registered = 0;

			PEC_SA_UnRegister(ctx_mem_dir_to_ring_no,
						ctx->SAHandle,
						DMABuf_NULLHandle,
						DMABuf_NULLHandle);

			DMABuf_Release(ctx->SAHandle);
		}
	}

	return ltq_aead_gcm_transform(req, LTQ_DEC);
}

static int do_ccm_fallback(struct aead_request *req,  struct ltq_aead_ctx *ctx,
						int direction)
{
	struct crypto_tfm *old_tfm = crypto_aead_tfm(crypto_aead_reqtfm(req));
	int ret = 0;

	if (ctx->aead_fb) {
		aead_request_set_tfm(req, ctx->aead_fb);
		if (direction)
			ret = crypto_aead_decrypt(req);
		else
			ret = crypto_aead_encrypt(req);

		 aead_request_set_tfm(req, __crypto_aead_cast(old_tfm));
	} else {
		ret = -EINVAL;
	}

	return ret;
}

static int ltq_aead_rfc4309_ccm_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src;
	struct scatterlist *dst;
	struct scatterlist *assoc = req->src;
	u8 *iv = ctx->iv;
	int ret;

	if (req->cryptlen == 0)
		return do_ccm_fallback(req, ctx, 0);

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	/* construction of the flag */
	iv[0] = 3;
	memcpy(iv + 1, ctx->nonce, 3);
	memcpy(iv + 4, req->iv, 8);
	memset(iv + 15 - iv[0], 0, iv[0] + 1);

	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src) ||
			(count_sg(req->src) > 0)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
								req->cryptlen + req->assoclen, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->unaligned = 1;
		ctrl->copy_bytes = req->cryptlen + req->assoclen;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if (!ctx->config_sa_done) {
		/* Tcrypt workaround */
		if (ctx->TCRDataOut) {
			kfree(ctx->TCRDataOut);
			ctx->registered = 0;

			PEC_SA_UnRegister(EIP97_RING_ENC,
							ctx->SAHandle,
							DMABuf_NULLHandle,
							DMABuf_NULLHandle);

			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_aead_assoc(authenc, ctrl, ctx,
								LTQ_ENC, ctx->enckeylen,
								ctx->enckey, req->iv,
								ctx->ipad, ctx->opad);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		ctx->TokenFlags = 0;
		ctx->config_sa_done = 1;
	}

	ctx->TokenFlags = TKB_PACKET_FLAG_HASHAPPEND;
	ctrl->dir = LTQ_ENC;
	ctrl->data.aead_req = req;
	ctrl->SA_flag = ctx->SA_flag;
	return ltq_process_aead(req, ctrl, assoc, src,
							dst, LTQ_ENC, iv,
							EIP97_ENC_CALLBACK);
}

static int ltq_aead_rfc4309_ccm_decrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(authenc);
	struct ltq_crypt_ctrl *ctrl;
	struct scatterlist *src;
	struct scatterlist *dst;
	struct scatterlist *assoc = req->src;
	u8 *iv = ctx->iv;
	int ret;

	ctrl = (struct ltq_crypt_ctrl *)kmalloc(sizeof(struct ltq_crypt_ctrl),
										GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	/* construction of the flags */
	iv[0] = 3;
	memcpy(iv + 1, ctx->nonce, 3);
	memcpy(iv + 4, req->iv, 8);
	memset(iv + 15 - iv[0], 0, iv[0] + 1);

	/* unaligned buffer have to be copied to one that is aligned */
	if (check_unaligned_buffer(req->src) ||
			(count_sg(req->src) > 0)) {
		ret = ltq_unaligned_cpy(req->src, &ctrl->sg_cpy,
							req->cryptlen + req->assoclen, 0);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		/* In-place transform */
		src = ctrl->sg_cpy;
		dst = ctrl->sg_cpy;
		ctrl->unaligned = 1;
		ctrl->copy_bytes = req->cryptlen + req->assoclen;
	} else {
		src = req->src;
		dst = req->dst;
		ctrl->unaligned = 0;
	}

	if (!ctx->config_sa_done) {
		/* Tcrypt workaround */
		if (ctx->TCRDataOut) {
			kfree(ctx->TCRDataOut);

			ctx->registered = 0;
			PEC_SA_UnRegister(EIP97_RING_DEC,
							ctx->SAHandle,
							DMABuf_NULLHandle,
							DMABuf_NULLHandle);

			DMABuf_Release(ctx->SAHandle);
		}

		ret = ltq_setup_aead_assoc(authenc, ctrl, ctx,
								LTQ_DEC, ctx->enckeylen,
								ctx->enckey, req->iv,
								ctx->ipad, ctx->opad);
		if (ret) {
			kfree(ctrl);
			return ret;
		}
		ctx->TokenFlags = 0;
		ctx->config_sa_done = 1;
	}

	ctrl->dir = LTQ_DEC;
	ctrl->data.aead_req = req;
	ctrl->SA_flag = ctx->SA_flag;
	return ltq_process_aead(req, ctrl, assoc, src,
							dst, LTQ_DEC, iv,
							EIP97_DEC_CALLBACK);
}

static int ltq_aead_rfc4309_ccm_setauthsize(struct crypto_aead *tfm,
						unsigned int authsize)
{
	struct ltq_aead_ctx *ctx = crypto_aead_ctx(tfm);

	switch (authsize) {
	case 8:
	case 12:
	case 16:
		break;
	default:
		return -EINVAL;
	}

	ctx->authsize = authsize;
	return 0;
}


static int ltq_crypto_device_initialize(struct platform_device *pdev)
{
	/* put all hardware initialization code here */
	/* clk, power, etc */
	struct clk *clk;

	clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "failed to get clk\n");
		return PTR_ERR(clk);
	}

	return clk_prepare_enable(clk);
}

/* zero msg disgests which are not supported by hw */
static const u8 md5_zero[MD5_DIGEST_SIZE] = {
	0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
	0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e,
};
static const u8 sha1_zero[SHA1_DIGEST_SIZE] = {
	0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32,
	0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8,
	0x07, 0x09
};

static const char sha224_zero[SHA224_DIGEST_SIZE] = {
	0xd1, 0x4a, 0x02, 0x8c, 0x2a, 0x3a, 0x2b, 0xc9, 0x47,
	0x61, 0x02, 0xbb, 0x28, 0x82, 0x34, 0xc4, 0x15, 0xa2,
	0xb0, 0x1f, 0x82, 0x8e, 0xa6, 0x2a, 0xc5, 0xb3, 0xe4,
	0x2f
};

static const u8 sha256_zero[SHA256_DIGEST_SIZE] = {
	0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a,
	0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae,
	0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99,
	0x1b, 0x78, 0x52, 0xb8, 0x55
};

static const u8 xcbc_aes_zero[AES_BLOCK_SIZE] = {
	0x75, 0xf0, 0x25, 0x1d, 0x52, 0x8a, 0xc0, 0x1c,
	0x45, 0x73, 0xdf, 0xd5, 0x84, 0xd7, 0x9f, 0x29
};

static const u8 sha512_zero[SHA512_DIGEST_SIZE] = {
	0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd,
	0xf1, 0x54, 0x28, 0x50, 0xd6, 0x6d, 0x80, 0x07,
	0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
	0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce,
	0x47, 0xd0, 0xd1, 0x3c, 0x5d, 0x85, 0xf2, 0xb0,
	0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
	0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81,
	0xa5, 0x38, 0x32, 0x7a, 0xf9, 0x27, 0xda, 0x3e
};

static struct ltq_crypto_template ltq_crypto_alg[] = {
	/* ABLKCIPHER */
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "arc4",
			.cra_driver_name = "arc4-ltq-crypto",
			.cra_blocksize = ARC4_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = ARC4_MIN_KEY_SIZE,
				.max_keysize = ARC4_MAX_KEY_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_ARCFOUR,
		.crypto_mode = SAB_CRYPTO_MODE_STATELESS,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "ecb(arc4)",
			.cra_driver_name = "ecb-arc4-ltq-crypto",
			.cra_blocksize = ARC4_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = ARC4_MIN_KEY_SIZE,
				.max_keysize = ARC4_MAX_KEY_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_ARCFOUR,
		.crypto_mode = SAB_CRYPTO_MODE_STATELESS | SAB_CRYPTO_MODE_ECB,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "cbc(aes)",
			.cra_driver_name = "cbc-aes-ltq-crypto",
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = AES_MIN_KEY_SIZE,
				.max_keysize = AES_MAX_KEY_SIZE,
				.ivsize = AES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "ofb(aes)",
			.cra_driver_name = "ofb-aes-ltq-crypto",
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = AES_MIN_KEY_SIZE,
				.max_keysize = AES_MAX_KEY_SIZE,
				.ivsize = AES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_OFB,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "rfc3686(ctr(aes))",
			.cra_driver_name = "rfc36868-ctr-aes-ltq-crypto",
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = AES_MIN_KEY_SIZE,
				.max_keysize = AES_MAX_KEY_SIZE,
				.ivsize = AES_BLOCK_SIZE,
				.setkey = ltq_rfc3686_setkey,
				.encrypt = ltq_rfc3686_encrypt,
				.decrypt = ltq_rfc3686_decrypt,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CTR,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "cbc(des3_ede)",
			.cra_driver_name = "cbc-des3_ede-ltq-crypto",
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = DES3_EDE_KEY_SIZE,
				.max_keysize = DES3_EDE_KEY_SIZE,
				.ivsize = DES3_EDE_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "ecb(des3_ede)",
			.cra_driver_name = "ecb-des3_ede-ltq-crypto",
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = DES3_EDE_KEY_SIZE,
				.max_keysize = DES3_EDE_KEY_SIZE,
				.ivsize = DES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_ECB,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "ofb(des3_ede)",
			.cra_driver_name = "ofb-des3_ede-ltq-crypto",
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = DES3_EDE_KEY_SIZE,
				.max_keysize = DES3_EDE_KEY_SIZE,
				.ivsize = DES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_OFB,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "ecb(des)",
			.cra_driver_name = "ecb-des-ltq-crypto",
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = DES_KEY_SIZE,
				.max_keysize = DES_KEY_SIZE,
				.ivsize = DES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_DES,
		.crypto_mode = SAB_CRYPTO_MODE_ECB,
	},
	{	.type = CRYPTO_ALG_TYPE_ABLKCIPHER,
		.alg.crypto = {
			.cra_name = "cbc(des)",
			.cra_driver_name = "cbc-des-ltq-crypto",
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
							CRYPTO_ALG_ASYNC,
			.cra_type		 = &crypto_ablkcipher_type,
			.cra_priority	 = 300,
			.cra_module		 = THIS_MODULE,
			.cra_ablkcipher = {
				.min_keysize = DES_KEY_SIZE,
				.max_keysize = DES_KEY_SIZE,
				.ivsize = DES_BLOCK_SIZE,
			}
		},
		.auth_flag = SAB_AUTH_NULL,
		.crypto_flag = SAB_CRYPTO_DES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	/* AHASH */
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.digestsize = SHA1_DIGEST_SIZE,
			.halg.statesize = sizeof(struct sha1_state),
			.halg.base = {
				.cra_name = "sha1",
				.cra_driver_name = "sha1-ltq-crypto",
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
							 CRYPTO_ALG_ASYNC,
				.cra_priority	 = 300,
				.cra_module		 = THIS_MODULE,
			}
		},
		.auth_flag = SAB_AUTH_HASH_SHA1,
		.crypto_flag = SAB_CRYPTO_NULL,
		.zero_msg = sha1_zero,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.digestsize = SHA224_DIGEST_SIZE,
			.halg.statesize = sizeof(struct sha256_state),
			.halg.base = {
				.cra_name = "sha224",
				.cra_driver_name = "sha224-ltq-crypto",
				.cra_blocksize = SHA224_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
							 CRYPTO_ALG_ASYNC,
				.cra_priority	 = 300,
				.cra_module		 = THIS_MODULE,
			}
		},
		.auth_flag = SAB_AUTH_HASH_SHA2_224,
		.crypto_flag = SAB_CRYPTO_NULL,
		.zero_msg = sha224_zero,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.digestsize = SHA256_DIGEST_SIZE,
			.halg.statesize = sizeof(struct sha256_state),
			.halg.base = {
				.cra_name = "sha256",
				.cra_driver_name = "sha256-ltq-crypto",
				.cra_blocksize = SHA256_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
							 CRYPTO_ALG_ASYNC,
				.cra_priority	 = 300,
				.cra_module		 = THIS_MODULE,
			}
		},
		.auth_flag = SAB_AUTH_HASH_SHA2_256,
		.crypto_flag = SAB_CRYPTO_NULL,
		.zero_msg = sha256_zero,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.digestsize = SHA512_DIGEST_SIZE,
			.halg.statesize = sizeof(struct sha512_state),
			.halg.base = {
				.cra_name = "sha512",
				.cra_driver_name = "sha512-ltq-crypto",
				.cra_blocksize = SHA512_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
							 CRYPTO_ALG_ASYNC,
				.cra_priority	 = 300,
				.cra_module		 = THIS_MODULE,
			}
		},
		.auth_flag = SAB_AUTH_HASH_SHA2_512,
		.crypto_flag = SAB_CRYPTO_NULL,
		.zero_msg = sha512_zero,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.digestsize = MD5_DIGEST_SIZE,
			.halg.statesize = sizeof(struct md5_state),
			.halg.base = {
				.cra_name = "md5",
				.cra_driver_name = "md5-ltq-crypto",
				.cra_blocksize = MD5_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
				.cra_priority	 = 300,
				.cra_module		 = THIS_MODULE,
			}
		},
		.auth_flag = SAB_AUTH_HASH_MD5,
		.crypto_flag = SAB_CRYPTO_NULL,
		.zero_msg = md5_zero,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct sha1_state),
			.halg.digestsize = SHA1_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(sha1)",
				.cra_driver_name = "sha1-hmac-ltq-crypto",
				.cra_blocksize = SHA1_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_SHA1,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct sha256_state),
			.halg.digestsize = SHA224_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(sha224)",
				.cra_driver_name = "sha224-hmac-ltq-crypto",
				.cra_blocksize = SHA224_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_224,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct sha256_state),
			.halg.digestsize = SHA256_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(sha256)",
				.cra_driver_name = "sha256-hmac-ltq-crypto",
				.cra_blocksize = SHA256_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_256,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct sha512_state),
			.halg.digestsize = SHA384_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(sha384)",
				.cra_driver_name = "sha384-hmac-ltq-crypto",
				.cra_blocksize = SHA384_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_384,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct sha512_state),
			.halg.digestsize = SHA512_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(sha512)",
				.cra_driver_name = "sha512-hmac-ltq-crypto",
				.cra_blocksize = SHA512_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_512,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.halg.statesize = sizeof(struct md5_state),
			.halg.digestsize = MD5_DIGEST_SIZE,
			.halg.base = {
				.cra_name = "hmac(md5)",
				.cra_driver_name = "md5-hmac-ltq-crypto",
				.cra_blocksize = MD5_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_HMAC_MD5,
		.crypto_flag = SAB_CRYPTO_NULL,
	},
	{	.type = CRYPTO_ALG_TYPE_AHASH,
		.alg.hash = {
			.setkey = ltq_xcbc_ahash_setkey,
			.halg.statesize = PAGE_SIZE / 8,
			.halg.digestsize = AES_BLOCK_SIZE,
			.halg.base = {
				.cra_name = "xcbc(aes)",
				.cra_driver_name = "xcbc-aes-ltq_crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AHASH |
						 CRYPTO_ALG_ASYNC,
			}
		},
		.auth_flag = SAB_AUTH_AES_XCBC_MAC,
		.crypto_flag = SAB_CRYPTO_NULL,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
		.zero_msg = xcbc_aes_zero,
	},
	/* AUTHENC */
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(sha1),cbc(aes))",
				.cra_driver_name = "authenc-hmac-sha1-cbc-aes-ltq-crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_SHA1,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(sha256),cbc(aes))",
				.cra_driver_name = "authenc-hmac-sha256-cbc-aes-ltq-crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_256,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(sha384),cbc(aes))",
				.cra_driver_name = "authenc-hmac-sha384-cbc-aes-ltq-crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA384_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_384,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(sha512),cbc(aes))",
				.cra_driver_name = "authenc-hmac-sha512-cbc-aes-ltq-crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA512_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_512,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(sha256),cbc(des3_ede))",
				.cra_driver_name = "authenc-hmac-sha256-des3_ede-ltq-crypto",
				.cra_blocksize = DES3_EDE_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_SHA2_256,
		.crypto_flag = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(md5),cbc(aes))",
				.cra_driver_name = "authenc-hmac-md5-cbc-aes-ltq-crypto",
				.cra_blocksize = AES_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = DES_BLOCK_SIZE,
			.maxauthsize = MD5_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_MD5,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{	.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "authenc(hmac(md5),cbc(des3_ede))",
				.cra_driver_name = "authenc-hmac-md5-cbc-3des-ltq-crypto",
				.cra_blocksize = DES3_EDE_BLOCK_SIZE,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = MD5_DIGEST_SIZE,
		},
		.auth_flag = SAB_AUTH_HMAC_MD5,
		.crypto_flag = SAB_CRYPTO_3DES,
		.crypto_mode = SAB_CRYPTO_MODE_CBC,
	},
	{
		.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "rfc4106(gcm(aes))",
				.cra_driver_name = "aead-rfc4106-aes-gcm-ltq-crypto",
				.cra_blocksize = 1,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = 8,
			.maxauthsize = AES_BLOCK_SIZE,
			.setkey = ltq_aead_rfc4xxx_gcm_setkey,
			.setauthsize = ltq_aead_gcm_rfc4106_setauthsize,
			.encrypt = ltq_gcm_rfc4xxx_encrypt,
			.decrypt = ltq_gcm_rfc4xxx_decrypt,
		},
		.auth_flag = SAB_AUTH_AES_GCM,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_GCM,
	},
	{
		.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "rfc4543(gcm(aes))",
				.cra_driver_name = "aead-rfc4543-aes-gcm-ltq-crypto",
				.cra_blocksize = 1,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC,
			},
			.ivsize = 8,
			.maxauthsize = AES_BLOCK_SIZE,
			.setkey = ltq_aead_rfc4xxx_gcm_setkey, /* share with GCM */
			.setauthsize = ltq_aead_gcm_rfc4543_setauthsize,
			.encrypt = ltq_gcm_rfc4xxx_encrypt,
			.decrypt = ltq_gcm_rfc4xxx_decrypt,
		},
		.auth_flag = SAB_AUTH_AES_GMAC,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_GMAC,
	},
	{
		.type = CRYPTO_ALG_TYPE_AEAD,
		.alg.aead = {
			.base = {
				.cra_name = "rfc4309(ccm(aes))",
				.cra_driver_name = "aead-rfc4309-aes-ccm-ltq-crypto",
				.cra_blocksize = 1,
				.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC |
								CRYPTO_ALG_NEED_FALLBACK,
			},
			.ivsize = 8,
			.maxauthsize = AES_BLOCK_SIZE,
			.setkey = ltq_aead_rfc4309_setkey,
			.setauthsize = ltq_aead_rfc4309_ccm_setauthsize,
			.encrypt = ltq_aead_rfc4309_ccm_encrypt,
			.decrypt = ltq_aead_rfc4309_ccm_decrypt,
		},
		.auth_flag = SAB_AUTH_AES_CCM,
		.crypto_flag = SAB_CRYPTO_AES,
		.crypto_mode = SAB_CRYPTO_MODE_CCM,
	}
};

static int __init ltq_crypto_init(struct platform_device *pdev)
{
	struct resource *res;
	PEC_InitBlock_t InitBlock = {0, 0};
	int i, err;
#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT
	int irq;
#endif


	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		pr_err("Cannot get EIP97 resource\n");
		return -ENOMEM;
	}

	res = devm_request_mem_region(&pdev->dev, res->start,
							resource_size(res),
							pdev->name);
	if (!res) {
		pr_err("failed to get memory region\n");
		return -ENXIO;
	}

	ltq_crypto_membase = devm_ioremap_nocache(&pdev->dev,
											res->start,
											resource_size(res));
	if (!ltq_crypto_membase) {
		pr_err("failed to remap memory\n");
		return -ENXIO;
	}

	err = ltq_crypto_device_initialize(pdev);
	if (err) {
		pr_err("Device error: Unable to initialize EIP97\n");
		return err;
	}

	/* Initialize EIP97 driver, global driver,
	 * DMA Resource, Interrupt & EIP202
	 */
	Driver97_Init(pdev);

	for (i = 0; i < LTQ_RINGS_USED_DRIVER; i++) {
		err = PEC_Init(i, &InitBlock);
		if (err != PEC_STATUS_OK) {
			pr_err("PEC_Init failed\n");
			return -EINVAL;
		}
	}

#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT
	irq = irq_of_parse_and_map(pdev->dev.of_node, 2);
	err = ltq_ipsec_init(FASTPATH_RING, irq);
	if (err) {
		pr_err("Error Initializing IPsec path\n");
		return -EINVAL;
	}
#else
	err = PEC_ResultNotify_Request(EIP97_RING_DEC,
				       (PEC_NotifyFunction_t)EIP97_DEC_CALLBACK,
				       1);
	if (err != PEC_STATUS_OK) {
		pr_err("Unable to register IRQ handler\n");
		return -EINVAL;
	}
#endif

	err = PEC_ResultNotify_Request(EIP97_RING_ENC,
				       (PEC_NotifyFunction_t)EIP97_ENC_CALLBACK,
				       1);
	if (err != PEC_STATUS_OK) {
		pr_err("Unable to register IRQ handler\n");
		return -EINVAL;
	}

	pr_info("LTQ Crypto Hardware Initialized ..\n");
	atomic_set(&packets_queue, 0);
	PEC_ResultNotificationCount = 0;

	/* Register ALGOs to linux crypto framework */
	for (i = 0; i < ARRAY_SIZE(ltq_crypto_alg); i++) {
		struct ltq_crypto_template *tmpl;
		struct crypto_alg *t_alg;

		tmpl = &ltq_crypto_alg[i];

		switch (tmpl->type) {
		case CRYPTO_ALG_TYPE_ABLKCIPHER:
			t_alg = &tmpl->alg.crypto;
			t_alg->cra_init = ltq_ablk_cra_init;
			t_alg->cra_exit = ltq_ablk_cra_exit;
			t_alg->cra_ctxsize = sizeof(struct ltq_crypto_ctx);
			if (!tmpl->alg.crypto.cra_ablkcipher.setkey)
				tmpl->alg.crypto.cra_ablkcipher.setkey = ltq_ablk_setkey;
			if (!tmpl->alg.crypto.cra_ablkcipher.encrypt)
				tmpl->alg.crypto.cra_ablkcipher.encrypt = ltq_ablk_encrypt;
			if (!tmpl->alg.crypto.cra_ablkcipher.decrypt)
				tmpl->alg.crypto.cra_ablkcipher.decrypt = ltq_ablk_decrypt;
			tmpl->alg.crypto.cra_ablkcipher.geniv = "eseqiv";
			break;
		case CRYPTO_ALG_TYPE_AHASH:
			t_alg = &tmpl->alg.hash.halg.base;
			t_alg->cra_type = &crypto_ahash_type;
			t_alg->cra_ctxsize = sizeof(struct ltq_ahash_ctx);
			t_alg->cra_init = ltq_ahash_cra_init;
			t_alg->cra_exit = ltq_ahash_cra_exit;
			tmpl->alg.hash.init = ltq_ahash_init;
			tmpl->alg.hash.export = ltq_ahash_export;
			tmpl->alg.hash.import = ltq_ahash_import;
			tmpl->alg.hash.update = ltq_ahash_update;
			tmpl->alg.hash.final = ltq_ahash_final;
			tmpl->alg.hash.finup = ltq_ahash_finup;
			if (!strncmp(t_alg->cra_name, "hmac", 4))
				tmpl->alg.hash.setkey = ltq_ahash_setkey;
			if (!strncmp(t_alg->cra_name, "xcbc", 4))
				tmpl->alg.hash.setkey = ltq_xcbc_ahash_setkey;
			tmpl->alg.hash.digest = ltq_ahash_digest;
			break;
		case CRYPTO_ALG_TYPE_AEAD:
			t_alg = &tmpl->alg.aead.base;
			t_alg->cra_ctxsize = sizeof(struct ltq_aead_ctx);
			tmpl->alg.aead.init = ltq_aead_cra_init;
			tmpl->alg.aead.exit = ltq_aead_cra_exit;
			if (!tmpl->alg.aead.setkey)
				tmpl->alg.aead.setkey = ltq_aead_setkey;
			if (!tmpl->alg.aead.setauthsize)
				tmpl->alg.aead.setauthsize = ltq_aead_setauthsize;
			if (!tmpl->alg.aead.encrypt)
				tmpl->alg.aead.encrypt = ltq_aead_encrypt;
			if (!tmpl->alg.aead.decrypt)
				tmpl->alg.aead.decrypt = ltq_aead_decrypt;
			break;
		default:
			pr_err("trying to register unkown algo type\n");
			return -EINVAL;
		}

		t_alg->cra_module = THIS_MODULE;
		t_alg->cra_priority = LTQ_CRYPTO_PRIORITY;
		t_alg->cra_alignmask = 0;
		t_alg->cra_flags |= CRYPTO_ALG_KERN_DRIVER_ONLY;

		if (tmpl->type == CRYPTO_ALG_TYPE_ABLKCIPHER) {
			err = crypto_register_alg(&tmpl->alg.crypto);
			if (err) {
				pr_err("Error trying to register algo driver: %s\n",
					tmpl->alg.crypto.cra_driver_name);
				goto crypt_err;
			}
		} else if (tmpl->type == CRYPTO_ALG_TYPE_AEAD) {
			err = crypto_register_aead(&tmpl->alg.aead);
			if (err) {
				pr_err("Error trying to register algo driver: %s\n",
					tmpl->alg.aead.base.cra_driver_name);
				goto crypt_err;
			}
		} else {
			err = crypto_register_ahash(&tmpl->alg.hash);
			if (err) {
				pr_err("Error trying to register algo driver: %s\n",
					tmpl->alg.hash.halg.base.cra_driver_name);
				goto crypt_err;
			}
		}
	}

	pr_info("LTQ crypto driver version: %d.%d.%d\n",
				V_MAJOR, V_MINOR, REVISION);

	return 0;

crypt_err:
	Driver97_Exit();
	for (i = 0; i < DRIVER_MAX_NOF_RING_TO_USE; i++)
		PEC_UnInit(i);

	return err;
}

static int ltq_crypto_remove(struct platform_device *pdev)
{
	Driver97_Exit();
	PEC_UnInit(EIP97_RING_ENC);
	PEC_UnInit(EIP97_RING_DEC);
	return 0;
}

static struct of_device_id ltq_crypto_match_table[] = {
	{.compatible = "lantiq,crypto-xrx500",},
	{},
};

static struct platform_driver ltq_crypto_driver = {
	.remove	= ltq_crypto_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "ltq-crypto-driver",
		.of_match_table = of_match_ptr(ltq_crypto_match_table),
	},
};

module_platform_driver_probe(ltq_crypto_driver, ltq_crypto_init);
MODULE_DESCRIPTION("Lantiq crypto engine support");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohammad Firdaus B Alias Thani");
