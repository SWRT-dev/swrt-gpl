/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "unitest_helper.h"
#include <stdio.h>
#include <string.h>

#include "dwpald_client.h"
#include "dwpal_ext.h"

static int __running_in_valgrind = 0;

typedef struct {
	int32_t passive;
	int32_t active;
	int32_t num_probe_reqs;
	int32_t probe_reqs_interval;
	int32_t num_chans_in_chunk;
	int32_t break_time;
	int32_t break_time_busy;
	uint32_t window_slice;
	uint32_t window_slice_overlap;
	uint32_t cts_to_self_duration;
} bg_scan_params_t;

#define INJECTED_EVENT_1_OP_CODE "INJECTED-EVENT-1-ZWDFT"
#define INJECTED_EVENT_2_OP_CODE "INJECTED-EVENT-2-AAA"
#define INJECTED_EVENT_3_OP_CODE "INJECTED-EVENT-3-TTTTTTTTTT"

#define INJECTED_EVENT_1_MSG "ZXCSSSEEEEEEEEEEEEEEEEEEAAAAAAAAAAAAAAAAAAAAAAAA" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS" \
			"AAAAAZXCSSSEEEEEEEEEEEEEEEEEEAAAAAAAAAAAAAAAAAAAAAAAA" \
			"AAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"

#define INJECTED_EVENT_2_MSG "INJECTED_EVENT_2_MSG AXXVVVDDDDDDDDDDDDDDDDDDDD"
#define INJECTED_EVENT_3_MSG "INJECTED_EVENT_3_MSG ASDSIIIIIIIIIII"

static unsigned int dwpal_ext_events_count[3] = { 0 };
static int dwpal_ext_events_cmp_err = 0;
static int dwpal_ext_events_order_err = 0;

static int dpal_ext_hostap_injected_event(char *ifname, char *op_code, char *msg, size_t len)
{
	const char *expected_msg = "";
	char expected_msg_buff[1024];
	int expected_len;
	unsigned int count_idx = 0;

	if (!strncmp(op_code, INJECTED_EVENT_1_OP_CODE, sizeof(INJECTED_EVENT_1_OP_CODE))) {
		expected_msg = INJECTED_EVENT_1_MSG;
		count_idx = 0;
		if (dwpal_ext_events_count[0] != dwpal_ext_events_count[1] || dwpal_ext_events_count[0] != dwpal_ext_events_count[2])
			dwpal_ext_events_order_err++;
	} else if (!strncmp(op_code, INJECTED_EVENT_2_OP_CODE, sizeof(INJECTED_EVENT_2_OP_CODE))) {
		expected_msg = INJECTED_EVENT_2_MSG;
		count_idx = 1;
		if (dwpal_ext_events_count[0] != dwpal_ext_events_count[1] + 1 || dwpal_ext_events_count[1] != dwpal_ext_events_count[2])
			dwpal_ext_events_order_err++;
	} else if (!strncmp(op_code, INJECTED_EVENT_3_OP_CODE, sizeof(INJECTED_EVENT_3_OP_CODE))) {
		expected_msg = INJECTED_EVENT_3_MSG;
		count_idx = 2;
		if (dwpal_ext_events_count[0] != dwpal_ext_events_count[1] || dwpal_ext_events_count[1] != dwpal_ext_events_count[2] + 1)
			dwpal_ext_events_order_err++;
	} else {
		return 0;
	}

	expected_len = snprintf(expected_msg_buff, sizeof(expected_msg_buff), "<3>%s %s %s", op_code, ifname, expected_msg);

	if ((int)len != expected_len) {
		ELOG("op_code '%s' wrong len %zu - expected %d", op_code, len, expected_len);
		dwpal_ext_events_cmp_err++;
		return 1;
	}
	if (strncmp(msg, expected_msg_buff, expected_len)) {
		ELOG("wrong msg");
		dwpal_ext_events_cmp_err++;
		return 1;
	}
	dwpal_ext_events_count[count_idx]++;
	return 0;
}

static unsigned int dwpald_events_count[3] = { 0 };
static int dwpald_events_cmp_err = 0;
static int dwpald_events_order_err = 0;
static int dwpald_event_slower_response = 0;

