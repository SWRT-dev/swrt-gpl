/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2012 Reddy <Reddy.Mallikarjun@lantiq.com>
 *  Copyright (C) 2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 *  Copyright (C) 2016 Intel Corporation.
 */

#ifndef LTQ_CDMA_H
#define LTQ_CDMA_H
/*!
 * \file ltq_dma.h
 * \ingroup LTQ_DMA_CORE
 * \brief LTQ Central DMA module register definition
 */

/** Register definitions */
/** Clock control register */
#define DMA_CLC			0x00

/** Module ID register */
#define DMA_ID			0x08

/** DMA control register, global */
#define DMA_CTRL		0x10
/** Global Software reset */
#define DMA_CTRL_RST		0x00000001
/**Descriptor read back (supported for VR9)*/
#define DMA_CTRL_DRB		0x00000100
/** Descriptor read back (supported for VR9) */
#define DMA_CTRL_MBRSTCNT_MASK	0x03FF0000
/** MUltiple Burst Arbitration (supported for VR9) */
#define DMA_CTRL_MBRSTARB	0x40000000
/** Packet Arbitration (supported for VR9) */
#define DMA_CTRL_PKTARB		0x80000000
/** channel polling register*/

#define DMA_CPOLL		0x14
/**Enable Descriptor polling*/
#define DMA_CPOLL_EN		0x80000000
/**Enable Descriptor polling*/
#define DMA_CPOLL_CNT_MASK	0x0000FFF0
#define DMA_CPOLL_CNT_VAL(val)	(((val) & 0xFFF) << 4)

/** Channel Related registers */
/** channel select register */
#define DMA_CS0			0x18
#define DMA_CS			DMA_CS0
#define DMA_CS_MASK		0x0000001F
#define DMA_CS_VAL_GET(val)	(((val) & 0x1F))
#define DMA_CS_VAL_SET(val)	(((val) & 0x1F))

/** channel control register */
#define DMA_CCTRL0		0x1C
#define DMA_CCTRL		DMA_CCTRL0
/** Channel On/ Off */
#define DMA_CCTRL_ON		0x00000001
/** Channel reset */
#define DMA_CCTRL_RST		0x00000002
/** direction of the channel*/
#define DMA_CCTRL_DIR		0x00000100
/** channel weight for Tx direction */
#define DMA_CCTRL_TXWGT_MASK			0x00030000
#define DMA_CCTRL_TXWGT_VAL(val)	(((val) & 0x3) << 16)
#define DMA_CCTRL_TXWGT_GET(val)	\
	((((val) & DMA_CCTRL_TXWGT_MASK) >> 16) & 0x3)
/** Packet drop enabled */
#define DMA_CCTRL_PDEN			0x00800000
#define DMA_CCTRL_PDEN_GET(val)		\
	((((val) & DMA_CCTRL_PDEN) >> 23) & 0x1)
/* Peripheral to Peripheral copy enable*/
#define DMA_CCTRL_P2PCPY		0x01000000
#define DMA_CCTRL_P2PCPY_GET(val)	\
	((((val) & DMA_CCTRL_P2PCPY) >> 24) & 0x1)
/** loopback enable */
#define DMA_CCTRL_LBEN			0x02000000
/** Loopback channel number mask*/
#define DMA_CCTRL_LBCHNR_MASK		0xFC000000
#define DMA_CCTRL_LBCHNR_SET(val)	(((val) & 0x3F) << 26)
#define DMA_CCTRL_LBCHNR_GET(val)	\
	((((val) & DMA_CCTRL_LBCHNR_MASK) >> 26) & 0x3F)

/** Channel descriptor base address register */
#define DMA_CDBA0		0x20
#define DMA_CDBA		DMA_CDBA0

/** Channel descriptor length register */
#define DMA_CDLEN0		0x24
#define DMA_CDLEN		DMA_CDLEN0
#define DMA_CDLEN_MASK		0x000000FF
#define DMA_CDLEN_VAL_SET(val)	(((val) & 0xFF))
#define DMA_CDLEN_VAL_GET(val)	(((val) & 0xFF))

/** Channel interrupt status register */
#define DMA_CIS0		0x28
#define DMA_CIS			DMA_CIS0

