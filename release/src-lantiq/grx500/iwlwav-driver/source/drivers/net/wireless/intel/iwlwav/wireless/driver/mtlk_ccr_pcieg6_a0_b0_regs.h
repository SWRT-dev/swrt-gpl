/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* List of HW A0/B0 dependent registers */

/*  Block 1: HOST_IF_IRQS registers HOST_IRQ_MASK etc
 *             are defined as MAC_GENERAL and accesible via MAC_GEN_REGFILE
 *
 *  Block 2: HOST_IF_ACC  registers HOST_MAC_MAILBOX_ACCUM_ADD/COUNT/SUM
 *             are defined as HOST_IF_ACC and accesible via HOST_IF_ACC
 */


#define HOST_IF_IRQS_BAR0   MAC_GEN_REGFILE_OFFSET_FROM_BAR0
#define HOST_IF_ACC_BAR0    HOST_IF_ACC_OFFSET_FROM_BAR0

/* Shared memory offset */
#define SHRAM_WRITABLE_OFFSET  SHARED_RAM_OFFSET_FROM_BAR0

#define _PCIE_HW_DEPEND_REGS_LIST_ \
    /* HOST_IF_IRQS <-- MAC General */ \
    _HW_REG_DEF_(HOST_IF_IRQS, MAC_GENERAL, HOST_IRQ_MASK)          \
    _HW_REG_DEF_(HOST_IF_IRQS, MAC_GENERAL, HOST_IRQ_STATUS)        \
    _HW_REG_DEF_(HOST_IF_IRQS, MAC_GENERAL, NPU2UPI_INTERRUPT_SET)  \
    _HW_REG_DEF_(HOST_IF_IRQS, MAC_GENERAL, PHI_INTERRUPT_CLEAR)    \
    /* HOST_IF_ACC */ \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF_ACC, HOST_MAC_MAILBOX_ACCUM_ADD) \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF_ACC, MAC_HOST_MAILBOX_ACCUM_COUNT) \
    _HW_REG_DEF_(HOST_IF_ACC, HOST_IF_ACC, MAC_HOST_MAILBOX_ACCUM_SUB) \

