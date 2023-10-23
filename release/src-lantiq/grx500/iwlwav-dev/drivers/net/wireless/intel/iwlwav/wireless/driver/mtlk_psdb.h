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
 * $Id:
 *
 * Platform Specific Data Base header
 *
 */

#ifndef __MTLK_PSDB_H__
#define __MTLK_PSDB_H__

#include "mtlk_coreui.h"
#include "mtlk_card_types.h"

 /*-------------------------------------------------*/
/*--- Data structures for the PSDB file content ---*/

/* PSDB file Version Info */
typedef struct psdf_ver_info {
    uint32      ver_num;
    uint32      rev_hash_lsb;
    uint32      rev_hash_msb;
} psdf_ver_info_t;

/* Platform data filed block */
typedef struct psdf_fields_block {
    uint32      id;     /* Field ID */
    uint32      value;  /* Field value */
} psdf_fields_block_t;

/* Platform data Regulatory Domain table entries */
typedef struct psdf_regd_entry {
    uint32      regd_code;  /* RegDomain code */
    uint32      chan_freq;  /* Channel frequency */
    uint32      chanwidth;  /* Bandwidth in MHz */
    uint32      phy_mode;   /* psdb_phy_mode_e */
    uint32      pw_limit;   /* Power Limit value */
} psdf_regd_entry_t;

/* Platform data Rate Power Offset table entries */
typedef struct psdf_rate_pw_entry {
    int32       pw_offs;    /* Power offset (signed value) */
    uint32      evm_back;   /* EVM Backoff */
} psdf_rate_pw_entry_t;

typedef struct psdb_mask_per_chan
{
  uint32        upper_channel;
  uint32        ant_mask;
} psdb_mask_per_chan_t;

typedef struct psdb_cca_th_entry
{
  int32         value_2G;
  int32         value_5G;
  int32         value_6G;
} psdb_cca_th_entry_t;

/* Platform data any table content, i.e. data */
typedef struct psdf_table_data {
    uint32  nrows;
    uint32  ncols;
    union {
        psdf_fields_block_t   fblocks[0]; /* Field blocks */
        psdf_regd_entry_t     regd_entries[0]; /* Regd entries */
        psdf_rate_pw_entry_t  rate_pw_entries[0];
        psdb_mask_per_chan_t  mask_per_chan_entries[0];
        psdb_cca_th_entry_t   cca_th_entries[0];
        uint32                words[0];
    };
} psdf_table_data_t;


/* Platform data any table (including RegDomain table) */
typedef struct psdf_any_table {
    uint32              marker;
    psdf_table_data_t   data;
} psdf_any_table_t;

/* Platform data Regulatory Domain table */
typedef struct psdf_any_table   psdf_regd_table_t;

/* Platform data fileds array */
typedef struct psdf_fields_array {
    psdf_any_table_t    table;      /* Marker and sizes */
    psdf_fields_block_t chip_id;    /* 1st */
    psdf_fields_block_t chip_rev;   /* 2nd */
} psdf_fields_array_t;

/*-------------------------------------------------*/
/*--- PSDB related Internal Driver Data structures */

/* CDB configuration table */
typedef enum   wave_cdb_cfg         wave_cdb_cfg_e;
typedef struct psdb_cdb_cfg_table   psdb_cdb_cfg_table_t;
typedef struct psdb_radio_cfg       psdb_radio_cfg_t;
typedef struct hw_psdb              hw_psdb_t;

enum wave_cdb_cfg {
  WAVE_CDB_CFG_INVALID = 0, /* invalid     */
  WAVE_CDB_CFG_SINGLE  = WAVE_CDB_CFG_INVALID,
  WAVE_CDB_CFG_2G,          /* 2.4 GHz     */
  WAVE_CDB_CFG_5G,          /* 5 GHz       */
  WAVE_CDB_CFG_5G_LOW,      /* 5 GHz low   */
  WAVE_CDB_CFG_5G_HIGH,     /* 5 GHz high  */
  WAVE_CDB_CFG_ALTERN,      /* alternating */
  WAVE_CDB_CFG_6G_UNII5,    /* 6 GHz U-NII-5 band only*/
  WAVE_CDB_CFG_6G,          /* 6GHz full spectrum */
  WAVE_CDB_CFG_NOT_SET,
  WAVE_CDB_CFG_LAST
};

