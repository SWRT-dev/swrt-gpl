/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "mtlkinc.h"

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

#include "mtlk_ccr_pcieg6_a0_b0_regs.h"

/* Include C source file to generate card specific CCR APIs
 */
#define _PCIEG6_GENERAL_API_  /* Put general APIs only into one of pcieg6*.c */
#include "mtlk_ccr_pcieg6_common_src.h"
#undef  _PCIEG6_GENERAL_API_

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg6a0_api_get (void)
{
  return &mtlk_ccr_api;
}
