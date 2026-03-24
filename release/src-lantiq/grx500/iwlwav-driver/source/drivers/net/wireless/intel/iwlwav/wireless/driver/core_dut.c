/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*! \file   core_dut.c
 *  \brief  DUT module core and DUT API handlers
 */

#include "mtlkinc.h"
#include "mtlkirbd.h"
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

/******************************************************************************/
/*! \brief      Handle DUT message PROGMODEL_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_PROGMODEL_CMD
 *  \note       value pointed to by \a size must be in range [1...sizeof(dut_progmodel_t)]
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_progmodel_msg (mtlk_irbd_t       *irbd,
                                     mtlk_handle_t      context,
                                     const mtlk_guid_t *evt,
                                     void              *buffer,
                                     uint32            *size)
{
  mtlk_error_t              res = MTLK_ERR_OK;
  wave_progmodel_data_t     pm_data;
  mtlk_core_firmware_file_t pm_file;
  mtlk_core_t              *core = (mtlk_core_t *)context;
  dut_progmodel_t          *param = (dut_progmodel_t *)buffer;
  unsigned                  oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_PROGMODEL_CMD);

  WAVE_CHECK_SIZE_MAX_VOID(*size, sizeof(dut_progmodel_t));

  wave_strcopy(pm_file.fname, param->name, ARRAY_SIZE(pm_file.fname));
  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG0_DS("CID-%04x: DUT Progmodel name is %s", oid, pm_file.fname);

  res = mtlk_hw_load_file(mtlk_vap_get_hwapi(core->vap_handle), pm_file.fname, &pm_file.fcontents);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Failed to read progmodel file contents, error %d", res);
  } else {
    pm_data.txmm = mtlk_vap_get_txmm(core->vap_handle);
    pm_data.ff   = &pm_file;
    res = mtlk_hw_set_prop(mtlk_vap_get_hwapi(core->vap_handle), MTLK_HW_PROGMODEL, &pm_data, 0 /* sizeof */);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Failed to send progmodel contents to FW, error %d", res);
    }

    mtlk_hw_unload_file(mtlk_vap_get_hwapi(core->vap_handle), &pm_file.fcontents);
  }

  param->status = res;

  ILOG0_SDS("DUT progmodel %s load status %d (%s)", pm_file.fname, res, mtlk_get_error_text(res));
}

