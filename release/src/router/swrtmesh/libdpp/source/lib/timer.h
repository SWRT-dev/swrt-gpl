/*
 * timer.h
 * Header file for timer utility functions.
 *
 * Copyright (C) 2021-2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 *
 */

#ifndef ATIMER_H
#define ATIMER_H

#include "timer_impl.h"

#ifndef ATIMER_T
#error "atimer_t not defined!"
#endif

void timer_init(atimer_t *t, void (*function)(atimer_t *));
int timer_set(atimer_t *t, uint32_t tmo_ms);
int timer_del(atimer_t *t);
int timer_pending(atimer_t *t);
int timer_remaining_ms(atimer_t *t);

#endif /* ATIMER_H */
