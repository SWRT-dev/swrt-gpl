/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012 Petr Machata, Red Hat Inc.
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

#ifndef SYSDEPS_LINUX_GNU_EVENTS_H
#define SYSDEPS_LINUX_GNU_EVENTS_H

#include "forward.h"

/* Declarations for event que functions.  */

enum ecb_status {
	ECB_CONT, /* The iteration should continue.  */
	ECB_YIELD, /* The iteration should stop, yielding this
		    * event.  */
	ECB_DEQUE, /* Like ECB_STOP, but the event should be removed
		    * from the queue.  */
};

struct Event *each_qd_event(enum ecb_status (*cb)(struct Event *event,
						  void *data), void *data);
void delete_events_for(struct process *proc);
void enque_event(struct Event *event);

#endif /* SYSDEPS_LINUX_GNU_EVENTS_H */
