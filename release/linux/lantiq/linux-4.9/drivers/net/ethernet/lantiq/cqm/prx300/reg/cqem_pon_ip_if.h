//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/cbm_pon_ip_if.xml
// Register File Name  : CQEM_PON_IP_IF
// Register File Title : PON IP IF Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _CQEM_PON_IP_IF_H
#define _CQEM_PON_IP_IF_H

//! \defgroup CQEM_PON_IP_IF Register File CQEM_PON_IP_IF - PON IP IF Register Description
//! @{

//! Base Address of CQEM_PIB
#define CQEM_PIB_MODULE_BASE 0x19100000u

//! \defgroup PIB_PON_IP_CMD Register PIB_PON_IP_CMD - PON IP Command Register
//! @{

//! Register Offset (relative)
#define PIB_PON_IP_CMD 0x100
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_PON_IP_CMD 0x19100100u

//! Register Reset Value
#define PIB_PON_IP_CMD_RST 0x00000000u

//! Field NUM_BYTES - Number of Bytes to be dequeued from the PORT_ID
#define PIB_PON_IP_CMD_NUM_BYTES_POS 0
//! Field NUM_BYTES - Number of Bytes to be dequeued from the PORT_ID
#define PIB_PON_IP_CMD_NUM_BYTES_MASK 0xFFFFFFu

//! Field PORT_ID - PORT ID of the port from which the descriptor are to be dequeued
#define PIB_PON_IP_CMD_PORT_ID_POS 24
//! Field PORT_ID - PORT ID of the port from which the descriptor are to be dequeued
#define PIB_PON_IP_CMD_PORT_ID_MASK 0xFF000000u

//! @}

//! \defgroup PIB_OVERSHOOT_BYTES Register PIB_OVERSHOOT_BYTES - PIB overshoot bytes register
//! @{

//! Register Offset (relative)
#define PIB_OVERSHOOT_BYTES 0x108
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_OVERSHOOT_BYTES 0x19100108u

//! Register Reset Value
#define PIB_OVERSHOOT_BYTES_RST 0x00000000u

//! Field OVERSHOOT - Overshoot Bytes
#define PIB_OVERSHOOT_BYTES_OVERSHOOT_POS 0
//! Field OVERSHOOT - Overshoot Bytes
#define PIB_OVERSHOOT_BYTES_OVERSHOOT_MASK 0xFFFFFFu

//! @}

//! \defgroup PIB_CTRL Register PIB_CTRL - PON IP IF Block Control Register
//! @{

//! Register Offset (relative)
#define PIB_CTRL 0x110
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_CTRL 0x19100110u

//! Register Reset Value
#define PIB_CTRL_RST 0x06000000u

//! Field PIB_EN - Enable the PIB
#define PIB_CTRL_PIB_EN_POS 0
//! Field PIB_EN - Enable the PIB
#define PIB_CTRL_PIB_EN_MASK 0x1u
//! Constant DIS - Disable
#define CONST_PIB_CTRL_PIB_EN_DIS 0x0
//! Constant EN - Enable
#define CONST_PIB_CTRL_PIB_EN_EN 0x1

//! Field PIB_PAUSE - PON IP IF Block Pause enable
#define PIB_CTRL_PIB_PAUSE_POS 1
//! Field PIB_PAUSE - PON IP IF Block Pause enable
#define PIB_CTRL_PIB_PAUSE_MASK 0x2u
//! Constant RUN - Run
#define CONST_PIB_CTRL_PIB_PAUSE_RUN 0x0
//! Constant PAUSE - Pause
#define CONST_PIB_CTRL_PIB_PAUSE_PAUSE 0x1

//! Field OVRFLW_INT_EN - Enable PIB command FIFO overflow Interrupt
#define PIB_CTRL_OVRFLW_INT_EN_POS 8
//! Field OVRFLW_INT_EN - Enable PIB command FIFO overflow Interrupt
#define PIB_CTRL_OVRFLW_INT_EN_MASK 0x100u
//! Constant DIS - Disable Interrupt
#define CONST_PIB_CTRL_OVRFLW_INT_EN_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PIB_CTRL_OVRFLW_INT_EN_EN 0x1

