/*
 * (C) Copyright 2018 MediaTek.Inc
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

#ifndef __MT7629_H
#define __MT7629_H

#include <linux/sizes.h>
#include <configs/autoconf.h>


/**********************************************************************************************
 *                                      ARM Cortex A7
 **********************************************************************************************/
#define CONFIG_MEDIATEK

/* Machine ID */
#define CONFIG_MACH_TYPE		7629

/* Serial drivers */
#define CONFIG_BAUDRATE 		115200

/*
 * SPL related defines
 */
//#define CONFIG_SPL
#define CONFIG_SPL_FRAMEWORK
#define CONFIG_SPL_DECOMPRESS_UBOOT
#define CONFIG_SPL_BOARD_INIT
#define CONFIG_SPL_LZMA
/*#define CONFIG_SPL_RAM_DEVICE*/

#define CONFIG_SPL_LIBCOMMON_SUPPORT	/* image.c */
#define CONFIG_SPL_LIBGENERIC_SUPPORT	/* string.c */
#define CONFIG_SPL_SERIAL_SUPPORT

#define CONFIG_SPL_TEXT_BASE		0x201000
#define CONFIG_SPL_MAX_SIZE 		0x20000     /* Max 128KB */
#define CONFIG_SPL_MAX_FOOTPRINT	0x10000

/* Just define any reasonable size */
#define CONFIG_SPL_STACK_SIZE		0x1000
/* SPL stack position - and stack goes down */
#define CONFIG_SPL_STACK 			0x101000

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

/* Uboot heap size for malloc() */
#define CONFIG_SYS_MALLOC_LEN               SZ_4M

#define CONFIG_SYS_TEXT_BASE                0x41E00000
#define CONFIG_SYS_SPL_ARGS_ADDR            0x40000000

#define CONFIG_SYS_UBOOT_START              CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_UBOOT_MAX_SIZE           SZ_2M
#define CONFIG_SYS_INIT_SP_ADDR             (CONFIG_SYS_TEXT_BASE + \
                                                CONFIG_SYS_UBOOT_MAX_SIZE - \
                                                GENERATED_GBL_DATA_SIZE)

/* RichOS memory partitions, load-addr is very important ! */
#define CONFIG_SYS_LOAD_ADDR                0x42007F1C
#define CONFIG_SYS_IMAGE_HDR_ADDR           CONFIG_SYS_LOAD_ADDR

/* Linux DRAM definition */
#define CONFIG_LOADADDR                     CONFIG_SYS_LOAD_ADDR

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 64 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_SYS_BOOTM_LEN	            0x4000000


/*
 * For different DDR Sample Type Select
 */
#define DRAM_PIN_MUX_TYPE_DDR3X16_BGA       0
#define DRAM_PIN_MUX_TYPE_DDR2X16_BGA       1
#define DRAM_PIN_MUX_TYPE_DDR3X8X2          4
#define CONFIG_DRAM_TYPE                    DRAM_PIN_MUX_TYPE_DDR3X16_BGA


/**********************************************************************************************
 *                                           Board
 **********************************************************************************************/

/* Board */
#if defined(MT7629_FPGA_BOARD)
#define CFG_FPGA_PLATFORM   1
#endif
#define CONFIG_ADD_MTK_HEADER               "n"

/* Not to access NOR NVRAM partition for bringup only, we can remove it late */
/*#define CONFIG_DELAY_ENVIRONMENT*/
#define CONFIG_BOARD_LATE_INIT


/**********************************************************************************************
 *                                          Devices
 **********************************************************************************************/

/********************** Flash *************************/
#if defined(ON_BOARD_SPI_NOR_FLASH_COMPONENT)
/* SPI Flash Configuration */

#define CONFIG_SPL_PAD_ALIGNMENT	0x4
#define CONFIG_MTK_BROM_HEADER_INFO	"media=nor"

/* Parallel Nor Flash */
#define CONFIG_SYS_NO_FLASH

