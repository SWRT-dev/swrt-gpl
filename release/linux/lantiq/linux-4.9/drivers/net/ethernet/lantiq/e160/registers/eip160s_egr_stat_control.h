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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_Stat_control_def.xml
// Register File Name  : EGR_STAT_CONTROL
// Register File Title : EIPE EIP_160s_e_32_Stat_control
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_STAT_CONTROL_H
#define _EGR_STAT_CONTROL_H

//! \defgroup EGR_STAT_CONTROL Register File EGR_STAT_CONTROL - EIPE EIP_160s_e_32_Stat_control
//! @{

//! Base Address of EGR_STAT_CONTROL
#define EGR_STAT_CONTROL_MODULE_BASE 0xC800u

//! \defgroup EIPE_COUNT_DEBUG1 Register EIPE_COUNT_DEBUG1 - EIPE COUNT DEBUG1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_DEBUG1 0x0
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_DEBUG1 0xC800u

//! Register Reset Value
#define EIPE_COUNT_DEBUG1_RST 0x00000000u

//! Field SA_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG1_SA_INCREMENT_MASK_POS 0
//! Field SA_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG1_SA_INCREMENT_MASK_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG1_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG1_RESERVED_0_MASK 0xFFF0u

//! Field SA_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG1_SA_OCTET_INCREMENT_POS 16
//! Field SA_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG1_SA_OCTET_INCREMENT_MASK 0x7FFF0000u

//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG1_RESERVED_1_POS 31
//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG1_RESERVED_1_MASK 0x80000000u

//! @}

//! \defgroup EIPE_COUNT_DEBUG2 Register EIPE_COUNT_DEBUG2 - EIPE COUNT DEBUG2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_DEBUG2 0x4
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_DEBUG2 0xC804u

//! Register Reset Value
#define EIPE_COUNT_DEBUG2_RST 0x00000000u

//! Field VLAN_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG2_VLAN_INCREMENT_MASK_POS 0
//! Field VLAN_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG2_VLAN_INCREMENT_MASK_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG2_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG2_RESERVED_0_MASK 0xFFE0u

//! Field VLAN_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG2_VLAN_OCTET_INCREMENT_POS 16
//! Field VLAN_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG2_VLAN_OCTET_INCREMENT_MASK 0x7FFF0000u

//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG2_RESERVED_1_POS 31
//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG2_RESERVED_1_MASK 0x80000000u

//! @}

//! \defgroup EIPE_COUNT_DEBUG3 Register EIPE_COUNT_DEBUG3 - EIPE COUNT DEBUG3
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_DEBUG3 0x8
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_DEBUG3 0xC808u

//! Register Reset Value
#define EIPE_COUNT_DEBUG3_RST 0x00000000u

//! Field GLOBAL_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG3_GLOBAL_INCREMENT_MASK_POS 0
//! Field GLOBAL_INCREMENT_MASK - No Content.
#define EIPE_COUNT_DEBUG3_GLOBAL_INCREMENT_MASK_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG3_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG3_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_COUNT_DEBUG4 Register EIPE_COUNT_DEBUG4 - EIPE COUNT DEBUG4
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_DEBUG4 0xC
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_DEBUG4 0xC80Cu

//! Register Reset Value
#define EIPE_COUNT_DEBUG4_RST 0x00000000u

//! Field SA_TEST - No Content.
#define EIPE_COUNT_DEBUG4_SA_TEST_POS 0
//! Field SA_TEST - No Content.
#define EIPE_COUNT_DEBUG4_SA_TEST_MASK 0x1u

//! Field VLAN_TEST - No Content.
#define EIPE_COUNT_DEBUG4_VLAN_TEST_POS 1
//! Field VLAN_TEST - No Content.
#define EIPE_COUNT_DEBUG4_VLAN_TEST_MASK 0x2u

//! Field GLOBAL_TEST - No Content.
#define EIPE_COUNT_DEBUG4_GLOBAL_TEST_POS 2
//! Field GLOBAL_TEST - No Content.
#define EIPE_COUNT_DEBUG4_GLOBAL_TEST_MASK 0x4u

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_0_POS 3
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_0_MASK 0x8u

