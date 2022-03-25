//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : D:/Users/shij/Perforce/l1033.grx500.win.v_shij.priv.dfv.grx500.dfv/ipg_lsd/lsd_sys/source/xml/reg_files/CBM_EQM.xml
// Register File Name  : CBM_EQM
// Register File Title : Central Buffer Manager Enqueue Manager Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CBM_EQM_H
#define _CBM_EQM_H

//! \defgroup CBM_EQM Register File CBM_EQM - Central Buffer Manager Enqueue Manager Register Description
//! @{

//! Base Address of CBM_EQM
#define CBM_EQM_MODULE_BASE 0x1E780000u

//! \defgroup CBM_EQM_CTRL Register CBM_EQM_CTRL - CBM Enqueue Manager Control Register
//! @{

//! Register Offset (relative)
#define CBM_EQM_CTRL 0x0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CBM_EQM_CTRL 0x1E780000u

//! Register Reset Value
#define CBM_EQM_CTRL_RST 0x00000400u

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

//! Field EQQCEN - Enqueue Queue Counter Enable
#define CBM_EQM_CTRL_EQQCEN_POS 4
//! Field EQQCEN - Enqueue Queue Counter Enable
#define CBM_EQM_CTRL_EQQCEN_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_EQM_CTRL_EQQCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_EQM_CTRL_EQQCEN_EN 0x1

//! Field MSEL - Counter Mode Select
#define CBM_EQM_CTRL_MSEL_POS 5
//! Field MSEL - Counter Mode Select
#define CBM_EQM_CTRL_MSEL_MASK 0x20u
//! Constant PKT - Packet counter
#define CONST_CBM_EQM_CTRL_MSEL_PKT 0x0
//! Constant BYTE - Byte counter
#define CONST_CBM_EQM_CTRL_MSEL_BYTE 0x1

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
#define CBM_EQM_CTRL_L_MASK 0xE00u

//! @}

//! \defgroup DBG_EQM_0 Register DBG_EQM_0 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_EQM_0 0x10
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DBG_EQM_0 0x1E780010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DBG_EQM_1 0x1E780014u

//! Register Reset Value
#define DBG_EQM_1_RST 0xC0010000u

//! Field DBG - Debug
#define DBG_EQM_1_DBG_POS 0
//! Field DBG - Debug
#define DBG_EQM_1_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TEST_EQM_0 Register TEST_EQM_0 - Hardware Test Register
//! @{

//! Register Offset (relative)
#define TEST_EQM_0 0x20
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_TEST_EQM_0 0x1E780020u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_TEST_EQM_1 0x1E780024u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_0 0x1E780030u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_1 0x1E780034u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_2 0x1E780038u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_3 0x1E78003Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_4 0x1E780040u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_5 0x1E780044u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_6 0x1E780048u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DEN_7 0x1E78004Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_0 0x1E780050u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_1 0x1E780054u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_2 0x1E780058u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_3 0x1E78005Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_4 0x1E780060u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_5 0x1E780064u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_6 0x1E780068u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_7 0x1E78006Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_8 0x1E780070u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_9 0x1E780074u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_10 0x1E780078u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_11 0x1E78007Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_12 0x1E780080u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_13 0x1E780084u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_14 0x1E780088u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_OVH_15 0x1E78008Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DMA_RDY_EN 0x1E780090u

//! Register Reset Value
#define DMA_RDY_EN_RST 0x0000003Fu

//! Field STD_EN0 - Standard Channel Ready Enable
#define DMA_RDY_EN_STD_EN0_POS 0
//! Field STD_EN0 - Standard Channel Ready Enable
#define DMA_RDY_EN_STD_EN0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_STD_EN0_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_STD_EN0_EN 0x1

//! Field JMB_EN0 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN0_POS 1
//! Field JMB_EN0 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN0_MASK 0x2u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_JMB_EN0_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_JMB_EN0_EN 0x1

//! Field STD_EN1 - Ready Enable
#define DMA_RDY_EN_STD_EN1_POS 2
//! Field STD_EN1 - Ready Enable
#define DMA_RDY_EN_STD_EN1_MASK 0x4u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_STD_EN1_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_STD_EN1_EN 0x1

//! Field JMB_EN1 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN1_POS 3
//! Field JMB_EN1 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN1_MASK 0x8u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_JMB_EN1_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_JMB_EN1_EN 0x1

//! Field STD_EN2 - Ready Enable
#define DMA_RDY_EN_STD_EN2_POS 4
//! Field STD_EN2 - Ready Enable
#define DMA_RDY_EN_STD_EN2_MASK 0x10u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_STD_EN2_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_STD_EN2_EN 0x1

//! Field JMB_EN2 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN2_POS 5
//! Field JMB_EN2 - Jumbo Channel Ready Enable
#define DMA_RDY_EN_JMB_EN2_MASK 0x20u
//! Constant DIS - DIS
#define CONST_DMA_RDY_EN_JMB_EN2_DIS 0x0
//! Constant EN - EN
#define CONST_DMA_RDY_EN_JMB_EN2_EN 0x1

//! @}

//! \defgroup CFG_CPU_IGP_0 Register CFG_CPU_IGP_0 - CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_0 0x10000
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_CPU_IGP_0 0x1E790000u

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

//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_0_BUFREQS_POS 1
//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_0_BUFREQS_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQS_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQS_EN 0x1

//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_0_BUFREQJ_POS 2
//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_0_BUFREQJ_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BUFREQJ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BUFREQJ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_0_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_0_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_DISCHALT_EN 0x1

//! Field BAS - Base Address Select
#define CFG_CPU_IGP_0_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_CPU_IGP_0_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_CPU_IGP_0_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_CPU_IGP_0_BAS_BA1 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_0_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_0_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_BRBPEN_EN 0x1

//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_0_EQDBPEN_POS 6
//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_0_EQDBPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_EQDBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_EQDBPEN_EN 0x1

//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_0_RCNTBPEN_POS 7
//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_0_RCNTBPEN_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_RCNTBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_RCNTBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_0_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_0_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_0_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_0_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_0_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_0_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_0_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_0_BP_EN_EN 0x1

//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_0_RCNTEN_POS 11
//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_0_RCNTEN_MASK 0x800u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_0_RCNTEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_0_RCNTEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_0_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_0_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_CPU_IGP_0 Register WM_CPU_IGP_0 - CPU Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_CPU_IGP_0 0x10004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_CPU_IGP_0 0x1E790004u

//! Register Reset Value
#define WM_CPU_IGP_0_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_0_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_0_LWM_MASK 0x3FFFu

//! Field HWM - High watermark threshold
#define WM_CPU_IGP_0_HWM_POS 16
//! Field HWM - High watermark threshold
#define WM_CPU_IGP_0_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_CPU_IGP_0 Register POCC_CPU_IGP_0 - CPU Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_CPU_IGP_0 0x10008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_CPU_IGP_0 0x1E790008u

//! Register Reset Value
#define POCC_CPU_IGP_0_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_0_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_0_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_CPU_IGP_0 Register EQPC_CPU_IGP_0 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_0 0x1000C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_CPU_IGP_0 0x1E79000Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_CPU_IGP_0 0x1E790010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_CPU_IGP_0 0x1E790014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_CPU_IGP_0 0x1E790018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_CPU_IGP_0 0x1E79001Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_CPU_IGP_0 0x1E790020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_0_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_0_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_0_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_0_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_0_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_0_HWMI_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_0_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_DR_INTOCC 0x1

//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_0_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_0_SPR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_SPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_SPR_INTOCC 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_0_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_0_JPR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_0_JPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_0_JPR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_0 Register IRNICR_CPU_IGP_0 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_0 0x10024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_CPU_IGP_0 0x1E790024u

//! Register Reset Value
#define IRNICR_CPU_IGP_0_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_0_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_0_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_0_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_0_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_0_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_0_HWMI_MASK 0x4u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_0_DR_MASK 0x8u

//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_0_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_0_SPR_MASK 0x10u

//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_0_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_0_JPR_MASK 0x20u

//! @}

//! \defgroup IRNEN_CPU_IGP_0 Register IRNEN_CPU_IGP_0 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_0 0x10028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_CPU_IGP_0 0x1E790028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_0_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_0_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_0_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_0_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_HWMI_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_0_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_0_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_DR_EN 0x1

//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_0_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_0_SPR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_SPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_SPR_EN 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_0_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_0_JPR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_0_JPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_0_JPR_EN 0x1

//! @}

//! \defgroup RCNT_INC_CPU_IGP_0 Register RCNT_INC_CPU_IGP_0 - CPU Ingress Port Repeat Count Increment
//! @{

//! Register Offset (relative)
#define RCNT_INC_CPU_IGP_0 0x10034
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNT_INC_CPU_IGP_0 0x1E790034u

//! Register Reset Value
#define RCNT_INC_CPU_IGP_0_RST 0xFFFFF800u

//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_0_DP_POS 11
//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_0_DP_MASK 0xFFFFF800u

//! @}

//! \defgroup DICC_IGP_0 Register DICC_IGP_0 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_0 0x10038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_0 0x1E790038u

//! Register Reset Value
#define DICC_IGP_0_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_0_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_0_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup RCNTC_CPU_IGP_0 Register RCNTC_CPU_IGP_0 - CPU Ingress Port enqueue repeat count counter
//! @{

//! Register Offset (relative)
#define RCNTC_CPU_IGP_0 0x1003C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNTC_CPU_IGP_0 0x1E79003Cu

//! Register Reset Value
#define RCNTC_CPU_IGP_0_RST 0x00000000u

//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_0_RCNTC_POS 0
//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_0_RCNTC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NSBPC_CPU_IGP_0 Register NSBPC_CPU_IGP_0 - CPU Ingress Port new standard buffer pointer counter
//! @{

//! Register Offset (relative)
#define NSBPC_CPU_IGP_0 0x10040
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NSBPC_CPU_IGP_0 0x1E790040u

//! Register Reset Value
#define NSBPC_CPU_IGP_0_RST 0x00000000u

//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_0_NSBPC_POS 0
//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_0_NSBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NJBPC_CPU_IGP_0 Register NJBPC_CPU_IGP_0 - CPU Ingress Port new jumbo buffer pointer counter
//! @{

//! Register Offset (relative)
#define NJBPC_CPU_IGP_0 0x10044
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NJBPC_CPU_IGP_0 0x1E790044u

//! Register Reset Value
#define NJBPC_CPU_IGP_0_RST 0x00000000u

//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_0_NJBPC_POS 0
//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_0_NJBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_0 Register DCNTR_IGP_0 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_0 0x1004C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_0 0x1E79004Cu

//! Register Reset Value
#define DCNTR_IGP_0_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_0_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_0_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_0_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_0_DCNT_MASK 0x3F00u

//! @}

//! \defgroup NEW_SPTR_CPU_IGP_0 Register NEW_SPTR_CPU_IGP_0 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR_CPU_IGP_0 0x10080
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR_CPU_IGP_0 0x1E790080u

//! Register Reset Value
#define NEW_SPTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_JPTR_CPU_IGP_0 Register NEW_JPTR_CPU_IGP_0 - CPU Ingress Port New Jumbo Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_JPTR_CPU_IGP_0 0x10090
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_JPTR_CPU_IGP_0 0x1E790090u

//! Register Reset Value
#define NEW_JPTR_CPU_IGP_0_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_0_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_0_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_0_SA_POS 11
//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_0_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_IGP_0 Register DESC0_0_CPU_IGP_0 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_0 0x10100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_0_CPU_IGP_0 0x1E790100u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_0_CPU_IGP_0 0x1E790104u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_0_CPU_IGP_0 0x1E790108u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_0_CPU_IGP_0 0x1E79010Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_1_CPU_IGP_0 0x1E790110u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_1_CPU_IGP_0 0x1E790114u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_1_CPU_IGP_0 0x1E790118u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_1_CPU_IGP_0 0x1E79011Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_CPU_IGP_1 0x1E791000u

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

//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_1_BUFREQS_POS 1
//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_1_BUFREQS_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQS_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQS_EN 0x1

//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_1_BUFREQJ_POS 2
//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_1_BUFREQJ_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BUFREQJ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BUFREQJ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_1_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_1_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_DISCHALT_EN 0x1

//! Field BAS - Base Address Select
#define CFG_CPU_IGP_1_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_CPU_IGP_1_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_CPU_IGP_1_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_CPU_IGP_1_BAS_BA1 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_1_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_1_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_BRBPEN_EN 0x1

//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_1_EQDBPEN_POS 6
//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_1_EQDBPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_EQDBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_EQDBPEN_EN 0x1

//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_1_RCNTBPEN_POS 7
//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_1_RCNTBPEN_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_RCNTBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_RCNTBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_1_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_1_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_1_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_1_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_1_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_1_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_1_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_1_BP_EN_EN 0x1

//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_1_RCNTEN_POS 11
//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_1_RCNTEN_MASK 0x800u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_1_RCNTEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_1_RCNTEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_1_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_1_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_CPU_IGP_1 Register WM_CPU_IGP_1 - CPU Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_CPU_IGP_1 0x11004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_CPU_IGP_1 0x1E791004u

//! Register Reset Value
#define WM_CPU_IGP_1_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_1_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_1_LWM_MASK 0x3FFFu

//! Field HWM - High watermark threshold
#define WM_CPU_IGP_1_HWM_POS 16
//! Field HWM - High watermark threshold
#define WM_CPU_IGP_1_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_CPU_IGP_1 Register POCC_CPU_IGP_1 - CPU Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_CPU_IGP_1 0x11008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_CPU_IGP_1 0x1E791008u

//! Register Reset Value
#define POCC_CPU_IGP_1_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_1_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_1_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_CPU_IGP_1 Register EQPC_CPU_IGP_1 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_1 0x1100C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_CPU_IGP_1 0x1E79100Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_CPU_IGP_1 0x1E791010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_CPU_IGP_1 0x1E791014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_CPU_IGP_1 0x1E791018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_CPU_IGP_1 0x1E79101Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_CPU_IGP_1 0x1E791020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_1_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_1_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_1_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_1_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_1_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_1_HWMI_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_1_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_DR_INTOCC 0x1

//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_1_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_1_SPR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_SPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_SPR_INTOCC 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_1_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_1_JPR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_1_JPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_1_JPR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_1 Register IRNICR_CPU_IGP_1 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_1 0x11024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_CPU_IGP_1 0x1E791024u

//! Register Reset Value
#define IRNICR_CPU_IGP_1_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_1_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_1_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_1_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_1_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_1_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_1_HWMI_MASK 0x4u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_1_DR_MASK 0x8u

//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_1_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_1_SPR_MASK 0x10u

//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_1_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_1_JPR_MASK 0x20u

//! @}

//! \defgroup IRNEN_CPU_IGP_1 Register IRNEN_CPU_IGP_1 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_1 0x11028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_CPU_IGP_1 0x1E791028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_1_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_1_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_1_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_1_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_HWMI_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_1_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_1_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_DR_EN 0x1

//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_1_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_1_SPR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_SPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_SPR_EN 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_1_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_1_JPR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_1_JPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_1_JPR_EN 0x1

//! @}

//! \defgroup RCNT_INC_CPU_IGP_1 Register RCNT_INC_CPU_IGP_1 - CPU Ingress Port Repeat Count Increment
//! @{

//! Register Offset (relative)
#define RCNT_INC_CPU_IGP_1 0x11034
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNT_INC_CPU_IGP_1 0x1E791034u

//! Register Reset Value
#define RCNT_INC_CPU_IGP_1_RST 0xFFFFF800u

//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_1_DP_POS 11
//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_1_DP_MASK 0xFFFFF800u

//! @}

//! \defgroup DICC_IGP_1 Register DICC_IGP_1 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_1 0x11038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_1 0x1E791038u

//! Register Reset Value
#define DICC_IGP_1_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_1_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_1_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup RCNTC_CPU_IGP_1 Register RCNTC_CPU_IGP_1 - CPU Ingress Port enqueue repeat count counter
//! @{

