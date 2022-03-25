//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : D:/Users/shij/Perforce/l1033.grx500.win.v_shij.priv.dfv.grx500.dfv/ipg_lsd/lsd_sys/source/xml/reg_files/CBM.xml
// Register File Name  : CBM
// Register File Title : Central Buffer Manager Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CBM_H
#define _CBM_H

//! \defgroup CBM Register File CBM - Central Buffer Manager Register Description
//! @{

//! Base Address of CBM
#define CBM_MODULE_BASE 0x1E710000u

//! \defgroup CBM_IRNCR_0 Register CBM_IRNCR_0 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_0 0x0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_0 0x1E710000u

//! Register Reset Value
#define CBM_IRNCR_0_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_0_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_0_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_0_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_0_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_0_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_0_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_0_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_0_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_0_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_0_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_0_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_0_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_0_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_0_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_0 Register CBM_IRNICR_0 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_0 0x4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_0 0x1E710004u

//! Register Reset Value
#define CBM_IRNICR_0_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_0_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_0_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_0_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_0_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_0_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_0_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_0_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_0_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_0_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_0_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_0_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_0_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_0_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_0_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_0 Register CBM_IRNEN_0 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_0 0x8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_0 0x1E710008u

//! Register Reset Value
#define CBM_IRNEN_0_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_0_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_0_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_0_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_0_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_0_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_0_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_0_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_0_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_0_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_0_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_0_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_0_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_0_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_0_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_0 Register IGP_IRNCR_0 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_0 0x10
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_0 0x1E710010u

//! Register Reset Value
#define IGP_IRNCR_0_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_0_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_0_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_0 Register IGP_IRNICR_0 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_0 0x14
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_0 0x1E710014u

//! Register Reset Value
#define IGP_IRNICR_0_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_0_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_0_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_0 Register IGP_IRNEN_0 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_0 0x18
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_0 0x1E710018u

//! Register Reset Value
#define IGP_IRNEN_0_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_0_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_0_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_0 Register EGP_IRNCR_0 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_0 0x20
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_0 0x1E710020u

//! Register Reset Value
#define EGP_IRNCR_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_0_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_0 Register EGP_IRNICR_0 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_0 0x24
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_0 0x1E710024u

//! Register Reset Value
#define EGP_IRNICR_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_0_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_0 Register EGP_IRNEN_0 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_0 0x28
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_0 0x1E710028u

//! Register Reset Value
#define EGP_IRNEN_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_0_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_1 Register CBM_IRNCR_1 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_1 0x40
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_1 0x1E710040u

//! Register Reset Value
#define CBM_IRNCR_1_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_1_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_1_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_1_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_1_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_1_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_1_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_1_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_1_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_1_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_1_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_1_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_1_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_1_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_1_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_1 Register CBM_IRNICR_1 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_1 0x44
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_1 0x1E710044u

//! Register Reset Value
#define CBM_IRNICR_1_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_1_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_1_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_1_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_1_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_1_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_1_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_1_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_1_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_1_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_1_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_1_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_1_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_1_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_1_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_1 Register CBM_IRNEN_1 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_1 0x48
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_1 0x1E710048u

//! Register Reset Value
#define CBM_IRNEN_1_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_1_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_1_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_1_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_1_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_1_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_1_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_1_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_1_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_1_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_1_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_1_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_1_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_1_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_1_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_1 Register IGP_IRNCR_1 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_1 0x50
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_1 0x1E710050u

//! Register Reset Value
#define IGP_IRNCR_1_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_1_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_1_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_1 Register IGP_IRNICR_1 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_1 0x54
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_1 0x1E710054u

//! Register Reset Value
#define IGP_IRNICR_1_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_1_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_1_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_1 Register IGP_IRNEN_1 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_1 0x58
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_1 0x1E710058u

//! Register Reset Value
#define IGP_IRNEN_1_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_1_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_1_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_1 Register EGP_IRNCR_1 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_1 0x60
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_1 0x1E710060u

//! Register Reset Value
#define EGP_IRNCR_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_1_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_1 Register EGP_IRNICR_1 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_1 0x64
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_1 0x1E710064u

