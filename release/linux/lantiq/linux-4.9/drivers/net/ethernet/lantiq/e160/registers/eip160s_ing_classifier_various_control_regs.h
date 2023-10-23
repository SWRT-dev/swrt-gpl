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
// LSD Source          : /home/p34x/p34x_chip/v_emouchel.priv.p34x_chip.registers/ipg_lsd/lsd_sys/source_32b/xml/reg_files/slice_pm/EIPI_EIP_160s_i_32_Classifier_various_control_regs_def.xml
// Register File Name  : ING_CLASSIFIER_VARIOUS_CONTROL_REGS
// Register File Title : EIPI EIP_160s_i_32_Classifier_various_control_regs
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _ING_CLASSIFIER_VARIOUS_CONTROL_REGS_H
#define _ING_CLASSIFIER_VARIOUS_CONTROL_REGS_H

//! \defgroup ING_CLASSIFIER_VARIOUS_CONTROL_REGS Register File ING_CLASSIFIER_VARIOUS_CONTROL_REGS - EIPI EIP_160s_i_32_Classifier_various_control_regs
//! @{

//! Base Address of ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_CLASSIFIER_VARIOUS_CONTROL_REGS_MODULE_BASE 0x7900u

//! \defgroup EIPI_SAM_CP_TAG Register EIPI_SAM_CP_TAG - EIPI SAM CP TAG
//! @{

//! Register Offset (relative)
#define EIPI_SAM_CP_TAG 0x0
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_CP_TAG 0x7900u

//! Register Reset Value
#define EIPI_SAM_CP_TAG_RST 0x00000000u

//! Field MAP_TBL - No Content.
#define EIPI_SAM_CP_TAG_MAP_TBL_POS 0
//! Field MAP_TBL - No Content.
#define EIPI_SAM_CP_TAG_MAP_TBL_MASK 0xFFFFFFu

//! Field DEF_UP - No Content.
#define EIPI_SAM_CP_TAG_DEF_UP_POS 24
//! Field DEF_UP - No Content.
#define EIPI_SAM_CP_TAG_DEF_UP_MASK 0x7000000u

//! Field STAG_UP_EN - No Content.
#define EIPI_SAM_CP_TAG_STAG_UP_EN_POS 27
//! Field STAG_UP_EN - No Content.
#define EIPI_SAM_CP_TAG_STAG_UP_EN_MASK 0x8000000u

//! Field QTAG_UP_EN - No Content.
#define EIPI_SAM_CP_TAG_QTAG_UP_EN_POS 28
//! Field QTAG_UP_EN - No Content.
#define EIPI_SAM_CP_TAG_QTAG_UP_EN_MASK 0x10000000u

//! Field PARSE_QINQ - No Content.
#define EIPI_SAM_CP_TAG_PARSE_QINQ_POS 29
//! Field PARSE_QINQ - No Content.
#define EIPI_SAM_CP_TAG_PARSE_QINQ_MASK 0x20000000u

//! Field PARSE_STAG - No Content.
#define EIPI_SAM_CP_TAG_PARSE_STAG_POS 30
//! Field PARSE_STAG - No Content.
#define EIPI_SAM_CP_TAG_PARSE_STAG_MASK 0x40000000u

//! Field PARSE_QTAG - No Content.
#define EIPI_SAM_CP_TAG_PARSE_QTAG_POS 31
//! Field PARSE_QTAG - No Content.
#define EIPI_SAM_CP_TAG_PARSE_QTAG_MASK 0x80000000u

//! @}

//! \defgroup EIPI_SAM_PP_TAGS Register EIPI_SAM_PP_TAGS - EIPI SAM PP TAGS
//! @{

//! Register Offset (relative)
#define EIPI_SAM_PP_TAGS 0x4
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_PP_TAGS 0x7904u

//! Register Reset Value
#define EIPI_SAM_PP_TAGS_RST 0xA8880081u