//! Field ILLEGAL_PORT_INT_EN - Enable Illegal Port Command Interrupt
#define PIB_CTRL_ILLEGAL_PORT_INT_EN_POS 9
//! Field ILLEGAL_PORT_INT_EN - Enable Illegal Port Command Interrupt
#define PIB_CTRL_ILLEGAL_PORT_INT_EN_MASK 0x200u
//! Constant DIS - Disable Interrupt
#define CONST_PIB_CTRL_ILLEGAL_PORT_INT_EN_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PIB_CTRL_ILLEGAL_PORT_INT_EN_EN 0x1

//! Field WAKEUP_INT_EN - Enable Wakeup Interrupt
#define PIB_CTRL_WAKEUP_INT_EN_POS 10
//! Field WAKEUP_INT_EN - Enable Wakeup Interrupt
#define PIB_CTRL_WAKEUP_INT_EN_MASK 0x400u
//! Constant DIS - Disable Interrupt
#define CONST_PIB_CTRL_WAKEUP_INT_EN_DIS 0x0
//! Constant EN - Enable Interrupt
#define CONST_PIB_CTRL_WAKEUP_INT_EN_EN 0x1

//! Field PIB_BYPASS - Enable PIB Bypass Mode
#define PIB_CTRL_PIB_BYPASS_POS 16
//! Field PIB_BYPASS - Enable PIB Bypass Mode
#define PIB_CTRL_PIB_BYPASS_MASK 0x10000u
//! Constant DIS - Disable Bypass
#define CONST_PIB_CTRL_PIB_BYPASS_DIS 0x0
//! Constant EN - Enable Bypass
#define CONST_PIB_CTRL_PIB_BYPASS_EN 0x1

//! Field PKT_LEN_ADJ_EN - Enable Packet Length Adjustment
#define PIB_CTRL_PKT_LEN_ADJ_EN_POS 20
//! Field PKT_LEN_ADJ_EN - Enable Packet Length Adjustment
#define PIB_CTRL_PKT_LEN_ADJ_EN_MASK 0x100000
//! Constant DIS - Disable adj
#define CONST_PIB_CTRL_PKT_LEN_ADJ_DIS 0x0
//! Constant EN - Enable Adj
#define CONST_PIB_CTRL_PIB_PKT_LEN_ADJ_EN 0x1

//! Field DQ_DLY - Dequeue Delay
#define PIB_CTRL_DQ_DLY_POS 24
//! Field DQ_DLY - Dequeue Delay
#define PIB_CTRL_DQ_DLY_MASK 0xF000000u

//! @}

//! \defgroup PIB_STATUS Register PIB_STATUS - PON IP Status Register
//! @{

//! Register Offset (relative)
#define PIB_STATUS 0x114
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_STATUS 0x19100114u

//! Register Reset Value
#define PIB_STATUS_RST 0x00020000u

//! Field PIB_EN_STS - PIB Block is enabled
#define PIB_STATUS_PIB_EN_STS_POS 0
//! Field PIB_EN_STS - PIB Block is enabled
#define PIB_STATUS_PIB_EN_STS_MASK 0x1u

//! Field PIB_PAUSE_STS - PIB Block is paused
#define PIB_STATUS_PIB_PAUSE_STS_POS 1
//! Field PIB_PAUSE_STS - PIB Block is paused
#define PIB_STATUS_PIB_PAUSE_STS_MASK 0x2u

//! Field FIFO_SIZE - Current PON IP FIFO Size
#define PIB_STATUS_FIFO_SIZE_POS 7
//! Field FIFO_SIZE - Current PON IP FIFO Size
#define PIB_STATUS_FIFO_SIZE_MASK 0xFF80u

//! Field FIFO_FULL - FIFO Full
#define PIB_STATUS_FIFO_FULL_POS 16
//! Field FIFO_FULL - FIFO Full
#define PIB_STATUS_FIFO_FULL_MASK 0x10000u

//! Field FIFO_EMPTY - FIFO Empty
#define PIB_STATUS_FIFO_EMPTY_POS 17
//! Field FIFO_EMPTY - FIFO Empty
#define PIB_STATUS_FIFO_EMPTY_MASK 0x20000u

