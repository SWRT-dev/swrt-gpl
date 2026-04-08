/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mhi_dut.h"
#include "dataex.h"
#include "txmm.h"
#include "mtlk_snprintf.h"
#include "mtlkhal.h"
#include "mtlk_df_user_priv.h"
#include "mtlk_df_priv.h"
#include "mtlk_df.h"
#include "mtlk_df_fw.h"
#include "core.h"
#include "mtlk_core_iface.h"
#include "mtlk_coreui.h"
#include "core_common.h"
#include "eeprom_irbd.h"
#include "fw_recovery.h"

#define LOG_LOCAL_GID   GID_DUT_CORE
#define LOG_LOCAL_FID   4

#define MTLK_DUT_FW_MSG_SEND_TIMEOUT  20000 /* ms */

/* Platform Specific */
int __MTLK_IFUNC    mtlk_psdb_load_to_hw (mtlk_hw_t *hw, mtlk_txmm_t *txmm);

struct _mtlk_dut_handlers_t
{
  mtlk_irbd_handle_t *fw_msg_handle;
  mtlk_irbd_handle_t *progmodel_msg_handle;
  mtlk_irbd_handle_t *add_vap_msg_handle;
  mtlk_irbd_handle_t *remove_vap_msg_handle;
  mtlk_irbd_handle_t *set_bss_msg_handle;
  mtlk_irbd_handle_t *set_wmm_parameters_msg_handle;
  mtlk_irbd_handle_t *stop_traffic_msg_handle;
  mtlk_irbd_handle_t *driver_fw_general_msg_handle;
  mtlk_irbd_handle_t *platform_data_fields_msg_handle;
  mtlk_eeprom_irbd_t *eeprom_irbd;
};

static const mtlk_guid_t _MTLK_IRBE_DUT_FW_CMD        = MTLK_IRB_GUID_DUT_FW_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_PROGMODEL_CMD = MTLK_IRB_GUID_DUT_PROGMODEL_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_START_CMD     = MTLK_IRB_GUID_DUT_START_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_STOP_CMD      = MTLK_IRB_GUID_DUT_STOP_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_ADD_VAP_CMD   = MTLK_IRB_GUID_DUT_ADD_VAP_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_REMOVE_VAP_CMD= MTLK_IRB_GUID_DUT_REMOVE_VAP_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_SET_BSS_CMD   = MTLK_IRB_GUID_DUT_SET_BSS_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD =
                                  MTLK_IRB_GUID_DUT_SET_WMM_PARAMETERS_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD   =
                                  MTLK_IRB_GUID_DUT_STOP_VAP_TRAFFIC_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_DRIVER_FW_GENERAL_CMD   =
                                  MTLK_IRB_GUID_DUT_DRIVER_FW_GENERAL_CMD;
static const mtlk_guid_t _MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD   =
                                  MTLK_IRB_GUID_DUT_PLATFORM_DATA_FIELDS_CMD;

static void __MTLK_IFUNC
_mtlk_dut_core_handle_progmodel_msg (mtlk_irbd_t       *irbd,
                                     mtlk_handle_t      context,
                                     const mtlk_guid_t *evt,
                                     void              *buffer,
                                     uint32            *size)
{
  int res = MTLK_ERR_OK;
  wave_progmodel_data_t     pm_data;
  mtlk_core_firmware_file_t pm_file;
  mtlk_core_t *core = (mtlk_core_t *)context;
  dut_progmodel_t *param = (dut_progmodel_t *)buffer;

  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_PROGMODEL_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  MTLK_ASSERT(param->name != NULL);

  wave_strcopy(pm_file.fname, (char *)param->name, ARRAY_SIZE(pm_file.fname));

  ILOG1_S("name is %s", (char *)param->name);

  res = mtlk_hw_load_file(mtlk_vap_get_hwapi(core->vap_handle), param->name, &pm_file.fcontents);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D ("Failed to read progmodel file contents, error: %d", res);
  }
  else
  {
    pm_data.txmm = mtlk_vap_get_txmm(core->vap_handle);
    pm_data.ff   = &pm_file;
    res = mtlk_hw_set_prop(mtlk_vap_get_hwapi(core->vap_handle), MTLK_HW_PROGMODEL, &pm_data, 0 /* sizeof */);
    if (res != MTLK_ERR_OK) {
      ELOG_D ("Failed to send progmodel contents to FW, error: %d", res);
    }

    mtlk_hw_unload_file(mtlk_vap_get_hwapi(core->vap_handle), &pm_file.fcontents);
  }

  param->status = res;

  ILOG0_SDS("DUT progmodel %s load status %d (%s)", pm_file.fname, res, mtlk_get_error_text(res));
}

