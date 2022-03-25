/*
 * (C) Copyright 2012 Stephen Warren
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>
#include <configs/autoconf.h>

//#define DEBUG

/**********************************************************************************************
 *                                      ARM Cortex A7
 **********************************************************************************************/
#if 1
/*
 *  Iverson 20140326 : 
 *      We can disable dcache for accessing non-cachable address instead of using mmu page table to provide noncachable address.
 */
/* Cache */
// Iverson debug
//#define CONFIG_SYS_DCACHE_OFF
#endif

/* Machine ID */
#define CONFIG_MACH_TYPE                    7622
#define CONFIG_SUPPORT_LINUX32BIT	        0


/**********************************************************************************************
 *                                          Memory
 **********************************************************************************************/
/* Memory layout */
/* DRAM definition */
/* 
 * Iverson 20140521 : We detect ram size automatically.
 *      CONFIG_SYS_SDRAM_SIZE define max uboot size.
 *      The max size that auto detection support is 256MB.
 */
#define CONFIG_NR_DRAM_BANKS		        1
#define CONFIG_SYS_SDRAM_BASE		        0x40000000

#if CONFIG_CUSTOMIZE_DRAM_SIZE
#if defined(ON_BOARD_512Mb_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_64M
#elif defined(ON_BOARD_1024Mb_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_128M
#elif defined(ON_BOARD_2048Mb_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_256M
#elif defined(ON_BOARD_4096Mb_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_512M
#elif defined(ON_BOARD_8192Mb_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_1G
#elif defined(ON_BOARD_2GB_DRAM_COMPONENT)
#define CONFIG_SYS_SDRAM_SIZE               SZ_2G
#endif
#endif

/* Code Layout */
//#define CONFIG_SYS_TEXT_BASE		        0x80000000
#define CONFIG_SYS_TEXT_BASE		        0x41E00000

/* Uboot definition */
#define CONFIG_SYS_UBOOT_BASE		        CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_UBOOT_MAX_SIZE           SZ_2M
#define CONFIG_SYS_INIT_SP_ADDR             (CONFIG_SYS_TEXT_BASE + \
                                                CONFIG_SYS_UBOOT_MAX_SIZE - \
                                                GENERATED_GBL_DATA_SIZE)

#if defined(MT7622_FPGA_BOARD)
#define CONFIG_SYS_MALLOC_LEN               SZ_4M
#else
#define CONFIG_SYS_MALLOC_LEN               SZ_32M
#endif

#if CONFIG_SUPPORT_LINUX32BIT
#define CONFIG_SYS_DECOMP_ADDR              0x40008000
#define CONFIG_SYS_LOAD_ADDR                0x42000000
#else
/* RichOS memory partitions */
#define CONFIG_SYS_DECOMP_ADDR              0x40080000
#if defined(MT7622_FPGA_BOARD)
#define CONFIG_SYS_LOAD_ADDR                0x4007FF28
#else
#define CONFIG_SYS_LOAD_ADDR                0x4007FF28
#endif
#endif

#define CFG_HTTP_DL_ADDR		CONFIG_SYS_LOAD_ADDR

#define CONFIG_SYS_IMAGE_HDR_ADDR           CONFIG_SYS_LOAD_ADDR

/* Linux DRAM definition */
#define CONFIG_LOADADDR			            CONFIG_SYS_LOAD_ADDR

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTM_LEN	            0x4000000

/**********************************************************************************************
 *                                           Board
 **********************************************************************************************/

/* Board */
#if defined(MT7622_FPGA_BOARD)
#define CFG_FPGA_PLATFORM   1
#endif
#define CONFIG_ADD_MTK_HEADER               "y"

#define CONFIG_BOARD_LATE_INIT

/**********************************************************************************************
 *                                          Devices
 **********************************************************************************************/

/********************** Flash *************************/
#if defined(ON_BOARD_SPI_NOR_FLASH_COMPONENT)
/* SPI Flash Configuration */

/* Parallel Nor Flash */
#define CONFIG_SYS_NO_FLASH

/* SPI NOR Flash */
#define CONFIG_CMD_NOR

#define CONFIG_ENV_IS_IN_NOR

#define CONFIG_ENV_SIZE                     SZ_4K
#define CONFIG_ENV_SECT_SIZE                0x10000	/* 64KB */
#define CONFIG_ENV_OFFSET                   0xA0000

#define CONFIG_NOR_PL_OFFSET               0x00000 //Preloader offset
#define CONFIG_NOR_PL_SIZE                 0x40000 //Preloader max size 256K         

#define CONFIG_NOR_ATF_OFFSET              0x40000 //ATF begin offset
#define CONFIG_NOR_ATF_SIZE                0x20000 //ATF max size 128k

#define CONFIG_NOR_UBOOT_OFFSET            0x60000 //Uboot offset 
#define CONFIG_NOR_UBOOT_SIZE              0x40000 //Uboot max size 256k

#define CONFIG_NOR_NVRAM_OFFSET            0xA0000 //NVRAM offset 
#define CONFIG_NOR_NVRAM_SIZE              0x20000 //NVRAM max size 128k

#define CONFIG_NOR_RF_OFFSET               0xC0000 //RF offset 
#define CONFIG_NOR_RF_SIZE                 0x20000 //RF max size 128k


