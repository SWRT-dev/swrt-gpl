/******************************************************************************
  Copyright (c) 2020, MaxLinear, Inc.

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

The source code contained or described herein and all documents related to the
source code ("Material") are owned by Intel Corporation or its suppliers or
licensors. Title to the Material remains with Maxlinear Inc. or its suppliers
and licensors. The Material contains trade secrets and proprietary and
confidential information of Maxlinear or its suppliers and licensors. The
Material is protected by worldwide copyright and trade secret laws and treaty
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in any way
without Maxlinear's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery of
the Materials, either expressly, by implication, inducement, estoppel or
otherwise. Any license under such intellectual property rights must be express
and approved by Maxlinear in writing.

******************************************************************************/
#ifndef __WAVE_THERMAL_H_INCLUDED__
#define __WAVE_THERMAL_H_INCLUDED__

#include "mtlk_mmb_drv.h"
#include <linux/pci.h>

mtlk_error_t wave_thermal_init (struct pci_dev *pdev, mtlk_hw_t *hw);
void wave_thermal_exit (mtlk_hw_t *hw);

#endif /* __WAVE_THERMAL_H_INCLUDED__ */
