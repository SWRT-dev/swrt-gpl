//#if !defined (ON_BOARD_NAND_FLASH_COMPONENT)
#if 0
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/time.h>
#include <linux/miscdevice.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>

#include <mach/mt_typedefs.h>
#include <mach/mt_clkmgr.h>
#include <mach/mtk_nand.h>
#include "board-custom.h"
#include "pmt.h"
#include "partition_define.h"
#include <mach/board.h>
#else
#include <common.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
//#include <asm/cacheflush.h>
//#include <asm/uaccess.h>

#include <asm/arch-mt7622/mt_typedefs.h>
//#include <asm/arch-mt7622/mt_clkmgr.h>
#include <asm/arch-mt7622/nand/mtk_nand.h>
#include <asm/arch-mt7622/nand/partition_define.h>
#include <asm/arch-mt7622/nand/pmt.h>
//#include <asm/arch-mt7622/nand/partition_define_private.h>
#endif

#define PMT 1
#ifdef PMT

pt_resident new_part[PART_MAX_COUNT];
pt_resident lastest_part[PART_MAX_COUNT];
unsigned char part_name[PART_MAX_COUNT][MAX_PARTITION_NAME_LEN];
struct excel_info PartInfo[PART_MAX_COUNT];

int block_size;
int page_size;
pt_info pi;
u8 sig_buf[PT_SIG_SIZE];
//not support add new partition automatically.

struct mtd_partition g_pasStatic_Partition[PART_MAX_COUNT];
int part_num;
//struct excel_info PartInfo[PART_MAX_COUNT];
#define MTD_SECFG_STR "seccnfg"
#define MTD_BOOTIMG_STR "boot"
#define MTD_ANDROID_STR "system"
#define MTD_SECRO_STR "secstatic"
#define MTD_USRDATA_STR "userdata"

DM_PARTITION_INFO_PACKET pmtctl;
struct mtd_partition g_exist_Partition[PART_MAX_COUNT];

//#define LPAGE 2048
//char page_buf[LPAGE+64];
//char page_readbuf[LPAGE];
char *page_buf;
char *page_readbuf;

#define  PMT_MAGIC	 'p'
#define PMT_READ		_IOW(PMT_MAGIC, 1, int)
#define PMT_WRITE 		_IOW(PMT_MAGIC, 2, int)
#define PMT_VERSION 	_IOW(PMT_MAGIC, 3, int)


extern bool g_bInitDone;
extern struct mtk_nand_host *host;

#if defined(CONFIG_MTK_SPI_NAND_SUPPORT)
extern snand_flashdev_info gn_devinfo;
#else
extern flashdev_info gn_devinfo;
#endif
typedef u32 (*GetLowPageNumber)(u32 pageNo);
extern GetLowPageNumber functArray[];

enum partition_type{	TYPE_RAW,	TYPE_UBIFS,	TYPE_YAFFS,};
static enum partition_type partition_type_array[] =
{
	TYPE_RAW,
	TYPE_RAW,
	TYPE_RAW,
	TYPE_YAFFS,
#if !defined(CONFIG_RAWFS_FS)
	TYPE_YAFFS,
#endif
	TYPE_RAW,
	TYPE_RAW,
	TYPE_RAW,
	TYPE_RAW,
	TYPE_RAW,
	TYPE_RAW,
#if defined(CONFIG_MTK_FACTORY_RESET_PROTECTION_SUPPORT)
	TYPE_RAW,
#endif
	TYPE_RAW,
	TYPE_RAW,
#if defined(CONFIG_MTK_FAT_ON_NAND)
	TYPE_UBIFS,
#endif
	TYPE_UBIFS,
	TYPE_UBIFS,
	TYPE_UBIFS,
	TYPE_RAW,};
#if defined(CONFIG_MTK_MLC_NAND_SUPPORT)
static bool MLC_DEVICE = TRUE;
#else
static bool MLC_DEVICE = FALSE;
#endif

bool init_pmt_done = FALSE;
void get_part_tab_from_complier(void)
{
    printk(KERN_INFO "get_pt_from_complier \n");

    memcpy(&g_exist_Partition, &g_pasStatic_Partition, sizeof(struct mtd_partition) * PART_MAX_COUNT);
}

