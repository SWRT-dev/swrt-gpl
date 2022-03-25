//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/CBM_EQM.xml
// Register File Name  : CQEM_ENQ
// Register File Title : Central QoS Manager - Enqueue Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CQEM_ENQ_H
#define _CQEM_ENQ_H

//! \defgroup CQEM_ENQ Register File CQEM_ENQ - Central QoS Manager - Enqueue Register Description
//! @{

//! Base Address of CQEM_ENQ
#define CQEM_ENQ_MODULE_BASE 0x19080000u

//! \defgroup CBM_EQM_CTRL Register CBM_EQM_CTRL - CBM Enqueue Manager Control Register
//! @{

//! Register Offset (relative)
#define CBM_EQM_CTRL 0x0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CBM_EQM_CTRL 0x19080000u

//! Register Reset Value
#define CBM_EQM_CTRL_RST 0x00001E00u

//! Field EQM_EN - Enable Enqueue Manager Core State Machine
#define CBM_EQM_CTRL_EQM_EN_POS 0
//! Field EQM_EN - Enable Enqueue Manager Core State Machine
#define CBM_EQM_CTRL_EQM_EN_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_EQM_CTRL_EQM_EN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_EQM_CTRL_EQM_EN_EN 0x1

//! Field EQM_FRZ - Freeze Enqueue Manager Core State Machine
#define CBM_EQM_CTRL_EQM_FRZ_POS 1
//! Field EQM_FRZ - Freeze Enqueue Manager Core State Machine
#define CBM_EQM_CTRL_EQM_FRZ_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_EQM_CTRL_EQM_FRZ_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_EQM_CTRL_EQM_FRZ_EN 0x1

//! Field EQM_ACT - Enqueue Manager Activity Status
#define CBM_EQM_CTRL_EQM_ACT_POS 2
//! Field EQM_ACT - Enqueue Manager Activity Status
#define CBM_EQM_CTRL_EQM_ACT_MASK 0x4u
//! Constant INACTIVE - Inactive
#define CONST_CBM_EQM_CTRL_EQM_ACT_INACTIVE 0x0
//! Constant ACTIVE - Active
#define CONST_CBM_EQM_CTRL_EQM_ACT_ACTIVE 0x1

//! Field SNOOPEN - Snoop Enable
#define CBM_EQM_CTRL_SNOOPEN_POS 6
//! Field SNOOPEN - Snoop Enable
#define CBM_EQM_CTRL_SNOOPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_EQM_CTRL_SNOOPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_EQM_CTRL_SNOOPEN_EN 0x1

//! Field PDEN - Programmable Delay Enable
#define CBM_EQM_CTRL_PDEN_POS 8
//! Field PDEN - Programmable Delay Enable
#define CBM_EQM_CTRL_PDEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CBM_EQM_CTRL_PDEN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_EQM_CTRL_PDEN_EN 0x1

//! Field L - Queue Length Threshold
#define CBM_EQM_CTRL_L_POS 9
//! Field L - Queue Length Threshold
#define CBM_EQM_CTRL_L_MASK 0x1FE00u

//! @}

//! \defgroup DBG_EQM_0 Register DBG_EQM_0 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_EQM_0 0x10
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DBG_EQM_0 0x19080010u

//! Register Reset Value
#define DBG_EQM_0_RST 0x00000000u

//! Field DBG - Debug
#define DBG_EQM_0_DBG_POS 0
//! Field DBG - Debug
#define DBG_EQM_0_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DBG_EQM_1 Register DBG_EQM_1 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_EQM_1 0x14
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DBG_EQM_1 0x19080014u

//! Register Reset Value
#define DBG_EQM_1_RST 0xC00004A0u

//! Field DBG - Debug
#define DBG_EQM_1_DBG_POS 0
//! Field DBG - Debug
#define DBG_EQM_1_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DBG_EQM_2 Register DBG_EQM_2 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_EQM_2 0x18
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DBG_EQM_2 0x19080018u

//! Register Reset Value
#define DBG_EQM_2_RST 0x00000000u

//! Field DBG - Debug
#define DBG_EQM_2_DBG_POS 0
//! Field DBG - Debug
#define DBG_EQM_2_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TEST_EQM_0 Register TEST_EQM_0 - Hardware Test Register
//! @{

//! Register Offset (relative)
#define TEST_EQM_0 0x20
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_TEST_EQM_0 0x19080020u

//! Register Reset Value
#define TEST_EQM_0_RST 0x00000000u

//! Field TEST - Test
#define TEST_EQM_0_TEST_POS 0
//! Field TEST - Test
#define TEST_EQM_0_TEST_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TEST_EQM_1 Register TEST_EQM_1 - Hardware Test Register
//! @{

//! Register Offset (relative)
#define TEST_EQM_1 0x24
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_TEST_EQM_1 0x19080024u

//! Register Reset Value
#define TEST_EQM_1_RST 0x00000000u

//! Field TEST - Test
#define TEST_EQM_1_TEST_POS 0
//! Field TEST - Test
#define TEST_EQM_1_TEST_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DEN_0 Register DEN_0 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_0 0x30
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_0 0x19080030u

//! Register Reset Value
#define DEN_0_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_0_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_0_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_0_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_0_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_0_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_0_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_0_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_0_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_0_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_0_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_0_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_0_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_0_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_0_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_0_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_0_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_0_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_0_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_0_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_0_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_0_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_0_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_0_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_0_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_0_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_0_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_0_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_0_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_0_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_0_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_0_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_0_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_0_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_0_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_0_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_0_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_0_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_0_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_0_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_0_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_0_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_0_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_0_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_0_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_0_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_0_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_0_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_0_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_0_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_0_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_0_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_0_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_0_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_0_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_0_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_0_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_0_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_0_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_0_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_0_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_0_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_0_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_0_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_0_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_0_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_0_DEN31_EN 0x1

//! @}

//! \defgroup DEN_1 Register DEN_1 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_1 0x34
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_1 0x19080034u

//! Register Reset Value
#define DEN_1_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_1_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_1_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_1_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_1_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_1_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_1_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_1_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_1_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_1_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_1_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_1_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_1_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_1_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_1_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_1_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_1_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_1_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_1_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_1_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_1_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_1_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_1_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_1_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_1_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_1_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_1_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_1_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_1_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_1_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_1_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_1_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_1_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_1_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_1_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_1_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_1_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_1_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_1_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_1_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_1_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_1_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_1_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_1_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_1_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_1_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_1_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_1_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_1_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_1_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_1_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_1_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_1_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_1_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_1_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_1_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_1_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_1_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_1_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_1_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_1_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_1_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_1_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_1_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_1_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_1_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_1_DEN31_EN 0x1

//! @}

//! \defgroup DEN_2 Register DEN_2 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_2 0x38
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_2 0x19080038u

//! Register Reset Value
#define DEN_2_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_2_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_2_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_2_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_2_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_2_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_2_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_2_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_2_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_2_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_2_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_2_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_2_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_2_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_2_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_2_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_2_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_2_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_2_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_2_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_2_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_2_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_2_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_2_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_2_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_2_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_2_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_2_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_2_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_2_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_2_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_2_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_2_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_2_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_2_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_2_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_2_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_2_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_2_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_2_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_2_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_2_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_2_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_2_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_2_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_2_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_2_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_2_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_2_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_2_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_2_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_2_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_2_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_2_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_2_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_2_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_2_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_2_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_2_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_2_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_2_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_2_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_2_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_2_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_2_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_2_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_2_DEN31_EN 0x1

//! @}

//! \defgroup DEN_3 Register DEN_3 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_3 0x3C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_3 0x1908003Cu

//! Register Reset Value
#define DEN_3_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_3_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_3_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_3_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_3_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_3_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_3_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_3_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_3_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_3_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_3_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_3_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_3_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_3_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_3_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_3_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_3_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_3_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_3_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_3_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_3_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_3_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_3_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_3_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_3_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_3_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_3_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_3_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_3_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_3_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_3_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_3_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_3_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_3_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_3_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_3_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_3_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_3_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_3_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_3_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_3_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_3_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_3_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_3_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_3_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_3_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_3_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_3_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_3_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_3_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_3_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_3_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_3_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_3_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_3_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_3_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_3_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_3_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_3_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_3_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_3_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_3_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_3_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_3_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_3_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_3_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_3_DEN31_EN 0x1

//! @}

//! \defgroup DEN_4 Register DEN_4 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_4 0x40
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_4 0x19080040u

//! Register Reset Value
#define DEN_4_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_4_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_4_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_4_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_4_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_4_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_4_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_4_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_4_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_4_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_4_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_4_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_4_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_4_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_4_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_4_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_4_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_4_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_4_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_4_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_4_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_4_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_4_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_4_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_4_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_4_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_4_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_4_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_4_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_4_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_4_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_4_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_4_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_4_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_4_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_4_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_4_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_4_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_4_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_4_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_4_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_4_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_4_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_4_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_4_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_4_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_4_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_4_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_4_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_4_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_4_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_4_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_4_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_4_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_4_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_4_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_4_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_4_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_4_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_4_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_4_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_4_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_4_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_4_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_4_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_4_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_4_DEN31_EN 0x1

//! @}

//! \defgroup DEN_5 Register DEN_5 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_5 0x44
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_5 0x19080044u

//! Register Reset Value
#define DEN_5_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_5_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_5_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_5_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_5_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_5_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_5_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_5_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_5_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_5_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_5_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_5_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_5_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_5_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_5_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_5_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_5_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_5_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_5_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_5_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_5_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_5_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_5_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_5_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_5_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_5_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_5_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_5_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_5_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_5_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_5_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_5_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_5_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_5_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_5_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_5_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_5_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_5_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_5_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_5_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_5_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_5_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_5_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_5_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_5_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_5_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_5_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_5_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_5_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_5_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_5_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_5_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_5_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_5_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_5_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_5_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_5_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_5_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_5_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_5_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_5_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_5_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_5_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_5_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_5_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_5_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_5_DEN31_EN 0x1

//! @}

//! \defgroup DEN_6 Register DEN_6 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_6 0x48
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_6 0x19080048u

//! Register Reset Value
#define DEN_6_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_6_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_6_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_6_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_6_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_6_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_6_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_6_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_6_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_6_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_6_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_6_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_6_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_6_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_6_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_6_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_6_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_6_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_6_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_6_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_6_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_6_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_6_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_6_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_6_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_6_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_6_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_6_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_6_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_6_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_6_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_6_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_6_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_6_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_6_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_6_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_6_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_6_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_6_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_6_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_6_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_6_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_6_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_6_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_6_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_6_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_6_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_6_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_6_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_6_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_6_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_6_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_6_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_6_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_6_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_6_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_6_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_6_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_6_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_6_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_6_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_6_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_6_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_6_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_6_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_6_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_6_DEN31_EN 0x1

//! @}

//! \defgroup DEN_7 Register DEN_7 - Delay Enable Register
//! @{

//! Register Offset (relative)
#define DEN_7 0x4C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DEN_7 0x1908004Cu

//! Register Reset Value
#define DEN_7_RST 0xFFFFFFFFu

//! Field DEN0 - Delay Enable 0
#define DEN_7_DEN0_POS 0
//! Field DEN0 - Delay Enable 0
#define DEN_7_DEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN0_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN0_EN 0x1

//! Field DEN1 - Delay Enable 1
#define DEN_7_DEN1_POS 1
//! Field DEN1 - Delay Enable 1
#define DEN_7_DEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN1_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN1_EN 0x1

//! Field DEN2 - Delay Enable 2
#define DEN_7_DEN2_POS 2
//! Field DEN2 - Delay Enable 2
#define DEN_7_DEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN2_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN2_EN 0x1

//! Field DEN3 - Delay Enable 3
#define DEN_7_DEN3_POS 3
//! Field DEN3 - Delay Enable 3
#define DEN_7_DEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN3_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN3_EN 0x1

//! Field DEN4 - Delay Enable 4
#define DEN_7_DEN4_POS 4
//! Field DEN4 - Delay Enable 4
#define DEN_7_DEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN4_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN4_EN 0x1

//! Field DEN5 - Delay Enable 5
#define DEN_7_DEN5_POS 5
//! Field DEN5 - Delay Enable 5
#define DEN_7_DEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN5_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN5_EN 0x1

//! Field DEN6 - Delay Enable 6
#define DEN_7_DEN6_POS 6
//! Field DEN6 - Delay Enable 6
#define DEN_7_DEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN6_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN6_EN 0x1

//! Field DEN7 - Delay Enable 7
#define DEN_7_DEN7_POS 7
//! Field DEN7 - Delay Enable 7
#define DEN_7_DEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN7_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN7_EN 0x1

//! Field DEN8 - Delay Enable 8
#define DEN_7_DEN8_POS 8
//! Field DEN8 - Delay Enable 8
#define DEN_7_DEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN8_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN8_EN 0x1

//! Field DEN9 - Delay Enable 9
#define DEN_7_DEN9_POS 9
//! Field DEN9 - Delay Enable 9
#define DEN_7_DEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN9_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN9_EN 0x1

//! Field DEN10 - Delay Enable 10
#define DEN_7_DEN10_POS 10
//! Field DEN10 - Delay Enable 10
#define DEN_7_DEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN10_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN10_EN 0x1

//! Field DEN11 - Delay Enable 11
#define DEN_7_DEN11_POS 11
//! Field DEN11 - Delay Enable 11
#define DEN_7_DEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN11_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN11_EN 0x1

//! Field DEN12 - Delay Enable 12
#define DEN_7_DEN12_POS 12
//! Field DEN12 - Delay Enable 12
#define DEN_7_DEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN12_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN12_EN 0x1

//! Field DEN13 - Delay Enable 13
#define DEN_7_DEN13_POS 13
//! Field DEN13 - Delay Enable 13
#define DEN_7_DEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN13_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN13_EN 0x1

//! Field DEN14 - Delay Enable 14
#define DEN_7_DEN14_POS 14
//! Field DEN14 - Delay Enable 14
#define DEN_7_DEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN14_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN14_EN 0x1

//! Field DEN15 - Delay Enable 15
#define DEN_7_DEN15_POS 15
//! Field DEN15 - Delay Enable 15
#define DEN_7_DEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN15_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN15_EN 0x1

//! Field DEN16 - Delay Enable 16
#define DEN_7_DEN16_POS 16
//! Field DEN16 - Delay Enable 16
#define DEN_7_DEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN16_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN16_EN 0x1

//! Field DEN17 - Delay Enable 17
#define DEN_7_DEN17_POS 17
//! Field DEN17 - Delay Enable 17
#define DEN_7_DEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN17_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN17_EN 0x1

//! Field DEN18 - Delay Enable 18
#define DEN_7_DEN18_POS 18
//! Field DEN18 - Delay Enable 18
#define DEN_7_DEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN18_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN18_EN 0x1

//! Field DEN19 - Delay Enable 19
#define DEN_7_DEN19_POS 19
//! Field DEN19 - Delay Enable 19
#define DEN_7_DEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN19_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN19_EN 0x1

//! Field DEN20 - Delay Enable 20
#define DEN_7_DEN20_POS 20
//! Field DEN20 - Delay Enable 20
#define DEN_7_DEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN20_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN20_EN 0x1

//! Field DEN21 - Delay Enable 21
#define DEN_7_DEN21_POS 21
//! Field DEN21 - Delay Enable 21
#define DEN_7_DEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN21_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN21_EN 0x1

//! Field DEN22 - Delay Enable 22
#define DEN_7_DEN22_POS 22
//! Field DEN22 - Delay Enable 22
#define DEN_7_DEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN22_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN22_EN 0x1

//! Field DEN23 - Delay Enable 23
#define DEN_7_DEN23_POS 23
//! Field DEN23 - Delay Enable 23
#define DEN_7_DEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN23_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN23_EN 0x1

//! Field DEN24 - Delay Enable 24
#define DEN_7_DEN24_POS 24
//! Field DEN24 - Delay Enable 24
#define DEN_7_DEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN24_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN24_EN 0x1

//! Field DEN25 - Delay Enable 25
#define DEN_7_DEN25_POS 25
//! Field DEN25 - Delay Enable 25
#define DEN_7_DEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN25_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN25_EN 0x1

//! Field DEN26 - Delay Enable 26
#define DEN_7_DEN26_POS 26
//! Field DEN26 - Delay Enable 26
#define DEN_7_DEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN26_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN26_EN 0x1

//! Field DEN27 - Delay Enable 27
#define DEN_7_DEN27_POS 27
//! Field DEN27 - Delay Enable 27
#define DEN_7_DEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN27_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN27_EN 0x1

//! Field DEN28 - Delay Enable 28
#define DEN_7_DEN28_POS 28
//! Field DEN28 - Delay Enable 28
#define DEN_7_DEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN28_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN28_EN 0x1

//! Field DEN29 - Delay Enable 29
#define DEN_7_DEN29_POS 29
//! Field DEN29 - Delay Enable 29
#define DEN_7_DEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN29_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN29_EN 0x1

//! Field DEN30 - Delay Enable 30
#define DEN_7_DEN30_POS 30
//! Field DEN30 - Delay Enable 30
#define DEN_7_DEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN30_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN30_EN 0x1

//! Field DEN31 - Delay Enable 31
#define DEN_7_DEN31_POS 31
//! Field DEN31 - Delay Enable 31
#define DEN_7_DEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_DEN_7_DEN31_DIS 0x0
//! Constant EN - EN
#define CONST_DEN_7_DEN31_EN 0x1

//! @}

//! \defgroup OVH_0 Register OVH_0 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_0 0x50
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_0 0x19080050u

//! Register Reset Value
#define OVH_0_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_0_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_0_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_1 Register OVH_1 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_1 0x54
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_1 0x19080054u

//! Register Reset Value
#define OVH_1_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_1_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_1_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_2 Register OVH_2 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_2 0x58
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_2 0x19080058u

