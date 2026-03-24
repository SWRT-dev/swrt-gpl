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

#ifndef __DCMODE_SW_COMMON_H__
#define __DCMODE_SW_COMMON_H__

#include <linux/pci.h>
#if 0
//------------------------------------------------------------------------------
// Device name, as registered with kernel and accessed in /dev/
//------------------------------------------------------------------------------
#define DEV_NAME "cppp"

//------------------------------------------------------------------------------
// Supported commands
//------------------------------------------------------------------------------
typedef enum
{
    CPPP_CMD_MIN,
    CPPP_CMD_ACQUIRE,
    CPPP_CMD_RELEASE,
    CPPP_CMD_START,
    CPPP_CMD_STOP,
    CPPP_CMD_ATTR_SET,
    CPPP_CMD_ATTR_GET,
    CPPP_CMD_XFER_WAIT,
    CPPP_CMD_XFER_WAKE,
    CPPP_CMD_MAX,
} cppp_cmd_id_t;

//------------------------------------------------------------------------------
// Structure describing contiguous block of memory
//------------------------------------------------------------------------------
typedef struct
{
    unsigned int phys;
    void *       virt;
    unsigned int size;
    uint64_t     cookie;
} cppp_mem_block_t;

//------------------------------------------------------------------------------
// Single communication channel
//------------------------------------------------------------------------------
typedef struct
{
    cppp_mem_block_t data; // Memory block for page management
    cppp_mem_block_t ctrl; // Memory block for shared counters
    unsigned int     id;   // Channel ID
    unsigned int     sync; // I/O type
    unsigned int     sram; // SRAM address of write-only counter
} cppp_ch_info_t;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
typedef struct
{
    cppp_flag_t      flags;   // Configuration options
    cppp_ch_info_t   ch[4];   // Channel info to be populated back
    cppp_id_t        rcli_id; // ID of reference client
    cppp_mem_block_t ppio;    // Packet processor I/O block

} cppp_acquire_t;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
typedef struct
{
    unsigned int id;  // Channel ID
    unsigned int out; // Direction of transfer
    unsigned int to;  // Timeout, nsec
} cppp_xfer_t;

//------------------------------------------------------------------------------
// Commands container structure
//------------------------------------------------------------------------------
typedef struct __attribute__ ((aligned(64)))
{
    cppp_cmd_id_t cmd_id;
    cppp_id_t     cli_id;
    int           fd;

    union
    {
        cppp_acquire_t _acquire;

        struct
        {
        } _release;

        struct
        {
        } _start;

        struct
        {
        } _stop;

        struct
        {
            cppp_attr_t  id;
            void *       data;
            unsigned int size;
        } _attr;

        cppp_xfer_t _xfer;

    } data;

} cppp_cmd_t;
#endif

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#define SW_DCMODE_CACHE_LINE_SIZE 32
/* #######################################################################################################*/
/* defines */
#define SW_DCMODE1_MAX_DEV_NUM       32
#define CACHE_LINE_SIZE 32
#define DCDP_SHORT_PKT_LEN 32

#define __CH_PRINT(ch, foo) foo(ch, __FUNCTION__)
#define SW_DCMODE1_LOCAL_VIRT(ch) ((unsigned int *) ch->lcl_cntr.virt)
#define SW_DCMODE1_REMOTE_VIRT(ch) ((unsigned int *) ch->rm_cntr.virt)

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

/* ######################## */

#define DC_DP_DEFINE_LOCK(lock) DEFINE_MUTEX(lock)
#define DC_DP_LOCK    mutex_lock
#define DC_DP_UNLOCK  mutex_unlock

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
    uint32_t   desc_dwsz;
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
#if 0 // Little Endian???
    uint16_t tid;  ///< Buffer tag, set by app CPU, replicated by FW
    uint16_t sid;  ///< Session ID, set by FW
    uint32_t misc; ///< Misc...
    uint32_t addr; ///< Physical address of a buffer
    uint8_t  flgs; ///< Buffer control flags
    uint8_t  rsvd; ///< Reserved...
    uint16_t size; ///< Buffer size
#else // Big endian
    uint32_t tid:16;  ///< Buffer tag, set by app CPU, replicated by FW
    uint32_t sid:16;  ///< Session ID, set by FW
    uint32_t misc; ///< Misc...
    uint32_t addr; ///< Physical address of a buffer
    uint32_t flgs:8; ///< Buffer control flags
    uint32_t rsvd:8; ///< Reserved...
    uint32_t size:16; ///< Buffer size
#endif
} sw_dcmode_bd_t;

