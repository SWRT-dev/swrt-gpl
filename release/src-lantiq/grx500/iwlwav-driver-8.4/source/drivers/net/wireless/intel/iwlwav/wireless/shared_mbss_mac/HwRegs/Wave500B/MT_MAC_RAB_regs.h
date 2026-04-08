/*******************************************************************
*                                                                   
*  MAC_RAB.h
*   Chip Name: 'Hyperion'                                   
*   Do not modify this file directly                                
*   To update the file, modify registers excel file                 
*   and run the macro 'Create H Files for MIPS SW'              
*   Written by: Avri Golan                                          
*                                                                   
*******************************************************************/

/* Production Date: 11/08/2013  */

#ifndef MAC_RAB_regs_H_FILE
#define MAC_RAB_regs_H_FILE

/* UPI_INTERRUPT */
/* Write 1 to interrupt UPI */
#define REG_UPI_INTERRUPT          0x0008
#define REG_UPI_INTERRUPT_MASK     0x00ffffff
#define REG_UPI_INTERRUPT_SHIFT    0
#define REG_UPI_INTERRUPT_WIDTH    24

/* UPI_INTERRUPT_CLEAR */
/* Write 1 to clear above */
#define REG_UPI_INTERRUPT_CLEAR          0x000c
#define REG_UPI_INTERRUPT_CLEAR_MASK     0x00ffffff
#define REG_UPI_INTERRUPT_CLEAR_SHIFT    0
#define REG_UPI_INTERRUPT_CLEAR_WIDTH    24

/* LPI_INTERRUPT */
/* Write 1 to interrupt LPI */
#define REG_LPI_INTERRUPT          0x0010
#define REG_LPI_INTERRUPT_MASK     0x00ffffff
#define REG_LPI_INTERRUPT_SHIFT    0
#define REG_LPI_INTERRUPT_WIDTH    24

/* LPI_INTERRUPT_CLEAR */
/* Write 1 to clear above */
#define REG_LPI_INTERRUPT_CLEAR          0x0014
#define REG_LPI_INTERRUPT_CLEAR_MASK     0x00ffffff
#define REG_LPI_INTERRUPT_CLEAR_SHIFT    0
#define REG_LPI_INTERRUPT_CLEAR_WIDTH    24

/* PHI_INTERRUPT */
/* Write 1 to interrupt HOST */
#define REG_PHI_INTERRUPT          0x0018
#define REG_PHI_INTERRUPT_MASK     0x00ffffff
#define REG_PHI_INTERRUPT_SHIFT    0
#define REG_PHI_INTERRUPT_WIDTH    24

/* PHI_INTERRUPT_CLEAR */
/* Write 1 to clear above */
#define REG_PHI_INTERRUPT_CLEAR          0x001c
#define REG_PHI_INTERRUPT_CLEAR_MASK     0x00ffffff
#define REG_PHI_INTERRUPT_CLEAR_SHIFT    0
#define REG_PHI_INTERRUPT_CLEAR_WIDTH    24

/* NPU2UPI_INTERRUPT */
/* Write 1 to interrupt */
#define REG_NPU2UPI_INTERRUPT          0x0020
#define REG_NPU2UPI_INTERRUPT_MASK     0x00ffffff
#define REG_NPU2UPI_INTERRUPT_SHIFT    0
#define REG_NPU2UPI_INTERRUPT_WIDTH    24

/* NPU2UPI_INTERRUPT_CLEAR */
/* Write 1 to clear above */
#define REG_NPU2UPI_INTERRUPT_CLEAR          0x0024
#define REG_NPU2UPI_INTERRUPT_CLEAR_MASK     0x00ffffff
#define REG_NPU2UPI_INTERRUPT_CLEAR_SHIFT    0
#define REG_NPU2UPI_INTERRUPT_CLEAR_WIDTH    24

/* ENABLE_NPU2UPI_INTERRUPT */
#define REG_ENABLE_NPU2UPI_INTERRUPT          0x0028
#define REG_ENABLE_NPU2UPI_INTERRUPT_MASK     0x00ffffff
#define REG_ENABLE_NPU2UPI_INTERRUPT_SHIFT    0
#define REG_ENABLE_NPU2UPI_INTERRUPT_WIDTH    24

/* REVISION */
/* Revision number register for PAS and BRC */
#define REG_REVISION          0x0040
#define REG_REVISION_MASK     0x00000fff
#define REG_REVISION_SHIFT    0
#define REG_REVISION_WIDTH    12

