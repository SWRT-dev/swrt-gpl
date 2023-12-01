// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2022 MediaTek Inc.

#include <crypto/aes.h>
#include <crypto/hash.h>
#include <crypto/hmac.h>
#include <crypto/sha.h>
#include <crypto/sha3.h>
#include <net/xfrm.h>
#include <linux/ip.h>
#include <linux/psp-sev.h>
#include <linux/netdevice.h>

#include "mtk_eth_soc.h"
#include "mtk_ipsec.h"

static inline void write_state_le(__le32 *dst, const u32 *src, u32 size)
{
	int i;

	for (i = 0; i < SIZE_IN_WORDS(size); i++)
		dst[i] = cpu_to_le32(src[i]);
}

static inline void write_state_be(__le32 *dst, const u32 *src, u32 size)
{
	int i;

	for (i = 0; i < SIZE_IN_WORDS(size); i++)
		dst[i] = cpu_to_be32(src[i]);
}

static int hmac_init_iv(struct crypto_shash *tfm,
			unsigned int blocksize, u8 *pad, void *state)
{
	SHASH_DESC_ON_STACK(desc, tfm);
	int ret;

	desc->tfm = tfm;

	ret = crypto_shash_init(desc);
	if (ret)
		return ret;

	ret = crypto_shash_update(desc, pad, blocksize);
	if (ret && ret != -EINPROGRESS && ret != -EBUSY)
		return ret;

	crypto_shash_export(desc, state);
	shash_desc_zero(desc);

	return 0;
}

static int hmac_init_pad(unsigned int blocksize, const u8 *key,
			 unsigned int keylen, u8 *ipad, u8 *opad)
{
	int i;

	if (keylen <= blocksize)
		memcpy(ipad, key, keylen);

	memset(ipad + keylen, 0, blocksize - keylen);
	memcpy(opad, ipad, blocksize);

	for (i = 0; i < blocksize; i++) {
		ipad[i] ^= HMAC_IPAD_VALUE;
		opad[i] ^= HMAC_OPAD_VALUE;
	}

	return 0;
}

int hmac_setkey(const char *alg, const u8 *key, unsigned int keylen,
		void *istate, void *ostate)
{
	struct crypto_shash *tfm;
	unsigned int blocksize;
	u8 *ipad, *opad;
	int ret;

	tfm = crypto_alloc_shash(alg, 0, 0);
	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	crypto_shash_clear_flags(tfm, ~0);
	blocksize = crypto_tfm_alg_blocksize(crypto_shash_tfm(tfm));

	ipad = kcalloc(2, blocksize, GFP_KERNEL);
	if (!ipad) {
		ret = -ENOMEM;
		goto free_request;
	}

	opad = ipad + blocksize;

	ret = hmac_init_pad(blocksize, key, keylen, ipad, opad);
	if (ret)
		goto free_ipad;

	ret = hmac_init_iv(tfm, blocksize, ipad, istate);
	if (ret)
		goto free_ipad;

	ret = hmac_init_iv(tfm, blocksize, opad, ostate);

free_ipad:
	kfree(ipad);
free_request:
	crypto_free_shash(tfm);

	return ret;
}

