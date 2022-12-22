#include "rt_config.h"
#include "hw_ctrl.h"

/* For wifi and md coex in colgin project*/
#ifdef WIFI_MD_COEX_SUPPORT
/*---------------------------------------------------------------------*/
/* WIFI and MD Coexistence Realize                                     */
/*---------------------------------------------------------------------*/

static UCHAR get_wifi_pcie_slot(struct _RTMP_ADAPTER *pAd)
{
	UCHAR slot_id;
	struct pci_dev *pdev = ((POS_COOKIE)(pAd->OS_Cookie))->pci_dev;

	MTWF_DBG(NULL, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "===> name: %s\n", pdev->bus->name);

	if (!strcmp(pdev->bus->name, "PCI Bus 0003:01"))
		slot_id = 3;
	else if (!strcmp(pdev->bus->name, "PCI Bus 0002:01"))
		slot_id = 2;
	else if (!strcmp(pdev->bus->name, "PCI Bus 0001:01"))
		slot_id = 1;
	else
		slot_id = 0;

	return slot_id;
}

/* realize the wifi md coex_tx event func*/
static int wifi_md_coex_tx_event(struct notifier_block *nb, unsigned long event, void *msg)
{
	struct _RTMP_ADAPTER *pAd;
	struct _COEX_APCCCI2FW_CMD *p_coex_apccci2fw_cmd;

	/* get the pAd address*/
	p_coex_apccci2fw_cmd = CONTAINER_OF(nb, struct _COEX_APCCCI2FW_CMD, coex_apccci2fw_notifier);
	pAd = p_coex_apccci2fw_cmd->priv;

	switch (event) {
	case APCCCI_DRIVER_FW:
	{
		if (msg == NULL) {
			MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, "received NULL data in notifier chain!!");
			return NOTIFY_DONE;
		}

		/* send cmd to FW*/
		HW_WIFI_COEX_APCCCI2FW(pAd, msg);
	}
	break;
	case QUERY_FW_STATUS:
	{
#ifdef COEX_DIRECT_PATH
		mtf_get_wm_pc_status(pAd);
#endif/* COEX_DIRECT_PATH */
	}
	default:
		break;
	}
	return NOTIFY_DONE;
}

/* register */
static int register_wifi_md_coex(struct _RTMP_ADAPTER *pAd)
{
	pAd->coex_apccci2fw_cmd.coex_apccci2fw_notifier.notifier_call = wifi_md_coex_tx_event;
	pAd->coex_apccci2fw_cmd.priv = pAd;

#ifdef COEX_DIRECT_PATH
	mtf_set_conn_infra_sysram(pAd);
#endif /* COEX_DIRECT_PATH */

	return register_wifi_md_coex_notifier(&pAd->coex_apccci2fw_cmd.coex_apccci2fw_notifier);
}

/* unregister */
static int unregister_wifi_md_coex(struct _RTMP_ADAPTER *pAd)
{
	int err = 0;

	err = unregister_wifi_md_coex_notifier(&pAd->coex_apccci2fw_cmd.coex_apccci2fw_notifier);
	pAd->coex_apccci2fw_cmd.coex_apccci2fw_notifier.notifier_call = NULL;
	pAd->coex_apccci2fw_cmd.priv = NULL;
	return err;
}


#ifdef COEX_DIRECT_PATH
/* mapping conn_infra_sysram entry to PCIe(2g/5g/dbdc) MMIO */
VOID mtf_set_conn_infra_sysram(struct _RTMP_ADAPTER *pAd)
{
	/* config conn infra sysram */
	HW_IO_WRITE32(pAd->hdev_ctrl, ADDR_CONN_INFRA_SYSRAM, VAL_CONN_INFRA_SYSRAM);
	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "infra_addr=0x%x, infra_val=0x%x\n",
			ADDR_CONN_INFRA_SYSRAM, VAL_CONN_INFRA_SYSRAM);
}