u64 part_get_startaddress(u64 byte_address, u32 *idx)
{
    int index = 0;
	if(TRUE == init_pmt_done)
	{
    while (index < part_num) {
		//MSG(INIT, "g_exist_Partition[%d].offset %x\n",index, g_exist_Partition[index].offset);
		if(g_exist_Partition[index].offset > byte_address)
		{
			//MSG(INIT, "find g_exist_Partition[%d].offset %x\n",index-1, g_exist_Partition[index-1].offset);
			*idx = index-1;
			return g_exist_Partition[index-1].offset;
		}
        index++;
    }
	}
#if defined (CONFIG_NAND_BOOTLOADER)
	else
	{
		*idx = PART_MAX_COUNT -1;
		return byte_address;
	}
#endif
	*idx = part_num-1;
    return byte_address;
}

bool raw_partition(u32 index)
{
	if(partition_type_array[index] == TYPE_RAW || partition_type_array[index] == TYPE_YAFFS )
		return TRUE;
	return FALSE;
}

int find_empty_page_from_top(u64 start_addr, struct mtd_info *mtd)
{
    int page_offset;//,i;
    u64 current_add;
#if defined(MTK_MLC_NAND_SUPPORT)
    int i;
#endif
    struct mtd_oob_ops ops_pt;
    struct erase_info ei;

    ei.mtd = mtd;
    ei.len = mtd->erasesize;
    ei.time = 1000;
    ei.retries = 2;
    ei.callback = NULL;

    ops_pt.datbuf = (uint8_t *)page_buf;
    ops_pt.mode = MTD_OPS_AUTO_OOB;
    ops_pt.len = mtd->writesize;
    ops_pt.retlen = 0;
    ops_pt.ooblen = 16;
    ops_pt.oobretlen = 0;
    ops_pt.oobbuf = (uint8_t *)(page_buf + page_size);
    ops_pt.ooboffs = 0;
    memset(page_buf, 0xFF, page_size + mtd->oobsize);
    memset(page_readbuf, 0xFF, page_size);
    //mt6577_nand_erase(start_addr); //for test
#if defined(MTK_MLC_NAND_SUPPORT)
    for (page_offset = 0,i=0; page_offset < (block_size / page_size); page_offset = functArray[gn_devinfo.feature_set.ptbl_idx](i++))
#else
    for (page_offset = 0; page_offset < (block_size / page_size); page_offset++)
#endif
    {
        current_add = start_addr + (page_offset * page_size);
        if (mtd->_read_oob(mtd, (loff_t) current_add, &ops_pt) != 0)
        {
            printk(KERN_INFO "find_emp read failed %llx \n", current_add);
            continue;
        } else
        {
            if (memcmp(page_readbuf, page_buf, page_size) || memcmp(page_buf + page_size, page_readbuf, 32))
            {
                continue;
            } else
            {
                printk(KERN_INFO "find_emp  at %x \n", page_offset);
                break;
            }

        }
    }
    printk(KERN_INFO "find_emp find empty at %x \n", page_offset);

    //i=(0x40);
    //printk (KERN_INFO "test code %x \n",i);
    //page_offset = 0x40;
    if (page_offset != 0x40)
    {
        printk(KERN_INFO "find_emp at  %x\n", page_offset);
        return page_offset;
    } else
    {
        printk(KERN_INFO "find_emp no empty \n");
	ei.addr =  start_addr;
        if (mtd->_erase(mtd, &ei) != 0)
        {                       //no good block for used in replace pool
            printk(KERN_INFO "find_emp erase mirror failed %llx\n", start_addr);
            pi.mirror_pt_has_space = 0;
            return 0xFFFF;
        } else
        {
            return 0;           //the first page is empty
        }

    }
}

