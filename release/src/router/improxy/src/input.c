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
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/igmp.h>
#include <netinet/ip.h>

#include <arpa/inet.h>
#include "proxy.h"
#include "timer.h"
#include "utils.h"
#include "input.h"
#include "handler.h"
#include "membership.h"
#include "kernel_api.h"

struct imp_multi_rec {
    unsigned char   rec_type;
    unsigned char   aux_len;
    unsigned short  src_num;
    struct in6_addr multi_addr;
    struct in6_addr src_list[0];
};

#define MLD_MODE_IS_INCLUDE        1
#define MLD_MODE_IS_EXCLUDE        2
#define MLD_CHANGE_TO_INCLUDE    3
#define MLD_CHANGE_TO_EXCLUDE    4
#define MLD_ALLOW_NEW_SOURCES    5
#define MLD_BLOCK_OLD_SOURCES    6


#define    IN_LOCAL_CONTROL_BLOCK(a)        ((((unsigned long)(a)) & 0xffffff00) == 0xe0000000)

#define    IN_LOCAL_SCOPE_BLOCK(a)        ((((unsigned long)(a)) & 0xEFFF0000) == 0xEFFF0000)

/**************************************************************************
 * FUNCTION NAME : imp_get_group_if_addr
 **************************************************************************
 * DESCRIPTION   :
 * Get group from p_if accroding to gourp ip information
 *************************************************************************/
static imp_group* imp_get_group_if_addr(imp_interface *p_if, pi_addr *pigp)
{
    imp_group *p_gp = LIST_FIRST(&p_if->group_list);
    if (p_if->type == INTERFACE_UPSTREAM)
        return NULL;
    while(p_gp) {
        if (memcmp(&p_gp->group_addr, pigp, sizeof( pi_addr)) == 0)
            return p_gp;
        p_gp = LIST_NEXT(p_gp, link);
    }
    return NULL;
}

static int imp_get_is_forward(imp_interface* p_if, pi_addr *pigp, pi_addr *pip)
{
    imp_source *msp = NULL;
    imp_group*    p_gp;
    struct timeval now;

    if (get_sysuptime(&now) == -1) {
        IMP_LOG_ERROR("can't get sysuptime\n");
        return 0;
    }

    p_gp = imp_get_group_if_addr(p_if, pigp);
    if (p_gp == NULL) {

        return 0;
    }


    if (p_gp->type == GROUP_INCLUDE) {
        for (msp = LIST_FIRST(&p_gp->src_list);msp;
            msp = LIST_NEXT(msp, link)) {

            if (memcmp(&msp->src_addr, pip, sizeof(pi_addr)) == 0
                && TIMEVAL_LT(now, msp->timer->tm))
                return 1;
        }
        return 0;
    }else if (p_gp->type == GROUP_EXCLUDE) {
        for (msp = LIST_FIRST(&p_gp->src_list);msp;
            msp = LIST_NEXT(msp, link)) {

            if (memcmp(&msp->src_addr, pip, sizeof(pi_addr)) == 0
                && TIMEVAL_LT(msp->timer->tm, now))
                return 0;
        }
        return 1;
    }
    return 0;
}

static int imp_get_mfcc_ttls(if_set *p_ttls, int length, pi_addr *pi_src,
    pi_addr *p_group, int iif_index)
{
    imp_interface *p_if = imp_interface_first();
    unsigned char flag = 0;
    int            vmif = 0;

    if (pi_src->ss.ss_family != p_group->ss.ss_family) {
        IMP_LOG_ERROR("unmatch family");
        return 0;
    }

    while(p_if) {
        if (p_if->type != INTERFACE_UPSTREAM &&
            imp_get_is_forward(p_if, p_group, pi_src) &&
            p_if->if_index < length &&
            p_if->if_index != iif_index) {
            
            vmif = k_get_vmif(p_if->if_index, p_group->ss.ss_family);
            IF_SET(vmif, p_ttls);
            flag = 1;
        }
        p_if = LIST_NEXT(p_if, link);
    }
    if (get_down_to_up_enable() == 1 && get_up_if_index() != iif_index) {

            vmif = k_get_vmif(get_up_if_index(), p_group->ss.ss_family);
            IF_SET(vmif, p_ttls);
            if (flag == 0) {
                
                k_add_mfc(iif_index, p_group, pi_src, p_ttls);
		   return 0;
            }
    }
    if (flag == 0)
        IMP_LOG_DEBUG("group %s isn't interest in packet from %s\n",
            imp_pi_ntoa(p_group), imp_pi_ntoa(pi_src));

    return flag;
}


