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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_Stat_SA_page0_def.xml
// Register File Name  : EGR_STAT_SA_PAGE0
// Register File Title : EIPE EIP_160s_e_32_Stat_SA_page0
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_STAT_SA_PAGE0_H
#define _EGR_STAT_SA_PAGE0_H

//! \defgroup EGR_STAT_SA_PAGE0 Register File EGR_STAT_SA_PAGE0 - EIPE EIP_160s_e_32_Stat_SA_page0
//! @{

//! Base Address of EGR_STAT_SA_PAGE0
#define EGR_STAT_SA_PAGE0_MODULE_BASE 0x8000u

//! \defgroup EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 0 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO 0x0
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8000u

//! Register Reset Value
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 0 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI 0x4
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8004u

//! Register Reset Value
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_RESERVED2 Register EIPE_SA_0_RESERVED2 - EIPE SA 0 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_RESERVED2 0x8
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_RESERVED2 0x8008u

//! Register Reset Value
#define EIPE_SA_0_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_RESERVED3 Register EIPE_SA_0_RESERVED3 - EIPE SA 0 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_RESERVED3 0xC
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_RESERVED3 0x800Cu

//! Register Reset Value
#define EIPE_SA_0_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 0 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO 0x10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO 0x8010u

//! Register Reset Value
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 0 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI 0x14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI 0x8014u

//! Register Reset Value
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_OUTPKTSTOOLONG_LO Register EIPE_SA_0_OUTPKTSTOOLONG_LO - EIPE SA 0 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTPKTSTOOLONG_LO 0x18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTPKTSTOOLONG_LO 0x8018u

//! Register Reset Value
#define EIPE_SA_0_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_0_OUTPKTSTOOLONG_HI Register EIPE_SA_0_OUTPKTSTOOLONG_HI - EIPE SA 0 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_0_OUTPKTSTOOLONG_HI 0x1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_0_OUTPKTSTOOLONG_HI 0x801Cu

//! Register Reset Value
#define EIPE_SA_0_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_0_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 1 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO 0x80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8080u

//! Register Reset Value
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 1 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI 0x84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8084u

//! Register Reset Value
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_RESERVED2 Register EIPE_SA_1_RESERVED2 - EIPE SA 1 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_RESERVED2 0x88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_RESERVED2 0x8088u

//! Register Reset Value
#define EIPE_SA_1_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_RESERVED3 Register EIPE_SA_1_RESERVED3 - EIPE SA 1 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_RESERVED3 0x8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_RESERVED3 0x808Cu

//! Register Reset Value
#define EIPE_SA_1_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 1 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO 0x90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO 0x8090u

//! Register Reset Value
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 1 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI 0x94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI 0x8094u

//! Register Reset Value
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTPKTSTOOLONG_LO Register EIPE_SA_1_OUTPKTSTOOLONG_LO - EIPE SA 1 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTPKTSTOOLONG_LO 0x98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTPKTSTOOLONG_LO 0x8098u

//! Register Reset Value
#define EIPE_SA_1_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_1_OUTPKTSTOOLONG_HI Register EIPE_SA_1_OUTPKTSTOOLONG_HI - EIPE SA 1 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_1_OUTPKTSTOOLONG_HI 0x9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_1_OUTPKTSTOOLONG_HI 0x809Cu

//! Register Reset Value
#define EIPE_SA_1_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_1_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 2 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO 0x100
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8100u

//! Register Reset Value
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 2 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI 0x104
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8104u

//! Register Reset Value
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_RESERVED2 Register EIPE_SA_2_RESERVED2 - EIPE SA 2 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_RESERVED2 0x108
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_RESERVED2 0x8108u

//! Register Reset Value
#define EIPE_SA_2_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_RESERVED3 Register EIPE_SA_2_RESERVED3 - EIPE SA 2 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_RESERVED3 0x10C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_RESERVED3 0x810Cu

//! Register Reset Value
#define EIPE_SA_2_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 2 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO 0x110
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO 0x8110u

//! Register Reset Value
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 2 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI 0x114
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI 0x8114u

//! Register Reset Value
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTPKTSTOOLONG_LO Register EIPE_SA_2_OUTPKTSTOOLONG_LO - EIPE SA 2 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTPKTSTOOLONG_LO 0x118
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTPKTSTOOLONG_LO 0x8118u

//! Register Reset Value
#define EIPE_SA_2_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_2_OUTPKTSTOOLONG_HI Register EIPE_SA_2_OUTPKTSTOOLONG_HI - EIPE SA 2 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_2_OUTPKTSTOOLONG_HI 0x11C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_2_OUTPKTSTOOLONG_HI 0x811Cu

//! Register Reset Value
#define EIPE_SA_2_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_2_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 3 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO 0x180
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8180u

//! Register Reset Value
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 3 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI 0x184
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8184u

//! Register Reset Value
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_RESERVED2 Register EIPE_SA_3_RESERVED2 - EIPE SA 3 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_RESERVED2 0x188
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_RESERVED2 0x8188u

//! Register Reset Value
#define EIPE_SA_3_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_RESERVED3 Register EIPE_SA_3_RESERVED3 - EIPE SA 3 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_RESERVED3 0x18C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_RESERVED3 0x818Cu

//! Register Reset Value
#define EIPE_SA_3_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 3 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO 0x190
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO 0x8190u

//! Register Reset Value
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 3 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI 0x194
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI 0x8194u

//! Register Reset Value
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTPKTSTOOLONG_LO Register EIPE_SA_3_OUTPKTSTOOLONG_LO - EIPE SA 3 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTPKTSTOOLONG_LO 0x198
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTPKTSTOOLONG_LO 0x8198u

