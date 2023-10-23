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
int gsw_mac_table_entry_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MAC_tableRead_t MAC_tableRead;

	printf("-------------------------------------------------------------\n");
	printf("   MAC Address    | Port  |   Age  |  SID  | FID  | Static\n");
	printf("-------------------------------------------------------------\n");

	memset(&MAC_tableRead, 0x00, sizeof(MAC_tableRead));
	MAC_tableRead.bInitial = 1;

	for (;;) {
		if (cli_ioctl(fd, GSW_MAC_TABLE_ENTRY_READ, &MAC_tableRead) != 0) {
			return (-2);
		}

		if (MAC_tableRead.bLast == 1)
			break;

		if (checkValidMAC_Address(MAC_tableRead.nMAC)) {
			if ((MAC_tableRead.nAgeTimer == 0) && (MAC_tableRead.bStaticEntry == 0)) {
				/* Do nothing */
				continue;
			}
		}

		if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2) {
			if (MAC_tableRead.bStaticEntry) {
				unsigned int i = 0;
				int port = 0;
				printMAC_Address(MAC_tableRead.nMAC);
				printf(" |  %3d  |  %4d  | %4d  | %3d  | TRUE(static)\n",
				       port, MAC_tableRead.nAgeTimer, MAC_tableRead.nSubIfId, MAC_tableRead.nFId);
				printf("\t\t\t\t\t nFilterFlag(key):\t0x%x\n\n", MAC_tableRead.nFilterFlag);

				for (i = 0; i < 16; i++) {
					if (!MAC_tableRead.nPortMap[i])
						continue;

					printf("\t\t\t\t\t PortMap[ %d ]:\t0x%x\n", i, MAC_tableRead.nPortMap[i]);
				}

				printf("\t\t\t\t\t Hit Status:\t%d\n", MAC_tableRead.hitstatus);
			} else {
				printMAC_Address(MAC_tableRead.nMAC);
				printf(" |  %3d  |  %4d  | %4d  | %3d  | FALSE(dynamic)\n",
				       MAC_tableRead.nPortId, MAC_tableRead.nAgeTimer, MAC_tableRead.nSubIfId, MAC_tableRead.nFId);
			}
		} else {
			if (MAC_tableRead.bStaticEntry != 0) {
				int port;

				for (port = 0; port < (sizeof(MAC_tableRead.nPortId) * 8 - 1); port++) {
					if (((MAC_tableRead.nPortId >> port) & 0x1) == 1) {
						printMAC_Address(MAC_tableRead.nMAC);
						printf(" |  %3d  |  %4d  |  %4d  | %3d  |  TRUE\n",
						       port, MAC_tableRead.nAgeTimer, MAC_tableRead.nSubIfId, MAC_tableRead.nFId);
					}
				}
			} else {
				printMAC_Address(MAC_tableRead.nMAC);
				printf(" |  %3d  |  %4d  |  %4d  | %3d  | FALSE\n",
				       MAC_tableRead.nPortId, MAC_tableRead.nAgeTimer, MAC_tableRead.nSubIfId, MAC_tableRead.nFId);
			}
		}

		memset(&MAC_tableRead, 0x00, sizeof(MAC_tableRead));
	}

	printf("------------------------------------------------------------\n");
	return 0;
}

int gsw_multicast_router_port_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastRouterRead_t multicastRouterRead;

	memset(&multicastRouterRead, 0x00, sizeof(multicastRouterRead));
	multicastRouterRead.bInitial = 1;

	for (;;) {
		if (cli_ioctl(fd, GSW_MULTICAST_ROUTER_PORT_READ, &multicastRouterRead) != 0)
			return (-2);

		if (multicastRouterRead.bLast == 1)
			break;

		printf("\tRouter Port : %d\n", multicastRouterRead.nPortId);
		memset(&multicastRouterRead, 0x00, sizeof(multicastRouterRead));
	}

	return 0;
}

int gsw_qos_class_dscp_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ClassDSCP_Cfg_t classDSCP_Cfg;
	int i;
	memset(&classDSCP_Cfg, 0x00, sizeof(classDSCP_Cfg));

	if (cli_ioctl(fd, GSW_QOS_CLASS_DSCP_GET, &classDSCP_Cfg) != 0)
		return (-2);

	for (i = 0; i < 16; i++)
		printf("\tnDSCP[%d] = %d\n", i, classDSCP_Cfg.nDSCP[i]);

	return 0;
}