//! Register Reset Value
#define OVH_2_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_2_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_2_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_3 Register OVH_3 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_3 0x5C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_3 0x1908005Cu

//! Register Reset Value
#define OVH_3_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_3_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_3_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_4 Register OVH_4 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_4 0x60
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_4 0x19080060u

//! Register Reset Value
#define OVH_4_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_4_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_4_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_5 Register OVH_5 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_5 0x64
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_5 0x19080064u

//! Register Reset Value
#define OVH_5_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_5_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_5_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_6 Register OVH_6 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_6 0x68
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_6 0x19080068u

//! Register Reset Value
#define OVH_6_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_6_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_6_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_7 Register OVH_7 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_7 0x6C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_7 0x1908006Cu

//! Register Reset Value
#define OVH_7_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_7_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_7_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_8 Register OVH_8 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_8 0x70
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_8 0x19080070u

//! Register Reset Value
#define OVH_8_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_8_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_8_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_9 Register OVH_9 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_9 0x74
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_9 0x19080074u

//! Register Reset Value
#define OVH_9_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_9_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_9_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_10 Register OVH_10 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_10 0x78
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_10 0x19080078u

//! Register Reset Value
#define OVH_10_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_10_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_10_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_11 Register OVH_11 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_11 0x7C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_11 0x1908007Cu

//! Register Reset Value
#define OVH_11_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_11_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_11_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_12 Register OVH_12 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_12 0x80
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_12 0x19080080u

//! Register Reset Value
#define OVH_12_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_12_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_12_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_13 Register OVH_13 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_13 0x84
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_13 0x19080084u

//! Register Reset Value
#define OVH_13_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_13_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_13_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_14 Register OVH_14 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_14 0x88
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_14 0x19080088u

//! Register Reset Value
#define OVH_14_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_14_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_14_OVH_MASK 0xFFu

//! @}

//! \defgroup OVH_15 Register OVH_15 - Overhead Byte
//! @{

//! Register Offset (relative)
#define OVH_15 0x8C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_OVH_15 0x1908008Cu

//! Register Reset Value
#define OVH_15_RST 0x00000000u

//! Field OVH - Overhead Bytes
#define OVH_15_OVH_POS 0
//! Field OVH - Overhead Bytes
#define OVH_15_OVH_MASK 0xFFu

//! @}

//! \defgroup DMA_RDY_EN Register DMA_RDY_EN - DMA Ready Enable
//! @{

//! Register Offset (relative)
#define DMA_RDY_EN 0x90
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DMA_RDY_EN 0x19080090u

//! Register Reset Value
#define DMA_RDY_EN_RST 0x0000003Fu

//! Field RDY_EN0 - Channel PB Ready Enable
#define DMA_RDY_EN_RDY_EN0_POS 0
//! Field RDY_EN0 - Channel PB Ready Enable
#define DMA_RDY_EN_RDY_EN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN0_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN0_EN 0x1

//! Field RDY_EN1 - Channel Size0 Ready Enable
#define DMA_RDY_EN_RDY_EN1_POS 1
//! Field RDY_EN1 - Channel Size0 Ready Enable
#define DMA_RDY_EN_RDY_EN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN1_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN1_EN 0x1

//! Field RDY_EN2 - Channel Size1 Ready Enable
#define DMA_RDY_EN_RDY_EN2_POS 2
//! Field RDY_EN2 - Channel Size1 Ready Enable
#define DMA_RDY_EN_RDY_EN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN2_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN2_EN 0x1

//! Field RDY_EN3 - Channel Size2 Ready Enable
#define DMA_RDY_EN_RDY_EN3_POS 3
//! Field RDY_EN3 - Channel Size2 Ready Enable
#define DMA_RDY_EN_RDY_EN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN3_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN3_EN 0x1

//! Field RDY_EN4 - Channel Size3 Ready Enable
#define DMA_RDY_EN_RDY_EN4_POS 4
//! Field RDY_EN4 - Channel Size3 Ready Enable
#define DMA_RDY_EN_RDY_EN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN4_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN4_EN 0x1

//! Field RDY_EN5 - Channel PB Ready Enable
#define DMA_RDY_EN_RDY_EN5_POS 5
//! Field RDY_EN5 - Channel PB Ready Enable
#define DMA_RDY_EN_RDY_EN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN5_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN5_EN 0x1

//! Field RDY_EN6 - Channel Size0 Ready Enable
#define DMA_RDY_EN_RDY_EN6_POS 6
//! Field RDY_EN6 - Channel Size0 Ready Enable
#define DMA_RDY_EN_RDY_EN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN6_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN6_EN 0x1

//! Field RDY_EN7 - Channel Size1 Ready Enable
#define DMA_RDY_EN_RDY_EN7_POS 7
//! Field RDY_EN7 - Channel Size1 Ready Enable
#define DMA_RDY_EN_RDY_EN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN7_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN7_EN 0x1

//! Field RDY_EN8 - Channel Size2 Ready Enable
#define DMA_RDY_EN_RDY_EN8_POS 8
//! Field RDY_EN8 - Channel Size2 Ready Enable
#define DMA_RDY_EN_RDY_EN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN8_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN8_EN 0x1

//! Field RDY_EN9 - Channel Size3 Ready Enable
#define DMA_RDY_EN_RDY_EN9_POS 9
//! Field RDY_EN9 - Channel Size3 Ready Enable
#define DMA_RDY_EN_RDY_EN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN9_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN9_EN 0x1

//! Field RDY_EN10 - Channel Size2 (Policy 4) Ready Enable
#define DMA_RDY_EN_RDY_EN10_POS 10
//! Field RDY_EN10 - Channel Size2 (Policy 4) Ready Enable
#define DMA_RDY_EN_RDY_EN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN10_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN10_EN 0x1

//! Field RDY_EN11 - Channel Size2 (Policy 4) Ready Enable
#define DMA_RDY_EN_RDY_EN11_POS 11
//! Field RDY_EN11 - Channel Size2 (Policy 4) Ready Enable
#define DMA_RDY_EN_RDY_EN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_RDY_EN11_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_RDY_EN11_EN 0x1

//! @}

//! \defgroup EP_THRSD_EN_0 Register EP_THRSD_EN_0 - Egress Port Threshold Enable
//! @{

//! Register Offset (relative)
#define EP_THRSD_EN_0 0x100
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EP_THRSD_EN_0 0x19080100u

//! Register Reset Value
#define EP_THRSD_EN_0_RST 0xFFFFFFFFu

//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_0_EPEN0_POS 0
//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_0_EPEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN0_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN0_EN 0x1

//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_0_EPEN1_POS 1
//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_0_EPEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN1_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN1_EN 0x1

//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_0_EPEN2_POS 2
//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_0_EPEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN2_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN2_EN 0x1

//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_0_EPEN3_POS 3
//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_0_EPEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN3_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN3_EN 0x1

//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_0_EPEN4_POS 4
//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_0_EPEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN4_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN4_EN 0x1

//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_0_EPEN5_POS 5
//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_0_EPEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN5_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN5_EN 0x1

//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_0_EPEN6_POS 6
//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_0_EPEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN6_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN6_EN 0x1

//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_0_EPEN7_POS 7
//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_0_EPEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN7_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN7_EN 0x1

//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_0_EPEN8_POS 8
//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_0_EPEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN8_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN8_EN 0x1

//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_0_EPEN9_POS 9
//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_0_EPEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN9_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN9_EN 0x1

//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_0_EPEN10_POS 10
//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_0_EPEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN10_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN10_EN 0x1

//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_0_EPEN11_POS 11
//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_0_EPEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN11_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN11_EN 0x1

//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_0_EPEN12_POS 12
//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_0_EPEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN12_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN12_EN 0x1

//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_0_EPEN13_POS 13
//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_0_EPEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN13_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN13_EN 0x1

//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_0_EPEN14_POS 14
//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_0_EPEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN14_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN14_EN 0x1

//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_0_EPEN15_POS 15
//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_0_EPEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN15_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN15_EN 0x1

//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_0_EPEN16_POS 16
//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_0_EPEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN16_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN16_EN 0x1

//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_0_EPEN17_POS 17
//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_0_EPEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN17_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN17_EN 0x1

//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_0_EPEN18_POS 18
//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_0_EPEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN18_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN18_EN 0x1

//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_0_EPEN19_POS 19
//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_0_EPEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN19_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN19_EN 0x1

//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_0_EPEN20_POS 20
//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_0_EPEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN20_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN20_EN 0x1

//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_0_EPEN21_POS 21
//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_0_EPEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN21_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN21_EN 0x1

//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_0_EPEN22_POS 22
//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_0_EPEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN22_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN22_EN 0x1

//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_0_EPEN23_POS 23
//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_0_EPEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN23_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN23_EN 0x1

//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_0_EPEN24_POS 24
//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_0_EPEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN24_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN24_EN 0x1

//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_0_EPEN25_POS 25
//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_0_EPEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN25_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN25_EN 0x1

//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_0_EPEN26_POS 26
//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_0_EPEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN26_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN26_EN 0x1

//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_0_EPEN27_POS 27
//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_0_EPEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN27_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN27_EN 0x1

//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_0_EPEN28_POS 28
//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_0_EPEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN28_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN28_EN 0x1

//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_0_EPEN29_POS 29
//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_0_EPEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN29_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN29_EN 0x1

//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_0_EPEN30_POS 30
//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_0_EPEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN30_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN30_EN 0x1

//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_0_EPEN31_POS 31
//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_0_EPEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_0_EPEN31_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_0_EPEN31_EN 0x1

//! @}

//! \defgroup EP_THRSD_EN_1 Register EP_THRSD_EN_1 - Egress Port Threshold Enable
//! @{

//! Register Offset (relative)
#define EP_THRSD_EN_1 0x104
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EP_THRSD_EN_1 0x19080104u

//! Register Reset Value
#define EP_THRSD_EN_1_RST 0xFFFFFFFFu

//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_1_EPEN0_POS 0
//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_1_EPEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN0_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN0_EN 0x1

//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_1_EPEN1_POS 1
//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_1_EPEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN1_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN1_EN 0x1

//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_1_EPEN2_POS 2
//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_1_EPEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN2_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN2_EN 0x1

//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_1_EPEN3_POS 3
//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_1_EPEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN3_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN3_EN 0x1

//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_1_EPEN4_POS 4
//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_1_EPEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN4_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN4_EN 0x1

//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_1_EPEN5_POS 5
//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_1_EPEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN5_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN5_EN 0x1

//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_1_EPEN6_POS 6
//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_1_EPEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN6_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN6_EN 0x1

//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_1_EPEN7_POS 7
//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_1_EPEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN7_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN7_EN 0x1

//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_1_EPEN8_POS 8
//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_1_EPEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN8_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN8_EN 0x1

//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_1_EPEN9_POS 9
//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_1_EPEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN9_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN9_EN 0x1

//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_1_EPEN10_POS 10
//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_1_EPEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN10_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN10_EN 0x1

//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_1_EPEN11_POS 11
//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_1_EPEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN11_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN11_EN 0x1

//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_1_EPEN12_POS 12
//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_1_EPEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN12_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN12_EN 0x1

//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_1_EPEN13_POS 13
//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_1_EPEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN13_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN13_EN 0x1

//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_1_EPEN14_POS 14
//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_1_EPEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN14_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN14_EN 0x1

//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_1_EPEN15_POS 15
//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_1_EPEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN15_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN15_EN 0x1

//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_1_EPEN16_POS 16
//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_1_EPEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN16_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN16_EN 0x1

//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_1_EPEN17_POS 17
//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_1_EPEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN17_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN17_EN 0x1

//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_1_EPEN18_POS 18
//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_1_EPEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN18_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN18_EN 0x1

//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_1_EPEN19_POS 19
//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_1_EPEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN19_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN19_EN 0x1

//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_1_EPEN20_POS 20
//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_1_EPEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN20_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN20_EN 0x1

//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_1_EPEN21_POS 21
//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_1_EPEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN21_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN21_EN 0x1

//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_1_EPEN22_POS 22
//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_1_EPEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN22_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN22_EN 0x1

//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_1_EPEN23_POS 23
//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_1_EPEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN23_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN23_EN 0x1

//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_1_EPEN24_POS 24
//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_1_EPEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN24_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN24_EN 0x1

//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_1_EPEN25_POS 25
//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_1_EPEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN25_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN25_EN 0x1

//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_1_EPEN26_POS 26
//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_1_EPEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN26_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN26_EN 0x1

//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_1_EPEN27_POS 27
//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_1_EPEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN27_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN27_EN 0x1

//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_1_EPEN28_POS 28
//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_1_EPEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN28_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN28_EN 0x1

//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_1_EPEN29_POS 29
//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_1_EPEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN29_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN29_EN 0x1

//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_1_EPEN30_POS 30
//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_1_EPEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN30_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN30_EN 0x1

//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_1_EPEN31_POS 31
//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_1_EPEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_1_EPEN31_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_1_EPEN31_EN 0x1

//! @}

//! \defgroup EP_THRSD_EN_2 Register EP_THRSD_EN_2 - Egress Port Threshold Enable
//! @{

//! Register Offset (relative)
#define EP_THRSD_EN_2 0x108
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EP_THRSD_EN_2 0x19080108u

//! Register Reset Value
#define EP_THRSD_EN_2_RST 0xFFFFFFFFu

//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_2_EPEN0_POS 0
//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_2_EPEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN0_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN0_EN 0x1

//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_2_EPEN1_POS 1
//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_2_EPEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN1_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN1_EN 0x1

//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_2_EPEN2_POS 2
//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_2_EPEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN2_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN2_EN 0x1

//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_2_EPEN3_POS 3
//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_2_EPEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN3_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN3_EN 0x1

//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_2_EPEN4_POS 4
//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_2_EPEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN4_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN4_EN 0x1

//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_2_EPEN5_POS 5
//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_2_EPEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN5_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN5_EN 0x1

//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_2_EPEN6_POS 6
//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_2_EPEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN6_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN6_EN 0x1

//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_2_EPEN7_POS 7
//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_2_EPEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN7_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN7_EN 0x1

//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_2_EPEN8_POS 8
//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_2_EPEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN8_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN8_EN 0x1

//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_2_EPEN9_POS 9
//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_2_EPEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN9_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN9_EN 0x1

//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_2_EPEN10_POS 10
//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_2_EPEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN10_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN10_EN 0x1

//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_2_EPEN11_POS 11
//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_2_EPEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN11_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN11_EN 0x1

//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_2_EPEN12_POS 12
//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_2_EPEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN12_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN12_EN 0x1

//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_2_EPEN13_POS 13
//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_2_EPEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN13_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN13_EN 0x1

//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_2_EPEN14_POS 14
//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_2_EPEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN14_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN14_EN 0x1

//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_2_EPEN15_POS 15
//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_2_EPEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN15_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN15_EN 0x1

//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_2_EPEN16_POS 16
//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_2_EPEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN16_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN16_EN 0x1

//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_2_EPEN17_POS 17
//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_2_EPEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN17_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN17_EN 0x1

//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_2_EPEN18_POS 18
//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_2_EPEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN18_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN18_EN 0x1

//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_2_EPEN19_POS 19
//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_2_EPEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN19_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN19_EN 0x1

//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_2_EPEN20_POS 20
//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_2_EPEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN20_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN20_EN 0x1

//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_2_EPEN21_POS 21
//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_2_EPEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN21_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN21_EN 0x1

//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_2_EPEN22_POS 22
//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_2_EPEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN22_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN22_EN 0x1

//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_2_EPEN23_POS 23
//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_2_EPEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN23_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN23_EN 0x1

//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_2_EPEN24_POS 24
//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_2_EPEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN24_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN24_EN 0x1

//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_2_EPEN25_POS 25
//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_2_EPEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN25_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN25_EN 0x1

//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_2_EPEN26_POS 26
//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_2_EPEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN26_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN26_EN 0x1

//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_2_EPEN27_POS 27
//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_2_EPEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN27_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN27_EN 0x1

//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_2_EPEN28_POS 28
//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_2_EPEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN28_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN28_EN 0x1

//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_2_EPEN29_POS 29
//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_2_EPEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN29_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN29_EN 0x1

//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_2_EPEN30_POS 30
//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_2_EPEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN30_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN30_EN 0x1

//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_2_EPEN31_POS 31
//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_2_EPEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_2_EPEN31_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_2_EPEN31_EN 0x1

//! @}

//! \defgroup EP_THRSD_EN_3 Register EP_THRSD_EN_3 - Egress Port Threshold Enable
//! @{

//! Register Offset (relative)
#define EP_THRSD_EN_3 0x10C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EP_THRSD_EN_3 0x1908010Cu

//! Register Reset Value
#define EP_THRSD_EN_3_RST 0xFFFFFFFFu

//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_3_EPEN0_POS 0
//! Field EPEN0 - EP Enable 0
#define EP_THRSD_EN_3_EPEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN0_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN0_EN 0x1

//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_3_EPEN1_POS 1
//! Field EPEN1 - EP Enable 1
#define EP_THRSD_EN_3_EPEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN1_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN1_EN 0x1

//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_3_EPEN2_POS 2
//! Field EPEN2 - EP Enable 2
#define EP_THRSD_EN_3_EPEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN2_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN2_EN 0x1

//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_3_EPEN3_POS 3
//! Field EPEN3 - EP Enable 3
#define EP_THRSD_EN_3_EPEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN3_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN3_EN 0x1

//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_3_EPEN4_POS 4
//! Field EPEN4 - EP Enable 4
#define EP_THRSD_EN_3_EPEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN4_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN4_EN 0x1

//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_3_EPEN5_POS 5
//! Field EPEN5 - EP Enable 5
#define EP_THRSD_EN_3_EPEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN5_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN5_EN 0x1

