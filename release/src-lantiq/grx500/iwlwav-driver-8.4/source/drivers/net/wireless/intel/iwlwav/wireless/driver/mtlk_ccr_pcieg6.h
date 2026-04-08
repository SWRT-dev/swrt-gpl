/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _MTLK_CCR_PCIEG6_H_
#define _MTLK_CCR_PCIEG6_H_

typedef struct
{
  void                  *dev_ctx; /* this must be first */
  struct pcieg6_pas_map *pas;
  void                  *pas_physical;
  mtlk_hw_t             *hw;
  uint8                 current_ucpu_state;
  uint8                 current_lcpu_state;
  uint8                 next_boot_mode;

  MTLK_DECLARE_INIT_STATUS;
} _mtlk_pcieg6_ccr_t;

/* called as interface API only in case of Gen6 "main" or "aux" PCIe early detection */
uint32 __MTLK_IFUNC wave_pcieg6_chip_id_read(void *bar1);
BOOL __MTLK_IFUNC wave_pcieg6_is_mode_dual_pci(void *bar1);

#endif /*_MTLK_CCR_PCIEG6_H_*/
