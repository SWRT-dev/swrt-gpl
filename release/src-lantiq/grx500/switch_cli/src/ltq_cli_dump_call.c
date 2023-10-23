/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ltq_cli_lib.h"
#include "ltq_cli_lib.h"
#include "lantiq_ethsw.h"

static void dumpParameter(char *data, int size)
{
	int i;

	if (data == NULL) return;

	printf("Dump %d bytes:", size);

	for (i = 0; i < size; i++) {
		if ((i % 4) == 0) {
			if ((i % 16) == 0) {
				printf("\n0x%02X:", i);
			}

			printf(" ");
		}

		printf("%02X ", 0xFF & ((unsigned char)data[i]));
	}

	printf("\n");
}

static int cli_dump_ethsw_cap_get(int fd, unsigned long int request, void *par)
{
	GSW_cap_t *param = (GSW_cap_t *)par;

	switch (param->nCapType) {
	case GSW_CAP_TYPE_PORT:
		strcpy(param->cDesc, "Number of physical Ethernet ports.");
		param->nCap = 7;
		break;

	case GSW_CAP_TYPE_VIRTUAL_PORT:
		strcpy(param->cDesc, "Number of virtual Ethernet ports.");
		param->nCap = 6;
		break;

	case GSW_CAP_TYPE_BUFFER_SIZE:
		strcpy(param->cDesc, "Size of internal packet memory [in Bytes].");
		param->nCap = 65536;
		break;

	case GSW_CAP_TYPE_SEGMENT_SIZE:
		strcpy(param->cDesc, "Buffer segment size. Byte size of a segment, used to store received packet data.");
		param->nCap = 256;
		break;

	case GSW_CAP_TYPE_PRIORITY_QUEUE:
		strcpy(param->cDesc, "Number of priority queues per device.");
		param->nCap = 32;
		break;

	case GSW_CAP_TYPE_METER:
		strcpy(param->cDesc, "Number of meter instances.");
		param->nCap = 8;
		break;

	case GSW_CAP_TYPE_RATE_SHAPER:
		strcpy(param->cDesc, "Number of rate shaper instances.");
		param->nCap = 16;
		break;

	case GSW_CAP_TYPE_VLAN_GROUP:
		strcpy(param->cDesc, "Number of VLAN groups that can be configured on the switch hardware.");
		param->nCap = 64;
		break;

	case GSW_CAP_TYPE_FID:
		strcpy(param->cDesc, "Number of Filtering Identifiers (FIDs)");
		param->nCap = 64;
		break;

	case GSW_CAP_TYPE_MAC_TABLE_SIZE:
		strcpy(param->cDesc, "Number of MAC table entries");
		param->nCap = 2048;
		break;

	case GSW_CAP_TYPE_MULTICAST_TABLE_SIZE:
		strcpy(param->cDesc, "Number of multicast level 3 hardware table entries");
		param->nCap = 64;
		break;

	case GSW_CAP_TYPE_PPPOE_SESSION:
		strcpy(param->cDesc, "Number of supported PPPoE sessions.");
		param->nCap = 16;
		break;

	default:
		break;
	}

	return IFX_SUCCESS;
}

static int cli_dump_ethsw_qos_queue_port_get(int fd, unsigned long int request, void *par)
{
	GSW_QoS_queuePort_t *param = (GSW_QoS_queuePort_t *)par;

	if (param->nTrafficClassId >= 16) {
		printf("ERROR: API called with large \"nTrafficClassId\" parameter!\n");
		return IFX_ERROR;
	}

	if (param->nPortId >= 7) {
		printf("ERROR: API called with large \"nPortId\" parameter!\n");
		return IFX_ERROR;
	}

	if (param->nTrafficClassId > 3) {
		param->nQueueId = param->nPortId * 4;
	} else {
		param->nQueueId = param->nTrafficClassId + (param->nPortId * 4);
	}

	return IFX_SUCCESS;
}

static int round = 0;

static int cli_dump_ethsw_mac_table_entry_read(int fd, unsigned long int request, void *par)
{
	static const GSW_MAC_tableRead_t table[] = {
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nFId = 0,
			.nPortId = 4,
			.nAgeTimer = 20,
			.bStaticEntry = IFX_TRUE,
			.nMAC = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xbc}
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nFId = 12,
			.nPortId = 0x12 | GSW_PORTMAP_FLAG_SET(GSW_MAC_tableRead_t),
			.nAgeTimer = 10,
			.bStaticEntry = IFX_TRUE,
			.nMAC = {0, 0, 0, 0, 0, 0}
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nFId = 18,
			.nPortId = 5,
			.nAgeTimer = 5,
			.bStaticEntry = IFX_FALSE,
			.nMAC = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}
		},
		{
			/* last entry */
			.bInitial = IFX_FALSE,
			.bLast = IFX_TRUE
		}
	};

	if (round < (sizeof(table) / sizeof(GSW_MAC_tableRead_t))) {
		memcpy(par, &table[round++], sizeof(GSW_MAC_tableRead_t));
	} else {
		printf("cli_dump called to often!\n");
		return IFX_ERROR;
	}

	return IFX_SUCCESS;
}

