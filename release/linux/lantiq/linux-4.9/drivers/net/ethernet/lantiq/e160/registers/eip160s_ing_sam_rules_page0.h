// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP or GPHY
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_SAM_rules_page0_def.xml
// Register File Name  : ING_SAM_RULES_PAGE0
// Register File Title : EIPI EIP_160s_i_32_SAM_rules_page0
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_SAM_RULES_PAGE0_H
#define _ING_SAM_RULES_PAGE0_H

//! \defgroup ING_SAM_RULES_PAGE0 Register File ING_SAM_RULES_PAGE0 - EIPI EIP_160s_i_32_SAM_rules_page0
//! @{

//! Base Address of ING_SAM_RULES_PAGE0
#define ING_SAM_RULES_PAGE0_MODULE_BASE 0x4000u

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_0 Register EIPI_SAM_MAC_SA_MATCH_LO_0 - EIPI SAM MAC SA MATCH LO 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_0 0x0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_0 0x4000u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_0_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_0_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_0_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_0 Register EIPI_SAM_MAC_SA_MATCH_HI_0 - EIPI SAM MAC SA MATCH HI 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_0 0x4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_0 0x4004u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_0_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_0_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_0_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_0_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_0_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_0 Register EIPI_SAM_MAC_DA_MATCH_LO_0 - EIPI SAM MAC DA MATCH LO 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_0 0x8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_0 0x4008u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_0_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_0_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_0_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_0 Register EIPI_SAM_MAC_DA_MATCH_HI_0 - EIPI SAM MAC DA MATCH HI 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_0 0xC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_0 0x400Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_0_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_0_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_0 Register EIPI_SAM_MAC_MISC_MATCH_0 - EIPI SAM MAC MISC MATCH 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_0 0x10
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_0 0x4010u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_0_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_0_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_0 Register EIPI_SAM_SCI_MATCH_LO_0 - EIPI SAM SCI MATCH LO 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_0 0x14
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_0 0x4014u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_0_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_0_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_0_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_0 Register EIPI_SAM_SCI_MATCH_HI_0 - EIPI SAM SCI MATCH HI 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_0 0x18
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_0 0x4018u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_0_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_0_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_0_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_0 Register EIPI_SAM_MASK_0 - EIPI SAM MASK 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_0 0x1C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_0 0x401Cu

//! Register Reset Value
#define EIPI_SAM_MASK_0_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_0_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_0_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_0_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_0_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_0_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_0_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_0_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_0_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_0_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_0_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_0_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_0_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_0_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_0 Register EIPI_SAM_EXT_MATCH_0 - EIPI SAM EXT MATCH 0
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_0 0x20
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_0 0x4020u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_0_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_0_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_0_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_0_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_0_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_0_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_0_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_0_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_0_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_0_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_0_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_1 Register EIPI_SAM_MAC_SA_MATCH_LO_1 - EIPI SAM MAC SA MATCH LO 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_1 0x40
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_1 0x4040u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_1_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_1_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_1_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_1 Register EIPI_SAM_MAC_SA_MATCH_HI_1 - EIPI SAM MAC SA MATCH HI 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_1 0x44
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_1 0x4044u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_1_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_1_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_1_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_1_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_1_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_1 Register EIPI_SAM_MAC_DA_MATCH_LO_1 - EIPI SAM MAC DA MATCH LO 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_1 0x48
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_1 0x4048u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_1_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_1_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_1_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_1 Register EIPI_SAM_MAC_DA_MATCH_HI_1 - EIPI SAM MAC DA MATCH HI 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_1 0x4C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_1 0x404Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_1_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_1_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_1 Register EIPI_SAM_MAC_MISC_MATCH_1 - EIPI SAM MAC MISC MATCH 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_1 0x50
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_1 0x4050u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_1_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_1_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_1 Register EIPI_SAM_SCI_MATCH_LO_1 - EIPI SAM SCI MATCH LO 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_1 0x54
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_1 0x4054u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_1_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_1_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_1_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_1 Register EIPI_SAM_SCI_MATCH_HI_1 - EIPI SAM SCI MATCH HI 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_1 0x58
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_1 0x4058u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_1_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_1_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_1_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_1 Register EIPI_SAM_MASK_1 - EIPI SAM MASK 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_1 0x5C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_1 0x405Cu

//! Register Reset Value
#define EIPI_SAM_MASK_1_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_1_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_1_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_1_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_1_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_1_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_1_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_1_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_1_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_1_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_1_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_1_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_1_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_1_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_1 Register EIPI_SAM_EXT_MATCH_1 - EIPI SAM EXT MATCH 1
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_1 0x60
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_1 0x4060u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_1_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_1_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_1_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_1_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_1_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_1_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_1_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_1_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_1_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_1_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_1_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_2 Register EIPI_SAM_MAC_SA_MATCH_LO_2 - EIPI SAM MAC SA MATCH LO 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_2 0x80
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_2 0x4080u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_2_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_2_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_2_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_2 Register EIPI_SAM_MAC_SA_MATCH_HI_2 - EIPI SAM MAC SA MATCH HI 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_2 0x84
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_2 0x4084u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_2_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_2_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_2_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_2_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_2_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_2 Register EIPI_SAM_MAC_DA_MATCH_LO_2 - EIPI SAM MAC DA MATCH LO 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_2 0x88
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_2 0x4088u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_2_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_2_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_2_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_2 Register EIPI_SAM_MAC_DA_MATCH_HI_2 - EIPI SAM MAC DA MATCH HI 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_2 0x8C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_2 0x408Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_2_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_2_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_2 Register EIPI_SAM_MAC_MISC_MATCH_2 - EIPI SAM MAC MISC MATCH 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_2 0x90
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_2 0x4090u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_2_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_2_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_2 Register EIPI_SAM_SCI_MATCH_LO_2 - EIPI SAM SCI MATCH LO 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_2 0x94
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_2 0x4094u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_2_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_2_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_2_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_2 Register EIPI_SAM_SCI_MATCH_HI_2 - EIPI SAM SCI MATCH HI 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_2 0x98
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_2 0x4098u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_2_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_2_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_2_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_2 Register EIPI_SAM_MASK_2 - EIPI SAM MASK 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_2 0x9C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_2 0x409Cu

//! Register Reset Value
#define EIPI_SAM_MASK_2_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_2_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_2_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_2_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_2_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_2_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_2_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_2_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_2_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_2_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_2_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_2_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_2_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_2_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_2 Register EIPI_SAM_EXT_MATCH_2 - EIPI SAM EXT MATCH 2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_2 0xA0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_2 0x40A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_2_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_2_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_2_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_2_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_2_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_2_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_2_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_2_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_2_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_2_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_2_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_3 Register EIPI_SAM_MAC_SA_MATCH_LO_3 - EIPI SAM MAC SA MATCH LO 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_3 0xC0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_3 0x40C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_3_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_3_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_3_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_3 Register EIPI_SAM_MAC_SA_MATCH_HI_3 - EIPI SAM MAC SA MATCH HI 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_3 0xC4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_3 0x40C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_3_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_3_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_3_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_3_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_3_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_3 Register EIPI_SAM_MAC_DA_MATCH_LO_3 - EIPI SAM MAC DA MATCH LO 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_3 0xC8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_3 0x40C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_3_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_3_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_3_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_3 Register EIPI_SAM_MAC_DA_MATCH_HI_3 - EIPI SAM MAC DA MATCH HI 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_3 0xCC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_3 0x40CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_3_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_3_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_3 Register EIPI_SAM_MAC_MISC_MATCH_3 - EIPI SAM MAC MISC MATCH 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_3 0xD0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_3 0x40D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_3_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_3_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_3 Register EIPI_SAM_SCI_MATCH_LO_3 - EIPI SAM SCI MATCH LO 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_3 0xD4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_3 0x40D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_3_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_3_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_3_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_3 Register EIPI_SAM_SCI_MATCH_HI_3 - EIPI SAM SCI MATCH HI 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_3 0xD8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_3 0x40D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_3_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_3_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_3_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_3 Register EIPI_SAM_MASK_3 - EIPI SAM MASK 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_3 0xDC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_3 0x40DCu