//! Register Reset Value
#define EIPE_SA_3_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_3_OUTPKTSTOOLONG_HI Register EIPE_SA_3_OUTPKTSTOOLONG_HI - EIPE SA 3 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_3_OUTPKTSTOOLONG_HI 0x19C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_3_OUTPKTSTOOLONG_HI 0x819Cu

//! Register Reset Value
#define EIPE_SA_3_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_3_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 4 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO 0x200
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8200u

//! Register Reset Value
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 4 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI 0x204
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8204u

//! Register Reset Value
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_RESERVED2 Register EIPE_SA_4_RESERVED2 - EIPE SA 4 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_RESERVED2 0x208
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_RESERVED2 0x8208u

//! Register Reset Value
#define EIPE_SA_4_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_RESERVED3 Register EIPE_SA_4_RESERVED3 - EIPE SA 4 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_RESERVED3 0x20C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_RESERVED3 0x820Cu

//! Register Reset Value
#define EIPE_SA_4_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 4 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO 0x210
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO 0x8210u

//! Register Reset Value
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 4 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI 0x214
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI 0x8214u

//! Register Reset Value
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTPKTSTOOLONG_LO Register EIPE_SA_4_OUTPKTSTOOLONG_LO - EIPE SA 4 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTPKTSTOOLONG_LO 0x218
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTPKTSTOOLONG_LO 0x8218u

//! Register Reset Value
#define EIPE_SA_4_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_4_OUTPKTSTOOLONG_HI Register EIPE_SA_4_OUTPKTSTOOLONG_HI - EIPE SA 4 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_4_OUTPKTSTOOLONG_HI 0x21C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_4_OUTPKTSTOOLONG_HI 0x821Cu

//! Register Reset Value
#define EIPE_SA_4_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_4_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 5 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO 0x280
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8280u

//! Register Reset Value
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 5 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI 0x284
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8284u

//! Register Reset Value
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_RESERVED2 Register EIPE_SA_5_RESERVED2 - EIPE SA 5 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_RESERVED2 0x288
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_RESERVED2 0x8288u

//! Register Reset Value
#define EIPE_SA_5_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_RESERVED3 Register EIPE_SA_5_RESERVED3 - EIPE SA 5 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_RESERVED3 0x28C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_RESERVED3 0x828Cu

//! Register Reset Value
#define EIPE_SA_5_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 5 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO 0x290
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO 0x8290u

//! Register Reset Value
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 5 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI 0x294
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI 0x8294u

//! Register Reset Value
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTPKTSTOOLONG_LO Register EIPE_SA_5_OUTPKTSTOOLONG_LO - EIPE SA 5 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTPKTSTOOLONG_LO 0x298
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTPKTSTOOLONG_LO 0x8298u

//! Register Reset Value
#define EIPE_SA_5_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_5_OUTPKTSTOOLONG_HI Register EIPE_SA_5_OUTPKTSTOOLONG_HI - EIPE SA 5 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_5_OUTPKTSTOOLONG_HI 0x29C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_5_OUTPKTSTOOLONG_HI 0x829Cu

//! Register Reset Value
#define EIPE_SA_5_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_5_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 6 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO 0x300
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8300u

//! Register Reset Value
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 6 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI 0x304
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8304u

//! Register Reset Value
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_RESERVED2 Register EIPE_SA_6_RESERVED2 - EIPE SA 6 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_RESERVED2 0x308
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_RESERVED2 0x8308u

//! Register Reset Value
#define EIPE_SA_6_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_RESERVED3 Register EIPE_SA_6_RESERVED3 - EIPE SA 6 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_RESERVED3 0x30C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_RESERVED3 0x830Cu

//! Register Reset Value
#define EIPE_SA_6_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 6 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO 0x310
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO 0x8310u

//! Register Reset Value
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 6 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI 0x314
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI 0x8314u

//! Register Reset Value
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTPKTSTOOLONG_LO Register EIPE_SA_6_OUTPKTSTOOLONG_LO - EIPE SA 6 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTPKTSTOOLONG_LO 0x318
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTPKTSTOOLONG_LO 0x8318u

//! Register Reset Value
#define EIPE_SA_6_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_6_OUTPKTSTOOLONG_HI Register EIPE_SA_6_OUTPKTSTOOLONG_HI - EIPE SA 6 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_6_OUTPKTSTOOLONG_HI 0x31C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_6_OUTPKTSTOOLONG_HI 0x831Cu

//! Register Reset Value
#define EIPE_SA_6_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_6_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 7 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO 0x380
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8380u

//! Register Reset Value
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 7 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI 0x384
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8384u

//! Register Reset Value
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_RESERVED2 Register EIPE_SA_7_RESERVED2 - EIPE SA 7 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_RESERVED2 0x388
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_RESERVED2 0x8388u

//! Register Reset Value
#define EIPE_SA_7_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_RESERVED3 Register EIPE_SA_7_RESERVED3 - EIPE SA 7 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_RESERVED3 0x38C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_RESERVED3 0x838Cu

//! Register Reset Value
#define EIPE_SA_7_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 7 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO 0x390
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO 0x8390u

//! Register Reset Value
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 7 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI 0x394
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI 0x8394u

//! Register Reset Value
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTPKTSTOOLONG_LO Register EIPE_SA_7_OUTPKTSTOOLONG_LO - EIPE SA 7 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTPKTSTOOLONG_LO 0x398
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTPKTSTOOLONG_LO 0x8398u

//! Register Reset Value
#define EIPE_SA_7_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_7_OUTPKTSTOOLONG_HI Register EIPE_SA_7_OUTPKTSTOOLONG_HI - EIPE SA 7 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_7_OUTPKTSTOOLONG_HI 0x39C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_7_OUTPKTSTOOLONG_HI 0x839Cu

