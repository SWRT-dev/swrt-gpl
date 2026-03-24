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
 * Originally written by Andrii Tseglytskyi
 *
 */

#include "mtlkinc.h"
#include "mtlkerr.h"
#include "channels.h"
#include "mtlkaux.h"

#include "eeprom.h"
#include "mtlk_eeprom.h"
#include "cis_manager.h"
#include "mtlk_algorithms.h"
#include "mtlk_coreui.h"
#include "mtlk_df_fw.h"
#include "mtlk_df.h"
#include "mtlkhal.h"
#include "hw_mmb.h"

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  #include "eeprom_gpio.h"
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

#define LOG_LOCAL_GID   GID_EEPROM
#define LOG_LOCAL_FID   0

/*****************************************************************************
* Local type definitions
******************************************************************************/

/* \cond DOXYGEN_IGNORE */
#define  MTLK_IDEFS_ON
#define  MTLK_IDEFS_PACKING 1
#include "mtlkidefs.h"
/* \endcond */

/* From TTPCom document "PCI/CardBus Host Reference Configuration
   Hardware Specification" p. 32 */
#define MTLK_EEPROM_EXEC_SIGNATURE              0x1BFC

#define MTLK_EE_BLOCKED_SEND_TIMEOUT     (10000) /* ms */

/* PCI configuration */
typedef struct _mtlk_eeprom_pci_cfg_v5_t {      /* len  ofs */
  /* uint16 eeprom_executive_signature; */      /*  2    2  */
  uint8 cal_file_type;                          /*  1    3  */ /* v6 specific */
  uint8 reserved1;                              /*  1    4  */
  uint8  revision_id;                           /*  1    5  */
  uint8  class_code[3];                         /*  3    8  */
  uint16 vendor_id;                             /*  2   10  */
  uint16 device_id;                             /*  2   12  */
  uint16 subsystem_vendor_id;                   /*  2   14  */
  uint16 subsystem_id;                          /*  2   16  */
  uint32 link_capabilities;                     /*  4   20  */
} __MTLK_IDATA mtlk_eeprom_pci_cfg_v5_t;

typedef struct _mtlk_eeprom_v5_t {
  mtlk_eeprom_pci_cfg_v5_t  config_area;        /*  2...19 */
  uint8                 reserved1[11];          /* 20...30 */
  uint8                 reserved2[ 9];          /* 31...39 */
  mtlk_cis_area_v5_t    cis_area;               /* 40      */
  /* ... CIS sections ... */
  uint8                 cis_data[0];
} __MTLK_IDATA mtlk_eeprom_v5_t;

typedef struct _mtlk_eeprom_t {
    uint16 eeprom_executive_signature;            /*  2    2  */
    mtlk_eeprom_v5_t    eeprom_v5;
} mtlk_eeprom_t;

/* Wave500/Wave600 EFUSE support */
typedef struct {
    union {
      uint8     bytes[MAX_AFE_CALIBRATION_DATA_SIZE];
      wave_efuse_afe_data_v6_t  v6;
    };
} mtlk_efuse_afe_data_t;

typedef struct {
  union {
      uint8 byte;
      struct {
        /* capability bits */
#if defined(__LITTLE_ENDIAN_BITFIELD)
        uint8   rev_id0     : 1;
        uint8   rev_id1     : 1;
        uint8   rev_id2     : 1;
        uint8   wapi_enable : 1;
        uint8   chip_mode0  : 1;
        uint8   chip_mode1  : 1;
        uint8   reserved6   : 1;
        uint8   reserved7   : 1;
#elif defined(__BIG_ENDIAN_BITFIELD)
        uint8   reserved7   : 1;
        uint8   reserved6   : 1;
        uint8   chip_mode1  : 1;
        uint8   chip_mode0  : 1;
        uint8   wapi_enable : 1;
        uint8   rev_id2     : 1;
        uint8   rev_id1     : 1;
        uint8   rev_id0     : 1;
#else
  # error Endianess not defined!
#endif
      } bits;
  };
} wave_efuse_capability_t;

struct wave_efuse_v6 {
  wave_efuse_capability_t   capability;     /*  0        */
  uint8                     reserved1[1];   /*  1        */
  /* wave_eeprom_pci_dual_ep_cfg_t */
  uint8                     reserved2[22];  /*  2     23 */
  /* Link_CAP_EN, EP1, EP0 etc */
  uint8                     reserved3[4];   /* 24     27 */
  wave_efuse_afe_data_v6_t  afe_data;       /* 28 ... 46 */
  uint8                     reserved4[1];   /* 47        */
};

/* EEPROM source */
typedef enum {
  EEPROM_TYPE_FILE = 0,

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  EEPROM_TYPE_GPIO,      /* GPIO */
#endif

  EEPROM_TYPE_LAST
} mtlk_eeprom_type_e;


#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/* all data read from the EEPROM (except PCI configuration) as a structure */
struct _mtlk_eeprom_data_t {
  mtlk_eeprom_type_e  ee_type;

  BOOL   valid;

  uint8  cal_file_type;
  uint16 vendor_id;
  uint16 device_id;
  uint16 sub_vendor_id;
  uint16 sub_device_id;
  int16  hdr_size;
  int16  cis_size;
  void  *raw_cis;
  mtlk_eeprom_cis_data_t  cis;

  uint8                   afe_size;
  mtlk_efuse_afe_data_t   afe_data;
  void *raw_data;
  uint32 raw_data_size;
} __MTLK_IDATA;

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

/*****************************************************************************
* Local definitions
******************************************************************************/
#define TPC_DEBUG 0

/*****************************************************************************
* Function implementation
******************************************************************************/

mtlk_eeprom_data_t* __MTLK_IFUNC
mtlk_eeprom_create(void)
{
    mtlk_eeprom_data_t    *eeprom_data =
            mtlk_osal_mem_alloc(sizeof(mtlk_eeprom_data_t), MTLK_MEM_TAG_EEPROM);

    if (NULL != eeprom_data)
    {
        memset(eeprom_data, 0, sizeof(mtlk_eeprom_data_t));
    }

    return eeprom_data;
}

