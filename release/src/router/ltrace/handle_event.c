/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Arnaud Patard, Mandriva SA
 * Copyright (C) 1998,2001,2002,2003,2004,2007,2008,2009 Juan Cespedes
 * Copyright (C) 2008 Luis Machado, IBM Corporation
 * Copyright (C) 2006 Ian Wienand
 * Copyright (C) 2006 Paul Gilliam, IBM Corporation
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

#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "backend.h"
#include "breakpoint.h"
#include "common.h"
#include "fetch.h"
#include "library.h"
#include "proc.h"
#include "prototype.h"
#include "summary.h"
#include "value_dict.h"

static void handle_signal(Event *event);
static void handle_exit(Event *event);
static void handle_exit_signal(Event *event);
static void handle_syscall(Event *event);
static void handle_arch_syscall(Event *event);
static void handle_sysret(Event *event);
static void handle_arch_sysret(Event *event);
static void handle_clone(Event *event);
static void handle_exec(Event *event);
static void handle_breakpoint(Event *event);
static void handle_new(Event *event);

static void callstack_push_syscall(struct process *proc, int sysnum);
static void callstack_push_symfunc(struct process *proc, struct breakpoint *bp);
/* XXX Stack maintenance should be moved to a dedicated module, or to
 * proc.c, and push/pop should be visible outside this module.  For
 * now, because we need this in proc.c, this is non-static.  */
void callstack_pop(struct process *proc);

static char *shortsignal(struct process *proc, int signum);
static char *sysname(struct process *proc, int sysnum);
static char *arch_sysname(struct process *proc, int sysnum);

static Event *
call_handler(struct process *proc, Event *event)
{
	assert(proc != NULL);

	struct event_handler *handler = proc->event_handler;
	if (handler == NULL)
		return event;

	return (*handler->on_event) (handler, event);
}

void
handle_event(Event *event)
{
	if (exiting == 1) {
		debug(1, "ltrace about to exit");
		os_ltrace_exiting();
		exiting = 2;
	}
	debug(DEBUG_FUNCTION, "handle_event(pid=%d, type=%d)",
	      event->proc ? event->proc->pid : -1, event->type);

	/* If the thread group or an individual task define an
	   overriding event handler, give them a chance to kick in.
	   We will end up calling both handlers, if the first one
	   doesn't sink the event.  */
	if (event->proc != NULL) {
		event = call_handler(event->proc, event);
		if (event == NULL)
			/* It was handled.  */
			return;

		/* Note: the previous handler has a chance to alter
		 * the event.  */
		if (event->proc != NULL
		    && event->proc->leader != NULL
		    && event->proc != event->proc->leader) {
			event = call_handler(event->proc->leader, event);
			if (event == NULL)
				return;
		}
	}

	switch (event->type) {
	case EVENT_NONE:
		debug(1, "event: none");
		return;

	case EVENT_SIGNAL:
		assert(event->proc != NULL);
		debug(1, "[%d] event: signal (%s [%d])",
		      event->proc->pid,
		      shortsignal(event->proc, event->e_un.signum),
		      event->e_un.signum);
		handle_signal(event);
		return;

	case EVENT_EXIT:
		assert(event->proc != NULL);
		debug(1, "[%d] event: exit (%d)",
		      event->proc->pid,
		      event->e_un.ret_val);
		handle_exit(event);
		return;

	case EVENT_EXIT_SIGNAL:
		assert(event->proc != NULL);
		debug(1, "[%d] event: exit signal (%s [%d])",
		      event->proc->pid,
		      shortsignal(event->proc, event->e_un.signum),
		      event->e_un.signum);
		handle_exit_signal(event);
		return;

	case EVENT_SYSCALL:
		assert(event->proc != NULL);
		debug(1, "[%d] event: syscall (%s [%d])",
		      event->proc->pid,
		      sysname(event->proc, event->e_un.sysnum),
		      event->e_un.sysnum);
		handle_syscall(event);
		return;

	case EVENT_SYSRET:
		assert(event->proc != NULL);
		debug(1, "[%d] event: sysret (%s [%d])",
		      event->proc->pid,
		      sysname(event->proc, event->e_un.sysnum),
		      event->e_un.sysnum);
		handle_sysret(event);
		return;

	case EVENT_ARCH_SYSCALL:
		assert(event->proc != NULL);
		debug(1, "[%d] event: arch_syscall (%s [%d])",
		      event->proc->pid,
		      arch_sysname(event->proc, event->e_un.sysnum),
		      event->e_un.sysnum);
		handle_arch_syscall(event);
		return;

	case EVENT_ARCH_SYSRET:
		assert(event->proc != NULL);
		debug(1, "[%d] event: arch_sysret (%s [%d])",
		      event->proc->pid,
		      arch_sysname(event->proc, event->e_un.sysnum),
		      event->e_un.sysnum);
		handle_arch_sysret(event);
		return;

	case EVENT_CLONE:
	case EVENT_VFORK:
		assert(event->proc != NULL);
		debug(1, "[%d] event: clone (%u)",
		      event->proc->pid, event->e_un.newpid);
		handle_clone(event);
		return;

	case EVENT_EXEC:
		assert(event->proc != NULL);
		debug(1, "[%d] event: exec()",
		      event->proc->pid);
		handle_exec(event);
		return;

	case EVENT_BREAKPOINT:
		assert(event->proc != NULL);
		debug(1, "[%d] event: breakpoint %p",
		      event->proc->pid, event->e_un.brk_addr);
		handle_breakpoint(event);
		return;

	case EVENT_NEW:
		debug(1, "[%d] event: new process",
		      event->e_un.newpid);
		handle_new(event);
		return;
	default:
		fprintf(stderr, "Error! unknown event?\n");
		exit(1);
	}
}

