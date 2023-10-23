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

/*#include <asm-generic/delay.h>*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include <dc_mode_sw_common.h>

#if 0
/* defines */
#define SW_DCMODE1_MAX_DEV_NUM       32
#define CACHE_LINE_SIZE 32

#define __CH_PRINT(ch, foo) foo(ch, __FUNCTION__)

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

/* ######################## */

#define DC_DP_LOCK    spin_lock_bh
#define DC_DP_UNLOCK  spin_unlock_bh

/* ######################## */
#define SW_DCMODE1_XFER_IN  0
#define SW_DCMODE1_XFER_OUT 1

typedef enum {
    SW_DCMODE1_CHAN_SOC2DEV = 0,
    SW_DCMODE1_CHAN_SOC2DEV_RET = 1,
    SW_DCMODE1_CHAN_DEV2SOC = 2,
    SW_DCMODE1_CHAN_DEV2SOC_RET = 3,
    SW_DCMODE1_CHAN_MAX = 4,
} sw_dcmode1_chan_id_t;

typedef struct
{
    unsigned int phys;
    void *       virt;
    unsigned int size;
    uint32_t     desc_dwsz;
    //uint64_t     cookie;
} sw_dcmode_mem_block_t;

/*
 * DW0 :
 *     tid - reserved
 *     sid : Subif info (MCF : Bit 13, VAPID : Bit 8-12 and STAID : Bit 0-7)
 * DW1 : misc - reserved
 * DW2 :
 *     addr - buffer address
 * DW3 :
 *     flgs (8 Bit) : Bit OWN(31),SOP(30),EOP(29)
 *                    class (Bit 24-27)
 *     size
 */
typedef struct __attribute__ ((__packed__)) __attribute__ ((aligned(16)))
{
    uint16_t tid;  ///< Buffer tag, set by app CPU, replicated by FW
    uint16_t sid;  ///< Session ID, set by FW
    uint32_t misc; ///< Misc...
    uint32_t addr; ///< Physical address of a buffer
    uint8_t  flgs; ///< Buffer control flags
    uint8_t  rsvd; ///< Reserved...
    uint16_t size; ///< Buffer size
} sw_dcmode_bd_t;

#if 0
typedef struct __attribute__ ((__packed__))
{
    union
    {
        volatile uint32_t wp;
        unsigned char     wp_cache_line[CACHE_LINE_SIZE];
    };

    union
    {
        volatile uint32_t rp;
        unsigned char     rp_cache_line[CACHE_LINE_SIZE];
    };

    uint32_t mask;
    uint32_t size;

} sw_dcmode_ring_t;
#endif

typedef struct
{
    unsigned int last_accumulated; /* Counter last accumulated value */
    unsigned int size; /* Counter max value (wrap-up value) */
} sw_dcmode_cntr_track_t;

typedef struct
{
    unsigned int phys; /* Counter physical address */
    void *       virt; /* Counter virtual address */
    unsigned int size; /* Counter size (=4Bytes) */
    unsigned int endian; /* Big or little endian */
    unsigned int mode; /* Cumulative or Incremental mode */
} sw_dcmode_cntr_t;

typedef struct __attribute__ ((aligned(32)))
{
    union
    {
        uint32_t cntr;
        unsigned char     cntr_cache_line[CACHE_LINE_SIZE];
    };
} sw_dcmode_cntr_type_t;

typedef struct
{
    /* Ring */
    sw_dcmode_mem_block_t data;  // Descriptor data
    sw_dcmode_ring_t     trck;  // Ring pointers and metadata tracker
    sw_dcmode_bd_t      * bufs;  // Descriptors local virtual address
    unsigned int     id  ;  // Channel ID

    /* Counter */
    sw_dcmode_cntr_track_t   lcl_cntr_trck;  // SoC counter tracker
    sw_dcmode_cntr_t   lcl_cntr;  // SoC counter and metadata tracker
    sw_dcmode_cntr_track_t   rm_cntr_trck;  // Device counter tracker
    sw_dcmode_cntr_t   rm_cntr;  // Device counter and metadata tracker

    char           * name;  // User friendly name
} sw_dcmode_chan_t;
#endif

typedef struct sw_dcmode1_bdr_mirror {
    struct list_head list;
    void *dma_addr;
    struct sk_buff *skb;
} sw_dcmode_bdr_mirror_t;

typedef struct sw_dcmode1_subif_info {
    int32_t used;
#define SW_DCMODE1_SUBIF_FREE         0x0
#define SW_DCMODE1_SUBIF_USED         0x1
    uint32_t reg_subif_flags;
    uint32_t subif:15;
    struct net_device  *netif;   /*! pointer to  net_device*/
    char device_name[IFNAMSIZ];  /*! devide name, like wlan0, */
} sw_dcmode1_subif_info_t;

typedef struct sw_dcmode1_dev_info {
    int32_t state;
#define SW_DCMODE1_ST_DEV_FREE         0x0
#define SW_DCMODE1_ST_DEV_REGISTERED   0x1
#define SW_DCMODE1_ST_SUBIF_REGISTERED 0x2
    uint32_t reg_dev_flags;
    void *tx_bdr_mirror;
    struct list_head free_tx_bdr_mirror;
    struct list_head alloc_tx_bdr_mirror;

    void *rx_bdr_mirror;
    struct list_head free_rx_bdr_mirror;
    struct list_head alloc_rx_bdr_mirror;
    sw_dcmode_chan_t    ch[SW_DCMODE1_CHAN_MAX]; // Array of Rings. Typically 4
    int32_t num_subif;
#define SW_DCMODE1_MAX_SUBIF_PER_DEV 16
    sw_dcmode1_subif_info_t subif_info[SW_DCMODE1_MAX_SUBIF_PER_DEV];
} sw_dcmode1_dev_info_t;
static struct sw_dcmode1_dev_info g_sw_dcmode1_dev_info[SW_DCMODE1_MAX_DEV_NUM];
DC_DP_DEFINE_LOCK(g_sw_dcmode1_dev_lock);
static int32_t g_sw_dcmode1_init_ok = 0;

#define SW_DCMODE1_CHAN_SOC2DEV_P(ctx) (&((ctx)->ch[SW_DCMODE1_CHAN_SOC2DEV]))
#define SW_DCMODE1_CHAN_SOC2DEV_RET_P(ctx) (&((ctx)->ch[SW_DCMODE1_CHAN_SOC2DEV_RET]))
#define SW_DCMODE1_CHAN_DEV2SOC_P(ctx) (&((ctx)->ch[SW_DCMODE1_CHAN_DEV2SOC]))
#define SW_DCMODE1_CHAN_DEV2SOC_RET_P(ctx) (&((ctx)->ch[SW_DCMODE1_CHAN_DEV2SOC_RET]))

#define DC_DP_SOC2DEV_RING_RES_P(p_dc_res) (&((p_dc_res)->rings.soc2dev))
#define DC_DP_SOC2DEV_RET_RING_RES_P(p_dc_res) (&((p_dc_res)->rings.soc2dev_ret))
#define DC_DP_DEV2SOC_RING_RES_P(p_dc_res) (&((p_dc_res)->rings.dev2soc))
#define DC_DP_DEV2SOC_RET_RING_RES_P(p_dc_res) (&((p_dc_res)->rings.dev2soc_ret))

#define DC_DP_CNTR_RES0_P(p_dc_res) (&((p_dc_res)->dccntr[0]))

#define DC_DP_RING_VIRT_BASE(p_ring_res) ((p_ring_res)->base)
#define DC_DP_RING_PHYS_BASE(p_ring_res) ((p_ring_res)->phys_base)
#define DC_DP_RING_SIZE(p_ring_res)    ((p_ring_res)->size)
#define DC_DP_RING_FLAGS(p_ring_res)    ((p_ring_res)->flags)
#define DC_DP_RING_PRIV(p_ring_res) ((p_ring_res)->ring)

#define SW_DCMODE1_PRIV_RING_VIRT_BASE(chan) ((chan)->data.virt)
#define SW_DCMODE1_PRIV_RING_PHYS_BASE(chan) ((chan)->data.phys)
#define SW_DCMODE1_PRIV_RING_SIZE(chan) ((chan)->data.size)
#define SW_DCMODE1_PRIV_RING_BUFS(chan) ((chan)->bufs)

#define SW_DCMODE1_SOC_CNTR_VIRT_BASE(chan) ((chan)->lcl_cntr->virt);
#define SW_DCMODE1_SOC_CNTR_PHYS_BASE(chan) ((chan)->lcl_cntr.phys);
#define SW_DCMODE1_SOC_CNTR_LEN(chan) ((chan)->lcl_cntr.size);
#define SW_DCMODE1_DEV_CNTR_VIRT_BASE(chan) ((chan)->rm_cntr->virt);
#define SW_DCMODE1_DEV_CNTR_PHYS_BASE(chan) ((chan)->rm_cntr.phys);
#define SW_DCMODE1_DEV_CNTR_LEN(chan) ((chan)->rm_cntr.size);

//#define SW_DCMODE1_SOC_LOCAL_CNTR_LAST_ACCUM(chan,n) (((chan)->lcl_cntr_trck.last_accumulated)+=n);
//#define SW_DCMODE1_SOC_REMOTE_CNTR_LAST_ACCUM(chan,n) (((chan)->rmt_cntr_trck.last_accumulated)+=n);

//#define SW_DCMODE1_REMOTE_VIRT(ch) ((unsigned int *) ch->rm_cntr.virt)

/*static inline int32_t
__xfer(sw_dcmode_chan_t *ch, sw_dcmode_bd_t *d, unsigned n, int xt, unsigned w);*/
void dump_list(struct list_head *head);

/* ################## */
static struct sw_dcmode1_dev_info *
sw_dcmode1_alloc_device(int32_t port_id)
{
    struct sw_dcmode1_dev_info *dev_ctx = NULL;

    if (port_id >= 0 && port_id < SW_DCMODE1_MAX_DEV_NUM) {
        if ( (SW_DCMODE1_ST_DEV_FREE == g_sw_dcmode1_dev_info[port_id].state) ) {
            dev_ctx = &g_sw_dcmode1_dev_info[port_id];
            memset(dev_ctx, 0, sizeof(struct sw_dcmode1_dev_info));
            dev_ctx->state = SW_DCMODE1_ST_DEV_REGISTERED;
        }
    }

    return dev_ctx;
}

static inline void
sw_dcmode1_free_device(struct sw_dcmode1_dev_info *dev_ctx, int32_t port_id)
{
    if (port_id >= 0 && port_id < SW_DCMODE1_MAX_DEV_NUM) {
        if ( (SW_DCMODE1_ST_DEV_REGISTERED == dev_ctx->state) )
            memset(dev_ctx, 0, sizeof(struct sw_dcmode1_dev_info));
    }
}