//! Field QTAG_VALUE - No Content.
#define EIPI_SAM_PP_TAGS_QTAG_VALUE_POS 0
//! Field QTAG_VALUE - No Content.
#define EIPI_SAM_PP_TAGS_QTAG_VALUE_MASK 0xFFFFu

//! Field STAG_VALUE - No Content.
#define EIPI_SAM_PP_TAGS_STAG_VALUE_POS 16
//! Field STAG_VALUE - No Content.
#define EIPI_SAM_PP_TAGS_STAG_VALUE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_PP_TAGS2 Register EIPI_SAM_PP_TAGS2 - EIPI SAM PP TAGS2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_PP_TAGS2 0x8
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_PP_TAGS2 0x7908u

//! Register Reset Value
#define EIPI_SAM_PP_TAGS2_RST 0x00920091u

//! Field STAG_VALUE2 - No Content.
#define EIPI_SAM_PP_TAGS2_STAG_VALUE2_POS 0
//! Field STAG_VALUE2 - No Content.
#define EIPI_SAM_PP_TAGS2_STAG_VALUE2_MASK 0xFFFFu

//! Field STAG_VALUE3 - No Content.
#define EIPI_SAM_PP_TAGS2_STAG_VALUE3_POS 16
//! Field STAG_VALUE3 - No Content.
#define EIPI_SAM_PP_TAGS2_STAG_VALUE3_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_CP_TAG2 Register EIPI_SAM_CP_TAG2 - EIPI SAM CP TAG2
//! @{

//! Register Offset (relative)
#define EIPI_SAM_CP_TAG2 0xC
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_CP_TAG2 0x790Cu

//! Register Reset Value
#define EIPI_SAM_CP_TAG2_RST 0x00000000u

//! Field MAP_TBL2 - No Content.
#define EIPI_SAM_CP_TAG2_MAP_TBL2_POS 0
//! Field MAP_TBL2 - No Content.
#define EIPI_SAM_CP_TAG2_MAP_TBL2_MASK 0xFFFFFFu

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_CP_TAG2_RESERVED_0_POS 24
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_CP_TAG2_RESERVED_0_MASK 0xFF000000u

//! @}

//! \defgroup EIPI_SAM_NM_PARAMS Register EIPI_SAM_NM_PARAMS - EIPI SAM NM PARAMS
//! @{

//! Register Offset (relative)
#define EIPI_SAM_NM_PARAMS 0x40
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_NM_PARAMS 0x7940u

//! Register Reset Value
#define EIPI_SAM_NM_PARAMS_RST 0xE588007Fu

//! Field COMP_ETYPE - No Content.
#define EIPI_SAM_NM_PARAMS_COMP_ETYPE_POS 0
//! Field COMP_ETYPE - No Content.
#define EIPI_SAM_NM_PARAMS_COMP_ETYPE_MASK 0x1u

//! Field CHECK_V - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_V_POS 1
//! Field CHECK_V - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_V_MASK 0x2u

//! Field CHECK_KAY - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_KAY_POS 2
//! Field CHECK_KAY - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_KAY_MASK 0x4u

//! Field CHECK_C_E - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_C_E_POS 3
//! Field CHECK_C_E - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_C_E_MASK 0x8u

//! Field CHECK_SC - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_SC_POS 4
//! Field CHECK_SC - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_SC_MASK 0x10u

//! Field CHECK_SL - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_SL_POS 5
//! Field CHECK_SL - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_SL_MASK 0x20u

//! Field CHECK_PN - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_PN_POS 6
//! Field CHECK_PN - No Content.
#define EIPI_SAM_NM_PARAMS_CHECK_PN_MASK 0x40u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_PARAMS_RESERVED_0_POS 7
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_PARAMS_RESERVED_0_MASK 0xFF80u

//! Field MSEC_TAG_VALUE - No Content.
#define EIPI_SAM_NM_PARAMS_MSEC_TAG_VALUE_POS 16
//! Field MSEC_TAG_VALUE - No Content.
#define EIPI_SAM_NM_PARAMS_MSEC_TAG_VALUE_MASK 0xFFFF0000u

