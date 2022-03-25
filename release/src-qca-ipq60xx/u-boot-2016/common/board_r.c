/*
 * Copyright (c) 2011 The Chromium OS Authors.
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
/* TODO: can we just include all these headers whether needed or not? */
#if defined(CONFIG_CMD_BEDBUG)
#include <bedbug/type.h>
#endif
#include <command.h>
#include <console.h>
#ifdef CONFIG_HAS_DATAFLASH
#include <dataflash.h>
#endif
#include <dm.h>
#include <environment.h>
#include <fdtdec.h>
#if defined(CONFIG_CMD_IDE)
#include <ide.h>
#endif
#include <initcall.h>
#ifdef CONFIG_PS2KBD
#include <keyboard.h>
#endif
#if defined(CONFIG_CMD_KGDB)
#include <kgdb.h>
#endif
#include <logbuff.h>
#include <malloc.h>
#include <mapmem.h>
#ifdef CONFIG_BITBANGMII
#include <miiphy.h>
#endif
#include <mmc.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <scsi.h>
#include <serial.h>
#include <spi.h>
#include <stdio_dev.h>
#include <trace.h>
#include <watchdog.h>
#ifdef CONFIG_CMD_AMBAPP
#include <ambapp.h>
#endif
#ifdef CONFIG_ADDR_MAP
#include <asm/mmu.h>
#endif
#include <asm/sections.h>
#ifdef CONFIG_X86
#include <asm/init_helpers.h>
#endif
#include <dm/root.h>
#include <linux/compiler.h>
#include <linux/err.h>
#ifdef CONFIG_AVR32
#include <asm/arch/mmu.h>
#endif
#ifdef CONFIG_SYS_HUSH_PARSER
#include <cli_hush.h>
#endif

#if defined(CONFIG_ASUS_PRODUCT)
#include <cli.h>
#include <version.h>
#include <flash_wrapper.h>
#include <gpio.h>
#include <replace.h>
#include <flash_wrapper.h>
#include <cmd_tftpServer.h>
#if defined(CONFIG_UBI_SUPPORT)
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <ubi_uboot.h>
#endif
#endif

#if defined(CONFIG_AQR_PHYADDR)
#include <netdev.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SPARC)
extern int prom_init(void);
#endif

#ifdef crc32
#undef crc32
#endif

