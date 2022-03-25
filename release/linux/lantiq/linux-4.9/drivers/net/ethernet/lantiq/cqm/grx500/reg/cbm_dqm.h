//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : D:/Users/shij/Perforce/l1033.grx500.win.v_shij.priv.dfv.grx500.dfv/ipg_lsd/lsd_sys/source/xml/reg_files/CBM_DQM.xml
// Register File Name  : CBM_DQM
// Register File Title : Central Buffer Manager Dequeue Manager Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CBM_DQM_H
#define _CBM_DQM_H

//! \defgroup CBM_DQM Register File CBM_DQM - Central Buffer Manager Dequeue Manager Register Description
//! @{

//! Base Address of CBM_DQM
#define CBM_DQM_MODULE_BASE 0x1E7C0000u

//! \defgroup CBM_DQM_CTRL Register CBM_DQM_CTRL - CBM Dequeue Manager Control Register
//! @{

//! Register Offset (relative)
#define CBM_DQM_CTRL 0x0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CBM_DQM_CTRL 0x1E7C0000u

//! Register Reset Value
#define CBM_DQM_CTRL_RST 0x00000000u

//! Field DQM_EN - Enable Dequeue Manager Core State Machine
#define CBM_DQM_CTRL_DQM_EN_POS 0
//! Field DQM_EN - Enable Dequeue Manager Core State Machine
#define CBM_DQM_CTRL_DQM_EN_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_DQM_CTRL_DQM_EN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_DQM_CTRL_DQM_EN_EN 0x1

//! Field DQM_FRZ - Freeze Dequeue Manager Core State Machine
#define CBM_DQM_CTRL_DQM_FRZ_POS 1
//! Field DQM_FRZ - Freeze Dequeue Manager Core State Machine
#define CBM_DQM_CTRL_DQM_FRZ_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_DQM_CTRL_DQM_FRZ_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_DQM_CTRL_DQM_FRZ_EN 0x1

//! Field DQM_ACT - Dequeue Manager Activity Status
#define CBM_DQM_CTRL_DQM_ACT_POS 2
//! Field DQM_ACT - Dequeue Manager Activity Status
#define CBM_DQM_CTRL_DQM_ACT_MASK 0x4u
//! Constant INACTIVE - Inactive
#define CONST_CBM_DQM_CTRL_DQM_ACT_INACTIVE 0x0
//! Constant ACTIVE - Active
#define CONST_CBM_DQM_CTRL_DQM_ACT_ACTIVE 0x1

//! Field DQQCEN - Dequeue Queue Counter Enable
#define CBM_DQM_CTRL_DQQCEN_POS 4
//! Field DQQCEN - Dequeue Queue Counter Enable
#define CBM_DQM_CTRL_DQQCEN_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_DQM_CTRL_DQQCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_DQM_CTRL_DQQCEN_EN 0x1

//! Field MSEL - Counter Mode Select
#define CBM_DQM_CTRL_MSEL_POS 5
//! Field MSEL - Counter Mode Select
#define CBM_DQM_CTRL_MSEL_MASK 0x20u
//! Constant PKT - Packet counter
#define CONST_CBM_DQM_CTRL_MSEL_PKT 0x0
//! Constant BYTE - Byte counter
#define CONST_CBM_DQM_CTRL_MSEL_BYTE 0x1

//! @}

//! \defgroup DBG_DQM_0 Register DBG_DQM_0 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_DQM_0 0x10
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DBG_DQM_0 0x1E7C0010u

//! Register Reset Value
#define DBG_DQM_0_RST 0x00000000u

//! Field DBG - Debug
#define DBG_DQM_0_DBG_POS 0
//! Field DBG - Debug
#define DBG_DQM_0_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DBG_DQM_1 Register DBG_DQM_1 - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define DBG_DQM_1 0x14
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DBG_DQM_1 0x1E7C0014u

//! Register Reset Value
#define DBG_DQM_1_RST 0x00000000u

//! Field DBG - Debug
#define DBG_DQM_1_DBG_POS 0
//! Field DBG - Debug
#define DBG_DQM_1_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TEST_DQM_0 Register TEST_DQM_0 - Hardware Test Register
//! @{

//! Register Offset (relative)
#define TEST_DQM_0 0x20
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_TEST_DQM_0 0x1E7C0020u

//! Register Reset Value
#define TEST_DQM_0_RST 0x00000000u

//! Field TEST - Test
#define TEST_DQM_0_TEST_POS 0
//! Field TEST - Test
#define TEST_DQM_0_TEST_MASK 0xFFFFFFFFu

//! @}

//! \defgroup TEST_DQM_1 Register TEST_DQM_1 - Hardware Test Register
//! @{

//! Register Offset (relative)
#define TEST_DQM_1 0x24
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_TEST_DQM_1 0x1E7C0024u

//! Register Reset Value
#define TEST_DQM_1_RST 0x00000000u

//! Field TEST - Test
#define TEST_DQM_1_TEST_POS 0
//! Field TEST - Test
#define TEST_DQM_1_TEST_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_0 Register CFG_CPU_EGP_0 - CPU Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_0 0x10000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_0 0x1E7D0000u

//! Register Reset Value
#define CFG_CPU_EGP_0_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_0_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_0_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_0_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_0_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_0_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_0_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_0_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_0_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_0_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_0_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_0_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_0_BFBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_0_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_0_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_0_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_0_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_0_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_0_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_0 Register DQPC_CPU_EGP_0 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_0 0x10004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_0 0x1E7D0004u

//! Register Reset Value
#define DQPC_CPU_EGP_0_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_0_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_0_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_0 Register IRNCR_CPU_EGP_0 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_0 0x10020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_0 0x1E7D0020u

//! Register Reset Value
#define IRNCR_CPU_EGP_0_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_0_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_0_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_0_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_0_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_0_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_0_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_0_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_0_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_0 Register IRNICR_CPU_EGP_0 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_0 0x10024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_0 0x1E7D0024u

//! Register Reset Value
#define IRNICR_CPU_EGP_0_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_0_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_0_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_0_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_0_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_0 Register IRNEN_CPU_EGP_0 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_0 0x10028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_0 0x1E7D0028u

//! Register Reset Value
#define IRNEN_CPU_EGP_0_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_0_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_0_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_0_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_0_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_0_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_0_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_0_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_0_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_0 Register DPTR_CPU_EGP_0 - CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_0 0x10030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_0 0x1E7D0030u

//! Register Reset Value
#define DPTR_CPU_EGP_0_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_0_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_0_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_0_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_0_DPTR_MASK 0x10000u

//! @}

//! \defgroup BPRC_CPU_EGP_0 Register BPRC_CPU_EGP_0 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_0 0x10034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_0 0x1E7D0034u

//! Register Reset Value
#define BPRC_CPU_EGP_0_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_0_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_0_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PTR_RTN_0_CPU_EGP_0 Register PTR_RTN_0_CPU_EGP_0 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_0_CPU_EGP_0 0x10080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_0_CPU_EGP_0 0x1E7D0080u

//! Register Reset Value
#define PTR_RTN_0_CPU_EGP_0_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_0_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_0_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_EGP_0 Register DESC0_0_CPU_EGP_0 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_EGP_0 0x10100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_0_CPU_EGP_0 0x1E7D0100u

//! Register Reset Value
#define DESC0_0_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_EGP_0 Register DESC1_0_CPU_EGP_0 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_EGP_0 0x10104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_0_CPU_EGP_0 0x1E7D0104u

//! Register Reset Value
#define DESC1_0_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_EGP_0 Register DESC2_0_CPU_EGP_0 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_EGP_0 0x10108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_0_CPU_EGP_0 0x1E7D0108u

//! Register Reset Value
#define DESC2_0_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_EGP_0 Register DESC3_0_CPU_EGP_0 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_EGP_0 0x1010C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_0_CPU_EGP_0 0x1E7D010Cu

//! Register Reset Value
#define DESC3_0_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_EGP_0 Register DESC0_1_CPU_EGP_0 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_EGP_0 0x10110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_1_CPU_EGP_0 0x1E7D0110u

//! Register Reset Value
#define DESC0_1_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_EGP_0 Register DESC1_1_CPU_EGP_0 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_EGP_0 0x10114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_1_CPU_EGP_0 0x1E7D0114u

//! Register Reset Value
#define DESC1_1_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_EGP_0 Register DESC2_1_CPU_EGP_0 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_EGP_0 0x10118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_1_CPU_EGP_0 0x1E7D0118u

//! Register Reset Value
#define DESC2_1_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_EGP_0 Register DESC3_1_CPU_EGP_0 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_EGP_0 0x1011C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_1_CPU_EGP_0 0x1E7D011Cu

//! Register Reset Value
#define DESC3_1_CPU_EGP_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_1 Register CFG_CPU_EGP_1 - CPU Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_1 0x11000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_1 0x1E7D1000u

//! Register Reset Value
#define CFG_CPU_EGP_1_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_1_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_1_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_1_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_1_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_1_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_1_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_1_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_1_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_1_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_1_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_1_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_1_BFBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_1_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_1_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_1_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_1_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_1_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_1_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_1 Register DQPC_CPU_EGP_1 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_1 0x11004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_1 0x1E7D1004u

//! Register Reset Value
#define DQPC_CPU_EGP_1_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_1_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_1_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_1 Register IRNCR_CPU_EGP_1 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_1 0x11020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_1 0x1E7D1020u

//! Register Reset Value
#define IRNCR_CPU_EGP_1_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_1_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_1_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_1_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_1_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_1_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_1_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_1_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_1_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_1 Register IRNICR_CPU_EGP_1 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_1 0x11024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_1 0x1E7D1024u

//! Register Reset Value
#define IRNICR_CPU_EGP_1_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_1_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_1_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_1_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_1_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_1 Register IRNEN_CPU_EGP_1 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_1 0x11028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_1 0x1E7D1028u

//! Register Reset Value
#define IRNEN_CPU_EGP_1_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_1_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_1_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_1_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_1_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_1_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_1_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_1_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_1_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_1 Register DPTR_CPU_EGP_1 - CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_1 0x11030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_1 0x1E7D1030u

//! Register Reset Value
#define DPTR_CPU_EGP_1_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_1_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_1_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_1_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_1_DPTR_MASK 0x10000u

//! @}

//! \defgroup BPRC_CPU_EGP_1 Register BPRC_CPU_EGP_1 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_1 0x11034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_1 0x1E7D1034u

//! Register Reset Value
#define BPRC_CPU_EGP_1_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_1_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_1_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PTR_RTN_0_CPU_EGP_1 Register PTR_RTN_0_CPU_EGP_1 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_0_CPU_EGP_1 0x11080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_0_CPU_EGP_1 0x1E7D1080u

//! Register Reset Value
#define PTR_RTN_0_CPU_EGP_1_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_1_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_1_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_EGP_1 Register DESC0_0_CPU_EGP_1 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_EGP_1 0x11100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_0_CPU_EGP_1 0x1E7D1100u

//! Register Reset Value
#define DESC0_0_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_EGP_1 Register DESC1_0_CPU_EGP_1 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_EGP_1 0x11104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_0_CPU_EGP_1 0x1E7D1104u

//! Register Reset Value
#define DESC1_0_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_EGP_1 Register DESC2_0_CPU_EGP_1 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_EGP_1 0x11108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_0_CPU_EGP_1 0x1E7D1108u

//! Register Reset Value
#define DESC2_0_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_EGP_1 Register DESC3_0_CPU_EGP_1 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_EGP_1 0x1110C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_0_CPU_EGP_1 0x1E7D110Cu

//! Register Reset Value
#define DESC3_0_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_EGP_1 Register DESC0_1_CPU_EGP_1 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_EGP_1 0x11110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_1_CPU_EGP_1 0x1E7D1110u

//! Register Reset Value
#define DESC0_1_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_EGP_1 Register DESC1_1_CPU_EGP_1 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_EGP_1 0x11114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_1_CPU_EGP_1 0x1E7D1114u

//! Register Reset Value
#define DESC1_1_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_EGP_1 Register DESC2_1_CPU_EGP_1 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_EGP_1 0x11118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_1_CPU_EGP_1 0x1E7D1118u

//! Register Reset Value
#define DESC2_1_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_EGP_1 Register DESC3_1_CPU_EGP_1 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_EGP_1 0x1111C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_1_CPU_EGP_1 0x1E7D111Cu

//! Register Reset Value
#define DESC3_1_CPU_EGP_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_2 Register CFG_CPU_EGP_2 - CPU Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_2 0x12000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_2 0x1E7D2000u

//! Register Reset Value
#define CFG_CPU_EGP_2_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_2_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_2_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_2_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_2_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_2_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_2_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_2_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_2_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_2_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_2_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_2_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_2_BFBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_2_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_2_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_2_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_2_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_2_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_2_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_2 Register DQPC_CPU_EGP_2 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_2 0x12004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_2 0x1E7D2004u

//! Register Reset Value
#define DQPC_CPU_EGP_2_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_2_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_2_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_2 Register IRNCR_CPU_EGP_2 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_2 0x12020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_2 0x1E7D2020u

//! Register Reset Value
#define IRNCR_CPU_EGP_2_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_2_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_2_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_2_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_2_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_2_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_2_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_2_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_2_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_2 Register IRNICR_CPU_EGP_2 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_2 0x12024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_2 0x1E7D2024u

//! Register Reset Value
#define IRNICR_CPU_EGP_2_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_2_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_2_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_2_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_2_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_2 Register IRNEN_CPU_EGP_2 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_2 0x12028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_2 0x1E7D2028u

//! Register Reset Value
#define IRNEN_CPU_EGP_2_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_2_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_2_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_2_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_2_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_2_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_2_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_2_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_2_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_2 Register DPTR_CPU_EGP_2 - CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_2 0x12030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_2 0x1E7D2030u

//! Register Reset Value
#define DPTR_CPU_EGP_2_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_2_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_2_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_2_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_2_DPTR_MASK 0x10000u

//! @}

//! \defgroup BPRC_CPU_EGP_2 Register BPRC_CPU_EGP_2 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_2 0x12034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_2 0x1E7D2034u

//! Register Reset Value
#define BPRC_CPU_EGP_2_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_2_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_2_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PTR_RTN_0_CPU_EGP_2 Register PTR_RTN_0_CPU_EGP_2 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_0_CPU_EGP_2 0x12080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_0_CPU_EGP_2 0x1E7D2080u

//! Register Reset Value
#define PTR_RTN_0_CPU_EGP_2_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_2_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_2_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_EGP_2 Register DESC0_0_CPU_EGP_2 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_EGP_2 0x12100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_0_CPU_EGP_2 0x1E7D2100u

//! Register Reset Value
#define DESC0_0_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_EGP_2 Register DESC1_0_CPU_EGP_2 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_EGP_2 0x12104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_0_CPU_EGP_2 0x1E7D2104u

//! Register Reset Value
#define DESC1_0_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_EGP_2 Register DESC2_0_CPU_EGP_2 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_EGP_2 0x12108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_0_CPU_EGP_2 0x1E7D2108u

//! Register Reset Value
#define DESC2_0_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_EGP_2 Register DESC3_0_CPU_EGP_2 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_EGP_2 0x1210C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_0_CPU_EGP_2 0x1E7D210Cu

//! Register Reset Value
#define DESC3_0_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_EGP_2 Register DESC0_1_CPU_EGP_2 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_EGP_2 0x12110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_1_CPU_EGP_2 0x1E7D2110u

//! Register Reset Value
#define DESC0_1_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_EGP_2 Register DESC1_1_CPU_EGP_2 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_EGP_2 0x12114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_1_CPU_EGP_2 0x1E7D2114u

//! Register Reset Value
#define DESC1_1_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_EGP_2 Register DESC2_1_CPU_EGP_2 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_EGP_2 0x12118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_1_CPU_EGP_2 0x1E7D2118u

//! Register Reset Value
#define DESC2_1_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_EGP_2 Register DESC3_1_CPU_EGP_2 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_EGP_2 0x1211C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_1_CPU_EGP_2 0x1E7D211Cu

//! Register Reset Value
#define DESC3_1_CPU_EGP_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_3 Register CFG_CPU_EGP_3 - CPU Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_3 0x13000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_3 0x1E7D3000u

//! Register Reset Value
#define CFG_CPU_EGP_3_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_3_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_3_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_3_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_3_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_3_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_3_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_3_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_3_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_3_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_3_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_3_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_3_BFBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_3_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_3_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_3_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_3_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_3_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_3_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_3 Register DQPC_CPU_EGP_3 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_3 0x13004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_3 0x1E7D3004u

//! Register Reset Value
#define DQPC_CPU_EGP_3_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_3_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_3_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_3 Register IRNCR_CPU_EGP_3 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_3 0x13020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_3 0x1E7D3020u

//! Register Reset Value
#define IRNCR_CPU_EGP_3_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_3_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_3_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_3_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_3_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_3_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_3_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_3_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_3_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_3 Register IRNICR_CPU_EGP_3 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_3 0x13024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_3 0x1E7D3024u

//! Register Reset Value
#define IRNICR_CPU_EGP_3_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_3_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_3_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_3_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_3_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_3 Register IRNEN_CPU_EGP_3 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_3 0x13028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_3 0x1E7D3028u

//! Register Reset Value
#define IRNEN_CPU_EGP_3_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_3_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_3_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_3_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_3_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_3_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_3_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_3_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_3_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_3 Register DPTR_CPU_EGP_3 - CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_3 0x13030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_3 0x1E7D3030u

//! Register Reset Value
#define DPTR_CPU_EGP_3_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_3_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_3_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_3_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_3_DPTR_MASK 0x10000u

//! @}

//! \defgroup BPRC_CPU_EGP_3 Register BPRC_CPU_EGP_3 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_3 0x13034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_3 0x1E7D3034u

//! Register Reset Value
#define BPRC_CPU_EGP_3_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_3_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_3_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PTR_RTN_0_CPU_EGP_3 Register PTR_RTN_0_CPU_EGP_3 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_0_CPU_EGP_3 0x13080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_0_CPU_EGP_3 0x1E7D3080u

//! Register Reset Value
#define PTR_RTN_0_CPU_EGP_3_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_3_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_3_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_EGP_3 Register DESC0_0_CPU_EGP_3 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_EGP_3 0x13100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_0_CPU_EGP_3 0x1E7D3100u

//! Register Reset Value
#define DESC0_0_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_EGP_3 Register DESC1_0_CPU_EGP_3 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_EGP_3 0x13104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_0_CPU_EGP_3 0x1E7D3104u

//! Register Reset Value
#define DESC1_0_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_EGP_3 Register DESC2_0_CPU_EGP_3 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_EGP_3 0x13108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_0_CPU_EGP_3 0x1E7D3108u

//! Register Reset Value
#define DESC2_0_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_EGP_3 Register DESC3_0_CPU_EGP_3 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_EGP_3 0x1310C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_0_CPU_EGP_3 0x1E7D310Cu

//! Register Reset Value
#define DESC3_0_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_EGP_3 Register DESC0_1_CPU_EGP_3 - CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_EGP_3 0x13110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_1_CPU_EGP_3 0x1E7D3110u

//! Register Reset Value
#define DESC0_1_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_EGP_3 Register DESC1_1_CPU_EGP_3 - CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_EGP_3 0x13114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_1_CPU_EGP_3 0x1E7D3114u

//! Register Reset Value
#define DESC1_1_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_EGP_3 Register DESC2_1_CPU_EGP_3 - CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_EGP_3 0x13118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_1_CPU_EGP_3 0x1E7D3118u

//! Register Reset Value
#define DESC2_1_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_EGP_3 Register DESC3_1_CPU_EGP_3 - CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_EGP_3 0x1311C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_1_CPU_EGP_3 0x1E7D311Cu

//! Register Reset Value
#define DESC3_1_CPU_EGP_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_4 Register CFG_CPU_EGP_4 - CPU WLAN Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_4 0x14000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_4 0x1E7D4000u

//! Register Reset Value
#define CFG_CPU_EGP_4_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_4_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_4_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_4_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_4_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_4_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_4_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_4_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_4_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_4_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_4_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_4_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_4_BFBPEN_EN 0x1

//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_4_DQBPEN_POS 3
//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_4_DQBPEN_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_4_DQBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_4_DQBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_4_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_4_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_4_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_4_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_4_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_4_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_4 Register DQPC_CPU_EGP_4 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_4 0x14004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_4 0x1E7D4004u

//! Register Reset Value
#define DQPC_CPU_EGP_4_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_4_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_4_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_4 Register IRNCR_CPU_EGP_4 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_4 0x14020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_4 0x1E7D4020u

//! Register Reset Value
#define IRNCR_CPU_EGP_4_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_4_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_4_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_4_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_4_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_4_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_4_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_4_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_4_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_4 Register IRNICR_CPU_EGP_4 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_4 0x14024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_4 0x1E7D4024u

//! Register Reset Value
#define IRNICR_CPU_EGP_4_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_4_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_4_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_4_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_4_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_4 Register IRNEN_CPU_EGP_4 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_4 0x14028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_4 0x1E7D4028u

//! Register Reset Value
#define IRNEN_CPU_EGP_4_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_4_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_4_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_4_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_4_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_4_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_4_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_4_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_4_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_4 Register DPTR_CPU_EGP_4 - Special CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_4 0x14030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_4 0x1E7D4030u

//! Register Reset Value
#define DPTR_CPU_EGP_4_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_4_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_4_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_4_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_4_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup BPRC_CPU_EGP_4 Register BPRC_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_4 0x14034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_4 0x1E7D4034u

//! Register Reset Value
#define BPRC_CPU_EGP_4_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_4_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_4_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BRPTR_SCPU_EGP_4 Register BRPTR_SCPU_EGP_4 - Special CPU Egress Port Buffer Return Pointer
//! @{

//! Register Offset (relative)
#define BRPTR_SCPU_EGP_4 0x14038
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BRPTR_SCPU_EGP_4 0x1E7D4038u

//! Register Reset Value
#define BRPTR_SCPU_EGP_4_RST 0x00000000u

//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_4_PTRBR_POS 0
//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_4_PTRBR_MASK 0x1Fu

//! @}

//! \defgroup PTR_RTN_0_CPU_EGP_4 Register PTR_RTN_0_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_0_CPU_EGP_4 0x14080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_0_CPU_EGP_4 0x1E7D4080u

//! Register Reset Value
#define PTR_RTN_0_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_0_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_1_CPU_EGP_4 Register PTR_RTN_1_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_1_CPU_EGP_4 0x14084
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_1_CPU_EGP_4 0x1E7D4084u

//! Register Reset Value
#define PTR_RTN_1_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_1_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_1_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_2_CPU_EGP_4 Register PTR_RTN_2_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_2_CPU_EGP_4 0x14088
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_2_CPU_EGP_4 0x1E7D4088u

//! Register Reset Value
#define PTR_RTN_2_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_2_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_2_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_3_CPU_EGP_4 Register PTR_RTN_3_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_3_CPU_EGP_4 0x1408C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_3_CPU_EGP_4 0x1E7D408Cu

//! Register Reset Value
#define PTR_RTN_3_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_3_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_3_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_4_CPU_EGP_4 Register PTR_RTN_4_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_4_CPU_EGP_4 0x14090
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_4_CPU_EGP_4 0x1E7D4090u

