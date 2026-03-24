
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This is a common module and shared across all other modules(atm tx, atm rx,
 * asm tx, asm rx, polling thread)
 */

#include "common/ima_common.h"

/*
 * Task of this function is to initialize the common module
 */
s32 init_common(void)
{
	s32 ret = RS_SUCCESS;

	return ret;
}

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_common(void)
{
	s32 ret = RS_SUCCESS;

	return ret;
}