void __MTLK_IFUNC
mtlk_eeprom_delete(mtlk_eeprom_data_t *eeprom_data)
{
    if (NULL != eeprom_data)
    {
        mtlk_osal_mem_free(eeprom_data);
    }
}


static mtlk_txmm_clb_action_e __MTLK_IFUNC
mtlk_reload_tpc_cfm_clb(mtlk_handle_t clb_usr_data, mtlk_txmm_data_t* data, mtlk_txmm_clb_reason_e reason)
{
  MTLK_UNREFERENCED_PARAM(clb_usr_data);
  MTLK_UNREFERENCED_PARAM(data);
  MTLK_UNREFERENCED_PARAM(reason);

  return MTLK_TXMM_CLBA_FREE;
}

static int
_mtlk_send_ant_params(mtlk_txmm_msg_t *man_msg, tpcAntParams_t *pTpcAntParams, int ant_number)
{
    int res;

    SLOG1(0, 0, tpcAntParams_t, pTpcAntParams);
    mtlk_dump(2, pTpcAntParams, sizeof(*pTpcAntParams), "dump of TpcAntParams");
    res = mtlk_txmm_msg_send(man_msg, mtlk_reload_tpc_cfm_clb, HANDLE_T(ant_number), TXMM_DEFAULT_TIMEOUT);
    if (res != MTLK_ERR_OK || UMI_OK != pTpcAntParams->Status) {
      ELOG_DD("Set UM_MAN_SET_TPC_ANT_PARAMS_REQ failed, res=%d status=%hhu", res, pTpcAntParams->Status);
      if (UMI_OK != pTpcAntParams->Status)
        res = MTLK_ERR_MAC;
    }

    return res;
}



static int
_mtlk_reload_tpc (mtlk_hw_band_e band, uint8 spectrum_mode, uint8 upper_lower, uint16 channel, mtlk_txmm_t *txmm,
                 mtlk_txmm_msg_t *man_msgs, uint32 nof_man_msgs, mtlk_eeprom_data_t *eeprom, uint8 num_tx_antennas)
{
  unsigned int ant, bw, tpc_freq_cnt;
  mtlk_eeprom_tpc_data_g6_t *tpc_data, *match, *match_lower;
  mtlk_txmm_data_t  *man_entry = NULL;
  tpcAntParams_t    *pTpcAntParams;
  TPC_FREQ          *tpc_list;
  mtlk_txmm_msg_t   *man_msg;
  size_t             tpc_data_size;
  int                res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(NULL != eeprom);
  MTLK_ASSERT(man_msgs != NULL);
  MTLK_ASSERT(nof_man_msgs >= num_tx_antennas);

  MTLK_STATIC_ASSERT(sizeof(tpc_data->tpc_chan_data) <= sizeof(TPC_FREQ));

  if(num_tx_antennas != eeprom->cis.ant_num)
  {
    ELOG_DD("EEPROM contains invalid antenna count: %d. Should be %d.", eeprom->cis.ant_num, num_tx_antennas);
    res = MTLK_ERR_EEPROM;
    goto END;
  }

  /* We need to check data for max possible antennas */
  for(ant = 0; ant < EEPROM_GEN6_MAX_ANT; ant++)
  {

    /* Skip antenna with empty data filled */
    if (NULL == eeprom->cis.tpc_gen6[ant])
        continue;

    man_msg = &man_msgs[ant];
    man_entry = mtlk_txmm_msg_get_empty_data(man_msg, txmm);
    if (man_entry == NULL)
    {
      ELOG_D("No free man slot available to set TPC for antenna %d", ant);
      res = MTLK_ERR_NO_RESOURCES;
      goto END;
    }

    ILOG1_DDD("Set TPC_ANT_%d, band=%d, channel=%d", ant, band, channel);
    man_entry->id = UM_MAN_SET_TPC_ANT_PARAMS_REQ;
    man_entry->payload_size = sizeof(tpcAntParams_t);
    pTpcAntParams = (tpcAntParams_t*) man_entry->payload;

    memset(pTpcAntParams, 0, sizeof(*pTpcAntParams));

    tpc_data_size = eeprom->cis.tpc_data_size;
    pTpcAntParams->ant = ant;
    pTpcAntParams->tpcFreqLen = tpc_data_size;
    tpc_list = pTpcAntParams->tpcParams.tpcFreq;
    tpc_freq_cnt = 0;

    ILOG2_D("tpc_data_size %u", tpc_data_size);

    for(bw = 0; bw < MAXIMUM_BANDWIDTHS_GEN6; bw++)
    {
      tpc_data = eeprom->cis.tpc_gen6[ant];
      match = NULL;
      match_lower = NULL;

      /*
       * Find exact channel match.
       * If no exact match, try closest lower and upper points.
       */
      while(tpc_data)
      {
        if ((tpc_data->band == band) &&
            (tpc_data->bandwidth == bw))
        {
          if(tpc_data->channel == channel)
          {
            match_lower = NULL;
            match = tpc_data;
            goto found_match;
          }
          else if(tpc_data->channel > channel)
          {
            match = tpc_data;
            goto found_match;
          }
          else
          {
            match_lower = tpc_data;
          }
        }
        tpc_data = tpc_data->next;
      }

      found_match:

      if(!(match || match_lower))
      {
        ILOG3_DD("EEPROM has no TPC data for ant %d bw %d", ant, bw);
        continue;   /* skip this bw */
      }

      if(match)
      {
        tpc_list[tpc_freq_cnt] = match->tpc_hdk_data;
        tpc_freq_cnt++;
      }
      if(match_lower)
      {
        tpc_list[tpc_freq_cnt] = match_lower->tpc_hdk_data;
        tpc_freq_cnt++;
      }
    }

    /* At least one tpc_freq for any BW has to be filled */
    /* But this restriction is not related to Gen6 */
    if (0 == tpc_freq_cnt) {
      ILOG2_D("EEPROM has no TPC data for ant %d", ant);
      mtlk_txmm_msg_cancel(man_msg);
      res = MTLK_ERR_OK; /* allowed on Gen6 */
    } else {
      mtlk_dump(2, pTpcAntParams, sizeof(*pTpcAntParams), "pTpcAntParams");
      res = _mtlk_send_ant_params(man_msg, pTpcAntParams, ant);
    }

    if (res != MTLK_ERR_OK) {
      /* Error already logged */
      goto END;
    }
  }

  res = MTLK_ERR_OK;

END:

  return res;
}

