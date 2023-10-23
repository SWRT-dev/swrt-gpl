/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

/*
 * ####################################
 *              Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netdevice.h>
//#include <linux/rculist.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include "directconnect_dp_device.h"
#include "directconnect_dp_dcmode_wrapper.h"
#include "directconnect_dp_macdb.h"
#include "directconnect_dp_parser.h"

#define DC_DP_PROC "dc_dp"
#define DC_DP_PROC_DBG "dbg"
#define DC_DP_PROC_DEV "dev"
#define DC_DP_PROC_DCMODE "dcmode"
#define DC_DP_PROC_MACDB "macdb"
#define DC_DP_PROC_PROTODB "protodb"

/*
 * ####################################
 *             Declaration
 * ####################################
 */

static int proc_read_dc_dp_dbg_seq_open(struct inode *, struct file *);
static ssize_t proc_write_dc_dp_dbg(struct file *, const char __user *, size_t , loff_t *);
static int proc_read_dc_dp_dev_show(struct seq_file *seq, void *v);
static int proc_read_dc_dp_dev_open(struct inode *, struct file *);
#if 0
static ssize_t proc_write_dc_dp_dev(struct file *, const char __user *, size_t , loff_t *);
#endif
static int proc_read_dc_dp_dcmode_show(struct seq_file *seq, void *v);
static int proc_read_dc_dp_dcmode_open(struct inode *, struct file *);
static int proc_read_dc_dp_macdb_show(struct seq_file *seq, void *v);
static int proc_read_dc_dp_macdb_open(struct inode *, struct file *);
static int proc_read_dc_dp_protodb_open(struct inode *, struct file *);
static int proc_read_dc_dp_protodb_show(struct seq_file *, void *);
static ssize_t proc_write_dc_dp_protodb(struct file *file,
                                             const char __user *buf,
                                             size_t count, loff_t *data);

#define DC_DP_F_ENUM_OR_STRING(name,value, short_name) name = value

/*Note: per bit one variable */
#define DC_DP_F_FLAG_LIST  \
    DC_DP_F_ENUM_OR_STRING(DC_DP_F_DEREGISTER, 0x00000001, "De-Register"), \
    DC_DP_F_ENUM_OR_STRING(DC_DP_F_FASTPATH,   0x00000010, "FASTPATH"), \
    DC_DP_F_ENUM_OR_STRING(DC_DP_F_DIRECTPATH,   0x00000020, "LITEPATH"),\
    DC_DP_F_ENUM_OR_STRING(DC_DP_F_SWPATH,      0x00000040, "SWPATH"),\
    DC_DP_F_ENUM_OR_STRING(DC_DP_F_QOS,       0x00001000, "QoS")

/*
 * ####################################
 *           Global Variable
 * ####################################
 */

#undef DC_DP_F_ENUM_OR_STRING
#define DC_DP_F_ENUM_OR_STRING(name,value, short_name) short_name
static int8_t *g_dc_dp_port_type_str[] = {
    DC_DP_F_FLAG_LIST
};
#undef DC_DP_F_ENUM_OR_STRING

#undef DC_DP_F_ENUM_OR_STRING
#define DC_DP_F_ENUM_OR_STRING(name,value, short_name) value
static uint32_t g_dc_dp_port_flag[] = {
    DC_DP_F_FLAG_LIST
};
#undef DC_DP_F_ENUM_OR_STRING

static int8_t *g_dc_dp_port_status_str[] = {
    "PORT_FREE",
    "PORT_ALLOCATED",
    "PORT_DEV_REGISTERED",
    "PORT_SUBIF_REGISTERED",
    "Invalid"
};

static int8_t *g_dc_dp_dbg_flag_str[] = {
    "dbg",            /*DC_DP_DBG_FLAG_DBG */

    "rx",            /*DC_DP_DBG_FLAG_DUMP_RX */
    "rx_data",        /*DC_DP_DBG_FLAG_DUMP_RX_DATA*/
    "rx_desc",        /*DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR */

    "tx",            /*DC_DP_DBG_FLAG_DUMP_TX */
    "tx_data",        /*DC_DP_DBG_FLAG_DUMP_TX_DATA */
    "tx_desc",        /*DC_DP_DBG_FLAG_DUMP_TX_DESCRIPTOR */

    /*the last one*/
    "err"
};