int gsw_qos_class_dscp_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ClassDSCP_Cfg_t ClassDSCP_Cfg;
	unsigned int nTrafficClass, cnt;
	unsigned char nDSCP;

	cnt = scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nDSCP", sizeof(nDSCP), &nDSCP);

	if (cnt != 2) return (-2);

	if (nTrafficClass >= 16) {
		printf("ERROR: Given \"nTrafficClass\" is out of range (15)\n");
		return (-3);
	}

	memset(&ClassDSCP_Cfg, 0x00, sizeof(ClassDSCP_Cfg));

	if (cli_ioctl(fd, GSW_QOS_CLASS_DSCP_GET, &ClassDSCP_Cfg) != 0)
		return (-4);

	ClassDSCP_Cfg.nDSCP[nTrafficClass] = nDSCP;

	if (cli_ioctl(fd, GSW_QOS_CLASS_DSCP_SET, &ClassDSCP_Cfg) != 0)
		return (-5);

	return 0;
}

int gsw_qos_class_pcp_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ClassPCP_Cfg_t classPCP_Cfg;
	int i;

	memset(&classPCP_Cfg, 0x00, sizeof(classPCP_Cfg));

	if (cli_ioctl(fd, GSW_QOS_CLASS_PCP_GET, &classPCP_Cfg) != 0)
		return (-2);

	for (i = 0; i < 16; i++)
		printf("\tnPCP[%d] = %d\n", i, classPCP_Cfg.nPCP[i]);

	return 0;
}

int gsw_qos_class_pcp_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_ClassPCP_Cfg_t classPCP_Cfg;
	unsigned int nTrafficClass, cnt;
	unsigned char nPCP;

	cnt = scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nPCP", sizeof(nPCP), &nPCP);

	if (cnt != 2) return (-2);

	if (nTrafficClass >= 16) {
		printf("ERROR: Given \"nTrafficClass\" is out of range (15)\n");
		return (-3);
	}

	memset(&classPCP_Cfg, 0x00, sizeof(classPCP_Cfg));

	if (cli_ioctl(fd, GSW_QOS_CLASS_PCP_GET, &classPCP_Cfg) != 0)
		return (-4);

	classPCP_Cfg.nPCP[nTrafficClass] = nPCP;

	if (cli_ioctl(fd, GSW_QOS_CLASS_PCP_SET, &classPCP_Cfg) != 0)
		return (-5);

	return 0;
}

int gsw_qos_dscp_class_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_DSCP_ClassCfg_t DSCP_ClassCfg;
	int i;

	memset(&DSCP_ClassCfg, 0x00, sizeof(DSCP_ClassCfg));

	if (cli_ioctl(fd, GSW_QOS_DSCP_CLASS_GET, &DSCP_ClassCfg) != 0)
		return (-2);

	for (i = 0; i < 64; i++)
		printf("\tnTrafficClass[%d] = %d\n", i, DSCP_ClassCfg.nTrafficClass[i]);

	return 0;
}

int gsw_qos_dscp_class_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_DSCP_ClassCfg_t DSCP_ClassCfg;
	unsigned char nTrafficClass;
	unsigned int nDSCP, cnt;

	cnt = scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nDSCP", sizeof(nDSCP), &nDSCP);

	if (cnt != 2) return (-2);

	if (nDSCP >= 64) {
		printf("ERROR: Given \"nDSCP\" is out of range (63)\n");
		return (-3);
	}

	memset(&DSCP_ClassCfg, 0x00, sizeof(DSCP_ClassCfg));

	if (cli_ioctl(fd, GSW_QOS_DSCP_CLASS_GET, &DSCP_ClassCfg) != 0)
		return (-4);

	DSCP_ClassCfg.nTrafficClass[nDSCP] = nTrafficClass;

	if (cli_ioctl(fd, GSW_QOS_DSCP_CLASS_SET, &DSCP_ClassCfg) != 0)
		return (-5);

	return 0;
}