//! Register Reset Value
#define EGP_IRNICR_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_1_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_1 Register EGP_IRNEN_1 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_1 0x68
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_1 0x1E710068u

//! Register Reset Value
#define EGP_IRNEN_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_1_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_2 Register CBM_IRNCR_2 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_2 0x80
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_2 0x1E710080u

//! Register Reset Value
#define CBM_IRNCR_2_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_2_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_2_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_2_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_2_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_2_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_2_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_2_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_2_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_2_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_2_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_2_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_2_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_2_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_2_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_2 Register CBM_IRNICR_2 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_2 0x84
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_2 0x1E710084u

//! Register Reset Value
#define CBM_IRNICR_2_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_2_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_2_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_2_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_2_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_2_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_2_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_2_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_2_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_2_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_2_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_2_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_2_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_2_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_2_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_2 Register CBM_IRNEN_2 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_2 0x88
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_2 0x1E710088u

//! Register Reset Value
#define CBM_IRNEN_2_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_2_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_2_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_2_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_2_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_2_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_2_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_2_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_2_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_2_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_2_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_2_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_2_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_2_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_2_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_2 Register IGP_IRNCR_2 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_2 0x90
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_2 0x1E710090u

//! Register Reset Value
#define IGP_IRNCR_2_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_2_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_2_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_2 Register IGP_IRNICR_2 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_2 0x94
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_2 0x1E710094u

//! Register Reset Value
#define IGP_IRNICR_2_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_2_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_2_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_2 Register IGP_IRNEN_2 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_2 0x98
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_2 0x1E710098u

//! Register Reset Value
#define IGP_IRNEN_2_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_2_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_2_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_2 Register EGP_IRNCR_2 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_2 0xA0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_2 0x1E7100A0u

//! Register Reset Value
#define EGP_IRNCR_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_2_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_2 Register EGP_IRNICR_2 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_2 0xA4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_2 0x1E7100A4u

//! Register Reset Value
#define EGP_IRNICR_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_2_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_2 Register EGP_IRNEN_2 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_2 0xA8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_2 0x1E7100A8u

//! Register Reset Value
#define EGP_IRNEN_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_2_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_3 Register CBM_IRNCR_3 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_3 0xC0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_3 0x1E7100C0u

//! Register Reset Value
#define CBM_IRNCR_3_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_3_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_3_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_3_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_3_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_3_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_3_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_3_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_3_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_3_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_3_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_3_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_3_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_3_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_3_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_3 Register CBM_IRNICR_3 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_3 0xC4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_3 0x1E7100C4u

//! Register Reset Value
#define CBM_IRNICR_3_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_3_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_3_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_3_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_3_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_3_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_3_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_3_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_3_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_3_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_3_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_3_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_3_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_3_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_3_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_3 Register CBM_IRNEN_3 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_3 0xC8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_3 0x1E7100C8u

//! Register Reset Value
#define CBM_IRNEN_3_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_3_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_3_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_3_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_3_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_3_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_3_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_3_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_3_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_3_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_3_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_3_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_3_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_3_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_3_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_3 Register IGP_IRNCR_3 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_3 0xD0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_3 0x1E7100D0u

//! Register Reset Value
#define IGP_IRNCR_3_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_3_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_3_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_3 Register IGP_IRNICR_3 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_3 0xD4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_3 0x1E7100D4u

//! Register Reset Value
#define IGP_IRNICR_3_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_3_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_3_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_3 Register IGP_IRNEN_3 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_3 0xD8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_3 0x1E7100D8u

//! Register Reset Value
#define IGP_IRNEN_3_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_3_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_3_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_3 Register EGP_IRNCR_3 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_3 0xE0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_3 0x1E7100E0u

//! Register Reset Value
#define EGP_IRNCR_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_3_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_3 Register EGP_IRNICR_3 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_3 0xE4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_3 0x1E7100E4u

//! Register Reset Value
#define EGP_IRNICR_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_3_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_3 Register EGP_IRNEN_3 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_3 0xE8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_3 0x1E7100E8u

