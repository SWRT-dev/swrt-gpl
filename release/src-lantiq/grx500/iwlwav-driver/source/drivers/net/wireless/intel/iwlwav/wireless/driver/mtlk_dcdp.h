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
 * Definition of DirectConnect types
 *
 */

#ifndef __MTLK_DCDP_H__
#define __MTLK_DCDP_H__

#define MTLK_DCDP_SEPARATE_REG      (0)     /* Register and initilalize datapath device in one place (0) or separately (1). */
                                            /* Should be removed further! */
#if MTLK_USE_DIRECTCONNECT_DP_API
#include <net/directconnect_dp_api.h>


#ifndef DC_DP_F_MC_NEW_STA
#define DC_DP_F_MC_NEW_STA           (0x20)
#endif

/* Detect extended DC DP version supporting new platforms: LGM & FLM */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,0)
#define WAVE_DCDP_LGM_FLM_SUPPORTED
#endif

/* Detect extended DC DP version supporting setting the number of PDs */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,3)
#define WAVE_DCDP_NUM_OF_PDS_SUPPORTED
#endif

/* Detect extended DC DP version supporting setting the DGAF */
#if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,4)
#define WAVE_DCDP_DGAF_SUPPORTED
#endif

/* Define new flags for compatibility with version not supporting LGM */
#ifndef WAVE_DCDP_LGM_FLM_SUPPORTED
#define DC_DP_F_FAST_WLAN           (0)     /* Formal definition. Real value is != 0 */
#define DC_DP_RING_HW_MODE0_EXT     (0x10)  /* Formal definition. Real value differs! */
#define DC_DP_RING_HW_MODE1_EXT     (0x11)  /* Formal definition. Real value differs! */
#endif

#define MTLK_DCDP_SWPATH_SUPPORTED  (0)     /* Set to 1 if DC DP SWPATH is supported by DCDP and driver */

#define MTLK_DCDP_DCCNTR_NUM        (1)     /* Number of DC counters. For WAVE500/600 it is 1 */
#define MTLK_DCDP_DCCNTR_SIZE       (4)     /* Counter has 4 bytes length */
#define MTLK_DCDP_DCCNTR_LENGTH     (1)     /* Length of counters. */

#define WAVE_DCDP_MAX_PORTS         (2)     /* Max number of supported ports */
#define WAVE_DCDP_MAX_NDEVS         (2)     /* Max number of supported ndevs */

typedef enum _mtlk_hw_dcdp_mode_t {
  MTLK_DC_DP_MODE_UNREGISTERED,     /*!< Device unregistered */
  MTLK_DC_DP_MODE_UNKNOWN,          /*!< Unknown mode  */
  MTLK_DC_DP_MODE_FASTPATH,         /*!< Fastpath mode */
  MTLK_DC_DP_MODE_SWPATH,           /*!< Software path mode */
  MTLK_DC_DP_MODE_LITEPATH,         /*!< LitePath mode + SW path */
  MTLK_DC_DP_MODE_LITEPATH_ONLY     /*!< LitePatn mode only. SW path should be supported internally by driver */
} mtlk_hw_dcdp_mode_t;

typedef enum _mtlk_dcdp_cntr_mode_t {
  MTLK_DC_DP_CNTR_MODE_LITTLE_ENDIAN,
  MTLK_DC_DP_CNTR_MODE_BIG_ENDIAN,
} mtlk_dcdp_cntr_mode_t;


#ifndef WAVE_DCDP_LGM_FLM_SUPPORTED
/* For DC DP API V4.x we will simulate reading of config parameters*/
struct dc_dp_dev_config {
    uint32_t txin_ring_size; /*!< [out] Max TX_IN Ring Size supported by SoC */
    uint32_t txout_ring_size; /*!< [out] Max TX_OUT Ring Size supported by SoC */
    uint32_t rxin_ring_size; /*!< [out] Max RX_IN Ring Size supported by SoC */
    uint32_t rxout_ring_size; /*!< [out] Max RX_OUT Ring Size supported by SoC */
    uint32_t rx_bufs; /*!< [out] Max Rx Bufs requested by Peripheral for prefetching in RXIN */
    uint32_t tx_bufs; /*!< [out] Max Tx Bufs requested by Peripheral for prefetching in TXIN */
};
#endif

typedef struct _mtlk_dcdp_init_info_t {
  uint32                rd_pool_size;
  #ifdef WAVE_DCDP_NUM_OF_PDS_SUPPORTED
  uint32                num_of_pds;
  #endif
  uint32                tx_ring_size;
  uint32                rx_ring_size;
  uint32                frag_ring_size;
  /* RX_IN + TX_IN */
  void                 *soc2dev_enq_phys_base;
  void                 *soc2dev_enq_base;
  void                 *dev2soc_ret_enq_phys_base;
  void                 *dev2soc_ret_enq_base;
  /* RX_OUT + TX_OUT */
  void                 *soc2dev_ret_deq_phys_base;
  void                 *soc2dev_ret_deq_base;
  void                 *dev2soc_deq_phys_base;
  void                 *dev2soc_deq_base;
  /* Fragmentation ring */
  void                 *dev2soc_frag_ring_phys_base;
  void                 *dev2soc_frag_ring_base;

  void                 *dev2soc_frag_deq_phys_base;
  void                 *dev2soc_frag_deq_base;
  void                 *dev2soc_frag_enq_phys_base;
  void                 *dev2soc_frag_enq_base;

  mtlk_dcdp_cntr_mode_t cntr_mode;
} mtlk_dcdp_init_info_t;


typedef struct _mtlk_dcdp_dev_t {
  /* port dependent part */
  BOOL                  is_registered[WAVE_DCDP_MAX_NDEVS];
  uint8                 dp_dev_port  [WAVE_DCDP_MAX_NDEVS]; /*!< DirectConnect dev port */
  int32                 dp_port_id   [WAVE_DCDP_MAX_PORTS]; /*!< DirectConnect port id */
  /* ndev dependent part */
  mtlk_ndev_t          *dp_ndev      [WAVE_DCDP_MAX_NDEVS]; /*!< network device */
  mtlk_ndev_t          *dp_radio_ndev[WAVE_DCDP_MAX_NDEVS]; /*!< radio network device (created separately for Litepath mode) */
  struct dc_dp_dev      dp_devspec   [WAVE_DCDP_MAX_NDEVS];
 /* common part */
  uint32                dp_port_flags;      /*!< DirectConnect port allocation flags */
  mtlk_hw_dcdp_mode_t   dp_mode;            /*!< Actual working mode */
  DataPathMode_e        fw_datapath_mode;   /*!< FW datapath mode */
  struct dc_dp_host_cap dp_cap;             /*!< DCDP driver capabilities */
  struct dc_dp_dccntr   dp_dccntr[MTLK_DCDP_DCCNTR_NUM]; /*!< dc dp counters */
  struct dc_dp_res      dp_resources;
  struct dc_dp_dev_config dp_config;
  BOOL                  dp_frag_wa_enable;  /*!< TRUE if Fragmentation workaround is required */
} mtlk_dcdp_dev_t;

#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

#endif /* __MTLK_DCDP_H__ */
