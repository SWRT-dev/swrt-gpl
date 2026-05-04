/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Zachary T Welch, CodeSourcery
 * Copyright (C) 2010 Joe Damato
 * Copyright (C) 1998,2008,2009 Juan Cespedes
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

#define _GNU_SOURCE /* For getline.  */
#include "config.h"

#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <link.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "breakpoint.h"
#include "config.h"
#include "debug.h"
#include "events.h"
#include "library.h"
#include "ltrace-elf.h"
#include "proc.h"

/* /proc/pid doesn't exist just after the fork, and sometimes `ltrace'
 * couldn't open it to find the executable.  So it may be necessary to
 * have a bit delay
 */

#define	MAX_DELAY	100000	/* 100000 microseconds = 0.1 seconds */

#define PROC_PID_FILE(VAR, FORMAT, PID)		\
	char VAR[strlen(FORMAT) + 6];		\
	sprintf(VAR, FORMAT, PID)

/*
 * Returns a (malloc'd) file name corresponding to a running pid
 */
char *
pid2name(pid_t pid) {
	if (!kill(pid, 0)) {
		int delay = 0;

		PROC_PID_FILE(proc_exe, "/proc/%d/exe", pid);

		while (delay < MAX_DELAY) {
			if (!access(proc_exe, F_OK)) {
				return strdup(proc_exe);
			}
			delay += 1000;	/* 1 milisecond */
		}
	}
	return NULL;
}

static FILE *
open_status_file(pid_t pid)
{
	PROC_PID_FILE(fn, "/proc/%d/status", pid);
	/* Don't complain if we fail.  This would typically happen
	   when the process is about to terminate, and these files are
	   not available anymore.  This function is called from the
	   event loop, and we don't want to clutter the output just
	   because the process terminates.  */
	return fopen(fn, "r");
}

static char *
find_line_starting(FILE * file, const char * prefix, size_t len)
{
	char * line = NULL;
	size_t line_len = 0;
	while (!feof(file)) {
		if (getline(&line, &line_len, file) < 0)
			return NULL;
		if (strncmp(line, prefix, len) == 0)
			return line;
	}
	return NULL;
}

static void
each_line_starting(FILE *file, const char *prefix,
		   enum callback_status (*cb)(const char *line,
					      const char *prefix,
					      void *data),
		   void *data)
{
	size_t len = strlen(prefix);
	char * line;
	while ((line = find_line_starting(file, prefix, len)) != NULL) {
		enum callback_status st = (*cb)(line, prefix, data);
		free(line);
		if (st == CBS_STOP)
			return;
	}
}

static enum callback_status
process_leader_cb(const char *line, const char *prefix, void *data)
{
	pid_t * pidp = data;
	*pidp = atoi(line + strlen(prefix));
	return CBS_STOP;
}

pid_t
process_leader(pid_t pid)
{
	pid_t tgid = 0;
	FILE * file = open_status_file(pid);
	if (file != NULL) {
		each_line_starting(file, "Tgid:\t", &process_leader_cb, &tgid);
		fclose(file);
	}

	return tgid;
}

static enum callback_status
process_stopped_cb(const char *line, const char *prefix, void *data)
{
	char c = line[strlen(prefix)];
	// t:tracing stop, T:job control stop
	*(int *)data = (c == 't' || c == 'T');
	return CBS_STOP;
}

int
process_stopped(pid_t pid)
{
	int is_stopped = -1;
	FILE * file = open_status_file(pid);
	if (file != NULL) {
		each_line_starting(file, "State:\t", &process_stopped_cb,
				   &is_stopped);
		fclose(file);
	}
	return is_stopped;
}

static enum callback_status
process_status_cb(const char *line, const char *prefix, void *data)
{
	const char * status = line + strlen(prefix);
	const char c = *status;

#define RETURN(C) do {					\
		*(enum process_status *)data = C;	\
		return CBS_STOP;			\
	} while (0)

	switch (c) {
	case 'Z': RETURN(PS_ZOMBIE);
	case 't': RETURN(PS_TRACING_STOP);
	case 'T':
		/* This can be either "T (stopped)" or, for older
		 * kernels, "T (tracing stop)".  */
		if (!strcmp(status, "T (stopped)\n"))
			RETURN(PS_STOP);
		else if (!strcmp(status, "T (tracing stop)\n"))
			RETURN(PS_TRACING_STOP);
		else {
			fprintf(stderr, "Unknown process status: %s",
				status);
			RETURN(PS_STOP); /* Some sort of stop
					  * anyway.  */
		}
	case 'D':
	case 'S': RETURN(PS_SLEEPING);
	}

	RETURN(PS_OTHER);