//! Register Reset Value
#define PTR_RTN_4_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_4_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_4_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_5_CPU_EGP_4 Register PTR_RTN_5_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_5_CPU_EGP_4 0x14094
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_5_CPU_EGP_4 0x1E7D4094u

//! Register Reset Value
#define PTR_RTN_5_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_5_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_5_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_6_CPU_EGP_4 Register PTR_RTN_6_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_6_CPU_EGP_4 0x14098
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_6_CPU_EGP_4 0x1E7D4098u

//! Register Reset Value
#define PTR_RTN_6_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_6_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_6_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_7_CPU_EGP_4 Register PTR_RTN_7_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_7_CPU_EGP_4 0x1409C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_7_CPU_EGP_4 0x1E7D409Cu

//! Register Reset Value
#define PTR_RTN_7_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_7_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_7_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_8_CPU_EGP_4 Register PTR_RTN_8_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_8_CPU_EGP_4 0x140A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_8_CPU_EGP_4 0x1E7D40A0u

//! Register Reset Value
#define PTR_RTN_8_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_8_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_8_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_9_CPU_EGP_4 Register PTR_RTN_9_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_9_CPU_EGP_4 0x140A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_9_CPU_EGP_4 0x1E7D40A4u

//! Register Reset Value
#define PTR_RTN_9_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_9_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_9_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_10_CPU_EGP_4 Register PTR_RTN_10_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_10_CPU_EGP_4 0x140A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_10_CPU_EGP_4 0x1E7D40A8u

//! Register Reset Value
#define PTR_RTN_10_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_10_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_10_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_11_CPU_EGP_4 Register PTR_RTN_11_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_11_CPU_EGP_4 0x140AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_11_CPU_EGP_4 0x1E7D40ACu

//! Register Reset Value
#define PTR_RTN_11_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_11_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_11_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_12_CPU_EGP_4 Register PTR_RTN_12_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_12_CPU_EGP_4 0x140B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_12_CPU_EGP_4 0x1E7D40B0u

//! Register Reset Value
#define PTR_RTN_12_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_12_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_12_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_13_CPU_EGP_4 Register PTR_RTN_13_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_13_CPU_EGP_4 0x140B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_13_CPU_EGP_4 0x1E7D40B4u

//! Register Reset Value
#define PTR_RTN_13_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_13_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_13_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_14_CPU_EGP_4 Register PTR_RTN_14_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_14_CPU_EGP_4 0x140B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_14_CPU_EGP_4 0x1E7D40B8u

//! Register Reset Value
#define PTR_RTN_14_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_14_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_14_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_15_CPU_EGP_4 Register PTR_RTN_15_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_15_CPU_EGP_4 0x140BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_15_CPU_EGP_4 0x1E7D40BCu

//! Register Reset Value
#define PTR_RTN_15_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_15_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_15_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_16_CPU_EGP_4 Register PTR_RTN_16_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_16_CPU_EGP_4 0x140C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_16_CPU_EGP_4 0x1E7D40C0u

//! Register Reset Value
#define PTR_RTN_16_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_16_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_16_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_17_CPU_EGP_4 Register PTR_RTN_17_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_17_CPU_EGP_4 0x140C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_17_CPU_EGP_4 0x1E7D40C4u

//! Register Reset Value
#define PTR_RTN_17_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_17_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_17_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_18_CPU_EGP_4 Register PTR_RTN_18_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_18_CPU_EGP_4 0x140C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_18_CPU_EGP_4 0x1E7D40C8u

//! Register Reset Value
#define PTR_RTN_18_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_18_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_18_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_19_CPU_EGP_4 Register PTR_RTN_19_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_19_CPU_EGP_4 0x140CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_19_CPU_EGP_4 0x1E7D40CCu

//! Register Reset Value
#define PTR_RTN_19_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_19_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_19_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_20_CPU_EGP_4 Register PTR_RTN_20_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_20_CPU_EGP_4 0x140D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_20_CPU_EGP_4 0x1E7D40D0u

//! Register Reset Value
#define PTR_RTN_20_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_20_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_20_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_21_CPU_EGP_4 Register PTR_RTN_21_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_21_CPU_EGP_4 0x140D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_21_CPU_EGP_4 0x1E7D40D4u

//! Register Reset Value
#define PTR_RTN_21_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_21_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_21_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_22_CPU_EGP_4 Register PTR_RTN_22_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_22_CPU_EGP_4 0x140D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_22_CPU_EGP_4 0x1E7D40D8u

//! Register Reset Value
#define PTR_RTN_22_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_22_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_22_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_23_CPU_EGP_4 Register PTR_RTN_23_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_23_CPU_EGP_4 0x140DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_23_CPU_EGP_4 0x1E7D40DCu

//! Register Reset Value
#define PTR_RTN_23_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_23_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_23_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_24_CPU_EGP_4 Register PTR_RTN_24_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_24_CPU_EGP_4 0x140E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_24_CPU_EGP_4 0x1E7D40E0u

//! Register Reset Value
#define PTR_RTN_24_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_24_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_24_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_25_CPU_EGP_4 Register PTR_RTN_25_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_25_CPU_EGP_4 0x140E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_25_CPU_EGP_4 0x1E7D40E4u

//! Register Reset Value
#define PTR_RTN_25_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_25_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_25_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_26_CPU_EGP_4 Register PTR_RTN_26_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_26_CPU_EGP_4 0x140E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_26_CPU_EGP_4 0x1E7D40E8u

//! Register Reset Value
#define PTR_RTN_26_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_26_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_26_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_27_CPU_EGP_4 Register PTR_RTN_27_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_27_CPU_EGP_4 0x140EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_27_CPU_EGP_4 0x1E7D40ECu

//! Register Reset Value
#define PTR_RTN_27_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_27_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_27_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_28_CPU_EGP_4 Register PTR_RTN_28_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_28_CPU_EGP_4 0x140F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_28_CPU_EGP_4 0x1E7D40F0u

//! Register Reset Value
#define PTR_RTN_28_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_28_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_28_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_29_CPU_EGP_4 Register PTR_RTN_29_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_29_CPU_EGP_4 0x140F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_29_CPU_EGP_4 0x1E7D40F4u

//! Register Reset Value
#define PTR_RTN_29_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_29_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_29_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_30_CPU_EGP_4 Register PTR_RTN_30_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_30_CPU_EGP_4 0x140F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_30_CPU_EGP_4 0x1E7D40F8u

//! Register Reset Value
#define PTR_RTN_30_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_30_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_30_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_31_CPU_EGP_4 Register PTR_RTN_31_CPU_EGP_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_31_CPU_EGP_4 0x140FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_31_CPU_EGP_4 0x1E7D40FCu

//! Register Reset Value
#define PTR_RTN_31_CPU_EGP_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_31_CPU_EGP_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_31_CPU_EGP_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_0_CPU_EGP_4 Register DESC0_0_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_0_CPU_EGP_4 0x14100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_0_CPU_EGP_4 0x1E7D4100u

//! Register Reset Value
#define DESC0_0_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_0_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_0_CPU_EGP_4 Register DESC1_0_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_0_CPU_EGP_4 0x14104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_0_CPU_EGP_4 0x1E7D4104u

//! Register Reset Value
#define DESC1_0_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_0_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_0_CPU_EGP_4 Register DESC2_0_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_0_CPU_EGP_4 0x14108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_0_CPU_EGP_4 0x1E7D4108u

//! Register Reset Value
#define DESC2_0_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_0_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_0_CPU_EGP_4 Register DESC3_0_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_0_CPU_EGP_4 0x1410C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_0_CPU_EGP_4 0x1E7D410Cu

//! Register Reset Value
#define DESC3_0_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_0_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_1_CPU_EGP_4 Register DESC0_1_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_1_CPU_EGP_4 0x14110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_1_CPU_EGP_4 0x1E7D4110u

//! Register Reset Value
#define DESC0_1_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_1_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_1_CPU_EGP_4 Register DESC1_1_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_1_CPU_EGP_4 0x14114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_1_CPU_EGP_4 0x1E7D4114u

//! Register Reset Value
#define DESC1_1_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_1_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_1_CPU_EGP_4 Register DESC2_1_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_1_CPU_EGP_4 0x14118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_1_CPU_EGP_4 0x1E7D4118u

//! Register Reset Value
#define DESC2_1_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_1_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_1_CPU_EGP_4 Register DESC3_1_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_1_CPU_EGP_4 0x1411C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_1_CPU_EGP_4 0x1E7D411Cu

//! Register Reset Value
#define DESC3_1_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_1_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_2_CPU_EGP_4 Register DESC0_2_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_2_CPU_EGP_4 0x14120
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_2_CPU_EGP_4 0x1E7D4120u

//! Register Reset Value
#define DESC0_2_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_2_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_2_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_2_CPU_EGP_4 Register DESC1_2_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_2_CPU_EGP_4 0x14124
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_2_CPU_EGP_4 0x1E7D4124u

//! Register Reset Value
#define DESC1_2_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_2_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_2_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_2_CPU_EGP_4 Register DESC2_2_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_2_CPU_EGP_4 0x14128
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_2_CPU_EGP_4 0x1E7D4128u

//! Register Reset Value
#define DESC2_2_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_2_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_2_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_2_CPU_EGP_4 Register DESC3_2_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_2_CPU_EGP_4 0x1412C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_2_CPU_EGP_4 0x1E7D412Cu

//! Register Reset Value
#define DESC3_2_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_2_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_2_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_3_CPU_EGP_4 Register DESC0_3_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_3_CPU_EGP_4 0x14130
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_3_CPU_EGP_4 0x1E7D4130u

//! Register Reset Value
#define DESC0_3_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_3_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_3_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_3_CPU_EGP_4 Register DESC1_3_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_3_CPU_EGP_4 0x14134
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_3_CPU_EGP_4 0x1E7D4134u

//! Register Reset Value
#define DESC1_3_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_3_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_3_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_3_CPU_EGP_4 Register DESC2_3_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_3_CPU_EGP_4 0x14138
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_3_CPU_EGP_4 0x1E7D4138u

//! Register Reset Value
#define DESC2_3_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_3_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_3_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_3_CPU_EGP_4 Register DESC3_3_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_3_CPU_EGP_4 0x1413C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_3_CPU_EGP_4 0x1E7D413Cu

//! Register Reset Value
#define DESC3_3_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_3_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_3_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_4_CPU_EGP_4 Register DESC0_4_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_4_CPU_EGP_4 0x14140
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_4_CPU_EGP_4 0x1E7D4140u

//! Register Reset Value
#define DESC0_4_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_4_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_4_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_4_CPU_EGP_4 Register DESC1_4_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_4_CPU_EGP_4 0x14144
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_4_CPU_EGP_4 0x1E7D4144u

//! Register Reset Value
#define DESC1_4_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_4_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_4_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_4_CPU_EGP_4 Register DESC2_4_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_4_CPU_EGP_4 0x14148
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_4_CPU_EGP_4 0x1E7D4148u

//! Register Reset Value
#define DESC2_4_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_4_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_4_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_4_CPU_EGP_4 Register DESC3_4_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_4_CPU_EGP_4 0x1414C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_4_CPU_EGP_4 0x1E7D414Cu

//! Register Reset Value
#define DESC3_4_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_4_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_4_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_5_CPU_EGP_4 Register DESC0_5_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_5_CPU_EGP_4 0x14150
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_5_CPU_EGP_4 0x1E7D4150u

//! Register Reset Value
#define DESC0_5_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_5_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_5_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_5_CPU_EGP_4 Register DESC1_5_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_5_CPU_EGP_4 0x14154
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_5_CPU_EGP_4 0x1E7D4154u

//! Register Reset Value
#define DESC1_5_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_5_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_5_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_5_CPU_EGP_4 Register DESC2_5_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_5_CPU_EGP_4 0x14158
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_5_CPU_EGP_4 0x1E7D4158u

//! Register Reset Value
#define DESC2_5_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_5_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_5_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_5_CPU_EGP_4 Register DESC3_5_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_5_CPU_EGP_4 0x1415C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_5_CPU_EGP_4 0x1E7D415Cu

//! Register Reset Value
#define DESC3_5_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_5_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_5_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_6_CPU_EGP_4 Register DESC0_6_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_6_CPU_EGP_4 0x14160
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_6_CPU_EGP_4 0x1E7D4160u

//! Register Reset Value
#define DESC0_6_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_6_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_6_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_6_CPU_EGP_4 Register DESC1_6_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_6_CPU_EGP_4 0x14164
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_6_CPU_EGP_4 0x1E7D4164u

//! Register Reset Value
#define DESC1_6_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_6_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_6_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_6_CPU_EGP_4 Register DESC2_6_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_6_CPU_EGP_4 0x14168
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_6_CPU_EGP_4 0x1E7D4168u

//! Register Reset Value
#define DESC2_6_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_6_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_6_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_6_CPU_EGP_4 Register DESC3_6_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_6_CPU_EGP_4 0x1416C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_6_CPU_EGP_4 0x1E7D416Cu

//! Register Reset Value
#define DESC3_6_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_6_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_6_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_7_CPU_EGP_4 Register DESC0_7_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_7_CPU_EGP_4 0x14170
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_7_CPU_EGP_4 0x1E7D4170u

//! Register Reset Value
#define DESC0_7_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_7_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_7_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_7_CPU_EGP_4 Register DESC1_7_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_7_CPU_EGP_4 0x14174
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_7_CPU_EGP_4 0x1E7D4174u

//! Register Reset Value
#define DESC1_7_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_7_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_7_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_7_CPU_EGP_4 Register DESC2_7_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_7_CPU_EGP_4 0x14178
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_7_CPU_EGP_4 0x1E7D4178u

//! Register Reset Value
#define DESC2_7_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_7_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_7_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_7_CPU_EGP_4 Register DESC3_7_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_7_CPU_EGP_4 0x1417C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_7_CPU_EGP_4 0x1E7D417Cu

//! Register Reset Value
#define DESC3_7_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_7_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_7_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_8_CPU_EGP_4 Register DESC0_8_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_8_CPU_EGP_4 0x14180
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_8_CPU_EGP_4 0x1E7D4180u

//! Register Reset Value
#define DESC0_8_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_8_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_8_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_8_CPU_EGP_4 Register DESC1_8_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_8_CPU_EGP_4 0x14184
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_8_CPU_EGP_4 0x1E7D4184u

//! Register Reset Value
#define DESC1_8_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_8_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_8_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_8_CPU_EGP_4 Register DESC2_8_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_8_CPU_EGP_4 0x14188
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_8_CPU_EGP_4 0x1E7D4188u

//! Register Reset Value
#define DESC2_8_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_8_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_8_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_8_CPU_EGP_4 Register DESC3_8_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_8_CPU_EGP_4 0x1418C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_8_CPU_EGP_4 0x1E7D418Cu

//! Register Reset Value
#define DESC3_8_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_8_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_8_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_9_CPU_EGP_4 Register DESC0_9_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_9_CPU_EGP_4 0x14190
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_9_CPU_EGP_4 0x1E7D4190u

//! Register Reset Value
#define DESC0_9_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_9_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_9_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_9_CPU_EGP_4 Register DESC1_9_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_9_CPU_EGP_4 0x14194
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_9_CPU_EGP_4 0x1E7D4194u

//! Register Reset Value
#define DESC1_9_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_9_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_9_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_9_CPU_EGP_4 Register DESC2_9_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_9_CPU_EGP_4 0x14198
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_9_CPU_EGP_4 0x1E7D4198u

//! Register Reset Value
#define DESC2_9_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_9_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_9_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_9_CPU_EGP_4 Register DESC3_9_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_9_CPU_EGP_4 0x1419C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_9_CPU_EGP_4 0x1E7D419Cu

//! Register Reset Value
#define DESC3_9_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_9_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_9_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_10_CPU_EGP_4 Register DESC0_10_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_10_CPU_EGP_4 0x141A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_10_CPU_EGP_4 0x1E7D41A0u

//! Register Reset Value
#define DESC0_10_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_10_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_10_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_10_CPU_EGP_4 Register DESC1_10_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_10_CPU_EGP_4 0x141A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_10_CPU_EGP_4 0x1E7D41A4u

//! Register Reset Value
#define DESC1_10_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_10_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_10_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_10_CPU_EGP_4 Register DESC2_10_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_10_CPU_EGP_4 0x141A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_10_CPU_EGP_4 0x1E7D41A8u

//! Register Reset Value
#define DESC2_10_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_10_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_10_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_10_CPU_EGP_4 Register DESC3_10_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_10_CPU_EGP_4 0x141AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_10_CPU_EGP_4 0x1E7D41ACu

//! Register Reset Value
#define DESC3_10_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_10_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_10_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_11_CPU_EGP_4 Register DESC0_11_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_11_CPU_EGP_4 0x141B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_11_CPU_EGP_4 0x1E7D41B0u

//! Register Reset Value
#define DESC0_11_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_11_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_11_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_11_CPU_EGP_4 Register DESC1_11_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_11_CPU_EGP_4 0x141B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_11_CPU_EGP_4 0x1E7D41B4u

//! Register Reset Value
#define DESC1_11_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_11_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_11_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_11_CPU_EGP_4 Register DESC2_11_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_11_CPU_EGP_4 0x141B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_11_CPU_EGP_4 0x1E7D41B8u

//! Register Reset Value
#define DESC2_11_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_11_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_11_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_11_CPU_EGP_4 Register DESC3_11_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_11_CPU_EGP_4 0x141BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_11_CPU_EGP_4 0x1E7D41BCu

//! Register Reset Value
#define DESC3_11_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_11_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_11_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_12_CPU_EGP_4 Register DESC0_12_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_12_CPU_EGP_4 0x141C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_12_CPU_EGP_4 0x1E7D41C0u

//! Register Reset Value
#define DESC0_12_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_12_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_12_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_12_CPU_EGP_4 Register DESC1_12_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_12_CPU_EGP_4 0x141C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_12_CPU_EGP_4 0x1E7D41C4u

//! Register Reset Value
#define DESC1_12_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_12_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_12_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_12_CPU_EGP_4 Register DESC2_12_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_12_CPU_EGP_4 0x141C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_12_CPU_EGP_4 0x1E7D41C8u

//! Register Reset Value
#define DESC2_12_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_12_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_12_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_12_CPU_EGP_4 Register DESC3_12_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_12_CPU_EGP_4 0x141CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_12_CPU_EGP_4 0x1E7D41CCu

//! Register Reset Value
#define DESC3_12_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_12_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_12_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_13_CPU_EGP_4 Register DESC0_13_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_13_CPU_EGP_4 0x141D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_13_CPU_EGP_4 0x1E7D41D0u

//! Register Reset Value
#define DESC0_13_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_13_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_13_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_13_CPU_EGP_4 Register DESC1_13_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_13_CPU_EGP_4 0x141D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_13_CPU_EGP_4 0x1E7D41D4u

//! Register Reset Value
#define DESC1_13_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_13_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_13_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_13_CPU_EGP_4 Register DESC2_13_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_13_CPU_EGP_4 0x141D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_13_CPU_EGP_4 0x1E7D41D8u

//! Register Reset Value
#define DESC2_13_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_13_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_13_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_13_CPU_EGP_4 Register DESC3_13_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_13_CPU_EGP_4 0x141DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_13_CPU_EGP_4 0x1E7D41DCu

//! Register Reset Value
#define DESC3_13_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_13_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_13_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_14_CPU_EGP_4 Register DESC0_14_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_14_CPU_EGP_4 0x141E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_14_CPU_EGP_4 0x1E7D41E0u

//! Register Reset Value
#define DESC0_14_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_14_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_14_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_14_CPU_EGP_4 Register DESC1_14_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_14_CPU_EGP_4 0x141E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_14_CPU_EGP_4 0x1E7D41E4u

//! Register Reset Value
#define DESC1_14_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_14_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_14_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_14_CPU_EGP_4 Register DESC2_14_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_14_CPU_EGP_4 0x141E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_14_CPU_EGP_4 0x1E7D41E8u

//! Register Reset Value
#define DESC2_14_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_14_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_14_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_14_CPU_EGP_4 Register DESC3_14_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_14_CPU_EGP_4 0x141EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_14_CPU_EGP_4 0x1E7D41ECu

//! Register Reset Value
#define DESC3_14_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_14_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_14_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_15_CPU_EGP_4 Register DESC0_15_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_15_CPU_EGP_4 0x141F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_15_CPU_EGP_4 0x1E7D41F0u

//! Register Reset Value
#define DESC0_15_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_15_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_15_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_15_CPU_EGP_4 Register DESC1_15_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_15_CPU_EGP_4 0x141F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_15_CPU_EGP_4 0x1E7D41F4u

//! Register Reset Value
#define DESC1_15_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_15_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_15_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_15_CPU_EGP_4 Register DESC2_15_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_15_CPU_EGP_4 0x141F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_15_CPU_EGP_4 0x1E7D41F8u

//! Register Reset Value
#define DESC2_15_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_15_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_15_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_15_CPU_EGP_4 Register DESC3_15_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_15_CPU_EGP_4 0x141FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_15_CPU_EGP_4 0x1E7D41FCu

//! Register Reset Value
#define DESC3_15_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_15_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_15_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_16_CPU_EGP_4 Register DESC0_16_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_16_CPU_EGP_4 0x14200
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_16_CPU_EGP_4 0x1E7D4200u

//! Register Reset Value
#define DESC0_16_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_16_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_16_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_16_CPU_EGP_4 Register DESC1_16_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_16_CPU_EGP_4 0x14204
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_16_CPU_EGP_4 0x1E7D4204u

//! Register Reset Value
#define DESC1_16_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_16_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_16_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_16_CPU_EGP_4 Register DESC2_16_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_16_CPU_EGP_4 0x14208
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_16_CPU_EGP_4 0x1E7D4208u

//! Register Reset Value
#define DESC2_16_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_16_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_16_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_16_CPU_EGP_4 Register DESC3_16_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_16_CPU_EGP_4 0x1420C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_16_CPU_EGP_4 0x1E7D420Cu

//! Register Reset Value
#define DESC3_16_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_16_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_16_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_17_CPU_EGP_4 Register DESC0_17_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_17_CPU_EGP_4 0x14210
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_17_CPU_EGP_4 0x1E7D4210u

//! Register Reset Value
#define DESC0_17_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_17_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_17_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_17_CPU_EGP_4 Register DESC1_17_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_17_CPU_EGP_4 0x14214
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_17_CPU_EGP_4 0x1E7D4214u

//! Register Reset Value
#define DESC1_17_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_17_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_17_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_17_CPU_EGP_4 Register DESC2_17_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_17_CPU_EGP_4 0x14218
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_17_CPU_EGP_4 0x1E7D4218u

//! Register Reset Value
#define DESC2_17_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_17_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_17_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_17_CPU_EGP_4 Register DESC3_17_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_17_CPU_EGP_4 0x1421C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_17_CPU_EGP_4 0x1E7D421Cu

//! Register Reset Value
#define DESC3_17_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_17_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_17_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_18_CPU_EGP_4 Register DESC0_18_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_18_CPU_EGP_4 0x14220
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_18_CPU_EGP_4 0x1E7D4220u

