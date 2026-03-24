/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ltq_cli_lib.h"

#ifndef IOCTL_PCTOOL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#endif

#define MAX_NUM_OF_DISPLAY_PORTS 12

int xgmac_main(u32 argc, char *argv[], void *fd);
int gswss_main(u32 argc, char *argv[], void *fd);
int lmac_main(u32 argc, char *argv[], void *fd);

int gsw_8021x_eapol_rule_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_8021X_EAPOL_Rule_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_8021X_EAPOL_Rule_t));

	if (cli_ioctl(fd, GSW_8021X_EAPOL_RULE_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eForwardPort", sizeof(param.eForwardPort), &param.eForwardPort);
	cnt += scanParamArg(argc, argv, "nForwardPortId", sizeof(param.nForwardPortId), &param.nForwardPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_8021X_EAPOL_RULE_SET, &param);
}

int gsw_8021x_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_8021X_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_8021X_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_8021X_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eState", sizeof(param.eState), &param.eState);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_8021X_PORT_CFG_SET, &param);
}

int gsw_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_cfg_t));

	if (cli_ioctl(fd, GSW_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eMAC_TableAgeTimer", sizeof(param.eMAC_TableAgeTimer), &param.eMAC_TableAgeTimer);
	cnt += scanParamArg(argc, argv, "nAgeTimer", sizeof(param.nAgeTimer), &param.nAgeTimer);
	cnt += scanParamArg(argc, argv, "bVLAN_Aware", sizeof(param.bVLAN_Aware), &param.bVLAN_Aware);
	cnt += scanParamArg(argc, argv, "nMaxPacketLen", sizeof(param.nMaxPacketLen), &param.nMaxPacketLen);
	cnt += scanParamArg(argc, argv, "bLearningLimitAction", sizeof(param.bLearningLimitAction), &param.bLearningLimitAction);
	cnt += scanParamArg(argc, argv, "bPauseMAC_ModeSrc", sizeof(param.bPauseMAC_ModeSrc), &param.bPauseMAC_ModeSrc);
	cnt += scanMAC_Arg(argc, argv, "nPauseMAC_Src", param.nPauseMAC_Src);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_CFG_SET, &param);
}

int gsw_cpu_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CPU_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_CPU_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	//if (cnt != 1)
	//	return (-3);

	if (cli_ioctl(fd, GSW_CPU_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bCPU_PortValid", sizeof(param.bCPU_PortValid), &param.bCPU_PortValid);
	cnt += scanParamArg(argc, argv, "bSpecialTagIngress", sizeof(param.bSpecialTagIngress), &param.bSpecialTagIngress);
	cnt += scanParamArg(argc, argv, "bSpecialTagEgress", sizeof(param.bSpecialTagEgress), &param.bSpecialTagEgress);
	cnt += scanParamArg(argc, argv, "bFcsCheck", sizeof(param.bFcsCheck), &param.bFcsCheck);
	cnt += scanParamArg(argc, argv, "bFcsGenerate", sizeof(param.bFcsGenerate), &param.bFcsGenerate);
	cnt += scanParamArg(argc, argv, "bSpecialTagEthType", sizeof(param.bSpecialTagEthType), &param.bSpecialTagEthType);
	cnt += scanParamArg(argc, argv, "bTsPtp", sizeof(param.bTsPtp), &param.bTsPtp);
	cnt += scanParamArg(argc, argv, "bTsNonptp", sizeof(param.bTsNonptp), &param.bTsNonptp);

	cnt += scanParamArg(argc, argv, "eNoMPEParserCfg", sizeof(param.eNoMPEParserCfg), &param.eNoMPEParserCfg);
	cnt += scanParamArg(argc, argv, "eMPE1ParserCfg", sizeof(param.eMPE1ParserCfg), &param.eMPE1ParserCfg);
	cnt += scanParamArg(argc, argv, "eMPE2ParserCfg", sizeof(param.eMPE2ParserCfg), &param.eMPE2ParserCfg);
	cnt += scanParamArg(argc, argv, "eMPE1MPE2ParserCfg", sizeof(param.eMPE1MPE2ParserCfg), &param.eMPE1MPE2ParserCfg);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_CPU_PORT_CFG_SET, &param);
}

int gsw_disable(int argc, char *argv[], void *fd, int numPar)
{
	return cli_ioctl(fd, GSW_DISABLE, 0);
}

int gsw_enable(int argc, char *argv[], void *fd, int numPar)
{
	return cli_ioctl(fd, GSW_ENABLE, 0);
}

int gsw_hw_init(int argc, char *argv[], void *fd, int numPar)
{
	GSW_HW_Init_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_HW_Init_t));

	cnt += scanParamArg(argc, argv, "eInitMode", sizeof(param.eInitMode), &param.eInitMode);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_HW_INIT, &param);
}

int gsw_mac_table_clear(int argc, char *argv[], void *fd, int numPar)
{
	return cli_ioctl(fd, GSW_MAC_TABLE_CLEAR, 0);
}

int gsw_mac_table_entry_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_tableAdd_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_MAC_tableAdd_t));

	cnt += scanParamArg(argc, argv, "nFId", sizeof(sVar.nFId), &sVar.nFId);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);
	cnt += scanParamArg(argc, argv, "nAgeTimer", sizeof(sVar.nAgeTimer), &sVar.nAgeTimer);
	cnt += scanParamArg(argc, argv, "bStaticEntry", sizeof(sVar.bStaticEntry), &sVar.bStaticEntry);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(sVar.nTrafficClass), &sVar.nTrafficClass);
	cnt += scanParamArg(argc, argv, "bIgmpControlled", sizeof(sVar.bIgmpControlled), &sVar.bIgmpControlled);
	cnt += scanParamArg(argc, argv, "nFilterFlag", sizeof(sVar.nFilterFlag), &sVar.nFilterFlag);
	cnt += scanParamArg(argc, argv, "nSVLAN_Id", sizeof(sVar.bIgmpControlled), &sVar.nSVLAN_Id);
	cnt += scanParamArg(argc, argv, "nSubIfId", sizeof(sVar.nSubIfId), &sVar.nSubIfId);
	cnt += scanMAC_Arg(argc, argv, "nMAC", sVar.nMAC);

	if (gswip_version == GSWIP_3_2) {
		cnt += scanMAC_Arg(argc, argv, "nAssociatedMAC", sVar.nAssociatedMAC);
	}

	cnt += scanParamArg(argc, argv, "nPortMapValueIndex0", sizeof(sVar.nPortMap[0]), &sVar.nPortMap[0]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex1", sizeof(sVar.nPortMap[1]), &sVar.nPortMap[1]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex2", sizeof(sVar.nPortMap[2]), &sVar.nPortMap[2]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex3", sizeof(sVar.nPortMap[3]), &sVar.nPortMap[3]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex4", sizeof(sVar.nPortMap[4]), &sVar.nPortMap[4]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex5", sizeof(sVar.nPortMap[5]), &sVar.nPortMap[5]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex6", sizeof(sVar.nPortMap[6]), &sVar.nPortMap[6]);
	cnt += scanParamArg(argc, argv, "nPortMapValueIndex7", sizeof(sVar.nPortMap[7]), &sVar.nPortMap[7]);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MAC_TABLE_ENTRY_ADD, &sVar);
}

int gsw_mac_table_entry_remove(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_tableRemove_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MAC_tableRemove_t));

	cnt += scanParamArg(argc, argv, "nFId", sizeof(param.nFId), &param.nFId);
	cnt += scanMAC_Arg(argc, argv, "nMAC", param.nMAC);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MAC_TABLE_ENTRY_REMOVE, &param);
}

int gsw_mdio_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MDIO_cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MDIO_cfg_t));

	if (cli_ioctl(fd, GSW_MDIO_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nMDIO_Speed", sizeof(param.nMDIO_Speed), &param.nMDIO_Speed);
	cnt += scanParamArg(argc, argv, "bMDIO_Enable", sizeof(param.bMDIO_Enable), &param.bMDIO_Enable);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MDIO_CFG_SET, &param);
}

int gsw_mdio_data_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MDIO_data_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MDIO_data_t));

	cnt += scanParamArg(argc, argv, "nAddressDev", sizeof(param.nAddressDev), &param.nAddressDev);
	cnt += scanParamArg(argc, argv, "nAddressReg", sizeof(param.nAddressReg), &param.nAddressReg);
	cnt += scanParamArg(argc, argv, "nData", sizeof(param.nData), &param.nData);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MDIO_DATA_WRITE, &param);
}

int gsw_mmd_data_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MMD_data_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MMD_data_t));

	cnt += scanParamArg(argc, argv, "nAddressDev", sizeof(param.nAddressDev), &param.nAddressDev);
	cnt += scanParamArg(argc, argv, "nAddressReg", sizeof(param.nAddressReg), &param.nAddressReg);
	cnt += scanParamArg(argc, argv, "nData", sizeof(param.nData), &param.nData);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MMD_DATA_WRITE, &param);
}

int gsw_monitor_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_monitorPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_monitorPortCfg_t));

	if (cli_ioctl(fd, GSW_MONITOR_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	cnt += scanParamArg(argc, argv, "bMonitorPort", sizeof(param.bMonitorPort), &param.bMonitorPort);
	cnt += scanParamArg(argc, argv, "nSubIfId",     sizeof(param.nSubIfId), &param.nSubIfId);

	if (gswip_version == GSWIP_3_2) {
		cnt += scanParamArg(argc, argv, "bEnableIngressMirror", sizeof(param.bEnableIngressMirror),
				    &param.bEnableIngressMirror);
		cnt += scanParamArg(argc, argv, "bEnableEgressMirror", sizeof(param.bEnableEgressMirror),
				    &param.bEnableEgressMirror);
		cnt += scanParamArg(argc, argv, "nMonitorLpId", sizeof(param.nMonitorLpId), &param.nMonitorLpId);
		cnt += scanParamArg(argc, argv, "bEnableMonitorQmap", sizeof(param.bEnableMonitorQmap), &param.bEnableMonitorQmap);
		cnt += scanParamArg(argc, argv, "nMonitorQid", sizeof(param.nMonitorQid), &param.nMonitorQid);
		cnt += scanParamArg(argc, argv, "nRedirectPortId", sizeof(param.nRedirectPortId), &param.nRedirectPortId);
	}

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MONITOR_PORT_CFG_SET, &param);
}

int gsw_multicast_router_port_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastRouter_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_multicastRouter_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MULTICAST_ROUTER_PORT_ADD, &param);
}

int gsw_multicast_router_port_remove(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastRouter_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_multicastRouter_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MULTICAST_ROUTER_PORT_REMOVE, &param);
}

int gsw_multicast_snoop_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastSnoopCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_multicastSnoopCfg_t));

	if (cli_ioctl(fd, GSW_MULTICAST_SNOOP_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eIGMP_Mode", sizeof(param.eIGMP_Mode), &param.eIGMP_Mode);
	cnt += scanParamArg(argc, argv, "bIGMPv3", sizeof(param.bIGMPv3), &param.bIGMPv3);
	cnt += scanParamArg(argc, argv, "bCrossVLAN", sizeof(param.bCrossVLAN), &param.bCrossVLAN);
	cnt += scanParamArg(argc, argv, "eForwardPort", sizeof(param.eForwardPort), &param.eForwardPort);
	cnt += scanParamArg(argc, argv, "nForwardPortId", sizeof(param.nForwardPortId), &param.nForwardPortId);
	cnt += scanParamArg(argc, argv, "nClassOfService", sizeof(param.nClassOfService), &param.nClassOfService);
	cnt += scanParamArg(argc, argv, "nRobust", sizeof(param.nRobust), &param.nRobust);
	cnt += scanParamArg(argc, argv, "nQueryInterval", sizeof(param.nQueryInterval), &param.nQueryInterval);
	cnt += scanParamArg(argc, argv, "eSuppressionAggregation", sizeof(param.eSuppressionAggregation), &param.eSuppressionAggregation);
	cnt += scanParamArg(argc, argv, "bFastLeave", sizeof(param.bFastLeave), &param.bFastLeave);
	cnt += scanParamArg(argc, argv, "bLearningRouter", sizeof(param.bLearningRouter), &param.bLearningRouter);
	cnt += scanParamArg(argc, argv, "bMulticastUnknownDrop", sizeof(param.bMulticastUnknownDrop), &param.bMulticastUnknownDrop);
	cnt += scanParamArg(argc, argv, "bMulticastFIDmode", sizeof(param.bMulticastFIDmode), &param.bMulticastFIDmode);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_MULTICAST_SNOOP_CFG_SET, &param);
}

int gsw_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "ePortType", sizeof(param.ePortType), &param.ePortType);
	cnt += scanParamArg(argc, argv, "eEnable", sizeof(param.eEnable), &param.eEnable);
	cnt += scanParamArg(argc, argv, "bUnicastUnknownDrop", sizeof(param.bUnicastUnknownDrop), &param.bUnicastUnknownDrop);
	cnt += scanParamArg(argc, argv, "bMulticastUnknownDrop", sizeof(param.bMulticastUnknownDrop), &param.bMulticastUnknownDrop);
	cnt += scanParamArg(argc, argv, "bReservedPacketDrop", sizeof(param.bReservedPacketDrop), &param.bReservedPacketDrop);
	cnt += scanParamArg(argc, argv, "bBroadcastDrop", sizeof(param.bBroadcastDrop), &param.bBroadcastDrop);
	cnt += scanParamArg(argc, argv, "bAging", sizeof(param.bAging), &param.bAging);
	cnt += scanParamArg(argc, argv, "bLearning", sizeof(param.bLearning), &param.bLearning);
	cnt += scanParamArg(argc, argv, "bLearningMAC_PortLock", sizeof(param.bLearningMAC_PortLock), &param.bLearningMAC_PortLock);
	cnt += scanParamArg(argc, argv, "nLearningLimit", sizeof(param.nLearningLimit), &param.nLearningLimit);
	cnt += scanParamArg(argc, argv, "bMAC_SpoofingDetection", sizeof(param.bMAC_SpoofingDetection), &param.bMAC_SpoofingDetection);
	cnt += scanParamArg(argc, argv, "eFlowCtrl", sizeof(param.eFlowCtrl), &param.eFlowCtrl);
	cnt += scanParamArg(argc, argv, "ePortMonitor", sizeof(param.ePortMonitor), &param.ePortMonitor);
	cnt += scanParamArg(argc, argv, "bIfCounters", sizeof(param.bIfCounters), &param.bIfCounters);
	cnt += scanParamArg(argc, argv, "nIfCountStartIdx", sizeof(param.nIfCountStartIdx), &param.nIfCountStartIdx);
	cnt += scanParamArg(argc, argv, "eIfRMONmode", sizeof(param.eIfRMONmode), &param.eIfRMONmode);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_PORT_CFG_SET, &param);
}

int gsw_port_link_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portLinkCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portLinkCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_PORT_LINK_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bDuplexForce", sizeof(param.bDuplexForce), &param.bDuplexForce);
	cnt += scanParamArg(argc, argv, "eDuplex", sizeof(param.eDuplex), &param.eDuplex);
	cnt += scanParamArg(argc, argv, "bSpeedForce", sizeof(param.bSpeedForce), &param.bSpeedForce);
	cnt += scanParamArg(argc, argv, "eSpeed", sizeof(param.eSpeed), &param.eSpeed);
	cnt += scanParamArg(argc, argv, "bLinkForce", sizeof(param.bLinkForce), &param.bLinkForce);
	cnt += scanParamArg(argc, argv, "eLink", sizeof(param.eLink), &param.eLink);
	cnt += scanParamArg(argc, argv, "eMII_Mode", sizeof(param.eMII_Mode), &param.eMII_Mode);
	cnt += scanParamArg(argc, argv, "eMII_Type", sizeof(param.eMII_Type), &param.eMII_Type);
	cnt += scanParamArg(argc, argv, "eClkMode", sizeof(param.eClkMode), &param.eClkMode);
	cnt += scanParamArg(argc, argv, "bLPI", sizeof(param.bLPI), &param.bLPI);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_PORT_LINK_CFG_SET, &param);
}

int gsw_port_redirect_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portRedirectCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portRedirectCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_PORT_REDIRECT_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bRedirectEgress", sizeof(param.bRedirectEgress), &param.bRedirectEgress);
	cnt += scanParamArg(argc, argv, "bRedirectIngress", sizeof(param.bRedirectIngress), &param.bRedirectIngress);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_PORT_REDIRECT_SET, &param);
}

int gsw_port_rgmii_clk_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portRGMII_ClkCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portRGMII_ClkCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_PORT_RGMII_CLK_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nDelayRx", sizeof(param.nDelayRx), &param.nDelayRx);
	cnt += scanParamArg(argc, argv, "nDelayTx", sizeof(param.nDelayTx), &param.nDelayTx);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_PORT_RGMII_CLK_CFG_SET, &param);
}

int gsw_qos_meter_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterCfg_t param;
	memset(&param, 0, sizeof(GSW_QoS_meterCfg_t));

		if (cli_ioctl(fd, GSW_QOS_METER_ALLOC, &param) != 0)
		return (-1);

	printf("\n\t Allocated nMeterId                        = %u\n", param.nMeterId);
	return 0;
}

int gsw_qos_meter_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_meterCfg_t));

	cnt += scanParamArg(argc, argv, "nMeterId", 32, &param.nMeterId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_METER_FREE, &param) != 0)
		return (-1);

	printf("\n\t Freed nMeterIdId                        = %u\n", param.nMeterId);
	return 0;
}

int gsw_qos_meter_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_meterCfg_t));

	cnt += scanParamArg(argc, argv, "nMeterId", 32, &param.nMeterId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_METER_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bEnable", sizeof(param.bEnable), &param.bEnable);
	cnt += scanParamArg(argc, argv, "eMtrType", sizeof(param.eMtrType), &param.eMtrType);
	cnt += scanParamArg(argc, argv, "nCbs", sizeof(param.nCbs), &param.nCbs);
	cnt += scanParamArg(argc, argv, "nEbs", sizeof(param.nEbs), &param.nEbs);
	cnt += scanParamArg(argc, argv, "nRate", sizeof(param.nRate), &param.nRate);
	cnt += scanParamArg(argc, argv, "nPiRate", sizeof(param.nPiRate), &param.nPiRate);
	cnt += scanParamArg(argc, argv, "cMeterName", sizeof(param.cMeterName), &param.cMeterName);
	cnt += scanParamArg(argc, argv, "nColourBlindMode", sizeof(param.nColourBlindMode), &param.nColourBlindMode);
	cnt += scanParamArg(argc, argv, "bPktMode", sizeof(param.bPktMode), &param.bPktMode);
	cnt += scanParamArg(argc, argv, "bLocalOverhd", sizeof(param.bLocalOverhd), &param.bLocalOverhd);
	cnt += scanParamArg(argc, argv, "nLocaloverhd", sizeof(param.nLocaloverhd), &param.nLocaloverhd);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_METER_CFG_SET, &param);
}

int gsw_qos_meter_port_assign(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterPort_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_meterPort_t));

	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);
	cnt += scanParamArg(argc, argv, "eDir", sizeof(param.eDir), &param.eDir);
	cnt += scanParamArg(argc, argv, "nPortIngressId", sizeof(param.nPortIngressId), &param.nPortIngressId);
	cnt += scanParamArg(argc, argv, "nPortEgressId", sizeof(param.nPortEgressId), &param.nPortEgressId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_METER_PORT_ASSIGN, &param);
}

int gsw_qos_meter_port_deassign(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterPort_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_meterPort_t));

	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);
	cnt += scanParamArg(argc, argv, "eDir", sizeof(param.eDir), &param.eDir);
	cnt += scanParamArg(argc, argv, "nPortIngressId", sizeof(param.nPortIngressId), &param.nPortIngressId);
	cnt += scanParamArg(argc, argv, "nPortEgressId", sizeof(param.nPortEgressId), &param.nPortEgressId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_METER_PORT_DEASSIGN, &param);
}

int gsw_qos_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eClassMode", sizeof(param.eClassMode), &param.eClassMode);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(param.nTrafficClass), &param.nTrafficClass);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_PORT_CFG_SET, &param);
}

int gsw_qos_port_remarking_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_portRemarkingCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_portRemarkingCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_PORT_REMARKING_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eDSCP_IngressRemarkingEnable", sizeof(param.eDSCP_IngressRemarkingEnable), &param.eDSCP_IngressRemarkingEnable);
	cnt += scanParamArg(argc, argv, "bDSCP_EgressRemarkingEnable", sizeof(param.bDSCP_EgressRemarkingEnable), &param.bDSCP_EgressRemarkingEnable);
	cnt += scanParamArg(argc, argv, "bPCP_IngressRemarkingEnable", sizeof(param.bPCP_IngressRemarkingEnable), &param.bPCP_IngressRemarkingEnable);
	cnt += scanParamArg(argc, argv, "bPCP_EgressRemarkingEnable", sizeof(param.bPCP_EgressRemarkingEnable), &param.bPCP_EgressRemarkingEnable);
//#ifdef SWAPI_ETC_CHIP
	cnt += scanParamArg(argc, argv, "bSTAG_PCP_IngressRemarkingEnable", sizeof(param.bSTAG_PCP_IngressRemarkingEnable), &param.bSTAG_PCP_IngressRemarkingEnable);
	cnt += scanParamArg(argc, argv, "bSTAG_DEI_IngressRemarkingEnable", sizeof(param.bSTAG_DEI_IngressRemarkingEnable), &param.bSTAG_DEI_IngressRemarkingEnable);
	cnt += scanParamArg(argc, argv, "bSTAG_PCP_DEI_EgressRemarkingEnable", sizeof(param.bSTAG_PCP_DEI_EgressRemarkingEnable), &param.bSTAG_PCP_DEI_EgressRemarkingEnable);

//#endif /* SWAPI_ETC_CHIP */
	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_PORT_REMARKING_CFG_SET, &param);
}

int gsw_qos_queue_port_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_queuePort_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_queuePort_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "nTrafficClassId", sizeof(param.nTrafficClassId), &param.nTrafficClassId);
	cnt += scanParamArg(argc, argv, "bRedirectionBypass", sizeof(param.bRedirectionBypass), &param.bRedirectionBypass);
	cnt += scanParamArg(argc, argv, "bExtrationEnable", sizeof(param.bExtrationEnable), &param.bExtrationEnable);

	if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eQMapMode", sizeof(param.eQMapMode), &param.eQMapMode);
	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);
	cnt += scanParamArg(argc, argv, "nRedirectPortId", sizeof(param.nRedirectPortId), &param.nRedirectPortId);

	if (gswip_version == GSWIP_3_2) {
		cnt += scanParamArg(argc, argv, "bEnableIngressPceBypass", sizeof(param.bEnableIngressPceBypass),
				    &param.bEnableIngressPceBypass);
		cnt += scanParamArg(argc, argv, "bReservedPortMode", sizeof(param.bReservedPortMode),
				    &param.bReservedPortMode);
	}

	if (cnt != numPar)
		return (-3);

	return cli_ioctl(fd, GSW_QOS_QUEUE_PORT_SET, &param);
}

int gsw_qos_scheduler_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_schedulerCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_schedulerCfg_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_SCHEDULER_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eType", sizeof(param.eType), &param.eType);
	cnt += scanParamArg(argc, argv, "nWeight", sizeof(param.nWeight), &param.nWeight);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_SCHEDULER_CFG_SET, &param);
}

int gsw_qos_shaper_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ShaperCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_ShaperCfg_t));

	cnt += scanParamArg(argc, argv, "nRateShaperId", sizeof(param.nRateShaperId), &param.nRateShaperId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_SHAPER_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bEnable", sizeof(param.bEnable), &param.bEnable);
	cnt += scanParamArg(argc, argv, "nCbs", sizeof(param.nCbs), &param.nCbs);
	cnt += scanParamArg(argc, argv, "nRate", sizeof(param.nRate), &param.nRate);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_SHAPER_CFG_SET, &param);
}

int gsw_qos_shaper_queue_assign(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ShaperQueue_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_ShaperQueue_t));

	cnt += scanParamArg(argc, argv, "nRateShaperId", sizeof(param.nRateShaperId), &param.nRateShaperId);
	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_SHAPER_QUEUE_ASSIGN, &param);
}

int gsw_qos_shaper_queue_deassign(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ShaperQueue_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_ShaperQueue_t));

	cnt += scanParamArg(argc, argv, "nRateShaperId", sizeof(param.nRateShaperId), &param.nRateShaperId);
	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_SHAPER_QUEUE_DEASSIGN, &param);
}

int gsw_qos_storm_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_stormCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_stormCfg_t));

	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);
	cnt += scanParamArg(argc, argv, "bBroadcast", sizeof(param.bBroadcast), &param.bBroadcast);
	cnt += scanParamArg(argc, argv, "bMulticast", sizeof(param.bMulticast), &param.bMulticast);
	cnt += scanParamArg(argc, argv, "bUnknownUnicast", sizeof(param.bUnknownUnicast), &param.bUnknownUnicast);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_STORM_CFG_SET, &param);
}

int gsw_qos_wred_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_Cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_WRED_Cfg_t));

	if (cli_ioctl(fd, GSW_QOS_WRED_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eProfile", sizeof(param.eProfile), &param.eProfile);
	cnt += scanParamArg(argc, argv, "eMode", sizeof(param.eMode), &param.eMode);
	cnt += scanParamArg(argc, argv, "eThreshMode", sizeof(param.eThreshMode), &param.eThreshMode);
	cnt += scanParamArg(argc, argv, "nRed_Min", sizeof(param.nRed_Min), &param.nRed_Min);
	cnt += scanParamArg(argc, argv, "nRed_Max", sizeof(param.nRed_Max), &param.nRed_Max);
	cnt += scanParamArg(argc, argv, "nYellow_Min", sizeof(param.nYellow_Min), &param.nYellow_Min);
	cnt += scanParamArg(argc, argv, "nYellow_Max", sizeof(param.nYellow_Max), &param.nYellow_Max);
	cnt += scanParamArg(argc, argv, "nGreen_Min", sizeof(param.nGreen_Min), &param.nGreen_Min);
	cnt += scanParamArg(argc, argv, "nGreen_Max", sizeof(param.nGreen_Max), &param.nGreen_Max);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_WRED_CFG_SET, &param);
}

int gsw_qos_wred_queue_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_QueueCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_WRED_QueueCfg_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_WRED_QUEUE_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nRed_Min", sizeof(param.nRed_Min), &param.nRed_Min);
	cnt += scanParamArg(argc, argv, "nRed_Max", sizeof(param.nRed_Max), &param.nRed_Max);
	cnt += scanParamArg(argc, argv, "nYellow_Min", sizeof(param.nYellow_Min), &param.nYellow_Min);
	cnt += scanParamArg(argc, argv, "nYellow_Max", sizeof(param.nYellow_Max), &param.nYellow_Max);
	cnt += scanParamArg(argc, argv, "nGreen_Min", sizeof(param.nGreen_Min), &param.nGreen_Min);
	cnt += scanParamArg(argc, argv, "nGreen_Max", sizeof(param.nGreen_Max), &param.nGreen_Max);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_WRED_QUEUE_CFG_SET, &param);
}

int gsw_rmon_clear(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_clear_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_RMON_clear_t));

	cnt += scanParamArg(argc, argv, "nRmonId", sizeof(param.nRmonId), &param.nRmonId);
	cnt += scanParamArg(argc, argv, "eRmonType", sizeof(param.eRmonType), &param.eRmonType);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_RMON_CLEAR, &param);
}

int gsw_stp_bpdu_rule_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_STP_BPDU_Rule_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_STP_BPDU_Rule_t));

	if (cli_ioctl(fd, GSW_STP_BPDU_RULE_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "eForwardPort", sizeof(param.eForwardPort), &param.eForwardPort);
	cnt += scanParamArg(argc, argv, "nForwardPortId", sizeof(param.nForwardPortId), &param.nForwardPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_STP_BPDU_RULE_SET, &param);
}

int gsw_stp_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_STP_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_STP_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_STP_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "ePortState", sizeof(param.ePortState), &param.ePortState);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_STP_PORT_CFG_SET, &param);
}

int gsw_vlan_id_create(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_IdCreate_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_IdCreate_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);
	cnt += scanParamArg(argc, argv, "nFId", sizeof(param.nFId), &param.nFId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_ID_CREATE, &param);
}

int gsw_vlan_id_delete(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_IdDelete_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_IdDelete_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_ID_DELETE, &param);
}

int gsw_vlan_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_VLAN_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nPortVId", sizeof(param.nPortVId), &param.nPortVId);
	cnt += scanParamArg(argc, argv, "bVLAN_UnknownDrop", sizeof(param.bVLAN_UnknownDrop), &param.bVLAN_UnknownDrop);
	cnt += scanParamArg(argc, argv, "bVLAN_ReAssign", sizeof(param.bVLAN_ReAssign), &param.bVLAN_ReAssign);
	cnt += scanParamArg(argc, argv, "eVLAN_MemberViolation", sizeof(param.eVLAN_MemberViolation), &param.eVLAN_MemberViolation);
	cnt += scanParamArg(argc, argv, "eAdmitMode", sizeof(param.eAdmitMode), &param.eAdmitMode);
	cnt += scanParamArg(argc, argv, "bTVM", sizeof(param.bTVM), &param.bTVM);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_PORT_CFG_SET, &param);
}

int gsw_vlan_port_member_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_portMemberAdd_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_portMemberAdd_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "bVLAN_TagEgress", sizeof(param.bVLAN_TagEgress), &param.bVLAN_TagEgress);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_PORT_MEMBER_ADD, &param);
}

int gsw_vlan_port_member_remove(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_portMemberRemove_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_portMemberRemove_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_PORT_MEMBER_REMOVE, &param);
}

int gsw_vlan_member_init(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_memberInit_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_memberInit_t));
	cnt += scanParamArg(argc, argv, "nPortMemberMap", sizeof(param.nPortMemberMap), &param.nPortMemberMap);
	cnt += scanParamArg(argc, argv, "nEgressTagMap", sizeof(param.nEgressTagMap), &param.nEgressTagMap);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_MEMBER_INIT, &param);
}

int gsw_vlan_reserved_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_reserved_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_reserved_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_RESERVED_ADD, &param);
}

int gsw_vlan_reserved_remove(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_reserved_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_reserved_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_VLAN_RESERVED_REMOVE, &param);
}