#undef RETURN
}

enum process_status
process_status(pid_t pid)
{
	enum process_status ret = PS_INVALID;
	FILE * file = open_status_file(pid);
	if (file != NULL) {
		each_line_starting(file, "State:\t", &process_status_cb, &ret);
		fclose(file);
		if (ret == PS_INVALID)
			fprintf(stderr,
				"Couldn't determine status of process %d: %s\n",
				pid, strerror(errno));
	} else {
		/* If the file is not present, the process presumably
		 * exited already.  */
		ret = PS_ZOMBIE;
	}

	return ret;
}

static int
all_digits(const char *str)
{
	while (isdigit(*str))
		str++;
	return !*str;
}

int
process_tasks(pid_t pid, pid_t **ret_tasks, size_t *ret_n)
{
	PROC_PID_FILE(fn, "/proc/%d/task", pid);
	DIR *d = opendir(fn);
	if (d == NULL)
		return -1;

	pid_t *tasks = NULL;
	size_t n = 0;
	size_t alloc = 0;

	while (1) {
		struct dirent entry;
		struct dirent *result;
		if (readdir_r(d, &entry, &result) != 0) {
		fail:
			free(tasks);
			closedir(d);
			return -1;
		}
		if (result == NULL)
			break;
		if (result->d_type == DT_DIR && all_digits(result->d_name)) {
			pid_t npid = atoi(result->d_name);
			if (n >= alloc) {
				alloc = alloc > 0 ? (2 * alloc) : 8;
				pid_t *ntasks = realloc(tasks,
							sizeof(*tasks) * alloc);
				if (ntasks == NULL)
					goto fail;
				tasks = ntasks;
			}
			assert(n < alloc);
			tasks[n++] = npid;
		}
	}

	closedir(d);

	*ret_tasks = tasks;
	*ret_n = n;
	return 0;
}

/* On native 64-bit system, we need to be careful when handling cross
 * tracing.  This select appropriate pointer depending on host and
 * target architectures.  XXX Really we should abstract this into the
 * ABI object, as theorized about somewhere on pmachata/revamp
 * branch.  */
static void *
select_32_64(struct process *proc, void *p32, void *p64)
{
	if (sizeof(long) == 4 || proc->mask_32bit)
		return p32;
	else
		return p64;
}

static int
fetch_dyn64(struct process *proc, arch_addr_t *addr, Elf64_Dyn *ret)
{
	if (umovebytes(proc, *addr, ret, sizeof(*ret)) != sizeof(*ret))
		return -1;
	*addr += sizeof(*ret);
	return 0;
}

static int
fetch_dyn32(struct process *proc, arch_addr_t *addr, Elf64_Dyn *ret)
{
	Elf32_Dyn dyn;
	if (umovebytes(proc, *addr, &dyn, sizeof(dyn)) != sizeof(dyn))
		return -1;

	*addr += sizeof(dyn);
	ret->d_tag = dyn.d_tag;
	ret->d_un.d_val = dyn.d_un.d_val;

	return 0;
}

static int (*
dyn_fetcher(struct process *proc))(struct process *,
				   arch_addr_t *, Elf64_Dyn *)
{
	return select_32_64(proc, fetch_dyn32, fetch_dyn64);
}