//! Register Reset Value
#define EGP_IRNEN_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_3_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_4 Register CBM_IRNCR_4 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_4 0x100
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_4 0x1E710100u

//! Register Reset Value
#define CBM_IRNCR_4_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_4_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_4_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_4_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_4_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_4_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_4_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_4_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_4_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_4_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_4_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_4_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_4_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_4_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_4_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_4 Register CBM_IRNICR_4 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_4 0x104
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_4 0x1E710104u

//! Register Reset Value
#define CBM_IRNICR_4_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_4_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_4_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_4_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_4_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_4_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_4_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_4_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_4_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_4_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_4_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_4_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_4_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_4_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_4_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_4 Register CBM_IRNEN_4 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_4 0x108
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_4 0x1E710108u

//! Register Reset Value
#define CBM_IRNEN_4_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_4_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_4_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_4_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_4_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_4_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_4_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_4_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_4_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_4_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_4_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_4_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_4_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_4_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_4_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_4 Register IGP_IRNCR_4 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_4 0x110
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_4 0x1E710110u

//! Register Reset Value
#define IGP_IRNCR_4_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_4_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_4_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_4 Register IGP_IRNICR_4 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_4 0x114
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_4 0x1E710114u

//! Register Reset Value
#define IGP_IRNICR_4_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_4_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_4_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_4 Register IGP_IRNEN_4 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_4 0x118
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_4 0x1E710118u

//! Register Reset Value
#define IGP_IRNEN_4_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_4_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_4_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_4 Register EGP_IRNCR_4 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_4 0x120
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_4 0x1E710120u

//! Register Reset Value
#define EGP_IRNCR_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_4_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_4 Register EGP_IRNICR_4 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_4 0x124
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_4 0x1E710124u

//! Register Reset Value
#define EGP_IRNICR_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_4_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_4 Register EGP_IRNEN_4 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_4 0x128
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_4 0x1E710128u

//! Register Reset Value
#define EGP_IRNEN_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_4_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_5 Register CBM_IRNCR_5 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_5 0x140
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_5 0x1E710140u

//! Register Reset Value
#define CBM_IRNCR_5_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_5_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_5_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_5_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_5_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_5_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_5_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_5_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_5_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_5_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_5_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_5_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_5_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_5_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_5_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_5 Register CBM_IRNICR_5 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_5 0x144
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_5 0x1E710144u

//! Register Reset Value
#define CBM_IRNICR_5_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_5_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_5_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_5_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_5_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_5_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_5_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_5_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_5_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_5_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_5_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_5_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_5_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_5_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_5_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_5 Register CBM_IRNEN_5 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_5 0x148
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_5 0x1E710148u

//! Register Reset Value
#define CBM_IRNEN_5_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_5_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_5_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_5_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_5_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_5_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_5_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_5_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_5_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_5_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_5_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_5_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_5_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_5_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_5_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_5 Register IGP_IRNCR_5 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_5 0x150
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_5 0x1E710150u

//! Register Reset Value
#define IGP_IRNCR_5_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_5_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_5_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_5 Register IGP_IRNICR_5 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_5 0x154
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_5 0x1E710154u

//! Register Reset Value
#define IGP_IRNICR_5_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_5_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_5_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_5 Register IGP_IRNEN_5 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_5 0x158
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_5 0x1E710158u

//! Register Reset Value
#define IGP_IRNEN_5_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_5_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_5_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_5 Register EGP_IRNCR_5 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_5 0x160
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_5 0x1E710160u

//! Register Reset Value
#define EGP_IRNCR_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_5_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_5 Register EGP_IRNICR_5 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_5 0x164
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_5 0x1E710164u

//! Register Reset Value
#define EGP_IRNICR_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_5_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_5 Register EGP_IRNEN_5 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_5 0x168
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_5 0x1E710168u

//! Register Reset Value
#define EGP_IRNEN_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_5_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_6 Register CBM_IRNCR_6 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_6 0x180
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_6 0x1E710180u

//! Register Reset Value
#define CBM_IRNCR_6_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_6_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_6_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_6_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_6_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_6_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_6_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_6_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_6_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_6_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_6_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_6_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_6_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_6_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_6_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_6 Register CBM_IRNICR_6 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_6 0x184
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_6 0x1E710184u

