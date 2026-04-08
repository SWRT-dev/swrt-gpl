/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
* $Id: $
*
* 
*
* Helper functions for GPL code implementation
*
*/

#include "mtlkinc.h"
#include "mhi_umi_propr.h"

uint32 __MTLK_IFUNC mtlk_get_umi_man_req_size(void)
{
  return sizeof(UMI_MAN);
}

uint32 __MTLK_IFUNC mtlk_get_umi_man_ind_size(void)
{
  return sizeof(UMI_IND);
}

uint32 __MTLK_IFUNC mtlk_get_umi_dbg_size(void)
{
  return sizeof(UMI_DBG);
}

uint32 __MTLK_IFUNC mtlk_get_umi_mbss_pre_activate_size(void)
{
  return sizeof(UMI_MBSS_PRE_ACTIVATE);
}