static inline unsigned int
sw_dcmode1_get_subifidx(uint32_t subif)
{
    return ((subif >> 8) & 0xF);
}

static inline unsigned int
sw_dcmode1_get_subifid(int32_t subif_idx)
{
    return ((subif_idx & 0xF) << 8);
}

static struct sw_dcmode1_subif_info *
sw_dcmode1_alloc_subif(struct sw_dcmode1_dev_info *dev_ctx, struct dp_subif *subif)
{
    struct sw_dcmode1_subif_info *subif_info = NULL;
    int32_t subif_idx;
    int32_t start = 0;
    int32_t end = SW_DCMODE1_MAX_SUBIF_PER_DEV;

    if (subif && subif->subif >= 0) {
        start = sw_dcmode1_get_subifidx(subif->subif);
        end = MAX((start + 1), SW_DCMODE1_MAX_SUBIF_PER_DEV);
    } else {
        printk("sw_dcmode1_alloc_subif :else :subif address = %p\n",subif);
        start = 0;
        end = MAX((start + 1), SW_DCMODE1_MAX_SUBIF_PER_DEV);

    }

    for (subif_idx = start; subif_idx < end; subif_idx++) {
        if ( (SW_DCMODE1_SUBIF_FREE == dev_ctx->subif_info[subif_idx].used) ) {
            subif_info = &dev_ctx->subif_info[subif_idx];
            subif_info->subif = sw_dcmode1_get_subifid(subif_idx);
            subif_info->used = SW_DCMODE1_SUBIF_USED;
            break;
        }
    }

    return subif_info;
}

static inline void
sw_dcmode1_free_subif(struct sw_dcmode1_dev_info *dev_ctx, uint32_t subif_id)
{
    int32_t subif_idx;
    struct sw_dcmode1_subif_info *subif_info = NULL;

    subif_idx = sw_dcmode1_get_subifidx(subif_id);
    subif_info = &dev_ctx->subif_info[subif_idx];

    if ( (SW_DCMODE1_SUBIF_USED == subif_info->used) )
        memset(subif_info, 0, sizeof(struct sw_dcmode1_subif_info));
}

static inline void *
sw_dcmode_mem_dma_alloc(uint32_t size, dma_addr_t *phys_base, uint32_t tag)
{
#if 0
    return kmalloc(size, GFP_ATOMIC | __GFP_DMA);
#else
    return dma_alloc_coherent(NULL, size, phys_base, GFP_DMA);
#endif
}

static inline void
sw_dcmode_mem_dma_free(void *virt_base, dma_addr_t phys_base, uint32_t size)
{
#if 0
    kfree(buf);
#else
    dma_free_coherent(NULL, size, virt_base, phys_base);
#endif
}

#if 0
static __INLINE void*
mtlk_osal_mem_dma_uncached_alloc (struct device *dev, uint32_t size, dma_addr_t *dma_addr)
{
#ifdef MTCFG_ENABLE_OBJPOOL
  void *buf = dma_alloc_coherent(dev, mem_leak_get_full_allocation_size(size), dma_addr, GFP_DMA);
  void *buf_leak = mem_leak_handle_allocated_buffer(buf, size, MTLK_SLID);
  *dma_addr += buf_leak - buf;
  return buf_leak;
#else
  return dma_alloc_coherent(dev, size, dma_addr, GFP_DMA);
#endif
}

static __INLINE void
mtlk_osal_mem_dma_uncached_free (struct device *dev, void *buffer, uint32_t size, dma_addr_t dma_addr)
{
#ifdef MTCFG_ENABLE_OBJPOOL
  void *buf = mem_leak_handle_buffer_to_free(buffer);
  dma_free_coherent(dev, size, buf, dma_addr - (buffer - buf));
#else
  dma_free_coherent(dev, size, buffer, dma_addr);
#endif
}
#endif
/*
typedef enum
{
  SW_DCMODE_DATA_FROM_BIDIRECTIONAL = PCI_DMA_BIDIRECTIONAL,
  SW_DCMODE_DATA_FROM_DEVICE = PCI_DMA_FROMDEVICE,
  SW_DCMODE_DATA_TO_DEVICE = PCI_DMA_TODEVICE
} sw_dcmode_data_direction_e;

static inline uint32_t
sw_dcmode_map_to_phys_addr (void                  *ctx,
                            void                  *buffer,
                            uint32_t                 size,
                            sw_dcmode_data_direction_e  direction)
{

    return dma_map_single(ctx, buffer, size, direction);
}

static inline void
sw_dcmode_unmap_phys_addr (void                  *ctx,
                           uint32_t                 addr,
                           uint32_t                 size,
                           sw_dcmode_data_direction_e  direction)
{
    dma_unmap_single(ctx, addr, size, direction);
}
*/
#define sw_dcmode_nbuf_map_to_phys_addr(ctx, nbuf, size, direction) \
  sw_dcmode_map_to_phys_addr(ctx, nbuf->data, size, direction)

#define  sw_dcmode_nbuf_unmap_phys_addr(ctx, nbuf, addr, size, direction) \
  sw_dcmode_unmap_phys_addr(ctx, addr, size, direction)

/* ################## */

#if 0
static inline void
dump_bd(sw_dcmode_bd_t *bdp)
{
    pr_info("DW0:%#x DW1:%#x DW2:%#x DW3:%#x", );
}
#endif

static inline void *
sw_dcmode_osal_mem_ddr_pages_alloc (unsigned long size)
{
    return (void *)__get_free_pages(GFP_DMA, get_order(size));
}

static inline void
sw_dcmode_osal_mem_ddr_pages_free (void *free_mem, unsigned long size)
{
    free_pages((unsigned long)free_mem, get_order(size));
}

static inline sw_dcmode_bdr_mirror_t *
sw_dcmode_get_free_bdp_from_bdr_mirror(struct list_head *head, sw_dcmode_bdr_mirror_t *bdp, uint32_t flags)
{
    DC_DP_LOCK(&g_sw_dcmode1_dev_lock);
    bdp = list_first_entry_or_null(head, sw_dcmode_bdr_mirror_t, list);
    //printk("bdp=%p\n",bdp);
    if (bdp)
        list_del(&bdp->list);
    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);
    return bdp;
}

static inline void
sw_dcmode_put_bdp_to_bdr_mirror(struct list_head *head, sw_dcmode_bdr_mirror_t *bdp, uint32_t flags)
{
    DC_DP_LOCK(&g_sw_dcmode1_dev_lock);
    list_add_tail(&bdp->list, head);
    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);
}

static inline sw_dcmode_bdr_mirror_t *
sw_dcmode_find_entry_in_bdr_mirror(struct list_head *head, void *dma_addr, uint32_t flags)
{
    int32_t bFound = 0/*, i*/;
    sw_dcmode_bdr_mirror_t *bdp = NULL;
    sw_dcmode_bdr_mirror_t *tmp_entry = NULL;

    DC_DP_LOCK(&g_sw_dcmode1_dev_lock);
    list_for_each_entry_safe(bdp, tmp_entry, head, list) {
        if (((uint32_t)bdp->dma_addr & ~(0x07U)) == ((uint32_t)dma_addr & ~(0x07U)) ) {
            /*if ((flags & DC_DP_F_TX_COMPLETE) == DC_DP_F_TX_COMPLETE)
                printk("BD Found in TX mirror !!!\n");
            else
                printk("BD Found in RX mirror !!!\n");*/
            bFound = 1;
            list_del(&bdp->list);
            break;
        }
    }
    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);
    if (bFound == 0) {
        bdp = NULL;
#if 0
        //printk(" ========= PPE PDB Ring Dump [0x230000] ========== ");
        for (i = 0; i < 128; i++)
        {
             printk("[%d]--- [0x%08x] \n", i, readl(0xbb230000 + i*4));
        }
#endif
        printk("Not found buffer address = 0x%pk!!!\n", dma_addr);
        dump_list(head);
    }

    return bdp;
}

#if 0
struct sk_buff *swdc_build_skb(void *data, unsigned int frag_size)
{
    struct skb_shared_info *shinfo;
    struct sk_buff *skb;
    unsigned int size;
    size = frag_size;
    /*printk("%s data 0x%x size %d\r\n",__func__, data, frag_size);*/
    skb = alloc_skb_head(GFP_ATOMIC);
    if (!skb) {
        DC_DP_ERROR(" SKB head alloc failed!!!\n");
        return NULL;
    }

    size -= SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

    memset(skb, 0, offsetof(struct sk_buff, tail));
    skb->truesize = SKB_TRUESIZE(size);
    skb->head_frag = 0;
    atomic_set(&skb->users, 1);
    skb->head = data;
    skb->data = data;
    skb_reset_tail_pointer(skb);
    skb->end = skb->tail + size;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
    skb->mac_header = ~0U;
    skb->transport_header = ~0U;
#endif

    /* make sure we initialize shinfo sequentially */
    shinfo = skb_shinfo(skb);
    memset(shinfo, 0, offsetof(struct skb_shared_info, dataref));
    atomic_set(&shinfo->dataref, 1);
    kmemcheck_annotate_variable(shinfo->destructor_arg);

    return skb;
}
#endif

#define WRAPAROUND_16_BITS 0x0000FFFF
static int32_t sw_dcmode_update_counter(uint16_t *curr, uint16_t *last, uint32_t *accumulated)
{
    int32_t delta;

    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"%s:: local counter value :[%d] head : [%d] last accumulated count: [%d]\n",__FUNCTION__,*curr,*last, *accumulated);
    if ( *curr >= *last) {
        delta = (*curr - *last);
    } else {
        //printk("WRAPAROUND \n");
        delta = (*curr + WRAPAROUND_16_BITS - *last);
    }
    *accumulated += delta;
    //*last = *curr;
    return delta;

}

#define MAX_BD_HANDLE 1
#define MAX_BDRX_HANDLE 1

static int32_t
sw_dcmode1_handle_ring_sw(void *ctx, struct module *owner, uint32_t port_id,
                          struct net_device *dev, struct dc_dp_ring *ring, uint32_t flags)
{
    int32_t ret = 0, rx_ret = 0, i=0, no_of_bd=0, retry_cnt=0;
    uint16_t delta, local_cnt=0;
    sw_dcmode_bd_t bd_rx[MAX_BDRX_HANDLE] = {0};
    struct dp_subif subif_id = {0};
    struct sw_dcmode1_dev_info *dev_ctx = (struct sw_dcmode1_dev_info *) ctx;
    struct sk_buff *skb = NULL;
    sw_dcmode_bdr_mirror_t *tx_bdp = NULL;
    sw_dcmode_bdr_mirror_t *rx_bdp = NULL;
    sw_dcmode_bd_t bd[MAX_BD_HANDLE] = {0};

