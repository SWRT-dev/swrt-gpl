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
 *  wlan driver helper for file-system access
 */
#ifndef __FSHLPR_H__
#define __FSHLPR_H__

#include "mtlkcontainer.h"

extern const mtlk_component_api_t irb_fs_hlpr_api;

int __MTLK_IFUNC
fs_hlpr_set_saver (char *cmd);

#endif /* __FSHLPR_H__ */