#if defined(CONFIG_AC_MODE_OFFSET)
#define CONFIG_NOR_LINUX_OFFSET            0xE0000 //Linux begin offset
#define CONFIG_NOR_LINUX_SIZE              0xF20000 //Linux max size 15M
#else
#define CONFIG_NOR_LINUX_OFFSET            0x140000 //Linux begin offset
#define CONFIG_NOR_LINUX_SIZE              0xEC0000 //Linux max size 15M
#endif

#define CONFIG_NOR_CTP_OFFSET              0xE0000 //CTP begin offset
#define CONFIG_NOR_CTP_SIZE                0xF20000 //CTP max size 15M

#define CONFIG_NOR_FLASHIMAGE_OFFSET       0x0       //flashimage.bin begin offset
#define CONFIG_NOR_FLASHIMAGE_SIZE         0x1000000 //flashimage.bin max size 16M



#define ENV_BOOT_WRITE_IMAGE \
	"boot_wr_img=filesize_check " __stringify(CONFIG_NOR_LINUX_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_LINUX_OFFSET) " " __stringify(CONFIG_NOR_LINUX_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " " __stringify(CONFIG_NOR_LINUX_SIZE) ";fi\0"
#define ENV_BOOT_READ_IMAGE \
    	"boot_rd_img=snor read ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " 2000" \
	";image_blks 1" \
	";snor read ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " ${img_align_size}\0"
#define ENV_WRITE_UBOOT \
	"wr_uboot=filesize_check " __stringify(CONFIG_NOR_UBOOT_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_UBOOT_OFFSET) " " __stringify(CONFIG_NOR_UBOOT_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_UBOOT_OFFSET) " " __stringify(CONFIG_NOR_UBOOT_SIZE) ";fi\0"
#define ENV_WRITE_ATF \
	"wr_atf=filesize_check " __stringify(CONFIG_NOR_ATF_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_ATF_OFFSET) " " __stringify(CONFIG_NOR_ATF_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_ATF_OFFSET) " 0x20000;fi\0"
#define ENV_WRITE_ROM_HEADER \
	"wr_pl=filesize_check " __stringify(CONFIG_NOR_PL_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_PL_OFFSET) " " __stringify(CONFIG_NOR_PL_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_PL_OFFSET) " 0x40000;fi\0"
#define ENV_WRITE_PRELOADER \
	"wr_pl=filesize_check " __stringify(CONFIG_NOR_PL_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_PL_OFFSET) " " __stringify(CONFIG_NOR_PL_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_PL_OFFSET) " 0x40000;fi\0"
#define ENV_WRITE_CTP \
	"wr_ctp=filesize_check " __stringify(CONFIG_NOR_CTP_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_CTP_OFFSET) " " __stringify(CONFIG_NOR_CTP_OFFSET) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_CTP_OFFSET) " 0xF20000;fi\0" 	
#define ENV_BOOT_READ_CTP \
	"boot_rd_ctp=snor read 0x40000000 " __stringify(CONFIG_NOR_CTP_OFFSET) " 0xF20000\0"

#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=filesize_check " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) ";fi\0" 
	

#elif defined(ON_BOARD_NAND_FLASH_COMPONENT) || \
      defined(ON_BOARD_SPI_NAND_FLASH_COMPONENT)
#define CONFIG_SYS_NO_FLASH

/* NAND Flash Configuration */
#define CONFIG_SYS_MAX_NAND_DEVICE	        1	
#define CONFIG_SYS_NAND_BASE                NFI_BASE
#define CONFIG_SYS_MAX_FLASH_SECT			256	
#define CONFIG_SYS_MAX_FLASH_BANKS			1
#define CONFIG_CMD_NAND
//#define CONFIG_MTD_DEBUG	/* NOTE(Nelson): add debug logs */
//#define CONFIG_MTD_DEBUG_VERBOSE  7
#define CONFIG_CMD_MTDPARTS     1
#define CONFIG_MTD_PARTITIONS   1
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_MTD_NAND_VERIFY_WRITE            1

#if defined (CONFIG_MTK_MLC_NAND_SUPPORT)
#define CONFIG_ENV_SIZE                     SZ_4K
#define CONFIG_NAND_HEADER_OFFSET              0x0000000
#define CONFIG_NAND_PL_OFFSET                  0x0800000
#define CONFIG_NAND_UBOOT_OFFSET               0x1000000
#define CONFIG_ENV_OFFSET                      0x2000000
#define CONFIG_NAND_LINUX_OFFSET               0x4000000
#define CONFIG_NAND_ATF_OFFSET                 0x63000000
#define CONFIG_MAX_NAND_PAGE_SIZE               8192
#define CONFIG_MAX_NAND_BLOCK_SIZE              2097152
#define CONFIG_MAX_UBOOT_SIZE                   393216  //262144
#define CONFIG_NAND_CTP_OFFSET                 0x4000000
#else
#define CONFIG_ENV_SIZE                     SZ_8K
#define CONFIG_NAND_HEADER_OFFSET           0x00000
#define CONFIG_NAND_PL_OFFSET               0x00000

#define CONFIG_NAND_ATF_OFFSET              0x80000//0x2140000
#define CONFIG_NAND_UBOOT_OFFSET            0xC0000

#define CONFIG_NAND_NVRAM_OFFSET            0x140000
#define CONFIG_NAND_RF_OFFSET               0x1C0000