//! Register Reset Value
#define EIPE_SA_7_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_7_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 8 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO 0x400
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8400u

//! Register Reset Value
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 8 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI 0x404
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8404u

//! Register Reset Value
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_RESERVED2 Register EIPE_SA_8_RESERVED2 - EIPE SA 8 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_RESERVED2 0x408
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_RESERVED2 0x8408u

//! Register Reset Value
#define EIPE_SA_8_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_RESERVED3 Register EIPE_SA_8_RESERVED3 - EIPE SA 8 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_RESERVED3 0x40C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_RESERVED3 0x840Cu

//! Register Reset Value
#define EIPE_SA_8_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 8 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO 0x410
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO 0x8410u

//! Register Reset Value
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 8 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI 0x414
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI 0x8414u

//! Register Reset Value
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTPKTSTOOLONG_LO Register EIPE_SA_8_OUTPKTSTOOLONG_LO - EIPE SA 8 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTPKTSTOOLONG_LO 0x418
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTPKTSTOOLONG_LO 0x8418u

//! Register Reset Value
#define EIPE_SA_8_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_8_OUTPKTSTOOLONG_HI Register EIPE_SA_8_OUTPKTSTOOLONG_HI - EIPE SA 8 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_8_OUTPKTSTOOLONG_HI 0x41C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_8_OUTPKTSTOOLONG_HI 0x841Cu

//! Register Reset Value
#define EIPE_SA_8_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_8_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 9 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO 0x480
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8480u

//! Register Reset Value
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 9 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI 0x484
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8484u

//! Register Reset Value
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_RESERVED2 Register EIPE_SA_9_RESERVED2 - EIPE SA 9 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_RESERVED2 0x488
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_RESERVED2 0x8488u

//! Register Reset Value
#define EIPE_SA_9_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_RESERVED3 Register EIPE_SA_9_RESERVED3 - EIPE SA 9 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_RESERVED3 0x48C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_RESERVED3 0x848Cu

//! Register Reset Value
#define EIPE_SA_9_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 9 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO 0x490
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO 0x8490u

//! Register Reset Value
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 9 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI 0x494
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI 0x8494u

//! Register Reset Value
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTPKTSTOOLONG_LO Register EIPE_SA_9_OUTPKTSTOOLONG_LO - EIPE SA 9 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTPKTSTOOLONG_LO 0x498
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTPKTSTOOLONG_LO 0x8498u

//! Register Reset Value
#define EIPE_SA_9_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_9_OUTPKTSTOOLONG_HI Register EIPE_SA_9_OUTPKTSTOOLONG_HI - EIPE SA 9 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_9_OUTPKTSTOOLONG_HI 0x49C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_9_OUTPKTSTOOLONG_HI 0x849Cu

//! Register Reset Value
#define EIPE_SA_9_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_9_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 10 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO 0x500
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8500u

//! Register Reset Value
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 10 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI 0x504
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8504u

//! Register Reset Value
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_RESERVED2 Register EIPE_SA_10_RESERVED2 - EIPE SA 10 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_RESERVED2 0x508
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_RESERVED2 0x8508u

//! Register Reset Value
#define EIPE_SA_10_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_RESERVED3 Register EIPE_SA_10_RESERVED3 - EIPE SA 10 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_RESERVED3 0x50C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_RESERVED3 0x850Cu

//! Register Reset Value
#define EIPE_SA_10_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 10 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO 0x510
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO 0x8510u

//! Register Reset Value
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 10 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI 0x514
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI 0x8514u

//! Register Reset Value
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTPKTSTOOLONG_LO Register EIPE_SA_10_OUTPKTSTOOLONG_LO - EIPE SA 10 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTPKTSTOOLONG_LO 0x518
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTPKTSTOOLONG_LO 0x8518u

//! Register Reset Value
#define EIPE_SA_10_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_10_OUTPKTSTOOLONG_HI Register EIPE_SA_10_OUTPKTSTOOLONG_HI - EIPE SA 10 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_10_OUTPKTSTOOLONG_HI 0x51C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_10_OUTPKTSTOOLONG_HI 0x851Cu

//! Register Reset Value
#define EIPE_SA_10_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_10_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 11 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO 0x580
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8580u

//! Register Reset Value
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 11 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI 0x584
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8584u

//! Register Reset Value
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_RESERVED2 Register EIPE_SA_11_RESERVED2 - EIPE SA 11 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_RESERVED2 0x588
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_RESERVED2 0x8588u

//! Register Reset Value
#define EIPE_SA_11_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_RESERVED3 Register EIPE_SA_11_RESERVED3 - EIPE SA 11 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_RESERVED3 0x58C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_RESERVED3 0x858Cu

//! Register Reset Value
#define EIPE_SA_11_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 11 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO 0x590
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO 0x8590u

//! Register Reset Value
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 11 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI 0x594
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI 0x8594u

//! Register Reset Value
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTPKTSTOOLONG_LO Register EIPE_SA_11_OUTPKTSTOOLONG_LO - EIPE SA 11 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTPKTSTOOLONG_LO 0x598
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTPKTSTOOLONG_LO 0x8598u

//! Register Reset Value
#define EIPE_SA_11_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_11_OUTPKTSTOOLONG_HI Register EIPE_SA_11_OUTPKTSTOOLONG_HI - EIPE SA 11 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_11_OUTPKTSTOOLONG_HI 0x59C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_11_OUTPKTSTOOLONG_HI 0x859Cu

//! Register Reset Value
#define EIPE_SA_11_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_11_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 12 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO 0x600
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8600u

