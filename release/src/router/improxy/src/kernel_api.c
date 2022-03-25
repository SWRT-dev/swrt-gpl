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

#include "proxy.h"
#include <netinet/in.h>
#include <linux/mroute.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <net/if.h>
#include "utils.h"
#include "proxy.h"
#include "kernel_api.h"

static int mif2phyif[MAXMIFS];
static int vif2phyif[MAXVIFS];

#ifdef VIF_NOT_EQUAL_MIF
static int get_vifi_ipv4(int ifindex)
{
    int i;
    for (i = 0; i < MAXVIFS; i++) {
        /* found already allocated one */
        if (vif2phyif[i] == ifindex) {
            return i;
        }

        /* you have seeked all the registerd mifs */
        if (vif2phyif[i] == 0) {
            vif2phyif[i] = ifindex;
            return i;
        }
    }
    return 0;
}

static int get_mifi_ipv6(int ifindex)
{
    int i;
    for (i = 0; i < MAXMIFS; i++) {
        /* found already allocated one */
        if (mif2phyif[i] == ifindex) {

            return i;
        }

        /* you have seeked all the registerd mifs */
        if (mif2phyif[i] == 0) {

            mif2phyif[i] = ifindex;
            return i;
        }
    }
    return 0;
}


static int get_rifi_ipv4(int vif)
{
    if (vif > MAXMIFS - 1)
        return 0;
    return vif2phyif[vif];

}

static int get_rifi_ipv6(int mif)
{
    if (mif > MAXMIFS - 1)
        return 0;
    return mif2phyif[mif];
}

#endif
int k_get_vmif(int ifindex, int family)
{
    if (family == AF_INET) {
   #ifdef VIF_NOT_EQUAL_MIF
        return get_vifi_ipv4(ifindex);
   #else
        return ifindex;
   #endif

    } else if (family == AF_INET6) {
   #ifdef VIF_NOT_EQUAL_MIF
        return get_mifi_ipv6(ifindex);
   #else
        return ifindex;
   #endif
    }

    return 0;
}

/*get real interface index*/
int k_get_rlif(int vif, int family)
{
    if (family == AF_INET) {
   #ifdef VIF_NOT_EQUAL_MIF
        return get_rifi_ipv4(vif);
   #else
        return vif;
   #endif

    } else if (family == AF_INET6) {
   #ifdef VIF_NOT_EQUAL_MIF
        return get_rifi_ipv6(vif);
   #else
        return vif;
   #endif
    }
   
    return 0;
}

void k_add_ip4_vif (int socket, struct in_addr* sin, int ifindex)
{
    struct vifctl vif;
    memset(&vif, 0, sizeof(vif));
    memcpy(&vif.vifc_lcl_addr, sin, sizeof(struct in_addr));

    vif.vifc_vifi = k_get_vmif(ifindex, AF_INET);

    vif.vifc_threshold = 1;
    IMP_LOG_DEBUG("vif.vifc_vifi = %d %s\n", vif.vifc_vifi, inet_ntoa(vif.vifc_lcl_addr));
    if (setsockopt(socket, IPPROTO_IP, MRT_ADD_VIF, &vif, sizeof(struct vifctl)) < 0 )
        perror("MRT_ADD_VIF failed:");
}
void k_del_ip4_vif (int socket, int ifindex)
{
    struct vifctl vif;
    memset(&vif, 0, sizeof(vif));


    vif.vifc_vifi = k_get_vmif(ifindex, AF_INET);

    if (setsockopt(socket, IPPROTO_IP, MRT_DEL_VIF, &vif, sizeof(struct vifctl)) < 0 )
        perror("MRT_ADD_VIF failed:");
}

STATUS k_start4_mproxy(int socket)
{
    int d = 1;
    if (setsockopt(socket, IPPROTO_IP, MRT_INIT, &d, sizeof(int)) < 0) {
        perror("MRT_INIT");
        return STATUS_NOK;
    }

    bzero(vif2phyif, sizeof(vif2phyif));
    vif2phyif[0] = -1;
    return STATUS_OK;
}

void k_stop4_mproxy(int socket)
{
    if (setsockopt(socket, IPPROTO_IP, MRT_DONE, NULL, 0) < 0)
        perror("MRT_DONE");
}


