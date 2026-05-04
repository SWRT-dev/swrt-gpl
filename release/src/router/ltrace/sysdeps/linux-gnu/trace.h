/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
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

#ifndef LTRACE_LINUX_TRACE_H
#define LTRACE_LINUX_TRACE_H

#include "proc.h"

/* This publishes some Linux-specific data structures used for process
 * handling.  */

/**
 * This is used for bookkeeping related to PIDs that the event
 * handlers work with.
 */
struct pid_task {
	pid_t pid;	/* This may be 0 for tasks that exited
			 * mid-handling.  */
	int sigstopped : 1;
	int got_event : 1;
	int delivered : 1;
	int vforked : 1;
	int sysret : 1;
};

struct pid_set {
	struct pid_task *tasks;
	size_t count;
	size_t alloc;
};

/**
 * Breakpoint re-enablement.  When we hit a breakpoint, we must
 * disable it, single-step, and re-enable it.  That single-step can be
 * done only by one task in a task group, while others are stopped,
 * otherwise the processes would race for who sees the breakpoint
 * disabled and who doesn't.  The following is to keep track of it
 * all.
 */
struct process_stopping_handler
{
	struct event_handler super;

	/* The task that is doing the re-enablement.  */
	struct process *task_enabling_breakpoint;

	/* The pointer being re-enabled.  */
	struct breakpoint *breakpoint_being_enabled;

	/* Software singlestep breakpoints, if any needed.  */
	struct breakpoint *sws_bps[2];

	/* When all tasks are stopped, this callback gets called.  */
	void (*on_all_stopped)(struct process_stopping_handler *);

	/* When we get a singlestep event, this is called to decide
	 * whether to stop stepping, or whether to enable the
	 * brakpoint, sink remaining signals, and continue
	 * everyone.  */
	enum callback_status (*keep_stepping_p)
		(struct process_stopping_handler *);

	/* Whether we need to use ugly workaround to get around
	 * various problems with singlestepping.  */
	enum callback_status (*ugly_workaround_p)
		(struct process_stopping_handler *);

	enum {
		/* We are waiting for everyone to land in t/T.  */
		PSH_STOPPING = 0,

		/* We are doing the PTRACE_SINGLESTEP.  */
		PSH_SINGLESTEP,

		/* We are waiting for all the SIGSTOPs to arrive so
		 * that we can sink them.  */
		PSH_SINKING,

		/* This is for tracking the ugly workaround.  */
		PSH_UGLY_WORKAROUND,
	} state;

	int exiting;

	struct pid_set pids;
};

/* Allocate a process stopping handler, initialize it and install it.
 * Return 0 on success or a negative value on failure.  Pass NULL for
 * each callback to use a default instead.  The default for
 * ON_ALL_STOPPED is LINUX_PTRACE_DISABLE_AND_SINGLESTEP, the default
 * for KEEP_STEPPING_P and UGLY_WORKAROUND_P is "no".  */
int process_install_stopping_handler
	(struct process *proc, struct breakpoint *sbp,
	 void (*on_all_stopped)(struct process_stopping_handler *),
	 enum callback_status (*keep_stepping_p)
		 (struct process_stopping_handler *),
	 enum callback_status (*ugly_workaround_p)
		(struct process_stopping_handler *));

void linux_ptrace_disable_and_singlestep(struct process_stopping_handler *self);
void linux_ptrace_disable_and_continue(struct process_stopping_handler *self);

/* When main binary needs to call an IFUNC function defined in the
 * binary itself, a PLT entry is set up so that dynamic linker can get
 * involved and resolve the symbol.  But unlike other PLT relocation,
 * this one can't rely on symbol table being available.  So it doesn't
 * reference the symbol by its name, but by its address, and
 * correspondingly, has another type.  When arch backend wishes to
 * support these IRELATIVE relocations, it should override
 * arch_elf_add_plt_entry and dispatch to this function for IRELATIVE
 * relocations.
 *
 * This function behaves as arch_elf_add_plt_entry, except that it
 * doesn't take name for a parameter, but instead looks up the name in
 * symbol tables in LTE.  */
enum plt_status linux_elf_add_plt_entry_irelative(struct process *proc,
						  struct ltelf *lte,
						  GElf_Rela *rela, size_t ndx,
						  struct library_symbol **ret);

/* Service routine of the above.  Determines a name corresponding to
 * ADDR, or invents a new one.  Returns NULL on failures, otherwise it
 * returns a malloc'd pointer that the caller is responsible for
 * freeing.  */
char *linux_elf_find_irelative_name(struct ltelf *lte, GElf_Addr addr);

/* Returns ${NAME}.IFUNC in a newly-malloc'd block, or NULL on
 * failures.  */
char *linux_append_IFUNC_to_name(const char *name);

/* Returns a statically allocated prototype that represents the
 * prototype "void *()".  Never fails.  */
struct prototype *linux_IFUNC_prototype(void);


#endif /* LTRACE_LINUX_TRACE_H */