//! Register Reset Value
#define CBM_IRNICR_6_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_6_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_6_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_6_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_6_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_6_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_6_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_6_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_6_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_6_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_6_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_6_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_6_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_6_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_6_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_6 Register CBM_IRNEN_6 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_6 0x188
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_6 0x1E710188u

//! Register Reset Value
#define CBM_IRNEN_6_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_6_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_6_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_6_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_6_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_6_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_6_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_6_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_6_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_6_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_6_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_6_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_6_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_6_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_6_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_6 Register IGP_IRNCR_6 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_6 0x190
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_6 0x1E710190u

//! Register Reset Value
#define IGP_IRNCR_6_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_6_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_6_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_6 Register IGP_IRNICR_6 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_6 0x194
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_6 0x1E710194u

//! Register Reset Value
#define IGP_IRNICR_6_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_6_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_6_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_6 Register IGP_IRNEN_6 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_6 0x198
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_6 0x1E710198u

//! Register Reset Value
#define IGP_IRNEN_6_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_6_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_6_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_6 Register EGP_IRNCR_6 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_6 0x1A0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_6 0x1E7101A0u

//! Register Reset Value
#define EGP_IRNCR_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_6_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_6 Register EGP_IRNICR_6 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_6 0x1A4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_6 0x1E7101A4u

//! Register Reset Value
#define EGP_IRNICR_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_6_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_6 Register EGP_IRNEN_6 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_6 0x1A8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_6 0x1E7101A8u

//! Register Reset Value
#define EGP_IRNEN_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_6_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_7 Register CBM_IRNCR_7 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_7 0x1C0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNCR_7 0x1E7101C0u

//! Register Reset Value
#define CBM_IRNCR_7_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_7_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNCR_7_TMUI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_TMUI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_TMUI_INTACK 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_7_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNCR_7_TMUERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_TMUERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_TMUERRI_INTACK 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_7_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNCR_7_FSQMI0_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMI0_INTACK 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_7_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNCR_7_FSQMERRI0_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMERRI0_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMERRI0_INTACK 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_7_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNCR_7_FSQMI1_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMI1_INTACK 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_7_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNCR_7_FSQMERRI1_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMERRI1_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMERRI1_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_7_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_7_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNICR_7 Register CBM_IRNICR_7 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_7 0x1C4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNICR_7 0x1E7101C4u

//! Register Reset Value
#define CBM_IRNICR_7_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_7_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNICR_7_TMUI_MASK 0x1u

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_7_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNICR_7_TMUERRI_MASK 0x2u

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_7_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNICR_7_FSQMI0_MASK 0x10u

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_7_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNICR_7_FSQMERRI0_MASK 0x20u

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_7_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNICR_7_FSQMI1_MASK 0x40u

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_7_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNICR_7_FSQMERRI1_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_7_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_7_LLI_MASK 0x30000u

//! @}

//! \defgroup CBM_IRNEN_7 Register CBM_IRNEN_7 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_7 0x1C8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_IRNEN_7 0x1E7101C8u

//! Register Reset Value
#define CBM_IRNEN_7_RST 0x00000000u

//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_7_TMUI_POS 0
//! Field TMUI - TMU Interrupt
#define CBM_IRNEN_7_TMUI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_TMUI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_TMUI_EN 0x1

//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_7_TMUERRI_POS 1
//! Field TMUERRI - TMU Error Interrupt
#define CBM_IRNEN_7_TMUERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_TMUERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_TMUERRI_EN 0x1

//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_7_FSQMI0_POS 4
//! Field FSQMI0 - FSQM Interrupt 0
#define CBM_IRNEN_7_FSQMI0_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMI0_EN 0x1

//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_7_FSQMERRI0_POS 5
//! Field FSQMERRI0 - FSQM Error Interrupt 0
#define CBM_IRNEN_7_FSQMERRI0_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMERRI0_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMERRI0_EN 0x1

