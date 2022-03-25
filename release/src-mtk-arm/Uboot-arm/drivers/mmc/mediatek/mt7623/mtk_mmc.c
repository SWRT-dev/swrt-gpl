
#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>


/*adapter layer for u-boot mmc*/
/*Mediatek MMC device */
extern int __mmc_init(int id);
extern int mmc_block_read(int dev_num, unsigned long blknr, u32 blkcnt, unsigned long *dst);
extern int mmc_block_write(int dev_num, unsigned long blknr, u32 blkcnt, unsigned long *src);
extern int mmc_block_erase(int dev_num, unsigned long blk_start, unsigned long blk_end);
extern int mmc_part_config(int dev_num, u8 ack, u8 part_num, u8 access);

int is_mtk_init = 0;

unsigned long  __mmc_block_read(int dev,
                      lbaint_t start,
                      lbaint_t blkcnt,
                      void *buffer){
if(start > (lbaint_t) 0xffffffff) return -1;
if(blkcnt > (lbaint_t) 0xffffffff) return -1;
return   mmc_block_read(dev, (unsigned long) start, (u32) blkcnt, buffer);
}


unsigned long   __mmc_block_write(int dev,
                       lbaint_t start,
                       lbaint_t blkcnt,
                       const void *buffer){
if(start > (lbaint_t) 0xffffffff) return -1;
if(blkcnt > (lbaint_t) 0xffffffff) return -1;
return   mmc_block_write(dev, (unsigned long) start, (u32) blkcnt, (void *)buffer);
}

unsigned long   __mmc_block_erase(int dev,
                       lbaint_t start,
                       lbaint_t blkcnt){
if(start > (lbaint_t) 0xffffffff) return -1;
if(blkcnt > (lbaint_t) 0xffffffff) return -1;
return mmc_block_erase(dev, (unsigned long)start, (unsigned long) blkcnt);
}

block_dev_desc_t g_mtk_mmc_block = {
    .lun = 0,
    .type = 0,
    .blksz = 512,
    .log2blksz = 9,
    .if_type = IF_TYPE_MMC,
    .dev = 1,
    .removable = 1,    
    .block_read = __mmc_block_read, 
    .block_write = __mmc_block_write,
    .block_erase = 0
};


block_dev_desc_t* mmc_get_dev(int dev) { 
        if(!is_mtk_init) 
            return 0;
        else {         
            return &g_mtk_mmc_block; 
        }
}

int mmc_info_helper(unsigned int *lba, char* vendor, char *product, char *revision);
int mmc_legacy_init(int id){

    int ret = 0, ret2=0;
    int i = 0;
    char buf[512];

    
    ret = __mmc_init(1);
    is_mtk_init = 1;

    /*test for read*/
    ret2 = mmc_block_read(1, 0, 1, (long unsigned int *)&buf[0]);
    printf("ret2 = %d\n", ret2);
    ret2 = mmc_block_read(1, 0, 1, (long unsigned int *)&buf[0]);
    printf("ret2 = %d\n", ret2);
    
    mmc_info_helper(
                (unsigned int *)&g_mtk_mmc_block.lba, 
                 g_mtk_mmc_block.vendor, 
                 g_mtk_mmc_block.product, 
                 g_mtk_mmc_block.revision) ; 

    

    printf("<= [mmc1 block 0] =>\n");
    for ( i = 0 ; i < 512 ; i+=8) {
        printf("[0x%08x] %02x %02x %02x %02x %02x %02x %02x %02x\n", 
            i,
            buf[i],
            buf[i+1],
            buf[i+2],
            buf[i+3],
            buf[i+4],
            buf[i+5],
            buf[i+6],
            buf[i+7]);
        }  
    
    //printf("lba = %d\n ", g_mtk_mmc_block.lba);
    init_part(&g_mtk_mmc_block);
    print_part(&g_mtk_mmc_block);



    return ret;
}



