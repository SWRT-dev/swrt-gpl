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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_Classifier_debug_def.xml
// Register File Name  : EGR_CLASSIFIER_DEBUG
// Register File Title : EIPE EIP_160s_e_32_Classifier_debug
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------
#ifndef _EGR_CLASSIFIER_DEBUG_H
#define _EGR_CLASSIFIER_DEBUG_H

//! \defgroup EGR_CLASSIFIER_DEBUG Register File EGR_CLASSIFIER_DEBUG - EIPE EIP_160s_e_32_Classifier_debug
//! @{

//! Base Address of EGR_CLASSIFIER_DEBUG
#define EGR_CLASSIFIER_DEBUG_MODULE_BASE 0x7C00u

//! \defgroup EIPE_CP_MATCH_DEBUG Register EIPE_CP_MATCH_DEBUG - EIPE CP MATCH DEBUG
//! @{

//! Register Offset (relative)
#define EIPE_CP_MATCH_DEBUG 0x0
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CP_MATCH_DEBUG 0x7C00u

//! Register Reset Value
#define EIPE_CP_MATCH_DEBUG_RST 0x00000000u

//! Field MAC_DA_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_0_POS 0
//! Field MAC_DA_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_0_MASK 0x1u

//! Field MAC_DA_1 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_1_POS 1
//! Field MAC_DA_1 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_1_MASK 0x2u

//! Field MAC_DA_2 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_2_POS 2
//! Field MAC_DA_2 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_2_MASK 0x4u

//! Field MAC_DA_3 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_3_POS 3
//! Field MAC_DA_3 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_3_MASK 0x8u

//! Field MAC_DA_4 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_4_POS 4
//! Field MAC_DA_4 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_4_MASK 0x10u

//! Field MAC_DA_5 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_5_POS 5
//! Field MAC_DA_5 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_5_MASK 0x20u

//! Field MAC_DA_6 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_6_POS 6
//! Field MAC_DA_6 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_6_MASK 0x40u

//! Field MAC_DA_7 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_7_POS 7
//! Field MAC_DA_7 - No Content.
#define EIPE_CP_MATCH_DEBUG_MAC_DA_7_MASK 0x80u

//! Field E_TYPE_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_0_POS 8
//! Field E_TYPE_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_0_MASK 0x100u

//! Field E_TYPE_1 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_1_POS 9
//! Field E_TYPE_1 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_1_MASK 0x200u

//! Field E_TYPE_2 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_2_POS 10
//! Field E_TYPE_2 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_2_MASK 0x400u

//! Field E_TYPE_3 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_3_POS 11
//! Field E_TYPE_3 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_3_MASK 0x800u

//! Field E_TYPE_4 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_4_POS 12
//! Field E_TYPE_4 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_4_MASK 0x1000u

//! Field E_TYPE_5 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_5_POS 13
//! Field E_TYPE_5 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_5_MASK 0x2000u

//! Field E_TYPE_6 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_6_POS 14
//! Field E_TYPE_6 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_6_MASK 0x4000u

//! Field E_TYPE_7 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_7_POS 15
//! Field E_TYPE_7 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_7_MASK 0x8000u

//! Field COMB_8 - No Content.
#define EIPE_CP_MATCH_DEBUG_COMB_8_POS 16
//! Field COMB_8 - No Content.
#define EIPE_CP_MATCH_DEBUG_COMB_8_MASK 0x10000u

//! Field COMB_9 - No Content.
#define EIPE_CP_MATCH_DEBUG_COMB_9_POS 17
//! Field COMB_9 - No Content.
#define EIPE_CP_MATCH_DEBUG_COMB_9_MASK 0x20000u

//! Field RANGE - No Content.
#define EIPE_CP_MATCH_DEBUG_RANGE_POS 18
//! Field RANGE - No Content.
#define EIPE_CP_MATCH_DEBUG_RANGE_MASK 0x40000u

//! Field CONST_44 - No Content.
#define EIPE_CP_MATCH_DEBUG_CONST_44_POS 19
//! Field CONST_44 - No Content.
#define EIPE_CP_MATCH_DEBUG_CONST_44_MASK 0x80000u

//! Field CONST_48 - No Content.
#define EIPE_CP_MATCH_DEBUG_CONST_48_POS 20
//! Field CONST_48 - No Content.
#define EIPE_CP_MATCH_DEBUG_CONST_48_MASK 0x100000u

//! Field E_TYPE_10 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_10_POS 21
//! Field E_TYPE_10 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_10_MASK 0x200000u

