/*
 * timer.c - timer utility functions.
 *
 * Copyright (C) 2021-2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "timer.h"

void timer_init(atimer_t *t, void (*function)(atimer_t *))
{
	struct uloop_timeout *ut = &t->t;

	ut->cb = (uloop_timeout_handler)function;
}

static void clock_getnow(struct timeval *tv)
{
	struct timespec now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	tv->tv_sec = now.tv_sec;
	tv->tv_usec = now.tv_nsec / 1000;
}

int timer_set(atimer_t *t, uint32_t tmo_ms)
{
	struct uloop_timeout *ut = &t->t;

	clock_getnow(&t->expires);
	t->expires.tv_sec += tmo_ms / 1000;
	t->expires.tv_usec += (tmo_ms % 1000) * 1000;
	if (t->expires.tv_usec > 1000000) {
		t->expires.tv_sec++;
		t->expires.tv_usec -= 1000000;
	}

	return uloop_timeout_set(ut, tmo_ms);
}

int timer_del(atimer_t *t)
{
	struct uloop_timeout *ut = &t->t;

	return uloop_timeout_cancel(ut);
}

int timer_pending(atimer_t *t)
{
	struct uloop_timeout *ut = &t->t;

	return ut->pending == true ? 1 : 0;
}

int timer_remaining_ms(atimer_t *t)
{
	struct timeval now, res;

	if (!timer_pending(t))
		return -1;

	clock_getnow(&now);
	timersub(&t->expires, &now, &res);

	return res.tv_sec * 1000 + res.tv_usec / 1000;
}