/* SPI NOR Flash */
#define CONFIG_CMD_NOR
#define CONFIG_SPL_NOR_SUPPORT

#define CONFIG_ENV_IS_IN_NOR

#define CONFIG_ENV_SIZE                    SZ_4K
#define CONFIG_ENV_SECT_SIZE               0x1000   /* 4KB */
#define CONFIG_ENV_OFFSET                  CONFIG_NOR_NVRAM_OFFSET

#define CONFIG_NOR_BOOTLOADER_OFFSET       0x00000  //boot-loader : u-boot-mtk.bin offset
#define CONFIG_NOR_BOOTLOADER_SIZE         0x60000  //boot-loader max size : SPL 64K + u-boot.img 320K

#define CONFIG_NOR_UBOOT_OFFSET            0x10000  //u-boot.img offset
#define CONFIG_NOR_UBOOT_SIZE              0x50000  //Uboot max size : u-boot.img 320K

#define CONFIG_NOR_NVRAM_OFFSET            0x60000  //NVRAM offset
#define CONFIG_NOR_NVRAM_SIZE              0x10000  //NVRAM max size 64K

#define CONFIG_NOR_RF_OFFSET               0x70000  //RF offset
#define CONFIG_NOR_RF_SIZE                 0x40000  //RF max size 256K

#define CONFIG_NOR_LINUX_OFFSET            0xB0000  //Linux begin offset
#define CONFIG_NOR_LINUX_SIZE              0xF20000 //Linux max size 15M

#define CONFIG_NOR_CTP_OFFSET              0xB0000  //CTP begin offset
#define CONFIG_NOR_CTP_SIZE                0xF20000 //CTP max size 15M

#define CONFIG_NOR_FLASHIMAGE_OFFSET       0x0      //flashimage.bin begin offset
#define CONFIG_NOR_FLASHIMAGE_SIZE         0x1000000//flashimage.bin max size 16M

#define CONFIG_NOR_BASE                    0x30000000	/* start of FLASH */
#define CONFIG_SYS_UBOOT_BASE              (CONFIG_NOR_BASE + CONFIG_NOR_UBOOT_OFFSET) /* u-boot.img image */
#define CONFIG_SYS_OS_BASE                 (CONFIG_NOR_BASE + CONFIG_NOR_LINUX_OFFSET) /* linux image */
#define CONFIG_SYS_FDT_BASE                (CONFIG_NOR_BASE) /* Not used, only for NOR build */


#define ENV_BOOT_WRITE_IMAGE \
	"boot_wr_img=filesize_check " __stringify(CONFIG_NOR_LINUX_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_LINUX_OFFSET) " ${file_align_size}" \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " ${filesize};fi\0"
#define ENV_BOOT_READ_IMAGE \
	"boot_rd_img=snor read ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " 2000" \
	";image_blks 1" \
	";snor read ${loadaddr} " __stringify(CONFIG_NOR_LINUX_OFFSET) " ${img_align_size}\0"
#define ENV_WRITE_UBOOT \
	"wr_uboot=filesize_check " __stringify(CONFIG_NOR_BOOTLOADER_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_BOOTLOADER_OFFSET) " " __stringify(CONFIG_NOR_BOOTLOADER_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_BOOTLOADER_OFFSET) " " __stringify(CONFIG_NOR_BOOTLOADER_SIZE) ";fi\0"
#define ENV_WRITE_CTP \
	"wr_ctp=filesize_check " __stringify(CONFIG_NOR_CTP_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_CTP_OFFSET) " " __stringify(CONFIG_NOR_CTP_OFFSET) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_CTP_OFFSET) " " __stringify(CONFIG_NOR_CTP_SIZE) ";fi\0"
#define ENV_BOOT_READ_CTP \
	"boot_rd_ctp=snor read 0x40000000 " __stringify(CONFIG_NOR_CTP_OFFSET) " 0xF20000\0"
#define ENV_WRITE_CUSTOM_IMAGE \
	"wr_cumstom_image=custom_image_check " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";if test ${img_result} = good; then snor erase " __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";snor write 0x40000000 "  __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) ";fi\0"
