#undef CONFIG_BOOTCOMMAND
#undef CONFIG_IPQ_ATAG_PART_LIST
#ifndef CONFIG_SYS_FLASH_BASE
#define CONFIG_SYS_FLASH_BASE	0xC0000000	/* define fake flash address. */
#endif
#define CONFIG_SYS_BOOTM_LEN	(70 << 20)	/* 70 MB */

#define CONFIG_EXPECTED_MACHID	(0x8010000)
#define CONFIG_QCA8075_PHYBASE	(0x8)

/*
 * ASUS configuration.
 * All CONFIG_XXX will be copied to include/autoconf.mk automatically.
 */
#define SPFVER			11
#define CONFIG_MODEL		"PL-AX56_XP4"
#define CONFIG_FLASH_TYPE	"nand"
#define CONFIG_BLS_FIT_IMAGE
#define CONFIG_ECC_THRESHOLD	(3)	/* per-page bit-flips threshold. */

#define CONFIG_METATOOLDIR	"build_ipq"

#define CONFIG_UBI_SUPPORT
#define CONFIG_DUAL_TRX
#define CONFIG_BLVER		"1003"
#define CONFIG_DUAL_BAND
#define CONFIG_HAVE_WAN_RED_LED

#define CONFIG_SYS_LOAD_ADDR	0x4B000000
#define CONFIG_SYS_LONGHELP
#define CONFIG_LZMA
#define CONFIG_CMDLINE_TAG

/*
 * #define CONFIG_RTAX89U_OLD_SR1	// PCB R1.00
 * #define CONFIG_RTAX89U_OLD_ER1	// PCB R3.00
 * #define CONFIG_RTAX89U_OLD_PR1	// PCB R3.50
 */

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

/*
 * Environment variables.
 */
#define CONFIG_IPADDR		192.168.1.1
#define CONFIG_SERVERIP		192.168.1.70
#define CONFIG_NETMASK		255.255.255.0
#define CONFIG_BOOTFILE		CONFIG_MODEL ".trx"		/* RT-AX89U.trx */
#define CONFIG_BOOTCOMMAND	"tftp"
#define CONFIG_ETHADDR		00:aa:bb:cc:dd:e0
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"imgaddr="MK_STR(CONFIG_SYS_LOAD_ADDR)"\0"

/*
 * Enable commands
 */

#ifndef __ASSEMBLY__
/* ipq806x_cdp.c */
extern const char *model;
extern const char *blver;
extern const char *bl_stage;

/* board.c */
extern int modifies;
#endif

/*-----------------------------------------------------------------------
 * Factory
 */
#define RAMAC0_OFFSET	0x1014	/* 2G+5G EEPROM, second set of MAC address. */
#define RAMAC1_OFFSET	0x100E	/* 2G+5G EEPROM, first set of MAC address. */
#define FTRY_PARM_SHIFT                 (0xF000)
#define OFFSET_PIN_CODE			(FTRY_PARM_SHIFT + 0xD180)	/* 8 bytes */
#define OFFSET_COUNTRY_CODE		(FTRY_PARM_SHIFT + 0xD188)	/* 2 bytes */
#define OFFSET_BOOT_VER			(FTRY_PARM_SHIFT + 0xD18A)	/* 4 bytes */

/*-----------------------------------------------------------------------
 * Bootloader size and Config size definitions
 */
#define CONFIG_MAX_BL_BINARY_SIZE	0x460000			/* 4.5MiB - 128KiB */
#define CFG_BOOTLOADER_SIZE		CONFIG_MAX_BL_BINARY_SIZE
#define CFG_MAX_BOOTLOADER_BINARY_SIZE	CONFIG_MAX_BL_BINARY_SIZE
#define CONFIG_ENV_SIZE			0x20000


#define MTDIDS				"nand0=nand0"
/* 1. Keep Bootloader size and environment size equal to CFG_BOOTLOADER_SIZE and CONFIG_ENV_SIZE respectively.
 * 2. Make sure all partitions defined in nand/nor-system-partption.*.bin is not beyond CFG_MAX_BOOTLOADER_BINARY_SIZE + CONFIG_ENV_SIZE.
 */
#define MTDPARTS			"mtdparts=nand0:4480k(Bootloader),128k(environment),-(UBI_DEV)"
#define BOOT_MTDPARTS			"mtdparts=nand0:4608k(Bootloader),-(UBI_DEV)"

/*
 * UBI volume size definitions
 * Don't define size for tailed reserved space due to it's size varies.
 */
#define PEB_SIZE			(128 * 1024)
#define LEB_SIZE			(PEB_SIZE - (2 * 2 * 1024))
#define CFG_UBI_NVRAM_NR_LEB		1
#define CFG_UBI_FACTORY_NR_LEB		1
#define CFG_UBI_FIRMWARE_NR_LEB		397	/* 124KB x 397 = 48.074MB */
#define CFG_UBI_FIRMWARE2_NR_LEB	397
#define CFG_UBI_APP_NR_LEB		167	/* 124KB x 167 ~= 20.22MB. This volume size cannot reach requested size due to UBI's overhead. */

#define CFG_UBI_NVRAM_SIZE		(LEB_SIZE * CFG_UBI_NVRAM_NR_LEB)
#define CFG_UBI_FACTORY_SIZE		(LEB_SIZE * CFG_UBI_FACTORY_NR_LEB)
#define CFG_UBI_FACTORY2_SIZE		(LEB_SIZE * CFG_UBI_FACTORY_NR_LEB)
#define CFG_UBI_FIRMWARE_SIZE		(LEB_SIZE * CFG_UBI_FIRMWARE_NR_LEB)
#define CFG_UBI_FIRMWARE2_SIZE		(LEB_SIZE * CFG_UBI_FIRMWARE2_NR_LEB)
#define CFG_UBI_APP_SIZE		(LEB_SIZE * CFG_UBI_APP_NR_LEB)

#define CFG_NVRAM_SIZE			CFG_UBI_NVRAM_SIZE

#define CFG_FACTORY_SIZE		(CFG_UBI_FACTORY_SIZE + CFG_UBI_FACTORY2_SIZE)

#define CFG_UBI_DEV_OFFSET		(CFG_BOOTLOADER_SIZE + CONFIG_ENV_SIZE)

/* Environment address, factory address, and firmware address definitions */
/* Basically, CFG_FACTORY_ADDR and CFG_KERN_ADDR are used to compatible to original code infrastructure.
 * Real nvram area would be moved into the nvram volume of UBI device.
 * Real Factory area would be moved into the Factory volume of UBI device.
 * Real firmware area would be moved into the linux and linux2 volume of UBI device.
 */
#define CONFIG_ENV_ADDR			(CONFIG_SYS_FLASH_BASE + CFG_BOOTLOADER_SIZE)
#define CFG_NVRAM_ADDR			(CONFIG_ENV_ADDR + CONFIG_ENV_SIZE)
#define CFG_FACTORY_ADDR		(CFG_NVRAM_ADDR + CFG_NVRAM_SIZE)
#define CFG_KERN_ADDR			(CFG_FACTORY_ADDR + CFG_FACTORY_SIZE)
#define CFG_KERN2_ADDR			(CFG_KERN_ADDR + CFG_UBI_FIRMWARE_SIZE)
/*-----------------------------------------------------------------------*/


/* Include header files of demo board. */
#include <configs/ipq6018.h>