static int dpald_hostap_injected_event_1(char *ifname, char *op_code, char *msg, size_t len)
{
	char expected_msg_buff[1024];
	int expected_len;

	if (dwpald_event_slower_response)
		usleep(10000);

	if (strncmp(op_code, INJECTED_EVENT_1_OP_CODE, sizeof(INJECTED_EVENT_1_OP_CODE))) {
		ELOG("wrong op code: %s", op_code);
		dwpald_events_cmp_err++;
		return 1;
	}

	expected_len = snprintf(expected_msg_buff, sizeof(expected_msg_buff), "<3>%s %s %s", op_code, ifname, INJECTED_EVENT_1_MSG);

	if ((int)len != expected_len) {
		ELOG("wrong len %zu", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (msg[len] != '\0') {
		ELOG("msg at len=%zu is not 0", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (strncmp(msg, expected_msg_buff, expected_len)) {
		ELOG("wrong msg");
		dwpald_events_cmp_err++;
		return 1;
	}
	dwpald_events_count[0]++;

	if (dwpald_events_count[0] != dwpald_events_count[1] + 1 || dwpald_events_count[0] != dwpald_events_count[2] + 1)
		dwpald_events_order_err++;

	return 0;
}

static int dpald_hostap_injected_event_2(char *ifname, char *op_code, char *msg, size_t len)
{
	char expected_msg_buff[1024];
	int expected_len;

	if (dwpald_event_slower_response)
		usleep(10000);

	if (strncmp(op_code, INJECTED_EVENT_2_OP_CODE, sizeof(INJECTED_EVENT_2_OP_CODE))) {
		ELOG("wrong op code: %s", op_code);
		dwpald_events_cmp_err++;
		return 1;
	}

	expected_len = snprintf(expected_msg_buff, sizeof(expected_msg_buff), "<3>%s %s %s", op_code, ifname, INJECTED_EVENT_2_MSG);

	if ((int)len != expected_len) {
		ELOG("wrong len %zu", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (msg[len] != '\0') {
		ELOG("msg at len=%zu is not 0", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (strncmp(msg, expected_msg_buff, expected_len)) {
		ELOG("wrong msg");
		dwpald_events_cmp_err++;
		return 1;
	}
	dwpald_events_count[1]++;

	if (dwpald_events_count[0] != dwpald_events_count[1] || dwpald_events_count[1] != dwpald_events_count[2] + 1)
		dwpald_events_order_err++;

	return 0;
}

static int dpald_hostap_injected_event_3(char *ifname, char *op_code, char *msg, size_t len)
{
	char expected_msg_buff[1024];
	int expected_len;

	if (dwpald_event_slower_response)
		usleep(10000);

	if (strncmp(op_code, INJECTED_EVENT_3_OP_CODE, sizeof(INJECTED_EVENT_3_OP_CODE))) {
		ELOG("wrong op code: %s", op_code);
		dwpald_events_cmp_err++;
		return 1;
	}

	expected_len = snprintf(expected_msg_buff, sizeof(expected_msg_buff), "<3>%s %s %s", op_code, ifname, INJECTED_EVENT_3_MSG);

	if ((int)len != expected_len) {
		ELOG("wrong len %zu", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (msg[len] != '\0') {
		ELOG("msg at len=%zu is not 0", len);
		dwpald_events_cmp_err++;
		return 1;
	}

	if (strncmp(msg, expected_msg_buff, expected_len)) {
		ELOG("wrong msg");
		dwpald_events_cmp_err++;
		return 1;
	}
	dwpald_events_count[2]++;

	if (dwpald_events_count[0] != dwpald_events_count[1] || dwpald_events_count[1] != dwpald_events_count[2])
		dwpald_events_order_err++;

	return 0;
}

static dwpald_hostap_event debug_hap_events[] = {
	{ INJECTED_EVENT_1_OP_CODE, sizeof(INJECTED_EVENT_1_OP_CODE) - 1, dpald_hostap_injected_event_1 },
	{ INJECTED_EVENT_2_OP_CODE, sizeof(INJECTED_EVENT_2_OP_CODE) - 1, dpald_hostap_injected_event_2 },
	{ INJECTED_EVENT_3_OP_CODE, sizeof(INJECTED_EVENT_3_OP_CODE) - 1, dpald_hostap_injected_event_3 },
};

static size_t num_debug_hap_events = (sizeof(debug_hap_events) / sizeof(debug_hap_events[0]));

static int scan_results_available = 0;

static int nl_event(struct nl_msg *msg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

	TLOG("received nl event");

	nla_parse(tb_msg, NL80211_ATTR_MAX,
		  genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0),
		  NULL);

	if (tb_msg[NL80211_ATTR_IFNAME]) {
		TLOG("nl event is from interface: %s",
		     nla_get_string(tb_msg[NL80211_ATTR_IFNAME]));
	} else if (tb_msg[NL80211_ATTR_IFINDEX]) {
		char ifname[IFNAMSIZ + 1] = { 0 };

		if_indextoname(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]), ifname);
		TLOG("nl event is from interface: %s", ifname);
	}

	switch (gnlh->cmd) {
	case NL80211_CMD_TRIGGER_SCAN:
		TLOG("nl event is 'SCAN_TRIGGERED'");
		break;
	case NL80211_CMD_NEW_SCAN_RESULTS:
		TLOG("nl event is 'SCAN_RESULTS_READY'");
		scan_results_available++;
		break;
	case NL80211_CMD_SCAN_ABORTED:
		TLOG("nl event is 'SCAN_ABORTED'");
		break;
	default:
		ELOG("nl event is 'UNKNOWN'");
		break;
	}

	return 0;
}

static int driver_event(char *ifname, int drv_event_id, void *data, size_t len)
{
	(void)data;
	TLOG("received event '%d' from iface '%s' (len=%zu)", drv_event_id, ifname, len);
	return 0;
}

static dwpald_driver_nl_event drv_events[] = {
	{ LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY , driver_event },
	{ LTQ_NL80211_VENDOR_EVENT_CHAN_DATA , driver_event },
};

static size_t num_drv_events = (sizeof(drv_events) / sizeof(drv_events[0]));

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

static int dwpal_ext_hap_attach(void)
{
	int ret;

	ret = dwpal_ext_hostap_interface_attach("wlan0", dpal_ext_hostap_injected_event);
	if (ret != DWPAL_SUCCESS)
		return 1;
	ret = dwpal_ext_hostap_interface_attach("wlan2", dpal_ext_hostap_injected_event);
	if (ret != DWPAL_SUCCESS)
		return 1;

	return 0;
}

static int dwpal_ext_hap_detach(void)
{
	int ret;

	ret = dwpal_ext_hostap_interface_detach("wlan2");
	if (ret != DWPAL_SUCCESS)
		return 1;
	ret = dwpal_ext_hostap_interface_detach("wlan0");
	if (ret != DWPAL_SUCCESS)
		return 1;

	return 0;
}

static int run_unit_test_daemon(int debug)
{
	if (debug)
		return execl("/usr/bin/dwpal_daemon", "dwpal_daemon", "-d", "-u", NULL);
	else
		return execl("/usr/bin/dwpal_daemon", "dwpal_daemon", "-i", "wlan2", "-u", NULL);
}

UNIT_TEST_DEFINE(1, N * connect disconnect to/from daemon)
	dwpald_ret ret;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);
		dwpald_unit_test_mode();

		for (i = 0; i < 15; i++) {
			ret = dwpald_connect("unitest1");
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("connect returned err (%d) i=%d", ret, i);

			ret = dwpald_start_listener();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("start listener returned err (%d) i=%d", ret, i);

			ret = dwpald_disconnect();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("disconnect returned err (%d) i=%d", ret, i);
		}

		ret = dwpald_connect("Executioner");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, N * attach detach to hostap & driver interfaces)
	dwpald_ret ret;
	unsigned int i, j, k;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);
		dwpald_unit_test_mode();

		TLOG("step 1 - attach N times to wlan0/2/6");

		for (i = 0; i < 2; i++) {
			ret = dwpald_connect("unitest2");
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("connect returned err (%d) i=%d", ret, i);

			ret = dwpald_start_listener();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("start listener returned err (%d) i=%d", ret, i);

			for (j = 0; j < 30; j++) {
				char *ifname[3] = { "wlan0", "wlan2", "wlan6" };

				for (k = 0; k < sizeof(ifname) / sizeof(char*); k++) {

					ret = dwpald_hostap_attach(ifname[k],
								   num_debug_hap_events,
								   debug_hap_events, 0);
					if (ret != DWPALD_SUCCESS && ret != DPWALD_DWPAL_IFACE_IS_DOWN)
						UNIT_TEST_FAILED("attach %s returned err (%d) i=%d j=%d", ifname[k], ret, i, j);
				}
			}

			ret = dwpald_disconnect();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("disconnect returned err (%d) i=%d", ret, i);
		}

		TLOG("step 1 - done");
		sleep(2);
		TLOG("step 2 - attach/detach N times to wlan0/2/6");

		for (i = 0; i < 2; i++) {
			ret = dwpald_connect("unitest2");
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("connect returned err (%d) i=%d", ret, i);

			ret = dwpald_start_listener();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("start listener returned err (%d) i=%d", ret, i);

			for (j = 0; j < 30; j++) {
				char *ifname[3] = { "wlan0", "wlan2", "wlan6" };

				for (k = 0; k < sizeof(ifname) / sizeof(char*); k++) {

					ret = dwpald_hostap_attach(ifname[k],
								   num_debug_hap_events,
								   debug_hap_events, 0);
					if (ret != DWPALD_SUCCESS && ret != DPWALD_DWPAL_IFACE_IS_DOWN)
						UNIT_TEST_FAILED("attach %s returned err (%d) i=%d j=%d", ifname[k], ret, i, j);
				}

				for (k = 0; k < sizeof(ifname) / sizeof(char*); k++) {
					ret = dwpald_hostap_detach(ifname[k]);
					if (ret != DWPALD_SUCCESS)
						UNIT_TEST_FAILED("detach %s returned err (%d) i=%d j=%d", ifname[k], ret, i, j);
				}

				if ((j + 1) % 10 == 0) {
					TLOG("step 2 - i=%d j %d", i, j + 1);
					sleep(2);
				}
			}

			ret = dwpald_disconnect();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("disconnect returned err (%d) i=%d", ret, i);
		}

		TLOG("step 2 - done");
		sleep(2);
		TLOG("step 3 - attach N times to nl_drv");

		for (i = 0; i < 4; i++) {
			ret = dwpald_connect("unitest2");
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("connect returned err (%d) i=%d", ret, i);

			ret = dwpald_start_listener();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("start listener returned err (%d) i=%d", ret, i);

			for (j = 0; j < 30; j++) {
				ret = dwpald_nl_drv_attach(num_drv_events, drv_events, nl_event);
				if (ret != DWPALD_SUCCESS)
					UNIT_TEST_FAILED("attach nl_drv returned err (%d) i=%d j=%d", ret, i, j);
			}

			ret = dwpald_disconnect();
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("disconnect returned err (%d) i=%d", ret, i);
			sleep(2);
		}

		TLOG("step 3 - done");

		ret = dwpald_connect("Executioner");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(3, verify hostapd commands vs dwpal_ext)
	dwpald_ret ret;
	int dret;
	char *ifname[2] = { "wlan0", "wlan2" };
	char reply1[4096], reply2[4096];
	size_t reply_size1, reply_size2;
	int i, ifindex = 0;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);
		dwpald_unit_test_mode();

		ret = dwpald_connect("unitest3");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_start_listener();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("start listener returned err (%d)", ret);

		for (i = 0; i < 2; i++) {
			ret = dwpald_hostap_attach(ifname[i],
						   num_debug_hap_events,
						   debug_hap_events, 0);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("attach iface %s returned err (%d)", ifname[i], ret);
		}

		if (dwpal_ext_hap_attach())
			UNIT_TEST_FAILED("dwpal_ext_hap_attach returned err");

		/* actual test start */

		for (i = 0; i < 10000; i++) {
			int failed_once = 0;
retry:
			reply_size1 = sizeof(reply1);
			memset(reply1, 0, sizeof(reply1));
			ret = dwpald_hostap_cmd(ifname[ifindex], "STATUS", sizeof("STATUS"), reply1, &reply_size1);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("dwpald_hostap_cmd returned err (%d) i=%d", ret, i);

			reply_size2 = sizeof(reply2);
			memset(reply2, 0, sizeof(reply2));
			dret = dwpal_ext_hostap_cmd_send(ifname[ifindex], "STATUS", NULL, reply2, &reply_size2);
			if (dret != DWPAL_SUCCESS)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d", ret, i);

			if (reply_size1 != reply_size2 || reply_size1 == 0 || reply1[0] == '\0') {
				if (failed_once++ == 0)
					goto retry;

				UNIT_TEST_FAILED("%zu != %zu i=%d", reply_size1, reply_size2, i);
			}

			if (reply1[reply_size1] != '\0')
				UNIT_TEST_FAILED("reply1 at reply_size1=%zu is not 0", reply_size1);

			if (memcmp(reply1, reply2, reply_size1)) {
				if (failed_once++ == 0)
					goto retry;

				UNIT_TEST_FAILED("memcmp failed i=%d", i);
			}

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_START

		for (i = 0; i < 10000; i++) {
			reply_size1 = sizeof(reply1);
			memset(reply1, 0, sizeof(reply1));
			ret = dwpald_hostap_cmd(ifname[ifindex], "STATUS", sizeof("STATUS"), reply1, &reply_size1);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("dwpald_hostap_cmd returned err (%d) i=%d", ret, i);

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_STOP(status command with daemon)

		UNIT_TEST_TIMER_START

		for (i = 0; i < 10000; i++) {
			reply_size2 = sizeof(reply2);
			memset(reply2, 0, sizeof(reply2));
			dret = dwpal_ext_hostap_cmd_send(ifname[ifindex], "STATUS", NULL, reply2, &reply_size2);
			if (dret != DWPAL_SUCCESS)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d", dret, i);

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_STOP(status command with dwapl_ext)

		/* actual test end */

		if (dwpal_ext_hap_detach())
			UNIT_TEST_FAILED("dwpal_ext_hap_detach returned err");

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpal_ext_hap_detach();
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(4, verify driver commands vs dwpal_ext)
	dwpald_ret ret;
	int dret;
	char *ifname[2] = { "wlan0", "wlan2" };
	bg_scan_params_t params1 = { 0 }, params2 = { 0 };
	unsigned char dataDpal[sizeof(params2) + 4];
	size_t params_size1, params_size2;
	int i, ifindex = 0, res = 0;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);
		dwpald_unit_test_mode();

		ret = dwpald_connect("unitest4");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_start_listener();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("start listener returned err (%d)", ret);

		if (dwpal_ext_driver_nl_attach(empty_DWPAL_nlVendorEventCallback,
					       empty_DWPAL_nlNonVendorEventCallback))
			UNIT_TEST_FAILED("dwpal_ext_driver_nl_attach returned err");

		ret = dwpald_nl_drv_attach(0, NULL, NULL);
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("dwpald attach returned err");

		/* actual test start */

		for (i = 0; i < 10000; i++) {
			int failed_once = 0;
retry:
			params_size1 = sizeof(params1);
			ret = dwpald_drv_get(ifname[ifindex], LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
					     &res, NULL, 0, &params1, &params_size1);
			if (ret != DWPALD_SUCCESS || res)
				UNIT_TEST_FAILED("dwpald_drv_get returned err (%d) res (%d) i=%d", ret, res, i);

			params_size2 = sizeof(dataDpal);
			memset(&dataDpal, 0, sizeof(dataDpal));
			dret = dwpal_ext_driver_nl_get(ifname[ifindex], NL80211_CMD_VENDOR, DWPAL_NETDEV_ID,
					LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG, NULL, 0,
					&params_size2, dataDpal);
			if (dret != DWPAL_SUCCESS)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d", ret, i);

			memcpy(&params2, &dataDpal[4], sizeof(params2));

			if (params_size1 != params_size2 - 4 || params_size1 == 0)
				UNIT_TEST_FAILED("%zu != %zu i=%d", params_size1, params_size2, i);

			if (memcmp(&params1, &params2, sizeof(params1))) {
				if (failed_once++ == 0)
					goto retry;

				UNIT_TEST_FAILED("memcmp failed i=%d a.passive %d b.passive %d a.dwell %d b.dwell %d", i,
						params1.passive, params2.passive, params1.break_time, params2.break_time);
			}

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_START

		for (i = 0; i < 10000; i++) {
			params_size1 = sizeof(params1);
			ret = dwpald_drv_get(ifname[ifindex], LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
					     &res, NULL, 0, &params1, &params_size1);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("dwpald_drv_get returned err (%d) i=%d", ret, i);

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_STOP(gBGScanParams command with daemon)

		UNIT_TEST_TIMER_START

		for (i = 0; i < 10000; i++) {
			params_size2 = sizeof(dataDpal);
			memset(&dataDpal, 0, sizeof(dataDpal));
			dret = dwpal_ext_driver_nl_get(ifname[ifindex], NL80211_CMD_VENDOR, DWPAL_NETDEV_ID,
					LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG, NULL, 0,
					&params_size2, dataDpal);
			if (dret != DWPAL_SUCCESS)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d", ret, i);

			ifindex++;
			ifindex %= 2;
		}

		UNIT_TEST_TIMER_STOP(gBGScanParams command with dwapl_ext)

		/* actual test end */

		if (dwpal_ext_driver_nl_detach())
			UNIT_TEST_FAILED("dwpal_ext_driver_nl_detach returned err");

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpal_ext_driver_nl_detach();
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(5, verify hostapd events vs dwpal_ext)
	dwpald_ret ret;
	int dret;
	char *ifname[2] = { "wlan0", "wlan2" };
	char reply[256];
	size_t reply_size;
	int i, ifindex = 0, cmd_idx = 0;
	char *commands[6] = { "INJECT_DEBUG_HOSTAP_EVENT " INJECTED_EVENT_1_OP_CODE " " INJECTED_EVENT_1_MSG,
			      "INJECT_DEBUG_HOSTAP_EVENT " INJECTED_EVENT_2_OP_CODE " " INJECTED_EVENT_2_MSG,
			      "INJECT_DEBUG_HOSTAP_EVENT " INJECTED_EVENT_3_OP_CODE " " INJECTED_EVENT_3_MSG,
			      "INJECT_DEBUG_HOSTAP_EVENT UNEXPECTED-EVENT-1 MSG1 3335352222222ssssffggggggg",
			      "INJECT_DEBUG_HOSTAP_EVENT UNEXPECTED-EVENT-2 MSG2 yyyyyyyyyyyyyyyyyyyyyyyyyy",
			      "INJECT_DEBUG_HOSTAP_EVENT UNEXPECTED-EVENT-2 MSG3 asdasdas adsadsa dawwwwwww" };

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);
		dwpald_unit_test_mode();

		ret = dwpald_connect("unitest5");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_start_listener();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("start listener returned err (%d)", ret);

		for (i = 0; i < 2; i++) {
			ret = dwpald_hostap_attach(ifname[i],
						   num_debug_hap_events,
						   debug_hap_events, 0);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("attach iface %s returned err (%d)", ifname[i], ret);
		}

		if (dwpal_ext_hap_attach())
			UNIT_TEST_FAILED("dwpal_ext_hap_attach returned err");

		/* actual test start */

		memset(dwpal_ext_events_count, 0, sizeof(dwpal_ext_events_count));
		memset(dwpald_events_count, 0, sizeof(dwpald_events_count));
		dwpal_ext_events_cmp_err = 0;
		dwpald_events_cmp_err = 0;
		dwpal_ext_events_order_err = 0;
		dwpald_events_order_err = 0;

		UNIT_TEST_TIMER_START

		for (i = 1; i <= 6000; i++) {
			reply_size = sizeof(reply);

			dret = dwpal_ext_hostap_cmd_send(ifname[ifindex], commands[cmd_idx], NULL, reply, &reply_size);
			if (dret != DWPAL_SUCCESS || strncmp(reply, "OK\n", 3) || reply_size != 3)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d reply=%s size=%zu", ret, i, reply, reply_size);

			// ret = dwpald_hostap_cmd(ifname[ifindex], commands[cmd_idx], strlen(commands[cmd_idx]) + 1, reply, &reply_size);
			// if (ret != DWPALD_SUCCESS || strncmp(reply, "OK\n", 3) || reply_size != 3)
			// 	UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d reply=%s size=%zu", ret, i, reply, reply_size);

			ifindex++;
			ifindex %= 2;

			cmd_idx++;
			cmd_idx %= 6;

			if (i % 50 == 0)
				usleep(100000);

			if (i % 3000 == 0) {
				WLOG("pause at i=%d", i);
				TLOG("so far dwpald:");
				TLOG("err: %u ooo: %u ev1: %u ev2: %u ev3: %u", dwpald_events_cmp_err, dwpald_events_order_err,
				     dwpald_events_count[0], dwpald_events_count[1], dwpald_events_count[2]);

				TLOG("so far dwpal_ext:");
				TLOG("err: %u ooo: %u ev1: %u ev2: %u ev3: %u", dwpal_ext_events_cmp_err, dwpal_ext_events_order_err,
				     dwpal_ext_events_count[0], dwpal_ext_events_count[1], dwpal_ext_events_count[2]);
				sleep(1);
			}
		}

		UNIT_TEST_TIMER_STOP(sent 6000 injected event commands to hostapd)

		TLOG("so far dwpald:");
		TLOG("err: %u ooo: %u ev1: %u ev2: %u ev3: %u", dwpald_events_cmp_err, dwpald_events_order_err,
		     dwpald_events_count[0], dwpald_events_count[1], dwpald_events_count[2]);

		TLOG("so far dwpal_ext:");
		TLOG("err: %u ooo: %u ev1: %u ev2: %u ev3: %u", dwpal_ext_events_cmp_err, dwpal_ext_events_order_err,
		     dwpal_ext_events_count[0], dwpal_ext_events_count[1], dwpal_ext_events_count[2]);

		if (dwpald_events_cmp_err || dwpal_ext_events_cmp_err ||
		    dwpald_events_count[0] != 1000 || dwpald_events_count[1] != 1000 || dwpald_events_count[2] != 1000 ||
		    dwpal_ext_events_count[0] != 1000 || dwpal_ext_events_count[1] != 1000 || dwpal_ext_events_count[2] != 1000)
			UNIT_TEST_FAILED("err detected or counters missing");

		/* actual test end */

		if (dwpal_ext_hap_detach())
			UNIT_TEST_FAILED("dwpal_ext_hap_detach returned err");

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpal_ext_hap_detach();
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

static int non_listenning_dwpald_client_term = 0;

static void sigusr2_handler(int signum)
{
	(void)signum;

	non_listenning_dwpald_client_term = 1;
}

static int non_listenning_dwpald_client(void)
{
	int i;
	dwpald_ret ret;
	char *ifname[2] = { "wlan0", "wlan2" };
	int timeout = 60;

	dwpald_unit_test_mode();

	ret = dwpald_connect("unitest6_bad");
	if (ret != DWPALD_SUCCESS) {
		ELOG("attach returned err (%d)", ret);
		return 1;
	}

	for (i = 0; i < 2; i++) {
		ret = dwpald_hostap_attach(ifname[i],
					   num_debug_hap_events,
					   debug_hap_events, 0);
		if (ret != DWPALD_SUCCESS) {
			ELOG("attach iface %s returned err (%d)", ifname[i], ret);
			return 1;
		}
	}

	signal(SIGUSR2, sigusr2_handler);

	while (!non_listenning_dwpald_client_term && timeout > 0) {
		sleep(1);
		timeout--;
	}

	TLOG("exited sleep");

	ret = dwpald_disconnect();
	if (ret != DWPALD_SUCCESS) {
		ELOG("disconnect returned err (%d)", ret);
		return 1;
	}

	return 0;
}

UNIT_TEST_DEFINE(6, check daemon stuck due to non listenning client)

	int bad_client = -1;
	dwpald_ret ret;
	int dret;
	char reply[256];
	size_t reply_size;
	int i;
	char *ifname[2] = { "wlan0", "wlan2" };
	char *command = "INJECT_DEBUG_HOSTAP_EVENT " INJECTED_EVENT_1_OP_CODE " " INJECTED_EVENT_1_MSG;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);

		memset(dwpal_ext_events_count, 0, sizeof(dwpal_ext_events_count));
		memset(dwpald_events_count, 0, sizeof(dwpald_events_count));
		dwpal_ext_events_cmp_err = 0;
		dwpald_events_cmp_err = 0;
		dwpal_ext_events_order_err = 0;
		dwpald_events_order_err = 0;

		bad_client = fork();
		if (bad_client == -1)
			UNIT_TEST_FAILED("fork")
		else if (bad_client == 0)
			exit(non_listenning_dwpald_client());

		dwpald_unit_test_mode();

		ret = dwpald_connect("unitest6");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("attach returned err (%d)", ret);

		ret = dwpald_start_listener();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("start listener returned err (%d)", ret);

		for (i = 0; i < 2; i++) {
			ret = dwpald_hostap_attach(ifname[i],
						   num_debug_hap_events,
						   debug_hap_events, 0);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("attach iface %s returned err (%d)", ifname[i], ret);
		}

		if (dwpal_ext_hap_attach())
			UNIT_TEST_FAILED("dwpal_ext_hap_attach returned err");

		/* actual test start */

		UNIT_TEST_TIMER_START

		for (i = 1; i <= 6000; i++) {
			reply_size = sizeof(reply);

			dret = dwpal_ext_hostap_cmd_send("wlan0", command, NULL, reply, &reply_size);
			if (dret != DWPAL_SUCCESS || strncmp(reply, "OK\n", 3) || reply_size != 3)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d reply=%s size=%zu", dret, i, reply, reply_size);

			if (i % 50 == 0) {
				usleep(100000);
				reply_size = sizeof(reply);
				ret = dwpald_hostap_cmd("wlan0", "PING", sizeof("PING"), reply, &reply_size);
				if (ret != DWPALD_SUCCESS || strncmp(reply, "PONG\n", 3) || reply_size != 5)
					UNIT_TEST_FAILED("dwpald_hostap_cmd PING returned err (%d) i=%d reply=%s size=%zu", ret, i, reply, reply_size);
			}

			if (i % 3000 == 0) {
				WLOG("pause at i=%d", i);
				TLOG("so far dwpald:");
				TLOG("err: %u ev1: %u", dwpald_events_cmp_err, dwpald_events_count[0]);

				TLOG("so far dwpal_ext:");
				TLOG("err: %u ev1: %u", dwpal_ext_events_cmp_err, dwpal_ext_events_count[0]);
				sleep(1);
			}
		}

		UNIT_TEST_TIMER_STOP(sent 6000 injected event commands to hostapd)

		TLOG("so far dwpald:");
		TLOG("err: %u ev1: %u", dwpald_events_cmp_err, dwpald_events_count[0]);

		TLOG("so far dwpal_ext:");
		TLOG("err: %u ev1: %u", dwpal_ext_events_cmp_err, dwpal_ext_events_count[0]);

		if (dwpald_events_cmp_err || dwpal_ext_events_cmp_err ||
		    dwpald_events_count[0] != 6000 || dwpal_ext_events_count[0] != 6000)
			UNIT_TEST_FAILED("err detected or counters missing");

		/* actual test end */

		if (dwpal_ext_hap_detach())
			UNIT_TEST_FAILED("dwpal_ext_hap_detach returned err");

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		kill(bad_client, SIGUSR2);
		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	if (bad_client > 0)
		kill(bad_client, SIGKILL);
	dwpal_ext_hap_detach();
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

static int slow_dwpald_client(void)
{
	int i;
	dwpald_ret ret;
	int max_num_sleeps = 10;
	char *ifname[2] = { "wlan0", "wlan2" };

	dwpald_unit_test_mode();

	dwpald_event_slower_response = 1;

	ret = dwpald_connect("unitest7_slow");
	if (ret != DWPALD_SUCCESS) {
		ELOG("attach returned err (%d)", ret);
		return 1;
	}

	ret = dwpald_start_listener();
	if (ret != DWPALD_SUCCESS) {
		ELOG("start listener returned err (%d)", ret);
		return 1;
	}

	for (i = 0; i < 2; i++) {
		ret = dwpald_hostap_attach(ifname[i],
					   num_debug_hap_events,
					   debug_hap_events, 0);
		if (ret != DWPALD_SUCCESS) {
			ELOG("attach iface %s returned err (%d)", ifname[i], ret);
			return 1;
		}
	}

	while (dwpald_events_count[0] < 1000 && max_num_sleeps--) {
		TLOG("dwpald_events_count == %d", dwpald_events_count[0]);
		sleep(1);
	}

	TLOG("exited sleep");

	dwpald_event_slower_response = 0;

	while (dwpald_events_count[0] != 1000 && max_num_sleeps++ <= 5) {
		TLOG("dwpald_events_count == %d", dwpald_events_count[0]);
		sleep(1);
	}

	if (dwpald_events_count[0] != 1000) {
		ELOG("dwpald_events_count == %d << 5000", dwpald_events_count[0]);
		return 1;
	}

	ret = dwpald_disconnect();
	if (ret != DWPALD_SUCCESS) {
		ELOG("disconnect returned err (%d)", ret);
		return 1;
	}

	return 0;
}

UNIT_TEST_DEFINE(7, check daemon stuck due to slow client)

	int bad_client = -1;
	dwpald_ret ret;
	int dret;
	char reply[256];
	size_t reply_size;
	int i;
	char *ifname[2] = { "wlan0", "wlan2" };
	char *command = "INJECT_DEBUG_HOSTAP_EVENT " INJECTED_EVENT_1_OP_CODE " " INJECTED_EVENT_1_MSG;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);
		usleep(100000);

		memset(dwpal_ext_events_count, 0, sizeof(dwpal_ext_events_count));
		memset(dwpald_events_count, 0, sizeof(dwpald_events_count));
		dwpal_ext_events_cmp_err = 0;
		dwpald_events_cmp_err = 0;
		dwpal_ext_events_order_err = 0;
		dwpald_events_order_err = 0;

		bad_client = fork();
		if (bad_client == -1)
			UNIT_TEST_FAILED("fork")
		else if (bad_client == 0)
			exit(slow_dwpald_client());

		dwpald_unit_test_mode();

		ret = dwpald_connect("unitest7");
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("attach returned err (%d)", ret);

		ret = dwpald_start_listener();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("start listener returned err (%d)", ret);

		for (i = 0; i < 2; i++) {
			ret = dwpald_hostap_attach(ifname[i],
						   num_debug_hap_events,
						   debug_hap_events, 0);
			if (ret != DWPALD_SUCCESS)
				UNIT_TEST_FAILED("attach iface %s returned err (%d)", ifname[i], ret);
		}

		if (dwpal_ext_hap_attach())
			UNIT_TEST_FAILED("dwpal_ext_hap_attach returned err");

		/* actual test start */

		UNIT_TEST_TIMER_START

		for (i = 1; i <= 1000; i++) {
			reply_size = sizeof(reply);

			dret = dwpal_ext_hostap_cmd_send("wlan0", command, NULL, reply, &reply_size);
			if (dret != DWPAL_SUCCESS || strncmp(reply, "OK\n", 3) || reply_size != 3)
				UNIT_TEST_FAILED("dwpal_ext_hostap_cmd_send returned err (%d) i=%d reply=%s size=%zu", dret, i, reply, reply_size);

			if (i % 50 == 0) {
				usleep(10000);
				reply_size = sizeof(reply);
				ret = dwpald_hostap_cmd("wlan0", "PING", sizeof("PING"), reply, &reply_size);
				if (ret != DWPALD_SUCCESS || strncmp(reply, "PONG\n", 3) || reply_size != 5)
					UNIT_TEST_FAILED("dwpald_hostap_cmd PING returned err (%d) i=%d reply=%s size=%zu", ret, i, reply, reply_size);
			}
		}

		sleep(1);

		UNIT_TEST_TIMER_STOP(sent 1000 injected event commands to hostapd)

		TLOG("so far dwpald:");
		TLOG("err: %u ev1: %u", dwpald_events_cmp_err, dwpald_events_count[0]);

		TLOG("so far dwpal_ext:");
		TLOG("err: %u ev1: %u", dwpal_ext_events_cmp_err, dwpal_ext_events_count[0]);

		if (dwpald_events_cmp_err || dwpal_ext_events_cmp_err ||
		    dwpald_events_count[0] != 1000 || dwpal_ext_events_count[0] != 1000)
			UNIT_TEST_FAILED("err detected or counters missing");

		{
			int status = 0, wait_res;
			do {
				usleep(1000);
				wait_res = waitpid(bad_client, &status, WNOHANG);
			} while (wait_res == 0);
			ILOG("slow client exited with status=%d wait_res=%d", status, wait_res);
			if (status)
				UNIT_TEST_FAILED("slow client returned with err=%d", status);
		}

		/* actual test end */

		if (dwpal_ext_hap_detach())
			UNIT_TEST_FAILED("dwpal_ext_hap_detach returned err");

		ret = dwpald_term_daemon();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	if (bad_client > 0)
		kill(bad_client, SIGKILL);
	dwpal_ext_hap_detach();
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(8, check dwpald client error codes)
	dwpald_ret ret;
	char reply[4096];
	size_t reply_size;
	bg_scan_params_t bg_scan_params = { 0 };
	size_t params_size;
	int cmd_res;

	dwpald_unit_test_mode();

	TLOG("step 1 - before dwpald_connect() call");

	ret = dwpald_start_listener();
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("start listener returned %d", ret);

	ret = dwpald_hostap_attach("wlan0", num_debug_hap_events, debug_hap_events, 0);
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("hostap attach returned %d", ret);

	ret = dwpald_hostap_detach("wlan0");
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("hostap detach returned %d", ret);

	ret = dwpald_nl_drv_attach(num_drv_events, drv_events, nl_event);
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("nl_drv attach returned %d", ret);

	reply_size = sizeof(reply);
	ret = dwpald_hostap_cmd("wlan0", "STATUS", sizeof("STATUS"), reply, &reply_size);
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("hostap cmd returned %d", ret);

	params_size = sizeof(bg_scan_params);
	ret = dwpald_drv_get("wlan0", LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
			     &cmd_res, NULL, 0, &bg_scan_params, &params_size);
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("drv cmd returned %d", ret);

	ret = dwpald_disconnect();
	if (ret != DWPALD_ERROR)
		UNIT_TEST_FAILED("disconnect returned %d", ret);

	TLOG("step 2 - after dwpald_connect() but daemon down");

	ret = dwpald_connect("unitest8");
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("connect returned err (%d)", ret);

	ret = dwpald_start_listener();
	if (ret == DWPALD_ERROR)
		UNIT_TEST_FAILED("start listener returned %d", ret);

	ret = dwpald_hostap_attach("wlan0", num_debug_hap_events, debug_hap_events, 0);
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("hostap attach returned %d", ret);

	ret = dwpald_hostap_detach("wlan0");
	if (ret != DWPALD_SUCCESS)
		UNIT_TEST_FAILED("hostap detach returned %d", ret);

	ret = dwpald_nl_drv_attach(num_drv_events, drv_events, nl_event);
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("nl_drv attach returned %d", ret);

	ret = dwpald_hostap_attach("wlan0", num_debug_hap_events, debug_hap_events, 0);
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("hostap attach returned %d", ret);

	reply_size = sizeof(reply);
	ret = dwpald_hostap_cmd("wlan0", "STATUS", sizeof("STATUS"), reply, &reply_size);
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("hostap cmd returned %d", ret);

	params_size = sizeof(bg_scan_params);
	ret = dwpald_drv_get("wlan0", LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
			     &cmd_res, NULL, 0, &bg_scan_params, &params_size);
	if (ret != DWPALD_DISCONNECTED)
		UNIT_TEST_FAILED("drv cmd returned %d", ret);

	TLOG("step 3 - daemon up");

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_unit_test_daemon(0));
	UNIT_TEST_FORKED_PARENET

		if (__running_in_valgrind)
			sleep(3);

		reply_size = sizeof(reply);
		ret = dwpald_hostap_cmd("wlan0", "STATUS", sizeof("STATUS"), reply, &reply_size);
		if (ret != DPWALD_DWPAL_IFACE_IS_DOWN)
			UNIT_TEST_FAILED("hostap cmd returned %d", ret);

		ret = dwpald_hostap_attach("wlan0", num_debug_hap_events, debug_hap_events, 0);
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("hostap attach returned %d", ret);

		reply_size = sizeof(reply);
		ret = dwpald_hostap_cmd("wlan0", "STATUS", sizeof("STATUS"), reply, &reply_size);
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("hostap cmd returned %d", ret);

		ret = dwpald_nl_drv_attach(num_drv_events, drv_events, nl_event);
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("nl_drv attach returned %d", ret);

		params_size = sizeof(bg_scan_params);
		ret = dwpald_drv_get("wlan0", LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
				&cmd_res, NULL, 0, &bg_scan_params, &params_size);
		if (ret != DWPALD_SUCCESS)
			UNIT_TEST_FAILED("drv cmd returned %d", ret);

		reply_size = sizeof(reply);
		ret = dwpald_hostap_cmd("wlan6", "STATUS", sizeof("STATUS"), reply, &reply_size);
		if (ret != DPWALD_DWPAL_IFACE_IS_DOWN)
			UNIT_TEST_FAILED("hostap cmd returned %d", ret);

		ret = dwpald_hostap_attach("wlan6", num_debug_hap_events, debug_hap_events, 0);
		if (ret != DPWALD_DWPAL_IFACE_IS_DOWN)
			UNIT_TEST_FAILED("hostap attach returned %d", ret);

		reply_size = sizeof(reply);
		ret = dwpald_hostap_cmd("wlan6", "STATUS", sizeof("STATUS"), reply, &reply_size);
		if (ret != DPWALD_DWPAL_IFACE_IS_DOWN)
			UNIT_TEST_FAILED("hostap cmd returned %d", ret);

		params_size = sizeof(bg_scan_params);
		ret = dwpald_drv_get("wlan6", LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
				&cmd_res, NULL, 0, &bg_scan_params, &params_size);
		if (ret != DWPALD_ERROR)
			UNIT_TEST_FAILED("drv cmd returned %d cmd_res=%d", ret, cmd_res);

		ret = dwpald_disconnect();
		if (ret == DWPALD_ERROR)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		ret = dwpald_connect("Executioner");
		if (ret == DWPALD_ERROR)
			UNIT_TEST_FAILED("connect returned err (%d)", ret);

		ret = dwpald_term_daemon();
		if (ret == DWPALD_ERROR)
			UNIT_TEST_FAILED("terminate request failed");

		ret = dwpald_disconnect();
		if (ret == DWPALD_ERROR)
			UNIT_TEST_FAILED("disconnect returned err (%d)", ret);

		sleep(1);

UNIT_TEST_CLEANUP_ON_ERRR
	dwpald_disconnect();
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(dwpal_daemon)
	__running_in_valgrind = is_running_in_valgrind();
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
	ADD_TEST(5)
	ADD_TEST(6)
	ADD_TEST(7)
	ADD_TEST(8)
UNIT_TEST_MODULE_DEFINITION_DONE
