/*
 * Copyright (C) 2012 Stefan Roese <sr@denx.de>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <spl.h>
#include <lzma/LzmaTypes.h>
#include <lzma/LzmaDec.h>
#include <lzma/LzmaTools.h>

size_t __spl_nor_uboot_base(void) { return CONFIG_SYS_UBOOT_BASE; }

size_t spl_nor_uboot_base(void)
	__attribute__((weak, alias("__spl_nor_uboot_base")));

size_t __spl_nor_uboot_max_size(void) { return 0; }

size_t spl_nor_uboot_max_size(void)
	__attribute__((weak, alias("__spl_nor_uboot_max_size")));

size_t __spl_nor_uboot_alignment(void) { return 4; }

size_t spl_nor_uboot_alignment(void)
	__attribute__((weak, alias("__spl_nor_uboot_alignment")));

static size_t spl_nor_search_uboot_image(void)
{
	size_t search_base = spl_nor_uboot_base();
	size_t max_search_size = spl_nor_uboot_max_size();
	size_t alignment = spl_nor_uboot_alignment();
	size_t search_end;
	struct image_header hdr;

	if (!max_search_size)
		return search_base;

	if (!alignment)
		alignment = 4;

	search_base = ALIGN(search_base, alignment);
	search_end = ALIGN(search_base + max_search_size, alignment);

	while (search_base < search_end)
	{
		memcpy(&hdr, (void *) search_base, sizeof(hdr));

		if (image_get_magic(&hdr) == IH_MAGIC)
		{
			if (image_get_os(&hdr) == IH_OS_U_BOOT)
				return search_base;
		}

		search_base += alignment;
	}

	return spl_nor_uboot_base();
}

void spl_nor_load_image(void)
{
	/*
	 * Loading of the payload to SDRAM is done with skipping of
	 * the mkimage header in this SPL NOR driver
	 */
	spl_image.flags |= SPL_COPY_PAYLOAD_ONLY;

	if (spl_start_uboot()) {
		size_t uboot_base = spl_nor_search_uboot_image();
		struct image_header hdr;
		SizeT lzma_len;
		int ret;

		memcpy(&hdr, (void *) uboot_base, sizeof (hdr));

		/*
		 * Load real U-Boot from its location in NOR flash to its
		 * defined location in SDRAM
		 */
		spl_parse_image_header(&hdr);

		if (!spl_image.entry_point)
			spl_image.entry_point = spl_image.load_addr;

		switch (hdr.ih_comp) {
#ifdef CONFIG_SPL_LZMA
		case IH_COMP_LZMA:
			/*
			 * Uncompress real U-Boot to its defined location in SDRAM
			 */
			lzma_len = CONFIG_SYS_BOOTM_LEN;

			ret = lzmaBuffToBuffDecompress((u8 *) spl_image.load_addr,
				&lzma_len,
				(u8 *) (uboot_base + sizeof(struct image_header)),
				spl_image.size);

			if (ret)
				printf("Error: LZMA decompression for failed with %d\n", ret);

			printf("Actual U-Boot image size: 0x%x\n", lzma_len);

			spl_image.size = lzma_len;

			break;
#endif /* CONFIG_SPL_LZMA */
		case IH_COMP_NONE:
		default:
			/*
			 * Directly copy U-Boot to its defined location in SDRAM
			 */

			memmove((void *) (unsigned long) spl_image.load_addr,
				(void *) (uboot_base + sizeof(struct image_header)),
				spl_image.size);
		}
	} else {
		/*
		 * Load Linux from its location in NOR flash to its defined
		 * location in SDRAM
		 */
		spl_parse_image_header(
			(const struct image_header *)CONFIG_SYS_OS_BASE);

		memcpy((void *)spl_image.load_addr,
		       (void *)(CONFIG_SYS_OS_BASE +
				sizeof(struct image_header)),
		       spl_image.size);

		/*
		 * Copy DT blob (fdt) to SDRAM. Passing pointer to flash
		 * doesn't work (16 KiB should be enough for DT)
		 */
		memcpy((void *)CONFIG_SYS_SPL_ARGS_ADDR,
		       (void *)(CONFIG_SYS_FDT_BASE),
		       (16 << 10));
	}
}
