/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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


