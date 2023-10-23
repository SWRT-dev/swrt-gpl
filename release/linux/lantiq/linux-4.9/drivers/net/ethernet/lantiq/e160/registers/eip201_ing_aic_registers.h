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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/ING_EIP201_Registers_def.xml
// Register File Name  : ING_EIP201_REGISTERS
// Register File Title : EIPI EIP_201_EIP201_Registers
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_EIP201_REGISTERS_H
#define _ING_EIP201_REGISTERS_H

//! \defgroup ING_EIP201_REGISTERS Register File ING_EIP201_REGISTERS - EIPI EIP_201_EIP201_Registers
//! @{

//! Base Address of ING_EIP201_REGISTERS
#define ING_EIP201_REGISTERS_MODULE_BASE 0xF800u

//! \defgroup EIPI_AIC_POL_CTRL Register EIPI_AIC_POL_CTRL - EIPI AIC POL CTRL
//! @{

//! Register Offset (relative)
#define EIPI_AIC_POL_CTRL 0x0
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_POL_CTRL 0xF800u

//! Register Reset Value
#define EIPI_AIC_POL_CTRL_RST 0x000003FFu

//! Field POLARITY_CONTROL_0 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_0_POS 0
//! Field POLARITY_CONTROL_0 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_0_MASK 0x1u

//! Field POLARITY_CONTROL_1 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_1_POS 1
//! Field POLARITY_CONTROL_1 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_1_MASK 0x2u

//! Field POLARITY_CONTROL_2 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_2_POS 2
//! Field POLARITY_CONTROL_2 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_2_MASK 0x4u

//! Field POLARITY_CONTROL_3 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_3_POS 3
//! Field POLARITY_CONTROL_3 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_3_MASK 0x8u

//! Field POLARITY_CONTROL_4 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_4_POS 4
//! Field POLARITY_CONTROL_4 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_4_MASK 0x10u

//! Field POLARITY_CONTROL_5 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_5_POS 5
//! Field POLARITY_CONTROL_5 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_5_MASK 0x20u

//! Field POLARITY_CONTROL_6 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_6_POS 6
//! Field POLARITY_CONTROL_6 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_6_MASK 0x40u

//! Field POLARITY_CONTROL_7 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_7_POS 7
//! Field POLARITY_CONTROL_7 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_7_MASK 0x80u

//! Field POLARITY_CONTROL_8 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_8_POS 8
//! Field POLARITY_CONTROL_8 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_8_MASK 0x100u

//! Field POLARITY_CONTROL_9 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_9_POS 9
//! Field POLARITY_CONTROL_9 - No Content.
#define EIPI_AIC_POL_CTRL_POLARITY_CONTROL_9_MASK 0x200u

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_POL_CTRL_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_POL_CTRL_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_TYPE_CTRL Register EIPI_AIC_TYPE_CTRL - EIPI AIC TYPE CTRL
//! @{

//! Register Offset (relative)
#define EIPI_AIC_TYPE_CTRL 0x4
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_TYPE_CTRL 0xF804u

//! Register Reset Value
#define EIPI_AIC_TYPE_CTRL_RST 0x000003FFu

//! Field TYPE_CONTROL_0 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_0_POS 0
//! Field TYPE_CONTROL_0 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_0_MASK 0x1u

//! Field TYPE_CONTROL_1 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_1_POS 1
//! Field TYPE_CONTROL_1 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_1_MASK 0x2u

//! Field TYPE_CONTROL_2 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_2_POS 2
//! Field TYPE_CONTROL_2 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_2_MASK 0x4u

//! Field TYPE_CONTROL_3 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_3_POS 3
//! Field TYPE_CONTROL_3 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_3_MASK 0x8u

//! Field TYPE_CONTROL_4 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_4_POS 4
//! Field TYPE_CONTROL_4 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_4_MASK 0x10u

//! Field TYPE_CONTROL_5 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_5_POS 5
//! Field TYPE_CONTROL_5 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_5_MASK 0x20u

//! Field TYPE_CONTROL_6 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_6_POS 6
//! Field TYPE_CONTROL_6 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_6_MASK 0x40u

//! Field TYPE_CONTROL_7 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_7_POS 7
//! Field TYPE_CONTROL_7 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_7_MASK 0x80u

//! Field TYPE_CONTROL_8 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_8_POS 8
//! Field TYPE_CONTROL_8 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_8_MASK 0x100u

//! Field TYPE_CONTROL_9 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_9_POS 9
//! Field TYPE_CONTROL_9 - No Content.
#define EIPI_AIC_TYPE_CTRL_TYPE_CONTROL_9_MASK 0x200u

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_TYPE_CTRL_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_TYPE_CTRL_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_ENABLE_CTRL Register EIPI_AIC_ENABLE_CTRL - EIPI AIC ENABLE CTRL
//! @{

//! Register Offset (relative)
#define EIPI_AIC_ENABLE_CTRL 0x8
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_ENABLE_CTRL 0xF808u

//! Register Reset Value
#define EIPI_AIC_ENABLE_CTRL_RST 0x00000000u

//! Field ENABLE_CONTROL - No Content.
#define EIPI_AIC_ENABLE_CTRL_ENABLE_CONTROL_POS 0
//! Field ENABLE_CONTROL - No Content.
#define EIPI_AIC_ENABLE_CTRL_ENABLE_CONTROL_MASK 0x3FFu

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLE_CTRL_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLE_CTRL_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_RAW_STAT_ENABLE_SET Register EIPI_AIC_RAW_STAT_ENABLE_SET - EIPI AIC RAW STAT ENABLE SET
//! @{