int gsw_wol_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_WoL_Cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_WoL_Cfg_t));

	if (cli_ioctl(fd, GSW_WOL_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanMAC_Arg(argc, argv, "nWolMAC", param.nWolMAC);
	cnt += scanMAC_Arg(argc, argv, "nWolPassword", param.nWolPassword);
	cnt += scanParamArg(argc, argv, "bWolPasswordEnable", sizeof(param.bWolPasswordEnable), &param.bWolPasswordEnable);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_WOL_CFG_SET, &param);
}

int gsw_wol_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_WoL_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_WoL_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_WOL_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bWakeOnLAN_Enable", sizeof(param.bWakeOnLAN_Enable), &param.bWakeOnLAN_Enable);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_WOL_PORT_CFG_SET, &param);
}

int gsw_irq_mask_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_irq_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_irq_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "eIrqSrc", sizeof(param.eIrqSrc), &param.eIrqSrc);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_IRQ_MASK_SET, &param);
}

int gsw_irq_status_clear(int argc, char *argv[], void *fd, int numPar)
{
	GSW_irq_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_irq_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "eIrqSrc", sizeof(param.eIrqSrc), &param.eIrqSrc);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_IRQ_STATUS_CLEAR, &param);
}

int gsw_pce_rule_delete(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_ruleDelete_t pce_rule;
	int cnt = 0;
	memset(&pce_rule, 0, sizeof(GSW_PCE_ruleDelete_t));

	cnt += scanParamArg(argc, argv, "pattern.nIndex", sizeof(pce_rule.nIndex), &pce_rule.nIndex);
	cnt += scanParamArg(argc, argv, "nRegion", sizeof(pce_rule.region), &pce_rule.region);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(pce_rule.logicalportid), &pce_rule.logicalportid);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(pce_rule.subifidgroup), &pce_rule.subifidgroup);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_PCE_RULE_DELETE, &pce_rule);
}

int gsw_reset(int argc, char *argv[], void *fd, int numPar)
{
	GSW_reset_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_reset_t));

	cnt += scanParamArg(argc, argv, "eReset", sizeof(param.eReset), &param.eReset);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_RESET, &param);
}

int gsw_8021x_eapol_rule_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_8021X_EAPOL_Rule_t param;
	memset(&param, 0, sizeof(GSW_8021X_EAPOL_Rule_t));

	if (cli_ioctl(fd, GSW_8021X_EAPOL_RULE_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("eForwardPort", param.eForwardPort, 0);
	printHex32Value("nForwardPortId", param.nForwardPortId, 0);
	return 0;
}

int gsw_8021x_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_8021X_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_8021X_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_8021X_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 32);
	printHex32Value("eState", param.eState, 0);
	return 0;
}

