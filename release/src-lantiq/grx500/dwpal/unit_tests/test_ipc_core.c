/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "wave_ipc_core.h"
#include "unitest_helper.h"

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>

static char msg1[] = "Generate extended debug information for "
		"inlined functions. Location view tracking markers are "
		"inserted at inlined entry points, so that address and view "
		"numbers can be computed and output in debug information. "
		"This can be enabled independently of location views, in which "
		"case the view numbers won’t be output, but it can only be "
		"enabled along with statement frontiers, and it is only enabled "
		"by default if location views are enabled."
		"Produce compressed debug sections in DWARF format, if that "
		"is supported. If type is not given, the default type depends on the "
		"capabilities of the assembler and linker used. type may be one of ‘none’ "
		"(don’t compress debug sections), ‘zlib’ (use zlib compression in ELF "
		"gABI format), or ‘zlib-gnu’ (use zlib compression in traditional "
		"GNU format). If the linker doesn’t support writing "
		"compressed debug sections, the option is rejected. Otherwise, "
		"if the assembler does not support them, -gz is silently ignored "
		"when producing object files.";
static char msg2[] = "Produce compressed debug sections in DWARF format, if that "
		"is supported. If type is not given, the default type depends on the "
		"capabilities of the assembler and linker used. type may be one of ‘none’ "
		"(don’t compress debug sections), ‘zlib’ (use zlib compression in ELF "
		"gABI format), or ‘zlib-gnu’ (use zlib compression in traditional "
		"GNU format). If the linker doesn’t support writing "
		"compressed debug sections, the option is rejected. Otherwise, "
		"if the assembler does not support them, -gz is silently ignored "
		"when producing object files."
		"Generate extended debug information for "
		"inlined functions. Location view tracking markers are "
		"inserted at inlined entry points, so that address and view "
		"numbers can be computed and output in debug information. "
		"This can be enabled independently of location views, in which "
		"case the view numbers won’t be output, but it can only be "
		"enabled along with statement frontiers, and it is only enabled "
		"by default if location views are enabled."
		"Generate extended debug information for "
		"inlined functions. Location view tracking markers are "
		"inserted at inlined entry points, so that address and view "
		"numbers can be computed and output in debug information. "
		"This can be enabled independently of location views, in which "
		"case the view numbers won’t be output, but it can only be "
		"enabled along with statement frontiers, and it is only enabled "
		"by default if location views are enabled."
		"Produce compressed debug sections in DWARF format, if that "
		"is supported. If type is not given, the default type depends on the "
		"capabilities of the assembler and linker used. type may be one of ‘none’ "
		"(don’t compress debug sections), ‘zlib’ (use zlib compression in ELF "
		"gABI format), or ‘zlib-gnu’ (use zlib compression in traditional "
		"GNU format). If the linker doesn’t support writing "
		"compressed debug sections, the option is rejected. Otherwise, "
		"if the assembler does not support them, -gz is silently ignored "
		"when producing object files.";


UNIT_TEST_DEFINE(1, alloc and put messages)

	wv_ipc_msg *a[10000] = { 0 };
	size_t i;

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		a[i] = wave_ipc_msg_alloc();
		if (!a[i])
			UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL, i=%zu", i);
	}

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++)
		wave_ipc_msg_put(a[i]);

UNIT_TEST_CLEANUP_ON_ERRR
	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++)
		if (a[i])
			wave_ipc_msg_put(a[i]);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, fill and get data)

	wv_ipc_msg *a, *b;
	char *m1, *m2;
	size_t s1, s2;

	a = wave_ipc_msg_alloc();
	b = wave_ipc_msg_alloc();
	if (!a || !b)
		UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL");

	if (WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(a, msg1, sizeof(msg1)) ||
		WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(b, msg2, sizeof(msg2)))
		UNIT_TEST_FAILED("wave_ipc_msg_fill_data retuned Failure");

	m1 = wave_ipc_msg_get_data(a);
	s1 = wave_ipc_msg_get_size(a);
	m2 = wave_ipc_msg_get_data(b);
	s2 = wave_ipc_msg_get_size(b);
	if (NULL == m1 || NULL == m2)
		UNIT_TEST_FAILED("wave_ipc_msg_get_data retuned NULL");

	if (s1 != sizeof(msg1) || s2 != sizeof(msg2))
		UNIT_TEST_FAILED("returned sizes incorrect. s1=%zu s2=%zu", s1, s2);

	if (strncmp(m1, msg1, s1) || strncmp(m2, msg2, s2))
		UNIT_TEST_FAILED("strncmp found mismatches");

	wave_ipc_msg_put(a);
	wave_ipc_msg_put(b);

