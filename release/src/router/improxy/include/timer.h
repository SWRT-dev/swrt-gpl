/*
 * This file is part of improxy.
 *
 * Copyright (C) 2012 by Haibo Xi <haibbo@gmail.com>
 *
 * The program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * Website: https://github.com/haibbo/improxy
 */

#ifndef __IMP_TIMER_H_
#define __IMP_TIMER_H_
#include "queue.h"

/*timer*/
typedef struct imp_timer {
    LIST_ENTRY(imp_timer)   link;
    struct  timeval         tm;
    struct imp_timer       *(*expire) (void*);
    void                   *expire_data;

} imp_timer;

imp_timer *imp_add_timer(imp_timer *(*func)(void* ),void *data);
struct timeval* imp_check_timer(void);
void imp_init_timer(void);
void timeval_sub(struct timeval *a, struct timeval *b, struct timeval *result);
void timeval_add(struct timeval *a, struct timeval *b, struct timeval *result);
void imp_set_timer(time_t tv_sec,imp_timer  *timer);
void imp_remove_timer(imp_timer **timer);
void imp_time_val(time_t tv_sec, struct timeval *result);

#define MILLION 1000000
/* a < b */
#define TIMEVAL_LT(a, b) (((a).tv_sec < (b).tv_sec) ||\
              (((a).tv_sec == (b).tv_sec) && \
                ((a).tv_usec < (b).tv_usec)))
/* a <= b */
#define TIMEVAL_LEQ(a, b) (((a).tv_sec < (b).tv_sec) ||\
               (((a).tv_sec == (b).tv_sec) &&\
                ((a).tv_usec <= (b).tv_usec)))
/* a == b */
#define TIMEVAL_EQUAL(a, b) ((a).tv_sec == (b).tv_sec &&\
                 (a).tv_usec == (b).tv_usec)

/* a == 0 */
#define TIMEVAL_ISZERO(a) ((a).tv_sec == 0 &&\
                 (a).tv_usec == 0)


#endif