//! Field E_TYPE_11 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_11_POS 22
//! Field E_TYPE_11 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_11_MASK 0x400000u

//! Field E_TYPE_12 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_12_POS 23
//! Field E_TYPE_12 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_12_MASK 0x800000u

//! Field E_TYPE_13 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_13_POS 24
//! Field E_TYPE_13 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_13_MASK 0x1000000u

//! Field E_TYPE_14 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_14_POS 25
//! Field E_TYPE_14 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_14_MASK 0x2000000u

//! Field E_TYPE_15 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_15_POS 26
//! Field E_TYPE_15 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_15_MASK 0x4000000u

//! Field E_TYPE_16 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_16_POS 27
//! Field E_TYPE_16 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_16_MASK 0x8000000u

//! Field E_TYPE_17 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_17_POS 28
//! Field E_TYPE_17 - No Content.
#define EIPE_CP_MATCH_DEBUG_E_TYPE_17_MASK 0x10000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_RESERVED_0_POS 29
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_DEBUG_RESERVED_0_MASK 0xE0000000u

//! @}

//! \defgroup EIPE_SAM_DEBUG Register EIPE_SAM_DEBUG - EIPE SAM DEBUG
//! @{

//! Register Offset (relative)
#define EIPE_SAM_DEBUG 0x20
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SAM_DEBUG 0x7C20u

//! Register Reset Value
#define EIPE_SAM_DEBUG_RST 0x00000000u

//! Field SA_NR - No Content.
#define EIPE_SAM_DEBUG_SA_NR_POS 0
//! Field SA_NR - No Content.
#define EIPE_SAM_DEBUG_SA_NR_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_0_MASK 0xFFE0u

//! Field RULE_NR - No Content.
#define EIPE_SAM_DEBUG_RULE_NR_POS 16
//! Field RULE_NR - No Content.
#define EIPE_SAM_DEBUG_RULE_NR_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_1_MASK 0xE00000u

//! Field SOURCE_PORT - No Content.
#define EIPE_SAM_DEBUG_SOURCE_PORT_POS 24
//! Field SOURCE_PORT - No Content.
#define EIPE_SAM_DEBUG_SOURCE_PORT_MASK 0x3000000u

//! Field MACSEC_CLASS - No Content.
#define EIPE_SAM_DEBUG_MACSEC_CLASS_POS 26
//! Field MACSEC_CLASS - No Content.
#define EIPE_SAM_DEBUG_MACSEC_CLASS_MASK 0xC000000u

//! Field CONTROL_PACKET - No Content.
#define EIPE_SAM_DEBUG_CONTROL_PACKET_POS 28
//! Field CONTROL_PACKET - No Content.
#define EIPE_SAM_DEBUG_CONTROL_PACKET_MASK 0x10000000u

//! Field RULE_HIT - No Content.
#define EIPE_SAM_DEBUG_RULE_HIT_POS 29
//! Field RULE_HIT - No Content.
#define EIPE_SAM_DEBUG_RULE_HIT_MASK 0x20000000u

//! Field SA_HIT - No Content.
#define EIPE_SAM_DEBUG_SA_HIT_POS 30
//! Field SA_HIT - No Content.
#define EIPE_SAM_DEBUG_SA_HIT_MASK 0x40000000u

//! Field RESERVED_2 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_2_POS 31
//! Field RESERVED_2 - No Content.
#define EIPE_SAM_DEBUG_RESERVED_2_MASK 0x80000000u

//! @}

//! \defgroup EIPE_SECTAG_DEBUG Register EIPE_SECTAG_DEBUG - EIPE SECTAG DEBUG
//! @{

//! Register Offset (relative)
#define EIPE_SECTAG_DEBUG 0x24
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SECTAG_DEBUG 0x7C24u

//! Register Reset Value
#define EIPE_SECTAG_DEBUG_RST 0x00000000u

//! Field MACSEC_CLASS - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_CLASS_POS 0
//! Field MACSEC_CLASS - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_CLASS_MASK 0x3u

//! Field MACSEC_SECTAG_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_SECTAG_VALID_POS 2
//! Field MACSEC_SECTAG_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_SECTAG_VALID_MASK 0x4u

//! Field MACSEC_SCI_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_SCI_VALID_POS 3
//! Field MACSEC_SCI_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_SCI_VALID_MASK 0x8u

