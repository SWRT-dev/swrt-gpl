//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/CBM.xml
// Register File Name  : CQEM
// Register File Title : Central QoS Engine Manager Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CQEM_H
#define _CQEM_H

//! \defgroup CQEM Register File CQEM - Central QoS Engine Manager Register Description
//! @{

//! Base Address of CQEM_CTRL
#define CQEM_CTRL_MODULE_BASE 0x19010000u

//! \defgroup CBM_IRNCR_0 Register CBM_IRNCR_0 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_0 0x0
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_0 0x19010000u

//! Register Reset Value
#define CBM_IRNCR_0_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_0_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_0_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_0_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_0_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_0_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_0_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_0_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_0_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_0_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_0_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_0_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_0_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_0_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_0_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_0_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_0_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_0_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_0_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_0_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_0_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_0_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_0_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_0 Register CBM_IRNICR_0 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_0 0x4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_0 0x19010004u

//! Register Reset Value
#define CBM_IRNICR_0_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_0_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_0_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_0_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_0_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_0_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_0_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_0_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_0_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_0_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_0_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_0_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_0_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_0_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_0_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_0_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_0_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_0_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_0_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_0_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_0_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_0 Register CBM_IRNEN_0 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_0 0x8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_0 0x19010008u

//! Register Reset Value
#define CBM_IRNEN_0_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_0_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_0_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_0_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_0_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_0_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_0_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_0_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_0_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_0_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_0_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_0_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_0_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_0_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_0_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_0_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_0_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_0_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_0_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_0_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_0_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_0_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_0_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_0_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_0_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_0 Register IGP_IRNCR_0 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_0 0x10
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_0 0x19010010u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_0 0x19010014u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_0 0x19010018u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_0 0x19010020u

//! Register Reset Value
#define EGP_IRNCR_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_0_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_0 Register EGP_IRNICR_0 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_0 0x24
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_0 0x19010024u

//! Register Reset Value
#define EGP_IRNICR_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_0_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_0 Register EGP_IRNEN_0 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_0 0x28
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_0 0x19010028u

//! Register Reset Value
#define EGP_IRNEN_0_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_0_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_0_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_1 Register CBM_IRNCR_1 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_1 0x40
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_1 0x19010040u

//! Register Reset Value
#define CBM_IRNCR_1_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_1_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_1_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_1_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_1_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_1_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_1_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_1_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_1_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_1_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_1_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_1_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_1_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_1_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_1_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_1_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_1_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_1_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_1_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_1_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_1_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_1_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_1_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_1 Register CBM_IRNICR_1 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_1 0x44
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_1 0x19010044u

//! Register Reset Value
#define CBM_IRNICR_1_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_1_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_1_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_1_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_1_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_1_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_1_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_1_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_1_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_1_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_1_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_1_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_1_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_1_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_1_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_1_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_1_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_1_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_1_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_1_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_1_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_1 Register CBM_IRNEN_1 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_1 0x48
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_1 0x19010048u

//! Register Reset Value
#define CBM_IRNEN_1_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_1_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_1_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_1_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_1_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_1_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_1_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_1_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_1_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_1_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_1_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_1_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_1_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_1_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_1_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_1_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_1_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_1_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_1_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_1_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_1_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_1_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_1_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_1_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_1_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_1 Register IGP_IRNCR_1 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_1 0x50
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_1 0x19010050u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_1 0x19010054u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_1 0x19010058u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_1 0x19010060u

//! Register Reset Value
#define EGP_IRNCR_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_1_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_1 Register EGP_IRNICR_1 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_1 0x64
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_1 0x19010064u

//! Register Reset Value
#define EGP_IRNICR_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_1_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_1 Register EGP_IRNEN_1 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_1 0x68
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_1 0x19010068u

//! Register Reset Value
#define EGP_IRNEN_1_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_1_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_1_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_2 Register CBM_IRNCR_2 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_2 0x80
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_2 0x19010080u

//! Register Reset Value
#define CBM_IRNCR_2_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_2_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_2_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_2_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_2_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_2_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_2_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_2_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_2_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_2_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_2_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_2_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_2_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_2_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_2_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_2_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_2_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_2_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_2_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_2_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_2_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_2_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_2_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_2 Register CBM_IRNICR_2 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_2 0x84
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_2 0x19010084u

//! Register Reset Value
#define CBM_IRNICR_2_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_2_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_2_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_2_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_2_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_2_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_2_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_2_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_2_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_2_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_2_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_2_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_2_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_2_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_2_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_2_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_2_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_2_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_2_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_2_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_2_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_2 Register CBM_IRNEN_2 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_2 0x88
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_2 0x19010088u

//! Register Reset Value
#define CBM_IRNEN_2_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_2_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_2_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_2_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_2_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_2_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_2_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_2_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_2_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_2_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_2_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_2_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_2_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_2_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_2_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_2_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_2_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_2_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_2_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_2_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_2_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_2_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_2_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_2_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_2_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_2 Register IGP_IRNCR_2 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_2 0x90
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_2 0x19010090u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_2 0x19010094u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_2 0x19010098u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_2 0x190100A0u

//! Register Reset Value
#define EGP_IRNCR_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_2_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_2 Register EGP_IRNICR_2 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_2 0xA4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_2 0x190100A4u

//! Register Reset Value
#define EGP_IRNICR_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_2_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_2 Register EGP_IRNEN_2 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_2 0xA8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_2 0x190100A8u

//! Register Reset Value
#define EGP_IRNEN_2_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_2_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_2_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_3 Register CBM_IRNCR_3 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_3 0xC0
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_3 0x190100C0u

