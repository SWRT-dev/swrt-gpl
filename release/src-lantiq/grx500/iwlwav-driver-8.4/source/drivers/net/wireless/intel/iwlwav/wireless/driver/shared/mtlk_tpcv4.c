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
*/
#include "mtlkinc.h"

#include "mtlk_tpcv4.h"

#define LOG_LOCAL_GID   GID_TPC4
#define LOG_LOCAL_FID   1

#define _MTLK_TPCV4_2PTS_LENGTH (20)
#define _MTLK_TPCV4_3PTS_LENGTH (26)
#define _MTLK_TPCV4_4PTS_LENGTH (32)
#define _MTLK_TPCV4_5PTS_LENGTH (39)

int __MTLK_IFUNC
mtlk_tpcv4_init(mtlk_tpcv4_t   *tpc,
                void           *data,
                uint8           size)
{
  MTLK_ASSERT( NULL != tpc  );
  MTLK_ASSERT( NULL != data );

  switch(size)
  {
  case _MTLK_TPCV4_2PTS_LENGTH: 
    tpc->num_points = 2;
    break;
  case _MTLK_TPCV4_3PTS_LENGTH: 
    tpc->num_points = 3;
    break;
  case _MTLK_TPCV4_4PTS_LENGTH: 
    tpc->num_points = 4;
    break;
  case _MTLK_TPCV4_5PTS_LENGTH: 
    tpc->num_points = 5;
    break;
  default:
    ELOG_D("Invalid data size (%d bytes)", size);
    return MTLK_ERR_PARAMS;
  }

  tpc->data = (char*) mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_TPC4);

  if(NULL == tpc->data)
  {
    ELOG_D("Failed to allocate memory (%d bytes)", size);
    return MTLK_ERR_NO_MEM;
  }

  memmove(tpc->data, data, size);

  ILOG2_DD("Object for %d points (%d bytes of data) created.", 
       tpc->num_points, size);

  return MTLK_ERR_OK;
}

void* __MTLK_IFUNC
mtlk_tpcv4_get_data(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT( NULL != tpc );

  return tpc->data;
}

int __MTLK_IFUNC
mtlk_tpcv4_get_data_size(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT( NULL != tpc );

  return mtlk_tpcv4_get_size_by_num_points(tpc->num_points);
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_size_by_num_points(int num_points)
{
  switch(num_points)
  {
  case 2:
    return _MTLK_TPCV4_2PTS_LENGTH;
  case 3:
    return _MTLK_TPCV4_3PTS_LENGTH;
  case 4:
    return _MTLK_TPCV4_4PTS_LENGTH;
  case 5:
    return _MTLK_TPCV4_5PTS_LENGTH;
  default:
    ELOG_D("Module does not support %d points", num_points);
    MTLK_ASSERT(FALSE);
    return 0;
  }
}

int __MTLK_IFUNC
mtlk_tpcv4_init_empty(mtlk_tpcv4_t   *tpc,
                      int             num_points)
{
  int data_size = 0;

  MTLK_ASSERT( NULL != tpc );

  switch(num_points)
  {
  case 2:
    data_size = _MTLK_TPCV4_2PTS_LENGTH;
    break;
  case 3:
    data_size = _MTLK_TPCV4_3PTS_LENGTH;
    break;
  case 4:
    data_size = _MTLK_TPCV4_4PTS_LENGTH;
    break;
  case 5:
    data_size = _MTLK_TPCV4_5PTS_LENGTH;
    break;
  default:
    ELOG_D("Module does not support %d points", num_points);
    return MTLK_ERR_PARAMS;
  }

  tpc->num_points = num_points;
  tpc->data = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_TPC4);

  if(NULL == tpc->data)
  {
    ELOG_D("Failed to allocate memory (%d bytes)", data_size);
    return MTLK_ERR_NO_MEM;
  }

  memset(tpc->data, 0, data_size);

  ILOG2_DD("Object for empty section with %d points (%d bytes of data) created.", 
       tpc->num_points, data_size);

  return MTLK_ERR_OK;

}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_num_points(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);
  return tpc->num_points;
}

void __MTLK_IFUNC
mtlk_tpcv4_cleanup(mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(NULL != tpc->data);

  mtlk_osal_mem_free(tpc->data);
}