//! Field FIFO_OVFL - FIFO Overflow
#define PIB_STATUS_FIFO_OVFL_POS 24
//! Field FIFO_OVFL - FIFO Overflow
#define PIB_STATUS_FIFO_OVFL_MASK 0x1000000u
//! Constant NOR - Normal
#define CONST_PIB_STATUS_FIFO_OVFL_NOR 0x0
//! Constant OVFL - Overflow
#define CONST_PIB_STATUS_FIFO_OVFL_OVFL 0x1

//! Field ILLEGAL_PORT - Illegal port command
#define PIB_STATUS_ILLEGAL_PORT_POS 25
//! Field ILLEGAL_PORT - Illegal port command
#define PIB_STATUS_ILLEGAL_PORT_MASK 0x2000000u
//! Constant NOR - Normal
#define CONST_PIB_STATUS_ILLEGAL_PORT_NOR 0x0
//! Constant ILL - Illegal
#define CONST_PIB_STATUS_ILLEGAL_PORT_ILL 0x1

//! Field WAKEUP_INT - Wake Up Interrupt
#define PIB_STATUS_WAKEUP_INT_POS 26
//! Field WAKEUP_INT - Wake Up Interrupt
#define PIB_STATUS_WAKEUP_INT_MASK 0x4000000u
//! Constant NOR - Normal
#define CONST_PIB_STATUS_WAKEUP_INT_NOR 0x0
//! Constant ILL - Illegal
#define CONST_PIB_STATUS_WAKEUP_INT_ILL 0x1

//! @}

//! \defgroup PIB_FIFO_OVFL_CMD_REG Register PIB_FIFO_OVFL_CMD_REG - FIFO Overflow command register
//! @{

//! Register Offset (relative)
#define PIB_FIFO_OVFL_CMD_REG 0x118
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_FIFO_OVFL_CMD_REG 0x19100118u

//! Register Reset Value
#define PIB_FIFO_OVFL_CMD_REG_RST 0x00000000u

//! Field OVFL_CMD - Overflow Command
#define PIB_FIFO_OVFL_CMD_REG_OVFL_CMD_POS 0
//! Field OVFL_CMD - Overflow Command
#define PIB_FIFO_OVFL_CMD_REG_OVFL_CMD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PIB_ILLEGAL_CMD_REG Register PIB_ILLEGAL_CMD_REG - PIB Illeagal Command Register
//! @{

//! Register Offset (relative)
#define PIB_ILLEGAL_CMD_REG 0x11C
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_ILLEGAL_CMD_REG 0x1910011Cu

//! Register Reset Value
#define PIB_ILLEGAL_CMD_REG_RST 0x00000000u

//! Field ILLEGAL_PORT_CMD - Illegal port command
#define PIB_ILLEGAL_CMD_REG_ILLEGAL_PORT_CMD_POS 0
//! Field ILLEGAL_PORT_CMD - Illegal port command
#define PIB_ILLEGAL_CMD_REG_ILLEGAL_PORT_CMD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PIB_PORT_EN_31_0 Register PIB_PORT_EN_31_0 - PON IP IF Port Enable Register Port 31-0
//! @{

//! Register Offset (relative)
#define PIB_PORT_EN_31_0 0x120
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_PORT_EN_31_0 0x19100120u

//! Register Reset Value
#define PIB_PORT_EN_31_0_RST 0x00000000u

//! Field EN_PORT0 - PORT0 Enable
#define PIB_PORT_EN_31_0_EN_PORT0_POS 0
//! Field EN_PORT0 - PORT0 Enable
#define PIB_PORT_EN_31_0_EN_PORT0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT0_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT0_EN 0x1

//! Field EN_PORT1 - PORT1 Enable
#define PIB_PORT_EN_31_0_EN_PORT1_POS 1
//! Field EN_PORT1 - PORT1 Enable
#define PIB_PORT_EN_31_0_EN_PORT1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT1_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT1_EN 0x1

//! Field EN_PORT2 - PORT2 Enable
#define PIB_PORT_EN_31_0_EN_PORT2_POS 2
//! Field EN_PORT2 - PORT2 Enable
#define PIB_PORT_EN_31_0_EN_PORT2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT2_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT2_EN 0x1

//! Field EN_PORT3 - PORT3 Enable
#define PIB_PORT_EN_31_0_EN_PORT3_POS 3
//! Field EN_PORT3 - PORT3 Enable
#define PIB_PORT_EN_31_0_EN_PORT3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT3_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT3_EN 0x1

