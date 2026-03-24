/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* List of HW dependent registers */
/* It is related to both WAV600D2 and WAV700 */

/*  Block 1: HOST_IF_IRQS registers HOST_IRQ_MASK etc
 *  Block 2: HOST_IF_ACC  registers HOST_MAC_MAILBOX_ACCUM_ADD/COUNT/SUM
 *  Both blocks are defined as HOST_IF and accesible via HOST_IF_REG_FILE
 */

#define HOST_IF_IRQS_BAR0   HOST_IF_REG_FILE_OFFSET_FROM_BAR0
#define HOST_IF_ACC_BAR0    HOST_IF_REG_FILE_OFFSET_FROM_BAR0

/* Shared memory offset */
#define SHRAM_WRITABLE_OFFSET  SHARED_RAM_WHITE_OFFSET_FROM_BAR0

#define _PCIE_HW_DEPEND_REGS_LIST_ \
    /* HOST_IF_IRQS <-- HOST_IF (moved from MAC_GENERAL) */ \
    _HW_REG_DEF_(HOST_IF_IRQS, HOST_IF, HOST_IRQ_MASK)          \
    _HW_REG_DEF_(HOST_IF_IRQS, HOST_IF, HOST_IRQ_STATUS)        \
    _HW_REG_DEF_(HOST_IF_IRQS, HOST_IF, NPU2UPI_INTERRUPT_SET)  \
    _HW_REG_DEF_(HOST_IF_IRQS, HOST_IF, PHI_INTERRUPT_CLEAR)    \
    /* HOST_IF_ACC <-- HOST_IF (moved from HOST_IF_ACC) */ \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF, HOST_MAC_MAILBOX_ACCUM_ADD) \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF, MAC_HOST_MAILBOX_ACCUM_COUNT) \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF, MAC_HOST_MAILBOX_ACCUM_SUB) \
    /* HOST_IF */ \
    _HW_REG_DEF_(HOST_IF, HOST_IF, WLAN_EFUSE_INDICATION)       \
    _HW_REG_DEF_(HOST_IF, HOST_IF, WLAN_SECURE_STATUS)          \
    _HW_REG_DEF_(HOST_IF, HOST_IF, UCPU_STAT)                   \

/* REG_HOST_IF_WLAN_SECURE_STATUS */
#define HOST_IF_WLAN_SECURE_STATUS_FIREWALL_ON     MTLK_BFIELD_INFO(0, 1)

/* REG_HOST_IF_WLAN_EFUSE_INDICATION */
#define HOST_IF_WLAN_EFUSE_INDICATION_RESERVED0   MTLK_BFIELD_INFO( 0, 8)
#define HOST_IF_WLAN_EFUSE_INDICATION_CHIP_TYPE   MTLK_BFIELD_INFO( 8, 8)
#define HOST_IF_WLAN_EFUSE_INDICATION_HW_TYPE     MTLK_BFIELD_INFO(16, 8)
#define HOST_IF_WLAN_EFUSE_INDICATION_HW_REV      MTLK_BFIELD_INFO(24, 8)

/* REG_HOST_IF_UCPU_STAT */
#define HOST_IF_UCPU_STAT_HOST_START_UCPU         1 /* bit0: hostStartUcpu */
