//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/umt.xml
// Register File Name  : UMT_CTRL
// Register File Title : UMT module Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _UMT_CTRL_H
#define _UMT_CTRL_H

//! \defgroup UMT_CTRL Register File UMT_CTRL - UMT module Register Description
//! @{

//! Base Address of CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_MODULE_BASE 0x19110000u

//! \defgroup UMT_HW_INT_STAT Register UMT_HW_INT_STAT
//! @{

//! Register Offset (relative)
#define UMT_HW_INT_STAT 0x9C
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_HW_INT_STAT 0x1911009Cu

//! Register Reset Value
#define UMT_HW_INT_STAT_RST 0x00000000u

//! Field MUMT00_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT00_ERR_POS 16
//! Field MUMT00_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT00_ERR_MASK 0x10000u

//! Field MUMT01_ERR
#define UMT_HW_INT_STAT_MUMT01_ERR_POS 17
//! Field MUMT01_ERR
#define UMT_HW_INT_STAT_MUMT01_ERR_MASK 0x20000u

//! Field MUMT10_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT10_ERR_POS 18
//! Field MUMT10_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT10_ERR_MASK 0x40000u

//! Field MUMT11_ERR
#define UMT_HW_INT_STAT_MUMT11_ERR_POS 19
//! Field MUMT11_ERR
#define UMT_HW_INT_STAT_MUMT11_ERR_MASK 0x80000u

//! Field MUMT20_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT20_ERR_POS 20
//! Field MUMT20_ERR - UMT HW Error
#define UMT_HW_INT_STAT_MUMT20_ERR_MASK 0x100000u

//! Field MUMT21_ERR
#define UMT_HW_INT_STAT_MUMT21_ERR_POS 21
//! Field MUMT21_ERR
#define UMT_HW_INT_STAT_MUMT21_ERR_MASK 0x200000u

//! @}

//! \defgroup UMT_SPARE_TIE0 Register UMT_SPARE_TIE0 - UMT Spare Register tie0
//! @{

//! Register Offset (relative)
#define UMT_SPARE_TIE0 0xA4
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_SPARE_TIE0 0x191100A4u

//! Register Reset Value
#define UMT_SPARE_TIE0_RST 0x00000000u

//! Field UMT_SPARE - SPARE register
#define UMT_SPARE_TIE0_UMT_SPARE_POS 0
//! Field UMT_SPARE - SPARE register
#define UMT_SPARE_TIE0_UMT_SPARE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DBG_D_TX0 Register UMT_DBG_D_TX0 - UMT Debug Descriptor TX0
//! @{

//! Register Offset (relative)
#define UMT_DBG_D_TX0 0xCC
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DBG_D_TX0 0x191100CCu

//! Register Reset Value
#define UMT_DBG_D_TX0_RST 0x00000000u

//! Field DWORD - Debug Descriptor TX Register
#define UMT_DBG_D_TX0_DWORD_POS 0
//! Field DWORD - Debug Descriptor TX Register
#define UMT_DBG_D_TX0_DWORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DBG_D_TX1 Register UMT_DBG_D_TX1 - UMT Debug Descriptor TX1
//! @{

//! Register Offset (relative)
#define UMT_DBG_D_TX1 0xD0
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DBG_D_TX1 0x191100D0u

//! Register Reset Value
#define UMT_DBG_D_TX1_RST 0x30000000u

//! Field DWORD - Debug Descriptor TX Register
#define UMT_DBG_D_TX1_DWORD_POS 0
//! Field DWORD - Debug Descriptor TX Register
#define UMT_DBG_D_TX1_DWORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DBG_D_RX0 Register UMT_DBG_D_RX0 - UMT Debug Descriptor RX
//! @{

//! Register Offset (relative)
#define UMT_DBG_D_RX0 0xD4
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DBG_D_RX0 0x191100D4u

//! Register Reset Value
#define UMT_DBG_D_RX0_RST 0x00000000u

