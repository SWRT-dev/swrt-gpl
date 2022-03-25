/*
*    Copyright (c) 2003-2019 Broadcom
*    All Rights Reserved
*
<:label-BRCM:2019:DUAL/GPL:standard

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/

#ifndef _BR_BCM_HOOKS_IEEE1905_H
#define _BR_BCM_HOOKS_IEEE1905_H

#if IS_ENABLED(CONFIG_BCM_IEEE1905)
struct sk_buff;

unsigned int bcm_br_ieee1905_nf(struct sk_buff *skb);
void bcm_br_ieee1905_pt_add(void);
void bcm_br_ieee1905_pt_del(void);
#else
#define bcm_br_ieee1905_nf(arg) NF_ACCEPT
#define bcm_br_ieee1905_pt_add() do {} while (0)
#define bcm_br_ieee1905_pt_del() do {} while (0)
#endif // #if IS_ENABLED(CONFIG_BCM_IEEE1905)
#endif // _BR_BCM_HOOKS_IEEE1905_H
