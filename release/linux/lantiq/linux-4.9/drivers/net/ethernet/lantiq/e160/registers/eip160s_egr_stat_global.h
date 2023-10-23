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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPE_EIP_160s_e_32_Stat_global_def.xml
// Register File Name  : EGR_STAT_GLOBAL
// Register File Title : EIPE EIP_160s_e_32_Stat_global
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _EGR_STAT_GLOBAL_H
#define _EGR_STAT_GLOBAL_H

//! \defgroup EGR_STAT_GLOBAL Register File EGR_STAT_GLOBAL - EIPE EIP_160s_e_32_Stat_global
//! @{

//! Base Address of EGR_STAT_GLOBAL
#define EGR_STAT_GLOBAL_MODULE_BASE 0xC400u

//! \defgroup EIPE_TRANSFORMERRORPKTS_LO Register EIPE_TRANSFORMERRORPKTS_LO - EIPE TransformErrorPkts lo
//! @{

//! Register Offset (relative)
#define EIPE_TRANSFORMERRORPKTS_LO 0x0
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_TRANSFORMERRORPKTS_LO 0xC400u

//! Register Reset Value
#define EIPE_TRANSFORMERRORPKTS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_TRANSFORMERRORPKTS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_TRANSFORMERRORPKTS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_TRANSFORMERRORPKTS_HI Register EIPE_TRANSFORMERRORPKTS_HI - EIPE TransformErrorPkts hi
//! @{

//! Register Offset (relative)
#define EIPE_TRANSFORMERRORPKTS_HI 0x4
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_TRANSFORMERRORPKTS_HI 0xC404u

//! Register Reset Value
#define EIPE_TRANSFORMERRORPKTS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_TRANSFORMERRORPKTS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_TRANSFORMERRORPKTS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSCTRL_LO Register EIPE_OUTPKTSCTRL_LO - EIPE OutPktsCtrl lo
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSCTRL_LO 0x8
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSCTRL_LO 0xC408u

//! Register Reset Value
#define EIPE_OUTPKTSCTRL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSCTRL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSCTRL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSCTRL_HI Register EIPE_OUTPKTSCTRL_HI - EIPE OutPktsCtrl hi
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSCTRL_HI 0xC
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSCTRL_HI 0xC40Cu

//! Register Reset Value
#define EIPE_OUTPKTSCTRL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSCTRL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSCTRL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSUNKNOWNSA_LO Register EIPE_OUTPKTSUNKNOWNSA_LO - EIPE OutPktsUnknownSA lo
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSUNKNOWNSA_LO 0x10
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSUNKNOWNSA_LO 0xC410u

//! Register Reset Value
#define EIPE_OUTPKTSUNKNOWNSA_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNKNOWNSA_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNKNOWNSA_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSUNKNOWNSA_HI Register EIPE_OUTPKTSUNKNOWNSA_HI - EIPE OutPktsUnknownSA hi
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSUNKNOWNSA_HI 0x14
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSUNKNOWNSA_HI 0xC414u

//! Register Reset Value
#define EIPE_OUTPKTSUNKNOWNSA_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNKNOWNSA_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNKNOWNSA_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSUNTAGGED_LO Register EIPE_OUTPKTSUNTAGGED_LO - EIPE OutPktsUntagged lo
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSUNTAGGED_LO 0x18
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSUNTAGGED_LO 0xC418u

//! Register Reset Value
#define EIPE_OUTPKTSUNTAGGED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNTAGGED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNTAGGED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTPKTSUNTAGGED_HI Register EIPE_OUTPKTSUNTAGGED_HI - EIPE OutPktsUntagged hi
//! @{

//! Register Offset (relative)
#define EIPE_OUTPKTSUNTAGGED_HI 0x1C
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTPKTSUNTAGGED_HI 0xC41Cu

//! Register Reset Value
#define EIPE_OUTPKTSUNTAGGED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNTAGGED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTPKTSUNTAGGED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTOVERSIZEPKTS_LO Register EIPE_OUTOVERSIZEPKTS_LO - EIPE OutOverSizePkts lo
//! @{

//! Register Offset (relative)
#define EIPE_OUTOVERSIZEPKTS_LO 0x20
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTOVERSIZEPKTS_LO 0xC420u

//! Register Reset Value
#define EIPE_OUTOVERSIZEPKTS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTOVERSIZEPKTS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTOVERSIZEPKTS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPE_OUTOVERSIZEPKTS_HI Register EIPE_OUTOVERSIZEPKTS_HI - EIPE OutOverSizePkts hi
//! @{

//! Register Offset (relative)
#define EIPE_OUTOVERSIZEPKTS_HI 0x24
//! Register Offset (absolute) for 1st Instance EGR_STAT_GLOBAL
#define EGR_REG_STAT_GLOBAL_OUTOVERSIZEPKTS_HI 0xC424u

//! Register Reset Value
#define EIPE_OUTOVERSIZEPKTS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPE_OUTOVERSIZEPKTS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPE_OUTOVERSIZEPKTS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
