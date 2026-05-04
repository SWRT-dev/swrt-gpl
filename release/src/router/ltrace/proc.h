/*
 * This file is part of ltrace.
 * Copyright (C) 2010,2011,2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Joe Damato
 * Copyright (C) 1998,2001,2008,2009 Juan Cespedes
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

#ifndef PROC_H
#define PROC_H

#include "config.h"

#include <sys/time.h>
#include <stdint.h>

#if defined(HAVE_LIBDW)
# include <elfutils/libdwfl.h>
#endif

#if defined(HAVE_LIBUNWIND)
# include <libunwind.h>
# include <libunwind-ptrace.h>
#endif /* defined(HAVE_LIBUNWIND) */

#include "ltrace.h"
#include "dict.h"
#include "sysdep.h"
#include "callback.h"
#include "forward.h"

struct event_handler {
	/* Event handler that overrides the default one.  Should
	 * return NULL if the event was handled, otherwise the
	 * returned event is passed to the default handler.  */
	Event *(*on_event)(struct event_handler *self, Event *event);

	/* Called when the event handler removal is requested.  */
	void (*destroy)(struct event_handler *self);
};

enum process_state {
	STATE_ATTACHED = 0,
	STATE_BEING_CREATED,
	STATE_IGNORED  /* ignore this process (it's a fork and no -f was used) */
};

struct output_state {
	size_t params_left;
	int need_delim;
};

struct callstack_element {
	union {
		int syscall;
		struct library_symbol * libfunc;
	} c_un;
	int is_syscall;
	arch_addr_t return_addr;
	struct timeval enter_time;
	struct fetch_context *fetch_context;
	struct value_dict *arguments;
	struct output_state out;
};

/* XXX We should get rid of this.  */
#define MAX_CALLDEPTH 64

/* XXX We would rather have this all organized a little differently,
 * have struct process for the whole group and struct task (or struct
 * lwp, struct thread) for what's there for per-thread stuff.  But for
 * now this is the less invasive way of structuring it.  */
struct process {
	enum process_state state;
	struct process *parent;         /* needed by STATE_BEING_CREATED */
	char * filename;
	pid_t pid;

	/* Dictionary of breakpoints (which is a mapping
	 * address->breakpoint).  This is NULL for non-leader
	 * processes.  */
	struct dict *breakpoints;

	int mask_32bit;           /* 1 if 64-bit ltrace is tracing 32-bit process */
	unsigned int personality;
	int tracesysgood;         /* signal indicating a PTRACE_SYSCALL trap */

	size_t callstack_depth;
	struct callstack_element callstack[MAX_CALLDEPTH];

	/* Linked list of libraries in backwards order of mapping.
	 * The last element is the executed binary itself.  */
	struct library *libraries;

	/* Arch-dependent: */
	void * instruction_pointer;
	void * stack_pointer;      /* To get return addr, args... */
	void * arch_ptr;

	/* XXX We would like to replace this with a pointer to ABI
	 * object that would provide the relevant services, instead of
	 * checking the necessary flags in the back end ad
	 * nauseam.  */
	short e_machine;
	char e_class;

#if defined(HAVE_LIBDW)
	/* Unwind info for leader, NULL for non-leader procs. */
	Dwfl *dwfl;

	/* Whether we still need to attach the DWARF library to this process. We
	 * try only once, and never again, regardless of whether we succeeded or
	 * not. 0 = shouldn't attach */
	int should_attach_dwfl;

#endif /* defined(HAVE_LIBDW) */

#if defined(HAVE_LIBUNWIND)
	/* libunwind address space */
	unw_addr_space_t unwind_as;
	void *unwind_priv;
#endif /* defined(HAVE_LIBUNWIND) */

	/* Set in leader.  */
	struct event_handler *event_handler;

	/**
	 * Process chaining.
	 **/
	struct process *next;

	/* LEADER points to the leader thread of the POSIX.1 process.
	   If X->LEADER == X, then X is the leader thread and the
	   process structures chained by NEXT represent other threads,
	   up until, but not including, the next leader thread.
	   LEADER may be NULL after the leader has already exited.  In
	   that case this process is waiting to be collected.  */
	struct process *leader;

	struct os_process_data os;
	struct arch_process_data arch;
};

/* Initialize a process given a path to binary FILENAME, with a PID,
 * and add the process to an internal chain of traced processes.  */
int process_init(struct process *proc, const char *filename, pid_t pid);

/* PROC underwent an exec.  This is a bit like process_destroy
 * followed by process_init, except that some state is kept and the
 * process doesn't lose it's place in the list of processes.  */
int process_exec(struct process *proc);

/* Release any memory allocated for PROC (but not PROC itself).  Does
 * NOT remove PROC from internal chain.
 *
 * XXX clearly this init/destroy pair is different than others and
 * should be fixed.  process_init should presumably be separate from
 * process_add.  */
