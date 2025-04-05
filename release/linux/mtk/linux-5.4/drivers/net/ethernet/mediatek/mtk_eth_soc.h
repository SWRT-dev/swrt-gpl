/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *
 *   Copyright (C) 2009-2016 John Crispin <blogic@openwrt.org>
 *   Copyright (C) 2009-2016 Felix Fietkau <nbd@openwrt.org>
 *   Copyright (C) 2013-2016 Michael Lee <igvtee@gmail.com>
 */

#ifndef MTK_ETH_H
#define MTK_ETH_H

#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/of_net.h>
#include <linux/u64_stats_sync.h>
#include <linux/refcount.h>
#include <linux/phylink.h>

#define MTK_QDMA_PAGE_SIZE	2048
#define	MTK_MAX_RX_LENGTH	1536
#define MTK_MAX_RX_LENGTH_2K	2048
#define MTK_MAX_RX_LENGTH_9K	9216
#define MTK_MIN_TX_LENGTH	60
#define MTK_DMA_SIZE(x)		(SZ_##x)
#define MTK_FQ_DMA_HEAD		32
#define MTK_FQ_DMA_LENGTH	2048
#define MTK_NAPI_WEIGHT		256

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_MAC_COUNT		3
#define MTK_WDMA_CNT		3
#else
#define MTK_MAC_COUNT		2
#define MTK_WDMA_CNT		2
#endif

#define MTK_RX_ETH_HLEN		(VLAN_ETH_HLEN + VLAN_HLEN + ETH_FCS_LEN)
#define MTK_RX_HLEN		(NET_SKB_PAD + MTK_RX_ETH_HLEN + NET_IP_ALIGN)
#define MTK_DMA_DUMMY_DESC	0xffffffff
#define MTK_DEFAULT_MSG_ENABLE	(NETIF_MSG_DRV | \
				 NETIF_MSG_PROBE | \
				 NETIF_MSG_LINK | \
				 NETIF_MSG_TIMER | \
				 NETIF_MSG_IFDOWN | \
				 NETIF_MSG_IFUP | \
				 NETIF_MSG_RX_ERR | \
				 NETIF_MSG_TX_ERR)
#define MTK_HW_FEATURES		(NETIF_F_IP_CSUM | \
				 NETIF_F_RXCSUM | \
				 NETIF_F_HW_VLAN_CTAG_TX | \
				 NETIF_F_SG | NETIF_F_TSO | \
				 NETIF_F_TSO6 | \
				 NETIF_F_IPV6_CSUM)
#define MTK_SET_FEATURES	(NETIF_F_LRO | \
				 NETIF_F_HW_VLAN_CTAG_RX)
#define MTK_HW_FEATURES_MT7628	(NETIF_F_SG | NETIF_F_RXCSUM)
#define NEXT_DESP_IDX(X, Y)	(((X) + 1) & ((Y) - 1))

#define MTK_QRX_OFFSET		0x10

#define MTK_HW_LRO_DMA_SIZE	64

#define	MTK_MAX_LRO_RX_LENGTH		(4096 * 3)
#define	MTK_MAX_LRO_IP_CNT		2
#define	MTK_HW_LRO_TIMER_UNIT		1	/* 20 us */
#define	MTK_HW_LRO_REFRESH_TIME		50000	/* 1 sec. */
#define	MTK_HW_LRO_AGG_TIME		10	/* 200us */
#define	MTK_HW_LRO_AGE_TIME		50	/* 1ms */
#define	MTK_HW_LRO_MAX_AGG_CNT		64
#define	MTK_HW_LRO_BW_THRE		3000
#define	MTK_HW_LRO_REPLACE_DELTA	1000
#define	MTK_HW_LRO_SDL_REMAIN_ROOM	1522

#define MTK_RSS_HASH_KEYSIZE		40
#define MTK_RSS_MAX_INDIRECTION_TABLE	128

/* Frame Engine Global Configuration */
#define MTK_FE_GLO_CFG(x)	((x < 8) ? 0x0 : 0x24)
#define MTK_FE_LINK_DOWN_P(x)	((x < 8) ? FIELD_PREP(GENMASK(15, 8), BIT(x)) :	\
					   FIELD_PREP(GENMASK(7, 0), BIT(x - 8)))

/* Frame Engine Global Reset Register */
#define MTK_RST_GL		0x04
#define RST_GL_PSE		BIT(0)

/* Frame Engine Interrupt Status Register */
#define MTK_FE_INT_STATUS	0x08
#define MTK_FE_INT_STATUS2	0x28
#define MTK_FE_INT_ENABLE	0x0C
#define MTK_FE_INT_FQ_EMPTY	BIT(8)
#define MTK_FE_INT_TSO_FAIL	BIT(12)
#define MTK_FE_INT_TSO_ILLEGAL	BIT(13)
#define MTK_FE_INT_TSO_ALIGN	BIT(14)
#define MTK_FE_INT_RFIFO_OV	BIT(18)
#define MTK_FE_INT_RFIFO_UF	BIT(19)
#define MTK_GDM1_AF		BIT(28)
#define MTK_GDM2_AF		BIT(29)
#if defined(CONFIG_MEDIATEK_NETSYS_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_FE_IRQ_NUM		(4)
#else
#define MTK_FE_IRQ_NUM		(3)
#endif
#define MTK_PDMA_IRQ_NUM	(4)
#define MTK_MAX_IRQ_NUM		(MTK_FE_IRQ_NUM + MTK_PDMA_IRQ_NUM)

/* PDMA HW LRO Alter Flow Timer Register */
#define MTK_PDMA_LRO_ALT_REFRESH_TIMER	0x1c

/* Frame Engine Interrupt Grouping Register */
#define MTK_FE_INT_GRP		0x20

/* Frame Engine LRO auto-learn table info */
#define MTK_FE_ALT_CF8		0x300
#define MTK_FE_ALT_SGL_CFC	0x304
#define MTK_FE_ALT_SEQ_CFC	0x308

/* CDMP Ingress Control Register */
#define MTK_CDMQ_IG_CTRL	0x1400
#define MTK_CDMQ_STAG_EN	BIT(0)

/* CDMP Ingress Control Register */
#define MTK_CDMP_IG_CTRL	0x400
#define MTK_CDMP_STAG_EN	BIT(0)

/* CDMP Exgress Control Register */
#define MTK_CDMP_EG_CTRL	0x404

/* GDM Ingress Control Register */
#define MTK_GDMA_FWD_CFG(x)	((x == MTK_GMAC3_ID) ?		\
				 0x540 : 0x500 + (x * 0x1000))
#define MTK_GDMA_SPECIAL_TAG	BIT(24)
#define MTK_GDMA_ICS_EN		BIT(22)
#define MTK_GDMA_TCS_EN		BIT(21)
#define MTK_GDMA_UCS_EN		BIT(20)
#define MTK_GDMA_STRP_CRC	BIT(16)
#define MTK_GDMA_TO_PDMA	0x0
#define MTK_GDMA_DROP_ALL	0x7777

/* GDM Egress Control Register */
#define MTK_GDMA_EG_CTRL(x)	((x == MTK_GMAC3_ID) ?		\
				 0x544 : 0x504 + (x * 0x1000))
#define MTK_GDMA_XGDM_SEL        BIT(31)

/* Unicast Filter MAC Address Register - Low */
#define MTK_GDMA_MAC_ADRL(x)	((x == MTK_GMAC3_ID) ?		\
				 0x548 : 0x508 + (x * 0x1000))

/* Unicast Filter MAC Address Register - High */
#define MTK_GDMA_MAC_ADRH(x)	((x == MTK_GMAC3_ID) ?		\
				 0x54C : 0x50C + (x * 0x1000))

/* Internal SRAM offset */
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_ETH_SRAM_OFFSET	0x300000
#else
#define MTK_ETH_SRAM_OFFSET	0x40000
#endif

/* FE global misc reg*/
#define MTK_FE_GLO_MISC         0x124

/* PSE Free Queue Flow Control  */
#define PSE_FQFC_CFG1		0x100
#define PSE_FQFC_CFG2		0x104
#define PSE_NO_DROP_CFG		0x108
#define PSE_PPE0_DROP		0x110

/* PSE Last FreeQ Page Request Control */
#define PSE_DUMY_REQ		0x10C
#define PSE_DUMMY_WORK_GDM(x)	BIT(16 + (x))
#define DUMMY_PAGE_THR		0x151

/* PSE Input Queue Reservation Register*/
#define PSE_IQ_REV(x)		(0x140 + ((x - 1) * 0x4))

/* PSE Output Queue Threshold Register*/
#define PSE_OQ_TH(x)		(0x160 + ((x - 1) * 0x4))

/* GDM and CDM Threshold */
#define MTK_GDM2_THRES		0x1530
#define MTK_CDM2_THRES		0x1534
#define MTK_CDMW0_THRES		0x164c
#define MTK_CDMW1_THRES		0x1650
#define MTK_CDME0_THRES		0x1654
#define MTK_CDME1_THRES		0x1658
#define MTK_CDMM_THRES		0x165c

#define MTK_PDMA_V2		BIT(4)

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define PDMA_BASE               0x6800
#define QDMA_BASE               0x4400
#define WDMA_BASE(x)		(0x4800 + ((x) * 0x400))
#define PPE_BASE(x)		((x == 2) ? 0x2E00 : 0x2200 + ((x) * 0x400))
#elif defined(CONFIG_MEDIATEK_NETSYS_V2)
#ifdef CONFIG_MEDIATEK_NETSYS_RX_V2
#define PDMA_BASE               0x6000
#else
#define PDMA_BASE               0x4000
#endif
#define QDMA_BASE               0x4400
#define WDMA_BASE(x)		(0x4800 + ((x) * 0x400))
#define PPE_BASE(x)		(0x2200 + ((x) * 0x400))
#else
#define PDMA_BASE               0x0800
#define QDMA_BASE               0x1800
#define WDMA_BASE(x)		(0x2800 + ((x) * 0x400))
#define PPE_BASE(x)		(0xE00 + ((x) * 0x400))
#endif
/* PDMA TX CPU Pointer Register */
#define MTK_PTX_CTX_IDX0	(PDMA_BASE + 0x08)
#define MTK_PTX_CTX_IDX_CFG(x)	(MTK_PTX_CTX_IDX0 + ((x) * 0x10))

/* PDMA TX DMA Pointer Register */
#define MTK_PTX_DTX_IDX0	(PDMA_BASE + 0x0c)
#define MTK_PTX_DTX_IDX_CFG(x)	(MTK_PTX_DTX_IDX0 + ((x) * 0x10))

/* PDMA RX Base Pointer Register */
#define MTK_PRX_BASE_PTR0	(PDMA_BASE + 0x100)
#define MTK_PRX_BASE_PTR_CFG(x)	(MTK_PRX_BASE_PTR0 + (x * 0x10))

/* PDMA RX Maximum Count Register */
#define MTK_PRX_MAX_CNT0	(MTK_PRX_BASE_PTR0 + 0x04)
#define MTK_PRX_MAX_CNT_CFG(x)	(MTK_PRX_MAX_CNT0 + (x * 0x10))

/* PDMA RX CPU Pointer Register */
#define MTK_PRX_CRX_IDX0	(MTK_PRX_BASE_PTR0 + 0x08)
#define MTK_PRX_CRX_IDX_CFG(x)	(MTK_PRX_CRX_IDX0 + (x * 0x10))

/* PDMA RX DMA Pointer Register */
#define MTK_PRX_DRX_IDX0	(MTK_PRX_BASE_PTR0 + 0x0c)
#define MTK_PRX_DRX_IDX_CFG(x)	(MTK_PRX_DRX_IDX0 + (x * 0x10))

/* PDMA HW LRO Control Registers */
#define BITS(m, n)			(~(BIT(m) - 1) & ((BIT(n) - 1) | BIT(n)))
#define MTK_HW_LRO_DIP_NUM		(4)
#if defined(CONFIG_MEDIATEK_NETSYS_RX_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_MAX_RX_RING_NUM		(8)
#define MTK_HW_LRO_RING_NUM		(4)
#define MTK_HW_LRO_RING(x)		((x) + 4)
#define MTK_HW_LRO_IRQ(x)		(x)
#define IS_HW_LRO_RING(ring_no)		(((ring_no) > 3) && ((ring_no) < 8))
#define MTK_L3_CKS_UPD_EN		BIT(19)
#define MTK_LRO_CRSN_BNW		BIT(22)
#define MTK_LRO_RING_RELINGUISH_REQ	(0xf << 24)
#define MTK_LRO_RING_RELINGUISH_DONE	(0xf << 28)
#else
#define MTK_MAX_RX_RING_NUM		(4)
#define MTK_HW_LRO_RING_NUM		(3)
#define MTK_HW_LRO_RING(x)		((x) + 1)
#define MTK_HW_LRO_IRQ(x)		((x) + 1)
#define IS_HW_LRO_RING(ring_no)		(((ring_no) > 0) && ((ring_no) < 4))
#define MTK_LRO_CRSN_BNW		BIT(6)
#define MTK_L3_CKS_UPD_EN		BIT(7)
#define MTK_LRO_RING_RELINGUISH_REQ	(0x7 << 26)
#define MTK_LRO_RING_RELINGUISH_DONE	(0x7 << 29)
#endif

#define IS_NORMAL_RING(ring_no)		((ring_no) == 0)
#define MTK_LRO_EN			BIT(0)
#define MTK_NON_LRO_MULTI_EN   		BIT(2)
#define MTK_LRO_DLY_INT_EN		BIT(5)
#define MTK_LRO_ALT_PKT_CNT_MODE	BIT(21)
#define MTK_LRO_L4_CTRL_PSH_EN		BIT(23)
#define MTK_CTRL_DW0_SDL_OFFSET		(3)
#define MTK_CTRL_DW0_SDL_MASK		BITS(3, 18)

#define MTK_ADMA_MODE			BIT(15)
#define MTK_LRO_MIN_RXD_SDL		(MTK_HW_LRO_SDL_REMAIN_ROOM << 16)

