/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

#define HOST_MAGIC   0xBEADFEED

enum {
  CHI_CPU_NUM_UM = 0,
  CHI_CPU_NUM_LM,
  CHI_CPU_NUM_TX,
  CHI_CPU_NUM_TX_1,
  CHI_CPU_NUM_IF,
  CHI_CPU_NUM_RX,
  CHI_CPU_NUM_RX_1,
  CHI_CPU_NUM_CNT
};

#define CHI_ADDR                        (0)

#define PROGMODEL_CHUNK_SIZE        8000
/**************************************************************/

#endif /* #ifndef __SHRAM_EX_H__ */
