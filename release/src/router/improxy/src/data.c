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

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "data.h"
#include "queue.h"
#include "os.h"
#include "kernel_api.h"

#include "membership.h"

LIST_HEAD(, imp_interface) ifp_head;


imp_interface* imp_interface_first(void)
{
    return LIST_FIRST(&ifp_head);
}
void imp_interface_init(void)
{
    LIST_INIT(&ifp_head);
}
void imp_interface_create(char *p_ifname, int family, if_type type)
{
    imp_interface* p_if = NULL;
    if((p_if = (imp_interface*)malloc(sizeof(imp_interface))) == NULL)
    {
        perror(__FUNCTION__);
        exit(1);
    }
    memset(p_if, 0, sizeof(imp_interface));
    strcpy(p_if->if_name, p_ifname);

    p_if->if_addr.ss.ss_family = family;
    p_if->type = type;

    LIST_INSERT_HEAD(&ifp_head, p_if, link);

    return;
}

imp_interface* imp_interface_find(int family, if_type type)
{
    imp_interface    *p_if = imp_interface_first();

    for(; p_if; p_if = LIST_NEXT(p_if, link)){

        if(p_if->if_addr.ss.ss_family == family &&
            p_if->type == type)
            return p_if;
    }
    return NULL;

}

void imp_interface_cleanup(imp_interface  *p_if)
{
    imp_group *p_gp;
    p_gp = LIST_FIRST(&p_if->group_list);
    while(p_gp)
    {
        imp_group_cleanup(p_gp);
        p_gp = LIST_FIRST(&p_if->group_list);
    }
    LIST_REMOVE(p_if, link);
    free(p_if);

    return;
}
void imp_interface_cleanup_all(void)
{
    imp_interface     *p_if;

    p_if = LIST_FIRST(&ifp_head);
    while(p_if)
    {
        imp_interface_cleanup(p_if);
        p_if = LIST_FIRST(&ifp_head);
    }
    return;
}

static void imp_interface_cleanup_family(int family)
{
    imp_interface    *p_if;

    p_if = imp_interface_find(family, INTERFACE_UPSTREAM);
    if(p_if) {

        imp_interface_cleanup(p_if);
    }
    p_if = imp_interface_find(family, INTERFACE_DOWNSTREAM);
    while(p_if) {

        imp_interface_cleanup(p_if);
        p_if = imp_interface_find(family, INTERFACE_DOWNSTREAM);
    }
    return;

}