#define CONFIG_ENV_OFFSET                   0x140000
#if defined (CONFIG_CMD_NMRP)
#define CONFIG_STR_OFFSET                   0x5bc0000
#define CONFIG_STR_MTD_SIZE                  0x400000
#define CONFIG_STR_BLK_SIZE                   0x40000
#endif
#if defined(CONFIG_AC_MODE_OFFSET)
#define CONFIG_NAND_LINUX_OFFSET            0x200000
#else
#define CONFIG_NAND_LINUX_OFFSET            0x2C0000
#endif
#define CONFIG_MAX_NAND_PAGE_SIZE               2048
#define CONFIG_MAX_NAND_BLOCK_SIZE              131072
#define CONFIG_MAX_UBOOT_SIZE                           0x80000  //262144
#define CONFIG_NAND_CTP_OFFSET              0x1400000

#define CONFIG_NAND_FLASHIMAGE_OFFSET       0x0

#define CONFIG_MAX_NAND_LINUX_SIZE              0x2600000
#define CONFIG_MAX_NAND_FLASHIMAGE_SIZE         0x8000000
#define CONFIG_MAX_NAND_ATF_SIZE                0x20000
#define CONFIG_MAX_NAND_PL_SIZE                 0x40000 //Preloader max size 256K  
#define CONFIG_MAX_NAND_ATF_SIZE                0x20000 //ATF max size 128k
#define CONFIG_MAX_NAND_CTP_SIZE                0xF20000 //CTP max size 15M


#endif

#if defined (CONFIG_MTK_DUAL_IMAGE_SUPPORT)
#define ENV_BOOT_WRITE_IMAGE \
	    "boot_wr_img=decrypt_image;if test ${decrypt_result} = good; then filesize_check " __stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
		";if test ${filesize_result} = good; then check_image; setenv skip_load_os 1;bootm;setenv skip_load_os 0"\
		";if test ${check_os_error} = 0; then nand erase.spread " __stringify(CONFIG_NAND_LINUX_OFFSET) "  ${filesize}" \
		";nand write ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " ${filesize};else echo Bad os !!; fi;fi;fi\0"	
#else
#define ENV_BOOT_WRITE_IMAGE \
	    "boot_wr_img=decrypt_image;if test ${decrypt_result} = good; then filesize_check " __stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
		";if test ${filesize_result} = good; then image_blks " __stringify(CONFIG_MAX_NAND_BLOCK_SIZE) \
		";nand erase.spread " __stringify(CONFIG_NAND_LINUX_OFFSET) "  ${filesize}" \
		";image_blks " __stringify(CONFIG_MAX_NAND_PAGE_SIZE) \
		";nand write ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " ${filesize};fi;fi\0"	
#endif
#define ENV_BOOT_READ_IMAGE \
    "boot_rd_img=nand read ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " " __stringify(CONFIG_MAX_NAND_LINUX_SIZE) ";check_image\0"
#define ENV_WRITE_UBOOT \
	"wr_uboot=filesize_check " __stringify(CONFIG_MAX_UBOOT_SIZE) \
	";if test ${filesize_result} = good; then mtk_image_blks " __stringify(CONFIG_MAX_NAND_BLOCK_SIZE) \
	";if test ${check_boot_error} = 0;then nand erase.spread " __stringify(CONFIG_NAND_UBOOT_OFFSET) "  ${filesize} "\
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_UBOOT_OFFSET) " ${filesize};fi;fi\0"
#define ENV_WRITE_ATF \
	"wr_atf=filesize_check " __stringify(CONFIG_MAX_NAND_ATF_SIZE) \
	";if test ${filesize_result} = good; then mtk_image_blks " __stringify(CONFIG_MAX_NAND_BLOCK_SIZE) \
	";nand erase.spread " __stringify(CONFIG_NAND_ATF_OFFSET) "	${filesize} "\
	";mtk_image_blks " __stringify(CONFIG_MAX_NAND_PAGE_SIZE) \
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_ATF_OFFSET) " ${filesize};fi\0"
#define ENV_WRITE_PRELOADER \
	"wr_pl=filesize_check " __stringify(CONFIG_MAX_NAND_PL_SIZE) \
	";if test ${filesize_result} = good; then nand erase.spread " __stringify(CONFIG_NAND_HEADER_OFFSET) " 40000 "\
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_HEADER_OFFSET) " 40000;fi\0"
#define ENV_WRITE_ROM_HEADER \
	"wr_rom_hdr=filesize_check " __stringify(CONFIG_MAX_NAND_PL_SIZE) \
	";if test ${filesize_result} = good; then nand erase.spread " __stringify(CONFIG_NAND_HEADER_OFFSET) " 20000 "\
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_HEADER_OFFSET) " 20000;fi\0"
#define ENV_WRITE_CTP \
	"wr_ctp=filesize_check " __stringify(CONFIG_MAX_NAND_CTP_SIZE) \
	";if test ${filesize_result} = good; then nand erase.spread " __stringify(CONFIG_NAND_CTP_OFFSET) " 3000000 "\
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_CTP_OFFSET) " 3000000;fi\0"		
#define ENV_BOOT_READ_CTP \
	"boot_rd_ctp=nand read 0x40000000 " __stringify(CONFIG_NAND_CTP_OFFSET) " 3000000\0"

