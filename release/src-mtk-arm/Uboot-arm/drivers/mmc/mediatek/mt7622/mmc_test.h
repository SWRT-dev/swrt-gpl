#ifndef MMC_TEST_H
#define MMC_TEST_H

//#define MMC_PROFILING
//#define MMC_TEST

#include "mmc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mmc_test_config {
    char           *desc;           /* description */
    int             id;             /* host id to test */
    int             autocmd;        /* auto command */
    int             mode;           /* PIO/DMA mode */
    int             uhsmode;        /* UHS mode */
    int             burstsz;        /* DMA burst size */
    int             piobits;
    unsigned int    flags;          /* DMA flags */             
    int             count;          /* repeat counts */
    int             clksrc;         /* clock source */
    unsigned int    clock;          /* clock frequency for testing */
    unsigned int    buswidth;       /* bus width */
    unsigned long   blknr;          /* n'th block number for read/write test */
    unsigned int    total_size;     /* total size to test */
    unsigned int    blksz;          /* block size */
    int             chunk_blks;     /* blocks of chunk */
    char           *buf;            /* read/write buffer */
    unsigned char   chk_result;     /* check write result? */
    unsigned char   tst_single;     /* test single block read/write? */
    unsigned char   tst_multiple;   /* test multiple block read/write? */
    unsigned char   tst_interleave; /* test interleave block read/write ? */
};

struct mmc_op_report {
    unsigned long count;        /* the count of this operation */
    unsigned long min_time;     /* the min. time of this operation */
    unsigned long max_time;     /* the max. time of this operation */
    unsigned long total_time;   /* the total time of this operation */
    unsigned long total_size;   /* the total size of this operation */
};
struct mmc_op_perf {
    struct mmc_host      *host;
    struct mmc_card      *card;
    struct mmc_op_report  single_blk_read;
    struct mmc_op_report  single_blk_write;    
    struct mmc_op_report  multi_blks_read;
    struct mmc_op_report  multi_blks_write;
};

typedef void (*mmc_prof_callback)(void *data, ulong id, ulong counts);

#ifdef MMC_PROFILING
extern struct mmc_op_perf *mmc_prof_handle(int id);
extern void mmc_prof_init(int id, struct mmc_host *host, struct mmc_card *card);
extern void mmc_prof_start(void);
extern void mmc_prof_stop(void);
extern unsigned int mmc_prof_count(void);
extern void mmc_prof_update(mmc_prof_callback cb, ulong id, void *data);
extern void mmc_prof_report(struct mmc_op_report *rpt);
extern int mmc_prof_dump(int dev_id);
#else
#define mmc_prof_handle(i)      NULL
#define mmc_prof_init(i,h,c)    do{}while(0)
#define mmc_prof_count()        0
#define mmc_prof_start()        do{}while(0)
#define mmc_prof_stop()         do{}while(0)
#define mmc_prof_update(c,i,d)  do{}while(0)
#define mmc_prof_report(rpt)    do{}while(0)
#define mmc_prof_dump(id)       do{}while(0)
#endif

#ifdef MMC_TEST
extern int mmc_test(int argc, char *argv[]);
#else
#define mmc_test(c,v)           0
#endif

#ifdef __cplusplus
}
#endif

#endif /* MMC_TEST_H */