void process_destroy(struct process *proc);

struct process *open_program(const char *filename, pid_t pid);
void open_pid(pid_t pid);
struct process *pid2proc(pid_t pid);

/* Clone the contents of PROC into the memory referenced by RETP.
 * Returns 0 on success or a negative value on failure.  */
int process_clone(struct process *retp, struct process *proc, pid_t pid);

/* Iterate through the processes that ltrace currently traces.  Tasks
 * are considered to be processes for the purpose of this iterator.
 * See callback.h for notes on iteration interfaces.  */
struct process *each_process(struct process *start_after,
			     enum callback_status (*cb)(struct process *proc,
							void *data),
			     void *data);

/* Iterate through list of tasks of given process PROC.  See
 * callback.h for notes on iteration interfaces.  */
struct process *each_task(struct process *proc, struct process *start_after,
			  enum callback_status (*cb)(struct process *proc,
						     void *data),
			  void *data);

void change_process_leader(struct process *proc, struct process *leader);

/* Prepare those parts of process initialization that need to be done
 * after _start is hit (i.e. after dynamic linking was done).  */
void process_hit_start(struct process *proc);

/* Remove process from the list of traced processes, drop any events
 * in the event queue, destroy it and free memory.  */
void remove_process(struct process *proc);

void install_event_handler(struct process *proc, struct event_handler *handler);
void destroy_event_handler(struct process *proc);

/* Add a library LIB to the list of PROC's libraries.  */
void proc_add_library(struct process *proc, struct library *lib);

/* Remove LIB from list of PROC's libraries.  Returns 0 if the library
 * was found and unlinked, otherwise returns a negative value.  */
int proc_remove_library(struct process *proc, struct library *lib);

/* Clear a delayed flag.  If a symbol is neither latent, nor delayed,
 * a breakpoint is inserted for it.  Returns 0 if the activation was
 * successful or a negative value if it failed.  Note that if a symbol
 * is both latent and delayed, this will not enable the corresponding
 * breakpoint.  */
int proc_activate_delayed_symbol(struct process *proc,
				 struct library_symbol *libsym);

/* Iterate through the libraries of PROC.  See callback.h for notes on
 * iteration interfaces.  */
struct library *proc_each_library(struct process *proc,
				  struct library *start_after,
				  enum callback_status (*cb)(struct process *p,
							     struct library *l,
							     void *data),
				  void *data);

/* Insert BP into PROC.  */
int proc_add_breakpoint(struct process *proc, struct breakpoint *bp);

/* Remove BP from PROC.  This has no reason to fail in runtime.  If it
 * does not find BP in PROC, it's hard error guarded by assertion.  */
void proc_remove_breakpoint(struct process *proc, struct breakpoint *bp);

/* Iterate through the breakpoints of PROC.  See callback.h for notes
 * on iteration interfaces.  */
arch_addr_t *proc_each_breakpoint(struct process *proc, arch_addr_t *start,
				  enum callback_status (*cb)
					(struct process *proc,
					 struct breakpoint *bp,
					 void *data),
				  void *data);

/* Iterate through the dynamic section at src_addr looking for D_TAG.
 * If tag is found, fill it's value in RET and return 0.
 * If tag is not found, return a negative value.  */
int proc_find_dynamic_entry_addr(struct process *proc, arch_addr_t src_addr,
				 int d_tag, arch_addr_t *ret);

/* Finds a symbol corresponding to LIBSYM in a process PROC.  Returns
 * 0 and sets *RETLIB and *RETSYM if the corresponding pointer is
 * non-NULL.  Returns a negative value when the symbols couldn't be
 * found.  */
int proc_find_symbol(struct process *proc, struct library_symbol *sym,
		     struct library **retlib, struct library_symbol **retsym);

/* Iterate through all symbols in all libraries of PROC.  See
 * callback.h for notes on this interface.  */
struct library_symbol *proc_each_symbol
	(struct process *proc, struct library_symbol *start_after,
	 enum callback_status (*cb)(struct library_symbol *, void *),
	 void *data);

/* Read 8, 16, 32 or 64-bit quantity located at ADDR in PROC.  The
 * resulting value is stored in *LP.  0 is returned on success or a
 * negative value on failure.  This uses umovebytes under the hood
 * (see backend.h).  */
int proc_read_8(struct process *proc, arch_addr_t addr, uint8_t *lp);
int proc_read_16(struct process *proc, arch_addr_t addr, uint16_t *lp);
int proc_read_32(struct process *proc, arch_addr_t addr, uint32_t *lp);
int proc_read_64(struct process *proc, arch_addr_t addr, uint64_t *lp);

#endif /* PROC_H */
