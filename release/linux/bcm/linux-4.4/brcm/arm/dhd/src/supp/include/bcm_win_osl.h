/*
 * bdm_win_osl.h -- win osl for external supplicant
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcm_win_osl.h,v 1.3 2010-12-11 01:39:18 $
 */

#ifndef _win_osl_h_
#define _win_osl_h_

#include <stdlib.h>
#include <assert.h>
#define ASSERT(args) assert(args)

/* Minimalist OSL layer */
#define MALLOC(osh, size)		malloc((size))
#define MFREE(osh, ptr, len)		free((ptr))

#define MALLOCED(osh)	(0)

#define OS_MALLOC(len)		malloc(len)
#define OS_FREE(p)		free(p)

#define bcopy(src, dst, len)	memcpy((dst), (src), (len))
#define bcmp(b1, b2, len)	memcmp((b1), (b2), (len))
#define bzero(b, len)		memset((b), 0, (len))

#define snprintf _snprintf

#endif /* _win_osl_h_ */