//! Register Reset Value
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 12 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI 0x604
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8604u

//! Register Reset Value
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_RESERVED2 Register EIPE_SA_12_RESERVED2 - EIPE SA 12 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_RESERVED2 0x608
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_RESERVED2 0x8608u

//! Register Reset Value
#define EIPE_SA_12_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_RESERVED3 Register EIPE_SA_12_RESERVED3 - EIPE SA 12 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_RESERVED3 0x60C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_RESERVED3 0x860Cu

//! Register Reset Value
#define EIPE_SA_12_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 12 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO 0x610
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO 0x8610u

//! Register Reset Value
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 12 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI 0x614
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI 0x8614u

//! Register Reset Value
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTPKTSTOOLONG_LO Register EIPE_SA_12_OUTPKTSTOOLONG_LO - EIPE SA 12 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTPKTSTOOLONG_LO 0x618
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTPKTSTOOLONG_LO 0x8618u

//! Register Reset Value
#define EIPE_SA_12_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_12_OUTPKTSTOOLONG_HI Register EIPE_SA_12_OUTPKTSTOOLONG_HI - EIPE SA 12 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_12_OUTPKTSTOOLONG_HI 0x61C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_12_OUTPKTSTOOLONG_HI 0x861Cu

//! Register Reset Value
#define EIPE_SA_12_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_12_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 13 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO 0x680
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8680u

//! Register Reset Value
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 13 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI 0x684
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8684u

//! Register Reset Value
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_RESERVED2 Register EIPE_SA_13_RESERVED2 - EIPE SA 13 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_RESERVED2 0x688
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_RESERVED2 0x8688u

//! Register Reset Value
#define EIPE_SA_13_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_RESERVED3 Register EIPE_SA_13_RESERVED3 - EIPE SA 13 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_RESERVED3 0x68C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_RESERVED3 0x868Cu

//! Register Reset Value
#define EIPE_SA_13_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 13 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO 0x690
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO 0x8690u

//! Register Reset Value
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 13 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI 0x694
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI 0x8694u

//! Register Reset Value
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTPKTSTOOLONG_LO Register EIPE_SA_13_OUTPKTSTOOLONG_LO - EIPE SA 13 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTPKTSTOOLONG_LO 0x698
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTPKTSTOOLONG_LO 0x8698u

//! Register Reset Value
#define EIPE_SA_13_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_13_OUTPKTSTOOLONG_HI Register EIPE_SA_13_OUTPKTSTOOLONG_HI - EIPE SA 13 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_13_OUTPKTSTOOLONG_HI 0x69C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_13_OUTPKTSTOOLONG_HI 0x869Cu

//! Register Reset Value
#define EIPE_SA_13_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_13_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 14 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO 0x700
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8700u

//! Register Reset Value
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 14 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI 0x704
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8704u

//! Register Reset Value
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_RESERVED2 Register EIPE_SA_14_RESERVED2 - EIPE SA 14 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_RESERVED2 0x708
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_RESERVED2 0x8708u

//! Register Reset Value
#define EIPE_SA_14_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_RESERVED3 Register EIPE_SA_14_RESERVED3 - EIPE SA 14 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_RESERVED3 0x70C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_RESERVED3 0x870Cu

//! Register Reset Value
#define EIPE_SA_14_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 14 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO 0x710
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO 0x8710u

//! Register Reset Value
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 14 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI 0x714
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI 0x8714u

//! Register Reset Value
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTPKTSTOOLONG_LO Register EIPE_SA_14_OUTPKTSTOOLONG_LO - EIPE SA 14 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTPKTSTOOLONG_LO 0x718
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTPKTSTOOLONG_LO 0x8718u

//! Register Reset Value
#define EIPE_SA_14_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_14_OUTPKTSTOOLONG_HI Register EIPE_SA_14_OUTPKTSTOOLONG_HI - EIPE SA 14 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_14_OUTPKTSTOOLONG_HI 0x71C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_14_OUTPKTSTOOLONG_HI 0x871Cu

//! Register Reset Value
#define EIPE_SA_14_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_14_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 15 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO 0x780
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8780u

//! Register Reset Value
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 15 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI 0x784
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8784u

//! Register Reset Value
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_RESERVED2 Register EIPE_SA_15_RESERVED2 - EIPE SA 15 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_RESERVED2 0x788
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_RESERVED2 0x8788u

//! Register Reset Value
#define EIPE_SA_15_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_RESERVED3 Register EIPE_SA_15_RESERVED3 - EIPE SA 15 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_RESERVED3 0x78C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_RESERVED3 0x878Cu

//! Register Reset Value
#define EIPE_SA_15_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 15 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO 0x790
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO 0x8790u

//! Register Reset Value
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 15 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI 0x794
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI 0x8794u

//! Register Reset Value
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTPKTSTOOLONG_LO Register EIPE_SA_15_OUTPKTSTOOLONG_LO - EIPE SA 15 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTPKTSTOOLONG_LO 0x798
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTPKTSTOOLONG_LO 0x8798u

//! Register Reset Value
#define EIPE_SA_15_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_15_OUTPKTSTOOLONG_HI Register EIPE_SA_15_OUTPKTSTOOLONG_HI - EIPE SA 15 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_15_OUTPKTSTOOLONG_HI 0x79C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_15_OUTPKTSTOOLONG_HI 0x879Cu

//! Register Reset Value
#define EIPE_SA_15_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_15_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 16 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO 0x800
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8800u

//! Register Reset Value
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 16 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI 0x804
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8804u

//! Register Reset Value
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_RESERVED2 Register EIPE_SA_16_RESERVED2 - EIPE SA 16 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_RESERVED2 0x808
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_RESERVED2 0x8808u

