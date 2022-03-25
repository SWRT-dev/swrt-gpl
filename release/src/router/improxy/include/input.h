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

#ifndef _IMP_INPUT_H_
#define _IMP_INPUT_H_
#include "proxy.h"
#include "data.h"
/*-----------------------------------------------------------------------
 * Name         : imp_input_report_v1v2
 *
 * Brief        : process igmpv1 and v2's report
 * Params       : [in] p_if   -- struct imp_interface
                  [in] igh   -- igmp message
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_input_report_v1v2(imp_interface  *p_if, struct igmphdr* igh);
void imp_input_report_mld(imp_interface  *p_if, struct mld_hdr* p_mldhr);
/*-----------------------------------------------------------------------
 * Name         : imp_input_report_v3
 *
 * Brief        : process igmpv3 report
 * Params       : [in] p_if   -- struct imp_interface
                  [in] ig3h  -- igmpv3 message
 * Return       : NULL
*------------------------------------------------------------------------
*/
void imp_input_report_v3(imp_interface *p_if, struct igmpv3_report* ig3h, int buf_len);
void imp_input_report_mldv2(imp_interface *p_if,struct mld_hdr *p_mldh, int buf_len);
void mcast_recv_mld(int sockfd, int version);
void mcast_recv_igmp(int sockfd, int version);
#endif
