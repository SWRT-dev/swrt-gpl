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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_CC_control_def.xml
// Register File Name  : ING_CC_CONTROL
// Register File Title : EIPI EIP_160s_i_32_CC_control
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_CC_CONTROL_H
#define _ING_CC_CONTROL_H

//! \defgroup ING_CC_CONTROL Register File ING_CC_CONTROL - EIPI EIP_160s_i_32_CC_control
//! @{

//! Base Address of ING_CC_CONTROL
#define ING_CC_CONTROL_MODULE_BASE 0xE800u

//! \defgroup EIPI_IG_CC_ENABLE1 Register EIPI_IG_CC_ENABLE1 - EIPI IG CC ENABLE1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_ENABLE1 0x0
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_ENABLE1 0xE800u

//! Register Reset Value
#define EIPI_IG_CC_ENABLE1_RST 0x00000000u

//! Field ENABLE_31_0 - No Content.
#define EIPI_IG_CC_ENABLE1_ENABLE_31_0_POS 0
//! Field ENABLE_31_0 - No Content.
#define EIPI_IG_CC_ENABLE1_ENABLE_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_IG_CC_CONTROL Register EIPI_IG_CC_CONTROL - EIPI IG CC CONTROL
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_CONTROL 0x40
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_CONTROL 0xE840u

//! Register Reset Value
#define EIPI_IG_CC_CONTROL_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_CONTROL_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_CONTROL_RESERVED_0_MASK 0x3FFFu

//! Field NON_MATCH_CTRL_ACT - No Content.
#define EIPI_IG_CC_CONTROL_NON_MATCH_CTRL_ACT_POS 14
//! Field NON_MATCH_CTRL_ACT - No Content.
#define EIPI_IG_CC_CONTROL_NON_MATCH_CTRL_ACT_MASK 0x4000u

//! Field NON_MATCH_ACT - No Content.
#define EIPI_IG_CC_CONTROL_NON_MATCH_ACT_POS 15
//! Field NON_MATCH_ACT - No Content.
#define EIPI_IG_CC_CONTROL_NON_MATCH_ACT_MASK 0x8000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_CONTROL_RESERVED_1_POS 16
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_CONTROL_RESERVED_1_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_TAGS Register EIPI_IG_CC_TAGS - EIPI IG CC TAGS
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_TAGS 0x44
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_TAGS 0xE844u

//! Register Reset Value
#define EIPI_IG_CC_TAGS_RST 0x05FF0000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_TAGS_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_TAGS_RESERVED_0_MASK 0xFFFFu

//! Field CP_ETYPE_MAX_LEN - No Content.
#define EIPI_IG_CC_TAGS_CP_ETYPE_MAX_LEN_POS 16
//! Field CP_ETYPE_MAX_LEN - No Content.
#define EIPI_IG_CC_TAGS_CP_ETYPE_MAX_LEN_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_CC_ENTRY_ENABLE_CTRL Register EIPI_CC_ENTRY_ENABLE_CTRL - EIPI CC ENTRY ENABLE CTRL
//! @{

//! Register Offset (relative)
#define EIPI_CC_ENTRY_ENABLE_CTRL 0x48
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_CC_ENTRY_ENABLE_CTRL 0xE848u

//! Register Reset Value
#define EIPI_CC_ENTRY_ENABLE_CTRL_RST 0x00000000u

//! Field CC_INDEX_SET - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CC_INDEX_SET_POS 0
//! Field CC_INDEX_SET - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CC_INDEX_SET_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_RESERVED_0_MASK 0x3FE0u

//! Field SET_ENABLE - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_SET_ENABLE_POS 14
//! Field SET_ENABLE - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_SET_ENABLE_MASK 0x4000u

//! Field SET_ALL - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_SET_ALL_POS 15
//! Field SET_ALL - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_SET_ALL_MASK 0x8000u