//! Register Offset (relative)
#define RCNTC_CPU_IGP_1 0x1103C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNTC_CPU_IGP_1 0x1E79103Cu

//! Register Reset Value
#define RCNTC_CPU_IGP_1_RST 0x00000000u

//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_1_RCNTC_POS 0
//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_1_RCNTC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NSBPC_CPU_IGP_1 Register NSBPC_CPU_IGP_1 - CPU Ingress Port new standard buffer pointer counter
//! @{

//! Register Offset (relative)
#define NSBPC_CPU_IGP_1 0x11040
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NSBPC_CPU_IGP_1 0x1E791040u

//! Register Reset Value
#define NSBPC_CPU_IGP_1_RST 0x00000000u

//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_1_NSBPC_POS 0
//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_1_NSBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NJBPC_CPU_IGP_1 Register NJBPC_CPU_IGP_1 - CPU Ingress Port new jumbo buffer pointer counter
//! @{

//! Register Offset (relative)
#define NJBPC_CPU_IGP_1 0x11044
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NJBPC_CPU_IGP_1 0x1E791044u

//! Register Reset Value
#define NJBPC_CPU_IGP_1_RST 0x00000000u

//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_1_NJBPC_POS 0
//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_1_NJBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_1 Register DCNTR_IGP_1 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_1 0x1104C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_1 0x1E79104Cu

//! Register Reset Value
#define DCNTR_IGP_1_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_1_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_1_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_1_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_1_DCNT_MASK 0x3F00u

//! @}

//! \defgroup NEW_SPTR_CPU_IGP_1 Register NEW_SPTR_CPU_IGP_1 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR_CPU_IGP_1 0x11080
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR_CPU_IGP_1 0x1E791080u

//! Register Reset Value
#define NEW_SPTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_JPTR_CPU_IGP_1 Register NEW_JPTR_CPU_IGP_1 - CPU Ingress Port New Jumbo Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_JPTR_CPU_IGP_1 0x11090
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_JPTR_CPU_IGP_1 0x1E791090u

//! Register Reset Value
#define NEW_JPTR_CPU_IGP_1_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_1_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_1_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_1_SA_POS 11
//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_1_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_IGP_1 Register DESC0_0_CPU_IGP_1 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_1 0x11100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_0_CPU_IGP_1 0x1E791100u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_0_CPU_IGP_1 0x1E791104u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_0_CPU_IGP_1 0x1E791108u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_0_CPU_IGP_1 0x1E79110Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_1_CPU_IGP_1 0x1E791110u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_1_CPU_IGP_1 0x1E791114u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_1_CPU_IGP_1 0x1E791118u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_1_CPU_IGP_1 0x1E79111Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_CPU_IGP_2 0x1E792000u

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

//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_2_BUFREQS_POS 1
//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_2_BUFREQS_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQS_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQS_EN 0x1

//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_2_BUFREQJ_POS 2
//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_2_BUFREQJ_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BUFREQJ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BUFREQJ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_2_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_2_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_DISCHALT_EN 0x1

//! Field BAS - Base Address Select
#define CFG_CPU_IGP_2_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_CPU_IGP_2_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_CPU_IGP_2_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_CPU_IGP_2_BAS_BA1 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_2_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_2_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_BRBPEN_EN 0x1

//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_2_EQDBPEN_POS 6
//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_2_EQDBPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_EQDBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_EQDBPEN_EN 0x1

//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_2_RCNTBPEN_POS 7
//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_2_RCNTBPEN_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_RCNTBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_RCNTBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_2_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_2_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_2_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_2_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_2_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_2_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_2_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_2_BP_EN_EN 0x1

//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_2_RCNTEN_POS 11
//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_2_RCNTEN_MASK 0x800u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_2_RCNTEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_2_RCNTEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_2_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_2_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_CPU_IGP_2 Register WM_CPU_IGP_2 - CPU Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_CPU_IGP_2 0x12004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_CPU_IGP_2 0x1E792004u

//! Register Reset Value
#define WM_CPU_IGP_2_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_2_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_2_LWM_MASK 0x3FFFu

//! Field HWM - High watermark threshold
#define WM_CPU_IGP_2_HWM_POS 16
//! Field HWM - High watermark threshold
#define WM_CPU_IGP_2_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_CPU_IGP_2 Register POCC_CPU_IGP_2 - CPU Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_CPU_IGP_2 0x12008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_CPU_IGP_2 0x1E792008u

//! Register Reset Value
#define POCC_CPU_IGP_2_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_2_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_2_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_CPU_IGP_2 Register EQPC_CPU_IGP_2 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_2 0x1200C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_CPU_IGP_2 0x1E79200Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_CPU_IGP_2 0x1E792010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_CPU_IGP_2 0x1E792014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_CPU_IGP_2 0x1E792018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_CPU_IGP_2 0x1E79201Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_CPU_IGP_2 0x1E792020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_2_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_2_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_2_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_2_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_2_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_2_HWMI_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_2_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_DR_INTOCC 0x1

//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_2_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_2_SPR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_SPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_SPR_INTOCC 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_2_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_2_JPR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_2_JPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_2_JPR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_2 Register IRNICR_CPU_IGP_2 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_2 0x12024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_CPU_IGP_2 0x1E792024u

//! Register Reset Value
#define IRNICR_CPU_IGP_2_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_2_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_2_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_2_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_2_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_2_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_2_HWMI_MASK 0x4u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_2_DR_MASK 0x8u

//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_2_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_2_SPR_MASK 0x10u

//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_2_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_2_JPR_MASK 0x20u

//! @}

//! \defgroup IRNEN_CPU_IGP_2 Register IRNEN_CPU_IGP_2 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_2 0x12028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_CPU_IGP_2 0x1E792028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_2_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_2_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_2_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_2_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_HWMI_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_2_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_2_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_DR_EN 0x1

//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_2_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_2_SPR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_SPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_SPR_EN 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_2_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_2_JPR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_2_JPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_2_JPR_EN 0x1

//! @}

//! \defgroup RCNT_INC_CPU_IGP_2 Register RCNT_INC_CPU_IGP_2 - CPU Ingress Port Repeat Count Increment
//! @{

//! Register Offset (relative)
#define RCNT_INC_CPU_IGP_2 0x12034
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNT_INC_CPU_IGP_2 0x1E792034u

//! Register Reset Value
#define RCNT_INC_CPU_IGP_2_RST 0xFFFFF800u

//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_2_DP_POS 11
//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_2_DP_MASK 0xFFFFF800u

//! @}

//! \defgroup DICC_IGP_2 Register DICC_IGP_2 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_2 0x12038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_2 0x1E792038u

//! Register Reset Value
#define DICC_IGP_2_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_2_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_2_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup RCNTC_CPU_IGP_2 Register RCNTC_CPU_IGP_2 - CPU Ingress Port enqueue repeat count counter
//! @{

//! Register Offset (relative)
#define RCNTC_CPU_IGP_2 0x1203C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNTC_CPU_IGP_2 0x1E79203Cu

//! Register Reset Value
#define RCNTC_CPU_IGP_2_RST 0x00000000u

//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_2_RCNTC_POS 0
//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_2_RCNTC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NSBPC_CPU_IGP_2 Register NSBPC_CPU_IGP_2 - CPU Ingress Port new standard buffer pointer counter
//! @{

//! Register Offset (relative)
#define NSBPC_CPU_IGP_2 0x12040
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NSBPC_CPU_IGP_2 0x1E792040u

//! Register Reset Value
#define NSBPC_CPU_IGP_2_RST 0x00000000u

//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_2_NSBPC_POS 0
//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_2_NSBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NJBPC_CPU_IGP_2 Register NJBPC_CPU_IGP_2 - CPU Ingress Port new jumbo buffer pointer counter
//! @{

//! Register Offset (relative)
#define NJBPC_CPU_IGP_2 0x12044
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NJBPC_CPU_IGP_2 0x1E792044u

//! Register Reset Value
#define NJBPC_CPU_IGP_2_RST 0x00000000u

//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_2_NJBPC_POS 0
//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_2_NJBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_2 Register DCNTR_IGP_2 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_2 0x1204C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_2 0x1E79204Cu

//! Register Reset Value
#define DCNTR_IGP_2_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_2_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_2_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_2_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_2_DCNT_MASK 0x3F00u

//! @}

//! \defgroup NEW_SPTR_CPU_IGP_2 Register NEW_SPTR_CPU_IGP_2 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR_CPU_IGP_2 0x12080
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR_CPU_IGP_2 0x1E792080u

//! Register Reset Value
#define NEW_SPTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_JPTR_CPU_IGP_2 Register NEW_JPTR_CPU_IGP_2 - CPU Ingress Port New Jumbo Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_JPTR_CPU_IGP_2 0x12090
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_JPTR_CPU_IGP_2 0x1E792090u

//! Register Reset Value
#define NEW_JPTR_CPU_IGP_2_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_2_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_2_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_2_SA_POS 11
//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_2_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_IGP_2 Register DESC0_0_CPU_IGP_2 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_2 0x12100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_0_CPU_IGP_2 0x1E792100u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_0_CPU_IGP_2 0x1E792104u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_0_CPU_IGP_2 0x1E792108u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_0_CPU_IGP_2 0x1E79210Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_1_CPU_IGP_2 0x1E792110u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_1_CPU_IGP_2 0x1E792114u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_1_CPU_IGP_2 0x1E792118u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_1_CPU_IGP_2 0x1E79211Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_CPU_IGP_3 0x1E793000u

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

//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_3_BUFREQS_POS 1
//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_3_BUFREQS_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQS_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQS_EN 0x1

//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_3_BUFREQJ_POS 2
//! Field BUFREQJ - Enable CPU Buffer Request For Jumbo Size Packet
#define CFG_CPU_IGP_3_BUFREQJ_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BUFREQJ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BUFREQJ_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_3_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_3_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_DISCHALT_EN 0x1

//! Field BAS - Base Address Select
#define CFG_CPU_IGP_3_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_CPU_IGP_3_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_CPU_IGP_3_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_CPU_IGP_3_BAS_BA1 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_3_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_3_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_BRBPEN_EN 0x1

//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_3_EQDBPEN_POS 6
//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_3_EQDBPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_EQDBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_EQDBPEN_EN 0x1

//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_3_RCNTBPEN_POS 7
//! Field RCNTBPEN - Repeat Count Back Pressure Enable
#define CFG_CPU_IGP_3_RCNTBPEN_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_RCNTBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_RCNTBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_3_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_3_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_3_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_3_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_3_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_3_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_3_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_3_BP_EN_EN 0x1

//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_3_RCNTEN_POS 11
//! Field RCNTEN - Enable CPU Repeat Count Request
#define CFG_CPU_IGP_3_RCNTEN_MASK 0x800u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_3_RCNTEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_3_RCNTEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_3_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_3_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_CPU_IGP_3 Register WM_CPU_IGP_3 - CPU Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_CPU_IGP_3 0x13004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_CPU_IGP_3 0x1E793004u

//! Register Reset Value
#define WM_CPU_IGP_3_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_3_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_3_LWM_MASK 0x3FFFu

//! Field HWM - High watermark threshold
#define WM_CPU_IGP_3_HWM_POS 16
//! Field HWM - High watermark threshold
#define WM_CPU_IGP_3_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_CPU_IGP_3 Register POCC_CPU_IGP_3 - CPU Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_CPU_IGP_3 0x13008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_CPU_IGP_3 0x1E793008u

//! Register Reset Value
#define POCC_CPU_IGP_3_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_3_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_3_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_CPU_IGP_3 Register EQPC_CPU_IGP_3 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_3 0x1300C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_CPU_IGP_3 0x1E79300Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_CPU_IGP_3 0x1E793010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_CPU_IGP_3 0x1E793014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_CPU_IGP_3 0x1E793018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_CPU_IGP_3 0x1E79301Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_CPU_IGP_3 0x1E793020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_3_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_3_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_3_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_3_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_3_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_3_HWMI_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_3_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_DR_INTOCC 0x1

//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_3_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_3_SPR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_SPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_SPR_INTOCC 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_3_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNCR_CPU_IGP_3_JPR_MASK 0x20u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_3_JPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_3_JPR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_3 Register IRNICR_CPU_IGP_3 - CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_3 0x13024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_CPU_IGP_3 0x1E793024u

//! Register Reset Value
#define IRNICR_CPU_IGP_3_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_3_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_3_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_3_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_3_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_3_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_3_HWMI_MASK 0x4u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_3_DR_MASK 0x8u

//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_3_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_3_SPR_MASK 0x10u

//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_3_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNICR_CPU_IGP_3_JPR_MASK 0x20u

//! @}

//! \defgroup IRNEN_CPU_IGP_3 Register IRNEN_CPU_IGP_3 - CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_3 0x13028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_CPU_IGP_3 0x1E793028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_3_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_3_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_3_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_3_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_HWMI_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_3_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_3_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_DR_EN 0x1

//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_3_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_3_SPR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_SPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_SPR_EN 0x1

//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_3_JPR_POS 5
//! Field JPR - Jumbo Pointer Ready
#define IRNEN_CPU_IGP_3_JPR_MASK 0x20u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_3_JPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_3_JPR_EN 0x1

//! @}

//! \defgroup RCNT_INC_CPU_IGP_3 Register RCNT_INC_CPU_IGP_3 - CPU Ingress Port Repeat Count Increment
//! @{

//! Register Offset (relative)
#define RCNT_INC_CPU_IGP_3 0x13034
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNT_INC_CPU_IGP_3 0x1E793034u

//! Register Reset Value
#define RCNT_INC_CPU_IGP_3_RST 0xFFFFF800u

//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_3_DP_POS 11
//! Field DP - Data Pointer
#define RCNT_INC_CPU_IGP_3_DP_MASK 0xFFFFF800u

//! @}

//! \defgroup DICC_IGP_3 Register DICC_IGP_3 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_3 0x13038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_3 0x1E793038u

//! Register Reset Value
#define DICC_IGP_3_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_3_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_3_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup RCNTC_CPU_IGP_3 Register RCNTC_CPU_IGP_3 - CPU Ingress Port enqueue repeat count counter
//! @{

//! Register Offset (relative)
#define RCNTC_CPU_IGP_3 0x1303C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_RCNTC_CPU_IGP_3 0x1E79303Cu

//! Register Reset Value
#define RCNTC_CPU_IGP_3_RST 0x00000000u

//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_3_RCNTC_POS 0
//! Field RCNTC - Per CPU Port Repeat Count Counter
#define RCNTC_CPU_IGP_3_RCNTC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NSBPC_CPU_IGP_3 Register NSBPC_CPU_IGP_3 - CPU Ingress Port new standard buffer pointer counter
//! @{

//! Register Offset (relative)
#define NSBPC_CPU_IGP_3 0x13040
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NSBPC_CPU_IGP_3 0x1E793040u

//! Register Reset Value
#define NSBPC_CPU_IGP_3_RST 0x00000000u

//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_3_NSBPC_POS 0
//! Field NSBPC - Per CPU New Standard Buffer Pointer Counter
#define NSBPC_CPU_IGP_3_NSBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup NJBPC_CPU_IGP_3 Register NJBPC_CPU_IGP_3 - CPU Ingress Port new jumbo buffer pointer counter
//! @{

//! Register Offset (relative)
#define NJBPC_CPU_IGP_3 0x13044
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NJBPC_CPU_IGP_3 0x1E793044u

//! Register Reset Value
#define NJBPC_CPU_IGP_3_RST 0x00000000u

//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_3_NJBPC_POS 0
//! Field NJBPC - Per CPU New Jumbo Buffer Pointer Counter
#define NJBPC_CPU_IGP_3_NJBPC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_3 Register DCNTR_IGP_3 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_3 0x1304C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_3 0x1E79304Cu

