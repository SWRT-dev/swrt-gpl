
#ifndef __NAND_DEVICE_LIST_H__
#define __NAND_DEVICE_LIST_H__

#define NAND_MAX_ID		5
#define CHIP_CNT		10
#define P_SIZE		16384
#define P_PER_BLK		256
#define C_SIZE		8192
#define RAMDOM_READ		(1<<0)
#define CACHE_READ		(1<<1)

#define RAND_TYPE_SAMSUNG 0
#define RAND_TYPE_TOSHIBA 1
#define RAND_TYPE_NONE 2

#define READ_RETRY_MAX 10
struct gFeature
{
	u32 address;
	u32 feature;
};

enum readRetryType
{
	RTYPE_MICRON,
	RTYPE_SANDISK,
	RTYPE_SANDISK_19NM,
	RTYPE_TOSHIBA,
	RTYPE_HYNIX,
	RTYPE_HYNIX_16NM
};

struct gFeatureSet
{
	u8 sfeatureCmd;
	u8 gfeatureCmd;
	u8 readRetryPreCmd;
	u8 readRetryCnt;
	u32 readRetryAddress;
	u32 readRetryDefault;
	u32 readRetryStart;
	enum readRetryType rtype;
	struct gFeature Interface;
	struct gFeature Async_timing;
};

struct gRandConfig
{
	u8 type;
	u32 seed[6];
};

enum pptbl
{
	MICRON_8K,
	HYNIX_8K,
	SANDISK_16K,
	PPTBL_NOT_SUPPORT,
};

struct MLC_feature_set
{
	enum pptbl ptbl_idx;
	struct gFeatureSet 	 FeatureSet;
	struct gRandConfig   randConfig;
};

enum flashdev_vendor
{
	VEND_SAMSUNG,
	VEND_MICRON,
	VEND_TOSHIBA,
	VEND_HYNIX,
	VEND_SANDISK,
	VEND_BIWIN,
	VEND_NONE,
};

enum flashdev_IOWidth
{
	IO_8BIT = 8,
	IO_16BIT = 16,
	IO_TOGGLEDDR = 9,
	IO_TOGGLESDR = 10,
	IO_ONFI = 12,
};

typedef struct
{
   u8 id[NAND_MAX_ID];
   u8 id_length;
   u8 addr_cycle;
   enum flashdev_IOWidth iowidth;
   u16 totalsize;
   u16 blocksize;
   u16 pagesize;
   u16 sparesize;
   u32 timmingsetting;
   u32 dqs_delay_ctrl;
   u32 s_acccon;
   u32 s_acccon1;
   u32 freq;
   enum flashdev_vendor vendor;
   u16 sectorsize;
   u8 devciename[30];
   u32 advancedmode;
   //struct MLC_feature_set feature_set;
}flashdev_info,*pflashdev_info;

static const flashdev_info gen_FlashTable[CHIP_CNT]={
	{{0xC2,0xA1,0x80,0x15,0x02}, 5,4,IO_8BIT,128,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "MX30UF1G18AC", 0},
	{{0xC2,0xAA,0x90,0x15,0x06}, 5,5,IO_8BIT,256,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "MX30UF2G18AC", 0},
	{{0xC2,0xAC,0x90,0x15,0x56}, 5,5,IO_8BIT,512,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "MX30UF4G18AB", 0},

	{{0xC8,0x81,0x80,0x15,0x40}, 5,4,IO_8BIT,128,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "F59D1G81MA", 0},
	/* {{0xC8,0xAA,0x90,0x15,0x44}, 5,5,IO_8BIT,256,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "F59D2G81A", 0}, */
	/* {{0xC8,0xAC,0x90,0x15,0x54}, 5,5,IO_8BIT,512,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "F59D4G81A", 0}, */

	{{0x01,0xA1,0x80,0x15,0x00}, 5,4,IO_8BIT,128,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "S34MS01G2", 0},
	{{0x01,0xAA,0x90,0x15,0x46}, 5,5,IO_8BIT,256,128,2048,112,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "S34MS02G2", 0},
	{{0x01,0xAC,0x90,0x15,0x56}, 5,5,IO_8BIT,512,128,2048,112,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "S34MS04G2", 0},

	{{0xEF,0xA1,0x00,0x95,0x00}, 5,4,IO_8BIT,128,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "W29N01HZSINF", 0},
	{{0xEF,0xAA,0x90,0x15,0x04}, 5,5,IO_8BIT,256,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "W29N02GZSIBA", 0},
	{{0xEF,0xAC,0x90,0x15,0x54}, 5,5,IO_8BIT,512,128,2048,64,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "W29N04GZSIBA", 0},

	/* {{0x98,0xA1,0x80,0x15,0x72}, 5,4,IO_8BIT,128,128,2048,112,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "TC58NYG0S3HTA00", 0}, */
	/* {{0x98,0xAA,0x90,0x15,0x76}, 5,5,IO_8BIT,256,128,2048,112,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "TC58NYG1S3HTA00", 0}, */
	/* {{0x98,0xAC,0x90,0x26,0x76}, 5,5,IO_8BIT,512,256,4096,224,0x10804211, 0x0, 0xC03222,0x101,80,VEND_NONE,512, "TC58NYG2S3HTA00", 0}, */
};

#endif
