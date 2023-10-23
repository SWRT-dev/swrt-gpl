/**
 * txmngr_4k_regs.h
 * Description: txmngr_4k_s HW registers definitions
 *              NOTE: B Step ONLY module
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_TX_MGR_4K_H_
#define _PP_TX_MGR_4K_H_

#define PP_TX_MGR_4K_GEN_DATE_STR             "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_TX_MGR_4K_BASE                                        (0xF1300000ULL)

/**
 * SW_REG_NAME : PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_REG
 * HW_REG_NAME : dbg_txport_act_pkt_crdt_reg
 * DESCRIPTION : Debug TXport Actual Packet credit, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - Actual packet credit of port. The amount of packet
 *                 credits the port currently has
 *
 */
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_REG ((TX_MGR_4K_BASE_ADDR) + 0x950)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_DBG_TXPORT_PKT_CRDT_OFF         (0)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_DBG_TXPORT_PKT_CRDT_LEN        (32)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_DBG_TXPORT_PKT_CRDT_MSK \
	(0xFFFFFFFF)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_DBG_TXPORT_PKT_CRDT_RST       (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_DBG_TXPORT_ACT_PKT_CRDT_REG + ((idx) << 12))

/**
 * SW_REG_NAME : PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_REG
 * HW_REG_NAME : dbg_txport_act_byte_crdt_reg
 * DESCRIPTION : Debug TXport Actual Byte credit, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RO] - Actual byte credit of port. The amount of byte
 *                 credits the port currently has
 *
 */
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_REG \
	((TX_MGR_4K_BASE_ADDR) + 0x954)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_DBG_TXPORT_BYTE_CRDT_OFF       (0)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_DBG_TXPORT_BYTE_CRDT_LEN      (32)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_DBG_TXPORT_BYTE_CRDT_MSK \
	(0xFFFFFFFF)
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_DBG_TXPORT_BYTE_CRDT_RST     (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_DBG_TXPORT_ACT_BYTE_CRDT_REG + ((idx) << 12))

/**
 * SW_REG_NAME : PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_REG
 * HW_REG_NAME : txmngr_txport_add_pkt_crdt
 * DESCRIPTION : pkt_crdt, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][WO] - Add packet credit to current port credit. The
 *                 amount of credits written is added to the current
 *                 amount of packet credit for the port
 *
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_REG \
	((TX_MGR_4K_BASE_ADDR) + 0x8)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_OFF                 (0)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_LEN                 (32)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_MSK                 (0xFFFFFFFF)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_RST                 (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_PKT_CRDT_REG + ((idx) << 12))

/**
 * SW_REG_NAME : PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG
 * HW_REG_NAME : txmngr_txport_add_byte_crdt
 * DESCRIPTION : byte_crdt, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][WO] - Add Byte credit to current port credit. The amount
 *                 of credits written is added to the current amount
 *                 of byte credit for the port
 *
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG \
	((TX_MGR_4K_BASE_ADDR) + 0x0)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_OFF                (0)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_LEN                (32)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_MSK                (0xFFFFFFFF)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_RST                (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG + ((idx) << 12))

/**
 * SW_REG_NAME : PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_REG
 * HW_REG_NAME : txmngr_txport_ovr_pkt_crdt
 * DESCRIPTION : pkt_crdt, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][WO] - Override packet credit of port credit. The amount
 *                 of credits written overrides the current amount of
 *                 packet credit for the port and used as initial
 *                 value
 *
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_REG \
	((TX_MGR_4K_BASE_ADDR) + 0xc)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_OFF                 (0)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_LEN                 (32)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_MSK                 (0xFFFFFFFF)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_RST                 (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_PKT_CRDT_REG + ((idx) << 12))

/**
 * SW_REG_NAME : PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG
 * HW_REG_NAME : txmngr_txport_ovr_byte_crdt
 * DESCRIPTION : byte_crdt, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][WO] - Override byte credit of port credit. The amount of
 *                 credits written overrides the current amount of
 *                 byte credit for the port and used as initial value
 *
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG \
	((TX_MGR_4K_BASE_ADDR) + 0x4)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_OFF                (0)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_LEN                (32)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_MSK                (0xFFFFFFFF)
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_RST                (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_4K_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG + ((idx) << 12))

#endif
