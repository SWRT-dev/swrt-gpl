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

#ifndef _IMP_HANDLER_H_
#define _IMP_HANDLER_H_

#include "data.h"
void mcast_is_ex_hander(imp_interface *p_if, pi_addr *p_ga,
    pa_list* p_src_list, im_version version);
void mcast_to_ex_hander(imp_interface *p_if, pi_addr *p_ga,
    pa_list* p_src_list, im_version version);
/*-----------------------------------------------------------------------
 * Name         : mcast_to_in_handler
 *
 * Brief        : hnadler of igmpv2 leave message and igmpv3 TO_IN message
 * Params       : [in] p_if   -- struct imp_interface
                  [in] p_ga  -- group address
 * Return       : NULL
*------------------------------------------------------------------------
*/
void mcast_to_in_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version);
void mcast_is_in_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version);
void mcast_allow_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version);
void mcast_block_handler(imp_interface *p_if, pi_addr *p_ga,
     pa_list* p_src_list, im_version version);
/*-----------------------------------------------------------------------
 * Name         : group_source_specific_timer_handler
 *
 * Brief        : timer handler to send Group-and-Source-Specific Query
 * Params       : [in] data -- struct sch_event
 * Return       : new timer
*------------------------------------------------------------------------
*/
imp_timer* group_source_specific_timer_handler(void   *data);
/*-----------------------------------------------------------------------
 * Name         : general_queries_timer_handler
 *
 * Brief        : send General Queries periodicity
 * Params       : [in] data -- struct imp_interface
 * Return       : NULL
*------------------------------------------------------------------------
*/
imp_timer* general_queries_timer_handler(void* data);
#endif