static uint32_t g_dc_dp_dbg_flag_list[] = {
    DC_DP_DBG_FLAG_DBG,

    DC_DP_DBG_FLAG_DUMP_RX,
    DC_DP_DBG_FLAG_DUMP_RX_DATA,
    DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR,

    DC_DP_DBG_FLAG_DUMP_TX,
    DC_DP_DBG_FLAG_DUMP_TX_DATA,
    DC_DP_DBG_FLAG_DUMP_TX_DESCRIPTOR,

    /*The last one*/
    DC_DP_DBG_FLAG_ERR
};

static inline int32_t _dc_dp_get_port_type_str_size(void)
{
    return ARRAY_SIZE(g_dc_dp_port_type_str);
}

static inline int32_t _dc_dp_get_dbg_flag_str_size(void)
{
    return ARRAY_SIZE(g_dc_dp_dbg_flag_str);
}

static inline int32_t _dc_dp_get_port_status_str_size(void)
{
    return ARRAY_SIZE(g_dc_dp_port_status_str);
}

static struct proc_dir_entry *g_dc_dp_proc = NULL;

static struct file_operations g_dc_dp_dbg_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_dc_dp_dbg_seq_open,
    .read       = seq_read,
    .write      = proc_write_dc_dp_dbg,
    .llseek     = seq_lseek,
    .release    = seq_release,
};

static struct file_operations g_dc_dp_dev_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_dc_dp_dev_open,
    .read       = seq_read,
#if 0
    .write      = proc_write_dc_dp_dev,
#endif
    .llseek     = seq_lseek,
    .release    = single_release,
};

static struct file_operations g_dc_dp_dcmode_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_dc_dp_dcmode_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static struct file_operations g_dc_dp_macdb_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_dc_dp_macdb_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static struct file_operations g_dc_dp_protodb_proc_fops = {
    .owner      = THIS_MODULE,
    .open       = proc_read_dc_dp_protodb_open,
    .read       = seq_read,
    .write      = proc_write_dc_dp_protodb,
    .llseek     = seq_lseek,
    .release    = single_release,
};

/*
 * ####################################
 *           Extern Variable
 * ####################################
 */

/*
 * ####################################
 *            Extern Function
 * ####################################
 */

/*
 * ####################################
 *            Local Function
 * ####################################
 */
static void proc_write_dc_dp_dbg_usage(void)
{
    pr_info("usage:\n");
    pr_info("    echo [enable|disable] dbg tx tx_data tx_desc rx rx_data rx_desc > /proc/%s/%s\n", DC_DP_PROC, DC_DP_PROC_DBG);
}

#if 0
static void proc_write_dc_dp_dev_usage(void)
{
    pr_info("usage:\n");
    //pr_info("    echo show dev <idx> > /proc/%s/%s\n", DC_DP_PROC, DC_DP_PROC_DEV);
    pr_info("    echo set dev <idx> umt_period <us> > /proc/%s/%s\n", DC_DP_PROC, DC_DP_PROC_DEV);
    pr_info("    echo set dev <idx> umt <0|1> > /proc/%s/%s\n", DC_DP_PROC, DC_DP_PROC_DEV);
    pr_info("    echo set dev <idx> dma <0|1> > /proc/%s/%s\n", DC_DP_PROC, DC_DP_PROC_DEV);
}
#endif

static void proc_write_dc_dp_protodb_usage(void)
{
    pr_info("usage:\n");
    pr_info("    echo add proto <value> pyld_start <value>"
            " pyldln_start <value> pyldln <value> > /proc/%s/%s\n",
            DC_DP_PROC, DC_DP_PROC_PROTODB);
    pr_info("    echo del proto <value> > /proc/%s/%s\n",
            DC_DP_PROC, DC_DP_PROC_PROTODB);
}

