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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_Classifier_control_packet_def.xml
// Register File Name  : EGR_CLASSIFIER_CONTROL_PACKET
// Register File Title : EIPE EIP_160s_e_32_Classifier_control_packet
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_CLASSIFIER_CONTROL_PACKET_H
#define _EGR_CLASSIFIER_CONTROL_PACKET_H

//! \defgroup EGR_CLASSIFIER_CONTROL_PACKET Register File EGR_CLASSIFIER_CONTROL_PACKET - EIPE EIP_160s_e_32_Classifier_control_packet
//! @{

//! Base Address of EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_CLASSIFIER_CONTROL_PACKET_MODULE_BASE 0x7800u

//! \defgroup EIPE_CP_MAC_DA_MATCH_0 Register EIPE_CP_MAC_DA_MATCH_0 - EIPE CP MAC DA MATCH 0
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_0 0x0
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_0 0x7800u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_0_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_0_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_0_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_0 Register EIPE_CP_MAC_DA_ET_MATCH_0 - EIPE CP MAC DA ET MATCH 0
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_0 0x4
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_0 0x7804u

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_0_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_0_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_0_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_0_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_0_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_1 Register EIPE_CP_MAC_DA_MATCH_1 - EIPE CP MAC DA MATCH 1
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_1 0x8
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_1 0x7808u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_1_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_1_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_1_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_1 Register EIPE_CP_MAC_DA_ET_MATCH_1 - EIPE CP MAC DA ET MATCH 1
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_1 0xC
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_1 0x780Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_1_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_1_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_1_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_1_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_1_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_2 Register EIPE_CP_MAC_DA_MATCH_2 - EIPE CP MAC DA MATCH 2
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_2 0x10
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_2 0x7810u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_2_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_2_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_2_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_2 Register EIPE_CP_MAC_DA_ET_MATCH_2 - EIPE CP MAC DA ET MATCH 2
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_2 0x14
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_2 0x7814u

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_2_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_2_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_2_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_2_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_2_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_3 Register EIPE_CP_MAC_DA_MATCH_3 - EIPE CP MAC DA MATCH 3
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_3 0x18
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_3 0x7818u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_3_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_3_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_3_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_3 Register EIPE_CP_MAC_DA_ET_MATCH_3 - EIPE CP MAC DA ET MATCH 3
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_3 0x1C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_3 0x781Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_3_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_3_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_3_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_3_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_3_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_4 Register EIPE_CP_MAC_DA_MATCH_4 - EIPE CP MAC DA MATCH 4
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_4 0x20
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_4 0x7820u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_4_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_4_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_4_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_4 Register EIPE_CP_MAC_DA_ET_MATCH_4 - EIPE CP MAC DA ET MATCH 4
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_4 0x24
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_4 0x7824u

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_4_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_4_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_4_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_4_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_4_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_5 Register EIPE_CP_MAC_DA_MATCH_5 - EIPE CP MAC DA MATCH 5
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_5 0x28
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_5 0x7828u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_5_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_5_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_5_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_5 Register EIPE_CP_MAC_DA_ET_MATCH_5 - EIPE CP MAC DA ET MATCH 5
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_5 0x2C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_5 0x782Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_5_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_5_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_5_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_5_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_5_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_6 Register EIPE_CP_MAC_DA_MATCH_6 - EIPE CP MAC DA MATCH 6
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_6 0x30
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_6 0x7830u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_6_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_6_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_6_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_6 Register EIPE_CP_MAC_DA_ET_MATCH_6 - EIPE CP MAC DA ET MATCH 6
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_6 0x34
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_6 0x7834u

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_6_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_6_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_6_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_6_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_6_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_7 Register EIPE_CP_MAC_DA_MATCH_7 - EIPE CP MAC DA MATCH 7
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_7 0x38
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_7 0x7838u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_7_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_7_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_7_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_7 Register EIPE_CP_MAC_DA_ET_MATCH_7 - EIPE CP MAC DA ET MATCH 7
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_7 0x3C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_7 0x783Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_7_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_7_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_7_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_7_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_7_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_8 Register EIPE_CP_MAC_DA_MATCH_8 - EIPE CP MAC DA MATCH 8
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_8 0x40
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_8 0x7840u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_8_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_8_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_8_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_8 Register EIPE_CP_MAC_DA_ET_MATCH_8 - EIPE CP MAC DA ET MATCH 8
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_8 0x44
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_8 0x7844u

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_8_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_8_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_8_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_8_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_8_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_MATCH_9 Register EIPE_CP_MAC_DA_MATCH_9 - EIPE CP MAC DA MATCH 9
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_MATCH_9 0x48
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_MATCH_9 0x7848u

