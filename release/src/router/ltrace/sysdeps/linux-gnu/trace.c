/*
 * This file is part of ltrace.
 * Copyright (C) 2007,2011,2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Joe Damato
 * Copyright (C) 1998,2002,2003,2004,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
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

#include <asm/unistd.h>
#include <assert.h>
#include <errno.h>
#include <gelf.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef HAVE_LIBSELINUX
# include <selinux/selinux.h>
#endif

#include "linux-gnu/trace-defs.h"
#include "linux-gnu/trace.h"
#include "backend.h"
#include "breakpoint.h"
#include "debug.h"
#include "events.h"
#include "fetch.h"
#include "ltrace-elf.h"
#include "options.h"
#include "proc.h"
#include "prototype.h"
#include "ptrace.h"
#include "type.h"
#include "value.h"

void
trace_fail_warning(pid_t pid)
{
	/* This was adapted from GDB.  */
#ifdef HAVE_LIBSELINUX
	static int checked = 0;
	if (checked)
		return;
	checked = 1;

	/* -1 is returned for errors, 0 if it has no effect, 1 if
	 * PTRACE_ATTACH is forbidden.  */
	if (security_get_boolean_active("deny_ptrace") == 1)
		fprintf(stderr,
"The SELinux boolean 'deny_ptrace' is enabled, which may prevent ltrace from\n"
"tracing other processes.  You can disable this process attach protection by\n"
"issuing 'setsebool deny_ptrace=0' in the superuser context.\n");
#endif /* HAVE_LIBSELINUX */
}

void
trace_me(void)
{
	debug(DEBUG_PROCESS, "trace_me: pid=%d", getpid());
	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
		perror("PTRACE_TRACEME");
		trace_fail_warning(getpid());
		exit(1);
	}
}

/* There's a (hopefully) brief period of time after the child process
 * forks when we can't trace it yet.  Here we wait for kernel to
 * prepare the process.  */
int
wait_for_proc(pid_t pid)
{
	/* man ptrace: PTRACE_ATTACH attaches to the process specified
	   in pid.  The child is sent a SIGSTOP, but will not
	   necessarily have stopped by the completion of this call;
	   use wait() to wait for the child to stop. */
	if (waitpid(pid, NULL, __WALL) != pid) {
		perror ("trace_pid: waitpid");
		return -1;
	}

	return 0;
}

int
trace_pid(pid_t pid)
{
	debug(DEBUG_PROCESS, "trace_pid: pid=%d", pid);
	/* This shouldn't emit error messages, as there are legitimate
	 * reasons that the PID can't be attached: like it may have
	 * already ended.  */
	if (ptrace(PTRACE_ATTACH, pid, 0, 0) < 0)
		return -1;

	return wait_for_proc(pid);
}

void
trace_set_options(struct process *proc)
{
	if (proc->tracesysgood & 0x80)
		return;

	pid_t pid = proc->pid;
	debug(DEBUG_PROCESS, "trace_set_options: pid=%d", pid);

	long options = PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACEFORK |
		PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE |
		PTRACE_O_TRACEEXEC;
	if (ptrace(PTRACE_SETOPTIONS, pid, 0, (void *)options) < 0 &&
	    ptrace(PTRACE_OLDSETOPTIONS, pid, 0, (void *)options) < 0) {
		perror("PTRACE_SETOPTIONS");
		return;
	}
	proc->tracesysgood |= 0x80;
}

void
untrace_pid(pid_t pid) {
	debug(DEBUG_PROCESS, "untrace_pid: pid=%d", pid);
	ptrace(PTRACE_DETACH, pid, 0, 0);
}

void
continue_after_signal(pid_t pid, int signum)
{
	debug(DEBUG_PROCESS, "continue_after_signal: pid=%d, signum=%d",
	      pid, signum);
	ptrace(PTRACE_SYSCALL, pid, 0, (void *)(uintptr_t)signum);
}

static enum ecb_status
event_for_pid(Event *event, void *data)
{
	if (event->proc != NULL && event->proc->pid == (pid_t)(uintptr_t)data)
		return ECB_YIELD;
	return ECB_CONT;
}

static int
have_events_for(pid_t pid)
{
	return each_qd_event(event_for_pid, (void *)(uintptr_t)pid) != NULL;
}

void
continue_process(pid_t pid)
{
	debug(DEBUG_PROCESS, "continue_process: pid=%d", pid);

	/* Only really continue the process if there are no events in
	   the queue for this process.  Otherwise just wait for the
	   other events to arrive.  */
	if (!have_events_for(pid))
		/* We always trace syscalls to control fork(),
		 * clone(), execve()... */
		ptrace(PTRACE_SYSCALL, pid, 0, 0);
	else
		debug(DEBUG_PROCESS,
		      "putting off the continue, events in que.");
}

static struct pid_task *
get_task_info(struct pid_set *pids, pid_t pid)
{
	assert(pid != 0);
	size_t i;
	for (i = 0; i < pids->count; ++i)
		if (pids->tasks[i].pid == pid)
			return &pids->tasks[i];

	return NULL;
}

static struct pid_task *
add_task_info(struct pid_set *pids, pid_t pid)
{
	if (pids->count == pids->alloc) {
		size_t ns = (2 * pids->alloc) ?: 4;
		struct pid_task *n = realloc(pids->tasks,
					     sizeof(*pids->tasks) * ns);
		if (n == NULL)
			return NULL;
		pids->tasks = n;
		pids->alloc = ns;
	}
	struct pid_task * task_info = &pids->tasks[pids->count++];
	memset(task_info, 0, sizeof(*task_info));
	task_info->pid = pid;
	return task_info;
}

