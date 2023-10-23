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
// LSD Source          : /home/p34x/p34x_chip/v_reddydev.priv.p34x_chip.chip_work/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_Stat_VLAN_def.xml
// Register File Name  : ING_STAT_VLAN
// Register File Title : EIPI EIP_160s_i_32_Stat_VLAN
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_STAT_VLAN_H
#define _ING_STAT_VLAN_H

//! \defgroup ING_STAT_VLAN Register File ING_STAT_VLAN - EIPI EIP_160s_i_32_Stat_VLAN
//! @{

//! Base Address of ING_STAT_VLAN
#define ING_STAT_VLAN_MODULE_BASE 0xC000u

//! \defgroup EIPI_VLAN_UP_0_INOCTETSVL_LO Register EIPI_VLAN_UP_0_INOCTETSVL_LO - EIPI VLAN UP 0 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INOCTETSVL_LO 0x0
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INOCTETSVL_LO 0xC000u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INOCTETSVL_HI Register EIPI_VLAN_UP_0_INOCTETSVL_HI - EIPI VLAN UP 0 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INOCTETSVL_HI 0x4
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INOCTETSVL_HI 0xC004u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_RESERVED2 Register EIPI_VLAN_UP_0_RESERVED2 - EIPI VLAN UP 0 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_RESERVED2 0x8
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_RESERVED2 0xC008u

//! Register Reset Value
#define EIPI_VLAN_UP_0_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_RESERVED3 Register EIPI_VLAN_UP_0_RESERVED3 - EIPI VLAN UP 0 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_RESERVED3 0xC
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_RESERVED3 0xC00Cu

//! Register Reset Value
#define EIPI_VLAN_UP_0_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INPKTSVL_LO Register EIPI_VLAN_UP_0_INPKTSVL_LO - EIPI VLAN UP 0 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INPKTSVL_LO 0x10
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INPKTSVL_LO 0xC010u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INPKTSVL_HI Register EIPI_VLAN_UP_0_INPKTSVL_HI - EIPI VLAN UP 0 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INPKTSVL_HI 0x14
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INPKTSVL_HI 0xC014u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO - EIPI VLAN UP 0 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO 0x18
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INDROPPEDPKTSVL_LO 0xC018u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI - EIPI VLAN UP 0 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI 0x1C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INDROPPEDPKTSVL_HI 0xC01Cu

//! Register Reset Value
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 0 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO 0x20
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INOVERSIZEPKTSVL_LO 0xC020u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 0 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI 0x24
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_0_INOVERSIZEPKTSVL_HI 0xC024u

//! Register Reset Value
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_0_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INOCTETSVL_LO Register EIPI_VLAN_UP_1_INOCTETSVL_LO - EIPI VLAN UP 1 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INOCTETSVL_LO 0x80
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INOCTETSVL_LO 0xC080u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INOCTETSVL_HI Register EIPI_VLAN_UP_1_INOCTETSVL_HI - EIPI VLAN UP 1 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INOCTETSVL_HI 0x84
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INOCTETSVL_HI 0xC084u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_RESERVED2 Register EIPI_VLAN_UP_1_RESERVED2 - EIPI VLAN UP 1 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_RESERVED2 0x88
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_RESERVED2 0xC088u

//! Register Reset Value
#define EIPI_VLAN_UP_1_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_RESERVED3 Register EIPI_VLAN_UP_1_RESERVED3 - EIPI VLAN UP 1 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_RESERVED3 0x8C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_RESERVED3 0xC08Cu

//! Register Reset Value
#define EIPI_VLAN_UP_1_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INPKTSVL_LO Register EIPI_VLAN_UP_1_INPKTSVL_LO - EIPI VLAN UP 1 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INPKTSVL_LO 0x90
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INPKTSVL_LO 0xC090u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INPKTSVL_HI Register EIPI_VLAN_UP_1_INPKTSVL_HI - EIPI VLAN UP 1 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INPKTSVL_HI 0x94
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INPKTSVL_HI 0xC094u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO - EIPI VLAN UP 1 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO 0x98
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INDROPPEDPKTSVL_LO 0xC098u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI - EIPI VLAN UP 1 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI 0x9C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INDROPPEDPKTSVL_HI 0xC09Cu

