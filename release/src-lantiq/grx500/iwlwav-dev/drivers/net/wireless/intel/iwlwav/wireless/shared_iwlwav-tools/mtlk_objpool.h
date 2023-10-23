/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
* $Id$
*
* 
*
* Written by: Dmitry Fleytman
*
*/

#ifndef _MTLK_OBJPOOL_H_
#define _MTLK_OBJPOOL_H_

/** 
*\file mtlk_objpool.h
*\brief OSAL object tracking module
*/

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

typedef enum _mtlk_objtypes_t
{
  /* ID's are started from magic number on order  */
  /* to catch situation when uninitialized object */
  /* is being passed to ObjPool functions         */

  MTLK_OBJTYPES_START = 0xABCD,

  MTLK_MEMORY_OBJ,
  MTLK_TIMER_OBJ,
  MTLK_SPINLOCK_OBJ,
  MTLK_EVENT_OBJ,
  MTLK_MUTEX_OBJ,
  MTLK_SLIST_OBJ,
  MTLK_LSLIST_OBJ,
  MTLK_DLIST_OBJ,
  MTLK_LDLIST_OBJ,
  MTLK_THREAD_OBJ, /* applications only */
  MTLK_TXMM_OBJ,   /* driver only */
#ifdef CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL
  MTLK_NET_BUF_OBJ, /* driver only */
#endif
  MTLK_WSS_OBJ,

  MTLK_OBJTYPES_END
} mtlk_objtypes_t;

struct _mtlk_objpool_t;

typedef struct _mtlk_objpool_t mtlk_objpool_t;
typedef mtlk_slid_t mtlk_objpool_context_t;

#define MTLK_DECLARE_OBJPOOL_CTX(name) \
  mtlk_objpool_context_t name;

#define MTLK_DECLARE_OBJPOOL(name) \
  extern mtlk_objpool_t name;

int __MTLK_IFUNC mtlk_objpool_init(mtlk_objpool_t* objpool);
void __MTLK_IFUNC mtlk_objpool_cleanup(mtlk_objpool_t* objpool);
void __MTLK_IFUNC mtlk_objpool_dump(mtlk_objpool_t* objpool);

const char * __MTLK_IFUNC mtlk_objpool_get_type_name(mtlk_objtypes_t object_type);

/* mtlk_objpool_enum_f return FALSE to stop enumeration, TRUE - to continue it */
typedef BOOL (__MTLK_IFUNC *mtlk_objpool_enum_f)(mtlk_objpool_t* objpool,
                                                 mtlk_slid_t creator_slid,
                                                 uint32      objects_number,
                                                 uint32      additional_allocations_size,
                                                 mtlk_handle_t context);

void __MTLK_IFUNC mtlk_objpool_enum_by_type(mtlk_objpool_t* objpool,
                                            mtlk_objtypes_t object_type,
                                            mtlk_objpool_enum_f clb,
                                            mtlk_handle_t context);

void __MTLK_IFUNC
mtlk_objpool_add_object_ex(mtlk_objpool_t* objpool, 
                           mtlk_objpool_context_t* objpool_ctx_ptr,
                           mtlk_objtypes_t object_type,
                           mtlk_slid_t creator_slid,
                           uint32 additional_allocation_size);

void __MTLK_IFUNC mtlk_objpool_remove_object_ex(mtlk_objpool_t* objpool,
                                                mtlk_objpool_context_t* objpool_ctx_ptr,
                                                mtlk_objtypes_t object_type,
                                                uint32 additional_allocation_size);

mtlk_slid_t __MTLK_IFUNC
mtlk_objpool_get_creator_slid (mtlk_objpool_t* objpool,
                               mtlk_objpool_context_t* objpool_ctx_ptr);

uint32 __MTLK_IFUNC
mtlk_objpool_get_memory_allocated(mtlk_objpool_t* objpool);

typedef struct _mtlk_objpool_memory_alarm_t
{
  uint32        limit;
  void          (__MTLK_IFUNC *clb)(mtlk_handle_t usr_ctx,
                                    uint32        allocated);
  mtlk_handle_t usr_ctx;
} mtlk_objpool_memory_alarm_t;

void __MTLK_IFUNC
mtlk_objpool_set_memory_alarm(mtlk_objpool_t* objpool, 
                              const mtlk_objpool_memory_alarm_t *alarm_info);

#else /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#define MTLK_DECLARE_OBJPOOL_CTX(name)
#define MTLK_DECLARE_OBJPOOL(name)

#define mtlk_objpool_init(objpool)    (MTLK_ERR_OK)
#define mtlk_objpool_cleanup(objpool)
#define mtlk_objpool_dump(objpool)
#define mtlk_objpool_enum_by_type(objpool, object_type, clb, context)

#define mtlk_objpool_add_object_ex(objpool, objfact, object_type, creator_slid, object_size)
#define mtlk_objpool_remove_object_ex(objpool, objfact, object_type, object_size)

#define mtlk_objpool_set_memory_alarm(objpool, alarm_info)

#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#define mtlk_objpool_add_object(objpool, objfact, object_type, creator_slid) \
  mtlk_objpool_add_object_ex((objpool), (objfact), (object_type), (creator_slid), HANDLE_T(0))

#define mtlk_objpool_remove_object(objpool, objfact, object_type) \
  mtlk_objpool_remove_object_ex((objpool), (objfact), (object_type), HANDLE_T(0))

#endif /* _MTLK_OBJPOOL_H_ */