//! Register Reset Value
#define DCNTR_IGP_3_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_3_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_3_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_3_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_3_DCNT_MASK 0x3F00u

//! @}

//! \defgroup NEW_SPTR_CPU_IGP_3 Register NEW_SPTR_CPU_IGP_3 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR_CPU_IGP_3 0x13080
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR_CPU_IGP_3 0x1E793080u

//! Register Reset Value
#define NEW_SPTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_JPTR_CPU_IGP_3 Register NEW_JPTR_CPU_IGP_3 - CPU Ingress Port New Jumbo Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_JPTR_CPU_IGP_3 0x13090
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_JPTR_CPU_IGP_3 0x1E793090u

//! Register Reset Value
#define NEW_JPTR_CPU_IGP_3_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_3_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_JPTR_CPU_IGP_3_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_3_SA_POS 11
//! Field SA - Segment Address
#define NEW_JPTR_CPU_IGP_3_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_IGP_3 Register DESC0_0_CPU_IGP_3 - CPU Ingress Port Descriptor 0 DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_3 0x13100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_0_CPU_IGP_3 0x1E793100u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_0_CPU_IGP_3 0x1E793104u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_0_CPU_IGP_3 0x1E793108u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_0_CPU_IGP_3 0x1E79310Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_1_CPU_IGP_3 0x1E793110u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_1_CPU_IGP_3 0x1E793114u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_1_CPU_IGP_3 0x1E793118u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_1_CPU_IGP_3 0x1E79311Cu

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

//! \defgroup CFG_CPU_IGP_4 Register CFG_CPU_IGP_4 - Special CPU Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_IGP_4 0x14000
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_CPU_IGP_4 0x1E794000u

//! Register Reset Value
#define CFG_CPU_IGP_4_RST 0x00000000u

//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_4_EQREQ_POS 0
//! Field EQREQ - Enable CPU Enqueue Request
#define CFG_CPU_IGP_4_EQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_EQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_EQREQ_EN 0x1

//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_4_BUFREQS_POS 1
//! Field BUFREQS - Enable CPU Buffer Request For Standard Size Packet
#define CFG_CPU_IGP_4_BUFREQS_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_BUFREQS_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_BUFREQS_EN 0x1

//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_4_DISCHALT_POS 3
//! Field DISCHALT - Enable Discard Halt
#define CFG_CPU_IGP_4_DISCHALT_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_DISCHALT_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_DISCHALT_EN 0x1

//! Field BAS - Base Address Select
#define CFG_CPU_IGP_4_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_CPU_IGP_4_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_CPU_IGP_4_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_CPU_IGP_4_BAS_BA1 0x1

//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_4_BRBPEN_POS 5
//! Field BRBPEN - Buffer Request Back Pressure Enable
#define CFG_CPU_IGP_4_BRBPEN_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_BRBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_BRBPEN_EN 0x1

//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_4_EQDBPEN_POS 6
//! Field EQDBPEN - Enqueue Descriptor Back Pressure Enable
#define CFG_CPU_IGP_4_EQDBPEN_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_EQDBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_EQDBPEN_EN 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_4_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_CPU_IGP_4_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_4_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_CPU_IGP_4_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_CPU_IGP_4_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_IGP_4_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_4_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_CPU_IGP_4_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_CPU_IGP_4_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_CPU_IGP_4_BP_EN_EN 0x1

//! Field DISC - Discard Reason
#define CFG_CPU_IGP_4_DISC_POS 16
//! Field DISC - Discard Reason
#define CFG_CPU_IGP_4_DISC_MASK 0xF0000u
//! Constant ACCEPTED - Accepted
#define CONST_CFG_CPU_IGP_4_DISC_ACCEPTED 0x0
//! Constant REASON1 - Reason 1
#define CONST_CFG_CPU_IGP_4_DISC_REASON1 0x1
//! Constant REASON2 - Reason 2
#define CONST_CFG_CPU_IGP_4_DISC_REASON2 0x2
//! Constant RES - Reserved
#define CONST_CFG_CPU_IGP_4_DISC_RES 0x3
//! Constant REASON4 - Reason 4
#define CONST_CFG_CPU_IGP_4_DISC_REASON4 0x4
//! Constant REASON5 - Reason 5
#define CONST_CFG_CPU_IGP_4_DISC_REASON5 0x5
//! Constant REASON6 - Reason 6
#define CONST_CFG_CPU_IGP_4_DISC_REASON6 0x6
//! Constant REASON7 - Reason 7
#define CONST_CFG_CPU_IGP_4_DISC_REASON7 0x7
//! Constant REASON8 - Reason 8
#define CONST_CFG_CPU_IGP_4_DISC_REASON8 0x8
//! Constant REASON9 - Reason 9
#define CONST_CFG_CPU_IGP_4_DISC_REASON9 0x9
//! Constant REASON10 - Reason 10
#define CONST_CFG_CPU_IGP_4_DISC_REASON10 0xA
//! Constant REASON11 - Reason 11
#define CONST_CFG_CPU_IGP_4_DISC_REASON11 0xB
//! Constant REASON12 - Reason 12
#define CONST_CFG_CPU_IGP_4_DISC_REASON12 0xC
//! Constant REASON13 - Reason 13
#define CONST_CFG_CPU_IGP_4_DISC_REASON13 0xD
//! Constant REASON14 - Reason 14
#define CONST_CFG_CPU_IGP_4_DISC_REASON14 0xE
//! Constant REASON15 - Reason 15
#define CONST_CFG_CPU_IGP_4_DISC_REASON15 0xF

//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_4_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_CPU_IGP_4_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_CPU_IGP_4 Register WM_CPU_IGP_4 - CPU Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_CPU_IGP_4 0x14004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_CPU_IGP_4 0x1E794004u

//! Register Reset Value
#define WM_CPU_IGP_4_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_4_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_CPU_IGP_4_LWM_MASK 0x3FFFu

//! Field HWM - High watermark threshold
#define WM_CPU_IGP_4_HWM_POS 16
//! Field HWM - High watermark threshold
#define WM_CPU_IGP_4_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_CPU_IGP_4 Register POCC_CPU_IGP_4 - CPU Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_CPU_IGP_4 0x14008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_CPU_IGP_4 0x1E794008u

//! Register Reset Value
#define POCC_CPU_IGP_4_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_4_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_CPU_IGP_4_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_CPU_IGP_4 Register EQPC_CPU_IGP_4 - CPU Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_CPU_IGP_4 0x1400C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_CPU_IGP_4 0x1E79400Cu

//! Register Reset Value
#define EQPC_CPU_IGP_4_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_4_PC_POS 0
//! Field PC - Per Port Packet Counter
#define EQPC_CPU_IGP_4_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC0_CPU_IGP_4 Register DISC_DESC0_CPU_IGP_4 - CPU Ingress Port Discard Descriptor 0
//! @{

//! Register Offset (relative)
#define DISC_DESC0_CPU_IGP_4 0x14010
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_CPU_IGP_4 0x1E794010u

//! Register Reset Value
#define DISC_DESC0_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DISC_DESC0_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC1_CPU_IGP_4 Register DISC_DESC1_CPU_IGP_4 - CPU Ingress Port Discard Descriptor 1
//! @{

//! Register Offset (relative)
#define DISC_DESC1_CPU_IGP_4 0x14014
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_CPU_IGP_4 0x1E794014u

//! Register Reset Value
#define DISC_DESC1_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DISC_DESC1_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC2_CPU_IGP_4 Register DISC_DESC2_CPU_IGP_4 - CPU Ingress Port Discard Descriptor 2
//! @{

//! Register Offset (relative)
#define DISC_DESC2_CPU_IGP_4 0x14018
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_CPU_IGP_4 0x1E794018u

//! Register Reset Value
#define DISC_DESC2_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DISC_DESC2_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DISC_DESC3_CPU_IGP_4 Register DISC_DESC3_CPU_IGP_4 - CPU Ingress Port Discard Descriptor 3
//! @{

//! Register Offset (relative)
#define DISC_DESC3_CPU_IGP_4 0x1401C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_CPU_IGP_4 0x1E79401Cu

//! Register Reset Value
#define DISC_DESC3_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DISC_DESC3_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_IGP_4 Register IRNCR_CPU_IGP_4 - Special CPU Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_IGP_4 0x14020
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_CPU_IGP_4 0x1E794020u

//! Register Reset Value
#define IRNCR_CPU_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNCR_CPU_IGP_4_PNA_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_4_PNA_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_4_PNA_INTACK 0x1

//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_4_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_CPU_IGP_4_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_4_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_4_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_4_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_CPU_IGP_4_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_4_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_CPU_IGP_4_HWMI_INTACK 0x1

//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_4_DR_POS 3
//! Field DR - Descriptor ready
#define IRNCR_CPU_IGP_4_DR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_4_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_4_DR_INTOCC 0x1

//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_4_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNCR_CPU_IGP_4_SPR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_IGP_4_SPR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_IGP_4_SPR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_IGP_4 Register IRNICR_CPU_IGP_4 - Special CPU Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_IGP_4 0x14024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_CPU_IGP_4 0x1E794024u

//! Register Reset Value
#define IRNICR_CPU_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_CPU_IGP_4_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_4_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_CPU_IGP_4_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_4_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_CPU_IGP_4_HWMI_MASK 0x4u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_4_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNICR_CPU_IGP_4_DR_MASK 0x8u

//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_4_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNICR_CPU_IGP_4_SPR_MASK 0x10u

//! @}

//! \defgroup IRNEN_CPU_IGP_4 Register IRNEN_CPU_IGP_4 - Special CPU Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_IGP_4 0x14028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_CPU_IGP_4 0x1E794028u

//! Register Reset Value
#define IRNEN_CPU_IGP_4_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_4_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNEN_CPU_IGP_4_PNA_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_4_PNA_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_4_PNA_EN 0x1

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_4_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_CPU_IGP_4_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_4_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_4_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_4_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_CPU_IGP_4_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_4_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_4_HWMI_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_4_DR_POS 3
//! Field DR - Descriptor Ready
#define IRNEN_CPU_IGP_4_DR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_4_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_4_DR_EN 0x1

//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_4_SPR_POS 4
//! Field SPR - Standard Pointer Ready
#define IRNEN_CPU_IGP_4_SPR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_IGP_4_SPR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_IGP_4_SPR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_IGP_4 Register DPTR_CPU_IGP_4 - Special CPU Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_IGP_4 0x14030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_CPU_IGP_4 0x1E794030u

//! Register Reset Value
#define DPTR_CPU_IGP_4_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_CPU_IGP_4_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_IGP_4_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_IGP_4_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_IGP_4_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup NEW_SPTR0_CPU_IGP_4 Register NEW_SPTR0_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR0_CPU_IGP_4 0x14080
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR0_CPU_IGP_4 0x1E794080u

//! Register Reset Value
#define NEW_SPTR0_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR0_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR0_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR0_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR0_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR1_CPU_IGP_4 Register NEW_SPTR1_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR1_CPU_IGP_4 0x14084
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR1_CPU_IGP_4 0x1E794084u

//! Register Reset Value
#define NEW_SPTR1_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR1_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR1_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR1_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR1_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR2_CPU_IGP_4 Register NEW_SPTR2_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR2_CPU_IGP_4 0x14088
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR2_CPU_IGP_4 0x1E794088u

//! Register Reset Value
#define NEW_SPTR2_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR2_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR2_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR2_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR2_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR3_CPU_IGP_4 Register NEW_SPTR3_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR3_CPU_IGP_4 0x1408C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR3_CPU_IGP_4 0x1E79408Cu

//! Register Reset Value
#define NEW_SPTR3_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR3_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR3_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR3_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR3_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR4_CPU_IGP_4 Register NEW_SPTR4_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR4_CPU_IGP_4 0x14090
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR4_CPU_IGP_4 0x1E794090u

//! Register Reset Value
#define NEW_SPTR4_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR4_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR4_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR4_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR4_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR5_CPU_IGP_4 Register NEW_SPTR5_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR5_CPU_IGP_4 0x14094
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR5_CPU_IGP_4 0x1E794094u

//! Register Reset Value
#define NEW_SPTR5_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR5_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR5_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR5_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR5_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR6_CPU_IGP_4 Register NEW_SPTR6_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR6_CPU_IGP_4 0x14098
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR6_CPU_IGP_4 0x1E794098u

//! Register Reset Value
#define NEW_SPTR6_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR6_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR6_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR6_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR6_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR7_CPU_IGP_4 Register NEW_SPTR7_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR7_CPU_IGP_4 0x1409C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR7_CPU_IGP_4 0x1E79409Cu

//! Register Reset Value
#define NEW_SPTR7_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR7_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR7_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR7_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR7_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR8_CPU_IGP_4 Register NEW_SPTR8_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR8_CPU_IGP_4 0x140A0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR8_CPU_IGP_4 0x1E7940A0u

//! Register Reset Value
#define NEW_SPTR8_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR8_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR8_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR8_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR8_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR9_CPU_IGP_4 Register NEW_SPTR9_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR9_CPU_IGP_4 0x140A4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR9_CPU_IGP_4 0x1E7940A4u

//! Register Reset Value
#define NEW_SPTR9_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR9_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR9_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR9_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR9_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR10_CPU_IGP_4 Register NEW_SPTR10_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR10_CPU_IGP_4 0x140A8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR10_CPU_IGP_4 0x1E7940A8u

//! Register Reset Value
#define NEW_SPTR10_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR10_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR10_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR10_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR10_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR11_CPU_IGP_4 Register NEW_SPTR11_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR11_CPU_IGP_4 0x140AC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR11_CPU_IGP_4 0x1E7940ACu

//! Register Reset Value
#define NEW_SPTR11_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR11_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR11_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR11_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR11_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR12_CPU_IGP_4 Register NEW_SPTR12_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR12_CPU_IGP_4 0x140B0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR12_CPU_IGP_4 0x1E7940B0u

//! Register Reset Value
#define NEW_SPTR12_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR12_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR12_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR12_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR12_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR13_CPU_IGP_4 Register NEW_SPTR13_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR13_CPU_IGP_4 0x140B4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR13_CPU_IGP_4 0x1E7940B4u

//! Register Reset Value
#define NEW_SPTR13_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR13_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR13_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR13_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR13_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR14_CPU_IGP_4 Register NEW_SPTR14_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR14_CPU_IGP_4 0x140B8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR14_CPU_IGP_4 0x1E7940B8u

//! Register Reset Value
#define NEW_SPTR14_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR14_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR14_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR14_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR14_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR15_CPU_IGP_4 Register NEW_SPTR15_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR15_CPU_IGP_4 0x140BC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR15_CPU_IGP_4 0x1E7940BCu

//! Register Reset Value
#define NEW_SPTR15_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR15_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR15_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR15_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR15_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR16_CPU_IGP_4 Register NEW_SPTR16_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR16_CPU_IGP_4 0x140C0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR16_CPU_IGP_4 0x1E7940C0u

//! Register Reset Value
#define NEW_SPTR16_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR16_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR16_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR16_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR16_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR17_CPU_IGP_4 Register NEW_SPTR17_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR17_CPU_IGP_4 0x140C4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR17_CPU_IGP_4 0x1E7940C4u

//! Register Reset Value
#define NEW_SPTR17_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR17_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR17_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR17_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR17_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR18_CPU_IGP_4 Register NEW_SPTR18_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR18_CPU_IGP_4 0x140C8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR18_CPU_IGP_4 0x1E7940C8u

//! Register Reset Value
#define NEW_SPTR18_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR18_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR18_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR18_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR18_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR19_CPU_IGP_4 Register NEW_SPTR19_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR19_CPU_IGP_4 0x140CC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR19_CPU_IGP_4 0x1E7940CCu

//! Register Reset Value
#define NEW_SPTR19_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR19_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR19_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR19_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR19_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR20_CPU_IGP_4 Register NEW_SPTR20_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR20_CPU_IGP_4 0x140D0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR20_CPU_IGP_4 0x1E7940D0u