//! Field EN_PORT4 - PORT4 Enable
#define PIB_PORT_EN_31_0_EN_PORT4_POS 4
//! Field EN_PORT4 - PORT4 Enable
#define PIB_PORT_EN_31_0_EN_PORT4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT4_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT4_EN 0x1

//! Field EN_PORT5 - PORT5 Enable
#define PIB_PORT_EN_31_0_EN_PORT5_POS 5
//! Field EN_PORT5 - PORT5 Enable
#define PIB_PORT_EN_31_0_EN_PORT5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT5_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT5_EN 0x1

//! Field EN_PORT6 - PORT6 Enable
#define PIB_PORT_EN_31_0_EN_PORT6_POS 6
//! Field EN_PORT6 - PORT6 Enable
#define PIB_PORT_EN_31_0_EN_PORT6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT6_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT6_EN 0x1

//! Field EN_PORT7 - PORT7 Enable
#define PIB_PORT_EN_31_0_EN_PORT7_POS 7
//! Field EN_PORT7 - PORT7 Enable
#define PIB_PORT_EN_31_0_EN_PORT7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT7_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT7_EN 0x1

//! Field EN_PORT8 - PORT8 Enable
#define PIB_PORT_EN_31_0_EN_PORT8_POS 8
//! Field EN_PORT8 - PORT8 Enable
#define PIB_PORT_EN_31_0_EN_PORT8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT8_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT8_EN 0x1

//! Field EN_PORT9 - PORT9 Enable
#define PIB_PORT_EN_31_0_EN_PORT9_POS 9
//! Field EN_PORT9 - PORT9 Enable
#define PIB_PORT_EN_31_0_EN_PORT9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT9_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT9_EN 0x1

//! Field EN_PORT10 - PORT10 Enable
#define PIB_PORT_EN_31_0_EN_PORT10_POS 10
//! Field EN_PORT10 - PORT10 Enable
#define PIB_PORT_EN_31_0_EN_PORT10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT10_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT10_EN 0x1

//! Field EN_PORT11 - PORT11 Enable
#define PIB_PORT_EN_31_0_EN_PORT11_POS 11
//! Field EN_PORT11 - PORT11 Enable
#define PIB_PORT_EN_31_0_EN_PORT11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT11_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT11_EN 0x1

//! Field EN_PORT12 - PORT12 Enable
#define PIB_PORT_EN_31_0_EN_PORT12_POS 12
//! Field EN_PORT12 - PORT12 Enable
#define PIB_PORT_EN_31_0_EN_PORT12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT12_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT12_EN 0x1

//! Field EN_PORT13 - PORT13 Enable
#define PIB_PORT_EN_31_0_EN_PORT13_POS 13
//! Field EN_PORT13 - PORT13 Enable
#define PIB_PORT_EN_31_0_EN_PORT13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT13_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT13_EN 0x1

//! Field EN_PORT14 - PORT14 Enable
#define PIB_PORT_EN_31_0_EN_PORT14_POS 14
//! Field EN_PORT14 - PORT14 Enable
#define PIB_PORT_EN_31_0_EN_PORT14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT14_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT14_EN 0x1

//! Field EN_PORT15 - PORT15 Enable
#define PIB_PORT_EN_31_0_EN_PORT15_POS 15
//! Field EN_PORT15 - PORT15 Enable
#define PIB_PORT_EN_31_0_EN_PORT15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT15_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT15_EN 0x1

//! Field EN_PORT16 - PORT16 Enable
#define PIB_PORT_EN_31_0_EN_PORT16_POS 16
//! Field EN_PORT16 - PORT16 Enable
#define PIB_PORT_EN_31_0_EN_PORT16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT16_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT16_EN 0x1

//! Field EN_PORT17 - PORT17 Enable
#define PIB_PORT_EN_31_0_EN_PORT17_POS 17
//! Field EN_PORT17 - PORT17 Enable
#define PIB_PORT_EN_31_0_EN_PORT17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT17_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT17_EN 0x1

//! Field EN_PORT18 - PORT18 Enable
#define PIB_PORT_EN_31_0_EN_PORT18_POS 18
//! Field EN_PORT18 - PORT18 Enable
#define PIB_PORT_EN_31_0_EN_PORT18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT18_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT18_EN 0x1

