/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _MTLK_CCR_PCIEG5_H_
#define _MTLK_CCR_PCIEG5_H_


typedef struct
{
  void                  *dev_ctx; /* this must be first */
  struct pcieg5_pas_map *pas;
  void                  *pas_physical;
  mtlk_hw_t             *hw;
  uint8                 current_ucpu_state;
  uint8                 current_lcpu_state;
  uint8                 next_boot_mode;

  MTLK_DECLARE_INIT_STATUS;
} _mtlk_pcieg5_ccr_t;

#endif /*_MTLK_CCR_PCIEG5_H_*/
