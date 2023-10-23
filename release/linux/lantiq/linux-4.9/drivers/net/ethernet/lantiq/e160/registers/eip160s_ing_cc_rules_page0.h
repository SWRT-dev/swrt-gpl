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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_CC_rules_page0_def.xml
// Register File Name  : ING_CC_RULES_PAGE0
// Register File Title : EIPI EIP_160s_i_32_CC_rules_page0
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_CC_RULES_PAGE0_H
#define _ING_CC_RULES_PAGE0_H

//! \defgroup ING_CC_RULES_PAGE0 Register File ING_CC_RULES_PAGE0 - EIPI EIP_160s_i_32_CC_rules_page0
//! @{

//! Base Address of ING_CC_RULES_PAGE0
#define ING_CC_RULES_PAGE0_MODULE_BASE 0xE000u

//! \defgroup EIPI_IG_CC_ENTRY_LO_0 Register EIPI_IG_CC_ENTRY_LO_0 - EIPI IG CC ENTRY LO 0
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_0 0x0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_0 0xE000u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_0_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_0_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_0 Register EIPI_IG_CC_ENTRY_HI_0 - EIPI IG CC ENTRY HI 0
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_0 0x4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_0 0xE004u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_0_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_0_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_0 Register EIPI_IG_CC_ENTRY2_0 - EIPI IG CC ENTRY2 0
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_0 0x8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_0 0xE008u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_0_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_0_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_0_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_0_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_0_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_0_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_0_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_1 Register EIPI_IG_CC_ENTRY_LO_1 - EIPI IG CC ENTRY LO 1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_1 0x10
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_1 0xE010u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_1_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_1_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_1 Register EIPI_IG_CC_ENTRY_HI_1 - EIPI IG CC ENTRY HI 1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_1 0x14
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_1 0xE014u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_1_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_1_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_1 Register EIPI_IG_CC_ENTRY2_1 - EIPI IG CC ENTRY2 1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_1 0x18
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_1 0xE018u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_1_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_1_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_1_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_1_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_1_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_1_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_1_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_2 Register EIPI_IG_CC_ENTRY_LO_2 - EIPI IG CC ENTRY LO 2
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_2 0x20
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_2 0xE020u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_2_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_2_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_2 Register EIPI_IG_CC_ENTRY_HI_2 - EIPI IG CC ENTRY HI 2
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_2 0x24
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_2 0xE024u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_2_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_2_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_2 Register EIPI_IG_CC_ENTRY2_2 - EIPI IG CC ENTRY2 2
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_2 0x28
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_2 0xE028u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_2_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_2_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_2_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_2_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_2_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_2_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_2_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_3 Register EIPI_IG_CC_ENTRY_LO_3 - EIPI IG CC ENTRY LO 3
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_3 0x30
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_3 0xE030u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_3_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_3_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_3 Register EIPI_IG_CC_ENTRY_HI_3 - EIPI IG CC ENTRY HI 3
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_3 0x34
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_3 0xE034u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_3_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_3_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_3 Register EIPI_IG_CC_ENTRY2_3 - EIPI IG CC ENTRY2 3
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_3 0x38
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_3 0xE038u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_3_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_3_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_3_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_3_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_3_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_3_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_3_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_4 Register EIPI_IG_CC_ENTRY_LO_4 - EIPI IG CC ENTRY LO 4
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_4 0x40
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_4 0xE040u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_4_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_4_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_4 Register EIPI_IG_CC_ENTRY_HI_4 - EIPI IG CC ENTRY HI 4
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_4 0x44
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_4 0xE044u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_4_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_4_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_4 Register EIPI_IG_CC_ENTRY2_4 - EIPI IG CC ENTRY2 4
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_4 0x48
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_4 0xE048u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_4_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_4_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_4_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_4_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_4_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_4_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_4_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_5 Register EIPI_IG_CC_ENTRY_LO_5 - EIPI IG CC ENTRY LO 5
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_5 0x50
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_5 0xE050u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_5_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_5_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_5 Register EIPI_IG_CC_ENTRY_HI_5 - EIPI IG CC ENTRY HI 5
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_5 0x54
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_5 0xE054u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_5_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_5_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_5 Register EIPI_IG_CC_ENTRY2_5 - EIPI IG CC ENTRY2 5
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_5 0x58
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_5 0xE058u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_5_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_5_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_5_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_5_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_5_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_5_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_5_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_6 Register EIPI_IG_CC_ENTRY_LO_6 - EIPI IG CC ENTRY LO 6
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_6 0x60
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_6 0xE060u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_6_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_6_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_6 Register EIPI_IG_CC_ENTRY_HI_6 - EIPI IG CC ENTRY HI 6
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_6 0x64
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_6 0xE064u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_6_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_6_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_6 Register EIPI_IG_CC_ENTRY2_6 - EIPI IG CC ENTRY2 6
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_6 0x68
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_6 0xE068u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_6_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_6_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_6_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_6_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_6_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_6_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_6_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_7 Register EIPI_IG_CC_ENTRY_LO_7 - EIPI IG CC ENTRY LO 7
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_7 0x70
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_7 0xE070u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_7_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_7_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_7 Register EIPI_IG_CC_ENTRY_HI_7 - EIPI IG CC ENTRY HI 7
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_7 0x74
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_7 0xE074u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_7_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_7_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_7 Register EIPI_IG_CC_ENTRY2_7 - EIPI IG CC ENTRY2 7
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_7 0x78
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_7 0xE078u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_7_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_7_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_7_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_7_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_7_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_7_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_7_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_8 Register EIPI_IG_CC_ENTRY_LO_8 - EIPI IG CC ENTRY LO 8
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_8 0x80
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_8 0xE080u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_8_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_8_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_8 Register EIPI_IG_CC_ENTRY_HI_8 - EIPI IG CC ENTRY HI 8
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_8 0x84
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_8 0xE084u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_8_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_8_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_8 Register EIPI_IG_CC_ENTRY2_8 - EIPI IG CC ENTRY2 8
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_8 0x88
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_8 0xE088u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_8_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_8_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_8_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_8_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_8_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_8_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_8_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_9 Register EIPI_IG_CC_ENTRY_LO_9 - EIPI IG CC ENTRY LO 9
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_9 0x90
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_9 0xE090u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_9_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_9_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_9 Register EIPI_IG_CC_ENTRY_HI_9 - EIPI IG CC ENTRY HI 9
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_9 0x94
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_9 0xE094u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_9_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_9_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_9 Register EIPI_IG_CC_ENTRY2_9 - EIPI IG CC ENTRY2 9
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_9 0x98
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_9 0xE098u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_9_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_9_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_9_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_9_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_9_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_9_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_9_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_10 Register EIPI_IG_CC_ENTRY_LO_10 - EIPI IG CC ENTRY LO 10
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_10 0xA0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_10 0xE0A0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_10_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_10_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_10 Register EIPI_IG_CC_ENTRY_HI_10 - EIPI IG CC ENTRY HI 10
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_10 0xA4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_10 0xE0A4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_10_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_10_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_10 Register EIPI_IG_CC_ENTRY2_10 - EIPI IG CC ENTRY2 10
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_10 0xA8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_10 0xE0A8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_10_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_10_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_10_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_10_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_10_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_10_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_10_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_11 Register EIPI_IG_CC_ENTRY_LO_11 - EIPI IG CC ENTRY LO 11
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_11 0xB0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_11 0xE0B0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_11_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_11_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_11 Register EIPI_IG_CC_ENTRY_HI_11 - EIPI IG CC ENTRY HI 11
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_11 0xB4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_11 0xE0B4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_11_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_11_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_11 Register EIPI_IG_CC_ENTRY2_11 - EIPI IG CC ENTRY2 11
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_11 0xB8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_11 0xE0B8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_11_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_11_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_11_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_11_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_11_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_11_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_11_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_12 Register EIPI_IG_CC_ENTRY_LO_12 - EIPI IG CC ENTRY LO 12
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_12 0xC0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_12 0xE0C0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_12_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_12_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_12 Register EIPI_IG_CC_ENTRY_HI_12 - EIPI IG CC ENTRY HI 12
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_12 0xC4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_12 0xE0C4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_12_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_12_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_12 Register EIPI_IG_CC_ENTRY2_12 - EIPI IG CC ENTRY2 12
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_12 0xC8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_12 0xE0C8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_12_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_12_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_12_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_12_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_12_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_12_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_12_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_13 Register EIPI_IG_CC_ENTRY_LO_13 - EIPI IG CC ENTRY LO 13
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_13 0xD0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_13 0xE0D0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_13_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_13_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_13 Register EIPI_IG_CC_ENTRY_HI_13 - EIPI IG CC ENTRY HI 13
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_13 0xD4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_13 0xE0D4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_13_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_13_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_13 Register EIPI_IG_CC_ENTRY2_13 - EIPI IG CC ENTRY2 13
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_13 0xD8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_13 0xE0D8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_13_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_13_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_13_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_13_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_13_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_13_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_13_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_14 Register EIPI_IG_CC_ENTRY_LO_14 - EIPI IG CC ENTRY LO 14
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_14 0xE0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_14 0xE0E0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_14_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_14_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_14 Register EIPI_IG_CC_ENTRY_HI_14 - EIPI IG CC ENTRY HI 14
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_14 0xE4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_14 0xE0E4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_14_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_14_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_14 Register EIPI_IG_CC_ENTRY2_14 - EIPI IG CC ENTRY2 14
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_14 0xE8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_14 0xE0E8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_14_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_14_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_14_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_14_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_14_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_14_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_14_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_15 Register EIPI_IG_CC_ENTRY_LO_15 - EIPI IG CC ENTRY LO 15
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_15 0xF0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_15 0xE0F0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_15_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_15_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_15 Register EIPI_IG_CC_ENTRY_HI_15 - EIPI IG CC ENTRY HI 15
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_15 0xF4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_15 0xE0F4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_15_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_15_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_15 Register EIPI_IG_CC_ENTRY2_15 - EIPI IG CC ENTRY2 15
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_15 0xF8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_15 0xE0F8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_15_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_15_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_15_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_15_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_15_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_15_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_15_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_16 Register EIPI_IG_CC_ENTRY_LO_16 - EIPI IG CC ENTRY LO 16
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_16 0x100
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_16 0xE100u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_16_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_16_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_16 Register EIPI_IG_CC_ENTRY_HI_16 - EIPI IG CC ENTRY HI 16
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_16 0x104
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_16 0xE104u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_16_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_16_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_16 Register EIPI_IG_CC_ENTRY2_16 - EIPI IG CC ENTRY2 16
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_16 0x108
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_16 0xE108u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_16_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_16_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_16_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_16_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_16_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_16_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_16_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_17 Register EIPI_IG_CC_ENTRY_LO_17 - EIPI IG CC ENTRY LO 17
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_17 0x110
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_17 0xE110u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_17_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_17_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_17 Register EIPI_IG_CC_ENTRY_HI_17 - EIPI IG CC ENTRY HI 17
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_17 0x114
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_17 0xE114u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_17_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_17_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_17 Register EIPI_IG_CC_ENTRY2_17 - EIPI IG CC ENTRY2 17
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_17 0x118
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_17 0xE118u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_17_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_17_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_17_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_17_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_17_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_17_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_17_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_18 Register EIPI_IG_CC_ENTRY_LO_18 - EIPI IG CC ENTRY LO 18
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_18 0x120
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_18 0xE120u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_18_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_18_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_18 Register EIPI_IG_CC_ENTRY_HI_18 - EIPI IG CC ENTRY HI 18
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_18 0x124
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_18 0xE124u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_18_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_18_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_18 Register EIPI_IG_CC_ENTRY2_18 - EIPI IG CC ENTRY2 18
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_18 0x128
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_18 0xE128u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_18_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_18_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_18_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_18_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_18_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_18_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_18_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_19 Register EIPI_IG_CC_ENTRY_LO_19 - EIPI IG CC ENTRY LO 19
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_19 0x130
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_19 0xE130u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_19_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_19_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_19 Register EIPI_IG_CC_ENTRY_HI_19 - EIPI IG CC ENTRY HI 19
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_19 0x134
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_19 0xE134u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_19_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_19_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_19 Register EIPI_IG_CC_ENTRY2_19 - EIPI IG CC ENTRY2 19
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_19 0x138
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_19 0xE138u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_19_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_19_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_19_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_19_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_19_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_19_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_19_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_20 Register EIPI_IG_CC_ENTRY_LO_20 - EIPI IG CC ENTRY LO 20
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_20 0x140
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_20 0xE140u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_20_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_20_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_20 Register EIPI_IG_CC_ENTRY_HI_20 - EIPI IG CC ENTRY HI 20
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_20 0x144
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_20 0xE144u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_20_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_20_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_20 Register EIPI_IG_CC_ENTRY2_20 - EIPI IG CC ENTRY2 20
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_20 0x148
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_20 0xE148u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_20_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_20_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_20_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_20_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_20_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_20_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_20_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_21 Register EIPI_IG_CC_ENTRY_LO_21 - EIPI IG CC ENTRY LO 21
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_21 0x150
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_21 0xE150u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_21_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_21_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_21 Register EIPI_IG_CC_ENTRY_HI_21 - EIPI IG CC ENTRY HI 21
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_21 0x154
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_21 0xE154u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_21_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_21_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_21 Register EIPI_IG_CC_ENTRY2_21 - EIPI IG CC ENTRY2 21
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_21 0x158
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_21 0xE158u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_21_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_21_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_21_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_21_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_21_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_21_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_21_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_22 Register EIPI_IG_CC_ENTRY_LO_22 - EIPI IG CC ENTRY LO 22
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_22 0x160
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_22 0xE160u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_22_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_22_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_22 Register EIPI_IG_CC_ENTRY_HI_22 - EIPI IG CC ENTRY HI 22
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_22 0x164
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_22 0xE164u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_22_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_22_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_22 Register EIPI_IG_CC_ENTRY2_22 - EIPI IG CC ENTRY2 22
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_22 0x168
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_22 0xE168u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_22_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_22_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_22_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_22_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_22_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_22_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_22_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_23 Register EIPI_IG_CC_ENTRY_LO_23 - EIPI IG CC ENTRY LO 23
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_23 0x170
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_23 0xE170u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_23_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_23_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_23 Register EIPI_IG_CC_ENTRY_HI_23 - EIPI IG CC ENTRY HI 23
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_23 0x174
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_23 0xE174u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_23_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_23_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_23 Register EIPI_IG_CC_ENTRY2_23 - EIPI IG CC ENTRY2 23
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_23 0x178
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_23 0xE178u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_23_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_23_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_23_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_23_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_23_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_23_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_23_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_24 Register EIPI_IG_CC_ENTRY_LO_24 - EIPI IG CC ENTRY LO 24
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_24 0x180
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_24 0xE180u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_24_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_24_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_24 Register EIPI_IG_CC_ENTRY_HI_24 - EIPI IG CC ENTRY HI 24
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_24 0x184
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_24 0xE184u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_24_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_24_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_24 Register EIPI_IG_CC_ENTRY2_24 - EIPI IG CC ENTRY2 24
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_24 0x188
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_24 0xE188u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_24_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_24_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_24_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_24_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_24_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_24_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_24_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_25 Register EIPI_IG_CC_ENTRY_LO_25 - EIPI IG CC ENTRY LO 25
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_25 0x190
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_25 0xE190u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_25_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_25_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_25 Register EIPI_IG_CC_ENTRY_HI_25 - EIPI IG CC ENTRY HI 25
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_25 0x194
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_25 0xE194u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_25_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_25_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_25 Register EIPI_IG_CC_ENTRY2_25 - EIPI IG CC ENTRY2 25
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_25 0x198
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_25 0xE198u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_25_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_25_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_25_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_25_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_25_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_25_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_25_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_26 Register EIPI_IG_CC_ENTRY_LO_26 - EIPI IG CC ENTRY LO 26
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_26 0x1A0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_26 0xE1A0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_26_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_26_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_26 Register EIPI_IG_CC_ENTRY_HI_26 - EIPI IG CC ENTRY HI 26
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_26 0x1A4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_26 0xE1A4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_26_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_26_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_26 Register EIPI_IG_CC_ENTRY2_26 - EIPI IG CC ENTRY2 26
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_26 0x1A8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_26 0xE1A8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_26_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_26_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_26_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_26_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_26_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_26_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_26_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_27 Register EIPI_IG_CC_ENTRY_LO_27 - EIPI IG CC ENTRY LO 27
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_27 0x1B0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_27 0xE1B0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_27_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_27_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_27 Register EIPI_IG_CC_ENTRY_HI_27 - EIPI IG CC ENTRY HI 27
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_27 0x1B4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_27 0xE1B4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_27_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_27_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_27 Register EIPI_IG_CC_ENTRY2_27 - EIPI IG CC ENTRY2 27
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_27 0x1B8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_27 0xE1B8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_27_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_27_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_27_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_27_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_27_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_27_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_27_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_28 Register EIPI_IG_CC_ENTRY_LO_28 - EIPI IG CC ENTRY LO 28
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_28 0x1C0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_28 0xE1C0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_28_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_28_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_28 Register EIPI_IG_CC_ENTRY_HI_28 - EIPI IG CC ENTRY HI 28
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_28 0x1C4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_28 0xE1C4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_28_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_28_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_28 Register EIPI_IG_CC_ENTRY2_28 - EIPI IG CC ENTRY2 28
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_28 0x1C8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_28 0xE1C8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_28_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_28_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_28_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_28_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_28_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_28_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_28_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_29 Register EIPI_IG_CC_ENTRY_LO_29 - EIPI IG CC ENTRY LO 29
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_29 0x1D0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_29 0xE1D0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_29_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_29_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_29 Register EIPI_IG_CC_ENTRY_HI_29 - EIPI IG CC ENTRY HI 29
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_29 0x1D4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_29 0xE1D4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_29_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_29_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_29 Register EIPI_IG_CC_ENTRY2_29 - EIPI IG CC ENTRY2 29
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_29 0x1D8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_29 0xE1D8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_29_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_29_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_29_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_29_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_29_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_29_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_29_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_30 Register EIPI_IG_CC_ENTRY_LO_30 - EIPI IG CC ENTRY LO 30
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_30 0x1E0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_30 0xE1E0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_30_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_30_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_30 Register EIPI_IG_CC_ENTRY_HI_30 - EIPI IG CC ENTRY HI 30
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_30 0x1E4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_30 0xE1E4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_30_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_30_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_30 Register EIPI_IG_CC_ENTRY2_30 - EIPI IG CC ENTRY2 30
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_30 0x1E8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_30 0xE1E8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_30_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_30_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_30_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_30_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_30_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_30_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_30_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_LO_31 Register EIPI_IG_CC_ENTRY_LO_31 - EIPI IG CC ENTRY LO 31
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_LO_31 0x1F0
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_LO_31 0xE1F0u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_LO_31_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_VLAN_ID_MASK 0xFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_RESERVED_0_POS 12
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_RESERVED_0_MASK 0xF000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_ENTRY_LO_31_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY_HI_31 Register EIPI_IG_CC_ENTRY_HI_31 - EIPI IG CC ENTRY HI 31
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY_HI_31 0x1F4
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY_HI_31 0xE1F4u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY_HI_31_RST 0x00000000u