static int
_mtlk_dut_core_send_add_vap (mtlk_handle_t context, const void *payload, uint32 size)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  UMI_ADD_VAP *areq;
  mtlk_core_t *nic = (mtlk_core_t *)context;
  dutAddVap_t       *p_dutAddVap = (dutAddVap_t *)payload;

  ILOG0_D("CID-%04x: Start activation", mtlk_vap_get_oid(nic->vap_handle));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send ACTIVATE request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = UM_MAN_ADD_VAP_REQ;
  man_entry->payload_size = sizeof (UMI_ADD_VAP);

  areq = (UMI_ADD_VAP*)(man_entry->payload);
  memset(areq, 0, sizeof(UMI_ADD_VAP));

  wave_pdb_get_mac(
      mtlk_vap_get_param_db(nic->vap_handle), 
      PARAM_DB_CORE_MAC_ADDR, &areq->sBSSID);
  areq->operationMode = OPERATION_MODE_DUT;
  areq->vapId = p_dutAddVap->vapIndex;

  mtlk_dump(2, areq, sizeof(UMI_ADD_VAP), "dump of UMI_ADD_VAP:");
  ILOG0_DYDD("CID-%04x: UMI_ADD_VAP: sBSSID %Y, vapId=%u, operationMode=%u",
          mtlk_vap_get_oid(nic->vap_handle),
          &areq->sBSSID, areq->vapId, areq->operationMode);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->Status) {
    ELOG_DDD("CID-%04x: Activate request failed, result=%d status=%hhu",
              mtlk_vap_get_oid(nic->vap_handle), result, areq->Status);
    if (UMI_OK != areq->Status)
      result = MTLK_ERR_MAC;
    goto CLEANUP;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_add_vap_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core;

  core = (mtlk_core_t *)context;
  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_ADD_VAP_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore AddVap");

  res = _mtlk_dut_core_send_add_vap(context, buffer, *size);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send activate request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}