static enum callback_status
task_stopped(struct process *task, void *data)
{
	enum process_status st = process_status(task->pid);
	if (data != NULL)
		*(enum process_status *)data = st;

	/* If the task is already stopped, don't worry about it.
	 * Likewise if it managed to become a zombie or terminate in
	 * the meantime.  This can happen when the whole thread group
	 * is terminating.  */
	switch (st) {
	case PS_INVALID:
	case PS_TRACING_STOP:
	case PS_ZOMBIE:
		return CBS_CONT;
	case PS_SLEEPING:
	case PS_STOP:
	case PS_OTHER:
		return CBS_STOP;
	}

	abort ();
}

/* Task is blocked if it's stopped, or if it's a vfork parent.  */
static enum callback_status
task_blocked(struct process *task, void *data)
{
	struct pid_set *pids = data;
	struct pid_task *task_info = get_task_info(pids, task->pid);
	if (task_info != NULL
	    && task_info->vforked)
		return CBS_CONT;

	return task_stopped(task, NULL);
}

static Event *process_vfork_on_event(struct event_handler *super, Event *event);

static enum callback_status
task_vforked(struct process *task, void *data)
{
	if (task->event_handler != NULL
	    && task->event_handler->on_event == &process_vfork_on_event)
		return CBS_STOP;
	return CBS_CONT;
}

static int
is_vfork_parent(struct process *task)
{
	return each_task(task->leader, NULL, &task_vforked, NULL) != NULL;
}

static enum callback_status
send_sigstop(struct process *task, void *data)
{
	struct process *leader = task->leader;
	struct pid_set *pids = data;

	/* Look for pre-existing task record, or add new.  */
	struct pid_task *task_info = get_task_info(pids, task->pid);
	if (task_info == NULL)
		task_info = add_task_info(pids, task->pid);
	if (task_info == NULL) {
		perror("send_sigstop: add_task_info");
		destroy_event_handler(leader);
		/* Signal failure upwards.  */
		return CBS_STOP;
	}

	/* This task still has not been attached to.  It should be
	   stopped by the kernel.  */
	if (task->state == STATE_BEING_CREATED)
		return CBS_CONT;

	/* Don't bother sending SIGSTOP if we are already stopped, or
	 * if we sent the SIGSTOP already, which happens when we are
	 * handling "onexit" and inherited the handler from breakpoint
	 * re-enablement.  */
	enum process_status st;
	if (task_stopped(task, &st) == CBS_CONT)
		return CBS_CONT;
	if (task_info->sigstopped) {
		if (!task_info->delivered)
			return CBS_CONT;
		task_info->delivered = 0;
	}

	/* Also don't attempt to stop the process if it's a parent of
	 * vforked process.  We set up event handler specially to hint
	 * us.  In that case parent is in D state, which we use to
	 * weed out unnecessary looping.  */
	if (st == PS_SLEEPING
	    && is_vfork_parent(task)) {
		task_info->vforked = 1;
		return CBS_CONT;
	}

	if (task_kill(task->pid, SIGSTOP) >= 0) {
		debug(DEBUG_PROCESS, "send SIGSTOP to %d", task->pid);
		task_info->sigstopped = 1;
	} else
		fprintf(stderr,
			"Warning: couldn't send SIGSTOP to %d\n", task->pid);

	return CBS_CONT;
}

/* On certain kernels, detaching right after a singlestep causes the
   tracee to be killed with a SIGTRAP (that even though the singlestep
   was properly caught by waitpid.  The ugly workaround is to put a
   breakpoint where IP points and let the process continue.  After
   this the breakpoint can be retracted and the process detached.  */
static void
ugly_workaround(struct process *proc)
{
	arch_addr_t ip = get_instruction_pointer(proc);
	struct breakpoint *found;
	if (DICT_FIND_VAL(proc->leader->breakpoints, &ip, &found) < 0) {
		insert_breakpoint_at(proc, ip, NULL);
	} else {
		assert(found != NULL);
		enable_breakpoint(proc, found);
	}
	ptrace(PTRACE_CONT, proc->pid, 0, 0);
}

static void
process_stopping_done(struct process_stopping_handler *self,
		      struct process *leader)
{
	debug(DEBUG_PROCESS, "process stopping done %d",
	      self->task_enabling_breakpoint->pid);

	if (!self->exiting) {
		size_t i;
		for (i = 0; i < self->pids.count; ++i)
			if (self->pids.tasks[i].pid != 0
			    && (self->pids.tasks[i].delivered
				|| self->pids.tasks[i].sysret))
				continue_process(self->pids.tasks[i].pid);
		continue_process(self->task_enabling_breakpoint->pid);
	}

	if (self->exiting) {
	ugly_workaround:
		self->state = PSH_UGLY_WORKAROUND;
		ugly_workaround(self->task_enabling_breakpoint);
	} else {
		switch ((self->ugly_workaround_p)(self)) {
		case CBS_FAIL:
			/* xxx handle me */
		case CBS_STOP:
			break;
		case CBS_CONT:
			goto ugly_workaround;
		}
		destroy_event_handler(leader);
	}
}

/* Before we detach, we need to make sure that task's IP is on the
 * edge of an instruction.  So for tasks that have a breakpoint event
 * in the queue, we adjust the instruction pointer, just like
 * continue_after_breakpoint does.  */
static enum ecb_status
undo_breakpoint(Event *event, void *data)
{
	if (event != NULL
	    && event->proc->leader == data
	    && event->type == EVENT_BREAKPOINT)
		set_instruction_pointer(event->proc, event->e_un.brk_addr);
	return ECB_CONT;
}