//! Field MACSEC_TAG_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_TAG_VALID_POS 4
//! Field MACSEC_TAG_VALID - No Content.
#define EIPE_SECTAG_DEBUG_MACSEC_TAG_VALID_MASK 0x10u

//! Field RESERVED_0 - No Content.
#define EIPE_SECTAG_DEBUG_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_SECTAG_DEBUG_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_SAM_MATCH_DEBUG1 Register EIPE_SAM_MATCH_DEBUG1 - EIPE SAM MATCH DEBUG1
//! @{

//! Register Offset (relative)
#define EIPE_SAM_MATCH_DEBUG1 0x40
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SAM_MATCH_DEBUG1 0x7C40u

//! Register Reset Value
#define EIPE_SAM_MATCH_DEBUG1_RST 0x00000000u

//! Field MATCH_31_0 - No Content.
#define EIPE_SAM_MATCH_DEBUG1_MATCH_31_0_POS 0
//! Field MATCH_31_0 - No Content.
#define EIPE_SAM_MATCH_DEBUG1_MATCH_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SAM_PP_DEBUG1 Register EIPE_SAM_PP_DEBUG1 - EIPE SAM PP DEBUG1
//! @{

//! Register Offset (relative)
#define EIPE_SAM_PP_DEBUG1 0x80
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SAM_PP_DEBUG1 0x7C80u

//! Register Reset Value
#define EIPE_SAM_PP_DEBUG1_RST 0x00000000u

//! Field PARSED_ETHERTYPE - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_ETHERTYPE_POS 0
//! Field PARSED_ETHERTYPE - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_ETHERTYPE_MASK 0xFFFFu

//! Field PARSED_VLAN - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_VLAN_POS 16
//! Field PARSED_VLAN - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_VLAN_MASK 0xFFF0000u

//! Field PARSED_UP - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_UP_POS 28
//! Field PARSED_UP - No Content.
#define EIPE_SAM_PP_DEBUG1_PARSED_UP_MASK 0x70000000u

//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG1_RESERVED_0_POS 31
//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG1_RESERVED_0_MASK 0x80000000u

//! @}

//! \defgroup EIPE_SAM_PP_DEBUG2 Register EIPE_SAM_PP_DEBUG2 - EIPE SAM PP DEBUG2
//! @{

//! Register Offset (relative)
#define EIPE_SAM_PP_DEBUG2 0x84
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SAM_PP_DEBUG2 0x7C84u

//! Register Reset Value
#define EIPE_SAM_PP_DEBUG2_RST 0x00000000u

//! Field VLAN_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_VLAN_VALID_POS 0
//! Field VLAN_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_VLAN_VALID_MASK 0x1u

//! Field QINQ_FOUND - No Content.
#define EIPE_SAM_PP_DEBUG2_QINQ_FOUND_POS 1
//! Field QINQ_FOUND - No Content.
#define EIPE_SAM_PP_DEBUG2_QINQ_FOUND_MASK 0x2u

//! Field STAG_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_STAG_VALID_POS 2
//! Field STAG_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_STAG_VALID_MASK 0x4u

//! Field QTAG_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_QTAG_VALID_POS 3
//! Field QTAG_VALID - No Content.
#define EIPE_SAM_PP_DEBUG2_QTAG_VALID_MASK 0x8u

//! Field LPIDLE_PKT - No Content.
#define EIPE_SAM_PP_DEBUG2_LPIDLE_PKT_POS 4
//! Field LPIDLE_PKT - No Content.
#define EIPE_SAM_PP_DEBUG2_LPIDLE_PKT_MASK 0x10u

//! Field ONE_WORD_PKT - No Content.
#define EIPE_SAM_PP_DEBUG2_ONE_WORD_PKT_POS 5
//! Field ONE_WORD_PKT - No Content.
#define EIPE_SAM_PP_DEBUG2_ONE_WORD_PKT_MASK 0x20u

//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG2_RESERVED_0_POS 6
//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG2_RESERVED_0_MASK 0x7FFFFFC0u

//! Field SINGLE_STEP_EN - No Content.
#define EIPE_SAM_PP_DEBUG2_SINGLE_STEP_EN_POS 31
//! Field SINGLE_STEP_EN - No Content.
#define EIPE_SAM_PP_DEBUG2_SINGLE_STEP_EN_MASK 0x80000000u

//! @}

//! \defgroup EIPE_SAM_PP_DEBUG3 Register EIPE_SAM_PP_DEBUG3 - EIPE SAM PP DEBUG3
//! @{