//! Register Reset Value
#define DESC0_18_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_18_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_18_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_18_CPU_EGP_4 Register DESC1_18_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_18_CPU_EGP_4 0x14224
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_18_CPU_EGP_4 0x1E7D4224u

//! Register Reset Value
#define DESC1_18_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_18_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_18_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_18_CPU_EGP_4 Register DESC2_18_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_18_CPU_EGP_4 0x14228
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_18_CPU_EGP_4 0x1E7D4228u

//! Register Reset Value
#define DESC2_18_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_18_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_18_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_18_CPU_EGP_4 Register DESC3_18_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_18_CPU_EGP_4 0x1422C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_18_CPU_EGP_4 0x1E7D422Cu

//! Register Reset Value
#define DESC3_18_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_18_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_18_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_19_CPU_EGP_4 Register DESC0_19_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_19_CPU_EGP_4 0x14230
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_19_CPU_EGP_4 0x1E7D4230u

//! Register Reset Value
#define DESC0_19_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_19_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_19_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_19_CPU_EGP_4 Register DESC1_19_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_19_CPU_EGP_4 0x14234
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_19_CPU_EGP_4 0x1E7D4234u

//! Register Reset Value
#define DESC1_19_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_19_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_19_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_19_CPU_EGP_4 Register DESC2_19_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_19_CPU_EGP_4 0x14238
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_19_CPU_EGP_4 0x1E7D4238u

//! Register Reset Value
#define DESC2_19_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_19_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_19_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_19_CPU_EGP_4 Register DESC3_19_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_19_CPU_EGP_4 0x1423C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_19_CPU_EGP_4 0x1E7D423Cu

//! Register Reset Value
#define DESC3_19_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_19_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_19_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_20_CPU_EGP_4 Register DESC0_20_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_20_CPU_EGP_4 0x14240
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_20_CPU_EGP_4 0x1E7D4240u

//! Register Reset Value
#define DESC0_20_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_20_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_20_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_20_CPU_EGP_4 Register DESC1_20_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_20_CPU_EGP_4 0x14244
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_20_CPU_EGP_4 0x1E7D4244u

//! Register Reset Value
#define DESC1_20_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_20_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_20_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_20_CPU_EGP_4 Register DESC2_20_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_20_CPU_EGP_4 0x14248
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_20_CPU_EGP_4 0x1E7D4248u

//! Register Reset Value
#define DESC2_20_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_20_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_20_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_20_CPU_EGP_4 Register DESC3_20_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_20_CPU_EGP_4 0x1424C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_20_CPU_EGP_4 0x1E7D424Cu

//! Register Reset Value
#define DESC3_20_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_20_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_20_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_21_CPU_EGP_4 Register DESC0_21_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_21_CPU_EGP_4 0x14250
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_21_CPU_EGP_4 0x1E7D4250u

//! Register Reset Value
#define DESC0_21_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_21_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_21_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_21_CPU_EGP_4 Register DESC1_21_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_21_CPU_EGP_4 0x14254
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_21_CPU_EGP_4 0x1E7D4254u

//! Register Reset Value
#define DESC1_21_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_21_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_21_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_21_CPU_EGP_4 Register DESC2_21_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_21_CPU_EGP_4 0x14258
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_21_CPU_EGP_4 0x1E7D4258u

//! Register Reset Value
#define DESC2_21_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_21_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_21_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_21_CPU_EGP_4 Register DESC3_21_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_21_CPU_EGP_4 0x1425C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_21_CPU_EGP_4 0x1E7D425Cu

//! Register Reset Value
#define DESC3_21_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_21_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_21_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_22_CPU_EGP_4 Register DESC0_22_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_22_CPU_EGP_4 0x14260
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_22_CPU_EGP_4 0x1E7D4260u

//! Register Reset Value
#define DESC0_22_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_22_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_22_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_22_CPU_EGP_4 Register DESC1_22_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_22_CPU_EGP_4 0x14264
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_22_CPU_EGP_4 0x1E7D4264u

//! Register Reset Value
#define DESC1_22_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_22_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_22_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_22_CPU_EGP_4 Register DESC2_22_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_22_CPU_EGP_4 0x14268
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_22_CPU_EGP_4 0x1E7D4268u

//! Register Reset Value
#define DESC2_22_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_22_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_22_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_22_CPU_EGP_4 Register DESC3_22_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_22_CPU_EGP_4 0x1426C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_22_CPU_EGP_4 0x1E7D426Cu

//! Register Reset Value
#define DESC3_22_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_22_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_22_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_23_CPU_EGP_4 Register DESC0_23_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_23_CPU_EGP_4 0x14270
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_23_CPU_EGP_4 0x1E7D4270u

//! Register Reset Value
#define DESC0_23_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_23_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_23_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_23_CPU_EGP_4 Register DESC1_23_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_23_CPU_EGP_4 0x14274
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_23_CPU_EGP_4 0x1E7D4274u

//! Register Reset Value
#define DESC1_23_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_23_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_23_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_23_CPU_EGP_4 Register DESC2_23_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_23_CPU_EGP_4 0x14278
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_23_CPU_EGP_4 0x1E7D4278u

//! Register Reset Value
#define DESC2_23_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_23_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_23_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_23_CPU_EGP_4 Register DESC3_23_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_23_CPU_EGP_4 0x1427C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_23_CPU_EGP_4 0x1E7D427Cu

//! Register Reset Value
#define DESC3_23_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_23_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_23_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_24_CPU_EGP_4 Register DESC0_24_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_24_CPU_EGP_4 0x14280
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_24_CPU_EGP_4 0x1E7D4280u

//! Register Reset Value
#define DESC0_24_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_24_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_24_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_24_CPU_EGP_4 Register DESC1_24_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_24_CPU_EGP_4 0x14284
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_24_CPU_EGP_4 0x1E7D4284u

//! Register Reset Value
#define DESC1_24_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_24_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_24_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_24_CPU_EGP_4 Register DESC2_24_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_24_CPU_EGP_4 0x14288
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_24_CPU_EGP_4 0x1E7D4288u

//! Register Reset Value
#define DESC2_24_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_24_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_24_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_24_CPU_EGP_4 Register DESC3_24_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_24_CPU_EGP_4 0x1428C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_24_CPU_EGP_4 0x1E7D428Cu

//! Register Reset Value
#define DESC3_24_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_24_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_24_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_25_CPU_EGP_4 Register DESC0_25_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_25_CPU_EGP_4 0x14290
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_25_CPU_EGP_4 0x1E7D4290u

//! Register Reset Value
#define DESC0_25_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_25_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_25_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_25_CPU_EGP_4 Register DESC1_25_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_25_CPU_EGP_4 0x14294
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_25_CPU_EGP_4 0x1E7D4294u

//! Register Reset Value
#define DESC1_25_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_25_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_25_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_25_CPU_EGP_4 Register DESC2_25_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_25_CPU_EGP_4 0x14298
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_25_CPU_EGP_4 0x1E7D4298u

//! Register Reset Value
#define DESC2_25_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_25_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_25_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_25_CPU_EGP_4 Register DESC3_25_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_25_CPU_EGP_4 0x1429C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_25_CPU_EGP_4 0x1E7D429Cu

//! Register Reset Value
#define DESC3_25_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_25_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_25_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_26_CPU_EGP_4 Register DESC0_26_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_26_CPU_EGP_4 0x142A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_26_CPU_EGP_4 0x1E7D42A0u

//! Register Reset Value
#define DESC0_26_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_26_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_26_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_26_CPU_EGP_4 Register DESC1_26_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_26_CPU_EGP_4 0x142A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_26_CPU_EGP_4 0x1E7D42A4u

//! Register Reset Value
#define DESC1_26_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_26_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_26_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_26_CPU_EGP_4 Register DESC2_26_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_26_CPU_EGP_4 0x142A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_26_CPU_EGP_4 0x1E7D42A8u

//! Register Reset Value
#define DESC2_26_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_26_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_26_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_26_CPU_EGP_4 Register DESC3_26_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_26_CPU_EGP_4 0x142AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_26_CPU_EGP_4 0x1E7D42ACu

//! Register Reset Value
#define DESC3_26_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_26_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_26_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_27_CPU_EGP_4 Register DESC0_27_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_27_CPU_EGP_4 0x142B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_27_CPU_EGP_4 0x1E7D42B0u

//! Register Reset Value
#define DESC0_27_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_27_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_27_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_27_CPU_EGP_4 Register DESC1_27_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_27_CPU_EGP_4 0x142B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_27_CPU_EGP_4 0x1E7D42B4u

//! Register Reset Value
#define DESC1_27_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_27_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_27_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_27_CPU_EGP_4 Register DESC2_27_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_27_CPU_EGP_4 0x142B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_27_CPU_EGP_4 0x1E7D42B8u

//! Register Reset Value
#define DESC2_27_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_27_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_27_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_27_CPU_EGP_4 Register DESC3_27_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_27_CPU_EGP_4 0x142BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_27_CPU_EGP_4 0x1E7D42BCu

//! Register Reset Value
#define DESC3_27_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_27_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_27_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_28_CPU_EGP_4 Register DESC0_28_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_28_CPU_EGP_4 0x142C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_28_CPU_EGP_4 0x1E7D42C0u

//! Register Reset Value
#define DESC0_28_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_28_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_28_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_28_CPU_EGP_4 Register DESC1_28_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_28_CPU_EGP_4 0x142C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_28_CPU_EGP_4 0x1E7D42C4u

//! Register Reset Value
#define DESC1_28_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_28_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_28_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_28_CPU_EGP_4 Register DESC2_28_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_28_CPU_EGP_4 0x142C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_28_CPU_EGP_4 0x1E7D42C8u

//! Register Reset Value
#define DESC2_28_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_28_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_28_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_28_CPU_EGP_4 Register DESC3_28_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_28_CPU_EGP_4 0x142CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_28_CPU_EGP_4 0x1E7D42CCu

//! Register Reset Value
#define DESC3_28_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_28_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_28_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_29_CPU_EGP_4 Register DESC0_29_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_29_CPU_EGP_4 0x142D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_29_CPU_EGP_4 0x1E7D42D0u

//! Register Reset Value
#define DESC0_29_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_29_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_29_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_29_CPU_EGP_4 Register DESC1_29_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_29_CPU_EGP_4 0x142D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_29_CPU_EGP_4 0x1E7D42D4u

//! Register Reset Value
#define DESC1_29_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_29_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_29_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_29_CPU_EGP_4 Register DESC2_29_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_29_CPU_EGP_4 0x142D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_29_CPU_EGP_4 0x1E7D42D8u

//! Register Reset Value
#define DESC2_29_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_29_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_29_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_29_CPU_EGP_4 Register DESC3_29_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_29_CPU_EGP_4 0x142DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_29_CPU_EGP_4 0x1E7D42DCu

//! Register Reset Value
#define DESC3_29_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_29_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_29_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_30_CPU_EGP_4 Register DESC0_30_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_30_CPU_EGP_4 0x142E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_30_CPU_EGP_4 0x1E7D42E0u

//! Register Reset Value
#define DESC0_30_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_30_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_30_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_30_CPU_EGP_4 Register DESC1_30_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_30_CPU_EGP_4 0x142E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_30_CPU_EGP_4 0x1E7D42E4u

//! Register Reset Value
#define DESC1_30_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_30_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_30_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_30_CPU_EGP_4 Register DESC2_30_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_30_CPU_EGP_4 0x142E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_30_CPU_EGP_4 0x1E7D42E8u

//! Register Reset Value
#define DESC2_30_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_30_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_30_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_30_CPU_EGP_4 Register DESC3_30_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_30_CPU_EGP_4 0x142EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_30_CPU_EGP_4 0x1E7D42ECu

//! Register Reset Value
#define DESC3_30_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_30_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_30_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_31_CPU_EGP_4 Register DESC0_31_CPU_EGP_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_31_CPU_EGP_4 0x142F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_31_CPU_EGP_4 0x1E7D42F0u

//! Register Reset Value
#define DESC0_31_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_31_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_31_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_31_CPU_EGP_4 Register DESC1_31_CPU_EGP_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_31_CPU_EGP_4 0x142F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_31_CPU_EGP_4 0x1E7D42F4u

//! Register Reset Value
#define DESC1_31_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_31_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_31_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_31_CPU_EGP_4 Register DESC2_31_CPU_EGP_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_31_CPU_EGP_4 0x142F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_31_CPU_EGP_4 0x1E7D42F8u

//! Register Reset Value
#define DESC2_31_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_31_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_31_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_31_CPU_EGP_4 Register DESC3_31_CPU_EGP_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_31_CPU_EGP_4 0x142FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_31_CPU_EGP_4 0x1E7D42FCu

//! Register Reset Value
#define DESC3_31_CPU_EGP_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_31_CPU_EGP_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_31_CPU_EGP_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_5 Register CFG_CPU_EGP_5 - Special CPU Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_5 0x15000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_5 0x1E7D5000u

//! Register Reset Value
#define CFG_CPU_EGP_5_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_5_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_5_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_5_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_5_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_5_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_5_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_5_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_5_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_5_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_5_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_5 Register DQPC_CPU_EGP_5 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_5 0x15004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_5 0x1E7D5004u

//! Register Reset Value
#define DQPC_CPU_EGP_5_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_5_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_5_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_5 Register IRNCR_CPU_EGP_5 - Special CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_5 0x15020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_5 0x1E7D5020u

//! Register Reset Value
#define IRNCR_CPU_EGP_5_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_5_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_5_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_5_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_5_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_5 Register IRNICR_CPU_EGP_5 - Special CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_5 0x15024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_5 0x1E7D5024u

//! Register Reset Value
#define IRNICR_CPU_EGP_5_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_5_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_5_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_5 Register IRNEN_CPU_EGP_5 - Special CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_5 0x15028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_5 0x1E7D5028u

//! Register Reset Value
#define IRNEN_CPU_EGP_5_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_5_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_5_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_5_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_5_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_5 Register DPTR_CPU_EGP_5 - CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_5 0x15030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_5 0x1E7D5030u

//! Register Reset Value
#define DPTR_CPU_EGP_5_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_5_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_5_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_5_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_5_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_6 Register CFG_DMA_EGP_6 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_6 0x16000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_6 0x1E7D6000u

//! Register Reset Value
#define CFG_DMA_EGP_6_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_6_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_6_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_6_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_6_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_6_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_6_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_6_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_6_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_6_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_6_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_6 Register DQPC_DMA_EGP_6 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_6 0x16004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_6 0x1E7D6004u

//! Register Reset Value
#define DQPC_DMA_EGP_6_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_6_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_6_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_6 Register IRNCR_DMA_EGP_6 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_6 0x16020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_6 0x1E7D6020u

//! Register Reset Value
#define IRNCR_DMA_EGP_6_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_6_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_6_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_6_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_6_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_6 Register IRNICR_DMA_EGP_6 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_6 0x16024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_6 0x1E7D6024u

//! Register Reset Value
#define IRNICR_DMA_EGP_6_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_6_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_6_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_6 Register IRNEN_DMA_EGP_6 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_6 0x16028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_6 0x1E7D6028u

//! Register Reset Value
#define IRNEN_DMA_EGP_6_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_6_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_6_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_6_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_6_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_6 Register DPTR_DMA_EGP_6 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_6 0x16030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_6 0x1E7D6030u

//! Register Reset Value
#define DPTR_DMA_EGP_6_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_6_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_6_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_6_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_6_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_7 Register CFG_DMA_EGP_7 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_7 0x17000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_7 0x1E7D7000u

//! Register Reset Value
#define CFG_DMA_EGP_7_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_7_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_7_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_7_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_7_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_7_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_7_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_7_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_7_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_7_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_7_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_7 Register DQPC_DMA_EGP_7 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_7 0x17004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_7 0x1E7D7004u

//! Register Reset Value
#define DQPC_DMA_EGP_7_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_7_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_7_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_7 Register IRNCR_DMA_EGP_7 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_7 0x17020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_7 0x1E7D7020u

//! Register Reset Value
#define IRNCR_DMA_EGP_7_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_7_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_7_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_7_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_7_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_7 Register IRNICR_DMA_EGP_7 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_7 0x17024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_7 0x1E7D7024u

//! Register Reset Value
#define IRNICR_DMA_EGP_7_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_7_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_7_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_7 Register IRNEN_DMA_EGP_7 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_7 0x17028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_7 0x1E7D7028u

//! Register Reset Value
#define IRNEN_DMA_EGP_7_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_7_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_7_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_7_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_7_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_7 Register DPTR_DMA_EGP_7 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_7 0x17030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_7 0x1E7D7030u

//! Register Reset Value
#define DPTR_DMA_EGP_7_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_7_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_7_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_7_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_7_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_8 Register CFG_DMA_EGP_8 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_8 0x18000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_8 0x1E7D8000u

//! Register Reset Value
#define CFG_DMA_EGP_8_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_8_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_8_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_8_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_8_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_8_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_8_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_8_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_8_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_8_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_8_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_8 Register DQPC_DMA_EGP_8 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_8 0x18004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_8 0x1E7D8004u

//! Register Reset Value
#define DQPC_DMA_EGP_8_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_8_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_8_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_8 Register IRNCR_DMA_EGP_8 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_8 0x18020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_8 0x1E7D8020u

//! Register Reset Value
#define IRNCR_DMA_EGP_8_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_8_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_8_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_8_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_8_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_8 Register IRNICR_DMA_EGP_8 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_8 0x18024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_8 0x1E7D8024u

//! Register Reset Value
#define IRNICR_DMA_EGP_8_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_8_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_8_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_8 Register IRNEN_DMA_EGP_8 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_8 0x18028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_8 0x1E7D8028u

//! Register Reset Value
#define IRNEN_DMA_EGP_8_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_8_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_8_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_8_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_8_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_8 Register DPTR_DMA_EGP_8 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_8 0x18030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_8 0x1E7D8030u

//! Register Reset Value
#define DPTR_DMA_EGP_8_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_8_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_8_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_8_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_8_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_9 Register CFG_DMA_EGP_9 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_9 0x19000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_9 0x1E7D9000u

//! Register Reset Value
#define CFG_DMA_EGP_9_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_9_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_9_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_9_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_9_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_9_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_9_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_9_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_9_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_9_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_9_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_9 Register DQPC_DMA_EGP_9 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_9 0x19004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_9 0x1E7D9004u

//! Register Reset Value
#define DQPC_DMA_EGP_9_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_9_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_9_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_9 Register IRNCR_DMA_EGP_9 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_9 0x19020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_9 0x1E7D9020u

//! Register Reset Value
#define IRNCR_DMA_EGP_9_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_9_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_9_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_9_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_9_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_9 Register IRNICR_DMA_EGP_9 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_9 0x19024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_9 0x1E7D9024u

//! Register Reset Value
#define IRNICR_DMA_EGP_9_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_9_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_9_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_9 Register IRNEN_DMA_EGP_9 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_9 0x19028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_9 0x1E7D9028u

//! Register Reset Value
#define IRNEN_DMA_EGP_9_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_9_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_9_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_9_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_9_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_9 Register DPTR_DMA_EGP_9 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_9 0x19030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_9 0x1E7D9030u

//! Register Reset Value
#define DPTR_DMA_EGP_9_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_9_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_9_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_9_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_9_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_10 Register CFG_DMA_EGP_10 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_10 0x1A000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_10 0x1E7DA000u

//! Register Reset Value
#define CFG_DMA_EGP_10_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_10_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_10_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_10_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_10_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_10_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_10_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_10_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_10_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_10_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_10_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_10 Register DQPC_DMA_EGP_10 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_10 0x1A004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_10 0x1E7DA004u

//! Register Reset Value
#define DQPC_DMA_EGP_10_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_10_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_10_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_10 Register IRNCR_DMA_EGP_10 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_10 0x1A020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_10 0x1E7DA020u

//! Register Reset Value
#define IRNCR_DMA_EGP_10_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_10_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_10_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_10_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_10_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_10 Register IRNICR_DMA_EGP_10 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_10 0x1A024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_10 0x1E7DA024u

//! Register Reset Value
#define IRNICR_DMA_EGP_10_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_10_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_10_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_10 Register IRNEN_DMA_EGP_10 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_10 0x1A028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_10 0x1E7DA028u

//! Register Reset Value
#define IRNEN_DMA_EGP_10_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_10_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_10_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_10_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_10_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_10 Register DPTR_DMA_EGP_10 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_10 0x1A030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_10 0x1E7DA030u

//! Register Reset Value
#define DPTR_DMA_EGP_10_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_10_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_10_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_10_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_10_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_11 Register CFG_DMA_EGP_11 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_11 0x1B000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_11 0x1E7DB000u

//! Register Reset Value
#define CFG_DMA_EGP_11_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_11_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_11_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_11_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_11_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_11_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_11_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_11_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_11_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_11_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_11_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_11 Register DQPC_DMA_EGP_11 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_11 0x1B004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_11 0x1E7DB004u

//! Register Reset Value
#define DQPC_DMA_EGP_11_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_11_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_11_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_11 Register IRNCR_DMA_EGP_11 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_11 0x1B020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_11 0x1E7DB020u

//! Register Reset Value
#define IRNCR_DMA_EGP_11_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_11_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_11_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_11_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_11_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_11 Register IRNICR_DMA_EGP_11 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_11 0x1B024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_11 0x1E7DB024u

//! Register Reset Value
#define IRNICR_DMA_EGP_11_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_11_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_11_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_11 Register IRNEN_DMA_EGP_11 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_11 0x1B028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_11 0x1E7DB028u

//! Register Reset Value
#define IRNEN_DMA_EGP_11_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_11_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_11_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_11_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_11_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_11 Register DPTR_DMA_EGP_11 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_11 0x1B030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_11 0x1E7DB030u

//! Register Reset Value
#define DPTR_DMA_EGP_11_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_11_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_11_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_11_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_11_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_12 Register CFG_DMA_EGP_12 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_12 0x1C000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_12 0x1E7DC000u

//! Register Reset Value
#define CFG_DMA_EGP_12_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_12_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_12_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_12_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_12_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_12_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_12_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_12_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_12_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_12_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_12_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_12 Register DQPC_DMA_EGP_12 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_12 0x1C004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_12 0x1E7DC004u

//! Register Reset Value
#define DQPC_DMA_EGP_12_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_12_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_12_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_12 Register IRNCR_DMA_EGP_12 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_12 0x1C020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_12 0x1E7DC020u

//! Register Reset Value
#define IRNCR_DMA_EGP_12_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_12_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_12_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_12_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_12_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_12 Register IRNICR_DMA_EGP_12 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_12 0x1C024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_12 0x1E7DC024u

//! Register Reset Value
#define IRNICR_DMA_EGP_12_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_12_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_12_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_12 Register IRNEN_DMA_EGP_12 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_12 0x1C028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_12 0x1E7DC028u

//! Register Reset Value
#define IRNEN_DMA_EGP_12_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_12_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_12_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_12_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_12_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_12 Register DPTR_DMA_EGP_12 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_12 0x1C030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_12 0x1E7DC030u

//! Register Reset Value
#define DPTR_DMA_EGP_12_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_12_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_12_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_12_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_12_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_13 Register CFG_DMA_EGP_13 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_13 0x1D000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_13 0x1E7DD000u