static enum callback_status
untrace_task(struct process *task, void *data)
{
	if (task != data)
		untrace_pid(task->pid);
	return CBS_CONT;
}

static enum callback_status
remove_task(struct process *task, void *data)
{
	/* Don't untrace leader just yet.  */
	if (task != data)
		remove_process(task);
	return CBS_CONT;
}

static enum callback_status
retract_breakpoint_cb(struct process *proc, struct breakpoint *bp, void *data)
{
	breakpoint_on_retract(bp, proc);
	return CBS_CONT;
}

static void
detach_process(struct process *leader)
{
	each_qd_event(&undo_breakpoint, leader);
	disable_all_breakpoints(leader);
	proc_each_breakpoint(leader, NULL, retract_breakpoint_cb, NULL);

	/* Now untrace the process, if it was attached to by -p.  */
	struct opt_p_t *it;
	for (it = opt_p; it != NULL; it = it->next) {
		struct process *proc = pid2proc(it->pid);
		if (proc == NULL)
			continue;
		if (proc->leader == leader) {
			each_task(leader, NULL, &untrace_task, NULL);
			break;
		}
	}
	each_task(leader, NULL, &remove_task, leader);
	destroy_event_handler(leader);
	remove_task(leader, NULL);
}

static void
handle_stopping_event(struct pid_task *task_info, Event **eventp)
{
	/* Mark all events, so that we know whom to SIGCONT later.  */
	if (task_info != NULL)
		task_info->got_event = 1;

	Event *event = *eventp;

	/* In every state, sink SIGSTOP events for tasks that it was
	 * sent to.  */
	if (task_info != NULL
	    && event->type == EVENT_SIGNAL
	    && event->e_un.signum == SIGSTOP) {
		debug(DEBUG_PROCESS, "SIGSTOP delivered to %d", task_info->pid);
		if (task_info->sigstopped
		    && !task_info->delivered) {
			task_info->delivered = 1;
			*eventp = NULL; // sink the event
		} else
			fprintf(stderr, "suspicious: %d got SIGSTOP, but "
				"sigstopped=%d and delivered=%d\n",
				task_info->pid, task_info->sigstopped,
				task_info->delivered);
	}
}

/* Some SIGSTOPs may have not been delivered to their respective tasks
 * yet.  They are still in the queue.  If we have seen an event for
 * that process, continue it, so that the SIGSTOP can be delivered and
 * caught by ltrace.  We don't mind that the process is after
 * breakpoint (and therefore potentially doesn't have aligned IP),
 * because the signal will be delivered without the process actually
 * starting.  */
static void
continue_for_sigstop_delivery(struct pid_set *pids)
{
	size_t i;
	for (i = 0; i < pids->count; ++i) {
		if (pids->tasks[i].pid != 0
		    && pids->tasks[i].sigstopped
		    && !pids->tasks[i].delivered
		    && pids->tasks[i].got_event) {
			debug(DEBUG_PROCESS, "continue %d for SIGSTOP delivery",
			      pids->tasks[i].pid);
			ptrace(PTRACE_SYSCALL, pids->tasks[i].pid, 0, 0);
		}
	}
}

static int
event_exit_p(Event *event)
{
	return event != NULL && (event->type == EVENT_EXIT
				 || event->type == EVENT_EXIT_SIGNAL);
}

static int
event_exit_or_none_p(Event *event)
{
	return event == NULL || event_exit_p(event)
		|| event->type == EVENT_NONE;
}

static int
await_sigstop_delivery(struct pid_set *pids, struct pid_task *task_info,
		       Event *event)
{
	/* If we still didn't get our SIGSTOP, continue the process
	 * and carry on.  */
	if (event != NULL && !event_exit_or_none_p(event)
	    && task_info != NULL && task_info->sigstopped) {
		debug(DEBUG_PROCESS, "continue %d for SIGSTOP delivery",
		      task_info->pid);
		/* We should get the signal the first thing
		 * after this, so it should be OK to continue
		 * even if we are over a breakpoint.  */
		ptrace(PTRACE_SYSCALL, task_info->pid, 0, 0);

	} else {
		/* If all SIGSTOPs were delivered, uninstall the
		 * handler and continue everyone.  */
		/* XXX I suspect that we should check tasks that are
		 * still around.  Is things are now, there should be a
		 * race between waiting for everyone to stop and one
		 * of the tasks exiting.  */
		int all_clear = 1;
		size_t i;
		for (i = 0; i < pids->count; ++i)
			if (pids->tasks[i].pid != 0
			    && pids->tasks[i].sigstopped
			    && !pids->tasks[i].delivered) {
				all_clear = 0;
				break;
			}
		return all_clear;
	}

	return 0;
}

static int
all_stops_accountable(struct pid_set *pids)
{
	size_t i;
	for (i = 0; i < pids->count; ++i)
		if (pids->tasks[i].pid != 0
		    && !pids->tasks[i].got_event
		    && !have_events_for(pids->tasks[i].pid))
			return 0;
	return 1;
}

#ifndef ARCH_HAVE_SW_SINGLESTEP
enum sw_singlestep_status
arch_sw_singlestep(struct process *proc, struct breakpoint *bp,
		   int (*add_cb)(arch_addr_t, struct sw_singlestep_data *),
		   struct sw_singlestep_data *data)
{
	return SWS_HW;
}
#endif

static Event *process_stopping_on_event(struct event_handler *super,
					Event *event);

static void
remove_sw_breakpoints(struct process *proc)
{
	struct process_stopping_handler *self
		= (void *)proc->leader->event_handler;
	assert(self != NULL);
	assert(self->super.on_event == process_stopping_on_event);

	int ct = sizeof(self->sws_bps) / sizeof(*self->sws_bps);
	int i;
	for (i = 0; i < ct; ++i)
		if (self->sws_bps[i] != NULL) {
			delete_breakpoint_at(proc, self->sws_bps[i]->addr);
			self->sws_bps[i] = NULL;
		}
}

