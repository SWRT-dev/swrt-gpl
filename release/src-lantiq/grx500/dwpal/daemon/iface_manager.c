/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "iface_manager.h"
#include "work_serializer.h"
#include "dwpal_daemon.h"
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

typedef struct _attached_interface {
	char ifname[IFNAMSIZ + 1];
	uint8_t state;
	l_list *events;
	l_list *attached_clients;
	bool keep_attached;
} attached_interface;

typedef struct _iface_manager {
	wv_ipserver *ipserver;
	manager_apis *man_apis;
	unsigned int detach_time;
	uint8_t iftype;
	work_serializer *serializer;
	l_list *attached_ifaces;
} iface_manager;

typedef struct {
	char ifname[IFNAMSIZ + 1];
	uint8_t seq_num;
	wv_ipstation *ipsta;
} detach_work;

typedef struct {
	wv_ipc_msg *cmd;
	uint8_t seq_num;
	wv_ipstation *ipsta;
} cmd_work;

typedef struct {
	wv_ipc_msg *event;
	char ifname[IFNAMSIZ + 1];
	void *info;
} event_work;

static int cmd_work_obj_clean(void *work_obj, void *ctx)
{
	cmd_work *cmd_w = (cmd_work*)work_obj;

	(void)ctx;

	if (cmd_w == NULL) return 1;
	wave_ipcs_sta_decref(cmd_w->ipsta);
	wave_ipc_msg_put(cmd_w->cmd);
	free(cmd_w);
	return 0;
}

static int event_work_obj_clean(void *work_obj, void *ctx)
{
	event_work *event_w = (event_work*)work_obj;

	(void)ctx;

	if (!event_w) return 1;
	wave_ipc_msg_put(event_w->event);
	if (event_w->info)
		free(event_w->info);
	free(event_w);
	return 0;
}

static int detach_work_obj_cmp(void *work_obj, void *cmp_to)
{
	detach_work *pending_work = (detach_work*)work_obj;
	detach_work *cancel_work = (detach_work*)cmp_to;

	if (!pending_work || !cancel_work)
		return 1;

	if (!strncmp(pending_work->ifname, cancel_work->ifname, sizeof(cancel_work->ifname)))
		return 0;

	return 1;
}

static int detach_work_obj_clean(void *work_obj, void *ctx)
{
	detach_work *detach_w = (detach_work*)work_obj;

	(void)ctx;

	if (!detach_w) return 1;
	wave_ipcs_sta_decref(detach_w->ipsta);
	free(detach_w);
	return 0;
}

static int sta_disconn_work_obj_clean(void *work_obj, void *ctx)
{
	wv_ipstation *ipsta = (wv_ipstation*)work_obj;

	(void)ctx;

	if (!ipsta) return 1;
	wave_ipcs_sta_decref(ipsta);
	return 0;
}

static int execute_cmd_work(work_serializer *s, void *work_obj, void *ctx);
static int send_event_work(work_serializer *s, void *work_obj, void *ctx);
static int iface_attach_work(work_serializer *s, void *work_obj, void *ctx);
static int iface_detach_work(work_serializer *s, void *work_obj, void *ctx);
static int sta_disconnect_work(work_serializer *s, void *work_obj, void *ctx);
static int iface_update_event_work(work_serializer *s, void *work_obj, void *ctx);


enum {
	IFACE_MAN_CMD_WORK,
	IFACE_MAN_EVENT_WORK,
	IFACE_MAN_ATTACH_WORK,
	IFACE_MAN_DETACH_WORK,
	IFACE_MAN_DISCONN_WORK,
	IFACE_MAN_UPDATE_EVENT_WORK,

	/* keep last */
	IFACE_MAN_NUM_WORK_TYPES,
};

static work_ops_t work_ops[] = {
	[IFACE_MAN_CMD_WORK] = { execute_cmd_work, cmd_work_obj_clean, NULL },
	[IFACE_MAN_EVENT_WORK] = { send_event_work, event_work_obj_clean, NULL },
	[IFACE_MAN_ATTACH_WORK] = { iface_attach_work, cmd_work_obj_clean, NULL },
	[IFACE_MAN_DETACH_WORK] = { iface_detach_work, detach_work_obj_clean, detach_work_obj_cmp },
	[IFACE_MAN_DISCONN_WORK] = { sta_disconnect_work, sta_disconn_work_obj_clean, NULL },
	[IFACE_MAN_UPDATE_EVENT_WORK] = { iface_update_event_work, cmd_work_obj_clean, NULL },
};

