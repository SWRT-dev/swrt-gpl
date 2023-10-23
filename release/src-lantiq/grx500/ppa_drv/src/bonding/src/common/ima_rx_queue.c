
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file contains the abstraction logic of a ring to a line-queue upstairs
 * being inspected by bonding library.
 * Within the IMA+ Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |   1   | Reassembly FSM Library              | ima_rx_reassembler.c(.h)  |
 *    |   2   | Bonding Library                     | ima_rx_bonding.c(.h)      |
 *    |   3   | ATM Cell Decode Library             | ima_rx_atm_cell.h         |
 *    |   4   | Line Queue Abstraction Library      | ima_rx_queue.c(.h)        |<=
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 *
 *    Unit Tests: 1. src/test/ut_rx_reassembly_testgroup_2.c
 *                2. src/test/ut_rx_reassembly_testgroup_3.c
 *                3. src/test/ut_rx_bonding_and_reassembly_testgroup_1.c
 */

#ifdef __KERNEL__
#include <linux/spinlock.h>
#else
#include <stdint.h> // uint8_t, uint32_t
#include <stddef.h> // NULL
#endif

#include "common/atm_bond.h"
#include "atm/ima_atm_rx.h"
#include "common/ima_rx_queue.h"

#ifdef __KERNEL__
#ifndef PRIV
#define PRIV (&g_privdata)
#endif

u8 *GetBuffer(uint32_t lineidx, uint32_t idx)
{
	struct desc_4dw *desc;
	u32 buffer_pointer;
	u32 buffer_length;
	u32 buffer_offset;

	if (lineidx >= 2) {
		pr_err("[%s:%d] lineidx:%d used invalid!\n",
			__func__, __LINE__, lineidx);
		return NULL;
	}

	desc = (struct desc_4dw *)PRIV->m_st_device_info[lineidx].start;
	desc += idx;

	//pr_info("[%s:%d] lineidx:%d idx=%d dw0:0x%08x, dw1:0x%08x,",
	//		" dw2:0x%08x, dw3:0x%08x\n",
	//		__func__, __LINE__, lineidx, idx,
	//		desc->dw0.all, desc->dw1.all,
	//		desc->data_pointer, desc->status.all);

	buffer_pointer = (u32)__va(desc->data_pointer);
	buffer_offset = desc->status.field.byte_offset;
	buffer_length = desc->status.field.data_len;
	if (buffer_length != 56) {
		pr_crit("[%s:%d] buffer size is %d (!=56) on lineidx:%d\n",
			__func__, __LINE__, buffer_length, lineidx);
	}
	dma_map_single(NULL, (void *)(buffer_pointer + buffer_offset),
			buffer_length, DMA_FROM_DEVICE);

	//dump_cell(lineidx, ((u8 *)(buffer_pointer + buffer_offset)),
	//		buffer_length, "RX");

	return ((u8 *)(buffer_pointer + buffer_offset));
}

void ReleaseBuffer(uint32_t lineidx, uint32_t idx)
{
	struct desc_4dw *desc;

	if (lineidx >= 2) {
		pr_err("[%s:%d] lineidx:%d used invalid!\n",
			__func__, __LINE__, lineidx);
		return;
	}

	/* FIXME : separate lock for rx? */
	desc = (struct desc_4dw *)PRIV->m_st_device_info[lineidx].start;
	desc += idx;

	//pr_info("[%s:%d] lineidx:%d idx=%d dw0:0x%08x, dw1:0x%08x,",
	//		" dw2:0x%08x, dw3:0x%08x\n",
	//		__func__, __LINE__, lineidx, idx,
	//		desc->dw0.all, desc->dw1.all,
	//		desc->data_pointer, desc->status.all);
	desc->status.field.own = 0;
	*((u32 *)PRIV->m_st_device_info[lineidx].umt_out_addr) = 1;
}

u8 IsBufferOwnedByHost(uint32_t lineidx, uint32_t idx)
{
	struct desc_4dw *desc;

	if (lineidx >= 2) {
		pr_err("[%s:%d] lineidx:%d used invalid!\n",
			__func__, __LINE__, lineidx);
		return 0;
	}

	/* FIXME : separate lock for rx? */
	desc = (struct desc_4dw *)PRIV->m_st_device_info[lineidx].start;
	desc += idx;

#if 0
	pr_info("[%s:%d] lineidx:%d idx=%d dw0:0x%08x, dw1:0x%08x, dw2:0x%08x, dw3:0x%08x\n",
		__func__, __LINE__, lineidx, idx,
		desc->dw0.all, desc->dw1.all,
		desc->data_pointer, desc->status.all);
#endif
	return (desc->status.field.own == 1);
}

#ifndef GET_ELEMENT
#define GET_ELEMENT(lineid, idx) GetBuffer((lineid), (idx))
#endif