static void
sw_singlestep_bp_on_hit(struct breakpoint *bp, struct process *proc)
{
	remove_sw_breakpoints(proc);
}

struct sw_singlestep_data {
	struct process_stopping_handler *self;
};

static int
sw_singlestep_add_bp(arch_addr_t addr, struct sw_singlestep_data *data)
{
	struct process_stopping_handler *self = data->self;
	struct process *proc = self->task_enabling_breakpoint;

	int ct = sizeof(self->sws_bps) / sizeof(*self->sws_bps);
	int i;
	for (i = 0; i < ct; ++i)
		if (self->sws_bps[i] == NULL) {
			static struct bp_callbacks cbs = {
				.on_hit = sw_singlestep_bp_on_hit,
			};
			struct breakpoint *bp
				= insert_breakpoint_at(proc, addr, NULL);
			breakpoint_set_callbacks(bp, &cbs);
			self->sws_bps[i] = bp;
			return 0;
		}

	assert(!"Too many sw singlestep breakpoints!");
	abort();
}

static int
singlestep(struct process_stopping_handler *self)
{
	size_t i;
	for (i = 0; i < sizeof(self->sws_bps) / sizeof(*self->sws_bps); ++i)
		self->sws_bps[i] = NULL;

	struct sw_singlestep_data data = { self };
	switch (arch_sw_singlestep(self->task_enabling_breakpoint,
				   self->breakpoint_being_enabled,
				   &sw_singlestep_add_bp, &data)) {
	case SWS_HW:
		/* Otherwise do the default action: singlestep.  */
		debug(1, "PTRACE_SINGLESTEP");
		if (ptrace(PTRACE_SINGLESTEP,
			   self->task_enabling_breakpoint->pid, 0, 0)) {
			perror("PTRACE_SINGLESTEP");
			return -1;
		}
		return 0;

	case SWS_OK:
		return 0;

	case SWS_FAIL:
		return -1;
	}
	abort();
}

static void
post_singlestep(struct process_stopping_handler *self,
		struct Event **eventp)
{
	continue_for_sigstop_delivery(&self->pids);

	if (*eventp != NULL && (*eventp)->type == EVENT_BREAKPOINT)
		*eventp = NULL; // handled

	struct process *proc = self->task_enabling_breakpoint;

	remove_sw_breakpoints(proc);
	self->breakpoint_being_enabled = NULL;
}

static void
singlestep_error(struct process_stopping_handler *self)
{
	struct process *teb = self->task_enabling_breakpoint;
	struct breakpoint *sbp = self->breakpoint_being_enabled;
	fprintf(stderr, "%d couldn't continue when handling %s (%p) at %p\n",
		teb->pid, breakpoint_name(sbp),	sbp->addr,
		get_instruction_pointer(teb));
	delete_breakpoint_at(teb->leader, sbp->addr);
}

static void
pt_continue(struct process_stopping_handler *self)
{
	struct process *teb = self->task_enabling_breakpoint;
	debug(1, "PTRACE_CONT");
	ptrace(PTRACE_CONT, teb->pid, 0, 0);
}

static void
pt_singlestep(struct process_stopping_handler *self)
{
	if (singlestep(self) < 0)
		singlestep_error(self);
}

static void
disable_and(struct process_stopping_handler *self,
	    void (*do_this)(struct process_stopping_handler *self))
{
	struct process *teb = self->task_enabling_breakpoint;
	debug(DEBUG_PROCESS, "all stopped, now singlestep/cont %d", teb->pid);
	if (self->breakpoint_being_enabled->enabled)
		disable_breakpoint(teb, self->breakpoint_being_enabled);
	(do_this)(self);
	self->state = PSH_SINGLESTEP;
}

void
linux_ptrace_disable_and_singlestep(struct process_stopping_handler *self)
{
	disable_and(self, &pt_singlestep);
}

void
linux_ptrace_disable_and_continue(struct process_stopping_handler *self)
{
	disable_and(self, &pt_continue);
}

/* This event handler is installed when we are in the process of
 * stopping the whole thread group to do the pointer re-enablement for
 * one of the threads.  We pump all events to the queue for later
 * processing while we wait for all the threads to stop.  When this
 * happens, we let the re-enablement thread to PTRACE_SINGLESTEP,
 * re-enable, and continue everyone.  */