iface_manager * iface_manager_init(wv_ipserver *ipserver, manager_apis *man_apis,
				   l_list *seed_ifaces, uint8_t iftype,
				   unsigned int detach_time)
{
	iface_manager *manager;
	int ret;

	manager = (iface_manager*)malloc(sizeof(iface_manager));
	if (manager == NULL)
		return NULL;

	memset(manager, 0, sizeof(iface_manager));
	manager->man_apis = man_apis;
	manager->iftype = iftype;
	manager->detach_time = detach_time;

	if ((manager->attached_ifaces = list_init()) == NULL)
		goto err;

	if (!seed_ifaces)
		goto after_seed;

	list_foreach_start(seed_ifaces, ifname, char)
		attached_interface *attached_if = malloc(sizeof(attached_interface));
		if (attached_if == NULL)
			goto err;

		attached_if->events = list_init();
		if (!attached_if->events) {
			free(attached_if);
			goto err;
		}

		attached_if->attached_clients = list_init();
		if (!attached_if->attached_clients) {
			list_free(attached_if->events);
			free(attached_if);
			goto err;
		}

		strncpy_s(attached_if->ifname, sizeof(attached_if->ifname),
			  ifname, sizeof(attached_if->ifname) - 1);
		attached_if->state = INTERFACE_DWPAL_STATE_UNKNOWN;
		attached_if->keep_attached = true;

		list_push_back(manager->attached_ifaces, attached_if);
	list_foreach_end

after_seed:
	manager->ipserver = ipserver;
	manager->serializer = serializer_create(work_ops, IFACE_MAN_NUM_WORK_TYPES, 1);
	if (manager->serializer == NULL)
		goto err;

	list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
		ret = man_apis->iface_attach(manager, attached_if->ifname,
					     &attached_if->state);
		if (ret)
			goto err;
	list_foreach_end

	return manager;

err:
	if (manager->attached_ifaces) {
		list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
			man_apis->iface_detach(attached_if->ifname);
		list_foreach_end
	}
	if (manager->serializer)
		serializer_destroy(manager->serializer);
	if (manager->attached_ifaces) {
		list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
			list_free(attached_if->events);
			list_free(attached_if->attached_clients);
			free(attached_if);
			list_foreach_remove_current_entry()
		list_foreach_end
		list_free(manager->attached_ifaces);
	}
	free(manager);
	return NULL;
}

int iface_manager_deinit(iface_manager *manager)
{
	if (manager == NULL)
		return 1;

	list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
		manager->man_apis->iface_detach(attached_if->ifname);
	list_foreach_end

	serializer_destroy(manager->serializer);

	list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
		list_free(attached_if->events);
		list_free(attached_if->attached_clients);
		free(attached_if);
		list_foreach_remove_current_entry()
	list_foreach_end
	list_free(manager->attached_ifaces);

	free(manager);
	return 0;
}

int iface_manager_sta_cmd_async(iface_manager *manager, wv_ipstation *ipsta,
				uint8_t seq_num, wv_ipc_msg *cmd)
{
	dwpald_header cmd_hdr = { 0 };
	void *work_obj = NULL;
	unsigned int work_id;
	free_cb work_obj_free_func;

	if (manager == NULL || cmd == NULL || ipsta == NULL)
		goto err;

	if (dwpald_header_peek(cmd, &cmd_hdr)) {
		ELOG("failed to get dwpald header from cmd of '%s'", wave_ipcs_sta_name(ipsta));
		goto err;
	}

	switch (cmd_hdr.header[0]) {
	case DWPALD_CMD:
	case DWPALD_ATTACH_REQ:
		{
			cmd_work *work = (cmd_work*)malloc(sizeof(cmd_work));
			if (!work)
				goto err;

			work->cmd = cmd;
			work->ipsta = ipsta;
			work->seq_num = seq_num;
			wave_ipcs_sta_incref(ipsta);

			work_obj = work;
			work_obj_free_func = cmd_work_obj_clean;

			if (cmd_hdr.header[0] == DWPALD_CMD)
				work_id = IFACE_MAN_CMD_WORK;
			else
				work_id = IFACE_MAN_ATTACH_WORK;
		}
		break;
	case DWPALD_DETACH_REQ:
		{
			void *data = wave_ipc_msg_get_data(cmd);
			size_t data_size = wave_ipc_msg_get_size(cmd);
			detach_work *work;

			if (data_size != sizeof(work->ifname))
				goto err;

			work = (detach_work*)malloc(sizeof(detach_work));
			if (!work)
				goto err;

			work->ipsta = ipsta;
			work->seq_num = seq_num;
			wave_ipcs_sta_incref(ipsta);
			strncpy_s(work->ifname, sizeof(work->ifname),
				  data, sizeof(work->ifname) - 1);

			wave_ipc_msg_put(cmd);
			cmd = NULL;

			work_obj = work;
			work_id = IFACE_MAN_DETACH_WORK;
			work_obj_free_func = detach_work_obj_clean;
		}
		break;
	case DWPALD_UPDATE_EVENT_REQ:
		{
			cmd_work *work = (cmd_work*)malloc(sizeof(cmd_work));
			if (!work)
				goto err;

			work->cmd = cmd;
			work->ipsta = ipsta;
			work->seq_num = seq_num;
			wave_ipcs_sta_incref(ipsta);

			work_obj = work;
			work_obj_free_func = cmd_work_obj_clean;
			work_id = IFACE_MAN_UPDATE_EVENT_WORK;
		}
		break;
	default:
		ELOG("received unsupported command (%d) from '%s'",
		     cmd_hdr.header[0], wave_ipcs_sta_name(ipsta));
		goto err;
	}

	if (serializer_exec_work_async(manager->serializer, work_id,
				       work_obj, manager)) {
		work_obj_free_func(work_obj, manager);
		return 1;
	}

	return 0;
err:
	if (cmd)
		wave_ipc_msg_put(cmd);
	return 1;
}