//! Register Reset Value
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 1 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO 0xA0
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INOVERSIZEPKTSVL_LO 0xC0A0u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 1 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI 0xA4
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_1_INOVERSIZEPKTSVL_HI 0xC0A4u

//! Register Reset Value
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_1_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INOCTETSVL_LO Register EIPI_VLAN_UP_2_INOCTETSVL_LO - EIPI VLAN UP 2 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INOCTETSVL_LO 0x100
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INOCTETSVL_LO 0xC100u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INOCTETSVL_HI Register EIPI_VLAN_UP_2_INOCTETSVL_HI - EIPI VLAN UP 2 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INOCTETSVL_HI 0x104
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INOCTETSVL_HI 0xC104u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_RESERVED2 Register EIPI_VLAN_UP_2_RESERVED2 - EIPI VLAN UP 2 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_RESERVED2 0x108
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_RESERVED2 0xC108u

//! Register Reset Value
#define EIPI_VLAN_UP_2_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_RESERVED3 Register EIPI_VLAN_UP_2_RESERVED3 - EIPI VLAN UP 2 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_RESERVED3 0x10C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_RESERVED3 0xC10Cu

//! Register Reset Value
#define EIPI_VLAN_UP_2_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INPKTSVL_LO Register EIPI_VLAN_UP_2_INPKTSVL_LO - EIPI VLAN UP 2 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INPKTSVL_LO 0x110
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INPKTSVL_LO 0xC110u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INPKTSVL_HI Register EIPI_VLAN_UP_2_INPKTSVL_HI - EIPI VLAN UP 2 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INPKTSVL_HI 0x114
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INPKTSVL_HI 0xC114u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO - EIPI VLAN UP 2 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO 0x118
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INDROPPEDPKTSVL_LO 0xC118u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI - EIPI VLAN UP 2 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI 0x11C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INDROPPEDPKTSVL_HI 0xC11Cu

//! Register Reset Value
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 2 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO 0x120
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INOVERSIZEPKTSVL_LO 0xC120u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 2 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI 0x124
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_2_INOVERSIZEPKTSVL_HI 0xC124u

//! Register Reset Value
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_2_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INOCTETSVL_LO Register EIPI_VLAN_UP_3_INOCTETSVL_LO - EIPI VLAN UP 3 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INOCTETSVL_LO 0x180
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INOCTETSVL_LO 0xC180u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INOCTETSVL_HI Register EIPI_VLAN_UP_3_INOCTETSVL_HI - EIPI VLAN UP 3 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INOCTETSVL_HI 0x184
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INOCTETSVL_HI 0xC184u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_RESERVED2 Register EIPI_VLAN_UP_3_RESERVED2 - EIPI VLAN UP 3 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_RESERVED2 0x188
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_RESERVED2 0xC188u

//! Register Reset Value
#define EIPI_VLAN_UP_3_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_RESERVED3 Register EIPI_VLAN_UP_3_RESERVED3 - EIPI VLAN UP 3 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_RESERVED3 0x18C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_RESERVED3 0xC18Cu

//! Register Reset Value
#define EIPI_VLAN_UP_3_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INPKTSVL_LO Register EIPI_VLAN_UP_3_INPKTSVL_LO - EIPI VLAN UP 3 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INPKTSVL_LO 0x190
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INPKTSVL_LO 0xC190u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INPKTSVL_HI Register EIPI_VLAN_UP_3_INPKTSVL_HI - EIPI VLAN UP 3 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INPKTSVL_HI 0x194
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INPKTSVL_HI 0xC194u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO - EIPI VLAN UP 3 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO 0x198
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INDROPPEDPKTSVL_LO 0xC198u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI - EIPI VLAN UP 3 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI 0x19C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INDROPPEDPKTSVL_HI 0xC19Cu

//! Register Reset Value
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 3 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO 0x1A0
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INOVERSIZEPKTSVL_LO 0xC1A0u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 3 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI 0x1A4
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_3_INOVERSIZEPKTSVL_HI 0xC1A4u

