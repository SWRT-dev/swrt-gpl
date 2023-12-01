/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2022 MediaTek Inc. */

#ifndef MTK_IPSEC_H
#define MTK_IPSEC_H

#define CTRL_WORD0_OUT		0x196b1006
#define CTRL_WORD1_OUT		0x51400001
#define CTRL_WORD0_IN		0x096ba20f
#define CTRL_WORD1_IN		0x00010001
#define SIZE_IN_WORDS(x)	((x) >> 2)

/* Global memory */
#define MTK_GLO_MEM_CFG		0x600
#define MTK_GLO_MEM_CTRL	0x604
#define MTK_GLO_MEM_DATA0	0x608
#define MTK_GLO_MEM_DATA1	0x60c
#define MTK_GLO_MEM_DATA2	0x610
#define MTK_GLO_MEM_DATA3	0x614
#define MTK_GLO_MEM_DATA4	0x618
#define MTK_GLO_MEM_DATA5	0x61c
#define MTK_GLO_MEM_DATA6	0x620
#define MTK_GLO_MEM_DATA7	0x624
#define MTK_GLO_MEM_DATA8	0x628
#define MTK_GLO_MEM_DATA9	0x62c

/* GLO MEM CTRL */
#define CTRL_CMD(x)		((x) << 30)
#define CTRL_CMD_SFT		30
#define CTRL_CMD_MASK		GENMASK(31, 30)
#define CTRL_INDEX(x)		((x) << 20)
#define CTRL_INDEX_SFT		20
#define CTRL_INDEX_MASK		GENMASK(29, 20)
#define CTRL_ADDR(x)		((x) << 0)
#define CTRL_ADDR_SFT		0
#define CTRL_ADDR_MASK		GENMASK(19, 0)

/* CDR Word0 */
#define TYPE(x)			((x) << 30)
#define TYPE_SFT		30
#define TYPE_MASK		GENMASK(31, 30)
#define ENCLASTDEST		BIT(25)
#define ENCLASTDEST_MASK	BIT(25)

/* CDR Word1 */
#define TOKEN_LEN(x)		((x) << 16)
#define TOKEN_LEN_SFT		16
#define TOKEN_LEN_MASK		GENMASK(23, 16)
#define APP_ID(x)		((x) << 9)
#define APP_ID_SFT		9
#define APP_ID_MASK		GENMASK(15, 9)
#define ADD_LEN(x)		((x) << 0)
#define ADD_LEN_SFT		0
#define ADD_LEN_MASK		GENMASK(7, 0)

/* CDR Word4 */
#define FLOW_LOOKUP		BIT(31)
#define FLOW_LOOKUP_MASK	BIT(31)
#define HW_SER(x)		((x) << 24)
#define HW_SER_SFT		24
#define HW_SER_MASK		GENMASK(29, 24)
#define ALLOW_PAD		BIT(23)
#define ALLOW_PAD_MASK		BIT(23)
#define STRIP_PAD		BIT(22)
#define STRIP_PAD_MASK		BIT(22)
#define USER_DEF(x)		((x) << 0)
#define USER_DEF_SFT		0
#define USER_DEF_MASK		GENMASK(15, 0)

/* CDR Word5 */
#define KEEP_OUTER		BIT(28)
#define KEEP_OUTER_MASK		BIT(28)
#define PARSE_ETH		BIT(27)
#define PARSE_ETH_MASK		BIT(27)
#define L4CHECKSUM		BIT(26)
#define L4CHECKSUM_MASK		BIT(26)
#define IPV4CHECKSUM		BIT(25)
#define IPV4CHECKSUM_MASK	BIT(25)
#define FL			BIT(24)
#define FL_MASK			BIT(24)
#define NEXT_HEADER(x)		((x) << 16)
#define NEXT_HEADER_SFT		16
#define NEXT_HEADER_MASK	GENMASK(23, 16)

#define HASH_CACHE_SIZE		SHA512_BLOCK_SIZE

struct ahash_export_state {
	u64 len;
	u64 processed;

	u32 digest;

	u32 state[SHA512_DIGEST_SIZE / sizeof(u32)];
	u8 cache[HASH_CACHE_SIZE];
};

/* Context Control */
struct context_record {
	__le32 control0;
	__le32 control1;

	__le32 data[62];
};

void mtk_ipsec_offload_init(struct mtk_eth *eth);
#endif /* MTK_IPSEC_H */