int __MTLK_IFUNC
mtlk_reload_tpc (mtlk_hw_band_e band, uint8 spectrum_mode, uint8 upper_lower, uint16 channel, mtlk_txmm_t *txmm,
                 mtlk_txmm_msg_t *man_msgs, uint32 nof_man_msgs, mtlk_eeprom_data_t *eeprom, uint8 num_tx_antennas)
{

  SLOG1(2, 1, mtlk_eeprom_cis_data_t, &eeprom->cis);
  SLOG1(2, 2, mtlk_eeprom_data_t, eeprom);

  switch(eeprom->cis.tpc_valid)
  {
    case TPC_VALID_GEN6:
      return _mtlk_reload_tpc(band, spectrum_mode, upper_lower, channel, txmm,
                              man_msgs, nof_man_msgs, eeprom, num_tx_antennas);
    default:
      return MTLK_ERR_EEPROM;
  }
}

/* FIXCFG80211: remove abilities */
static const mtlk_ability_id_t _eeprom_abilities[] = {
  WAVE_HW_REQ_GET_EEPROM_CFG,
};

/**
  Cleanup EEPROM data

  \param ee_data    Pointer to the structured view of EEPROM [O]

  \return
    MTLK_ERR...
*/
void __MTLK_IFUNC
mtlk_clean_eeprom_data(mtlk_eeprom_data_t *eeprom_data)
{
  MTLK_ASSERT(NULL != eeprom_data);

  mtlk_eeprom_cis_data_clean(&eeprom_data->cis);
  mtlk_osal_mem_free(eeprom_data->raw_data);

  memset(eeprom_data, 0, sizeof(*eeprom_data));
}

/**
  Check CRC field in EEPROM data

  \param raw_eeprom

  \return
    MTLK_ERR...
*/
static int
_eeprom_crc32_validate (mtlk_eeprom_t *raw_eeprom, int raw_cis_offs, int raw_cis_size)
{
  void *raw_cis;
  int res = MTLK_ERR_EEPROM;
  uint16 crc_data_len = 0;
  uint32 crc = 0, eeprom_crc = 0;

  MTLK_ASSERT(raw_eeprom);

  raw_cis = (void *)raw_eeprom + raw_cis_offs;

  res = mtlk_cis_crc_parse(raw_cis, raw_cis_size, &eeprom_crc, &crc_data_len);
  if (MTLK_ERR_OK != res)
    return res;

  ILOG2_DD("eeprom_crc 0x%08X, crc_data_len 0x%02X", eeprom_crc, crc_data_len);

  if (0 == crc_data_len)
  {
    /* No CRC section in the CIS area */
    return MTLK_ERR_OK;
  }

  eeprom_crc = MAC_TO_HOST32(eeprom_crc);

  ILOG2_PD("call mtlk_osal_crc32(0, %p, 0x%02X)", raw_eeprom, raw_cis_offs + crc_data_len);

  crc = mtlk_osal_crc32(~0L, (void*)raw_eeprom, raw_cis_offs + crc_data_len);
  crc ^= ~0L;

  if (crc != eeprom_crc)
  {
    ELOG_DD("Invalid EEPROM Checksum: 0x%08X (0x%08X)",
            crc, eeprom_crc);
    return MTLK_ERR_EEPROM;
  }

  return MTLK_ERR_OK;
}