/* PDMA RSS Control Registers */
#define MTK_RX_NAPI_NUM			(8)
#define MTK_RX_RSS_NUM			(3)
#define MTK_RSS_RING(x)			((x) + 1)
#define MTK_RSS_EN			BIT(0)
#define MTK_RSS_CFG_REQ			BIT(2)
#define MTK_RSS_IPV6_STATIC_HASH	(0x7 << 8)
#define MTK_RSS_IPV4_STATIC_HASH	(0x7 << 12)

/* PDMA Global Configuration Register */
#define MTK_PDMA_GLO_CFG	(PDMA_BASE + 0x204)
#define MTK_CSR_CLKGATE_BYP	BIT(30)
#define MTK_MULTI_EN		BIT(10)
#define MTK_RX_DMA_LRO_EN	BIT(8)
#define MTK_PDMA_SIZE_8DWORDS	(1 << 4)

/* PDMA V2 Global Configuration Register */
#define MTK_DEC_WCOMP		BIT(28)
#define MTK_PDMA_RESV_BUF	(0x40 << 16)
#define MTK_PDMA_MUTLI_CNT	(0xf << 12)
#define MTK_MULTI_EN_V2		BIT(11)
#define MTK_CHK_DDONE		BIT(10)

/* PDMA RX DMA Configuration Register */
#define MTK_PDMA_LRO_SDL	(0x3000)
#define MTK_RX_CFG_SDL_OFFSET	(16)

/* PDMA Reset Index Register */
#define MTK_PDMA_RST_IDX	(PDMA_BASE + 0x208)
#define MTK_PST_DRX_IDX0	BIT(16)
#define MTK_PST_DTX_IDX0	BIT(0)
#define MTK_PST_DRX_IDX_CFG(x)	(MTK_PST_DRX_IDX0 << (x))
#define MTK_PST_DTX_IDX_CFG(x)	(MTK_PST_DTX_IDX0 << (x))

/*PDMA HW RX Index Register*/
#define MTK_ADMA_CRX_PTR	(PDMA_BASE + 0x108)
#define MTK_ADMA_DRX_PTR	(PDMA_BASE + 0x10C)

/* PDMA Delay Interrupt Register */
#define MTK_PDMA_DELAY_INT		(PDMA_BASE + 0x20c)
#define MTK_PDMA_TX_DELAY_INT0		(PDMA_BASE + 0x2b0)
#define MTK_PDMA_TX_DELAY_INT1		(PDMA_BASE + 0x2b4)
#if defined(CONFIG_MEDIATEK_NETSYS_RX_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_PDMA_RSS_DELAY_INT		(PDMA_BASE + 0x2c0)
#else
#define MTK_PDMA_RSS_DELAY_INT		(PDMA_BASE + 0x270)
#endif
#define MTK_PDMA_DELAY_RX_EN		BIT(15)
#define MTK_PDMA_DELAY_RX_PINT		4
#define MTK_PDMA_DELAY_RX_PINT_SHIFT	8
#define MTK_PDMA_DELAY_RX_PTIME		4
#define MTK_PDMA_DELAY_RX_DELAY		\
	(MTK_PDMA_DELAY_RX_EN | MTK_PDMA_DELAY_RX_PTIME | \
	(MTK_PDMA_DELAY_RX_PINT << MTK_PDMA_DELAY_RX_PINT_SHIFT))

/* PDMA Interrupt Status Register */
#define MTK_PDMA_INT_STATUS	(PDMA_BASE + 0x220)

/* PDMA Interrupt Mask Register */
#define MTK_PDMA_INT_MASK	(PDMA_BASE + 0x228)

/* PDMA Interrupt grouping registers */
#define MTK_PDMA_INT_GRP1	(PDMA_BASE + 0x250)
#define MTK_PDMA_INT_GRP2	(PDMA_BASE + 0x254)
#if defined(CONFIG_MEDIATEK_NETSYS_RX_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_PDMA_INT_GRP3	(PDMA_BASE + 0x258)
#else
#define MTK_PDMA_INT_GRP3	(PDMA_BASE + 0x22c)
#endif
#define MTK_MAX_DELAY_INT	0x8f0f
#define MTK_MAX_DELAY_INT_V2	0x8f0f8f0f

/* PDMA HW LRO IP Setting Registers */
#define MTK_RING_MYIP_VLD		BIT(9)

/* PDMA HW LRO ALT Debug Registers */
#define MTK_LRO_ALT_INDEX_OFFSET	(8)

/* PDMA HW LRO Ring Control Registers */
#define MTK_RING_AGE_TIME_L		((MTK_HW_LRO_AGE_TIME & 0x3ff) << 22)
#define MTK_RING_AGE_TIME_H		((MTK_HW_LRO_AGE_TIME >> 10) & 0x3f)
#define MTK_RING_PSE_MODE        	(1 << 6)
#define MTK_RING_AUTO_LERAN_MODE	(3 << 6)
#define MTK_RING_VLD			BIT(8)
#define MTK_RING_MAX_AGG_TIME		((MTK_HW_LRO_AGG_TIME & 0xffff) << 10)
#define MTK_RING_MAX_AGG_CNT_L		((MTK_HW_LRO_MAX_AGG_CNT & 0x3f) << 26)
#define MTK_RING_MAX_AGG_CNT_H		((MTK_HW_LRO_MAX_AGG_CNT >> 6) & 0x3)

/* LRO_RX_RING_CTRL_DW masks */
#define MTK_LRO_RING_AGG_TIME_MASK	BITS(10, 25)
#define MTK_LRO_RING_AGG_CNT_L_MASK	BITS(26, 31)
#define MTK_LRO_RING_AGG_CNT_H_MASK	BITS(0, 1)
#define MTK_LRO_RING_AGE_TIME_L_MASK	BITS(22, 31)
#define MTK_LRO_RING_AGE_TIME_H_MASK	BITS(0, 5)

/* LRO_RX_RING_CTRL_DW0 offsets */
#define MTK_RX_IPV6_FORCE_OFFSET	(0)
#define MTK_RX_IPV4_FORCE_OFFSET	(1)

/* LRO_RX_RING_CTRL_DW1 offsets  */
#define MTK_LRO_RING_AGE_TIME_L_OFFSET	(22)

/* LRO_RX_RING_CTRL_DW2 offsets  */
#define MTK_LRO_RING_AGE_TIME_H_OFFSET	(0)
#define MTK_RX_MODE_OFFSET		(6)
#define MTK_RX_PORT_VALID_OFFSET	(8)
#define MTK_RX_MYIP_VALID_OFFSET	(9)
#define MTK_LRO_RING_AGG_TIME_OFFSET	(10)
#define MTK_LRO_RING_AGG_CNT_L_OFFSET	(26)

/* LRO_RX_RING_CTRL_DW3 offsets  */
#define MTK_LRO_RING_AGG_CNT_H_OFFSET	(0)

/* LRO_RX_RING_STP_DTP_DW offsets */
#define MTK_RX_TCP_DEST_PORT_OFFSET	(0)
#define MTK_RX_TCP_SRC_PORT_OFFSET	(16)

/* QDMA TX Queue Configuration Registers */
#define MTK_QTX_CFG(x)			(QDMA_BASE + (x * 0x10))
#define MTK_QTX_CFG_HW_RESV_CNT_OFFSET	GENMASK(15, 8)
#define MTK_QTX_CFG_SW_RESV_CNT_OFFSET	GENMASK(7, 0)
#define MTK_QTX_OFFSET			0x10
#define QDMA_RES_THRES			4

/* QDMA TX Queue Scheduler Registers */
#define MTK_QTX_SCH(x)			(QDMA_BASE + 4 + (x * 0x10))
#define MTK_QTX_SCH_TX_SEL		BIT(31)
#define MTK_QTX_SCH_TX_SEL_V2		GENMASK(31, 30)
#define MTK_QTX_SCH_LEAKY_BUCKET_EN	BIT(30)
#define MTK_QTX_SCH_LEAKY_BUCKET_SIZE	GENMASK(29, 28)
#define MTK_QTX_SCH_MIN_RATE_EN		BIT(27)
#define MTK_QTX_SCH_MIN_RATE_MAN	GENMASK(26, 20)
#define MTK_QTX_SCH_MIN_RATE_EXP	GENMASK(19, 16)
#define MTK_QTX_SCH_MAX_RATE_WEIGHT	GENMASK(15, 12)
#define MTK_QTX_SCH_MAX_RATE_EN		BIT(11)
#define MTK_QTX_SCH_MAX_RATE_MAN	GENMASK(10, 4)
#define MTK_QTX_SCH_MAX_RATE_EXP	GENMASK(3, 0)

/* QDMA RX Base Pointer Register */
#define MTK_QRX_BASE_PTR0	(QDMA_BASE + 0x100)
#define MTK_QRX_BASE_PTR_CFG(x)	(MTK_QRX_BASE_PTR0 + ((x) * 0x10))

/* QDMA RX Maximum Count Register */
#define MTK_QRX_MAX_CNT0	(QDMA_BASE + 0x104)
#define MTK_QRX_MAX_CNT_CFG(x)	(MTK_QRX_MAX_CNT0 + ((x) * 0x10))

/* QDMA RX CPU Pointer Register */
#define MTK_QRX_CRX_IDX0	(QDMA_BASE + 0x108)
#define MTK_QRX_CRX_IDX_CFG(x)	(MTK_QRX_CRX_IDX0 + ((x) * 0x10))

/* QDMA RX DMA Pointer Register */
#define MTK_QRX_DRX_IDX0	(QDMA_BASE + 0x10c)

/* QDMA Page Configuration Register */
#define MTK_QDMA_PAGE		(QDMA_BASE + 0x1f0)
#define MTK_QTX_CFG_PAGE	GENMASK(3, 0)
#define MTK_QTX_PER_PAGE	(16)

/* QDMA Global Configuration Register */
#define MTK_QDMA_GLO_CFG	(QDMA_BASE + 0x204)
#define MTK_RX_2B_OFFSET	BIT(31)
#define MTK_PKT_RX_WDONE	BIT(27)
#define MTK_RX_BT_32DWORDS	(3 << 11)
#define MTK_NDP_CO_PRO		BIT(10)
#define MTK_TX_WB_DDONE		BIT(6)
#define MTK_DMA_SIZE_MASK	GENMASK(5, 4)
#define MTK_DMA_SIZE_16DWORDS	(2 << 4)
#define MTK_DMA_SIZE_32DWORDS	(3 << 4)
#define MTK_RX_DMA_BUSY		BIT(3)
#define MTK_TX_DMA_BUSY		BIT(1)
#define MTK_RX_DMA_EN		BIT(2)
#define MTK_TX_DMA_EN		BIT(0)
#define MTK_DMA_BUSY_TIMEOUT	HZ

/* QDMA V2 Global Configuration Register */
#define MTK_CHK_DDONE_EN	BIT(28)
#define MTK_PKT_RX_WDONE	BIT(27)
#define MTK_DMAD_WR_WDONE	BIT(26)
#define MTK_WCOMP_EN		BIT(24)
#define MTK_RESV_BUF		(0x80 << 16)
#define MTK_MUTLI_CNT		(0x4 << 12)
#define MTK_RESV_BUF_MASK	(0xff << 16)

/* QDMA Reset Index Register */
#define MTK_QDMA_RST_IDX	(QDMA_BASE + 0x208)

/* QDMA Delay Interrupt Register */
#define MTK_QDMA_DELAY_INT	(QDMA_BASE + 0x20c)

/* QDMA Flow Control Register */
#define MTK_QDMA_FC_THRES	(QDMA_BASE + 0x210)
#define FC_THRES_DROP_MODE	BIT(20)
#define FC_THRES_DROP_EN	(7 << 16)
#define FC_THRES_MIN		0x4444

/* QDMA TX Scheduler Rate Control Register */
#define MTK_QDMA_TX_2SCH_BASE	(QDMA_BASE + 0x214)

/* QDMA Interrupt Status Register */
#define MTK_QDMA_INT_STATUS	(QDMA_BASE + 0x218)
#if defined(CONFIG_MEDIATEK_NETSYS_RX_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_RX_DONE_INT(ring_no)						\
	(MTK_HAS_CAPS(eth->soc->caps, MTK_RSS) ? (BIT(24 + (ring_no))) :	\
	 ((ring_no) ? BIT(16 + (ring_no)) : BIT(14)))
#else
#define MTK_RX_DONE_INT(ring_no)						\
	(MTK_HAS_CAPS(eth->soc->caps, MTK_RSS) ? ((ring_no) ? BIT(24 + (ring_no)) : BIT(30)) :	\
	 (BIT(16 + (ring_no))))
#endif
#define MTK_RX_DONE_INT3	BIT(19)
#define MTK_RX_DONE_INT2	BIT(18)
#define MTK_RX_DONE_INT1	BIT(17)
#define MTK_RX_DONE_INT0	BIT(16)
#define MTK_TX_DONE_INT3	BIT(3)
#define MTK_TX_DONE_INT2	BIT(2)
#define MTK_TX_DONE_INT1	BIT(1)
#define MTK_TX_DONE_INT0	BIT(0)
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_TX_DONE_INT(ring_no)				\
	(MTK_HAS_CAPS(eth->soc->caps, MTK_QDMA) ? BIT(28) :	\
	 BIT(4 + (ring_no)))
#else
#define MTK_TX_DONE_INT(ring_no)				\
	(MTK_HAS_CAPS(eth->soc->caps, MTK_QDMA) ? BIT(28) :	\
	 BIT((ring_no)))
#endif

/* QDMA Interrupt grouping registers */
#define MTK_QDMA_INT_GRP1	(QDMA_BASE + 0x220)
#define MTK_QDMA_INT_GRP2	(QDMA_BASE + 0x224)
#define MTK_RLS_DONE_INT	BIT(0)

/* QDMA Interrupt Status Register */
#define MTK_QDMA_INT_MASK	(QDMA_BASE + 0x21c)

/* QDMA DMA FSM */
#define MTK_QDMA_FSM		(QDMA_BASE + 0x234)

/* QDMA Interrupt Mask Register */
#define MTK_QDMA_HRED2		(QDMA_BASE + 0x244)

/* QDMA TX Forward CPU Pointer Register */
#define MTK_QTX_CTX_PTR		(QDMA_BASE +0x300)

/* QDMA TX Forward DMA Pointer Register */
#define MTK_QTX_DTX_PTR		(QDMA_BASE +0x304)

