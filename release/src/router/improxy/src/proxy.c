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

#define _GNU_SOURCE
#include "proxy.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/if.h>
#include <signal.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include "timer.h"
#include "utils.h"
#include "input.h"
#include "handler.h"
#include "kernel_api.h"
#include "data.h"
#include "membership.h"

mcast_proxy mproxy;
char    pid_file[1024] = {0};

#define IGMP_ALL_ROUTERS        0xE0000002
#define IGMP_ALL_ROUTERS_V3     0xE0000016

static STATUS load_config(const char *file)
{
    FILE    *fd;
    char    buffer[1024] = {0};
    char    downinf[10][IFNAMSIZ], upinf[IFNAMSIZ];
    int     down_num, i;

    down_num = 0;
    memset(downinf, 0, 10 * IFNAMSIZ);
    memset(upinf, 0, IFNAMSIZ);

    fd = fopen(file, "r");
    if(fd == NULL)
    {
        perror(__FUNCTION__);
        exit(1);
    }

    while(fgets(buffer, 1024, fd))
    {
        char  *p_token, *p_value;
        if (strchr(buffer, '\n')) *(strchr(buffer, '\n')) = '\0';
        IMP_LOG_DEBUG("buffer = %s\n", buffer);
        p_token = buffer + strspn(buffer, " \t");
        if (*p_token == '#' || *p_token == '\0') {
            IMP_LOG_DEBUG("need continue\n");
            continue;
        }

        p_token = strtok(p_token, " \t");
        if (p_token == NULL)
            continue;

        p_value = strtok(NULL, " \t");
        if (p_value == NULL) {

            IMP_LOG_DEBUG("%s don't have value\n", p_token);
            return STATUS_NOK;;
        }

        IMP_LOG_DEBUG("token is %s\n", p_token);
        if (strcmp(p_token, "igmp") == 0) {

            if (strcmp(p_value, "enable") == 0) {
                char *p_version = NULL;

                mproxy.igmp_version = IM_IGMPv3_MLDv2;

                p_version = strtok(NULL, " \t");
                if(p_version != NULL) {

                    int  version    = 0;

                    if(strcmp(p_version, "version") != 0){

                        IMP_LOG_ERROR("error token %s\n", p_version);
                        return STATUS_NOK;
                    }
                    p_version = strtok(NULL, " \t");
                    version = atoi(p_version);
                    if (version < IM_IGMPv1 || version > IM_IGMPv3_MLDv2) {

                        IMP_LOG_ERROR("error version %d, should be 1 2 or 3\n",
                            version);
                        return STATUS_NOK;
                    }
                    mproxy.igmp_version = version;
                }
            }
            else if (strcmp(p_value, "disable") == 0)
                mproxy.igmp_version = IM_DISABLE;
            else {
                IMP_LOG_ERROR("error value %s, should be enable or disable\n",
                    p_value);
                return STATUS_NOK;
            }

        }
    #ifdef ENABLE_IMP_MLD
        else if (strcmp(p_token, "mld") == 0) {

            if (strcmp(p_value, "enable") == 0) {

                char *p_version = NULL;

                mproxy.mld_version = IM_IGMPv3_MLDv2;

                p_version = strtok(NULL, " \t");

                if (p_version != NULL ) {

                    int  version    = 0;

                    if( strcmp(p_version, "version") != 0){

                        IMP_LOG_ERROR("error token %s\n", p_version);
                        return STATUS_NOK;
                    }
                    p_version = strtok(NULL, " \t");
                    version = atoi(p_version);
                    if (version < IM_IGMPv2_MLDv1 - 1 ||
                        version > IM_IGMPv3_MLDv2 -1) {

                        IMP_LOG_ERROR("error version %d, should be 1 or 2\n",
                            version - 1);
                        return STATUS_NOK;
                    }
                    mproxy.mld_version = version + 1;
                }
            }
            else if (strcmp(p_value, "disable") == 0)
                mproxy.mld_version = IM_DISABLE;
            else {
                IMP_LOG_ERROR("error value %s, should be enable or disable\n", p_value);
                return STATUS_NOK;
            }

        }
    #endif
        else if (strcmp(p_token, "upstream") == 0) {

            if (upinf[0]) {

                IMP_LOG_ERROR("only allow a upstream interface\n");
                return STATUS_NOK;
            }
            if (strlen(p_value) >= sizeof(upinf)) {
                IMP_LOG_ERROR("too long name for upstream: %s\n", p_value);
                return STATUS_NOK;
            }
            strcpy(upinf, p_value);
            IMP_LOG_DEBUG("upstream interface is %s\n", p_value);
        } else if (strcmp(p_token, "downstream") == 0) {

            if (strlen(p_value) >= sizeof(downinf[down_num])) {
               IMP_LOG_ERROR("too long name for downstream: %s\n", p_value);
               return STATUS_NOK;
            }
            strcpy(downinf[down_num], p_value);
            down_num++;
            IMP_LOG_DEBUG("downstream interface is %s\n", p_value);
        } else if (strcmp(p_token, "quickleave") == 0) {

            if (strcmp(p_value, "enable") == 0) {
                mproxy.quick_leave = 1;
            }else if (strcmp(p_value, "disable") == 0) {
                mproxy.quick_leave = 0;
            }else {

                IMP_LOG_ERROR("error value %s, should be enable or disable\n", p_value);
                return STATUS_NOK;
            }
        }else if (strcmp(p_token, "down_to_up") == 0) {

            if (strcmp(p_value, "enable") == 0) {
                mproxy.down_to_up = 1;
            }else if (strcmp(p_value, "disable") == 0) {
                mproxy.down_to_up = 0;
            }else {

                IMP_LOG_ERROR("error value %s, should be enable or disable\n", p_value);
                return STATUS_NOK;
            }
            IMP_LOG_DEBUG("%s forwarding packets form downstream if to upstream if\n",
                           mproxy.down_to_up ? "Enable" : "Disable");
        }
        else {

            IMP_LOG_ERROR("unknown token %s\n", p_token);
            return STATUS_NOK;
        }
    }
    if(upinf[0] == 0) {

        IMP_LOG_ERROR("must be specified an upstream interface\n");
        return STATUS_NOK;
    }
    if(mproxy.igmp_version != IM_DISABLE) {

        imp_interface_create(upinf, AF_INET, INTERFACE_UPSTREAM);
        for(i = 0; i < down_num ; i ++) {
            imp_interface_create(downinf[i], AF_INET, INTERFACE_DOWNSTREAM);
        }

    }

#ifdef ENABLE_IMP_MLD
    if(mproxy.mld_version != IM_DISABLE) {

        imp_interface_create(upinf, AF_INET6, INTERFACE_UPSTREAM);
        for(i = 0; i < down_num; i ++) {

            imp_interface_create(downinf[i], AF_INET6, INTERFACE_DOWNSTREAM);
        }
    }
#endif
    fclose(fd);
    return STATUS_OK;

}

