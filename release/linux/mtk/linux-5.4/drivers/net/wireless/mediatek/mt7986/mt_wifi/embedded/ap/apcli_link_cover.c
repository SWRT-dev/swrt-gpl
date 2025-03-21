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
/*
 ***************************************************************************
 ***************************************************************************

	Module Name:
	apcli_ctrl.c

	Abstract:

	Revision History:
	Who			When			What
	--------	----------		----------------------------------------------
	Fonchi		2006-06-23      modified for rt61-APClinent
*/
#ifdef APCLI_SUPPORT
#ifdef APCLI_LINK_COVER_SUPPORT
#include "rt_config.h"

extern VOID *p5G_pAd;
extern VOID *p2G_pAd;

INT ApcliLinkMonitorThread(
	IN ULONG Context)
{
	RTMP_ADAPTER *pAd;
	RTMP_ADAPTER *pAd_other_band;
	RTMP_OS_TASK *pTask;
	int status = 0;

	pTask = (RTMP_OS_TASK *)Context;
	pAd = (PRTMP_ADAPTER)RTMP_OS_TASK_DATA_GET(pTask);
	RtmpOSTaskCustomize(pTask);

	if (p2G_pAd == NULL) {
		MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
				("##### no 2G pAd!!!\n"));
		/* RtmpOSTaskNotifyToExit(pTask); */
		/* return 0; */
	} else if (p5G_pAd == NULL) {
		MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
				("##### no 5G pAd!!!\n"));
		/* RtmpOSTaskNotifyToExit(pTask); */
		/* return 0; */
	}

	if (p5G_pAd == pAd) {
		MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
				("we are 5G interface, wait 2G link update\n"));
		pAd_other_band = p2G_pAd;
	} else {
		MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
				("we are 2G interface, wait 5G link update\n"));
		pAd_other_band = p5G_pAd;
	}

	while (pTask && !RTMP_OS_TASK_IS_KILLED(pTask) && (pAd_other_band != NULL)) {
		if (RtmpOSTaskWait(pAd, pTask, &status) == FALSE) {
			RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS);
			break;
		}

		if (status != 0)
			break;

		/* TODO: wait_for_completion */
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
	MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
			("<---ApcliLinkMonitorThread\n"));
	/* if (pTask) */
	/* RtmpOSTaskNotifyToExit(pTask); */
	return 0;
}

NDIS_STATUS RtmpApcliLinkTaskInit(IN PRTMP_ADAPTER pAd)
{
	RTMP_OS_TASK *pTask;
	NDIS_STATUS status;
	/* Creat ApCli Link Monitor Thread */
	pTask = &pAd->apcliLinkTask;
	RTMP_OS_TASK_INIT(pTask, "LinkMonitorTask", pAd);
	/* status = RtmpOSTaskAttach(pTask, RTPCICmdThread, (ULONG)pTask); */
	status = RtmpOSTaskAttach(pTask, ApcliLinkMonitorThread, (ULONG)pTask);

	if (status == NDIS_STATUS_FAILURE) {
		MTWF_LOG(DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_TRACE,
				("%s: Unable to start ApcliLinkMonitorThread!\n", get_dev_name_prefix(pAd, INT_APCLI)));
		return NDIS_STATUS_FAILURE;
	}

	return NDIS_STATUS_SUCCESS;
}

VOID RtmpApcliLinkTaskExit(
	IN RTMP_ADAPTER *pAd)
{
	INT ret;
	/* Terminate cmdQ thread */
	if (RTMP_OS_TASK_LEGALITY(&pAd->apcliLinkTask)) {
		/*RTUSBCMDUp(&pAd->cmdQTask); */
		ret = RtmpOSTaskKill(&pAd->apcliLinkTask);

		if (ret == NDIS_STATUS_FAILURE)
			MTWF_DBG(pAd, DBG_CAT_CLIENT, CATCLIENT_APCLI, DBG_LVL_ERROR, "Kill command task fail!\n");
	}
}
#endif /* APCLI_LINK_COVER_SUPPORT */
#endif /* APCLI_SUPPORT */