int gsw_qos_pcp_class_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_PCP_ClassCfg_t PCP_ClassCfg;
	int i;

	memset(&PCP_ClassCfg, 0x00, sizeof(PCP_ClassCfg));

	if (cli_ioctl(fd, GSW_QOS_PCP_CLASS_GET, &PCP_ClassCfg) != 0)
		return (-2);

	for (i = 0; i < 16; i++)
		printf("\tnTrafficClass[%d] = %d\n", i, (int)PCP_ClassCfg.nTrafficClass[i]);

	return 0;
}

int gsw_qos_pcp_class_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_PCP_ClassCfg_t PCP_ClassCfg;
	unsigned char nTrafficClass;
	unsigned int nPCP, cnt;

	cnt = scanParamArg(argc, argv, "nTrafficClass", sizeof(nTrafficClass), &nTrafficClass);
	cnt += scanParamArg(argc, argv, "nPCP", sizeof(nPCP), &nPCP);

	if (cnt != 2) return (-2);

	if (nPCP >= 16) {
		printf("ERROR: Given \"nPCP\" is out of range (7)\n");
		return (-3);
	}

	memset(&PCP_ClassCfg, 0x00, sizeof(PCP_ClassCfg));

	if (cli_ioctl(fd, GSW_QOS_PCP_CLASS_GET, &PCP_ClassCfg) != 0)
		return (-4);

	PCP_ClassCfg.nTrafficClass[nPCP] = nTrafficClass;

	if (cli_ioctl(fd, GSW_QOS_PCP_CLASS_SET, &PCP_ClassCfg) != 0)
		return (-5);

	return 0;
}

int gsw_qos_dscp_drop_precedence_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_DSCP_DropPrecedenceCfg_t DropPrecedenceCfg;
	int i;

	memset(&DropPrecedenceCfg, 0x00, sizeof(DropPrecedenceCfg));

	if (cli_ioctl(fd, GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET, &DropPrecedenceCfg) != 0)
		return (-2);

	for (i = 0; i < 64; i++) {
		char *ptr;

		switch (DropPrecedenceCfg.nDSCP_DropPrecedence[i]) {
		case GSW_DROP_PRECEDENCE_CRITICAL:
			ptr = "CRITICAL";
			break;

		case GSW_DROP_PRECEDENCE_GREEN:
			ptr = "GREEN";
			break;

		case GSW_DROP_PRECEDENCE_YELLOW:
			ptr = "YELLOW";
			break;

		case GSW_DROP_PRECEDENCE_RED:
			ptr = "RED";
			break;

		default:
			ptr = "UNKNOWN";
			break;
		}

		printf("\tDSCP_DropPrecedence[%d] = %s(%d)\n",
		       i, ptr, DropPrecedenceCfg.nDSCP_DropPrecedence[i]);
	}

	return 0;
}

int gsw_qos_dscp_drop_precedence_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_DSCP_DropPrecedenceCfg_t DropPrecedenceCfg;
	unsigned int nDSCP_DropPrecedence, nDSCP, cnt;

	cnt = scanParamArg(argc, argv, "nDSCP_DropPrecedence", sizeof(nDSCP_DropPrecedence), &nDSCP_DropPrecedence);
	cnt += scanParamArg(argc, argv, "nDSCP", sizeof(nDSCP), &nDSCP);

	if (cnt != 2) return (-2);

	if (nDSCP >= 64) {
		printf("ERROR: Given \"nDSCP\" is out of range (63)\n");
		return (-3);
	}

	memset(&DropPrecedenceCfg, 0x00, sizeof(DropPrecedenceCfg));

	if (cli_ioctl(fd, GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET, &DropPrecedenceCfg) != 0)
		return (-4);

	DropPrecedenceCfg.nDSCP_DropPrecedence[nDSCP] = nDSCP_DropPrecedence;

	if (cli_ioctl(fd, GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET, &DropPrecedenceCfg) != 0)
		return (-5);

	return 0;
}