//! Register Reset Value
#define CBM_IRNCR_3_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_3_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_3_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_3_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_3_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_3_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_3_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_3_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_3_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_3_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_3_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_3_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_3_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_3_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_3_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_3_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_3_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_3_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_3_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_3_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_3_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_3_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_3_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_3 Register CBM_IRNICR_3 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_3 0xC4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_3 0x190100C4u

//! Register Reset Value
#define CBM_IRNICR_3_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_3_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_3_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_3_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_3_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_3_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_3_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_3_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_3_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_3_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_3_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_3_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_3_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_3_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_3_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_3_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_3_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_3_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_3_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_3_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_3_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_3 Register CBM_IRNEN_3 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_3 0xC8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_3 0x190100C8u

//! Register Reset Value
#define CBM_IRNEN_3_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_3_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_3_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_3_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_3_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_3_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_3_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_3_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_3_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_3_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_3_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_3_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_3_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_3_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_3_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_3_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_3_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_3_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_3_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_3_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_3_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_3_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_3_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_3_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_3_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_3 Register IGP_IRNCR_3 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_3 0xD0
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_3 0x190100D0u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_3 0x190100D4u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_3 0x190100D8u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_3 0x190100E0u

//! Register Reset Value
#define EGP_IRNCR_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_3_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_3 Register EGP_IRNICR_3 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_3 0xE4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_3 0x190100E4u

//! Register Reset Value
#define EGP_IRNICR_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_3_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_3 Register EGP_IRNEN_3 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_3 0xE8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_3 0x190100E8u

//! Register Reset Value
#define EGP_IRNEN_3_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_3_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_3_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_4 Register CBM_IRNCR_4 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_4 0x100
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_4 0x19010100u

//! Register Reset Value
#define CBM_IRNCR_4_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_4_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_4_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_4_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_4_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_4_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_4_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_4_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_4_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_4_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_4_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_4_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_4_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_4_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_4_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_4_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_4_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_4_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_4_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_4_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_4_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_4_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_4_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_4 Register CBM_IRNICR_4 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_4 0x104
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_4 0x19010104u

//! Register Reset Value
#define CBM_IRNICR_4_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_4_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_4_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_4_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_4_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_4_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_4_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_4_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_4_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_4_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_4_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_4_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_4_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_4_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_4_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_4_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_4_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_4_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_4_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_4_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_4_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_4 Register CBM_IRNEN_4 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_4 0x108
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_4 0x19010108u

//! Register Reset Value
#define CBM_IRNEN_4_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_4_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_4_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_4_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_4_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_4_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_4_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_4_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_4_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_4_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_4_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_4_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_4_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_4_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_4_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_4_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_4_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_4_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_4_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_4_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_4_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_4_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_4_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_4_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_4_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_4 Register IGP_IRNCR_4 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_4 0x110
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_4 0x19010110u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_4 0x19010114u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_4 0x19010118u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_4 0x19010120u

//! Register Reset Value
#define EGP_IRNCR_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_4_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_4 Register EGP_IRNICR_4 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_4 0x124
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_4 0x19010124u

//! Register Reset Value
#define EGP_IRNICR_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_4_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_4 Register EGP_IRNEN_4 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_4 0x128
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_4 0x19010128u

//! Register Reset Value
#define EGP_IRNEN_4_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_4_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_4_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_5 Register CBM_IRNCR_5 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_5 0x140
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_5 0x19010140u

//! Register Reset Value
#define CBM_IRNCR_5_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_5_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_5_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_5_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_5_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_5_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_5_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_5_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_5_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_5_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_5_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_5_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_5_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_5_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_5_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_5_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_5_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_5_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_5_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_5_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_5_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_5_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_5_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_5 Register CBM_IRNICR_5 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_5 0x144
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_5 0x19010144u

//! Register Reset Value
#define CBM_IRNICR_5_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_5_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_5_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_5_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_5_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_5_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_5_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_5_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_5_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_5_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_5_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_5_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_5_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_5_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_5_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_5_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_5_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_5_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_5_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_5_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_5_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_5 Register CBM_IRNEN_5 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_5 0x148
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_5 0x19010148u

//! Register Reset Value
#define CBM_IRNEN_5_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_5_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_5_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_5_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_5_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_5_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_5_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_5_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_5_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_5_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_5_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_5_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_5_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_5_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_5_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_5_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_5_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_5_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_5_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_5_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_5_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_5_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_5_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_5_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_5_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_5 Register IGP_IRNCR_5 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_5 0x150
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_5 0x19010150u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_5 0x19010154u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_5 0x19010158u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_5 0x19010160u

//! Register Reset Value
#define EGP_IRNCR_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_5_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_5 Register EGP_IRNICR_5 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_5 0x164
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_5 0x19010164u

//! Register Reset Value
#define EGP_IRNICR_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_5_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_5 Register EGP_IRNEN_5 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_5 0x168
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_5 0x19010168u

//! Register Reset Value
#define EGP_IRNEN_5_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_5_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_5_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_6 Register CBM_IRNCR_6 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_6 0x180
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_6 0x19010180u

//! Register Reset Value
#define CBM_IRNCR_6_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_6_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_6_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_6_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_6_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_6_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_6_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_6_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_6_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_6_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_6_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_6_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_6_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_6_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_6_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_6_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_6_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_6_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_6_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_6_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_6_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_6_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_6_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_6 Register CBM_IRNICR_6 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_6 0x184
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_6 0x19010184u

//! Register Reset Value
#define CBM_IRNICR_6_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_6_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_6_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_6_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_6_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_6_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_6_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_6_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_6_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_6_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_6_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_6_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_6_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_6_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_6_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_6_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_6_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_6_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_6_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_6_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_6_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_6 Register CBM_IRNEN_6 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_6 0x188
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_6 0x19010188u