/*****************************************************************************
**
** NAME         _eeprom_parse
**
** PARAMETERS   eeprom_data           pointer to eeprom data struct to fill in
**              raw_eeprom            pointer to the buffer to parse
**
** RETURNS      MTLK_ERR...
**
** DESCRIPTION  This function called to perform parsing of EEPROM
**
******************************************************************************/
static int
_eeprom_parse (mtlk_eeprom_t *raw_eeprom, mtlk_eeprom_data_t *parsed_eeprom, int eeprom_size, uint32 chip_id)
{
  mtlk_eeprom_v5_t  *eeprom_v5 = &raw_eeprom->eeprom_v5;
  uint8     ver0, ver1;
  int       cis_size = 0, hdr_size = 0, full_size;
  int res = MTLK_ERR_EEPROM;

  MTLK_ASSERT(NULL != parsed_eeprom);
  mtlk_dump(2, raw_eeprom, MIN(128, eeprom_size), "EEPROM");

  SLOG0(2, 1, mtlk_eeprom_t, raw_eeprom);

  /* Verify EEPROM signature */
  if (raw_eeprom->eeprom_executive_signature != HOST_TO_MAC16(MTLK_EEPROM_EXEC_SIGNATURE))
  {
    ELOG_D("Invalid EEPROM Executive Signature: 0x%04X. "
           "Default parameters are used.",
           MAC_TO_HOST16(raw_eeprom->eeprom_executive_signature));
    return MTLK_ERR_EEPROM;
  }

  parsed_eeprom->raw_data = raw_eeprom;
  parsed_eeprom->raw_data_size = eeprom_size;

  /* Parse EEPROM */

  /* Check version number and get CIS data ptr and size */
  /* 1. Check for NEW EEPROM format version 5 and more */
  /* 2. Check for OLD EEPROM format versions 4 */

#define MTLK_EEPROM_VER_5       5
#define MTLK_EEPROM_VER_6       6

  ver0 = 0;
  ver1 = eeprom_v5->cis_area.version1;
  if (ver1 == MTLK_EEPROM_VER_6) {
    parsed_eeprom->cal_file_type = eeprom_v5->config_area.cal_file_type;
  } else {
    goto _ERROR_VER;
  }

  /* All data after header are CIS data */
  hdr_size = (void *)&eeprom_v5->cis_area - (void *)raw_eeprom;
  cis_size = eeprom_size - hdr_size;
  parsed_eeprom->raw_cis  = eeprom_v5->cis_data;

  /* Fill parsed EEPROM data */
  parsed_eeprom->vendor_id      = eeprom_v5->config_area.vendor_id;
  parsed_eeprom->sub_vendor_id  = eeprom_v5->config_area.subsystem_vendor_id;
  parsed_eeprom->device_id      = eeprom_v5->config_area.device_id;
  parsed_eeprom->sub_device_id  = eeprom_v5->config_area.subsystem_id;

  parsed_eeprom->cis.version = MTLK_MAKE_EEPROM_VERSION(ver1, ver0);
  parsed_eeprom->hdr_size = hdr_size;
  parsed_eeprom->cis_size = cis_size;

  full_size = hdr_size + cis_size;

  /* Match chip_id with card device_id */
  {
    uint32 dev_id = MAC_TO_HOST16(parsed_eeprom->device_id);

    ILOG0_DSDDD("EEPROM info: ver 0x%04x, cal_file_type %s, device_id 0x%04X, header size %d, data size %d, ",
                 parsed_eeprom->cis.version,
                 wave_eeprom_cal_file_type_to_string(parsed_eeprom->cal_file_type),
                 dev_id, hdr_size, full_size);

    if (chip_id != dev_id) {
        ELOG_D("Actual chip_id 0x%04X is not match with EEPROM device_id", chip_id);

        return MTLK_ERR_EEPROM;
    }
  }

  if (eeprom_size < hdr_size) {
    ELOG_DD("Full EEPROM size (%d bytes) is smaller than EEPROM header (%d bytes)",
          eeprom_size, hdr_size);

    return MTLK_ERR_EEPROM;
  }

  if (eeprom_size < full_size) {

   ELOG_DD("EEPROM contains invalid data: data size (%d) is bigger than total size (%d)",
       full_size, eeprom_size);

    return MTLK_ERR_EEPROM;
  }

  res = _eeprom_crc32_validate(raw_eeprom, parsed_eeprom->raw_cis - (void *)raw_eeprom, cis_size);
  if (MTLK_ERR_OK != res) {
      return res;
  }

  res = mtlk_cis_data_parse(parsed_eeprom->raw_cis, cis_size,
                            &parsed_eeprom->cis);

  SLOG1(2, 2, mtlk_eeprom_data_t, parsed_eeprom);

  return res;

_ERROR_VER:
    ELOG_DD("Unsupported version of the EEPROM: %u.%u", ver0, ver1);

    return MTLK_ERR_NOT_SUPPORTED;
}


#define _EFUSE_V5_DATA_CHECK    0   /* define to 1 OR remove after integration complete */

static int
_efuse_v6_parse (struct wave_efuse_v6 *efuse, mtlk_eeprom_data_t *parsed_eeprom, int size, uint32 chip_id)
{
    wave_efuse_afe_data_v6_t    *afe_data;
    uint8   byte;

    MTLK_UNREFERENCED_PARAM(chip_id);

    /* Capabilities */
    byte = efuse->capability.byte;
    ILOG1_D("EFUSE capability: 0x%02X", byte);

    /* AFE data */
    afe_data = &efuse->afe_data;
    mtlk_dump(1, afe_data, sizeof(*afe_data), "AFE data");
    if (!afe_data->trim_indicator) {
        WLOG_V("EFUSE AFE trim indicator is not set, skip AFE data");
    } else {
        parsed_eeprom->afe_size = AFE_CALIBRATION_DATA_SIZE_GEN6;
        parsed_eeprom->afe_data.v6 = *afe_data;   /* struct copy */
    }

    return MTLK_ERR_OK;
}

#ifdef EEPROM_DATA_ACCESS
/**
  Add EEPROM abilities into the ability manager.
*/
int __MTLK_IFUNC
mtlk_eeprom_access_init(mtlk_vap_handle_t vap)
{
  struct _mtlk_abmgr_t *abmgr;
  int res;

  abmgr = mtlk_vap_get_abmgr(vap);

  res = mtlk_abmgr_register_ability_set(abmgr, _eeprom_abilities, ARRAY_SIZE(_eeprom_abilities));
  if (MTLK_ERR_OK != res) {
      goto FINISH;
  }

  mtlk_abmgr_enable_ability_set(abmgr, _eeprom_abilities, ARRAY_SIZE(_eeprom_abilities));

FINISH:
  return res;
}

void __MTLK_IFUNC
mtlk_eeprom_access_cleanup(mtlk_vap_handle_t vap)
{
  struct _mtlk_abmgr_t *abmgr;

  abmgr = mtlk_vap_get_abmgr(vap);

  mtlk_abmgr_disable_ability_set(abmgr, _eeprom_abilities, ARRAY_SIZE(_eeprom_abilities));
  mtlk_abmgr_unregister_ability_set(abmgr, _eeprom_abilities, ARRAY_SIZE(_eeprom_abilities));
}

