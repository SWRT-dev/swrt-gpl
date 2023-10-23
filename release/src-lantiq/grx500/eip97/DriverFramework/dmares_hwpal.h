/* dmares_hwpal.h
 *
 * HW and OS abstraction API
 * for the Driver Framework DMAResource API implementation
 *
 */

/*****************************************************************************
* Copyright (c) 2012-2013 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef DMARES_HWPAL_H_
#define DMARES_HWPAL_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Driver Framework DMAResource Types API
#include "dmares_types.h"

// Driver Framework C Library abstraction APi
#include "clib.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_MaxAlignment_Get
 */
unsigned int
HWPAL_DMAResource_MaxAlignment_Get(void);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_DCache_Alignment_Get
 */
unsigned int
HWPAL_DMAResource_DCache_Alignment_Get(void);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_MemAlloc
 */
void *
HWPAL_DMAResource_MemAlloc(
        size_t ByteCount);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_MemFree
 */
void
HWPAL_DMAResource_MemFree(
        void * Buf_p);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Lock_Alloc
 */
void *
HWPAL_DMAResource_Lock_Alloc(void);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Lock_Free
 */
void
HWPAL_DMAResource_Lock_Free(void * Lock_p);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Lock_Acquire
 */
void
HWPAL_DMAResource_Lock_Acquire(
        void * Lock_p,
        unsigned long * Flags);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Lock_Release
 */
void
HWPAL_DMAResource_Lock_Release(
        void * Lock_p,
        unsigned long * Flags);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Alloc
 */
int
HWPAL_DMAResource_Alloc(
        const DMAResource_Properties_t RequestedProperties,
        DMAResource_AddrPair_t * const AddrPair_p,
        DMAResource_Handle_t * const Handle_p);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Release
 */
int
HWPAL_DMAResource_Release(
        const DMAResource_Handle_t Handle);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Init
 */
bool
HWPAL_DMAResource_Init(void);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_UnInit
 */
void
HWPAL_DMAResource_UnInit(void);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_CheckAndRegister
 */
int
HWPAL_DMAResource_CheckAndRegister(
        const DMAResource_Properties_t RequestedProperties,
        const DMAResource_AddrPair_t AddrPair,
        const char AllocatorRef,
        DMAResource_Handle_t * const Handle_p);


/*----------------------------------------------------------------------------
 * HWPAL_DMAResource_Record_Update
 */
int
HWPAL_DMAResource_Record_Update(
        const int Identifier,
        DMAResource_Record_t * const Rec_p);


#endif // DMARES_HWPAL_H_


/* end of file dmares_hwpal.h */