static Event *
process_stopping_on_event(struct event_handler *super, Event *event)
{
	struct process_stopping_handler *self = (void *)super;
	struct process *task = event->proc;
	struct process *leader = task->leader;
	struct process *teb = self->task_enabling_breakpoint;

	debug(DEBUG_PROCESS,
	      "process_stopping_on_event: pid %d; event type %d; state %d",
	      task->pid, event->type, self->state);

	struct pid_task *task_info = get_task_info(&self->pids, task->pid);
	if (task_info == NULL)
		fprintf(stderr, "new task??? %d\n", task->pid);
	handle_stopping_event(task_info, &event);

	int state = self->state;
	int event_to_queue = !event_exit_or_none_p(event);

	/* Deactivate the entry if the task exits.  */
	if (event_exit_p(event) && task_info != NULL)
		task_info->pid = 0;

	/* Always handle sysrets.  Whether sysret occurred and what
	 * sys it rets from may need to be determined based on process
	 * stack, so we need to keep that in sync with reality.  Note
	 * that we don't continue the process after the sysret is
	 * handled.  See continue_after_syscall.  */
	if (event != NULL && event->type == EVENT_SYSRET) {
		debug(1, "%d LT_EV_SYSRET", event->proc->pid);
		event_to_queue = 0;
		if (task_info != NULL)
			task_info->sysret = 1;
	}

	switch (state) {
	case PSH_STOPPING:
		/* If everyone is stopped, singlestep.  */
		if (each_task(leader, NULL, &task_blocked,
			      &self->pids) == NULL) {
			(self->on_all_stopped)(self);
			state = self->state;
		}
		break;

	case PSH_SINGLESTEP:
		/* In singlestep state, breakpoint signifies that we
		 * have now stepped, and can re-enable the breakpoint.  */
		if (event != NULL && task == teb) {

			/* If this was caused by a real breakpoint, as
			 * opposed to a singlestep, assume that it's
			 * an artificial breakpoint installed for some
			 * reason for the re-enablement.  In that case
			 * handle it.  */
			if (event->type == EVENT_BREAKPOINT) {
				arch_addr_t ip
					= get_instruction_pointer(task);
				struct breakpoint *other
					= address2bpstruct(leader, ip);
				if (other != NULL)
					breakpoint_on_hit(other, task);
			}

			/* If we got SIGNAL instead of BREAKPOINT,
			 * then this is not singlestep at all.  */
			if (event->type == EVENT_SIGNAL) {
			do_singlestep:
				if (singlestep(self) < 0) {
					singlestep_error(self);
					post_singlestep(self, &event);
					goto psh_sinking;
				}
				break;
			} else {
				switch ((self->keep_stepping_p)(self)) {
				case CBS_FAIL:
					/* XXX handle me */
				case CBS_STOP:
					break;
				case CBS_CONT:
					/* Sink singlestep event.  */
					if (event->type == EVENT_BREAKPOINT)
						event = NULL;
					goto do_singlestep;
				}
			}

			/* Re-enable the breakpoint that we are
			 * stepping over.  */
			struct breakpoint *sbp = self->breakpoint_being_enabled;
			if (sbp->enabled)
				enable_breakpoint(teb, sbp);

			post_singlestep(self, &event);
			goto psh_sinking;
		}
		break;

	psh_sinking:
		state = self->state = PSH_SINKING;
		/* Fall through.  */
	case PSH_SINKING:
		if (await_sigstop_delivery(&self->pids, task_info, event))
			process_stopping_done(self, leader);
		break;

	case PSH_UGLY_WORKAROUND:
		if (event == NULL)
			break;
		if (event->type == EVENT_BREAKPOINT) {
			undo_breakpoint(event, leader);
			if (task == teb)
				self->task_enabling_breakpoint = NULL;
		}
		if (self->task_enabling_breakpoint == NULL
		    && all_stops_accountable(&self->pids)) {
			undo_breakpoint(event, leader);
			detach_process(leader);
			event = NULL; // handled
		}
	}

	if (event != NULL && event_to_queue) {
		enque_event(event);
		event = NULL; // sink the event
	}

	return event;
}

static void
process_stopping_destroy(struct event_handler *super)
{
	struct process_stopping_handler *self = (void *)super;
	free(self->pids.tasks);
}

static enum callback_status
no(struct process_stopping_handler *self)
{
	return CBS_STOP;
}

int
process_install_stopping_handler(struct process *proc, struct breakpoint *sbp,
				 void (*as)(struct process_stopping_handler *),
				 enum callback_status (*ks)
					 (struct process_stopping_handler *),
				 enum callback_status (*uw)
					(struct process_stopping_handler *))
{
	debug(DEBUG_FUNCTION,
	      "process_install_stopping_handler: pid=%d", proc->pid);

	struct process_stopping_handler *handler = calloc(sizeof(*handler), 1);
	if (handler == NULL)
		return -1;

	if (as == NULL)
		as = &linux_ptrace_disable_and_singlestep;
	if (ks == NULL)
		ks = &no;
	if (uw == NULL)
		uw = &no;

	handler->super.on_event = process_stopping_on_event;
	handler->super.destroy = process_stopping_destroy;
	handler->task_enabling_breakpoint = proc;
	handler->breakpoint_being_enabled = sbp;
	handler->on_all_stopped = as;
	handler->keep_stepping_p = ks;
	handler->ugly_workaround_p = uw;

	install_event_handler(proc->leader, &handler->super);

	if (each_task(proc->leader, NULL, &send_sigstop,
		      &handler->pids) != NULL) {
		destroy_event_handler(proc);
		return -1;
	}

	/* And deliver the first fake event, in case all the
	 * conditions are already fulfilled.  */
	Event ev = {
		.type = EVENT_NONE,
		.proc = proc,
	};
	process_stopping_on_event(&handler->super, &ev);

	return 0;
}

void
continue_after_breakpoint(struct process *proc, struct breakpoint *sbp)
{
	debug(DEBUG_PROCESS,
	      "continue_after_breakpoint: pid=%d, addr=%p",
	      proc->pid, sbp->addr);

	set_instruction_pointer(proc, sbp->addr);

	if (sbp->enabled == 0) {
		continue_process(proc->pid);
	} else if (process_install_stopping_handler
			(proc, sbp, NULL, NULL, NULL) < 0) {
		perror("process_stopping_handler_create");
		/* Carry on not bothering to re-enable.  */
		continue_process(proc->pid);
	}
}

/**
 * Ltrace exit.  When we are about to exit, we have to go through all
 * the processes, stop them all, remove all the breakpoints, and then
 * detach the processes that we attached to using -p.  If we left the
 * other tasks running, they might hit stray return breakpoints and
 * produce artifacts, so we better stop everyone, even if it's a bit
 * of extra work.
 */
