/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "nl_iface.h"
#include "dwpal_daemon.h"
#include "dwpal_ext.h"
#include "linked_list.h"
#include "logs.h"

#include <stdlib.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

typedef struct _drv_event {
	uint32_t nl_id;
	l_list *registered_stations;
} drv_event;

static iface_manager *__manager = NULL;
static int is_attached = 0;

typedef struct {
	wv_ipserver *ipserver;
	wv_ipstation *ipsta;
	uint8_t seq_num;
	int is_multi_msg;
	int invoked;
} nl_response_forward_to;

static int dwpal_ext_nl80211_callback(struct nl_msg *msg, void *arg)
{
	nl_response_forward_to *to = (nl_response_forward_to*)arg;
	wv_ipc_msg *resp;
	dwpald_header resp_hdr = { 0 };

	LOG(2, "dwpal_ext_nl80211_callback");

	if (to == NULL)
		return NL_SKIP;

	to->invoked = 1;

	resp = dwpald_ipc_msg_from_nl_msg(msg);
	if (resp == NULL) {
		ELOG("failed to copy");
		return NL_SKIP;
	}

	resp_hdr.header[0] = DWPALD_CMD_RESP;
	resp_hdr.header[1] = DWPALD_IF_TYPE_KERNEL;
	resp_hdr.header[2] = 1; /* contains message */
	resp_hdr.header[3] = 0;
	resp_hdr.header[4] = DWPAL_SUCCESS;

	if (dwpald_header_push(resp, &resp_hdr)) {
		ELOG("failed to push header");
		wave_ipc_msg_put(resp);
		return NL_SKIP;
	}

	wave_ipcs_send_response_to(to->ipserver, to->ipsta, to->seq_num,
				   resp, to->is_multi_msg);
	wave_ipc_msg_put(resp);

	return NL_SKIP;
}

static int nl_execute_command(wv_ipserver *ipserv, wv_ipc_msg *cmd,
			      wv_ipstation *ipsta, uint8_t seq_num)
{
	struct nl_msg *msg = NULL;
	struct nlmsghdr *hdr;
	nl_response_forward_to to;
	DWPAL_Ret dpal_ret;
	int res = 0;

	LOG(2, "executing nl command from serializer ctx");

	msg = dwpald_nl_msg_from_ipc_msg(cmd);
	if (msg == NULL) {
		ELOG("failed to get nl_msg from ipc msg");
		return 1;
	}

	hdr = nlmsg_hdr(msg);

	to.ipserver = ipserv;
	to.ipsta = ipsta;
	to.seq_num = seq_num;
	to.invoked = 0;
	to.is_multi_msg = !!(hdr->nlmsg_flags & NLM_F_DUMP);

	dpal_ret = dwpal_ext_nl80211_cmd_send(msg, &res, dwpal_ext_nl80211_callback, &to, false);
	if (dpal_ret != DWPAL_SUCCESS)
		ELOG("nl80211 cmd send returned err %d", dpal_ret);

	if (!to.invoked || to.is_multi_msg) {
		wv_ipc_msg *resp;
		dwpald_header resp_hdr = { 0 };

		if ((resp = wave_ipc_msg_alloc()) == NULL)
			return 1;;

		resp_hdr.header[0] = DWPALD_CMD_RESP;
		resp_hdr.header[1] = DWPALD_IF_TYPE_KERNEL;
		resp_hdr.header[2] = 0;
		resp_hdr.header[3] = res;
		resp_hdr.header[4] = dpal_ret;

		LOG(2, "sending ack/finish/error result");
		dwpald_header_push(resp, &resp_hdr);
		wave_ipcs_send_response_to(ipserv, ipsta, seq_num, resp, 0);
		wave_ipc_msg_put(resp);
	}

	return 0;
}