#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=decrypt_image;if test ${decrypt_result} = good; then filesize_check " __stringify(CONFIG_MAX_NAND_FLASHIMAGE_SIZE) \
	";if test ${filesize_result} = good; then nand erase.chip " \
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_FLASHIMAGE_OFFSET) " 8000000;fi;fi\0"

#if defined (CONFIG_CMD_NMRP)
#define ENV_BOOT_NMRP_WRITE_STR \
    "boot_nmrp_wr_str=filesize_check " __stringify(CONFIG_STR_BLK_SIZE) \
        ";if test ${filesize_result} = good; then str_blks ; if test ${str_blk_crc} = ok;then nand erase.spread  ${stringtableoffset}  ${filesize}" \
        ";nand write ${loadaddr}  ${stringtableoffset} ${filesize};fi;fi\0"
#define ENV_BOOT_NMRP_CLEAN_BLKS \
    "boot_nmrp_clr_str_blks=nand erase.spread  ${stringtableoffset}  ${stringtablefreesize} \0"
#define ENV_BOOT_NMRP_STR_OFFSET \
    "stringtableoffset=" __stringify(CONFIG_STR_OFFSET)  " \0"
#define ENV_BOOT_NMRP_STR_FREESIZE \
    "stringtablefreesize=0\0"

#define ENV_BOOT_NMRP_CMD \
	ENV_BOOT_NMRP_WRITE_STR \
	ENV_BOOT_NMRP_CLEAN_BLKS \
	ENV_BOOT_NMRP_STR_OFFSET \
	ENV_BOOT_NMRP_STR_FREESIZE
#else
#define ENV_BOOT_NMRP_CMD
#endif

#if defined (CONFIG_MTK_DUAL_IMAGE_SUPPORT)
#define BOOT_VERSION \
    "boot_version=20210401\0"
#define BACKUP_OS_NEED_CHECK \
    "backup_os_need_check=1\0"
#define ENV_BOOT_BACKUP_IMAGE \
    "boot_backup_img=run boot_check_os; if test ${check_os_error} = 0; then " \
        "nand read ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
        ";nand erase.spread "__stringify(CONFIG_MTK_BACKUP_IMAGE_OFFSET)  " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
        ";nand write ${loadaddr} " __stringify(CONFIG_MTK_BACKUP_IMAGE_OFFSET)  " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) " ;fi\0"
#define ENV_BOOT_RESTORE_IMAGE \
    "boot_restore_img=setenv backup_os_need_check 1; run  boot_check_backup_os; if test ${backup_os_need_check} = 0; then " \
        "nand read ${loadaddr} " __stringify(CONFIG_MTK_BACKUP_IMAGE_OFFSET) " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
        ";nand erase.spread "__stringify(CONFIG_NAND_LINUX_OFFSET)  " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
        ";nand write ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET)  " "__stringify(CONFIG_MAX_NAND_LINUX_SIZE) " ;reset;fi\0"
#define ENV_BOOT_CHECK_BACKUP_OS \
    "boot_check_backup_os=setenv skip_load_os 1;nand read ${loadaddr} " __stringify(CONFIG_MTK_BACKUP_IMAGE_OFFSET) " " __stringify(CONFIG_MAX_NAND_LINUX_SIZE)  \
        ";check_image;bootm; setenv skip_load_os 0;"\
        "if test ${check_os_error} = 1; then  echo Bad backup os !!;run boot_backup_img; else setenv backup_os_need_check 0;fi\0"

#define ENV_BOOT_CHECK_OS \
    "boot_check_os=setenv skip_load_os 1; run boot_rd_img;bootm;setenv skip_load_os 0;" \
        "if test ${check_os_error} = 1; then echo Bad os !!;fi\0"

#define ENV_BOOT_DUAL_IMAGE_CMD \
	BOOT_VERSION \
	BACKUP_OS_NEED_CHECK \
	ENV_BOOT_BACKUP_IMAGE \
	ENV_BOOT_RESTORE_IMAGE \
	ENV_BOOT_CHECK_BACKUP_OS \
	ENV_BOOT_CHECK_OS
#else
#define ENV_BOOT_DUAL_IMAGE_CMD
#endif

#elif defined(ON_BOARD_EMMC_COMPONENT)
/*EMMC Configuration*/
/* Parallel Nor Flash */
/* Build error elimination*/
#define CONFIG_SYS_NO_FLASH
/**/
#define CONFIG_CMD_MMC
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_SIZE                     SZ_4K
#define CONFIG_ENV_OFFSET                   0x140000
#define CONFIG_SYS_MMC_ENV_DEV              0
#define CONFIG_SYS_MAX_FLASH_SECT           512

#define CONFIG_EMMC_HEADER_BLOCK            0x0
#define CONFIG_MAX_HEADER_BLOCK_NUM         0x50
#define CONFIG_MAX_HEADER_SIZE              0xA000           // 0x50 * 512 = 40KB

#define CONFIG_EMMC_GPT_BLOCK                0x0
#define CONFIG_MAX_GPT_BLOCK_NUM             0x22
#define CONFIG_MAX_GPT_SIZE                  0x4400           // 0x22 * 512 = 17KB

#define CONFIG_EMMC_PL_BLOCK                0x0
#define CONFIG_MAX_PL_BLOCK_NUM             0x200
#define CONFIG_MAX_PL_SIZE                  0x40000           // 0x200 * 512 = 256KB