//! Register Offset (relative)
#define EIPE_SAM_PP_DEBUG3 0x88
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_SAM_PP_DEBUG3 0x7C88u

//! Register Reset Value
#define EIPE_SAM_PP_DEBUG3_RST 0x00000000u

//! Field PARSED_VLAN_INNER - No Content.
#define EIPE_SAM_PP_DEBUG3_PARSED_VLAN_INNER_POS 0
//! Field PARSED_VLAN_INNER - No Content.
#define EIPE_SAM_PP_DEBUG3_PARSED_VLAN_INNER_MASK 0xFFFu

//! Field PARSED_UP_INNER - No Content.
#define EIPE_SAM_PP_DEBUG3_PARSED_UP_INNER_POS 12
//! Field PARSED_UP_INNER - No Content.
#define EIPE_SAM_PP_DEBUG3_PARSED_UP_INNER_MASK 0x7000u

//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG3_RESERVED_0_POS 15
//! Field RESERVED_0 - No Content.
#define EIPE_SAM_PP_DEBUG3_RESERVED_0_MASK 0xFFFF8000u

//! @}

//! \defgroup EIPE_DBG_PARSED_DA_LO Register EIPE_DBG_PARSED_DA_LO - EIPE DBG PARSED DA LO
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_DA_LO 0x100
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_DA_LO 0x7D00u

//! Register Reset Value
#define EIPE_DBG_PARSED_DA_LO_RST 0x00000000u

//! Field PARSED_MAC_DA_31_0 - No Content.
#define EIPE_DBG_PARSED_DA_LO_PARSED_MAC_DA_31_0_POS 0
//! Field PARSED_MAC_DA_31_0 - No Content.
#define EIPE_DBG_PARSED_DA_LO_PARSED_MAC_DA_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_PARSED_DA_HI Register EIPE_DBG_PARSED_DA_HI - EIPE DBG PARSED DA HI
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_DA_HI 0x104
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_DA_HI 0x7D04u

//! Register Reset Value
#define EIPE_DBG_PARSED_DA_HI_RST 0x00000000u

//! Field PARSED_MAC_DA_47_32 - No Content.
#define EIPE_DBG_PARSED_DA_HI_PARSED_MAC_DA_47_32_POS 0
//! Field PARSED_MAC_DA_47_32 - No Content.
#define EIPE_DBG_PARSED_DA_HI_PARSED_MAC_DA_47_32_MASK 0xFFFFu

//! Field ORIG_ETH_TYPE - No Content.
#define EIPE_DBG_PARSED_DA_HI_ORIG_ETH_TYPE_POS 16
//! Field ORIG_ETH_TYPE - No Content.
#define EIPE_DBG_PARSED_DA_HI_ORIG_ETH_TYPE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_DBG_PARSED_SA_LO Register EIPE_DBG_PARSED_SA_LO - EIPE DBG PARSED SA LO
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SA_LO 0x108
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SA_LO 0x7D08u

//! Register Reset Value
#define EIPE_DBG_PARSED_SA_LO_RST 0x00000000u

//! Field PARSED_MAC_SA_31_0 - No Content.
#define EIPE_DBG_PARSED_SA_LO_PARSED_MAC_SA_31_0_POS 0
//! Field PARSED_MAC_SA_31_0 - No Content.
#define EIPE_DBG_PARSED_SA_LO_PARSED_MAC_SA_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_PARSED_SA_HI Register EIPE_DBG_PARSED_SA_HI - EIPE DBG PARSED SA HI
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SA_HI 0x10C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SA_HI 0x7D0Cu

//! Register Reset Value
#define EIPE_DBG_PARSED_SA_HI_RST 0x00000000u

//! Field PARSED_MAC_SA_47_32 - No Content.
#define EIPE_DBG_PARSED_SA_HI_PARSED_MAC_SA_47_32_POS 0
//! Field PARSED_MAC_SA_47_32 - No Content.
#define EIPE_DBG_PARSED_SA_HI_PARSED_MAC_SA_47_32_MASK 0xFFFFu

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PARSED_SA_HI_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PARSED_SA_HI_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_DBG_PARSED_SECTAG_LO Register EIPE_DBG_PARSED_SECTAG_LO - EIPE DBG PARSED SECTAG LO
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SECTAG_LO 0x110
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SECTAG_LO 0x7D10u

//! Register Reset Value
#define EIPE_DBG_PARSED_SECTAG_LO_RST 0x00000000u