static DWPAL_Ret dwpal_nlVendorEventCallback(char *ifname, int event, int subevent,
					     size_t len, unsigned char *data)
{
	wv_ipc_msg *e_msg;
	dwpald_header hdr = { 0 };
	char *event_data;
	size_t total_msg_size = 0, reserve_size;
	uint16_t data_size = (uint16_t)len;
	int *extra_info = NULL;

	LOG(2, "got dwpal_nlVendorEventCallback ifname=%s event=%d sub=%d len=%zu",
	    ifname, event, subevent, len);

	if (event != NL80211_CMD_VENDOR) {
		BUG("event is not vendor event");
		return DWPAL_FAILURE;
	}

	if ((size_t)data_size != len) {
		ELOG("data_size err, len=%zu", len);
		return DWPAL_FAILURE;
	}

	if ((e_msg = wave_ipc_msg_alloc()) == NULL)
		return DWPAL_FAILURE;

	hdr.header[0] = DWPALD_EVENT;
	hdr.header[1] = DWPALD_IF_TYPE_DRIVER;
	hdr.header[2] = strlen(ifname);
	wv_aligned_16_bit_assign(&hdr.header[3], data_size);
	dwpald_header_push(e_msg, &hdr);

	reserve_size = sizeof(int) + hdr.header[2] + data_size;
	if (wave_ipc_msg_reserve_data(e_msg, reserve_size) != WAVE_IPC_SUCCESS) {
		ELOG("could not reserve %zu data in ipc msg", reserve_size);
		wave_ipc_msg_put(e_msg);
		return DWPAL_FAILURE;
	}

	if ((event_data = wave_ipc_msg_get_data(e_msg)) == NULL) {
		wave_ipc_msg_put(e_msg);
		return DWPAL_FAILURE;
	}

	*((int*)(event_data)) = subevent;
	total_msg_size += sizeof(int);

	memcpy_s(event_data + total_msg_size, reserve_size - total_msg_size,
			ifname, hdr.header[2]);
	total_msg_size += hdr.header[2];

	if (data_size)
		memcpy_s(event_data + total_msg_size, reserve_size - total_msg_size,
				data, data_size);

	wave_ipcs_push_event_header(e_msg);

	extra_info = (int*)malloc(sizeof(int));
	if (!extra_info) {
		wave_ipc_msg_put(e_msg);
		return DWPAL_FAILURE;
	}

	*extra_info = subevent;

	iface_manager_event_received(__manager, e_msg, DWPALD_NL_DRV_IFNAME,
				     sizeof(DWPALD_NL_DRV_IFNAME), extra_info);

	return DWPAL_SUCCESS;
}

static DWPAL_Ret dwpal_nlNonVendorEventCallback(struct nl_msg *msg)
{
	wv_ipc_msg *e_msg;
	dwpald_header hdr = { 0 };

	LOG(2, "got dwpal_nlNonVendorEventCallback");

	e_msg = dwpald_ipc_msg_from_nl_msg(msg);
	if (e_msg == NULL) {
		ELOG("failed to copy nl_msg into ipc_msg");
		return 1;
	}

	hdr.header[0] = DWPALD_EVENT;
	hdr.header[1] = DWPALD_IF_TYPE_KERNEL;
	if (dwpald_header_push(e_msg, &hdr)) {
		ELOG("failed to push header");
		wave_ipc_msg_put(e_msg);
		return 1;
	}

	wave_ipcs_push_event_header(e_msg);

	iface_manager_event_received(__manager, e_msg, DWPALD_NL_DRV_IFNAME,
				     sizeof(DWPALD_NL_DRV_IFNAME), NULL);

	return DWPAL_SUCCESS;
}

static int nl_iface_attach(iface_manager *manager, char *ifname,
			   uint8_t *state)
{
	int nl80211_id = 0;
	DWPAL_Ret ret;

	(void)ifname;

	__manager = manager;

	if (!is_attached) {
		DWPAL_Ret dwpal_ret;

		LOG(2, "attaching to kernel/driver via dwpal_ext");
		dwpal_ret = dwpal_ext_driver_nl_attach(dwpal_nlVendorEventCallback,
						       dwpal_nlNonVendorEventCallback);
		if (dwpal_ret == DWPAL_FAILURE) {
			BUG("driver nl interface attach with dwpal failed");
			return 1;
		}

		is_attached = 1;
	}

	ret = dwpal_ext_nl80211_id_get(&nl80211_id);
	if (ret)
		return 1;

	*state = (uint8_t)nl80211_id;

	return 0;
}