static STATUS k_add_ip4_mfc(int socket, int iif_index, pi_addr *p_mcastgrp
    ,pi_addr *p_origin, if_set *p_ttls)
{
    struct mfcctl mfc;
    int i;

    memset(&mfc, 0, sizeof(struct mfcctl));

    memcpy(&mfc.mfcc_origin, &p_origin->v4.sin_addr, sizeof(struct in_addr));
    memcpy(&mfc.mfcc_mcastgrp, &p_mcastgrp->v4.sin_addr, sizeof(struct in_addr));

    //mfc.mfcc_parent = k_get_vmif(get_up_if_index(), AF_INET);
    mfc.mfcc_parent = k_get_vmif(iif_index, AF_INET);

    IMP_LOG_DEBUG("upif_index = %d\n", get_up_if_index());
    for (i = 0;i < MAXVIFS;i++) {

        if (IF_ISSET(i, p_ttls))
           mfc.mfcc_ttls[i] = 1;
        else
           mfc.mfcc_ttls[i] = 0;
    }
    return setsockopt(socket, IPPROTO_IP, MRT_ADD_MFC, &mfc, sizeof(struct mfcctl));


}

static STATUS k_del_ip4_mfc(int socket, pi_addr *p_mcastgrp, pi_addr *p_origin)
{
    struct mfcctl mfc;

    memset(&mfc, 0, sizeof(struct mfcctl));
    memcpy(&mfc.mfcc_origin, &p_origin->v4.sin_addr, sizeof(struct in_addr));
    memcpy(&mfc.mfcc_mcastgrp, &p_mcastgrp->v4.sin_addr, sizeof(struct in_addr));

    return setsockopt(socket, IPPROTO_IP, MRT_DEL_MFC, &mfc, sizeof(struct mfcctl));

}



void k_add_ip6_mif (int socket, int ifindex)
{

    struct mif6ctl mif;
    mifi_t mifi;

    mifi = k_get_vmif(ifindex, AF_INET6);

    memset(&mif, 0, sizeof(mif));
    mif.mif6c_mifi= mifi;
    mif.mif6c_pifi= ifindex;
    mif.mif6c_flags = 0;
    IMP_LOG_DEBUG("vif.mif6c_mifi = %d mif6c_pifi %d\n", mif.mif6c_mifi, mif.mif6c_pifi);
    if (setsockopt(socket, IPPROTO_IPV6, MRT6_ADD_MIF, &mif, sizeof(struct mif6ctl)) < 0 )
        perror("MRT6_ADD_MIF failed:");
}
void k_del_ip6_mif (int socket, int ifindex)
{
    mifi_t mifi;

    mifi = k_get_vmif(ifindex, AF_INET6);

    if (setsockopt(socket, IPPROTO_IPV6, MRT6_DEL_MIF, &mifi, sizeof(mifi)) < 0 )
        perror("MRT6_DEL_MIF failed:");
}

STATUS k_start6_mproxy(int socket)
{

    int d = 1;
    if (setsockopt(socket, IPPROTO_IPV6, MRT6_INIT, &d, sizeof(int)) < 0) {

        perror("MRT6_INIT");
        return STATUS_NOK;
    }
    /* start vif/mif management */
    bzero(mif2phyif, sizeof(mif2phyif));
    mif2phyif[0] = -1;
    return STATUS_OK;
}

void k_stop6_mproxy(int socket)
{
    if (setsockopt(socket, IPPROTO_IPV6, MRT6_DONE, NULL, 0) < 0)
        perror("MRT6_DONE");
}


static STATUS k_add_ip6_mfc(int socket, int iif_index, pi_addr *p_mcastgrp
    , pi_addr *p_origin, if_set *p_ttls)
{

    struct mf6cctl mf6c;

    memset(&mf6c, 0, sizeof(mf6c));
    memcpy(&mf6c.mf6cc_origin, &p_origin->v6, sizeof(mf6c.mf6cc_origin));
    memcpy(&mf6c.mf6cc_mcastgrp, &p_mcastgrp->v6, sizeof(mf6c.mf6cc_mcastgrp));

    mf6c.mf6cc_parent = k_get_vmif(iif_index, AF_INET6);

    mf6c.mf6cc_ifset = *p_ttls;

    return setsockopt(socket, IPPROTO_IPV6, MRT6_ADD_MFC, &mf6c, sizeof(mf6c));

}

static STATUS k_del_ip6_mfc(int socket, pi_addr *p_mcastgrp, pi_addr *p_origin)
{

    struct mf6cctl mf6c;

    memset(&mf6c, 0, sizeof(mf6c));
    memcpy(&mf6c.mf6cc_origin, &p_origin->v6, sizeof(mf6c.mf6cc_origin));
    memcpy(&mf6c.mf6cc_mcastgrp, &p_mcastgrp->v6, sizeof(mf6c.mf6cc_mcastgrp));

    return setsockopt(socket, IPPROTO_IPV6, MRT6_DEL_MFC, &mf6c, sizeof(mf6c));
}

