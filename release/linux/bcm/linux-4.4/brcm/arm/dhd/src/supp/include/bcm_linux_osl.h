/*
 * bdm_linux_osl.h -- linux osl for external supplicant
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcm_linux_osl.h,v 1.6 2009-12-04 22:33:24 $
 */

#ifndef _linux_osl_h_
#define _linux_osl_h_

#include <stdlib.h>
#include <assert.h>

/* Minimalist OSL layer */
#define MALLOC(osh, size)		malloc((size))
#define MFREE(osh, ptr, len)		free((ptr))

#define MALLOCED(osh)	(0)

#define ASSERT assert
#define OS_MALLOC(len) malloc(len)
#define OS_FREE(p) free(p)



#endif /* _linux_osl_h_ */