//! Field DWORD - Debug Descriptor RX Register
#define UMT_DBG_D_RX0_DWORD_POS 0
//! Field DWORD - Debug Descriptor RX Register
#define UMT_DBG_D_RX0_DWORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DBG_D_RX1 Register UMT_DBG_D_RX1 - UMT Debug Descriptor RX
//! @{

//! Register Offset (relative)
#define UMT_DBG_D_RX1 0xD8
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DBG_D_RX1 0x191100D8u

//! Register Reset Value
#define UMT_DBG_D_RX1_RST 0x0000FFFFu

//! Field DWORD - Debug Descriptor RX Register
#define UMT_DBG_D_RX1_DWORD_POS 0
//! Field DWORD - Debug Descriptor RX Register
#define UMT_DBG_D_RX1_DWORD_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_PERD Register UMT_PERD - UMT SELF COUNTING MODE PERIOD
//! @{

//! Register Offset (relative)
#define UMT_PERD 0xDC
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_PERD 0x191100DCu

//! Register Reset Value
#define UMT_PERD_RST 0x000017ACu

//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERD_UMT_P_POS 0
//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERD_UMT_P_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_TRG_MUX Register UMT_TRG_MUX - UMT Event Trigger Mux Selection
//! @{

//! Register Offset (relative)
#define UMT_TRG_MUX 0xE0
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_TRG_MUX 0x191100E0u

//! Register Reset Value
#define UMT_TRG_MUX_RST 0x02100987u

//! Field UMT_RX0_SEL - Select DMA1TX Event for UMT RX0 Counter
#define UMT_TRG_MUX_UMT_RX0_SEL_POS 0
//! Field UMT_RX0_SEL - Select DMA1TX Event for UMT RX0 Counter
#define UMT_TRG_MUX_UMT_RX0_SEL_MASK 0xFu

//! Field UMT_RX1_SEL - Select DMA1TX Event for UMT RX1 Counter
#define UMT_TRG_MUX_UMT_RX1_SEL_POS 4
//! Field UMT_RX1_SEL - Select DMA1TX Event for UMT RX1 Counter
#define UMT_TRG_MUX_UMT_RX1_SEL_MASK 0xF0u

//! Field UMT_RX2_SEL - Select DMA1TX Event for UMT RX2 Counter
#define UMT_TRG_MUX_UMT_RX2_SEL_POS 8
//! Field UMT_RX2_SEL - Select DMA1TX Event for UMT RX2 Counter
#define UMT_TRG_MUX_UMT_RX2_SEL_MASK 0xF00u

//! Field UMT_TX0_SEL - Select CBM Event for UMT TX0 Counter
#define UMT_TRG_MUX_UMT_TX0_SEL_POS 16
//! Field UMT_TX0_SEL - Select CBM Event for UMT TX0 Counter
#define UMT_TRG_MUX_UMT_TX0_SEL_MASK 0xF0000u

//! Field UMT_TX1_SEL - Select CBM Event for UMT TX1 Counter
#define UMT_TRG_MUX_UMT_TX1_SEL_POS 20
//! Field UMT_TX1_SEL - Select CBM Event for UMT TX1 Counter
#define UMT_TRG_MUX_UMT_TX1_SEL_MASK 0xF00000u

//! Field UMT_TX2_SEL - Select CBM Event for UMT TX2 Counter
#define UMT_TRG_MUX_UMT_TX2_SEL_POS 24
//! Field UMT_TX2_SEL - Select CBM Event for UMT TX2 Counter
#define UMT_TRG_MUX_UMT_TX2_SEL_MASK 0xF000000u

//! @}

//! \defgroup UMT_COUNTER_CTRL Register UMT_COUNTER_CTRL - UMT Counter Control
//! @{

//! Register Offset (relative)
#define UMT_COUNTER_CTRL 0xE4
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_COUNTER_CTRL 0x191100E4u

//! Register Reset Value
#define UMT_COUNTER_CTRL_RST 0x00000000u