struct ltrace_exiting_handler
{
	struct event_handler super;
	struct pid_set pids;
};

static Event *
ltrace_exiting_on_event(struct event_handler *super, Event *event)
{
	struct ltrace_exiting_handler *self = (void *)super;
	struct process *task = event->proc;
	struct process *leader = task->leader;

	debug(DEBUG_PROCESS,
	      "ltrace_exiting_on_event: pid %d; event type %d",
	      task->pid, event->type);

	struct pid_task *task_info = get_task_info(&self->pids, task->pid);
	handle_stopping_event(task_info, &event);

	if (event != NULL && event->type == EVENT_BREAKPOINT)
		undo_breakpoint(event, leader);

	if (await_sigstop_delivery(&self->pids, task_info, event)
	    && all_stops_accountable(&self->pids))
		detach_process(leader);

	/* Sink all non-exit events.  We are about to exit, so we
	 * don't bother with queuing them. */
	if (event_exit_or_none_p(event))
		return event;

	return NULL;
}

static void
ltrace_exiting_destroy(struct event_handler *super)
{
	struct ltrace_exiting_handler *self = (void *)super;
	free(self->pids.tasks);
}

static int
ltrace_exiting_install_handler(struct process *proc)
{
	/* Only install to leader.  */
	if (proc->leader != proc)
		return 0;

	/* Perhaps we are already installed, if the user passed
	 * several -p options that are tasks of one process.  */
	if (proc->event_handler != NULL
	    && proc->event_handler->on_event == &ltrace_exiting_on_event)
		return 0;

	/* If stopping handler is already present, let it do the
	 * work.  */
	if (proc->event_handler != NULL) {
		assert(proc->event_handler->on_event
		       == &process_stopping_on_event);
		struct process_stopping_handler *other
			= (void *)proc->event_handler;
		other->exiting = 1;
		return 0;
	}

	struct ltrace_exiting_handler *handler
		= calloc(sizeof(*handler), 1);
	if (handler == NULL) {
		perror("malloc exiting handler");
	fatal:
		/* XXXXXXXXXXXXXXXXXXX fixme */
		return -1;
	}

	handler->super.on_event = ltrace_exiting_on_event;
	handler->super.destroy = ltrace_exiting_destroy;
	install_event_handler(proc->leader, &handler->super);

	if (each_task(proc->leader, NULL, &send_sigstop,
		      &handler->pids) != NULL)
		goto fatal;

	return 0;
}

/*
 * When the traced process vforks, it's suspended until the child
 * process calls _exit or exec*.  In the meantime, the two share the
 * address space.
 *
 * The child process should only ever call _exit or exec*, but we
 * can't count on that (it's not the role of ltrace to policy, but to
 * observe).  In any case, we will _at least_ have to deal with
 * removal of vfork return breakpoint (which we have to smuggle back
 * in, so that the parent can see it, too), and introduction of exec*
 * return breakpoint.  Since we already have both breakpoint actions
 * to deal with, we might as well support it all.
 *
 * The gist is that we pretend that the child is in a thread group
 * with its parent, and handle it as a multi-threaded case, with the
 * exception that we know that the parent is blocked, and don't
 * attempt to stop it.  When the child execs, we undo the setup.
 */

struct process_vfork_handler
{
	struct event_handler super;
	int vfork_bp_refd:1;
};

static Event *
process_vfork_on_event(struct event_handler *super, Event *event)
{
	debug(DEBUG_PROCESS,
	      "process_vfork_on_event: pid %d; event type %d",
	      event->proc->pid, event->type);

	struct process_vfork_handler *self = (void *)super;
	struct process *proc = event->proc;
	assert(self != NULL);

	switch (event->type) {
	case EVENT_BREAKPOINT:
		/* We turn on the vfork return breakpoint (which
		 * should be the one that we have tripped over just
		 * now) one extra time, so that the vfork parent hits
		 * it as well.  */
		if (!self->vfork_bp_refd) {
			struct breakpoint *sbp = NULL;
			DICT_FIND_VAL(proc->leader->breakpoints,
				      &event->e_un.brk_addr, &sbp);
			assert(sbp != NULL);
			breakpoint_turn_on(sbp, proc->leader);
			self->vfork_bp_refd = 1;
		}
		break;

	case EVENT_EXIT:
	case EVENT_EXIT_SIGNAL:
	case EVENT_EXEC:
		/* Remove the leader that we artificially set up
		 * earlier.  */
		change_process_leader(proc, proc);
		destroy_event_handler(proc);
		continue_process(proc->parent->pid);

	default:
		;
	}

	return event;
}

void
continue_after_vfork(struct process *proc)
{
	debug(DEBUG_PROCESS, "continue_after_vfork: pid=%d", proc->pid);
	struct process_vfork_handler *handler = calloc(sizeof(*handler), 1);
	if (handler == NULL) {
		perror("malloc vfork handler");
		/* Carry on not bothering to treat the process as
		 * necessary.  */
		continue_process(proc->parent->pid);
		return;
	}

	/* We must set up custom event handler, so that we see
	 * exec/exit events for the task itself.  */
	handler->super.on_event = process_vfork_on_event;
	install_event_handler(proc, &handler->super);

	/* Make sure that the child is sole thread.  */
	assert(proc->leader == proc);
	assert(proc->next == NULL || proc->next->leader != proc);

	/* Make sure that the child's parent is properly set up.  */
	assert(proc->parent != NULL);
	assert(proc->parent->leader != NULL);

	change_process_leader(proc, proc->parent->leader);
}

static int
is_mid_stopping(struct process *proc)
{
	return proc != NULL
		&& proc->event_handler != NULL
		&& proc->event_handler->on_event == &process_stopping_on_event;
}

