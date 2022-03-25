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
#ifndef __IMP_UTILS_H_
#define __IMP_UTILS_H_
#include <sys/time.h>

#define IF_NAME_MAX_LEN 64

typedef enum {
    STATUS_NOK  = -1,
    STATUS_OK   = 0,
} STATUS;

typedef enum {
    FALSE = 0,
    TRUE  = 1,
} BOOL;

enum{
    LOG_PRI_FATAL,
    LOG_PRI_ERROR,
    LOG_PRI_WARNING,
    LOG_PRI_INFO,
    LOG_PRI_DEBUG,
    LOG_PRI_LOWEST,
};

/*protocol independent*/
typedef union _pi_addr {
    struct sockaddr_in      v4;
    struct sockaddr_in6     v6;
    struct sockaddr         sa;
    struct sockaddr_storage ss;
}pi_addr;
#define pi_family    ss.ss_family

typedef struct pa_list {
    struct pa_list     *next;
    pi_addr            addr;
}pa_list;

#define IMP_LOG_FATAL(format, args...) \
    logger_build(LOG_PRI_FATAL, "FATAL", __FUNCTION__, __LINE__, format, ##args)

#define IMP_LOG_ERROR(format, args...) \
    logger_build(LOG_PRI_ERROR, "ERROR", __FUNCTION__, __LINE__, format, ##args)

#define IMP_LOG_WARNING(format, args...) \
    logger_build(LOG_PRI_WARNING, "WARNING", __FUNCTION__, __LINE__, format, ##args)

#define IMP_LOG_INFO(format, args...) \
    logger_build(LOG_PRI_INFO, "INFO", __FUNCTION__, __LINE__, format, ##args)

#define IMP_LOG_DEBUG(format, args...) \
    logger_build(LOG_PRI_DEBUG, "DEBUG", __FUNCTION__, __LINE__, format, ##args)


/*************************************************************************/
int get_sysuptime(struct timeval* tm);
/**************************************************************************
 * FUNCTION NAME : in_cusm
 **************************************************************************
 * DESCRIPTION   :
 * calculate checksum
*************************************************************************/
unsigned short in_cusm(unsigned short* buf , int len);
void logger_build(unsigned char  priority, const char *header,
    const char *function_name, int line_num, const char *format, ...);
char* imp_inet_ntoa(unsigned long ip);
char* imp_inet6_ntoa(unsigned char *in6);
/**************************************************************************
 * FUNCTION NAME : imp_pi_ntoa
 **************************************************************************
 * DESCRIPTION   :
 * convert pi_addr to string for print
 *************************************************************************/
char* imp_pi_ntoa(pi_addr *p_pia);
/**************************************************************************
 * FUNCTION NAME : imp_family_to_level
 **************************************************************************
 * DESCRIPTION   :
 * return protocol level base on family
 *************************************************************************/
int imp_family_to_level(int family);
pi_addr* imp_build_piaddr(int family, void* addr, pi_addr *p_pia);
//#define NETUTILS_PATH_PROCNET_IFINET6    "/proc/net/if_inet6"
//static unsigned long parse_hex(const char *p_str, char *p_inet);

STATUS imp_get_ip6_addr(char *p_ifname, struct in6_addr *p_addr);

void get_all_host_addr(int family, pi_addr *p_addr);

pa_list* pa_list_build(void *p_src_list,int num_src,int family);

void pa_list_cleanup(pa_list **p_head);

pa_list* pa_list_find(pa_list *p_head, pi_addr *p_addr);

#if 0
pa_list* pa_list_cp_from_is(imp_source* p_source);
#endif
pa_list *pa_list_add(pa_list* p_head,pi_addr* p_addr);
pa_list* pa_list_dup(pa_list* p_src_list);
pa_list* pa_list_del(pa_list* p_head,pi_addr *p_addr);

#endif