//! Field UMT_CH0_SND_DIS - Disable OCP Master from sending any UMT RX0/TX0 Counters
#define UMT_COUNTER_CTRL_UMT_CH0_SND_DIS_POS 0
//! Field UMT_CH0_SND_DIS - Disable OCP Master from sending any UMT RX0/TX0 Counters
#define UMT_COUNTER_CTRL_UMT_CH0_SND_DIS_MASK 0x1u
//! Constant SND - Enable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH0_SND_DIS_SND 0x0
//! Constant DISND - Disable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH0_SND_DIS_DISND 0x1

//! Field UMT_CH1_SND_DIS - Disable OCP Master from sending any UMT RX1/TX1 Counters
#define UMT_COUNTER_CTRL_UMT_CH1_SND_DIS_POS 1
//! Field UMT_CH1_SND_DIS - Disable OCP Master from sending any UMT RX1/TX1 Counters
#define UMT_COUNTER_CTRL_UMT_CH1_SND_DIS_MASK 0x2u
//! Constant SND - Enable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH1_SND_DIS_SND 0x0
//! Constant DISND - Disable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH1_SND_DIS_DISND 0x1

//! Field UMT_CH2_SND_DIS - Disable OCP Master from sending any UMT RX2/TX2 Counters
#define UMT_COUNTER_CTRL_UMT_CH2_SND_DIS_POS 2
//! Field UMT_CH2_SND_DIS - Disable OCP Master from sending any UMT RX2/TX2 Counters
#define UMT_COUNTER_CTRL_UMT_CH2_SND_DIS_MASK 0x4u
//! Constant SND - Enable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH2_SND_DIS_SND 0x0
//! Constant DISND - Disable Sending
#define CONST_UMT_COUNTER_CTRL_UMT_CH2_SND_DIS_DISND 0x1

//! Field UMT_CH0_CNTR_CLR - Clear UMT RX0/TX0 Counters
#define UMT_COUNTER_CTRL_UMT_CH0_CNTR_CLR_POS 4
//! Field UMT_CH0_CNTR_CLR - Clear UMT RX0/TX0 Counters
#define UMT_COUNTER_CTRL_UMT_CH0_CNTR_CLR_MASK 0x10u
//! Constant NCLR - NOT CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH0_CNTR_CLR_NCLR 0x0
//! Constant CLR - CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH0_CNTR_CLR_CLR 0x1

//! Field UMT_CH1_CNTR_CLR - Clear UMT RX1/TX1 Counters
#define UMT_COUNTER_CTRL_UMT_CH1_CNTR_CLR_POS 5
//! Field UMT_CH1_CNTR_CLR - Clear UMT RX1/TX1 Counters
#define UMT_COUNTER_CTRL_UMT_CH1_CNTR_CLR_MASK 0x20u
//! Constant NCLR - NOT CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH1_CNTR_CLR_NCLR 0x0
//! Constant CLR - CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH1_CNTR_CLR_CLR 0x1

//! Field UMT_CH2_CNTR_CLR - Clear UMT RX2/TX2 Counters
#define UMT_COUNTER_CTRL_UMT_CH2_CNTR_CLR_POS 6
//! Field UMT_CH2_CNTR_CLR - Clear UMT RX2/TX2 Counters
#define UMT_COUNTER_CTRL_UMT_CH2_CNTR_CLR_MASK 0x40u
//! Constant NCLR - NOT CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH2_CNTR_CLR_NCLR 0x0
//! Constant CLR - CLEAR
#define CONST_UMT_COUNTER_CTRL_UMT_CH2_CNTR_CLR_CLR 0x1

//! @}

//! \defgroup UMT_GCTRL Register UMT_GCTRL - Global Control Register
//! @{

//! Register Offset (relative)
#define UMT_GCTRL 0x200
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_GCTRL 0x19110200u

//! Register Reset Value
#define UMT_GCTRL_RST 0x10000000u