/* QDMA TX Forward DMA Counter */
#define MTK_QDMA_FWD_CNT	(QDMA_BASE + 0x308)

/* QDMA TX Release CPU Pointer Register */
#define MTK_QTX_CRX_PTR		(QDMA_BASE +0x310)

/* QDMA TX Release DMA Pointer Register */
#define MTK_QTX_DRX_PTR		(QDMA_BASE +0x314)

/* QDMA FQ Head Pointer Register */
#define MTK_QDMA_FQ_HEAD	(QDMA_BASE +0x320)

/* QDMA FQ Head Pointer Register */
#define MTK_QDMA_FQ_TAIL	(QDMA_BASE +0x324)

/* QDMA FQ Free Page Counter Register */
#define MTK_QDMA_FQ_CNT		(QDMA_BASE +0x328)

/* QDMA FQ Free Page Buffer Length Register */
#define MTK_QDMA_FQ_BLEN	(QDMA_BASE +0x32c)

/* QDMA TX Scheduler Rate Control Register */
#define MTK_QDMA_TX_4SCH_BASE(x)	(QDMA_BASE + 0x398 + (((x) >> 1) * 0x4))
#define MTK_QDMA_TX_SCH_MASK		GENMASK(15, 0)

/* WDMA Registers */
#define MTK_WDMA_CTX_PTR(x)	(WDMA_BASE(x) + 0x8)
#define MTK_WDMA_DTX_PTR(x)	(WDMA_BASE(x) + 0xC)
#define MTK_WDMA_GLO_CFG(x)	(WDMA_BASE(x) + 0x204)
#define MTK_WDMA_TX_DBG_MON0(x)	(WDMA_BASE(x) + 0x230)
#define MTK_WDMA_RX_DBG_MON1(x)	(WDMA_BASE(x) + 0x3c4)
#define MTK_WDMA_RX_MAX_CNT(x)	(WDMA_BASE(x) + 0x104)
#define MTK_WDMA_CRX_PTR(x)	(WDMA_BASE(x) + 0x108)
#define MTK_WDMA_DRX_PTR(x)	(WDMA_BASE(x) + 0x10C)
#define MTK_CDM_FS_PARSER_FSM_MASK	GENMASK(27, 24)
#define MTK_CDM_FS_FSM_MASK		GENMASK(19, 16)
#define MTK_CDM_TS_PARSER_FSM_MASK	GENMASK(12, 8)
#define MTK_CDM_TS_FSM_MASK		GENMASK(3, 0)

/*TDMA Register*/
#define MTK_TDMA_GLO_CFG	(0x6204)

/* GMA1 Received Good Byte Count Register */
#if defined(CONFIG_MEDIATEK_NETSYS_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_GDM1_TX_GBCNT       0x1C00
#else
#define MTK_GDM1_TX_GBCNT	0x2400
#endif

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_STAT_OFFSET		0x80
#else
#define MTK_STAT_OFFSET		0x40
#endif

/* QDMA TX NUM */
#define MTK_QDMA_TX_NUM		16
#define MTK_QDMA_PAGE_NUM	8
#define MTK_QDMA_TX_MASK	((MTK_QDMA_TX_NUM) - 1)
#define QID_LOW_BITS(x)         ((x) & 0xf)
#define QID_HIGH_BITS(x)        ((((x) >> 4) & 0x3) << 20)
#define QID_BITS_V2(x)		(((x) & 0x3f) << 16)

#define MTK_QDMA_GMAC2_QID	8
#define MTK_QDMA_GMAC3_QID	6

/* QDMA V2 descriptor txd8 */
#define TX_DMA_CDRT_SHIFT          0
#define TX_DMA_CDRT_MASK           0xff
#define TX_DMA_TOPS_ENTRY_SHIFT    8
#define TX_DMA_TOPS_ENTRY_MASK     0x3f

/* QDMA V2 descriptor txd6 */
#define TX_DMA_INS_VLAN_V2         BIT(16)

/* QDMA V2 descriptor txd5 */
#define TX_DMA_CHKSUM_V2           (0x7 << 28)
#define TX_DMA_TSO_V2              BIT(31)
#define TX_DMA_SPTAG_V3            BIT(27)

/* QDMA V2 descriptor txd4 */
#define EIP197_QDMA_TPORT          3
#define TX_DMA_TPORT_SHIFT         0
#define TX_DMA_TPORT_MASK          0xf
#define TX_DMA_FPORT_SHIFT_V2      8
#define TX_DMA_FPORT_MASK_V2       0xf
#define TX_DMA_SWC_V2              BIT(30)

#define MTK_TX_DMA_BUF_LEN      0x3fff
#define MTK_TX_DMA_BUF_LEN_V2   0xffff
#define MTK_TX_DMA_BUF_SHIFT    16
#define MTK_TX_DMA_BUF_SHIFT_V2 8

#define MTK_RX_DMA_BUF_LEN      0x3fff
#define MTK_RX_DMA_BUF_SHIFT    16

#define RX_DMA_SPORT_SHIFT      19
#define RX_DMA_SPORT_SHIFT_V2   26
#define RX_DMA_SPORT_MASK       0x7
#define RX_DMA_SPORT_MASK_V2    0xf

/* QDMA descriptor txd4 */
#define TX_DMA_CHKSUM		(0x7 << 29)
#define TX_DMA_TSO		BIT(28)
#define TX_DMA_FPORT_SHIFT	25
#define TX_DMA_FPORT_MASK	0x7
#define TX_DMA_INS_VLAN		BIT(16)

/* QDMA descriptor txd3 */
#define TX_DMA_OWNER_CPU	BIT(31)
#define TX_DMA_LS0		BIT(30)
#define TX_DMA_PLEN0(_x)	(((_x) & eth->soc->txrx.dma_max_len) <<	\
				 eth->soc->txrx.dma_len_offset)
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define TX_DMA_PLEN1(_x)	(((_x) & eth->soc->txrx.dma_max_len) << \
				 eth->soc->txrx.dma_len_offset)
#else
#define TX_DMA_PLEN1(_x)	((_x) & eth->soc->txrx.dma_max_len)
#endif
#define TX_DMA_SWC		BIT(14)
#define TX_DMA_ADDR64_MASK	GENMASK(3, 0)
#if IS_ENABLED(CONFIG_64BIT)
# define TX_DMA_GET_ADDR64(x)	(((u64)FIELD_GET(TX_DMA_ADDR64_MASK, (x))) << 32)
# define TX_DMA_PREP_ADDR64(x)	FIELD_PREP(TX_DMA_ADDR64_MASK, ((x) >> 32))
#else
# define TX_DMA_GET_ADDR64(x)	(0)
# define TX_DMA_PREP_ADDR64(x)	(0)
#endif

/* PDMA on MT7628 */
#define TX_DMA_DONE		BIT(31)
#define TX_DMA_LS1		BIT(14)
#define TX_DMA_DESP2_DEF	(TX_DMA_LS0 | TX_DMA_DONE)

/* QDMA descriptor rxd2 */
#define RX_DMA_DONE		BIT(31)
#define RX_DMA_LSO		BIT(30)
#if defined(CONFIG_MEDIATEK_NETSYS_RX_V2) || defined(CONFIG_MEDIATEK_NETSYS_V3)
#define RX_DMA_PLEN0(_x)	(((_x) & eth->soc->txrx.dma_max_len) << eth->soc->txrx.dma_len_offset)
#define RX_DMA_GET_PLEN0(_x)	(((_x) >> eth->soc->txrx.dma_len_offset) & eth->soc->txrx.dma_max_len)
#else
#define RX_DMA_PLEN0(_x)	\
	(((_x) & MTK_RX_DMA_BUF_LEN) << MTK_RX_DMA_BUF_SHIFT)
#define RX_DMA_GET_PLEN0(_x)	\
	(((_x) >> MTK_RX_DMA_BUF_SHIFT) & MTK_RX_DMA_BUF_LEN)
#endif

#define RX_DMA_GET_AGG_CNT(_x)	(((_x) >> 2) & 0xff)
#define RX_DMA_GET_REV(_x)	(((_x) >> 10) & 0x1f)
#define RX_DMA_VTAG		BIT(15)
#define RX_DMA_ADDR64_MASK	GENMASK(3, 0)
#if IS_ENABLED(CONFIG_64BIT)
# define RX_DMA_GET_ADDR64(x)	(((u64)FIELD_GET(RX_DMA_ADDR64_MASK, (x))) << 32)
# define RX_DMA_PREP_ADDR64(x)	FIELD_PREP(RX_DMA_ADDR64_MASK, ((x) >> 32))
#else
# define RX_DMA_GET_ADDR64(x)	(0)
# define RX_DMA_PREP_ADDR64(x)	(0)
#endif

/* QDMA descriptor rxd3 */
#define RX_DMA_VID(_x)		((_x) & VLAN_VID_MASK)
#define RX_DMA_TCI(_x)		((_x) & (VLAN_PRIO_MASK | VLAN_VID_MASK))
#define RX_DMA_VPID(_x)		(((_x) >> 16) & 0xffff)

/* QDMA descriptor rxd4 */
#define RX_DMA_L4_VALID		BIT(24)
#define RX_DMA_L4_VALID_PDMA	BIT(30)		/* when PDMA is used */
#define RX_DMA_SPECIAL_TAG	BIT(22)		/* switch header in packet */

#define RX_DMA_GET_SPORT(_x) 	(((_x) >> RX_DMA_SPORT_SHIFT) & RX_DMA_SPORT_MASK)
#define RX_DMA_GET_SPORT_V2(_x) (((_x) >> RX_DMA_SPORT_SHIFT_V2) & RX_DMA_SPORT_MASK_V2)

/* PDMA TX Num */
#define MTK_MAX_TX_RING_NUM	(4)
#define MTK_TX_NAPI_NUM		(4)
#define MTK_PDMA_TX_NUM		(4)

/* PDMA V2 descriptor txd4 */
#define TX_DMA_LS1_V2	BIT(30)

/* PDMA V2 descriptor txd5 */
#define TX_DMA_FPORT_SHIFT_PDMA	16

/* PDMA V2 descriptor rxd3 */
#define RX_DMA_VTAG_V2          BIT(0)
#define RX_DMA_L4_VALID_V2      BIT(2)

/* PDMA V2 descriptor rxd4 */
#define RX_DMA_VID_V2(_x)       RX_DMA_VID(_x)
#define RX_DMA_TCI_V2(_x)	RX_DMA_TCI(_x)
#define RX_DMA_VPID_V2(_x)	RX_DMA_VPID(_x)

/* PDMA V2 descriptor rxd6 */
#define RX_DMA_GET_FLUSH_RSN_V2(_x)	((_x) & 0x7)
#define RX_DMA_GET_AGG_CNT_V2(_x)	(((_x) >> 16) & 0xff)
#define RX_DMA_GET_TOPS_CRSN(_x)	(((_x) >> 24) & 0xff)

/* PDMA V2 descriptor rxd7 */
#define RX_DMA_GET_CDRT(_x)		(((_x) >> 8) & 0xff)

/* PHY Polling and SMI Master Control registers */
#define MTK_PPSC		0x10000
#define PPSC_MDC_CFG		GENMASK(29, 24)
#define PPSC_MDC_TURBO		BIT(20)
#define MDC_MAX_FREQ		25000000
#define MDC_MAX_DIVIDER		63

/* PHY Indirect Access Control registers */
#define MTK_PHY_IAC		0x10004
#define PHY_IAC_ACCESS		BIT(31)
#define PHY_IAC_READ		BIT(19)
#define PHY_IAC_READ_C45	(3 << 18)
#define PHY_IAC_ADDR_C45	(0 << 18)
#define PHY_IAC_WRITE		BIT(18)
#define PHY_IAC_START		BIT(16)
#define PHY_IAC_START_C45	(0 << 16)
#define PHY_IAC_ADDR_SHIFT	20
#define PHY_IAC_REG_SHIFT	25
#define PHY_IAC_TIMEOUT		HZ

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_MAC_MISC		0x10010
#else
#define MTK_MAC_MISC		0x1000c
#endif
#define MISC_MDC_TURBO		BIT(4)
#define MTK_MUX_TO_ESW		BIT(0)

/* XMAC status registers */
#define MTK_XGMAC_STS(x)	((x == MTK_GMAC3_ID) ? 0x1001C : 0x1000C)
#define MTK_XGMAC_FORCE_MODE(x)	((x == MTK_GMAC2_ID) ? BIT(31) : BIT(15))
#define MTK_XGMAC_FORCE_LINK(x)	((x == MTK_GMAC2_ID) ? BIT(27) : BIT(11))
#define MTK_USXGMII_PCS_LINK	BIT(8)
#define MTK_XGMAC_RX_FC		BIT(5)
#define MTK_XGMAC_TX_FC		BIT(4)
#define MTK_USXGMII_PCS_MODE	GENMASK(3, 1)
#define MTK_XGMAC_LINK_STS	BIT(0)

/* GSW bridge registers */
#define MTK_GSW_CFG		(0x10080)
#define GSWTX_IPG_MASK		GENMASK(19, 16)
#define GSWTX_IPG_SHIFT		16
#define GSWRX_IPG_MASK		GENMASK(3, 0)
#define GSWRX_IPG_SHIFT		0
#define GSW_IPG_11		11

/* Mac control registers */
#define MTK_MAC_MCR(x)		(0x10100 + (x * 0x100))
#define MAC_MCR_MAX_RX_MASK	GENMASK(25, 24)
#define MAC_MCR_MAX_RX(_x)	(MAC_MCR_MAX_RX_MASK & ((_x) << 24))
#define MAC_MCR_MAX_RX_1518	0x0
#define MAC_MCR_MAX_RX_1536	0x1
#define MAC_MCR_MAX_RX_1552	0x2
#define MAC_MCR_MAX_RX_2048	0x3
#define MAC_MCR_IPG_CFG		(BIT(18) | BIT(16) | BIT(12))
#define MAC_MCR_FORCE_MODE	BIT(15)
#define MAC_MCR_TX_EN		BIT(14)
#define MAC_MCR_RX_EN		BIT(13)
#define MAC_MCR_PRMBL_LMT_EN	BIT(10)
#define MAC_MCR_BACKOFF_EN	BIT(9)
#define MAC_MCR_BACKPR_EN	BIT(8)
#define MAC_MCR_FORCE_EEE1000	BIT(7)
#define MAC_MCR_FORCE_EEE100	BIT(6)
#define MAC_MCR_FORCE_RX_FC	BIT(5)
#define MAC_MCR_FORCE_TX_FC	BIT(4)
#define MAC_MCR_SPEED_1000	BIT(3)
#define MAC_MCR_SPEED_100	BIT(2)
#define MAC_MCR_FORCE_DPX	BIT(1)
#define MAC_MCR_FORCE_LINK	BIT(0)
#define MAC_MCR_FORCE_LINK_DOWN	(MAC_MCR_FORCE_MODE)

