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
#include <sys/time.h>
#include <stdio.h>
#include "timer.h"
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <syscall.h>
#include "utils.h"
#include "proxy.h"
#include "data.h"

extern mcast_proxy  mproxy;
static unsigned long parse_hex(const char *p_str, unsigned char *p_inet);
#define PROC_FILE_UPTIME "/proc/uptime"
#define ALL_HOST_GROUP 0xe0000001U

/**************************************************************************
 * FUNCTION NAME : get_sysuptime
 **************************************************************************
 * DESCRIPTION   :
 * get system uptime form /proc/uptime
 * Return:
 * -1 fail   0 OK
 *************************************************************************/
int get_sysuptime(struct timeval* tm)
{
#ifdef CLOCK_MONOTONIC
    struct timespec ts;
    int res;

    res = syscall(SYS_clock_gettime, CLOCK_MONOTONIC, &ts);
    if (res != 0) {
        perror(__FUNCTION__);
        return -1;
    }

    tm->tv_sec = ts.tv_sec;
    tm->tv_usec = ts.tv_nsec / 1000;
#else
    int fd = 0, res = 0;
    char line[1024] = {0};
    unsigned int ws, wu, is, iu;

    fd = open(PROC_FILE_UPTIME, 'r');
    if (fd == -1)
    {
        perror(__FUNCTION__);
        return -1 ;
    }

    res = read(fd, line, sizeof(line));
    close(fd);
    if (res == 0){

        perror(__FUNCTION__);
        return -1;
    }
    sscanf(line, "%d.%d %d.%d", &ws, &wu, &is, &iu);
    tm->tv_sec = ws;
    tm->tv_usec = wu * 10 * 1000;
#endif

    return 0;
}

/**************************************************************************
 * FUNCTION NAME : in_cusm
 **************************************************************************
 * DESCRIPTION   :
 * calculate checksum
*************************************************************************/
unsigned short in_cusm(unsigned short* buf , int len)
{
    int sum = 0;
    int nword = len/2;
    while(nword)
    {
        sum += *buf++;
        nword--;
    }
    if (len%2)
        sum += *((char *)buf);

    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;
    return ~sum;
}


void logger_build(unsigned char  priority, const char *header,
    const char *function_name, int line_num, const char *format, ...)
{
    va_list va;
    char logbuf[1024]={0};
    char *p = logbuf;

    if (priority <= mproxy.print_level || priority <= mproxy.syslog_level) {

        p += sprintf(p, "%s[%s@%d]: ", header, function_name, line_num);
        va_start(va, format);
        p += vsprintf(p, format, va);
        va_end(va);
    }

    if (priority <= mproxy.print_level) {
        printf("%s", logbuf);
    }

    if (priority <= mproxy.syslog_level) {
        openlog("IMPROXY", LOG_NDELAY, LOG_USER);
        syslog(LOG_WARNING, "%s", logbuf);
        closelog();
    }

    return ;
}

char* imp_inet_ntoa(unsigned long ip)
{
    static char buf[INET_ADDRSTRLEN * 10] = {0};
    static char *p = buf;
    char *res = NULL;
    struct in_addr sin;

    sin.s_addr = ip;

    memset(p, 0, INET_ADDRSTRLEN);
    strcpy(p, inet_ntoa(sin));
    res = p;

    p += INET_ADDRSTRLEN;
    if ((p - buf ) >= (INET_ADDRSTRLEN * 10))
        p = buf;

    return res;

}
char* imp_inet6_ntoa(unsigned char *in6)
{
    static char ipbuf[INET6_ADDRSTRLEN * 10];
    static char *ipStr = ipbuf;
    char *res;

    memset(ipStr, 0, INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, in6, ipStr, INET6_ADDRSTRLEN);
    res = ipStr;

    ipStr += INET6_ADDRSTRLEN;
    if ((ipStr - ipbuf) >= INET6_ADDRSTRLEN * 10)
        ipStr = ipbuf;

    return res;

}
/**************************************************************************
 * FUNCTION NAME : imp_pi_ntoa
 **************************************************************************
 * DESCRIPTION   :
 * convert pi_addr to string for print
 *************************************************************************/