/* PAS_SECURE_WRITE */
/* Write 0xaaaa, then 0x5555, then to secured address */
#define REG_PAS_SECURE_WRITE          0x0044
#define REG_PAS_SECURE_WRITE_MASK     0x0000ffff
#define REG_PAS_SECURE_WRITE_SHIFT    0
#define REG_PAS_SECURE_WRITE_WIDTH    16

/* SW_RESET_NTD */
/* Write 0 to reset the unit */
#define REG_SW_RESET_NTD          0x0048
#define REG_SW_RESET_NTD_MASK     0x00000001
#define REG_SW_RESET_NTD_SHIFT    0
#define REG_SW_RESET_NTD_WIDTH    1

/* SW_RESET_TXC */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TXC          0x0048
#define REG_SW_RESET_TXC_MASK     0x00000004
#define REG_SW_RESET_TXC_SHIFT    2
#define REG_SW_RESET_TXC_WIDTH    1

/* SW_RESET_TXC_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TXC_REG          0x0048
#define REG_SW_RESET_TXC_REG_MASK     0x00000008
#define REG_SW_RESET_TXC_REG_SHIFT    3
#define REG_SW_RESET_TXC_REG_WIDTH    1

/* SW_RESET_TIM */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TIM          0x0048
#define REG_SW_RESET_TIM_MASK     0x00000010
#define REG_SW_RESET_TIM_SHIFT    4
#define REG_SW_RESET_TIM_WIDTH    1

/* SW_RESET_TIM_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TIM_REG          0x0048
#define REG_SW_RESET_TIM_REG_MASK     0x00000020
#define REG_SW_RESET_TIM_REG_SHIFT    5
#define REG_SW_RESET_TIM_REG_WIDTH    1

/* SW_RESET_DEL */
/* Write 0 to reset the unit */
#define REG_SW_RESET_DEL          0x0048
#define REG_SW_RESET_DEL_MASK     0x00000040
#define REG_SW_RESET_DEL_SHIFT    6
#define REG_SW_RESET_DEL_WIDTH    1

/* SW_RESET_DEL_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_DEL_REG          0x0048
#define REG_SW_RESET_DEL_REG_MASK     0x00000080
#define REG_SW_RESET_DEL_REG_SHIFT    7
#define REG_SW_RESET_DEL_REG_WIDTH    1

/* SW_RESET_TCC */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TCC          0x0048
#define REG_SW_RESET_TCC_MASK     0x00000100
#define REG_SW_RESET_TCC_SHIFT    8
#define REG_SW_RESET_TCC_WIDTH    1

/* SW_RESET_RXC */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXC          0x0048
#define REG_SW_RESET_RXC_MASK     0x00000400
#define REG_SW_RESET_RXC_SHIFT    10
#define REG_SW_RESET_RXC_WIDTH    1

/* SW_RESET_RXC_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXC_REG          0x0048
#define REG_SW_RESET_RXC_REG_MASK     0x00000800
#define REG_SW_RESET_RXC_REG_SHIFT    11
#define REG_SW_RESET_RXC_REG_WIDTH    1

/* SW_RESET_RXF */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXF          0x0048
#define REG_SW_RESET_RXF_MASK     0x00001000
#define REG_SW_RESET_RXF_SHIFT    12
#define REG_SW_RESET_RXF_WIDTH    1

/* SW_RESET_RXF_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXF_REG          0x0048
#define REG_SW_RESET_RXF_REG_MASK     0x00002000
#define REG_SW_RESET_RXF_REG_SHIFT    13
#define REG_SW_RESET_RXF_REG_WIDTH    1

/* SW_RESET_RCC */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RCC          0x0048
#define REG_SW_RESET_RCC_MASK     0x00004000
#define REG_SW_RESET_RCC_SHIFT    14
#define REG_SW_RESET_RCC_WIDTH    1

/* SW_RESET_RXD */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXD          0x0048
#define REG_SW_RESET_RXD_MASK     0x00010000
#define REG_SW_RESET_RXD_SHIFT    16
#define REG_SW_RESET_RXD_WIDTH    1

/* SW_RESET_RXD_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RXD_REG          0x0048
#define REG_SW_RESET_RXD_REG_MASK     0x00020000
#define REG_SW_RESET_RXD_REG_SHIFT    17
#define REG_SW_RESET_RXD_REG_WIDTH    1

/* SW_RESET_DUR */
/* Write 0 to reset the unit */
#define REG_SW_RESET_DUR          0x0048
#define REG_SW_RESET_DUR_MASK     0x00040000
#define REG_SW_RESET_DUR_SHIFT    18
#define REG_SW_RESET_DUR_WIDTH    1

