/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_CCR_PCIE_H_
#define _MTLK_CCR_PCIE_H_

typedef struct
{
  void                  *dev_ctx; /* this must be first */
  void                  *pas;
  void                  *pas_physical;
  mtlk_hw_t             *hw;

  MTLK_DECLARE_INIT_STATUS;
} mtlk_pcie_ccr_t;

#endif /*_MTLK_CCR_PCIE_H_*/