//! Field PARSED_SECTAG_31_0 - No Content.
#define EIPE_DBG_PARSED_SECTAG_LO_PARSED_SECTAG_31_0_POS 0
//! Field PARSED_SECTAG_31_0 - No Content.
#define EIPE_DBG_PARSED_SECTAG_LO_PARSED_SECTAG_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_PARSED_SECTAG_HI Register EIPE_DBG_PARSED_SECTAG_HI - EIPE DBG PARSED SECTAG HI
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SECTAG_HI 0x114
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SECTAG_HI 0x7D14u

//! Register Reset Value
#define EIPE_DBG_PARSED_SECTAG_HI_RST 0x00000000u

//! Field PARSED_SECTAG_63_32 - No Content.
#define EIPE_DBG_PARSED_SECTAG_HI_PARSED_SECTAG_63_32_POS 0
//! Field PARSED_SECTAG_63_32 - No Content.
#define EIPE_DBG_PARSED_SECTAG_HI_PARSED_SECTAG_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_PARSED_SCI_LO Register EIPE_DBG_PARSED_SCI_LO - EIPE DBG PARSED SCI LO
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SCI_LO 0x118
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SCI_LO 0x7D18u

//! Register Reset Value
#define EIPE_DBG_PARSED_SCI_LO_RST 0x00000000u

//! Field PARSED_SCI_31_0 - No Content.
#define EIPE_DBG_PARSED_SCI_LO_PARSED_SCI_31_0_POS 0
//! Field PARSED_SCI_31_0 - No Content.
#define EIPE_DBG_PARSED_SCI_LO_PARSED_SCI_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_PARSED_SCI_HI Register EIPE_DBG_PARSED_SCI_HI - EIPE DBG PARSED SCI HI
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PARSED_SCI_HI 0x11C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_PARSED_SCI_HI 0x7D1Cu

//! Register Reset Value
#define EIPE_DBG_PARSED_SCI_HI_RST 0x00000000u

//! Field PARSED_SCI_63_32 - No Content.
#define EIPE_DBG_PARSED_SCI_HI_PARSED_SCI_63_32_POS 0
//! Field PARSED_SCI_63_32 - No Content.
#define EIPE_DBG_PARSED_SCI_HI_PARSED_SCI_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_FLOW_LOOKUP Register EIPE_DBG_FLOW_LOOKUP - EIPE DBG FLOW LOOKUP
//! @{

//! Register Offset (relative)
#define EIPE_DBG_FLOW_LOOKUP 0x120
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_FLOW_LOOKUP 0x7D20u

//! Register Reset Value
#define EIPE_DBG_FLOW_LOOKUP_RST 0x00000000u

//! Field FLOW_LOOKUP_RES - No Content.
#define EIPE_DBG_FLOW_LOOKUP_FLOW_LOOKUP_RES_POS 0
//! Field FLOW_LOOKUP_RES - No Content.
#define EIPE_DBG_FLOW_LOOKUP_FLOW_LOOKUP_RES_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_SA_UPDATE Register EIPE_DBG_SA_UPDATE - EIPE DBG SA UPDATE
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_UPDATE 0x124
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_UPDATE 0x7D24u

//! Register Reset Value
#define EIPE_DBG_SA_UPDATE_RST 0x00000000u

//! Field SA_UPDATE_CTRL_WORD - No Content.
#define EIPE_DBG_SA_UPDATE_SA_UPDATE_CTRL_WORD_POS 0
//! Field SA_UPDATE_CTRL_WORD - No Content.
#define EIPE_DBG_SA_UPDATE_SA_UPDATE_CTRL_WORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_ATOK_DEBUG_0 Register EIPE_CRYPTO_ATOK_DEBUG_0 - EIPE CRYPTO ATOK DEBUG 0
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_ATOK_DEBUG_0 0x128
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CRYPTO_ATOK_DEBUG_0 0x7D28u

//! Register Reset Value
#define EIPE_CRYPTO_ATOK_DEBUG_0_RST 0x00000000u

//! Field ATOK_WORD - No Content.
#define EIPE_CRYPTO_ATOK_DEBUG_0_ATOK_WORD_POS 0
//! Field ATOK_WORD - No Content.
#define EIPE_CRYPTO_ATOK_DEBUG_0_ATOK_WORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_CMMD_DEBUG_0 Register EIPE_CRYPTO_CMMD_DEBUG_0 - EIPE CRYPTO CMMD DEBUG 0
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_CMMD_DEBUG_0 0x130
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CRYPTO_CMMD_DEBUG_0 0x7D30u

