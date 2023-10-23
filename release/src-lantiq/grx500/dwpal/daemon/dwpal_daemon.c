/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "dwpal_daemon.h"
#include "wave_ipc_server.h"
#include "linked_list.h"
#include "logs.h"

#include "iface_manager.h"
#include "hostap_iface.h"
#include "nl_iface.h"
#include "stadb.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

#ifdef INCLUDE_BREAKPAD
#include "breakpad_wrapper.h"
#endif

static char *server_name = DWPALD_SERVER_NAME;
static unsigned int detach_time = 60; /* 1 minute */

struct _dwpal_daemon {
	wv_ipserver *ipserver;

	station_db stadb;

	iface_manager *hap_man;
	iface_manager *nl_man;

	int terminate;
	int term_from_signum;
} dwpald = { 0 };

static void forceful_termination_handler(int signum)
{
	(void)signum;

	exit(1);
}

static void termination_handler(int signum)
{
	dwpald.terminate = 1;
	dwpald.term_from_signum = signum;
	signal(SIGALRM, forceful_termination_handler);
	alarm(2);
}

static int dwpald_setup_signal_handlers(void)
{
	struct sigaction signal_action;
	signal_action.sa_handler = termination_handler;
	sigemptyset(&signal_action.sa_mask);
	signal_action.sa_flags = 0;

	if (sigaction(SIGINT, &signal_action, NULL)) {
		ELOG("sigaction for SIGINT returned err (errno=%d)", errno);
		return 1;
	}

	if (sigaction(SIGTERM, &signal_action, NULL)) {
		ELOG("sigaction for SIGTERM returned err (errno=%d)", errno);
		return 1;
	}

	if (sigaction(SIGHUP, &signal_action, NULL)) {
		ELOG("sigaction for SIGHUP returned err (errno=%d)", errno);
		return 1;
	}

	return 0;
}

static int dwpald_cmd_async(wv_ipserver *ipserv, wv_ipstation *ipsta,
			    uint8_t seq_num, wv_ipc_msg *cmd)
{
	int res;
	dwpald_header cmd_hdr = { 0 };

	(void)ipserv;

	if (dwpald_header_peek(cmd, &cmd_hdr)) {
		ELOG("failed to get dwpald header from cmd of '%s'",
		     wave_ipcs_sta_name(ipsta));
		wave_ipc_msg_put(cmd);
		return 1;
	}

	switch (cmd_hdr.header[0]) {
	case DWPALD_CMD:
	case DWPALD_ATTACH_REQ:
	case DWPALD_DETACH_REQ:
	case DWPALD_UPDATE_EVENT_REQ:
		LOG(2, "received iftype:%hhu %d command from '%s'",
		    cmd_hdr.header[0], cmd_hdr.header[1], wave_ipcs_sta_name(ipsta));

		if (cmd_hdr.header[1] == DWPALD_IF_TYPE_HOSTAP) {
			res = iface_manager_sta_cmd_async(dwpald.hap_man, ipsta,
							  seq_num, cmd);
		} else if (cmd_hdr.header[1] == DWPALD_IF_TYPE_KERNEL) {
			res = iface_manager_sta_cmd_async(dwpald.nl_man, ipsta,
							  seq_num, cmd);
		} else {
			ELOG("iftype %d is not supported. sta='%s'",
			     cmd_hdr.header[1], wave_ipcs_sta_name(ipsta));
			wave_ipc_msg_put(cmd);
			res = 1;
		}
		break;
#ifdef CONFIG_DWPALD_DEBUG_TOOLS
	case DWPALD_TERMINATE_REQ:
		LOG(1, "received terminate command from '%s'",
		    wave_ipcs_sta_name(ipsta));

		wave_ipc_msg_put(cmd);
		{
			wv_ipc_ret ret;
			wv_ipc_msg *resp;
			dwpald_header resp_hdr = { 0 };
			resp_hdr.header[0] = DWPALD_TERMINATE_RESP;

			if ((resp = wave_ipc_msg_alloc()) == NULL)
				return 1;

			dwpald_header_push(resp, &resp_hdr);
			ret = wave_ipcs_send_response_to(ipserv, ipsta,
							 seq_num, resp, 0);
			wave_ipc_msg_put(resp);
			if (ret != WAVE_IPC_SUCCESS) {
				ELOG("send_response_to '%s' returned err (ret=%d)",
				     wave_ipcs_sta_name(ipsta), ret);
				return 1;
			}

			/* don't send ipc_req_failed, since we already answered */
			res = 0;
		}
		/* this will cause daemon to stop within ~1 second */
		dwpald.terminate = 1;
		break;
	case DWPALD_CONNECTED_CLIENTS_REQ:
		wave_ipc_msg_put(cmd);
		{
			wv_ipc_ret ret;
			wv_ipc_msg *resp;
			dwpald_header resp_hdr = { 0 };
			resp_hdr.header[0] = DWPALD_CONNECTED_CLIENTS_RESP;
			char response[1024] = { 0 };
			size_t resp_len = 0;
			unsigned int i = 1;

			if ((resp = wave_ipc_msg_alloc()) == NULL)
				return 1;

			dwpald_header_push(resp, &resp_hdr);

			LOCK_STA_DB(&dwpald.stadb);
			list_foreach_start(dwpald.stadb.sta_list, ipsta_tmp, wv_ipstation)
				int res = sprintf_s(&response[resp_len],
						       sizeof(response) - resp_len,
						       "%u. %s\n", i++,
						       wave_ipcs_sta_name(ipsta_tmp));
				if (res > 0)
					resp_len += res;
				else {
					// TODO: return faulure ?
				}
			list_foreach_end
			UNLOCK_STA_DB(&dwpald.stadb);

			wave_ipc_msg_fill_data(resp, response, resp_len);
			ret = wave_ipcs_send_response_to(ipserv, ipsta,
							 seq_num, resp, 0);
			wave_ipc_msg_put(resp);
			if (ret != WAVE_IPC_SUCCESS) {
				ELOG("send_response_to '%s' returned err (ret=%d)",
				     wave_ipcs_sta_name(ipsta), ret);
				return 1;
			}

			/* don't send ipc_req_failed, since we already answered */
			res = 0;
		}
		break;
#endif

	default:
		ELOG("received unsupported command (%d) from '%s'",
		     cmd_hdr.header[0], wave_ipcs_sta_name(ipsta));
		wave_ipc_msg_put(cmd);
		return 1;
	}

	return res;
}