//! Register Reset Value
#define CFG_DMA_EGP_13_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_13_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_13_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_13_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_13_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_13_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_13_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_13_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_13_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_13_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_13_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_13 Register DQPC_DMA_EGP_13 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_13 0x1D004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_13 0x1E7DD004u

//! Register Reset Value
#define DQPC_DMA_EGP_13_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_13_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_13_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_13 Register IRNCR_DMA_EGP_13 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_13 0x1D020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_13 0x1E7DD020u

//! Register Reset Value
#define IRNCR_DMA_EGP_13_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_13_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_13_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_13_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_13_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_13 Register IRNICR_DMA_EGP_13 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_13 0x1D024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_13 0x1E7DD024u

//! Register Reset Value
#define IRNICR_DMA_EGP_13_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_13_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_13_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_13 Register IRNEN_DMA_EGP_13 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_13 0x1D028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_13 0x1E7DD028u

//! Register Reset Value
#define IRNEN_DMA_EGP_13_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_13_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_13_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_13_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_13_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_13 Register DPTR_DMA_EGP_13 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_13 0x1D030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_13 0x1E7DD030u

//! Register Reset Value
#define DPTR_DMA_EGP_13_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_13_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_13_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_13_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_13_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_14 Register CFG_DMA_EGP_14 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_14 0x1E000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_14 0x1E7DE000u

//! Register Reset Value
#define CFG_DMA_EGP_14_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_14_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_14_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_14_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_14_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_14_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_14_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_14_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_14_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_14_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_14_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_14 Register DQPC_DMA_EGP_14 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_14 0x1E004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_14 0x1E7DE004u

//! Register Reset Value
#define DQPC_DMA_EGP_14_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_14_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_14_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_14 Register IRNCR_DMA_EGP_14 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_14 0x1E020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_14 0x1E7DE020u

//! Register Reset Value
#define IRNCR_DMA_EGP_14_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_14_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_14_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_14_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_14_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_14 Register IRNICR_DMA_EGP_14 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_14 0x1E024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_14 0x1E7DE024u

//! Register Reset Value
#define IRNICR_DMA_EGP_14_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_14_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_14_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_14 Register IRNEN_DMA_EGP_14 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_14 0x1E028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_14 0x1E7DE028u

//! Register Reset Value
#define IRNEN_DMA_EGP_14_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_14_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_14_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_14_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_14_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_14 Register DPTR_DMA_EGP_14 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_14 0x1E030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_14 0x1E7DE030u

//! Register Reset Value
#define DPTR_DMA_EGP_14_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_14_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_14_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_14_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_14_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_15 Register CFG_DMA_EGP_15 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_15 0x1F000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_15 0x1E7DF000u

//! Register Reset Value
#define CFG_DMA_EGP_15_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_15_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_15_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_15_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_15_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_15_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_15_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_15_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_15_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_15_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_15_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_15 Register DQPC_DMA_EGP_15 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_15 0x1F004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_15 0x1E7DF004u

//! Register Reset Value
#define DQPC_DMA_EGP_15_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_15_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_15_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_15 Register IRNCR_DMA_EGP_15 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_15 0x1F020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_15 0x1E7DF020u

//! Register Reset Value
#define IRNCR_DMA_EGP_15_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_15_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_15_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_15_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_15_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_15 Register IRNICR_DMA_EGP_15 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_15 0x1F024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_15 0x1E7DF024u

//! Register Reset Value
#define IRNICR_DMA_EGP_15_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_15_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_15_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_15 Register IRNEN_DMA_EGP_15 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_15 0x1F028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_15 0x1E7DF028u

//! Register Reset Value
#define IRNEN_DMA_EGP_15_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_15_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_15_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_15_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_15_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_15 Register DPTR_DMA_EGP_15 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_15 0x1F030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_15 0x1E7DF030u

//! Register Reset Value
#define DPTR_DMA_EGP_15_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_15_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_15_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_15_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_15_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_16 Register CFG_DMA_EGP_16 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_16 0x20000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_16 0x1E7E0000u

//! Register Reset Value
#define CFG_DMA_EGP_16_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_16_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_16_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_16_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_16_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_16_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_16_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_16_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_16_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_16_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_16_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_16 Register DQPC_DMA_EGP_16 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_16 0x20004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_16 0x1E7E0004u

//! Register Reset Value
#define DQPC_DMA_EGP_16_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_16_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_16_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_16 Register IRNCR_DMA_EGP_16 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_16 0x20020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_16 0x1E7E0020u

//! Register Reset Value
#define IRNCR_DMA_EGP_16_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_16_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_16_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_16_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_16_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_16 Register IRNICR_DMA_EGP_16 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_16 0x20024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_16 0x1E7E0024u

//! Register Reset Value
#define IRNICR_DMA_EGP_16_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_16_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_16_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_16 Register IRNEN_DMA_EGP_16 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_16 0x20028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_16 0x1E7E0028u

//! Register Reset Value
#define IRNEN_DMA_EGP_16_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_16_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_16_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_16_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_16_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_16 Register DPTR_DMA_EGP_16 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_16 0x20030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_16 0x1E7E0030u

//! Register Reset Value
#define DPTR_DMA_EGP_16_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_16_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_16_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_16_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_16_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_17 Register CFG_DMA_EGP_17 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_17 0x21000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_17 0x1E7E1000u

//! Register Reset Value
#define CFG_DMA_EGP_17_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_17_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_17_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_17_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_17_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_17_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_17_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_17_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_17_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_17_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_17_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_17 Register DQPC_DMA_EGP_17 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_17 0x21004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_17 0x1E7E1004u

//! Register Reset Value
#define DQPC_DMA_EGP_17_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_17_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_17_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_17 Register IRNCR_DMA_EGP_17 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_17 0x21020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_17 0x1E7E1020u

//! Register Reset Value
#define IRNCR_DMA_EGP_17_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_17_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_17_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_17_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_17_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_17 Register IRNICR_DMA_EGP_17 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_17 0x21024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_17 0x1E7E1024u

//! Register Reset Value
#define IRNICR_DMA_EGP_17_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_17_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_17_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_17 Register IRNEN_DMA_EGP_17 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_17 0x21028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_17 0x1E7E1028u

//! Register Reset Value
#define IRNEN_DMA_EGP_17_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_17_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_17_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_17_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_17_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_17 Register DPTR_DMA_EGP_17 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_17 0x21030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_17 0x1E7E1030u

//! Register Reset Value
#define DPTR_DMA_EGP_17_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_17_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_17_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_17_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_17_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_18 Register CFG_DMA_EGP_18 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_18 0x22000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_18 0x1E7E2000u

//! Register Reset Value
#define CFG_DMA_EGP_18_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_18_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_18_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_18_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_18_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_18_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_18_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_18_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_18_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_18_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_18_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_18 Register DQPC_DMA_EGP_18 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_18 0x22004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_18 0x1E7E2004u

//! Register Reset Value
#define DQPC_DMA_EGP_18_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_18_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_18_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_18 Register IRNCR_DMA_EGP_18 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_18 0x22020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_18 0x1E7E2020u

//! Register Reset Value
#define IRNCR_DMA_EGP_18_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_18_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_18_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_18_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_18_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_18 Register IRNICR_DMA_EGP_18 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_18 0x22024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_18 0x1E7E2024u

//! Register Reset Value
#define IRNICR_DMA_EGP_18_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_18_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_18_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_18 Register IRNEN_DMA_EGP_18 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_18 0x22028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_18 0x1E7E2028u

//! Register Reset Value
#define IRNEN_DMA_EGP_18_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_18_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_18_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_18_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_18_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_18 Register DPTR_DMA_EGP_18 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_18 0x22030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_18 0x1E7E2030u

//! Register Reset Value
#define DPTR_DMA_EGP_18_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_18_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_18_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_18_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_18_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_19 Register CFG_DMA_EGP_19 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_19 0x23000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_19 0x1E7E3000u

//! Register Reset Value
#define CFG_DMA_EGP_19_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_19_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_19_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_19_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_19_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_19_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_19_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_19_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_19_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_19_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_19_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_19 Register DQPC_DMA_EGP_19 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_19 0x23004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_19 0x1E7E3004u

//! Register Reset Value
#define DQPC_DMA_EGP_19_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_19_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_19_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_19 Register IRNCR_DMA_EGP_19 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_19 0x23020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_19 0x1E7E3020u

//! Register Reset Value
#define IRNCR_DMA_EGP_19_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_19_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_19_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_19_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_19_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_19 Register IRNICR_DMA_EGP_19 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_19 0x23024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_19 0x1E7E3024u

//! Register Reset Value
#define IRNICR_DMA_EGP_19_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_19_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_19_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_19 Register IRNEN_DMA_EGP_19 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_19 0x23028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_19 0x1E7E3028u

//! Register Reset Value
#define IRNEN_DMA_EGP_19_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_19_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_19_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_19_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_19_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_19 Register DPTR_DMA_EGP_19 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_19 0x23030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_19 0x1E7E3030u

//! Register Reset Value
#define DPTR_DMA_EGP_19_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_19_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_19_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_19_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_19_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_20 Register CFG_DMA_EGP_20 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_20 0x24000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_20 0x1E7E4000u

//! Register Reset Value
#define CFG_DMA_EGP_20_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_20_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_20_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_20_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_20_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_20_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_20_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_20_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_20_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_20_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_20_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_20 Register DQPC_DMA_EGP_20 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_20 0x24004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_20 0x1E7E4004u

//! Register Reset Value
#define DQPC_DMA_EGP_20_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_20_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_20_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_20 Register IRNCR_DMA_EGP_20 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_20 0x24020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_20 0x1E7E4020u

//! Register Reset Value
#define IRNCR_DMA_EGP_20_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_20_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_20_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_20_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_20_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_20 Register IRNICR_DMA_EGP_20 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_20 0x24024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_20 0x1E7E4024u

//! Register Reset Value
#define IRNICR_DMA_EGP_20_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_20_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_20_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_20 Register IRNEN_DMA_EGP_20 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_20 0x24028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_20 0x1E7E4028u

//! Register Reset Value
#define IRNEN_DMA_EGP_20_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_20_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_20_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_20_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_20_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_20 Register DPTR_DMA_EGP_20 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_20 0x24030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_20 0x1E7E4030u

//! Register Reset Value
#define DPTR_DMA_EGP_20_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_20_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_20_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_20_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_20_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_21 Register CFG_DMA_EGP_21 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_21 0x25000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_21 0x1E7E5000u

//! Register Reset Value
#define CFG_DMA_EGP_21_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_21_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_21_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_21_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_21_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_21_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_21_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_21_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_21_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_21_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_21_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_21 Register DQPC_DMA_EGP_21 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_21 0x25004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_21 0x1E7E5004u

//! Register Reset Value
#define DQPC_DMA_EGP_21_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_21_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_21_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_21 Register IRNCR_DMA_EGP_21 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_21 0x25020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_21 0x1E7E5020u

//! Register Reset Value
#define IRNCR_DMA_EGP_21_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_21_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_21_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_21_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_21_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_21 Register IRNICR_DMA_EGP_21 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_21 0x25024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_21 0x1E7E5024u

//! Register Reset Value
#define IRNICR_DMA_EGP_21_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_21_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_21_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_21 Register IRNEN_DMA_EGP_21 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_21 0x25028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_21 0x1E7E5028u

//! Register Reset Value
#define IRNEN_DMA_EGP_21_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_21_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_21_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_21_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_21_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_21 Register DPTR_DMA_EGP_21 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_21 0x25030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_21 0x1E7E5030u

//! Register Reset Value
#define DPTR_DMA_EGP_21_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_21_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_21_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_21_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_21_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_22 Register CFG_DMA_EGP_22 - DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_22 0x26000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_22 0x1E7E6000u

//! Register Reset Value
#define CFG_DMA_EGP_22_RST 0x00000000u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_22_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_22_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_22_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_22_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_22_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_22_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_22_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_22_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_22_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_22_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_22 Register DQPC_DMA_EGP_22 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_22 0x26004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_22 0x1E7E6004u

//! Register Reset Value
#define DQPC_DMA_EGP_22_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_22_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_22_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_22 Register IRNCR_DMA_EGP_22 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_22 0x26020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_22 0x1E7E6020u

//! Register Reset Value
#define IRNCR_DMA_EGP_22_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_22_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_22_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_22_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_22_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_22 Register IRNICR_DMA_EGP_22 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_22 0x26024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_22 0x1E7E6024u

//! Register Reset Value
#define IRNICR_DMA_EGP_22_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_22_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_22_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_22 Register IRNEN_DMA_EGP_22 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_22 0x26028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_22 0x1E7E6028u

//! Register Reset Value
#define IRNEN_DMA_EGP_22_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_22_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_22_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_22_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_22_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_22 Register DPTR_DMA_EGP_22 - DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_22 0x26030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_22 0x1E7E6030u

//! Register Reset Value
#define DPTR_DMA_EGP_22_RST 0x00000001u

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_22_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_22_ND_MASK 0x1u

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_22_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_22_DPTR_MASK 0x10000u

//! @}

//! \defgroup CFG_DMA_EGP_23 Register CFG_DMA_EGP_23 - Legacy DMA Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_DMA_EGP_23 0x27000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_DMA_EGP_23 0x1E7E7000u

//! Register Reset Value
#define CFG_DMA_EGP_23_RST 0x00001800u

//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_23_DQREQ_POS 0
//! Field DQREQ - Enable DMA Dequeue Request
#define CFG_DMA_EGP_23_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_23_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_23_DQREQ_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_23_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_DMA_EGP_23_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_DMA_EGP_23_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_DMA_EGP_23_DQPCEN_EN 0x1

//! Field DESC_INV - Descriptor Invert
#define CFG_DMA_EGP_23_DESC_INV_POS 11
//! Field DESC_INV - Descriptor Invert
#define CFG_DMA_EGP_23_DESC_INV_MASK 0x800u
//! Constant DIS - Disable
#define CONST_CFG_DMA_EGP_23_DESC_INV_DIS 0x0
//! Constant EN - Enable
#define CONST_CFG_DMA_EGP_23_DESC_INV_EN 0x1

//! Field LDM - Legacy Descriptor Mode
#define CFG_DMA_EGP_23_LDM_POS 12
//! Field LDM - Legacy Descriptor Mode
#define CFG_DMA_EGP_23_LDM_MASK 0x1000u
//! Constant NORMAL - Normal mode
#define CONST_CFG_DMA_EGP_23_LDM_NORMAL 0x0
//! Constant LEGACY - Legacy mode
#define CONST_CFG_DMA_EGP_23_LDM_LEGACY 0x1

//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_23_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_DMA_EGP_23_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_DMA_EGP_23 Register DQPC_DMA_EGP_23 - DMA Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_DMA_EGP_23 0x27004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_DMA_EGP_23 0x1E7E7004u

//! Register Reset Value
#define DQPC_DMA_EGP_23_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_23_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_DMA_EGP_23_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_DMA_EGP_23 Register IRNCR_DMA_EGP_23 - DMA Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_DMA_EGP_23 0x27020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_DMA_EGP_23 0x1E7E7020u

//! Register Reset Value
#define IRNCR_DMA_EGP_23_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_23_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_DMA_EGP_23_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_DMA_EGP_23_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_DMA_EGP_23_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_DMA_EGP_23 Register IRNICR_DMA_EGP_23 - DMA Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_DMA_EGP_23 0x27024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_DMA_EGP_23 0x1E7E7024u

//! Register Reset Value
#define IRNICR_DMA_EGP_23_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_23_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_DMA_EGP_23_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_DMA_EGP_23 Register IRNEN_DMA_EGP_23 - DMA Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_DMA_EGP_23 0x27028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_DMA_EGP_23 0x1E7E7028u

//! Register Reset Value
#define IRNEN_DMA_EGP_23_RST 0x00000000u

//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_23_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_DMA_EGP_23_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_DMA_EGP_23_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_DMA_EGP_23_DR_EN 0x1

//! @}

//! \defgroup DPTR_DMA_EGP_23 Register DPTR_DMA_EGP_23 - Legacy DMA Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_DMA_EGP_23 0x27030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_DMA_EGP_23 0x1E7E7030u

//! Register Reset Value
#define DPTR_DMA_EGP_23_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_23_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_DMA_EGP_23_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_23_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_DMA_EGP_23_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup LDESC0_0_DMA_EGP_23 Register LDESC0_0_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_0_DMA_EGP_23 0x27100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_0_DMA_EGP_23 0x1E7E7100u

//! Register Reset Value
#define LDESC0_0_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_0_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_0_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_0_DMA_EGP_23 Register LDESC1_0_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_0_DMA_EGP_23 0x27104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_0_DMA_EGP_23 0x1E7E7104u

//! Register Reset Value
#define LDESC1_0_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_0_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_0_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_1_DMA_EGP_23 Register LDESC0_1_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_1_DMA_EGP_23 0x27108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_1_DMA_EGP_23 0x1E7E7108u

//! Register Reset Value
#define LDESC0_1_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_1_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_1_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_1_DMA_EGP_23 Register LDESC1_1_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_1_DMA_EGP_23 0x2710C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_1_DMA_EGP_23 0x1E7E710Cu

//! Register Reset Value
#define LDESC1_1_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_1_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_1_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_2_DMA_EGP_23 Register LDESC0_2_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_2_DMA_EGP_23 0x27110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_2_DMA_EGP_23 0x1E7E7110u

//! Register Reset Value
#define LDESC0_2_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_2_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_2_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_2_DMA_EGP_23 Register LDESC1_2_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_2_DMA_EGP_23 0x27114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_2_DMA_EGP_23 0x1E7E7114u

//! Register Reset Value
#define LDESC1_2_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_2_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_2_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_3_DMA_EGP_23 Register LDESC0_3_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_3_DMA_EGP_23 0x27118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_3_DMA_EGP_23 0x1E7E7118u

//! Register Reset Value
#define LDESC0_3_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_3_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_3_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_3_DMA_EGP_23 Register LDESC1_3_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_3_DMA_EGP_23 0x2711C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_3_DMA_EGP_23 0x1E7E711Cu

//! Register Reset Value
#define LDESC1_3_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_3_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_3_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_4_DMA_EGP_23 Register LDESC0_4_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_4_DMA_EGP_23 0x27120
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_4_DMA_EGP_23 0x1E7E7120u

//! Register Reset Value
#define LDESC0_4_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_4_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_4_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_4_DMA_EGP_23 Register LDESC1_4_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_4_DMA_EGP_23 0x27124
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_4_DMA_EGP_23 0x1E7E7124u

//! Register Reset Value
#define LDESC1_4_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_4_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_4_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_5_DMA_EGP_23 Register LDESC0_5_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_5_DMA_EGP_23 0x27128
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_5_DMA_EGP_23 0x1E7E7128u

//! Register Reset Value
#define LDESC0_5_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_5_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_5_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_5_DMA_EGP_23 Register LDESC1_5_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_5_DMA_EGP_23 0x2712C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_5_DMA_EGP_23 0x1E7E712Cu

//! Register Reset Value
#define LDESC1_5_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_5_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_5_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_6_DMA_EGP_23 Register LDESC0_6_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_6_DMA_EGP_23 0x27130
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_6_DMA_EGP_23 0x1E7E7130u

//! Register Reset Value
#define LDESC0_6_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_6_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_6_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_6_DMA_EGP_23 Register LDESC1_6_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_6_DMA_EGP_23 0x27134
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_6_DMA_EGP_23 0x1E7E7134u

//! Register Reset Value
#define LDESC1_6_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_6_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_6_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_7_DMA_EGP_23 Register LDESC0_7_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_7_DMA_EGP_23 0x27138
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_7_DMA_EGP_23 0x1E7E7138u

//! Register Reset Value
#define LDESC0_7_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_7_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_7_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_7_DMA_EGP_23 Register LDESC1_7_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_7_DMA_EGP_23 0x2713C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_7_DMA_EGP_23 0x1E7E713Cu

//! Register Reset Value
#define LDESC1_7_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_7_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_7_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_8_DMA_EGP_23 Register LDESC0_8_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_8_DMA_EGP_23 0x27140
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_8_DMA_EGP_23 0x1E7E7140u

//! Register Reset Value
#define LDESC0_8_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_8_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_8_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_8_DMA_EGP_23 Register LDESC1_8_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_8_DMA_EGP_23 0x27144
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_8_DMA_EGP_23 0x1E7E7144u

//! Register Reset Value
#define LDESC1_8_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_8_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_8_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_9_DMA_EGP_23 Register LDESC0_9_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_9_DMA_EGP_23 0x27148
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_9_DMA_EGP_23 0x1E7E7148u

//! Register Reset Value
#define LDESC0_9_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_9_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_9_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_9_DMA_EGP_23 Register LDESC1_9_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_9_DMA_EGP_23 0x2714C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_9_DMA_EGP_23 0x1E7E714Cu

//! Register Reset Value
#define LDESC1_9_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_9_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_9_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_10_DMA_EGP_23 Register LDESC0_10_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_10_DMA_EGP_23 0x27150
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_10_DMA_EGP_23 0x1E7E7150u

//! Register Reset Value
#define LDESC0_10_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_10_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_10_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_10_DMA_EGP_23 Register LDESC1_10_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_10_DMA_EGP_23 0x27154
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_10_DMA_EGP_23 0x1E7E7154u

//! Register Reset Value
#define LDESC1_10_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_10_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_10_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_11_DMA_EGP_23 Register LDESC0_11_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_11_DMA_EGP_23 0x27158
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_11_DMA_EGP_23 0x1E7E7158u

//! Register Reset Value
#define LDESC0_11_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_11_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_11_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_11_DMA_EGP_23 Register LDESC1_11_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_11_DMA_EGP_23 0x2715C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_11_DMA_EGP_23 0x1E7E715Cu

//! Register Reset Value
#define LDESC1_11_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_11_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_11_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_12_DMA_EGP_23 Register LDESC0_12_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_12_DMA_EGP_23 0x27160
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_12_DMA_EGP_23 0x1E7E7160u

//! Register Reset Value
#define LDESC0_12_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_12_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_12_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_12_DMA_EGP_23 Register LDESC1_12_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_12_DMA_EGP_23 0x27164
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_12_DMA_EGP_23 0x1E7E7164u

//! Register Reset Value
#define LDESC1_12_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_12_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_12_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_13_DMA_EGP_23 Register LDESC0_13_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_13_DMA_EGP_23 0x27168
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_13_DMA_EGP_23 0x1E7E7168u

//! Register Reset Value
#define LDESC0_13_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_13_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_13_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_13_DMA_EGP_23 Register LDESC1_13_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_13_DMA_EGP_23 0x2716C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_13_DMA_EGP_23 0x1E7E716Cu

//! Register Reset Value
#define LDESC1_13_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_13_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_13_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_14_DMA_EGP_23 Register LDESC0_14_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_14_DMA_EGP_23 0x27170
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_14_DMA_EGP_23 0x1E7E7170u

//! Register Reset Value
#define LDESC0_14_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_14_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_14_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_14_DMA_EGP_23 Register LDESC1_14_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_14_DMA_EGP_23 0x27174
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_14_DMA_EGP_23 0x1E7E7174u

