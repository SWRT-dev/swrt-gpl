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
#ifndef __IMP_PROXY_H_
#define __IMP_PROXY_H_
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <errno.h>
#include <sys/queue.h>
#include <sys/time.h>
#include <asm/byteorder.h>
#include <netinet/icmp6.h>
#include "timer.h"
#include "os.h"

#define IMP_VERSION "0.3"

#define MLD_V2_LISTENER_REPORT 143

#define DEFAULT_RV  2
#define TIMER_QI    125
#define TIMER_QRI   10
#define TIMER_LMQI  1
#define TIMER_SQI   TIMER_QI/4
#define TIMER_GMI   ((DEFAULT_RV * TIMER_QI) + TIMER_QRI)
#define TIMER_OHPI  ((DEFAULT_RV * TIMER_QI) + TIMER_QRI)
#define TIMER_OQPI  ((DEFAULT_RV * TIMER_QI) + TIMER_QRI/2)
#define TIMER_LMQT  (DEFAULT_RV * TIMER_LMQI)
/* The Startup Query Interval is the interval between General Queries
 * sent by a Querier on startup.  Default: 1/4 the Query Interval.[RFC 3376 8.6]
 */
#define TIMER_SQMI  ((TIMER_QI)/4)

#define VIF_NOT_EQUAL_MIF
#define ENABLE_IMP_MLD

#define MAX_RECV_BUF_LEN 2048

enum {
    IMP_ENABLE  = 1,
    IMP_DISABLE   = 0
};

#define mld_num_multi    mld_icmp6_hdr.icmp6_data16[1]

typedef struct {
    int     igmp_version;
    int     mld_version;
    int     igmp_socket;
    int     mld_socket;
    int     igmp_udp_socket;
    int     mld_udp_socket;
    int     upif_index;
    unsigned char print_level;
    unsigned char syslog_level;
    unsigned char quick_leave;
    unsigned char down_to_up;
}mcast_proxy;

int get_udp_socket(int family);
int get_igmp_mld_socket(int family);
int get_up_if_index(void);
int get_down_to_up_enable(void);
int get_group_leave_time();
int get_im_version(int family);
int get_group_leave_time(void);
#endif
