/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*  *****************************************************************************
 *         File Name    : dwpal.c                             	                *
 *         Description  : D-WPAL control interface 		                        *
 *                                                                              *
 *  *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <linux/types.h>
#include <libnl3/netlink/socket.h>
#include <libnl3/netlink/genl/ctrl.h>
#include <linux/netlink.h>

#include <net/if.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#include "dwpal.h"
#include "dwpal_log.h"	//Logging

/* list of banned SDL methods */
#define strlen(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strtok(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcat(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncat(...) 	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcpy(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncpy(...) 	SDL_BANNED_FUNCTION ERROR_TOKEN
#define snprintf(...)	SDL_BANNED_FUNCTION ERROR_TOKEN

#define DWPAL_MAX_NUM_OF_ELEMENTS 512

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#ifndef NETLINK_EXT_ACK
#define NETLINK_EXT_ACK 11
#endif

#define OUI_LTQ 0xAC9A96

#if IS_RANDOMISE_SUPPORTED_BY_DRIVER  // randomise is NOT supported by the Driver
#define ETH_ALEN 6
#endif

#define MAX_NL_SERVICE_NAME_LEN 16

enum nlServiceEnum
{
	NL_SERVICE_VENDOR,
	NL_SERVICE_SCAN
};

typedef struct {
	enum nlServiceEnum serviceEnum; 
	char serviceName[MAX_NL_SERVICE_NAME_LEN];
}nlService_t;

nlService_t gNlServices[] = { { NL_SERVICE_VENDOR, "vendor"} ,
                            { NL_SERVICE_SCAN, "scan" }
                            }; // Add more service as in when required

typedef struct
{
	union
	{
		struct
		{
			char   VAPName[DWPAL_VAP_NAME_STRING_LENGTH]; /* "wlan0", "wlan0.1", "wlan1", "wlan2.2", ..., "wlan5", ... */
			char   operationMode[DWPAL_OPERATING_MODE_STRING_LENGTH];
			char   wpaCtrlName[DWPAL_WPA_CTRL_STRING_LENGTH];
			struct wpa_ctrl *wpaCtrlPtr;
			struct wpa_ctrl *listenerWpaCtrlPtr;   /*needed when closing it*/
			int    fd;
			DWPAL_wpaCtrlEventCallback wpaCtrlEventCallback;  /* callback function for hostapd received events while command is being sent; can be NULL */
		} hostapd;

		struct
		{
			struct nl_sock *nlSocketEvent, *nlSocketCmdGet;
			int    fd, fdCmdGet, nl80211_id;
			DWPAL_nlVendorEventCallback nlEventCallback, nlCmdGetCallback;
			DWPAL_nlNonVendorEventCallback nlNonVendorEventCallback;
		} driver;
	} interface;
} DWPAL_Context;


/* Local static functions */

static int no_seq_check(struct nl_msg *msg, void *arg)
{
	(void)msg;
	(void)arg;
	return NL_OK;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
			 void *arg)
{
	(void)nla;
	if (!err)
		return NL_STOP;
	console_printf(" ERROR: %s\n", strerror(-(err->error)));
	if (arg)
		*(int *)arg = err->error;
	return NL_STOP;
}

/* Last message in a series of multi part messages received 
   eg: scan dump reply is multi part messages
   to notify last message received, finish callback will be called*/

static int finish(struct nl_msg *msg, void *arg)
{
	DWPAL_Context     *localContext = (DWPAL_Context *)(arg);
	char              ifname[IF_NAMESIZE] = "\0";
	struct nlattr     *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));

	nla_parse(tb,
			  NL80211_ATTR_MAX,
			  genlmsg_attrdata(gnlh, 0),
			  genlmsg_attrlen(gnlh, 0),
			  NULL);

	if (tb[NL80211_ATTR_IFINDEX] != NULL)
	{
		if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ifname);
	}

	/* notify application that finish is been recieved */
	if (localContext->interface.driver.nlNonVendorEventCallback != NULL)
	{
		localContext->interface.driver.nlNonVendorEventCallback(msg);
	}
	return NL_OK;
}


static int ack_recv(struct nl_msg *msg, void *arg)
{
	(void)msg;
	if (arg)
		*(int *)arg = 0;
	return NL_STOP; // ack received, no more processing
}


static DWPAL_Ret nlInternalNlCallback(DWPAL_NlEventType nlEventType, struct nl_msg *msg, void *arg)
{
	DWPAL_Context     *localContext = (DWPAL_Context *)(arg);
	struct nlattr     *attr;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr     *tb[NL80211_ATTR_MAX + 1];
	unsigned char     *data;
	int               len, vendor_subcmd = -1;
	char              ifname[DWPAL_VAP_NAME_STRING_LENGTH] = "\0";

	console_printf("%s Entry; nlEventType= %d (DWPAL_NL_UNSOLICITED_EVENT=0, DWPAL_NL_SOLICITED_EVENT=1)\n", __FUNCTION__, nlEventType);

	nla_parse(tb,
			  NL80211_ATTR_MAX,
			  genlmsg_attrdata(gnlh, 0),
			  genlmsg_attrlen(gnlh, 0),
			  NULL);

	attr = nla_find(genlmsg_attrdata(gnlh, 0),
					genlmsg_attrlen(gnlh, 0),
					NL80211_ATTR_VENDOR_DATA);

	if (!attr)
	{  /* Handle a non-vendor event */
		console_printf("%s; vendor data attribute missing ==> non Vendor command\n", __FUNCTION__);

		/* Call the NL non-Vendor callback function */
		if (localContext->interface.driver.nlNonVendorEventCallback != NULL)
		{
			localContext->interface.driver.nlNonVendorEventCallback(msg);
		}
	}
	else
	{  /* Handle a vendor event */
		if ( (nlEventType == DWPAL_NL_UNSOLICITED_EVENT) && (localContext->interface.driver.nlEventCallback == NULL) )
		{
			console_printf("%s; 'DWPAL_NL_UNSOLICITED_EVENT' and nlEventCallback=NULL ==> exit\n", __FUNCTION__);
			return DWPAL_SUCCESS;
		}

		data = (unsigned char *)nla_data(attr);
		len = nla_len(attr);

		if ( (gnlh->cmd == NL80211_CMD_VENDOR) && (tb[NL80211_ATTR_VENDOR_SUBCMD] != NULL) )
		{
			vendor_subcmd = nla_get_u32(tb[NL80211_ATTR_VENDOR_SUBCMD]);
		}

		if (tb[NL80211_ATTR_IFINDEX] != NULL)
		{
			if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ifname);
		}

		if (nlEventType == DWPAL_NL_SOLICITED_EVENT)
		{
			/* Call the NL 'get command' callback function */
			if (localContext->interface.driver.nlCmdGetCallback(ifname, gnlh->cmd, vendor_subcmd, (size_t)len, data) == DWPAL_FAILURE)
			{
				console_printf("%s; nlCmdGetCallback failed ==> cont...\n", __FUNCTION__);
			}
		}
		else if (nlEventType == DWPAL_NL_UNSOLICITED_EVENT)
		{
			/* Call the NL callback function */
			localContext->interface.driver.nlEventCallback(ifname, gnlh->cmd, vendor_subcmd, (size_t)len, data);
		}
		else
		{
			console_printf("%s; invalid nlEventType (%d) ==> Abort!\n", __FUNCTION__, nlEventType);
			return DWPAL_FAILURE;
		}
	}

	return DWPAL_SUCCESS;
}


static int nlInternalCmdGetCallback(struct nl_msg *msg, void *arg)
{
	console_printf("%s Entry\n", __FUNCTION__);

	if (nlInternalNlCallback(DWPAL_NL_SOLICITED_EVENT, msg, arg) == DWPAL_FAILURE)
	{
		console_printf("%s; nlInternalNlCallback ERROR ==> Abort!\n", __FUNCTION__);
		return (int)DWPAL_FAILURE;
	}

	return (int)DWPAL_SUCCESS;
}


static int nlInternalEventCallback(struct nl_msg *msg, void *arg)
{
	console_printf("%s Entry\n", __FUNCTION__);

	if (nlInternalNlCallback(DWPAL_NL_UNSOLICITED_EVENT, msg, arg) == DWPAL_FAILURE)
	{
		console_printf("%s; nlInternalNlCallback ERROR ==> Abort!\n", __FUNCTION__);
		return (int)DWPAL_FAILURE;
	}

	return (int)DWPAL_SUCCESS;
}


static DWPAL_Ret parse_hex(char *hexmask, unsigned char *result, size_t *outlen)
{
	int pos = 0;

	*outlen = 0;

	while (1)
	{
		char *cp = strchr(hexmask, ':');
		if (cp)
		{
			*cp = 0;
			cp++;
		}

		if ( (strcmp(hexmask, "xx") == 0) || (strcmp(hexmask, "--") == 0) )
		{
		}
		else
		{
			int  temp;
			char *end;

			temp = strtoul(hexmask, &end, 16);
			if ( (*end) || ((temp < 0) || (temp > 255)) )
			{
				return DWPAL_FAILURE;
			}

			result[pos] = temp;
		}

		(*outlen)++;
		pos++;

		if (!cp)
		{
			break;
		}

		hexmask = cp;
	}

	return DWPAL_SUCCESS;
}


#if IS_RANDOMISE_SUPPORTED_BY_DRIVER  // randomise is NOT supported by the Driver
static int mac_addr_a2n(unsigned char *mac_addr, char *arg)
{
	int i;

	for (i = 0; i < ETH_ALEN ; i++) {
		unsigned int temp;
		char *cp = strchr(arg, ':');
		if (cp) {
			*cp = 0;
			cp++;
		}
		if (sscanf(arg, "%x", &temp) != 1)
			return -1;
		if (temp > 255)
			return -1;

		mac_addr[i] = temp;
		if (!cp)
			break;
		arg = cp;
	}
	if (i < ETH_ALEN - 1)
		return -1;

	return 0;
}


static int parse_random_mac_addr(struct nl_msg *msg, char *arg)
{
	char *a_addr, *a_mask, *sep;
	unsigned char addr[ETH_ALEN], mask[ETH_ALEN];
	char *addrs = arg + 9;

	if (*addrs != '=')
		return 0;

	addrs++;
	sep = strchr(addrs, '/');
	a_addr = addrs;

	if (!sep)
		return 1;

	*sep = 0;
	a_mask = sep + 1;
	if (mac_addr_a2n(addr, a_addr) || mac_addr_a2n(mask, a_mask))
		return 1;

	NLA_PUT(msg, NL80211_ATTR_MAC, ETH_ALEN, addr);
	NLA_PUT(msg, NL80211_ATTR_MAC_MASK, ETH_ALEN, mask);

	return 0;
 nla_put_failure:
	return -ENOBUFS;
}
#endif


