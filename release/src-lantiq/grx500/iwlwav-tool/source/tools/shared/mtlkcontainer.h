/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_CONTAINER_H__
#define __MTLK_CONTAINER_H__

#include "mtlk_osal.h"
#include "mtlklist.h"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef struct
{
  mtlk_handle_t (__MTLK_IFUNC *start)(void);
  uint32        (__MTLK_IFUNC *iterate)(mtlk_handle_t ctx);
  void          (__MTLK_IFUNC *stop)(mtlk_handle_t ctx);
} __MTLK_IDATA mtlk_component_api_t;

#define MTLK_COMPONENT_NAME_LEN 8

typedef struct
{
  const mtlk_component_api_t *api;
  char                        name[MTLK_COMPONENT_NAME_LEN + 1];
} __MTLK_IDATA mtlk_component_t;

typedef struct
{
  mtlk_dlist_t      components;
  uint32            state;
} __MTLK_IDATA mtlk_container_t;

int  __MTLK_IFUNC mtlk_container_init(mtlk_container_t *cont);
int  __MTLK_IFUNC mtlk_container_register(mtlk_container_t       *cont, 
                                          const mtlk_component_t *comp);
int  __MTLK_IFUNC mtlk_container_run(mtlk_container_t  *cont,
                                     mtlk_osal_event_t *close_evt);
void __MTLK_IFUNC mtlk_container_cleanup(mtlk_container_t *cont);


#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_CONTAINER_H__ */