#if defined(CONFIG_ASUS_PRODUCT)
extern int do_bootm(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_load_serial_bin(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_tftpb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_tftpd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
extern int do_source (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#if defined(CONFIG_AQR_PHYADDR)
extern int ipq_board_fw_download(unsigned int phy_addr);
extern void __ipq_qca_aquantia_phy_init(void);
#endif

#if defined(UBOOT_STAGE1)
#define BOOTFILENAME	"uboot_stage1.img"
#elif defined(UBOOT_STAGE2)
#define BOOTFILENAME	"uboot_stage2.img"
#else
#define BOOTFILENAME	"u-boot_" CONFIG_FLASH_TYPE ".img"
#endif

#define SEL_LOAD_LINUX_WRITE_FLASH_BY_SERIAL	'0'
#define SEL_LOAD_LINUX_SDRAM			'1'
#define SEL_LOAD_LINUX_WRITE_FLASH		'2'
#define SEL_BOOT_FLASH				'3'
#define SEL_ENTER_CLI				'4'
#define SEL_LOAD_BOOT_SDRAM_VIA_SERIAL		'5'
#define SEL_LOAD_BOOT_WRITE_FLASH_BY_SERIAL	'7'
#define SEL_LOAD_BOOT_SDRAM			'8'
#define SEL_LOAD_BOOT_WRITE_FLASH		'9'

#if defined(UBOOT_STAGE1)
extern struct stage2_loc g_s2_loc;

#define BOOT_IMAGE_NAME	"Bootloader stage1 code"
#define SYS_IMAGE_NAME	"Bootloader stage2 code"
#elif defined(UBOOT_STAGE2)
#define BOOT_IMAGE_NAME "Bootloader stage1/2 code"
#define SYS_IMAGE_NAME  "System code"
#else
#define BOOT_IMAGE_NAME "Boot Loader code"
#define SYS_IMAGE_NAME  "System code"
#endif

int modifies = 0;

#define ARGV_LEN	128
static char file_name_space[ARGV_LEN];
#endif	/* CONFIG_ASUS_PRODUCT */

ulong monitor_flash_len;

__weak int board_flash_wp_on(void)
{
	/*
	 * Most flashes can't be detected when write protection is enabled,
	 * so provide a way to let U-Boot gracefully ignore write protected
	 * devices.
	 */
	return 0;
}

__weak void cpu_secondary_init_r(void)
{
}

static int initr_secondary_cpu(void)
{
	/*
	 * after non-volatile devices & environment is setup and cpu code have
	 * another round to deal with any initialization that might require
	 * full access to the environment or loading of some image (firmware)
	 * from a non-volatile device
	 */
	/* TODO: maybe define this for all archs? */
	cpu_secondary_init_r();

	return 0;
}

static int initr_trace(void)
{
#ifdef CONFIG_TRACE
	trace_init(gd->trace_buff, CONFIG_TRACE_BUFFER_SIZE);
#endif

	return 0;
}

static int initr_reloc(void)
{
	/* tell others: relocation done */
	gd->flags |= GD_FLG_RELOC | GD_FLG_FULL_MALLOC_INIT;

	return 0;
}

#ifdef CONFIG_ARM
/*
 * Some of these functions are needed purely because the functions they
 * call return void. If we change them to return 0, these stubs can go away.
 */
static int initr_caches(void)
{
	/* Enable caches */
	enable_caches();
	return 0;
}
#endif

__weak int fixup_cpu(void)
{
	return 0;
}

static int initr_reloc_global_data(void)
{
#ifdef __ARM__
	monitor_flash_len = _end - __image_copy_start;
#elif defined(CONFIG_NDS32)
	monitor_flash_len = (ulong)&_end - (ulong)&_start;
#elif !defined(CONFIG_SANDBOX) && !defined(CONFIG_NIOS2)
	monitor_flash_len = (ulong)&__init_end - gd->relocaddr;
#endif
#if defined(CONFIG_MPC85xx) || defined(CONFIG_MPC86xx)
	/*
	 * The gd->cpu pointer is set to an address in flash before relocation.
	 * We need to update it to point to the same CPU entry in RAM.
	 * TODO: why not just add gd->reloc_ofs?
	 */
	gd->arch.cpu += gd->relocaddr - CONFIG_SYS_MONITOR_BASE;

	/*
	 * If we didn't know the cpu mask & # cores, we can save them of
	 * now rather than 'computing' them constantly
	 */
	fixup_cpu();
#endif
#ifdef CONFIG_SYS_EXTRA_ENV_RELOC
	/*
	 * Some systems need to relocate the env_addr pointer early because the
	 * location it points to will get invalidated before env_relocate is
	 * called.  One example is on systems that might use a L2 or L3 cache
	 * in SRAM mode and initialize that cache from SRAM mode back to being
	 * a cache in cpu_init_r.
	 */
	gd->env_addr += gd->relocaddr - CONFIG_SYS_MONITOR_BASE;
#endif
#ifdef CONFIG_OF_EMBED
	/*
	* The fdt_blob needs to be moved to new relocation address
	* incase of FDT blob is embedded with in image
	*/
	gd->fdt_blob += gd->reloc_off;
#endif

	return 0;
}

static int initr_serial(void)
{
	serial_initialize();
	return 0;
}

#if defined(CONFIG_PPC) || defined(CONFIG_M68K)
static int initr_trap(void)
{
	/*
	 * Setup trap handlers
	 */
#if defined(CONFIG_PPC)
	trap_init(gd->relocaddr);
#else
	trap_init(CONFIG_SYS_SDRAM_BASE);
#endif
	return 0;
}
#endif

#ifdef CONFIG_ADDR_MAP
static int initr_addr_map(void)
{
	init_addr_map();

	return 0;
}
#endif

#ifdef CONFIG_LOGBUFFER
unsigned long logbuffer_base(void)
{
	return gd->ram_top - LOGBUFF_LEN;
}

static int initr_logbuffer(void)
{
	logbuff_init_ptrs();
	return 0;
}
#endif

#ifdef CONFIG_POST
static int initr_post_backlog(void)
{
	post_output_backlog();
	return 0;
}
#endif

#ifdef CONFIG_SYS_DELAYED_ICACHE
static int initr_icache_enable(void)
{
	return 0;
}
#endif

#if defined(CONFIG_SYS_INIT_RAM_LOCK) && defined(CONFIG_E500)
static int initr_unlock_ram_in_cache(void)
{
	unlock_ram_in_cache();	/* it's time to unlock D-cache in e500 */
	return 0;
}
#endif

#ifdef CONFIG_PCI
static int initr_pci(void)
{
#ifndef CONFIG_DM_PCI
	pci_init();
#endif

	return 0;
}
#endif

#ifdef CONFIG_WINBOND_83C553
static int initr_w83c553f(void)
{
	/*
	 * Initialise the ISA bridge
	 */
	initialise_w83c553f();
	return 0;
}
#endif

static int initr_barrier(void)
{
#ifdef CONFIG_PPC
	/* TODO: Can we not use dmb() macros for this? */
	asm("sync ; isync");
#endif
	return 0;
}

static int initr_malloc(void)
{
	ulong malloc_start;

#ifdef CONFIG_SYS_MALLOC_F_LEN
	debug("Pre-reloc malloc() used %#lx bytes (%ld KB)\n", gd->malloc_ptr,
	      gd->malloc_ptr / 1024);
#endif
	/* The malloc area is immediately below the monitor copy in DRAM */
	malloc_start = gd->relocaddr - TOTAL_MALLOC_LEN;
	mem_malloc_init((ulong)map_sysmem(malloc_start, TOTAL_MALLOC_LEN),
			TOTAL_MALLOC_LEN);
	return 0;
}

static int initr_console_record(void)
{
#if defined(CONFIG_CONSOLE_RECORD)
	return console_record_init();
#else
	return 0;
#endif
}

#ifdef CONFIG_SYS_NONCACHED_MEMORY
static int initr_noncached(void)
{
	noncached_init();
	return 0;
}
#endif

#ifdef CONFIG_DM
static int initr_dm(void)
{
	/* Save the pre-reloc driver model and start a new one */
	gd->dm_root_f = gd->dm_root;
	gd->dm_root = NULL;
#ifdef CONFIG_TIMER
	gd->timer = NULL;
#endif
	return dm_init_and_scan(false);
}
#endif

static int initr_bootstage(void)
{
	/* We cannot do this before initr_dm() */
	bootstage_mark_name(BOOTSTAGE_ID_START_UBOOT_R, "board_init_r");

	return 0;
}

__weak int power_init_board(void)
{
	return 0;
}

static int initr_announce(void)
{
	debug("Now running in RAM - U-Boot at: %08lx\n", gd->relocaddr);
	return 0;
}

#ifdef CONFIG_NEEDS_MANUAL_RELOC
static int initr_manual_reloc_cmdtable(void)
{
	fixup_cmdtable(ll_entry_start(cmd_tbl_t, cmd),
		       ll_entry_count(cmd_tbl_t, cmd));
	return 0;
}
#endif

#if !defined(CONFIG_SYS_NO_FLASH)
static int initr_flash(void)
{
	ulong flash_size = 0;
	bd_t *bd = gd->bd;

	puts("Flash: ");

	if (board_flash_wp_on())
		printf("Uninitialized - Write Protect On\n");
	else
		flash_size = flash_init();

	print_size(flash_size, "");
#ifdef CONFIG_SYS_FLASH_CHECKSUM
	/*
	* Compute and print flash CRC if flashchecksum is set to 'y'
	*
	* NOTE: Maybe we should add some WATCHDOG_RESET()? XXX
	*/
	if (getenv_yesno("flashchecksum") == 1) {
		printf("  CRC: %08X", crc32(0,
			(const unsigned char *) CONFIG_SYS_FLASH_BASE,
			flash_size));
	}
#endif /* CONFIG_SYS_FLASH_CHECKSUM */
	putc('\n');

	/* update start of FLASH memory    */
#ifdef CONFIG_SYS_FLASH_BASE
	bd->bi_flashstart = CONFIG_SYS_FLASH_BASE;
#endif
	/* size of FLASH memory (final value) */
	bd->bi_flashsize = flash_size;

#if defined(CONFIG_SYS_UPDATE_FLASH_SIZE)
	/* Make a update of the Memctrl. */
	update_flash_size(flash_size);
#endif


#if defined(CONFIG_OXC) || defined(CONFIG_RMU)
	/* flash mapped at end of memory map */
	bd->bi_flashoffset = CONFIG_SYS_TEXT_BASE + flash_size;
#elif CONFIG_SYS_MONITOR_BASE == CONFIG_SYS_FLASH_BASE
	bd->bi_flashoffset = monitor_flash_len;	/* reserved area for monitor */
#endif
	return 0;
}
#endif

#if defined(CONFIG_PPC) && !defined(CONFIG_DM_SPI)
static int initr_spi(void)
{
	/* PPC does this here */
#ifdef CONFIG_SPI
#if !defined(CONFIG_ENV_IS_IN_EEPROM)
	spi_init_f();
#endif
	spi_init_r();
#endif
	return 0;
}
#endif

#ifdef CONFIG_CMD_NAND
/* go init the NAND */
static int initr_nand(void)
{
	puts("NAND:  ");
	nand_init();
	return 0;
}
#endif

#if defined(CONFIG_CMD_ONENAND)
/* go init the NAND */
static int initr_onenand(void)
{
	puts("NAND:  ");
	onenand_init();
	return 0;
}
#endif

#ifdef CONFIG_GENERIC_MMC
static int initr_mmc(void)
{
	puts("MMC:   ");
	mmc_initialize(gd->bd);
	return 0;
}
#endif

#ifdef CONFIG_HAS_DATAFLASH
static int initr_dataflash(void)
{
	AT91F_DataflashInit();
	dataflash_print_info();
	return 0;
}
#endif

/*
 * Tell if it's OK to load the environment early in boot.
 *
 * If CONFIG_OF_CONFIG is defined, we'll check with the FDT to see
 * if this is OK (defaulting to saying it's OK).
 *
 * NOTE: Loading the environment early can be a bad idea if security is
 *       important, since no verification is done on the environment.
 *
 * @return 0 if environment should not be loaded, !=0 if it is ok to load
 */
static int should_load_env(void)
{
#ifdef CONFIG_OF_CONTROL
	return fdtdec_get_config_int(gd->fdt_blob, "load-environment", 1);
#elif defined CONFIG_DELAY_ENVIRONMENT
	return 0;
#else
	return 1;
#endif
}

static int initr_env(void)
{
	/* initialize environment */
	if (should_load_env())
		env_relocate();
	else
		set_default_env(NULL);
#ifdef CONFIG_OF_CONTROL
	setenv_addr("fdtcontroladdr", gd->fdt_blob);
#endif

	/* Initialize from environment */
	load_addr = getenv_ulong("loadaddr", 16, load_addr);
#if defined(CONFIG_SYS_EXTBDINFO)
#if defined(CONFIG_405GP) || defined(CONFIG_405EP)
#if defined(CONFIG_I2CFAST)
	/*
	 * set bi_iic_fast for linux taking environment variable
	 * "i2cfast" into account
	 */
	{
		char *s = getenv("i2cfast");

		if (s && ((*s == 'y') || (*s == 'Y'))) {
			gd->bd->bi_iic_fast[0] = 1;
			gd->bd->bi_iic_fast[1] = 1;
		}
	}
#endif /* CONFIG_I2CFAST */
#endif /* CONFIG_405GP, CONFIG_405EP */
#endif /* CONFIG_SYS_EXTBDINFO */
	return 0;
}

#ifdef CONFIG_SYS_BOOTPARAMS_LEN
static int initr_malloc_bootparams(void)
{
	gd->bd->bi_boot_params = (ulong)malloc(CONFIG_SYS_BOOTPARAMS_LEN);
	if (!gd->bd->bi_boot_params) {
		puts("WARNING: Cannot allocate space for boot parameters\n");
		return -ENOMEM;
	}
	return 0;
}
#endif

static int initr_jumptable(void)
{
	jumptable_init();
	return 0;
}

#if defined(CONFIG_API)
static int initr_api(void)
{
	/* Initialize API */
	api_init();
	return 0;
}
#endif

/* enable exceptions */
#if defined(CONFIG_ARM) || defined(CONFIG_AVR32)
static int initr_enable_interrupts(void)
{
	enable_interrupts();
	return 0;
}
#endif

#if defined(CONFIG_ASUS_PRODUCT) && defined(CONFIG_QCA_APPSBL_DLOAD)
static int check_clear_crashdump_magic(void)
{
#if defined(CONFIG_ARCH_IPQ806x)
	unsigned long * dmagic1 = (unsigned long *) 0x2A03F000;
	unsigned long * dmagic2 = (unsigned long *) 0x2A03F004;

	/* check if we are in download mode */
	if (*dmagic1 == 0xE47B337D && *dmagic2 == 0x0501CAB0) {
		/* clear the magic and run the dump command */
		*dmagic1 = 0;
		*dmagic2 = 0;
		printf("\nCrashdump magic found, clear it and reboot!\n");
		/* reset the system, some images might not be loaded
		 * when crashmagic is found
		 */
		run_command("reset", 0);
	}
#elif defined(CONFIG_ARCH_IPQ807x) || defined(CONFIG_ARCH_IPQ6018)
	char *p;

	/*
	 * If kernel has crashed in previous boot,
	 * jump to crash dump collection.
	 */
	if (!apps_iscrashed())
		return 0;

	p = getenv("dump_crashdump");
	if (p != NULL && !strcmp(p, "yes")) {
		printf("\nCrashdump magic found, upload crashdump!\n");
		setenv("ethaddr", MK_STR(CONFIG_ETHADDR));
		dump_func(FULL_DUMP);
	} else {
		printf("\nCrashdump magic found, clear it and reboot!\n");
	}

	/* reset the system, some images might not be loaded
	 * when crashmagic is found
	 * reset_crashdump() will clear crashdump magic.
	 */
	run_command("reset", 0);
#else
#error Define Dump Action!
#endif
	return 0;
}
#endif

#ifdef CONFIG_CMD_NET
static int initr_ethaddr(void)
{
	bd_t *bd = gd->bd;

	/* kept around for legacy kernels only ... ignore the next section */
	eth_getenv_enetaddr("ethaddr", bd->bi_enetaddr);
#ifdef CONFIG_HAS_ETH1
	eth_getenv_enetaddr("eth1addr", bd->bi_enet1addr);
#endif
#ifdef CONFIG_HAS_ETH2
	eth_getenv_enetaddr("eth2addr", bd->bi_enet2addr);
#endif
#ifdef CONFIG_HAS_ETH3
	eth_getenv_enetaddr("eth3addr", bd->bi_enet3addr);
#endif
#ifdef CONFIG_HAS_ETH4
	eth_getenv_enetaddr("eth4addr", bd->bi_enet4addr);
#endif
#ifdef CONFIG_HAS_ETH5
	eth_getenv_enetaddr("eth5addr", bd->bi_enet5addr);
#endif
	return 0;
}
#endif /* CONFIG_CMD_NET */

#ifdef CONFIG_CMD_KGDB
static int initr_kgdb(void)
{
	puts("KGDB:  ");
	kgdb_init();
	return 0;
}
#endif

#if defined(CONFIG_STATUS_LED)
static int initr_status_led(void)
{
#if defined(STATUS_LED_BOOT)
	status_led_set(STATUS_LED_BOOT, STATUS_LED_BLINKING);
#else
	status_led_init();
#endif
	return 0;
}
#endif

#if defined(CONFIG_CMD_AMBAPP) && defined(CONFIG_SYS_AMBAPP_PRINT_ON_STARTUP)
extern int do_ambapp_print(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

static int initr_ambapp_print(void)
{
	puts("AMBA:\n");
	do_ambapp_print(NULL, 0, 0, NULL);

	return 0;
}
#endif

#if defined(CONFIG_CMD_SCSI)
static int initr_scsi(void)
{
	puts("SCSI:  ");
	scsi_init();

	return 0;
}
#endif

#if defined(CONFIG_CMD_DOC)
static int initr_doc(void)
{
	puts("DOC:   ");
	doc_init();
	return 0;
}
#endif

#ifdef CONFIG_BITBANGMII
static int initr_bbmii(void)
{
	bb_miiphy_init();
	return 0;
}
#endif

#ifdef CONFIG_CMD_NET
static int initr_net(void)
{
#if !defined(CONFIG_IPQ_ETH_INIT_DEFER)
	puts("Net:   ");
	eth_initialize();
#endif
#if defined(CONFIG_RESET_PHY_R)
	debug("Reset Ethernet PHY\n");
	reset_phy();
#endif
	return 0;
}
#endif

#ifdef CONFIG_POST
static int initr_post(void)
{
	post_run(NULL, POST_RAM | post_bootmode_get(0));
	return 0;
}
#endif

#if defined(CONFIG_CMD_PCMCIA) && !defined(CONFIG_CMD_IDE)
static int initr_pcmcia(void)
{
	puts("PCMCIA:");
	pcmcia_init();
	return 0;
}
#endif

#if defined(CONFIG_CMD_IDE)
static int initr_ide(void)
{
#ifdef	CONFIG_IDE_8xx_PCCARD
	puts("PCMCIA:");
#else
	puts("IDE:   ");
#endif
#if defined(CONFIG_START_IDE)
	if (board_start_ide())
		ide_init();
#else
	ide_init();
#endif
	return 0;
}
#endif

#if defined(CONFIG_PRAM) || defined(CONFIG_LOGBUFFER)
/*
 * Export available size of memory for Linux, taking into account the
 * protected RAM at top of memory
 */
int initr_mem(void)
{
	ulong pram = 0;
	char memsz[32];

# ifdef CONFIG_PRAM
	pram = getenv_ulong("pram", 10, CONFIG_PRAM);
# endif
# if defined(CONFIG_LOGBUFFER) && !defined(CONFIG_ALT_LB_ADDR)
	/* Also take the logbuffer into account (pram is in kB) */
	pram += (LOGBUFF_LEN + LOGBUFF_OVERHEAD) / 1024;
# endif
	sprintf(memsz, "%ldk", (long int) ((gd->ram_size / 1024) - pram));
	setenv("mem", memsz);

	return 0;
}
#endif

#ifdef CONFIG_CMD_BEDBUG
static int initr_bedbug(void)
{
	bedbug_init();

	return 0;
}
#endif

#ifdef CONFIG_PS2KBD
static int initr_kbd(void)
{
	puts("PS/2:  ");
	kbd_init();
	return 0;
}
#endif

#if defined(CONFIG_ASUS_PRODUCT)
#if !defined(UBOOT_STAGE1)
void set_ver(void)
{
	int rc;

	rc = replace(OFFSET_BOOT_VER, (unsigned char*)blver, 4);
	if (rc)
		printf("\n### [set boot ver] flash write fail\n");
}

static void __call_replace(unsigned long addr, unsigned char *ptr, int len, char *msg)
{
	int rc;
	char *status = "ok";

	if (!ptr || len <= 0)
		return;

	if (!msg)
		msg = "";

	rc = replace(addr, ptr, len);
	if (rc)
		status = "fail";

	printf("\n### [%s] flash writs %s\n", msg, status);
}

void init_mac(void)
{
	int i;
	unsigned char mac[6];
	char *tmp, *end, *default_ethaddr_str = MK_STR(CONFIG_ETHADDR);

	printf("\ninit mac\n");
	for (i = 0, tmp = default_ethaddr_str; i < 6; ++i) {
		mac[i] = tmp? simple_strtoul(tmp, &end, 16):0;
		if (tmp)
			tmp = (*end)? end+1:end;
	}
	mac[5] &= 0xFC;	/* align with 4 */
	__call_replace(RAMAC0_OFFSET, mac, sizeof(mac), "init mac");

#if defined(CONFIG_DUAL_BAND)
	mac[5] += 4;
	__call_replace(RAMAC1_OFFSET, mac, sizeof(mac), "init mac2");
#endif

	__call_replace(OFFSET_COUNTRY_CODE, (unsigned char*) "DB", 2, "init countrycode");
	__call_replace(OFFSET_PIN_CODE, (unsigned char*) "12345670", 8, "init pincode");
}

/* Restore to default. */
int reset_to_default(void)
{
	ulong addr, size;

#if defined(CONFIG_UBI_SUPPORT)
	unsigned char *p;

	addr = CFG_NVRAM_ADDR;
	size = CFG_NVRAM_SIZE;
	p = malloc(CFG_NVRAM_SIZE);
	if (!p)
		p = (unsigned char*) CONFIG_SYS_LOAD_ADDR;

	memset(p, 0xFF, CFG_NVRAM_SIZE);
	ra_flash_erase_write(p, addr, size, 0);

	if (p != (unsigned char*) CONFIG_SYS_LOAD_ADDR)
		free(p);
#endif
	/* erase U-Boot environment whether it shared same block with nvram or not. */
	addr = CONFIG_ENV_ADDR;
	size = CONFIG_ENV_SIZE;
	printf("Erase 0x%08lx size 0x%lx\n", addr, size);
	ranand_set_sbb_max_addr(addr + size);
	ra_flash_erase(addr, size);
	ranand_set_sbb_max_addr(0);

	return 0;
}
#endif	/* !UBOOT_STAGE1 */

static void input_value(char *str)
{
	if (str)
		strcpy(console_buffer, str);
	else
		console_buffer[0] = '\0';

	while(1) {
		if (__cli_readline ("==:", 1) > 0) {
			strcpy (str, console_buffer);
			break;
		}
		else
			break;
	}
}

int tftp_config(int type, char *argv[])
{
	char *s;
	char default_file[ARGV_LEN], file[ARGV_LEN], devip[ARGV_LEN], srvip[ARGV_LEN], default_ip[ARGV_LEN];
	static char buf_addr[] = "0x80060000XXX";

	printf(" Please Input new ones /or Ctrl-C to discard\n");

	memset(default_file, 0, ARGV_LEN);
	memset(file, 0, ARGV_LEN);
	memset(devip, 0, ARGV_LEN);
	memset(srvip, 0, ARGV_LEN);
	memset(default_ip, 0, ARGV_LEN);

	printf("\tInput device IP ");
	s = getenv("ipaddr");
	memcpy(devip, s, strlen(s));
	memcpy(default_ip, s, strlen(s));

	printf("(%s) ", devip);
	input_value(devip);
	setenv("ipaddr", devip);
	if (strcmp(default_ip, devip) != 0)
		modifies++;

	printf("\tInput server IP ");
	s = getenv("serverip");
	memcpy(srvip, s, strlen(s));
	memset(default_ip, 0, ARGV_LEN);
	memcpy(default_ip, s, strlen(s));

	printf("(%s) ", srvip);
	input_value(srvip);
	setenv("serverip", srvip);
	if (strcmp(default_ip, srvip) != 0)
		modifies++;

	sprintf(buf_addr, "0x%x", CONFIG_SYS_LOAD_ADDR);
	argv[1] = buf_addr;

	switch (type) {
	case SEL_LOAD_BOOT_SDRAM:	/* fall through */
	case SEL_LOAD_BOOT_WRITE_FLASH:	/* fall through */
	case SEL_LOAD_BOOT_WRITE_FLASH_BY_SERIAL:
		printf("\tInput Uboot filename ");
		strncpy(argv[2], BOOTFILENAME, ARGV_LEN);
		break;
	case SEL_LOAD_LINUX_WRITE_FLASH:/* fall through */
	case SEL_LOAD_LINUX_SDRAM:
		printf("\tInput Linux Kernel filename ");
		strncpy(argv[2], "uImage", ARGV_LEN);
		break;
	default:
		printf("%s: Unknown type %d\n", __func__, type);
	}

	s = getenv("bootfile");
	if (s != NULL) {
		memcpy(file, s, strlen(s));
		memcpy(default_file, s, strlen(s));
	}
	printf("(%s) ", file);
	input_value(file);
	if (file == NULL)
		return 1;
	copy_filename(argv[2], file, sizeof(file));
	setenv("bootfile", file);
	if (strcmp(default_file, file) != 0)
		modifies++;

	return 0;
}

#if defined(CONFIG_CMD_SOURCE)
/** Program FIT image bootloader.
 * @addr:
 * @return:
 * 	0:	success
 *  otherwise:	fail
 */
int program_bootloader_fit_image(ulong addr, ulong size)
{
	int rcode;
	char *vb, *vr;
	static char tmp[] = "$imgaddr:scriptXXXXXXXXXX";
	char *run_fit_img_script[] = { "source", tmp };
#if defined(CONFIG_UBI_SUPPORT)
	const struct ubi_device *ubi = get_ubi_device();
	char *ubi_detach[] = { "ubi", "detach"};
#endif

	if (!addr)
		return -1;

#if defined(CONFIG_UBI_SUPPORT)
	/* detach UBI_DEV */
	if (ubi)
		do_ubi(NULL, 0, ARRAY_SIZE(ubi_detach), ubi_detach);
#endif

#ifdef CONFIG_SYS_HUSH_PARSER
	u_boot_hush_start ();
#endif

#if defined(CONFIG_HUSH_INIT_VAR)
	hush_init_var ();
#endif

	vb = getenv("verbose");
	vr = getenv("verify");
	setenv("verbose", NULL);
	setenv("verify", NULL);

	sprintf(tmp, "%lx", addr);
	setenv("imgaddr", tmp);
	sprintf(tmp, "%lx:script", addr);
	ranand_set_sbb_max_addr(CFG_BOOTLOADER_SIZE);
	rcode = do_source(NULL, 0, ARRAY_SIZE(run_fit_img_script), run_fit_img_script);
	ranand_set_sbb_max_addr(0);
	setenv("verbose", vb);
	setenv("verify", vr);

	return rcode;
}
#endif

#if !defined(UBOOT_STAGE1)
/* System Load %s to SDRAM via TFTP */
static void handle_boottype_1(void)
{
	int argc= 3;
	char *argv[4];
	cmd_tbl_t c, *cmdtp = &c;

	argv[2] = &file_name_space[0];
	memset(file_name_space, 0, sizeof(file_name_space));

	tftp_config(SEL_LOAD_LINUX_SDRAM, argv);
	argc= 3;
	setenv("autostart", "yes");
	do_tftpb(cmdtp, 0, argc, argv);
}
#endif

/* System Load %s then write to Flash via TFTP */
static void handle_boottype_2(void)
{
	int argc= 3, confirm = 0;
	char *argv[4];
	cmd_tbl_t c, *cmdtp = &c;
	char addr_str[11+20];
	unsigned int load_address;

	argv[2] = &file_name_space[0];
	memset(file_name_space, 0, sizeof(file_name_space));

	printf(" Warning!! Erase Linux in Flash then burn new one. Are you sure?(Y/N)\n");
	confirm = getc();
	if (confirm != 'y' && confirm != 'Y') {
		printf(" Operation terminated\n");
		return;
	}
#if defined(UBOOT_STAGE1)
	setenv("bootfile", "uboot_stage2.img");
#endif
	tftp_config(SEL_LOAD_LINUX_WRITE_FLASH, argv);
	argc= 3;
	setenv("autostart", "no");
	do_tftpb(cmdtp, 0, argc, argv);

	load_address = simple_strtoul(argv[1], NULL, 16);
	{
#if defined(UBOOT_STAGE1)
		struct stage2_loc *s2 = &g_s2_loc;

		ranand_write_stage2(load_address, net_boot_file_size);
		ranand_locate_stage2(s2);
		sprintf(addr_str, "0x%X", s2->good->code);
#else
		ra_flash_erase_write((uchar*)load_address+sizeof(image_header_t), CFG_KERN_ADDR, net_boot_file_size, 0);
#if defined(CONFIG_DUAL_TRX)
		ra_flash_erase_write((uchar*)load_address+sizeof(image_header_t), CFG_KERN2_ADDR, net_boot_file_size-sizeof(image_header_t), 0);
#endif
#endif
	}

#if defined(CONFIG_AQR_PHYADDR)
	if (is_aqr_phy_exist()) {
		ipq_board_fw_download(CONFIG_AQR_PHYADDR);
		__ipq_qca_aquantia_phy_init();
	}
#endif

	argc= 2;
#if !defined(UBOOT_STAGE1)
	sprintf(addr_str, "0x%X#config@1", load_address+sizeof(image_header_t));
#endif
	argv[1] = &addr_str[0];
	do_bootm(cmdtp, 0, argc, argv);
}

/* System Boot Linux via Flash */
static void handle_boottype_3(void)
{
	char *argv[2] = {"", ""};
	char addr_str[11+20];
	cmd_tbl_t c, *cmdtp = &c;

#if !defined(UBOOT_STAGE1)
	sprintf(addr_str, "0x%X#config@1", CFG_KERN_ADDR);
	argv[1] = &addr_str[0];
#endif

#if !defined(UBOOT_STAGE1)
	if (!chkVer())
	       set_ver();

	if ((chkMAC()) < 0)
	       init_mac();
#endif /* ! UBOOT_STAGE1 */

//	eth_initialize();
	do_tftpd(cmdtp, 0, 2, argv);
#if defined(CONFIG_ASUS_PRODUCT)
	leds_off();
	power_led_on();
#endif
}

/* System Enter Boot Command Line Interface */
static void handle_boottype_4(void)
{
	printf ("\n%s\n", version_string);
	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;) {
		main_loop ();
	}
}

#if 0 //!defined(UBOOT_STAGE1)
/* System Load %s to SDRAM via Serial (*.bin) */
static void handle_boottype_5(void)
{
	int my_tmp, argc= 3;
	char *argv[4];
	char tftp_load_addr[] = "0x81000000XXX";
	cmd_tbl_t c, *cmdtp = &c;

	argv[2] = &file_name_space[0];
	memset(file_name_space, 0, sizeof(file_name_space));
	sprintf(tftp_load_addr, "0x%x", CONFIG_SYS_LOAD_ADDR);

	argc= 4;
	argv[1] = tftp_load_addr;
	setenv("autostart", "yes");
	my_tmp = do_load_serial_bin(cmdtp, 0, argc, argv);
	net_boot_file_size=simple_strtoul(getenv("filesize"), NULL, 16);

	if (net_boot_file_size > CFG_BOOTLOADER_SIZE || my_tmp == 1)
		printf("Abort: Bootloader is too big or download aborted!\n");
}
#endif

/* System Load %s then write to Flash via Serial */
static void handle_boottype_7(void)
{
	int my_tmp, argc= 3;
	cmd_tbl_t c, *cmdtp = &c;
	char tftp_load_addr[] = "0x81000000XXX";
	char *argv[4] = { "loadb", tftp_load_addr, NULL, NULL };
	unsigned int addr = CONFIG_SYS_LOAD_ADDR;

	sprintf(tftp_load_addr, "0x%x", addr);
	argv[1] = tftp_load_addr;
	argc=2;
	setenv("autostart", "no");
	my_tmp = do_load_serial_bin(cmdtp, 0, argc, argv);
	if (my_tmp == 1) {
		printf("Abort: Load bootloader from serial fail!\n");
		return;
	}

	net_boot_file_size = simple_strtoul(getenv("filesize"), NULL, 16);
	if (net_boot_file_size > 0 && net_boot_file_size <= CONFIG_MAX_BL_BINARY_SIZE)
		program_bootloader(addr, net_boot_file_size);

	//reset
	do_reset(cmdtp, 0, argc, argv);
}

#if 0 //!defined(UBOOT_STAGE1)
/* System Load %s to SDRAM via TFTP.(*.bin) */
static void handle_boottype_8(void)
{
	int argc= 3;
	char *argv[4];
	cmd_tbl_t c, *cmdtp = &c;

	argv[2] = &file_name_space[0];
	memset(file_name_space, 0, sizeof(file_name_space));

	tftp_config(SEL_LOAD_BOOT_SDRAM, argv);
	argc= 5;
	setenv("autostart", "yes");
	do_tftpb(cmdtp, 0, argc, argv);
}
#endif

/* System Load %s then write to Flash via TFTP. (.bin) */
static void handle_boottype_9(void)
{
	int argc= 3, confirm = 0;
	char *argv[4];
	cmd_tbl_t c, *cmdtp = &c;

	argv[2] = &file_name_space[0];
	memset(file_name_space, 0, sizeof(file_name_space));

	printf(" Warning!! Erase %s in Flash then burn new one. Are you sure?(Y/N)\n", BOOT_IMAGE_NAME);
	confirm = getc();
	if (confirm != 'y' && confirm != 'Y') {
		printf(" Operation terminated\n");
		return;
	}
	setenv("bootfile", BOOTFILENAME);
	tftp_config(SEL_LOAD_BOOT_WRITE_FLASH, argv);
	argc= 3;
	setenv("autostart", "no");
	do_tftpb(cmdtp, 0, argc, argv);
	if (net_boot_file_size > 0 && net_boot_file_size <= CONFIG_MAX_BL_BINARY_SIZE)
		program_bootloader(CONFIG_SYS_LOAD_ADDR, net_boot_file_size);

	//reset
	do_reset(cmdtp, 0, argc, argv);
}

#if 0 //defined (CONFIG_ENV_IS_IN_NAND)
/* System Load %s then write to Flash via Serial */
static void handle_boottype_0(void)
{
	int argc= 3;
	char *argv[4];
	cmd_tbl_t c, *cmdtp = &c;

	argc= 1;
	setenv("autostart", "no");
	do_load_serial_bin(cmdtp, 0, argc, argv);
	net_boot_file_size=simple_strtoul(getenv("filesize"), NULL, 16);
#if defined(UBOOT_STAGE1)
	ranand_write_stage2(CONFIG_SYS_LOAD_ADDR, net_boot_file_size);
#else
	ra_flash_erase_write((uchar*) CONFIG_SYS_LOAD_ADDR, CFG_KERN_ADDR, net_boot_file_size, 0);
#endif

	//reset
	do_reset(cmdtp, 0, argc, argv);
}
#endif

static struct boot_menu_s {
	char type;
	void (*func)(void);
	char *msg;
	const char *param1;
} boot_menu[] = {
#if 0 //defined(CONFIG_ENV_IS_IN_NAND)
	{ SEL_LOAD_LINUX_WRITE_FLASH_BY_SERIAL,	handle_boottype_0, "Load %s then write to Flash via Serial.", SYS_IMAGE_NAME },
#endif
#if !defined(UBOOT_STAGE1)
	{ SEL_LOAD_LINUX_SDRAM,			handle_boottype_1, "Load %s to SDRAM via TFTP.", SYS_IMAGE_NAME },
#endif
	{ SEL_LOAD_LINUX_WRITE_FLASH,		handle_boottype_2, "Load %s then write to Flash via TFTP.", SYS_IMAGE_NAME },
	{ SEL_BOOT_FLASH,			handle_boottype_3, "Boot %s via Flash (default).", SYS_IMAGE_NAME },
	{ SEL_ENTER_CLI,			handle_boottype_4, "Entr boot command line interface.", NULL },
#if 0 //!defined(UBOOT_STAGE1)
	{ SEL_LOAD_BOOT_SDRAM_VIA_SERIAL,	handle_boottype_5, "Load %s to SDRAM via Serial.", BOOT_IMAGE_NAME },
#endif
	{ SEL_LOAD_BOOT_WRITE_FLASH_BY_SERIAL,	handle_boottype_7, "Load %s then write to Flash via Serial.", BOOT_IMAGE_NAME },
#if 0 //!defined(UBOOT_STAGE1)
	{ SEL_LOAD_BOOT_SDRAM,			handle_boottype_8, "Load %s to SDRAM via TFTP.", BOOT_IMAGE_NAME },
#endif
	{ SEL_LOAD_BOOT_WRITE_FLASH,		handle_boottype_9, "Load %s then write to Flash via TFTP.", BOOT_IMAGE_NAME },

	{ 0, NULL, NULL, NULL },
};

static int OperationSelect(void)
{
	char valid_boot_type[16];
	char msg[256];
	struct boot_menu_s *p = &boot_menu[0];
	char *s = getenv ("bootdelay"), *q = &valid_boot_type[0];
	int my_tmp, BootType = '3', timer1 = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

	/* clear uart buf */
	while ((my_tmp = tstc()) != 0)
		BootType = getc();

	memset(valid_boot_type, 0, sizeof(valid_boot_type));
	printf("\nPlease choose the operation: \n");
	while (p->func) {
		*q++ = p->type;
		sprintf(msg, "   %c: %s\n", p->type, p->msg);
		if (p->param1)
			printf(msg, p->param1);
		else
			printf(msg);

		p++;
	}
	*q = '\0';

	if (timer1 > 5)
		timer1 = 5;
#if defined(UBOOT_STAGE1) || defined(UBOOT_STAGE2)
	if (timer1 <= CONFIG_BOOTDELAY)
		timer1 = 0;
#endif

	timer1 *= 100;
	if (!timer1)
		timer1 = 20;
	while (timer1 > 0) {
		--timer1;
		/* delay 10ms */
		if ((my_tmp = tstc()) != 0) {	/* we got a key press	*/
			timer1 = 0;	/* no more delay	*/
			BootType = getc();
			if (!strchr(valid_boot_type, BootType))
				BootType = '3';
			printf("\n\rYou choosed %c\n\n", BootType);
			break;
		}
		if (DETECT() || DETECT_WPS()) {
			BootType = '3';
			break;
		}
		udelay (10000);
		if ((timer1 / 100 * 100) == timer1)
			printf ("\b\b\b%2d ", timer1 / 100);
	}
	putc ('\n');

	return BootType;
}

#if defined(GTAXY16000)
typedef struct {
	unsigned char addr;
	unsigned char val[2];
} i2c_cmd_tpl;
#define	AURA_DEV_ADDRESS	0x4E
#define	SAFE_MEMADDRESS		0x42000000

void AURA_LED_init(void)
{
	char runcmd[256];
	i2c_cmd_tpl cmd_list[] = {
		{ 0x0, { 0x80, 0x20} },
		{ 0x3, { 0x01, 0x00} },
		{ 0x0, { 0x80, 0x10} },
		{ 0x3, { 0x01, 0x80} }, /*Red*/
		{ 0x0, { 0x80, 0x12} },
		{ 0x3, { 0x01, 0x00} }, /*Green*/
		{ 0x0, { 0x80, 0x11} },
		{ 0x3, { 0x01, 0x00} }, /*Blue*/
		{ 0x0, { 0x80, 0x20} },
		{ 0x3, { 0x01, 0x00} },
		{ 0x0, { 0x80, 0x24} },
		{ 0x3, { 0x01, 0x00} },
		{ 0x0, { 0x80, 0x21} },
		{ 0x3, { 0x01, 0x03} }, /*mode*/
		{ 0x0, { 0x80, 0x22} },
		{ 0x3, { 0x01, 0x01} }, /*speed*/
		{ 0x0, { 0x80, 0x23} },
		{ 0x3, { 0x01, 0x00} }, /*direction*/
		{ 0x0, { 0x80, 0x2f} },
		{ 0x3, { 0x01, 0x01} }, /*apply*/
/* STOP testing cmd
		{ 0x0, { 0x80, 0x21} },
		{ 0x3, { 0x01, 0x02} },
		{ 0x0, { 0x80, 0x2f} },
		{ 0x3, { 0x01, 0x01} },
*/
	};
	int i;
	unsigned char *pt = (unsigned char *)SAFE_MEMADDRESS;
	/* init */
	snprintf(runcmd, sizeof(runcmd), "i2c dev 0 && i2c probe");
	if (run_command(runcmd, 0)) {
		printf("!!! NO I2C device detected !!!\n");
		return;
	}
	for ( i=0; i<sizeof(cmd_list)/sizeof(i2c_cmd_tpl); i++ ) {
		*pt = cmd_list[i].val[0];
		*(pt+1) = cmd_list[i].val[1];
		snprintf(runcmd, sizeof(runcmd), "i2c write %x %x %x.1 2 -s", SAFE_MEMADDRESS, AURA_DEV_ADDRESS, cmd_list[i].addr);
		//printf("VVVV[%s]\n", runcmd);
		run_command(runcmd, 0);
	}
}
#endif /* GTAXY16000 */
#endif /* CONFIG_ASUS_PRODUCT */

static int run_main_loop(void)
{
#if defined(CONFIG_ASUS_PRODUCT)
	cmd_tbl_t *cmdtp = NULL;
	char *argv[4], msg[256];
	int argc = 3, BootType = '3';
	struct boot_menu_s *p = &boot_menu[0];

	argv[2] = &file_name_space[0];
	file_name_space[0] = '\0';
#endif

#ifdef CONFIG_SANDBOX
	sandbox_main_loop_init();
#endif
#if defined(CONFIG_ASUS_PRODUCT)
	/* Boot Loader Menu */

#if defined(GTAXY16000)
	// AURA_LED_init();
#endif
	//LANWANPartition();	/* FIXME */
	disable_all_leds();	/* Inhibit ALL LED, except PWR LED. */
	leds_off();
	power_led_on();

	ra_flash_init_layout();

	BootType = OperationSelect();
	for (p = &boot_menu[0]; p->func; ++p ) {
		if (p->type != BootType) {
			continue;
		}

		sprintf(msg, "   %c: %s\n", p->type, p->msg);
		if (p->param1)
			printf(msg, p->param1);
		else
			printf(msg);

		p->func();
		break;
	}

	if (!p->func) {
		printf("   \nSystem Boot Linux via Flash.\n");
		do_bootm(cmdtp, 0, 1, argv);
	}

	for (;;) {
		do_reset(cmdtp, 0, argc, argv);
	}
#else	/* !CONFIG_ASUS_PRODUCT */
	/* main_loop() can return to retry autoboot, if so just run it again */
	for (;;)
		main_loop();
#endif	/* CONFIG_ASUS_PRODUCT */

	return 0;
}

/*
 * Over time we hope to remove these functions with code fragments and
 * stub funtcions, and instead call the relevant function directly.
 *
 * We also hope to remove most of the driver-related init and do it if/when
 * the driver is later used.
 *
 * TODO: perhaps reset the watchdog in the initcall function after each call?
 */
init_fnc_t init_sequence_r[] = {
	initr_trace,
	initr_reloc,
	/* TODO: could x86/PPC have this also perhaps? */
#ifdef CONFIG_ARM
	initr_caches,
	/* Note: For Freescale LS2 SoCs, new MMU table is created in DDR.
	 *	 A temporary mapping of IFC high region is since removed,
	 *	 so environmental variables in NOR flash is not availble
	 *	 until board_init() is called below to remap IFC to high
	 *	 region.
	 */
#endif
	initr_reloc_global_data,
#if defined(CONFIG_SYS_INIT_RAM_LOCK) && defined(CONFIG_E500)
	initr_unlock_ram_in_cache,
#endif
	initr_barrier,
	initr_malloc,
	initr_console_record,
#ifdef CONFIG_SYS_NONCACHED_MEMORY
	initr_noncached,
#endif
	bootstage_relocate,
#ifdef CONFIG_DM
	initr_dm,
#endif
	initr_bootstage,
#if defined(CONFIG_ARM) || defined(CONFIG_NDS32)
	board_init,	/* Setup chipselects */
#endif
	/*
	 * TODO: printing of the clock inforamtion of the board is now
	 * implemented as part of bdinfo command. Currently only support for
	 * davinci SOC's is added. Remove this check once all the board
	 * implement this.
	 */
#ifdef CONFIG_CLOCKS
	set_cpu_clk_info, /* Setup clock information */
#endif
	stdio_init_tables,
	initr_serial,
	initr_announce,
	INIT_FUNC_WATCHDOG_RESET
#ifdef CONFIG_NEEDS_MANUAL_RELOC
	initr_manual_reloc_cmdtable,
#endif
#if defined(CONFIG_PPC) || defined(CONFIG_M68K)
	initr_trap,
#endif
#ifdef CONFIG_ADDR_MAP
	initr_addr_map,
#endif
#if defined(CONFIG_BOARD_EARLY_INIT_R)
	board_early_init_r,
#endif
	INIT_FUNC_WATCHDOG_RESET
#ifdef CONFIG_LOGBUFFER
	initr_logbuffer,
#endif
#ifdef CONFIG_POST
	initr_post_backlog,
#endif
	INIT_FUNC_WATCHDOG_RESET
#ifdef CONFIG_SYS_DELAYED_ICACHE
	initr_icache_enable,
#endif
#if defined(CONFIG_PCI) && defined(CONFIG_SYS_EARLY_PCI_INIT)
	/*
	 * Do early PCI configuration _before_ the flash gets initialised,
	 * because PCU ressources are crucial for flash access on some boards.
	 */
	initr_pci,
#endif
#ifdef CONFIG_WINBOND_83C553
	initr_w83c553f,
#endif
#ifdef CONFIG_ARCH_EARLY_INIT_R
	arch_early_init_r,
#endif
	power_init_board,
#ifndef CONFIG_SYS_NO_FLASH
	initr_flash,
#endif
	INIT_FUNC_WATCHDOG_RESET
#if defined(CONFIG_PPC) || defined(CONFIG_M68K) || defined(CONFIG_X86) || \
	defined(CONFIG_SPARC)
	/* initialize higher level parts of CPU like time base and timers */
	cpu_init_r,
#endif
#ifdef CONFIG_PPC
	initr_spi,
#endif
#ifdef CONFIG_CMD_NAND
	initr_nand,
#endif
#ifdef CONFIG_CMD_ONENAND
	initr_onenand,
#endif
#ifdef CONFIG_GENERIC_MMC
	initr_mmc,
#endif
#ifdef CONFIG_HAS_DATAFLASH
	initr_dataflash,
#endif
	initr_env,
#ifdef CONFIG_SYS_BOOTPARAMS_LEN
	initr_malloc_bootparams,
#endif
	INIT_FUNC_WATCHDOG_RESET
	initr_secondary_cpu,
#if defined(CONFIG_ID_EEPROM) || defined(CONFIG_SYS_I2C_MAC_OFFSET)
	mac_read_from_eeprom,
#endif
	INIT_FUNC_WATCHDOG_RESET
#if defined(CONFIG_PCI) && !defined(CONFIG_SYS_EARLY_PCI_INIT)
	/*
	 * Do pci configuration
	 */
	initr_pci,
#endif
	stdio_add_devices,
	initr_jumptable,
#ifdef CONFIG_API
	initr_api,
#endif
	console_init_r,		/* fully init console as a device */
#ifdef CONFIG_DISPLAY_BOARDINFO_LATE
	show_board_info,
#endif
#ifdef CONFIG_ARCH_MISC_INIT
	arch_misc_init,		/* miscellaneous arch-dependent init */
#endif
#ifdef CONFIG_MISC_INIT_R
	misc_init_r,		/* miscellaneous platform-dependent init */
#endif
#if defined(CONFIG_UBI_SUPPORT)
	ranand_check_and_fix_bootloader,
#endif
	INIT_FUNC_WATCHDOG_RESET
#ifdef CONFIG_CMD_KGDB
	initr_kgdb,
#endif
	interrupt_init,
#if defined(CONFIG_ARM) || defined(CONFIG_AVR32)
	initr_enable_interrupts,
#endif
#if defined(CONFIG_ASUS_PRODUCT) && defined(CONFIG_QCA_APPSBL_DLOAD)
	check_clear_crashdump_magic,
#endif
#if defined(CONFIG_MICROBLAZE) || defined(CONFIG_AVR32) || defined(CONFIG_M68K)
	timer_init,		/* initialize timer */
#endif
#if defined(CONFIG_STATUS_LED)
	initr_status_led,
#endif
	/* PPC has a udelay(20) here dating from 2002. Why? */
#ifdef CONFIG_CMD_NET
	initr_ethaddr,
#endif
#ifdef CONFIG_BOARD_LATE_INIT
	board_late_init,
#endif
#if defined(CONFIG_CMD_AMBAPP)
	ambapp_init_reloc,
#if defined(CONFIG_SYS_AMBAPP_PRINT_ON_STARTUP)
	initr_ambapp_print,
#endif
#endif
#ifdef CONFIG_CMD_SCSI
	INIT_FUNC_WATCHDOG_RESET
	initr_scsi,
#endif
#ifdef CONFIG_CMD_DOC
	INIT_FUNC_WATCHDOG_RESET
	initr_doc,
#endif
#ifdef CONFIG_BITBANGMII
	initr_bbmii,
#endif
#ifdef CONFIG_CMD_NET
	INIT_FUNC_WATCHDOG_RESET
	initr_net,
#endif
#ifdef CONFIG_POST
	initr_post,
#endif
#if defined(CONFIG_CMD_PCMCIA) && !defined(CONFIG_CMD_IDE)
	initr_pcmcia,
#endif
#if defined(CONFIG_CMD_IDE)
	initr_ide,
#endif
#ifdef CONFIG_LAST_STAGE_INIT
	INIT_FUNC_WATCHDOG_RESET
	/*
	 * Some parts can be only initialized if all others (like
	 * Interrupts) are up and running (i.e. the PC-style ISA
	 * keyboard).
	 */
	last_stage_init,
#endif
#ifdef CONFIG_CMD_BEDBUG
	INIT_FUNC_WATCHDOG_RESET
	initr_bedbug,
#endif
#if defined(CONFIG_PRAM) || defined(CONFIG_LOGBUFFER)
	initr_mem,
#endif
#ifdef CONFIG_PS2KBD
	initr_kbd,
#endif
#if defined(CONFIG_SPARC)
	prom_init,
#endif
	run_main_loop,
};

void board_init_r(gd_t *new_gd, ulong dest_addr)
{
#ifdef CONFIG_NEEDS_MANUAL_RELOC
	int i;
#endif
#ifdef CONFIG_AVR32
	mmu_init_r(dest_addr);
#endif

#if !defined(CONFIG_X86) && !defined(CONFIG_ARM) && !defined(CONFIG_ARM64)
	gd = new_gd;
#endif

#ifdef CONFIG_NEEDS_MANUAL_RELOC
	for (i = 0; i < ARRAY_SIZE(init_sequence_r); i++)
		init_sequence_r[i] += gd->reloc_off;
#endif

	if (initcall_run_list(init_sequence_r))
		hang();

	/* NOTREACHED - run_main_loop() does not return */
	hang();
}