int gsw_qos_meter_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_meterPortGet_t qos_meterportget;

	memset(&qos_meterportget, 0x00, sizeof(qos_meterportget));
	qos_meterportget.bInitial = 1;

	for (;;) {
		if (cli_ioctl(fd, GSW_QOS_METER_PORT_GET, &qos_meterportget) != 0) {
			return (-2);
		}

		if (qos_meterportget.bLast != 0)
			break;

		switch (qos_meterportget.eDir) {
		case GSW_DIRECTION_NONE:
			printf("\tMeter Index = %2d | Direction = None\n", qos_meterportget.nMeterId);
			break;

		case GSW_DIRECTION_INGRESS:
			printf("\tMeter Index = %2d | Direction = Ingress | PortID = %2d\n",
			       qos_meterportget.nMeterId, qos_meterportget.nPortIngressId);
			break;

		case GSW_DIRECTION_EGRESS:
			printf("\tMeter Index = %2d | Direction = Egress | PortID = %2d\n",
			       qos_meterportget.nMeterId, qos_meterportget.nPortEgressId);
			break;

		case GSW_DIRECTION_BOTH:
			printf("\tMeter Index = %2d | Direction = Both | Ingress PortID = %2d | Egress PortID = %2d\n",
			       qos_meterportget.nMeterId,
			       qos_meterportget.nPortIngressId,
			       qos_meterportget.nPortEgressId);
			break;
		}

		memset(&qos_meterportget, 0x00, sizeof(qos_meterportget));
	}

	return 0;
}

int gsw_version_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_version_t version;
	int i;

	for (i = 0;; i++) {
		memset(&version, 0x00, sizeof(version));
		version.nId = (u16)i;

		if (cli_ioctl(fd, GSW_VERSION_GET, &version) != 0)
			return (-2);

		if ((strlen(version.cName) == 0) || (strlen(version.cVersion) == 0))
			break;

		printf("\tVersion ID Name: %s\n", version.cName);
		printf("\tVersion String: %s\n", version.cVersion);
	}

	return 0;
}

int gsw_cap_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_cap_t cap;
	GSW_capType_t i;

	for (i = GSW_CAP_TYPE_PORT; i < GSW_CAP_TYPE_LAST; i++) {
		memset(&cap, 0x00, sizeof(cap));
		cap.nCapType = i;

		if (cli_ioctl(fd, GSW_CAP_GET, &cap) != 0)
			return (-2);

		if (cap.nCap != 0) {
			printf("\t%s : %d\n", cap.cDesc, cap.nCap);
		}
	}

	return 0;
}

int gsw_cpu_port_extend_cfg_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CPU_PortExtendCfg_t CPU_PortExtendCfg;
	char *peHeaderAdd;

	memset(&CPU_PortExtendCfg, 0x00, sizeof(CPU_PortExtendCfg));

	if (cli_ioctl(fd, GSW_CPU_PORT_EXTEND_CFG_GET, &CPU_PortExtendCfg) != 0)
		return (-2);

	switch (CPU_PortExtendCfg.eHeaderAdd) {
	case GSW_CPU_HEADER_NO:
		peHeaderAdd = "Add No Header";
		break;

	case GSW_CPU_HEADER_MAC:
		peHeaderAdd = "Add Ethernet Header";
		break;

	case GSW_CPU_HEADER_VLAN:
		peHeaderAdd = "Add Ethernet & VLAN Header";
		break;

	default:
		peHeaderAdd = "UNKOWN";
		break;
	}

	printf("eHeaderAdd : %s\n", peHeaderAdd);
	printf("bHeaderRemove : %s\n", (CPU_PortExtendCfg.bHeaderRemove > 0) ? "TRUE" : "FALSE");

	if (CPU_PortExtendCfg.eHeaderAdd != GSW_CPU_HEADER_NO) {
		printf("sHeader.nMAC_Src : ");
		printMAC_Address(CPU_PortExtendCfg.sHeader.nMAC_Src);
		printf("\nsHeader.nMAC_Dst : ");
		printMAC_Address(CPU_PortExtendCfg.sHeader.nMAC_Dst);
		printf("\nsHeader.nEthertype : %04X\n", CPU_PortExtendCfg.sHeader.nEthertype);
	}

	if (CPU_PortExtendCfg.eHeaderAdd == GSW_CPU_HEADER_VLAN) {
		printf("sHeader.nVLAN_Prio : %d\n", CPU_PortExtendCfg.sHeader.nVLAN_Prio);
		printf("sHeader.nVLAN_CFI : %d\n", CPU_PortExtendCfg.sHeader.nVLAN_CFI);
		printf("sHeader.nVLAN_ID : %d\n", CPU_PortExtendCfg.sHeader.nVLAN_ID);
	}

	printf("ePauseCtrl : %s\n",
	       (CPU_PortExtendCfg.ePauseCtrl == GSW_CPU_PAUSE_DISPATCH) ? "DISPATCH" : "FORWARD");

	printf("bFcsRemove : %s\n", (CPU_PortExtendCfg.bFcsRemove > 0) ? "TRUE" : "FALSE");
	printf("nWAN_Ports : 0x%08X\n", CPU_PortExtendCfg.nWAN_Ports);

	return 0;
}