int
proc_find_dynamic_entry_addr(struct process *proc, arch_addr_t src_addr,
			     int d_tag, arch_addr_t *ret)
{
	debug(DEBUG_FUNCTION, "find_dynamic_entry()");

	if (ret == NULL || src_addr == 0 || d_tag < 0)
		return -1;

	int i = 0;
	while (1) {
		Elf64_Dyn entry;
		if (dyn_fetcher(proc)(proc, &src_addr, &entry) < 0
		    || entry.d_tag == DT_NULL
		    || i++ > 100) { /* Arbitrary cut-off so that we
				     * don't loop forever if the
				     * binary is corrupted.  */
			debug(2, "Couldn't find address for dtag!");
			return -1;
		}

		if (entry.d_tag == d_tag) {
			/* XXX The double cast should be removed when
			 * arch_addr_t becomes integral type.  */
			*ret = (arch_addr_t)(uintptr_t)entry.d_un.d_val;
			debug(2, "found address: %p in dtag %d", *ret, d_tag);
			return 0;
		}
	}
}

/* Our own type for representing 32-bit linkmap.  We can't rely on the
 * definition in link.h, because that's only accurate for our host
 * architecture, not for target architecture (where the traced process
 * runs). */
#define LT_LINK_MAP(BITS)			\
	{					\
		Elf##BITS##_Addr l_addr;	\
		Elf##BITS##_Addr l_name;	\
		Elf##BITS##_Addr l_ld;		\
		Elf##BITS##_Addr l_next;	\
		Elf##BITS##_Addr l_prev;	\
	}
struct lt_link_map_32 LT_LINK_MAP(32);
struct lt_link_map_64 LT_LINK_MAP(64);

static int
fetch_lm64(struct process *proc, arch_addr_t addr,
	   struct lt_link_map_64 *ret)
{
	if (umovebytes(proc, addr, ret, sizeof(*ret)) != sizeof(*ret))
		return -1;
	return 0;
}

static int
fetch_lm32(struct process *proc, arch_addr_t addr,
	   struct lt_link_map_64 *ret)
{
	struct lt_link_map_32 lm;
	if (umovebytes(proc, addr, &lm, sizeof(lm)) != sizeof(lm))
		return -1;

	ret->l_addr = lm.l_addr;
	ret->l_name = lm.l_name;
	ret->l_ld = lm.l_ld;
	ret->l_next = lm.l_next;
	ret->l_prev = lm.l_prev;

	return 0;
}

static int (*
lm_fetcher(struct process *proc))(struct process *,
				  arch_addr_t, struct lt_link_map_64 *)
{
	return select_32_64(proc, fetch_lm32, fetch_lm64);
}

/* The same as above holds for struct r_debug.  */
#define LT_R_DEBUG(BITS)			\
	{					\
		int r_version;			\
		Elf##BITS##_Addr r_map;		\
		Elf##BITS##_Addr r_brk;		\
		int r_state;			\
		Elf##BITS##_Addr r_ldbase;	\
	}

struct lt_r_debug_32 LT_R_DEBUG(32);
struct lt_r_debug_64 LT_R_DEBUG(64);

static int
fetch_rd64(struct process *proc, arch_addr_t addr,
	   struct lt_r_debug_64 *ret)
{
	if (umovebytes(proc, addr, ret, sizeof(*ret)) != sizeof(*ret))
		return -1;
	return 0;
}

static int
fetch_rd32(struct process *proc, arch_addr_t addr,
	   struct lt_r_debug_64 *ret)
{
	struct lt_r_debug_32 rd;
	if (umovebytes(proc, addr, &rd, sizeof(rd)) != sizeof(rd))
		return -1;

	ret->r_version = rd.r_version;
	ret->r_map = rd.r_map;
	ret->r_brk = rd.r_brk;
	ret->r_state = rd.r_state;
	ret->r_ldbase = rd.r_ldbase;

	return 0;
}

static int (*
rdebug_fetcher(struct process *proc))(struct process *,
				      arch_addr_t, struct lt_r_debug_64 *)
{
	return select_32_64(proc, fetch_rd32, fetch_rd64);
}

static int
fetch_auxv64_entry(int fd, Elf64_auxv_t *ret)
{
	/* Reaching EOF is as much problem as not reading whole
	 * entry.  */
	return read(fd, ret, sizeof(*ret)) == sizeof(*ret) ? 0 : -1;
}

static int
fetch_auxv32_entry(int fd, Elf64_auxv_t *ret)
{
	Elf32_auxv_t auxv;
	if (read(fd, &auxv, sizeof(auxv)) != sizeof(auxv))
		return -1;

	ret->a_type = auxv.a_type;
	ret->a_un.a_val = auxv.a_un.a_val;
	return 0;
}

