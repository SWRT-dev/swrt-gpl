#include "partition_define.h"
static const struct excel_info PartInfo_Private[PART_NUM]={			{"preloader",262144,0x0, NAND},
			{"uboot",524288,0x40000, NAND},
			{"config",262144,0xc0000, NAND},
			{"factory",262144,0x100000, NAND},
			{"bootimg",33554432,0x140000, NAND},
			{"recovery",33554432,0x2140000, NAND},
			{"rootfs",16777216,0x4140000, NAND},
			{"usrdata",16777216,0x5140000, NAND},
			{"bmtpool",20971520,0xFFFF0000, NAND},
 };

#ifdef  CONFIG_MTK_EMMC_SUPPORT
struct MBR_EBR_struct MBR_EBR_px[MBR_COUNT]={
	{"mbr", {}},
};

EXPORT_SYMBOL(MBR_EBR_px);
#endif