//! Register Reset Value
#define EIPI_SAM_MASK_3_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_3_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_3_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_3_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_3_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_3_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_3_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_3_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_3_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_3_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_3_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_3_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_3_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_3_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_3 Register EIPI_SAM_EXT_MATCH_3 - EIPI SAM EXT MATCH 3
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_3 0xE0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_3 0x40E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_3_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_3_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_3_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_3_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_3_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_3_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_3_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_3_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_3_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_3_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_3_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_4 Register EIPI_SAM_MAC_SA_MATCH_LO_4 - EIPI SAM MAC SA MATCH LO 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_4 0x100
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_4 0x4100u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_4_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_4_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_4_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_4 Register EIPI_SAM_MAC_SA_MATCH_HI_4 - EIPI SAM MAC SA MATCH HI 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_4 0x104
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_4 0x4104u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_4_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_4_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_4_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_4_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_4_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_4 Register EIPI_SAM_MAC_DA_MATCH_LO_4 - EIPI SAM MAC DA MATCH LO 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_4 0x108
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_4 0x4108u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_4_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_4_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_4_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_4 Register EIPI_SAM_MAC_DA_MATCH_HI_4 - EIPI SAM MAC DA MATCH HI 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_4 0x10C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_4 0x410Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_4_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_4_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_4 Register EIPI_SAM_MAC_MISC_MATCH_4 - EIPI SAM MAC MISC MATCH 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_4 0x110
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_4 0x4110u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_4_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_4_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_4 Register EIPI_SAM_SCI_MATCH_LO_4 - EIPI SAM SCI MATCH LO 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_4 0x114
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_4 0x4114u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_4_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_4_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_4_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_4 Register EIPI_SAM_SCI_MATCH_HI_4 - EIPI SAM SCI MATCH HI 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_4 0x118
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_4 0x4118u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_4_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_4_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_4_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_4 Register EIPI_SAM_MASK_4 - EIPI SAM MASK 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_4 0x11C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_4 0x411Cu

//! Register Reset Value
#define EIPI_SAM_MASK_4_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_4_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_4_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_4_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_4_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_4_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_4_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_4_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_4_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_4_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_4_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_4_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_4_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_4_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_4 Register EIPI_SAM_EXT_MATCH_4 - EIPI SAM EXT MATCH 4
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_4 0x120
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_4 0x4120u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_4_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_4_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_4_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_4_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_4_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_4_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_4_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_4_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_4_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_4_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_4_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_5 Register EIPI_SAM_MAC_SA_MATCH_LO_5 - EIPI SAM MAC SA MATCH LO 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_5 0x140
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_5 0x4140u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_5_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_5_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_5_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_5 Register EIPI_SAM_MAC_SA_MATCH_HI_5 - EIPI SAM MAC SA MATCH HI 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_5 0x144
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_5 0x4144u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_5_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_5_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_5_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_5_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_5_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_5 Register EIPI_SAM_MAC_DA_MATCH_LO_5 - EIPI SAM MAC DA MATCH LO 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_5 0x148
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_5 0x4148u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_5_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_5_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_5_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_5 Register EIPI_SAM_MAC_DA_MATCH_HI_5 - EIPI SAM MAC DA MATCH HI 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_5 0x14C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_5 0x414Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_5_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_5_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_5 Register EIPI_SAM_MAC_MISC_MATCH_5 - EIPI SAM MAC MISC MATCH 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_5 0x150
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_5 0x4150u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_5_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_5_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_5 Register EIPI_SAM_SCI_MATCH_LO_5 - EIPI SAM SCI MATCH LO 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_5 0x154
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_5 0x4154u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_5_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_5_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_5_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_5 Register EIPI_SAM_SCI_MATCH_HI_5 - EIPI SAM SCI MATCH HI 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_5 0x158
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_5 0x4158u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_5_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_5_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_5_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_5 Register EIPI_SAM_MASK_5 - EIPI SAM MASK 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_5 0x15C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_5 0x415Cu

//! Register Reset Value
#define EIPI_SAM_MASK_5_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_5_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_5_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_5_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_5_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_5_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_5_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_5_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_5_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_5_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_5_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_5_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_5_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_5_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_5 Register EIPI_SAM_EXT_MATCH_5 - EIPI SAM EXT MATCH 5
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_5 0x160
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_5 0x4160u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_5_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_5_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_5_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_5_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_5_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_5_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_5_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_5_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_5_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_5_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_5_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_6 Register EIPI_SAM_MAC_SA_MATCH_LO_6 - EIPI SAM MAC SA MATCH LO 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_6 0x180
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_6 0x4180u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_6_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_6_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_6_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_6 Register EIPI_SAM_MAC_SA_MATCH_HI_6 - EIPI SAM MAC SA MATCH HI 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_6 0x184
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_6 0x4184u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_6_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_6_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_6_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_6_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_6_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_6 Register EIPI_SAM_MAC_DA_MATCH_LO_6 - EIPI SAM MAC DA MATCH LO 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_6 0x188
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_6 0x4188u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_6_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_6_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_6_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_6 Register EIPI_SAM_MAC_DA_MATCH_HI_6 - EIPI SAM MAC DA MATCH HI 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_6 0x18C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_6 0x418Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_6_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_6_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_6 Register EIPI_SAM_MAC_MISC_MATCH_6 - EIPI SAM MAC MISC MATCH 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_6 0x190
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_6 0x4190u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_6_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_6_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_6 Register EIPI_SAM_SCI_MATCH_LO_6 - EIPI SAM SCI MATCH LO 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_6 0x194
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_6 0x4194u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_6_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_6_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_6_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_6 Register EIPI_SAM_SCI_MATCH_HI_6 - EIPI SAM SCI MATCH HI 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_6 0x198
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_6 0x4198u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_6_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_6_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_6_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_6 Register EIPI_SAM_MASK_6 - EIPI SAM MASK 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_6 0x19C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_6 0x419Cu

//! Register Reset Value
#define EIPI_SAM_MASK_6_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_6_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_6_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_6_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_6_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_6_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_6_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_6_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_6_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_6_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_6_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_6_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_6_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_6_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_6 Register EIPI_SAM_EXT_MATCH_6 - EIPI SAM EXT MATCH 6
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_6 0x1A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_6 0x41A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_6_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_6_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_6_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_6_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_6_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_6_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_6_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_6_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_6_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_6_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_6_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_7 Register EIPI_SAM_MAC_SA_MATCH_LO_7 - EIPI SAM MAC SA MATCH LO 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_7 0x1C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_7 0x41C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_7_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_7_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_7_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_7 Register EIPI_SAM_MAC_SA_MATCH_HI_7 - EIPI SAM MAC SA MATCH HI 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_7 0x1C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_7 0x41C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_7_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_7_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_7_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_7_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_7_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_7 Register EIPI_SAM_MAC_DA_MATCH_LO_7 - EIPI SAM MAC DA MATCH LO 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_7 0x1C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_7 0x41C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_7_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_7_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_7_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_7 Register EIPI_SAM_MAC_DA_MATCH_HI_7 - EIPI SAM MAC DA MATCH HI 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_7 0x1CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_7 0x41CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_7_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_7_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_7 Register EIPI_SAM_MAC_MISC_MATCH_7 - EIPI SAM MAC MISC MATCH 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_7 0x1D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_7 0x41D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_7_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_7_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_7 Register EIPI_SAM_SCI_MATCH_LO_7 - EIPI SAM SCI MATCH LO 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_7 0x1D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_7 0x41D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_7_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_7_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_7_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_7 Register EIPI_SAM_SCI_MATCH_HI_7 - EIPI SAM SCI MATCH HI 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_7 0x1D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_7 0x41D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_7_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_7_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_7_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_7 Register EIPI_SAM_MASK_7 - EIPI SAM MASK 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_7 0x1DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_7 0x41DCu

//! Register Reset Value
#define EIPI_SAM_MASK_7_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_7_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_7_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_7_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_7_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_7_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_7_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_7_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_7_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_7_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_7_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_7_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_7_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_7_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_7 Register EIPI_SAM_EXT_MATCH_7 - EIPI SAM EXT MATCH 7
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_7 0x1E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_7 0x41E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_7_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_7_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_7_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_7_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_7_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_7_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_7_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_7_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_7_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_7_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_7_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_8 Register EIPI_SAM_MAC_SA_MATCH_LO_8 - EIPI SAM MAC SA MATCH LO 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_8 0x200
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_8 0x4200u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_8_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_8_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_8_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_8 Register EIPI_SAM_MAC_SA_MATCH_HI_8 - EIPI SAM MAC SA MATCH HI 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_8 0x204
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_8 0x4204u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_8_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_8_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_8_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_8_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_8_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_8 Register EIPI_SAM_MAC_DA_MATCH_LO_8 - EIPI SAM MAC DA MATCH LO 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_8 0x208
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_8 0x4208u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_8_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_8_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_8_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_8 Register EIPI_SAM_MAC_DA_MATCH_HI_8 - EIPI SAM MAC DA MATCH HI 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_8 0x20C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_8 0x420Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_8_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_8_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_8 Register EIPI_SAM_MAC_MISC_MATCH_8 - EIPI SAM MAC MISC MATCH 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_8 0x210
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_8 0x4210u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_8_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_8_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_8 Register EIPI_SAM_SCI_MATCH_LO_8 - EIPI SAM SCI MATCH LO 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_8 0x214
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_8 0x4214u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_8_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_8_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_8_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_8 Register EIPI_SAM_SCI_MATCH_HI_8 - EIPI SAM SCI MATCH HI 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_8 0x218
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_8 0x4218u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_8_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_8_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_8_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_8 Register EIPI_SAM_MASK_8 - EIPI SAM MASK 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_8 0x21C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_8 0x421Cu

