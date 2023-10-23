/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"

#include "progmodel.h"
#include "mtlkhal.h"
#include "mtlkparams.h"
#include "core.h"
#include "mhi_umi.h"
#include "mtlkaux.h"
#include "mtlk_df_fw.h"
#include "mtlk_card_selector.h"
#include "wave_radio.h"

#define LOG_LOCAL_GID   GID_PROGMODEL
#define LOG_LOCAL_FID   1

#define WAVE_PRGMDL_LOAD_START_TIMEOUT 10000 /* ms */

#define GEN6_PROGMODEL_COUNT     3
/* make sure this is maximum progmodels count */
#define PROGMODEL_COUNT_MAX      GEN6_PROGMODEL_COUNT

typedef struct _wave_prgmdl_parse_t
{
    char const *chip_name;
    uint16 dev_id;
    uint8  hw_type;
    uint8  hw_revision;
    uint8  chip_revision;
    uint8  freq; /* HW band */
    uint8  count;
} wave_prgmdl_parse_t;

typedef struct
{
    mtlk_core_firmware_file_t ff[PROGMODEL_COUNT_MAX];
    mtlk_card_type_t card_type;
    wave_prgmdl_parse_t prgmdl_params;
    BOOL is_loaded_from_os;
    wave_radio_t *radio;
    mtlk_txmm_t *txmm;
}  wave_progmodel_t;

static void _wave_progmodel_free_os(wave_progmodel_t *fw);
static void _wave_progmodel_get_fname(wave_progmodel_t *fw);

static int
_wave_progmodel_prepare_loading_to_hw (const wave_progmodel_t *fw)
{
  int               res = MTLK_ERR_UNKNOWN;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry = NULL;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg,
                                                 fw->txmm,
                                                 &res);
  if (!man_entry) {
    ELOG_D("Can't prepare progmodel loading due to lack of MM (err=%d)", res);
    goto FINISH;
  }

   man_entry->id = UM_MAN_DOWNLOAD_PROG_MODEL_PERMISSION_REQ;
   man_entry->payload_size = 0;
   res = mtlk_txmm_msg_send_blocked(&man_msg, WAVE_PRGMDL_LOAD_START_TIMEOUT);
   if (res != MTLK_ERR_OK) {
     ELOG_D("Can't prepare FW for downloading programming model, timed-out. Err#%d", res);
     goto FINISH;
   }

   res = MTLK_ERR_OK;

FINISH:
  if (man_entry) {
    mtlk_txmm_msg_cleanup(&man_msg);
  }

  return res;
}

static void
_wave_progmodel_cleanup (wave_progmodel_t *fw)
{
  if (fw->is_loaded_from_os)
    _wave_progmodel_free_os(fw);

}

static mtlk_eeprom_data_t*
_wave_progmodel_eeprom_get(mtlk_hw_api_t *hwapi)
{
  mtlk_eeprom_data_t *ee_data = NULL;
  (void)mtlk_hw_get_prop(hwapi, MTLK_HW_PROP_EEPROM_DATA, &ee_data, sizeof(&ee_data));
  return ee_data;
}

static int
_wave_progmodel_init (mtlk_txmm_t *txmm, wave_progmodel_t *fw, wave_radio_t *radio, int freq)
{
  mtlk_hw_api_t   *hwapi;
  mtlk_card_type_t card_type = MTLK_CARD_UNKNOWN;

  MTLK_ASSERT(fw != NULL);
  MTLK_ASSERT(radio != NULL);
  MTLK_ASSERT(txmm != NULL);

  fw->prgmdl_params.freq = freq;
  fw->radio = radio;
  fw->txmm = txmm;

  hwapi = txmm->hw_api;
  if (MTLK_ERR_OK != mtlk_hw_get_prop(hwapi, MTLK_HW_PROP_CHIP_NAME,
                         &fw->prgmdl_params.chip_name, sizeof(&fw->prgmdl_params.chip_name)))
  {
    goto ERROR;
  }

  MTLK_ASSERT(NULL != fw->prgmdl_params.chip_name);

  if (MTLK_ERR_OK != mtlk_eeprom_is_valid(_wave_progmodel_eeprom_get(hwapi)))
  {
    goto ERROR;
  }
  else
  {
    mtlk_eeprom_data_t *eeprom_data = _wave_progmodel_eeprom_get(hwapi);

    fw->prgmdl_params.dev_id      = MAC_TO_HOST16(mtlk_eeprom_get_device_id(eeprom_data));
    fw->prgmdl_params.hw_type     = mtlk_eeprom_get_nic_type(eeprom_data);
    fw->prgmdl_params.hw_revision = mtlk_eeprom_get_nic_revision(eeprom_data);
  }

  if (MTLK_ERR_OK != mtlk_hw_get_prop(hwapi, MTLK_HW_PROP_CHIP_REVISION,
                         &fw->prgmdl_params.chip_revision, sizeof(fw->prgmdl_params.chip_revision)))
  {
    goto ERROR;
  }


  if (MTLK_ERR_OK != mtlk_hw_get_prop(hwapi, MTLK_HW_PROP_CARD_TYPE, &card_type, sizeof(&card_type)))
  {
    goto ERROR;
  }

  fw->card_type = card_type;
  CARD_SELECTOR_START(card_type);
    IF_CARD_G6_OR_G7 ( fw->prgmdl_params.count = GEN6_PROGMODEL_COUNT; );
  CARD_SELECTOR_END();

  return MTLK_ERR_OK;

ERROR:
  return MTLK_ERR_EEPROM;
}