/*protocol independent code*/
STATUS k_mcast_join(pi_addr* p_addr, char* ifname)
{

    struct group_req req = {0};
    int socket = 0;

    if (ifname != NULL) {
        if ((req.gr_interface = if_nametoindex(ifname)) == 0) {

            IMP_LOG_ERROR("if_nametoindex failed");
            return STATUS_NOK;
        }
    }
    else
        req.gr_interface = get_up_if_index();/*if ifindex == 0, kernel will select interface*/
    if (ifname) IMP_LOG_DEBUG("ifname = %s ", ifname);
    IMP_LOG_DEBUG("req.gr_interface= %d\n", req.gr_interface);
    memcpy(&req.gr_group, p_addr, sizeof(req.gr_group));

    socket = get_udp_socket(p_addr->ss.ss_family);

    return setsockopt(socket, imp_family_to_level(p_addr->ss.ss_family),
        MCAST_JOIN_GROUP, &req, sizeof(struct group_req));
}

STATUS k_mcast_leave( pi_addr* p_addr, char* ifname)
{
    #if 1
    struct group_req req;
    int socket = 0;

    if (ifname != NULL) {
        if ((req.gr_interface = if_nametoindex(ifname)) == 0) {
            IMP_LOG_ERROR("if_nametoindex failed");
            return STATUS_NOK;
        }
    }
    else
        req.gr_interface = get_up_if_index();/*if ifindex == 0, kernel will select interface*/

    memcpy(&req.gr_group, p_addr, sizeof(req.gr_group));

    socket = get_udp_socket(p_addr->ss.ss_family);
    return setsockopt(socket, imp_family_to_level(p_addr->ss.ss_family), MCAST_LEAVE_GROUP,
        &req, sizeof(struct group_req));
    #else
    return k_mcast_msfilter(p_addr, NULL, MCAST_INCLUDE);
    #endif

}

STATUS k_add_mfc(int iif_index, pi_addr *p_mcastgrp , pi_addr *p_origin, if_set *p_ttls)
{
    int socket = 0;
    socket = get_igmp_mld_socket(p_mcastgrp->ss.ss_family);

    if (p_mcastgrp->ss.ss_family == AF_INET) {

        return k_add_ip4_mfc(socket, iif_index, p_mcastgrp, p_origin, p_ttls);
    } else {

        return k_add_ip6_mfc(socket, iif_index, p_mcastgrp, p_origin, p_ttls);
    }
}

STATUS k_del_mfc(pi_addr *p_mcastgrp, pi_addr *p_origin)
{
    int socket = 0;
    socket = get_igmp_mld_socket(p_mcastgrp->ss.ss_family);
    if (p_mcastgrp->ss.ss_family == AF_INET) {
        IMP_LOG_DEBUG("clean mfc %s %s\n", imp_pi_ntoa(p_origin), imp_pi_ntoa(p_mcastgrp));
        return k_del_ip4_mfc(socket, p_mcastgrp, p_origin);
    } else {

        return k_del_ip6_mfc(socket, p_mcastgrp, p_origin);
    }
}


#if 0
STATUS k_mcast_msfilter(pi_addr* p_addr, pa_list *p_addr_list, int fmode)
{

    struct group_filter* p_gf;
    pa_list *p_node = NULL;
    int socket = 0;
    int res = 0;

    p_gf = (struct group_filter*)malloc(sizeof(struct group_filter));
    if(p_gf == NULL) {
        IMP_LOG_ERROR("malloc failed\n");
        return STATUS_NOK;
    }

    bzero(p_gf, sizeof(struct group_filter));

    p_gf->gf_interface = get_up_if_index();/*if ifindex == 0, kernel will select interface*/
    p_gf->gf_fmode = fmode;
    memcpy(&p_gf->gf_group, p_addr, sizeof(p_gf->gf_group));
    IMP_LOG_DEBUG("p_gf->gf_interface = %d\n", p_gf->gf_interface);
    for(p_node = p_addr_list; p_node; p_node = p_node->next) {

        if(p_gf->gf_numsrc == 0) {

            memcpy(p_gf->gf_slist, &p_node->addr.ss, sizeof(struct sockaddr_storage));
        }else {

            struct group_filter *p_gf_tmp = NULL;
            p_gf_tmp = (struct group_filter*)realloc(p_gf,
                GROUP_FILTER_SIZE(p_gf->gf_numsrc) + sizeof(struct sockaddr_storage));

            if(p_gf_tmp == NULL) {
                IMP_LOG_ERROR("realloc failed\n");
                free(p_gf);
                return STATUS_NOK;
            }

            p_gf = p_gf_tmp;
            memcpy(&p_gf->gf_slist[p_gf->gf_numsrc], &p_node->addr.ss,
                sizeof(struct sockaddr_storage));

        }
        p_gf->gf_numsrc++;
    }

    socket = get_udp_socket(p_addr->ss.ss_family);

    res = setsockopt(socket, imp_family_to_level(p_addr->ss.ss_family),
        MCAST_MSFILTER, p_gf, GROUP_FILTER_SIZE(p_gf->gf_numsrc));
    if (res < 0)
        perror(__FUNCTION__);
    free(p_gf);
    return res;
}
#endif