int gsw_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_cfg_t param;
	memset(&param, 0, sizeof(GSW_cfg_t));

	if (cli_ioctl(fd, GSW_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("eMAC_TableAgeTimer", param.eMAC_TableAgeTimer, 0);
	printf("\t%40s:\t%u\n", "nAgeTimer", param.nAgeTimer);
	printf("\t%40s:\t%d\n", "bVLAN_Aware", param.bVLAN_Aware);
	printHex32Value("nMaxPacketLen", param.nMaxPacketLen, 0);
	printf("\t%40s:\t%d\n", "bLearningLimitAction", param.bLearningLimitAction);
	printf("\t%40s:\t%d\n", "bPauseMAC_ModeSrc", param.bPauseMAC_ModeSrc);
	printf("\t%40s:\t", "nPauseMAC_Src");
	printMAC_Address(param.nPauseMAC_Src);
	printf("\n");
	return 0;
}

int gsw_cpu_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CPU_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_CPU_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CPU_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printf("\t%40s:\t%s\n", "bCPU_PortValid", (param.bCPU_PortValid > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bSpecialTagIngress", (param.bSpecialTagIngress > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bSpecialTagEgress", (param.bSpecialTagEgress > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bFcsCheck", (param.bFcsCheck > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bFcsGenerate", (param.bFcsGenerate > 0) ? "TRUE" : "FALSE");
	printHex32Value("bSpecialTagEthType", param.bSpecialTagEthType, 0);
	printf("\t%40s:\t%s\n", "bTsPtp", (param.bTsPtp > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bTsNonptp", (param.bTsNonptp > 0) ? "TRUE" : "FALSE");

	printf("\t%40s:\t%d\n", "eNoMPEParserCfg", param.eNoMPEParserCfg);
	printf("\t%40s:\t%d\n", "eMPE1ParserCfg", param.eMPE1ParserCfg);
	printf("\t%40s:\t%d\n", "eMPE2ParserCfg", param.eMPE2ParserCfg);
	printf("\t%40s:\t%d\n", "eMPE1MPE2ParserCfg", param.eMPE1MPE2ParserCfg);

	return 0;
}

int gsw_mac_table_entry_query(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_tableQuery_t sVar;
	int cnt = 0, i = 0;
	memset(&sVar, 0, sizeof(GSW_MAC_tableQuery_t));

	cnt += scanMAC_Arg(argc, argv, "nMAC", sVar.nMAC);
	cnt += scanParamArg(argc, argv, "nFId", sizeof(sVar.nFId), &sVar.nFId);
	cnt += scanParamArg(argc, argv, "nFilterFlag", sizeof(sVar.nFilterFlag), &sVar.nFilterFlag);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MAC_TABLE_ENTRY_QUERY, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printf("\t%40s:\t", "nMAC");
	printMAC_Address(sVar.nMAC);
	printf("\n");
	printf("\t\t\t\t\t    nFId:\t%d\n", sVar.nFId);
	printf("\t\t\t\t\tnFilterFlag:\t%d\n", sVar.nFilterFlag);
	printf("\t%40s:\t%s\n", "bFound", (sVar.bFound > 0) ? "1" : "0");
	printf("\t\t\t\t\t nPortId:\t0x%x\n", sVar.nPortId);
	printf("\t\t\t\t\tAgeTimer:\t%d\n", sVar.nAgeTimer);
	printf("\t\t\t\t\tnSubIfId:\t%d\n", sVar.nSubIfId);
	printf("\t%40s:\t%s\n", "bStaticEntry", (sVar.bStaticEntry > 0) ? "TRUE" : "FALSE");
	printf("\t\t\t\t bIgmpControlled:\t%d\n", sVar.bIgmpControlled);
	printf("\t\t\t\t\tnSVLAN_Id:\t%d\n", sVar.nSVLAN_Id);

	if (gswip_version == GSWIP_3_2 || gswip_version == GSWIP_3_1) {
		if (sVar.bStaticEntry) {
			for (i = 0; i < 8; i++) {
				printf("\t\t\t\t\t PortMap[ %d ]:\t0x%x\n", i, sVar.nPortMap[i]);
			}

			printf("\t\t\t\t\t Hit status:\t%d\n", sVar.hitstatus);
		} else { /*Dynamic Entry*/
			printf("\t%40s:\t%s\n", "bEntryChanged", (sVar.bEntryChanged > 0) ? "TRUE" : "FALSE");
		}

		if (gswip_version == GSWIP_3_2) {
			printf("\t%40s:\t", "nAssociatedMAC");
			printMAC_Address(sVar.nAssociatedMAC);
			printf("\n");
		}
	}

	return 0;
}

int gsw_mdio_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MDIO_cfg_t param;
	memset(&param, 0, sizeof(GSW_MDIO_cfg_t));

	if (cli_ioctl(fd, GSW_MDIO_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nMDIO_Speed", param.nMDIO_Speed, 0);
	printf("\t%40s:\t%s\n", "bMDIO_Enable", (param.bMDIO_Enable > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_mdio_data_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MDIO_data_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MDIO_data_t));

	cnt += scanParamArg(argc, argv, "nAddressDev", sizeof(param.nAddressDev), &param.nAddressDev);
	cnt += scanParamArg(argc, argv, "nAddressReg", sizeof(param.nAddressReg), &param.nAddressReg);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MDIO_DATA_READ, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nAddressDev", param.nAddressDev, 0);
	printHex32Value("nAddressReg", param.nAddressReg, 0);
	printHex32Value("nData", param.nData, 0);
	return 0;
}

int gsw_mmd_data_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MMD_data_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_MMD_data_t));

	cnt += scanParamArg(argc, argv, "nAddressDev", sizeof(param.nAddressDev), &param.nAddressDev);
	cnt += scanParamArg(argc, argv, "nAddressReg", sizeof(param.nAddressReg), &param.nAddressReg);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MMD_DATA_READ, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nAddressDev", param.nAddressDev, 0);
	printHex32Value("nAddressReg", param.nAddressReg, 0);
	printHex32Value("nData", param.nData, 0);
	return 0;
}

int gsw_monitor_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_monitorPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_monitorPortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (gswip_version == GSWIP_3_2) {
		cnt += scanParamArg(argc, argv, "bEnableMonitorQmap", 16, &param.bEnableMonitorQmap);
	}



	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MONITOR_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printf("\n\tnPortId %d", param.nPortId);
	printf("\t%40s:\t%s\n", "bMonitorPort", (param.bMonitorPort > 0) ? "TRUE" : "FALSE");
	printf("\n\tnSubIfId %d", param.nSubIfId);

	if (gswip_version == GSWIP_3_2) {
		printf("\n\tnbEnableIngressMirror %d", param.bEnableIngressMirror);
		printf("\n\tnbEnableEgressMirror %d", param.bEnableEgressMirror);
		printf("\n\tbEnableMonitorQmap %d", param.bEnableMonitorQmap);
		printf("\n\tnMonitorLpId %d", param.nMonitorLpId);
		printf("\n\tnMonitorQid %d", param.nMonitorQid);
		printf("\n\tnRedirectPortId %d", param.nRedirectPortId);
	}

	return 0;
}

int gsw_multicast_snoop_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastSnoopCfg_t param;
	memset(&param, 0, sizeof(GSW_multicastSnoopCfg_t));

	if (cli_ioctl(fd, GSW_MULTICAST_SNOOP_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("eIGMP_Mode", param.eIGMP_Mode, 0);
	printf("\t%40s:\t%s\n", "bIGMPv3", (param.bIGMPv3 > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bCrossVLAN", (param.bCrossVLAN > 0) ? "TRUE" : "FALSE");
	printHex32Value("eForwardPort", param.eForwardPort, 0);
	printHex32Value("nForwardPortId", param.nForwardPortId, 0);
	printHex32Value("nClassOfService", param.nClassOfService, 0);
	printHex32Value("nRobust", param.nRobust, 0);
	printHex32Value("nQueryInterval", param.nQueryInterval, 0);
	printHex32Value("eSuppressionAggregation", param.eSuppressionAggregation, 0);
	printf("\t%40s:\t%s\n", "bFastLeave", (param.bFastLeave > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bLearningRouter", (param.bLearningRouter > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bMulticastUnknownDrop", (param.bMulticastUnknownDrop > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bMulticastFIDmode", (param.bMulticastFIDmode > 0) ? "TRUE" : "FALSE");

	return 0;
}

int gsw_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "ePortType", sizeof(param.ePortType), &param.ePortType);


	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\t ePortType				= %u", param.ePortType);
	printf("\n\t nPortId				= %u", param.nPortId);
	printf("\n\t eEnable				= %u", param.eEnable);
	printf("\n\t bUnicastUnknownDrop	= %u", param.bUnicastUnknownDrop);
	printf("\n\t bMulticastUnknownDrop	= %u", param.bMulticastUnknownDrop);
	printf("\n\t bReservedPacketDrop	= %u", param.bReservedPacketDrop);
	printf("\n\t bBroadcastDrop			= %u", param.bBroadcastDrop);
	printf("\n\t bAging					= %u", param.bAging);
	printf("\n\t bLearning				= %u", param.bLearning);
	printf("\n\t bLearningMAC_PortLock	= %u", param.bLearningMAC_PortLock);
	printf("\n\t nLearningLimit			= %u", param.nLearningLimit);
	printf("\n\t bMAC_SpoofingDetection	= %u", param.bMAC_SpoofingDetection);
	printf("\n\t eFlowCtrl				= %u", param.eFlowCtrl);
	printf("\n\t ePortMonitor			= %u", param.ePortMonitor);
	printf("\n\t bIfCounters			= %u", param.bIfCounters);
	printf("\n\t nIfCountStartIdx		= %u", param.nIfCountStartIdx);
	printf("\n\t eIfRMONmode			= %u", param.eIfRMONmode);
	return 0;
}

int gsw_port_link_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portLinkCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portLinkCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_LINK_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printf("\t%40s:\t%s\n", "bDuplexForce", (param.bDuplexForce > 0) ? "TRUE" : "FALSE");
	printHex32Value("eDuplex", param.eDuplex, 0);
	printf("\t%40s:\t%s\n", "bSpeedForce", (param.bSpeedForce > 0) ? "TRUE" : "FALSE");
	printHex32Value("eSpeed", param.eSpeed, 0);
	printf("\t%40s:\t%s\n", "bLinkForce", (param.bLinkForce > 0) ? "TRUE" : "FALSE");
	printHex32Value("eLink", param.eLink, 0);
	printHex32Value("eMII_Mode", param.eMII_Mode, 0);
	printHex32Value("eMII_Type", param.eMII_Type, 0);
	printHex32Value("eClkMode", param.eClkMode, 0);
	printf("\t%40s:\t%s\n", "bLPI", (param.bLPI > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_port_phy_addr_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portPHY_Addr_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portPHY_Addr_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_PHY_ADDR_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("nAddressDev", param.nAddressDev, 0);
	return 0;
}

int gsw_port_phy_query(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portPHY_Query_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portPHY_Query_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_PHY_QUERY, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printf("\t%40s:\t%s\n", "bPHY_Present", (param.bPHY_Present > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_port_redirect_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portRedirectCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portRedirectCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_REDIRECT_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printf("\t%40s:\t%s\n", "bRedirectEgress", (param.bRedirectEgress > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bRedirectIngress", (param.bRedirectIngress > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_port_rgmii_clk_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_portRGMII_ClkCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_portRGMII_ClkCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PORT_RGMII_CLK_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("nDelayRx", param.nDelayRx, 0);
	printHex32Value("nDelayTx", param.nDelayTx, 0);
	return 0;
}

int gsw_qos_meter_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_meterCfg_t));
	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_METER_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printf("\t%40s:\t%s\n", "bEnable", (param.bEnable > 0) ? "TRUE" : "FALSE");
	printHex32Value("nMeterId", param.nMeterId, 0);
	printHex32Value("eMtrType", param.eMtrType, 0);
	printHex32Value("nCbs", param.nCbs, 0);
	printHex32Value("nEbs", param.nEbs, 0);
	printHex32Value("nRate", param.nRate, 0);
	printHex32Value("nPiRate", param.nPiRate, 0);
	//printHex32Value("cMeterName",  param.cMeterName, 0);
	printHex32Value("nColourBlindMode",  param.nColourBlindMode, 0);
	printHex32Value("bPktMode",  param.bPktMode, 0);
	printf("\t%40s:\t%s\n", "bLocalOverhd", (param.bLocalOverhd > 0) ? "TRUE" : "FALSE");
	printHex32Value("nLocaloverhd",  param.nLocaloverhd, 0);
	return 0;
}

int gsw_qos_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("eClassMode", param.eClassMode, 0);
	printHex32Value("nTrafficClass", param.nTrafficClass, 0);
	return 0;
}

int gsw_qos_port_remarking_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_portRemarkingCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_portRemarkingCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_PORT_REMARKING_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("eDSCP_IngressRemarkingEnable", param.eDSCP_IngressRemarkingEnable, 0);
	printf("\t%40s:\t%s\n", "bDSCP_EgressRemarkingEnable", (param.bDSCP_EgressRemarkingEnable > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bPCP_IngressRemarkingEnable", (param.bPCP_IngressRemarkingEnable > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bPCP_EgressRemarkingEnable", (param.bPCP_EgressRemarkingEnable > 0) ? "TRUE" : "FALSE");
//#ifdef SWAPI_ETC_CHIP
	printf("\t%40s:\t%s\n", "bSTAG_PCP_IngressRemarkingEnable", (param.bSTAG_PCP_IngressRemarkingEnable > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bSTAG_DEI_IngressRemarkingEnable", (param.bSTAG_DEI_IngressRemarkingEnable > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bSTAG_PCP_DEI_EgressRemarkingEnable", (param.bSTAG_PCP_DEI_EgressRemarkingEnable > 0) ? "TRUE" : "FALSE");
//#endif   /* SWAPI_ETC_CHIP */
	return 0;
}

int gsw_qos_scheduler_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_schedulerCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_schedulerCfg_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_SCHEDULER_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nQueueId", param.nQueueId, 0);
	printHex32Value("eType", param.eType, 0);
	printHex32Value("nWeight", param.nWeight, 0);
	return 0;
}

int gsw_qos_shaper_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ShaperCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_ShaperCfg_t));

	cnt += scanParamArg(argc, argv, "nRateShaperId", sizeof(param.nRateShaperId), &param.nRateShaperId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_SHAPER_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nRateShaperId", param.nRateShaperId, 0);
	printf("\t%40s:\t%s\n", "bEnable", (param.bEnable > 0) ? "TRUE" : "FALSE");
	printHex32Value("nCbs", param.nCbs, 0);
	printHex32Value("nRate", param.nRate, 0);
	return 0;
}

int gsw_qos_shaper_queue_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ShaperQueueGet_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_ShaperQueueGet_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_SHAPER_QUEUE_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nQueueId", param.nQueueId, 0);
	printf("\t%40s:\t%s\n", "bAssigned", (param.bAssigned > 0) ? "TRUE" : "FALSE");
	printHex32Value("nRateShaperId", param.nRateShaperId, 0);
	return 0;
}

int gsw_qos_storm_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_stormCfg_t param;
	memset(&param, 0, sizeof(GSW_QoS_stormCfg_t));

	if (cli_ioctl(fd, GSW_QOS_STORM_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nMeterId", param.nMeterId, 0);
	printf("\t%40s:\t%s\n", "bBroadcast", (param.bBroadcast > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bMulticast", (param.bMulticast > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bUnknownUnicast", (param.bUnknownUnicast > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_qos_wred_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_Cfg_t param;
	memset(&param, 0, sizeof(GSW_QoS_WRED_Cfg_t));

	if (cli_ioctl(fd, GSW_QOS_WRED_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("eProfile", param.eProfile, 0);
	printHex32Value("eMode", param.eMode, 0);
	printHex32Value("eThreshMode", param.eThreshMode, 0);
	printHex32Value("nRed_Min", param.nRed_Min, 0);
	printHex32Value("nRed_Max", param.nRed_Max, 0);
	printHex32Value("nYellow_Min", param.nYellow_Min, 0);
	printHex32Value("nYellow_Max", param.nYellow_Max, 0);
	printHex32Value("nGreen_Min", param.nGreen_Min, 0);
	printHex32Value("nGreen_Max", param.nGreen_Max, 0);
	return 0;
}

int gsw_qos_wred_queue_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_QueueCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_WRED_QueueCfg_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_WRED_QUEUE_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nQueueId", param.nQueueId, 0);
	printHex32Value("nRed_Min", param.nRed_Min, 0);
	printHex32Value("nRed_Max", param.nRed_Max, 0);
	printHex32Value("nYellow_Min", param.nYellow_Min, 0);
	printHex32Value("nYellow_Max", param.nYellow_Max, 0);
	printHex32Value("nGreen_Min", param.nGreen_Min, 0);
	printHex32Value("nGreen_Max", param.nGreen_Max, 0);
	return 0;
}


int gsw_debug_ctp_rmon_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Debug_RMON_Port_cnt_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_Debug_RMON_Port_cnt_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);

	if (cnt != numPar)
		return (-2);

	/*CTP port RX*/
	sVar.ePortType = 0;

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\n");
	printf("nRxGoodPkts                             : %u\n", sVar.nRxGoodPkts);
	printf("nRxUnicastPkts                          : %u\n", sVar.nRxUnicastPkts);
	printf("nRxBroadcastPkts                        : %u\n", sVar.nRxBroadcastPkts);
	printf("nRxMulticastPkts                        : %u\n", sVar.nRxMulticastPkts);
	printf("nRxFCSErrorPkts                         : %u\n", sVar.nRxFCSErrorPkts);
	printf("nRxUnderSizeGoodPkts                    : %u\n", sVar.nRxUnderSizeGoodPkts);
	printf("nRxOversizeGoodPkts                     : %u\n", sVar.nRxOversizeGoodPkts);
	printf("nRxUnderSizeErrorPkts                   : %u\n", sVar.nRxUnderSizeErrorPkts);
	printf("nRxOversizeErrorPkts                    : %u\n", sVar.nRxOversizeErrorPkts);
	printf("nRxFilteredPkts                         : %u\n", sVar.nRxFilteredPkts);
	printf("nRx64BytePkts                           : %u\n", sVar.nRx64BytePkts);
	printf("nRx127BytePkts                          : %u\n", sVar.nRx127BytePkts);
	printf("nRx255BytePkts                          : %u\n", sVar.nRx255BytePkts);
	printf("nRx511BytePkts                          : %u\n", sVar.nRx511BytePkts);
	printf("nRx1023BytePkts                         : %u\n", sVar.nRx1023BytePkts);
	printf("nRxMaxBytePkts                          : %u\n", sVar.nRxMaxBytePkts);
	printf("nRxDroppedPkts                          : %u\n", sVar.nRxDroppedPkts);
	printf("nRxExtendedVlanDiscardPkts              : %u\n", sVar.nRxExtendedVlanDiscardPkts);
	printf("nMtuExceedDiscardPkts                   : %u\n", sVar.nMtuExceedDiscardPkts);
	printf("nRxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytes, (unsigned long long)sVar.nRxGoodBytes);
	printf("nRxBadBytes                             : %llu (0x%llx)\n", (unsigned long long)sVar.nRxBadBytes, (unsigned long long)sVar.nRxBadBytes);

	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nRxUnicastPktsYellowRed                 : %u\n", sVar.nRxUnicastPktsYellowRed);
		printf("nRxBroadcastPktsYellowRed               : %u\n", sVar.nRxBroadcastPktsYellowRed);
		printf("nRxMulticastPktsYellowRed               : %u\n", sVar.nRxMulticastPktsYellowRed);
		printf("nRxGoodPktsYellowRed                    : %u\n", sVar.nRxGoodPktsYellowRed);
		printf("nRxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytesYellowRed, (unsigned long long)sVar.nRxGoodBytesYellowRed);
	}



	/*CTP port TX*/
	sVar.ePortType = 1;

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\n");
	printf("nTxGoodPkts                             : %u\n", sVar.nTxGoodPkts);
	printf("nTxUnicastPkts                          : %u\n", sVar.nTxUnicastPkts);
	printf("nTxBroadcastPkts                        : %u\n", sVar.nTxBroadcastPkts);
	printf("nTxMulticastPkts                        : %u\n", sVar.nTxMulticastPkts);
	printf("nTx64BytePkts                           : %u\n", sVar.nTx64BytePkts);
	printf("nTx127BytePkts                          : %u\n", sVar.nTx127BytePkts);
	printf("nTx255BytePkts                          : %u\n", sVar.nTx255BytePkts);
	printf("nTx511BytePkts                          : %u\n", sVar.nTx511BytePkts);
	printf("nTx1023BytePkts                         : %u\n", sVar.nTx1023BytePkts);
	printf("nTxMaxBytePkts                          : %u\n", sVar.nTxMaxBytePkts);
	printf("nTxDroppedPkts                          : %u\n", sVar.nTxDroppedPkts);
	printf("nTxOversizeGoodPkts                     : %u\n", sVar.nTxOversizeGoodPkts);
	printf("nTxUnderSizeGoodPkts                    : %u\n", sVar.nTxUnderSizeGoodPkts);
	printf("nTxAcmDroppedPkts                       : %u\n", sVar.nTxAcmDroppedPkts);
	printf("nTxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytes, (unsigned long long)sVar.nTxGoodBytes);

	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nTxUnicastPktsYellowRed                 : %u\n", sVar.nTxUnicastPktsYellowRed);
		printf("nTxBroadcastPktsYellowRed               : %u\n", sVar.nTxBroadcastPktsYellowRed);
		printf("nTxMulticastPktsYellowRed               : %u\n", sVar.nTxMulticastPktsYellowRed);
		printf("nTxGoodPktsYellowRed                    : %u\n", sVar.nTxGoodPktsYellowRed);
		printf("nTxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytesYellowRed, (unsigned long long)sVar.nTxGoodBytesYellowRed);
	}


	return 0;
}


int gsw_debug_bridge_rmon_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Debug_RMON_Port_cnt_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_Debug_RMON_Port_cnt_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);

	if (cnt != numPar)
		return (-2);

	/*bridge port RX*/
	sVar.ePortType = 2;

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\n");
	printf("nRxGoodPkts                             : %u\n", sVar.nRxGoodPkts);
	printf("nRxUnicastPkts                          : %u\n", sVar.nRxUnicastPkts);
	printf("nRxBroadcastPkts                        : %u\n", sVar.nRxBroadcastPkts);
	printf("nRxMulticastPkts                        : %u\n", sVar.nRxMulticastPkts);
	printf("nRxFCSErrorPkts                         : %u\n", sVar.nRxFCSErrorPkts);
	printf("nRxUnderSizeGoodPkts                    : %u\n", sVar.nRxUnderSizeGoodPkts);
	printf("nRxOversizeGoodPkts                     : %u\n", sVar.nRxOversizeGoodPkts);
	printf("nRxUnderSizeErrorPkts                   : %u\n", sVar.nRxUnderSizeErrorPkts);
	printf("nRxOversizeErrorPkts                    : %u\n", sVar.nRxOversizeErrorPkts);
	printf("nRxFilteredPkts                         : %u\n", sVar.nRxFilteredPkts);
	printf("nRx64BytePkts                           : %u\n", sVar.nRx64BytePkts);
	printf("nRx127BytePkts                          : %u\n", sVar.nRx127BytePkts);
	printf("nRx255BytePkts                          : %u\n", sVar.nRx255BytePkts);
	printf("nRx511BytePkts                          : %u\n", sVar.nRx511BytePkts);
	printf("nRx1023BytePkts                         : %u\n", sVar.nRx1023BytePkts);
	printf("nRxMaxBytePkts                          : %u\n", sVar.nRxMaxBytePkts);
	printf("nRxDroppedPkts                          : %u\n", sVar.nRxDroppedPkts);
	printf("nRxExtendedVlanDiscardPkts              : %u\n", sVar.nRxExtendedVlanDiscardPkts);
	printf("nMtuExceedDiscardPkts                   : %u\n", sVar.nMtuExceedDiscardPkts);
	printf("nRxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytes, (unsigned long long)sVar.nRxGoodBytes);
	printf("nRxBadBytes                             : %llu (0x%llx)\n", (unsigned long long)sVar.nRxBadBytes, (unsigned long long)sVar.nRxBadBytes);

	/*bridge port TX*/
	sVar.ePortType = 3;

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\n");
	printf("nTxGoodPkts                             : %u\n", sVar.nTxGoodPkts);
	printf("nTxUnicastPkts                          : %u\n", sVar.nTxUnicastPkts);
	printf("nTxBroadcastPkts                        : %u\n", sVar.nTxBroadcastPkts);
	printf("nTxMulticastPkts                        : %u\n", sVar.nTxMulticastPkts);
	printf("nTx64BytePkts                           : %u\n", sVar.nTx64BytePkts);
	printf("nTx127BytePkts                          : %u\n", sVar.nTx127BytePkts);
	printf("nTx255BytePkts                          : %u\n", sVar.nTx255BytePkts);
	printf("nTx511BytePkts                          : %u\n", sVar.nTx511BytePkts);
	printf("nTx1023BytePkts                         : %u\n", sVar.nTx1023BytePkts);
	printf("nTxMaxBytePkts                          : %u\n", sVar.nTxMaxBytePkts);
	printf("nTxDroppedPkts                          : %u\n", sVar.nTxDroppedPkts);
	printf("nTxOversizeGoodPkts                     : %u\n", sVar.nTxOversizeGoodPkts);
	printf("nTxUnderSizeGoodPkts                    : %u\n", sVar.nTxUnderSizeGoodPkts);
	printf("nTxAcmDroppedPkts                       : %u\n", sVar.nTxAcmDroppedPkts);
	printf("nTxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytes, (unsigned long long)sVar.nTxGoodBytes);

	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nTxUnicastPktsYellowRed                 : %u\n", sVar.nTxUnicastPktsYellowRed);
		printf("nTxBroadcastPktsYellowRed               : %u\n", sVar.nTxBroadcastPktsYellowRed);
		printf("nTxMulticastPktsYellowRed               : %u\n", sVar.nTxMulticastPktsYellowRed);
		printf("nTxGoodPktsYellowRed                    : %u\n", sVar.nTxGoodPktsYellowRed);
		printf("nTxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytesYellowRed, (unsigned long long)sVar.nTxGoodBytesYellowRed);
	}


	return 0;
}


int gsw_debug_ctpbypass_rmon_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Debug_RMON_Port_cnt_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_Debug_RMON_Port_cnt_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);

	if (cnt != numPar)
		return (-2);

	/*CTP bypass TX*/
	sVar.ePortType = 4;

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("\n\n");
	printf("nTxGoodPkts                             : %u\n", sVar.nTxGoodPkts);
	printf("nTxUnicastPkts                          : %u\n", sVar.nTxUnicastPkts);
	printf("nTxBroadcastPkts                        : %u\n", sVar.nTxBroadcastPkts);
	printf("nTxMulticastPkts                        : %u\n", sVar.nTxMulticastPkts);
	printf("nTx64BytePkts                           : %u\n", sVar.nTx64BytePkts);
	printf("nTx127BytePkts                          : %u\n", sVar.nTx127BytePkts);
	printf("nTx255BytePkts                          : %u\n", sVar.nTx255BytePkts);
	printf("nTx511BytePkts                          : %u\n", sVar.nTx511BytePkts);
	printf("nTx1023BytePkts                         : %u\n", sVar.nTx1023BytePkts);
	printf("nTxMaxBytePkts                          : %u\n", sVar.nTxMaxBytePkts);
	printf("nTxDroppedPkts                          : %u\n", sVar.nTxDroppedPkts);
	printf("nTxOversizeGoodPkts                     : %u\n", sVar.nTxOversizeGoodPkts);
	printf("nTxUnderSizeGoodPkts                    : %u\n", sVar.nTxUnderSizeGoodPkts);
	printf("nTxAcmDroppedPkts                       : %u\n", sVar.nTxAcmDroppedPkts);
	printf("nTxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytes, (unsigned long long)sVar.nTxGoodBytes);


	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nTxUnicastPktsYellowRed                 : %u\n", sVar.nTxUnicastPktsYellowRed);
		printf("nTxBroadcastPktsYellowRed               : %u\n", sVar.nTxBroadcastPktsYellowRed);
		printf("nTxMulticastPktsYellowRed               : %u\n", sVar.nTxMulticastPktsYellowRed);
		printf("nTxGoodPktsYellowRed                    : %u\n", sVar.nTxGoodPktsYellowRed);
		printf("nTxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytesYellowRed, (unsigned long long)sVar.nTxGoodBytesYellowRed);
	}


	return 0;
}

int gsw_debug_rmon_port_get_all(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Debug_RMON_Port_cnt_t sVar_rx[MAX_NUM_OF_DISPLAY_PORTS];
	GSW_Debug_RMON_Port_cnt_t sVar_tx[MAX_NUM_OF_DISPLAY_PORTS];
	u32 i = 0, j = 0, cnt = 0, start = 0, end = 0, max_read = 0;
	int maxctp, maxbridge;
	GSW_RMON_portType_t eCounerType;
	GSW_cap_t param;

	memset(sVar_rx, 0, sizeof(GSW_Debug_RMON_Port_cnt_t)*MAX_NUM_OF_DISPLAY_PORTS);
	memset(sVar_tx, 0, sizeof(GSW_Debug_RMON_Port_cnt_t)*MAX_NUM_OF_DISPLAY_PORTS);

	cnt += scanParamArg(argc, argv, "ePortType", sizeof(eCounerType), &eCounerType);
	cnt += scanParamArg(argc, argv, "Start", sizeof(start), &start);
	cnt += scanParamArg(argc, argv, "End", sizeof(end), &end);

	if (cnt != numPar)
		return (-2);

	memset(&param, 0, sizeof(GSW_cap_t));
	param.nCapType = GSW_CAP_TYPE_CTP;

	if (cli_ioctl(fd, GSW_CAP_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	maxctp = param.nCap;
	memset(&param, 0, sizeof(GSW_cap_t));
	param.nCapType = GSW_CAP_TYPE_BRIDGE_PORT;

	if (cli_ioctl(fd, GSW_CAP_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	maxbridge = param.nCap;

	if (eCounerType == 0 || eCounerType == 1) {
		if (start >= maxctp)
			start = maxctp - 1;

		if (end < start)
			end = start;
		else if (end >= maxctp)
			end = maxctp - 1;

	}

	if (eCounerType == 2 || eCounerType == 3) {
		if (start >= maxbridge)
			start = maxbridge - 1;

		if (end < start)
			end = start;
		else if (end >= maxbridge)
			end = maxbridge - 1;
	}

	max_read = end - start;

	if (max_read >= MAX_NUM_OF_DISPLAY_PORTS) {
		printf("\n\tWARNING: Display only 12 ports, please check start and end\n");
		return -1;
	}

	for (j = 0; j < 2; j++) {
		if (eCounerType == GSW_RMON_CTP_PORT_RX && j == 1) {
			eCounerType = GSW_RMON_CTP_PORT_TX;
		} else if (eCounerType == GSW_RMON_BRIDGE_PORT_RX && j == 1) {
			eCounerType = GSW_RMON_BRIDGE_PORT_TX;
		}

		switch (eCounerType) {
		case 0:
		case 1:
			printf("Reading CTP Port %s Counters\n", j ? "Tx" : "Rx");
			break;

		case 2:
		case 3:
			printf("Reading BRIDGE Port %s Counters\n", j ? "Tx" : "Rx");
			break;

		case 4:
			printf("Reading Bypass-PCE Port Tx Counters\n");
			break;

		default:
			break;
		}

		if (j == 0) {	// Getting RX port RMON
			for (i = 0; i <= max_read; i++) {
				sVar_rx[i].nPortId = start + i;
				sVar_rx[i].ePortType = eCounerType;

				if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar_rx[i]) != 0) {
					printf("ioctl returned with ERROR!\n");
					return (-1);
				}
			}
		} else {	// Getting TX port RMON
			for (i = 0; i <= max_read; i++) {
				sVar_tx[i].nPortId = start + i;
				sVar_tx[i].ePortType = eCounerType;

				if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar_tx[i]) != 0) {
					printf("ioctl returned with ERROR!\n");
					return (-1);
				}
			}
		}
	}

	printf("\n\n");

	printf("Port                                   : ");

	for (i = start; i <= end; i++)
		printf("%11u", i);

	printf("\n");

	if (sVar_rx[0].ePortType != 4) {
		printf("\n");

		printf("nRxGoodPkts                            : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxGoodPkts);

		printf("\n");

		printf("nRxUnicastPkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxUnicastPkts);

		printf("\n");

		printf("nRxBroadcastPkts                       : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxBroadcastPkts);

		printf("\n");

		printf("nRxMulticastPkts                       : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxMulticastPkts);

		printf("\n");

		printf("nRxFCSErrorPkts                        : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxFCSErrorPkts);

		printf("\n");

		printf("nRxUnderSizeGoodPkts                   : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxUnderSizeGoodPkts);

		printf("\n");

		printf("nRxOversizeGoodPkts                    : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxOversizeGoodPkts);

		printf("\n");

		printf("nRxUnderSizeErrorPkts                  : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxUnderSizeErrorPkts);

		printf("\n");

		printf("nRxOversizeErrorPkts                   : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxOversizeErrorPkts);

		printf("\n");

		printf("nRxFilteredPkts                        : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxFilteredPkts);

		printf("\n");

		printf("nRx64BytePkts                          : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRx64BytePkts);

		printf("\n");

		printf("nRx127BytePkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRx127BytePkts);

		printf("\n");

		printf("nRx255BytePkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRx255BytePkts);

		printf("\n");


		printf("nRx511BytePkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRx511BytePkts);

		printf("\n");


		printf("nRx1023BytePkts                        : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRx1023BytePkts);

		printf("\n");

		printf("nRxMaxBytePkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxMaxBytePkts);

		printf("\n");


		printf("nRxDroppedPkts                         : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxDroppedPkts);

		printf("\n");

		printf("nRxExtendedVlanDiscardPkts             : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nRxExtendedVlanDiscardPkts);

		printf("\n");

		printf("nMtuExceedDiscardPkts                  : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", sVar_rx[i].nMtuExceedDiscardPkts);

		printf("\n");

		printf("nRxGoodBytes                           : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", (u32)sVar_rx[i].nRxGoodBytes);

		printf("\n");

		printf("nRxBadBytes                            : ");

		for (i = 0; i <= max_read; i++)
			printf("%11u", (u32)sVar_rx[i].nRxBadBytes);

		printf("\n");
	}

	printf("\n");

	printf("nTxGoodPkts                            : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxGoodPkts);

	printf("\n");

	printf("nTxUnicastPkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxUnicastPkts);

	printf("\n");

	printf("nTxBroadcastPkts                       : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxBroadcastPkts);

	printf("\n");

	printf("nTxMulticastPkts                       : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxMulticastPkts);

	printf("\n");

	printf("nTx64BytePkts                          : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTx64BytePkts);

	printf("\n");

	printf("nTx127BytePkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTx127BytePkts);

	printf("\n");

	printf("nTx255BytePkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTx255BytePkts);

	printf("\n");

	printf("nTx511BytePkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTx511BytePkts);

	printf("\n");

	printf("nTx1023BytePkts                        : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTx1023BytePkts);

	printf("\n");

	printf("nTxMaxBytePkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxMaxBytePkts);

	printf("\n");

	printf("nTxDroppedPkts                         : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxDroppedPkts);

	printf("\n");

	printf("nTxOversizeGoodPkts                    : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxOversizeGoodPkts);

	printf("\n");

	printf("nTxUnderSizeGoodPkts                   : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxUnderSizeGoodPkts);

	printf("\n");

	printf("nTxAcmDroppedPkts                      : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", sVar_tx[i].nTxAcmDroppedPkts);

	printf("\n");

	printf("nTxGoodBytes                           : ");

	for (i = 0; i <= max_read; i++)
		printf("%11u", (u32)sVar_tx[i].nTxGoodBytes);

	printf("\n");

	return 0;
}


int gsw_debug_rmon_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Debug_RMON_Port_cnt_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_Debug_RMON_Port_cnt_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);
	cnt += scanParamArg(argc, argv, "ePortType", sizeof(sVar.ePortType), &sVar.ePortType);
	cnt += scanParamArg(argc, argv, "b64BitMode", sizeof(sVar.b64BitMode), &sVar.b64BitMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("nPortId                                 : %d\n", sVar.nPortId);
	printf("ePortType                               : %d\n", sVar.ePortType);
	printf("RMON Counter BitMode                    : 64 \n");
	printf("\n\n");
	printf("nRxGoodPkts                             : %u\n", sVar.nRxGoodPkts);
	printf("nRxUnicastPkts                          : %u\n", sVar.nRxUnicastPkts);
	printf("nRxBroadcastPkts                        : %u\n", sVar.nRxBroadcastPkts);
	printf("nRxMulticastPkts                        : %u\n", sVar.nRxMulticastPkts);
	printf("nRxFCSErrorPkts                         : %u\n", sVar.nRxFCSErrorPkts);
	printf("nRxUnderSizeGoodPkts                    : %u\n", sVar.nRxUnderSizeGoodPkts);
	printf("nRxOversizeGoodPkts                     : %u\n", sVar.nRxOversizeGoodPkts);
	printf("nRxUnderSizeErrorPkts                   : %u\n", sVar.nRxUnderSizeErrorPkts);
	printf("nRxOversizeErrorPkts                    : %u\n", sVar.nRxOversizeErrorPkts);
	printf("nRxFilteredPkts                         : %u\n", sVar.nRxFilteredPkts);
	printf("nRx64BytePkts                           : %u\n", sVar.nRx64BytePkts);
	printf("nRx127BytePkts                          : %u\n", sVar.nRx127BytePkts);
	printf("nRx255BytePkts                          : %u\n", sVar.nRx255BytePkts);
	printf("nRx511BytePkts                          : %u\n", sVar.nRx511BytePkts);
	printf("nRx1023BytePkts                         : %u\n", sVar.nRx1023BytePkts);
	printf("nRxMaxBytePkts                          : %u\n", sVar.nRxMaxBytePkts);
	printf("nRxDroppedPkts                          : %u\n", sVar.nRxDroppedPkts);
	printf("nRxExtendedVlanDiscardPkts              : %u\n", sVar.nRxExtendedVlanDiscardPkts);
	printf("nMtuExceedDiscardPkts                   : %u\n", sVar.nMtuExceedDiscardPkts);
	printf("nRxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytes, (unsigned long long)sVar.nRxGoodBytes);
	printf("nRxBadBytes                             : %llu (0x%llx)\n", (unsigned long long)sVar.nRxBadBytes, (unsigned long long)sVar.nRxBadBytes);

	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nRxUnicastPktsYellowRed                 : %u\n", sVar.nRxUnicastPktsYellowRed);
		printf("nRxBroadcastPktsYellowRed               : %u\n", sVar.nRxBroadcastPktsYellowRed);
		printf("nRxMulticastPktsYellowRed               : %u\n", sVar.nRxMulticastPktsYellowRed);
		printf("nRxGoodPktsYellowRed                    : %u\n", sVar.nRxGoodPktsYellowRed);
		printf("nRxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytesYellowRed, (unsigned long long)sVar.nRxGoodBytesYellowRed);
	}


	printf("\n\n");
	printf("nTxGoodPkts                             : %u\n", sVar.nTxGoodPkts);
	printf("nTxUnicastPkts                          : %u\n", sVar.nTxUnicastPkts);
	printf("nTxBroadcastPkts                        : %u\n", sVar.nTxBroadcastPkts);
	printf("nTxMulticastPkts                        : %u\n", sVar.nTxMulticastPkts);
	printf("nTx64BytePkts                           : %u\n", sVar.nTx64BytePkts);
	printf("nTx127BytePkts                          : %u\n", sVar.nTx127BytePkts);
	printf("nTx255BytePkts                          : %u\n", sVar.nTx255BytePkts);
	printf("nTx511BytePkts                          : %u\n", sVar.nTx511BytePkts);
	printf("nTx1023BytePkts                         : %u\n", sVar.nTx1023BytePkts);
	printf("nTxMaxBytePkts                          : %u\n", sVar.nTxMaxBytePkts);
	printf("nTxDroppedPkts                          : %u\n", sVar.nTxDroppedPkts);
	printf("nTxOversizeGoodPkts                     : %u\n", sVar.nTxOversizeGoodPkts);
	printf("nTxUnderSizeGoodPkts                    : %u\n", sVar.nTxUnderSizeGoodPkts);
	printf("nTxAcmDroppedPkts                       : %u\n", sVar.nTxAcmDroppedPkts);
	printf("nTxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytes, (unsigned long long)sVar.nTxGoodBytes);


	/*Valid only for GSWIP3.2 */
	if (gswip_version == GSWIP_3_2) {
		printf("nTxUnicastPktsYellowRed                 : %u\n", sVar.nTxUnicastPktsYellowRed);
		printf("nTxBroadcastPktsYellowRed               : %u\n", sVar.nTxBroadcastPktsYellowRed);
		printf("nTxMulticastPktsYellowRed               : %u\n", sVar.nTxMulticastPktsYellowRed);
		printf("nTxGoodPktsYellowRed                    : %u\n", sVar.nTxGoodPktsYellowRed);
		printf("nTxGoodBytesYellowRed                   : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytesYellowRed, (unsigned long long)sVar.nTxGoodBytesYellowRed);
	}



	return 0;
}


int gsw_rmon_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_Port_cnt_t sVar;
	int cnt = 0;
	memset(&sVar, 0, sizeof(GSW_RMON_Port_cnt_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);
	cnt += scanParamArg(argc, argv, "ePortType", sizeof(sVar.ePortType), &sVar.ePortType);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(sVar.nSubIfIdGroup), &sVar.nSubIfIdGroup);
	cnt += scanParamArg(argc, argv, "bPceBypass", sizeof(sVar.bPceBypass), &sVar.bPceBypass);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_PORT_GET, &sVar) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("nPortId                                 : %d\n", sVar.nPortId);

	switch (sVar.ePortType) {
	case GSW_LOGICAL_PORT:
		printf("ePortType                              : GSW_LOGICAL_PORT\n");
		break;

	case GSW_PHYSICAL_PORT:
		printf("ePortType							   : GSW_PHYSICAL_PORT\n");
		break;

	case GSW_CTP_PORT:
		printf("ePortType							   : GSW_CTP_PORT\n");
		break;

	case GSW_BRIDGE_PORT:
		printf("ePortType							   : GSW_BRIDGE_PORT\n");
		break;

	default:
		printf("ePortType							   : UNKNOWN\n");
	}

	printf("nSubIfIdGroup                           : %d\n", sVar.nSubIfIdGroup);
	printf("bPceBypass						       : %d\n", sVar.bPceBypass);

	if (!sVar.bPceBypass) {
		printf("\n\n");
		printf("nRxGoodPkts 							: %u\n", sVar.nRxGoodPkts);
		printf("nRxUnicastPkts							: %u\n", sVar.nRxUnicastPkts);
		printf("nRxBroadcastPkts						: %u\n", sVar.nRxBroadcastPkts);
		printf("nRxMulticastPkts						: %u\n", sVar.nRxMulticastPkts);
		printf("nRxFCSErrorPkts 						: %u\n", sVar.nRxFCSErrorPkts);
		printf("nRxUnderSizeGoodPkts					: %u\n", sVar.nRxUnderSizeGoodPkts);
		printf("nRxOversizeGoodPkts 					: %u\n", sVar.nRxOversizeGoodPkts);
		printf("nRxUnderSizeErrorPkts					: %u\n", sVar.nRxUnderSizeErrorPkts);
		printf("nRxOversizeErrorPkts					: %u\n", sVar.nRxOversizeErrorPkts);
		printf("nRxFilteredPkts 						: %u\n", sVar.nRxFilteredPkts);
		printf("nRx64BytePkts							: %u\n", sVar.nRx64BytePkts);
		printf("nRx127BytePkts							: %u\n", sVar.nRx127BytePkts);
		printf("nRx255BytePkts							: %u\n", sVar.nRx255BytePkts);
		printf("nRx511BytePkts							: %u\n", sVar.nRx511BytePkts);
		printf("nRx1023BytePkts 						: %u\n", sVar.nRx1023BytePkts);
		printf("nRxMaxBytePkts							: %u\n", sVar.nRxMaxBytePkts);
		printf("nRxDroppedPkts							: %u\n", sVar.nRxDroppedPkts);
		printf("nRxExtendedVlanDiscardPkts				: %u\n", sVar.nRxExtendedVlanDiscardPkts);
		printf("nMtuExceedDiscardPkts					: %u\n", sVar.nMtuExceedDiscardPkts);
		printf("nRxGoodBytes							: %llu (0x%llx)\n", (unsigned long long)sVar.nRxGoodBytes, (unsigned long long)sVar.nRxGoodBytes);
		printf("nRxBadBytes 							: %llu (0x%llx)\n", (unsigned long long)sVar.nRxBadBytes, (unsigned long long)sVar.nRxBadBytes);
	}

	printf("\n\n");
	printf("nTxGoodPkts                             : %u\n", sVar.nTxGoodPkts);
	printf("nTxUnicastPkts                          : %u\n", sVar.nTxUnicastPkts);
	printf("nTxBroadcastPkts                        : %u\n", sVar.nTxBroadcastPkts);
	printf("nTxMulticastPkts                        : %u\n", sVar.nTxMulticastPkts);
	printf("nTx64BytePkts                           : %u\n", sVar.nTx64BytePkts);
	printf("nTx127BytePkts                          : %u\n", sVar.nTx127BytePkts);
	printf("nTx255BytePkts                          : %u\n", sVar.nTx255BytePkts);
	printf("nTx511BytePkts                          : %u\n", sVar.nTx511BytePkts);
	printf("nTx1023BytePkts                         : %u\n", sVar.nTx1023BytePkts);
	printf("nTxMaxBytePkts                          : %u\n", sVar.nTxMaxBytePkts);
	printf("nTxDroppedPkts                          : %u\n", sVar.nTxDroppedPkts);
	printf("nTxOversizeGoodPkts                     : %u\n", sVar.nTxOversizeGoodPkts);
	printf("nTxUnderSizeGoodPkts                    : %u\n", sVar.nTxUnderSizeGoodPkts);
	printf("nTxAcmDroppedPkts                       : %u\n", sVar.nTxAcmDroppedPkts);
	printf("nTxGoodBytes                            : %llu (0x%llx)\n", (unsigned long long)sVar.nTxGoodBytes, (unsigned long long)sVar.nTxGoodBytes);

	return 0;
}

int gsw_stp_bpdu_rule_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_STP_BPDU_Rule_t param;
	memset(&param, 0, sizeof(GSW_STP_BPDU_Rule_t));

	if (cli_ioctl(fd, GSW_STP_BPDU_RULE_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("eForwardPort", param.eForwardPort, 0);
	printHex32Value("nForwardPortId", param.nForwardPortId, 0);
	return 0;
}

int gsw_stp_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_STP_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_STP_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_STP_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("ePortState", param.ePortState, 0);
	return 0;
}

int gsw_vlan_id_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_IdGet_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_IdGet_t));

	cnt += scanParamArg(argc, argv, "nVId", sizeof(param.nVId), &param.nVId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLAN_ID_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nVId", param.nVId, 0);
	printHex32Value("nFId", param.nFId, 0);
	return 0;
}

int gsw_vlan_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_VLAN_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLAN_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printHex32Value("nPortVId", param.nPortVId, 0);
	printf("\t%40s:\t%s\n", "bVLAN_UnknownDrop", (param.bVLAN_UnknownDrop > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bVLAN_ReAssign", (param.bVLAN_ReAssign > 0) ? "TRUE" : "FALSE");
	printHex32Value("eVLAN_MemberViolation", param.eVLAN_MemberViolation, 0);
	printHex32Value("eAdmitMode", param.eAdmitMode, 0);
	printf("\t%40s:\t%s\n", "bTVM", (param.bTVM > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_wol_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_WoL_Cfg_t param;
	memset(&param, 0, sizeof(GSW_WoL_Cfg_t));

	if (cli_ioctl(fd, GSW_WOL_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printf("\t%40s:\t", "nWolMAC");
	printMAC_Address(param.nWolMAC);
	printf("\n");
	printf("\t%40s:\t", "nWolPassword");
	printMAC_Address(param.nWolPassword);
	printf("\n");
	printf("\t%40s:\t%s\n", "bWolPasswordEnable", (param.bWolPasswordEnable > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_wol_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_WoL_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_WoL_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_WOL_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 8);
	printf("\t%40s:\t%s\n", "bWakeOnLAN_Enable", (param.bWakeOnLAN_Enable > 0) ? "TRUE" : "FALSE");
	return 0;
}

int gsw_irq_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_irq_t param;
	memset(&param, 0, sizeof(GSW_irq_t));

	if (cli_ioctl(fd, GSW_IRQ_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 32);
	printHex32Value("eIrqSrc", param.eIrqSrc, 0);
	return 0;
}

int gsw_irq_mask_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_irq_t param;
	memset(&param, 0, sizeof(GSW_irq_t));

	if (cli_ioctl(fd, GSW_IRQ_MASK_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 32);
	printHex32Value("eIrqSrc", param.eIrqSrc, 0);
	return 0;
}

int gsw_register_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_register_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_register_t));

	cnt += scanParamArg(argc, argv, "nRegAddr", sizeof(param.nRegAddr), &param.nRegAddr);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nRegAddr", param.nRegAddr, 0);
	printHex32Value("nData", param.nData, 0);
	return 0;
}
int gsw_trunking_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_trunkingCfg_t param;

	memset(&param, 0, sizeof(GSW_trunkingCfg_t));

	if (cli_ioctl(fd, GSW_TRUNKING_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printHex32Value("bIP_Src", param.bIP_Src, 0);
	printHex32Value("bIP_Dst", param.bIP_Dst, 0);
	printHex32Value("bMAC_Src", param.bMAC_Src, 0);
	printHex32Value("bMAC_Dst", param.bMAC_Dst, 0);
	return 0;
}
int gsw_trunking_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_trunkingCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_trunkingCfg_t));

	if (cli_ioctl(fd, GSW_TRUNKING_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "bIP_Src", sizeof(param.bIP_Src), &param.bIP_Src);
	cnt += scanParamArg(argc, argv, "bIP_Dst", sizeof(param.bIP_Dst), &param.bIP_Dst);
	cnt += scanParamArg(argc, argv, "bMAC_Src", sizeof(param.bMAC_Src), &param.bMAC_Src);
	cnt += scanParamArg(argc, argv, "bMAC_Dst", sizeof(param.bMAC_Dst), &param.bMAC_Dst);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_TRUNKING_CFG_SET, &param);
}
int gsw_trunking_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_trunkingPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_trunkingPortCfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_TRUNKING_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printHex32Value("nPortId", param.nPortId, 0);
	printf("\t%40s:\t%s\n", "bAggregateEnable", (param.bAggregateEnable > 0) ? "TRUE" : "FALSE");
	printHex32Value("nAggrPortId", param.nAggrPortId, 0);
	return 0;
}
int gsw_trunking_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_trunkingPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_trunkingPortCfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_TRUNKING_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	cnt += scanParamArg(argc, argv, "bAggregateEnable", sizeof(param.bAggregateEnable), &param.bAggregateEnable);
	cnt += scanParamArg(argc, argv, "nAggrPortId", sizeof(param.nAggrPortId), &param.nAggrPortId);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_TRUNKING_PORT_CFG_SET, &param);
}

int gsw_qos_wred_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_WRED_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_WRED_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printHex32Value("nPortId", param.nPortId, 0);
	printHex32Value("nRed_Min", param.nRed_Min, 0);
	printHex32Value("nRed_Max", param.nRed_Max, 0);
	printHex32Value("nYellow_Min", param.nYellow_Min, 0);
	printHex32Value("nYellow_Max", param.nYellow_Max, 0);
	printHex32Value("nGreen_Min", param.nGreen_Min, 0);
	printHex32Value("nGreen_Max", param.nGreen_Max, 0);
	return 0;
}

int gsw_qos_wred_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_WRED_PortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_WRED_PortCfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-3);

	if (cli_ioctl(fd, GSW_QOS_WRED_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nRed_Min", sizeof(param.nRed_Min), &param.nRed_Min);
	cnt += scanParamArg(argc, argv, "nRed_Max", sizeof(param.nRed_Max), &param.nRed_Max);
	cnt += scanParamArg(argc, argv, "nYellow_Min", sizeof(param.nYellow_Min), &param.nYellow_Min);
	cnt += scanParamArg(argc, argv, "nYellow_Max", sizeof(param.nYellow_Max), &param.nYellow_Max);
	cnt += scanParamArg(argc, argv, "nGreen_Min", sizeof(param.nGreen_Min), &param.nGreen_Min);
	cnt += scanParamArg(argc, argv, "nGreen_Max", sizeof(param.nGreen_Max), &param.nGreen_Max);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_WRED_PORT_CFG_SET, &param);
}

int gsw_qos_flowctrl_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_FlowCtrlCfg_t param;
	memset(&param, 0, sizeof(GSW_QoS_FlowCtrlCfg_t));

	if (cli_ioctl(fd, GSW_QOS_FLOWCTRL_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printHex32Value("nFlowCtrlNonConform_Min", param.nFlowCtrlNonConform_Min, 0);
	printHex32Value("nFlowCtrlNonConform_Max", param.nFlowCtrlNonConform_Max, 0);
	printHex32Value("nFlowCtrlConform_Min", param.nFlowCtrlConform_Min, 0);
	printHex32Value("nFlowCtrlConform_Max", param.nFlowCtrlConform_Max, 0);
	return 0;
}


int gsw_qos_flowctrl_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_FlowCtrlCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_FlowCtrlCfg_t));

	if (cli_ioctl(fd, GSW_QOS_FLOWCTRL_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nFlowCtrlNonConform_Min", sizeof(param.nFlowCtrlNonConform_Min), &param.nFlowCtrlNonConform_Min);
	cnt += scanParamArg(argc, argv, "nFlowCtrlNonConform_Max", sizeof(param.nFlowCtrlNonConform_Max), &param.nFlowCtrlNonConform_Max);
	cnt += scanParamArg(argc, argv, "nFlowCtrlConform_Min", sizeof(param.nFlowCtrlConform_Min), &param.nFlowCtrlConform_Min);
	cnt += scanParamArg(argc, argv, "nFlowCtrlConform_Max", sizeof(param.nFlowCtrlConform_Max), &param.nFlowCtrlConform_Max);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_FLOWCTRL_CFG_SET, &param);
}

int gsw_qos_flowctrl_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_FlowCtrlPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_FlowCtrlPortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_FLOWCTRL_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printHex32Value("nPortId", param.nPortId, 0);
	printHex32Value("nFlowCtrl_Min", param.nFlowCtrl_Min, 0);
	printHex32Value("nFlowCtrl_Max", param.nFlowCtrl_Max, 0);
	return 0;
}
int gsw_qos_flowctrl_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_FlowCtrlPortCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_FlowCtrlPortCfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", 32, &param.nPortId);

	if (cnt != 1)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_FLOWCTRL_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	cnt += scanParamArg(argc, argv, "nFlowCtrl_Min", sizeof(param.nFlowCtrl_Min), &param.nFlowCtrl_Min);
	cnt += scanParamArg(argc, argv, "nFlowCtrl_Max", sizeof(param.nFlowCtrl_Max), &param.nFlowCtrl_Max);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_FLOWCTRL_PORT_CFG_SET, &param);
}

int gsw_qos_queue_buffer_reserve_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_QueueBufferReserveCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_QueueBufferReserveCfg_t));
	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printHex32Value("nQueueId", param.nQueueId, 0);
	printHex32Value("nBufferReserved", param.nBufferReserved, 0);
	return 0;
}

int gsw_qos_queue_buffer_reserve_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_QueueBufferReserveCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_QueueBufferReserveCfg_t));

	cnt += scanParamArg(argc, argv, "nQueueId", sizeof(param.nQueueId), &param.nQueueId);

	if (cnt != 1)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	cnt += scanParamArg(argc, argv, "nBufferReserved", sizeof(param.nBufferReserved), &param.nBufferReserved);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET, &param);
}

//#ifdef SWAPI_ETC_CHIP
int gsw_svlan_cfg_get(int argc, char *argv[], void *fd, int numPar)
{

	GSW_SVLAN_cfg_t param;
	memset(&param, 0, sizeof(GSW_SVLAN_cfg_t));

	if (cli_ioctl(fd, GSW_SVLAN_CFG_GET, &param) != 0) {
		return (-1);
	}

	printf("\tnEthertype :0x%04x\n", param.nEthertype);
	return 0;
}

int gsw_svlan_cfg_set(int argc, char *argv[], void *fd, int numPar)
{

	GSW_SVLAN_cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_SVLAN_cfg_t));

	cnt += scanParamArg(argc, argv, "nEthertype", sizeof(param.nEthertype), &param.nEthertype);

	if (cnt != 1)
		return (-2);

	return cli_ioctl(fd, GSW_SVLAN_CFG_SET, &param);
}

int gsw_svlan_port_cfg_get(int argc, char *argv[], void *fd, int numPar)
{

	GSW_SVLAN_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_SVLAN_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_SVLAN_PORT_CFG_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	printf("Returned values:\n----------------\n");
	printf("\t%40s:\t0x%04x\n", "nPortId", param.nPortId);
	printf("\t%40s:\t0x%04x\n", "nPortVId", param.nPortVId);
	printf("\t%40s:\t%s\n", "bSVLAN_TagSupport", (param.bSVLAN_TagSupport > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bSVLAN_MACbasedTag", (param.bSVLAN_MACbasedTag > 0) ? "TRUE" : "FALSE");
	printf("\t%40s:\t%s\n", "bVLAN_ReAssign", (param.bVLAN_ReAssign > 0) ? "TRUE" : "FALSE");
	printHex32Value("eVLAN_MemberViolation", param.eVLAN_MemberViolation, 0);
	printHex32Value("eAdmitMode", param.eAdmitMode, 0);
	return 0;
}

int gsw_svlan_port_cfg_set(int argc, char *argv[], void *fd, int numPar)
{

	GSW_SVLAN_portCfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_SVLAN_portCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-2);

	if (cli_ioctl(fd, GSW_SVLAN_PORT_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nPortVId", sizeof(param.nPortVId), &param.nPortVId);
	cnt += scanParamArg(argc, argv, "bSVLAN_TagSupport", sizeof(param.bSVLAN_TagSupport), &param.bSVLAN_TagSupport);
	cnt += scanParamArg(argc, argv, "bSVLAN_MACbasedTag", sizeof(param.bSVLAN_MACbasedTag), &param.bSVLAN_MACbasedTag);
	cnt += scanParamArg(argc, argv, "bVLAN_ReAssign", sizeof(param.bVLAN_ReAssign), &param.bVLAN_ReAssign);
	cnt += scanParamArg(argc, argv, "eVLAN_MemberViolation", sizeof(param.eVLAN_MemberViolation), &param.eVLAN_MemberViolation);
	cnt += scanParamArg(argc, argv, "eAdmitMode", sizeof(param.eAdmitMode), &param.eAdmitMode);

	if (cnt != numPar)
		return (-2);

	return cli_ioctl(fd, GSW_SVLAN_PORT_CFG_SET, &param);

}

int gsw_qos_svlan_class_pcp_port_set(int argc, char *argv[], void *fd, int numPar)
{

	GSW_QoS_SVLAN_ClassPCP_PortCfg_t param;
	unsigned char nTrafficClass = 0, nSPCP = 0, nCPCP = 0, nDSCP = 0;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_QoS_SVLAN_ClassPCP_PortCfg_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nCPCP", sizeof(nCPCP), &nCPCP);
	cnt += scanParamArg(argc, argv, "nSPCP", sizeof(nSPCP), &nSPCP);
	cnt += scanParamArg(argc, argv, "nDSCP", sizeof(nDSCP), &nDSCP);

	if (cnt != numPar)
		return (-2);

	if (nTrafficClass >= 16) {
		printf("ERROR: Given \"nTrafficClass\" is out of range (15)\n");
		return (-3);
	}

	if (cli_ioctl(fd, GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &param) != 0)
		return (-2);

	param.nCPCP[nTrafficClass] = nCPCP;
	param.nSPCP[nTrafficClass] = nSPCP;
	param.nDSCP[nTrafficClass] = nDSCP;

	if (cli_ioctl(fd, GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, &param) != 0)
		return (-5);

	return 0;

}

int gsw_qos_svlan_class_pcp_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_SVLAN_ClassPCP_PortCfg_t param;
	int cnt = 0, i;

	memset(&param, 0, sizeof(GSW_QoS_SVLAN_ClassPCP_PortCfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, &param) != 0)
		return (-2);

	printf("\n\t nPortId = %d\n", param.nPortId);

	for (i = 0; i < 16; i++) {
		printf("\t nCPCP[%2d] = %d", i, param.nCPCP[i]);
		printf("\t nSPCP[%2d] = %d", i, param.nSPCP[i]);
		printf("\t nDSCP[%2d] = %d\n", i, param.nDSCP[i]);
	}

	return 0;
}

int gsw_qos_svlan_pcp_class_set(int argc, char *argv[], void *fd, int numPar)
{

	GSW_QoS_SVLAN_PCP_ClassCfg_t param;
	unsigned char nTrafficClass = 0, nTrafficColor = 0, nPCP_Remark_Enable = 0, nDEI_Remark_Enable = 0;
	unsigned int nPCP = 0, cnt = 0;

	cnt += scanParamArg(argc, argv, "nPCP", sizeof(nPCP), &nPCP);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nTrafficColor", sizeof(nTrafficColor), &nTrafficColor);
	cnt += scanParamArg(argc, argv, "nPCP_Remark_Enable", sizeof(nPCP_Remark_Enable), &nPCP_Remark_Enable);
	cnt += scanParamArg(argc, argv, "nDEI_Remark_Enable", sizeof(nDEI_Remark_Enable), &nDEI_Remark_Enable);

	if (nPCP >= 16) {
		printf("ERROR: Given \"nPCP\" is out of range (15)\n");
		return (-3);
	}

	memset(&param, 0x00, sizeof(param));

	if (cli_ioctl(fd, GSW_QOS_SVLAN_PCP_CLASS_GET, &param) != 0)
		return (-4);

	param.nTrafficClass[nPCP] = nTrafficClass;
	param.nTrafficColor[nPCP] = nTrafficColor;
	param.nPCP_Remark_Enable[nPCP] = nPCP_Remark_Enable;
	param.nDEI_Remark_Enable[nPCP] = nDEI_Remark_Enable;

	if (cli_ioctl(fd, GSW_QOS_SVLAN_PCP_CLASS_SET, &param) != 0)
		return (-5);

	return 0;
}

int gsw_qos_svlan_pcp_class_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_SVLAN_PCP_ClassCfg_t param;
	int i;
	memset(&param, 0x00, sizeof(param));

	if (cli_ioctl(fd, GSW_QOS_SVLAN_PCP_CLASS_GET, &param) != 0)
		return (-2);

	for (i = 0; i < 16; i++) {
		printf("\tnTrafficClass[%2d] = %d, TrafficColor[%2d] = %d  ", i, param.nTrafficClass[i], i, param.nTrafficColor[i]);
		printf("PCP_Remark_Enable[%2d] = %d, nDEI_Remark_Enable[%2d] = %d \n", i, param.nPCP_Remark_Enable[i], i, param.nDEI_Remark_Enable[i]);
	}

	return 0;
}

int gsw_pce_eg_vlan_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_EgVLAN_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PCE_EgVLAN_Cfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "eEgVLANmode", sizeof(param.eEgVLANmode), &param.eEgVLANmode);
	cnt += scanParamArg(argc, argv, "bEgVidEna", sizeof(param.bEgVidEna), &param.bEgVidEna);
	cnt += scanParamArg(argc, argv, "nEgStartVLANIdx", sizeof(param.nEgStartVLANIdx), &param.nEgStartVLANIdx);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_EG_VLAN_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pce_eg_vlan_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_EgVLAN_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PCE_EgVLAN_Cfg_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_EG_VLAN_CFG_GET, &param) != 0)
		return (-1);

	printf("\n\t nPortId       	: %d\n", param.nPortId);
	printf("\t nEgStartVLANIdx	: %d\n", param.nEgStartVLANIdx);
	printf("\t eEgVLANmode  		: %d\n", param.eEgVLANmode);
//	printf("\t bEgVidEna    : %d\n", param.bEgVidEna);

	return 0;
}

int gsw_pce_eg_vlan_entry_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_EgVLAN_Entry_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_PCE_EgVLAN_Entry_t));
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.nIndex), &param.nIndex);
	cnt += scanParamArg(argc, argv, "bEgVLAN_Action", sizeof(param.bEgVLAN_Action), &param.bEgVLAN_Action);
	cnt += scanParamArg(argc, argv, "bEgSVidRem_Action", sizeof(param.bEgSVidRem_Action), &param.bEgSVidRem_Action);
	cnt += scanParamArg(argc, argv, "bEgSVidIns_Action", sizeof(param.bEgSVidIns_Action), &param.bEgSVidIns_Action);
	cnt += scanParamArg(argc, argv, "nEgSVid", sizeof(param.nEgSVid), &param.nEgSVid);
	cnt += scanParamArg(argc, argv, "bEgCVidRem_Action", sizeof(param.bEgCVidRem_Action), &param.bEgCVidRem_Action);
	cnt += scanParamArg(argc, argv, "bEgCVidIns_Action", sizeof(param.bEgCVidIns_Action), &param.bEgCVidIns_Action);
	cnt += scanParamArg(argc, argv, "nEgCVid", sizeof(param.nEgCVid), &param.nEgCVid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_EG_VLAN_ENTRY_WRITE, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pce_eg_vlan_entry_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_EgVLAN_Entry_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_PCE_EgVLAN_Entry_t));
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.nIndex), &param.nIndex);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_EG_VLAN_ENTRY_READ, &param) != 0)
		return (-1);

	printf("\n\t nPortId					: %d\n", param.nPortId);
	printf("\t nIndex							: %d\n", param.nIndex);
	printf("\t nEgSVid						: %d\n", param.nEgSVid);
	printf("\t nEgCVid						: %d\n", param.nEgCVid);
	printf("\t bEgVLAN_Action   	: %d\n", param.bEgVLAN_Action);
	printf("\t bEgSVidRem_Action	: %d\n", param.bEgSVidRem_Action);
	printf("\t bEgSVidIns_Action 	: %d\n", param.bEgSVidIns_Action);
	printf("\t bEgCVidRem_Action 	: %d\n", param.bEgCVidRem_Action);
	printf("\t bEgCVidIns_Action 	: %d\n", param.bEgCVidIns_Action);
	return 0;
}

int gsw_pmac_bm_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_BM_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_BM_Cfg_t));
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(param.nTxDmaChanId), &param.nTxDmaChanId);
	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_BM_CFG_GET, &param) != 0)
		return (-1);

	printHex32Value("nTxDmaChanId", param.nTxDmaChanId, 0);
	printHex32Value("txQMask", param.txQMask, 0);
	printHex32Value("rxPortMask", param.rxPortMask, 0);
	return 0;
}