    sw_dcmode_chan_t *ch = (sw_dcmode_chan_t *)ring->ring;
    sw_dcmode_cntr_t   *locl_cntr = &ch->lcl_cntr;
    sw_dcmode_cntr_track_t   *locl_cntr_trck = &ch->lcl_cntr_trck;
    sw_dcmode_ring_t  *track = &ch->trck;

    // Validate ctx, owner, port_id, dev, ring
    if ( (NULL == dev_ctx) /*|| (NULL == owner) || (NULL == dev)*/ ) {
        DC_DP_ERROR("failed to handle ring for SW DCMode1!!!\n");
        goto err_out;
    }

    if (flags & DC_DP_F_RX) {
        /*
        1. Read the dev2soc_enq_base (in SoC memory) counter location;
            NOTE: counter value should be cumulative.
        2. LOOP:
        2.1. Process dev2soc ring based on the above counter value
        2.2. Read the buffer pointer for desc and build a skb
            NOTE : SOP,EOP handling
        2.3. Call Rx CB
        2.4. Allocate a new buffer and set to dev2soc_ret ring desc
        3. Update the dev2soc ring read/head pointer
        4. Update the dev2soc_ret ring write/head pointer
        5. Write the <counter value> to dev2soc_deq_base counter location;
            NOTE: keep correct endianness and mode
        6. Write the <counter value> to dev2soc_ret_enq_base counter location;
            NOTE: keep correct endianness and mode
         */

        //printk("<%s>head value=%d\n",__FUNCTION__,track->head);
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"local counter value :[%d]\n",*((uint32_t *)(locl_cntr->virt)));
RETRY:
        local_cnt = *((uint32_t *)(locl_cntr->virt));
        delta = sw_dcmode_update_counter(&local_cnt, &track->head, &locl_cntr_trck->last_accumulated);
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"Delta :[%d]\n",delta);

        /*if (*((uint32_t *)(locl_cntr->virt)) < track->head)
            DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"Local Counter<Track Head : delta:[%d] local counter value :[%d] head :[%d] tail:[%d] last accumulated count: [%d]\n",delta, *((uint32_t *)(locl_cntr->virt)), track->head, track->tail, locl_cntr_trck->last_accumulated);*/

        //if (delta == 0) {
//        printk(" [DC_DP]: RXOUT delta = %d, local cnt = %d, head = %d, ACA cnt = %d, ppe cnt = %d\n", delta, local_cnt, track->head, readl(0xbb050120), cpu_to_le32(readl(0xbb321488)));
        //}
        no_of_bd = MIN(delta, MAX_BDRX_HANDLE);
        //no_of_bd = MIN((*((uint32_t *)(locl_cntr->virt)) - track->head), 32);
        //no_of_bd = (*((uint32_t *)(locl_cntr->virt)) - track->head);
        if (no_of_bd == 0) {
            if (retry_cnt < 2){
                retry_cnt++;
                udelay(100);        //usleep(100);
                //printk("Retrying %d\n",retry_cnt);
                goto RETRY;
            } else {
                //DC_DP_ERROR("Nothing in RX_OUT handling: delta:[%d] Local counter = %d,head=%d tail:[%d] last accumulated count: [%d]\n", delta, *((uint32_t *)(locl_cntr->virt)), track->head, track->tail, locl_cntr_trck->last_accumulated);
                //printk(" [DC_DP]: RXOUT delta = %d, local cnt = %d, head = %d, ACA cnt = %d, PPE_RXOUT = %d PPE_RXIN = %d\n", delta, local_cnt, track->head, readl(0xbb050120), cpu_to_le32(readl(0xbb321488)), cpu_to_le32(readl(0xbb32148c)));
                return 0;
            }

        }
        memset(&bd_rx[0], 0, no_of_bd * sizeof(sw_dcmode_bd_t));

        rx_ret = __xfer((sw_dcmode_chan_t *)ring->ring, &bd_rx[0], no_of_bd, SW_DCMODE1_XFER_IN, 0); // on dev2soc ring
        //ret = __xfer((sw_dcmode_chan_t *)ring->ring, &bd, no_of_bd, SW_DCMODE1_XFER_IN, 0); // on dev2soc ring
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "==== rx_ret = %d===no_of_bd=%d==\n",rx_ret, no_of_bd);
        for (i = 0; i < no_of_bd; i++) {
            // for loop start

            // Fill subif_id from descriptor {port_id, Subif, STAID}
            subif_id.port_id = port_id;
            subif_id.subif = bd_rx[i].sid;
            //skb->data = (uint8_t*)sw_dcmode_unmap_phys_addr(dev_ctx, bd.addr, bd.size, SW_DCMODE_DATA_FROM_DEVICE);
            //skb = dc_dp_alloc_skb(bd.size, &subif_id, 0);

            //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"%s :bd.addr=[%#x] bd.size=%d\n",__FUNCTION__,bd[i].addr,bd[i].size);
            //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"bd.sid=%d , bd.misc=%d ,bd.flag =%d, bd.rsvd=%d \n",bd[i].sid,bd[i].misc,bd[i].flgs,bd[i].rsvd);
            if (bd_rx[i].addr == 0 || bd_rx[i].size == 0) {
                DC_DP_ERROR(" Invalid Buffer descriptor or size for RX_IN ring handling!!!\n");
                ret = -1;
                goto err_out;
            }
#if 1
            rx_bdp = sw_dcmode_find_entry_in_bdr_mirror(&dev_ctx->alloc_rx_bdr_mirror, (void *)bd_rx[i].addr, DC_DP_F_RX);
            if (NULL != rx_bdp) {
                skb = rx_bdp->skb;
                sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->free_rx_bdr_mirror, rx_bdp, 0);

                /* TODO: Update the Tx mib */

                //dc_dp_free_skb(&subif_id, skb);
            } else {
                DC_DP_ERROR(" RX BD not found !!!\n");
                ret = -1;
                goto err_out;
            }
            rx_bdp = NULL;
#endif
            //skb = build_skb((void*)phys_to_virt(bd.addr), 1000);
            //skb = build_skb((void*)phys_to_virt(bd[i].addr), DC_DP_PKT_BUF_SIZE_DEFAULT);
            //skb = build_skb((void*)phys_to_virt(bd.addr), bd.size);
            // Find net_device corresponding descriptor {port_id, Subif}

            // Build skb from descriptor buffer address and len

            // Fill skb->DW from descriptor status field
            if (!skb)
            {
                //recycle_data((void*)phys_to_virt(bd.addr));
                DC_DP_ERROR(" Invalid Buffer descriptor or size for RX_IN ring handling!!!\n");
                ret = -1;
                goto err_out;
            }
            else {
                //skb_reserve(skb, NET_SKB_PAD + NET_IP_ALIGN);
                //skb->dev = dev;
                //skb_put(skb, bd.size);
                skb_put(skb,bd_rx[i].size);
            }

            /* Invalidate the cache */
            sw_dcmode_map_to_phys_addr(NULL, skb->data, skb->len, SW_DCMODE_DATA_FROM_DEVICE);

            /*DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"<%s>:  skb->data = 0x%p, skb->head = 0x%p ,skb->tail = 0x%p, skb->len=%d\n",__FUNCTION__,
                    skb->data,skb->head,skb->tail,skb->len);*/

            //printk("========= call the rx callback ========\n");
            ret = dc_dp_rx((dev ? dev : g_sw_dcmode1_dev_info[port_id].subif_info[sw_dcmode1_get_subifidx(subif_id.subif)].netif), NULL, &subif_id, skb, skb->len, 0);
            //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG," <%s> RX Done --> Return the buffer to RX_RET ring\n",__FUNCTION__);
            /*
            1. Read the dev2soc_ret_deq_base counter location;
                NOTE: counter value should be cumulative.
            2. Update dev2soc_ret ring read/tail pointer
            */
            rx_bdp = sw_dcmode_get_free_bdp_from_bdr_mirror(&dev_ctx->free_rx_bdr_mirror, rx_bdp, 0);
            if ( (NULL == rx_bdp) ) {
                DC_DP_ERROR(" No free BD in RX_IN mirror ring !!!\n");
                goto err_out;
            }

            memset(&bd_rx[i], 0, sizeof(sw_dcmode_bd_t));
            subif_id.port_id = port_id;
            subif_id.subif = 0;
            skb = dc_dp_alloc_skb(DC_DP_PKT_BUF_SIZE_DEFAULT, &subif_id, 0);
                        if (!skb)
                goto err_out;
            //skb->head = NULL;
            //dev_kfree_skb_any(skb);

            /* Invalidate the cache */
            bd_rx[i].addr = (uint32_t)sw_dcmode_map_to_phys_addr(NULL, skb->data, DC_DP_PKT_BUF_SIZE_DEFAULT, SW_DCMODE_DATA_FROM_DEVICE);
            bd_rx[i].size = DC_DP_PKT_BUF_SIZE_DEFAULT;
            //bd.addr = (uint32_t)sw_dcmode_map_to_phys_addr(NULL, skb->data, DC_DP_PKT_BUF_SIZE_DEFAULT, SW_DCMODE_DATA_FROM_DEVICE);
            //bd.size = DC_DP_PKT_BUF_SIZE_DEFAULT;
            //bd.flgs = ; // set the OWN bit

            ret = __xfer(SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), &bd_rx[i], 1, SW_DCMODE1_XFER_OUT, 1); // on dev2soc_ret ring
            //ret = __xfer(SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), &bd, 1, SW_DCMODE1_XFER_OUT, 1); // on dev2soc_ret ring

            rx_bdp->dma_addr = (void *)bd_rx[i].addr;
            rx_bdp->skb = skb;
            sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->alloc_rx_bdr_mirror, rx_bdp, 0);

        } // for loop end
        ret = rx_ret;
    }

    if (flags & DC_DP_F_TX_COMPLETE) {
        /*
        1. Read the soc2dev_ret_enq_base (in SoC memory) counter location;
            NOTE: counter value MUST be cumulative.
        2. LOOP:
        2.1. Process soc2dev_ret ring based on the above counter offset value
        2.2. free the buffer pointer from desc. (kfree() will work?)
        3. Update the soc2dev_ret ring read/head pointer
        4. Write the <counter value> to soc2dev_ret_deq_base counter location;
            NOTE: keep correct endianness
         */
        //memset(&bd, 0, sizeof(sw_dcmode_bd_t));
RETRY_TX:
        local_cnt = *((uint32_t *)(locl_cntr->virt));
        //printk("local_cnt = %d\n",local_cnt);
        delta = sw_dcmode_update_counter(&local_cnt, &track->head, &locl_cntr_trck->last_accumulated);
        /*if ( delta > 1 ){
            printk("delta > 1 :local_cnt = %d\n",local_cnt);
            DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"delta:[%d] local counter value :[%d] head : [%d] tail : [%d] ,last accumulated count: [%d]\n"                    ,delta, *((uint32_t *)(locl_cntr->virt)), track->head, track->tail, locl_cntr_trck->last_accumulated);
        }*/
    //    if (delta == 0) {
//        printk(" [DC_DP]: TXOUT delta = %d, local cnt = %d, head = %d, ACA cnt = %d, ppe cnt = %d\n", delta, local_cnt, track->head, readl(0xbb050124), cpu_to_le32(readl(0xbb321484)));
    //    }
        no_of_bd = MIN(delta, MAX_BD_HANDLE);
        /*if (no_of_bd == 0) {
                        DC_DP_ERROR("Nothing to process in TX_OUT handling no_of_bd=0: delta:%d Local counter = %d, head=%d tail:%d last accumulated count: [%d]\n"                        ,delta, *((uint32_t *)(locl_cntr->virt)), track->head, track->tail, locl_cntr_trck->last_accumulated);
                        return 0;
                }*/
        if (no_of_bd == 0) {
            if (retry_cnt < 2) {
                retry_cnt++;
                udelay(100);        //usleep(100);
                //printk("Retrying %d\n",retry_cnt);
                goto RETRY_TX;
            } else {
                //DC_DP_ERROR("Nothing in TX_OUT handling: delta:[%d] Local counter = %d,head=%d tail:[%d] last accumulated count: [%d]\n", delta, *((uint32_t *)(locl_cntr->virt)), track->head, track->tail, locl_cntr_trck->last_accumulated);
                //printk(" [DC_DP]: TXOUT delta = %d, local cnt = %d, head = %d, ACA cnt = %d, PPE_TXOUT = %d PPE_TXIN = %d\n", delta, local_cnt, track->head, readl(0xbb050124), cpu_to_le32(readl(0xbb321484)), cpu_to_le32(readl(0xbb321480)));
                return 0;
            }

        }

        memset(&bd[0], 0, no_of_bd * sizeof(sw_dcmode_bd_t));
        ret = __xfer(SW_DCMODE1_CHAN_SOC2DEV_RET_P(dev_ctx), &bd[0], no_of_bd, SW_DCMODE1_XFER_IN, 0); // on soc2dev_ret ring
        //ret = __xfer(SW_DCMODE1_CHAN_SOC2DEV_RET_P(dev_ctx), &bd, 1, SW_DCMODE1_XFER_IN, 0); // on soc2dev_ret ring

        //for (i=0; i < no_of_bd; i++) {
        for (i=0; i < ret; i++) {
            subif_id.port_id = port_id;
            subif_id.subif = bd[i].sid;

            //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> :bd.addr=[%#x] bd.size=%d\n",__FUNCTION__,bd[i].addr,bd[i].size);
            /*
               1. Search the returned desc buffer address in the allocated pseudo tx ring and find the corresponding skb.
               2. Remove the pseudo tx desc from the allocated pseudo tx ring.
               3. Free the corresponding skb and return the pseudo tx desc to free pseudo tx ring.
             */
            if (bd[i].addr == 0 || bd[i].size == 0) {
                DC_DP_ERROR(" Invalid Buffer descriptor or size for TX_IN ring handling!!!\n");
                ret = -1;
                goto err_out;
            }

            tx_bdp = sw_dcmode_find_entry_in_bdr_mirror(&dev_ctx->alloc_tx_bdr_mirror, (void *)bd[i].addr, DC_DP_F_TX_COMPLETE);
            if (NULL != tx_bdp) {
                skb = tx_bdp->skb;
                sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->free_tx_bdr_mirror, tx_bdp, 0);

                /* TODO: Update the Tx mib */

                dc_dp_free_skb(&subif_id, skb);
            } else {
                DC_DP_ERROR(" TX BD not found [i=%d][ret=%d] !!!\n",i,ret);
                ret = -1;
                goto err_out;
            }
        }
    }

