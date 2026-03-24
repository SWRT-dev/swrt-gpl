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

static char cmd3[] = "what are the top stories on the news today?";
static char* resp3[] = { "0 there's the coronavirus thing..",
			 "1 something about elections...",
			 "2 something about kim jong-un's health mystery",
			 "3 something about indepentends crashing due to lockdown",
			 "4 and that's about it" };

static char cmd4[] = "what do you have on your tesk?";
static char* resp4[] = { "0 intel laptop",
			 "1 tandem laptop",
			 "2 mouse",
			 "3 Axepoint",
			 "4 Puma7",
			 "5 A lot of cables",
			 "6 cup of coffee" };

static char event1[] = "DID YOU KNOW THAT THE BIRD WAS THE WORD?";

#define TEST1_NUM_ITER	1000
#define TEST2_NUM_ITER	10000

static size_t events_received_cnt = 0;
static size_t commands_received_cnt = 0;
static int __stop_cond = 0;
static int __err_detected = 0;

size_t __wait_for_x_events;

static int blocked_events_term(void *arg)
{
	(void)arg;

	if (__wait_for_x_events == events_received_cnt || __err_detected)
		return 1;

	return 0;
}

static int null_event_handler(void *arg, wv_ipc_msg *event)
{
	char *data;

	(void)arg;
	events_received_cnt++;
	if (NULL == wave_ipc_msg_get_data(event)) {
		ELOG("wave_ipc_msg_get_data returned NULL");
		__err_detected = 1;
		return 1;
	}

	data = wave_ipc_msg_get_data(event);
	if (wave_ipc_msg_get_size(event) != sizeof(event1) || !data ||
	    strncmp(data, event1, sizeof(event1))) {
		ELOG("strncmp failed");
		__err_detected = 1;
		return 1;
	}

	return 0;
}

static int verify_multi_msg_response(wv_ipc_msg *reply, char **arr, int arr_size);

static int cmd_sending_event_handler_aka_annoying(void *arg, wv_ipc_msg *event)
{
	wv_ipclient *handle = (wv_ipclient*)arg;
	char *e_data;

	events_received_cnt++;
	if (NULL == wave_ipc_msg_get_data(event)) {
		ELOG("wave_ipc_msg_get_data returned NULL");
		__err_detected = 1;
		return 1;
	}

	e_data = wave_ipc_msg_get_data(event);
	if (wave_ipc_msg_get_size(event) != sizeof(event1) || !e_data ||
	    strncmp(e_data, event1, sizeof(event1))) {
		ELOG("strncmp failed");
		__err_detected = 1;
		return 1;
	}

	{
		wv_ipc_msg *cmd = wave_ipc_msg_alloc();
		wv_ipc_msg *reply;
		wv_ipc_ret ret;
		char *data;
		size_t size;
		int r = rand() % 2;
		char *e_resp;
		size_t e_size;

		if (cmd == NULL)
			goto fail;

		if (r == 0) {
			wave_ipc_msg_fill_data(cmd, cmd1, sizeof(cmd1));
			e_size = sizeof(resp1);
			e_resp = resp1;
		} else
			wave_ipc_msg_fill_data(cmd, cmd3, sizeof(cmd3));

		ret = wave_ipcc_send_cmd(handle, cmd, &reply);
		wave_ipc_msg_put(cmd);
		if (ret != WAVE_IPC_SUCCESS) {
			ELOG("wave_ipcc_send_cmd return FAILURE %d, r=%d ev_rec=%zu", ret, r, events_received_cnt);
			goto fail;
		}

		if (r == 0) {
			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (data == NULL) {
				ELOG("wave_ipc_msg_get_data return NULL r=%d", r);
				goto fail;
			}

			if (size != e_size || strcmp(data, e_resp)) {
				ELOG("expected to get '%s' got '%s', r=%d", e_resp, data, r);
				goto fail;
			}
		} else if (r == 1 && verify_multi_msg_response(reply, resp3, 5)) {
			ELOG("mulit msg fail r=%d", r);
			goto fail;
		}
		wave_ipc_msg_put(reply);
	}

	return 0;

fail:
	__err_detected = 1;
	return 1;
}

static int __die_after_n_commands = TEST1_NUM_ITER;
static int __die_after_n_events = TEST1_NUM_ITER;

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
		if (reply == NULL)
			goto err;
		wave_ipc_msg_fill_data(reply, resp1, sizeof(resp1));
		wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else if (size == sizeof(cmd2) && data && !strncmp(data, cmd2, sizeof(cmd2))) {
		wv_ipc_msg *reply = wave_ipc_msg_alloc();
		if (reply == NULL)
			goto err;
		wave_ipc_msg_fill_data(reply, resp2, sizeof(resp2));
		wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else if (size == sizeof(cmd3) && data && !strncmp(data, cmd3, sizeof(cmd3))) {
		size_t i;

		for (i = 0; i < sizeof(resp3) / sizeof(resp3[0]); i++) {
			wv_ipc_msg *reply = wave_ipc_msg_alloc();
			if (reply == NULL)
				goto err;
			wave_ipc_msg_fill_data(reply, resp3[i], strlen(resp3[i]) + 1);
			wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply,
						   (i + 1 != sizeof(resp3) / sizeof(resp3[0])));
			wave_ipc_msg_put(reply);
		}
	} else if (size == sizeof(cmd4) && data && !strncmp(data, cmd4, sizeof(cmd4))) {
		size_t i;

		for (i = 0; i < sizeof(resp4) / sizeof(resp4[0]); i++) {
			wv_ipc_msg *reply = wave_ipc_msg_alloc();
			if (reply == NULL)
				goto err;
			wave_ipc_msg_fill_data(reply, resp4[i], strlen(resp4[i]) + 1);
			wave_ipcs_send_response_to(ipserv, ipsta, seq_num, reply,
						   (i + 1 != sizeof(resp4) / sizeof(resp4[0])));
			wave_ipc_msg_put(reply);
		}
	} else {
		goto err;
	}
	wave_ipc_msg_put(cmd);

	num_cmds++;
	if (num_cmds == __die_after_n_commands) {
		TLOG("answered %d cmds", num_cmds);
		__die_after_n_commands = 0;
	}
	return 0;