static int
_mtlk_dut_core_send_remove_vap (mtlk_core_t *nic, uint8 vapIndex)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  UMI_REMOVE_VAP *areq;

  ILOG0_D("CID-%04x: Start activation", mtlk_vap_get_oid(nic->vap_handle));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send ACTIVATE request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = UM_MAN_REMOVE_VAP_REQ;
  man_entry->payload_size = sizeof (UMI_REMOVE_VAP);

  areq = (UMI_REMOVE_VAP*)(man_entry->payload);
  memset(areq, 0, sizeof(UMI_REMOVE_VAP));

  areq->vapId = vapIndex;

  mtlk_dump(2, areq, sizeof(UMI_REMOVE_VAP), "dump of UMI_REMOVE_VAP:");
  ILOG0_DD("CID-%04x: UMI_REMOVE_VAP vapIndex=%d", mtlk_vap_get_oid(nic->vap_handle), areq->vapId);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Cannot send activate request due to TXMM err#%d", mtlk_vap_get_oid(nic->vap_handle), result);
    goto CLEANUP;
  }

  if (areq->u16Status != UMI_OK)
  {
    WLOG_DD("CID-%04x: Activate VAP request failed with code %d", mtlk_vap_get_oid(nic->vap_handle), areq->u16Status);
    result = MTLK_ERR_UNKNOWN;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_remove_vap_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core = (mtlk_core_t *)context;
  dutRemoveVap_t       *p_dutRemoveVap = (dutRemoveVap_t *)buffer;

  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_REMOVE_VAP_CMD) == 0);
  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(buffer != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore RemVap");

  res = _mtlk_dut_core_send_remove_vap(core, p_dutRemoveVap->vapIndex);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send deactivate request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_fw_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *)context;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  mtlk_txmm_t      *txmm;

  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_FW_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  if (sizeof(dutMessage_t) < *size) {
    ELOG_DDD("CID-%04x: DUT FW request of unexpected size %d bytes arrived (expected %d bytes or less)",
             mtlk_vap_get_oid(core->vap_handle), *size, sizeof(dutMessage_t));
    *size = 0;
    return;
  }

  txmm = mtlk_vap_get_txdm(core->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, txmm, NULL);
  if (man_entry)
  {
    man_entry->id           = UM_DBG_DUT_MSG_REQ;
    man_entry->payload_size = *size;
    wave_memcpy(man_entry->payload, txmm->base->cfg.max_payload_size, buffer, *size);
    mtlk_dump(2, buffer, *size, "Tx Req to FW:");

    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_DUT_FW_MSG_SEND_TIMEOUT);

    if (MTLK_ERR_OK == res)
    {
      wave_memcpy(buffer, *size, man_entry->payload, *size);
      mtlk_dump(2, buffer, *size, "Rx From FW:");
    }
    else
    {
      ELOG_DDS("CID-%04x: DUT FW request status %d (%s)", mtlk_vap_get_oid(core->vap_handle), res, mtlk_get_error_text(res));
      memset(buffer, 0xee, *size);
    }

    mtlk_txmm_msg_cleanup(&man_msg);
  }
  else
  {
    ELOG_D("CID-%04x: Failed to send DUT FW request due to lack of management messages", mtlk_vap_get_oid(core->vap_handle));
  }
}

/* SET BSS */
static int
_mtlk_dut_core_send_set_bss (mtlk_handle_t context, const void *payload, uint32 size)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  UMI_SET_BSS *areq;
  mtlk_core_t *nic = (mtlk_core_t *)context;
  UMI_SET_BSS *p_dllMsg = (UMI_SET_BSS *)payload;

  ILOG1_D("CID-%04x: SET_BSS", mtlk_vap_get_oid(nic->vap_handle));
  if (size != sizeof(*p_dllMsg)) {
    WLOG_DD("size != sizeof(*p_dllMsg), %d != %d", size, sizeof(*p_dllMsg));
  }

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send ACTIVATE request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = UM_BSS_SET_BSS_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_SET_BSS*)(man_entry->payload);
  *areq = *p_dllMsg;

  mtlk_dump(2, areq, sizeof(UMI_SET_BSS), "dump of UMI_SET_BSS:");
  ILOG0_D("CID-%04x: UMI_SET_BSS", mtlk_vap_get_oid(nic->vap_handle));

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->Status) {
    ELOG_DDD("CID-%04x: Activate request failed, result=%d status=%hhu",
            mtlk_vap_get_oid(nic->vap_handle), result, areq->Status);
    if (UMI_OK != areq->Status)
      result = MTLK_ERR_MAC;
    goto CLEANUP;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_set_bss_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core;

  core = (mtlk_core_t *)context;
  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_SET_BSS_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore SetBss");

  res = _mtlk_dut_core_send_set_bss(context, buffer, *size);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send activate request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}

