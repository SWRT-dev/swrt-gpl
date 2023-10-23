/**
 * txmngr_crdt_regs.h
 * Description: txmngr_crdt_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_TX_MGR_CRDT_H_
#define _PP_TX_MGR_CRDT_H_

#define PP_TX_MGR_CRDT_GEN_DATE_STR           "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_TX_MGR_CRDT_BASE                                      (0xF1200400ULL)

/**
 * SW_REG_NAME : PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_REG
 * HW_REG_NAME : txmngr_txport_add_pkt_crdt
 * DESCRIPTION : pkt_crdt
 *
 *  Register Fields :
 *   [31: 0][WO] - Add packet credit to current port credit. The
 *                 amount of credits written is added to the current
 *                 amount of packet credit for the port
 *
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_REG \
	((TX_MGR_CRDT_BASE_ADDR) + 0x8)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_OFF               (0)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_LEN               (32)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_MSK               (0xFFFFFFFF)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_PKT_CRDT_REG + ((idx) << 4))

/**
 * SW_REG_NAME : PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG
 * HW_REG_NAME : txmngr_txport_add_byte_crdt
 * DESCRIPTION : byte_crdt
 *
 *  Register Fields :
 *   [31: 0][WO] - Add Byte credit to current port credit. The amount
 *                 of credits written is added to the current amount
 *                 of byte credit for the port
 *
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG \
	((TX_MGR_CRDT_BASE_ADDR) + 0x0)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_OFF              (0)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_LEN              (32)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_MSK              (0xFFFFFFFF)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_CRDT_TXMNGR_TXPORT_ADD_BYTE_CRDT_REG + ((idx) << 4))

/**
 * SW_REG_NAME : PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_REG
 * HW_REG_NAME : txmngr_txport_ovr_pkt_crdt
 * DESCRIPTION : pkt_crdt
 *
 *  Register Fields :
 *   [31: 0][WO] - Override packet credit of port credit. The amount
 *                 of credits written overrides the current amount of
 *                 packet credit for the port and used as initial
 *                 value
 *
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_REG \
	((TX_MGR_CRDT_BASE_ADDR) + 0xc)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_OFF               (0)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_LEN               (32)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_MSK               (0xFFFFFFFF)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_PKT_CRDT_REG + ((idx) << 4))

/**
 * SW_REG_NAME : PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG
 * HW_REG_NAME : txmngr_txport_ovr_byte_crdt
 * DESCRIPTION : byte_crdt
 *
 *  Register Fields :
 *   [31: 0][WO] - Override byte credit of port credit. The amount of
 *                 credits written overrides the current amount of
 *                 byte credit for the port and used as initial value
 *
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG \
	((TX_MGR_CRDT_BASE_ADDR) + 0x4)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_OFF              (0)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_LEN              (32)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_MSK              (0xFFFFFFFF)
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG_IDX(idx) \
	(PP_TX_MGR_CRDT_TXMNGR_TXPORT_OVR_BYTE_CRDT_REG + ((idx) << 4))

#endif
