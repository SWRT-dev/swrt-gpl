
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h> // spin_lock_bh(), spin_unlock_bh()
#include <net/lantiq_cbm_api.h> // TODO
#else
#include <stdint.h> // uint8_t, uint32_t
#include <stddef.h> // NULL
#include <stdlib.h> // malloc
#endif
#include "atm/ima_tx_cell_buffer.h"

#define CELL_BUFFER_SIZE		   64

TxCBData_t g_privTxCBData;

#define PRIV (&g_privTxCBData)

#ifdef __KERNEL__
#define INIT_SPINLOCK() spin_lock_init( &(PRIV->spinlock) )
#define SPINLOCK()	  spin_lock_bh( &(PRIV->spinlock) )
#define SPINUNLOCK()	spin_unlock_bh( &(PRIV->spinlock) )
#define CELL_BUFFER_KMALLOC() kmalloc(CELL_BUFFER_SIZE, GFP_ATOMIC)
#define CELL_BUFFER_KFREE(buff)	\
do {									\
	/* TODO : use uniform buffer ether cbm buffer or kmalloc'd */	\
	if (check_ptr_validation((uint32_t)buff)) {			\
		cbm_buffer_free(0, buff, 0);				\
	} else {							\
		kfree(buff);						\
	}								\
} while(0)
#else
#define INIT_SPINLOCK()
#define SPINLOCK()
#define SPINUNLOCK()
#define CELL_BUFFER_KMALLOC() (void *)malloc(CELL_BUFFER_SIZE);
#define CELL_BUFFER_KFREE(buff)  free((buff))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* To be called from module-init */
int32_t
init_txbuff_stack(void)
{
	uint32_t counter;
	PRIV->free_count = 0;
	PRIV->malloc_count = 0;
	PRIV->lock_count = 0;

	INIT_SPINLOCK();

	for( counter = 0; counter < CELL_BUFFER_POOL_SIZE; ++counter )
	{
		PRIV->ptrs[counter] = NULL;
	}

	return 0;
}

/* to be called once prior to a loop of contiguous pop_from_txbuff_stack() calls */
bool
reserve_from_txbuff_stack(uint32_t count)
{
	bool result = true;
	uint32_t pending, counter;

	SPINLOCK();

	if( PRIV->free_count - PRIV->lock_count >= count )
	{
		// Sufficient free-buffers already available

		PRIV->lock_count += count;
		goto end;
	}

	// Sufficient buffers unavailable
	pending = count - PRIV->free_count + PRIV->lock_count;

	if( CELL_BUFFER_MALLOC_LIMIT < (pending + PRIV->malloc_count) )
	{
		// Crosses the malloc-barrier => Backpressure

		result = false;
		goto end;
	}

	// Within the malloc barrier. Try allocating each of them
	for( counter = 0; counter < pending; ++counter )
	{
		PRIV->ptrs[PRIV->free_count] = CELL_BUFFER_KMALLOC();

		if( NULL == PRIV->ptrs[PRIV->free_count] )
		{
			result = false;
			goto end;
		}

		PRIV->malloc_count += 1;
		PRIV->free_count += 1;
	}

	PRIV->lock_count += count;

end:

	SPINUNLOCK();

	return result;
}

/* FIXME: This function is NEW and UNTESTED. */
/* To be called to unlock a set of previously locked but unused buffers */
void
unreserve_from_txbuff_stack(uint32_t count)
{
	SPINLOCK();

	if( PRIV->lock_count >= count )
	{
		// Sufficient locked-buffers available
		PRIV->lock_count -= count;

	} else {

		// Insufficient locked-buffers. Unlock whatever is left
		PRIV->lock_count = 0;
	}

	SPINUNLOCK();
}

/*
get the next free buffer from cell-buffer-pool. Desired number of cell buffers MUST necessarily be reserved earlier by a call to CBReserver()
*/
void *
pop_from_txbuff_stack(void)
{
	void *retval = NULL;

	SPINLOCK();
	if( PRIV->free_count )
	{
		PRIV->free_count -= 1;
		retval = PRIV->ptrs[PRIV->free_count];
		PRIV->ptrs[PRIV->free_count] = NULL;
		PRIV->lock_count -= (PRIV->lock_count ? 1 : 0);
	}
	SPINUNLOCK();

	return retval;
}

