/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mtlk_coreui.h"

#include "mgmt.h"

/**
  The main entry point for management frame transmission

  \param core           Handle to the virtual interface core descriptor [I]
  \param nbuf           Handle to the network buffer descriptor [I]
  \param clb            Handle to the callback function for management
                        frame transmission status handling [I]
                        NULL - if callback is not needed.
  \param clb_usr_data   Handle to the user data for callback function [I]
                        NULL - if data is not needed.

  \return
    MTLK_ERR_OK on success or error value from \ref mtlk_error_t
*/
int __MTLK_IFUNC
mtlk_mgmt_transmit (
  mtlk_core_t *core,
  mtlk_nbuf_t *nbuf,
  mtlk_mgmt_txclb_f clb,
  mtlk_handle_t clb_usr_data)
{
  int res = MTLK_ERR_OK;

  /* FIXCFG80211: do transmission */

  return res;
}