//! Register Reset Value
#define LDESC1_14_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_14_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_14_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_15_DMA_EGP_23 Register LDESC0_15_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_15_DMA_EGP_23 0x27178
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_15_DMA_EGP_23 0x1E7E7178u

//! Register Reset Value
#define LDESC0_15_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_15_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_15_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_15_DMA_EGP_23 Register LDESC1_15_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_15_DMA_EGP_23 0x2717C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_15_DMA_EGP_23 0x1E7E717Cu

//! Register Reset Value
#define LDESC1_15_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_15_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_15_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_16_DMA_EGP_23 Register LDESC0_16_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_16_DMA_EGP_23 0x27180
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_16_DMA_EGP_23 0x1E7E7180u

//! Register Reset Value
#define LDESC0_16_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_16_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_16_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_16_DMA_EGP_23 Register LDESC1_16_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_16_DMA_EGP_23 0x27184
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_16_DMA_EGP_23 0x1E7E7184u

//! Register Reset Value
#define LDESC1_16_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_16_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_16_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_17_DMA_EGP_23 Register LDESC0_17_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_17_DMA_EGP_23 0x27188
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_17_DMA_EGP_23 0x1E7E7188u

//! Register Reset Value
#define LDESC0_17_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_17_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_17_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_17_DMA_EGP_23 Register LDESC1_17_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_17_DMA_EGP_23 0x2718C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_17_DMA_EGP_23 0x1E7E718Cu

//! Register Reset Value
#define LDESC1_17_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_17_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_17_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_18_DMA_EGP_23 Register LDESC0_18_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_18_DMA_EGP_23 0x27190
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_18_DMA_EGP_23 0x1E7E7190u

//! Register Reset Value
#define LDESC0_18_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_18_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_18_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_18_DMA_EGP_23 Register LDESC1_18_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_18_DMA_EGP_23 0x27194
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_18_DMA_EGP_23 0x1E7E7194u

//! Register Reset Value
#define LDESC1_18_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_18_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_18_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_19_DMA_EGP_23 Register LDESC0_19_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_19_DMA_EGP_23 0x27198
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_19_DMA_EGP_23 0x1E7E7198u

//! Register Reset Value
#define LDESC0_19_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_19_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_19_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_19_DMA_EGP_23 Register LDESC1_19_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_19_DMA_EGP_23 0x2719C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_19_DMA_EGP_23 0x1E7E719Cu

//! Register Reset Value
#define LDESC1_19_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_19_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_19_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_20_DMA_EGP_23 Register LDESC0_20_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_20_DMA_EGP_23 0x271A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_20_DMA_EGP_23 0x1E7E71A0u

//! Register Reset Value
#define LDESC0_20_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_20_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_20_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_20_DMA_EGP_23 Register LDESC1_20_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_20_DMA_EGP_23 0x271A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_20_DMA_EGP_23 0x1E7E71A4u

//! Register Reset Value
#define LDESC1_20_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_20_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_20_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_21_DMA_EGP_23 Register LDESC0_21_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_21_DMA_EGP_23 0x271A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_21_DMA_EGP_23 0x1E7E71A8u

//! Register Reset Value
#define LDESC0_21_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_21_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_21_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_21_DMA_EGP_23 Register LDESC1_21_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_21_DMA_EGP_23 0x271AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_21_DMA_EGP_23 0x1E7E71ACu

//! Register Reset Value
#define LDESC1_21_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_21_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_21_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_22_DMA_EGP_23 Register LDESC0_22_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_22_DMA_EGP_23 0x271B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_22_DMA_EGP_23 0x1E7E71B0u

//! Register Reset Value
#define LDESC0_22_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_22_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_22_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_22_DMA_EGP_23 Register LDESC1_22_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_22_DMA_EGP_23 0x271B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_22_DMA_EGP_23 0x1E7E71B4u

//! Register Reset Value
#define LDESC1_22_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_22_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_22_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_23_DMA_EGP_23 Register LDESC0_23_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_23_DMA_EGP_23 0x271B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_23_DMA_EGP_23 0x1E7E71B8u

//! Register Reset Value
#define LDESC0_23_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_23_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_23_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_23_DMA_EGP_23 Register LDESC1_23_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_23_DMA_EGP_23 0x271BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_23_DMA_EGP_23 0x1E7E71BCu

//! Register Reset Value
#define LDESC1_23_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_23_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_23_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_24_DMA_EGP_23 Register LDESC0_24_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_24_DMA_EGP_23 0x271C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_24_DMA_EGP_23 0x1E7E71C0u

//! Register Reset Value
#define LDESC0_24_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_24_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_24_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_24_DMA_EGP_23 Register LDESC1_24_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_24_DMA_EGP_23 0x271C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_24_DMA_EGP_23 0x1E7E71C4u

//! Register Reset Value
#define LDESC1_24_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_24_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_24_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_25_DMA_EGP_23 Register LDESC0_25_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_25_DMA_EGP_23 0x271C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_25_DMA_EGP_23 0x1E7E71C8u

//! Register Reset Value
#define LDESC0_25_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_25_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_25_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_25_DMA_EGP_23 Register LDESC1_25_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_25_DMA_EGP_23 0x271CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_25_DMA_EGP_23 0x1E7E71CCu

//! Register Reset Value
#define LDESC1_25_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_25_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_25_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_26_DMA_EGP_23 Register LDESC0_26_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_26_DMA_EGP_23 0x271D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_26_DMA_EGP_23 0x1E7E71D0u

//! Register Reset Value
#define LDESC0_26_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_26_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_26_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_26_DMA_EGP_23 Register LDESC1_26_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_26_DMA_EGP_23 0x271D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_26_DMA_EGP_23 0x1E7E71D4u

//! Register Reset Value
#define LDESC1_26_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_26_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_26_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_27_DMA_EGP_23 Register LDESC0_27_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_27_DMA_EGP_23 0x271D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_27_DMA_EGP_23 0x1E7E71D8u

//! Register Reset Value
#define LDESC0_27_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_27_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_27_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_27_DMA_EGP_23 Register LDESC1_27_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_27_DMA_EGP_23 0x271DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_27_DMA_EGP_23 0x1E7E71DCu

//! Register Reset Value
#define LDESC1_27_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_27_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_27_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_28_DMA_EGP_23 Register LDESC0_28_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_28_DMA_EGP_23 0x271E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_28_DMA_EGP_23 0x1E7E71E0u

//! Register Reset Value
#define LDESC0_28_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_28_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_28_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_28_DMA_EGP_23 Register LDESC1_28_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_28_DMA_EGP_23 0x271E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_28_DMA_EGP_23 0x1E7E71E4u

//! Register Reset Value
#define LDESC1_28_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_28_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_28_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_29_DMA_EGP_23 Register LDESC0_29_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_29_DMA_EGP_23 0x271E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_29_DMA_EGP_23 0x1E7E71E8u

//! Register Reset Value
#define LDESC0_29_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_29_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_29_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_29_DMA_EGP_23 Register LDESC1_29_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_29_DMA_EGP_23 0x271EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_29_DMA_EGP_23 0x1E7E71ECu

//! Register Reset Value
#define LDESC1_29_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_29_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_29_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_30_DMA_EGP_23 Register LDESC0_30_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_30_DMA_EGP_23 0x271F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_30_DMA_EGP_23 0x1E7E71F0u

//! Register Reset Value
#define LDESC0_30_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_30_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_30_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_30_DMA_EGP_23 Register LDESC1_30_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_30_DMA_EGP_23 0x271F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_30_DMA_EGP_23 0x1E7E71F4u

//! Register Reset Value
#define LDESC1_30_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_30_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_30_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC0_31_DMA_EGP_23 Register LDESC0_31_DMA_EGP_23 - DMA Egress Port Descriptor 0
//! @{

//! Register Offset (relative)
#define LDESC0_31_DMA_EGP_23 0x271F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC0_31_DMA_EGP_23 0x1E7E71F8u

//! Register Reset Value
#define LDESC0_31_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define LDESC0_31_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define LDESC0_31_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup LDESC1_31_DMA_EGP_23 Register LDESC1_31_DMA_EGP_23 - DMA egress Port Descriptor 1
//! @{

//! Register Offset (relative)
#define LDESC1_31_DMA_EGP_23 0x271FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_LDESC1_31_DMA_EGP_23 0x1E7E71FCu

//! Register Reset Value
#define LDESC1_31_DMA_EGP_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define LDESC1_31_DMA_EGP_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define LDESC1_31_DMA_EGP_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_24 Register CFG_CPU_EGP_24 - CPU WLAN Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_24 0x28000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_24 0x1E7E8000u

//! Register Reset Value
#define CFG_CPU_EGP_24_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_24_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_24_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_24_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_24_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_24_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_24_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_24_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_24_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_24_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_24_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_24_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_24_BFBPEN_EN 0x1

//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_24_DQBPEN_POS 3
//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_24_DQBPEN_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_24_DQBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_24_DQBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_24_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_24_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_24_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_24_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_24_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_24_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_24 Register DQPC_CPU_EGP_24 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_24 0x28004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_24 0x1E7E8004u

//! Register Reset Value
#define DQPC_CPU_EGP_24_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_24_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_24_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_24 Register IRNCR_CPU_EGP_24 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_24 0x28020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_24 0x1E7E8020u

//! Register Reset Value
#define IRNCR_CPU_EGP_24_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_24_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_24_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_24_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_24_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_24_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_24_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_24_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_24_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_24 Register IRNICR_CPU_EGP_24 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_24 0x28024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_24 0x1E7E8024u

//! Register Reset Value
#define IRNICR_CPU_EGP_24_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_24_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_24_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_24_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_24_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_24 Register IRNEN_CPU_EGP_24 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_24 0x28028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_24 0x1E7E8028u

//! Register Reset Value
#define IRNEN_CPU_EGP_24_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_24_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_24_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_24_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_24_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_24_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_24_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_24_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_24_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_24 Register DPTR_CPU_EGP_24 - Special CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_24 0x28030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_24 0x1E7E8030u

//! Register Reset Value
#define DPTR_CPU_EGP_24_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_24_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_24_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_24_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_24_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup BPRC_CPU_EGP_24 Register BPRC_CPU_EGP_24 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_24 0x28034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_24 0x1E7E8034u

//! Register Reset Value
#define BPRC_CPU_EGP_24_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_24_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_24_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BRPTR_SCPU_EGP_24 Register BRPTR_SCPU_EGP_24 - Special CPU Egress Port Buffer Return Pointer
//! @{

//! Register Offset (relative)
#define BRPTR_SCPU_EGP_24 0x28038
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BRPTR_SCPU_EGP_24 0x1E7E8038u

//! Register Reset Value
#define BRPTR_SCPU_EGP_24_RST 0x00000000u

//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_24_PTRBR_POS 0
//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_24_PTRBR_MASK 0x1Fu

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_0 Register PTR_RTN_CPU_EGP_24_0 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_0 0x28080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_0 0x1E7E8080u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_0_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_0_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_0_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_1 Register PTR_RTN_CPU_EGP_24_1 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_1 0x28084
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_1 0x1E7E8084u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_1_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_1_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_1_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_2 Register PTR_RTN_CPU_EGP_24_2 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_2 0x28088
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_2 0x1E7E8088u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_2_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_2_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_2_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_3 Register PTR_RTN_CPU_EGP_24_3 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_3 0x2808C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_3 0x1E7E808Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_3_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_3_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_3_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_4 Register PTR_RTN_CPU_EGP_24_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_4 0x28090
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_4 0x1E7E8090u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_5 Register PTR_RTN_CPU_EGP_24_5 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_5 0x28094
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_5 0x1E7E8094u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_5_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_5_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_5_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_6 Register PTR_RTN_CPU_EGP_24_6 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_6 0x28098
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_6 0x1E7E8098u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_6_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_6_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_6_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_7 Register PTR_RTN_CPU_EGP_24_7 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_7 0x2809C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_7 0x1E7E809Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_7_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_7_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_7_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_8 Register PTR_RTN_CPU_EGP_24_8 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_8 0x280A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_8 0x1E7E80A0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_8_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_8_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_8_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_9 Register PTR_RTN_CPU_EGP_24_9 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_9 0x280A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_9 0x1E7E80A4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_9_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_9_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_9_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_10 Register PTR_RTN_CPU_EGP_24_10 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_10 0x280A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_10 0x1E7E80A8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_10_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_10_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_10_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_11 Register PTR_RTN_CPU_EGP_24_11 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_11 0x280AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_11 0x1E7E80ACu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_11_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_11_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_11_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_12 Register PTR_RTN_CPU_EGP_24_12 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_12 0x280B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_12 0x1E7E80B0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_12_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_12_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_12_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_13 Register PTR_RTN_CPU_EGP_24_13 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_13 0x280B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_13 0x1E7E80B4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_13_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_13_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_13_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_14 Register PTR_RTN_CPU_EGP_24_14 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_14 0x280B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_14 0x1E7E80B8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_14_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_14_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_14_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_15 Register PTR_RTN_CPU_EGP_24_15 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_15 0x280BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_15 0x1E7E80BCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_15_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_15_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_15_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_16 Register PTR_RTN_CPU_EGP_24_16 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_16 0x280C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_16 0x1E7E80C0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_16_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_16_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_16_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_17 Register PTR_RTN_CPU_EGP_24_17 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_17 0x280C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_17 0x1E7E80C4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_17_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_17_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_17_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_18 Register PTR_RTN_CPU_EGP_24_18 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_18 0x280C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_18 0x1E7E80C8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_18_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_18_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_18_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_19 Register PTR_RTN_CPU_EGP_24_19 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_19 0x280CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_19 0x1E7E80CCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_19_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_19_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_19_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_20 Register PTR_RTN_CPU_EGP_24_20 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_20 0x280D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_20 0x1E7E80D0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_20_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_20_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_20_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_21 Register PTR_RTN_CPU_EGP_24_21 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_21 0x280D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_21 0x1E7E80D4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_21_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_21_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_21_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_22 Register PTR_RTN_CPU_EGP_24_22 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_22 0x280D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_22 0x1E7E80D8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_22_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_22_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_22_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_23 Register PTR_RTN_CPU_EGP_24_23 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_23 0x280DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_23 0x1E7E80DCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_23_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_23_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_23_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_24 Register PTR_RTN_CPU_EGP_24_24 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_24 0x280E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_24 0x1E7E80E0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_24_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_24_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_24_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_25 Register PTR_RTN_CPU_EGP_24_25 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_25 0x280E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_25 0x1E7E80E4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_25_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_25_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_25_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_26 Register PTR_RTN_CPU_EGP_24_26 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_26 0x280E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_26 0x1E7E80E8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_26_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_26_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_26_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_27 Register PTR_RTN_CPU_EGP_24_27 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_27 0x280EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_27 0x1E7E80ECu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_27_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_27_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_27_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_28 Register PTR_RTN_CPU_EGP_24_28 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_28 0x280F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_28 0x1E7E80F0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_28_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_28_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_28_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_29 Register PTR_RTN_CPU_EGP_24_29 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_29 0x280F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_29 0x1E7E80F4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_29_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_29_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_29_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_30 Register PTR_RTN_CPU_EGP_24_30 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_30 0x280F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_30 0x1E7E80F8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_30_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_30_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_30_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_24_31 Register PTR_RTN_CPU_EGP_24_31 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_24_31 0x280FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_24_31 0x1E7E80FCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_24_31_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_31_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_24_31_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_CPU_EGP_24_0 Register DESC0_CPU_EGP_24_0 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_0 0x28100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_0 0x1E7E8100u

//! Register Reset Value
#define DESC0_CPU_EGP_24_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_0 Register DESC1_CPU_EGP_24_0 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_0 0x28104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_0 0x1E7E8104u

//! Register Reset Value
#define DESC1_CPU_EGP_24_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_0 Register DESC2_CPU_EGP_24_0 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_0 0x28108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_0 0x1E7E8108u

//! Register Reset Value
#define DESC2_CPU_EGP_24_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_0 Register DESC3_CPU_EGP_24_0 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_0 0x2810C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_0 0x1E7E810Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_1 Register DESC0_CPU_EGP_24_1 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_1 0x28110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_1 0x1E7E8110u

//! Register Reset Value
#define DESC0_CPU_EGP_24_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_1 Register DESC1_CPU_EGP_24_1 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_1 0x28114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_1 0x1E7E8114u

//! Register Reset Value
#define DESC1_CPU_EGP_24_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_1 Register DESC2_CPU_EGP_24_1 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_1 0x28118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_1 0x1E7E8118u

//! Register Reset Value
#define DESC2_CPU_EGP_24_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_1 Register DESC3_CPU_EGP_24_1 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_1 0x2811C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_1 0x1E7E811Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_2 Register DESC0_CPU_EGP_24_2 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_2 0x28120
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_2 0x1E7E8120u

//! Register Reset Value
#define DESC0_CPU_EGP_24_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_2 Register DESC1_CPU_EGP_24_2 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_2 0x28124
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_2 0x1E7E8124u

//! Register Reset Value
#define DESC1_CPU_EGP_24_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_2 Register DESC2_CPU_EGP_24_2 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_2 0x28128
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_2 0x1E7E8128u

//! Register Reset Value
#define DESC2_CPU_EGP_24_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_2 Register DESC3_CPU_EGP_24_2 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_2 0x2812C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_2 0x1E7E812Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_3 Register DESC0_CPU_EGP_24_3 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_3 0x28130
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_3 0x1E7E8130u

//! Register Reset Value
#define DESC0_CPU_EGP_24_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_3 Register DESC1_CPU_EGP_24_3 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_3 0x28134
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_3 0x1E7E8134u

//! Register Reset Value
#define DESC1_CPU_EGP_24_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_3 Register DESC2_CPU_EGP_24_3 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_3 0x28138
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_3 0x1E7E8138u

//! Register Reset Value
#define DESC2_CPU_EGP_24_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_3 Register DESC3_CPU_EGP_24_3 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_3 0x2813C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_3 0x1E7E813Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_4 Register DESC0_CPU_EGP_24_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_4 0x28140
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_4 0x1E7E8140u

//! Register Reset Value
#define DESC0_CPU_EGP_24_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_4 Register DESC1_CPU_EGP_24_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_4 0x28144
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_4 0x1E7E8144u

//! Register Reset Value
#define DESC1_CPU_EGP_24_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_4 Register DESC2_CPU_EGP_24_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_4 0x28148
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_4 0x1E7E8148u

//! Register Reset Value
#define DESC2_CPU_EGP_24_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_4 Register DESC3_CPU_EGP_24_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_4 0x2814C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_4 0x1E7E814Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_5 Register DESC0_CPU_EGP_24_5 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_5 0x28150
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_5 0x1E7E8150u

//! Register Reset Value
#define DESC0_CPU_EGP_24_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_5 Register DESC1_CPU_EGP_24_5 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_5 0x28154
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_5 0x1E7E8154u

//! Register Reset Value
#define DESC1_CPU_EGP_24_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_5 Register DESC2_CPU_EGP_24_5 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_5 0x28158
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_5 0x1E7E8158u

//! Register Reset Value
#define DESC2_CPU_EGP_24_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_5 Register DESC3_CPU_EGP_24_5 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_5 0x2815C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_5 0x1E7E815Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_6 Register DESC0_CPU_EGP_24_6 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_6 0x28160
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_6 0x1E7E8160u

//! Register Reset Value
#define DESC0_CPU_EGP_24_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_6 Register DESC1_CPU_EGP_24_6 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_6 0x28164
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_6 0x1E7E8164u

//! Register Reset Value
#define DESC1_CPU_EGP_24_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_6 Register DESC2_CPU_EGP_24_6 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_6 0x28168
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_6 0x1E7E8168u

//! Register Reset Value
#define DESC2_CPU_EGP_24_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_6 Register DESC3_CPU_EGP_24_6 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_6 0x2816C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_6 0x1E7E816Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_7 Register DESC0_CPU_EGP_24_7 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_7 0x28170
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_7 0x1E7E8170u

//! Register Reset Value
#define DESC0_CPU_EGP_24_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_7 Register DESC1_CPU_EGP_24_7 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_7 0x28174
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_7 0x1E7E8174u

//! Register Reset Value
#define DESC1_CPU_EGP_24_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_7 Register DESC2_CPU_EGP_24_7 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_7 0x28178
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_7 0x1E7E8178u

//! Register Reset Value
#define DESC2_CPU_EGP_24_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_7 Register DESC3_CPU_EGP_24_7 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_7 0x2817C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_7 0x1E7E817Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_8 Register DESC0_CPU_EGP_24_8 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_8 0x28180
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_8 0x1E7E8180u

//! Register Reset Value
#define DESC0_CPU_EGP_24_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_8 Register DESC1_CPU_EGP_24_8 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_8 0x28184
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_8 0x1E7E8184u

//! Register Reset Value
#define DESC1_CPU_EGP_24_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_8 Register DESC2_CPU_EGP_24_8 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_8 0x28188
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_8 0x1E7E8188u

//! Register Reset Value
#define DESC2_CPU_EGP_24_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_8 Register DESC3_CPU_EGP_24_8 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_8 0x2818C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_8 0x1E7E818Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_9 Register DESC0_CPU_EGP_24_9 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_9 0x28190
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_9 0x1E7E8190u

//! Register Reset Value
#define DESC0_CPU_EGP_24_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_9 Register DESC1_CPU_EGP_24_9 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_9 0x28194
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_9 0x1E7E8194u

//! Register Reset Value
#define DESC1_CPU_EGP_24_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_9 Register DESC2_CPU_EGP_24_9 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_9 0x28198
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_9 0x1E7E8198u

//! Register Reset Value
#define DESC2_CPU_EGP_24_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_9 Register DESC3_CPU_EGP_24_9 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_9 0x2819C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_9 0x1E7E819Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_10 Register DESC0_CPU_EGP_24_10 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_10 0x281A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_10 0x1E7E81A0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_10 Register DESC1_CPU_EGP_24_10 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_10 0x281A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_10 0x1E7E81A4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_10 Register DESC2_CPU_EGP_24_10 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_10 0x281A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_10 0x1E7E81A8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_10 Register DESC3_CPU_EGP_24_10 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_10 0x281AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_10 0x1E7E81ACu

//! Register Reset Value
#define DESC3_CPU_EGP_24_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_11 Register DESC0_CPU_EGP_24_11 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_11 0x281B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_11 0x1E7E81B0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_11 Register DESC1_CPU_EGP_24_11 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_11 0x281B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_11 0x1E7E81B4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_11 Register DESC2_CPU_EGP_24_11 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_11 0x281B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_11 0x1E7E81B8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_11 Register DESC3_CPU_EGP_24_11 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_11 0x281BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_11 0x1E7E81BCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_12 Register DESC0_CPU_EGP_24_12 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_12 0x281C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_12 0x1E7E81C0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_12 Register DESC1_CPU_EGP_24_12 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_12 0x281C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_12 0x1E7E81C4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_12 Register DESC2_CPU_EGP_24_12 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_12 0x281C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_12 0x1E7E81C8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_12 Register DESC3_CPU_EGP_24_12 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_12 0x281CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_12 0x1E7E81CCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_13 Register DESC0_CPU_EGP_24_13 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_13 0x281D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_13 0x1E7E81D0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_13 Register DESC1_CPU_EGP_24_13 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_13 0x281D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_13 0x1E7E81D4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_13 Register DESC2_CPU_EGP_24_13 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_13 0x281D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_13 0x1E7E81D8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_13 Register DESC3_CPU_EGP_24_13 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_13 0x281DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_13 0x1E7E81DCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_14 Register DESC0_CPU_EGP_24_14 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_14 0x281E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_14 0x1E7E81E0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_14 Register DESC1_CPU_EGP_24_14 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_14 0x281E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_14 0x1E7E81E4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_14 Register DESC2_CPU_EGP_24_14 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_14 0x281E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_14 0x1E7E81E8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_14 Register DESC3_CPU_EGP_24_14 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_14 0x281EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_14 0x1E7E81ECu