#ifndef RELEASE_LINEQ_HEAD
#define RELEASE_LINEQ_HEAD(lineid, idx) ReleaseBuffer((lineid), (idx))
#endif

#ifndef LINEQ_ELEMENT_IS_NEW
#define LINEQ_ELEMENT_IS_NEW(lineid, idx) IsBufferOwnedByHost((lineid), (idx))
#endif
#endif

void
init_rx_lineq(unsigned int lineid, void *data)
{
	struct bond_drv_cfg_params *cfg = (struct bond_drv_cfg_params *)data;

	/*
	 * spinlock this lineq due to concurrency issues with immediate
	 * interrupt-handler: tasklet over rx_head, rx_count, rx_tail, rx_size
	 */
#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	/*
	 * TODO: Disable lineq interrupts.
	 * Ideally they should be disabled before this function gets called
	 */

	/* Initialize the Rx HW ring metadata */
	PRIV->m_st_device_info[lineid].start
		= cfg->ep_conn[lineid].rxout.soc_desc_base;
	PRIV->m_st_device_info[lineid].start_phys
		= cfg->ep_conn[lineid].rxout.soc_desc_base_phys;
	PRIV->m_st_device_info[lineid].rx_size
		= cfg->ep_conn[lineid].rxout.soc_desc_num;
	PRIV->m_st_device_info[lineid].rx_head
		= PRIV->m_st_device_info[lineid].rx_tail = 0;
	PRIV->m_st_device_info[lineid].rx_count = 0;

	PRIV->m_st_device_info[lineid].umt_out_addr
		= cfg->aca_umt_hdparams[lineid][ACA_RXIN];

	/* TODO: Enable lineq interrupts */

	/* spinunlock this lineq */
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif
}

unsigned char *
read_rx_lineq_head(unsigned int lineid)
{
	int usedspace = 0;

	/*
	 * spinlock due to concurrency issues with immediate
	 * interrupt-handler (poller) over rx_count
	 */
#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	usedspace = PRIV->m_st_device_info[lineid].rx_count;

	/* spinunlock */
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	if (!usedspace) {
		// Line Queue is empty
		return NULL;
	}

	return GET_ELEMENT(lineid, PRIV->m_st_device_info[lineid].rx_head);
}

void
dequeue_rx_lineq(unsigned int lineid)
{
	/*
	 * spinlock due to concurrency issues with immediate
	 * interrupt-handler (poller) over rx_count
	 */
#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	if (!PRIV->m_st_device_info[lineid].rx_count) {

		/* spinunlock */
#ifdef __KERNEL__
		spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

		// Line Queue is empty. Nothing to de-queue
		return;
	}

	RELEASE_LINEQ_HEAD(lineid,  PRIV->m_st_device_info[lineid].rx_head);

	/* Move metadata.head to next element in the ring */
	PRIV->m_st_device_info[lineid].rx_head =
		(PRIV->m_st_device_info[lineid].rx_head + 1) %
		PRIV->m_st_device_info[lineid].rx_size;

	/* Reduce metadata.size of the ring to one element less */
	--(PRIV->m_st_device_info[lineid].rx_count);

	/* spinunlock */
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif
}

unsigned int
enqueue_rx_lineq(unsigned int lineid)
{
	int count = 0;
	int freespace = 0;
	uint32_t descidx;

	/* spinlock due to concurrency issues with tasklet over rx_count */
#ifdef __KERNEL__
	spin_lock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	freespace = PRIV->m_st_device_info[lineid].rx_size -
			PRIV->m_st_device_info[lineid].rx_count;

	/* spinunlock */
#ifdef __KERNEL__
	spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

	/*
	 * Skim through the new entries, if any,
	 * starting from tail, max upto freespace
	 */
	for (count = 0; count < freespace; ++count) {
		descidx = (PRIV->m_st_device_info[lineid].rx_tail + count) %
				(PRIV->m_st_device_info[lineid].rx_size);

		// Deemed fit to be a new arrival ?
		if (!(LINEQ_ELEMENT_IS_NEW(lineid, descidx)))
			break;
	}

	/* How many new arrivals ? */
	if (count) {
		/* Yes, new arrivals */

		/* Elongate tail to cover these new arrivals */
		PRIV->m_st_device_info[lineid].rx_tail =
			(PRIV->m_st_device_info[lineid].rx_tail + count) %
			PRIV->m_st_device_info[lineid].rx_size;

		/*
		 * spinlock due to concurrency issues with
		 * tasklet over rx_count
		 */
#ifdef __KERNEL__
		spin_lock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif

		PRIV->m_st_device_info[lineid].rx_count += count;

		/* spinunlock */
#ifdef __KERNEL__
		spin_unlock_bh(&(PRIV->m_st_device_info[lineid].rx_spinlock));
#endif
	}

	return count;
}
