/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
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

#include "config.h"

#include <sys/param.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "common.h"
#include "proc.h"
#include "prototype.h"
#include "read_config_file.h"
#include "summary.h"

char *command = NULL;

int exiting = 0;		/* =1 if a SIGINT or SIGTERM has been received */

static enum callback_status
stop_non_p_processes(struct process *proc, void *data)
{
	int stop = 1;

	struct opt_p_t *it;
	for (it = opt_p; it != NULL; it = it->next) {
		struct process *p_proc = pid2proc(it->pid);
		if (p_proc == NULL) {
			printf("stop_non_p_processes: %d terminated?\n", it->pid);
			continue;
		}
		if (p_proc == proc || p_proc->leader == proc->leader) {
			stop = 0;
			break;
		}
	}

	if (stop) {
		debug(2, "Sending SIGSTOP to process %u", proc->pid);
		kill(proc->pid, SIGSTOP);
	}

	return CBS_CONT;
}

static void
signal_alarm(int sig) {
	signal(SIGALRM, SIG_DFL);
	each_process(NULL, &stop_non_p_processes, NULL);
}

static void
signal_exit(int sig)
{
	if (exiting != 0)
		return;

	exiting = 1 + !!os_ltrace_exiting_sighandler();

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGALRM, signal_alarm);
	//alarm(1);
}

static void
normal_exit(void)
{
	if (options.summary)
		show_summary();
	if (options.output) {
		fclose(options.output);
		options.output = NULL;
	}
}

void
ltrace_init(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	struct opt_p_t *opt_p_tmp;

	atexit(normal_exit);
	signal(SIGINT, signal_exit);	/* Detach processes when interrupted */
	signal(SIGTERM, signal_exit);	/*  ... or killed */

	argv = process_options(argc, argv);
	init_global_config();

	if (command) {
		/* Check that the binary ABI is supported before
		 * calling execute_program.  */
		{
			struct ltelf lte;
			if (ltelf_init(&lte, command) == 0)
				ltelf_destroy(&lte);
			else
				exit(EXIT_FAILURE);
		}

		pid_t pid = execute_program(command, argv);
		struct process *proc = open_program(command, pid);
		if (proc == NULL) {
			fprintf(stderr, "couldn't open program '%s': %s\n",
				command, strerror(errno));
			exit(EXIT_FAILURE);
		}

		trace_set_options(proc);
		continue_process(pid);
	}
	opt_p_tmp = opt_p;
	while (opt_p_tmp) {
		open_pid(opt_p_tmp->pid);
		opt_p_tmp = opt_p_tmp->next;
	}
}

static int num_ltrace_callbacks[EVENT_MAX];
static callback_func * ltrace_callbacks[EVENT_MAX];

void
ltrace_add_callback(callback_func func, Event_type type) {
	ltrace_callbacks[type] = realloc(ltrace_callbacks[type], (num_ltrace_callbacks[type]+1)*sizeof(callback_func));
	ltrace_callbacks[type][num_ltrace_callbacks[type]++] = func;
}

static void
dispatch_callbacks(Event * ev) {
	int i;
	/* Ignoring case 1: signal into a dying tracer */
	if (ev->type==EVENT_SIGNAL && 
			exiting && ev->e_un.signum == SIGSTOP) {
		return;
	}
	/* Ignoring case 2: process being born before a clone event */
	if (ev->proc && ev->proc->state == STATE_IGNORED) {
		return;
	}
	for (i=0; i<num_ltrace_callbacks[ev->type]; i++) {
		ltrace_callbacks[ev->type][i](ev);
	}
}

void
ltrace_main(void) {
	Event * ev;
	while (1) {
		ev = next_event();
		dispatch_callbacks(ev);
		handle_event(ev);
	}
}