//! Field EN_PORT19 - PORT19 Enable
#define PIB_PORT_EN_31_0_EN_PORT19_POS 19
//! Field EN_PORT19 - PORT19 Enable
#define PIB_PORT_EN_31_0_EN_PORT19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT19_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT19_EN 0x1

//! Field EN_PORT20 - PORT20 Enable
#define PIB_PORT_EN_31_0_EN_PORT20_POS 20
//! Field EN_PORT20 - PORT20 Enable
#define PIB_PORT_EN_31_0_EN_PORT20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT20_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT20_EN 0x1

//! Field EN_PORT21 - PORT21 Enable
#define PIB_PORT_EN_31_0_EN_PORT21_POS 21
//! Field EN_PORT21 - PORT21 Enable
#define PIB_PORT_EN_31_0_EN_PORT21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT21_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT21_EN 0x1

//! Field EN_PORT22 - PORT22 Enable
#define PIB_PORT_EN_31_0_EN_PORT22_POS 22
//! Field EN_PORT22 - PORT22 Enable
#define PIB_PORT_EN_31_0_EN_PORT22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT22_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT22_EN 0x1

//! Field EN_PORT23 - PORT23 Enable
#define PIB_PORT_EN_31_0_EN_PORT23_POS 23
//! Field EN_PORT23 - PORT23 Enable
#define PIB_PORT_EN_31_0_EN_PORT23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT23_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT23_EN 0x1

//! Field EN_PORT24 - PORT24 Enable
#define PIB_PORT_EN_31_0_EN_PORT24_POS 24
//! Field EN_PORT24 - PORT24 Enable
#define PIB_PORT_EN_31_0_EN_PORT24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT24_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT24_EN 0x1

//! Field EN_PORT25 - PORT25 Enable
#define PIB_PORT_EN_31_0_EN_PORT25_POS 25
//! Field EN_PORT25 - PORT25 Enable
#define PIB_PORT_EN_31_0_EN_PORT25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT25_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT25_EN 0x1

//! Field EN_PORT26 - PORT26 Enable
#define PIB_PORT_EN_31_0_EN_PORT26_POS 26
//! Field EN_PORT26 - PORT26 Enable
#define PIB_PORT_EN_31_0_EN_PORT26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT26_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT26_EN 0x1

//! Field EN_PORT27 - PORT27 Enable
#define PIB_PORT_EN_31_0_EN_PORT27_POS 27
//! Field EN_PORT27 - PORT27 Enable
#define PIB_PORT_EN_31_0_EN_PORT27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT27_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT27_EN 0x1

//! Field EN_PORT28 - PORT28 Enable
#define PIB_PORT_EN_31_0_EN_PORT28_POS 28
//! Field EN_PORT28 - PORT28 Enable
#define PIB_PORT_EN_31_0_EN_PORT28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT28_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT28_EN 0x1

//! Field EN_PORT29 - PORT29 Enable
#define PIB_PORT_EN_31_0_EN_PORT29_POS 29
//! Field EN_PORT29 - PORT29 Enable
#define PIB_PORT_EN_31_0_EN_PORT29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT29_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT29_EN 0x1

//! Field EN_PORT30 - PORT30 Enable
#define PIB_PORT_EN_31_0_EN_PORT30_POS 30
//! Field EN_PORT30 - PORT30 Enable
#define PIB_PORT_EN_31_0_EN_PORT30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT30_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT30_EN 0x1

//! Field EN_PORT31 - PORT31 Enable
#define PIB_PORT_EN_31_0_EN_PORT31_POS 31
//! Field EN_PORT31 - PORT31 Enable
#define PIB_PORT_EN_31_0_EN_PORT31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_31_0_EN_PORT31_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_31_0_EN_PORT31_EN 0x1

//! @}

//! \defgroup PIB_PORT_EN_64_32 Register PIB_PORT_EN_64_32 - PON IP IF Port Enable Register Port 63-32
//! @{

//! Register Offset (relative)
#define PIB_PORT_EN_64_32 0x124
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_PORT_EN_64_32 0x19100124u

//! Register Reset Value
#define PIB_PORT_EN_64_32_RST 0x00000000u