/* XFI Mac control registers */
#define MTK_XMAC_BASE(x)	(0x12000 + ((x - 1) * 0x1000))
#define MTK_XMAC_MCR(x)		(MTK_XMAC_BASE(x))
#define XMAC_MCR_TRX_DISABLE	0xf
#define XMAC_MCR_FORCE_TX_FC	BIT(5)
#define XMAC_MCR_FORCE_RX_FC	BIT(4)

/* XFI Mac Rx configuration registers */
#define MTK_XMAC_RX_CFG2(x)	(MTK_XMAC_MCR(x) + 0xd0)
#define MTK_XMAC_MAX_RX_MASK	GENMASK(13, 0)

/* XFI Mac logic reset registers */
#define MTK_XMAC_LOGIC_RST(x)	(MTK_XMAC_BASE(x) + 0x10)
#define XMAC_LOGIC_RST		BIT(0)

/* XFI Mac count global control */
#define MTK_XMAC_CNT_CTRL(x)	(MTK_XMAC_BASE(x) + 0x100)
#define XMAC_GLB_CNTCLR		BIT(0)

/* Mac EEE control registers */
#define MTK_MAC_EEE(x)		(0x10104 + (x * 0x100))
#define MAC_EEE_WAKEUP_TIME_1000	GENMASK(31, 24)
#define MAC_EEE_WAKEUP_TIME_100	GENMASK(23, 16)
#define MAC_EEE_LPI_TXIDLE_THD	GENMASK(15, 8)
#define MAC_EEE_RESV0		GENMASK(7, 4)
#define MAC_EEE_CKG_TXILDE	BIT(3)
#define MAC_EEE_CKG_RXLPI	BIT(2)
#define MAC_EEE_TX_DOWN_REQ	BIT(1)
#define MAC_EEE_LPI_MODE	BIT(0)

/* Mac status registers */
#define MTK_MAC_MSR(x)		(0x10108 + (x * 0x100))
#define MAC_MSR_EEE1G		BIT(7)
#define MAC_MSR_EEE100M		BIT(6)
#define MAC_MSR_RX_FC		BIT(5)
#define MAC_MSR_TX_FC		BIT(4)
#define MAC_MSR_SPEED_1000	BIT(3)
#define MAC_MSR_SPEED_100	BIT(2)
#define MAC_MSR_SPEED_MASK	(MAC_MSR_SPEED_1000 | MAC_MSR_SPEED_100)
#define MAC_MSR_DPX		BIT(1)
#define MAC_MSR_LINK		BIT(0)

/* TRGMII RXC control register */
#define TRGMII_RCK_CTRL		0x10300
#define DQSI0(x)		((x << 0) & GENMASK(6, 0))
#define DQSI1(x)		((x << 8) & GENMASK(14, 8))
#define RXCTL_DMWTLAT(x)	((x << 16) & GENMASK(18, 16))
#define RXC_RST			BIT(31)
#define RXC_DQSISEL		BIT(30)
#define RCK_CTRL_RGMII_1000	(RXC_DQSISEL | RXCTL_DMWTLAT(2) | DQSI1(16))
#define RCK_CTRL_RGMII_10_100	RXCTL_DMWTLAT(2)

#define NUM_TRGMII_CTRL		5

/* TRGMII RXC control register */
#define TRGMII_TCK_CTRL		0x10340
#define TXCTL_DMWTLAT(x)	((x << 16) & GENMASK(18, 16))
#define TXC_INV			BIT(30)
#define TCK_CTRL_RGMII_1000	TXCTL_DMWTLAT(2)
#define TCK_CTRL_RGMII_10_100	(TXC_INV | TXCTL_DMWTLAT(2))

/* TRGMII TX Drive Strength */
#define TRGMII_TD_ODT(i)	(0x10354 + 8 * (i))
#define  TD_DM_DRVP(x)		((x) & 0xf)
#define  TD_DM_DRVN(x)		(((x) & 0xf) << 4)

/* TRGMII Interface mode register */
#define INTF_MODE		0x10390
#define TRGMII_INTF_DIS		BIT(0)
#define TRGMII_MODE		BIT(1)
#define TRGMII_CENTRAL_ALIGNED	BIT(2)
#define INTF_MODE_RGMII_1000    (TRGMII_MODE | TRGMII_CENTRAL_ALIGNED)
#define INTF_MODE_RGMII_10_100  0

/* GPIO port control registers for GMAC 2*/
#define GPIO_OD33_CTRL8		0x4c0
#define GPIO_BIAS_CTRL		0xed0
#define GPIO_DRV_SEL10		0xf00

/* SoC hardware version register */
#define HWVER_BIT_NETSYS_1_2	BIT(0)
#define HWVER_BIT_NETSYS_3	BIT(8)

/* ethernet subsystem chip id register */
#define ETHSYS_CHIPID0_3	0x0
#define ETHSYS_CHIPID4_7	0x4
#define MT7623_ETH		7623
#define MT7622_ETH		7622
#define MT7621_ETH		7621

/* ethernet system control register */
#define ETHSYS_SYSCFG		0x10
#define SYSCFG_DRAM_TYPE_DDR2	BIT(4)

/* ethernet subsystem config register */
#define ETHSYS_SYSCFG0		0x14
#define SYSCFG0_GE_MASK		0x3
#define SYSCFG0_GE_MODE(x, y)	(x << (12 + (y * 2)))
#define SYSCFG0_SGMII_MASK     GENMASK(9, 7)
#define SYSCFG0_SGMII_GMAC1    ((2 << 8) & SYSCFG0_SGMII_MASK)
#define SYSCFG0_SGMII_GMAC2    ((3 << 8) & SYSCFG0_SGMII_MASK)
#define SYSCFG0_SGMII_GMAC1_V2 BIT(9)
#define SYSCFG0_SGMII_GMAC2_V2 BIT(8)
#define SYSCFG0_SGMII_GMAC3_V2 BIT(7)


/* ethernet subsystem clock register */
#define ETHSYS_CLKCFG0		0x2c
#define ETHSYS_TRGMII_CLK_SEL362_5	BIT(11)
#define ETHSYS_TRGMII_MT7621_MASK	(BIT(5) | BIT(6))
#define ETHSYS_TRGMII_MT7621_APLL	BIT(6)
#define ETHSYS_TRGMII_MT7621_DDR_PLL	BIT(5)

/* ethernet reset control register */
#define ETHSYS_RSTCTRL	0x34
#define RSTCTRL_FE	BIT(6)
#define RSTCTRL_ETH 	BIT(23)
#if defined(CONFIG_MEDIATEK_NETSYS_V2)
#define RSTCTRL_PPE0	BIT(30)
#define RSTCTRL_PPE1 	BIT(31)
#elif defined(CONFIG_MEDIATEK_NETSYS_V3)
#define RSTCTRL_PPE0	BIT(29)
#define RSTCTRL_PPE1	BIT(30)
#define RSTCTRL_PPE2	BIT(31)
#define RSTCTRL_WDMA0	BIT(24)
#define RSTCTRL_WDMA1	BIT(25)
#define RSTCTRL_WDMA2	BIT(26)
#else
#define RSTCTRL_PPE0	BIT(31)
#define RSTCTRL_PPE1	0
#endif

/* ethernet reset check idle register */
#define ETHSYS_FE_RST_CHK_IDLE_EN 	0x28

/* ethernet non-idle check register */
#define ETHSYS_LP_NONE_IDLE_LAT0 (0x144)
#define ETHSYS_LP_NONE_IDLE_LAT1 (0x148)

/* ethernet dma channel agent map */
#define ETHSYS_DMA_AG_MAP	0x408
#define ETHSYS_DMA_AG_MAP_PDMA	BIT(0)
#define ETHSYS_DMA_AG_MAP_QDMA	BIT(1)
#define ETHSYS_DMA_AG_MAP_PPE	BIT(2)

/* SGMII subsystem config registers */
#define SGMSYS_PCS_CONTROL_1	0x0
#define SGMII_BMSR		GENMASK(31, 16)
#define SGMII_AN_RESTART	BIT(9)
#define SGMII_ISOLATE		BIT(10)
#define SGMII_AN_ENABLE		BIT(12)
#define SGMII_LINK_STATYS	BIT(18)
#define SGMII_AN_ABILITY	BIT(19)
#define SGMII_AN_COMPLETE	BIT(21)
#define SGMII_PCS_FAULT		BIT(23)
#define SGMII_AN_EXPANSION_CLR	BIT(30)

/* Register to set SGMII speed */
#define SGMSYS_PCS_ADVERTISE	0x08
#define SGMII_ADVERTISE		GENMASK(15, 0)
#define SGMII_LPA		GENMASK(31, 16)
#define SGMII_LPA_SPEED_MASK	GENMASK(11, 10)
#define SGMII_LPA_SPEED_10	0
#define SGMII_LPA_SPEED_100	1
#define SGMII_LPA_SPEED_1000	2
#define SGMII_LPA_DUPLEX	BIT(12)
#define SGMII_LPA_LINK		BIT(15)

/* Register to programmable link timer, the unit in 2 * 8ns */
#define SGMSYS_PCS_LINK_TIMER	0x18
#define SGMII_LINK_TIMER_DEFAULT	(0x186a0 & GENMASK(19, 0))

/* Register to control remote fault */
#define SGMSYS_SGMII_MODE		0x20
#define SGMII_IF_MODE_SGMII		BIT(0)
#define SGMII_SPEED_DUPLEX_AN		BIT(1)
#define SGMII_SPEED_MASK		GENMASK(3, 2)
#define SGMII_SPEED_10			0x0
#define SGMII_SPEED_100			BIT(2)
#define SGMII_SPEED_1000		BIT(3)
#define SGMII_DUPLEX_HALF		BIT(4)
#define SGMII_IF_MODE_BIT5		BIT(5)
#define SGMII_REMOTE_FAULT_DIS		BIT(8)
#define SGMII_CODE_SYNC_SET_VAL		BIT(9)
#define SGMII_CODE_SYNC_SET_EN		BIT(10)
#define SGMII_SEND_AN_ERROR_EN		BIT(11)
#define SGMII_IF_MODE_MASK		GENMASK(5, 1)

/* Register to reset SGMII design */
#define SGMII_RESERVED_0	0x34
#define SGMII_SW_RESET		BIT(0)

/* Register to reset SGMII design */
#define SGMII_RESERVED_0	0x34
#define SGMII_SW_RESET		BIT(0)

/* Register to set SGMII speed, ANA RG_ Control Signals III*/
#define SGMSYS_ANA_RG_CS3	0x2028
#define RG_PHY_SPEED_MASK	(BIT(2) | BIT(3))
#define RG_PHY_SPEED_1_25G	0x0
#define RG_PHY_SPEED_3_125G	BIT(2)

/* Register to power up QPHY */
#define SGMSYS_QPHY_PWR_STATE_CTRL 0xe8
#define	SGMII_PHYA_PWD		BIT(4)

/* Register to QPHY wrapper control */
#define SGMSYS_QPHY_WRAP_CTRL	0xec
#define SGMII_PN_SWAP_MASK	GENMASK(1, 0)
#define SGMII_PN_SWAP_RX	BIT(1)
#define SGMII_PN_SWAP_TX	BIT(0)

/* USXGMII subsystem config registers */
/* Register to control speed */
#define RG_PHY_TOP_SPEED_CTRL1	0x80C
#define USXGMII_RATE_UPDATE_MODE	BIT(31)
#define USXGMII_MAC_CK_GATED	BIT(29)
#define USXGMII_IF_FORCE_EN	BIT(28)
#define USXGMII_RATE_ADAPT_MODE	GENMASK(10, 8)
#define USXGMII_RATE_ADAPT_MODE_X1	0
#define USXGMII_RATE_ADAPT_MODE_X2	1
#define USXGMII_RATE_ADAPT_MODE_X4	2
#define USXGMII_RATE_ADAPT_MODE_X10	3
#define USXGMII_RATE_ADAPT_MODE_X100	4
#define USXGMII_RATE_ADAPT_MODE_X5	5
#define USXGMII_RATE_ADAPT_MODE_X50	6
#define USXGMII_XFI_RX_MODE	GENMASK(6, 4)
#define USXGMII_XFI_RX_MODE_10G	0
#define USXGMII_XFI_RX_MODE_5G	1
#define USXGMII_XFI_TX_MODE	GENMASK(2, 0)
#define USXGMII_XFI_TX_MODE_10G	0
#define USXGMII_XFI_TX_MODE_5G	1

/* Register to control PCS AN */
#define RG_PCS_AN_CTRL0		0x810
#define USXGMII_AN_RESTART	BIT(31)
#define USXGMII_AN_SYNC_CNT	GENMASK(30, 11)
#define USXGMII_AN_ENABLE	BIT(0)

#define RG_PCS_AN_CTRL2		0x818
#define USXGMII_LINK_TIMER_IDLE_DETECT	GENMASK(29, 20)
#define USXGMII_LINK_TIMER_COMP_ACK_DETECT	GENMASK(19, 10)
#define USXGMII_LINK_TIMER_AN_RESTART	GENMASK(9, 0)

/* Register to read PCS AN status */
#define RG_PCS_AN_STS0		0x81C
#define USXGMII_LPA_SPEED_MASK	GENMASK(11, 9)
#define USXGMII_LPA_SPEED_10	0
#define USXGMII_LPA_SPEED_100	1
#define USXGMII_LPA_SPEED_1000	2
#define USXGMII_LPA_SPEED_10000	3
#define USXGMII_LPA_SPEED_2500	4
#define USXGMII_LPA_SPEED_5000	5
#define USXGMII_LPA_DUPLEX	BIT(12)
#define USXGMII_LPA_LINK	BIT(15)
#define USXGMII_LPA_LATCH	BIT(31)

