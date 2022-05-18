/* SPDX-License-Identifier: GPL-2.0 */
/*
 * si_voice_datatypes.h
 * ProSLIC datatypes file
 *
 * Author(s): 
 * laj
 *
 * Distributed by: 
 * Silicon Laboratories, Inc
 *
 * This file contains proprietary information.	 
 * No dissemination allowed without prior written permission from
 * Silicon Laboratories, Inc.
 *
 * File Description:
 * This is the header file that contains
 * type definitions for the data types
 * used in the demonstration code.
 *
 */
#ifndef DATATYPES_H
#define DATATYPES_H
#include "proslic_api_config.h"

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#include <linux/types.h>
typedef u_int8_t            BOOLEAN;
typedef int8_t              int8;
typedef u_int8_t            uInt8;
typedef uInt8               uChar;
typedef int16_t             int16;
typedef u_int16_t           uInt16;
typedef int32_t             int32;
typedef u_int32_t           uInt32;
typedef u_int32_t           ramData;


#include <linux/slab.h>
#include <linux/kernel.h> /* for abs() */
/* NOTE: kcalloc was introduced in ~2.6.14, otherwise use kzalloc() with (X)*(Y) for the block size */
#define SIVOICE_CALLOC(X,Y)   kcalloc((X),(Y), GFP_KERNEL)
#define SIVOICE_FREE(X)       kfree((X))
#define SIVOICE_MALLOC(X)     kmalloc((X), GFP_KERNEL)
#define SIVOICE_STRCPY        strcpy
#define SIVOICE_STRNCPY       strncpy
#define SIVOICE_MEMSET        memset
#define SIVOICE_ABS           abs
#endif

