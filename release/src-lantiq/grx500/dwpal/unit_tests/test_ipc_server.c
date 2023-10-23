/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "stdio.h"
#include "wave_ipc_client.h"
#include "wave_ipc_server.h"
#include "unitest_helper.h"

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

static char cmd1[] = "DID YOU HEAR ABOUT UPDOG?";
static char resp1[] = "NO, WHAT'S UP DOG?";

static char cmd2[] = "Knock Knock";
static char resp2[] = "OMG! Who's there ??????";

static int __stop_cond = 0;

static int cmd_async(wv_ipserver *ipserv, wv_ipstation *ipsta, uint8_t seq_num,
		     wv_ipc_msg *cmd)
{
	char *data;
	size_t size;
	static int num_cmds = 0;

	data = wave_ipc_msg_get_data(cmd);
	size = wave_ipc_msg_get_size(cmd);

	if (size == sizeof(cmd1) && data && !strncmp(data, cmd1, sizeof(cmd1))) {
		wv_ipc_msg *reply = wave_ipc_msg_alloc();
		if (!reply) return 1;
		wave_ipc_msg_fill_data(reply, resp1, sizeof(resp1));
		wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else if (size == sizeof(cmd2) && data && !strncmp(data, cmd2, sizeof(cmd2))) {
		wv_ipc_msg *reply = wave_ipc_msg_alloc();
		if (!reply) return 1;
		wave_ipc_msg_fill_data(reply, resp2, sizeof(resp2));
		wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else {
		ELOG("err");
		__stop_cond = 1;
		return 1;
	}
	wave_ipc_msg_put(cmd);
	num_cmds++;

	return 0;
}

static int stop_cond(wv_ipserver *ipserv)
{
	(void)ipserv;
	return __stop_cond;
}

static int removing_client(wv_ipserver* ipserv, wv_ipstation *sta)
{
	(void)sta;
	(void)ipserv;
	__stop_cond = 1;
	return 0;
}

static int run_cmd_sending_client(int num_cmds)
{
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	wv_ipc_msg *cmd = NULL, *reply = NULL;
	char *data;
	size_t size;
	int i;

	usleep(100000);

	ret = wave_ipcc_connect(&handle, "unitest_client", "unitest_server");
	if (ret == WAVE_IPC_ERROR) {
		ELOG("wave_ipcc_connect returned error");
		goto err;
	}

	for (i = 0; i < num_cmds; i++) {
		cmd = wave_ipc_msg_alloc();
		if (cmd == NULL) {
			ELOG("wave_ipc_msg_alloc returned NULL i=%d", i);
			goto err;
		}

		wave_ipc_msg_fill_data(cmd, "DID YOU HEAR ABOUT UPDOG?", sizeof("DID YOU HEAR ABOUT UPDOG?"));

		ret = wave_ipcc_send_cmd(handle, cmd, &reply);
		wave_ipc_msg_put(cmd);
		cmd = NULL;
		if (ret != WAVE_IPC_SUCCESS) {
			ELOG("wave_ipcc_send_cmd returned FAILURE i=%d", i);
			goto err;
		}

		data = wave_ipc_msg_get_data(reply);
		size = wave_ipc_msg_get_size(reply);
		if (size != sizeof(resp1) || !data || strncmp(data, resp1, sizeof(resp1))) {
			ELOG("wave_ipcc_stop_listener returned error");
			goto err;
		}
		wave_ipc_msg_put(reply);
		reply = NULL;
	}

	ret = wave_ipcc_disconnect(&handle);
	if (ret == WAVE_IPC_ERROR) {
		ELOG("wave_ipcc_disconnect returned error");
		goto err;
	}

	SLOG("succesfully sent %d commands to the server", num_cmds)
	return 0;

err:
	if (handle) wave_ipcc_disconnect(&handle);
	if (cmd) wave_ipc_msg_put(cmd);
	if (reply) wave_ipc_msg_put(reply);

	return 1;
}

UNIT_TEST_DEFINE(1, create and destroy N times)

	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;
	wv_ipserver_callbacks clbs;
	int i, j;

	memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
	clbs.cmd_async = cmd_async;
	clbs.stop_cond = stop_cond;

	for (i = 0; i < 1000; i++) {
		ret = wave_ipcs_create(&handle, "unitest_server");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcs_create returned error i=%d", i);

		for (j = 0; j < 10; j++) {
			__stop_cond = 1;
			ret = wave_ipcs_run(handle, &clbs);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcs_run returned error i=%d j=%d", i, j);
		}

		ret = wave_ipcs_delete(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcs_destroy returned error i=%d", i);
		handle = NULL;
	}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) wave_ipcs_delete(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, create server and get N commands from client)

	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;
	wv_ipserver_callbacks clbs;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_cmd_sending_client(1000));
	UNIT_TEST_FORKED_PARENET
		memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
		clbs.cmd_async = cmd_async;
		clbs.stop_cond = stop_cond;
		clbs.removing_client = removing_client;

		ret = wave_ipcs_create(&handle, "unitest_server");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcs_create returned error");

		__stop_cond = 0;
		ret = wave_ipcs_run(handle, &clbs);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcs_run returned error");

		ret = wave_ipcs_delete(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcs_destroy returned error");
		handle = NULL;
UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) wave_ipcs_delete(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(ipc_server)
	ADD_TEST(1)
	ADD_TEST(2)
UNIT_TEST_MODULE_DEFINITION_DONE