//! @}

//! \defgroup EIPI_SAM_NM_FLOW_NCP Register EIPI_SAM_NM_FLOW_NCP - EIPI SAM NM FLOW NCP
//! @{

//! Register Offset (relative)
#define EIPI_SAM_NM_FLOW_NCP 0x44
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_NM_FLOW_NCP 0x7944u

//! Register Reset Value
#define EIPI_SAM_NM_FLOW_NCP_RST 0x00000000u

//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE0_POS 0
//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE0_MASK 0x1u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_0_POS 1
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_0_MASK 0x2u

//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT0_POS 2
//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT0_MASK 0xCu

//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED0_POS 4
//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED0_MASK 0x10u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_1_POS 5
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_1_MASK 0x20u

//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION0_POS 6
//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION0_MASK 0xC0u

//! Field FLOW_TYPE1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE1_POS 8
//! Field FLOW_TYPE1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE1_MASK 0x100u

//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_2_POS 9
//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_2_MASK 0x200u

//! Field DEST_PORT1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT1_POS 10
//! Field DEST_PORT1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT1_MASK 0xC00u

//! Field DROP_NON_RESERVED1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED1_POS 12
//! Field DROP_NON_RESERVED1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED1_MASK 0x1000u

//! Field RESERVED_3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_3_POS 13
//! Field RESERVED_3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_3_MASK 0x2000u

//! Field DROP_ACTION1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION1_POS 14
//! Field DROP_ACTION1 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION1_MASK 0xC000u

//! Field FLOW_TYPE2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE2_POS 16
//! Field FLOW_TYPE2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE2_MASK 0x10000u

//! Field RESERVED_4 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_4_POS 17
//! Field RESERVED_4 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_4_MASK 0x20000u

//! Field DEST_PORT2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT2_POS 18
//! Field DEST_PORT2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT2_MASK 0xC0000u

//! Field DROP_NON_RESERVED2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED2_POS 20
//! Field DROP_NON_RESERVED2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED2_MASK 0x100000u

//! Field RESERVED_5 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_5_POS 21
//! Field RESERVED_5 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_5_MASK 0x200000u

//! Field DROP_ACTION2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION2_POS 22
//! Field DROP_ACTION2 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION2_MASK 0xC00000u

//! Field FLOW_TYPE3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE3_POS 24
//! Field FLOW_TYPE3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_FLOW_TYPE3_MASK 0x1000000u

//! Field RESERVED_6 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_6_POS 25
//! Field RESERVED_6 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_6_MASK 0x2000000u

//! Field DEST_PORT3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT3_POS 26
//! Field DEST_PORT3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DEST_PORT3_MASK 0xC000000u

//! Field DROP_NON_RESERVED3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED3_POS 28
//! Field DROP_NON_RESERVED3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_NON_RESERVED3_MASK 0x10000000u

//! Field RESERVED_7 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_7_POS 29
//! Field RESERVED_7 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_RESERVED_7_MASK 0x20000000u

//! Field DROP_ACTION3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION3_POS 30
//! Field DROP_ACTION3 - No Content.
#define EIPI_SAM_NM_FLOW_NCP_DROP_ACTION3_MASK 0xC0000000u

//! @}

//! \defgroup EIPI_SAM_NM_FLOW_CP Register EIPI_SAM_NM_FLOW_CP - EIPI SAM NM FLOW CP
//! @{

//! Register Offset (relative)
#define EIPI_SAM_NM_FLOW_CP 0x48
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_NM_FLOW_CP 0x7948u

//! Register Reset Value
#define EIPI_SAM_NM_FLOW_CP_RST 0x00000000u

//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE0_POS 0
//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE0_MASK 0x1u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_0_POS 1
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_0_MASK 0x2u

//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT0_POS 2
//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT0_MASK 0xCu

//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED0_POS 4
//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED0_MASK 0x10u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_1_POS 5
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_1_MASK 0x20u

//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION0_POS 6
//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION0_MASK 0xC0u