//! Register Reset Value
#define NEW_SPTR20_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR20_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR20_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR20_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR20_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR21_CPU_IGP_4 Register NEW_SPTR21_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR21_CPU_IGP_4 0x140D4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR21_CPU_IGP_4 0x1E7940D4u

//! Register Reset Value
#define NEW_SPTR21_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR21_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR21_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR21_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR21_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR22_CPU_IGP_4 Register NEW_SPTR22_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR22_CPU_IGP_4 0x140D8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR22_CPU_IGP_4 0x1E7940D8u

//! Register Reset Value
#define NEW_SPTR22_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR22_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR22_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR22_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR22_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR23_CPU_IGP_4 Register NEW_SPTR23_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR23_CPU_IGP_4 0x140DC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR23_CPU_IGP_4 0x1E7940DCu

//! Register Reset Value
#define NEW_SPTR23_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR23_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR23_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR23_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR23_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR24_CPU_IGP_4 Register NEW_SPTR24_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR24_CPU_IGP_4 0x140E0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR24_CPU_IGP_4 0x1E7940E0u

//! Register Reset Value
#define NEW_SPTR24_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR24_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR24_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR24_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR24_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR25_CPU_IGP_4 Register NEW_SPTR25_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR25_CPU_IGP_4 0x140E4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR25_CPU_IGP_4 0x1E7940E4u

//! Register Reset Value
#define NEW_SPTR25_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR25_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR25_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR25_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR25_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR26_CPU_IGP_4 Register NEW_SPTR26_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR26_CPU_IGP_4 0x140E8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR26_CPU_IGP_4 0x1E7940E8u

//! Register Reset Value
#define NEW_SPTR26_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR26_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR26_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR26_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR26_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR27_CPU_IGP_4 Register NEW_SPTR27_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR27_CPU_IGP_4 0x140EC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR27_CPU_IGP_4 0x1E7940ECu

//! Register Reset Value
#define NEW_SPTR27_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR27_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR27_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR27_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR27_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR28_CPU_IGP_4 Register NEW_SPTR28_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR28_CPU_IGP_4 0x140F0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR28_CPU_IGP_4 0x1E7940F0u

//! Register Reset Value
#define NEW_SPTR28_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR28_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR28_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR28_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR28_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR29_CPU_IGP_4 Register NEW_SPTR29_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR29_CPU_IGP_4 0x140F4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR29_CPU_IGP_4 0x1E7940F4u

//! Register Reset Value
#define NEW_SPTR29_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR29_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR29_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR29_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR29_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR30_CPU_IGP_4 Register NEW_SPTR30_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR30_CPU_IGP_4 0x140F8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR30_CPU_IGP_4 0x1E7940F8u

//! Register Reset Value
#define NEW_SPTR30_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR30_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR30_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR30_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR30_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup NEW_SPTR31_CPU_IGP_4 Register NEW_SPTR31_CPU_IGP_4 - CPU Ingress Port New Standard Buffer Pointer
//! @{

//! Register Offset (relative)
#define NEW_SPTR31_CPU_IGP_4 0x140FC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_NEW_SPTR31_CPU_IGP_4 0x1E7940FCu

//! Register Reset Value
#define NEW_SPTR31_CPU_IGP_4_RST 0xFFFFF800u

//! Field OFFSET - Offset
#define NEW_SPTR31_CPU_IGP_4_OFFSET_POS 0
//! Field OFFSET - Offset
#define NEW_SPTR31_CPU_IGP_4_OFFSET_MASK 0x7FFu

//! Field SA - Segment Address
#define NEW_SPTR31_CPU_IGP_4_SA_POS 11
//! Field SA - Segment Address
#define NEW_SPTR31_CPU_IGP_4_SA_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_IGP_4 Register DESC0_0_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_IGP_4 0x14100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_0_CPU_IGP_4 0x1E794100u

//! Register Reset Value
#define DESC0_0_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_IGP_4 Register DESC1_0_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_IGP_4 0x14104
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_0_CPU_IGP_4 0x1E794104u

//! Register Reset Value
#define DESC1_0_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_IGP_4 Register DESC2_0_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_IGP_4 0x14108
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_0_CPU_IGP_4 0x1E794108u

//! Register Reset Value
#define DESC2_0_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_IGP_4 Register DESC3_0_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_IGP_4 0x1410C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_0_CPU_IGP_4 0x1E79410Cu

//! Register Reset Value
#define DESC3_0_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_IGP_4 Register DESC0_1_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_IGP_4 0x14110
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_1_CPU_IGP_4 0x1E794110u

//! Register Reset Value
#define DESC0_1_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_IGP_4 Register DESC1_1_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_IGP_4 0x14114
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_1_CPU_IGP_4 0x1E794114u

//! Register Reset Value
#define DESC1_1_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_IGP_4 Register DESC2_1_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_IGP_4 0x14118
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_1_CPU_IGP_4 0x1E794118u

//! Register Reset Value
#define DESC2_1_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_IGP_4 Register DESC3_1_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_IGP_4 0x1411C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_1_CPU_IGP_4 0x1E79411Cu

//! Register Reset Value
#define DESC3_1_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_2_CPU_IGP_4 Register DESC0_2_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_2_CPU_IGP_4 0x14120
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_2_CPU_IGP_4 0x1E794120u

//! Register Reset Value
#define DESC0_2_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_2_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_2_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_2_CPU_IGP_4 Register DESC1_2_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_2_CPU_IGP_4 0x14124
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_2_CPU_IGP_4 0x1E794124u

//! Register Reset Value
#define DESC1_2_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_2_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_2_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_2_CPU_IGP_4 Register DESC2_2_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_2_CPU_IGP_4 0x14128
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_2_CPU_IGP_4 0x1E794128u

//! Register Reset Value
#define DESC2_2_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_2_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_2_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_2_CPU_IGP_4 Register DESC3_2_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_2_CPU_IGP_4 0x1412C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_2_CPU_IGP_4 0x1E79412Cu

//! Register Reset Value
#define DESC3_2_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_2_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_2_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_3_CPU_IGP_4 Register DESC0_3_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_3_CPU_IGP_4 0x14130
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_3_CPU_IGP_4 0x1E794130u

//! Register Reset Value
#define DESC0_3_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_3_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_3_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_3_CPU_IGP_4 Register DESC1_3_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_3_CPU_IGP_4 0x14134
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_3_CPU_IGP_4 0x1E794134u

//! Register Reset Value
#define DESC1_3_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_3_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_3_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_3_CPU_IGP_4 Register DESC2_3_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_3_CPU_IGP_4 0x14138
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_3_CPU_IGP_4 0x1E794138u

//! Register Reset Value
#define DESC2_3_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_3_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_3_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_3_CPU_IGP_4 Register DESC3_3_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_3_CPU_IGP_4 0x1413C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_3_CPU_IGP_4 0x1E79413Cu

//! Register Reset Value
#define DESC3_3_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_3_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_3_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_4_CPU_IGP_4 Register DESC0_4_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_4_CPU_IGP_4 0x14140
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_4_CPU_IGP_4 0x1E794140u

//! Register Reset Value
#define DESC0_4_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_4_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_4_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_4_CPU_IGP_4 Register DESC1_4_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_4_CPU_IGP_4 0x14144
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_4_CPU_IGP_4 0x1E794144u

//! Register Reset Value
#define DESC1_4_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_4_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_4_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_4_CPU_IGP_4 Register DESC2_4_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_4_CPU_IGP_4 0x14148
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_4_CPU_IGP_4 0x1E794148u

//! Register Reset Value
#define DESC2_4_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_4_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_4_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_4_CPU_IGP_4 Register DESC3_4_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_4_CPU_IGP_4 0x1414C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_4_CPU_IGP_4 0x1E79414Cu

//! Register Reset Value
#define DESC3_4_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_4_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_4_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_5_CPU_IGP_4 Register DESC0_5_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_5_CPU_IGP_4 0x14150
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_5_CPU_IGP_4 0x1E794150u

//! Register Reset Value
#define DESC0_5_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_5_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_5_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_5_CPU_IGP_4 Register DESC1_5_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_5_CPU_IGP_4 0x14154
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_5_CPU_IGP_4 0x1E794154u

//! Register Reset Value
#define DESC1_5_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_5_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_5_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_5_CPU_IGP_4 Register DESC2_5_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_5_CPU_IGP_4 0x14158
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_5_CPU_IGP_4 0x1E794158u

//! Register Reset Value
#define DESC2_5_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_5_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_5_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_5_CPU_IGP_4 Register DESC3_5_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_5_CPU_IGP_4 0x1415C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_5_CPU_IGP_4 0x1E79415Cu

//! Register Reset Value
#define DESC3_5_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_5_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_5_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_6_CPU_IGP_4 Register DESC0_6_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_6_CPU_IGP_4 0x14160
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_6_CPU_IGP_4 0x1E794160u

//! Register Reset Value
#define DESC0_6_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_6_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_6_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_6_CPU_IGP_4 Register DESC1_6_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_6_CPU_IGP_4 0x14164
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_6_CPU_IGP_4 0x1E794164u

//! Register Reset Value
#define DESC1_6_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_6_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_6_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_6_CPU_IGP_4 Register DESC2_6_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_6_CPU_IGP_4 0x14168
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_6_CPU_IGP_4 0x1E794168u

//! Register Reset Value
#define DESC2_6_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_6_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_6_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_6_CPU_IGP_4 Register DESC3_6_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_6_CPU_IGP_4 0x1416C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_6_CPU_IGP_4 0x1E79416Cu

//! Register Reset Value
#define DESC3_6_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_6_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_6_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_7_CPU_IGP_4 Register DESC0_7_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_7_CPU_IGP_4 0x14170
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_7_CPU_IGP_4 0x1E794170u

//! Register Reset Value
#define DESC0_7_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_7_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_7_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_7_CPU_IGP_4 Register DESC1_7_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_7_CPU_IGP_4 0x14174
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_7_CPU_IGP_4 0x1E794174u

//! Register Reset Value
#define DESC1_7_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_7_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_7_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_7_CPU_IGP_4 Register DESC2_7_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_7_CPU_IGP_4 0x14178
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_7_CPU_IGP_4 0x1E794178u

//! Register Reset Value
#define DESC2_7_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_7_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_7_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_7_CPU_IGP_4 Register DESC3_7_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_7_CPU_IGP_4 0x1417C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_7_CPU_IGP_4 0x1E79417Cu

//! Register Reset Value
#define DESC3_7_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_7_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_7_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_8_CPU_IGP_4 Register DESC0_8_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_8_CPU_IGP_4 0x14180
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_8_CPU_IGP_4 0x1E794180u

//! Register Reset Value
#define DESC0_8_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_8_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_8_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_8_CPU_IGP_4 Register DESC1_8_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_8_CPU_IGP_4 0x14184
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_8_CPU_IGP_4 0x1E794184u

//! Register Reset Value
#define DESC1_8_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_8_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_8_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_8_CPU_IGP_4 Register DESC2_8_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_8_CPU_IGP_4 0x14188
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_8_CPU_IGP_4 0x1E794188u

//! Register Reset Value
#define DESC2_8_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_8_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_8_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_8_CPU_IGP_4 Register DESC3_8_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_8_CPU_IGP_4 0x1418C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_8_CPU_IGP_4 0x1E79418Cu

//! Register Reset Value
#define DESC3_8_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_8_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_8_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_9_CPU_IGP_4 Register DESC0_9_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_9_CPU_IGP_4 0x14190
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_9_CPU_IGP_4 0x1E794190u

//! Register Reset Value
#define DESC0_9_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_9_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_9_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_9_CPU_IGP_4 Register DESC1_9_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_9_CPU_IGP_4 0x14194
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_9_CPU_IGP_4 0x1E794194u

//! Register Reset Value
#define DESC1_9_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_9_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_9_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_9_CPU_IGP_4 Register DESC2_9_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_9_CPU_IGP_4 0x14198
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_9_CPU_IGP_4 0x1E794198u

//! Register Reset Value
#define DESC2_9_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_9_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_9_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_9_CPU_IGP_4 Register DESC3_9_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_9_CPU_IGP_4 0x1419C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_9_CPU_IGP_4 0x1E79419Cu

//! Register Reset Value
#define DESC3_9_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_9_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_9_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_10_CPU_IGP_4 Register DESC0_10_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_10_CPU_IGP_4 0x141A0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_10_CPU_IGP_4 0x1E7941A0u

//! Register Reset Value
#define DESC0_10_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_10_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_10_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_10_CPU_IGP_4 Register DESC1_10_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_10_CPU_IGP_4 0x141A4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_10_CPU_IGP_4 0x1E7941A4u

//! Register Reset Value
#define DESC1_10_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_10_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_10_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_10_CPU_IGP_4 Register DESC2_10_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_10_CPU_IGP_4 0x141A8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_10_CPU_IGP_4 0x1E7941A8u

//! Register Reset Value
#define DESC2_10_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_10_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_10_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_10_CPU_IGP_4 Register DESC3_10_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_10_CPU_IGP_4 0x141AC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_10_CPU_IGP_4 0x1E7941ACu

//! Register Reset Value
#define DESC3_10_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_10_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_10_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_11_CPU_IGP_4 Register DESC0_11_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_11_CPU_IGP_4 0x141B0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_11_CPU_IGP_4 0x1E7941B0u

//! Register Reset Value
#define DESC0_11_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_11_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_11_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_11_CPU_IGP_4 Register DESC1_11_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_11_CPU_IGP_4 0x141B4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_11_CPU_IGP_4 0x1E7941B4u

//! Register Reset Value
#define DESC1_11_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_11_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_11_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_11_CPU_IGP_4 Register DESC2_11_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_11_CPU_IGP_4 0x141B8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_11_CPU_IGP_4 0x1E7941B8u

//! Register Reset Value
#define DESC2_11_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_11_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_11_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_11_CPU_IGP_4 Register DESC3_11_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_11_CPU_IGP_4 0x141BC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_11_CPU_IGP_4 0x1E7941BCu

//! Register Reset Value
#define DESC3_11_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_11_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_11_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_12_CPU_IGP_4 Register DESC0_12_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_12_CPU_IGP_4 0x141C0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_12_CPU_IGP_4 0x1E7941C0u

//! Register Reset Value
#define DESC0_12_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_12_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_12_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_12_CPU_IGP_4 Register DESC1_12_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_12_CPU_IGP_4 0x141C4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_12_CPU_IGP_4 0x1E7941C4u

//! Register Reset Value
#define DESC1_12_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_12_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_12_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_12_CPU_IGP_4 Register DESC2_12_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_12_CPU_IGP_4 0x141C8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_12_CPU_IGP_4 0x1E7941C8u

//! Register Reset Value
#define DESC2_12_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_12_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_12_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_12_CPU_IGP_4 Register DESC3_12_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_12_CPU_IGP_4 0x141CC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_12_CPU_IGP_4 0x1E7941CCu

//! Register Reset Value
#define DESC3_12_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_12_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_12_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_13_CPU_IGP_4 Register DESC0_13_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_13_CPU_IGP_4 0x141D0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_13_CPU_IGP_4 0x1E7941D0u

//! Register Reset Value
#define DESC0_13_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_13_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_13_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_13_CPU_IGP_4 Register DESC1_13_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_13_CPU_IGP_4 0x141D4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_13_CPU_IGP_4 0x1E7941D4u

//! Register Reset Value
#define DESC1_13_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_13_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_13_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_13_CPU_IGP_4 Register DESC2_13_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_13_CPU_IGP_4 0x141D8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_13_CPU_IGP_4 0x1E7941D8u

//! Register Reset Value
#define DESC2_13_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_13_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_13_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_13_CPU_IGP_4 Register DESC3_13_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_13_CPU_IGP_4 0x141DC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_13_CPU_IGP_4 0x1E7941DCu