static int (*
auxv_fetcher(struct process *proc))(int, Elf64_auxv_t *)
{
	return select_32_64(proc, fetch_auxv32_entry, fetch_auxv64_entry);
}

static void
crawl_linkmap(struct process *proc, struct lt_r_debug_64 *dbg)
{
	debug (DEBUG_FUNCTION, "crawl_linkmap()");

	if (!dbg || !dbg->r_map) {
		debug(2, "Debug structure or it's linkmap are NULL!");
		return;
	}

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t addr = (arch_addr_t)(uintptr_t)dbg->r_map;

	while (addr != 0) {
		struct lt_link_map_64 rlm = {};
		if (lm_fetcher(proc)(proc, addr, &rlm) < 0) {
			debug(2, "Unable to read link map");
			return;
		}

		arch_addr_t key = addr;
		/* XXX The double cast should be removed when
		 * arch_addr_t becomes integral type.  */
		addr = (arch_addr_t)(uintptr_t)rlm.l_next;
		if (rlm.l_name == 0) {
			debug(2, "Name of mapped library is NULL");
			return;
		}

		char lib_name[BUFSIZ];
		/* XXX The double cast should be removed when
		 * arch_addr_t becomes integral type.  */
		umovebytes(proc, (arch_addr_t)(uintptr_t)rlm.l_name,
			   lib_name, sizeof(lib_name));

		/* Library name can be an empty string, in which case
		 * the entry represents either the main binary, or a
		 * VDSO.  Unfortunately we can't rely on that, as in
		 * recent glibc, that entry is initialized to VDSO
		 * SONAME.
		 *
		 * It's not clear how to detect VDSO in this case.  We
		 * can't assume that l_name of real DSOs will be
		 * either absolute or relative (for LD_LIBRARY_PATH=:
		 * it will be neither).  We can't compare l_addr with
		 * AT_SYSINFO_EHDR either, as l_addr is bias (which
		 * also means it's not unique, and therefore useless
		 * for this).  We could load VDSO from process image
		 * and at least compare actual SONAMEs.  For now, this
		 * kludge is about the best that we can do.  */
		if (*lib_name == 0
		    || strcmp(lib_name, "linux-vdso.so.1") == 0
		    || strcmp(lib_name, "linux-gate.so.1") == 0
		    || strcmp(lib_name, "linux-vdso32.so.1") == 0
		    || strcmp(lib_name, "linux-vdso64.so.1") == 0)
			continue;

		/* Do we have that library already?  */
		if (proc_each_library(proc, NULL, library_with_key_cb, &key))
			continue;

		struct library *lib = malloc(sizeof(*lib));
		if (lib == NULL) {
		fail:
			free(lib);
			fprintf(stderr, "Couldn't load ELF object %s: %s\n",
				lib_name, strerror(errno));
			continue;
		}

		if (library_init(lib, LT_LIBTYPE_DSO) < 0)
			goto fail;

		if (ltelf_read_library(lib, proc, lib_name, rlm.l_addr) < 0) {
			library_destroy(lib);
			goto fail;
		}

		lib->key = key;
		proc_add_library(proc, lib);
	}
	return;
}

static int
load_debug_struct(struct process *proc, struct lt_r_debug_64 *ret)
{
	debug(DEBUG_FUNCTION, "load_debug_struct");

	if (rdebug_fetcher(proc)(proc, proc->os.debug_addr, ret) < 0) {
		debug(2, "This process does not have a debug structure!");
		return -1;
	}

	return 0;
}

static void
rdebug_bp_on_hit(struct breakpoint *bp, struct process *proc)
{
	debug(DEBUG_FUNCTION, "arch_check_dbg");

	struct lt_r_debug_64 rdbg;
	if (load_debug_struct(proc, &rdbg) < 0) {
		debug(2, "Unable to load debug structure!");
		return;
	}

	if (rdbg.r_state == RT_CONSISTENT) {
		debug(2, "Linkmap is now consistent");
		switch (proc->os.debug_state) {
		case RT_ADD:
			debug(2, "Adding DSO to linkmap");
			crawl_linkmap(proc, &rdbg);
			break;
		case RT_DELETE:
			debug(2, "Removing DSO from linkmap");
			// XXX unload that library
			break;
		default:
			debug(2, "Unexpected debug state!");
		}
	}

	proc->os.debug_state = rdbg.r_state;
}