void
continue_after_syscall(struct process *proc, int sysnum, int ret_p)
{
	/* Don't continue if we are mid-stopping.  */
	if (ret_p && (is_mid_stopping(proc) || is_mid_stopping(proc->leader))) {
		debug(DEBUG_PROCESS,
		      "continue_after_syscall: don't continue %d",
		      proc->pid);
		return;
	}
	continue_process(proc->pid);
}

void
continue_after_exec(struct process *proc)
{
	continue_process(proc->pid);

	/* After the exec, we expect to hit the first executable
	 * instruction.
	 *
	 * XXX TODO It would be nice to have this removed, but then we
	 * need to do that also for initial call to wait_for_proc in
	 * execute_program.  In that case we could generate a
	 * EVENT_FIRST event or something, or maybe this could somehow
	 * be rolled into EVENT_NEW.  */
	wait_for_proc(proc->pid);
	continue_process(proc->pid);
}

/* If ltrace gets SIGINT, the processes directly or indirectly run by
 * ltrace get it too.  We just have to wait long enough for the signal
 * to be delivered and the process terminated, which we notice and
 * exit ltrace, too.  So there's not much we need to do there.  We
 * want to keep tracing those processes as usual, in case they just
 * SIG_IGN the SIGINT to do their shutdown etc.
 *
 * For processes ran on the background, we want to install an exit
 * handler that stops all the threads, removes all breakpoints, and
 * detaches.
 */
void
os_ltrace_exiting(void)
{
	struct opt_p_t *it;
	for (it = opt_p; it != NULL; it = it->next) {
		struct process *proc = pid2proc(it->pid);
		if (proc == NULL || proc->leader == NULL)
			continue;
		if (ltrace_exiting_install_handler(proc->leader) < 0)
			fprintf(stderr,
				"Couldn't install exiting handler for %d.\n",
				proc->pid);
	}
}

int
os_ltrace_exiting_sighandler(void)
{
	extern int linux_in_waitpid;
	if (linux_in_waitpid) {
		os_ltrace_exiting();
		return 1;
	}
	return 0;
}

size_t
umovebytes(struct process *proc, arch_addr_t addr, void *buf, size_t len)
{

	union {
		long a;
		char c[sizeof(long)];
	} a;
	int started = 0;
	size_t offset = 0, bytes_read = 0;

	while (offset < len) {
		a.a = ptrace(PTRACE_PEEKTEXT, proc->pid, addr + offset, 0);
		if (a.a == -1 && errno) {
			if (started && errno == EIO)
				return bytes_read;
			else
				return -1;
		}
		started = 1;

		if (len - offset >= sizeof(long)) {
			memcpy(buf + offset, &a.c[0], sizeof(long));
			bytes_read += sizeof(long);
		}
		else {
			memcpy(buf + offset, &a.c[0], len - offset);
			bytes_read += (len - offset);
		}
		offset += sizeof(long);
	}

	return bytes_read;
}

struct irelative_name_data_t {
	GElf_Addr addr;
	const char *found_name;
};

static enum callback_status
irelative_name_cb(GElf_Sym *symbol, const char *name, void *d)
{
	struct irelative_name_data_t *data = d;

	if (symbol->st_value == data->addr) {
		bool is_ifunc = false;
#ifdef STT_GNU_IFUNC
		is_ifunc = GELF_ST_TYPE(symbol->st_info) == STT_GNU_IFUNC;
#endif
		data->found_name = name;

		/* Keep looking, unless we found the actual IFUNC
		 * symbol.  What we matched may have been a symbol
		 * denoting the resolver function, which would have
		 * the same address.  */
		return CBS_STOP_IF(is_ifunc);
	}

	return CBS_CONT;
}

char *
linux_elf_find_irelative_name(struct ltelf *lte, GElf_Addr addr)
{
	struct irelative_name_data_t data = { addr, NULL };
	if (addr != 0
	    && elf_each_symbol(lte, 0,
			       irelative_name_cb, &data).status < 0)
		return NULL;

	const char *name;
	if (data.found_name != NULL) {
		name = data.found_name;
	} else {
#define NAME "IREL."
		/* NAME\0 + 0x + digits.  */
		char *tmp_name = alloca(sizeof NAME + 2 + 16);
		sprintf(tmp_name, NAME "%#" PRIx64, (uint64_t) addr);
		name = tmp_name;
#undef NAME
	}

	return strdup(name);
}

enum plt_status
linux_elf_add_plt_entry_irelative(struct process *proc, struct ltelf *lte,
				  GElf_Rela *rela, size_t ndx,
				  struct library_symbol **ret)

{
	char *name = linux_elf_find_irelative_name(lte, rela->r_addend);
	int i = default_elf_add_plt_entry(proc, lte, name, rela, ndx, ret);
	free(name);
	return i < 0 ? PLT_FAIL : PLT_OK;
}

struct prototype *
linux_IFUNC_prototype(void)
{
	static struct prototype ret;
	if (ret.return_info == NULL) {
		prototype_init(&ret);
		ret.return_info = type_get_voidptr();
		ret.own_return_info = 0;
	}
	return &ret;
}

int
os_library_symbol_init(struct library_symbol *libsym)
{
	libsym->os = (struct os_library_symbol_data){};
	return 0;
}

void
os_library_symbol_destroy(struct library_symbol *libsym)
{
}

int
os_library_symbol_clone(struct library_symbol *retp,
			struct library_symbol *libsym)
{
	retp->os = libsym->os;
	return 0;
}