#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=filesize_check " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";if test ${filesize_result} = good; then snor erase " __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " " __stringify(CONFIG_NOR_FLASHIMAGE_SIZE) \
	";snor write ${loadaddr} " __stringify(CONFIG_NOR_FLASHIMAGE_OFFSET) " ${filesize};fi\0"


#elif defined(ON_BOARD_NAND_FLASH_COMPONENT) || \
      defined(ON_BOARD_SPI_NAND_FLASH_COMPONENT)

#define CONFIG_SPL_PAD_ALIGNMENT	0x1000

#if defined(ON_BOARD_2K_64_SNAND_COMPONENT)
#define CONFIG_MTK_BROM_HEADER_INFO	"media=snand;nandinfo=2k+64"
#elif defined(ON_BOARD_2K_128_SNAND_COMPONENT)
#define CONFIG_MTK_BROM_HEADER_INFO	"media=snand;nandinfo=2k+128"
#elif defined(ON_BOARD_4K_256_SNAND_COMPONENT)
#define CONFIG_MTK_BROM_HEADER_INFO	"media=snand;nandinfo=4k+256"
#endif

#define CONFIG_SYS_NO_FLASH

/* SPI NAND Flash */
#define CONFIG_CMD_NAND
#define CONFIG_SPL_NAND_SUPPORT
#define CONFIG_SPL_DRIVERS_MISC_SUPPORT

/* NAND Flash Configuration */
#define CONFIG_SYS_MAX_NAND_DEVICE          1
#define CONFIG_SYS_NAND_BASE                0x1100D000
#define CONFIG_SYS_MAX_FLASH_SECT           256
#define CONFIG_SYS_MAX_FLASH_BANKS          1

#define CONFIG_MAX_NAND_PAGE_SIZE           2048
#define CONFIG_MAX_NAND_BLOCK_SIZE          131072

#define CONFIG_CMD_MTDPARTS                 1
#define CONFIG_MTD_PARTITIONS               1
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_MTD_NAND_VERIFY_WRITE        1

#define CONFIG_ENV_SIZE                     SZ_4K
#define CONFIG_ENV_OFFSET                   CONFIG_NAND_NVRAM_OFFSET

#define CONFIG_NAND_BOOTLOADER_OFFSET       0x00000
#define CONFIG_MAX_BOOTLOADER_SIZE          0x100000 //Uboot max size : SPL 128K + u-boot.img 384K + 512K(backup)

#define CONFIG_NAND_NVRAM_OFFSET            0x100000 //NVRAM offset
#define CONFIG_NAND_NVRAM_SIZE              0x40000  //NVRAM max size : 128K + 128K(backup)

#define CONFIG_NAND_RF_OFFSET               0x140000
#define CONFIG_MAX_NAND_RF_SIZE             0x80000  //RF max size : 256K + 256K(backup)

#define CONFIG_NAND_LINUX_OFFSET            0x1C0000
#define CONFIG_MAX_NAND_LINUX_SIZE          0x2000000

#define CONFIG_NAND_CTP_OFFSET              0x1C0000
#define CONFIG_MAX_NAND_CTP_SIZE            0xF20000 //CTP max size 15M

#define CONFIG_NAND_FLASHIMAGE_OFFSET       0x0
#define CONFIG_MAX_NAND_FLASHIMAGE_SIZE     0x8000000

#define CONFIG_SYS_NAND_U_BOOT_OFFS         0x20000
#define CONFIG_SYS_NAND_PAGE_SIZE           2048

