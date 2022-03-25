#ifndef __PARTITION_DEFINE_H__
#define __PARTITION_DEFINE_H__




#define KB  (1024)
#define MB  (1024 * KB)
#define GB  (1024 * MB)

#define PART_PRELOADER "PRELOADER" 
#define PART_UBOOT "UBOOT" 
#define PART_CONFIG "CONFIG" 
#define PART_FACTORY "FACTORY" 
#define PART_BOOTIMG "BOOTIMG" 
#define PART_RECOVERY "RECOVERY" 
#define PART_ROOTFS "ROOTFS" 
#define PART_USRDATA "USRDATA" 
#define PART_BMTPOOL "BMTPOOL" 
/*preloader re-name*/
#define PART_USER "USER" 
/*Uboot re-name*/

#define PART_FLAG_NONE              0 
#define PART_FLAG_LEFT             0x1 
#define PART_FLAG_END              0x2 
#define PART_MAGIC              0x58881688 

#define PART_SIZE_PRELOADER			(256*KB)
#define PART_OFFSET_PRELOADER			(0x0)
#define PART_SIZE_UBOOT			(512*KB)
#define PART_OFFSET_UBOOT			(0x40000)
#define PART_SIZE_CONFIG			(256*KB)
#define PART_OFFSET_CONFIG			(0xc0000)
#define PART_SIZE_FACTORY			(256*KB)
#define PART_OFFSET_FACTORY			(0x100000)
#define PART_SIZE_BOOTIMG			(32768*KB)
#define PART_OFFSET_BOOTIMG			(0x140000)
#define PART_SIZE_RECOVERY			(32768*KB)
#define PART_OFFSET_RECOVERY			(0x2140000)
#define PART_SIZE_ROOTFS			(16384*KB)
#define PART_OFFSET_ROOTFS			(0x4140000)
#define PART_SIZE_USRDATA			(16384*KB)
#define PART_OFFSET_USRDATA			(0x5140000)
#define PART_SIZE_BMTPOOL			(0*KB)
#define PART_OFFSET_BMTPOOL			(0xFFFF0000)
#ifndef RAND_START_ADDR
#define RAND_START_ADDR   64
#endif


#define PART_NUM			9



#define PART_MAX_COUNT			 40

#define MBR_START_ADDRESS_BYTE			(*KB)

typedef enum  {
	EMMC = 1,
	NAND = 2,
} dev_type;

#if defined(MTK_EMMC_SUPPORT) || defined(CONFIG_MTK_EMMC_SUPPORT)
typedef enum {
	EMMC_PART_UNKNOWN=0
	,EMMC_PART_BOOT1
	,EMMC_PART_BOOT2
	,EMMC_PART_RPMB
	,EMMC_PART_GP1
	,EMMC_PART_GP2
	,EMMC_PART_GP3
	,EMMC_PART_GP4
	,EMMC_PART_USER
	,EMMC_PART_END
} Region;
#else
typedef enum {
NAND_PART_UNKNOWN=0
,NAND_PART_USER
} Region;
#endif

struct excel_info{
	char * name;
	unsigned long long size;
	unsigned long long start_address;
	dev_type type ;
	unsigned int partition_idx;
	Region region;
};

#if defined(MTK_EMMC_SUPPORT) || defined(CONFIG_MTK_EMMC_SUPPORT)
/*MBR or EBR struct*/
#define SLOT_PER_MBR 4
#define MBR_COUNT 8

struct MBR_EBR_struct{
	char part_name[8];
	int part_index[SLOT_PER_MBR];
};

extern struct MBR_EBR_struct MBR_EBR_px[MBR_COUNT];
#endif
extern struct excel_info *PartInfo;


#endif
