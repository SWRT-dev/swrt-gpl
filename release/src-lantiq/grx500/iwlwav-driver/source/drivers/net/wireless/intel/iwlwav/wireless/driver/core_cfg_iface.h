/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _CORE_CFG_IFACE_H_
#define _CORE_CFG_IFACE_H_

uint8 core_cfg_get_cur_bonding(mtlk_core_t *core);
uint8 core_cfg_get_freq_band_cur(mtlk_core_t *core);
uint8 core_cfg_get_freq_band_cfg(mtlk_core_t *core);
BOOL core_cfg_is_connected (mtlk_core_t *core);
BOOL core_cfg_is_halted (mtlk_core_t *core);

#endif /* _CORE_CFG_IFACE_H_ */