int gsw_cpu_port_extend_cfg_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_CPU_PortExtendCfg_t CPU_PortExtendCfg;
	unsigned int cnt;

	memset(&CPU_PortExtendCfg, 0x00, sizeof(CPU_PortExtendCfg));

	if (cli_ioctl(fd, GSW_CPU_PORT_EXTEND_CFG_GET, &CPU_PortExtendCfg) != 0)
		return (-2);

	cnt = scanParamArg(argc, argv, "eHeaderAdd", sizeof(CPU_PortExtendCfg.eHeaderAdd), &CPU_PortExtendCfg.eHeaderAdd);
	cnt += scanParamArg(argc, argv, "bHeaderRemove", sizeof(CPU_PortExtendCfg.bHeaderRemove), &CPU_PortExtendCfg.bHeaderRemove);
	cnt += scanMAC_Arg(argc, argv, "sHeader.nMAC_Src", CPU_PortExtendCfg.sHeader.nMAC_Src);
	cnt += scanMAC_Arg(argc, argv, "sHeader.nMAC_Dst", CPU_PortExtendCfg.sHeader.nMAC_Dst);
	cnt += scanParamArg(argc, argv, "sHeader.nEthertype", sizeof(CPU_PortExtendCfg.sHeader.nEthertype), &CPU_PortExtendCfg.sHeader.nEthertype);
	cnt += scanParamArg(argc, argv, "sHeader.nVLAN_Prio", sizeof(CPU_PortExtendCfg.sHeader.nVLAN_Prio), &CPU_PortExtendCfg.sHeader.nVLAN_Prio);
	cnt += scanParamArg(argc, argv, "sHeader.nVLAN_CFI", sizeof(CPU_PortExtendCfg.sHeader.nVLAN_CFI), &CPU_PortExtendCfg.sHeader.nVLAN_CFI);
	cnt += scanParamArg(argc, argv, "sHeader.nVLAN_ID", sizeof(CPU_PortExtendCfg.sHeader.nVLAN_ID), &CPU_PortExtendCfg.sHeader.nVLAN_ID);
	cnt += scanParamArg(argc, argv, "ePauseCtrl", sizeof(CPU_PortExtendCfg.ePauseCtrl), &CPU_PortExtendCfg.ePauseCtrl);
	cnt += scanParamArg(argc, argv, "bFcsRemove", sizeof(CPU_PortExtendCfg.bFcsRemove), &CPU_PortExtendCfg.bFcsRemove);
	cnt += scanParamArg(argc, argv, "nWAN_Ports", sizeof(CPU_PortExtendCfg.nWAN_Ports), &CPU_PortExtendCfg.nWAN_Ports);

	if (cnt != numPar) return (-3);

	if (cli_ioctl(fd, GSW_CPU_PORT_EXTEND_CFG_SET, &CPU_PortExtendCfg) != 0)
		return (-4);

	return 0;
}