char* imp_pi_ntoa(pi_addr *p_pia)
{
    if (p_pia->ss.ss_family == AF_INET)
        return imp_inet_ntoa(p_pia->v4.sin_addr.s_addr);
    else if (p_pia->ss.ss_family == AF_INET6)
        return imp_inet6_ntoa(p_pia->v6.sin6_addr.s6_addr);
    else
        return "Server error";
}

/**************************************************************************
 * FUNCTION NAME : imp_family_to_level
 **************************************************************************
 * DESCRIPTION   :
 * return protocol level base on family
 *************************************************************************/
int imp_family_to_level(int family)
{
    switch (family) {
    case AF_INET:
        return IPPROTO_IP;
    case AF_INET6:
        return IPPROTO_IPV6;
    default:
        IMP_LOG_ERROR("unknown family");
        return -1;
    }
}

pi_addr* imp_build_piaddr(int family, void* addr, pi_addr *p_pia)
{
    bzero(p_pia, sizeof(pi_addr));

    if (family == AF_INET) {
        p_pia->v4.sin_family = family;
        memcpy(&p_pia->v4.sin_addr, (struct in_addr*)addr, sizeof(struct in_addr));

    }else if (family == AF_INET6) {
        p_pia->v6.sin6_family = family;
        memcpy(&p_pia->v6.sin6_addr, (struct in6_addr*)addr, sizeof(struct in6_addr));

    }else {
        IMP_LOG_FATAL("unsupport family:%d\n", family);
        exit(1);
    }

    return p_pia;

}

#define NETUTILS_PATH_PROCNET_IFINET6    "/proc/net/if_inet6"
static unsigned long parse_hex(const char *p_str, unsigned char *p_inet)
{
    unsigned long len = 0;
    char *p = NULL;

    if (NULL == (p = (char*)p_str) )
        return 0;
    while (*p)
    {
        int tmp;
        if (p[1] == 0)
            return 0;
        if (sscanf(p, "%02x", &tmp) != 1)
            return 0;
        p_inet[len] = tmp;
        len++;
        p += 2;
    }
    return len;
}

STATUS imp_get_ip6_addr(char *p_ifname, struct in6_addr *p_addr)
{
    FILE *fp = NULL;
    char ipv6Addr[INET6_ADDRSTRLEN];
    unsigned int ifIndex;
    unsigned int prefixLen;
    unsigned int addrScope;
    unsigned int ifFlags;
    char ifName[IF_NAME_MAX_LEN];

    if ((p_ifname == NULL)  || (p_addr == NULL))
    {
        return STATUS_NOK;
    }

/* # cat /proc/net/if_inet6  (see net/ipv6/addrconf.c for more info)
 *  00000000000000000000000000000001 01 80 10 80 lo
 *  +------------------------------+ ++ ++ ++ ++ ++
 *  |                                |  |  |  |  |
 *  1                                2  3  4  5  6
 *
 *  1. IPv6 address displayed in 32 hexadecimal chars without colons as separator
 *  2. Netlink device number (interface index) in hexadecimal (see ip addr , too)
 *  3. Prefix length in hexadecimal
 *  4. Scope value (see kernel source include/net/ipv6.h and net/ipv6/addrconf.c for more info)
 *  5. Interface flags (see include/linux/rtnetlink.h and net/ipv6/addrconf.c for more info)
 *  6. Device name
*/
    if ((fp = fopen(NETUTILS_PATH_PROCNET_IFINET6, "r")) != NULL)
    {
        while (fscanf(fp, "%s %02x %02x %02x %02x %s\n",
                  ipv6Addr, &ifIndex, &prefixLen, &addrScope, &ifFlags, ifName) != EOF)
        {

            if ( strcmp(p_ifname, ifName) != 0 ) {
                /* Interface name does not match, try next */
                continue;
            }

            if ( strncmp(ipv6Addr, "fe80", strlen("fe80")) != 0) {
                /*we need find link-local address*/
                continue;
            }

            /* Convert IP address from text */
            parse_hex(ipv6Addr, p_addr->s6_addr);

            fclose(fp);
            return STATUS_OK;

        }
        fclose(fp);
    }

    return STATUS_NOK;
}

