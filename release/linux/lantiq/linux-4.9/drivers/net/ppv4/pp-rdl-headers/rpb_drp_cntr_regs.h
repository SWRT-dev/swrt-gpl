/**
 * rpb_drp_cntr_regs.h
 * Description: ppv4_rpb_drp_cntr_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_RPB_DRP_H_
#define _PP_RPB_DRP_H_

#define PP_RPB_DRP_GEN_DATE_STR               "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_RPB_DRP_BASE                                          (0xF0010000ULL)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT0_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt0_tc0
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC0_REG             ((RPB_DRP_BASE_ADDR) + 0x0)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC0_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC0_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC0_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC0_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT0_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt0_tc1
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC1_REG             ((RPB_DRP_BASE_ADDR) + 0x4)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC1_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC1_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC1_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC1_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT0_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt0_tc2
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC2_REG             ((RPB_DRP_BASE_ADDR) + 0x8)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC2_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC2_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC2_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC2_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT0_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt0_tc3
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC3_REG            ((RPB_DRP_BASE_ADDR) + 0x0C)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC3_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC3_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC3_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT0_TC3_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT1_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt1_tc0
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC0_REG            ((RPB_DRP_BASE_ADDR) + 0x10)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC0_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC0_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC0_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC0_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT1_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt1_tc1
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC1_REG            ((RPB_DRP_BASE_ADDR) + 0x14)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC1_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC1_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC1_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC1_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT1_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt1_tc2
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC2_REG            ((RPB_DRP_BASE_ADDR) + 0x18)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC2_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC2_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC2_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC2_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT1_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt1_tc3
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC3_REG            ((RPB_DRP_BASE_ADDR) + 0x1C)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC3_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC3_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC3_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT1_TC3_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT2_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt2_tc0
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC0_REG            ((RPB_DRP_BASE_ADDR) + 0x20)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC0_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC0_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC0_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC0_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT2_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt2_tc1
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC1_REG            ((RPB_DRP_BASE_ADDR) + 0x24)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC1_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC1_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC1_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC1_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT2_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt2_tc2
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC2_REG            ((RPB_DRP_BASE_ADDR) + 0x28)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC2_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC2_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC2_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC2_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT2_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt2_tc3
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC3_REG            ((RPB_DRP_BASE_ADDR) + 0x2C)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC3_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC3_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC3_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT2_TC3_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT3_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt3_tc0
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC0_REG            ((RPB_DRP_BASE_ADDR) + 0x30)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC0_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC0_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC0_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC0_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT3_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt3_tc1
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC1_REG            ((RPB_DRP_BASE_ADDR) + 0x34)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC1_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC1_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC1_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC1_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT3_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt3_tc2
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC2_REG            ((RPB_DRP_BASE_ADDR) + 0x38)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC2_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC2_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC2_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC2_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_PKT_PRT3_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_pkt_prt3_tc3
 * DESCRIPTION : Drop Packets per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC3_REG            ((RPB_DRP_BASE_ADDR) + 0x3C)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC3_RPB_DPKT_CNTR_OFF              (0)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC3_RPB_DPKT_CNTR_LEN              (32)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC3_RPB_DPKT_CNTR_MSK              (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_PKT_PRT3_TC3_RPB_DPKT_CNTR_RST              (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt0_tc0
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_REG           ((RPB_DRP_BASE_ADDR) + 0x40)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt0_tc1
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_REG           ((RPB_DRP_BASE_ADDR) + 0x44)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC1_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt0_tc2
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_REG           ((RPB_DRP_BASE_ADDR) + 0x48)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC2_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt0_tc3
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_REG           ((RPB_DRP_BASE_ADDR) + 0x4C)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT0_TC3_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt1_tc0
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_REG           ((RPB_DRP_BASE_ADDR) + 0x50)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC0_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt1_tc1
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_REG           ((RPB_DRP_BASE_ADDR) + 0x54)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC1_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt1_tc2
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_REG           ((RPB_DRP_BASE_ADDR) + 0x58)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC2_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt1_tc3
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_REG           ((RPB_DRP_BASE_ADDR) + 0x5C)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT1_TC3_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt2_tc0
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_REG           ((RPB_DRP_BASE_ADDR) + 0x60)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC0_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt2_tc1
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_REG           ((RPB_DRP_BASE_ADDR) + 0x64)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC1_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt2_tc2
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_REG           ((RPB_DRP_BASE_ADDR) + 0x68)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC2_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt2_tc3
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_REG           ((RPB_DRP_BASE_ADDR) + 0x6C)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT2_TC3_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt3_tc0
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_REG           ((RPB_DRP_BASE_ADDR) + 0x70)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC0_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt3_tc1
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_REG           ((RPB_DRP_BASE_ADDR) + 0x74)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC1_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt3_tc2
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_REG           ((RPB_DRP_BASE_ADDR) + 0x78)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC2_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_REG
 * HW_REG_NAME : rpb_drp_cntr_byte_prt3_tc3
 * DESCRIPTION : Drop Bytes in Packet per port per TC
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of Bytes in packets dropped per Port per TC
 *
 */
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_REG           ((RPB_DRP_BASE_ADDR) + 0x7C)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_RPB_DBYTE_CNTR_OFF            (0)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_RPB_DBYTE_CNTR_LEN            (32)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_RPB_DBYTE_CNTR_MSK            (0xFFFFFFFF)
#define PP_RPB_DRP_CNTR_BYTE_PRT3_TC3_RPB_DBYTE_CNTR_RST            (0x0)

/**
 * SW_REG_NAME : PP_RPB_DRP_BM_BASE_ADDRH_REG
 * HW_REG_NAME : BM_base_addrh_reg
 * DESCRIPTION : Base address high
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_RPB_DRP_BM_BASE_ADDRH_REG                ((RPB_DRP_BASE_ADDR) + 0x80)
#define PP_RPB_DRP_BM_BASE_ADDRH_RSV_OFF                            (4)
#define PP_RPB_DRP_BM_BASE_ADDRH_RSV_LEN                            (28)
#define PP_RPB_DRP_BM_BASE_ADDRH_RSV_MSK                            (0xFFFFFFF0)
#define PP_RPB_DRP_BM_BASE_ADDRH_RSV_RST                            (0x0)
#define PP_RPB_DRP_BM_BASE_ADDRH_OFF                                (0)
#define PP_RPB_DRP_BM_BASE_ADDRH_LEN                                (4)
#define PP_RPB_DRP_BM_BASE_ADDRH_MSK                                (0x0000000F)
#define PP_RPB_DRP_BM_BASE_ADDRH_RST                                (0xf)

/**
 * SW_REG_NAME : PP_RPB_DRP_BM_BASE_ADDRL_REG
 * HW_REG_NAME : BM_base_addrl_reg
 * DESCRIPTION : Base address Low
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_RPB_DRP_BM_BASE_ADDRL_REG                ((RPB_DRP_BASE_ADDR) + 0x84)
#define PP_RPB_DRP_BM_BASE_ADDRL_OFF                                (0)
#define PP_RPB_DRP_BM_BASE_ADDRL_LEN                                (32)
#define PP_RPB_DRP_BM_BASE_ADDRL_MSK                                (0xFFFFFFFF)
#define PP_RPB_DRP_BM_BASE_ADDRL_RST                                (0xfe080000)

#endif
