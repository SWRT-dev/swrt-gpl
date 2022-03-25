#ifndef CUST_PART_H
#define CUST_PART_H

#include "partition_define.h"

//#ifdef MTK_MLC_NAND_SUPPORT for slc and mlc
enum partition_type
{
	TYPE_LOW,
	TYPE_FULL,
};
//#endif

typedef union
{
    struct
    {
        unsigned int magic;     /* partition magic */
        unsigned int dsize;     /* partition data size */
        char name[32];          /* partition name */    
        unsigned int maddr;     /* partition memory address */
    } info;
    unsigned char data[512];
} part_hdr_t;

#ifdef MTK_EMMC_SUPPORT
typedef struct
{
    unsigned char *name;        /* partition name */
    unsigned long startblk;     /* partition start blk */
    unsigned long long size;         /* partition size */
    unsigned long blks;         /* partition blks */
    unsigned long flags;        /* partition flags */
    unsigned int part_id;
} part_t;
#else
typedef struct
{
    unsigned char *name;        /* partition name */
    unsigned long startblk;     /* partition start blk */
    unsigned long size;         /* partition size */
    unsigned long blks;         /* partition blks */
    unsigned long flags;        /* partition flags */
    unsigned int part_id;       // enum partition_type type;

	enum partition_type type;


} part_t;
#endif

extern void cust_part_init(void);
extern part_t *cust_part_tbl(void);

#endif /* CUST_PART_H */