void get_all_host_addr(int family, pi_addr *p_addr)
{

    if (family == AF_INET) {

    unsigned long idst = htonl(ALL_HOST_GROUP);
    imp_build_piaddr(AF_INET, &idst, p_addr);
    } else if (family == AF_INET6) {

        struct in6_addr addr;
        if (inet_pton(AF_INET6, "FF02::1", &addr) == 1)
            imp_build_piaddr(AF_INET6, &addr, p_addr);
    }

    return;
}
pa_list* pa_list_build(void *p_src_list, int num_src, int family)
{
    pa_list *p_head = NULL;
    int i;
    int offset = 0;

    if(p_src_list == NULL || num_src == 0)
        return NULL;

    if(family == AF_INET)
        offset = sizeof(struct in_addr);
    else
        offset = sizeof(struct in6_addr);


    for(i = 0; i< num_src; i++) {
        pa_list *p_node = NULL;

        if((p_node = malloc(sizeof(pa_list))) == NULL) {

            IMP_LOG_FATAL("malloc failed");
            return NULL;
        }

        bzero(p_node, sizeof(pa_list));

        imp_build_piaddr(family, (char*)p_src_list + (i*offset), &p_node->addr);


        p_node->next = p_head;
        p_head = p_node;

    }
    return p_head;

}
pa_list* pa_list_dup(pa_list* p_src_list)
{
    pa_list *p_head = NULL, *p_node = NULL;

    for(p_node = p_src_list; p_node; p_node = p_node->next) {
        pa_list *p_new = NULL;
        if((p_new = malloc(sizeof(pa_list))) == NULL) {

            IMP_LOG_FATAL("malloc failed");
            return NULL;
        }
        memcpy(p_new, p_node, sizeof(pa_list));
        p_new->next = p_head;
        p_head = p_new;
    }

    return p_head;

}

/*new node should be in front*/
pa_list *pa_list_add(pa_list* p_head, pi_addr* p_addr)
{
    pa_list *p_new = NULL;
    if((p_new = malloc(sizeof(pa_list))) == NULL) {

        IMP_LOG_FATAL("malloc failed");
        return NULL;
    }

    memcpy(&p_new->addr, p_addr, sizeof(pi_addr));
    p_new->next = p_head;
    return p_new;
}
void pa_list_cleanup(pa_list** pp_head)
{
    pa_list *p_next = NULL;
    pa_list *p_head = *pp_head;
    for (; p_head; p_head = p_next) {

        p_next = p_head->next;
        free(p_head);
    }
    *pp_head = NULL;
    return;
}
pa_list* pa_list_find(pa_list *p_head, pi_addr *p_addr)
{

    while (p_head) {

        if(memcmp(&p_head->addr, p_addr, sizeof(pi_addr)) == 0)
            return p_head;
        p_head = p_head->next;
    }

    return NULL;

}
pa_list* pa_list_del(pa_list* p_head, pi_addr *p_addr)
{
    pa_list *p_node =NULL ;
    for (p_node = p_head; p_node; p_node = p_node->next) {

        if (memcmp(&p_node->addr, p_addr, sizeof(pi_addr)) == 0){

            pa_list *p_new_head = p_node == p_head ? p_node->next : p_head;
            free(p_node);
            return p_new_head;
        }

    }
    return p_head;
}

