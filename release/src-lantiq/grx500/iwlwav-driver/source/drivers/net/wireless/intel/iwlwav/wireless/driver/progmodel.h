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
 * 
 *
 * Progmodel
 *
 */
#ifndef __PROGMODEL_H__
#define __PROGMODEL_H__

#include "txmm.h"

typedef struct _wave_progmodel_data_t {
    mtlk_txmm_t                         *txmm;
    struct _mtlk_core_firmware_file_t   *ff;
} wave_progmodel_data_t;

struct _wave_radio_t;

int __MTLK_IFUNC wave_progmodel_load(mtlk_txmm_t *txmm, struct _wave_radio_t *radio, int freq);

#endif