//! Register Reset Value
#define EIPI_SAM_MASK_8_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_8_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_8_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_8_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_8_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_8_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_8_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_8_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_8_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_8_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_8_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_8_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_8_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_8_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_8 Register EIPI_SAM_EXT_MATCH_8 - EIPI SAM EXT MATCH 8
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_8 0x220
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_8 0x4220u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_8_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_8_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_8_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_8_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_8_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_8_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_8_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_8_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_8_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_8_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_8_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_9 Register EIPI_SAM_MAC_SA_MATCH_LO_9 - EIPI SAM MAC SA MATCH LO 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_9 0x240
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_9 0x4240u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_9_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_9_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_9_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_9 Register EIPI_SAM_MAC_SA_MATCH_HI_9 - EIPI SAM MAC SA MATCH HI 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_9 0x244
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_9 0x4244u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_9_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_9_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_9_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_9_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_9_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_9 Register EIPI_SAM_MAC_DA_MATCH_LO_9 - EIPI SAM MAC DA MATCH LO 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_9 0x248
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_9 0x4248u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_9_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_9_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_9_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_9 Register EIPI_SAM_MAC_DA_MATCH_HI_9 - EIPI SAM MAC DA MATCH HI 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_9 0x24C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_9 0x424Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_9_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_9_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_9 Register EIPI_SAM_MAC_MISC_MATCH_9 - EIPI SAM MAC MISC MATCH 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_9 0x250
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_9 0x4250u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_9_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_9_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_9 Register EIPI_SAM_SCI_MATCH_LO_9 - EIPI SAM SCI MATCH LO 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_9 0x254
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_9 0x4254u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_9_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_9_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_9_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_9 Register EIPI_SAM_SCI_MATCH_HI_9 - EIPI SAM SCI MATCH HI 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_9 0x258
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_9 0x4258u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_9_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_9_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_9_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_9 Register EIPI_SAM_MASK_9 - EIPI SAM MASK 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_9 0x25C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_9 0x425Cu

//! Register Reset Value
#define EIPI_SAM_MASK_9_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_9_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_9_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_9_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_9_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_9_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_9_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_9_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_9_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_9_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_9_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_9_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_9_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_9_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_9 Register EIPI_SAM_EXT_MATCH_9 - EIPI SAM EXT MATCH 9
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_9 0x260
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_9 0x4260u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_9_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_9_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_9_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_9_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_9_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_9_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_9_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_9_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_9_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_9_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_9_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_10 Register EIPI_SAM_MAC_SA_MATCH_LO_10 - EIPI SAM MAC SA MATCH LO 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_10 0x280
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_10 0x4280u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_10_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_10_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_10_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_10 Register EIPI_SAM_MAC_SA_MATCH_HI_10 - EIPI SAM MAC SA MATCH HI 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_10 0x284
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_10 0x4284u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_10_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_10_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_10_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_10_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_10_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_10 Register EIPI_SAM_MAC_DA_MATCH_LO_10 - EIPI SAM MAC DA MATCH LO 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_10 0x288
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_10 0x4288u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_10_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_10_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_10_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_10 Register EIPI_SAM_MAC_DA_MATCH_HI_10 - EIPI SAM MAC DA MATCH HI 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_10 0x28C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_10 0x428Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_10_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_10_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_10 Register EIPI_SAM_MAC_MISC_MATCH_10 - EIPI SAM MAC MISC MATCH 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_10 0x290
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_10 0x4290u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_10_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_10_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_10 Register EIPI_SAM_SCI_MATCH_LO_10 - EIPI SAM SCI MATCH LO 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_10 0x294
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_10 0x4294u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_10_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_10_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_10_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_10 Register EIPI_SAM_SCI_MATCH_HI_10 - EIPI SAM SCI MATCH HI 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_10 0x298
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_10 0x4298u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_10_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_10_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_10_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_10 Register EIPI_SAM_MASK_10 - EIPI SAM MASK 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_10 0x29C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_10 0x429Cu

//! Register Reset Value
#define EIPI_SAM_MASK_10_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_10_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_10_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_10_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_10_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_10_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_10_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_10_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_10_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_10_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_10_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_10_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_10_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_10_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_10 Register EIPI_SAM_EXT_MATCH_10 - EIPI SAM EXT MATCH 10
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_10 0x2A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_10 0x42A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_10_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_10_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_10_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_10_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_10_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_10_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_10_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_10_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_10_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_10_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_10_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_11 Register EIPI_SAM_MAC_SA_MATCH_LO_11 - EIPI SAM MAC SA MATCH LO 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_11 0x2C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_11 0x42C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_11_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_11_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_11_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_11 Register EIPI_SAM_MAC_SA_MATCH_HI_11 - EIPI SAM MAC SA MATCH HI 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_11 0x2C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_11 0x42C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_11_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_11_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_11_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_11_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_11_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_11 Register EIPI_SAM_MAC_DA_MATCH_LO_11 - EIPI SAM MAC DA MATCH LO 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_11 0x2C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_11 0x42C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_11_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_11_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_11_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_11 Register EIPI_SAM_MAC_DA_MATCH_HI_11 - EIPI SAM MAC DA MATCH HI 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_11 0x2CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_11 0x42CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_11_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_11_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_11 Register EIPI_SAM_MAC_MISC_MATCH_11 - EIPI SAM MAC MISC MATCH 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_11 0x2D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_11 0x42D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_11_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_11_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_11 Register EIPI_SAM_SCI_MATCH_LO_11 - EIPI SAM SCI MATCH LO 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_11 0x2D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_11 0x42D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_11_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_11_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_11_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_11 Register EIPI_SAM_SCI_MATCH_HI_11 - EIPI SAM SCI MATCH HI 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_11 0x2D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_11 0x42D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_11_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_11_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_11_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_11 Register EIPI_SAM_MASK_11 - EIPI SAM MASK 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_11 0x2DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_11 0x42DCu

//! Register Reset Value
#define EIPI_SAM_MASK_11_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_11_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_11_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_11_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_11_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_11_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_11_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_11_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_11_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_11_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_11_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_11_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_11_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_11_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_11 Register EIPI_SAM_EXT_MATCH_11 - EIPI SAM EXT MATCH 11
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_11 0x2E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_11 0x42E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_11_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_11_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_11_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_11_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_11_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_11_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_11_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_11_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_11_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_11_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_11_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_12 Register EIPI_SAM_MAC_SA_MATCH_LO_12 - EIPI SAM MAC SA MATCH LO 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_12 0x300
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_12 0x4300u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_12_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_12_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_12_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_12 Register EIPI_SAM_MAC_SA_MATCH_HI_12 - EIPI SAM MAC SA MATCH HI 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_12 0x304
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_12 0x4304u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_12_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_12_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_12_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_12_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_12_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_12 Register EIPI_SAM_MAC_DA_MATCH_LO_12 - EIPI SAM MAC DA MATCH LO 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_12 0x308
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_12 0x4308u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_12_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_12_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_12_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_12 Register EIPI_SAM_MAC_DA_MATCH_HI_12 - EIPI SAM MAC DA MATCH HI 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_12 0x30C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_12 0x430Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_12_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_12_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_12 Register EIPI_SAM_MAC_MISC_MATCH_12 - EIPI SAM MAC MISC MATCH 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_12 0x310
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_12 0x4310u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_12_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_12_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_12 Register EIPI_SAM_SCI_MATCH_LO_12 - EIPI SAM SCI MATCH LO 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_12 0x314
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_12 0x4314u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_12_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_12_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_12_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_12 Register EIPI_SAM_SCI_MATCH_HI_12 - EIPI SAM SCI MATCH HI 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_12 0x318
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_12 0x4318u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_12_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_12_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_12_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_12 Register EIPI_SAM_MASK_12 - EIPI SAM MASK 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_12 0x31C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_12 0x431Cu