//! Field EN_PORT0 - PORT0 Enable
#define PIB_PORT_EN_64_32_EN_PORT0_POS 0
//! Field EN_PORT0 - PORT0 Enable
#define PIB_PORT_EN_64_32_EN_PORT0_MASK 0x1u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT0_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT0_EN 0x1

//! Field EN_PORT1 - PORT1 Enable
#define PIB_PORT_EN_64_32_EN_PORT1_POS 1
//! Field EN_PORT1 - PORT1 Enable
#define PIB_PORT_EN_64_32_EN_PORT1_MASK 0x2u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT1_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT1_EN 0x1

//! Field EN_PORT2 - PORT2 Enable
#define PIB_PORT_EN_64_32_EN_PORT2_POS 2
//! Field EN_PORT2 - PORT2 Enable
#define PIB_PORT_EN_64_32_EN_PORT2_MASK 0x4u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT2_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT2_EN 0x1

//! Field EN_PORT3 - PORT3 Enable
#define PIB_PORT_EN_64_32_EN_PORT3_POS 3
//! Field EN_PORT3 - PORT3 Enable
#define PIB_PORT_EN_64_32_EN_PORT3_MASK 0x8u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT3_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT3_EN 0x1

//! Field EN_PORT4 - PORT4 Enable
#define PIB_PORT_EN_64_32_EN_PORT4_POS 4
//! Field EN_PORT4 - PORT4 Enable
#define PIB_PORT_EN_64_32_EN_PORT4_MASK 0x10u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT4_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT4_EN 0x1

//! Field EN_PORT5 - PORT5 Enable
#define PIB_PORT_EN_64_32_EN_PORT5_POS 5
//! Field EN_PORT5 - PORT5 Enable
#define PIB_PORT_EN_64_32_EN_PORT5_MASK 0x20u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT5_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT5_EN 0x1

//! Field EN_PORT6 - PORT6 Enable
#define PIB_PORT_EN_64_32_EN_PORT6_POS 6
//! Field EN_PORT6 - PORT6 Enable
#define PIB_PORT_EN_64_32_EN_PORT6_MASK 0x40u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT6_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT6_EN 0x1

//! Field EN_PORT7 - PORT7 Enable
#define PIB_PORT_EN_64_32_EN_PORT7_POS 7
//! Field EN_PORT7 - PORT7 Enable
#define PIB_PORT_EN_64_32_EN_PORT7_MASK 0x80u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT7_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT7_EN 0x1

//! Field EN_PORT8 - PORT8 Enable
#define PIB_PORT_EN_64_32_EN_PORT8_POS 8
//! Field EN_PORT8 - PORT8 Enable
#define PIB_PORT_EN_64_32_EN_PORT8_MASK 0x100u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT8_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT8_EN 0x1

//! Field EN_PORT9 - PORT9 Enable
#define PIB_PORT_EN_64_32_EN_PORT9_POS 9
//! Field EN_PORT9 - PORT9 Enable
#define PIB_PORT_EN_64_32_EN_PORT9_MASK 0x200u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT9_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT9_EN 0x1

//! Field EN_PORT10 - PORT10 Enable
#define PIB_PORT_EN_64_32_EN_PORT10_POS 10
//! Field EN_PORT10 - PORT10 Enable
#define PIB_PORT_EN_64_32_EN_PORT10_MASK 0x400u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT10_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT10_EN 0x1

//! Field EN_PORT11 - PORT11 Enable
#define PIB_PORT_EN_64_32_EN_PORT11_POS 11
//! Field EN_PORT11 - PORT11 Enable
#define PIB_PORT_EN_64_32_EN_PORT11_MASK 0x800u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT11_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT11_EN 0x1

//! Field EN_PORT12 - PORT12 Enable
#define PIB_PORT_EN_64_32_EN_PORT12_POS 12
//! Field EN_PORT12 - PORT12 Enable
#define PIB_PORT_EN_64_32_EN_PORT12_MASK 0x1000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT12_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT12_EN 0x1

//! Field EN_PORT13 - PORT13 Enable
#define PIB_PORT_EN_64_32_EN_PORT13_POS 13
//! Field EN_PORT13 - PORT13 Enable
#define PIB_PORT_EN_64_32_EN_PORT13_MASK 0x2000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT13_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT13_EN 0x1