/*
-- release the cell buffer back to cell-buffer-pool
*/
void
push_into_txbuff_stack( void *buff)
{
	SPINLOCK();
	if( CELL_BUFFER_POOL_SIZE > PRIV->free_count )
	{
		PRIV->ptrs[PRIV->free_count] = buff;
		PRIV->free_count += 1;

	} else {
		pr_crit("[%s:%d] %u - 0x%px\n", __func__, __LINE__, PRIV->free_count, buff);
		CELL_BUFFER_KFREE(buff);
	}
	SPINUNLOCK();
}

/*
-- kfree all the cell buffers sitting in cell-buffer-pool
*/
void
cleanup_txbuff_stack(void)
{
	SPINLOCK();
	while( PRIV->free_count )
	{
		if( PRIV->ptrs[PRIV->free_count - 1] )
		{
			CELL_BUFFER_KFREE(PRIV->ptrs[PRIV->free_count - 1]);
		}
		PRIV->free_count -= 1;
	}
	SPINUNLOCK();
}

#ifndef __KERNEL__

/*===================================================================*
 *  UNIT TESTS                                                       *
 *                                                                   *
 *  To run:                                                          *
 *    $> cd source/ppa_drv/src/bonding                               *
 *    $> gcc -Wall src/atm/ima_tx_cell_buffer.c                      *
 *    $> ./a.out                                                     *
 *===================================================================*/

#include <stdio.h>

#define BEGIN_UNIT_TESTS() int main( int argc, char *argv[] ) { int total=0, passed=0;
#define END_UNIT_TESTS() \
		printf( "===========================================\n" ); \
		printf( "TOTAL TESTS: %d	PASSED: %d	FAILED: %d\n", total, passed, total - passed ); \
		printf( "===========================================\n" ); \
		return 0; \
	}\
	typedef int DUMMY;
#define BEGIN_TEST(desc) { printf( "Unit test %d: %s\n{\n", total+1, (desc) ); int pass=1;
#define END_TEST() ++total; printf( "}\nUnit test %d ", total ); if( pass ) { ++passed; printf( "passed\n" ); } else printf( "failed\n" ); }
#define PASSED() (1==pass)
#define PRINTF(format, ...) printf( "	" format, ## __VA_ARGS__);
#define NULL_STRINGIFY(ptr) (NULL==(void *)(ptr) ? "NULL" : "NOT NULL")
#define ASSERT_NUM(str, expected, actual) PRINTF( "%s: Expected: %d, Actual: %d\n", (str), (expected), (actual) );\
				if( (expected) != (actual) ) pass=0;
#define ASSERT_NULL(str, actual) PRINTF( "%s: Expected: NULL, Actual: %s\n", (str), NULL_STRINGIFY((actual)) );\
				if( NULL != (actual) ) pass=0;
#define ASSERT_NOT_NULL(str, actual) PRINTF( "%s: Expected: NOT NULL, Actual: %s\n", (str), NULL_STRINGIFY((actual)) );\
				if( NULL == (actual) ) pass=0;

