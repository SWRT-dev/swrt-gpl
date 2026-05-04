/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
 * Copyright (C) 2008,2009 Juan Cespedes
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

#include <errno.h>
#include <gelf.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>

#include "proc.h"
#include "common.h"
#include "library.h"

/* A bundle is 128 bits.  */
#define BUNDLE_SIZE 16

/* The PLT has
 *
 * - 3 bundles as a header
 *
 * - The special reserved entry
 *
 * - Following that, each PLT entry has it's initial code that the GOT
 *   entry points to.  Each PLT entry has one bundle allocated.
 *
 * - Following that, each PLT entry has two bundles of actual PLT
 *   code, i.e. load up the address from the GOT and jump to it.  This
 *   is the point we want to insert the breakpoint, as this will be
 *   captured every time we jump to the PLT entry in the code.
 */

GElf_Addr
arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela * rela)
{
	/* Find number of entires by removing header and special
	 * entry, dividing total size by three, since each PLT entry
	 * will have 3 bundles (1 for inital entry and two for the PLT
	 * code). */
	int entries = (lte->plt_size - 4 * BUNDLE_SIZE) / (3 * BUNDLE_SIZE);

	/* Now the point we want to break on is the PLT entry after
	 * all the header stuff */
	unsigned long addr =
	    lte->plt_addr + (4 * BUNDLE_SIZE) + (BUNDLE_SIZE * entries) +
	    (2 * ndx * BUNDLE_SIZE);
	debug(3, "Found PLT %zd entry at %lx", ndx, addr);

	return addr;
}

void *
sym2addr(struct process *proc, struct library_symbol *sym)
{
	return sym->enter_addr;
}

int
arch_translate_address_dyn(struct process *proc,
			   arch_addr_t addr, arch_addr_t *ret)
{
	errno = 0;
	unsigned long l = ptrace(PTRACE_PEEKTEXT, proc->pid, addr, 0);
	if (l == -1UL && errno) {
		fprintf(stderr,	"dynamic .opd translation of %p: %s\n",
			addr, strerror(errno));
		return -1;
	}

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	*ret = (arch_addr_t)(uintptr_t)l;
	return 0;
}

int
arch_translate_address(struct ltelf *lte,
		       arch_addr_t addr, arch_addr_t *ret)
{
	*ret = addr;
	return 0;
}
