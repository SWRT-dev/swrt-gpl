/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2014 ~ 2015 Lei Chuanhua <chuanhua.lei@lantiq.com>
 *  Copyright (C) 2016 ~ 2017 Intel Corporation.
 */
#ifndef LANTIQ_DMAX_H
#define LANTIQ_DMAX_H
#include <linux/types.h>

/*!
 * \defgroup LTQ_DMA_CORE Intel High Performance DMA Core Driver
 * \brief High Performance DMA Core Module, supports LTQ major SoC platform
 */

/*!
 * \defgroup LTQ_DMA_DRV_API DMA Common APIs
 * \ingroup LTQ_DMA_CORE
 * \brief DMA Common APIs definitions for other modules
 *
 * \note A note on dma api call sequence

 *  All drivers needing DMA channels, should allocate and release them
 *  through the public routines \ref ltq_request_dma and \ref ltq_free_dma.\n
 *  <b>A generic initialization sequence</b>\n
 *  1. \ref ltq_request_dma (mandatory)\n
 *  2. \ref ltq_dma_chan_pktsize_cfg (optional)\n
 *  3. Alloc / free callback setup by calling\n
 *     \ref ltq_dma_chan_buf_alloc_callback_cfg\n
 *     \ref ltq_dma_chan_buf_free_callback_cfg (optional)\n
 *  4. tx or rx descriptor or data buffer setup by calling\n
 *    \ref ltq_dma_chan_desc_alloc\n
 *    \ref ltq_dma_chan_data_buf_alloc\n
 *	for CBM \n
 *   \ref ltq_dma_chan_desc_cfg\n
 *  5. P2P configuration\n
 *  6. Channel on\n
 *
 *  <b>A generic teardown sequence</b>\n
 *  1. channel off/reset\n
 *  2. free descriptor and data buffer\n
 *  3. free dma channel\n
 */
/*!
 * \file lantiq_dmax.h
 * \ingroup LTQ_DMA_CORE
 * \brief Header file for intel high performance DMA core driver
 */

/*
 * DMA controller, port and channel encoding: 32 bits total as the following
 * layout:
 * controller | port  | channe number
 * 31.......24|23...16|15...........0
 */
#define _DMA_CHANBITS	16
#define _DMA_PORTBITS	8
#define _DMA_CTRLBITS	8

#define _DMA_CHANMASK	((1 << _DMA_CHANBITS) - 1)
#define _DMA_PORTMASK	((1 << _DMA_PORTBITS) - 1)
#define _DMA_CTRLMASK	((1 << _DMA_CTRLBITS) - 1)

#define _DMA_CHANSHIFT	0
#define _DMA_PORTSHIFT	(_DMA_CHANSHIFT + _DMA_CHANBITS)
#define _DMA_CTRLSHIFT	(_DMA_PORTSHIFT + _DMA_PORTBITS)

#define _DMA_C(controller, port, channel) \
	(((controller)  << _DMA_CTRLSHIFT) | \
	 ((port)  << _DMA_PORTSHIFT) | \
	 ((channel)  << _DMA_CHANSHIFT))

#define _DMA_CONTROLLER(nr)	(((nr) >> _DMA_CTRLSHIFT) & _DMA_CTRLMASK)
#define _DMA_PORT(nr)		(((nr) >> _DMA_PORTSHIFT) & _DMA_PORTMASK)
#define _DMA_CHANNEL(nr)	(((nr) >> _DMA_CHANSHIFT) & _DMA_CHANMASK)

#define MAX_DMA_CHAN_PER_PORT	64
#define MAX_DMA_PORT_PER_CTRL	4

enum dma_controller {
	DMA0 = 0,
	DMA1TX,
	DMA1RX,
	DMA2TX,
	DMA2RX,
	DMA3,
	DMA4,
	DMAMAX,
};

enum dma_ctrl_port {
	DMA0_SPI0 = 0,
	DMA0_SPI1 = 1,
	DMA0_HSNAND = 2,
	DMA0_MEMCOPY = 3,
	DMA1TX_PORT = 0,
	DMA1RX_PORT = 0,
	DMA2TX_PORT = 0,
	DMA2RX_PORT = 0,
	DMA3_PORT = 0,
	DMA4_PORT = 0,
};

enum dma_endian {
	DMA_ENDIAN_TYPE0 = 0,
	DMA_ENDIAN_TYPE1,
	DMA_ENDIAN_TYPE2,
	DMA_ENDIAN_TYPE3,
	DMA_ENDIAN_MAX,
};

enum dma_burst {
	DMA_BURSTL_2DW = 1,
	DMA_BURSTL_4DW = 2,
	DMA_BURSTL_8DW = 3,
	DMA_BURSTL_16DW = 16,
	DMA_BURSTL_32DW = 32,
};

enum dma_pkt_drop {
	DMA_PKT_DROP_DISABLE = 0,
	DMA_PKT_DROP_ENABLE,
};