/* SW_RESET_DUR_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_DUR_REG          0x0048
#define REG_SW_RESET_DUR_REG_MASK     0x00080000
#define REG_SW_RESET_DUR_REG_SHIFT    19
#define REG_SW_RESET_DUR_REG_WIDTH    1

/* SW_RESET_RTA */
/* Write 0 to reset the unit */
#define REG_SW_RESET_RTA          0x0048
#define REG_SW_RESET_RTA_MASK     0x00100000
#define REG_SW_RESET_RTA_SHIFT    20
#define REG_SW_RESET_RTA_WIDTH    1

/* SW_RESET_BF */
/* Write 0 to reset the unit */
#define REG_SW_RESET_BF          0x0048
#define REG_SW_RESET_BF_MASK     0x00200000
#define REG_SW_RESET_BF_SHIFT    21
#define REG_SW_RESET_BF_WIDTH    1

/* SW_RESET_WEP */
/* Write 0 to reset the unit */
#define REG_SW_RESET_WEP          0x0048
#define REG_SW_RESET_WEP_MASK     0x00400000
#define REG_SW_RESET_WEP_SHIFT    22
#define REG_SW_RESET_WEP_WIDTH    1

/* SW_RESET_AGG_BUILDER */
/* Write 0 to reset the unit */
#define REG_SW_RESET_AGG_BUILDER          0x0048
#define REG_SW_RESET_AGG_BUILDER_MASK     0x00800000
#define REG_SW_RESET_AGG_BUILDER_SHIFT    23
#define REG_SW_RESET_AGG_BUILDER_WIDTH    1

/* SW_RESET_AGG_BUILDER_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_AGG_BUILDER_REG          0x0048
#define REG_SW_RESET_AGG_BUILDER_REG_MASK     0x01000000
#define REG_SW_RESET_AGG_BUILDER_REG_SHIFT    24
#define REG_SW_RESET_AGG_BUILDER_REG_WIDTH    1

/* SW_RESET_ADDR2INDEX */
/* Write 0 to reset the unit */
#define REG_SW_RESET_ADDR2INDEX          0x0048
#define REG_SW_RESET_ADDR2INDEX_MASK     0x02000000
#define REG_SW_RESET_ADDR2INDEX_SHIFT    25
#define REG_SW_RESET_ADDR2INDEX_WIDTH    1

/* SW_RESET_ADDR2INDEX_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_ADDR2INDEX_REG          0x0048
#define REG_SW_RESET_ADDR2INDEX_REG_MASK     0x04000000
#define REG_SW_RESET_ADDR2INDEX_REG_SHIFT    26
#define REG_SW_RESET_ADDR2INDEX_REG_WIDTH    1

/* SW_RESET_TX_HANDLER */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TX_HANDLER          0x0048
#define REG_SW_RESET_TX_HANDLER_MASK     0x08000000
#define REG_SW_RESET_TX_HANDLER_SHIFT    27
#define REG_SW_RESET_TX_HANDLER_WIDTH    1

/* SW_RESET_TX_HANDLER_REG */
/* Write 0 to reset the unit */
#define REG_SW_RESET_TX_HANDLER_REG          0x0048
#define REG_SW_RESET_TX_HANDLER_REG_MASK     0x10000000
#define REG_SW_RESET_TX_HANDLER_REG_SHIFT    28
#define REG_SW_RESET_TX_HANDLER_REG_WIDTH    1

/* ENABLE_UPI_INTERRUPT */
#define REG_ENABLE_UPI_INTERRUPT          0x004c
#define REG_ENABLE_UPI_INTERRUPT_MASK     0x00ffffff
#define REG_ENABLE_UPI_INTERRUPT_SHIFT    0
#define REG_ENABLE_UPI_INTERRUPT_WIDTH    24

/* ENABLE_LPI_INTERRUPT */
#define REG_ENABLE_LPI_INTERRUPT          0x0050
#define REG_ENABLE_LPI_INTERRUPT_MASK     0x00ffffff
#define REG_ENABLE_LPI_INTERRUPT_SHIFT    0
#define REG_ENABLE_LPI_INTERRUPT_WIDTH    24

