/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/


#ifndef __MTLK_GPL_HELPER_H__
#define __MTLK_GPL_HELPER_H__

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

uint32 __MTLK_IFUNC mtlk_get_umi_man_req_size(void);
uint32 __MTLK_IFUNC mtlk_get_umi_man_ind_size(void);
uint32 __MTLK_IFUNC mtlk_get_umi_dbg_size(void);
uint32 __MTLK_IFUNC mtlk_get_umi_mbss_pre_activate_size(void);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_GPL_HELPER_H__ */
