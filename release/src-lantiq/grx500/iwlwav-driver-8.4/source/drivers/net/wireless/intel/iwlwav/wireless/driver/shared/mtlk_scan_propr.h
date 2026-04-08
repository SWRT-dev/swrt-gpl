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
* Proprietary includes for library only!
*
*/

#ifndef _MTLK_SCAN_PROPR_H_
#define _MTLK_SCAN_PROPR_H_

#include "mhi_umi.h"
#include "mhi_umi_propr.h"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

struct _mtlk_scan_vector_t
{
    uint16 count;
    uint16 used;
    FREQUENCY_ELEMENT *params;
} __MTLK_IDATA;

FREQUENCY_ELEMENT * __MTLK_IFUNC mtlk_scan_vector_get_offset(struct _mtlk_scan_vector_t *vector, uint8 offs);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* _MTLK_SCAN_PROPR_H_ */