//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_3_EPEN6_POS 6
//! Field EPEN6 - EP Enable 6
#define EP_THRSD_EN_3_EPEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN6_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN6_EN 0x1

//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_3_EPEN7_POS 7
//! Field EPEN7 - EP Enable 7
#define EP_THRSD_EN_3_EPEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN7_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN7_EN 0x1

//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_3_EPEN8_POS 8
//! Field EPEN8 - EP Enable 8
#define EP_THRSD_EN_3_EPEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN8_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN8_EN 0x1

//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_3_EPEN9_POS 9
//! Field EPEN9 - EP Enable 9
#define EP_THRSD_EN_3_EPEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN9_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN9_EN 0x1

//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_3_EPEN10_POS 10
//! Field EPEN10 - EP Enable 10
#define EP_THRSD_EN_3_EPEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN10_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN10_EN 0x1

//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_3_EPEN11_POS 11
//! Field EPEN11 - EP Enable 11
#define EP_THRSD_EN_3_EPEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN11_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN11_EN 0x1

//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_3_EPEN12_POS 12
//! Field EPEN12 - EP Enable 12
#define EP_THRSD_EN_3_EPEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN12_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN12_EN 0x1

//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_3_EPEN13_POS 13
//! Field EPEN13 - EP Enable 13
#define EP_THRSD_EN_3_EPEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN13_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN13_EN 0x1

//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_3_EPEN14_POS 14
//! Field EPEN14 - EP Enable 14
#define EP_THRSD_EN_3_EPEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN14_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN14_EN 0x1

//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_3_EPEN15_POS 15
//! Field EPEN15 - EP Enable 15
#define EP_THRSD_EN_3_EPEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN15_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN15_EN 0x1

//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_3_EPEN16_POS 16
//! Field EPEN16 - EP Enable 16
#define EP_THRSD_EN_3_EPEN16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN16_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN16_EN 0x1

//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_3_EPEN17_POS 17
//! Field EPEN17 - EP Enable 17
#define EP_THRSD_EN_3_EPEN17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN17_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN17_EN 0x1

//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_3_EPEN18_POS 18
//! Field EPEN18 - EP Enable 18
#define EP_THRSD_EN_3_EPEN18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN18_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN18_EN 0x1

//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_3_EPEN19_POS 19
//! Field EPEN19 - EP Enable 19
#define EP_THRSD_EN_3_EPEN19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN19_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN19_EN 0x1

//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_3_EPEN20_POS 20
//! Field EPEN20 - EP Enable 20
#define EP_THRSD_EN_3_EPEN20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN20_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN20_EN 0x1

//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_3_EPEN21_POS 21
//! Field EPEN21 - EP Enable 21
#define EP_THRSD_EN_3_EPEN21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN21_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN21_EN 0x1

//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_3_EPEN22_POS 22
//! Field EPEN22 - EP Enable 22
#define EP_THRSD_EN_3_EPEN22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN22_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN22_EN 0x1

//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_3_EPEN23_POS 23
//! Field EPEN23 - EP Enable 23
#define EP_THRSD_EN_3_EPEN23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN23_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN23_EN 0x1

//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_3_EPEN24_POS 24
//! Field EPEN24 - EP Enable 24
#define EP_THRSD_EN_3_EPEN24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN24_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN24_EN 0x1

//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_3_EPEN25_POS 25
//! Field EPEN25 - EP Enable 25
#define EP_THRSD_EN_3_EPEN25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN25_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN25_EN 0x1

//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_3_EPEN26_POS 26
//! Field EPEN26 - EP Enable 26
#define EP_THRSD_EN_3_EPEN26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN26_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN26_EN 0x1

//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_3_EPEN27_POS 27
//! Field EPEN27 - EP Enable 27
#define EP_THRSD_EN_3_EPEN27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN27_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN27_EN 0x1

//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_3_EPEN28_POS 28
//! Field EPEN28 - EP Enable 28
#define EP_THRSD_EN_3_EPEN28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN28_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN28_EN 0x1

//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_3_EPEN29_POS 29
//! Field EPEN29 - EP Enable 29
#define EP_THRSD_EN_3_EPEN29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN29_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN29_EN 0x1

//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_3_EPEN30_POS 30
//! Field EPEN30 - EP Enable 30
#define EP_THRSD_EN_3_EPEN30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN30_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN30_EN 0x1

//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_3_EPEN31_POS 31
//! Field EPEN31 - EP Enable 31
#define EP_THRSD_EN_3_EPEN31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_EP_THRSD_EN_3_EPEN31_DIS 0x0
//! Constant EN - EN
#define CONST_EP_THRSD_EN_3_EPEN31_EN 0x1

//! @}

//! \defgroup IP_OCC_EN Register IP_OCC_EN - Ingress Port Occupancy Counter Enable
//! @{

//! Register Offset (relative)
#define IP_OCC_EN 0x110
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_EN 0x19080110u

//! Register Reset Value
#define IP_OCC_EN_RST 0x0000FFFFu

//! Field IPOCEN0 - IPOC Enable 0
#define IP_OCC_EN_IPOCEN0_POS 0
//! Field IPOCEN0 - IPOC Enable 0
#define IP_OCC_EN_IPOCEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN0_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN0_EN 0x1

//! Field IPOCEN1 - IPOC Enable 1
#define IP_OCC_EN_IPOCEN1_POS 1
//! Field IPOCEN1 - IPOC Enable 1
#define IP_OCC_EN_IPOCEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN1_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN1_EN 0x1

//! Field IPOCEN2 - IPOC Enable 2
#define IP_OCC_EN_IPOCEN2_POS 2
//! Field IPOCEN2 - IPOC Enable 2
#define IP_OCC_EN_IPOCEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN2_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN2_EN 0x1

//! Field IPOCEN3 - IPOC Enable 3
#define IP_OCC_EN_IPOCEN3_POS 3
//! Field IPOCEN3 - IPOC Enable 3
#define IP_OCC_EN_IPOCEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN3_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN3_EN 0x1

//! Field IPOCEN4 - IPOC Enable 4
#define IP_OCC_EN_IPOCEN4_POS 4
//! Field IPOCEN4 - IPOC Enable 4
#define IP_OCC_EN_IPOCEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN4_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN4_EN 0x1

//! Field IPOCEN5 - IPOC Enable 5
#define IP_OCC_EN_IPOCEN5_POS 5
//! Field IPOCEN5 - IPOC Enable 5
#define IP_OCC_EN_IPOCEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN5_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN5_EN 0x1

//! Field IPOCEN6 - IPOC Enable 6
#define IP_OCC_EN_IPOCEN6_POS 6
//! Field IPOCEN6 - IPOC Enable 6
#define IP_OCC_EN_IPOCEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN6_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN6_EN 0x1

//! Field IPOCEN7 - IPOC Enable 7
#define IP_OCC_EN_IPOCEN7_POS 7
//! Field IPOCEN7 - IPOC Enable 7
#define IP_OCC_EN_IPOCEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN7_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN7_EN 0x1

//! Field IPOCEN8 - IPOC Enable 8
#define IP_OCC_EN_IPOCEN8_POS 8
//! Field IPOCEN8 - IPOC Enable 8
#define IP_OCC_EN_IPOCEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN8_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN8_EN 0x1

//! Field IPOCEN9 - IPOC Enable 9
#define IP_OCC_EN_IPOCEN9_POS 9
//! Field IPOCEN9 - IPOC Enable 9
#define IP_OCC_EN_IPOCEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN9_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN9_EN 0x1

//! Field IPOEN10 - IP Enable 10
#define IP_OCC_EN_IPOEN10_POS 10
//! Field IPOEN10 - IP Enable 10
#define IP_OCC_EN_IPOEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOEN10_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOEN10_EN 0x1

//! Field IPOCEN11 - IPOC Enable 11
#define IP_OCC_EN_IPOCEN11_POS 11
//! Field IPOCEN11 - IPOC Enable 11
#define IP_OCC_EN_IPOCEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN11_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN11_EN 0x1

//! Field IPOCEN12 - IPOC Enable 12
#define IP_OCC_EN_IPOCEN12_POS 12
//! Field IPOCEN12 - IPOC Enable 12
#define IP_OCC_EN_IPOCEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN12_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN12_EN 0x1

//! Field IPOCEN13 - IPOC Enable 13
#define IP_OCC_EN_IPOCEN13_POS 13
//! Field IPOCEN13 - IPOC Enable 13
#define IP_OCC_EN_IPOCEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN13_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN13_EN 0x1

//! Field IPOCEN14 - IPOC Enable 14
#define IP_OCC_EN_IPOCEN14_POS 14
//! Field IPOCEN14 - IPOC Enable 14
#define IP_OCC_EN_IPOCEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN14_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN14_EN 0x1

//! Field IPOCEN15 - IPOC Enable 15
#define IP_OCC_EN_IPOCEN15_POS 15
//! Field IPOCEN15 - IPOC Enable 15
#define IP_OCC_EN_IPOCEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_IP_OCC_EN_IPOCEN15_DIS 0x0
//! Constant EN - EN
#define CONST_IP_OCC_EN_IPOCEN15_EN 0x1

//! @}

//! \defgroup IP_THRSD_EN Register IP_THRSD_EN - Ingress Port Threshold Enable
//! @{

//! Register Offset (relative)
#define IP_THRSD_EN 0x114
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRSD_EN 0x19080114u

//! Register Reset Value
#define IP_THRSD_EN_RST 0x0000FFFFu

//! Field IPEN0 - IP Enable 0
#define IP_THRSD_EN_IPEN0_POS 0
//! Field IPEN0 - IP Enable 0
#define IP_THRSD_EN_IPEN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN0_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN0_EN 0x1

//! Field IPEN1 - IP Enable 1
#define IP_THRSD_EN_IPEN1_POS 1
//! Field IPEN1 - IP Enable 1
#define IP_THRSD_EN_IPEN1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN1_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN1_EN 0x1

//! Field IPEN2 - IP Enable 2
#define IP_THRSD_EN_IPEN2_POS 2
//! Field IPEN2 - IP Enable 2
#define IP_THRSD_EN_IPEN2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN2_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN2_EN 0x1

//! Field IPEN3 - IP Enable 3
#define IP_THRSD_EN_IPEN3_POS 3
//! Field IPEN3 - IP Enable 3
#define IP_THRSD_EN_IPEN3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN3_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN3_EN 0x1

//! Field IPEN4 - IP Enable 4
#define IP_THRSD_EN_IPEN4_POS 4
//! Field IPEN4 - IP Enable 4
#define IP_THRSD_EN_IPEN4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN4_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN4_EN 0x1

//! Field IPEN5 - IP Enable 5
#define IP_THRSD_EN_IPEN5_POS 5
//! Field IPEN5 - IP Enable 5
#define IP_THRSD_EN_IPEN5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN5_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN5_EN 0x1

//! Field IPEN6 - IP Enable 6
#define IP_THRSD_EN_IPEN6_POS 6
//! Field IPEN6 - IP Enable 6
#define IP_THRSD_EN_IPEN6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN6_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN6_EN 0x1

//! Field IPEN7 - IP Enable 7
#define IP_THRSD_EN_IPEN7_POS 7
//! Field IPEN7 - IP Enable 7
#define IP_THRSD_EN_IPEN7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN7_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN7_EN 0x1

//! Field IPEN8 - IP Enable 8
#define IP_THRSD_EN_IPEN8_POS 8
//! Field IPEN8 - IP Enable 8
#define IP_THRSD_EN_IPEN8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN8_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN8_EN 0x1

//! Field IPEN9 - IP Enable 9
#define IP_THRSD_EN_IPEN9_POS 9
//! Field IPEN9 - IP Enable 9
#define IP_THRSD_EN_IPEN9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN9_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN9_EN 0x1

//! Field IPEN10 - IP Enable 10
#define IP_THRSD_EN_IPEN10_POS 10
//! Field IPEN10 - IP Enable 10
#define IP_THRSD_EN_IPEN10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN10_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN10_EN 0x1

//! Field IPEN11 - IP Enable 11
#define IP_THRSD_EN_IPEN11_POS 11
//! Field IPEN11 - IP Enable 11
#define IP_THRSD_EN_IPEN11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN11_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN11_EN 0x1

//! Field IPEN12 - IP Enable 12
#define IP_THRSD_EN_IPEN12_POS 12
//! Field IPEN12 - IP Enable 12
#define IP_THRSD_EN_IPEN12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN12_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN12_EN 0x1

//! Field IPEN13 - IP Enable 13
#define IP_THRSD_EN_IPEN13_POS 13
//! Field IPEN13 - IP Enable 13
#define IP_THRSD_EN_IPEN13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN13_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN13_EN 0x1

//! Field IPEN14 - IP Enable 14
#define IP_THRSD_EN_IPEN14_POS 14
//! Field IPEN14 - IP Enable 14
#define IP_THRSD_EN_IPEN14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN14_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN14_EN 0x1

//! Field IPEN15 - IP Enable 15
#define IP_THRSD_EN_IPEN15_POS 15
//! Field IPEN15 - IP Enable 15
#define IP_THRSD_EN_IPEN15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_IP_THRSD_EN_IPEN15_DIS 0x0
//! Constant EN - EN
#define CONST_IP_THRSD_EN_IPEN15_EN 0x1

//! @}

//! \defgroup IP_BP_STATUS Register IP_BP_STATUS - Ingress Port Back Pressure Status
//! @{

//! Register Offset (relative)
#define IP_BP_STATUS 0x118
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_BP_STATUS 0x19080118u

//! Register Reset Value
#define IP_BP_STATUS_RST 0x00000000u

//! Field IP_BP_STS_HWM - Ingress Port Back Pressure HWM Status
#define IP_BP_STATUS_IP_BP_STS_HWM_POS 0
//! Field IP_BP_STS_HWM - Ingress Port Back Pressure HWM Status
#define IP_BP_STATUS_IP_BP_STS_HWM_MASK 0xFFFFu
//! Constant NUL - NULL
#define CONST_IP_BP_STATUS_IP_BP_STS_HWM_NUL 0x0000u
//! Constant SET - BP_EN
#define CONST_IP_BP_STATUS_IP_BP_STS_HWM_SET 0x0001u

//! Field IP_BP_STS_LWM - Ingress Port Back Pressure LWM Status
#define IP_BP_STATUS_IP_BP_STS_LWM_POS 16
//! Field IP_BP_STS_LWM - Ingress Port Back Pressure LWM Status
#define IP_BP_STATUS_IP_BP_STS_LWM_MASK 0xFFFF0000u
//! Constant NUL - NULL
#define CONST_IP_BP_STATUS_IP_BP_STS_LWM_NUL 0x0000u
//! Constant SET - BP_DIS
#define CONST_IP_BP_STATUS_IP_BP_STS_LWM_SET 0x0001u

//! @}

//! \defgroup IP_OCC_0 Register IP_OCC_0 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_0 0x120
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_0 0x19080120u

//! Register Reset Value
#define IP_OCC_0_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_0_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_0_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_1 Register IP_OCC_1 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_1 0x124
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_1 0x19080124u

//! Register Reset Value
#define IP_OCC_1_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_1_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_1_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_2 Register IP_OCC_2 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_2 0x128
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_2 0x19080128u

//! Register Reset Value
#define IP_OCC_2_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_2_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_2_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_3 Register IP_OCC_3 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_3 0x12C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_3 0x1908012Cu

//! Register Reset Value
#define IP_OCC_3_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_3_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_3_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_4 Register IP_OCC_4 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_4 0x130
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_4 0x19080130u

//! Register Reset Value
#define IP_OCC_4_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_4_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_4_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_5 Register IP_OCC_5 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_5 0x134
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_5 0x19080134u

//! Register Reset Value
#define IP_OCC_5_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_5_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_5_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_6 Register IP_OCC_6 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_6 0x138
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_6 0x19080138u

//! Register Reset Value
#define IP_OCC_6_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_6_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_6_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_7 Register IP_OCC_7 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_7 0x13C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_7 0x1908013Cu

//! Register Reset Value
#define IP_OCC_7_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_7_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_7_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_8 Register IP_OCC_8 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_8 0x140
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_8 0x19080140u

//! Register Reset Value
#define IP_OCC_8_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_8_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_8_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_9 Register IP_OCC_9 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_9 0x144
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_9 0x19080144u

//! Register Reset Value
#define IP_OCC_9_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_9_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_9_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_10 Register IP_OCC_10 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_10 0x148
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_10 0x19080148u

//! Register Reset Value
#define IP_OCC_10_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_10_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_10_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_11 Register IP_OCC_11 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_11 0x14C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_11 0x1908014Cu

//! Register Reset Value
#define IP_OCC_11_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_11_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_11_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_12 Register IP_OCC_12 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_12 0x150
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_12 0x19080150u

//! Register Reset Value
#define IP_OCC_12_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_12_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_12_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_13 Register IP_OCC_13 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_13 0x154
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_13 0x19080154u

//! Register Reset Value
#define IP_OCC_13_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_13_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_13_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_14 Register IP_OCC_14 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_14 0x158
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_14 0x19080158u

//! Register Reset Value
#define IP_OCC_14_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_14_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_14_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_OCC_15 Register IP_OCC_15 - Enqueue IP Occupancy Counter
//! @{

//! Register Offset (relative)
#define IP_OCC_15 0x15C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_OCC_15 0x1908015Cu

//! Register Reset Value
#define IP_OCC_15_RST 0x00000000u

//! Field POCC - Port Occupancy Count
#define IP_OCC_15_POCC_POS 0
//! Field POCC - Port Occupancy Count
#define IP_OCC_15_POCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_0 Register IP_THRESHOLD_HWM_0 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_0 0x160
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_0 0x19080160u

//! Register Reset Value
#define IP_THRESHOLD_HWM_0_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_0_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_0_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_1 Register IP_THRESHOLD_HWM_1 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_1 0x164
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_1 0x19080164u

