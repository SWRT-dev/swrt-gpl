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

#ifndef __KERNEL_API_H_
#define __KERNEL_API_H_
#include <netinet/in.h>
#include <linux/mroute6.h>
#include "utils.h"


void k_add_ip4_vif (int socket, struct in_addr* sin, int ifindex);
void k_del_ip4_vif (int socket, int ifindex);
STATUS k_start4_mproxy(int socket);
void k_stop4_mproxy(int socket);
int k_get_vmif(int ifindex,int family);
int k_get_rlif(int ifindex, int family);
void k_add_ip6_mif (int socket, int ifindex);
void k_del_ip6_mif (int socket, int ifindex);
STATUS k_start6_mproxy(int socket);
void k_stop6_mproxy(int socket);
/*protocol independent code*/
STATUS k_mcast_join( pi_addr* pia, char* ifname);
STATUS k_mcast_leave(pi_addr* pia, char* ifname);
STATUS k_add_mfc(int iif_index, pi_addr *p_mcastgrp , pi_addr *p_origin, if_set *p_ttls);
STATUS k_del_mfc(pi_addr *p_mcastgrp, pi_addr *p_origin);
STATUS k_mcast_msfilter(pi_addr* p_addr, pa_list *p_addr_list,int fmode);
#endif