//! Register Reset Value
#define CBM_IRNEN_6_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_6_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_6_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_6_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_6_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_6_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_6_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_6_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_6_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_6_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_6_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_6_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_6_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_6_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_6_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_6_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_6_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_6_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_6_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_6_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_6_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_6_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_6_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_6_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_6_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_6 Register IGP_IRNCR_6 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_6 0x190
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_6 0x19010190u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_6 0x19010194u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_6 0x19010198u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_6 0x190101A0u

//! Register Reset Value
#define EGP_IRNCR_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_6_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_6 Register EGP_IRNICR_6 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_6 0x1A4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_6 0x190101A4u

//! Register Reset Value
#define EGP_IRNICR_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_6_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_6 Register EGP_IRNEN_6 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_6 0x1A8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_6 0x190101A8u

//! Register Reset Value
#define EGP_IRNEN_6_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_6_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_6_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_IRNCR_7 Register CBM_IRNCR_7 - CBM IRN Capture Register
//! @{

//! Register Offset (relative)
#define CBM_IRNCR_7 0x1C0
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNCR_7 0x190101C0u

//! Register Reset Value
#define CBM_IRNCR_7_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_7_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNCR_7_QOSI_MASK 0x1u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_QOSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_QOSI_INTACK 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_7_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNCR_7_QOSERRI_MASK 0x2u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_QOSERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_QOSERRI_INTACK 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_7_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNCR_7_FSQMI_MASK 0x10u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMI_INTACK 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_7_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNCR_7_FSQMERRI_MASK 0x20u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_FSQMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_FSQMERRI_INTACK 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNCR_7_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNCR_7_BMI_MASK 0x40u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_BMI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_BMI_INTACK 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_7_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNCR_7_BMERRI_MASK 0x80u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_BMERRI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_BMERRI_INTACK 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNCR_7_LSI_MASK 0xF00u
//! Constant NUL - NULL
#define CONST_CBM_IRNCR_7_LSI_NUL 0x0
//! Constant INTACK - INTACK
#define CONST_CBM_IRNCR_7_LSI_INTACK 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_7_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNCR_7_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_7_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNCR_7_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNCR_7_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_7_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNCR_7_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNICR_7 Register CBM_IRNICR_7 - CBM IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define CBM_IRNICR_7 0x1C4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNICR_7 0x190101C4u

//! Register Reset Value
#define CBM_IRNICR_7_RST 0x00000000u

//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_7_QOSI_POS 0
//! Field QOSI - QOS Interrupt
#define CBM_IRNICR_7_QOSI_MASK 0x1u

//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_7_QOSERRI_POS 1
//! Field QOSERRI - QOS Error Interrupt
#define CBM_IRNICR_7_QOSERRI_MASK 0x2u

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_7_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNICR_7_FSQMI_MASK 0x10u

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_7_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNICR_7_FSQMERRI_MASK 0x20u

//! Field BMI - BM Interrupt
#define CBM_IRNICR_7_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNICR_7_BMI_MASK 0x40u

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_7_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNICR_7_BMERRI_MASK 0x80u

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNICR_7_LSI_MASK 0xF00u

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_7_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNICR_7_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_7_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNICR_7_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNICR_7_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_7_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNICR_7_CPUPI_MASK 0x100000u

//! @}

//! \defgroup CBM_IRNEN_7 Register CBM_IRNEN_7 - CBM IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define CBM_IRNEN_7 0x1C8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IRNEN_7 0x190101C8u

//! Register Reset Value
#define CBM_IRNEN_7_RST 0x00000000u

//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_7_QOSI_POS 0
//! Field QOSI - QoS Interrupt
#define CBM_IRNEN_7_QOSI_MASK 0x1u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_QOSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_QOSI_EN 0x1

//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_7_QOSERRI_POS 1
//! Field QOSERRI - QoS Error Interrupt
#define CBM_IRNEN_7_QOSERRI_MASK 0x2u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_QOSERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_QOSERRI_EN 0x1

//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_7_FSQMI_POS 4
//! Field FSQMI - FSQM Interrupt
#define CBM_IRNEN_7_FSQMI_MASK 0x10u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMI_EN 0x1

//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_7_FSQMERRI_POS 5
//! Field FSQMERRI - FSQM Error Interrupt
#define CBM_IRNEN_7_FSQMERRI_MASK 0x20u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_FSQMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_FSQMERRI_EN 0x1

//! Field BMI - BM Interrupt
#define CBM_IRNEN_7_BMI_POS 6
//! Field BMI - BM Interrupt
#define CBM_IRNEN_7_BMI_MASK 0x40u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_BMI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_BMI_EN 0x1

//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_7_BMERRI_POS 7
//! Field BMERRI - BM Error Interrupt
#define CBM_IRNEN_7_BMERRI_MASK 0x80u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_BMERRI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_BMERRI_EN 0x1

//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_7_LSI_POS 8
//! Field LSI - Load Spreader Interrupt
#define CBM_IRNEN_7_LSI_MASK 0xF00u
//! Constant DIS - DIS
#define CONST_CBM_IRNEN_7_LSI_DIS 0x0
//! Constant EN - EN
#define CONST_CBM_IRNEN_7_LSI_EN 0x1

//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_7_PIBI_POS 12
//! Field PIBI - PIB Block Interrupt
#define CBM_IRNEN_7_PIBI_MASK 0x3000u

//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_7_UMTI_POS 14
//! Field UMTI - UMT Block Interrupt
#define CBM_IRNEN_7_UMTI_MASK 0x4000u

