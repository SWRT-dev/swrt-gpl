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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_System_control_def.xml
// Register File Name  : EGR_SYSTEM_CONTROL
// Register File Title : EIPE EIP_160s_e_32_System_control
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_SYSTEM_CONTROL_H
#define _EGR_SYSTEM_CONTROL_H

//! \defgroup EGR_SYSTEM_CONTROL Register File EGR_SYSTEM_CONTROL - EIPE EIP_160s_e_32_System_control
//! @{

//! Base Address of EGR_SYSTEM_CONTROL
#define EGR_SYSTEM_CONTROL_MODULE_BASE 0xFC00u

//! \defgroup EIPE_EIP160_CONFIG2 Register EIPE_EIP160_CONFIG2 - EIPE EIP160 CONFIG2
//! @{

//! Register Offset (relative)
#define EIPE_EIP160_CONFIG2 0x3F4
//! Register Offset (absolute) for 1st Instance EGR_SYSTEM_CONTROL
#define EGR_REG_SYS_CTRL_EIP160_CONFIG2 0xFFF4u

//! Register Reset Value
#define EIPE_EIP160_CONFIG2_RST 0x0A05C554u

//! Field SA_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_SA_COUNTERS_POS 0
//! Field SA_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_SA_COUNTERS_MASK 0xFu

//! Field VL_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_VL_COUNTERS_POS 4
//! Field VL_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_VL_COUNTERS_MASK 0xF0u

//! Field GLOBAL_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_GLOBAL_COUNTERS_POS 8
//! Field GLOBAL_COUNTERS - No Content.
#define EIPE_EIP160_CONFIG2_GLOBAL_COUNTERS_MASK 0x3F00u

//! Field SATURATE_CNTRS - No Content.
#define EIPE_EIP160_CONFIG2_SATURATE_CNTRS_POS 14
//! Field SATURATE_CNTRS - No Content.
#define EIPE_EIP160_CONFIG2_SATURATE_CNTRS_MASK 0x4000u

//! Field AUTO_CNTR_RESET - No Content.
#define EIPE_EIP160_CONFIG2_AUTO_CNTR_RESET_POS 15
//! Field AUTO_CNTR_RESET - No Content.
#define EIPE_EIP160_CONFIG2_AUTO_CNTR_RESET_MASK 0x8000u

//! Field SA_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_SA_OCTET_CTRS_POS 16
//! Field SA_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_SA_OCTET_CTRS_MASK 0x30000u

//! Field VL_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_VL_OCTET_CTRS_POS 18
//! Field VL_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_VL_OCTET_CTRS_MASK 0xC0000u

//! Field GLOB_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_GLOB_OCTET_CTRS_POS 20
//! Field GLOB_OCTET_CTRS - No Content.
#define EIPE_EIP160_CONFIG2_GLOB_OCTET_CTRS_MASK 0xF00000u

//! Field INGRESS_ONLY - No Content.
#define EIPE_EIP160_CONFIG2_INGRESS_ONLY_POS 24
//! Field INGRESS_ONLY - No Content.
#define EIPE_EIP160_CONFIG2_INGRESS_ONLY_MASK 0x1000000u

//! Field EGRESS_ONLY - No Content.
#define EIPE_EIP160_CONFIG2_EGRESS_ONLY_POS 25
//! Field EGRESS_ONLY - No Content.
#define EIPE_EIP160_CONFIG2_EGRESS_ONLY_MASK 0x2000000u

//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_CONFIG2_RESERVED_0_POS 26
//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_CONFIG2_RESERVED_0_MASK 0x4000000u

//! Field TAG_BYPASS - No Content.
#define EIPE_EIP160_CONFIG2_TAG_BYPASS_POS 27
//! Field TAG_BYPASS - No Content.
#define EIPE_EIP160_CONFIG2_TAG_BYPASS_MASK 0x8000000u

//! Field RESERVED_1 - No Content.
#define EIPE_EIP160_CONFIG2_RESERVED_1_POS 28
//! Field RESERVED_1 - No Content.
#define EIPE_EIP160_CONFIG2_RESERVED_1_MASK 0xF0000000u

//! @}

//! \defgroup EIPE_EIP160_CONFIG Register EIPE_EIP160_CONFIG - EIPE EIP160 CONFIG
//! @{

//! Register Offset (relative)
#define EIPE_EIP160_CONFIG 0x3F8
//! Register Offset (absolute) for 1st Instance EGR_SYSTEM_CONTROL
#define EGR_REG_SYS_CTRL_EIP160_CONFIG 0xFFF8u

//! Register Reset Value
#define EIPE_EIP160_CONFIG_RST 0x00000020u

//! Field NR_OF_SAS - No Content.
#define EIPE_EIP160_CONFIG_NR_OF_SAS_POS 0
//! Field NR_OF_SAS - No Content.
#define EIPE_EIP160_CONFIG_NR_OF_SAS_MASK 0xFFu

//! Field CONSIST_CHECKS - No Content.
#define EIPE_EIP160_CONFIG_CONSIST_CHECKS_POS 8
//! Field CONSIST_CHECKS - No Content.
#define EIPE_EIP160_CONFIG_CONSIST_CHECKS_MASK 0xFF00u

//! Field MATCH_SCI - No Content.
#define EIPE_EIP160_CONFIG_MATCH_SCI_POS 16
//! Field MATCH_SCI - No Content.
#define EIPE_EIP160_CONFIG_MATCH_SCI_MASK 0x10000u

//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_CONFIG_RESERVED_0_POS 17
//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_CONFIG_RESERVED_0_MASK 0xFFFE0000u

//! @}

//! \defgroup EIPE_EIP160_VERSION Register EIPE_EIP160_VERSION - EIPE EIP160 VERSION
//! @{

//! Register Offset (relative)
#define EIPE_EIP160_VERSION 0x3FC
//! Register Offset (absolute) for 1st Instance EGR_SYSTEM_CONTROL
#define EGR_REG_SYS_CTRL_EIP160_VERSION 0xFFFCu

//! Register Reset Value
#define EIPE_EIP160_VERSION_RST 0x04225FA0u

//! Field EIP_NUMBER - No Content.
#define EIPE_EIP160_VERSION_EIP_NUMBER_POS 0
//! Field EIP_NUMBER - No Content.
#define EIPE_EIP160_VERSION_EIP_NUMBER_MASK 0xFFu

//! Field EIP_NUMBER_COMPL - No Content.
#define EIPE_EIP160_VERSION_EIP_NUMBER_COMPL_POS 8
//! Field EIP_NUMBER_COMPL - No Content.
#define EIPE_EIP160_VERSION_EIP_NUMBER_COMPL_MASK 0xFF00u

//! Field PATCH_LEVEL - No Content.
#define EIPE_EIP160_VERSION_PATCH_LEVEL_POS 16
//! Field PATCH_LEVEL - No Content.
#define EIPE_EIP160_VERSION_PATCH_LEVEL_MASK 0xF0000u

//! Field MINOR_VERSION - No Content.
#define EIPE_EIP160_VERSION_MINOR_VERSION_POS 20
//! Field MINOR_VERSION - No Content.
#define EIPE_EIP160_VERSION_MINOR_VERSION_MASK 0xF00000u

//! Field MAJOR_VERSION - No Content.
#define EIPE_EIP160_VERSION_MAJOR_VERSION_POS 24
//! Field MAJOR_VERSION - No Content.
#define EIPE_EIP160_VERSION_MAJOR_VERSION_MASK 0xF000000u

//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_VERSION_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPE_EIP160_VERSION_RESERVED_0_MASK 0xF0000000u

//! @}

//! @}

#endif