typedef struct Pending_New Pending_New;
struct Pending_New {
	pid_t pid;
	Pending_New * next;
};
static Pending_New * pending_news = NULL;

static int
pending_new(pid_t pid) {
	Pending_New * p;

	debug(DEBUG_FUNCTION, "pending_new(%d)", pid);

	p = pending_news;
	while (p) {
		if (p->pid == pid) {
			return 1;
		}
		p = p->next;
	}
	return 0;
}

static void
pending_new_insert(pid_t pid) {
	Pending_New * p;

	debug(DEBUG_FUNCTION, "pending_new_insert(%d)", pid);

	p = malloc(sizeof(Pending_New));
	if (!p) {
		perror("malloc()");
		exit(1);
	}
	p->pid = pid;
	p->next = pending_news;
	pending_news = p;
}

static void
pending_new_remove(pid_t pid)
{
	debug(DEBUG_FUNCTION, "pending_new_remove(%d)", pid);

	Pending_New **pp;
	for (pp = &pending_news; *pp != NULL; pp = &(*pp)->next)
		if ((*pp)->pid == pid) {
			Pending_New *p = *pp;
			*pp = p->next;
			free(p);
			return;
		}
}

static void
handle_clone(Event *event)
{
	debug(DEBUG_FUNCTION, "handle_clone(pid=%d)", event->proc->pid);

	struct process *proc = malloc(sizeof(*proc));
	pid_t newpid = event->e_un.newpid;
	if (proc == NULL
	    || process_clone(proc, event->proc, newpid) < 0) {
		free(proc);
		proc = NULL;
		fprintf(stderr,
			"Couldn't initialize tracing of process %d.\n",
			newpid);

	} else {
		proc->parent = event->proc;
		/* We save register values to the arch pointer, and
		 * these need to be per-thread.  XXX arch_ptr should
		 * be retired in favor of fetch interface anyway.  */
		proc->arch_ptr = NULL;
	}

	if (pending_new(newpid)) {
		pending_new_remove(newpid);

		if (proc != NULL) {
			proc->event_handler = NULL;
			if (event->proc->state == STATE_ATTACHED
			    && options.follow)
				proc->state = STATE_ATTACHED;
			else
				proc->state = STATE_IGNORED;
		}

		continue_process(newpid);

	} else if (proc != NULL) {
		proc->state = STATE_BEING_CREATED;
	}

	if (event->type != EVENT_VFORK)
		continue_process(event->proc->pid);
	else if (proc != NULL)
		continue_after_vfork(proc);
	else
		continue_process(newpid);
}

