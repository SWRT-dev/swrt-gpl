#include <common.h>
#include <linux/string.h>
#include <config.h>
#include <malloc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <asm/arch/nand/mtk_nand.h>
#include <asm/arch/nand/mtk_snand_k.h>
#include <asm/arch/nand/snand_device_list.h>
#include <asm/arch/timer.h>

/* NAND driver */
struct mtk_nand_host_hw {
	unsigned int nfi_bus_width;            /* NFI_BUS_WIDTH */
	unsigned int nfi_access_timing;        /* NFI_ACCESS_TIMING */
	unsigned int nfi_cs_num;               /* NFI_CS_NUM */
	unsigned int nand_sec_size;            /* NAND_SECTOR_SIZE */
	unsigned int nand_sec_shift;           /* NAND_SECTOR_SHIFT */
	unsigned int nand_ecc_size;
	unsigned int nand_ecc_bytes;
	unsigned int nand_ecc_mode;
};

#define NFI_DEFAULT_ACCESS_TIMING        0x10804211/* 0x30c77fff//0x44333 */
/* 0x10804211(used for Toshiba, MXIC, ESMT) */
#define NFI_CS_NUM                  	 (2)
struct mtk_nand_host_hw mtk_nand_hw = {
	.nfi_bus_width          	= 8,
	.nfi_access_timing		= NFI_DEFAULT_ACCESS_TIMING,
	.nfi_cs_num			= NFI_CS_NUM,
	.nand_sec_size			= 512,
	.nand_sec_shift			= 9,
	.nand_ecc_size			= 2048,
	.nand_ecc_bytes			= 32,
	.nand_ecc_mode			= NAND_ECC_HW,
};

#define NFI_DEFAULT_CS               (0)
#define mb() __asm__ __volatile__("": : :"memory")

#define MODULE_NAME	"# MTK SNAND #"
#define _MTK_NAND_DUMMY_DRIVER_
#define __INTERNAL_USE_AHB_MODE__ 	(1)

typedef enum
{
	  SNAND_RB_DEFAULT    = 0
	, SNAND_RB_READ_ID    = 1
	, SNAND_RB_CMD_STATUS = 2
	, SNAND_RB_PIO        = 3
} SNAND_Read_Byte_Mode;

typedef enum
{
	  SNAND_IDLE                 = 0
	, SNAND_DEV_ERASE_DONE
	, SNAND_DEV_PROG_DONE
	, SNAND_BUSY
	, SNAND_NFI_CUST_READING
	, SNAND_NFI_AUTO_ERASING
	, SNAND_DEV_PROG_EXECUTING
} SNAND_Status;

SNAND_Read_Byte_Mode    g_snand_read_byte_mode 	= SNAND_RB_DEFAULT;
SNAND_Status            g_snand_dev_status     	= SNAND_IDLE;    /* used for mtk_snand_dev_ready() and interrupt waiting service */
int                     g_snand_cmd_status      = NAND_STATUS_READY;

u8 g_snand_id_data[SNAND_MAX_ID + 1];
u8 g_snand_id_data_idx = 0;

/* Read Split related definitions and variables */
#define SNAND_RS_BOUNDARY_BLOCK                     (2)
#define SNAND_RS_BOUNDARY_KB                        (1024)
#define SNAND_RS_SPARE_PER_SECTOR_FIRST_PART_VAL    (16)                                        /* MT6572 shoud fix this as 16 */
#define SNAND_RS_SPARE_PER_SECTOR_FIRST_PART_NFI    (PAGEFMT_SPARE_16 << PAGEFMT_SPARE_SHIFT)   /* MT6572 shoud fix this as 16 */
#define SNAND_RS_ECC_BIT_FIRST_PART                 (4)                                         /* MT6572 shoud fix this as 4 */

u32 g_snand_rs_ecc_bit_second_part;
u32 g_snand_rs_spare_per_sector_second_part_nfi;
u32 g_snand_rs_num_page = 0;
u32 g_snand_rs_cur_part = 0xFFFFFFFF;

u32 g_snand_spare_per_sector = 0;   /* because Read Split feature will change spare_per_sector in run-time, thus use a variable to indicate current spare_per_sector */
#define NFI_WAIT_STATE_DONE(state) do {; } while (__raw_readl(NFI_STA_REG32) & state)
#define NFI_WAIT_TO_READY()  do {; } while (!(__raw_readl(NFI_STA_REG32) & STA_BUSY2READY))

#define NAND_SECTOR_SIZE    (512)
#define OOB_PER_SECTOR      (28)
#define OOB_AVAI_PER_SECTOR (8)
#define CACHE_ALIGNE_SIZE   (64)

__attribute__((aligned(CACHE_ALIGNE_SIZE))) unsigned char g_snand_spare[128];
__attribute__((aligned(CACHE_ALIGNE_SIZE))) unsigned char g_snand_temp[4096 + 128];

#if defined(MTK_COMBO_NAND_SUPPORT)
	/* BMT_POOL_SIZE is not used anymore */
#else
	#ifndef PART_SIZE_BMTPOOL
	#define BMT_POOL_SIZE (80)
	#else
	#define BMT_POOL_SIZE (PART_SIZE_BMTPOOL)
	#endif
#endif

u32 g_bmt_sz = BMT_POOL_SIZE;

/* NOTE(Bayi) */
#define GPIO_SNAND_BASE    0x10217000
#define GPIO_MODE(x) (GPIO_SNAND_BASE + 0x300 + 0x10 * x)
#define GPIO_DRIV(x) (GPIO_SNAND_BASE + 0x6600 + 0x10 * x)

/*******************************************************************************
 * Gloable Varible Definition
 *******************************************************************************/
struct nand_perf_log
{
	unsigned int ReadPageCount;
	suseconds_t  ReadPageTotalTime;
	unsigned int ReadBusyCount;
	suseconds_t  ReadBusyTotalTime;
	unsigned int ReadDMACount;
	suseconds_t  ReadDMATotalTime;

	unsigned int WritePageCount;
	suseconds_t  WritePageTotalTime;
	unsigned int WriteBusyCount;
	suseconds_t  WriteBusyTotalTime;
	unsigned int WriteDMACount;
	suseconds_t  WriteDMATotalTime;

	unsigned int EraseBlockCount;
	suseconds_t  EraseBlockTotalTime;

	unsigned int ReadSectorCount;
	suseconds_t  ReadSectorTotalTime;
};

#ifdef NAND_PFM
static suseconds_t g_PFM_R = 0;
static suseconds_t g_PFM_W = 0;
static suseconds_t g_PFM_E = 0;
static u32 g_PFM_RNum = 0;
static u32 g_PFM_RD = 0;
static u32 g_PFM_WD = 0;
static struct timeval g_now;

#define PFM_BEGIN(time) \
do_gettimeofday(&g_now); \
(time) = g_now;

#define PFM_END_R(time, n) \
do_gettimeofday(&g_now); \
g_PFM_R += (g_now.tv_sec * 1000000 + g_now.tv_usec) - (time.tv_sec * 1000000 + time.tv_usec); \
g_PFM_RNum += 1; \
g_PFM_RD += n; \
MSG(PERFORMANCE, "%s - Read PFM: %lu, data: %d, ReadOOB: %d (%d, %d)\n", MODULE_NAME, g_PFM_R, g_PFM_RD, g_kCMD.purereadoob, g_kCMD.purereadoobnum, g_PFM_RNum);

#define PFM_END_W(time, n) \
do_gettimeofday(&g_now); \
g_PFM_W += (g_now.tv_sec * 1000000 + g_now.tv_usec) - (time.tv_sec * 1000000 + time.tv_usec); \
g_PFM_WD += n; \
MSG(PERFORMANCE, "%s - Write PFM: %lu, data: %d\n", MODULE_NAME, g_PFM_W, g_PFM_WD);

#define PFM_END_E(time) \
do_gettimeofday(&g_now); \
g_PFM_E += (g_now.tv_sec * 1000000 + g_now.tv_usec) - (time.tv_sec * 1000000 + time.tv_usec); \
MSG(PERFORMANCE, "%s - Erase PFM: %lu\n", MODULE_NAME, g_PFM_E);
#else
#define PFM_BEGIN(time)
#define PFM_END_R(time, n)
#define PFM_END_W(time, n)
#define PFM_END_E(time)
#endif

#define TIMEOUT_1   0x1fff
#define TIMEOUT_2   0x8ff
#define TIMEOUT_3   0xffff
#define TIMEOUT_4   0xffff      /* 5000   //PIO */

#define NFI_ISSUE_COMMAND(cmd, col_addr, row_addr, col_num, row_num) \
	do { \
		DRV_WriteReg(NFI_CMD_REG16, cmd); \
		while (DRV_Reg32(NFI_STA_REG32) & STA_CMD_STATE); \
		DRV_WriteReg32(NFI_COLADDR_REG32, col_addr); \
		DRV_WriteReg32(NFI_ROWADDR_REG32, row_addr); \
		DRV_WriteReg(NFI_ADDRNOB_REG16, col_num | (row_num << ADDR_ROW_NOB_SHIFT)); \
		while (DRV_Reg32(NFI_STA_REG32) & STA_ADDR_STATE); \
	} while (0);

/* ------------------------------------------------------------------------------- */
static struct NAND_CMD g_kCMD;
bool g_bInitDone;
static u32 g_value = 0;
static int g_page_size;
static int g_block_size;

#if __INTERNAL_USE_AHB_MODE__
unsigned char g_bHwEcc = true;
#else
unsigned char g_bHwEcc = false;
#endif

#define SNAND_MAX_PAGE_SIZE	(4096)
#define _SNAND_CACHE_LINE_SIZE  (256)

static u8 *local_buffer_16_align;   /* 16 byte aligned buffer, for HW issue */
__attribute__((aligned(_SNAND_CACHE_LINE_SIZE))) static u8 local_buffer[SNAND_MAX_PAGE_SIZE + _SNAND_CACHE_LINE_SIZE];

extern void nand_release_device(struct mtd_info *mtd);
extern int nand_get_device(struct nand_chip *chip, struct mtd_info *mtd, int new_state);
void mtk_snand_reset(void);

struct mtk_nand_host *host;
static u8 g_running_dma = 0;
#ifdef DUMP_NATIVE_BACKTRACE
static u32 g_dump_count = 0;
#endif

int manu_id;
int dev_id;

static u8 local_oob_buf[NAND_MAX_OOBSIZE];

#ifdef _MTK_NAND_DUMMY_DRIVER_
int dummy_driver_debug;
#endif

void nand_enable_clock(void)
{
}

void nand_disable_clock(void)
{
}

static struct nand_ecclayout nand_oob_16 = {
	.eccbytes = 8,
	.eccpos = {8, 9, 10, 11, 12, 13, 14, 15},
	.oobfree = {{1, 6}, {0, 0}}
};

struct nand_ecclayout nand_oob_64 = {
	.eccbytes = 32,
	.eccpos = {32, 33, 34, 35, 36, 37, 38, 39,
	           40, 41, 42, 43, 44, 45, 46, 47,
	           48, 49, 50, 51, 52, 53, 54, 55,
	           56, 57, 58, 59, 60, 61, 62, 63},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 6}, {0, 0}}
};

struct nand_ecclayout nand_oob_128 = {
	.eccbytes = 64,
	.eccpos = {
		64, 65, 66, 67, 68, 69, 70, 71,
		72, 73, 74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 86,
		88, 89, 90, 91, 92, 93, 94, 95,
		96, 97, 98, 99, 100, 101, 102, 103,
		104, 105, 106, 107, 108, 109, 110, 111,
		112, 113, 114, 115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127},
	.oobfree = {{1, 7}, {9, 7}, {17, 7}, {25, 7}, {33, 7}, {41, 7}, {49, 7}, {57, 6}}
};

snand_flashdev_info devinfo;

