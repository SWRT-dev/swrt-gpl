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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_OPPE_def.xml
// Register File Name  : EGR_OPPE
// Register File Title : EIPE EIP_160s_e_32_OPPE
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_OPPE_H
#define _EGR_OPPE_H

//! \defgroup EGR_OPPE Register File EGR_OPPE - EIPE EIP_160s_e_32_OPPE
//! @{

//! Base Address of EGR_OPPE
#define EGR_OPPE_MODULE_BASE 0xF000u

//! \defgroup EIPE_SA_PN_THR_SUMMARY1 Register EIPE_SA_PN_THR_SUMMARY1 - EIPE SA PN THR SUMMARY1
//! @{

//! Register Offset (relative)
#define EIPE_SA_PN_THR_SUMMARY1 0x0
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_SA_PN_THR_SUMMARY1 0xF000u

//! Register Reset Value
#define EIPE_SA_PN_THR_SUMMARY1_RST 0x00000000u

//! Field SA_31_0 - No Content.
#define EIPE_SA_PN_THR_SUMMARY1_SA_31_0_POS 0
//! Field SA_31_0 - No Content.
#define EIPE_SA_PN_THR_SUMMARY1_SA_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_0 Register EIPE_VLAN_MTU_CHECK_0 - EIPE VLAN MTU CHECK 0
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_0 0x100
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_0 0xF100u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_0_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_0_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_0_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_0_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_0_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_0_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_0_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_1 Register EIPE_VLAN_MTU_CHECK_1 - EIPE VLAN MTU CHECK 1
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_1 0x104
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_1 0xF104u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_1_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_1_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_1_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_1_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_1_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_1_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_1_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_2 Register EIPE_VLAN_MTU_CHECK_2 - EIPE VLAN MTU CHECK 2
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_2 0x108
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_2 0xF108u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_2_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_2_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_2_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_2_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_2_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_2_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_2_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_3 Register EIPE_VLAN_MTU_CHECK_3 - EIPE VLAN MTU CHECK 3
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_3 0x10C
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_3 0xF10Cu

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_3_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_3_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_3_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_3_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_3_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_3_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_3_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_4 Register EIPE_VLAN_MTU_CHECK_4 - EIPE VLAN MTU CHECK 4
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_4 0x110
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_4 0xF110u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_4_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_4_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_4_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_4_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_4_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_4_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_4_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_5 Register EIPE_VLAN_MTU_CHECK_5 - EIPE VLAN MTU CHECK 5
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_5 0x114
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_5 0xF114u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_5_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_5_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_5_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_5_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_5_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_5_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_5_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_6 Register EIPE_VLAN_MTU_CHECK_6 - EIPE VLAN MTU CHECK 6
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_6 0x118
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_6 0xF118u

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_6_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_6_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_6_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_6_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_6_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_6_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_6_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_VLAN_MTU_CHECK_7 Register EIPE_VLAN_MTU_CHECK_7 - EIPE VLAN MTU CHECK 7
//! @{

//! Register Offset (relative)
#define EIPE_VLAN_MTU_CHECK_7 0x11C
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_VLAN_MTU_CHECK_7 0xF11Cu

//! Register Reset Value
#define EIPE_VLAN_MTU_CHECK_7_RST 0x000005EEu

//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_7_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_VLAN_MTU_CHECK_7_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_7_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_VLAN_MTU_CHECK_7_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_7_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_VLAN_MTU_CHECK_7_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_NON_VLAN_MTU_CHECK Register EIPE_NON_VLAN_MTU_CHECK - EIPE NON VLAN MTU CHECK
//! @{

//! Register Offset (relative)
#define EIPE_NON_VLAN_MTU_CHECK 0x120
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_NON_VLAN_MTU_CHECK 0xF120u

//! Register Reset Value
#define EIPE_NON_VLAN_MTU_CHECK_RST 0x000005EAu

//! Field MTU_COMPARE - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_MTU_COMPARE_POS 0
//! Field MTU_COMPARE - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_MTU_COMPARE_MASK 0x7FFFu

//! Field MTU_COMP_DROP - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_MTU_COMP_DROP_POS 15
//! Field MTU_COMP_DROP - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_MTU_COMP_DROP_MASK 0x8000u