/* SET_WMM_PARAMETERS */
static int
_mtlk_dut_core_send_set_wmm_parameters (mtlk_handle_t context, const void *payload, uint32 size)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  UMI_SET_WMM_PARAMETERS *areq;
  mtlk_core_t *nic = (mtlk_core_t *)context;
  UMI_SET_WMM_PARAMETERS *p_dllMsg = (UMI_SET_WMM_PARAMETERS*)payload;

  ILOG1_D("CID-%04x:  SET_WMM_PARAMETERS ", mtlk_vap_get_oid(nic->vap_handle));
  if (size != sizeof(*p_dllMsg)) {
    WLOG_DD("size != sizeof(*p_dllMsg), %d != %d", size, sizeof(*p_dllMsg));
  }

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send  SET_WMM_PARAMETERS request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = UM_MAN_SET_WMM_PARAMETERS_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_SET_WMM_PARAMETERS*)(man_entry->payload);
  *areq = *p_dllMsg;

  mtlk_dump(2, areq, sizeof(UMI_SET_WMM_PARAMETERS), "dump of UMI_SET_WMM_PARAMETERS:");
  ILOG0_DD("CID-%04x: UMI_SET_WMM_PARAMETERS vapId=%u", mtlk_vap_get_oid(nic->vap_handle), areq->vapId);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->u8Status) {
    ELOG_DDD("CID-%04x: SET_WMM_PARAMETERS request failed, result=%d status=%hhu",
            mtlk_vap_get_oid(nic->vap_handle), result, areq->u8Status);
    if (UMI_OK != areq->u8Status)
      result = MTLK_ERR_MAC;
    goto CLEANUP;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_set_wmm_parameters_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core;


  core = (mtlk_core_t *)context;
  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore SetWmmParams");

  res = _mtlk_dut_core_send_set_wmm_parameters(context, buffer, *size);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send  SET_WMM_PARAMETERS request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}

/* STOP_VAP_TRAFFIC */
static int
_mtlk_dut_core_send_stop_vap_traffic (mtlk_handle_t context, const void *payload, uint32 size)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  UMI_STOP_VAP_TRAFFIC *areq;
  mtlk_core_t *nic = (mtlk_core_t *)context;
  UMI_STOP_VAP_TRAFFIC *p_dllMsg = (UMI_STOP_VAP_TRAFFIC*)payload;

  ILOG1_D("CID-%04x:  STOP_VAP_TRAFFIC ", mtlk_vap_get_oid(nic->vap_handle));
  if (size != sizeof(*p_dllMsg)) {
    WLOG_DD("size != sizeof(*p_dllMsg), %d != %d", size, sizeof(*p_dllMsg));
  }

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send  STOP_VAP_TRAFFIC request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = UM_MAN_STOP_VAP_TRAFFIC_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_STOP_VAP_TRAFFIC*)(man_entry->payload);
  *areq = *p_dllMsg;

  mtlk_dump(2, areq, sizeof(UMI_STOP_VAP_TRAFFIC), "dump of UMI_STOP_VAP_TRAFFIC:");
  ILOG0_DD("CID-%04x: UMI_STOP_VAP_TRAFFIC vapId=%u", mtlk_vap_get_oid(nic->vap_handle), areq->vapId);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK  != MAC_TO_HOST16(areq->u16Status)) {
    ELOG_DDD("CID-%04x: STOP_VAP_TRAFFIC failed, result=%i status=%hu", mtlk_vap_get_oid(nic->vap_handle),
      result, MAC_TO_HOST16(areq->u16Status));
    if (UMI_OK != MAC_TO_HOST16(areq->u16Status))
      result = MTLK_ERR_MAC;
    goto CLEANUP;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_stop_vap_traffic_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core;


  core = (mtlk_core_t *)context;
  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore stopVapTraffic");

  res = _mtlk_dut_core_send_stop_vap_traffic(context, buffer, *size);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send  STOP_VAP_TRAFFIC request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}