void add_all_vif(void)
{
    imp_interface* p_if;
    for(p_if = LIST_FIRST(&ifp_head);p_if;p_if = LIST_NEXT(p_if, link))
    {
        if (p_if->if_addr.ss.ss_family == AF_INET)
            k_add_ip4_vif(get_igmp_mld_socket(AF_INET), &p_if->if_addr.v4.sin_addr, p_if->if_index);

        if (p_if->if_addr.ss.ss_family == AF_INET6)
             k_add_ip6_mif(get_igmp_mld_socket(AF_INET6), p_if->if_index);
    }
}
void del_all_vif(void)
{
    imp_interface* p_if;
    for(p_if = LIST_FIRST(&ifp_head);p_if;p_if = LIST_NEXT(p_if, link))
    {

        if (p_if->if_addr.ss.ss_family == AF_INET)
            k_del_ip4_vif(get_igmp_mld_socket(AF_INET), p_if->if_index);
        if (p_if->if_addr.ss.ss_family == AF_INET6)
            k_del_ip6_mif(get_igmp_mld_socket(AF_INET6), p_if->if_index);
    }
}
STATUS init_interface(mcast_proxy *p_mproxy)
{
    imp_interface *p_if;
    imp_interface *ifp_next;

    for(p_if = LIST_FIRST(&ifp_head);p_if;p_if = ifp_next)
    {
        struct ifreq    ifr;
        int fd = 0;
        struct sockaddr *sa = NULL;

        ifp_next = LIST_NEXT(p_if, link);

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(fd == -1)
        {
            IMP_LOG_ERROR("socket error: %s\n", strerror(errno));
            return STATUS_NOK;
        }
        memset(&ifr, 0, sizeof(struct ifreq));
        strcpy(ifr.ifr_name, p_if->if_name);


        if(p_if->if_addr.ss.ss_family == AF_INET)
        {

            if(ioctl(fd, SIOCGIFADDR, (caddr_t)&ifr, sizeof(struct ifreq)) < 0){
                IMP_LOG_ERROR("SIOCGIFADDR filed %s\n", ifr.ifr_name);
                goto error;
            }
            sa = &ifr.ifr_addr;

            imp_build_piaddr(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), &p_if->if_addr);
            IMP_LOG_DEBUG("%s's ipv4 address is %s\n", p_if->if_name, imp_pi_ntoa(&p_if->if_addr));

        } else {

            struct in6_addr in6;
            if(imp_get_ip6_addr(p_if->if_name, &in6) == STATUS_NOK) {

                IMP_LOG_ERROR("interface %s don't have ipv6 address\n", p_if->if_name);
                goto error;
            }
            imp_build_piaddr(AF_INET6, &in6, &p_if->if_addr);
            IMP_LOG_DEBUG("%s's ipv6 address is %s\n", p_if->if_name, imp_pi_ntoa(&p_if->if_addr));
        }

        if(ioctl(fd, SIOCGIFINDEX, (caddr_t)&ifr, sizeof(struct ifreq)) < 0){
            goto error;
        }
        IMP_LOG_DEBUG("if_index = %d\n", ifr.ifr_ifindex);

        p_if->if_index = ifr.ifr_ifindex;


        if (p_if->type == INTERFACE_UPSTREAM)
            p_mproxy->upif_index = p_if->if_index;

         if(ioctl(fd, SIOCGIFMTU, (caddr_t)&ifr, sizeof(struct ifreq)) < 0){
            goto error;
        }

        p_if->if_mtu = ifr.ifr_mtu;
        IMP_LOG_DEBUG("if_mtu = %d\n", p_if->if_mtu);


        if(ioctl(fd, SIOCGIFFLAGS, (caddr_t)&ifr, sizeof(struct ifreq)) < 0){
            goto error;
        }
        IMP_LOG_DEBUG("if_falgs = %d\n", ifr.ifr_flags);

        close(fd);
        continue;

    error:
        close(fd);
        perror(__FUNCTION__);
        LIST_REMOVE(p_if, link);
        free(p_if);
    }

    if (p_mproxy->igmp_version == IM_DISABLE ||
        imp_interface_find(AF_INET, INTERFACE_UPSTREAM) == NULL ||
        imp_interface_find(AF_INET, INTERFACE_DOWNSTREAM) == NULL) {

        imp_interface_cleanup_family(AF_INET);
        if (p_mproxy->igmp_version != IM_DISABLE)
            IMP_LOG_ERROR("Disable IGMP, there is not igmp upstream interface or"
                " igmp downstream interface\n");
        p_mproxy->igmp_version = IM_DISABLE;
    }

    if (p_mproxy->mld_version == IM_DISABLE||
        imp_interface_find(AF_INET6, INTERFACE_UPSTREAM) == NULL||
        imp_interface_find(AF_INET6, INTERFACE_DOWNSTREAM) == NULL) {


        imp_interface_cleanup_family(AF_INET6);
        if(p_mproxy->mld_version != IM_DISABLE)
            IMP_LOG_ERROR("Disable MLD, there is not mld upstream interface or"
                " mld downstream interface\n");
        p_mproxy->mld_version = IM_DISABLE;
    }
    if(p_mproxy->igmp_version == IM_DISABLE &&
        p_mproxy->mld_version == IM_DISABLE) {

        IMP_LOG_FATAL("exiting.....\n");
        return STATUS_NOK;
    }

    return STATUS_OK;

}

/*-----------------------------------------------------------------------
 * Name         : imp_group_find
 *
 * Brief        : find group in p_if
 * Params       : [in] p_if    -- interface
                   [in] p_pig   -- group's address

 * Return       : if found, return imp_group or NULL
*------------------------------------------------------------------------
*/

