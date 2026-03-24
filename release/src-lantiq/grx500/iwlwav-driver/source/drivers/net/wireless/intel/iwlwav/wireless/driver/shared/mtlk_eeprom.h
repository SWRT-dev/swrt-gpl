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
 * EEPROM data processing module
 *
 * Originally written by Grygorii Strashko
 *
 */

#ifndef MTLK_EEPROM_H_
#define MTLK_EEPROM_H_

#define MTLK_MAKE_EEPROM_VERSION(major, minor) (((major) << 8) | (minor))

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef uint8   mtlk_one_byte_data_t;

typedef struct {
    union {
      uint8 bytes[2];
      struct {
        uint8   lsb;
        uint8   msb;
      };
    };
} mtlk_two_bytes_data_t;

static __INLINE uint16
mtlk_two_bytes_value (mtlk_two_bytes_data_t *p)
{
    return (((uint16)(p->lsb) << 0) + /* LSB */
            ((uint16)(p->msb) << 8)); /* MSB */
}

/*--------- CIS v6 RX (RSSI) --------------------------------*/
typedef struct {
    uint8                   ant_mask;
    mtlk_two_bytes_data_t   start_chan;
    mtlk_two_bytes_data_t   end_chan;
    mtlk_two_bytes_data_t   calibration_chan;
    uint8                   calibration_temperature;
    /* ... */
} cis_v6_rssi_t;

/*--------- CIS v6 TX Channel for 1, 2 or 3 regions ---------*/

/* Per region data sizes */
#define CIS_V6_TX_CHAN_PW_EVM_SIZE          (3)   /* 3 bytes: QAM16, UEVM idx, UEVM gain */
#define CIS_V6_TX_CHAN_S2D_GAIN_SIZE(n)     (BITS_TO_BYTES((n) * 4)) /* 4 bits per region */
#define CIS_V6_TX_CHAN_S2D_OFFS_SIZE(n)     (1 * (n))                /* 1 byte per region */
#define CIS_V6_TX_CHAN_AB_DATA_SIZE(n)      (4 * (n))                /* 4 bytes: a/b lsb/msb */

#define CIS_V6_TX_CHAN_DATA_SIZE(n)         (CIS_V6_TX_CHAN_PW_EVM_SIZE      + \
                                             CIS_V6_TX_CHAN_S2D_GAIN_SIZE(n) + \
                                             CIS_V6_TX_CHAN_S2D_OFFS_SIZE(n) + \
                                             CIS_V6_TX_CHAN_AB_DATA_SIZE(n))

#define CIS_V6_TX_CHAN_DATA_MIN_SIZE        (CIS_V6_TX_CHAN_DATA_SIZE(1)) /* min 1 region */
#define CIS_V6_TX_CHAN_DATA_MAX_SIZE        (CIS_V6_TX_CHAN_DATA_SIZE(3)) /* max 3 region */

typedef struct {
    mtlk_two_bytes_data_t   chan_num_and_bw;
                            /* without Channel data */
    uint8                   chan_data[0];
} cis_v6_tx_chan_hdr_t;

typedef struct {
    mtlk_two_bytes_data_t   chan_num_and_bw;
                            /* with max size of Channel data */
    uint8                   chan_data[CIS_V6_TX_CHAN_DATA_MAX_SIZE];
} cis_v6_tx_chan_data_t;

typedef struct {
    /* Common section */
    uint8                   ant_mask_and_nof_regions;
    cis_v6_tx_chan_hdr_t    chan_hdr[1]; /* at least one */
} cis_v6_tx_common_t;

/* ant_mask_and_nof_regions */
#define CIS_TX_COMMON_ANT_MASK      MTLK_BFIELD_INFO(0, 5)
#define CIS_TX_COMMON_NOF_REGIONS   MTLK_BFIELD_INFO(6, 2)

#define CIS_TX_COMMON_CHAN_NUM      MTLK_BFIELD_INFO(0, 11)
/* Bit 11 indicates if it is 6GHz channel or not */
#define CIS_TX_COMMON_CHAN_6G       MTLK_BFIELD_INFO(11,1)
#define CIS_TX_COMMON_BANDWIDTH     MTLK_BFIELD_INFO(12, 4)
#define CIS_TX_COMMON_BAND_MASK     0x08

/*-----------------------------------------------------------*/

struct _mtlk_eeprom_tpc_chan_data_t
{
  struct _mtlk_eeprom_tpc_chan_data_t *next;
  uint8     band;
  uint8     channel;
  uint8     bandwidth;
  uint8     data_size;
  union {
    TPC_FREQ                tpc_hdk_data;
    cis_v6_tx_chan_data_t   tpc_chan_data;
  };
} __MTLK_IDATA;

typedef struct _mtlk_eeprom_tpc_chan_data_t mtlk_eeprom_tpc_data_g6_t;

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

/* Functions can be declared here */

#endif /* MTLK_EEPROM_H_ */