//! Register Reset Value
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_3_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INOCTETSVL_LO Register EIPI_VLAN_UP_4_INOCTETSVL_LO - EIPI VLAN UP 4 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INOCTETSVL_LO 0x200
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INOCTETSVL_LO 0xC200u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INOCTETSVL_HI Register EIPI_VLAN_UP_4_INOCTETSVL_HI - EIPI VLAN UP 4 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INOCTETSVL_HI 0x204
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INOCTETSVL_HI 0xC204u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_RESERVED2 Register EIPI_VLAN_UP_4_RESERVED2 - EIPI VLAN UP 4 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_RESERVED2 0x208
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_RESERVED2 0xC208u

//! Register Reset Value
#define EIPI_VLAN_UP_4_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_RESERVED3 Register EIPI_VLAN_UP_4_RESERVED3 - EIPI VLAN UP 4 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_RESERVED3 0x20C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_RESERVED3 0xC20Cu

//! Register Reset Value
#define EIPI_VLAN_UP_4_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INPKTSVL_LO Register EIPI_VLAN_UP_4_INPKTSVL_LO - EIPI VLAN UP 4 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INPKTSVL_LO 0x210
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INPKTSVL_LO 0xC210u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INPKTSVL_HI Register EIPI_VLAN_UP_4_INPKTSVL_HI - EIPI VLAN UP 4 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INPKTSVL_HI 0x214
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INPKTSVL_HI 0xC214u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO - EIPI VLAN UP 4 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO 0x218
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INDROPPEDPKTSVL_LO 0xC218u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI - EIPI VLAN UP 4 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI 0x21C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INDROPPEDPKTSVL_HI 0xC21Cu

//! Register Reset Value
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 4 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO 0x220
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INOVERSIZEPKTSVL_LO 0xC220u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 4 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI 0x224
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_4_INOVERSIZEPKTSVL_HI 0xC224u

//! Register Reset Value
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_4_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INOCTETSVL_LO Register EIPI_VLAN_UP_5_INOCTETSVL_LO - EIPI VLAN UP 5 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INOCTETSVL_LO 0x280
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INOCTETSVL_LO 0xC280u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INOCTETSVL_HI Register EIPI_VLAN_UP_5_INOCTETSVL_HI - EIPI VLAN UP 5 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INOCTETSVL_HI 0x284
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INOCTETSVL_HI 0xC284u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_RESERVED2 Register EIPI_VLAN_UP_5_RESERVED2 - EIPI VLAN UP 5 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_RESERVED2 0x288
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_RESERVED2 0xC288u

//! Register Reset Value
#define EIPI_VLAN_UP_5_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_RESERVED3 Register EIPI_VLAN_UP_5_RESERVED3 - EIPI VLAN UP 5 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_RESERVED3 0x28C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_RESERVED3 0xC28Cu

//! Register Reset Value
#define EIPI_VLAN_UP_5_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INPKTSVL_LO Register EIPI_VLAN_UP_5_INPKTSVL_LO - EIPI VLAN UP 5 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INPKTSVL_LO 0x290
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INPKTSVL_LO 0xC290u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INPKTSVL_HI Register EIPI_VLAN_UP_5_INPKTSVL_HI - EIPI VLAN UP 5 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INPKTSVL_HI 0x294
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INPKTSVL_HI 0xC294u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO - EIPI VLAN UP 5 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO 0x298
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INDROPPEDPKTSVL_LO 0xC298u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI - EIPI VLAN UP 5 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI 0x29C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INDROPPEDPKTSVL_HI 0xC29Cu

//! Register Reset Value
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 5 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO 0x2A0
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INOVERSIZEPKTSVL_LO 0xC2A0u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 5 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI 0x2A4
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_5_INOVERSIZEPKTSVL_HI 0xC2A4u

