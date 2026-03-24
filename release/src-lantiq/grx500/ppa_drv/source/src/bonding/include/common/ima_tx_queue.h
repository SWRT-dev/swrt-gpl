
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_TX_QUEUE_H__
#define __IMA_TX_QUEUE_H__

struct tx_desc_4dw {
	union {
		struct {
			u32 resv:17;
			u32 dest_id:15;
		} __packed field;
		u32 all;
	} __packed dw0;
	union {
		struct {
			u32 resv:20;
			u32 ep:4;
			u32 resv1:8;
		} __packed field;
		u32 all;
	} __packed dw1;
	dma_addr_t data_pointer; /* dw2 */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 resv:2;
			u32 byte_offset:3;
			u32 resv1:7;
			u32 data_len:16;
		} __packed field;
		u32 all;
	} __packed status; /*dw3 */
} __packed __aligned(16);

struct host_txin_desc_cache {
	u8 *buff;
	u32 bufflen;
	dma_addr_t buff_phys;
	u16 ep;
	u16 dest_id;
};

struct host_txin_desc_cache read_host_txin_head_desc(u32 idx);
void write_host_txout_tail(u32 bufflen, void *buff, dma_addr_t buff_phys);
void write_device_txin_tail(u32 lineid, u32 idx, u8 *buf);
u8 *read_device_txout_head(u32 lineidx, u32 idx);

#endif /* __IMA_TX_QUEUE_H__ */