int get_udp_socket(int family)
{
    if(family == AF_INET)
        return mproxy.igmp_udp_socket;
    else if(family == AF_INET6)
        return mproxy.mld_udp_socket;
    else
        IMP_LOG_ERROR("unsoupprt family %d", family);
    return 0;
}

int get_igmp_mld_socket(int family)
{
    if(family == AF_INET)
        return mproxy.igmp_socket;
    else if(family == AF_INET6)
        return mproxy.mld_socket;
    else
        IMP_LOG_ERROR("unsoupprt family %d", family);
    return 0;
}
int get_up_if_index(void)
{
    return mproxy.upif_index;
}

int get_down_to_up_enable(void)
{
    return mproxy.down_to_up;
}

int get_im_version(int family)
{
    if(family == AF_INET)
        return mproxy.igmp_version;
    else if(family == AF_INET6)
        return mproxy.mld_version;
    else
        IMP_LOG_ERROR("unsoupprt family %d", family);
    return 0;
}

int get_group_leave_time()
{
    if(mproxy.quick_leave)
        return 0;
    return TIMER_LMQT;
}

void free_resource(void)
{
    imp_interface_cleanup_all();
    imp_membership_db_cleanup_all();

    return;

}
STATUS init_mproxy4(mcast_proxy *p_mp)
{
    char ra[4];
    int i;
    unsigned char ui = 0;
    pi_addr pia;


    bzero(&pia, sizeof(pia));

    p_mp->igmp_socket = -1;
    p_mp->igmp_udp_socket = -1;

    if (p_mp->igmp_version != IM_DISABLE) {

        imp_interface* p_if = NULL;

        p_mp->igmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP);
        if(p_mp->igmp_socket == -1)
        {
            IMP_LOG_ERROR("can't create igmp socket\n");
            return STATUS_NOK;
        }
        /* Set router alert */
        ra[0] = 148;
        ra[1] = 4;
        ra[2] = 0;
        ra[3] = 0;
        if (setsockopt(p_mp->igmp_socket, IPPROTO_IP, IP_OPTIONS, ra, 4) < 0)
            IMP_LOG_ERROR("IP_OPTIONS: %s\n", strerror(errno));

        /* Set reuseaddr, ttl, loopback and set outgoing interface */
        i = 1;
        if (setsockopt(p_mp->igmp_socket, SOL_SOCKET, SO_REUSEADDR,
            (void*)&i, sizeof(i)) < 0)
            IMP_LOG_ERROR("SO_REUSEADDR: %s\n", strerror(errno));

        i = 256*1024;
        if(setsockopt(p_mp->igmp_socket, SOL_SOCKET, SO_RCVBUF,
            (void*)&i, sizeof(i)) <0 )
            IMP_LOG_ERROR("SO_RCVBUF: %s\n", strerror(errno));

        ui = 1;
        if (setsockopt(p_mp->igmp_socket, IPPROTO_IP, IP_MULTICAST_TTL,
            (void*)&ui, sizeof(ui)))
            IMP_LOG_ERROR("IP_MULTICAST_TTL: %s\n", strerror(errno));
        ui = 1;
        if (setsockopt(p_mp->igmp_socket, IPPROTO_IP, IP_MULTICAST_LOOP,
            (void*)&ui, sizeof(ui)) < 0)
            IMP_LOG_ERROR("IP_MULTICAST_LOOP: %s\n", strerror(errno));
        ui = 1;
        if (setsockopt(p_mp->igmp_socket, IPPROTO_IP, IP_PKTINFO, &ui, sizeof(ui)))
            IMP_LOG_ERROR("IP_PKTINFO: %s\n", strerror(errno));
        ui = 0xC0; /* set DSCP mark CS6 for outgoing */
        if (setsockopt(p_mp->igmp_socket, IPPROTO_IP, IP_TOS, &ui, sizeof(ui))) {
            IMP_LOG_ERROR("IP_TOS: %s\n", strerror(errno));
        }

        if(k_start4_mproxy(p_mp->igmp_socket) < 0)
            return STATUS_NOK;

        IMP_LOG_DEBUG("p_mp->igmp_socket = %d\n", p_mp->igmp_socket);
        p_mp->igmp_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(p_mp->igmp_udp_socket == -1)
        {
            IMP_LOG_ERROR("can't create udp socket\n");
            return STATUS_NOK;
        }
        for(p_if = imp_interface_first(); p_if; p_if = LIST_NEXT(p_if, link)){

            if(p_if->if_addr.ss.ss_family != AF_INET ||
                p_if->type != INTERFACE_DOWNSTREAM)
                continue;
            pia.ss.ss_family = AF_INET;
            pia.v4.sin_addr.s_addr = htonl(IGMP_ALL_ROUTERS);
            if(k_mcast_join( &pia, p_if->if_name) < 0)
                IMP_LOG_ERROR("k_mcast_join: %s\n", strerror(errno));

            /* On each interface over which this protocol for multicast router is being run,
             * the router MUST enable reception of multicast address 224.0.0.22
             * [RFC 3376 section 6]
             */
            pia.v4.sin_addr.s_addr = htonl(IGMP_ALL_ROUTERS_V3);
            if(k_mcast_join( &pia, p_if->if_name) < 0)
                 IMP_LOG_ERROR("k_mcast_join: %s\n", strerror(errno));
        }
    }
    return STATUS_OK;
}
STATUS init_mproxy6(mcast_proxy *p_mp)
{
    struct icmp6_filter filt;
    int             on;
    static struct {
        struct ip6_hbh hdr;
        struct ip6_opt_router rt;
        unsigned char padding[2];
    } hopts = {
        .hdr = {0, 0},
        .rt = {IP6OPT_ROUTER_ALERT, 2, {0, IP6_ALERT_MLD}},
        .padding = {0x1, 0} /* PadN option, RFC 2460, 4.2 */
    };

#ifdef ENABLE_IMP_MLD
    if (p_mp->mld_version != IM_DISABLE) {

        struct in6_addr all_ipv6_router;
        imp_interface* p_if = NULL;

        p_mp->mld_socket = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);

        if(p_mp->mld_socket == -1)
        {
            IMP_LOG_ERROR("can't create mld socket\n");
            return STATUS_NOK;
        }

        /* filter all non-MLD ICMP messages */
        ICMP6_FILTER_SETBLOCKALL(&filt);
        ICMP6_FILTER_SETPASS(MLD_LISTENER_REPORT, &filt);
        ICMP6_FILTER_SETPASS(MLD_LISTENER_REDUCTION, &filt);
        ICMP6_FILTER_SETPASS(MLD_V2_LISTENER_REPORT, &filt);

        if (setsockopt(p_mp->mld_socket, IPPROTO_ICMPV6, ICMP6_FILTER, &filt,
               sizeof(filt)) < 0)
            IMP_LOG_ERROR("ICMP6_FILTER: %s\n", strerror(errno));

        /* specify to tell receiving interface */
        on = 1;
        if (setsockopt(p_mp->mld_socket, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on,
               sizeof(on)) < 0)
            IMP_LOG_ERROR("IPV6_RECVPKTINFO: %s\n", strerror(errno));

        on = 1;
        if (setsockopt(p_mp->mld_socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &on,
               sizeof(on)) < 0)
            IMP_LOG_ERROR("IPV6_MULTICAST_HOPS: %s\n", strerror(errno));

        on = 0;
        if (setsockopt(p_mp->mld_socket, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &on,
               sizeof(on)) < 0)
            IMP_LOG_ERROR("IPV6_MULTICAST_LOOP: %s\n", strerror(errno));

        on = 1;
        if (setsockopt(p_mp->mld_socket, IPPROTO_IPV6, IPV6_RECVHOPOPTS, &on,
               sizeof(on)) < 0)
            IMP_LOG_ERROR("IPV6_RECVHOPOPTS: %s\n", strerror(errno));

        /* RTR-ALERT option */
        if (setsockopt(p_mp->mld_socket, IPPROTO_IPV6, IPV6_HOPOPTS, &hopts, sizeof(hopts))) {
            IMP_LOG_ERROR("IPV6_HOPOPTS: %s\n", strerror(errno));
        }

        if (k_start6_mproxy(p_mp->mld_socket) < 0 )
            return STATUS_NOK;

        IMP_LOG_DEBUG("p_mp->mld_socket = %d\n", p_mp->mld_socket);
        p_mp->mld_udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
        if(p_mp->mld_udp_socket == -1)
        {
            IMP_LOG_ERROR("can't create mld udp socket\n");
            return STATUS_NOK;
        }

        for(p_if = imp_interface_first(); p_if; p_if = LIST_NEXT(p_if, link)){

            if(p_if->if_addr.ss.ss_family != AF_INET6 ||
                p_if->type != INTERFACE_DOWNSTREAM)
                continue;

            if(inet_pton(AF_INET6, "FF02::2", &all_ipv6_router) == 1) {

                pi_addr pir;
                imp_build_piaddr(AF_INET6, &all_ipv6_router, &pir);

                if(k_mcast_join(&pir, p_if->if_name) < 0)
                    IMP_LOG_DEBUG("k_mcast_join: %s\n", strerror(errno));

            }
            if(inet_pton(AF_INET6, "FF02::16", &all_ipv6_router) == 1) {

                pi_addr pir;
                imp_build_piaddr(AF_INET6, &all_ipv6_router, &pir);

                if(k_mcast_join(&pir, p_if->if_name) < 0)
                    IMP_LOG_DEBUG("k_mcast_join: %s\n", strerror(errno));

            }
        }
    }
