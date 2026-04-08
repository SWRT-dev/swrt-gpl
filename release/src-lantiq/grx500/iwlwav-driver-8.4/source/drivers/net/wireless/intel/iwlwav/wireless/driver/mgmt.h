/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _MTLK_MGMT_H_
#define _MTLK_MGMT_H_

#include "mtlk_coreui.h"

int __MTLK_IFUNC
mtlk_mgmt_transmit (
  mtlk_core_t *core,
  mtlk_nbuf_t *nbuf,
  mtlk_mgmt_txclb_f clb,
  mtlk_handle_t clb_usr_data);

#endif /* _MTLK_MGMT_H_ */