static void
handle_new(Event *event)
{
	debug(DEBUG_FUNCTION, "handle_new(pid=%d)", event->e_un.newpid);

	struct process *proc = pid2proc(event->e_un.newpid);
	if (!proc) {
		pending_new_insert(event->e_un.newpid);
	} else {
		assert(proc->state == STATE_BEING_CREATED);
		if (options.follow) {
			proc->state = STATE_ATTACHED;
		} else {
			proc->state = STATE_IGNORED;
		}
		continue_process(proc->pid);
	}
}

static char *
shortsignal(struct process *proc, int signum)
{
	static char *signalent0[] = {
#include "signalent.h"
	};
	static char *signalent1[] = {
#include "signalent1.h"
	};
	static char **signalents[] = { signalent0, signalent1 };
	int nsignals[] = { sizeof signalent0 / sizeof signalent0[0],
		sizeof signalent1 / sizeof signalent1[0]
	};

	debug(DEBUG_FUNCTION, "shortsignal(pid=%d, signum=%d)", proc->pid, signum);

	assert(proc->personality < sizeof signalents / sizeof signalents[0]);
	if (signum < 0 || signum >= nsignals[proc->personality]) {
		return "UNKNOWN_SIGNAL";
	} else {
		return signalents[proc->personality][signum];
	}
}

static char *
sysname(struct process *proc, int sysnum)
{
	static char result[128];
	static char *syscallent0[] = {
#include "syscallent.h"
	};
	static char *syscallent1[] = {
#include "syscallent1.h"
	};
	static char **syscallents[] = { syscallent0, syscallent1 };
	int nsyscalls[] = {
		sizeof syscallent0 / sizeof syscallent0[0],
		sizeof syscallent1 / sizeof syscallent1[0],
	};

	debug(DEBUG_FUNCTION, "sysname(pid=%d, sysnum=%d)", proc->pid, sysnum);

	assert(proc->personality < sizeof syscallents / sizeof syscallents[0]);
	if (sysnum < 0 || sysnum >= nsyscalls[proc->personality]) {
		sprintf(result, "SYS_%d", sysnum);
		return result;
	} else {
		return syscallents[proc->personality][sysnum];
	}
}

static char *
arch_sysname(struct process *proc, int sysnum)
{
	static char result[128];
	static char *arch_syscallent[] = {
#include "arch_syscallent.h"
	};
	int nsyscalls = sizeof arch_syscallent / sizeof arch_syscallent[0];

	debug(DEBUG_FUNCTION, "arch_sysname(pid=%d, sysnum=%d)", proc->pid, sysnum);

	if (sysnum < 0 || sysnum >= nsyscalls) {
		sprintf(result, "ARCH_%d", sysnum);
		return result;
	} else {
		sprintf(result, "ARCH_%s", arch_syscallent[sysnum]);
		return result;
	}
}

#ifndef HAVE_STRSIGNAL
# define strsignal(SIGNUM) "???"
#endif

static void
handle_signal(Event *event) {
	debug(DEBUG_FUNCTION, "handle_signal(pid=%d, signum=%d)", event->proc->pid, event->e_un.signum);
	if (event->proc->state != STATE_IGNORED && !options.no_signals) {
		output_line(event->proc, "--- %s (%s) ---",
				shortsignal(event->proc, event->e_un.signum),
				strsignal(event->e_un.signum));
	}
	continue_after_signal(event->proc->pid, event->e_un.signum);
}

static int
init_syscall_symbol(struct library_symbol *libsym, const char *name)
{
	static struct library syscall_lib;

	if (syscall_lib.protolib == NULL) {
		struct protolib *protolib
			= protolib_cache_load(&g_protocache, "syscalls", 0, 1);
		if (protolib == NULL) {
			fprintf(stderr, "Couldn't load system call prototypes:"
				" %s.\n", strerror(errno));

			/* Instead, get a fake one just so we can
			 * carry on, limping.  */
			protolib = malloc(sizeof *protolib);
			if (protolib == NULL) {
				fprintf(stderr, "Couldn't even allocate a fake "
					"prototype library: %s.\n",
					strerror(errno));
				abort();
			}
			protolib_init(protolib);
		}

		assert(protolib != NULL);
		if (library_init(&syscall_lib, LT_LIBTYPE_SYSCALL) < 0) {
			fprintf(stderr, "Couldn't initialize system call "
				"library: %s.\n", strerror(errno));
			abort();
		}

		library_set_soname(&syscall_lib, "SYS", 0);
		syscall_lib.protolib = protolib;
	}

	if (library_symbol_init(libsym, 0, name, 0, LS_TOPLT_NONE) < 0)
		return -1;

	libsym->lib = &syscall_lib;
	return 0;
}

