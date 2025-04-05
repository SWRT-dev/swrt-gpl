// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef _JITTERENTROPY_H
#define _JITTERENTROPY_H
#include <linux/types.h>

/* Flags that can be used to initialize the RNG */
#define JENT_DISABLE_STIR (1<<0) /* Disable stirring the entropy pool */
#define JENT_DISABLE_UNBIAS (1<<1) /* Disable the Von-Neuman Unbiaser */
#define JENT_DISABLE_MEMORY_ACCESS (1<<2) /* Disable memory access for more
					     entropy, saves MEMORY_SIZE RAM for
					     entropy collector */
#define DRIVER_NAME     "jitterentropy"

struct rand_data {
	/* all data values that are vital to maintain the security
	 * of the RNG are marked as SENSITIVE. A user must not
	 * access that information while the RNG executes its loops to
	 * calculate the next random value. */
	__u64 data;		/* SENSITIVE Actual random number */
	__u64 old_data;		/* SENSITIVE Previous random number */
	__u64 prev_time;	/* SENSITIVE Previous time stamp */
#define DATA_SIZE_BITS ((sizeof(__u64)) * 8)
	__u64 last_delta;	/* SENSITIVE stuck test */
	__s64 last_delta2;	/* SENSITIVE stuck test */
	unsigned int osr;	/* Oversample rate */
#define JENT_MEMORY_BLOCKS 64
#define JENT_MEMORY_BLOCKSIZE 32
#define JENT_MEMORY_ACCESSLOOPS 128
#define JENT_MEMORY_SIZE (JENT_MEMORY_BLOCKS*JENT_MEMORY_BLOCKSIZE)
	unsigned char *mem;	/* Memory access location with size of
				 * memblocks * memblocksize */
	unsigned int memlocation; /* Pointer to byte in *mem */
	unsigned int memblocks;	/* Number of memory blocks in *mem */
	unsigned int memblocksize; /* Size of one memory block in bytes */
	unsigned int memaccessloops; /* Number of memory accesses per random
				      * bit generation */

	/* Repetition Count Test */
	unsigned int rct_count;			/* Number of stuck values */

	/* Intermittent health test failure threshold of 2^-30 */
#define JENT_RCT_CUTOFF		30	/* Taken from SP800-90B sec 4.4.1 */
#define JENT_APT_CUTOFF		325	/* Taken from SP800-90B sec 4.4.2 */
	/* Permanent health test failure threshold of 2^-60 */
#define JENT_RCT_CUTOFF_PERMANENT	60
#define JENT_APT_CUTOFF_PERMANENT	355
#define JENT_APT_WINDOW_SIZE	512	/* Data window size */
	/* LSB of time stamp to process */
#define JENT_APT_LSB		16
#define JENT_APT_WORD_MASK	(JENT_APT_LSB - 1)
	unsigned int apt_observations;	/* Number of collected observations */
	unsigned int apt_count;		/* APT counter */
	unsigned int apt_base;		/* APT base reference */
	unsigned int apt_base_set:1;	/* APT base reference set? */
	unsigned int stir:1;		/* Post-processing stirring */
	unsigned int disable_unbias:1;	/* Deactivate Von-Neuman unbias */
};
extern void *jent_zalloc(unsigned int len);
extern void jent_zfree(void *ptr);
extern void jent_memcpy(void *dest, const void *src, unsigned int n);
extern void jent_get_nstime(__u64 *out);

struct rand_data;
extern int jent_entropy_init(void);
extern int jent_read_entropy(struct rand_data *ec, unsigned char *data,
			     unsigned int len);

extern struct rand_data *jent_entropy_collector_alloc(unsigned int osr,
						      unsigned int flags);
extern void jent_entropy_collector_free(struct rand_data *entropy_collector);

unsigned int jent_version(void);

void jent_lfsr_time(struct rand_data *ec, __u64 time, __u64 loop_cnt, int stuck);

int jent_measure_jitter(struct rand_data *ec);

#endif /* _JITTERENTROPY_H */