enum dma_channel {
	DMA_CHANNEL_0 = 0,
	DMA_CHANNEL_1,
	DMA_CHANNEL_2,
	DMA_CHANNEL_3,
	DMA_CHANNEL_4,
	DMA_CHANNEL_5,
	DMA_CHANNEL_6,
	DMA_CHANNEL_7,
	DMA_CHANNEL_8,
	DMA_CHANNEL_9,
	DMA_CHANNEL_10,
	DMA_CHANNEL_11,
	DMA_CHANNEL_12,
	DMA_CHANNEL_13,
	DMA_CHANNEL_14,
	DMA_CHANNEL_15,
	DMA_CHANNEL_16,
	DMA_CHANNEL_17,
	DMA_CHANNEL_18,
	DMA_CHANNEL_19,
	DMA_CHANNEL_20,
	DMA_CHANNEL_21,
	DMA_CHANNEL_22,
	DMA_CHANNEL_23,
	DMA_CHANNEL_24,
	DMA_CHANNEL_25,
	DMA_CHANNEL_26,
	DMA_CHANNEL_27,
	DMA_CHANNEL_28,
	DMA_CHANNEL_29,
	DMA_CHANNEL_30,
	DMA_CHANNEL_31,
	DMA_CHANNEL_32,
	DMA_CHANNEL_33,
	DMA_CHANNEL_34,
	DMA_CHANNEL_35,
	DMA_CHANNEL_36,
	DMA_CHANNEL_37,
	DMA_CHANNEL_38,
	DMA_CHANNEL_39,
	DMA_CHANNEL_40,
	DMA_CHANNEL_41,
	DMA_CHANNEL_42,
	DMA_CHANNEL_43,
	DMA_CHANNEL_44,
	DMA_CHANNEL_45,
	DMA_CHANNEL_46,
	DMA_CHANNEL_47,
	DMA_CHANNEL_48,
	DMA_CHANNEL_49,
	DMA_CHANNEL_50,
	DMA_CHANNEL_51,
	DMA_CHANNEL_52,
	DMA_CHANNEL_53,
	DMA_CHANNEL_54,
	DMA_CHANNEL_55,
	DMA_CHANNEL_56,
	DMA_CHANNEL_57,
	DMA_CHANNEL_58,
	DMA_CHANNEL_59,
	DMA_CHANNEL_60,
	DMA_CHANNEL_61,
	DMA_CHANNEL_62,
	DMA_CHANNEL_63,
};

/* API channel list will be used by DMA clients */

/* DMA0 */
#define DMA0_SPI0_RX		_DMA_C(DMA0, DMA0_SPI0, DMA_CHANNEL_0)
#define DMA0_SPI0_TX		_DMA_C(DMA0, DMA0_SPI0, DMA_CHANNEL_1)
#define DMA0_SPI1_RX		_DMA_C(DMA0, DMA0_SPI1, DMA_CHANNEL_2)
#define DMA0_SPI1_TX		_DMA_C(DMA0, DMA0_SPI1, DMA_CHANNEL_3)
#define DMA0_HSNAND_RX		_DMA_C(DMA0, DMA0_HSNAND, DMA_CHANNEL_4)
#define DMA0_HSNAND_TX		_DMA_C(DMA0, DMA0_HSNAND, DMA_CHANNEL_5)
#define DMA0_MEMCPY_CLASS0_RX	_DMA_C(DMA0, DMA0_MEMCOPY, DMA_CHANNEL_12)
#define DMA0_MEMCPY_CLASS0_TX	_DMA_C(DMA0, DMA0_MEMCOPY, DMA_CHANNEL_13)
#define DMA0_MEMCPY_CLASS1_RX	_DMA_C(DMA0, DMA0_MEMCOPY, DMA_CHANNEL_14)
#define DMA0_MEMCPY_CLASS1_TX	_DMA_C(DMA0, DMA0_MEMCOPY, DMA_CHANNEL_15)

/* DMA1TX */
#define DMA1TX_LAN_SWITCH_CLASS0	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_0)

#define DMA1TX_LAN_SWITCH_CLASS1	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_1)
#define DMA1TX_LAN_SWITCH_CLASS2	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_2)
#define DMA1TX_LAN_SWITCH_CLASS3	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_3)

#define DMA1TX_CHAN4_RESERV	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_4)
#define DMA1TX_LOOP_FCS_REGEN	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_5)
#define DMA1TX_CHAN6_RESERV	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_6)

#define DMA1TX_EXT_WLAN_PCIE_CLASS7	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_7)
#define DMA1TX_INTERNAL_WLAN_CLASS8	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_8)

#define DMA1TX_USB_LAN_CLASS9	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_9)
#define DMA1TX_USB_LAN_CLASS10	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_10)
#define DMA1TX_CHAN11_RESERV	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_11)
#define DMA1TX_USB_WAN_CLASS12	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_12)

#define DMA1TX_DSL_WAN_CBMP18_CLASS13	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_13)
#define DMA1TX_DMA1RX_CH14_CLASS14	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_14)
#define DMA1TX_GSWIP_R_WAN_CBMP19_CLASS15	\
	_DMA_C(DMA1TX, DMA1TX_PORT, DMA_CHANNEL_15)

/* DMA1RX */
#define DMA1RX_TMU_CLASS0	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_0)
#define DMA1RX_TMU_CLASS1	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_1)
#define DMA1RX_CBM_P10_CLASS9	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_9)
#define DMA1RX_CBM_P11_CLASS10	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_10)
#define DMA1RX_CBM_P12_CLASS11	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_11)
#define DMA1RX_CBM_P13_CLASS12	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_12)
#define DMA1RX_CBM_P14_CLASS13	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_13)
#define DMA1RX_DMA1TX_CH14_CLASS14	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_14)
#define DMA1RX_CBM_P8_CLASS15	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_15)
#define DMA1RX_CBM_P8_CLASS16_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_16)
#define DMA1RX_CBM_P10_CLASS25_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_25)
#define DMA1RX_CBM_P11_CLASS26_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_26)
#define DMA1RX_CBM_P12_CLASS27_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_27)
#define DMA1RX_CBM_P13_CLASS28_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_28)
#define DMA1RX_CBM_P14_CLASS29_JUMBO	\
	_DMA_C(DMA1RX, DMA1RX_PORT, DMA_CHANNEL_29)