//! Register Reset Value
#define DESC3_13_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_13_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_13_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_14_CPU_IGP_4 Register DESC0_14_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_14_CPU_IGP_4 0x141E0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_14_CPU_IGP_4 0x1E7941E0u

//! Register Reset Value
#define DESC0_14_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_14_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_14_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_14_CPU_IGP_4 Register DESC1_14_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_14_CPU_IGP_4 0x141E4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_14_CPU_IGP_4 0x1E7941E4u

//! Register Reset Value
#define DESC1_14_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_14_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_14_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_14_CPU_IGP_4 Register DESC2_14_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_14_CPU_IGP_4 0x141E8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_14_CPU_IGP_4 0x1E7941E8u

//! Register Reset Value
#define DESC2_14_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_14_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_14_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_14_CPU_IGP_4 Register DESC3_14_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_14_CPU_IGP_4 0x141EC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_14_CPU_IGP_4 0x1E7941ECu

//! Register Reset Value
#define DESC3_14_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_14_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_14_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_15_CPU_IGP_4 Register DESC0_15_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_15_CPU_IGP_4 0x141F0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_15_CPU_IGP_4 0x1E7941F0u

//! Register Reset Value
#define DESC0_15_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_15_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_15_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_15_CPU_IGP_4 Register DESC1_15_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_15_CPU_IGP_4 0x141F4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_15_CPU_IGP_4 0x1E7941F4u

//! Register Reset Value
#define DESC1_15_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_15_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_15_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_15_CPU_IGP_4 Register DESC2_15_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_15_CPU_IGP_4 0x141F8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_15_CPU_IGP_4 0x1E7941F8u

//! Register Reset Value
#define DESC2_15_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_15_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_15_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_15_CPU_IGP_4 Register DESC3_15_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_15_CPU_IGP_4 0x141FC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_15_CPU_IGP_4 0x1E7941FCu

//! Register Reset Value
#define DESC3_15_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_15_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_15_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_16_CPU_IGP_4 Register DESC0_16_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_16_CPU_IGP_4 0x14200
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_16_CPU_IGP_4 0x1E794200u

//! Register Reset Value
#define DESC0_16_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_16_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_16_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_16_CPU_IGP_4 Register DESC1_16_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_16_CPU_IGP_4 0x14204
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_16_CPU_IGP_4 0x1E794204u

//! Register Reset Value
#define DESC1_16_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_16_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_16_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_16_CPU_IGP_4 Register DESC2_16_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_16_CPU_IGP_4 0x14208
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_16_CPU_IGP_4 0x1E794208u

//! Register Reset Value
#define DESC2_16_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_16_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_16_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_16_CPU_IGP_4 Register DESC3_16_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_16_CPU_IGP_4 0x1420C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_16_CPU_IGP_4 0x1E79420Cu

//! Register Reset Value
#define DESC3_16_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_16_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_16_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_17_CPU_IGP_4 Register DESC0_17_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_17_CPU_IGP_4 0x14210
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_17_CPU_IGP_4 0x1E794210u

//! Register Reset Value
#define DESC0_17_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_17_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_17_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_17_CPU_IGP_4 Register DESC1_17_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_17_CPU_IGP_4 0x14214
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_17_CPU_IGP_4 0x1E794214u

//! Register Reset Value
#define DESC1_17_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_17_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_17_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_17_CPU_IGP_4 Register DESC2_17_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_17_CPU_IGP_4 0x14218
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_17_CPU_IGP_4 0x1E794218u

//! Register Reset Value
#define DESC2_17_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_17_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_17_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_17_CPU_IGP_4 Register DESC3_17_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_17_CPU_IGP_4 0x1421C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_17_CPU_IGP_4 0x1E79421Cu

//! Register Reset Value
#define DESC3_17_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_17_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_17_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_18_CPU_IGP_4 Register DESC0_18_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_18_CPU_IGP_4 0x14220
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_18_CPU_IGP_4 0x1E794220u

//! Register Reset Value
#define DESC0_18_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_18_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_18_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_18_CPU_IGP_4 Register DESC1_18_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_18_CPU_IGP_4 0x14224
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_18_CPU_IGP_4 0x1E794224u

//! Register Reset Value
#define DESC1_18_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_18_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_18_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_18_CPU_IGP_4 Register DESC2_18_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_18_CPU_IGP_4 0x14228
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_18_CPU_IGP_4 0x1E794228u

//! Register Reset Value
#define DESC2_18_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_18_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_18_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_18_CPU_IGP_4 Register DESC3_18_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_18_CPU_IGP_4 0x1422C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_18_CPU_IGP_4 0x1E79422Cu

//! Register Reset Value
#define DESC3_18_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_18_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_18_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_19_CPU_IGP_4 Register DESC0_19_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_19_CPU_IGP_4 0x14230
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_19_CPU_IGP_4 0x1E794230u

//! Register Reset Value
#define DESC0_19_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_19_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_19_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_19_CPU_IGP_4 Register DESC1_19_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_19_CPU_IGP_4 0x14234
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_19_CPU_IGP_4 0x1E794234u

//! Register Reset Value
#define DESC1_19_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_19_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_19_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_19_CPU_IGP_4 Register DESC2_19_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_19_CPU_IGP_4 0x14238
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_19_CPU_IGP_4 0x1E794238u

//! Register Reset Value
#define DESC2_19_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_19_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_19_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_19_CPU_IGP_4 Register DESC3_19_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_19_CPU_IGP_4 0x1423C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_19_CPU_IGP_4 0x1E79423Cu

//! Register Reset Value
#define DESC3_19_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_19_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_19_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_20_CPU_IGP_4 Register DESC0_20_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_20_CPU_IGP_4 0x14240
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_20_CPU_IGP_4 0x1E794240u

//! Register Reset Value
#define DESC0_20_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_20_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_20_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_20_CPU_IGP_4 Register DESC1_20_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_20_CPU_IGP_4 0x14244
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_20_CPU_IGP_4 0x1E794244u

//! Register Reset Value
#define DESC1_20_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_20_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_20_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_20_CPU_IGP_4 Register DESC2_20_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_20_CPU_IGP_4 0x14248
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_20_CPU_IGP_4 0x1E794248u

//! Register Reset Value
#define DESC2_20_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_20_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_20_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_20_CPU_IGP_4 Register DESC3_20_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_20_CPU_IGP_4 0x1424C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_20_CPU_IGP_4 0x1E79424Cu

//! Register Reset Value
#define DESC3_20_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_20_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_20_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_21_CPU_IGP_4 Register DESC0_21_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_21_CPU_IGP_4 0x14250
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_21_CPU_IGP_4 0x1E794250u

//! Register Reset Value
#define DESC0_21_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_21_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_21_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_21_CPU_IGP_4 Register DESC1_21_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_21_CPU_IGP_4 0x14254
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_21_CPU_IGP_4 0x1E794254u

//! Register Reset Value
#define DESC1_21_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_21_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_21_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_21_CPU_IGP_4 Register DESC2_21_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_21_CPU_IGP_4 0x14258
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_21_CPU_IGP_4 0x1E794258u

//! Register Reset Value
#define DESC2_21_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_21_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_21_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_21_CPU_IGP_4 Register DESC3_21_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_21_CPU_IGP_4 0x1425C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_21_CPU_IGP_4 0x1E79425Cu

//! Register Reset Value
#define DESC3_21_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_21_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_21_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_22_CPU_IGP_4 Register DESC0_22_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_22_CPU_IGP_4 0x14260
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_22_CPU_IGP_4 0x1E794260u

//! Register Reset Value
#define DESC0_22_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_22_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_22_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_22_CPU_IGP_4 Register DESC1_22_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_22_CPU_IGP_4 0x14264
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_22_CPU_IGP_4 0x1E794264u

//! Register Reset Value
#define DESC1_22_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_22_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_22_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_22_CPU_IGP_4 Register DESC2_22_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_22_CPU_IGP_4 0x14268
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_22_CPU_IGP_4 0x1E794268u

//! Register Reset Value
#define DESC2_22_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_22_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_22_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_22_CPU_IGP_4 Register DESC3_22_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_22_CPU_IGP_4 0x1426C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_22_CPU_IGP_4 0x1E79426Cu

//! Register Reset Value
#define DESC3_22_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_22_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_22_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_23_CPU_IGP_4 Register DESC0_23_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_23_CPU_IGP_4 0x14270
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_23_CPU_IGP_4 0x1E794270u

//! Register Reset Value
#define DESC0_23_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_23_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_23_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_23_CPU_IGP_4 Register DESC1_23_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_23_CPU_IGP_4 0x14274
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_23_CPU_IGP_4 0x1E794274u

//! Register Reset Value
#define DESC1_23_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_23_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_23_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_23_CPU_IGP_4 Register DESC2_23_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_23_CPU_IGP_4 0x14278
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_23_CPU_IGP_4 0x1E794278u

//! Register Reset Value
#define DESC2_23_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_23_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_23_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_23_CPU_IGP_4 Register DESC3_23_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_23_CPU_IGP_4 0x1427C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_23_CPU_IGP_4 0x1E79427Cu

//! Register Reset Value
#define DESC3_23_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_23_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_23_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_24_CPU_IGP_4 Register DESC0_24_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_24_CPU_IGP_4 0x14280
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_24_CPU_IGP_4 0x1E794280u

//! Register Reset Value
#define DESC0_24_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_24_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_24_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_24_CPU_IGP_4 Register DESC1_24_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_24_CPU_IGP_4 0x14284
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_24_CPU_IGP_4 0x1E794284u

//! Register Reset Value
#define DESC1_24_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_24_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_24_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_24_CPU_IGP_4 Register DESC2_24_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_24_CPU_IGP_4 0x14288
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_24_CPU_IGP_4 0x1E794288u

//! Register Reset Value
#define DESC2_24_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_24_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_24_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_24_CPU_IGP_4 Register DESC3_24_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_24_CPU_IGP_4 0x1428C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_24_CPU_IGP_4 0x1E79428Cu

//! Register Reset Value
#define DESC3_24_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_24_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_24_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_25_CPU_IGP_4 Register DESC0_25_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_25_CPU_IGP_4 0x14290
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_25_CPU_IGP_4 0x1E794290u

//! Register Reset Value
#define DESC0_25_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_25_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_25_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_25_CPU_IGP_4 Register DESC1_25_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_25_CPU_IGP_4 0x14294
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_25_CPU_IGP_4 0x1E794294u

//! Register Reset Value
#define DESC1_25_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_25_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_25_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_25_CPU_IGP_4 Register DESC2_25_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_25_CPU_IGP_4 0x14298
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_25_CPU_IGP_4 0x1E794298u

//! Register Reset Value
#define DESC2_25_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_25_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_25_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_25_CPU_IGP_4 Register DESC3_25_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_25_CPU_IGP_4 0x1429C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_25_CPU_IGP_4 0x1E79429Cu

//! Register Reset Value
#define DESC3_25_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_25_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_25_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_26_CPU_IGP_4 Register DESC0_26_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_26_CPU_IGP_4 0x142A0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_26_CPU_IGP_4 0x1E7942A0u

//! Register Reset Value
#define DESC0_26_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_26_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_26_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_26_CPU_IGP_4 Register DESC1_26_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_26_CPU_IGP_4 0x142A4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_26_CPU_IGP_4 0x1E7942A4u

//! Register Reset Value
#define DESC1_26_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_26_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_26_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_26_CPU_IGP_4 Register DESC2_26_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_26_CPU_IGP_4 0x142A8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_26_CPU_IGP_4 0x1E7942A8u

//! Register Reset Value
#define DESC2_26_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_26_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_26_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_26_CPU_IGP_4 Register DESC3_26_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_26_CPU_IGP_4 0x142AC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_26_CPU_IGP_4 0x1E7942ACu

//! Register Reset Value
#define DESC3_26_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_26_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_26_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_27_CPU_IGP_4 Register DESC0_27_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_27_CPU_IGP_4 0x142B0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_27_CPU_IGP_4 0x1E7942B0u

//! Register Reset Value
#define DESC0_27_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_27_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_27_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_27_CPU_IGP_4 Register DESC1_27_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_27_CPU_IGP_4 0x142B4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_27_CPU_IGP_4 0x1E7942B4u

//! Register Reset Value
#define DESC1_27_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_27_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_27_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_27_CPU_IGP_4 Register DESC2_27_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_27_CPU_IGP_4 0x142B8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_27_CPU_IGP_4 0x1E7942B8u

//! Register Reset Value
#define DESC2_27_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_27_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_27_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_27_CPU_IGP_4 Register DESC3_27_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_27_CPU_IGP_4 0x142BC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_27_CPU_IGP_4 0x1E7942BCu

//! Register Reset Value
#define DESC3_27_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_27_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_27_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_28_CPU_IGP_4 Register DESC0_28_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_28_CPU_IGP_4 0x142C0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_28_CPU_IGP_4 0x1E7942C0u

//! Register Reset Value
#define DESC0_28_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_28_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_28_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_28_CPU_IGP_4 Register DESC1_28_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_28_CPU_IGP_4 0x142C4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_28_CPU_IGP_4 0x1E7942C4u

//! Register Reset Value
#define DESC1_28_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_28_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_28_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_28_CPU_IGP_4 Register DESC2_28_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_28_CPU_IGP_4 0x142C8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_28_CPU_IGP_4 0x1E7942C8u

//! Register Reset Value
#define DESC2_28_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_28_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_28_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_28_CPU_IGP_4 Register DESC3_28_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_28_CPU_IGP_4 0x142CC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_28_CPU_IGP_4 0x1E7942CCu

//! Register Reset Value
#define DESC3_28_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_28_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_28_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_29_CPU_IGP_4 Register DESC0_29_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_29_CPU_IGP_4 0x142D0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_29_CPU_IGP_4 0x1E7942D0u

//! Register Reset Value
#define DESC0_29_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_29_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_29_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_29_CPU_IGP_4 Register DESC1_29_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_29_CPU_IGP_4 0x142D4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_29_CPU_IGP_4 0x1E7942D4u

//! Register Reset Value
#define DESC1_29_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_29_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_29_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_29_CPU_IGP_4 Register DESC2_29_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_29_CPU_IGP_4 0x142D8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_29_CPU_IGP_4 0x1E7942D8u

//! Register Reset Value
#define DESC2_29_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_29_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_29_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_29_CPU_IGP_4 Register DESC3_29_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_29_CPU_IGP_4 0x142DC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_29_CPU_IGP_4 0x1E7942DCu

//! Register Reset Value
#define DESC3_29_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_29_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_29_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_30_CPU_IGP_4 Register DESC0_30_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_30_CPU_IGP_4 0x142E0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_30_CPU_IGP_4 0x1E7942E0u

//! Register Reset Value
#define DESC0_30_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_30_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_30_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_30_CPU_IGP_4 Register DESC1_30_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_30_CPU_IGP_4 0x142E4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_30_CPU_IGP_4 0x1E7942E4u

//! Register Reset Value
#define DESC1_30_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_30_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_30_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_30_CPU_IGP_4 Register DESC2_30_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_30_CPU_IGP_4 0x142E8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_30_CPU_IGP_4 0x1E7942E8u

//! Register Reset Value
#define DESC2_30_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_30_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_30_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_30_CPU_IGP_4 Register DESC3_30_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_30_CPU_IGP_4 0x142EC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_30_CPU_IGP_4 0x1E7942ECu

//! Register Reset Value
#define DESC3_30_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_30_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_30_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_31_CPU_IGP_4 Register DESC0_31_CPU_IGP_4 - CPU Ingress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_31_CPU_IGP_4 0x142F0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC0_31_CPU_IGP_4 0x1E7942F0u