/******************************************************************************/
/*! \brief      Handle DUT message ADD_VAP_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_ADD_VAP_CMD
 *  \note       value pointed to by \a size must be equal to sizeof(dutAddVap_t)
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_add_vap_msg (
                              mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t      result;
  mtlk_txmm_data_t *man_entry = NULL;
  mtlk_txmm_msg_t   activate_msg;
  UMI_ADD_VAP      *areq;
  dutAddVap_t      *p_dutAddVap = (dutAddVap_t *)buffer;
  uint8             vap_id;
  mtlk_core_t      *nic = (mtlk_core_t *)context;
  unsigned          oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_ADD_VAP_CMD);
  WAVE_CHECK_SIZE_EXP_VOID(*size, sizeof(*p_dutAddVap));

  vap_id = p_dutAddVap->vapIndex;
  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG0_DD("CID-%04x: DUTCore AddVap vapId %u", oid, vap_id);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send ADD_VAP request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = UM_MAN_ADD_VAP_REQ;
  man_entry->payload_size = sizeof(UMI_ADD_VAP);

  areq = (UMI_ADD_VAP*)(man_entry->payload);
  wave_pdb_get_mac(mtlk_vap_get_param_db(nic->vap_handle), PARAM_DB_CORE_MAC_ADDR, &areq->sBSSID);
  areq->vapId = vap_id;
  areq->operationMode = OPERATION_MODE_DUT;

  mtlk_dump(2, areq, sizeof(UMI_ADD_VAP), "dump of UMI_ADD_VAP:");
  ILOG0_DYDD("CID-%04x: UMI_ADD_VAP: sBSSID %Y, vapId=%u, operationMode=%u",
             oid, &areq->sBSSID, areq->vapId, areq->operationMode);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->Status) {
    ELOG_DDD("CID-%04x: ADD_VAP request failed, result=%d status=%hhu", oid, result, areq->Status);
    if (UMI_OK != areq->Status)
      result = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send ADD_VAP request to DUT core err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message REMOVE_VAP_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_REMOVE_VAP_CMD
 *  \note       value pointed to by \a size must be equal to sizeof(dutRemoveVap_t)
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_remove_vap_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t      result;
  mtlk_core_t      *nic = (mtlk_core_t *)context;
  unsigned          oid;
  dutRemoveVap_t   *p_dutRemoveVap = (dutRemoveVap_t *)buffer;
  mtlk_txmm_data_t *man_entry = NULL;
  mtlk_txmm_msg_t   activate_msg;
  UMI_REMOVE_VAP   *areq;
  uint8             vap_id;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_REMOVE_VAP_CMD);
  WAVE_CHECK_SIZE_EXP_VOID(*size, sizeof(*p_dutRemoveVap));

  vap_id = p_dutRemoveVap->vapIndex;
  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG0_DD("CID-%04x: DUTCore RemVap vapId %u", oid, vap_id);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send REMOVE_VAP request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = UM_MAN_REMOVE_VAP_REQ;
  man_entry->payload_size = sizeof (UMI_REMOVE_VAP);

  areq = (UMI_REMOVE_VAP*)(man_entry->payload);
  areq->vapId = vap_id;

  mtlk_dump(2, areq, sizeof(UMI_REMOVE_VAP), "dump of UMI_REMOVE_VAP:");

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Cannot send REMOVE_VAP request due to TXMM err#%d", oid, result);
    goto CLEANUP;
  }

  if (MAC_TO_HOST16(areq->u16Status) != UMI_OK) {
    WLOG_DD("CID-%04x: REMOVE_VAP request failed with status %u", oid, MAC_TO_HOST16(areq->u16Status));
    result = MTLK_ERR_MAC;
  }

CLEANUP:
  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send deactivate request to DUT core err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message FW_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_FW_CMD
 *  \note       value pointed to by \a size must be in range [1...sizeof(dutMessage_t)]
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_fw_msg (mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t      res = MTLK_ERR_OK;
  mtlk_core_t      *core = (mtlk_core_t *)context;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  mtlk_txmm_t      *txmm;
  unsigned          oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_FW_CMD);
  WAVE_CHECK_SIZE_MAX_VOID(*size, sizeof(dutMessage_t));

  oid = mtlk_vap_get_oid(core->vap_handle);
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
      ELOG_DDS("CID-%04x: DUT FW request status %d (%s)", oid, res, mtlk_get_error_text(res));
      memset(buffer, 0xee, *size);
    }

    mtlk_txmm_msg_cleanup(&man_msg);
  }
  else
  {
    ELOG_D("CID-%04x: Failed to send DUT FW request due to lack of management messages", oid);
  }
}

/******************************************************************************/
/*! \brief      Handle DUT message SET_BSS_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_SET_BSS_CMD
 *  \note       value pointed to by \a size must be equal to sizeof(UMI_SET_BSS)
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_set_bss_msg (
                              mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t       result;
  mtlk_txmm_data_t  *man_entry = NULL;
  mtlk_txmm_msg_t    activate_msg;
  UMI_SET_BSS       *areq;
  UMI_SET_BSS       *msg = (UMI_SET_BSS *)buffer;
  mtlk_core_t       *nic = (mtlk_core_t *)context;
  unsigned           oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_SET_BSS_CMD);

  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG1_D("CID-%04x: DUTCore SET_BSS", oid);

  WAVE_CHECK_SIZE_EXP_VOID(*size, sizeof(*msg));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send SET_BSS request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = UM_BSS_SET_BSS_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_SET_BSS*)(man_entry->payload);
  *areq = *msg;

  mtlk_dump(2, areq, sizeof(UMI_SET_BSS), "dump of UMI_SET_BSS:");

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->Status) {
    ELOG_DDD("CID-%04x: SET_BSS request failed, result=%d status=%hhu", oid, result, areq->Status);
    if (UMI_OK != areq->Status)
      result = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send SET_BSS request err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message SET_WMM_PARAMETERS_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD
 *  \note       value pointed to by \a size must be equal to sizeof(UMI_SET_WMM_PARAMETERS)
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_set_wmm_parameters_msg (
                              mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t            result;
  mtlk_txmm_data_t       *man_entry = NULL;
  mtlk_txmm_msg_t         activate_msg;
  UMI_SET_WMM_PARAMETERS *areq;
  UMI_SET_WMM_PARAMETERS *msg = (UMI_SET_WMM_PARAMETERS*)buffer;
  mtlk_core_t            *nic = (mtlk_core_t *)context;
  unsigned                oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD);

  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG1_D("CID-%04x: DUTCore SET_WMM_PARAMETERS", oid);

  WAVE_CHECK_SIZE_EXP_VOID(*size, sizeof(*msg));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send SET_WMM_PARAMETERS request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = UM_MAN_SET_WMM_PARAMETERS_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_SET_WMM_PARAMETERS*)(man_entry->payload);
  *areq = *msg;

  mtlk_dump(2, areq, sizeof(UMI_SET_WMM_PARAMETERS), "dump of UMI_SET_WMM_PARAMETERS:");
  ILOG0_DD("CID-%04x: UMI_SET_WMM_PARAMETERS vapId=%u", oid, areq->vapId);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != areq->u8Status) {
    ELOG_DDD("CID-%04x: SET_WMM_PARAMETERS request failed, result=%d status=%hhu",
            oid, result, areq->u8Status);
    if (UMI_OK != areq->u8Status)
      result = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send SET_WMM_PARAMETERS request err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message STOP_VAP_TRAFFIC_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD
 *  \note       value pointed to by \a size must be equal to sizeof(UMI_STOP_VAP_TRAFFIC)
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_stop_vap_traffic_msg (
                              mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t          result;
  mtlk_txmm_data_t     *man_entry = NULL;
  mtlk_txmm_msg_t       activate_msg;
  UMI_STOP_VAP_TRAFFIC *areq;
  UMI_STOP_VAP_TRAFFIC *msg = (UMI_STOP_VAP_TRAFFIC*)buffer;
  mtlk_core_t          *nic = (mtlk_core_t *)context;
  unsigned              oid;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD);

  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG1_D("CID-%04x: DUTCore STOP_VAP_TRAFFIC", oid);

  WAVE_CHECK_SIZE_EXP_VOID(*size, sizeof(*msg));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send  STOP_VAP_TRAFFIC request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = UM_MAN_STOP_VAP_TRAFFIC_REQ;
  man_entry->payload_size = sizeof (*areq);

  areq = (UMI_STOP_VAP_TRAFFIC*)(man_entry->payload);
  *areq = *msg;

  mtlk_dump(2, areq, sizeof(UMI_STOP_VAP_TRAFFIC), "dump of UMI_STOP_VAP_TRAFFIC:");
  ILOG0_DD("CID-%04x: UMI_STOP_VAP_TRAFFIC vapId=%u", mtlk_vap_get_oid(nic->vap_handle), areq->vapId);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK  != MAC_TO_HOST16(areq->u16Status)) {
    ELOG_DDD("CID-%04x: STOP_VAP_TRAFFIC failed, result=%i status=%hu",
             oid, result, MAC_TO_HOST16(areq->u16Status));
    if (UMI_OK != MAC_TO_HOST16(areq->u16Status))
      result = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send  STOP_VAP_TRAFFIC request to DUT core err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message DRIVER_FW_GENERAL_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_DRIVER_FW_GENERAL_CMD
 *  \note       value pointed to by \a size must be in range [16, sizeof(UMI_MAN)]
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_driver_fw_general_msg (
                              mtlk_irbd_t       *irbd,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32            *size)
{
  mtlk_error_t             result;
  mtlk_txmm_data_t        *man_entry = NULL;
  mtlk_txmm_msg_t          activate_msg;
  mtlk_txmm_t             *txmm;
  dutDriverFwGeneralMsg_t *msg = (dutDriverFwGeneralMsg_t*)buffer;
  mtlk_core_t             *nic = (mtlk_core_t *)context;
  unsigned                 oid;
  size_t                   hdr_size, total_size;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_DRIVER_FW_GENERAL_CMD);

  oid = mtlk_vap_get_oid(nic->vap_handle);
  ILOG1_D("CID-%04x: DUTCore GeneralFW", oid);

  hdr_size = sizeof(*msg) - sizeof(msg->param);
  mtlk_dump(2, buffer, MIN(*size, hdr_size), "DUTCore GeneralFWReq header");

  WAVE_CHECK_SIZE_MIN_VOID(*size, hdr_size);

  msg->umiReqId =  DUT_TO_HOST32(msg->umiReqId);
  msg->umiLen   =  DUT_TO_HOST32(msg->umiLen);
  total_size = hdr_size + msg->umiLen;
  ILOG1_DDD("CID-%04x: DRIVER_FW_GENERAL, umiReqId %u, umiLen %u", oid, msg->umiReqId, msg->umiLen);
  if (*size != total_size) {
    ELOG_DDD("umiReqId %u, incorrect size %u (expected %u)", msg->umiReqId, *size, total_size);
    mtlk_dump(0, msg, MIN(32, *size), "dump of FW REQ:");
    return;
  }

  txmm = mtlk_vap_get_txmm(nic->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&activate_msg, txmm, &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send request to MAC due to the lack of MAN_MSG", oid);
    goto finish;
  }

  man_entry->id           = msg->umiReqId;
  man_entry->payload_size = msg->umiLen;
  wave_memcpy(man_entry->payload, txmm->base->cfg.max_payload_size, msg->param, msg->umiLen);

  mtlk_dump(2, man_entry->payload, msg->umiLen, "dump of FW REQ:");
  ILOG0_D("CID-%04x: DRIVER_FW_GENERAL", oid);

  result = mtlk_txmm_msg_send_blocked(&activate_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Cannot send activate request due to TXMM err#%d", oid, result);
  }

  mtlk_txmm_msg_cleanup(&activate_msg);

finish:
  if (result != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: Send general request to DUT core err#%d", oid, result);
  }

  return;
}

/******************************************************************************/
/*! \brief      Handle DUT message PLATFORM_DATA_FIELDS_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload, must not be NULL
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD
 *  \note       value pointed to by \a size could be any due to the payload of DUT message is ignored
 *  \note       \a buffer could be NULL if the value pointed to by \a size is ZERO
 *
 *  \return     void
 */