//! Register Reset Value
#define EIPE_CP_MAC_DA_MATCH_9_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_9_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_MATCH_9_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_ET_MATCH_9 Register EIPE_CP_MAC_DA_ET_MATCH_9 - EIPE CP MAC DA ET MATCH 9
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_ET_MATCH_9 0x4C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_ET_MATCH_9 0x784Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_ET_MATCH_9_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_9_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_9_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_9_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_DA_ET_MATCH_9_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_10 Register EIPE_CP_MAC_ET_MATCH_10 - EIPE CP MAC ET MATCH 10
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_10 0x50
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_10 0x7850u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_10_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_10_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_10_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_10_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_10_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_11 Register EIPE_CP_MAC_ET_MATCH_11 - EIPE CP MAC ET MATCH 11
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_11 0x54
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_11 0x7854u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_11_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_11_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_11_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_11_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_11_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_12 Register EIPE_CP_MAC_ET_MATCH_12 - EIPE CP MAC ET MATCH 12
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_12 0x58
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_12 0x7858u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_12_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_12_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_12_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_12_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_12_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_13 Register EIPE_CP_MAC_ET_MATCH_13 - EIPE CP MAC ET MATCH 13
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_13 0x5C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_13 0x785Cu

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_13_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_13_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_13_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_13_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_13_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_14 Register EIPE_CP_MAC_ET_MATCH_14 - EIPE CP MAC ET MATCH 14
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_14 0x60
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_14 0x7860u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_14_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_14_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_14_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_14_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_14_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_15 Register EIPE_CP_MAC_ET_MATCH_15 - EIPE CP MAC ET MATCH 15
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_15 0x64
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_15 0x7864u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_15_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_15_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_15_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_15_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_15_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_16 Register EIPE_CP_MAC_ET_MATCH_16 - EIPE CP MAC ET MATCH 16
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_16 0x68
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_16 0x7868u

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_16_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_16_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_16_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_16_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_16_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_ET_MATCH_17 Register EIPE_CP_MAC_ET_MATCH_17 - EIPE CP MAC ET MATCH 17
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_ET_MATCH_17 0x6C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_ET_MATCH_17 0x786Cu

//! Register Reset Value
#define EIPE_CP_MAC_ET_MATCH_17_RST 0x00000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_17_RESERVED_0_POS 0
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_ET_MATCH_17_RESERVED_0_MASK 0xFFFFu

//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_17_ETHER_TYPE_MATCH_POS 16
//! Field ETHER_TYPE_MATCH - No Content.
#define EIPE_CP_MAC_ET_MATCH_17_ETHER_TYPE_MATCH_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_START_LO Register EIPE_CP_MAC_DA_START_LO - EIPE CP MAC DA START LO
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_START_LO 0x80
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_START_LO 0x7880u

//! Register Reset Value
#define EIPE_CP_MAC_DA_START_LO_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_START_LO_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_START_LO_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_START_HI Register EIPE_CP_MAC_DA_START_HI - EIPE CP MAC DA START HI
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_START_HI 0x84
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_START_HI 0x7884u

//! Register Reset Value
#define EIPE_CP_MAC_DA_START_HI_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_START_HI_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_START_HI_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_START_HI_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_START_HI_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_END_LO Register EIPE_CP_MAC_DA_END_LO - EIPE CP MAC DA END LO
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_END_LO 0x88
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_END_LO 0x7888u