UNIT_TEST_CLEANUP_ON_ERRR
	if (a) wave_ipc_msg_put(a);
	if (b) wave_ipc_msg_put(b);
UNIT_TEST_DEFINITION_DONE

#define push_hdr(msg, hdr) { \
	if (WAVE_IPC_SUCCESS != wave_ipc_msg_push_hdr(msg, hdr, sizeof(hdr))) \
		UNIT_TEST_FAILED("wave_ipc_msg_push_hdr retuned Failure"); \
}

#define pop_hdr(msg, orig_header) { \
	uint8_t _hdr[5]; \
	uint8_t _size = sizeof(_hdr); \
	if (WAVE_IPC_SUCCESS != wave_ipc_msg_pop_hdr(msg, _hdr, &_size)) \
		UNIT_TEST_FAILED("wave_ipc_msg_pop_hdr retuned Failure"); \
	if (_size != sizeof(orig_header) || memcmp(orig_header, _hdr, _size)) \
		UNIT_TEST_FAILED("size/memcmp failed size=%d, hdr=%s", _size, _hdr); \
}

UNIT_TEST_DEFINE(3, push and pop header)

	wv_ipc_msg *a, *b;
	uint8_t headr1[] = "abc";
	uint8_t headr2[] = "def";
	uint8_t headr3[] = "ghee";
	uint8_t headr4[] = "xx";
	uint8_t headr5[] = "z";
	uint8_t hdr[5];
	uint8_t size;

	a = wave_ipc_msg_alloc();
	b = wave_ipc_msg_alloc();
	if (!a || !b)
		UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL");

	push_hdr(a, headr1)
	push_hdr(b, headr2)
	push_hdr(a, headr3)
	push_hdr(b, headr4)
	push_hdr(b, headr5)

	pop_hdr(a, headr3)
	pop_hdr(b, headr5)
	pop_hdr(b, headr4)
	pop_hdr(a, headr1)
	pop_hdr(b, headr2)

	size = sizeof(hdr);
	if (WAVE_IPC_SUCCESS == wave_ipc_msg_pop_hdr(a, hdr, &size))
		UNIT_TEST_FAILED("wave_ipc_msg_pop_hdr retuned WAVE_IPC_SUCCESS");
	size = sizeof(hdr);
	if (WAVE_IPC_SUCCESS == wave_ipc_msg_pop_hdr(b, hdr, &size))
		UNIT_TEST_FAILED("wave_ipc_msg_pop_hdr retuned WAVE_IPC_SUCCESS");

	wave_ipc_msg_put(a);
	wave_ipc_msg_put(b);

UNIT_TEST_CLEANUP_ON_ERRR
	if (a) wave_ipc_msg_put(a);
	if (b) wave_ipc_msg_put(b);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(4, push and pop header in a row)
	wv_ipc_msg *a;
	uint8_t headr1[] = "abc";
	uint8_t headr3[] = "ghee";
	uint8_t hdr[5];
	uint8_t size;

	a = wave_ipc_msg_alloc();
	if (!a)
		UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL");

	push_hdr(a, headr1)
	push_hdr(a, headr3)
	pop_hdr(a, headr3)
	pop_hdr(a, headr1)

	push_hdr(a, headr1)
	pop_hdr(a, headr1)

	push_hdr(a, headr1)
	pop_hdr(a, headr1)

	push_hdr(a, headr1)
	push_hdr(a, headr3)
	pop_hdr(a, headr3)
	pop_hdr(a, headr1)

	size = sizeof(hdr);
	if (WAVE_IPC_SUCCESS == wave_ipc_msg_pop_hdr(a, hdr, &size))
		UNIT_TEST_FAILED("wave_ipc_msg_pop_hdr retuned WAVE_IPC_SUCCESS");

	wave_ipc_msg_put(a);