static int multicastParamRead(int argc, char *argv[], GSW_multicastTable_t *param)
{
	int cnt = 0;
	int ipParamCnt1 = 0;
	int ipParamCnt2 = 0;

	memset(param, 0, sizeof(GSW_multicastTable_t));

	cnt += scanParamArg(argc, argv, "nPortId", sizeof(param->nPortId), &param->nPortId);
	cnt += scanParamArg(argc, argv, "nSubIfId", sizeof(param->nSubIfId), &param->nSubIfId);
	cnt += scanParamArg(argc, argv, "eIPVersion", sizeof(param->eIPVersion), &param->eIPVersion);
	cnt += scanParamArg(argc, argv, "eModeMember", sizeof(param->eModeMember), &param->eModeMember);
	cnt += scanParamArg(argc, argv, "nFID", sizeof(param->nFID), &param->nFID);
	cnt += scanParamArg(argc, argv, "bExclSrcIP", sizeof(param->bExclSrcIP), &param->bExclSrcIP);

	if (param->eIPVersion == GSW_IP_SELECT_IPV4) {
		ipParamCnt1 = scanIPv4_Arg(argc, argv, "uIP_Gda", &param->uIP_Gda.nIPv4);
		ipParamCnt2 = scanIPv4_Arg(argc, argv, "uIP_Gsa", &param->uIP_Gsa.nIPv4);
	} else {
		ipParamCnt1 = scanIPv6_Arg(argc, argv, "uIP_Gda", param->uIP_Gda.nIPv6);
		ipParamCnt2 = scanIPv6_Arg(argc, argv, "uIP_Gsa", param->uIP_Gsa.nIPv6);
	}

	if ((param->eModeMember != GSW_IGMP_MEMBER_DONT_CARE) &&
	    (ipParamCnt1 == 0) && (ipParamCnt2 == 0))
		return (-2);

	if (ipParamCnt1 == 0)
		return (-3);

	return 0;
}

int gsw_multicast_table_entry_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastTable_t param;
	int retval;

	retval = multicastParamRead(argc, argv, &param);

	if (retval != 0) return retval;

	if (cli_ioctl(fd, GSW_MULTICAST_TABLE_ENTRY_ADD, &param) != 0)
		return -1;

	return 0;
}

int gsw_multicast_table_entry_remove(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastTable_t param;
	int retval;

	retval = multicastParamRead(argc, argv, &param);

	if (retval != 0) return retval;

	if (cli_ioctl(fd, GSW_MULTICAST_TABLE_ENTRY_REMOVE, &param) != 0)
		return -1;

	return 0;
}

static void dump_multicast_table_entry(GSW_multicastTableRead_t *ptr)
{
	if (ptr->eIPVersion == GSW_IP_SELECT_IPV4) {
		printIPv4_Address(ptr->uIP_Gda.nIPv4);
	} else
		printIPv6_Address(ptr->uIP_Gda.nIPv6);

	if (ptr->eModeMember != GSW_IGMP_MEMBER_DONT_CARE) {

		if (ptr->eIPVersion == GSW_IP_SELECT_IPV4) {
			printIPv4_Address(ptr->uIP_Gsa.nIPv4);
		} else
			printIPv6_Address(ptr->uIP_Gsa.nIPv6);

		printf(" %11s |", (ptr->eModeMember == GSW_IGMP_MEMBER_INCLUDE) ? "INCLUDE" : "EXCLUDE");
		printf(" %10s |", (ptr->bExclSrcIP == 1) ? "TRUE" : "FALSE");
	}

	if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2)
		printf(" %9d |", ptr->hitstatus);

	printf("\n");

}