#define CONFIG_EMMC_ATF_BLOCK               0x400
#define CONFIG_MAX_ATF_BLOCK_NUM            0x200
#define CONFIG_MAX_ATF_SIZE                 0x40000           // 0x200 * 512 = 256KB

#define CONFIG_EMMC_UBOOT_BLOCK             0x600
#define CONFIG_MAX_UBOOT_BLOCK_NUM          0x400
#define CONFIG_MAX_UBOOT_SIZE               0x80000           // 0x400 * 512 = 512KB

#define CONFIG_EMMC_NVRAM_BLOCK		    0xA00
#define CONFIG_MAX_NVRAM_BLOCK_NUM          0x400
#define CONFIG_MAX_NVRAM_SIZE               0x80000           // 0x400 * 512 = 512KB

#define CONFIG_EMMC_RF_BLOCK                0xE00
#define CONFIG_MAX_RF_BLOCK_NUM             0x200
#define CONFIG_MAX_RF_SIZE                  0x40000           // 0x200 * 512 = 256KB

#define CONFIG_EMMC_LINUX_BLOCK             0x1000
#define CONFIG_MAX_LINUX_BLOCK_NUM          0x18000
#define CONFIG_MAX_LINUX_SIZE               0x1E00000         //0x18000*512 =30M

#define CONFIG_EMMC_CTP_BLOCK               0x1000
#define CONFIG_MAX_CTP_BLOCK_NUM            0xa000
#define CONFIG_MAX_CTP_SIZE                 0x1400000         //0xa000*512 =20M

#define CONFIG_EMMC_FLASHIMAGE_BLOCK               0x0
#define CONFIG_MAX_FLASHIMAGE_BLOCK_NUM            0xa000
#define CONFIG_MAX_FLASHIMAGE_SIZE                 0x1400000         //0xa000*512 =20M

#define ENV_BOOT_WRITE_IMAGE \
    "boot_wr_img=filesize_check " __stringify(CONFIG_MAX_LINUX_SIZE) \
	";if test ${filesize_result} = good; then image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    "  ${filesize};mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " " __stringify(CONFIG_MAX_LINUX_BLOCK_NUM) ";fi\0"
#define ENV_BOOT_READ_IMAGE \
    "boot_rd_img=mmc device 0;mmc read ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " 1" \
    ";image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    ";mmc read ${loadaddr} " __stringify(CONFIG_EMMC_LINUX_BLOCK) " ${img_blks}\0"
#define ENV_WRITE_UBOOT \
    "wr_uboot=filesize_check " __stringify(CONFIG_MAX_UBOOT_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_UBOOT_BLOCK) \
    " " __stringify(CONFIG_MAX_UBOOT_BLOCK_NUM) ";fi\0"
#define ENV_WRITE_ATF \
	"wr_atf=filesize_check " __stringify(CONFIG_MAX_ATF_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_ATF_BLOCK) \
	" " __stringify(CONFIG_MAX_ATF_BLOCK_NUM) ";fi\0"
#define ENV_WRITE_PRELOADER \
	"wr_pl=filesize_check " __stringify(CONFIG_MAX_PL_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0; mmc write ${loadaddr} " __stringify(CONFIG_EMMC_PL_BLOCK) \
	" " __stringify(CONFIG_MAX_PL_BLOCK_NUM) " emmc_preloader;fi\0"
#define ENV_WRITE_ROM_HEADER \
	"wr_rom_hdr=filesize_check " __stringify(CONFIG_MAX_HEADER_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_HEADER_BLOCK) \
	" " __stringify(CONFIG_MAX_HEADER_BLOCK_NUM) ";fi\0"
#define ENV_WRITE_CTP \
    "wr_ctp=filesize_check " __stringify(CONFIG_MAX_CTP_SIZE) \
	";if test ${filesize_result} = good; then ctp_check;if test ${ctp_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_CTP_BLOCK) \
    " " __stringify(CONFIG_MAX_CTP_BLOCK_NUM) ";reset; fi;fi\0"

#define ENV_BOOT_READ_CTP \
    "boot_rd_ctp=mmc device 0;mmc read ${loadaddr} " __stringify(CONFIG_EMMC_CTP_BLOCK) \
    " " __stringify(CONFIG_MAX_CTP_BLOCK_NUM) "\0"

#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=filesize_check " __stringify(CONFIG_MAX_FLASHIMAGE_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_FLASHIMAGE_BLOCK) \
		" " __stringify(CONFIG_MAX_FLASHIMAGE_BLOCK_NUM) ";fi\0"

#define ENV_WRITE_GPT \
	"wr_gpt=filesize_check " __stringify(CONFIG_MAX_GPT_SIZE) \
	";if test ${filesize_result} = good; then mmc device 0;mmc write ${loadaddr} " __stringify(CONFIG_EMMC_GPT_BLOCK) \
		" " __stringify(CONFIG_MAX_GPT_BLOCK_NUM) ";fi\0"


#elif defined(OFF_BOARD_SD_CARD_COMPONENT)
/*SD card configuration*/
/* Build error elimination*/
#define CONFIG_CMD_MMC
#define CONFIG_SYS_NO_FLASH
/**/
#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_ENV_IS_IN_SD
#define CONFIG_ENV_SIZE                     SZ_4K
#define CONFIG_ENV_OFFSET                   0x140000
#define CONFIG_SYS_MMC_ENV_DEV              1

#define ENV_BOOT_FROM_SD  \
    "boot5=mmc init; fatload mmc 0:1 0x84000000 uimage ; bootm\0"

#define CONFIG_SYS_MAX_FLASH_SECT           512

#define CONFIG_SDCARD_HEADER_BLOCK            0x0
#define CONFIG_MAX_HEADER_SIZE              0x50           // 0x50 * 512 = 40KB

#define CONFIG_SDCARD_GPT_BLOCK		    0x0
#define CONFIG_MAX_GPT_SIZE                 0x22           // 0x22 * 512 = 17KB

#define CONFIG_SDCARD_PL_BLOCK		    0x100
#define CONFIG_MAX_PL_SIZE                  0x200           // 0x200 * 512 = 256KB

#define CONFIG_SDCARD_ATF_BLOCK             0x400
#define CONFIG_MAX_ATF_SIZE                 0x200           // 0x200 * 512 = 256KB

#define CONFIG_SDCARD_UBOOT_BLOCK           0x600
#define CONFIG_MAX_UBOOT_SIZE               0x400           // 0x400 * 512 = 512KB

#define CONFIG_SDCARD_NVRAM_BLOCK           0xA00
#define CONFIG_MAX_NVRAM_SIZE               0x400           // 0x400 * 512 = 512KB

#define CONFIG_SDCARD_RF_BLOCK              0xE00
#define CONFIG_MAX_RF_SIZE                  0x200           // 0x200 * 512 = 256KB

#define CONFIG_SDCARD_LINUX_BLOCK           0x1000
#define CONFIG_MAX_LINUX_SIZE               0xa000         //0xa000*512 =20M

#define CONFIG_SDCARD_CTP_BLOCK             0x1000
#define CONFIG_MAX_CTP_SIZE                 0xa000         //0xa000*512 =20M

#define CONFIG_SDCARD_FLASHIMAGE_BLOCK      0x0
#define CONFIG_MAX_FLASHIMAGE_SIZE          0xa000         //0xa000*512 =20M


#define ENV_BOOT_WRITE_IMAGE \
    "boot_wr_img=image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    "  ${filesize};mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_LINUX_BLOCK) " ${img_blks}\0"