void dump_nfi(void)
{
#if __DEBUG_NAND
	MSG(INIT, "~~~~Dump NFI/SNF/GPIO Register in Kernel~~~~\n");
	MSG(INIT, "NFI_CNFG_REG16: 0x%x\n", DRV_Reg16(NFI_CNFG_REG16));
	MSG(INIT, "NFI_PAGEFMT_REG16: 0x%x\n", DRV_Reg16(NFI_PAGEFMT_REG16));
	MSG(INIT, "NFI_CON_REG16: 0x%x\n", DRV_Reg16(NFI_CON_REG16));
	MSG(INIT, "NFI_ACCCON_REG32: 0x%x\n", DRV_Reg32(NFI_ACCCON_REG32));
	MSG(INIT, "NFI_INTR_EN_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_EN_REG16));
	MSG(INIT, "NFI_INTR_REG16: 0x%x\n", DRV_Reg16(NFI_INTR_REG16));
	MSG(INIT, "NFI_CMD_REG16: 0x%x\n", DRV_Reg16(NFI_CMD_REG16));
	MSG(INIT, "NFI_ADDRNOB_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRNOB_REG16));
	MSG(INIT, "NFI_COLADDR_REG32: 0x%x\n", DRV_Reg32(NFI_COLADDR_REG32));
	MSG(INIT, "NFI_ROWADDR_REG32: 0x%x\n", DRV_Reg32(NFI_ROWADDR_REG32));
	MSG(INIT, "NFI_STRDATA_REG16: 0x%x\n", DRV_Reg16(NFI_STRDATA_REG16));
	MSG(INIT, "NFI_DATAW_REG32: 0x%x\n", DRV_Reg32(NFI_DATAW_REG32));
	MSG(INIT, "NFI_DATAR_REG32: 0x%x\n", DRV_Reg32(NFI_DATAR_REG32));
	MSG(INIT, "NFI_PIO_DIRDY_REG16: 0x%x\n", DRV_Reg16(NFI_PIO_DIRDY_REG16));
	MSG(INIT, "NFI_STA_REG32: 0x%x\n", DRV_Reg32(NFI_STA_REG32));
	MSG(INIT, "NFI_FIFOSTA_REG16: 0x%x\n", DRV_Reg16(NFI_FIFOSTA_REG16));
	MSG(INIT, "NFI_LOCKSTA_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKSTA_REG16));
	MSG(INIT, "NFI_ADDRCNTR_REG16: 0x%x\n", DRV_Reg16(NFI_ADDRCNTR_REG16));
	MSG(INIT, "NFI_STRADDR_REG32: 0x%x\n", DRV_Reg32(NFI_STRADDR_REG32));
	MSG(INIT, "NFI_BYTELEN_REG16: 0x%x\n", DRV_Reg16(NFI_BYTELEN_REG16));
	MSG(INIT, "NFI_CSEL_REG16: 0x%x\n", DRV_Reg16(NFI_CSEL_REG16));
	MSG(INIT, "NFI_IOCON_REG16: 0x%x\n", DRV_Reg16(NFI_IOCON_REG16));
	MSG(INIT, "NFI_FDM0L_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0L_REG32));
	MSG(INIT, "NFI_FDM0M_REG32: 0x%x\n", DRV_Reg32(NFI_FDM0M_REG32));
	MSG(INIT, "NFI_LOCK_REG16: 0x%x\n", DRV_Reg16(NFI_LOCK_REG16));
	MSG(INIT, "NFI_LOCKCON_REG32: 0x%x\n", DRV_Reg32(NFI_LOCKCON_REG32));
	MSG(INIT, "NFI_LOCKANOB_REG16: 0x%x\n", DRV_Reg16(NFI_LOCKANOB_REG16));
	MSG(INIT, "NFI_FIFODATA0_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA0_REG32));
	MSG(INIT, "NFI_FIFODATA1_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA1_REG32));
	MSG(INIT, "NFI_FIFODATA2_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA2_REG32));
	MSG(INIT, "NFI_FIFODATA3_REG32: 0x%x\n", DRV_Reg32(NFI_FIFODATA3_REG32));
	MSG(INIT, "NFI_MASTERSTA_REG16: 0x%x\n", DRV_Reg16(NFI_MASTERSTA_REG16));
	MSG(INIT, "NFI_SPIADDRCNTR_REG32: 0x%x\n", DRV_Reg32(NFI_SPIADDRCNTR_REG32));
	MSG(INIT, "NFI_SPIBYTELEN_REG32: 0x%x\n", DRV_Reg32(NFI_SPIBYTELEN_REG32));

	MSG(INIT, "RW_SNAND_MAC_CTL: 0x%x\n", DRV_Reg32(RW_SNAND_MAC_CTL));
	MSG(INIT, "RW_SNAND_MAC_OUTL: 0x%x\n", DRV_Reg32(RW_SNAND_MAC_OUTL));
	MSG(INIT, "RW_SNAND_MAC_INL: 0x%x\n", DRV_Reg32(RW_SNAND_MAC_INL));

	MSG(INIT, "RW_SNAND_RD_CTL1: 0x%x\n", DRV_Reg32(RW_SNAND_RD_CTL1));
	MSG(INIT, "RW_SNAND_RD_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_RD_CTL2));
	MSG(INIT, "RW_SNAND_RD_CTL3: 0x%x\n", DRV_Reg32(RW_SNAND_RD_CTL3));

	MSG(INIT, "RW_SNAND_GF_CTL1: 0x%x\n", DRV_Reg32(RW_SNAND_GF_CTL1));
	MSG(INIT, "RW_SNAND_GF_CTL3: 0x%x\n", DRV_Reg32(RW_SNAND_GF_CTL3));

	MSG(INIT, "RW_SNAND_PG_CTL1: 0x%x\n", DRV_Reg32(RW_SNAND_PG_CTL1));
	MSG(INIT, "RW_SNAND_PG_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_PG_CTL2));
	MSG(INIT, "RW_SNAND_PG_CTL3: 0x%x\n", DRV_Reg32(RW_SNAND_PG_CTL3));

	MSG(INIT, "RW_SNAND_ER_CTL: 0x%x\n", DRV_Reg32(RW_SNAND_ER_CTL));
	MSG(INIT, "RW_SNAND_ER_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_ER_CTL2));

	MSG(INIT, "RW_SNAND_MISC_CTL: 0x%x\n", DRV_Reg32(RW_SNAND_MISC_CTL));
	MSG(INIT, "RW_SNAND_MISC_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_MISC_CTL2));

	MSG(INIT, "RW_SNAND_DLY_CTL1: 0x%x\n", DRV_Reg32(RW_SNAND_DLY_CTL1));
	MSG(INIT, "RW_SNAND_DLY_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_DLY_CTL2));
	MSG(INIT, "RW_SNAND_DLY_CTL3: 0x%x\n", DRV_Reg32(RW_SNAND_DLY_CTL3));
	MSG(INIT, "RW_SNAND_DLY_CTL4: 0x%x\n", DRV_Reg32(RW_SNAND_DLY_CTL4));

	MSG(INIT, "RW_SNAND_STA_CTL1: 0x%x\n", DRV_Reg32(RW_SNAND_STA_CTL1));
	MSG(INIT, "RW_SNAND_STA_CTL2: 0x%x\n", DRV_Reg32(RW_SNAND_STA_CTL2));
	MSG(INIT, "RW_SNAND_STA_CTL3: 0x%x\n", DRV_Reg32(RW_SNAND_STA_CTL3));

	MSG(INIT, "RW_SNAND_CNFG: 0x%x\n", DRV_Reg32(RW_SNAND_CNFG));
#endif
}

/* No bmt mechanism in lerpard, so define empty function */
int init_bmt(struct nand_chip * chip,  int size)
{
	return 0;
}

void  bm_swap(struct mtd_info *mtd, u8 *fdmbuf, u8 *buf)
{
	u8 *psw1, *psw2, tmp;
	int sec_num = mtd->writesize / 512;

	/* the data */
	psw1 = buf + mtd->writesize - ((mtd->oobsize * (sec_num - 1) / sec_num));
	/* the first FDM byte of the last sector */
	psw2 = fdmbuf + (8 * (sec_num - 1));

	tmp = *psw1;
	*psw1 = *psw2;
	*psw2 = tmp;
}

bool mtk_snand_get_device_info(u8 *id, snand_flashdev_info *devinfo)
{
	u32 i, m, n, mismatch;
	int target=-1;
	u8 target_id_len = 0;

	for (i = 0; i < ARRAY_SIZE(gen_snand_FlashTable); i++) {
		mismatch = 0;

		for (m = 0; m < gen_snand_FlashTable[i].id_length; m++) {
			if (id[m]!=gen_snand_FlashTable[i].id[m]) {
				mismatch = 1;
				break;
			}
		}

		if (mismatch == 0 && gen_snand_FlashTable[i].id_length > target_id_len) {
				target = i;
				target_id_len = gen_snand_FlashTable[i].id_length;
		}
	}

	if (target != -1) {
		MSG(INIT, "Recognize SNAND: ID [");

		for (n = 0; n < gen_snand_FlashTable[target].id_length; n++) {
			devinfo->id[n] = gen_snand_FlashTable[target].id[n];
			MSG(INIT, "%x ", devinfo->id[n]);
		}

		MSG(INIT, "], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]MB\n", gen_snand_FlashTable[target].devicename, gen_snand_FlashTable[target].pagesize, gen_snand_FlashTable[target].sparesize, gen_snand_FlashTable[target].totalsize);
		devinfo->id_length = gen_snand_FlashTable[target].id_length;
		devinfo->blocksize = gen_snand_FlashTable[target].blocksize;    /* KB */
		devinfo->advancedmode = gen_snand_FlashTable[target].advancedmode;
		devinfo->pagesize = gen_snand_FlashTable[target].pagesize;
		devinfo->sparesize = gen_snand_FlashTable[target].sparesize;
		devinfo->totalsize = gen_snand_FlashTable[target].totalsize;

		memcpy(devinfo->devicename, gen_snand_FlashTable[target].devicename, sizeof(devinfo->devicename));

		devinfo->SNF_DLY_CTL1 = gen_snand_FlashTable[target].SNF_DLY_CTL1;
		devinfo->SNF_DLY_CTL2 = gen_snand_FlashTable[target].SNF_DLY_CTL2;
		devinfo->SNF_DLY_CTL3 = gen_snand_FlashTable[target].SNF_DLY_CTL3;
		devinfo->SNF_DLY_CTL4 = gen_snand_FlashTable[target].SNF_DLY_CTL4;
		devinfo->SNF_MISC_CTL = gen_snand_FlashTable[target].SNF_MISC_CTL;
		devinfo->SNF_DRIVING = gen_snand_FlashTable[target].SNF_DRIVING;

		/* init read split boundary */
		/* g_snand_rs_num_page = SNAND_RS_BOUNDARY_BLOCK * ((devinfo->blocksize << 10) / devinfo->pagesize); */
		g_snand_rs_num_page = SNAND_RS_BOUNDARY_KB * 1024 / devinfo->pagesize;
		g_snand_spare_per_sector = devinfo->sparesize / (devinfo->pagesize / NAND_SECTOR_SIZE);

		return true;
	} else {
		MSG(INIT, "Not Found NAND: ID [");
		for (n = 0; n < SNAND_MAX_ID; n++) {
			MSG(INIT, "%x ", id[n]);
		}
		MSG(INIT, "]\n");
		return false;
	}
}

bool mtk_snand_is_vendor_reserved_blocks(u32 row_addr)
{
	u32 page_per_block = (devinfo.blocksize << 10) / devinfo.pagesize;
	u32 target_block = row_addr / page_per_block;

	if (devinfo.advancedmode & SNAND_ADV_VENDOR_RESERVED_BLOCKS)
		if (target_block >= 2045 && target_block <= 2048)
			return true;

	return false;
}

static void mtk_snand_wait_us(u32 us)
{
	udelay(us);
}

static void mtk_snand_dev_mac_enable(SNAND_Mode mode)
{
	u32 mac;

	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/* SPI */
	if (mode == SPI) {
		mac &= ~SNAND_MAC_SIO_SEL;   /* Clear SIO_SEL to send command in SPI style */
		mac |= SNAND_MAC_EN;         /* Enable Macro Mode */
	}
	/* QPI */
	else {
		/*
		 * SFI V2: QPI_EN only effects direct read mode, and it is moved into DIRECT_CTL in V2
		 *         There's no need to clear the bit again.
		 */
		mac |= (SNAND_MAC_SIO_SEL | SNAND_MAC_EN);  /* Set SIO_SEL to send command in QPI style, and enable Macro Mode */
	}

	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);
}

/**
 * @brief This funciton triggers SFI to issue command to serial flash, wait SFI until ready.
 *
 * @remarks: !NOTE! This function must be used with mtk_snand_dev_mac_enable in pair!
 */
static void mtk_snand_dev_mac_trigger(void)
{
	u32 mac;

	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/* Trigger SFI: Set TRIG and enable Macro mode */
	mac |= (SNAND_TRIG | SNAND_MAC_EN);
	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);

	/*
	 * Wait for SFI ready
	 * Step 1. Wait for WIP_READY becoming 1 (WIP register is ready)
	 */
	while (!(DRV_Reg32(RW_SNAND_MAC_CTL) & SNAND_WIP_READY));

	/*
	 * Step 2. Wait for WIP becoming 0 (Controller finishes command write process)
	 */
	while ((DRV_Reg32(RW_SNAND_MAC_CTL) & SNAND_WIP));

}

/**
 * @brief This funciton leaves Macro mode and enters Direct Read mode
 *
 * @remarks: !NOTE! This function must be used after mtk_snand_dev_mac_trigger
 */
static void mtk_snand_dev_mac_leave(void)
{
	u32 mac;

	/* clear SF_TRIG and leave mac mode */
	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/*
	 * Clear following bits
	 * SF_TRIG: Confirm the macro command sequence is completed
	 * SNAND_MAC_EN: Leaves macro mode, and enters direct read mode
	 * SNAND_MAC_SIO_SEL: Always reset quad macro control bit at the end
	 */
	mac &= ~(SNAND_TRIG | SNAND_MAC_EN | SNAND_MAC_SIO_SEL);
	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);
}

static void mtk_snand_dev_mac_op(SNAND_Mode mode)
{
	mtk_snand_dev_mac_enable(mode);
	mtk_snand_dev_mac_trigger();
	mtk_snand_dev_mac_leave();
}

static void mtk_snand_dev_command_ext(SNAND_Mode mode, const u8 cmd[], u8 data[], const u32 outl, const u32 inl)
{
	u32   tmp;
	u32   i, j, p_data;
	volatile u8 *p_tmp;

	p_tmp = (u8 *)(&tmp);

	/* Moving commands into SFI GPRAM */
	for (i = 0, p_data = RW_SNAND_GPRAM_DATA; i < outl; p_data += 4) {
		/* Using 4 bytes aligned copy, by moving the data into the temp buffer and then write to GPRAM */
		for (j = 0, tmp = 0; i < outl && j < 4; i++, j++) {
			p_tmp[j] = cmd[i];
		}

		DRV_WriteReg32(p_data, tmp);
	}

	DRV_WriteReg32(RW_SNAND_MAC_OUTL, outl);
	DRV_WriteReg32(RW_SNAND_MAC_INL, inl);
	mtk_snand_dev_mac_op(mode);

	/* for NULL data, this loop will be skipped */
	for (i = 0, p_data = RW_SNAND_GPRAM_DATA + outl; i < inl; ++i, ++data, ++p_data)
		*data = DRV_Reg8(p_data);

	return;
}

static void mtk_snand_dev_command(const u32 cmd, u8 outlen)
{
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, outlen);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 0);
	mtk_snand_dev_mac_op(SPI);

	return;
}

static void mtk_snand_reset_dev(void)
{
	u8 cmd = SNAND_CMD_SW_RESET;

	/* issue SW RESET command to device */
	mtk_snand_dev_command_ext(SPI, &cmd, NULL, 1, 0);

	/* wait for awhile, then polling status register (required by spec) */
	mtk_snand_wait_us(SNAND_DEV_RESET_LATENCY_US);

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8));
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	/* polling status register */

	for (; ;) {
		mtk_snand_dev_mac_op(SPI);

		cmd = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if (0 == (cmd & SNAND_STATUS_OIP)) {
			break;
		}
	}
}

static u32 mtk_snand_reverse_byte_order(u32 num)
{
	u32 ret = 0;

	ret |= ((num >> 24) & 0x000000FF);
	ret |= ((num >> 8)  & 0x0000FF00);
	ret |= ((num << 8)  & 0x00FF0000);
	ret |= ((num << 24) & 0xFF000000);

	return ret;
}

static u32 mtk_snand_gen_c1a3(const u32 cmd, const u32 address)
{
	return ((mtk_snand_reverse_byte_order(address) & 0xFFFFFF00) | (cmd & 0xFF));
}

static void mtk_snand_dev_enable_spiq(bool enable)
{
	u8   regval;
	u32  cmd;

	/* read QE in status register */
	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	mtk_snand_dev_mac_op(SPI);

	regval = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);
	/* MSG(INIT, "[SNF] [Cheng] read status register[configuration register]:0x%x \n\r", regval); */

	if (!enable)    /* disable x4 */
	{
		if ((regval & SNAND_OTP_QE) == 0) {
			return;
		} else {
			regval = regval & ~SNAND_OTP_QE;
		}
	} else    /* enable x4 */
	{
		if ((regval & SNAND_OTP_QE) == 1) {
			return;
		} else {
			regval = regval | SNAND_OTP_QE;
		}
	}

	/* if goes here, it means QE needs to be set as new different value */

	/* write status register */
	cmd = SNAND_CMD_SET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8) | (regval << 16);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 3);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 0);

	mtk_snand_dev_mac_op(SPI);
}

static void mtk_snand_dev_die_select_op(u8 die_id)
{
	u32  cmd;

	cmd = SNAND_CMD_DIE_SELECT | (die_id << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL , 0);

	mtk_snand_dev_mac_op(SPI);
}

static u32 mtk_snand_dev_die_select(struct mtd_info *mtd, u32 page)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	u32 total_blocks = devinfo.totalsize << (20 - chip->phys_erase_shift);
	u16 page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	u16 page_in_block = page % page_per_block;
	u32 block = page / page_per_block;

	/* Die Select operation */
	if (devinfo.advancedmode & SNAND_ADV_TWO_DIE) {
		if (block >= total_blocks >> 1) {
			mtk_snand_dev_die_select_op(1);
			block -= total_blocks >> 1;
		} else {
			mtk_snand_dev_die_select_op(0);
		}
	}

	return (block * page_per_block + page_in_block);
}

