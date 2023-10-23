/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "unitest_helper.h"
#include "dwpal_ext.h"

static int empty_dwpal_ext_hostap_event_callback(char *VAPName, char *opCode, char *msg, size_t msgStringLen)
{
	(void)VAPName;
	(void)opCode;
	(void)msg;
	(void)msgStringLen;

	return 0;
}

static DWPAL_Ret empty_DWPAL_nlVendorEventCallback(char *ifname, int event, int subevent, size_t len, unsigned char *data)
{
	(void)ifname;
	(void)event;
	(void)subevent;
	(void)len;
	(void)data;

	return 0;
}

static DWPAL_Ret empty_DWPAL_nlNonVendorEventCallback(struct nl_msg *msg)
{
	(void)msg;

	return 0;
}

UNIT_TEST_DEFINE(1, N * attach detach wlan0)
	DWPAL_Ret ret;
	int i;

	for (i = 0; i < 20; i++) {
		ret = dwpal_ext_hostap_interface_attach("wlan0", empty_dwpal_ext_hostap_event_callback);
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_detach("wlan0");
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);
	}

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, N * attach detach wlan6 - does not exist)
	DWPAL_Ret ret;
	int i;

	for (i = 0; i < 20; i++) {
		ret = dwpal_ext_hostap_interface_attach("wlan6", empty_dwpal_ext_hostap_event_callback);
		if (ret != DWPAL_INTERFACE_IS_DOWN)
			UNIT_TEST_FAILED("iface_attach returned err/ok (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_detach("wlan6");
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);
	}

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(3, N * attach detach nl)
	DWPAL_Ret ret;
	int i;

	for (i = 0; i < 20; i++) {
		ret = dwpal_ext_driver_nl_attach(empty_DWPAL_nlVendorEventCallback, empty_DWPAL_nlNonVendorEventCallback);
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_driver_nl_detach();
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);
	}

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(4, N * attach detach all combined)
	DWPAL_Ret ret;
	int i;

	for (i = 0; i < 15; i++) {
		ret = dwpal_ext_hostap_interface_attach("wlan0", empty_dwpal_ext_hostap_event_callback);
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_attach("wlan2", empty_dwpal_ext_hostap_event_callback);
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_attach("wlan6", empty_dwpal_ext_hostap_event_callback);
		if (ret != DWPAL_INTERFACE_IS_DOWN)
			UNIT_TEST_FAILED("iface_attach returned err/ok (%d) i=%d", ret, i);

		ret = dwpal_ext_driver_nl_attach(empty_DWPAL_nlVendorEventCallback, empty_DWPAL_nlNonVendorEventCallback);
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_detach("wlan0");
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_detach("wlan2");
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_hostap_interface_detach("wlan6");
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);

		ret = dwpal_ext_driver_nl_detach();
		if (ret != DWPAL_SUCCESS)
			UNIT_TEST_FAILED("iface_attach returned err (%d) i=%d", ret, i);
	}

UNIT_TEST_CLEANUP_ON_ERRR
	dwpal_ext_hostap_interface_detach("wlan0");
	dwpal_ext_hostap_interface_detach("wlan2");
	dwpal_ext_driver_nl_detach();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(dwpal_ext)
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
UNIT_TEST_MODULE_DEFINITION_DONE