//! Register Reset Value
#define DESC0_31_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_31_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_31_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_31_CPU_IGP_4 Register DESC1_31_CPU_IGP_4 - CPU Ingress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_31_CPU_IGP_4 0x142F4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC1_31_CPU_IGP_4 0x1E7942F4u

//! Register Reset Value
#define DESC1_31_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_31_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_31_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_31_CPU_IGP_4 Register DESC2_31_CPU_IGP_4 - CPU Ingress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_31_CPU_IGP_4 0x142F8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC2_31_CPU_IGP_4 0x1E7942F8u

//! Register Reset Value
#define DESC2_31_CPU_IGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_31_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_31_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_31_CPU_IGP_4 Register DESC3_31_CPU_IGP_4 - CPU Ingress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_31_CPU_IGP_4 0x142FC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DESC3_31_CPU_IGP_4 0x1E7942FCu

//! Register Reset Value
#define DESC3_31_CPU_IGP_4_RST 0x80000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_31_CPU_IGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_31_CPU_IGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_DMA_IGP_5 Register CFG_DMA_IGP_5 - DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_5 0x15000
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_5 0x1E795000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_5_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_5_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_5_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_5_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_5_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_5_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_5_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_5_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_5_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_5_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_5_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_5_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_5_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_5_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_5 Register WM_DMA_IGP_5 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_5 0x15004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_5 0x1E795004u

//! Register Reset Value
#define WM_DMA_IGP_5_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_5_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_5_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_5_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_5_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_5 Register POCC_DMA_IGP_5 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_5 0x15008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_5 0x1E795008u

//! Register Reset Value
#define POCC_DMA_IGP_5_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_5_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_5_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_5 Register EQPC_DMA_IGP_5 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_5 0x1500C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_5 0x1E79500Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_5 0x1E795010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_5 0x1E795014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_5 0x1E795018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_5 0x1E79501Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_5 0x1E795020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_5_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_5_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_5_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_5_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_5_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_5_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_5_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_5_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_5_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_5_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_5_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_5_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_5_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_5 Register IRNICR_DMA_IGP_5 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_5 0x15024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_5 0x1E795024u

//! Register Reset Value
#define IRNICR_DMA_IGP_5_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_5_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_5_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_5_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_5_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_5_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_5_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_5_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_5_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_5_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_5_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_5 Register IRNEN_DMA_IGP_5 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_5 0x15028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_5 0x1E795028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_5_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_5_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_5_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_5_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_5_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_5_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_5_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_5_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_5_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_5_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_5 Register DPTR_DMA_IGP_5 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_5 0x15030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_5 0x1E795030u

//! Register Reset Value
#define DPTR_DMA_IGP_5_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_5_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_5_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_5_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_5_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_5_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_5_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_5_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_5_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_5 Register DICC_IGP_5 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_5 0x15038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_5 0x1E795038u

//! Register Reset Value
#define DICC_IGP_5_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_5_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_5_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_5 Register DCNTR_IGP_5 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_5 0x1504C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_5 0x1E79504Cu

//! Register Reset Value
#define DCNTR_IGP_5_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_5_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_6 0x1E796000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_6_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_6_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_6_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_6_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_6_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_6_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_6_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_6_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_6_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_6_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_6_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_6_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_6_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_6_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_6 Register WM_DMA_IGP_6 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_6 0x16004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_6 0x1E796004u

//! Register Reset Value
#define WM_DMA_IGP_6_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_6_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_6_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_6_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_6_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_6 Register POCC_DMA_IGP_6 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_6 0x16008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_6 0x1E796008u

//! Register Reset Value
#define POCC_DMA_IGP_6_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_6_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_6_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_6 Register EQPC_DMA_IGP_6 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_6 0x1600C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_6 0x1E79600Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_6 0x1E796010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_6 0x1E796014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_6 0x1E796018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_6 0x1E79601Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_6 0x1E796020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_6_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_6_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_6_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_6_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_6_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_6_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_6_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_6_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_6_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_6_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_6_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_6_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_6_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_6 Register IRNICR_DMA_IGP_6 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_6 0x16024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_6 0x1E796024u

//! Register Reset Value
#define IRNICR_DMA_IGP_6_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_6_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_6_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_6_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_6_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_6_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_6_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_6_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_6_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_6_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_6_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_6 Register IRNEN_DMA_IGP_6 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_6 0x16028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_6 0x1E796028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_6_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_6_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_6_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_6_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_6_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_6_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_6_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_6_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_6_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_6_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_6 Register DPTR_DMA_IGP_6 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_6 0x16030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_6 0x1E796030u

//! Register Reset Value
#define DPTR_DMA_IGP_6_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_6_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_6_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_6_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_6_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_6_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_6_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_6_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_6_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_6 Register DICC_IGP_6 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_6 0x16038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_6 0x1E796038u

//! Register Reset Value
#define DICC_IGP_6_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_6_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_6_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_6 Register DCNTR_IGP_6 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_6 0x1604C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_6 0x1E79604Cu

//! Register Reset Value
#define DCNTR_IGP_6_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_6_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_7 0x1E797000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_7_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_7_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_7_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_7_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_7_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_7_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_7_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_7_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_7_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_7_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_7_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_7_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_7_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_7_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_7 Register WM_DMA_IGP_7 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_7 0x17004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_7 0x1E797004u

//! Register Reset Value
#define WM_DMA_IGP_7_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_7_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_7_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_7_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_7_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_7 Register POCC_DMA_IGP_7 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_7 0x17008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_7 0x1E797008u

//! Register Reset Value
#define POCC_DMA_IGP_7_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_7_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_7_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_7 Register EQPC_DMA_IGP_7 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_7 0x1700C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_7 0x1E79700Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_7 0x1E797010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_7 0x1E797014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_7 0x1E797018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_7 0x1E79701Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_7 0x1E797020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_7_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_7_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_7_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_7_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_7_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_7_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_7_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_7_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_7_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_7_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_7_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_7_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_7_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_7 Register IRNICR_DMA_IGP_7 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_7 0x17024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_7 0x1E797024u

//! Register Reset Value
#define IRNICR_DMA_IGP_7_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_7_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_7_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_7_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_7_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_7_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_7_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_7_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_7_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_7_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_7_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_7 Register IRNEN_DMA_IGP_7 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_7 0x17028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_7 0x1E797028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_7_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_7_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_7_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_7_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_7_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_7_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_7_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_7_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_7_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_7_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_7 Register DPTR_DMA_IGP_7 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_7 0x17030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_7 0x1E797030u

//! Register Reset Value
#define DPTR_DMA_IGP_7_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_7_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_7_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_7_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_7_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_7_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_7_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_7_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_7_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_7 Register DICC_IGP_7 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_7 0x17038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_7 0x1E797038u

//! Register Reset Value
#define DICC_IGP_7_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_7_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_7_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_7 Register DCNTR_IGP_7 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_7 0x1704C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_7 0x1E79704Cu

//! Register Reset Value
#define DCNTR_IGP_7_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_7_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_8 0x1E798000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_8_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_8_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_8_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_8_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_8_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_8_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_8_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_8_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_8_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_8_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_8_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_8_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_8_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_8_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_8 Register WM_DMA_IGP_8 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_8 0x18004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_8 0x1E798004u

//! Register Reset Value
#define WM_DMA_IGP_8_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_8_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_8_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_8_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_8_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_8 Register POCC_DMA_IGP_8 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_8 0x18008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_8 0x1E798008u

//! Register Reset Value
#define POCC_DMA_IGP_8_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_8_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_8_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_8 Register EQPC_DMA_IGP_8 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_8 0x1800C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_8 0x1E79800Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_8 0x1E798010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_8 0x1E798014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_8 0x1E798018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_8 0x1E79801Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_8 0x1E798020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_8_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_8_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_8_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_8_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_8_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_8_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_8_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_8_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_8_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_8_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_8_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_8_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_8_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_8 Register IRNICR_DMA_IGP_8 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_8 0x18024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_8 0x1E798024u

//! Register Reset Value
#define IRNICR_DMA_IGP_8_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_8_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_8_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_8_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_8_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_8_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_8_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_8_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_8_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_8_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_8_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_8 Register IRNEN_DMA_IGP_8 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_8 0x18028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_8 0x1E798028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_8_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_8_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_8_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_8_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_8_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_8_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_8_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_8_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_8_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_8_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_8 Register DPTR_DMA_IGP_8 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_8 0x18030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_8 0x1E798030u

//! Register Reset Value
#define DPTR_DMA_IGP_8_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_8_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_8_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_8_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_8_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_8_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_8_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_8_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_8_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_8 Register DICC_IGP_8 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_8 0x18038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_8 0x1E798038u

//! Register Reset Value
#define DICC_IGP_8_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_8_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_8_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_8 Register DCNTR_IGP_8 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_8 0x1804C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_8 0x1E79804Cu

//! Register Reset Value
#define DCNTR_IGP_8_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_8_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_9 0x1E799000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_9_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_9_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_9_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_9_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_9_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_9_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_9_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_9_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_9_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_9_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_9_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_9_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_9_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_9_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_9 Register WM_DMA_IGP_9 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_9 0x19004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_9 0x1E799004u

//! Register Reset Value
#define WM_DMA_IGP_9_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_9_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_9_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_9_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_9_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_9 Register POCC_DMA_IGP_9 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_9 0x19008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_9 0x1E799008u

//! Register Reset Value
#define POCC_DMA_IGP_9_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_9_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_9_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_9 Register EQPC_DMA_IGP_9 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_9 0x1900C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_9 0x1E79900Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_9 0x1E799010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_9 0x1E799014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_9 0x1E799018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_9 0x1E79901Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_9 0x1E799020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_9_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_9_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_9_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_9_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_9_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_9_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_9_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_9_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_9_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_9_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_9_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_9_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_9_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_9 Register IRNICR_DMA_IGP_9 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_9 0x19024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_9 0x1E799024u

//! Register Reset Value
#define IRNICR_DMA_IGP_9_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_9_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_9_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_9_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_9_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_9_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_9_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_9_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_9_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_9_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_9_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_9 Register IRNEN_DMA_IGP_9 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_9 0x19028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_9 0x1E799028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_9_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_9_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_9_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_9_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_9_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_9_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_9_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_9_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_9_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_9_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_9 Register DPTR_DMA_IGP_9 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_9 0x19030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_9 0x1E799030u

//! Register Reset Value
#define DPTR_DMA_IGP_9_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_9_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_9_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_9_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_9_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_9_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_9_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_9_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_9_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_9 Register DICC_IGP_9 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_9 0x19038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_9 0x1E799038u

//! Register Reset Value
#define DICC_IGP_9_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_9_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_9_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_9 Register DCNTR_IGP_9 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_9 0x1904C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_9 0x1E79904Cu

//! Register Reset Value
#define DCNTR_IGP_9_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_9_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_10 0x1E79A000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_10_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_10_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_10_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_10_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_10_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_10_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_10_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_10_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_10_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_10_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_10_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_10_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_10_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_10_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_10 Register WM_DMA_IGP_10 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_10 0x1A004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_10 0x1E79A004u

//! Register Reset Value
#define WM_DMA_IGP_10_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_10_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_10_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_10_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_10_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_10 Register POCC_DMA_IGP_10 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_10 0x1A008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_10 0x1E79A008u

//! Register Reset Value
#define POCC_DMA_IGP_10_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_10_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_10_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_10 Register EQPC_DMA_IGP_10 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_10 0x1A00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_10 0x1E79A00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_10 0x1E79A010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_10 0x1E79A014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_10 0x1E79A018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_10 0x1E79A01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_10 0x1E79A020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_10_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_10_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_10_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_10_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_10_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_10_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_10_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_10_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_10_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_10_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_10_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_10_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_10_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_10 Register IRNICR_DMA_IGP_10 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_10 0x1A024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_10 0x1E79A024u

//! Register Reset Value
#define IRNICR_DMA_IGP_10_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_10_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_10_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_10_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_10_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_10_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_10_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_10_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_10_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_10_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_10_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_10 Register IRNEN_DMA_IGP_10 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_10 0x1A028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_10 0x1E79A028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_10_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_10_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_10_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_10_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_10_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_10_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_10_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_10_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_10_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_10_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_10 Register DPTR_DMA_IGP_10 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_10 0x1A030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_10 0x1E79A030u

//! Register Reset Value
#define DPTR_DMA_IGP_10_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_10_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_10_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_10_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_10_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_10_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_10_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_10_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_10_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_10 Register DICC_IGP_10 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_10 0x1A038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_10 0x1E79A038u

//! Register Reset Value
#define DICC_IGP_10_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_10_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_10_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_10 Register DCNTR_IGP_10 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_10 0x1A04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_10 0x1E79A04Cu

//! Register Reset Value
#define DCNTR_IGP_10_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_10_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_11 0x1E79B000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_11_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_11_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_11_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_11_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_11_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_11_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_11_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_11_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_11_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_11_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_11_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_11_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_11_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_11_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_11 Register WM_DMA_IGP_11 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_11 0x1B004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_11 0x1E79B004u

//! Register Reset Value
#define WM_DMA_IGP_11_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_11_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_11_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_11_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_11_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_11 Register POCC_DMA_IGP_11 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_11 0x1B008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_11 0x1E79B008u

//! Register Reset Value
#define POCC_DMA_IGP_11_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_11_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_11_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_11 Register EQPC_DMA_IGP_11 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_11 0x1B00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_11 0x1E79B00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_11 0x1E79B010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_11 0x1E79B014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_11 0x1E79B018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_11 0x1E79B01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_11 0x1E79B020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_11_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_11_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_11_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_11_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_11_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_11_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_11_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_11_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_11_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_11_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_11_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_11_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_11_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_11 Register IRNICR_DMA_IGP_11 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_11 0x1B024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_11 0x1E79B024u

//! Register Reset Value
#define IRNICR_DMA_IGP_11_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_11_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_11_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_11_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_11_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_11_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_11_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_11_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_11_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_11_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_11_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_11 Register IRNEN_DMA_IGP_11 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_11 0x1B028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_11 0x1E79B028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_11_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_11_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_11_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_11_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_11_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_11_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_11_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_11_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_11_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_11_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_11 Register DPTR_DMA_IGP_11 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_11 0x1B030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_11 0x1E79B030u

//! Register Reset Value
#define DPTR_DMA_IGP_11_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_11_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_11_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_11_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_11_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_11_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_11_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_11_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_11_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_11 Register DICC_IGP_11 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_11 0x1B038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_11 0x1E79B038u

//! Register Reset Value
#define DICC_IGP_11_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_11_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_11_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_11 Register DCNTR_IGP_11 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_11 0x1B04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_11 0x1E79B04Cu

//! Register Reset Value
#define DCNTR_IGP_11_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_11_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_12 0x1E79C000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_12_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_12_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_12_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_12_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_12_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_12_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_12_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_12_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_12_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_12_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_12_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_12_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_12_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_12_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_12 Register WM_DMA_IGP_12 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_12 0x1C004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_12 0x1E79C004u

//! Register Reset Value
#define WM_DMA_IGP_12_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_12_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_12_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_12_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_12_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_12 Register POCC_DMA_IGP_12 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_12 0x1C008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_12 0x1E79C008u

//! Register Reset Value
#define POCC_DMA_IGP_12_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_12_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_12_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_12 Register EQPC_DMA_IGP_12 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_12 0x1C00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_12 0x1E79C00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_12 0x1E79C010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_12 0x1E79C014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_12 0x1E79C018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_12 0x1E79C01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_12 0x1E79C020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_12_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_12_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_12_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_12_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_12_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_12_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_12_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_12_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_12_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_12_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_12_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_12_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_12_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_12 Register IRNICR_DMA_IGP_12 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_12 0x1C024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_12 0x1E79C024u