/* get wm pc status to avoid FW hang */
VOID mtf_get_wm_pc_status(struct _RTMP_ADAPTER *pAd)
{
	unsigned int wm_pc = 0;
	static unsigned int wm_pc_same_count; /* default init 0 in BSS section */

	/* query MCU: WM status */
	RTMP_IO_READ32(pAd->hdev_ctrl, 0x7c060204, &wm_pc);
	if (pAd->coex_pre_wm_pc == wm_pc) {
		wm_pc_same_count++;
		MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
			"%s:WM_PC[0x%x] = 0x%x has not changed count = %d\n",
			0x7c060204, wm_pc, wm_pc_same_count);
		if (wm_pc_same_count % 10 == 0) {
			MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_ERROR,
				"WM_PC[0x%x] = 0x%x has not changed within 3s!!\n",
				0x7c060204, wm_pc);
			wm_pc_same_count = 0;
			call_wifi_md_coex_notifier(UNREGISTER_WIFI_MD_DTB, &pAd->fw2apccci_msg);
		}
	} else {
		pAd->coex_pre_wm_pc = wm_pc;
		wm_pc_same_count = 0;
	}
}
#endif/* COEX_DIRECT_PATH */

/* send wifi info */
int send_wifi_info_to_wifi_md_coex(struct _RTMP_ADAPTER *pAd, BOOL status)
{
	struct wifi_dev *wdev = NULL;
	struct _EXT_EVENT_FW2APCCCI_T *fw2apccci_msg = &pAd->fw2apccci_msg;
	UCHAR pci_slot_id = get_wifi_pcie_slot(pAd);
	UCHAR band_idx = 0, i, card_type = 0, pci_num;
	CHANNEL_CTRL *p_chn_ctrl;
	BOOLEAN b_found_5g_wdev = FALSE;

	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "pAd=%p, status=%d\n", pAd, status);

	if (pAd->CommonCfg.dbdc_mode) {
		for (i = 0; i < WDEV_NUM_MAX; i++) {
			wdev = pAd->wdev_list[i];
			if (wdev != NULL) {
				if (WMODE_CAP_5G(wdev->PhyMode)) {
					band_idx = HcGetBandByWdev(wdev);
					b_found_5g_wdev = TRUE;
					break;
				}
			}
		}

		if (b_found_5g_wdev) {
			p_chn_ctrl = hc_get_channel_ctrl(pAd->hdev_ctrl, band_idx);
			if (p_chn_ctrl) {
				if (p_chn_ctrl->ChGrpABandEn == 0)
					card_type = COEX_WIFI_DBDC;
				else if (p_chn_ctrl->ChGrpABandEn == 0b1100)
					card_type = COEX_WIFI_DBDC_5G_HIGH;
				else if (p_chn_ctrl->ChGrpABandEn == 0b0011)
					card_type = COEX_WIFI_DBDC_5G_LOW;
			}
		}

	}
	else
	{
		wdev = &pAd->ApCfg.MBSSID[MAIN_MBSSID].wdev;
		if (wdev) {
			if (WMODE_CAP_5G(wdev->PhyMode)) {
				p_chn_ctrl = hc_get_channel_ctrl(pAd->hdev_ctrl, band_idx);
				if (p_chn_ctrl) {
					if (p_chn_ctrl->ChGrpABandEn == 0)
						card_type = COEX_WIFI_5G;
					else if (p_chn_ctrl->ChGrpABandEn == 0b1100)
						card_type = COEX_WIFI_5G_HIGH;
					else if (p_chn_ctrl->ChGrpABandEn == 0b0011)
						card_type = COEX_WIFI_5G_LOW;
				}
			} else if (WMODE_CAP_2G(wdev->PhyMode)) {
				card_type = COEX_WIFI_2G;
			}
		}
	}

	if (card_type == 0) {
		MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_ERROR, "card_type == 0.\n");
		return FALSE;
	}

	pci_num = mt_get_active_pci_num();

	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "pci_slot_id=%d, card_type=%d, pci_num=%d\n", pci_slot_id, card_type, pci_num);
	fw2apccci_msg->pci_slot_id = pci_slot_id;
	fw2apccci_msg->card_type = card_type;
	fw2apccci_msg->pci_num = pci_num;
	if (status)
		call_wifi_md_coex_notifier(REGISTER_WIFI_MD_DTB, fw2apccci_msg);
	else
		call_wifi_md_coex_notifier(UNREGISTER_WIFI_MD_DTB, fw2apccci_msg);
	return TRUE;
}