//! Field CC_INDEX_CLEAR - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CC_INDEX_CLEAR_POS 16
//! Field CC_INDEX_CLEAR - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CC_INDEX_CLEAR_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_RESERVED_1_MASK 0x3FE00000u

//! Field CLEAR_ENABLE - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_POS 30
//! Field CLEAR_ENABLE - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CLEAR_ENABLE_MASK 0x40000000u

//! Field CLEAR_ALL - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CLEAR_ALL_POS 31
//! Field CLEAR_ALL - No Content.
#define EIPI_CC_ENTRY_ENABLE_CTRL_CLEAR_ALL_MASK 0x80000000u

//! @}

//! \defgroup EIPI_IG_CP_TAG Register EIPI_IG_CP_TAG - EIPI IG CP TAG
//! @{

//! Register Offset (relative)
#define EIPI_IG_CP_TAG 0x50
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CP_TAG 0xE850u

//! Register Reset Value
#define EIPI_IG_CP_TAG_RST 0x00000000u

//! Field MAP_TBL - No Content.
#define EIPI_IG_CP_TAG_MAP_TBL_POS 0
//! Field MAP_TBL - No Content.
#define EIPI_IG_CP_TAG_MAP_TBL_MASK 0xFFFFFFu

//! Field DEF_UP - No Content.
#define EIPI_IG_CP_TAG_DEF_UP_POS 24
//! Field DEF_UP - No Content.
#define EIPI_IG_CP_TAG_DEF_UP_MASK 0x7000000u

//! Field STAQ_UP_EN - No Content.
#define EIPI_IG_CP_TAG_STAQ_UP_EN_POS 27
//! Field STAQ_UP_EN - No Content.
#define EIPI_IG_CP_TAG_STAQ_UP_EN_MASK 0x8000000u

//! Field QTAG_UP_EN - No Content.
#define EIPI_IG_CP_TAG_QTAG_UP_EN_POS 28
//! Field QTAG_UP_EN - No Content.
#define EIPI_IG_CP_TAG_QTAG_UP_EN_MASK 0x10000000u

//! Field PARSE_QINQ - No Content.
#define EIPI_IG_CP_TAG_PARSE_QINQ_POS 29
//! Field PARSE_QINQ - No Content.
#define EIPI_IG_CP_TAG_PARSE_QINQ_MASK 0x20000000u

//! Field PARSE_STAG - No Content.
#define EIPI_IG_CP_TAG_PARSE_STAG_POS 30
//! Field PARSE_STAG - No Content.
#define EIPI_IG_CP_TAG_PARSE_STAG_MASK 0x40000000u

//! Field PARSE_QTAG - No Content.
#define EIPI_IG_CP_TAG_PARSE_QTAG_POS 31
//! Field PARSE_QTAG - No Content.
#define EIPI_IG_CP_TAG_PARSE_QTAG_MASK 0x80000000u

//! @}

//! \defgroup EIPI_IG_PP_TAGS Register EIPI_IG_PP_TAGS - EIPI IG PP TAGS
//! @{

//! Register Offset (relative)
#define EIPI_IG_PP_TAGS 0x54
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_PP_TAGS 0xE854u

//! Register Reset Value
#define EIPI_IG_PP_TAGS_RST 0xA8880081u

//! Field QTAG_VALUE - No Content.
#define EIPI_IG_PP_TAGS_QTAG_VALUE_POS 0
//! Field QTAG_VALUE - No Content.
#define EIPI_IG_PP_TAGS_QTAG_VALUE_MASK 0xFFFFu

//! Field STAG_VALUE - No Content.
#define EIPI_IG_PP_TAGS_STAG_VALUE_POS 16
//! Field STAG_VALUE - No Content.
#define EIPI_IG_PP_TAGS_STAG_VALUE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_PP_TAGS2 Register EIPI_IG_PP_TAGS2 - EIPI IG PP TAGS2
//! @{

//! Register Offset (relative)
#define EIPI_IG_PP_TAGS2 0x58
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_PP_TAGS2 0xE858u