int iface_manager_event_received(iface_manager *manager, wv_ipc_msg *event,
				 const char *ifname, size_t ifnamsiz, void *info)
{
	event_work *work = (event_work*)malloc(sizeof(event_work));
	if (!work) {
		wave_ipc_msg_put(event);
		return 1;
	}

	work->event = event;
	work->info = info;
	strncpy_s(work->ifname, sizeof(work->ifname), ifname, ifnamsiz - 1);

	if (serializer_exec_work_async(manager->serializer, IFACE_MAN_EVENT_WORK,
				       work, manager)) {
		event_work_obj_clean(work, manager);
		return 1;
	}

	return 0;
}

int iface_manager_sta_disconnected(iface_manager *manager, wv_ipstation *ipsta)
{
	wave_ipcs_sta_incref(ipsta);

	if (serializer_exec_work_async(manager->serializer, IFACE_MAN_DISCONN_WORK,
				       ipsta, manager)) {
		wave_ipcs_sta_decref(ipsta);
		return 1;
	}

	return 0;
}

static int execute_cmd_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	cmd_work *cmd_w = (cmd_work*)work_obj;
	int ret;

	(void)s;

	if (cmd_w == NULL || manager == NULL) return 1;

	ret = manager->man_apis->execute_command(manager->ipserver, cmd_w->cmd,
						 cmd_w->ipsta, cmd_w->seq_num);
	if (ret) {
		wave_ipcs_send_req_failed_to(manager->ipserver, cmd_w->ipsta,
					     cmd_w->seq_num);
		return 1;
	}

	return 0;
}

static int send_event_to_sta_list(iface_manager *manager, l_list *clients,
				  wv_ipc_msg *event)
{
	list_foreach_start(clients, ipsta, wv_ipstation)
		wv_ipc_ret ret;

		ret = wave_ipcs_send_to(manager->ipserver, event, ipsta);
		if (ret != WAVE_IPC_SUCCESS) {
			LOG(2, "failed to send this event to %s",
				wave_ipcs_sta_name(ipsta));
		}
	list_foreach_end

	return 0;
}

static int send_event_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	event_work *event_w = (event_work*)work_obj;

	(void)s;

	if (!event_w || manager == NULL) return 1;

	list_foreach_start(manager->attached_ifaces, attached_if, attached_interface)
		if (!strncmp(event_w->ifname, attached_if->ifname, sizeof(attached_if->ifname))) {

			if (!event_w->info)
				return send_event_to_sta_list(manager,
							      attached_if->attached_clients,
							      event_w->event);

			return manager->man_apis->send_event(manager->ipserver,
							     attached_if->ifname,
							     event_w->event,
							     event_w->info,
							     attached_if->events,
							     &attached_if->state);
		}
	list_foreach_end

	return 1;
}