#define ENV_BOOT_WRITE_IMAGE \
	"boot_wr_img=filesize_check " __stringify(CONFIG_MAX_NAND_LINUX_SIZE) \
	";if test ${filesize_result} = good; then image_blks " __stringify(CONFIG_MAX_NAND_BLOCK_SIZE) \
	";nand erase.spread " __stringify(CONFIG_NAND_LINUX_OFFSET) "  ${filesize}" \
	";image_blks " __stringify(CONFIG_MAX_NAND_PAGE_SIZE) \
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " ${filesize};fi\0"
#define ENV_BOOT_READ_IMAGE \
	"boot_rd_img=nand read ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " 0x2000" \
	";image_blks " __stringify(CONFIG_MAX_NAND_PAGE_SIZE) \
	";nand read ${loadaddr} " __stringify(CONFIG_NAND_LINUX_OFFSET) " ${img_align_size}\0"
#define ENV_WRITE_UBOOT \
	"wr_uboot=filesize_check " __stringify(CONFIG_MAX_BOOTLOADER_SIZE) \
	";if test ${filesize_result} = good; then mtk_image_blks " __stringify(CONFIG_MAX_NAND_BLOCK_SIZE) \
	";nand erase.spread " __stringify(CONFIG_NAND_BOOTLOADER_OFFSET) "  ${filesize} "\
	";mtk_image_blks " __stringify(CONFIG_MAX_NAND_PAGE_SIZE) \
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_BOOTLOADER_OFFSET) " ${filesize};fi\0"
#define ENV_WRITE_CTP \
	"wr_ctp=filesize_check " __stringify(CONFIG_MAX_NAND_CTP_SIZE) \
	";if test ${filesize_result} = good; then nand erase.spread " __stringify(CONFIG_NAND_CTP_OFFSET) " 0xF20000 "\
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_CTP_OFFSET) " " __stringify(CONFIG_MAX_NAND_CTP_SIZE) ";fi\0"
#define ENV_BOOT_READ_CTP \
	"boot_rd_ctp=nand read 0x40000000 " __stringify(CONFIG_NAND_CTP_OFFSET) " 0xF20000\0"
#define ENV_WRITE_CUSTOM_IMAGE \
	"wr_cumstom_image=custom_image_check " __stringify(CONFIG_MAX_NAND_FLASHIMAGE_SIZE) \
	";if test ${img_result} = good; then nand erase.chip " \
	";nand write 0x40000000 " __stringify(CONFIG_NAND_FLASHIMAGE_OFFSET) " 0x1800000;fi\0"
#define ENV_WRITE_FLASHIMAGE \
	"wr_flashimage=filesize_check " __stringify(CONFIG_MAX_NAND_FLASHIMAGE_SIZE) \
	";if test ${filesize_result} = good; then nand erase.chip " \
	";nand write ${loadaddr} " __stringify(CONFIG_NAND_FLASHIMAGE_OFFSET) " ${filesize};fi\0"

#else

#error "No any flash component error!!!"

#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE                     SZ_4K

#define ENV_BOOT_WRITE_IMAGE  "boot_wr_img=none\0"
#define ENV_BOOT_READ_IMAGE   "boot_rd_img=none\0"
#define ENV_WRITE_UBOOT       "wr_uboot=none\0"
#define ENV_WRITE_CTP
#define ENV_BOOT_READ_CTP

#endif

#define CONFIG_ENV_VARS_UBOOT_CONFIG

/* Watchdog */
#define CFG_HW_WATCHDOG 1
#define CONFIG_WATCHDOG_OFF

/* Console configuration */
#define CONFIG_SYS_CBSIZE		            1024
#define CONFIG_SYS_PBSIZE		            (CONFIG_SYS_CBSIZE +		\
					                            sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

#define ENV_DEVICE_SETTINGS \
	"stdin=serial\0" \
	"stdout=serial\0" \
	"stderr=serial\0"

/********************** Ethernet *************************/
#define CONFIG_ETHADDR                      00:0C:E7:11:22:33
#define CONFIG_IPADDR                       192.168.1.1
#define CONFIG_SERVERIP                     192.168.1.3
#define CONFIG_BOOTFILE                     "lede_uImage"
#define CONFIG_CMD_NET
#define CONFIG_RT2880_ETH

#define RALINK_REG(x)		(*((volatile u32 *)(x)))


/**********************************************************************************************
 *                                       Boot Menu
 **********************************************************************************************/
