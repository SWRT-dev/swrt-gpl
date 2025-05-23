/*
 * Copyright (c) [2020], MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws.
 * The information contained herein is confidential and proprietary to
 * MediaTek Inc. and/or its licensors.
 * Except as otherwise provided in the applicable licensing terms with
 * MediaTek Inc. and/or its licensors, any reproduction, modification, use or
 * disclosure of MediaTek Software, and information contained herein, in whole
 * or in part, shall be strictly prohibited.
*/

#include "rt_config.h"
#if defined(CONFIG_CPE_SUPPORT)
#include "chip/mt7915_cr.h"
#endif

/*
 * ========================================================================
 * Routine Description:
 *    PCI command kernel thread.
 *
 * Arguments:
 *	*Context			the pAd, driver control block pointer
 *
 * Return Value:
 *   0					close the thread
 * ========================================================================
 */
INT RTPCICmdThread(
	IN ULONG Context)
{
	RTMP_ADAPTER *pAd;
	RTMP_OS_TASK *pTask;
	int status;

	status = 0;
	pTask = (RTMP_OS_TASK *)Context;
	pAd = (PRTMP_ADAPTER)RTMP_OS_TASK_DATA_GET(pTask);
	RtmpOSTaskCustomize(pTask);
	NdisAcquireSpinLock(&pAd->CmdQLock);
	pAd->CmdQ.CmdQState = RTMP_TASK_STAT_RUNNING;
	NdisReleaseSpinLock(&pAd->CmdQLock);

	while (pAd->CmdQ.CmdQState == RTMP_TASK_STAT_RUNNING) {
		if (RtmpOSTaskWait(pAd, pTask, &status) == FALSE) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			break;
		}

		if (pAd->CmdQ.CmdQState == RTMP_TASK_STAT_STOPED)
			break;

		if (!pAd->PM_FlgSuspend)
			CMDHandler(pAd);
	}

	if (!pAd->PM_FlgSuspend) {
		/* Clear the CmdQElements. */
		CmdQElmt	*pCmdQElmt = NULL;

		NdisAcquireSpinLock(&pAd->CmdQLock);
		pAd->CmdQ.CmdQState = RTMP_TASK_STAT_STOPED;

		while (pAd->CmdQ.size) {
			RTThreadDequeueCmd(&pAd->CmdQ, &pCmdQElmt);

			if (pCmdQElmt) {
				if (pCmdQElmt->CmdFromNdis == TRUE) {
					if (pCmdQElmt->buffer != NULL)
						os_free_mem(pCmdQElmt->buffer);

					os_free_mem((PUCHAR)pCmdQElmt);
				} else {
					if ((pCmdQElmt->buffer != NULL) && (pCmdQElmt->bufferlength != 0))
						os_free_mem(pCmdQElmt->buffer);

					os_free_mem((PUCHAR)pCmdQElmt);
				}
			}
		}

		NdisReleaseSpinLock(&pAd->CmdQLock);
	}

	/* notify the exit routine that we're actually exiting now
	 *
	 * complete()/wait_for_completion() is similar to up()/down(),
	 * except that complete() is safe in the case where the structure
	 * is getting deleted in a parallel mode of execution (i.e. just
	 * after the down() -- that's necessary for the thread-shutdown
	 * case.
	 *
	 * complete_and_exit() goes even further than this -- it is safe in
	 * the case that the thread of the caller is going away (not just
	 * the structure) -- this is necessary for the module-remove case.
	 * This is important in preemption kernels, which transfer the flow
	 * of execution immediately upon a complete().
	 */
	MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_INFO, "<---RTPCICmdThread\n");
	RtmpOSTaskNotifyToExit(pTask);
	return 0;
}



#ifdef MT_MAC
NDIS_STATUS mt_pci_chip_cfg(RTMP_ADAPTER *pAd, USHORT  id)
{
	NDIS_STATUS ret = NDIS_STATUS_SUCCESS;
	struct _RTMP_CHIP_CAP *cap = hc_get_chip_cap(pAd->hdev_ctrl);

	if (id == 0 || id == 0xffff)
		ret = NDIS_STATUS_FAILURE;

#ifdef CONFIG_FWOWN_SUPPORT

	if ((id == NIC7615_PCIe_DEVICE_ID)
		 || (id == NIC7616_PCIe_DEVICE_ID)
		 || (id == NIC761A_PCIe_DEVICE_ID)
		 || (id == NIC7611_PCIe_DEVICE_ID)
		 || (id == NIC7637_PCIe_DEVICE_ID))
		DriverOwn(pAd);

#endif
	pAd->ChipID = (UINT32)id;

	if ((id == NIC7603_PCIe_DEVICE_ID)
		|| (id == NIC7615_PCIe_DEVICE_ID)
		|| (id == NIC7616_PCIe_DEVICE_ID)
		|| (id == NIC761A_PCIe_DEVICE_ID)
		|| (id == NIC7611_PCIe_DEVICE_ID)
		|| (id == NIC7637_PCIe_DEVICE_ID)
		|| (id == NICP18_PCIe_DEVICE_ID)
		|| (id == NIC7663_PCIe_DEVICE_ID)
		|| (id == NICAXE_PCIe_DEVICE_ID)
		|| (id == 0x3280) /* debug by randy */
	   ) {
		UINT32 Value = 0;

		MAC_IO_READ32(pAd->hdev_ctrl, TOP_HVR, &Value);
		pAd->HWVersion = Value;

		if (Value == 0)
			ret = NDIS_STATUS_FAILURE;

		MAC_IO_READ32(pAd->hdev_ctrl, TOP_FVR, &Value);
		pAd->FWVersion = Value;

		if (Value == 0)
			ret = NDIS_STATUS_FAILURE;

		MAC_IO_READ32(pAd->hdev_ctrl, TOP_HCR, &Value);
		pAd->ChipID = Value;

		if (id == NIC7616_PCIe_DEVICE_ID)
			pAd->RfIcType = RFIC_7616;
		else if (id == NIC761A_PCIe_DEVICE_ID)
			pAd->RfIcType = RFIC_7615A;

		if (Value == 0)
			ret = NDIS_STATUS_FAILURE;

		if (IS_MT7603(pAd)) {
			MAC_IO_READ32(pAd->hdev_ctrl, STRAP_STA, &Value);
			pAd->AntMode = (Value >> 24) & 0x1;
		}

		MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_INFO,
				"HWVer=0x%x, FWVer=0x%x, pAd->ChipID=0x%x\n",
				pAd->HWVersion, pAd->FWVersion, pAd->ChipID);
	}