static int iface_attach_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	cmd_work *cmd_w = (cmd_work*)work_obj;
	attached_interface *attached_iface = NULL;
	wv_ipc_msg *cmd;
	wv_ipc_msg *resp;
	dwpald_header hdr = { 0 };
	char ifname[16 + 1];
	wv_ipstation *ipsta;
	size_t data_size;
	char *data;
	int ret;

	(void)s;

	if (cmd_w == NULL || manager == NULL) return 1;

	cmd = cmd_w->cmd;
	ipsta = cmd_w->ipsta;

	data = wave_ipc_msg_get_data(cmd);
	data_size = wave_ipc_msg_get_size(cmd);

	if (data_size < sizeof(ifname) || !data)
		goto err;;

	strncpy_s(ifname, sizeof(ifname), data, sizeof(ifname) - 1);
	data += sizeof(ifname);
	data_size -= sizeof(ifname);

	list_foreach_start(manager->attached_ifaces, tmp, attached_interface)
		if (!strncmp(tmp->ifname, ifname, sizeof(tmp->ifname))) {
			attached_iface = tmp;
			break;
		}
	list_foreach_end

	if (!attached_iface) {
		attached_iface = (attached_interface*)malloc(sizeof(attached_interface));
		if (!attached_iface)
			goto err;

		attached_iface->events = list_init();
		if (!attached_iface->events) {
			free(attached_iface);
			goto err;
		}

		attached_iface->attached_clients = list_init();
		if (!attached_iface->attached_clients) {
			list_free(attached_iface->events);
			free(attached_iface);
			goto err;
		}

		strncpy_s(attached_iface->ifname, sizeof(attached_iface->ifname),
			  ifname, sizeof(attached_iface->ifname));
		attached_iface->keep_attached = false;

		ret = manager->man_apis->iface_attach(manager, ifname,
						      &attached_iface->state);
		if (ret) {
			list_free(attached_iface->attached_clients);
			list_free(attached_iface->events);
			free(attached_iface);
			goto err;
		}

		list_push_back(manager->attached_ifaces, attached_iface);
	} else {
		ret = manager->man_apis->iface_attach(manager, ifname,
						      &attached_iface->state);
		if (ret)
			goto err;
	}

	list_remove(attached_iface->attached_clients, ipsta);
	list_push_back(attached_iface->attached_clients, ipsta);

	if (data_size) {
		ret = manager->man_apis->register_sta_to_events(attached_iface->events,
								ipsta, data, data_size);
		if (ret)
			goto err;
	}

	if ((resp = wave_ipc_msg_alloc()) == NULL)
		goto err;

	hdr.header[0] = DWPALD_ATTACH_RESP;
	hdr.header[1] = manager->iftype;
	hdr.header[2] = attached_iface->state;
	dwpald_header_push(resp, &hdr);
	wave_ipcs_send_response_to(manager->ipserver, ipsta, cmd_w->seq_num, resp, 0);
	wave_ipc_msg_put(resp);
	return 0;

err:
	wave_ipcs_send_req_failed_to(manager->ipserver, ipsta, cmd_w->seq_num);
	return 1;
}

static int iface_update_event_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	cmd_work *cmd_w = (cmd_work*)work_obj;
	attached_interface *attached_iface = NULL;
	wv_ipc_msg *cmd;
	wv_ipc_msg *resp;
	dwpald_header hdr = { 0 };
	char ifname[ IFNAMSIZ + 1];
	wv_ipstation *ipsta;
	size_t data_size;
	char *data;

	(void)s;

	if (cmd_w == NULL || manager == NULL) return 1;

	cmd = cmd_w->cmd;
	ipsta = cmd_w->ipsta;

	data = wave_ipc_msg_get_data(cmd);
	data_size = wave_ipc_msg_get_size(cmd);

	if (data_size < sizeof(ifname) || !data)
		goto err;

	strncpy_s(ifname, sizeof(ifname), data, sizeof(ifname) - 1);
	data += sizeof(ifname);
	data_size -= sizeof(ifname);

	list_foreach_start(manager->attached_ifaces, tmp, attached_interface)
		if (!strncmp(tmp->ifname, ifname, sizeof(tmp->ifname))) {
			attached_iface = tmp;
			break;
		}
	list_foreach_end

	if (!attached_iface)
			goto err;
	if (data_size) {
		if(!manager->man_apis->unregister_sta_from_events(attached_iface->events,
								ipsta)) {
			if(manager->man_apis->register_sta_to_events(attached_iface->events,
								ipsta, data, data_size)) {
				goto err;
			}
		}
		else
			goto err;
	}

	if ((resp = wave_ipc_msg_alloc()) == NULL)
		goto err;

	hdr.header[0] = DWPALD_UPDATE_EVENT_RESP;
	hdr.header[1] = manager->iftype;
	dwpald_header_push(resp, &hdr);
	wave_ipcs_send_response_to(manager->ipserver, ipsta, cmd_w->seq_num, resp, 0);
	wave_ipc_msg_put(resp);
	return 0;