//! Register Reset Value
#define EIPE_CP_MAC_DA_END_LO_RST 0x00000000u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_END_LO_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_END_LO_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_END_HI Register EIPE_CP_MAC_DA_END_HI - EIPE CP MAC DA END HI
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_END_HI 0x8C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_END_HI 0x788Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_END_HI_RST 0x00000000u

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_END_HI_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_END_HI_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_END_HI_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_END_HI_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_44_BITS_LO Register EIPE_CP_MAC_DA_44_BITS_LO - EIPE CP MAC DA 44 BITS LO
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_44_BITS_LO 0x90
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_44_BITS_LO 0x7890u

//! Register Reset Value
#define EIPE_CP_MAC_DA_44_BITS_LO_RST 0x00C28001u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_LO_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_LO_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_44_BITS_HI Register EIPE_CP_MAC_DA_44_BITS_HI - EIPE CP MAC DA 44 BITS HI
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_44_BITS_HI 0x94
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_44_BITS_HI 0x7894u

//! Register Reset Value
#define EIPE_CP_MAC_DA_44_BITS_HI_RST 0x00000000u

//! Field MAC_DA_MATCH_15_8 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_MAC_DA_MATCH_15_8_POS 0
//! Field MAC_DA_MATCH_15_8 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_MAC_DA_MATCH_15_8_MASK 0xFFu

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_RESERVED_0_POS 8
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_RESERVED_0_MASK 0xF00u

//! Field MAC_DA_MATCH_7_4 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_MAC_DA_MATCH_7_4_POS 12
//! Field MAC_DA_MATCH_7_4 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_MAC_DA_MATCH_7_4_MASK 0xF000u

//! Field RESERVED_1 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_RESERVED_1_POS 16
//! Field RESERVED_1 - No Content.
#define EIPE_CP_MAC_DA_44_BITS_HI_RESERVED_1_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MAC_DA_48_BITS_LO Register EIPE_CP_MAC_DA_48_BITS_LO - EIPE CP MAC DA 48 BITS LO
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_48_BITS_LO 0x98
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_48_BITS_LO 0x7898u

//! Register Reset Value
#define EIPE_CP_MAC_DA_48_BITS_LO_RST 0xCC0C0001u

//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_LO_MAC_DA_MATCH_47_16_POS 0
//! Field MAC_DA_MATCH_47_16 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_LO_MAC_DA_MATCH_47_16_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_CP_MAC_DA_48_BITS_HI Register EIPE_CP_MAC_DA_48_BITS_HI - EIPE CP MAC DA 48 BITS HI
//! @{

//! Register Offset (relative)
#define EIPE_CP_MAC_DA_48_BITS_HI 0x9C
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MAC_DA_48_BITS_HI 0x789Cu

//! Register Reset Value
#define EIPE_CP_MAC_DA_48_BITS_HI_RST 0x0000CCCCu

//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_HI_MAC_DA_MATCH_15_0_POS 0
//! Field MAC_DA_MATCH_15_0 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_HI_MAC_DA_MATCH_15_0_MASK 0xFFFFu

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_HI_RESERVED_0_POS 16
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MAC_DA_48_BITS_HI_RESERVED_0_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPE_CP_MATCH_MODE Register EIPE_CP_MATCH_MODE - EIPE CP MATCH MODE
//! @{

//! Register Offset (relative)
#define EIPE_CP_MATCH_MODE 0xF8
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MATCH_MODE 0x78F8u

//! Register Reset Value
#define EIPE_CP_MATCH_MODE_RST 0x00000000u

//! Field VLAN_SEL_0 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_0_POS 0
//! Field VLAN_SEL_0 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_0_MASK 0x1u

//! Field VLAN_SEL_1 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_1_POS 1
//! Field VLAN_SEL_1 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_1_MASK 0x2u

//! Field VLAN_SEL_2 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_2_POS 2
//! Field VLAN_SEL_2 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_2_MASK 0x4u

//! Field VLAN_SEL_3 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_3_POS 3
//! Field VLAN_SEL_3 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_3_MASK 0x8u

//! Field VLAN_SEL_4 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_4_POS 4
//! Field VLAN_SEL_4 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_4_MASK 0x10u

//! Field VLAN_SEL_5 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_5_POS 5
//! Field VLAN_SEL_5 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_5_MASK 0x20u

//! Field VLAN_SEL_6 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_6_POS 6
//! Field VLAN_SEL_6 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_6_MASK 0x40u