//! Field UMT_MD - UMT Mode
#define UMT_GCTRL_UMT_MD_POS 1
//! Field UMT_MD - UMT Mode
#define UMT_GCTRL_UMT_MD_MASK 0x2u
//! Constant CNT - Self Counting Mode
#define CONST_UMT_GCTRL_UMT_MD_CNT 0x0
//! Constant MSG - USER MSg MODE
#define CONST_UMT_GCTRL_UMT_MD_MSG 0x1

//! Field UMT_EN - ENABLE UMT
#define UMT_GCTRL_UMT_EN_POS 2
//! Field UMT_EN - ENABLE UMT
#define UMT_GCTRL_UMT_EN_MASK 0x4u
//! Constant DISU - DISABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN_DISU 0x0
//! Constant ENU - ENABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN_ENU 0x1

//! Field UMT_MSG_RRDY - UMT Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY_POS 3
//! Field UMT_MSG_RRDY - UMT Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY_MASK 0x8u
//! Constant NRDY - Not ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY_NRDY 0x0
//! Constant RDY - ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY_RDY 0x1

//! Field UMT_RST - Reset UMT
#define UMT_GCTRL_UMT_RST_POS 4
//! Field UMT_RST - Reset UMT
#define UMT_GCTRL_UMT_RST_MASK 0x10u
//! Constant MNRST - SW writes 0 to release of reset
#define CONST_UMT_GCTRL_UMT_RST_MNRST 0x0
//! Constant MRST - SW writes 1 to reset
#define CONST_UMT_GCTRL_UMT_RST_MRST 0x1

//! Field DEBUG_STEP - Freeze all running FSM for debugging
#define UMT_GCTRL_DEBUG_STEP_POS 5
//! Field DEBUG_STEP - Freeze all running FSM for debugging
#define UMT_GCTRL_DEBUG_STEP_MASK 0x20u
//! Constant OVER - step complete
#define CONST_UMT_GCTRL_DEBUG_STEP_OVER 0x0
//! Constant STEP - step ahead
#define CONST_UMT_GCTRL_DEBUG_STEP_STEP 0x1

//! Field DEBUG_PAUSE - PAUSE HW for Debug Access
#define UMT_GCTRL_DEBUG_PAUSE_POS 6
//! Field DEBUG_PAUSE - PAUSE HW for Debug Access
#define UMT_GCTRL_DEBUG_PAUSE_MASK 0x40u
//! Constant NORM - Normal operating
#define CONST_UMT_GCTRL_DEBUG_PAUSE_NORM 0x0
//! Constant PAUSE - force HW pause
#define CONST_UMT_GCTRL_DEBUG_PAUSE_PAUSE 0x1

//! Field PORT_EN - Enable UMT Ports
#define UMT_GCTRL_PORT_EN_POS 8
//! Field PORT_EN - Enable UMT Ports
#define UMT_GCTRL_PORT_EN_MASK 0x700u

//! Field UMT_MD1 - UMT Mode for ch1
#define UMT_GCTRL_UMT_MD1_POS 16
//! Field UMT_MD1 - UMT Mode for ch1
#define UMT_GCTRL_UMT_MD1_MASK 0x10000u
//! Constant CNT - Self Counting Mode
#define CONST_UMT_GCTRL_UMT_MD1_CNT 0x0
//! Constant MSG - USER MSg MODE
#define CONST_UMT_GCTRL_UMT_MD1_MSG 0x1

//! Field UMT_EN1 - ENABLE UMT for ch1
#define UMT_GCTRL_UMT_EN1_POS 17
//! Field UMT_EN1 - ENABLE UMT for ch1
#define UMT_GCTRL_UMT_EN1_MASK 0x20000u
//! Constant DISU - DISABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN1_DISU 0x0
//! Constant ENU - ENABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN1_ENU 0x1

//! Field UMT_MSG_RRDY1 - UMT ch 1 Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY1_POS 18
//! Field UMT_MSG_RRDY1 - UMT ch 1 Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY1_MASK 0x40000u
//! Constant NRDY - Not ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY1_NRDY 0x0
//! Constant RDY - ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY1_RDY 0x1