UNIT_TEST_CLEANUP_ON_ERRR
	if (a) wave_ipc_msg_put(a);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(5, send and recv over socket)

	wv_ipc_msg *a = NULL, *b = NULL, *c = NULL;
	wv_ipc_ret ret;
	uint8_t headr1[] = "abc";
	uint8_t headr2[] = "ghee";
	int i;
	char *m1;
	size_t s1;

	struct sockaddr_un sockaddr;
	int server = -1;
	int accepted_client = -1;
	int client = -1;

	server = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server == -1)
		UNIT_TEST_FAILED("socket");

	client = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client == -1)
		UNIT_TEST_FAILED("socket");

	sockaddr.sun_family = AF_UNIX;
	snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "/tmp/saros_test");
	unlink(sockaddr.sun_path);

	bind(server, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	listen(server, 6);

	connect(client, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	accepted_client = accept(server, NULL, 0);

	ILOG( "server=%d client=%d accepted=%d", server, client, accepted_client);

	UNIT_TEST_TIMER_START

	for (i = 0; i < 10000; i++) {
		a = wave_ipc_msg_alloc();
		b = wave_ipc_msg_alloc();
		if (!a || !b)
			UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL");

		push_hdr(a, headr1)
		if (WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(a, msg1, sizeof(msg1)))
			UNIT_TEST_FAILED("wave_ipc_msg_fill_data retuned Failure");
		push_hdr(a, headr2)

		push_hdr(b, headr2)
		if (WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(b, msg2, sizeof(msg2)))
			UNIT_TEST_FAILED("wave_ipc_msg_fill_data retuned Failure");
		push_hdr(b, headr1)

		if (WAVE_IPC_SUCCESS != wave_ipc_send_msg(client, a, 0))
			UNIT_TEST_FAILED("wave_ipc_send_msg retuned Failure");
		wave_ipc_msg_put(a);
		a = NULL;

		if (WAVE_IPC_SUCCESS != wave_ipc_send_msg(client, b, 0))
			UNIT_TEST_FAILED("wave_ipc_send_msg retuned Failure");
		wave_ipc_msg_put(b);
		b = NULL;

		if ((ret = wave_ipc_recv_msg(accepted_client , &c)))
			UNIT_TEST_FAILED("wave_ipc_recv_msg retuned err (%d)", ret);

		pop_hdr(c, headr2)
		pop_hdr(c, headr1)

		m1 = wave_ipc_msg_get_data(c);
		s1 = wave_ipc_msg_get_size(c);
		if (NULL == m1)
			UNIT_TEST_FAILED("wave_ipc_msg_get_data retuned NULL");

		if (s1 != sizeof(msg1))
			UNIT_TEST_FAILED("returned sizes incorrect. s1=%zu", s1);

		if (strncmp(m1, msg1, s1))
			UNIT_TEST_FAILED("strncmp found mismatches");

		wave_ipc_msg_put(c);
		c = NULL;

		if ((ret = wave_ipc_recv_msg(accepted_client , &c)))
			UNIT_TEST_FAILED("wave_ipc_recv_msg retuned err (%d)", ret);

		pop_hdr(c, headr1)
		pop_hdr(c, headr2)

		m1 = wave_ipc_msg_get_data(c);
		s1 = wave_ipc_msg_get_size(c);
		if (NULL == m1)
			UNIT_TEST_FAILED("wave_ipc_msg_get_data retuned NULL");

		if (s1 != sizeof(msg2))
			UNIT_TEST_FAILED("returned sizes incorrect. s1=%zu", s1);

		if (strncmp(m1, msg2, s1))
			UNIT_TEST_FAILED("strncmp found mismatches");

		wave_ipc_msg_put(c);
		c = NULL;
	}

	UNIT_TEST_TIMER_STOP(sending commands with sockets in same thread)

	close(server);
	close(accepted_client);
	close(client);
	unlink(sockaddr.sun_path);

UNIT_TEST_CLEANUP_ON_ERRR
	if (a) wave_ipc_msg_put(a);
	if (b) wave_ipc_msg_put(b);
	if (c) wave_ipc_msg_put(c);

	if (server != -1) close(server);
	if (client != -1) close(client);
	if (accepted_client != -1) close(accepted_client);
	unlink(sockaddr.sun_path);
UNIT_TEST_DEFINITION_DONE

static int run_server(int num_cmds)
{
	struct sockaddr_un sockaddr;
	int server = -1;
	int accepted_client = -1;
	wv_ipc_msg *a = NULL, *c = NULL;
	wv_ipc_ret ret;
	char *m1;
	size_t s1;
	int i;

	server = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server == -1) {
		ELOG("socket");
		return 1;
	}

	sockaddr.sun_family = AF_UNIX;
	snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "/tmp/saros_test2");
	unlink(sockaddr.sun_path);

	bind(server, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
	listen(server, 6);

	accepted_client = accept(server, NULL, 0);

	for (i = 0; i < num_cmds; i++) {
		if ((ret = wave_ipc_recv_msg(accepted_client , &c))) {
			ELOG("wave_ipc_recv_msg retuned err (%d) i=%d", ret, i);
			goto err;
		}

		m1 = wave_ipc_msg_get_data(c);
		s1 = wave_ipc_msg_get_size(c);
		if (NULL == m1) {
			ELOG("wave_ipc_msg_get_data retuned NULL i=%d", i);
			goto err;
		}

		if (s1 != sizeof(msg1)) {
			ELOG("returned sizes incorrect. s1=%zu i=%d", s1, i);
			goto err;
		}

		if (strncmp(m1, msg1, s1)) {
			ELOG("strncmp found mismatches i=%d", i);
			goto err;
		}

		wave_ipc_msg_put(c);
		c = NULL;

		a = wave_ipc_msg_alloc();
		if (!a) {
			ELOG("wave_ipc_msg_alloc retuned NULL i=%d", i);
			goto err;
		}

		if (WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(a, msg2, sizeof(msg2))) {
			ELOG("wave_ipc_msg_fill_data retuned Failure i=%d", i);
			goto err;
		}

		if (WAVE_IPC_SUCCESS != wave_ipc_send_msg(accepted_client, a, 0)) {
			ELOG("wave_ipc_send_msg retuned Failure i=%d", i)
			goto err;
		}

		wave_ipc_msg_put(a);
		a = NULL;
	}

	usleep(10000);

	close(server);
	close(accepted_client);
	unlink(sockaddr.sun_path);

	return 0;
err:
	if (a) wave_ipc_msg_put(a);
	if (c) wave_ipc_msg_put(c);

	if (server != -1) close(server);
	if (accepted_client != -1) close(accepted_client);
	unlink(sockaddr.sun_path);

	return 1;
}

UNIT_TEST_DEFINE(6, send and recv over socket - forked)

	wv_ipc_msg *a = NULL, *c = NULL;
	wv_ipc_ret ret;
	char *m1;
	size_t s1;
	int i;
	struct sockaddr_un sockaddr;
	int client = -1;

	UNIT_TEST_FORK
	UNIT_TEST_FORKED_CHILD
		exit(run_server(10000));
	UNIT_TEST_FORKED_PARENET

		usleep(200000);

		client = socket(AF_UNIX, SOCK_STREAM, 0);
		if (client == -1)
			UNIT_TEST_FAILED("socket");

		sockaddr.sun_family = AF_UNIX;
		snprintf(sockaddr.sun_path, sizeof(sockaddr.sun_path), "/tmp/saros_test2");
		connect(client, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

		UNIT_TEST_TIMER_START

		for (i = 0; i < 10000; i++) {
			a = wave_ipc_msg_alloc();
			if (!a)
				UNIT_TEST_FAILED("wave_ipc_msg_alloc retuned NULL i=%d", i);

			if (WAVE_IPC_SUCCESS != wave_ipc_msg_fill_data(a, msg1, sizeof(msg1)))
				UNIT_TEST_FAILED("wave_ipc_msg_fill_data retuned Failure i=%d", i);

			if (WAVE_IPC_SUCCESS != wave_ipc_send_msg(client, a, 0))
				UNIT_TEST_FAILED("wave_ipc_send_msg retuned Failure i=%d", i)

			wave_ipc_msg_put(a);
			a = NULL;

			if ((ret = wave_ipc_recv_msg(client , &c)))
				UNIT_TEST_FAILED("wave_ipc_recv_msg retuned err (%d) i=%d", ret, i);

			m1 = wave_ipc_msg_get_data(c);
			s1 = wave_ipc_msg_get_size(c);
			if (NULL == m1)
				UNIT_TEST_FAILED("wave_ipc_msg_get_data retuned NULL i=%d", i);

			if (s1 != sizeof(msg2))
				UNIT_TEST_FAILED("returned sizes incorrect. s1=%zu i=%d", s1, i);

			if (strncmp(m1, msg2, s1))
				UNIT_TEST_FAILED("strncmp found mismatches i=%d", i);

			wave_ipc_msg_put(c);
			c = NULL;
		}

		UNIT_TEST_TIMER_STOP(sending commands with sockets - forked)

		usleep(10000);
		close(client);

UNIT_TEST_CLEANUP_ON_ERRR
	if (a) wave_ipc_msg_put(a);
	if (c) wave_ipc_msg_put(c);
	if (client != -1) close(client);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(ipc_core)
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
	ADD_TEST(5)
	ADD_TEST(6)
UNIT_TEST_MODULE_DEFINITION_DONE