typedef struct {
  uint8             tx_ant_num;      /* Number of TX antennas */
  uint8             rx_ant_num;      /* Number of RX antennas */
  uint8             tx_ant_mask;     /* TX antennas mask */
  uint8             rx_ant_mask;     /* RX antennas mask */
  uint8             tx_ant_sel_mask; /* Tx_AntSelection_Mask */
  uint8             rx_ant_sel_mask; /* Rx_AntSelection_Mask */
  uint8             tx_ant_factor;   /* will be added to TX power limit */
} wave_ant_params_t;

struct psdb_radio_cfg {
  wave_cdb_cfg_e    cdb_config;
  mtlk_hw_band_e    wave_band;   /* band in wave format */
  uint8             lower_chan;
  uint8             upper_chan;
  wave_ant_params_t radio_ant_cfg;
};

struct psdb_cdb_cfg_table {
  /* Radio configs */
  psdb_radio_cfg_t  radio_cfg[WAVE_CARD_RADIO_NUM_MAX];
  /* Card config */
  wave_ant_params_t card_ant_cfg;
  BOOL              band_support_2G;
  BOOL              band_support_5G;
  BOOL              band_support_6G;
  uint8             zwdfs_ant_mask;
  int8              cca_th_params[MTLK_HW_BAND_MAX][MTLK_CCA_TH_PARAMS_LEN]; /* 2G/5G/6G Hz */
  BOOL              cca_th_is_filled; /* TRUE if filled */
                    /* last channel for radio 0, all above are for radio 1 */
  uint8             radio_chan_threshod;
  uint8             nof_entries; /* number of radios configured */
};

static __INLINE BOOL
wave_psdb_cdb_cfg_idx_is_valid (psdb_cdb_cfg_table_t *cdb_table, wave_uint idx)
{
  return ((cdb_table != NULL) &&
          (ARRAY_SIZE(cdb_table->radio_cfg) > idx));
}

static __INLINE BOOL
wave_cdb_cfg_is_unknown (wave_cdb_cfg_e config)
{ /* Checking for the range */
  return ((unsigned)WAVE_CDB_CFG_LAST <= (unsigned)config);
}

static __INLINE BOOL
wave_cdb_cfg_is_invalid (wave_cdb_cfg_e config)
{ /* Checking for exact value */
  return (WAVE_CDB_CFG_INVALID == config);
}

static __INLINE BOOL
wave_cdb_cfg_is_not_set (wave_cdb_cfg_e config)
{ /* Checking for the range */
  return (WAVE_CDB_CFG_NOT_SET == config);
}

const char *    __MTLK_IFUNC   wave_cdb_cfg_to_string(wave_cdb_cfg_e config);
wave_cdb_cfg_e  __MTLK_IFUNC   wave_psdb_get_radio_cdb_config (hw_psdb_t *psdb, unsigned idx);

int __MTLK_IFUNC
wave_psdb_get_radio_cca_th_params (hw_psdb_t *psdb, unsigned radio_idx, iwpriv_cca_th_t *params);

/* Regulatory Domain table */

/* Phy modes */
typedef enum psdb_phy_mode_e {
    PSDB_PHY_MODE_11B   = 0,
    PSDB_PHY_MODE_OFDM  = 1,
    PSDB_PHY_MODE_MU    = 2, /* Multi user */
    PSDB_PHY_MODE_BF    = 3, /* Beamforming */
    PSDB_PHY_MODE_11AX  = 4,
    PSDB_PHY_MODE_11N   = 5,
    PSDB_PHY_MODE_11AG  = 6,
    PSDB_PHY_MODE_BF_11AX  = 7

} psdb_phy_mode_t;