static void
_mtlk_dut_core_handle_platform_data_fields_msg (
        mtlk_irbd_t       *irbd,
        mtlk_handle_t      context,
        const mtlk_guid_t *evt,
        void              *buffer,
        uint32            *size)
{
    mtlk_error_t res;
    mtlk_core_t *core = (mtlk_core_t *)context;

    MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD);

    mtlk_dump(2, buffer, *size, "DUTCore LoadPlatformData");

    res = mtlk_psdb_load_to_hw(mtlk_vap_get_hw(core->vap_handle),
                               mtlk_vap_get_txmm(core->vap_handle));

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

/******************************************************************************/
/*! \brief      Unregister DUT API handlers for the DUT core
 *
 *  \param[in]  core     pointer to DUT core, must not be NULL
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *
 *  \return     void
 */
static void
_mtlk_dut_core_irbd_unregister (mtlk_core_t *core, mtlk_irbd_t *irbd)
{
  mtlk_vap_handle_t vap_handle;
  mtlk_dut_core_t *dut_core;

  vap_handle = core->vap_handle;
  dut_core = &core->dut_core;

  ILOG0_D("CID-%04x: Unregister DUT IRBD...", mtlk_vap_get_oid(vap_handle));

  MTLK_STOP_BEGIN(dut_core, MTLK_OBJ_PTR(dut_core))
    MTLK_STOP_STEP(dut_core, EEPROM_IRBD, MTLK_OBJ_PTR(dut_core),
                   mtlk_eeprom_irbd_unregister, (dut_core->dut_handlers->eeprom_irbd));
    MTLK_STOP_STEP(dut_core, CMD_PLATFORM_DATA_FIELDS, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->platform_data_fields_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_DRIVER_FW_GENERAL_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->driver_fw_general_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_STOP_VAP_TRAFFIC_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->stop_traffic_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_SET_WMM_PARAMETERS_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->set_wmm_parameters_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_SET_BSS_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->set_bss_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_REMOVE_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->remove_vap_msg_handle));
    MTLK_STOP_STEP(dut_core, CMD_ADD_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->add_vap_msg_handle));
    MTLK_STOP_STEP(dut_core, PROGMODEL_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->fw_msg_handle));
    MTLK_STOP_STEP(dut_core, FW_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                   mtlk_irbd_unregister, (irbd, dut_core->dut_handlers->progmodel_msg_handle));
    MTLK_STOP_STEP(dut_core, ALLOC_HANDLERS, MTLK_OBJ_PTR(dut_core),
                   mtlk_osal_mem_free, (dut_core->dut_handlers));
    dut_core->dut_handlers = NULL;
  MTLK_STOP_END(dut_core, MTLK_OBJ_PTR(dut_core))
}

