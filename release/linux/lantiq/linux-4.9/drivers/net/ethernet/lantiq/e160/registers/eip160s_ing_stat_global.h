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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_Stat_global_def.xml
// Register File Name  : ING__STAT_GLOBAL
// Register File Title : EIPI EIP_160s_i_32_Stat_global
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING__STAT_GLOBAL_H
#define _ING__STAT_GLOBAL_H

//! \defgroup ING__STAT_GLOBAL Register File ING__STAT_GLOBAL - EIPI EIP_160s_i_32_Stat_global
//! @{

//! Base Address of ING__STAT_GLOBAL
#define ING_STAT_GLOBAL_MODULE_BASE 0xC400u

//! \defgroup EIPI_TRANSFORMERRORPKTS_LO Register EIPI_TRANSFORMERRORPKTS_LO - EIPI TransformErrorPkts lo
//! @{

//! Register Offset (relative)
#define EIPI_TRANSFORMERRORPKTS_LO 0x0
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_TRANSFORMERRORPKTS_LO 0xC400u

//! Register Reset Value
#define EIPI_TRANSFORMERRORPKTS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_TRANSFORMERRORPKTS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_TRANSFORMERRORPKTS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_TRANSFORMERRORPKTS_HI Register EIPI_TRANSFORMERRORPKTS_HI - EIPI TransformErrorPkts hi
//! @{

//! Register Offset (relative)
#define EIPI_TRANSFORMERRORPKTS_HI 0x4
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_TRANSFORMERRORPKTS_HI 0xC404u

//! Register Reset Value
#define EIPI_TRANSFORMERRORPKTS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_TRANSFORMERRORPKTS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_TRANSFORMERRORPKTS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSCTRL_LO Register EIPI_INPKTSCTRL_LO - EIPI InPktsCtrl lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSCTRL_LO 0x8
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSCTRL_LO 0xC408u

//! Register Reset Value
#define EIPI_INPKTSCTRL_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSCTRL_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSCTRL_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSCTRL_HI Register EIPI_INPKTSCTRL_HI - EIPI InPktsCtrl hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSCTRL_HI 0xC
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSCTRL_HI 0xC40Cu

//! Register Reset Value
#define EIPI_INPKTSCTRL_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSCTRL_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSCTRL_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSNOTAG_LO Register EIPI_INPKTSNOTAG_LO - EIPI InPktsNoTag lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSNOTAG_LO 0x10
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSNOTAG_LO 0xC410u

//! Register Reset Value
#define EIPI_INPKTSNOTAG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSNOTAG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSNOTAG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSNOTAG_HI Register EIPI_INPKTSNOTAG_HI - EIPI InPktsNoTag hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSNOTAG_HI 0x14
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSNOTAG_HI 0xC414u

//! Register Reset Value
#define EIPI_INPKTSNOTAG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSNOTAG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSNOTAG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNTAGGED_LO Register EIPI_INPKTSUNTAGGED_LO - EIPI InPktsUntagged lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNTAGGED_LO 0x18
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNTAGGED_LO 0xC418u

//! Register Reset Value
#define EIPI_INPKTSUNTAGGED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNTAGGED_HI Register EIPI_INPKTSUNTAGGED_HI - EIPI InPktsUntagged hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNTAGGED_HI 0x1C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNTAGGED_HI 0xC41Cu

//! Register Reset Value
#define EIPI_INPKTSUNTAGGED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSTAGGED_LO Register EIPI_INPKTSTAGGED_LO - EIPI InPktsTagged lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSTAGGED_LO 0x20
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSTAGGED_LO 0xC420u

//! Register Reset Value
#define EIPI_INPKTSTAGGED_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSTAGGED_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSTAGGED_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSTAGGED_HI Register EIPI_INPKTSTAGGED_HI - EIPI InPktsTagged hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSTAGGED_HI 0x24
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSTAGGED_HI 0xC424u

//! Register Reset Value
#define EIPI_INPKTSTAGGED_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSTAGGED_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSTAGGED_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSBADTAG_LO Register EIPI_INPKTSBADTAG_LO - EIPI InPktsBadTag lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSBADTAG_LO 0x28
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSBADTAG_LO 0xC428u

//! Register Reset Value
#define EIPI_INPKTSBADTAG_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSBADTAG_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSBADTAG_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSBADTAG_HI Register EIPI_INPKTSBADTAG_HI - EIPI InPktsBadTag hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSBADTAG_HI 0x2C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSBADTAG_HI 0xC42Cu

//! Register Reset Value
#define EIPI_INPKTSBADTAG_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSBADTAG_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSBADTAG_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNTAGGEDMISS_LO Register EIPI_INPKTSUNTAGGEDMISS_LO - EIPI InPktsUntaggedMiss lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNTAGGEDMISS_LO 0x30
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNTAGGEDMISS_LO 0xC430u

