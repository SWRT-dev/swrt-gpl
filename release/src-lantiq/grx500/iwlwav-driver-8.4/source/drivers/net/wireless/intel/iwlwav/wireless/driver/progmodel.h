/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

