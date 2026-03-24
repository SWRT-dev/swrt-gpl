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

/* Includes */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>
#if defined(CONFIG_X86_INTEL_LGM)
#include <linux/pp_api.h>
#include <dt-bindings/net/intel,lgm-cqm.h>
#include <net/datapath_api_gswip32.h>
#else
#include <linux/dma/lantiq_dmax.h>
#endif /* CONFIG_X86_INTEL_LGM */

#include <net/switch_api/lantiq_gsw_api.h>
#if IS_ENABLED(CONFIG_PPA)
#include <net/ppa/ppa_api.h>
#endif /* CONFIG_PPA */

#include <directconnect_dp_api.h>
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include <dc_mode_sw_common.h>

#if defined(CONFIG_X86_INTEL_LGM)
#include <net/qos_mgr/qos_mgr_common.h>
#include <net/qos_mgr/qos_mgr_hook.h>
#endif /* CONFIG_X86_INTEL_LGM */

/* Defines */
#define DCMODE_EXT_DRV_MODULE_NAME "dc_mode-ext"
#define DCMODE_EXT_DRV_MODULE_VERSION "1.0"

#define DCMODE_EXT_BRIDGE_FLOW_LEARNING    1

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define DCMODE_EXT_MAX_PORT          20
#if defined(CONFIG_X86_INTEL_LGM)
#if defined(CONFIG_WLAN_MAX_VAPS) && (CONFIG_WLAN_MAX_VAPS <= 16)
#define DCMODE_EXT_MAX_SUBIF_PER_DEV 16
#else
#define DCMODE_EXT_MAX_SUBIF_PER_DEV 32
#endif /* CONFIG_WLAN_MAX_VAPS */
#define DCMODE_EXT_ALT_SUBIF_PER_DEV 16
#define DCMODE_EXT_SUBIFID_BITSZ     5
#define DCMODE_EXT_SUBIFID_MASK      0x1F
#else
#define DCMODE_EXT_MAX_SUBIF_PER_DEV 16
#define DCMODE_EXT_SUBIFID_BITSZ     4
#define DCMODE_EXT_SUBIFID_MASK      0xF
#define DEV2SOC_FRAG_EXCEPTION_HANDLING
#endif /* CONFIG_X86_INTEL_LGM */
#define DCMODE_EXT_SUBIFID_OFFSET    8
#define DCMODE_EXT_GET_SUBIFIDX(subif_id) \
    ((subif_id >> DCMODE_EXT_SUBIFID_OFFSET) & DCMODE_EXT_SUBIFID_MASK)
#define DCMODE_EXT_GET_BITS(var, pos, width) \
    (((var) >> (pos)) & ((0x1 << (width)) - 1))
#define DCMODE_EXT_SET_BITS(var, pos, width, val) \
    do { \
        (var) &= ~(((0x1 << (width)) - 1) << (pos)); \
        (var) |= (((val) & ((0x1 << (width)) - 1)) << (pos)); \
    } while (0)
#define MULTIPORT_WORKAROUND_MAX_SUBIF_NUM 8
#define MULTIPORT_WORKAROUND_SUBIFID_MASK \
    (MULTIPORT_WORKAROUND_MAX_SUBIF_NUM << DCMODE_EXT_SUBIFID_OFFSET)

#define DC_DP_DEFINE_LOCK(lock) DEFINE_MUTEX(lock)
#define DC_DP_LOCK    mutex_lock
#define DC_DP_UNLOCK  mutex_unlock

#define DCMODE_EXT_MAX_DEV_NUM      4
#define DCMODE_EXT_MAX_DEV_PORT_NUM (DCMODE_EXT_MAX_DEV_NUM << 1)
#if defined(CONFIG_INTEL_UMT_INTERVAL_DEFAULT)
#define DCMODE_EXT_DEF_UMT_PERIOD   CONFIG_INTEL_UMT_INTERVAL_DEFAULT
#else
#define DCMODE_EXT_DEF_UMT_PERIOD   20 /* in us */
#endif /* CONFIG_INTEL_UMT_INTERVAL_DEFAULT */

#define DC_DP_MAX_SOC_CLASS        16

/* LGM Chip Identification Register */
#define MPS_CHIPID 0x4
#define MPS_CHIPID_VER_A0 0x0
#define MPS_CHIPID_VER_A2 0x1
#define MPS_CHIPID_VER_A3 0x2
#define MPS_CHIPID_VER_B0 0x3

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING

#define FRAG_READY_ITER_CNTR        100
#define DMA1TX_SIZE_ALIGNMENT_MASK  0x0007 /* DMA1TX DMA size alignemnt size is 8 bytes */
#define OWN(desc_p)                 (desc_p->status.field.own)
#define SOP(desc_p)                 (desc_p->status.field.sop)
#define EOP(desc_p)                 (desc_p->status.field.eop)
#define FRAG_READY(desc_p)          (desc_p->status.field.dic)
#define BYTE_OFFSET(desc_p)         (desc_p->status.field.byte_offset)
#define DATA_LEN(desc_p)            (desc_p->status.field.data_len)
#define DATA_POINTER(desc_p)        (desc_p->data_pointer)

#define CHAN_SOC2DEV_P(ctx) (&((ctx)->shared_info->ch[DCMODE_CHAN_SOC2DEV]))
#define CHAN_SOC2DEV_RET_P(ctx) (&((ctx)->shared_info->ch[DCMODE_CHAN_SOC2DEV_RET]))
#define CHAN_DEV2SOC_P(ctx) (&((ctx)->shared_info->ch[DCMODE_CHAN_DEV2SOC]))
#define CHAN_DEV2SOC_RET_P(ctx) (&((ctx)->shared_info->ch[DCMODE_CHAN_DEV2SOC_RET]))
#define CHAN_DEV2SOC_EX_P(ctx) (&((ctx)->shared_info->ch[DCMODE_CHAN_DEV2SOC_EXCEPT]))

typedef enum dcmode_chan_id {
    DCMODE_CHAN_SOC2DEV = 0,
    DCMODE_CHAN_SOC2DEV_RET,
    DCMODE_CHAN_DEV2SOC,
    DCMODE_CHAN_DEV2SOC_RET,
    DCMODE_CHAN_DEV2SOC_EXCEPT,
    DCMODE_CHAN_MAX,
} dcmode_chan_id_t;

typedef struct __attribute__ ((__packed__))
{
    uint32_t dev2soc_ring_idx;
} dev2soc_frag_except_bd_t;

typedef struct __attribute__ ((__packed__))
{
    volatile uint32_t wp;
    volatile uint32_t rp;

    uint32_t size;
    uint32_t quota;
} dcmode_ring_track_t;

typedef struct
{
    void *       phys; /* Ring physical base address */
    void *       virt; /* Ring virtual base address */
    unsigned int size; /* Ring size (=4Bytes) */
    unsigned int desc_dwsz; /* Ring size (=4Bytes) */

    dcmode_ring_track_t trck;  // Ring pointers and metadata tracker
} dcmode_ring_t;

typedef struct
{
    void *       phys; /* Counter physical address */
    void *       virt; /* Counter virtual address */
    unsigned int size; /* Counter size (=4Bytes) */
    unsigned int rw_endian; /* Big or little endian */
    unsigned int rw_mode; /* Cumulative or Incremental mode */
} dcmode_cntr_t;

typedef struct
{
    dcmode_chan_id_t  id  ;  // Channel ID

    /* Ring */
    dcmode_ring_t     ring; /* Ring */

    /* Counter */
    dcmode_cntr_t     write_cntr;
    dcmode_cntr_t     read_cntr;
} dcmode_chan_t;
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

struct dcmode_ext_subif_info {
    struct net_device  *netif;   /*! pointer to net_device*/
};

struct dcmode_ext_dev_shared_info
{
#define DCMODE_DEV_STATUS_FREE    0x0
#define DCMODE_DEV_STATUS_USED    0x1
    int32_t status;
    int32_t port_id;
    int32_t alt_port_id; /* Multiport reference port id */
    int32_t ref_count;
    int32_t cqm_pid;
    int32_t umt_id;
    int32_t umt_period;
    uint32_t dma_ctrlid;
    uint32_t dma_cid;
    uint32_t dma_ch;
    int32_t tx_policy_base;
    int32_t tx_policy_num;
    uint8_t high_4bits;
    int32_t num_bufpools;
    struct dc_dp_buf_pool *buflist;
#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    dcmode_chan_t    ch[DCMODE_CHAN_MAX]; // Array of Rings. Typically 4
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */
    uint32_t dc_cap;
    uint32_t dev_alloc_flags;
    int32_t num_subif;
    struct dcmode_ext_subif_info subif_info[DCMODE_EXT_MAX_SUBIF_PER_DEV];
};

struct dcmode_ext_dev_info
{
#define DCMODE_EXT_DEV_STATUS_FREE    0x0
#define DCMODE_EXT_DEV_STATUS_USED    0x1
    int32_t status;
    int32_t port_id;
    uint32_t alloc_flags;
    int32_t num_subif;

    /* shared info */
    struct dcmode_ext_dev_shared_info *shared_info;
};
static struct dcmode_ext_dev_shared_info g_dcmode_ext_dev_shinfo[DCMODE_EXT_MAX_DEV_NUM];
static struct dcmode_ext_dev_info g_dcmode_ext_dev[DCMODE_EXT_MAX_DEV_PORT_NUM];
static struct dcmode_ext_dev_info *g_dcmode_ext_dev_p[DCMODE_EXT_MAX_PORT] = {NULL} ;
static struct dp_cap g_dp_cap = {0};
DC_DP_DEFINE_LOCK(g_dcmode_ext_dev_lock);
static int32_t g_dcmode_ext_init_ok = 0;
static enum dc_dp_dev_type g_dev_type = 0;
static uint8_t g_chip_ver = 0;
static int reboot_on_buffer_leak __read_mostly;
module_param(reboot_on_buffer_leak, int, 0644);
MODULE_PARM_DESC(reboot_on_buffer_leak, "Reboot on buffer leak (default: off)");

/* Function prototypes */
/* Local */
static int32_t
dcmode_ext_rx_cb(struct net_device *rxif, struct net_device *txif,
                 struct sk_buff *skb, int32_t len);
static int32_t
dcmode_ext_get_netif_subifid_cb(struct net_device *netif,
                                struct sk_buff *skb, void *subif_data,
                                uint8_t dst_mac[MAX_ETH_ALEN],
                                dp_subif_t *subif, uint32_t flags);

static inline int32_t
dcmode_ext_setup_ring_resources(struct dcmode_ext_dev_info *dev_info, int32_t port_id,
                                struct dp_dev_data *dp_device, struct dc_dp_res *res, uint32_t flags);
static inline void
dcmode_ext_cleanup_ring_resources(struct dcmode_ext_dev_info *dev_info, int32_t port_id, struct dc_dp_res *res, uint32_t flags);
static inline uint8_t
_dc_dp_get_class2devqos(uint8_t *class2prio, uint8_t *prio2devqos, uint8_t class);

#if 0
#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
static void _dc_dp_dump_raw_data(char *buf, int len, char *prefix_str);
static void _dc_dp_dump_rx_pmac(struct pmac_rx_hdr *pmac);
#endif /* CONFIG_DIRECTCONNECT_DP_DBG */
#endif /* 0 */

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
#define _MMB_OPS_ADDR(a) (volatile void __iomem *)(a)

static inline void
dcmode_raw_writel(uint32_t val, volatile void __iomem *addr)
{
    writel(val, _MMB_OPS_ADDR(addr));
}

static inline uint32_t
dcmode_raw_readl(const volatile void __iomem *addr)
{
    uint32_t val = readl(_MMB_OPS_ADDR(addr));
    return val;
}

static inline uint32_t
dcmode_read_cntr(dcmode_chan_t *ch)
{
    return dcmode_raw_readl(ch->read_cntr.virt);
}

static inline void
dcmode_write_cntr(dcmode_chan_t *ch, uint32_t write_val)
{
    return dcmode_raw_writel(write_val, ch->write_cntr.virt);
}
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

/*
 * ========================================================================
 * Local Interface API
 * ========================================================================
 */

static inline bool is_multiport(uint32_t alloc_flags)
{
    return (0 != (alloc_flags & DC_DP_F_MULTI_PORT));
}