#define NETWORK_TO_HOST32 HOST_TO_MAC32
/* GENERAL FW REQ */
static int
_mtlk_dut_core_send_driver_fw_general (mtlk_handle_t context, const void *payload, uint32 size)
{
  int result;
  mtlk_txmm_data_t* man_entry=NULL;
  mtlk_txmm_msg_t activate_msg;
  mtlk_txmm_t *txmm;
  mtlk_core_t *nic = (mtlk_core_t *)context;
  dutDriverFwGeneralMsg_t *p_dllMsg = (dutDriverFwGeneralMsg_t *)payload;
  static const int FW_GENERAL_MESSAGE_HEAD_LEN = 4 * sizeof(uint32);

  p_dllMsg->umiReqId =  NETWORK_TO_HOST32(p_dllMsg->umiReqId);
  p_dllMsg->umiLen =  NETWORK_TO_HOST32(p_dllMsg->umiLen);
  ILOG1_DD("CID-%04x: DRIVER_FW_GENERAL, umiReqId(%d)", mtlk_vap_get_oid(nic->vap_handle), p_dllMsg->umiReqId);
  if (size != (FW_GENERAL_MESSAGE_HEAD_LEN + p_dllMsg->umiLen)) {
    ELOG_D("umiReqId is %d", p_dllMsg->umiReqId);
    ELOG_DD("size != sizeof(*p_dllMsg), %d != %d", size, FW_GENERAL_MESSAGE_HEAD_LEN + p_dllMsg->umiLen);
    mtlk_dump(0, p_dllMsg, size, "dump of FW REQ:");
    return MTLK_ERR_PARAMS;
  }

  txmm = mtlk_vap_get_txmm(nic->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, txmm, &result);
  if (man_entry == NULL)
  {
    ELOG_D("CID-%04x: Can't send request to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    return result;
  }

  man_entry->id           = p_dllMsg->umiReqId;
  man_entry->payload_size = p_dllMsg->umiLen;
  wave_memcpy(man_entry->payload, txmm->base->cfg.max_payload_size, p_dllMsg->param, p_dllMsg->umiLen);

  mtlk_dump(2, man_entry->payload, p_dllMsg->umiLen, "dump of FW REQ:");
  ILOG0_D("CID-%04x: DRIVER_FW_GENERAL", mtlk_vap_get_oid(nic->vap_handle));

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Cannot send activate request due to TXMM err#%d", mtlk_vap_get_oid(nic->vap_handle), result);
    goto CLEANUP;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);
  return result;
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_driver_fw_general_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  int res;
  mtlk_core_t *core;

  core = (mtlk_core_t *)context;
  MTLK_ASSERT(core != NULL);

  mtlk_dump(2, buffer, *size, "DUTCore GeneralFWReq");

  res = _mtlk_dut_core_send_driver_fw_general(context, buffer, *size);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send general request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
  }

  return;
}


static int
_mtlk_dut_core_load_platform_data (mtlk_handle_t context)
{
    mtlk_core_t *nic = (mtlk_core_t *)context;
    return mtlk_psdb_load_to_hw (mtlk_vap_get_hw(nic->vap_handle),
                                 mtlk_vap_get_txmm(nic->vap_handle));
}

static void __MTLK_IFUNC
_mtlk_dut_core_handle_platform_data_fields_msg (mtlk_irbd_t       *irbd,
        mtlk_handle_t      context,
        const mtlk_guid_t *evt,
        void              *buffer,
        uint32            *size)
{
    int res;
    mtlk_core_t *core;


    core = (mtlk_core_t *)context;
    MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD) == 0);
    MTLK_ASSERT(core != NULL);

    mtlk_dump(2, buffer, *size, "DUTCore LoadPlatformData");

    res = _mtlk_dut_core_load_platform_data(context);
    if (res != MTLK_ERR_OK) {
      ELOG_DD("CID-%04x: Send load platform data request to DUT core err#%d", mtlk_vap_get_oid(core->vap_handle), res);
    }

    return;
}

MTLK_START_STEPS_LIST_BEGIN(dut_core)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, ALLOC_HANDLERS)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, FW_CMD_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, PROGMODEL_CMD_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_ADD_VAP_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_REMOVE_VAP_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_SET_BSS_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_SET_WMM_PARAMETERS_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_STOP_VAP_TRAFFIC_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_DRIVER_FW_GENERAL_HANDLER)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, CMD_PLATFORM_DATA_FIELDS)
  MTLK_START_STEPS_LIST_ENTRY(dut_core, EEPROM_IRBD)