static int mtk_ipsec_add_sa(struct xfrm_state *xs)
{
	struct net_device *dev = xs->xso.dev;
	struct mtk_mac *mac = netdev_priv(dev);
	struct mtk_eth *eth = mac->hw;
	struct context_record *context;
	struct ahash_export_state istate, ostate;
	unsigned char *key_aalg;
	unsigned char *key_ealg;
	unsigned int checksum;
	unsigned int key_len;
	int i;
	int cdrt_idx;

	if (xs->props.family != AF_INET) {
		netdev_info(dev, "Only IPv4 xfrm states may be offloaded\n");
		return -EINVAL;
	}

	if (xs->id.proto != IPPROTO_ESP) {
		netdev_info(dev, "Unsupported protocol 0x%04x\n",
			    xs->id.proto);
		return -EINVAL;
	}

	context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (unlikely(!context))
		return -ENOMEM;

	/**
	 * Set Transform record
	 * cdrt_idx=0, outbound for encryption
	 * cdrt_idx=1, inbound for decryption
	 **/
	if (xs->xso.flags & XFRM_OFFLOAD_INBOUND) {
		/* rx path */
		context->control0 = CTRL_WORD0_IN;
		context->control1 = CTRL_WORD1_IN;
		context->data[46] = 0x01020000;
		context->data[49] = 0x6117d6a5;
		context->data[50] = 0x07040c10;
		context->data[52] = 0xdd07000c;
		context->data[53] = 0xe4561820;
		cdrt_idx = 1;

	} else {
		/* tx path */
		context->control0 = CTRL_WORD0_OUT;
		context->control1 = CTRL_WORD1_OUT;
		memcpy(context->data + 38, &xs->props.saddr.a4, 4);
		memcpy(context->data + 42, &xs->id.daddr.a4, 4);
		context->data[46] = 0x04020000;
		context->data[49] = 0x9e14ed69;
		context->data[50] = 0x01020c10;
		context->data[52] = 0xd0060000;
		context->data[53] = 0xe1560811;
		context->data[55] = 0x00000049;
		cdrt_idx = 0;
	}
	context->data[47] = 0x00080000;
	context->data[48] = 0x00f00008;
	context->data[51] = 0x94119411;

	/* Calculate Checksum */
	checksum = 0;
	checksum += context->data[38] % 0x10000;
	checksum += context->data[38] / 0x10000;
	checksum += context->data[42] % 0x10000;
	checksum += context->data[42] / 0x10000;
	checksum += checksum / 0x10000;
	checksum = checksum % 0x10000;
	context->data[39] = checksum;

	/* EIP-96 context words[2...39]*/
	if (strcmp(xs->aalg->alg_name, "hmac(sha1)") == 0) {
		key_aalg = &xs->aalg->alg_key[0];
		hmac_setkey("sha1-generic", key_aalg,
			    xs->aalg->alg_key_len / 8,
			    &istate.state, &ostate.state);
		key_ealg = &xs->ealg->alg_key[0];
		key_len = xs->ealg->alg_key_len / 8;
		write_state_le(context->data, (const u32 *)key_ealg, key_len);
		write_state_be(context->data + SIZE_IN_WORDS(key_len),
			       (const u32 *)&istate.state, SHA1_DIGEST_SIZE);

		key_len += SHA1_DIGEST_SIZE;
		write_state_be(context->data + SIZE_IN_WORDS(key_len),
			       (const u32 *)&ostate.state, SHA1_DIGEST_SIZE);

		key_len += SHA1_DIGEST_SIZE;
		memcpy(context->data + SIZE_IN_WORDS(key_len),
		       &xs->id.spi, 4);
	}
	//TODO: sha256
	//else if (strcmp(xs->aalg->alg_name, "hmac(sha256)") == 0) {
	//}

	/**
	 * Set CDRT for inline IPSec
	 * Follow FE_CSR_MEM config flow.
	 **/

	/* Command descriptor W0-W3 */
	for (i = MTK_GLO_MEM_DATA0; i <= MTK_GLO_MEM_DATA9; i = i + 4)
		mtk_w32(eth, 0, i);

	mtk_w32(eth, TYPE(3), MTK_GLO_MEM_DATA0);
	mtk_w32(eth, TOKEN_LEN(48), MTK_GLO_MEM_DATA1);
	mtk_w32(eth, __psp_pa(context) | 2, MTK_GLO_MEM_DATA2);
	mtk_w32(eth, CTRL_CMD(1) | CTRL_INDEX(3) | CTRL_ADDR(cdrt_idx * 3),
		MTK_GLO_MEM_CTRL);

	/* Command descriptor W4-W7 */
	for (i = MTK_GLO_MEM_DATA0; i <= MTK_GLO_MEM_DATA9; i = i + 4)
		mtk_w32(eth, 0, i);

	mtk_w32(eth, HW_SER(2) | ALLOW_PAD | STRIP_PAD, MTK_GLO_MEM_DATA0);
	mtk_w32(eth, CTRL_CMD(1) | CTRL_INDEX(3) | CTRL_ADDR(cdrt_idx * 3 + 1),
		MTK_GLO_MEM_CTRL);

	/* Command descriptor W8-W11 */
	for (i = MTK_GLO_MEM_DATA0; i <= MTK_GLO_MEM_DATA9; i = i + 4)
		mtk_w32(eth, 0, i);

	mtk_w32(eth, CTRL_CMD(1) | CTRL_INDEX(3) | CTRL_ADDR(cdrt_idx * 3 + 2),
		MTK_GLO_MEM_CTRL);

	xs->xso.offload_handle = (unsigned long)context;

	return 0;
}

static void mtk_ipsec_free_state(struct xfrm_state *xs)
{
	struct context_record *context;

	if (!xs->xso.offload_handle)
		return;

	context = (struct context_record *)xs->xso.offload_handle;
	kfree(context);
}

static bool mtk_ipsec_offload_ok(struct sk_buff *skb,
				 struct xfrm_state *xs)
{
	struct xfrm_offload *xo = NULL;

	if (xs->xso.flags & XFRM_OFFLOAD_INBOUND) {
		/* rx path */
		if (xfrm_offload(skb) != NULL)
			xo = xfrm_offload(skb);

	} else {
		/* tx path */
		if (xfrm_offload(skb) != NULL)
			xo = xfrm_offload(skb);
	}

	if (xs->props.family == AF_INET) {
		/* Offload with IPv4 options is not supported yet */
		if (ip_hdr(skb)->ihl != 5)
			return false;
	}

	return true;
}

static const struct xfrmdev_ops mtk_xfrmdev_ops = {
	.xdo_dev_state_add = mtk_ipsec_add_sa,
	.xdo_dev_state_free = mtk_ipsec_free_state,
	.xdo_dev_offload_ok = mtk_ipsec_offload_ok,
};

void mtk_ipsec_offload_init(struct mtk_eth *eth)
{
	int i;

	for (i = 0; i < MTK_MAC_COUNT; i++)
		eth->netdev[i]->xfrmdev_ops = &mtk_xfrmdev_ops;
}