//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_7_LLI_POS 16
//! Field LLI - Linked List Interrupt
#define CBM_IRNEN_7_LLI_MASK 0x10000u

//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_7_CPUPI_POS 20
//! Field CPUPI - CPU Pool Interrupt
#define CBM_IRNEN_7_CPUPI_MASK 0x100000u

//! @}

//! \defgroup IGP_IRNCR_7 Register IGP_IRNCR_7 - Ingress Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define IGP_IRNCR_7 0x1D0
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNCR_7 0x190101D0u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNICR_7 0x190101D4u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_IGP_IRNEN_7 0x190101D8u

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
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNCR_7 0x190101E0u

//! Register Reset Value
#define EGP_IRNCR_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNCR_7_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNICR_7 Register EGP_IRNICR_7 - Egress Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define EGP_IRNICR_7 0x1E4
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNICR_7 0x190101E4u

//! Register Reset Value
#define EGP_IRNICR_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNICR_7_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup EGP_IRNEN_7 Register EGP_IRNEN_7 - Egress Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define EGP_IRNEN_7 0x1E8
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_EGP_IRNEN_7 0x190101E8u

//! Register Reset Value
#define EGP_IRNEN_7_RST 0x00000000u

//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_7_EGPI_POS 0
//! Field EGPI - Egress Port Interrupt
#define EGP_IRNEN_7_EGPI_MASK 0x3FFFFFFu

//! @}

//! \defgroup CBM_VERSION_REG Register CBM_VERSION_REG - CBM/CQEM Version Register
//! @{

//! Register Offset (relative)
#define CBM_VERSION_REG 0x200
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_VERSION_REG 0x19010200u

//! Register Reset Value
#define CBM_VERSION_REG_RST 0x00000002u

//! Field MAJOR - MAJOR Version Number
#define CBM_VERSION_REG_MAJOR_POS 0
//! Field MAJOR - MAJOR Version Number
#define CBM_VERSION_REG_MAJOR_MASK 0xFu

//! Field MINOR - MINOR Version Number
#define CBM_VERSION_REG_MINOR_POS 8
//! Field MINOR - MINOR Version Number
#define CBM_VERSION_REG_MINOR_MASK 0xF00u

//! @}

//! \defgroup CBM_BUF_SIZE Register CBM_BUF_SIZE - Buffer Size Select Register
//! @{

//! Register Offset (relative)
#define CBM_BUF_SIZE 0x204
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_BUF_SIZE 0x19010204u

//! Register Reset Value
#define CBM_BUF_SIZE_RST 0x07040301u

//! Field SIZE0 - Buffer Size of Pool 0
#define CBM_BUF_SIZE_SIZE0_POS 0
//! Field SIZE0 - Buffer Size of Pool 0
#define CBM_BUF_SIZE_SIZE0_MASK 0x7u
//! Constant S128 - 128B buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S128 0x0
//! Constant S256 - 256B buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S256 0x1
//! Constant S512 - 512B buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S512 0x2
//! Constant S1024 - 1kB buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S1024 0x3
//! Constant S2048 - 2kB buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S2048 0x4
//! Constant S4096 - 4kB buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S4096 0x5
//! Constant S8192 - 8kB buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S8192 0x6
//! Constant S10240 - 10kB buffers
#define CONST_CBM_BUF_SIZE_SIZE0_S10240 0x7

//! Field SIZE1 - Buffer Size of Pool 1
#define CBM_BUF_SIZE_SIZE1_POS 8
//! Field SIZE1 - Buffer Size of Pool 1
#define CBM_BUF_SIZE_SIZE1_MASK 0x700u
//! Constant S128 - 128B buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S128 0x0
//! Constant S256 - 256B buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S256 0x1
//! Constant S512 - 512B buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S512 0x2
//! Constant S1024 - 1kB buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S1024 0x3
//! Constant S2048 - 2kB buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S2048 0x4
//! Constant S4096 - 4kB buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S4096 0x5
//! Constant S8192 - 8kB buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S8192 0x6
//! Constant S10240 - 10kB buffers
#define CONST_CBM_BUF_SIZE_SIZE1_S10240 0x7

//! Field SIZE2 - Buffer Size of Pool 2
#define CBM_BUF_SIZE_SIZE2_POS 16
//! Field SIZE2 - Buffer Size of Pool 2
#define CBM_BUF_SIZE_SIZE2_MASK 0x70000u
//! Constant S128 - 128B buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S128 0x0
//! Constant S256 - 256B buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S256 0x1
//! Constant S512 - 512B buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S512 0x2
//! Constant S1024 - 1kB buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S1024 0x3
//! Constant S2048 - 2kB buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S2048 0x4
//! Constant S4096 - 4kB buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S4096 0x5
//! Constant S8192 - 8kB buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S8192 0x6
//! Constant S10240 - 10kB buffers
#define CONST_CBM_BUF_SIZE_SIZE2_S10240 0x7

//! Field SIZE3 - Buffer Size of Pool 3
#define CBM_BUF_SIZE_SIZE3_POS 24
//! Field SIZE3 - Buffer Size of Pool 3
#define CBM_BUF_SIZE_SIZE3_MASK 0x7000000u
//! Constant S128 - 128B buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S128 0x0
//! Constant S256 - 256B buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S256 0x1
//! Constant S512 - 512B buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S512 0x2
//! Constant S1024 - 1kB buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S1024 0x3
//! Constant S2048 - 2kB buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S2048 0x4
//! Constant S4096 - 4kB buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S4096 0x5
//! Constant S8192 - 8kB buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S8192 0x6
//! Constant S10240 - 10kB buffers
#define CONST_CBM_BUF_SIZE_SIZE3_S10240 0x7