static inline bool is_multiport_main(uint32_t alloc_flags)
{
    return ((0 != (alloc_flags & DC_DP_F_MULTI_PORT)) &&
            (0 == (alloc_flags & DC_DP_F_SHARED_RES)));
}

static inline bool is_multiport_sub(uint32_t alloc_flags)
{
    return ((0 != (alloc_flags & DC_DP_F_MULTI_PORT)) &&
            (0 != (alloc_flags & DC_DP_F_SHARED_RES)));
}

static inline bool is_multiport_main_by_subif(struct dp_subif *subif_id)
{
    return (DCMODE_EXT_GET_SUBIFIDX(subif_id->subif) >= MULTIPORT_WORKAROUND_MAX_SUBIF_NUM);
}

static inline void
multiport_wa_forward_map_subifid(struct dcmode_ext_dev_info *dev_ctx, struct dp_subif *subif_id)
{
    /* Multiport 1st device? */
    if (is_multiport_main(dev_ctx->alloc_flags))
        subif_id->subif |= MULTIPORT_WORKAROUND_SUBIFID_MASK;
    /* Multiport sub-sequent device? */
    else
        subif_id->port_id = dev_ctx->shared_info->port_id;
}

static inline void
multiport_wa_reverse_map_subifid(struct dcmode_ext_dev_info *dev_ctx, struct dp_subif *subif_id)
{
    /* Multiport 1st device? */
    if (is_multiport_main_by_subif(subif_id))
        subif_id->subif &= ~MULTIPORT_WORKAROUND_SUBIFID_MASK;

    /* Multiport sub-sequent device? */
    else
        subif_id->port_id = dev_ctx->shared_info->alt_port_id;
}

static inline bool is_multiport_sub_by_subif(dp_subif_t *subif_id)
{
    return (DCMODE_EXT_GET_SUBIFIDX(subif_id->subif) < MULTIPORT_WORKAROUND_MAX_SUBIF_NUM);
}

#if defined(CONFIG_X86_INTEL_LGM)
static inline bool
is_required_buffer_marking(struct dc_dp_dev *devspec, uint32_t alloc_flags)
{
    bool ret = false;

    if (devspec->dev_cap_req & DC_DP_F_DEV_REQ_BUFFER_MARKING) {
        ret = true;

    /* Default to be set for FAST_WLAN */
    } else if ((alloc_flags & DC_DP_F_FAST_WLAN) ||
               !(alloc_flags & DC_DP_F_FAST_DSL)) {
        ret = true;
    }

    return ret;
}
#endif /* CONFIG_X86_INTEL_LGM */

static struct dcmode_ext_dev_shared_info *
dcmode_ext_alloc_dev_shared_ctx(int32_t port_id, int32_t ref_port_id, uint32_t alloc_flags)
{
    int32_t dev_shinfo_idx;
    struct dcmode_ext_dev_shared_info *dev_shinfo_ctx = NULL;
    struct dcmode_ext_dev_info *ref_dev_ctx = g_dcmode_ext_dev_p[ref_port_id];

    /* Multiport 2nd device? */
    if (is_multiport_sub(alloc_flags) && ref_dev_ctx) {
        dev_shinfo_ctx = ref_dev_ctx->shared_info;
        if (!dev_shinfo_ctx) {
            DC_DP_ERROR("failed to find device shared context "
                "for port:%d!\n", port_id);
            goto err_out;
        }

        dev_shinfo_ctx->alt_port_id = port_id;
        dev_shinfo_ctx->ref_count++;
    } else {
        /* Find a free device shinfo index */
        for (dev_shinfo_idx = 0; dev_shinfo_idx < DCMODE_EXT_MAX_DEV_NUM; dev_shinfo_idx++) {
            if (g_dcmode_ext_dev_shinfo[dev_shinfo_idx].status != DCMODE_EXT_DEV_STATUS_USED) {
                break;
            }
        }

        if (dev_shinfo_idx >= DCMODE_EXT_MAX_DEV_NUM) {
            DC_DP_ERROR("failed to allocate device shared context "
                "for port:%d!\n", port_id);
            goto err_out;
        }
        dev_shinfo_ctx = &g_dcmode_ext_dev_shinfo[dev_shinfo_idx];

        memset(dev_shinfo_ctx, 0, sizeof(struct dcmode_ext_dev_shared_info));
        dev_shinfo_ctx->status = DCMODE_EXT_DEV_STATUS_USED;

        /* Multiport 2nd radio? */
        if (is_multiport_sub(alloc_flags)) {
            dev_shinfo_ctx->port_id = ref_port_id;
            dev_shinfo_ctx->alt_port_id = port_id;
        } else
            dev_shinfo_ctx->port_id = port_id;

        dev_shinfo_ctx->ref_count = 1;
    }

err_out:
    return dev_shinfo_ctx;
}

static inline void
dcmode_ext_free_dev_shared_ctx(struct dcmode_ext_dev_shared_info *dev_shinfo_ctx)
{
    if (dev_shinfo_ctx) {
        dev_shinfo_ctx->ref_count--;
        if (0 == dev_shinfo_ctx->ref_count)
            memset(dev_shinfo_ctx, 0, sizeof(struct dcmode_ext_dev_shared_info));
    }
}

static struct dcmode_ext_dev_info *
dcmode_ext_alloc_dev_ctx(int32_t port_id, int32_t ref_port_id, uint32_t alloc_flags)
{
    int32_t dev_idx;
    struct dcmode_ext_dev_info *dev_ctx = NULL;
    struct dcmode_ext_dev_shared_info *dev_shinfo_ctx = NULL;

    /* Find a free device index */
    for (dev_idx = 0; dev_idx < DCMODE_EXT_MAX_DEV_PORT_NUM; dev_idx++) {
        if (g_dcmode_ext_dev[dev_idx].status != DCMODE_EXT_DEV_STATUS_USED) {
            break;
        }
    }

    if (dev_idx >= DCMODE_EXT_MAX_DEV_PORT_NUM) {
        DC_DP_ERROR("failed to allocate device context "
            "for port:%d!\n", port_id);
        goto out;
    }

    /* Allocate device shared context */
    dev_shinfo_ctx = dcmode_ext_alloc_dev_shared_ctx(port_id, ref_port_id, alloc_flags);
    if (!dev_shinfo_ctx)
        goto out;

    dev_ctx = &g_dcmode_ext_dev[dev_idx];

    /* Reset DC ModeX device structure */
    memset(dev_ctx, 0, sizeof(struct dcmode_ext_dev_info));
    dev_ctx->status = DCMODE_EXT_DEV_STATUS_USED;
    dev_ctx->port_id = port_id;
    dev_ctx->alloc_flags = alloc_flags;
    dev_ctx->shared_info = dev_shinfo_ctx;

    g_dcmode_ext_dev_p[port_id] = dev_ctx;

out:
    return dev_ctx;
}

static inline void
dcmode_ext_free_dev_ctx(struct dcmode_ext_dev_info *dev_ctx)
{
    if (dev_ctx) {
        /* Free device shared context */
        dcmode_ext_free_dev_shared_ctx(dev_ctx->shared_info);

        g_dcmode_ext_dev_p[dev_ctx->port_id] = NULL;
        memset(dev_ctx, 0, sizeof(struct dcmode_ext_dev_info));
    }
}

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
static int32_t
dcmode_ext_prepare_channel(dcmode_chan_t *ch, dcmode_chan_id_t ch_id, struct dc_dp_res *res)
{
    ch->id = ch_id;

    if (DCMODE_CHAN_DEV2SOC == ch_id) {
        /* Dev2SoC ring */
        ch->ring.phys = res->rings.dev2soc.phys_base;
        ch->ring.virt = res->rings.dev2soc.base;
        ch->ring.size = res->rings.dev2soc.size;
        ch->ring.desc_dwsz = res->rings.dev2soc.desc_dwsz;
        ch->ring.trck.rp = 0;
        ch->ring.trck.wp = 0;
        ch->ring.trck.size = res->rings.dev2soc.size;

        res->rings.dev2soc.ring = (void *)ch;

        /* No counters required */

    } else if (DCMODE_CHAN_DEV2SOC_EXCEPT == ch_id) {
        /* Dev2SoC_Except ring */
        res->rings.dev2soc_except.desc_dwsz = 1;

        ch->ring.phys = res->rings.dev2soc_except.phys_base;
        ch->ring.virt = res->rings.dev2soc_except.base;
        ch->ring.size = res->rings.dev2soc_except.size;
        ch->ring.desc_dwsz = res->rings.dev2soc_except.desc_dwsz;
        ch->ring.trck.rp = 0;
        ch->ring.trck.wp = 0;
        ch->ring.trck.size = res->rings.dev2soc_except.size;
        ch->ring.trck.quota = 32;

        /* Initialize ring */
        memset(ch->ring.virt, 0, (res->rings.dev2soc_except.size * sizeof(dev2soc_frag_except_bd_t)));
        res->rings.dev2soc_except.ring = (void *)ch;

        /* Dev2SoC_Except counters */
        if (res->dccntr[0].dev2soc_except_deq_base) {
            ch->write_cntr.phys = res->dccntr[0].dev2soc_except_deq_phys_base;
            ch->write_cntr.virt = res->dccntr[0].dev2soc_except_deq_base;
            ch->write_cntr.size = res->dccntr[0].dev2soc_except_deq_dccntr_len;
            if (res->dccntr[0].soc_write_dccntr_mode & DC_DP_F_DCCNTR_MODE_BIG_ENDIAN)
                ch->write_cntr.rw_endian = DC_DP_F_DCCNTR_MODE_BIG_ENDIAN;
            else
                ch->write_cntr.rw_endian = DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN;

            if (res->dccntr[0].soc_write_dccntr_mode & DC_DP_F_DCCNTR_MODE_INCREMENTAL)
                ch->write_cntr.rw_mode = DC_DP_F_DCCNTR_MODE_INCREMENTAL;
            else
                ch->write_cntr.rw_mode = DC_DP_F_DCCNTR_MODE_CUMULATIVE;
        }

        if (res->dccntr[0].dev2soc_except_enq_base) {
            ch->read_cntr.phys = res->dccntr[0].dev2soc_except_enq_phys_base;
            ch->read_cntr.virt = res->dccntr[0].dev2soc_except_enq_base;
            ch->read_cntr.size = res->dccntr[0].dev2soc_except_enq_dccntr_len;
            ch->read_cntr.rw_endian = ch->write_cntr.rw_endian;
            ch->read_cntr.rw_mode = ch->write_cntr.rw_mode;

            /* Initialize counters */
            if (0 != dcmode_read_cntr(ch))
                dcmode_write_cntr(ch, dcmode_read_cntr(ch));
        }
    }

    return 0;
}

static void
dcmode_ext_cleanup_channel(dcmode_chan_t *ch, dcmode_chan_id_t ch_id, struct dc_dp_res *res)
{
    memset(ch, 0, sizeof(dcmode_chan_t));
}
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

static inline int32_t
dcmode_ext_setup_ring_resources(struct dcmode_ext_dev_info *dev_ctx, int32_t port_id,
                                struct dp_dev_data *dp_device, struct dc_dp_res *res, uint32_t flags)
{
    int32_t ret = DC_DP_SUCCESS;
#if defined(CONFIG_X86_INTEL_LGM)
    struct pp_bmgr_pool_params pool_prms;
#else
    int32_t buflist_idx;
    uint32_t **v_buflist_base = NULL;
    uint32_t **p_buflist_base = NULL;
#endif /* CONFIG_X86_INTEL_LGM */

    /* rx_out ring */
    res->rings.dev2soc.phys_base = dp_device->rx_ring[0].out_enq_paddr;
    res->rings.dev2soc.base = dp_device->rx_ring[0].out_enq_vaddr;
    res->rings.dev2soc.size = dp_device->rx_ring[0].out_enq_ring_size;
    res->rings.dev2soc.desc_dwsz = 4;
#if defined(CONFIG_X86_INTEL_LGM)
    res->rings.dev2soc.policy_base = dp_device->rx_ring[0].rx_policy_base;
    res->rings.dev2soc.pool_id = dp_device->rx_ring[0].rx_poolid;
    memset(&pool_prms, 0, sizeof(struct pp_bmgr_pool_params));
    if (pp_bmgr_pool_conf_get(dp_device->rx_ring[0].rx_poolid, &pool_prms)) {
        DC_DP_ERROR("pp_bmgr_pool_conf_get() failed for rx_poolid:%d!\n",
            dp_device->rx_ring[0].rx_poolid);
        return DC_DP_FAILURE;
    }
    res->rings.dev2soc.high_4bits = pool_prms.base_addr_high;
    res->rings.rxout_temp_dw3 = 0;
    /* Pool_Policy[23:16] */
    res->rings.rxout_temp_dw3 |= ((res->rings.dev2soc.policy_base & 0xFF) << 16);
    /* Source_Pool[27:24] */
    res->rings.rxout_temp_dw3 |= (res->rings.dev2soc.pool_id << 24);
    res->rings.rxout_temp_dw1 = 0;
    /* Buffer Pointer[3:0] */
    res->rings.rxout_temp_dw1 |= (res->rings.dev2soc.high_4bits & 0xF);
#endif /* CONFIG_X86_INTEL_LGM */

