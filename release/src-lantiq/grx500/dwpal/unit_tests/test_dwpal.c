/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "unitest_helper.h"
#include <stdio.h>
#include <string.h>

int unit_test_module_dwpal_ext(char *tests);
int unit_test_module_dwpal_daemon(char *tests);

int main(int argc, char *argv[])
{
	int i, total = 0;

	for (i = 1; i < argc; i++) {
		int res = 0;

		ILOG( "starting test: %s", argv[i]);
		if (!strcmp(argv[i], "all")) {
			res += unit_test_module_dwpal_ext(NULL);
			res += unit_test_module_dwpal_daemon(NULL);
		} else if (!strncmp(argv[i], "dwpal_ext", sizeof("dwpal_ext") - 1)) {
			res += unit_test_module_dwpal_ext(argv[i]);
		} else if (!strncmp(argv[i], "daemon", sizeof("daemon") - 1)) {
			res += unit_test_module_dwpal_daemon(argv[i]);
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