//! @}

//! \defgroup CBM_PB_BASE Register CBM_PB_BASE - Packet Buffer Base Address
//! @{

//! Register Offset (relative)
#define CBM_PB_BASE 0x208
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_PB_BASE 0x19010208u

//! Register Reset Value
#define CBM_PB_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_PB_BASE_BASE_POS 7
//! Field BASE - Base Address
#define CBM_PB_BASE_BASE_MASK 0xFFFFFF80u

//! @}

//! \defgroup CBM_CTRL Register CBM_CTRL - CBM Control
//! @{

//! Register Offset (relative)
#define CBM_CTRL 0x210
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CTRL 0x19010210u

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

//! Field UMT_RST - UMT Reset
#define CBM_CTRL_UMT_RST_POS 8
//! Field UMT_RST - UMT Reset
#define CBM_CTRL_UMT_RST_MASK 0x100u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_UMT_RST_NORMAL 0x0
//! Constant RESET - Reset the UMT
#define CONST_CBM_CTRL_UMT_RST_RESET 0x1

//! Field LSBYP - Hardware SMS lightspeed mode
#define CBM_CTRL_LSBYP_POS 16
//! Field LSBYP - Hardware SMS lightspeed mode
#define CBM_CTRL_LSBYP_MASK 0x10000u
//! Constant BYP - Lightspeed mode bypass
#define CONST_CBM_CTRL_LSBYP_BYP 0x0
//! Constant LS - Lightspeed mode enable
#define CONST_CBM_CTRL_LSBYP_LS 0x1

//! Field PBSEL - Packet Buffer Select
#define CBM_CTRL_PBSEL_POS 17
//! Field PBSEL - Packet Buffer Select
#define CBM_CTRL_PBSEL_MASK 0x20000u
//! Constant PBSEL0 - size 128 Bytes
#define CONST_CBM_CTRL_PBSEL_PBSEL0 0x0
//! Constant PBSEL1 - size 2 KBytes
#define CONST_CBM_CTRL_PBSEL_PBSEL1 0x1

//! Field ACA_EP0_EN - ACA EP0 Enable
#define CBM_CTRL_ACA_EP0_EN_POS 18
//! Field ACA_EP0_EN - ACA EP0 Enable
#define CBM_CTRL_ACA_EP0_EN_MASK 0x40000u

//! Field ACA_EP1_EN - ACA EP1 Enable
#define CBM_CTRL_ACA_EP1_EN_POS 20
//! Field ACA_EP1_EN - ACA EP1 Enable
#define CBM_CTRL_ACA_EP1_EN_MASK 0x100000u

//! Field ACA_EP2_EN - ACA EP2 Enable
#define CBM_CTRL_ACA_EP2_EN_POS 22
//! Field ACA_EP2_EN - ACA EP2 Enable
#define CBM_CTRL_ACA_EP2_EN_MASK 0x400000u

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

//! Field LS_RST - LS Reset
#define CBM_CTRL_LS_RST_POS 26
//! Field LS_RST - LS Reset
#define CBM_CTRL_LS_RST_MASK 0x4000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_LS_RST_NORMAL 0x0
//! Constant RESET - Reset the LS
#define CONST_CBM_CTRL_LS_RST_RESET 0x1

//! Field QOS_RST - QOS Reset
#define CBM_CTRL_QOS_RST_POS 27
//! Field QOS_RST - QOS Reset
#define CBM_CTRL_QOS_RST_MASK 0x8000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_QOS_RST_NORMAL 0x0
//! Constant RESET - Reset the QOS and BM
#define CONST_CBM_CTRL_QOS_RST_RESET 0x1

//! Field PIB_RST - PIB Reset
#define CBM_CTRL_PIB_RST_POS 28
//! Field PIB_RST - PIB Reset
#define CBM_CTRL_PIB_RST_MASK 0x10000000u
//! Constant NORMAL - Normal Operation
#define CONST_CBM_CTRL_PIB_RST_NORMAL 0x0
//! Constant RESET - Reset the BM
#define CONST_CBM_CTRL_PIB_RST_RESET 0x1

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

//! \defgroup CBM_LL_DBG Register CBM_LL_DBG - CBM Linked List Debug
//! @{

//! Register Offset (relative)
#define CBM_LL_DBG 0x214
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_LL_DBG 0x19010214u

//! Register Reset Value
#define CBM_LL_DBG_RST 0x7FFF0000u

//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_ERR_PORT_POS 1
//! Field ERR_PORT - Error Port Type
#define CBM_LL_DBG_ERR_PORT_MASK 0x2u
//! Constant IGP - Ingress Port
#define CONST_CBM_LL_DBG_ERR_PORT_IGP 0x0
//! Constant EGP - Egress Port
#define CONST_CBM_LL_DBG_ERR_PORT_EGP 0x1

//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_ERR_EGPID_POS 3
//! Field ERR_EGPID - Error Egress Port ID
#define CBM_LL_DBG_ERR_EGPID_MASK 0x3F8u

//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_ERR_IGPID_POS 10
//! Field ERR_IGPID - Error Ingress Port ID
#define CBM_LL_DBG_ERR_IGPID_MASK 0xC00u

//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_ERR_LSA_POS 16
//! Field ERR_LSA - Error LSA
#define CBM_LL_DBG_ERR_LSA_MASK 0x7FFF0000u

//! @}

//! \defgroup CBM_IP_VAL Register CBM_IP_VAL - CBM Ingress Port Value Register
//! @{

//! Register Offset (relative)
#define CBM_IP_VAL 0x218
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_IP_VAL 0x19010218u