#if defined(CONFIG_CPE_SUPPORT)
	else if (id == NIC7915_PCIe_DEVICE_ID) {
		UINT32 reg = 0x74030D00;	/* PCIe debug dummy 8 CRs: 0x74030D00 ~ 0x74030D1C */
		UINT32 addr, cr_val = 0, val, i;

		val = 0x18000000 | GET_L1_REMAP_BASE(reg);
		HIF_IO_WRITE32(pAd->hdev_ctrl, HIF_ADDR_L1_REMAP_ADDR, val);
		HIF_IO_READ32(pAd->hdev_ctrl, HIF_ADDR_L1_REMAP_ADDR, &cr_val);
		if (val != cr_val) {
			MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_ERROR,
					 "write L1 remap addr fail! val=0x%x, cr_val=0x%x\n",
					  val, cr_val);
			return NDIS_STATUS_FAILURE;
		}

		val = 0xFFFFFFFF;
		for (i = 0; i < 8; i++) {
			addr = HIF_ADDR_L1_REMAP_BASE_ADDR + GET_L1_REMAP_OFFSET(reg) + (4 * i);
			HIF_IO_WRITE32(pAd->hdev_ctrl, addr, val);
			HIF_IO_READ32(pAd->hdev_ctrl, addr, &cr_val);
			if (val != cr_val) {
				MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_ERROR,
						 "write PCIe debug dummy CR fail! addr=0x%x, val=0x%x, cr_val=0x%x\n",
						  addr, val, cr_val);
				return NDIS_STATUS_FAILURE;
			}
		}

		val = 0xAAAAAAAA;
		for (i = 0; i < 8; i++) {
			addr = HIF_ADDR_L1_REMAP_BASE_ADDR + GET_L1_REMAP_OFFSET(reg) + (4 * i);
			HIF_IO_WRITE32(pAd->hdev_ctrl, addr, val);
			HIF_IO_READ32(pAd->hdev_ctrl, addr, &cr_val);
			if (val != cr_val) {
				MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_ERROR,
						 "write PCIe debug dummy CR fail! addr=0x%x, val=0x%x, cr_val=0x%x\n",
						  addr, val, cr_val);
				return NDIS_STATUS_FAILURE;
			}
		}

		val = 0x55555555;
		for (i = 0; i < 8; i++) {
			addr = HIF_ADDR_L1_REMAP_BASE_ADDR + GET_L1_REMAP_OFFSET(reg) + (4 * i);
			HIF_IO_WRITE32(pAd->hdev_ctrl, addr, val);
			HIF_IO_READ32(pAd->hdev_ctrl, addr, &cr_val);
			if (val != cr_val) {
				MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_ERROR,
						 "write PCIe debug dummy CR fail! addr=0x%x, val=0x%x, cr_val=0x%x\n",
						  addr, val, cr_val);
				return NDIS_STATUS_FAILURE;
			}
		}
	}
#endif

	cap->hif_type = HIF_MT;
	pAd->infType = RTMP_DEV_INF_PCIE;

	return ret;
}
#endif /* MT_MAC */


/***************************************************************************
 *
 *	PCIe device initialization related procedures.
 *
 ***************************************************************************/
VOID RTMPInitPCIeDevice(RT_CMD_PCIE_INIT *pConfig, VOID *pAdSrc)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	VOID *pci_dev = pConfig->pPciDev;
	USHORT  device_id = 0;
	POS_COOKIE pObj;
	NDIS_STATUS ret;

	pObj = (POS_COOKIE) pAd->OS_Cookie;
	pci_read_config_word(pci_dev, pConfig->ConfigDeviceID, &device_id);
#ifndef RT_BIG_ENDIAN
	device_id = le2cpu16(device_id);
#endif /* RT_BIG_ENDIAN */
	pObj->DeviceID = device_id;
	MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_INFO, "device_id=0x%x\n",
			device_id);
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_ADVANCE_POWER_SAVE_PCIE_DEVICE);
#ifdef MT_MAC
	ret = mt_pci_chip_cfg(pAd, device_id);

	/* check pci configuration CR can be read successfully */
	if (ret != NDIS_STATUS_SUCCESS) {
		pConfig->pci_init_succeed = FALSE;
		MTWF_DBG(pAd, DBG_CAT_HIF, CATHIF_PCI, DBG_LVL_ERROR, "pci configuration space can't be read\n");
		return;
	}

	pConfig->pci_init_succeed = TRUE;

#endif /* MT_MAC */

	if (pAd->infType != RTMP_DEV_INF_UNKNOWN)
		RtmpRaDevCtrlInit(pAd, pAd->infType);
}