//! Register Reset Value
#define IRNICR_DMA_IGP_12_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_12_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_12_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_12_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_12_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_12_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_12_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_12_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_12_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_12_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_12_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_12 Register IRNEN_DMA_IGP_12 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_12 0x1C028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_12 0x1E79C028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_12_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_12_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_12_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_12_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_12_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_12_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_12_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_12_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_12_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_12_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_12 Register DPTR_DMA_IGP_12 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_12 0x1C030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_12 0x1E79C030u

//! Register Reset Value
#define DPTR_DMA_IGP_12_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_12_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_12_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_12_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_12_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_12_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_12_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_12_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_12_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_12 Register DICC_IGP_12 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_12 0x1C038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_12 0x1E79C038u

//! Register Reset Value
#define DICC_IGP_12_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_12_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_12_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_12 Register DCNTR_IGP_12 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_12 0x1C04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_12 0x1E79C04Cu

//! Register Reset Value
#define DCNTR_IGP_12_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_12_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_13 0x1E79D000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_13_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_13_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_13_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_13_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_13_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_13_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_13_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_13_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_13_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_13_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_13_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_13_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_13_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_13_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_13 Register WM_DMA_IGP_13 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_13 0x1D004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_13 0x1E79D004u

//! Register Reset Value
#define WM_DMA_IGP_13_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_13_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_13_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_13_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_13_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_13 Register POCC_DMA_IGP_13 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_13 0x1D008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_13 0x1E79D008u

//! Register Reset Value
#define POCC_DMA_IGP_13_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_13_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_13_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_13 Register EQPC_DMA_IGP_13 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_13 0x1D00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_13 0x1E79D00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_13 0x1E79D010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_13 0x1E79D014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_13 0x1E79D018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_13 0x1E79D01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_13 0x1E79D020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_13_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_13_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_13_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_13_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_13_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_13_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_13_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_13_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_13_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_13_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_13_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_13_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_13_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_13 Register IRNICR_DMA_IGP_13 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_13 0x1D024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_13 0x1E79D024u

//! Register Reset Value
#define IRNICR_DMA_IGP_13_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_13_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_13_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_13_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_13_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_13_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_13_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_13_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_13_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_13_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_13_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_13 Register IRNEN_DMA_IGP_13 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_13 0x1D028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_13 0x1E79D028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_13_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_13_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_13_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_13_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_13_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_13_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_13_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_13_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_13_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_13_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_13 Register DPTR_DMA_IGP_13 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_13 0x1D030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_13 0x1E79D030u

//! Register Reset Value
#define DPTR_DMA_IGP_13_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_13_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_13_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_13_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_13_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_13_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_13_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_13_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_13_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_13 Register DICC_IGP_13 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_13 0x1D038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_13 0x1E79D038u

//! Register Reset Value
#define DICC_IGP_13_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_13_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_13_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_13 Register DCNTR_IGP_13 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_13 0x1D04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_13 0x1E79D04Cu

//! Register Reset Value
#define DCNTR_IGP_13_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_13_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_14 0x1E79E000u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_14_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_14_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_14_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_14_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_14_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_14_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_14_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_14_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_14_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_14_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_14_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_14_POCCEN_EN 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_14_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_14_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_14 Register WM_DMA_IGP_14 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_14 0x1E004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_14 0x1E79E004u

//! Register Reset Value
#define WM_DMA_IGP_14_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_14_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_14_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_14_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_14_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_14 Register POCC_DMA_IGP_14 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_14 0x1E008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_14 0x1E79E008u

//! Register Reset Value
#define POCC_DMA_IGP_14_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_14_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_14_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_14 Register EQPC_DMA_IGP_14 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_14 0x1E00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_14 0x1E79E00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_14 0x1E79E010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_14 0x1E79E014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_14 0x1E79E018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_14 0x1E79E01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_14 0x1E79E020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_14_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_14_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_14_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_14_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_14_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_14_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_14_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_14_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_14_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_14_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_14_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_14_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_14_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_14 Register IRNICR_DMA_IGP_14 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_14 0x1E024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_14 0x1E79E024u

//! Register Reset Value
#define IRNICR_DMA_IGP_14_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_14_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_14_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_14_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_14_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_14_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_14_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_14_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_14_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_14_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_14_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_14 Register IRNEN_DMA_IGP_14 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_14 0x1E028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_14 0x1E79E028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_14_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_14_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_14_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_14_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_14_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_14_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_14_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_14_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_14_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_14_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_14 Register DPTR_DMA_IGP_14 - DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_14 0x1E030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_14 0x1E79E030u

//! Register Reset Value
#define DPTR_DMA_IGP_14_RST 0x00000101u

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_14_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_14_NSD_MASK 0x1u

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_14_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_14_NJD_MASK 0x100u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_14_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_14_SDPTR_MASK 0x10000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_14_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_14_JDPTR_MASK 0x1000000u

//! @}

//! \defgroup DICC_IGP_14 Register DICC_IGP_14 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_14 0x1E038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_14 0x1E79E038u

//! Register Reset Value
#define DICC_IGP_14_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_14_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_14_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_14 Register DCNTR_IGP_14 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_14 0x1E04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_14 0x1E79E04Cu

//! Register Reset Value
#define DCNTR_IGP_14_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_14_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_14_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_14_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_14_DCNT_MASK 0x3F00u

//! @}

//! \defgroup CFG_DMA_IGP_15 Register CFG_DMA_IGP_15 - Legacy DMA Ingress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_IGP_15 0x1F000
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_CFG_DMA_IGP_15 0x1E79F000u

//! Register Reset Value
#define CFG_DMA_IGP_15_RST 0x00001800u

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

//! Field BAS - Base Address Select
#define CFG_DMA_IGP_15_BAS_POS 4
//! Field BAS - Base Address Select
#define CFG_DMA_IGP_15_BAS_MASK 0x10u
//! Constant BA0 - Base Address 0
#define CONST_CFG_DMA_IGP_15_BAS_BA0 0x0
//! Constant BA1 - Base Address 1
#define CONST_CFG_DMA_IGP_15_BAS_BA1 0x1

//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_15_EQPCEN_POS 8
//! Field EQPCEN - Enqueue Counter Enable
#define CFG_DMA_IGP_15_EQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_15_EQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_15_EQPCEN_EN 0x1

//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_15_POCCEN_POS 9
//! Field POCCEN - Port Occupancy Counter Enable
#define CFG_DMA_IGP_15_POCCEN_MASK 0x200u
//! Constant DIS - DIS
#define CONST_CFG_DMA_IGP_15_POCCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_IGP_15_POCCEN_EN 0x1

//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_15_BP_EN_POS 10
//! Field BP_EN - Backpressure enable
#define CFG_DMA_IGP_15_BP_EN_MASK 0x400u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_15_BP_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_15_BP_EN_EN 0x1

//! Field DESC_INV - Descriptor Invert
#define CFG_DMA_IGP_15_DESC_INV_POS 11
//! Field DESC_INV - Descriptor Invert
#define CFG_DMA_IGP_15_DESC_INV_MASK 0x800u
//! Constant DIS - Disable
#define CONST_CFG_DMA_IGP_15_DESC_INV_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_IGP_15_DESC_INV_EN 0x1

//! Field LDM - Legacy Descriptor Mode
#define CFG_DMA_IGP_15_LDM_POS 12
//! Field LDM - Legacy Descriptor Mode
#define CFG_DMA_IGP_15_LDM_MASK 0x1000u
//! Constant NORMAL - Normal mode
#define CONST_CFG_DMA_IGP_15_LDM_NORMAL 0x0
//! Constant LEGACY - Legacy mode
#define CONST_CFG_DMA_IGP_15_LDM_LEGACY 0x1

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

//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_15_OVH_POS 20
//! Field OVH - Overhead Bytes
#define CFG_DMA_IGP_15_OVH_MASK 0xFF00000u

//! @}

//! \defgroup WM_DMA_IGP_15 Register WM_DMA_IGP_15 - DMA Ingress Port Watermarks
//! @{

//! Register Offset (relative)
#define WM_DMA_IGP_15 0x1F004
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_WM_DMA_IGP_15 0x1E79F004u

//! Register Reset Value
#define WM_DMA_IGP_15_RST 0x3FFF0000u

//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_15_LWM_POS 0
//! Field LWM - Low watermark threshold
#define WM_DMA_IGP_15_LWM_MASK 0x3FFFu

//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_15_HWM_POS 16
//! Field HWM - Low watermark threshold
#define WM_DMA_IGP_15_HWM_MASK 0x3FFF0000u

//! @}

//! \defgroup POCC_DMA_IGP_15 Register POCC_DMA_IGP_15 - DMA Ingress Port Occupancy Counter
//! @{

//! Register Offset (relative)
#define POCC_DMA_IGP_15 0x1F008
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_POCC_DMA_IGP_15 0x1E79F008u

//! Register Reset Value
#define POCC_DMA_IGP_15_RST 0x00000000u

//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_15_POCC_POS 0
//! Field POCC - Per Port Occupancy Counter
#define POCC_DMA_IGP_15_POCC_MASK 0x3FFFu

//! @}

//! \defgroup EQPC_DMA_IGP_15 Register EQPC_DMA_IGP_15 - DMA Ingress Port enqueue packet counter
//! @{

//! Register Offset (relative)
#define EQPC_DMA_IGP_15 0x1F00C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_EQPC_DMA_IGP_15 0x1E79F00Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC0_DMA_IGP_15 0x1E79F010u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC1_DMA_IGP_15 0x1E79F014u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC2_DMA_IGP_15 0x1E79F018u

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DISC_DESC3_DMA_IGP_15 0x1E79F01Cu

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
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNCR_DMA_IGP_15 0x1E79F020u

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

//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_15_LWMI_POS 1
//! Field LWMI - Low watermark interrupt
#define IRNCR_DMA_IGP_15_LWMI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_LWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_15_LWMI_INTACK 0x1

//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_15_HWMI_POS 2
//! Field HWMI - High watermark interrupt
#define IRNCR_DMA_IGP_15_HWMI_MASK 0x4u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_HWMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_IRNCR_DMA_IGP_15_HWMI_INTACK 0x1

//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_15_SDR_POS 3
//! Field SDR - Standard Descriptor ready
#define IRNCR_DMA_IGP_15_SDR_MASK 0x8u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_SDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_15_SDR_INTOCC 0x1

//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_15_JDR_POS 4
//! Field JDR - Jumbo Descriptor ready
#define IRNCR_DMA_IGP_15_JDR_MASK 0x10u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_IGP_15_JDR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_IGP_15_JDR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_IGP_15 Register IRNICR_DMA_IGP_15 - DMA Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_IGP_15 0x1F024
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNICR_DMA_IGP_15 0x1E79F024u

//! Register Reset Value
#define IRNICR_DMA_IGP_15_RST 0x00000000u

//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_15_PNA_POS 0
//! Field PNA - Packet Not Accepted
#define IRNICR_DMA_IGP_15_PNA_MASK 0x1u

//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_15_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNICR_DMA_IGP_15_LWMI_MASK 0x2u

//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_15_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNICR_DMA_IGP_15_HWMI_MASK 0x4u

//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_15_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNICR_DMA_IGP_15_SDR_MASK 0x8u

//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_15_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNICR_DMA_IGP_15_JDR_MASK 0x10u

//! @}

//! \defgroup IRNEN_DMA_IGP_15 Register IRNEN_DMA_IGP_15 - DMA Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_IGP_15 0x1F028
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_IRNEN_DMA_IGP_15 0x1E79F028u

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

//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_15_LWMI_POS 1
//! Field LWMI - Low Watermark Interrupt
#define IRNEN_DMA_IGP_15_LWMI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_LWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_LWMI_EN 0x1

//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_15_HWMI_POS 2
//! Field HWMI - High Watermark Interrupt
#define IRNEN_DMA_IGP_15_HWMI_MASK 0x4u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_HWMI_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_HWMI_EN 0x1

//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_15_SDR_POS 3
//! Field SDR - Standard Descriptor Ready
#define IRNEN_DMA_IGP_15_SDR_MASK 0x8u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_SDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_SDR_EN 0x1

//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_15_JDR_POS 4
//! Field JDR - Jumbo Descriptor Ready
#define IRNEN_DMA_IGP_15_JDR_MASK 0x10u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_IGP_15_JDR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_IGP_15_JDR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_IGP_15 Register DPTR_DMA_IGP_15 - Legacy DMA Ingress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_IGP_15 0x1F030
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DPTR_DMA_IGP_15 0x1E79F030u

//! Register Reset Value
#define DPTR_DMA_IGP_15_RST 0x0000071Fu

//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_15_NSD_POS 0
//! Field NSD - Number of Standard Descriptors
#define DPTR_DMA_IGP_15_NSD_MASK 0x1Fu

//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_15_NJD_POS 8
//! Field NJD - Number of Jumbo Descriptors
#define DPTR_DMA_IGP_15_NJD_MASK 0x700u

//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_15_SDPTR_POS 16
//! Field SDPTR - Standard Descriptor Pointer
#define DPTR_DMA_IGP_15_SDPTR_MASK 0x1F0000u

//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_15_JDPTR_POS 24
//! Field JDPTR - Jumbo Descriptor Pointer
#define DPTR_DMA_IGP_15_JDPTR_MASK 0x7000000u

//! @}

//! \defgroup DICC_IGP_15 Register DICC_IGP_15 - Ingress Port Discard counter
//! @{

//! Register Offset (relative)
#define DICC_IGP_15 0x1F038
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DICC_IGP_15 0x1E79F038u

//! Register Reset Value
#define DICC_IGP_15_RST 0x00000000u

//! Field DICC - Per Port Discard Counter
#define DICC_IGP_15_DICC_POS 0
//! Field DICC - Per Port Discard Counter
#define DICC_IGP_15_DICC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DCNTR_IGP_15 Register DCNTR_IGP_15 - Ingress Port Delay Counter
//! @{

//! Register Offset (relative)
#define DCNTR_IGP_15 0x1F04C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_DCNTR_IGP_15 0x1E79F04Cu

//! Register Reset Value
#define DCNTR_IGP_15_RST 0x00000100u

//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_15_DLY_POS 0
//! Field DLY - Additional Delay before TMU enqueue
#define DCNTR_IGP_15_DLY_MASK 0x3Fu

//! Field DCNT - Counter Value
#define DCNTR_IGP_15_DCNT_POS 8
//! Field DCNT - Counter Value
#define DCNTR_IGP_15_DCNT_MASK 0x3F00u

//! @}

//! \defgroup SDESC0_0_IGP_15 Register SDESC0_0_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_0_IGP_15 0x1F100
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_0_IGP_15 0x1E79F100u

//! Register Reset Value
#define SDESC0_0_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_0_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_0_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_0_IGP_15 Register SDESC1_0_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_0_IGP_15 0x1F104
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_0_IGP_15 0x1E79F104u

//! Register Reset Value
#define SDESC1_0_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_0_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_0_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_1_IGP_15 Register SDESC0_1_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_1_IGP_15 0x1F108
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_1_IGP_15 0x1E79F108u

//! Register Reset Value
#define SDESC0_1_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_1_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_1_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_1_IGP_15 Register SDESC1_1_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_1_IGP_15 0x1F10C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_1_IGP_15 0x1E79F10Cu

//! Register Reset Value
#define SDESC1_1_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_1_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_1_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_2_IGP_15 Register SDESC0_2_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_2_IGP_15 0x1F110
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_2_IGP_15 0x1E79F110u

//! Register Reset Value
#define SDESC0_2_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_2_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_2_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_2_IGP_15 Register SDESC1_2_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_2_IGP_15 0x1F114
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_2_IGP_15 0x1E79F114u

//! Register Reset Value
#define SDESC1_2_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_2_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_2_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_3_IGP_15 Register SDESC0_3_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_3_IGP_15 0x1F118
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_3_IGP_15 0x1E79F118u

//! Register Reset Value
#define SDESC0_3_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_3_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_3_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_3_IGP_15 Register SDESC1_3_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_3_IGP_15 0x1F11C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_3_IGP_15 0x1E79F11Cu