bool find_mirror_pt_from_bottom(u64 *start_addr, struct mtd_info * mtd)
{
    int mpt_locate;
    u64 mpt_start_addr;
    u64 current_start_addr = 0;
    u8 pmt_spare[4];
    struct mtd_oob_ops ops_pt;

#if defined (CONFIG_NAND_BOOTLOADER)
    mpt_start_addr = (mtd->size)-(u64)(((PMT_POOL_SIZE)+(g_bmt_sz)) * block_size) + block_size;//((mtd->size) + block_size);
#else
    mpt_start_addr = ((mtd->size) + block_size);
#endif
    //mpt_start_addr=MPT_LOCATION*block_size-page_size;
    memset(page_buf, 0xFF, page_size + mtd->oobsize);

    ops_pt.datbuf = (uint8_t *) page_buf;
    ops_pt.mode = MTD_OPS_AUTO_OOB;
    ops_pt.len = mtd->writesize;
    ops_pt.retlen = 0;
    ops_pt.ooblen = 16;
    ops_pt.oobretlen = 0;
    ops_pt.oobbuf = (uint8_t *)(page_buf + page_size);
    ops_pt.ooboffs = 0;
    printk(KERN_INFO "find_mirror find begain at %llx \n", mpt_start_addr);

    for (mpt_locate = ((block_size / page_size) - 1); mpt_locate >= 0; mpt_locate--)//mpt_locate--)
    {
        memset(pmt_spare, 0xFF, PT_SIG_SIZE);

        current_start_addr = mpt_start_addr + mpt_locate * page_size;
        if (mtd->_read_oob(mtd, (loff_t) current_start_addr, &ops_pt) != 0)
        {
            printk(KERN_INFO "find_mirror read  failed %llx %x \n", current_start_addr, mpt_locate);
        }
        memcpy(pmt_spare, &page_buf[page_size], PT_SIG_SIZE);   //auto do need skip bad block
        //need enhance must be the larget sequnce number
#if 0
        {
            int i;
            for (i = 0; i < 8; i++)
            {
                printk(KERN_INFO "%x %x \n", page_buf[i], page_buf[2048 + i]);
            }

        }
#endif
        if (is_valid_mpt((void *)page_buf)/* && is_valid_mpt((char *)pmt_spare)*/)
        {
            //if no pt, pt.has space is 0;
            pi.sequencenumber = page_buf[PT_SIG_SIZE + page_size];
            printk(KERN_INFO "find_mirror find valid pt at %llx sq %x \n", current_start_addr, pi.sequencenumber);
            break;
        } else
        {
            continue;
        }
    }
    if (mpt_locate == -1)
    {
        printk(KERN_INFO "no valid mirror page\n");
        pi.sequencenumber = 0;
        return FALSE;
    } else
    {
        *start_addr = current_start_addr;
        return TRUE;
    }
}

