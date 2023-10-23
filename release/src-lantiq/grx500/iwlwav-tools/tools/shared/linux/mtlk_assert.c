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
#include <assert.h>

void __MTLK_IFUNC
__mtlk_assert (mtlk_slid_t slid)
{
  printf("Assertion failed " MTLK_SLID_FMT "\n",
         MTLK_SLID_ARGS(slid));
  assert(0);
}

void __MTLK_IFUNC
__mtlk_assert_f(mtlk_slid_t slid, const char *file)
{
  printf("Assertion failed in file %s " MTLK_SLID_FMT "\n",
         file, MTLK_SLID_ARGS(slid));
  assert(0);
}