//! Register Reset Value
#define SDESC1_3_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_3_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_3_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_4_IGP_15 Register SDESC0_4_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_4_IGP_15 0x1F120
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_4_IGP_15 0x1E79F120u

//! Register Reset Value
#define SDESC0_4_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_4_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_4_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_4_IGP_15 Register SDESC1_4_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_4_IGP_15 0x1F124
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_4_IGP_15 0x1E79F124u

//! Register Reset Value
#define SDESC1_4_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_4_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_4_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_5_IGP_15 Register SDESC0_5_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_5_IGP_15 0x1F128
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_5_IGP_15 0x1E79F128u

//! Register Reset Value
#define SDESC0_5_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_5_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_5_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_5_IGP_15 Register SDESC1_5_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_5_IGP_15 0x1F12C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_5_IGP_15 0x1E79F12Cu

//! Register Reset Value
#define SDESC1_5_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_5_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_5_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_6_IGP_15 Register SDESC0_6_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_6_IGP_15 0x1F130
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_6_IGP_15 0x1E79F130u

//! Register Reset Value
#define SDESC0_6_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_6_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_6_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_6_IGP_15 Register SDESC1_6_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_6_IGP_15 0x1F134
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_6_IGP_15 0x1E79F134u

//! Register Reset Value
#define SDESC1_6_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_6_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_6_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_7_IGP_15 Register SDESC0_7_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_7_IGP_15 0x1F138
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_7_IGP_15 0x1E79F138u

//! Register Reset Value
#define SDESC0_7_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_7_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_7_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_7_IGP_15 Register SDESC1_7_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_7_IGP_15 0x1F13C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_7_IGP_15 0x1E79F13Cu

//! Register Reset Value
#define SDESC1_7_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_7_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_7_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_8_IGP_15 Register SDESC0_8_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_8_IGP_15 0x1F140
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_8_IGP_15 0x1E79F140u

//! Register Reset Value
#define SDESC0_8_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_8_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_8_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_8_IGP_15 Register SDESC1_8_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_8_IGP_15 0x1F144
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_8_IGP_15 0x1E79F144u

//! Register Reset Value
#define SDESC1_8_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_8_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_8_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_9_IGP_15 Register SDESC0_9_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_9_IGP_15 0x1F148
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_9_IGP_15 0x1E79F148u

//! Register Reset Value
#define SDESC0_9_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_9_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_9_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_9_IGP_15 Register SDESC1_9_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_9_IGP_15 0x1F14C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_9_IGP_15 0x1E79F14Cu

//! Register Reset Value
#define SDESC1_9_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_9_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_9_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_10_IGP_15 Register SDESC0_10_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_10_IGP_15 0x1F150
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_10_IGP_15 0x1E79F150u

//! Register Reset Value
#define SDESC0_10_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_10_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_10_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_10_IGP_15 Register SDESC1_10_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_10_IGP_15 0x1F154
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_10_IGP_15 0x1E79F154u

//! Register Reset Value
#define SDESC1_10_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_10_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_10_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_11_IGP_15 Register SDESC0_11_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_11_IGP_15 0x1F158
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_11_IGP_15 0x1E79F158u

//! Register Reset Value
#define SDESC0_11_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_11_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_11_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_11_IGP_15 Register SDESC1_11_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_11_IGP_15 0x1F15C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_11_IGP_15 0x1E79F15Cu

//! Register Reset Value
#define SDESC1_11_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_11_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_11_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_12_IGP_15 Register SDESC0_12_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_12_IGP_15 0x1F160
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_12_IGP_15 0x1E79F160u

//! Register Reset Value
#define SDESC0_12_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_12_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_12_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_12_IGP_15 Register SDESC1_12_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_12_IGP_15 0x1F164
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_12_IGP_15 0x1E79F164u

//! Register Reset Value
#define SDESC1_12_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_12_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_12_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_13_IGP_15 Register SDESC0_13_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_13_IGP_15 0x1F168
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_13_IGP_15 0x1E79F168u

//! Register Reset Value
#define SDESC0_13_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_13_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_13_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_13_IGP_15 Register SDESC1_13_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_13_IGP_15 0x1F16C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_13_IGP_15 0x1E79F16Cu

//! Register Reset Value
#define SDESC1_13_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_13_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_13_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_14_IGP_15 Register SDESC0_14_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_14_IGP_15 0x1F170
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_14_IGP_15 0x1E79F170u

//! Register Reset Value
#define SDESC0_14_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_14_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_14_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_14_IGP_15 Register SDESC1_14_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_14_IGP_15 0x1F174
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_14_IGP_15 0x1E79F174u

//! Register Reset Value
#define SDESC1_14_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_14_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_14_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_15_IGP_15 Register SDESC0_15_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_15_IGP_15 0x1F178
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_15_IGP_15 0x1E79F178u

//! Register Reset Value
#define SDESC0_15_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_15_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_15_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_15_IGP_15 Register SDESC1_15_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_15_IGP_15 0x1F17C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_15_IGP_15 0x1E79F17Cu

//! Register Reset Value
#define SDESC1_15_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_15_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_15_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_16_IGP_15 Register SDESC0_16_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_16_IGP_15 0x1F180
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_16_IGP_15 0x1E79F180u

//! Register Reset Value
#define SDESC0_16_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_16_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_16_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_16_IGP_15 Register SDESC1_16_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_16_IGP_15 0x1F184
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_16_IGP_15 0x1E79F184u

//! Register Reset Value
#define SDESC1_16_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_16_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_16_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_17_IGP_15 Register SDESC0_17_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_17_IGP_15 0x1F188
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_17_IGP_15 0x1E79F188u

//! Register Reset Value
#define SDESC0_17_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_17_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_17_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_17_IGP_15 Register SDESC1_17_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_17_IGP_15 0x1F18C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_17_IGP_15 0x1E79F18Cu

//! Register Reset Value
#define SDESC1_17_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_17_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_17_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_18_IGP_15 Register SDESC0_18_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_18_IGP_15 0x1F190
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_18_IGP_15 0x1E79F190u

//! Register Reset Value
#define SDESC0_18_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_18_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_18_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_18_IGP_15 Register SDESC1_18_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_18_IGP_15 0x1F194
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_18_IGP_15 0x1E79F194u

//! Register Reset Value
#define SDESC1_18_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_18_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_18_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_19_IGP_15 Register SDESC0_19_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_19_IGP_15 0x1F198
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_19_IGP_15 0x1E79F198u

//! Register Reset Value
#define SDESC0_19_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_19_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_19_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_19_IGP_15 Register SDESC1_19_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_19_IGP_15 0x1F19C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_19_IGP_15 0x1E79F19Cu

//! Register Reset Value
#define SDESC1_19_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_19_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_19_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_20_IGP_15 Register SDESC0_20_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_20_IGP_15 0x1F1A0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_20_IGP_15 0x1E79F1A0u

//! Register Reset Value
#define SDESC0_20_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_20_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_20_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_20_IGP_15 Register SDESC1_20_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_20_IGP_15 0x1F1A4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_20_IGP_15 0x1E79F1A4u

//! Register Reset Value
#define SDESC1_20_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_20_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_20_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_21_IGP_15 Register SDESC0_21_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_21_IGP_15 0x1F1A8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_21_IGP_15 0x1E79F1A8u

//! Register Reset Value
#define SDESC0_21_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_21_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_21_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_21_IGP_15 Register SDESC1_21_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_21_IGP_15 0x1F1AC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_21_IGP_15 0x1E79F1ACu

//! Register Reset Value
#define SDESC1_21_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_21_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_21_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_22_IGP_15 Register SDESC0_22_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_22_IGP_15 0x1F1B0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_22_IGP_15 0x1E79F1B0u

//! Register Reset Value
#define SDESC0_22_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_22_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_22_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_22_IGP_15 Register SDESC1_22_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_22_IGP_15 0x1F1B4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_22_IGP_15 0x1E79F1B4u

//! Register Reset Value
#define SDESC1_22_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_22_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_22_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_23_IGP_15 Register SDESC0_23_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_23_IGP_15 0x1F1B8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_23_IGP_15 0x1E79F1B8u

//! Register Reset Value
#define SDESC0_23_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_23_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_23_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_23_IGP_15 Register SDESC1_23_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_23_IGP_15 0x1F1BC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_23_IGP_15 0x1E79F1BCu

//! Register Reset Value
#define SDESC1_23_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_23_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_23_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_24_IGP_15 Register SDESC0_24_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_24_IGP_15 0x1F1C0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_24_IGP_15 0x1E79F1C0u

//! Register Reset Value
#define SDESC0_24_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_24_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_24_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_24_IGP_15 Register SDESC1_24_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_24_IGP_15 0x1F1C4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_24_IGP_15 0x1E79F1C4u

//! Register Reset Value
#define SDESC1_24_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_24_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_24_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_25_IGP_15 Register SDESC0_25_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_25_IGP_15 0x1F1C8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_25_IGP_15 0x1E79F1C8u

//! Register Reset Value
#define SDESC0_25_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_25_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_25_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_25_IGP_15 Register SDESC1_25_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_25_IGP_15 0x1F1CC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_25_IGP_15 0x1E79F1CCu

//! Register Reset Value
#define SDESC1_25_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_25_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_25_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_26_IGP_15 Register SDESC0_26_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_26_IGP_15 0x1F1D0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_26_IGP_15 0x1E79F1D0u

//! Register Reset Value
#define SDESC0_26_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_26_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_26_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_26_IGP_15 Register SDESC1_26_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_26_IGP_15 0x1F1D4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_26_IGP_15 0x1E79F1D4u

//! Register Reset Value
#define SDESC1_26_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_26_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_26_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_27_IGP_15 Register SDESC0_27_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_27_IGP_15 0x1F1D8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_27_IGP_15 0x1E79F1D8u

//! Register Reset Value
#define SDESC0_27_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_27_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_27_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_27_IGP_15 Register SDESC1_27_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_27_IGP_15 0x1F1DC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_27_IGP_15 0x1E79F1DCu

//! Register Reset Value
#define SDESC1_27_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_27_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_27_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_28_IGP_15 Register SDESC0_28_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_28_IGP_15 0x1F1E0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_28_IGP_15 0x1E79F1E0u

//! Register Reset Value
#define SDESC0_28_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_28_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_28_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_28_IGP_15 Register SDESC1_28_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_28_IGP_15 0x1F1E4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_28_IGP_15 0x1E79F1E4u

//! Register Reset Value
#define SDESC1_28_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_28_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_28_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_29_IGP_15 Register SDESC0_29_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_29_IGP_15 0x1F1E8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_29_IGP_15 0x1E79F1E8u

//! Register Reset Value
#define SDESC0_29_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_29_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_29_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_29_IGP_15 Register SDESC1_29_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_29_IGP_15 0x1F1EC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_29_IGP_15 0x1E79F1ECu

//! Register Reset Value
#define SDESC1_29_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_29_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_29_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_30_IGP_15 Register SDESC0_30_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_30_IGP_15 0x1F1F0
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_30_IGP_15 0x1E79F1F0u

//! Register Reset Value
#define SDESC0_30_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_30_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_30_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_30_IGP_15 Register SDESC1_30_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_30_IGP_15 0x1F1F4
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_30_IGP_15 0x1E79F1F4u

//! Register Reset Value
#define SDESC1_30_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_30_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_30_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC0_31_IGP_15 Register SDESC0_31_IGP_15 - DMA Ingress Port Standard Descriptor DW0
//! @{

//! Register Offset (relative)
#define SDESC0_31_IGP_15 0x1F1F8
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC0_31_IGP_15 0x1E79F1F8u

//! Register Reset Value
#define SDESC0_31_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define SDESC0_31_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define SDESC0_31_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup SDESC1_31_IGP_15 Register SDESC1_31_IGP_15 - DMA Ingress Port Standard Descriptor DW1
//! @{

//! Register Offset (relative)
#define SDESC1_31_IGP_15 0x1F1FC
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_SDESC1_31_IGP_15 0x1E79F1FCu

//! Register Reset Value
#define SDESC1_31_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define SDESC1_31_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define SDESC1_31_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_0_IGP_15 Register JDESC0_0_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_0_IGP_15 0x1F800
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_0_IGP_15 0x1E79F800u

//! Register Reset Value
#define JDESC0_0_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_0_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_0_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_0_IGP_15 Register JDESC1_0_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_0_IGP_15 0x1F804
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_0_IGP_15 0x1E79F804u

//! Register Reset Value
#define JDESC1_0_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_0_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_0_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_1_IGP_15 Register JDESC0_1_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_1_IGP_15 0x1F808
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_1_IGP_15 0x1E79F808u

//! Register Reset Value
#define JDESC0_1_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_1_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_1_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_1_IGP_15 Register JDESC1_1_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_1_IGP_15 0x1F80C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_1_IGP_15 0x1E79F80Cu

//! Register Reset Value
#define JDESC1_1_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_1_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_1_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_2_IGP_15 Register JDESC0_2_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_2_IGP_15 0x1F810
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_2_IGP_15 0x1E79F810u

//! Register Reset Value
#define JDESC0_2_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_2_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_2_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_2_IGP_15 Register JDESC1_2_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_2_IGP_15 0x1F814
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_2_IGP_15 0x1E79F814u

//! Register Reset Value
#define JDESC1_2_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_2_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_2_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_3_IGP_15 Register JDESC0_3_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_3_IGP_15 0x1F818
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_3_IGP_15 0x1E79F818u

//! Register Reset Value
#define JDESC0_3_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_3_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_3_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_3_IGP_15 Register JDESC1_3_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_3_IGP_15 0x1F81C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_3_IGP_15 0x1E79F81Cu

//! Register Reset Value
#define JDESC1_3_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_3_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_3_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_4_IGP_15 Register JDESC0_4_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_4_IGP_15 0x1F820
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_4_IGP_15 0x1E79F820u

//! Register Reset Value
#define JDESC0_4_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_4_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_4_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_4_IGP_15 Register JDESC1_4_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_4_IGP_15 0x1F824
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_4_IGP_15 0x1E79F824u

//! Register Reset Value
#define JDESC1_4_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_4_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_4_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_5_IGP_15 Register JDESC0_5_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_5_IGP_15 0x1F828
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_5_IGP_15 0x1E79F828u

//! Register Reset Value
#define JDESC0_5_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_5_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_5_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_5_IGP_15 Register JDESC1_5_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_5_IGP_15 0x1F82C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_5_IGP_15 0x1E79F82Cu

//! Register Reset Value
#define JDESC1_5_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_5_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_5_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_6_IGP_15 Register JDESC0_6_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_6_IGP_15 0x1F830
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_6_IGP_15 0x1E79F830u

//! Register Reset Value
#define JDESC0_6_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_6_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_6_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_6_IGP_15 Register JDESC1_6_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_6_IGP_15 0x1F834
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_6_IGP_15 0x1E79F834u

//! Register Reset Value
#define JDESC1_6_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_6_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_6_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC0_7_IGP_15 Register JDESC0_7_IGP_15 - DMA Ingress Port Jumbo Descriptor DW0
//! @{

//! Register Offset (relative)
#define JDESC0_7_IGP_15 0x1F838
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC0_7_IGP_15 0x1E79F838u

//! Register Reset Value
#define JDESC0_7_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define JDESC0_7_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define JDESC0_7_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup JDESC1_7_IGP_15 Register JDESC1_7_IGP_15 - DMA Ingress Port Jumbo Descriptor DW1
//! @{

//! Register Offset (relative)
#define JDESC1_7_IGP_15 0x1F83C
//! Register Offset (absolute) for 1st Instance CBM_EQM
#define CBM_EQM_JDESC1_7_IGP_15 0x1E79F83Cu

//! Register Reset Value
#define JDESC1_7_IGP_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define JDESC1_7_IGP_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define JDESC1_7_IGP_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