//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_7_FSQMI1_POS 6
//! Field FSQMI1 - FSQM Interrupt 1
#define CBM_IRNEN_7_FSQMI1_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMI1_EN 0x1

//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_7_FSQMERRI1_POS 7
//! Field FSQMERRI1 - FSQM Error Interrupt 1
#define CBM_IRNEN_7_FSQMERRI1_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMERRI1_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMERRI1_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_7_LSI_MASK 0xF00u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_7_LLI_MASK 0x30000u

//! @}

//! \defgroup IGP_IRNCR_7 Register IGP_IRNCR_7 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_7 0x1D0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNCR_7 0x1E7101D0u

//! Register Reset Value
#define IGP_IRNCR_7_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_7_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNCR_7_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNICR_7 Register IGP_IRNICR_7 - Ingress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define IGP_IRNICR_7 0x1D4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNICR_7 0x1E7101D4u

//! Register Reset Value
#define IGP_IRNICR_7_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_7_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNICR_7_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup IGP_IRNEN_7 Register IGP_IRNEN_7 - Ingress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define IGP_IRNEN_7 0x1D8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_IGP_IRNEN_7 0x1E7101D8u

//! Register Reset Value
#define IGP_IRNEN_7_RST 0x00000000u

//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_7_IGPI_POS 0
//! Field IGPI - Ingress Port Interrupt
#define IGP_IRNEN_7_IGPI_MASK 0xFFFFu

//! @}

//! \defgroup EGP_IRNCR_7 Register EGP_IRNCR_7 - Egress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define EGP_IRNCR_7 0x1E0
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNCR_7 0x1E7101E0u

//! Register Reset Value
#define EGP_IRNCR_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_7_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_7 Register EGP_IRNICR_7 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_7 0x1E4
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNICR_7 0x1E7101E4u

//! Register Reset Value
#define EGP_IRNICR_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_7_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_7 Register EGP_IRNEN_7 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_7 0x1E8
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_EGP_IRNEN_7 0x1E7101E8u

//! Register Reset Value
#define EGP_IRNEN_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_7_EGPI_MASK 0x7FFFFFFu

//! @}

//! \defgroup CBM_SBA_0 Register CBM_SBA_0 - Standard Base Address
//! @{

//! Register Offset (relative)
#define CBM_SBA_0 0x200
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_SBA_0 0x1E710200u

//! Register Reset Value
#define CBM_SBA_0_RST 0x00000000u

//! Field BA - Base Address
#define CBM_SBA_0_BA_POS 11
//! Field BA - Base Address
#define CBM_SBA_0_BA_MASK 0xFFFFF800u

//! @}

//! \defgroup CBM_SBA_1 Register CBM_SBA_1 - Standard Base Address
//! @{

//! Register Offset (relative)
#define CBM_SBA_1 0x204
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_SBA_1 0x1E710204u

//! Register Reset Value
#define CBM_SBA_1_RST 0x00000000u

//! Field BA - Base Address
#define CBM_SBA_1_BA_POS 11
//! Field BA - Base Address
#define CBM_SBA_1_BA_MASK 0xFFFFF800u

//! @}

//! \defgroup CBM_JBA_0 Register CBM_JBA_0 - Jumbo Base Address
//! @{

//! Register Offset (relative)
#define CBM_JBA_0 0x208
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_JBA_0 0x1E710208u

//! Register Reset Value
#define CBM_JBA_0_RST 0x00000000u

//! Field BA - Base Address
#define CBM_JBA_0_BA_POS 11
//! Field BA - Base Address
#define CBM_JBA_0_BA_MASK 0xFFFFF800u

//! @}

//! \defgroup CBM_JBA_1 Register CBM_JBA_1 - Jumbo Base Address
//! @{

//! Register Offset (relative)
#define CBM_JBA_1 0x20C
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_JBA_1 0x1E71020Cu

//! Register Reset Value
#define CBM_JBA_1_RST 0x00000000u

//! Field BA - Base Address
#define CBM_JBA_1_BA_POS 11
//! Field BA - Base Address
#define CBM_JBA_1_BA_MASK 0xFFFFF800u

//! @}

//! \defgroup CBM_CTRL Register CBM_CTRL - CBM Control
//! @{

