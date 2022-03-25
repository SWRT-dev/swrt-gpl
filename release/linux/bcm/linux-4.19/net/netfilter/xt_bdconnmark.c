/*
 *     xt_bdconnmark - Netfilter module to operate on Bitdefender connection
 *     marks
 *
 *     Based on:
 *
 *     xt_connmark - Netfilter module to operate on connection marks
 *
 *     Copyright (C) 2002,2004 MARA Systems AB <http://www.marasystems.com>
 *     by Henrik Nordstrom <hno@marasystems.com>
 *     Copyright © CC Computer Consultants GmbH, 2007 - 2008
 *     Jan Engelhardt <jengelh@medozas.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_connmark.h>

MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_BDCONNMARK");
MODULE_ALIAS("ip6t_BDCONNMARK");
MODULE_ALIAS("ipt_bdconnmark");
MODULE_ALIAS("ip6t_bdconnmark");

static unsigned int
bdconnmark_tg_shift(struct sk_buff *skb, const struct xt_connmark_tginfo2 *info)
{
       enum ip_conntrack_info ctinfo;
       u_int32_t new_targetmark;
       struct nf_conn *ct;
       u_int32_t newmark;

       ct = nf_ct_get(skb, &ctinfo);
       if (ct == NULL)
               return XT_CONTINUE;

       switch (info->mode) {
       case XT_CONNMARK_SET:
               newmark = (ct->bdmark & ~info->ctmask) ^ info->ctmark;
               if (info->shift_dir == D_SHIFT_RIGHT)
                       newmark >>= info->shift_bits;
               else
                       newmark <<= info->shift_bits;

               if (ct->bdmark != newmark) {
                       ct->bdmark = newmark;
               }
               break;
       case XT_CONNMARK_SAVE:
               new_targetmark = (skb->mark & info->nfmask);
               if (info->shift_dir == D_SHIFT_RIGHT)
                       new_targetmark >>= info->shift_bits;
               else
                       new_targetmark <<= info->shift_bits;

               newmark = (ct->bdmark & ~info->ctmask) ^
                         new_targetmark;
               if (ct->bdmark != newmark) {
                       ct->bdmark = newmark;
               }
               break;
       case XT_CONNMARK_RESTORE:
               new_targetmark = (ct->bdmark & info->ctmask);
               if (info->shift_dir == D_SHIFT_RIGHT)
                       new_targetmark >>= info->shift_bits;
               else
                       new_targetmark <<= info->shift_bits;

               newmark = (skb->mark & ~info->nfmask) ^
                         new_targetmark;
               skb->mark = newmark;
               break;
       }
       return XT_CONTINUE;
}

static unsigned int
bdconnmark_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
       const struct xt_connmark_tginfo1 *info = par->targinfo;
       const struct xt_connmark_tginfo2 info2 = {
               .ctmark = info->ctmark,
               .ctmask = info->ctmask,
               .nfmask = info->nfmask,
               .mode   = info->mode,
       };

       return bdconnmark_tg_shift(skb, &info2);
}

static unsigned int
bdconnmark_tg_v2(struct sk_buff *skb, const struct xt_action_param *par)
{
       const struct xt_connmark_tginfo2 *info = par->targinfo;

       return bdconnmark_tg_shift(skb, info);
}

static int bdconnmark_tg_check(const struct xt_tgchk_param *par)
{
       int ret;

       ret = nf_ct_netns_get(par->net, par->family);
       if (ret < 0)
               pr_info("cannot load conntrack support for proto=%u\n",
                       par->family);
       return ret;
}

static void bdconnmark_tg_destroy(const struct xt_tgdtor_param *par)
{
       nf_ct_netns_put(par->net, par->family);
}

static bool
bdconnmark_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
       const struct xt_connmark_mtinfo1 *info = par->matchinfo;
       enum ip_conntrack_info ctinfo;
       const struct nf_conn *ct;

       ct = nf_ct_get(skb, &ctinfo);
       if (ct == NULL)
               return false;

       return ((ct->bdmark & info->mask) == info->mark) ^ info->invert;
}

static int bdconnmark_mt_check(const struct xt_mtchk_param *par)
{
       int ret;

       ret = nf_ct_netns_get(par->net, par->family);
       if (ret < 0)
               pr_info("cannot load conntrack support for proto=%u\n",
                       par->family);
       return ret;
}

static void bdconnmark_mt_destroy(const struct xt_mtdtor_param *par)
{
       nf_ct_netns_put(par->net, par->family);
}

static struct xt_target bdconnmark_tg_reg[] __read_mostly = {
        {
                .name           = "BDCONNMARK",
                .revision       = 1,
                .family         = NFPROTO_UNSPEC,
                .checkentry     = bdconnmark_tg_check,
                .target         = bdconnmark_tg,
                .targetsize     = sizeof(struct xt_connmark_tginfo1),
                .destroy        = bdconnmark_tg_destroy,
                .me             = THIS_MODULE,
        },
       {
               .name           = "BDCONNMARK",
               .revision       = 2,
               .family         = NFPROTO_UNSPEC,
               .checkentry     = bdconnmark_tg_check,
               .target         = bdconnmark_tg_v2,
               .targetsize     = sizeof(struct xt_connmark_tginfo2),
               .destroy        = bdconnmark_tg_destroy,
               .me             = THIS_MODULE,
       }
};

static struct xt_match bdconnmark_mt_reg __read_mostly = {
       .name           = "bdconnmark",
       .revision       = 1,
       .family         = NFPROTO_UNSPEC,
       .checkentry     = bdconnmark_mt_check,
       .match          = bdconnmark_mt,
       .matchsize      = sizeof(struct xt_connmark_mtinfo1),
       .destroy        = bdconnmark_mt_destroy,
       .me             = THIS_MODULE,
};

static int __init bdconnmark_mt_init(void)
{
       int ret;

        ret = xt_register_targets(bdconnmark_tg_reg,
                                 ARRAY_SIZE(bdconnmark_tg_reg));
       if (ret < 0)
               return ret;
       ret = xt_register_match(&bdconnmark_mt_reg);
       if (ret < 0) {
                xt_unregister_targets(bdconnmark_tg_reg,
                                     ARRAY_SIZE(bdconnmark_tg_reg));
               return ret;
       }
       return 0;
}

static void __exit bdconnmark_mt_exit(void)
{
       xt_unregister_match(&bdconnmark_mt_reg);
        xt_unregister_targets(bdconnmark_tg_reg, ARRAY_SIZE(bdconnmark_tg_reg));
}

module_init(bdconnmark_mt_init);
module_exit(bdconnmark_mt_exit);