//! Field RESERVED_0 - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_NON_VLAN_MTU_CHECK_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_COUNT_SECFAIL1 Register EIPE_COUNT_SECFAIL1 - EIPE COUNT SECFAIL1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SECFAIL1 0x124
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_COUNT_SECFAIL1 0xF124u

//! Register Reset Value
#define EIPE_COUNT_SECFAIL1_RST 0x80FE0000u

//! Field SA_SECFAIL_MASK - No Content.
#define EIPE_COUNT_SECFAIL1_SA_SECFAIL_MASK_POS 0
//! Field SA_SECFAIL_MASK - No Content.
#define EIPE_COUNT_SECFAIL1_SA_SECFAIL_MASK_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SECFAIL1_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SECFAIL1_RESERVED_0_MASK 0x7FF0u

//! Field VLAN_CNTR_UPDATE - No Content.
#define EIPE_COUNT_SECFAIL1_VLAN_CNTR_UPDATE_POS 15
//! Field VLAN_CNTR_UPDATE - No Content.
#define EIPE_COUNT_SECFAIL1_VLAN_CNTR_UPDATE_MASK 0x8000u

//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_SECFAIL1_RESERVED_1_POS 16
//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_SECFAIL1_RESERVED_1_MASK 0x10000u

//! Field ERROR_MASK - No Content.
#define EIPE_COUNT_SECFAIL1_ERROR_MASK_POS 17
//! Field ERROR_MASK - No Content.
#define EIPE_COUNT_SECFAIL1_ERROR_MASK_MASK 0xFFFE0000u

//! @}

//! \defgroup EIPE_COUNT_SECFAIL2 Register EIPE_COUNT_SECFAIL2 - EIPE COUNT SECFAIL2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SECFAIL2 0x128
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_COUNT_SECFAIL2 0xF128u

//! Register Reset Value
#define EIPE_COUNT_SECFAIL2_RST 0x00000000u

//! Field GLOBAL_SECFAIL_MASK - No Content.
#define EIPE_COUNT_SECFAIL2_GLOBAL_SECFAIL_MASK_POS 0
//! Field GLOBAL_SECFAIL_MASK - No Content.
#define EIPE_COUNT_SECFAIL2_GLOBAL_SECFAIL_MASK_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SECFAIL2_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SECFAIL2_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_CRYPTO_RSLT_DEBUG_0 Register EIPE_CRYPTO_RSLT_DEBUG_0 - EIPE CRYPTO RSLT DEBUG 0
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_RSLT_DEBUG_0 0x130
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_CRYPTO_RSLT_DEBUG_0 0xF130u

//! Register Reset Value
#define EIPE_CRYPTO_RSLT_DEBUG_0_RST 0x00000000u

//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_0_RESULT_TOKEN_WORDS_POS 0
//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_0_RESULT_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_RSLT_DEBUG_1 Register EIPE_CRYPTO_RSLT_DEBUG_1 - EIPE CRYPTO RSLT DEBUG 1
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_RSLT_DEBUG_1 0x134
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_CRYPTO_RSLT_DEBUG_1 0xF134u

//! Register Reset Value
#define EIPE_CRYPTO_RSLT_DEBUG_1_RST 0x00000000u

//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_1_RESULT_TOKEN_WORDS_POS 0
//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_1_RESULT_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_RSLT_DEBUG_2 Register EIPE_CRYPTO_RSLT_DEBUG_2 - EIPE CRYPTO RSLT DEBUG 2
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_RSLT_DEBUG_2 0x138
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_CRYPTO_RSLT_DEBUG_2 0xF138u

//! Register Reset Value
#define EIPE_CRYPTO_RSLT_DEBUG_2_RST 0x00000000u

//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_2_RESULT_TOKEN_WORDS_POS 0
//! Field RESULT_TOKEN_WORDS - No Content.
#define EIPE_CRYPTO_RSLT_DEBUG_2_RESULT_TOKEN_WORDS_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CRYPTO_DEBUG_CNTR Register EIPE_CRYPTO_DEBUG_CNTR - EIPE CRYPTO DEBUG CNTR
//! @{

//! Register Offset (relative)
#define EIPE_CRYPTO_DEBUG_CNTR 0x13C
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_CRYPTO_DEBUG_CNTR 0xF13Cu