#endif /* EEPROM_DATA_ACCESS */

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
/**
  Read EEPROM data to buffer via GPIO

  \param hw_api       Handle to HW API [I]
  \param buffer       Buffer to read to [O]
  \param buffer_size  Size of available buffer [I]
  \param bytes_read   Number of bytes was read [O]

  \return
    MTLK_ERR...
*/
static int
_eeprom_read_from_gpio(mtlk_hw_api_t *hw_api, void* buffer, uint32 buffer_size, int* bytes_read)
{
  mtlk_ccr_t *ccr = NULL;
  uint32 eeprom_size = 0;
  int res = 0;

  *bytes_read = 0;

  (void)mtlk_hw_get_prop(hw_api, MTLK_HW_PROP_CCR, &ccr, sizeof(&ccr));
  res = eeprom_gpio_init(ccr, &eeprom_size);
  if (MTLK_ERR_OK != res)
  {
    ELOG_V("Unable to initialize access to EEPROM via GPIO");
    return res;
  }

  ILOG1_D("EEPROM via GPIO available (size:%d)", eeprom_size);

  MTLK_ASSERT(buffer_size >= eeprom_size);

  if(buffer_size > eeprom_size)
    buffer_size = eeprom_size;

  res = eeprom_gpio_read(ccr, 0, buffer, buffer_size);
  if (MTLK_ERR_OK != res)
  {
    ELOG_V("Unable to read data from EEPROM via GPIO");
  }
  else
  {
    *bytes_read = buffer_size;

    if (((uint16*)buffer)[0] != HOST_TO_MAC16(MTLK_EEPROM_EXEC_SIGNATURE))
    {
      ELOG_V("Invalid data received from EEPROM");
      res = MTLK_ERR_EEPROM;
    }
  }

  eeprom_gpio_clean(ccr);

  return res;
}
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

/**
  Read EEPROM data to buffer from file

  \param hw_api       Handle to HW API [I]
  \param buffer       Buffer to read to [O]
  \param buffer_size  Size of available buffer [I]
  \param bytes_read   Number of bytes was read [O]

  \return
    MTLK_ERR...
*/
static int
_eeprom_read_from_file(mtlk_hw_api_t *hw_api, char *buffer, uint32 buffer_size, uint32 *bytes_read)

{
  int res = MTLK_ERR_OK;
  mtlk_df_fw_file_buf_t fb;
  char fname[0x20];

  (void)mtlk_hw_get_prop(hw_api, MTLK_HW_EEPROM_NAME, fname, sizeof(fname));

  res = mtlk_hw_load_file(hw_api, fname, &fb);
  if (res != MTLK_ERR_OK)
    return res;

  if (buffer_size < fb.size)
  {
    ELOG_DD("Unexpected size of EEPROM data: "
            "received (%d) more then allowed (%d)",
            fb.size, buffer_size);
    res = MTLK_ERR_EEPROM;
    goto ERROR;
  }

  if (fb.size < sizeof(mtlk_eeprom_t))
  {
    ELOG_D("Invalid size of EEPROM data (%d)", fb.size);
    res = MTLK_ERR_EEPROM;
    goto ERROR;
  }

  if (((uint16*)fb.buffer)[0] != HOST_TO_MAC16(MTLK_EEPROM_EXEC_SIGNATURE))
  {
    ELOG_V("Invalid data received from EEPROM");
    mtlk_dump(0, fb.buffer, MIN(32, fb.size), "EEPROM head");
    res = MTLK_ERR_EEPROM;
    goto ERROR;
  }

  wave_memcpy (buffer, buffer_size, fb.buffer, fb.size);
  *bytes_read = fb.size;

ERROR:
  mtlk_hw_unload_file(hw_api, &fb);

  return res;
}

/**
  Read EFUSE data to buffer

  \param hw_api       Handle to HW API [I]
  \param buffer       Handle to buffer holder [O]
  \param bytes_read   Number of bytes was read [O]

  \return
    MTLK_ERR...
*/

static int
_efuse_read_raw_data(mtlk_hw_api_t *hw_api, void** buffer, int* bytes_read, uint32 chip_id, uint32 hw_type)
{
  mtlk_ccr_t *ccr;
  uint32 efuse_size;

  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(NULL != bytes_read);

  ccr = NULL;
  *bytes_read = 0;

  if (!_mtlk_card_is_asic(hw_type)) {
      WLOG_V("EFUSE is unsupported on FPGA/Emul devices");
      return MTLK_ERR_OK;
  }

  (void)mtlk_hw_get_prop(hw_api, MTLK_HW_PROP_CCR, &ccr, sizeof(&ccr));
  MTLK_ASSERT(NULL != ccr);

  efuse_size = __mtlk_ccr_efuse_get_size(ccr);
  if (0 == efuse_size) {
    ELOG_V("EFUSE is not supported");
    return MTLK_ERR_OK;
  }

  *buffer = mtlk_osal_mem_alloc(efuse_size, MTLK_MEM_TAG_EEPROM);
  if(NULL == *buffer)
  {
    ELOG_V("Failed to allocate memory for EFUSE");
    return MTLK_ERR_NO_MEM;
  }

  *bytes_read = __mtlk_ccr_efuse_read(ccr, *buffer, 0, efuse_size);

  if(0 == *bytes_read)
  {
    mtlk_osal_mem_free(*buffer);
    *buffer = NULL;

    ELOG_V("Failed to read EFUSE");

    return MTLK_ERR_EEPROM;
  }

  ILOG1_D("EFUSE reading finished, %d bytes read", *bytes_read);

  return MTLK_ERR_OK;
}

/* Convert EEPROM type to string */
static const char *
_mtlk_eeprom_type_to_string (mtlk_eeprom_type_e ee_type)
{
  switch (ee_type) {
  case EEPROM_TYPE_FILE:
    return "FILE";

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  case EEPROM_TYPE_GPIO:
    return "GPIO";
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

  default:
    return "UNKNOWN";
  }
}