static wave_progmodel_t *
_wave_progmodel_create (mtlk_txmm_t *txmm, wave_radio_t *radio, int freq)
{
  wave_progmodel_t *fw = mtlk_osal_mem_alloc(sizeof(wave_progmodel_t), MTLK_MEM_TAG_PROGMODEL);

  MTLK_ASSERT(freq == MTLK_HW_BAND_6_GHZ || freq == MTLK_HW_BAND_5_2_GHZ || freq == MTLK_HW_BAND_2_4_GHZ);

  if (fw != NULL)
  {
    memset(fw, 0, sizeof(*fw));

    if (_wave_progmodel_init(txmm, fw, radio, freq) == MTLK_ERR_OK)
    {
      ILOG3_V("Progmodel initialized");
    }
    else
    {
      ELOG_V("Can't initialize Progmodel object");
      mtlk_osal_mem_free(fw);
      fw = NULL;
    }
  }
  else
  {
    ELOG_V("Unable to create progmodel object");
  }

  return fw;
}

static void
_wave_progmodel_free_os (wave_progmodel_t *fw)
{
  int i;

  MTLK_ASSERT(fw->is_loaded_from_os); /* "free already freed" attempts should be eliminated */

  for (i = 0; i < fw->prgmdl_params.count; i++)
    mtlk_hw_unload_file(fw->txmm->hw_api, &fw->ff[i].fcontents);

  fw->is_loaded_from_os = FALSE;
}

static int
_wave_progmodel_load_from_os (wave_progmodel_t *fw)
{
  int i, t;
  int res;

  MTLK_ASSERT(!fw->is_loaded_from_os); /* double load attempts should be eliminated */

  /* compose file names to download */
  _wave_progmodel_get_fname(fw);

  for (i = 0; i < fw->prgmdl_params.count; i++) {
    ILOG1_S("Requesting firmware %s", fw->ff[i].fname);

    res = mtlk_hw_load_file(fw->txmm->hw_api, fw->ff[i].fname, &fw->ff[i].fcontents);
    if (res != MTLK_ERR_OK) {
      ELOG_SD("ERROR: Progmodel (%s) loading failed. Err#%d", fw->ff[i].fname, res);
      res = MTLK_ERR_UNKNOWN;
      goto ERROR;
    }

    fw->ff[i].update_version = (i == MTLK_FILE_TYPE_HW) ? TRUE : FALSE;
  }

  fw->is_loaded_from_os = TRUE;

  return MTLK_ERR_OK;

ERROR:
  /* free what was loaded previously */
  for (t = 0; t < i; t++)
    mtlk_hw_unload_file(fw->txmm->hw_api, &fw->ff[t].fcontents);

  return res;
}


static __INLINE BOOL
__wave_progmodel_is_loaded (const wave_progmodel_t *fw)
{
  uint8 last_pm_freq;
  MTLK_ASSERT(NULL != fw);
  last_pm_freq = wave_radio_last_pm_freq_get(fw->radio);
  ILOG4_PDD("Comparing radio's at %p last_pm_freq %d with %d",
            fw->radio, last_pm_freq, fw->prgmdl_params.freq);
  return fw->prgmdl_params.freq == last_pm_freq;
}