#define ENV_BOOT_READ_IMAGE \
    "boot_rd_img=mmc device 1;mmc read ${loadaddr} " __stringify(CONFIG_SDCARD_LINUX_BLOCK) " 1" \
    ";image_blks " __stringify(CONFIG_SYS_MAX_FLASH_SECT) \
    ";mmc read ${loadaddr} " __stringify(CONFIG_SDCARD_LINUX_BLOCK) " ${img_blks}\0"
#define ENV_WRITE_UBOOT \
    "wr_uboot=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_UBOOT_BLOCK) \
    " " __stringify(CONFIG_MAX_UBOOT_SIZE) "\0"
#define ENV_WRITE_ATF \
	"wr_atf=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_ATF_BLOCK) \
	" " __stringify(CONFIG_MAX_ATF_SIZE) "\0"
#define ENV_WRITE_PRELOADER \
	"wr_pl=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_PL_BLOCK) \
	" " __stringify(CONFIG_MAX_PL_SIZE) "\0"
#define ENV_WRITE_ROM_HEADER \
	"wr_rom_hdr=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_HEADER_BLOCK) \
	" " __stringify(CONFIG_MAX_HEADER_SIZE) "\0"
#define ENV_WRITE_CTP \
	"wr_ctp=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_CTP_BLOCK) \
	" " __stringify(CONFIG_MAX_CTP_SIZE) "\0"
#define ENV_BOOT_READ_CTP \
	"boot_rd_ctp=mmc device 1;mmc read ${loadaddr} " __stringify(CONFIG_SDCARD_CTP_BLOCK) \
	" " __stringify(CONFIG_MAX_CTP_SIZE) "\0"
#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_FLASHIMAGE_BLOCK) \
		" " __stringify(CONFIG_MAX_FLASHIMAGE_SIZE) "\0"
#define ENV_WRITE_GPT \
	"wr_gpt=mmc device 1;mmc write ${loadaddr} " __stringify(CONFIG_SDCARD_GPT_BLOCK) \
		" " __stringify(CONFIG_MAX_GPT_SIZE) "\0"


#else
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE                     SZ_16K

#define ENV_BOOT_WRITE_IMAGE "boot_wr_img=none\0"
#define ENV_BOOT_READ_IMAGE "boot_rd_img=none\0"
#define ENV_WRITE_UBOOT "wr_uboot=none\0"
#define ENV_WRITE_ATF
#define ENV_WRITE_PRELOADER
#define ENV_WRITE_ROM_HEADER
#define ENV_WRITE_CTP
#define ENV_BOOT_READ_CTP

#endif

#define CONFIG_ENV_VARS_UBOOT_CONFIG

#if defined(ON_BOARD_EMMC_COMPONENT) || defined(OFF_BOARD_SD_CARD_COMPONENT)
/********************** MMC ***********************************/
#define PART_DEBUG
#define CONFIG_MMC
#define CONFIG_FS_FAT
#define CONFIG_CMD_FAT
#define CONFIG_MEDIATEK_MMC

#define CONFIG_DOS_PARTITION
#endif