//! Register Reset Value
#define EIPE_CRYPTO_CMMD_DEBUG_0_RST 0x00000000u

//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_0_COMMAND_TOKEN_WORDS_POS 0
//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_0_COMMAND_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_CMMD_DEBUG_1 Register EIPE_CRYPTO_CMMD_DEBUG_1 - EIPE CRYPTO CMMD DEBUG 1
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_CMMD_DEBUG_1 0x134
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CRYPTO_CMMD_DEBUG_1 0x7D34u

//! Register Reset Value
#define EIPE_CRYPTO_CMMD_DEBUG_1_RST 0x00000000u

//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_1_COMMAND_TOKEN_WORDS_POS 0
//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_1_COMMAND_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_CMMD_DEBUG_2 Register EIPE_CRYPTO_CMMD_DEBUG_2 - EIPE CRYPTO CMMD DEBUG 2
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_CMMD_DEBUG_2 0x138
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CRYPTO_CMMD_DEBUG_2 0x7D38u

//! Register Reset Value
#define EIPE_CRYPTO_CMMD_DEBUG_2_RST 0x00000000u

//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_2_COMMAND_TOKEN_WORDS_POS 0
//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_2_COMMAND_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_CMMD_DEBUG_3 Register EIPE_CRYPTO_CMMD_DEBUG_3 - EIPE CRYPTO CMMD DEBUG 3
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_CMMD_DEBUG_3 0x13C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_CRYPTO_CMMD_DEBUG_3 0x7D3Cu

//! Register Reset Value
#define EIPE_CRYPTO_CMMD_DEBUG_3_RST 0x00000000u