//! Register Reset Value
#define EIPE_SA_16_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_RESERVED3 Register EIPE_SA_16_RESERVED3 - EIPE SA 16 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_RESERVED3 0x80C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_RESERVED3 0x880Cu

//! Register Reset Value
#define EIPE_SA_16_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 16 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO 0x810
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO 0x8810u

//! Register Reset Value
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 16 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI 0x814
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI 0x8814u

//! Register Reset Value
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTPKTSTOOLONG_LO Register EIPE_SA_16_OUTPKTSTOOLONG_LO - EIPE SA 16 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTPKTSTOOLONG_LO 0x818
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTPKTSTOOLONG_LO 0x8818u

//! Register Reset Value
#define EIPE_SA_16_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_16_OUTPKTSTOOLONG_HI Register EIPE_SA_16_OUTPKTSTOOLONG_HI - EIPE SA 16 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_16_OUTPKTSTOOLONG_HI 0x81C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_16_OUTPKTSTOOLONG_HI 0x881Cu

//! Register Reset Value
#define EIPE_SA_16_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_16_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 17 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO 0x880
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8880u

//! Register Reset Value
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 17 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI 0x884
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8884u

//! Register Reset Value
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_RESERVED2 Register EIPE_SA_17_RESERVED2 - EIPE SA 17 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_RESERVED2 0x888
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_RESERVED2 0x8888u

//! Register Reset Value
#define EIPE_SA_17_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_RESERVED3 Register EIPE_SA_17_RESERVED3 - EIPE SA 17 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_RESERVED3 0x88C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_RESERVED3 0x888Cu

//! Register Reset Value
#define EIPE_SA_17_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 17 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO 0x890
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO 0x8890u

//! Register Reset Value
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 17 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI 0x894
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI 0x8894u

//! Register Reset Value
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTPKTSTOOLONG_LO Register EIPE_SA_17_OUTPKTSTOOLONG_LO - EIPE SA 17 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTPKTSTOOLONG_LO 0x898
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTPKTSTOOLONG_LO 0x8898u

//! Register Reset Value
#define EIPE_SA_17_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_17_OUTPKTSTOOLONG_HI Register EIPE_SA_17_OUTPKTSTOOLONG_HI - EIPE SA 17 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_17_OUTPKTSTOOLONG_HI 0x89C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_17_OUTPKTSTOOLONG_HI 0x889Cu

//! Register Reset Value
#define EIPE_SA_17_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_17_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 18 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO 0x900
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8900u

//! Register Reset Value
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 18 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI 0x904
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8904u

//! Register Reset Value
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_RESERVED2 Register EIPE_SA_18_RESERVED2 - EIPE SA 18 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_RESERVED2 0x908
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_RESERVED2 0x8908u

//! Register Reset Value
#define EIPE_SA_18_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_RESERVED3 Register EIPE_SA_18_RESERVED3 - EIPE SA 18 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_RESERVED3 0x90C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_RESERVED3 0x890Cu

//! Register Reset Value
#define EIPE_SA_18_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 18 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO 0x910
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO 0x8910u

//! Register Reset Value
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 18 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI 0x914
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI 0x8914u

//! Register Reset Value
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTPKTSTOOLONG_LO Register EIPE_SA_18_OUTPKTSTOOLONG_LO - EIPE SA 18 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTPKTSTOOLONG_LO 0x918
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTPKTSTOOLONG_LO 0x8918u

//! Register Reset Value
#define EIPE_SA_18_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_18_OUTPKTSTOOLONG_HI Register EIPE_SA_18_OUTPKTSTOOLONG_HI - EIPE SA 18 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_18_OUTPKTSTOOLONG_HI 0x91C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_18_OUTPKTSTOOLONG_HI 0x891Cu

//! Register Reset Value
#define EIPE_SA_18_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_18_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 19 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO 0x980
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8980u

//! Register Reset Value
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 19 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI 0x984
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8984u

//! Register Reset Value
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_RESERVED2 Register EIPE_SA_19_RESERVED2 - EIPE SA 19 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_RESERVED2 0x988
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_RESERVED2 0x8988u

//! Register Reset Value
#define EIPE_SA_19_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_RESERVED3 Register EIPE_SA_19_RESERVED3 - EIPE SA 19 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_RESERVED3 0x98C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_RESERVED3 0x898Cu

//! Register Reset Value
#define EIPE_SA_19_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 19 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO 0x990
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO 0x8990u

//! Register Reset Value
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 19 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI 0x994
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI 0x8994u

//! Register Reset Value
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTPKTSTOOLONG_LO Register EIPE_SA_19_OUTPKTSTOOLONG_LO - EIPE SA 19 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTPKTSTOOLONG_LO 0x998
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTPKTSTOOLONG_LO 0x8998u

//! Register Reset Value
#define EIPE_SA_19_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_19_OUTPKTSTOOLONG_HI Register EIPE_SA_19_OUTPKTSTOOLONG_HI - EIPE SA 19 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_19_OUTPKTSTOOLONG_HI 0x99C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_19_OUTPKTSTOOLONG_HI 0x899Cu

//! Register Reset Value
#define EIPE_SA_19_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_19_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 20 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO 0xA00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8A00u

//! Register Reset Value
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 20 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI 0xA04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8A04u

//! Register Reset Value
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_RESERVED2 Register EIPE_SA_20_RESERVED2 - EIPE SA 20 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_RESERVED2 0xA08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_RESERVED2 0x8A08u

//! Register Reset Value
#define EIPE_SA_20_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_RESERVED3 Register EIPE_SA_20_RESERVED3 - EIPE SA 20 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_RESERVED3 0xA0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_RESERVED3 0x8A0Cu

