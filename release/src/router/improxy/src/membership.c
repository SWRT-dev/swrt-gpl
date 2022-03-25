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

#include "data.h"
#include "kernel_api.h"
#include "proxy.h"
#include "membership.h"

LIST_HEAD(, imp_membership_db) md_head;

void imp_membership_db_init()
{
    LIST_INIT(&md_head);
}
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_find
 *
 * Brief        : find membership db base on group address
 * Params       : [in] p_ga -- group addr
 * Return       : imp_membership_db
*------------------------------------------------------------------------
*/
imp_membership_db* imp_membership_db_find(pi_addr *p_pia)
{
    imp_membership_db *p_mdb;
    for (p_mdb = LIST_FIRST(&md_head) ; p_mdb;
        p_mdb = LIST_NEXT(p_mdb, link)) {

        if (memcmp(&p_mdb->pig, p_pia, sizeof(pi_addr)) == 0)
            return p_mdb;
    }
    return NULL;
}
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_add
 *
 * Brief        : add group into membership db
 * Params       : [in] p_ga -- group addr will to be added
                   [in] p_src_list -- source list
                   [in] p_ga -- group type
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_membership_db_add(pi_addr *p_ga, pa_list* p_src_list, group_type type)
{
    imp_membership_db *p_md;
    if (imp_membership_db_find(p_ga) != NULL) {
        IMP_LOG_INFO("group %s's memship db has alreay existed\n");
        return;
    }
    if ((p_md = malloc(sizeof(imp_membership_db))) == NULL) {

        IMP_LOG_FATAL("malloc fail %s\n", strerror(errno));
        exit(1);
    }
    bzero(p_md, sizeof(imp_membership_db));
    memcpy(&p_md->pig, p_ga, sizeof(pi_addr));
    p_md->type = type;

    p_md->src_list = pa_list_dup(p_src_list);
    p_md->update_flag = 1;

    LIST_INSERT_HEAD(&md_head, p_md, link);

    return;
}

void imp_membership_db_cleanup(imp_membership_db *p_md)
{
    imp_mfc *p_mfc;

    if (p_md == NULL) {
        return;
    }

    pa_list_cleanup(&p_md->src_list);
    p_mfc = LIST_FIRST(&p_md->mfc_list);
    while (p_mfc) {

        LIST_REMOVE(p_mfc, link);
        /*pp need MRT_DEL_MFC first to update*/
        k_del_mfc(&p_md->pig, &p_mfc->pia);
        free(p_mfc);
        p_mfc = LIST_FIRST(&p_md->mfc_list);
    }

    LIST_REMOVE( p_md, link);
    free(p_md);

    return;
}
void imp_membership_db_cleanup_all(void)
{
    imp_membership_db *p_md;
    p_md = LIST_FIRST(&md_head);

    while(p_md) {

        imp_membership_db_cleanup(p_md);
        p_md = LIST_FIRST(&md_head);
    }

}
static void imp_membership_db_print(imp_membership_db *p_md)
{
    pa_list *p_pa;
    imp_mfc *p_mfc;
    printf("%s's membership db:\n", imp_pi_ntoa(&p_md->pig));
    printf("\tfmode %s\n", p_md->type == GROUP_EXCLUDE?"exclude mode":"include mode");
    printf("\tsource list:\n");
    for (p_pa = p_md->src_list; p_pa; p_pa = p_pa->next)
        printf("\t\t%s\n", imp_pi_ntoa(&p_pa->addr));

    printf("\tmfc list:\n");
    for (p_mfc = LIST_FIRST(&p_md->mfc_list); p_mfc;
        p_mfc = LIST_NEXT(p_mfc, link)) {

        imp_interface *p_if;
        printf("\t\tOrigin:%s outf:", imp_pi_ntoa(&p_mfc->pia));

        for (p_if = imp_interface_first(); p_if; p_if = LIST_NEXT(p_if, link)){

            int vmif = k_get_vmif(p_if->if_index, p_if->if_addr.ss.ss_family);
            if (vmif < 1)
                continue;
            if (IF_ISSET(vmif, &p_mfc->ttls) != 0)
                printf("%s ", p_if->if_name);
        }
        printf("\n");
    }
}
void imp_membership_db_print_all(void)
{
    imp_membership_db *p_md;
    p_md = LIST_FIRST(&md_head);
    printf("\n************************************************\n");

    for( ; p_md; p_md = LIST_NEXT(p_md, link)) {

        imp_membership_db_print(p_md);
    }
}