    /* rx_in ring is being used? */
    if (dp_device->rx_ring[0].in_alloc_paddr) {
        res->rings.dev2soc_ret.phys_base = dp_device->rx_ring[0].in_alloc_paddr;
        /* res->rings.dev2soc_ret.base */
        res->rings.dev2soc_ret.size = dp_device->rx_ring[0].in_alloc_ring_size;
#if defined(CONFIG_X86_INTEL_LGM)
        res->rings.dev2soc_ret.desc_dwsz = 2;
#else
        res->rings.dev2soc_ret.desc_dwsz = 4;
#endif /* CONFIG_X86_INTEL_LGM */

    /* rx_in ring same as rx_out ring */
    } else
        res->rings.dev2soc_ret = res->rings.dev2soc;

    dev_ctx->shared_info->cqm_pid = dp_device->rx_ring[0].out_enq_port_id;
    dev_ctx->shared_info->dma_ch = dp_device->rx_ring[0].out_dma_ch_to_gswip;

    /* tx_in ring */
    res->rings.soc2dev.phys_base = dp_device->tx_ring[0].in_deq_paddr;
    /* res->rings.soc2dev.base */
    res->rings.soc2dev.size = dp_device->tx_ring[0].in_deq_ring_size;
    res->rings.soc2dev.desc_dwsz = 4;

    /* tx_out ring */
    res->rings.soc2dev_ret.phys_base = dp_device->tx_ring[0].out_free_paddr;
    /* res->rings.soc2dev_ret.base */
    res->rings.soc2dev_ret.size = dp_device->tx_ring[0].out_free_ring_size;
    res->rings.soc2dev_ret.desc_dwsz = 2;
    res->rings.soc2dev_ret.policy_base = dp_device->tx_ring[0].txout_policy_base;
    dev_ctx->shared_info->tx_policy_base = dp_device->tx_ring[0].txout_policy_base;
    dev_ctx->shared_info->tx_policy_num = dp_device->tx_ring[0].policy_num;
    res->rings.soc2dev_ret.pool_id = dp_device->tx_ring[0].tx_poolid;
#if defined(CONFIG_X86_INTEL_LGM)
    /* NOTE: base_addr_high is same for both Tx and Rx poolid */
    res->rings.soc2dev_ret.high_4bits = pool_prms.base_addr_high;
    res->rings.txout_temp_dw3 = 0;
    /* Buffer Pointer[26:23] */
    res->rings.txout_temp_dw3 |= ((res->rings.soc2dev_ret.high_4bits & 0xF) << 23);
    if ((dp_device->tx_ring[0].policy_num > 1) &&
        !dp_device->tx_ring[0].f_out_auto_free) {
        DC_DP_ERROR("f_out_auto_free:%d MUST be set for policy_num:%d!\n",
            dp_device->tx_ring[0].f_out_auto_free,
            dp_device->tx_ring[0].policy_num);
        return DC_DP_FAILURE;
    } else if (dp_device->tx_ring[0].policy_num == 1) {
        /* Pool_Policy[21:14] */
        res->rings.txout_temp_dw3 |= ((res->rings.soc2dev_ret.policy_base & 0xFF) << 14);
        /* Source_Pool[3:0] */
        res->rings.txout_temp_dw3 |= (res->rings.soc2dev_ret.pool_id & 0xF);
    }
    dev_ctx->shared_info->high_4bits = res->rings.soc2dev_ret.high_4bits;
#else
    res->rings.txout_temp_dw3 = 0;
    /* Policy[22:20] */
    res->rings.txout_temp_dw3 |= ((res->rings.soc2dev_ret.policy_base & 0x7) << 20);
    /* Pool[18:16] */
    res->rings.txout_temp_dw3 |= ((res->rings.soc2dev_ret.pool_id & 0x7) << 16);
#endif /* CONFIG_X86_INTEL_LGM */

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    /* NOTE: dev2soc exception ring provided by peripheral driver */
    if (res->rings.dev2soc_except.base && (res->rings.dev2soc_except.size > 0))
        res->rings.dev2soc_except.desc_dwsz = 1;
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

#if defined(CONFIG_X86_INTEL_LGM)
    /* NOTE : In LGM, no buflist resource update required */
    res->num_bufpools = 0;
    dev_ctx->shared_info->num_bufpools = res->num_bufpools;
#else
    /* Allocate Rx buffers */
    res->num_bufpools = dp_device->rx_ring[0].num_pkt;
    dev_ctx->shared_info->num_bufpools = res->num_bufpools;

    res->buflist = kmalloc((res->num_bufpools * sizeof(struct dc_dp_buf_pool)), GFP_KERNEL);
    if (!res->buflist) {
        DC_DP_ERROR("failed to allocate buflist of size %d!\n", res->num_bufpools);
        return DC_DP_FAILURE;
    }

    v_buflist_base = (uint32_t **)dp_device->rx_ring[0].pkt_list_vaddr;
    p_buflist_base = (uint32_t **)dp_device->rx_ring[0].pkt_base_vaddr;
    for (buflist_idx = 0; buflist_idx < res->num_bufpools; buflist_idx++) {
        res->buflist[buflist_idx].pool = (void *)v_buflist_base[buflist_idx];
        res->buflist[buflist_idx].phys_pool = (void *)p_buflist_base[buflist_idx];
        res->buflist[buflist_idx].size = dp_device->rx_ring[0].rx_pkt_size;
    }
#endif /* CONFIG_X86_INTEL_LGM */

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    /* Dev2SoC ring and counter */
    dcmode_ext_prepare_channel(CHAN_DEV2SOC_P(dev_ctx), DCMODE_CHAN_DEV2SOC, res);
    /* Dev2SoC Exception ring and counter */
    dcmode_ext_prepare_channel(CHAN_DEV2SOC_EX_P(dev_ctx), DCMODE_CHAN_DEV2SOC_EXCEPT, res);
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

    return ret;
}

static inline void
dcmode_ext_cleanup_ring_resources(struct dcmode_ext_dev_info *dev_ctx, int32_t port_id, struct dc_dp_res *res, uint32_t flags)
{
#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    /* Dev2SoC ring and counter */
    dcmode_ext_cleanup_channel(CHAN_DEV2SOC_P(dev_ctx), DCMODE_CHAN_DEV2SOC, res);
    /* Dev2SoC Exception ring and counter */
    dcmode_ext_cleanup_channel(CHAN_DEV2SOC_EX_P(dev_ctx), DCMODE_CHAN_DEV2SOC_EXCEPT, res);
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */
}

#if 0
#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
static void
_dc_dp_dump_rx_pmac(struct pmac_rx_hdr *pmac)
{
    int i;
    unsigned char *p = (char *)pmac;

    if (!pmac) {
        pr_err("dump_rx_pmac pmac NULL ??\n");
        return ;
    }

    pr_info("PMAC at 0x%p: ", p);
    for (i = 0; i < 8; i++)
        pr_info("0x%02x ", p[i]);
    pr_info("\n");

    /*byte 0 */
    pr_info("  byte 0:res=%d ver_done=%d ip_offset=%d\n", pmac->res1,
           pmac->ver_done, pmac->ip_offset);
    /*byte 1 */
    pr_info("  byte 1:tcp_h_offset=%d tcp_type=%d\n", pmac->tcp_h_offset,
           pmac->tcp_type);
    /*byte 2 */
    pr_info("  byte 2:ppid=%d class=%d\n", pmac->sppid, pmac->class);
    /*byte 3 */
    pr_info("  byte 3:res=%d pkt_type=%d\n", pmac->res2, pmac->pkt_type);
    /*byte 4 */
    pr_info("  byte 4:res=%d redirect=%d res2=%d src_sub_inf_id=%d\n",
           pmac->res3, pmac->redirect, pmac->res4, pmac->src_sub_inf_id);
    /*byte 5 */
    pr_info("  byte 5:src_sub_inf_id2=%d\n", pmac->src_sub_inf_id2);
    /*byte 6 */
    pr_info("  byte 6:port_map=%d\n", pmac->port_map);
    /*byte 7 */
    pr_info("  byte 7:port_map2=%d\n", pmac->port_map2);
}
#endif /* CONFIG_DIRECTCONNECT_DP_DBG */
#endif /* 0 */

#define DC_DP_DEV_CLASS_MASK    0x7
static inline uint8_t
_dc_dp_get_class2devqos(uint8_t class2prio[], uint8_t prio2devqos[], uint8_t class)
{
    uint8_t devqos;
    uint8_t prio;

    class = (class & 0x0F);
    prio = class2prio[class];

    prio = (prio & DC_DP_DEV_CLASS_MASK);
    devqos = prio2devqos[prio];

    return devqos;
}

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
static inline int32_t
dcmode_set_ring_start_index(dcmode_chan_t *ch, uint32_t start_idx)
{
    if (start_idx >= ch->ring.trck.size) {
        DC_DP_ERROR("start_idx=%#x >= ch->ring.trck.size=%#x!!!\n", start_idx, ch->ring.trck.size);
        return -1;
    }

    ch->ring.trck.rp = start_idx;
    ch->ring.trck.wp = start_idx;

    return 0;
}

static inline void *
dcmode_get_bdp_from_ring(dcmode_chan_t *ch)
{
    return ((void *)((uint32_t *)ch->ring.virt + (ch->ring.trck.rp * ch->ring.desc_dwsz)));
}

static inline void dcmode_ring_inc_read_index(dcmode_chan_t *ch, uint32_t idx)
{
    ch->ring.trck.rp =
        (ch->ring.trck.rp + idx) % ch->ring.trck.size;
}

static inline void dcmode_ring_inc_write_index(dcmode_chan_t *ch, uint32_t idx)
{
    ch->ring.trck.wp =
        (ch->ring.trck.wp + idx) % ch->ring.trck.size;
}