static int proc_read_dc_dp_dbg(struct seq_file *seq, void *v)
{
    int i;

    if (!capable(CAP_NET_ADMIN)) {
         printk("Read Permission denied");
         return 0;
    }

    seq_printf(seq, "dbg_flag=0x%08x\n", dc_dp_get_dbg_flag());
    seq_printf(seq, "Supported Flags =%d\n",
           _dc_dp_get_dbg_flag_str_size());
    seq_printf(seq, "Enabled Flags(0x%0x):", dc_dp_get_dbg_flag());

    for (i = 0; i < _dc_dp_get_dbg_flag_str_size(); i++)
        if ((dc_dp_get_dbg_flag() & g_dc_dp_dbg_flag_list[i]) == g_dc_dp_dbg_flag_list[i])
            seq_printf(seq, "%s ", g_dc_dp_dbg_flag_str[i]);

    seq_printf(seq, "\n");

    return 0;
}

static int proc_read_dc_dp_dbg_seq_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_dc_dp_dbg, NULL);
}

static ssize_t proc_write_dc_dp_dbg(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
    int len;
    char str[40];
    char *p;
    int f_enable;

    if (!capable(CAP_NET_ADMIN)) {
         printk("Write Permission denied");
         return 0;
    }

    len = min(count, (size_t)(sizeof(str) - 1));
    len -= copy_from_user(str, buf, len);
    while ( len && str[len - 1] <= ' ' )
        len--;
    str[len] = 0;
    for ( p = str; *p && *p <= ' '; p++, len-- );
    if ( !*p ) {
        goto help;
    }

    if (!strncmp(p, "enable", 6)) {
        f_enable = 1;
    } else if (!strncmp(p, "disable", 7)) {
        f_enable = 0;
    } else {
        goto help;
    }

#if 0
    for (i = 1; i < num; i++) {
        for (j = 0; j < _dc_dp_get_dbg_flag_str_size(); j++) {
            if (!strncmp(param_list[i],
                       g_dc_dp_dbg_flag_str[j])) {
                set_ltq_dbg_flag(dc_dp_get_dbg_flag(),
                         f_enable, g_dc_dp_dbg_flag_list[j]);
                break;
            }
        }
    }
#endif
    return len;

help:
    proc_write_dc_dp_dbg_usage();
    return count;
}