//! Register Reset Value
#define IP_THRESHOLD_HWM_1_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_1_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_1_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_2 Register IP_THRESHOLD_HWM_2 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_2 0x168
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_2 0x19080168u

//! Register Reset Value
#define IP_THRESHOLD_HWM_2_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_2_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_2_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_3 Register IP_THRESHOLD_HWM_3 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_3 0x16C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_3 0x1908016Cu

//! Register Reset Value
#define IP_THRESHOLD_HWM_3_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_3_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_3_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_4 Register IP_THRESHOLD_HWM_4 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_4 0x170
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_4 0x19080170u

//! Register Reset Value
#define IP_THRESHOLD_HWM_4_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_4_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_4_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_5 Register IP_THRESHOLD_HWM_5 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_5 0x174
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_5 0x19080174u

//! Register Reset Value
#define IP_THRESHOLD_HWM_5_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_5_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_5_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_6 Register IP_THRESHOLD_HWM_6 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_6 0x178
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_6 0x19080178u

//! Register Reset Value
#define IP_THRESHOLD_HWM_6_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_6_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_6_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_7 Register IP_THRESHOLD_HWM_7 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_7 0x17C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_7 0x1908017Cu

//! Register Reset Value
#define IP_THRESHOLD_HWM_7_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_7_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_7_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_8 Register IP_THRESHOLD_HWM_8 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_8 0x180
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_8 0x19080180u

//! Register Reset Value
#define IP_THRESHOLD_HWM_8_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_8_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_8_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_9 Register IP_THRESHOLD_HWM_9 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_9 0x184
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_9 0x19080184u

//! Register Reset Value
#define IP_THRESHOLD_HWM_9_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_9_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_9_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_10 Register IP_THRESHOLD_HWM_10 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_10 0x188
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_10 0x19080188u

//! Register Reset Value
#define IP_THRESHOLD_HWM_10_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_10_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_10_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_11 Register IP_THRESHOLD_HWM_11 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_11 0x18C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_11 0x1908018Cu

//! Register Reset Value
#define IP_THRESHOLD_HWM_11_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_11_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_11_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_12 Register IP_THRESHOLD_HWM_12 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_12 0x190
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_12 0x19080190u

//! Register Reset Value
#define IP_THRESHOLD_HWM_12_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_12_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_12_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_13 Register IP_THRESHOLD_HWM_13 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_13 0x194
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_13 0x19080194u

//! Register Reset Value
#define IP_THRESHOLD_HWM_13_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_13_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_13_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_14 Register IP_THRESHOLD_HWM_14 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_14 0x198
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_14 0x19080198u

//! Register Reset Value
#define IP_THRESHOLD_HWM_14_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_14_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_14_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_HWM_15 Register IP_THRESHOLD_HWM_15 - Enqueue Ingress Port Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_HWM_15 0x19C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_HWM_15 0x1908019Cu

//! Register Reset Value
#define IP_THRESHOLD_HWM_15_RST 0xFFFFFFFFu

//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_15_IP_TH_HWM_POS 0
//! Field IP_TH_HWM - HWM Threshold Value
#define IP_THRESHOLD_HWM_15_IP_TH_HWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_0 Register IP_THRESHOLD_LWM_0 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_0 0x1A0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_0 0x190801A0u

//! Register Reset Value
#define IP_THRESHOLD_LWM_0_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_0_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_0_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_1 Register IP_THRESHOLD_LWM_1 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_1 0x1A4
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_1 0x190801A4u

//! Register Reset Value
#define IP_THRESHOLD_LWM_1_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_1_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_1_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_2 Register IP_THRESHOLD_LWM_2 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_2 0x1A8
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_2 0x190801A8u

//! Register Reset Value
#define IP_THRESHOLD_LWM_2_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_2_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_2_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_3 Register IP_THRESHOLD_LWM_3 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_3 0x1AC
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_3 0x190801ACu

//! Register Reset Value
#define IP_THRESHOLD_LWM_3_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_3_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_3_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_4 Register IP_THRESHOLD_LWM_4 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_4 0x1B0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_4 0x190801B0u

//! Register Reset Value
#define IP_THRESHOLD_LWM_4_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_4_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_4_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_5 Register IP_THRESHOLD_LWM_5 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_5 0x1B4
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_5 0x190801B4u

//! Register Reset Value
#define IP_THRESHOLD_LWM_5_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_5_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_5_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_6 Register IP_THRESHOLD_LWM_6 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_6 0x1B8
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_6 0x190801B8u

//! Register Reset Value
#define IP_THRESHOLD_LWM_6_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_6_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_6_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_7 Register IP_THRESHOLD_LWM_7 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_7 0x1BC
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_7 0x190801BCu

//! Register Reset Value
#define IP_THRESHOLD_LWM_7_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_7_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_7_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_8 Register IP_THRESHOLD_LWM_8 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_8 0x1C0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_8 0x190801C0u

//! Register Reset Value
#define IP_THRESHOLD_LWM_8_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_8_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_8_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_9 Register IP_THRESHOLD_LWM_9 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_9 0x1C4
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_9 0x190801C4u

//! Register Reset Value
#define IP_THRESHOLD_LWM_9_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_9_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_9_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_10 Register IP_THRESHOLD_LWM_10 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_10 0x1C8
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_10 0x190801C8u

//! Register Reset Value
#define IP_THRESHOLD_LWM_10_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_10_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_10_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_11 Register IP_THRESHOLD_LWM_11 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_11 0x1CC
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_11 0x190801CCu

//! Register Reset Value
#define IP_THRESHOLD_LWM_11_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_11_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_11_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_12 Register IP_THRESHOLD_LWM_12 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_12 0x1D0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_12 0x190801D0u

//! Register Reset Value
#define IP_THRESHOLD_LWM_12_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_12_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_12_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_13 Register IP_THRESHOLD_LWM_13 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_13 0x1D4
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_13 0x190801D4u

//! Register Reset Value
#define IP_THRESHOLD_LWM_13_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_13_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_13_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_14 Register IP_THRESHOLD_LWM_14 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_14 0x1D8
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_14 0x190801D8u

//! Register Reset Value
#define IP_THRESHOLD_LWM_14_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_14_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_14_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IP_THRESHOLD_LWM_15 Register IP_THRESHOLD_LWM_15 - Enqueue Ingress Port Low Watermark Threshold
//! @{

//! Register Offset (relative)
#define IP_THRESHOLD_LWM_15 0x1DC
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IP_THRESHOLD_LWM_15 0x190801DCu

//! Register Reset Value
#define IP_THRESHOLD_LWM_15_RST 0xFFFFFFFFu

//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_15_IP_TH_LWM_POS 0
//! Field IP_TH_LWM - Threshold Value
#define IP_THRESHOLD_LWM_15_IP_TH_LWM_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EPON_EPON_MODE_REG Register EPON_EPON_MODE_REG - EPON Queue range register
//! @{

//! Register Offset (relative)
#define EPON_EPON_MODE_REG 0x1F0
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EPON_EPON_MODE_REG 0x190801F0u

//! Register Reset Value
#define EPON_EPON_MODE_REG_RST 0x00000000u

//! Field STARTQ - Start Queue
#define EPON_EPON_MODE_REG_STARTQ_POS 0
//! Field STARTQ - Start Queue
#define EPON_EPON_MODE_REG_STARTQ_MASK 0x1FFu

//! Field ENDQ - End Queue
#define EPON_EPON_MODE_REG_ENDQ_POS 10
//! Field ENDQ - End Queue
#define EPON_EPON_MODE_REG_ENDQ_MASK 0x7FC00u

//! Field EPONBASEPORT - EPON Base Port Value
#define EPON_EPON_MODE_REG_EPONBASEPORT_POS 20
//! Field EPONBASEPORT - EPON Base Port Value
#define EPON_EPON_MODE_REG_EPONBASEPORT_MASK 0xFF00000u

//! Field EPONPKTSIZADJ - EPON Packet size Adjustment enable
#define EPON_EPON_MODE_REG_EPONPKTSIZADJ_POS 30
//! Field EPONPKTSIZADJ - EPON Packet size Adjustment enable
#define EPON_EPON_MODE_REG_EPONPKTSIZADJ_MASK 0x40000000u

//! Field EPONCHKEN - EPON Mode Check Enable
#define EPON_EPON_MODE_REG_EPONCHKEN_POS 31
//! Field EPONCHKEN - EPON Mode Check Enable
#define EPON_EPON_MODE_REG_EPONCHKEN_MASK 0x80000000u

//! @}

//! \defgroup EPON_PORTBP_REG_0 Register EPON_PORTBP_REG_0 - EPON Mode backpressure enable register
//! @{

//! Register Offset (relative)
#define EPON_PORTBP_REG_0 0x1F4
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EPON_PORTBP_REG_0 0x190801F4u

//! Register Reset Value
#define EPON_PORTBP_REG_0_RST 0x00000000u

//! Field PORTBPEN - Port Back Pressure Enable
#define EPON_PORTBP_REG_0_PORTBPEN_POS 0
//! Field PORTBPEN - Port Back Pressure Enable
#define EPON_PORTBP_REG_0_PORTBPEN_MASK 0xFFFFFFFFu

//! @}

//! \defgroup EPON_PORTBP_REG_1 Register EPON_PORTBP_REG_1 - EPON Mode backpressure enable register
//! @{

//! Register Offset (relative)
#define EPON_PORTBP_REG_1 0x1F8
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EPON_PORTBP_REG_1 0x190801F8u

//! Register Reset Value
#define EPON_PORTBP_REG_1_RST 0x00000000u

//! Field PORTBPEN - Port Back Pressure Enable
#define EPON_PORTBP_REG_1_PORTBPEN_POS 0
//! Field PORTBPEN - Port Back Pressure Enable
#define EPON_PORTBP_REG_1_PORTBPEN_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_IGP_0 Register CFG_CPU_IGP_0 - CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_0 0x10000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_CPU_IGP_0 0x19090000u

//! Register Reset Value
#define CFG_CPU_IGP_0_RST 0x00000000u

//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_0_EQREQ_POS 0
//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_0_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_EQREQ_EN 0x1

//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_0_BUFREQ0_POS 1
//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_0_BUFREQ0_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQ0_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQ0_EN 0x1

//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_0_BUFREQ1_POS 2
//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_0_BUFREQ1_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQ1_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQ1_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_0_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_0_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_DISCHALT_EN 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_0_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_0_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BRBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_0_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_0_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_0_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_0_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_0_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_0_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_CPU_IGP_0_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_CPU_IGP_0_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_CPU_IGP_0_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_CPU_IGP_0_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_CPU_IGP_0_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_CPU_IGP_0_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_CPU_IGP_0_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_CPU_IGP_0_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_CPU_IGP_0_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_CPU_IGP_0_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_CPU_IGP_0_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_CPU_IGP_0_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_CPU_IGP_0_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_CPU_IGP_0_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_CPU_IGP_0_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_CPU_IGP_0_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_CPU_IGP_0_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_CPU_IGP_0_DISC_REASON15 0xF

//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_0_BUFREQ2_POS 20
//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_0_BUFREQ2_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQ2_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQ2_EN 0x1

//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_0_BUFREQ3_POS 21
//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_0_BUFREQ3_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQ3_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQ3_EN 0x1

//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_0_BUFREQ4_POS 22
//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_0_BUFREQ4_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQ4_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQ4_EN 0x1

//! @}

//! \defgroup EQPC_CPU_IGP_0 Register EQPC_CPU_IGP_0 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_0 0x1000C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_CPU_IGP_0 0x1909000Cu

//! Register Reset Value
#define EQPC_CPU_IGP_0_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_0_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_0_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_CPU_IGP_0 Register DISC_DESC0_CPU_IGP_0 - CPU Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_CPU_IGP_0 0x10010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_CPU_IGP_0 0x19090010u

//! Register Reset Value
#define DISC_DESC0_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_CPU_IGP_0 Register DISC_DESC1_CPU_IGP_0 - CPU Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_CPU_IGP_0 0x10014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_CPU_IGP_0 0x19090014u

//! Register Reset Value
#define DISC_DESC1_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_CPU_IGP_0 Register DISC_DESC2_CPU_IGP_0 - CPU Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_CPU_IGP_0 0x10018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_CPU_IGP_0 0x19090018u

//! Register Reset Value
#define DISC_DESC2_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_CPU_IGP_0 Register DISC_DESC3_CPU_IGP_0 - CPU Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_CPU_IGP_0 0x1001C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_CPU_IGP_0 0x1909001Cu

//! Register Reset Value
#define DISC_DESC3_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_IGP_0 Register IRNCR_CPU_IGP_0 - CPU Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_IGP_0 0x10020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_CPU_IGP_0 0x19090020u

//! Register Reset Value
#define IRNCR_CPU_IGP_0_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_0_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_0_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_0_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_0_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_DR_INTOCC 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_0_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_0_PBR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_PBR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_PBR_INTOCC 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_0_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_0_S0PR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_S0PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_S0PR_INTOCC 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_0_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_0_S1PR_MASK 0x40u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_S1PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_S1PR_INTOCC 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_0_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_0_S2PR_MASK 0x80u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_S2PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_S2PR_INTOCC 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_0_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_0_S3PR_MASK 0x100u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_S3PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_S3PR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_0 Register IRNICR_CPU_IGP_0 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_0 0x10024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_CPU_IGP_0 0x19090024u

//! Register Reset Value
#define IRNICR_CPU_IGP_0_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_0_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_0_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_0_DR_MASK 0x8u

//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_0_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_0_PBR_MASK 0x10u

//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_0_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_0_S0PR_MASK 0x20u

//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_0_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_0_S1PR_MASK 0x40u

//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_0_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_0_S2PR_MASK 0x80u

//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_0_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_0_S3PR_MASK 0x100u

//! @}

//! \defgroup IRNEN_CPU_IGP_0 Register IRNEN_CPU_IGP_0 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_0 0x10028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_CPU_IGP_0 0x19090028u

//! Register Reset Value
#define IRNEN_CPU_IGP_0_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_0_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_0_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_0_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_DR_EN 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_0_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_0_PBR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_PBR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_PBR_EN 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_0_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_0_S0PR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_S0PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_S0PR_EN 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_0_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_0_S1PR_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_S1PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_S1PR_EN 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_0_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_0_S2PR_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_S2PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_S2PR_EN 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_0_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_0_S3PR_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_S3PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_S3PR_EN 0x1

//! @}

//! \defgroup DICC_IGP_0 Register DICC_IGP_0 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_0 0x10038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_0 0x19090038u

//! Register Reset Value
#define DICC_IGP_0_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_0_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_0_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_0 Register DROP_IGP_0 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_0 0x1003C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_0 0x1909003Cu

//! Register Reset Value
#define DROP_IGP_0_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_0_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_0_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NPBPC_CPU_IGP_0 Register NPBPC_CPU_IGP_0 - CPU Ingress Port new packet buffer pointer counter
//! @{

//! Register Offset (relative)
#define NPBPC_CPU_IGP_0 0x10040
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NPBPC_CPU_IGP_0 0x19090040u

//! Register Reset Value
#define NPBPC_CPU_IGP_0_RST 0x00000000u

//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_0_NPBPC_POS 0
//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_0_NPBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS0PC_CPU_IGP_0 Register NS0PC_CPU_IGP_0 - CPU Ingress Port new size0 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS0PC_CPU_IGP_0 0x10044
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS0PC_CPU_IGP_0 0x19090044u

//! Register Reset Value
#define NS0PC_CPU_IGP_0_RST 0x00000000u

//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_0_NS0PC_POS 0
//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_0_NS0PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS1PC_CPU_IGP_0 Register NS1PC_CPU_IGP_0 - CPU Ingress Port new size1 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS1PC_CPU_IGP_0 0x10048
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS1PC_CPU_IGP_0 0x19090048u

//! Register Reset Value
#define NS1PC_CPU_IGP_0_RST 0x00000000u

//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_0_NS1PC_POS 0
//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_0_NS1PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS2PC_CPU_IGP_0 Register NS2PC_CPU_IGP_0 - CPU Ingress Port new size2 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS2PC_CPU_IGP_0 0x1004C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS2PC_CPU_IGP_0 0x1909004Cu

//! Register Reset Value
#define NS2PC_CPU_IGP_0_RST 0x00000000u

//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_0_NS2PC_POS 0
//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_0_NS2PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS3PC_CPU_IGP_0 Register NS3PC_CPU_IGP_0 - CPU Ingress Port new size3 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS3PC_CPU_IGP_0 0x10050
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS3PC_CPU_IGP_0 0x19090050u

//! Register Reset Value
#define NS3PC_CPU_IGP_0_RST 0x00000000u

//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_0_NS3PC_POS 0
//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_0_NS3PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NEW_PBPTR_CPU_IGP_0 Register NEW_PBPTR_CPU_IGP_0 - CPU Ingress Port New Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_PBPTR_CPU_IGP_0 0x10080
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_PBPTR_CPU_IGP_0 0x19090080u

//! Register Reset Value
#define NEW_PBPTR_CPU_IGP_0_RST 0xFFFFFF80u

//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_0_OFFSET_MASK 0x7Fu

//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_0_SA_POS 7
//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_0_SA_MASK 0xFFFFFF80u

//! @}

//! \defgroup NEW_S0PTR_CPU_IGP_0 Register NEW_S0PTR_CPU_IGP_0 - CPU Ingress Port New Size0 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S0PTR_CPU_IGP_0 0x10084
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S0PTR_CPU_IGP_0 0x19090084u

//! Register Reset Value
#define NEW_S0PTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S1PTR_CPU_IGP_0 Register NEW_S1PTR_CPU_IGP_0 - CPU Ingress Port New Size1 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S1PTR_CPU_IGP_0 0x10088
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S1PTR_CPU_IGP_0 0x19090088u

