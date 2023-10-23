/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Author: Leonid Yegoshin (yegoshin@mips.com)
 * Copyright (C) 2012 MIPS Technologies, Inc.
 * Copyright (C) 2014 Lei Chuanhua <Chuanhua.lei@lantiq.com>
 * Copyright (C) 2017 Intel Corporation.
 */

#ifndef _ASM_LANTIQ_SPACES_H
#define _ASM_LANTIQ_SPACES_H

#include <linux/sizes.h>

#ifdef CONFIG_SOC_GRX500
#ifdef CONFIG_EVA

#ifdef CONFIG_LTQ_EVA_2GB

#define MIPS_RESERVED_MEM_SIZE	0

#define PHYS_START		_AC(0x20000000, UL)
#define VIRT_START		_AC(0x20000000, UL)

#define PAGE_OFFSET		(VIRT_START + MIPS_RESERVED_MEM_SIZE)
#define PHYS_OFFSET		(PHYS_START + MIPS_RESERVED_MEM_SIZE)

/* No Highmem Support */
#define HIGHMEM_START		_AC(0xffff0000, UL)

#define UNCAC_BASE		(_AC(0xa0000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define CAC_BASE		(_AC(0x20000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define IO_BASE			UNCAC_BASE /* Must be the same */

#define KSEG
#define KUSEG			0x00000000
#define KSEG0			0x20000000
#define KSEG1			0xa0000000
#define KSEG2			0xc0000000
#define KSEG3			0xe0000000

#define CKUSEG			0x00000000
#define CKSEG0			0x20000000
#define CKSEG1			0xa0000000
#define CKSEG2			_AC(0xc0000000, UL)
#define CKSEG3			0xe0000000

/* Range from 0x2000.0000 ~ 0x9FFF.FFFF for KSEG0 */
#define KSEGX(a)		(((_ACAST32_(a)) < 0xA0000000) ?	\
					KSEG0 : ((_ACAST32_(a)) & 0xE0000000))

#define INDEX_BASE		CKSEG0
#define MAP_BASE		CKSEG2
#define VMALLOC_END		(MAP_BASE + _AC(0x20000000, UL) - 2 * PAGE_SIZE)

#define IO_SIZE			_AC(0x10000000 - MIPS_RESERVED_MEM_SIZE, UL)
#define IO_SHIFT		_AC(0x10000000, UL)
#ifdef CONFIG_DMA_COHERENT
#define CPHYSADDR(a)		((_ACAST32_(a))  - PAGE_OFFSET +	\
				 PHYS_OFFSET + 0xA0000000)
#else
#define CPHYSADDR(a)		((_ACAST32_(a)) - PAGE_OFFSET + PHYS_OFFSET)
#endif
#define RPHYSADDR(a)		((_ACAST32_(a)) & 0x1fffffff)

#define LEGACY_KSEC0(a)		(RPHYSADDR(a) | 0x80000000)
#define LEGACY_TO_VADDR(a)      ((a) - 0x80000000 + PAGE_OFFSET)

/* DRAM one */
#define CKSEG0ADDR(a)		((_ACAST32_(a)))

/* IO space one */
#define CKSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)

/*
 * Map an address to a certain kernel segment
 */
/* DRAM one */
#define KSEG0ADDR(a)		(CKSEG0ADDR(a))

/* IO space one */
#define KSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)
#define __pa_symbol(x)		__pa(x)

#elif defined(CONFIG_LTQ_EVA_1GB)
#define MIPS_RESERVED_MEM_SIZE	0

#define PHYS_START		_AC(0x20000000, UL)
#define VIRT_START		_AC(0x60000000, UL)

#define PAGE_OFFSET		(VIRT_START + MIPS_RESERVED_MEM_SIZE)
#define PHYS_OFFSET		(PHYS_START + MIPS_RESERVED_MEM_SIZE)

/* No Highmem Support */
#define HIGHMEM_START		_AC(0xffff0000, UL)

#define UNCAC_BASE		(_AC(0xa0000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define CAC_BASE		(_AC(0x60000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define IO_BASE			UNCAC_BASE /* Must be the same */