err_out:
    return ret;
}

static int32_t
sw_dcmode1_prepare_rx_channel_buffer(struct sw_dcmode1_dev_info *dev_ctx, sw_dcmode_chan_t *chan, int32_t port_id,
                                     struct dc_dp_res *res,
                                     sw_dcmode1_chan_id_t chan_id)
{
    struct dp_subif subif = {0};
    struct sk_buff *skb;
    int32_t buf_idx;
    struct dc_dp_buf_pool *buflist;
    sw_dcmode_bdr_mirror_t *rx_bdp = NULL;

    /* TODO : Allocate buffer pool (for dev2soc ring) */
    buflist = (struct dc_dp_buf_pool *)kmalloc(res->num_bufs_req * sizeof(struct dc_dp_buf_pool), GFP_KERNEL);
    if (!buflist)
        return -ENOMEM;

    subif.port_id = port_id;
    for (buf_idx = 0; buf_idx < res->num_bufs_req; buf_idx++) {
        skb = dc_dp_alloc_skb(DC_DP_PKT_BUF_SIZE_DEFAULT, &subif, 0);
        if (!skb)
            goto err_out_free_buf;

        /* TODO : need to fill res->[num_bufpools,buflist] */
        buflist[buf_idx].pool = skb->data;
        buflist[buf_idx].phys_pool = (void *)sw_dcmode_map_to_phys_addr(NULL, skb->data, DC_DP_PKT_BUF_SIZE_DEFAULT, SW_DCMODE_DATA_FROM_DEVICE);
        buflist[buf_idx].size = DC_DP_PKT_BUF_SIZE_DEFAULT;

        rx_bdp = sw_dcmode_get_free_bdp_from_bdr_mirror(&dev_ctx->free_rx_bdr_mirror, rx_bdp, 0);
        if (!rx_bdp)
            goto err_out_free_buf;

        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"rx_bdp=%p\n",rx_bdp);
        rx_bdp->dma_addr = buflist[buf_idx].phys_pool;
        rx_bdp->skb = skb;
        sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->alloc_rx_bdr_mirror, rx_bdp, 0);
        //printk("after sw_dcmode_put_bdp_to_bdr_mirro\n");
    }

    /* FIXME : Set initlal buffer to dev2soc_ret ring or dev2soc ring? */

    /* Returned resources */
    res->num_bufpools = res->num_bufs_req;
    res->buflist = buflist;

    return 0;

err_out_free_buf:
    //sw_dcmode1_cleanup_channel_buffer();
    //for (
    if (buflist)
        kfree(buflist);
    return -ENOMEM;
}

void
dump_list(struct list_head *head)
{
    sw_dcmode_bdr_mirror_t *bdp = NULL;
    int32_t count = 0;

    list_for_each_entry(bdp, head, list) {
        count++;
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"bdp->skb = %p and bdp->dma_addr = %p\n",bdp->skb,bdp->dma_addr);
    }
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"count = %d\n",count);
}

static inline int32_t
sw_dcmode_prepare_tx_mirror_ring(struct sw_dcmode1_dev_info *dev_ctx, uint32_t ring_sz, uint32_t flags)
{
    int32_t ret = -1;
    int32_t bd_idx;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Prepare TX mirror ring of size %d\n",ring_sz);
    /* Allocate Tx mirror ring and set it as free */
    dev_ctx->tx_bdr_mirror = sw_dcmode_osal_mem_ddr_pages_alloc(ring_sz * sizeof(sw_dcmode_bdr_mirror_t));
    if ( (NULL == dev_ctx->tx_bdr_mirror) )
        goto err_out;

    memset(dev_ctx->tx_bdr_mirror, 0, (ring_sz * sizeof(sw_dcmode_bdr_mirror_t)));

    INIT_LIST_HEAD(&dev_ctx->alloc_tx_bdr_mirror);
    INIT_LIST_HEAD(&dev_ctx->free_tx_bdr_mirror);
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"before  sw_dcmode_put_bdp_to_bdr_mirror\n");

    //for (bd_idx = 0; bd_idx < ring_sz; bd_idx++) {
    for (bd_idx = 0; bd_idx < ring_sz; bd_idx++) {
        //printk("bd_idx=%d\n",bd_idx);
        sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->free_tx_bdr_mirror, &((sw_dcmode_bdr_mirror_t *)dev_ctx->tx_bdr_mirror)[bd_idx], 0);
    }
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"after  sw_dcmode_put_bdp_to_bdr_mirror\n");
    //dump_list(&dev_ctx->free_tx_bdr_mirror);
    //dump_list(&dev_ctx->alloc_tx_bdr_mirror);

    ret = 0;

err_out:
    return ret;
}
static inline int32_t
sw_dcmode_prepare_rx_mirror_ring(struct sw_dcmode1_dev_info *dev_ctx, uint32_t ring_sz, uint32_t flags)
{
    int32_t ret = -1;
    int32_t bd_idx;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Prepare RX mirror ring of size %d\n",ring_sz);
    /* Allocate Tx mirror ring and set it as free */
    dev_ctx->rx_bdr_mirror = sw_dcmode_osal_mem_ddr_pages_alloc(ring_sz * sizeof(sw_dcmode_bdr_mirror_t));
    if ( (NULL == dev_ctx->rx_bdr_mirror) )
        goto err_out;

    memset(dev_ctx->rx_bdr_mirror, 0, (ring_sz * sizeof(sw_dcmode_bdr_mirror_t)));

    INIT_LIST_HEAD(&dev_ctx->alloc_rx_bdr_mirror);
    INIT_LIST_HEAD(&dev_ctx->free_rx_bdr_mirror);
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"before  sw_dcmode_put_bdp_to_bdr_mirror\n");

    //for (bd_idx = 0; bd_idx < ring_sz; bd_idx++) {
    for (bd_idx = 0; bd_idx < ring_sz; bd_idx++) {
        //printk("bd_idx=%d\n",bd_idx);
        sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->free_rx_bdr_mirror, &((sw_dcmode_bdr_mirror_t *)dev_ctx->rx_bdr_mirror)[bd_idx], 0);
    }
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"after  sw_dcmode_put_bdp_to_bdr_mirror\n");
    //dump_list(&dev_ctx->free_tx_bdr_mirror);
    //dump_list(&dev_ctx->alloc_tx_bdr_mirror);

    ret = 0;

err_out:
    return ret;
}