//int load_exist_part_tab(u8 *buf,struct mtd_info *mtd)
int load_exist_part_tab(u8 * buf)
{
    u64 pt_start_addr;
    u64 pt_cur_addr;
    int pt_locate;
    int reval = DM_ERR_OK;
    u64 mirror_address;

    //u8 pmt_spare[PT_SIG_SIZE];
    struct mtd_oob_ops ops_pt;
    struct mtd_info *mtd;
    mtd = &host->mtd;

    block_size = mtd->erasesize;    // gn_devinfo.blocksize*1024;
    page_size = mtd->writesize; // gn_devinfo.pagesize;
    //if(host->hw->nand_sec_shift == 10) //MLC
    //	block_size = block_size >> 1;
#if defined (CONFIG_NAND_BOOTLOADER)
    pt_start_addr = (mtd->size)-(u64)(((PMT_POOL_SIZE)+(g_bmt_sz)) * block_size) ;
#else
    pt_start_addr = (mtd->size);
#endif
    //pt_start_addr=PT_LOCATION*block_size;
    printk(KERN_INFO "load_exist_part_tab %llx\n", pt_start_addr);
    ops_pt.datbuf = (uint8_t *) page_buf;
    ops_pt.mode = MTD_OPS_AUTO_OOB;
    ops_pt.len = mtd->writesize;
    ops_pt.retlen = 0;
    ops_pt.ooblen = 16;
    ops_pt.oobretlen = 0;
    ops_pt.oobbuf = (uint8_t *)(page_buf + page_size);
    ops_pt.ooboffs = 0;

    printk(KERN_INFO "ops_pt.len %x \n", ops_pt.len);
    if (mtd->_read_oob == NULL)
    {
        printk(KERN_INFO "shoud not happpen \n");
    }

    for (pt_locate = 0; pt_locate < (block_size / page_size); pt_locate++)
    {
        pt_cur_addr = pt_start_addr + pt_locate * page_size;
        //memset(pmt_spare,0xFF,PT_SIG_SIZE);

        //printk (KERN_INFO "load_pt read pt %x \n",pt_cur_addr);

        if (mtd->_read_oob(mtd, (loff_t) pt_cur_addr, &ops_pt) != 0)
        {
            printk(KERN_INFO "load_pt read pt failded: %llx\n", (u64) pt_cur_addr);
        }
#if 0
        {
            int i;
            for (i = 0; i < 8; i++)
            {
                printk(KERN_INFO "%x %x \n", *(page_buf + i), *(page_buf + 2048 + i));
            }

        }
#endif
        //memcpy(pmt_spare,&page_buf[LPAGE] ,PT_SIG_SIZE); //do not need skip bad block flag
        if (is_valid_pt((void *)page_buf) && is_valid_pt((void *)(page_buf + mtd->writesize)))
        {
            pi.sequencenumber = page_buf[PT_SIG_SIZE + page_size];
            printk(KERN_INFO "load_pt find valid pt at %llx sq %x \n", pt_start_addr, pi.sequencenumber);
            break;
        } else
        {
            continue;
        }
    }
    //for test
    //pt_locate=(block_size/page_size);
    if (pt_locate == (block_size / page_size))
    {
        //first download or download is not compelte after erase or can not download last time
        printk(KERN_INFO "load_pt find pt failed \n");
        pi.pt_has_space = 0;    //or before download pt power lost

        if (!find_mirror_pt_from_bottom(&mirror_address, mtd))
        {
            printk(KERN_INFO "First time download \n");
            reval = ERR_NO_EXIST;
            return reval;
        } else
        {
            //used the last valid mirror pt, at lease one is valid.
            mtd->_read_oob(mtd, (loff_t) mirror_address, &ops_pt);
        }
    }
    memcpy(&lastest_part, &page_buf[PT_SIG_SIZE], sizeof(lastest_part));

    return reval;
}

#if !defined (CONFIG_NAND_BOOTLOADER)
static struct file_operations pmt_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = pmt_ioctl,
    .open = pmt_open,
    .release = pmt_release,
};

static struct miscdevice pmt_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "pmt",
    .fops = &pmt_fops,
};
#endif
static int lowercase(int c)
{
	if((c >= 'A') && (c <= 'Z'))
		c += 'a' - 'A';
	return c;
}

void construct_mtd_partition(struct mtd_info *mtd)
{
	int i,j;
	for(i = 0; i < PART_MAX_COUNT; i++)
	{
		if(lastest_part[i].size == 0)
			break;
		for(j=0; j < MAX_PARTITION_NAME_LEN; j++)
		{
			if(lastest_part[i].name[j] == 0)
				break;
			part_name[i][j] = lowercase(lastest_part[i].name[j]);
		}
		strcpy(PartInfo[i].name, (const char *)part_name[i]);
		strcpy(g_exist_Partition[i].name, (const char *)part_name[i]);
		if(!strcmp((const char *)lastest_part[i].name,"SECCFG"))
			g_exist_Partition[i].name = MTD_SECFG_STR;

		if(!strcmp((const char *)lastest_part[i].name,"BOOTIMG"))
			g_exist_Partition[i].name = MTD_BOOTIMG_STR;

		if(!strcmp((const char *)lastest_part[i].name,"SEC_RO"))
			g_exist_Partition[i].name = MTD_SECRO_STR;

		if(!strcmp((const char *)lastest_part[i].name,"ANDROID"))
			g_exist_Partition[i].name = MTD_ANDROID_STR;

		if(!strcmp((const char *)lastest_part[i].name,"USRDATA"))
			g_exist_Partition[i].name = MTD_USRDATA_STR;

		g_exist_Partition[i].size = (u_int32_t) lastest_part[i].size;//mtd partition
	        g_exist_Partition[i].offset = (u_int32_t) lastest_part[i].offset;
		g_exist_Partition[i].mask_flags = lastest_part[i].mask_flags;
#if 1
		if(MLC_DEVICE == TRUE)
		{
			mtd->eraseregions[i].offset = lastest_part[i].offset;
			mtd->eraseregions[i].erasesize = mtd->erasesize;
			if(partition_type_array[i] == TYPE_RAW)
			{
				mtd->eraseregions[i].erasesize = mtd->erasesize/2;
			}
			mtd->numeraseregions++;
		}
#endif

		//PartInfo[i].name = part_name[i];  //dumchar
		PartInfo[i].type = NAND;
		PartInfo[i].start_address = lastest_part[i].offset;
		PartInfo[i].size = lastest_part[i].size;
		printk("partition %s %s size %llx\n", lastest_part[i].name,PartInfo[i].name, g_exist_Partition[i].offset);
	}
	part_num = i;
	g_exist_Partition[i-1].size = MTDPART_SIZ_FULL;
}