static int32_t
handle_dev2soc_frag_exception(dcmode_chan_t *ch, uint32_t start_idx)
{
     int32_t ret;
     struct dma_rx_desc *desc_bd_p;
     struct dma_rx_desc *first_desc_bd_p = NULL;
     int32_t desc_byte_offset;
     int32_t desc_data_len;
     int32_t desc_aligned_data_len = 0;
     int32_t desc_unaligned_data_len = 0;
     uint32_t desc_unaligned_data_vpos = 0;
     int32_t desc_total_data_len = 0;
     int32_t desc_total_len = 0;
     int32_t frag_ready = 1;
     uint32_t src_data_pointer = 0;
     uint32_t data_vpointer = 0;
     uint32_t dest_data_pointer = 0;
     uint32_t dest_data_end_pointer = 0;
     uint32_t total_packet_len = 0;
     uint32_t padlen = 0;

     ret = dcmode_set_ring_start_index(ch, start_idx);
     if (ret < 0)
         return ret;

     /* LOOP on frag_ready */
     do {
         desc_bd_p = (struct dma_rx_desc *)dcmode_get_bdp_from_ring(ch);

         if (!FRAG_READY(desc_bd_p)) {
             /* Loop some time in case HD is not written yet */
             int iter = FRAG_READY_ITER_CNTR;
             do {
                 if (FRAG_READY(desc_bd_p))
                     break;
             } while (--iter);

             if (iter == 0) {
                 DC_DP_ERROR("FRAG_READY bit is not set! 0x%pK:"
                     "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x\n",
                     desc_bd_p, desc_bd_p->dw0.all, desc_bd_p->dw1.all,
                     desc_bd_p->data_pointer, desc_bd_p->status.all);
                 return -1;
             }
         }

         desc_byte_offset = BYTE_OFFSET(desc_bd_p);
         desc_data_len = DATA_LEN(desc_bd_p);
         desc_total_len = (desc_byte_offset + desc_data_len);

         /* Handling for intermediate fragment of a packet */
         if ((0 == SOP(desc_bd_p)) && (0 == EOP(desc_bd_p))) {
             /* Byte_Offset MUST be ZERO and Data_length MUST be multiple of 8 bytes */

//             DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);

             if (desc_byte_offset || desc_unaligned_data_len ||
                          (desc_data_len & DMA1TX_SIZE_ALIGNMENT_MASK)) {
                 data_vpointer = (uint32_t)phys_to_virt(DATA_POINTER(desc_bd_p));
                 (void)dma_map_single(NULL, (void *)data_vpointer,
                            (desc_data_len + desc_unaligned_data_len), DMA_FROM_DEVICE);

                 dest_data_pointer = (data_vpointer + desc_unaligned_data_len);
                 src_data_pointer = (data_vpointer + desc_byte_offset);
                 if (dest_data_pointer != src_data_pointer)
                     memmove((void *)dest_data_pointer, (void *)src_data_pointer, DATA_LEN(desc_bd_p));

                 if (desc_unaligned_data_len)
                     memcpy((void *)(data_vpointer), (void *)(desc_unaligned_data_vpos), desc_unaligned_data_len);

                 desc_total_data_len = (desc_data_len + desc_unaligned_data_len);
                 desc_unaligned_data_len = (desc_total_data_len & DMA1TX_SIZE_ALIGNMENT_MASK);
                 desc_aligned_data_len = (desc_total_data_len - desc_unaligned_data_len);
                 desc_unaligned_data_vpos = (data_vpointer + desc_aligned_data_len);
                 if (desc_aligned_data_len)
                     (void)dma_map_single(NULL, (void *)data_vpointer, desc_aligned_data_len, DMA_TO_DEVICE);
                 DATA_LEN(desc_bd_p) = desc_aligned_data_len;
                 BYTE_OFFSET(desc_bd_p) = 0;
             }

             FRAG_READY(desc_bd_p) = 0;
             OWN(desc_bd_p) = 1;

//             DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);

         /* Handling for 1st fragment of a packet */
         } else if ((1 == SOP(desc_bd_p)) && (0 == EOP(desc_bd_p))) {
             /* (Byte_Offset+Data_length) MUST be multiple of 8 bytes */

//             DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);

             first_desc_bd_p = desc_bd_p; // Save first desc of the packet, to set OWN bit as the last action
             desc_unaligned_data_len = (desc_total_len & DMA1TX_SIZE_ALIGNMENT_MASK);
             if (desc_unaligned_data_len) {
                 desc_aligned_data_len = (desc_data_len - desc_unaligned_data_len);
                 data_vpointer = (uint32_t)phys_to_virt(DATA_POINTER(desc_bd_p));
                 desc_unaligned_data_vpos = (data_vpointer + desc_byte_offset + desc_aligned_data_len);
                 (void)dma_map_single(NULL, (void *)desc_unaligned_data_vpos, desc_unaligned_data_len, DMA_FROM_DEVICE);
                 DATA_LEN(desc_bd_p) = desc_aligned_data_len;
             }

             FRAG_READY(desc_bd_p) = 0;

//             DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);

         /* Handling for last fragment of a packet */
         } else if ((0 == SOP(desc_bd_p)) && (1 == EOP(desc_bd_p))) {
             /* Byte_Offset MUST be ZERO */

//             DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);
             if ((desc_byte_offset || desc_unaligned_data_len) ||
                     (total_packet_len < DCDP_SHORT_PKT_LEN)) {
                 desc_total_data_len = (desc_data_len + desc_unaligned_data_len);
                 data_vpointer = (uint32_t)phys_to_virt(DATA_POINTER(desc_bd_p));
                 (void)dma_map_single(NULL, (void *)data_vpointer, (desc_data_len + desc_unaligned_data_len), DMA_FROM_DEVICE);

                 if (desc_byte_offset || desc_unaligned_data_len) {
                     dest_data_pointer = (data_vpointer + desc_unaligned_data_len);
                     src_data_pointer = (data_vpointer + desc_byte_offset);
                     if (dest_data_pointer != src_data_pointer)
                         memmove((void *)dest_data_pointer, (void *)src_data_pointer, DATA_LEN(desc_bd_p));
                     memcpy((void *)(data_vpointer), (void *)(desc_unaligned_data_vpos), desc_unaligned_data_len);
                     BYTE_OFFSET(desc_bd_p) = 0;
                 }

                 if (total_packet_len < DCDP_SHORT_PKT_LEN) {
                     dest_data_end_pointer = (data_vpointer + desc_total_data_len);
                     padlen = DCDP_SHORT_PKT_LEN - total_packet_len;
                     memset((void *)dest_data_end_pointer, 0, padlen);
                     desc_total_data_len += padlen;
                 }
                 (void)dma_map_single(NULL, (void *)data_vpointer, desc_total_data_len, DMA_TO_DEVICE);
                 DATA_LEN(desc_bd_p) = desc_total_data_len;
             }

             FRAG_READY(desc_bd_p) = 0;
             OWN(desc_bd_p) = 1;
//           DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_RX_DESCRIPTOR, "DW0=0x%08x DW1=0x%08x DW2=0x%08x DW3=0x%08x.\n",
//                         desc_bd_p->dw0.all, desc_bd_p->dw1.all, desc_bd_p->data_pointer, desc_bd_p->status.all);

             wmb();

             /* Set the OWN bit of 1st fragment */
             if (first_desc_bd_p)
                 OWN(first_desc_bd_p) = 1;

             frag_ready = 0;
         } else
               DC_DP_ERROR("Received fragment with SOP (%d) and EOP (%d) set", SOP(desc_bd_p),
			       EOP(desc_bd_p));

         dcmode_ring_inc_read_index(ch, 1);
     } while (1 == frag_ready);

     return 0;
}
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

static inline void
get_soc_capability(uint32_t *cap)
{
    /* Linux offload capability */
    *cap = DC_DP_F_HOST_CAP_SG;
    if (g_dp_cap.tx_hw_chksum)
        *cap |= DC_DP_F_HOST_CAP_HW_CSUM;
    if (g_dp_cap.rx_hw_chksum)
        *cap |= DC_DP_F_HOST_CAP_RXCSUM;
    if (g_dp_cap.hw_tso) {
        *cap |= DC_DP_F_HOST_CAP_TSO;
        *cap |= DC_DP_F_HOST_CAP_TSO6;
    }

#if !defined(CONFIG_X86_INTEL_LGM)
#ifdef CONFIG_PPA_LRO
    *cap |= DC_DP_F_HOST_CAP_LRO;
#endif /* CONFIG_PPA_LRO */

    /* FCS capability */
    *cap |= DC_DP_F_HOST_CAP_TX_FCS;
    *cap |= DC_DP_F_HOST_CAP_RX_FCS;
    *cap |= DC_DP_F_HOST_CAP_TX_WO_FCS;
    *cap |= DC_DP_F_HOST_CAP_RX_WO_FCS;

    /* PMAC capability */
    *cap |= DC_DP_F_HOST_CAP_TX_PMAC;
    *cap |= DC_DP_F_HOST_CAP_RX_PMAC;
    *cap |= DC_DP_F_HOST_CAP_RX_WO_PMAC;
#endif /* CONFIG_X86_INTEL_LGM */

    /* QoS */
    *cap |= DC_DP_F_HOST_CAP_HW_QOS | DC_DP_F_HOST_CAP_HW_QOS_WAN;
    *cap |= DC_DP_F_HOST_CAP_DEVQOS;

#if defined(CONFIG_X86_INTEL_LGM)
    *cap |= DC_DP_F_HOST_CAP_AUTO_DETECT_BUFFER_RETURN;
    *cap |= DC_DP_F_HOST_CAP_BUFFER_MARKING;
#endif /* CONFIG_X86_INTEL_LGM */
#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    /* Rx fragmentation workaround */
    *cap |= DC_DP_F_HOST_CAP_RX_FRAG_HANDLING_RESTRICTED;
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */
}

static void
dump_dc_resources(int32_t port_id, struct dc_dp_res *resources)
{
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "DC Resources for port:%d\n", port_id);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  TXIN|base:0x%pK phys_base:0x%pK "
        "size:%d desc_dwsz:%d\n",
        resources->rings.soc2dev.base,
        resources->rings.soc2dev.phys_base,
        resources->rings.soc2dev.size,
        resources->rings.soc2dev.desc_dwsz);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  TXOUT|base:0x%pK phys_base:0x%pK "
        "size:%d desc_dwsz:%d\n",
        resources->rings.soc2dev_ret.base,
        resources->rings.soc2dev_ret.phys_base,
        resources->rings.soc2dev_ret.size,
        resources->rings.soc2dev_ret.desc_dwsz);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  RXOUT|base:0x%pK phys_base:0x%pK "
        "size:%d desc_dwsz:%d\n",
        resources->rings.dev2soc.base,
        resources->rings.dev2soc.phys_base,
        resources->rings.dev2soc.size,
        resources->rings.dev2soc.desc_dwsz);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  RXIN|base:0x%pK phys_base:0x%pK "
        "size:%d desc_dwsz:%d\n",
        resources->rings.dev2soc_ret.base,
        resources->rings.dev2soc_ret.phys_base,
        resources->rings.dev2soc_ret.size,
        resources->rings.dev2soc_ret.desc_dwsz);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  TX|num_bufs:%d/%d policy_base:%d "
        "pool_id:%d high_4bits:%d\n",
        resources->tx_num_bufpools, resources->tx_num_bufs_req,
        resources->rings.soc2dev_ret.policy_base,
        resources->rings.soc2dev_ret.pool_id,
        resources->rings.soc2dev_ret.high_4bits);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  RX|num_bufs:%d/%d, policy_base:%d "
        "pool_id:%d high_4bits:%d\n",
        resources->num_bufpools, resources->num_bufs_req,
        resources->rings.dev2soc.policy_base,
        resources->rings.dev2soc.pool_id,
        resources->rings.dev2soc.high_4bits);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "  DESC TEMPLATE|txout_dw3:0x%x "
        "rxout_dw1:0x%x rxout_dw3:0x%x\n",
        resources->rings.txout_temp_dw3,
        resources->rings.rxout_temp_dw1,
        resources->rings.rxout_temp_dw3);
}

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#include <linux/of_address.h>
#include <linux/of_platform.h>
static void *
find_pci_sysdata(const u8 *compatible, dma_addr_t umt_ctl_daddr)
{
    struct device_node *np = NULL;
    struct of_pci_range_parser parser;
    struct of_pci_range range;
    struct platform_device *pdev;
    bool found;
    void *sysdata;
    //struct device *dev;
    //struct pci_bus *bus;

    do {
        found = false;
        np = of_find_compatible_node(np, NULL, compatible);
        if (!np || !of_device_is_available(np)) {
            pr_debug("failed to get device_node:0x%pK of compatible:\"%s\"\n",
                np, compatible);
            continue;
        }

        if (of_pci_range_parser_init(&parser, np)) {
            pr_warn("missing \"ranges\" property on device_node:%s!\n",
                (np->name ? np->name : "NA"));
            continue;
        }

        pr_debug("parsing \"ranges\" property on device_node:%s\n",
            (np->name ? np->name : "NA"));
        for_each_of_pci_range(&parser, &range) {
            /* Read next ranges element */
            pr_debug("pci_space: 0x%08x pci_addr:0x%016llx ",
                range.pci_space, range.pci_addr);
            pr_debug("cpu_addr:0x%016llx size:0x%016llx\n",
                range.cpu_addr, range.size);

            /* compare the range to match against UMT address */
            if ((umt_ctl_daddr >= range.pci_addr) &&
                (umt_ctl_daddr < (range.pci_addr + range.size))) {
                found = true;
                break;
            }
        }
    } while (np && !found);

    if (!np)
        return NULL;

    /* Get platform_dev and dev */
    pdev = of_find_device_by_node(np);
    if (!pdev)
        return NULL;
    //dev = &pdev->dev;

    /* Get pci-dev/pci-bus from platform dev */

    /* workaround */
    sysdata = platform_get_drvdata(pdev);
    pr_info("pci sysdata:0x%pK derived from umt_ctl_daddr:%llx\n",
        sysdata, umt_ctl_daddr);

    return sysdata;
}

