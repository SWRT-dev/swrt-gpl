/*
 * bcmsec_types.h -- local typedefs file
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcmsec_types.h,v 1.1 2009-12-04 22:30:45 $
 */

#ifndef _bcmsec_types_h_
#define _bcmsec_types_h_


typedef void clientdata_t;
typedef void clientdata;

struct ctx {
	clientdata *client; /* opaque pointer to app context */
	void *supctx;		/* our local context pointer */
};
typedef struct ctx ctx_t;


#endif /* _bcmsec_types_h_ */
