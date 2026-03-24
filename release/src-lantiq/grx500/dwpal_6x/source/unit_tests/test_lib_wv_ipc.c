/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "unitest_helper.h"
#include <stdio.h>
#include <string.h>

int unit_test_module_linked_list(char *tests);
int unit_test_module_obj_pool(char *tests);
int unit_test_module_work_serializer(char *tests);
int unit_test_module_ipc_core(char *tests);
int unit_test_module_ipc_client(char *tests);
int unit_test_module_ipc_server(char *tests);

int main(int argc, char *argv[])
{
	int i, total = 0;

	for (i = 1; i < argc; i++) {
		int res = 0;

		ILOG( "starting test: %s", argv[i]);
		if (!strcmp(argv[i], "all")) {
			res += unit_test_module_linked_list(NULL);
			res += unit_test_module_obj_pool(NULL);
			res += unit_test_module_work_serializer(NULL);
			res += unit_test_module_ipc_core(NULL);
			res += unit_test_module_ipc_client(NULL);
			res += unit_test_module_ipc_server(NULL);
		} else if (!strncmp(argv[i], "list", sizeof("list") - 1)) {
			res += unit_test_module_linked_list(argv[i]);
		} else if (!strncmp(argv[i], "obj_pool", sizeof("obj_pool") - 1)) {
			res += unit_test_module_obj_pool(argv[i]);
		} else if (!strncmp(argv[i], "serializer", sizeof("serializer") - 1)) {
			res += unit_test_module_work_serializer(argv[i]);
		} else if (!strncmp(argv[i], "core", sizeof("core") - 1)) {
			res += unit_test_module_ipc_core(argv[i]);
		} else if (!strncmp(argv[i], "client", sizeof("client") - 1)) {
			unit_test_module_ipc_client(argv[i]);
		} else if (!strncmp(argv[i], "server", sizeof("server") - 1)) {
			res += unit_test_module_ipc_server(argv[i]);
		} else {
			ELOG("unknown unit test: %s", argv[i]);
		}

		if (res) {
			ELOG("unit test: %s, had %d failures", argv[i], res);
		} else {
			SLOG("unit test: %s passed", argv[i]);
		}

		total += res;
	}

	if (total) {
		ELOG("had %d failures", total);
	} else {
		SLOG("all passed");
	}

	return total;
}