//! Register Reset Value
#define EIPE_SA_20_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 20 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO 0xA10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO 0x8A10u

//! Register Reset Value
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 20 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI 0xA14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI 0x8A14u

//! Register Reset Value
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTPKTSTOOLONG_LO Register EIPE_SA_20_OUTPKTSTOOLONG_LO - EIPE SA 20 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTPKTSTOOLONG_LO 0xA18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTPKTSTOOLONG_LO 0x8A18u

//! Register Reset Value
#define EIPE_SA_20_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_20_OUTPKTSTOOLONG_HI Register EIPE_SA_20_OUTPKTSTOOLONG_HI - EIPE SA 20 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_20_OUTPKTSTOOLONG_HI 0xA1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_20_OUTPKTSTOOLONG_HI 0x8A1Cu

//! Register Reset Value
#define EIPE_SA_20_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_20_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 21 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO 0xA80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8A80u

//! Register Reset Value
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 21 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI 0xA84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8A84u

//! Register Reset Value
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_RESERVED2 Register EIPE_SA_21_RESERVED2 - EIPE SA 21 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_RESERVED2 0xA88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_RESERVED2 0x8A88u

//! Register Reset Value
#define EIPE_SA_21_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_RESERVED3 Register EIPE_SA_21_RESERVED3 - EIPE SA 21 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_RESERVED3 0xA8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_RESERVED3 0x8A8Cu

//! Register Reset Value
#define EIPE_SA_21_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 21 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO 0xA90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO 0x8A90u

//! Register Reset Value
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 21 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI 0xA94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI 0x8A94u

//! Register Reset Value
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTPKTSTOOLONG_LO Register EIPE_SA_21_OUTPKTSTOOLONG_LO - EIPE SA 21 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTPKTSTOOLONG_LO 0xA98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTPKTSTOOLONG_LO 0x8A98u

//! Register Reset Value
#define EIPE_SA_21_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_21_OUTPKTSTOOLONG_HI Register EIPE_SA_21_OUTPKTSTOOLONG_HI - EIPE SA 21 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_21_OUTPKTSTOOLONG_HI 0xA9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_21_OUTPKTSTOOLONG_HI 0x8A9Cu

//! Register Reset Value
#define EIPE_SA_21_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_21_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 22 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO 0xB00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8B00u

//! Register Reset Value
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 22 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI 0xB04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8B04u

//! Register Reset Value
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_RESERVED2 Register EIPE_SA_22_RESERVED2 - EIPE SA 22 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_RESERVED2 0xB08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_RESERVED2 0x8B08u

//! Register Reset Value
#define EIPE_SA_22_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_RESERVED3 Register EIPE_SA_22_RESERVED3 - EIPE SA 22 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_RESERVED3 0xB0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_RESERVED3 0x8B0Cu

//! Register Reset Value
#define EIPE_SA_22_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 22 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO 0xB10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO 0x8B10u

//! Register Reset Value
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 22 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI 0xB14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI 0x8B14u

//! Register Reset Value
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTPKTSTOOLONG_LO Register EIPE_SA_22_OUTPKTSTOOLONG_LO - EIPE SA 22 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTPKTSTOOLONG_LO 0xB18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTPKTSTOOLONG_LO 0x8B18u

//! Register Reset Value
#define EIPE_SA_22_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_22_OUTPKTSTOOLONG_HI Register EIPE_SA_22_OUTPKTSTOOLONG_HI - EIPE SA 22 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_22_OUTPKTSTOOLONG_HI 0xB1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_22_OUTPKTSTOOLONG_HI 0x8B1Cu

//! Register Reset Value
#define EIPE_SA_22_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_22_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 23 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO 0xB80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8B80u

//! Register Reset Value
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 23 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI 0xB84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8B84u

//! Register Reset Value
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_RESERVED2 Register EIPE_SA_23_RESERVED2 - EIPE SA 23 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_RESERVED2 0xB88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_RESERVED2 0x8B88u

//! Register Reset Value
#define EIPE_SA_23_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_RESERVED3 Register EIPE_SA_23_RESERVED3 - EIPE SA 23 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_RESERVED3 0xB8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_RESERVED3 0x8B8Cu

//! Register Reset Value
#define EIPE_SA_23_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 23 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO 0xB90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO 0x8B90u

//! Register Reset Value
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 23 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI 0xB94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI 0x8B94u

//! Register Reset Value
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTPKTSTOOLONG_LO Register EIPE_SA_23_OUTPKTSTOOLONG_LO - EIPE SA 23 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTPKTSTOOLONG_LO 0xB98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTPKTSTOOLONG_LO 0x8B98u

//! Register Reset Value
#define EIPE_SA_23_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_23_OUTPKTSTOOLONG_HI Register EIPE_SA_23_OUTPKTSTOOLONG_HI - EIPE SA 23 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_23_OUTPKTSTOOLONG_HI 0xB9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_23_OUTPKTSTOOLONG_HI 0x8B9Cu

//! Register Reset Value
#define EIPE_SA_23_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_23_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 24 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO 0xC00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8C00u

//! Register Reset Value
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 24 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI 0xC04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8C04u

//! Register Reset Value
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_RESERVED2 Register EIPE_SA_24_RESERVED2 - EIPE SA 24 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_RESERVED2 0xC08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_RESERVED2 0x8C08u

//! Register Reset Value
#define EIPE_SA_24_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_RESERVED3 Register EIPE_SA_24_RESERVED3 - EIPE SA 24 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_RESERVED3 0xC0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_RESERVED3 0x8C0Cu

//! Register Reset Value
#define EIPE_SA_24_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 24 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO 0xC10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO 0x8C10u