int gsw_multicast_table_entry_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_multicastTableRead_t multicastTableRead;
	int k = 0, valid = 0;
	u8 mcasthitsts_en = 0;

	if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2) {
		GSW_register_t param;
		memset(&param, 0, sizeof(GSW_register_t));
		param.nRegAddr = 0x456;

		if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}

		if (param.nData & 0x2000)
			mcasthitsts_en = 1;
		else
			mcasthitsts_en = 0;
	}


	if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2) {
		printf("-----------------------------------------------------------------------------------------------------------\n");
		printf("| %4s | %15s | %3s | %15s | %15s | %11s | %10s | %9s |\n", "Port", "Sub Id", "FID", "GDA", "GSA", "Member Mode", "bExclSrcIP", "HitStatus");
		printf("-----------------------------------------------------------------------------------------------------------\n");
	} else {
		printf("-----------------------------------------------------------------------------------------------\n");
		printf("| %4s | %15s | %3s | %15s | %15s | %11s | %10s |\n", "Port", "Sub Id", "FID", "GDA", "GSA", "Member Mode", "bExclSrcIP");
		printf("-----------------------------------------------------------------------------------------------\n");
	}

	memset(&multicastTableRead, 0x00, sizeof(multicastTableRead));
	multicastTableRead.bInitial = 1;

	for (;;) {
		if (cli_ioctl(fd, GSW_MULTICAST_TABLE_ENTRY_READ, &multicastTableRead) != 0) {
			return (-2);
		}

		if (multicastTableRead.bLast == 1)
			break;


		if ((multicastTableRead.nPortId == 0) && (multicastTableRead.nSubIfId == 0) && (multicastTableRead.nFID == 0)) {
			valid = 0;

			for (k = 0; k < 8; k++) {
				if (multicastTableRead.uIP_Gsa.nIPv6[k] != 0)
					valid = 1;
			}

			for (k = 0; k < 8; k++) {
				if (multicastTableRead.uIP_Gda.nIPv6[k] != 0)
					valid = 1;
			}

			for (k = 0; k < 16; k++) {
				if (multicastTableRead.nPortMap[k] != 0)
					valid = 1;
			}

			if (valid == 0)
				continue;
		}

		if (multicastTableRead.nPortId & GSW_PORTMAP_FLAG_GET(GSW_multicastTableRead_t)) {

			unsigned int i = 0, j = 0, mask = 1;

			if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2) {
				for (j = 0; j < 16; j++) {
					i = 0;
					mask = 1;

					if (!multicastTableRead.nPortMap[j])
						continue;

					while (mask <= (1 << 16)) {
						if (mask & multicastTableRead.nPortMap[j]) {
							if (mcasthitsts_en && (j == 7) && (mask == (1 << 15))) {
								break;
							}

							printf("| %4d |", (j * 16) + i);
							printf(" %15d |", multicastTableRead.nSubIfId);
							printf(" %3d |", multicastTableRead.nFID);
							dump_multicast_table_entry(&multicastTableRead);
						}

						i++;
						mask = 1 << i;
					}
				}
			} else {
				while (mask != GSW_PORTMAP_FLAG_GET(GSW_multicastTableRead_t)) {
					if (mask & multicastTableRead.nPortId) {
						printf("| %4d |", (j * 16) + i);
						printf(" %15d |", multicastTableRead.nSubIfId);
						printf(" %3d |", multicastTableRead.nFID);
						dump_multicast_table_entry(&multicastTableRead);
					}

					i++;
					mask = 1 << i;
				}
			}
		} else {
			printf("| %4d |", multicastTableRead.nPortId);
			printf(" %15d |", multicastTableRead.nSubIfId);
			printf(" %3d |", multicastTableRead.nFID);
			dump_multicast_table_entry(&multicastTableRead);
		}

		memset(&multicastTableRead, 0x00, sizeof(multicastTableRead));
	}

	if (gswip_version == GSWIP_3_1 || gswip_version == GSWIP_3_2)
		printf("-----------------------------------------------------------------------------------------------------------\n");
	else
		printf("-----------------------------------------------------------------------------------------------\n");

	return 0;
}

int gsw_vlan_port_member_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_VLAN_portMemberRead_t portMemberRead;

	printf("----------------------------\n");
	printf(" VLAN ID | Port | Tag Member\n");
	printf("----------------------------\n");
	memset(&portMemberRead, 0x00, sizeof(portMemberRead));
	portMemberRead.bInitial = 1;

	for (;;) {
		if (cli_ioctl(fd, GSW_VLAN_PORT_MEMBER_READ, &portMemberRead) != 0) {
			return (-2);
		}

		if (portMemberRead.bLast == 1)
			break;

		if (portMemberRead.nVId) {
//				if (portMemberRead.nPortId & 0xFFFF) {
			if (portMemberRead.nPortId & GSW_PORTMAP_FLAG_GET(GSW_VLAN_portMemberRead_t)) {
				unsigned int i = 0, mask = 1;

				while (mask != GSW_PORTMAP_FLAG_GET(GSW_VLAN_portMemberRead_t)) {
					if (mask & portMemberRead.nPortId)
						printf("%8d | %4d | %s\n",
						       portMemberRead.nVId,
						       i,
						       (portMemberRead.nTagId & mask) ? "1" : "0");

					i++;
					mask = 1 << i;
				}
			} else
				printf("%8d | %4d | %s\n", portMemberRead.nVId, portMemberRead.nPortId,
				       (portMemberRead.nTagId) ? "1" : "0");
		}

		memset(&portMemberRead, 0x00, sizeof(portMemberRead));
	}

	printf("----------------------------\n");
	return 0;
}