/* Read Split related APIs */
static bool mtk_snand_rs_if_require_split(void) /* must be executed after snand_rs_reconfig_nfiecc() */
{
	if (devinfo.advancedmode & SNAND_ADV_READ_SPLIT) {
		if (g_snand_rs_cur_part != 0) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
/* Bayi: temp solution for add mtd to param */
static void mtk_snand_rs_reconfig_nfiecc(struct mtd_info *mtd, u32 row_addr)
{
	/* 1. only decode part should be re-configured */
	/* 2. only re-configure essential part (fixed register will not be re-configured) */

	u16 reg16;
	u32 ecc_bit;
	u32 ecc_bit_cfg = ECC_CNFG_ECC4;
	u32 decode_size;
	/* /struct mtd_info * mtd = &host->mtd; */

	if (0 == (devinfo.advancedmode & SNAND_ADV_READ_SPLIT)) {
		return;
	}

	if (row_addr < g_snand_rs_num_page) {
		if (g_snand_rs_cur_part != 0) {
			ecc_bit = SNAND_RS_ECC_BIT_FIRST_PART;

			reg16 = DRV_Reg16(NFI_PAGEFMT_REG16);
			reg16 &= ~PAGEFMT_SPARE_MASK;
			reg16 |= SNAND_RS_SPARE_PER_SECTOR_FIRST_PART_NFI;
			DRV_WriteReg16(NFI_PAGEFMT_REG16, reg16);

			g_snand_spare_per_sector = SNAND_RS_SPARE_PER_SECTOR_FIRST_PART_VAL;

			g_snand_rs_cur_part = 0;
		} else {
			return;
		}
	} else {
		if (g_snand_rs_cur_part != 1) {
			ecc_bit = g_snand_rs_ecc_bit_second_part;

			reg16 = DRV_Reg16(NFI_PAGEFMT_REG16);
			reg16 &= ~PAGEFMT_SPARE_MASK;
			reg16 |= g_snand_rs_spare_per_sector_second_part_nfi;
			DRV_WriteReg16(NFI_PAGEFMT_REG16, reg16);

			g_snand_spare_per_sector = mtd->oobsize / (mtd->writesize / NAND_SECTOR_SIZE);

			g_snand_rs_cur_part = 1;
		} else {
			return;
		}
	}

	switch (ecc_bit) {
		case 4:
			ecc_bit_cfg = ECC_CNFG_ECC4;
			break;
		case 8:
			ecc_bit_cfg = ECC_CNFG_ECC8;
			break;
		case 10:
			ecc_bit_cfg = ECC_CNFG_ECC10;
			break;
		case 12:
			ecc_bit_cfg = ECC_CNFG_ECC12;
			break;
		default:
			break;
	}
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
	do
	{;
	}
	while (!DRV_Reg16(ECC_DECIDLE_REG16));

	decode_size = ((NAND_SECTOR_SIZE + 1/*OOB_AVAI_PER_SECTOR*/) << 3) + ecc_bit * 13;

	/* configure ECC decoder && encoder */
	DRV_WriteReg32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT | ecc_bit_cfg | DEC_CNFG_NFI | DEC_CNFG_EMPTY_EN | (decode_size << DEC_CNFG_CODE_SHIFT));
}


/******************************************************************************
 * mtk_snand_ecc_config
 *
 * DESCRIPTION:
 *   Configure HW ECC!
 *
 * PARAMETERS:
 *   struct mtk_nand_host_hw *hw
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_config(struct mtk_nand_host_hw *hw, u32 ecc_bit)
{
	u32 encode_size;
	u32 decode_size;
	u32 ecc_bit_cfg = ECC_CNFG_ECC4;

	switch (ecc_bit) {
	case 4:
		ecc_bit_cfg = ECC_CNFG_ECC4;
		break;
	case 8:
		ecc_bit_cfg = ECC_CNFG_ECC8;
		break;
	case 10:
		ecc_bit_cfg = ECC_CNFG_ECC10;
		break;
	case 12:
		ecc_bit_cfg = ECC_CNFG_ECC12;
		break;
		default:
			break;
	}
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
	do
	{;
	}
	while (!DRV_Reg16(ECC_DECIDLE_REG16));

	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
	do
	{;
	}
	while (!DRV_Reg32(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	DRV_WriteReg32(ECC_FDMADDR_REG32, NFI_FDM0L_REG32);

	/* Sector + FDM */
	encode_size = (hw->nand_sec_size + 1/* [bayi note]: use 1 for upstream format. 8*/) << 3;
	/* Sector + FDM + YAFFS2 meta data bits */
	decode_size = ((hw->nand_sec_size + 1) << 3) + ecc_bit * 13;

	/* configure ECC decoder && encoder */
	DRV_WriteReg32(ECC_DECCNFG_REG32, ecc_bit_cfg | DEC_CNFG_NFI | DEC_CNFG_EMPTY_EN | (decode_size << DEC_CNFG_CODE_SHIFT));

	DRV_WriteReg32(ECC_ENCCNFG_REG32, ecc_bit_cfg | ENC_CNFG_NFI | (encode_size << ENC_CNFG_MSG_SHIFT));
#ifndef MANUAL_CORRECT
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);
#else
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_EL);
#endif
}

/******************************************************************************
 * mtk_snand_ecc_decode_start
 *
 * DESCRIPTION:
 *   HW ECC Decode Start !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_decode_start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) ;
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);
}

/******************************************************************************
 * mtk_snand_ecc_decode_end
 *
 * DESCRIPTION:
 *   HW ECC Decode End !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_decode_end(void)
{
	u32 timeout = 0xFFFF;

	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) {
		timeout--;

		if (timeout == 0)   /* Stanley Chu (need check timeout value again) */
		{
			break;
		}
	}

	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
}

/******************************************************************************
 * mtk_snand_ecc_encode_start
 *
 * DESCRIPTION:
 *   HW ECC Encode Start !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_encode_start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE)) ;
	mb();
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_EN);
}

/******************************************************************************
 * mtk_snand_ecc_encode_end
 *
 * DESCRIPTION:
 *   HW ECC Encode End !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_encode_end(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg32(ECC_ENCIDLE_REG32) & ENC_IDLE)) ;
	mb();
	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
}

/******************************************************************************
 * mtk_snand_check_bch_error
 *
 * DESCRIPTION:
 *   Check BCH error or not !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd
 *	 u8* databuf
 *	 u32 u4SecIndex
 *	 u32 u4PageAddr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_check_bch_error(struct mtd_info *mtd, u8 *databuf, u8 *spareBuf, u32 u4SecIndex, u32 u4PageAddr)
{
	bool ret = true;
	u16 sector_done_mask = 1 << u4SecIndex;
	u32 error_num_debug0, error_num_debug1, i, err_num;
	u32 timeout = 0xFFFF;
	u32 correct_count = 0;
	u16 failed_sec = 0;

#ifdef MANUAL_CORRECT
	u32 error_bit_loc[6];
	u32 error_num_debug;
	u32 error_byte_loc, bit_offset;
	u32 error_bit_loc_1th, error_bit_loc_2nd;
#endif

	while (0 == (sector_done_mask & DRV_Reg16(ECC_DECDONE_REG16))) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
#ifndef MANUAL_CORRECT
	error_num_debug0 = DRV_Reg32(ECC_DECENUM0_REG32);
	error_num_debug1 = DRV_Reg32(ECC_DECENUM1_REG32);
	if (0 != (error_num_debug0 & 0xFFFFF) || 0 != (error_num_debug1 & 0xFFFFF)) {
		for (i = 0; i <= u4SecIndex; ++i) {
			if (i < 4) {
				err_num = DRV_Reg32(ECC_DECENUM0_REG32) >> (i * 5);
			} else {
				err_num = DRV_Reg32(ECC_DECENUM1_REG32) >> ((i - 4) * 5);
			}
			err_num &= 0x1F;
		failed_sec =0;

			if (0x1F == err_num) {
				failed_sec++;
				ret = false;
				MSG(INIT, "[%s] ECC-U, PA=%d, S=%d\n", __func__, u4PageAddr, i);
			} else {
				if (err_num)
					correct_count += err_num;
			}
		}

		mtd->ecc_stats.failed += failed_sec;
		if (correct_count > 2 && ret)
			mtd->ecc_stats.corrected++;
	}
#else
	/* We will manually correct the error bits in the last sector, not all the sectors of the page! */
	memset(error_bit_loc, 0x0, sizeof(error_bit_loc));
	error_num_debug = DRV_Reg32(ECC_DECENUM_REG32);
	err_num = DRV_Reg32(ECC_DECENUM_REG32) >> (u4SecIndex << 2);
	err_num &= 0xF;

	if (err_num) {
		if (0xF == err_num) {
			mtd->ecc_stats.failed++;
			ret = false;
			/* MSG(INIT, "UnCorrectable at PageAddr=%d\n", u4PageAddr); */
		} else {
			for (i = 0; i < ((err_num + 1) >> 1); ++i) {
				error_bit_loc[i] = DRV_Reg32(ECC_DECEL0_REG32 + i);
				error_bit_loc_1th = error_bit_loc[i] & 0x1FFF;

				if (error_bit_loc_1th < 0x1000) {
					error_byte_loc = error_bit_loc_1th / 8;
					bit_offset = error_bit_loc_1th % 8;
					databuf[error_byte_loc] = databuf[error_byte_loc] ^ (1 << bit_offset);
					mtd->ecc_stats.corrected++;
				} else {
					mtd->ecc_stats.failed++;
					/* MSG(INIT, "UnCorrectable ErrLoc=%d\n", au4ErrBitLoc[i]); */
				}
				error_bit_loc_2nd = (error_bit_loc[i] >> 16) & 0x1FFF;
				if (0 != error_bit_loc_2nd) {
					if (error_bit_loc_2nd < 0x1000) {
						error_byte_loc = error_bit_loc_2nd / 8;
						bit_offset = error_bit_loc_2nd % 8;
						databuf[error_byte_loc] = databuf[error_byte_loc] ^ (1 << bit_offset);
						mtd->ecc_stats.corrected++;
					} else {
						mtd->ecc_stats.failed++;
						/* MSG(INIT, "UnCorrectable High ErrLoc=%d\n", au4ErrBitLoc[i]); */
					}
				}
			}
		}
		if (0 == (DRV_Reg16(ECC_DECFER_REG16) & (1 << u4SecIndex))) {
			ret = false;
		}
	}
#endif
	return ret;
}

/******************************************************************************
 * mtk_snand_RFIFOValidSize
 *
 * DESCRIPTION:
 *   Check the Read FIFO data bytes !
 *
 * PARAMETERS:
 *   u16 u2Size
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_RFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_RD_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) < u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	return true;
}

/******************************************************************************
 * mtk_snand_WFIFOValidSize
 *
 * DESCRIPTION:
 *   Check the Write FIFO data bytes !
 *
 * PARAMETERS:
 *   u16 u2Size
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_WFIFOValidSize(u16 u2Size)
{
	u32 timeout = 0xFFFF;
	while (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) > u2Size) {
		timeout--;
		if (0 == timeout) {
			return false;
		}
	}
	return true;
}

/******************************************************************************
 * mtk_snand_status_ready
 *
 * DESCRIPTION:
 *   Indicate the NAND device is ready or not !
 *
 * PARAMETERS:
 *   u32 u4Status
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_status_ready(u32 status)
{
	u32 timeout = 0xFFFF;

	status &= ~STA_NAND_BUSY;

	while ((DRV_Reg32(NFI_STA_REG32) & status) != 0) {
		timeout--;

		if (0 == timeout) {
			return false;
		}
	}

	return true;
}

/******************************************************************************
 * mtk_snand_reset_con
 *
 * DESCRIPTION:
 *   Reset the NAND device hardware component !
 *
 * PARAMETERS:
 *   struct mtk_nand_host *host (Initial setting data)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_reset_con(void)
{
	u32 reg32;

	/* HW recommended reset flow */
	int timeout = 0xFFFF;

	/* part 1. SNF */

	reg32 = DRV_Reg32(RW_SNAND_MISC_CTL);
	reg32 |= SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg32);
	reg32 &= ~SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg32);

	/* part 2. NFI */

	for (; ;) {
		if (0 == DRV_Reg16(NFI_MASTERSTA_REG16)) {
			break;
		}
	}

	mb();

	DRV_WriteReg16(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);

	for (; ;) {
		if (0 == (DRV_Reg16(NFI_STA_REG32) & (STA_NFI_FSM_MASK | STA_NAND_FSM_MASK))) {
			break;
		}

		timeout--;

		if (!timeout) {
			MSG(INIT, "[%s] NFI_MASTERSTA_REG16 timeout!!\n", __func__);
		}
	}

	/* issue reset operation */

	mb();

	return mtk_snand_status_ready(STA_NFI_FSM_MASK | STA_NAND_BUSY) && mtk_snand_RFIFOValidSize(0) && mtk_snand_WFIFOValidSize(0);
}

/******************************************************************************
 * mtk_snand_set_mode
 *
 * DESCRIPTION:
 *    Set the oepration mode !
 *
 * PARAMETERS:
 *   u16 u2OpMode (read/write)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_set_mode(u16 opmode)
{
	u16 mode = DRV_Reg16(NFI_CNFG_REG16);
	mode &= ~CNFG_OP_MODE_MASK;
	mode |= opmode;
	DRV_WriteReg16(NFI_CNFG_REG16, mode);
}

/******************************************************************************
 * mtk_snand_set_autoformat
 *
 * DESCRIPTION:
 *    Enable/Disable hardware autoformat !
 *
 * PARAMETERS:
 *   bool bEnable (Enable/Disable)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_set_autoformat(bool enable)
{
	if (enable) {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	}
}

/******************************************************************************
 * mtk_snand_configure_fdm
 *
 * DESCRIPTION:
 *   Configure the FDM data size !
 *
 * PARAMETERS:
 *   u16 u2FDMSize
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_configure_fdm(u16 fdmsize, u16 fdmeccsize)
{
	NFI_CLN_REG16(NFI_PAGEFMT_REG16, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, fdmsize << PAGEFMT_FDM_SHIFT);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, fdmeccsize << PAGEFMT_FDM_ECC_SHIFT);
}

/******************************************************************************
 * mtk_snand_check_RW_count
 *
 * DESCRIPTION:
 *    Check the RW how many sectors !
 *
 * PARAMETERS:
 *   u16 u2WriteSize
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_check_RW_count(u16 writesize)
{
	u32 timeout = 0xFFFF;
	u16 sectors = writesize >> 9;

	while (ADDRCNTR_CNTR(DRV_Reg16(NFI_ADDRCNTR_REG16)) < sectors) {
		timeout--;
		if (0 == timeout) {
			MSG(INIT, "[%s] timeout\n", __func__);
			MSG(INIT, "sectnum:0x%x, addrcnt:0x%x\n", sectors, ADDRCNTR_CNTR(DRV_Reg16(NFI_ADDRCNTR_REG16)));
			return false;
		}
	}
	return true;
}

/******************************************************************************
 * mtk_snand_ready_for_read
 *
 * DESCRIPTION:
 *    Prepare hardware environment for read !
 *
 * PARAMETERS:
 *   struct nand_chip *nand, u32 rowaddr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_ready_for_read(struct nand_chip *nand, u32 rowaddr, u32 coladdr, u32 sectors, bool full, u8 *buf)
{
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	bool ret = false;
	u32 cmd, reg;
	u32 col_addr = coladdr;
	u32 timeout = 0xFFFF;
	SNAND_Mode mode = SPIQ;
#if __INTERNAL_USE_AHB_MODE__
	unsigned long phys = 0;
#endif

	if (!mtk_snand_reset_con()) {
		goto cleanup;
	}

	/* 1. Read page to cache */

	cmd = mtk_snand_gen_c1a3(SNAND_CMD_PAGE_READ, rowaddr); /* PAGE_READ command + 3-byte address */

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 1 + 3);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 0);

	mtk_snand_dev_mac_op(SPI);

	/* 2. Get features (status polling) */

	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8);

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	for (; ;) {
		mtk_snand_dev_mac_op(SPI);

		timeout--;
		if (0 == timeout) {
			MSG(INIT, "[%s] timeout\n", __func__);
			break;
		}

		cmd = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if ((cmd & SNAND_STATUS_OIP) == 0) {
			/* use MTK ECC, not device ECC */
			/* if (SNAND_STATUS_TOO_MANY_ERROR_BITS == (cmd & SNAND_STATUS_ECC_STATUS_MASK)) */
			/* { */
			/* ret = false; */
			/* } */

			break;
		}
	}

	/* ------ SNF Part ------ */

	/* set PAGE READ command & address */
	reg = (SNAND_CMD_PAGE_READ << SNAND_PAGE_READ_CMD_OFFSET) | (rowaddr & SNAND_PAGE_READ_ADDRESS_MASK);
	DRV_WriteReg32(RW_SNAND_RD_CTL1, reg);

	/* set DATA READ dummy cycle and command (use default value, ignored) */
	if (mode == SPI) {
		reg = DRV_Reg32(RW_SNAND_RD_CTL2);
		reg &= ~SNAND_DATA_READ_CMD_MASK;
		reg |= SNAND_CMD_RANDOM_READ & SNAND_DATA_READ_CMD_MASK;
		DRV_WriteReg32(RW_SNAND_RD_CTL2, reg);
	} else if (mode == SPIQ) {
		mtk_snand_dev_enable_spiq(true);

		reg = DRV_Reg32(RW_SNAND_RD_CTL2);
		reg &= ~SNAND_DATA_READ_CMD_MASK;
		reg |= SNAND_CMD_RANDOM_READ_SPIQ & SNAND_DATA_READ_CMD_MASK;
		DRV_WriteReg32(RW_SNAND_RD_CTL2, reg);
	}

	/* set DATA READ address */
	DRV_WriteReg32(RW_SNAND_RD_CTL3, (col_addr & SNAND_DATA_READ_ADDRESS_MASK));

	/* set SNF timing */
	reg = DRV_Reg32(RW_SNAND_MISC_CTL);

	reg |= SNAND_DATARD_CUSTOM_EN;

	if (mode == SPI) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
	} else if (mode == SPIQ) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
		reg |= ((SNAND_DATA_READ_MODE_X4 << SNAND_DATA_READ_MODE_OFFSET) & SNAND_DATA_READ_MODE_MASK);
	}

	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);

	/* set SNF data length */

	reg = sectors * (NAND_SECTOR_SIZE + g_snand_spare_per_sector);

	DRV_WriteReg32(RW_SNAND_MISC_CTL2, (reg | (reg << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET)));

	/* ------ NFI Part ------ */

	mtk_snand_set_mode(CNFG_OP_CUST);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	DRV_WriteReg16(NFI_CON_REG16, sectors << CON_NFI_SEC_SHIFT);

	if (g_bHwEcc) {
		/* Enable HW ECC */
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	}

	if (full) {
#if __INTERNAL_USE_AHB_MODE__
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);

		phys = (u32)buf;

		if (!phys) {
			MSG(INIT, "[mtk_snand_ready_for_read]convert virt addr (%x) to phys add (%x)fail!!!", (u32)buf, (u32)phys);
			return false;
		} else {
			DRV_WriteReg32(NFI_STRADDR_REG32, phys);
		}