//! Register Reset Value
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 24 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI 0xC14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI 0x8C14u

//! Register Reset Value
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTPKTSTOOLONG_LO Register EIPE_SA_24_OUTPKTSTOOLONG_LO - EIPE SA 24 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTPKTSTOOLONG_LO 0xC18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTPKTSTOOLONG_LO 0x8C18u

//! Register Reset Value
#define EIPE_SA_24_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_24_OUTPKTSTOOLONG_HI Register EIPE_SA_24_OUTPKTSTOOLONG_HI - EIPE SA 24 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_24_OUTPKTSTOOLONG_HI 0xC1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_24_OUTPKTSTOOLONG_HI 0x8C1Cu

//! Register Reset Value
#define EIPE_SA_24_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_24_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 25 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO 0xC80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8C80u

//! Register Reset Value
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 25 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI 0xC84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8C84u

//! Register Reset Value
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_RESERVED2 Register EIPE_SA_25_RESERVED2 - EIPE SA 25 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_RESERVED2 0xC88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_RESERVED2 0x8C88u

//! Register Reset Value
#define EIPE_SA_25_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_RESERVED3 Register EIPE_SA_25_RESERVED3 - EIPE SA 25 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_RESERVED3 0xC8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_RESERVED3 0x8C8Cu

//! Register Reset Value
#define EIPE_SA_25_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 25 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO 0xC90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO 0x8C90u

//! Register Reset Value
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 25 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI 0xC94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI 0x8C94u

//! Register Reset Value
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTPKTSTOOLONG_LO Register EIPE_SA_25_OUTPKTSTOOLONG_LO - EIPE SA 25 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTPKTSTOOLONG_LO 0xC98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTPKTSTOOLONG_LO 0x8C98u

//! Register Reset Value
#define EIPE_SA_25_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_25_OUTPKTSTOOLONG_HI Register EIPE_SA_25_OUTPKTSTOOLONG_HI - EIPE SA 25 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_25_OUTPKTSTOOLONG_HI 0xC9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_25_OUTPKTSTOOLONG_HI 0x8C9Cu

//! Register Reset Value
#define EIPE_SA_25_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_25_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 26 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO 0xD00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8D00u

//! Register Reset Value
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 26 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI 0xD04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8D04u

//! Register Reset Value
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_RESERVED2 Register EIPE_SA_26_RESERVED2 - EIPE SA 26 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_RESERVED2 0xD08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_RESERVED2 0x8D08u

//! Register Reset Value
#define EIPE_SA_26_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_RESERVED3 Register EIPE_SA_26_RESERVED3 - EIPE SA 26 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_RESERVED3 0xD0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_RESERVED3 0x8D0Cu

//! Register Reset Value
#define EIPE_SA_26_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 26 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO 0xD10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO 0x8D10u

//! Register Reset Value
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 26 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI 0xD14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI 0x8D14u

//! Register Reset Value
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTPKTSTOOLONG_LO Register EIPE_SA_26_OUTPKTSTOOLONG_LO - EIPE SA 26 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTPKTSTOOLONG_LO 0xD18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTPKTSTOOLONG_LO 0x8D18u

//! Register Reset Value
#define EIPE_SA_26_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_26_OUTPKTSTOOLONG_HI Register EIPE_SA_26_OUTPKTSTOOLONG_HI - EIPE SA 26 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_26_OUTPKTSTOOLONG_HI 0xD1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_26_OUTPKTSTOOLONG_HI 0x8D1Cu

//! Register Reset Value
#define EIPE_SA_26_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_26_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 27 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO 0xD80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8D80u

//! Register Reset Value
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 27 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI 0xD84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8D84u

//! Register Reset Value
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_RESERVED2 Register EIPE_SA_27_RESERVED2 - EIPE SA 27 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_RESERVED2 0xD88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_RESERVED2 0x8D88u

//! Register Reset Value
#define EIPE_SA_27_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_RESERVED3 Register EIPE_SA_27_RESERVED3 - EIPE SA 27 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_RESERVED3 0xD8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_RESERVED3 0x8D8Cu

//! Register Reset Value
#define EIPE_SA_27_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 27 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO 0xD90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO 0x8D90u

//! Register Reset Value
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 27 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI 0xD94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI 0x8D94u

//! Register Reset Value
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTPKTSTOOLONG_LO Register EIPE_SA_27_OUTPKTSTOOLONG_LO - EIPE SA 27 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTPKTSTOOLONG_LO 0xD98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTPKTSTOOLONG_LO 0x8D98u

//! Register Reset Value
#define EIPE_SA_27_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_27_OUTPKTSTOOLONG_HI Register EIPE_SA_27_OUTPKTSTOOLONG_HI - EIPE SA 27 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_27_OUTPKTSTOOLONG_HI 0xD9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_27_OUTPKTSTOOLONG_HI 0x8D9Cu

//! Register Reset Value
#define EIPE_SA_27_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_27_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 28 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO 0xE00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8E00u

//! Register Reset Value
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 28 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI 0xE04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8E04u

//! Register Reset Value
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_RESERVED2 Register EIPE_SA_28_RESERVED2 - EIPE SA 28 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_RESERVED2 0xE08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_RESERVED2 0x8E08u

//! Register Reset Value
#define EIPE_SA_28_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_RESERVED3 Register EIPE_SA_28_RESERVED3 - EIPE SA 28 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_RESERVED3 0xE0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_RESERVED3 0x8E0Cu

//! Register Reset Value
#define EIPE_SA_28_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 28 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO 0xE10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO 0x8E10u

//! Register Reset Value
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 28 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI 0xE14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI 0x8E14u

