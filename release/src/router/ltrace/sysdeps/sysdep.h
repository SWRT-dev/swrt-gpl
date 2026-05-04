/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
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

#ifndef LTRACE_SYSDEP_H
#define LTRACE_SYSDEP_H

#include <arch.h>
#ifndef ARCH_HAVE_ADDRESS_TYPES
/* We should in general be able to trace 64-bit processes with 32-bit
 * ltrace (this is possible on PPC, and generally there should be no
 * problem tracing x86_64 processes from x32 ltrace, though it isn't
 * possible from i386 ltrace).  But ltrace is currently hopelessly
 * infested with using void* for host address.  So keep with it, for
 * now.  */
typedef void *arch_addr_t;
#endif

#include <os.h>

#ifndef ARCH_HAVE_LTELF_DATA
struct arch_ltelf_data {
};
#endif

#ifndef OS_HAVE_BREAKPOINT_DATA
struct os_breakpoint_data {
};
#endif

#ifndef ARCH_HAVE_BREAKPOINT_DATA
struct arch_breakpoint_data {
};
#endif

#ifndef OS_HAVE_LIBRARY_SYMBOL_DATA
struct os_library_symbol_data {
};
#endif

#ifndef ARCH_HAVE_LIBRARY_SYMBOL_DATA
struct arch_library_symbol_data {
};
#endif

#ifndef OS_HAVE_LIBRARY_DATA
struct os_library_data {
};
#endif

#ifndef ARCH_HAVE_LIBRARY_DATA
struct arch_library_data {
};
#endif

#ifndef OS_HAVE_PROCESS_DATA
struct os_process_data {
};
#endif

#ifndef ARCH_HAVE_PROCESS_DATA
struct arch_process_data {
};
#endif

#endif /* LTRACE_SYSDEP_H */
