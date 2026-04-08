/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "mtlkinc.h"

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

#include "mtlk_ccr_pcieg6_a0_b0_regs.h"

/* Include C source file to generate card specific CCR APIs
 */
#undef _PCIEG6_GENERAL_API_   /* Don't include general APIs */
#include "mtlk_ccr_pcieg6_common_src.h"

const mtlk_ccr_api_t *__MTLK_IFUNC
mtlk_ccr_pcieg6b0_api_get (void)
{
  return &mtlk_ccr_api;
}
