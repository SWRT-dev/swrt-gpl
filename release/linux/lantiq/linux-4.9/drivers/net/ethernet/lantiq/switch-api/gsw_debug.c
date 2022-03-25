/******************************************************************************
                Copyright (c) 2016, 2017 Intel Corporation

For licensing information, see the file 'LICENSE' in the root folder of
this software module.
******************************************************************************/

#include <gsw_init.h>
#include <gsw_defconf.h>
#include <xgmac_common.h>



static GSW_Debug_RMON_Port_cnt_t Rmon_get;
static GSW_CTP_portAssignment_t Assign_get;
static GSW_CTP_portConfig_t CTP_get;
static GSW_BRIDGE_portConfig_t BP_get;
static GSW_BRIDGE_portConfig_t Temp_BP_get;
static GSW_BRIDGE_config_t Brdg_get;
static GSW_QoS_queuePort_t q_map;
static GSW_PMAC_Cnt_t Pmac_Count;

GSW_return_t GSW_Debug_RMON_Port_Get(void *cdev, GSW_Debug_RMON_Port_cnt_t *parm)
{
	u8 RmonCntOffset = 0;
	u32 data = 0, data1 = 0;
	u32 r_frame = 0, r_unicast = 0, r_multicast = 0,
	    t_frame = 0, t_unicast = 0, t_multicast = 0;
	u32 rgbcl = 0, rbbcl = 0, tgbcl = 0;
	u64 rgbch = 0, rbbch = 0, tgbch = 0;
	u32 Portmode = 0, PortId = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 count = 0;
	bmtbl_prog_t bmtable = {0};

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	Portmode = parm->ePortType;
	PortId = parm->nPortId;
	/*Clear the structure before using*/
	memset(parm, 0, sizeof(GSW_Debug_RMON_Port_cnt_t));

	switch (Portmode) {
	case GSW_RMON_CTP_PORT_RX:
	case GSW_RMON_CTP_PORT_TX:
	case GSW_RMON_CTP_PORT_PCE_BYPASS:
		if (PortId >= gswdev->num_of_ctp) {
			pr_err("PortId %d >= gswdev->num_of_ctp %d\n", PortId, gswdev->num_of_ctp);
			return GSW_statusErr;
		}

		break;

	case GSW_RMON_BRIDGE_PORT_RX:
	case GSW_RMON_BRIDGE_PORT_TX:
		if (PortId >= gswdev->num_of_bridge_port) {
			pr_err("PortId %d >= gswdev->num_of_ctp %d\n", PortId, gswdev->num_of_bridge_port);
			return GSW_statusErr;
		}

		break;

	default:
		pr_err("ePortType Not Supported :ERROR (ePortType=%d)\n", Portmode);
		return GSW_statusErr;
	}

	/*GSW_ENABLE should be called before calling RMON_GET
	  In GSW_ENABLE RMON Counter for 0- 15 logical port are enabled*/

	if (Portmode == GSW_RMON_CTP_PORT_RX || Portmode == GSW_RMON_BRIDGE_PORT_RX)
		count = RMON_COUNTER_OFFSET_GSWIP3_1;
	else
		count = 14;

	//One-time populate.
	bmtable.adr.rmon.portOffset = PortId;
	bmtable.tableID = (BM_Table_ID)Portmode;
	bmtable.b64bitMode = 1;
	bmtable.numValues = 4;

	for (RmonCntOffset = 0; RmonCntOffset < count; RmonCntOffset++) {
		bmtable.adr.rmon.counterOffset = RmonCntOffset;
		gsw_bm_table_read(cdev, &bmtable);
		data = (bmtable.value[1] << 16) | (bmtable.value[0] & 0xFFFF);
		data1 = (bmtable.value[3] << 16) | (bmtable.value[2] & 0xFFFF);

		/*Only for RX, As per SAS counter offset 0 - 21 is
		  shared between RX and TX.
		  Depends upon ePortType selection*/
		if (Portmode == GSW_RMON_CTP_PORT_RX ||
		    Portmode == GSW_RMON_BRIDGE_PORT_RX) {
			switch (RmonCntOffset) {
			case 0x00: /*RX Size 64 frame count*/
				parm->nRx64BytePkts = data;
				parm->nRx127BytePkts = data1; /* Receive Size 65-127 Frame Count */
				break;

			case 0x02: /* Receive Size 128-255 Frame Count */
				parm->nRx255BytePkts = data;
				parm->nRx511BytePkts = data1; /* Receive Size 256-511 Frame Count */
				break;

			case 0x04: /* Receive Size 512-1023 Frame Count */
				parm->nRx1023BytePkts = data;
				parm->nRxMaxBytePkts = data1; /* Receive Size 1024 - 1518 Frame Count */
				break;

			case 0x06: /* Receive Unicast Frame Count */
				parm->nRxUnicastPkts = r_unicast = data;
				parm->nRxMulticastPkts = r_multicast = data1; /* Receive Multicast Frame Count1 */
				break;

			case 0x08: /* Receive Undersize Good Count */
				parm->nRxUnderSizeGoodPkts = data; /* Less than 64 byes. */
				parm->nRxOversizeGoodPkts = data1; /* Receive Oversize (> 1518) Good Count */
				break;

			case 0x0A: /* Receive Good Byte Count (Low) */
				rgbcl = data;
				rgbch = data1; /* Receive Good Byte Count (High) */
				break;

			case 0x0C: /* Receive Frme Count */
				parm->nRxBroadcastPkts = r_frame = data;
				parm->nRxFilteredPkts = data1; /* Receive Drop (Filter) Frame Count */
				break;

			case 0x0E: /* Receive Extended Vlan Discard Frame Count */
				parm->nRxExtendedVlanDiscardPkts = data;

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					parm->nRxFCSErrorPkts = data1; /* Receive MAC/FCS Error frame Count */

				break;

			case 0x10: /* Receive Undersize Bad Count */

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					parm->nRxUnderSizeErrorPkts = data;

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					parm->nRxOversizeErrorPkts = data1; /* Receive Oversize Bad Count */

				break;

			case 0x12: /* MTU Exceed Discard Count */

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					parm->nMtuExceedDiscardPkts = data;

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					parm->nRxDroppedPkts = data1; /* Receive Discard (Tail-Drop) Frame Count */

				break;

			case 0x14: /* Receive Bad Byte Count (Low) */

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					rbbcl = data;

				/*only applicable for CTP RX*/
				if (Portmode == GSW_RMON_CTP_PORT_RX)
					rbbch = data1; /* Receive Bad Byte Count (High) */

				break;
			}
		}

		/*Only for TX, As per SAS counter offset 0 - 21 is
		  shared between RX and TX.
		  Depends upon ePortType selection*/
		if (Portmode == GSW_RMON_CTP_PORT_TX ||
		    Portmode == GSW_RMON_CTP_PORT_PCE_BYPASS ||
		    Portmode == GSW_RMON_BRIDGE_PORT_TX) {
			switch (RmonCntOffset) {
			case 0x00: /* Transmit Size 64 Frame Count */
				parm->nTx64BytePkts = data;
				parm->nTx127BytePkts = data1; /* Transmit Size 65-127 Frame Count */
				break;

			case 0x02: /* Transmit Size 128-255 Frame Count */
				parm->nTx255BytePkts = data;
				parm->nTx511BytePkts = data1; /* Transmit Size 256-511 Frame Count */
				break;

			case 0x04: /* Transmit Size 512-1023 Frame Count */
				parm->nTx1023BytePkts = data;
				parm->nTxMaxBytePkts = data1; /* Transmit Size 1024 - 1518 Frame Count */
				break;

			case 0x06:  /* Transmit Unicast Frame Count */
				parm->nTxUnicastPkts = t_unicast = data;
				parm->nTxMulticastPkts = t_multicast = data1; /* Transmit Multicast Frame Count1 */
				break;

			case 0x08: /* Transmit Undersize Good Count */
				parm->nTxUnderSizeGoodPkts = data; /* Less than 64 byes */
				parm->nTxOversizeGoodPkts = data1; /* Transmit Oversize (> 1518) Good Count */
				break;

			case 0x0A: /* Transmit Good Byte Count (Low) */
				tgbcl = data;
				tgbch = data1; /* Transmit Good Byte Count (High) */
				break;

			case 0x0C: /* Transmit BroadCast Count */
				parm->nTxBroadcastPkts = t_frame = data;
				parm->nTxAcmDroppedPkts = data1; /* Egress Queue Discard,(Active Congestion Management) frame count.*/
				break;
			}
		}

		RmonCntOffset = RmonCntOffset + 1;
	}

	if (Portmode == GSW_RMON_CTP_PORT_RX ||
	    Portmode == GSW_RMON_BRIDGE_PORT_RX) {
		/* Receive Good Byte Count */
		parm->nRxGoodBytes = (u64)(((rgbch & 0xFFFFFFFF) << 32) | (rgbcl & 0xFFFFFFFF));

		if (Portmode == GSW_RMON_CTP_PORT_RX) {
			/* Receive Bad Byte Count */
			parm->nRxBadBytes = (u64)(((rbbch & 0xFFFFFFFF) << 32) | (rbbcl & 0xFFFFFFFF));
		}

		parm->nRxGoodPkts = r_frame + r_unicast + r_multicast;
	}

	if (Portmode == GSW_RMON_CTP_PORT_TX ||
	    Portmode == GSW_RMON_CTP_PORT_PCE_BYPASS ||
	    Portmode == GSW_RMON_BRIDGE_PORT_TX) {
		parm->nTxGoodPkts = t_frame + t_unicast + t_multicast;
		/* Transmit Good Byte Count */
		parm->nTxGoodBytes = (u64)(((tgbch & 0xFFFFFFFF) << 32) | (tgbcl & 0xFFFFFFFF));
	}

	parm->ePortType = Portmode;
	parm->nPortId = PortId;

	return GSW_statusOk;
}