/*UBOOT MMC*/
/*EMMC*/
block_dev_desc_t g_mtk_mmc_block_emmc = {
    .lun = 0,
    .type = 0,
    .blksz = 512,
    .log2blksz = 9,
    .if_type = IF_TYPE_MMC,
    .dev = 0,
    .removable = 1,    
    .block_read =  __mmc_block_read, 
    .block_write = __mmc_block_write,
    .block_erase = __mmc_block_erase
};

/*EMMC*/
block_dev_desc_t g_mtk_mmc_block_sd = {
    .lun = 0,
    .type = 0,
    .blksz = 512,
    .log2blksz = 9,
    .if_type = IF_TYPE_MMC,
    .dev = 1,
    .removable = 1,    
    .block_read =  __mmc_block_read, 
    .block_write = __mmc_block_write,
    .block_erase = __mmc_block_erase
};

struct mmc_data_priv {
    char id;
};
static int cur_dev_num = -1;
static struct mmc mtk_mmc[1]; //0:emmc 1:sd
static struct mmc_data_priv mtk_mmc_data_priv[1];

/*UBOOT MMC LAYER*/
int get_mmc_num(void)
{
	return cur_dev_num;
}

struct mmc *find_mmc_device(int dev_num)
{
    if(dev_num > 1 || dev_num < 0) 
        return 0;
    else {
        printf("dev_num = %d\n", dev_num);
        mtk_mmc[dev_num].priv = &mtk_mmc_data_priv[dev_num];
        mtk_mmc_data_priv[dev_num].id = dev_num;
        return &mtk_mmc[dev_num];
        }
}

int mmc_info_helper2(int id,
     char *cid, int cid_len,
     unsigned int *speed,
     unsigned int *read_bl_len,
     unsigned int *scr, int scr_len,
     unsigned int *high_capacity,
     unsigned int *blk_num,
     unsigned int *bus_width);

int mmc_init(struct mmc *mmc){

    char id ;
    int scr[2];
    unsigned int blk_num;
    struct  mmc_data_priv *priv;
    priv = mmc->priv;
    id = priv->id;
    if(id == 0){ 
       strcpy(mmc->name,"emmc"); 
       mmc->block_dev = g_mtk_mmc_block_emmc;
    }
    else if(id == 1) { 
       strcpy(mmc->name,"sdcard");
       mmc->block_dev = g_mtk_mmc_block_sd;
    }
    __mmc_init(id);

    mmc_info_helper2(
        id,
        (char *)mmc->cid, sizeof(mmc->cid),
        &mmc->tran_speed,
        &mmc->read_bl_len,
        (unsigned int *)&scr[0], sizeof(scr), 
        (unsigned int *)&mmc->high_capacity,
        &blk_num,
        &mmc->bus_width
    ) ; 

    mmc->scr[0] = scr[0];
	mmc->scr[1] = scr[1];

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			if ((mmc->scr[0] >> 15) & 0x1)
				mmc->version = SD_VERSION_3;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}
    mmc->capacity = blk_num;
    mmc->capacity =  mmc->capacity * (u64)mmc->read_bl_len;
    mmc->write_bl_len = mmc->read_bl_len;
//printf("read_bl_len=%d, write_bl_len=%d\n", mmc->read_bl_len, mmc->write_bl_len);

return 0;

}

int mmc_getwp(struct mmc *mmc)
{
    return 0;
}

/*
 * Modify EXT_CSD[179] which is PARTITION_CONFIG (formerly BOOT_CONFIG)
 * based on the passed in values for BOOT_ACK, BOOT_PARTITION_ENABLE and
 * PARTITION_ACCESS.
 *
 * Returns 0 on success.
 */
int mmc_set_part_conf(struct mmc *mmc, u8 ack, u8 part_num, u8 access)
{
	struct  mmc_data_priv *priv;
	priv = mmc->priv;
	int id = priv->id;
	int err;

	err = mmc_part_config(id, ack, part_num, access);

	if (err)
		return err;

	return 0;
}