//! Register Reset Value
#define EIPI_IG_PP_TAGS2_RST 0x00920091u

//! Field STAG_VALUE2 - No Content.
#define EIPI_IG_PP_TAGS2_STAG_VALUE2_POS 0
//! Field STAG_VALUE2 - No Content.
#define EIPI_IG_PP_TAGS2_STAG_VALUE2_MASK 0xFFFFu

//! Field STAG_VALUE3 - No Content.
#define EIPI_IG_PP_TAGS2_STAG_VALUE3_POS 16
//! Field STAG_VALUE3 - No Content.
#define EIPI_IG_PP_TAGS2_STAG_VALUE3_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CP_TAG2 Register EIPI_IG_CP_TAG2 - EIPI IG CP TAG2
//! @{

//! Register Offset (relative)
#define EIPI_IG_CP_TAG2 0x5C
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CP_TAG2 0xE85Cu

//! Register Reset Value
#define EIPI_IG_CP_TAG2_RST 0x00000000u

//! Field MAP_TBL2 - No Content.
#define EIPI_IG_CP_TAG2_MAP_TBL2_POS 0
//! Field MAP_TBL2 - No Content.
#define EIPI_IG_CP_TAG2_MAP_TBL2_MASK 0xFFFFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CP_TAG2_RESERVED_0_POS 24
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CP_TAG2_RESERVED_0_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_IG_CC_DEBUG_1 Register EIPI_IG_CC_DEBUG_1 - EIPI IG CC DEBUG 1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_DEBUG_1 0x400
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_DEBUG_1 0xEC00u

//! Register Reset Value
#define EIPI_IG_CC_DEBUG_1_RST 0x00000000u

//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_DEBUG_1_VLAN_ID_POS 0
//! Field VLAN_ID - No Content.
#define EIPI_IG_CC_DEBUG_1_VLAN_ID_MASK 0xFFFu

//! Field VLAN_UP - No Content.
#define EIPI_IG_CC_DEBUG_1_VLAN_UP_POS 12
//! Field VLAN_UP - No Content.
#define EIPI_IG_CC_DEBUG_1_VLAN_UP_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_1_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_1_RESERVED_0_MASK 0x8000u

//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_DEBUG_1_PAYLOAD_E_TYPE_POS 16
//! Field PAYLOAD_E_TYPE - No Content.
#define EIPI_IG_CC_DEBUG_1_PAYLOAD_E_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_IG_CC_DEBUG_2 Register EIPI_IG_CC_DEBUG_2 - EIPI IG CC DEBUG 2
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_DEBUG_2 0x404
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_DEBUG_2 0xEC04u

//! Register Reset Value
#define EIPI_IG_CC_DEBUG_2_RST 0x00008000u

//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_RULE_HIT_POS 0
//! Field RULE_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_RULE_HIT_MASK 0x1u

//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_VLAN_VALID_POS 1
//! Field VLAN_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_VLAN_VALID_MASK 0x2u

//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_ETYPE_VALID_POS 2
//! Field ETYPE_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_ETYPE_VALID_MASK 0x4u

//! Field CONTROL_PKT - No Content.
#define EIPI_IG_CC_DEBUG_2_CONTROL_PKT_POS 3
//! Field CONTROL_PKT - No Content.
#define EIPI_IG_CC_DEBUG_2_CONTROL_PKT_MASK 0x8u

//! Field STAG_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_STAG_VALID_POS 4
//! Field STAG_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_STAG_VALID_MASK 0x10u

//! Field QTAG_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_QTAG_VALID_POS 5
//! Field QTAG_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_QTAG_VALID_MASK 0x20u

//! Field PARSED_ETYPE_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_PARSED_ETYPE_VALID_POS 6
//! Field PARSED_ETYPE_VALID - No Content.
#define EIPI_IG_CC_DEBUG_2_PARSED_ETYPE_VALID_MASK 0x40u