imp_group* imp_group_find(imp_interface *p_if, pi_addr *p_pig)
{
    imp_group *p_gp;
    for (p_gp = LIST_FIRST(&p_if->group_list);p_gp;
        p_gp =  LIST_NEXT(p_gp, link)) {

        if (memcmp(&p_gp->group_addr, p_pig, sizeof(pi_addr)))
            continue;
        return p_gp;
    }
    return NULL;
}

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
    pa_list* p_src_list, group_type type, im_version version)
{
    imp_group *p_gp = malloc(sizeof(imp_group));
    pa_list *p_node = NULL;

    if (p_gp == NULL) {

        IMP_LOG_FATAL("malloc fail");
        exit(1);
    }

    bzero(p_gp, sizeof(*p_gp));

    p_gp->parent_if = p_if;
    p_gp->type = type;
    p_gp->version = version;
    memcpy(&p_gp->group_addr, p_pig, sizeof(pi_addr));

    /*
     *    The group timer is only used when a group is in EXCLUDE mode and it
        *    represents the time for the *filter-mode* of the group to expire and
        *    switch to INCLUDE mode.
        *    rfc 3376 [6.2.2]
        */
    if (p_gp->type == GROUP_EXCLUDE) {

        p_gp->timer = imp_add_timer(group_timer_handler, p_gp);
        imp_set_timer(TIMER_GMI, p_gp->timer);
    }
    imp_group_version_timer_update(p_gp, version);

    if (imp_membership_db_find(p_pig) == NULL) {

        /*add membership entry*/
        imp_membership_db_add(p_pig, p_src_list, type);
    }

    for(p_node = p_src_list; p_node; p_node = p_node->next) {

        imp_source_create(p_gp, &p_node->addr, type);
    }

    imp_membership_db_turnon_update(&p_gp->group_addr);

    k_mcast_join(&p_gp->group_addr, NULL);

    LIST_INSERT_HEAD(&p_if->group_list, p_gp, link);

    return p_gp;

}
/*-----------------------------------------------------------------------
 * Name         : imp_group_version_timer_update
 *
 * Brief        : update version timer
 * Params       : [in] p_gp    -- whose version timer need to be update
                   [in] version -- igmp/mld's version
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_group_version_timer_update(imp_group *p_gp, im_version version)
{
    imp_timer **p_mt = NULL;
    imp_timer *(*func) (void*) = NULL;

    if (version == IM_IGMPv3_MLDv2) {

    /* The Group Compatibility Mode of a group record changes whenever an
     * older version report (than the current compatibility mode) is heard
     * or when certain timer conditions occur.
     * [RFC3376 Section 7.3.2]
    */
        return;
    }

    if (version == IM_IGMPv1) {

        p_mt = &p_gp->v1_timer;
        func = group_v1_timer_handler;
    } else if (version == IM_IGMPv2_MLDv1) {

        p_mt = &p_gp->v2_timer;
        func = group_v2_timer_handler;
    }

    if (*p_mt == NULL)
        *p_mt  = imp_add_timer(func, p_gp);
    imp_set_timer(TIMER_OHPI, *p_mt);

    if (version < p_gp->version)
        p_gp->version = version;
    return;

}


void imp_group_print(imp_interface *p_if)
{
    imp_group *p_gp = LIST_FIRST(&p_if->group_list);
    struct timeval now;

    printf("\ninterface %s:%s\n\n", p_if->if_name, imp_pi_ntoa(&p_if->if_addr));
    while(p_gp) {

        get_sysuptime(&now);
        printf("group %s %s\n", imp_pi_ntoa(&p_gp->group_addr),
            p_gp->type == GROUP_EXCLUDE?"exclude mode":"include mode");
        if(p_gp->timer)
            printf("\tgroup time %ld\n", p_gp->timer->tm.tv_sec - now.tv_sec);
        printf("\tversion: %d\n", p_gp->version);
        {

            imp_source *p_is = NULL;
            for(p_is = LIST_FIRST(&p_gp->src_list);p_is;
                p_is = LIST_NEXT(p_is, link)){
                printf("\t\taddree: %s %s %ld\n", imp_pi_ntoa(&p_is->src_addr),
                    TIMEVAL_ISZERO(p_is->timer->tm)?"block":"allow",
                    p_is->timer->tm.tv_sec? p_is->timer->tm.tv_sec -now.tv_sec:0);

            }
        }
        p_gp = LIST_NEXT(p_gp, link);
    }

}
/*-----------------------------------------------------------------------
 * Name         : imp_group_exist_scheduled
 *
 * Brief        : find if there exist shcheduled event in group
 * Params       : [in] p_gp -- imp_group
 * Return       : 1:exist  0:not exist
*------------------------------------------------------------------------
*/
int imp_group_exist_scheduled(imp_group *p_gp)
{
    if(p_gp->gs_sch_times != 0 ||
        imp_source_find_scheduled(p_gp, 0) != NULL ||
        imp_source_find_scheduled(p_gp, 1) != NULL)
        return 1;

    return 0;
}