//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SAI_HIT_POS 0
//! Field SAI_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SAI_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_ETYPE_VALID_MASK 0x4u

//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_CTRL_PACKET_POS 3
//! Field CTRL_PACKET - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_CTRL_PACKET_MASK 0x8u

//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_HIT_MASK_POS 4
//! Field SA_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_HIT_MASK_MASK 0x10u

//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_NR_MASK_POS 5
//! Field SA_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_NR_MASK_MASK 0x20u

//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_VALID_MASK_POS 6
//! Field VLAN_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_VALID_MASK_MASK 0x40u

//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_ID_MASK_POS 7
//! Field VLAN_ID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_ID_MASK_MASK 0x80u

//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_ETYPE_VALID_MASK_POS 8
//! Field ETYPE_VALID_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_ETYPE_VALID_MASK_MASK 0x100u

//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_PAYL_ETYPE_MASK_POS 9
//! Field PAYL_ETYPE_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_PAYL_ETYPE_MASK_MASK 0x200u

//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_CTRL_PACKET_MASK_POS 10
//! Field CTRL_PACKET_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_CTRL_PACKET_MASK_MASK 0x400u

//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_NR_MASK_POS 11
//! Field RULE_NR_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_NR_MASK_MASK 0x800u

//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_MATCH_PRIORITY_POS 12
//! Field MATCH_PRIORITY - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_MATCH_PRIORITY_MASK 0x7000u

