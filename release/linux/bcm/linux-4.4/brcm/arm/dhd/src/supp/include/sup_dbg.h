/*
 * sup_dbg.h -- debug support for user space
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: sup_dbg.h,v 1.6 2010-02-26 19:54:45 $
 */

#ifndef _BCM_DBG_H
#define _BCM_DBG_H

#if defined(OSL_EXT)
	#include <osl.h>
#endif   /* (OSL_EXT) */


/* support wlc_sup.c debug msgs */
#ifdef BCMDBG
#define	WL_ERROR(args)		printf args
#define WL_WSEC(args)		printf args
#define DEBUG_PRINTF(args)	printf args
#define WL_WSEC_ON()	1
#else
#define WL_ERROR(args)
#define WL_WSEC(args)
#define DEBUG_PRINTF(args)
#define WL_WSEC_ON()	0
#endif


#define	WL_TRACE(args)





#endif /* _BCM_DBG_H */
