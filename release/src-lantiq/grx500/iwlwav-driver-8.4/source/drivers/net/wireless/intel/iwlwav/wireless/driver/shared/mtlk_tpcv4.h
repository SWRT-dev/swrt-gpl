/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* $Id$ */

#ifndef _MTLK_TPCV4_H_
#define _MTLK_TPCV4_H_

/** 
*\file mtlk_tpcv4.h

*\brief TPC V4 eeprom structure accessors

*\defgroup EEPROM EEPROM structures access
*\{
*/

#include "mtlkbfield.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef struct
{
  uint8 *data;
  uint8 num_points;
} __MTLK_IDATA mtlk_tpcv4_t;

int __MTLK_IFUNC
mtlk_tpcv4_init(mtlk_tpcv4_t   *tpc,
                void           *data,
                uint8           size);

int __MTLK_IFUNC
mtlk_tpcv4_init_empty(mtlk_tpcv4_t   *tpc,
                      int             num_points);

int __MTLK_IFUNC
mtlk_tpcv4_get_data_size(const mtlk_tpcv4_t *tpc);

void* __MTLK_IFUNC
mtlk_tpcv4_get_data(const mtlk_tpcv4_t *tpc);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_size_by_num_points(int num_points);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_num_points(const mtlk_tpcv4_t *tpc);

void __MTLK_IFUNC
mtlk_tpcv4_cleanup(mtlk_tpcv4_t *tpc);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_channel(const mtlk_tpcv4_t *tpc);

void __MTLK_IFUNC
mtlk_tpcv4_set_channel(mtlk_tpcv4_t *tpc, uint8 channel);

typedef enum 
{
  MTLK_TPC_CB  = 0,
  MTLK_TPC_NCB = 1
} MTLK_TPC_CB_FLAG;

MTLK_TPC_CB_FLAG __MTLK_IFUNC
mtlk_tpcv4_get_cb_flag(const mtlk_tpcv4_t *tpc);

void __MTLK_IFUNC
mtlk_tpcv4_set_cb_flag(mtlk_tpcv4_t *tpc, MTLK_TPC_CB_FLAG cb_flag);

typedef enum 
{
  MTLK_TPC_BAND_5GHZ = 0,
  MTLK_TPC_BAND_2GHZ = 1
} MTLK_TPC_BAND;

MTLK_TPC_BAND __MTLK_IFUNC
mtlk_tpcv4_get_band_by_raw_buffer(uint8* buffer);

MTLK_TPC_BAND __MTLK_IFUNC
mtlk_tpcv4_get_band(const mtlk_tpcv4_t *tpc);

void __MTLK_IFUNC
mtlk_tpcv4_set_band(mtlk_tpcv4_t *tpc, MTLK_TPC_BAND band);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_tpc_val(const mtlk_tpcv4_t *tpc, uint8 antenna);

void __MTLK_IFUNC
mtlk_tpcv4_set_tpc_val(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 tpc_val);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_mul(const mtlk_tpcv4_t *tpc, uint8 antenna);

void __MTLK_IFUNC
mtlk_tpcv4_set_backoff_mul(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 mul_val);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_width(const mtlk_tpcv4_t *tpc);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_packed(const mtlk_tpcv4_t *tpc, uint8 antenna);

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff(const mtlk_tpcv4_t *tpc, uint8 antenna, uint8 point);

void __MTLK_IFUNC
mtlk_tpcv4_set_backoff(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 point, uint8 backoff);

typedef struct _mtlk_tpc4_point_t
{
  unsigned int x;
  unsigned int y;
} mtlk_tpc_point_t;

void __MTLK_IFUNC
mtlk_tpcv4_get_point(const mtlk_tpcv4_t *tpc, uint8 antenna, 
                     uint8 point_num, mtlk_tpc_point_t *point);

void __MTLK_IFUNC
mtlk_tpcv4_set_point(mtlk_tpcv4_t *tpc, uint8 antenna, 
                     uint8 point_num, const mtlk_tpc_point_t *point);

/*\}*/

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* !_MTLK_TPCV4_H_ */