//! Field EN_PORT14 - PORT14 Enable
#define PIB_PORT_EN_64_32_EN_PORT14_POS 14
//! Field EN_PORT14 - PORT14 Enable
#define PIB_PORT_EN_64_32_EN_PORT14_MASK 0x4000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT14_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT14_EN 0x1

//! Field EN_PORT15 - PORT15 Enable
#define PIB_PORT_EN_64_32_EN_PORT15_POS 15
//! Field EN_PORT15 - PORT15 Enable
#define PIB_PORT_EN_64_32_EN_PORT15_MASK 0x8000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT15_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT15_EN 0x1

//! Field EN_PORT16 - PORT16 Enable
#define PIB_PORT_EN_64_32_EN_PORT16_POS 16
//! Field EN_PORT16 - PORT16 Enable
#define PIB_PORT_EN_64_32_EN_PORT16_MASK 0x10000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT16_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT16_EN 0x1

//! Field EN_PORT17 - PORT17 Enable
#define PIB_PORT_EN_64_32_EN_PORT17_POS 17
//! Field EN_PORT17 - PORT17 Enable
#define PIB_PORT_EN_64_32_EN_PORT17_MASK 0x20000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT17_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT17_EN 0x1

//! Field EN_PORT18 - PORT18 Enable
#define PIB_PORT_EN_64_32_EN_PORT18_POS 18
//! Field EN_PORT18 - PORT18 Enable
#define PIB_PORT_EN_64_32_EN_PORT18_MASK 0x40000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT18_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT18_EN 0x1

//! Field EN_PORT19 - PORT19 Enable
#define PIB_PORT_EN_64_32_EN_PORT19_POS 19
//! Field EN_PORT19 - PORT19 Enable
#define PIB_PORT_EN_64_32_EN_PORT19_MASK 0x80000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT19_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT19_EN 0x1

//! Field EN_PORT20 - PORT20 Enable
#define PIB_PORT_EN_64_32_EN_PORT20_POS 20
//! Field EN_PORT20 - PORT20 Enable
#define PIB_PORT_EN_64_32_EN_PORT20_MASK 0x100000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT20_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT20_EN 0x1

//! Field EN_PORT21 - PORT21 Enable
#define PIB_PORT_EN_64_32_EN_PORT21_POS 21
//! Field EN_PORT21 - PORT21 Enable
#define PIB_PORT_EN_64_32_EN_PORT21_MASK 0x200000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT21_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT21_EN 0x1

//! Field EN_PORT22 - PORT22 Enable
#define PIB_PORT_EN_64_32_EN_PORT22_POS 22
//! Field EN_PORT22 - PORT22 Enable
#define PIB_PORT_EN_64_32_EN_PORT22_MASK 0x400000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT22_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT22_EN 0x1

//! Field EN_PORT23 - PORT23 Enable
#define PIB_PORT_EN_64_32_EN_PORT23_POS 23
//! Field EN_PORT23 - PORT23 Enable
#define PIB_PORT_EN_64_32_EN_PORT23_MASK 0x800000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT23_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT23_EN 0x1

//! Field EN_PORT24 - PORT24 Enable
#define PIB_PORT_EN_64_32_EN_PORT24_POS 24
//! Field EN_PORT24 - PORT24 Enable
#define PIB_PORT_EN_64_32_EN_PORT24_MASK 0x1000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT24_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT24_EN 0x1

//! Field EN_PORT25 - PORT25 Enable
#define PIB_PORT_EN_64_32_EN_PORT25_POS 25
//! Field EN_PORT25 - PORT25 Enable
#define PIB_PORT_EN_64_32_EN_PORT25_MASK 0x2000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT25_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT25_EN 0x1

//! Field EN_PORT26 - PORT26 Enable
#define PIB_PORT_EN_64_32_EN_PORT26_POS 26
//! Field EN_PORT26 - PORT26 Enable
#define PIB_PORT_EN_64_32_EN_PORT26_MASK 0x4000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT26_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT26_EN 0x1

//! Field EN_PORT27 - PORT27 Enable
#define PIB_PORT_EN_64_32_EN_PORT27_POS 27
//! Field EN_PORT27 - PORT27 Enable
#define PIB_PORT_EN_64_32_EN_PORT27_MASK 0x8000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT27_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT27_EN 0x1