static DWPAL_Ret scanParamHandle(struct nl_msg *msg, ScanParams *scanParams)
{
	// Take logic from handle_scan()

	bool          isFrequencies = false, isSSIDs = false;
	int           i, nlaIdx=0, res;
	struct nl_msg *frequencies = NULL, *ssids = NULL;
	unsigned char *ies = NULL, *meshid = NULL, *tmpies = NULL;
	size_t        len, ies_len = 0, meshid_len = 0;
	unsigned int  flags = 0;

	if ( (msg == NULL) || (scanParams == NULL) )
	{
		console_printf("%s; msg and/or scanParams is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Handle lowpri, flush & ap_force */
	if (scanParams->lowpri)
	{
		flags |= NL80211_SCAN_FLAG_LOW_PRIORITY;
	}

	if (scanParams->flush)
	{
		flags |= NL80211_SCAN_FLAG_FLUSH;
	}

	if (scanParams->ap_force)
	{
		flags |= NL80211_SCAN_FLAG_AP;
	}

#if IS_RANDOMISE_SUPPORTED_BY_DRIVER  // randomise is NOT supported by the Driver
	/* Handle randomise */
	if (scanParams->randomise[0] != '\0')
	{
		flags |= NL80211_SCAN_FLAG_RANDOM_ADDR;
		res = parse_random_mac_addr(msg, scanParams->randomise);
		if (res != 0)
		{
			console_printf("%s; parse_random_mac_addr error ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
		}
	}
#endif

	/* Handle ies */
	if (scanParams->ies[0] != '\0')
	{
		console_printf("%s; ies= '%s'\n", __FUNCTION__, scanParams->ies);
		len = strnlen_s(scanParams->ies, SCAN_PARAM_STRING_LENGTH) / 2;

		ies = calloc(len + 2, 1);
		if (ies == NULL)
		{
			console_printf("%s; ies malloc failed ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
		}

		if (parse_hex(scanParams->ies, ies, &ies_len) == DWPAL_FAILURE)
		{
			console_printf("%s; parse_hex error ==> Abort!\n", __FUNCTION__);
			free((void *)ies);
			return DWPAL_FAILURE;
		}
	}

	/* Handle meshid */
	if (scanParams->meshid[0] != '\0')
	{
		console_printf("%s; meshid= '%s'\n", __FUNCTION__, scanParams->meshid);
		meshid_len = strnlen_s(scanParams->meshid, SCAN_PARAM_STRING_LENGTH);
		console_printf("%s; meshid_len= %d\n", __FUNCTION__, meshid_len);
		meshid = (unsigned char *)malloc(meshid_len + 2);
		if (meshid == NULL)
		{
			console_printf("%s; ies malloc failed ==> Abort!\n", __FUNCTION__);
			free((void *)ies);
			return DWPAL_FAILURE;
		}
		meshid[0] = 114; /* mesh element id */
		meshid[1] = meshid_len;
		memcpy_s(&meshid[2], meshid_len, scanParams->meshid, meshid_len);
		meshid_len += 2;
	}

	/* Handle ies + meshid */
	if ( (ies != NULL) || (meshid != NULL) )
	{
		tmpies = (unsigned char *)malloc(ies_len + meshid_len);
		if (tmpies == NULL)
		{
			console_printf("%s; tmpies malloc failed ==> Abort!\n", __FUNCTION__);
			free((void *)ies);
			free((void *)meshid);
			return DWPAL_FAILURE;
		}

		if (ies != NULL)
		{
			memcpy_s(tmpies, ies_len, ies, ies_len);
		}

		if (meshid != NULL)
		{
			memcpy_s(&tmpies[ies_len], meshid_len, meshid, meshid_len);
		}

		if (ies != NULL)
		{
			free((void *)ies);
		}

		if (meshid != NULL)
		{
			free((void *)meshid);
		}

		if (nla_put(msg, NL80211_ATTR_IE, ies_len + meshid_len, tmpies) < 0)
		{
			console_printf("%s; nla_put NL80211_ATTR_IE failed ==> Abort!\n", __FUNCTION__);
			free((void *)tmpies);
			return DWPAL_FAILURE;
		}
		
		free((void *)tmpies);
	}

	/* Handle SSIDs */
	ssids = nlmsg_alloc();
	if (ssids == NULL)
	{
		console_printf("%s; ssids nlmsg_alloc failed ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	for (i=0; i < NUM_OF_SSIDS; i++)
	{
		if (scanParams->ssid[i][0] != '\0')
		{
			console_printf("%s; ssid[%d]= '%s'; nlaIdx= %d\n", __FUNCTION__, i, scanParams->ssid[i], nlaIdx);
			isSSIDs = true;
			if (nla_put(ssids, nlaIdx, strnlen_s(scanParams->ssid[i], SSID_STRING_LENGTH), scanParams->ssid[i]) < 0)
			//if (nla_put(ssids, i, strnlen_s(scanParams->ssid[i], SSID_STRING_LENGTH), scanParams->ssid[i]) < 0)
			{
				console_printf("%s; nla_put SSIDs failed ==> Abort!\n", __FUNCTION__);
				nlmsg_free(ssids);
				return DWPAL_FAILURE;
			}

			nlaIdx++;
		}
		else
		{
			break;
		}
	}

	if (isSSIDs == false)
	{
		console_printf("%s; do not set SSIDs, clear it\n", __FUNCTION__);
		if (nla_put(ssids, 1, 0, "") < 0)
		{
			console_printf("%s; nla_put clear SSIDs failed ==> Abort!\n", __FUNCTION__);
			nlmsg_free(ssids);
			return DWPAL_FAILURE;
		}
	}

	if (scanParams->passive == false)
	{
		console_printf("%s; passive is false ==> set SSIDs\n", __FUNCTION__);
		nla_put_nested(msg, NL80211_ATTR_SCAN_SSIDS, ssids);
	}

	nlmsg_free(ssids);

	/* Handle Frequencies */
	frequencies = nlmsg_alloc();
	if (frequencies == NULL)
	{
		console_printf("%s; frequencies nlmsg_alloc failed ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	for (i=0; i < NUM_OF_FREQUENCIES; i++)
	{
		if (scanParams->freq[i] > 0)
		{
			console_printf("%s; freq[%d]= %d, nlaIdx= %d\n", __FUNCTION__, i, scanParams->freq[i], nlaIdx);
			isFrequencies = true;
			res = nla_put_u32(frequencies, nlaIdx, scanParams->freq[i]);
			//res = nla_put_u32(frequencies, i, scanParams->freq[i]);
			//NLA_PUT_U32(frequencies, i, scanParams->freq[i]);
			if (res < 0)
			{
				console_printf("%s; building frequencies message failed ==> Abort!\n", __FUNCTION__);
				nlmsg_free(frequencies);
				return DWPAL_FAILURE;
			}

			nlaIdx++;
		}
		else
		{
			break;
		}
	}

	if (isFrequencies)
	{
		console_printf("%s; set frequencies\n", __FUNCTION__);
		nla_put_nested(msg, NL80211_ATTR_SCAN_FREQUENCIES, frequencies);
	}

	nlmsg_free(frequencies);

	/* In case that flags are present, update msg with it */
	if (flags != 0)
	{
		console_printf("%s; set flags (0x%x)\n", __FUNCTION__, (unsigned int)flags);
		res = nla_put_u32(msg, NL80211_ATTR_SCAN_FLAGS, flags);
		//NLA_PUT_U32(msg, NL80211_ATTR_SCAN_FLAGS, flags);
		if (res < 0)
		{
			console_printf("%s; building message NL80211_ATTR_SCAN_FLAGS failed ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
		}
	}

	return DWPAL_SUCCESS;
}


static DWPAL_Ret scanPerform(void *context, char *ifname, enum nl80211_commands nl80211Command, int flags, DWPAL_nlNonVendorEventCallback nlEventCallback, ScanParams *scanParams)
{
	int              res;
	struct nl_msg    *msg;
	DWPAL_Context    *localContext = (DWPAL_Context *)(context);
	signed long long devidx = 0;

	console_printf("%s Entry; ifname= '%s'\n", __FUNCTION__, ifname);

	if (localContext == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if ( (nl80211Command == NL80211_CMD_GET_SCAN /*0x20*/) && (nlEventCallback == NULL) )
	{
		console_printf("%s; cmd=NL80211_CMD_GET_SCAN, nlEventCallback is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (nlEventCallback != NULL)
		localContext->interface.driver.nlNonVendorEventCallback = nlEventCallback;

	if (localContext->interface.driver.nlSocketEvent == NULL)
	{
		console_printf("%s; nlSocket is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	msg = nlmsg_alloc();
	if (msg == NULL)
	{
		console_printf("%s; nlmsg_alloc returned NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	console_printf("%s; nl80211_id= %d\n", __FUNCTION__, localContext->interface.driver.nl80211_id);

	/* calling genlmsg_put() is a must! without it, the callback won't be called! */
	genlmsg_put(msg, 0, 0, localContext->interface.driver.nl80211_id, 0, flags, nl80211Command, 0);

	devidx = if_nametoindex(ifname);
	if (devidx < 0)
	{
		console_printf("%s; devidx ERROR (devidx= %lld) ==> Abort!\n", __FUNCTION__, devidx);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	res = nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);  /* DWPAL_NETDEV_ID */
	//NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);
	if (res < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	if ( (nl80211Command == NL80211_CMD_TRIGGER_SCAN /*0x21*/) && (scanParams != NULL) )
	{
		if (scanParamHandle(msg, scanParams) == DWPAL_FAILURE)
		{
			console_printf("%s; scanParamHandle returned error ==> Abort!\n", __FUNCTION__);
			nlmsg_free(msg);
			return DWPAL_FAILURE;
		}
	}

	/* will trigger nlEventCallback() function call */
	res = nl_send_auto(localContext->interface.driver.nlSocketEvent, msg);  // can use nl_send_auto_complete(nlSocket, msg) instead
	if (res < 0)
	{
		console_printf("%s; nl_send_auto returned ERROR (res= %d) ==> Abort!\n", __FUNCTION__, res);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	nlmsg_free(msg);

	return DWPAL_SUCCESS;
}


static DWPAL_Ret nlSocketCreate(struct nl_sock **nlSocket, int *fd)
{
	int res = 1;

	*nlSocket = nl_socket_alloc();
	if (*nlSocket == NULL)
	{
		console_printf("%s; nl_socket_alloc ERROR ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Connect to generic netlink socket on kernel side */
	if (genl_connect(*nlSocket) < 0)
	{
		console_printf("%s; genl_connect ERROR ==> Abort!\n", __FUNCTION__);
		nl_socket_free(*nlSocket);
		return DWPAL_FAILURE;
	}

	if (nl_socket_set_buffer_size(*nlSocket, DRIVER_NL_TO_DWPAL_MSG_LENGTH, DRIVER_NL_TO_DWPAL_MSG_LENGTH) != 0)
	{
		console_printf("%s; nl_socket_set_buffer_size ERROR ==> Abort!\n", __FUNCTION__);
		nl_socket_free(*nlSocket);
		return DWPAL_FAILURE;
	}

	*fd = nl_socket_get_fd(*nlSocket);
	if (*fd == -1)
	{
		console_printf("%s; nl_socket_get_fd ERROR ==> Abort!\n", __FUNCTION__);
		nl_socket_free(*nlSocket);
		return DWPAL_FAILURE;
	}
	console_printf("%s; driver.fd= %d\n", __FUNCTION__, *fd);

	/* manipulate options for the socket referred to by the file descriptor - driver.fd */
	setsockopt(*fd, SOL_NETLINK /*option level argument*/,
		   NETLINK_EXT_ACK, &res, sizeof(res));

	return DWPAL_SUCCESS;
}


static bool mandatoryFieldValueGet(char *buf, size_t *bufLen, char **p2str, int totalSizeOfArg, char fieldValue[] /*OUT*/)
{
	char *param = strtok_s(buf, bufLen, " ", p2str);

	if (param == NULL)
	{
		console_printf("%s; param is NULL ==> Abort!\n", __FUNCTION__);
		return false;
	}

	if (fieldValue != NULL)
	{
		if (strnlen_s(param, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) > (size_t)(totalSizeOfArg - 1))
		{
			console_printf("%s; param ('%s') length (%d) is higher than allocated size (%d) ==> Abort!\n", __FUNCTION__, param, strnlen_s(param, totalSizeOfArg), totalSizeOfArg-1);
			return false;
		}

		strcpy_s(fieldValue, strnlen_s(param, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1, param);
	}

	return true;
}


static bool arrayValuesGet(char *stringOfValues, size_t totalSizeOfArg, ParamParsingType paramParsingType, size_t *numOfValidArgs /*OUT*/, void *array /*OUT*/)
{
	/* fill in the output array with list of integer elements (from decimal/hex base), for example:
	   "SupportedRates=2 4 11 22 12 18 24 36 48 72 96 108" or "HT_MCS=FF FF FF 00 00 00 00 00 00 00 C2 01 01 00 00 00"
	   also, in case of "DWPAL_STR_ARRAY_PARAM", handle multiple repetitive field, for example:
	   "... non_pref_chan=81:200:1:5 non_pref_chan=81:100:2:9 non_pref_chan=81:200:1:7 non_pref_chan=81:100:2:5 ..." or
	   "... non_pref_chan=81:200:1:5 81:100:2:9 81:200:1:7 81:100:2:5 ..." */

	int     idx = 0;
	char    *p2str, *param, *tokenString;
	rsize_t dmaxLen = strnlen_s(stringOfValues, DWPAL_TO_HOSTAPD_MSG_LENGTH);

	tokenString = stringOfValues;

	do
	{
		param = strtok_s(tokenString, &dmaxLen, " ", &p2str);
		if (param == NULL)
		{
			((int *)array)[idx] = 0;
			break;
		}

		if (idx < (int)totalSizeOfArg)
		{
			if (numOfValidArgs != NULL)
			{
				(*numOfValidArgs)++;
			}

			if (paramParsingType == DWPAL_INT_HEX_ARRAY_PARAM)
			{
				((int *)array)[idx] = strtol(param, NULL, 16);
			}
			else if (paramParsingType == DWPAL_INT_ARRAY_PARAM)
			{
				((int *)array)[idx] = atoi(param);
			}
			else if (paramParsingType == DWPAL_STR_ARRAY_PARAM)
			{
				strcpy_s(&(((char *)array)[idx * HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH]), strnlen_s(param, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1, param);
			}
		}

		tokenString = NULL;

		idx++;
	} while (idx < DWPAL_MAX_NUM_OF_ELEMENTS);  /* allow up to 512 elements per field (array) */

	if (idx >= (int)totalSizeOfArg)
	{
		console_printf("%s; actual number of arguments (%d) is bigger/equal then totalSizeOfArg (%d) ==> Abort!\n", __FUNCTION__, idx, totalSizeOfArg);
		return false;
	}

	return true;
}


static bool fieldValuesGet(char *buf, size_t bufLen, ParamParsingType parsingType, const char *stringToSearch, char *endFieldName[], char *stringOfValues /*OUT*/)
{
	/* handles list of fields, one by one in the same row, for example: "... btm_supported=1 ..." or
	   "... SupportedRates=2 4 11 22 12 18 24 36 48 72 96 108 ..." */

	char    *stringStart, *stringEnd, *restOfStringStart, *closerStringEnd = NULL;
	char    *localBuf = NULL;
	char    *localStringToSearch = NULL;
	int     i, idx=0, numOfCharacters = 0, numOfCharactersToCopy = 0;
	bool    isFirstEndOfString = true, ret = false;
	char    *tempStringOfValues=NULL, *localEndFieldName=NULL;
	int		status;

	tempStringOfValues = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (tempStringOfValues == NULL)
	{
		console_printf("%s; malloc failed ==> Abort!\n", __FUNCTION__);
		return false;
	}

	localEndFieldName = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (localEndFieldName == NULL)
	{
		free((void *)tempStringOfValues);
		console_printf("%s; malloc failed ==> Abort!\n", __FUNCTION__);
		return false;
	}

	localBuf = (char *)malloc(bufLen + 2 /* '\0' & 'blank' */);
	if (localBuf == NULL)
	{
		console_printf("%s; malloc failed ==> Abort!\n", __FUNCTION__);
		free((void *)tempStringOfValues);
		free((void *)localEndFieldName);
		return false;
	}

	/* Add ' ' at the beginning of a string - to handle a case in which the buf starts with the
	   value of stringToSearch, like buf= 'candidate=d8:fe:e3:3e:bd:14,2178,83,5,7,255 candidate=...' */
	status = sprintf_s(localBuf, bufLen + 2, " %s", buf);
	if (status <= 0)
	{
		console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
		goto err;
	}

	/* localStringToSearch set to stringToSearch with addition of " " at the beginning -
	   it is a MUST in order to differentiate between "ssid" and "bssid" */
	localStringToSearch = (char *)malloc(strnlen_s(stringToSearch, DWPAL_FIELD_NAME_LENGTH) + 2 /*'\0' & 'blank' */);
	if (localStringToSearch == NULL)
	{
		console_printf("%s; localStringToSearch is NULL ==> Abort!\n", __FUNCTION__);
		free((void *)localBuf);
		free((void *)tempStringOfValues);
		free((void *)localEndFieldName);
		return false;
	}

	status = sprintf_s(localStringToSearch, DWPAL_FIELD_NAME_LENGTH, " %s", stringToSearch);
	if (status <= 0)
	{
		console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
		goto err;
	}

	restOfStringStart = localBuf;

	while ( (stringStart = strstr(restOfStringStart, localStringToSearch)) != NULL )
	{
		ret = true;  /* mark that at least one fiels was found */

		/* move the string pointer to the beginning of the field's value */
		restOfStringStart = stringStart + strnlen_s(localStringToSearch, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
		//console_printf("%s; stringStart= 0x%x, strlen of ('%s')= %d ==> restOfStringStart= 0x%x\n",
			   //__FUNCTION__, (unsigned int)stringStart, localStringToSearch, strnlen_s(localStringToSearch, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH), (unsigned int)restOfStringStart);

		/* find all beginning of all other fields (and get the closest to the current field) in order to know where the field's value ends */
		i = 0;
		while (strncmp(endFieldName[i], "\n", 1))
		{  /* run over all field names in the string */
			status = sprintf_s(localEndFieldName, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, " %s", endFieldName[i]);  /* in order to differentiate between VHT_MCS and HT_MCS */
			if (status <= 0)
			{
				console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
				goto err;
			}
			stringEnd = strstr(restOfStringStart, localEndFieldName);
			if (stringEnd != NULL)
			{
				stringEnd++;  /* move one character ahead due to the ' ' at the beginning of localEndFieldName */
				//console_printf("%s; localEndFieldName= '%s' FOUND! (i= %d)\n", __FUNCTION__, localEndFieldName, i);
				if (isFirstEndOfString)
				{
					isFirstEndOfString = false;
					closerStringEnd = stringEnd;
				}
				else
				{  /* Make sure that closerStringEnd will point to the closest field ahead */
					closerStringEnd = (stringEnd < closerStringEnd)? stringEnd : closerStringEnd;
				}

				//console_printf("%s; inside the 'while': closerStringEnd= 0x%x\n", __FUNCTION__, (unsigned int)closerStringEnd);
			}

			i++;
		}

		//console_printf("%s; after the 'while': closerStringEnd= 0x%x\n", __FUNCTION__, (unsigned int)closerStringEnd);

		if (closerStringEnd == NULL)
		{  /* Meaning, this is the last parameter in the string */
			//console_printf("%s; closerStringEnd is NULL; restOfStringStart= '%s'\n", __FUNCTION__, restOfStringStart);
			closerStringEnd = restOfStringStart + strnlen_s(restOfStringStart, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1 /* for '\0' */;
			//console_printf("%s; closerStringEnd is 'NULL': closerStringEnd= 0x%x\n", __FUNCTION__, (unsigned int)closerStringEnd);

			//console_printf("%s; String end did NOT found ==> set closerStringEnd to the end of buf; closerStringEnd= 0x%x\n", __FUNCTION__, (unsigned int)closerStringEnd);
		}

		//console_printf("%s; stringToSearch= '%s'; restOfStringStart= '%s'; buf= '%s'\n", __FUNCTION__, stringToSearch, restOfStringStart, buf);
		//console_printf("%s; restOfStringStart= 0x%x, closerStringEnd= 0x%x ==> characters to copy = %d\n", __FUNCTION__, (unsigned int)restOfStringStart, (unsigned int)closerStringEnd, closerStringEnd - restOfStringStart);

		/* set 'numOfCharacters' with the number of characters to copy (including the blank or end-of-string at the end) */
		numOfCharacters = closerStringEnd - restOfStringStart;
		if (numOfCharacters <= 0)
		{
			console_printf("%s; numOfCharacters= %d ==> Abort!\n", __FUNCTION__, numOfCharacters);
			free((void *)localBuf);
			free((void *)localStringToSearch);
			free((void *)tempStringOfValues);
			free((void *)localEndFieldName);
			return false;
		}

		if (numOfCharacters >= (int)HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH)
		{
			console_printf("%s; numOfCharacters (%d) >= HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH (%d) ==> Abort!\n", __FUNCTION__, numOfCharacters, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
			free((void *)localBuf);
			free((void *)localStringToSearch);
			free((void *)tempStringOfValues);
			free((void *)localEndFieldName);
			return false;
		}

		/* Copy the characters of the value, and set the last one to '\0' */
		strncpy_s(tempStringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, restOfStringStart, numOfCharacters);
		tempStringOfValues[numOfCharacters - 1] = '\0';
		//console_printf("%s; stringToSearch= '%s'; tempStringOfValues= '%s'\n", __FUNCTION__, stringToSearch, tempStringOfValues);

		/* Check if all elements are valid; if an element of an array contains "=", it is NOT valid ==> do NOT copy it! */
		for (i=0; i < numOfCharacters; i++)
		{
			if ( (parsingType == DWPAL_STR_ARRAY_PARAM) ||
			     (parsingType == DWPAL_INT_ARRAY_PARAM) ||
				 (parsingType == DWPAL_INT_HEX_ARRAY_PARAM) )
			{
				if ( (tempStringOfValues[i] == '=') || (tempStringOfValues[i] == '\0') )
				{
					numOfCharactersToCopy = i + 1 /* convert index to number-of */;
					break;  /* Only in case of array of values, do NOT allow "=" character */
				}
			}
			/* Special case for SSIDs that may can contain spaces. */
			else if ( (strncmp(localEndFieldName, "SSID=", 6) == 0) || (strncmp(localEndFieldName, " SSID=", 7) == 0) )
			{
				if (tempStringOfValues[i] == ' ')
				{
					numOfCharactersToCopy = i + 1 /* convert index to number-of */;
				}
				else if (tempStringOfValues[i] == '=')
				{
					break;
				}
				else if(tempStringOfValues[i] == '\0')
				{
					numOfCharactersToCopy = i + 1 /* convert index to number-of */;
					break;
				}
			}
			else if ( (tempStringOfValues[i] == ' ') || (tempStringOfValues[i] == '\0') )
			{
				numOfCharactersToCopy = i + 1 /* convert index to number-of */;
				break;
			}
		}

		strncpy_s(&stringOfValues[idx], HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH, restOfStringStart, numOfCharactersToCopy);
		idx += numOfCharactersToCopy;
		stringOfValues[idx] = '\0';

		//console_printf("%s; stringToSearch= '%s'; stringOfValues= '%s'\n", __FUNCTION__, stringToSearch, stringOfValues);

		closerStringEnd = NULL;
	}

	/* Remove all ' ' from the end of the string */
	for (i= idx-1; i >= 0; i--)
	{
		if (stringOfValues[i] != ' ')
		{
			break;  /* Stop removing the ' ' characters when the first non-blank character was found! */
		}
		else if (stringOfValues[i] == ' ')
		{  /* stringOfValues[i] == ' ' */
			stringOfValues[i] = '\0';
		}
	}

	//console_printf("%s; stringToSearch= '%s'; stringOfValues= '%s'\n", __FUNCTION__, stringToSearch, stringOfValues);

	free((void *)localBuf);
	free((void *)localStringToSearch);
	free((void *)tempStringOfValues);
	free((void *)localEndFieldName);

	//console_printf("%s; ret= %d, stringToSearch= '%s'; stringOfValues= '%s'\n", __FUNCTION__, ret, stringToSearch, stringOfValues);

	return ret;

err:
	if (localBuf) free((void *)localBuf);
	if (localStringToSearch) free((void *)localStringToSearch);
	if (tempStringOfValues) free((void *)tempStringOfValues);
	if (localEndFieldName) free((void *)localEndFieldName);
	return false;
}


static bool isColumnOfFields(char *msg, char *endFieldName[])
{
	int i = 0, numOfFieldsInLine = 0;

	//console_printf("%s; line= '%s'\n", __FUNCTION__, msg);

	if (endFieldName == NULL)
	{
		console_printf("%s; endFieldName= 'NULL' ==> not a column!\n", __FUNCTION__);
		return false;
	}

	while (strncmp(endFieldName[i], "\n", 1))
	{  /* run over all field names in the string */
		if (strstr(msg, endFieldName[i]) != NULL)
		{
			numOfFieldsInLine++;

			if (numOfFieldsInLine > 1)
			{
				//console_printf("%s; Not a column (numOfFieldsInLine= %d) ==> return!\n", __FUNCTION__, numOfFieldsInLine);
				return false;
			}

			/* Move ahead inside the line, to avoid double recognition (like "PacketsSent" and "DiscardPacketsSent") */
			msg += strnlen_s(endFieldName[i], HOSTAPD_TO_DWPAL_MSG_LENGTH);
		}

		i++;
	}

	//console_printf("%s; It is a column (numOfFieldsInLine= %d)\n", __FUNCTION__, numOfFieldsInLine);

	return true;
}


static bool columnOfParamsToRowConvert(char *msg, size_t msgLen, char *endFieldName[])
{
	char    *localMsg = strdup(msg), *lineMsg, *p2str;
	rsize_t dmaxLen = (rsize_t)msgLen;
	bool    isColumn = true;
	int     i;

	if (localMsg == NULL)
	{
		console_printf("%s; strdup error ==> Abort!\n", __FUNCTION__);
		return false;
	}

	lineMsg = strtok_s(localMsg, &dmaxLen, "\n", &p2str);

	while (lineMsg != NULL)
	{
		isColumn = isColumnOfFields(lineMsg, endFieldName);

		if (isColumn == false)
		{
			//console_printf("%s; Not a column ==> break!\n", __FUNCTION__);
			break;
		}

		lineMsg = strtok_s(NULL, &dmaxLen, "\n", &p2str);
	}

	free ((void *)localMsg);

	if (isColumn)
	{
		/* Modify the column string to be in ONE raw  */
		for (i=0; i < (int)msgLen; i++)
		{
			if (msg[i] == '\n')
			{
				msg[i] = ' ';
			}
		}

		msg[msgLen] = '\0';
	}

	return true;
}



/* Low Level APIs */

DWPAL_Ret dwpal_proc_file_copy(void *buffer, uint *ret, uint max_size, char * filename)
{
	FILE *ptr_myfile;
	char full_path[MAX_FILE_NAME];
	int res = sprintf_s(full_path, sizeof(full_path), "/proc/net/mtlk/%s", filename);
	if (res < 0 || res >= (int)sizeof(full_path)){
		console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	ptr_myfile=fopen(full_path,"r");
	if (!ptr_myfile)
	{
		console_printf("%s; Unable to open file!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}
	*ret = fread(buffer,1,max_size,ptr_myfile);

	fclose(ptr_myfile);
	return DWPAL_SUCCESS;
}

DWPAL_Ret dwpal_driver_nl_scan_dump(void *context, char *ifname, DWPAL_nlNonVendorEventCallback nlEventCallback)
{
	return scanPerform(context, ifname, NL80211_CMD_GET_SCAN, NLM_F_DUMP, nlEventCallback, NULL);
}


DWPAL_Ret dwpal_driver_nl_scan_trigger(void *context, char *ifname, ScanParams *scanParams)
{
	return scanPerform(context, ifname, NL80211_CMD_TRIGGER_SCAN, 0, NULL, scanParams);
}

/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_driver_nl_cmd_send(void *context, DWPAL_NlEventType nlEventType, char *ifname, enum nl80211_commands nl80211Command, CmdIdType cmdIdType, enum ltq_nl80211_vendor_subcmds subCommand, unsigned char *vendorData, size_t vendorDataSize)
 **************************************************************************
 *  \brief driver-NL send command 
 *  \param[in] void *context - Provides all the interface information
 *  \param[in] DWPAL_NlEventType nlEventType - Indicates which command it is: regular command or a "get" command
 *  \param[in] char *ifname - the radio interface
 *  \param[in] unsigned int nl80211Command - NL 80211 command. Note: currently we support ONLY NL80211_CMD_VENDOR (0x67)
 *  \param[in] CmdIdType cmdIdType - The command ID type: NETDEV, PHY or WDEV
 *  \param[in] unsigned int subCommand - the vendor’s sub-command
 *  \param[in] unsigned char *vendorData - the vendor’s data (can be NULL)
 *  \param[in] size_t vendorDataSize - the vendor’s data length (if the vendor data is NULL, it should be ‘0’)
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_driver_nl_cmd_send(void *context,
                                   DWPAL_NlEventType nlEventType,
								   char *ifname,
								   enum nl80211_commands nl80211Command,
								   CmdIdType cmdIdType,
								   enum ltq_nl80211_vendor_subcmds subCommand,
								   unsigned char *vendorData,
								   size_t vendorDataSize)
{
	int              i, res;
	struct nl_msg    *msg;
	DWPAL_Context    *localContext = (DWPAL_Context *)(context);
	signed long long devidx = 0;
	struct nl_sock   *nlSocket = NULL;

	console_printf("%s Entry\n", __FUNCTION__);

	if (nl80211Command != NL80211_CMD_VENDOR /*0x67*/)
	{
		console_printf("%s; non supported command (0x%x); currently we support ONLY NL80211_CMD_VENDOR (0x67) ==> Abort!\n", __FUNCTION__, (unsigned int)nl80211Command);
		return DWPAL_FAILURE;
	}

	for (i=0; i < (int)vendorDataSize; i++)
	{
		console_printf("%s; vendorData[%d]= 0x%x\n", __FUNCTION__, i, vendorData[i]);
	}

	if (nlEventType == DWPAL_NL_UNSOLICITED_EVENT)
	{
		nlSocket = localContext->interface.driver.nlSocketEvent;
	}
	else if (nlEventType == DWPAL_NL_SOLICITED_EVENT)
	{
		nlSocket = localContext->interface.driver.nlSocketCmdGet;
	}
	else
	{
		console_printf("%s; invalid nlEventType (%d) ==> Abort!\n", __FUNCTION__, nlEventType);
		return DWPAL_FAILURE;
	}

	if (localContext == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (nlSocket == NULL)
	{
		console_printf("%s; nlSocket is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	msg = nlmsg_alloc();
	if (msg == NULL)
	{
		console_printf("%s; nlmsg_alloc returned NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	console_printf("%s; nl80211_id= %d, nl80211Command= %d\n", __FUNCTION__, localContext->interface.driver.nl80211_id, nl80211Command);

	/* calling genlmsg_put() is a must! without it, the callback won't be called! */
	genlmsg_put(msg, 0, 0, localContext->interface.driver.nl80211_id, 0,0, nl80211Command /* NL80211_CMD_VENDOR=0x67*/, 0);

	//iw dev wlan0 vendor recv 0xAC9A96 0x69 0x00 ==> send "0xAC9A96 0x69 0x00"
	devidx = if_nametoindex(ifname);
	if (devidx < 0)
	{
		console_printf("%s; devidx ERROR (devidx= %lld) ==> Abort!\n", __FUNCTION__, devidx);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	switch (cmdIdType)
	{
		case DWPAL_NETDEV_ID:
			res = nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);
			//NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);
			break;

		case DWPAL_PHY_ID:
			res = nla_put_u32(msg, NL80211_ATTR_WIPHY, devidx);
			//NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, devidx);
			break;

		case DWPAL_WDEV_ID:
			res = nla_put_u64(msg, NL80211_ATTR_WDEV, devidx);
			//NLA_PUT_U64(msg, NL80211_ATTR_WDEV, devidx);
			break;

		default:
			console_printf("%s; cmdIdType ERROR (cmdIdType= %d) ==> Abort!\n", __FUNCTION__, cmdIdType);
			nlmsg_free(msg);
			return DWPAL_FAILURE;
	}

	if (res < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	res = nla_put_u32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ /*0xAC9A96*/);
	//NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, OUI_LTQ /*0xAC9A96*/);
	if (res < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	res = nla_put_u32(msg, NL80211_ATTR_VENDOR_SUBCMD, subCommand);
	//NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, subCommand);
	if (res < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	if ( (vendorDataSize > 0) && (vendorData != NULL) )
	{
		//NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, count, buf);
		res = nla_put(msg, NL80211_ATTR_VENDOR_DATA, (int)vendorDataSize, (void *)vendorData);
		if (res < 0)
		{
			console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
			nlmsg_free(msg);
			return DWPAL_FAILURE;
		}
	}

	/* will trigger nlInternalEventCallback() / nlInternalCmdGetCallback() function call */
	res = nl_send_auto(nlSocket, msg);  // can use nl_send_auto_complete(nlSocket, msg) instead
	if (res < 0)
	{
		console_printf("%s; nl_send_auto returned ERROR (res= %d) ==> Abort!\n", __FUNCTION__, res);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	nlmsg_free(msg);

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_driver_nl_msg_get(void *context, DWPAL_NlEventType nlEventType, DWPAL_nlVendorEventCallback nlEventCallback)
 **************************************************************************
 *  \brief Get the Driver-NL message, It will cause the equivalent provided callback function to be called with the event data
 *  \param[in] void *context - Provides all the interface information
 *  \param[in] DWPAL_NlEventType nlEventType - Indicates whether it is an unsolicited event to get or solicited event (derived from "get" command) to get
 *  \param[in] DWPAL_nlVendorEventCallback nlEventCallback - Provides the callback function to be called with the event data
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_driver_nl_msg_get(void *context, DWPAL_NlEventType nlEventType, DWPAL_nlVendorEventCallback nlEventCallback, DWPAL_nlNonVendorEventCallback nlNonVendorEventCallback)
{
	int           res, err = 0;
	struct nl_cb  *cb;
	DWPAL_Context *localContext = (DWPAL_Context *)(context);

	console_printf("%s Entry; nlEventType= %d (DWPAL_NL_UNSOLICITED_EVENT=0, DWPAL_NL_SOLICITED_EVENT=1)\n", __FUNCTION__, nlEventType);

	if (localContext == NULL)
	{
		console_printf("%s; localContext is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if ( ( (nlEventType == DWPAL_NL_UNSOLICITED_EVENT) && (localContext->interface.driver.nlSocketEvent == NULL) ) ||
	     ( (nlEventType == DWPAL_NL_SOLICITED_EVENT) && (localContext->interface.driver.nlSocketCmdGet == NULL) ) )
	{
		console_printf("%s; nlSocket is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Connect the nl socket to its message callback function */
	cb = nl_cb_alloc(NL_CB_DEFAULT);

	if (cb == NULL)
	{
		console_printf("%s; failed to allocate netlink callbacks ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish, context);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_recv, NULL);
	if (nlEventType == DWPAL_NL_UNSOLICITED_EVENT)
	{
		/* nlEventCallback can be NULL; in that case, the D-WPAL client's callback function won't be called */
		localContext->interface.driver.nlEventCallback = nlEventCallback;
		if (nlNonVendorEventCallback != NULL)
			localContext->interface.driver.nlNonVendorEventCallback = nlNonVendorEventCallback;

		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nlInternalEventCallback, context);

		/* will trigger nlEventCallback() function call */
		res = nl_recvmsgs(localContext->interface.driver.nlSocketEvent, cb);
		nl_cb_put(cb);
	}
	else if (nlEventType == DWPAL_NL_SOLICITED_EVENT)
	{
		/* nlEventCallback can be NULL; in that case, the D-WPAL client's callback function won't be called */
		localContext->interface.driver.nlCmdGetCallback = nlEventCallback;

		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nlInternalCmdGetCallback, context);

		/* will trigger nlEventCallback() function call */
		res = nl_recvmsgs(localContext->interface.driver.nlSocketCmdGet, cb);
		nl_cb_put(cb);
	}
	else
	{
		console_printf("%s; invalid nlEventType (%d) ==> Abort!\n", __FUNCTION__, nlEventType);
		nl_cb_put(cb);
		return DWPAL_FAILURE;
	}

	if (res < 0 || err < 0)
	{
		console_printf("%s; nl_recvmsgs returned ERROR, res= %d (err= %d) ==> Abort!\n", __FUNCTION__, res, err);
		return DWPAL_FAILURE;
	}

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_driver_nl_fd_get(void *context, int *fd , int *fdCmdGet)
 **************************************************************************
 *  \brief Driver-NL fd get, Will fill up the fd output parameters - one of the event, and the other of the "get" command
 *  \param[in] void *context - Provides all the interface information
 *  \param[out] int *fd - Provides the interface's fd value
 *  \param[out] int *fdCmdGet - Provides the interface's fd value of the "get" command
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_driver_nl_fd_get(void *context, int *fd /*OUT*/, int *fdCmdGet /*OUT*/)
{
	if ( (context == NULL) || (fd == NULL) || (fdCmdGet == NULL) )
	{
		console_printf("%s; context and/or fd, fdCmdGet is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	*fd = ((DWPAL_Context *)context)->interface.driver.fd;
	if (*fd == (-1))
	{
		console_printf("%s; fd value is (-1) ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	*fdCmdGet = ((DWPAL_Context *)context)->interface.driver.fdCmdGet;
	if (*fdCmdGet == (-1))
	{
		console_printf("%s; fdCmdGet value is (-1) ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	return DWPAL_SUCCESS;
}


static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	(void)msg;

	if (ret)
		*ret = 0;
	return NL_SKIP;
}


static int dwpal_empty_nl80211Callback(struct nl_msg *msg, void *arg)
{
	(void)msg;
	(void)arg;
	return NL_SKIP;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_nl80211_cmd_send(void *context, struct nl_msg *msg, DWPAL_nl80211Callback cb)
 **************************************************************************
 *  \brief NL80211 send command
 *  \param[in] void *context - Provides all the interface information
 *  \param[in] struct nl_msg *msg - the nl command to send
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_nl80211_cmd_send(void *context, struct nl_msg *msg, int *cmd_res /*OUT*/, DWPAL_nl80211Callback nlCallback, void *cb_arg)
{
	DWPAL_Context	*localContext = (DWPAL_Context *)(context);
	struct nl_sock	*nlSocket = NULL;
	struct nl_cb 	*cb = NULL;
	int		res = 0;
	DWPAL_Ret	ret = DWPAL_FAILURE;
	bool		nl_socket_clean = false;
	int		fdCmdGet;

	console_printf("%s Entry\n", __FUNCTION__);

	if (msg == NULL)
	{
		console_printf("%s; msg is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		goto err;
	}

	nlSocket = localContext->interface.driver.nlSocketCmdGet;
	if (nlSocket == NULL)
	{
		console_printf("%s; nlSocket is NULL ==> Abort!\n", __FUNCTION__);
		goto err;
	}

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (cb == NULL)
	{
		console_printf("%s; failed to allocate netlink callbacks ==> Abort!\n", __FUNCTION__);
		goto err;
	}

	fdCmdGet = localContext->interface.driver.fdCmdGet;
	if (fdCmdGet == (-1))
	{
		console_printf("%s; fdCmdGet value is (-1) ==> Abort!\n", __FUNCTION__);
		goto err;
	}

	while (nl_socket_clean == false)
	{
		fd_set		rfds;
		struct timeval	tv;

		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(fdCmdGet, &rfds);

		res = select(fdCmdGet + 1, &rfds, NULL, NULL, &tv);
		if (res == -1 && errno == EINTR)
		{
			usleep(1000);
			continue;
		}
		else if (res == -1)
		{
			console_printf("%s; select() returned error, errno = %d ==> Abort!\n", __FUNCTION__, errno);
			goto err;
		}
		else if (res > 0 && FD_ISSET(fdCmdGet, &rfds))
		{
			struct nl_cb *cleanup_cb = NULL;

			console_printf("%s; need to perform cleanup\n", __FUNCTION__);

			cleanup_cb = nl_cb_alloc(NL_CB_DEFAULT);
			if (cleanup_cb == NULL)
			{
				console_printf("%s; failed to allocate netlink callbacks ==> Abort!\n", __FUNCTION__);
				goto err;
			}

			nl_cb_err(cleanup_cb, NL_CB_CUSTOM, error_handler, NULL);
			nl_cb_set(cleanup_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
			nl_cb_set(cleanup_cb, NL_CB_ACK, NL_CB_CUSTOM, ack_recv, NULL);
			nl_cb_set(cleanup_cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, NULL);
			nl_cb_set(cleanup_cb, NL_CB_VALID, NL_CB_CUSTOM, dwpal_empty_nl80211Callback, NULL);

			nl_recvmsgs(nlSocket, cleanup_cb);
			nl_cb_put(cleanup_cb);
		}
		else
		{
			nl_socket_clean = true;
		}
	}

	res = nl_send_auto(nlSocket, msg);
	if (res < 0)
	{
		console_printf("%s; nl_send_auto returned ERROR (res= %d) ==> Abort!\n", __FUNCTION__, res);
		goto err;
	}

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, cmd_res);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_recv, cmd_res);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, cmd_res);

	if (nlCallback)
	{
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nlCallback, cb_arg);
	}

	*cmd_res = 1;
	while (*cmd_res > 0)
	{
		fd_set		rfds;
		struct timeval	tv;

		tv.tv_sec = 2;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(fdCmdGet, &rfds);

		res = select(fdCmdGet + 1, &rfds, NULL, NULL, &tv);
		if (res == -1 && errno == EINTR)
		{
			usleep(1000);
			continue;
		}
		else if (res == -1)
		{
			console_printf("%s; select() returned error, errno = %d ==> Abort!\n", __FUNCTION__, errno);
			goto err;
		}
		else if (res == 0)
		{
			console_printf("%s; Timeout ==> Abort!\n", __FUNCTION__);
			goto err;
		}

		res = nl_recvmsgs(nlSocket, cb);
		if (res == NLE_DUMP_INTR)
		{
			console_printf("%s; nl_recvmsgs returned NLE_DUMP_INTR\n", __FUNCTION__);
			*cmd_res = -EAGAIN;
		}
		else if (res < 0)
		{
			console_printf("%s; nl_recvmsgs returned ERROR (res= %d)\n", __FUNCTION__, res);
		}
	}

	ret = DWPAL_SUCCESS;
err:
	if (cb)
		nl_cb_put(cb);
	nlmsg_free(msg);
	return ret;
}


DWPAL_Ret dwpal_driver_nl_scan_dump_sync(void *context, char *ifname, int *cmd_res /*OUT*/, DWPAL_nl80211Callback nlCallback, void *cb_arg)
{
	int              ret;
	struct nl_msg    *msg;
	DWPAL_Context    *localContext = (DWPAL_Context *)(context);
	signed long long devidx = 0;

	console_printf("%s Entry; ifname= '%s'\n", __FUNCTION__, ifname);

	if (localContext == NULL || !cmd_res)
	{
		console_printf("%s; context or cmd_res is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	msg = nlmsg_alloc();
	if (msg == NULL)
	{
		console_printf("%s; nlmsg_alloc returned NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	console_printf("%s; nl80211_id= %d\n", __FUNCTION__, localContext->interface.driver.nl80211_id);

	/* calling genlmsg_put() is a must! without it, the callback won't be called! */
	genlmsg_put(msg, 0, 0, localContext->interface.driver.nl80211_id, 0, NLM_F_DUMP, NL80211_CMD_GET_SCAN, 0);

	devidx = if_nametoindex(ifname);
	if (devidx < 0)
	{
		console_printf("%s; devidx ERROR (devidx= %lld) ==> Abort!\n", __FUNCTION__, devidx);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	ret = nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);  /* DWPAL_NETDEV_ID */
	if (ret < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	return dwpal_nl80211_cmd_send(context, msg, cmd_res, nlCallback, cb_arg);
}


DWPAL_Ret dwpal_driver_nl_scan_trigger_sync(void *context, char *ifname, int *cmd_res /*OUT*/, ScanParams *scanParams)
{
	int              ret;
	struct nl_msg    *msg;
	DWPAL_Context    *localContext = (DWPAL_Context *)(context);
	signed long long devidx = 0;

	console_printf("%s Entry; ifname= '%s'\n", __FUNCTION__, ifname);

	if (localContext == NULL || !cmd_res)
	{
		console_printf("%s; context or cmd_res is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	msg = nlmsg_alloc();
	if (msg == NULL)
	{
		console_printf("%s; nlmsg_alloc returned NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	console_printf("%s; nl80211_id= %d\n", __FUNCTION__, localContext->interface.driver.nl80211_id);

	/* calling genlmsg_put() is a must! without it, the callback won't be called! */
	genlmsg_put(msg, 0, 0, localContext->interface.driver.nl80211_id, 0, 0, NL80211_CMD_TRIGGER_SCAN, 0);

	devidx = if_nametoindex(ifname);
	if (devidx < 0)
	{
		console_printf("%s; devidx ERROR (devidx= %lld) ==> Abort!\n", __FUNCTION__, devidx);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	ret = nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);  /* DWPAL_NETDEV_ID */
	if (ret < 0)
	{
		console_printf("%s; building message failed ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	if (scanParamHandle(msg, scanParams) == DWPAL_FAILURE)
	{
		console_printf("%s; scanParamHandle returned error ==> Abort!\n", __FUNCTION__);
		nlmsg_free(msg);
		return DWPAL_FAILURE;
	}

	return dwpal_nl80211_cmd_send(context, msg, cmd_res, NULL, NULL);
}


DWPAL_Ret dwpal_nl80211_id_get(void *context, int *nl80211_id /*OUT*/)
{
	DWPAL_Context *localContext = (DWPAL_Context *)(context);

	console_printf("%s Entry\n", __FUNCTION__);

	if (nl80211_id == NULL)
	{
		console_printf("%s; nl80211_id is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	*nl80211_id = localContext->interface.driver.nl80211_id;
	return (*nl80211_id < 0) ? DWPAL_FAILURE : DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_driver_nl_detach(void **context)
 **************************************************************************
 *  \brief Driver-NL interface detach, Will detach/reset the interface towards/from the Driver NL socket
 *  \param[in,out] void **context - Will deallocate and set context to NULL
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_driver_nl_detach(void **context /*IN/OUT*/)
{
	DWPAL_Context *localContext;

	console_printf("%s Entry\n", __FUNCTION__);

	if (context == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	localContext = (DWPAL_Context *)(*context);
	if (localContext == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext->interface.driver.nlSocketEvent == NULL)
	{
		console_printf("%s; nlSocketEvent is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Note: calling nl_close() is NOT needed - The socket is closed automatically when using nl_socket_free() */
	console_printf("%s; close NL sockets\n", __FUNCTION__);
	nl_socket_free(localContext->interface.driver.nlSocketEvent);
	nl_socket_free(localContext->interface.driver.nlSocketCmdGet);

	free(*context);
	*context = NULL;

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_driver_nl_attach(void **context)
 **************************************************************************
 *  \brief Driver-NL interface attach, Will set the interface towards/from the Driver NL socket
 *  \param[out] void **context - Will be allocated and returned for further use with this interface
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_driver_nl_attach(void **context /*OUT*/)
{
	int           mcid, i;
	DWPAL_Context *localContext;

	console_printf("%s Entry\n", __FUNCTION__);

	if (context == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	*context = malloc(sizeof(DWPAL_Context));
	if (*context == NULL)
	{
		console_printf("%s; malloc for context failed ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	localContext = (DWPAL_Context *)(*context);
	memset(localContext, 0, sizeof(DWPAL_Context));

	/* Create the NL socket for the events (unsolicited events) */
	if (nlSocketCreate(&localContext->interface.driver.nlSocketEvent, &localContext->interface.driver.fd) == DWPAL_FAILURE)
	{
		console_printf("%s; nlSocketCreate failed ==> Abort!\n", __FUNCTION__);
		free(*context);
		*context = NULL;
		return DWPAL_FAILURE;
	}

	for (i = 0; i < (int)(sizeof(gNlServices)/sizeof(nlService_t)); i++)
	{
		mcid = genl_ctrl_resolve_grp(localContext->interface.driver.nlSocketEvent, "nl80211", gNlServices[i].serviceName);

		console_printf("%s; mcid= %d for adding service %s\n", __FUNCTION__, mcid, gNlServices[i].serviceName);

		if (nl_socket_add_membership(localContext->interface.driver.nlSocketEvent, mcid) < 0)
		{
			console_printf("%s; nl_socket_add_membership ERROR ==> Abort!\n", __FUNCTION__);
			free(*context);
			*context = NULL;
			return DWPAL_FAILURE;
		}
	}

	/* Create the NL socket for the 'get commands' (solicited events) */
	if (nlSocketCreate(&localContext->interface.driver.nlSocketCmdGet, &localContext->interface.driver.fdCmdGet) == DWPAL_FAILURE)
	{
		console_printf("%s; nlSocketCreate failed ==> Abort!\n", __FUNCTION__);
		free(*context);
		*context = NULL;
		return DWPAL_FAILURE;
	}

	/* Ask kernel to resolve nl80211_id name to nl80211_id id; nl80211_id is being used only for command send */
	localContext->interface.driver.nl80211_id = genl_ctrl_resolve(localContext->interface.driver.nlSocketCmdGet, "nl80211");
	if (localContext->interface.driver.nl80211_id < 0)
	{
		console_printf("%s; genl_ctrl_resolve ERROR ==> Abort!\n", __FUNCTION__);
		nl_socket_free(localContext->interface.driver.nlSocketEvent);
		nl_socket_free(localContext->interface.driver.nlSocketCmdGet);
		free(*context);
		*context = NULL;
		return DWPAL_FAILURE;
	}
	console_printf("%s; driver.nl80211_id= %d\n", __FUNCTION__, localContext->interface.driver.nl80211_id);

	console_printf("%s; driver.nlSocketEvent= %p, fd= %d, driver.nlEventCallback= %p, driver.nlNonVendorEventCallback = %p driver.nl80211_id= %d; nlSocketCmdGet= %p, fdCmdGet= %d\n",
	       __FUNCTION__, (void *)localContext->interface.driver.nlSocketEvent, localContext->interface.driver.fd,
		   (void *)localContext->interface.driver.nlEventCallback, (void *)localContext->interface.driver.nlNonVendorEventCallback, localContext->interface.driver.nl80211_id,
		   (void *)localContext->interface.driver.nlSocketCmdGet, localContext->interface.driver.fdCmdGet);

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_string_to_struct_parse(char *msg, size_t msgLen, FieldsToParse fieldsToParse[], size_t userBufLen))
 **************************************************************************
 *  \brief Provides parsing services from hostap string to structure
 *  \param[in] char *msg - Provides the string to be parsed
 *  \param[in] size_t msgLen - The string’s length
 *  \param[in] FieldsToParse fieldsToParse[] - The information needed for the actual parsing
 *  \param[in] size_t userBufLen - The user total buffer size to hold parsed info from inside fieldsToParse
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_string_to_struct_parse(char *msg, size_t msgLen, FieldsToParse fieldsToParse[], size_t userBufLen)
{
	DWPAL_Ret ret = DWPAL_SUCCESS;
	int       i = 0, idx = 0, numOfNameArrayArgs = 0, lineIdx = 0;
	bool      isEndFieldNameAllocated = false, isMissingParam = false;
	char      *stringOfValues = NULL;
	char      *lineMsg, *localMsg, *p2str = NULL, *p2strMandatory = NULL;
	rsize_t   dmaxLen, dmaxLenMandatory;
	size_t    sizeOfStruct = 0, msgStringLen;
	char      **endFieldName = NULL;

	stringOfValues = (char *)malloc(HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);
	if (stringOfValues == NULL)
	{
		console_printf("%s; malloc failed ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if ( (msg == NULL) || (msgLen == 0) || (fieldsToParse == NULL) )
	{
		console_printf("%s; input params error ==> Abort!\n", __FUNCTION__);
		free((void *)stringOfValues);
		return DWPAL_FAILURE;
	}

	if ( (msgStringLen = strnlen_s(msg, HOSTAPD_TO_DWPAL_MSG_LENGTH)) > msgLen )
	{
		console_printf("%s; msgStringLen (%d) is bigger than msgLen (%d) ==> Abort!\n", __FUNCTION__, msgStringLen, msgLen);
		free((void *)stringOfValues);
		return DWPAL_FAILURE;
	}

	/* Convert msgLen to string length format (without the '\0' character) */
	msgLen = dmaxLen = msgStringLen;

	/* Set values for 'numOfNameArrayArgs' and 'sizeOfStruct' */
	while (fieldsToParse[i].parsingType != DWPAL_NUM_OF_PARSING_TYPES)
	{
		/* Set numOfNameArrayArgs with the number of endFieldName arguments - needed for the dynamic allocation */
		if (fieldsToParse[i].stringToSearch != NULL)
		{
			numOfNameArrayArgs++;
		}

		/* Set sizeOfStruct with the structure size of the output parameter - needed for advancing the output array index (in case of many lines) */
		if (fieldsToParse[i].field != NULL)
		{
			switch (fieldsToParse[i].parsingType)
			{
				case DWPAL_STR_PARAM:
					if ( (fieldsToParse[i].field != NULL) && (fieldsToParse[i].totalSizeOfArg == 0) )
					{
						console_printf("%s; Error; DWPAL_STR_PARAM must have positive value for totalSizeOfArg ==> Abort!\n", __FUNCTION__);
						free((void *)stringOfValues);
						return DWPAL_FAILURE;
					}

					sizeOfStruct += sizeof(char) * fieldsToParse[i].totalSizeOfArg;  /* array of characters (string) */
					//console_printf("%s; DWPAL_STR_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_STR_ARRAY_PARAM:
					if ( (fieldsToParse[i].field != NULL) && (fieldsToParse[i].totalSizeOfArg == 0) )
					{
						console_printf("%s; Error; DWPAL_STR_ARRAY_PARAM must have positive value for totalSizeOfArg ==> Abort!\n", __FUNCTION__);
						free((void *)stringOfValues);
						return DWPAL_FAILURE;
					}

					sizeOfStruct += fieldsToParse[i].totalSizeOfArg;
					//console_printf("%s; DWPAL_STR_ARRAY_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_CHAR_PARAM:
					sizeOfStruct += sizeof(char);
					//console_printf("%s; DWPAL_CHAR_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_UNSIGNED_CHAR_PARAM:
					sizeOfStruct += sizeof(unsigned char);
					//console_printf("%s; DWPAL_UNSIGNED_CHAR_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_SHORT_INT_PARAM:
					sizeOfStruct += sizeof(short int);
					//console_printf("%s; DWPAL_SHORT_INT_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_INT_PARAM:
				case DWPAL_INT_HEX_PARAM:
					sizeOfStruct += sizeof(int);
					//console_printf("%s; DWPAL_INT_PARAM/DWPAL_INT_HEX_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_UNSIGNED_INT_PARAM:
					sizeOfStruct += sizeof(unsigned int);
					//console_printf("%s; DWPAL_UNSIGNED_INT_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_LONG_LONG_INT_PARAM:
					sizeOfStruct += sizeof(long long int);
					//console_printf("%s; DWPAL_LONG_LONG_INT_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_UNSIGNED_LONG_LONG_INT_PARAM:
					sizeOfStruct += sizeof(unsigned long long int);
					//console_printf("%s; DWPAL_UNSIGNED_LONG_LONG_INT_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_INT_ARRAY_PARAM:
				case DWPAL_INT_HEX_ARRAY_PARAM:
					if ( (fieldsToParse[i].field != NULL) && (fieldsToParse[i].totalSizeOfArg == 0) )
					{
						console_printf("%s; Error; DWPAL_INT_ARRAY_PARAM/DWPAL_INT_HEX_ARRAY_PARAM must have positive value for totalSizeOfArg ==> Abort!\n", __FUNCTION__);
						free((void *)stringOfValues);
						return DWPAL_FAILURE;
					}

					sizeOfStruct += sizeof(int) * fieldsToParse[i].totalSizeOfArg;
					//console_printf("%s; DWPAL_INT_ARRAY_PARAM/DWPAL_INT_HEX_ARRAY_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_BOOL_PARAM:
					sizeOfStruct += sizeof(bool);
					//console_printf("%s; DWPAL_BOOL_PARAM; sizeOfStruct= %d\n", __FUNCTION__, sizeOfStruct);
					break;

				case DWPAL_DUMMY_PARAM:
					break;

				default:
					console_printf("%s; (parsingType= %d) ERROR ==> Abort!\n", __FUNCTION__, fieldsToParse[i].parsingType);
					ret = DWPAL_FAILURE;
					break;
			}
		}

		i++;
	}

	/* Allocate and set the value for each endFieldName[] string */
	if (numOfNameArrayArgs > 0)
	{
		numOfNameArrayArgs++;  /* for the last allocated argument */

		endFieldName = (char **)malloc(sizeof(*endFieldName) * numOfNameArrayArgs);
		if (endFieldName == NULL)
		{
			console_printf("%s; malloc endFieldName failed ==> Abort!\n", __FUNCTION__);
			ret = DWPAL_FAILURE;
		}
		else
		{
			memset((void *)endFieldName, 0, sizeof(*endFieldName) * numOfNameArrayArgs);
		}

		i = idx = 0;
		while ( (fieldsToParse[i].parsingType != DWPAL_NUM_OF_PARSING_TYPES) && (idx < (numOfNameArrayArgs-1)) && (ret == DWPAL_SUCCESS) )
		{
			if (fieldsToParse[i].numOfValidArgs != NULL)
			{
				*(fieldsToParse[i].numOfValidArgs) = 0;
			}

			if (fieldsToParse[i].stringToSearch != NULL)
			{
				endFieldName[idx] =  (char *)malloc(DWPAL_FIELD_NAME_LENGTH);
				if (endFieldName[idx] == NULL)
				{
					console_printf("%s; malloc endFieldName[%d] failed ==> Abort!\n", __FUNCTION__, i);
					ret = DWPAL_FAILURE;
					break;
				}

				memset((void *)endFieldName[idx], '\0', DWPAL_FIELD_NAME_LENGTH);  /* Clear the field name */
				strcpy_s(endFieldName[idx], strnlen_s(fieldsToParse[i].stringToSearch, DWPAL_FIELD_NAME_LENGTH) + 1, fieldsToParse[i].stringToSearch);

				idx++;
			}

			i++;
		}

		if (ret == DWPAL_SUCCESS)
		{
			endFieldName[idx] =  (char *)malloc(DWPAL_FIELD_NAME_LENGTH);
			if (endFieldName[idx] == NULL)
			{
				console_printf("%s; malloc endFieldName[%d] failed ==> Abort!\n", __FUNCTION__, idx);
				ret = DWPAL_FAILURE;
			}
			else
			{
				memset((void *)endFieldName[idx], '\0', DWPAL_FIELD_NAME_LENGTH);  /* Clear the field name */
				strcpy_s(endFieldName[idx], 2, "\n");
				isEndFieldNameAllocated = true;
			}
		}
	}

	//console_printf("%s; msg= '%s'\n", __FUNCTION__, msg);

	/* In case of a column, convert it to one raw */
	if ( (ret == DWPAL_SUCCESS) && (isEndFieldNameAllocated) )
	{
		if (columnOfParamsToRowConvert(msg, msgLen , endFieldName) == false)
		{
			console_printf("%s; columnOfParamsToRowConvert error ==> Abort!\n", __FUNCTION__);
			ret = DWPAL_FAILURE;
		}
	}

	//console_printf("%s; After columnOfParamsToRowConvert; msg= '%s'\n", __FUNCTION__, msg);

	/* Perform the actual parsing */
	lineMsg = strtok_s(msg, &dmaxLen, "\n", &p2str);
	localMsg = lineMsg;
	lineIdx = 0;

	while ( (lineMsg != NULL) && (ret == DWPAL_SUCCESS) )
	{
		void *field;
		char *localMsgDup = NULL;

		//console_printf("%s; lineMsg= '%s'\n", __FUNCTION__, lineMsg);

		i = 0;
		while ( (fieldsToParse[i].parsingType != DWPAL_NUM_OF_PARSING_TYPES) && (ret == DWPAL_SUCCESS) )
		{
			if (fieldsToParse[i].field == NULL)
			{
				field = NULL;
			}
			else
			{
				if (lineIdx * sizeOfStruct >= userBufLen)
				{
					console_printf("%s; user did not allocate enough buffer for receiving all lines ==> Abort!\n", __FUNCTION__);
					ret = DWPAL_FAILURE;
					break;
				}
				/* set the output parameter - move it to the next array index (needed when parsing many lines) */
				field = (void *)((char*)fieldsToParse[i].field + (lineIdx * sizeOfStruct));
				//console_printf("%s; lineIdx= %d, sizeOfStruct= %d, field= 0x%x\n", __FUNCTION__, lineIdx, sizeOfStruct, (unsigned int)field);
			}

			switch (fieldsToParse[i].parsingType)
			{
				case DWPAL_STR_PARAM:
					if (fieldsToParse[i].stringToSearch == NULL)
					{  /* Handle mandatory parameters WITHOUT any string-prefix */
						if (localMsg != NULL)
						{
							localMsgDup = strdup(localMsg);
							if (localMsgDup == NULL)
							{
								console_printf("%s; localMsgDup is NULL, Failed strdup ==> Abort!\n", __FUNCTION__);
								ret = DWPAL_FAILURE;
								break;
							}
						}

						dmaxLenMandatory = (rsize_t)strnlen_s(lineMsg, HOSTAPD_TO_DWPAL_MSG_LENGTH);
						if (mandatoryFieldValueGet(((localMsg != NULL)? localMsgDup : NULL) /*will be NULL starting from 2nd param*/,
						                           &dmaxLenMandatory,
						                           &p2strMandatory,
						                           (int)fieldsToParse[i].totalSizeOfArg,
						                           (char *)field /*OUT*/) == false)
						{
							console_printf("%s; mandatory is NULL ==> Abort!\n", __FUNCTION__);
							ret = DWPAL_FAILURE;  /* mandatory parameter is missing ==> Abort! */
						}
						else if (fieldsToParse[i].numOfValidArgs != NULL)
						{
							(*(fieldsToParse[i].numOfValidArgs))++;
						}

						localMsg = NULL;  /* for 2nd, 3rd, ... parameter */
					}
					else
					{
						if (isEndFieldNameAllocated == false)
						{
							console_printf("%s; DWPAL_STR_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
							ret = DWPAL_FAILURE;
							break;
						}

						if (field == NULL)
						{
							console_printf("%s; DWPAL_STR_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
						}
						else
						{
							memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
							if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								if ((strnlen_s(stringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1) > fieldsToParse[i].totalSizeOfArg)
								{
									console_printf("%s; string length (%d) is bigger the allocated string size (%d)\n",
												__FUNCTION__, strnlen_s(stringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1, fieldsToParse[i].totalSizeOfArg);
									ret = DWPAL_FAILURE;  /* longer string then allocated ==> Abort! */
								}
								else
								{
									strcpy_s((char *)field, strnlen_s(stringOfValues, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH) + 1, stringOfValues);
								}
							}
							else
							{
								isMissingParam = true;
							}
						}
					}
					break;

				case DWPAL_STR_ARRAY_PARAM:
				/* handle multiple repetitive field, for example:
				   "... non_pref_chan=81:200:1:5 non_pref_chan=81:100:2:9 non_pref_chan=81:200:1:7 non_pref_chan=81:100:2:5 ..." or
				   "... non_pref_chan=81:200:1:5 81:100:2:9 81:200:1:7 81:100:2:5 ..." */
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_STR_ARRAY_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_STR_ARRAY_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (arrayValuesGet(stringOfValues, fieldsToParse[i].totalSizeOfArg, DWPAL_STR_ARRAY_PARAM, fieldsToParse[i].numOfValidArgs, (char *)field) == false)
							{
								console_printf("%s; arrayValuesGet ERROR\n", __FUNCTION__);
							}
						}
						else
						{
							isMissingParam = true;
						}

						if ( (fieldsToParse[i].numOfValidArgs != NULL) && (*(fieldsToParse[i].numOfValidArgs) == 0) )
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_CHAR_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_CHAR_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_CHAR_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(char *)field = (char)atoi(stringOfValues);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(char *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_UNSIGNED_CHAR_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_UNSIGNED_CHAR_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_UNSIGNED_CHAR_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(unsigned char *)field = (unsigned char)atoi(stringOfValues);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(unsigned char *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_SHORT_INT_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_SHORT_INT_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_SHORT_INT_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(short int *)field = (short int)atoi(stringOfValues);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(short int *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_INT_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_INT_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_INT_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(int *)field = atoi(stringOfValues);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(int *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_UNSIGNED_INT_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_UNSIGNED_INT_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_UNSIGNED_INT_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(unsigned int *)field = strtoul(stringOfValues, NULL, 10);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(unsigned int *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_LONG_LONG_INT_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_LONG_LONG_INT_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_LONG_LONG_INT_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(long long int *)field = atoll(stringOfValues);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(long long int *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_UNSIGNED_LONG_LONG_INT_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_UNSIGNED_LONG_LONG_INT_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_UNSIGNED_LONG_LONG_INT_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (strncmp(stringOfValues, "UNKNOWN", sizeof("UNKNOWN") - 1))
							{
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									(*(fieldsToParse[i].numOfValidArgs))++;
								}

								*(unsigned long long int *)field = strtoull(stringOfValues, NULL, 10);
							}
							else
							{  /* In case that the return value is "UNKNOWN", set isValid to 'false' and value to '0' */
								if (fieldsToParse[i].numOfValidArgs != NULL)
								{
									*(fieldsToParse[i].numOfValidArgs) = 0;
								}

								*(unsigned long long int *)field = 0;
								isMissingParam = true;
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_INT_ARRAY_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_INT_ARRAY_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_INT_ARRAY_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							//console_printf("%s; Before arrayValuesGet(); fieldsToParse[%d].numOfValidArgs= %d, stringOfValues= '%s'\n", __FUNCTION__, i, *(fieldsToParse[i].numOfValidArgs), stringOfValues);
							if (arrayValuesGet(stringOfValues, fieldsToParse[i].totalSizeOfArg, DWPAL_INT_ARRAY_PARAM, fieldsToParse[i].numOfValidArgs, field) == false)
							{
								console_printf("%s; arrayValuesGet ERROR\n", __FUNCTION__);
							}
							//console_printf("%s; After arrayValuesGet(); fieldsToParse[%d].numOfValidArgs= %d\n", __FUNCTION__, i, *(fieldsToParse[i].numOfValidArgs));
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_INT_HEX_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_INT_HEX_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_INT_HEX_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (fieldsToParse[i].numOfValidArgs != NULL)
							{
								(*(fieldsToParse[i].numOfValidArgs))++;
							}

							*((int *)field) = strtol(stringOfValues, NULL, 16);
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_INT_HEX_ARRAY_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_INT_HEX_ARRAY_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_INT_HEX_ARRAY_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (arrayValuesGet(stringOfValues, fieldsToParse[i].totalSizeOfArg, DWPAL_INT_HEX_ARRAY_PARAM, fieldsToParse[i].numOfValidArgs, field) == false)
							{
								console_printf("%s; arrayValuesGet (stringToSearch= '%s') ERROR ==> Abort!\n", __FUNCTION__, fieldsToParse[i].stringToSearch);
								ret = DWPAL_FAILURE; /* array of string detected, but getting its arguments failed ==> Abort! */
							}
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_BOOL_PARAM:
					if (isEndFieldNameAllocated == false)
					{
						console_printf("%s; DWPAL_BOOL_PARAM; isEndFieldNameAllocated=false ==> Abort!\n", __FUNCTION__);
						ret = DWPAL_FAILURE;
						break;
					}

					if (field == NULL)
					{
						console_printf("%s; DWPAL_BOOL_PARAM; fieldsToParse[%d].field=NULL ==> cont...\n", __FUNCTION__, i);
					}
					else
					{
						memset(stringOfValues, 0, HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH);  /* reset the string value array */
						if (fieldValuesGet(lineMsg, msgLen, fieldsToParse[i].parsingType, fieldsToParse[i].stringToSearch, endFieldName, stringOfValues) == true)
						{
							if (fieldsToParse[i].numOfValidArgs != NULL)
							{
								(*(fieldsToParse[i].numOfValidArgs))++;
							}

							*((bool *)field) = atoi(stringOfValues);
						}
						else
						{
							isMissingParam = true;
						}
					}
					break;

				case DWPAL_DUMMY_PARAM:
					break;

				default:
					console_printf("%s; (parsingType= %d) ERROR ==> Abort!\n", __FUNCTION__, fieldsToParse[i].parsingType);
					ret = DWPAL_FAILURE;
					break;
			}

			i++;
		}

		if (localMsgDup != NULL)
		{
			free((void *)localMsgDup);
		}

		lineMsg = strtok_s(NULL, &dmaxLen, "\n", &p2str);
		lineIdx++;
		localMsg = lineMsg;
	}

	/* free the allocated string array (if needed) */
	for (i=0; i < numOfNameArrayArgs; i++)
	{
		if ( (endFieldName != NULL) && (isEndFieldNameAllocated) && (endFieldName[i] != NULL) )
			free((void *)endFieldName[i]);
	}

	if (endFieldName != NULL)
	{
		free((void *)endFieldName);
	}

	free((void *)stringOfValues);

	if (ret != DWPAL_FAILURE)
	{
		if (isMissingParam)
		{
			ret = DWPAL_SUCCESS;  // was DWPAL_MISSING_PARAM
		}
	}

	return ret;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_cmd_send(void *context, const char *cmdHeader, FieldsToCmdParse *fieldsToCmdParse, char *reply, size_t *replyLen)
 **************************************************************************
 *  \brief Build and send hostap command
 *  \param[in] void *context - Provides all the interface information
 *  \param[in] const char *cmdHeader - The beginning of the hostap command string
 *  \param[in] FieldsToCmdParse *fieldsToCmdParse - The command parsing information, in which accordingly, the command string (after the header) will be created
 *  \param[out] char *reply - The output string returning from the hostap command
 *  \param[in,out] size_t *replyLen - Provide the max output string length, and get back the actual string length
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_cmd_send(void *context, const char *cmdHeader, FieldsToCmdParse *fieldsToCmdParse, char *reply /*OUT*/, size_t *replyLen /*IN/OUT*/)
{
	int  i;
	int  ret;
	char cmd[DWPAL_TO_HOSTAPD_MSG_LENGTH_INTERNAL];
	char buf[DWPAL_TO_HOSTAPD_MSG_LENGTH_INTERNAL];
	int	 status;
	size_t reply_len_tmp;
	DWPAL_Context *localContext = (DWPAL_Context *)context;

	if ( (localContext == NULL) || (cmdHeader == NULL) || (reply == NULL) || (replyLen == NULL) || (*replyLen == 0) )
	{
		console_printf("%s; input params error ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext->interface.hostapd.wpaCtrlPtr == NULL)
	{
		console_printf("%s; input params error (wpaCtrlPtr = NULL) ==> Abort!\n", __FUNCTION__);
		*replyLen = 0;
		return DWPAL_FAILURE;
	}

	//console_printf("%s Entry; VAPName= '%s', cmdHeader= '%s', replyLen= %d\n", __FUNCTION__, localContext->interface.hostapd.VAPName, cmdHeader, *replyLen);

	status = sprintf_s(cmd, sizeof(cmd), "%s", cmdHeader);
	if (status <= 0)
	{
		console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
		*replyLen = 0;
		return DWPAL_FAILURE;
	}

	if (fieldsToCmdParse != NULL)
	{
		i = 0;
		while (fieldsToCmdParse[i].parsingType != DWPAL_NUM_OF_PARSING_TYPES)
		{
			if (fieldsToCmdParse[i].field != NULL)
			{
				switch (fieldsToCmdParse[i].parsingType)
				{
					case DWPAL_STR_PARAM:
						//console_printf("%s; fieldsToCmdParse[%d].field= '%s'\n", __FUNCTION__, i, (char *)fieldsToCmdParse[i].field);
						if (fieldsToCmdParse[i].preParamString == NULL)
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %s", buf, (char *)fieldsToCmdParse[i].field);
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						else
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %s%s", buf, fieldsToCmdParse[i].preParamString, (char *)fieldsToCmdParse[i].field);
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						break;

					case DWPAL_STR_ARRAY_PARAM:
						break;

					case DWPAL_INT_PARAM:
						//console_printf("%s; fieldsToCmdParse[%d].field= %d\n", __FUNCTION__, i, *((int *)fieldsToCmdParse[i].field));
						if (fieldsToCmdParse[i].preParamString == NULL)
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %d", buf, *((int *)fieldsToCmdParse[i].field));
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						else
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %s%d", buf, fieldsToCmdParse[i].preParamString, *((int *)fieldsToCmdParse[i].field));
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						break;

					case DWPAL_UNSIGNED_INT_PARAM:
						//console_printf("%s; fieldsToCmdParse[%d].field= %u\n", __FUNCTION__, i, *((unsigned int *)fieldsToCmdParse[i].field));
						if (fieldsToCmdParse[i].preParamString == NULL)
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %u", buf, *((unsigned int *)fieldsToCmdParse[i].field));
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						else
						{
							strcpy_s(buf, sizeof(buf), cmd);
							status = sprintf_s(cmd, sizeof(cmd), "%s %s%u", buf, fieldsToCmdParse[i].preParamString, *((unsigned int *)fieldsToCmdParse[i].field));
							if (status <= 0)
							{
								console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
								*replyLen = 0;
								return DWPAL_FAILURE;
							}
						}
						break;

					case DWPAL_CHAR_PARAM:
					case DWPAL_UNSIGNED_CHAR_PARAM:
					case DWPAL_SHORT_INT_PARAM:
					case DWPAL_LONG_LONG_INT_PARAM:
					case DWPAL_UNSIGNED_LONG_LONG_INT_PARAM:
					case DWPAL_INT_ARRAY_PARAM:
					case DWPAL_INT_HEX_ARRAY_PARAM:
						break;

					case DWPAL_BOOL_PARAM:
						break;

					case DWPAL_DUMMY_PARAM:
						break;

					default:
						console_printf("%s; (parsingType= %d) ERROR ==> Abort!\n", __FUNCTION__, fieldsToCmdParse[i].parsingType);
						*replyLen = 0;
						return DWPAL_FAILURE;
						break;
				}
			}

			i++;
		}
	}

	//console_printf("%s; cmd= '%s'\n", __FUNCTION__, cmd);

	memset((void *)reply, '\0', *replyLen);  /* Clear the output buffer */

	if (localContext->interface.hostapd.wpaCtrlEventCallback == NULL)
	{
		/* non-valid wpaCtrlEventCallback states that this is a one-way connection */
		while (wpa_ctrl_pending(localContext->interface.hostapd.wpaCtrlPtr))
		{
			/* clear this message still stuck in the socket, probably becuase hostapd
			 * took too long to answer a previous request made by us.
			 * if not cleared, this message will be the response for this wpa_ctrl_request call */
			size_t buffLen = HOSTAPD_TO_DWPAL_MSG_LENGTH * sizeof(char);
			char *buff = (char *)malloc((size_t)buffLen);

			console_printf("%s; socket has pending messages\n", __FUNCTION__);

			if (buff == NULL)
			{
				console_printf("%s; malloc Failed ==> Abort!\n", __FUNCTION__);
				return DWPAL_FAILURE;
			}

			memset((void *)buff, '\0', buffLen);
			buffLen -= 1 * sizeof(char);
			ret = wpa_ctrl_recv(localContext->interface.hostapd.wpaCtrlPtr,
					    buff, &buffLen);
			free((void *)buff);
			if (ret < 0)
			{
				console_printf("%s; wpa_ctrl_recv() returned ERROR ==> Abort!\n", __FUNCTION__);
				return DWPAL_FAILURE;
			}
		}
	}

	reply_len_tmp = *replyLen;
	ret = wpa_ctrl_request(localContext->interface.hostapd.wpaCtrlPtr,
	                       cmd,
						   strnlen_s(cmd, sizeof(cmd)),
						   reply,
						   &reply_len_tmp /* should be msg-len in/out param */,
						   localContext->interface.hostapd.wpaCtrlEventCallback);
	if (ret < 0)
	{
		console_printf("%s; wpa_ctrl_request() returned error; VAPName= '%s' (ret= %d) ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName, ret);
		*replyLen = 0;
		return DWPAL_FAILURE;
	}

	if (reply_len_tmp >= *replyLen)
	{
		console_printf("%s; reply_len_tmp(%zu) >= replyLen(%zu) ==> some data may be lost!\n", __FUNCTION__, reply_len_tmp, *replyLen);
		reply_len_tmp = *replyLen - 1;
	}

	/* we need it to clear the "junk" at the end of the string */
	reply[reply_len_tmp] = '\0';
	*replyLen = reply_len_tmp;

	if (reply[0] == '\0')
	{  /* The 'reply' string is empty */
		*replyLen = 0;
	}
	//console_printf("%s; replyLen= %d\nreply=\n%s\n", __FUNCTION__, *replyLen, reply);

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_event_get(void *context, char *msg , size_t *msgLen, char *opCode)
 **************************************************************************
 *  \brief Will get the complete event and op-code (after internal parsing) from the hostapd (requested event was via fd get)
 *  \param[in] void *context - Provides all the interface information
 *  \param[out] char *msg - the complete event buffer received from hostapd
 *  \param[in,out] size_t *msgLen - input is buffer size, output is the actual event buffer length copied
 *  \param[out] char *opCode - output the parsed event opcode 
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_event_get(void *context, char *msg /*OUT*/, size_t *msgLen /*IN/OUT*/, char *opCode /*OUT*/)
{
	int     ret;
	char    *localOpCode;
	rsize_t dmaxLen;
	char    *localMsg, *p2str;
	struct  wpa_ctrl *wpaCtrlPtr = NULL;

	if ( (context == NULL) || (msg == NULL) || (msgLen == NULL) || (opCode == NULL) )
	{
		console_printf("%s; context/msg/msgLen/opCode is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	wpaCtrlPtr = (((DWPAL_Context *)context)->interface.hostapd.wpaCtrlEventCallback == NULL)?
	             /* one-way*/ ((DWPAL_Context *)context)->interface.hostapd.listenerWpaCtrlPtr :
	             /* two-way*/ ((DWPAL_Context *)context)->interface.hostapd.wpaCtrlPtr;

	if (wpaCtrlPtr == NULL)
	{
		console_printf("%s; wpaCtrlPtr= NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* In order to get ALL pending messages (and return the last one), all of the below should be inside "while" loop */
	ret = wpa_ctrl_pending(wpaCtrlPtr);
	switch (ret)
	{
		case -1:  /* error */
			console_printf("%s; wpa_ctrl_pending() returned ERROR ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
			break;

		case 0:  /* there are no pending messages */
			return DWPAL_NO_PENDING_MESSAGES;
			break;

		case 1:  /* there are pending messages */
			break;

		default:
			console_printf("%s; wpa_ctrl_pending() returned unknown (%d) value ==> Abort!\n", __FUNCTION__, ret);
			return DWPAL_FAILURE;
			break;
	}

	/* There are pending messages */
	if (wpa_ctrl_recv(wpaCtrlPtr, msg, msgLen) == 0)
	{
		//console_printf("%s; msgLen= %d\nmsg= '%s'\n", __FUNCTION__, *msgLen, msg);
		msg[*msgLen] = '\0';
		if (*msgLen <= 5)
		{
			console_printf("%s; '%s' is NOT a report ==> Abort!\n", __FUNCTION__, msg);
			return DWPAL_FAILURE;
		}
		else
		{
			dmaxLen = (rsize_t)*msgLen;
			localMsg = strdup(msg);
			localOpCode = strtok_s(localMsg, &dmaxLen, ">", &p2str);
			localOpCode = strtok_s(NULL, &dmaxLen, " ", &p2str);
			strcpy_s(opCode, strnlen_s(localOpCode, DWPAL_OPCODE_STRING_LENGTH) + 1, localOpCode);
			free((void *)localMsg);
		}
	}
	else
	{
		console_printf("%s; wpa_ctrl_recv() returned ERROR ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_event_fd_get(void *context, int *fd)
 **************************************************************************
 *  \brief supply the specific hostapd event file descriptor to listen to
 *  \param[in] void *context - Provides all the interface information
 *  \param[out] int *fd - Provides the interfaces fd value
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_event_fd_get(void *context, int *fd /*OUT*/)
{
	if ( (context == NULL) || (fd == NULL) )
	{
		//console_printf("%s; context and/or fd is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	*fd = ((DWPAL_Context *)context)->interface.hostapd.fd;

	if (*fd == (-1))
	{
		//console_printf("%s; fd value is (-1) ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_socket_close(void **context)
 **************************************************************************
 *  \brief close hostapd socket/s
 *  \param[in] void *context - Provides all the interface information
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_socket_close(void **context)
{
	int           ret;
	DWPAL_Context *localContext;

	console_printf("%s Entry\n", __FUNCTION__);

	if (context == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	localContext = (DWPAL_Context *)(*context);
	if (localContext == NULL)
	{
		console_printf("%s; localContext is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext->interface.hostapd.wpaCtrlPtr == NULL)
	{
		console_printf("%s; wpaCtrlPtr is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Close 'wpaCtrlPtr' */
	console_printf("%s; close wpaCtrlPtr, VAPName= '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
	wpa_ctrl_close(localContext->interface.hostapd.wpaCtrlPtr);

	if (localContext->interface.hostapd.wpaCtrlEventCallback == NULL)
	{  /* non-valid wpaCtrlEventCallback states that this is a one-way connection ==> turn off the event listener in an additional socket */
		if (localContext->interface.hostapd.listenerWpaCtrlPtr == NULL)
		{
			console_printf("%s; listenerWpaCtrlPtr is NULL ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
		}

		/* Detach & Close 'listenerWpaCtrlPtr' */
		console_printf("%s; detach listenerWpaCtrlPtr\n", __FUNCTION__);
		if ((ret = wpa_ctrl_detach(localContext->interface.hostapd.listenerWpaCtrlPtr)) != 0)
		{
			console_printf("%s; wpa_ctrl_detach 'listenerWpaCtrlPtr' (VAPName= '%s') returned ERROR (ret= %d) ==> cont...\n",
						   __FUNCTION__, localContext->interface.hostapd.VAPName, ret);
		}
		console_printf("%s; close listenerWpaCtrlPtr\n", __FUNCTION__);
		wpa_ctrl_close(localContext->interface.hostapd.listenerWpaCtrlPtr);
	}

	/* free 'context' */
	free(*context);
	*context = NULL;

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_is_socket_alive(void *context, bool *isExist)
 **************************************************************************
 *  \brief does the hostapd interface exist?
 *  \param[in] void *context - Provides all the interface information
 *  \param[out] bool *isExist - Provides the answer, exists or not
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_is_socket_alive(void *context, bool *isExist /*OUT*/)
{
	char   reply[HOSTAPD_TO_DWPAL_SHORT_REPLY_LENGTH];
	size_t replyLen = sizeof(reply) - 1;

	if ( (context == NULL) || (isExist == NULL) )
	{
		console_printf("%s; context and/or isExist is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	//console_printf("%s; VAPName= '%s'\n", __FUNCTION__, ((DWPAL_Context *)context)->interface.hostapd.VAPName);

	*isExist = false;

	if (((DWPAL_Context *)context)->interface.hostapd.VAPName[0] == '\0')
	{
		console_printf("%s; invalid radio name ('%s') ==> Abort!\n", __FUNCTION__, ((DWPAL_Context *)context)->interface.hostapd.VAPName);
		return DWPAL_FAILURE;
	}

	if (dwpal_hostap_cmd_send(context, "PING", NULL, reply, &replyLen) == DWPAL_SUCCESS)
	{
		//console_printf("%s; Interface '%s' exists\n", __FUNCTION__, ((DWPAL_Context *)context)->interface.hostapd.VAPName);
		*isExist = true;
	}
	else
	{
		console_printf("%s; Interface '%s' not present\n", __FUNCTION__, ((DWPAL_Context *)context)->interface.hostapd.VAPName);
		printf("%s; PING Failed! Interface '%s' not present\n", __FUNCTION__, ((DWPAL_Context *)context)->interface.hostapd.VAPName);
	}

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_interface_detach(void **context)
 **************************************************************************
 *  \brief Will reset and detach the interface towards/from the hostapd/supplicant of the requested radio interface
 *  \param[in,out] void **context - context should have wpa_ctrl pointer provided as input parameter. parameters will be updated to NULL as output parameters
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_interface_detach(void **context /*IN/OUT*/)
{
	DWPAL_Context *localContext;
	int           ret;

	console_printf("%s Entry\n", __FUNCTION__);

	if (context == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	localContext = (DWPAL_Context *)(*context);
	if (localContext == NULL)
	{
		console_printf("%s; localContext is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext->interface.hostapd.wpaCtrlPtr == NULL)
	{
		console_printf("%s; wpaCtrlPtr= NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (localContext->interface.hostapd.wpaCtrlEventCallback != NULL)
	{  /* Valid wpaCtrlEventCallback states that this is a two-way connection (for both command and events) */
		if ((ret = wpa_ctrl_detach(localContext->interface.hostapd.wpaCtrlPtr)) != 0)
		{
			console_printf("%s; wpa_ctrl_detach 'listenerWpaCtrlPtr' (VAPName= '%s') returned ERROR (ret= %d) ==> cont...\n",
						   __FUNCTION__, localContext->interface.hostapd.VAPName, ret);
		}
	}
	else
	{  /* non-valid wpaCtrlEventCallback states that this is a one-way connection */
		/* Close & reset 'listenerWpaCtrlPtr' */
		if (localContext->interface.hostapd.listenerWpaCtrlPtr == NULL)
		{
			console_printf("%s; listenerWpaCtrlPtr= NULL ==> Abort!\n", __FUNCTION__);
			return DWPAL_FAILURE;
		}

		console_printf("%s; call wpa_ctrl_detach() listenerWpaCtrlPtr; VAPName= '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
		if ((ret = wpa_ctrl_detach(localContext->interface.hostapd.listenerWpaCtrlPtr)) != 0)
		{
			console_printf("%s; wpa_ctrl_detach 'listenerWpaCtrlPtr' (VAPName= '%s') returned ERROR (ret= %d) ==> cont...\n",
						   __FUNCTION__, localContext->interface.hostapd.VAPName, ret);
		}

		console_printf("%s; call wpa_ctrl_close() listenerWpaCtrlPtr; VAPName= '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
		wpa_ctrl_close(localContext->interface.hostapd.listenerWpaCtrlPtr);
	}

	/* Close 'wpaCtrlPtr' */
	console_printf("%s; call wpa_ctrl_close() wpaCtrlPtr; VAPName= '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
	wpa_ctrl_close(localContext->interface.hostapd.wpaCtrlPtr);

	free(*context);
	*context = NULL;

	return DWPAL_SUCCESS;
}


/**************************************************************************/
/*! \fn DWPAL_Ret dwpal_hostap_interface_attach(void **context, const char *VAPName, DWPAL_wpaCtrlEventCallback wpaCtrlEventCallback)
 **************************************************************************
 *  \brief Will set the interface towards/from the hostapd/supplicant of the requested radio interface
 *  \param[out] void **context - context should have wpa_ctrl pointer provided as input parameter. parameters will be updated to NULL as output parameters
 *  \param[in] const char *VAPName - The radio/VAP name
 *  \param[in] DWPAL_wpaCtrlEventCallback wpaCtrlEventCallback - Callback function to be used while event is received during command send, when using the same socket for command/event. When using ‘NULL’, it means that the interface will use one socket for the command, and one for the event
 *  \return DWPAL_Ret (DWPAL_SUCCESS for success, other for failure)
 ***************************************************************************/
DWPAL_Ret dwpal_hostap_interface_attach(void **context /*OUT*/, const char *VAPName, DWPAL_wpaCtrlEventCallback wpaCtrlEventCallback)
{
	DWPAL_Context *localContext;
	char          wpaCtrlName[DWPAL_WPA_CTRL_STRING_LENGTH];
	int	status;

	console_printf("%s; VAPName= '%s', wpaCtrlEventCallback= %p\n", __FUNCTION__, VAPName, (void *)wpaCtrlEventCallback);

	if (context == NULL)
	{
		console_printf("%s; context is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	if (VAPName == NULL)
	{
		console_printf("%s; VAPName is NULL ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	/* Temporary due to two-way socket hostapd bug */
	if (wpaCtrlEventCallback != NULL)
	{  /* Valid wpaCtrlEventCallback states that this is a two-way connection (for both command and events) */
		console_printf("%s; currently, two-way connection (for '%s') is NOT supported - use one-way connection ==> Abort!\n", __FUNCTION__, VAPName);
		return DWPAL_FAILURE;
	}

	*context = malloc(sizeof(DWPAL_Context));
	if (*context == NULL)
	{
		console_printf("%s; malloc for context failed ==> Abort!\n", __FUNCTION__);
		return DWPAL_FAILURE;
	}

	localContext = (DWPAL_Context *)(*context);
	memset(localContext, 0, sizeof(DWPAL_Context));

	strncpy_s((void *)(localContext->interface.hostapd.VAPName), DWPAL_VAP_NAME_STRING_LENGTH, VAPName, strnlen_s(VAPName, DWPAL_VAP_NAME_STRING_LENGTH));
	localContext->interface.hostapd.VAPName[sizeof(localContext->interface.hostapd.VAPName) - 1] = '\0';
	localContext->interface.hostapd.fd = -1;
	localContext->interface.hostapd.wpaCtrlPtr = NULL;
	localContext->interface.hostapd.wpaCtrlEventCallback = wpaCtrlEventCallback;

	/* check if '/var/run/hostapd/wlanX' or '/var/run/wpa_supplicant/wlanX' exists, and update context's database */
	status = sprintf_s(wpaCtrlName, DWPAL_WPA_CTRL_STRING_LENGTH, "%s%s", "/var/run/hostapd/", localContext->interface.hostapd.VAPName);
	if (status <= 0)
	{
		console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
		free(*context);
		*context = NULL;
		return DWPAL_FAILURE;
	}
	if (access(wpaCtrlName, F_OK) == 0)
	{
		//console_printf("%s; Radio '%s' exists - AP Mode\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
		strcpy_s(localContext->interface.hostapd.operationMode, 3, "AP");
		strcpy_s(localContext->interface.hostapd.wpaCtrlName, strnlen_s(wpaCtrlName, DWPAL_WPA_CTRL_STRING_LENGTH) + 1, wpaCtrlName);
	}
	else
	{
		status = sprintf_s(wpaCtrlName, DWPAL_WPA_CTRL_STRING_LENGTH, "%s%s", "/var/run/wpa_supplicant/", localContext->interface.hostapd.VAPName);
		if (status <= 0)
		{
			console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
			free(*context);
			*context = NULL;
			return DWPAL_FAILURE;
		}
		if (access(wpaCtrlName, F_OK) == 0)
		{
			//console_printf("%s; Radio '%s' exists - STA Mode\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
			strcpy_s(localContext->interface.hostapd.operationMode, 4, "STA");
			strcpy_s(localContext->interface.hostapd.wpaCtrlName, strnlen_s(wpaCtrlName, DWPAL_WPA_CTRL_STRING_LENGTH) + 1, wpaCtrlName);
		}
		else
		{
			localContext->interface.hostapd.operationMode[0] = '\0';
			localContext->interface.hostapd.wpaCtrlName[0] = '\0';

			//console_printf("%s; radio interface '%s' not present ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

			/* free 'context' */
			free(*context);
			*context = NULL;

			return DWPAL_FAILURE;
		}
	}

	console_printf("%s; wpa_ctrl_open wpaCtrlPtr (for interface '%s')\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
	localContext->interface.hostapd.wpaCtrlPtr = wpa_ctrl_open(localContext->interface.hostapd.wpaCtrlName);
	if (localContext->interface.hostapd.wpaCtrlPtr == NULL)
	{
		console_printf("%s; wpaCtrlPtr (for interface '%s') is NULL! ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

		/* free 'context' */
		free(*context);
		*context = NULL;

		return DWPAL_FAILURE;
	}

	if (localContext->interface.hostapd.wpaCtrlEventCallback != NULL)
	{  /* Valid wpaCtrlEventCallback states that this is a two-way connection (for both command and events) */
		console_printf("%s; set up two-way connection for '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

		/* Reset listenerWpaCtrlPtr which used only in one-way connection */
		localContext->interface.hostapd.listenerWpaCtrlPtr = NULL;

		if (wpa_ctrl_attach(localContext->interface.hostapd.wpaCtrlPtr) != 0)
		{
			console_printf("%s; wpa_ctrl_open ok BUT wpa_ctrl_attach FAILED!!! VAPName= '%s' ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

			/* Close 'wpaCtrlPtr' and free 'context' */
			wpa_ctrl_close(localContext->interface.hostapd.wpaCtrlPtr);
			free(*context);
			*context = NULL;

			return DWPAL_FAILURE;
		}

		localContext->interface.hostapd.fd = wpa_ctrl_get_fd(localContext->interface.hostapd.wpaCtrlPtr);
	}
	else
	{  /* non-valid wpaCtrlEventCallback states that this is a one-way connection ==> turn on the event listener in an additional socket */
		console_printf("%s; wpa_ctrl_open listenerWpaCtrlPtr (for interface '%s')\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
		localContext->interface.hostapd.listenerWpaCtrlPtr = wpa_ctrl_open(localContext->interface.hostapd.wpaCtrlName);
		console_printf("%s; set up one-way connection for '%s'\n", __FUNCTION__, localContext->interface.hostapd.VAPName);
		if (localContext->interface.hostapd.listenerWpaCtrlPtr == NULL)
		{
			console_printf("%s; listenerWpaCtrlPtr (for interface '%s') is NULL! ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

			/* Close 'wpaCtrlPtr' and free 'context' */
			wpa_ctrl_close(localContext->interface.hostapd.wpaCtrlPtr);
			free(*context);
			*context = NULL;

			return DWPAL_FAILURE;
		}

		if (wpa_ctrl_attach(localContext->interface.hostapd.listenerWpaCtrlPtr) != 0)
		{
			console_printf("%s; wpa_ctrl_open ok BUT wpa_ctrl_attach FAILED!!! VAPName= '%s' (listener) ==> Abort!\n", __FUNCTION__, localContext->interface.hostapd.VAPName);

			/* Close 'wpaCtrlPtr' & 'listenerWpaCtrlPtr' and free 'context' */
			wpa_ctrl_close(localContext->interface.hostapd.wpaCtrlPtr);
			wpa_ctrl_close(localContext->interface.hostapd.listenerWpaCtrlPtr);
			free(*context);
			*context = NULL;

			return DWPAL_FAILURE;
		}

		localContext->interface.hostapd.fd = wpa_ctrl_get_fd(localContext->interface.hostapd.listenerWpaCtrlPtr);
	}

	return DWPAL_SUCCESS;
}