//! Register Reset Value
#define NEW_S1PTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S2PTR_CPU_IGP_0 Register NEW_S2PTR_CPU_IGP_0 - CPU Ingress Port New Size2 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S2PTR_CPU_IGP_0 0x1008C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S2PTR_CPU_IGP_0 0x1909008Cu

//! Register Reset Value
#define NEW_S2PTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S3PTR_CPU_IGP_0 Register NEW_S3PTR_CPU_IGP_0 - CPU Ingress Port New Size3 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S3PTR_CPU_IGP_0 0x10090
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S3PTR_CPU_IGP_0 0x19090090u

//! Register Reset Value
#define NEW_S3PTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DCNTR_IGP_0 Register DCNTR_IGP_0 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_0 0x1009C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_0 0x1909009Cu

//! Register Reset Value
#define DCNTR_IGP_0_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_0_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_0_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_0_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_0_DCNT_MASK 0x3F00u

//! @}

//! \defgroup DESC0_0_CPU_IGP_0 Register DESC0_0_CPU_IGP_0 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_0 0x10100
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_0_CPU_IGP_0 0x19090100u

//! Register Reset Value
#define DESC0_0_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_IGP_0 Register DESC1_0_CPU_IGP_0 - CPU Ingress Port Descriptor 0 DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_IGP_0 0x10104
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_0_CPU_IGP_0 0x19090104u

//! Register Reset Value
#define DESC1_0_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_IGP_0 Register DESC2_0_CPU_IGP_0 - CPU Ingress Port Descriptor 0 DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_IGP_0 0x10108
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_0_CPU_IGP_0 0x19090108u

//! Register Reset Value
#define DESC2_0_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_IGP_0 Register DESC3_0_CPU_IGP_0 - CPU Ingress Port Descriptor 0 DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_IGP_0 0x1010C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_0_CPU_IGP_0 0x1909010Cu

//! Register Reset Value
#define DESC3_0_CPU_IGP_0_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_IGP_0 Register DESC0_1_CPU_IGP_0 - CPU Ingress Port Descriptor 1 DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_IGP_0 0x10110
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_1_CPU_IGP_0 0x19090110u

//! Register Reset Value
#define DESC0_1_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_IGP_0 Register DESC1_1_CPU_IGP_0 - CPU Ingress Port Descriptor 1 DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_IGP_0 0x10114
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_1_CPU_IGP_0 0x19090114u

//! Register Reset Value
#define DESC1_1_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_IGP_0 Register DESC2_1_CPU_IGP_0 - CPU Ingress Port Descriptor 1 DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_IGP_0 0x10118
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_1_CPU_IGP_0 0x19090118u

//! Register Reset Value
#define DESC2_1_CPU_IGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_IGP_0 Register DESC3_1_CPU_IGP_0 - CPU Ingress Port Descriptor 1 DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_IGP_0 0x1011C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_1_CPU_IGP_0 0x1909011Cu

//! Register Reset Value
#define DESC3_1_CPU_IGP_0_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_0_DESC_MASK 0x7FFFFFFFu

//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_0_OWN_POS 31
//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_0_OWN_MASK 0x80000000u

//! @}

//! \defgroup CFG_CPU_IGP_1 Register CFG_CPU_IGP_1 - CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_1 0x11000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_CPU_IGP_1 0x19091000u

//! Register Reset Value
#define CFG_CPU_IGP_1_RST 0x00000000u

//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_1_EQREQ_POS 0
//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_1_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_EQREQ_EN 0x1

//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_1_BUFREQ0_POS 1
//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_1_BUFREQ0_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQ0_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQ0_EN 0x1

//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_1_BUFREQ1_POS 2
//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_1_BUFREQ1_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQ1_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQ1_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_1_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_1_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_DISCHALT_EN 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_1_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_1_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BRBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_1_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_1_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_1_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_1_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_1_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_1_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_CPU_IGP_1_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_CPU_IGP_1_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_CPU_IGP_1_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_CPU_IGP_1_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_CPU_IGP_1_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_CPU_IGP_1_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_CPU_IGP_1_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_CPU_IGP_1_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_CPU_IGP_1_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_CPU_IGP_1_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_CPU_IGP_1_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_CPU_IGP_1_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_CPU_IGP_1_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_CPU_IGP_1_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_CPU_IGP_1_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_CPU_IGP_1_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_CPU_IGP_1_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_CPU_IGP_1_DISC_REASON15 0xF

//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_1_BUFREQ2_POS 20
//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_1_BUFREQ2_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQ2_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQ2_EN 0x1

//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_1_BUFREQ3_POS 21
//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_1_BUFREQ3_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQ3_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQ3_EN 0x1

//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_1_BUFREQ4_POS 22
//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_1_BUFREQ4_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQ4_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQ4_EN 0x1

//! @}

//! \defgroup EQPC_CPU_IGP_1 Register EQPC_CPU_IGP_1 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_1 0x1100C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_CPU_IGP_1 0x1909100Cu

//! Register Reset Value
#define EQPC_CPU_IGP_1_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_1_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_1_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_CPU_IGP_1 Register DISC_DESC0_CPU_IGP_1 - CPU Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_CPU_IGP_1 0x11010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_CPU_IGP_1 0x19091010u

//! Register Reset Value
#define DISC_DESC0_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_CPU_IGP_1 Register DISC_DESC1_CPU_IGP_1 - CPU Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_CPU_IGP_1 0x11014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_CPU_IGP_1 0x19091014u

//! Register Reset Value
#define DISC_DESC1_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_CPU_IGP_1 Register DISC_DESC2_CPU_IGP_1 - CPU Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_CPU_IGP_1 0x11018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_CPU_IGP_1 0x19091018u

//! Register Reset Value
#define DISC_DESC2_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_CPU_IGP_1 Register DISC_DESC3_CPU_IGP_1 - CPU Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_CPU_IGP_1 0x1101C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_CPU_IGP_1 0x1909101Cu

//! Register Reset Value
#define DISC_DESC3_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_IGP_1 Register IRNCR_CPU_IGP_1 - CPU Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_IGP_1 0x11020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_CPU_IGP_1 0x19091020u

//! Register Reset Value
#define IRNCR_CPU_IGP_1_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_1_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_1_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_1_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_1_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_DR_INTOCC 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_1_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_1_PBR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_PBR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_PBR_INTOCC 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_1_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_1_S0PR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_S0PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_S0PR_INTOCC 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_1_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_1_S1PR_MASK 0x40u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_S1PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_S1PR_INTOCC 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_1_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_1_S2PR_MASK 0x80u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_S2PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_S2PR_INTOCC 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_1_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_1_S3PR_MASK 0x100u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_S3PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_S3PR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_1 Register IRNICR_CPU_IGP_1 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_1 0x11024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_CPU_IGP_1 0x19091024u

//! Register Reset Value
#define IRNICR_CPU_IGP_1_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_1_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_1_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_1_DR_MASK 0x8u

//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_1_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_1_PBR_MASK 0x10u

//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_1_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_1_S0PR_MASK 0x20u

//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_1_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_1_S1PR_MASK 0x40u

//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_1_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_1_S2PR_MASK 0x80u

//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_1_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_1_S3PR_MASK 0x100u

//! @}

//! \defgroup IRNEN_CPU_IGP_1 Register IRNEN_CPU_IGP_1 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_1 0x11028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_CPU_IGP_1 0x19091028u

//! Register Reset Value
#define IRNEN_CPU_IGP_1_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_1_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_1_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_1_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_DR_EN 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_1_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_1_PBR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_PBR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_PBR_EN 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_1_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_1_S0PR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_S0PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_S0PR_EN 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_1_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_1_S1PR_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_S1PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_S1PR_EN 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_1_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_1_S2PR_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_S2PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_S2PR_EN 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_1_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_1_S3PR_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_S3PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_S3PR_EN 0x1

//! @}

//! \defgroup DICC_IGP_1 Register DICC_IGP_1 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_1 0x11038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_1 0x19091038u

//! Register Reset Value
#define DICC_IGP_1_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_1_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_1_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_1 Register DROP_IGP_1 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_1 0x1103C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_1 0x1909103Cu

//! Register Reset Value
#define DROP_IGP_1_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_1_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_1_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NPBPC_CPU_IGP_1 Register NPBPC_CPU_IGP_1 - CPU Ingress Port new packet buffer pointer counter
//! @{

//! Register Offset (relative)
#define NPBPC_CPU_IGP_1 0x11040
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NPBPC_CPU_IGP_1 0x19091040u

//! Register Reset Value
#define NPBPC_CPU_IGP_1_RST 0x00000000u

//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_1_NPBPC_POS 0
//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_1_NPBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS0PC_CPU_IGP_1 Register NS0PC_CPU_IGP_1 - CPU Ingress Port new size0 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS0PC_CPU_IGP_1 0x11044
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS0PC_CPU_IGP_1 0x19091044u

//! Register Reset Value
#define NS0PC_CPU_IGP_1_RST 0x00000000u

//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_1_NS0PC_POS 0
//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_1_NS0PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS1PC_CPU_IGP_1 Register NS1PC_CPU_IGP_1 - CPU Ingress Port new size1 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS1PC_CPU_IGP_1 0x11048
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS1PC_CPU_IGP_1 0x19091048u

//! Register Reset Value
#define NS1PC_CPU_IGP_1_RST 0x00000000u

//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_1_NS1PC_POS 0
//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_1_NS1PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS2PC_CPU_IGP_1 Register NS2PC_CPU_IGP_1 - CPU Ingress Port new size2 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS2PC_CPU_IGP_1 0x1104C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS2PC_CPU_IGP_1 0x1909104Cu

//! Register Reset Value
#define NS2PC_CPU_IGP_1_RST 0x00000000u

//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_1_NS2PC_POS 0
//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_1_NS2PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS3PC_CPU_IGP_1 Register NS3PC_CPU_IGP_1 - CPU Ingress Port new size3 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS3PC_CPU_IGP_1 0x11050
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS3PC_CPU_IGP_1 0x19091050u

//! Register Reset Value
#define NS3PC_CPU_IGP_1_RST 0x00000000u

//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_1_NS3PC_POS 0
//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_1_NS3PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NEW_PBPTR_CPU_IGP_1 Register NEW_PBPTR_CPU_IGP_1 - CPU Ingress Port New Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_PBPTR_CPU_IGP_1 0x11080
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_PBPTR_CPU_IGP_1 0x19091080u

//! Register Reset Value
#define NEW_PBPTR_CPU_IGP_1_RST 0xFFFFFF80u

//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_1_OFFSET_MASK 0x7Fu

//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_1_SA_POS 7
//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_1_SA_MASK 0xFFFFFF80u

//! @}

//! \defgroup NEW_S0PTR_CPU_IGP_1 Register NEW_S0PTR_CPU_IGP_1 - CPU Ingress Port New Size0 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S0PTR_CPU_IGP_1 0x11084
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S0PTR_CPU_IGP_1 0x19091084u

//! Register Reset Value
#define NEW_S0PTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S1PTR_CPU_IGP_1 Register NEW_S1PTR_CPU_IGP_1 - CPU Ingress Port New Size1 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S1PTR_CPU_IGP_1 0x11088
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S1PTR_CPU_IGP_1 0x19091088u

//! Register Reset Value
#define NEW_S1PTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S2PTR_CPU_IGP_1 Register NEW_S2PTR_CPU_IGP_1 - CPU Ingress Port New Size2 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S2PTR_CPU_IGP_1 0x1108C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S2PTR_CPU_IGP_1 0x1909108Cu

//! Register Reset Value
#define NEW_S2PTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S3PTR_CPU_IGP_1 Register NEW_S3PTR_CPU_IGP_1 - CPU Ingress Port New Size3 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S3PTR_CPU_IGP_1 0x11090
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S3PTR_CPU_IGP_1 0x19091090u

//! Register Reset Value
#define NEW_S3PTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DCNTR_IGP_1 Register DCNTR_IGP_1 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_1 0x1109C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_1 0x1909109Cu

//! Register Reset Value
#define DCNTR_IGP_1_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_1_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_1_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_1_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_1_DCNT_MASK 0x3F00u

//! @}

//! \defgroup DESC0_0_CPU_IGP_1 Register DESC0_0_CPU_IGP_1 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_1 0x11100
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_0_CPU_IGP_1 0x19091100u

//! Register Reset Value
#define DESC0_0_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_IGP_1 Register DESC1_0_CPU_IGP_1 - CPU Ingress Port Descriptor 0 DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_IGP_1 0x11104
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_0_CPU_IGP_1 0x19091104u

//! Register Reset Value
#define DESC1_0_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_IGP_1 Register DESC2_0_CPU_IGP_1 - CPU Ingress Port Descriptor 0 DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_IGP_1 0x11108
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_0_CPU_IGP_1 0x19091108u

//! Register Reset Value
#define DESC2_0_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_IGP_1 Register DESC3_0_CPU_IGP_1 - CPU Ingress Port Descriptor 0 DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_IGP_1 0x1110C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_0_CPU_IGP_1 0x1909110Cu

//! Register Reset Value
#define DESC3_0_CPU_IGP_1_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_IGP_1 Register DESC0_1_CPU_IGP_1 - CPU Ingress Port Descriptor 1 DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_IGP_1 0x11110
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_1_CPU_IGP_1 0x19091110u

//! Register Reset Value
#define DESC0_1_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_IGP_1 Register DESC1_1_CPU_IGP_1 - CPU Ingress Port Descriptor 1 DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_IGP_1 0x11114
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_1_CPU_IGP_1 0x19091114u

//! Register Reset Value
#define DESC1_1_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_IGP_1 Register DESC2_1_CPU_IGP_1 - CPU Ingress Port Descriptor 1 DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_IGP_1 0x11118
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_1_CPU_IGP_1 0x19091118u

//! Register Reset Value
#define DESC2_1_CPU_IGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_IGP_1 Register DESC3_1_CPU_IGP_1 - CPU Ingress Port Descriptor 1 DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_IGP_1 0x1111C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_1_CPU_IGP_1 0x1909111Cu

//! Register Reset Value
#define DESC3_1_CPU_IGP_1_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_1_DESC_MASK 0x7FFFFFFFu

//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_1_OWN_POS 31
//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_1_OWN_MASK 0x80000000u

//! @}

//! \defgroup CFG_CPU_IGP_2 Register CFG_CPU_IGP_2 - CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_2 0x12000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_CPU_IGP_2 0x19092000u

//! Register Reset Value
#define CFG_CPU_IGP_2_RST 0x00000000u

//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_2_EQREQ_POS 0
//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_2_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_EQREQ_EN 0x1

//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_2_BUFREQ0_POS 1
//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_2_BUFREQ0_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQ0_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQ0_EN 0x1

//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_2_BUFREQ1_POS 2
//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_2_BUFREQ1_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQ1_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQ1_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_2_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_2_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_DISCHALT_EN 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_2_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_2_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BRBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_2_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_2_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_2_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_2_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_2_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_2_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_CPU_IGP_2_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_CPU_IGP_2_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_CPU_IGP_2_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_CPU_IGP_2_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_CPU_IGP_2_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_CPU_IGP_2_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_CPU_IGP_2_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_CPU_IGP_2_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_CPU_IGP_2_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_CPU_IGP_2_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_CPU_IGP_2_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_CPU_IGP_2_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_CPU_IGP_2_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_CPU_IGP_2_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_CPU_IGP_2_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_CPU_IGP_2_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_CPU_IGP_2_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_CPU_IGP_2_DISC_REASON15 0xF

//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_2_BUFREQ2_POS 20
//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_2_BUFREQ2_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQ2_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQ2_EN 0x1

//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_2_BUFREQ3_POS 21
//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_2_BUFREQ3_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQ3_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQ3_EN 0x1

//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_2_BUFREQ4_POS 22
//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_2_BUFREQ4_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQ4_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQ4_EN 0x1

//! @}

//! \defgroup EQPC_CPU_IGP_2 Register EQPC_CPU_IGP_2 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_2 0x1200C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_CPU_IGP_2 0x1909200Cu

//! Register Reset Value
#define EQPC_CPU_IGP_2_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_2_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_2_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_CPU_IGP_2 Register DISC_DESC0_CPU_IGP_2 - CPU Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_CPU_IGP_2 0x12010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_CPU_IGP_2 0x19092010u

//! Register Reset Value
#define DISC_DESC0_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_CPU_IGP_2 Register DISC_DESC1_CPU_IGP_2 - CPU Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_CPU_IGP_2 0x12014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_CPU_IGP_2 0x19092014u

//! Register Reset Value
#define DISC_DESC1_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_CPU_IGP_2 Register DISC_DESC2_CPU_IGP_2 - CPU Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_CPU_IGP_2 0x12018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_CPU_IGP_2 0x19092018u

//! Register Reset Value
#define DISC_DESC2_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_CPU_IGP_2 Register DISC_DESC3_CPU_IGP_2 - CPU Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_CPU_IGP_2 0x1201C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_CPU_IGP_2 0x1909201Cu

//! Register Reset Value
#define DISC_DESC3_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_IGP_2 Register IRNCR_CPU_IGP_2 - CPU Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_IGP_2 0x12020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_CPU_IGP_2 0x19092020u

//! Register Reset Value
#define IRNCR_CPU_IGP_2_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_2_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_2_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_2_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_2_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_DR_INTOCC 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_2_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_2_PBR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_PBR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_PBR_INTOCC 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_2_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_2_S0PR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_S0PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_S0PR_INTOCC 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_2_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_2_S1PR_MASK 0x40u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_S1PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_S1PR_INTOCC 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_2_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_2_S2PR_MASK 0x80u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_S2PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_S2PR_INTOCC 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_2_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_2_S3PR_MASK 0x100u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_S3PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_S3PR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_2 Register IRNICR_CPU_IGP_2 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_2 0x12024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_CPU_IGP_2 0x19092024u

