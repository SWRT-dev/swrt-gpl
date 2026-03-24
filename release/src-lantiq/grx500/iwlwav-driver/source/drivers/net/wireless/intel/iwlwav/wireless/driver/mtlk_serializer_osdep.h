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
 * Linux dependent serializer part
 *
 */

#ifndef __MTLK_SERIALIZER_OSDEP_H__
#define __MTLK_SERIALIZER_OSDEP_H__

#include "mtlk_osal.h"
#include "mtlkstartup.h"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef void (*mtlk_szr_clb_t)(mtlk_handle_t ctx);

typedef struct _mtlk_serializer_osdep_t
{
  mtlk_osal_event_t       new_event;
  struct task_struct     *thread;
  volatile BOOL           stop;
  mtlk_szr_clb_t          szr_clb;
  mtlk_handle_t           szr_clb_ctx;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
} __MTLK_IDATA mtlk_serializer_osdep_t;

int  __MTLK_IFUNC _mtlk_serializer_osdep_init(mtlk_serializer_osdep_t *osdep);
int  __MTLK_IFUNC _mtlk_serializer_osdep_start(mtlk_serializer_osdep_t* osdep,
                                               const char *name,
                                               mtlk_szr_clb_t szr_clb,
                                               mtlk_handle_t szr_clb_ctx);
void __MTLK_IFUNC _mtlk_serializer_osdep_notify(mtlk_serializer_osdep_t *osdep);
void __MTLK_IFUNC _mtlk_serializer_osdep_stop(mtlk_serializer_osdep_t *osdep);
void __MTLK_IFUNC _mtlk_serializer_osdep_cleanup(mtlk_serializer_osdep_t *osdep);

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_SERIALIZER_OSDEP_H__ */