//! Register Reset Value
#define DESC3_CPU_EGP_24_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_15 Register DESC0_CPU_EGP_24_15 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_15 0x281F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_15 0x1E7E81F0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_15 Register DESC1_CPU_EGP_24_15 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_15 0x281F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_15 0x1E7E81F4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_15 Register DESC2_CPU_EGP_24_15 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_15 0x281F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_15 0x1E7E81F8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_15 Register DESC3_CPU_EGP_24_15 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_15 0x281FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_15 0x1E7E81FCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_16 Register DESC0_CPU_EGP_24_16 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_16 0x28200
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_16 0x1E7E8200u

//! Register Reset Value
#define DESC0_CPU_EGP_24_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_16 Register DESC1_CPU_EGP_24_16 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_16 0x28204
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_16 0x1E7E8204u

//! Register Reset Value
#define DESC1_CPU_EGP_24_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_16 Register DESC2_CPU_EGP_24_16 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_16 0x28208
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_16 0x1E7E8208u

//! Register Reset Value
#define DESC2_CPU_EGP_24_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_16 Register DESC3_CPU_EGP_24_16 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_16 0x2820C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_16 0x1E7E820Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_17 Register DESC0_CPU_EGP_24_17 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_17 0x28210
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_17 0x1E7E8210u

//! Register Reset Value
#define DESC0_CPU_EGP_24_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_17 Register DESC1_CPU_EGP_24_17 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_17 0x28214
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_17 0x1E7E8214u

//! Register Reset Value
#define DESC1_CPU_EGP_24_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_17 Register DESC2_CPU_EGP_24_17 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_17 0x28218
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_17 0x1E7E8218u

//! Register Reset Value
#define DESC2_CPU_EGP_24_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_17 Register DESC3_CPU_EGP_24_17 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_17 0x2821C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_17 0x1E7E821Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_18 Register DESC0_CPU_EGP_24_18 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_18 0x28220
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_18 0x1E7E8220u

//! Register Reset Value
#define DESC0_CPU_EGP_24_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_18 Register DESC1_CPU_EGP_24_18 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_18 0x28224
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_18 0x1E7E8224u

//! Register Reset Value
#define DESC1_CPU_EGP_24_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_18 Register DESC2_CPU_EGP_24_18 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_18 0x28228
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_18 0x1E7E8228u

//! Register Reset Value
#define DESC2_CPU_EGP_24_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_18 Register DESC3_CPU_EGP_24_18 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_18 0x2822C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_18 0x1E7E822Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_19 Register DESC0_CPU_EGP_24_19 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_19 0x28230
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_19 0x1E7E8230u

//! Register Reset Value
#define DESC0_CPU_EGP_24_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_19 Register DESC1_CPU_EGP_24_19 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_19 0x28234
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_19 0x1E7E8234u

//! Register Reset Value
#define DESC1_CPU_EGP_24_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_19 Register DESC2_CPU_EGP_24_19 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_19 0x28238
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_19 0x1E7E8238u

//! Register Reset Value
#define DESC2_CPU_EGP_24_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_19 Register DESC3_CPU_EGP_24_19 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_19 0x2823C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_19 0x1E7E823Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_20 Register DESC0_CPU_EGP_24_20 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_20 0x28240
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_20 0x1E7E8240u

//! Register Reset Value
#define DESC0_CPU_EGP_24_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_20 Register DESC1_CPU_EGP_24_20 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_20 0x28244
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_20 0x1E7E8244u

//! Register Reset Value
#define DESC1_CPU_EGP_24_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_20 Register DESC2_CPU_EGP_24_20 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_20 0x28248
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_20 0x1E7E8248u

//! Register Reset Value
#define DESC2_CPU_EGP_24_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_20 Register DESC3_CPU_EGP_24_20 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_20 0x2824C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_20 0x1E7E824Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_21 Register DESC0_CPU_EGP_24_21 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_21 0x28250
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_21 0x1E7E8250u

//! Register Reset Value
#define DESC0_CPU_EGP_24_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_21 Register DESC1_CPU_EGP_24_21 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_21 0x28254
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_21 0x1E7E8254u

//! Register Reset Value
#define DESC1_CPU_EGP_24_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_21 Register DESC2_CPU_EGP_24_21 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_21 0x28258
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_21 0x1E7E8258u

//! Register Reset Value
#define DESC2_CPU_EGP_24_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_21 Register DESC3_CPU_EGP_24_21 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_21 0x2825C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_21 0x1E7E825Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_22 Register DESC0_CPU_EGP_24_22 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_22 0x28260
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_22 0x1E7E8260u

//! Register Reset Value
#define DESC0_CPU_EGP_24_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_22 Register DESC1_CPU_EGP_24_22 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_22 0x28264
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_22 0x1E7E8264u

//! Register Reset Value
#define DESC1_CPU_EGP_24_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_22 Register DESC2_CPU_EGP_24_22 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_22 0x28268
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_22 0x1E7E8268u

//! Register Reset Value
#define DESC2_CPU_EGP_24_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_22 Register DESC3_CPU_EGP_24_22 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_22 0x2826C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_22 0x1E7E826Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_23 Register DESC0_CPU_EGP_24_23 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_23 0x28270
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_23 0x1E7E8270u

//! Register Reset Value
#define DESC0_CPU_EGP_24_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_23 Register DESC1_CPU_EGP_24_23 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_23 0x28274
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_23 0x1E7E8274u

//! Register Reset Value
#define DESC1_CPU_EGP_24_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_23 Register DESC2_CPU_EGP_24_23 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_23 0x28278
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_23 0x1E7E8278u

//! Register Reset Value
#define DESC2_CPU_EGP_24_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_23 Register DESC3_CPU_EGP_24_23 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_23 0x2827C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_23 0x1E7E827Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_24 Register DESC0_CPU_EGP_24_24 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_24 0x28280
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_24 0x1E7E8280u

//! Register Reset Value
#define DESC0_CPU_EGP_24_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_24 Register DESC1_CPU_EGP_24_24 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_24 0x28284
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_24 0x1E7E8284u

//! Register Reset Value
#define DESC1_CPU_EGP_24_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_24 Register DESC2_CPU_EGP_24_24 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_24 0x28288
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_24 0x1E7E8288u

//! Register Reset Value
#define DESC2_CPU_EGP_24_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_24 Register DESC3_CPU_EGP_24_24 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_24 0x2828C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_24 0x1E7E828Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_25 Register DESC0_CPU_EGP_24_25 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_25 0x28290
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_25 0x1E7E8290u

//! Register Reset Value
#define DESC0_CPU_EGP_24_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_25 Register DESC1_CPU_EGP_24_25 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_25 0x28294
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_25 0x1E7E8294u

//! Register Reset Value
#define DESC1_CPU_EGP_24_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_25 Register DESC2_CPU_EGP_24_25 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_25 0x28298
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_25 0x1E7E8298u

//! Register Reset Value
#define DESC2_CPU_EGP_24_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_25 Register DESC3_CPU_EGP_24_25 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_25 0x2829C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_25 0x1E7E829Cu

//! Register Reset Value
#define DESC3_CPU_EGP_24_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_26 Register DESC0_CPU_EGP_24_26 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_26 0x282A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_26 0x1E7E82A0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_26 Register DESC1_CPU_EGP_24_26 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_26 0x282A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_26 0x1E7E82A4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_26 Register DESC2_CPU_EGP_24_26 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_26 0x282A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_26 0x1E7E82A8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_26 Register DESC3_CPU_EGP_24_26 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_26 0x282AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_26 0x1E7E82ACu

//! Register Reset Value
#define DESC3_CPU_EGP_24_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_27 Register DESC0_CPU_EGP_24_27 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_27 0x282B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_27 0x1E7E82B0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_27 Register DESC1_CPU_EGP_24_27 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_27 0x282B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_27 0x1E7E82B4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_27 Register DESC2_CPU_EGP_24_27 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_27 0x282B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_27 0x1E7E82B8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_27 Register DESC3_CPU_EGP_24_27 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_27 0x282BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_27 0x1E7E82BCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_28 Register DESC0_CPU_EGP_24_28 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_28 0x282C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_28 0x1E7E82C0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_28 Register DESC1_CPU_EGP_24_28 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_28 0x282C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_28 0x1E7E82C4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_28 Register DESC2_CPU_EGP_24_28 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_28 0x282C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_28 0x1E7E82C8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_28 Register DESC3_CPU_EGP_24_28 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_28 0x282CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_28 0x1E7E82CCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_29 Register DESC0_CPU_EGP_24_29 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_29 0x282D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_29 0x1E7E82D0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_29 Register DESC1_CPU_EGP_24_29 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_29 0x282D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_29 0x1E7E82D4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_29 Register DESC2_CPU_EGP_24_29 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_29 0x282D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_29 0x1E7E82D8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_29 Register DESC3_CPU_EGP_24_29 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_29 0x282DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_29 0x1E7E82DCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_30 Register DESC0_CPU_EGP_24_30 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_30 0x282E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_30 0x1E7E82E0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_30 Register DESC1_CPU_EGP_24_30 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_30 0x282E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_30 0x1E7E82E4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_30 Register DESC2_CPU_EGP_24_30 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_30 0x282E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_30 0x1E7E82E8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_30 Register DESC3_CPU_EGP_24_30 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_30 0x282EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_30 0x1E7E82ECu

//! Register Reset Value
#define DESC3_CPU_EGP_24_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_24_31 Register DESC0_CPU_EGP_24_31 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_24_31 0x282F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_24_31 0x1E7E82F0u

//! Register Reset Value
#define DESC0_CPU_EGP_24_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_24_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_24_31 Register DESC1_CPU_EGP_24_31 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_24_31 0x282F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_24_31 0x1E7E82F4u

//! Register Reset Value
#define DESC1_CPU_EGP_24_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_24_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_24_31 Register DESC2_CPU_EGP_24_31 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_24_31 0x282F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_24_31 0x1E7E82F8u

//! Register Reset Value
#define DESC2_CPU_EGP_24_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_24_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_24_31 Register DESC3_CPU_EGP_24_31 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_24_31 0x282FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_24_31 0x1E7E82FCu

//! Register Reset Value
#define DESC3_CPU_EGP_24_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_24_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_25 Register CFG_CPU_EGP_25 - CPU WLAN Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_25 0x29000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_25 0x1E7E9000u

//! Register Reset Value
#define CFG_CPU_EGP_25_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_25_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_25_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_25_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_25_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_25_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_25_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_25_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_25_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_25_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_25_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_25_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_25_BFBPEN_EN 0x1

//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_25_DQBPEN_POS 3
//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_25_DQBPEN_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_25_DQBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_25_DQBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_25_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_25_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_25_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_25_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_25_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_25_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_25 Register DQPC_CPU_EGP_25 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_25 0x29004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_25 0x1E7E9004u

//! Register Reset Value
#define DQPC_CPU_EGP_25_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_25_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_25_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_25 Register IRNCR_CPU_EGP_25 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_25 0x29020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_25 0x1E7E9020u

//! Register Reset Value
#define IRNCR_CPU_EGP_25_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_25_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_25_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_25_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_25_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_25_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_25_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_25_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_25_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_25 Register IRNICR_CPU_EGP_25 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_25 0x29024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_25 0x1E7E9024u

//! Register Reset Value
#define IRNICR_CPU_EGP_25_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_25_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_25_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_25_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_25_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_25 Register IRNEN_CPU_EGP_25 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_25 0x29028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_25 0x1E7E9028u

//! Register Reset Value
#define IRNEN_CPU_EGP_25_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_25_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_25_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_25_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_25_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_25_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_25_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_25_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_25_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_25 Register DPTR_CPU_EGP_25 - Special CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_25 0x29030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_25 0x1E7E9030u

//! Register Reset Value
#define DPTR_CPU_EGP_25_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_25_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_25_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_25_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_25_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup BPRC_CPU_EGP_25 Register BPRC_CPU_EGP_25 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_25 0x29034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_25 0x1E7E9034u

//! Register Reset Value
#define BPRC_CPU_EGP_25_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_25_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_25_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BRPTR_SCPU_EGP_25 Register BRPTR_SCPU_EGP_25 - Special CPU Egress Port Buffer Return Pointer
//! @{

//! Register Offset (relative)
#define BRPTR_SCPU_EGP_25 0x29038
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BRPTR_SCPU_EGP_25 0x1E7E9038u

//! Register Reset Value
#define BRPTR_SCPU_EGP_25_RST 0x00000000u

//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_25_PTRBR_POS 0
//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_25_PTRBR_MASK 0x1Fu

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_0 Register PTR_RTN_CPU_EGP_25_0 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_0 0x29080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_0 0x1E7E9080u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_0_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_0_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_0_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_1 Register PTR_RTN_CPU_EGP_25_1 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_1 0x29084
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_1 0x1E7E9084u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_1_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_1_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_1_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_2 Register PTR_RTN_CPU_EGP_25_2 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_2 0x29088
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_2 0x1E7E9088u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_2_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_2_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_2_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_3 Register PTR_RTN_CPU_EGP_25_3 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_3 0x2908C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_3 0x1E7E908Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_3_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_3_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_3_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_4 Register PTR_RTN_CPU_EGP_25_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_4 0x29090
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_4 0x1E7E9090u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_5 Register PTR_RTN_CPU_EGP_25_5 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_5 0x29094
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_5 0x1E7E9094u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_5_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_5_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_5_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_6 Register PTR_RTN_CPU_EGP_25_6 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_6 0x29098
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_6 0x1E7E9098u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_6_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_6_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_6_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_7 Register PTR_RTN_CPU_EGP_25_7 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_7 0x2909C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_7 0x1E7E909Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_7_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_7_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_7_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_8 Register PTR_RTN_CPU_EGP_25_8 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_8 0x290A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_8 0x1E7E90A0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_8_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_8_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_8_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_9 Register PTR_RTN_CPU_EGP_25_9 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_9 0x290A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_9 0x1E7E90A4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_9_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_9_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_9_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_10 Register PTR_RTN_CPU_EGP_25_10 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_10 0x290A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_10 0x1E7E90A8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_10_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_10_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_10_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_11 Register PTR_RTN_CPU_EGP_25_11 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_11 0x290AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_11 0x1E7E90ACu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_11_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_11_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_11_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_12 Register PTR_RTN_CPU_EGP_25_12 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_12 0x290B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_12 0x1E7E90B0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_12_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_12_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_12_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_13 Register PTR_RTN_CPU_EGP_25_13 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_13 0x290B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_13 0x1E7E90B4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_13_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_13_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_13_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_14 Register PTR_RTN_CPU_EGP_25_14 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_14 0x290B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_14 0x1E7E90B8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_14_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_14_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_14_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_15 Register PTR_RTN_CPU_EGP_25_15 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_15 0x290BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_15 0x1E7E90BCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_15_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_15_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_15_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_16 Register PTR_RTN_CPU_EGP_25_16 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_16 0x290C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_16 0x1E7E90C0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_16_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_16_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_16_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_17 Register PTR_RTN_CPU_EGP_25_17 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_17 0x290C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_17 0x1E7E90C4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_17_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_17_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_17_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_18 Register PTR_RTN_CPU_EGP_25_18 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_18 0x290C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_18 0x1E7E90C8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_18_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_18_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_18_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_19 Register PTR_RTN_CPU_EGP_25_19 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_19 0x290CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_19 0x1E7E90CCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_19_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_19_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_19_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_20 Register PTR_RTN_CPU_EGP_25_20 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_20 0x290D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_20 0x1E7E90D0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_20_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_20_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_20_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_21 Register PTR_RTN_CPU_EGP_25_21 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_21 0x290D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_21 0x1E7E90D4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_21_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_21_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_21_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_22 Register PTR_RTN_CPU_EGP_25_22 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_22 0x290D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_22 0x1E7E90D8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_22_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_22_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_22_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_23 Register PTR_RTN_CPU_EGP_25_23 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_23 0x290DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_23 0x1E7E90DCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_23_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_23_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_23_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_24 Register PTR_RTN_CPU_EGP_25_24 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_24 0x290E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_24 0x1E7E90E0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_24_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_24_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_24_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_25 Register PTR_RTN_CPU_EGP_25_25 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_25 0x290E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_25 0x1E7E90E4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_25_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_25_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_25_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_26 Register PTR_RTN_CPU_EGP_25_26 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_26 0x290E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_26 0x1E7E90E8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_26_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_26_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_26_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_27 Register PTR_RTN_CPU_EGP_25_27 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_27 0x290EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_27 0x1E7E90ECu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_27_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_27_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_27_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_28 Register PTR_RTN_CPU_EGP_25_28 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_28 0x290F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_28 0x1E7E90F0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_28_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_28_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_28_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_29 Register PTR_RTN_CPU_EGP_25_29 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_29 0x290F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_29 0x1E7E90F4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_29_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_29_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_29_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_30 Register PTR_RTN_CPU_EGP_25_30 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_30 0x290F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_30 0x1E7E90F8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_30_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_30_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_30_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_25_31 Register PTR_RTN_CPU_EGP_25_31 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_25_31 0x290FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_25_31 0x1E7E90FCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_25_31_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_31_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_25_31_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_CPU_EGP_25_0 Register DESC0_CPU_EGP_25_0 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_0 0x29100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_0 0x1E7E9100u

//! Register Reset Value
#define DESC0_CPU_EGP_25_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_0 Register DESC1_CPU_EGP_25_0 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_0 0x29104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_0 0x1E7E9104u

//! Register Reset Value
#define DESC1_CPU_EGP_25_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_0 Register DESC2_CPU_EGP_25_0 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_0 0x29108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_0 0x1E7E9108u

//! Register Reset Value
#define DESC2_CPU_EGP_25_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_0 Register DESC3_CPU_EGP_25_0 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_0 0x2910C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_0 0x1E7E910Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_1 Register DESC0_CPU_EGP_25_1 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_1 0x29110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_1 0x1E7E9110u

//! Register Reset Value
#define DESC0_CPU_EGP_25_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_1 Register DESC1_CPU_EGP_25_1 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_1 0x29114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_1 0x1E7E9114u

//! Register Reset Value
#define DESC1_CPU_EGP_25_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_1 Register DESC2_CPU_EGP_25_1 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_1 0x29118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_1 0x1E7E9118u

//! Register Reset Value
#define DESC2_CPU_EGP_25_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_1 Register DESC3_CPU_EGP_25_1 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_1 0x2911C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_1 0x1E7E911Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_2 Register DESC0_CPU_EGP_25_2 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_2 0x29120
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_2 0x1E7E9120u

//! Register Reset Value
#define DESC0_CPU_EGP_25_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_2 Register DESC1_CPU_EGP_25_2 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_2 0x29124
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_2 0x1E7E9124u

//! Register Reset Value
#define DESC1_CPU_EGP_25_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_2 Register DESC2_CPU_EGP_25_2 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_2 0x29128
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_2 0x1E7E9128u

//! Register Reset Value
#define DESC2_CPU_EGP_25_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_2 Register DESC3_CPU_EGP_25_2 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_2 0x2912C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_2 0x1E7E912Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_3 Register DESC0_CPU_EGP_25_3 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_3 0x29130
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_3 0x1E7E9130u

//! Register Reset Value
#define DESC0_CPU_EGP_25_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_3 Register DESC1_CPU_EGP_25_3 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_3 0x29134
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_3 0x1E7E9134u

//! Register Reset Value
#define DESC1_CPU_EGP_25_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_3 Register DESC2_CPU_EGP_25_3 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_3 0x29138
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_3 0x1E7E9138u

//! Register Reset Value
#define DESC2_CPU_EGP_25_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_3 Register DESC3_CPU_EGP_25_3 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_3 0x2913C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_3 0x1E7E913Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_4 Register DESC0_CPU_EGP_25_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_4 0x29140
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_4 0x1E7E9140u

//! Register Reset Value
#define DESC0_CPU_EGP_25_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_4 Register DESC1_CPU_EGP_25_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_4 0x29144
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_4 0x1E7E9144u

//! Register Reset Value
#define DESC1_CPU_EGP_25_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_4 Register DESC2_CPU_EGP_25_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_4 0x29148
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_4 0x1E7E9148u

//! Register Reset Value
#define DESC2_CPU_EGP_25_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_4 Register DESC3_CPU_EGP_25_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_4 0x2914C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_4 0x1E7E914Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_5 Register DESC0_CPU_EGP_25_5 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_5 0x29150
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_5 0x1E7E9150u

//! Register Reset Value
#define DESC0_CPU_EGP_25_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_5 Register DESC1_CPU_EGP_25_5 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_5 0x29154
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_5 0x1E7E9154u

//! Register Reset Value
#define DESC1_CPU_EGP_25_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_5 Register DESC2_CPU_EGP_25_5 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_5 0x29158
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_5 0x1E7E9158u

//! Register Reset Value
#define DESC2_CPU_EGP_25_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_5 Register DESC3_CPU_EGP_25_5 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_5 0x2915C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_5 0x1E7E915Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_6 Register DESC0_CPU_EGP_25_6 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_6 0x29160
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_6 0x1E7E9160u

//! Register Reset Value
#define DESC0_CPU_EGP_25_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_6 Register DESC1_CPU_EGP_25_6 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_6 0x29164
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_6 0x1E7E9164u

//! Register Reset Value
#define DESC1_CPU_EGP_25_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_6 Register DESC2_CPU_EGP_25_6 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_6 0x29168
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_6 0x1E7E9168u

//! Register Reset Value
#define DESC2_CPU_EGP_25_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_6 Register DESC3_CPU_EGP_25_6 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_6 0x2916C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_6 0x1E7E916Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_7 Register DESC0_CPU_EGP_25_7 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_7 0x29170
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_7 0x1E7E9170u

//! Register Reset Value
#define DESC0_CPU_EGP_25_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_7 Register DESC1_CPU_EGP_25_7 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_7 0x29174
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_7 0x1E7E9174u

//! Register Reset Value
#define DESC1_CPU_EGP_25_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_7 Register DESC2_CPU_EGP_25_7 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_7 0x29178
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_7 0x1E7E9178u

//! Register Reset Value
#define DESC2_CPU_EGP_25_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_7 Register DESC3_CPU_EGP_25_7 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_7 0x2917C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_7 0x1E7E917Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_8 Register DESC0_CPU_EGP_25_8 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_8 0x29180
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_8 0x1E7E9180u

//! Register Reset Value
#define DESC0_CPU_EGP_25_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_8 Register DESC1_CPU_EGP_25_8 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_8 0x29184
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_8 0x1E7E9184u