//! Field VLAN_SEL_7 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_7_POS 7
//! Field VLAN_SEL_7 - No Content.
#define EIPE_CP_MATCH_MODE_VLAN_SEL_7_MASK 0x80u

//! Field E_TYPE_SEL_0 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_0_POS 8
//! Field E_TYPE_SEL_0 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_0_MASK 0x100u

//! Field E_TYPE_SEL_1 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_1_POS 9
//! Field E_TYPE_SEL_1 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_1_MASK 0x200u

//! Field E_TYPE_SEL_2 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_2_POS 10
//! Field E_TYPE_SEL_2 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_2_MASK 0x400u

//! Field E_TYPE_SEL_3 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_3_POS 11
//! Field E_TYPE_SEL_3 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_3_MASK 0x800u

//! Field E_TYPE_SEL_4 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_4_POS 12
//! Field E_TYPE_SEL_4 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_4_MASK 0x1000u

//! Field E_TYPE_SEL_5 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_5_POS 13
//! Field E_TYPE_SEL_5 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_5_MASK 0x2000u

//! Field E_TYPE_SEL_6 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_6_POS 14
//! Field E_TYPE_SEL_6 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_6_MASK 0x4000u

//! Field E_TYPE_SEL_7 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_7_POS 15
//! Field E_TYPE_SEL_7 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_7_MASK 0x8000u

//! Field E_TYPE_SEL_8 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_8_POS 16
//! Field E_TYPE_SEL_8 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_8_MASK 0x10000u

//! Field E_TYPE_SEL_9 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_9_POS 17
//! Field E_TYPE_SEL_9 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_9_MASK 0x20000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_MODE_RESERVED_0_POS 18
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_MODE_RESERVED_0_MASK 0x1C0000u

//! Field E_TYPE_SEL_10 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_10_POS 21
//! Field E_TYPE_SEL_10 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_10_MASK 0x200000u

//! Field E_TYPE_SEL_11 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_11_POS 22
//! Field E_TYPE_SEL_11 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_11_MASK 0x400000u

//! Field E_TYPE_SEL_12 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_12_POS 23
//! Field E_TYPE_SEL_12 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_12_MASK 0x800000u

//! Field E_TYPE_SEL_13 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_13_POS 24
//! Field E_TYPE_SEL_13 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_13_MASK 0x1000000u

//! Field E_TYPE_SEL_14 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_14_POS 25
//! Field E_TYPE_SEL_14 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_14_MASK 0x2000000u

//! Field E_TYPE_SEL_15 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_15_POS 26
//! Field E_TYPE_SEL_15 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_15_MASK 0x4000000u

//! Field E_TYPE_SEL_16 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_16_POS 27
//! Field E_TYPE_SEL_16 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_16_MASK 0x8000000u

//! Field E_TYPE_SEL_17 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_17_POS 28
//! Field E_TYPE_SEL_17 - No Content.
#define EIPE_CP_MATCH_MODE_E_TYPE_SEL_17_MASK 0x10000000u

//! Field RESERVED_1 - No Content.
#define EIPE_CP_MATCH_MODE_RESERVED_1_POS 29
//! Field RESERVED_1 - No Content.
#define EIPE_CP_MATCH_MODE_RESERVED_1_MASK 0xE0000000u

//! @}

//! \defgroup EIPE_CP_MATCH_ENABLE Register EIPE_CP_MATCH_ENABLE - EIPE CP MATCH ENABLE
//! @{

//! Register Offset (relative)
#define EIPE_CP_MATCH_ENABLE 0xFC
//! Register Offset (absolute) for 1st Instance EGR_CLASSIFIER_CONTROL_PACKET
#define EGR_REG_CLF_CP_MATCH_ENABLE 0x78FCu

//! Register Reset Value
#define EIPE_CP_MATCH_ENABLE_RST 0x00000000u

//! Field MAC_DA_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_0_POS 0
//! Field MAC_DA_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_0_MASK 0x1u

//! Field MAC_DA_1 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_1_POS 1
//! Field MAC_DA_1 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_1_MASK 0x2u

//! Field MAC_DA_2 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_2_POS 2
//! Field MAC_DA_2 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_2_MASK 0x4u

