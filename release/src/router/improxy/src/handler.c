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

#include "kernel_api.h"
#include "membership.h"
#include "handler.h"

extern void send_igmp_mld_query(imp_interface *p_if, im_version version,
    pi_addr *p_dst, pi_addr *p_gp_addr, imp_source* p_sources, int sflag);

void mcast_is_ex_hander(imp_interface *p_if, pi_addr *p_ga,
    pa_list* p_src_list, im_version version)
{
    imp_group *p_gp = NULL;
    pa_list *p_node = NULL;;


    p_gp = imp_group_find(p_if, p_ga);
    if (p_gp != NULL) {

        if (p_gp->version < version && version == IM_IGMPv3_MLDv2) {

            IMP_LOG_INFO("Group %s Group Compatibility Mode" \
                "is lesser than IGMPV3\n", imp_pi_ntoa(&p_gp->group_addr));
            return;
        }

        /*update version timer*/
        imp_group_version_timer_update(p_gp, version);

        if(p_gp->type == GROUP_INCLUDE) {
            /* Router State   Report Rec'd  New Router State     Actions
             * ------------   ------------  ----------------     -------
             *  INCLUDE (A)    IS_EX (B)     EXCLUDE (A*B, B-A)   (B-A)=0
             *                                                    Delete (A-B)
             *                                                    Group Timer=GMI
             */

            imp_source  *p_is = NULL, *p_is_next = NULL;

            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {
                    /*Delete (A-B)*/
                    imp_source_cleanup(p_is);
                }
            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){
                    /*(B-A)=0*/
                    imp_source_create(p_gp, &p_node->addr, BLOCKING);/*forward or not*/
                }
            }

            p_gp->type = GROUP_EXCLUDE;


            p_gp->timer = imp_add_timer(group_timer_handler, p_gp);
            /*Group Timer=GMI*/
            imp_set_timer(TIMER_GMI, p_gp->timer);

        } else {

            /*  Router State   Report Rec'd  New Router State   Actions
                *  ------------   ------------  ----------------   -------
             *  EXCLUDE (X, Y)  IS_EX (A)     EXCLUDE (A-Y, Y*A)    (A-X-Y)=GMI
             *                                                    Delete (X-A)
             *                                                    Delete (Y-A)
             *                                                    Group Timer=GMI
             */
            imp_source  *p_is = NULL, *p_is_next = NULL;

            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);

                if(TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;
                /*Delete (X-A)*/
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {
                    imp_source_cleanup(p_is);
                }
            }


            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);

                if(!TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;

                /*Delete (Y-A)*/
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {
                    imp_source_cleanup(p_is);
                }
            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, BLOCKING) == NULL
                    && imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){
                    /*(A-X-Y)=GMI*/
                    imp_source_create(p_gp, &p_node->addr, FORWARDING);/*forward or not*/
                }
            }

            /*Group Timer=GMI*/
            imp_set_timer(TIMER_GMI, p_gp->timer);

        }

        return;
    }

    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_EXCLUDE, version);

    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }


    imp_group_print(p_if);
    return;

}