//! Register Reset Value
#define EIPI_SAM_MASK_12_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_12_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_12_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_12_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_12_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_12_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_12_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_12_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_12_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_12_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_12_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_12_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_12_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_12_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_12 Register EIPI_SAM_EXT_MATCH_12 - EIPI SAM EXT MATCH 12
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_12 0x320
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_12 0x4320u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_12_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_12_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_12_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_12_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_12_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_12_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_12_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_12_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_12_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_12_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_12_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_13 Register EIPI_SAM_MAC_SA_MATCH_LO_13 - EIPI SAM MAC SA MATCH LO 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_13 0x340
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_13 0x4340u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_13_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_13_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_13_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_13 Register EIPI_SAM_MAC_SA_MATCH_HI_13 - EIPI SAM MAC SA MATCH HI 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_13 0x344
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_13 0x4344u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_13_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_13_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_13_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_13_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_13_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_13 Register EIPI_SAM_MAC_DA_MATCH_LO_13 - EIPI SAM MAC DA MATCH LO 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_13 0x348
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_13 0x4348u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_13_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_13_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_13_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_13 Register EIPI_SAM_MAC_DA_MATCH_HI_13 - EIPI SAM MAC DA MATCH HI 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_13 0x34C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_13 0x434Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_13_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_13_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_13 Register EIPI_SAM_MAC_MISC_MATCH_13 - EIPI SAM MAC MISC MATCH 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_13 0x350
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_13 0x4350u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_13_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_13_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_13 Register EIPI_SAM_SCI_MATCH_LO_13 - EIPI SAM SCI MATCH LO 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_13 0x354
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_13 0x4354u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_13_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_13_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_13_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_13 Register EIPI_SAM_SCI_MATCH_HI_13 - EIPI SAM SCI MATCH HI 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_13 0x358
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_13 0x4358u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_13_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_13_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_13_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_13 Register EIPI_SAM_MASK_13 - EIPI SAM MASK 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_13 0x35C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_13 0x435Cu

//! Register Reset Value
#define EIPI_SAM_MASK_13_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_13_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_13_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_13_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_13_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_13_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_13_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_13_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_13_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_13_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_13_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_13_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_13_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_13_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_13 Register EIPI_SAM_EXT_MATCH_13 - EIPI SAM EXT MATCH 13
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_13 0x360
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_13 0x4360u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_13_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_13_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_13_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_13_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_13_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_13_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_13_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_13_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_13_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_13_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_13_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_14 Register EIPI_SAM_MAC_SA_MATCH_LO_14 - EIPI SAM MAC SA MATCH LO 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_14 0x380
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_14 0x4380u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_14_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_14_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_14_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_14 Register EIPI_SAM_MAC_SA_MATCH_HI_14 - EIPI SAM MAC SA MATCH HI 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_14 0x384
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_14 0x4384u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_14_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_14_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_14_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_14_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_14_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_14 Register EIPI_SAM_MAC_DA_MATCH_LO_14 - EIPI SAM MAC DA MATCH LO 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_14 0x388
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_14 0x4388u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_14_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_14_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_14_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_14 Register EIPI_SAM_MAC_DA_MATCH_HI_14 - EIPI SAM MAC DA MATCH HI 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_14 0x38C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_14 0x438Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_14_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_14_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_14 Register EIPI_SAM_MAC_MISC_MATCH_14 - EIPI SAM MAC MISC MATCH 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_14 0x390
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_14 0x4390u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_14_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_14_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_14 Register EIPI_SAM_SCI_MATCH_LO_14 - EIPI SAM SCI MATCH LO 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_14 0x394
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_14 0x4394u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_14_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_14_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_14_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_14 Register EIPI_SAM_SCI_MATCH_HI_14 - EIPI SAM SCI MATCH HI 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_14 0x398
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_14 0x4398u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_14_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_14_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_14_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_14 Register EIPI_SAM_MASK_14 - EIPI SAM MASK 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_14 0x39C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_14 0x439Cu

//! Register Reset Value
#define EIPI_SAM_MASK_14_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_14_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_14_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_14_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_14_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_14_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_14_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_14_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_14_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_14_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_14_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_14_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_14_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_14_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_14 Register EIPI_SAM_EXT_MATCH_14 - EIPI SAM EXT MATCH 14
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_14 0x3A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_14 0x43A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_14_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_14_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_14_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_14_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_14_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_14_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_14_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_14_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_14_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_14_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_14_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_15 Register EIPI_SAM_MAC_SA_MATCH_LO_15 - EIPI SAM MAC SA MATCH LO 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_15 0x3C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_15 0x43C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_15_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_15_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_15_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_15 Register EIPI_SAM_MAC_SA_MATCH_HI_15 - EIPI SAM MAC SA MATCH HI 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_15 0x3C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_15 0x43C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_15_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_15_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_15_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_15_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_15_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_15 Register EIPI_SAM_MAC_DA_MATCH_LO_15 - EIPI SAM MAC DA MATCH LO 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_15 0x3C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_15 0x43C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_15_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_15_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_15_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_15 Register EIPI_SAM_MAC_DA_MATCH_HI_15 - EIPI SAM MAC DA MATCH HI 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_15 0x3CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_15 0x43CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_15_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_15_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_15 Register EIPI_SAM_MAC_MISC_MATCH_15 - EIPI SAM MAC MISC MATCH 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_15 0x3D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_15 0x43D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_15_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_15_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_15 Register EIPI_SAM_SCI_MATCH_LO_15 - EIPI SAM SCI MATCH LO 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_15 0x3D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_15 0x43D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_15_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_15_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_15_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_15 Register EIPI_SAM_SCI_MATCH_HI_15 - EIPI SAM SCI MATCH HI 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_15 0x3D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_15 0x43D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_15_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_15_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_15_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_15 Register EIPI_SAM_MASK_15 - EIPI SAM MASK 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_15 0x3DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_15 0x43DCu

//! Register Reset Value
#define EIPI_SAM_MASK_15_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_15_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_15_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_15_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_15_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_15_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_15_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_15_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_15_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_15_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_15_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_15_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_15_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_15_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_15 Register EIPI_SAM_EXT_MATCH_15 - EIPI SAM EXT MATCH 15
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_15 0x3E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_15 0x43E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_15_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_15_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_15_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_15_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_15_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_15_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_15_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_15_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_15_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_15_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_15_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_16 Register EIPI_SAM_MAC_SA_MATCH_LO_16 - EIPI SAM MAC SA MATCH LO 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_16 0x400
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_16 0x4400u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_16_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_16_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_16_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_16 Register EIPI_SAM_MAC_SA_MATCH_HI_16 - EIPI SAM MAC SA MATCH HI 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_16 0x404
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_16 0x4404u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_16_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_16_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_16_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_16_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_16_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_16 Register EIPI_SAM_MAC_DA_MATCH_LO_16 - EIPI SAM MAC DA MATCH LO 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_16 0x408
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_16 0x4408u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_16_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_16_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_16_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_16 Register EIPI_SAM_MAC_DA_MATCH_HI_16 - EIPI SAM MAC DA MATCH HI 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_16 0x40C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_16 0x440Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_16_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_16_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_16 Register EIPI_SAM_MAC_MISC_MATCH_16 - EIPI SAM MAC MISC MATCH 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_16 0x410
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_16 0x4410u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_16_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_16_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_16 Register EIPI_SAM_SCI_MATCH_LO_16 - EIPI SAM SCI MATCH LO 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_16 0x414
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_16 0x4414u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_16_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_16_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_16_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_16 Register EIPI_SAM_SCI_MATCH_HI_16 - EIPI SAM SCI MATCH HI 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_16 0x418
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_16 0x4418u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_16_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_16_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_16_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_16 Register EIPI_SAM_MASK_16 - EIPI SAM MASK 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_16 0x41C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_16 0x441Cu

//! Register Reset Value
#define EIPI_SAM_MASK_16_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_16_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_16_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_16_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_16_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_16_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_16_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_16_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_16_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_16_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_16_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_16_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_16_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_16_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_16 Register EIPI_SAM_EXT_MATCH_16 - EIPI SAM EXT MATCH 16
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_16 0x420
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_16 0x4420u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_16_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_16_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_16_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_16_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_16_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_16_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_16_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_16_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_16_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_16_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_16_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_17 Register EIPI_SAM_MAC_SA_MATCH_LO_17 - EIPI SAM MAC SA MATCH LO 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_17 0x440
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_17 0x4440u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_17_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_17_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_17_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_17 Register EIPI_SAM_MAC_SA_MATCH_HI_17 - EIPI SAM MAC SA MATCH HI 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_17 0x444
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_17 0x4444u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_17_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_17_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_17_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_17_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_17_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_17 Register EIPI_SAM_MAC_DA_MATCH_LO_17 - EIPI SAM MAC DA MATCH LO 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_17 0x448
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_17 0x4448u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_17_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_17_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_17_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_17 Register EIPI_SAM_MAC_DA_MATCH_HI_17 - EIPI SAM MAC DA MATCH HI 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_17 0x44C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_17 0x444Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_17_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_17_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_17 Register EIPI_SAM_MAC_MISC_MATCH_17 - EIPI SAM MAC MISC MATCH 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_17 0x450
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_17 0x4450u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_17_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_17_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_17 Register EIPI_SAM_SCI_MATCH_LO_17 - EIPI SAM SCI MATCH LO 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_17 0x454
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_17 0x4454u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_17_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_17_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_17_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_17 Register EIPI_SAM_SCI_MATCH_HI_17 - EIPI SAM SCI MATCH HI 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_17 0x458
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_17 0x4458u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_17_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_17_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_17_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_17 Register EIPI_SAM_MASK_17 - EIPI SAM MASK 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_17 0x45C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_17 0x445Cu