#else   /* use MCU */
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif

		if (g_bHwEcc) {
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		} else {
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		}
	} else    /* raw data */
	{
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
	}

	mtk_snand_set_autoformat(full);

	if (full) {
		if (g_bHwEcc) {
			mtk_snand_ecc_decode_start();
		}
	}

	ret = true;

	cleanup:

	return ret;
}

/******************************************************************************
 * mtk_snand_ready_for_write
 *
 * DESCRIPTION:
 *    Prepare hardware environment for write !
 *
 * PARAMETERS:
 *   struct nand_chip *nand, u32 rowaddr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_ready_for_write(struct nand_chip *nand, u32 rowaddr, u32 col_addr, bool full, u8 *buf)
{
	bool ret = false;
	u32 sec_num = 1 << (nand->page_shift - 9);
	u32 reg;
	SNAND_Mode mode = SPIQ;
#if __INTERNAL_USE_AHB_MODE__
	unsigned long phys = 0;
	/* u32 T_phys = 0; */
#endif

	/* Toshiba spi nand just use SPI mode*/
	if(devinfo.id[0] == 0x98)
		mode = SPI;

	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	if (!mtk_snand_reset_con()) {
		return false;
	}

	/* 1. Write Enable */
	mtk_snand_dev_command(SNAND_CMD_WRITE_ENABLE, 1);

	/* ------ SNF Part ------ */

	/* set SPI-NAND command */
	if (SPI == mode) {
		reg = SNAND_CMD_WRITE_ENABLE | (SNAND_CMD_PROGRAM_LOAD << SNAND_PG_LOAD_CMD_OFFSET) | (SNAND_CMD_PROGRAM_EXECUTE << SNAND_PG_EXE_CMD_OFFSET);
		DRV_WriteReg32(RW_SNAND_PG_CTL1, reg);
	} else if (SPIQ == mode) {
		reg = SNAND_CMD_WRITE_ENABLE | (SNAND_CMD_PROGRAM_LOAD_X4<< SNAND_PG_LOAD_CMD_OFFSET) | (SNAND_CMD_PROGRAM_EXECUTE << SNAND_PG_EXE_CMD_OFFSET);
		DRV_WriteReg32(RW_SNAND_PG_CTL1, reg);
		mtk_snand_dev_enable_spiq(true);
	}

	/* set program load address */
	DRV_WriteReg32(RW_SNAND_PG_CTL2, col_addr & SNAND_PG_LOAD_ADDR_MASK);

	/* set program execution address */
	DRV_WriteReg32(RW_SNAND_PG_CTL3, rowaddr);

	/* set SNF data length  (set in snand_xxx_write_data) */

	/* set SNF timing */
	reg = DRV_Reg32(RW_SNAND_MISC_CTL);

	reg |= SNAND_PG_LOAD_CUSTOM_EN;    /* use custom program */

	if (SPI == mode) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
		reg |= ((SNAND_DATA_READ_MODE_X1 << SNAND_DATA_READ_MODE_OFFSET) & SNAND_DATA_READ_MODE_MASK);
		reg &=~ SNAND_PG_LOAD_X4_EN;
	} else if (SPIQ == mode) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
		reg |= ((SNAND_DATA_READ_MODE_X4 << SNAND_DATA_READ_MODE_OFFSET) & SNAND_DATA_READ_MODE_MASK);
		reg |= SNAND_PG_LOAD_X4_EN;
	}

	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);

	/* set SNF data length */

	reg = sec_num * (NAND_SECTOR_SIZE + g_snand_spare_per_sector);
	/* MSG(INIT, "BAYI-TEST 5, SNF data length:0x%x, g_bHwEcc:0x%x \n", reg, g_bHwEcc); */
	DRV_WriteReg32(RW_SNAND_MISC_CTL2, reg | (reg << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET));

	/* ------ NFI Part ------ */

	mtk_snand_set_mode(CNFG_OP_PRGM);

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_READ_EN);

	DRV_WriteReg16(NFI_CON_REG16, sec_num << CON_NFI_SEC_SHIFT);

	if (full) {
#if __INTERNAL_USE_AHB_MODE__
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);

		phys = (u32)buf;

		if (!phys) {
			MSG(INIT, "[mt6575_nand_ready_for_write]convert virt addr (%x) to phys add fail!!!", (u32)buf);
			return false;
		} else {
			DRV_WriteReg32(NFI_STRADDR_REG32, phys);
		}
#else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
#endif
		if (g_bHwEcc) {
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		} else {
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		}
	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
	}

	mtk_snand_set_autoformat(full);

	if (full) {
		if (g_bHwEcc) {
			mtk_snand_ecc_encode_start();
		}
	}

	ret = true;

	return ret;
}

static bool mtk_snand_check_dececc_done(u32 sectors)
{
	u32 timeout, dec_mask;
	timeout = 0xffff;
	dec_mask = (1 << sectors) - 1;
	while ((dec_mask != DRV_Reg16(ECC_DECDONE_REG16)) && timeout > 0)
		timeout--;
	if (timeout == 0) {
		MSG(VERIFY, "ECC_DECDONE: timeout\n");
		return false;
	}
	return true;
}

/******************************************************************************
 * mtk_snand_read_page_data
 *
 * DESCRIPTION:
 *   Fill the page data into buffer !
 *
 * PARAMETERS:
 *   u8* databuf, u32 size
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_dma_read_data(struct mtd_info *mtd, u8 *buf, u32 num_sec, u8 full)
{
	int timeout = 0xffff;

#if !defined (CONFIG_SYS_DCACHE_OFF)
	int invalid_dcache_len;
#endif

#if !defined (CONFIG_NAND_BOOTLOADER)
	sg_init_one(&sg, buf, num_sec * (NAND_SECTOR_SIZE + g_snand_spare_per_sector));
	dma_map_sg(mtd->dev.parent, &sg, 1, dir);
#elif !defined (CONFIG_SYS_DCACHE_OFF)
	invalid_dcache_len = num_sec * (NAND_SECTOR_SIZE + g_snand_spare_per_sector);

	invalid_dcache_len = (invalid_dcache_len + CACHE_ALIGNE_SIZE -1) / CACHE_ALIGNE_SIZE * CACHE_ALIGNE_SIZE;

	invalidate_dcache_range((unsigned long)buf, (unsigned long)(buf + invalid_dcache_len));
#endif

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	if ((unsigned int)buf % 16) /* TODO: can not use AHB mode here */
	{
		MSG(INIT, "Un-16-aligned address\n");
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	} else {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	}

	/* set dummy command to trigger NFI enter custom mode */
	DRV_WriteReg16(NFI_CMD_REG16, NAND_CMD_DUMMYREAD);

	mb();

	DRV_Reg16(NFI_INTR_REG16);  /* read clear */

	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);

	mb();
	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BRD);
	mb();

	g_snand_dev_status = SNAND_NFI_CUST_READING;
	g_snand_cmd_status = 0; /* busy */
	g_running_dma = 1;

	while (!DRV_Reg16(NFI_INTR_REG16)) {
		timeout--;

		if (0 == timeout)   /* time out */
		{
			MSG(INIT, "[%s] poll nfi_intr error\n", __func__);
			dump_nfi();
			g_snand_dev_status = SNAND_IDLE;
			g_running_dma = 0;
			return false;   /* 4  // AHB Mode Time Out! */
		}
	}

	while (num_sec > ((DRV_Reg16(NFI_BYTELEN_REG16) & 0xf000) >> 12)) {
		timeout--;

		if (0 == timeout) {
			MSG(INIT, "[%s] poll BYTELEN error(num_sec=%d)\n", __func__, num_sec);
			dump_nfi();
			g_snand_dev_status = SNAND_IDLE;
			g_running_dma = 0;
			return false;   /* 4  // AHB Mode Time Out! */
		}
	}

	g_snand_dev_status = SNAND_IDLE;
	g_running_dma = 0;

	return true;
}

static bool mtk_snand_read_page_data(struct mtd_info *mtd, u8 *databuf, u32 sectors, u8 full)
{
	return mtk_snand_dma_read_data(mtd, databuf, sectors, full);
}

/******************************************************************************
 * mtk_snand_write_page_data
 *
 * DESCRIPTION:
 *   Fill the page data into buffer !
 *
 * PARAMETERS:
 *   u8* databuf, u32 size
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_dma_write_data(struct mtd_info *mtd, u8 *databuf, u32 size, u8 full)
{
	u32 snf_len;

#if !defined (CONFIG_NAND_BOOTLOADER)
	sg_init_one(&sg, databuf, size);
	dma_map_sg(mtd->dev.parent, &sg, 1, dir);
#elif !defined (CONFIG_SYS_DCACHE_OFF)
	flush_dcache_range((unsigned long) databuf, (unsigned long)databuf + size);
#endif

	/* set SNF data length */
	if (full) {
		snf_len = size + mtd->oobsize; /* 64; //devinfo.sparesize; */
	} else {
		snf_len = size;
	}

	DRV_WriteReg32(RW_SNAND_MISC_CTL2, ((snf_len) | (snf_len << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET)));

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	/* set dummy command to trigger NFI enter custom mode */
	DRV_WriteReg16(NFI_CMD_REG16, NAND_CMD_DUMMYPROG);

	DRV_Reg16(NFI_INTR_REG16);
	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_CUSTOM_PROG_DONE_INTR_EN);

	if ((unsigned int)databuf % 16)    /* TODO: can not use AHB mode here */
	{
		MSG(INIT, "Un-16-aligned address\n");
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	} else {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	}

	mb();

	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BWR);

	g_running_dma = 3;

	while (!(DRV_Reg32(RW_SNAND_STA_CTL1) & SNAND_CUSTOM_PROGRAM));

	g_running_dma = 0;

	return true;
}

static bool mtk_snand_mcu_write_data(struct mtd_info *mtd, const u8 *buf, u32 length, u8 full)
{
	u32 timeout = 0xFFFF;
	u32 i;
	u32 *buf32;
	u32 snf_len;

	/* set SNF data length */
	if (full) {
		snf_len += length + devinfo.sparesize;
	} else {
		snf_len = length;
	}

	DRV_WriteReg32(RW_SNAND_MISC_CTL2, ((snf_len) | (snf_len << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET)));

	/* set dummy command to trigger NFI enter custom mode */
	DRV_WriteReg16(NFI_CMD_REG16, NAND_CMD_DUMMYPROG);

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	mb();

	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BWR);

	buf32 = (u32 *) buf;

	if ((u32) buf % 4 || length % 4)
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);
	else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	if ((u32) buf % 4 || length % 4) {
		for (i = 0; (i < (length)) && (timeout > 0);) {
			if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1) {
				DRV_WriteReg32(NFI_DATAW_REG32, *buf++);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				MSG(INIT, "[%s] timeout\n", __func__);
				dump_nfi();
				return false;
			}
		}
	} else {
		for (i = 0; (i < (length >> 2)) && (timeout > 0);) {
			/* if (FIFO_WR_REMAIN(DRV_Reg16(NFI_FIFOSTA_REG16)) <= 12) */
			if (DRV_Reg16(NFI_PIO_DIRDY_REG16) & 1) {
				DRV_WriteReg32(NFI_DATAW_REG32, *buf32++);
				i++;
			} else {
				timeout--;
			}
			if (0 == timeout) {
				MSG(INIT, "[%s] timeout\n", __func__);
				dump_nfi();
				return false;
			}
		}
	}

	return true;
}

static bool mtk_snand_write_page_data(struct mtd_info *mtd, u8 *buf, u32 size, u8 full)
{
#if (__INTERNAL_USE_AHB_MODE__)
	return mtk_snand_dma_write_data(mtd, buf, size, full);
#else
	return mtk_snand_mcu_write_data(mtd, buf, size, full);
#endif
}

/******************************************************************************
 * mtk_snand_read_fdm_data
 *
 * DESCRIPTION:
 *   Read a fdm data !
 *
 * PARAMETERS:
 *   u8* databuf, u32 sectors
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_read_fdm_data(u8 *databuf, u32 sectors)
{
	u32 i;
	u32 *buf32 = (u32 *) databuf;

	if (buf32) {
		for (i = 0; i < sectors; ++i) {
			*buf32++ = DRV_Reg32(NFI_FDM0L_REG32 + (i * 8));
			*buf32++ = DRV_Reg32(NFI_FDM0M_REG32 + (i * 8));
		}
	}
}

/******************************************************************************
 * mtk_snand_write_fdm_data
 *
 * DESCRIPTION:
 *   Write a fdm data !
 *
 * PARAMETERS:
 *   u8* databuf, u32 sectors
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
__attribute__((aligned(CACHE_ALIGNE_SIZE))) static u8 fdm_buf[64];
static void mtk_snand_write_fdm_data(struct nand_chip *chip, u8 *databuf, u32 sectors)
{
	u32 i, j;
	u8 checksum = 0;
	bool empty = true;
	struct nand_oobfree *free_entry;
	u32 *buf32;

	memcpy(fdm_buf, databuf, sectors * 8);

	free_entry = chip->ecc.layout->oobfree;

	for (i = 0; i < MTD_MAX_OOBFREE_ENTRIES && free_entry[i].length; i++) {
		for (j = 0; j < free_entry[i].length; j++) {
			if (databuf[free_entry[i].offset + j] != 0xFF)
				empty = false;

			checksum ^= databuf[free_entry[i].offset + j];
		}
	}

	if (!empty) {
		fdm_buf[free_entry[i - 1].offset + free_entry[i - 1].length] = checksum;
	}

	buf32 = (u32 *) fdm_buf;

	for (i = 0; i < sectors; ++i) {
		DRV_WriteReg32(NFI_FDM0L_REG32 + (i * 8), *buf32++);
		DRV_WriteReg32(NFI_FDM0M_REG32 + (i * 8), *buf32++);
	}
}

/******************************************************************************
 * mtk_snand_stop_read
 *
 * DESCRIPTION:
 *   Stop read operation !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_stop_read(void)
{
	/* ------ NFI Part */
	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BRD);

	/* ------ SNF Part */
	/* set 1 then set 0 to clear done flag */
	DRV_WriteReg32(RW_SNAND_STA_CTL1, SNAND_CUSTOM_PROGRAM);
	DRV_WriteReg32(RW_SNAND_STA_CTL1, 0);

	/* clear essential SNF setting */
	NFI_CLN_REG32(RW_SNAND_MISC_CTL, SNAND_PG_LOAD_CUSTOM_EN);

	if (g_bHwEcc) {
		mtk_snand_ecc_decode_end();
	}

	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	mtk_snand_reset_con();
}

/******************************************************************************
 * mtk_snand_stop_write
 *
 * DESCRIPTION:
 *   Stop write operation !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_stop_write(void)
{
	/* ------ NFI Part */

	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BWR);

	/* ------ SNF Part */

	/* set 1 then set 0 to clear done flag */
	DRV_WriteReg32(RW_SNAND_STA_CTL1, SNAND_CUSTOM_PROGRAM);
	DRV_WriteReg32(RW_SNAND_STA_CTL1, 0);

	/* clear essential SNF setting */
	NFI_CLN_REG32(RW_SNAND_MISC_CTL, SNAND_PG_LOAD_CUSTOM_EN);

	mtk_snand_dev_enable_spiq(false);

	if (g_bHwEcc) {
		mtk_snand_ecc_encode_end();
	}

	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);
}

