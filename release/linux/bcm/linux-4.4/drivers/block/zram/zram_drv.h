/*
 * Compressed RAM block device
 *
 * Copyright (C) 2008, 2009, 2010  Nitin Gupta
 *               2012, 2013 Minchan Kim
 *
 * This code is released using a dual license strategy: BSD/GPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of 3-clause BSD License
 * Released under the terms of GNU General Public License Version 2.0
 *
 */

#ifndef _ZRAM_DRV_H_
#define _ZRAM_DRV_H_

#include <linux/spinlock.h>
#include <linux/zsmalloc.h>

#include "zcomp.h"

/*-- Configurable parameters */

/*
 * Pages that compress to size greater than this are stored
 * uncompressed in memory.
 */
static const size_t max_zpage_size = PAGE_SIZE / 4 * 3;

/*
 * NOTE: max_zpage_size must be less than or equal to:
 *   ZS_MAX_ALLOC_SIZE. Otherwise, zs_malloc() would
 * always return failure.
 */

/*-- End of configurable params */

#define SECTORS_PER_PAGE_SHIFT	(PAGE_SHIFT - SECTOR_SHIFT)
#define SECTORS_PER_PAGE	(1 << SECTORS_PER_PAGE_SHIFT)
#define ZRAM_LOGICAL_BLOCK_SHIFT 12
#define ZRAM_LOGICAL_BLOCK_SIZE	(1 << ZRAM_LOGICAL_BLOCK_SHIFT)
#define ZRAM_SECTOR_PER_LOGICAL_BLOCK	\
	(1 << (ZRAM_LOGICAL_BLOCK_SHIFT - SECTOR_SHIFT))


/*
 * The lower ZRAM_FLAG_SHIFT bits of table.value is for
 * object size (excluding header), the higher bits is for
 * zram_pageflags.
 *
 * zram is mainly used for memory efficiency so we want to keep memory
 * footprint small so we can squeeze size and flags into a field.
 * The lower ZRAM_FLAG_SHIFT bits is for object size (excluding header),
 * the higher bits is for zram_pageflags.
 */
#define ZRAM_FLAG_SHIFT 24

/* Flags for zram pages (table[page_no].value) */
enum zram_pageflags {
	/* Page consists entirely of zeros */
	ZRAM_ZERO = ZRAM_FLAG_SHIFT,
	ZRAM_ACCESS,	/* page is now accessed */

	__NR_ZRAM_PAGEFLAGS,
};

/*-- Data structures */

/* Allocated for each disk page */
struct zram_table_entry {
	unsigned long handle;
	unsigned long value;
#ifdef CONFIG_PREEMPT_RT_BASE
	spinlock_t lock;
#endif
};

struct zram_stats {
	atomic64_t compr_data_size;	/* compressed size of pages stored */
	atomic64_t num_reads;	/* failed + successful */
	atomic64_t num_writes;	/* --do-- */
	atomic64_t failed_reads;	/* can happen when memory is too low */
	atomic64_t failed_writes;	/* can happen when memory is too low */
	atomic64_t invalid_io;	/* non-page-aligned I/O requests */
	atomic64_t notify_free;	/* no. of swap slot free notifications */
	atomic64_t zero_pages;		/* no. of zero filled pages */
	atomic64_t pages_stored;	/* no. of pages currently stored */
	atomic_long_t max_used_pages;	/* no. of maximum pages stored */
};

struct zram_meta {
	struct zram_table_entry *table;
	struct zs_pool *mem_pool;
};

struct zram {
	struct zram_meta *meta;
	struct zcomp *comp;
	struct gendisk *disk;
	/* Prevent concurrent execution of device init */
	struct rw_semaphore init_lock;
	/*
	 * the number of pages zram can consume for storing compressed data
	 */
	unsigned long limit_pages;
	int max_comp_streams;

	struct zram_stats stats;
	atomic_t refcount; /* refcount for zram_meta */
	/* wait all IO under all of cpu are done */
	wait_queue_head_t io_done;
	/*
	 * This is the limit on amount of *uncompressed* worth of data
	 * we can store in a disk.
	 */
	u64 disksize;	/* bytes */
	char compressor[10];
	/*
	 * zram is claimed so open request will be failed
	 */
	bool claim; /* Protected by bdev->bd_mutex */
};

#ifndef CONFIG_PREEMPT_RT_BASE
static inline void zram_lock_table(struct zram_table_entry *table)
{
	bit_spin_lock(ZRAM_ACCESS, &table->value);
}

static inline void zram_unlock_table(struct zram_table_entry *table)
{
	bit_spin_unlock(ZRAM_ACCESS, &table->value);
}

static inline void zram_meta_init_table_locks(struct zram_meta *meta, u64 disksize) { }
#else /* CONFIG_PREEMPT_RT_BASE */
static inline void zram_lock_table(struct zram_table_entry *table)
{
	spin_lock(&table->lock);
	__set_bit(ZRAM_ACCESS, &table->value);
}

static inline void zram_unlock_table(struct zram_table_entry *table)
{
	__clear_bit(ZRAM_ACCESS, &table->value);
	spin_unlock(&table->lock);
}

static inline void zram_meta_init_table_locks(struct zram_meta *meta, u64 disksize)
{
        size_t num_pages = disksize >> PAGE_SHIFT;
        size_t index;

        for (index = 0; index < num_pages; index++) {
		spinlock_t *lock = &meta->table[index].lock;
		spin_lock_init(lock);
        }
}
#endif /* CONFIG_PREEMPT_RT_BASE */

#endif