//! Register Reset Value
#define EIPI_SAM_MASK_17_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_17_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_17_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_17_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_17_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_17_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_17_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_17_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_17_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_17_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_17_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_17_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_17_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_17_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_17 Register EIPI_SAM_EXT_MATCH_17 - EIPI SAM EXT MATCH 17
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_17 0x460
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_17 0x4460u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_17_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_17_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_17_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_17_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_17_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_17_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_17_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_17_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_17_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_17_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_17_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_18 Register EIPI_SAM_MAC_SA_MATCH_LO_18 - EIPI SAM MAC SA MATCH LO 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_18 0x480
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_18 0x4480u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_18_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_18_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_18_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_18 Register EIPI_SAM_MAC_SA_MATCH_HI_18 - EIPI SAM MAC SA MATCH HI 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_18 0x484
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_18 0x4484u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_18_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_18_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_18_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_18_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_18_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_18 Register EIPI_SAM_MAC_DA_MATCH_LO_18 - EIPI SAM MAC DA MATCH LO 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_18 0x488
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_18 0x4488u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_18_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_18_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_18_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_18 Register EIPI_SAM_MAC_DA_MATCH_HI_18 - EIPI SAM MAC DA MATCH HI 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_18 0x48C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_18 0x448Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_18_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_18_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_18 Register EIPI_SAM_MAC_MISC_MATCH_18 - EIPI SAM MAC MISC MATCH 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_18 0x490
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_18 0x4490u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_18_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_18_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_18 Register EIPI_SAM_SCI_MATCH_LO_18 - EIPI SAM SCI MATCH LO 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_18 0x494
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_18 0x4494u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_18_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_18_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_18_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_18 Register EIPI_SAM_SCI_MATCH_HI_18 - EIPI SAM SCI MATCH HI 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_18 0x498
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_18 0x4498u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_18_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_18_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_18_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_18 Register EIPI_SAM_MASK_18 - EIPI SAM MASK 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_18 0x49C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_18 0x449Cu

//! Register Reset Value
#define EIPI_SAM_MASK_18_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_18_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_18_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_18_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_18_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_18_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_18_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_18_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_18_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_18_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_18_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_18_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_18_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_18_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_18 Register EIPI_SAM_EXT_MATCH_18 - EIPI SAM EXT MATCH 18
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_18 0x4A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_18 0x44A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_18_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_18_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_18_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_18_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_18_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_18_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_18_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_18_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_18_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_18_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_18_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_19 Register EIPI_SAM_MAC_SA_MATCH_LO_19 - EIPI SAM MAC SA MATCH LO 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_19 0x4C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_19 0x44C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_19_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_19_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_19_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_19 Register EIPI_SAM_MAC_SA_MATCH_HI_19 - EIPI SAM MAC SA MATCH HI 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_19 0x4C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_19 0x44C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_19_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_19_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_19_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_19_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_19_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_19 Register EIPI_SAM_MAC_DA_MATCH_LO_19 - EIPI SAM MAC DA MATCH LO 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_19 0x4C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_19 0x44C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_19_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_19_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_19_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_19 Register EIPI_SAM_MAC_DA_MATCH_HI_19 - EIPI SAM MAC DA MATCH HI 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_19 0x4CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_19 0x44CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_19_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_19_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_19 Register EIPI_SAM_MAC_MISC_MATCH_19 - EIPI SAM MAC MISC MATCH 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_19 0x4D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_19 0x44D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_19_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_19_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_19 Register EIPI_SAM_SCI_MATCH_LO_19 - EIPI SAM SCI MATCH LO 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_19 0x4D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_19 0x44D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_19_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_19_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_19_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_19 Register EIPI_SAM_SCI_MATCH_HI_19 - EIPI SAM SCI MATCH HI 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_19 0x4D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_19 0x44D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_19_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_19_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_19_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_19 Register EIPI_SAM_MASK_19 - EIPI SAM MASK 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_19 0x4DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_19 0x44DCu

//! Register Reset Value
#define EIPI_SAM_MASK_19_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_19_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_19_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_19_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_19_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_19_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_19_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_19_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_19_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_19_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_19_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_19_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_19_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_19_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_19 Register EIPI_SAM_EXT_MATCH_19 - EIPI SAM EXT MATCH 19
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_19 0x4E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_19 0x44E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_19_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_19_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_19_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_19_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_19_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_19_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_19_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_19_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_19_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_19_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_19_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_20 Register EIPI_SAM_MAC_SA_MATCH_LO_20 - EIPI SAM MAC SA MATCH LO 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_20 0x500
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_20 0x4500u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_20_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_20_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_20_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_20 Register EIPI_SAM_MAC_SA_MATCH_HI_20 - EIPI SAM MAC SA MATCH HI 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_20 0x504
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_20 0x4504u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_20_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_20_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_20_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_20_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_20_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_20 Register EIPI_SAM_MAC_DA_MATCH_LO_20 - EIPI SAM MAC DA MATCH LO 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_20 0x508
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_20 0x4508u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_20_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_20_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_20_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_20 Register EIPI_SAM_MAC_DA_MATCH_HI_20 - EIPI SAM MAC DA MATCH HI 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_20 0x50C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_20 0x450Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_20_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_20_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_20 Register EIPI_SAM_MAC_MISC_MATCH_20 - EIPI SAM MAC MISC MATCH 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_20 0x510
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_20 0x4510u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_20_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_20_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_20 Register EIPI_SAM_SCI_MATCH_LO_20 - EIPI SAM SCI MATCH LO 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_20 0x514
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_20 0x4514u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_20_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_20_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_20_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_20 Register EIPI_SAM_SCI_MATCH_HI_20 - EIPI SAM SCI MATCH HI 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_20 0x518
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_20 0x4518u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_20_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_20_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_20_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_20 Register EIPI_SAM_MASK_20 - EIPI SAM MASK 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_20 0x51C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_20 0x451Cu

//! Register Reset Value
#define EIPI_SAM_MASK_20_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_20_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_20_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_20_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_20_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_20_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_20_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_20_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_20_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_20_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_20_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_20_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_20_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_20_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_20 Register EIPI_SAM_EXT_MATCH_20 - EIPI SAM EXT MATCH 20
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_20 0x520
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_20 0x4520u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_20_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_20_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_20_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_20_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_20_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_20_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_20_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_20_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_20_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_20_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_20_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_21 Register EIPI_SAM_MAC_SA_MATCH_LO_21 - EIPI SAM MAC SA MATCH LO 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_21 0x540
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_21 0x4540u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_21_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_21_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_21_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_21 Register EIPI_SAM_MAC_SA_MATCH_HI_21 - EIPI SAM MAC SA MATCH HI 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_21 0x544
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_21 0x4544u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_21_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_21_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_21_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_21_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_21_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_21 Register EIPI_SAM_MAC_DA_MATCH_LO_21 - EIPI SAM MAC DA MATCH LO 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_21 0x548
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_21 0x4548u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_21_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_21_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_21_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_21 Register EIPI_SAM_MAC_DA_MATCH_HI_21 - EIPI SAM MAC DA MATCH HI 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_21 0x54C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_21 0x454Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_21_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_21_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_21 Register EIPI_SAM_MAC_MISC_MATCH_21 - EIPI SAM MAC MISC MATCH 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_21 0x550
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_21 0x4550u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_21_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_21_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_21 Register EIPI_SAM_SCI_MATCH_LO_21 - EIPI SAM SCI MATCH LO 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_21 0x554
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_21 0x4554u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_21_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_21_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_21_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_21 Register EIPI_SAM_SCI_MATCH_HI_21 - EIPI SAM SCI MATCH HI 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_21 0x558
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_21 0x4558u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_21_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_21_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_21_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_21 Register EIPI_SAM_MASK_21 - EIPI SAM MASK 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_21 0x55C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_21 0x455Cu