static int32_t
dcmode_ext_get_subif_by_dev(struct net_device *dev, struct dp_subif *subif)
{
    int32_t ret = DC_DP_FAILURE;
    int32_t dev_idx;
    int32_t subif_idx;

    /*DC_DP_LOCK(&g_dcmode_ext_dev_lock);*/
    for (dev_idx = 0; dev_idx < DCMODE_EXT_MAX_DEV_NUM; dev_idx++) {
        if (g_dcmode_ext_dev_shinfo[dev_idx].status == DCMODE_DEV_STATUS_FREE)
            continue;

        if (dev) {
            for (subif_idx = 0; subif_idx < DCMODE_EXT_MAX_SUBIF_PER_DEV; subif_idx++) {
                if (g_dcmode_ext_dev_shinfo[dev_idx].subif_info[subif_idx].netif == dev) {
                    subif->port_id = g_dcmode_ext_dev_shinfo[dev_idx].port_id;
                    subif->subif = (subif_idx << DCMODE_EXT_SUBIFID_OFFSET);

                    ret = DC_DP_SUCCESS;
                    goto out;
                }
            }
        } else {
            /* NOTE: DSL null device like PPPoA, IPoA etc. */
            if (g_dcmode_ext_dev_shinfo[dev_idx].dev_alloc_flags & DC_DP_F_FAST_DSL) {
                subif->port_id = g_dcmode_ext_dev_shinfo[dev_idx].port_id;

                ret = DC_DP_SUCCESS;
                break;
            }
        }
    }

out:
    /*DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);*/
    return ret;
}

static u8
find_chip_ver(void)
{
    const u8 *compatible = "intel,chipid";
    struct device_node *np;
    struct platform_device *pdev;
    struct resource *res;
    void __iomem *membase;
    u32 id;
    unsigned int ver = 0;

    np = of_find_compatible_node(NULL, NULL, compatible);
    if (!np || !of_device_is_available(np)) {
        pr_err("[%s] failed to get node:0x%pK of compatible:\"%s\"\n",
            __func__, np, compatible);
        goto out;
    }

    /* Get platform_dev and dev */
    pdev = of_find_device_by_node(np);
    if (!pdev) {
        pr_err("[%s] failed to find platform device by node:0x%pK\n",
            __func__, np);
        goto out;
    }

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        pr_err("[%s] failed to get mem resource!\n", __func__);
        goto out;
    }

    membase = ioremap(res->start, resource_size(res));
    if (IS_ERR(membase)) {
        pr_err("[%s] failed to ioremap mem resource!\n", __func__);
        goto out;
    }

    id = readl(membase + MPS_CHIPID);
    ver = (id & GENMASK(31, 28)) >> 28;
    iounmap(membase);
    pr_debug("[%s] LGM SoC Version:%d\n", __func__, ver);

out:
    return ver;
}
#endif /* CONFIG_X86_INTEL_LGM */

/*
 * ========================================================================
 * DirectConnect Driver Interface API (SoC specific)
 * ========================================================================
 */
static int32_t
dcmode_ext_get_peripheral_config(int32_t port_id,
                                 enum dc_dp_dev_type dev_id,
                                 struct dc_dp_dev_config *dev_config)
{
    int32_t ret = DC_DP_SUCCESS;
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
    struct dp_dc_res res = {0};

    /* Minimum guaranteed buffers, if any */
    res.dp_port = port_id;
    if (dev_id == DC_DP_DEV_WAV_600_24G)
        res.res_id = DP_RES_ID_WAV614;
    else if ((dev_id == DC_DP_DEV_WAV_600_5G) ||
             (dev_id == DC_DP_DEV_WAV_600_6G))
        res.res_id = DP_RES_ID_WAV624;
    else if (dev_id == DC_DP_DEV_WAV_600_CDB)
        res.res_id = DP_RES_ID_WAV654;

    ret = dp_get_dc_config(&res, 0);
    if (ret == DP_SUCCESS) {
        dev_config->rx_bufs = res.rx_res.rx_bufs;
        dev_config->tx_bufs = res.tx_res.tx_bufs;
    }
#endif /* CONFIG_X86_INTEL_LGM */

    g_dev_type = dev_id;
    return ret;
}

static int32_t
dcmode_ext_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;

    if (cap) {
        cap->fastpath.support = 1;
#if defined(CONFIG_X86_INTEL_LGM)
        cap->fastpath.hw_dcmode = DC_DP_MODE_TYPE_1_EXT;
#else
        cap->fastpath.hw_dcmode = DC_DP_MODE_TYPE_0_EXT;
#endif /* CONFIG_X86_INTEL_LGM */

        if (g_dp_cap.umt.umt_hw_auto.enable == 1) {
            if (g_dp_cap.umt.umt_hw_auto.rx_accumulate == 1)
                cap->fastpath.hw_cmode.dev2soc_write = DC_DP_F_DCCNTR_MODE_CUMULATIVE;
            if (g_dp_cap.umt.umt_hw_auto.rx_incremental == 1)
                cap->fastpath.hw_cmode.dev2soc_write |= DC_DP_F_DCCNTR_MODE_INCREMENTAL;
            if (g_dp_cap.umt.umt_hw_auto.tx_accumulate == 1)
                cap->fastpath.hw_cmode.soc2dev_write |= DC_DP_F_DCCNTR_MODE_CUMULATIVE;
            if (g_dp_cap.umt.umt_hw_auto.tx_incremental == 1)
                cap->fastpath.hw_cmode.soc2dev_write |= DC_DP_F_DCCNTR_MODE_INCREMENTAL;
        }

        cap->fastpath.hw_cmode.soc2dev_write |= DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN;
        cap->fastpath.hw_cmode.dev2soc_write |= DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN;

        get_soc_capability(&cap->fastpath.hw_cap);
        ret = DC_DP_SUCCESS;
    }

    return ret;
}

static int32_t
dcmode_ext_mark_pkt_devqos(
        uint32_t devqos,
        struct sk_buff *skb)
{
#if defined(CONFIG_X86_INTEL_LGM)
    uint32_t dev_qos;
    struct dma_rx_desc_0 *qos_desc_dw0 = (struct dma_rx_desc_0 *) &skb->DW0;

    /** update class: 31:28
     * Dev_QoS: 27:24 of
     * skb->DW0 bit */
    dev_qos = devqos << 24;
    dev_qos |= skb->priority << 28;

    qos_desc_dw0->all &= 0x00FFFFFF;
    qos_desc_dw0->all |= dev_qos;
#endif /* CONFIG_X86_INTEL_LGM */
    return 0;
}

static int32_t
dcmode_ext_register_dev_ex(void *ctx,
                           struct module *owner, uint32_t port_id,
                           struct net_device *dev, struct dc_dp_cb *datapathcb,
                           struct dc_dp_res *resources, struct dc_dp_dev *devspec,
                           int32_t ref_port_id, uint32_t alloc_flags, uint32_t flags)
{
    int32_t ret;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    dp_cb_t dp_cb = {0};
    struct dp_dev_data dp_device = {0};
    int32_t umt_idx;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "dev_ctx=0x%pK, owner=0x%pK, port_id=%u, "
        "dev=0x%pK, datapathcb=0x%pK, resources=0x%pK, dev_spec=0x%pK, "
        "flags=0x%08x\n", dev_ctx, owner, port_id, dev, datapathcb,
        resources, devspec, flags);

    /* De-register */
    if (flags & DC_DP_F_DEREGISTER) {

        DC_DP_LOCK(&g_dcmode_ext_dev_lock);

        if (!(dev_ctx && dev_ctx->shared_info)) {
            ret = DC_DP_FAILURE;
            goto err_unlock_out;
        }

        /* De-register DC ModeX device from DC Common layer */
        dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, DC_DP_DCMODE_DEV_DEREGISTER);

        /* De-register device from DP Lib */
        /* Skip, iff sub-sequent Multiport device? */
        if (!is_multiport_sub(dev_ctx->alloc_flags))
            dp_register_dev_ext(0, owner, port_id, &dp_cb, &dp_device, DP_F_DEREGISTER);

        /* For the last device */
        if (1 == dev_ctx->shared_info->ref_count)
            dcmode_ext_cleanup_ring_resources(dev_ctx, dev_ctx->shared_info->port_id, resources, flags);

        /* Free DC ModeX device context */
        dcmode_ext_free_dev_ctx(dev_ctx);

        DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d\n", DC_DP_SUCCESS);
        return DC_DP_SUCCESS;
    }

    /* Validate input arguments */
    if ((resources->num_dccntr != 1) || !resources->dccntr) {
        DC_DP_ERROR("failed to register device for port:%d!\n", port_id);
        ret = DC_DP_FAILURE;
        goto err_out;
    }

#if defined(CONFIG_X86_INTEL_LGM)
    if (resources->dccntr->soc_write_dccntr_mode & DC_DP_F_DCCNTR_MODE_BIG_ENDIAN) {
        DC_DP_ERROR("failed to register device for port:%d, "
           "as unsupported dccntr_mode:%d!\n", port_id,
           resources->dccntr->soc_write_dccntr_mode);
        ret = DC_DP_FAILURE;
        goto err_out;
    }