//! Register Reset Value
#define EIPI_INPKTSUNTAGGEDMISS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGEDMISS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGEDMISS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNTAGGEDMISS_HI Register EIPI_INPKTSUNTAGGEDMISS_HI - EIPI InPktsUntaggedMiss hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNTAGGEDMISS_HI 0x34
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNTAGGEDMISS_HI 0xC434u

//! Register Reset Value
#define EIPI_INPKTSUNTAGGEDMISS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGEDMISS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNTAGGEDMISS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSNOSCI_LO Register EIPI_INPKTSNOSCI_LO - EIPI InPktsNoSCI lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSNOSCI_LO 0x38
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSNOSCI_LO 0xC438u

//! Register Reset Value
#define EIPI_INPKTSNOSCI_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSNOSCI_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSNOSCI_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSNOSCI_HI Register EIPI_INPKTSNOSCI_HI - EIPI InPktsNoSCI hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSNOSCI_HI 0x3C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSNOSCI_HI 0xC43Cu

//! Register Reset Value
#define EIPI_INPKTSNOSCI_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSNOSCI_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSNOSCI_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNKNOWNSCI_LO Register EIPI_INPKTSUNKNOWNSCI_LO - EIPI InPktsUnknownSCI lo
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNKNOWNSCI_LO 0x40
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNKNOWNSCI_LO 0xC440u

//! Register Reset Value
#define EIPI_INPKTSUNKNOWNSCI_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNKNOWNSCI_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNKNOWNSCI_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INPKTSUNKNOWNSCI_HI Register EIPI_INPKTSUNKNOWNSCI_HI - EIPI InPktsUnknownSCI hi
//! @{

//! Register Offset (relative)
#define EIPI_INPKTSUNKNOWNSCI_HI 0x44
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INPKTSUNKNOWNSCI_HI 0xC444u

//! Register Reset Value
#define EIPI_INPKTSUNKNOWNSCI_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INPKTSUNKNOWNSCI_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INPKTSUNKNOWNSCI_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO Register EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO - EIPI InConsistCheckControlledNotPass lo
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO 0x48
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKCONTROLLEDNOTPASS_LO 0xC448u

//! Register Reset Value
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI Register EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI - EIPI InConsistCheckControlledNotPass hi
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI 0x4C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKCONTROLLEDNOTPASS_HI 0xC44Cu

//! Register Reset Value
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDNOTPASS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO Register EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO - EIPI InConsistCheckUncontrolledNotPass lo
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO 0x50
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO 0xC450u

//! Register Reset Value
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI Register EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI - EIPI InConsistCheckUncontrolledNotPass hi
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI 0x54
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI 0xC454u

//! Register Reset Value
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDNOTPASS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKCONTROLLEDPASS_LO Register EIPI_INCONSISTCHECKCONTROLLEDPASS_LO - EIPI InConsistCheckControlledPass lo
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_LO 0x58
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKCONTROLLEDPASS_LO 0xC458u

//! Register Reset Value
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKCONTROLLEDPASS_HI Register EIPI_INCONSISTCHECKCONTROLLEDPASS_HI - EIPI InConsistCheckControlledPass hi
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_HI 0x5C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKCONTROLLEDPASS_HI 0xC45Cu

//! Register Reset Value
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKCONTROLLEDPASS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO Register EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO - EIPI InConsistCheckUncontrolledPass lo
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO 0x60
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKUNCONTROLLEDPASS_LO 0xC460u

//! Register Reset Value
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI Register EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI - EIPI InConsistCheckUncontrolledPass hi
//! @{

//! Register Offset (relative)
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI 0x64
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INCONSISTCHECKUNCONTROLLEDPASS_HI 0xC464u

//! Register Reset Value
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INCONSISTCHECKUNCONTROLLEDPASS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INOVERSIZEPKTS_LO Register EIPI_INOVERSIZEPKTS_LO - EIPI InOverSizePkts lo
//! @{

//! Register Offset (relative)
#define EIPI_INOVERSIZEPKTS_LO 0x68
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INOVERSIZEPKTS_LO 0xC468u

//! Register Reset Value
#define EIPI_INOVERSIZEPKTS_LO_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INOVERSIZEPKTS_LO_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INOVERSIZEPKTS_LO_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EIPI_INOVERSIZEPKTS_HI Register EIPI_INOVERSIZEPKTS_HI - EIPI InOverSizePkts hi
//! @{

//! Register Offset (relative)
#define EIPI_INOVERSIZEPKTS_HI 0x6C
//! Register Offset (absolute) for 1st Instance ING__STAT_GLOBAL
#define ING_REG_STAT_GLOBAL_INOVERSIZEPKTS_HI 0xC46Cu

//! Register Reset Value
#define EIPI_INOVERSIZEPKTS_HI_RST 0x00000000u

//! Field COUNTER - No Content.
#define EIPI_INOVERSIZEPKTS_HI_COUNTER_POS 0
//! Field COUNTER - No Content.
#define EIPI_INOVERSIZEPKTS_HI_COUNTER_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