void mcast_to_ex_hander(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version)
{
    imp_group *p_gp = NULL;
    pa_list *p_node = NULL;;

    p_gp = imp_group_find(p_if, p_ga);
    if (p_gp != NULL) {

        imp_source     *p_is = NULL, *p_is_next = NULL;
        int             enable_sch = 0;
        struct timeval  lmqt;

        imp_time_val(TIMER_LMQT, &lmqt);

        if (p_gp->version < version && version == IM_IGMPv3_MLDv2) {

            IMP_LOG_INFO("Group %s Group Compatibility Mode" \
                "is lesser than IGMPV3\n", imp_pi_ntoa(&p_gp->group_addr));
            return;
        }

        /*update version timer*/
        imp_group_version_timer_update(p_gp, version);


        if(p_gp->type == GROUP_INCLUDE) {
            /* Router State   Report Rec'd  New Router State     Actions
             * ------------   ------------  ----------------     -------
             *  INCLUDE (A)    TO_EX (B)    EXCLUDE (A*B, B-A)   (B-A)=0
             *                                                    Send Q(G, A*B)
             *                                                    Delete (A-B)
             *                                                    Group Timer=GMI
             */


            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {

                    /*Delete (A-B)*/
                    imp_source_cleanup(p_is);
                } else if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {

                    /*Send Q(G, A*B)*/
                    imp_set_timer(TIMER_LMQT, p_is->timer);
                    p_is->times = DEFAULT_RV;
                    enable_sch = 1;
                }
            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){

                    /*(B-A)=0*/
                    imp_source_create(p_gp, &p_node->addr, BLOCKING);/*forward or not*/
                }
            }

            p_gp->type = GROUP_EXCLUDE;

            p_gp->timer = imp_add_timer(group_timer_handler, p_gp);
            /*Group Timer=GMI*/
            imp_set_timer(TIMER_GMI, p_gp->timer);

        } else {

            /*  Router State   Report Rec'd  New Router State   Actions
                *  ------------   ------------  ----------------   -------
             *  EXCLUDE (X, Y)  TO_EX (A)     EXCLUDE (A-Y, Y*A)    (A-X-Y)=Group Timer
             *                                                    Delete (X-A)
             *                                                    Delete (Y-A)
             *                                                    Send Q(G, A-Y)
             *                                                    Group Timer=GMI
             */

            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);

                if(TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {

                    /*Delete (X-A)*/
                    imp_source_cleanup(p_is);
                } else if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {

                    /*Send Q(G, A-Y)*/
                    imp_set_timer(TIMER_LMQT, p_is->timer);
                    p_is->times = DEFAULT_RV;
                    enable_sch = 1;
                }
            }


            for (p_is = LIST_FIRST(&p_gp->src_list); p_is ;
                    p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);
                if(!TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;
                /*Delete (Y-A)*/
                if(pa_list_find(p_src_list, &p_is->src_addr) == NULL) {
                    imp_source_cleanup(p_is);
                }
            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, BLOCKING) == NULL
                    && imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){
                    /*(A-X-Y)=Group Timer*/
                    p_is = imp_source_create(p_gp, &p_node->addr, FORWARDING);/*forward or not*/
                    memcpy(&p_is->timer->tm, &p_gp->timer->tm, sizeof(struct timeval));

                    if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {

                        imp_set_timer(TIMER_LMQT, p_is->timer);
                        p_is->times = DEFAULT_RV;
                        enable_sch = 1;
                    }
                }
            }

            /*Group Timer=GMI*/
            imp_set_timer(TIMER_GMI, p_gp->timer);

        }

        if(enable_sch && p_gp->sch_timer == NULL) {

            p_gp->sch_timer = imp_add_timer(group_source_specific_timer_handler, p_gp);
            imp_set_timer(0, p_gp->sch_timer);
        }

        imp_membership_db_update(p_ga);

        return;
    }

    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_EXCLUDE, version);

    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }

    imp_membership_db_update(p_ga);
    imp_group_print(p_if);

}