//! Register Reset Value
#define DESC1_CPU_EGP_25_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_8 Register DESC2_CPU_EGP_25_8 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_8 0x29188
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_8 0x1E7E9188u

//! Register Reset Value
#define DESC2_CPU_EGP_25_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_8 Register DESC3_CPU_EGP_25_8 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_8 0x2918C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_8 0x1E7E918Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_9 Register DESC0_CPU_EGP_25_9 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_9 0x29190
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_9 0x1E7E9190u

//! Register Reset Value
#define DESC0_CPU_EGP_25_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_9 Register DESC1_CPU_EGP_25_9 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_9 0x29194
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_9 0x1E7E9194u

//! Register Reset Value
#define DESC1_CPU_EGP_25_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_9 Register DESC2_CPU_EGP_25_9 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_9 0x29198
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_9 0x1E7E9198u

//! Register Reset Value
#define DESC2_CPU_EGP_25_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_9 Register DESC3_CPU_EGP_25_9 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_9 0x2919C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_9 0x1E7E919Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_10 Register DESC0_CPU_EGP_25_10 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_10 0x291A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_10 0x1E7E91A0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_10 Register DESC1_CPU_EGP_25_10 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_10 0x291A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_10 0x1E7E91A4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_10 Register DESC2_CPU_EGP_25_10 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_10 0x291A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_10 0x1E7E91A8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_10 Register DESC3_CPU_EGP_25_10 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_10 0x291AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_10 0x1E7E91ACu

//! Register Reset Value
#define DESC3_CPU_EGP_25_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_11 Register DESC0_CPU_EGP_25_11 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_11 0x291B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_11 0x1E7E91B0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_11 Register DESC1_CPU_EGP_25_11 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_11 0x291B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_11 0x1E7E91B4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_11 Register DESC2_CPU_EGP_25_11 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_11 0x291B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_11 0x1E7E91B8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_11 Register DESC3_CPU_EGP_25_11 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_11 0x291BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_11 0x1E7E91BCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_12 Register DESC0_CPU_EGP_25_12 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_12 0x291C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_12 0x1E7E91C0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_12 Register DESC1_CPU_EGP_25_12 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_12 0x291C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_12 0x1E7E91C4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_12 Register DESC2_CPU_EGP_25_12 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_12 0x291C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_12 0x1E7E91C8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_12 Register DESC3_CPU_EGP_25_12 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_12 0x291CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_12 0x1E7E91CCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_13 Register DESC0_CPU_EGP_25_13 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_13 0x291D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_13 0x1E7E91D0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_13 Register DESC1_CPU_EGP_25_13 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_13 0x291D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_13 0x1E7E91D4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_13 Register DESC2_CPU_EGP_25_13 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_13 0x291D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_13 0x1E7E91D8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_13 Register DESC3_CPU_EGP_25_13 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_13 0x291DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_13 0x1E7E91DCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_14 Register DESC0_CPU_EGP_25_14 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_14 0x291E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_14 0x1E7E91E0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_14 Register DESC1_CPU_EGP_25_14 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_14 0x291E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_14 0x1E7E91E4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_14 Register DESC2_CPU_EGP_25_14 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_14 0x291E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_14 0x1E7E91E8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_14 Register DESC3_CPU_EGP_25_14 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_14 0x291EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_14 0x1E7E91ECu

//! Register Reset Value
#define DESC3_CPU_EGP_25_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_15 Register DESC0_CPU_EGP_25_15 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_15 0x291F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_15 0x1E7E91F0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_15 Register DESC1_CPU_EGP_25_15 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_15 0x291F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_15 0x1E7E91F4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_15 Register DESC2_CPU_EGP_25_15 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_15 0x291F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_15 0x1E7E91F8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_15 Register DESC3_CPU_EGP_25_15 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_15 0x291FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_15 0x1E7E91FCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_16 Register DESC0_CPU_EGP_25_16 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_16 0x29200
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_16 0x1E7E9200u

//! Register Reset Value
#define DESC0_CPU_EGP_25_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_16 Register DESC1_CPU_EGP_25_16 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_16 0x29204
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_16 0x1E7E9204u

//! Register Reset Value
#define DESC1_CPU_EGP_25_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_16 Register DESC2_CPU_EGP_25_16 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_16 0x29208
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_16 0x1E7E9208u

//! Register Reset Value
#define DESC2_CPU_EGP_25_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_16 Register DESC3_CPU_EGP_25_16 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_16 0x2920C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_16 0x1E7E920Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_17 Register DESC0_CPU_EGP_25_17 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_17 0x29210
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_17 0x1E7E9210u

//! Register Reset Value
#define DESC0_CPU_EGP_25_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_17 Register DESC1_CPU_EGP_25_17 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_17 0x29214
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_17 0x1E7E9214u

//! Register Reset Value
#define DESC1_CPU_EGP_25_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_17 Register DESC2_CPU_EGP_25_17 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_17 0x29218
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_17 0x1E7E9218u

//! Register Reset Value
#define DESC2_CPU_EGP_25_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_17 Register DESC3_CPU_EGP_25_17 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_17 0x2921C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_17 0x1E7E921Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_18 Register DESC0_CPU_EGP_25_18 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_18 0x29220
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_18 0x1E7E9220u

//! Register Reset Value
#define DESC0_CPU_EGP_25_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_18 Register DESC1_CPU_EGP_25_18 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_18 0x29224
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_18 0x1E7E9224u

//! Register Reset Value
#define DESC1_CPU_EGP_25_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_18 Register DESC2_CPU_EGP_25_18 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_18 0x29228
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_18 0x1E7E9228u

//! Register Reset Value
#define DESC2_CPU_EGP_25_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_18 Register DESC3_CPU_EGP_25_18 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_18 0x2922C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_18 0x1E7E922Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_19 Register DESC0_CPU_EGP_25_19 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_19 0x29230
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_19 0x1E7E9230u

//! Register Reset Value
#define DESC0_CPU_EGP_25_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_19 Register DESC1_CPU_EGP_25_19 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_19 0x29234
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_19 0x1E7E9234u

//! Register Reset Value
#define DESC1_CPU_EGP_25_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_19 Register DESC2_CPU_EGP_25_19 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_19 0x29238
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_19 0x1E7E9238u

//! Register Reset Value
#define DESC2_CPU_EGP_25_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_19 Register DESC3_CPU_EGP_25_19 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_19 0x2923C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_19 0x1E7E923Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_20 Register DESC0_CPU_EGP_25_20 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_20 0x29240
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_20 0x1E7E9240u

//! Register Reset Value
#define DESC0_CPU_EGP_25_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_20 Register DESC1_CPU_EGP_25_20 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_20 0x29244
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_20 0x1E7E9244u

//! Register Reset Value
#define DESC1_CPU_EGP_25_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_20 Register DESC2_CPU_EGP_25_20 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_20 0x29248
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_20 0x1E7E9248u

//! Register Reset Value
#define DESC2_CPU_EGP_25_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_20 Register DESC3_CPU_EGP_25_20 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_20 0x2924C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_20 0x1E7E924Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_21 Register DESC0_CPU_EGP_25_21 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_21 0x29250
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_21 0x1E7E9250u

//! Register Reset Value
#define DESC0_CPU_EGP_25_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_21 Register DESC1_CPU_EGP_25_21 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_21 0x29254
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_21 0x1E7E9254u

//! Register Reset Value
#define DESC1_CPU_EGP_25_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_21 Register DESC2_CPU_EGP_25_21 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_21 0x29258
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_21 0x1E7E9258u

//! Register Reset Value
#define DESC2_CPU_EGP_25_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_21 Register DESC3_CPU_EGP_25_21 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_21 0x2925C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_21 0x1E7E925Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_22 Register DESC0_CPU_EGP_25_22 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_22 0x29260
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_22 0x1E7E9260u

//! Register Reset Value
#define DESC0_CPU_EGP_25_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_22 Register DESC1_CPU_EGP_25_22 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_22 0x29264
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_22 0x1E7E9264u

//! Register Reset Value
#define DESC1_CPU_EGP_25_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_22 Register DESC2_CPU_EGP_25_22 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_22 0x29268
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_22 0x1E7E9268u

//! Register Reset Value
#define DESC2_CPU_EGP_25_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_22 Register DESC3_CPU_EGP_25_22 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_22 0x2926C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_22 0x1E7E926Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_23 Register DESC0_CPU_EGP_25_23 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_23 0x29270
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_23 0x1E7E9270u

//! Register Reset Value
#define DESC0_CPU_EGP_25_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_23 Register DESC1_CPU_EGP_25_23 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_23 0x29274
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_23 0x1E7E9274u

//! Register Reset Value
#define DESC1_CPU_EGP_25_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_23 Register DESC2_CPU_EGP_25_23 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_23 0x29278
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_23 0x1E7E9278u

//! Register Reset Value
#define DESC2_CPU_EGP_25_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_23 Register DESC3_CPU_EGP_25_23 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_23 0x2927C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_23 0x1E7E927Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_24 Register DESC0_CPU_EGP_25_24 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_24 0x29280
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_24 0x1E7E9280u

//! Register Reset Value
#define DESC0_CPU_EGP_25_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_24 Register DESC1_CPU_EGP_25_24 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_24 0x29284
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_24 0x1E7E9284u

//! Register Reset Value
#define DESC1_CPU_EGP_25_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_24 Register DESC2_CPU_EGP_25_24 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_24 0x29288
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_24 0x1E7E9288u

//! Register Reset Value
#define DESC2_CPU_EGP_25_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_24 Register DESC3_CPU_EGP_25_24 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_24 0x2928C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_24 0x1E7E928Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_25 Register DESC0_CPU_EGP_25_25 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_25 0x29290
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_25 0x1E7E9290u

//! Register Reset Value
#define DESC0_CPU_EGP_25_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_25 Register DESC1_CPU_EGP_25_25 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_25 0x29294
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_25 0x1E7E9294u

//! Register Reset Value
#define DESC1_CPU_EGP_25_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_25 Register DESC2_CPU_EGP_25_25 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_25 0x29298
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_25 0x1E7E9298u

//! Register Reset Value
#define DESC2_CPU_EGP_25_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_25 Register DESC3_CPU_EGP_25_25 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_25 0x2929C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_25 0x1E7E929Cu

//! Register Reset Value
#define DESC3_CPU_EGP_25_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_26 Register DESC0_CPU_EGP_25_26 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_26 0x292A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_26 0x1E7E92A0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_26 Register DESC1_CPU_EGP_25_26 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_26 0x292A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_26 0x1E7E92A4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_26 Register DESC2_CPU_EGP_25_26 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_26 0x292A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_26 0x1E7E92A8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_26 Register DESC3_CPU_EGP_25_26 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_26 0x292AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_26 0x1E7E92ACu

//! Register Reset Value
#define DESC3_CPU_EGP_25_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_27 Register DESC0_CPU_EGP_25_27 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_27 0x292B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_27 0x1E7E92B0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_27 Register DESC1_CPU_EGP_25_27 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_27 0x292B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_27 0x1E7E92B4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_27 Register DESC2_CPU_EGP_25_27 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_27 0x292B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_27 0x1E7E92B8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_27 Register DESC3_CPU_EGP_25_27 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_27 0x292BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_27 0x1E7E92BCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_28 Register DESC0_CPU_EGP_25_28 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_28 0x292C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_28 0x1E7E92C0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_28 Register DESC1_CPU_EGP_25_28 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_28 0x292C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_28 0x1E7E92C4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_28 Register DESC2_CPU_EGP_25_28 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_28 0x292C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_28 0x1E7E92C8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_28 Register DESC3_CPU_EGP_25_28 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_28 0x292CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_28 0x1E7E92CCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_29 Register DESC0_CPU_EGP_25_29 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_29 0x292D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_29 0x1E7E92D0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_29 Register DESC1_CPU_EGP_25_29 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_29 0x292D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_29 0x1E7E92D4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_29 Register DESC2_CPU_EGP_25_29 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_29 0x292D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_29 0x1E7E92D8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_29 Register DESC3_CPU_EGP_25_29 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_29 0x292DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_29 0x1E7E92DCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_30 Register DESC0_CPU_EGP_25_30 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_30 0x292E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_30 0x1E7E92E0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_30 Register DESC1_CPU_EGP_25_30 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_30 0x292E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_30 0x1E7E92E4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_30 Register DESC2_CPU_EGP_25_30 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_30 0x292E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_30 0x1E7E92E8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_30 Register DESC3_CPU_EGP_25_30 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_30 0x292EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_30 0x1E7E92ECu

//! Register Reset Value
#define DESC3_CPU_EGP_25_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_25_31 Register DESC0_CPU_EGP_25_31 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_25_31 0x292F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_25_31 0x1E7E92F0u

//! Register Reset Value
#define DESC0_CPU_EGP_25_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_25_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_25_31 Register DESC1_CPU_EGP_25_31 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_25_31 0x292F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_25_31 0x1E7E92F4u

//! Register Reset Value
#define DESC1_CPU_EGP_25_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_25_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_25_31 Register DESC2_CPU_EGP_25_31 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_25_31 0x292F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_25_31 0x1E7E92F8u

//! Register Reset Value
#define DESC2_CPU_EGP_25_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_25_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_25_31 Register DESC3_CPU_EGP_25_31 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_25_31 0x292FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_25_31 0x1E7E92FCu

//! Register Reset Value
#define DESC3_CPU_EGP_25_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_25_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CFG_CPU_EGP_26 Register CFG_CPU_EGP_26 - CPU WLAN Egress Port Configuration
//! @{

//! Register Offset (relative)
#define CFG_CPU_EGP_26 0x2A000
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_CFG_CPU_EGP_26 0x1E7EA000u

//! Register Reset Value
#define CFG_CPU_EGP_26_RST 0x00000000u

//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_26_DQREQ_POS 0
//! Field DQREQ - Enable CPU Dequeue Request
#define CFG_CPU_EGP_26_DQREQ_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_26_DQREQ_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_26_DQREQ_EN 0x1

//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_26_BUFRTN_POS 1
//! Field BUFRTN - Enable CPU Buffer Return
#define CFG_CPU_EGP_26_BUFRTN_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_26_BUFRTN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_26_BUFRTN_EN 0x1

//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_26_BFBPEN_POS 2
//! Field BFBPEN - Buffer Return Back Pressure Enable
#define CFG_CPU_EGP_26_BFBPEN_MASK 0x4u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_26_BFBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_26_BFBPEN_EN 0x1

//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_26_DQBPEN_POS 3
//! Field DQBPEN - Dequeue Back Pressure Enable
#define CFG_CPU_EGP_26_DQBPEN_MASK 0x8u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_26_DQBPEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_26_DQBPEN_EN 0x1

//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_26_DQPCEN_POS 8
//! Field DQPCEN - Dequeue Counter Enable
#define CFG_CPU_EGP_26_DQPCEN_MASK 0x100u
//! Constant DIS - DIS
#define CONST_CFG_CPU_EGP_26_DQPCEN_DIS 0x0
//! Constant EN - EN
#define CONST_CFG_CPU_EGP_26_DQPCEN_EN 0x1

//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_26_EPMAP_POS 16
//! Field EPMAP - Egress port mapping
#define CFG_CPU_EGP_26_EPMAP_MASK 0x7F0000u

//! @}

//! \defgroup DQPC_CPU_EGP_26 Register DQPC_CPU_EGP_26 - CPU Egress Port dequeue packet counter
//! @{

//! Register Offset (relative)
#define DQPC_CPU_EGP_26 0x2A004
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DQPC_CPU_EGP_26 0x1E7EA004u

//! Register Reset Value
#define DQPC_CPU_EGP_26_RST 0x00000000u

//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_26_PC_POS 0
//! Field PC - Per Port Packet Counter
#define DQPC_CPU_EGP_26_PC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup IRNCR_CPU_EGP_26 Register IRNCR_CPU_EGP_26 - CPU Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IRNCR_CPU_EGP_26 0x2A020
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNCR_CPU_EGP_26 0x1E7EA020u

//! Register Reset Value
#define IRNCR_CPU_EGP_26_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_26_PR_POS 0
//! Field PR - Pointer Ready
#define IRNCR_CPU_EGP_26_PR_MASK 0x1u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_26_PR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_26_PR_INTOCC 0x1

//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_26_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNCR_CPU_EGP_26_DR_MASK 0x2u
//! Constant NUL - NULL
#define CONST_IRNCR_CPU_EGP_26_DR_NUL 0x0
//! Constant INTOCC - INTOCC
#define CONST_IRNCR_CPU_EGP_26_DR_INTOCC 0x1

//! @}

//! \defgroup IRNICR_CPU_EGP_26 Register IRNICR_CPU_EGP_26 - CPU Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IRNICR_CPU_EGP_26 0x2A024
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNICR_CPU_EGP_26 0x1E7EA024u

//! Register Reset Value
#define IRNICR_CPU_EGP_26_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_26_PR_POS 0
//! Field PR - Pointer Ready
#define IRNICR_CPU_EGP_26_PR_MASK 0x1u

//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_26_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNICR_CPU_EGP_26_DR_MASK 0x2u

//! @}

//! \defgroup IRNEN_CPU_EGP_26 Register IRNEN_CPU_EGP_26 - CPU Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IRNEN_CPU_EGP_26 0x2A028
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_IRNEN_CPU_EGP_26 0x1E7EA028u

//! Register Reset Value
#define IRNEN_CPU_EGP_26_RST 0x00000000u

//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_26_PR_POS 0
//! Field PR - Pointer Ready
#define IRNEN_CPU_EGP_26_PR_MASK 0x1u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_26_PR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_26_PR_EN 0x1

//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_26_DR_POS 1
//! Field DR - Descriptor Ready
#define IRNEN_CPU_EGP_26_DR_MASK 0x2u
//! Constant DIS - DIS
#define CONST_IRNEN_CPU_EGP_26_DR_DIS 0x0
//! Constant EN - EN
#define CONST_IRNEN_CPU_EGP_26_DR_EN 0x1

//! @}

//! \defgroup DPTR_CPU_EGP_26 Register DPTR_CPU_EGP_26 - Special CPU Egress Port Descriptor Pointer
//! @{

//! Register Offset (relative)
#define DPTR_CPU_EGP_26 0x2A030
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DPTR_CPU_EGP_26 0x1E7EA030u

//! Register Reset Value
#define DPTR_CPU_EGP_26_RST 0x0000001Fu

//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_26_ND_POS 0
//! Field ND - Number of Descriptors
#define DPTR_CPU_EGP_26_ND_MASK 0x1Fu

//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_26_DPTR_POS 16
//! Field DPTR - Descriptor Pointer
#define DPTR_CPU_EGP_26_DPTR_MASK 0x1F0000u

//! @}

//! \defgroup BPRC_CPU_EGP_26 Register BPRC_CPU_EGP_26 - CPU Egress Port Buffer Pointer Return counter
//! @{

//! Register Offset (relative)
#define BPRC_CPU_EGP_26 0x2A034
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BPRC_CPU_EGP_26 0x1E7EA034u

//! Register Reset Value
#define BPRC_CPU_EGP_26_RST 0x00000000u

//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_26_BPRC_POS 0
//! Field BPRC - Per Port Buffer Pointer Return Counter
#define BPRC_CPU_EGP_26_BPRC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BRPTR_SCPU_EGP_26 Register BRPTR_SCPU_EGP_26 - Special CPU Egress Port Buffer Return Pointer
//! @{

//! Register Offset (relative)
#define BRPTR_SCPU_EGP_26 0x2A038
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_BRPTR_SCPU_EGP_26 0x1E7EA038u

//! Register Reset Value
#define BRPTR_SCPU_EGP_26_RST 0x00000000u

//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_26_PTRBR_POS 0
//! Field PTRBR - Pointer of Buffer Return
#define BRPTR_SCPU_EGP_26_PTRBR_MASK 0x1Fu

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_0 Register PTR_RTN_CPU_EGP_26_0 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_0 0x2A080
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_0 0x1E7EA080u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_0_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_0_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_0_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_1 Register PTR_RTN_CPU_EGP_26_1 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_1 0x2A084
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_1 0x1E7EA084u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_1_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_1_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_1_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_2 Register PTR_RTN_CPU_EGP_26_2 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_2 0x2A088
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_2 0x1E7EA088u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_2_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_2_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_2_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_3 Register PTR_RTN_CPU_EGP_26_3 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_3 0x2A08C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_3 0x1E7EA08Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_3_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_3_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_3_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_4 Register PTR_RTN_CPU_EGP_26_4 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_4 0x2A090
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_4 0x1E7EA090u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_4_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_4_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_4_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_5 Register PTR_RTN_CPU_EGP_26_5 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_5 0x2A094
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_5 0x1E7EA094u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_5_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_5_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_5_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_6 Register PTR_RTN_CPU_EGP_26_6 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_6 0x2A098
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_6 0x1E7EA098u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_6_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_6_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_6_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_7 Register PTR_RTN_CPU_EGP_26_7 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_7 0x2A09C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_7 0x1E7EA09Cu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_7_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_7_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_7_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_8 Register PTR_RTN_CPU_EGP_26_8 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_8 0x2A0A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_8 0x1E7EA0A0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_8_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_8_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_8_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_9 Register PTR_RTN_CPU_EGP_26_9 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_9 0x2A0A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_9 0x1E7EA0A4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_9_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_9_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_9_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_10 Register PTR_RTN_CPU_EGP_26_10 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_10 0x2A0A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_10 0x1E7EA0A8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_10_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_10_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_10_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_11 Register PTR_RTN_CPU_EGP_26_11 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_11 0x2A0AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_11 0x1E7EA0ACu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_11_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_11_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_11_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_12 Register PTR_RTN_CPU_EGP_26_12 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_12 0x2A0B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_12 0x1E7EA0B0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_12_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_12_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_12_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_13 Register PTR_RTN_CPU_EGP_26_13 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_13 0x2A0B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_13 0x1E7EA0B4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_13_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_13_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_13_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_14 Register PTR_RTN_CPU_EGP_26_14 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_14 0x2A0B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_14 0x1E7EA0B8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_14_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_14_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_14_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_15 Register PTR_RTN_CPU_EGP_26_15 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_15 0x2A0BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_15 0x1E7EA0BCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_15_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_15_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_15_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_16 Register PTR_RTN_CPU_EGP_26_16 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_16 0x2A0C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_16 0x1E7EA0C0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_16_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_16_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_16_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_17 Register PTR_RTN_CPU_EGP_26_17 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_17 0x2A0C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_17 0x1E7EA0C4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_17_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_17_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_17_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_18 Register PTR_RTN_CPU_EGP_26_18 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_18 0x2A0C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_18 0x1E7EA0C8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_18_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_18_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_18_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_19 Register PTR_RTN_CPU_EGP_26_19 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_19 0x2A0CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_19 0x1E7EA0CCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_19_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_19_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_19_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_20 Register PTR_RTN_CPU_EGP_26_20 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_20 0x2A0D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_20 0x1E7EA0D0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_20_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_20_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_20_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_21 Register PTR_RTN_CPU_EGP_26_21 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_21 0x2A0D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_21 0x1E7EA0D4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_21_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_21_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_21_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_22 Register PTR_RTN_CPU_EGP_26_22 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_22 0x2A0D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_22 0x1E7EA0D8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_22_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_22_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_22_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_23 Register PTR_RTN_CPU_EGP_26_23 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_23 0x2A0DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_23 0x1E7EA0DCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_23_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_23_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_23_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_24 Register PTR_RTN_CPU_EGP_26_24 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_24 0x2A0E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_24 0x1E7EA0E0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_24_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_24_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_24_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_25 Register PTR_RTN_CPU_EGP_26_25 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_25 0x2A0E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_25 0x1E7EA0E4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_25_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_25_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_25_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_26 Register PTR_RTN_CPU_EGP_26_26 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_26 0x2A0E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_26 0x1E7EA0E8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_26_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_26_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_26_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_27 Register PTR_RTN_CPU_EGP_26_27 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_27 0x2A0EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_27 0x1E7EA0ECu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_27_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_27_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_27_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_28 Register PTR_RTN_CPU_EGP_26_28 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_28 0x2A0F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_28 0x1E7EA0F0u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_28_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_28_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_28_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_29 Register PTR_RTN_CPU_EGP_26_29 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_29 0x2A0F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_29 0x1E7EA0F4u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_29_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_29_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_29_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_30 Register PTR_RTN_CPU_EGP_26_30 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_30 0x2A0F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_30 0x1E7EA0F8u

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_30_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_30_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_30_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup PTR_RTN_CPU_EGP_26_31 Register PTR_RTN_CPU_EGP_26_31 - CPU Egress Port Buffer Pointer Return
//! @{