//! Register Offset (relative)
#define CBM_CTRL 0x210
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_CTRL 0x1E710210u

//! Register Reset Value
#define CBM_CTRL_RST 0x00000000u

//! Field ACT - CBM Activity Status
#define CBM_CTRL_ACT_POS 0
//! Field ACT - CBM Activity Status
#define CBM_CTRL_ACT_MASK 0x1u
//! Constant INACTIVE - Inactive
#define CONST_CBM_CTRL_ACT_INACTIVE 0x0
//! Constant ACTIVE - Active
#define CONST_CBM_CTRL_ACT_ACTIVE 0x1

//! Field WLAN_EP3 - WLAN EP 3
#define CBM_CTRL_WLAN_EP3_POS 4
//! Field WLAN_EP3 - WLAN EP 3
#define CBM_CTRL_WLAN_EP3_MASK 0xF0u

//! Field WLAN_EP2 - WLAN EP 2
#define CBM_CTRL_WLAN_EP2_POS 8
//! Field WLAN_EP2 - WLAN EP 2
#define CBM_CTRL_WLAN_EP2_MASK 0xF00u

//! Field WLAN_EP1 - WLAN EP 1
#define CBM_CTRL_WLAN_EP1_POS 12
//! Field WLAN_EP1 - WLAN EP 1
#define CBM_CTRL_WLAN_EP1_MASK 0xF000u

//! Field LSBYP - Hardware SMS lightspeed mode
#define CBM_CTRL_LSBYP_POS 16
//! Field LSBYP - Hardware SMS lightspeed mode
#define CBM_CTRL_LSBYP_MASK 0x10000u
//! Constant BYP - Lightspeed mode bypass
#define CONST_CBM_CTRL_LSBYP_BYP 0x0
//! Constant LS - Lightspeed mode enable
#define CONST_CBM_CTRL_LSBYP_LS 0x1

//! Field JSEL - Jumbo Select
#define CBM_CTRL_JSEL_POS 17
//! Field JSEL - Jumbo Select
#define CBM_CTRL_JSEL_MASK 0x20000u
//! Constant JSEL0 - Jumbo frame size 8 KBytes
#define CONST_CBM_CTRL_JSEL_JSEL0 0x0
//! Constant JSEL1 - Jumbo frame size 16 KBytes
#define CONST_CBM_CTRL_JSEL_JSEL1 0x1

//! Field WLAN_EP0 - WLAN EP 0
#define CBM_CTRL_WLAN_EP0_POS 20
//! Field WLAN_EP0 - WLAN EP 0
#define CBM_CTRL_WLAN_EP0_MASK 0xF00000u

//! Field LL_DBG - Linked List Debug
#define CBM_CTRL_LL_DBG_POS 24
//! Field LL_DBG - Linked List Debug
#define CBM_CTRL_LL_DBG_MASK 0x1000000u
//! Constant DIS - Disable
#define CONST_CBM_CTRL_LL_DBG_DIS 0x0
//! Constant EN - Enable
#define CONST_CBM_CTRL_LL_DBG_EN 0x1

//! Field DESC64B_RST - 64-bit Descriptor Reset
#define CBM_CTRL_DESC64B_RST_POS 25
//! Field DESC64B_RST - 64-bit Descriptor Reset
#define CBM_CTRL_DESC64B_RST_MASK 0x2000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_DESC64B_RST_NORMAL 0x0
//! Constant RESET - Reset the 64-bit descriptors
#define CONST_CBM_CTRL_DESC64B_RST_RESET 0x1

//! Field LS_RST - Load Spreader Reset
#define CBM_CTRL_LS_RST_POS 26
//! Field LS_RST - Load Spreader Reset
#define CBM_CTRL_LS_RST_MASK 0x4000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_LS_RST_NORMAL 0x0
//! Constant RESET - Reset the Load Spreader
#define CONST_CBM_CTRL_LS_RST_RESET 0x1