//! Register Reset Value
#define EIPI_SAM_MASK_21_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_21_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_21_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_21_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_21_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_21_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_21_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_21_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_21_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_21_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_21_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_21_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_21_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_21_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_21 Register EIPI_SAM_EXT_MATCH_21 - EIPI SAM EXT MATCH 21
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_21 0x560
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_21 0x4560u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_21_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_21_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_21_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_21_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_21_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_21_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_21_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_21_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_21_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_21_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_21_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_22 Register EIPI_SAM_MAC_SA_MATCH_LO_22 - EIPI SAM MAC SA MATCH LO 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_22 0x580
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_22 0x4580u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_22_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_22_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_22_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_22 Register EIPI_SAM_MAC_SA_MATCH_HI_22 - EIPI SAM MAC SA MATCH HI 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_22 0x584
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_22 0x4584u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_22_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_22_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_22_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_22_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_22_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_22 Register EIPI_SAM_MAC_DA_MATCH_LO_22 - EIPI SAM MAC DA MATCH LO 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_22 0x588
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_22 0x4588u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_22_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_22_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_22_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_22 Register EIPI_SAM_MAC_DA_MATCH_HI_22 - EIPI SAM MAC DA MATCH HI 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_22 0x58C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_22 0x458Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_22_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_22_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_22 Register EIPI_SAM_MAC_MISC_MATCH_22 - EIPI SAM MAC MISC MATCH 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_22 0x590
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_22 0x4590u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_22_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_22_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_22 Register EIPI_SAM_SCI_MATCH_LO_22 - EIPI SAM SCI MATCH LO 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_22 0x594
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_22 0x4594u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_22_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_22_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_22_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_22 Register EIPI_SAM_SCI_MATCH_HI_22 - EIPI SAM SCI MATCH HI 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_22 0x598
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_22 0x4598u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_22_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_22_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_22_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_22 Register EIPI_SAM_MASK_22 - EIPI SAM MASK 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_22 0x59C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_22 0x459Cu

//! Register Reset Value
#define EIPI_SAM_MASK_22_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_22_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_22_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_22_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_22_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_22_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_22_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_22_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_22_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_22_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_22_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_22_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_22_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_22_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_22 Register EIPI_SAM_EXT_MATCH_22 - EIPI SAM EXT MATCH 22
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_22 0x5A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_22 0x45A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_22_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_22_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_22_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_22_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_22_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_22_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_22_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_22_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_22_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_22_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_22_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_23 Register EIPI_SAM_MAC_SA_MATCH_LO_23 - EIPI SAM MAC SA MATCH LO 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_23 0x5C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_23 0x45C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_23_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_23_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_23_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_23 Register EIPI_SAM_MAC_SA_MATCH_HI_23 - EIPI SAM MAC SA MATCH HI 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_23 0x5C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_23 0x45C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_23_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_23_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_23_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_23_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_23_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_23 Register EIPI_SAM_MAC_DA_MATCH_LO_23 - EIPI SAM MAC DA MATCH LO 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_23 0x5C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_23 0x45C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_23_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_23_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_23_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_23 Register EIPI_SAM_MAC_DA_MATCH_HI_23 - EIPI SAM MAC DA MATCH HI 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_23 0x5CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_23 0x45CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_23_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_23_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_23 Register EIPI_SAM_MAC_MISC_MATCH_23 - EIPI SAM MAC MISC MATCH 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_23 0x5D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_23 0x45D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_23_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_23_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_23 Register EIPI_SAM_SCI_MATCH_LO_23 - EIPI SAM SCI MATCH LO 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_23 0x5D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_23 0x45D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_23_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_23_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_23_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_23 Register EIPI_SAM_SCI_MATCH_HI_23 - EIPI SAM SCI MATCH HI 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_23 0x5D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_23 0x45D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_23_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_23_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_23_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_23 Register EIPI_SAM_MASK_23 - EIPI SAM MASK 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_23 0x5DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_23 0x45DCu

//! Register Reset Value
#define EIPI_SAM_MASK_23_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_23_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_23_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_23_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_23_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_23_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_23_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_23_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_23_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_23_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_23_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_23_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_23_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_23_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_23 Register EIPI_SAM_EXT_MATCH_23 - EIPI SAM EXT MATCH 23
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_23 0x5E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_23 0x45E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_23_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_23_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_23_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_23_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_23_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_23_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_23_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_23_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_23_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_23_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_23_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_24 Register EIPI_SAM_MAC_SA_MATCH_LO_24 - EIPI SAM MAC SA MATCH LO 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_24 0x600
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_24 0x4600u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_24_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_24_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_24_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_24 Register EIPI_SAM_MAC_SA_MATCH_HI_24 - EIPI SAM MAC SA MATCH HI 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_24 0x604
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_24 0x4604u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_24_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_24_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_24_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_24_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_24_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_24 Register EIPI_SAM_MAC_DA_MATCH_LO_24 - EIPI SAM MAC DA MATCH LO 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_24 0x608
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_24 0x4608u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_24_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_24_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_24_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_24 Register EIPI_SAM_MAC_DA_MATCH_HI_24 - EIPI SAM MAC DA MATCH HI 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_24 0x60C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_24 0x460Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_24_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_24_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_24 Register EIPI_SAM_MAC_MISC_MATCH_24 - EIPI SAM MAC MISC MATCH 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_24 0x610
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_24 0x4610u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_24_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_24_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_24 Register EIPI_SAM_SCI_MATCH_LO_24 - EIPI SAM SCI MATCH LO 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_24 0x614
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_24 0x4614u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_24_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_24_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_24_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_24 Register EIPI_SAM_SCI_MATCH_HI_24 - EIPI SAM SCI MATCH HI 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_24 0x618
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_24 0x4618u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_24_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_24_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_24_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_24 Register EIPI_SAM_MASK_24 - EIPI SAM MASK 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_24 0x61C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_24 0x461Cu

//! Register Reset Value
#define EIPI_SAM_MASK_24_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_24_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_24_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_24_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_24_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_24_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_24_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_24_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_24_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_24_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_24_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_24_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_24_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_24_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_24 Register EIPI_SAM_EXT_MATCH_24 - EIPI SAM EXT MATCH 24
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_24 0x620
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_24 0x4620u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_24_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_24_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_24_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_24_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_24_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_24_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_24_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_24_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_24_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_24_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_24_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_25 Register EIPI_SAM_MAC_SA_MATCH_LO_25 - EIPI SAM MAC SA MATCH LO 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_25 0x640
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_25 0x4640u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_25_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_25_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_25_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_25 Register EIPI_SAM_MAC_SA_MATCH_HI_25 - EIPI SAM MAC SA MATCH HI 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_25 0x644
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_25 0x4644u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_25_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_25_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_25_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_25_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_25_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_25 Register EIPI_SAM_MAC_DA_MATCH_LO_25 - EIPI SAM MAC DA MATCH LO 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_25 0x648
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_25 0x4648u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_25_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_25_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_25_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_25 Register EIPI_SAM_MAC_DA_MATCH_HI_25 - EIPI SAM MAC DA MATCH HI 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_25 0x64C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_25 0x464Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_25_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_25_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_25 Register EIPI_SAM_MAC_MISC_MATCH_25 - EIPI SAM MAC MISC MATCH 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_25 0x650
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_25 0x4650u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_25_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_25_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_25 Register EIPI_SAM_SCI_MATCH_LO_25 - EIPI SAM SCI MATCH LO 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_25 0x654
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_25 0x4654u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_25_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_25_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_25_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_25 Register EIPI_SAM_SCI_MATCH_HI_25 - EIPI SAM SCI MATCH HI 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_25 0x658
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_25 0x4658u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_25_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_25_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_25_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_25 Register EIPI_SAM_MASK_25 - EIPI SAM MASK 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_25 0x65C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_25 0x465Cu

//! Register Reset Value
#define EIPI_SAM_MASK_25_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_25_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_25_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_25_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_25_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_25_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_25_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_25_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_25_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_25_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_25_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_25_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_25_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_25_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_25 Register EIPI_SAM_EXT_MATCH_25 - EIPI SAM EXT MATCH 25
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_25 0x660
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_25 0x4660u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_25_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_25_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_25_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_25_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_25_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_25_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_25_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_25_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_25_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_25_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_25_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_26 Register EIPI_SAM_MAC_SA_MATCH_LO_26 - EIPI SAM MAC SA MATCH LO 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_26 0x680
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_26 0x4680u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_26_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_26_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_26_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_26 Register EIPI_SAM_MAC_SA_MATCH_HI_26 - EIPI SAM MAC SA MATCH HI 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_26 0x684
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_26 0x4684u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_26_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_26_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_26_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_26_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_26_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_26 Register EIPI_SAM_MAC_DA_MATCH_LO_26 - EIPI SAM MAC DA MATCH LO 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_26 0x688
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_26 0x4688u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_26_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_26_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_26_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_26 Register EIPI_SAM_MAC_DA_MATCH_HI_26 - EIPI SAM MAC DA MATCH HI 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_26 0x68C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_26 0x468Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_26_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_26_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_26 Register EIPI_SAM_MAC_MISC_MATCH_26 - EIPI SAM MAC MISC MATCH 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_26 0x690
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_26 0x4690u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_26_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_26_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_26 Register EIPI_SAM_SCI_MATCH_LO_26 - EIPI SAM SCI MATCH LO 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_26 0x694
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_26 0x4694u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_26_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_26_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_26_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_26 Register EIPI_SAM_SCI_MATCH_HI_26 - EIPI SAM SCI MATCH HI 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_26 0x698
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_26 0x4698u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_26_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_26_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_26_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_26 Register EIPI_SAM_MASK_26 - EIPI SAM MASK 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_26 0x69C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_26 0x469Cu