//! Register Reset Value
#define IRNICR_CPU_IGP_2_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_2_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_2_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_2_DR_MASK 0x8u

//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_2_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_2_PBR_MASK 0x10u

//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_2_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_2_S0PR_MASK 0x20u

//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_2_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_2_S1PR_MASK 0x40u

//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_2_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_2_S2PR_MASK 0x80u

//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_2_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_2_S3PR_MASK 0x100u

//! @}

//! \defgroup IRNEN_CPU_IGP_2 Register IRNEN_CPU_IGP_2 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_2 0x12028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_CPU_IGP_2 0x19092028u

//! Register Reset Value
#define IRNEN_CPU_IGP_2_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_2_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_2_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_2_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_DR_EN 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_2_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_2_PBR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_PBR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_PBR_EN 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_2_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_2_S0PR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_S0PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_S0PR_EN 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_2_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_2_S1PR_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_S1PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_S1PR_EN 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_2_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_2_S2PR_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_S2PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_S2PR_EN 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_2_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_2_S3PR_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_S3PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_S3PR_EN 0x1

//! @}

//! \defgroup DICC_IGP_2 Register DICC_IGP_2 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_2 0x12038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_2 0x19092038u

//! Register Reset Value
#define DICC_IGP_2_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_2_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_2_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_2 Register DROP_IGP_2 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_2 0x1203C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_2 0x1909203Cu

//! Register Reset Value
#define DROP_IGP_2_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_2_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_2_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NPBPC_CPU_IGP_2 Register NPBPC_CPU_IGP_2 - CPU Ingress Port new packet buffer pointer counter
//! @{

//! Register Offset (relative)
#define NPBPC_CPU_IGP_2 0x12040
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NPBPC_CPU_IGP_2 0x19092040u

//! Register Reset Value
#define NPBPC_CPU_IGP_2_RST 0x00000000u

//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_2_NPBPC_POS 0
//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_2_NPBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS0PC_CPU_IGP_2 Register NS0PC_CPU_IGP_2 - CPU Ingress Port new size0 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS0PC_CPU_IGP_2 0x12044
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS0PC_CPU_IGP_2 0x19092044u

//! Register Reset Value
#define NS0PC_CPU_IGP_2_RST 0x00000000u

//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_2_NS0PC_POS 0
//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_2_NS0PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS1PC_CPU_IGP_2 Register NS1PC_CPU_IGP_2 - CPU Ingress Port new size1 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS1PC_CPU_IGP_2 0x12048
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS1PC_CPU_IGP_2 0x19092048u

//! Register Reset Value
#define NS1PC_CPU_IGP_2_RST 0x00000000u

//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_2_NS1PC_POS 0
//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_2_NS1PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS2PC_CPU_IGP_2 Register NS2PC_CPU_IGP_2 - CPU Ingress Port new size2 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS2PC_CPU_IGP_2 0x1204C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS2PC_CPU_IGP_2 0x1909204Cu

//! Register Reset Value
#define NS2PC_CPU_IGP_2_RST 0x00000000u

//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_2_NS2PC_POS 0
//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_2_NS2PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS3PC_CPU_IGP_2 Register NS3PC_CPU_IGP_2 - CPU Ingress Port new size3 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS3PC_CPU_IGP_2 0x12050
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS3PC_CPU_IGP_2 0x19092050u

//! Register Reset Value
#define NS3PC_CPU_IGP_2_RST 0x00000000u

//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_2_NS3PC_POS 0
//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_2_NS3PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NEW_PBPTR_CPU_IGP_2 Register NEW_PBPTR_CPU_IGP_2 - CPU Ingress Port New Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_PBPTR_CPU_IGP_2 0x12080
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_PBPTR_CPU_IGP_2 0x19092080u

//! Register Reset Value
#define NEW_PBPTR_CPU_IGP_2_RST 0xFFFFFF80u

//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_2_OFFSET_MASK 0x7Fu

//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_2_SA_POS 7
//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_2_SA_MASK 0xFFFFFF80u

//! @}

//! \defgroup NEW_S0PTR_CPU_IGP_2 Register NEW_S0PTR_CPU_IGP_2 - CPU Ingress Port New Size0 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S0PTR_CPU_IGP_2 0x12084
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S0PTR_CPU_IGP_2 0x19092084u

//! Register Reset Value
#define NEW_S0PTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S1PTR_CPU_IGP_2 Register NEW_S1PTR_CPU_IGP_2 - CPU Ingress Port New Size1 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S1PTR_CPU_IGP_2 0x12088
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S1PTR_CPU_IGP_2 0x19092088u

//! Register Reset Value
#define NEW_S1PTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S2PTR_CPU_IGP_2 Register NEW_S2PTR_CPU_IGP_2 - CPU Ingress Port New Size2 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S2PTR_CPU_IGP_2 0x1208C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S2PTR_CPU_IGP_2 0x1909208Cu

//! Register Reset Value
#define NEW_S2PTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S3PTR_CPU_IGP_2 Register NEW_S3PTR_CPU_IGP_2 - CPU Ingress Port New Size3 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S3PTR_CPU_IGP_2 0x12090
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S3PTR_CPU_IGP_2 0x19092090u

//! Register Reset Value
#define NEW_S3PTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DCNTR_IGP_2 Register DCNTR_IGP_2 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_2 0x1209C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_2 0x1909209Cu

//! Register Reset Value
#define DCNTR_IGP_2_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_2_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_2_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_2_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_2_DCNT_MASK 0x3F00u

//! @}

//! \defgroup DESC0_0_CPU_IGP_2 Register DESC0_0_CPU_IGP_2 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_2 0x12100
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_0_CPU_IGP_2 0x19092100u

//! Register Reset Value
#define DESC0_0_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_IGP_2 Register DESC1_0_CPU_IGP_2 - CPU Ingress Port Descriptor 0 DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_IGP_2 0x12104
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_0_CPU_IGP_2 0x19092104u

//! Register Reset Value
#define DESC1_0_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_IGP_2 Register DESC2_0_CPU_IGP_2 - CPU Ingress Port Descriptor 0 DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_IGP_2 0x12108
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_0_CPU_IGP_2 0x19092108u

//! Register Reset Value
#define DESC2_0_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_IGP_2 Register DESC3_0_CPU_IGP_2 - CPU Ingress Port Descriptor 0 DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_IGP_2 0x1210C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_0_CPU_IGP_2 0x1909210Cu

//! Register Reset Value
#define DESC3_0_CPU_IGP_2_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_IGP_2 Register DESC0_1_CPU_IGP_2 - CPU Ingress Port Descriptor 1 DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_IGP_2 0x12110
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_1_CPU_IGP_2 0x19092110u

//! Register Reset Value
#define DESC0_1_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_IGP_2 Register DESC1_1_CPU_IGP_2 - CPU Ingress Port Descriptor 1 DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_IGP_2 0x12114
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_1_CPU_IGP_2 0x19092114u

//! Register Reset Value
#define DESC1_1_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_IGP_2 Register DESC2_1_CPU_IGP_2 - CPU Ingress Port Descriptor 1 DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_IGP_2 0x12118
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_1_CPU_IGP_2 0x19092118u

//! Register Reset Value
#define DESC2_1_CPU_IGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_IGP_2 Register DESC3_1_CPU_IGP_2 - CPU Ingress Port Descriptor 1 DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_IGP_2 0x1211C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_1_CPU_IGP_2 0x1909211Cu

//! Register Reset Value
#define DESC3_1_CPU_IGP_2_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_2_DESC_MASK 0x7FFFFFFFu

//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_2_OWN_POS 31
//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_2_OWN_MASK 0x80000000u

//! @}

//! \defgroup CFG_CPU_IGP_3 Register CFG_CPU_IGP_3 - CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_3 0x13000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_CPU_IGP_3 0x19093000u

//! Register Reset Value
#define CFG_CPU_IGP_3_RST 0x00000000u

//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_3_EQREQ_POS 0
//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_3_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_EQREQ_EN 0x1

//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_3_BUFREQ0_POS 1
//! Field BUFREQ0 - Enable CPU Buffer Request For Internal Packet Buffer Pointers
#define CFG_CPU_IGP_3_BUFREQ0_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQ0_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQ0_EN 0x1

//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_3_BUFREQ1_POS 2
//! Field BUFREQ1 - Enable CPU Buffer Request For Size0 Packet
#define CFG_CPU_IGP_3_BUFREQ1_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQ1_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQ1_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_3_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_3_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_DISCHALT_EN 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_3_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_3_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BRBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_3_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_3_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_3_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_3_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_3_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_3_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_CPU_IGP_3_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_CPU_IGP_3_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_CPU_IGP_3_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_CPU_IGP_3_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_CPU_IGP_3_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_CPU_IGP_3_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_CPU_IGP_3_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_CPU_IGP_3_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_CPU_IGP_3_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_CPU_IGP_3_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_CPU_IGP_3_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_CPU_IGP_3_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_CPU_IGP_3_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_CPU_IGP_3_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_CPU_IGP_3_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_CPU_IGP_3_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_CPU_IGP_3_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_CPU_IGP_3_DISC_REASON15 0xF

//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_3_BUFREQ2_POS 20
//! Field BUFREQ2 - Enable CPU Buffer Request For Size1 Packet
#define CFG_CPU_IGP_3_BUFREQ2_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQ2_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQ2_EN 0x1

//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_3_BUFREQ3_POS 21
//! Field BUFREQ3 - Enable CPU Buffer Request For Size2 Packet
#define CFG_CPU_IGP_3_BUFREQ3_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQ3_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQ3_EN 0x1

//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_3_BUFREQ4_POS 22
//! Field BUFREQ4 - Enable CPU Buffer Request For Size3 Packet
#define CFG_CPU_IGP_3_BUFREQ4_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQ4_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQ4_EN 0x1

//! @}

//! \defgroup EQPC_CPU_IGP_3 Register EQPC_CPU_IGP_3 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_3 0x1300C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_CPU_IGP_3 0x1909300Cu

//! Register Reset Value
#define EQPC_CPU_IGP_3_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_3_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_3_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_CPU_IGP_3 Register DISC_DESC0_CPU_IGP_3 - CPU Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_CPU_IGP_3 0x13010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_CPU_IGP_3 0x19093010u

//! Register Reset Value
#define DISC_DESC0_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_CPU_IGP_3 Register DISC_DESC1_CPU_IGP_3 - CPU Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_CPU_IGP_3 0x13014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_CPU_IGP_3 0x19093014u

//! Register Reset Value
#define DISC_DESC1_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_CPU_IGP_3 Register DISC_DESC2_CPU_IGP_3 - CPU Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_CPU_IGP_3 0x13018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_CPU_IGP_3 0x19093018u

//! Register Reset Value
#define DISC_DESC2_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_CPU_IGP_3 Register DISC_DESC3_CPU_IGP_3 - CPU Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_CPU_IGP_3 0x1301C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_CPU_IGP_3 0x1909301Cu

//! Register Reset Value
#define DISC_DESC3_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_IGP_3 Register IRNCR_CPU_IGP_3 - CPU Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_IGP_3 0x13020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_CPU_IGP_3 0x19093020u

//! Register Reset Value
#define IRNCR_CPU_IGP_3_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_3_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_3_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_3_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_3_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_DR_INTOCC 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_3_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNCR_CPU_IGP_3_PBR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_PBR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_PBR_INTOCC 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_3_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNCR_CPU_IGP_3_S0PR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_S0PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_S0PR_INTOCC 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_3_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNCR_CPU_IGP_3_S1PR_MASK 0x40u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_S1PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_S1PR_INTOCC 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_3_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNCR_CPU_IGP_3_S2PR_MASK 0x80u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_S2PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_S2PR_INTOCC 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_3_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNCR_CPU_IGP_3_S3PR_MASK 0x100u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_S3PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_S3PR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_3 Register IRNICR_CPU_IGP_3 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_3 0x13024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_CPU_IGP_3 0x19093024u

//! Register Reset Value
#define IRNICR_CPU_IGP_3_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_3_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_3_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_3_DR_MASK 0x8u

//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_3_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNICR_CPU_IGP_3_PBR_MASK 0x10u

//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_3_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNICR_CPU_IGP_3_S0PR_MASK 0x20u

//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_3_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNICR_CPU_IGP_3_S1PR_MASK 0x40u

//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_3_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNICR_CPU_IGP_3_S2PR_MASK 0x80u

//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_3_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNICR_CPU_IGP_3_S3PR_MASK 0x100u

//! @}

//! \defgroup IRNEN_CPU_IGP_3 Register IRNEN_CPU_IGP_3 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_3 0x13028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_CPU_IGP_3 0x19093028u

//! Register Reset Value
#define IRNEN_CPU_IGP_3_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_3_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_3_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_3_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_DR_EN 0x1

//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_3_PBR_POS 4
//! Field PBR - Packet Buffer Pointer Ready
#define IRNEN_CPU_IGP_3_PBR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_PBR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_PBR_EN 0x1

//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_3_S0PR_POS 5
//! Field S0PR - Size0 Pointer Ready
#define IRNEN_CPU_IGP_3_S0PR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_S0PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_S0PR_EN 0x1

//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_3_S1PR_POS 6
//! Field S1PR - Size1 Pointer Ready
#define IRNEN_CPU_IGP_3_S1PR_MASK 0x40u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_S1PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_S1PR_EN 0x1

//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_3_S2PR_POS 7
//! Field S2PR - Size2 Pointer Ready
#define IRNEN_CPU_IGP_3_S2PR_MASK 0x80u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_S2PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_S2PR_EN 0x1

//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_3_S3PR_POS 8
//! Field S3PR - Size3 Pointer Ready
#define IRNEN_CPU_IGP_3_S3PR_MASK 0x100u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_S3PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_S3PR_EN 0x1

//! @}

//! \defgroup DICC_IGP_3 Register DICC_IGP_3 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_3 0x13038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_3 0x19093038u

//! Register Reset Value
#define DICC_IGP_3_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_3_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_3_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_3 Register DROP_IGP_3 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_3 0x1303C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_3 0x1909303Cu

//! Register Reset Value
#define DROP_IGP_3_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_3_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_3_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NPBPC_CPU_IGP_3 Register NPBPC_CPU_IGP_3 - CPU Ingress Port new packet buffer pointer counter
//! @{

//! Register Offset (relative)
#define NPBPC_CPU_IGP_3 0x13040
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NPBPC_CPU_IGP_3 0x19093040u

//! Register Reset Value
#define NPBPC_CPU_IGP_3_RST 0x00000000u

//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_3_NPBPC_POS 0
//! Field NPBPC - Per CPU New Packet Buffer Pointer Counter
#define NPBPC_CPU_IGP_3_NPBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS0PC_CPU_IGP_3 Register NS0PC_CPU_IGP_3 - CPU Ingress Port new size0 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS0PC_CPU_IGP_3 0x13044
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS0PC_CPU_IGP_3 0x19093044u

//! Register Reset Value
#define NS0PC_CPU_IGP_3_RST 0x00000000u

//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_3_NS0PC_POS 0
//! Field NS0PC - Per CPU New Size0 Buffer Pointer Counter
#define NS0PC_CPU_IGP_3_NS0PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS1PC_CPU_IGP_3 Register NS1PC_CPU_IGP_3 - CPU Ingress Port new size1 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS1PC_CPU_IGP_3 0x13048
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS1PC_CPU_IGP_3 0x19093048u

//! Register Reset Value
#define NS1PC_CPU_IGP_3_RST 0x00000000u

//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_3_NS1PC_POS 0
//! Field NS1PC - Per CPU New Size1 Buffer Pointer Counter
#define NS1PC_CPU_IGP_3_NS1PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS2PC_CPU_IGP_3 Register NS2PC_CPU_IGP_3 - CPU Ingress Port new size2 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS2PC_CPU_IGP_3 0x1304C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS2PC_CPU_IGP_3 0x1909304Cu

//! Register Reset Value
#define NS2PC_CPU_IGP_3_RST 0x00000000u

//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_3_NS2PC_POS 0
//! Field NS2PC - Per CPU New Size1 Buffer Pointer Counter
#define NS2PC_CPU_IGP_3_NS2PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NS3PC_CPU_IGP_3 Register NS3PC_CPU_IGP_3 - CPU Ingress Port new size3 buffer pointer counter
//! @{

//! Register Offset (relative)
#define NS3PC_CPU_IGP_3 0x13050
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NS3PC_CPU_IGP_3 0x19093050u

//! Register Reset Value
#define NS3PC_CPU_IGP_3_RST 0x00000000u

//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_3_NS3PC_POS 0
//! Field NS3PC - Per CPU New Size3 Buffer Pointer Counter
#define NS3PC_CPU_IGP_3_NS3PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NEW_PBPTR_CPU_IGP_3 Register NEW_PBPTR_CPU_IGP_3 - CPU Ingress Port New Packet Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_PBPTR_CPU_IGP_3 0x13080
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_PBPTR_CPU_IGP_3 0x19093080u

//! Register Reset Value
#define NEW_PBPTR_CPU_IGP_3_RST 0xFFFFFF80u

//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_PBPTR_CPU_IGP_3_OFFSET_MASK 0x7Fu

//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_3_SA_POS 7
//! Field SA - Segment Address
#define NEW_PBPTR_CPU_IGP_3_SA_MASK 0xFFFFFF80u

//! @}

//! \defgroup NEW_S0PTR_CPU_IGP_3 Register NEW_S0PTR_CPU_IGP_3 - CPU Ingress Port New Size0 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S0PTR_CPU_IGP_3 0x13084
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S0PTR_CPU_IGP_3 0x19093084u

//! Register Reset Value
#define NEW_S0PTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S0PTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_S0PTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S1PTR_CPU_IGP_3 Register NEW_S1PTR_CPU_IGP_3 - CPU Ingress Port New Size1 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S1PTR_CPU_IGP_3 0x13088
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S1PTR_CPU_IGP_3 0x19093088u