/* ENABLE_PHI_INTERRUPT */
#define REG_ENABLE_PHI_INTERRUPT          0x0054
#define REG_ENABLE_PHI_INTERRUPT_MASK     0x00ffffff
#define REG_ENABLE_PHI_INTERRUPT_SHIFT    0
#define REG_ENABLE_PHI_INTERRUPT_WIDTH    24

/* UPPER_CPU_PB_ONOFF_N_MODE */
/* Secure write */
#define REG_UPPER_CPU_PB_ONOFF_N_MODE          0x0058
#define REG_UPPER_CPU_PB_ONOFF_N_MODE_MASK     0x00000001
#define REG_UPPER_CPU_PB_ONOFF_N_MODE_SHIFT    0
#define REG_UPPER_CPU_PB_ONOFF_N_MODE_WIDTH    1

/* UPPER_CPU_CONST_RESET_N_VALUE */
/* Secure write */
#define REG_UPPER_CPU_CONST_RESET_N_VALUE          0x0058
#define REG_UPPER_CPU_CONST_RESET_N_VALUE_MASK     0x00000002
#define REG_UPPER_CPU_CONST_RESET_N_VALUE_SHIFT    1
#define REG_UPPER_CPU_CONST_RESET_N_VALUE_WIDTH    1

/* UPPER_CPU_REQUIRED_BOOT_MODE */
/* Secure write */
#define REG_UPPER_CPU_REQUIRED_BOOT_MODE          0x0058
#define REG_UPPER_CPU_REQUIRED_BOOT_MODE_MASK     0x0000000c
#define REG_UPPER_CPU_REQUIRED_BOOT_MODE_SHIFT    2
#define REG_UPPER_CPU_REQUIRED_BOOT_MODE_WIDTH    2

/* UPPER_CPU_OVERRIDE_HOST_MODE */
/* Secure write */
#define REG_UPPER_CPU_OVERRIDE_HOST_MODE          0x0058
#define REG_UPPER_CPU_OVERRIDE_HOST_MODE_MASK     0x00000010
#define REG_UPPER_CPU_OVERRIDE_HOST_MODE_SHIFT    4
#define REG_UPPER_CPU_OVERRIDE_HOST_MODE_WIDTH    1

/* LOWER_CPU_PB_ONOFF_N_MODE */
/* Secure write */
#define REG_LOWER_CPU_PB_ONOFF_N_MODE          0x0058
#define REG_LOWER_CPU_PB_ONOFF_N_MODE_MASK     0x00000100
#define REG_LOWER_CPU_PB_ONOFF_N_MODE_SHIFT    8
#define REG_LOWER_CPU_PB_ONOFF_N_MODE_WIDTH    1

/* LOWER_CPU_CONST_RESET_N_VALUE */
/* Secure write */
#define REG_LOWER_CPU_CONST_RESET_N_VALUE          0x0058
#define REG_LOWER_CPU_CONST_RESET_N_VALUE_MASK     0x00000200
#define REG_LOWER_CPU_CONST_RESET_N_VALUE_SHIFT    9
#define REG_LOWER_CPU_CONST_RESET_N_VALUE_WIDTH    1

/* LOWER_CPU_REQUIRED_BOOT_MODE */
/* Secure write */
#define REG_LOWER_CPU_REQUIRED_BOOT_MODE          0x0058
#define REG_LOWER_CPU_REQUIRED_BOOT_MODE_MASK     0x00000c00
#define REG_LOWER_CPU_REQUIRED_BOOT_MODE_SHIFT    10
#define REG_LOWER_CPU_REQUIRED_BOOT_MODE_WIDTH    2

/* LOWER_CPU_OVERRIDE_HOST_MODE */
/* Secure write */
#define REG_LOWER_CPU_OVERRIDE_HOST_MODE          0x0058
#define REG_LOWER_CPU_OVERRIDE_HOST_MODE_MASK     0x00001000
#define REG_LOWER_CPU_OVERRIDE_HOST_MODE_SHIFT    12
#define REG_LOWER_CPU_OVERRIDE_HOST_MODE_WIDTH    1

/* HOSTLESS_PB_MODE */
/* Secure write */
#define REG_HOSTLESS_PB_MODE          0x0058
#define REG_HOSTLESS_PB_MODE_MASK     0x00010000
#define REG_HOSTLESS_PB_MODE_SHIFT    16
#define REG_HOSTLESS_PB_MODE_WIDTH    1