GSW_return_t GSW_Debug_GetLpStatistics(void *cdev, GSW_debug_t *parm)
{
	u32 i = 0, j = 0, k = 0, ret = 0;
	u32 first_port = 0, no_port = 0;
	GSW_CTP_portAssignment_t Assign_get;
	static GSW_CTP_portConfig_t CTP_get;
	static GSW_BRIDGE_portConfig_t BP_get;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n\n");
	printk("\t\t----------------------------------------\n");
	printk("\t\t\t LOGICAL PORT Statistics\n");
	printk("\t\t----------------------------------------\n");

	for (j = 0; j < gswdev->tpnum; j++) {

		memset(&Assign_get, 0x00, sizeof(Assign_get));
		Assign_get.nLogicalPortId = j;
		ret = GSW_CTP_PortAssignmentGet(cdev, &Assign_get);

		if (ret == GSW_statusErr) {
			pr_err("GSW_CTP_PortAssignmentGet returns ERROR\n");
			return GSW_statusErr;
		}

		printk("\n");
		printk("\n------ Logical Port %u Configuration ------\n", j);
		printk("\n\t nLogicalPortId         = %u", Assign_get.nLogicalPortId);
		printk("\n\t nFirstCtpPortId        = %u", Assign_get.nFirstCtpPortId);
		printk("\n\t nNumberOfCtpPort       = %u", Assign_get.nNumberOfCtpPort);
		printk("\n\t eMode                  = %u", Assign_get.eMode);
		printk("\n\n\t The Following CTP Ports are associated with Logical port %u :", j);

		memset(&CTP_get, 0x00, sizeof(CTP_get));
		memset(&BP_get, 0x00, sizeof(BP_get));
		first_port = Assign_get.nFirstCtpPortId;
		no_port = Assign_get.nNumberOfCtpPort;

		for (i = first_port, k = 0; i < (first_port + no_port); i++, k++) {
			CTP_get.nLogicalPortId = j;
			CTP_get.nSubIfIdGroup = k;
			CTP_get.eMask = 0xffffffff;

			if (!gswdev->ctpportconfig_idx[(first_port + k)].IndexInUse) {
				continue;
			}

			ret = GSW_CtpPortConfigGet(cdev, &CTP_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_CtpPortConfigGet returns ERROR\n");
				return GSW_statusErr;
			}

			BP_get.nBridgePortId = CTP_get.nBridgePortId;
			BP_get.eMask = 0xffffffff;
			ret = GSW_BridgePortConfigGet(cdev, &BP_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_BridgePortConfigGet returns ERROR\n");
				return GSW_statusErr;
			}

			if (i == 287)
				printk("\n\t\t %u. CTP Ports %u is Default Port--Dummy port (connected to)--> Bridge port %u (Bridge/Fid = %u)\n",
				       (k + 1), i, CTP_get.nBridgePortId, BP_get.nBridgeId);
			else
				printk("\n\t\t %u. CTP Ports %u (connected to)--> Bridge port %u (Bridge/Fid = %u)\n",
				       (k + 1), i, CTP_get.nBridgePortId, BP_get.nBridgeId);

		}

	}


	printk("\n\n");
	printk("To Get individual CTP port's statistics, Please use following command :\n");
	printk("switch_cli GSW_DEBUG_CTP_STATISTICS Index=CTP_PORT_INDEX --> example: 0 to 287\n");
	return GSW_statusOk;
}