//! Register Reset Value
#define CBM_IP_VAL_RST 0x00000000u

//! Field IP_VAL0 - IP Value0
#define CBM_IP_VAL_IP_VAL0_POS 0
//! Field IP_VAL0 - IP Value0
#define CBM_IP_VAL_IP_VAL0_MASK 0xFu

//! Field IP_VAL1 - IP Value1
#define CBM_IP_VAL_IP_VAL1_POS 8
//! Field IP_VAL1 - IP Value1
#define CBM_IP_VAL_IP_VAL1_MASK 0xF00u

//! @}

//! \defgroup CBM_BSL_CTRL Register CBM_BSL_CTRL - CBM Buffer Selection Logic Control Register
//! @{

//! Register Offset (relative)
#define CBM_BSL_CTRL 0x220
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_BSL_CTRL 0x19010220u

//! Register Reset Value
#define CBM_BSL_CTRL_RST 0x00000000u

//! Field BSL1_EN - BSL DMA1Rx Enable
#define CBM_BSL_CTRL_BSL1_EN_POS 0
//! Field BSL1_EN - BSL DMA1Rx Enable
#define CBM_BSL_CTRL_BSL1_EN_MASK 0x1u
//! Constant DIS - Disable
#define CONST_CBM_BSL_CTRL_BSL1_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CBM_BSL_CTRL_BSL1_EN_EN 0x1

//! Field BSL1_MODE - BSL1 Mode
#define CBM_BSL_CTRL_BSL1_MODE_POS 4
//! Field BSL1_MODE - BSL1 Mode
#define CBM_BSL_CTRL_BSL1_MODE_MASK 0x10u
//! Constant PRIO - Priority Based Selection
#define CONST_CBM_BSL_CTRL_BSL1_MODE_PRIO 0x0
//! Constant WRR - WRR Based Selection
#define CONST_CBM_BSL_CTRL_BSL1_MODE_WRR 0x1

//! Field BSL1_WRR_RATIO - BSL1 WRR Allocation Ratio
#define CBM_BSL_CTRL_BSL1_WRR_RATIO_POS 8
//! Field BSL1_WRR_RATIO - BSL1 WRR Allocation Ratio
#define CBM_BSL_CTRL_BSL1_WRR_RATIO_MASK 0x300u

//! Field BSL2_EN - BSL DMA2Rx Enable
#define CBM_BSL_CTRL_BSL2_EN_POS 16
//! Field BSL2_EN - BSL DMA2Rx Enable
#define CBM_BSL_CTRL_BSL2_EN_MASK 0x10000u
//! Constant DIS - Disable
#define CONST_CBM_BSL_CTRL_BSL2_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_CBM_BSL_CTRL_BSL2_EN_EN 0x1

//! Field BSL2_MODE - BSL2 Mode
#define CBM_BSL_CTRL_BSL2_MODE_POS 20
//! Field BSL2_MODE - BSL2 Mode
#define CBM_BSL_CTRL_BSL2_MODE_MASK 0x100000u
//! Constant PRIO - Priority Based Selection
#define CONST_CBM_BSL_CTRL_BSL2_MODE_PRIO 0x0
//! Constant WRR - WRR Based Selection
#define CONST_CBM_BSL_CTRL_BSL2_MODE_WRR 0x1

//! Field BSL2_WRR_RATIO - BSL2 WRR Allocation Ratio
#define CBM_BSL_CTRL_BSL2_WRR_RATIO_POS 24
//! Field BSL2_WRR_RATIO - BSL2 WRR Allocation Ratio
#define CBM_BSL_CTRL_BSL2_WRR_RATIO_MASK 0x3000000u

//! @}

//! \defgroup CBM_QID_MODE_SEL_REG_0 Register CBM_QID_MODE_SEL_REG_0 - CBM QID Mode Selection Register
//! @{

//! Register Offset (relative)
#define CBM_QID_MODE_SEL_REG_0 0x230
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_QID_MODE_SEL_REG_0 0x19010230u

//! Register Reset Value
#define CBM_QID_MODE_SEL_REG_0_RST 0x00000000u

//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE0_POS 0
//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE0_MASK 0x3u

//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE1_POS 2
//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE1_MASK 0xCu

//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE2_POS 4
//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE2_MASK 0x30u

//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE3_POS 6
//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE3_MASK 0xC0u

//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE4_POS 8
//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE4_MASK 0x300u

//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE5_POS 10
//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE5_MASK 0xC00u

//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE6_POS 12
//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE6_MASK 0x3000u

//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE7_POS 14
//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE7_MASK 0xC000u

//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE8_POS 16
//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE8_MASK 0x30000u

//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE9_POS 18
//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE9_MASK 0xC0000u

//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE10_POS 20
//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE10_MASK 0x300000u

//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE11_POS 22
//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE11_MASK 0xC00000u

//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE12_POS 24
//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE12_MASK 0x3000000u

//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE13_POS 26
//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE13_MASK 0xC000000u

//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE14_POS 28
//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE14_MASK 0x30000000u

//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE15_POS 30
//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_0_MODE15_MASK 0xC0000000u

//! @}

//! \defgroup CBM_QID_MODE_SEL_REG_1 Register CBM_QID_MODE_SEL_REG_1 - CBM QID Mode Selection Register
//! @{

//! Register Offset (relative)
#define CBM_QID_MODE_SEL_REG_1 0x234
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_QID_MODE_SEL_REG_1 0x19010234u

//! Register Reset Value
#define CBM_QID_MODE_SEL_REG_1_RST 0x00000000u

//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE0_POS 0
//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE0_MASK 0x3u

//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE1_POS 2
//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE1_MASK 0xCu

//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE2_POS 4
//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE2_MASK 0x30u