static bool mtk_snand_read_page_part2(struct mtd_info *mtd, u32 row_addr, u32 num_sec, u8 *buf)
{
	bool    ret = true;
	u32     reg;
	u32     col_part2, i, len;
	u32     spare_per_sector;
	u8      *buf_part2;
	u32     timeout = 0xFFFF;
	u32     old_dec_mode = 0;
	unsigned long     buf_phy;

	spare_per_sector = mtd->oobsize / (mtd->writesize / NAND_SECTOR_SIZE);

	for (i = 0; i < 2; i++) {
		mtk_snand_reset_con();

		if (0 == i) {
			col_part2 = (NAND_SECTOR_SIZE + spare_per_sector) * (num_sec - 1);

			buf_part2 = buf;

			len = 2112 - col_part2;
		} else {
			col_part2 = 2112;

			buf_part2 += len;   /* append to first round */

			len = (num_sec * (NAND_SECTOR_SIZE + spare_per_sector)) - 2112;
		}

		/* ------ SNF Part ------ */

		/* set DATA READ address */
		DRV_WriteReg32(RW_SNAND_RD_CTL3, (col_part2 & SNAND_DATA_READ_ADDRESS_MASK));

		if (0 == i) {
			/* set RW_SNAND_MISC_CTL */
			reg = DRV_Reg32(RW_SNAND_MISC_CTL);

			reg |= SNAND_DATARD_CUSTOM_EN;

			reg &= ~SNAND_DATA_READ_MODE_MASK;
			reg |= ((SNAND_DATA_READ_MODE_X4 << SNAND_DATA_READ_MODE_OFFSET) & SNAND_DATA_READ_MODE_MASK);

			DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);
		}

		/* set SNF data length */
		reg = len | (len << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET);

		DRV_WriteReg32(RW_SNAND_MISC_CTL2, reg);

		/* ------ NFI Part ------ */

		if (0 == i) {
			mtk_snand_set_mode(CNFG_OP_CUST);
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
			NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
			NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
			mtk_snand_set_autoformat(false);
		}

		/* DRV_WriteReg16(NFI_CON_REG16, 1 << CON_NFI_SEC_SHIFT); */

		buf_phy = (u32)buf_part2;

		DRV_WriteReg32(NFI_STRADDR_REG32, buf_phy);

		DRV_WriteReg32(NFI_SPIDMA_REG32, SPIDMA_SEC_EN | (len & SPIDMA_SEC_SIZE_MASK));

		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

		/* set dummy command to trigger NFI enter custom mode */
		DRV_WriteReg16(NFI_CMD_REG16, NAND_CMD_DUMMYREAD);

		DRV_Reg16(NFI_INTR_REG16);  /* read clear */
		DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);

		NFI_SET_REG16(NFI_CON_REG16, (1 << CON_NFI_SEC_SHIFT) | CON_NFI_BRD);     /* fixed to sector number 1 */

		timeout = 0xFFFF;

		while (!(DRV_Reg16(NFI_INTR_REG16) & INTR_AHB_DONE))    /* for custom read, wait NFI's INTR_AHB_DONE done to ensure all data are transferred to buffer */
		{
			timeout--;

			if (0 == timeout) {
				ret = false;
				goto unmap_and_cleanup;
			}
		}

		timeout = 0xFFFF;

		while (((DRV_Reg16(NFI_BYTELEN_REG16) & 0xf000) >> 12) != 1) {
			timeout--;

			if (0 == timeout) {
				ret = false;
				goto unmap_and_cleanup;
			}
		}

		/* ------ NFI Part */

		NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BRD);

		/* ------ SNF Part */

		/* set 1 then set 0 to clear done flag */
		DRV_WriteReg32(RW_SNAND_STA_CTL1, SNAND_CUSTOM_READ);
		DRV_WriteReg32(RW_SNAND_STA_CTL1, 0);

		/* clear essential SNF setting */
		if (1 == i) {
			NFI_CLN_REG32(RW_SNAND_MISC_CTL, SNAND_DATARD_CUSTOM_EN);
		}
	}

	if (g_bHwEcc) {
		/* configure ECC decoder && encoder */
		reg = DRV_Reg32(ECC_DECCNFG_REG32);
		old_dec_mode = reg & DEC_CNFG_DEC_MODE_MASK;
		reg &= ~DEC_CNFG_DEC_MODE_MASK;
		reg |= DEC_CNFG_AHB;
		reg |= DEC_CNFG_DEC_BURST_EN;
		DRV_WriteReg32(ECC_DECCNFG_REG32, reg);

		buf_phy = (u32)buf;

		DRV_WriteReg32(ECC_DECDIADDR_REG32, (u32)buf_phy);

		DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
		DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);

		while (!((DRV_Reg32(ECC_DECDONE_REG16)) & (1 << 0)));

		reg = DRV_Reg32(ECC_DECENUM0_REG32);

		if (0 != reg) {
			reg &= 0x1F;

			if (0x1F == reg) {
				ret = false;
				MSG(INIT, "NFI, ECC-U(2), PA=%d, S=%d\n", row_addr, num_sec - 1);
			} else {
				if (reg) {
					MSG(INIT, "NFI ECC-C(2), #err:%d, PA=%d, S=%d\n", reg, row_addr, num_sec - 1);
				}
			}
		}

		/* restore essential NFI and ECC registers */
		DRV_WriteReg32(NFI_SPIDMA_REG32, 0);
		reg = DRV_Reg32(ECC_DECCNFG_REG32);
		reg &= ~DEC_CNFG_DEC_MODE_MASK;
		reg |= old_dec_mode;
		reg &= ~DEC_CNFG_DEC_BURST_EN;
		DRV_WriteReg32(ECC_DECCNFG_REG32, reg);
		DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
		DRV_WriteReg32(ECC_DECDIADDR_REG32, 0);
	}

unmap_and_cleanup:

	return ret;
}


/******************************************************************************
 * mtk_nand_exec_read_page
 *
 * DESCRIPTION:
 *   Read a page data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, u32 rowaddr, u32 pagesize,
 *   u8* pagebuf, u8* fdmbuf
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
int mtk_nand_exec_read_page(struct mtd_info *mtd, u32 rowaddr, u32 pagesize, u8 *pagebuf, u8 *fdmbuf)
{
	u8 *buf;
	u8 *p_buf_local;
	bool ret = true;
	u8 retry;
	struct nand_chip *nand = mtd->priv;
	u32 sectors = pagesize >> 9;
#ifdef NAND_PFM
	struct timeval pfm_time_read;
#endif
	PFM_BEGIN(pfm_time_read);
	memset(local_buffer_16_align, 0x5a, pagesize);
#if !defined (CONFIG_SYS_DCACHE_OFF)
	if (((u32) pagebuf % 64) && local_buffer_16_align)
#else
	if (((unsigned long) pagebuf % 16) && local_buffer_16_align)
#endif
	{
		/* MSG(INIT, "<READ> Data buffer not 16 bytes aligned %d:, address %p\n", pagesize, pagebuf); */
		buf = local_buffer_16_align;
	} else {
		buf = pagebuf;
	}

	rowaddr = mtk_snand_dev_die_select(mtd, rowaddr);
	mtk_snand_rs_reconfig_nfiecc(mtd, rowaddr);

	if (mtk_snand_rs_if_require_split()) {
		sectors--;
	}

	retry = 0;

mtk_nand_exec_read_page_retry:

	ret = true;

	if (mtk_snand_ready_for_read(nand, rowaddr, 0, sectors, true, buf)) {
		if (!mtk_snand_read_page_data(mtd, buf, sectors, true)) {
			MSG(INIT, "[%s]mtk_snand_read_page_data() FAIL!!!\n", __func__);
			ret = false;
		}

		if (!mtk_snand_status_ready(STA_NAND_BUSY)) {
			ret = false;
		}

		if (g_bHwEcc) {
			if (!mtk_snand_check_dececc_done(sectors)) {
				MSG(INIT, "[%s]mtk_snand_check_dececc_done() FAIL!!!\n", __func__);
				ret = false;
			}
		}

		mtk_snand_read_fdm_data(fdmbuf, sectors);

		if (g_bHwEcc) {
			if (!mtk_snand_check_bch_error(mtd, buf, fdmbuf, sectors - 1, rowaddr)) {
				/* MSG(READ, "[%s]mtk_snand_check_bch_error() FAIL!!!\n", __func__); */
				MSG(INIT, "[%s]mtk_snand_check_bch_error() FAIL!!!\n", __func__);
				ret = false;
			}
		}

		bm_swap(mtd, fdmbuf, buf);
		mtk_snand_stop_read();
	}

	if (false == ret) {
		if (retry < 0)/* no need to read retry */
		{
			retry++;
			MSG(INIT, "[%s] read retrying ... (the %d time)\n", __func__, retry);
			mtk_snand_reset_dev();

			goto mtk_nand_exec_read_page_retry;
		}
	}

	if (mtk_snand_rs_if_require_split()) {
		/* read part II */

		/* sectors++; */
		sectors = pagesize >> 9;

		/* note: use local temp buffer to read part 2 */
		if (!mtk_snand_read_page_part2(mtd, rowaddr, sectors, g_snand_temp))
			ret = false;

		mb();

		/* copy data */

		p_buf_local = buf + NAND_SECTOR_SIZE * (sectors - 1);

		memcpy(p_buf_local, g_snand_temp, NAND_SECTOR_SIZE);

		mb();

		/* copy FDM data */

		p_buf_local = fdmbuf + OOB_AVAI_PER_SECTOR * (sectors - 1);

		memcpy(p_buf_local, (g_snand_temp + NAND_SECTOR_SIZE), OOB_AVAI_PER_SECTOR);
	}

	mtk_snand_dev_enable_spiq(false);

	if (buf == local_buffer_16_align) {
		memcpy(pagebuf, buf, pagesize);
	}

	PFM_END_R(pfm_time_read, pagesize + 32);
	/* MSG(INIT, "BAYI-TEST, pagebuf:0x%x, 0x%x, 0x%x, 0x%x, 0x%x rowaddr:0x%x\n", pagebuf[0], pagebuf[1], pagebuf[2], pagebuf[3], pagebuf[4], rowaddr); */

	return ret;
}

/******************************************************************************
 * mtk_nand_exec_write_page
 *
 * DESCRIPTION:
 *   Write a page data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, u32 rowaddr, u32 pagesize,
 *   u8* pagebuf, u8* fdmbuf
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/

static bool mtk_snand_dev_program_execute(u32 page)
{
	u32 cmd;
	bool ret = true;

	/* 3. Program Execute */

	#if 1 /* added by Bayi for write success */
	g_snand_dev_status = SNAND_DEV_PROG_EXECUTING;
	g_snand_cmd_status = 0; /* busy */
	#else
	g_snand_dev_status = SNAND_IDLE; /* SNAND_NFI_AUTO_ERASING; */
	g_snand_cmd_status = NAND_STATUS_READY; /* 0; // busy */
	#endif
	cmd = mtk_snand_gen_c1a3(SNAND_CMD_PROGRAM_EXECUTE, page);

	mtk_snand_dev_command(cmd, 4);

	return ret;
}

int mtk_nand_exec_write_page(struct mtd_info *mtd, u32 rowaddr, u32 pagesize, u8 *pagebuf, u8 *fdmbuf)
{
	struct nand_chip *chip = mtd->priv;
	u32 sectors = pagesize >> 9;
	u8 *buf;
	u8 status;

#ifdef _MTK_NAND_DUMMY_DRIVER_
	if (dummy_driver_debug) {
#ifdef TESTTIME
		unsigned long long time = sched_clock();
		if (!((time * 123 + 59) % 32768)) {
			MSG(INIT, "[NAND_DUMMY_DRIVER] Simulate write error at page: 0x%x\n", rowaddr);
			return -EIO;
		}
#endif
	}
#endif

#ifdef NAND_PFM
	struct timeval pfm_time_write;
#endif
	PFM_BEGIN(pfm_time_write);
#if !defined (CONFIG_SYS_DCACHE_OFF)
	if (((u32) pagebuf % 64) && local_buffer_16_align)
#else
	if (((u32) pagebuf % 16) && local_buffer_16_align)
#endif
	{
		/* MSG(INIT, "Data buffer not 16 bytes aligned: %p\n", pagebuf); */
		memcpy(local_buffer_16_align, pagebuf, mtd->writesize);
		buf = local_buffer_16_align;
	} else
		buf = pagebuf;

	rowaddr = mtk_snand_dev_die_select(mtd, rowaddr);
	mtk_snand_rs_reconfig_nfiecc(mtd, rowaddr);
	bm_swap(mtd, fdmbuf, buf);

	if (mtk_snand_ready_for_write(chip, rowaddr, 0, true, buf)) {

		mtk_snand_write_fdm_data(chip, fdmbuf, sectors);

		(void)mtk_snand_write_page_data(mtd, buf, pagesize, true);
		(void)mtk_snand_check_RW_count(pagesize);
		mtk_snand_stop_write();
		{
			mtk_snand_dev_program_execute(rowaddr);
		}
	}

	PFM_END_W(pfm_time_write, pagesize + 32);
	bm_swap(mtd, fdmbuf, buf);

	status = chip->waitfunc(mtd, chip); /* return NAND_STATUS_READY or return NAND_STATUS_FAIL */

	if (status & NAND_STATUS_FAIL)
		return -EIO;
	else
		return 0;
}

/******************************************************************************
 *
 * Write a page to a logical address
 *
 * Return values
 *  0: No error
 *  1: Error
 *
 *****************************************************************************/
static int mtk_snand_write_page(struct mtd_info *mtd, struct nand_chip *chip, const u8 * buf, int oob_required, int page, int cached, int raw)
{
	int page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	int block = page / page_per_block;
	u16 page_in_block = page % page_per_block;

	if (true == mtk_snand_is_vendor_reserved_blocks(page))
		return 1;

	if (mtk_nand_exec_write_page(mtd, page, mtd->writesize, (u8 *) buf, chip->oob_poi)) {
		MSG(INIT, "write fail at block: 0x%x, page: 0x%x\n", block, page_in_block);
		return -EIO;
	}

	return 0;
}

static void mtk_snand_dev_read_id(u8 id[])
{
	u8 cmd = SNAND_CMD_READ_ID;

	mtk_snand_dev_command_ext(SPI, &cmd, id, 1, SNAND_MAX_ID + 1);
}

/******************************************************************************
 * mtk_snand_command_bp
 *
 * DESCRIPTION:
 *   Handle the commands from MTD !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, unsigned int command, int column, int page_addr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_command_bp(struct mtd_info *mtd, unsigned int command, int column, int page_addr)
{
	struct nand_chip *nand = mtd->priv;
#ifdef NAND_PFM
	struct timeval pfm_time_erase;
#endif
	switch (command) {
		case NAND_CMD_SEQIN:
			memset(g_kCMD.au1OOB, 0xFF, sizeof(g_kCMD.au1OOB));
			g_kCMD.databuf = NULL;
			g_kCMD.rowaddr = page_addr;
			g_kCMD.coladdr = column;
			break;

		case NAND_CMD_PAGEPROG:
			if (g_kCMD.databuf || (0xFF != g_kCMD.au1OOB[0])) {
				u8 *databuf = g_kCMD.databuf ? g_kCMD.databuf : nand->buffers->databuf;
				mtk_nand_exec_write_page(mtd, g_kCMD.rowaddr, mtd->writesize, databuf, g_kCMD.au1OOB);
				g_kCMD.rowaddr = (u32) -1;
				g_kCMD.oobrowaddr = (u32) -1;
			}
			break;

		case NAND_CMD_READOOB:
			g_kCMD.rowaddr = page_addr;
			g_kCMD.coladdr = column + mtd->writesize;
#ifdef NAND_PFM
			g_kCMD.purereadoob = 1;
			g_kCMD.purereadoobnum += 1;
#endif
			break;

		case NAND_CMD_READ0:
			g_kCMD.rowaddr = page_addr;
			g_kCMD.coladdr = column;
#ifdef NAND_PFM
			g_kCMD.purereadoob = 0;
#endif
			break;

		case NAND_CMD_ERASE1:
			MSG(INIT, "[SNAND-ERROR] Not allowed NAND_CMD_ERASE1!\n");
			break;

		case NAND_CMD_ERASE2:
			MSG(INIT, "[SNAND-ERROR] Not allowed NAND_CMD_ERASE2!\n");
			break;

		case NAND_CMD_STATUS:

			if ((SNAND_DEV_ERASE_DONE == g_snand_dev_status) ||
				(SNAND_DEV_PROG_DONE == g_snand_dev_status))
			{
				g_snand_dev_status = SNAND_IDLE;	/* reset command status */
			}

			g_snand_read_byte_mode = SNAND_RB_CMD_STATUS;

			break;

		case NAND_CMD_RESET:

			(void)mtk_snand_reset_con();

			g_snand_read_byte_mode = SNAND_RB_DEFAULT;

			break;

		case NAND_CMD_READID:

			mtk_snand_reset_dev();
			mtk_snand_reset_con();

			mb();

			mtk_snand_dev_read_id(g_snand_id_data);

			g_snand_id_data_idx = 1;

			g_snand_read_byte_mode = SNAND_RB_READ_ID;

			break;

		default:
			BUG();
			break;
	}
}