/* Chanwidth */
typedef enum psdb_chanwidth_e {
    PSDB_CHANWIDTH_20   = 20,
    PSDB_CHANWIDTH_40   = 40,
    PSDB_CHANWIDTH_80   = 80,
    PSDB_CHANWIDTH_160  = 160
} psdb_chanwidth_t;

/* Phy mode + Chanwidth (per channel) */
typedef enum psdb_phy_cw_e {
    PSDB_PHY_CW_11B     = 0,
    PSDB_PHY_CW_OFDM_20,
    PSDB_PHY_CW_OFDM_40,
    PSDB_PHY_CW_OFDM_80,
    PSDB_PHY_CW_OFDM_160,
    PSDB_PHY_CW_MU_20,
    PSDB_PHY_CW_MU_40,
    PSDB_PHY_CW_MU_80,
    PSDB_PHY_CW_MU_160,
    PSDB_PHY_CW_BF_20,
    PSDB_PHY_CW_BF_40,
    PSDB_PHY_CW_BF_80,
    PSDB_PHY_CW_BF_160,
    PSDB_PHY_CW_AX_20,
    PSDB_PHY_CW_AX_40,
    PSDB_PHY_CW_AX_80,
    PSDB_PHY_CW_AX_160,
    PSDB_PHY_CW_N_20,
    PSDB_PHY_CW_N_40,
    PSDB_PHY_CW_AG_20,
    PSDB_PHY_CW_BF_AX_20,
    PSDB_PHY_CW_BF_AX_40,
    PSDB_PHY_CW_BF_AX_80,
    PSDB_PHY_CW_BF_AX_160,

    PSDB_PHY_CW_LAST
} psdb_phy_cw_t;

/* Platform data any table (including RegDomain table) */
typedef struct psdb_any_table {
    uint32  offs;       /* Offset in PSDB file */
    uint32  id;         /* Field ID */
    uint32  nrows;      /* Number of rows    */
    uint32  ncols;      /* Number of columns */
    uint32  nof_words;  /* Number of words   */
    psdf_table_data_t data; /* Table data */
} psdb_any_table_t;

/* Platform data fields array */
typedef struct psdb_any_table   psdb_fields_array_t;

/* Power limits for all PHY modes */
typedef struct psdb_pw_limits {
    uint16   pw_limits[PSDB_PHY_CW_LAST];
} psdb_pw_limits_t;

/* Regulatory Domain related data */
typedef struct psdb_regd_parsed {
    uint16              regd_code;  /* RegDomain code */
    char                country[2]; /* Country code */
    psdb_pw_limits_t    pw_by_chan[NUM_TOTAL_CHAN_FREQS + 1]; /* HW power limits for all channels in 2.4 and 5 GHz bands */
    psdb_pw_limits_t    pw_by_chan_6g[NUM_TOTAL_CHAN_FREQS_6G + 1]; /* HW power limits for all the channels in the 6GHz band */
} psdb_regd_parsed_t;

/* Rate Power related data */

#define PSDB_RATES_B      1 /* The same for all */
#define PSDB_RATES_AG     NUM_RATE_MCS_AG
#define PSDB_RATES_N      NUM_RATE_MCS_N
#define PSDB_RATES_AC     (NUM_RATE_MCS_AC * PHY_STAT_RATE_NSS_MAX)
#define PSDB_RATES_AX     (NUM_RATE_MCS_AX * PHY_STAT_RATE_NSS_MAX)
#define PSDB_RATES_TOTAL  (PSDB_RATES_B + PSDB_RATES_AG +  PSDB_RATES_N + PSDB_RATES_AC + PSDB_RATES_AX)

typedef struct psdb_rate_pw_table {
  union {
    mtlk_rate_pw_entry_t    rates_all[PSDB_RATES_TOTAL];
    struct {
      mtlk_rate_pw_entry_t  rates_11b[PSDB_RATES_B];
      mtlk_rate_pw_entry_t  rates_11ag[PSDB_RATES_AG];
      mtlk_rate_pw_entry_t  rates_11n[PSDB_RATES_N];
      mtlk_rate_pw_entry_t  rates_11ac[PSDB_RATES_AC];
      mtlk_rate_pw_entry_t  rates_11ax[PSDB_RATES_AX];
    };
  };
} psdb_rate_pw_table_t;