//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE3_POS 6
//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE3_MASK 0xC0u

//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE4_POS 8
//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE4_MASK 0x300u

//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE5_POS 10
//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE5_MASK 0xC00u

//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE6_POS 12
//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE6_MASK 0x3000u

//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE7_POS 14
//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE7_MASK 0xC000u

//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE8_POS 16
//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE8_MASK 0x30000u

//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE9_POS 18
//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE9_MASK 0xC0000u

//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE10_POS 20
//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE10_MASK 0x300000u

//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE11_POS 22
//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE11_MASK 0xC00000u

//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE12_POS 24
//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE12_MASK 0x3000000u

//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE13_POS 26
//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE13_MASK 0xC000000u

//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE14_POS 28
//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE14_MASK 0x30000000u

//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE15_POS 30
//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_1_MODE15_MASK 0xC0000000u

//! @}

//! \defgroup CBM_QID_MODE_SEL_REG_2 Register CBM_QID_MODE_SEL_REG_2 - CBM QID Mode Selection Register
//! @{

//! Register Offset (relative)
#define CBM_QID_MODE_SEL_REG_2 0x238
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_QID_MODE_SEL_REG_2 0x19010238u

//! Register Reset Value
#define CBM_QID_MODE_SEL_REG_2_RST 0x00000000u

//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE0_POS 0
//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE0_MASK 0x3u

//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE1_POS 2
//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE1_MASK 0xCu

//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE2_POS 4
//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE2_MASK 0x30u

//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE3_POS 6
//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE3_MASK 0xC0u

//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE4_POS 8
//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE4_MASK 0x300u

//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE5_POS 10
//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE5_MASK 0xC00u

//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE6_POS 12
//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE6_MASK 0x3000u

//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE7_POS 14
//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE7_MASK 0xC000u

//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE8_POS 16
//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE8_MASK 0x30000u

//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE9_POS 18
//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE9_MASK 0xC0000u

//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE10_POS 20
//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE10_MASK 0x300000u

//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE11_POS 22
//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE11_MASK 0xC00000u

//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE12_POS 24
//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE12_MASK 0x3000000u

//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE13_POS 26
//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE13_MASK 0xC000000u

//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE14_POS 28
//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE14_MASK 0x30000000u

//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE15_POS 30
//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_2_MODE15_MASK 0xC0000000u

//! @}

//! \defgroup CBM_QID_MODE_SEL_REG_3 Register CBM_QID_MODE_SEL_REG_3 - CBM QID Mode Selection Register
//! @{

//! Register Offset (relative)
#define CBM_QID_MODE_SEL_REG_3 0x23C
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_QID_MODE_SEL_REG_3 0x1901023Cu

//! Register Reset Value
#define CBM_QID_MODE_SEL_REG_3_RST 0x00000000u

//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE0_POS 0
//! Field MODE0 - Mode0 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE0_MASK 0x3u

//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE1_POS 2
//! Field MODE1 - Mode1 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE1_MASK 0xCu

//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE2_POS 4
//! Field MODE2 - Mode2 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE2_MASK 0x30u

//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE3_POS 6
//! Field MODE3 - Mode3 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE3_MASK 0xC0u

//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE4_POS 8
//! Field MODE4 - Mode4 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE4_MASK 0x300u

//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE5_POS 10
//! Field MODE5 - Mode5 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE5_MASK 0xC00u

//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE6_POS 12
//! Field MODE6 - Mode6 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE6_MASK 0x3000u

//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE7_POS 14
//! Field MODE7 - Mode7 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE7_MASK 0xC000u

//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE8_POS 16
//! Field MODE8 - Mode8 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE8_MASK 0x30000u

//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE9_POS 18
//! Field MODE9 - Mode9 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE9_MASK 0xC0000u

//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE10_POS 20
//! Field MODE10 - Mode10 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE10_MASK 0x300000u

//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE11_POS 22
//! Field MODE11 - Mode11 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE11_MASK 0xC00000u

//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE12_POS 24
//! Field MODE12 - Mode12 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE12_MASK 0x3000000u

//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE13_POS 26
//! Field MODE13 - Mode13 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE13_MASK 0xC000000u

//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE14_POS 28
//! Field MODE14 - Mode14 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE14_MASK 0x30000000u

//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE15_POS 30
//! Field MODE15 - Mode15 Bits
#define CBM_QID_MODE_SEL_REG_3_MODE15_MASK 0xC0000000u

//! @}

//! \defgroup CBM_BM_BASE Register CBM_BM_BASE - Buffer Manager Base Address
//! @{

//! Register Offset (relative)
#define CBM_BM_BASE 0x240
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_BM_BASE 0x19010240u

//! Register Reset Value
#define CBM_BM_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_BM_BASE_BASE_POS 17
//! Field BASE - Base Address
#define CBM_BM_BASE_BASE_MASK 0xFFFE0000u

//! @}

//! \defgroup CBM_WRED_BASE Register CBM_WRED_BASE - WRED Query Base Address
//! @{

//! Register Offset (relative)
#define CBM_WRED_BASE 0x244
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_WRED_BASE 0x19010244u

//! Register Reset Value
#define CBM_WRED_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_WRED_BASE_BASE_POS 12
//! Field BASE - Base Address
#define CBM_WRED_BASE_BASE_MASK 0xFFFFF000u

//! @}

//! \defgroup CBM_QPUSH_BASE Register CBM_QPUSH_BASE - Queue Push Base Address
//! @{

//! Register Offset (relative)
#define CBM_QPUSH_BASE 0x248
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_QPUSH_BASE 0x19010248u