#ifndef ARCH_HAVE_FIND_DL_DEBUG
int
arch_find_dl_debug(struct process *proc, arch_addr_t dyn_addr,
		   arch_addr_t *ret)
{
	return proc_find_dynamic_entry_addr(proc, dyn_addr, DT_DEBUG, ret);
}
#endif

int
linkmap_init(struct process *proc, arch_addr_t dyn_addr)
{
	debug(DEBUG_FUNCTION, "linkmap_init(%d, dyn_addr=%p)", proc->pid, dyn_addr);

	if (arch_find_dl_debug(proc, dyn_addr, &proc->os.debug_addr) == -1) {
		debug(2, "Couldn't find debug structure!");
		return -1;
	}

	int status;
	struct lt_r_debug_64 rdbg;
	if ((status = load_debug_struct(proc, &rdbg)) < 0) {
		debug(2, "No debug structure or no memory to allocate one!");
		return status;
	}

	crawl_linkmap(proc, &rdbg);

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t addr = (arch_addr_t)(uintptr_t)rdbg.r_brk;
	if (arch_translate_address_dyn(proc, addr, &addr) < 0)
		return -1;

	struct breakpoint *rdebug_bp = insert_breakpoint_at(proc, addr, NULL);
	if (rdebug_bp == NULL) {
		/* This is not fatal, the tracing can continue with
		 * reduced functionality.  */
		fprintf(stderr,
			"Couldn't insert _r_debug breakpoint to %d: %s.\n"
			"As a result of that, ltrace will not be able to "
			"detect and trace\nnewly-loaded libraries.\n",
			proc->pid, strerror(errno));
	} else {
		static struct bp_callbacks rdebug_callbacks = {
			.on_hit = rdebug_bp_on_hit,
		};
		rdebug_bp->cbs = &rdebug_callbacks;
	}

	return 0;
}

int
task_kill (pid_t pid, int sig)
{
	// Taken from GDB
        int ret;

        errno = 0;
        ret = syscall (__NR_tkill, pid, sig);
	return ret;
}

void
process_removed(struct process *proc)
{
	delete_events_for(proc);
}

int
process_get_entry(struct process *proc,
		  arch_addr_t *entryp,
		  arch_addr_t *interp_biasp)
{
	PROC_PID_FILE(fn, "/proc/%d/auxv", proc->pid);
	int fd = open(fn, O_RDONLY);
	int ret = 0;
	if (fd == -1) {
	fail:
		fprintf(stderr, "couldn't read %s: %s", fn, strerror(errno));
		ret = -1;
	done:
		if (fd != -1)
			close(fd);
		return ret;
	}

	arch_addr_t at_entry = 0;
	arch_addr_t at_bias = 0;
	while (1) {
		Elf64_auxv_t entry = {};
		if (auxv_fetcher(proc)(fd, &entry) < 0)
			goto fail;

		switch (entry.a_type) {
		case AT_BASE:
			/* XXX The double cast should be removed when
			 * arch_addr_t becomes integral type.  */
			at_bias = (arch_addr_t)(uintptr_t)entry.a_un.a_val;
			continue;

		case AT_ENTRY:
			/* XXX The double cast should be removed when
			 * arch_addr_t becomes integral type.  */
			at_entry = (arch_addr_t)(uintptr_t)entry.a_un.a_val;
		default:
			continue;

		case AT_NULL:
			break;
		}
		break;
	}

	if (entryp != NULL)
		*entryp = at_entry;
	if (interp_biasp != NULL)
		*interp_biasp = at_bias;
	goto done;
}

int
os_process_init(struct process *proc)
{
	proc->os.debug_addr = 0;
	proc->os.debug_state = 0;
	return 0;
}

void
os_process_destroy(struct process *proc)
{
}

int
os_process_clone(struct process *retp, struct process *proc)
{
	retp->os = proc->os;
	return 0;
}

int
os_process_exec(struct process *proc)
{
	return 0;
}
