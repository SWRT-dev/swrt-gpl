/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "dwpald_client.h"
#include "logs.h"
#include "dwpal.h"

#include <unistd.h>
#include <net/if.h>
#include <string.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

static int hostap_event(char *ifname, char *op_code, char *msg, size_t len)
{
	(void)msg;
	LOG(1, "received event '%s' from iface '%s' (len=%zu)", op_code, ifname, len);
	if (len)
		LOG(1, "msg=%s", msg);
	return 0;
}

static dwpald_hostap_event hap_events[] = {
	{ "AP-STA-CONNECTED", sizeof("AP-STA-CONNECTED") - 1, hostap_event },
	{ "AP-STA-DISCONNECTED", sizeof("AP-STA-DISCONNECTED") - 1, hostap_event },
	{ "INTERFACE_CONNECTED_OK", sizeof("INTERFACE_CONNECTED_OK") - 1, hostap_event },
};

static int driver_event(char *ifname, int drv_event_id, void *data, size_t len)
{
	(void)data;
	LOG(1, "received event '%d' from iface '%s' (len=%zu)", drv_event_id, ifname, len);
	return 0;
}

static dwpald_driver_nl_event drv_events[] = {
	{ LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY , driver_event },
	{ LTQ_NL80211_VENDOR_EVENT_CHAN_DATA , driver_event },
};

static int scan_results_available = 0;

static int nl_event(struct nl_msg *msg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

	LOG(1, "received nl event");

	nla_parse(tb_msg, NL80211_ATTR_MAX,
		  genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0),
		  NULL);

	if (tb_msg[NL80211_ATTR_IFNAME]) {
		LOG(1, "nl event is from interface: %s",
		     nla_get_string(tb_msg[NL80211_ATTR_IFNAME]));
	} else if (tb_msg[NL80211_ATTR_IFINDEX]) {
		char ifname[IFNAMSIZ + 1] = { 0 };

		if_indextoname(nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]), ifname);
		LOG(1, "nl event is from interface: %s", ifname);
	}

	switch (gnlh->cmd) {
	case NL80211_CMD_TRIGGER_SCAN:
		LOG(1, "nl event is 'SCAN_TRIGGERED'");
		break;
	case NL80211_CMD_NEW_SCAN_RESULTS:
		LOG(1, "nl event is 'SCAN_RESULTS_READY'");
		scan_results_available++;
		break;
	case NL80211_CMD_SCAN_ABORTED:
		LOG(1, "nl event is 'SCAN_ABORTED'");
		break;
	default:
		ELOG("nl event is 'UNKNOWN'");
		break;
	}

	return 0;
}

static int nl_scan_result(struct nl_msg *msg, void *cb_arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *bss[NL80211_BSS_MAX + 1];
	static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
		[NL80211_BSS_BSSID] = { .type = NLA_UNSPEC },
		[NL80211_BSS_FREQUENCY] = { .type = NLA_U32 },
	};

	(void)cb_arg;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);
	if (!tb[NL80211_ATTR_BSS])
		return 1;
	if (nla_parse_nested(bss, NL80211_BSS_MAX, tb[NL80211_ATTR_BSS],
			     bss_policy))
		return 1;

	char *mac_addr = nla_data(bss[NL80211_BSS_BSSID]);

	LOG(1, "received nl scan result: %02X:%02X:%02X:%02X:%02X:%02X from freq:%u",
	     (uint8_t)mac_addr[0], (uint8_t)mac_addr[1], (uint8_t)mac_addr[2],
	     (uint8_t)mac_addr[3], (uint8_t)mac_addr[4], (uint8_t)mac_addr[5],
	     nla_get_u32(bss[NL80211_BSS_FREQUENCY]));

	return 0;
}

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