static int nl_iface_detach(char *ifname)
{
	(void)ifname;

	dwpal_ext_driver_nl_detach();
	is_attached = 0;
	return 0;
}

static int nl_manager_register_sta_to_drv_event(l_list *events, wv_ipstation *ipsta,
						uint32_t nl_id)
{
	drv_event *event = NULL;

	list_foreach_start(events, tmp, drv_event)
		if (tmp->nl_id == nl_id) {
			event = tmp;
			break;
		}
	list_foreach_end

	if (event) {
		list_remove(event->registered_stations, ipsta);
		list_push_front(event->registered_stations, ipsta);
		LOG(2, "registered %s to existing drv event %u",
		    wave_ipcs_sta_name(ipsta), nl_id);
	} else {
		event = (drv_event*)malloc(sizeof(drv_event));
		if (!event)
			return 1;

		if (!(event->registered_stations = list_init())) {
			free(event);
			return 1;
		}

		event->nl_id = nl_id;
		list_push_front(event->registered_stations, ipsta);
		list_push_front(events, event);
		LOG(2, "registered %s to newly created drv event %u",
		    wave_ipcs_sta_name(ipsta), nl_id);
	}

	return 0;
}

static int nl_register_sta_to_events(l_list *events, wv_ipstation *ipsta,
				     const char *reg_str, size_t len)
{
	uint32_t *data = (uint32_t*)reg_str;
	size_t i = 0, num_events;

	if (!len || !reg_str)
		return 1;

	if (len % sizeof(uint32_t)) {
		ELOG("driver events registration cmd does not align with uint32");
		return 1;
	}
	num_events = len / sizeof(uint32_t);

	for (i = 0; i < num_events; i++) {
		uint32_t nl_id = data[i];

		LOG(2, "register req for %u by %s", nl_id, wave_ipcs_sta_name(ipsta));
		if (nl_manager_register_sta_to_drv_event(events, ipsta, nl_id)) {
			ELOG("failed to register sta %s to event %u",
			     wave_ipcs_sta_name(ipsta), nl_id);
			return 1;
		}
	}

	return 0;
}

static int nl_unregister_sta_from_events(l_list *events, wv_ipstation *ipsta)
{
	list_foreach_start(events, event, drv_event)
		list_remove(event->registered_stations, ipsta);
		if (!list_get_size(event->registered_stations)) {
			list_foreach_remove_current_entry();
			LOG(2, "drv event %u is deleted due to no stations left",
			    event->nl_id);

			list_free(event->registered_stations);
			free(event);
		}
	list_foreach_end

	return 0;
}

static int nl_send_drv_event(wv_ipserver *ipserv, char *ifname, wv_ipc_msg *e_msg,
			     void *info, l_list *events, uint8_t *state)
{
	drv_event *event = NULL;
	uint32_t nl_id;

	(void)state;
	(void)ifname;

	nl_id = *(int *)info;
	list_foreach_start(events, tmp, drv_event)
		if (tmp->nl_id == nl_id) {
			event = tmp;
			break;
		}
	list_foreach_end

	if (event == NULL) {
		LOG(2, "no station registered to this event");
		return 0;
	}

	list_foreach_start(event->registered_stations, ipsta, wv_ipstation)
		wv_ipc_ret ret;

		ret = wave_ipcs_send_to(ipserv, e_msg, ipsta);
		if (ret != WAVE_IPC_SUCCESS) {
			LOG(2, "failed to send this event to %s",
				wave_ipcs_sta_name(ipsta));
		}
	list_foreach_end

	return 0;
}

static manager_apis apis = {
	.execute_command = nl_execute_command,
	.iface_attach = nl_iface_attach,
	.iface_detach = nl_iface_detach,
	.register_sta_to_events = nl_register_sta_to_events,
	.unregister_sta_from_events = nl_unregister_sta_from_events,
	.send_event = nl_send_drv_event,
};

manager_apis * nl_man_apis_get(void)
{
	return &apis;
}