/* Account the unfinished functions on the call stack.  */
static void
account_current_callstack(struct process *proc)
{
	if (! options.summary)
		return;

	struct timedelta spent[proc->callstack_depth];

	size_t i;
	for (i = 0; i < proc->callstack_depth; ++i) {
		struct callstack_element *elem = &proc->callstack[i];
		spent[i] = calc_time_spent(elem->enter_time);
	}

	for (i = 0; i < proc->callstack_depth; ++i) {
		struct callstack_element *elem = &proc->callstack[i];
		struct library_symbol syscall, *libsym = NULL;
		if (elem->is_syscall) {
			const char *name = sysname(proc, elem->c_un.syscall);
			if (init_syscall_symbol(&syscall, name) >= 0)
				libsym = &syscall;

		} else {
			libsym = elem->c_un.libfunc;
		}

		if (libsym != NULL) {
			summary_account_call(libsym, spent[i]);

			if (elem->is_syscall)
				library_symbol_destroy(&syscall);
		}
	}
}

static void
handle_exit(Event *event) {
	debug(DEBUG_FUNCTION, "handle_exit(pid=%d, status=%d)", event->proc->pid, event->e_un.ret_val);
	if (event->proc->state != STATE_IGNORED) {
		output_line(event->proc, "+++ exited (status %d) +++",
				event->e_un.ret_val);
	}

	account_current_callstack(event->proc);
	remove_process(event->proc);
}

static void
handle_exit_signal(Event *event) {
	debug(DEBUG_FUNCTION, "handle_exit_signal(pid=%d, signum=%d)", event->proc->pid, event->e_un.signum);
	if (event->proc->state != STATE_IGNORED) {
		output_line(event->proc, "+++ killed by %s +++",
				shortsignal(event->proc, event->e_un.signum));
	}

	account_current_callstack(event->proc);
	remove_process(event->proc);
}

static void
output_syscall(struct process *proc, const char *name, enum tof tof,
	       bool left, struct timedelta *spent)
{
	if (left)
		assert(spent == NULL);

	struct library_symbol syscall;
	if (init_syscall_symbol(&syscall, name) >= 0) {
		if (left) {
			if (! options.summary)
				output_left(tof, proc, &syscall);
		} else if (options.summary) {
			summary_account_call(&syscall, *spent);
		} else {
			output_right(tof, proc, &syscall, spent);
		}

		library_symbol_destroy(&syscall);
	}
}

static void
output_syscall_left(struct process *proc, const char *name)
{
	output_syscall(proc, name, LT_TOF_SYSCALL, true, NULL);
}

static void
output_syscall_right(struct process *proc, const char *name,
		     struct timedelta *spent)
{
	output_syscall(proc, name, LT_TOF_SYSCALLR, false, spent);
}

static void
handle_syscall(Event *event)
{
	debug(DEBUG_FUNCTION, "handle_syscall(pid=%d, sysnum=%d)", event->proc->pid, event->e_un.sysnum);
	if (event->proc->state != STATE_IGNORED) {
		callstack_push_syscall(event->proc, event->e_un.sysnum);
		if (options.syscalls)
			output_syscall_left(event->proc,
					    sysname(event->proc,
						    event->e_un.sysnum));
	}
	continue_after_syscall(event->proc, event->e_un.sysnum, 0);
}

static void
handle_exec(Event *event)
{
	struct process *proc = event->proc;

	/* Save the PID so that we can use it after unsuccessful
	 * process_exec.  */
	pid_t pid = proc->pid;

	debug(DEBUG_FUNCTION, "handle_exec(pid=%d)", proc->pid);
	if (proc->state == STATE_IGNORED) {
	untrace:
		untrace_pid(pid);
		remove_process(proc);
		return;
	}
	output_line(proc, "--- Called exec() ---");

	account_current_callstack(proc);

	if (process_exec(proc) < 0) {
		fprintf(stderr,
			"couldn't reinitialize process %d after exec\n", pid);
		goto untrace;
	}

	continue_after_exec(proc);
}

