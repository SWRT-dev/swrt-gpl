
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_TX_CELL_BUFFER_H__
#define __IMA_TX_CELL_BUFFER_H__

#define CELL_BUFFER_PREALLOC_LIMIT 64
#define CELL_BUFFER_MALLOC_LIMIT  1024
#define CELL_BUFFER_POOL_SIZE	 (CELL_BUFFER_PREALLOC_LIMIT + CELL_BUFFER_MALLOC_LIMIT)

typedef struct
{
	void *ptrs[CELL_BUFFER_POOL_SIZE]; /* array list of free buffers */
	uint32_t free_count;			   /* number of free buffers available */
	uint32_t malloc_count;			 /* number of new free buffers allocated so far */
	uint32_t lock_count;			   /* number of free buffers reserved for future distribution  */

#ifdef __KERNEL__
	spinlock_t   spinlock;
#endif

} TxCBData_t;

extern TxCBData_t g_privTxCBData;

s32 init_txbuff_stack(void);
bool reserve_from_txbuff_stack(u32 count);
void unreserve_from_txbuff_stack(uint32_t count);
void *pop_from_txbuff_stack(void);
void push_into_txbuff_stack(void *buff);
void cleanup_txbuff_stack(void);

#endif /* __IMA_TX_CELL_BUFFER_H__ */
