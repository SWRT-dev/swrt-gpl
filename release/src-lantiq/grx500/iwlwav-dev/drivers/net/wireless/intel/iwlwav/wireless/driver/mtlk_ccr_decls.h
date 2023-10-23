/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* $Id$ */
#if !defined (SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DECLS)
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DECLS */
#undef SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DECLS

#include "mtlk_ccr_common.h"
#include "mtlk_ccr_pcie.h"
#include "mtlk_ccr_api.h"

typedef struct _mtlk_ccr_t
{
  union
  {
    void               *dev_ctx; /* this must be first */
    mtlk_pcie_ccr_t     pcie; /* for all PCIE: Gen6/Gen7 */
  } mem;
  mtlk_osal_mutex_t      eeprom_mutex;
  const mtlk_ccr_api_t   *card_api;
  mtlk_chip_info_t const *chip_info;
  mtlk_card_type_t       hw_type;
#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
  mtlk_osal_timer_t      poll_interrupts;
#endif
  MTLK_DECLARE_INIT_STATUS;
} mtlk_ccr_t;
