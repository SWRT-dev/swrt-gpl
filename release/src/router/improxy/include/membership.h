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

#ifndef _IMP_MEMBERSHIP_H_
#define _IMP_MEMBERSHIP_H_


#include "data.h"
typedef struct imp_mfc {
    LIST_ENTRY(imp_mfc) link;
    struct if_set         ttls;
    pi_addr             pia;
    int                 iif_index;
}imp_mfc;

typedef struct imp_membership_db {
    LIST_ENTRY(imp_membership_db)     link;
    pi_addr                         pig;
    LIST_HEAD(, imp_mfc)             mfc_list;
    group_type                      type;
    unsigned char                     update_flag;
    pa_list                         *src_list;
}imp_membership_db;


void imp_membership_db_init();
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_find
 *
 * Brief        : find membership db base on group address
 * Params       : [in] p_ga -- group addr
 * Return       : imp_membership_db
*------------------------------------------------------------------------
*/
imp_membership_db* imp_membership_db_find(pi_addr *p_pia);
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
void imp_membership_db_add(pi_addr *p_ga,pa_list* p_src_list,group_type type);
void imp_membership_db_cleanup(imp_membership_db *p_md);
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_add
 *
 * Brief        : add mfc into membership db
 * Params       : [in] p_ga  -- mfc's group addr
                  [in] p_src -- mfc's src addr
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_membership_db_mfc_add(int iif_index, pi_addr *p_ga, pi_addr *p_src, if_set *p_ttls);
/*-----------------------------------------------------------------------
 * Name         : imp_membership_db_mfc_find
 *
 * Brief        : find  mfc from membership db
 * Params       : [in] p_md  -- imp_membership_db
                  [in] p_src -- mfc's src addr
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_mfc* imp_membership_db_mfc_find(imp_membership_db *p_md, pi_addr *p_src);
void imp_membership_db_mfc_update(pi_addr *p_ga,pi_addr *p_src,
    int if_index, char enable);
void imp_membership_db_turnon_update(pi_addr *p_ga);
void imp_membership_db_update(pi_addr *p_ga);
void imp_membership_db_cleanup_all(void);
void imp_membership_db_print_all(void);
#endif