//! Field MAC_DA_3 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_3_POS 3
//! Field MAC_DA_3 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_3_MASK 0x8u

//! Field MAC_DA_4 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_4_POS 4
//! Field MAC_DA_4 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_4_MASK 0x10u

//! Field MAC_DA_5 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_5_POS 5
//! Field MAC_DA_5 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_5_MASK 0x20u

//! Field MAC_DA_6 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_6_POS 6
//! Field MAC_DA_6 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_6_MASK 0x40u

//! Field MAC_DA_7 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_7_POS 7
//! Field MAC_DA_7 - No Content.
#define EIPE_CP_MATCH_ENABLE_MAC_DA_7_MASK 0x80u

//! Field E_TYPE_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_0_POS 8
//! Field E_TYPE_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_0_MASK 0x100u

//! Field E_TYPE_1 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_1_POS 9
//! Field E_TYPE_1 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_1_MASK 0x200u

//! Field E_TYPE_2 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_2_POS 10
//! Field E_TYPE_2 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_2_MASK 0x400u

//! Field E_TYPE_3 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_3_POS 11
//! Field E_TYPE_3 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_3_MASK 0x800u

//! Field E_TYPE_4 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_4_POS 12
//! Field E_TYPE_4 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_4_MASK 0x1000u

//! Field E_TYPE_5 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_5_POS 13
//! Field E_TYPE_5 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_5_MASK 0x2000u

//! Field E_TYPE_6 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_6_POS 14
//! Field E_TYPE_6 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_6_MASK 0x4000u

//! Field E_TYPE_7 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_7_POS 15
//! Field E_TYPE_7 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_7_MASK 0x8000u

//! Field COMB_8 - No Content.
#define EIPE_CP_MATCH_ENABLE_COMB_8_POS 16
//! Field COMB_8 - No Content.
#define EIPE_CP_MATCH_ENABLE_COMB_8_MASK 0x10000u

//! Field COMB_9 - No Content.
#define EIPE_CP_MATCH_ENABLE_COMB_9_POS 17
//! Field COMB_9 - No Content.
#define EIPE_CP_MATCH_ENABLE_COMB_9_MASK 0x20000u

//! Field RANGE - No Content.
#define EIPE_CP_MATCH_ENABLE_RANGE_POS 18
//! Field RANGE - No Content.
#define EIPE_CP_MATCH_ENABLE_RANGE_MASK 0x40000u

//! Field CONST_44 - No Content.
#define EIPE_CP_MATCH_ENABLE_CONST_44_POS 19
//! Field CONST_44 - No Content.
#define EIPE_CP_MATCH_ENABLE_CONST_44_MASK 0x80000u

//! Field CONST_48 - No Content.
#define EIPE_CP_MATCH_ENABLE_CONST_48_POS 20
//! Field CONST_48 - No Content.
#define EIPE_CP_MATCH_ENABLE_CONST_48_MASK 0x100000u

//! Field E_TYPE_10 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_10_POS 21
//! Field E_TYPE_10 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_10_MASK 0x200000u

//! Field E_TYPE_11 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_11_POS 22
//! Field E_TYPE_11 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_11_MASK 0x400000u

//! Field E_TYPE_12 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_12_POS 23
//! Field E_TYPE_12 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_12_MASK 0x800000u

//! Field E_TYPE_13 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_13_POS 24
//! Field E_TYPE_13 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_13_MASK 0x1000000u

//! Field E_TYPE_14 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_14_POS 25
//! Field E_TYPE_14 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_14_MASK 0x2000000u

//! Field E_TYPE_15 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_15_POS 26
//! Field E_TYPE_15 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_15_MASK 0x4000000u

//! Field E_TYPE_16 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_16_POS 27
//! Field E_TYPE_16 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_16_MASK 0x8000000u

//! Field E_TYPE_17 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_17_POS 28
//! Field E_TYPE_17 - No Content.
#define EIPE_CP_MATCH_ENABLE_E_TYPE_17_MASK 0x10000000u

//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_RESERVED_0_POS 29
//! Field RESERVED_0 - No Content.
#define EIPE_CP_MATCH_ENABLE_RESERVED_0_MASK 0xE0000000u

//! @}

//! @}

#endif