#define _MTLK_MAX_ANTENNAS (3)
#define _MTLK_MAX_POINTS   (5)

/* Following structure defines offsets of lowest 8 bits of power value */
/* per antenna and per point                                           */
static struct 
{ 
  uint8 offset_x; 
  uint8 offset_y;
} pa_lower_byte_offsets[_MTLK_MAX_ANTENNAS][_MTLK_MAX_POINTS] = {
  /* Antenna                                                                 */
  /*    |                                                                    */
  /*    V           Point -->     0        1         2         3         4   */
  /*                                                                         */
  /*    0    */ {             { 7,  8}, { 9, 10}, {20, 21}, {26, 27}, {32, 33} },
  /*    1    */ {             {11, 12}, {13, 14}, {22, 23}, {28, 29}, {34, 35} },
  /*    2    */ {             {15, 16}, {17, 18}, {24, 25}, {30, 31}, {36, 37} }
};

/* Following structure defines offsets of MSB (nines) bit of x values  */
/* per antenna and per point                                           */
static struct 
{ 
  uint8 offset; 
  uint8 bit_number;
} x_high_byte_offsets [_MTLK_MAX_ANTENNAS][_MTLK_MAX_POINTS] = {
  /* Antenna                                                                 */
  /*    |                                                                    */
  /*    V           Point -->     0        1         2         3         4   */  
  /*                                                                         */
  /*    0    */ {             { 3,  5}, {19,  0}, {19,  3}, {19,  6}, {38,  0} },
  /*    1    */ {             { 3,  6}, {19,  1}, {19,  4}, {19,  7}, {38,  1} },
  /*    2    */ {             { 3,  7}, {19,  2}, {19,  5}, { 1,  7}, {38,  2} }
};

static void _mtlk_get_offsets(uint8 antenna, uint8 point_num,
                              int *low_part_x_offset, int* y_offset, 
                              int* high_part_x_offset, int* high_part_x_bit)
{
  *low_part_x_offset = pa_lower_byte_offsets[antenna][point_num].offset_x;
  *y_offset = pa_lower_byte_offsets[antenna][point_num].offset_y;

  *high_part_x_offset = x_high_byte_offsets[antenna][point_num].offset;
  *high_part_x_bit = x_high_byte_offsets[antenna][point_num].bit_number;
};

void __MTLK_IFUNC
mtlk_tpcv4_get_point(const mtlk_tpcv4_t *tpc, uint8 antenna, 
                     uint8 point_num, mtlk_tpc_point_t *point)
{
  int low_part_x_offset, y_offset, high_part_x_offset, high_part_x_bit;

  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(NULL != point);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);
  MTLK_ASSERT(point_num < tpc->num_points);

  _mtlk_get_offsets(antenna, point_num, 
                    &low_part_x_offset, &y_offset, 
                    &high_part_x_offset, &high_part_x_bit);

  point->x = 0;
  MTLK_U_BITS_SET(point->x, 0, 8, tpc->data[low_part_x_offset]);
  MTLK_U_BITS_SET(point->x, 8, 1, MTLK_U_BITS_GET(tpc->data[high_part_x_offset], 
                                                  high_part_x_bit, 1));

  point->y = tpc->data[y_offset];
}

void __MTLK_IFUNC
mtlk_tpcv4_set_point(mtlk_tpcv4_t *tpc, uint8 antenna, 
                     uint8 point_num, const mtlk_tpc_point_t *point)
{
  int low_part_x_offset, y_offset, high_part_x_offset, high_part_x_bit;

  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(NULL != point);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);
  MTLK_ASSERT(point_num < tpc->num_points);

  _mtlk_get_offsets(antenna, point_num, 
                    &low_part_x_offset, &y_offset, 
                    &high_part_x_offset, &high_part_x_bit);

  tpc->data[low_part_x_offset] = MTLK_U_BITS_GET(point->x, 0, 8);
  MTLK_U_BITS_SET(tpc->data[high_part_x_offset], high_part_x_bit,
                  1, MTLK_U_BITS_GET(point->x, 8, 1));

  tpc->data[y_offset] = point->y;
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_packed(const mtlk_tpcv4_t *tpc, uint8 antenna)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);

  return tpc->data[4 + antenna];
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff(const mtlk_tpcv4_t *tpc, uint8 antenna, uint8 point)
{
  int backoff_width_bits;

  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);

  /* Point 0 has no backoff */
  MTLK_ASSERT(point > 0);
  /* Point should be one of the points in the structure */
  MTLK_ASSERT(point < tpc->num_points); 

  backoff_width_bits = mtlk_tpcv4_get_backoff_width(tpc);
  return MTLK_U_BITS_GET(tpc->data[4 + antenna], 
                         (point - 1) *  backoff_width_bits, 
                         backoff_width_bits);
}