static int dwpald_stop_cond(wv_ipserver *ipserv)
{
	(void)ipserv;

	if (dwpald.terminate && dwpald.term_from_signum)
		ELOG("cought signal '%d' ==> stopping daemon", dwpald.term_from_signum);

	return dwpald.terminate;
}

static int dwpald_adding_client(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	(void)ipserv;

	LOG(1, "dwpald client '%s' connected to the daemon",
	    wave_ipcs_sta_name(ipsta));

	LOCK_STA_DB(&dwpald.stadb);
	if (list_push_front(dwpald.stadb.sta_list, ipsta)) {
		ELOG("list_remove returned err");
		UNLOCK_STA_DB(&dwpald.stadb);
		return 1;
	}
	UNLOCK_STA_DB(&dwpald.stadb);
	wave_ipcs_sta_incref(ipsta);

	return 0;
}

static int dwpald_removing_client(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	(void)ipserv;

	LOG(1, "dwpald client '%s' disconnected from the daemon",
	    wave_ipcs_sta_name(ipsta));

	LOCK_STA_DB(&dwpald.stadb);
	if (list_remove(dwpald.stadb.sta_list, ipsta)) {
		UNLOCK_STA_DB(&dwpald.stadb);
		ELOG("list_remove returned err");
		return 1;
	}
	iface_manager_sta_disconnected(dwpald.hap_man, ipsta);
	iface_manager_sta_disconnected(dwpald.nl_man, ipsta);
	UNLOCK_STA_DB(&dwpald.stadb);
	wave_ipcs_sta_decref(ipsta);

	return 0;
}

wv_ipserver_callbacks callbacks = {
	.cmd_async = dwpald_cmd_async,
	.stop_cond = dwpald_stop_cond,
	.adding_client = dwpald_adding_client,
	.removing_client = dwpald_removing_client,
};