static int proc_read_dc_dp_dev_show(struct seq_file *seq, void *v)
{
    int32_t dev_idx = 0;
    int i;
    struct dc_dp_priv_dev_info *p_hw_dev_info = dc_dp_get_hw_device_head();
    struct dc_dp_priv_dev_info *p_sw_dev_info = dc_dp_get_sw_device_head();
    struct dc_dp_priv_dev_info *dev_info = NULL;

    if (!capable(CAP_NET_ADMIN)) {
         printk ("Read Permission denied");
         return 0;
    }

    for (dev_idx = 0; dev_idx < DC_DP_MAX_HW_DEVICE + DC_DP_MAX_SW_DEVICE; dev_idx++) {
        if (dev_idx < DC_DP_MAX_HW_DEVICE)
            dev_info = &p_hw_dev_info[dev_idx];
        else
            dev_info = &p_sw_dev_info[dev_idx - DC_DP_MAX_HW_DEVICE];

        if (dev_info->flags == DC_DP_DEV_FREE) {
            seq_printf(seq, "%d: Not registered\n", dev_idx);
            continue;
        }

        seq_printf(seq,
               "%d: module=0x%px(name:%8s) dev_port=%02d dp_port=%02d\n",
               dev_idx, dev_info->owner, dev_info->owner->name,
               dev_info->dev_port, dev_info->port_id);
        seq_printf(seq,  "    status:            %s\n",
            g_dc_dp_port_status_str[dev_info->flags]);

        seq_printf(seq, "    allocate_flags:    ");
        for (i = 0; i < _dc_dp_get_port_type_str_size(); i++) {
            if (dev_info->alloc_flags & g_dc_dp_port_flag[i])
                seq_printf(seq,  "%s ", g_dc_dp_port_type_str[i]);
        }
        seq_printf(seq, "\n");

        seq_printf(seq, "    cb->rx_fn:         0x%px\n",
            dev_info->cb.rx_fn);
        seq_printf(seq, "    cb->stop_fn:       0x%px\n",
            dev_info->cb.stop_fn);
        seq_printf(seq, "    cb->restart_fn:    0x%px\n",
            dev_info->cb.restart_fn);
        seq_printf(seq, "    cb->get_subif_fn:  0x%px\n",
            dev_info->cb.get_subif_fn);
        seq_printf(seq, "    cb->get_desc_info_fn:  0x%px\n",
            dev_info->cb.get_desc_info_fn);
        seq_printf(seq, "    cb->reset_mib_fn:  0x%px\n",
            dev_info->cb.reset_mib_fn);
        seq_printf(seq, "    cb->get_mib_fn:  0x%px\n",
            dev_info->cb.reset_mib_fn);
        seq_printf(seq, "    cb->recovery_fn:  0x%px\n",
            dev_info->cb.recovery_fn);

        seq_printf(seq, "    class2prio:        ");
        for (i = 0; i < DC_DP_MAX_SOC_CLASS; i++) {
            seq_printf(seq, "[%d->%d],", i, dev_info->class2prio[i]);
        }
        seq_printf(seq, "\n");
        seq_printf(seq, "    prio2devqos:          ");
        for (i = 0; i < DC_DP_MAX_DEV_CLASS; i++) {
            seq_printf(seq, "[%d->%d],", i, dev_info->prio2qos[i]);
        }
        seq_printf(seq, "\n");

        dc_dp_dcmode_wrapper_dump_proc(dev_info, seq);

        seq_printf(seq, "    num_subif:         %02d\n", dev_info->num_subif);
        for (i = 0; i < DC_DP_MAX_SUBIF_PER_DEV; i++) {
            if (dev_info->subif_info[i].flags) {
                seq_printf(seq,
                       "      [%02d]: subif=0x%04x(vap=%d) netif=0x%px(name=%s), device_name=%s\n",
                       i, dev_info->subif_info[i].subif,
                       (dev_info->subif_info[i].subif >> DC_DP_SUBIFID_OFFSET) & DC_DP_SUBIFID_MASK,
                       dev_info->subif_info[i].netif,
                       dev_info->subif_info[i].netif ? dev_info->subif_info[i].netif->name : "NULL",
                       dev_info->subif_info[i].device_name);
            }
        }
    }

    return 0;
}

static int proc_read_dc_dp_dev_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_dc_dp_dev_show, NULL);
}

static __always_inline char *parse_token(char **str, char *delim)
{
    *str = strim(*str);
    return strsep(str, delim);
}

static uint32_t btoi(char *str)
{
    unsigned int sum = 0;
    signed len = 0, i = 0;

    len = strlen(str);
    len = len - 1;
    while (len >= 0) {
        if (*(str + len) == '1')
            sum = (sum | (1 << i));
        i++;
        len--;
    }
    return sum;
}

static int32_t dc_dp_atoi(uint8_t *str)
{
    uint32_t n = 0;
    int32_t i = 0;
    int32_t nega_sign = 0;

    if (!str)
        return 0;
    //dp_replace_ch(str, strlen(str), '.', 0);
    //dp_replace_ch(str, strlen(str), ' ', 0);
    //dp_replace_ch(str, strlen(str), '\r', 0);
    //dp_replace_ch(str, strlen(str), '\n', 0);
    if (str[0] == 0)
        return 0;

    if (str[0] == 'b' || str[0] == 'B') {    /*binary format */
        n = btoi(str + 1);
    } else if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))) {
        /*hex format */
        str += 2;

        while (str[i]) {
            n = n * 16;
            if (('0' <= str[i] && str[i] <= '9')) {
                n += str[i] - '0';
            } else if (('A' <= str[i] && str[i] <= 'F')) {
                n += str[i] - 'A' + 10;
                ;
            } else if (('a' <= str[i] && str[i] <= 'f')) {
                n += str[i] - 'a' + 10;
                ;
            } else
                DC_DP_ERROR("Wrong value:%u\n", str[i]);

            i++;
        }

    } else {
        if (str[i] == '-') {    /*negative sign */
            nega_sign = 1;
            i++;
        }
        while (str[i]) {
            n *= 10;
            n += str[i] - '0';
            i++;
        }
    }
    if (nega_sign)
        n = -(int)n;
    return n;
}