/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_add
 *
 * Brief        : add mfc into membership db
 * Params       : [in] p_ga  -- mfc's group addr
                  [in] p_src -- mfc's src addr
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_membership_db_mfc_add(int iif_index, pi_addr *p_ga, pi_addr *p_src, if_set *p_ttls)
{
    imp_membership_db *p_md;
    imp_mfc *p_mfc;

    if ((p_md = imp_membership_db_find(p_ga)) == NULL) {
        IMP_LOG_INFO("group %s's memship db don't existed\n", imp_pi_ntoa(p_ga));
        return;
    }

    if ( k_add_mfc(iif_index, p_ga, p_src, p_ttls) < 0 ) {

        IMP_LOG_ERROR("add mfc fail %s\n", strerror(errno));
        return;
    }

    if ((p_mfc = malloc(sizeof(imp_mfc))) == NULL) {

        IMP_LOG_FATAL("malloc fail %s\n", strerror(errno));
        exit(1);
    }

    memcpy(&p_mfc->pia, p_src, sizeof(pi_addr));
    memcpy(&p_mfc->ttls, p_ttls, sizeof(if_set));
    p_mfc->iif_index = iif_index;
    
    LIST_INSERT_HEAD(&p_md->mfc_list, p_mfc, link);


}

/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_find
 *
 * Brief        : find  mfc from membership db
 * Params       : [in] p_md  -- imp_membership_db
                  [in] p_src -- mfc's src addr
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_mfc* imp_membership_db_mfc_find(imp_membership_db *p_md, pi_addr *p_src)
{
    imp_mfc *p_mfc;


    p_mfc = LIST_FIRST(&p_md->mfc_list);
    while (p_mfc) {

        if (memcmp(&p_mfc->pia, p_src, sizeof(pi_addr)) == 0) {
            return p_mfc;
        }
        p_mfc = LIST_NEXT(p_mfc, link);
    }
    return NULL;
}
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_update
 *
 * Brief        : update multicast forwarding cache
 * Params       : [in] p_ga      -- mfc's group assress
                  [in] p_src     -- mfc's source address,
                                  if it is null, update all mfc related to p_ga!
                  [in] if_index -- mfc's interface index
                  [in] enable     -- forward or not
 * Return       : NULL
*------------------------------------------------------------------------
*/