static void
handle_arch_syscall(Event *event) {
	debug(DEBUG_FUNCTION, "handle_arch_syscall(pid=%d, sysnum=%d)", event->proc->pid, event->e_un.sysnum);
	if (event->proc->state != STATE_IGNORED) {
		callstack_push_syscall(event->proc, 0xf0000 + event->e_un.sysnum);
		if (options.syscalls) {
			output_syscall_left(event->proc,
					    arch_sysname(event->proc,
							 event->e_un.sysnum));
		}
	}
	continue_process(event->proc->pid);
}

static void
handle_x_sysret(Event *event, char *(*name_cb)(struct process *, int))
{
	debug(DEBUG_FUNCTION, "handle_x_sysret(pid=%d, sysnum=%d)",
	      event->proc->pid, event->e_un.sysnum);

	if (event->proc->state != STATE_IGNORED) {
		unsigned d = event->proc->callstack_depth;
		assert(d > 0);
		struct callstack_element *elem = &event->proc->callstack[d - 1];
		assert(elem->is_syscall);

		struct timedelta spent = calc_time_spent(elem->enter_time);
		if (options.syscalls)
			output_syscall_right(event->proc,
					     name_cb(event->proc,
						     event->e_un.sysnum),
					     &spent);

		callstack_pop(event->proc);
	}
	continue_after_syscall(event->proc, event->e_un.sysnum, 1);
}

static void
handle_sysret(Event *event)
{
	handle_x_sysret(event, &sysname);
}

static void
handle_arch_sysret(Event *event)
{
	handle_x_sysret(event, &arch_sysname);
}

static void
output_right_tos(struct process *proc)
{
	size_t d = proc->callstack_depth;
	assert(d > 0);
	struct callstack_element *elem = &proc->callstack[d - 1];
	assert(! elem->is_syscall);

	if (proc->state != STATE_IGNORED) {
		struct timedelta spent = calc_time_spent(elem->enter_time);
		if (options.summary)
			summary_account_call(elem->c_un.libfunc, spent);
		else
			output_right(LT_TOF_FUNCTIONR, proc, elem->c_un.libfunc,
				     &spent);
	}
}

#ifndef ARCH_HAVE_SYMBOL_RET
void arch_symbol_ret(struct process *proc, struct library_symbol *libsym)
{
}
#endif

static void
handle_breakpoint(Event *event)
{
	int i, j;
	struct breakpoint *sbp;
	struct process *leader = event->proc->leader;
	void *brk_addr = event->e_un.brk_addr;

	/* The leader has terminated.  */
	if (leader == NULL) {
		continue_process(event->proc->pid);
		return;
	}

	debug(DEBUG_FUNCTION, "handle_breakpoint(pid=%d, addr=%p)",
	      event->proc->pid, brk_addr);
	debug(2, "event: breakpoint (%p)", brk_addr);

	for (i = event->proc->callstack_depth - 1; i >= 0; i--) {
		if (brk_addr == event->proc->callstack[i].return_addr) {
			for (j = event->proc->callstack_depth - 1; j > i; j--)
				callstack_pop(event->proc);

			struct library_symbol *libsym =
			    event->proc->callstack[i].c_un.libfunc;

			arch_symbol_ret(event->proc, libsym);
			output_right_tos(event->proc);
			callstack_pop(event->proc);

			/* Pop also any other entries that seem like
			 * they are linked to the current one: they
			 * have the same return address, but were made
			 * for different symbols.  This should only
			 * happen for entry point tracing, i.e. for -x
			 * everywhere, or -x and -e on MIPS.  */
			while (event->proc->callstack_depth > 0) {
				struct callstack_element *prev;
				size_t d = event->proc->callstack_depth;
				prev = &event->proc->callstack[d - 1];

				if (prev->c_un.libfunc == libsym
				    || prev->return_addr != brk_addr)
					break;

				arch_symbol_ret(event->proc,
						prev->c_un.libfunc);
				output_right_tos(event->proc);
				callstack_pop(event->proc);
			}

			/* Maybe the previous callstack_pop's got rid
			 * of the breakpoint, but if we are in a
			 * recursive call, it's still enabled.  In
			 * that case we need to skip it properly.  */
			if ((sbp = address2bpstruct(leader, brk_addr)) != NULL) {
				continue_after_breakpoint(event->proc, sbp);
			} else {
				set_instruction_pointer(event->proc, brk_addr);
				continue_process(event->proc->pid);
			}
			return;
		}
	}

	if ((sbp = address2bpstruct(leader, brk_addr)) != NULL)
		breakpoint_on_hit(sbp, event->proc);
	else if (event->proc->state != STATE_IGNORED)
		output_line(event->proc,
			    "unexpected breakpoint at %p", brk_addr);

	/* breakpoint_on_hit may delete its own breakpoint, so we have
	 * to look it up again.  */
	if ((sbp = address2bpstruct(leader, brk_addr)) != NULL) {

		if (event->proc->state != STATE_IGNORED
		    && sbp->libsym != NULL) {
			event->proc->stack_pointer = get_stack_pointer(event->proc);
			callstack_push_symfunc(event->proc, sbp);
			if (! options.summary)
				output_left(LT_TOF_FUNCTION, event->proc,
					    sbp->libsym);
		}

		breakpoint_on_continue(sbp, event->proc);
		return;
	} else {
		set_instruction_pointer(event->proc, brk_addr);
	}

	continue_process(event->proc->pid);
}