/******************************************************************************/
/*! \brief      Stop DUT core
 *
 *  \param[in]  core     pointer to DUT core, must not be NULL
 *
 *  \return     MTLK_ERR_OK
 */
static mtlk_error_t
_mtlk_dut_core_start (mtlk_core_t *core)
{
  core->dut_core.isStarted = TRUE;
  ILOG0_D("CID-%04x: DUT Core started...", mtlk_vap_get_oid(core->vap_handle));

  return MTLK_ERR_OK;
}

/******************************************************************************/
/*! \brief      Register DUT API handlers for the DUT core
 *
 *  \param[in]  core     pointer to DUT core, must not be NULL
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *
 *  \return     MTLK_ERR_OK if success, other if error
 */
static mtlk_error_t
_mtlk_dut_core_irbd_register (mtlk_core_t *core, mtlk_irbd_t *irbd)
{
  mtlk_vap_handle_t vap_handle;
  mtlk_dut_core_t *dut_core;

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
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_FW_CMD, 1, _mtlk_dut_core_handle_fw_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->fw_msg_handle, dut_core->dut_handlers->fw_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, PROGMODEL_CMD_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_PROGMODEL_CMD, 1, _mtlk_dut_core_handle_progmodel_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->progmodel_msg_handle, dut_core->dut_handlers->progmodel_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_ADD_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_ADD_VAP_CMD, 1, _mtlk_dut_core_handle_add_vap_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->add_vap_msg_handle, dut_core->dut_handlers->add_vap_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_REMOVE_VAP_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_REMOVE_VAP_CMD, 1, _mtlk_dut_core_handle_remove_vap_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->remove_vap_msg_handle, dut_core->dut_handlers->remove_vap_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_SET_BSS_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_SET_BSS_CMD, 1, _mtlk_dut_core_handle_set_bss_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->set_bss_msg_handle, dut_core->dut_handlers->set_bss_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_SET_WMM_PARAMETERS_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_SET_WMM_PARAMETERS_CMD, 1, _mtlk_dut_core_handle_set_wmm_parameters_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->set_wmm_parameters_msg_handle, dut_core->dut_handlers->set_wmm_parameters_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_STOP_VAP_TRAFFIC_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_STOP_VAP_TRAFFIC_CMD, 1, _mtlk_dut_core_handle_stop_vap_traffic_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->stop_traffic_msg_handle, dut_core->dut_handlers->stop_traffic_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_DRIVER_FW_GENERAL_HANDLER, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_DRIVER_FW_GENERAL_CMD, 1, _mtlk_dut_core_handle_driver_fw_general_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->driver_fw_general_msg_handle, dut_core->dut_handlers->driver_fw_general_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, CMD_PLATFORM_DATA_FIELDS, MTLK_OBJ_PTR(dut_core),
                       mtlk_irbd_register, (irbd,
                       &_MTLK_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD, 1, _mtlk_dut_core_handle_platform_data_fields_msg, HANDLE_T(core)),
                       dut_core->dut_handlers->platform_data_fields_msg_handle, dut_core->dut_handlers->platform_data_fields_msg_handle != NULL,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(dut_core, EEPROM_IRBD, MTLK_OBJ_PTR(dut_core),
                       mtlk_eeprom_irbd_register, (vap_handle),
                       dut_core->dut_handlers->eeprom_irbd, dut_core->dut_handlers->eeprom_irbd != NULL,
                       MTLK_ERR_NO_RESOURCES);
  MTLK_START_FINALLY(dut_core, MTLK_OBJ_PTR(dut_core))
  MTLK_START_RETURN(dut_core, MTLK_OBJ_PTR(dut_core), _mtlk_dut_core_irbd_unregister, (core, irbd))
}