/**
  Read EEPROM data to buffer

  \param hw_api       Handle to HW API [I]
  \param buffer       Handle to buffer holder [O]
  \param bytes_read   Number of bytes was read [O]

  \return
    MTLK_ERR...
*/
static int
_eeprom_read_raw_data (mtlk_hw_api_t *hw_api, void **buffer, uint32 *bytes_read,
                       mtlk_card_type_t card_type, mtlk_eeprom_type_e ee_type)
{
  int res = 0;
  uint32 eeprom_size = 0;

  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != buffer);
  MTLK_ASSERT(NULL != bytes_read);

  *bytes_read = 0;

  *buffer = mtlk_osal_mem_alloc(MTLK_MAX_EEPROM_SIZE, MTLK_MEM_TAG_EEPROM);
  if(NULL == *buffer)
  {
    ELOG_V("Failed to allocate memory for EEPROM");
    return MTLK_ERR_NO_MEM;
  }

  /* Read EEPROM */
  switch (ee_type) {
  case EEPROM_TYPE_FILE:
    res = _eeprom_read_from_file(hw_api, *buffer, MTLK_MAX_EEPROM_SIZE, bytes_read);
    break;
#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  case EEPROM_TYPE_GPIO:
  /* In 2k eeprom, last 392 bytes is to store signature header bin */
    eeprom_size = MTLK_MAX_EEPROM_SIZE - MTLK_SIGNED_CAL_HEADER_SIZE;
    res = _eeprom_read_from_gpio(hw_api, *buffer, eeprom_size, bytes_read);
    break;
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

  default:
    ELOG_D("Unknown EEPROM type (%d)", ee_type);
    res = MTLK_ERR_EEPROM;
    break;
  }

  if(MTLK_ERR_OK != res)
  {
    mtlk_osal_mem_free(*buffer);
    *buffer = NULL;

    WLOG_S("Failed to read EEPROM (%s)", _mtlk_eeprom_type_to_string(ee_type));
  }
  else
  {
    ILOG1_SD("EEPROM (%s) reading finished, %d bytes read.", _mtlk_eeprom_type_to_string(ee_type), *bytes_read);
  }

  return res;
}

char* __MTLK_IFUNC
mtlk_eeprom_band_to_string(mtlk_hw_band_e band)
{
  switch (band) {
  case MTLK_HW_BAND_5_2_GHZ:
    return "5.2";
  case MTLK_HW_BAND_2_4_GHZ:
    return "2.4";
  case MTLK_HW_BAND_6_GHZ:
    return "6.0";
  case MTLK_HW_BAND_2_4_5_2_GHZ:
    return "dual_2.4_5.2";
  case MTLK_HW_BAND_2_4_6_GHZ:
    return "dual_2.4_6.0";
  case MTLK_HW_BAND_5_2_6_GHZ:
    return "dual_5.2_6.0";
  default:
    return "Unknown";
  }
}

uint32 __MTLK_IFUNC
mtlk_eeprom_get_xtal_value(const mtlk_eeprom_data_t *ee_data)
{
  MTLK_ASSERT(NULL != ee_data);

  if (ee_data->valid)
    return (uint32)MAC_TO_HOST16(ee_data->cis.xtal);

  return (uint32)XTAL_DEFAULT_VALUE;
}

BOOL __MTLK_IFUNC
mtlk_eeprom_is_band_supported(const mtlk_eeprom_data_t *ee_data, mtlk_hw_band_e band)
{
  MTLK_STATIC_ASSERT(MTLK_HW_BAND_2_4_GHZ == UMI_PHY_TYPE_BAND_2_4_GHZ);
  MTLK_STATIC_ASSERT(MTLK_HW_BAND_5_2_GHZ == UMI_PHY_TYPE_BAND_5_2_GHZ);
  MTLK_STATIC_ASSERT(MTLK_HW_BAND_6_GHZ   == UMI_PHY_TYPE_BAND_6_GHZ);
  MTLK_ASSERT(NULL != ee_data);

  if ((band == MTLK_HW_BAND_2_4_5_2_GHZ && ee_data->cis.band24_supported && ee_data->cis.band52_supported) ||
      (band == MTLK_HW_BAND_2_4_GHZ && ee_data->cis.band24_supported) ||
      (band == MTLK_HW_BAND_5_2_GHZ && ee_data->cis.band52_supported) ||
      (band == MTLK_HW_BAND_2_4_6_GHZ && ee_data->cis.band24_supported && ee_data->cis.band6g_supported) ||
      (band == MTLK_HW_BAND_5_2_6_GHZ && ee_data->cis.band52_supported && ee_data->cis.band6g_supported) ||
      (band == MTLK_HW_BAND_6_GHZ && ee_data->cis.band6g_supported))
    return TRUE;

  /* WLOG_D("Band %i NOT supported according to EEPROM", band); */
  return FALSE;
}

void __MTLK_IFUNC
mtlk_get_eeprom_raw_data (mtlk_eeprom_data_t *eeprom_data, const uint8 **raw_data, uint32 *raw_data_size)
{
  MTLK_ASSERT(NULL != eeprom_data);
  MTLK_ASSERT(NULL != raw_data);
  MTLK_ASSERT(NULL != raw_data_size);

  *raw_data = eeprom_data->raw_data;
  *raw_data_size = eeprom_data->raw_data_size;

}