err:
	wave_ipc_msg_put(cmd);
	__stop_cond = 1;
	__err_detected = 1;
	return 1;
}

static int stop_cond(wv_ipserver *ipserv)
{
	(void)ipserv;
	return (__stop_cond) || (__die_after_n_events == 0 && __die_after_n_commands == 0);
}

static int run_basic_server(int die_after_n_commands)
{
	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;

	wv_ipserver_callbacks clbs;
	memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
	clbs.cmd_async = cmd_async;
	clbs.stop_cond = stop_cond;

	TLOG("creating server");

	ret = wave_ipcs_create(&handle, "server_t2");
	if (ret == WAVE_IPC_ERROR)
		return 1;

	TLOG("running server");

	__die_after_n_events = 0;
	__die_after_n_commands = die_after_n_commands;
	__stop_cond = 0;

	ret = wave_ipcs_run(handle, &clbs);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	TLOG("stopping server");

	ret = wave_ipcs_delete(&handle);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	return 0;
}

static void* send_events(void *data)
{
	wv_ipserver *handle = (wv_ipserver*)data;
	wv_ipc_ret ret;
	int i;

	for (i = 0; i < __die_after_n_events; i++) {
		wv_ipc_msg *event = wave_ipc_msg_alloc();
		if (event == NULL) {
			__stop_cond = 1;
			return (void*)-1;
		}

		wave_ipc_msg_fill_data(event, event1, sizeof(event1));
		ret = wave_ipcs_send_event_all(handle, event);
		wave_ipc_msg_put(event);
		if (ret != WAVE_IPC_SUCCESS) {
			TLOG("wave_ipcs_send_event_all returned error\n");
			__stop_cond = 1;
			return (void*)-1;
		}
		usleep(500);
	}

	TLOG("sent %d events", __die_after_n_events);

	__die_after_n_events = 0;

	return NULL;
}

static pthread_t thread;

static int adding_client(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	(void)ipsta;
	TLOG("station connected -> starting events thread");
	pthread_create(&thread, NULL, send_events, ipserv);
	return 0;
}

static int run_events_server(int die_after_n_commands, int die_after_n_events)
{
	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;

	wv_ipserver_callbacks clbs;
	memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
	clbs.cmd_async = cmd_async;
	clbs.stop_cond = stop_cond;
	clbs.adding_client = adding_client;

	TLOG("creating server");

	ret = wave_ipcs_create(&handle, "server_t2");
	if (ret == WAVE_IPC_ERROR)
		return 1;

	TLOG("running server");

	__stop_cond = 0;
	__die_after_n_commands = die_after_n_commands;
	__die_after_n_events = die_after_n_events;
	ret = wave_ipcs_run(handle, &clbs);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	TLOG("stopping server");

	pthread_join(thread, NULL);

	ret = wave_ipcs_delete(&handle);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	return 0;
}

static int run_crashing_events_server(int die_after_n_events)
{
	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;
	int i = 0, num_runs = 10;

	wv_ipserver_callbacks clbs;
	memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
	clbs.cmd_async = cmd_async;
	clbs.stop_cond = stop_cond;
	clbs.adding_client = adding_client;

	TLOG("creating server");

	do {
		ret = wave_ipcs_create(&handle, "server_t2");
		if (ret == WAVE_IPC_ERROR)
			return 1;

		TLOG("running server");

		__stop_cond = 0;
		__die_after_n_commands = 0;
		__die_after_n_events = die_after_n_events / 10;
		ret = wave_ipcs_run(handle, &clbs);
		if (ret == WAVE_IPC_ERROR)
			return 1;

		i++;
		if (i < num_runs) {
			TLOG("Oops server crashed!");
		} else {
			TLOG("stopping server");
		}

		pthread_join(thread, NULL);

		ret = wave_ipcs_delete(&handle);
		if (ret == WAVE_IPC_ERROR)
			return 1;
	} while (i < num_runs);

	return 0;
}

static int verify_multi_msg_response(wv_ipc_msg *reply, char **arr, int arr_size)
{
	wv_ipc_msg *next;
	char *data;
	size_t size;
	int i;

	if (!wave_ipc_msg_is_multi_msg(reply)) {
		ELOG("reply is not multimsg");
		return 1;
	}

	next = reply;
	for (i = 0; i < arr_size; i++) {
		char *e_resp = arr[i];
		size_t e_size = strlen(e_resp) + 1;

		data = wave_ipc_msg_get_data(next);
		size = wave_ipc_msg_get_size(next);
		if (data == NULL) {
			ELOG("wave_ipc_msg_get_data return NULL");
			return 1;
		}
		if (size != e_size || strcmp(data, e_resp)) {
			ELOG("expected to get '%s' got '%s' (i=%d)",
				e_resp, data, i);
			return 1;
		}

		next = wave_ipc_multi_msg_get_next(next);
	}

	if (next != NULL) {
		ELOG("next is not NULL after loop");
		return 1;
	}

	return 0;
}