#endif
    return STATUS_OK;
}

void init_timer(mcast_proxy* p_mp)
{
    imp_interface* p_if;
    for(p_if = imp_interface_first();p_if;p_if = LIST_NEXT(p_if, link))
    {
        if(p_if->type == INTERFACE_UPSTREAM)
            continue;
        p_if->startup = DEFAULT_RV;
        p_if->gq_timer = imp_add_timer( general_queries_timer_handler, p_if);
        /*send generial query fast*/
        imp_set_timer(2, p_if->gq_timer);
    }
}
static void sig_term_handler()
{
    if (pid_file[0] != 0)
        unlink(pid_file);
    free_resource();
    exit(1);
}
static void sig_usr1_handler()
{
    imp_interface* p_if;
    for(p_if = imp_interface_first();p_if;p_if = LIST_NEXT(p_if, link))
    {
        if(p_if->type == INTERFACE_UPSTREAM)
            continue;
        imp_group_print(p_if);
    }
    imp_membership_db_print_all();
}
static void show_usage()
{
    printf("Usage: improxy -c config file \n"
                    "\t-d log level 1,2,3,4,5\n"
                    "\t-p pid file\n"
                    "\t-v show version\n"
                    "\t-h this help\n"
                    "Version: %s\n", IMP_VERSION);
    exit(1);
}
int main(int argc, char *argv[])
{
    int opt = 10, pid = 0;
    char    config[1024] = {0};
    int log_level = 0;
    FILE *pidfp;

    memset(&mproxy, 0, sizeof(mcast_proxy));

    mproxy.print_level = LOG_PRI_ERROR;
    mproxy.syslog_level = LOG_PRI_ERROR;

    while((opt = getopt(argc, argv, "c:d:vp:h")) > 0)
    {
        switch (opt) {
            case 'c':
                IMP_LOG_DEBUG("config file is %s\n", optarg);
                strcpy(config, optarg);
                break;
            case 'p':
                IMP_LOG_DEBUG("pid file is %s\n", optarg);
                strcpy(pid_file, optarg);
                break;
            case 'd':
                /*to do log*/
                log_level = atoi(optarg);
                if(log_level < 0) {
                    IMP_LOG_DEBUG("log level must be greater than 0\n");
                    exit(1);
                }
                if(log_level > LOG_PRI_LOWEST)
                    log_level = LOG_PRI_DEBUG;
                mproxy.print_level = log_level;
                IMP_LOG_DEBUG("logLevel = %d\n", log_level);
                break;
            case 'v':
                printf("version: %s\n", IMP_VERSION);
                exit(1);
            case 'h':
                show_usage();
            default:
                show_usage();
        }

    }

    if(config[0] == 0) {

       show_usage();
    }

    /* dump current PID */
    pid = getpid();
    if ((pidfp = fopen(pid_file, "w")) != NULL) {
        fprintf(pidfp, "%d\n", pid);
        fclose(pidfp);
    }
    atexit(free_resource);
    signal(SIGTERM, sig_term_handler);
    signal(SIGINT, sig_term_handler);
    signal(SIGUSR1, sig_usr1_handler);

    imp_interface_init();
    imp_init_timer();
    imp_membership_db_init();


    if (load_config(config) == STATUS_NOK ||
        init_interface(&mproxy) == STATUS_NOK ||
        init_mproxy4(&mproxy) == STATUS_NOK ||
        init_mproxy6(&mproxy) == STATUS_NOK) {

        IMP_LOG_ERROR("exiting.........\n");
        free_resource();
        exit(1);
    }
    add_all_vif();
    init_timer(&mproxy);


    while(1)
    {
        struct timeval* tm;
        fd_set  fdset;
        int max = 0;
        int resval = 0;
        FD_ZERO(&fdset);
        if(mproxy.igmp_socket > 0)
            FD_SET(mproxy.igmp_socket, &fdset);

#ifdef ENABLE_IMP_MLD
        if(mproxy.mld_socket > 0)
            FD_SET(mproxy.mld_socket, &fdset);

        max = mproxy.igmp_socket > mproxy.mld_socket ?
                mproxy.igmp_socket:mproxy.mld_socket;
#else
        max = mproxy.igmp_socket;
#endif
        tm = imp_check_timer();

        resval = select(max + 1, &fdset, NULL, NULL, tm);
        if(resval == -1)
        {
            perror(__FUNCTION__);
        }else if(resval)
            IMP_LOG_DEBUG("Data arrive\n");
        else
            IMP_LOG_DEBUG("timeout\n");

        if(mproxy.igmp_socket > 0 &&
            FD_ISSET(mproxy.igmp_socket, &fdset)) {

            mcast_recv_igmp(mproxy.igmp_socket, mproxy.igmp_version);
        }
#ifdef ENABLE_IMP_MLD
        if (mproxy.mld_socket > 0 &&
            FD_ISSET(mproxy.mld_socket, &fdset)) {

            mcast_recv_mld(mproxy.mld_socket, mproxy.mld_version);
        }
#endif
    }

    free_resource();
    del_all_vif();
    k_stop4_mproxy(mproxy.igmp_socket);
    k_stop6_mproxy(mproxy.mld_socket);
}