GSW_return_t GSW_Debug_GetCtpStatistics(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i = 0, j = 0, k = 0, f = 0, match = 0, ctpidx = 0, lp = 0, ret = 0;
	u32 first_port = 0, no_port = 0, count = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	memset(&Assign_get, 0x00, sizeof(Assign_get));
	memset(&CTP_get, 0x00, sizeof(CTP_get));
	memset(&BP_get, 0x00, sizeof(BP_get));
	memset(&Brdg_get, 0x00, sizeof(Brdg_get));
	memset(&Pmac_Count, 0x00, sizeof(Pmac_Count));
	memset(&Rmon_get, 0x00, sizeof(Rmon_get));


	printk("\n");
	ctpidx = parm->nTableIndex;
	lp = gswdev->ctpportconfig_idx[ctpidx].AssociatedLogicalPort;

	if (!gswdev->ctpportconfig_idx[ctpidx].IndexInUse) {
		printk("ERROR :CTP Port %u = NotInUse (CTP port not Allocated) and ", ctpidx);

		if (lp == 0xFF)
			printk("Associated to Logical port = None\n");
		else
			printk("Associated to Logical port = %d\n\n", lp);

		printk("\n");
		printk("Please check CTP to Logical port assignment using following command :\n");
		printk("switch_cli GSW_DEBUG_LP_STATISTICS\n");
		return GSW_statusErr;
	} else {
		if (lp == 0xFF) {
			printk("ERROR :CTP Port %d = InUse and ", ctpidx);
			printk("Associated to Logical port = None\n\n");
			printk("\n");
			printk("Please check CTP to Logical port assignment using following command :\n");
			printk("switch_cli GSW_DEBUG_LP_STATISTICS\n");
			return GSW_statusErr;
		} else {
			memset(&Assign_get, 0x00, sizeof(Assign_get));
			Assign_get.nLogicalPortId = lp;
			ret = GSW_CTP_PortAssignmentGet(cdev, &Assign_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_CTP_PortAssignmentGet returns ERROR\n");
				return GSW_statusErr;
			}

			first_port = Assign_get.nFirstCtpPortId;
			no_port = Assign_get.nNumberOfCtpPort;
			CTP_get.nSubIfIdGroup = 0;
			CTP_get.nLogicalPortId = lp;
			CTP_get.eMask = 0xFFFFFFFF;

			for (i = first_port; ((i < (first_port + no_port)) && !match); i++) {
				if ((first_port + CTP_get.nSubIfIdGroup) == ctpidx)
					match = 1;
				else
					CTP_get.nSubIfIdGroup++;
			}

			if (!match) {
				printk("ERROR : CTP index %d is not with in the Logical port %d assigned CTP range\n", ctpidx, lp);
				printk("\n\t nLogicalPortId 		= %u", Assign_get.nLogicalPortId);
				printk("\n\t nFirstCtpPortId		= %u", Assign_get.nFirstCtpPortId);
				printk("\n\t nNumberOfCtpPort		= %u", Assign_get.nNumberOfCtpPort);
				printk("\n\t eMode					= %u", Assign_get.eMode);
				printk("\n");
				printk("Please check CTP to Logical port assignment using following command :\n");
				printk("switch_cli GSW_DEBUG_LP_STATISTICS\n");
				return GSW_statusErr;
			}

			ret = GSW_CtpPortConfigGet(cdev, &CTP_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_CtpPortConfigGet returns ERROR\n");
				return GSW_statusErr;
			}

			BP_get.nBridgePortId = CTP_get.nBridgePortId;
			BP_get.eMask = 0xffffffff;

			ret = GSW_BridgePortConfigGet(cdev, &BP_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_BridgePortConfigGet returns ERROR\n");
				return GSW_statusErr;
			}

			Brdg_get.nBridgeId = BP_get.nBridgeId;
			Brdg_get.eMask = 0xffffffff;

			ret = GSW_BridgeConfigGet(cdev, &Brdg_get);

			if (ret == GSW_statusErr) {
				pr_err("GSW_BridgeConfigGet returns ERROR\n");
				return GSW_statusErr;
			}

		}

		printk("\n\n");
		printk("\t\t----------------------------------------\n");
		printk("\t\t\t CTP PORT %d Statistics\n", ctpidx);
		printk("\t\t----------------------------------------\n");
		printk("\n\n");

		printk("CTP PORT %u is associated with Logical port %u and Configured with Bridge port %u\n\n", ctpidx, lp, CTP_get.nBridgePortId);
		printk("Logical Port %d Configuration :\n", lp);
		printk("\n\t nLogicalPortId         = %u", Assign_get.nLogicalPortId);
		printk("\n\t nFirstCtpPortId        = %u", Assign_get.nFirstCtpPortId);
		printk("\n\t nNumberOfCtpPort       = %u", Assign_get.nNumberOfCtpPort);
		printk("\n\t eMode                  = %u", Assign_get.eMode);
		printk("\n");

		printk("\nCTP PORT %u Configuration (CTP PORT %u Connected to Bridge Port %u) :", ctpidx, ctpidx, CTP_get.nBridgePortId);
		printk("\n\t nLogicalPortId 					= %u", CTP_get.nLogicalPortId);
		printk("\n\t nSubIfIdGroup						= %u", CTP_get.nSubIfIdGroup);
		memset(&Rmon_get, 0x00, sizeof(GSW_Debug_RMON_Port_cnt_t));
		Rmon_get.nPortId = Assign_get.nFirstCtpPortId + CTP_get.nSubIfIdGroup;
		/*bridge port RX rmon*/
		Rmon_get.ePortType = 0;
		GSW_Debug_RMON_Port_Get(cdev, &Rmon_get);
		printk("\n\t\t --Ingress CTP Port  %u RX RMON Counter :", Rmon_get.nPortId);
		printk("\n\t\t\t nRxGoodPkts             = %u", Rmon_get.nRxGoodPkts);
		printk("\n\t\t\t nRxUnicastPkts          = %u", Rmon_get.nRxUnicastPkts);
		printk("\n\t\t\t nRxBroadcastPkts        = %u", Rmon_get.nRxBroadcastPkts);
		printk("\n\t\t\t nRxMulticastPkts        = %u", Rmon_get.nRxMulticastPkts);
		printk("\n\t\t\t nRxFCSErrorPkts         = %u", Rmon_get.nRxFCSErrorPkts);
		printk("\n\t\t\t nRxUnderSizeGoodPkts    = %u", Rmon_get.nRxUnderSizeGoodPkts);
		printk("\n\t\t\t nRxOversizeGoodPkts     = %u", Rmon_get.nRxOversizeGoodPkts);
		printk("\n\t\t\t nRxUnderSizeErrorPkts   = %u", Rmon_get.nRxUnderSizeErrorPkts);
		printk("\n\t\t\t nRxOversizeErrorPkts    = %u", Rmon_get.nRxOversizeErrorPkts);
		printk("\n\t\t\t nRxFilteredPkts         = %u", Rmon_get.nRxFilteredPkts);
		printk("\n\t\t\t nRxDroppedPkts          = %u", Rmon_get.nRxDroppedPkts);
		printk("\n\t nBridgePortId                      = %u", CTP_get.nBridgePortId);
		printk("\n\t bForcedTrafficClass                = %u", CTP_get.bForcedTrafficClass);
		printk("\n\t nDefaultTrafficClass               = %u", CTP_get.nDefaultTrafficClass);
		printk("\n\t bIngressExtendedVlanEnable         = %u", CTP_get.bIngressExtendedVlanEnable);
		printk("\n\t bIngressExtendedVlanIgmpEnable     = %u", CTP_get.bIngressExtendedVlanIgmpEnable);
		printk("\n\t bEgressExtendedVlanEnable          = %u", CTP_get.bEgressExtendedVlanEnable);
		printk("\n\t bEgressExtendedVlanIgmpEnable      = %u", CTP_get.bEgressExtendedVlanIgmpEnable);
		printk("\n\t bIngressNto1VlanEnable             = %u", CTP_get.bIngressNto1VlanEnable);
		printk("\n\t bEgressNto1VlanEnable              = %u", CTP_get.bEgressNto1VlanEnable);
		printk("\n\t bIngressMeteringEnable             = %u", CTP_get.bIngressMeteringEnable);
		printk("\n\t bEgressMeteringEnable              = %u", CTP_get.bEgressMeteringEnable);
		printk("\n\t bBridgingBypassEnable              = %u", CTP_get.bBridgingBypass);

		if (CTP_get.bBridgingBypass) {
			printk("\n\tCTP PORT %u Egress Configuration (Bridge ByPass Enabled) :", ctpidx);
			printk("\n\t nDestLogicalPortId 				= %u", CTP_get.nDestLogicalPortId);
			printk("\n\t nDestSubIfIdGroup					= %u", CTP_get.nDestSubIfIdGroup);
			printk("\n\t bPmapperEnable 					= %u", CTP_get.bPmapperEnable);

			if (CTP_get.bPmapperEnable) {
				printk("\n\t ePmapperMappingMode				= %u", CTP_get.ePmapperMappingMode);
				printk("\n\t nPmapperId 						= %u", BP_get.sPmapper.nPmapperId);
			}
		}

		printk("\n");

		printk("\nBridge PORT %u Configuration (Bridge PORT %u Connected to Bridge/Fid %u) :",
		       CTP_get.nBridgePortId, CTP_get.nBridgePortId, BP_get.nBridgeId);
		printk("\n\t nBridgePortId                                      = %u", BP_get.nBridgePortId);
		memset(&Rmon_get, 0x00, sizeof(GSW_Debug_RMON_Port_cnt_t));
		Rmon_get.nPortId = BP_get.nBridgePortId;
		/*bridge port RX rmon*/
		Rmon_get.ePortType = 2;
		GSW_Debug_RMON_Port_Get(cdev, &Rmon_get);
		printk("\n\t\t --Ingress Bridge Port  %u RX RMON Counter :", BP_get.nBridgePortId);
		printk("\n\t\t\t nRxGoodPkts             = %u", Rmon_get.nRxGoodPkts);
		printk("\n\t\t\t nRxUnicastPkts          = %u", Rmon_get.nRxUnicastPkts);
		printk("\n\t\t\t nRxBroadcastPkts        = %u", Rmon_get.nRxBroadcastPkts);
		printk("\n\t\t\t nRxMulticastPkts        = %u", Rmon_get.nRxMulticastPkts);
		printk("\n\t\t\t nRxFCSErrorPkts         = %u", Rmon_get.nRxFCSErrorPkts);
		printk("\n\t\t\t nRxUnderSizeGoodPkts    = %u", Rmon_get.nRxUnderSizeGoodPkts);
		printk("\n\t\t\t nRxOversizeGoodPkts     = %u", Rmon_get.nRxOversizeGoodPkts);
		printk("\n\t\t\t nRxUnderSizeErrorPkts   = %u", Rmon_get.nRxUnderSizeErrorPkts);
		printk("\n\t\t\t nRxOversizeErrorPkts    = %u", Rmon_get.nRxOversizeErrorPkts);
		printk("\n\t\t\t nRxFilteredPkts         = %u", Rmon_get.nRxFilteredPkts);
		printk("\n\t\t\t nRxDroppedPkts          = %u", Rmon_get.nRxDroppedPkts);
		printk("\n\t nBridgeId                                          = %u", BP_get.nBridgeId);
		printk("\n\t\t --nBridgeId/FID %u's Configuration :", BP_get.nBridgeId);
		printk("\n\t\t\t nMacLearningLimit                  = %u", Brdg_get.nMacLearningLimit);
		printk("\n\t\t\t nMacLearningCount                  = %u", Brdg_get.nMacLearningCount);
		printk("\n\t\t\t nLearningDiscardEvent              = %u", Brdg_get.nLearningDiscardEvent);
		printk("\n\t\t\t bBroadcastMeterEnable              = %u", Brdg_get.bSubMeteringEnable[0]);
		printk("\n\t\t\t bMulticastMeterEnable              = %u", Brdg_get.bSubMeteringEnable[1]);
		printk("\n\t\t\t bUnknownMulticastIpMeterEnable     = %u", Brdg_get.bSubMeteringEnable[2]);
		printk("\n\t\t\t bUnknownMulticastNonIpMeterEnable  = %u", Brdg_get.bSubMeteringEnable[3]);
		printk("\n\t\t\t bUnknownUniCastMeterEnable         = %u", Brdg_get.bSubMeteringEnable[4]);
		printk("\n\t bIngressExtendedVlanEnable                         = %u", BP_get.bIngressExtendedVlanEnable);
		printk("\n\t bEgressExtendedVlanEnable                          = %u", BP_get.bEgressExtendedVlanEnable);
		printk("\n\t bIngressMeteringEnable                             = %u", BP_get.bIngressMeteringEnable);
		printk("\n\t bEgressMeteringEnable                              = %u", BP_get.bEgressSubMeteringEnable[5]);
		printk("\n\t bEgressBroadcastSubMeteringEnable                  = %u", BP_get.bEgressSubMeteringEnable[0]);
		printk("\n\t bEgressMulticastSubMeteringEnable                  = %u", BP_get.bEgressSubMeteringEnable[1]);
		printk("\n\t bEgressUnknownMulticastIPSubMeteringEnable         = %u", BP_get.bEgressSubMeteringEnable[2]);
		printk("\n\t bEgressUnknownMulticastNonIPSubMeteringEnable      = %u", BP_get.bEgressSubMeteringEnable[3]);
		printk("\n\t bEgressUnknownUnicastSubMeteringEnable             = %u", BP_get.bEgressSubMeteringEnable[4]);
		printk("\n\t bPmapperEnable                                     = %u", BP_get.bPmapperEnable);
		printk("\n\t nMacLearningLimit                                  = %u", BP_get.nMacLearningLimit);
		printk("\n\t nMacLearningCount                                  = %u", BP_get.nMacLearningCount);
		printk("\n\t bIngressVlanFilterEnable                           = %u", BP_get.bIngressVlanFilterEnable);
		printk("\n\t bBypassEgressVlanFilter1Enable                     = %u", BP_get.bBypassEgressVlanFilter1);
		printk("\n\t bEgressVlanFilter1Enable                           = %u", BP_get.bEgressVlanFilter1Enable);
		printk("\n\t bEgressVlanFilter2Enable                           = %u", BP_get.bEgressVlanFilter2Enable);

		for (j = 0; j < 8; j++)
			printk("\n\t nBridgePortMapIndex[%u]                             = 0x%x", j, BP_get.nBridgePortMap[j]);

		printk("\n");
		printk("\nBridge PORT %u's Egress Configuration :", BP_get.nBridgePortId);
		printk("\n\t nDestLogicalPortId           = %u", BP_get.nDestLogicalPortId);
		printk("\n\t nDestSubIfIdGroup            = %u", BP_get.nDestSubIfIdGroup);

		if (BP_get.bPmapperEnable) {
			printk("\n\t ePmapperMappingMode                                = %u", BP_get.ePmapperMappingMode);
			printk("\n\t nPmapperId 										= %u", BP_get.sPmapper.nPmapperId);
		}

		printk("\n\n");
		printk("\nBridge PORT %u (Ingress) is Mapped to Following Egress Bridge Ports :", BP_get.nBridgePortId);
		count = 0;

		for (j = 0, k = 0; j < 8; j++) {
			for (i = 0; i < 16; i++, k++) {
				if (BP_get.nBridgePortMap[j] & (1 << i)) {
					memset(&Temp_BP_get, 0x00, sizeof(Temp_BP_get));
					Temp_BP_get.nBridgePortId = k;
					Temp_BP_get.eMask = 0xffffffff;
					ret = GSW_BridgePortConfigGet(cdev, &Temp_BP_get);

					if (ret == GSW_statusErr) {
						pr_err("GSW_BridgePortConfigGet(EGBP) returns ERROR\n");
						return GSW_statusErr;
					}

					count++;
					printk("\n\t Egress BP ---------------------------------------- = %u", k);
					memset(&Rmon_get, 0x00, sizeof(Rmon_get));
					Rmon_get.nPortId = k;
					/*bridge port TX rmon*/
					Rmon_get.ePortType = 3;
					GSW_Debug_RMON_Port_Get(cdev, &Rmon_get);
					printk("\n\t\t --Egress Bridge Port  %u TX RMON Counter :", k);
					printk("\n\t\t\t nTxGoodPkts       = %u", Rmon_get.nTxGoodPkts);
					printk("\n\t\t\t nTxUnicastPkt     = %u", Rmon_get.nTxUnicastPkts);
					printk("\n\t\t\t nTxBroadcastPkts  = %u", Rmon_get.nTxBroadcastPkts);
					printk("\n\t\t\t nTxMulticastPkts  = %u", Rmon_get.nTxMulticastPkts);
					printk("\n\t\t\t nTxDroppedPkts    = %u", Rmon_get.nTxDroppedPkts);
					printk("\n\t\t nDestLogicalPortId 								    = %u", Temp_BP_get.nDestLogicalPortId);
					printk("\n\t\t nDestSubIfIdGroup									= %u", Temp_BP_get.nDestSubIfIdGroup);

					if (Temp_BP_get.bPmapperEnable) {
						printk("\n\t\t ePmapperMappingMode								= %u", Temp_BP_get.ePmapperMappingMode);
						printk("\n\t\t nPmapperId 										= %u", Temp_BP_get.sPmapper.nPmapperId);
					} else {
						memset(&Assign_get, 0x00, sizeof(Assign_get));
						Assign_get.nLogicalPortId = Temp_BP_get.nDestLogicalPortId;
						ret = GSW_CTP_PortAssignmentGet(cdev, &Assign_get);

						if (ret == GSW_statusErr) {
							pr_err("GSW_CTP_PortAssignmentGet (EGBP) returns ERROR\n");
							return GSW_statusErr;
						}

						printk("\n\t\t\t --Destination Logical Port %d's Configuration :", Temp_BP_get.nDestLogicalPortId);
						printk("\n\t\t\t\t nLogicalPortId       = %u", Assign_get.nLogicalPortId);
						printk("\n\t\t\t\t nFirstCtpPortId      = %u", Assign_get.nFirstCtpPortId);
						printk("\n\t\t\t\t nNumberOfCtpPort     = %u", Assign_get.nNumberOfCtpPort);
						printk("\n\t\t\t\t eMode                = %u", Assign_get.eMode);
						memset(&Rmon_get, 0x00, sizeof(Rmon_get));
						Rmon_get.nPortId = Assign_get.nFirstCtpPortId + Temp_BP_get.nDestSubIfIdGroup;
						/*ctp port TX rmon*/
						Rmon_get.ePortType = 1;
						GSW_Debug_RMON_Port_Get(cdev, &Rmon_get);
						printk("\n\n\t\t\t --NOTE : FirstCtp(%u) of (Dst LP %u) + DstSubId(%u) of (EGBP %u)",
						       Assign_get.nFirstCtpPortId, Assign_get.nLogicalPortId, Temp_BP_get.nDestSubIfIdGroup,
						       k);
						printk("\n\t\t\t Egress CTP Port %u TX RMON Counter :", k);
						printk("\n\t\t\t\t nTxGoodPkts       = %u", Rmon_get.nTxGoodPkts);
						printk("\n\t\t\t\t nTxUnicastPkt     = %u", Rmon_get.nTxUnicastPkts);
						printk("\n\t\t\t\t nTxBroadcastPkts  = %u", Rmon_get.nTxBroadcastPkts);
						printk("\n\t\t\t\t nTxMulticastPkts  = %u", Rmon_get.nTxMulticastPkts);
						printk("\n\t\t\t\t nTxDroppedPkts    = %u", Rmon_get.nTxDroppedPkts);
					}

					printk("\n");
					printk("\n\t\t nDestLogicalPortId %u's PCE queue mapping Configuration as per Traffic Class :", Temp_BP_get.nDestLogicalPortId);

					/*Traffic Class from 0 to 15*/
					for (f = 0; f < 16; f++) {
						memset(&q_map, 0, sizeof(GSW_QoS_queuePort_t));
						q_map.nTrafficClassId = i;
						q_map.nPortId = Temp_BP_get.nDestLogicalPortId;
						GSW_QoS_QueuePortGet(cdev, &q_map);
						printk("\n\t\t\t TC = %u, QId = %u, QMapMode = %u, ExEna = %u, RedirectPortId = %u\n",
						       q_map.nTrafficClassId, q_map.nQueueId, q_map.eQMapMode, q_map.bExtrationEnable, q_map.nRedirectPortId);

						if (q_map.nRedirectPortId == 0 || q_map.nRedirectPortId == 1) {
							memset(&Pmac_Count, 0x00, sizeof(Pmac_Count));
							Pmac_Count.nPmacId = q_map.nRedirectPortId;
							Pmac_Count.nTxDmaChanId = lp;
							GSW_PMAC_CountGet(cdev, &Pmac_Count);
							printk("\t\t\t\t nPmacId                            = %u\n", Pmac_Count.nPmacId);
							printk("\t\t\t\t nTxDmaChanId                       = %u\n", Pmac_Count.nTxDmaChanId);
							printk("\t\t\t\t Ingress Discard Packet Count       = %u\n", Pmac_Count.nDiscPktsCount);
							printk("\t\t\t\t Ingress Discard Byte Count         = %u\n", Pmac_Count.nDiscBytesCount);
							printk("\t\t\t\t Egress Checksum Error Packet Count = %u\n", Pmac_Count.nChkSumErrPktsCount);
							printk("\t\t\t\t Egress Checksum Error Byte Count   = %u\n", Pmac_Count.nChkSumErrBytesCount);
							printk("\t\t\t\t Ingress Total Packet Count         = %u\n", Pmac_Count.nIngressPktsCount);
							printk("\t\t\t\t Ingress Total Byte Count           = %u\n", Pmac_Count.nIngressBytesCount);
							printk("\t\t\t\t Egress Total Packet Count          = %u\n", Pmac_Count.nEgressPktsCount);
							printk("\t\t\t\t Egress Total Byte Count            = %u\n", Pmac_Count.nEgressBytesCount);
						}
					}
				}
			}
		}

		if (!count)
			printk("\nIs Mapped to None !!!\n");
	}

	printk("\n");
	printk("NOTE:\n");
	printk("To Get PCE BYPASS Qmapping info,Please use the following command :\n");
	printk("switch_cli GSW_DEBUG_DEF_BYP_QMAP\n");

	printk("\n");
	printk("NOTE:\n");
	printk("To Get CTP to Logical port assignment statistics (for all logical ports) using following command :\n");
	printk("switch_cli GSW_DEBUG_LP_STATISTICS\n");
	return GSW_statusOk;
}