//! Field SA_NR - No Content.
#define EIPE_COUNT_DEBUG4_SA_NR_POS 4
//! Field SA_NR - No Content.
#define EIPE_COUNT_DEBUG4_SA_NR_MASK 0x1F0u

//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_1_POS 9
//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_1_MASK 0xE00u

//! Field VLAN_UP - No Content.
#define EIPE_COUNT_DEBUG4_VLAN_UP_POS 12
//! Field VLAN_UP - No Content.
#define EIPE_COUNT_DEBUG4_VLAN_UP_MASK 0x7000u

//! Field RESERVED_2 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_2_POS 15
//! Field RESERVED_2 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_2_MASK 0x8000u

//! Field GLOBAL_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG4_GLOBAL_OCTET_INCREMENT_POS 16
//! Field GLOBAL_OCTET_INCREMENT - No Content.
#define EIPE_COUNT_DEBUG4_GLOBAL_OCTET_INCREMENT_MASK 0x7FFF0000u

//! Field RESERVED_3 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_3_POS 31
//! Field RESERVED_3 - No Content.
#define EIPE_COUNT_DEBUG4_RESERVED_3_MASK 0x80000000u

//! @}

//! \defgroup EIPE_COUNT_CONTROL Register EIPE_COUNT_CONTROL - EIPE COUNT CONTROL
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_CONTROL 0x10
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_CONTROL 0xC810u

//! Register Reset Value
#define EIPE_COUNT_CONTROL_RST 0x00000000u

//! Field RESET_ALL - No Content.
#define EIPE_COUNT_CONTROL_RESET_ALL_POS 0
//! Field RESET_ALL - No Content.
#define EIPE_COUNT_CONTROL_RESET_ALL_MASK 0x1u

//! Field DEBUG_ACCESS - No Content.
#define EIPE_COUNT_CONTROL_DEBUG_ACCESS_POS 1
//! Field DEBUG_ACCESS - No Content.
#define EIPE_COUNT_CONTROL_DEBUG_ACCESS_MASK 0x2u

//! Field SATURATE_CNTRS - No Content.
#define EIPE_COUNT_CONTROL_SATURATE_CNTRS_POS 2
//! Field SATURATE_CNTRS - No Content.
#define EIPE_COUNT_CONTROL_SATURATE_CNTRS_MASK 0x4u

//! Field AUTO_CNTR_RESET - No Content.
#define EIPE_COUNT_CONTROL_AUTO_CNTR_RESET_POS 3
//! Field AUTO_CNTR_RESET - No Content.
#define EIPE_COUNT_CONTROL_AUTO_CNTR_RESET_MASK 0x8u

//! Field RESET_SUMMARY - No Content.
#define EIPE_COUNT_CONTROL_RESET_SUMMARY_POS 4
//! Field RESET_SUMMARY - No Content.
#define EIPE_COUNT_CONTROL_RESET_SUMMARY_MASK 0x10u

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_CONTROL_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_CONTROL_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_COUNT_INCEN1 Register EIPE_COUNT_INCEN1 - EIPE COUNT INCEN1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_INCEN1 0x14
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_INCEN1 0xC814u

//! Register Reset Value
#define EIPE_COUNT_INCEN1_RST 0x001F000Fu

//! Field SA_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN1_SA_INCREMENT_ENABLE_POS 0
//! Field SA_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN1_SA_INCREMENT_ENABLE_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_INCEN1_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_INCEN1_RESERVED_0_MASK 0xFFF0u

//! Field VLAN_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN1_VLAN_INCREMENT_ENABLE_POS 16
//! Field VLAN_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN1_VLAN_INCREMENT_ENABLE_MASK 0x1F0000u

//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_INCEN1_RESERVED_1_POS 21
//! Field RESERVED_1 - No Content.
#define EIPE_COUNT_INCEN1_RESERVED_1_MASK 0xFFE00000u

//! @}

//! \defgroup EIPE_COUNT_INCEN2 Register EIPE_COUNT_INCEN2 - EIPE COUNT INCEN2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_INCEN2 0x18
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_INCEN2 0xC818u

//! Register Reset Value
#define EIPE_COUNT_INCEN2_RST 0x0000001Fu