#endif /* CONFIG_X86_INTEL_LGM */

    DC_DP_LOCK(&g_dcmode_ext_dev_lock);

    if (g_dcmode_ext_dev_p[port_id]) {
        ret = DC_DP_FAILURE;
        goto err_unlock_out;
    }

    dev_ctx = dcmode_ext_alloc_dev_ctx(port_id, ref_port_id, alloc_flags);
    if (!dev_ctx) {
        ret = DC_DP_FAILURE;
        goto err_unlock_out;
    }

    /* Skip, iff sub-sequent Multiport device? */
    if (!is_multiport_sub(alloc_flags)) {

        /* Datapath Library callback registration */
        dp_cb.rx_fn = dcmode_ext_rx_cb;
        dp_cb.stop_fn = datapathcb->stop_fn;
        dp_cb.restart_fn = datapathcb->restart_fn;
        if (is_multiport(alloc_flags))
            dp_cb.get_subifid_fn = dcmode_ext_get_netif_subifid_cb;
        else
            dp_cb.get_subifid_fn = dc_dp_get_netif_subifid;
        dp_cb.reset_mib_fn = datapathcb->reset_mib_fn;
        dp_cb.get_mib_fn = datapathcb->get_mib_fn;

        /* Update DP device structure */
        dp_device.num_rx_ring = 1;
        dp_device.num_tx_ring = 1;
        dp_device.num_umt_port = resources->num_dccntr;

        /* Rx ring */
        dp_device.rx_ring[0].out_enq_ring_size = resources->rings.dev2soc.size;
        dp_device.rx_ring[0].num_pkt = resources->num_bufs_req;
        dp_device.rx_ring[0].rx_pkt_size = DC_DP_PKT_BUF_SIZE_DEFAULT;
#if defined(CONFIG_X86_INTEL_LGM)
        dp_device.rx_ring[0].prefill_pkt_num = 0;
#else
        dp_device.rx_ring[0].prefill_pkt_num = dp_device.rx_ring[0].num_pkt;
#endif /* CONFIG_X86_INTEL_LGM */
        dp_device.rx_ring[0].out_msg_mode = UMT_RXIN_MSG_ADD;
        dp_device.rx_ring[0].out_qos_mode = DP_RXOUT_BYPASS_QOS_ONLY;

        /* Tx ring */
        dp_device.tx_ring[0].in_deq_ring_size = resources->rings.soc2dev.size;
        if (alloc_flags & DC_DP_F_FAST_DSL)
            dp_device.tx_ring[0].num_tx_pkt = resources->tx_num_bufs_req;
        dp_device.tx_ring[0].tx_pkt_size = DC_DP_PKT_BUF_SIZE_DEFAULT;

        /* UMT info */
        for (umt_idx = 0; umt_idx < dp_device.num_umt_port; umt_idx++) {
            dp_device.umt[umt_idx].ctl.enable = 1;
            dp_device.umt[umt_idx].ctl.fflag = UMT_NOT_SND_ZERO_CNT;
            if (resources->dccntr[umt_idx].dev2soc_dccntr_timer > 0)
                dp_device.umt[umt_idx].ctl.msg_interval = resources->dccntr[umt_idx].dev2soc_dccntr_timer;
            else
                dp_device.umt[umt_idx].ctl.msg_interval = DCMODE_EXT_DEF_UMT_PERIOD;

            if (resources->dccntr[umt_idx].umt_mode == DC_DP_UMT_MODE_HW_AUTO) {
                dp_device.umt[umt_idx].ctl.daddr = (dma_addr_t)resources->dccntr[umt_idx].dev2soc_ret_enq_phys_base;
                dp_device.umt[umt_idx].ctl.msg_mode = UMT_MSG_SELFCNT;
            } else if (resources->dccntr[umt_idx].umt_mode == DC_DP_UMT_MODE_SW) {
                dp_device.umt[umt_idx].ctl.daddr = (dma_addr_t)resources->dccntr[umt_idx].dev2soc_ret_enq_base;
                dp_device.umt[umt_idx].ctl.msg_mode = UMT_MSG_SELFCNT;
            } else if (resources->dccntr[umt_idx].umt_mode == DC_DP_UMT_MODE_HW_USER) {
                dp_device.umt[umt_idx].ctl.daddr = (dma_addr_t)resources->dccntr[umt_idx].dev2soc_ret_enq_phys_base;
                dp_device.umt[umt_idx].ctl.msg_mode = UMT_MSG_USER_MODE;
            }

            if (resources->dccntr[umt_idx].soc_write_dccntr_mode & DC_DP_F_DCCNTR_MODE_CUMULATIVE)
                dp_device.umt[umt_idx].ctl.cnt_mode = UMT_CNT_ACC;
            else
                dp_device.umt[umt_idx].ctl.cnt_mode = UMT_CNT_INC;
        }

#if defined(CONFIG_X86_INTEL_LGM)
        /* Enable auto detection of policy and buffer pool, for all DC ports */
        dp_device.tx_ring[0].f_out_auto_free = 1;
        devspec->dc_cap |= DC_DP_F_HOST_CAP_AUTO_DETECT_BUFFER_RETURN;

        if (is_required_buffer_marking(devspec, alloc_flags)) {
            dp_device.enable_cqm_meta = 1;
            devspec->dc_cap |= DC_DP_F_HOST_CAP_BUFFER_MARKING;
        }

        dp_device.max_ctp = DCMODE_EXT_ALT_SUBIF_PER_DEV;
        if (alloc_flags & DC_DP_F_FAST_WLAN) {
            if (g_dev_type == DC_DP_DEV_WAV_600_24G) {
                dp_device.bm_policy_res_id = DP_RES_ID_WAV614;
                dp_device.max_ctp = DCMODE_EXT_MAX_SUBIF_PER_DEV;
                dp_device.max_gpid = DCMODE_EXT_MAX_SUBIF_PER_DEV;
            } else if ((g_dev_type == DC_DP_DEV_WAV_600_5G) ||
                       (g_dev_type == DC_DP_DEV_WAV_600_6G)) {
                dp_device.bm_policy_res_id = DP_RES_ID_WAV624;
                dp_device.max_ctp = DCMODE_EXT_MAX_SUBIF_PER_DEV;
                dp_device.max_gpid = DCMODE_EXT_MAX_SUBIF_PER_DEV;
            } else {
                pr_warn("registering an unknown WLAN dev_type:%d!\n", g_dev_type);
            }
        }

        if (resources->dccntr[0].dev2soc_ret_enq_phys_base) {
            dp_device.opt_param.pcidata = find_pci_sysdata("intel,lgm-pcie",
                (dma_addr_t)resources->dccntr[0].dev2soc_ret_enq_phys_base);

            if (!dp_device.opt_param.pcidata) {
                DC_DP_ERROR("failed to find pcidata from umt_ctl_daddr:%llx!\n",
                    (dma_addr_t)resources->dccntr[0].dev2soc_ret_enq_phys_base);
                ret = DC_DP_FAILURE;
                goto err_out_free_dev;
            }
        }
#else
        dp_device.max_ctp = DCMODE_EXT_MAX_SUBIF_PER_DEV;
#endif /* CONFIG_X86_INTEL_LGM */

        ret = dp_register_dev_ext(0, owner, port_id, &dp_cb, &dp_device, 0);
        if (ret != DP_SUCCESS) {
            DC_DP_ERROR("dp_register_dev_ext() failed for port:%d!\n", port_id);
            if (reboot_on_buffer_leak) {
                pr_emerg("possibly, DC Tx/Rx ring buffer leak detected! Rebooting...\n");
                orderly_reboot();
            }
            goto err_out_free_dev;
        }
    }

    /* For the first device */
    if (1 == dev_ctx->shared_info->ref_count) {
        /* Setup ring resources */
        ret = dcmode_ext_setup_ring_resources(dev_ctx,
            dev_ctx->shared_info->port_id, &dp_device, resources, flags);
        if (ret)
            goto err_out_dereg_dev;

        if (devspec->dev_cap_req & DC_DP_F_DEV_REQ_TX_FCS)
            dev_ctx->shared_info->dc_cap |= DC_DP_F_DEV_REQ_TX_FCS;

        dev_ctx->shared_info->dev_alloc_flags = alloc_flags;
    }

    devspec->dc_accel_used = DC_DP_ACCEL_FULL_OFFLOAD;
#if defined(CONFIG_X86_INTEL_LGM)
    devspec->dc_tx_ring_used = DC_DP_RING_HW_MODE1_EXT;
    devspec->dc_rx_ring_used = DC_DP_RING_HW_MODE1_EXT;
#else
    devspec->dc_tx_ring_used = DC_DP_RING_HW_MODE0_EXT;
    devspec->dc_rx_ring_used = DC_DP_RING_HW_MODE0_EXT;
#endif /* CONFIG_X86_INTEL_LGM */
    get_soc_capability(&devspec->dc_cap);

    /* Register DC ModeX device to DC common layer */
    ret = dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, 0);
    if (ret) {
        DC_DP_ERROR("failed to register device to DC common layer!!!\n");
        goto err_out_dereg_dev;
    }

    DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

    if (!is_multiport_sub(alloc_flags))
        dump_dc_resources(port_id, resources);

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d\n", ret);
    return ret;

err_out_dereg_dev:
    /* Skip, iff sub-sequent Multiport device? */
    if (!is_multiport_sub(alloc_flags))
        dp_register_dev_ext(0, owner, port_id, &dp_cb, &dp_device, DP_F_DEREGISTER);

err_out_free_dev:
    dcmode_ext_free_dev_ctx(dev_ctx);

err_unlock_out:
    DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

err_out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned %d!\n", ret);
    return ret;
}

static int32_t
dcmode_ext_register_subif(void *ctx,
                          struct module *owner, struct net_device *dev,
                          const uint8_t *subif_name, struct dp_subif *subif_id, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    int32_t subif_idx;
    int32_t subif_idx_mapped;
    struct dp_subif_data subif_data = {0};

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "dev_ctx=0x%pK, owner=0x%pK, dev=0x%pK, "
        "subif_id=0x%pK, flags=0x%08x\n", dev_ctx, owner, dev, subif_id, flags);

    subif_idx = DCMODE_EXT_GET_SUBIFIDX(subif_id->subif);

    /* De-register */
    if (flags & DC_DP_F_DEREGISTER) {

        DC_DP_LOCK(&g_dcmode_ext_dev_lock);

        /* Multiport device? */
        if (is_multiport(dev_ctx->alloc_flags))
            multiport_wa_forward_map_subifid(dev_ctx, subif_id);
        subif_idx_mapped = DCMODE_EXT_GET_SUBIFIDX(subif_id->subif);

        /* De-register subif from Datapath Library/Core */
        ret = dp_register_subif_ext(0, owner, dev, (uint8_t *)subif_name,
            subif_id, &subif_data, DP_F_DEREGISTER);
        if (ret != DP_SUCCESS) {
            if (is_multiport(dev_ctx->alloc_flags))
                multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

            DC_DP_ERROR("dp_register_subif_ext() failed for "
                "<port:%d, subif:0x%x>!\n", subif_id->port_id, subif_id->subif);
            goto err_unlock_out;
        }

        dev_ctx->num_subif--;
        dev_ctx->shared_info->num_subif--;
        dev_ctx->shared_info->subif_info[subif_idx_mapped].netif = NULL;

        /* Multiport device? */
        if (is_multiport(dev_ctx->alloc_flags))
            multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

        DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned 0\n");
        return DC_DP_SUCCESS;
    }

    DC_DP_LOCK(&g_dcmode_ext_dev_lock);

    /* Multiport device, single port should not have more than 8 Subif */
    if (is_multiport(dev_ctx->alloc_flags)) {
        if (subif_idx >= MULTIPORT_WORKAROUND_MAX_SUBIF_NUM) {
            DC_DP_ERROR("requested subif:0x%x is not in valid "
                "multiport subif range[0-7]!\n", subif_id->subif);
            goto err_unlock_out;
        }

        multiport_wa_forward_map_subifid(dev_ctx, subif_id);
    }
    subif_idx_mapped = DCMODE_EXT_GET_SUBIFIDX(subif_id->subif);

    /* Register subif to Datapath Library/Core */
    ret = dp_register_subif_ext(0, owner, dev, (uint8_t *)subif_name, subif_id, &subif_data, 0);
    if (ret != DP_SUCCESS) {
        if (is_multiport(dev_ctx->alloc_flags))
            multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

        DC_DP_ERROR("dp_register_subif_ext() failed for "
            "<port:%d, subif:0x%x>!\n", subif_id->port_id, subif_id->subif);
        goto err_unlock_out;
    }

    dev_ctx->shared_info->subif_info[subif_idx_mapped].netif = dev;
    dev_ctx->shared_info->num_subif++;
    dev_ctx->num_subif++;

    /* Multiport device? */
    if (is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

    DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d "
        "<port:%d, subif:0x%x>\n", ret, subif_id->port_id, subif_id->subif);
    return ret;

err_unlock_out:
    DC_DP_UNLOCK(&g_dcmode_ext_dev_lock);

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned %d!\n", ret);
    return ret;
}