static inline void
sw_dcmode_cleanup_tx_mirror_ring(struct sw_dcmode1_dev_info *dev_ctx, uint32_t ring_sz, uint32_t flags)
{
    /* TODO : Free Tx mirror ring */
    sw_dcmode_bdr_mirror_t *bdp = NULL;
    int32_t count = 0;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Cleanup TX mirror ring of size %d\n",ring_sz);

    list_for_each_entry(bdp, &dev_ctx->alloc_tx_bdr_mirror, list) {
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"bdp->skb = %p and bdp->dma_addr = %p\n",bdp->skb,bdp->dma_addr);
        dev_kfree_skb(bdp->skb);
        count++;
    }
    /* Free Tx mirror ring and set it as free */
    sw_dcmode_osal_mem_ddr_pages_free(dev_ctx->tx_bdr_mirror, (ring_sz * sizeof(sw_dcmode_bdr_mirror_t)));
    printk(" Tx Mirror Ring clean done !!");
}
static inline void
sw_dcmode_cleanup_rx_mirror_ring(struct sw_dcmode1_dev_info *dev_ctx, uint32_t ring_sz, uint32_t flags)
{
    /* TODO : Free Rx mirror ring */
    sw_dcmode_bdr_mirror_t *bdp = NULL;
    int32_t count = 0;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Cleanup RX mirror ring of size %d\n",ring_sz);

    list_for_each_entry(bdp, &dev_ctx->alloc_rx_bdr_mirror, list) {
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"(dev=%s) bdp->skb = %p and bdp->dma_addr = %p\n",
            (bdp->skb->dev?bdp->skb->dev->name:"NULL"), bdp->skb, bdp->dma_addr);
        dev_kfree_skb(bdp->skb);
        count++;
    }
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Cleaned up RX mirror ring of size %d\n",count);
    /* Free Tx mirror ring and set it as free */
    sw_dcmode_osal_mem_ddr_pages_free(dev_ctx->rx_bdr_mirror, (ring_sz * sizeof(sw_dcmode_bdr_mirror_t)));
}

static int32_t
sw_dcmode1_prepare_tx_channel_buffer(struct sw_dcmode1_dev_info *dev_ctx, sw_dcmode_chan_t *chan, int32_t port_id,
                                     struct dc_dp_res *res, sw_dcmode1_chan_id_t chan_id)
{
    struct dp_subif subif = {0};
    struct sk_buff *skb;
    int32_t buf_idx;
    struct dc_dp_buf_pool *buflist;
    sw_dcmode_bdr_mirror_t *tx_bdp = NULL;
    //int32_t *bdp_ptr = NULL;

    /* TODO : Allocate buffer pool (for dev2soc ring) */
    buflist = (struct dc_dp_buf_pool *)kmalloc(res->tx_num_bufs_req * sizeof(struct dc_dp_buf_pool), GFP_KERNEL);
    if ( (NULL == buflist) )
        return -1;

    subif.port_id = port_id;
    for (buf_idx = 0; buf_idx < res->tx_num_bufs_req; buf_idx++) {
        skb = dc_dp_alloc_skb(DC_DP_PKT_BUF_SIZE_DEFAULT, &subif, 0);
        if (!skb)
            break;

        /* TODO : need to fill res->[num_bufpools,buflist] */
        buflist[buf_idx].pool = skb->data;
        buflist[buf_idx].phys_pool = (void *)sw_dcmode_map_to_phys_addr(NULL, skb->data, DC_DP_PKT_BUF_SIZE_DEFAULT, SW_DCMODE_DATA_FROM_DEVICE);
        buflist[buf_idx].size = DC_DP_PKT_BUF_SIZE_DEFAULT;

        //tx_bdp = NULL;
        tx_bdp = sw_dcmode_get_free_bdp_from_bdr_mirror(&dev_ctx->free_tx_bdr_mirror, tx_bdp, 0);
        if (!tx_bdp)
            break;

        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG,"tx_bdp=%p\n",tx_bdp);
        tx_bdp->dma_addr = buflist[buf_idx].phys_pool;
        tx_bdp->skb = skb;
        sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->alloc_tx_bdr_mirror, tx_bdp, 0);
        //printk("after sw_dcmode_put_bdp_to_bdr_mirro\n");
    }

    //dump_list(&dev_ctx->free_tx_bdr_mirror);
    //dump_list(&dev_ctx->alloc_tx_bdr_mirror);
    /* FIXME : Set initlal buffer to soc2dev ring? */

    /* Returned resources */
    res->tx_num_bufpools = buf_idx;
    res->tx_buflist = buflist;
    res->tx_desc_start_idx = 0;
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "res->tx_num_bufpools=%d\n",res->tx_num_bufpools);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "res->tx_buflist=%p",res->tx_buflist);

    return 0;
}

#if 0 // FIXME
static void
sw_dcmode1_cleanup_channel_buffer(sw_dcmode_chan_t *chan, int32_t port_id,
                                  struct dc_dp_res *res,
                                  sw_dcmode1_chan_id_t chan_id)
{
    int32_t i;
    uint32_t order;

    /* De-allocate Rx buffer pool */
    if (chan_id == SW_DCMODE1_CHAN_DEV2SOC_RET) {
        if (res->buflist) {
            for (i = 0; i < res->num_bufpools; i++) {
                DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "De-allocating pool %d (0x%p) of size %d bytes.\n",
                            (i + 1), res->buflist[i].pool, res->buflist[i].size);
                if (res->buflist[i].pool) {
                    order = get_order(res->buflist[i].size);
                    free_pages((unsigned long)res->buflist[i].pool, order);
                }
            }

            DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "De-allocating %d buffer lists.\n", res->num_bufpools);
            kfree(res->buflist);
        }
    }
    /* De-allocate Tx buffer pool */
    if (chan_id == SW_DCMODE1_CHAN_SOC2DEV) {
        if (res->tx_buflist) {
            for (i = 0; i < res->tx_num_bufpools; i++) {
                DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "De-allocating pool %d (0x%p) of size %d bytes.\n",
                            (i + 1), res->tx_buflist[i].pool, res->tx_buflist[i].size);
                if (res->tx_buflist[i].pool) {
                    order = get_order(res->tx_buflist[i].size);
                    free_pages((unsigned long)res->tx_buflist[i].pool, order);
                }
            }

            DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "De-allocating %d buffer lists.\n", res->tx_num_bufpools);
            kfree(res->tx_buflist);
        }
    }
}
#endif

static void sw_dcmode1_ch_init(sw_dcmode_chan_t * ch, unsigned int size)
{
    memset(ch, 0, sizeof(sw_dcmode_chan_t));
    ch->trck.head = 0;
    ch->trck.tail = 0;
    ch->trck.size = size;
    ch->trck.mask = size - 1;
}

static void
update_ring_resources(sw_dcmode_chan_t *chan, struct dc_dp_ring *ring_res, void *ring_v_base, void *ring_p_base, sw_dcmode1_chan_id_t chan_id)
{
    /* Update local ring resources */
    chan->data.virt = ring_v_base;
    chan->data.phys = (unsigned int)ring_p_base;
    chan->data.size = ring_res->size;
    chan->bufs = chan->data.virt;
    chan->data.desc_dwsz = 4;

    sw_dcmode1_ch_init(chan,ring_res->size);

    /*if (chan_id == SW_DCMODE1_CHAN_DEV2SOC)
      {
      chan->trck.head = 1;

      }*/
    /* Update returned ring resources */
    ring_res->base = ring_v_base;
    ring_res->phys_base = ring_p_base;
    ring_res->ring = (void *)chan;
    ring_res->desc_dwsz = chan->data.desc_dwsz;
}

static void
update_cntr_resources(sw_dcmode_chan_t *chan, struct dc_dp_dccntr *cntr_res, void *cntr_v_base, void *cntr_p_base, uint32_t mode, sw_dcmode1_chan_id_t chan_id)
{
    /* Update local counter resources */
    chan->lcl_cntr.virt = cntr_v_base;
    chan->lcl_cntr.phys = (unsigned int)cntr_p_base;
    chan->lcl_cntr.size = 4;
    chan->lcl_cntr.mode = mode;

    if (chan_id == SW_DCMODE1_CHAN_SOC2DEV) {
        chan->rm_cntr.virt = cntr_res->soc2dev_enq_base;
        chan->rm_cntr.phys = (unsigned int)cntr_res->soc2dev_enq_phys_base;
        chan->rm_cntr.size = cntr_res->soc2dev_enq_dccntr_len;
    } else if (chan_id == SW_DCMODE1_CHAN_SOC2DEV_RET) {
        chan->rm_cntr.virt = cntr_res->soc2dev_ret_deq_base;
        chan->rm_cntr.phys = (unsigned int)cntr_res->soc2dev_ret_deq_phys_base;
        chan->rm_cntr.size = cntr_res->soc2dev_ret_deq_dccntr_len;
    } else if (chan_id == SW_DCMODE1_CHAN_DEV2SOC) {
        chan->rm_cntr.virt = cntr_res->dev2soc_deq_base;
        chan->rm_cntr.phys = (unsigned int)cntr_res->dev2soc_deq_phys_base;
        chan->rm_cntr.size = cntr_res->dev2soc_deq_dccntr_len;
    } else if (chan_id == SW_DCMODE1_CHAN_DEV2SOC_RET) {
        chan->rm_cntr.virt = cntr_res->dev2soc_ret_enq_base;
        chan->rm_cntr.phys = (unsigned int)cntr_res->dev2soc_ret_enq_phys_base;
        chan->rm_cntr.size = cntr_res->soc2dev_ret_enq_dccntr_len;
    }
    chan->rm_cntr.mode = cntr_res->soc_write_dccntr_mode;

    /* Update returned counter resources */
    if (chan_id == SW_DCMODE1_CHAN_SOC2DEV) {
        cntr_res->soc2dev_deq_base = cntr_v_base;
        cntr_res->soc2dev_deq_phys_base = cntr_p_base;
        cntr_res->soc2dev_deq_dccntr_len = 4;
        cntr_res->dev_write_dccntr_mode = mode;
    } else if (chan_id == SW_DCMODE1_CHAN_SOC2DEV_RET) {
        cntr_res->soc2dev_ret_enq_base = cntr_v_base;
        cntr_res->soc2dev_ret_enq_phys_base = cntr_p_base;
        cntr_res->soc2dev_ret_enq_dccntr_len = 4;
        cntr_res->dev_write_dccntr_mode = mode;
    } else if (chan_id == SW_DCMODE1_CHAN_DEV2SOC) {
        cntr_res->dev2soc_enq_base = cntr_v_base;
        cntr_res->dev2soc_enq_phys_base = cntr_p_base;
        cntr_res->dev2soc_enq_dccntr_len = 4;
        cntr_res->dev_write_dccntr_mode = mode;
    } else if (chan_id == SW_DCMODE1_CHAN_DEV2SOC_RET) {
        cntr_res->dev2soc_ret_deq_base = cntr_v_base;
        cntr_res->dev2soc_ret_deq_phys_base = cntr_p_base;
        cntr_res->dev2soc_ret_deq_dccntr_len = 4;
        cntr_res->dev_write_dccntr_mode = mode;
    }
}