/* DMA2TX */
#define DMA2TX_CBM_P6_CLASS0	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_0)
#define DMA2TX_CBM_P6_CLASS1	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_1)
#define DMA2TX_CBM_P7_CLASS2	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_2)
#define DMA2TX_CBM_P8_CLASS3	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_3)
#define DMA2TX_CBM_P9_CLASS4	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_4)
#define DMA2TX_CBM_P10_CLASS5	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_5)
#define DMA2TX_CBM_P11_CLASS6	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_6)
#define DMA2TX_CBM_P12_CLASS9	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_9)
#define DMA2TX_CBM_P13_CLASS10	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_10)
#define DMA2TX_CBM_P14_CLASS11	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_11)
#define DMA2TX_CBM_P15_CLASS12	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_12)
#define DMA2TX_CBM_P16_CLASS13	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_13)
#define DMA2TX_CBM_P17_CLASS14	_DMA_C(DMA2TX, DMA2TX_PORT, DMA_CHANNEL_14)
/* DMA2RX */
#define DMA2RX_GSWIP_R_CLASS0	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_0)
#define DMA2RX_GSWIP_R_CLASS1	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_1)
#define DMA2RX_GSWIP_R_CLASS2	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_2)
#define DMA2RX_GSWIP_R_CLASS3	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_3)

#define DMA2RX_GSWIP_R_CLASS4	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_4)
#define DMA2RX_GSWIP_R_CLASS5	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_5)
#define DMA2RX_GSWIP_R_CLASS6	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_6)

#define DMA2RX_CBMP5_CLASS14	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_14)
#define DMA2RX_CBMP6_CLASS15	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_15)
#define DMA2RX_CBMP5_CLASS30_JUMBO	\
	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_30)
#define DMA2RX_CBMP6_CLASS31_JUMBO	\
	_DMA_C(DMA2RX, DMA2RX_PORT, DMA_CHANNEL_31)
/* DMA3 */
#define DMA3_TOE_MEMCOPY_CLASS0_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_0)
#define DMA3_TOE_MEMCOPY_CLASS0_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_1)
#define DMA3_TOE_MEMCOPY_CLASS1_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_2)
#define DMA3_TOE_MEMCOPY_CLASS1_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_3)
#define DMA3_TOE_MEMCOPY_CLASS2_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_4)
#define DMA3_TOE_MEMCOPY_CLASS2_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_5)
#define DMA3_TOE_MEMCOPY_CLASS3_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_6)
#define DMA3_TOE_MEMCOPY_CLASS3_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_7)
#define DMA3_TOE_MEMCOPY_CLASS4_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_8)
#define DMA3_TOE_MEMCOPY_CLASS4_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_9)
#define DMA3_TOE_MEMCOPY_CLASS5_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_10)
#define DMA3_TOE_MEMCOPY_CLASS5_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_11)
#define DMA3_MCOPY_MEMCOPY_CLASS6_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_12)
#define DMA3_MCOPY_MEMCOPY_CLASS6_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_13)
#define DMA3_MCOPY_MEMCOPY_CLASS7_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_14)
#define DMA3_MCOPY_MEMCOPY_CLASS7_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_15)
#define DMA3_MCOPY_MEMCOPY_CLASS8_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_16)
#define DMA3_MCOPY_MEMCOPY_CLASS8_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_17)
#define DMA3_MCOPY_MEMCOPY_CLASS9_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_18)
#define DMA3_MCOPY_MEMCOPY_CLASS9_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_19)
#define DMA3_MCOPY_MEMCOPY_CLASS10_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_20)
#define DMA3_MCOPY_MEMCOPY_CLASS10_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_21)
#define DMA3_MCOPY_MEMCOPY_CLASS11_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_22)
#define DMA3_MCOPY_MEMCOPY_CLASS11_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_23)
#define DMA3_MCOPY_MEMCOPY_CLASS12_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_24)
#define DMA3_MCOPY_MEMCOPY_CLASS12_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_25)
#define DMA3_MCOPY_MEMCOPY_CLASS13_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_26)
#define DMA3_MCOPY_MEMCOPY_CLASS13_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_27)
#define DMA3_MCOPY_MEMCOPY_CLASS14_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_28)
#define DMA3_MCOPY_MEMCOPY_CLASS14_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_29)
#define DMA3_TOE_MEMCOPY_CLASS15_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_30)
#define DMA3_TOE_MEMCOPY_CLASS15_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_31)
#define DMA3_TOE_MEMCOPY_CLASS16_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_32)
#define DMA3_TOE_MEMCOPY_CLASS16_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_33)
#define DMA3_TOE_MEMCOPY_CLASS17_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_34)
#define DMA3_TOE_MEMCOPY_CLASS17_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_35)
#define DMA3_TOE_MEMCOPY_CLASS18_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_36)
#define DMA3_TOE_MEMCOPY_CLASS18_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_37)
#define DMA3_TOE_MEMCOPY_CLASS19_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_38)
#define DMA3_TOE_MEMCOPY_CLASS19_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_39)
#define DMA3_TOE_MEMCOPY_CLASS20_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_40)
#define DMA3_TOE_MEMCOPY_CLASS20_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_41)
#define DMA3_TOE_MEMCOPY_CLASS21_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_42)
#define DMA3_TOE_MEMCOPY_CLASS21_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_43)
#define DMA3_TOE_MEMCOPY_CLASS22_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_44)
#define DMA3_TOE_MEMCOPY_CLASS22_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_45)
#define DMA3_TOE_MEMCOPY_CLASS23_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_46)
#define DMA3_TOE_MEMCOPY_CLASS23_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_47)
#define DMA3_TOE_MEMCOPY_CLASS24_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_48)
#define DMA3_TOE_MEMCOPY_CLASS24_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_49)
#define DMA3_TOE_MEMCOPY_CLASS25_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_50)
#define DMA3_TOE_MEMCOPY_CLASS25_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_51)
#define DMA3_TOE_MEMCOPY_CLASS26_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_52)
#define DMA3_TOE_MEMCOPY_CLASS26_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_53)
#define DMA3_TOE_MEMCOPY_CLASS27_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_54)
#define DMA3_TOE_MEMCOPY_CLASS27_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_55)
#define DMA3_TOE_MEMCOPY_CLASS28_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_56)
#define DMA3_TOE_MEMCOPY_CLASS28_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_57)
#define DMA3_TOE_MEMCOPY_CLASS29_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_58)
#define DMA3_TOE_MEMCOPY_CLASS29_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_59)
#define DMA3_TOE_MEMCOPY_CLASS30_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_60)
#define DMA3_TOE_MEMCOPY_CLASS30_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_61)
#define DMA3_TOE_MEMCOPY_CLASS31_RX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_62)
#define DMA3_TOE_MEMCOPY_CLASS31_TX	_DMA_C(DMA3, DMA3_PORT, DMA_CHANNEL_63)
/* DMA4 */
#define DMA4_MPE_MEMCOPY_CLASS0_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_0)
#define DMA4_MPE_MEMCOPY_CLASS0_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_1)
#define DMA4_MPE_MEMCOPY_CLASS1_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_2)
#define DMA4_MPE_MEMCOPY_CLASS1_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_3)
#define DMA4_MPE_MEMCOPY_CLASS2_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_4)
#define DMA4_MPE_MEMCOPY_CLASS2_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_5)
#define DMA4_MPE_MEMCOPY_CLASS3_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_6)
#define DMA4_MPE_MEMCOPY_CLASS3_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_7)
#define DMA4_MPE_MEMCOPY_CLASS4_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_8)
#define DMA4_MPE_MEMCOPY_CLASS4_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_9)
#define DMA4_MPE_MEMCOPY_CLASS5_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_10)
#define DMA4_MPE_MEMCOPY_CLASS5_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_11)
#define DMA4_MPE_MEMCOPY_CLASS6_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_12)
#define DMA4_MPE_MEMCOPY_CLASS6_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_13)
#define DMA4_MPE_MEMCOPY_CLASS7_RX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_14)
#define DMA4_MPE_MEMCOPY_CLASS7_TX	_DMA_C(DMA4, DMA4_PORT, DMA_CHANNEL_15)