static int cli_dump_ethsw_vlan_port_member_read(int fd, unsigned long int request, void *par)
{
	static const GSW_VLAN_portMemberRead_t table[] = {
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nVId = 2,
			.nPortId = 5,
			.nTagId = 5
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nVId = 6,
			.nPortId = 2,
			.nTagId = 0
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nVId = 4,
			.nPortId = 0x37 | GSW_PORTMAP_FLAG_SET(GSW_VLAN_portMemberRead_t),
			.nTagId = 0x11 | GSW_PORTMAP_FLAG_SET(GSW_VLAN_portMemberRead_t)
		},
		{
			/* last entry */
			.bInitial = IFX_FALSE,
			.bLast = IFX_TRUE
		}
	};

	if (round < (sizeof(table) / sizeof(GSW_VLAN_portMemberRead_t))) {
		memcpy(par, &table[round++], sizeof(GSW_VLAN_portMemberRead_t));
	} else {
		printf("cli_dump called to often!\n");
		return IFX_ERROR;
	}

	return IFX_SUCCESS;
}

static int cli_dump_ethsw_multicast_table_entry_read(int fd, unsigned long int request, void *par)
{

	static const GSW_multicastTableRead_t table[] = {
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nPortId = 3,
			.eIPVersion = GSW_IP_SELECT_IPV4,
			.uIP_Gda.nIPv4 = 0xE000000A, /* 224.0.0.10 */
			.uIP_Gsa.nIPv4 = 0xC0A80001, /* 192.168.0.1 */
			.eModeMember = GSW_IGMP_MEMBER_INCLUDE
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nPortId = 0x7F | GSW_PORTMAP_FLAG_SET(GSW_multicastTableRead_t),
			.eIPVersion = GSW_IP_SELECT_IPV4,
			.uIP_Gda.nIPv4 = 0xE0000118, /* 224.0.1.24 */
			.eModeMember = GSW_IGMP_MEMBER_DONT_CARE
		},
		{
			.bInitial = IFX_FALSE,
			.bLast = IFX_FALSE,
			.nPortId = 5,
			.eIPVersion = GSW_IP_SELECT_IPV6,
			.uIP_Gda.nIPv6 = {0xFF00, 0, 255, 0, 0, 0, 0, 1},
			.uIP_Gsa.nIPv6 = {0xFF00, 0, 0, 0, 0, 0, 0, 1},
			.eModeMember = GSW_IGMP_MEMBER_EXCLUDE
		},
		{
			/* last entry */
			.bInitial = IFX_FALSE,
			.bLast = IFX_TRUE,
		}
	};


	if (round < (sizeof(table) / sizeof(GSW_multicastTableRead_t))) {
		memcpy(par, &table[round++], sizeof(GSW_multicastTableRead_t));
	} else {
		printf("cli_dump called to often!\n");
		return IFX_ERROR;
	}

	return IFX_SUCCESS;
}

int cli_ioctl(int fd, unsigned long int request, void *par)
{
	switch (request) {
	case GSW_MAC_TABLE_ENTRY_READ:
		return cli_dump_ethsw_mac_table_entry_read(fd, request, par);

	case GSW_VLAN_PORT_MEMBER_READ:
		return cli_dump_ethsw_vlan_port_member_read(fd, request, par);

	case GSW_MULTICAST_TABLE_ENTRY_READ:
		return cli_dump_ethsw_multicast_table_entry_read(fd, request, par);

	case GSW_QOS_QUEUE_PORT_GET:
		return cli_dump_ethsw_qos_queue_port_get(fd, request, par);

	case GSW_CAP_GET:
		return cli_dump_ethsw_cap_get(fd, request, par);

	default:
		if (_IOC_DIR(request) & _IOC_WRITE) {
			dumpParameter(par, _IOC_SIZE(request));
		}

		if (_IOC_DIR(request) & _IOC_READ) {
			memset(par, 0xFF, _IOC_SIZE(request));
		}

		break;
	}

	return 0;
}