//! Register Offset (relative)
#define EIPI_AIC_RAW_STAT_ENABLE_SET 0xC
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_RAW_STAT_ENABLE_SET 0xF80Cu

//! Register Reset Value
#define EIPI_AIC_RAW_STAT_ENABLE_SET_RST 0x00000000u

//! Field RAW_STATUS_ENABLE_SET - No Content.
#define EIPI_AIC_RAW_STAT_ENABLE_SET_RAW_STATUS_ENABLE_SET_POS 0
//! Field RAW_STATUS_ENABLE_SET - No Content.
#define EIPI_AIC_RAW_STAT_ENABLE_SET_RAW_STATUS_ENABLE_SET_MASK 0x3FFu

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_RAW_STAT_ENABLE_SET_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_RAW_STAT_ENABLE_SET_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_ENABLED_STAT_ACK Register EIPI_AIC_ENABLED_STAT_ACK - EIPI AIC ENABLED STAT ACK
//! @{

//! Register Offset (relative)
#define EIPI_AIC_ENABLED_STAT_ACK 0x10
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_ENABLED_STAT_ACK 0xF810u

//! Register Reset Value
#define EIPI_AIC_ENABLED_STAT_ACK_RST 0x00000000u

//! Field ENABLED_STATUS_ACK - No Content.
#define EIPI_AIC_ENABLED_STAT_ACK_ENABLED_STATUS_ACK_POS 0
//! Field ENABLED_STATUS_ACK - No Content.
#define EIPI_AIC_ENABLED_STAT_ACK_ENABLED_STATUS_ACK_MASK 0x3FFu

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLED_STAT_ACK_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLED_STAT_ACK_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_ENABLE_CLR Register EIPI_AIC_ENABLE_CLR - EIPI AIC ENABLE CLR
//! @{

//! Register Offset (relative)
#define EIPI_AIC_ENABLE_CLR 0x14
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_ENABLE_CLR 0xF814u

//! Register Reset Value
#define EIPI_AIC_ENABLE_CLR_RST 0x00000000u

//! Field ENABLE_CLR - No Content.
#define EIPI_AIC_ENABLE_CLR_ENABLE_CLR_POS 0
//! Field ENABLE_CLR - No Content.
#define EIPI_AIC_ENABLE_CLR_ENABLE_CLR_MASK 0x3FFu

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLE_CLR_RESERVED_0_POS 10
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_ENABLE_CLR_RESERVED_0_MASK 0xFFFFFC00u

//! @}

//! \defgroup EIPI_AIC_OPTIONS Register EIPI_AIC_OPTIONS - EIPI AIC OPTIONS
//! @{

//! Register Offset (relative)
#define EIPI_AIC_OPTIONS 0x18
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_OPTIONS 0xF818u

//! Register Reset Value
#define EIPI_AIC_OPTIONS_RST 0x0000000Au

//! Field NR_OF_INPUTS - No Content.
#define EIPI_AIC_OPTIONS_NR_OF_INPUTS_POS 0
//! Field NR_OF_INPUTS - No Content.
#define EIPI_AIC_OPTIONS_NR_OF_INPUTS_MASK 0x3Fu

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_OPTIONS_RESERVED_0_POS 6
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_OPTIONS_RESERVED_0_MASK 0xFFFFFFC0u

//! @}

//! \defgroup EIPI_AIC_VERSION Register EIPI_AIC_VERSION - EIPI AIC VERSION
//! @{

//! Register Offset (relative)
#define EIPI_AIC_VERSION 0x1C
//! Register Offset (absolute) for 1st Instance ING_EIP201_REGISTERS
#define EGR_REG_EIP201_AIC_VERSION 0xF81Cu

//! Register Reset Value
#define EIPI_AIC_VERSION_RST 0x012236C9u

//! Field EIP_NUMBER - No Content.
#define EIPI_AIC_VERSION_EIP_NUMBER_POS 0
//! Field EIP_NUMBER - No Content.
#define EIPI_AIC_VERSION_EIP_NUMBER_MASK 0xFFu

//! Field EIP_NUMBER_COMPL - No Content.
#define EIPI_AIC_VERSION_EIP_NUMBER_COMPL_POS 8
//! Field EIP_NUMBER_COMPL - No Content.
#define EIPI_AIC_VERSION_EIP_NUMBER_COMPL_MASK 0xFF00u

//! Field PATCH_LEVEL - No Content.
#define EIPI_AIC_VERSION_PATCH_LEVEL_POS 16
//! Field PATCH_LEVEL - No Content.
#define EIPI_AIC_VERSION_PATCH_LEVEL_MASK 0xF0000u

//! Field MINOR_VERSION - No Content.
#define EIPI_AIC_VERSION_MINOR_VERSION_POS 20
//! Field MINOR_VERSION - No Content.
#define EIPI_AIC_VERSION_MINOR_VERSION_MASK 0xF00000u

//! Field MAJOR_VERSION - No Content.
#define EIPI_AIC_VERSION_MAJOR_VERSION_POS 24
//! Field MAJOR_VERSION - No Content.
#define EIPI_AIC_VERSION_MAJOR_VERSION_MASK 0xF000000u

//! Field RESERVED_0 - No Content.
#define EIPI_AIC_VERSION_RESERVED_0_POS 28
//! Field RESERVED_0 - No Content.
#define EIPI_AIC_VERSION_RESERVED_0_MASK 0xF0000000u

//! @}

//! @}

#endif