/* Register to QPHY wrapper control */
#define RG_PHY_TOP_CTRL0	0x82C
#define USXGMII_PN_SWAP_MASK	GENMASK(1, 0)
#define USXGMII_PN_SWAP_RX	BIT(1)
#define USXGMII_PN_SWAP_TX	BIT(0)

/* Register to read PCS Link status */
#define RG_PCS_RX_STATUS0	0x904
#define RG_PCS_RX_STATUS_UPDATE	BIT(16)
#define RG_PCS_RX_LINK_STATUS	BIT(2)

/* Register to control USXGMII XFI PLL digital */
#define XFI_PLL_DIG_GLB8	0x08
#define RG_XFI_PLL_EN		BIT(31)

/* Register to control USXGMII XFI PLL analog */
#define XFI_PLL_ANA_GLB8	0x108
#define RG_XFI_PLL_ANA_SWWA	0x02283248

/* Infrasys subsystem config registers */
#define INFRA_MISC2            0x70c
#define CO_QPHY_SEL            BIT(0)
#define GEPHY_MAC_SEL          BIT(1)

/* Toprgu subsystem config registers */
#define TOPRGU_SWSYSRST		0x18
#define SWSYSRST_UNLOCK_KEY	GENMASK(31, 24)
#define SWSYSRST_XFI_PLL_GRST	BIT(16)
#define SWSYSRST_XFI_PEXPT1_GRST	BIT(15)
#define SWSYSRST_XFI_PEXPT0_GRST	BIT(14)
#define SWSYSRST_XFI1_GRST	BIT(13)
#define SWSYSRST_XFI0_GRST	BIT(12)
#define SWSYSRST_SGMII1_GRST	BIT(2)
#define SWSYSRST_SGMII0_GRST	BIT(1)
#define TOPRGU_SWSYSRST_EN		0xFC

/* Top misc registers */
#define TOP_MISC_NETSYS_PCS_MUX	0x84
#define NETSYS_PCS_MUX_MASK	GENMASK(1, 0)
#define	MUX_G2_USXGMII_SEL	BIT(1)
#define MUX_HSGMII1_G1_SEL	BIT(0)
#define USB_PHY_SWITCH_REG     0x218
#define QPHY_SEL_MASK          GENMASK(1, 0)
#define SGMII_QPHY_SEL	       0x2

/*MDIO control*/
#define MII_MMD_ACC_CTL_REG             0x0d
#define MII_MMD_ADDR_DATA_REG           0x0e
#define MMD_OP_MODE_DATA BIT(14)

/* MT7628/88 specific stuff */
#define MT7628_PDMA_OFFSET	0x0800
#define MT7628_SDM_OFFSET	0x0c00

#define MT7628_TX_BASE_PTR0	(MT7628_PDMA_OFFSET + 0x00)
#define MT7628_TX_MAX_CNT0	(MT7628_PDMA_OFFSET + 0x04)
#define MT7628_TX_CTX_IDX0	(MT7628_PDMA_OFFSET + 0x08)
#define MT7628_TX_DTX_IDX0	(MT7628_PDMA_OFFSET + 0x0c)
#define MT7628_PST_DTX_IDX0	BIT(0)

#define MT7628_SDM_MAC_ADRL	(MT7628_SDM_OFFSET + 0x0c)
#define MT7628_SDM_MAC_ADRH	(MT7628_SDM_OFFSET + 0x10)

#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#if !defined(CONFIG_NET_MEDIATEK_HNAT) && !defined(CONFIG_NET_MEDIATEK_HNAT_MODULE)
struct tnl_desc {
	u32 entry : 15;
	u32 filled : 3;
	u32 crsn : 5;
	u32 resv1 : 3;
	u32 sport : 4;
	u32 resv2 : 1;
	u32 alg : 1;
	u32 iface : 8;
	u32 wdmaid : 2;
	u32 rxid : 2;
	u32 wcid : 16;
	u32 bssid : 8;
	u32 usr_info : 16;
	u32 tid : 4;
	u32 is_fixedrate : 1;
	u32 is_prior : 1;
	u32 is_sp : 1;
	u32 hf : 1;
	u32 amsdu : 1;
	u32 tops : 6;
	u32 is_decap : 1;
	u32 cdrt : 8;
	u32 resv3 : 4;
	u32 magic_tag_protect : 16;
} __packed;

#define TNL_MAGIC_TAG 0x6789
#define skb_tnl_cdrt(skb) (((struct tnl_desc *)((skb)->head))->cdrt)
#define skb_tnl_set_cdrt(skb, cdrt) ((skb_tnl_cdrt(skb)) = (cdrt))
#define skb_tnl_magic_tag(skb) (((struct tnl_desc *)((skb)->head))->magic_tag_protect)
#define is_tnl_tag_valid(skb) (skb_tnl_magic_tag(skb) == TNL_MAGIC_TAG)
#else /* defined(CONFIG_NET_MEDIATEK_HNAT) || defined(CONFIG_NET_MEDIATEK_HNAT_MODULE) */
#define skb_tnl_cdrt(skb) (0)
#define skb_tnl_set_cdrt(skb, cdrt) (0)
#define skb_tnl_magic_tag(skb) (0)
#define is_tnl_tag_valid(skb) (0)
#endif /* !defined(CONFIG_NET_MEDIATEK_HNAT) && !defined(CONFIG_NET_MEDIATEK_HNAT_MODULE) */
#endif /* defined(CONFIG_MEDIATEK_NETSYS_V3) */

struct mtk_rx_dma {
	unsigned int rxd1;
	unsigned int rxd2;
	unsigned int rxd3;
	unsigned int rxd4;
} __packed __aligned(4);

struct mtk_rx_dma_v2 {
	unsigned int rxd1;
	unsigned int rxd2;
	unsigned int rxd3;
	unsigned int rxd4;
	unsigned int rxd5;
	unsigned int rxd6;
	unsigned int rxd7;
	unsigned int rxd8;
} __packed __aligned(4);

struct mtk_tx_dma {
	unsigned int txd1;
	unsigned int txd2;
	unsigned int txd3;
	unsigned int txd4;
} __packed __aligned(4);

struct mtk_tx_dma_v2 {
	unsigned int txd1;
	unsigned int txd2;
	unsigned int txd3;
	unsigned int txd4;
	unsigned int txd5;
	unsigned int txd6;
	unsigned int txd7;
	unsigned int txd8;
} __packed __aligned(4);

struct mtk_eth;
struct mtk_mac;
struct mtk_mux;

/* struct mtk_hw_stats - the structure that holds the traffic statistics.
 * @stats_lock:		make sure that stats operations are atomic
 * @reg_offset:		the status register offset of the SoC
 * @syncp:		the refcount
 *
 * All of the supported SoCs have hardware counters for traffic statistics.
 * Whenever the status IRQ triggers we can read the latest stats from these
 * counters and store them in this struct.
 */
struct mtk_hw_stats {
	u64 tx_bytes;
	u64 tx_packets;
	u64 tx_skip;
	u64 tx_collisions;
	u64 rx_bytes;
	u64 rx_packets;
	u64 rx_overflow;
	u64 rx_fcs_errors;
	u64 rx_short_errors;
	u64 rx_long_errors;
	u64 rx_checksum_errors;
	u64 rx_flow_control_packets;

	spinlock_t		stats_lock;
	u32			reg_offset;
	struct u64_stats_sync	syncp;
};

enum mtk_tx_flags {
	/* PDMA descriptor can point at 1-2 segments. This enum allows us to
	 * track how memory was allocated so that it can be freed properly.
	 */
	MTK_TX_FLAGS_SINGLE0	= 0x01,
	MTK_TX_FLAGS_PAGE0	= 0x02,

	/* MTK_TX_FLAGS_FPORTx allows tracking which port the transmitted
	 * SKB out instead of looking up through hardware TX descriptor.
	 */
	MTK_TX_FLAGS_FPORT0	= 0x04,
	MTK_TX_FLAGS_FPORT1	= 0x08,
	MTK_TX_FLAGS_FPORT2	= 0x10,
};

/* This enum allows us to identify how the clock is defined on the array of the
 * clock in the order
 */
enum mtk_clks_map {
	MTK_CLK_ETHIF,
	MTK_CLK_SGMIITOP,
	MTK_CLK_ESW,
	MTK_CLK_GP0,
	MTK_CLK_GP1,
	MTK_CLK_GP2,
	MTK_CLK_GP3,
	MTK_CLK_XGP1,
	MTK_CLK_XGP2,
	MTK_CLK_XGP3,
	MTK_CLK_CRYPTO,
	MTK_CLK_FE,
	MTK_CLK_TRGPLL,
	MTK_CLK_SGMII_TX_250M,
	MTK_CLK_SGMII_RX_250M,
	MTK_CLK_SGMII_CDR_REF,
	MTK_CLK_SGMII_CDR_FB,
	MTK_CLK_SGMII2_TX_250M,
	MTK_CLK_SGMII2_RX_250M,
	MTK_CLK_SGMII2_CDR_REF,
	MTK_CLK_SGMII2_CDR_FB,
	MTK_CLK_SGMII_CK,
	MTK_CLK_ETH2PLL,
	MTK_CLK_WOCPU0,
	MTK_CLK_WOCPU1,
	MTK_CLK_ETHWARP_WOCPU2,
	MTK_CLK_ETHWARP_WOCPU1,
	MTK_CLK_ETHWARP_WOCPU0,
	MTK_CLK_TOP_USXGMII_SBUS_0_SEL,
	MTK_CLK_TOP_USXGMII_SBUS_1_SEL,
	MTK_CLK_TOP_SGM_0_SEL,
	MTK_CLK_TOP_SGM_1_SEL,
	MTK_CLK_TOP_XFI_PHY_0_XTAL_SEL,
	MTK_CLK_TOP_XFI_PHY_1_XTAL_SEL,
	MTK_CLK_TOP_ETH_GMII_SEL,
	MTK_CLK_TOP_ETH_REFCK_50M_SEL,
	MTK_CLK_TOP_ETH_SYS_200M_SEL,
	MTK_CLK_TOP_ETH_SYS_SEL,
	MTK_CLK_TOP_ETH_XGMII_SEL,
	MTK_CLK_TOP_ETH_MII_SEL,
	MTK_CLK_TOP_NETSYS_SEL,
	MTK_CLK_TOP_NETSYS_500M_SEL,
	MTK_CLK_TOP_NETSYS_PAO_2X_SEL,
	MTK_CLK_TOP_NETSYS_SYNC_250M_SEL,
	MTK_CLK_TOP_NETSYS_PPEFB_250M_SEL,
	MTK_CLK_TOP_NETSYS_WARP_SEL,
	MTK_CLK_TOP_MACSEC_SEL,
	MTK_CLK_MAX
};

#define MT7623_CLKS_BITMAP	(BIT(MTK_CLK_ETHIF) | BIT(MTK_CLK_ESW) |  \
				 BIT(MTK_CLK_GP1) | BIT(MTK_CLK_GP2) | \
				 BIT(MTK_CLK_TRGPLL))
#define MT7622_CLKS_BITMAP	(BIT(MTK_CLK_ETHIF) | BIT(MTK_CLK_ESW) |  \
				 BIT(MTK_CLK_GP0) | BIT(MTK_CLK_GP1) | \
				 BIT(MTK_CLK_GP2) | \
				 BIT(MTK_CLK_SGMII_TX_250M) | \
				 BIT(MTK_CLK_SGMII_RX_250M) | \
				 BIT(MTK_CLK_SGMII_CDR_REF) | \
				 BIT(MTK_CLK_SGMII_CDR_FB) | \
				 BIT(MTK_CLK_SGMII_CK) | \
				 BIT(MTK_CLK_ETH2PLL))
#define MT7621_CLKS_BITMAP	(0)
#define MT7628_CLKS_BITMAP	(0)
#define MT7629_CLKS_BITMAP	(BIT(MTK_CLK_ETHIF) | BIT(MTK_CLK_ESW) |  \
				 BIT(MTK_CLK_GP0) | BIT(MTK_CLK_GP1) | \
				 BIT(MTK_CLK_GP2) | BIT(MTK_CLK_FE) | \
				 BIT(MTK_CLK_SGMII_TX_250M) | \
				 BIT(MTK_CLK_SGMII_RX_250M) | \
				 BIT(MTK_CLK_SGMII_CDR_REF) | \
				 BIT(MTK_CLK_SGMII_CDR_FB) | \
				 BIT(MTK_CLK_SGMII2_TX_250M) | \
				 BIT(MTK_CLK_SGMII2_RX_250M) | \
				 BIT(MTK_CLK_SGMII2_CDR_REF) | \
				 BIT(MTK_CLK_SGMII2_CDR_FB) | \
				 BIT(MTK_CLK_SGMII_CK) | \
				 BIT(MTK_CLK_ETH2PLL) | BIT(MTK_CLK_SGMIITOP))

#define MT7986_CLKS_BITMAP	(BIT(MTK_CLK_FE) | BIT(MTK_CLK_GP2) | BIT(MTK_CLK_GP1) | \
                                 BIT(MTK_CLK_WOCPU1) | BIT(MTK_CLK_WOCPU0) | \
                                 BIT(MTK_CLK_SGMII_TX_250M) | \
                                 BIT(MTK_CLK_SGMII_RX_250M) | \
                                 BIT(MTK_CLK_SGMII_CDR_REF) | \
                                 BIT(MTK_CLK_SGMII_CDR_FB) | \
                                 BIT(MTK_CLK_SGMII2_TX_250M) | \
                                 BIT(MTK_CLK_SGMII2_RX_250M) | \
                                 BIT(MTK_CLK_SGMII2_CDR_REF) | \
                                 BIT(MTK_CLK_SGMII2_CDR_FB))