char *
linux_append_IFUNC_to_name(const char *name)
{
#define S ".IFUNC"
	char *tmp_name = malloc(strlen(name) + sizeof S);
	if (tmp_name == NULL)
		return NULL;
	sprintf(tmp_name, "%s%s", name, S);
#undef S
	return tmp_name;
}

enum plt_status
os_elf_add_func_entry(struct process *proc, struct ltelf *lte,
		      const GElf_Sym *sym,
		      arch_addr_t addr, const char *name,
		      struct library_symbol **ret)
{
	if (GELF_ST_TYPE(sym->st_info) == STT_FUNC)
		return PLT_DEFAULT;

	bool ifunc = false;
#ifdef STT_GNU_IFUNC
	ifunc = GELF_ST_TYPE(sym->st_info) == STT_GNU_IFUNC;
#endif

	if (ifunc) {
		char *tmp_name = linux_append_IFUNC_to_name(name);
		struct library_symbol *tmp = malloc(sizeof *tmp);
		if (tmp_name == NULL || tmp == NULL) {
		fail:
			free(tmp_name);
			free(tmp);
			return PLT_FAIL;
		}

		if (library_symbol_init(tmp, addr, tmp_name, 1,
					LS_TOPLT_NONE) < 0)
			goto fail;
		tmp->proto = linux_IFUNC_prototype();
		tmp->os.is_ifunc = 1;

		*ret = tmp;
		return PLT_OK;
	}

	*ret = NULL;
	return PLT_OK;
}

static enum callback_status
libsym_at_address(struct library_symbol *libsym, void *addrp)
{
	arch_addr_t addr = *(arch_addr_t *)addrp;
	return CBS_STOP_IF(addr == libsym->enter_addr);
}

static void
ifunc_ret_hit(struct breakpoint *bp, struct process *proc)
{
	struct fetch_context *fetch = fetch_arg_init(LT_TOF_FUNCTION, proc,
						     type_get_voidptr());
	if (fetch == NULL)
		return;

	struct breakpoint *nbp = NULL;
	int own_libsym = 0;
	struct library_symbol *libsym = NULL;

	struct value value;
	value_init(&value, proc, NULL, type_get_voidptr(), 0);
	size_t sz = value_size(&value, NULL);
	union {
		uint64_t u64;
		uint32_t u32;
		arch_addr_t a;
	} u;

	if (fetch_retval(fetch, LT_TOF_FUNCTIONR, proc,
			 value.type, &value) < 0
	    || sz > 8 /* Captures failure as well.  */
	    || value_extract_buf(&value, (void *) &u, NULL) < 0) {
	fail:
		fprintf(stderr,
			"Couldn't trace the function "
			"indicated by IFUNC resolver.\n");
		goto done;
	}

	assert(sz == 4 || sz == 8);
	/* XXX double casts below:  */
	if (sz == 4)
		u.a = (arch_addr_t)(uintptr_t)u.u32;
	else
		u.a = (arch_addr_t)(uintptr_t)u.u64;
	if (arch_translate_address_dyn(proc, u.a, &u.a) < 0) {
		fprintf(stderr, "Couldn't OPD-translate the address returned"
			" by the IFUNC resolver.\n");
		goto done;
	}

	assert(bp->os.ret_libsym != NULL);

	struct library *lib = bp->os.ret_libsym->lib;
	assert(lib != NULL);

	/* Look if we already have a symbol with this address.
	 * Otherwise create a new one.  */
	libsym = library_each_symbol(lib, NULL, libsym_at_address, &u.a);
	if (libsym == NULL) {
		libsym = malloc(sizeof *libsym);
		char *name = strdup(bp->os.ret_libsym->name);

		if (libsym == NULL
		    || name == NULL
		    || library_symbol_init(libsym, u.a, name, 1,
					   LS_TOPLT_NONE) < 0) {
			free(libsym);
			free(name);
			goto fail;
		}

		/* Snip the .IFUNC token.  */
		*strrchr(name, '.') = 0;

		own_libsym = 1;
		library_add_symbol(lib, libsym);
	}

	nbp = malloc(sizeof *bp);
	if (nbp == NULL || breakpoint_init(nbp, proc, u.a, libsym) < 0)
		goto fail;

	/* If there already is a breakpoint at that address, that is
	 * suspicious, but whatever.  */
	struct breakpoint *pre_bp = insert_breakpoint(proc, nbp);
	if (pre_bp == NULL)
		goto fail;
	if (pre_bp == nbp) {
		/* PROC took our breakpoint, so these resources are
		 * not ours anymore.  */
		nbp = NULL;
		own_libsym = 0;
	}

done:
	free(nbp);
	if (own_libsym) {
		library_symbol_destroy(libsym);
		free(libsym);
	}
	fetch_arg_done(fetch);
}

static int
create_ifunc_ret_bp(struct breakpoint **ret,
		    struct breakpoint *bp, struct process *proc)
{
	*ret = create_default_return_bp(proc);
	if (*ret == NULL)
		return -1;
	static struct bp_callbacks cbs = {
		.on_hit = ifunc_ret_hit,
	};
	breakpoint_set_callbacks(*ret, &cbs);

	(*ret)->os.ret_libsym = bp->libsym;

	return 0;
}

int
os_breakpoint_init(struct process *proc, struct breakpoint *bp)
{
	if (bp->libsym != NULL && bp->libsym->os.is_ifunc) {
		static struct bp_callbacks cbs = {
			.get_return_bp = create_ifunc_ret_bp,
		};
		breakpoint_set_callbacks(bp, &cbs);
	}
	return 0;
}

void
os_breakpoint_destroy(struct breakpoint *bp)
{
}

int
os_breakpoint_clone(struct breakpoint *retp, struct breakpoint *bp)
{
	return 0;
}