//! Register Reset Value
#define EIPE_CRYPTO_DEBUG_CNTR_RST 0x00000000u

//! Field PACKET_COUNT - No Content.
#define EIPE_CRYPTO_DEBUG_CNTR_PACKET_COUNT_POS 0
//! Field PACKET_COUNT - No Content.
#define EIPE_CRYPTO_DEBUG_CNTR_PACKET_COUNT_MASK 0x7FFFFFFFu

//! Field PP_RESULTACTION - No Content.
#define EIPE_CRYPTO_DEBUG_CNTR_PP_RESULTACTION_POS 31
//! Field PP_RESULTACTION - No Content.
#define EIPE_CRYPTO_DEBUG_CNTR_PP_RESULTACTION_MASK 0x80000000u

//! @}

//! \defgroup EIPE_DBG_PP_DROP_ENABLE Register EIPE_DBG_PP_DROP_ENABLE - EIPE DBG PP DROP ENABLE
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PP_DROP_ENABLE 0x140
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_DBG_PP_DROP_ENABLE 0xF140u

//! Register Reset Value
#define EIPE_DBG_PP_DROP_ENABLE_RST 0x030F001Fu

//! Field IG_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_IG_DROP_ENABLE_POS 0
//! Field IG_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_IG_DROP_ENABLE_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_0_MASK 0xFFE0u

//! Field EG_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_EG_DROP_ENABLE_POS 16
//! Field EG_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_EG_DROP_ENABLE_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_1_MASK 0xF00000u

//! Field GL_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_GL_DROP_ENABLE_POS 24
//! Field GL_DROP_ENABLE - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_GL_DROP_ENABLE_MASK 0x3000000u

//! Field RESERVED_2 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_2_POS 26
//! Field RESERVED_2 - No Content.
#define EIPE_DBG_PP_DROP_ENABLE_RESERVED_2_MASK 0xFC000000u

//! @}

//! \defgroup EIPE_DBG_PP_DROP_STATUS Register EIPE_DBG_PP_DROP_STATUS - EIPE DBG PP DROP STATUS
//! @{

//! Register Offset (relative)
#define EIPE_DBG_PP_DROP_STATUS 0x144
//! Register Offset (absolute) for 1st Instance EGR_OPPE
#define EGR_REG_OPPE_DBG_PP_DROP_STATUS 0xF144u

//! Register Reset Value
#define EIPE_DBG_PP_DROP_STATUS_RST 0x00000000u

//! Field IG_DROP_FLAGS - No Content.
#define EIPE_DBG_PP_DROP_STATUS_IG_DROP_FLAGS_POS 0
//! Field IG_DROP_FLAGS - No Content.
#define EIPE_DBG_PP_DROP_STATUS_IG_DROP_FLAGS_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_0_MASK 0xFFE0u

//! Field EG_DROP_FLAGS - No Content.
#define EIPE_DBG_PP_DROP_STATUS_EG_DROP_FLAGS_POS 16
//! Field EG_DROP_FLAGS - No Content.
#define EIPE_DBG_PP_DROP_STATUS_EG_DROP_FLAGS_MASK 0xF0000u

//! Field RESERVED_1 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_1_POS 20
//! Field RESERVED_1 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_1_MASK 0xF00000u

//! Field OID_DROP_OPERATION - No Content.
#define EIPE_DBG_PP_DROP_STATUS_OID_DROP_OPERATION_POS 24
//! Field OID_DROP_OPERATION - No Content.
#define EIPE_DBG_PP_DROP_STATUS_OID_DROP_OPERATION_MASK 0x1000000u

//! Field TRANSFORM_ERROR - No Content.
#define EIPE_DBG_PP_DROP_STATUS_TRANSFORM_ERROR_POS 25
//! Field TRANSFORM_ERROR - No Content.
#define EIPE_DBG_PP_DROP_STATUS_TRANSFORM_ERROR_MASK 0x2000000u

//! Field RESERVED_2 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_2_POS 26
//! Field RESERVED_2 - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RESERVED_2_MASK 0x1C000000u

//! Field RES_OUT - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RES_OUT_POS 29
//! Field RES_OUT - No Content.
#define EIPE_DBG_PP_DROP_STATUS_RES_OUT_MASK 0xE0000000u

//! @}

//! @}

#endif