#define MT7981_CLKS_BITMAP	(BIT(MTK_CLK_FE) | BIT(MTK_CLK_GP2) | BIT(MTK_CLK_GP1) | \
                                 BIT(MTK_CLK_WOCPU0) | \
                                 BIT(MTK_CLK_SGMII_TX_250M) | \
                                 BIT(MTK_CLK_SGMII_RX_250M) | \
                                 BIT(MTK_CLK_SGMII_CDR_REF) | \
                                 BIT(MTK_CLK_SGMII_CDR_FB) | \
                                 BIT(MTK_CLK_SGMII2_TX_250M) | \
                                 BIT(MTK_CLK_SGMII2_RX_250M) | \
                                 BIT(MTK_CLK_SGMII2_CDR_REF) | \
                                 BIT(MTK_CLK_SGMII2_CDR_FB))

#define MT7988_CLKS_BITMAP	(BIT(MTK_CLK_FE) | BIT(MTK_CLK_ESW) | \
				 BIT(MTK_CLK_GP1) | BIT(MTK_CLK_GP2) | \
				 BIT(MTK_CLK_GP3) | BIT(MTK_CLK_XGP1) | \
				 BIT(MTK_CLK_XGP2) | BIT(MTK_CLK_XGP3) | \
				 BIT(MTK_CLK_CRYPTO) | \
				 BIT(MTK_CLK_SGMII_TX_250M) | \
				 BIT(MTK_CLK_SGMII_RX_250M) | \
				 BIT(MTK_CLK_SGMII2_TX_250M) | \
				 BIT(MTK_CLK_SGMII2_RX_250M) | \
				 BIT(MTK_CLK_ETHWARP_WOCPU2) | \
				 BIT(MTK_CLK_ETHWARP_WOCPU1) | \
				 BIT(MTK_CLK_ETHWARP_WOCPU0) | \
				 BIT(MTK_CLK_TOP_USXGMII_SBUS_0_SEL) | \
				 BIT(MTK_CLK_TOP_USXGMII_SBUS_1_SEL) | \
				 BIT(MTK_CLK_TOP_SGM_0_SEL) | \
				 BIT(MTK_CLK_TOP_SGM_1_SEL) | \
				 BIT(MTK_CLK_TOP_XFI_PHY_0_XTAL_SEL) | \
				 BIT(MTK_CLK_TOP_XFI_PHY_1_XTAL_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_GMII_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_REFCK_50M_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_SYS_200M_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_SYS_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_XGMII_SEL) | \
				 BIT(MTK_CLK_TOP_ETH_MII_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_500M_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_PAO_2X_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_SYNC_250M_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_PPEFB_250M_SEL) | \
				 BIT(MTK_CLK_TOP_NETSYS_WARP_SEL) | \
				 BIT(MTK_CLK_TOP_MACSEC_SEL))

enum mtk_dev_state {
	MTK_HW_INIT,
	MTK_RESETTING
};

/* PSE Port Definition */
enum mtk_pse_port {
	PSE_ADMA_PORT = 0,
	PSE_GDM1_PORT,
	PSE_GDM2_PORT,
	PSE_PPE0_PORT,
	PSE_PPE1_PORT,
	PSE_QDMA_TX_PORT,
	PSE_QDMA_RX_PORT,
	PSE_DROP_PORT,
	PSE_WDMA0_PORT,
	PSE_WDMA1_PORT,
	PSE_TDMA_PORT,
	PSE_NONE_PORT,
	PSE_PPE2_PORT,
	PSE_WDMA2_PORT,
	PSE_EIP197_PORT,
	PSE_GDM3_PORT,
	PSE_PORT_MAX
};

/* GMAC Identifier */
enum mtk_gmac_id {
	MTK_GMAC1_ID = 0,
	MTK_GMAC2_ID,
	MTK_GMAC3_ID,
	MTK_GMAC_ID_MAX
};

/* GDM Type */
enum mtk_gdm_type {
	MTK_GDM_TYPE = 0,
	MTK_XGDM_TYPE,
	MTK_GDM_TYPE_MAX
};

enum mtk_hw_id {
	MTK_HWID_V1 = 0,
	MTK_HWID_V2,
	MTK_HWID_MAX
};

static inline const char *gdm_type(int type)
{
	switch (type) {
	case MTK_GDM_TYPE:
		return "gdm";
	case MTK_XGDM_TYPE:
		return "xgdm";
	default:
		return "unkown";
	}
}

/* struct mtk_tx_buf -	This struct holds the pointers to the memory pointed at
 *			by the TX descriptor	s
 * @skb:		The SKB pointer of the packet being sent
 * @dma_addr0:		The base addr of the first segment
 * @dma_len0:		The length of the first segment
 * @dma_addr1:		The base addr of the second segment
 * @dma_len1:		The length of the second segment
 */
struct mtk_tx_buf {
	struct sk_buff *skb;
	u32 flags;
	DEFINE_DMA_UNMAP_ADDR(dma_addr0);
	DEFINE_DMA_UNMAP_LEN(dma_len0);
	DEFINE_DMA_UNMAP_ADDR(dma_addr1);
	DEFINE_DMA_UNMAP_LEN(dma_len1);
};

/* struct mtk_tx_ring -	This struct holds info describing a TX ring
 * @dma:		The descriptor ring
 * @buf:		The memory pointed at by the ring
 * @phys:		The physical addr of tx_buf
 * @next_free:		Pointer to the next free descriptor
 * @last_free:		Pointer to the last free descriptor
 * @last_free_ptr:	Hardware pointer value of the last free descriptor
 * @thresh:		The threshold of minimum amount of free descriptors
 * @free_count:		QDMA uses a linked list. Track how many free descriptors
 *			are present
 */
struct mtk_tx_ring {
	void *dma;
	struct mtk_tx_buf *buf;
	dma_addr_t phys;
	void *next_free;
	void *last_free;
	u32 last_free_ptr;
	u32 ring_no;
	u16 thresh;
	atomic_t free_count;
	int dma_size;
	void *dma_pdma;	/* For MT7628/88 PDMA handling */
	dma_addr_t phys_pdma;
	int cpu_idx;
};

/* PDMA rx ring mode */
enum mtk_rx_flags {
	MTK_RX_FLAGS_NORMAL = 0,
	MTK_RX_FLAGS_HWLRO,
	MTK_RX_FLAGS_QDMA,
};

/* struct mtk_rx_ring -	This struct holds info describing a RX ring
 * @dma:		The descriptor ring
 * @data:		The memory pointed at by the ring
 * @phys:		The physical addr of rx_buf
 * @frag_size:		How big can each fragment be
 * @buf_size:		The size of each packet buffer
 * @calc_idx:		The current head of ring
 * @ring_no:		The index of ring
 */
struct mtk_rx_ring {
	void *dma;
	u8 **data;
	dma_addr_t phys;
	u16 frag_size;
	u16 buf_size;
	u16 dma_size;
	bool calc_idx_update;
	u16 calc_idx;
	u32 crx_idx_reg;
	u32 ring_no;
};

/* struct mtk_rss_params -	This is the structure holding parameters
				for the RSS ring
 * @hash_key			The element is used to record the
				secret key for the RSS ring
 * indirection_table		The element is used to record the
				indirection table for the RSS ring
 */
struct mtk_rss_params {
	u32		hash_key[MTK_RSS_HASH_KEYSIZE / sizeof(u32)];
	u8		indirection_table[MTK_RSS_MAX_INDIRECTION_TABLE];
};

/* struct mtk_napi -	This is the structure holding NAPI-related information,
 *			and a mtk_napi struct is binding to one interrupt group
 * @napi:		The NAPI struct
 * @rx_ring:		Pointer to the memory holding info about the RX ring
 * @irq_grp_idx:	The index indicates which interrupt group that this
 *			mtk_napi is binding to
 */
struct mtk_napi {
	struct napi_struct	napi;
	struct mtk_eth		*eth;
	struct mtk_tx_ring	*tx_ring;
	struct mtk_rx_ring	*rx_ring;
	u32			irq_grp_no;
};

enum mkt_eth_capabilities {
	MTK_RGMII_BIT = 0,
	MTK_TRGMII_BIT,
	MTK_SGMII_BIT,
	MTK_USXGMII_BIT,
	MTK_2P5GPHY_BIT,
	MTK_ESW_BIT,
	MTK_GEPHY_BIT,
	MTK_MUX_BIT,
	MTK_INFRA_BIT,
	MTK_SHARED_SGMII_BIT,
	MTK_HWLRO_BIT,
	MTK_RSS_BIT,
	MTK_SHARED_INT_BIT,
	MTK_PDMA_INT_BIT,
	MTK_TRGMII_MT7621_CLK_BIT,
	MTK_QDMA_BIT,
	MTK_NETSYS_V1_BIT,
	MTK_NETSYS_V2_BIT,
	MTK_NETSYS_RX_V2_BIT,
	MTK_NETSYS_V3_BIT,
	MTK_SOC_MT7628_BIT,
	MTK_RSTCTRL_PPE1_BIT,
	MTK_RSTCTRL_PPE2_BIT,
	MTK_U3_COPHY_V2_BIT,
	MTK_36BIT_DMA_BIT,

	/* MUX BITS*/
	MTK_ETH_MUX_GDM1_TO_GMAC1_ESW_BIT,
	MTK_ETH_MUX_GMAC2_GMAC0_TO_GEPHY_BIT,
	MTK_ETH_MUX_U3_GMAC2_TO_QPHY_BIT,
	MTK_ETH_MUX_GMAC2_TO_2P5GPHY_BIT,
	MTK_ETH_MUX_GMAC1_GMAC2_TO_SGMII_RGMII_BIT,
	MTK_ETH_MUX_GMAC12_TO_GEPHY_SGMII_BIT,
	MTK_ETH_MUX_GMAC123_TO_GEPHY_SGMII_BIT,
	MTK_ETH_MUX_GMAC123_TO_USXGMII_BIT,

	/* PATH BITS */
	MTK_ETH_PATH_GMAC1_RGMII_BIT,
	MTK_ETH_PATH_GMAC1_TRGMII_BIT,
	MTK_ETH_PATH_GMAC1_SGMII_BIT,
	MTK_ETH_PATH_GMAC2_RGMII_BIT,
	MTK_ETH_PATH_GMAC2_SGMII_BIT,
	MTK_ETH_PATH_GMAC2_2P5GPHY_BIT,
	MTK_ETH_PATH_GMAC2_GEPHY_BIT,
	MTK_ETH_PATH_GMAC3_SGMII_BIT,
	MTK_ETH_PATH_GDM1_ESW_BIT,
	MTK_ETH_PATH_GMAC1_USXGMII_BIT,
	MTK_ETH_PATH_GMAC2_USXGMII_BIT,
	MTK_ETH_PATH_GMAC3_USXGMII_BIT,
};

/* Supported hardware group on SoCs */
#define MTK_RGMII		BIT_ULL(MTK_RGMII_BIT)
#define MTK_TRGMII		BIT_ULL(MTK_TRGMII_BIT)
#define MTK_SGMII		BIT_ULL(MTK_SGMII_BIT)
#define MTK_USXGMII		BIT_ULL(MTK_USXGMII_BIT)
#define MTK_2P5GPHY		BIT_ULL(MTK_2P5GPHY_BIT)
#define MTK_ESW			BIT_ULL(MTK_ESW_BIT)
#define MTK_GEPHY		BIT_ULL(MTK_GEPHY_BIT)
#define MTK_MUX			BIT_ULL(MTK_MUX_BIT)
#define MTK_INFRA		BIT_ULL(MTK_INFRA_BIT)
#define MTK_SHARED_SGMII	BIT_ULL(MTK_SHARED_SGMII_BIT)
#define MTK_HWLRO		BIT_ULL(MTK_HWLRO_BIT)
#define MTK_RSS			BIT_ULL(MTK_RSS_BIT)
#define MTK_SHARED_INT		BIT_ULL(MTK_SHARED_INT_BIT)
#define MTK_PDMA_INT		BIT_ULL(MTK_PDMA_INT_BIT)
#define MTK_TRGMII_MT7621_CLK	BIT_ULL(MTK_TRGMII_MT7621_CLK_BIT)
#define MTK_QDMA		BIT_ULL(MTK_QDMA_BIT)
#define MTK_NETSYS_V1		BIT_ULL(MTK_NETSYS_V1_BIT)
#define MTK_NETSYS_V2		BIT_ULL(MTK_NETSYS_V2_BIT)
#define MTK_NETSYS_RX_V2	BIT(MTK_NETSYS_RX_V2_BIT)
#define MTK_NETSYS_V3		BIT_ULL(MTK_NETSYS_V3_BIT)
#define MTK_SOC_MT7628		BIT_ULL(MTK_SOC_MT7628_BIT)
#define MTK_RSTCTRL_PPE1	BIT_ULL(MTK_RSTCTRL_PPE1_BIT)
#define MTK_RSTCTRL_PPE2	BIT_ULL(MTK_RSTCTRL_PPE2_BIT)
#define MTK_U3_COPHY_V2		BIT_ULL(MTK_U3_COPHY_V2_BIT)
#define MTK_36BIT_DMA		BIT_ULL(MTK_36BIT_DMA_BIT)

#define MTK_ETH_MUX_GDM1_TO_GMAC1_ESW		\
	BIT_ULL(MTK_ETH_MUX_GDM1_TO_GMAC1_ESW_BIT)
#define MTK_ETH_MUX_GMAC2_GMAC0_TO_GEPHY	\
	BIT_ULL(MTK_ETH_MUX_GMAC2_GMAC0_TO_GEPHY_BIT)
#define MTK_ETH_MUX_U3_GMAC2_TO_QPHY		\
	BIT_ULL(MTK_ETH_MUX_U3_GMAC2_TO_QPHY_BIT)
#define MTK_ETH_MUX_GMAC2_TO_2P5GPHY		\
	BIT_ULL(MTK_ETH_MUX_GMAC2_TO_2P5GPHY_BIT)
#define MTK_ETH_MUX_GMAC1_GMAC2_TO_SGMII_RGMII	\
	BIT_ULL(MTK_ETH_MUX_GMAC1_GMAC2_TO_SGMII_RGMII_BIT)
#define MTK_ETH_MUX_GMAC12_TO_GEPHY_SGMII	\
	BIT_ULL(MTK_ETH_MUX_GMAC12_TO_GEPHY_SGMII_BIT)