/**
  Read EEPROM data in to the driver friendly structure

  \param hw_api     Handle to HW API [I]
  \param ee_data    Pointer to the structured view of EEPROM [O]

  \return
    Error is handled by "mtlk_eeprom_is_valid" function
*/
int __MTLK_IFUNC
mtlk_eeprom_read_and_parse(mtlk_hw_api_t *hw_api, mtlk_eeprom_data_t *ee_data, uint32 chip_id)
{
  int   result, full_eeprom_size;
  void* raw_eeprom;
  mtlk_eeprom_type_e    ee_type;
  mtlk_card_type_t      card_type = MTLK_CARD_UNKNOWN;
  uint8 eeprom_type_start = 0, eeprom_type_end = EEPROM_TYPE_LAST; /* operation mode default */
  uint8 storage_type = 0;
  BOOL is_gen6_d2 = FALSE;

  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != ee_data);

  /* assume we have no valid EEPROM */
  ee_data->valid = FALSE;

  result = mtlk_hw_get_prop(hw_api, MTLK_HW_PROP_CARD_TYPE, &card_type, sizeof(&card_type));
  if (MTLK_ERR_OK != result)
  {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  is_gen6_d2 = mtlk_hw_type_is_gen6_d2 (hw_api->hw);
  if (is_gen6_d2)
  {
    storage_type = mtlk_hw_get_cal_storage_type(hw_api->hw);
    if (storage_type > 0)
    {
      switch(storage_type)
      {
        case DUT_NV_MEMORY_FLASH:
          eeprom_type_start = EEPROM_TYPE_FILE;
          break;
        case DUT_NV_MEMORY_EEPROM:
          eeprom_type_start = EEPROM_TYPE_GPIO;
          break;
        default:
          ELOG_D("Unsupported storage type : %d",storage_type);
          return MTLK_ERR_NOT_SUPPORTED;
      }
      eeprom_type_end   = eeprom_type_start + 1;
    }
  }

  /* Try to read EEPROM from filesystem, GPIO etc */
  for (ee_type = eeprom_type_start; ee_type < eeprom_type_end; ee_type++) {
  /* Read EEPROM data into new allocated buffer */
    result = _eeprom_read_raw_data(hw_api, &raw_eeprom, &full_eeprom_size, card_type, ee_type);
    if(MTLK_ERR_OK != result) {
      /* Error already logged */
      continue;
    }

    /* Parse EEPROM data and free raw_eeprom buffer */
    result = _eeprom_parse(raw_eeprom, ee_data, full_eeprom_size, chip_id);
    if (MTLK_ERR_OK == result) {
      goto _parse_Ok;
    } else {
      ELOG_SD("EEPROM (%s) parsing failed with code %d", _mtlk_eeprom_type_to_string(ee_type), result);
      mtlk_clean_eeprom_data(ee_data);
      continue;
    }
  }

  ELOG_V("No any valid EEPROM found");
  goto ERROR;

_parse_Ok:
  ee_data->ee_type = ee_type;
  if (!(_chipid_is_gen6_d2_or_gen7(chip_id)))
  {
    /* Read EFUSE data into new allocated buffer */
    result = _efuse_read_raw_data(hw_api, &raw_eeprom, &full_eeprom_size,
                                  chip_id, ee_data->cis.card_id.type /* hw_type */);
    if(MTLK_ERR_OK != result) {
      /* Return error, raw_eeprom already freed */
      goto ERROR;
    }

    /* Parse EFUSE data and free raw_eeprom buffer */
    if (0 != full_eeprom_size) {
      mtlk_dump(1, raw_eeprom, full_eeprom_size, "eFuse");

      result = _efuse_v6_parse(raw_eeprom, ee_data, full_eeprom_size, chip_id);

      mtlk_osal_mem_free(raw_eeprom);
      if(MTLK_ERR_OK != result) {
        goto ERROR;
      }
    }
  }

  /* mark data as valid */
  ee_data->valid = TRUE;

  result = MTLK_ERR_OK;

ERROR:
  return result;
}

void __MTLK_IFUNC mtlk_eeprom_get_cfg(mtlk_eeprom_data_t *eeprom, mtlk_eeprom_data_cfg_t *cfg)
{
  const char *ee_type_str;

  MTLK_ASSERT(NULL != eeprom);
  MTLK_ASSERT(NULL != cfg);

  cfg->eeprom_type_id = eeprom->ee_type; /* EEPROM type: File, GPIO etc */
  ee_type_str = _mtlk_eeprom_type_to_string(eeprom->ee_type);
  wave_strcopy(cfg->eeprom_type_str, ee_type_str, sizeof(cfg->eeprom_type_str)); /* last char is '\0' */

  cfg->eeprom_version = eeprom->cis.version;
  cfg->hdr_size = eeprom->hdr_size;
  wave_memcpy(cfg->mac_address, sizeof(cfg->mac_address), eeprom->cis.card_id.mac_address, MTLK_EEPROM_SN_LEN);
  cfg->type = eeprom->cis.card_id.type;
  cfg->revision = eeprom->cis.card_id.revision;

  cfg->is_asic = _mtlk_card_is_asic(cfg->type);
  if (cfg->is_asic) { /* neither FPGA nor Emulation */
      cfg->is_emul      = FALSE;
      cfg->is_fpga      = FALSE;
      cfg->is_phy_dummy = FALSE;
  } else { /* either FPGA or Emulation */
      cfg->is_emul      = _mtlk_card_is_emul(cfg->revision); /* Emulation */
      cfg->is_fpga      = !cfg->is_emul;
      cfg->is_phy_dummy = _mtlk_card_is_phy_dummy(cfg->revision);
  }

  cfg->cal_file_type = eeprom->cal_file_type;
  cfg->vendor_id = MAC_TO_HOST16(eeprom->vendor_id);
  cfg->device_id = MAC_TO_HOST16(eeprom->device_id);
  cfg->sub_vendor_id = MAC_TO_HOST16(eeprom->sub_vendor_id);
  cfg->sub_device_id = MAC_TO_HOST16(eeprom->sub_device_id);
  wave_memcpy(cfg->sn, sizeof(cfg->sn), eeprom->cis.card_id.sn, MTLK_EEPROM_SN_LEN);
  cfg->production_week = eeprom->cis.card_id.production_week;
  cfg->production_year = eeprom->cis.card_id.production_year;
}

