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

#include "HwHeaders/WAVE600/HwMemoryMap.h"
#include "HwRegs/WAVE600/HostIfAccRegs.h"
#include "HwRegs/WAVE600/EmeraldEnvRegs.h"
#include "HwRegs/WAVE600/MacGeneralRegs.h"
#include "HwRegs/WAVE600/MacHtExtensionsRegs.h"
#include "HwRegs/WAVE600/PacRxcRegs.h"
#include "HwRegs/WAVE600/PacTimRegs.h"
#include "HwRegs/WAVE600/PhyRxTdRegs.h"
#include "HwRegs/WAVE600/SocRegsRegs.h"
#include "HwRegs/WAVE600/PhyAgcAnt0Regs.h"

#include "mtlk_ccr_pcie_common_regs.h"
#include "mtlk_ccr_pcieg6_a0_b0_regs.h"

#define MTLK_FIREWALL_ON_UPDATE_SHARED_RAM_SIZE(size, reg_val)  {reg_val = 0;}

/* Include C source file to generate card specific CCR APIs
 */
#define _WAVE_PCIE_GENERAL_API_  /* Put general APIs only into one of *pcie*.c */
#include "mtlk_ccr_pcie_common_src.h"
#include "mtlk_ccr_pcieg6_a0_b0_common_src.h"
#undef  _WAVE_PCIE_GENERAL_API_

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg6a0_api_get (void)
{
  return &mtlk_ccr_api;
}

uint32 __MTLK_IFUNC wave_pcieg6a0b0_chip_id_read(void *bar1)
{
  return _wave_pcie_bar1_chip_id_read(bar1);
}

BOOL __MTLK_IFUNC
wave_pcieg6a0b0_is_mode_dual_pci(void *bar1)
{
  return _wave_pcie_bar1_is_mode_dual_pci(bar1);
}