//! Field EN_PORT28 - PORT28 Enable
#define PIB_PORT_EN_64_32_EN_PORT28_POS 28
//! Field EN_PORT28 - PORT28 Enable
#define PIB_PORT_EN_64_32_EN_PORT28_MASK 0x10000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT28_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT28_EN 0x1

//! Field EN_PORT29 - PORT29 Enable
#define PIB_PORT_EN_64_32_EN_PORT29_POS 29
//! Field EN_PORT29 - PORT29 Enable
#define PIB_PORT_EN_64_32_EN_PORT29_MASK 0x20000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT29_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT29_EN 0x1

//! Field EN_PORT30 - PORT30 Enable
#define PIB_PORT_EN_64_32_EN_PORT30_POS 30
//! Field EN_PORT30 - PORT30 Enable
#define PIB_PORT_EN_64_32_EN_PORT30_MASK 0x40000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT30_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT30_EN 0x1

//! Field EN_PORT31 - PORT31 Enable
#define PIB_PORT_EN_64_32_EN_PORT31_POS 31
//! Field EN_PORT31 - PORT31 Enable
#define PIB_PORT_EN_64_32_EN_PORT31_MASK 0x80000000u
//! Constant DIS - DIS
#define CONST_PIB_PORT_EN_64_32_EN_PORT31_DIS 0x0
//! Constant EN - EN
#define CONST_PIB_PORT_EN_64_32_EN_PORT31_EN 0x1

//! @}

//! \defgroup PORT_IRNCR_0 Register PORT_IRNCR_0 - PON Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define PORT_IRNCR_0 0x130
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNCR_0 0x19100130u

//! Register Reset Value
#define PORT_IRNCR_0_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNCR_0_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNCR_0_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_IRNICR_0 Register PORT_IRNICR_0 - PON Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define PORT_IRNICR_0 0x134
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNICR_0 0x19100134u

//! Register Reset Value
#define PORT_IRNICR_0_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNICR_0_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNICR_0_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_IRNEN_0 Register PORT_IRNEN_0 - PON Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define PORT_IRNEN_0 0x138
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNEN_0 0x19100138u

//! Register Reset Value
#define PORT_IRNEN_0_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNEN_0_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNEN_0_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_IRNCR_1 Register PORT_IRNCR_1 - PON Port IRN Capture Register
//! @{

//! Register Offset (relative)
#define PORT_IRNCR_1 0x140
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNCR_1 0x19100140u

//! Register Reset Value
#define PORT_IRNCR_1_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNCR_1_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNCR_1_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_IRNICR_1 Register PORT_IRNICR_1 - PON Port IRN Interrupt Control Register
//! @{

//! Register Offset (relative)
#define PORT_IRNICR_1 0x144
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNICR_1 0x19100144u

//! Register Reset Value
#define PORT_IRNICR_1_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNICR_1_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNICR_1_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_IRNEN_1 Register PORT_IRNEN_1 - PON Port IRN Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define PORT_IRNEN_1 0x148
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PORT_IRNEN_1 0x19100148u

//! Register Reset Value
#define PORT_IRNEN_1_RST 0x00000000u

//! Field PONPI - PON Port Interrupt
#define PORT_IRNEN_1_PONPI_POS 0
//! Field PONPI - PON Port Interrupt
#define PORT_IRNEN_1_PONPI_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PIB_DBG Register PIB_DBG - Hardware Debug Register
//! @{

//! Register Offset (relative)
#define PIB_DBG 0x920
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_DBG 0x19100920u

//! Register Reset Value
#define PIB_DBG_RST 0x00000000u

//! Field DBG - Debug
#define PIB_DBG_DBG_POS 0
//! Field DBG - Debug
#define PIB_DBG_DBG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PIB_TEST Register PIB_TEST - Hardware Test Register
//! @{

//! Register Offset (relative)
#define PIB_TEST 0x930
//! Register Offset (absolute) for 1st Instance CQEM_PIB
#define CQEM_PIB_PIB_TEST 0x19100930u

//! Register Reset Value
#define PIB_TEST_RST 0x00000000u

//! Field TEST - Test
#define PIB_TEST_TEST_POS 0
//! Field TEST - Test
#define PIB_TEST_TEST_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