MTLK_START_INNER_STEPS_BEGIN(dut_core)
MTLK_START_STEPS_LIST_END(dut_core);

static void
_mtlk_dut_core_stop (mtlk_core_t *core)
{

  core->dut_core.isStarted = FALSE;

  ILOG0_D("CID-%04x: DUT Core stopped.", mtlk_vap_get_oid(core->vap_handle));
}

static void
_mtlk_dut_core_irbd_unregister (mtlk_core_t *core)
{
  mtlk_vap_handle_t vap_handle;
  mtlk_dut_core_t *dut_core;

  MTLK_ASSERT(core != NULL);

  vap_handle = core->vap_handle;
  dut_core = &core->dut_core;

  ILOG0_D("CID-%04x: Unregister DUT IRBD...", mtlk_vap_get_oid(vap_handle));

  MTLK_STOP_BEGIN(dut_core, MTLK_OBJ_PTR(dut_core))
    MTLK_STOP_STEP(dut_core, EEPROM_IRBD, MTLK_OBJ_PTR(dut_core),
                   mtlk_eeprom_irbd_unregister, (dut_core->dut_handlers->eeprom_irbd));
    MTLK_STOP_STEP(dut_core, CMD_PLATFORM_DATA_FIELDS, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->platform_data_fields_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_DRIVER_FW_GENERAL_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->driver_fw_general_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_STOP_VAP_TRAFFIC_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->stop_traffic_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_SET_WMM_PARAMETERS_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->set_wmm_parameters_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_SET_BSS_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->set_bss_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_REMOVE_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->remove_vap_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_ADD_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->add_vap_msg_handle));
    MTLK_STOP_STEP(dut_core, PROGMODEL_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->fw_msg_handle));
    MTLK_STOP_STEP(dut_core, FW_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (mtlk_vap_get_irbd(vap_handle), dut_core->dut_handlers->progmodel_msg_handle));
    MTLK_STOP_STEP(dut_core, ALLOC_HANDLERS, MTLK_OBJ_PTR(dut_core),
                   mtlk_osal_mem_free, (dut_core->dut_handlers));
    dut_core->dut_handlers = NULL;
  MTLK_STOP_END(dut_core, MTLK_OBJ_PTR(dut_core))
}

static int
_mtlk_dut_core_start (mtlk_core_t *core)
{
  MTLK_ASSERT(core != NULL);

  core->dut_core.isStarted = TRUE;
  ILOG0_D("CID-%04x: DUT Core started...", mtlk_vap_get_oid(core->vap_handle));

  return MTLK_ERR_OK;
}