/******************************************************************************
 * mtk_snand_select_chip
 *
 * DESCRIPTION:
 *   Select a chip !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, int chip
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip == -1 && false == g_bInitDone) {
		struct nand_chip *nand = mtd->priv;

		struct mtk_nand_host *host_ptr = host;

		struct mtk_nand_host_hw *hw = host_ptr->hw;
		u32 spare_per_sector = mtd->oobsize/(mtd->writesize/512);
		u32 ecc_bit = 4;
		u32 spare_bit = PAGEFMT_SPARE_16;

		if (spare_per_sector >= 28) {
			spare_bit = PAGEFMT_SPARE_28;
			ecc_bit = 12;
			spare_per_sector = 28;
		} else if (spare_per_sector >= 27) {
			spare_bit = PAGEFMT_SPARE_27;
			ecc_bit = 8;
			spare_per_sector = 27;
		} else if (spare_per_sector >= 26) {
			spare_bit = PAGEFMT_SPARE_26;
			ecc_bit = 8;
			spare_per_sector = 26;
		} else if (spare_per_sector >= 16) {
			spare_bit = PAGEFMT_SPARE_16;
			ecc_bit = 4;
			spare_per_sector = 16;
		} else {
			MSG(INIT, "[NAND]: NFI not support oobsize: %x\n", spare_per_sector);
			assert(0);
		}
		mtd->oobsize = spare_per_sector * (mtd->writesize/512);

		/* Setup PageFormat */
		if (4096 == mtd->writesize) {
			NFI_SET_REG16(NFI_PAGEFMT_REG16, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K);
			nand->cmdfunc = mtk_snand_command_bp;
		} else if (2048 == mtd->writesize) {
			NFI_SET_REG16(NFI_PAGEFMT_REG16, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K);
			nand->cmdfunc = mtk_snand_command_bp;
		}

		mtk_snand_ecc_config(hw, ecc_bit);

		g_snand_rs_ecc_bit_second_part = ecc_bit;
		g_snand_rs_spare_per_sector_second_part_nfi = (spare_bit << PAGEFMT_SPARE_SHIFT);
		g_snand_spare_per_sector = spare_per_sector;

		g_bInitDone = true;
	}
	switch (chip) {
		case -1:
			break;
		case 0:
#if defined(CONFIG_EARLY_LINUX_PORTING)		/* FPGA NAND is placed at CS1 not CS0 */
			/* DRV_WriteReg16(NFI_CSEL_REG16, 1); */
			break;
#endif
		case 1:
			/* DRV_WriteReg16(NFI_CSEL_REG16, chip); */
			break;
	}
}

/******************************************************************************
 * mtk_snand_read_byte
 *
 * DESCRIPTION:
 *   Read a byte of data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static uint8_t mtk_snand_read_byte(struct mtd_info *mtd)
{
	u8      reg8;
	u32     reg32;

	if (SNAND_RB_READ_ID == g_snand_read_byte_mode) {
		if (g_snand_id_data_idx > SNAND_MAX_ID) {
			return 0;
		} else {
			return g_snand_id_data[g_snand_id_data_idx++];
		}
	} else if (SNAND_RB_CMD_STATUS == g_snand_read_byte_mode) /* get feature to see the status of program and erase (e.g., nand_wait) */
	{
		if ((SNAND_DEV_ERASE_DONE == g_snand_dev_status) ||
			(SNAND_DEV_PROG_DONE == g_snand_dev_status))
		{
			return g_snand_cmd_status;	/* report the latest device operation status (program OK, erase OK, program failed, erase failed ...etc) */
		} else if (SNAND_DEV_PROG_EXECUTING == g_snand_dev_status)    /* check ready again */
		{
			DRV_WriteReg32(RW_SNAND_GPRAM_DATA, (SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8)));
			DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
			DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

			mtk_snand_dev_mac_op(SPI);

			reg8 = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

			if (0 == (reg8 & SNAND_STATUS_OIP)) /* ready */
			{
				g_snand_dev_status = SNAND_DEV_PROG_DONE;

				if (0 != (reg8 & SNAND_STATUS_PROGRAM_FAIL)) /* ready but having fail report from device */
				{
					MSG(INIT, "[snand] Prog failed!\n");	/* Stanley Chu (add more infomation like page address) */

					g_snand_cmd_status = NAND_STATUS_READY | NAND_STATUS_FAIL;
				} else {
					g_snand_cmd_status = NAND_STATUS_READY;
				}

				return g_snand_cmd_status;
			} else {
				return 0;	/* busy */
			}
		} else if (SNAND_NFI_AUTO_ERASING == g_snand_dev_status)  /* check ready again */
		{
			/* wait for auto erase finish */
			reg32 = DRV_Reg32(RW_SNAND_STA_CTL1);

			if ((reg32 & SNAND_AUTO_BLKER) == 0) {
				return 0;	/* busy */
			} else {
				g_snand_dev_status = SNAND_DEV_ERASE_DONE;
				reg8 = (u8)(DRV_Reg32(RW_SNAND_GF_CTL1) & SNAND_GF_STATUS_MASK);

				if (0 != (reg8 & SNAND_STATUS_ERASE_FAIL)) /* ready but having fail report from device */
				{
					MSG(INIT, "[snand] Erase failed!\n");  /* Stanley Chu (add more infomation like page address) */
					g_snand_cmd_status = NAND_STATUS_READY | NAND_STATUS_FAIL;
				} else {
					g_snand_cmd_status = NAND_STATUS_READY;
				}
				return g_snand_cmd_status;    /* return non-0 */
			}
		} else if ((SNAND_NFI_CUST_READING == g_snand_dev_status)) {
			g_snand_cmd_status = 0; /* busy */
			return g_snand_cmd_status;
		} else    /* idle */
		{
			g_snand_dev_status = SNAND_IDLE;
			g_snand_cmd_status = (NAND_STATUS_READY | NAND_STATUS_WP);
			return g_snand_cmd_status;    /* return NAND_STATUS_WP to indicate SPI-NAND does NOT WP */
		}
	} else {
		return 0;
	}
}

/******************************************************************************
 * mtk_snand_read_buf
 *
 * DESCRIPTION:
 *   Read NAND data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, uint8_t *buf, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *nand = (struct nand_chip *)mtd->priv;
	struct NAND_CMD *pkCMD = &g_kCMD;
	u32 coladdr = pkCMD->coladdr;
	u32 pagesize = mtd->writesize;

	if (coladdr < pagesize) {
		if ((coladdr == 0) && (len >= pagesize)) {
			mtk_nand_exec_read_page(mtd, pkCMD->rowaddr, pagesize, buf, pkCMD->au1OOB);
			if (len > pagesize) {
				u32 size = min(len - pagesize, sizeof(pkCMD->au1OOB));
				memcpy(buf + pagesize, pkCMD->au1OOB, size);
			}
		} else {
			mtk_nand_exec_read_page(mtd, pkCMD->rowaddr, pagesize, nand->buffers->databuf, pkCMD->au1OOB);
			memcpy(buf, nand->buffers->databuf + coladdr, len);
		}
		pkCMD->oobrowaddr = pkCMD->rowaddr;
	} else {
		u32 u4Offset = coladdr - pagesize;
		u32 size = min(len - u4Offset, sizeof(pkCMD->au1OOB));
		if (pkCMD->oobrowaddr != pkCMD->rowaddr) {
			mtk_nand_exec_read_page(mtd, pkCMD->rowaddr, pagesize, nand->buffers->databuf, pkCMD->au1OOB);
			pkCMD->oobrowaddr = pkCMD->rowaddr;
		}
		memcpy(buf, pkCMD->au1OOB + u4Offset, size);
	}
	pkCMD->coladdr += len;
}

/******************************************************************************
 * mtk_snand_write_buf
 *
 * DESCRIPTION:
 *   Write NAND data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, const uint8_t *buf, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct NAND_CMD *pkCMD = &g_kCMD;
	u32 coladdr = pkCMD->coladdr;
	u32 pagesize = mtd->writesize;
	int i4Size, i;

	if (coladdr >= pagesize) {
		u32 u4Offset = coladdr - pagesize;
		u8 *pOOB = pkCMD->au1OOB + u4Offset;
		i4Size = min(len, (int)(sizeof(pkCMD->au1OOB) - u4Offset));

		for (i = 0; i < i4Size; i++) {
			pOOB[i] &= buf[i];
		}
	} else {
		pkCMD->databuf = (u8 *)buf;
	}

	pkCMD->coladdr += len;
}

/******************************************************************************
 * mtk_snand_write_page_hwecc
 *
 * DESCRIPTION:
 *   Write NAND data with hardware ecc !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static int mtk_snand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required)
{
	mtk_snand_write_buf(mtd, buf, mtd->writesize);
	mtk_snand_write_buf(mtd, chip->oob_poi, mtd->oobsize);
	return 0;
}

/******************************************************************************
 * mtk_snand_read_page_hwecc
 *
 * DESCRIPTION:
 *   Read NAND data with hardware ecc !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static int mtk_snand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t *buf, int oob_required, int page)
{
	struct NAND_CMD *pkCMD = &g_kCMD;
	u32 coladdr = pkCMD->coladdr;
	u32 pagesize = mtd->writesize;

	if (coladdr == 0) {
		mtk_nand_exec_read_page(mtd, pkCMD->rowaddr, pagesize, buf, chip->oob_poi);
		pkCMD->coladdr += pagesize + mtd->oobsize;
	}
	return 0;
}

/******************************************************************************
 *
 * Read a page to a logical address
 *
 *****************************************************************************/
static int mtk_snand_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	if (mtk_nand_exec_read_page(mtd, page, mtd->writesize, buf, chip->oob_poi))
		return 0;

	return -EIO;
}

/******************************************************************************
 *
 * Erase a block at a logical address
 *
 *****************************************************************************/

static void mtk_snand_dev_stop_erase(void)
{
	u32 reg;

	/* set 1 then set 0 to clear done flag */
	reg = DRV_Reg32(RW_SNAND_STA_CTL1);

	DRV_WriteReg32(RW_SNAND_STA_CTL1, reg);
	reg = reg & ~SNAND_AUTO_BLKER;
	DRV_WriteReg32(RW_SNAND_STA_CTL1, reg);

	/* clear trigger bit */
	reg = DRV_Reg32(RW_SNAND_ER_CTL);
	reg &= ~SNAND_AUTO_ERASE_TRIGGER;
	DRV_WriteReg32(RW_SNAND_ER_CTL, reg);

	g_snand_dev_status = SNAND_IDLE;
}

void mtk_snand_dev_unlock_all_blocks(void)
{
	u32 cmd;
	u8  lock;
	u8  lock_new;

	/* MSG(INIT, "[SNF] Unlock all blocks ...\n\r"); */

	/* read original block lock settings */
	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_BLOCK_LOCK << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	mtk_snand_dev_mac_enable(SPI);
	mtk_snand_dev_mac_trigger();
	mtk_snand_dev_mac_leave();

	lock = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

	lock_new = lock & ~SNAND_BLOCK_LOCK_BITS;

	if (lock != lock_new) {
		/* write enable */
		mtk_snand_dev_command(SNAND_CMD_WRITE_ENABLE, 1);

		/* set features */
		cmd = SNAND_CMD_SET_FEATURES | (SNAND_CMD_FEATURES_BLOCK_LOCK << 8) | (lock_new << 16);
		mtk_snand_dev_command(cmd, 3);
	}

	/* cosnfrm if unlock is successfull */
	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_BLOCK_LOCK << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	mtk_snand_dev_mac_enable(SPI);
	mtk_snand_dev_mac_trigger();
	mtk_snand_dev_mac_leave();

	lock = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

	if (lock & SNAND_BLOCK_LOCK_BITS) {
		MSG(INIT, "[SNF] Unlock all blocks failed!\n\r");
		while (1);
	}
}

/**********************************************************
Description : SPI_NAND_Reset_Device
Input       : None
Output      : None
***********************************************************/

void mtk_snand_ecc_control(u8 enable)
{
	u32 cmd;
	u8  otp;
	u8  otp_new;

	/* MSG(INIT, "[SNF] [Cheng] ECC Control (1: Enable, 0: Disable) : %d\n\r", enable); */

	/* read original otp settings */

	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);
	mtk_snand_dev_mac_op(SPI);

	otp = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

	if (enable == true) {
		otp_new = otp | SNAND_OTP_ECC_ENABLE;
	} else {
		otp_new = otp & ~SNAND_OTP_ECC_ENABLE;
	}

	if (otp != otp_new) {
		/* write enable */

		mtk_snand_dev_command(SNAND_CMD_WRITE_ENABLE, 1);


		/* set features */
		cmd = SNAND_CMD_SET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8) | (otp_new << 16);

		mtk_snand_dev_command(cmd, 3);


		/* cosnfrm */

		/* read original otp settings */

		cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8);
		DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
		DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
		DRV_WriteReg32(RW_SNAND_MAC_INL, 1);
		mtk_snand_dev_mac_op(SPI);

		otp = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if (otp != otp_new) {
			while (1);
		}
	}
}

int mtk_snand_reset_device(void)
{
	u8      cmd = SNAND_CMD_SW_RESET;
	u32     cmd32 = SNAND_CMD_SW_RESET;
	bool    tomeout = false;
	u32     timeout_tick;
	u32     start_tick;

	mtk_snand_reset();

	/* issue SW RESET command to device */
	mtk_snand_dev_command_ext(SPI, &cmd, NULL, 1, 0);

	/* wait for awhile, then polling status register (required by spec) */
	gpt_busy_wait_us(SNAND_DEV_RESET_LATENCY_US);


	/* 2. Get features (status polling) */

	cmd32 = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8);

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd32);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	mtk_snand_dev_mac_op(SPI);

	/* polling status register */
	timeout_tick = gpt4_time2tick_us(SNAND_MAX_RDY_AFTER_RST_LATENCY_US);
	start_tick = gpt4_get_current_tick();

	for (tomeout = false; tomeout != true;) {
		mtk_snand_dev_mac_enable(SPI);
		mtk_snand_dev_mac_trigger();
		mtk_snand_dev_mac_leave();

		cmd = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);
		if (0 == (cmd & SNAND_STATUS_OIP)) {
			break;
		}

		tomeout = gpt4_timeout_tick(start_tick, timeout_tick);
	}

	if (!tomeout)
		return 0;
	else
		return -1;
}

void mtk_snand_dev_erase(u32 row_addr)   /* auto erase */
{
	u32  reg;

	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	if (!mtk_snand_reset_con()) {
		return;
	}

		/* 1. Write Enable */
	mtk_snand_dev_command(SNAND_CMD_WRITE_ENABLE, 1);

	mtk_snand_reset_device();

	/* erase address */
	DRV_WriteReg32(RW_SNAND_ER_CTL2, row_addr);

	/* set loop limit and polling cycles */
	reg = (SNAND_LOOP_LIMIT_NO_LIMIT << SNAND_LOOP_LIMIT_OFFSET) | 0x20;
	DRV_WriteReg32(RW_SNAND_GF_CTL3, reg);

	/* set latch latency & CS de-select latency (ignored) */

	/* set erase command */
	reg = SNAND_CMD_BLOCK_ERASE << SNAND_ER_CMD_OFFSET;
	DRV_WriteReg32(RW_SNAND_ER_CTL, reg);

	/* trigger interrupt waiting */
	reg = DRV_Reg16(NFI_INTR_EN_REG16);
	reg = INTR_AUTO_BLKER_INTR_EN;
	DRV_WriteReg16(NFI_INTR_EN_REG16, reg);

	/* trigger auto erase */
	reg = DRV_Reg32(RW_SNAND_ER_CTL);
	reg |= SNAND_AUTO_ERASE_TRIGGER;
	DRV_WriteReg32(RW_SNAND_ER_CTL, reg);

#if 1
	g_snand_dev_status = SNAND_NFI_AUTO_ERASING;
	g_snand_cmd_status = 0; /* busy */
#else
	g_snand_dev_status = SNAND_IDLE; /* SNAND_NFI_AUTO_ERASING; */
	g_snand_cmd_status = NAND_STATUS_READY; /* 0; // busy */

#endif
}