/* Handle IDC info event sent by fw, update to sw_db. */
VOID Coex_IDC_Info_Handle(RTMP_ADAPTER *pAd, COEX_IDC_INFO *pEventIdcInfo)
{
	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "\n");
	NdisCopyMemory(&pAd->idcInfo, pEventIdcInfo, sizeof(pAd->idcInfo));

	return;
}

/* Set IDC state: 1: enable, 0: disable. */
INT Set_Idc_State(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	BOOLEAN	bEnable;

	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "\n");
	if (arg == NULL || strlen(arg) == 0)
		return FALSE;

	bEnable = os_str_tol(arg, 0, 10);
	if (bEnable != 0 && bEnable != 1) {
		MTWF_PRINT("Wrong param input.\n");
		return FALSE;
	}

	if (pAd->idcState != bEnable) {
		MtCmdIdcStateUpdate(pAd, bEnable);
		MTWF_PRINT("IDC state change to %s.\n", bEnable?"Enable":"Disable");

		pAd->idcState = bEnable;

		if (bEnable) {
			if (pAd->LteSafeChCtrl.bEnabled) {
				/* Query safe channel info when enable IDC */
				HW_QUERY_LTE_SAFE_CHANNEL(pAd);
				pAd->LteSafeChCtrl.bQueryLteDone = TRUE;
			}
		} else {
			/* clear all unsafe info when disable IDC */
			NdisFillMemory(pAd->LteSafeChCtrl.SafeChnBitmask, sizeof(pAd->LteSafeChCtrl.SafeChnBitmask), 0xFF);
		}
	}

	return TRUE;
}