//! Register Reset Value
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTPKTSTOOLONG_LO Register EIPE_SA_28_OUTPKTSTOOLONG_LO - EIPE SA 28 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTPKTSTOOLONG_LO 0xE18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTPKTSTOOLONG_LO 0x8E18u

//! Register Reset Value
#define EIPE_SA_28_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_28_OUTPKTSTOOLONG_HI Register EIPE_SA_28_OUTPKTSTOOLONG_HI - EIPE SA 28 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_28_OUTPKTSTOOLONG_HI 0xE1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_28_OUTPKTSTOOLONG_HI 0x8E1Cu

//! Register Reset Value
#define EIPE_SA_28_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_28_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 29 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO 0xE80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8E80u

//! Register Reset Value
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 29 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI 0xE84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8E84u

//! Register Reset Value
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_RESERVED2 Register EIPE_SA_29_RESERVED2 - EIPE SA 29 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_RESERVED2 0xE88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_RESERVED2 0x8E88u

//! Register Reset Value
#define EIPE_SA_29_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_RESERVED3 Register EIPE_SA_29_RESERVED3 - EIPE SA 29 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_RESERVED3 0xE8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_RESERVED3 0x8E8Cu

//! Register Reset Value
#define EIPE_SA_29_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 29 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO 0xE90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO 0x8E90u

//! Register Reset Value
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 29 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI 0xE94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI 0x8E94u

//! Register Reset Value
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTPKTSTOOLONG_LO Register EIPE_SA_29_OUTPKTSTOOLONG_LO - EIPE SA 29 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTPKTSTOOLONG_LO 0xE98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTPKTSTOOLONG_LO 0x8E98u

//! Register Reset Value
#define EIPE_SA_29_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_29_OUTPKTSTOOLONG_HI Register EIPE_SA_29_OUTPKTSTOOLONG_HI - EIPE SA 29 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_29_OUTPKTSTOOLONG_HI 0xE9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_29_OUTPKTSTOOLONG_HI 0x8E9Cu

//! Register Reset Value
#define EIPE_SA_29_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_29_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 30 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO 0xF00
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8F00u

//! Register Reset Value
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 30 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI 0xF04
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8F04u

//! Register Reset Value
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_RESERVED2 Register EIPE_SA_30_RESERVED2 - EIPE SA 30 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_RESERVED2 0xF08
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_RESERVED2 0x8F08u

//! Register Reset Value
#define EIPE_SA_30_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_RESERVED3 Register EIPE_SA_30_RESERVED3 - EIPE SA 30 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_RESERVED3 0xF0C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_RESERVED3 0x8F0Cu

//! Register Reset Value
#define EIPE_SA_30_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 30 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO 0xF10
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO 0x8F10u

//! Register Reset Value
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 30 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI 0xF14
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI 0x8F14u

//! Register Reset Value
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTPKTSTOOLONG_LO Register EIPE_SA_30_OUTPKTSTOOLONG_LO - EIPE SA 30 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTPKTSTOOLONG_LO 0xF18
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTPKTSTOOLONG_LO 0x8F18u

//! Register Reset Value
#define EIPE_SA_30_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_30_OUTPKTSTOOLONG_HI Register EIPE_SA_30_OUTPKTSTOOLONG_HI - EIPE SA 30 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_30_OUTPKTSTOOLONG_HI 0xF1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_30_OUTPKTSTOOLONG_HI 0x8F1Cu

//! Register Reset Value
#define EIPE_SA_30_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_30_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO Register EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO - EIPE SA 31 OutOctetsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO 0xF80
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO 0x8F80u

//! Register Reset Value
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI Register EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI - EIPE SA 31 OutOctetsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI 0xF84
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI 0x8F84u

//! Register Reset Value
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTOCTETSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_RESERVED2 Register EIPE_SA_31_RESERVED2 - EIPE SA 31 Reserved2
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_RESERVED2 0xF88
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_RESERVED2 0x8F88u

//! Register Reset Value
#define EIPE_SA_31_RESERVED2_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_RESERVED2_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_RESERVED2_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_RESERVED3 Register EIPE_SA_31_RESERVED3 - EIPE SA 31 Reserved3
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_RESERVED3 0xF8C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_RESERVED3 0x8F8Cu

//! Register Reset Value
#define EIPE_SA_31_RESERVED3_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_RESERVED3_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_RESERVED3_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO Register EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO - EIPE SA 31 OutPktsEncryptedProtected lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO 0xF90
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO 0x8F90u

//! Register Reset Value
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI Register EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI - EIPE SA 31 OutPktsEncryptedProtected hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI 0xF94
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI 0x8F94u

//! Register Reset Value
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSENCRYPTEDPROTECTED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTPKTSTOOLONG_LO Register EIPE_SA_31_OUTPKTSTOOLONG_LO - EIPE SA 31 OutPktsTooLong lo
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTPKTSTOOLONG_LO 0xF98
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTPKTSTOOLONG_LO 0x8F98u

//! Register Reset Value
#define EIPE_SA_31_OUTPKTSTOOLONG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSTOOLONG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSTOOLONG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_SA_31_OUTPKTSTOOLONG_HI Register EIPE_SA_31_OUTPKTSTOOLONG_HI - EIPE SA 31 OutPktsTooLong hi
//! @{

//! Register Offset (relative)
#define EIPE_SA_31_OUTPKTSTOOLONG_HI 0xF9C
//! Register Offset (absolute) for 1st Instance EGR_STAT_SA_PAGE0
#define EGR_REG_STAT_SA_31_OUTPKTSTOOLONG_HI 0x8F9Cu

//! Register Reset Value
#define EIPE_SA_31_OUTPKTSTOOLONG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSTOOLONG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_SA_31_OUTPKTSTOOLONG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