int gsw_pmac_bm_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_BM_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_BM_Cfg_t));
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(param.nTxDmaChanId), &param.nTxDmaChanId);
	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "txQMask", sizeof(param.txQMask), &param.txQMask);
	cnt += scanParamArg(argc, argv, "rxPortMask", sizeof(param.rxPortMask), &param.rxPortMask);

	if (cli_ioctl(fd, GSW_PMAC_BM_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pmac_eg_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Eg_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Eg_Cfg_t));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "nDestPortId", sizeof(param.nDestPortId), &param.nDestPortId);
	cnt += scanParamArg(argc, argv, "bProcFlagsSelect", sizeof(param.bProcFlagsSelect), &param.bProcFlagsSelect);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(param.nTrafficClass), &param.nTrafficClass);
	cnt += scanParamArg(argc, argv, "nFlowIDMsb", sizeof(param.nFlowIDMsb), &param.nFlowIDMsb);

	cnt += scanParamArg(argc, argv, "bMpe1Flag", sizeof(param.bMpe1Flag), &param.bMpe1Flag);
	cnt += scanParamArg(argc, argv, "bMpe2Flag", sizeof(param.bMpe2Flag), &param.bMpe2Flag);
	cnt += scanParamArg(argc, argv, "bEncFlag", sizeof(param.bEncFlag), &param.bEncFlag);
	cnt += scanParamArg(argc, argv, "bDecFlag", sizeof(param.bDecFlag), &param.bDecFlag);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_EG_CFG_GET, &param) != 0)
		return (-1);

	printf("nPmacId					= %u\n", param.nPmacId);
	printf("nDestPortId				= %u\n", param.nDestPortId);
	printf("nTrafficClass				= %u\n", param.nTrafficClass);
	printf("bMpe1Flag				= %u\n", param.bMpe1Flag);
	printf("bMpe2Flag				= %u\n", param.bMpe2Flag);
	printf("bDecFlag				= %u\n", param.bDecFlag);
	printf("bEncFlag				= %u\n", param.bEncFlag);
	printf("nFlowIDMsb				= %u\n", param.nFlowIDMsb);
	printf("bProcFlagsSelect			= %u\n", param.bProcFlagsSelect);
	printf("nRxDmaChanId				= %u\n", param.nRxDmaChanId);
	printf("bRemL2Hdr				= %u\n", param.bRemL2Hdr);
	printf("numBytesRem				= %u\n", param.numBytesRem);
	printf("bFcsEna					= %u\n", param.bFcsEna);
	printf("bPmacEna				= %u\n", param.bPmacEna);
	printf("bRedirEnable				= %u\n", param.bRedirEnable);
	printf("bBslSegmentDisable			= %u\n", param.bBslSegmentDisable);
	printf("nBslTrafficClass			= 0x%x\n", param.nBslTrafficClass);
	printf("bResDW1Enable				= %u\n", param.bResDW1Enable);
	printf("nResDW1					= 0x%x\n", param.nResDW1);
	printf("bRes1DW0Enable				= %u\n", param.bRes1DW0Enable);
	printf("nRes1DW0				= 0x%x\n", param.nRes1DW0);
	printf("bRes2DW0Enable				= %u\n", param.bRes2DW0Enable);
	printf("nRes2DW0				= 0x%x\n", param.nRes2DW0);
	printf("bTCEnable				= %u\n", param.bTCEnable);
	return 0;
}

int gsw_pmac_eg_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Eg_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Eg_Cfg_t));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "bRedirEnable", sizeof(param.bRedirEnable), &param.bRedirEnable);
	cnt += scanParamArg(argc, argv, "bBslSegmentDisable", sizeof(param.bBslSegmentDisable), &param.bBslSegmentDisable);
	cnt += scanParamArg(argc, argv, "nBslTrafficClass", sizeof(param.nBslTrafficClass), &param.nBslTrafficClass);
	cnt += scanParamArg(argc, argv, "bResDW1Enable", sizeof(param.bResDW1Enable), &param.bResDW1Enable);
	cnt += scanParamArg(argc, argv, "bRes2DW0Enable", sizeof(param.bRes2DW0Enable), &param.bRes2DW0Enable);
	cnt += scanParamArg(argc, argv, "bRes1DW0Enable", sizeof(param.bRes1DW0Enable), &param.bRes1DW0Enable);
	cnt += scanParamArg(argc, argv, "bTCEnable", sizeof(param.bTCEnable), &param.bTCEnable);
	cnt += scanParamArg(argc, argv, "nDestPortId", sizeof(param.nDestPortId), &param.nDestPortId);
	cnt += scanParamArg(argc, argv, "bProcFlagsSelect", sizeof(param.bProcFlagsSelect), &param.bProcFlagsSelect);
	cnt += scanParamArg(argc, argv, "nTrafficClass", sizeof(param.nTrafficClass), &param.nTrafficClass);
	cnt += scanParamArg(argc, argv, "nFlowIDMsb", sizeof(param.nFlowIDMsb), &param.nFlowIDMsb);
	cnt += scanParamArg(argc, argv, "bMpe1Flag", sizeof(param.bMpe1Flag), &param.bMpe1Flag);
	cnt += scanParamArg(argc, argv, "bMpe2Flag", sizeof(param.bMpe2Flag), &param.bMpe2Flag);
	cnt += scanParamArg(argc, argv, "bEncFlag", sizeof(param.bEncFlag), &param.bEncFlag);
	cnt += scanParamArg(argc, argv, "bDecFlag", sizeof(param.bDecFlag), &param.bDecFlag);
	cnt += scanParamArg(argc, argv, "nRxDmaChanId", sizeof(param.nRxDmaChanId), &param.nRxDmaChanId);
	cnt += scanParamArg(argc, argv, "bRemL2Hdr", sizeof(param.bRemL2Hdr), &param.bRemL2Hdr);
	cnt += scanParamArg(argc, argv, "numBytesRem", sizeof(param.numBytesRem), &param.numBytesRem);
	cnt += scanParamArg(argc, argv, "bFcsEna", sizeof(param.bFcsEna), &param.bFcsEna);
	cnt += scanParamArg(argc, argv, "bPmacEna", sizeof(param.bPmacEna), &param.bPmacEna);
	cnt += scanParamArg(argc, argv, "nResDW1", sizeof(param.nResDW1), &param.nResDW1);
	cnt += scanParamArg(argc, argv, "nRes1DW0", sizeof(param.nRes1DW0), &param.nRes1DW0);
	cnt += scanParamArg(argc, argv, "nRes2DW0", sizeof(param.nRes2DW0), &param.nRes2DW0);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_EG_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}
#if 0
int gsw_pmac_eg_count_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Eg_Cnt_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Eg_Cnt_t));
	cnt += scanParamArg(argc, argv, "nTxPortId", sizeof(param.nTxPortId), &param.nTxPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_EG_COUNT_GET, &param) != 0)
		return (-1);

	printHex32Value("nTxPortId", param.nTxPortId, 0);
	printHex32Value("nChkSumErrPktsCount", param.nChkSumErrPktsCount, 0);
	printHex32Value("nChkSumErrBytesCount", param.nChkSumErrBytesCount, 0);
	return 0;
}
#endif
int gsw_pmac_ig_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Ig_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Ig_Cfg_t));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(param.nTxDmaChanId), &param.nTxDmaChanId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_IG_CFG_GET, &param) != 0)
		return (-1);

	printf("nPmacId					= %u\n", param.nPmacId);
	printf("nTxDmaChanId				= %u\n", param.nTxDmaChanId);
	printf("bErrPktsDisc				= %u\n", param.bErrPktsDisc);
	printf("bPmapDefault				= %u\n", param.bPmapDefault);
	printf("bPmapEna				= %u\n", param.bPmapEna);
	printf("bClassDefault				= %u\n", param.bClassDefault);
	printf("bClassEna				= %u\n", param.bClassEna);
	printf("eSubId					= %u\n", param.eSubId);
	printf("bSpIdDefault				= %u\n", param.bSpIdDefault);
	printf("bPmacPresent				= %u\n", param.bPmacPresent);
	printf("defPmacHdr				= ");
	printf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
	       param.defPmacHdr[0],
	       param.defPmacHdr[1],
	       param.defPmacHdr[2],
	       param.defPmacHdr[3],
	       param.defPmacHdr[4],
	       param.defPmacHdr[5],
	       param.defPmacHdr[6],
	       param.defPmacHdr[7]);
	return 0;
}

int gsw_pmac_ig_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Ig_Cfg_t param;
	unsigned int cnt = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Ig_Cfg_t));
	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(param.nTxDmaChanId), &param.nTxDmaChanId);
	cnt += scanParamArg(argc, argv, "bErrPktsDisc", sizeof(param.bErrPktsDisc), &param.bErrPktsDisc);
	cnt += scanParamArg(argc, argv, "bPmapDefault", sizeof(param.bPmapDefault), &param.bPmapDefault);
	cnt += scanParamArg(argc, argv, "bPmapEna", sizeof(param.bPmapEna), &param.bPmapEna);
	cnt += scanParamArg(argc, argv, "bClassDefault", sizeof(param.bClassDefault), &param.bClassDefault);
	cnt += scanParamArg(argc, argv, "bClassEna", sizeof(param.bClassEna), &param.bClassEna);
	cnt += scanParamArg(argc, argv, "eSubId", sizeof(param.eSubId), &param.eSubId);
	cnt += scanParamArg(argc, argv, "bSpIdDefault", sizeof(param.bSpIdDefault), &param.bSpIdDefault);
	cnt += scanParamArg(argc, argv, "bPmacPresent", sizeof(param.bPmacPresent), &param.bPmacPresent);
	cnt += scanPMAC_Arg(argc, argv, "defPmacHdr", param.defPmacHdr);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_IG_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pmac_count_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Cnt_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_PMAC_Cnt_t));
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(param.nTxDmaChanId), &param.nTxDmaChanId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_COUNT_GET, &param) != 0)
		return (-1);

	printHex32Value("nTxDmaChanId", param.nTxDmaChanId, 0);
	printHex32Value("nDiscPktsCount", param.nDiscPktsCount, 0);
	printHex32Value("nDiscBytesCount", param.nDiscBytesCount, 0);
	printHex32Value("nChkSumErrPktsCount", param.nChkSumErrPktsCount, 0);
	printHex32Value("nChkSumErrBytesCount", param.nChkSumErrBytesCount, 0);
	return 0;
}

int gsw_rmon_mode_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_mode_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_RMON_mode_t));
	cnt += scanParamArg(argc, argv, "eRmonType", sizeof(param.eRmonType), &param.eRmonType);
	cnt += scanParamArg(argc, argv, "eCountMode", sizeof(param.eCountMode), &param.eCountMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_MODE_SET, &param) != 0)
		return (-1);

	return 0;
}

int gsw_rmon_if_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_If_cnt_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_RMON_If_cnt_t));
	cnt += scanParamArg(argc, argv, "nIfId", sizeof(param.nIfId), &param.nIfId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_IF_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nIfId", param.nIfId, 0);
	printHex32Value("countMode", param.countMode, 0);
	printHex32Value("nRxPktsCount", param.nRxPktsCount, 0);
	printHex32Value("nRxDiscPktsCount", param.nRxDiscPktsCount, 0);
	printHex32Value("nRxBytesCount", param.nRxBytesCount, 0);
	printHex32Value("nTxPktsCount", param.nTxPktsCount, 0);
	printHex32Value("nTxDiscPktsCount", param.nTxDiscPktsCount, 0);
	printHex32Value("nTxBytesCount", param.nTxBytesCount, 0);
	return 0;
}

int gsw_rmon_redirect_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_Redirect_cnt_t param;

	memset(&param, 0, sizeof(GSW_RMON_Redirect_cnt_t));

	if (cli_ioctl(fd, GSW_RMON_REDIRECT_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nRxPktsCount", param.nRxPktsCount, 0);
	printHex32Value("nRxDiscPktsCount", param.nRxDiscPktsCount, 0);
//	printHex32Value("nRxBytesCount", param.nRxBytesCount, 0);
	printf("\t%40s:\t%llu (0x%llx)\n", "nRxBytesCount", (unsigned long long)param.nRxBytesCount, (unsigned long long)param.nRxBytesCount);
	printHex32Value("nTxPktsCount", param.nTxPktsCount, 0);
	printHex32Value("nTxDiscPktsCount", param.nTxDiscPktsCount, 0);
//	printHex32Value("nTxBytesCount", param.nTxBytesCount, 0);
	printf("\t%40s:\t%llu (0x%llx)\n", "nTxBytesCount", (unsigned long long)param.nTxBytesCount, (unsigned long long)param.nTxBytesCount);
	return 0;
}

int gsw_rmon_route_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_Route_cnt_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_RMON_Route_cnt_t));
	cnt += scanParamArg(argc, argv, "nRoutedPortId", sizeof(param.nRoutedPortId), &param.nRoutedPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_ROUTE_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nRoutedPortId", param.nRoutedPortId, 0);
	printHex32Value("nRxUCv4UDPPktsCount", param.nRxUCv4UDPPktsCount, 0);
	printHex32Value("nRxUCv4TCPPktsCount", param.nRxUCv4TCPPktsCount, 0);
	printHex32Value("nRxUCv6UDPPktsCount", param.nRxUCv6UDPPktsCount, 0);
	printHex32Value("nRxUCv6TCPPktsCount", param.nRxUCv6TCPPktsCount, 0);
	printHex32Value("nRxMCv4PktsCount", param.nRxMCv4PktsCount, 0);
	printHex32Value("nRxMCv6PktsCount", param.nRxMCv6PktsCount, 0);
	printHex32Value("nRxCpuPktsCount", param.nRxCpuPktsCount, 0);
	printHex32Value("nRxPktsDropCount", param.nRxPktsDropCount, 0);
	printHex32Value("nRxIPv4BytesCount", param.nRxIPv4BytesCount, 0);
	printHex32Value("nRxIPv6BytesCount", param.nRxIPv6BytesCount, 0);
	printHex32Value("nRxCpuBytesCount", param.nRxCpuBytesCount, 0);
	printHex32Value("nRxBytesDropCount", param.nRxBytesDropCount, 0);
	printHex32Value("nTxPktsCount", param.nTxPktsCount, 0);
	printHex32Value("nTxBytesCount", param.nTxBytesCount, 0);
	return 0;
}

int gsw_rmon_meter_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_Meter_cnt_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_RMON_Meter_cnt_t));
	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_METER_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nMeterId", param.nMeterId, 0);
	printHex32Value("nGreenCount", param.nGreenCount, 0);
	printHex32Value("nYellowCount", param.nYellowCount, 0);
	printHex32Value("nRedCount", param.nRedCount, 0);
	printHex32Value("nResCount", param.nResCount, 0);

	return 0;
}

int gsw_rmon_flow_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_flowGet_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_RMON_flowGet_t));
	cnt += scanParamArg(argc, argv, "bIndex", sizeof(param.bIndex), &param.bIndex);
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.nIndex), &param.nIndex);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);
	cnt += scanParamArg(argc, argv, "nFlowId", sizeof(param.nFlowId), &param.nFlowId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_FLOW_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nIndex", param.nIndex, 0);
	printHex32Value("nRxPkts", param.nRxPkts, 0);
	printHex32Value("nTxPkts", param.nTxPkts, 0);
	printHex32Value("nTxPceBypassPkts", param.nTxPceBypassPkts, 0);

	return 0;
}

int gsw_rmon_tflow_clear(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_flowGet_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(GSW_RMON_flowGet_t));

	cnt += scanParamArg(argc, argv, "bIndex", sizeof(sVar.bIndex), &sVar.bIndex);
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(sVar.nIndex), &sVar.nIndex);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nPortId), &sVar.nPortId);
	cnt += scanParamArg(argc, argv, "nFlowId", sizeof(sVar.nFlowId), &sVar.nFlowId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_TFLOW_CLEAR, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_qos_meter_act(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_mtrAction_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_QoS_mtrAction_t));
	cnt += scanParamArg(argc, argv, "nCpuUserId", sizeof(param.nCpuUserId), &param.nCpuUserId);
	cnt += scanParamArg(argc, argv, "nMeterId", sizeof(param.nMeterId), &param.nMeterId);
	cnt += scanParamArg(argc, argv, "bMeterEna", sizeof(param.bMeterEna), &param.bMeterEna);
	cnt += scanParamArg(argc, argv, "nSecMeterId", sizeof(param.nSecMeterId), &param.nSecMeterId);
	cnt += scanParamArg(argc, argv, "bSecMeterEna", sizeof(param.bSecMeterEna), &param.bSecMeterEna);
	cnt += scanParamArg(argc, argv, "ePreColor", sizeof(param.ePreColor), &param.ePreColor);
	cnt += scanParamArg(argc, argv, "pktLen", sizeof(param.pktLen), &param.pktLen);
	cnt += scanParamArg(argc, argv, "eOutColor", sizeof(param.eOutColor), &param.eOutColor);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_METER_ACT, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pmac_glbl_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Glbl_Cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_PMAC_Glbl_Cfg_t));

	if (cli_ioctl(fd, GSW_PMAC_GLBL_CFG_GET, &param) != 0)
		return (-4);

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);
	cnt += scanParamArg(argc, argv, "bRxFCSDis", sizeof(param.bRxFCSDis), &param.bRxFCSDis);
	cnt += scanParamArg(argc, argv, "eProcFlagsEgCfg", sizeof(param.eProcFlagsEgCfg), &param.eProcFlagsEgCfg);
	cnt += scanParamArg(argc, argv, "nBslThreshold0", sizeof(param.nBslThreshold[0]), &param.nBslThreshold[0]);
	cnt += scanParamArg(argc, argv, "nBslThreshold1", sizeof(param.nBslThreshold[1]), &param.nBslThreshold[1]);
	cnt += scanParamArg(argc, argv, "nBslThreshold2", sizeof(param.nBslThreshold[2]), &param.nBslThreshold[2]);
	cnt += scanParamArg(argc, argv, "bAPadEna", sizeof(param.bAPadEna), &param.bAPadEna);
	cnt += scanParamArg(argc, argv, "bPadEna", sizeof(param.bPadEna), &param.bPadEna);
	cnt += scanParamArg(argc, argv, "bVPadEna", sizeof(param.bVPadEna), &param.bVPadEna);
	cnt += scanParamArg(argc, argv, "bSVPadEna", sizeof(param.bSVPadEna), &param.bSVPadEna);
	cnt += scanParamArg(argc, argv, "bTxFCSDis", sizeof(param.bTxFCSDis), &param.bTxFCSDis);
	cnt += scanParamArg(argc, argv, "bIPTransChkRegDis", sizeof(param.bIPTransChkRegDis), &param.bIPTransChkRegDis);
	cnt += scanParamArg(argc, argv, "bIPTransChkVerDis", sizeof(param.bIPTransChkVerDis), &param.bIPTransChkVerDis);
	cnt += scanParamArg(argc, argv, "bJumboEna", sizeof(param.bJumboEna), &param.bJumboEna);
	cnt += scanParamArg(argc, argv, "nMaxJumboLen", sizeof(param.nMaxJumboLen), &param.nMaxJumboLen);
	cnt += scanParamArg(argc, argv, "nJumboThreshLen", sizeof(param.nJumboThreshLen), &param.nJumboThreshLen);
	cnt += scanParamArg(argc, argv, "bLongFrmChkDis", sizeof(param.bLongFrmChkDis), &param.bLongFrmChkDis);
	cnt += scanParamArg(argc, argv, "eShortFrmChkType", sizeof(param.eShortFrmChkType), &param.eShortFrmChkType);
	cnt += scanParamArg(argc, argv, "bProcFlagsEgCfgEna", sizeof(param.bProcFlagsEgCfgEna), &param.bProcFlagsEgCfgEna);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_GLBL_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}

int gsw_pmac_glbl_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Glbl_Cfg_t param;
	unsigned int cnt = 0, i = 0;
	memset(&param, 0, sizeof(GSW_PMAC_Ig_Cfg_t));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(param.nPmacId), &param.nPmacId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_GLBL_CFG_GET, &param) != 0)
		return (-1);

	printf("nPmacId								 = %u\n", param.nPmacId);
	printf("bAPadEna							= %u\n", param.bAPadEna);
	printf("bPadEna								 = %u\n", param.bPadEna);
	printf("bVPadEna 							 = %u\n", param.bVPadEna);
	printf("bSVPadEna							 = %u\n", param.bSVPadEna);
	printf("bRxFCSDis							 = %u\n", param.bRxFCSDis);
	printf("bTxFCSDis							 = %u\n", param.bTxFCSDis);
	printf("bIPTransChkRegDis					 = %u\n", param.bIPTransChkRegDis);
	printf("bIPTransChkVerDis					 = %u\n", param.bIPTransChkVerDis);
	printf("bJumboEna							 = %u\n", param.bJumboEna);
	printf("nMaxJumboLen 						 = %u\n", param.nMaxJumboLen);
	printf("nJumboThreshLen						 = %u\n", param.nJumboThreshLen);
	printf("bLongFrmChkDis						 = %u\n", param.bLongFrmChkDis);
	printf("eShortFrmChkType 					 = %u\n", param.eShortFrmChkType);
	printf("bProcFlagsEgCfgEna					 = %u\n", param.bProcFlagsEgCfgEna);
	printf("eProcFlagsEgCfg						 = %u\n", param.eProcFlagsEgCfg);

	for (i = 0; i <= 2; i++)
		printf("nBslThreshold[%d]					 = %u\n", i, param.nBslThreshold[i]);

	return 0;
}