void mcast_to_in_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version)
{
    imp_group     *p_gp;
    pa_list  *p_node = NULL;
    struct timeval lmqt;

    imp_time_val(TIMER_LMQT, &lmqt);

    p_gp = imp_group_find(p_if, p_ga);
    if (p_gp != NULL) {


        IMP_LOG_DEBUG("Leave Group %s\n", imp_pi_ntoa(p_ga));

        if (p_gp->version < version) {

            IMP_LOG_WARNING("%s's Group Compatibility Mode is IGMPv1.\n",
                imp_pi_ntoa(p_ga));
        return;
        }

        if (p_gp->type == GROUP_INCLUDE) {

            /*INCLUDE (A)    TO_IN (B)    INCLUDE (A+B)           (B)=GMI
                                                                  Send Q(G, A-B)
            */
            imp_source  *p_is = NULL;

            for (p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = LIST_NEXT(p_is, link)) {

                if (pa_list_find(p_src_list, &p_is->src_addr) == NULL) {

                    /* Send Q(G, A-B)*/
                    if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {
                        p_is->times = DEFAULT_RV;
                        imp_set_timer(TIMER_LMQT, p_is->timer);
                    }
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }

            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){

                    imp_source_create(p_gp, &p_node->addr, FORWARDING);/*forward*/
                }
            }


        } else {

            /*EXCLUDE (X, Y)  TO_IN (A)    EXCLUDE (X+A, Y-A)     (A)=GMI
             *                                                  Send Q(G, X-A)
             *                                                  Send Q(G)
             */
            imp_source  *p_is = NULL, *p_is_next = NULL;

            /*Y-A*/
            for (p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);

                if(!TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;

                if (pa_list_find(p_src_list, &p_is->src_addr) != NULL) {

                    imp_source_cleanup(p_is);
                }
            }

            for (p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = LIST_NEXT(p_is, link)) {

                if(TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;

                if (pa_list_find(p_src_list, &p_is->src_addr) == NULL) {
                    /*Send Q(G, X-A)*/
                    if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {
                        p_is->times = DEFAULT_RV;
                        imp_set_timer(TIMER_LMQT, p_is->timer);
                    }
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }

            }

            for(p_node = p_src_list; p_node; p_node = p_node->next) {

                if (imp_source_find(p_gp, &p_node->addr, FORWARDING) == NULL){

                    imp_source_create(p_gp, &p_node->addr, FORWARDING);/*forward*/
                }
            }

            /*Send Q(G)*/
            if(p_gp->gs_sch_times == 0) {
                IMP_LOG_DEBUG("Send Q(G)\n");
                p_gp->gs_sch_times = DEFAULT_RV;
                imp_set_timer(get_group_leave_time(), p_gp->timer);
            }
        }

        if(p_gp->sch_timer == NULL && get_group_leave_time()) {

            p_gp->sch_timer = imp_add_timer(
                                group_source_specific_timer_handler, p_gp);
            imp_set_timer(0, p_gp->sch_timer);
        }

        return;
    }

    if(p_src_list == NULL)
        return;

    IMP_LOG_DEBUG("Can't find group %s, will create it\n", imp_pi_ntoa(p_ga));
    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_INCLUDE, version);
    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }

    imp_group_print(p_if);

}
void mcast_is_in_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version)
{
    imp_group *p_gp = NULL;
    pa_list   *p_node = NULL;;

    p_gp = imp_group_find(p_if, p_ga);

    if (p_gp != NULL) {

        imp_source  *p_is = NULL, *p_is_next = NULL;

        if (p_gp->version < version && version == IM_IGMPv3_MLDv2) {

            IMP_LOG_INFO("Group %s Group Compatibility Mode" \
                "is lesser than IGMPV3\n", imp_pi_ntoa(&p_gp->group_addr));
            return;
        }

        /*update version timer*/
        imp_group_version_timer_update(p_gp, version);

        if(p_gp->type == GROUP_INCLUDE) {

            /*INCLUDE (A)    IS_IN (B)     INCLUDE (A+B)            (B)=GMI*/

            for (p_node = p_src_list ;p_node; p_node = p_node->next ){

                p_is = imp_source_find(p_gp, &p_node->addr, FORWARDING);
                if (p_is == NULL) {
                    imp_source_create(p_gp, &p_node->addr, FORWARDING);
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }
            }

        } else {
            /*EXCLUDE (X, Y)  IS_IN (A)     EXCLUDE (X+A, Y-A)        (A)=GMI*/

            for (p_node = p_src_list ;p_node; p_node = p_node->next ){

                p_is = imp_source_find(p_gp, &p_node->addr, FORWARDING);
                if (p_is == NULL) {

                    imp_source_create(p_gp, &p_node->addr, FORWARDING);
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }
            }

             for(p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);
                if (!TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;

                if( pa_list_find(p_src_list, &p_is->src_addr) != NULL)
                    imp_source_cleanup(p_is);
            }


         }


        return;
    }

    if(p_src_list == NULL)
        return;

    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_INCLUDE, version);

    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }

    imp_group_print(p_if);
}