//! Register Reset Value
#define NEW_S1PTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S1PTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_S1PTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S2PTR_CPU_IGP_3 Register NEW_S2PTR_CPU_IGP_3 - CPU Ingress Port New Size2 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S2PTR_CPU_IGP_3 0x1308C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S2PTR_CPU_IGP_3 0x1909308Cu

//! Register Reset Value
#define NEW_S2PTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S2PTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_S2PTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_S3PTR_CPU_IGP_3 Register NEW_S3PTR_CPU_IGP_3 - CPU Ingress Port New Size3 Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_S3PTR_CPU_IGP_3 0x13090
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_NEW_S3PTR_CPU_IGP_3 0x19093090u

//! Register Reset Value
#define NEW_S3PTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_S3PTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_S3PTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DCNTR_IGP_3 Register DCNTR_IGP_3 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_3 0x1309C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_3 0x1909309Cu

//! Register Reset Value
#define DCNTR_IGP_3_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_3_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_3_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_3_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_3_DCNT_MASK 0x3F00u

//! @}

//! \defgroup DESC0_0_CPU_IGP_3 Register DESC0_0_CPU_IGP_3 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_3 0x13100
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_0_CPU_IGP_3 0x19093100u

//! Register Reset Value
#define DESC0_0_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_IGP_3 Register DESC1_0_CPU_IGP_3 - CPU Ingress Port Descriptor 0 DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_IGP_3 0x13104
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_0_CPU_IGP_3 0x19093104u

//! Register Reset Value
#define DESC1_0_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_IGP_3 Register DESC2_0_CPU_IGP_3 - CPU Ingress Port Descriptor 0 DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_IGP_3 0x13108
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_0_CPU_IGP_3 0x19093108u

//! Register Reset Value
#define DESC2_0_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_IGP_3 Register DESC3_0_CPU_IGP_3 - CPU Ingress Port Descriptor 0 DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_IGP_3 0x1310C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_0_CPU_IGP_3 0x1909310Cu

//! Register Reset Value
#define DESC3_0_CPU_IGP_3_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_IGP_3 Register DESC0_1_CPU_IGP_3 - CPU Ingress Port Descriptor 1 DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_IGP_3 0x13110
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC0_1_CPU_IGP_3 0x19093110u

//! Register Reset Value
#define DESC0_1_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_IGP_3 Register DESC1_1_CPU_IGP_3 - CPU Ingress Port Descriptor 1 DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_IGP_3 0x13114
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC1_1_CPU_IGP_3 0x19093114u

//! Register Reset Value
#define DESC1_1_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_IGP_3 Register DESC2_1_CPU_IGP_3 - CPU Ingress Port Descriptor 1 DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_IGP_3 0x13118
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC2_1_CPU_IGP_3 0x19093118u

//! Register Reset Value
#define DESC2_1_CPU_IGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_IGP_3 Register DESC3_1_CPU_IGP_3 - CPU Ingress Port Descriptor 1 DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_IGP_3 0x1311C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DESC3_1_CPU_IGP_3 0x1909311Cu

//! Register Reset Value
#define DESC3_1_CPU_IGP_3_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_3_DESC_MASK 0x7FFFFFFFu

//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_3_OWN_POS 31
//! Field OWN - OWN bit
#define DESC3_1_CPU_IGP_3_OWN_MASK 0x80000000u

//! @}

//! \defgroup CFG_DMA_IGP_4 Register CFG_DMA_IGP_4 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_4 0x14000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_4 0x19094000u

//! Register Reset Value
#define CFG_DMA_IGP_4_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_4_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_4_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_4_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_4_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_4_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_4_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_4_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_4_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_4_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_4_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_4_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_4_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_4_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_4_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_4_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_4_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_4_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_4_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_4_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_4_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_4_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_4_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_4_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_4_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_4_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_4_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_4_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_4_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_4_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_4_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_4_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_4_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_4_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_4_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_4 Register EQPC_DMA_IGP_4 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_4 0x1400C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_4 0x1909400Cu

//! Register Reset Value
#define EQPC_DMA_IGP_4_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_4_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_4_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_4 Register DISC_DESC0_DMA_IGP_4 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_4 0x14010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_4 0x19094010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_4 Register DISC_DESC1_DMA_IGP_4 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_4 0x14014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_4 0x19094014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_4 Register DISC_DESC2_DMA_IGP_4 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_4 0x14018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_4 0x19094018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_4 Register DISC_DESC3_DMA_IGP_4 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_4 0x1401C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_4 0x1909401Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_4 Register IRNCR_DMA_IGP_4 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_4 0x14020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_4 0x19094020u

//! Register Reset Value
#define IRNCR_DMA_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_4_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_4_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_4_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_4_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_4_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_4_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_4_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_4 Register IRNICR_DMA_IGP_4 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_4 0x14024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_4 0x19094024u

//! Register Reset Value
#define IRNICR_DMA_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_4_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_4_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_4_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_4 Register IRNEN_DMA_IGP_4 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_4 0x14028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_4 0x19094028u

//! Register Reset Value
#define IRNEN_DMA_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_4_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_4_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_4_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_4_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_4_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_4_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_4_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_4 Register DPTR_DMA_IGP_4 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_4 0x14030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_4 0x19094030u

//! Register Reset Value
#define DPTR_DMA_IGP_4_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_4_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_4_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_4_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_4_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_4 Register DICC_IGP_4 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_4 0x14038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_4 0x19094038u

//! Register Reset Value
#define DICC_IGP_4_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_4_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_4_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_4 Register DROP_IGP_4 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_4 0x1403C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_4 0x1909403Cu

//! Register Reset Value
#define DROP_IGP_4_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_4_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_4_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_4 Register DCNTR_IGP_4 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_4 0x1409C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_4 0x1909409Cu

//! Register Reset Value
#define DCNTR_IGP_4_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_4_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_4_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_4_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_4_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_5 Register CFG_DMA_IGP_5 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_5 0x15000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_5 0x19095000u

//! Register Reset Value
#define CFG_DMA_IGP_5_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_5_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_5_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_5_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_5_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_5_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_5_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_5_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_5_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_5_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_5_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_5_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_5_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_5_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_5_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_5_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_5_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_5_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_5_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_5_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_5_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_5_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_5_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_5_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_5_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_5_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_5_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_5_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_5_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_5_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_5_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_5_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_5_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_5_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_5_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_5 Register EQPC_DMA_IGP_5 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_5 0x1500C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_5 0x1909500Cu

//! Register Reset Value
#define EQPC_DMA_IGP_5_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_5_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_5_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_5 Register DISC_DESC0_DMA_IGP_5 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_5 0x15010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_5 0x19095010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_5 Register DISC_DESC1_DMA_IGP_5 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_5 0x15014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_5 0x19095014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_5 Register DISC_DESC2_DMA_IGP_5 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_5 0x15018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_5 0x19095018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_5 Register DISC_DESC3_DMA_IGP_5 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_5 0x1501C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_5 0x1909501Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_5 Register IRNCR_DMA_IGP_5 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_5 0x15020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_5 0x19095020u

//! Register Reset Value
#define IRNCR_DMA_IGP_5_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_5_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_5_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_5_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_5_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_5_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_5_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_5 Register IRNICR_DMA_IGP_5 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_5 0x15024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_5 0x19095024u

//! Register Reset Value
#define IRNICR_DMA_IGP_5_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_5_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_5_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_5_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_5_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_5 Register IRNEN_DMA_IGP_5 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_5 0x15028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_5 0x19095028u

//! Register Reset Value
#define IRNEN_DMA_IGP_5_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_5_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_5_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_5_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_5_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_5 Register DPTR_DMA_IGP_5 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_5 0x15030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_5 0x19095030u

//! Register Reset Value
#define DPTR_DMA_IGP_5_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_5_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_5_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_5_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_5_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_5 Register DICC_IGP_5 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_5 0x15038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_5 0x19095038u

//! Register Reset Value
#define DICC_IGP_5_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_5_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_5_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_5 Register DROP_IGP_5 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_5 0x1503C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_5 0x1909503Cu

//! Register Reset Value
#define DROP_IGP_5_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_5_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_5_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_5 Register DCNTR_IGP_5 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_5 0x1509C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_5 0x1909509Cu

//! Register Reset Value
#define DCNTR_IGP_5_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_5_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_5_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_5_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_5_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_6 Register CFG_DMA_IGP_6 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_6 0x16000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_6 0x19096000u

//! Register Reset Value
#define CFG_DMA_IGP_6_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_6_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_6_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_6_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_6_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_6_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_6_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_6_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_6_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_6_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_6_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_6_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_6_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_6_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_6_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_6_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_6_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_6_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_6_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_6_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_6_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_6_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_6_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_6_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_6_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_6_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_6_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_6_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_6_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_6_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_6_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_6_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_6_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_6_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_6_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_6 Register EQPC_DMA_IGP_6 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_6 0x1600C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_6 0x1909600Cu

//! Register Reset Value
#define EQPC_DMA_IGP_6_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_6_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_6_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_6 Register DISC_DESC0_DMA_IGP_6 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_6 0x16010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_6 0x19096010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_6 Register DISC_DESC1_DMA_IGP_6 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_6 0x16014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_6 0x19096014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_6 Register DISC_DESC2_DMA_IGP_6 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_6 0x16018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_6 0x19096018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_6 Register DISC_DESC3_DMA_IGP_6 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_6 0x1601C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_6 0x1909601Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_6 Register IRNCR_DMA_IGP_6 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_6 0x16020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_6 0x19096020u

//! Register Reset Value
#define IRNCR_DMA_IGP_6_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_6_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_6_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_6_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_6_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_6_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_6_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_6 Register IRNICR_DMA_IGP_6 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_6 0x16024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_6 0x19096024u

//! Register Reset Value
#define IRNICR_DMA_IGP_6_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_6_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_6_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_6_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_6_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_6 Register IRNEN_DMA_IGP_6 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_6 0x16028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_6 0x19096028u

//! Register Reset Value
#define IRNEN_DMA_IGP_6_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_6_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_6_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_6_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_6_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_6 Register DPTR_DMA_IGP_6 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_6 0x16030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_6 0x19096030u

//! Register Reset Value
#define DPTR_DMA_IGP_6_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_6_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_6_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_6_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_6_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_6 Register DICC_IGP_6 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_6 0x16038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_6 0x19096038u

//! Register Reset Value
#define DICC_IGP_6_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_6_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_6_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_6 Register DROP_IGP_6 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_6 0x1603C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_6 0x1909603Cu

//! Register Reset Value
#define DROP_IGP_6_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_6_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_6_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_6 Register DCNTR_IGP_6 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_6 0x1609C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_6 0x1909609Cu

//! Register Reset Value
#define DCNTR_IGP_6_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_6_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_6_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_6_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_6_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_7 Register CFG_DMA_IGP_7 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_7 0x17000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_7 0x19097000u

//! Register Reset Value
#define CFG_DMA_IGP_7_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_7_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_7_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_7_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_7_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_7_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_7_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_7_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_7_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_7_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_7_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_7_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_7_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_7_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_7_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_7_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_7_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_7_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_7_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_7_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_7_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_7_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_7_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_7_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_7_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_7_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_7_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_7_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_7_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_7_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_7_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_7_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_7_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_7_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_7_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_7 Register EQPC_DMA_IGP_7 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_7 0x1700C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_7 0x1909700Cu

//! Register Reset Value
#define EQPC_DMA_IGP_7_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_7_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_7_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_7 Register DISC_DESC0_DMA_IGP_7 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_7 0x17010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_7 0x19097010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_7 Register DISC_DESC1_DMA_IGP_7 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_7 0x17014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_7 0x19097014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_7 Register DISC_DESC2_DMA_IGP_7 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_7 0x17018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_7 0x19097018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_7 Register DISC_DESC3_DMA_IGP_7 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_7 0x1701C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_7 0x1909701Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_7 Register IRNCR_DMA_IGP_7 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_7 0x17020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_7 0x19097020u

//! Register Reset Value
#define IRNCR_DMA_IGP_7_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_7_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_7_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_7_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_7_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_7_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_7_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_7 Register IRNICR_DMA_IGP_7 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_7 0x17024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_7 0x19097024u

//! Register Reset Value
#define IRNICR_DMA_IGP_7_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_7_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_7_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_7_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_7_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_7 Register IRNEN_DMA_IGP_7 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_7 0x17028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_7 0x19097028u

//! Register Reset Value
#define IRNEN_DMA_IGP_7_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_7_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_7_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_7_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_7_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_7 Register DPTR_DMA_IGP_7 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_7 0x17030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_7 0x19097030u

//! Register Reset Value
#define DPTR_DMA_IGP_7_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_7_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_7_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_7_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_7_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_7 Register DICC_IGP_7 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_7 0x17038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_7 0x19097038u

//! Register Reset Value
#define DICC_IGP_7_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_7_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_7_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_7 Register DROP_IGP_7 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_7 0x1703C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_7 0x1909703Cu

//! Register Reset Value
#define DROP_IGP_7_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_7_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_7_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_7 Register DCNTR_IGP_7 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_7 0x1709C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_7 0x1909709Cu

//! Register Reset Value
#define DCNTR_IGP_7_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_7_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_7_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_7_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_7_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_8 Register CFG_DMA_IGP_8 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_8 0x18000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_8 0x19098000u

//! Register Reset Value
#define CFG_DMA_IGP_8_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_8_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_8_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_8_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_8_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_8_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_8_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_8_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_8_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_8_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_8_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_8_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_8_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_8_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_8_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_8_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_8_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_8_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_8_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_8_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_8_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_8_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_8_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_8_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_8_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_8_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_8_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_8_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_8_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_8_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_8_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_8_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_8_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_8_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_8_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_8 Register EQPC_DMA_IGP_8 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_8 0x1800C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_8 0x1909800Cu

//! Register Reset Value
#define EQPC_DMA_IGP_8_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_8_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_8_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_8 Register DISC_DESC0_DMA_IGP_8 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_8 0x18010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_8 0x19098010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_8 Register DISC_DESC1_DMA_IGP_8 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_8 0x18014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_8 0x19098014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_8 Register DISC_DESC2_DMA_IGP_8 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_8 0x18018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_8 0x19098018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_8 Register DISC_DESC3_DMA_IGP_8 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_8 0x1801C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_8 0x1909801Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_8 Register IRNCR_DMA_IGP_8 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_8 0x18020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_8 0x19098020u

//! Register Reset Value
#define IRNCR_DMA_IGP_8_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_8_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_8_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_8_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_8_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_8_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_8_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_8 Register IRNICR_DMA_IGP_8 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_8 0x18024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_8 0x19098024u

//! Register Reset Value
#define IRNICR_DMA_IGP_8_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_8_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_8_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_8_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_8_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_8 Register IRNEN_DMA_IGP_8 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_8 0x18028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_8 0x19098028u

//! Register Reset Value
#define IRNEN_DMA_IGP_8_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_8_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_8_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_8_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_8_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_8 Register DPTR_DMA_IGP_8 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_8 0x18030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_8 0x19098030u

//! Register Reset Value
#define DPTR_DMA_IGP_8_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_8_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_8_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_8_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_8_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_8 Register DICC_IGP_8 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_8 0x18038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_8 0x19098038u

//! Register Reset Value
#define DICC_IGP_8_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_8_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_8_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_8 Register DROP_IGP_8 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_8 0x1803C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_8 0x1909803Cu

//! Register Reset Value
#define DROP_IGP_8_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_8_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_8_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_8 Register DCNTR_IGP_8 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_8 0x1809C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_8 0x1909809Cu

//! Register Reset Value
#define DCNTR_IGP_8_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_8_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_8_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_8_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_8_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_9 Register CFG_DMA_IGP_9 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_9 0x19000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_9 0x19099000u

//! Register Reset Value
#define CFG_DMA_IGP_9_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_9_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_9_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_9_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_9_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_9_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_9_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_9_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_9_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_9_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_9_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_9_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_9_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_9_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_9_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_9_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_9_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_9_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_9_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_9_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_9_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_9_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_9_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_9_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_9_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_9_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_9_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_9_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_9_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_9_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_9_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_9_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_9_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_9_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_9_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_9 Register EQPC_DMA_IGP_9 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_9 0x1900C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_9 0x1909900Cu

//! Register Reset Value
#define EQPC_DMA_IGP_9_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_9_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_9_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_9 Register DISC_DESC0_DMA_IGP_9 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_9 0x19010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_9 0x19099010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_9 Register DISC_DESC1_DMA_IGP_9 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_9 0x19014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_9 0x19099014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_9 Register DISC_DESC2_DMA_IGP_9 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_9 0x19018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_9 0x19099018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_9 Register DISC_DESC3_DMA_IGP_9 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_9 0x1901C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_9 0x1909901Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_9 Register IRNCR_DMA_IGP_9 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_9 0x19020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_9 0x19099020u

//! Register Reset Value
#define IRNCR_DMA_IGP_9_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_9_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_9_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_9_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_9_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_9_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_9_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_9 Register IRNICR_DMA_IGP_9 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_9 0x19024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_9 0x19099024u

//! Register Reset Value
#define IRNICR_DMA_IGP_9_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_9_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_9_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_9_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_9_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_9 Register IRNEN_DMA_IGP_9 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_9 0x19028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_9 0x19099028u

//! Register Reset Value
#define IRNEN_DMA_IGP_9_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_9_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_9_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_9_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_9_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_9 Register DPTR_DMA_IGP_9 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_9 0x19030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_9 0x19099030u

//! Register Reset Value
#define DPTR_DMA_IGP_9_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_9_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_9_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_9_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_9_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_9 Register DICC_IGP_9 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_9 0x19038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_9 0x19099038u