void imp_membership_db_mfc_update(pi_addr *p_ga, pi_addr *p_src,
    int if_index, char enable)
{
    imp_membership_db *p_md;
    imp_mfc *p_mfc, *p_mfc_next;
    if_set  nullset;

    IF_ZERO(&nullset);

    if ((p_md = imp_membership_db_find(p_ga)) == NULL) {
        IMP_LOG_INFO("group %s's memship db don't existed\n", imp_pi_ntoa(p_ga));
        return;
    }

    p_mfc = LIST_FIRST(&p_md->mfc_list);

    while (p_mfc) {

        char bEnable = 0;
        int vmif = 0;
        p_mfc_next = LIST_NEXT(p_mfc, link);

        if(p_src != NULL &&
            memcmp(&p_mfc->pia, p_src, sizeof(pi_addr)) != 0) {
            p_mfc = p_mfc_next;
            continue;
        }

        vmif = k_get_vmif(if_index, p_md->pig.ss.ss_family);
        bEnable = IF_ISSET(vmif, &p_mfc->ttls)?1:0;

        IMP_LOG_DEBUG("if_index %d %s %d\n", vmif, imp_pi_ntoa(&p_mfc->pia), bEnable);
        if (bEnable == enable) {
            p_mfc = p_mfc_next;
            IMP_LOG_DEBUG("2\n");
            continue;
        }

        if (enable)
            IF_SET(vmif, &p_mfc->ttls);
        else
            IF_CLR(vmif, &p_mfc->ttls);

        if (memcmp(&nullset, &p_mfc->ttls, sizeof(if_set)) == 0) {

            /*del mfc*/
            k_del_mfc(&p_md->pig, &p_mfc->pia);
            LIST_REMOVE(p_mfc, link);
            free(p_mfc);
        } else {
            IMP_LOG_DEBUG("3\n");
            /*pp need MRT_DEL_MFC first to update*/
            k_del_mfc(&p_md->pig, &p_mfc->pia);
            /*change mfc*/
            k_add_mfc(p_mfc->iif_index, &p_md->pig, &p_mfc->pia, &p_mfc->ttls);
        }

        if(p_src != NULL)
            return;

        p_mfc = p_mfc_next;
    }

    return;
}
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_turnon_update
 *
 * Brief        : turn on update flag to update membership db
 * Params       : [in] p_ga      -- group address whose membership db need to be updated
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_membership_db_turnon_update(pi_addr *p_ga)
{
    imp_membership_db *p_md;

    if ((p_md = imp_membership_db_find(p_ga)) == NULL) {
        IMP_LOG_INFO("group %s's memship db don't existed\n", imp_pi_ntoa(p_ga));
        return;
    }
    p_md->update_flag = TRUE;
}

/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_update
 *
 * Brief        : merage&update echo downstream interface's source filter and
                  update multicast forwarding cache
 * Params       : [in] p_ga      -- group address whose membership db need to be updated
 * Return       : NULL