#if 0
static ssize_t proc_write_dc_dp_dev(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
    int len;
    char str[40] = {0};
    char *p, *param_name = NULL, *param_val = NULL;
    int32_t dev_idx;
    char *delim = " \t\n\v\f\r";
    int32_t value;

    len = min(count, (size_t)(sizeof(str) - 1));
    len -= copy_from_user(str, buf, len);

    p = str;

    if (!strncmp(p, "set", 3)) {
        /* set ... */
        parse_token(&p, delim); //Skip command 'set'
        if (!p) {
            goto help;
        }
        param_name = parse_token(&p, delim);
        if (!p) {
            goto help;
        }
        if (strncmp(param_name, "dev", 3)) {
            goto help;
        }
        param_val = parse_token(&p, delim);
        if (!p) {
            goto help;
        }
        dev_idx = dc_dp_atoi(param_val);
        if (dev_idx >= DC_DP_MAX_DEV_NUM) {
            goto help;
        }
        param_name = parse_token(&p, delim);
        if (!p) {
            goto help;
        }
        param_val = parse_token(&p, delim);
        value = dc_dp_atoi(param_val);

        if (!strncmp(param_name, "umt_period", strlen("umt_period"))) {
            g_priv_info[dev_idx].umt_period = value;
#if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE
            ltq_umt_set_period(g_priv_info[dev_idx].umt_id, g_priv_info[dev_idx].port_id, value);
#else /* #if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE */
            ltq_umt_set_period(value);
#endif /* #else */

        } else if (!strncmp(param_name, "umt", 3)) {
            if (value == 0) {
#if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE
                ltq_umt_enable(g_priv_info[dev_idx].umt_id, g_priv_info[dev_idx].port_id, 0);
#else /* #if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE */
                ltq_umt_enable(0);
#endif /* #else */

            } else {
#if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE
                ltq_umt_enable(g_priv_info[dev_idx].umt_id, g_priv_info[dev_idx].port_id, 1);
#else /* #if defined(CONFIG_LTQ_UMT_EXPAND_MODE) && CONFIG_LTQ_UMT_EXPAND_MODE */
                ltq_umt_enable(1);
#endif /* #else */
            }

        } else if (!strncmp(param_name, "dma", 3)) {
            if (value == 0) {
                ltq_dma_chan_off(g_priv_info[dev_idx].dma_ch);
            } else {
                ltq_dma_chan_on(g_priv_info[dev_idx].dma_ch);
            }
        } else {
            goto help;
        }
    } else {
        goto help;
    }

    return len;

help:
    proc_write_dc_dp_dev_usage();
    return count;
}
#endif