//! Field TMU_RST - TMU Reset
#define CBM_CTRL_TMU_RST_POS 27
//! Field TMU_RST - TMU Reset
#define CBM_CTRL_TMU_RST_MASK 0x8000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_TMU_RST_NORMAL 0x0
//! Constant RESET - Reset the TMU
#define CONST_CBM_CTRL_TMU_RST_RESET 0x1

//! Field FSQM1_RST - FSQM1 Reset
#define CBM_CTRL_FSQM1_RST_POS 28
//! Field FSQM1_RST - FSQM1 Reset
#define CBM_CTRL_FSQM1_RST_MASK 0x10000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_FSQM1_RST_NORMAL 0x0
//! Constant RESET - Reset the FSQM1
#define CONST_CBM_CTRL_FSQM1_RST_RESET 0x1

//! Field FSQM0_RST - FSQM0 Reset
#define CBM_CTRL_FSQM0_RST_POS 29
//! Field FSQM0_RST - FSQM0 Reset
#define CBM_CTRL_FSQM0_RST_MASK 0x20000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_FSQM0_RST_NORMAL 0x0
//! Constant RESET - Reset the FSQM0
#define CONST_CBM_CTRL_FSQM0_RST_RESET 0x1

//! Field DQM_RST - DQM Reset
#define CBM_CTRL_DQM_RST_POS 30
//! Field DQM_RST - DQM Reset
#define CBM_CTRL_DQM_RST_MASK 0x40000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_DQM_RST_NORMAL 0x0
//! Constant RESET - Reset the Dequeue Manager
#define CONST_CBM_CTRL_DQM_RST_RESET 0x1

//! Field EQM_RST - EQM Reset
#define CBM_CTRL_EQM_RST_POS 31
//! Field EQM_RST - EQM Reset
#define CBM_CTRL_EQM_RST_MASK 0x80000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_EQM_RST_NORMAL 0x0
//! Constant RESET - Reset the Enqueue Manager
#define CONST_CBM_CTRL_EQM_RST_RESET 0x1

//! @}

//! \defgroup CBM_LL_DBG_0 Register CBM_LL_DBG_0 - CBM Linked List Debug
//! @{

//! Register Offset (relative)
#define CBM_LL_DBG_0 0x214
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_LL_DBG_0 0x1E710214u

//! Register Reset Value
#define CBM_LL_DBG_0_RST 0x7FFF0000u

//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_0_ERR_PORT_POS 1
//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_0_ERR_PORT_MASK 0x2u
//! Constant IGP - Ingress Port
#define CONST_CBM_LL_DBG_0_ERR_PORT_IGP 0x0
//! Constant EGP - Egress Port
#define CONST_CBM_LL_DBG_0_ERR_PORT_EGP 0x1

//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_0_ERR_EGPID_POS 4
//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_0_ERR_EGPID_MASK 0x1F0u

//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_0_ERR_IGPID_POS 10
//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_0_ERR_IGPID_MASK 0xC00u

//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_0_ERR_LSA_POS 16
//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_0_ERR_LSA_MASK 0x7FFF0000u

//! @}

//! \defgroup CBM_LL_DBG_1 Register CBM_LL_DBG_1 - CBM Linked List Debug
//! @{

//! Register Offset (relative)
#define CBM_LL_DBG_1 0x218
//! Register Offset (absolute) for 1st Instance CBM
#define CBM_CBM_LL_DBG_1 0x1E710218u

//! Register Reset Value
#define CBM_LL_DBG_1_RST 0x7FFF0000u

//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_1_ERR_PORT_POS 1
//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_1_ERR_PORT_MASK 0x2u
//! Constant IGP - Ingress Port
#define CONST_CBM_LL_DBG_1_ERR_PORT_IGP 0x0
//! Constant EGP - Egress Port
#define CONST_CBM_LL_DBG_1_ERR_PORT_EGP 0x1

//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_1_ERR_EGPID_POS 4
//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_1_ERR_EGPID_MASK 0x1F0u

//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_1_ERR_IGPID_POS 10
//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_1_ERR_IGPID_MASK 0xC00u

//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_1_ERR_LSA_POS 16
//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_1_ERR_LSA_MASK 0x7FFF0000u

//! @}

//! @}

#endif