typedef struct psdb_ant_mask_per_chan_table
{
  uint32                nof_entries;
  psdb_mask_per_chan_t  entries[0];
} psdb_ant_mask_per_chan_table_t;

/* Data structure for all the Platform Specific data.
 * This sctructure will be created per HW card.
 */
struct hw_psdb {
    psdf_ver_info_t     *ver_info;      /* Version Info */
    psdb_fields_array_t *fields_data;   /* Pointer to Fields array */
    psdb_any_table_t    *regd_table;    /* Pointer to unparsed RegDomain table */
    psdb_regd_parsed_t  *regd_parsed;   /* Pointer to parsed RegDomain table */
    psdb_rate_pw_table_t *rate_pw_table; /* Pointer to Rate Power table */

    psdb_ant_mask_per_chan_table_t  *ant_mask_per_chan; /* Pointer to AntennaMask table */

    psdb_cdb_cfg_table_t  cdb_cfg_table; /* CDB config */

    psdb_any_table_t    **tables;       /* Pointer to array of tables */
    uint32               nof_tables;    /* Number of tables (without RegDomain) */

    uint32               beamform_caps; /* Beamforming capabilities */

    uint8                chip_rev;      /* Chip_Revision */
};

/*-------------------------------------------------*/

static __INLINE psdb_ant_mask_per_chan_table_t *
wave_psdb_get_ant_mask_per_chan_table (hw_psdb_t *psdb)
{
  psdb_ant_mask_per_chan_table_t *ampc = NULL;

  if (psdb) {
    ampc = psdb->ant_mask_per_chan;
  }

  return ampc;
}

static __INLINE void
wave_psdb_cfg_radio_number_get (hw_psdb_t *psdb, unsigned *radio_number)
{
  *radio_number = psdb->cdb_cfg_table.nof_entries;
}

wave_ant_params_t *
wave_psdb_get_card_ant_params (hw_psdb_t *psdb);

/*-------------------------------------------------*/

hw_psdb_t           *mtlk_core_get_psdb (mtlk_core_t *core);
hw_psdb_t           *mtlk_hw_get_psdb (mtlk_hw_t *hw);
psdb_any_table_t    *mtlk_hw_get_psdb_regd_table (mtlk_hw_t *hw);

/* Field IDs of single fields */
#define PSDB_FIELD_CHIP_ID          0x01 /* Chip ID of platform */
#define PSDB_FIELD_CHIP_REV         0x02 /* Chip_Revision */
#define PSDB_FIELD_RF_CHIP_ID       0x03 /* RF_Chip_ID */
#define PSDB_FIELD_BAND_SUPPORT_2G  0x04 /* Band_Support_2.4G */
#define PSDB_FIELD_BAND_SUPPORT_5G  0x05 /* Band_Support_5G */
#define PSDB_FIELD_TX_ANTENNAS_MASK 0x06 /* TX antennas mask */
#define PSDB_FIELD_RX_ANTENNAS_MASK 0x07 /* RX antennas mask */
#define PSDB_FIELD_TX_ANT_SEL_MASK  0x08 /* Tx_AntSelection_Mask */
#define PSDB_FIELD_RX_ANT_SEL_MASK  0x09 /* Rx_AntSelection_Mask */
#define PSDB_FIELD_HW_TYPE          0x0B /* HW type */
#define PSDB_FIELD_HW_REV           0x0C /* HW revision */
#define PSDB_FIELD_BAND_SUPPORT_6G  0x0D /* Band_Support_6G */
#define PSDB_FIELD_ZWDFS_ANT_MASK   0x0E /* ZWDFS antenna mask */