err:
	wave_ipcs_send_req_failed_to(manager->ipserver, ipsta, cmd_w->seq_num);
	return 1;
}

static int iface_detach_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	detach_work *detach_w = (detach_work*)work_obj;
	attached_interface *attached_iface = NULL;
	detach_work *future_detach = NULL;

	(void)s;

	future_detach = (detach_work*)malloc(sizeof(detach_work));
	if (!future_detach)
		return 1;

	strncpy_s(future_detach->ifname, sizeof(future_detach->ifname),
		  detach_w->ifname, sizeof(future_detach->ifname) - 1);
	future_detach->ipsta = NULL;

	serializer_cancel_delayed_work(manager->serializer, IFACE_MAN_DETACH_WORK,
				       future_detach, manager);

	list_foreach_start(manager->attached_ifaces, tmp, attached_interface)
		if (!strncmp(tmp->ifname, detach_w->ifname, sizeof(tmp->ifname))) {
			attached_iface = tmp;
			break;
		}
	list_foreach_end

	if (!attached_iface)
		goto err;

	if (detach_w->ipsta) {
		wv_ipc_msg *resp;
		dwpald_header hdr = { 0 };

		manager->man_apis->unregister_sta_from_events(attached_iface->events,
							      detach_w->ipsta);

		list_remove(attached_iface->attached_clients, detach_w->ipsta);
		if (attached_iface->keep_attached == false &&
		    list_get_size(attached_iface->attached_clients) == 0)
			serializer_add_delayed_work(manager->serializer, IFACE_MAN_DETACH_WORK,
						    future_detach, manager, manager->detach_time, 0);

		hdr.header[0] = DWPALD_DETACH_RESP;
		hdr.header[1] = manager->iftype;

		if ((resp = wave_ipc_msg_alloc()) == NULL)
			return 1;

		dwpald_header_push(resp, &hdr);
		wave_ipcs_send_response_to(manager->ipserver, detach_w->ipsta,
					   detach_w->seq_num, resp, 0);
		wave_ipc_msg_put(resp);
	} else {
		free(future_detach);

		if (attached_iface->keep_attached == false &&
		    list_get_size(attached_iface->attached_clients) == 0) {
			manager->man_apis->iface_detach(detach_w->ifname);

			LOG(1, "removing attached interface %s", attached_iface->ifname);
			list_free(attached_iface->events);
			list_free(attached_iface->attached_clients);
			list_remove(manager->attached_ifaces, attached_iface);
			free(attached_iface);
		}
	}

	return 0;

err:
	if (future_detach)
		free(future_detach);
	if (detach_w->ipsta)
		wave_ipcs_send_req_failed_to(manager->ipserver, detach_w->ipsta,
					     detach_w->seq_num);
	return 1;
}

static int sta_disconnect_work(work_serializer *s, void *work_obj, void *ctx)
{
	iface_manager *manager = (iface_manager*)ctx;
	wv_ipstation *ipsta = (wv_ipstation*)work_obj;

	(void)s;

	if (!ipsta) return 1;

	list_foreach_start(manager->attached_ifaces, attached_iface, attached_interface)
		manager->man_apis->unregister_sta_from_events(attached_iface->events,
							      ipsta);
		list_remove(attached_iface->attached_clients, ipsta);
		if (attached_iface->keep_attached == false &&
		    list_get_size(attached_iface->attached_clients) == 0) {
			detach_work *future_detach = NULL;

			future_detach = (detach_work*)malloc(sizeof(detach_work));
			if (!future_detach)
				continue;

			strncpy_s(future_detach->ifname, sizeof(future_detach->ifname),
				  attached_iface->ifname, sizeof(attached_iface->ifname) - 1);
			future_detach->ipsta = NULL;

			serializer_cancel_delayed_work(manager->serializer, IFACE_MAN_DETACH_WORK,
						       future_detach, manager);
			serializer_add_delayed_work(manager->serializer, IFACE_MAN_DETACH_WORK,
						    future_detach, manager, manager->detach_time, 0);
		}
	list_foreach_end

	return 0;
}