//! Field GLOBAL_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN2_GLOBAL_INCREMENT_ENABLE_POS 0
//! Field GLOBAL_INCREMENT_ENABLE - No Content.
#define EIPE_COUNT_INCEN2_GLOBAL_INCREMENT_ENABLE_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_INCEN2_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_INCEN2_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_COUNT_FRAME_THR1 Register EIPE_COUNT_FRAME_THR1 - EIPE COUNT FRAME THR1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_FRAME_THR1 0x20
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_FRAME_THR1 0xC820u

//! Register Reset Value
#define EIPE_COUNT_FRAME_THR1_RST 0x00000000u

//! Field COUNT_FRAME_THR_31_0 - No Content.
#define EIPE_COUNT_FRAME_THR1_COUNT_FRAME_THR_31_0_POS 0
//! Field COUNT_FRAME_THR_31_0 - No Content.
#define EIPE_COUNT_FRAME_THR1_COUNT_FRAME_THR_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_COUNT_FRAME_THR2 Register EIPE_COUNT_FRAME_THR2 - EIPE COUNT FRAME THR2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_FRAME_THR2 0x24
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_FRAME_THR2 0xC824u

//! Register Reset Value
#define EIPE_COUNT_FRAME_THR2_RST 0x00000000u

//! Field COUNT_FRAME_THR_63_32 - No Content.
#define EIPE_COUNT_FRAME_THR2_COUNT_FRAME_THR_63_32_POS 0
//! Field COUNT_FRAME_THR_63_32 - No Content.
#define EIPE_COUNT_FRAME_THR2_COUNT_FRAME_THR_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_COUNT_OCTET_THR1 Register EIPE_COUNT_OCTET_THR1 - EIPE COUNT OCTET THR1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_OCTET_THR1 0x30
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_OCTET_THR1 0xC830u

//! Register Reset Value
#define EIPE_COUNT_OCTET_THR1_RST 0x00000000u

//! Field COUNT_OCTET_THR_31_0 - No Content.
#define EIPE_COUNT_OCTET_THR1_COUNT_OCTET_THR_31_0_POS 0
//! Field COUNT_OCTET_THR_31_0 - No Content.
#define EIPE_COUNT_OCTET_THR1_COUNT_OCTET_THR_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_COUNT_OCTET_THR2 Register EIPE_COUNT_OCTET_THR2 - EIPE COUNT OCTET THR2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_OCTET_THR2 0x34
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_OCTET_THR2 0xC834u

//! Register Reset Value
#define EIPE_COUNT_OCTET_THR2_RST 0x00000000u

//! Field COUNT_OCTET_THR_63_32 - No Content.
#define EIPE_COUNT_OCTET_THR2_COUNT_OCTET_THR_63_32_POS 0
//! Field COUNT_OCTET_THR_63_32 - No Content.
#define EIPE_COUNT_OCTET_THR2_COUNT_OCTET_THR_63_32_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_GL Register EIPE_COUNT_SUMMARY_GL - EIPE COUNT SUMMARY GL
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_GL 0x40
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_GL 0xC840u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_GL_RST 0x00000000u

//! Field GLOBAL_CNT - No Content.
#define EIPE_COUNT_SUMMARY_GL_GLOBAL_CNT_POS 0
//! Field GLOBAL_CNT - No Content.
#define EIPE_COUNT_SUMMARY_GL_GLOBAL_CNT_MASK 0x1Fu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_GL_RESERVED_0_POS 5
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_GL_RESERVED_0_MASK 0xFFFFFFE0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_VL1 Register EIPE_COUNT_SUMMARY_VL1 - EIPE COUNT SUMMARY VL1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_VL1 0x44
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_VL1 0xC844u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_VL1_RST 0x00000000u

//! Field VLAN_CNT0 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT0_POS 0
//! Field VLAN_CNT0 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT0_MASK 0xFFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_RESERVED_0_POS 8
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_RESERVED_0_MASK 0xFF00u

//! Field VLAN_CNT2 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT2_POS 16
//! Field VLAN_CNT2 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT2_MASK 0xFF0000u

//! Field VLAN_CNT3 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT3_POS 24
//! Field VLAN_CNT3 - No Content.
#define EIPE_COUNT_SUMMARY_VL1_VLAN_CNT3_MASK 0xFF000000u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_VL2 Register EIPE_COUNT_SUMMARY_VL2 - EIPE COUNT SUMMARY VL2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_VL2 0x48
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_VL2 0xC848u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_VL2_RST 0x00000000u