/* UEEPROM_OVERRIDE */
#define REG_UEEPROM_OVERRIDE          0x005c
#define REG_UEEPROM_OVERRIDE_MASK     0x00000001
#define REG_UEEPROM_OVERRIDE_SHIFT    0
#define REG_UEEPROM_OVERRIDE_WIDTH    1

/* UEE_SCL_IN */
#define REG_UEE_SCL_IN          0x005c
#define REG_UEE_SCL_IN_MASK     0x00000010
#define REG_UEE_SCL_IN_SHIFT    4
#define REG_UEE_SCL_IN_WIDTH    1

/* UEE_SCL_OUT */
#define REG_UEE_SCL_OUT          0x005c
#define REG_UEE_SCL_OUT_MASK     0x00000020
#define REG_UEE_SCL_OUT_SHIFT    5
#define REG_UEE_SCL_OUT_WIDTH    1

/* UEE_SCL_OE */
/* Active high */
#define REG_UEE_SCL_OE          0x005c
#define REG_UEE_SCL_OE_MASK     0x00000040
#define REG_UEE_SCL_OE_SHIFT    6
#define REG_UEE_SCL_OE_WIDTH    1

/* UEE_SDA_IN */
#define REG_UEE_SDA_IN          0x005c
#define REG_UEE_SDA_IN_MASK     0x00000100
#define REG_UEE_SDA_IN_SHIFT    8
#define REG_UEE_SDA_IN_WIDTH    1

/* UEE_SDA_OUT */
#define REG_UEE_SDA_OUT          0x005c
#define REG_UEE_SDA_OUT_MASK     0x00000200
#define REG_UEE_SDA_OUT_SHIFT    9
#define REG_UEE_SDA_OUT_WIDTH    1

/* UEE_SDA_OE */
/* Active high */
#define REG_UEE_SDA_OE          0x005c
#define REG_UEE_SDA_OE_MASK     0x00000400
#define REG_UEE_SDA_OE_SHIFT    10
#define REG_UEE_SDA_OE_WIDTH    1

/* HL_MOSI */
#define REG_HL_MOSI          0x005c
#define REG_HL_MOSI_MASK     0x00001000
#define REG_HL_MOSI_SHIFT    12
#define REG_HL_MOSI_WIDTH    1

/* HL_SCS */
/* Active low at the device */
#define REG_HL_SCS          0x005c
#define REG_HL_SCS_MASK     0x00002000
#define REG_HL_SCS_SHIFT    13
#define REG_HL_SCS_WIDTH    1

/* SHRAM_BASE_ADDR */
#define REG_SHRAM_BASE_ADDR          0x006c
#define REG_SHRAM_BASE_ADDR_MASK     0x00ffffff
#define REG_SHRAM_BASE_ADDR_SHIFT    0
#define REG_SHRAM_BASE_ADDR_WIDTH    24

/* SHRAM_INT_MASK */
#define REG_SHRAM_INT_MASK          0x0070
#define REG_SHRAM_INT_MASK_MASK     0x0000ffff
#define REG_SHRAM_INT_MASK_SHIFT    0
#define REG_SHRAM_INT_MASK_WIDTH    16

/* SHRAM_STATUS_CLEAR */
#define REG_SHRAM_STATUS_CLEAR          0x0074
#define REG_SHRAM_STATUS_CLEAR_MASK     0x0000ffff
#define REG_SHRAM_STATUS_CLEAR_SHIFT    0
#define REG_SHRAM_STATUS_CLEAR_WIDTH    16

/* SHRAM_STATUS_REG */
#define REG_SHRAM_STATUS_REG          0x0078
#define REG_SHRAM_STATUS_REG_MASK     0x0000ffff
#define REG_SHRAM_STATUS_REG_SHIFT    0
#define REG_SHRAM_STATUS_REG_WIDTH    16

/* SHRAM_CLIENT_AT_IRQ */
#define REG_SHRAM_CLIENT_AT_IRQ          0x007c
#define REG_SHRAM_CLIENT_AT_IRQ_MASK     0x0000000f
#define REG_SHRAM_CLIENT_AT_IRQ_SHIFT    0
#define REG_SHRAM_CLIENT_AT_IRQ_WIDTH    4

/* DEL_FLOW_CONTROL_EN */
#define REG_DEL_FLOW_CONTROL_EN          0x00b4
#define REG_DEL_FLOW_CONTROL_EN_MASK     0x00000003
#define REG_DEL_FLOW_CONTROL_EN_SHIFT    0
#define REG_DEL_FLOW_CONTROL_EN_WIDTH    2