static imp_interface* mcast_if_get_by_addr(pi_addr *p_pia)
{
    imp_interface *p_if = imp_interface_first();

    while(p_if) {

       if (memcmp(&p_if->if_addr, p_pia, sizeof(pi_addr)) == 0)
            return p_if;

        p_if = LIST_NEXT(p_if, link);
    }

    return NULL;
}

static int imp_verify_multicast_addr(pi_addr *p_pia)
{

    if(p_pia->ss.ss_family == AF_INET6) {

        if (!IN6_IS_ADDR_MULTICAST(p_pia->v6.sin6_addr.s6_addr)) {

            IMP_LOG_ERROR("Address %s isn't multicast address\n",
                           imp_pi_ntoa(p_pia));
            return -1;
        }
        /* Interface-Local scope spans only a single interface on a node
         * and is useful only for loopback transmission of multicast.
         *
         * Link-Local multicast scope spans the same topological region as
         * the corresponding unicast scope.
         * [RFC 4291 section 2.7]
         */

        if (IN6_IS_ADDR_MC_LINKLOCAL(p_pia->v6.sin6_addr.s6_addr) ||
            IN6_IS_ADDR_MC_NODELOCAL(p_pia->v6.sin6_addr.s6_addr)) {

            IMP_LOG_WARNING("Group address %s is link-local multicast address\n",
                           imp_pi_ntoa(p_pia));
            return -1;
        }
        /*
         * Nodes must not originate a packet to a multicast address whose scop
         * field contains the reserved value 0; if such a packet is received, it
         * must be silently dropped.  Nodes should not originate a packet to a
         * multicast address whose scop field contains the reserved value F; if
         * such a packet is sent or received, it must be treated the same as
         * packets destined to a global (scop E) multicast address.
         * [RFC 4291 section 2.7]
         */
         if ((p_pia->v6.sin6_addr.s6_addr[1] & 0x0f) == 0) {

            IMP_LOG_WARNING("group address %s's scope is 0, Ignore it.\n",
                             imp_pi_ntoa(p_pia));
            return -1;
        }
    } else if(p_pia->ss.ss_family == AF_INET) {

        if (!IN_MULTICAST(ntohl(p_pia->v4.sin_addr.s_addr))) {

            IMP_LOG_ERROR("group address %s isn't multicast adddress\n",
                          imp_pi_ntoa(p_pia));
            return -1;
        }

        /*
         *    Address Range                 Size       Designation
         *    -------------                 ----       -----------
         *    224.0.0.0 - 224.0.0.255       (/24)      Local Network Control Block
         *
         *    Addresses in the Local Network Control Block are used for protocol
         *    control traffic that is not forwarded off link.
         *    [RFC 5771 section 4]
         */
        if (IN_LOCAL_CONTROL_BLOCK(ntohl(p_pia->v4.sin_addr.s_addr))) {

            IMP_LOG_INFO("Group address %s is Local Network Control Block\n",
                          imp_pi_ntoa(p_pia));
            return -1;
        }

	/* 239.255.0.0/16 is defined to be the IPv4 Local Scope.
	 * [RFC 2365 section 6.1]
	 */

        if (IN_LOCAL_SCOPE_BLOCK(ntohl(p_pia->v4.sin_addr.s_addr))) {

            IMP_LOG_INFO("Group address %s belongs to IPv4 Local Scope.\n",
                          imp_pi_ntoa(p_pia));
            return -1;
        }


    }else
        return -1;

    return 0;
}