//! Field QINQ_FOUND - No Content.
#define EIPI_IG_CC_DEBUG_2_QINQ_FOUND_POS 7
//! Field QINQ_FOUND - No Content.
#define EIPI_IG_CC_DEBUG_2_QINQ_FOUND_MASK 0x80u

//! Field SA_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_SA_HIT_POS 8
//! Field SA_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_SA_HIT_MASK 0x100u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_0_POS 9
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_0_MASK 0x3E00u

//! Field CC_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_CC_HIT_POS 14
//! Field CC_HIT - No Content.
#define EIPI_IG_CC_DEBUG_2_CC_HIT_MASK 0x4000u

//! Field ACTION_TAKEN - No Content.
#define EIPI_IG_CC_DEBUG_2_ACTION_TAKEN_POS 15
//! Field ACTION_TAKEN - No Content.
#define EIPI_IG_CC_DEBUG_2_ACTION_TAKEN_MASK 0x8000u

//! Field RULE_NR - No Content.
#define EIPI_IG_CC_DEBUG_2_RULE_NR_POS 16
//! Field RULE_NR - No Content.
#define EIPI_IG_CC_DEBUG_2_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_1_MASK 0xE00000u

//! Field MATCH_INDEX - No Content.
#define EIPI_IG_CC_DEBUG_2_MATCH_INDEX_POS 24
//! Field MATCH_INDEX - No Content.
#define EIPI_IG_CC_DEBUG_2_MATCH_INDEX_MASK 0x1F000000u

//! Field RESERVED_2 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_2_POS 29
//! Field RESERVED_2 - No Content.
#define EIPI_IG_CC_DEBUG_2_RESERVED_2_MASK 0xE0000000u

//! @}

//! \defgroup EIPI_IG_CC_DEBUG_3 Register EIPI_IG_CC_DEBUG_3 - EIPI IG CC DEBUG 3
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_DEBUG_3 0x408
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_DEBUG_3 0xEC08u

//! Register Reset Value
#define EIPI_IG_CC_DEBUG_3_RST 0x00000000u

//! Field PARSED_VLAN_INNER - No Content.
#define EIPI_IG_CC_DEBUG_3_PARSED_VLAN_INNER_POS 0
//! Field PARSED_VLAN_INNER - No Content.
#define EIPI_IG_CC_DEBUG_3_PARSED_VLAN_INNER_MASK 0xFFFu

//! Field PARSED_UP_INNER - No Content.
#define EIPI_IG_CC_DEBUG_3_PARSED_UP_INNER_POS 12
//! Field PARSED_UP_INNER - No Content.
#define EIPI_IG_CC_DEBUG_3_PARSED_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_3_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPI_IG_CC_DEBUG_3_RESERVED_0_MASK 0x8000u

//! Field SA_ID - No Content.
#define EIPI_IG_CC_DEBUG_3_SA_ID_POS 16
//! Field SA_ID - No Content.
#define EIPI_IG_CC_DEBUG_3_SA_ID_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_DEBUG_3_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPI_IG_CC_DEBUG_3_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPI_IG_CC_DEBUG_MATCH1 Register EIPI_IG_CC_DEBUG_MATCH1 - EIPI IG CC DEBUG MATCH1
//! @{

//! Register Offset (relative)
#define EIPI_IG_CC_DEBUG_MATCH1 0x440
//! Register Offset (absolute) for 1st Instance ING_CC_CONTROL
#define ING_REG_IG_CC_DEBUG_MATCH1 0xEC40u

//! Register Reset Value
#define EIPI_IG_CC_DEBUG_MATCH1_RST 0xFFFFFFFFu

//! Field MATCH_31_0 - No Content.
#define EIPI_IG_CC_DEBUG_MATCH1_MATCH_31_0_POS 0
//! Field MATCH_31_0 - No Content.
#define EIPI_IG_CC_DEBUG_MATCH1_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