int gsw_ctp_port_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portAssignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nNumberOfCtpPort", sizeof(sVar.nNumberOfCtpPort), &sVar.nNumberOfCtpPort);
	cnt += scanParamArg(argc, argv, "eMode", sizeof(sVar.eMode), &sVar.eMode);
	cnt += scanParamArg(argc, argv, "nBridgePortId", sizeof(sVar.nBridgePortId), &sVar.nBridgePortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_ASSIGNMENT_ALLOC, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_ctp_port_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portAssignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_ASSIGNMENT_FREE, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_ctp_port_assigment_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portAssignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nFirstCtpPortId", sizeof(sVar.nFirstCtpPortId), &sVar.nFirstCtpPortId);
	cnt += scanParamArg(argc, argv, "nNumberOfCtpPort", sizeof(sVar.nNumberOfCtpPort), &sVar.nNumberOfCtpPort);
	cnt += scanParamArg(argc, argv, "eMode", sizeof(sVar.eMode), &sVar.eMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_ASSIGNMENT_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_ctp_port_assigment_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portAssignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_ASSIGNMENT_GET, &sVar) != 0)
		return (-1);



	printf("\n\t nLogicalPortId         = %u", sVar.nLogicalPortId);
	printf("\n\t nFirstCtpPortId        = %u", sVar.nFirstCtpPortId);
	printf("\n\t nNumberOfCtpPort       = %u", sVar.nNumberOfCtpPort);
	printf("\n\t eMode                  = %u", sVar.eMode);
	printf("\n");

	return 0;
}


int gsw_extendedvlan_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_EXTENDEDVLAN_alloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nNumberOfEntries", sizeof(sVar.nNumberOfEntries), &sVar.nNumberOfEntries);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_EXTENDEDVLAN_ALLOC, &sVar) != 0)
		return (-1);

	printf("\n\tAllocated ExtendedVlanblock = %u", sVar.nExtendedVlanBlockId);
	printf("\n\t Number of block entries associated with ExtendedVlanblock %d	= %u\n",
	       sVar.nExtendedVlanBlockId, sVar.nNumberOfEntries);
	return 0;
}


int gsw_vlanfilter_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLANFILTER_alloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nNumberOfEntries", sizeof(sVar.nNumberOfEntries), &sVar.nNumberOfEntries);
	cnt += scanParamArg(argc, argv, "bDiscardUntagged", sizeof(sVar.bDiscardUntagged), &sVar.bDiscardUntagged);
	cnt += scanParamArg(argc, argv, "bDiscardUnmatchedTagged", sizeof(sVar.bDiscardUnmatchedTagged), &sVar.bDiscardUnmatchedTagged);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLANFILTER_ALLOC, &sVar) != 0)
		return (-1);

	printf("\n\tAllocated VlanFilterblock = %u", sVar.nVlanFilterBlockId);
	printf("\n\t Number of block entries associated with VlanFilterblock %d	= %u",
	       sVar.nVlanFilterBlockId, sVar.nNumberOfEntries);
	return 0;
}


int gsw_extendedvlan_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_EXTENDEDVLAN_config_t sVar;
	unsigned int cnt = 0;
	unsigned char bDscp2PcpMapEnable = 0, nDscp2PcpMapValue = 0, f = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nExtendedVlanBlockId", sizeof(sVar.nExtendedVlanBlockId), &sVar.nExtendedVlanBlockId);
	cnt += scanParamArg(argc, argv, "nEntryIndex", sizeof(sVar.nEntryIndex), &sVar.nEntryIndex);

	cnt += scanParamArg(argc, argv, "eOuterVlanFilterVlanType", sizeof(sVar.sFilter.sOuterVlan.eType), &sVar.sFilter.sOuterVlan.eType);
	cnt += scanParamArg(argc, argv, "bOuterVlanFilterPriorityEnable", sizeof(sVar.sFilter.sOuterVlan.bPriorityEnable), &sVar.sFilter.sOuterVlan.bPriorityEnable);
	cnt += scanParamArg(argc, argv, "nOuterVlanFilterPriorityVal", sizeof(sVar.sFilter.sOuterVlan.nPriorityVal), &sVar.sFilter.sOuterVlan.nPriorityVal);
	cnt += scanParamArg(argc, argv, "bOuterVlanFilterVidEnable", sizeof(sVar.sFilter.sOuterVlan.bVidEnable), &sVar.sFilter.sOuterVlan.bVidEnable);
	cnt += scanParamArg(argc, argv, "nOuterVlanFilterVidVal", sizeof(sVar.sFilter.sOuterVlan.nVidVal), &sVar.sFilter.sOuterVlan.nVidVal);
	cnt += scanParamArg(argc, argv, "eOuterVlanFilterTpid", sizeof(sVar.sFilter.sOuterVlan.eTpid), &sVar.sFilter.sOuterVlan.eTpid);
	cnt += scanParamArg(argc, argv, "eOuterVlanFilterDei", sizeof(sVar.sFilter.sOuterVlan.eDei), &sVar.sFilter.sOuterVlan.eDei);

	cnt += scanParamArg(argc, argv, "eInnerVlanFilterVlanType", sizeof(sVar.sFilter.sInnerVlan.eType), &sVar.sFilter.sInnerVlan.eType);
	cnt += scanParamArg(argc, argv, "bInnerVlanFilterPriorityEnable", sizeof(sVar.sFilter.sInnerVlan.bPriorityEnable), &sVar.sFilter.sInnerVlan.bPriorityEnable);
	cnt += scanParamArg(argc, argv, "nInnerVlanFilterPriorityVal", sizeof(sVar.sFilter.sInnerVlan.nPriorityVal), &sVar.sFilter.sInnerVlan.nPriorityVal);
	cnt += scanParamArg(argc, argv, "bInnerVlanFilterVidEnable", sizeof(sVar.sFilter.sInnerVlan.bVidEnable), &sVar.sFilter.sInnerVlan.bVidEnable);
	cnt += scanParamArg(argc, argv, "nInnerVlanFilterVidVal", sizeof(sVar.sFilter.sInnerVlan.nVidVal), &sVar.sFilter.sInnerVlan.nVidVal);
	cnt += scanParamArg(argc, argv, "eInnerVlanFilterTpid", sizeof(sVar.sFilter.sInnerVlan.eTpid), &sVar.sFilter.sInnerVlan.eTpid);
	cnt += scanParamArg(argc, argv, "eInnerVlanFilterDei", sizeof(sVar.sFilter.sInnerVlan.eDei), &sVar.sFilter.sInnerVlan.eDei);


	cnt += scanParamArg(argc, argv, "eEtherType", sizeof(sVar.sFilter.eEtherType), &sVar.sFilter.eEtherType);
	cnt += scanParamArg(argc, argv, "eRemoveTagAction", sizeof(sVar.sTreatment.eRemoveTag), &sVar.sTreatment.eRemoveTag);

	cnt += scanParamArg(argc, argv, "bOuterVlanActionEnable", sizeof(sVar.sTreatment.bAddOuterVlan), &sVar.sTreatment.bAddOuterVlan);
	cnt += scanParamArg(argc, argv, "eOuterVlanActionPriorityMode", sizeof(sVar.sTreatment.sOuterVlan.ePriorityMode), &sVar.sTreatment.sOuterVlan.ePriorityMode);
	cnt += scanParamArg(argc, argv, "eOuterVlanActionPriorityVal", sizeof(sVar.sTreatment.sOuterVlan.ePriorityVal), &sVar.sTreatment.sOuterVlan.ePriorityVal);
	cnt += scanParamArg(argc, argv, "eOuterVlanActionVidMode", sizeof(sVar.sTreatment.sOuterVlan.eVidMode), &sVar.sTreatment.sOuterVlan.eVidMode);
	cnt += scanParamArg(argc, argv, "eOuterVlanActionVidVal", sizeof(sVar.sTreatment.sOuterVlan.eVidVal), &sVar.sTreatment.sOuterVlan.eVidVal);
	cnt += scanParamArg(argc, argv, "eOuterVlanActionTpid", sizeof(sVar.sTreatment.sOuterVlan.eTpid), &sVar.sTreatment.sOuterVlan.eTpid);
	cnt += scanParamArg(argc, argv, "eOuterVlanActioneDei", sizeof(sVar.sTreatment.sOuterVlan.eDei), &sVar.sTreatment.sOuterVlan.eDei);

	cnt += scanParamArg(argc, argv, "bInnerVlanActionEnable", sizeof(sVar.sTreatment.bAddInnerVlan), &sVar.sTreatment.bAddInnerVlan);
	cnt += scanParamArg(argc, argv, "eInnerVlanActionPriorityMode", sizeof(sVar.sTreatment.sInnerVlan.ePriorityMode), &sVar.sTreatment.sInnerVlan.ePriorityMode);
	cnt += scanParamArg(argc, argv, "eInnerVlanActionPriorityVal", sizeof(sVar.sTreatment.sInnerVlan.ePriorityVal), &sVar.sTreatment.sInnerVlan.ePriorityVal);
	cnt += scanParamArg(argc, argv, "eInnerVlanActionVidMode", sizeof(sVar.sTreatment.sInnerVlan.eVidMode), &sVar.sTreatment.sInnerVlan.eVidMode);
	cnt += scanParamArg(argc, argv, "eInnerVlanActionVidVal", sizeof(sVar.sTreatment.sInnerVlan.eVidVal), &sVar.sTreatment.sInnerVlan.eVidVal);
	cnt += scanParamArg(argc, argv, "eInnerVlanActionTpid", sizeof(sVar.sTreatment.sInnerVlan.eTpid), &sVar.sTreatment.sInnerVlan.eTpid);
	cnt += scanParamArg(argc, argv, "eInnerVlanActioneDei", sizeof(sVar.sTreatment.sInnerVlan.eDei), &sVar.sTreatment.sInnerVlan.eDei);

	cnt += scanParamArg(argc, argv, "bReassignBridgePortEnable", sizeof(sVar.sTreatment.bReassignBridgePort), &sVar.sTreatment.bReassignBridgePort);
	cnt += scanParamArg(argc, argv, "nNewBridgePortId", sizeof(sVar.sTreatment.nNewBridgePortId), &sVar.sTreatment.nNewBridgePortId);

	cnt += scanParamArg(argc, argv, "bNewDscpEnable", sizeof(sVar.sTreatment.bNewDscpEnable), &sVar.sTreatment.bNewDscpEnable);
	cnt += scanParamArg(argc, argv, "nNewDscp", sizeof(sVar.sTreatment.nNewDscp), &sVar.sTreatment.nNewDscp);

	cnt += scanParamArg(argc, argv, "bNewTrafficClassEnable", sizeof(sVar.sTreatment.bNewTrafficClassEnable), &sVar.sTreatment.bNewTrafficClassEnable);
	cnt += scanParamArg(argc, argv, "nNewTrafficClass", sizeof(sVar.sTreatment.nNewTrafficClass), &sVar.sTreatment.nNewTrafficClass);

	cnt += scanParamArg(argc, argv, "bNewMeterEnable", sizeof(sVar.sTreatment.bNewMeterEnable), &sVar.sTreatment.bNewMeterEnable);
	cnt += scanParamArg(argc, argv, "sNewTrafficMeterId", sizeof(sVar.sTreatment.sNewTrafficMeterId), &sVar.sTreatment.sNewTrafficMeterId);

	cnt += scanParamArg(argc, argv, "bLoopbackEnable", sizeof(sVar.sTreatment.bLoopbackEnable), &sVar.sTreatment.bLoopbackEnable);
	cnt += scanParamArg(argc, argv, "bDaSaSwapEnable", sizeof(sVar.sTreatment.bDaSaSwapEnable), &sVar.sTreatment.bDaSaSwapEnable);
	cnt += scanParamArg(argc, argv, "bMirrorEnable", sizeof(sVar.sTreatment.bMirrorEnable), &sVar.sTreatment.bMirrorEnable);

	cnt += scanParamArg(argc, argv, "bDscp2PcpMapEnable", sizeof(bDscp2PcpMapEnable), &bDscp2PcpMapEnable);
	cnt += scanParamArg(argc, argv, "nDscp2PcpMapValue", sizeof(nDscp2PcpMapValue), &nDscp2PcpMapValue);

	if (bDscp2PcpMapEnable) {
		for (f = 0; f < 64; f++)
			sVar.sTreatment.nDscp2PcpMap[f] = nDscp2PcpMapValue;
	}

	cnt += scanParamArg(argc, argv, "bOriginalPacketFilterMode", sizeof(sVar.sFilter.bOriginalPacketFilterMode), &sVar.sFilter.bOriginalPacketFilterMode);
	cnt += scanParamArg(argc, argv, "eFilter_4_Tpid_Mode", sizeof(sVar.sFilter.eFilter_4_Tpid_Mode), &sVar.sFilter.eFilter_4_Tpid_Mode);
	cnt += scanParamArg(argc, argv, "eTreatment_4_Tpid_Mode", sizeof(sVar.sTreatment.eTreatment_4_Tpid_Mode), &sVar.sTreatment.eTreatment_4_Tpid_Mode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_EXTENDEDVLAN_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_extendedvlan_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_EXTENDEDVLAN_config_t sVar;
	unsigned int cnt = 0;
	unsigned char f = 0;
	memset(&sVar, 0x00, sizeof(sVar));



	cnt += scanParamArg(argc, argv, "nExtendedVlanBlockId", sizeof(sVar.nExtendedVlanBlockId), &sVar.nExtendedVlanBlockId);
	cnt += scanParamArg(argc, argv, "nEntryIndex", sizeof(sVar.nEntryIndex), &sVar.nEntryIndex);


	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_EXTENDEDVLAN_GET, &sVar) != 0)
		return (-1);


	printf("\n\t nExtendedVlanBlockId                 = %u", sVar.nExtendedVlanBlockId);
	printf("\n\t nEntryIndex                          = %u", sVar.nEntryIndex);
	printf("\n\t eOuterVlanFilterVlanType             = %u", sVar.sFilter.sOuterVlan.eType);
	printf("\n\t bOuterVlanFilterPriorityEnable       = %u", sVar.sFilter.sOuterVlan.bPriorityEnable);
	printf("\n\t nOuterVlanFilterPriorityVal          = %u", sVar.sFilter.sOuterVlan.nPriorityVal);
	printf("\n\t bOuterVlanFilterVidEnable            = %u", sVar.sFilter.sOuterVlan.bVidEnable);
	printf("\n\t nOuterVlanFilterVidVal               = %u", sVar.sFilter.sOuterVlan.nVidVal);
	printf("\n\t eOuterVlanFilterTpid                 = %u", sVar.sFilter.sOuterVlan.eTpid);
	printf("\n\t eOuterVlanFilterDei                  = %u", sVar.sFilter.sOuterVlan.eDei);

	printf("\n\t eInnerVlanFilterVlanType             = %u", sVar.sFilter.sInnerVlan.eType);
	printf("\n\t bInnerVlanFilterPriorityEnable       = %u", sVar.sFilter.sInnerVlan.bPriorityEnable);
	printf("\n\t nInnerVlanFilterPriorityVal          = %u", sVar.sFilter.sInnerVlan.nPriorityVal);
	printf("\n\t bInnerVlanFilterVidEnable            = %u", sVar.sFilter.sInnerVlan.bVidEnable);
	printf("\n\t nInnerVlanFilterVidVal               = %u", sVar.sFilter.sInnerVlan.nVidVal);
	printf("\n\t eInnerVlanFilterTpid                 = %u", sVar.sFilter.sInnerVlan.eTpid);
	printf("\n\t eInnerVlanFilterDei                  = %u", sVar.sFilter.sInnerVlan.eDei);
	printf("\n\t eEtherType                           = %u", sVar.sFilter.eEtherType);

	printf("\n\t eRemoveTagAction                     = %u", sVar.sTreatment.eRemoveTag);
	printf("\n\t bOuterVlanActionEnable               = %u", sVar.sTreatment.bAddOuterVlan);
	printf("\n\t eOuterVlanActionPriorityMode         = %u", sVar.sTreatment.sOuterVlan.ePriorityMode);
	printf("\n\t eOuterVlanActionPriorityVal          = %u", sVar.sTreatment.sOuterVlan.ePriorityVal);
	printf("\n\t eOuterVlanActionVidMode              = %u", sVar.sTreatment.sOuterVlan.eVidMode);
	printf("\n\t eOuterVlanActionVidVal               = %u", sVar.sTreatment.sOuterVlan.eVidVal);
	printf("\n\t eOuterVlanActionTpid                 = %u", sVar.sTreatment.sOuterVlan.eTpid);
	printf("\n\t eOuterVlanActioneDei                 = %u", sVar.sTreatment.sOuterVlan.eDei);


	printf("\n\t bInnerVlanActionEnable               = %u", sVar.sTreatment.bAddInnerVlan);
	printf("\n\t eInnerVlanActionPriorityMode         = %u", sVar.sTreatment.sInnerVlan.ePriorityMode);
	printf("\n\t eInnerVlanActionPriorityVal          = %u", sVar.sTreatment.sInnerVlan.ePriorityVal);
	printf("\n\t eInnerVlanActionVidMode              = %u", sVar.sTreatment.sInnerVlan.eVidMode);
	printf("\n\t eInnerVlanActionVidVal               = %u", sVar.sTreatment.sInnerVlan.eVidVal);
	printf("\n\t eInnerVlanActionTpid                 = %u", sVar.sTreatment.sInnerVlan.eTpid);
	printf("\n\t eInnerVlanActioneDei                 = %u", sVar.sTreatment.sInnerVlan.eDei);

	printf("\n\t bNewDscpEnable                       = %u", sVar.sTreatment.bNewDscpEnable);
	printf("\n\t nNewDscp                             = %u", sVar.sTreatment.nNewDscp);
	printf("\n\t bNewTrafficClassEnable               = %u", sVar.sTreatment.bNewTrafficClassEnable);
	printf("\n\t nNewTrafficClass                     = %u", sVar.sTreatment.nNewTrafficClass);
	printf("\n\t bNewMeterEnable                      = %u", sVar.sTreatment.bNewMeterEnable);
	printf("\n\t sNewTrafficMeterId                   = %u", sVar.sTreatment.sNewTrafficMeterId);
	printf("\n\t bLoopbackEnable                      = %u", sVar.sTreatment.bLoopbackEnable);
	printf("\n\t bDaSaSwapEnable                      = %u", sVar.sTreatment.bDaSaSwapEnable);
	printf("\n\t bMirrorEnable                        = %u", sVar.sTreatment.bMirrorEnable);
	printf("\n\t bReassignBridgePortEnable            = %u", sVar.sTreatment.bReassignBridgePort);
	printf("\n\t nNewBridgePortId                     = %u", sVar.sTreatment.nNewBridgePortId);
	printf("\n");

	if (sVar.sTreatment.sOuterVlan.ePriorityMode == GSW_EXTENDEDVLAN_TREATMENT_DSCP ||
	    sVar.sTreatment.sInnerVlan.ePriorityMode == GSW_EXTENDEDVLAN_TREATMENT_DSCP) {
		for (f = 0; f < 64; f++)
			printf("\n\t nDscp2PcpMap[%d]                     = %u", f, sVar.sTreatment.nDscp2PcpMap[f]);
	}

	cnt += scanParamArg(argc, argv, "bOriginalPacketFilterMode", sizeof(sVar.sFilter.bOriginalPacketFilterMode), &sVar.sFilter.bOriginalPacketFilterMode);
	cnt += scanParamArg(argc, argv, "eFilter_4_Tpid_Mode", sizeof(sVar.sFilter.eFilter_4_Tpid_Mode), &sVar.sFilter.eFilter_4_Tpid_Mode);
	cnt += scanParamArg(argc, argv, "eTreatment_4_Tpid_Mode", sizeof(sVar.sTreatment.eTreatment_4_Tpid_Mode), &sVar.sTreatment.eTreatment_4_Tpid_Mode);
	printf("\n\t bOriginalPacketFilterMode            = %u", sVar.sFilter.bOriginalPacketFilterMode);
	printf("\n\t eFilter_4_Tpid_Mode                  = %u", sVar.sFilter.eFilter_4_Tpid_Mode);
	printf("\n\t eTreatment_4_Tpid_Mode               = %u", sVar.sTreatment.eTreatment_4_Tpid_Mode);

	return 0;
}

int gsw_extendedvlan_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_EXTENDEDVLAN_alloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nExtendedVlanBlockId", sizeof(sVar.nExtendedVlanBlockId), &sVar.nExtendedVlanBlockId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_EXTENDEDVLAN_FREE, &sVar) != 0)
		return (-1);

	printf("\n\t Number of deleted entries associated with ExVlanblock %d  = %u",
	       sVar.nExtendedVlanBlockId, sVar.nNumberOfEntries);
	return 0;
}

int gsw_vlanfilter_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLANFILTER_config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nVlanFilterBlockId", sizeof(sVar.nVlanFilterBlockId), &sVar.nVlanFilterBlockId);
	cnt += scanParamArg(argc, argv, "nEntryIndex", sizeof(sVar.nEntryIndex), &sVar.nEntryIndex);
	cnt += scanParamArg(argc, argv, "eVlanFilterMask", sizeof(sVar.eVlanFilterMask), &sVar.eVlanFilterMask);
	cnt += scanParamArg(argc, argv, "nVal", sizeof(sVar.nVal), &sVar.nVal);
	cnt += scanParamArg(argc, argv, "bDiscardMatched", sizeof(sVar.bDiscardMatched), &sVar.bDiscardMatched);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLANFILTER_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_vlanfilter_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLANFILTER_config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nVlanFilterBlockId", sizeof(sVar.nVlanFilterBlockId), &sVar.nVlanFilterBlockId);
	cnt += scanParamArg(argc, argv, "nEntryIndex", sizeof(sVar.nEntryIndex), &sVar.nEntryIndex);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLANFILTER_GET, &sVar) != 0)
		return (-1);

	printf("\n\t nVlanFilterBlockId                 = %u", sVar.nVlanFilterBlockId);
	printf("\n\t nEntryIndex                        = %u", sVar.nEntryIndex);
	printf("\n\t eVlanFilterMask                    = %u", sVar.eVlanFilterMask);
	printf("\n\t nVal                               = %u", sVar.nVal);
	printf("\n\t bDiscardMatched                    = %u", sVar.bDiscardMatched);
	printf("\n");
	return 0;
}

int gsw_vlanfilter_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLANFILTER_alloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nVlanFilterBlockId", sizeof(sVar.nVlanFilterBlockId), &sVar.nVlanFilterBlockId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_VLANFILTER_FREE, &sVar) != 0)
		return (-1);

	printf("\n\t Number of deleted entries associated with VlanFilterblock %d  = %u",
	       sVar.nVlanFilterBlockId, sVar.nNumberOfEntries);
	return 0;
}