extern struct dc_dp_dcmode **dc_dp_get_dcmode_head(void);
static int proc_read_dc_dp_dcmode_show(struct seq_file *seq, void *v)
{
    int32_t dcmode_idx;
    struct dc_dp_dcmode **dcmode = dc_dp_get_dcmode_head();

    if (!capable(CAP_NET_ADMIN)) {
         printk("Read Permission denied");
         return 0;
    }

    for (dcmode_idx = 0; dcmode_idx < DC_DP_DCMODE_MAX; dcmode_idx++) {
        if (dcmode[dcmode_idx] == NULL) {
            seq_printf(seq, "%d: Not registered\n", dcmode_idx);
            continue;
        }
        seq_printf(seq, "%d:\n", dcmode_idx);
        seq_printf(seq, "    dcmode_cap:         0x%0x\n",
            (uint32_t) dcmode[dcmode_idx]->dcmode_cap);

        seq_printf(seq, "    register_dev:         0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->register_dev);
        seq_printf(seq, "    register_subif:       0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->register_subif);
        seq_printf(seq, "    xmit:    0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->xmit);
        seq_printf(seq, "    handle_ring_sw:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->handle_ring_sw);
        seq_printf(seq, "    add_session_shortcut_forward:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->add_session_shortcut_forward);
        seq_printf(seq, "    disconn_if:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->disconn_if);
        seq_printf(seq, "    get_netif_stats:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->get_netif_stats);
        seq_printf(seq, "    clear_netif_stats:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->clear_netif_stats);
        seq_printf(seq, "    register_qos_class2prio_cb:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->register_qos_class2prio_cb);
        seq_printf(seq, "    map_class2devqos:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->map_class2devqos);
        seq_printf(seq, "    alloc_skb:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->alloc_skb);
        seq_printf(seq, "    free_skb:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->free_skb);
        seq_printf(seq, "    change_dev_status:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->change_dev_status);
        seq_printf(seq, "    get_wol_cfg:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->get_wol_cfg);
        seq_printf(seq, "    set_wol_cfg:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->set_wol_cfg);
        seq_printf(seq, "    set_wol_ctrl:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->set_wol_ctrl);
        seq_printf(seq, "    get_wol_ctrl_status:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->get_wol_ctrl_status);
        seq_printf(seq, "    dump_proc:  0x%px\n",
            dcmode[dcmode_idx]->dcmode_ops->dump_proc);
    }

    return 0;
}

static int proc_read_dc_dp_dcmode_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_dc_dp_dcmode_show, NULL);
}

static int proc_read_dc_dp_macdb_show(struct seq_file *seq, void *v)
{
    if (!capable(CAP_NET_ADMIN)) {
         printk("Read Permission denied");
         return 0;
    }

    dc_dp_macdb_seq_dump(seq);
    return 0;
}

static int proc_read_dc_dp_protodb_show(struct seq_file *seq, void *v)
{
    if (!capable(CAP_NET_ADMIN)) {
         printk("Read Permission denied");
         return 0;
    }

    dc_dp_parser_dump_proto(seq);
    return 0;
}

static int proc_read_dc_dp_macdb_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_dc_dp_macdb_show, NULL);
}

static int proc_read_dc_dp_protodb_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_read_dc_dp_protodb_show, NULL);
}

static void
parse_and_del_protodb_entry(char *p)
{
    char *param_name = NULL, *param_val = NULL;
    char *delim = " \t\n\v\f\r";
    uint16_t proto;

    param_name = parse_token(&p, delim);
    if (!p)
        goto help;

    if (!strncmp(param_name, "proto", strlen("proto"))) {
        param_val = parse_token(&p, delim);
        proto = dc_dp_atoi(param_val);
        if (proto <= 0)
            goto help;
    } else
        goto help;

    dc_dp_parser_del_proto(proto);
    return;
help:
    proc_write_dc_dp_protodb_usage();
}

static void
parse_and_add_protodb_entry(char *p)
{
    char *param_name = NULL, *param_val = NULL;
    char *delim = " \t\n\v\f\r";
    uint16_t proto, payload_start_offset, payloadlen_start_offset;
    uint32_t pyld_len;

    param_name = parse_token(&p, delim);
    if (!p)
        goto help;

    if (!strncmp(param_name, "proto", strlen("proto"))) {
        param_val = parse_token(&p, delim);
        if (!p)
            goto help;
        proto = dc_dp_atoi(param_val);
        if (proto <= 0)
            goto help;
    } else
        goto help;

    param_name = parse_token(&p, delim);
    if (!p)
        goto help;

    if (!strncmp(param_name, "pyld_start", strlen("pyld_start"))) {
        param_val = parse_token(&p, delim);
        if (!p)
            goto help;
        payload_start_offset = dc_dp_atoi(param_val);
        if (payload_start_offset <= 0)
            goto help;
    } else
        goto help;

    param_name = parse_token(&p, delim);
    if (!p)
        goto help;

    if (!strncmp(param_name, "pyldln_start", strlen("pyldln_start"))) {
        param_val = parse_token(&p, delim);
        if (!p)
            goto help;
        payloadlen_start_offset = dc_dp_atoi(param_val);
        if (payloadlen_start_offset <= 0)
            goto help;
    } else
        goto help;

    param_name = parse_token(&p, delim);
    if (!p)
        goto help;

    if (!strncmp(param_name, "pyldln", strlen("pyldln"))) {
        param_val = parse_token(&p, delim);
        /* Last argument: no NULL check required */

        pyld_len = dc_dp_atoi(param_val);
        if (pyld_len <= 0)
            goto help;
    } else
        goto help;
    dc_dp_parser_add_proto(proto, payload_start_offset, payloadlen_start_offset, pyld_len);
    return;
help:
    proc_write_dc_dp_protodb_usage();
}

static ssize_t proc_write_dc_dp_protodb(struct file *file,
               const char __user *buf, size_t count, loff_t *data)
{
    int len;
    char str[128] = {0};
    char *p, *param_name = NULL;
    char *delim = " \t\n\v\f\r";

    if (!capable(CAP_NET_ADMIN)) {
         printk("Write Permission denied");
         return 0;
    }

    len = min(count, (size_t)(sizeof(str) - 1));
    len -= copy_from_user(str, buf, len);

    p = str;

    /* skip spaces */
    while (len && str[len - 1] <= ' ')
        len--;

    str[len] = 0;
    for (p = str; *p && (*p) <= ' '; p++, len--);

    if (!(*p))
        return count;

    /* Parse 'add' / 'del' */
    param_name = parse_token(&p, delim);
    if (!p)
        goto help;
    if (!strncmp(param_name, "add", 3))
        parse_and_add_protodb_entry(p);
    else if (!strncmp(param_name, "del", 3))
        parse_and_del_protodb_entry(p);
    else
        goto help;
    return len;

help:
    proc_write_dc_dp_protodb_usage();
    return count;
}

int32_t dc_dp_proc_init(void)
{
    struct proc_dir_entry *entry;

    g_dc_dp_proc = proc_mkdir(DC_DP_PROC, NULL);
    if (!g_dc_dp_proc)
        return -ENOMEM;

    entry = proc_create_data(DC_DP_PROC_DBG, 0, g_dc_dp_proc,
            &g_dc_dp_dbg_proc_fops, NULL);
    if (!entry)
        goto __dbg_proc_err;

    entry = proc_create_data(DC_DP_PROC_DEV, 0, g_dc_dp_proc,
            &g_dc_dp_dev_proc_fops, NULL);
    if (!entry)
        goto __dev_proc_err;

    entry = proc_create_data(DC_DP_PROC_DCMODE, 0, g_dc_dp_proc,
            &g_dc_dp_dcmode_proc_fops, NULL);
    if (!entry)
        goto __dcmode_proc_err;

    entry = proc_create_data(DC_DP_PROC_MACDB, 0, g_dc_dp_proc,
            &g_dc_dp_macdb_proc_fops, NULL);
    if (!entry)
        goto __macdb_proc_err;

    entry = proc_create_data(DC_DP_PROC_PROTODB, 0, g_dc_dp_proc,
            &g_dc_dp_protodb_proc_fops, NULL);
    if (!entry)
        goto __protodb_proc_err;

    return 0;

__protodb_proc_err:
    remove_proc_entry(DC_DP_PROC_MACDB, g_dc_dp_proc);

__macdb_proc_err:
    remove_proc_entry(DC_DP_PROC_DCMODE, g_dc_dp_proc);

__dcmode_proc_err:
    remove_proc_entry(DC_DP_PROC_DEV, g_dc_dp_proc);

__dev_proc_err:
    remove_proc_entry(DC_DP_PROC_DBG, g_dc_dp_proc);

__dbg_proc_err:
    proc_remove(g_dc_dp_proc);
    return -ENOMEM;
}

void dc_dp_proc_exit(void)
{
    remove_proc_entry(DC_DP_PROC_PROTODB, g_dc_dp_proc);
    remove_proc_entry(DC_DP_PROC_MACDB, g_dc_dp_proc);
    remove_proc_entry(DC_DP_PROC_DCMODE, g_dc_dp_proc);
    remove_proc_entry(DC_DP_PROC_DEV, g_dc_dp_proc);
    remove_proc_entry(DC_DP_PROC_DBG, g_dc_dp_proc);
    proc_remove(g_dc_dp_proc);
    g_dc_dp_proc = NULL;
}