#if defined(MT7629_FPGA_BOARD)
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
    "boot0=download_setting kernel;tftpboot ${loadaddr} ${kernel_filename};bootm\0"

#define ENV_BOOT_CMD1 \
    "boot1=download_setting kernel;tftpboot ${loadaddr} ${kernel_filename};run boot_wr_img;run boot_rd_img;bootm\0"

#define ENV_BOOT_CMD2 \
    "boot2=run boot_rd_img;bootm\0"

#define ENV_BOOT_CMD3 \
    "boot3=download_setting uboot;tftpboot ${loadaddr} ${uboot_filename};run wr_uboot;invaild_env\0"

#define ENV_BOOT_CMD4 \
    "boot4=loadb;run wr_uboot;invaild_env\0"

#define ENV_BOOT_CMD5 \
    "boot5=download_setting ctp;tftpboot ${loadaddr} ${ctp_filename};run wr_ctp\0"

#define ENV_BOOT_CMD6 \
    "boot6=run wr_cumstom_image;invaild_env\0"

#define ENV_BOOT_CMD7 \
    "boot7=download_setting flashimage;tftpboot ${loadaddr} ${flashimage_filename};run wr_flashimage;invaild_env\0"


#define ENV_BOOT_CMD \
    ENV_BOOT_WRITE_IMAGE \
    ENV_BOOT_READ_IMAGE \
    ENV_WRITE_UBOOT \
    ENV_WRITE_CTP \
    ENV_BOOT_READ_CTP \
    ENV_WRITE_CUSTOM_IMAGE \
    ENV_WRITE_FLASHIMAGE \
    ENV_BOOT_CMD0 \
    ENV_BOOT_CMD1 \
    ENV_BOOT_CMD2 \
    ENV_BOOT_CMD3 \
    ENV_BOOT_CMD4 \
    ENV_BOOT_CMD5 \
    ENV_BOOT_CMD6 \
    ENV_BOOT_CMD7

#define ENV_BOOT_MENU \
    "bootmenu_0=1. System Load Linux to SDRAM via TFTP.=run boot0\0" \
    "bootmenu_1=2. System Load Linux Kernel then write to Flash via TFTP.=run boot1\0" \
    "bootmenu_2=3. Boot system code via Flash.=run boot2\0" \
    "bootmenu_3=4. System Load U-Boot then write to Flash via TFTP.=run boot3\0" \
    "bootmenu_4=5. System Load U-Boot then write to Flash via Serial.=run boot4\0" \
    "bootmenu_5=6. System Load CTP then write to Flash via TFTP.=run boot5\0" \
    "bootmenu_6=7. Debugger load image then write to Flash.=run boot6\0" \
    "bootmenu_7=8. System Load flashimage then write to Flash via TFTP.=run boot7\0" \
    "bootmenu_delay=30\0" \
    ""

#define CONFIG_EXTRA_ENV_SETTINGS \
    "kernel_filename=lede-kernel.bin\0" \
    "uboot_filename=u-boot-mtk.bin\0" \
    "ctp_filename=ctp.bin\0" \
    "flashimage_filename=flashimage.bin\0" \
    "invaild_env=no\0" \
    ENV_DEVICE_SETTINGS \
    ENV_BOOT_CMD \
    ENV_BOOT_MENU


/**********************************************************************************************
 *                                      FDT
 **********************************************************************************************/
#define CONFIG_FIT
#define CONFIG_OF_LIBFDT
#define CONFIG_FDT_DEBUG

/**********************************************************************************************
 *                                       UBoot Command
 **********************************************************************************************/
/* Shell */
#define CONFIG_SYS_MAXARGS		            8
#define CONFIG_SYS_PROMPT		            "MT7629> "
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
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMD_PING


/**********************************************************************************************
 *                                      Compression
 **********************************************************************************************/
/*
 * Iverson 20150510 :
 *      Denali-2 use lk to decompress kernel.
 */
#define CONFIG_LZMA
#define CONFIG_MTGPIO

#endif