//! Field UMT_MD2 - UMT Mode for ch2
#define UMT_GCTRL_UMT_MD2_POS 19
//! Field UMT_MD2 - UMT Mode for ch2
#define UMT_GCTRL_UMT_MD2_MASK 0x80000u
//! Constant CNT - Self Counting Mode
#define CONST_UMT_GCTRL_UMT_MD2_CNT 0x0
//! Constant MSG - USER MSg MODE
#define CONST_UMT_GCTRL_UMT_MD2_MSG 0x1

//! Field UMT_EN2 - ENABLE UMT for ch2
#define UMT_GCTRL_UMT_EN2_POS 20
//! Field UMT_EN2 - ENABLE UMT for ch2
#define UMT_GCTRL_UMT_EN2_MASK 0x100000u
//! Constant DISU - DISABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN2_DISU 0x0
//! Constant ENU - ENABLE UMT PORT
#define CONST_UMT_GCTRL_UMT_EN2_ENU 0x1

//! Field UMT_MSG_RRDY2 - UMT ch 2 Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY2_POS 21
//! Field UMT_MSG_RRDY2 - UMT ch 2 Message taken by Hardware Ready for SW Re-write
#define UMT_GCTRL_UMT_MSG_RRDY2_MASK 0x200000u
//! Constant NRDY - Not ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY2_NRDY 0x0
//! Constant RDY - ready for re-write
#define CONST_UMT_GCTRL_UMT_MSG_RRDY2_RDY 0x1

//! Field OCP_UMT_ENDI_B - Control OCP SWAPPER byte for UMT
#define UMT_GCTRL_OCP_UMT_ENDI_B_POS 28
//! Field OCP_UMT_ENDI_B - Control OCP SWAPPER byte for UMT
#define UMT_GCTRL_OCP_UMT_ENDI_B_MASK 0x10000000u
//! Constant NO_SWP
#define CONST_UMT_GCTRL_OCP_UMT_ENDI_B_NO_SWP 0x0
//! Constant SWAP
#define CONST_UMT_GCTRL_OCP_UMT_ENDI_B_SWAP 0x1

//! Field OCP_UMT_ENDI_W - Control OCP SWAPPER WORD for UMT
#define UMT_GCTRL_OCP_UMT_ENDI_W_POS 29
//! Field OCP_UMT_ENDI_W - Control OCP SWAPPER WORD for UMT
#define UMT_GCTRL_OCP_UMT_ENDI_W_MASK 0x20000000u
//! Constant NO_SWP
#define CONST_UMT_GCTRL_OCP_UMT_ENDI_W_NO_SWP 0x0
//! Constant SWAP
#define CONST_UMT_GCTRL_OCP_UMT_ENDI_W_SWAP 0x1

//! @}

//! \defgroup UMT_INT_MASK Register UMT_INT_MASK - Module Interrupt Mask register
//! @{

//! Register Offset (relative)
#define UMT_INT_MASK 0x204
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_INT_MASK 0x19110204u

//! Register Reset Value
#define UMT_INT_MASK_RST 0x00000000u

//! Field S18 - Interrupt Mask bit
#define UMT_INT_MASK_S18_POS 18
//! Field S18 - Interrupt Mask bit
#define UMT_INT_MASK_S18_MASK 0x40000u
//! Constant UM - Mask out this bit
#define CONST_UMT_INT_MASK_S18_UM 0x0
//! Constant M - Mask to Select this bit
#define CONST_UMT_INT_MASK_S18_M 0x1

//! @}

//! \defgroup UMT_INTERNAL_INT_MASK Register UMT_INTERNAL_INT_MASK - Module Internal Interrupt Mask Register
//! @{

//! Register Offset (relative)
#define UMT_INTERNAL_INT_MASK 0x208
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_INTERNAL_INT_MASK 0x19110208u

//! Register Reset Value
#define UMT_INTERNAL_INT_MASK_RST 0x00000000u

