/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