/********************** Watchdog *************************/
#define CFG_HW_WATCHDOG 1
#define CONFIG_WATCHDOG_OFF


/********************** Console UART *************************/
/* Uart baudrate */
#define CONFIG_BAUDRATE                     115200
 
/* Console configuration */
#define CONFIG_SYS_CBSIZE		            1024
#define CONFIG_SYS_PBSIZE		            (CONFIG_SYS_CBSIZE +		\
					                            sizeof(CONFIG_SYS_PROMPT) + 16)
//#define CONFIG_CONSOLE_MUX
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define ENV_DEVICE_SETTINGS \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"stderr=serial\0"

/********************** Ethernet *************************/
#define CONFIG_ETHADDR                      00:1E:E3:34:56:78
#define CONFIG_IPADDR                       192.168.1.1
#define CONFIG_SERVERIP                     192.168.1.100
#define CONFIG_BOOTFILE                     "iverson_uImage"
#define CONFIG_CMD_NET
#define CONFIG_RT2880_ETH

#define RALINK_REG(x)		(*((volatile u32 *)(x)))


/**********************************************************************************************
 *                                       Boot Menu
 **********************************************************************************************/
#if 1
#if defined(MT7622_FPGA_BOARD)
#define CONFIG_BOOTDELAY                    5
#else
#define CONFIG_BOOTDELAY                    3
#endif
#define CONFIG_BOOTCOMMAND                  "No"
#define CONFIG_CMD_BOOTMENU
#define CONFIG_MENU
#define CONFIG_MENU_SHOW
#define CONFIG_MENU_ACTIVE_ENTRY            2

#define ENV_BOOT_CMD0 \
    "boot0=download_setting kernel;tftpboot ${loadaddr} ${kernel_filename}; bootm\0"

#define ENV_BOOT_CMD1 \
    "boot1=download_setting kernel;tftpboot ${loadaddr} ${kernel_filename};run boot_wr_img;run boot_rd_img;bootm\0"

#if defined (CONFIG_MTK_DUAL_IMAGE_SUPPORT)
#define ENV_BOOT_CMD2 \
    "boot2=run boot_check_backup_os;run boot_rd_img;bootm\0"
#else
#define ENV_BOOT_CMD2 \
    "boot2=run boot_rd_img;bootm\0"
#endif

#define ENV_BOOT_CMD3 \
    "boot3=download_setting uboot;tftpboot ${loadaddr} ${uboot_filename};run wr_uboot;invaild_env\0"

#define ENV_BOOT_CMD4 \
    "boot4=loadb;run wr_uboot;invaild_env\0"

#define ENV_BOOT_CMD5 \
    "boot5=download_setting atf;tftpboot ${loadaddr} ${atf_filename};run wr_atf\0"

#define ENV_BOOT_CMD6 \
    "boot6=download_setting preloader;tftpboot ${loadaddr} ${preloader_filename};run wr_pl\0"

#define ENV_BOOT_CMD7 \
    "boot7=download_setting hdr;tftpboot ${loadaddr} ${hdr_filename};run wr_rom_hdr\0"

#define ENV_BOOT_CMD8 \
    "boot8=download_setting ctp;tftpboot ${loadaddr} ${ctp_filename};run wr_ctp\0"

#define ENV_BOOT_CMD9 \
    "boot9=run boot_rd_ctp;boot_to_ctp\0"

#define ENV_BOOT_CMD10 \
    "boot10=if " \
    "dialog \"WARNING, this operation will flash all partitions (preloader + atf + uboot + linux)\";" \
    "then " \
    "download_setting flashimage;tftpboot ${loadaddr} ${flashimage_filename};run wr_flashimage;invaild_env;" \
    "else " \
    "echo \"operation aborted by user\";" \
    "fi;\0"

#if defined(ON_BOARD_EMMC_COMPONENT) || defined(OFF_BOARD_SD_CARD_COMPONENT)

#define ENV_BOOT_CMD11 \
    "boot11=download_setting gpt;tftpboot ${loadaddr} ${gpt_filename};run wr_gpt\0"

#define ENV_BOOT_CMD \
    ENV_BOOT_WRITE_IMAGE \
    ENV_BOOT_READ_IMAGE \
    ENV_WRITE_UBOOT \
    ENV_WRITE_ATF \
    ENV_WRITE_PRELOADER \
    ENV_WRITE_ROM_HEADER \
    ENV_WRITE_CTP \
    ENV_BOOT_READ_CTP \
    ENV_WRITE_FLASHIMAGE \
    ENV_WRITE_GPT \
    ENV_BOOT_CMD0 \
    ENV_BOOT_CMD1 \
    ENV_BOOT_CMD2 \
    ENV_BOOT_CMD3 \
    ENV_BOOT_CMD4 \
    ENV_BOOT_CMD5 \
    ENV_BOOT_CMD6 \
    ENV_BOOT_CMD7 \
    ENV_BOOT_CMD8 \
    ENV_BOOT_CMD9 \
    ENV_BOOT_CMD10 \
	ENV_BOOT_CMD11