int gsw_bridge_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgeId", sizeof(sVar.nBridgeId), &sVar.nBridgeId);
	cnt += scanParamArg(argc, argv, "bMacLearningLimitEnable", sizeof(sVar.bMacLearningLimitEnable), &sVar.bMacLearningLimitEnable);

	if (findStringParam(argc, argv, "bMacLearningLimitEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_MAC_LEARNING_LIMIT;

	cnt += scanParamArg(argc, argv, "nMacLearningLimit", sizeof(sVar.nMacLearningLimit), &sVar.nMacLearningLimit);

	cnt += scanParamArg(argc, argv, "eForwardBroadcast", sizeof(sVar.eForwardBroadcast), &sVar.eForwardBroadcast);

	if (findStringParam(argc, argv, "eForwardBroadcast"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE;

	cnt += scanParamArg(argc, argv, "eForwardUnknownMulticastIp", sizeof(sVar.eForwardUnknownMulticastIp), &sVar.eForwardUnknownMulticastIp);

	if (findStringParam(argc, argv, "eForwardUnknownMulticastIp"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE;

	cnt += scanParamArg(argc, argv, "eForwardUnknownMulticastNonIp", sizeof(sVar.eForwardUnknownMulticastNonIp), &sVar.eForwardUnknownMulticastNonIp);

	if (findStringParam(argc, argv, "eForwardUnknownMulticastNonIp"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE;

	cnt += scanParamArg(argc, argv, "eForwardUnknownUnicast", sizeof(sVar.eForwardUnknownUnicast), &sVar.eForwardUnknownUnicast);

	if (findStringParam(argc, argv, "eForwardUnknownUnicast"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE;


	cnt += scanParamArg(argc, argv, "bBroadcastMeterEnable", sizeof(sVar.bSubMeteringEnable[0]), &sVar.bSubMeteringEnable[0]);

	if (findStringParam(argc, argv, "bBroadcastMeterEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_SUB_METER;

	cnt += scanParamArg(argc, argv, "nBroadcastMeterId", sizeof(sVar.nTrafficSubMeterId[0]), &sVar.nTrafficSubMeterId[0]);

	cnt += scanParamArg(argc, argv, "bMulticastMeterEnable", sizeof(sVar.bSubMeteringEnable[1]), &sVar.bSubMeteringEnable[1]);

	if (findStringParam(argc, argv, "bMulticastMeterEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_SUB_METER;

	cnt += scanParamArg(argc, argv, "nMulticastMeterId", sizeof(sVar.nTrafficSubMeterId[1]), &sVar.nTrafficSubMeterId[1]);

	cnt += scanParamArg(argc, argv, "bUnknownMulticastIpMeterEnable", sizeof(sVar.bSubMeteringEnable[2]), &sVar.bSubMeteringEnable[2]);

	if (findStringParam(argc, argv, "bUnknownMulticastIpMeterEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_SUB_METER;

	cnt += scanParamArg(argc, argv, "nUnknownMulticastIpMeterId", sizeof(sVar.nTrafficSubMeterId[2]), &sVar.nTrafficSubMeterId[2]);

	cnt += scanParamArg(argc, argv, "bUnknownMulticastNonIpMeterEnable", sizeof(sVar.bSubMeteringEnable[3]), &sVar.bSubMeteringEnable[3]);

	if (findStringParam(argc, argv, "bUnknownMulticastNonIpMeterEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_SUB_METER;

	cnt += scanParamArg(argc, argv, "nUnknownMulticastNonIpMeterId", sizeof(sVar.nTrafficSubMeterId[3]), &sVar.nTrafficSubMeterId[3]);

	cnt += scanParamArg(argc, argv, "bUnknownUniCastMeterEnable", sizeof(sVar.bSubMeteringEnable[4]), &sVar.bSubMeteringEnable[4]);

	if (findStringParam(argc, argv, "bUnknownUniCastMeterEnable"))
		sVar.eMask |=  GSW_BRIDGE_CONFIG_MASK_SUB_METER;

	cnt += scanParamArg(argc, argv, "nUnknownUniCastMeterId", sizeof(sVar.nTrafficSubMeterId[4]), &sVar.nTrafficSubMeterId[4]);

	if (findStringParam(argc, argv, "bForce")) {
		cnt += 1;
		sVar.eMask |=  GSW_CTP_PORT_CONFIG_MASK_FORCE;
	}

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_BRIDGE_CONFIG_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_bridge_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgeId", sizeof(sVar.nBridgeId), &sVar.nBridgeId);
	cnt += scanParamArg(argc, argv, "eMask", sizeof(sVar.eMask), &sVar.eMask);

	if (cnt != numPar)
		return (-2);

	if (!sVar.eMask)
		sVar.eMask = 0xFFFFFFFF;


	if (cli_ioctl(fd, GSW_BRIDGE_CONFIG_GET, &sVar) != 0)
		return (-1);

	printf("\n\t nBridgeId                          = %u", sVar.nBridgeId);
	printf("\n\t eMask                              = 0x%x", sVar.eMask);
	printf("\n\t bMacLearningLimitEnable            = %u", sVar.bMacLearningLimitEnable);
	printf("\n\t nMacLearningLimit                  = %u", sVar.nMacLearningLimit);
	printf("\n\t nMacLearningCount                  = %u", sVar.nMacLearningCount);
	printf("\n\t nLearningDiscardEvent              = %u", sVar.nLearningDiscardEvent);
	printf("\n\t eForwardBroadcast                  = %u", sVar.eForwardBroadcast);
	printf("\n\t eForwardUnknownMulticastIp         = %u", sVar.eForwardUnknownMulticastIp);
	printf("\n\t eForwardUnknownMulticastNonIp      = %u", sVar.eForwardUnknownMulticastNonIp);
	printf("\n\t eForwardUnknownUnicast             = %u", sVar.eForwardUnknownUnicast);
	printf("\n\t bBroadcastMeterEnable              = %u", sVar.bSubMeteringEnable[0]);
	printf("\n\t nBroadcastMeterId                  = %u", sVar.nTrafficSubMeterId[0]);
	printf("\n\t bMulticastMeterEnable              = %u", sVar.bSubMeteringEnable[1]);
	printf("\n\t nMulticastMeterId                  = %u", sVar.nTrafficSubMeterId[1]);
	printf("\n\t bUnknownMulticastIpMeterEnable     = %u", sVar.bSubMeteringEnable[2]);
	printf("\n\t nUnknownMulticastIpMeterId         = %u", sVar.nTrafficSubMeterId[2]);
	printf("\n\t bUnknownMulticastNonIpMeterEnable  = %u", sVar.bSubMeteringEnable[3]);
	printf("\n\t nUnknownMulticastNonIpMeterId      = %u", sVar.nTrafficSubMeterId[3]);
	printf("\n\t bUnknownUniCastMeterEnable         = %u", sVar.bSubMeteringEnable[4]);
	printf("\n\t nUnknownUniCastMeterId             = %u", sVar.nTrafficSubMeterId[4]);
	printf("\n");

	return 0;
}


int gsw_bridge_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_alloc_t sVar;
	memset(&sVar, 0x00, sizeof(sVar));

	if (cli_ioctl(fd, GSW_BRIDGE_ALLOC, &sVar) != 0)
		return (-1);

	printf("\n\t Allocated Bridge ID             = %u", sVar.nBridgeId);
	return 0;
}


int gsw_bridge_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_alloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgeId", sizeof(sVar.nBridgeId), &sVar.nBridgeId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_BRIDGE_FREE, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_ctp_port_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portConfig_t sVar;
	unsigned int cnt = 0, i;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(sVar.nSubIfIdGroup), &sVar.nSubIfIdGroup);
	cnt += scanParamArg(argc, argv, "nBridgePortId", sizeof(sVar.nBridgePortId), &sVar.nBridgePortId);

	if (findStringParam(argc, argv, "nBridgePortId"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID;

	cnt += scanParamArg(argc, argv, "bForcedTrafficClass", sizeof(sVar.bForcedTrafficClass), &sVar.bForcedTrafficClass);

	if (findStringParam(argc, argv, "bForcedTrafficClass"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS;

	cnt += scanParamArg(argc, argv, "nDefaultTrafficClass", sizeof(sVar.nDefaultTrafficClass), &sVar.nDefaultTrafficClass);

	if (findStringParam(argc, argv, "nDefaultTrafficClass"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_FORCE_TRAFFIC_CLASS;

	cnt += scanParamArg(argc, argv, "bIngressExtendedVlanEnable", sizeof(sVar.bIngressExtendedVlanEnable), &sVar.bIngressExtendedVlanEnable);

	if (findStringParam(argc, argv, "bIngressExtendedVlanEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;

	cnt += scanParamArg(argc, argv, "nIngressExtendedVlanBlockId", sizeof(sVar.nIngressExtendedVlanBlockId), &sVar.nIngressExtendedVlanBlockId);

	cnt += scanParamArg(argc, argv, "bIngressExtendedVlanIgmpEnable", sizeof(sVar.bIngressExtendedVlanIgmpEnable), &sVar.bIngressExtendedVlanIgmpEnable);

	if (findStringParam(argc, argv, "bIngressExtendedVlanIgmpEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP;

	cnt += scanParamArg(argc, argv, "nIngressExtendedVlanBlockIdIgmp", sizeof(sVar.nIngressExtendedVlanBlockIdIgmp), &sVar.nIngressExtendedVlanBlockIdIgmp);

	cnt += scanParamArg(argc, argv, "bEgressExtendedVlanEnable", sizeof(sVar.bEgressExtendedVlanEnable), &sVar.bEgressExtendedVlanEnable);

	if (findStringParam(argc, argv, "bEgressExtendedVlanEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN;

	cnt += scanParamArg(argc, argv, "nEgressExtendedVlanBlockId", sizeof(sVar.nEgressExtendedVlanBlockId), &sVar.nEgressExtendedVlanBlockId);

	cnt += scanParamArg(argc, argv, "bEgressExtendedVlanIgmpEnable", sizeof(sVar.bEgressExtendedVlanIgmpEnable), &sVar.bEgressExtendedVlanIgmpEnable);

	if (findStringParam(argc, argv, "bEgressExtendedVlanIgmpEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP;

	cnt += scanParamArg(argc, argv, "nEgressExtendedVlanBlockIdIgmp", sizeof(sVar.nEgressExtendedVlanBlockIdIgmp), &sVar.nEgressExtendedVlanBlockIdIgmp);

	cnt += scanParamArg(argc, argv, "bIngressNto1VlanEnable", sizeof(sVar.bIngressNto1VlanEnable), &sVar.bIngressNto1VlanEnable);

	if (findStringParam(argc, argv, "bIngressNto1VlanEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_INRESS_NTO1_VLAN;

	cnt += scanParamArg(argc, argv, "bEgressNto1VlanEnable", sizeof(sVar.bEgressNto1VlanEnable), &sVar.bEgressNto1VlanEnable);

	if (findStringParam(argc, argv, "bEgressNto1VlanEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_NTO1_VLAN;

	cnt += scanParamArg(argc, argv, "eIngressMarkingMode", sizeof(sVar.eIngressMarkingMode), &sVar.eIngressMarkingMode);

	if (findStringParam(argc, argv, "eIngressMarkingMode"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_INGRESS_MARKING;

	cnt += scanParamArg(argc, argv, "eEgressMarkingMode", sizeof(sVar.eEgressMarkingMode), &sVar.eEgressMarkingMode);

	if (findStringParam(argc, argv, "eEgressMarkingMode"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_EGRESS_MARKING;

	cnt += scanParamArg(argc, argv, "bEgressMarkingOverrideEnable", sizeof(sVar.bEgressMarkingOverrideEnable), &sVar.bEgressMarkingOverrideEnable);

	if (findStringParam(argc, argv, "bEgressMarkingOverrideEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_EGRESS_MARKING_OVERRIDE;

	cnt += scanParamArg(argc, argv, "eEgressMarkingModeOverride", sizeof(sVar.eEgressMarkingModeOverride), &sVar.eEgressMarkingModeOverride);

	cnt += scanParamArg(argc, argv, "eEgressRemarkingMode", sizeof(sVar.eEgressRemarkingMode), &sVar.eEgressRemarkingMode);

	if (findStringParam(argc, argv, "eEgressRemarkingMode"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_EGRESS_REMARKING;

	cnt += scanParamArg(argc, argv, "bIngressMeteringEnable", sizeof(sVar.bIngressMeteringEnable), &sVar.bIngressMeteringEnable);

	if (findStringParam(argc, argv, "bIngressMeteringEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_INGRESS_METER;

	cnt += scanParamArg(argc, argv, "nIngressTrafficMeterId", sizeof(sVar.nIngressTrafficMeterId), &sVar.nIngressTrafficMeterId);
	cnt += scanParamArg(argc, argv, "bEgressMeteringEnable", sizeof(sVar.bEgressMeteringEnable), &sVar.bEgressMeteringEnable);

	if (findStringParam(argc, argv, "bEgressMeteringEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_EGRESS_METER;

	cnt += scanParamArg(argc, argv, "nEgressTrafficMeterId", sizeof(sVar.nEgressTrafficMeterId), &sVar.nEgressTrafficMeterId);

	cnt += scanParamArg(argc, argv, "bBridgingBypass", sizeof(sVar.bBridgingBypass), &sVar.bBridgingBypass);

	if (findStringParam(argc, argv, "bBridgingBypass"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_BRIDGING_BYPASS;

	cnt += scanParamArg(argc, argv, "nDestLogicalPortId", sizeof(sVar.nDestLogicalPortId), &sVar.nDestLogicalPortId);
	cnt += scanParamArg(argc, argv, "nDestSubIfIdGroup", sizeof(sVar.nDestSubIfIdGroup), &sVar.nDestSubIfIdGroup);
	cnt += scanParamArg(argc, argv, "bPmapperEnable", sizeof(sVar.bPmapperEnable), &sVar.bPmapperEnable);

	if (sVar.bPmapperEnable) {
		cnt += scanParamArg(argc, argv, "ePmapperMappingMode", sizeof(sVar.ePmapperMappingMode), &sVar.ePmapperMappingMode);
		cnt += scanPMAP_Arg(argc, argv, "nPmapperDestSubIfIdGroup", sVar.sPmapper.nDestSubIfIdGroup);
		printf("i.  The first entry of each P-mapper index is for Non-IP and Non-VLAN tagging packets\n");
		printf("ii. The entry 8 to 1 of each P-mapper index is for PCP mapping entries\n");
		printf("iii.The entry 72 to 9 of each P-mapper index is for DSCP mapping entries\n");
		printf("User Configured nDestSubIfIdGroup list as below\n");

		for (i = 0; i <= 72; i++)
			printf("sVar.sPmapper.nDestSubIfIdGroup[%d] = %d\n", i, sVar.sPmapper.nDestSubIfIdGroup[i]);
	}

	cnt += scanParamArg(argc, argv, "nFirstFlowEntryIndex", sizeof(sVar.nFirstFlowEntryIndex), &sVar.nFirstFlowEntryIndex);

	if (findStringParam(argc, argv, "nFirstFlowEntryIndex"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

	cnt += scanParamArg(argc, argv, "nNumberOfFlowEntries", sizeof(sVar.nNumberOfFlowEntries), &sVar.nNumberOfFlowEntries);

	cnt += scanParamArg(argc, argv, "bIngressLoopbackEnable", sizeof(sVar.bIngressLoopbackEnable), &sVar.bIngressLoopbackEnable);

	if (findStringParam(argc, argv, "bIngressLoopbackEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	cnt += scanParamArg(argc, argv, "bIngressDaSaSwapEnable", sizeof(sVar.bIngressDaSaSwapEnable), &sVar.bIngressDaSaSwapEnable);

	if (findStringParam(argc, argv, "bIngressDaSaSwapEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	cnt += scanParamArg(argc, argv, "bEgressLoopbackEnable", sizeof(sVar.bEgressLoopbackEnable), &sVar.bEgressLoopbackEnable);

	if (findStringParam(argc, argv, "bEgressLoopbackEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	cnt += scanParamArg(argc, argv, "bEgressDaSaSwapEnable", sizeof(sVar.bEgressDaSaSwapEnable), &sVar.bEgressDaSaSwapEnable);

	if (findStringParam(argc, argv, "bEgressDaSaSwapEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	cnt += scanParamArg(argc, argv, "bIngressMirrorEnable", sizeof(sVar.bIngressMirrorEnable), &sVar.bIngressMirrorEnable);

	if (findStringParam(argc, argv, "bIngressMirrorEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	cnt += scanParamArg(argc, argv, "bEgressMirrorEnable", sizeof(sVar.bEgressMirrorEnable), &sVar.bEgressMirrorEnable);

	if (findStringParam(argc, argv, "bEgressMirrorEnable"))
		sVar.eMask |= GSW_CTP_PORT_CONFIG_LOOPBACK_AND_MIRROR;

	if (findStringParam(argc, argv, "bForce")) {
		cnt += 1;
		sVar.eMask |=  GSW_CTP_PORT_CONFIG_MASK_FORCE;
	}

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_CONFIG_SET, &sVar) != 0)
		return (-1);

	return 0;
}


int gsw_ctp_port_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portConfig_t sVar;
	unsigned int cnt = 0, i;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(sVar.nSubIfIdGroup), &sVar.nSubIfIdGroup);
	cnt += scanParamArg(argc, argv, "eMask", sizeof(sVar.eMask), &sVar.eMask);

	if (cnt != numPar)
		return (-2);

	if (!sVar.eMask)
		sVar.eMask = 0xFFFFFFFF;


	if (cli_ioctl(fd, GSW_CTP_PORT_CONFIG_GET, &sVar) != 0)
		return (-1);


	printf("\n\t nLogicalPortId                     = %u", sVar.nLogicalPortId);
	printf("\n\t nSubIfIdGroup                      = %u", sVar.nSubIfIdGroup);
	printf("\n\t eMask                              = 0x%x", sVar.eMask);
	printf("\n\t nBridgePortId                      = %u", sVar.nBridgePortId);
	printf("\n\t bForcedTrafficClass                = %u", sVar.bForcedTrafficClass);
	printf("\n\t nDefaultTrafficClass               = %u", sVar.nDefaultTrafficClass);
	printf("\n\t bIngressExtendedVlanEnable         = %u", sVar.bIngressExtendedVlanEnable);
	printf("\n\t nIngressExtendedVlanBlockId        = %u", sVar.nIngressExtendedVlanBlockId);
	printf("\n\t bIngressExtendedVlanIgmpEnable     = %u", sVar.bIngressExtendedVlanIgmpEnable);
	printf("\n\t nIngressExtendedVlanBlockIdIgmp    = %u", sVar.nIngressExtendedVlanBlockIdIgmp);
	printf("\n\t bEgressExtendedVlanEnable          = %u", sVar.bEgressExtendedVlanEnable);
	printf("\n\t nEgressExtendedVlanBlockId         = %u", sVar.nEgressExtendedVlanBlockId);
	printf("\n\t bEgressExtendedVlanIgmpEnable      = %u", sVar.bEgressExtendedVlanIgmpEnable);
	printf("\n\t nEgressExtendedVlanBlockIdIgmp     = %u", sVar.nEgressExtendedVlanBlockIdIgmp);
	printf("\n\t bIngressNto1VlanEnable             = %u", sVar.bIngressNto1VlanEnable);
	printf("\n\t bEgressNto1VlanEnable              = %u", sVar.bEgressNto1VlanEnable);
	printf("\n\t eIngressMarkingMode                = %u", sVar.eIngressMarkingMode);
	printf("\n\t eEgressMarkingMode                 = %u", sVar.eEgressMarkingMode);
	printf("\n\t bEgressMarkingOverrideEnable       = %u", sVar.eEgressMarkingModeOverride);
	printf("\n\t eEgressRemarkingMode               = %u", sVar.eEgressRemarkingMode);
	printf("\n\t bIngressMeteringEnable             = %u", sVar.bIngressMeteringEnable);
	printf("\n\t nIngressTrafficMeterId             = %u", sVar.nIngressTrafficMeterId);
	printf("\n\t bEgressMeteringEnable              = %u", sVar.bEgressMeteringEnable);
	printf("\n\t nEgressTrafficMeterId              = %u", sVar.nEgressTrafficMeterId);
	printf("\n\t bBridgingBypass                    = %u", sVar.bBridgingBypass);
	printf("\n\t nDestLogicalPortId                 = %u", sVar.nDestLogicalPortId);
	printf("\n\t nDestSubIfIdGroup                  = %u", sVar.nDestSubIfIdGroup);
	printf("\n\t bPmapperEnable                     = %u", sVar.bPmapperEnable);
	printf("\n\t ePmapperMappingMode                = %u", sVar.ePmapperMappingMode);
	printf("\n\t nPmapperId                         = %u", sVar.sPmapper.nPmapperId);

	for (i = 0; i < 73; i++)
		printf("\n\t nDestSubIfIdGroup[%u]         		= %u", i, sVar.sPmapper.nDestSubIfIdGroup[i]);

	printf("\n\t nFirstFlowEntryIndex         	   = %u", sVar.nFirstFlowEntryIndex);
	printf("\n\t nNumberOfFlowEntries         	   = %u", sVar.nNumberOfFlowEntries);
	printf("\n\t bIngressDaSaSwapEnable         	   = %u", sVar.bIngressDaSaSwapEnable);
	printf("\n\t bEgressDaSaSwapEnable         	   = %u", sVar.bEgressDaSaSwapEnable);
	printf("\n\t bIngressLoopbackEnable         	   = %u", sVar.bIngressLoopbackEnable);
	printf("\n\t bEgressLoopbackEnable         	   = %u", sVar.bEgressLoopbackEnable);
	printf("\n\t bIngressMirrorEnable         	   = %u", sVar.bIngressMirrorEnable);
	printf("\n\t bEgressMirrorEnable         	   = %u", sVar.bEgressMirrorEnable);

	printf("\n");

	return 0;
}

int gsw_ctp_port_config_reset(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CTP_portConfig_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(sVar.nSubIfIdGroup), &sVar.nSubIfIdGroup);

	if (findStringParam(argc, argv, "bForce")) {
		cnt += 1;
		sVar.eMask |=  GSW_CTP_PORT_CONFIG_MASK_FORCE;
	}

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_CTP_PORT_CONFIG_RESET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_bridge_port_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_portConfig_t sVar;
	unsigned int cnt = 0, i, bBridgePortMapEnable = 0, Index = 0, MapValue = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgePortId", sizeof(sVar.nBridgePortId), &sVar.nBridgePortId);
	cnt += scanParamArg(argc, argv, "nBridgeId", sizeof(sVar.eMask), &sVar.nBridgeId);

	if (findStringParam(argc, argv, "nBridgeId"))
		sVar.eMask |= GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID;

	cnt += scanParamArg(argc, argv, "bIngressExtendedVlanEnable", sizeof(sVar.bIngressExtendedVlanEnable), &sVar.bIngressExtendedVlanEnable);

	if (findStringParam(argc, argv, "bIngressExtendedVlanEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN;

	cnt += scanParamArg(argc, argv, "nIngressExtendedVlanBlockId", sizeof(sVar.nIngressExtendedVlanBlockId), &sVar.nIngressExtendedVlanBlockId);

	cnt += scanParamArg(argc, argv, "bEgressExtendedVlanEnable", sizeof(sVar.bEgressExtendedVlanEnable), &sVar.bEgressExtendedVlanEnable);

	if (findStringParam(argc, argv, "bEgressExtendedVlanEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN;

	cnt += scanParamArg(argc, argv, "nEgressExtendedVlanBlockId", sizeof(sVar.nEgressExtendedVlanBlockId), &sVar.nEgressExtendedVlanBlockId);

	cnt += scanParamArg(argc, argv, "eIngressMarkingMode", sizeof(sVar.eIngressMarkingMode), &sVar.eIngressMarkingMode);

	if (findStringParam(argc, argv, "eIngressMarkingMode"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_MARKING;

	cnt += scanParamArg(argc, argv, "eEgressRemarkingMode", sizeof(sVar.eEgressRemarkingMode), &sVar.eEgressRemarkingMode);

	if (findStringParam(argc, argv, "eEgressRemarkingMode"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_REMARKING;

	cnt += scanParamArg(argc, argv, "bIngressMeteringEnable", sizeof(sVar.bIngressMeteringEnable), &sVar.bIngressMeteringEnable);

	if (findStringParam(argc, argv, "bIngressMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_METER;

	cnt += scanParamArg(argc, argv, "nIngressTrafficMeterId", sizeof(sVar.nIngressTrafficMeterId), &sVar.nIngressTrafficMeterId);

	cnt += scanParamArg(argc, argv, "bEgressMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[5]), &sVar.bEgressSubMeteringEnable[5]);

	if (findStringParam(argc, argv, "bEgressMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "nEgressTrafficMeterId", sizeof(sVar.nEgressTrafficSubMeterId[5]), &sVar.nEgressTrafficSubMeterId[5]);

	cnt += scanParamArg(argc, argv, "bEgressBroadcastSubMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[0]), &sVar.bEgressSubMeteringEnable[0]);

	if (findStringParam(argc, argv, "bEgressBroadcastSubMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "bEgressMulticastSubMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[1]), &sVar.bEgressSubMeteringEnable[1]);

	if (findStringParam(argc, argv, "bEgressMulticastSubMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "bEgressUnknownMulticastIPSubMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[2]), &sVar.bEgressSubMeteringEnable[2]);

	if (findStringParam(argc, argv, "bEgressUnknownMulticastIPSubMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "bEgressUnknownMulticastNonIPSubMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[3]), &sVar.bEgressSubMeteringEnable[3]);

	if (findStringParam(argc, argv, "bEgressUnknownMulticastNonIPSubMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "bEgressUnknownUnicastSubMeteringEnable", sizeof(sVar.bEgressSubMeteringEnable[4]), &sVar.bEgressSubMeteringEnable[4]);

	if (findStringParam(argc, argv, "bEgressUnknownUnicastSubMeteringEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_SUB_METER;

	cnt += scanParamArg(argc, argv, "nEgressBroadcastSubMeteringId", sizeof(sVar.nEgressTrafficSubMeterId[0]), &sVar.nEgressTrafficSubMeterId[0]);
	cnt += scanParamArg(argc, argv, "nEgressMulticastSubMeteringId", sizeof(sVar.nEgressTrafficSubMeterId[1]), &sVar.nEgressTrafficSubMeterId[1]);
	cnt += scanParamArg(argc, argv, "nEgressUnknownMulticastIPSubMeteringId", sizeof(sVar.nEgressTrafficSubMeterId[2]), &sVar.nEgressTrafficSubMeterId[2]);
	cnt += scanParamArg(argc, argv, "nEgressUnknownMulticastNonIPSubMeteringId", sizeof(sVar.nEgressTrafficSubMeterId[3]), &sVar.nEgressTrafficSubMeterId[3]);
	cnt += scanParamArg(argc, argv, "nEgressUnknownUnicastSubMeteringId", sizeof(sVar.nEgressTrafficSubMeterId[4]), &sVar.nEgressTrafficSubMeterId[4]);

	cnt += scanParamArg(argc, argv, "nDestLogicalPortId", sizeof(sVar.nDestLogicalPortId), &sVar.nDestLogicalPortId);

	if (findStringParam(argc, argv, "nDestLogicalPortId"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;

	cnt += scanParamArg(argc, argv, "nDestSubIfIdGroup", sizeof(sVar.nDestSubIfIdGroup), &sVar.nDestSubIfIdGroup);

	if (findStringParam(argc, argv, "nDestSubIfIdGroup"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;

	cnt += scanParamArg(argc, argv, "bPmapperEnable", sizeof(sVar.bPmapperEnable), &sVar.bPmapperEnable);

	if (findStringParam(argc, argv, "bPmapperEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_CTP_MAPPING;

	if (sVar.bPmapperEnable) {
		cnt += scanParamArg(argc, argv, "ePmapperMappingMode", sizeof(sVar.ePmapperMappingMode), &sVar.ePmapperMappingMode);
		cnt += scanPMAP_Arg(argc, argv, "nPmapperDestSubIfIdGroup", sVar.sPmapper.nDestSubIfIdGroup);
		printf("i.  The first entry of each P-mapper index is for Non-IP and Non-VLAN tagging packets\n");
		printf("ii. The entry 8 to 1 of each P-mapper index is for PCP mapping entries\n");
		printf("iii.The entry 72 to 9 of each P-mapper index is for DSCP mapping entries\n");
		printf("User Configured nDestSubIfIdGroup list as below\n");

		for (i = 0; i <= 72; i++)
			printf("sVar.sPmapper.nDestSubIfIdGroup[%d] = %d\n", i, sVar.sPmapper.nDestSubIfIdGroup[i]);

	}

	cnt += scanParamArg(argc, argv, "bBridgePortMapEnable", sizeof(bBridgePortMapEnable), &bBridgePortMapEnable);

	if (findStringParam(argc, argv, "bBridgePortMapEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;

	if (bBridgePortMapEnable) {
		cnt += scanParamArg(argc, argv, "Index", sizeof(Index), &Index);
		cnt += scanParamArg(argc, argv, "MapValue", sizeof(MapValue), &MapValue);
		sVar.nBridgePortMap[Index] = MapValue;
	}

	cnt += scanParamArg(argc, argv, "bMcDestIpLookupDisable", sizeof(sVar.bMcDestIpLookupDisable), &sVar.bMcDestIpLookupDisable);

	if (findStringParam(argc, argv, "bMcDestIpLookupDisable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_IP_LOOKUP;

	cnt += scanParamArg(argc, argv, "bMcSrcIpLookupEnable", sizeof(sVar.bMcSrcIpLookupEnable), &sVar.bMcSrcIpLookupEnable);

	if (findStringParam(argc, argv, "bMcSrcIpLookupEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_IP_LOOKUP;

	cnt += scanParamArg(argc, argv, "bDestMacLookupDisable", sizeof(sVar.bDestMacLookupDisable), &sVar.bDestMacLookupDisable);

	if (findStringParam(argc, argv, "bDestMacLookupDisable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MC_DEST_MAC_LOOKUP;

	cnt += scanParamArg(argc, argv, "bSrcMacLearningDisable", sizeof(sVar.bSrcMacLearningDisable), &sVar.bSrcMacLearningDisable);

	if (findStringParam(argc, argv, "bSrcMacLearningDisable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MC_SRC_MAC_LEARNING;

	cnt += scanParamArg(argc, argv, "bMacSpoofingDetectEnable", sizeof(sVar.bMacSpoofingDetectEnable), &sVar.bMacSpoofingDetectEnable);

	if (findStringParam(argc, argv, "bMacSpoofingDetectEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MAC_SPOOFING;

	cnt += scanParamArg(argc, argv, "bPortLockEnable", sizeof(sVar.bPortLockEnable), &sVar.bPortLockEnable);

	if (findStringParam(argc, argv, "bPortLockEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_PORT_LOCK;

	cnt += scanParamArg(argc, argv, "bMacLearningLimitEnable", sizeof(sVar.bMacLearningLimitEnable), &sVar.bMacLearningLimitEnable);

	if (findStringParam(argc, argv, "bMacLearningLimitEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_MAC_LEARNING_LIMIT;

	cnt += scanParamArg(argc, argv, "nMacLearningLimit", sizeof(sVar.nMacLearningLimit), &sVar.nMacLearningLimit);

	cnt += scanParamArg(argc, argv, "bIngressVlanFilterEnable", sizeof(sVar.bIngressVlanFilterEnable), &sVar.bIngressVlanFilterEnable);

	if (findStringParam(argc, argv, "bIngressVlanFilterEnable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER;

	cnt += scanParamArg(argc, argv, "nIngressVlanFilterBlockId", sizeof(sVar.nIngressVlanFilterBlockId), &sVar.nIngressVlanFilterBlockId);

	cnt += scanParamArg(argc, argv, "bBypassEgressVlanFilter1", sizeof(sVar.bBypassEgressVlanFilter1), &sVar.bBypassEgressVlanFilter1);

	if (findStringParam(argc, argv, "bBypassEgressVlanFilter1"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER;

	cnt += scanParamArg(argc, argv, "bEgressVlanFilter1Enable", sizeof(sVar.bEgressVlanFilter1Enable), &sVar.bEgressVlanFilter1Enable);

	if (findStringParam(argc, argv, "bEgressVlanFilter1Enable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1;

	cnt += scanParamArg(argc, argv, "nEgressVlanFilter1BlockId", sizeof(sVar.nEgressVlanFilter1BlockId), &sVar.nEgressVlanFilter1BlockId);

	cnt += scanParamArg(argc, argv, "bEgressVlanFilter2Enable", sizeof(sVar.bEgressVlanFilter2Enable), &sVar.bEgressVlanFilter2Enable);

	if (findStringParam(argc, argv, "bEgressVlanFilter2Enable"))
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER2;

	cnt += scanParamArg(argc, argv, "nEgressVlanFilter2BlockId", sizeof(sVar.nEgressVlanFilter2BlockId), &sVar.nEgressVlanFilter2BlockId);

	if (findStringParam(argc, argv, "bForce")) {
		cnt += 1;
		sVar.eMask |=  GSW_BRIDGE_PORT_CONFIG_MASK_FORCE;
	}


	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_BRIDGE_PORT_CONFIG_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_bridge_port_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_portConfig_t sVar;
	unsigned int cnt = 0, i;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgePortId", sizeof(sVar.nBridgePortId), &sVar.nBridgePortId);
	cnt += scanParamArg(argc, argv, "eMask", sizeof(sVar.eMask), &sVar.eMask);

	if (cnt != numPar)
		return (-2);


	if (!sVar.eMask)
		sVar.eMask = 0xFFFFFFFF;

	if (cli_ioctl(fd, GSW_BRIDGE_PORT_CONFIG_GET, &sVar) != 0)
		return (-1);

	printf("\n\t nBridgePortId                                      = 0x%x", sVar.nBridgePortId);
	printf("\n\t eMask                                              = 0x%x", sVar.eMask);
	printf("\n\t nBridgeId                                          = 0x%x", sVar.nBridgeId);
	printf("\n\t bIngressExtendedVlanEnable                         = 0x%x", sVar.bIngressExtendedVlanEnable);
	printf("\n\t nIngressExtendedVlanBlockId                        = 0x%x", sVar.nIngressExtendedVlanBlockId);
	printf("\n\t bEgressExtendedVlanEnable                          = 0x%x", sVar.bEgressExtendedVlanEnable);
	printf("\n\t nEgressExtendedVlanBlockId                         = 0x%x", sVar.nEgressExtendedVlanBlockId);
	printf("\n\t eIngressMarkingMode                                = 0x%x", sVar.eIngressMarkingMode);
	printf("\n\t eEgressRemarkingMode                               = 0x%x", sVar.eEgressRemarkingMode);
	printf("\n\t bIngressMeteringEnable                             = 0x%x", sVar.bIngressMeteringEnable);
	printf("\n\t nIngressTrafficMeterId                             = 0x%x", sVar.nIngressTrafficMeterId);
	printf("\n\t bEgressMeteringEnable                              = 0x%x", sVar.bEgressSubMeteringEnable[5]);
	printf("\n\t nEgressTrafficMeterId                              = 0x%x", sVar.nEgressTrafficSubMeterId[5]);
	printf("\n\t bEgressBroadcastSubMeteringEnable                  = 0x%x", sVar.bEgressSubMeteringEnable[0]);
	printf("\n\t bEgressMulticastSubMeteringEnable                  = 0x%x", sVar.bEgressSubMeteringEnable[1]);
	printf("\n\t bEgressUnknownMulticastIPSubMeteringEnable         = 0x%x", sVar.bEgressSubMeteringEnable[2]);
	printf("\n\t bEgressUnknownMulticastNonIPSubMeteringEnable      = 0x%x", sVar.bEgressSubMeteringEnable[3]);
	printf("\n\t bEgressUnknownUnicastSubMeteringEnable             = 0x%x", sVar.bEgressSubMeteringEnable[4]);
	printf("\n\t nEgressBroadcastSubMeteringId                      = 0x%x", sVar.nEgressTrafficSubMeterId[0]);
	printf("\n\t nEgressMulticastSubMeteringId                      = 0x%x", sVar.nEgressTrafficSubMeterId[1]);
	printf("\n\t bEgressUnknownMulticastIPSubMeteringEnable         = 0x%x", sVar.nEgressTrafficSubMeterId[2]);
	printf("\n\t bEgressUnknownMulticastNonIPSubMeteringEnable      = 0x%x", sVar.nEgressTrafficSubMeterId[3]);
	printf("\n\t bEgressUnknownUnicastSubMeteringEnable             = 0x%x", sVar.nEgressTrafficSubMeterId[4]);
	printf("\n\t nDestLogicalPortId                                 = 0x%x", sVar.nDestLogicalPortId);
	printf("\n\t nDestSubIfIdGroup                                  = 0x%x", sVar.nDestSubIfIdGroup);
	printf("\n\t bPmapperEnable                                     = 0x%x", sVar.bPmapperEnable);
	printf("\n\t ePmapperMappingMode                                = 0x%x", sVar.ePmapperMappingMode);
	printf("\n\t nPmapperId                                         = 0x%x", sVar.sPmapper.nPmapperId);

	for (i = 0; i < 73; i++)
		printf("\n\t nDestSubIfIdGroup[%u]                              = 0x%x", i, sVar.sPmapper.nDestSubIfIdGroup[i]);

	for (i = 0; i < 16; i++)
		printf("\n\t nBridgePortMapIndex[%u]                                 = 0x%x", i, sVar.nBridgePortMap[i]);

	printf("\n\t bMcDestIpLookupDisable                             = 0x%x", sVar.bMcDestIpLookupDisable);
	printf("\n\t bMcSrcIpLookupEnable                               = 0x%x", sVar.bMcSrcIpLookupEnable);
	printf("\n\t bDestMacLookupDisable                              = 0x%x", sVar.bDestMacLookupDisable);
	printf("\n\t bSrcMacLearningDisable                             = 0x%x", sVar.bSrcMacLearningDisable);
	printf("\n\t bMacSpoofingDetectEnable                           = 0x%x", sVar.bMacSpoofingDetectEnable);
	printf("\n\t bPortLockEnable                                    = 0x%x", sVar.bPortLockEnable);
	printf("\n\t bMacLearningLimitEnable                            = 0x%x", sVar.bMacLearningLimitEnable);
	printf("\n\t nMacLearningLimit                                  = 0x%x", sVar.nMacLearningLimit);
	printf("\n\t nMacLearningCount                                  = 0x%x", sVar.nMacLearningCount);
	printf("\n\t bIngressVlanFilterEnable                           = 0x%x", sVar.bIngressVlanFilterEnable);
	printf("\n\t nIngressVlanFilterBlockId                          = 0x%x", sVar.nIngressVlanFilterBlockId);
	printf("\n\t bBypassEgressVlanFilter1                           = 0x%x", sVar.bBypassEgressVlanFilter1);
	printf("\n\t bEgressVlanFilter1Enable                           = 0x%x", sVar.bEgressVlanFilter1Enable);
	printf("\n\t nEgressVlanFilter1BlockId                          = 0x%x", sVar.nEgressVlanFilter1BlockId);
	printf("\n\t bEgressVlanFilter2Enable                           = 0x%x", sVar.bEgressVlanFilter2Enable);
	printf("\n\t nEgressVlanFilter2BlockId                          = 0x%x", sVar.nEgressVlanFilter2BlockId);
	printf("\n");

	return 0;
}

int gsw_bridge_port_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_portAlloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_BRIDGE_PORT_ALLOC, &sVar) != 0)
		return (-1);

	printf("\n\t Alocated Bridge Port ID                          = %d", sVar.nBridgePortId);
	return 0;
}

int gsw_bridge_port_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_BRIDGE_portAlloc_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nBridgePortId", sizeof(sVar.nBridgePortId), &sVar.nBridgePortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_BRIDGE_PORT_FREE, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_pmac_rmon_get_all(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Cnt_t eg[16];
	GSW_PMAC_Cnt_t ig[16];
	u32 i = 0, j = 0, cnt = 0;
	u32 start = 0, end = 12;
	u32 max_read = 0;
	u8 pmacId = 0;

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(pmacId), &pmacId);
	cnt += scanParamArg(argc, argv, "Start", sizeof(start), &start);
	cnt += scanParamArg(argc, argv, "End", sizeof(end), &end);

	memset(eg, 0, sizeof(GSW_PMAC_Cnt_t) * 16);
	memset(ig, 0, sizeof(GSW_PMAC_Cnt_t) * 16);

	max_read = end - start;

	if (max_read > 16 || end > 16) {
		printf("Display only 16 ports, please check start and end\n");
		return -1;
	}

	printf("\n");

	for (j = 0; j < 2; j++) {

		printf("Reading PmacId %d:  %s\n", pmacId, (j == 0) ? "Egress" : "Ingress");

		if (j == 0) {	// Getting Egress Pmac rmon
			for (i = 0; i < max_read; i++) {
				eg[i].nPmacId = pmacId;
				eg[i].nTxDmaChanId = start + i;

				if (cli_ioctl(fd, GSW_PMAC_COUNT_GET, &eg[i]) != 0) {
					printf("ioctl returned with ERROR!\n");
					return (-1);
				}
			}
		} else {	// Getting Ingress Pmac rmon
			for (i = 0; i < max_read; i++) {
				ig[i].nPmacId = pmacId;
				ig[i].nTxDmaChanId = start + i;

				if (cli_ioctl(fd, GSW_PMAC_COUNT_GET, &ig[i]) != 0) {
					printf("ioctl returned with ERROR!\n");
					return (-1);
				}
			}
		}
	}

	printf("\n");

	printf("Rx Logical Port                              : ");

	for (i = start; i < end; i++)
		printf("%11u", i);

	printf("\n");
	printf("\n");

	printf("Egress Checksum Error Packet Count           : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", eg[i].nChkSumErrPktsCount);

	printf("\n");

	printf("Egress Checksum Error Byte Count             : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", eg[i].nChkSumErrBytesCount);

	printf("\n");

	printf("Egress Total Packet Count                    : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", eg[i].nEgressPktsCount);

	printf("\n");

	printf("Egress Total Byte Count                      : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", eg[i].nEgressBytesCount);

	printf("\n");



	printf("\n");
	printf("\n");

	printf("DMA TxCh                                     : ");

	for (i = start; i < end; i++)
		printf("%11u", i);

	printf("\n");
	printf("\n");

	printf("Ingress Discard Packet Count                 : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", ig[i].nDiscPktsCount);

	printf("\n");

	printf("Ingress Discard Byte Count                   : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", ig[i].nDiscBytesCount);

	printf("\n");

	printf("Ingress Total Packet Count                   : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", ig[i].nIngressPktsCount);

	printf("\n");

	printf("Ingress Total Byte Count                     : ");

	for (i = 0; i < max_read; i++)
		printf("%11u", ig[i].nIngressBytesCount);

	printf("\n");



	return 0;
}


int gsw_pmac_rmon_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Cnt_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(sVar.nPmacId), &sVar.nPmacId);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.nTxDmaChanId), &sVar.nTxDmaChanId);
	cnt += scanParamArg(argc, argv, "b64BitMode", sizeof(sVar.b64BitMode), &sVar.b64BitMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PMAC_COUNT_GET, &sVar) != 0)
		return (-1);

	printf("\t nPmacId					= %u\n", sVar.nPmacId);
	printf("\t nPortId					= %u\n", sVar.nTxDmaChanId);
	printf("\t Ingress Discard Packet Count			= %u\n", sVar.nDiscPktsCount);
	printf("\t Ingress Discard Byte Count			= %u\n", sVar.nDiscBytesCount);
	printf("\t Egress Checksum Error Packet Count		= %u\n", sVar.nChkSumErrPktsCount);
	printf("\t Egress Checksum Error Byte Count		= %u\n", sVar.nChkSumErrBytesCount);
	printf("\t Ingress Total Packet Count			= %u\n", sVar.nIngressPktsCount);
	printf("\t Ingress Total Byte Count			= %u\n", sVar.nIngressBytesCount);
	printf("\t Egress Total Packet Count			= %u\n", sVar.nEgressPktsCount);
	printf("\t Egress Total Byte Count			= %u\n", sVar.nEgressBytesCount);
	printf("\t Ingress Header Packet Count			= %u\n", sVar.nIngressHdrPktsCount);
	printf("\t Ingress Header Byte Count			= %u\n", sVar.nIngressHdrBytesCount);
	printf("\t Egress Header Packet Count			= %u\n", sVar.nEgressHdrPktsCount);
	printf("\t Egress Header Byte Count			= %u\n", sVar.nEgressHdrBytesCount);
	printf("\t Egress Header Discard Packet Count		= %u\n", sVar.nEgressHdrDiscPktsCount);
	printf("\t Egress Header Discard Byte Count		= %u\n", sVar.nEgressHdrDiscBytesCount);
	return 0;
}



int gsw_debug_ctpstatistics(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_CTP_STATISTICS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_lpstatistics(int argc, char *argv[], void *fd, int numPar)
{
	u32 i = 0, j = 0, k = 0;
	u32 first_port = 0, no_port = 0;
	GSW_CTP_portAssignment_t Assign_get;
	static GSW_CTP_portConfig_t CTP_get;
	static GSW_BRIDGE_portConfig_t BP_get;

	memset(&Assign_get, 0x00, sizeof(Assign_get));
	memset(&CTP_get, 0x00, sizeof(CTP_get));
	memset(&BP_get, 0x00, sizeof(BP_get));

	printf("\n\n");
	printf("\t\t----------------------------------------\n");
	printf("\t\t\t LOGICAL PORT Statistics\n");
	printf("\t\t----------------------------------------\n");

	for (j = 0; j < 12; j++) {
		Assign_get.nLogicalPortId = j;

		if (cli_ioctl(fd, GSW_CTP_PORT_ASSIGNMENT_GET, &Assign_get) != 0) {
			printf("GSW_CTP_PortAssignmentGet returns ERROR\n");
			return (-1);
		}

		printf("\n");
		printf("\n------ Logical Port %u Configuration ------\n", j);
		printf("\n\t nLogicalPortId         = %u", Assign_get.nLogicalPortId);
		printf("\n\t nFirstCtpPortId        = %u", Assign_get.nFirstCtpPortId);
		printf("\n\t nNumberOfCtpPort       = %u", Assign_get.nNumberOfCtpPort);
		printf("\n\t eMode                  = %u", Assign_get.eMode);
		printf("\n\n\t The Following CTP Ports are associated with Logical port %u :", j);

		first_port = Assign_get.nFirstCtpPortId;
		no_port = Assign_get.nNumberOfCtpPort;

		for (i = first_port, k = 0; i < (first_port + no_port); i++, k++) {
			CTP_get.nLogicalPortId = j;
			CTP_get.nSubIfIdGroup = k;
			CTP_get.eMask = 0xffffffff;

			if (cli_ioctl(fd, GSW_CTP_PORT_CONFIG_GET, &CTP_get) != 0) {
				printf("GSW_CTP_PORT_CONFIG_GET returns ERROR\n");
				return (-1);
			}

			BP_get.nBridgePortId = CTP_get.nBridgePortId;
			BP_get.eMask = 0xffffffff;

			if (cli_ioctl(fd, GSW_BRIDGE_PORT_CONFIG_GET, &BP_get) != 0) {
				printf("GSW_BRIDGE_PORT_CONFIG_GET returns ERROR\n");
				return (-1);
			}

			if (i == 287)
				printf("\n\t\t %u. CTP Ports %u is Default Port--Dummy port (connected to)--> Bridge port %u (Bridge/Fid = %u)\n",
				       (k + 1), i, CTP_get.nBridgePortId, BP_get.nBridgeId);
			else
				printf("\n\t\t %u. CTP Ports %u (connected to)--> Bridge port %u (Bridge/Fid = %u)\n",
				       (k + 1), i, CTP_get.nBridgePortId, BP_get.nBridgeId);
		}
	}

	printf("\n\n");
	printf("To Get individual CTP port's statistics, Please use following command :\n");
	printf("switch_cli GSW_DEBUG_CTP_STATISTICS Index=CTP_PORT_INDEX --> example: 0 to 287\n");

	return 0;
}

int gsw_debug_ctptablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_CTPTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}


int gsw_debug_bridgeporttablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_BRDGPORTTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_tunneltemptablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_DEBUG_TUNNELTEMP_STATUS, &sVar) != 0)
			return (-1);
	}

	return 0;
}

int gsw_debug_bridgetablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_BRDGTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_Exvlantablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);
	cnt += scanParamArg(argc, argv, "blockid", sizeof(sVar.nblockid), &sVar.nblockid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_EXVLANTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_VlanFiltertablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);
	cnt += scanParamArg(argc, argv, "blockid", sizeof(sVar.nblockid), &sVar.nblockid);
	cnt += scanParamArg(argc, argv, "DiscardUnMatchedTagged", sizeof(sVar.nDiscardUnMatchedTagged), &sVar.nDiscardUnMatchedTagged);
	cnt += scanParamArg(argc, argv, "DiscardUntagged", sizeof(sVar.nDiscardUntagged), &sVar.nDiscardUntagged);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_VLANFILTERTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_Metertablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_METERTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_Dscp2Pcptablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_DSCP2PCPTABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_Pmappertablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_PMAPPER_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}


int gsw_irq_register(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Irq_Op_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eBlk", sizeof(sVar.blk), &sVar.blk);
	cnt += scanParamArg(argc, argv, "eEvent", sizeof(sVar.event), &sVar.event);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.portid), &sVar.portid);
	cnt += scanParamArg(argc, argv, "CallBackPtr", sizeof(sVar.call_back), &sVar.call_back);
	cnt += scanParamArg(argc, argv, "DataPtr", sizeof(sVar.param), &sVar.param);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_IRQ_REGISTER, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_irq_unregister(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Irq_Op_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eBlk", sizeof(sVar.blk), &sVar.blk);
	cnt += scanParamArg(argc, argv, "eEvent", sizeof(sVar.event), &sVar.event);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.portid), &sVar.portid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_IRQ_UNREGISTER, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_irq_enable(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Irq_Op_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eBlk", sizeof(sVar.blk), &sVar.blk);
	cnt += scanParamArg(argc, argv, "eEvent", sizeof(sVar.event), &sVar.event);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.portid), &sVar.portid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_IRQ_ENABLE, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_irq_disable(int argc, char *argv[], void *fd, int numPar)
{
	GSW_Irq_Op_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eBlk", sizeof(sVar.blk), &sVar.blk);
	cnt += scanParamArg(argc, argv, "eEvent", sizeof(sVar.event), &sVar.event);
	cnt += scanParamArg(argc, argv, "nPortId", sizeof(sVar.portid), &sVar.portid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_IRQ_DISBALE, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_PrintPceIrqList(int argc, char *argv[], void *fd, int numPar)
{
	if (cli_ioctl(fd, GSW_DEBUG_PRINT_PCEIRQ_LIST, 0) != 0)
		return (-1);

	return 0;
}


int gsw_debug_pmac_eg(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(sVar.nPmacId), &sVar.nPmacId);
	cnt += scanParamArg(argc, argv, "nDestPortId", sizeof(sVar.nDestPort), &sVar.nDestPort);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_PMAC_EG, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_pmac_bp(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(sVar.nPmacId), &sVar.nPmacId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_PMAC_BP, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_pmac_ig(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(sVar.nPmacId), &sVar.nPmacId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_PMAC_IG, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_def_pce_qmap(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_DEF_QMAP, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_debug_def_byp_qmap(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_DEF_BYP_QMAP, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_xgmac_cli(int argc, char *argv[], void *fd, int numPar)
{
	xgmac_main(argc, argv, fd);
	return 0;
}

int gsw_gswss_cli(int argc, char *argv[], void *fd, int numPar)
{
	gswss_main(argc, argv, fd);
	return 0;
}

int gsw_lmac_cli(int argc, char *argv[], void *fd, int numPar)
{
	lmac_main(argc, argv, fd);
	return 0;
}

int gsw_macsec_cli(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_cfg_t sVar;
	unsigned int cnt = 0;

	memset(&sVar, 0x00, sizeof(sVar));
	sVar.argc = argc;

	for (cnt = 0; cnt < argc; cnt++) {
		strcpy((char *)sVar.argv[cnt], argv[cnt]);
	}

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_pmacbr_cli(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_cfg_t sVar;
	unsigned int cnt = 0;

	memset(&sVar, 0x00, sizeof(sVar));
	sVar.argc = argc;

	for (cnt = 0; cnt < argc; cnt++) {
		strcpy((char *)sVar.argv[cnt], argv[cnt]);
	}

	if (cli_ioctl(fd, GSW_PMACBR_CFG, &sVar) != 0)
		return (-1);

	return 0;
}

struct _tbl_dump_ {
	char *tbl_type;
	char *tbl_name;
	u32 entries;
	u32 tbl_addr;
};

struct _tbl_dump_ tbl_dump_gswr[] = {
	{"PCE", "Parser Microcode Table", 256, 0x00},
	{"PCE", "Active VLAN Table", 64, 0x01},
	{"PCE", "VLAN Map Table", 4096, 0x02},
	{"PCE", "PPPoE Table", 16, 0x03},
	{"PCE", "Protocol Table", 32, 0x04},
	{"PCE", "Flags Table", 32, 0x18},
	{"PCE", "App Table", 64, 0x05},
	{"PCE", "IP MSB Table", 64, 0x06},
	{"PCE", "IP LSB Table", 64, 0x07},
	{"PCE", "IP Pktlen Table", 16, 0x08},
	{"PCE", "CTAG Pcp Table", 8, 0x09},
	{"PCE", "Dscp Table", 64, 0x0A},
	{"PCE", "Mac Br Table", 2048, 0x0B},
	{"PCE", "Mult Sw Table", 64, 0x0D},
	{"PCE", "Mult Hw Table", 64, 0x0E},
	{"PCE", "Tflow Table", 512, 0x0F},
	{"PCE", "Qmap Table", 512, 0x11},
	{"PCE", "Met 0 Table", 64, 0x12},
	{"PCE", "Met 1 Table", 64, 0x13},
	{"PCE", "Mac Da Table", 64, 0x16},
	{"PCE", "Mac Sa Table", 64, 0x17},
	{"PCE", "Spcp Table", 16, 0x1A},
	{"PCE", "Stpps Table", 256, 0x1B},
	{"PCE", "Eg Remark Table", 256, 0x1C},
	{"PCE", "Payload Table", 64, 0x1D},
	{"PCE", "Eg Vlan Table", 256, 0x1E},
};

struct _tbl_dump_ tbl_dump_gswl[] = {
	{"PCE", "Parser Microcode Table", 256, 0x00},
	{"PCE", "Active VLAN Table", 64, 0x01},
	{"PCE", "VLAN Map Table", 4096, 0x02},
	{"PCE", "PPPoE Table", 16, 0x03},
	{"PCE", "Protocol Table", 32, 0x04},
	{"PCE", "Flags Table", 32, 0x18},
	{"PCE", "App Table", 64, 0x05},
	{"PCE", "IP MSB Table", 64, 0x06},
	{"PCE", "IP LSB Table", 64, 0x07},
	{"PCE", "IP Pktlen Table", 16, 0x08},
	{"PCE", "CTAG Pcp Table", 8, 0x09},
	{"PCE", "Dscp Table", 64, 0x0A},
	{"PCE", "Mac Br Table", 2048, 0x0B},
	{"PCE", "Mult Sw Table", 64, 0x0D},
	{"PCE", "Mult Hw Table", 64, 0x0E},
	{"PCE", "Tflow Table", 64, 0x0F},
	{"PCE", "Qmap Table", 224, 0x11},
	{"PCE", "Met 0 Table", 64, 0x12},
	{"PCE", "Met 1 Table", 64, 0x13},
	{"PCE", "Mac Da Table", 64, 0x16},
	{"PCE", "Mac Sa Table", 64, 0x17},
	{"PCE", "Spcp Table", 16, 0x1A},
	{"PCE", "Stpps Table", 112, 0x1B},
	{"PCE", "Eg Remark Table", 112, 0x1C},
	{"PCE", "Eg Vlan Table", 256, 0x1E},
};

struct _tbl_dump_ tbl_dump_route_gswr[] = {
	{"ROUTE_PCE", "Routing Session Table", 4096, 0x00},
	{"ROUTE_PCE", "Routing IP Table", 512, 0x01},
	{"ROUTE_PCE", "Routing Mac Table", 512, 0x02},
	{"ROUTE_PCE", "Routing PPPoE Table", 16, 0x03},
	{"ROUTE_PCE", "Routing Tunnel Table", 16, 0x04},
	{"ROUTE_PCE", "Routing Rtp Table", 64, 0x05},
	{"ROUTE_PCE", "Routing Mtu Table", 8, 0x06},
};

struct _tbl_dump_ tbl_dump_bm[] = {
	{"BM", "BM WFQ", 32, 0x08},
	{"BM", "BM PQM CTXT Table", 32, 0x09},
	{"BM", "BM QMAP Table", 32, 0x0E},
};

struct _tbl_dump_ tbl_dump_pmac_gswr[] = {
	{"PMAC", "PMAC BP Table", 16, 0x00},
	{"PMAC", "PMAC IG Table", 16, 0x01},
	{"PMAC", "PMAC EG Table", 1024, 0x02},
};

struct _tbl_dump_ tbl_dump_pmac_gswl[] = {
	{"PMAC", "PMAC BP Table", 16, 0x00},
	{"PMAC", "PMAC IG Table", 16, 0x01},
	{"PMAC", "PMAC EG Table", 512, 0x02},
};


struct _tbl_dump_ tbl_dump_gsw31[] = {
	{"PCE", "Parser Microcode Table", 256, 0x00},
	{"PCE", "VLANMAP Table", 1024, 0x02},
	{"PCE", "PPPoE Table", 16, 0x03},
	{"PCE", "Protocol Table", 32, 0x04},
	{"PCE", "Flags Table", 64, 0x18},
	{"PCE", "App Table", 64, 0x05},
	{"PCE", "IP MSB Table", 64, 0x06},
	{"PCE", "IP LSB Table", 64, 0x07},
	{"PCE", "IP Pktlen Table", 16, 0x08},
	{"PCE", "CTAG Pcp Table", 8, 0x09},
	{"PCE", "Dscp Table", 64, 0x0A},
	{"PCE", "Mac Br Table", 4096, 0x0B},
	{"PCE", "Mult Sw Table", 512, 0x0D},
	{"PCE", "Mult Hw Table", 64, 0x0E},
	{"PCE", "Tflow Table", 512, 0x0F},
	{"PCE", "Qmap Table", 512, 0x11},
	{"PCE", "IGCTP Table", 288, 0x12},
	{"PCE", "EGCTP Table", 288, 0x13},
	{"PCE", "IGBRG Table", 128, 0x14},
	{"PCE", "EGBRG Table", 128, 0x15},
	{"PCE", "Mac Da Table", 64, 0x16},
	{"PCE", "Mac Sa Table", 64, 0x17},
	{"PCE", "BRGCFG Table", 64, 0x19},
	{"PCE", "Spcp Table", 16, 0x1A},
	{"PCE", "COLMARK Table", 128, 0x1B},
	{"PCE", "REMARK Table", 80, 0x1C},
	{"PCE", "Payload Table", 64, 0x1D},
	{"PCE", "Extended VLAN Table", 1024, 0x1E},
	{"PCE", "P-Mapping Table", 292, 0x1F},
	{"PCE", "Dscp2Pcp Table", 64, 0xC},
};

struct _tbl_dump_ tbl_dump_pmac_gsw31[] = {
	{"PMAC", "PMAC BP Table", 17, 0x00},
	{"PMAC", "PMAC IG Table", 17, 0x01},
	{"PMAC", "PMAC EG Table", 1024, 0x02},
};


typedef struct {
	u16 num_key;
	u16 num_mask;
	u16 num_val;
} gsw_pce_tbl_t;


static const gsw_pce_tbl_t gsw_pce_tbl_30[] = {
	{0, 0, 4}, {1, 1, 0}, {0, 0, 3}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {4, 4, 0}, {4, 4, 0},
	{1, 1, 0}, {0, 0, 1}, {0, 0, 1}, {4, 0, 2}, {0, 0, 0}, {3, 0, 2}, {2, 0, 5}, {16, 0, 10},
	{0, 0, 0}, {0, 0, 1}, {1, 1, 1}, {1, 1, 1}, {0, 0, 0}, {0, 0, 0}, {3, 1, 0}, {3, 1, 0},
	{1, 1, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 2}, {1, 1, 0}, {0, 0, 2}
};

static const gsw_pce_tbl_t gsw_pce_tbl_31[] = {
	{0, 0, 4}, {0, 0, 0}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 0}, {4, 4, 0}, {4, 4, 0},
	{1, 1, 0}, {0, 0, 1}, {0, 0, 1}, {4, 0, 10}, {0, 0, 2}, {19, 0, 10}, {2, 0, 5}, {22, 0, 18},
	{0, 0, 0}, {0, 0, 1}, {0, 0, 9}, {0, 0, 7}, {0, 0, 18}, {0, 0, 14}, {3, 1, 0}, {3, 1, 0},
	{1, 1, 0}, {0, 0, 10}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {1, 1, 0}, {4, 0, 6}, {0, 0, 1}
};

static const gsw_pce_tbl_t gsw_pce_rt_tbl_30[] = {
	{6, 0, 13}, {0, 0, 9}, {0, 0, 3}, {0, 0, 1}, {0, 0, 4}, {0, 0, 2}, {0, 0, 1}
};

static const gsw_pce_tbl_t gsw_bm_tbl[] = {
	{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	{0, 0, 1}, {0, 0, 2}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}
};

static const gsw_pce_tbl_t gsw_pmac_tbl[] = {
	{0, 0, 3}, {0, 0, 5}, {0, 0, 3}
};

int gsw_dump_mem(int argc, char *argv[], void *fd, int numPar)
{
	GSW_table_t sVar;
	unsigned int i = 0, j = 0, k = 0, m = 0;
	GSW_register_t reg, param;
	u32 devIdx = 0;
	int num_of_elem;

	memset(&sVar, 0x00, sizeof(sVar));
	memset(&reg, 0, sizeof(GSW_register_t));

	scanParamArg(argc, argv, "dev", sizeof(devIdx), &devIdx);

	memset(&param, 0, sizeof(GSW_register_t));
	param.nRegAddr = 0x13;

	if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	/*store in global variable*/
	if (param.nData == GSWIP_3_0) {

		for (i = 0; i < 0xEFF; i++) {

			reg.nData = 0;
			reg.nRegAddr = i;

			if (cli_ioctl(fd, GSW_REGISTER_GET, &reg) != 0) {
				printf("ioctl returned with ERROR!\n");
				return (-1);
			}

			if (devIdx == 0) {
				printf("%08x:  %08x\n", 0xbc000000 + i, reg.nData);
			} else {
				printf("%08x:  %08x\n", 0xba000000 + i, reg.nData);
			}
		}

		if (devIdx == 1) {
			printf("===========================================\n");
			printf("GSWIP-R PCE table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_gswr) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_gswr[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_gswr[i].entries; j++) {

					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = j;
					sVar.tbl_addr = tbl_dump_gswr[i].tbl_addr;
					sVar.tbl_id = 1;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_key; k++)
						printf("\tKey  %d:  %04x\n", k, sVar.ptdata.key[k]);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_mask; k++)
						printf("\tMask %d:  %04x\n", k, sVar.ptdata.mask[k]);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.ptdata.val[k]);

					printf("\tValid:   %x\n", sVar.ptdata.valid);
					printf("\tType:    %x\n", sVar.ptdata.type);
					printf("\n");

					for (m = 0; m < 500000; m++);

				}
			}

			printf("===========================================\n");
			printf("GSWIP-R Routing table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_route_gswr) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_route_gswr[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_route_gswr[i].entries; j++) {

					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = j;
					sVar.tbl_addr = tbl_dump_route_gswr[i].tbl_addr;
					sVar.tbl_id = 2;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);


					for (k = 0; k < gsw_pce_rt_tbl_30[sVar.tbl_addr].num_key; k++)
						printf("\tKey  %d:  %04x\n", k, sVar.ptdata.key[k]);

					for (k = 0; k < gsw_pce_rt_tbl_30[sVar.tbl_addr].num_mask; k++)
						printf("\tMask %d:  %04x\n", k, sVar.ptdata.mask[k]);

					for (k = 0; k < gsw_pce_rt_tbl_30[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.ptdata.val[k]);

					printf("\tValid:   %x\n", sVar.ptdata.valid);
					printf("\tType:    %x\n", sVar.ptdata.type);
					printf("\n");

					for (m = 0; m < 500000; m++);
				}
			}

			printf("===========================================\n");
			printf("GSWIP-R BM table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_bm) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_bm[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_bm[i].entries; j++) {

					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = j;
					sVar.tbl_addr = tbl_dump_bm[i].tbl_addr;
					sVar.tbl_id = 3;
					sVar.bm_numValues = gsw_bm_tbl[sVar.tbl_addr].num_val;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_bm_tbl[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.bmtable.value[k]);

					printf("\n");

					for (m = 0; m < 500000; m++);
				}
			}

			printf("===========================================\n");
			printf("GSWIP-R PMAC table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_pmac_gswr) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_pmac_gswr[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_pmac_gswr[i].entries; j++) {

					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = 0;
					sVar.tbl_addr = tbl_dump_pmac_gswr[i].tbl_addr;
					sVar.tbl_id = 4;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_pmac_tbl[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.pmactable.val[k]);

					printf("\n");

					for (m = 0; m < 500000; m++);
				}
			}

		} else {

			printf("===========================================\n");
			printf("GSWIP-L PCE table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_gswl) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {
				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_gswl[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_gswl[i].entries; j++) {
					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = j;
					sVar.tbl_addr = tbl_dump_gswl[i].tbl_addr;
					sVar.tbl_id = 1;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_key; k++)
						printf("\tKey  %d:  %04x\n", k, sVar.ptdata.key[k]);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_mask; k++)
						printf("\tMask %d:  %04x\n", k, sVar.ptdata.mask[k]);

					for (k = 0; k < gsw_pce_tbl_30[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.ptdata.val[k]);

					printf("\tValid:   %x\n", sVar.ptdata.valid);
					printf("\tType:    %x\n", sVar.ptdata.type);
					printf("\n");

					for (m = 0; m < 500000; m++);
				}
			}

			printf("===========================================\n");
			printf("GSWIP-L BM table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_bm) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_bm[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_bm[i].entries; j++) {

					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = j;
					sVar.tbl_addr = tbl_dump_bm[i].tbl_addr;
					sVar.tbl_id = 3;
					sVar.bm_numValues = gsw_bm_tbl[sVar.tbl_addr].num_val;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_bm_tbl[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.bmtable.value[k]);

					printf("\n");

					for (m = 0; m < 500000; m++);
				}
			}

			printf("===========================================\n");
			printf("GSWIP-L PMAC table\n");
			printf("===========================================\n");
			num_of_elem =
				(sizeof(tbl_dump_pmac_gswl) / sizeof(struct _tbl_dump_));

			for (i = 0; i < num_of_elem; i++) {

				printf("===========================================\n");
				printf("Table Name: %s\n", tbl_dump_pmac_gswl[i].tbl_name);
				printf("===========================================\n");

				for (j = 0; j < tbl_dump_pmac_gswl[i].entries; j++) {
					memset(&sVar, 0, sizeof(sVar));

					printf("Table Idx: %02d\n", j);
					sVar.tbl_entry = 0;
					sVar.tbl_addr = tbl_dump_pmac_gswl[i].tbl_addr;
					sVar.tbl_id = 4;

					while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

					for (k = 0; k < gsw_pmac_tbl[sVar.tbl_addr].num_val; k++)
						printf("\tVal  %d:  %04x\n", k, sVar.pmactable.val[k]);

					printf("\n");

					for (m = 0; m < 1000000; m++);
				}
			}

		}

	} else if ((param.nData == GSWIP_3_1) || (param.nData == GSWIP_3_2)) {

		for (i = 0; i < 0xEFF; i++) {

			reg.nData = 0;
			reg.nRegAddr = i;

			if (cli_ioctl(fd, GSW_REGISTER_GET, &reg) != 0) {
				printf("ioctl returned with ERROR!\n");
				return (-1);
			}

			if (param.nData == GSWIP_3_1)
				printf("%08x:  %08x\n", 0xb8455000 + i, reg.nData);
			else
				printf("%08x:  %08x\n", 0xb8445000 + i, reg.nData);
		}

		printf("===========================================\n");

		if (param.nData == GSWIP_3_1)
			printf("GSWIP-31 PCE table\n");
		else
			printf("GSWIP-32 PCE table\n");

		printf("===========================================\n");
		num_of_elem =
			(sizeof(tbl_dump_gsw31) / sizeof(struct _tbl_dump_));

		for (i = 0; i < num_of_elem; i++) {
			printf("===========================================\n");
			printf("Table Name: %s\n", tbl_dump_gsw31[i].tbl_name);
			printf("===========================================\n");

			for (j = 0; j < tbl_dump_gsw31[i].entries; j++) {
				memset(&sVar, 0, sizeof(sVar));

				printf("Table Idx: %02d\n", j);
				sVar.tbl_entry = j;
				sVar.tbl_addr = tbl_dump_gsw31[i].tbl_addr;
				sVar.tbl_id = 1;

				while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

				for (k = 0; k < gsw_pce_tbl_31[sVar.tbl_addr].num_key; k++)
					printf("\tKey  %d:  %04x\n", k, sVar.ptdata.key[k]);

				for (k = 0; k < gsw_pce_tbl_31[sVar.tbl_addr].num_mask; k++)
					printf("\tMask %d:  %04x\n", k, sVar.ptdata.mask[k]);

				for (k = 0; k < gsw_pce_tbl_31[sVar.tbl_addr].num_val; k++)
					printf("\tVal  %d:  %04x\n", k, sVar.ptdata.val[k]);

				printf("\tValid:   %x\n", sVar.ptdata.valid);
				printf("\tType:    %x\n", sVar.ptdata.type);
				printf("\n");

				for (m = 0; m < 500000; m++);
			}
		}

		printf("===========================================\n");

		if (param.nData == GSWIP_3_1)
			printf("GSWIP-31 BM table\n");
		else
			printf("GSWIP-32 BM table\n");

		printf("===========================================\n");
		num_of_elem =
			(sizeof(tbl_dump_bm) / sizeof(struct _tbl_dump_));

		for (i = 0; i < num_of_elem; i++) {

			printf("===========================================\n");
			printf("Table Name: %s\n", tbl_dump_bm[i].tbl_name);
			printf("===========================================\n");

			for (j = 0; j < tbl_dump_bm[i].entries; j++) {

				memset(&sVar, 0, sizeof(sVar));

				printf("Table Idx: %02d\n", j);
				sVar.tbl_entry = j;
				sVar.tbl_addr = tbl_dump_bm[i].tbl_addr;
				sVar.tbl_id = 3;
				sVar.bm_numValues = gsw_bm_tbl[sVar.tbl_addr].num_val;

				while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

				for (k = 0; k < gsw_bm_tbl[sVar.tbl_addr].num_val; k++)
					printf("\tVal  %d:  %04x\n", k, sVar.bmtable.value[k]);

				printf("\n");

				for (m = 0; m < 500000; m++);
			}
		}

		printf("===========================================\n");

		if (param.nData == GSWIP_3_1)
			printf("GSWIP-31 PMAC table\n");
		else
			printf("GSWIP-32 PMAC table\n");

		printf("===========================================\n");
		num_of_elem =
			(sizeof(tbl_dump_pmac_gsw31) / sizeof(struct _tbl_dump_));

		for (i = 0; i < num_of_elem; i++) {

			printf("===========================================\n");
			printf("Table Name: %s\n", tbl_dump_pmac_gsw31[i].tbl_name);
			printf("===========================================\n");

			for (j = 0; j < tbl_dump_pmac_gsw31[i].entries; j++) {
				memset(&sVar, 0, sizeof(sVar));

				printf("Table Idx: %02d\n", j);
				sVar.tbl_entry = 0;
				sVar.tbl_addr = tbl_dump_pmac_gsw31[i].tbl_addr;
				sVar.tbl_id = 4;

				while (cli_ioctl(fd, GSW_DUMP_MEM, &sVar) != 0);

				for (k = 0; k < gsw_pmac_tbl[sVar.tbl_addr].num_val; k++)
					printf("\tVal  %d:  %04x\n", k, sVar.pmactable.val[k]);

				printf("\n");

				for (m = 0; m < 1000000; m++);
			}
		}
	}

	return 0;
}

int gsw_dscp2pcp_map_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_DSCP2PCP_map_t param;
	unsigned int cnt = 0;
	unsigned char i = 0;

	memset(&param, 0, sizeof(GSW_DSCP2PCP_map_t));
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.nIndex), &param.nIndex);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DSCP2PCP_MAP_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("nIndex", param.nIndex, 0);

	for (i = 0; i < 64; i++)
		printf("\t nMap[%d]            = %d\n", i, param.nMap[i]);

	return 0;
}

int gsw_defaul_mac_filter_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACFILTER_default_t param;
	unsigned int cnt = 0;
	unsigned char i = 0;

	memset(&param, 0, sizeof(GSW_MACFILTER_default_t));
	cnt += scanParamArg(argc, argv, "eType", 32, &param.eType);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEFAUL_MAC_FILTER_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("eType", param.eType, 0);

	for (i = 0; i <= 7; i++)
		printf("\t nPortmap[%d]            = 0x%x\n", i, param.nPortmap[i]);

	return 0;
}

int gsw_defaul_mac_filter_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACFILTER_default_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eType", sizeof(sVar.eType), &sVar.eType);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEFAUL_MAC_FILTER_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_pbb_tunneltemplate_config_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PBB_Tunnel_Template_Config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nTunnelTemplateId", sizeof(sVar.nTunnelTemplateId), &sVar.nTunnelTemplateId);

	cnt += scanParamArg(argc, argv, "bIheaderDstMACEnable", sizeof(sVar.bIheaderDstMACEnable), &sVar.bIheaderDstMACEnable);
	cnt += scanMAC_Arg(argc, argv, "nIheaderDstMAC", sVar.nIheaderDstMAC);
	cnt += scanParamArg(argc, argv, "bIheaderSrcMACEnable", sizeof(sVar.bIheaderSrcMACEnable), &sVar.bIheaderSrcMACEnable);
	cnt += scanMAC_Arg(argc, argv, "nIheaderSrcMAC", sVar.nIheaderSrcMAC);

	cnt += scanParamArg(argc, argv, "bItagEnable", sizeof(sVar.bItagEnable), &sVar.bItagEnable);
	cnt += scanParamArg(argc, argv, "bItagTpidEnable", sizeof(sVar.sItag.bTpidEnable), &sVar.sItag.bTpidEnable);
	cnt += scanParamArg(argc, argv, "nItagTpid", sizeof(sVar.sItag.nTpid), &sVar.sItag.nTpid);
	cnt += scanParamArg(argc, argv, "bItagPcpEnable", sizeof(sVar.sItag.bPcpEnable), &sVar.sItag.bPcpEnable);
	cnt += scanParamArg(argc, argv, "nItagPcp", sizeof(sVar.sItag.nPcp), &sVar.sItag.nPcp);
	cnt += scanParamArg(argc, argv, "bItagDeiEnable", sizeof(sVar.sItag.bDeiEnable), &sVar.sItag.bDeiEnable);
	cnt += scanParamArg(argc, argv, "nItagDei", sizeof(sVar.sItag.nDei), &sVar.sItag.nDei);
	cnt += scanParamArg(argc, argv, "bItagUacEnable", sizeof(sVar.sItag.bUacEnable), &sVar.sItag.bUacEnable);
	cnt += scanParamArg(argc, argv, "nItagUac", sizeof(sVar.sItag.nUac), &sVar.sItag.nUac);
	cnt += scanParamArg(argc, argv, "bItagResEnable", sizeof(sVar.sItag.bResEnable), &sVar.sItag.bResEnable);
	cnt += scanParamArg(argc, argv, "nItagRes", sizeof(sVar.sItag.nRes), &sVar.sItag.nRes);
	cnt += scanParamArg(argc, argv, "bItagSidEnable", sizeof(sVar.sItag.bSidEnable), &sVar.sItag.bSidEnable);
	cnt += scanParamArg(argc, argv, "nItagSid", sizeof(sVar.sItag.nSid), &sVar.sItag.nSid);

	cnt += scanParamArg(argc, argv, "bBtagEnable", sizeof(sVar.bBtagEnable), &sVar.bBtagEnable);
	cnt += scanParamArg(argc, argv, "bBtagTpidEnable", sizeof(sVar.sBtag.bTpidEnable), &sVar.sBtag.bTpidEnable);
	cnt += scanParamArg(argc, argv, "nBtagTpid", sizeof(sVar.sBtag.nTpid), &sVar.sBtag.nTpid);
	cnt += scanParamArg(argc, argv, "bBtagPcpEnable", sizeof(sVar.sBtag.bPcpEnable), &sVar.sBtag.bPcpEnable);
	cnt += scanParamArg(argc, argv, "nBtagPcp", sizeof(sVar.sBtag.nPcp), &sVar.sBtag.nPcp);
	cnt += scanParamArg(argc, argv, "bBtagDeiEnable", sizeof(sVar.sBtag.bDeiEnable), &sVar.sBtag.bDeiEnable);
	cnt += scanParamArg(argc, argv, "nBtagDei", sizeof(sVar.sBtag.nDei), &sVar.sBtag.nDei);
	cnt += scanParamArg(argc, argv, "bBtagVidEnable", sizeof(sVar.sBtag.bVidEnable), &sVar.sBtag.bVidEnable);
	cnt += scanParamArg(argc, argv, "nBtagVid", sizeof(sVar.sBtag.nVid), &sVar.sBtag.nVid);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_PBB_TUNNEL_TEMPLATE_SET, &sVar) != 0)
			return (-1);
	}

	return 0;
}

int gsw_pbb_tunneltemplate_config_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PBB_Tunnel_Template_Config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));


	cnt += scanParamArg(argc, argv, "nTunnelTemplateId", sizeof(sVar.nTunnelTemplateId), &sVar.nTunnelTemplateId);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_PBB_TUNNEL_TEMPLATE_GET, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	printf("\n\t nTunnelTemplateId						= %u", sVar.nTunnelTemplateId);
	printf("\n\t nIheaderDstMAC							=");
	printMAC_Address(sVar.nIheaderDstMAC);
	printf("\n\t nIheaderSrcMAC							=");
	printMAC_Address(sVar.nIheaderSrcMAC);
	printf("\n\t nItagTpid								=0x%x", sVar.sItag.nTpid);
	printf("\n\t nItagPcp								= %u", sVar.sItag.nPcp);
	printf("\n\t nItagDei								= %u", sVar.sItag.nDei);
	printf("\n\t nItagUac								= %u", sVar.sItag.nUac);
	printf("\n\t nItagRes								= %u", sVar.sItag.nRes);
	printf("\n\t nItagSid								= %u", sVar.sItag.nSid);
	printf("\n\t nBtagTpid								=0x%x", sVar.sBtag.nTpid);
	printf("\n\t nBtagPcp								= %u", sVar.sBtag.nPcp);
	printf("\n\t nBtagDei								= %u", sVar.sBtag.nDei);
	printf("\n\t nBtagVid								= %u", sVar.sBtag.nVid);

	return 0;
}

int gsw_pbb_tunneltemplate_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PBB_Tunnel_Template_Config_t sVar;
	memset(&sVar, 0x00, sizeof(sVar));

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_PBB_TUNNEL_TEMPLATE_ALLOC, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	printf("\n\t Allocated nTunnelTemplateId						= %u", sVar.nTunnelTemplateId);
	return 0;
}

int gsw_pbb_tunneltemplate_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PBB_Tunnel_Template_Config_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nTunnelTemplateId", sizeof(sVar.nTunnelTemplateId), &sVar.nTunnelTemplateId);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_PBB_TUNNEL_TEMPLATE_FREE, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	printf("\n\t Freed nTunnelTemplateId						= %u", sVar.nTunnelTemplateId);
	return 0;
}


int gsw_lpidTogpid_assignmentset(int argc, char *argv[], void *fd, int numPar)
{
	GSW_LPID_to_GPID_Assignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nFirstGlobalPortId", sizeof(sVar.nFirstGlobalPortId), &sVar.nFirstGlobalPortId);
	cnt += scanParamArg(argc, argv, "nNumberOfGlobalPort", sizeof(sVar.nNumberOfGlobalPort), &sVar.nNumberOfGlobalPort);

	if (findStringParam(argc, argv, "nValidBits")) {
		cnt += scanParamArg(argc, argv, "nValidBits", sizeof(sVar.nValidBits), &sVar.nValidBits);
	} else {
		sVar.nValidBits = 0x7;
	}


	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_LPID_TO_GPID_ASSIGNMENT_SET, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	return 0;
}

int gsw_lpidTogpid_assignmentget(int argc, char *argv[], void *fd, int numPar)
{
	GSW_LPID_to_GPID_Assignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_LPID_TO_GPID_ASSIGNMENT_GET, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	printf("\n\t nLogicalPortId								= %u", sVar.nLogicalPortId);
	printf("\n\t nFirstGlobalPortId							= %u", sVar.nFirstGlobalPortId);
	printf("\n\t nNumberOfGlobalPort						= %u", sVar.nNumberOfGlobalPort);
	printf("\n\t nValidBits									= %u", sVar.nValidBits);

	return 0;
}


int gsw_gpidTolpid_assignmentset(int argc, char *argv[], void *fd, int numPar)
{
	GSW_GPID_to_LPID_Assignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nGlobalPortId", sizeof(sVar.nGlobalPortId), &sVar.nGlobalPortId);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(sVar.nLogicalPortId), &sVar.nLogicalPortId);
	cnt += scanParamArg(argc, argv, "nSubIf_GroupField", sizeof(sVar.nSubIf_GroupField), &sVar.nSubIf_GroupField);
	cnt += scanParamArg(argc, argv, "nSubIf_GroupField_OverRide", sizeof(sVar.nSubIf_GroupField_OverRide), &sVar.nSubIf_GroupField_OverRide);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_GPID_TO_LPID_ASSIGNMENT_SET, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	return 0;
}

int gsw_gpidTolpid_assignmentget(int argc, char *argv[], void *fd, int numPar)
{
	GSW_GPID_to_LPID_Assignment_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "nGlobalPortId", sizeof(sVar.nGlobalPortId), &sVar.nGlobalPortId);

	if (cnt != numPar)
		return (-2);

	if (gswip_version == GSWIP_3_2) {
		if (cli_ioctl(fd, GSW_GPID_TO_LPID_ASSIGNMENT_GET, &sVar) != 0)
			return (-1);
	} else {
		return 0;
	}

	printf("\n\t nGlobalPortId							= %u", sVar.nGlobalPortId);
	printf("\n\t nLogicalPortId							= %u", sVar.nLogicalPortId);
	printf("\n\t nSubIf_GroupField						= %u", sVar.nSubIf_GroupField);

	if (sVar.nSubIf_GroupField_OverRide)
		printf("\n\t nSubIf_GroupField_OverRide				= TRUE");
	else
		printf("\n\t nSubIf_GroupField_OverRide				= FALSE");

	return 0;
}

int gsw_qos_color_marking_table_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_colorMarkingEntry_t sVar;
	unsigned int cnt = 0;
	unsigned char i = 0, temp_i = 0, pri = 0, dei = 0, TableIndex = 0;

	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eMode", sizeof(sVar.eMode), &sVar.eMode);

	switch (sVar.eMode) {
	case GSW_MARKING_ALL_GREEN:
		break;

	case GSW_MARKING_INTERNAL_MARKING:
		break;

	case GSW_MARKING_DEI:
		break;

	case GSW_MARKING_PCP_8P0D:
	case GSW_MARKING_PCP_7P1D:
	case GSW_MARKING_PCP_6P2D:
	case GSW_MARKING_PCP_5P3D:
		TableIndex = ((sVar.eMode - GSW_MARKING_PCP_8P0D) << 4);

		for (i = 0; i <= 15; i++, TableIndex++) {
			pri = (i >> 1) & 7;
			dei = i & 1;
			//Priority
			sVar.nPriority[TableIndex] = temp_i = pri;

			/*DEI 0*/
			if (dei == 0) {
				if (4 == pri) {
					if (sVar.eMode != GSW_MARKING_PCP_8P0D)
						sVar.nPriority[TableIndex] = temp_i = (pri + 1);
				}

				if (2 == pri) {
					if ((sVar.eMode == GSW_MARKING_PCP_6P2D) ||
					    (sVar.eMode == GSW_MARKING_PCP_5P3D))
						sVar.nPriority[TableIndex] = temp_i = (pri + 1);
				}

				if (0 == pri) {
					if (sVar.eMode == GSW_MARKING_PCP_5P3D)
						sVar.nPriority[TableIndex] = temp_i = (pri + 1);
				}
			}

			/*DEI 1*/
			if (dei == 1) {
				if (5 == pri) {
					if (sVar.eMode != GSW_MARKING_PCP_8P0D)
						sVar.nPriority[TableIndex] = temp_i = (pri - 1);
				}

				if (3 == pri) {
					if ((sVar.eMode == GSW_MARKING_PCP_6P2D) ||
					    (sVar.eMode == GSW_MARKING_PCP_5P3D))
						sVar.nPriority[TableIndex] = temp_i = (pri - 1);
				}

				if (1 == pri) {
					if (sVar.eMode == GSW_MARKING_PCP_5P3D)
						sVar.nPriority[TableIndex] = temp_i = (pri - 1);
				}
			}

			//Color
			if ((0 == temp_i) || (3 == temp_i) || (4 == temp_i))
				sVar.nColor[TableIndex] = 3; //Red
			else if (temp_i <= 2)
				sVar.nColor[TableIndex] = 2; //Yellow
			else
				sVar.nColor[TableIndex] = 1; //Green
		}

		break;

	case GSW_MARKING_DSCP_AF:
		for (i = 0; i <= 63; i++, TableIndex++) {
			temp_i = i & 7;
			sVar.nPriority[TableIndex] = temp_i;

			//Color
			if ((0 == temp_i) || (3 == temp_i) || (4 == temp_i))
				sVar.nColor[TableIndex] = 3; //Red
			else if (temp_i <= 2)
				sVar.nColor[TableIndex] = 2; //Yellow
			else
				sVar.nColor[TableIndex] = 1; //Green
		}

		break;
	}

	cnt += scanParamArg(argc, argv, "nPriority", sizeof(sVar.nPriority), &sVar.nPriority);
	cnt += scanParamArg(argc, argv, "nColor", sizeof(sVar.nColor), &sVar.nColor);


	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_COLOR_MARKING_TABLE_SET, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_qos_color_marking_table_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_colorMarkingEntry_t param;
	unsigned char i = 0;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_QoS_colorMarkingEntry_t));
	cnt += scanParamArg(argc, argv, "eMode", sizeof(param.eMode), &param.eMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_COLOR_MARKING_TABLE_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("eMode", param.eMode, 0);
	printf("\nPCPnDEI or DSCP to Internal frame prioirty&color map:\n");

	for (i = 0; i <= 63; i++)
		printf("\t Priority[%2d]  &  Color[%2d]        = %2d n %2d\n", i, i, param.nPriority[i], param.nColor[i]);

	return 0;
}

int gsw_qos_color_remarking_table_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_colorRemarkingEntry_t param;
	unsigned char i = 0;
	unsigned int cnt = 0;

	memset(&param, 0, sizeof(GSW_QoS_colorRemarkingEntry_t));
	cnt += scanParamArg(argc, argv, "eMode", sizeof(param.eMode), &param.eMode);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_COLOR_REMARKING_TABLE_GET, &param) != 0)
		return (-1);

	printf("Returned values:\n----------------\n");
	printHex32Value("eMode", param.eMode, 0);

	printf("\nThe color-bit & internal frame prioirty to PCPnDEI or DSCP map:\n");

	for (i = 0; i < 16; i++) {
		if (param.eMode != 7) {
			if (i < 8) {
				if (i == 0)
					printf("For color bit = 0\n");

				printf("PCP[%2d] & DEI[%2d]  = %2d n %2d \n", i, i, ((param.nVal[i] >> 1) & 7), (param.nVal[i] & 1));
			} else {
				if (i == 8)
					printf("For color bit = 1\n");

				printf("PCP[%2d] & DEI[%2d]  = %2d n %2d \n", i, i, ((param.nVal[i] >> 1) & 7), (param.nVal[i] & 1));
			}
		} else {
			printf("DSCP[%2d]  = %d \n", i, param.nVal[i]);
		}
	}

	return 0;
}


int gsw_qos_color_remarking_table_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_colorRemarkingEntry_t sVar;
	unsigned int cnt = 0;
	unsigned char i = 0, TableIndex = 0;

	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "eMode", sizeof(sVar.eMode), &sVar.eMode);

	switch (sVar.eMode) {
	case GSW_REMARKING_NONE:
		break;

	case GSW_REMARKING_DEI:
		break;

	case GSW_REMARKING_PCP_8P0D:
	case GSW_REMARKING_PCP_7P1D:
	case GSW_REMARKING_PCP_6P2D:
	case GSW_REMARKING_PCP_5P3D:

		/*Color-bit 0*/
		for (i = 0; i <= 7; i++, TableIndex++) {
			//Priority
			sVar.nVal[TableIndex] = (i << 1);

			if (4 == i) {
				if (sVar.eMode != GSW_REMARKING_PCP_8P0D)
					sVar.nVal[TableIndex] = ((i + 1) << 1);
			}

			if (2 == i) {
				if ((sVar.eMode == GSW_REMARKING_PCP_6P2D) ||
				    (sVar.eMode == GSW_REMARKING_PCP_5P3D))
					sVar.nVal[TableIndex] = ((i + 1) << 1);
			}

			if (0 == i) {
				if (sVar.eMode == GSW_REMARKING_PCP_5P3D)
					sVar.nVal[TableIndex] = ((i + 1) << 1);
			}
		}

		/*Color-bit 0*/
		for (i = 8; i <= 15; i++, TableIndex++) {
			//Priority
			sVar.nVal[TableIndex] = (((i - 8) << 1) | 1);

			if (13 == i) {
				if (sVar.eMode != GSW_REMARKING_PCP_8P0D)
					sVar.nVal[TableIndex] = ((((i - 1) - 8) << 1) | 1);
			}

			if (11 == i) {
				if ((sVar.eMode == GSW_REMARKING_PCP_6P2D) ||
				    (sVar.eMode == GSW_REMARKING_PCP_5P3D))
					sVar.nVal[TableIndex] = ((((i - 1) - 8) << 1) | 1);
			}

			if (9 == i) {
				if (sVar.eMode == GSW_REMARKING_PCP_5P3D)
					sVar.nVal[TableIndex] = ((((i - 1) - 8) << 1) | 1);
			}
		}

		break;

	case GSW_REMARKING_DSCP_AF:
		TableIndex = 0;

		for (i = 0; i <= 15; i++, TableIndex++) {
			if (i > 7)
				sVar.nVal[TableIndex] = (0x1f - (i - 8));
			else
				sVar.nVal[TableIndex] = (0x3f - i);
		}

		break;
	}

	cnt += scanParamArg(argc, argv, "nVal", sizeof(sVar.nVal), &sVar.nVal);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_QOS_COLOR_REMARKING_TABLE_SET, &sVar) != 0)
		return (-1);

	return 0;
}


int gsw_defaul_pce_qmap_get(int argc, char *argv[], void *fd, int numPar)
{
	int i = 0;
	int cnt = 0;
	GSW_QoS_queuePort_t q_map;
	memset(&q_map, 0, sizeof(GSW_QoS_queuePort_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(q_map.nPortId), &q_map.nPortId);

	printf("\nGSWIP Default PCE-Q-MAP\n");
	printf("%15s %15s %15s %15s %15s %15s\n",
	       "EgLpid", "Ext", "Traf_Cls", "Q_Map_Mode", "Qid", "Redir_Lpid");

	for (i = 0; i <= 15; i++) {
		q_map.nTrafficClassId = i;

		if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &q_map) != 0)
			return (-1);

		printf("%15d %15d %15d %15d %15d %15d ",
		       q_map.nPortId, q_map.bExtrationEnable,
		       q_map.nTrafficClassId, q_map.eQMapMode,
		       q_map.nQueueId, q_map.nRedirectPortId);
		printf("\n");

	}

	return 0;
}



int gsw_defaul_bypass_qmap_get(int argc, char *argv[], void *fd, int numPar)
{
	int i = 0;
	int cnt = 0;
	GSW_QoS_queuePort_t q_map;
	memset(&q_map, 0, sizeof(GSW_QoS_queuePort_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(q_map.nPortId), &q_map.nPortId);

	printf("\nGSWIP Default PCE Bypass Q-MAP\n");
	printf("%15s %15s %15s %15s %15s %15s\n",
	       "EgMpid", "Ext", "Traf_Cls", "Q_Map_Mode", "Qid", "Redir_Lpid");

	for (i = 0; i <= 15; i++) {
		q_map.bRedirectionBypass = 1;

		if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &q_map) != 0)
			return (-1);

		printf("%15d %15d %15d %15d %15d %15d ",
		       q_map.nPortId, q_map.bExtrationEnable,
		       q_map.nTrafficClassId, q_map.eQMapMode,
		       q_map.nQueueId, q_map.nRedirectPortId);
		printf("\n");

	}

	return 0;
}


int gsw_pmac_get_ig_cfg(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PMAC_Ig_Cfg_t ig_cfg;
	unsigned int cnt = 0;
	memset(&ig_cfg, 0, sizeof(GSW_PMAC_Ig_Cfg_t));

	cnt += scanParamArg(argc, argv, "nPmacId", sizeof(ig_cfg.nPmacId), &ig_cfg.nPmacId);
	cnt += scanParamArg(argc, argv, "nTxDmaChanId", sizeof(ig_cfg.nTxDmaChanId), &ig_cfg.nTxDmaChanId);

	if (cnt != numPar)
		return (-2);


	printf("\nGSWIP PMAC IG CFG\n");
	printf("%10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
	       "PmacId", "TxDmaChId", "ErrPktDisc", "ClassEn",
	       "ClassDef", "eSubId", "bSpIdDef", "bPmacPr", "DefPmacHdr");

	if (cli_ioctl(fd, GSW_PMAC_IG_CFG_GET, &ig_cfg) != 0)
		return (-1);

	printf("%10d %10d %10d %10d %10d %10d %10d %10d %10x:%x:%x:%x:%x:%x:%x:%x",
	       ig_cfg.nPmacId, ig_cfg.nTxDmaChanId,
	       ig_cfg.bErrPktsDisc, ig_cfg.bClassEna,
	       ig_cfg.bClassDefault, ig_cfg.eSubId,
	       ig_cfg.bSpIdDefault, ig_cfg.bPmacPresent,
	       ig_cfg.defPmacHdr[0], ig_cfg.defPmacHdr[1],
	       ig_cfg.defPmacHdr[2], ig_cfg.defPmacHdr[3],
	       ig_cfg.defPmacHdr[4], ig_cfg.defPmacHdr[5],
	       ig_cfg.defPmacHdr[6], ig_cfg.defPmacHdr[7]);
	printf("\n");
	return 0;
}

int gsw_pce_rule_alloc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_alloc_t sVar = {0};
	unsigned int cnt = 0;

	cnt += scanParamArg(argc, argv, "nNumberOfEntries", sizeof(sVar.num_of_rules), &sVar.num_of_rules);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_RULE_ALLOC, &sVar) != 0)
		return (-1);

	printf("\n\tAllocated blockid = %u", sVar.blockid);
	printf("\n\t Number of entries associated with blockid %u = %u\n",
	       sVar.blockid, sVar.num_of_rules);

	return 0;
}

int gsw_pce_rule_free(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_alloc_t sVar = {0};
	unsigned int cnt = 0;

	cnt += scanParamArg(argc, argv, "nBlockId", sizeof(sVar.blockid), &sVar.blockid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_RULE_FREE, &sVar) != 0)
		return (-1);

	printf("\n\tFreed blockid = %u", sVar.blockid);
	printf("\n\t Number of entries associated with blockid %u = %u\n",
	       sVar.blockid, sVar.num_of_rules);

	return 0;
}

int gsw_debug_pceruletablestatus(int argc, char *argv[], void *fd, int numPar)
{
	GSW_debug_t sVar;
	unsigned int cnt = 0;
	memset(&sVar, 0x00, sizeof(sVar));

	cnt += scanParamArg(argc, argv, "Index", sizeof(sVar.nTableIndex), &sVar.nTableIndex);
	cnt += scanParamArg(argc, argv, "Set", sizeof(sVar.nForceSet), &sVar.nForceSet);
	cnt += scanParamArg(argc, argv, "CheckAll", sizeof(sVar.nCheckIndexInUse), &sVar.nCheckIndexInUse);
	cnt += scanParamArg(argc, argv, "blockid", sizeof(sVar.nblockid), &sVar.nblockid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_DEBUG_PCERULETABLE_STATUS, &sVar) != 0)
		return (-1);

	return 0;
}

int gsw_vxlan_cfg_uget(int argc, char *argv[], void *fd, int numPar)
{
	gsw_vxlan_cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(gsw_vxlan_cfg_t));
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.index), &param.index);

	if (cnt != 1)
		return (-2);

	if (cli_ioctl(fd, GSW_VXLAN_CFG_GET, &param) != 0) {
		return (-1);
	}

	printf("\n\t nValue = 0x%x\n", param.vxlan_value);
	return 0;
}

int gsw_vxlan_cfg_uset(int argc, char *argv[], void *fd, int numPar)
{
	gsw_vxlan_cfg_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(gsw_vxlan_cfg_t));
	cnt += scanParamArg(argc, argv, "nValue", sizeof(param.vxlan_value), &param.vxlan_value);
	cnt += scanParamArg(argc, argv, "nIndex", sizeof(param.index), &param.index);

	if (cnt != 2)
		return (-2);

	if (cli_ioctl(fd, GSW_VXLAN_CFG_SET, &param) != 0)
		return (-1);

	return 0;
}

//#endif /* SWAPI_ETC_CHIP */