#define KSEG
#define KUSEG			0x00000000
#define KSEG0			0x60000000
#define KSEG1			0xa0000000
#define KSEG2			0xc0000000
#define KSEG3			0xe0000000

#define CKUSEG			0x00000000
#define CKSEG0			0x60000000
#define CKSEG1			0xa0000000
#define CKSEG2			_AC(0xc0000000, UL)
#define CKSEG3			0xe0000000

/* Range from 0x6000.0000 ~ 0x9FFF.FFFF for KSEG0 */
#define KSEGX(a)		(((_ACAST32_(a)) < 0xA0000000) ?	\
					KSEG0 : ((_ACAST32_(a)) & 0xE0000000))

#define INDEX_BASE		CKSEG0
#define MAP_BASE		CKSEG2
#define VMALLOC_END		(MAP_BASE + _AC(0x20000000, UL) - 2 * PAGE_SIZE)

#define IO_SIZE			_AC(0x10000000 - MIPS_RESERVED_MEM_SIZE, UL)
#define IO_SHIFT		_AC(0x10000000, UL)
#ifdef CONFIG_DMA_COHERENT
#define CPHYSADDR(a)		((_ACAST32_(a))  - PAGE_OFFSET +	\
				 PHYS_OFFSET + 0xA0000000) /* 1GB limitation */
#else
#define CPHYSADDR(a)		((_ACAST32_(a)) - PAGE_OFFSET + PHYS_OFFSET)
#endif
#define RPHYSADDR(a)		((_ACAST32_(a)) & 0x1fffffff)

#define LEGACY_KSEC0(a)		(RPHYSADDR(a) | 0x80000000)
#define LEGACY_TO_VADDR(a)      ((a) - 0x80000000 + PAGE_OFFSET)

/* DRAM one */
#define CKSEG0ADDR(a)		((_ACAST32_(a)))

/* IO space one */
#define CKSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)

/*
 * Map an address to a certain kernel segment
 */
/* DRAM one */
#define KSEG0ADDR(a)		(CKSEG0ADDR(a))

/* IO space one */
#define KSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)
#define __pa_symbol(x)		__pa(x)

#elif defined(CONFIG_LTQ_EVA_LEGACY)

#define MIPS_RESERVED_MEM_SIZE	0

#define PHYS_START		_AC(0x20000000, UL)
#define VIRT_START		_AC(0x80000000, UL)

#define PAGE_OFFSET		(VIRT_START + MIPS_RESERVED_MEM_SIZE)
#define PHYS_OFFSET		(PHYS_START + MIPS_RESERVED_MEM_SIZE)

/* No Highmem Support */
#define HIGHMEM_START		_AC(0xffff0000, UL)

#define UNCAC_BASE		(_AC(0xa0000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define CAC_BASE		(_AC(0x80000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define IO_BASE			UNCAC_BASE

#define KSEG
#define KUSEG			0x00000000
#define KSEG0			0x80000000
#define KSEG1			0xa0000000
#define KSEG2			0xc0000000
#define KSEG3			0xe0000000

#define CKUSEG			0x00000000
#define CKSEG0			0x80000000
#define CKSEG1			0xa0000000
#define CKSEG2			_AC(0xc0000000, UL)
#define CKSEG3			0xe0000000

#define KSEGX(a)		((_ACAST32_(a)) & 0xe0000000)

#define INDEX_BASE		CKSEG0
#define MAP_BASE		CKSEG2
#define VMALLOC_END		(MAP_BASE + _AC(0x20000000, UL) - 2 * PAGE_SIZE)

#define IO_SIZE			_AC(0x10000000, UL)
#define IO_SHIFT		_AC(0x10000000, UL)
#define CPHYSADDR(a)		((_ACAST32_(a)) - PAGE_OFFSET + PHYS_OFFSET)
#define RPHYSADDR(a)		((_ACAST32_(a)) & 0x1fffffff)
#define LEGACY_KSEC0(a)		(RPHYSADDR(a) | KSEG0)
#define LEGACY_TO_VADDR(a)      ((a) - 0x80000000 + PAGE_OFFSET)

/* DRAM one */
#define CKSEG0ADDR(a)		((_ACAST32_(a)))

/* IO space one */
#define CKSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)