//! Register Reset Value
#define EIPI_SAM_MASK_26_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_26_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_26_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_26_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_26_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_26_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_26_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_26_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_26_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_26_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_26_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_26_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_26_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_26_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_26 Register EIPI_SAM_EXT_MATCH_26 - EIPI SAM EXT MATCH 26
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_26 0x6A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_26 0x46A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_26_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_26_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_26_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_26_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_26_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_26_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_26_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_26_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_26_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_26_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_26_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_27 Register EIPI_SAM_MAC_SA_MATCH_LO_27 - EIPI SAM MAC SA MATCH LO 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_27 0x6C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_27 0x46C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_27_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_27_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_27_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_27 Register EIPI_SAM_MAC_SA_MATCH_HI_27 - EIPI SAM MAC SA MATCH HI 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_27 0x6C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_27 0x46C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_27_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_27_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_27_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_27_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_27_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_27 Register EIPI_SAM_MAC_DA_MATCH_LO_27 - EIPI SAM MAC DA MATCH LO 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_27 0x6C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_27 0x46C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_27_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_27_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_27_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_27 Register EIPI_SAM_MAC_DA_MATCH_HI_27 - EIPI SAM MAC DA MATCH HI 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_27 0x6CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_27 0x46CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_27_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_27_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_27 Register EIPI_SAM_MAC_MISC_MATCH_27 - EIPI SAM MAC MISC MATCH 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_27 0x6D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_27 0x46D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_27_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_27_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_27 Register EIPI_SAM_SCI_MATCH_LO_27 - EIPI SAM SCI MATCH LO 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_27 0x6D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_27 0x46D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_27_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_27_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_27_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_27 Register EIPI_SAM_SCI_MATCH_HI_27 - EIPI SAM SCI MATCH HI 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_27 0x6D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_27 0x46D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_27_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_27_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_27_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_27 Register EIPI_SAM_MASK_27 - EIPI SAM MASK 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_27 0x6DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_27 0x46DCu

//! Register Reset Value
#define EIPI_SAM_MASK_27_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_27_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_27_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_27_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_27_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_27_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_27_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_27_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_27_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_27_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_27_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_27_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_27_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_27_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_27 Register EIPI_SAM_EXT_MATCH_27 - EIPI SAM EXT MATCH 27
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_27 0x6E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_27 0x46E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_27_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_27_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_27_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_27_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_27_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_27_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_27_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_27_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_27_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_27_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_27_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_28 Register EIPI_SAM_MAC_SA_MATCH_LO_28 - EIPI SAM MAC SA MATCH LO 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_28 0x700
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_28 0x4700u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_28_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_28_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_28_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_28 Register EIPI_SAM_MAC_SA_MATCH_HI_28 - EIPI SAM MAC SA MATCH HI 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_28 0x704
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_28 0x4704u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_28_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_28_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_28_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_28_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_28_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_28 Register EIPI_SAM_MAC_DA_MATCH_LO_28 - EIPI SAM MAC DA MATCH LO 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_28 0x708
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_28 0x4708u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_28_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_28_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_28_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_28 Register EIPI_SAM_MAC_DA_MATCH_HI_28 - EIPI SAM MAC DA MATCH HI 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_28 0x70C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_28 0x470Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_28_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_28_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_28 Register EIPI_SAM_MAC_MISC_MATCH_28 - EIPI SAM MAC MISC MATCH 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_28 0x710
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_28 0x4710u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_28_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_28_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_28 Register EIPI_SAM_SCI_MATCH_LO_28 - EIPI SAM SCI MATCH LO 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_28 0x714
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_28 0x4714u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_28_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_28_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_28_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_28 Register EIPI_SAM_SCI_MATCH_HI_28 - EIPI SAM SCI MATCH HI 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_28 0x718
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_28 0x4718u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_28_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_28_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_28_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_28 Register EIPI_SAM_MASK_28 - EIPI SAM MASK 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_28 0x71C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_28 0x471Cu

//! Register Reset Value
#define EIPI_SAM_MASK_28_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_28_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_28_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_28_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_28_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_28_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_28_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_28_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_28_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_28_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_28_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_28_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_28_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_28_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_28 Register EIPI_SAM_EXT_MATCH_28 - EIPI SAM EXT MATCH 28
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_28 0x720
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_28 0x4720u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_28_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_28_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_28_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_28_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_28_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_28_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_28_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_28_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_28_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_28_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_28_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_29 Register EIPI_SAM_MAC_SA_MATCH_LO_29 - EIPI SAM MAC SA MATCH LO 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_29 0x740
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_29 0x4740u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_29_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_29_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_29_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_29 Register EIPI_SAM_MAC_SA_MATCH_HI_29 - EIPI SAM MAC SA MATCH HI 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_29 0x744
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_29 0x4744u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_29_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_29_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_29_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_29_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_29_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_29 Register EIPI_SAM_MAC_DA_MATCH_LO_29 - EIPI SAM MAC DA MATCH LO 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_29 0x748
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_29 0x4748u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_29_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_29_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_29_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_29 Register EIPI_SAM_MAC_DA_MATCH_HI_29 - EIPI SAM MAC DA MATCH HI 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_29 0x74C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_29 0x474Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_29_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_29_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_29 Register EIPI_SAM_MAC_MISC_MATCH_29 - EIPI SAM MAC MISC MATCH 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_29 0x750
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_29 0x4750u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_29_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_29_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_29 Register EIPI_SAM_SCI_MATCH_LO_29 - EIPI SAM SCI MATCH LO 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_29 0x754
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_29 0x4754u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_29_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_29_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_29_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_29 Register EIPI_SAM_SCI_MATCH_HI_29 - EIPI SAM SCI MATCH HI 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_29 0x758
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_29 0x4758u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_29_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_29_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_29_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_29 Register EIPI_SAM_MASK_29 - EIPI SAM MASK 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_29 0x75C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_29 0x475Cu

//! Register Reset Value
#define EIPI_SAM_MASK_29_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_29_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_29_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_29_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_29_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_29_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_29_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_29_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_29_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_29_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_29_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_29_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_29_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_29_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_29 Register EIPI_SAM_EXT_MATCH_29 - EIPI SAM EXT MATCH 29
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_29 0x760
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_29 0x4760u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_29_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_29_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_29_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_29_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_29_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_29_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_29_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_29_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_29_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_29_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_29_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_30 Register EIPI_SAM_MAC_SA_MATCH_LO_30 - EIPI SAM MAC SA MATCH LO 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_30 0x780
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_30 0x4780u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_30_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_30_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_30_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_30 Register EIPI_SAM_MAC_SA_MATCH_HI_30 - EIPI SAM MAC SA MATCH HI 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_30 0x784
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_30 0x4784u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_30_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_30_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_30_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_30_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_30_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_30 Register EIPI_SAM_MAC_DA_MATCH_LO_30 - EIPI SAM MAC DA MATCH LO 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_30 0x788
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_30 0x4788u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_30_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_30_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_30_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_30 Register EIPI_SAM_MAC_DA_MATCH_HI_30 - EIPI SAM MAC DA MATCH HI 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_30 0x78C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_30 0x478Cu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_30_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_30_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_30 Register EIPI_SAM_MAC_MISC_MATCH_30 - EIPI SAM MAC MISC MATCH 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_30 0x790
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_30 0x4790u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_30_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_30_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_30 Register EIPI_SAM_SCI_MATCH_LO_30 - EIPI SAM SCI MATCH LO 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_30 0x794
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_30 0x4794u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_30_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_30_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_30_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_30 Register EIPI_SAM_SCI_MATCH_HI_30 - EIPI SAM SCI MATCH HI 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_30 0x798
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_30 0x4798u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_30_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_30_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_30_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_30 Register EIPI_SAM_MASK_30 - EIPI SAM MASK 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_30 0x79C
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_30 0x479Cu

//! Register Reset Value
#define EIPI_SAM_MASK_30_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_30_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_30_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_30_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_30_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_30_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_30_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_30_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_30_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_30_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_30_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_30_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_30_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_30_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_30 Register EIPI_SAM_EXT_MATCH_30 - EIPI SAM EXT MATCH 30
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_30 0x7A0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_30 0x47A0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_30_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_30_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_30_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_30_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_30_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_30_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_30_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_30_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_30_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_30_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_30_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_LO_31 Register EIPI_SAM_MAC_SA_MATCH_LO_31 - EIPI SAM MAC SA MATCH LO 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_LO_31 0x7C0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_LO_31 0x47C0u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_LO_31_RST 0x00000000u

