/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

#include <net/ppa_api.h>

#include "qosal_inc.h"
#include "qosal_engine.h"
#include "sysapi.h"
#include "qosal_ugw_debug.h"
#include "qosal_utils.h"
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define AX_F24S_PORT	0
#define SWITCH_DEV_NODE_2   "/dev/switch_api/2"
#define ETH0_5_DIR_PATH		"/sys/class/net/eth0_5/address"

#define PPA_DEVICE   "/dev/ifx_ppa"
#define ST_OPER_GET	"/usr/sbin/status_oper -f" PLATFORM_XML"/etc/switchports.conf GET"
#define SW_PORTS_FILE	PLATFORM_XML"/etc/switchports.conf"
#define SW_PORTS_SEC	"switch_ports"
#define SW_PORTS_LTAG	"switch_lan_ports"
#define SW_PORTS_WTAG	"switch_mii1_port"
#define SW_PORTS_TAG_LEN 16

#define DSL_NUM_LINE 1

#define DSL_NUM_LINE_XRX500 1
#define SWITCH_LAN_PORT_ID_XRX500 2
#define SWITCH_WAN_PORT_ID_XRX500 15
/* all rate are in kbps - ETHLAN : 100M, ETHWAN : 100M, PTMWAN: 60M, ATMWAN : 1M */
#define DEF_ETH_LAN_RATE 1000000
#define DEF_ETH_WAN_RATE 1000000
#define DEF_PTM_WAN_RATE 60000
#define DEF_LTE_WAN_RATE 100000
#define DEF_ATM_WAN_RATE 1000
#define DEF_WLAN_RATE 1000000
#define DEF_LOCAL_RATE 10000

#define RATE_SYSFS_PATH	"/sys/class/net/"
#define BRIDGE_NAME		"br-lan"
#define DSL_STATUS_FILE	"/var/status/dsl"

#define DSL_JSON_LVL1_DSL		"dsl"
#define DSL_JSON_LVL2_STAT		"status"
#define DSL_JSON_LVL3_LINE		"line"
#define DSL_JSON_LVL4_STAT		"status"
#define DSL_JSON_LVL4_CHANL		"channel"
#define DSL_JSON_LVL5_CUR_RATE	"curr_rate"
#define DSL_JSON_LVL6_US_DIR	"us"

static uint32_t read_dsl_rate(void);
static uint32_t get_sysfs_rate(char *ifname);
int32_t nMapInUse, nDefVlanPrioCnt;
vlanMap xVlanMap[DEFAULT_TC];
extern int32_t vlanPrioUpdate(char *sIface, int32_t inPrio, int32_t out8021p, uint32_t flags);
extern void vlanPrioCfg(char *sIface);

/* =============================================================================
 *  Function Name : read_dsl_rate
 *  Description   : Read DSL uplink rate by parsing DSL daemon status file
 * ============================================================================*/