//! Register Reset Value
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_5_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INOCTETSVL_LO Register EIPI_VLAN_UP_6_INOCTETSVL_LO - EIPI VLAN UP 6 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INOCTETSVL_LO 0x300
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INOCTETSVL_LO 0xC300u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INOCTETSVL_HI Register EIPI_VLAN_UP_6_INOCTETSVL_HI - EIPI VLAN UP 6 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INOCTETSVL_HI 0x304
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INOCTETSVL_HI 0xC304u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_RESERVED2 Register EIPI_VLAN_UP_6_RESERVED2 - EIPI VLAN UP 6 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_RESERVED2 0x308
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_RESERVED2 0xC308u

//! Register Reset Value
#define EIPI_VLAN_UP_6_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_RESERVED3 Register EIPI_VLAN_UP_6_RESERVED3 - EIPI VLAN UP 6 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_RESERVED3 0x30C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_RESERVED3 0xC30Cu

//! Register Reset Value
#define EIPI_VLAN_UP_6_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INPKTSVL_LO Register EIPI_VLAN_UP_6_INPKTSVL_LO - EIPI VLAN UP 6 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INPKTSVL_LO 0x310
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INPKTSVL_LO 0xC310u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INPKTSVL_HI Register EIPI_VLAN_UP_6_INPKTSVL_HI - EIPI VLAN UP 6 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INPKTSVL_HI 0x314
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INPKTSVL_HI 0xC314u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO - EIPI VLAN UP 6 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO 0x318
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INDROPPEDPKTSVL_LO 0xC318u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI - EIPI VLAN UP 6 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI 0x31C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INDROPPEDPKTSVL_HI 0xC31Cu

//! Register Reset Value
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 6 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO 0x320
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INOVERSIZEPKTSVL_LO 0xC320u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 6 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI 0x324
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_6_INOVERSIZEPKTSVL_HI 0xC324u

//! Register Reset Value
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_6_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INOCTETSVL_LO Register EIPI_VLAN_UP_7_INOCTETSVL_LO - EIPI VLAN UP 7 InOctetsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INOCTETSVL_LO 0x380
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INOCTETSVL_LO 0xC380u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INOCTETSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOCTETSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOCTETSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INOCTETSVL_HI Register EIPI_VLAN_UP_7_INOCTETSVL_HI - EIPI VLAN UP 7 InOctetsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INOCTETSVL_HI 0x384
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INOCTETSVL_HI 0xC384u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INOCTETSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOCTETSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOCTETSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_RESERVED2 Register EIPI_VLAN_UP_7_RESERVED2 - EIPI VLAN UP 7 Reserved2
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_RESERVED2 0x388
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_RESERVED2 0xC388u

//! Register Reset Value
#define EIPI_VLAN_UP_7_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_RESERVED3 Register EIPI_VLAN_UP_7_RESERVED3 - EIPI VLAN UP 7 Reserved3
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_RESERVED3 0x38C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_RESERVED3 0xC38Cu

//! Register Reset Value
#define EIPI_VLAN_UP_7_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INPKTSVL_LO Register EIPI_VLAN_UP_7_INPKTSVL_LO - EIPI VLAN UP 7 InPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INPKTSVL_LO 0x390
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INPKTSVL_LO 0xC390u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INPKTSVL_HI Register EIPI_VLAN_UP_7_INPKTSVL_HI - EIPI VLAN UP 7 InPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INPKTSVL_HI 0x394
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INPKTSVL_HI 0xC394u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO Register EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO - EIPI VLAN UP 7 InDroppedPktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO 0x398
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INDROPPEDPKTSVL_LO 0xC398u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI Register EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI - EIPI VLAN UP 7 InDroppedPktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI 0x39C
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INDROPPEDPKTSVL_HI 0xC39Cu

//! Register Reset Value
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INDROPPEDPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO Register EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO - EIPI VLAN UP 7 InOverSizePktsVL lo
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO 0x3A0
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INOVERSIZEPKTSVL_LO 0xC3A0u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI Register EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI - EIPI VLAN UP 7 InOverSizePktsVL hi
//! @{

//! Register Offset (relative)
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI 0x3A4
//! Register Offset (absolute) for 1st Instance ING_STAT_VLAN
#define ING_REG_STAT_VLAN_UP_7_INOVERSIZEPKTSVL_HI 0xC3A4u

//! Register Reset Value
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_VLAN_UP_7_INOVERSIZEPKTSVL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
