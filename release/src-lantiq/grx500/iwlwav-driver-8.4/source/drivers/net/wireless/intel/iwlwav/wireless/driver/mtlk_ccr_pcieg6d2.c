/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "mtlkinc.h"

#include "HwHeaders/WAVE600D2/HwMemoryMap.h"
#include "HwRegs/WAVE600D2/HostIfAccRegs.h"
#include "HwRegs/WAVE600D2/EmeraldEnvRegs.h"
#include "HwRegs/WAVE600D2/MacGeneralRegs.h"
#include "HwRegs/WAVE600D2/MacHtExtensionsRegs.h"
#include "HwRegs/WAVE600D2/PacRxcRegs.h"
#include "HwRegs/WAVE600D2/PacTimRegs.h"
#include "HwRegs/WAVE600D2/PhyRxTdRegs.h"
#include "HwRegs/WAVE600D2/SocRegsRegs.h"
#include "HwRegs/WAVE600D2/HostIfRegs.h"
#include "HwRegs/WAVE600D2/PhyAgcAnt0Regs.h"

#include "mtlk_ccr_pcieg6_d2_regs.h"

/* Include C source file to generate card specific CCR APIs
 */
#undef _PCIEG6_GENERAL_API_   /* Don't include general APIs */
#include "mtlk_ccr_pcieg6_common_src.h"

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg6d2_api_get (void)
{
  return &mtlk_ccr_api;
}
