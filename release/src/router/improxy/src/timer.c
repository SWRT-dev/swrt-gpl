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
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/queue.h>
#include "proxy.h"
#include "timer.h"
#include "utils.h"

LIST_HEAD(, imp_timer) timer_head;

static struct timeval tm_max = {0x7FFFFFFF, 0x7FFFFFFF};


imp_timer *imp_add_timer(imp_timer *(*func)(void* ), void *data)
{
    imp_timer *newtimer;

    if ((newtimer = malloc(sizeof(*newtimer))) == NULL) {
        IMP_LOG_ERROR("can't allocate memory");
        return NULL;
    }

    memset(newtimer, 0, sizeof(imp_timer));

    if (func == NULL) {
        IMP_LOG_FATAL("timer's callback function undefind");
        exit(1);
    }
    newtimer->expire = func;
    newtimer->expire_data = data;
    newtimer->tm = tm_max;

    LIST_INSERT_HEAD(&timer_head, newtimer, link);

    return newtimer;
}

/*Double pointer to make timer to NULL*/
void imp_remove_timer(imp_timer **timer)
{
    LIST_REMOVE(*timer, link);
    free(*timer);
    *timer = NULL;
}

void imp_init_timer(void)
{
    LIST_INIT(&timer_head);
}
/*-----------------------------------------------------------------------
 * Name         : imp_set_timer
 *
 * Brief        : group timer expired
 * Params       : [in] tv_sec -- second need to wait
                  [in] timer  --
 * Return       : void
*------------------------------------------------------------------------
*/

void imp_set_timer(time_t tv_sec, imp_timer  *timer)
{
    struct timeval now;

    if(timer == NULL) {
        IMP_LOG_ERROR("timer is null");
        return;
    }

    if(get_sysuptime(&now) < 0) {
        IMP_LOG_ERROR("Can't get time of now");
        return;
    }
    now.tv_sec += tv_sec;

    memcpy(&timer->tm, &now, sizeof(struct timeval));
    return;

}


void imp_time_val(time_t tv_sec, struct timeval *result)
{
    struct timeval now;


    if(get_sysuptime(&now) < 0) {
        IMP_LOG_ERROR("Can't get time of now");
        return;
    }
    now.tv_sec += tv_sec;

    memcpy(result, &now, sizeof(struct timeval));
    return;

}

void timeval_add(struct timeval *a, struct timeval *b, struct timeval *result)
{
    long l;

    if ((l = a->tv_usec + b->tv_usec) < MILLION) {
        result->tv_usec = l;
        result->tv_sec = a->tv_sec + b->tv_sec;
    }
    else {
        result->tv_usec = l - MILLION;
        result->tv_sec = a->tv_sec + b->tv_sec + 1;
    }
}

void timeval_sub(struct timeval *a, struct timeval *b, struct timeval *result)
{
    long l;

    if ((l = a->tv_usec - b->tv_usec) >= 0) {
        result->tv_usec = l;
        result->tv_sec = a->tv_sec - b->tv_sec;
    }
    else {
        result->tv_usec = MILLION + l;
        result->tv_sec = a->tv_sec - b->tv_sec - 1;
    }
}


struct timeval* imp_check_timer(void)
{
    static struct timeval returnval;
    imp_timer *mt, *mt_next;
    struct timeval *w = &tm_max;
    struct timeval now;
    int res = 0;

    for (mt = LIST_FIRST(&timer_head); mt; mt = mt_next)
    {

        mt_next = LIST_NEXT(mt, link);
        memset(&now, 0, sizeof(now));

        if ((res = get_sysuptime(&now)) == -1 ){
            returnval.tv_sec = returnval.tv_usec = 0;
            return (&returnval);
        }

        if(TIMEVAL_ISZERO(mt->tm))
            continue;

        if (TIMEVAL_LT(mt->tm, now)) {

            if (mt->expire(mt->expire_data) == NULL) {

                IMP_LOG_DEBUG("continue............\n");
                /*some object have many timers, like group*/
                mt_next = LIST_FIRST(&timer_head);
                w = &tm_max;
                continue;/*time has been freed*/
            }
        }

        if (TIMEVAL_LT(mt->tm, *w))
            w = &mt->tm;

    }

    if (TIMEVAL_EQUAL(*w, tm_max))
        return NULL;

    if ((res = get_sysuptime(&now)) == -1 ){
        returnval.tv_sec = returnval.tv_usec = 0;
        return (&returnval);
    }
    //printf("w %d now %d\n", w->tv_sec, now.tv_sec);
    if (TIMEVAL_LT(now, *w)){
        memcpy(&returnval, w, sizeof(returnval));
        timeval_sub(w, &now, &returnval);
    }
    else {
        returnval.tv_sec = returnval.tv_usec = 0;
    }

    return &returnval;

}