*------------------------------------------------------------------------
*/
/*rfc 4605 4.1.  Membership Database*/
void imp_membership_db_update(pi_addr *p_ga)
{
    imp_membership_db *p_md;
    imp_interface    *p_if = NULL;

    if ((p_md = imp_membership_db_find(p_ga)) == NULL) {
        IMP_LOG_INFO("group %s's memship db don't existed\n", imp_pi_ntoa(p_ga));
        return;
    }
    if (p_md->update_flag == 0)
        return;

    pa_list_cleanup(&p_md->src_list);

    p_md->type = GROUP_UNDEFINED;

    for (p_if = imp_interface_first(); p_if; p_if = LIST_NEXT(p_if, link))
    {
        imp_group  *p_gp = NULL;
        p_gp = imp_group_find(p_if, p_ga);

        if(p_gp == NULL) {
            IMP_LOG_DEBUG("interface %s don't have %s\n", p_if->if_name, imp_pi_ntoa(p_ga));
            imp_membership_db_mfc_update(p_ga, NULL, p_if->if_index, FALSE);
            continue;
        }
        imp_membership_db_mfc_update(p_ga, NULL, p_if->if_index, TRUE);


        if (p_md->type == GROUP_UNDEFINED) {

            p_md->type = p_gp->type;

            if (p_md->type == GROUP_EXCLUDE) {
                imp_source  *p_is = NULL;

                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    p_md->src_list = pa_list_add(p_md->src_list, &p_is->src_addr);
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, FALSE);
                }

            }else {

                imp_mfc *p_imp_mfc = NULL;
                imp_mfc *imp_mfc_next = NULL;
                imp_source  *p_is = NULL;

                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    p_md->src_list = pa_list_add(p_md->src_list, &p_is->src_addr);
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, TRUE);
                }
                for ( p_imp_mfc = LIST_FIRST(&p_md->mfc_list);  p_imp_mfc;
                    p_imp_mfc = imp_mfc_next) {

                    imp_mfc_next = LIST_NEXT(p_imp_mfc, link);
                    if (imp_source_find(p_gp, &p_imp_mfc->pia, 1) == NULL)
                        imp_membership_db_mfc_update(p_ga, &p_imp_mfc->pia, p_if->if_index, FALSE);
                }
            }
        } else if (p_md->type == GROUP_INCLUDE) {

            if (p_gp->type == GROUP_INCLUDE) {
                /*if *all* such records have a filter mode of INCLUDE, then the
                 * filter mode of the interface record is INCLUDE, and the source list
                 * of the interface record is the union of the source lists of all the
                 * socket records.
                 * rfc 3376 Section 3.2
                 */
                /*A+B */
                pa_list *p_old_head = NULL;
                imp_source  *p_is = NULL;
                imp_mfc *p_imp_mfc = NULL;
                imp_mfc *imp_mfc_next = NULL;

                p_old_head = p_md->src_list;
                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    if (pa_list_find(p_old_head, &p_is->src_addr) == NULL) {
                        p_md->src_list = pa_list_add(p_md->src_list, &p_is->src_addr);
                    }
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, TRUE);
                }


                for ( p_imp_mfc = LIST_FIRST(&p_md->mfc_list);  p_imp_mfc;
                    p_imp_mfc = imp_mfc_next) {

                    imp_mfc_next = LIST_NEXT(p_imp_mfc, link);
                    if (imp_source_find(p_gp, &p_imp_mfc->pia, 1) == NULL)
                        imp_membership_db_mfc_update(p_ga, &p_imp_mfc->pia, p_if->if_index, FALSE);
                }
            } else {
                /* if *any* such record has a filter mode of EXCLUDE, then the filter
                 * mode of the interface record is EXCLUDE, and the source list of the
                 * interface record is the intersection of the source lists of all
                 * socket records in EXCLUDE mode, minus those source addresses that
                 * appear in any socket record in INCLUDE mode.
                 * rfc 3376 Section 3.2
                 */
                /*B-A*/
                pa_list *p_old_head = NULL;
                imp_source  *p_is = NULL;

                p_md->type = GROUP_EXCLUDE;

                p_old_head = p_md->src_list;

                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    if (pa_list_find(p_old_head, &p_is->src_addr) == NULL) {

                        p_md->src_list = pa_list_add(p_md->src_list, &p_is->src_addr);
                    }
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, FALSE);

                }
                pa_list_cleanup(&p_old_head);
            }
        } else if (p_md->type == GROUP_EXCLUDE) {

            if(p_gp->type == GROUP_INCLUDE) {
                /* if *any* such record has a filter mode of EXCLUDE, then the filter
                 * mode of the interface record is EXCLUDE, and the source list of the
                 * interface record is the intersection of the source lists of all
                 * socket records in EXCLUDE mode, minus those source addresses that
                 * appear in any socket record in INCLUDE mode.
                 * rfc 3376 Section 3.2
                 */
                /*A-B*/
                imp_source  *p_is = NULL;

                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    if (pa_list_find(p_md->src_list, &p_is->src_addr) != NULL) {

                        p_md->src_list = pa_list_del(p_md->src_list, &p_is->src_addr);
                    }
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, TRUE);

                }
            } else {
                /* if *any* such record has a filter mode of EXCLUDE, then the filter
                 * mode of the interface record is EXCLUDE, and the source list of the
                 * interface record is the intersection of the source lists of all
                 * socket records in EXCLUDE mode, minus those source addresses that
                 * appear in any socket record in INCLUDE mode.
                 * rfc 3376 Section 3.2
                 */
                /*A*B*/
                imp_source  *p_is = NULL;

                for (p_is = LIST_FIRST(&p_gp->src_list);  p_is ;
                    p_is = LIST_NEXT(p_is, link)) {

                    if (pa_list_find(p_md->src_list, &p_is->src_addr) == NULL) {

                        p_md->src_list = pa_list_del(p_md->src_list, &p_is->src_addr);
                    }
                    imp_membership_db_mfc_update(p_ga, &p_is->src_addr, p_if->if_index, FALSE);
                }
            }

        }

    }
    if (p_md->type == GROUP_UNDEFINED) {

        k_mcast_leave( &p_md->pig, NULL);
        imp_membership_db_cleanup(p_md);
        return;
    }

    //k_mcast_msfilter(&p_md->pig, p_md->src_list, p_md->type);
    p_md->update_flag = 0;
    return;
}