static void* multimsg_resp_cmd_sender(void *data)
{
	wv_ipclient *handle = (wv_ipclient*)data;
	wv_ipc_ret ret;
	int j;

	usleep(1000);
	for (j = 0; j < TEST2_NUM_ITER; j++) {
		wv_ipc_msg *cmd = wave_ipc_msg_alloc();
		wv_ipc_msg *reply;

		if (cmd == NULL)
			goto fail;

		wave_ipc_msg_fill_data(cmd, cmd3, sizeof(cmd3));
		ret = wave_ipcc_send_cmd(handle, cmd, &reply);
		wave_ipc_msg_put(cmd);
		if (ret != WAVE_IPC_SUCCESS) {
			ELOG("wave_ipcc_send_cmd return FAILURE %d", ret);
			goto fail;
		}

		if (verify_multi_msg_response(reply, resp3,
					      sizeof(resp3) / sizeof(resp3[0]))) {
			wave_ipc_msg_put(reply);
			ELOG("verify_multi_msg_response returned err");
			goto fail;
		}

		wave_ipc_msg_put(reply);
	}
	SLOG("finished sending %i multimsg response commands", j);
	return NULL;

fail:
	ELOG("failed at i = %i\n", j);
	return (void*)-1;
}

static void* random_cmd_sender(void *_data)
{
	wv_ipclient *handle = (wv_ipclient*)_data;
	wv_ipc_ret ret;
	int num_times_cmd1 = 0;
	int num_times_cmd2 = 0;
	int num_times_cmd3 = 0;
	int num_times_cmd4 = 0;
	int j;

	usleep(1000);
	for (j = 0; j < 10000; j++) {
		wv_ipc_msg *cmd = wave_ipc_msg_alloc();
		wv_ipc_msg *reply;
		char *data;
		size_t size;
		int r = rand() % 4;
		char *e_resp = NULL;
		size_t e_size = 0;

		if (cmd == NULL)
			goto fail;

		if (r == 0) {
			wave_ipc_msg_fill_data(cmd, cmd1, sizeof(cmd1));
			e_size = sizeof(resp1);
			e_resp = resp1;
			num_times_cmd1++;
		} else if (r == 1) {
			wave_ipc_msg_fill_data(cmd, cmd2, sizeof(cmd2));
			e_size = sizeof(resp2);
			e_resp = resp2;
			num_times_cmd2++;
		} else if (r == 2) {
			wave_ipc_msg_fill_data(cmd, cmd3, sizeof(cmd3));
			num_times_cmd3++;
		} else if (r == 3) {
			wave_ipc_msg_fill_data(cmd, cmd4, sizeof(cmd4));
			num_times_cmd4++;
		}

		ret = wave_ipcc_send_cmd(handle, cmd, &reply);
		wave_ipc_msg_put(cmd);
		if (ret != WAVE_IPC_SUCCESS) {
			ELOG("wave_ipcc_send_cmd return FAILURE %d, r=%d", ret, r);
			goto fail;
		}

		if (r == 0 || r == 1) {
			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (data == NULL) {
				ELOG("wave_ipc_msg_get_data return NULL r=%d", r);
				goto fail;
			}

			if (size != e_size || strcmp(data, e_resp)) {
				ELOG("expected to get '%s' got '%s', r=%d", e_resp, data, r);
				goto fail;
			}
		} else if ((r == 2 && verify_multi_msg_response(reply, resp3, 5)) ||
			   (r == 3 && verify_multi_msg_response(reply, resp4, 7))) {
			ELOG("mulit msg fail r=%d", r);
			goto fail;
		}
		wave_ipc_msg_put(reply);
	}
	SLOG("finished sending %i commands, %d cmd1 %d cmd2 %d cmd3 %d cmd4",
	     j, num_times_cmd1, num_times_cmd2, num_times_cmd3, num_times_cmd4);

	return NULL;

fail:
	ELOG("failed at i = %i\n", j);
	return (void*)-1;
}