//! Field FLOW_TYPE1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE1_POS 8
//! Field FLOW_TYPE1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE1_MASK 0x100u

//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_2_POS 9
//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_2_MASK 0x200u

//! Field DEST_PORT1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT1_POS 10
//! Field DEST_PORT1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT1_MASK 0xC00u

//! Field DROP_NON_RESERVED1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED1_POS 12
//! Field DROP_NON_RESERVED1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED1_MASK 0x1000u

//! Field RESERVED_3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_3_POS 13
//! Field RESERVED_3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_3_MASK 0x2000u

//! Field DROP_ACTION1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION1_POS 14
//! Field DROP_ACTION1 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION1_MASK 0xC000u

//! Field FLOW_TYPE2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE2_POS 16
//! Field FLOW_TYPE2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE2_MASK 0x10000u

//! Field RESERVED_4 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_4_POS 17
//! Field RESERVED_4 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_4_MASK 0x20000u

//! Field DEST_PORT2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT2_POS 18
//! Field DEST_PORT2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT2_MASK 0xC0000u

//! Field DROP_NON_RESERVED2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED2_POS 20
//! Field DROP_NON_RESERVED2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED2_MASK 0x100000u

//! Field RESERVED_5 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_5_POS 21
//! Field RESERVED_5 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_5_MASK 0x200000u

//! Field DROP_ACTION2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION2_POS 22
//! Field DROP_ACTION2 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION2_MASK 0xC00000u

//! Field FLOW_TYPE3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE3_POS 24
//! Field FLOW_TYPE3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_FLOW_TYPE3_MASK 0x1000000u

//! Field RESERVED_6 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_6_POS 25
//! Field RESERVED_6 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_6_MASK 0x2000000u

//! Field DEST_PORT3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT3_POS 26
//! Field DEST_PORT3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DEST_PORT3_MASK 0xC000000u

//! Field DROP_NON_RESERVED3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED3_POS 28
//! Field DROP_NON_RESERVED3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_NON_RESERVED3_MASK 0x10000000u

//! Field RESERVED_7 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_7_POS 29
//! Field RESERVED_7 - No Content.
#define EIPI_SAM_NM_FLOW_CP_RESERVED_7_MASK 0x20000000u

//! Field DROP_ACTION3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION3_POS 30
//! Field DROP_ACTION3 - No Content.
#define EIPI_SAM_NM_FLOW_CP_DROP_ACTION3_MASK 0xC0000000u

//! @}

//! \defgroup EIPI_SAM_NM_FLOW_EXPL Register EIPI_SAM_NM_FLOW_EXPL - EIPI SAM NM FLOW EXPL
//! @{

//! Register Offset (relative)
#define EIPI_SAM_NM_FLOW_EXPL 0x4C
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_SAM_NM_FLOW_EXPL 0x794Cu

//! Register Reset Value
#define EIPI_SAM_NM_FLOW_EXPL_RST 0x00000000u

//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_FLOW_TYPE0_POS 0
//! Field FLOW_TYPE0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_FLOW_TYPE0_MASK 0x1u

//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_0_POS 1
//! Field RESERVED_0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_0_MASK 0x2u

//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DEST_PORT0_POS 2
//! Field DEST_PORT0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DEST_PORT0_MASK 0xCu

//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DROP_NON_RESERVED0_POS 4
//! Field DROP_NON_RESERVED0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DROP_NON_RESERVED0_MASK 0x10u

//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_1_POS 5
//! Field RESERVED_1 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_1_MASK 0x20u

//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DROP_ACTION0_POS 6
//! Field DROP_ACTION0 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_DROP_ACTION0_MASK 0xC0u

//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_2_POS 8
//! Field RESERVED_2 - No Content.
#define EIPI_SAM_NM_FLOW_EXPL_RESERVED_2_MASK 0xFFFFFF00u

//! @}

//! \defgroup EIPI_MISC_CONTROL Register EIPI_MISC_CONTROL - EIPI MISC CONTROL
//! @{

