/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "hw_mmb.h"
#include "mtlk_ccr_pcie.h"
#include "mtlk_ccr_api.h"
#include "mtlk_ccr_common.h"

#include "HwHeaders/WAVE700/HwMemoryMap.h"
#include "HwRegs/WAVE700/HostIfAccRegs.h"
#include "HwRegs/WAVE700/HostIfRegs.h"
#include "HwRegs/WAVE700/EmeraldEnvRegs.h"
#include "HwRegs/WAVE700/MacGeneralRegs.h"
#include "HwRegs/WAVE700/MacHtExtensionsRegs.h"
#include "HwRegs/WAVE700/PacRxcRegs.h"
#include "HwRegs/WAVE700/PacTimRegs.h"
#include "HwRegs/WAVE700/PhyRxTdRegs.h"
#include "HwRegs/WAVE700/SocRegsRegs.h"
#include "HwRegs/WAVE700/HostIfRegs.h"
#include "HwRegs/WAVE700/PhyAgcAnt0Regs.h"

#include "mtlk_ccr_pcie_common_regs.h"
#include "mtlk_ccr_pcieg6d2_g7_regs.h"

#define MTLK_FIREWALL_ON_UPDATE_SHARED_RAM_SIZE(size,reg_val) \
   __ccr_mem_readl (ccr_mem, HOST_IF_WLAN_SECURE_STATUS, reg_val);  \
   if (MTLK_BFIELD_GET(reg_val,  HOST_IF_WLAN_SECURE_STATUS_FIREWALL_ON ) == 1) { \
     size  -= SHARED_RAM_BLACK_SIZE; \
   } \

/* Include C source file to generate card specific CCR APIs
 */
#undef _WAVE_PCIE_GENERAL_API_   /* Don't include general APIs */
#include "mtlk_ccr_pcie_common_src.h"
#include "mtlk_ccr_pcieg6d2_g7_common_src.h"

#define LOG_LOCAL_GID   GID_CCR_PCIE
#define LOG_LOCAL_FID   7

static const mtlk_ccr_api_t mtlk_ccr_api = {
  _mtlk_pcie_ccr_init,
  _mtlk_pcie_ccr_cleanup,
  _mtlk_pcie_ccr_read_chip_id,
  _mtlk_pcie_ccr_get_sec_boot_fw_info,
  _mtlk_pcie_ccr_get_fw_dump_info,
  _mtlk_pcie_ccr_get_hw_dump_info,
  _mtlk_pcie_enable_interrupts,
  _mtlk_pcie_disable_interrupts,
  _mtlk_pcie_clear_interrupts_if_pending,
  _mtlk_pcie_disable_interrupts_if_pending,
  _mtlk_pcie_initiate_doorbell_interrupt,
  _mtlk_pcie_is_interrupt_pending,
  _mtlk_pcie_recover_interrupts,
  _mtlk_pcie_release_ctl_from_reset,
  _mtlk_pcie_put_ctl_to_reset,
  _mtlk_pcie_exit_debug_mode,
  _mtlk_pcie_put_cpus_to_reset,
  _mtlk_pcie_reset_mac,
  _mtlk_pcie_power_on_cpus,
  _mtlk_pcie_get_afe_value,
  _mtlk_pcie_get_progmodel_version,
  _mtlk_pcie_bist_efuse,
  _mtlk_pcie_init_pll,
  _mtlk_pcie_secure_release_cpus_reset, /* HW specific */
  _mtlk_pcie_check_bist,
  _mtlk_pcie_eeprom_init,
  _mtlk_pcie_eeprom_override_write,
  _mtlk_pcie_eeprom_override_read,
  _mtlk_pcie_ccr_get_mips_freq,
  _mtlk_pcie_set_msi_intr_mode,
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  _mtlk_pcie_read_hw_timestamp,
#endif
  _mtlk_pcie_ccr_ctrl_ring_init,
  _mtlk_pcie_ccr_ring_get_hds_to_process,
  _mtlk_pcie_ccr_ring_clear_interrupt,
  _mtlk_pcie_ccr_ring_initiate_doorbell_interrupt,
  _mtlk_pcie_ccr_print_irq_regs,
  _mtlk_pcie_efuse_read_data,
  _mtlk_pcie_efuse_write_data,
  _mtlk_pcie_efuse_get_size,
  _mtlk_pcie_ccr_read_uram_version,
  _mtlk_pcie_read_phy_inband_power,
  _wave_pcie_read_hw_type,
  _wave_pcie_read_hw_rev,
};

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg7_api_get (void)
{
  return &mtlk_ccr_api;
}
