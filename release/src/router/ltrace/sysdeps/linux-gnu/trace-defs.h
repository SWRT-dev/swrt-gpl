/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
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
#ifndef TRACE_DEFS_H
#define TRACE_DEFS_H
#include <sys/ptrace.h>

/* If the system headers did not provide the constants, hard-code the
 * normal values.  */
#ifndef PTRACE_OLDSETOPTIONS
# define PTRACE_OLDSETOPTIONS    21
#endif

#ifndef PTRACE_SETOPTIONS
# define PTRACE_SETOPTIONS       0x4200
#endif

#ifndef PTRACE_GETEVENTMSG
# define PTRACE_GETEVENTMSG      0x4201
#endif

/* Options set using PTRACE_SETOPTIONS.  */
#ifndef PTRACE_O_TRACESYSGOOD
# define PTRACE_O_TRACESYSGOOD   0x00000001
#endif

#ifndef PTRACE_O_TRACEFORK
# define PTRACE_O_TRACEFORK      0x00000002
#endif

#ifndef PTRACE_O_TRACEVFORK
# define PTRACE_O_TRACEVFORK     0x00000004
#endif

#ifndef PTRACE_O_TRACECLONE
# define PTRACE_O_TRACECLONE     0x00000008
#endif

#ifndef PTRACE_O_TRACEEXEC
# define PTRACE_O_TRACEEXEC      0x00000010
#endif

#ifndef PTRACE_O_TRACEVFORKDONE
# define PTRACE_O_TRACEVFORKDONE 0x00000020
#endif

#ifndef PTRACE_O_TRACEEXIT
# define PTRACE_O_TRACEEXIT      0x00000040
#endif

/* Wait extended result codes for the above trace options.  */
#ifndef PTRACE_EVENT_FORK
# define PTRACE_EVENT_FORK       1
#endif

#ifndef PTRACE_EVENT_VFORK
# define PTRACE_EVENT_VFORK      2
#endif

#ifndef PTRACE_EVENT_CLONE
# define PTRACE_EVENT_CLONE      3
#endif

#ifndef PTRACE_EVENT_EXEC
# define PTRACE_EVENT_EXEC       4
#endif

#ifndef PTRACE_EVENT_VFORK_DONE
# define PTRACE_EVENT_VFORK_DONE 5
#endif

#ifndef PTRACE_EVENT_EXIT
# define PTRACE_EVENT_EXIT       6
#endif

#endif /* TRACE_DEFS_H */