static int32_t
dcmode_ext_xmit(void *ctx, struct net_device *rx_if,
                struct dp_subif *rx_subif, struct dp_subif *tx_subif,
                struct sk_buff *skb, int32_t len, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    uint32_t dp_flags = 0;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    struct dma_tx_desc_0 *desc_0 = (struct dma_tx_desc_0 *) &skb->DW0;
    struct dma_tx_desc_1 *desc_1 = (struct dma_tx_desc_1 *) &skb->DW1;

    if (!tx_subif) {
        DC_DP_ERROR("tx_subif is NULL!!!\n");
        goto drop;
    }

    /* Multiport device? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_forward_map_subifid(dev_ctx, tx_subif);

    /* skb->DWx */
    desc_1->field.ep = tx_subif->port_id;
    desc_0->field.dest_sub_if_id = tx_subif->subif;

    if (dev_ctx && dev_ctx->shared_info &&
        (dev_ctx->shared_info->dc_cap & DC_DP_F_DEV_REQ_TX_FCS))
        dp_flags |= DP_TX_DSL_FCS;

    if (skb->ip_summed == CHECKSUM_PARTIAL)
        dp_flags |= DP_TX_CAL_CHKSUM;

    /* Send it to Datapath library for transmit */
    ret = dp_xmit(skb->dev, tx_subif, skb, skb->len, dp_flags);

    /* Multiport device? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, tx_subif);

    return ret;

drop:
    if (skb)
        dev_kfree_skb_any(skb);

    return ret;
}

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
static int32_t
dcmode_ext_handle_ring_sw(void *ctx, struct module *owner, uint32_t port_id, struct net_device *dev,
                          struct dc_dp_ring *ring, uint32_t flags)
{
    int32_t ret = 0;
    uint32_t idx;
    uint32_t cntr_val;
    uint32_t dev2soc_ring_idx;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    dev2soc_frag_except_bd_t *frag_ex_bd;
    uint32_t quota = 0;

    if (!dev_ctx) {
        DC_DP_ERROR("dev_ctx is NULL\n");
        ret = -1;
        goto out;
    }

    if (flags & DC_DP_F_RX_FRAG_EXCEPT) {
//        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "ctx=0x%p, owner=%s, port_id=%d, dev=%s, ring=0x%p, flags=0x%08x.\n",
//                    ctx, (owner ? owner->name : "NULL"), port_id, (dev ? dev->name : "NULL"), ring, flags);
        cntr_val = dcmode_read_cntr(CHAN_DEV2SOC_EX_P(dev_ctx));
        if (0 == cntr_val)
            goto out;

//        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Read counter val from dev2soc_frag_except ring = 0x%x.\n", cntr_val);

        quota = MIN(CHAN_DEV2SOC_EX_P(dev_ctx)->ring.trck.quota, cntr_val);
        for (idx = 0; idx < quota; idx++) {
            frag_ex_bd = (dev2soc_frag_except_bd_t *)dcmode_get_bdp_from_ring(CHAN_DEV2SOC_EX_P(dev_ctx));
            dev2soc_ring_idx = frag_ex_bd->dev2soc_ring_idx;
//            DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Reference index of dev2soc ring = 0x%x.\n", dev2soc_ring_idx);
            ret = handle_dev2soc_frag_exception(CHAN_DEV2SOC_P(dev_ctx), dev2soc_ring_idx);
            if (0 != ret) {
                DC_DP_ERROR("Failed to fix DMA for dev2soc ring idx = 0x%x - ret=%d.\n", dev2soc_ring_idx, ret);
                break;
            }
            dcmode_ring_inc_read_index(CHAN_DEV2SOC_EX_P(dev_ctx), 1);
        }
        dcmode_ring_inc_write_index(CHAN_DEV2SOC_EX_P(dev_ctx), idx);
        dcmode_write_cntr(CHAN_DEV2SOC_EX_P(dev_ctx), idx);
    }

out:
    return ret;
}
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

static int32_t
dcmode_ext_return_bufs(void *ctx, uint32_t port_id,
                       void **buflist, uint32_t buflist_len, uint32_t flags)
{
    int32_t ret = DC_DP_SUCCESS;
    int32_t buf_idx;
    phys_addr_t **tmp_buflist;
    uint32_t buf_free_count = 0;
    struct dp_buffer_info info = {0};
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    uint8_t high_4bits;

    /* skip tx buffer return only for LGM B0 onward */
    if (IS_ENABLED(CONFIG_X86_INTEL_LGM) && (g_chip_ver >= MPS_CHIPID_VER_B0)) {
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,
            "skip tx buffer return for LGM chip_ver:%d\n", g_chip_ver);
        goto out;
    }

    if (!dev_ctx) {
        ret = DC_DP_FAILURE;
        goto out;
    }

    if ((buflist_len > 0) && buflist) {
        info.policy_base = dev_ctx->shared_info->tx_policy_base;
        info.policy_num = dev_ctx->shared_info->tx_policy_num;
        high_4bits = dev_ctx->shared_info->high_4bits;

        tmp_buflist = (phys_addr_t **)buflist;
        for (buf_idx = 0; buf_idx < buflist_len; buf_idx++) {
            info.addr = (phys_addr_t)tmp_buflist[buf_idx];
            if (high_4bits)
                info.addr |= (phys_addr_t)((u64)high_4bits << 32);

            /*
             * flag=0, for buffer virtual address
             * flag=1, for buffer physical address (used)
             */
            ret = dp_free_buffer_by_policy(&info, 1);
            if (ret == DP_SUCCESS)
                buf_free_count++;
        }
        if (buf_free_count == buflist_len)
            ret = DC_DP_SUCCESS;
        else
            ret = DC_DP_FAILURE;
    }

out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Tx buffer returned = %d/%d\n", buf_free_count, buflist_len);

    return ret;
}

#if (defined(DCMODE_EXT_BRIDGE_FLOW_LEARNING) && DCMODE_EXT_BRIDGE_FLOW_LEARNING)
static int32_t
dcmode_ext_add_session_shortcut_forward(void *ctx, struct dp_subif *subif, struct sk_buff *skb, uint32_t flags)
{
    int32_t ret = DC_DP_SUCCESS;

    /* FIXME : For LGM, need to review. */
#if !defined(CONFIG_X86_INTEL_LGM)
#if IS_ENABLED(CONFIG_PPA) && defined(CONFIG_PPA_BR_SESS_LEARNING)
    struct ethhdr *eth;

    if (!skb)
        return DC_DP_FAILURE;

    /* FIXME : Enabled bridge flow learning globally for all netif (mainly appicable for WLAN netif) */
    if (flags & DC_DP_F_PREFORWARDING) {
        skb_reset_mac_header(skb);
        eth = eth_hdr(skb);
        if (unlikely(is_multicast_ether_addr(eth->h_dest)) ||
            unlikely(ether_addr_equal_64bits(eth->h_dest, skb->dev->dev_addr))) {
            /* Skipping, as no acceleration is possible */
            return 0;
        }

        skb->pkt_type = PACKET_OTHERHOST;
        skb->protocol = ntohs(eth->h_proto);
        skb_set_network_header(skb, ETH_HLEN);

        if (ppa_hook_session_add_fn)
            ret = ppa_hook_session_add_fn(skb, NULL, (PPA_F_BRIDGED_SESSION | PPA_F_BEFORE_NAT_TRANSFORM));
    } else if (flags & DC_DP_F_POSTFORWARDING) {
        if (ppa_hook_session_add_fn)
            ret = ppa_hook_session_add_fn(skb, NULL, PPA_F_BRIDGED_SESSION);
    }
#endif /* CONFIG_PPA_BR_SESS_LEARNING */
#endif /* CONFIG_X86_INTEL_LGM */

    return ret;
}
#endif /* DCMODE_EXT_BRIDGE_FLOW_LEARNING */

static int32_t
dcmode_ext_disconn_if(void *ctx, struct net_device *netif, struct dp_subif *subif_id,
                      uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags)
{
    int32_t ret = DC_DP_SUCCESS;

#if IS_ENABLED(CONFIG_PPA)
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;

    /* Multiport device? */
    if (subif_id && dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_forward_map_subifid(dev_ctx, subif_id);

    /* Remove all the sessions from PPA */
    if (ppa_hook_disconn_if_fn)
        ret = ppa_hook_disconn_if_fn(netif, subif_id, mac_addr, flags);

    /* Multiport device? */
    if (subif_id && dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, subif_id);
#endif /* CONFIG_PPA */

    return ret;
}

/*
 * ========================================================================
 * Callbacks Registered to Datapath Library/Core
 * ========================================================================
 */
static int32_t
dcmode_ext_rx_cb(struct net_device *rxif, struct net_device *txif,
                 struct sk_buff *skb, int32_t len)
{
    int32_t ret;
#if defined(CONFIG_X86_INTEL_LGM)
    struct dma_rx_desc_0 *desc_0 = (struct dma_rx_desc_0 *)&skb->DW0;
#else
    struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;
#endif /* CONFIG_X86_INTEL_LGM */
    struct dp_subif *rx_subif = NULL;
    struct dcmode_ext_dev_info *dev_ctx = NULL;

    if (!skb) {
        DC_DP_ERROR("failed to receive as skb=%p!!!\n", skb);
        goto err_out;
    }

    rx_subif = kmalloc(sizeof(struct dp_subif), GFP_ATOMIC);
    if (!rx_subif) {
        DC_DP_ERROR("failed to allocate rx_subif!!!\n");
        goto err_out_drop;
    }

#if defined(CONFIG_X86_INTEL_LGM)
    /* FIXME: workaround to extract received port_id */
    ret = dcmode_ext_get_subif_by_dev(rxif, rx_subif);
    if (ret != DC_DP_SUCCESS) {
        DC_DP_ERROR("why <port_id=%d, subif_id=0x%x> for rxif=%s???\n",
            rx_subif->port_id, rx_subif->subif, (rxif ? rxif->name : "NA"));
        goto err_out_drop;
    }
    rx_subif->subif = desc_0->field.dest_sub_if_id;
#else
    rx_subif->port_id = desc_1->field.ip;
    rx_subif->subif = desc_1->field.session_id;

    len -= sizeof(struct pmac_rx_hdr);
    skb_pull(skb, sizeof(struct pmac_rx_hdr));
#endif /* CONFIG_X86_INTEL_LGM */

    dev_ctx = g_dcmode_ext_dev_p[rx_subif->port_id];
    if (!dev_ctx) {
        DC_DP_ERROR("failed to receive as dev_ctx=%pK for port:%d!\n",
            dev_ctx, rx_subif->port_id);
        goto err_out_drop;
    }

    /* Multiport device? */
    if (is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, rx_subif);

    if (rxif && (rxif->features & NETIF_F_RXCSUM))
        skb->ip_summed = CHECKSUM_UNNECESSARY;

    ret = dc_dp_rx(rxif, txif, rx_subif, skb, skb->len, 0);

    if (rx_subif)
        kfree(rx_subif);
    return ret;

err_out_drop:
    dev_kfree_skb_any(skb);
    if (rx_subif)
        kfree(rx_subif);

err_out:
    return DP_FAILURE;
}

static int32_t
dcmode_ext_get_netif_subifid_cb(struct net_device *netif,
                                struct sk_buff *skb, void *subif_data,
                                uint8_t dst_mac[MAX_ETH_ALEN],
                                dp_subif_t *subif, uint32_t flags) /*! get subifid */
{
    int32_t ret = 1;
    struct dcmode_ext_dev_info *dev_ctx;
    struct dcmode_ext_dev_shared_info *dev_shared_ctx;
    int32_t subif_idx;

    /* Validate input argument */
    if (!netif) {
        DC_DP_ERROR("netif is NULL!\n");
        goto out;
    }

    if (!subif) {
        DC_DP_ERROR("subif is NULL!\n");
        goto out;
    }

    /* Validate device context */
    dev_ctx = g_dcmode_ext_dev_p[subif->port_id];
    if (!dev_ctx) {
        DC_DP_ERROR("port:%d not registered!\n", subif->port_id);
        goto out;
    }

    dev_shared_ctx = dev_ctx->shared_info;
    if (!dev_shared_ctx) {
        DC_DP_ERROR("port:%d not registered!\n", subif->port_id);
        goto out;
    }

    /* FIXME : No valid subif from DPLib in get_subif_fn CB? */
    subif_idx = DCMODE_EXT_GET_SUBIFIDX(subif->subif);
    if (dev_shared_ctx->subif_info[subif_idx].netif != netif) {
        for (subif_idx = 0; subif_idx < DCMODE_EXT_MAX_SUBIF_PER_DEV; subif_idx++) {
            if (dev_shared_ctx->subif_info[subif_idx].netif == netif) {
                subif->subif &= ~(DCMODE_EXT_SUBIFID_MASK << DCMODE_EXT_SUBIFID_OFFSET);
                subif->subif |= (subif_idx << DCMODE_EXT_SUBIFID_OFFSET);
                break;
            }
        }

        if (subif_idx == DCMODE_EXT_MAX_SUBIF_PER_DEV) {
            DC_DP_ERROR("No matching netif=0x%pK!\n", netif);
            goto out;
        }
    }

    multiport_wa_reverse_map_subifid(dev_ctx, subif);
    ret = dc_dp_get_netif_subifid(netif, skb, subif_data, dst_mac, subif, flags);

    /* Multiport 1st device? */
    if (subif_idx >= MULTIPORT_WORKAROUND_MAX_SUBIF_NUM)
        subif->subif |= MULTIPORT_WORKAROUND_SUBIFID_MASK;

    /* Multiport sub-sequent device? */
    else
        subif->port_id = dev_ctx->shared_info->port_id;

out:
    return ret;
}

/*
 * ========================================================================
 * Misclleneous API
 * ========================================================================
 */

static int32_t
dcmode_ext_get_netif_stats(void *ctx,
                           struct net_device *netif, struct dp_subif *subif_id,
                           struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
    int32_t ret;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    uint32_t dp_flags = 0x0;

    /* Multiport radio? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_forward_map_subifid(dev_ctx, subif_id);

    if (flags & DC_DP_F_SUBIF_LOGICAL)
        dp_flags = DP_F_STATS_SUBIF;

    ret = dp_get_netif_stats(netif, subif_id, if_stats, dp_flags);

    /* Multiport radio? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

    return ret;
}

static int32_t
dcmode_ext_clear_netif_stats(void *ctx,
                             struct net_device *netif, struct dp_subif *subif_id,
                             uint32_t flags)
{
    int32_t ret;
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
    uint32_t dp_flags = 0x0;

    /* Multiport radio? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_forward_map_subifid(dev_ctx, subif_id);

    if (flags & DC_DP_F_SUBIF_LOGICAL)
        dp_flags = DP_F_STATS_SUBIF;

    ret = dp_clear_netif_stats(netif, subif_id, dp_flags);

    /* Multiport radio? */
    if (dev_ctx && is_multiport(dev_ctx->alloc_flags))
        multiport_wa_reverse_map_subifid(dev_ctx, subif_id);

    return ret;
}

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
static int32_t
dcmode_ext_get_property(void *ctx,
                        struct net_device *netif, struct dp_subif *subif_id,
                        struct dc_dp_prop *property)
{
    int32_t ret = DC_DP_FAILURE;
    uint32_t pos;
    unsigned long prop_id_bitmap;

    prop_id_bitmap = property->prop_id;
    for_each_set_bit(pos, &prop_id_bitmap, SZ_32) {
        switch (1 << pos) {
        case DC_DP_PROP_ID_MTU:
            ret = dp_get_mtu_size(netif, &property->mtu);
            break;
        case DC_DP_PROP_ID_DGAF:
            /* TODO: need to call respective ppa api */
            property->dgaf_disabled = 0;
            ret = DC_DP_SUCCESS;
            break;
        default:
            pr_debug("[%s] Unsupported property id=0x%x for dev=%s\n",
                __func__, (1 << pos), (netif ? netif->name : "NA"));
            break;
        }
    }

    return ret;
}

static int32_t
dcmode_ext_set_property(void *ctx,
                        struct net_device *netif, struct dp_subif *subif_id,
                        struct dc_dp_prop *property)
{
    int32_t ret = DC_DP_FAILURE;
    uint8_t pos;
    unsigned long prop_id_bitmap;

    prop_id_bitmap = property->prop_id;
    for_each_set_bit(pos, &prop_id_bitmap, SZ_32) {
        switch (1 << pos) {
        case DC_DP_PROP_ID_MTU:
            ret = dp_set_mtu_size(netif, property->mtu);
            break;
        case DC_DP_PROP_ID_DGAF:
            /* TODO: need to call respective ppa api */
            if (!property->dgaf_disabled)
                ret = DC_DP_SUCCESS;
            break;
        default:
            pr_debug("[%s] Unsupported property id=0x%x for dev=%s\n",
                __func__, (1 << pos), (netif ? netif->name : "NA"));
            break;
        }
    }

    return ret;
}
#endif /* CONFIG_X86_INTEL_LGM */

static void
dcmode_ext_get_subif_param(void *ctx, struct dc_dp_subif *dc_subif)
{
    uint8_t mcf;

    if (!dc_subif)
        return;

    dc_subif->vap_id = DCMODE_EXT_GET_BITS(dc_subif->subif,
        DCMODE_EXT_SUBIFID_OFFSET, DCMODE_EXT_SUBIFID_BITSZ);
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
    mcf = DCMODE_EXT_GET_BITS(dc_subif->subif, 15, 1);
#else
    mcf = DCMODE_EXT_GET_BITS(dc_subif->subif, 14, 1);
#endif /* CONFIG_X86_INTEL_LGM */
    if (mcf) {
        dc_subif->mcf = mcf;
        dc_subif->mc_index = DCMODE_EXT_GET_BITS(dc_subif->subif, 0, 8);
    } else
        dc_subif->sta_id = DCMODE_EXT_GET_BITS(dc_subif->subif, 0, 8);
}

static void
dcmode_ext_set_subif_param(void *ctx, struct dc_dp_subif *dc_subif)
{
    if (!dc_subif)
        return;

    DCMODE_EXT_SET_BITS(dc_subif->subif, DCMODE_EXT_SUBIFID_OFFSET,
        DCMODE_EXT_SUBIFID_BITSZ, dc_subif->vap_id);
    if (dc_subif->mcf) {
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
        DCMODE_EXT_SET_BITS(dc_subif->subif, 15, 1, dc_subif->mcf);
#else
        DCMODE_EXT_SET_BITS(dc_subif->subif, 14, 1, dc_subif->mcf);
#endif /* CONFIG_X86_INTEL_LGM */
        DCMODE_EXT_SET_BITS(dc_subif->subif, 0, 8, dc_subif->mc_index);
    } else
        DCMODE_EXT_SET_BITS(dc_subif->subif, 0, 8, dc_subif->sta_id);
}

static void
dcmode_ext_dump_proc(void *ctx, struct seq_file *seq)
{
    struct dcmode_ext_dev_info *dev_ctx = (struct dcmode_ext_dev_info *)ctx;
#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    int i;
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

    if (!dev_ctx)
        return;

    seq_printf(seq, "    cqm_pid:           %d\n",
           dev_ctx->shared_info->cqm_pid);

    seq_printf(seq, "    dma_ch:            %d\n",
           dev_ctx->shared_info->dma_ch);

    seq_printf(seq, "    num_bufpools:      %02d\n",
           dev_ctx->shared_info->num_bufpools);

#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    for (i = 0; i < DCMODE_CHAN_MAX; i++) {
        if (DCMODE_CHAN_DEV2SOC == i)
            seq_printf(seq, "    Ring: dev2soc:\n");
        else if (DCMODE_CHAN_DEV2SOC_EXCEPT == i)
            seq_printf(seq, "    Ring: dev2soc_frag_except:\n");
        else
            continue;

        seq_printf(seq, "      Ring Base:           P:0x%pK, V:0x%pK\n",
               dev_ctx->shared_info->ch[i].ring.phys, dev_ctx->shared_info->ch[i].ring.virt);
        seq_printf(seq, "      Ring Size:           %d, each of size %d DW\n",
               dev_ctx->shared_info->ch[i].ring.size, dev_ctx->shared_info->ch[i].ring.desc_dwsz);
        seq_printf(seq, "      Ring Track:          rp=%#x wp=%#x\n",
               dev_ctx->shared_info->ch[i].ring.trck.rp, dev_ctx->shared_info->ch[i].ring.trck.wp);
        seq_printf(seq, "      Soc To Dev Write Counter:           P:0x%pK, V:0x%pK (Size %d Bytes)\n",
               dev_ctx->shared_info->ch[i].write_cntr.phys, dev_ctx->shared_info->ch[i].write_cntr.virt,
               dev_ctx->shared_info->ch[i].write_cntr.size);
        seq_printf(seq, "      Soc From Dev Read Counter:           P:0x%pK, V:0x%pK (Size %d Bytes)\n",
               dev_ctx->shared_info->ch[i].read_cntr.phys, dev_ctx->shared_info->ch[i].read_cntr.virt,
               dev_ctx->shared_info->ch[i].read_cntr.size);
    }
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */

    seq_printf(seq, "    umt_id:            %d\n",
           dev_ctx->shared_info->umt_id);
    seq_printf(seq, "    umt_period:        %d (in us)\n",
           dev_ctx->shared_info->umt_period);
}