int main(int argc, char *argv[])
{
	dwpald_ret ret;
	char ifname[IF_NAMESIZE + 1] = { 0 };
	char cmd1[] = "STATUS", cmd2[] = "PING";
	char reply[4096];
	size_t reply_len;
	int res = 0, wait_count = 0;
	scan_params params = { 0 };
	bg_scan_params_t scan_params_old = { 0 }, scan_params_new = { 0 };
	size_t params_size;

	size_t num_hap_events = (sizeof(hap_events) / sizeof(dwpald_hostap_event));
	size_t num_drv_events = (sizeof(drv_events) / sizeof(dwpald_driver_nl_event));

	if (argc <= 1) {
		ELOG("enter interface name for commands testing");
		return 1;
	}

	strncpy_s(ifname, sizeof(ifname), argv[1], sizeof(ifname) - 1);

	LOG(1, "connecting to dwpal daemon");
	ret = dwpald_connect("demo_client");
	if (ret == DWPALD_ERROR) {
		ELOG("dwpald connect returned error");
		return 1;
	}

	LOG(1, "attaching to hostap iface %s via dwpal daemon", ifname);
	ret = dwpald_hostap_attach(ifname, num_hap_events, hap_events, 0);
	if (ret == DWPALD_ERROR) {
		ELOG("dwpald hostap attach returned error");
		goto end;
	}

	LOG(1, "attaching to drv nl via dwpal daemon");
	ret = dwpald_nl_drv_attach(num_drv_events, drv_events, nl_event);
	if (ret == DWPALD_ERROR) {
		ELOG("dwpald hostap attach returned error");
		goto end;
	}

	LOG(1, "starting dwpald_client listener thread");
	ret = dwpald_start_listener();
	if (ret == DWPALD_ERROR) {
		ELOG("dwpald hostap attach returned error");
		goto end;
	}

	LOG(1, "dwpald attach returned with SUCCESS/DISCONNECTED (ret=%d)", ret);

#ifdef CONFIG_DWPALD_DEBUG_TOOLS
	sleep(1);

	LOG(1, "sending GET CONNECTED CLIENTS command");
	reply_len = sizeof(reply);
	dwpald_get_connected_clients(reply, &reply_len);
	LOG(1, "dwpald returned:\n%s", reply);
#endif

	sleep(1);

	LOG(1, "sending PING command");
	reply_len = sizeof(reply);
	ret = dwpald_hostap_cmd(ifname, cmd2, sizeof(cmd2),
				reply, &reply_len);
	if (ret != DWPALD_SUCCESS)
		ELOG("hostap cmd returned err (ret=%d)", ret);
	else
		LOG(1, "dwpald returned:\n%s", reply);

	LOG(1, "sending STATUS command");
	reply_len = sizeof(reply);
	ret = dwpald_hostap_cmd(ifname, cmd1, sizeof(cmd1),
				reply, &reply_len);
	if (ret != DWPALD_SUCCESS)
		ELOG("hostap cmd returned err (ret=%d)", ret);
	else
		LOG(1, "dwpald returned:\n%s", reply);

	sleep(1);

	LOG(1, "get current BG scan params from the driver");
	params_size = sizeof(scan_params_old);
	ret = dwpald_drv_get(ifname, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
			     &res, NULL, 0, &scan_params_old, &params_size);
	if (ret != DWPALD_SUCCESS) {
		ELOG("dwpald_drv_get returned err");
		goto end;
	} else if (res < 0) {
		ELOG("res=%d", res);
		goto end;
	}

	LOG(1, "passive:%d active:%d num_probe_reqs:%d probe_reqs_interval:%d "
	       "num_chans_in_chunk:%d break_time:%d window_slice:%u "
	       "window_slice_overlap:%u cts_to_self_duration:%u",
	       scan_params_old.passive, scan_params_old.active,
	       scan_params_old.num_probe_reqs, scan_params_old.probe_reqs_interval,
	       scan_params_old.num_chans_in_chunk, scan_params_old.break_time,
	       scan_params_old.window_slice, scan_params_old.window_slice_overlap,
	       scan_params_old.cts_to_self_duration);

	memcpy_s(&scan_params_new, sizeof(bg_scan_params_t),
		 &scan_params_old, sizeof(bg_scan_params_t));
	LOG(1, "setting new BG scan params to the driver");
	scan_params_new.break_time = 200;
	scan_params_new.break_time_busy = 200;
	scan_params_new.passive = 40;
	scan_params_new.active = 40;

	params_size = sizeof(scan_params_new);
	ret = dwpald_drv_set(ifname, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS_BG,
			     &res, &scan_params_new, params_size);
	if (ret != DWPALD_SUCCESS)
		ELOG("dwpald_drv_set returned err");
	else if (res < 0)
		ELOG("res=%d", res);

	memset(&scan_params_new, 0, sizeof(scan_params_new));
	LOG(1, "get current BG scan params from the driver again to check set");
	params_size = sizeof(scan_params_new);
	ret = dwpald_drv_get(ifname, LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG,
			     &res, NULL, 0, &scan_params_new, &params_size);
	if (ret != DWPALD_SUCCESS)
		ELOG("dwpald_drv_get returned err");
	else if (res < 0)
		ELOG("res=%d", res);

	LOG(1, "passive:%d active:%d num_probe_reqs:%d probe_reqs_interval:%d "
	       "num_chans_in_chunk:%d break_time:%d window_slice:%u "
	       "window_slice_overlap:%u cts_to_self_duration:%u",
	       scan_params_new.passive, scan_params_new.active,
	       scan_params_new.num_probe_reqs, scan_params_new.probe_reqs_interval,
	       scan_params_new.num_chans_in_chunk, scan_params_new.break_time,
	       scan_params_new.window_slice, scan_params_new.window_slice_overlap,
	       scan_params_new.cts_to_self_duration);

	if (scan_params_new.break_time != 200)
		ELOG("failed to change the bg scan break time");

	sleep(1);

	params.ap_force = true;
	LOG(1, "Trigger Scan (should work)");
	ret = dwpald_ieee80211_scan_trigger(ifname, &params, &res);
	if (ret != DWPALD_SUCCESS) {
		ELOG("scan trigger returned err");
		goto end;
	} else if (res < 0)
		ELOG("res=%d", res);

	LOG(1, "Trigger Scan again (should fail with EBUSY[-16])");
	ret = dwpald_ieee80211_scan_trigger(ifname, &params, &res);
	if (ret != DWPALD_SUCCESS) {
		ELOG("scan trigger returned err");
	} else if (res < 0) {
		ELOG("res=%d", res);
	}

	while (!scan_results_available && wait_count < 15) {
		sleep(1);
		wait_count++;
		LOG(1, "waiting for scan to finish");
	}

	LOG(1, "return old BG scan params to the driver");
	params_size = sizeof(scan_params_old);
	ret = dwpald_drv_set(ifname, LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS_BG,
			     &res, &scan_params_old, params_size);
	if (ret != DWPALD_SUCCESS)
		ELOG("dwpald_drv_set returned err");
	else if (res < 0)
		ELOG("res=%d", res);

	LOG(1, "Trigger Scan results dump");
	ret = dwpald_ieee80211_scan_dump(ifname, nl_scan_result, &res, NULL);
	if (ret != DWPALD_SUCCESS)
		ELOG("scan dump returned err");
	else if (res < 0)
		ELOG("res=%d", res);

end:
	LOG(1, "detaching from dwpal daemon");
	ret = dwpald_disconnect();
	if (ret == DWPALD_ERROR) {
		ELOG("dwpald_disconnect returned error");
		return 1;
	}

	return 0;
}