#define MTK_ETH_MUX_GMAC123_TO_GEPHY_SGMII	\
	BIT_ULL(MTK_ETH_MUX_GMAC123_TO_GEPHY_SGMII_BIT)
#define MTK_ETH_MUX_GMAC123_TO_USXGMII	\
	BIT_ULL(MTK_ETH_MUX_GMAC123_TO_USXGMII_BIT)

/* Supported path present on SoCs */
#define MTK_ETH_PATH_GMAC1_RGMII	BIT_ULL(MTK_ETH_PATH_GMAC1_RGMII_BIT)
#define MTK_ETH_PATH_GMAC1_TRGMII	BIT_ULL(MTK_ETH_PATH_GMAC1_TRGMII_BIT)
#define MTK_ETH_PATH_GMAC1_SGMII	BIT_ULL(MTK_ETH_PATH_GMAC1_SGMII_BIT)
#define MTK_ETH_PATH_GMAC2_RGMII	BIT_ULL(MTK_ETH_PATH_GMAC2_RGMII_BIT)
#define MTK_ETH_PATH_GMAC2_SGMII	BIT_ULL(MTK_ETH_PATH_GMAC2_SGMII_BIT)
#define MTK_ETH_PATH_GMAC2_2P5GPHY	BIT_ULL(MTK_ETH_PATH_GMAC2_2P5GPHY_BIT)
#define MTK_ETH_PATH_GMAC2_GEPHY	BIT_ULL(MTK_ETH_PATH_GMAC2_GEPHY_BIT)
#define MTK_ETH_PATH_GMAC3_SGMII	BIT_ULL(MTK_ETH_PATH_GMAC3_SGMII_BIT)
#define MTK_ETH_PATH_GDM1_ESW		BIT_ULL(MTK_ETH_PATH_GDM1_ESW_BIT)
#define MTK_ETH_PATH_GMAC1_USXGMII	BIT_ULL(MTK_ETH_PATH_GMAC1_USXGMII_BIT)
#define MTK_ETH_PATH_GMAC2_USXGMII	BIT_ULL(MTK_ETH_PATH_GMAC2_USXGMII_BIT)
#define MTK_ETH_PATH_GMAC3_USXGMII	BIT_ULL(MTK_ETH_PATH_GMAC3_USXGMII_BIT)

#define MTK_GMAC1_RGMII		(MTK_ETH_PATH_GMAC1_RGMII | MTK_RGMII)
#define MTK_GMAC1_TRGMII	(MTK_ETH_PATH_GMAC1_TRGMII | MTK_TRGMII)
#define MTK_GMAC1_SGMII		(MTK_ETH_PATH_GMAC1_SGMII | MTK_SGMII)
#define MTK_GMAC2_RGMII		(MTK_ETH_PATH_GMAC2_RGMII | MTK_RGMII)
#define MTK_GMAC2_SGMII		(MTK_ETH_PATH_GMAC2_SGMII | MTK_SGMII)
#define MTK_GMAC2_2P5GPHY	(MTK_ETH_PATH_GMAC2_2P5GPHY | MTK_2P5GPHY)
#define MTK_GMAC2_GEPHY		(MTK_ETH_PATH_GMAC2_GEPHY | MTK_GEPHY)
#define MTK_GMAC3_SGMII		(MTK_ETH_PATH_GMAC3_SGMII | MTK_SGMII)
#define MTK_GDM1_ESW		(MTK_ETH_PATH_GDM1_ESW | MTK_ESW)
#define MTK_GMAC1_USXGMII	(MTK_ETH_PATH_GMAC1_USXGMII | MTK_USXGMII)
#define MTK_GMAC2_USXGMII	(MTK_ETH_PATH_GMAC2_USXGMII | MTK_USXGMII)
#define MTK_GMAC3_USXGMII	(MTK_ETH_PATH_GMAC3_USXGMII | MTK_USXGMII)

/* MUXes present on SoCs */
/* 0: GDM1 -> GMAC1, 1: GDM1 -> ESW */
#define MTK_MUX_GDM1_TO_GMAC1_ESW (MTK_ETH_MUX_GDM1_TO_GMAC1_ESW | MTK_MUX)

/* 0: GMAC2 -> GEPHY, 1: GMAC0 -> GePHY */
#define MTK_MUX_GMAC2_GMAC0_TO_GEPHY    \
	(MTK_ETH_MUX_GMAC2_GMAC0_TO_GEPHY | MTK_MUX | MTK_INFRA)

/* 0: U3 -> QPHY, 1: GMAC2 -> QPHY */
#define MTK_MUX_U3_GMAC2_TO_QPHY        \
	(MTK_ETH_MUX_U3_GMAC2_TO_QPHY | MTK_MUX | MTK_INFRA)

/* 2: GMAC1 -> SGMII, 3: GMAC2 -> SGMII */
#define MTK_MUX_GMAC1_GMAC2_TO_SGMII_RGMII      \
	(MTK_ETH_MUX_GMAC1_GMAC2_TO_SGMII_RGMII | MTK_MUX | \
	MTK_SHARED_SGMII)

/* 2: GMAC2 -> XGMII */
#define MTK_MUX_GMAC2_TO_2P5GPHY      \
	(MTK_ETH_MUX_GMAC2_TO_2P5GPHY | MTK_MUX | MTK_INFRA)

/* 0: GMACx -> GEPHY, 1: GMACx -> SGMII where x is 1 or 2 */
#define MTK_MUX_GMAC12_TO_GEPHY_SGMII   \
	(MTK_ETH_MUX_GMAC12_TO_GEPHY_SGMII | MTK_MUX)

#define MTK_MUX_GMAC123_TO_GEPHY_SGMII   \
	(MTK_ETH_MUX_GMAC123_TO_GEPHY_SGMII | MTK_MUX)

#define MTK_MUX_GMAC123_TO_USXGMII   \
	(MTK_ETH_MUX_GMAC123_TO_USXGMII | MTK_MUX | MTK_INFRA)

#define MTK_HAS_CAPS(caps, _x)		(((caps) & (_x)) == (_x))

#define MT7621_CAPS  (MTK_GMAC1_RGMII | MTK_GMAC1_TRGMII | \
		      MTK_GMAC2_RGMII | MTK_SHARED_INT | \
		      MTK_TRGMII_MT7621_CLK | MTK_QDMA | MTK_NETSYS_V1)

#define MT7622_CAPS  (MTK_GMAC1_RGMII | MTK_GMAC1_SGMII | MTK_GMAC2_RGMII | \
		      MTK_GMAC2_SGMII | MTK_GDM1_ESW | \
		      MTK_MUX_GDM1_TO_GMAC1_ESW | MTK_NETSYS_V1 | \
		      MTK_MUX_GMAC1_GMAC2_TO_SGMII_RGMII | MTK_QDMA)

#define MT7623_CAPS  (MTK_GMAC1_RGMII | MTK_GMAC1_TRGMII | MTK_GMAC2_RGMII | \
		      MTK_QDMA | MTK_NETSYS_V1)

#define MT7628_CAPS  (MTK_SHARED_INT | MTK_SOC_MT7628 | MTK_NETSYS_V1)

#define MT7629_CAPS  (MTK_GMAC1_SGMII | MTK_GMAC2_SGMII | MTK_GMAC2_GEPHY | \
		      MTK_GDM1_ESW | MTK_MUX_GDM1_TO_GMAC1_ESW | \
		      MTK_MUX_GMAC2_GMAC0_TO_GEPHY | \
		      MTK_MUX_U3_GMAC2_TO_QPHY | MTK_NETSYS_V1 | \
		      MTK_MUX_GMAC12_TO_GEPHY_SGMII | MTK_QDMA)

#define MT7986_CAPS   (MTK_PDMA_INT | MTK_GMAC1_SGMII | MTK_GMAC2_SGMII | \
                       MTK_MUX_GMAC12_TO_GEPHY_SGMII | MTK_QDMA | \
		       MTK_NETSYS_V2 | MTK_RSS)

#define MT7981_CAPS   (MTK_PDMA_INT | MTK_GMAC1_SGMII | MTK_GMAC2_SGMII | \
		       MTK_GMAC2_GEPHY | MTK_MUX_GMAC12_TO_GEPHY_SGMII | MTK_QDMA | \
		       MTK_MUX_U3_GMAC2_TO_QPHY | MTK_U3_COPHY_V2 | \
		       MTK_NETSYS_V2 | MTK_RSS)

#define MT7988_CAPS   (MTK_GMAC1_SGMII | MTK_GMAC2_SGMII | MTK_GMAC3_SGMII | \
		       MTK_PDMA_INT | MTK_MUX_GMAC123_TO_GEPHY_SGMII | MTK_QDMA | \
		       MTK_NETSYS_V3 | MTK_RSTCTRL_PPE1 | MTK_RSTCTRL_PPE2 | \
		       MTK_GMAC1_USXGMII | MTK_GMAC2_USXGMII | \
		       MTK_GMAC3_USXGMII | MTK_MUX_GMAC123_TO_USXGMII | \
		       MTK_GMAC2_2P5GPHY | MTK_MUX_GMAC2_TO_2P5GPHY | MTK_RSS | \
		       MTK_NETSYS_RX_V2 | MTK_36BIT_DMA)

struct mtk_tx_dma_desc_info {
	dma_addr_t	addr;
	u32		size;
	u16		vlan_tci;
	u16		qid;
	u8		gso:1;
	u8		csum:1;
	u8		vlan:1;
	u8		first:1;
	u8		last:1;
};

struct mtk_reg_map {
	u32	tx_irq_mask;
	u32	tx_irq_status;
	struct {
		u32	tx_ptr;		/* tx base pointer */
		u32	tx_cnt_cfg;	/* tx max count configuration  */
		u32	pctx_ptr;	/* tx cpu pointer */
		u32	pdtx_ptr;	/* tx dma pointer */
		u32	rx_ptr;		/* rx base pointer */
		u32	rx_cnt_cfg;	/* rx max count configuration */
		u32	pcrx_ptr;	/* rx cpu pointer */
		u32	glo_cfg;	/* global configuration */
		u32	rst_idx;	/* reset index */
		u32	delay_irq;	/* delay interrupt */
		u32	rx_cfg;		/* rx dma configuration */
		u32	irq_status;	/* interrupt status */
		u32	irq_mask;	/* interrupt mask */
		u32	int_grp;	/* interrupt group1 */
		u32	int_grp2;	/* interrupt group2 */
		u32	lro_ctrl_dw0;	/* lro control dword0 */
		u32	lro_alt_score_delta;	/* lro auto-learn score delta */
		u32	lro_rx_dly_int;	/* lro rx ring delay interrupt */
		u32	lro_rx_dip_dw0;	/* lro rx ring dip dword0 */
		u32	lro_rx_ctrl_dw0;	/* lro rx ring ctrl dword0 */
		u32	lro_alt_dbg;	/* lro auto-learn debug */
		u32	lro_alt_dbg_data;	/* lro auto-learn debug data */
		u32	rss_glo_cfg;	/* rss global configuration */
		u32	rss_hash_key_dw0;	/* rss hash key dword0 */
		u32	rss_indr_table_dw0;	/* rss indirection table dword0 */
	} pdma;
	struct {
		u32	qtx_cfg;	/* tx queue configuration */
		u32	qtx_sch;	/* tx queue scheduler configuration */
		u32	rx_ptr;		/* rx base pointer */
		u32	rx_cnt_cfg;	/* rx max count configuration */
		u32	qcrx_ptr;	/* rx cpu pointer */
		u32	glo_cfg;	/* global configuration */
		u32	rst_idx;	/* reset index */
		u32	delay_irq;	/* delay interrupt */
		u32	fc_th;		/* flow control */
		u32	int_grp;	/* interrupt group1 */
		u32	int_grp2;	/* interrupt group2 */
		u32	hred2;		/* interrupt mask */
		u32	ctx_ptr;	/* tx acquire cpu pointer */
		u32	dtx_ptr;	/* tx acquire dma pointer */
		u32	crx_ptr;	/* tx release cpu pointer */
		u32	drx_ptr;	/* tx release dma pointer */
		u32	fq_head;	/* fq head pointer */
		u32	fq_tail;	/* fq tail pointer */
		u32	fq_count;	/* fq free page count */
		u32	fq_blen;	/* fq free page buffer length */
		u32	tx_sch_rate;	/* tx scheduler rate control
					   registers */
	} qdma;
	u32	gdm1_cnt;
	u32	gdma_to_ppe0;
	u32	ppe_base[3];
	u32	wdma_base[3];
};

/* struct mtk_eth_data -	This is the structure holding all differences
 *				among various plaforms
 * @reg_map			Soc register map.
 * @ana_rgc3:			The offset for register ANA_RGC3 related to
 *				sgmiisys syscon
 * @caps			Flags shown the extra capability for the SoC
 * @hw_features			Flags shown HW features
 * @required_clks		Flags shown the bitmap for required clocks on
 *				the target SoC
 * @required_pctl		A bool value to show whether the SoC requires
 *				the extra setup for those pins used by GMAC.
 * @txd_size			Tx DMA descriptor size.
 * @rxd_size			Rx DMA descriptor size.
 * @rx_dma_l4_valid		Rx DMA valid register mask.
 * @dma_max_len			Max DMA tx/rx buffer length.
 * @dma_len_offset		Tx/Rx DMA length field offset.
 */
struct mtk_soc_data {
	const struct mtk_reg_map *reg_map;
	u32		ana_rgc3;
	u32		rss_num;
	u64		caps;
	u64		required_clks;
	bool		required_pctl;
	netdev_features_t hw_features;
	bool		has_sram;
	struct {
		u32	txd_size;
		u32	rxd_size;
		u32	tx_dma_size;
		u32	rx_dma_size;
		u32	fq_dma_size;
		u32	rx_dma_l4_valid;
		u32	dma_max_len;
		u32	dma_len_offset;
	} txrx;
};

#define MTK_DMA_MONITOR_TIMEOUT		msecs_to_jiffies(1000)

/* currently no SoC has more than 3 macs */
#if defined(CONFIG_MEDIATEK_NETSYS_V3)
#define MTK_MAX_DEVS		3
#else
#define MTK_MAX_DEVS		2
#endif

