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

#ifndef __IMP_DATA_H_
#define __IMP_DATA_H_
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
#include "utils.h"
#include "timer.h"
#include "os.h"


#include "proxy.h"

/*************************************************************************
                            * DEFINES *
**************************************************************************
*/
#define INTERFACE_NAME_MAX_LEN 64

typedef enum {
    IM_DISABLE = 0,
    IM_IGMPv1  = 1,
    IM_IGMPv2_MLDv1   = 2,
    IM_IGMPv3_MLDv2  = 3,
} im_version;

typedef enum {
    INTERFACE_DOWNSTREAM,
    INTERFACE_UPSTREAM
} if_type;


typedef enum {
    GROUP_EXCLUDE,
    GROUP_INCLUDE,
    GROUP_UNDEFINED
} group_type;

enum {
    BLOCKING,
    FORWARDING,
};


/*************************************************************************
                            * DATA TYPES *
**************************************************************************
*/
struct imp_interface;
struct imp_group;
/**/
typedef struct imp_source  {
    LIST_ENTRY(imp_source)  link;
    pi_addr                 src_addr;
    imp_timer              *timer;
    struct imp_group       *parent_gp;
    int                     times;
}imp_source;


/*group */
typedef struct imp_group{
    LIST_ENTRY(imp_group)   link;
    int                     if_index;
    struct imp_interface   *parent_if;
    pi_addr                 group_addr;
    group_type              type;
    im_version              version;
    imp_timer              *timer; /*group timer*/
    imp_timer              *v1_timer;/*IGMPv1/MLDv1 Host Present timer*/
    imp_timer              *v2_timer;/*IGMPv2 Host Present timer*/
    imp_timer              *sch_timer;
    int                     gs_sch_times;/*group specific timer*/
    LIST_HEAD(, imp_source) src_list;

}imp_group;


/* interface */
typedef struct imp_interface{
    LIST_ENTRY(imp_interface)   link;
    char                        if_name[INTERFACE_NAME_MAX_LEN];
    pi_addr                     if_addr;
    if_type                     type;
    int                         if_index;
    int                         if_igmp_socket;
    int                         if_mld_socket;
    int                         if_mtu;
    int                         startup;
    LIST_HEAD(, imp_group)      group_list;
    imp_timer                  *gq_timer;

}imp_interface;

/*************************************************************************
                            * FUNCTION PROTOTYPES *
**************************************************************************
*/
imp_interface* imp_interface_first(void);
void imp_interface_init(void);
STATUS init_interface(mcast_proxy *p_mproxy);
void imp_interface_create(char *p_ifname, int family, if_type type);
imp_interface* imp_interface_find(int family, if_type type);
void add_all_vif(void);
void del_all_vif(void);
void imp_interface_cleanup(imp_interface  *p_if);
void imp_interface_cleanup_all(void);

imp_group* imp_group_find(imp_interface *p_if, pi_addr *p_pig);
/*-----------------------------------------------------------------------
 * Name         : imp_group_create
 *
 * Brief        : create a new group and add it into interface's group list
 * Params       : [in] p_if     --
                   [in] p_pig   -- group's address
                     [in] version -- igmp/mld's version
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_group* imp_group_create(imp_interface *p_if, pi_addr *p_pig,
    pa_list* p_src_list,group_type type,im_version version);
/*-----------------------------------------------------------------------
 * Name         : imp_group_version_timer_update
 *
 * Brief        : update version timer
 * Params       : [in] p_gp    -- whose version timer need to be update
                   [in] version -- igmp/mld's version
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_group_version_timer_update(imp_group *p_gp, im_version version);
void imp_group_print(imp_interface *p_if);
/*-----------------------------------------------------------------------
 * Name         : imp_group_exist_scheduled
 *
 * Brief        : find if there exist shcheduled event in group
 * Params       : [in] p_gp -- group
 * Return       : 1:exist  0:not exist
*------------------------------------------------------------------------
*/
int imp_group_exist_scheduled(imp_group *p_gp);

/*-----------------------------------------------------------------------
 * Name         : group_timer_handler
 *
 * Brief        : group timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_timer_handler(void *data);
/*-----------------------------------------------------------------------
 * Name         : group_v1_timer_handler
 *
 * Brief        : group IGMPv1/MLDv1 version timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_v1_timer_handler(void* data);
/*-----------------------------------------------------------------------
 * Name         : group_v2_timer_handler
 *
 * Brief        : group IGMPv2 version timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_v2_timer_handler(void* data);
/*-----------------------------------------------------------------------
 * Name         : imp_group_cleanup
 *
 * Brief        : group IGMPv2 version timer expired
 * Params       : [in] p_gp -- imp_group need to be free
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_group_cleanup(imp_group *p_gp);

/*-----------------------------------------------------------------------
 * Name         : imp_source_find
 *
 * Brief        : find source member which is allowed or blocked in group
 * Params       : [in] p_gp    -- group
                   [in] p_addr  -- source member's address
                   [in] forward -- 0:find blocked source member
                                     1:find allowed source member
 * Return       : point to source member
*------------------------------------------------------------------------
*/
imp_source* imp_source_find(imp_group *p_gp,pi_addr *p_addr,int forward);
/*-----------------------------------------------------------------------
 * Name         : imp_source_exist_allow
 *
 * Brief        : check if there exsit source which is allowed in group
 * Params       : [in] p_gp -- struct imp_group
 * Return       : 1:exsit 0:not exsit
*------------------------------------------------------------------------
*/
int imp_source_exist_allow(imp_group *p_gp);
/*-----------------------------------------------------------------------
 * Name         : imp_source_exist_block
 *
 * Brief        : check if there exsit source which is blocked in group
 * Params       : [in] p_gp -- struct imp_group
 * Return       : 1:exsit 0:not exsit
*------------------------------------------------------------------------
*/
int imp_source_exist_block(imp_group *p_gp);
/*-----------------------------------------------------------------------
 * Name         : imp_source_is_scheduled
 *
 * Brief        : check if source member is scheduled to be inlcuded
                     group source specific query
 * Params       : [in] p_is -- source member
                   [in] sflag -- 1:member's timer is greater than LMQT
                                   0:member's timer is less than LMQT
 * Return       : 1:exsit 0:not exsit
*------------------------------------------------------------------------
*/
int imp_source_is_scheduled(imp_source  *p_is, int sflag);
imp_source* imp_source_find_scheduled(imp_group *p_gp,int sflag);
/*-----------------------------------------------------------------------
 * Name         : imp_source_create
 *
 * Brief        : create a source member and add it into group's source list
 * Params       : [in] p_gp    -- group
                   [in] p_addr  -- source member's address
                   [in] forward -- if this source is allowed or not
 * Return       : point to source member
*------------------------------------------------------------------------
*/
imp_source* imp_source_create(imp_group *p_gp,pi_addr *p_addr,int forward);
/*-----------------------------------------------------------------------
 * Name         : imp_source_cleanup
 *
 * Brief        : clean a source member
 * Params       : [in] p_is  -- source member
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_source_cleanup(imp_source  *p_is);
imp_timer* source_timer_handler(void *data);

#endif