static int
_mtlk_dut_core_irbd_register (mtlk_core_t *core)
{
  mtlk_vap_handle_t vap_handle;
  mtlk_dut_core_t *dut_core;

  MTLK_ASSERT(core != NULL);

  vap_handle = core->vap_handle;
  dut_core = &core->dut_core;

  ILOG0_D("CID-%04x: Register DUT IRBD...", mtlk_vap_get_oid(vap_handle));

  MTLK_START_TRY(dut_core, MTLK_OBJ_PTR(dut_core))
    MTLK_START_STEP_EX(dut_core, ALLOC_HANDLERS, MTLK_OBJ_PTR(dut_core),
                       mtlk_osal_mem_alloc,
                       (sizeof(mtlk_dut_handlers_t), MTLK_MEM_TAG_DUT_CORE),
                       dut_core->dut_handlers, dut_core->dut_handlers != NULL,
                       MTLK_ERR_NO_MEM);
    memset(dut_core->dut_handlers, 0, sizeof(mtlk_dut_handlers_t));
    MTLK_START_STEP_EX(dut_core, FW_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_FW_CMD, 1, _mtlk_dut_core_handle_fw_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->fw_msg_handle, dut_core->dut_handlers->fw_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, PROGMODEL_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_PROGMODEL_CMD, 1, _mtlk_dut_core_handle_progmodel_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->progmodel_msg_handle, dut_core->dut_handlers->progmodel_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_ADD_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_ADD_VAP_CMD, 1, _mtlk_dut_core_handle_add_vap_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->add_vap_msg_handle, dut_core->dut_handlers->add_vap_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_REMOVE_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_REMOVE_VAP_CMD, 1, _mtlk_dut_core_handle_remove_vap_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->remove_vap_msg_handle, dut_core->dut_handlers->remove_vap_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_SET_BSS_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_SET_BSS_CMD, 1, _mtlk_dut_core_handle_set_bss_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->set_bss_msg_handle, dut_core->dut_handlers->set_bss_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_SET_WMM_PARAMETERS_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD, 1, _mtlk_dut_core_handle_set_wmm_parameters_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->set_wmm_parameters_msg_handle, dut_core->dut_handlers->set_wmm_parameters_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_STOP_VAP_TRAFFIC_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD, 1, _mtlk_dut_core_handle_stop_vap_traffic_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->stop_traffic_msg_handle, dut_core->dut_handlers->stop_traffic_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_DRIVER_FW_GENERAL_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_DRIVER_FW_GENERAL_CMD, 1, _mtlk_dut_core_handle_driver_fw_general_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->driver_fw_general_msg_handle, dut_core->dut_handlers->driver_fw_general_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_PLATFORM_DATA_FIELDS, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (mtlk_vap_get_irbd(vap_handle),
                       &_MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD, 1, _mtlk_dut_core_handle_platform_data_fields_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->platform_data_fields_msg_handle, dut_core->dut_handlers->platform_data_fields_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, EEPROM_IRBD, MTLK_OBJ_PTR(dut_core),
                       mtlk_eeprom_irbd_register, (vap_handle),
                       dut_core->dut_handlers->eeprom_irbd, dut_core->dut_handlers->eeprom_irbd != NULL,
                       MTLK_ERR_NO_RESOURCES);
  MTLK_START_FINALLY(dut_core, MTLK_OBJ_PTR(dut_core))
  MTLK_START_RETURN(dut_core, MTLK_OBJ_PTR(dut_core), _mtlk_dut_core_irbd_unregister, (core))
}

static void
core_dut_df_stop(mtlk_df_t *df)
{
  int res;
  mtlk_clpb_t *clpb = NULL;

  MTLK_ASSERT(NULL != df);

  do {
    res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_DEACTIVATE, &clpb, NULL, 0);
    res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_DEACTIVATE, TRUE);
    mtlk_osal_msleep(100);
  } while ((MTLK_ERR_OK != res) && (MTLK_ERR_FW != res));
}