//! Field S18 - Internal Interrupt Mask bit
#define UMT_INTERNAL_INT_MASK_S18_POS 18
//! Field S18 - Internal Interrupt Mask bit
#define UMT_INTERNAL_INT_MASK_S18_MASK 0x40000u
//! Constant UM - Mask out this bit
#define CONST_UMT_INTERNAL_INT_MASK_S18_UM 0x0
//! Constant M - Mask to select this bit
#define CONST_UMT_INTERNAL_INT_MASK_S18_M 0x1

//! @}

//! \defgroup UMT_INT_EN Register UMT_INT_EN - Module Interrupt EN register
//! @{

//! Register Offset (relative)
#define UMT_INT_EN 0x20C
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_INT_EN 0x1911020Cu

//! Register Reset Value
#define UMT_INT_EN_RST 0x00000000u

//! Field S18 - Interrupt Enable bit
#define UMT_INT_EN_S18_POS 18
//! Field S18 - Interrupt Enable bit
#define UMT_INT_EN_S18_MASK 0x40000u
//! Constant UEN - Disable this bit
#define CONST_UMT_INT_EN_S18_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_UMT_INT_EN_S18_EN 0x1

//! @}

//! \defgroup UMT_INTERNAL_INT_EN Register UMT_INTERNAL_INT_EN - Module Internal Interrupt Enable Register
//! @{

//! Register Offset (relative)
#define UMT_INTERNAL_INT_EN 0x210
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_INTERNAL_INT_EN 0x19110210u

//! Register Reset Value
#define UMT_INTERNAL_INT_EN_RST 0x00000000u

//! Field P0Y - Internal Interrupt Enable Bit
#define UMT_INTERNAL_INT_EN_P0Y_POS 0
//! Field P0Y - Internal Interrupt Enable Bit
#define UMT_INTERNAL_INT_EN_P0Y_MASK 0x1u
//! Constant UEN - Disable this bit
#define CONST_UMT_INTERNAL_INT_EN_P0Y_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_UMT_INTERNAL_INT_EN_P0Y_EN 0x1

//! Field S18 - Internal Interrupt Enable Bit
#define UMT_INTERNAL_INT_EN_S18_POS 18
//! Field S18 - Internal Interrupt Enable Bit
#define UMT_INTERNAL_INT_EN_S18_MASK 0x40000u
//! Constant UEN - Disable this bit
#define CONST_UMT_INTERNAL_INT_EN_S18_UEN 0x0
//! Constant EN - Enable this bit
#define CONST_UMT_INTERNAL_INT_EN_S18_EN 0x1

//! @}

//! \defgroup UMT_INT_STAT Register UMT_INT_STAT - Module Interrupt States register
//! @{

//! Register Offset (relative)
#define UMT_INT_STAT 0x214
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_INT_STAT 0x19110214u

//! Register Reset Value
#define UMT_INT_STAT_RST 0x00000000u

//! Field S18 - Interrupt States bit
#define UMT_INT_STAT_S18_POS 18
//! Field S18 - Interrupt States bit
#define UMT_INT_STAT_S18_MASK 0x40000u
//! Constant UEN - Disabled this bit
#define CONST_UMT_INT_STAT_S18_UEN 0x0
//! Constant EN - UMT Error
#define CONST_UMT_INT_STAT_S18_EN 0x1

//! @}

//! \defgroup UMT_SW_MODE Register UMT_SW_MODE - UMT Software Mode
//! @{

//! Register Offset (relative)
#define UMT_SW_MODE 0x218
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_SW_MODE 0x19110218u

//! Register Reset Value
#define UMT_SW_MODE_RST 0x00000003u

//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_SW_MSG_MD_POS 0
//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_SW_MSG_MD_MASK 0x3u
//! Constant NON - no msg
#define CONST_UMT_SW_MODE_SW_MSG_MD_NON 0x0
//! Constant MSG0 - MSG0 word to be programmed
#define CONST_UMT_SW_MODE_SW_MSG_MD_MSG0 0x1
//! Constant MSG1 - MSG1 to be programmed
#define CONST_UMT_SW_MODE_SW_MSG_MD_MSG1 0x2
//! Constant MSG11 - Both msg to be programmed
#define CONST_UMT_SW_MODE_SW_MSG_MD_MSG11 0x3