//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_31_MAC_SA_MATCH_47_16_POS 0
//! Field MAC_SA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_LO_31_MAC_SA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_SA_MATCH_HI_31 Register EIPI_SAM_MAC_SA_MATCH_HI_31 - EIPI SAM MAC SA MATCH HI 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_SA_MATCH_HI_31 0x7C4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_SA_MATCH_HI_31 0x47C4u

//! Register Reset Value
#define EIPI_SAM_MAC_SA_MATCH_HI_31_RST 0x00000000u

//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_31_MAC_SA_MATCH_15_0_POS 0
//! Field MAC_SA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_31_MAC_SA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_31_ETHER_TYPE_POS 16
//! Field ETHER_TYPE - No Content.
#define EIPI_SAM_MAC_SA_MATCH_HI_31_ETHER_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_LO_31 Register EIPI_SAM_MAC_DA_MATCH_LO_31 - EIPI SAM MAC DA MATCH LO 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_LO_31 0x7C8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_LO_31 0x47C8u

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_LO_31_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_31_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_LO_31_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MAC_DA_MATCH_HI_31 Register EIPI_SAM_MAC_DA_MATCH_HI_31 - EIPI SAM MAC DA MATCH HI 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_DA_MATCH_HI_31 0x7CC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_DA_MATCH_HI_31 0x47CCu

//! Register Reset Value
#define EIPI_SAM_MAC_DA_MATCH_HI_31_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_VLAN_ID_POS 16
//! Field VLAN_ID - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_VLAN_ID_MASK 0xFFF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_DA_MATCH_HI_31_RESERVED_0_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_SAM_MAC_MISC_MATCH_31 Register EIPI_SAM_MAC_MISC_MATCH_31 - EIPI SAM MAC MISC MATCH 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MAC_MISC_MATCH_31 0x7D0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MAC_MISC_MATCH_31 0x47D0u

//! Register Reset Value
#define EIPI_SAM_MAC_MISC_MATCH_31_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_QTAG_VALID_MASK 0x8u

//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_VLAN_UP_POS 4
//! Field VLAN_UP - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_VLAN_UP_MASK 0x70u

//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_CONTROL_PACKET_POS 7
//! Field CONTROL_PACKET - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_CONTROL_PACKET_MASK 0x80u

//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_UNTAGGED_POS 8
//! Field UNTAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_UNTAGGED_MASK 0x100u

//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_TAGGED_POS 9
//! Field TAGGED - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_TAGGED_MASK 0x200u

//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_BAD_TAG_POS 10
//! Field BAD_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_BAD_TAG_MASK 0x400u

//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_KAY_TAG_POS 11
//! Field KAY_TAG - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_KAY_TAG_MASK 0x800u

//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_SOURCE_PORT_POS 12
//! Field SOURCE_PORT - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_SOURCE_PORT_MASK 0x3000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_RESERVED_0_POS 14
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_RESERVED_0_MASK 0xC000u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_MATCH_PRIORITY_POS 16
//! Field MATCH_PRIORITY - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_MATCH_PRIORITY_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_RESERVED_1_MASK 0xF00000u

//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_MACSEC_TCI_AN_POS 24
//! Field MACSEC_TCI_AN - No Content.
#define EIPI_SAM_MAC_MISC_MATCH_31_MACSEC_TCI_AN_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_LO_31 Register EIPI_SAM_SCI_MATCH_LO_31 - EIPI SAM SCI MATCH LO 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_LO_31 0x7D4
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_LO_31 0x47D4u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_LO_31_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_31_MACSEC_SCI_MATCH_63_32_POS 0
//! Field MACSEC_SCI_MATCH_63_32 - No Content.
#define EIPI_SAM_SCI_MATCH_LO_31_MACSEC_SCI_MATCH_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_SCI_MATCH_HI_31 Register EIPI_SAM_SCI_MATCH_HI_31 - EIPI SAM SCI MATCH HI 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_SCI_MATCH_HI_31 0x7D8
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_SCI_MATCH_HI_31 0x47D8u

//! Register Reset Value
#define EIPI_SAM_SCI_MATCH_HI_31_RST 0x00000000u

//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_31_MACSEC_SCI_MATCH_31_0_POS 0
//! Field MACSEC_SCI_MATCH_31_0 - No Content.
#define EIPI_SAM_SCI_MATCH_HI_31_MACSEC_SCI_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_SAM_MASK_31 Register EIPI_SAM_MASK_31 - EIPI SAM MASK 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_MASK_31 0x7DC
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_MASK_31 0x47DCu

//! Register Reset Value
#define EIPI_SAM_MASK_31_RST 0x00000000u

//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_0_MASK_POS 0
//! Field MAC_SA_0_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_0_MASK_MASK 0x1u

//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_1_MASK_POS 1
//! Field MAC_SA_1_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_1_MASK_MASK 0x2u

//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_2_MASK_POS 2
//! Field MAC_SA_2_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_2_MASK_MASK 0x4u

//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_3_MASK_POS 3
//! Field MAC_SA_3_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_3_MASK_MASK 0x8u

//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_4_MASK_POS 4
//! Field MAC_SA_4_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_4_MASK_MASK 0x10u

//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_5_MASK_POS 5
//! Field MAC_SA_5_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_SA_5_MASK_MASK 0x20u

//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_0_MASK_POS 6
//! Field MAC_DA_0_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_0_MASK_MASK 0x40u

//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_1_MASK_POS 7
//! Field MAC_DA_1_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_1_MASK_MASK 0x80u

//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_2_MASK_POS 8
//! Field MAC_DA_2_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_2_MASK_MASK 0x100u

//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_3_MASK_POS 9
//! Field MAC_DA_3_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_3_MASK_MASK 0x200u

//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_4_MASK_POS 10
//! Field MAC_DA_4_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_4_MASK_MASK 0x400u

//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_5_MASK_POS 11
//! Field MAC_DA_5_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_DA_5_MASK_MASK 0x800u

//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_ETYPE_MASK_POS 12
//! Field MAC_ETYPE_MASK - No Content.
#define EIPI_SAM_MASK_31_MAC_ETYPE_MASK_MASK 0x1000u

//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_VLD_MASK_POS 13
//! Field VLAN_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_VLD_MASK_MASK 0x2000u

//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_31_QINQ_FOUND_MASK_POS 14
//! Field QINQ_FOUND_MASK - No Content.
#define EIPI_SAM_MASK_31_QINQ_FOUND_MASK_MASK 0x4000u

//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_STAG_VLD_MASK_POS 15
//! Field STAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_STAG_VLD_MASK_MASK 0x8000u

//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_QTAG_VLD_MASK_POS 16
//! Field QTAG_VLD_MASK - No Content.
#define EIPI_SAM_MASK_31_QTAG_VLD_MASK_MASK 0x10000u

//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_UP_MASK_POS 17
//! Field VLAN_UP_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_UP_MASK_MASK 0x20000u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_ID_MASK_POS 18
//! Field VLAN_ID_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_ID_MASK_MASK 0x40000u

//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_31_SOURCE_PORT_MASK_POS 19
//! Field SOURCE_PORT_MASK - No Content.
#define EIPI_SAM_MASK_31_SOURCE_PORT_MASK_MASK 0x80000u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_31_CTRL_PACKET_MASK_POS 20
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_SAM_MASK_31_CTRL_PACKET_MASK_MASK 0x100000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_UP_INNER_MASK_POS 21
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_UP_INNER_MASK_MASK 0x200000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_ID_INNER_MASK_POS 22
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_SAM_MASK_31_VLAN_ID_INNER_MASK_MASK 0x400000u

//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_31_MACSEC_SCI_MASK_POS 23
//! Field MACSEC_SCI_MASK - No Content.
#define EIPI_SAM_MASK_31_MACSEC_SCI_MASK_MASK 0x800000u

//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_31_TCI_AN_MASK_POS 24
//! Field TCI_AN_MASK - No Content.
#define EIPI_SAM_MASK_31_TCI_AN_MASK_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_EXT_MATCH_31 Register EIPI_SAM_EXT_MATCH_31 - EIPI SAM EXT MATCH 31
//! @{

//! Register Offset (relative)
#define EIPI_SAM_EXT_MATCH_31 0x7E0
//! Register Offset (absolute) for 1st Instance ING_SAM_RULES_PAGE0
#define ING_REG_SAM_EXT_MATCH_31 0x47E0u

//! Register Reset Value
#define EIPI_SAM_EXT_MATCH_31_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_31_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_31_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_31_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_SAM_EXT_MATCH_31_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_31_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_EXT_MATCH_31_RESERVED_0_MASK 0x8000u

//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_31_FLOW_INDEX_POS 16
//! Field FLOW_INDEX - No Content.
#define EIPI_SAM_EXT_MATCH_31_FLOW_INDEX_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_31_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_EXT_MATCH_31_RESERVED_1_MASK 0xFFE00000u

//! @}

//! @}

#endif