/*-----------------------------------------------------------------------
 * Name         : group_timer_handler
 *
 * Brief        : group timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_timer_handler(void *data)
{
    imp_group    *p_gp = data;
    pi_addr       pa;

    /*    If all source timers have expired then delete Group Record.
     *     If there are still source record timers running, switch to
     *    INCLUDE filter-mode using those source records with running
     *    timers as the INCLUDE source record state.
     *     rfc 3367 [6.2.2]
     */

    memcpy(&pa, &p_gp->group_addr, sizeof(pa));

    if (imp_source_exist_allow(p_gp) == 0) {

        IMP_LOG_DEBUG("cleanup group %s\n", imp_pi_ntoa(&pa));
        imp_group_cleanup(p_gp);

    } else {

        IMP_LOG_DEBUG("group %s timeout, will contive to include mode\n", imp_pi_ntoa(&pa));

        p_gp->type = GROUP_INCLUDE;
        imp_remove_timer(&p_gp->timer);
        imp_membership_db_turnon_update(&pa);
    }

    imp_membership_db_update(&pa);
    return NULL;
}
/*-----------------------------------------------------------------------
 * Name         : group_v1_timer_handler
 *
 * Brief        : group IGMPv1/MLDv1 version timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_v1_timer_handler(void* data)
{
    imp_group* p_gp = data;

    IMP_LOG_DEBUG("group %s IGMPv1/MLDv1 version timer expired\n", imp_pi_ntoa(&p_gp->group_addr));

    /* When the IGMPv1 Host Present
     * timer expires, a router switches to Group Compatibility mode of
     * IGMPv2 if it has a running IGMPv2 Host Present timer.  If it does not
     * have a running IGMPv2 Host Present timer then it switches to Group
     * Compatibility of IGMPv3.
     * [RFC3376 Section 7.3.2]
    */
    if (p_gp->v2_timer) {

        p_gp->version = IM_IGMPv2_MLDv1;
    }
    else {

        /*back to IGMPv3 mode*/
        p_gp->version = IM_IGMPv3_MLDv2;
    }
    imp_remove_timer(&p_gp->v1_timer);

    return NULL;
}
/*-----------------------------------------------------------------------
 * Name         : group_v2_timer_handler
 *
 * Brief        : group IGMPv2 version timer expired
 * Params       : [in] data -- struct imp_group
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* group_v2_timer_handler(void* data)
{
    imp_group* p_gp = data;
    IMP_LOG_DEBUG("group %s IGMPv2 version timer expired\n", imp_pi_ntoa(&p_gp->group_addr));

   /* When the IGMPv2 Host Present timer expires
    * and the IGMPv1 Host Present timer is not running, a router switches
    * to Group Compatibility mode of IGMPv3.
    * [RFC3376 Section 7.3.2]
   */
    if (p_gp->v1_timer == NULL) {

        /*back to IGMPv3 mode*/
        p_gp->version = IM_IGMPv3_MLDv2;
    }
    imp_remove_timer(&p_gp->v2_timer);

    return NULL;
}

/*-----------------------------------------------------------------------
 * Name         : imp_group_cleanup
 *
 * Brief        : group IGMPv2 version timer expired
 * Params       : [in] p_gp -- imp_group need to be free
 * Return       : NULL
*------------------------------------------------------------------------
*/