/*-----------------------------------------------------------------------
 * Name         : imp_input_report_v1v2
 *
 * Brief        : process igmpv1 and v2's report
 * Params       : [in] p_if   -- struct imp_interface
                  [in] igh   -- igmp message
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_input_report_v1v2(imp_interface  *p_if, struct igmphdr* igh)
{
    pi_addr pig;

    imp_build_piaddr(AF_INET, &igh->group, &pig);

    IMP_LOG_DEBUG("group address %s\n", imp_pi_ntoa(&pig));

    if (imp_verify_multicast_addr(&pig) < 0)
        return;

    switch (igh->type) {

        case IGMP_HOST_MEMBERSHIP_REPORT:
            mcast_is_ex_hander(p_if, &pig, NULL, IM_IGMPv1);
            break;
        case IGMPV2_HOST_MEMBERSHIP_REPORT:
            mcast_is_ex_hander(p_if, &pig, NULL, IM_IGMPv2_MLDv1);
            break;
        case IGMP_HOST_LEAVE_MESSAGE:
            mcast_to_in_handler(p_if, &pig, NULL, IM_IGMPv2_MLDv1);
            break;
    }
    imp_membership_db_update(&pig);
    return;

}

void imp_input_report_mld(imp_interface  *p_if, struct mld_hdr* p_mldhr)
{
    pi_addr pig;

    imp_build_piaddr(AF_INET6, &p_mldhr->mld_addr, &pig);

    if (imp_verify_multicast_addr(&pig) < 0)
        return;

    IMP_LOG_DEBUG("test %s\n", imp_pi_ntoa(&pig));

    switch (p_mldhr->mld_type) {

        case MLD_LISTENER_REPORT:
            mcast_is_ex_hander(p_if, &pig, NULL, IM_IGMPv2_MLDv1);
            break;
        case MLD_LISTENER_REDUCTION:
            mcast_to_in_handler(p_if, &pig, NULL, IM_IGMPv2_MLDv1);
            break;
    }
    imp_membership_db_update(&pig);
    return;

}

/*-----------------------------------------------------------------------
 * Name         : imp_input_report_v3
 *
 * Brief        : process igmpv3 report
 * Params       : [in] p_if   -- struct imp_interface
                  [in] p_ig3h  -- igmpv3 message
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_input_report_v3(imp_interface *p_if, struct igmpv3_report* p_ig3h, int buf_len)
{
    int     i = 0;
    pi_addr pig;
    int     cur_len = 0;

    cur_len  += sizeof(struct igmpv3_report);

    IMP_LOG_DEBUG("type : 0x%x\n num p_grec %d\n", p_ig3h->type, ntohs(p_ig3h->ngrec));

    for (; i < ntohs(p_ig3h->ngrec); i++) {

        struct igmpv3_grec *p_grec = NULL;
        pa_list*            p_src_list = NULL;
        int                 k;

        p_grec = (struct igmpv3_grec*)((char*)p_ig3h + cur_len);

        cur_len += (ntohs(p_grec->grec_nsrcs)*sizeof(int) + sizeof(struct igmpv3_grec));
        if (cur_len > buf_len) {

            IMP_LOG_ERROR("exceed buffer\n");
            return;
        }
        imp_build_piaddr(AF_INET, &p_grec->grec_mca, &pig);
        IMP_LOG_DEBUG("group is = %s\n", imp_pi_ntoa(&pig));

        if (imp_verify_multicast_addr(&pig) < 0)
            continue;

        IMP_LOG_DEBUG("number of source = %d\n", p_grec->grec_nsrcs);
        for (k = 0; k < ntohs(p_grec->grec_nsrcs); k++) {

            pi_addr pa;
            imp_build_piaddr(AF_INET, &p_grec->grec_src[k], &pa);
            p_src_list = pa_list_add(p_src_list, &pa);
        }

        switch (p_grec->grec_type) {

            case IGMPV3_MODE_IS_INCLUDE:
                IMP_LOG_DEBUG("IGMPV3_MODE_IS_INCLUDE\n");
                mcast_is_in_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case IGMPV3_MODE_IS_EXCLUDE:
                IMP_LOG_DEBUG("IGMPV3_MODE_IS_EXCLUDE\n");
                mcast_is_ex_hander(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case IGMPV3_CHANGE_TO_INCLUDE:
                /*leave */
                IMP_LOG_DEBUG("IGMPV3_CHANGE_TO_INCLUDE\n");
                mcast_to_in_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case IGMPV3_CHANGE_TO_EXCLUDE:
                /*join*/
                IMP_LOG_DEBUG("IGMPV3_CHANGE_TO_EXCLUDE\n");
                mcast_to_ex_hander(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case IGMPV3_ALLOW_NEW_SOURCES:
                IMP_LOG_DEBUG("IGMPV3_ALLOW_NEW_SOURCES\n");
                mcast_allow_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case IGMPV3_BLOCK_OLD_SOURCES:
                IMP_LOG_DEBUG("IGMPV3_BLOCK_OLD_SOURCES\n");
                mcast_block_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            default:
                IMP_LOG_ERROR("error group type\n");
                pa_list_cleanup(&p_src_list);
                return;
        }
        imp_membership_db_update(&pig);
        pa_list_cleanup(&p_src_list);

    }
    return;
}