BEGIN_UNIT_TESTS()
{
	uint8_t result;
	void *buff;

	BEGIN_TEST( "init_txbuff_stack(): Verify it properly updates the private data" ) {

		init_txbuff_stack();
		PRINTF( "Called push_into_txbuff_stack()\n" );

		ASSERT_NUM( "FREE_COUNT", 0, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 0, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 0, PRIV->lock_count );

	} END_TEST();

	BEGIN_TEST( "init_txbuff_stack(): Verify free-list is empty" ) {

		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(1): Verify it kmallocs one cell buffer when the free-list is empty" ) {

		result = reserve_from_txbuff_stack(1);
		ASSERT_NUM( "ALLOCCBS(1) return", 1, result);
		ASSERT_NUM( "FREE_COUNT", 1, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 1, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 1, PRIV->lock_count );
		ASSERT_NOT_NULL( "PRIV->PTRS[0]", PRIV->ptrs[0] );
		ASSERT_NULL( "PRIV->PTRS[1]", PRIV->ptrs[1] );

	} END_TEST();

	BEGIN_TEST( "pop_from_txbuff_stack(): Verify it hands over the sole buffer from the free-list therefore making the list empty" ) {

		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		ASSERT_NUM( "FREE_COUNT", 0, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 1, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 0, PRIV->lock_count );
		ASSERT_NULL( "PRIV->PTRS[0]", PRIV->ptrs[0] );
		ASSERT_NULL( "PRIV->PTRS[1]", PRIV->ptrs[1] );

	} END_TEST();

	BEGIN_TEST( "pop_from_txbuff_stack(): Verify it fails to return a buffer when the free-list is empty" ) {

		char *tmpbuff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", tmpbuff );

	} END_TEST();

	BEGIN_TEST( "push_into_txbuff_stack(): Verify it inserts the free-buffer back into the empty free-list" ) {

		push_into_txbuff_stack(buff);
		ASSERT_NUM( "FREE_COUNT", 1, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 1, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 0, PRIV->lock_count );
		ASSERT_NOT_NULL( "PRIV->PTRS[0]", PRIV->ptrs[0] );
		ASSERT_NULL( "PRIV->PTRS[1]", PRIV->ptrs[1] );

	} END_TEST();

	BEGIN_TEST( "cleanup_txbuff_stack(): Verify it empties the free-list" ) {

		cleanup_txbuff_stack();
		ASSERT_NUM( "FREE_COUNT", 0, PRIV->free_count );

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(5): Verify it kmallocs 5 cell-buffers, and they get removed from the free-list during each pop_from_txbuff_stack()-call" ) {

		init_txbuff_stack();
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );
		result = reserve_from_txbuff_stack(5);
		ASSERT_NUM( "reserve_from_txbuff_stack(5) return", 1, result );
		ASSERT_NUM( "FREE_COUNT", 5, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 5, PRIV->lock_count );
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(5) followed by reserve_from_txbuff_stack(2): Verify the second call kmallocs 2 more cell-buffers" ) {

		init_txbuff_stack();
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );
		result = reserve_from_txbuff_stack(5);
		ASSERT_NUM( "reserve_from_txbuff_stack(5) return", 1, result );
		ASSERT_NUM( "FREE_COUNT", 5, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 5, PRIV->lock_count );
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		ASSERT_NUM( "FREE_COUNT", 3, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 3, PRIV->lock_count );
		result = reserve_from_txbuff_stack(2);
		ASSERT_NUM( "reserve_from_txbuff_stack(2) return", 1, result );
		ASSERT_NUM( "FREE_COUNT", 5, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 7, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 5, PRIV->lock_count );
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );

	} END_TEST();

	BEGIN_TEST( "push_into_txbuff_stack(): Verify it adds the free cellbuffer back into the free-list" ) {

		init_txbuff_stack();
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );
		result = reserve_from_txbuff_stack(5);
		ASSERT_NUM( "reserve_from_txbuff_stack(5) return", 1, result );
		ASSERT_NUM( "FREE_COUNT", 5, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 5, PRIV->lock_count );
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		ASSERT_NUM( "FREE_COUNT", 3, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 3, PRIV->lock_count );
		buff = CELL_BUFFER_KMALLOC();
		ASSERT_NOT_NULL( "CELL_BUFFER_MALLOC() return", buff );
		push_into_txbuff_stack(buff);
		PRINTF( "Called push_into_txbuff_stack()\n" );
		ASSERT_NUM( "FREE_COUNT", 4, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 3, PRIV->lock_count );
		buff = CELL_BUFFER_KMALLOC();
		ASSERT_NOT_NULL( "CELL_BUFFER_MALLOC() return", buff );
		push_into_txbuff_stack(buff);
		PRINTF( "Called push_into_txbuff_stack()\n" );
		ASSERT_NUM( "FREE_COUNT", 5, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 5, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 3, PRIV->lock_count );
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NOT_NULL( "GETNEXTCB() return", buff );
		if( PASSED() ) free(buff);
		buff = pop_from_txbuff_stack();
		ASSERT_NULL( "GETNEXTCB() return", buff );

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify it allocates upto CELL_BUFFER_MALLOC_LIMIT" ) {

		init_txbuff_stack();
		result = reserve_from_txbuff_stack(CELL_BUFFER_MALLOC_LIMIT);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_MALLOC_LIMIT)", 1, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->lock_count );
		cleanup_txbuff_stack();

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify it does not allocate beyond CELL_BUFFER_MALLOC_LIMIT" ) {

		init_txbuff_stack();
		result = reserve_from_txbuff_stack(CELL_BUFFER_MALLOC_LIMIT + 1);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_MALLOC_LIMIT + 1)", 0, result );
		ASSERT_NUM( "FREE_COUNT", 0, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", 0, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", 0, PRIV->lock_count );
		cleanup_txbuff_stack();

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify it does not cross CELL_BUFFER_MALLOC_LIMIT when it is called in multiple slabs" ) {

		init_txbuff_stack();
		result = reserve_from_txbuff_stack(CELL_BUFFER_MALLOC_LIMIT/2);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_MALLOC_LIMIT/2)", 1, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->lock_count );
		result = reserve_from_txbuff_stack(CELL_BUFFER_MALLOC_LIMIT/2 + 1);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_MALLOC_LIMIT/2 + 1)", 0, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_MALLOC_LIMIT/2, PRIV->lock_count );
		cleanup_txbuff_stack();

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify free-list can hold upto CELL_BUFFER_POOL_SIZE entries" ) {
		int i;

		init_txbuff_stack();

		for( i=0; i<CELL_BUFFER_PREALLOC_LIMIT; ++i )
		{
			buff = CELL_BUFFER_KMALLOC();
			ASSERT_NOT_NULL( "CELL_BUFFER_MALLOC() return", buff);

			push_into_txbuff_stack(buff);
			ASSERT_NUM( "FREE_COUNT", i+1, PRIV->free_count );
		}
		result = reserve_from_txbuff_stack(CELL_BUFFER_POOL_SIZE);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_POOL_SIZE)", 1, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->lock_count );

		cleanup_txbuff_stack();

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify reserve_from_txbuff_stackrefuses to allocate beyond CELL_BUFFER_POOL_SIZE entries" ) {

		int i;

		init_txbuff_stack();

		for( i=0; i<CELL_BUFFER_PREALLOC_LIMIT; ++i )
		{
			buff = CELL_BUFFER_KMALLOC();
			ASSERT_NOT_NULL( "CELL_BUFFER_MALLOC() return", buff);

			push_into_txbuff_stack(buff);
			ASSERT_NUM( "FREE_COUNT", i+1, PRIV->free_count );
		}
		result = reserve_from_txbuff_stack(CELL_BUFFER_POOL_SIZE);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_POOL_SIZE)", 1, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->lock_count );

		result = reserve_from_txbuff_stack(1);
		ASSERT_NUM( "AllocSBs(1)", 0, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->lock_count );

		cleanup_txbuff_stack();

	} END_TEST();

	BEGIN_TEST( "reserve_from_txbuff_stack(): Verify push_into_txbuff_stackdoes not insert into Free-list beyond CELL_BUFFER_POOL_SIZE entries" ) {

		int i;

		init_txbuff_stack();

		for( i=0; i<CELL_BUFFER_PREALLOC_LIMIT; ++i )
		{
			buff = CELL_BUFFER_KMALLOC();
			ASSERT_NOT_NULL( "CELL_BUFFER_MALLOC() return", buff);

			push_into_txbuff_stack(buff);
			ASSERT_NUM( "FREE_COUNT", i+1, PRIV->free_count );
		}
		result = reserve_from_txbuff_stack(CELL_BUFFER_POOL_SIZE);
		ASSERT_NUM( "AllocSBs(CELL_BUFFER_POOL_SIZE)", 1, result );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->lock_count );

		buff = CELL_BUFFER_KMALLOC();
		PRINTF( "Called CELL_BUFFER_KMALLOC\n" );
		push_into_txbuff_stack(buff);
		PRINTF( "Called push_into_txbuff_stack\n" );
		ASSERT_NUM( "FREE_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->free_count );
		ASSERT_NUM( "MALLOC_COUNT", CELL_BUFFER_MALLOC_LIMIT, PRIV->malloc_count );
		ASSERT_NUM( "LOCK_COUNT", CELL_BUFFER_POOL_SIZE, PRIV->lock_count );

		cleanup_txbuff_stack();

	} END_TEST();
}
END_UNIT_TESTS();

#endif /* __KERNEL__ */