int mtk_nand_erase_hw(struct mtd_info *mtd, int page)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int ret =0;

#ifdef _MTK_NAND_DUMMY_DRIVER_
	if (dummy_driver_debug) {
#ifdef TESTTIME
		unsigned long long time = sched_clock();
		if (!((time * 123 + 59) % 1024)) {
			MSG(INIT, "[NAND_DUMMY_DRIVER] Simulate erase error at page: 0x%x\n", page);
			return NAND_STATUS_FAIL;
		}
#endif
	}
#endif

	if (true == mtk_snand_is_vendor_reserved_blocks(page))  /* return erase failed for reserved blocks */
	{
		return NAND_STATUS_FAIL;
	}

	if (SNAND_BUSY < g_snand_dev_status) {
		MSG(INIT, "[%s] device is not IDLE!!\n", __func__);
	}
	page = mtk_snand_dev_die_select(mtd, page);

	mtk_snand_dev_erase(page);

	ret = chip->waitfunc(mtd, chip);

	mtk_snand_dev_stop_erase();

	return ret;
}

static int mtk_nand_erase(struct mtd_info *mtd, int page)
{
	/* get mapping */
	struct nand_chip *chip = mtd->priv;
	int page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	int block = page / page_per_block;
	int status;

	status = mtk_nand_erase_hw(mtd, page);

	if (status & NAND_STATUS_FAIL) {
		MSG(INIT, "Erase fail at block: 0x%x\n", block);
		return NAND_STATUS_FAIL;
	}

	return 0;
}

/******************************************************************************
 * mtk_snand_read_oob_raw
 *
 * DESCRIPTION:
 *   Read oob data
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, const uint8_t *buf, int addr, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   this function read raw oob data out of flash, so need to re-organise
 *   data format before using.
 *   len should be times of 8, call this after nand_get_device.
 *   Should notice, this function read data without ECC protection.
 *
 *****************************************************************************/
static int mtk_snand_read_oob_raw(struct mtd_info *mtd, uint8_t *buf, int page_addr, int len)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int ret = true;
	int num_sec, num_sec_original;
	u32 i;

	if (len > NAND_MAX_OOBSIZE || len % OOB_AVAI_PER_SECTOR || !buf) {
		MSG(INIT, KERN_WARNING "[%s] invalid parameter, len: %d, buf: %p\n", __func__, len, buf);
		return -EINVAL;
	}

	num_sec_original = num_sec = len / OOB_AVAI_PER_SECTOR;

	page_addr = mtk_snand_dev_die_select(mtd, page_addr);
	mtk_snand_rs_reconfig_nfiecc(mtd, page_addr);

	if (((num_sec_original * NAND_SECTOR_SIZE) == mtd->writesize) && mtk_snand_rs_if_require_split()) {
		num_sec--;
	}

	/* read the 1st sector (including its spare area) with MTK ECC enabled */
	if (mtk_snand_ready_for_read(chip, page_addr, 0, num_sec, true, g_snand_temp)) {
		if (!mtk_snand_read_page_data(mtd, g_snand_temp, num_sec, true))  /* only read 1 sector */
		{
			ret = false;
		}
		if (!mtk_snand_status_ready(STA_NAND_BUSY)) {
			ret = false;
		}

		if (!mtk_snand_check_dececc_done(num_sec)) {
			ret = false;
		}

		mtk_snand_read_fdm_data(g_snand_spare, num_sec);
		bm_swap(mtd, g_snand_spare, g_snand_temp);
		mtk_snand_stop_read();
	}

	if (((num_sec_original * NAND_SECTOR_SIZE) == mtd->writesize) && mtk_snand_rs_if_require_split()) {
		/* read part II */

		num_sec++;

		/* note: use local temp buffer to read part 2 */
		mtk_snand_read_page_part2(mtd, page_addr, num_sec, g_snand_temp + ((num_sec - 1) * NAND_SECTOR_SIZE));

		/* copy spare data */
		for (i = 0; i < OOB_AVAI_PER_SECTOR; i++) {
			g_snand_spare[(num_sec - 1) * OOB_AVAI_PER_SECTOR + i] = g_snand_temp[((num_sec - 1) * NAND_SECTOR_SIZE) + NAND_SECTOR_SIZE + i];
		}
	}

	mtk_snand_dev_enable_spiq(false);

	num_sec = num_sec * OOB_AVAI_PER_SECTOR;

	for (i = 0; i < num_sec; i++) {
		buf[i] = g_snand_spare[i];
	}

	if (true == ret)
		return 0;
	else
		return 1;
}

static int mtk_snand_write_oob_raw(struct mtd_info *mtd, const uint8_t * buf, int page_addr, int len)
{
	struct nand_chip *chip = mtd->priv;
	u32 col_addr = 0;
	u32 sector = 0;
	int write_len = 0;
	int status;
	int sec_num = 1 << (chip->page_shift-9);
	int spare_per_sector = mtd->oobsize/sec_num;

	if (len >  NAND_MAX_OOBSIZE || len % OOB_AVAI_PER_SECTOR || !buf) {
		MSG(INIT, KERN_WARNING "[%s] invalid parameter, len: %d, buf: %p\n", __func__, len, buf);
		return -EINVAL;
	}
	page_addr = mtk_snand_dev_die_select(mtd, page_addr);
	mtk_snand_rs_reconfig_nfiecc(mtd, page_addr);

	while (len > 0) {
		write_len = min(len,  spare_per_sector);
		col_addr = sector * (NAND_SECTOR_SIZE +  spare_per_sector) + NAND_SECTOR_SIZE;
		if (!mtk_snand_ready_for_write(chip, page_addr, col_addr, false, NULL)) {
			return -EIO;
		}

		if (!mtk_snand_mcu_write_data(mtd, buf + sector * spare_per_sector, write_len, false)) {
			return -EIO;
		}

		(void)mtk_snand_check_RW_count(write_len);

		mtk_snand_stop_write();

		mtk_snand_dev_program_execute(page_addr);

		status = chip->waitfunc(mtd, chip);

		if (status & NAND_STATUS_FAIL) {
			MSG(INIT, "<by > status: %d\n", status);
			return -EIO;
		}

		len -= write_len;
		sector++;
	}

	return 0;
}

static int mtk_snand_write_oob_hw(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	/* u8 *buf = chip->oob_poi; */
	int i, iter;

	int sec_num = 1 << (chip->page_shift-9);
	int spare_per_sector = mtd->oobsize/sec_num;

	memcpy(local_oob_buf, chip->oob_poi, mtd->oobsize);

	/* copy ecc data */
	for (i = 0; i < chip->ecc.layout->eccbytes; i++) {
		iter = (i / OOB_AVAI_PER_SECTOR) * spare_per_sector + OOB_AVAI_PER_SECTOR + i % OOB_AVAI_PER_SECTOR;
		local_oob_buf[iter] = chip->oob_poi[chip->ecc.layout->eccpos[i]];
		/* chip->oob_poi[chip->ecc.layout->eccpos[i]] = local_oob_buf[iter]; */
	}

	/* copy FDM data */
	for (i = 0; i < sec_num; i++) {
		memcpy(&local_oob_buf[i * spare_per_sector], &chip->oob_poi[i * OOB_AVAI_PER_SECTOR], OOB_AVAI_PER_SECTOR);
	}

	return mtk_snand_write_oob_raw(mtd, local_oob_buf, page, mtd->oobsize);
}

static int mtk_snand_write_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	int block = page / page_per_block;
	u16 page_in_block = page % page_per_block;

	if (mtk_snand_write_oob_hw(mtd, chip, page)) {
		MSG(INIT, "write oob fail at block: 0x%x, page: 0x%x\n", block, page_in_block);
		return -EIO;
	}

	return 0;
}

int mtk_nand_block_markbad_hw(struct mtd_info *mtd, loff_t offset)
{
	struct nand_chip *chip = mtd->priv;
	int block = (int)offset >> chip->phys_erase_shift;
	int page = block * (1 << (chip->phys_erase_shift - chip->page_shift));
	unsigned int pagesize = 1 << chip->page_shift;
	u32 sectors = pagesize >> 9;
	unsigned int bm_offset = (sectors - 1) * OOB_AVAI_PER_SECTOR;
	int ret;
	u8 fdmbuf[64];

	memset(fdmbuf, 0xFF, 64);
	memset(local_buffer, 0xFF, mtd->writesize);
	/* Bad mark at the first byte of the last sector */
	fdmbuf[bm_offset] = 0;

	MSG(INIT, "Mark bad at block: 0x%x\n", block);
	ret = mtk_nand_exec_write_page(mtd, page, pagesize, local_buffer, fdmbuf);
	return ret;
}

static int mtk_snand_block_markbad(struct mtd_info *mtd, loff_t offset)
{
	struct nand_chip *chip = mtd->priv;
	int block = (int)offset >> chip->phys_erase_shift;
	int ret;

	nand_get_device(chip, mtd, FL_WRITING);

	ret = mtk_nand_block_markbad_hw(mtd, block << chip->phys_erase_shift);

	nand_release_device(mtd);

	return ret;
}

int mtk_snand_read_oob_hw(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int i;
	u8 iter = 0;

	int sec_num = 1 << (chip->page_shift-9);
	int spare_per_sector = mtd->oobsize/sec_num;
#ifdef TESTTIME
	unsigned long long time1, time2;

	time1 = sched_clock();
#endif

	/* NOTE(Nelson):
		* mtd->oobsize = devinfo.sparesize; (64)
		* but total available OOB size is 32
		* So modify to (sec_num * OOB_AVAI_PER_SECTOR) (32)
		*/
	/* if (mtk_snand_read_oob_raw(mtd, chip->oob_poi, page, mtd->oobsize)) */
	if (mtk_snand_read_oob_raw(mtd, chip->oob_poi, page, (sec_num * OOB_AVAI_PER_SECTOR))) {
		/* printk( "[%s]mtk_snand_read_oob_raw return failed\n", __func__); */
		return -EIO;
	}
#ifdef TESTTIME
	time2 = sched_clock() - time1;
	if (!readoobflag) {
		readoobflag = 1;
		MSG(INIT, "[%s] time is %llu", __func__, time2);
	}
#endif

	/* adjust to ecc physical layout to memory layout */
	/*********************************************************/
	/* FDM0 | ECC0 | FDM1 | ECC1 | FDM2 | ECC2 | FDM3 | ECC3 */
	/*  8B  |  8B  |  8B  |  8B  |  8B  |  8B  |  8B  |  8B  */
	/*********************************************************/

	memcpy(local_oob_buf, chip->oob_poi, mtd->oobsize);

	/* copy ecc data */
	for (i = 0; i < chip->ecc.layout->eccbytes; i++) {
		iter = (i / OOB_AVAI_PER_SECTOR) *  spare_per_sector + OOB_AVAI_PER_SECTOR + i % OOB_AVAI_PER_SECTOR;
		chip->oob_poi[chip->ecc.layout->eccpos[i]] = local_oob_buf[iter];
	}

	/* copy FDM data */
	for (i = 0; i < sec_num; i++) {
		memcpy(&chip->oob_poi[i * OOB_AVAI_PER_SECTOR], &local_oob_buf[i *  spare_per_sector], OOB_AVAI_PER_SECTOR);
	}

	return 0;
}

static int mtk_snand_read_oob(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	mtk_snand_read_oob_hw(mtd, chip, page);

	return 0;                   /* the return value is sndcmd */
}

int mtk_nand_block_bad_hw(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int page_addr = (int)(ofs >> chip->page_shift);
	unsigned int page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	page_addr &= ~(page_per_block - 1);
	unsigned int pagesize = 1 << chip->page_shift;
	u32 sectors = pagesize >> 9;
	unsigned int offset = (sectors - 1) * OOB_AVAI_PER_SECTOR;

	if (true == mtk_snand_is_vendor_reserved_blocks(page_addr)) /* return bad block if it is reserved block */
	{
		return 1;
	}

	if (mtk_snand_read_oob_raw(mtd, g_snand_spare, page_addr, sectors * OOB_AVAI_PER_SECTOR))   /* need read full sector */
	{
		MSG(INIT, KERN_WARNING "mtk_snand_read_oob_raw return error\n");
		return 1;
	}

	if (g_snand_spare[offset] != 0xff) {
		MSG(INIT, KERN_WARNING "\nBad block detected at 0x%x, Badmark is 0x%x\n", page_addr, g_snand_spare[offset]);
		return 1;
	}

	return 0;                   /* everything is OK, good block */
}

static int mtk_snand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	int chipnr = 0;

	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int block = (int)ofs >> chip->phys_erase_shift;
	int ret;

	if (getchip) {
		chipnr = (int)(ofs >> chip->chip_shift);
		nand_get_device(chip, mtd, FL_READING);
		/* Select the NAND device */
		chip->select_chip(mtd, chipnr);
	}


	ret = mtk_nand_block_bad_hw(mtd, block << chip->phys_erase_shift);
	if (ret) {
		MSG(INIT, "Bad block: 0x%x\n", block);
		ret = 1;
	}

	if (getchip) {
		nand_release_device(mtd);
	}

	return ret;
}
/******************************************************************************
 * mtk_nand_init_size
 *
 * DESCRIPTION:
 *   initialize the pagesize, oobsize, blocksize
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, struct nand_chip *this, u8 *id_data
 *
 * RETURNS:
 *   Buswidth
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/

static int mtk_nand_init_size(struct mtd_info *mtd, struct nand_chip *this, u8 *id_data)
{
	/* Get page size */
	mtd->writesize = devinfo.pagesize;

	/* Get oobsize */
	mtd->oobsize = devinfo.sparesize;

	/* Get blocksize. */
	mtd->erasesize = devinfo.blocksize * 1024;

	return 0;
}

/******************************************************************************
 * mtk_snand_verify_buf
 *
 * DESCRIPTION:
 *   Verify the NAND write data is correct or not !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, const uint8_t *buf, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE

char gacBuf[4096 + 128];

static int mtk_snand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	struct NAND_CMD *pkCMD = &g_kCMD;
	u32 pagesize = mtd->writesize;
	u32 *pSrc, *pDst;
	int i;

	mtk_nand_exec_read_page(mtd, pkCMD->rowaddr, pagesize, (u8 *)gacBuf, (u8 *)(gacBuf + pagesize));

	pSrc = (u32 *) buf;
	pDst = (u32 *) gacBuf;
	len = len / sizeof(u32);
	for (i = 0; i < len; ++i) {
		if (*pSrc != *pDst) {
			MSG(VERIFY, "mtk_snand_verify_buf page fail at page %d\n", pkCMD->rowaddr);
			return -1;
		}
		pSrc++;
		pDst++;
	}

	pSrc = (u32 *) chip->oob_poi;
	pDst = (u32 *) (gacBuf + pagesize);

	if ((pSrc[0] != pDst[0]) || (pSrc[1] != pDst[1]) || (pSrc[2] != pDst[2]) || (pSrc[3] != pDst[3]) || (pSrc[4] != pDst[4]) || (pSrc[5] != pDst[5]))
		/* TODO: Ask Designer Why? */
		/* (pSrc[6] != pDst[6]) || (pSrc[7] != pDst[7])) */
	{
		MSG(VERIFY, "mtk_snand_verify_buf oob fail at page %d\n", pkCMD->rowaddr);
		MSG(VERIFY, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pSrc[0], pSrc[1], pSrc[2], pSrc[3], pSrc[4], pSrc[5], pSrc[6], pSrc[7]);
		MSG(VERIFY, "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pDst[0], pDst[1], pDst[2], pDst[3], pDst[4], pDst[5], pDst[6], pDst[7]);
		return -1;
	}

	return 0;
}
#endif