/* Channel Interrupt Status Register  */
/** End of packet interrupt */
#define DMA_CIS_EOP		0x00000002
/** Descriptor Under-Run Interrupt  */
#define DMA_CIS_DUR		0x00000004
/** Descriptor Complete Interrupt  */
#define DMA_CIS_DESCPT		0x00000008
/** Channel Off Interrupt  */
#define DMA_CIS_CHOFF		0x00000010
/** SAI Read Error Interrupt */
#define DMA_CIS_RDERR		0x00000020
/** all interrupts */
#define DMA_CIS_ALL	(DMA_CIS_EOP | DMA_CIS_DUR | DMA_CIS_DESCPT \
			| DMA_CIS_CHOFF | DMA_CIS_RDERR)

/** Channel interrupt enable register */
#define DMA_CIE0		0x2C
#define DMA_CIE			DMA_CIE0

/**  Channel Interrupt Enable Register */
/** End of packet interrupt enable */
#define DMA_CIE_EOP		0x00000002
/** Descriptor Under-Run Interrupt enable  */
#define DMA_CIE_DUR		0x00000004
/** Descriptor Complete Interrupt  enable*/
#define DMA_CIE_DESCPT		0x00000008
/** Channel Off Interrupt enable */
#define DMA_CIE_CHOFF		0x00000010
/** SAI Read Error Interrupt enable*/
#define DMA_CIE_RDERR		0x00000020

/** enable all interrupts */
#define DMA_CIE_ALL	(DMA_CIE_EOP | DMA_CIE_DUR | DMA_CIE_DESCPT \
			| DMA_CIE_CHOFF | DMA_CIE_RDERR)

/** Channel Global buffer length register (not supported for Danube)*/
#define DMA_CGBL		0x30
#define DMA_CGBL_MASK		0x0000FFFF
#define DMA_CGBL_SET_VAL(val)	(((val) & 0xFFFF))
#define DMA_CGBL_GET_VAL(val)	(((val) & 0xFFFF))

/** DMA Current Descriptor Pointer Register (Supports only VR9)*/
#define DMA_CDPTNRD		0x34

/** Port Related registers */
/** Port select registers */
#define DMA_PS0			0x40
#define DMA_PS			DMA_PS0
/** Port select mask*/
#define DMA_PS_PS_MASK		0x00000007
#define DMA_PS_VAL_SET(val)	(((val) & 0x7))
#define DMA_PS_VAL_GET(val)	(((val) & 0x7))

#define DMA_PCTRL0		0x44
#define DMA_PCTRL		DMA_PCTRL0
/** Port Control bit mask*/
/** General purpose control, only used to memcopy port*/
#define DMA_PCTRL_GPC			0x00010000
#define DMA_PCTRL_GPC_VAL_SET(val)	(((val) & 0x1) << 16)
#define DMA_PCTRL_GPC_VAL_GET(val)	(((val) & DMA_PCTRL_GPC) >> 16)

/** Port Endianness for Tranmit Direction  */
#define DMA_PCTRL_TXENDI_SET_VAL(val)	(((val) & 0x3) << 10)
/** Port Endianness for Receive Direction  */
#define DMA_PCTRL_RXENDI_SET_VAL(val)	(((val) & 0x3) << 8)
/** Port Burst Length for Transmit Direction */
#define DMA_PCTRL_TXBL_SET_VAL(val)	(((val) & 0x3) << 4)
/* Port Burst Length for Receive Direction */
#define DMA_PCTRL_RXBL_SET_VAL(val)	(((val) & 0x3) << 2)
/* Port Weight for Transmit Direction  */
#define DMA_PCTRL_TXWGT_SET_VAL(val)	(((val) & 0x7) << 12)
/* Port packet drop enabled*/
#define DMA_PCTRL_PDEN_SET_VAL(val)	(((val) & 0x1) << 6)

/** Interrupt Related registers */
/** Interrupt node enable register */
#define DMA_IRNEN		0xf4
/** Interrupt node control register */
#define DMA_IRNCR		0xf8
/** Interrupt capture register */
#define DMA_IRNICR		0xfc