/**
  Read EEPROM data in to the buffer

  \param hw_api Handle to HW API [I]
  \param buf    Pointer to the buffer [O]
  \param len    Size of available buffer [I]

  \return
    MTLK_ERR...
*/
int __MTLK_IFUNC
mtlk_eeprom_get_raw_data(mtlk_hw_api_t *hw_api, mtlk_eeprom_cfg_t *eeprom_cfg)
{
  void *data = NULL;
  int res = MTLK_ERR_OK;
  mtlk_card_type_t card_type = MTLK_CARD_UNKNOWN;

  MTLK_ASSERT(NULL != hw_api);
  MTLK_ASSERT(NULL != eeprom_cfg);

  res = mtlk_hw_get_prop(hw_api, MTLK_HW_PROP_CARD_TYPE, &card_type, sizeof(&card_type));
  if (MTLK_ERR_OK != res)
  {
    return MTLK_ERR_NOT_SUPPORTED;
  }

  res = _eeprom_read_raw_data(hw_api, &data, &eeprom_cfg->eeprom_total_size,
                              card_type,
                              eeprom_cfg->eeprom_data.eeprom_type_id);
  if(MTLK_ERR_OK != res)
  {
    return res;
  }

  if (eeprom_cfg->eeprom_total_size <= sizeof(eeprom_cfg->eeprom_raw_data))
  {
    wave_memcpy(eeprom_cfg->eeprom_raw_data, sizeof(eeprom_cfg->eeprom_raw_data), data, eeprom_cfg->eeprom_total_size);
  }
  else
  {
    res = MTLK_ERR_PARAMS;
  }

  mtlk_osal_mem_free(data);
  return res;
}

 uint8 __MTLK_IFUNC
mtlk_eeprom_get_nic_type(mtlk_eeprom_data_t *eeprom_data)
{
  MTLK_ASSERT(NULL != eeprom_data);
  return eeprom_data->cis.card_id.type;
}

uint8 __MTLK_IFUNC
mtlk_eeprom_get_nic_revision(mtlk_eeprom_data_t *eeprom_data)
{
    MTLK_ASSERT(NULL != eeprom_data);
    return eeprom_data->cis.card_id.revision;
}

uint8 __MTLK_IFUNC
mtlk_eeprom_get_tx_ant_mask(mtlk_eeprom_data_t *eeprom_data)
{
    MTLK_ASSERT(NULL != eeprom_data);
    return eeprom_data->cis.ant_mask;
}

const uint8* __MTLK_IFUNC
mtlk_eeprom_get_nic_mac_addr(mtlk_eeprom_data_t *eeprom_data)
{
    MTLK_ASSERT(NULL != eeprom_data);
    return eeprom_data->cis.card_id.mac_address;
}

uint8 __MTLK_IFUNC
mtlk_eeprom_get_num_antennas(mtlk_eeprom_data_t *eeprom)
{
  MTLK_ASSERT(eeprom);

  return eeprom->cis.ant_num;
}

int __MTLK_IFUNC
mtlk_eeprom_is_valid(const mtlk_eeprom_data_t *ee_data)
{
    MTLK_ASSERT(NULL != ee_data);
    return (ee_data->valid) ? MTLK_ERR_OK : MTLK_ERR_UNKNOWN;
}

uint8 __MTLK_IFUNC
mtlk_eeprom_get_disable_sm_channels(mtlk_eeprom_data_t *eeprom)
{
    MTLK_ASSERT(NULL != eeprom);
    return eeprom->cis.card_id.dev_opt_mask.s.disable_sm_channels;
}

uint8 __MTLK_IFUNC
wave_eeprom_cal_file_type_get (mtlk_eeprom_data_t *eeprom)
{
  MTLK_ASSERT(NULL != eeprom);
  return eeprom->cal_file_type;
}
#ifdef UNUSED_CODE
uint16 __MTLK_IFUNC
mtlk_eeprom_get_vendor_id(mtlk_eeprom_data_t *eeprom)
{
    MTLK_ASSERT(NULL != eeprom);
    return eeprom->vendor_id;
}
#endif /* UNUSED_CODE */
uint16 __MTLK_IFUNC
mtlk_eeprom_get_device_id(mtlk_eeprom_data_t *eeprom)
{
    MTLK_ASSERT(NULL != eeprom);
    return eeprom->device_id;
}

uint16 mtlk_eeprom_get_version(mtlk_eeprom_data_t *eeprom)
{
    MTLK_ASSERT(NULL != eeprom);
    return eeprom->cis.version;
}

int __MTLK_IFUNC
mtlk_eeprom_check_ee_data(mtlk_eeprom_data_t *eeprom, mtlk_txmm_t* txmm_p, BOOL is_ap)
{
  MTLK_ASSERT(NULL != eeprom);

  if (MTLK_ERR_OK == mtlk_eeprom_is_valid(eeprom))
  {
    /* Check EEPROM options mask */
    ILOG1_D("Options mask is 0x%02x", eeprom->cis.card_id.dev_opt_mask.d);

    if (eeprom->cis.card_id.dev_opt_mask.s.ap_disabled && is_ap) {
      ELOG_V("AP functionality is not available on this device");
      goto err_ap_func_is_not_available;
    }

    if (mtlk_eeprom_get_disable_sm_channels(eeprom)) {
      ILOG0_V("DFS (SM-required) channels will not be used");
    }
  }
  return MTLK_ERR_OK;

err_ap_func_is_not_available:
  return MTLK_ERR_UNKNOWN;
}

uint8 *
mtlk_eeprom_get_afe_data(mtlk_eeprom_data_t *eeprom, unsigned *size)
{
    *size = eeprom->afe_size;
    return  eeprom->afe_data.bytes;
}

uint8 *
mtlk_eeprom_get_rfic_data(mtlk_eeprom_data_t *eeprom, unsigned *size)
{
    *size = eeprom->cis.rfic.cis_size;
    return  eeprom->cis.rfic.cis_data;
}

uint8 *
mtlk_eeprom_get_rssi_data (mtlk_eeprom_data_t *eeprom, unsigned *size, mtlk_hw_band_e band)
{
    mtlk_cis_rssi_t *rssi = NULL;

    rssi = mtlk_cis_get_rssi_data(&eeprom->cis, band);
    if (rssi) {
      *size = rssi->cis_size;
      return  rssi->cis_data;
    } else {
      *size = 0;
      return NULL;
    }
}