/* Beamforming capabilities (BfSupportedModesBitMap) */
#define PSDB_BF_CAPS_IMPLICIT_BF_SUPPORT        MTLK_BFIELD_INFO(0,  1) /* 1 bit starting BIT 0 */
#define PSDB_BF_CAPS_IMPLICIT_BF_CDD_SUPPORT    MTLK_BFIELD_INFO(1,  1) /* 1 bit starting BIT 1 */
#define PSDB_BF_CAPS_STBC_1X2_SUPPORT           MTLK_BFIELD_INFO(2,  1) /* 1 bit starting BIT 2 */
#define PSDB_BF_CAPS_STBC_2x4_SUPPORT           MTLK_BFIELD_INFO(3,  1) /* 1 bit starting BIT 3 */
#define PSDB_BF_CAPS_EXPLICIT_SUPPORT           MTLK_BFIELD_INFO(4,  1) /* 1 bit starting BIT 4 */
#define PSDB_BF_CAPS_EXPLICIT_BF_CALIBR_REQ     MTLK_BFIELD_INFO(5,  1) /* 1 bit starting BIT 5 */
#define PSDB_BF_CAPS_IMPLICIT_BF_CALIBR_REQ     MTLK_BFIELD_INFO(6,  1) /* 1 bit starting BIT 6 */

/* Conversion from channel number to index for 6GHz band: 6G chan num = (2*index)+1 *
 * Note: The chan nums 31,63,95,...,223,231 are not allowed                         */
#define MOD_32_MASK              0x1F /* chan num % 32 */
#define INVALID_6G_CHAN_NUM_LOW  0x1F /* remainder = 31 */
#define LAST_INVALID_6G_CHAN_NUM 231

static __INLINE uint32
mtlk_psdb_get_chip_revision (hw_psdb_t *psdb)
{
    return psdb->chip_rev;
}

static __INLINE psdb_any_table_t *
mtlk_psdb_get_regd_table (hw_psdb_t *psdb)
{
    return psdb->regd_table;
}

int psdb_get_hw_limits (hw_psdb_t *psdb, const char *country, int chan_freq, psdb_pw_limits_t *pw_limits);

int psdb_get_field_val (hw_psdb_t *psdb, uint32 id, uint32 *val);

int core_get_psdb_hw_limits (mtlk_core_t *core, const char *country, int chan_freq, psdb_pw_limits_t *pw_limits);

int __MTLK_IFUNC mtlk_psdb_load_data (mtlk_hw_t *hw, mtlk_txmm_t *txmm, hw_psdb_t *psdb);
int __MTLK_IFUNC mtlk_psdb_get_rate_power_list (mtlk_core_t *core, mtlk_clpb_t *clpb);

uint32 __MTLK_IFUNC mtlk_psdb_country_to_regd_code (const char *country);
BOOL   __MTLK_IFUNC mtlk_psdb_country_is_supported (const char *country);

uint32 psdb_get_bf_capabilities (hw_psdb_t *psdb);
BOOL   psdb_get_bf_explicit_cap (hw_psdb_t *psdb);

uint32 core_get_psdb_bf_capabilities (mtlk_core_t *core);

#ifdef CONFIG_WAVE_DEBUG
BOOL   core_get_psdb_bf_explicit_cap (mtlk_core_t *core);
#endif

int __MTLK_IFUNC mtlk_psdb_get_hw_limits_list (mtlk_core_t *core, mtlk_clpb_t *clpb);

uint32 __MTLK_IFUNC wave_psdb_get_ant_mask_by_channel (hw_psdb_t *psdb, uint32 channel);

const wave_ant_params_t *
       __MTLK_IFUNC wave_psdb_get_radio_ant_params(hw_psdb_t *psdb, unsigned radio_idx);

BOOL   __MTLK_IFUNC wave_psdb_is_radio_band_supported(hw_psdb_t *psdb, unsigned radio_idx, mtlk_hw_band_e band);

uint8 __MTLK_IFUNC wave_psdb_get_hw_radio_band_cfg (hw_psdb_t *psdb);

uint8 __MTLK_IFUNC wave_psdb_ant_mask_to_ant_sel_mask(uint8 ant_mask);

#endif /* __MTLK_PSDB_H__ */