#define ENV_BOOT_MENU \
    "bootmenu_0=1. System Load Linux to SDRAM via TFTP.=run boot0\0" \
    "bootmenu_1=2. System Load Linux Kernel then write to Flash via TFTP.=run boot1\0" \
    "bootmenu_2=3. Boot system code via Flash.=run boot2\0" \
    "bootmenu_3=4. System Load U-Boot then write to Flash via TFTP.=run boot3\0" \
    "bootmenu_4=5. System Load U-Boot then write to Flash via Serial.=run boot4\0" \
    "bootmenu_5=6. System Load ATF then write to Flash via TFTP.=run boot5\0" \
    "bootmenu_6=7. System Load Preloader then write to Flash via TFTP.=run boot6\0" \
    "bootmenu_7=8. System Load ROM header then write to Flash via TFTP.=run boot7\0" \
    "bootmenu_8=9. System Load CTP then write to Flash via TFTP.=run boot8\0" \
	"bootmenu_9=a. System Load CTP then Boot to CTP (via Flash).=run boot9\0" \
	"bootmenu_10=b. System Load SingleImage then write to Flash via TFTP.=run boot10\0" \
	"bootmenu_11=c. System Load partition table then write to Flash via TFTP.=run boot11\0" \
    "bootmenu_delay=30\0" \
    ""
#else

#define ENV_BOOT_CMD \
    ENV_BOOT_WRITE_IMAGE \
    ENV_BOOT_READ_IMAGE \
    ENV_WRITE_UBOOT \
    ENV_WRITE_ATF \
    ENV_WRITE_PRELOADER \
    ENV_WRITE_ROM_HEADER \
    ENV_WRITE_CTP \
    ENV_BOOT_READ_CTP \
    ENV_WRITE_FLASHIMAGE \
    ENV_BOOT_CMD0 \
    ENV_BOOT_CMD1 \
    ENV_BOOT_CMD2 \
    ENV_BOOT_CMD3 \
    ENV_BOOT_CMD4 \
    ENV_BOOT_CMD5 \
    ENV_BOOT_CMD6 \
    ENV_BOOT_CMD7 \
    ENV_BOOT_CMD8 \
    ENV_BOOT_CMD9 \
    ENV_BOOT_CMD10


#define ENV_BOOT_MENU \
    "bootmenu_0=1. System Load Linux to SDRAM via TFTP.=run boot0\0" \
    "bootmenu_1=2. System Load Linux Kernel then write to Flash via TFTP.=run boot1\0" \
    "bootmenu_2=3. Boot system code via Flash.=run boot2\0" \
    "bootmenu_3=4. System Load U-Boot then write to Flash via TFTP.=run boot3\0" \
    "bootmenu_4=5. System Load U-Boot then write to Flash via Serial.=run boot4\0" \
    "bootmenu_5=6. System Load ATF then write to Flash via TFTP.=run boot5\0" \
    "bootmenu_6=7. System Load Preloader then write to Flash via TFTP.=run boot6\0" \
    "bootmenu_7=8. System Load ROM header then write to Flash via TFTP.=run boot7\0" \
    "bootmenu_8=9. System Load CTP then write to Flash via TFTP.=run boot8\0" \
	"bootmenu_9=a. System Load CTP then Boot to CTP (via Flash).=run boot9\0" \
	"bootmenu_10=b. System Load SingleImage then write to Flash via TFTP.=run boot10\0" \
    "bootmenu_delay=30\0" \
    ""

#endif // ON_BOARD_EMMC_COMPONENT

#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0x6c000000\0" \
    "kernel_filename=iverson_uImage\0" \
    "uboot_filename=u-boot-mtk.bin\0" \
    "atf_filename=trustzone.bin\0" \
    "preloader_filename=preloader_fpga7622_64_ldvt.bin\0" \
    "hdr_filename=hdr.binary\0" \
    "ctp_filename=ctp.bin\0" \
    "flashimage_filename=flashimage.bin\0" \
    "gpt_filename=GPT_EMMC\0" \
    "invaild_env=no\0" \
	ENV_DEVICE_SETTINGS \
	ENV_BOOT_CMD \
	ENV_BOOT_DUAL_IMAGE_CMD \
	ENV_BOOT_NMRP_CMD \
	ENV_BOOT_MENU
#else
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0x6c000000\0" \
	ENV_DEVICE_SETTINGS

#endif

/**********************************************************************************************
 *                                      FDT
 **********************************************************************************************/
#define CONFIG_FIT
#define CONFIG_OF_LIBFDT
#define CONFIG_FDT_DEBUG

/**********************************************************************************************
 *                                      ATF
 **********************************************************************************************/
#define CONFIG_MTK_ATF

/**********************************************************************************************
 *                                       UBoot Command
 **********************************************************************************************/
/* Shell */
#define CONFIG_SYS_MAXARGS		            8
#define CONFIG_SYS_PROMPT		            "MT7622> "
#define CONFIG_COMMAND_HISTORY

/* Commands */
#include <config_cmd_default.h>

/* Device tree support */
#define CONFIG_OF_BOARD_SETUP
/* ATAGs support for bootm/bootz */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_NFS

#define CONFIG_SYS_LONGHELP
#define CONFIG_CMD_PING

#define CONFIG_SYS_HUSH_PARSER


/**********************************************************************************************
 *                                      Compression
 **********************************************************************************************/
/* 
 * Iverson 20150510 :  
 *      Denali-2 use lk to decompress kernel.
 */
#define CONFIG_LZMA                             
#define CONFIG_MTGPIO

#define CONFIG_AES
#define CONFIG_ENV_OVERWRITE

#endif