//! Register Offset (relative)
#define PTR_RTN_CPU_EGP_26_31 0x2A0FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_PTR_RTN_CPU_EGP_26_31 0x1E7EA0FCu

//! Register Reset Value
#define PTR_RTN_CPU_EGP_26_31_RST 0xFFFFF800u

//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_31_BPTR_POS 11
//! Field BPTR - Buffer Pointer
#define PTR_RTN_CPU_EGP_26_31_BPTR_MASK 0xFFFFF800u

//! @}

//! \defgroup DESC0_CPU_EGP_26_0 Register DESC0_CPU_EGP_26_0 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_0 0x2A100
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_0 0x1E7EA100u

//! Register Reset Value
#define DESC0_CPU_EGP_26_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_0 Register DESC1_CPU_EGP_26_0 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_0 0x2A104
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_0 0x1E7EA104u

//! Register Reset Value
#define DESC1_CPU_EGP_26_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_0 Register DESC2_CPU_EGP_26_0 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_0 0x2A108
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_0 0x1E7EA108u

//! Register Reset Value
#define DESC2_CPU_EGP_26_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_0 Register DESC3_CPU_EGP_26_0 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_0 0x2A10C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_0 0x1E7EA10Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_0_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_0_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_0_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_1 Register DESC0_CPU_EGP_26_1 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_1 0x2A110
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_1 0x1E7EA110u

//! Register Reset Value
#define DESC0_CPU_EGP_26_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_1 Register DESC1_CPU_EGP_26_1 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_1 0x2A114
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_1 0x1E7EA114u

//! Register Reset Value
#define DESC1_CPU_EGP_26_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_1 Register DESC2_CPU_EGP_26_1 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_1 0x2A118
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_1 0x1E7EA118u

//! Register Reset Value
#define DESC2_CPU_EGP_26_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_1 Register DESC3_CPU_EGP_26_1 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_1 0x2A11C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_1 0x1E7EA11Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_1_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_1_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_1_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_2 Register DESC0_CPU_EGP_26_2 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_2 0x2A120
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_2 0x1E7EA120u

//! Register Reset Value
#define DESC0_CPU_EGP_26_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_2 Register DESC1_CPU_EGP_26_2 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_2 0x2A124
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_2 0x1E7EA124u

//! Register Reset Value
#define DESC1_CPU_EGP_26_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_2 Register DESC2_CPU_EGP_26_2 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_2 0x2A128
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_2 0x1E7EA128u

//! Register Reset Value
#define DESC2_CPU_EGP_26_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_2 Register DESC3_CPU_EGP_26_2 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_2 0x2A12C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_2 0x1E7EA12Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_2_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_2_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_2_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_3 Register DESC0_CPU_EGP_26_3 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_3 0x2A130
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_3 0x1E7EA130u

//! Register Reset Value
#define DESC0_CPU_EGP_26_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_3 Register DESC1_CPU_EGP_26_3 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_3 0x2A134
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_3 0x1E7EA134u

//! Register Reset Value
#define DESC1_CPU_EGP_26_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_3 Register DESC2_CPU_EGP_26_3 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_3 0x2A138
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_3 0x1E7EA138u

//! Register Reset Value
#define DESC2_CPU_EGP_26_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_3 Register DESC3_CPU_EGP_26_3 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_3 0x2A13C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_3 0x1E7EA13Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_3_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_3_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_3_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_4 Register DESC0_CPU_EGP_26_4 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_4 0x2A140
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_4 0x1E7EA140u

//! Register Reset Value
#define DESC0_CPU_EGP_26_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_4 Register DESC1_CPU_EGP_26_4 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_4 0x2A144
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_4 0x1E7EA144u

//! Register Reset Value
#define DESC1_CPU_EGP_26_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_4 Register DESC2_CPU_EGP_26_4 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_4 0x2A148
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_4 0x1E7EA148u

//! Register Reset Value
#define DESC2_CPU_EGP_26_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_4 Register DESC3_CPU_EGP_26_4 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_4 0x2A14C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_4 0x1E7EA14Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_4_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_4_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_4_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_5 Register DESC0_CPU_EGP_26_5 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_5 0x2A150
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_5 0x1E7EA150u

//! Register Reset Value
#define DESC0_CPU_EGP_26_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_5 Register DESC1_CPU_EGP_26_5 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_5 0x2A154
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_5 0x1E7EA154u

//! Register Reset Value
#define DESC1_CPU_EGP_26_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_5 Register DESC2_CPU_EGP_26_5 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_5 0x2A158
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_5 0x1E7EA158u

//! Register Reset Value
#define DESC2_CPU_EGP_26_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_5 Register DESC3_CPU_EGP_26_5 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_5 0x2A15C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_5 0x1E7EA15Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_5_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_5_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_5_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_6 Register DESC0_CPU_EGP_26_6 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_6 0x2A160
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_6 0x1E7EA160u

//! Register Reset Value
#define DESC0_CPU_EGP_26_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_6 Register DESC1_CPU_EGP_26_6 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_6 0x2A164
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_6 0x1E7EA164u

//! Register Reset Value
#define DESC1_CPU_EGP_26_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_6 Register DESC2_CPU_EGP_26_6 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_6 0x2A168
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_6 0x1E7EA168u

//! Register Reset Value
#define DESC2_CPU_EGP_26_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_6 Register DESC3_CPU_EGP_26_6 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_6 0x2A16C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_6 0x1E7EA16Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_6_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_6_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_6_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_7 Register DESC0_CPU_EGP_26_7 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_7 0x2A170
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_7 0x1E7EA170u

//! Register Reset Value
#define DESC0_CPU_EGP_26_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_7 Register DESC1_CPU_EGP_26_7 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_7 0x2A174
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_7 0x1E7EA174u

//! Register Reset Value
#define DESC1_CPU_EGP_26_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_7 Register DESC2_CPU_EGP_26_7 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_7 0x2A178
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_7 0x1E7EA178u

//! Register Reset Value
#define DESC2_CPU_EGP_26_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_7 Register DESC3_CPU_EGP_26_7 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_7 0x2A17C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_7 0x1E7EA17Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_7_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_7_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_7_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_8 Register DESC0_CPU_EGP_26_8 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_8 0x2A180
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_8 0x1E7EA180u

//! Register Reset Value
#define DESC0_CPU_EGP_26_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_8 Register DESC1_CPU_EGP_26_8 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_8 0x2A184
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_8 0x1E7EA184u

//! Register Reset Value
#define DESC1_CPU_EGP_26_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_8 Register DESC2_CPU_EGP_26_8 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_8 0x2A188
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_8 0x1E7EA188u

//! Register Reset Value
#define DESC2_CPU_EGP_26_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_8 Register DESC3_CPU_EGP_26_8 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_8 0x2A18C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_8 0x1E7EA18Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_8_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_8_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_8_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_9 Register DESC0_CPU_EGP_26_9 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_9 0x2A190
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_9 0x1E7EA190u

//! Register Reset Value
#define DESC0_CPU_EGP_26_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_9 Register DESC1_CPU_EGP_26_9 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_9 0x2A194
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_9 0x1E7EA194u

//! Register Reset Value
#define DESC1_CPU_EGP_26_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_9 Register DESC2_CPU_EGP_26_9 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_9 0x2A198
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_9 0x1E7EA198u

//! Register Reset Value
#define DESC2_CPU_EGP_26_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_9 Register DESC3_CPU_EGP_26_9 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_9 0x2A19C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_9 0x1E7EA19Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_9_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_9_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_9_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_10 Register DESC0_CPU_EGP_26_10 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_10 0x2A1A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_10 0x1E7EA1A0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_10 Register DESC1_CPU_EGP_26_10 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_10 0x2A1A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_10 0x1E7EA1A4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_10 Register DESC2_CPU_EGP_26_10 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_10 0x2A1A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_10 0x1E7EA1A8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_10 Register DESC3_CPU_EGP_26_10 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_10 0x2A1AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_10 0x1E7EA1ACu

//! Register Reset Value
#define DESC3_CPU_EGP_26_10_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_10_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_10_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_11 Register DESC0_CPU_EGP_26_11 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_11 0x2A1B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_11 0x1E7EA1B0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_11 Register DESC1_CPU_EGP_26_11 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_11 0x2A1B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_11 0x1E7EA1B4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_11 Register DESC2_CPU_EGP_26_11 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_11 0x2A1B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_11 0x1E7EA1B8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_11 Register DESC3_CPU_EGP_26_11 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_11 0x2A1BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_11 0x1E7EA1BCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_11_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_11_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_11_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_12 Register DESC0_CPU_EGP_26_12 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_12 0x2A1C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_12 0x1E7EA1C0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_12 Register DESC1_CPU_EGP_26_12 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_12 0x2A1C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_12 0x1E7EA1C4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_12 Register DESC2_CPU_EGP_26_12 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_12 0x2A1C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_12 0x1E7EA1C8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_12 Register DESC3_CPU_EGP_26_12 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_12 0x2A1CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_12 0x1E7EA1CCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_12_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_12_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_12_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_13 Register DESC0_CPU_EGP_26_13 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_13 0x2A1D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_13 0x1E7EA1D0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_13 Register DESC1_CPU_EGP_26_13 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_13 0x2A1D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_13 0x1E7EA1D4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_13 Register DESC2_CPU_EGP_26_13 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_13 0x2A1D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_13 0x1E7EA1D8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_13 Register DESC3_CPU_EGP_26_13 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_13 0x2A1DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_13 0x1E7EA1DCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_13_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_13_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_13_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_14 Register DESC0_CPU_EGP_26_14 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_14 0x2A1E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_14 0x1E7EA1E0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_14 Register DESC1_CPU_EGP_26_14 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_14 0x2A1E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_14 0x1E7EA1E4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_14 Register DESC2_CPU_EGP_26_14 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_14 0x2A1E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_14 0x1E7EA1E8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_14 Register DESC3_CPU_EGP_26_14 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_14 0x2A1EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_14 0x1E7EA1ECu

//! Register Reset Value
#define DESC3_CPU_EGP_26_14_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_14_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_14_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_15 Register DESC0_CPU_EGP_26_15 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_15 0x2A1F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_15 0x1E7EA1F0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_15 Register DESC1_CPU_EGP_26_15 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_15 0x2A1F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_15 0x1E7EA1F4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_15 Register DESC2_CPU_EGP_26_15 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_15 0x2A1F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_15 0x1E7EA1F8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_15 Register DESC3_CPU_EGP_26_15 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_15 0x2A1FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_15 0x1E7EA1FCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_15_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_15_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_15_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_16 Register DESC0_CPU_EGP_26_16 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_16 0x2A200
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_16 0x1E7EA200u

//! Register Reset Value
#define DESC0_CPU_EGP_26_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_16 Register DESC1_CPU_EGP_26_16 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_16 0x2A204
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_16 0x1E7EA204u

//! Register Reset Value
#define DESC1_CPU_EGP_26_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_16 Register DESC2_CPU_EGP_26_16 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_16 0x2A208
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_16 0x1E7EA208u

//! Register Reset Value
#define DESC2_CPU_EGP_26_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_16 Register DESC3_CPU_EGP_26_16 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_16 0x2A20C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_16 0x1E7EA20Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_16_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_16_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_16_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_17 Register DESC0_CPU_EGP_26_17 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_17 0x2A210
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_17 0x1E7EA210u

//! Register Reset Value
#define DESC0_CPU_EGP_26_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_17 Register DESC1_CPU_EGP_26_17 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_17 0x2A214
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_17 0x1E7EA214u

//! Register Reset Value
#define DESC1_CPU_EGP_26_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_17 Register DESC2_CPU_EGP_26_17 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_17 0x2A218
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_17 0x1E7EA218u

//! Register Reset Value
#define DESC2_CPU_EGP_26_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_17 Register DESC3_CPU_EGP_26_17 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_17 0x2A21C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_17 0x1E7EA21Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_17_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_17_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_17_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_18 Register DESC0_CPU_EGP_26_18 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_18 0x2A220
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_18 0x1E7EA220u

//! Register Reset Value
#define DESC0_CPU_EGP_26_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_18 Register DESC1_CPU_EGP_26_18 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_18 0x2A224
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_18 0x1E7EA224u

//! Register Reset Value
#define DESC1_CPU_EGP_26_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_18 Register DESC2_CPU_EGP_26_18 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_18 0x2A228
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_18 0x1E7EA228u

//! Register Reset Value
#define DESC2_CPU_EGP_26_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_18 Register DESC3_CPU_EGP_26_18 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_18 0x2A22C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_18 0x1E7EA22Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_18_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_18_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_18_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_19 Register DESC0_CPU_EGP_26_19 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_19 0x2A230
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_19 0x1E7EA230u

//! Register Reset Value
#define DESC0_CPU_EGP_26_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_19 Register DESC1_CPU_EGP_26_19 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_19 0x2A234
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_19 0x1E7EA234u

//! Register Reset Value
#define DESC1_CPU_EGP_26_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_19 Register DESC2_CPU_EGP_26_19 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_19 0x2A238
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_19 0x1E7EA238u

//! Register Reset Value
#define DESC2_CPU_EGP_26_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_19 Register DESC3_CPU_EGP_26_19 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_19 0x2A23C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_19 0x1E7EA23Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_19_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_19_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_19_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_20 Register DESC0_CPU_EGP_26_20 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_20 0x2A240
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_20 0x1E7EA240u

//! Register Reset Value
#define DESC0_CPU_EGP_26_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_20 Register DESC1_CPU_EGP_26_20 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_20 0x2A244
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_20 0x1E7EA244u

//! Register Reset Value
#define DESC1_CPU_EGP_26_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_20 Register DESC2_CPU_EGP_26_20 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_20 0x2A248
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_20 0x1E7EA248u

//! Register Reset Value
#define DESC2_CPU_EGP_26_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_20 Register DESC3_CPU_EGP_26_20 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_20 0x2A24C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_20 0x1E7EA24Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_20_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_20_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_20_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_21 Register DESC0_CPU_EGP_26_21 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_21 0x2A250
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_21 0x1E7EA250u

//! Register Reset Value
#define DESC0_CPU_EGP_26_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_21 Register DESC1_CPU_EGP_26_21 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_21 0x2A254
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_21 0x1E7EA254u

//! Register Reset Value
#define DESC1_CPU_EGP_26_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_21 Register DESC2_CPU_EGP_26_21 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_21 0x2A258
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_21 0x1E7EA258u

//! Register Reset Value
#define DESC2_CPU_EGP_26_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_21 Register DESC3_CPU_EGP_26_21 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_21 0x2A25C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_21 0x1E7EA25Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_21_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_21_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_21_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_22 Register DESC0_CPU_EGP_26_22 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_22 0x2A260
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_22 0x1E7EA260u

//! Register Reset Value
#define DESC0_CPU_EGP_26_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_22 Register DESC1_CPU_EGP_26_22 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_22 0x2A264
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_22 0x1E7EA264u

//! Register Reset Value
#define DESC1_CPU_EGP_26_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_22 Register DESC2_CPU_EGP_26_22 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_22 0x2A268
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_22 0x1E7EA268u

//! Register Reset Value
#define DESC2_CPU_EGP_26_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_22 Register DESC3_CPU_EGP_26_22 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_22 0x2A26C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_22 0x1E7EA26Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_22_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_22_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_22_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_23 Register DESC0_CPU_EGP_26_23 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_23 0x2A270
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_23 0x1E7EA270u

//! Register Reset Value
#define DESC0_CPU_EGP_26_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_23 Register DESC1_CPU_EGP_26_23 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_23 0x2A274
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_23 0x1E7EA274u

//! Register Reset Value
#define DESC1_CPU_EGP_26_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_23 Register DESC2_CPU_EGP_26_23 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_23 0x2A278
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_23 0x1E7EA278u

//! Register Reset Value
#define DESC2_CPU_EGP_26_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_23 Register DESC3_CPU_EGP_26_23 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_23 0x2A27C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_23 0x1E7EA27Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_23_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_23_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_23_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_24 Register DESC0_CPU_EGP_26_24 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_24 0x2A280
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_24 0x1E7EA280u

//! Register Reset Value
#define DESC0_CPU_EGP_26_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_24 Register DESC1_CPU_EGP_26_24 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_24 0x2A284
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_24 0x1E7EA284u

//! Register Reset Value
#define DESC1_CPU_EGP_26_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_24 Register DESC2_CPU_EGP_26_24 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_24 0x2A288
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_24 0x1E7EA288u

//! Register Reset Value
#define DESC2_CPU_EGP_26_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_24 Register DESC3_CPU_EGP_26_24 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_24 0x2A28C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_24 0x1E7EA28Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_24_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_24_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_24_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_25 Register DESC0_CPU_EGP_26_25 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_25 0x2A290
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_25 0x1E7EA290u

//! Register Reset Value
#define DESC0_CPU_EGP_26_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_25 Register DESC1_CPU_EGP_26_25 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_25 0x2A294
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_25 0x1E7EA294u

//! Register Reset Value
#define DESC1_CPU_EGP_26_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_25 Register DESC2_CPU_EGP_26_25 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_25 0x2A298
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_25 0x1E7EA298u

//! Register Reset Value
#define DESC2_CPU_EGP_26_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_25 Register DESC3_CPU_EGP_26_25 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_25 0x2A29C
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_25 0x1E7EA29Cu

//! Register Reset Value
#define DESC3_CPU_EGP_26_25_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_25_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_25_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_26 Register DESC0_CPU_EGP_26_26 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_26 0x2A2A0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_26 0x1E7EA2A0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_26 Register DESC1_CPU_EGP_26_26 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_26 0x2A2A4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_26 0x1E7EA2A4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_26 Register DESC2_CPU_EGP_26_26 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_26 0x2A2A8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_26 0x1E7EA2A8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_26 Register DESC3_CPU_EGP_26_26 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_26 0x2A2AC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_26 0x1E7EA2ACu

//! Register Reset Value
#define DESC3_CPU_EGP_26_26_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_26_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_26_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_27 Register DESC0_CPU_EGP_26_27 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_27 0x2A2B0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_27 0x1E7EA2B0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_27 Register DESC1_CPU_EGP_26_27 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_27 0x2A2B4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_27 0x1E7EA2B4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_27 Register DESC2_CPU_EGP_26_27 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_27 0x2A2B8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_27 0x1E7EA2B8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_27 Register DESC3_CPU_EGP_26_27 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_27 0x2A2BC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_27 0x1E7EA2BCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_27_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_27_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_27_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_28 Register DESC0_CPU_EGP_26_28 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_28 0x2A2C0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_28 0x1E7EA2C0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_28 Register DESC1_CPU_EGP_26_28 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_28 0x2A2C4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_28 0x1E7EA2C4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_28 Register DESC2_CPU_EGP_26_28 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_28 0x2A2C8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_28 0x1E7EA2C8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_28 Register DESC3_CPU_EGP_26_28 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_28 0x2A2CC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_28 0x1E7EA2CCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_28_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_28_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_28_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_29 Register DESC0_CPU_EGP_26_29 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_29 0x2A2D0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_29 0x1E7EA2D0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_29 Register DESC1_CPU_EGP_26_29 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_29 0x2A2D4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_29 0x1E7EA2D4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_29 Register DESC2_CPU_EGP_26_29 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_29 0x2A2D8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_29 0x1E7EA2D8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_29 Register DESC3_CPU_EGP_26_29 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_29 0x2A2DC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_29 0x1E7EA2DCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_29_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_29_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_29_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_30 Register DESC0_CPU_EGP_26_30 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_30 0x2A2E0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_30 0x1E7EA2E0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_30 Register DESC1_CPU_EGP_26_30 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_30 0x2A2E4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_30 0x1E7EA2E4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_30 Register DESC2_CPU_EGP_26_30 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_30 0x2A2E8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_30 0x1E7EA2E8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_30 Register DESC3_CPU_EGP_26_30 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_30 0x2A2EC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_30 0x1E7EA2ECu

//! Register Reset Value
#define DESC3_CPU_EGP_26_30_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_30_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_30_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC0_CPU_EGP_26_31 Register DESC0_CPU_EGP_26_31 - Special CPU Egress Port Descriptor DW0
//! @{

//! Register Offset (relative)
#define DESC0_CPU_EGP_26_31 0x2A2F0
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC0_CPU_EGP_26_31 0x1E7EA2F0u

//! Register Reset Value
#define DESC0_CPU_EGP_26_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 0
#define DESC0_CPU_EGP_26_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC1_CPU_EGP_26_31 Register DESC1_CPU_EGP_26_31 - Special CPU Egress Port Descriptor DW1
//! @{

//! Register Offset (relative)
#define DESC1_CPU_EGP_26_31 0x2A2F4
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC1_CPU_EGP_26_31 0x1E7EA2F4u

//! Register Reset Value
#define DESC1_CPU_EGP_26_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 1
#define DESC1_CPU_EGP_26_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC2_CPU_EGP_26_31 Register DESC2_CPU_EGP_26_31 - Special CPU Egress Port Descriptor DW2
//! @{

//! Register Offset (relative)
#define DESC2_CPU_EGP_26_31 0x2A2F8
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC2_CPU_EGP_26_31 0x1E7EA2F8u

//! Register Reset Value
#define DESC2_CPU_EGP_26_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 2
#define DESC2_CPU_EGP_26_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup DESC3_CPU_EGP_26_31 Register DESC3_CPU_EGP_26_31 - Special CPU Egress Port Descriptor DW3
//! @{

//! Register Offset (relative)
#define DESC3_CPU_EGP_26_31 0x2A2FC
//! Register Offset (absolute) for 1st Instance CBM_DQM
#define CBM_DQM_DESC3_CPU_EGP_26_31 0x1E7EA2FCu

//! Register Reset Value
#define DESC3_CPU_EGP_26_31_RST 0x00000000u

//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_31_DESC_POS 0
//! Field DESC - Descriptor Double Word 3
#define DESC3_CPU_EGP_26_31_DESC_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