void mcast_allow_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version)
{
    imp_group *p_gp = NULL;
    pa_list   *p_node = NULL;;

    p_gp = imp_group_find(p_if, p_ga);
    if (p_gp != NULL) {

        imp_source  *p_is = NULL, *p_is_next = NULL;

        if (p_gp->version < version && version == IM_IGMPv3_MLDv2) {

            IMP_LOG_INFO("Group %s Group Compatibility Mode" \
                "is lesser than IGMPV3\n", imp_pi_ntoa(&p_gp->group_addr));
            return;
        }

        /*update version timer*/
        imp_group_version_timer_update(p_gp, version);

        if(p_gp->type == GROUP_INCLUDE) {

            /*INCLUDE (A)    ALLOW (B)      INCLUDE (A+B)           (B)=GMI*/

            for (p_node = p_src_list ;p_node; p_node = p_node->next ){

                p_is = imp_source_find(p_gp, &p_node->addr, FORWARDING);
                if (p_is == NULL) {

                    imp_source_create(p_gp, &p_node->addr, FORWARDING);
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }
            }

        } else {
            /*EXCLUDE (X, Y)  ALLOW (A)    EXCLUDE (X+A, Y-A)       (A)=GMI*/

            for (p_node = p_src_list ;p_node; p_node = p_node->next ){

                p_is = imp_source_find(p_gp, &p_node->addr, FORWARDING);
                if (p_is == NULL) {

                    imp_source_create(p_gp, &p_node->addr, FORWARDING);
                } else {

                    imp_set_timer(TIMER_GMI, p_is->timer);
                }
            }

             for(p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = p_is_next) {

                p_is_next = LIST_NEXT(p_is, link);
                if (!TIMEVAL_ISZERO(p_is->timer->tm))
                    continue;

                if( pa_list_find(p_src_list, &p_is->src_addr) != NULL)
                    imp_source_cleanup(p_is);
            }


         }
        return;
    }
    
    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_INCLUDE, version);

    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }

    imp_membership_db_update(p_ga);

    imp_group_print(p_if);
}

void mcast_block_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version)
{
    imp_group   *p_gp = NULL;
    pa_list     *p_node = NULL;;

    p_gp = imp_group_find(p_if, p_ga);

    if (p_gp != NULL) {

        imp_source  *p_is = NULL, *p_is_next = NULL;
        int enable_sch = 0;
        struct timeval lmqt;

        imp_time_val(TIMER_LMQT, &lmqt);

        if (p_gp->version < version && version == IM_IGMPv3_MLDv2) {

            IMP_LOG_INFO("Group %s Group Compatibility Mode" \
                "is lesser than IGMPV3\n", imp_pi_ntoa(&p_gp->group_addr));
            return;
        }

        /*update version timer*/
        imp_group_version_timer_update(p_gp, version);

        if(p_gp->type == GROUP_INCLUDE) {

            /*INCLUDE (A)    BLOCK (B)    INCLUDE (A)             Send Q(G, A*B)*/
             for(p_is = LIST_FIRST(&p_gp->src_list); p_is;
                p_is = p_is_next) {

                if (pa_list_find(p_src_list, &p_is->src_addr) != NULL &&
                    TIMEVAL_LT(lmqt, p_is->timer->tm)) {

                    imp_set_timer(TIMER_LMQT, p_is->timer);
                    p_is->times = DEFAULT_RV;
                    enable_sch = 1;
                }
            }

        } else {
            /*EXCLUDE (X, Y)  BLOCK (A)    EXCLUDE (X+(A-Y), Y)     (A-X-Y)=Group Timer
                                                                    Send Q(G, A-Y)*/

            for (p_node = p_src_list ;p_node; p_node = p_node->next ){


                if (imp_source_find(p_gp, &p_node->addr, BLOCKING) == NULL) {

                    p_is = imp_source_find(p_gp, &p_node->addr, FORWARDING);

                    if (p_is == NULL) {

                        p_is = imp_source_create(p_gp, &p_node->addr, FORWARDING);

                        /*(A-X-Y)=Group Timer*/
                        memcpy(&p_is->timer->tm, &p_gp->timer->tm, sizeof(struct timeval));
                    }

                    /*Send Q(G, A-Y)*/
                    if(TIMEVAL_LT(lmqt, p_is->timer->tm)) {

                        p_is->times = DEFAULT_RV;
                        enable_sch = 1;
                        imp_set_timer(TIMER_LMQT, p_is->timer);

                    }

                }
            }

         }

        if(enable_sch && p_gp->sch_timer == NULL ) {

            p_gp->sch_timer = imp_add_timer(group_source_specific_timer_handler, p_gp);
            imp_set_timer(0, p_gp->sch_timer);
        }

        return;
    }
    
    p_gp = imp_group_create(p_if, p_ga, p_src_list, GROUP_EXCLUDE, version);

    if (p_gp == NULL) {
        IMP_LOG_ERROR("imp_group_create failed");
        return;
    }

    imp_membership_db_update(p_ga);

    imp_group_print(p_if);

}


