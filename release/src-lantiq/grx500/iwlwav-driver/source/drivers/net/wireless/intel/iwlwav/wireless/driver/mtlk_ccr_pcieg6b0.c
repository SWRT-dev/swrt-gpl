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

#include "HwHeaders/WAVE600B/HwMemoryMap.h"
#include "HwRegs/WAVE600B/HostIfAccRegs.h"
#include "HwRegs/WAVE600B/EmeraldEnvRegs.h"
#include "HwRegs/WAVE600B/MacGeneralRegs.h"
#include "HwRegs/WAVE600B/MacHtExtensionsRegs.h"
#include "HwRegs/WAVE600B/PacRxcRegs.h"
#include "HwRegs/WAVE600B/PacTimRegs.h"
#include "HwRegs/WAVE600B/PhyRxTdRegs.h"
#include "HwRegs/WAVE600B/SocRegsRegs.h"
#include "HwRegs/WAVE600B/PhyAgcAnt0Regs.h"

#include "mtlk_ccr_pcie_common_regs.h"
#include "mtlk_ccr_pcieg6_a0_b0_regs.h"

#define MTLK_FIREWALL_ON_UPDATE_SHARED_RAM_SIZE(size, reg_val)  {reg_val = 0;}

/* Include C source file to generate card specific CCR APIs
 */
#undef _WAVE_PCIE_GENERAL_API_   /* Don't include general APIs */
#include "mtlk_ccr_pcie_common_src.h"
#include "mtlk_ccr_pcieg6_a0_b0_common_src.h"

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg6b0_api_get (void)
{
  return &mtlk_ccr_api;
}