void imp_input_report_mldv2(imp_interface *p_if, struct mld_hdr *p_mldh, int buf_len)
{
    int i = 0, num = 0, cur_len = 0;
    pi_addr pig;

    num =  ntohs(p_mldh->mld_num_multi);
    cur_len += sizeof(struct icmp6_hdr);

    for (; i < num; i++) {

        struct imp_multi_rec* p_rec = (struct imp_multi_rec*)((char*)p_mldh + cur_len);
         pa_list *p_src_list = NULL;
         int k;

        imp_build_piaddr(AF_INET6, &p_rec->multi_addr, &pig);
        IMP_LOG_DEBUG("group is = %s\n", imp_pi_ntoa(&pig));

        if (imp_verify_multicast_addr(&pig) < 0)
            continue;

        cur_len += (ntohs(p_rec->src_num)*sizeof(struct in6_addr) +
                        sizeof(struct imp_multi_rec));

        if (cur_len > buf_len) {

            IMP_LOG_ERROR("exceed buffer\n");
            return;
        }

        for (k = 0; k < ntohs(p_rec->src_num); k++) {

            pi_addr pa;
            imp_build_piaddr(AF_INET6, &p_rec->src_list[k], &pa);
            p_src_list = pa_list_add(p_src_list, &pa);
        }

        switch (p_rec->rec_type) {

            case MLD_MODE_IS_INCLUDE:
                IMP_LOG_INFO("MLD_MODE_IS_INCLUDE\n");
                mcast_is_in_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case MLD_MODE_IS_EXCLUDE:
                IMP_LOG_INFO("MLD_MODE_IS_EXCLUDE\n");
                mcast_is_ex_hander(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case MLD_CHANGE_TO_INCLUDE:
                /*leave */
                IMP_LOG_INFO("MLD_CHANGE_TO_INCLUDE\n");
                mcast_to_in_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case MLD_CHANGE_TO_EXCLUDE:
                /*join*/
                IMP_LOG_INFO("MLD_CHANGE_TO_EXCLUDE\n");
                mcast_to_ex_hander(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case MLD_ALLOW_NEW_SOURCES:
                IMP_LOG_INFO("MLD_ALLOW_NEW_SOURCES\n");
                mcast_allow_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            case MLD_BLOCK_OLD_SOURCES:
                IMP_LOG_INFO("MLD_BLOCK_OLD_SOURCES\n");
                mcast_block_handler(p_if, &pig, p_src_list, IM_IGMPv3_MLDv2);
                break;
            default:
                IMP_LOG_ERROR("error group type\n");
                pa_list_cleanup(&p_src_list);
                return;
        }
        imp_membership_db_update(&pig);
        pa_list_cleanup(&p_src_list);

    }
}


#define MAXCTRLSIZE                     \
    (sizeof(struct cmsghdr) + sizeof(struct sockaddr_in6) +  \
    sizeof(struct cmsghdr) + sizeof(int) + 32)

void mcast_recv_mld(int sockfd, int version)
{
    unsigned char buf[MAX_RECV_BUF_LEN] = {0};
    int num = 0;
    pi_addr pig, pia;
    struct msghdr msg;
    struct sockaddr_in6 sa;
    struct iovec    iov;
    struct cmsghdr *cmsg;
    char           *ctrl = (char *) malloc(MAXCTRLSIZE);
    struct in6_pktinfo *info;
    unsigned int if_index = 0;
    struct mld_hdr  *p_mldh;
    imp_interface *p_if = NULL;

    /*To get the unique index of the interfacethe packet was received on*/
    bzero(&msg, sizeof(msg));
    iov.iov_base = buf;
    iov.iov_len = 2048;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = ctrl;
    msg.msg_controllen = MAXCTRLSIZE;
    msg.msg_name = &sa;
    msg.msg_namelen = sizeof(struct sockaddr_in6);

    num = recvmsg(get_igmp_mld_socket(AF_INET6), &msg, MSG_WAITALL);
    if (num <= 20) {
        IMP_LOG_ERROR("BAD packet received n=%d \n", num);
        free(ctrl);
        return;
    }

    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
     cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_type == IPV6_PKTINFO) {


            info = (struct in6_pktinfo *) CMSG_DATA(cmsg);
            if_index = info->ipi6_ifindex;
            imp_build_piaddr(AF_INET6, &info->ipi6_addr, &pig);
            imp_build_piaddr(AF_INET6, &sa.sin6_addr, &pia);
            IMP_LOG_DEBUG("from %s %s %d\n", imp_pi_ntoa(&pig), imp_pi_ntoa(&pia), if_index);
        }
    }
    free(ctrl);

    if (if_index == 0) {
        /*
         * Structure used to communicate from kernel to multicast router.
         * We'll overlay the structure onto an MLD header (not an IPv6 heder like igmpmsg{}
         * used for IPv4 implementation). This is because this structure will be passed via an
         * IPv6 raw socket, on wich an application will only receiver the payload i.e the data after
         * the IPv6 header and all the extension headers. (See section 3 of RFC 3542)
         */
        struct mrt6msg *p_mrmsg;
        int iif_index;

        p_mrmsg = msg.msg_iov->iov_base;

        IMP_LOG_DEBUG("DUMP mrmsg:\n\tmif %d\n\tmsgtype %d\n\tsrc %s\n\tgroup %s\n",
        p_mrmsg->im6_mif, p_mrmsg->im6_msgtype, imp_inet6_ntoa(p_mrmsg->im6_src.s6_addr),
        imp_inet6_ntoa(p_mrmsg->im6_dst.s6_addr));

        imp_build_piaddr(AF_INET6, &p_mrmsg->im6_dst, &pig);
        imp_build_piaddr(AF_INET6, &p_mrmsg->im6_src, &pia);

          //mif = k_get_vmif(get_up_if_index(), AF_INET6);
        iif_index = k_get_rlif(p_mrmsg->im6_mif, AF_INET6);
        IMP_LOG_DEBUG("k_get_vmif = %d im6_mif %d\n", iif_index, p_mrmsg->im6_mif);

        //if (mif == p_mrmsg->im6_mif) {

        if_set ttls;

        bzero(&ttls, sizeof(ttls));

        /*get ttls*/
        if (imp_get_mfcc_ttls(&ttls, MAXVIFS, &pia, &pig, iif_index) != 0) {

            IMP_LOG_DEBUG("add MFC:src -- %s  group -- %s\n\n", imp_pi_ntoa(&pia), imp_pi_ntoa(&pig));
            imp_membership_db_mfc_add(iif_index, &pig, &pia, &ttls);
        }
        //}
        return;

    }

    /*return if the interface don't enable igmp*/
    for(p_if = imp_interface_first();p_if;p_if = LIST_NEXT(p_if, link))
    {
        if(p_if->if_index == if_index && p_if->if_addr.ss.ss_family == AF_INET6)
            break;
    }

    if (p_if == NULL) {

        IMP_LOG_DEBUG("Don't exist this VIF\n");
        return;
    }

    if (p_if->type != INTERFACE_DOWNSTREAM) {

        IMP_LOG_DEBUG("Don't exist this Downstream VIF\n");
        return;
    }

    p_mldh = msg.msg_iov->iov_base;

    switch (p_mldh->mld_type) {
        case MLD_LISTENER_REPORT:
        case MLD_LISTENER_REDUCTION:
            imp_input_report_mld(p_if, p_mldh);
            break;
        case MLD_V2_LISTENER_REPORT:
            /* An MLDv2 Report MUST be sent with a valid IPv6 link-local source
             * address, or the unspecified address (::), if the sending interface
             * has not acquired a valid link-local address yet.
             * [rfc 3810 5.2.13]
            */
            if (!(IN6_IS_ADDR_LINKLOCAL(&sa.sin6_addr) || IN6_IS_ADDR_UNSPECIFIED(&sa.sin6_addr))) {

                IMP_LOG_ERROR("MLDv2 Report MUST be sent with a valid IPv6 link-local "
                    "source address, or the unspecified address (::).\n");
                return;
            }
            if (version < IM_IGMPv3_MLDv2) {

                IMP_LOG_WARNING("current version is mldv%d\n", version - 1);
                return;
            }
            imp_input_report_mldv2(p_if, p_mldh, num);
            break;
        default:
            IMP_LOG_WARNING("unknown type %d\n", p_mldh->mld_type);
            break;

    }

}
void mcast_recv_igmp(int sockfd, int version)
{
    unsigned char buf[MAX_RECV_BUF_LEN] = {0};
    int num = 0;
    struct iphdr* ip = NULL;
    int iph_len = 0, igh_len = 0;
    struct igmphdr *p_igh;
    pi_addr pig, pia;
    struct msghdr msg;
    struct sockaddr sa;
    struct iovec    iov;
    struct cmsghdr *cmsg;
    char           *ctrl = (char *) malloc(MAXCTRLSIZE);
    struct in_pktinfo *info;
    unsigned int if_index = 0;
    imp_interface* p_if = NULL;

    /*To get the unique index of the interfacethe packet was received on*/
    bzero(&msg, sizeof(msg));
    iov.iov_base = buf;
    iov.iov_len = 2048;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = ctrl;
    msg.msg_controllen = MAXCTRLSIZE;
    msg.msg_name = &sa;
    msg.msg_namelen = sizeof(struct sockaddr);

    num = recvmsg(get_igmp_mld_socket(AF_INET), &msg, MSG_WAITALL);
    if (num <= sizeof(*ip)) {
        IMP_LOG_ERROR("BAD packet received n=%d \n", num);
        free(ctrl);
        return;
    }

    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
     cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_type == IP_PKTINFO) {
            info = (struct in_pktinfo *) CMSG_DATA(cmsg);
            if_index = info->ipi_ifindex;
        }
    }
    free(ctrl);
    ip = (struct iphdr*)buf;


    if (ip->protocol != IPPROTO_IGMP && ip->protocol != 0) {

        IMP_LOG_ERROR("error protocol number %d\n", ip->protocol);
        return;
    }

    imp_build_piaddr(AF_INET, &ip->daddr, &pig);
    imp_build_piaddr(AF_INET, &ip->saddr, &pia);

    if (mcast_if_get_by_addr(&pia) != NULL) {

        IMP_LOG_DEBUG("Ignore igmp from myself\n");
        return;
    }

    /*when protocol is zero, we need add MFC base one this kind of packet*/
    if (ip->protocol == 0) {

        if_set ttls;
	    struct igmpmsg *p_imsg;
	    int iif_index;

	    p_imsg = (struct igmpmsg*)buf;
        bzero(&ttls, sizeof(ttls));

	    iif_index = k_get_rlif(p_imsg->im_vif, AF_INET);
	    if (iif_index == 0) {
		    IMP_LOG_ERROR("Can not found realted upstream interface, vif:%d\n", p_imsg->im_vif);
		    return;
	    }
	    IMP_LOG_DEBUG("k_get_rlif = %d im_vif = %d\n", iif_index, p_imsg->im_vif);
        /*get ttls*/
        if(imp_get_mfcc_ttls(&ttls, MAXVIFS, &pia, &pig, iif_index) != 0){

            IMP_LOG_DEBUG("add MFC:src -- %s group -- %s\n\n", imp_pi_ntoa(&pia), imp_pi_ntoa(&pig));
            imp_membership_db_mfc_add(iif_index, &pig, &pia, &ttls);
        }
        return;
    } else if (ip->protocol == 0) {

        return;
    }

