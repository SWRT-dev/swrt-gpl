/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DWPAL_EXT_H_
#define __DWPAL_EXT_H_


#include "dwpal.h"

#define NUM_OF_SUPPORTED_VAPS 32

#define cli_printf(...)       \
{                             \
	if (isCliPrintf)          \
		printf(__VA_ARGS__);  \
}


typedef int (*DwpalExtHostapEventCallback)(char *VAPName, char *opCode, char *msg, size_t msgStringLen);
typedef DWPAL_nlVendorEventCallback DwpalExtNlEventCallback;  /* DWPAL_Ret DWPAL_nlVendorEventCallback(size_t len, unsigned char *data); */
typedef DWPAL_nlNonVendorEventCallback DwpalExtNlNonVendorEventCallback;


/* APIs */
DWPAL_Ret dwpal_ext_driver_nl_scan_dump(char *ifname, DWPAL_nlNonVendorEventCallback nlEventCallback); /* deprecated */
DWPAL_Ret dwpal_ext_driver_nl_scan_trigger(char *ifname, ScanParams *scanParams); /* deprecated */
DWPAL_Ret dwpal_ext_driver_nl_get(char *ifname, unsigned int nl80211Command, CmdIdType cmdIdType, unsigned int subCommand, unsigned char *vendorData, size_t vendorDataSize, size_t *outLen, unsigned char *outData);
DWPAL_Ret dwpal_ext_driver_nl_cmd_send(char *ifname, unsigned int nl80211Command, CmdIdType cmdIdType, unsigned int subCommand, unsigned char *vendorData, size_t vendorDataSize);
DWPAL_Ret dwpal_ext_nl80211_cmd_send(struct nl_msg *msg, int *res /*OUT*/, DWPAL_nl80211Callback nlCallback, void *cb_arg, bool lock_cmd);
DWPAL_Ret dwpal_ext_driver_nl_scan_dump_sync(char *ifname, int *cmd_res /*OUT*/, DWPAL_nl80211Callback nlCallback, void *cb_arg, bool lock_cmd);
DWPAL_Ret dwpal_ext_driver_nl_scan_trigger_sync(char *ifname, int *cmd_res /*OUT*/, ScanParams *scanParams, bool lock_cmd);
DWPAL_Ret dwpal_ext_nl80211_id_get(int *nl80211_id /*OUT*/);
DWPAL_Ret dwpal_ext_driver_nl_detach(void);
DWPAL_Ret dwpal_ext_driver_nl_attach(DwpalExtNlEventCallback nlEventCallback, DwpalExtNlNonVendorEventCallback nlNonVendorEventCallback);

DWPAL_Ret dwpal_ext_hostap_cmd_send(char *VAPName, char *cmdHeader, FieldsToCmdParse *fieldsToCmdParse, char *reply /*OUT*/, size_t *replyLen /*IN/OUT*/);
DWPAL_Ret dwpal_ext_hostap_interface_detach(char *VAPName);
DWPAL_Ret dwpal_ext_hostap_interface_attach(char *VAPName, DwpalExtHostapEventCallback eventCallback);
DWPAL_Ret dwpal_ext_interfaceIndexGet(char *interfaceType, char *VAPName, int *idx);

#endif  //__DWPAL_EXT_H_
