/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_SIGNAL_HANDLER_H__
#define __MTLK_SIGNAL_HANDLER_H__

typedef void (__MTLK_IFUNC *mtlk_shandler_clb_f)(mtlk_handle_t usr_ctx, int signo);

int __MTLK_IFUNC
mtlk_shandler_init(mtlk_shandler_clb_f clb, mtlk_handle_t usr_ctx);
void __MTLK_IFUNC
mtlk_shandler_cleanup(void);

#endif