void part_init_pmt(struct mtd_info *mtd, u8 * buf)
{
#if !defined (CONFIG_NAND_BOOTLOADER)	/* NOTE(Nelson): Need to check if it is useful? */
    struct mtd_partition *part;
    u64 lastblk;
    int retval = 0;
    int i = 0;
    int err = 0;
    printk(KERN_INFO "part_init_pmt  %s\n", __TIME__);
    page_buf = kzalloc(mtd->writesize + mtd->oobsize, GFP_KERNEL);
    page_readbuf = kzalloc(mtd->writesize, GFP_KERNEL);
#if 0
    part = &g_pasStatic_Partition[0];
    lastblk = part->offset + part->size;
    printk(KERN_INFO "offset  %llx part->size %llx %s\n", part->offset, part->size, part->name);

    while (1)
    {
        part++;
		if(part->offset == MTDPART_OFS_APPEND)
        part->offset = lastblk;
        lastblk = part->offset + part->size;
        printk(KERN_INFO "mt_part_init_pmt %llx\n", part->offset);
        if (part->size == 0)    ////the last partition sizefull ==0
        {
            break;
        }
    }
#endif
	init_pmt_done = FALSE;
    memset(&pi, 0xFF, sizeof(pi));
    memset(&lastest_part, 0, PART_MAX_COUNT * sizeof(pt_resident));
    retval = load_exist_part_tab(buf);

    if (retval == ERR_NO_EXIST) //first run preloader before dowload
    {
        //and valid mirror last download or first download
        printk(KERN_INFO "%s no pt \n", __func__);
        get_part_tab_from_complier();   //get from complier
        if(MLC_DEVICE == TRUE)
			mtd->numeraseregions = 0;
		for (i = 0; i < part_num; i++)
		{
			#if 1
			if(MLC_DEVICE == TRUE)
			{
				mtd->eraseregions[i].offset = lastest_part[i].offset;
				mtd->eraseregions[i].erasesize = mtd->erasesize;
				if(partition_type_array[i] == TYPE_RAW || partition_type_array[i] == TYPE_YAFFS)
				{
					mtd->eraseregions[i].erasesize = mtd->erasesize/2;
				}
				mtd->numeraseregions++;
			}
			#endif
        }
    } else
    {
        printk(KERN_INFO "Find pt or mpt \n");
		if(MLC_DEVICE == TRUE)
			mtd->numeraseregions = 0;
        construct_mtd_partition(mtd);
    }
	init_pmt_done = TRUE;

    printk(KERN_INFO ": register NAND PMT device ...\n");
#if !defined (CONFIG_NAND_BOOTLOADER)
#ifndef MTK_EMMC_SUPPORT

    err = misc_register(&pmt_dev);
    if (unlikely(err))
    {
        printk(KERN_INFO "PMT failed to register device!\n");
        //return err;
    }
#endif
#endif
#endif
}

int get_part_num_nand()
{
	return part_num;
}

#endif