static int32_t sw_dcmode1_prepare_channel(sw_dcmode_chan_t *chan,
                                          struct dc_dp_ring *ring_res,
                                          struct dc_dp_dccntr *cntr_res,
                                          sw_dcmode1_chan_id_t chan_id)
{
    int32_t ret = 0;
    uint32_t ring_size;
    uint32_t cntr_size;
    void *ring_v_base;
    void *ring_p_base = NULL;
    void *cntr_v_base;
    void *cntr_p_base = NULL;
    uint32_t mode = 0;
    dma_addr_t cntr_dma_addr, ring_dma_addr;

    memset(chan, 0, sizeof(sw_dcmode_chan_t));

    /* Allocate ring */
    ring_size = DC_DP_RING_SIZE(ring_res) * sizeof(sw_dcmode_bd_t);
    printk("the size of ring:%d",ring_size);
    ring_v_base = sw_dcmode_mem_dma_alloc(ring_size, &ring_dma_addr, 0);
    if (!ring_v_base) {
        ret = -ENOMEM;
        goto out;
    }
    printk("ring_v_base:%p",ring_v_base);
    memset(ring_v_base, 0, ring_size);

    //ring_p_base = (void *)sw_dcmode_map_to_phys_addr(NULL, ring_v_base, ring_size, SW_DCMODE_DATA_FROM_DEVICE);
    ring_p_base = (void *)ring_dma_addr;
    printk("ring_p_base:%p",ring_p_base);

    /* Update ring resources */
    update_ring_resources(chan, ring_res, ring_v_base, ring_p_base, chan_id);

    /* Allocate SoC counter memory (soc2dev_deq_base, soc2dev_ret_enq_base) */
    cntr_size = sizeof(sw_dcmode_cntr_type_t);
    printk("cntr_size:%d",cntr_size);

    cntr_v_base = sw_dcmode_mem_dma_alloc(cntr_size, &cntr_dma_addr, 0);
    if (!cntr_v_base) {
        sw_dcmode_mem_dma_free(ring_v_base, ring_dma_addr, ring_size);
        ret = -ENOMEM;
        goto out;
    }
    printk("cntr_v_base:%p\n",cntr_v_base);
    memset(cntr_v_base, 0, cntr_size);

    //cntr_p_base = (void *)sw_dcmode_map_to_phys_addr(NULL, cntr_v_base, cntr_size, SW_DCMODE_DATA_FROM_DEVICE);
    cntr_p_base = (void *)cntr_dma_addr;
    printk("cntr_p_base:%p",cntr_p_base);

    mode = DC_DP_F_DCCNTR_MODE_CUMULATIVE;
#ifdef CONFIG_LITTLE_ENDIAN
    mode |= DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN;
#else
    mode |= DC_DP_F_DCCNTR_MODE_BIG_ENDIAN;
#endif

    /* Update counter resources */
    update_cntr_resources(chan, cntr_res, cntr_v_base, cntr_p_base, mode, chan_id);

out:
    return ret;
}

static void
sw_dcmode1_cleanup_channel(sw_dcmode_chan_t *chan,
                           struct dc_dp_ring *ring_res,
                           struct dc_dp_dccntr *cntr_res,
                           sw_dcmode1_chan_id_t chan_id)
{
    /* TODO */
    //free local counter associated to this channel
    sw_dcmode_mem_dma_free(chan->lcl_cntr.virt, (dma_addr_t)chan->lcl_cntr.phys, sizeof(sw_dcmode_cntr_type_t));
    //free ring
    sw_dcmode_mem_dma_free(chan->data.virt, (dma_addr_t)chan->data.phys, (chan->data.size * sizeof(sw_dcmode_bd_t)));
    memset(chan, 0, sizeof(sw_dcmode_chan_t));
}

static void
get_capability(uint32_t *cap)
{
    //*cap = DC_DP_F_HOST_CAP_SG;
}

static int32_t
sw_dcmode1_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
    int32_t ret = -1;

    if (cap) {
        cap->swpath.support = 1;
        cap->swpath.sw_dcmode = DC_DP_MODE_TYPE_1;

        cap->swpath.sw_cmode.soc2dev_write = (DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_CUMULATIVE);
        cap->swpath.sw_cmode.soc2dev_write |= (DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN);

        cap->swpath.sw_cmode.dev2soc_write = DC_DP_F_DCCNTR_MODE_CUMULATIVE;
        cap->swpath.sw_cmode.dev2soc_write |= (DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN);

        get_capability(&cap->swpath.sw_cap);

        ret = 0;
    }

    return ret;
}

static int32_t
sw_dcmode1_register_dev(void *ctx, struct module *owner, uint32_t port_id,
                        struct net_device *dev, struct dc_dp_cb *datapathcb,
                        struct dc_dp_res *resources, struct dc_dp_dev *devspec,
                        uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct sw_dcmode1_dev_info *dev_ctx;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "dev_ctx=%p, owner=%p, port_id=%u, dev=%p, datapathcb=%p, \
            resources=%p, dev_spec=%p, flags=0x%08X\n",
            ctx, owner, port_id, dev, datapathcb, resources, devspec, flags);

    if ( (flags & DC_DP_F_DEREGISTER) ) {
        if (NULL == ctx)
            goto err_out;

        dev_ctx = (struct sw_dcmode1_dev_info *) ctx;

        /* De-register SW DC Mode1 device from DC Common layer */
        dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, DC_DP_DCMODE_DEV_DEREGISTER);

        DC_DP_LOCK(&g_sw_dcmode1_dev_lock);

        if ( (dev_ctx->reg_dev_flags & DC_DP_F_ALLOC_SW_RX_RING) ) {
            /* TODO */
            /*sw_dcmode1_cleanup_channel_buffer(SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), port_id,
                    resources, SW_DCMODE1_CHAN_DEV2SOC_RET);*/
            printk("Rx mirror ring clean up start !!\n");
            sw_dcmode_cleanup_rx_mirror_ring(dev_ctx, DC_DP_RING_SIZE(DC_DP_DEV2SOC_RING_RES_P(resources)), 0);
            printk("Rx mirror ring clean up done [1133]!!\n");
            sw_dcmode1_cleanup_channel(SW_DCMODE1_CHAN_DEV2SOC_P(dev_ctx), DC_DP_DEV2SOC_RING_RES_P(resources),
                    DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_DEV2SOC);
            printk("DEV2SOC cleanup channel done  [1136]!!\n");
            sw_dcmode1_cleanup_channel(SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), DC_DP_DEV2SOC_RET_RING_RES_P(resources),
                    DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_DEV2SOC_RET);
            printk("DEV2SOC_Ret cleanup channel done [1139] !!\n");
        }

        if ( (dev_ctx->reg_dev_flags & DC_DP_F_ALLOC_SW_TX_RING) ) {
            /* TODO */
            /*sw_dcmode1_cleanup_channel_buffer(SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), port_id,
                    resources, SW_DCMODE1_CHAN_SOC2DEV);*/
            printk("Tx mirror ring clean up start [1148]!!\n");
            sw_dcmode_cleanup_tx_mirror_ring(dev_ctx, DC_DP_RING_SIZE(DC_DP_SOC2DEV_RING_RES_P(resources)), 0);
            printk("Tx mirror ring clean up done [1150]!!\n");
            sw_dcmode1_cleanup_channel(SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), DC_DP_SOC2DEV_RING_RES_P(resources),
                    DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_SOC2DEV);
            printk("SOC2DEV cleanup channel done [1153]!!\n");
            sw_dcmode1_cleanup_channel(SW_DCMODE1_CHAN_SOC2DEV_RET_P(dev_ctx), DC_DP_SOC2DEV_RET_RING_RES_P(resources),
                    DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_SOC2DEV_RET);
            printk("SOC2DEV_Ret cleanup channel done [1156]!!\n");
        }

        sw_dcmode1_free_device(dev_ctx, port_id);

        DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);

        return DC_DP_SUCCESS;
    }
#if 0
    if (NULL != ctx) {
        DC_DP_ERROR("failed to register device for the port_id %d!!!\n", port_id);
        goto err_out;
    }
    /* Find a free device index */
    for (dev_idx = 0; dev_idx < SW_DCMODE1_MAX_DEV_NUM; dev_idx++) {
        if (g_sw_dcmode1_dev_info[dev_idx].status != SW_DCMODE1_SUBIF_USED) {
            break;
        }
    }

    if (dev_idx >= SW_DCMODE1_MAX_DEV_NUM) {
        DC_DP_ERROR("failed to register dev as it reaches maximum directconnect device limit - %d!!!\n", SW_DCMODE1_MAX_DEV_NUM);
        goto err_out;
    }
    dev_ctx = &g_sw_dcmode1_dev_info[dev_idx];
#endif
    /* Reset DC Mode1 device structure */
    //    memset(dev_ctx, 0, sizeof(struct sw_dcmode1_dev_info_t));

    /* Validate input arguments */
    if (resources->num_dccntr != 1 && !resources->dccntr) {
        DC_DP_ERROR("failed to register device for the port_id %d!!!\n", port_id);
        goto err_out;
    }

    if ( 0 == (flags & (DC_DP_F_ALLOC_SW_TX_RING | DC_DP_F_ALLOC_SW_RX_RING)) ) {
        DC_DP_ERROR("failed to register device for the port_id %d!!!\n", port_id);
        goto err_out;
    }

    DC_DP_LOCK(&g_sw_dcmode1_dev_lock);

    dev_ctx = sw_dcmode1_alloc_device(port_id);
    if (NULL == dev_ctx) {
        DC_DP_ERROR("failed to register device for the port_id %d!!!\n", port_id);
        goto err_unlock_out;
    }

    if ( (flags & DC_DP_F_ALLOC_SW_RX_RING) ) {

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Preparing SW Rx ring.\n");
        sw_dcmode1_prepare_channel(SW_DCMODE1_CHAN_DEV2SOC_P(dev_ctx), DC_DP_DEV2SOC_RING_RES_P(resources),
                DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_DEV2SOC);
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Preparing SW Rx ring!!!!.\n");
        sw_dcmode1_prepare_channel(SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), DC_DP_DEV2SOC_RET_RING_RES_P(resources),
                DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_DEV2SOC_RET);

        if (sw_dcmode_prepare_rx_mirror_ring(dev_ctx, DC_DP_RING_SIZE(DC_DP_DEV2SOC_RET_RING_RES_P(resources)), 0) == -1 )
        {
            DC_DP_ERROR("failed to prepare RX mirror ring!!!\n");
            ret = -1;
            goto err_unlock_out;

        }
        sw_dcmode1_prepare_rx_channel_buffer(dev_ctx, SW_DCMODE1_CHAN_DEV2SOC_RET_P(dev_ctx), port_id,
                resources, SW_DCMODE1_CHAN_DEV2SOC_RET);

        /* Save */

        /* Returned devspec */
        devspec->dc_rx_ring_used = DC_DP_RING_SW_MODE1;
    }

    if ( (flags & DC_DP_F_ALLOC_SW_TX_RING) ) {
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Preparing SW Tx ring.\n");
        sw_dcmode1_prepare_channel(SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), DC_DP_SOC2DEV_RING_RES_P(resources),
                DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_SOC2DEV);
        sw_dcmode1_prepare_channel(SW_DCMODE1_CHAN_SOC2DEV_RET_P(dev_ctx), DC_DP_SOC2DEV_RET_RING_RES_P(resources),
                DC_DP_CNTR_RES0_P(resources), SW_DCMODE1_CHAN_SOC2DEV_RET);

        if (sw_dcmode_prepare_tx_mirror_ring(dev_ctx, DC_DP_RING_SIZE(DC_DP_SOC2DEV_RING_RES_P(resources)), 0) == -1 )
        {
            DC_DP_ERROR("failed to prepare TX mirror ring!!!\n");
            ret = -1;
            goto err_unlock_out;

        }

        sw_dcmode1_prepare_tx_channel_buffer(dev_ctx, SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), port_id,
                resources, SW_DCMODE1_CHAN_SOC2DEV);
        /* Save */

        /* Returned devspec */
        devspec->dc_tx_ring_used = DC_DP_RING_SW_MODE1;
    }

    /* Save */
    dev_ctx->reg_dev_flags = flags;

    /* Returned devspec */
    devspec->dc_accel_used = DC_DP_ACCEL_NO_OFFLOAD;
    get_capability(&devspec->dc_cap);

    /* Register DC Mode1 device to DC common layer */
    ret = dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, 0);
    if (ret) {
        DC_DP_ERROR("failed to register device to DC common layer!!!\n");
        goto err_unlock_out;
    }

    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);

    return DC_DP_SUCCESS;