/*
 * Map an address to a certain kernel segment
 */
/* DRAM one */
#define KSEG0ADDR(a)		(CKSEG0ADDR(a))

/* IO space one */
#define KSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)
#define __pa_symbol(x)		__pa(x)
#else
#error "Wrong EVA mode chosen"
#endif

#else /* Legacy  */

#define MIPS_RESERVED_MEM_SIZE	0

#define PHYS_START		_AC(0x20000000, UL)
#define VIRT_START		_AC(0x80000000, UL)

#define PAGE_OFFSET		(VIRT_START + MIPS_RESERVED_MEM_SIZE)
#define PHYS_OFFSET		(PHYS_START + MIPS_RESERVED_MEM_SIZE)

/* No Highmem Support */
#define HIGHMEM_START		_AC(0xffff0000, UL)

#define UNCAC_BASE		(_AC(0xa0000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define CAC_BASE		(_AC(0x80000000, UL) + MIPS_RESERVED_MEM_SIZE)

#define IO_BASE			UNCAC_BASE

#define KSEG
#define KUSEG			0x00000000
#define KSEG0			0x80000000
#define KSEG1			0xa0000000
#define KSEG2			0xc0000000
#define KSEG3			0xe0000000

#define CKUSEG			0x00000000
#define CKSEG0			0x80000000
#define CKSEG1			0xa0000000
#define CKSEG2			_AC(0xc0000000, UL)
#define CKSEG3			0xe0000000

#define KSEGX(a)		((_ACAST32_(a)) & 0xe0000000)

#define INDEX_BASE		CKSEG0
#define MAP_BASE		CKSEG2
#define VMALLOC_END		(MAP_BASE + _AC(0x20000000, UL) - 2 * PAGE_SIZE)

#define IO_SIZE			_AC(0x10000000, UL)
#define IO_SHIFT		_AC(0x10000000, UL)
#define CPHYSADDR(a)		((_ACAST32_(a)) - PAGE_OFFSET + PHYS_OFFSET)
#define RPHYSADDR(a)		((_ACAST32_(a)) & 0x1fffffff)
#define LEGACY_KSEC0(a)		(RPHYSADDR(a) | KSEG0)
#define LEGACY_TO_VADDR(a)      ((a) - 0x80000000 + PAGE_OFFSET)

/* DRAM one */
#define CKSEG0ADDR(a)		((_ACAST32_(a)))

/* IO space one */
#define CKSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)

/*
 * Map an address to a certain kernel segment
 */
/* DRAM one */
#define KSEG0ADDR(a)		(CKSEG0ADDR(a))

/* IO space one */
#define KSEG1ADDR(a)		(RPHYSADDR(a) | KSEG1)
#define __pa_symbol(x)		__pa(x)
#endif /* CONFIG_EVA */
#endif /* CONFIG_SOC_GRX500 */

#ifdef CONFIG_SOC_TYPE_GRX500_TEP

/* skip first 128 MB DDR */
#if defined(CONFIG_SOC_PRX300_BOOTCORE) && defined(CONFIG_USE_EMULATOR)
#define MIPS_RESERVED_MEM_SIZE	_AC(0x06000000, UL)
#else
#define MIPS_RESERVED_MEM_SIZE	_AC(0x08000000, UL)
#endif

#define PHYS_START		_AC(0x00000000, UL)
#define VIRT_START		_AC(0x80000000, UL)

#define PAGE_OFFSET		(VIRT_START + MIPS_RESERVED_MEM_SIZE)
#define PHYS_OFFSET		(PHYS_START + MIPS_RESERVED_MEM_SIZE)

/* No Highmem Support */
#define HIGHMEM_START		_AC(0xffff0000, UL)

#define UNCAC_BASE		(_AC(0xa0000000, UL) + MIPS_RESERVED_MEM_SIZE)
#define CAC_BASE		(_AC(0x80000000, UL) + MIPS_RESERVED_MEM_SIZE)

#define IO_BASE			UNCAC_BASE

#endif /* CONFIG_SOC_TYPE_GRX500_TEP */

#include <asm/mach-generic/spaces.h>

#endif /* __ASM_MALTA_SPACES_H */