#if 0
    /*print hex*/
    for(;i<num;i++)
    {
        if(i%10 == 0)
            printf("\n");

        printf("0x%02x ", buf[i]);
    }
    printf("\n");
#endif
    /*return if the interface don't enable igmp*/
    for(p_if = imp_interface_first();p_if;p_if = LIST_NEXT(p_if, link))
    {
        if(p_if->if_index == if_index && p_if->if_addr.ss.ss_family == AF_INET)
            break;
    }
    
    IMP_LOG_DEBUG("src addr = %s received interface %d\n",
        imp_inet_ntoa(((struct sockaddr_in*)&sa)->sin_addr.s_addr), if_index);

    if(p_if == NULL){

        IMP_LOG_DEBUG("Don't exist this VIF %d\n", if_index);
        return;
    }

    if (p_if->type != INTERFACE_DOWNSTREAM) {

        IMP_LOG_DEBUG("Don't exist this Downstream VIF\n");
        return;
    }


    iph_len = ip->ihl * sizeof(int);

    if(iph_len < 24) {

        /*to do:need to judge option if is RAO or not */
        IMP_LOG_WARNING("Don't contain Router alter option\n");
        return;
    }
    igh_len = num - iph_len;
    p_igh = (struct igmphdr*)((char*)ip + iph_len);

    if(in_cusm((unsigned short*)p_igh, igh_len) != 0) {

        IMP_LOG_ERROR("check sum is error\n");
        return;
    }

    switch (p_igh->type) {

        case IGMP_HOST_MEMBERSHIP_REPORT:
            imp_input_report_v1v2(p_if, p_igh);
            break;
        case IGMP_HOST_LEAVE_MESSAGE:
        case IGMPV2_HOST_MEMBERSHIP_REPORT:
            if (version < IM_IGMPv2_MLDv1) {
                IMP_LOG_WARNING("Don't support IGMPv2, " \
                                "current version is igmpv%d\n", version);
                return;
            }
            imp_input_report_v1v2(p_if, p_igh);
            break;
        case IGMPV3_HOST_MEMBERSHIP_REPORT:
            if (version < IM_IGMPv3_MLDv2) {
                IMP_LOG_WARNING("Don't support IGMPv3, " \
                                "current version is igmpv%d\n", version);
                return;
            }
            imp_input_report_v3(p_if, (struct igmpv3_report*)p_igh, igh_len);
            break;
        default:
            IMP_LOG_WARNING("unknown igmp type 0x%x\n", p_igh->type);
            break;

    }

}