static void
callstack_push_syscall(struct process *proc, int sysnum)
{
	struct callstack_element *elem;

	debug(DEBUG_FUNCTION, "callstack_push_syscall(pid=%d, sysnum=%d)", proc->pid, sysnum);
	/* FIXME: not good -- should use dynamic allocation. 19990703 mortene. */
	if (proc->callstack_depth == MAX_CALLDEPTH - 1) {
		fprintf(stderr, "%s: Error: call nesting too deep!\n", __func__);
		abort();
		return;
	}

	elem = &proc->callstack[proc->callstack_depth];
	*elem = (struct callstack_element){};
	elem->is_syscall = 1;
	elem->c_un.syscall = sysnum;
	elem->return_addr = NULL;

	proc->callstack_depth++;
	if (opt_T || options.summary) {
		struct timezone tz;
		gettimeofday(&elem->enter_time, &tz);
	}
}

static void
callstack_push_symfunc(struct process *proc, struct breakpoint *bp)
{
	struct callstack_element *elem;

	debug(DEBUG_FUNCTION, "callstack_push_symfunc(pid=%d, symbol=%s)",
	      proc->pid, bp->libsym->name);
	/* FIXME: not good -- should use dynamic allocation. 19990703 mortene. */
	if (proc->callstack_depth == MAX_CALLDEPTH - 1) {
		fprintf(stderr, "%s: Error: call nesting too deep!\n", __func__);
		abort();
		return;
	}

	elem = &proc->callstack[proc->callstack_depth++];
	*elem = (struct callstack_element){};
	elem->is_syscall = 0;
	elem->c_un.libfunc = bp->libsym;

	struct breakpoint *rbp = NULL;
	if (breakpoint_get_return_bp(&rbp, bp, proc) == 0
	    && rbp != NULL) {
		struct breakpoint *ext_rbp = insert_breakpoint(proc, rbp);
		if (ext_rbp != rbp) {
			breakpoint_destroy(rbp);
			free(rbp);
			rbp = ext_rbp;
		}
	}

	elem->return_addr = rbp != NULL ? rbp->addr : 0;

	if (opt_T || options.summary) {
		struct timezone tz;
		gettimeofday(&elem->enter_time, &tz);
	}
}

void
callstack_pop(struct process *proc)
{
	struct callstack_element *elem;
	assert(proc->callstack_depth > 0);

	debug(DEBUG_FUNCTION, "callstack_pop(pid=%d)", proc->pid);
	elem = &proc->callstack[proc->callstack_depth - 1];
	if (!elem->is_syscall && elem->return_addr) {
		struct breakpoint *bp
			= address2bpstruct(proc->leader, elem->return_addr);
		if (bp != NULL) {
			breakpoint_on_hit(bp, proc);
			delete_breakpoint(proc, bp);
		}
	}

	if (elem->fetch_context != NULL)
		fetch_arg_done(elem->fetch_context);

	if (elem->arguments != NULL) {
		val_dict_destroy(elem->arguments);
		free(elem->arguments);
	}

	proc->callstack_depth--;
}