//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_3_COMMAND_TOKEN_WORDS_POS 0
//! Field COMMAND_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_CMMD_DEBUG_3_COMMAND_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_0 Register EIPE_DBG_SA_MATCHES1_0 - EIPE DBG SA MATCHES1 0
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_0 0x200
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_0 0x7E00u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_0_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_0_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_0_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_0_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_0 Register EIPE_DBG_SA_MATCHES2_0 - EIPE DBG SA MATCHES2 0
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_0 0x204
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_0 0x7E04u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_0_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_0_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_0_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_0_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_0_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_0_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_0_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_1 Register EIPE_DBG_SA_MATCHES1_1 - EIPE DBG SA MATCHES1 1
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_1 0x208
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_1 0x7E08u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_1_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_1_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_1_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_1_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_1 Register EIPE_DBG_SA_MATCHES2_1 - EIPE DBG SA MATCHES2 1
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_1 0x20C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_1 0x7E0Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_1_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_1_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_1_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_1_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_1_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_1_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_1_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_2 Register EIPE_DBG_SA_MATCHES1_2 - EIPE DBG SA MATCHES1 2
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_2 0x210
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_2 0x7E10u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_2_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_2_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_2_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_2_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_2 Register EIPE_DBG_SA_MATCHES2_2 - EIPE DBG SA MATCHES2 2
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_2 0x214
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_2 0x7E14u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_2_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_2_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_2_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_2_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_2_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_2_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_2_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_3 Register EIPE_DBG_SA_MATCHES1_3 - EIPE DBG SA MATCHES1 3
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_3 0x218
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_3 0x7E18u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_3_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_3_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_3_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_3_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_3 Register EIPE_DBG_SA_MATCHES2_3 - EIPE DBG SA MATCHES2 3
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_3 0x21C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_3 0x7E1Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_3_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_3_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_3_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_3_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_3_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_3_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_3_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_4 Register EIPE_DBG_SA_MATCHES1_4 - EIPE DBG SA MATCHES1 4
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_4 0x220
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_4 0x7E20u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_4_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_4_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_4_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_4_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_4 Register EIPE_DBG_SA_MATCHES2_4 - EIPE DBG SA MATCHES2 4
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_4 0x224
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_4 0x7E24u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_4_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_4_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_4_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_4_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_4_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_4_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_4_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_5 Register EIPE_DBG_SA_MATCHES1_5 - EIPE DBG SA MATCHES1 5
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_5 0x228
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_5 0x7E28u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_5_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_5_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_5_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_5_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_5 Register EIPE_DBG_SA_MATCHES2_5 - EIPE DBG SA MATCHES2 5
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_5 0x22C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_5 0x7E2Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_5_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_5_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_5_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_5_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_5_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_5_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_5_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_6 Register EIPE_DBG_SA_MATCHES1_6 - EIPE DBG SA MATCHES1 6
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_6 0x230
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_6 0x7E30u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_6_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_6_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_6_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_6_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_6 Register EIPE_DBG_SA_MATCHES2_6 - EIPE DBG SA MATCHES2 6
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_6 0x234
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_6 0x7E34u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_6_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_6_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_6_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_6_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_6_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_6_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_6_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_7 Register EIPE_DBG_SA_MATCHES1_7 - EIPE DBG SA MATCHES1 7
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_7 0x238
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_7 0x7E38u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_7_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_7_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_7_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_7_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_7 Register EIPE_DBG_SA_MATCHES2_7 - EIPE DBG SA MATCHES2 7
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_7 0x23C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_7 0x7E3Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_7_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_7_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_7_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_7_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_7_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_7_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_7_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_8 Register EIPE_DBG_SA_MATCHES1_8 - EIPE DBG SA MATCHES1 8
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_8 0x240
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_8 0x7E40u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_8_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_8_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_8_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_8_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_8 Register EIPE_DBG_SA_MATCHES2_8 - EIPE DBG SA MATCHES2 8
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_8 0x244
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_8 0x7E44u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_8_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_8_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_8_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_8_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_8_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_8_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_8_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_9 Register EIPE_DBG_SA_MATCHES1_9 - EIPE DBG SA MATCHES1 9
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_9 0x248
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_9 0x7E48u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_9_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_9_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_9_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_9_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_9 Register EIPE_DBG_SA_MATCHES2_9 - EIPE DBG SA MATCHES2 9
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_9 0x24C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_9 0x7E4Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_9_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_9_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_9_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_9_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_9_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_9_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_9_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_10 Register EIPE_DBG_SA_MATCHES1_10 - EIPE DBG SA MATCHES1 10
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_10 0x250
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_10 0x7E50u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_10_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_10_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_10_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_10_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_10 Register EIPE_DBG_SA_MATCHES2_10 - EIPE DBG SA MATCHES2 10
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_10 0x254
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_10 0x7E54u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_10_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_10_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_10_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_10_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_10_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_10_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_10_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_11 Register EIPE_DBG_SA_MATCHES1_11 - EIPE DBG SA MATCHES1 11
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_11 0x258
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_11 0x7E58u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_11_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_11_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_11_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_11_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_11 Register EIPE_DBG_SA_MATCHES2_11 - EIPE DBG SA MATCHES2 11
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_11 0x25C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_11 0x7E5Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_11_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_11_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_11_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_11_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_11_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_11_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_11_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_12 Register EIPE_DBG_SA_MATCHES1_12 - EIPE DBG SA MATCHES1 12
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_12 0x260
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_12 0x7E60u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_12_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_12_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_12_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_12_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_12 Register EIPE_DBG_SA_MATCHES2_12 - EIPE DBG SA MATCHES2 12
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_12 0x264
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_12 0x7E64u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_12_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_12_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_12_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_12_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_12_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_12_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_12_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_13 Register EIPE_DBG_SA_MATCHES1_13 - EIPE DBG SA MATCHES1 13
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_13 0x268
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_13 0x7E68u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_13_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_13_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_13_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_13_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_13 Register EIPE_DBG_SA_MATCHES2_13 - EIPE DBG SA MATCHES2 13
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_13 0x26C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_13 0x7E6Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_13_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_13_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_13_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_13_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_13_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_13_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_13_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_14 Register EIPE_DBG_SA_MATCHES1_14 - EIPE DBG SA MATCHES1 14
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_14 0x270
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_14 0x7E70u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_14_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_14_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_14_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_14_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_14 Register EIPE_DBG_SA_MATCHES2_14 - EIPE DBG SA MATCHES2 14
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_14 0x274
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_14 0x7E74u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_14_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_14_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_14_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_14_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_14_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_14_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_14_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES1_15 Register EIPE_DBG_SA_MATCHES1_15 - EIPE DBG SA MATCHES1 15
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES1_15 0x278
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES1_15 0x7E78u

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES1_15_RST 0x00000000u

//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_0_MATCH_POS 0
//! Field MAC_SA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_0_MATCH_MASK 0x1u

//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_1_MATCH_POS 1
//! Field MAC_SA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_1_MATCH_MASK 0x2u

//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_2_MATCH_POS 2
//! Field MAC_SA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_2_MATCH_MASK 0x4u

//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_3_MATCH_POS 3
//! Field MAC_SA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_3_MATCH_MASK 0x8u