/******************************************************************************/
/*! \brief      Stop DUT core by DF
 *
 *  \param[in]  df    pointer to DF, must not be NULL
 *
 *  \return     none
 */
static void
_mtlk_dut_core_df_stop(mtlk_df_t *df)
{
  mtlk_error_t res;
  mtlk_clpb_t *clpb = NULL;

  do {
    res = _mtlk_df_user_invoke_core(df, WAVE_CORE_REQ_DEACTIVATE, &clpb, NULL, 0);
    res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_DEACTIVATE, TRUE);
    mtlk_osal_msleep(100);
  } while ((MTLK_ERR_OK != res) && (MTLK_ERR_FW != res));
}

/******************************************************************************/
/*! \brief      Handle DUT message START_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_START_CMD
 *  \note       value pointed to by \a size could be any due to the payload of DUT message is ignored
 *  \note       \a buffer could be NULL if the value pointed to by \a size is ZERO
 *
 *  \return     void
 */
static void
_mtlk_dut_core_prepare (mtlk_irbd_t       *irbd,
                        mtlk_handle_t      context,
                        const mtlk_guid_t *evt,
                        void              *buffer,
                        uint32            *size)
{
  mtlk_error_t res = MTLK_ERR_OK;
  BOOL run_dut = TRUE;
  int max_vaps_count, vap_index;
  mtlk_core_t *core = (mtlk_core_t *)context;
  mtlk_vap_manager_t *vap_mgr;
  mtlk_vap_handle_t vap_handle;
  mtlk_core_t *cur_core;
  unsigned     cur_oid;
  mtlk_df_t   *cur_df;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_START_CMD);

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
        MTLK_ASSERT(NULL != cur_df);
        if (NULL == cur_df) {
          ELOG_D("DUT get vap(%d): DF is NULL", vap_index);
          continue;
        }

        ILOG0_DD("DUT get vap(%d) handler, stopped=%d...",
            vap_index, cur_core->is_stopped);
        if (!cur_core->is_stopped) {
          /* FIXME: Feng Lin
           * If 'Recovery None' is supported,
           * we don't need remove existing VAPs.
           */
          ILOG0_D("CID-%04x: DUT Stop VAP...", cur_oid);
          _mtlk_dut_core_df_stop(cur_df);

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

/******************************************************************************/
/*! \brief      Handle DUT message STOP_CMD
 *
 *  \param[in]  irbd     pointer to Driver IRB object, must not be NULL
 *  \param[in]  context  nic instance handle, must not be ZERO
 *  \param[in]  evt      pointer to event, must not be NULL
 *  \param[in]  buffer   pointer to DUT message payload
 *  \param[in]  size     pointer to size of the payload, must not be NULL
 *
 *  \note       value pointed to by \a evt must be equal to _MTLK_IRBE_DUT_STOP_CMD
 *  \note       value pointed to by \a size could be any due to the payload of DUT message is ignored
 *  \note       \a buffer could be NULL if the value pointed to by \a size is ZERO
 *
 *  \return     void
 */
static void
_mtlk_dut_core_unprepare (mtlk_irbd_t       *irbd,
                          mtlk_handle_t      context,
                          const mtlk_guid_t *evt,
                          void              *buffer,
                          uint32            *size)
{
  mtlk_core_t *core = (mtlk_core_t *)context;

  MTLK_IRBD_CHECK_CTX_EVT(irbd, context, evt, buffer, size, &_MTLK_IRBE_DUT_STOP_CMD);

  if (core->dut_core.isStarted) {
    core->dut_core.isStarted = FALSE;
    ILOG0_D("CID-%04x: DUT Core stopped.", mtlk_vap_get_oid(core->vap_handle));
  }
}

/******************************************************************************/
/*! \brief      Register DUT core
 *
 *  \param[in]  core    pointer to DUT core, must not be NULL
 *
  * \return     MTLK_ERR_OK if success, other if error
 */
mtlk_error_t __MTLK_IFUNC
mtlk_dut_core_register (mtlk_core_t *core)
{
  mtlk_error_t  res;
  mtlk_irbd_t  *irbd;

  MTLK_ASSERT(core != NULL);
  if(!core)
    return MTLK_ERR_UNKNOWN;

  core->dut_core.isStarted = FALSE;

  if (!mtlk_vap_is_master(core->vap_handle)) {
    return MTLK_ERR_OK;
  }

  irbd = mtlk_vap_get_irbd(core->vap_handle);
  MTLK_RET_CHECK_PTR(irbd);

  res = _mtlk_dut_core_irbd_register(core, irbd);
  if (MTLK_ERR_OK != res) {
    return res;
  }

  core->dut_core.dut_start_handle = mtlk_irbd_register(irbd,
                                                       &_MTLK_IRBE_DUT_START_CMD,
                                                       1,
                                                       _mtlk_dut_core_prepare,
                                                       HANDLE_T(core));
  if (NULL == core->dut_core.dut_start_handle) {
    return MTLK_ERR_NO_RESOURCES;
  }

  core->dut_core.dut_stop_handle = mtlk_irbd_register(irbd,
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

/******************************************************************************/
/*! \brief      Unregister DUT core
 *
 *  \param[in]  core    pointer to DUT core, must not be NULL
 *
 *  \return     none
 */
void __MTLK_IFUNC
mtlk_dut_core_unregister (mtlk_core_t *core)
{
  mtlk_irbd_t     *irbd;

  MTLK_ASSERT(core != NULL);
  if(!core)
    return;

  if (!mtlk_vap_is_master(core->vap_handle)) {
    return;
  }

  irbd = mtlk_vap_get_irbd(core->vap_handle);
  MTLK_CHECK_PTR_VOID(irbd);

  _mtlk_dut_core_irbd_unregister(core, irbd);

  if (NULL != core->dut_core.dut_start_handle) {
    mtlk_irbd_unregister(irbd,
                         core->dut_core.dut_start_handle);
  }
  if (NULL != core->dut_core.dut_stop_handle) {
    mtlk_irbd_unregister(irbd,
                         core->dut_core.dut_stop_handle);
  }
}