//! Register Offset (relative)
#define EIPI_MISC_CONTROL 0x7C
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_MISC_CONTROL 0x797Cu

//! Register Reset Value
#define EIPI_MISC_CONTROL_RST 0x00000000u

//! Field MC_LATENCY_FIX_7_0 - No Content.
#define EIPI_MISC_CONTROL_MC_LATENCY_FIX_7_0_POS 0
//! Field MC_LATENCY_FIX_7_0 - No Content.
#define EIPI_MISC_CONTROL_MC_LATENCY_FIX_7_0_MASK 0xFFu

//! Field STATIC_BYPASS - No Content.
#define EIPI_MISC_CONTROL_STATIC_BYPASS_POS 8
//! Field STATIC_BYPASS - No Content.
#define EIPI_MISC_CONTROL_STATIC_BYPASS_MASK 0x100u

//! Field NM_MACSEC_EN - No Content.
#define EIPI_MISC_CONTROL_NM_MACSEC_EN_POS 9
//! Field NM_MACSEC_EN - No Content.
#define EIPI_MISC_CONTROL_NM_MACSEC_EN_MASK 0x200u

//! Field VALIDATE_FRAMES - No Content.
#define EIPI_MISC_CONTROL_VALIDATE_FRAMES_POS 10
//! Field VALIDATE_FRAMES - No Content.
#define EIPI_MISC_CONTROL_VALIDATE_FRAMES_MASK 0xC00u

//! Field SECTAG_AFTER_VLAN - No Content.
#define EIPI_MISC_CONTROL_SECTAG_AFTER_VLAN_POS 12
//! Field SECTAG_AFTER_VLAN - No Content.
#define EIPI_MISC_CONTROL_SECTAG_AFTER_VLAN_MASK 0x1000u

//! Field RESERVED_0 - No Content.
#define EIPI_MISC_CONTROL_RESERVED_0_POS 13
//! Field RESERVED_0 - No Content.
#define EIPI_MISC_CONTROL_RESERVED_0_MASK 0xFFE000u

//! Field XFORM_REC_SIZE - No Content.
#define EIPI_MISC_CONTROL_XFORM_REC_SIZE_POS 24
//! Field XFORM_REC_SIZE - No Content.
#define EIPI_MISC_CONTROL_XFORM_REC_SIZE_MASK 0x3000000u

//! Field MC_LATENCY_FIX_13_8 - No Content.
#define EIPI_MISC_CONTROL_MC_LATENCY_FIX_13_8_POS 26
//! Field MC_LATENCY_FIX_13_8 - No Content.
#define EIPI_MISC_CONTROL_MC_LATENCY_FIX_13_8_MASK 0xFC000000u

//! @}

//! \defgroup EIPI_CRYPT_AUTH_CTRL Register EIPI_CRYPT_AUTH_CTRL - EIPI CRYPT AUTH CTRL
//! @{

//! Register Offset (relative)
#define EIPI_CRYPT_AUTH_CTRL 0x84
//! Register Offset (absolute) for 1st Instance ING_CLASSIFIER_VARIOUS_CONTROL_REGS
#define ING_REG_CLF_VARIOUS_CTRL_CRYPT_AUTH_CTRL 0x7984u

//! Register Reset Value
#define EIPI_CRYPT_AUTH_CTRL_RST 0x00000000u

//! Field BYPASS_LEN - No Content.
#define EIPI_CRYPT_AUTH_CTRL_BYPASS_LEN_POS 0
//! Field BYPASS_LEN - No Content.
#define EIPI_CRYPT_AUTH_CTRL_BYPASS_LEN_MASK 0xFFu

//! Field RESERVED_0 - No Content.
#define EIPI_CRYPT_AUTH_CTRL_RESERVED_0_POS 8
//! Field RESERVED_0 - No Content.
#define EIPI_CRYPT_AUTH_CTRL_RESERVED_0_MASK 0xFFFFFF00u

//! @}

//! @}

#endif
