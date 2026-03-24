
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <linux/kernel.h>
#include <linux/types.h>
#include "common/ima_common.h"
#include "common/ima_tx_queue.h"
#include "atm/ima_tx_module.h"

#define TXPRIV (&g_tx_privdata)

struct host_txin_desc_cache
read_host_txin_head_desc(u32 idx)
{
	struct tx_desc_4dw *desc_raw;
	struct tx_desc_4dw desc;
	struct host_txin_desc_cache desc_cache;

	desc_raw = (struct tx_desc_4dw *)TXPRIV->host_txin_ring.start;
	desc_raw += idx;

	/* NOTE: copy the raw desc before any other read */
	memcpy(&desc, desc_raw, sizeof(struct tx_desc_4dw));
	desc_cache.buff_phys =
		desc.data_pointer + desc.status.field.byte_offset;
	desc_cache.buff = __va(desc_cache.buff_phys);
	desc_cache.bufflen = desc.status.field.data_len;
	desc_cache.ep = desc.dw1.field.ep;
	desc_cache.dest_id = desc.dw0.field.dest_id;

	dma_unmap_single(NULL, desc_cache.buff_phys,
		desc_cache.bufflen, DMA_FROM_DEVICE);

	return desc_cache;
}

void
write_host_txout_tail(u32 bufflen, void *buff, dma_addr_t buff_phys)
{
	u32 *desc;

	dma_map_single(NULL, buff, bufflen, DMA_FROM_DEVICE);
	if (dma_mapping_error(NULL, buff_phys))
		pr_err("[%s:%d] dma_map_single() failed\n", __func__, __LINE__);

	desc = (u32 *)TXPRIV->host_txout_ring.start;
	desc += TXPRIV->host_txout_ring.tail;

	*desc = buff_phys;

	TXPRIV->host_txout_ring.tail
		= (TXPRIV->host_txout_ring.tail + 1)
			% TXPRIV->host_txout_ring.size;
}

#ifdef ASM_LOOPBACK
#include "common/ima_rx_queue.h"

void
write_device_txin_tail(u32 lineidx, u32 idx, u8 *buf)
{
	struct desc_4dw *desc;
	dma_addr_t buf_phys;

	buf_phys = dma_map_single(NULL, buf, 56, DMA_TO_DEVICE);
	if (dma_mapping_error(NULL, buf_phys))
		pr_err("[%s:%d] dma_map_single() failed\n", __func__, __LINE__);

	desc = (struct desc_4dw *)TXPRIV->ep[lineidx].start;
	desc += idx;

	memset(desc, 0, sizeof(struct desc_4dw));
	desc->dw1.field.ep |= *(u16 *)(buf + 60) & 0xf;
	desc->dw0.field.dest_id |= *(u16 *)(buf + 62) & 0x78; /* [6,3]=qid */
	desc->dw0.field.dest_id |= 0x04; /* [2]=mpoa_pt */

	desc->data_pointer = (buf_phys & ~0x7);

	desc->status.field.sop = 1;
	desc->status.field.eop = 1;
	desc->status.field.byte_offset = (buf_phys & 0x7);
	desc->status.field.data_len = 56;
	wmb();
	desc->status.field.own = 1;

	*((u32 *)TXPRIV->ep[lineidx].umt_out_addr) = 1;
}

u8 *
read_device_txout_head(u32 lineidx, u32 idx)
{
	struct desc_4dw *desc = NULL;
	dma_addr_t buf_phys = 0;
	u8 *bufptr = NULL;

	desc = (struct desc_4dw *)TXPRIV->ep[lineidx].start;
	desc += idx;

	if (0 == desc->status.field.own)
		buf_phys = (desc->data_pointer + desc->status.field.byte_offset);

	if (buf_phys) {
		dma_unmap_single(NULL, (dma_addr_t)buf_phys,
			56, DMA_FROM_DEVICE);
		bufptr = __va(buf_phys);
		desc->data_pointer = desc->status.field.byte_offset = 0;
	}

	return bufptr;
}
#else
void
write_device_txin_tail(u32 lineidx, u32 idx, u8 *buf)
{
	struct tx_desc_4dw *desc;
	dma_addr_t buf_phys;

	buf_phys = dma_map_single(NULL, buf, 56, DMA_TO_DEVICE);
	if (dma_mapping_error(NULL, buf_phys))
		pr_err("[%s:%d] dma_map_single() failed\n", __func__, __LINE__);

	desc = (struct tx_desc_4dw *)TXPRIV->ep[lineidx].start;
	desc += idx;

	memset(desc, 0, sizeof(struct tx_desc_4dw));
	desc->dw1.field.ep |= *(u16 *)(buf + 60) & 0xf;
	desc->dw0.field.dest_id |= *(u16 *)(buf + 62) & 0x78; /* [6,3]=qid */
	desc->dw0.field.dest_id |= 0x04; /* [2]=mpoa_pt */

	desc->data_pointer = (buf_phys & ~0x7);

	desc->status.field.sop = 1;
	desc->status.field.eop = 1;
	desc->status.field.byte_offset = (buf_phys & 0x7);
	desc->status.field.data_len = 56;
	wmb();
	desc->status.field.own = 1;

	*((u32 *)TXPRIV->ep[lineidx].umt_out_addr) = 1;
}

u8 *
read_device_txout_head(u32 lineidx, u32 idx)
{
	u32 *desc;
	u8 *bufptr = NULL;

	desc = (u32 *)TXPRIV->ep[lineidx].txout_start;
	desc += idx;

	if (*desc) {
		dma_unmap_single(NULL, (dma_addr_t)*desc,
			56, DMA_FROM_DEVICE);
		bufptr = __va(*desc);
		*desc = 0;
	}

	return bufptr;
}
#endif