//! @}

//! \defgroup UMT_MSG0 Register UMT_MSG0 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG0 0x220
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG0 0x19110220u

//! Register Reset Value
#define UMT_MSG0_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG0_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG0_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_MSG1 Register UMT_MSG1 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG1 0x224
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG1 0x19110224u

//! Register Reset Value
#define UMT_MSG1_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG1_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG1_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DEST Register UMT_DEST - UMT_DST
//! @{

//! Register Offset (relative)
#define UMT_DEST 0x230
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DEST 0x19110230u

//! Register Reset Value
#define UMT_DEST_RST 0x00000000u

//! Field UMT_TA - Target address of UMT
#define UMT_DEST_UMT_TA_POS 0
//! Field UMT_TA - Target address of UMT
#define UMT_DEST_UMT_TA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_TO_CNT_0 Register PORT_TO_CNT_0 - Primary interrupt timeout counter
//! @{

//! Register Offset (relative)
#define PORT_TO_CNT_0 0x300
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_PORT_TO_CNT_0 0x19110300u

//! Register Reset Value
#define PORT_TO_CNT_0_RST 0x00000000u

//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_0_TO_C_POS 0
//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_0_TO_C_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_TO_CNT_1 Register PORT_TO_CNT_1 - Primary interrupt timeout counter
//! @{

//! Register Offset (relative)
#define PORT_TO_CNT_1 0x310
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_PORT_TO_CNT_1 0x19110310u

//! Register Reset Value
#define PORT_TO_CNT_1_RST 0x00000000u

//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_1_TO_C_POS 0
//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_1_TO_C_MASK 0xFFFFFFFFu

//! @}

//! \defgroup PORT_TO_CNT_2 Register PORT_TO_CNT_2 - Primary interrupt timeout counter
//! @{

//! Register Offset (relative)
#define PORT_TO_CNT_2 0x320
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_PORT_TO_CNT_2 0x19110320u

//! Register Reset Value
#define PORT_TO_CNT_2_RST 0x00000000u

//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_2_TO_C_POS 0
//! Field TO_C - Timeout counter value for primary interrupt
#define PORT_TO_CNT_2_TO_C_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_MSG1_0 Register UMT_MSG1_0 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG1_0 0x400
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG1_0 0x19110400u

//! Register Reset Value
#define UMT_MSG1_0_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG1_0_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG1_0_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_MSG1_1 Register UMT_MSG1_1 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG1_1 0x404
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG1_1 0x19110404u

//! Register Reset Value
#define UMT_MSG1_1_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG1_1_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG1_1_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_PERIOD_CH1 Register UMT_PERIOD_CH1 - UMT SELF COUNTING MODE PERIOD
//! @{

//! Register Offset (relative)
#define UMT_PERIOD_CH1 0x420
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_PERIOD_CH1 0x19110420u

//! Register Reset Value
#define UMT_PERIOD_CH1_RST 0x000017ACu

//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERIOD_CH1_UMT_P_POS 0
//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERIOD_CH1_UMT_P_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DEST_1 Register UMT_DEST_1 - UMT_DST
//! @{

//! Register Offset (relative)
#define UMT_DEST_1 0x430
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DEST_1 0x19110430u

//! Register Reset Value
#define UMT_DEST_1_RST 0x00000000u

//! Field UMT_TA - Target address of UMT
#define UMT_DEST_1_UMT_TA_POS 0
//! Field UMT_TA - Target address of UMT
#define UMT_DEST_1_UMT_TA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_SW_MODE_CH1 Register UMT_SW_MODE_CH1 - UMT Software Mode
//! @{