//! Register Reset Value
#define CBM_QPUSH_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_QPUSH_BASE_BASE_POS 12
//! Field BASE - Base Address
#define CBM_QPUSH_BASE_BASE_MASK 0xFFFFF000u

//! @}

//! \defgroup CBM_TX_CREDIT_BASE Register CBM_TX_CREDIT_BASE - TX Credit Base Address
//! @{

//! Register Offset (relative)
#define CBM_TX_CREDIT_BASE 0x24C
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_TX_CREDIT_BASE 0x1901024Cu

//! Register Reset Value
#define CBM_TX_CREDIT_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_TX_CREDIT_BASE_BASE_POS 11
//! Field BASE - Base Address
#define CBM_TX_CREDIT_BASE_BASE_MASK 0xFFFFF800u

//! @}

//! \defgroup CBM_EPON_BASE Register CBM_EPON_BASE - EPON Descriptor Write Base Address
//! @{

//! Register Offset (relative)
#define CBM_EPON_BASE 0x250
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_EPON_BASE 0x19010250u

//! Register Reset Value
#define CBM_EPON_BASE_RST 0x00000000u

//! Field BASE - Base Address
#define CBM_EPON_BASE_BASE_POS 0
//! Field BASE - Base Address
#define CBM_EPON_BASE_BASE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CBM_CPU_POOL_BUF_RTRN_START_ADDR Register CBM_CPU_POOL_BUF_RTRN_START_ADDR - CPU Pool Buffer Return Start Address
//! @{

//! Register Offset (relative)
#define CBM_CPU_POOL_BUF_RTRN_START_ADDR 0x280
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CPU_POOL_BUF_RTRN_START_ADDR 0x19010280u

//! Register Reset Value
#define CBM_CPU_POOL_BUF_RTRN_START_ADDR_RST 0x00000000u

//! Field ADDR - Start Address
#define CBM_CPU_POOL_BUF_RTRN_START_ADDR_ADDR_POS 0
//! Field ADDR - Start Address
#define CBM_CPU_POOL_BUF_RTRN_START_ADDR_ADDR_MASK 0xFFFFFFFFu

//! @}

//! \defgroup CBM_CPU_POOL_BUF_ALW_NUM Register CBM_CPU_POOL_BUF_ALW_NUM - CPU Pool Buffer Allowed Number
//! @{

//! Register Offset (relative)
#define CBM_CPU_POOL_BUF_ALW_NUM 0x284
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CPU_POOL_BUF_ALW_NUM 0x19010284u

//! Register Reset Value
#define CBM_CPU_POOL_BUF_ALW_NUM_RST 0x00000000u

//! Field NUMBER - Number
#define CBM_CPU_POOL_BUF_ALW_NUM_NUMBER_POS 0
//! Field NUMBER - Number
#define CBM_CPU_POOL_BUF_ALW_NUM_NUMBER_MASK 0xFFFFu

//! @}

//! \defgroup CBM_CPU_POOL_ENQ_CNT Register CBM_CPU_POOL_ENQ_CNT - CBM CPU Pool Enqueue Count
//! @{

//! Register Offset (relative)
#define CBM_CPU_POOL_ENQ_CNT 0x288
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CPU_POOL_ENQ_CNT 0x19010288u

//! Register Reset Value
#define CBM_CPU_POOL_ENQ_CNT_RST 0x00000000u

//! Field COUNT - Count
#define CBM_CPU_POOL_ENQ_CNT_COUNT_POS 0
//! Field COUNT - Count
#define CBM_CPU_POOL_ENQ_CNT_COUNT_MASK 0xFFFFu

//! Field STS - Status
#define CBM_CPU_POOL_ENQ_CNT_STS_POS 16
//! Field STS - Status
#define CBM_CPU_POOL_ENQ_CNT_STS_MASK 0x10000u
//! Constant NA - Not Allowed
#define CONST_CBM_CPU_POOL_ENQ_CNT_STS_NA 0x0
//! Constant ALLOW - Allowed
#define CONST_CBM_CPU_POOL_ENQ_CNT_STS_ALLOW 0x1

//! @}

//! \defgroup CBM_CPU_POOL_BUF_RTRN_CNT Register CBM_CPU_POOL_BUF_RTRN_CNT - CBM CPU Pool Buffer Return Count
//! @{

//! Register Offset (relative)
#define CBM_CPU_POOL_BUF_RTRN_CNT 0x28C
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CPU_POOL_BUF_RTRN_CNT 0x1901028Cu

//! Register Reset Value
#define CBM_CPU_POOL_BUF_RTRN_CNT_RST 0x00000000u

//! Field COUNT - Count
#define CBM_CPU_POOL_BUF_RTRN_CNT_COUNT_POS 0
//! Field COUNT - Count
#define CBM_CPU_POOL_BUF_RTRN_CNT_COUNT_MASK 0xFFFFu

//! @}

//! \defgroup CBM_CPU_POOL_ENQ_DEC Register CBM_CPU_POOL_ENQ_DEC - CPU POOL ENQUEUE DECrement
//! @{

//! Register Offset (relative)
#define CBM_CPU_POOL_ENQ_DEC 0x290
//! Register Offset (absolute) for 1st Instance CQEM_CTRL
#define CQEM_CTRL_CBM_CPU_POOL_ENQ_DEC 0x19010290u

//! Register Reset Value
#define CBM_CPU_POOL_ENQ_DEC_RST 0x00000000u

//! Field DEC - Count Decrement
#define CBM_CPU_POOL_ENQ_DEC_DEC_POS 0
//! Field DEC - Count Decrement
#define CBM_CPU_POOL_ENQ_DEC_DEC_MASK 0xFFFFu

//! @}

//! @}

#endif