/**********************************************************
Description : SNAND_GPIO_config
***********************************************************/
void mtk_snand_gpio_config(int state)
{
	u32 reg;
	u32 snfi_gpio_backup[4] = {0};

	if (state) {
		/* pinmux config */
		snfi_gpio_backup[0] = DRV_Reg32(GPIO_MODE(7));
		snfi_gpio_backup[1] = DRV_Reg32(GPIO_MODE(8));
		reg = snfi_gpio_backup[0];
		reg &= ~(0xFF<<24);
		reg |= (0x22<<24);
		DRV_WriteReg32(GPIO_MODE(7), reg);
		reg = snfi_gpio_backup[1];
		reg &= ~(0xFFFF);
		reg |= (0x2222);
		DRV_WriteReg32(GPIO_MODE(8), reg);

		/* IO driving config */
		snfi_gpio_backup[2] = DRV_Reg32(GPIO_DRIV(1));
		snfi_gpio_backup[3] = DRV_Reg32(GPIO_DRIV(2));
		reg = snfi_gpio_backup[2];
		reg |= 0x66666000;
		DRV_WriteReg32(GPIO_DRIV(1), reg);
		reg = snfi_gpio_backup[3];
		reg |= 0x6;
		DRV_WriteReg32(GPIO_DRIV(2), reg);
	} else {
		DRV_WriteReg32(GPIO_MODE(7), snfi_gpio_backup[0]);
		DRV_WriteReg32(GPIO_MODE(8), snfi_gpio_backup[1]);
		DRV_WriteReg32(GPIO_DRIV(1), snfi_gpio_backup[2]);
		DRV_WriteReg32(GPIO_DRIV(2), snfi_gpio_backup[3]);
	}
}

void mtk_snand_clk_setting(void)
{
	int reg;
	/* MT6572 NFI default clock is on */
	DRV_WriteReg32(0x1000205C, 1);
	/* Default is NFI mode., switch to SNAND mode */
	DRV_WriteReg32(RW_SNAND_CNFG, 1);

	reg = DRV_Reg32(SYS_CKGEN_BASE + SYS_CLK_CFG_1);
	/* MSG(INIT, "<uboot > before setting:0x%x\n", reg); */
	reg = reg & ~0xf0000;
	reg = reg | 0x60000;
	/* MSG(INIT, "<uboot > after setting:0x%x\n", reg); */
	DRV_WriteReg32(SYS_CKGEN_BASE + SYS_CLK_CFG_1, reg);
}

/**********************************************************
Description : SNAND_Reset
Remark      :
***********************************************************/
void mtk_snand_reset(void)
{
	bool    tomeout;
	u32     reg;
	u32     timeout_tick;
	u32     start_tick;

	/* ------ Part 1. NFI */

	/* 1-1. wait for NFI Master Status being cleared */

	timeout_tick = gpt4_time2tick_us(SNAND_MAX_NFI_RESET_US);
	start_tick = gpt4_get_current_tick();

	for (tomeout = false; tomeout != true;) {
		reg = DRV_Reg16(NFI_STA_REG32);

		if (0 == (reg & (STA_NFI_FSM_MASK | STA_NAND_FSM_MASK))) {
			break;
		}

		tomeout = gpt4_timeout_tick(start_tick, timeout_tick);
	}

	/* 1-2. reset NFI State Machine and Clear FIFO Counter */

	reg = CON_FIFO_FLUSH | CON_NFI_RST;
	DRV_WriteReg16(NFI_CON_REG16, reg);

	/* 1-3. wait for NAND Interface & NFI Internal FSM being reset */

	timeout_tick = gpt4_time2tick_us(SNAND_MAX_NFI_RESET_US);
	start_tick = gpt4_get_current_tick();

	while (1) {
		reg = DRV_Reg16(NFI_STA_REG32);

		if (0 == (reg & (STA_NFI_FSM_MASK | STA_NAND_FSM_MASK))) {
			break;
		}

		tomeout = gpt4_timeout_tick(start_tick, timeout_tick);
	}

	/* ------ Part  2. SNF: trigger SW_RST bit to reset state machine */

	reg = DRV_Reg32(RW_SNAND_MISC_CTL);
	reg |= SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);
	reg &= ~SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);
}

/******************************************************************************
 * mtk_nand_init_hw
 *
 * DESCRIPTION:
 *   Initial NAND device hardware component !
 *
 * PARAMETERS:
 *   struct mtk_nand_host *host (Initial setting data)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_nand_init_hw(struct mtk_nand_host *host)
{
	struct mtk_nand_host_hw *hw = host->hw;

	g_bInitDone = false;
	g_kCMD.oobrowaddr = (u32) -1;

	/* Set default NFI access timing control */
	DRV_WriteReg32(NFI_ACCCON_REG32, hw->nfi_access_timing);
	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg16(NFI_PAGEFMT_REG16, 0);

	/* Reset the state machine and data FIFO, because flushing FIFO */
	(void)mtk_snand_reset_con();

	/* Set the ECC engine */
	if (hw->nand_ecc_mode == NAND_ECC_HW) {
		/* MSG(INIT, "%s : Use HW ECC\n", MODULE_NAME); */
		if (g_bHwEcc) {
			NFI_SET_REG32(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		}

		mtk_snand_ecc_config(host->hw, 4);
		mtk_snand_configure_fdm(8, 1);
	}

	/* Initilize interrupt. Clear interrupt, read clear. */
	DRV_Reg16(NFI_INTR_REG16);

	/* Interrupt arise when read data or program data to/from AHB is done. */
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	/* Enable automatic disable ECC clock when NFI is busy state */

	/* REMARKS! HWDCM_SWCON_ON should not be enabled for SPI-NAND! Otherwise NFI will behave abnormally! */
	DRV_WriteReg16(NFI_DEBUG_CON1_REG16, (WBUF_EN));

	/* NOTE(Bayi): add gpio init and clk select */
#if !defined(LEOPARD_FPGA_BOARD)
	mtk_snand_gpio_config(true); /* no use for FPGA */
#endif
	mtk_snand_clk_setting();
	/* NOTE(Nelson): Switch mode to Serial NAND */
	DRV_WriteReg32(RW_SNAND_CNFG, 1);

	/* Set sample delay */
	DRV_WriteReg32(RW_SNAND_DLY_CTL3, 40);

	/* NOTE(Bayi): reset device and set ecc control */
	gpt_busy_wait_us(SNAND_MAX_RDY_AFTER_RST_LATENCY_US);
	mtk_snand_reset_device();
	/* NOTE(Bayi): disable internal ecc, and use mtk ecc */
	mtk_snand_ecc_control(false);
	mtk_snand_dev_unlock_all_blocks();


	#ifdef CONFIG_PM
	host->saved_para.suspend_flag = 0;
	#endif
	/* Reset */
}

/* ------------------------------------------------------------------------------- */
static int mtk_snand_dev_ready(struct mtd_info *mtd)
{
	u8  reg8;
	u32 reg32;

	if (SNAND_DEV_PROG_EXECUTING == g_snand_dev_status) {
		DRV_WriteReg32(RW_SNAND_GPRAM_DATA, (SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8)));
		DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
		DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

		mtk_snand_dev_mac_op(SPI);

		reg8 = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if (0 == (reg8 & SNAND_STATUS_OIP)) /* ready */
		{
			g_snand_dev_status = SNAND_DEV_PROG_DONE;

			if (0 != (reg8 & SNAND_STATUS_PROGRAM_FAIL)) /* ready but having fail report from device */
			{
				g_snand_cmd_status = NAND_STATUS_READY | NAND_STATUS_FAIL;
			} else {
				g_snand_cmd_status = NAND_STATUS_READY;
			}
		} else {
			g_snand_cmd_status = 0;
		}
	} else if (SNAND_NFI_AUTO_ERASING == g_snand_dev_status) {
		/* wait for auto erase finish */

		reg32 = DRV_Reg32(RW_SNAND_STA_CTL1);

		if ((reg32 & SNAND_AUTO_BLKER) == 0) {
			g_snand_cmd_status = 0; /* busy */
		} else {
			g_snand_dev_status = SNAND_DEV_ERASE_DONE;

			reg8 = (u8)(DRV_Reg32(RW_SNAND_GF_CTL1) & SNAND_GF_STATUS_MASK);
			/* printk("Erase status: RW_SNAND_GF_CTL1: 0x%x\n", reg8); */
			if (0 != (reg8 & SNAND_STATUS_ERASE_FAIL)) /* ready but having fail report from device */
			{
				g_snand_cmd_status = NAND_STATUS_READY | NAND_STATUS_FAIL;
			} else {
				g_snand_cmd_status = NAND_STATUS_READY;
			}
		}
	} else if (SNAND_NFI_CUST_READING == g_snand_dev_status) {
		g_snand_cmd_status = 0; /* busy */
	} else {
		g_snand_cmd_status = NAND_STATUS_READY; /* idle */
	}

	return g_snand_cmd_status;
}

/******************************************************************************
 * mtk_snand_probe
 *
 * DESCRIPTION:
 *   register the nand device file operations !
 *
 * PARAMETERS:
 *   struct platform_device *pdev : device structure
 *
 * RETURNS:
 *   0 : Success
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/

int board_nand_init(struct nand_chip *nand_chip)
{
	struct mtk_nand_host_hw *hw;
	struct mtd_info *mtd;
	int err = 0;
	u8 id[SNAND_MAX_ID];
	int i;
	struct mtk_nand_host *host_ptr;

	/* hw = (struct mtk_nand_host_hw *)pdev->dev.platform_data;	TODO(Nelson): need check!!!  */
	hw = &mtk_nand_hw;
	BUG_ON(!hw);

	/* Allocate memory for the device structure (and zero it) */
	host_ptr = kzalloc(sizeof(struct mtk_nand_host), GFP_KERNEL);
	if (!host_ptr) {
		MSG(INIT, "mtk_nand: failed to allocate device structure.\n");
		return -ENOMEM;
	}

	/* Allocate memory for 16 byte aligned buffer */
	local_buffer_16_align = local_buffer;

	host_ptr->hw = hw;

	/* init mtd data structure */
	host = host_ptr;
	mtd = &host->mtd;
	nand_chip->priv = mtd;
	mtd->priv = nand_chip;

	mtd->name = "MTK-SNand";
	hw->nand_ecc_mode = NAND_ECC_HW;

	/* Set address of NAND IO lines */
	/* nand_chip->IO_ADDR_R = (void __iomem *)NFI_DATAR_REG32; */
	/* nand_chip->IO_ADDR_W = (void __iomem *)NFI_DATAW_REG32; */
	nand_chip->IO_ADDR_R = NULL;
	nand_chip->IO_ADDR_W = NULL;
	nand_chip->chip_delay = 20; /* 20us command delay time */
	nand_chip->ecc.mode = hw->nand_ecc_mode;    /* enable ECC */

	nand_chip->read_byte = mtk_snand_read_byte;
	nand_chip->read_buf = mtk_snand_read_buf;
	nand_chip->write_buf = mtk_snand_write_buf;
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	nand_chip->verify_buf = mtk_snand_verify_buf;
#endif
	nand_chip->select_chip = mtk_snand_select_chip;
	nand_chip->dev_ready = mtk_snand_dev_ready;
	nand_chip->cmdfunc = mtk_snand_command_bp;
	nand_chip->ecc.read_page = mtk_snand_read_page_hwecc;
	nand_chip->ecc.write_page = mtk_snand_write_page_hwecc;

	nand_chip->ecc.layout = &nand_oob_64;
	nand_chip->ecc.size = hw->nand_ecc_size;    /* 2048 */
	nand_chip->ecc.bytes = hw->nand_ecc_bytes;  /* 32 */

	nand_chip->options = NAND_SKIP_BBTSCAN;

	/* For BMT, we need to revise driver architecture */
	nand_chip->write_page = mtk_snand_write_page;
	nand_chip->read_page = mtk_snand_read_page;
	nand_chip->ecc.write_oob = mtk_snand_write_oob;
	nand_chip->ecc.read_oob = mtk_snand_read_oob;
	nand_chip->block_markbad = mtk_snand_block_markbad;   /* need to add nand_get_device()/nand_release_device(). */
	nand_chip->erase = mtk_nand_erase;
	nand_chip->block_bad = mtk_snand_block_bad;
	nand_chip->init_size = mtk_nand_init_size;

	mtk_nand_init_hw(host);

	/* Select the device */
	nand_chip->select_chip(mtd, NFI_DEFAULT_CS);

	/*
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up
	 */
	nand_chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Send the command for reading device ID */
	nand_chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	for (i = 0; i < SNAND_MAX_ID; i++) {
		id[i]=nand_chip->read_byte(mtd);
	}

	manu_id = id[0];
	dev_id = id[1];

	if (!mtk_snand_get_device_info(id, &devinfo)) {
		MSG(INIT, "Not Support this Device! \r\n");
	}


	/* Sample delay adjust */
	/* DRV_WriteReg32(RW_SNAND_DLY_CTL3, devinfo.SNF_DLY_CTL3); */

	if (devinfo.advancedmode & SNAND_ADV_TWO_DIE) {
		mtk_snand_dev_die_select_op(1);
		/* disable internal ecc, and use mtk ecc */
		mtk_snand_ecc_control(false);
		mtk_snand_dev_unlock_all_blocks();
	}
	if (devinfo.pagesize == 4096) {
		nand_chip->ecc.layout = &nand_oob_128;
		hw->nand_ecc_size = 4096;
	} else if (devinfo.pagesize == 2048) {
		nand_chip->ecc.layout = &nand_oob_64;
		hw->nand_ecc_size = 2048;
	} else if (devinfo.pagesize == 512) {
		nand_chip->ecc.layout = &nand_oob_16;
		hw->nand_ecc_size = 512;
	}

	nand_chip->ecc.layout->eccbytes = devinfo.sparesize-OOB_AVAI_PER_SECTOR * (devinfo.pagesize/NAND_SECTOR_SIZE);
	hw->nand_ecc_bytes = nand_chip->ecc.layout->eccbytes;

	/* Modify to fit device character */
	nand_chip->ecc.size = hw->nand_ecc_size;
	nand_chip->ecc.bytes = hw->nand_ecc_bytes;
	/* MSG(INIT, "BAYI-TEST 6, nand_chip->ecc.size:%d, nand_chip->ecc.bytes: %d, devinfo.sparesize:0x%x\n", nand_chip->ecc.size, nand_chip->ecc.bytes, devinfo.sparesize); */

	for (i = 0; i < nand_chip->ecc.layout->eccbytes; i++) {
		nand_chip->ecc.layout->eccpos[i]=OOB_AVAI_PER_SECTOR * (devinfo.pagesize/NAND_SECTOR_SIZE) + i;
	}

	/* MSG(INIT, "[NAND] pagesz:%d, eccbytes: %d\n", */
	/* devinfo.pagesize, nand_chip->ecc.layout->eccbytes); */

	/* MSG(INIT, "Support this Device in MTK table! %x \r\n", id); */
	hw->nfi_bus_width = 4;
	/* DRV_WriteReg32(NFI_ACCCON_REG32, NFI_DEFAULT_ACCESS_TIMING); */

	mtd->oobsize = devinfo.sparesize;

	nand_chip->options |= mtk_nand_init_size(mtd, nand_chip, NULL);
	mtd->writesize = devinfo.pagesize;
	nand_chip->chipsize = (((uint64_t)devinfo.totalsize) <<20);
	nand_chip->page_shift = ffs(mtd->writesize) - 1;
	nand_chip->phys_erase_shift = ffs(devinfo.blocksize * 1024)-1;
	nand_chip->numchips = 1;
	nand_chip->pagemask = (nand_chip->chipsize >> nand_chip->page_shift) - 1;

	g_page_size = mtd->writesize;
	g_block_size = devinfo.blocksize << 10;

	nand_chip->select_chip(mtd, 0);
	mtd->size = nand_chip->chipsize;

	mtd->size = nand_chip->chipsize;

#ifdef _MTK_NAND_DUMMY_DRIVER_
	dummy_driver_debug = 0;
#endif

	/* Successfully!! */
	if (!err) {
		MSG(INIT, "[mtk_snand] probe successfully!\n");
		nand_disable_clock();
		return err;
	}

	MSG(INIT, "[mtk_snand] mtk_snand_probe fail, err = %d!\n", err);
	nand_release(mtd);

	kfree(host);
	nand_disable_clock();
	return err;
}