static struct dc_dp_dcmode_ops dcmode_ext_ops = {
    .get_peripheral_config = dcmode_ext_get_peripheral_config,
    .get_host_capability = dcmode_ext_get_host_capability,
    .mark_pkt_devqos = dcmode_ext_mark_pkt_devqos,
    .register_dev_ex = dcmode_ext_register_dev_ex,
    .register_subif = dcmode_ext_register_subif,
    .xmit = dcmode_ext_xmit,
#ifdef DEV2SOC_FRAG_EXCEPTION_HANDLING
    .handle_ring_sw = dcmode_ext_handle_ring_sw,
#endif /* DEV2SOC_FRAG_EXCEPTION_HANDLING */
    .return_bufs = dcmode_ext_return_bufs,
    .add_session_shortcut_forward = dcmode_ext_add_session_shortcut_forward,
    .disconn_if = dcmode_ext_disconn_if,
    .get_netif_stats = dcmode_ext_get_netif_stats,
    .clear_netif_stats = dcmode_ext_clear_netif_stats,
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
    .get_property = dcmode_ext_get_property,
    .set_property = dcmode_ext_set_property,
#endif /* CONFIG_X86_INTEL_LGM */
    .get_subif_param = dcmode_ext_get_subif_param,
    .set_subif_param = dcmode_ext_set_subif_param,
    .dump_proc = dcmode_ext_dump_proc
};

static struct dc_dp_dcmode dcmode_ext = {
    .dcmode_cap = DC_DP_F_DCMODE_HW | DC_DP_F_DCMODE_0,
    .dcmode_ops = &dcmode_ext_ops
};

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
static int qos_mgr_class2prio_event(QOS_MGR_NOTIFIER_BLOCK *nb,
                                    unsigned long action, void *ptr)
{
    struct qos_mgr_class2prio_notifier_info *info;

    switch (action) {
        case QOS_MGR_CLASS2PRIO_DEFAULT:
        case QOS_MGR_CLASS2PRIO_CHANGE:
            info = (struct qos_mgr_class2prio_notifier_info *)ptr;
            if (info)
                dc_dp_qos_class2prio(info->port_id, info->dev, info->class2prio);
            break;
        default:
            break;
    }

    return PPA_NOTIFY_OK;
}

QOS_MGR_NOTIFIER_BLOCK qos_mgr_class2prio_notifier = {
    .notifier_call = qos_mgr_class2prio_event
};
#elif IS_ENABLED(CONFIG_PPA)
static int ppa_class2prio_event(PPA_NOTIFIER_BLOCK *nb,
                                unsigned long action, void *ptr)
{
    struct ppa_class2prio_notifier_info *info;

    switch (action) {
    case PPA_CLASS2PRIO_DEFAULT:
    case PPA_CLASS2PRIO_CHANGE:
        info = (struct ppa_class2prio_notifier_info *)ptr;
        if (info)
            dc_dp_qos_class2prio(info->port_id, info->dev, info->class2prio);
        break;
    default:
        break;
    }

    return PPA_NOTIFY_OK;
}

PPA_NOTIFIER_BLOCK ppa_class2prio_notifier = {
    .notifier_call = ppa_class2prio_event
};
#endif /* CONFIG_X86_INTEL_LGM */

static __init int dcmode_ext_init_module(void)
{
    int32_t ret = 0;

    if (!g_dcmode_ext_init_ok) {
        /* Get Platform capability */
        ret = dp_get_cap(&g_dp_cap, 0);
        if (ret)
            DC_DP_ERROR("failed to get DP capability!!!\n");

        memset(g_dcmode_ext_dev, 0, sizeof(g_dcmode_ext_dev));

        /* Register DCMODE */
        ret = dc_dp_register_dcmode(&dcmode_ext, 0);

#if IS_ENABLED(CONFIG_PPA)
        ppa_check_if_netif_fastpath_fn = dc_dp_check_if_netif_fastpath;
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
        qos_mgr_register_event_notifier(&qos_mgr_class2prio_notifier);
#else
        ppa_register_event_notifier(&ppa_class2prio_notifier);
#endif /* CONFIG_X86_INTEL_LGM */
#endif /* CONFIG_PPA */

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
        g_chip_ver = find_chip_ver();
        qos_mgr_get_class2devqos_fn = dc_dp_get_class2devqos;
#endif /* CONFIG_X86_INTEL_LGM */

        g_dcmode_ext_init_ok = 1;
    }

    return ret;
}

static __exit void dcmode_ext_exit_module(void)
{
    if (g_dcmode_ext_init_ok) {
#if IS_ENABLED(CONFIG_PPA)
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
        qos_mgr_unregister_event_notifier(&qos_mgr_class2prio_notifier);
#else
        ppa_unregister_event_notifier(&ppa_class2prio_notifier);
#endif /* CONFIG_X86_INTEL_LGM */
        ppa_check_if_netif_fastpath_fn = NULL;
#endif /* CONFIG_PPA */

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
        qos_mgr_get_class2devqos_fn = NULL;
#endif /* CONFIG_X86_INTEL_LGM */

        /* De-register DCMODE */
        dc_dp_register_dcmode(&dcmode_ext, DC_DP_F_DCMODE_DEREGISTER);

        /* Reset private data structure */
        memset(g_dcmode_ext_dev, 0, sizeof(g_dcmode_ext_dev));
        g_dcmode_ext_init_ok = 0;
    }
}

module_init(dcmode_ext_init_module);
module_exit(dcmode_ext_exit_module);

MODULE_AUTHOR("Anath Bandhu Garai");
MODULE_DESCRIPTION("Extended DC Mode support for any platform");
MODULE_LICENSE("GPL");
MODULE_VERSION(DCMODE_EXT_DRV_MODULE_VERSION);
