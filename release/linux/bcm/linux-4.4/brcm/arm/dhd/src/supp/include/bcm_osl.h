/*
 * bcm_osl.h -- osl include for external supplicant
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcm_osl.h,v 1.10 2010-11-12 10:54:50 $
 */

#ifndef _bcm_osl_h_
#define _bcm_osl_h_


#if defined(WIN32) || defined(UNDER_CE)
#include <bcm_win_osl.h>
#elif defined(linux)
#include <bcm_linux_osl.h>
#elif defined(OSL_EXT)
#include <bcm_osl_ext_osl.h>
#elif defined(TARGETOS_symbian)
#include <bcm_symbian_osl.h>
#else
#error "Unknown operating system!"
#endif






#endif /* _bcm_osl_h_ */