/*-----------------------------------------------------------------------
 * Name         : group_source_specific_timer_handler
 *
 * Brief        : timer handler to send Group-and-Source-Specific Query
 * Params       : [in] data -- struct sch_event
 * Return       : new timer
*------------------------------------------------------------------------
*/
imp_timer* group_source_specific_timer_handler(void   *data)
{

    IMP_LOG_DEBUG("Enter\n");

    imp_group       *p_gp   = data;
    imp_interface   *p_if   = p_gp->parent_if;
    struct timeval   lqmt;
    imp_source      *p_is;

    /*When building a group and source specific query for a group G, two
     *separate query messages are sent for the group.  The first one has
     *the "Suppress Router-Side Processing" bit set and contains all the
     *sources with retransmission state and timers greater than LMQT.  The
     *second has the "Suppress Router-Side Processing" bit clear and
     *contains all the sources with retransmission state and timers lower
     *or equal to LMQT.  If either of the two calculated messages does not
     *contain any sources, then its transmission is suppressed.
     *rfc 3376 Section 6.6.3.2
    */

    if(imp_source_find_scheduled(p_gp, TRUE) != NULL) {/*Suppress Router-Side Processing 1*/
        send_igmp_mld_query(p_if, p_gp->version, &p_gp->group_addr,
                &p_gp->group_addr, LIST_FIRST(&p_gp->src_list), 1);

    }

    if(imp_source_find_scheduled(p_gp, FALSE) != NULL) {
        send_igmp_mld_query(p_if, p_gp->version, &p_gp->group_addr,
                &p_gp->group_addr, LIST_FIRST(&p_gp->src_list), 0);
    }

    for(p_is = LIST_FIRST(&p_gp->src_list); p_is;
        p_is = LIST_NEXT(p_is, link)){

        if(p_is->times)
            p_is->times--;
    }

    /* When a table action "Send Q(G)" is encountered, then the group timer
     * must be lowered to LMQT.  The router must then immediately send a
     * group specific query as well as schedule [Last Member Query Count -
     * 1] query retransmissions to be sent every [Last Member Query
     * Interval] over [Last Member Query Time].
     * When transmitting a group specific query, if the group timer is
     * larger than LMQT, the "Suppress Router-Side Processing" bit is set in
     * the query message.
     * rfc 3376 Section 6.6.3.1
     */
    imp_time_val(TIMER_LMQT, &lqmt);

    if (p_gp->gs_sch_times != 0 ) {

        if(TIMEVAL_LT(lqmt, p_gp->timer->tm))/*Suppress Router-Side Processing 1*/ {
            IMP_LOG_DEBUG("TIMEVAL_LT %d %d\n", lqmt.tv_sec, p_gp->timer->tm.tv_sec);
            send_igmp_mld_query(p_if, p_gp->version, &p_gp->group_addr,
                &p_gp->group_addr, NULL, TRUE);
        }
        else
            send_igmp_mld_query(p_if, p_gp->version, &p_gp->group_addr,
                &p_gp->group_addr, NULL, FALSE);

        p_gp->gs_sch_times--;

    }

    /*update timer*/
    if (p_gp->gs_sch_times == 0 &&
        imp_source_find_scheduled(p_gp, FALSE) == NULL &&
        imp_source_find_scheduled(p_gp, TRUE)  == NULL) {

        IMP_LOG_DEBUG("REMOVE\n");
        imp_remove_timer(&p_gp->sch_timer);
    }
    else
        imp_set_timer(TIMER_LMQI, p_gp->sch_timer);

    return p_gp->sch_timer;

}

/*-----------------------------------------------------------------------
 * Name         : general_queries_timer_handler
 *
 * Brief        : send General Queries periodicity
 * Params       : [in] data -- struct imp_interface
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* general_queries_timer_handler(void* data)
{
    imp_interface*  p_if = data;
    pi_addr         dst;
    int             version;

    IMP_LOG_DEBUG("Enter\n");

    if (p_if->startup > 0 ) {

        p_if->startup--;
        imp_set_timer(TIMER_SQMI, p_if->gq_timer);

        if (p_if->startup == 0) {

            imp_set_timer(TIMER_QI, p_if->gq_timer);
        }

    } else {
        imp_set_timer(TIMER_QI, p_if->gq_timer);
    }
    get_all_host_addr(p_if->if_addr.ss.ss_family, &dst);
    version = get_im_version(p_if->if_addr.ss.ss_family);

    send_igmp_mld_query(p_if, version, &dst, NULL, NULL, 0);
    return p_if->gq_timer;
}