//! Field VLAN_CNT4 - No Content.
#define EIPE_COUNT_SUMMARY_VL2_VLAN_CNT4_POS 0
//! Field VLAN_CNT4 - No Content.
#define EIPE_COUNT_SUMMARY_VL2_VLAN_CNT4_MASK 0xFFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_VL2_RESERVED_0_POS 8
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_VL2_RESERVED_0_MASK 0xFFFFFF00u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_PSA1 Register EIPE_COUNT_SUMMARY_PSA1 - EIPE COUNT SUMMARY PSA1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_PSA1 0x50
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_PSA1 0xC850u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_PSA1_RST 0x00000000u

//! Field SA_31_0 - No Content.
#define EIPE_COUNT_SUMMARY_PSA1_SA_31_0_POS 0
//! Field SA_31_0 - No Content.
#define EIPE_COUNT_SUMMARY_PSA1_SA_31_0_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA0 Register EIPE_COUNT_SUMMARY_SA0 - EIPE COUNT SUMMARY SA0
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA0 0x100
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA0 0xC900u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA0_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA0_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA0_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA0_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA0_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA1 Register EIPE_COUNT_SUMMARY_SA1 - EIPE COUNT SUMMARY SA1
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA1 0x104
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA1 0xC904u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA1_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA1_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA1_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA1_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA1_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA2 Register EIPE_COUNT_SUMMARY_SA2 - EIPE COUNT SUMMARY SA2
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA2 0x108
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA2 0xC908u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA2_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA2_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA2_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA2_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA2_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA3 Register EIPE_COUNT_SUMMARY_SA3 - EIPE COUNT SUMMARY SA3
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA3 0x10C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA3 0xC90Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA3_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA3_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA3_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA3_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA3_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA4 Register EIPE_COUNT_SUMMARY_SA4 - EIPE COUNT SUMMARY SA4
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA4 0x110
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA4 0xC910u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA4_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA4_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA4_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA4_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA4_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA5 Register EIPE_COUNT_SUMMARY_SA5 - EIPE COUNT SUMMARY SA5
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA5 0x114
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA5 0xC914u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA5_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA5_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA5_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA5_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA5_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA6 Register EIPE_COUNT_SUMMARY_SA6 - EIPE COUNT SUMMARY SA6
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA6 0x118
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA6 0xC918u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA6_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA6_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA6_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA6_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA6_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA7 Register EIPE_COUNT_SUMMARY_SA7 - EIPE COUNT SUMMARY SA7
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA7 0x11C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA7 0xC91Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA7_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA7_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA7_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA7_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA7_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA8 Register EIPE_COUNT_SUMMARY_SA8 - EIPE COUNT SUMMARY SA8
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA8 0x120
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA8 0xC920u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA8_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA8_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA8_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA8_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA8_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA9 Register EIPE_COUNT_SUMMARY_SA9 - EIPE COUNT SUMMARY SA9
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA9 0x124
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA9 0xC924u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA9_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA9_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA9_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA9_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA9_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA10 Register EIPE_COUNT_SUMMARY_SA10 - EIPE COUNT SUMMARY SA10
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA10 0x128
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA10 0xC928u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA10_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA10_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA10_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA10_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA10_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA11 Register EIPE_COUNT_SUMMARY_SA11 - EIPE COUNT SUMMARY SA11
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA11 0x12C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA11 0xC92Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA11_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA11_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA11_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA11_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA11_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA12 Register EIPE_COUNT_SUMMARY_SA12 - EIPE COUNT SUMMARY SA12
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA12 0x130
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA12 0xC930u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA12_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA12_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA12_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA12_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA12_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA13 Register EIPE_COUNT_SUMMARY_SA13 - EIPE COUNT SUMMARY SA13
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA13 0x134
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA13 0xC934u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA13_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA13_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA13_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA13_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA13_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA14 Register EIPE_COUNT_SUMMARY_SA14 - EIPE COUNT SUMMARY SA14
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA14 0x138
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA14 0xC938u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA14_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA14_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA14_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA14_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA14_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA15 Register EIPE_COUNT_SUMMARY_SA15 - EIPE COUNT SUMMARY SA15
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA15 0x13C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA15 0xC93Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA15_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA15_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA15_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA15_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA15_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA16 Register EIPE_COUNT_SUMMARY_SA16 - EIPE COUNT SUMMARY SA16
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA16 0x140
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA16 0xC940u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA16_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA16_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA16_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA16_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA16_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA17 Register EIPE_COUNT_SUMMARY_SA17 - EIPE COUNT SUMMARY SA17
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA17 0x144
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA17 0xC944u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA17_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA17_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA17_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA17_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA17_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA18 Register EIPE_COUNT_SUMMARY_SA18 - EIPE COUNT SUMMARY SA18
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA18 0x148
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA18 0xC948u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA18_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA18_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA18_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA18_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA18_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA19 Register EIPE_COUNT_SUMMARY_SA19 - EIPE COUNT SUMMARY SA19
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA19 0x14C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA19 0xC94Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA19_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA19_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA19_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA19_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA19_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA20 Register EIPE_COUNT_SUMMARY_SA20 - EIPE COUNT SUMMARY SA20
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA20 0x150
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA20 0xC950u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA20_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA20_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA20_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA20_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA20_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA21 Register EIPE_COUNT_SUMMARY_SA21 - EIPE COUNT SUMMARY SA21
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA21 0x154
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA21 0xC954u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA21_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA21_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA21_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA21_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA21_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA22 Register EIPE_COUNT_SUMMARY_SA22 - EIPE COUNT SUMMARY SA22
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA22 0x158
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA22 0xC958u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA22_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA22_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA22_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA22_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA22_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA23 Register EIPE_COUNT_SUMMARY_SA23 - EIPE COUNT SUMMARY SA23
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA23 0x15C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA23 0xC95Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA23_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA23_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA23_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA23_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA23_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA24 Register EIPE_COUNT_SUMMARY_SA24 - EIPE COUNT SUMMARY SA24
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA24 0x160
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA24 0xC960u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA24_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA24_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA24_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA24_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA24_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA25 Register EIPE_COUNT_SUMMARY_SA25 - EIPE COUNT SUMMARY SA25
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA25 0x164
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA25 0xC964u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA25_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA25_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA25_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA25_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA25_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA26 Register EIPE_COUNT_SUMMARY_SA26 - EIPE COUNT SUMMARY SA26
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA26 0x168
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA26 0xC968u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA26_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA26_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA26_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA26_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA26_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA27 Register EIPE_COUNT_SUMMARY_SA27 - EIPE COUNT SUMMARY SA27
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA27 0x16C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA27 0xC96Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA27_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA27_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA27_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA27_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA27_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA28 Register EIPE_COUNT_SUMMARY_SA28 - EIPE COUNT SUMMARY SA28
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA28 0x170
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA28 0xC970u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA28_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA28_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA28_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA28_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA28_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA29 Register EIPE_COUNT_SUMMARY_SA29 - EIPE COUNT SUMMARY SA29
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA29 0x174
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA29 0xC974u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA29_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA29_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA29_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA29_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA29_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA30 Register EIPE_COUNT_SUMMARY_SA30 - EIPE COUNT SUMMARY SA30
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA30 0x178
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA30 0xC978u

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA30_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA30_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA30_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA30_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA30_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! \defgroup EIPE_COUNT_SUMMARY_SA31 Register EIPE_COUNT_SUMMARY_SA31 - EIPE COUNT SUMMARY SA31
//! @{

//! Register Offset (relative)
#define EIPE_COUNT_SUMMARY_SA31 0x17C
//! Register Offset (absolute) for 1st Instance EGR_STAT_CONTROL
#define EGR_REG_STAT_CTRL_COUNT_SUMMARY_SA31 0xC97Cu

//! Register Reset Value
#define EIPE_COUNT_SUMMARY_SA31_RST 0x00000000u

//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA31_SA_CNT_POS 0
//! Field SA_CNT - No Content.
#define EIPE_COUNT_SUMMARY_SA31_SA_CNT_MASK 0xFu

//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA31_RESERVED_0_POS 4
//! Field RESERVED_0 - No Content.
#define EIPE_COUNT_SUMMARY_SA31_RESERVED_0_MASK 0xFFFFFFF0u

//! @}

//! @}

#endif
