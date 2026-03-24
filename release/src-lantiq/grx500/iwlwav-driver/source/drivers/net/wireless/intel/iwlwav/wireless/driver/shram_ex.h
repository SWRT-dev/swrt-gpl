/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __SHRAM_EX_H__
#define __SHRAM_EX_H__

/**************************************************************
 * Shared memory info
 **************************************************************/
#define SHARED_RAM_BCL_ON_EXCEPTION_BASE        (0x1FEF0)
 
#define SHARED_RAM_BCL_ON_EXCEPTION_CTL         (SHARED_RAM_BCL_ON_EXCEPTION_BASE + 0)
#define SHARED_RAM_BCL_ON_EXCEPTION_UNIT        (SHARED_RAM_BCL_ON_EXCEPTION_BASE + 4)
#define SHARED_RAM_BCL_ON_EXCEPTION_ADDR        (SHARED_RAM_BCL_ON_EXCEPTION_BASE + 8)
#define SHARED_RAM_BCL_ON_EXCEPTION_SIZE        (SHARED_RAM_BCL_ON_EXCEPTION_BASE + 12)
#define SHARED_RAM_BCL_ON_EXCEPTION_DATA        (SHARED_RAM_BCL_ON_EXCEPTION_BASE + 16)

#define BCL_IDLE  0
#define BCL_WRITE 1
#define BCL_READ  2

#define HWI_OFFSET_global                 8
#define HWI_MASK_global                   1
#define HWI_OFFSET_local_hrt_to_host      1
#define HWI_MASK_local_hrt_to_host        1

#define HWI_OFFSET_write_data_phase_limit 10
#define HWI_MASK_write_data_phase_limit   3
#define HWI_VALUE_write_data_phase_limit  2

#define HWI_OFFSET_read_data_phase_limit  8
#define HWI_MASK_read_data_phase_limit    3
#define HWI_VALUE_read_data_phase_limit   2

#define HWI_OFFSET_cpu_power_on_reset   (0)
#define HWI_OFFSET_upper_cpu_reset      (1)
#define HWI_OFFSET_lower_cpu_reset      (2)

#define HWI_MASK_cpu_power_on_reset     (1)
#define HWI_MASK_upper_cpu_reset        (1)
#define HWI_MASK_lower_cpu_reset        (1)

#define HWI_MASK_general_purpose_control_enable_ox      0x00000004
#define HWI_MASK_general_purpose_control_ctl_release    0x00000008
#define HWI_MASK_cpu_control_pci_cpu_mode               0x00000030
#define HWI_MASK_general_purpose_control_pci_clk        0x00000002

#define HOST_MAGIC              0xBEADFEED
#define CHI_ADDR                0
#define PROGMODEL_CHUNK_SIZE    8000

/* Enumeration of binary Firmware files for all HW platforms.
 * Secure Boot specific is marked with "_SECB" suffix.
 * FW binaries must be loaded one by one in the exact order.
 * Don't reorder.
 */
typedef enum {
  WAVE_FW_NUM_UM = 0,         /* Upper MAC (unified RAM) */
  /* WAVE_FW_NUM_LM, */       /* Lower MAC (obsolete) */
  WAVE_FW_NUM_SECB_FC,        /* FW certificate */
  WAVE_FW_NUM_SECB_DC,        /* DUT certificate */
  WAVE_FW_NUM_SECB_FW_HDR,    /* Signed FW header */
  WAVE_FW_NUM_SECB_PSD_HDR,   /* Signed PSD header */
  WAVE_FW_NUM_SECB_CAL_HDR,   /* Signed Cal file header */
  WAVE_FW_NUM_TX,             /* Tx Sender CPU 0 / CPUs */
  WAVE_FW_NUM_TX_1,           /* Tx Sender CPU 1 */
  WAVE_FW_NUM_RX,             /* Rx Handler CPU 0 / CPUs */
  WAVE_FW_NUM_RX_1,           /* Rx Handler CPU 1 */
  WAVE_FW_NUM_IF,             /* Host Interface CPUs */
  WAVE_FW_NUM_SECB_IF_1,
  WAVE_FW_NUM_SECB_IF_2,
  WAVE_FW_NUM_SECB_IF_3,
  WAVE_FW_NUM_SECB_IF_4,
  WAVE_FW_NUM_SECB_PM_CMN,    /* Progmodel Common */
  WAVE_FW_NUM_SECB_PM_ANT_RX, /* Progmodel Rx Ant */
  WAVE_FW_NUM_SECB_PM_ANT_TX, /* Progmodel Tx Ant */
  WAVE_FW_NUM_SECB_PSD_FILE,  /* PSD file */
  WAVE_FW_NUM_SECB_CAL_FILE,  /* Cal file */

  WAVE_FW_NUM_CNT             /* Total number */
} wave_fw_num_e;

#endif /* #ifndef __SHRAM_EX_H__ */