#define MTK_SGMII_PHYSPEED_AN          BIT(31)
#define MTK_SGMII_PHYSPEED_MASK        GENMASK(2, 0)
#define MTK_SGMII_PHYSPEED_1000        BIT(0)
#define MTK_SGMII_PHYSPEED_2500        BIT(1)
#define MTK_SGMII_PHYSPEED_5000	       BIT(2)
#define MTK_SGMII_PHYSPEED_10000       BIT(3)

#define MTK_HAS_FLAGS(flags, _x)       (((flags) & (_x)) == (_x))

/* struct mtk_sgmii_pcs - This structure holds each sgmii regmap and associated
 *			data
 * @regmap:		The register map pointing at the range used to setup
 *			SGMII modes
 * @regmap_pextp:	The register map pointing at the range used to setup
 *			PHYA
 * @ana_rgc3:		The offset refers to register ANA_RGC3 related to regmap
 * @id:			The element is used to record the index of PCS
 * @pcs:		Phylink PCS structure
 */
struct mtk_sgmii_pcs {
	struct mtk_eth		*eth;
	struct regmap		*regmap;
	struct regmap		*regmap_pextp;
	struct mutex		regmap_lock;
	struct mutex		reset_lock;
	phy_interface_t		interface;
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising);
	unsigned long		link_poll_expire;
	unsigned int		mode;
	u32			flags;
	u32			ana_rgc3;
	u32			polarity;
	u8			id;
	struct phylink_link_state	state;
	struct delayed_work	link_poll;
	struct phylink_pcs	pcs;
};

/* struct mtk_sgmii -	This is the structure holding sgmii regmap and its
 *			characteristics
 * @pll:		The register map pointing at the range used to setup
 *			PLL
 * @pcs			Array of individual PCS structures
 */
struct mtk_sgmii {
	struct mtk_sgmii_pcs	pcs[MTK_MAX_DEVS];
	struct regmap		*pll;
};

/* struct mtk_usxgmii_pcs - This structure holds each usxgmii regmap and
 *			associated data
 * @regmap:		The register map pointing at the range used to setup
 *			USXGMII modes
 * @regmap_pextp:	The register map pointing at the range used to setup
 *			PHYA
 * @id:			The element is used to record the index of PCS
 * @pcs:		Phylink PCS structure
 */
struct mtk_usxgmii_pcs {
	struct mtk_eth		*eth;
	struct regmap		*regmap;
	struct regmap		*regmap_pextp;
	struct mutex		regmap_lock;
	struct mutex		reset_lock;
	phy_interface_t		interface;
	unsigned long		link_poll_expire;
	unsigned int		mode;
	u32			polarity;
	u8			id;
	struct phylink_link_state	state;
	struct delayed_work	link_poll;
	struct phylink_pcs	pcs;
};

/* struct mtk_usxgmii -	This is the structure holding usxgmii regmap and its
 *			characteristics
 * @pll:		The register map pointing at the range used to setup
 *			PLL
 * @pcs			Array of individual PCS structures
 */
struct mtk_usxgmii {
	struct mtk_usxgmii_pcs	pcs[MTK_MAX_DEVS];
	struct regmap		*pll;
};

/* struct mtk_reset_event - This is the structure holding statistics counters
 *			for reset events
 * @count:		The counter is used to record the number of events
 */
struct mtk_reset_event {
	u32 count[32];
};

/* struct mtk_phylink_priv - This is the structure holding private data for phylink
 * @desc:		Pointer to the memory holding info about the phylink gpio
 * @id:			The element is used to record the phy index of phylink
 * @phyaddr:		The element is used to record the phy address of phylink
 * @link:		The element is used to record the phy link status of phylink
 */
struct mtk_phylink_priv {
	struct net_device	*dev;
	struct gpio_desc	*desc;
	char			label[16];
	int			id;
	int			phyaddr;
	int			link;
};

struct adma_monitor {
	struct adma_rx_monitor {
		u32		pre_drx[4];
		u16		pre_opq;
		u8		hang_count;
	} rx;
};

struct qdma_monitor {
	struct qdma_tx_monitor {
		u8		hang_count;
	} tx;
	struct qdma_rx_monitor {
		u32		pre_fq_head;
		u32		pre_fq_tail;
		u8		hang_count;
	} rx;
};

struct tdma_monitor {
	struct tdma_tx_monitor {
		u32		pre_ipq10;
		u32		pre_fsm;
		u8		hang_count;
	} tx;
	struct tdma_rx_monitor {
		u32		pre_fsm;
		u8		hang_count;
	} rx;
};

struct wdma_monitor {
	struct wdma_tx_monitor {
		u32		pre_dtx[MTK_WDMA_CNT];
		u16		pre_opq_gdm[MTK_MAX_DEVS];
		u16		pre_opq_wdma[MTK_WDMA_CNT];
		u8		err_opq_gdm[MTK_MAX_DEVS];
		u8		err_opq_wdma[MTK_WDMA_CNT];
		u8		hang_count[MTK_WDMA_CNT];
	} tx;
	struct wdma_rx_monitor {
		u32		pre_crx[MTK_WDMA_CNT];
		u32		pre_drx[MTK_WDMA_CNT];
		u32		pre_opq[MTK_WDMA_CNT];
		u8		hang_count[MTK_WDMA_CNT];
	} rx;
};

struct gdm_monitor {
	struct gdm_tx_monitor {
		u64		pre_tx_cnt[MTK_MAX_DEVS];
		u32		pre_fsm_gdm[MTK_MAX_DEVS];
		u32		pre_opq_gdm[MTK_MAX_DEVS];
		u8		hang_count_gdm[MTK_MAX_DEVS];
		u8		hang_count_gmac[MTK_MAX_DEVS];
	} tx;
	struct gdm_rx_monitor {
		u64		pre_rx_cnt[MTK_MAX_DEVS];
		u32		pre_fsm_gdm[MTK_MAX_DEVS];
		u16		pre_ipq_gdm[MTK_MAX_DEVS];
		u8		hang_count_gdm[MTK_MAX_DEVS];
		u8		hang_count_gmac[MTK_MAX_DEVS];
	} rx;
};

/* struct mtk_eth -	This is the main datasructure for holding the state
 *			of the driver
 * @dev:		The device pointer
 * @dma_dev:		The device pointer used for dma mapping/alloc
 * @base:		The mapped register i/o base
 * @page_lock:		Make sure that register operations are atomic
 * @tx_irq__lock:	Make sure that IRQ register operations are atomic
 * @rx_irq__lock:	Make sure that IRQ register operations are atomic
 * @dummy_dev:		we run 2 netdevs on 1 physical DMA ring and need a
 *			dummy for NAPI to work
 * @netdev:		The netdev instances
 * @mac:		Each netdev is linked to a physical MAC
 * @irq:		The IRQ that we are using
 * @msg_enable:		Ethtool msg level
 * @ethsys:		The register map pointing at the range used to setup
 *			MII modes
 * @infra:              The register map pointing at the range used to setup
 *                      SGMII and GePHY path
 * @pctl:		The register map pointing at the range used to setup
 *			GMAC port drive/slew values
 * @dma_refcnt:		track how many netdevs are using the DMA engine
 * @tx_ring:		Pointer to the memory holding info about the TX ring
 * @rx_ring:		Pointer to the memory holding info about the RX ring
 * @rx_ring_qdma:	Pointer to the memory holding info about the QDMA RX ring
 * @tx_napi:		The TX NAPI struct
 * @rx_napi:		The RX NAPI struct
 * @scratch_ring:	Newer SoCs need memory for a second HW managed TX ring
 * @phy_scratch_ring:	physical address of scratch_ring
 * @scratch_head:	The scratch memory that scratch_ring points to.
 * @clks:		clock array for all clocks required
 * @mii_bus:		If there is a bus we need to create an instance for it
 * @pending_work:	The workqueue used to reset the dma ring
 * @state:		Initialization and runtime state of the device
 * @soc:		Holding specific data among vaious SoCs
 */

struct mtk_eth {
	struct device			*dev;
	struct device			*dma_dev;
	void __iomem			*base;
	void __iomem			*sram_base;
	spinlock_t			page_lock;
	spinlock_t			tx_irq_lock;
	spinlock_t			rx_irq_lock;
	spinlock_t			txrx_irq_lock;
	struct net_device		dummy_dev;
	struct net_device		*netdev[MTK_MAX_DEVS];
	struct mtk_mac			*mac[MTK_MAX_DEVS];
	struct mtk_mux			*mux[MTK_MAX_DEVS];
	int				irq_fe[MTK_FE_IRQ_NUM];
	int				irq_pdma[MTK_PDMA_IRQ_NUM];
	u8				hwver;
	u32				msg_enable;
	u32				pppq_toggle;
	unsigned long			sysclk;
	struct regmap			*ethsys;
	struct regmap                   *infra;
	struct regmap                   *toprgu;
	struct mtk_sgmii		*sgmii;
	struct mtk_usxgmii		*usxgmii;
	struct regmap			*pctl;
	bool				hwlro;
	refcount_t			dma_refcnt;
	struct mtk_tx_ring		tx_ring[MTK_MAX_TX_RING_NUM];
	struct mtk_rx_ring		rx_ring[MTK_MAX_RX_RING_NUM];
	struct mtk_rx_ring		rx_ring_qdma;
	struct mtk_napi			tx_napi[MTK_TX_NAPI_NUM];
	struct mtk_napi			rx_napi[MTK_RX_NAPI_NUM];
	struct mtk_rss_params		rss_params;
	void				*scratch_ring;
	struct mtk_reset_event		reset_event;
	dma_addr_t			phy_scratch_ring;
	void				*scratch_head[MTK_FQ_DMA_HEAD];
	struct clk			*clks[MTK_CLK_MAX];

	struct mii_bus			*mii_bus;
	struct work_struct		pending_work;
	unsigned long			state;

	const struct mtk_soc_data	*soc;

	struct {
		struct delayed_work	monitor_work;
		struct adma_monitor	adma_monitor;
		struct qdma_monitor	qdma_monitor;
		struct tdma_monitor	tdma_monitor;
		struct wdma_monitor	wdma_monitor;
		struct gdm_monitor	gdm_monitor;
	} reset;

	u32				rx_dma_l4_valid;
	int				ip_align;
	spinlock_t			syscfg0_lock;
	struct notifier_block		netdevice_notifier;
};

/* struct mtk_mac -	the structure that holds the info about the MACs of the
 *			SoC
 * @id:			The number of the MAC
 * @interface:		Interface mode kept for detecting change in hw settings
 * @of_node:		Our devicetree node
 * @hw:			Backpointer to our main datastruture
 * @hw_stats:		Packet statistics counter
 */
struct mtk_mac {
	unsigned int			id;
	phy_interface_t			interface;
	unsigned int			mode;
	unsigned int			type;
	int				speed;
	struct device_node		*of_node;
	struct phylink			*phylink;
	struct phylink_config		phylink_config;
	struct mtk_phylink_priv		phylink_priv;
	struct mtk_eth			*hw;
	struct mtk_hw_stats		*hw_stats;
	__be32				hwlro_ip[MTK_MAX_LRO_IP_CNT];
	int				hwlro_ip_cnt;
	unsigned int			syscfg0;
	bool				tx_lpi_enabled;
	u32				tx_lpi_timer;
	struct notifier_block		device_notifier;
};

/* struct mtk_mux_data -	the structure that holds the private data about the
 *			 Passive MUXs of the SoC
 */
struct mtk_mux_data {
	struct device_node		*of_node;
	struct phylink			*phylink;
};

/* struct mtk_mux -	the structure that holds the info about the Passive MUXs of the
 *			SoC
 */
struct mtk_mux {
	struct delayed_work		poll;
	struct gpio_desc		*gpio[2];
	struct mtk_mux_data		*data[2];
	struct mtk_mac			*mac;
	unsigned int			channel;
};

/* the struct describing the SoC. these are declared in the soc_xyz.c files */
extern struct mtk_eth *g_eth;
extern const struct of_device_id of_mtk_match[];
extern u32 mtk_hwlro_stats_ebl;
extern u32 dbg_show_level;

/* tunnel offload related */
extern u32 (*mtk_get_tnl_netsys_params)(struct sk_buff *skb);
extern struct net_device *(*mtk_get_tnl_dev)(u8 tops_crsn);
extern void (*mtk_set_tops_crsn)(struct sk_buff *skb, u8 tops_crsn);

/* read the hardware status register */
void mtk_stats_update_mac(struct mtk_mac *mac);

void mtk_w32(struct mtk_eth *eth, u32 val, unsigned reg);
u32 mtk_r32(struct mtk_eth *eth, unsigned reg);
u32 mtk_m32(struct mtk_eth *eth, u32 mask, u32 set, unsigned reg);

struct phylink_pcs *mtk_sgmii_select_pcs(struct mtk_sgmii *ss, unsigned int id);
int mtk_sgmii_init(struct mtk_eth *eth, struct device_node *np,
		   u32 ana_rgc3);

int mtk_gmac_sgmii_path_setup(struct mtk_eth *eth, int mac_id);
int mtk_gmac_2p5gphy_path_setup(struct mtk_eth *eth, int mac_id);
int mtk_gmac_gephy_path_setup(struct mtk_eth *eth, int mac_id);
int mtk_gmac_rgmii_path_setup(struct mtk_eth *eth, int mac_id);
int mtk_gmac_usxgmii_path_setup(struct mtk_eth *eth, int mac_id);
void mtk_gdm_config(struct mtk_eth *eth, u32 id, u32 config);
void ethsys_reset(struct mtk_eth *eth, u32 reset_bits);

int mtk_mac2xgmii_id(struct mtk_eth *eth, int mac_id);
struct phylink_pcs *mtk_usxgmii_select_pcs(struct mtk_usxgmii *ss, unsigned int id);
int mtk_usxgmii_init(struct mtk_eth *eth, struct device_node *r);
int mtk_toprgu_init(struct mtk_eth *eth, struct device_node *r);

void mtk_eth_set_dma_device(struct mtk_eth *eth, struct device *dma_dev);
u32 mtk_rss_indr_table(struct mtk_rss_params *rss_params, int index);
#endif /* MTK_ETH_H */