static int run_dwpald_daemon(l_list *hostap_ifaces)
{
	int ret = 1;

	memset(&dwpald, 0, sizeof(dwpald));

	if (dwpald_setup_signal_handlers()) {
		ELOG("failed to setup signal handlers");
		goto end;
	}

	LOG(2, "creating station db");
	dwpald.stadb.sta_list = list_init();
	pthread_mutex_init(&dwpald.stadb.lock, NULL);

	LOG(2, "creating ipc server");
	if (WAVE_IPC_SUCCESS != wave_ipcs_create(&dwpald.ipserver, server_name)) {
		ELOG("ipcs create returned error");
		goto end;
	}

	LOG(2, "creating hostap manager");
	dwpald.hap_man = iface_manager_init(dwpald.ipserver, hostap_man_apis_get(),
					    hostap_ifaces, DWPALD_IF_TYPE_HOSTAP, detach_time);

	LOG(2, "creating nl manager");
	dwpald.nl_man = iface_manager_init(dwpald.ipserver, nl_man_apis_get(),
					   NULL, DWPALD_IF_TYPE_KERNEL, detach_time);

	LOG(1, "runnig ipc server");
	if (WAVE_IPC_SUCCESS != wave_ipcs_run(dwpald.ipserver, &callbacks)) {
		ELOG("ipcs run returned error");
		goto end;
	} else
		ret = 0;

end:
	LOG(1, "stopping ipc server");
	if (dwpald.ipserver != NULL &&
	    WAVE_IPC_SUCCESS != wave_ipcs_delete(&dwpald.ipserver))
		ELOG("ipcs delete returned error");

	LOG(1, "stopping hostap manager");
	if (dwpald.hap_man)
		iface_manager_deinit(dwpald.hap_man);

	LOG(1, "stopping nl manager");
	if (dwpald.nl_man)
		iface_manager_deinit(dwpald.nl_man);

	if (dwpald.stadb.sta_list)
		list_free(dwpald.stadb.sta_list);

	return ret;
}

static void usage(void)
{
	LOG(1, "usage: dwpal_daemon [-i<ifname>] [-hsdB]\n"
	    "Options:\n"
	    "   -h           help (show this text)\n"
	    "   -i<ifname>   hostap interface to attach to via dwpal\n"
	    "   -B           run as daemon in the background\n"
	    "   -d           increase log level to debug\n"
#ifdef CONFIG_DWPALD_DEBUG_TOOLS
	    "   -u           starts the server's sock under different name for unit testing\n"
#endif
	    "   -s           output to syslog");
}

int main(int argc, char *argv[]) {
	l_list *hostap_ifaces = NULL;
	extern char *optarg;
	char *ifname;
	int daemonize = 0, open_syslog = 0, opened_syslog = 0;
	int ret = 1;
	int c;

#ifdef INCLUDE_BREAKPAD
	breakpad_ExceptionHandler();
#endif

	LOG(1, "starting daemon");

	if (!(hostap_ifaces = list_init()))
		return 1;

	while ((c = getopt(argc, argv, "i:Bdhsu")) != -1) {
		switch (c) {
		case 'i':
			ifname = (char*)malloc(IFNAMSIZ + 1);
			if (!ifname)
				goto free;

			memset(ifname, 0, IFNAMSIZ + 1);
			strncpy_s(ifname, IFNAMSIZ + 1, optarg, IFNAMSIZ);
			LOG(1, "adding interface %s to hostap attach list", ifname);
			list_push_back(hostap_ifaces, ifname);
			break;
		case 'B':
			daemonize = 1;
			break;
		case 's':
			open_syslog = 1;
			break;
		case 'd':
			LOG(1, "using log level 2");
			__log_level = 2;
			break;
#ifdef CONFIG_DWPALD_DEBUG_TOOLS
		case 'u':
			server_name = DWPALD_SERVER_UTEST_NAME;
			detach_time = 1; /* 1 second */
			break;
#endif
		case 'h':
			usage();
			goto free;
			break;

		default:
			ELOG("bad arguments");
			usage();
			goto free;
		}
	}

	if (open_syslog) {
		if (wv_open_syslog("dwpal_daemon")) {
			ELOG("failed to open syslog");
			goto free;
		}
		opened_syslog = 1;
		__use_syslog = 1;
	}

	if (daemonize && daemon(0, 0))
		ELOG("daemon() call returned err, errno=%d", errno);

	ret = run_dwpald_daemon(hostap_ifaces);

free:
	while ((ifname = list_pop_front(hostap_ifaces)))
		free(ifname);
	list_free(hostap_ifaces);

	if (opened_syslog)
		wv_close_syslog();

	return ret;
}