/* WEP_FLOW_CONTROL_EN */
#define REG_WEP_FLOW_CONTROL_EN          0x00b4
#define REG_WEP_FLOW_CONTROL_EN_MASK     0x0000000c
#define REG_WEP_FLOW_CONTROL_EN_SHIFT    2
#define REG_WEP_FLOW_CONTROL_EN_WIDTH    2

/* FLOW_CONTROL_RD_ROOF_ADDR_EN */
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_EN          0x00b4
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_EN_MASK     0x00000010
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_EN_SHIFT    4
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_EN_WIDTH    1

/* WEP_FLOW_CONTROL_WR_ADDR */
#define REG_WEP_FLOW_CONTROL_WR_ADDR          0x00b8
#define REG_WEP_FLOW_CONTROL_WR_ADDR_MASK     0x0007ffff
#define REG_WEP_FLOW_CONTROL_WR_ADDR_SHIFT    0
#define REG_WEP_FLOW_CONTROL_WR_ADDR_WIDTH    19

/* WEP_FLOW_CONTROL_RD_ADDR */
#define REG_WEP_FLOW_CONTROL_RD_ADDR          0x00bc
#define REG_WEP_FLOW_CONTROL_RD_ADDR_MASK     0x00ffffff
#define REG_WEP_FLOW_CONTROL_RD_ADDR_SHIFT    0
#define REG_WEP_FLOW_CONTROL_RD_ADDR_WIDTH    24

/* WEP_FLOW_CONTROL_REQ_PENDING */
#define REG_WEP_FLOW_CONTROL_REQ_PENDING          0x00bc
#define REG_WEP_FLOW_CONTROL_REQ_PENDING_MASK     0x80000000
#define REG_WEP_FLOW_CONTROL_REQ_PENDING_SHIFT    31
#define REG_WEP_FLOW_CONTROL_REQ_PENDING_WIDTH    1

/* DEL_FLOW_CONTROL_WR_ADDR */
#define REG_DEL_FLOW_CONTROL_WR_ADDR          0x00c0
#define REG_DEL_FLOW_CONTROL_WR_ADDR_MASK     0x0007ffff
#define REG_DEL_FLOW_CONTROL_WR_ADDR_SHIFT    0
#define REG_DEL_FLOW_CONTROL_WR_ADDR_WIDTH    19

/* DEL_FLOW_CONTROL_RD_ADDR */
#define REG_DEL_FLOW_CONTROL_RD_ADDR          0x00c4
#define REG_DEL_FLOW_CONTROL_RD_ADDR_MASK     0x00ffffff
#define REG_DEL_FLOW_CONTROL_RD_ADDR_SHIFT    0
#define REG_DEL_FLOW_CONTROL_RD_ADDR_WIDTH    24

/* DEL_FLOW_CONTROL_REQ_PENDING */
#define REG_DEL_FLOW_CONTROL_REQ_PENDING          0x00c4
#define REG_DEL_FLOW_CONTROL_REQ_PENDING_MASK     0x80000000
#define REG_DEL_FLOW_CONTROL_REQ_PENDING_SHIFT    31
#define REG_DEL_FLOW_CONTROL_REQ_PENDING_WIDTH    1

/* DMA_MON_NUM_WR_BYTES */
#define REG_DMA_MON_NUM_WR_BYTES          0x00c8
#define REG_DMA_MON_NUM_WR_BYTES_MASK     0x000000ff
#define REG_DMA_MON_NUM_WR_BYTES_SHIFT    0
#define REG_DMA_MON_NUM_WR_BYTES_WIDTH    8

/* DMA_MON_NUM_WR_BYTES_COUNTER */
#define REG_DMA_MON_NUM_WR_BYTES_COUNTER          0x00c8
#define REG_DMA_MON_NUM_WR_BYTES_COUNTER_MASK     0x01ff0000
#define REG_DMA_MON_NUM_WR_BYTES_COUNTER_SHIFT    16
#define REG_DMA_MON_NUM_WR_BYTES_COUNTER_WIDTH    9

/* FLOW_CONTROL_RD_ROOF_ADDR */
#define REG_FLOW_CONTROL_RD_ROOF_ADDR          0x00dc
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_MASK     0x0007ffff
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_SHIFT    0
#define REG_FLOW_CONTROL_RD_ROOF_ADDR_WIDTH    19
#endif 