typedef struct
{
     //SW_DCMODE1_SOC_REMOTE_CNTR_LAST_ACCUM(chan,n) ((chan)->lcl_cntr_trck.last_accumulated);
  //  SW_DCMODE1_SOC_REMOTE_CNTR_LAST_ACCUM(chan,n) 
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

//------------------------------------------------------------------------------
// Ring state tracker, tailored for modulo type management
//------------------------------------------------------------------------------
typedef struct __attribute__ ((__packed__))
{
    union
    {
        uint16_t head;
        unsigned char     head_cache_line[SW_DCMODE_CACHE_LINE_SIZE];
    };

    union
    {
        uint16_t tail;
        unsigned char     tail_cache_line[SW_DCMODE_CACHE_LINE_SIZE];
    };

    uint32_t mask;
    uint32_t size;

} sw_dcmode_ring_t;

typedef struct __attribute__ ((aligned(32)))
{
    union
    {
        volatile uint32_t cntr;
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

typedef enum
{
  SW_DCMODE_DATA_FROM_BIDIRECTIONAL = DMA_BIDIRECTIONAL,
  SW_DCMODE_DATA_TO_DEVICE = DMA_TO_DEVICE,
  SW_DCMODE_DATA_FROM_DEVICE = DMA_FROM_DEVICE
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


static inline void *
get_bd_from_channel(sw_dcmode_chan_t *ch, int32_t index)
{
	void *bdp;
	printk("Index: %d Track Head: %d\n",index, ch->trck.head);
	bdp = (void *)((uint32_t *)ch->data.virt + (index * ch->data.desc_dwsz));

//	if(index > ch->trck.head)
//		return NULL;
	if (ch->trck.head == ch->trck.size)
		ch->trck.head = 0;

	return bdp;
} 



//static inline int32_t
//__xfer(sw_dcmode_chan_t *ch, sw_dcmode_bd_t *d, unsigned n, int xt, unsigned w);
extern int32_t __xfer(sw_dcmode_chan_t *ch, sw_dcmode_bd_t *d, unsigned n, int xt, unsigned w);


/*############################################################################################################*/


//------------------------------------------------------------------------------
// Ring metadata is shared with FW which has different endiannes. To address that
// ring access calls store values in big-endian but return them for Atom use as
// little-endian.
//------------------------------------------------------------------------------
//#define SW_DCMODE_SWAP32(x) __builtin_bswap32(x)
#define SW_DCMODE_SWAP32(x) x

//------------------------------------------------------------------------------
// Emtpy test
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_empty(sw_dcmode_ring_t * r)
{
    return SW_DCMODE_SWAP32(r->tail) == SW_DCMODE_SWAP32(r->head);
}

//-----------------------------------------------------------------------------
// Full test
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_full(sw_dcmode_ring_t * r)
{
    return (((SW_DCMODE_SWAP32(r->tail) - SW_DCMODE_SWAP32(r->head) - 1) & r->mask) == 0);
}

//------------------------------------------------------------------------------
// Number of entries currently inserted in the ring
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_count_used(sw_dcmode_ring_t * r)
{
	//printk("%s:r->mask:%d r->tail:%d r->head:%d \n ",__FUNCTION__,r->mask,r->tail,r->head);	
    return ((SW_DCMODE_SWAP32(r->head) - SW_DCMODE_SWAP32(r->tail)) & r->mask);
}

//------------------------------------------------------------------------------
// Number of empty entries available in the list
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_count_free(sw_dcmode_ring_t * r)
{
	//printk("%s:r->mask:%d r->tail:%d r->head:%d \n ",__FUNCTION__,r->mask,r->tail,r->head);	
    return (r->mask + SW_DCMODE_SWAP32(r->tail) - SW_DCMODE_SWAP32(r->head));
    //return (r->mask + SW_DCMODE_SWAP32(r->mask));
}

//------------------------------------------------------------------------------
// Number of available entries, depending on the context of usage
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_count(sw_dcmode_ring_t * r, unsigned int prod)
{
	
	//printk("print1:%s :%d\n",__FUNCTION__,__LINE__);	
    return prod ? sw_dcmode_ring_count_free(r) : sw_dcmode_ring_count_used(r);
}

//------------------------------------------------------------------------------
// Counter query, dependent on context of usage
//------------------------------------------------------------------------------
static inline unsigned int sw_dcmode_ring_index(sw_dcmode_ring_t * r, unsigned int prod)
{
	//printk("%s:r->mask:%d r->tail:%d r->head:%d \n ",__FUNCTION__,r->mask,r->tail,r->head);	
    //return (SW_DCMODE_SWAP32(prod ? r->head : r->tail) & r->mask);
    return ((prod ? r->head : r->tail) & r->mask);
}

//------------------------------------------------------------------------------
// Endianness conversion free, wraparound free counter query
//------------------------------------------------------------------------------
//static inline unsigned int sw_dcmode_ring_index_raw(sw_dcmode_ring_t * r, unsigned int prod)
static inline uint32_t sw_dcmode_ring_index_raw(sw_dcmode_ring_t * r, unsigned int prod)
{
	//printk("%s: r->tail:%d r->head:%d \n ",__FUNCTION__,r->tail,r->head);	
    return prod ? r->head : r->tail;
}

//------------------------------------------------------------------------------
// Counter increment, dependent on context of usage
//------------------------------------------------------------------------------
static inline void sw_dcmode_ring_index_inc(sw_dcmode_ring_t * r, unsigned int prod, unsigned int n)
{
	//printk("%s: r->tail:%d r->head:%d \n ",__FUNCTION__,r->tail,r->head);	
    if (prod)
    {
        r->head = SW_DCMODE_SWAP32(SW_DCMODE_SWAP32(r->head) + n);
    }
    else
    {
        r->tail = SW_DCMODE_SWAP32(SW_DCMODE_SWAP32(r->tail) + n);
    }
}

#endif // __DCMODE_SW_COMMON_H__
