#ifndef __TAIL_H__
#define __TAIL_H__
/*
 * all data in network byte order (aka natural aka bigendian)
 */

#define MAX_STRING 12
#ifdef TRX_NEW
#define MAX_VER 5
#else
#define MAX_VER 4
#endif
/* If hw[i].kernel == ROOTFS_OFFSET_MAGIC,
 * rootfilesystem offset (uImage header size + kernel size)
 * can be calculated by following equation:
 * (hw[i].minor << 16) | (hw[i+1].major << 8) | (hw[i+1].minor)
 */
#define ROOTFS_OFFSET_MAGIC	0xA9	/* Occupy two version_t		*/

typedef struct {
	uint8_t major;
	uint8_t minor; 
} version_t;

typedef struct {
	version_t kernel;
	version_t fs;
	char	  productid[MAX_STRING];
#ifdef TRX_NEW
       uint16_t  sn;
       uint16_t  en;
       uint8_t   pkey;
       uint8_t   key;
       version_t hw[MAX_VER];
#else
       version_t hw[MAX_VER*2];
#endif
} TAIL;

#endif	/* __TAIL_H__ */