//! Register Reset Value
#define DICC_IGP_9_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_9_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_9_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_9 Register DROP_IGP_9 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_9 0x1903C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_9 0x1909903Cu

//! Register Reset Value
#define DROP_IGP_9_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_9_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_9_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_9 Register DCNTR_IGP_9 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_9 0x1909C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_9 0x1909909Cu

//! Register Reset Value
#define DCNTR_IGP_9_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_9_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_9_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_9_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_9_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_10 Register CFG_DMA_IGP_10 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_10 0x1A000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_10 0x1909A000u

//! Register Reset Value
#define CFG_DMA_IGP_10_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_10_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_10_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_10_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_10_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_10_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_10_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_10_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_10_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_10_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_10_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_10_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_10_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_10_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_10_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_10_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_10_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_10_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_10_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_10_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_10_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_10_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_10_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_10_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_10_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_10_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_10_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_10_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_10_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_10_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_10_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_10_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_10_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_10_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_10_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_10 Register EQPC_DMA_IGP_10 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_10 0x1A00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_10 0x1909A00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_10_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_10_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_10_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_10 Register DISC_DESC0_DMA_IGP_10 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_10 0x1A010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_10 0x1909A010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_10 Register DISC_DESC1_DMA_IGP_10 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_10 0x1A014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_10 0x1909A014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_10 Register DISC_DESC2_DMA_IGP_10 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_10 0x1A018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_10 0x1909A018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_10 Register DISC_DESC3_DMA_IGP_10 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_10 0x1A01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_10 0x1909A01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_10 Register IRNCR_DMA_IGP_10 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_10 0x1A020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_10 0x1909A020u

//! Register Reset Value
#define IRNCR_DMA_IGP_10_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_10_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_10_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_10_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_10_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_10_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_10_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_10 Register IRNICR_DMA_IGP_10 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_10 0x1A024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_10 0x1909A024u

//! Register Reset Value
#define IRNICR_DMA_IGP_10_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_10_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_10_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_10_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_10_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_10 Register IRNEN_DMA_IGP_10 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_10 0x1A028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_10 0x1909A028u

//! Register Reset Value
#define IRNEN_DMA_IGP_10_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_10_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_10_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_10_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_10_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_10 Register DPTR_DMA_IGP_10 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_10 0x1A030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_10 0x1909A030u

//! Register Reset Value
#define DPTR_DMA_IGP_10_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_10_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_10_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_10_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_10_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_10 Register DICC_IGP_10 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_10 0x1A038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_10 0x1909A038u

//! Register Reset Value
#define DICC_IGP_10_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_10_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_10_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_10 Register DROP_IGP_10 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_10 0x1A03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_10 0x1909A03Cu

//! Register Reset Value
#define DROP_IGP_10_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_10_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_10_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_10 Register DCNTR_IGP_10 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_10 0x1A09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_10 0x1909A09Cu

//! Register Reset Value
#define DCNTR_IGP_10_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_10_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_10_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_10_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_10_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_11 Register CFG_DMA_IGP_11 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_11 0x1B000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_11 0x1909B000u

//! Register Reset Value
#define CFG_DMA_IGP_11_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_11_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_11_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_11_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_11_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_11_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_11_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_11_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_11_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_11_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_11_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_11_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_11_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_11_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_11_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_11_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_11_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_11_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_11_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_11_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_11_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_11_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_11_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_11_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_11_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_11_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_11_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_11_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_11_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_11_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_11_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_11_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_11_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_11_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_11_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_11 Register EQPC_DMA_IGP_11 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_11 0x1B00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_11 0x1909B00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_11_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_11_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_11_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_11 Register DISC_DESC0_DMA_IGP_11 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_11 0x1B010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_11 0x1909B010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_11 Register DISC_DESC1_DMA_IGP_11 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_11 0x1B014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_11 0x1909B014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_11 Register DISC_DESC2_DMA_IGP_11 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_11 0x1B018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_11 0x1909B018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_11 Register DISC_DESC3_DMA_IGP_11 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_11 0x1B01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_11 0x1909B01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_11 Register IRNCR_DMA_IGP_11 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_11 0x1B020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_11 0x1909B020u

//! Register Reset Value
#define IRNCR_DMA_IGP_11_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_11_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_11_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_11_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_11_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_11_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_11_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_11 Register IRNICR_DMA_IGP_11 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_11 0x1B024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_11 0x1909B024u

//! Register Reset Value
#define IRNICR_DMA_IGP_11_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_11_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_11_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_11_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_11_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_11 Register IRNEN_DMA_IGP_11 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_11 0x1B028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_11 0x1909B028u

//! Register Reset Value
#define IRNEN_DMA_IGP_11_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_11_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_11_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_11_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_11_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_11 Register DPTR_DMA_IGP_11 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_11 0x1B030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_11 0x1909B030u

//! Register Reset Value
#define DPTR_DMA_IGP_11_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_11_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_11_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_11_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_11_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_11 Register DICC_IGP_11 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_11 0x1B038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_11 0x1909B038u

//! Register Reset Value
#define DICC_IGP_11_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_11_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_11_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_11 Register DROP_IGP_11 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_11 0x1B03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_11 0x1909B03Cu

//! Register Reset Value
#define DROP_IGP_11_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_11_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_11_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_11 Register DCNTR_IGP_11 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_11 0x1B09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_11 0x1909B09Cu

//! Register Reset Value
#define DCNTR_IGP_11_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_11_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_11_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_11_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_11_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_12 Register CFG_DMA_IGP_12 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_12 0x1C000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_12 0x1909C000u

//! Register Reset Value
#define CFG_DMA_IGP_12_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_12_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_12_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_12_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_12_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_12_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_12_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_12_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_12_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_12_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_12_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_12_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_12_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_12_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_12_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_12_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_12_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_12_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_12_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_12_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_12_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_12_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_12_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_12_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_12_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_12_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_12_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_12_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_12_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_12_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_12_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_12_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_12_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_12_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_12_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_12 Register EQPC_DMA_IGP_12 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_12 0x1C00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_12 0x1909C00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_12_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_12_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_12_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_12 Register DISC_DESC0_DMA_IGP_12 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_12 0x1C010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_12 0x1909C010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_12 Register DISC_DESC1_DMA_IGP_12 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_12 0x1C014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_12 0x1909C014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_12 Register DISC_DESC2_DMA_IGP_12 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_12 0x1C018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_12 0x1909C018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_12 Register DISC_DESC3_DMA_IGP_12 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_12 0x1C01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_12 0x1909C01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_12 Register IRNCR_DMA_IGP_12 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_12 0x1C020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_12 0x1909C020u

//! Register Reset Value
#define IRNCR_DMA_IGP_12_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_12_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_12_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_12_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_12_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_12_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_12_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_12 Register IRNICR_DMA_IGP_12 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_12 0x1C024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_12 0x1909C024u

//! Register Reset Value
#define IRNICR_DMA_IGP_12_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_12_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_12_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_12_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_12_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_12 Register IRNEN_DMA_IGP_12 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_12 0x1C028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_12 0x1909C028u

//! Register Reset Value
#define IRNEN_DMA_IGP_12_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_12_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_12_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_12_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_12_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_12 Register DPTR_DMA_IGP_12 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_12 0x1C030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_12 0x1909C030u

//! Register Reset Value
#define DPTR_DMA_IGP_12_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_12_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_12_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_12_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_12_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_12 Register DICC_IGP_12 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_12 0x1C038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_12 0x1909C038u

//! Register Reset Value
#define DICC_IGP_12_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_12_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_12_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_12 Register DROP_IGP_12 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_12 0x1C03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_12 0x1909C03Cu

//! Register Reset Value
#define DROP_IGP_12_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_12_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_12_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_12 Register DCNTR_IGP_12 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_12 0x1C09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_12 0x1909C09Cu

//! Register Reset Value
#define DCNTR_IGP_12_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_12_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_12_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_12_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_12_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_13 Register CFG_DMA_IGP_13 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_13 0x1D000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_13 0x1909D000u

//! Register Reset Value
#define CFG_DMA_IGP_13_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_13_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_13_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_13_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_13_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_13_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_13_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_13_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_13_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_13_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_13_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_13_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_13_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_13_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_13_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_13_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_13_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_13_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_13_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_13_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_13_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_13_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_13_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_13_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_13_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_13_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_13_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_13_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_13_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_13_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_13_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_13_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_13_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_13_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_13_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_13 Register EQPC_DMA_IGP_13 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_13 0x1D00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_13 0x1909D00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_13_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_13_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_13_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_13 Register DISC_DESC0_DMA_IGP_13 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_13 0x1D010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_13 0x1909D010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_13 Register DISC_DESC1_DMA_IGP_13 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_13 0x1D014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_13 0x1909D014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_13 Register DISC_DESC2_DMA_IGP_13 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_13 0x1D018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_13 0x1909D018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_13 Register DISC_DESC3_DMA_IGP_13 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_13 0x1D01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_13 0x1909D01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_13 Register IRNCR_DMA_IGP_13 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_13 0x1D020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_13 0x1909D020u

//! Register Reset Value
#define IRNCR_DMA_IGP_13_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_13_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_13_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_13_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_13_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_13_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_13_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_13 Register IRNICR_DMA_IGP_13 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_13 0x1D024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_13 0x1909D024u

//! Register Reset Value
#define IRNICR_DMA_IGP_13_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_13_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_13_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_13_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_13_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_13 Register IRNEN_DMA_IGP_13 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_13 0x1D028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_13 0x1909D028u

//! Register Reset Value
#define IRNEN_DMA_IGP_13_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_13_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_13_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_13_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_13_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_13 Register DPTR_DMA_IGP_13 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_13 0x1D030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_13 0x1909D030u

//! Register Reset Value
#define DPTR_DMA_IGP_13_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_13_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_13_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_13_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_13_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_13 Register DICC_IGP_13 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_13 0x1D038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_13 0x1909D038u

//! Register Reset Value
#define DICC_IGP_13_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_13_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_13_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_13 Register DROP_IGP_13 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_13 0x1D03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_13 0x1909D03Cu

//! Register Reset Value
#define DROP_IGP_13_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_13_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_13_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_13 Register DCNTR_IGP_13 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_13 0x1D09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_13 0x1909D09Cu

//! Register Reset Value
#define DCNTR_IGP_13_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_13_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_13_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_13_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_13_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_14 Register CFG_DMA_IGP_14 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_14 0x1E000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_14 0x1909E000u

//! Register Reset Value
#define CFG_DMA_IGP_14_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_14_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_14_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_14_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_14_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_14_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_14_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_14_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_14_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_14_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_14_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_14_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_14_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_14_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_14_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_14_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_14_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_14_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_14_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_14_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_14_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_14_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_14_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_14_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_14_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_14_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_14_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_14_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_14_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_14_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_14_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_14_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_14_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_14_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_14_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_14 Register EQPC_DMA_IGP_14 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_14 0x1E00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_14 0x1909E00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_14_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_14_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_14_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_14 Register DISC_DESC0_DMA_IGP_14 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_14 0x1E010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_14 0x1909E010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_14 Register DISC_DESC1_DMA_IGP_14 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_14 0x1E014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_14 0x1909E014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_14 Register DISC_DESC2_DMA_IGP_14 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_14 0x1E018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_14 0x1909E018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_14 Register DISC_DESC3_DMA_IGP_14 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_14 0x1E01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_14 0x1909E01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_14 Register IRNCR_DMA_IGP_14 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_14 0x1E020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_14 0x1909E020u

//! Register Reset Value
#define IRNCR_DMA_IGP_14_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_14_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_14_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_14_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_14_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_14_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_14_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_14 Register IRNICR_DMA_IGP_14 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_14 0x1E024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_14 0x1909E024u

//! Register Reset Value
#define IRNICR_DMA_IGP_14_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_14_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_14_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_14_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_14_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_14 Register IRNEN_DMA_IGP_14 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_14 0x1E028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_14 0x1909E028u

//! Register Reset Value
#define IRNEN_DMA_IGP_14_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_14_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_14_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_14_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_14_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_14 Register DPTR_DMA_IGP_14 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_14 0x1E030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_14 0x1909E030u

//! Register Reset Value
#define DPTR_DMA_IGP_14_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_14_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_14_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_14_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_14_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_14 Register DICC_IGP_14 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_14 0x1E038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_14 0x1909E038u

//! Register Reset Value
#define DICC_IGP_14_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_14_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_14_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_14 Register DROP_IGP_14 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_14 0x1E03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_14 0x1909E03Cu

//! Register Reset Value
#define DROP_IGP_14_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_14_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_14_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_14 Register DCNTR_IGP_14 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_14 0x1E09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_14 0x1909E09Cu

//! Register Reset Value
#define DCNTR_IGP_14_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_14_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_14_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_14_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_14_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_15 Register CFG_DMA_IGP_15 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_15 0x1F000
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_CFG_DMA_IGP_15 0x1909F000u

//! Register Reset Value
#define CFG_DMA_IGP_15_RST 0x00000000u

//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_15_EQREQ_POS 0
//! Field EQREQ - Enable DMA Enqueue Request
#define CFG_DMA_IGP_15_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_15_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_15_EQREQ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_15_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_DMA_IGP_15_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_15_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_15_DISCHALT_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_15_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_15_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_15_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_15_EQPCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_15_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_15_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_15_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_15_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_DMA_IGP_15_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_DMA_IGP_15_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_DMA_IGP_15_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_DMA_IGP_15_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_DMA_IGP_15_DISC_REASON2 0x2
//! Constant REASON3 - Reason 3
#define CONST_CFG_DMA_IGP_15_DISC_REASON3 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_DMA_IGP_15_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_DMA_IGP_15_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_DMA_IGP_15_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_DMA_IGP_15_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_DMA_IGP_15_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_DMA_IGP_15_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_DMA_IGP_15_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_DMA_IGP_15_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_DMA_IGP_15_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_DMA_IGP_15_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_DMA_IGP_15_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_DMA_IGP_15_DISC_REASON15 0xF

//! @}

//! \defgroup EQPC_DMA_IGP_15 Register EQPC_DMA_IGP_15 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_15 0x1F00C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_EQPC_DMA_IGP_15 0x1909F00Cu

//! Register Reset Value
#define EQPC_DMA_IGP_15_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_15_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_DMA_IGP_15_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_DMA_IGP_15 Register DISC_DESC0_DMA_IGP_15 - DMA Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_DMA_IGP_15 0x1F010
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC0_DMA_IGP_15 0x1909F010u

//! Register Reset Value
#define DISC_DESC0_DMA_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_DMA_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_DMA_IGP_15 Register DISC_DESC1_DMA_IGP_15 - DMA Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_DMA_IGP_15 0x1F014
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC1_DMA_IGP_15 0x1909F014u

//! Register Reset Value
#define DISC_DESC1_DMA_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_DMA_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_DMA_IGP_15 Register DISC_DESC2_DMA_IGP_15 - DMA Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_DMA_IGP_15 0x1F018
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC2_DMA_IGP_15 0x1909F018u

//! Register Reset Value
#define DISC_DESC2_DMA_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_DMA_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_DMA_IGP_15 Register DISC_DESC3_DMA_IGP_15 - DMA Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_DMA_IGP_15 0x1F01C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DISC_DESC3_DMA_IGP_15 0x1909F01Cu

//! Register Reset Value
#define DISC_DESC3_DMA_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_DMA_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_IGP_15 Register IRNCR_DMA_IGP_15 - DMA Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_IGP_15 0x1F020
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNCR_DMA_IGP_15 0x1909F020u

//! Register Reset Value
#define IRNCR_DMA_IGP_15_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_15_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_DMA_IGP_15_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_15_PNA_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_15_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_DMA_IGP_15_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_15_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_15 Register IRNICR_DMA_IGP_15 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_15 0x1F024
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNICR_DMA_IGP_15 0x1909F024u

//! Register Reset Value
#define IRNICR_DMA_IGP_15_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_15_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_15_PNA_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_15_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_DMA_IGP_15_DR_MASK 0x8u

//! @}

//! \defgroup IRNEN_DMA_IGP_15 Register IRNEN_DMA_IGP_15 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_15 0x1F028
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_IRNEN_DMA_IGP_15 0x1909F028u

//! Register Reset Value
#define IRNEN_DMA_IGP_15_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_15_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_DMA_IGP_15_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_PNA_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_15_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_DMA_IGP_15_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_15 Register DPTR_DMA_IGP_15 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_15 0x1F030
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DPTR_DMA_IGP_15 0x1909F030u

//! Register Reset Value
#define DPTR_DMA_IGP_15_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_15_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_IGP_15_ND_MASK 0x7u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_15_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_IGP_15_DPTR_MASK 0x70000u

//! @}

//! \defgroup DICC_IGP_15 Register DICC_IGP_15 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_15 0x1F038
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DICC_IGP_15 0x1909F038u

//! Register Reset Value
#define DICC_IGP_15_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_15_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_15_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DROP_IGP_15 Register DROP_IGP_15 - Ingress Port Drop counter
//! @{

//! Register Offset (relative)
#define DROP_IGP_15 0x1F03C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DROP_IGP_15 0x1909F03Cu

//! Register Reset Value
#define DROP_IGP_15_RST 0x00000000u

//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_15_DROPC_POS 0
//! Field DROPC - Per Port Drop Counter
#define DROP_IGP_15_DROPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_15 Register DCNTR_IGP_15 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_15 0x1F09C
//! Register Offset (absolute) for 1st Instance CQEM_ENQ
#define CQEM_ENQ_DCNTR_IGP_15 0x1909F09Cu

//! Register Reset Value
#define DCNTR_IGP_15_RST 0x00000100u

//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_15_DLY_POS 0
//! Field DLY - Additional Delay before QoS enqueue
#define DCNTR_IGP_15_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_15_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_15_DCNT_MASK 0x3F00u

//! @}

//! @}

#endif