static int
_wave_progmodel_load_to_hw (const wave_progmodel_t *fw)
{
  wave_progmodel_data_t     pm_data;
  int i;
  int res = _wave_progmodel_prepare_loading_to_hw(fw);

  if (res != MTLK_ERR_OK) {
    goto FINISH;
  }

  pm_data.txmm = fw->txmm;  /* txmm is required for loading per radio */

  for (i = 0; i < fw->prgmdl_params.count; i++) {
    /* we pray that HW won't truncate our file buffer, therefore we cast away constness */
    pm_data.ff = (mtlk_core_firmware_file_t *)&fw->ff[i];
    res = mtlk_hw_set_prop(fw->txmm->hw_api, MTLK_HW_PROGMODEL, &pm_data, 0);
    if (res != MTLK_ERR_OK) {
      ELOG_SD("ERROR: Progmodel (%s) loading failed. Err#%d", fw->ff[i].fname, res);
      goto FINISH;
    }
  }

  ILOG4_PD("Saving in radio at %p last_pm_freq %d",
            fw->radio, fw->prgmdl_params.freq);
  wave_radio_last_pm_freq_set(fw->radio, fw->prgmdl_params.freq);

FINISH:
  return res;
}

static void
_wave_progmodel_delete (wave_progmodel_t *fw)
{
  MTLK_ASSERT(fw != NULL);

  _wave_progmodel_cleanup(fw);
  mtlk_osal_mem_free(fw);
}

/* Progmodel loading */

int __MTLK_IFUNC
wave_progmodel_load(mtlk_txmm_t *txmm, wave_radio_t *radio, int freq)
{
  wave_progmodel_t *fw;
  int res;

  MTLK_ASSERT(radio != NULL);

  /* Just return OK if radio has dummy phy */
  if (wave_radio_is_phy_dummy(radio)) {
    ILOG0_V("Progmodel is not needed for dummy phy");
    return MTLK_ERR_OK;
  }

  fw = _wave_progmodel_create(txmm, radio, freq);
  if (fw == NULL) {
    ELOG_V("Unable to create progmodel");
    return MTLK_ERR_NO_MEM;
  }

  /* Check if we need to load progmodels */
  if (__wave_progmodel_is_loaded(fw)) {
    ILOG2_D("Progmodel for freq %d already loaded", freq);
    res = MTLK_ERR_OK;
    goto FINISH;
  }

  res = _wave_progmodel_load_from_os(fw);
  if (res != MTLK_ERR_OK) {
    ELOG_D("ERROR: Progmodel load from OS failed. Err#%d", res);
    goto FINISH;
  }

  res = _wave_progmodel_load_to_hw(fw);
  if (res != MTLK_ERR_OK)
    ELOG_D("ERROR: Progmodel load to HW failed. Err#%d", res);

FINISH:
  _wave_progmodel_delete(fw);

  return res;
}

#define PM_SUFFIX_NONE                ""
#define PM_SUFFIX_WRX500              "A"
#define PM_SUFFIX_WRX514              "BG_wrx5xx"
#define PM_SUFFIX_WRX300              "BG"

static void
_wave_progmodel_compose_ff_data (wave_progmodel_t *fw, int num, mtlk_file_type_e ftype,
                                 char *suffix1, char *suffix2)
{
  fw->ff[num].ftype = ftype;
  mtlk_snprintf(fw->ff[num].fname, sizeof(fw->ff[0].fname), "ProgModel_%s_%s%s.bin",
                fw->prgmdl_params.chip_name, suffix1, suffix2);
}

static void
_wave_progmodel_get_fname (wave_progmodel_t *fw)
{
  wave_prgmdl_parse_t *param = &fw->prgmdl_params;
  char *fpga_suffix;

  fpga_suffix = _mtlk_card_is_asic(param->hw_type) ? "" : "_fpga";

  CARD_SELECTOR_START(fw->card_type);
  IF_CARD_G6_OR_G7 (
    /* file 0: common progmodel */
    _wave_progmodel_compose_ff_data(fw, 0, MTLK_FILE_TYPE_HW, "Common", fpga_suffix);
    /* file 1: TX antenna dependent */
    _wave_progmodel_compose_ff_data(fw, 1, MTLK_FILE_TYPE_ANT_TX, "Ant_Tx", fpga_suffix);
    /* file 2: RX antenna dependent */
    _wave_progmodel_compose_ff_data(fw, 2, MTLK_FILE_TYPE_ANT_RX, "Ant_Rx", fpga_suffix);
  );
  CARD_SELECTOR_END();
}