//! Register Offset (relative)
#define UMT_SW_MODE_CH1 0x434
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_SW_MODE_CH1 0x19110434u

//! Register Reset Value
#define UMT_SW_MODE_CH1_RST 0x00000003u

//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_CH1_SW_MSG_MD_POS 0
//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_CH1_SW_MSG_MD_MASK 0x3u
//! Constant NON - no msg
#define CONST_UMT_SW_MODE_CH1_SW_MSG_MD_NON 0x0
//! Constant MSG0 - MSG0 word to be programmed
#define CONST_UMT_SW_MODE_CH1_SW_MSG_MD_MSG0 0x1
//! Constant MSG1 - MSG1 to be programmed
#define CONST_UMT_SW_MODE_CH1_SW_MSG_MD_MSG1 0x2
//! Constant MSG11 - Both msg to be programmed
#define CONST_UMT_SW_MODE_CH1_SW_MSG_MD_MSG11 0x3

//! @}

//! \defgroup UMT_MSG2_0 Register UMT_MSG2_0 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG2_0 0x500
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG2_0 0x19110500u

//! Register Reset Value
#define UMT_MSG2_0_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG2_0_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG2_0_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_MSG2_1 Register UMT_MSG2_1 - USER Message Contents
//! @{

//! Register Offset (relative)
#define UMT_MSG2_1 0x504
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_MSG2_1 0x19110504u

//! Register Reset Value
#define UMT_MSG2_1_RST 0x00000000u

//! Field MSG - USER MSG
#define UMT_MSG2_1_MSG_POS 0
//! Field MSG - USER MSG
#define UMT_MSG2_1_MSG_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_PERIOD_CH2 Register UMT_PERIOD_CH2 - UMT SELF COUNTING MODE PERIOD
//! @{

//! Register Offset (relative)
#define UMT_PERIOD_CH2 0x520
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_PERIOD_CH2 0x19110520u

//! Register Reset Value
#define UMT_PERIOD_CH2_RST 0x000017ACu

//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERIOD_CH2_UMT_P_POS 0
//! Field UMT_P - PERIOD for UMT Self Counting MODE
#define UMT_PERIOD_CH2_UMT_P_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_DEST_2 Register UMT_DEST_2 - UMT_DST
//! @{

//! Register Offset (relative)
#define UMT_DEST_2 0x530
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_DEST_2 0x19110530u

//! Register Reset Value
#define UMT_DEST_2_RST 0x00000000u

//! Field UMT_TA - Target address of UMT
#define UMT_DEST_2_UMT_TA_POS 0
//! Field UMT_TA - Target address of UMT
#define UMT_DEST_2_UMT_TA_MASK 0xFFFFFFFFu

//! @}

//! \defgroup UMT_SW_MODE_CH2 Register UMT_SW_MODE_CH2 - UMT Software Mode
//! @{

//! Register Offset (relative)
#define UMT_SW_MODE_CH2 0x534
//! Register Offset (absolute) for 1st Instance CQEM_UMT_CTRL
#define CQEM_UMT_CTRL_UMT_SW_MODE_CH2 0x19110534u

//! Register Reset Value
#define UMT_SW_MODE_CH2_RST 0x00000003u

//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_CH2_SW_MSG_MD_POS 0
//! Field SW_MSG_MD - UMT software Msg mode
#define UMT_SW_MODE_CH2_SW_MSG_MD_MASK 0x3u
//! Constant NON - no msg
#define CONST_UMT_SW_MODE_CH2_SW_MSG_MD_NON 0x0
//! Constant MSG0 - MSG0 word to be programmed
#define CONST_UMT_SW_MODE_CH2_SW_MSG_MD_MSG0 0x1
//! Constant MSG1 - MSG1 to be programmed
#define CONST_UMT_SW_MODE_CH2_SW_MSG_MD_MSG1 0x2
//! Constant MSG11 - Both msg to be programmed
#define CONST_UMT_SW_MODE_CH2_SW_MSG_MD_MSG11 0x3

//! @}

//! @}

#endif