//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_MATCH_ACTION_POS 15
//! Field MATCH_ACTION - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_MATCH_ACTION_MASK 0x8000u

//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_OR_RULE_NR_POS 16
//! Field SA_OR_RULE_NR - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_SA_OR_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RESERVED_0_POS 21
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RESERVED_0_MASK 0xE00000u

//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_UP_INNER_MASK_POS 24
//! Field VLAN_UP_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_UP_INNER_MASK_MASK 0x1000000u

//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_ID_INNER_MASK_POS 25
//! Field VLAN_ID_INNER_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_VLAN_ID_INNER_MASK_MASK 0x2000000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_HIT_POS 26
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_HIT_MASK 0x4000000u

//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_HIT_MASK_POS 27
//! Field RULE_HIT_MASK - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RULE_HIT_MASK_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_ENTRY_HI_31_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPI_IG_CC_ENTRY2_31 Register EIPI_IG_CC_ENTRY2_31 - EIPI IG CC ENTRY2 31
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENTRY2_31 0x1F8
//! Register Offset (absolute) for 1st Instance ING_CC_RULES_PAGE0
#define ING_REG_IG_CC_ENTRY2_31 0xE1F8u

//! Register Reset Value
#define EIPI_IG_CC_ENTRY2_31_RST 0x00000000u

//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_31_VLAN_ID_INNER_POS 0
//! Field VLAN_ID_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_31_VLAN_ID_INNER_MASK 0xFFFu

//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_31_VLAN_UP_INNER_POS 12
//! Field VLAN_UP_INNER - No Content.
#define EIPI_IG_CC_ENTRY2_31_VLAN_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_31_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_ENTRY2_31_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! @}

#endif