UNIT_TEST_DEFINE(1, create and destroy N times with listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i, j, use_event_thread = 0;

	for (i = 0; i < 10; i++) {
		ret = wave_ipcc_connect(&handle, "unitest1", "unitest_server0");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error i=%d", i);

		for (j = 0; j < 10; j++) {
			ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, use_event_thread);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_start_listener returned error i=%d j=%d", i, j);

			ret = wave_ipcc_stop_listener(handle);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error i=%d j=%d", i, j);

			use_event_thread++;
			use_event_thread %= 2;
		}

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error i=%d", i);
	}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, connect and N * say hello no listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	wv_ipc_msg *cmd = NULL, *reply = NULL;
	char *data;
	size_t size;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(TEST1_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(100000);

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		for (i = 0; i < TEST1_NUM_ITER; i++) {
			cmd = wave_ipc_msg_alloc();
			if (cmd == NULL)
				UNIT_TEST_FAILED("wave_ipc_msg_alloc returned NULL i=%d", i);

			wave_ipc_msg_fill_data(cmd, "DID YOU HEAR ABOUT UPDOG?", sizeof("DID YOU HEAR ABOUT UPDOG?"));

			ret = wave_ipcc_send_cmd(handle, cmd, &reply);
			wave_ipc_msg_put(cmd);
			cmd = NULL;
			if (ret != WAVE_IPC_SUCCESS)
				UNIT_TEST_FAILED("wave_ipcc_send_cmd returned FAILURE i=%d", i);
			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (size != sizeof(resp1) || !data || strncmp(data, resp1, sizeof(resp1)))
				UNIT_TEST_FAILED("resp1 strncmp FAILURE i=%d", i);
			wave_ipc_msg_put(reply);
			reply = NULL;
		}

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_disconnect(&handle);
	}
	if (cmd) wave_ipc_msg_put(cmd);
	if (reply) wave_ipc_msg_put(reply);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(3, N * connect and say hello no listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	wv_ipc_msg *cmd = NULL, *reply = NULL;
	char *data;
	size_t size;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(10));
	UNIT_TEST_FORKED_PARENET

		usleep(100000);

		for (i = 0; i < 10; i++) {
			ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_connect returned error i=%d", i);

			cmd = wave_ipc_msg_alloc();
			if (cmd == NULL)
				UNIT_TEST_FAILED("wave_ipc_msg_alloc returned NULL i=%d", i);

			wave_ipc_msg_fill_data(cmd, "DID YOU HEAR ABOUT UPDOG?", sizeof("DID YOU HEAR ABOUT UPDOG?"));

			ret = wave_ipcc_send_cmd(handle, cmd, &reply);
			wave_ipc_msg_put(cmd);
			cmd = NULL;
			if (ret != WAVE_IPC_SUCCESS)
				UNIT_TEST_FAILED("wave_ipcc_send_cmd returned FAILURE i=%d", i);
			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (size != sizeof(resp1) || !data || strncmp(data, resp1, sizeof(resp1)))
				UNIT_TEST_FAILED("resp1 strncmp FAILURE i=%d", i);
			wave_ipc_msg_put(reply);
			reply = NULL;

			ret = wave_ipcc_disconnect(&handle);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_disconnect returned error i=%d", i);
		}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_disconnect(&handle);
	}
	if (cmd) wave_ipc_msg_put(cmd);
	if (reply) wave_ipc_msg_put(reply);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(4, connect and N * say hello with listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	wv_ipc_msg *cmd = NULL, *reply = NULL;
	char *data;
	size_t size;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(100000);

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		UNIT_TEST_TIMER_START

		for (i = 0; i < TEST2_NUM_ITER; i++) {
			cmd = wave_ipc_msg_alloc();
			if (cmd == NULL)
				UNIT_TEST_FAILED("wave_ipc_msg_alloc returned NULL i=%d", i);

			wave_ipc_msg_fill_data(cmd, "DID YOU HEAR ABOUT UPDOG?", sizeof("DID YOU HEAR ABOUT UPDOG?"));

			ret = wave_ipcc_send_cmd(handle, cmd, &reply);
			wave_ipc_msg_put(cmd);
			cmd = NULL;
			if (ret != WAVE_IPC_SUCCESS)
				UNIT_TEST_FAILED("wave_ipcc_send_cmd returned FAILURE i=%d", i);

			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (size != sizeof(resp1) || !data || strncmp(data, resp1, sizeof(resp1)))
				UNIT_TEST_FAILED("resp1 strncmp FAILURE i=%d", i);
			wave_ipc_msg_put(reply);
			reply = NULL;
		}

		UNIT_TEST_TIMER_STOP(sending commands with ipc)

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
	if (cmd) wave_ipc_msg_put(cmd);
	if (reply) wave_ipc_msg_put(reply);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(5, N * connect and say hello with listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	wv_ipc_msg *cmd = NULL, *reply = NULL;
	char *data;
	size_t size;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(10));
	UNIT_TEST_FORKED_PARENET

		usleep(100000);

		for (i = 0; i < 10; i++) {
			ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_connect returned error i=%d", i);

			ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

			cmd = wave_ipc_msg_alloc();
			if (cmd == NULL)
				UNIT_TEST_FAILED("wave_ipc_msg_alloc returned NULL i=%d", i);

			wave_ipc_msg_fill_data(cmd, "DID YOU HEAR ABOUT UPDOG?", sizeof("DID YOU HEAR ABOUT UPDOG?"));

			ret = wave_ipcc_send_cmd(handle, cmd, &reply);
			wave_ipc_msg_put(cmd);
			cmd = NULL;
			if (ret != WAVE_IPC_SUCCESS)
				UNIT_TEST_FAILED("wave_ipcc_send_cmd returned FAILURE i=%d", i);

			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (size != sizeof(resp1) || !data || strncmp(data, resp1, sizeof(resp1)))
				UNIT_TEST_FAILED("resp1 strncmp FAILURE i=%d", i);
			wave_ipc_msg_put(reply);
			reply = NULL;

			ret = wave_ipcc_stop_listener(handle);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

			ret = wave_ipcc_disconnect(&handle);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_disconnect returned error i=%d", i);
		}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
	if (cmd) wave_ipc_msg_put(cmd);
	if (reply) wave_ipc_msg_put(reply);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(6, connect and get N events no listerner)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(0, TEST1_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		__wait_for_x_events = TEST1_NUM_ITER;

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_blocked_event_listener(handle, null_event_handler, NULL, NULL, NULL, blocked_events_term, handle);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", events_received_cnt);

		SLOG("received %zu Events", events_received_cnt);

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle)
		wave_ipcc_disconnect(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(7, connect and get N events with listerner)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(0, TEST1_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		for (i = 0; i < 1500; i++) {
			if (events_received_cnt >= TEST1_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST1_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST1_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(8, send N multimsg response commands with listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t3", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		if (multimsg_resp_cmd_sender(handle) != NULL)
			UNIT_TEST_FAILED("multimsg_resp_cmd_sender ret err");

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(9, send N multimsg response commands no listener)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t3", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		if (multimsg_resp_cmd_sender(handle) != NULL)
			UNIT_TEST_FAILED("multimsg_resp_cmd_sender ret err");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(10, send N random commands with listener)

	time_t t;
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		srand((unsigned) time(&t));
		random_cmd_sender(handle);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(11, send N random commands with listener while getting N events)

	time_t t;
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(TEST2_NUM_ITER, TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		events_received_cnt = 0;
		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		srand((unsigned) time(&t));
		random_cmd_sender(handle);

		for (i = 0; i < 1500; i++) {
			if (events_received_cnt >= TEST2_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST2_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST2_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(12, from K Threads: send N random commands)

	pthread_t thread1, thread2, thread3, thread4;
	time_t t;
	void *result1, *result2, *result3, *result4;
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(5 * TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		srand((unsigned) time(&t));

		pthread_create(&thread1, NULL, random_cmd_sender, handle);
		pthread_create(&thread2, NULL, random_cmd_sender, handle);
		pthread_create(&thread3, NULL, random_cmd_sender, handle);
		pthread_create(&thread4, NULL, random_cmd_sender, handle);
		random_cmd_sender(handle);

		pthread_join(thread1, &result1);
		pthread_join(thread2, &result2);
		pthread_join(thread3, &result3);
		pthread_join(thread4, &result4);

		if (result1 != NULL)
			UNIT_TEST_FAILED("thread1 returned error");

		if (result2 != NULL)
			UNIT_TEST_FAILED("thread2 returned error");

		if (result3 != NULL)
			UNIT_TEST_FAILED("thread3 returned error");

		if (result4 != NULL)
			UNIT_TEST_FAILED("thread4 returned error");

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(13, from K Threads: send N random commands while getting N events)

	pthread_t thread1, thread2, thread3, thread4;
	time_t t;
	void *result1, *result2, *result3, *result4;
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(5 * TEST2_NUM_ITER, 2 * TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		events_received_cnt = 0;
		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		srand((unsigned) time(&t));

		pthread_create(&thread1, NULL, random_cmd_sender, handle);
		pthread_create(&thread2, NULL, random_cmd_sender, handle);
		pthread_create(&thread3, NULL, random_cmd_sender, handle);
		pthread_create(&thread4, NULL, random_cmd_sender, handle);
		random_cmd_sender(handle);

		for (i = 0; i < 1500; i++) {
			if (events_received_cnt >= 2 * TEST2_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				break;

			usleep(10000);
		}

		pthread_join(thread1, &result1);
		pthread_join(thread2, &result2);
		pthread_join(thread3, &result3);
		pthread_join(thread4, &result4);

		if (result1 != NULL)
			UNIT_TEST_FAILED("thread1 returned error");

		if (result2 != NULL)
			UNIT_TEST_FAILED("thread2 returned error");

		if (result3 != NULL)
			UNIT_TEST_FAILED("thread3 returned error");

		if (result4 != NULL)
			UNIT_TEST_FAILED("thread4 returned error");

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < 2 * TEST2_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", 2 * TEST2_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(14, connect K clients same pid & send N random commands)

	time_t t;
	wv_ipc_ret ret;
	wv_ipclient *handle[3] = { NULL };
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(3 * TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		for (i = 0; i < 3; i++) {
			char name[48] = "no_X_client";
			name[3] = (char)i;
			ret = wave_ipcc_connect(&handle[i], name, "server_t2");
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_connect returned error i=%d", i);

			ret = wave_ipcc_start_listener(handle[i], null_event_handler, NULL, NULL, NULL, NULL, 0);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_start_listener returned error i=%d", i);
		}

		srand((unsigned) time(&t));
		random_cmd_sender(handle[0]);
		random_cmd_sender(handle[1]);
		random_cmd_sender(handle[2]);

		for (i = 0; i < 3; i++) {
			ret = wave_ipcc_stop_listener(handle[i]);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error i=%d", i);

			ret = wave_ipcc_disconnect(&handle[i]);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_disconnect returned error i=%d", i);
		}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle[0]) {
		wave_ipcc_stop_listener(handle[0]);
		wave_ipcc_disconnect(&handle[0]);
	}
	if (handle[1]) {
		wave_ipcc_stop_listener(handle[1]);
		wave_ipcc_disconnect(&handle[1]);
	}
	if (handle[2]) {
		wave_ipcc_stop_listener(handle[2]);
		wave_ipcc_disconnect(&handle[2]);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(15, connect K clients same pid & from K Threads: send N random commands)

	time_t t;
	wv_ipc_ret ret;
	wv_ipclient *handle[3] = { NULL };
	pthread_t thread1 = 0, thread2 = 0;
	void *result;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_basic_server(3 * TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		for (i = 0; i < 3; i++) {
			char name[48] = "no_X_client";
			name[3] = (char)i;
			ret = wave_ipcc_connect(&handle[i], name, "server_t2");
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_connect returned error i=%d", i);

			ret = wave_ipcc_start_listener(handle[i], null_event_handler, NULL, NULL, NULL, NULL, 0);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_start_listener returned error i=%d", i);
		}

		srand((unsigned) time(&t));
		pthread_create(&thread1, NULL, random_cmd_sender, handle[0]);
		pthread_create(&thread2, NULL, random_cmd_sender, handle[1]);
		random_cmd_sender(handle[2]);

		pthread_join(thread1, &result);
		if (result != NULL)
			UNIT_TEST_FAILED("thread1 returned error");
		thread1 = 0;

		pthread_join(thread2, &result);
		if (result != NULL)
			UNIT_TEST_FAILED("thread2 returned error");
		thread2 = 0;

		for (i = 0; i < 3; i++) {
			ret = wave_ipcc_stop_listener(handle[i]);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error i=%d", i);

			ret = wave_ipcc_disconnect(&handle[i]);
			if (ret == WAVE_IPC_ERROR)
				UNIT_TEST_FAILED("wave_ipcc_disconnect returned error i=%d", i);
		}

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle[0]) {
		wave_ipcc_stop_listener(handle[0]);
		wave_ipcc_disconnect(&handle[0]);
	}
	if (handle[1]) {
		wave_ipcc_stop_listener(handle[1]);
		wave_ipcc_disconnect(&handle[1]);
	}
	if (handle[2]) {
		wave_ipcc_stop_listener(handle[2]);
		wave_ipcc_disconnect(&handle[2]);
	}
	if (thread1) pthread_join(thread1, &result);
	if (thread2) pthread_join(thread2, &result);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(16, crashing server: receive N events with listener)
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_crashing_events_server(TEST1_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		ret = wave_ipcc_start_listener(handle, null_event_handler, NULL, NULL, NULL, NULL, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		for (i = 0; i < 6000; i++) {
			if (events_received_cnt >= TEST1_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST1_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST1_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(17, crashing server: receive N events no listener)
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_crashing_events_server(TEST1_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		__wait_for_x_events = TEST1_NUM_ITER;

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_blocked_event_listener(handle, null_event_handler, NULL, NULL, NULL, blocked_events_term, handle);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", events_received_cnt);

		if (events_received_cnt < TEST1_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST1_NUM_ITER, events_received_cnt);

		SLOG("received %zu Events", events_received_cnt);

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) wave_ipcc_disconnect(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(18, send command from event callback feature with lisntener)
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(TEST2_NUM_ITER, TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		ret = wave_ipcc_start_listener(handle, cmd_sending_event_handler_aka_annoying, NULL, NULL, NULL, handle, 0);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		for (i = 0; i < 6000; i++) {
			if (events_received_cnt >= TEST2_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST2_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST2_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(19, send command from event callback feature no listener)
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(TEST2_NUM_ITER, TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		__wait_for_x_events = TEST2_NUM_ITER;

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		ret = wave_ipcc_blocked_event_listener(handle, cmd_sending_event_handler_aka_annoying,
						       NULL, NULL, NULL, blocked_events_term, handle);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", events_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST2_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST2_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) wave_ipcc_disconnect(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(20, send command from event callback feature with lisntener with events thread)
	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_events_server(TEST2_NUM_ITER, TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		events_received_cnt = 0;
		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		__err_detected = 0;
		ret = wave_ipcc_start_listener(handle, cmd_sending_event_handler_aka_annoying, NULL, NULL, NULL, handle, 1);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_start_listener returned error");

		for (i = 0; i < 6000; i++) {
			if (events_received_cnt >= TEST2_NUM_ITER) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		if (events_received_cnt < TEST2_NUM_ITER)
			UNIT_TEST_FAILED("received less than %d events, (%zu)\n", TEST2_NUM_ITER, events_received_cnt);

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

static pthread_t thread1, thread2;
static wv_ipserver *cmd_server = NULL;
static wv_ipstation *cmd_reply_sta = NULL;
static int __die_after_n_commands_1 = 0;
static int __die_after_n_commands_2 = 0;

static int cmd_srv_stop_cond(wv_ipserver *ipserv)
{
	(void)ipserv;
	return (__stop_cond) || (__die_after_n_commands_1 == 0 && __die_after_n_commands_2 == 0 &&
				 __die_after_n_events == 0 && __die_after_n_commands == 0);
}

static void* serv_send_commands(void *arg)
{
	int *__die_after_n_cmd = (int*)arg;
	wv_ipc_ret ret;
	int i;
	char *data;
	size_t size;
	char *e_resp;
	size_t e_size;

	for (i = 0; i < *__die_after_n_cmd; i++) {
		wv_ipc_msg *cmd = wave_ipc_msg_alloc();
		wv_ipc_msg *reply;
		int r = rand() % 2;

		if (cmd == NULL)
			goto err;

		if (r == 0) {
			wave_ipc_msg_fill_data(cmd, cmd1, sizeof(cmd1));
			e_size = sizeof(resp1);
			e_resp = resp1;
		} else
			wave_ipc_msg_fill_data(cmd, cmd3, sizeof(cmd3));

		ret = wave_ipcs_send_cmd_to(cmd_server, cmd_reply_sta, cmd, &reply);
		wave_ipc_msg_put(cmd);
		if (ret != WAVE_IPC_SUCCESS) {
			ELOG("wave_ipcs_send_cmd_to return FAILURE ret=%d, r=%d i=%d", ret, r, i);
			goto err;
		}

		if (r == 0) {
			data = wave_ipc_msg_get_data(reply);
			size = wave_ipc_msg_get_size(reply);
			if (data == NULL) {
				ELOG("wave_ipc_msg_get_data return NULL r=%d i=%d", r, i);
				wave_ipc_msg_put(reply);
				goto err;
			}

			if (size != e_size || strcmp(data, e_resp)) {
				ELOG("expected to get '%s' got '%s', r=%d i=%d", e_resp, data, r, i);
				wave_ipc_msg_put(reply);
				goto err;
			}
		} else if (r == 1 && verify_multi_msg_response(reply, resp3, 5)) {
			ELOG("mulit msg fail r=%d i=%d", r, i);
			wave_ipc_msg_put(reply);
			goto err;
		}
		wave_ipc_msg_put(reply);
	}

	TLOG("sent %d commands", *__die_after_n_cmd);

	*__die_after_n_cmd = 0;
	return NULL;
err:
	__stop_cond = 1;
	return NULL;
}

static int adding_client_for_commands(wv_ipserver *ipserv, wv_ipstation *ipsta)
{
	cmd_reply_sta = ipsta;
	TLOG("station connected -> starting command threads");
	pthread_create(&thread1, NULL, serv_send_commands, (void*)&__die_after_n_commands_1);
	pthread_create(&thread2, NULL, serv_send_commands, (void*)&__die_after_n_commands_2);
	if (__die_after_n_events)
		pthread_create(&thread, NULL, send_events, ipserv);
	else
		thread = 0;
	return 0;
}

static int run_command_sending_server(int die_after_n_commands_to_client,
				      int die_after_n_events, int die_after_n_commands)
{
	wv_ipc_ret ret;
	wv_ipserver *handle = NULL;

	wv_ipserver_callbacks clbs;
	memset(&clbs, 0, sizeof(wv_ipserver_callbacks));
	clbs.cmd_async = cmd_async;
	clbs.stop_cond = cmd_srv_stop_cond;
	clbs.adding_client = adding_client_for_commands;

	TLOG("creating server");

	ret = wave_ipcs_create(&handle, "server_t2");
	if (ret == WAVE_IPC_ERROR)
		return 1;

	cmd_server = handle;
	TLOG("running server");

	__stop_cond = 0;
	__die_after_n_commands_1 = die_after_n_commands_to_client;
	__die_after_n_commands_2 = die_after_n_commands_to_client;
	__die_after_n_events = die_after_n_events;
	__die_after_n_commands = die_after_n_commands;
	ret = wave_ipcs_run(handle, &clbs);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	TLOG("stopping server");

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	if (thread)
		pthread_join(thread, NULL);

	ret = wave_ipcs_delete(&handle);
	if (ret == WAVE_IPC_ERROR)
		return 1;

	if (__stop_cond)
		return 1;

	return 0;
}

size_t __wait_for_x_commands;

static int blocked_commands_term(void *arg)
{
	(void)arg;

	if ((__wait_for_x_commands == commands_received_cnt &&
	     __wait_for_x_events == events_received_cnt) || __err_detected)
		return 1;

	return 0;
}

static int client_cmd(void *arg, uint8_t seq_num, wv_ipc_msg *cmd)
{
	char *data;
	size_t size;
	wv_ipclient *handle = (wv_ipclient*)arg;

	data = wave_ipc_msg_get_data(cmd);
	size = wave_ipc_msg_get_size(cmd);
	if (size == sizeof(cmd1) && data && !strncmp(data, cmd1, sizeof(cmd1))) {
		wv_ipc_msg *reply = wave_ipc_msg_alloc();
		if (reply == NULL)
			goto err;
		wave_ipc_msg_fill_data(reply, resp1, sizeof(resp1));
		wave_ipcc_send_response(handle, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else if (size == sizeof(cmd2) && data && !strncmp(data, cmd2, sizeof(cmd2))) {
		wv_ipc_msg *reply = wave_ipc_msg_alloc();
		if (reply == NULL)
			goto err;
		wave_ipc_msg_fill_data(reply, resp2, sizeof(resp2));
		wave_ipcc_send_response(handle, seq_num, reply, 0);
		wave_ipc_msg_put(reply);
	} else if (size == sizeof(cmd3) && data && !strncmp(data, cmd3, sizeof(cmd3))) {
		size_t i;

		for (i = 0; i < sizeof(resp3) / sizeof(resp3[0]); i++) {
			wv_ipc_msg *reply = wave_ipc_msg_alloc();
			if (reply == NULL)
				goto err;
			wave_ipc_msg_fill_data(reply, resp3[i], strlen(resp3[i]) + 1);
			wave_ipcc_send_response(handle, seq_num, reply,
						   (i + 1 != sizeof(resp3) / sizeof(resp3[0])));
			wave_ipc_msg_put(reply);
		}
	} else if (size == sizeof(cmd4) && data && !strncmp(data, cmd4, sizeof(cmd4))) {
		size_t i;

		for (i = 0; i < sizeof(resp4) / sizeof(resp4[0]); i++) {
			wv_ipc_msg *reply = wave_ipc_msg_alloc();
			if (reply == NULL)
				goto err;
			wave_ipc_msg_fill_data(reply, resp4[i], strlen(resp4[i]) + 1);
			wave_ipcc_send_response(handle, seq_num, reply,
						   (i + 1 != sizeof(resp4) / sizeof(resp4[0])));
			wave_ipc_msg_put(reply);
		}
	} else {
		goto err;
	}

	commands_received_cnt++;

	return 0;

err:
	__stop_cond = 1;
	__err_detected = 1;
	return 1;
}

UNIT_TEST_DEFINE(21, send commands from server to blocking client)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_command_sending_server(TEST2_NUM_ITER, 0, 0));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		__wait_for_x_commands = TEST2_NUM_ITER * 2;
		commands_received_cnt = 0;
		events_received_cnt = 0;
		__wait_for_x_events = 0;
		__err_detected = 0;

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_blocked_event_listener(handle, null_event_handler, client_cmd, NULL, NULL, blocked_commands_term, handle);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", commands_received_cnt);

		SLOG("received %zu Commands", commands_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle)
		wave_ipcc_disconnect(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(22, send commands and events from server to blocking client)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_command_sending_server(TEST2_NUM_ITER, TEST2_NUM_ITER, 0));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		__wait_for_x_commands = TEST2_NUM_ITER * 2;
		commands_received_cnt = 0;
		events_received_cnt = 0;
		__wait_for_x_events = TEST2_NUM_ITER;
		__err_detected = 0;

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_blocked_event_listener(handle, null_event_handler, client_cmd, NULL, NULL, blocked_commands_term, handle);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", commands_received_cnt);

		SLOG("received %zu Commands & %zu events", commands_received_cnt, events_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle)
		wave_ipcc_disconnect(&handle);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(23, send commands from server to listener client)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_command_sending_server(TEST2_NUM_ITER, 0, 0));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		__wait_for_x_commands = TEST2_NUM_ITER * 2;
		commands_received_cnt = 0;
		events_received_cnt = 0;
		__wait_for_x_events = 0;
		__err_detected = 0;

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, client_cmd, NULL, NULL, handle, 0);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", commands_received_cnt);

		for (i = 0; i < 1500; i++) {
			if (commands_received_cnt >= __wait_for_x_commands) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		SLOG("received %zu Commands & %zu events", commands_received_cnt, events_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(24, send commands and events from server to listener client)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_command_sending_server(TEST2_NUM_ITER, TEST2_NUM_ITER, 0));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		__wait_for_x_commands = TEST2_NUM_ITER * 2;
		commands_received_cnt = 0;
		events_received_cnt = 0;
		__wait_for_x_events = TEST2_NUM_ITER;
		__err_detected = 0;

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, client_cmd, NULL, NULL, handle, 0);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", commands_received_cnt);

		for (i = 0; i < 1500; i++) {
			if (commands_received_cnt >= __wait_for_x_commands &&
			    events_received_cnt >= __wait_for_x_events) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		SLOG("received %zu Commands & %zu events", commands_received_cnt, events_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(25, send commands and events from server to cmd sending listener client)

	wv_ipc_ret ret;
	wv_ipclient *handle = NULL;
	int i;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_command_sending_server(TEST2_NUM_ITER, TEST2_NUM_ITER, TEST2_NUM_ITER));
	UNIT_TEST_FORKED_PARENET

		usleep(10000);

		__wait_for_x_commands = TEST2_NUM_ITER * 2;
		commands_received_cnt = 0;
		events_received_cnt = 0;
		__wait_for_x_events = TEST2_NUM_ITER;
		__err_detected = 0;

		ret = wave_ipcc_connect(&handle, "clinet_t2", "server_t2");
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_connect returned error");

		ret = wave_ipcc_start_listener(handle, null_event_handler, client_cmd, NULL, NULL, handle, 0);
		if (ret != WAVE_IPC_SUCCESS)
			UNIT_TEST_FAILED("wave_ipcc_blocked_event_listener returned error i=%zu", commands_received_cnt);

		if (random_cmd_sender(handle))
			UNIT_TEST_FAILED("random_cmd_sender failed");

		for (i = 0; i < 1500; i++) {
			if (commands_received_cnt >= __wait_for_x_commands &&
			    events_received_cnt >= __wait_for_x_events) {
				SLOG("exit early");
				break;
			}

			if (__err_detected)
				UNIT_TEST_FAILED("__err_detected");

			usleep(10000);
		}

		SLOG("received %zu Commands & %zu events", commands_received_cnt, events_received_cnt);

		if (__err_detected)
			UNIT_TEST_FAILED("__err_detected");

		ret = wave_ipcc_stop_listener(handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_stop_listener returned error");

		ret = wave_ipcc_disconnect(&handle);
		if (ret == WAVE_IPC_ERROR)
			UNIT_TEST_FAILED("wave_ipcc_disconnect returned error");

UNIT_TEST_CLEANUP_ON_ERRR
	if (handle) {
		wave_ipcc_stop_listener(handle);
		wave_ipcc_disconnect(&handle);
	}
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(ipc_client)
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
	ADD_TEST(5)
	ADD_TEST(6)
	ADD_TEST(7)
	ADD_TEST(8)
	ADD_TEST(9)
	ADD_TEST(10)
	ADD_TEST(11)
	ADD_TEST(12)
	ADD_TEST(13)
	ADD_TEST(14)
	ADD_TEST(15)
	ADD_TEST(16)
	ADD_TEST(17)
	ADD_TEST(18)
	ADD_TEST(19)
	ADD_TEST(20)
	ADD_TEST(21)
	ADD_TEST(22)
	ADD_TEST(23)
	ADD_TEST(24)
	ADD_TEST(25)
UNIT_TEST_MODULE_DEFINITION_DONE