//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_4_MATCH_POS 4
//! Field MAC_SA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_4_MATCH_MASK 0x10u

//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_5_MATCH_POS 5
//! Field MAC_SA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_SA_5_MATCH_MASK 0x20u

//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_0_MATCH_POS 6
//! Field MAC_DA_0_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_0_MATCH_MASK 0x40u

//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_1_MATCH_POS 7
//! Field MAC_DA_1_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_1_MATCH_MASK 0x80u

//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_2_MATCH_POS 8
//! Field MAC_DA_2_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_2_MATCH_MASK 0x100u

//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_3_MATCH_POS 9
//! Field MAC_DA_3_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_3_MATCH_MASK 0x200u

//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_4_MATCH_POS 10
//! Field MAC_DA_4_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_4_MATCH_MASK 0x400u

//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_5_MATCH_POS 11
//! Field MAC_DA_5_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_DA_5_MATCH_MASK 0x800u

//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_ETYPE_MATCH_POS 12
//! Field MAC_ETYPE_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MAC_ETYPE_MATCH_MASK 0x1000u

//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_VLD_MATCH_POS 13
//! Field VLAN_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_VLD_MATCH_MASK 0x2000u

//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_QINQ_FOUND_MATCH_POS 14
//! Field QINQ_FOUND_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_QINQ_FOUND_MATCH_MASK 0x4000u

//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_STAG_VLD_MATCH_POS 15
//! Field STAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_STAG_VLD_MATCH_MASK 0x8000u

//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_QTAG_VLD_MATCH_POS 16
//! Field QTAG_VLD_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_QTAG_VLD_MATCH_MASK 0x10000u

//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_UP_MATCH_POS 17
//! Field VLAN_UP_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_UP_MATCH_MASK 0x20000u

//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_ID_MATCH_POS 18
//! Field VLAN_ID_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_VLAN_ID_MATCH_MASK 0x40000u

//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_SOURCE_PORT_MATCH_POS 19
//! Field SOURCE_PORT_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_SOURCE_PORT_MATCH_MASK 0x80000u

//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_CTRL_PACKET_MATCH_POS 20
//! Field CTRL_PACKET_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_CTRL_PACKET_MATCH_MASK 0x100000u

//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MACSEC_TAG_MATCH_POS 21
//! Field MACSEC_TAG_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_MACSEC_TAG_MATCH_MASK 0x200000u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_15_RESERVED_0_POS 22
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES1_15_RESERVED_0_MASK 0xC00000u

//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_TCI_AN_MATCH_POS 24
//! Field TCI_AN_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES1_15_TCI_AN_MATCH_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_DBG_SA_MATCHES2_15 Register EIPE_DBG_SA_MATCHES2_15 - EIPE DBG SA MATCHES2 15
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCHES2_15 0x27C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCHES2_15 0x7E7Cu

//! Register Reset Value
#define EIPE_DBG_SA_MATCHES2_15_RST 0x00000000u

//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_15_VLAN_UP_INNER_MATCH_POS 0
//! Field VLAN_UP_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_15_VLAN_UP_INNER_MATCH_MASK 0x1u

//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_15_VLAN_ID_INNER_MATCH_POS 1
//! Field VLAN_ID_INNER_MATCH - No Content.
#define EIPE_DBG_SA_MATCHES2_15_VLAN_ID_INNER_MATCH_MASK 0x2u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_15_RESERVED_0_POS 2
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCHES2_15_RESERVED_0_MASK 0xFFFFFFFCu

//! @}

//! \defgroup EIPE_DBG_SA_MATCH_SEL Register EIPE_DBG_SA_MATCH_SEL - EIPE DBG SA MATCH SEL
//! @{

//! Register Offset (relative)
#define EIPE_DBG_SA_MATCH_SEL 0x3FC
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_DEBUG
#define EGR_REG_CLF_DBG_SA_MATCH_SEL 0x7FFCu

//! Register Reset Value
#define EIPE_DBG_SA_MATCH_SEL_RST 0x00000000u

//! Field SAM_SET_16_SEL - No Content.
#define EIPE_DBG_SA_MATCH_SEL_SAM_SET_16_SEL_POS 0
//! Field SAM_SET_16_SEL - No Content.
#define EIPE_DBG_SA_MATCH_SEL_SAM_SET_16_SEL_MASK 0x1u

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCH_SEL_RESERVED_0_POS 1
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_SA_MATCH_SEL_RESERVED_0_MASK 0xFFFFFFFEu

//! @}

//! @}

#endif