enum dma_pseudo_irq {
	RCV_INT = 1,
	TX_BUF_FULL_INT = 2,
	TRANSMIT_CPT_INT = 4,
};

#ifdef CONFIG_CPU_BIG_ENDIAN

#ifdef CONFIG_XBAR_LE
/* Four DWs descriptor format */
struct dma_rx_desc {
	union {
		struct {
			u32 resv:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 session_id:12;
			u32 tcp_err:1;
			u32 nat:1;
			u32 dec:1;
			u32 enc:1;
			u32 mpe2:1;
			u32 mpe1:1;
			u32 color:2;
			u32 ep:4;
			u32 resv:4;
			u32 cla:4;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer;/* dw2 */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 dic:1;
			u32 pdu:1;
			u32 byte_offset:3;
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */

} __packed __aligned(16);

struct dma_tx_desc {
	union {
		struct {
			u32 resv:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 session_id:12;
			u32 tcp_err:1;
			u32 nat:1;
			u32 dec:1;
			u32 enc:1;
			u32 mpe2:1;
			u32 mpe1:1;
			u32 color:2;
			u32 ep:4;
			u32 resv:4;
			u32 cla:4;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer;/* dw2 */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 dic:1;
			u32 pdu:1;
			u32 byte_offset:3;
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /* dw3 */
} __packed __aligned(16);

#else /* Normal big endian */
/* Four DWs descriptor format */
struct dma_rx_desc {
	union {
		struct {
			u32 session_id:12;
			u32 tcp_err:1;
			u32 nat:1;
			u32 dec:1;
			u32 enc:1;
			u32 mpe2:1;
			u32 mpe1:1;
			u32 color:2;
			u32 ep:4;
			u32 resv:4;
			u32 cla:4;
		} __packed field;
		u32 all;
	} __packed dw1;
	union {
		struct {
			u32 resv:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 dic:1;
			u32 pdu:1;
			u32 byte_offset:3;
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
	dma_addr_t data_pointer;/* dw2 */
} __packed __aligned(16);

struct dma_tx_desc {
	union {
		struct {
			u32 session_id:12;
			u32 tcp_err:1;
			u32 nat:1;
			u32 dec:1;
			u32 enc:1;
			u32 mpe2:1;
			u32 mpe1:1;
			u32 color:2;
			u32 ep:4;
			u32 resv:4;
			u32 cla:4;
		} __packed field;
		u32 all;
	} __packed dw1;
	union {
		struct {
			u32 resv:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 dic:1;
			u32 pdu:1;
			u32 byte_offset:3;
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /* dw3 */
	dma_addr_t data_pointer;/* dw2 */
} __packed __aligned(16);

#endif /* CONFIG_XBAR_LE */

/* 2 DWs format descriptor */
struct dma_rx_desc_2dw {
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 reserve_25_27:3;
			u32 byte_offset:2;
			u32 rx_sideband:4;
			u32 reserve16_18:3;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);

struct dma_tx_desc_2dw {
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 byte_offset:5;
			u32 reserved:7;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);
#else
/* Four DWs descriptor format */
struct dma_rx_desc {
	union {
		struct {
			u32 dest_id:15;
			u32 eth_type:2;
			u32 flow_id:8;
			u32 tunnel_id:4;
			u32 resv:3;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 cla:4;
			u32 resv:4;
			u32 ep:4;
			u32 color:2;
			u32 mpe1:1;
			u32 mpe2:1;
			u32 enc:1;
			u32 dec:1;
			u32 nat:1;
			u32 tcp_err:1;
			u32 session_id:12;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer; /* dw2 */
	union {
		struct {
			u32 data_len:16;
			u32 mpoa_mode:2;
			u32 mpoa_pt:1;
			u32 qid:4;
			u32 byte_offset:3;
			u32 pdu:1;
			u32 dic:1;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
} __packed __aligned(16);

struct dma_tx_desc {
	union {
		struct {
			u32 dest_id:15;
			u32 eth_type:2;
			u32 flow_id:8;
			u32 tunnel_id:4;
			u32 resv:3;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 cla:4;
			u32 resv:4;
			u32 ep:4;
			u32 color:2;
			u32 mpe1:1;
			u32 mpe2:1;
			u32 enc:1;
			u32 dec:1;
			u32 nat:1;
			u32 tcp_err:1;
			u32 session_id:12;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer; /* dw2 */
	union {
		struct {
			u32 data_len:16;
			u32 mpoa_mode:2;
			u32 mpoa_pt:1;
			u32 qid:4;
			u32 byte_offset:3;
			u32 pdu:1;
			u32 dic:1;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	} __packed status; /* dw3 */
} __packed __aligned(16);

/* 2 DWs format descriptor */
struct dma_rx_desc_2dw {
	union {
		struct {
			u32 data_len:16;
			u32 reserve16_18:3;
			u32 rx_sideband:4;
			u32 byte_offset:2;
			u32 reserve_25_27:3;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);

struct dma_tx_desc_2dw {
	union {
		struct {
			u32 data_len:16;
			u32 reserved:7;
			u32 byte_offset:5;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	} __packed status;
	dma_addr_t data_pointer; /* Descriptor data pointer */
} __packed __aligned(8);
#endif /* CONFIG_CPU_BIG_ENDIAN */

/*!
 * \addtogroup LTQ_DMA_DRV_API
 */
/*@{*/

/*!
 * \fn char * (*buffer_alloc_t)(int len, int *byte_offset, void **opt)
 * \brief This callback function is for buffer allocation
 * \param[in] len   buffer length
 * \param[out] byte_offset   byte offset after alignment
 * \param[out] opt   optional return data pointer
 *
 * \return   NULL if failed to alloca memory
 * \return   allocated buffer pointer
 */
typedef char * (*buffer_alloc_t)(int len, int *byte_offset, void **opt);

/*!
 * \fn int (*buffer_free_t)(char *dataptr, void *opt)
 * \brief This callback function is for buffer free
 * \param[in] dataptr   to be freed data pointer
 * \param[out] opt   optional return data pointer
 *
 * \return   0 free memory successfully
 * \return   < 0 failed to free memory
 */
typedef int (*buffer_free_t)(char *dataptr, void *opt);

/*!
 * \fn int (*intr_handler_t)(u32 lnr, void *priv, int flags)
 * \brief This callback function is for interrupt callback
 * \param[in] lnr   logical channel number
 * \param[in] priv   interrupt specific private data
 * \param[in] flags   indicates TX/RX, buffer full interrupt
 *
 * \return   0 interrupt handling properly
 * \return   < 0, failed to handle interrupt
 */
typedef int (*intr_handler_t)(u32 lnr, void *priv, int flags);

/*!
 * \fn int ltq_request_dma(u32 chan, const char *device_id)
 * \brief Request and reserve a DMA channel
 * \param[in] chan   DMA logical channel number
 * \param[in] device_id   reserving device ID string, used to identify
 *			  who is the owner
 * \return   0 on success
 * \return   < 0 on failure
 *
 * This function is called before any other DMA API. The
 * requested DMA channel can be freed by calling \ref ltq_free_dma
 */
int ltq_request_dma(u32 chan, const char *device_id);

/*!
 * \fn int ltq_free_dma(u32 chan)
 * \brief Free a reserved DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   < 0 on failure
 *
 * This function is called after all DMA resource released
 */
int ltq_free_dma(u32 chan);

/*!
 * \fn int ltq_dma_chan_on(u32 chan)
 * \brief Switch on a DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function turns on the specified channel so that it
 * can start DMA transaction. Before calling this function,
 * assume that descriptor, data buffer has been prepared.
 * This channel can be turned off by calling \ref ltq_dma_chan_off
 */
int ltq_dma_chan_on(u32 chan);

/*!
 * \fn int ltq_dma_chan_off(u32 chan)
 * \brief Switch off a DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function turns off the specified channel.In case
 * it is planned to interrupt an ongoing transfer for
 * a while and the channel off mechanism should be used.
 * The transfer can be continued by switching the
 * channel on again at any time
 */
int ltq_dma_chan_off(u32 chan);

/*!
 * \fn int ltq_dma_chan_open(u32 chan)
 * \brief Open a DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function turns on the specified channel like
 * \ref ltq_dma_chan_on, it also enable interrupt for RX
 * channel
 */
int ltq_dma_chan_open(u32 chan);

/*!
 * \fn int ltq_dma_chan_close(u32 chan)
 * \brief Close a DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function turns off the specified channel like
 * \ref ltq_dma_chan_off, it also disable channel interrupt
 */
int ltq_dma_chan_close(u32 chan);

/*!
 * \fn int ltq_dma_chan_irq_enable(u32 chan)
 * \brief Enable channel interrupt
 * \param[in] chan   DMA logical channel number
 * \return 0 on success
 * \return kernel bug reported on failure
 *
 * This function enable channel interrupt. It normally
 * follows \ref ltq_dma_chan_on. \ref ltq_dma_chan_open is a
 * combination of \ref ltq_dma_chan_on and this function
 */
int ltq_dma_chan_irq_enable(u32 chan);

/*!
 * \fn int ltq_dma_chan_irq_disable(u32 chan)
 * \brief Disable channel interrupt
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function disable channel interrupt. It normally
 * follows \ref ltq_dma_chan_off. \ref ltq_dma_chan_close is a
 * combination of \ref ltq_dma_chan_off and this function
 */
int ltq_dma_chan_irq_disable(u32 chan);

/*!
 * \fn int ltq_dma_chan_reset(u32 chan)
 * \brief Reset a DMA channel
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure

 * This function resets the dma channel.In case a
 * channel need to be stopped immediately, only
 * the channel reset can be used.
 * A channel reset will reset the internal descriptor
 * list pointer inside the Descriptor Manager is
 * to the base address of the descriptor list
 * programmed in DMA_CDBA
 * it is recommended to use channel off at first to
 * make sure that the current transfer will be finished
 * smoothly. After the related last descriptor is finally
 * updated ) the channel can be reset to get a clean
 * starting point to restart this channel at any time.
 */
int ltq_dma_chan_reset(u32 chan);

/*!
 * \fn int ltq_dma_chan_pktsize_cfg(u32 chan, size_t pktsize)
 * \brief Set up channel packet size
 * \param[in] chan   DMA logical channel number
 * \param[in] pktsize   packet size
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configures packet size for the specified
 * dma channel
 */
int ltq_dma_chan_pktsize_cfg(u32 chan, size_t pktsize);

/*!
 * \fn int ltq_dma_chan_desc_alloc(u32 chan, u32 desc_num)
 * \brief Alloc dma descriptors
 * \param[in] chan   DMA logical channel number
 * \param[in] desc_num   descriptor number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function allocate dma descriptor for the specified
 * dma channel. It will gurannte the descriptor is coherent
 * The allocated descriptors can be freed by calling
 * \ref ltq_dma_chan_desc_free
 */
int ltq_dma_chan_desc_alloc(u32 chan, u32 desc_num);

/*!
 * \fn int ltq_dma_chan_desc_free(u32 chan)
 * \brief Free dma descriptors
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function free dma descriptor for the specified
 * dma channel.
 */
int ltq_dma_chan_desc_free(u32 chan);

/*!
 * \fn int ltq_dma_chan_data_buf_alloc(u32 chan)
 * \brief Alloc dma data buffer
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function allocate dma data buffer for the specified
 * dma channel. By default, it will call kmalloc to allocate
 * data buffer. However, callers can register their own
 * data buffer allocation callback function by calling
 * ltq_dma_chan_buf_alloc_callback_cfg first.
 * The allocated data buffer can be freed by calling
 * \ref ltq_dma_chan_desc_free
 */
int ltq_dma_chan_data_buf_alloc(u32 chan);

/*!
 * \fn int ltq_dma_chan_data_buf_free(u32 chan)
 * \brief Free dma data buffer
 * \param[in] chan   DMA logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function free dma data buffer for the specified
 * dma channel. By default, it will call kfree to free
 * data buffer. However, callers can register their own
 * data buffer free callback function by calling
 * \ref ltq_dma_chan_buf_free_callback_cfg. If callers register
 * their own data buffer allocation callback function
 * \ref ltq_dma_chan_buf_alloc_callback_cfg, they also should
 * register the corresponding data buffer free callback
 * function \ref ltq_dma_chan_buf_free_callback_cfg
 */
int ltq_dma_chan_data_buf_free(u32 chan);

/*!
 * \fn int ltq_dma_chan_buf_alloc_callback_cfg(u32 chan, buffer_alloc_t alloc)
 * \brief Register data buffer allocate callback
 * \param[in] chan   DMA logical channel number
 * \param[in] alloc   Data buffer allocation callback
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function register dma data buffer allocation callback function
 * It is called before \ref ltq_dma_chan_desc_alloc
 */
int ltq_dma_chan_buf_alloc_callback_cfg(u32 chan, buffer_alloc_t alloc);

/*!
 * \fn int ltq_dma_chan_buf_free_callback_cfg(u32 chan, buffer_free_t free)
 * \brief Register data buffer free callback
 * \param[in] chan DMA logical channel number
 * \param[in] free Data buffer free callback
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function register dma data buffer free callback function
 * It is called before \ref ltq_dma_chan_data_buf_free
 */
int ltq_dma_chan_buf_free_callback_cfg(u32 chan, buffer_free_t free);

/*!
 * \fn ltq_dma_chan_irq_callback_cfg(u32 chan, irq_handler_t handler,
 * void *data)
 * \brief Register channel interrupt callback
 * \param[in] chan   DMA logical channel number
 * \param[in] handler   interrupt handler
 * \param[in] data   interrupt handler specific data
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function register channel interrupt callback function. It provides
 * a chance to caller to change default channel interrupt handler.
 * It maybe is useful for some real-time application which wants to handler
 * DMA related stuff in hardware interrupt instead of soft interrupt context
 * Some more work and verification is needed for this API, extra data
 * structure has to be exported
 */
int ltq_dma_chan_irq_callback_cfg(u32 chan, irq_handler_t handler,
				  void *data);

/*!
 * \fn int ltq_dma_chan_pseudo_irq_handler_callback_cfg(u32 chan,
 * intr_handler_t handler, void *priv)
 * \brief Register pseudo interrupt callback
 * \param[in] chan   DMA logical channel number
 * \param[in] handler   pseudo interrupt handler
 * \param[in] priv   interrupt handler specific data
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function register channel pseudo interrupt callback function. Its major
 * purpose to provide tasklet level soft interrupt callback to be compatible
 * with legacy DMA/DMA0
 */
int ltq_dma_chan_pseudo_irq_handler_callback_cfg(u32 chan,
						 intr_handler_t handler,
						 void *priv);

/*!
 * \fn dma_addr_t ltq_dma_chan_get_desc_phys_base(u32 chan)
 * \brief Get descriptor base physical address
 * \param[in] chan   DMA logical channel number
 * \return channel descriptor base physical address
 *
 * This function returns the descriptor base physical address to
 * the caller.
 * In normal case, callers should not care about it. It provides a chance
 * to caller to manipulate the DMA related stuff by itself
 */
dma_addr_t ltq_dma_chan_get_desc_phys_base(u32 chan);

/*!
 * \fn int ltq_dma_chan_desc_cfg(u32 chan, dma_addr_t desc_base, int desc_num)
 * \brief Configure low level channel descriptors
 * \param[in] chan   DMA logical channel number
 * \param[in] desc_base   descriptor base physical address
 * \param[in] desc_num   number of descriptors
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configure the low level channel descriptors. It will be
 * used by CBM whose descriptor is not DDR, actually some registers.
 */
int ltq_dma_chan_desc_cfg(u32 chan, dma_addr_t desc_base, int desc_num);

/*!
 * \fn int ltq_dma_p2p_cfg(u32 rx_chan, u32 tx_chan)
 * \brief Peripheral to Peripheral configuration
 * \param[in] rx_chan   P2P RX logical channel number
 * \param[in] tx_chan   P2P TX logical channel number
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configure DMA P2P functions.
 * 1) RX and TX channel has the same descriptor list so that they can
 * link together.\n
 * 2) TX channel has to enable P2P\n
 * 3) Global buffer length has to be configured on TX channel DMA instance. \n
 * 4) P2P can be extended from intraDMA in leagcy SoC to interDMA.\n
 * Assume RX and TX has been configured using DMA descriptor and data
 * buffer functions such as \ref ltq_dma_chan_desc_alloc and
 * \ref ltq_dma_chan_data_buf_alloc
 */
int ltq_dma_p2p_cfg(u32 rx_chan, u32 tx_chan);

/*!
 * \fn int ltq_dma_chan_sync_desc_setup(u32 chan, char *buf, size_t len)
 * \brief Peripheral to Peripheral configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] buf   data buffer provided by caller, must be unmapped address
 * \param[in] len   length of data buffer
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configures application drived synchronous DMA operation
 * such as SSC/HSNAND. In these applications, callers will provide data
 * buffer for transmitting or receiving. Some textbook called this DMA
 * type as synchronous DMA to differentiate from external event driving
 * DMA like NIC and etc.
 */
int ltq_dma_chan_sync_desc_setup(u32 chan, char *buf, size_t len);

/*!
 * \fn int ltq_dma_chan_pkt_drop_cfg(u32 chan, int enable)
 * \brief DMA packet drop configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] enable   disable or enable channel packet drop
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configures dma channel packet drop (enable or disable)
 * dma also has port packet drop function. However, both port or channel
 * packet drop will be reflected on the same counter. In P2P case, packet
 * drop should not be enabled
 */
int ltq_dma_chan_pkt_drop_cfg(u32 chan, int enable);

/*!
 * \fn int ltq_dma_chan_txwgt_cfg(u32 chan, int txwgt)
 * \brief TX channel weight configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] txwgt   TX weight
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configures dma channel weight to change the transmission
 * priority. the higher the bigger
 */
int ltq_dma_chan_txwgt_cfg(u32 chan, int txwgt);

/*!
 * \fn int ltq_dma_chan_fast_path_cfg(u32 tx_chan, u32 rx_chan, int enable)
 * \brief DMA fast path configuration
 * \param[in] tx_chan   fast path dma tx logical channel
 * \param[in] rx_chan   Fast path dma rx logical channel
 * \param[in] enable: disable or enable fast path
 * \return   0 on success
 * \return   kernel bug reported on failure
 *
 * This function configures dma fast path function. There is no DDR
 * or descriptor involved at all. Only RX channel needs to be configure.
 * However, TX channel number has to be known before. It only applies to
 * the same DMA instance. There is no user case for GRX350/5xx
 */
int ltq_dma_chan_fast_path_cfg(u32 tx_chan, u32 rx_chan, int enable);

/*!
 * \fn int ltq_dma_chan_read(u32 chan, char **dataptr, void **opt)
 * \brief Get data packet from DMA
 * \param[in] chan   DMA logical channel number
 * \param[out] dataptr   pointer to received data
 * \param[out] opt   generic pointer
 * \return >0   valid packet data length
 * \return <0   on failure
 *
 * This function is called when the client driver gets a pseudo
 * DMA interrupt(RCV_INT). It is backward compatible with legacy DMA/DMA0
 */
int ltq_dma_chan_read(u32 chan, char **dataptr, void **opt);

/*!
 * \fn int ltq_dma_chan_write(u32 chan, char *dataptr, int len, int sop,
 * int eop, void *opt)
 * \brief Write data packet through DMA
 * \param[in] chan   DMA logical channel number
 * \param[in] dataptr   pointer to transmit data
 * \param[in] len   length of transmit data
 * \param[in] sop   start of packet
 * \param[in] eop   end of packet
 * \param[in] opt   generic pointer
 * \return >0   valid packet data length
 * \return <0   on failure
 *
 * This function gets the data packet from the client driver and
 * sends over on DMA channel. It also supports simple scatter/gather operation
 * by sop and eop. More advanced sg to be implemented
 */
int ltq_dma_chan_write(u32 chan, char *dataptr, int len, int sop, int eop,
		       void *opt);

/*!
 * \fn int ltq_dma_chan_polling_cfg(u32 chan, u32 nonarb_cnt, u32 arb_cnt)
 * \brief DMA per channel polling configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] nonarb_cnt   non arbitration polling counter
 * \param[in] arb_cnt   arbitration polling counter
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures per channel polling counter for arbitration and
 * non-arbitration channel. Please note, non-arbitration counter has lower
 * priority, therefore, non-arbitration counter should be less than arbitration
 * counter. Without per channel configuration, it will fall back to DMA global
 * polling counter
 */
int ltq_dma_chan_polling_cfg(u32 chan, u32 nonarb_cnt, u32 arb_cnt);

/*!
 * \fn int ltq_dma_chan_hdrm_cfg(u32 chan, u32 hdr_len)
 * \brief DMA per channel header only data transfer configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] hdr_len   header only data transfer mode header length
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel header only data transfer header
 * length. The valid header length should be less than 255. other value will
 * disable header mode data transfer
 */
int ltq_dma_chan_hdrm_cfg(u32 chan, u32 hdr_len);

/*!
 * \fn int ltq_dma_chan_byte_offset_cfg(u32 chan, u32 boff_len)
 * \brief DMA per channel byte offset length configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] boff_len   byte offset length length
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel byte offset length. The valid byte
 * offset length should be less than 255. other value will disalbe per channel
 * byte offset function
 */
int ltq_dma_chan_byte_offset_cfg(u32 chan, u32 boff_len);

/*!
 * \fn int ltq_dma_chan_int_coalesc_cfg(u32 chan, u32 coal_len, u32 timeout)
 * \brief DMA per channel interrupt coalescing configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] coal_len   interrupt coalescing number
 * \param[in] timeout    Interrupt timeout interval
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel interrupt coalescing number.
 * The valid interrupt coalescing length should be less than 255. other value
 * will disalbe per channel interrupt coalescing function. Timeout mechanism
 * also supported. For example, if the last several interrupts are less than
 * interrupt coalescing length, once timeout is configured, timeout interrupt
 * will be generated. It is the software responsbility to retrieve the pending
 * interrupts.
 */
int ltq_dma_chan_int_coalesc_cfg(u32 chan, u32 coal_len, u32 timeout);

/*!
 * \fn int ltq_dma_chan_sw_poll_cfg(u32 chan, int enable)
 * \brief DMA per channel software polling configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] enable   enable or disable software polling
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel software polling feature. Once
 * DMA descriptors prepared by software, then enable it. After descriptors
 * are used out by DMA, HW will clear it. Software has to re enable software
 * polling for next transaction. It will reduce hardware polling chances.
 */
int ltq_dma_chan_sw_poll_cfg(u32 chan, int enable);

/*!
 * \fn int ltq_dma_chan_data_endian_cfg(u32 chan, u32 endian_type)
 * \brief DMA per channel data payload endianness configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] endian_type   endianness types
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel data payload endianness feature
 * according to different endianness requirement. By default, without this
 * configuration, it will fall back to port based endianness configuration
 */
int ltq_dma_chan_data_endian_cfg(u32 chan, u32 endian_type);

/*!
 * \fn int ltq_dma_chan_data_endian_disable(u32 chan)
 * \brief DMA per channel data payload endianness disable configuration
 * \param[in] chan   DMA logical channel number
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel data payload endianness, then it
 * wll fall back to port based endianness configuation. It usually will be used
 * by shutdown routines.
 */
int ltq_dma_chan_data_endian_disable(u32 chan);

/*!
 * \fn int ltq_dma_chan_desc_endian_cfg(u32 chan, u32 endian_type)
 * \brief DMA per channel descriptor endianness configuration
 * \param[in] chan   DMA logical channel number
 * \param[in] endian_type   endianness types
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel descriptor endianness feature
 * according to different endianness requirement.By default, without this
 * configuration, it will fall back to port based endianness configuration
 */
int ltq_dma_chan_desc_endian_cfg(u32 chan, u32 endian_type);

/*!
 * \fn int ltq_dma_chan_desc_endian_disable(u32 chan)
 * \brief DMA per channel descriptor endianness disable configuration
 * \param[in] chan   DMA logical channel number
 * \return 0    on success
 * \return -EINVAL   invalid input parameters
 * \return -EPERM    hardware not supported this feature
 *
 * This function configures DMA per channel descriptor endianness, then it
 * wll fall back to port based endianness configuation. It usually will be used
 * by shutdown routines.
 */
int ltq_dma_chan_desc_endian_disable(u32 chan);

u32 ltq_dma_chan_get_desc_vir_base(u32 chan);

/* @ } */
#endif /* LANTIQ_DMAX_H */