void imp_group_cleanup(imp_group *p_gp)
{
    imp_source *p_is = NULL;

    if (p_gp->timer != NULL)
        imp_remove_timer(&p_gp->timer);

    if (p_gp->sch_timer != NULL)
        imp_remove_timer(&p_gp->sch_timer);

    if (p_gp->v1_timer != NULL)
        imp_remove_timer(&p_gp->v1_timer);

    if (p_gp->v2_timer != NULL)
        imp_remove_timer(&p_gp->v2_timer);

    p_is = LIST_FIRST(&p_gp->src_list);
    while(p_is) {

        imp_source_cleanup(p_is);
        p_is = LIST_FIRST(&p_gp->src_list);
    }
    LIST_REMOVE(p_gp, link);

    imp_membership_db_turnon_update(&p_gp->group_addr);
    free(p_gp);
}
/*source*/
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
imp_source* imp_source_find(imp_group *p_gp, pi_addr *p_addr, int forward)
{
    imp_source  *p_is = NULL;

    for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
            p_is = LIST_NEXT(p_is, link)) {

        if(forward == FORWARDING && TIMEVAL_ISZERO(p_is->timer->tm))
            continue;
        if(forward == BLOCKING && !TIMEVAL_ISZERO(p_is->timer->tm))
            continue;
        if(memcmp(p_addr, &p_is->src_addr, sizeof(pi_addr)) == 0) {
            return p_is;
        }
    }
    return NULL;

}
/*-----------------------------------------------------------------------
 * Name         : imp_source_exist_allow
 *
 * Brief        : check if there exsit source which is allowed in group
 * Params       : [in] p_gp -- struct imp_group
 * Return       : 1:exsit 0:not exsit
*------------------------------------------------------------------------
*/
int imp_source_exist_allow(imp_group *p_gp)
{
    imp_source  *p_is = NULL;

    for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
            p_is = LIST_NEXT(p_is, link)) {

        if(!TIMEVAL_ISZERO(p_is->timer->tm))
            return 1;

    }
    return 0;

}
/*-----------------------------------------------------------------------
 * Name         : imp_source_exist_block
 *
 * Brief        : check if there exsit source which is blocked in group
 * Params       : [in] p_gp -- struct imp_group
 * Return       : 1:exsit 0:not exsit
*------------------------------------------------------------------------
*/
int imp_source_exist_block(imp_group *p_gp)
{
    imp_source  *p_is = NULL;

    for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
            p_is = LIST_NEXT(p_is, link)) {

        if(TIMEVAL_ISZERO(p_is->timer->tm))
            return 1;
    }
    return 0;

}
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
int imp_source_is_scheduled(imp_source  *p_is, int sflag)
{
    struct timeval lmqt;

    if(TIMEVAL_ISZERO(p_is->timer->tm) || p_is->times == 0 )
        return 0;

    imp_time_val(TIMER_LMQT, &lmqt);

    if(sflag == 1 && TIMEVAL_LT(lmqt, p_is->timer->tm))
        return 1;
    else if (sflag == 0 && TIMEVAL_LEQ(p_is->timer->tm, lmqt)) {
        return 1;
    }

    return 0;

}

imp_source* imp_source_find_scheduled(imp_group *p_gp, int sflag)
{
    imp_source  *p_is = NULL;

    for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
            p_is = LIST_NEXT(p_is, link)) {

        if (imp_source_is_scheduled(p_is, sflag) != 0)
            return p_is;;

    }
    return NULL;

}
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
imp_source* imp_source_create(imp_group *p_gp, pi_addr *p_addr, int forward)
{
    imp_source  *p_is = NULL;

    p_is = malloc(sizeof(imp_source));

    if(p_is == NULL) {

        IMP_LOG_FATAL("malloc faild:%s\n", strerror(errno));
        return NULL;
    }

    bzero(p_is, sizeof(pi_addr));
    memcpy(&p_is->src_addr, p_addr, sizeof(pi_addr));
    p_is->parent_gp = p_gp;
    p_is->timer = imp_add_timer(source_timer_handler, p_is);

    if(forward)
        imp_set_timer(TIMER_GMI, p_is->timer);
    else
        bzero(&p_is->timer->tm, sizeof(struct timeval));

    LIST_INSERT_HEAD(&p_gp->src_list, p_is, link);
    imp_membership_db_turnon_update(&p_gp->group_addr);
    return p_is;

}
/*-----------------------------------------------------------------------
 * Name         : imp_source_cleanup
 *
 * Brief        : clean a source member
 * Params       : [in] p_is  -- source member
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_source_cleanup(imp_source  *p_is)
{
    imp_group  *p_gp = p_is->parent_gp;

    imp_membership_db_turnon_update(&p_gp->group_addr);
    if(p_is->timer)
        imp_remove_timer(&p_is->timer);

    LIST_REMOVE(p_is, link);
    free(p_is);

}
/*-----------------------------------------------------------------------
 * Name         : source_timer_handler
 *
 * Brief        : source timer expired
 * Params       : [in] data -- struct imp_source
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* source_timer_handler(void *data)
{
    imp_source *p_is = data;
    imp_group  *p_gp = p_is->parent_gp;
    pi_addr     pa;

    memcpy(&pa, &p_gp->group_addr, sizeof(pa));

    IMP_LOG_DEBUG("source %s timeout, free it\n", imp_pi_ntoa(&p_is->src_addr));

    imp_source_cleanup(p_is);

    if(p_gp->type == GROUP_INCLUDE && imp_source_exist_allow(p_gp) == 0){

        /* If there are no more source records for the group, delete group record.
         * rfc 3376 [6.3]
         */
        imp_group_cleanup(p_gp);
    }
    imp_membership_db_update(&pa);
    return NULL;
}