/** DMA descriptor bitfields */
/** Descriptor owner bit indication*/
/** Descriptor is owned by DMA */
#define DMA_OWN			1
/** Descriptor is owned by CPU */
#define CPU_OWN			0
/** Descriprot Complete */
#define DMA_DESC_CPT_SET	0x40000000
/** Start of Packet */
#define DMA_DESC_SOP_SET	0x20000000
/** End of Packet */
#define DMA_DESC_EOP_SET	0x10000000

/** Descriptor byte offset */
#define DMA_DESC_BYTE_OFFSET		23
/** Receive Descriptor byte offset */
#define DMA_RX_DESC_BYTE_OFFSET(val)    \
	((val & 0x3) << DMA_DESC_BYTE_OFFSET)
/** Transmit Descriptor byte offset */
#define DMA_TX_DESC_BYTE_OFFSET(val)    \
	((val & 0x1f) << DMA_DESC_BYTE_OFFSET)

/*
 * Default Transmit/Receive endiannes type
 * B0_B1_B2_B3 No byte switching
 */
#define DMA_DEFAULT_TX_ENDIANNESS	IFX_DMA_ENDIAN_TYPE3
#define DMA_DEFAULT_RX_ENDIANNESS	IFX_DMA_ENDIAN_TYPE3
/** channel weight value*/
#define DMA_CH_DEFAULT_WEIGHT		100
/** Default Port Transmit weight value */
#define DMA_TX_PORT_DEFAULT_WEIGHT	1
/** Default Port Transmit weight value */
#define DMA_TX_CHAN_DEFAULT_WEIGHT	1
/** Default packet drop enabled for port */
#define DMA_DEF_PORT_BASED_PKT_DROP_EN	0
/** Default packet drop enabled for channel   */
#define DMA_DEF_CHAN_BASED_PKT_DROP_EN	0
/** Default Transmit burst length  */
#define DMA_PORT_DEFAULT_TX_BURST_LEN	IFX_DMA_BURSTL_8
/** Default Receive burst length  */
#define DMA_PORT_DEFAULT_RX_BURST_LEN	IFX_DMA_BURSTL_8

/** default enabled interrupts */
#define DMA_CIE_DEFAULT			(DMA_CIE_DESCPT | DMA_CIE_EOP)
/** disable all interrupts */
#define DMA_CIE_DISABLE_ALL		0
/** default channel polling interval value*/
#define DMA_DEFAULT_POLL_VALUE		4

#ifdef CONFIG_CPU_LITTLE_ENDIAN
struct rx_desc {
	union {
		struct {
			unsigned int data_length:16;
			unsigned int reserved16_22:7;
			unsigned int byte_offset:2;
			unsigned int reserve_25_27:3;
			unsigned int eop:1;
			unsigned int sop:1;
			unsigned int C:1;
			unsigned int OWN:1;
		} __packed field;
		unsigned int word;
	} __packed status;
	dma_addr_t data_pointer;
} __packed __aligned(8);

struct tx_desc {
	union {
		struct {
			unsigned int data_length:16;
			unsigned int reserved:7;
			unsigned int byte_offset:5;
			unsigned int eop:1;
			unsigned int sop:1;
			unsigned int C:1;
			unsigned int OWN:1;
		} __packed field;
		unsigned int word;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);
#else
struct rx_desc {
	union {
		struct {
			unsigned int OWN:1;
			unsigned int C:1;
			unsigned int sop:1;
			unsigned int eop:1;
			unsigned int reserve_25_27:3;
			unsigned int byte_offset:2;
			unsigned int reserve16_22:7;
			unsigned int data_length:16;
		} __packed field;
		unsigned int word;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);

struct tx_desc {
	union {
		struct {
			unsigned int OWN:1;
			unsigned int C:1;
			unsigned int sop:1;
			unsigned int eop:1;
			unsigned int byte_offset:5;
			unsigned int reserved:7;
			unsigned int data_length:16;
		} __packed field;
		unsigned int word;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);

#endif /*CONFIG_CPU_LITTLE_ENDIAN  */

struct dma_ctrl {
	void __iomem *membase;
	int irq_base;
	int chans; /* for the current controller */
	unsigned int chained_irq;
	u32 budget;
	struct device *dev;
};

#endif /* LTQ_CDMA_H */
