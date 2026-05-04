/*
 * This file is part of ltrace.
 * Copyright (C) 2009 Juan Cespedes
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <stdio.h>
#include <unistd.h>

#include "ltrace.h"

/*
static int count_call =0;
static int count_ret  =0;

static void
callback_call(Event * ev) {
	count_call ++;
}
static void
callback_ret(Event * ev) {
	count_ret ++;
}

static void
endcallback(Event *ev) {
	printf("%d calls\n%d rets\n",count_call, count_ret);
}
*/

int
main(int argc, char *argv[]) {
	ltrace_init(argc, argv);

/*
	ltrace_add_callback(callback_call, EVENT_SYSCALL);
	ltrace_add_callback(callback_ret, EVENT_SYSRET);
	ltrace_add_callback(endcallback, EVENT_EXIT);
*/

	ltrace_main();
	return 0;
}