static void __MTLK_IFUNC
_mtlk_dut_core_prepare (mtlk_irbd_t       *irbd,
                        mtlk_handle_t      context,
                        const mtlk_guid_t *evt,
                        void              *buffer,
                        uint32            *size)
{
  int res = MTLK_ERR_OK;
  BOOL run_dut = TRUE;
  int max_vaps_count, vap_index;
  mtlk_core_t *core = (mtlk_core_t *)context;
  mtlk_vap_manager_t *vap_mgr;
  mtlk_core_t *cur_core;
  uint16      cur_oid;
  mtlk_vap_handle_t vap_handle;
  mtlk_df_t   *cur_df;

  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_START_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  if (!core->dut_core.isStarted) {
    if (mtlk_core_is_stopping(core)) {
      ELOG_D("CID-%04x: Cannot activate DUT mode during interface DOWNING", mtlk_vap_get_oid(core->vap_handle));
      return;
    }

    vap_mgr = mtlk_vap_get_manager(core->vap_handle);
    max_vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
    for (vap_index = 0; vap_index < max_vaps_count; vap_index++) {
      res = mtlk_vap_manager_get_vap_handle(vap_mgr, vap_index, &vap_handle);
      ILOG0_DD("DUT get vap(%d) handler, res=%d...", vap_index, res);
      if (MTLK_ERR_OK == res) {
        cur_core = mtlk_vap_get_core(vap_handle);
        cur_oid = mtlk_vap_get_oid(vap_handle);
        cur_df = mtlk_vap_get_df(vap_handle);

        ILOG0_DD("DUT get vap(%d) handler, stopped=%d...",
            vap_index, cur_core->is_stopped);
        if (!cur_core->is_stopped) {
          /* FIXME: Feng Lin
           * If 'Recovery None' is supported,
           * we don't need remove existing VAPs.
           */
          ILOG0_D("CID-%04x: DUT Stop VAP...", cur_oid);
          core_dut_df_stop(cur_df);

          res = mtlk_df_down(cur_df);
          if (res != MTLK_ERR_OK) {
            ELOG_D("CID-%04x: Cannot interface DOWN for DUT mode", cur_oid);
            run_dut = FALSE;
            break;
          }
        }
      }
    }
    if (run_dut) {
      (void)_mtlk_dut_core_start(core);
    }
  }
}

static void __MTLK_IFUNC
_mtlk_dut_core_unprepare (mtlk_irbd_t       *irbd,
                          mtlk_handle_t      context,
                          const mtlk_guid_t *evt,
                          void              *buffer,
                          uint32            *size)
{
  mtlk_core_t *core = (mtlk_core_t *)context;

  MTLK_ASSERT(mtlk_guid_compare(evt, &_MTLK_IRBE_DUT_STOP_CMD) == 0);
  MTLK_ASSERT(core != NULL);

  if (core->dut_core.isStarted) {
    _mtlk_dut_core_stop(core);
  }
}

int __MTLK_IFUNC
mtlk_dut_core_register (mtlk_core_t *core)
{
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(core != NULL);

  core->dut_core.isStarted = FALSE;

  if (!mtlk_vap_is_master(core->vap_handle)) {
    return MTLK_ERR_OK;
  }

  res = _mtlk_dut_core_irbd_register(core);
  if (MTLK_ERR_OK != res) {
    return res;
  }

  core->dut_core.dut_start_handle = mtlk_irbd_register(mtlk_vap_get_irbd(core->vap_handle),
                                                       &_MTLK_IRBE_DUT_START_CMD,
                                                       1,
                                                       _mtlk_dut_core_prepare,
                                                       HANDLE_T(core));
  if (NULL == core->dut_core.dut_start_handle) {
    return MTLK_ERR_NO_RESOURCES;
  }

  core->dut_core.dut_stop_handle = mtlk_irbd_register(mtlk_vap_get_irbd(core->vap_handle),
                                                      &_MTLK_IRBE_DUT_STOP_CMD,
                                                      1,
                                                      _mtlk_dut_core_unprepare,
                                                      HANDLE_T(core));
  if (NULL == core->dut_core.dut_stop_handle) {
    return MTLK_ERR_NO_RESOURCES;
  }

  if (mtlk_vap_is_dut(core->vap_handle)) {
    ILOG0_V("Start in DUT mode!");
    res = _mtlk_dut_core_start(core);
  }

  return res;
}

void __MTLK_IFUNC
mtlk_dut_core_unregister (mtlk_core_t *core)
{
  MTLK_ASSERT(core != NULL);

  if (!mtlk_vap_is_master(core->vap_handle)) {
    return;
  }

  _mtlk_dut_core_irbd_unregister(core);

  if (NULL != core->dut_core.dut_start_handle) {
    mtlk_irbd_unregister(mtlk_vap_get_irbd(core->vap_handle),
                         core->dut_core.dut_start_handle);
  }
  if (NULL != core->dut_core.dut_stop_handle) {
    mtlk_irbd_unregister(mtlk_vap_get_irbd(core->vap_handle),
                         core->dut_core.dut_stop_handle);
  }
}