GSW_return_t GSW_Debug_CtpTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j, lp = 0, bp = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_ctp; j++) {
			if (gswdev->ctpportconfig_idx[j].IndexInUse) {
				lp = gswdev->ctpportconfig_idx[j].AssociatedLogicalPort;
				bp = gswdev->ctpportconfig_idx[j].BrdgPortId;

				if (lp == 0xFF)
					printk("CTP Port %d	= InUse, Associated to Logical port = None and Configured with BrdgPort Id %d.\n", j, bp);
				else
					printk("CTP Port %d	= InUse, Associated to Logical port = %d and Configured with BrdgPort Id %d.\n", j, lp, bp);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->ctpportconfig_idx[parm->nTableIndex].IndexInUse = 1;
		printk("\nCTP Port Table Index %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("\nCTP Port Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse							 = %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt						 = %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IndexInUsageCnt);

		lp = gswdev->ctpportconfig_idx[parm->nTableIndex].AssociatedLogicalPort;

		if (lp == 0xFF)
			printk("AssociatedLogicalPort				 = None\n");
		else
			printk("AssociatedLogicalPort				 = %d\n", lp);

		printk("BrdgIdPortAssigned						= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].BrdgIdPortAssigned);
		printk("BrdgPortId								= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].BrdgPortId);
		printk("IngressExVlanNonIgmpBlkAssigned			= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressExVlanNonIgmpBlkAssigned);
		printk("IngressExVlanIgmpBlkId					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressExVlanIgmpBlkId);
		printk("IngressExVlanIgmpBlkAssigned			= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressExVlanIgmpBlkAssigned);
		printk("IngressMeteringAssigned					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressMeteringAssigned);
		printk("IngressTrafficMeterId					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressTrafficMeterId);
		printk("IngressBridgeBypassPmapperAssigned		= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressBridgeBypassPmapperAssigned);
		printk("IngressBridgeBypassPmappperId			= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressBridgeBypassPmappperIdx);
		printk("EgressExVlanNonIgmpBlkAssigned			= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressExVlanNonIgmpBlkAssigned);
		printk("EgressExVlanNonIgmpBlkId				= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressExVlanNonIgmpBlkId);
		printk("EgressExVlanIgmpBlkAssigned				= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressExVlanIgmpBlkAssigned);
		printk("EgressExVlanIgmpBlkId					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressExVlanIgmpBlkId);
		printk("EgressMeteringAssigned					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressMeteringAssigned);
		printk("EgressTrafficMeterId					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].EgressTrafficMeterId);
		printk("IngressTflowAssigned					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressTflowAssigned);
		printk("IngressTflowFirstIdx					= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressTflowFirstIdx);
		printk("IngressTflowNumberOfEntrys				= %d\n", gswdev->ctpportconfig_idx[parm->nTableIndex].IngressTflowNumberOfEntrys);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_BrgPortTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j, b = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_bridge_port; j++) {
			if (gswdev->brdgeportconfig_idx[j].IndexInUse) {
				b = gswdev->brdgeportconfig_idx[j].BrdgId;
				printk("Bridge Port %d	= InUse, Configured with Brdg Id/Fid %d.\n", j, b);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->brdgeportconfig_idx[parm->nTableIndex].IndexInUse = 1;
		printk("\nBridge Port table Index %d is Forced to InUSe\n", parm->nTableIndex);

	} else {

		printk("\n");
		printk("BridgePort Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse							 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt						 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IndexInUsageCnt);
		printk("BrdgIdAssigned						 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].BrdgIdAssigned);
		printk("BrdgId								 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].BrdgId);
		printk("IngressExVlanBlkAssigned 			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IngressExVlanBlkAssigned);
		printk("IngressExVlanBlkId					 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IngressExVlanBlkId);
		printk("EgressExVlanBlkAssigned				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressExVlanBlkAssigned);
		printk("EgressExVlanBlkId					 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressExVlanBlkId);
		printk("IngressMeteringAssigned				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IngressMeteringAssigned);
		printk("IngressTrafficMeterId				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IngressTrafficMeterId);
		printk("EgressMeteringAssigned				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressMeteringAssigned);
		printk("EgressTrafficMeterId 				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressTrafficMeterId);
		printk("BroadcastMeteringAssigned			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].BroadcastMeteringAssigned);
		printk("BroadcastMeteringId					 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].BroadcastMeteringId);
		printk("MulticastMeteringAssigned			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].MulticastMeteringAssigned);
		printk("MulticastMeteringId					 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].MulticastMeteringId);
		printk("UnknownUniCastMeteringAssigned		 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownUniCastMeteringAssigned);
		printk("UnknownUniCastMeteringId 			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownUniCastMeteringId);
		printk("UnknownMultiIpMeteringAssigned		 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownMultiIpMeteringAssigned);
		printk("UnknownMultiIpMeteringId 			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownMultiIpMeteringId);
		printk("UnknownMultiNonIpMeteringAssigned	 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownMultiNonIpMeteringAssigned);
		printk("UnknownMultiNonIpMeteringId			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownMultiNonIpMeteringId);
		printk("PmapperAssigned						 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].PmapperAssigned);
		printk("PmappperIdx							 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].PmappperIdx);
		printk("IngressVlanFilterAssigned			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].IngressVlanFilterBlkId);
		printk("IngressVlanFilterBlkId				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].UnknownMultiNonIpMeteringId);
		printk("EgressVlanFilter1Assigned			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressVlanFilter1Assigned);
		printk("EgressVlanFilter1BlkId				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressVlanFilter1BlkId);
		printk("EgressVlanFilter2Assigned			 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressVlanFilter2Assigned);
		printk("EgressVlanFilter2BlkId				 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].EgressVlanFilter2BlkId);
		printk("StpState 							 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].StpState);
		printk("P8021xState							 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].P8021xState);
		printk("LearningLimit						 = %d\n", gswdev->brdgeportconfig_idx[parm->nTableIndex].LearningLimit);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_BrgTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_bridge_port; j++) {
			if (gswdev->brdgeconfig_idx[j].IndexInUse) {
				printk("Bridge  %d	= InUse.\n", j);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->brdgeconfig_idx[parm->nTableIndex].IndexInUse = 1;
		printk("Bridge Table Index %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("Brdg Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse							 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt						 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].IndexInUsageCnt);
		printk("BroadcastMeteringAssigned			 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].BroadcastMeteringAssigned);
		printk("BroadcastMeteringId					 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].BroadcastMeteringId);
		printk("MulticastMeteringAssigned			 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].MulticastMeteringAssigned);
		printk("MulticastMeteringId					 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].MulticastMeteringId);
		printk("UnknownUniCastMeteringAssigned		 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownUniCastMeteringAssigned);
		printk("UnknownUniCastMeteringId 			 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownUniCastMeteringId);
		printk("UnknownMultiIpMeteringAssigned		 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownMultiIpMeteringAssigned);
		printk("UnknownMultiIpMeteringId 			 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownMultiIpMeteringId);
		printk("UnknownMultiNonIpMeteringAssigned	 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownMultiNonIpMeteringAssigned);
		printk("UnknownMultiNonIpMeteringId			 = %d\n", gswdev->brdgeconfig_idx[parm->nTableIndex].UnknownMultiNonIpMeteringId);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_ExvlanTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j, blockid = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_extendvlan; j++) {
			if (gswdev->extendvlan_idx.vlan_idx[j].IndexInUse) {
				blockid = gswdev->extendvlan_idx.vlan_idx[j].VlanBlockId;
				printk("ExVlan Table Index  %d = InUse, Associated to ExVlan Block Id %d.\n", j, blockid);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].IndexInUse = 1;
		gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].VlanBlockId = parm->nblockid;
		gswdev->extendvlan_idx.nUsedEntry++;
		printk("Exvlan Table Index  %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("Exvlan Table Index %d:\n\n", parm->nTableIndex);
		printk("BlockId						= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].VlanBlockId);
		printk("IndexInUse					= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt				= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].IndexInUsageCnt);
		printk("Dscp2PcpPointerAssigned		= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].Dscp2PcpPointerAssigned);
		printk("Dscp2PcpPointer				= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].Dscp2PcpPointer);
		printk("MeterAssigned				= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].MeterAssigned);
		printk("MeterId						= %d\n", gswdev->extendvlan_idx.vlan_idx[parm->nTableIndex].MeterId);
		printk("Number Exvlan table entry used	= %d\n", gswdev->extendvlan_idx.nUsedEntry);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_VlanFilterTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j, blockid = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_vlanfilter; j++) {
			if (gswdev->vlanfilter_idx.filter_idx[j].IndexInUse) {
				blockid = gswdev->vlanfilter_idx.filter_idx[j].FilterBlockId;
				printk("Vlan Filter Table Index  %d = InUse, Associated to Vlan Filetr Block Id %d.\n", j, blockid);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].IndexInUse = 1;
		gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].FilterBlockId = parm->nblockid;
		gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].DiscardUntagged = parm->nDiscardUntagged;
		gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].DiscardUnMatchedTagged = parm->nDiscardUnMatchedTagged;
		gswdev->vlanfilter_idx.nUsedEntry++;
		printk("Vlan Filter Table Index  %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("Vlan Filter Table Index %d:\n\n", parm->nTableIndex);
		printk("FilterBlockId			= %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].FilterBlockId);
		printk("IndexInUse				= %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt			= %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].IndexInUsageCnt);
		printk("DiscardUntagged			= %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].DiscardUntagged);
		printk("DiscardUnMatchedTagged  = %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].DiscardUnMatchedTagged);
		printk("FilterMask				= %d\n", gswdev->vlanfilter_idx.filter_idx[parm->nTableIndex].FilterMask);
		printk("Number Vlan Filter table entry used = %d\n", gswdev->vlanfilter_idx.nUsedEntry);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_MeterTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_meters; j++) {
			if (gswdev->meter_idx[parm->nTableIndex].IndexInUse) {
				printk("Meter Table Index  %d = InUse.\n", j);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->meter_idx[parm->nTableIndex].IndexInUse = 1;
		printk("Meter Table Index  %d is Forced to InUSe\n", parm->nTableIndex);

	} else {
		printk("Meter Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse			   = %d\n", gswdev->meter_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt		   = %d\n", gswdev->meter_idx[parm->nTableIndex].IndexInUsageCnt);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_Dscp2PcpTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nForceSet) {
		gswdev->dscp2pcp_idx[parm->nTableIndex].IndexInUse = 1;
		printk("Dscp2Pcp Table Index  %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("Dscp2Pcp Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse							 = %d\n", gswdev->dscp2pcp_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt						 = %d\n", gswdev->dscp2pcp_idx[parm->nTableIndex].IndexInUsageCnt);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_PmapperTableStatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 j;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	printk("\n");

	if (parm->nCheckIndexInUse) {
		for (j = 0; j < gswdev->num_of_pmapper; j++) {
			if (gswdev->pmapper_idx[j].IndexInUse) {
				printk("P-Mapper Table Index  %d = InUse.\n", j);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->pmapper_idx[parm->nTableIndex].IndexInUse = 1;
		printk("P-Mapper Table Index  %d is Forced to InUSe\n", parm->nTableIndex);
	} else {
		printk("P-Mapper Table Index %d:\n\n", parm->nTableIndex);
		printk("IndexInUse			   = %d\n", gswdev->pmapper_idx[parm->nTableIndex].IndexInUse);
		printk("IndexInUsageCnt		   = %d\n", gswdev->pmapper_idx[parm->nTableIndex].IndexInUsageCnt);
	}

	return GSW_statusOk;
}

GSW_return_t GSW_Debug_PmacBpTable(void *cdev, GSW_debug_t *parm)
{
	pmac_get_bp_cfg(cdev, parm->nPmacId);
	return GSW_statusOk;
}
GSW_return_t GSW_Debug_PmacEgTable(void *cdev, GSW_debug_t *parm)
{
	pmac_get_eg_cfg(cdev, parm->nPmacId, parm->nDestPort);
	return GSW_statusOk;
}
GSW_return_t GSW_Debug_PmacIgTable(void *cdev, GSW_debug_t *parm)
{
	pmac_get_ig_cfg(cdev, parm->nPmacId);
	return GSW_statusOk;
}
GSW_return_t GSW_Debug_PceBypassTable(void *cdev, GSW_debug_t *parm)
{
	gsw_get_def_bypass_qmap(cdev);
	return GSW_statusOk;
}
GSW_return_t GSW_Debug_PceQTable(void *cdev, GSW_debug_t *parm)
{
	gsw_get_def_pce_qmap(cdev);
	return GSW_statusOk;

}

GSW_return_t GSW_XgmacCfg(void *cdev, GSW_MAC_Cli_t *mac_cfg)
{
	struct mac_ops *ops = NULL;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		pr_err("XGMAC Cfg Supported only in GSWIP3.1 and above\n");
		return GSW_statusErr;
	}

	ops = gsw_get_mac_ops(0, MAC_2);

	if (!ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ret = ops->xgmac_cli(mac_cfg);

	return ret;
}

GSW_return_t GSW_GswssCfg(void *cdev, GSW_MAC_Cli_t *mac_cfg)
{
	struct adap_ops *ops = NULL;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	int ret = 0;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		pr_err("GSWSS Cfg Supported only in GSWIP3.1 and above\n");
		return GSW_statusErr;
	}


	ops = gsw_get_adap_ops(0);

	if (!ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ret = ops->ss_cli(mac_cfg);

	return ret;
}

GSW_return_t GSW_LmacCfg(void *cdev, GSW_MAC_Cli_t *mac_cfg)
{
	struct mac_ops *ops = NULL;
	int ret = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		pr_err("LMAC Cfg Supported only in GSWIP3.1 and above\n");
		return GSW_statusErr;
	}

	ops = gsw_get_mac_ops(0, MAC_2);

	if (!ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ret = ops->lmac_cli(mac_cfg);

	return ret;
}


GSW_return_t GSW_MacsecCfg(void *cdev, GSW_MAC_cfg_t *macsec_cfg)
{
#if 0
	u8 *argv[10];
	int i = 0;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	struct macsec_ops *ops = NULL;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_NOT_31(gswdev->gipver)) {
		pr_err("Macsec Cfg Supported only in GSWIP3.1 and above\n");
		return GSW_statusErr;
	}

	if (macsec_cfg->devid > 1) {
		pr_err("Invalid Command Name or Params\n");
		return GSW_statusErr;
	}

	ops = gsw_get_macsec_ops(macsec_cfg->devid);

	if (!ops) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	ops->macsec_cli(ops, macsec_cfg);

#endif
	return GSW_statusOk;
}

GSW_return_t gsw_debug_tflow_tablestatus(void *cdev, GSW_debug_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 i, blockid = 0;

	if (!gswdev) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	pr_info("\n");

	if (parm->nCheckIndexInUse) {
		for (i = 0; i < gswdev->tftblsize; i++) {
			if (gswdev->tflow_idx.flow_idx[i].indexinuse) {
				blockid = gswdev->tflow_idx.flow_idx[i].tflowblockid;
				pr_info("TFLOW Table Index  %d = InUse, Associated to TFLOW Block Id %d.\n",
					i, blockid);
			}
		}

		return GSW_statusOk;
	}

	if (parm->nForceSet) {
		gswdev->tflow_idx.flow_idx[parm->nTableIndex].indexinuse = 1;
		gswdev->tflow_idx.flow_idx[parm->nTableIndex].tflowblockid = parm->nblockid;
		gswdev->tflow_idx.usedentry++;
		pr_info("TFLOW Table Index  %d is Forced to InUSe\n",
			parm->nTableIndex);
	} else {
		pr_info("TFLOW  Table Index %d:\n\n",
			parm->nTableIndex);
		pr_info("BlockId						= %d\n",
			gswdev->tflow_idx.flow_idx[parm->nTableIndex].tflowblockid);
		pr_info("IndexInUse					= %d\n",
			gswdev->tflow_idx.flow_idx[parm->nTableIndex].indexinuse);
		pr_info("IndexInUsageCnt				= %d\n",
			gswdev->tflow_idx.flow_idx[parm->nTableIndex].indexinusagecnt);
	}

	return GSW_statusOk;
}