void __MTLK_IFUNC
mtlk_tpcv4_set_backoff(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 point, uint8 backoff)
{
  int backoff_width_bits;

  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);

  /* Point 0 has no backoff */
  MTLK_ASSERT(point > 0);
  /* Point should be one of the points in the structure */
  MTLK_ASSERT(point < tpc->num_points); 

  backoff_width_bits = mtlk_tpcv4_get_backoff_width(tpc);
  
  /* Backoff field width should fit the value */
  MTLK_ASSERT(backoff < (1 << backoff_width_bits));

  MTLK_U_BITS_SET(tpc->data[4 + antenna], 
                  (point - 1) *  backoff_width_bits, 
                  backoff_width_bits,
                  backoff);
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_channel(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);
  return tpc->data[0];
}

void __MTLK_IFUNC
mtlk_tpcv4_set_channel(mtlk_tpcv4_t *tpc, uint8 channel)
{
  MTLK_ASSERT(NULL != tpc);
  tpc->data[0] = channel;
}

MTLK_TPC_CB_FLAG __MTLK_IFUNC
mtlk_tpcv4_get_cb_flag(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);
  return (MTLK_TPC_CB_FLAG) MTLK_U_BITS_GET(tpc->data[1], 6, 1);
}

void __MTLK_IFUNC
mtlk_tpcv4_set_cb_flag(mtlk_tpcv4_t *tpc, MTLK_TPC_CB_FLAG cb_flag)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(cb_flag < 2);

  MTLK_U_BITS_SET(tpc->data[1], 6, 1, cb_flag);
}

MTLK_TPC_BAND __MTLK_IFUNC
mtlk_tpcv4_get_band_by_raw_buffer(uint8* buffer)
{
  return (MTLK_TPC_BAND) MTLK_U_BITS_GET(buffer[1], 5, 1);
}

MTLK_TPC_BAND __MTLK_IFUNC
mtlk_tpcv4_get_band(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);
  return mtlk_tpcv4_get_band_by_raw_buffer(tpc->data);
}

void __MTLK_IFUNC
mtlk_tpcv4_set_band(mtlk_tpcv4_t *tpc, MTLK_TPC_BAND band)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(band < 2);

  MTLK_U_BITS_SET(tpc->data[1], 5, 1, band);
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_tpc_val(const mtlk_tpcv4_t *tpc, uint8 antenna)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);

  return MTLK_U_BITS_GET(tpc->data[antenna + 1], 0, 5);
}

void __MTLK_IFUNC
mtlk_tpcv4_set_tpc_val(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 tpc_val)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);
  MTLK_ASSERT(tpc_val < 0x20);

  MTLK_U_BITS_SET(tpc->data[antenna + 1], 0, 5, tpc_val);
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_mul(const mtlk_tpcv4_t *tpc, uint8 antenna)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);

  return MTLK_U_BITS_GET(tpc->data[2], 5 + antenna, 1);
}

void __MTLK_IFUNC
mtlk_tpcv4_set_backoff_mul(mtlk_tpcv4_t *tpc, uint8 antenna, uint8 mul_val)
{
  MTLK_ASSERT(NULL != tpc);
  MTLK_ASSERT(antenna < _MTLK_MAX_ANTENNAS);
  MTLK_ASSERT(mul_val < 2);

  MTLK_U_BITS_SET(tpc->data[2], 5 + antenna, 1, mul_val);
}

uint8 __MTLK_IFUNC
mtlk_tpcv4_get_backoff_width(const mtlk_tpcv4_t *tpc)
{
  MTLK_ASSERT(NULL != tpc);

  /* Number of bits for backoff may be 2 or 4 */
  /* depending on number points, the idea is  */
  /* to fit all backoffs for into the 8 bits. */
  return (tpc->num_points < 4) ? 4 : 2;
}

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"
