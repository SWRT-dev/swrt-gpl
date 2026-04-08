/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 */

#ifndef __mtlk_fast_mem_h__
#define __mtlk_fast_mem_h__

/** 
*\file mtlk_serializer.h 

*\defgroup FAST_MEM Fast memory allocator
*\{

  Fast memory allocator provides memory allocation services from fast memory pool,
  in case it is supported by platform. If there is no fast memory pool on the
  platform, usual memory provided by mtlk_osal_mem_alloc/mtlk_osal_mem_free is used.
  Since fast memory is extremely expensive resource, there is special algorithm
  intended to fine tune fast memory allocation for different driver modules.
  Module client wanting to allocate fast memory is to identify itself by providing
  its ID. System integrator may specify which clients receive memory from fast pool,
  while others receive usual memory from OSAL allocator.

*/

#define MTLK_FM_USER_CORE     (1)   /*!< Caller ID for core        */
#define MTLK_FM_USER_VAPMGR   (1)   /*!< Caller ID for vap manager */
#define MTLK_FM_USER_MMBDRV   (1)   /*!< Caller ID for MMB driver */
#define MTLK_FM_USER_DF       (1)   /*!< Caller ID for DF          */
#define MTLK_FM_USER_RADIO    (1)   /*!< Caller ID for wave radio  */

#ifndef CPTCFG_IWLWAV_ENABLE_OBJPOOL

/*! Fast memory allocation

    \param   caller_id        ID if the caller
    \param   size             Allocation size
    \return  pointer to allocated memory
*/
void* __MTLK_IFUNC
mtlk_fast_mem_alloc(uint32 caller_id, size_t size);
#else
void* __MTLK_IFUNC
_mtlk_fast_mem_alloc_objpool(uint32           caller_id,
                             size_t           size,
                             mtlk_slid_t      caller_slid);
#define mtlk_fast_mem_alloc(caller_id, size) \
  _mtlk_fast_mem_alloc_objpool(caller_id, size, MTLK_SLID)
#endif

/*! Fast memory deallocation

    \param   addr             Pointer to memory buffer to free
*/
void __MTLK_IFUNC
mtlk_fast_mem_free(void* addr);

/*! Print information about fast memory availability to the console.

    \param   caller_id        ID if the caller
    \param   addr             Pointer to memory buffer to free
*/
void __MTLK_IFUNC
mtlk_fast_mem_print_info(void);

/*\}*/

#endif /* __mtlk_fast_mem_h__ */