static uint32_t read_dsl_rate()
{
	ENTRY
	enum {
		IDX_DSL,
		__IDX_MAX
	};
	static const struct blobmsg_policy lvl1[] = {
		[IDX_DSL] = { .name = DSL_JSON_LVL1_DSL, .type = BLOBMSG_TYPE_TABLE },
	};
	struct blob_buf b = { 0 };
	struct blob_attr *tb[__IDX_MAX];
	char *status = NULL;
	const char *hname;
	struct blob_attr *cur, *tcur, *t1cur, *t2cur, *t3cur, *t4cur;
	unsigned int rem, trem, t1rem, t2rem, t3rem, t4rem;
	uint32_t rate = 0, linerate = 0;

	blob_buf_init(&b, 0);

	if (!blobmsg_add_json_from_file(&b, DSL_STATUS_FILE)) {
		LOGF_LOG_ERROR("Failed to load %s\n", DSL_STATUS_FILE);
		goto exit_lbl;
	}

	blobmsg_parse(lvl1, __IDX_MAX, tb, blob_data(b.head), blob_len(b.head));

	blobmsg_for_each_attr(cur, tb[IDX_DSL], rem) {
		hname = blobmsg_name(cur);
		if (!hname)
			continue;

		if (strcmp(hname, DSL_JSON_LVL2_STAT) == 0) {
			blobmsg_for_each_attr(tcur, cur, trem) {
				hname = blobmsg_name(tcur);
				if (!hname)
					continue;

				if (strcmp(hname, DSL_JSON_LVL3_LINE) == 0) {
					blobmsg_for_each_attr(t1cur, tcur, t1rem) {
						hname = blobmsg_name(t1cur);
						if (!hname)
							continue;

						LOGF_LOG_DEBUG("Line Number : [%s]\n", hname);
						blobmsg_for_each_attr(t2cur, t1cur, t2rem) {
							hname = blobmsg_name(t2cur);
							if (!hname)
								continue;

							if (strcmp(hname, DSL_JSON_LVL4_STAT) == 0) {
								status = blobmsg_get_string(t2cur);

								if (strcmp(status, "UP") != 0) {
									/* If status is not UP then check other line */
									break;
								}
							}
							else if (strcmp(hname, DSL_JSON_LVL4_CHANL) == 0) {
								blobmsg_for_each_attr(t3cur, t2cur, t3rem) {
									hname = blobmsg_name(t3cur);
									if (!hname)
										continue;

									if (strcmp(hname, DSL_JSON_LVL5_CUR_RATE) == 0) {
										blobmsg_for_each_attr(t4cur, t3cur, t4rem) {
											hname = blobmsg_name(t4cur);
											if (!hname)
												continue;

											if (strcmp(hname, DSL_JSON_LVL6_US_DIR) == 0) {
												linerate = 0;
												linerate = blobmsg_get_u32(t4cur);
												LOGF_LOG_DEBUG("Line rate : [%u]\n", linerate);
												rate += linerate;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

exit_lbl:
	if (rate != 0)
		rate = rate / 1000;

	LOGF_LOG_DEBUG("Exit with rate:[%u] kbps\n", rate);
	return rate;
}

/* =============================================================================
 *  Function Name : get_sysfs_rate
 *  Description   : Parse sysfs entry to get rate
 * ============================================================================*/
static uint32_t get_sysfs_rate(char *ifname) {
	ENTRY
	uint32_t rate = 0;
	char rate_path[MAX_STR_LEN] = {0};
	char sBuf[MAX_STR_LEN] = {0};
	char *end = NULL;
	FILE *fp = NULL;

	if (ifname == NULL) {
		LOGF_LOG_ERROR("Interface is invalid\n");
		goto exit_lbl;
	}
	LOGF_LOG_DEBUG("Check interface for : [%s]\n", ifname);

	snprintf(rate_path, MAX_STR_LEN, "%s%s/speed", RATE_SYSFS_PATH, ifname);

	fp = fopen(rate_path, "rx");
	if (fp == NULL) {
		LOGF_LOG_ERROR("Unable to open file\n");
		goto exit_lbl;
	}

	if (fread(sBuf, sizeof(char), sizeof(sBuf), fp) <= 0) {
		LOGF_LOG_ERROR("Unable to Read from file\n");
		fclose(fp);
		goto exit_lbl;
	}

	rate = (uint32_t)strtoul(sBuf, &end, 10);
	if (rate == 0 || rate > (MAX_IF_RATE/1000)) {
		LOGF_LOG_ERROR("Invalid Interface Rate!\n");
		fclose(fp);
		goto exit_lbl;
	}
	rate *= 1000;
	fclose(fp);

exit_lbl:
	LOGF_LOG_DEBUG("Rate:[%u]\n", rate);
	EXIT
	return rate;
}

/* =============================================================================
 *  Function Name : sysapi_iface_rate_get
 *  Description   : Returns port/interface/link rate of specified interface category
 *  			for xrx500 family models.
 * ============================================================================*/
int32_t sysapi_iface_rate_get(char *ifname, iftype_t iftype, uint32_t *rate)
{
	uint32_t bw_ratio = 100, new_rate = 0;
	GSW_portLinkCfg_t port_cfg;

	*rate = 0;

	switch(iftype) {
		case QOS_IF_CATEGORY_ETHLAN:
		case QOS_IF_CATEGORY_ETHWAN:
		case QOS_IF_CATEGORY_PTMWAN:
		case QOS_IF_CATEGORY_LTEWAN:
		case QOS_IF_CATEGORY_LOCAL:
		case QOS_IF_CATEGORY_WLANDP:
		case QOS_IF_CATEGORY_WLANNDP:
			bw_ratio = 100;
			break;
		case QOS_IF_CATEGORY_ATMWAN:
			bw_ratio = 80;
			break;
		default:
			break;
	}

	if ( iftype == QOS_IF_CATEGORY_ETHLAN) {
		if (strcmp(ifname, BRIDGE_NAME) == 0) {
			*rate = DEF_ETH_LAN_RATE;
			goto found_rate;
		}
		else {
			*rate = get_sysfs_rate(ifname);
		}
	}
	else if ( iftype == QOS_IF_CATEGORY_ETHWAN) {
		if (access(ETH0_5_DIR_PATH, F_OK) == LTQ_SUCCESS) {
			memset_s(&port_cfg, sizeof(port_cfg), 0);
			port_cfg.nPortId = AX_F24S_PORT;
			qos_call_ioctl(SWITCH_DEV_NODE_2, GSW_PORT_LINK_CFG_GET, &port_cfg);
			LOGF_LOG_DEBUG("nPortId:[%d] eSpeed:[%d]\n", port_cfg.nPortId, port_cfg.eSpeed);
			*rate = port_cfg.eSpeed * 1000;
		}
		else {
			*rate = get_sysfs_rate("eth1");
		}
	}
	else if ( iftype == QOS_IF_CATEGORY_PTMWAN || iftype == QOS_IF_CATEGORY_ATMWAN ) {
		*rate = read_dsl_rate();
	}
	/* set default rates in case actual rate could not be fectched.. */
	if(*rate == 0) {
		switch(iftype) {
			case QOS_IF_CATEGORY_ETHLAN:
				*rate = DEF_ETH_LAN_RATE;
				break;
			case QOS_IF_CATEGORY_ETHWAN:
				*rate = DEF_ETH_WAN_RATE;
				break;
			case QOS_IF_CATEGORY_PTMWAN:
				*rate = DEF_PTM_WAN_RATE;
				break;
			case QOS_IF_CATEGORY_LTEWAN:
				*rate = DEF_LTE_WAN_RATE;
				break;
			case QOS_IF_CATEGORY_ATMWAN:
				*rate = DEF_ATM_WAN_RATE;
				break;
			case QOS_IF_CATEGORY_WLANDP:
				*rate = DEF_WLAN_RATE;
				break;
			case QOS_IF_CATEGORY_WLANNDP:
				*rate = DEF_WLAN_RATE;
				break;
			case QOS_IF_CATEGORY_LOCAL:
				*rate = DEF_LOCAL_RATE;
				break;
			default:
				break;
		}
	}

found_rate:
	if (*rate == 0 || *rate > MAX_IF_RATE)
		return LTQ_FAIL;

	new_rate = (((*rate) * bw_ratio)/100);
	*rate = new_rate;
	LOGF_LOG_DEBUG("Rate : [%d]\n", *rate);

	return LTQ_SUCCESS;

}

/* =============================================================================
 *  Function Name : sysapi_set_ppefp
 *  Description   : API to enable/disable specific dapatahs for given criteria.
		    No PPA FAPI/API is available as of now. So we use ppacmd command
		    to achieve required action.
		    Also there is no support to enable/disable specific datapath for
		    specific criteria.
		    Hence fastpath is enabled/disabled as of now.
 * ============================================================================*/
int32_t sysapi_set_ppefp(char *ifname, iftype_t iftype, uint32_t enable)
{
	UNUSED(iftype);
	UNUSED(ifname);
	PPA_CMD_PPA_FASTPATH_ENABLE_INFO fp_info;

	memset_s(&fp_info, sizeof(PPA_CMD_PPA_FASTPATH_ENABLE_INFO), 0x00);

	fp_info.ppe_fastpath_enable = enable;
	qos_call_ioctl(PPA_DEVICE,PPA_CMD_SET_PPA_FASTPATH_ENABLE, &fp_info);

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sysapi_set_pkts_redirection
 *  Description   : API to add/delete redirection of packets from one interface 
 										to other interface (IFB), caller should make sure that from 
										interface is base interface else redirection may not work 
										for software accelerated paths.
 * ============================================================================*/
int32_t sysapi_set_pkts_redirection(char *from, char *to, uint32_t enable)
{
	char cmd[MAX_DATA_LEN] = {0};

	/* Delete redirection rule */
	snprintf(cmd, sizeof(cmd), "tc qdisc del dev %s root;", from);
	RUN_CMD(cmd);

	/* Add redirection rule */
	if (enable == QOSAL_TRUE) {
		snprintf(cmd, sizeof(cmd), "tc qdisc add dev %s root handle 1: htb;", from);
		RUN_CMD(cmd);

		snprintf(cmd, sizeof(cmd), "ifconfig %s up;", to);
		RUN_CMD(cmd);

		snprintf(cmd, sizeof(cmd), "tc filter add dev %s parent 1: protocol all u32 match u32 0 0 action mirred egress redirect dev %s;", from, to);
		RUN_CMD(cmd);
	}

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : vlanPrioUpdate
 *  Description   : manage and track vlan priority map for interface sIface
 *			for add/delete operation,
 *			case 1 - when no vlan priority filter is specified.
 *				map specified vlan priority (out8021p) for all possible
 *				vlan priority filter value (1-8)
 *			case 2 - when vlan priority filter is specified.
 *				map specified vlan priority (out8021p) for specified
 *				vlan priority filter value
 *			case 2 is given precendence over case 1.
 * ============================================================================*/
int32_t vlanPrioUpdate(char *sIface, int32_t inPrio, int32_t mapPrio, uint32_t flags)
{
	int32_t nCnt = 0, nCnt2 = 0, nFound = 0, nInitVal = 0, nRet = LTQ_SUCCESS;

	for(nCnt=0; nCnt < nMapInUse; nCnt++) {
		if(strcmp(xVlanMap[nCnt].sIf, sIface) == 0) {
			nFound = 1;
			break;
		}
	}
	if(flags & CL_ADD) {
		if(nFound == 1) {
			/* entry is already present for specified interface */
			if(inPrio > 0) {
				/* update only specific mapping */
				if(flags & CPU_CALLER) {
					xVlanMap[nCnt].nVlanPrioMap[DEFAULT_TC - inPrio] = mapPrio;
				}
				else if(flags & PPA_CALLER) {
					xVlanMap[nCnt].nVlanPrioPPAMap[MAX_TC_NUM - inPrio] = mapPrio;
				}
			}
			else {
				/* set entire map of this interface with default vlan priority
				 * wherever no specific mapping is present
				 */
				for(nCnt2=0; nCnt2 < DEFAULT_TC; nCnt2++) {
					if(flags & CPU_CALLER) {
						if(xVlanMap[nCnt].nVlanPrioMap[nCnt2] == -1)
							xVlanMap[nCnt].nVlanPrioMap[nCnt2] = mapPrio;
					}
					else if(flags & PPA_CALLER) {
						if(xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] == -1)
							xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] = mapPrio;
					}
				}
				xVlanMap[nCnt].defIfVlanPrio = mapPrio;
			}
			nDefVlanPrioCnt++;
		}
		else {
			/* entry is not present for specified interface */
			if(nCnt == DEFAULT_TC) {
				/* no place to add new map */
				nRet = LTQ_FAIL;
				goto exit_lbl;
			}
			else {
				/* first initialize entire map with either default value or
				 * specified 1p value if valid. in case of former update
				 * specific map.
				 * Here caller will always be CPU so we are not checking for PAE entry
				 * and we set PAE mapping entry wth -1 value.
				 */
				nInitVal = (inPrio == 0)?mapPrio:DEF_VLAN_PRIO;
				for(nCnt2=0; nCnt2 < DEFAULT_TC; nCnt2++) {
					xVlanMap[nCnt].nVlanPrioMap[nCnt2] = nInitVal;
					xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] = DEF_VLAN_PRIO;
				}
				if(inPrio > 0) {
					if(flags & CPU_CALLER) {
						xVlanMap[nCnt].nVlanPrioMap[DEFAULT_TC - inPrio] = mapPrio;
					}
					else if(flags & PPA_CALLER) {
						xVlanMap[nCnt].nVlanPrioPPAMap[MAX_TC_NUM - inPrio] = mapPrio;
					}
				}
				else {
					xVlanMap[nCnt].defIfVlanPrio = mapPrio;
				}
				nDefVlanPrioCnt++;
				snprintf(xVlanMap[nCnt].sIf, MAX_IF_NAME_LEN, "%s", sIface);
			}
			nMapInUse++;
		}
		vlanPrioCfg(sIface);
	}
	else if(flags & CL_DEL) {
		if(nFound == 1) {
			if(inPrio > 0) {
				/* restore map for this specific vlan priority - either to
				 * default vlan priority for this interface or none
				 */
				if(flags & CPU_CALLER) {
					xVlanMap[nCnt].nVlanPrioMap[DEFAULT_TC - inPrio] = (xVlanMap[nCnt].defIfVlanPrio > -1)?(xVlanMap[nCnt].defIfVlanPrio):DEF_VLAN_PRIO;
				}
				else if(flags & PPA_CALLER) { 
					xVlanMap[nCnt].nVlanPrioPPAMap[MAX_TC_NUM - inPrio] = (xVlanMap[nCnt].defIfVlanPrio > -1)?(xVlanMap[nCnt].defIfVlanPrio):DEF_VLAN_PRIO;
				}
			}
			else {
				/* restore entire map for interface, retaining any specific maps if configured */
				for(nCnt2=0; nCnt2 < DEFAULT_TC; nCnt2++) {
					if(flags & CPU_CALLER) {
						if(xVlanMap[nCnt].nVlanPrioMap[nCnt2] == xVlanMap[nCnt].defIfVlanPrio)
							xVlanMap[nCnt].nVlanPrioMap[nCnt2] = DEF_VLAN_PRIO;
					}
					else if(flags & PPA_CALLER) {
						if(xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] == xVlanMap[nCnt].defIfVlanPrio) 
							xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] = DEF_VLAN_PRIO;
					}
				}
				xVlanMap[nCnt].defIfVlanPrio = DEF_VLAN_PRIO;
			}
			nDefVlanPrioCnt--;
			vlanPrioCfg(sIface);
			if(nDefVlanPrioCnt <= 0 )
				nMapInUse--;
		}
		else {
			/* fail. interface doesn't have map associated */
			nRet = LTQ_FAIL;
			goto exit_lbl;
		}
	}

exit_lbl:
	return nRet;
}

/* =============================================================================
 *  Function Name : vlanPrioCfg
 *  Description   : configure vlan priority in system for specified interface
 * ============================================================================*/
void vlanPrioCfg(char *sIface)
{
	int32_t nCnt = 0, nCnt2 = 0, nFound = 0;
	char sCmd[MAX_DATA_LEN] = {0};

	if(strnlen_s(sIface, MAX_IF_NAME_LEN) == 0) {
		goto lbl;
	}

	snprintf(sCmd, sizeof(sCmd), "/proc/%s", sIface);
	if(access(sCmd, F_OK | R_OK) != 0) {
		/* if interface is non-VLAN based, then nothing to do */
		goto lbl;
	}

	for(nCnt=0; nCnt < nMapInUse; nCnt++) {
		if(strcmp(xVlanMap[nCnt].sIf, sIface) == 0) {
			nFound = 1;
			break;
		}
	}
	if(nFound == 1) {
		for(nCnt2=0; nCnt2 < DEFAULT_TC; nCnt2++) {
			/* skb_prio is 1-8 and vlan_qos is 0-7 */
			if(xVlanMap[nCnt].nVlanPrioMap[nCnt2] != DEF_VLAN_PRIO) {
				snprintf(sCmd, sizeof(sCmd), "vconfig set_egress_map %s %d %d\n", sIface, nCnt2, xVlanMap[nCnt].nVlanPrioMap[nCnt2]);
				RUN_CMD(sCmd);
			}
			if(xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] != DEF_VLAN_PRIO) {
				snprintf(sCmd, sizeof(sCmd), "vconfig set_egress_map %s %d %d\n", sIface, nCnt2, xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2]);
				RUN_CMD(sCmd);
			}
			if((xVlanMap[nCnt].nVlanPrioMap[nCnt2] == DEF_VLAN_PRIO) && (xVlanMap[nCnt].nVlanPrioPPAMap[nCnt2] == DEF_VLAN_PRIO)) {
				snprintf(sCmd, sizeof(sCmd), "vconfig set_egress_map %s %d %d\n", sIface, nCnt2, 0);
				RUN_CMD(sCmd);
                        }
		}
		if(xVlanMap[nCnt].defIfVlanPrio > -1) {
			/* disable hardware fastpath to achieve interface based vlan priority marking.
			 * as in hardware fastpath, vlan priority is not learnt by PPA.
			 */
			sysapi_set_ppefp(NULL, 0, 0);
		}
		else {
			/* enable hardware fastpath if default vlan prio is not configured
			 * on any interface.
			 */
			if(nDefVlanPrioCnt <= 1) {
				/* need to skip if any specific remarking is configured */
				sysapi_set_ppefp(NULL, 0, 1);
			}
		}
	}
lbl:
	return;
}

/* =============================================================================
 *  Function Name : sysapi_no_of_lan_ports 
 *  Description   : returns no of lan ports depending upon lan port separation. 
 * ============================================================================*/
int32_t sysapi_no_of_lan_ports()
{
    return 4;
}

int32_t qos_call_ioctl(const char *dev_node, int ioctl_cmd, void *data)
{
	int32_t nRet = LTQ_SUCCESS;
	int fd  = 0;

	if (dev_node == NULL) {
		LOGF_LOG_ERROR("Invalid Device Node!\n");
		return LTQ_FAIL;
	}

	if ((fd = open (dev_node, O_RDWR)) < LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Unable to open device node!\n");
		nRet = LTQ_FAIL;
	}
	else {
		if (ioctl (fd, ioctl_cmd, data) < LTQ_SUCCESS) {
			LOGF_LOG_ERROR("Ioctl failed!\n");
			nRet = LTQ_FAIL;
		}
		close (fd);
	}

	return nRet;
}