err_unlock_out:
    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned %d.\n", ret);

err_out:
    return ret;
}

static int32_t
sw_dcmode1_register_subif(void *ctx, struct module *owner,
        struct net_device *dev, const uint8_t *subif_name, struct dp_subif *subif_id,
        uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct sw_dcmode1_dev_info *dev_ctx = (struct sw_dcmode1_dev_info *)ctx;
    struct sw_dcmode1_subif_info *subif_ctx = NULL;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "dev_ctx=%p, owner=%p, dev=%p, subif_id=%p, flags=0x%08X\n",
            dev_ctx, owner, dev, subif_id, flags);

    /* Validate input arguments */
    if ( (NULL == dev_ctx) || (NULL== owner) || (NULL == dev) || (NULL == subif_id) ) {
        DC_DP_ERROR("failed to register/de-register subif to/from SW DCMode1!!!\n");
        goto err_out;
    }

    /* De-register */
    if ( (flags & DC_DP_F_DEREGISTER) ) {

        DC_DP_LOCK(&g_sw_dcmode1_dev_lock);

        sw_dcmode1_free_subif(dev_ctx, subif_id->subif);

        dev_ctx->num_subif--;
        /* For the last subif */
        if ( (0 == dev_ctx->num_subif) ) {
            /* TODO */

            dev_ctx->state = SW_DCMODE1_ST_DEV_REGISTERED;
        }

        DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d.\n", DC_DP_SUCCESS);
        return DC_DP_SUCCESS;
    }

    DC_DP_LOCK(&g_sw_dcmode1_dev_lock);

    /* Allocate a new subif */
    subif_ctx = sw_dcmode1_alloc_subif(dev_ctx, subif_id);
    if (NULL == subif_ctx)
        goto err_unlock_out;

    //printk("####### Subif returned : [%d]\n",subif_ctx->subif);
    /* Save */
    subif_ctx->reg_subif_flags = flags;
    //subif_ctx->subif = subif_id->subif;
    subif_ctx->netif = dev;
    strncpy(subif_ctx->device_name, dev->name, (IFNAMSIZ - 1));

    dev_ctx->num_subif++;

    /* For the first subif*/
    if ( (1 == dev_ctx->num_subif) ) {
        /* TODO */

        dev_ctx->state = SW_DCMODE1_ST_SUBIF_REGISTERED;
    }

    /* Returned subif */
    subif_id->subif = subif_ctx->subif;

    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);
    //printk("SWDC :SUBIF_ID->subif : %p\n",subif_id->subif);
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "SWDC: Success, returned %d. Subif_Addr: %p. Subif_Id: %d\n", DC_DP_SUCCESS, subif_id, subif_id->subif);
    return DC_DP_SUCCESS;

err_unlock_out:
    DC_DP_UNLOCK(&g_sw_dcmode1_dev_lock);

err_out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned %d.\n", ret);
    return ret;
}

#if 0
static void __print_channel_counters(sw_dcmode_chan_t * ch, const char * msg)
{
    printk("%s:| %10d | %10d | %10d\n",
               msg,
               (ch->trck.head),
               (ch->trck.tail),
               (*SW_DCMODE1_REMOTE_VIRT(ch)));
}
#endif

//static inline int __xfer_wait(sw_dcmode_chan_t * ch, unsigned out, unsigned n, unsigned w, unsigned * index)
static inline int __xfer_wait(sw_dcmode_chan_t * ch, uint32_t out, uint32_t n, uint32_t w, uint32_t * index)
{
    // Query room available for transfer
    //int avl = (__ctrl_pull(ch, out), sw_dcmode_ring_count(ch->trck, out));
    //ch->trck = kzalloc(sizeof(sw_dcmode_ring_t),GFP_KERNEL);

    int avl;
    avl = sw_dcmode_ring_count(&ch->trck, out);
    //printk("%s,avl=:%d\n",__FUNCTION__,avl);

    // Still no room and we are in a sync mode -> wait, invalidate and retry
    if (w && avl) {
        // Fetch latest control data
        //sw_dcmode_ring_index_inc(&ch->trck, SW_DCMODE1_XFER_IN, *SW_DCMODE1_LOCAL_VIRT(ch));
        sw_dcmode_ring_index_inc(&ch->trck, SW_DCMODE1_XFER_IN, n);

        avl = sw_dcmode_ring_count(&ch->trck, out);
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> <IN> avl=:%d\n",__FUNCTION__,avl);
    } else if (!w && !avl) {
        // Fetch latest control data
        //sw_dcmode_ring_index_inc(&ch->trck, SW_DCMODE1_XFER_OUT, *SW_DCMODE1_LOCAL_VIRT(ch));
        sw_dcmode_ring_index_inc(&ch->trck, SW_DCMODE1_XFER_OUT, n);

        avl = sw_dcmode_ring_count(&ch->trck, out);
        //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> <OUT> avl=:%d\n",__FUNCTION__,avl);
    }

    // Raw index
    *index = sw_dcmode_ring_index(&ch->trck, out);
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> :<%d> index:%d\n",__FUNCTION__,__LINE__,*index);

    // Debug output
    //__CH_PRINT(ch, __print_channel_counters);

    return MIN(avl, n);
}

//------------------------------------------------------------------------------
// Transfer completion handler
//------------------------------------------------------------------------------
static inline void __xfer_wake(sw_dcmode_chan_t * ch, uint32_t out, uint32_t n, uint32_t w)
{
    // Update ring pointer
    sw_dcmode_ring_index_inc(&ch->trck, out, n);

    // Update remote copy
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> SW_DCMODE1_REMOTE_VIRT:%p value=%d\n",__FUNCTION__, SW_DCMODE1_REMOTE_VIRT(ch),*SW_DCMODE1_REMOTE_VIRT(ch));
    //printk("Actual value: %d Convert value: %d\n",sw_dcmode_ring_index_raw(&ch->trck, out), cpu_to_le32(sw_dcmode_ring_index_raw(&ch->trck, out)));
    //printk("Actual value: %d CPU to BE32  Convert value: %d\n",sw_dcmode_ring_index_raw(&ch->trck, out), cpu_to_be32(sw_dcmode_ring_index_raw(&ch->trck, out)));

    //TODO for cumulatuve/incremental handling
    //*SW_DCMODE1_REMOTE_VIRT(ch) = cpu_to_le32(sw_dcmode_ring_index_raw(&ch->trck, out));
    *SW_DCMODE1_REMOTE_VIRT(ch) = cpu_to_le32(n); // right now incremental
    //SW_DCMODE1_SOC_REMOTE_CNTR_LAST_ACCUM(chan, n) ((chan)->lcl_cntr_trck.last_accumulated);
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> Update Remote counter :%p value=%d\n",__FUNCTION__, SW_DCMODE1_REMOTE_VIRT(ch),*SW_DCMODE1_REMOTE_VIRT(ch));

    // Debug output
    //__CH_PRINT(ch, __print_channel_counters);
}

void dump_bd(sw_dcmode_bd_t *src_main)
{
    printk("DW0:tid=%d  sid=%d\n",src_main->tid,src_main->sid);
    printk("DW1:misc=%d\n",src_main->misc);
    printk("DW2:addr=%#x\n",src_main->addr);
    printk("DW3:flgs=%d  rsvd=%d  size=%d\n",src_main->flgs,src_main->rsvd,src_main->size);
}

//------------------------------------------------------------------------------
// Generic descriptor to / from page transfer routine
//------------------------------------------------------------------------------
int32_t __xfer(sw_dcmode_chan_t * ch, sw_dcmode_bd_t * d, uint32_t n, int xt, uint32_t w)
{
    unsigned int nb_cp, size, i = 0, size_main, size_xtra, out = (xt == SW_DCMODE1_XFER_OUT);
    sw_dcmode_bd_t *src_main, *dst_main, *src_xtra, *dst_xtra;

    nb_cp = __xfer_wait(ch, out, n, w, &i);

    // Size
    size = ch->trck.size;
    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG," <%s> nb_cp:%d i=%d size=%d\n",__FUNCTION__,nb_cp,i,size);

    // Source and destination pointers
    src_main = out ? d : &ch->bufs[i];
    dst_main = out ? &ch->bufs[i] : d;
    src_xtra = out ? &d[size-i] : ch->bufs;
    dst_xtra = out ? ch->bufs : &d[size-i];

    // Sizes
    size_main = sizeof(*d) * ((i + nb_cp < size) ? nb_cp : (size - i));
    size_xtra = sizeof(*d) * ((i + nb_cp < size) ? 0     : nb_cp - (size - i));

    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s> :dst_main=[%p] src_main=[%p] size_main=[%d] size_extra:[%d]\n",__FUNCTION__,dst_main,src_main,size_main, size_xtra);
#if 0
    // FIXME: use non-cachable memory for the desc
    if (!out)
        sw_dcmode_map_to_phys_addr(NULL, src_main, size_main, SW_DCMODE_DATA_FROM_DEVICE);
#endif
    // Otherwise fallback to a generic copy
    {
        memcpy(dst_main, src_main, size_main);
        memcpy(dst_xtra, src_xtra, size_xtra);
    }