/* Show IDC info, for debug use. */
INT Show_Idc_Info(RTMP_ADAPTER *pAd, RTMP_STRING *arg)
{
	COEX_IDC_INFO *pEventIdcInfo = &pAd->idcInfo;
	UINT8 idx;

	MTWF_DBG(pAd, DBG_CAT_COEX, DBG_SUBCAT_ALL, DBG_LVL_INFO, "\n");

	MtCmdIdcInfoQuery(pAd);
	MTWF_PRINT("IDC Info\n---------\n");

	MTWF_PRINT("idcEnable=%d\n", pEventIdcInfo->idcEnable);
	if (!pEventIdcInfo->idcEnable)
		return TRUE;

	MTWF_PRINT("MD operfreq: LTE/NR\n");
	MTWF_PRINT("  lte_oper_band=%d, lte_dl_freq=%d, lte_ul_freq=%d, nr_oper_band=%d, nr_dl_freq=%d, nr_ul_freq=%d\n",
		pEventIdcInfo->lte_oper_band, pEventIdcInfo->lte_dl_freq, pEventIdcInfo->lte_ul_freq,
		pEventIdcInfo->nr_oper_band, pEventIdcInfo->nr_dl_freq, pEventIdcInfo->nr_ul_freq);

	MTWF_PRINT("  u4SafeChannelBitmask:");
	for (idx = 0; idx < WIFI_CH_MASK_IDX_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->u4SafeChannelBitmask[idx]);
	}
	MTWF_PRINT("\n  u4TdmChannelBitmask:");
	for (idx = 0; idx < WIFI_CH_MASK_IDX_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->u4TdmChannelBitmask[idx]);
	}
	MTWF_PRINT("\n  u4PwrChannelBitmask:");
	for (idx = 0; idx < WIFI_CH_MASK_IDX_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->u4PwrChannelBitmask[idx]);
	}
	MTWF_PRINT("\n  u4FdmChannelBitmask:");
	for (idx = 0; idx < WIFI_CH_MASK_IDX_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->u4FdmChannelBitmask[idx]);
	}

	MTWF_PRINT("\nIDC Solution (Free run / Power backoff / TDM / Unsafe FDM)");
	MTWF_PRINT("\n  u4WiFiFinalSoluation:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=%d", idx, pEventIdcInfo->u4WiFiFinalSoluation[idx]);
	}
	MTWF_PRINT("\n  u4MDFinalSoluation:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=%d", idx, pEventIdcInfo->u4MDFinalSoluation[idx]);
	}

	MTWF_PRINT("\nMD Connected mode / Idle mode\n");
	MTWF_PRINT("  lteTxExist=%d, nrTxExist=%d\n", pEventIdcInfo->lteTxExist, pEventIdcInfo->nrTxExist);

	MTWF_PRINT("TDM flag\n");
	MTWF_PRINT("  isTdmForLte:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=%d", idx, pEventIdcInfo->isTdmForLte[idx]);
	}
	MTWF_PRINT("\n  isTdmForNr:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=%d", idx, pEventIdcInfo->isTdmForNr[idx]);
	}
	MTWF_PRINT("\n  lteCcBmp:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->lteCcBmp[idx]);
	}
	MTWF_PRINT("\n  nrCcBmp:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->nrCcBmp[idx]);
	}

	MTWF_PRINT("\nPwr flag\n");
	MTWF_PRINT("  u2LteScellBmp=0x%x, u2NrScellBmp=0x%x", pEventIdcInfo->u2LteScellBmp, pEventIdcInfo->u2NrScellBmp);

	MTWF_PRINT("\nMD / Wi-Fi TDM time\n");
	MTWF_PRINT("  tdm_lte_window=%d, tdm_lte_conn_window=%d, tdm_nr_window=%d, tdm_conn_window=%d",
		pEventIdcInfo->tdm_lte_window, pEventIdcInfo->tdm_lte_conn_window,
		pEventIdcInfo->tdm_nr_window, pEventIdcInfo->tdm_conn_window);

	MTWF_PRINT("\n3-Wire Arbitration");
	MTWF_PRINT("\n  fg3WireCommon:");
	for (idx = 0; idx < WIFI_RAM_BAND_NUM; idx++) {
		MTWF_PRINT(" [%d]=0x%x", idx, pEventIdcInfo->fg3WireCommon[idx]);
	}
	MTWF_PRINT("  u43WirePin=%d, u43WireReq=%d\n",
		pEventIdcInfo->u43WirePin, pEventIdcInfo->u43WireReq);

	return TRUE;
}

/* send debug level value */
VOID send_debug_level_to_wifi_md_coex(UCHAR debug_level)
{
	call_wifi_md_coex_notifier(COEX_DEBUG_LEVEL_MSG, &debug_level);

	return;
}

/* Do COEX init */
VOID init_wifi_md_coex(struct _RTMP_ADAPTER *pAd)
{
	pAd->idcState = TRUE;

	/* step1: register notification chain */
	register_wifi_md_coex(pAd);
	/* step2: notify coex module wifi up */
	send_wifi_info_to_wifi_md_coex(pAd, TRUE);
}

/* Do COEX de-init */
VOID deinit_wifi_md_coex(struct _RTMP_ADAPTER *pAd)
{
	pAd->idcState = FALSE;

	/* unregister notification chain */
	unregister_wifi_md_coex(pAd);
}

#endif /* WIFI_MD_COEX_SUPPORT */

