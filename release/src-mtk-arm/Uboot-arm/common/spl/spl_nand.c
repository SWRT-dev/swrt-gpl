/*
 * Copyright (C) 2011
 * Corscience GmbH & Co. KG - Simon Schwarz <schwarz@corscience.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <config.h>
#include <spl.h>
#include <asm/io.h>
#include <nand.h>
#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>

size_t __spl_nand_uboot_base(void) { return CONFIG_SYS_NAND_U_BOOT_OFFS; }

size_t spl_nand_uboot_base(void)
	__attribute__((weak, alias("__spl_nand_uboot_base")));

size_t __spl_nand_uboot_max_size(void) { return 0; }

size_t spl_nand_uboot_max_size(void)
	__attribute__((weak, alias("__spl_nand_uboot_max_size")));

size_t __spl_nand_uboot_alignment(void) { return 4; }

size_t spl_nand_uboot_alignment(void)
	__attribute__((weak, alias("__spl_nand_uboot_alignment")));

static size_t spl_nand_search_uboot_image(void)
{
	size_t search_base = spl_nand_uboot_base();
	size_t max_search_size = spl_nand_uboot_max_size();
	size_t alignment = spl_nand_uboot_alignment();
	size_t search_end;
	struct image_header *hdr = (struct image_header *)(CONFIG_SYS_TEXT_BASE);

	if (!max_search_size)
		return search_base;

	if (!alignment)
		alignment = 4;

	search_base = ALIGN(search_base, alignment);
	search_end = ALIGN(search_base + max_search_size, alignment);

	while (search_base < search_end)
	{
		nand_spl_load_image(search_base, CONFIG_SYS_NAND_PAGE_SIZE, hdr);

		if (image_get_magic(hdr) == IH_MAGIC)
		{
			if (image_get_os(hdr) == IH_OS_U_BOOT)
				return search_base;
		}

		search_base += alignment;
	}

	return spl_nand_uboot_base();
}

void spl_nand_load_image(void)
{
	struct image_header *header;
	int *src __attribute__((unused));
	int *dst __attribute__((unused));
	size_t uboot_base;
#ifdef CONFIG_SPL_LZMA
	SizeT lzma_len;
	int ret;
#endif

	debug("spl: nand - using hw ecc\n");
	nand_init();

	/*use CONFIG_SYS_TEXT_BASE as temporary storage area */
	header = (struct image_header *)(CONFIG_SYS_TEXT_BASE);
#ifdef CONFIG_SPL_OS_BOOT
	if (!spl_start_uboot()) {
		/*
		 * load parameter image
		 * load to temp position since nand_spl_load_image reads
		 * a whole block which is typically larger than
		 * CONFIG_CMD_SPL_WRITE_SIZE therefore may overwrite
		 * following sections like BSS
		 */
		nand_spl_load_image(CONFIG_CMD_SPL_NAND_OFS,
			CONFIG_CMD_SPL_WRITE_SIZE,
			(void *)CONFIG_SYS_TEXT_BASE);
		/* copy to destintion */
		for (dst = (int *)CONFIG_SYS_SPL_ARGS_ADDR,
				src = (int *)CONFIG_SYS_TEXT_BASE;
				src < (int *)(CONFIG_SYS_TEXT_BASE +
				CONFIG_CMD_SPL_WRITE_SIZE);
				src++, dst++) {
			writel(readl(src), dst);
		}

		/* load linux */
		nand_spl_load_image(CONFIG_SYS_NAND_SPL_KERNEL_OFFS,
			CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
		spl_parse_image_header(header);
		if (header->ih_os == IH_OS_LINUX) {
			/* happy - was a linux */
			nand_spl_load_image(CONFIG_SYS_NAND_SPL_KERNEL_OFFS,
				spl_image.size, (void *)spl_image.load_addr);
			nand_deselect();
			return;
		} else {
			puts("The Expected Linux image was not "
				"found. Please check your NAND "
				"configuration.\n");
			puts("Trying to start u-boot now...\n");
		}
	}
#endif
#ifdef CONFIG_NAND_ENV_DST
	nand_spl_load_image(CONFIG_ENV_OFFSET,
		CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET, spl_image.size,
		(void *)spl_image.load_addr);
#ifdef CONFIG_ENV_OFFSET_REDUND
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND,
		CONFIG_SYS_NAND_PAGE_SIZE, (void *)header);
	spl_parse_image_header(header);
	nand_spl_load_image(CONFIG_ENV_OFFSET_REDUND, spl_image.size,
		(void *)spl_image.load_addr);
#endif
#endif

	/* Load u-boot */
	uboot_base = spl_nand_search_uboot_image();
	nand_spl_load_image(uboot_base, CONFIG_SYS_NAND_PAGE_SIZE, (void *) header);
	spl_parse_image_header(header);

	if (!spl_image.entry_point)
			spl_image.entry_point = spl_image.load_addr;

	switch (header->ih_comp) {
#ifdef CONFIG_SPL_LZMA
	case IH_COMP_LZMA:
		/* Load compressed u-boot to SDRAM first */
		header = (struct image_header *)(CONFIG_SYS_SDRAM_BASE);
		nand_spl_load_image(uboot_base,
			ALIGN(spl_image.size, CONFIG_SYS_NAND_PAGE_SIZE), (void *) header);

		/* Uncompress real U-Boot to its defined location in SDRAM */
		lzma_len = CONFIG_SYS_BOOTM_LEN;
		spl_image.load_addr += sizeof(struct image_header);

		ret = lzmaBuffToBuffDecompress((u8 *) spl_image.load_addr,
			&lzma_len,
			(u8 *) ((size_t) header + sizeof(struct image_header)),
			spl_image.size - sizeof(struct image_header));

		if (ret)
			printf("Error: LZMA decompression failed with %d\n", ret);

		printf("Actual U-Boot image size: 0x%x\n", lzma_len);

		spl_image.size = lzma_len;

		break;
#endif /* CONFIG_SPL_LZMA */
	case IH_COMP_NONE:
default:
		/* Load U-Boot to its defined location in SDRAM */
		nand_spl_load_image(uboot_base,
			spl_image.size, (void *)spl_image.load_addr);
	}

	nand_deselect();
}