#if 0
    // FIXME: use non-cachable memory for the desc
    if (out)
        sw_dcmode_map_to_phys_addr(NULL, dst_main, size_main, SW_DCMODE_DATA_TO_DEVICE);
#endif

    wmb();

    // Notify other side
    __xfer_wake(ch, out, nb_cp, w);

    //out:
    return nb_cp;
}
EXPORT_SYMBOL(__xfer);

static int32_t
sw_dcmode1_xmit(void *ctx, struct net_device *rx_if, struct dp_subif *rx_subif, struct dp_subif *tx_subif,
                struct sk_buff *skb, int32_t len, uint32_t flags)
{
    int32_t ret = -1;
    struct sw_dcmode1_dev_info *dev_ctx = (struct sw_dcmode1_dev_info *)ctx;
    sw_dcmode_bd_t bd = {0};
    sw_dcmode_bdr_mirror_t *tx_bdp = NULL;

    /* Validate input arguments */
    if ( (NULL == skb) ) {
        goto err_out;
    }

    if ( (NULL == dev_ctx) || (NULL == tx_subif) ) {
        goto drop;
    }

    /*
    1. Read the soc2dev_deq_base (in SoC memory) counter location;
        NOTE: counter value MUST be cumulative.
    2. Update soc2dev ring read/tail pointer accordingly
    */

    /*
    1. Enqueue the skb->data to desc of soc2dev ring (write/head pointer)
    2. Update soc2dev ring write/head pointer
    3. Write (<1> if incremental, otherwise cumulative counter) to remote soc2dev_enq_base counter location (endianness??).
        NOTE: Writing in every xmit over PCIe is not efficient???
    4. skb->head = NULL;
    5. dev_kfree_skb_any(skb);
    */
    /*DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "dev_ctx=%p, rx_if=%p, rx_subif=%p, tx_subif=%p, skb=%p, skb->data=%p"
            "len=%d, flags=0x%08X\n",
            dev_ctx, rx_if, rx_subif, tx_subif, skb, skb->data, len, flags);*/

    tx_bdp = sw_dcmode_get_free_bdp_from_bdr_mirror(&dev_ctx->free_tx_bdr_mirror, tx_bdp, 0);
    if ( (NULL == tx_bdp) ) {
        DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_TX, "Tx mirror ring is full!!!\n");
        goto drop;
    }

    /* TODO: Handle Scattered skb, if any */
    /* NOTE : Peripheral may or may not support non-linear buffer in xmit */

    /* Fill the descr from skb */
    bd.addr = (uint32_t)sw_dcmode_map_to_phys_addr(NULL, skb->data, skb->len, SW_DCMODE_DATA_TO_DEVICE);
    bd.size = skb->len;
    /* TODO : update other fileds */
    bd.sid = tx_subif->subif;
    bd.flgs = 0xE0;

    //DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "<%s>:bd.addr=%#x bd.size=%d bd.sid=%d\n",__FUNCTION__,bd.addr,bd.size,bd.sid);

    /* TODO: Be carefull about cloned skb */
    //ret = __xfer(SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), &bd, 1, SW_DCMODE1_XFER_OUT, 0); // on soc2dev ring
    ret = __xfer(SW_DCMODE1_CHAN_SOC2DEV_P(dev_ctx), &bd, 1, SW_DCMODE1_XFER_OUT, 1); // on soc2dev ring
    if (!ret)
        goto put_to_free_mirror_ring;

    /*
       Allocate a pseudo tx desc from pseudo tx ring and save skb.
        NOTE: Corresponding skb will be freed in Tx complete event.
     */
    tx_bdp->dma_addr = (void *)bd.addr;
    tx_bdp->skb = skb;
    sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->alloc_tx_bdr_mirror, tx_bdp, 0);
    //dump_list(&dev_ctx->free_tx_bdr_mirror);
    //dump_list(&dev_ctx->alloc_tx_bdr_mirror);

    return ret;

put_to_free_mirror_ring:
    sw_dcmode_put_bdp_to_bdr_mirror(&dev_ctx->free_tx_bdr_mirror, tx_bdp, 0);

drop:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DUMP_TX, "failed to xmit, dropping!!!\n");

    /* TODO: Update the Tx mib */

    dev_kfree_skb_any(skb);

err_out:
    return ret;
}

static int32_t
sw_dcmode1_get_netif_stats(void *ctx, struct net_device *netif,
              struct dp_subif *subif_id,
              struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
    return 0;
}

static int32_t
sw_dcmode1_clear_netif_stats(void *ctx, struct net_device *netif,
                struct dp_subif *subif_id, uint32_t flags)
{
    return 0;
}

static int32_t
sw_dcmode1_change_dev_status(void *ctx, int32_t port_id, uint32_t flags)
{
    return 0;
}

static void
sw_dcmode1_dump_proc(void *ctx, struct seq_file *seq)
{
    struct sw_dcmode1_dev_info *dev_ctx = (struct sw_dcmode1_dev_info *)ctx;
    int i =0;
    char ch_id[4][15] = {"SOC2DEV","SOC2DEV_RET","DEV2SOC","DEV2SOC_RET"};

    if (!dev_ctx)
        return;

    for (i = 0; i < SW_DCMODE1_CHAN_MAX; i++)
    {
        sw_dcmode_chan_t *ch = &dev_ctx->ch[i];
        sw_dcmode_mem_block_t *ring = &ch->data;
        sw_dcmode_cntr_t   *locl_cntr = &ch->lcl_cntr;
        sw_dcmode_cntr_t   *rmt_cntr = &ch->rm_cntr;
        sw_dcmode_ring_t  *track = &ch->trck;

        seq_printf(seq, "\nchnanel :[%d]\n",i);

        seq_printf(seq, "  Name    : [%s]\n",ch_id[i]);
        seq_printf(seq, "    Rings resources\n");
        seq_printf(seq, "      virtual address  :[0x%pk]\n",(ring->virt));
        seq_printf(seq, "      phys address     :[%#x]\n",(ring->phys));
        seq_printf(seq, "      size             :[%d]\n",(ring->size));
        seq_printf(seq, "      desc_dwsz        :[%d]\n",(ring->desc_dwsz));
        seq_printf(seq, "      head             :[%d]\n",(track->head));
        seq_printf(seq, "      tail             :[%d]\n",(track->tail));

        seq_printf(seq, "    local counters \n");
        seq_printf(seq, "       virtual address :[0x%pk]\n",locl_cntr->virt);
        seq_printf(seq, "       physaddr:[%#x] \n",locl_cntr->phys);
        seq_printf(seq, "       size            :[%d]\n",(locl_cntr->size));
        seq_printf(seq, "       Endian          :[%d]\n",(locl_cntr->endian));
        seq_printf(seq, "       Mode            :[%d]\n",(locl_cntr->mode));
        seq_printf(seq, "       counter value   :[%d]\n",*((uint32_t *)(locl_cntr->virt)));

        seq_printf(seq, "    remote counters \n");
        seq_printf(seq, "       virtual addres  :[0x%pk]\n",rmt_cntr->virt);
        seq_printf(seq, "       physaddr:[%#x]\n",rmt_cntr->phys);
        seq_printf(seq, "       size            :[%d]\n",(rmt_cntr->size));
        seq_printf(seq, "       Endian          :[%d]\n",(rmt_cntr->endian));
        seq_printf(seq, "       Mode            :[%d]\n",(rmt_cntr->mode));
        seq_printf(seq, "       counter value   :[%d]\n",*((uint32_t *)(rmt_cntr->virt)));
    }
}

struct sk_buff *
sw_dcmode1_alloc_skb(void *dev_ctx, uint32_t len, struct dp_subif *subif, uint32_t flags)
{
    struct sk_buff *new_skb;

    new_skb = dev_alloc_skb(len);

    return new_skb;
}

int32_t
sw_dcmode1_free_skb(void *dev_ctx, struct dp_subif *subif, struct sk_buff *skb)
{
    //printk("<%s> Freeing SKB \n",__FUNCTION__);
    dev_kfree_skb_any(skb);

    return 0;
}
static struct dc_dp_dcmode_ops sw_dcmode1_ops = {
    .get_host_capability = sw_dcmode1_get_host_capability,
    .register_dev = sw_dcmode1_register_dev,
    .register_subif = sw_dcmode1_register_subif,
    .xmit = sw_dcmode1_xmit,
    .handle_ring_sw = sw_dcmode1_handle_ring_sw,
    .add_session_shortcut_forward = NULL,
    .disconn_if = NULL,
    .get_netif_stats = sw_dcmode1_get_netif_stats,
    .clear_netif_stats = sw_dcmode1_clear_netif_stats,
    .register_qos_class2prio_cb = NULL,
    .map_class2devqos = NULL,
    .alloc_skb = NULL,
    .free_skb = sw_dcmode1_free_skb,
    .change_dev_status = sw_dcmode1_change_dev_status,
    .get_wol_cfg = NULL,
    .set_wol_cfg = NULL,
    .get_wol_ctrl_status = NULL,
    .set_wol_ctrl = NULL,
    .dump_proc = sw_dcmode1_dump_proc
};

static struct dc_dp_dcmode sw_dcmode1 = {
    .dcmode_cap = DC_DP_F_DCMODE_SW | DC_DP_F_DCMODE_1,
    .dcmode_ops = &sw_dcmode1_ops
};

int sw_dcmode1_init(void)
{
    int32_t ret = 0;

    if (0 == g_sw_dcmode1_init_ok) {
        memset(&g_sw_dcmode1_dev_info,0,sizeof(struct sw_dcmode1_dev_info));
        /* Register SW DCMODE1 */
        ret = dc_dp_register_dcmode(&sw_dcmode1, 0);
        if (ret == -1) {
            printk("sowfare dc mode registration is failing ??\n");
        }
        g_sw_dcmode1_init_ok = 1;
    }

    return ret;
}

void sw_dcmode1_exit(void)
{
    if (g_sw_dcmode1_init_ok) {
        /* De-register SW DCMODE1 */
        dc_dp_register_dcmode(&sw_dcmode1, DC_DP_F_DCMODE_DEREGISTER);

        g_sw_dcmode1_init_ok = 0;
    }
}

#ifndef SW_DCMODE1_BUILTIN
static __init int sw_dcmode1_init_module(void)
{
    int32_t ret = 0;

    ret = sw_dcmode1_init();

    return ret;
}

static __exit void sw_dcmode1_exit_module(void)
{
    sw_dcmode1_exit();
}

module_init(sw_dcmode1_init_module);
module_exit(sw_dcmode1_exit_module);

MODULE_AUTHOR("Anath Bandhu Garai");
MODULE_DESCRIPTION("DC Mode1 support for SW");
MODULE_LICENSE("GPL");
#endif
