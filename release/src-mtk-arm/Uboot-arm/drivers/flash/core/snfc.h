/*
 * Copyright (c) 2012  MediaTek, Inc.
 * Author : Guochun.Mao@mediatek.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __SNFC_H__
#define __SNFC_H__

#include "../snfc_os.h"

/* Flash opcodes. */
#define NOR_OP_WREN		0x06	/* Write enable */
#define NOR_OP_RDSR		0x05	/* Read status register */
#define NOR_OP_WRSR		0x01	/* Write status register 1 byte */
#define NOR_OP_READ		0x03	/* Read data bytes (low frequency) */
#define NOR_OP_READ_FAST	0x0b	/* Read data bytes (high frequency) */
#define NOR_OP_READ_1_1_2	0x3b	/* Read data bytes (Dual SPI) */
#define NOR_OP_READ_1_1_4	0x6b	/* Read data bytes (Quad SPI) */
#define NOR_OP_READ_2IO		0xbb	/* Read data bytes (2xIO SPI, MXIC) */
#define NOR_OP_READ_4IO		0xeb	/* Read data bytes (4xIO SPI, MXIC) */
#define NOR_OP_PP		0x02	/* Page program (up to 256 bytes) */
#define NOR_OP_BE_4K		0x20	/* Erase 4KiB block */
#define NOR_OP_BE_4K_PMC	0xd7	/* Erase 4KiB block on PMC chips */
#define NOR_OP_BE_32K		0x52	/* Erase 32KiB block */
#define NOR_OP_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define NOR_OP_BE_64K		0xd8	/* Sector erase (usually 64KiB) */
#define NOR_OP_RDID		0x9f	/* Read JEDEC ID */
#define NOR_OP_REMS		0x90
#define NOR_OP_RDCR		0x35	/* Read configuration register */
#define NOR_OP_RDFSR		0x70	/* Read flag status register */
#define NOR_OP_RSTEN		0x66
#define NOR_OP_RST		0x99
#define NOR_OP_EN4B		0xb7	/* Enter 4-byte mode */
#define NOR_OP_EX4B		0xe9	/* Exit 4-byte mode */
#define NOR_OP_RDP		0xab	/* Release from deep power-down */
#define NOR_OP_ENDP		0xb9	/* Enter deep power-down */

#define NOR_OP_SUSPEND_MXIC	0xb0	/* MXIC flash suspend */
#define NOR_OP_RESUME_MXIC	0x30	/* MXIC flash resume */
#define NOR_OP_RDSCUR_MXIC	0x2b	/* Read security register */

#define NOR_OP_SUSPEND_WINBOND	0x75	/* Winbond flash suspend */
#define NOR_OP_RESUME_WINBOND	0x7A	/* Winbond flash resume */

/* 4-byte address opcodes - used on Spansion and some Macronix flashes. */
#define NOR_OP_READ4		0x13	/* Read data bytes (low frequency) */
#define NOR_OP_READ4_FAST	0x0c	/* Read data bytes (high frequency) */
#define NOR_OP_READ4_1_1_2	0x3c	/* Read data bytes (Dual SPI) */
#define NOR_OP_READ4_1_1_4	0x6c	/* Read data bytes (Quad SPI) */
#define NOR_OP_PP_4B		0x12	/* Page program (up to 256 bytes) */
#define NOR_OP_SE_4B		0xdc	/* Sector erase (usually 64KiB) */

/* Used for SST flashes only. */
#define NOR_OP_BP		0x02	/* Byte program */
#define NOR_OP_WRDI		0x04	/* Write disable */
/* Auto address increment word program */
#define NOR_OP_AAI_WP		0xad

/* Used for Macronix and Winbond flashes. */
#define NOR_OP_RDCR_MXIC	0x15

/* Used for Spansion flashes only. */
#define NOR_OP_BRWR		0x17	/* Bank register write */

/* Used for Micron flashes only. */
#define NOR_OP_RD_EVCR		0x85    /* Read EVCR register */
#define NOR_OP_WD_EVCR		0x81    /* Write EVCR register */

/* Status Register bits. */
#define SR_WIP			BIT(0) /* Write in progress */
#define SR_WEL			BIT(1) /* Write enable latch */

/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0			BIT(2) /* Block protect 0 */
#define SR_BP1			BIT(3) /* Block protect 1 */
#define SR_BP2			BIT(4) /* Block protect 2 */
/* Block protect 3(not all vendor has this bit)*/
#define SR_BP3			BIT(5)
#define SR_BP3_MT		BIT(6)

#define SR_SRWD			BIT(7) /* SR write protect */

#define SR_QUAD_EN_MX		BIT(6) /* Macronix Quad I/O */

/* Enhanced Volatile Configuration Register bits */
#define EVCR_QUAD_EN_MICRON	BIT(7) /* Micron Quad I/O */

/* Flag Status Register bits */
#define FSR_READY		BIT(7)

/* Configuration Register bits. */
#define CR_QUAD_EN_SPAN		BIT(1) /* Spansion Quad I/O */

#define NOR_JEDEC_ID_LEN	3 /*we only use 3 bytes, it's enough for us */

struct snfc_delsel {
	u8 sio_0_in; /*REG_SF_DELSEL0 bits 3:0*/
	u8 sio_0_out; /*REG_SF_DELSEL2 bits 3:0*/
	u8 sio_1_in; /*REG_SF_DELSEL2 bits 7:4*/
	u8 sio_1_out; /*REG_SF_DELSEL1 bits 3:0*/
	u8 sio_2_in; /*REG_SF_DELSEL3 bits 7:4*/
	u8 sio_2_out; /*REG_SF_DELSEL3 bits 3:0*/
	u8 sio_3_in; /*REG_SF_DELSEL4 bits 7:4*/
	u8 sio_3_out; /*REG_SF_DELSEL4 bits 3:0*/
	u8 sck_out; /*REG_SF_DELSEL0 bits 7:4*/
};

enum {
	/* Read protocol mode */
	SF_SINGLE_READ = 0,
	SF_FAST_READ   = 1,
	SF_1_1_2_READ  = 2,
	SF_2XIO_READ   = 3,
	SF_1_1_4_READ  = 4,
	SF_4XIO_READ   = 5,

	/* Read controller path */
	SF_PIO_PATH    = 0,
	SF_DIRECT_PATH = 1,
	SF_DMA_PATH    = 2,

	/* Write controller mode */
	SF_BYTE_WRITE       = 0,
	SF_PAGE_WRITE       = 1,
	SF_MULTI_PAGE_WRITE = 2,

	/* Erase protocol mode */
	SF_4K_ERASE     = 0,
	SF_32K_ERASE    = 1,
	SF_64K_ERASE    = 2,
	SF_CHIP_ERASE   = 3,

	/* Addr width */
	SF_ADDR_WIDTH_3B = 3,
	SF_ADDR_WIDTH_4B = 4,
};

#define NOR_MF_ATMEL		0x1f
#define NOR_MF_ESMT		0x1c
#define NOR_MF_XTX		0x0B
#define NOR_MF_INTEL		0x89
#define NOR_MF_MICRON		0x20
#define NOR_MF_MACRONIX		0xc2
#define NOR_MF_ISSI		0x9d
#define NOR_MF_WINBOND		0xef
#define NOR_MF_SPANSION		0x01
#define NOR_MF_SST		0xbf
#define NOR_MF_GIGADEVICE	0xc8

struct snfc_handler {
	const struct spi_nor *nor;
	const struct snfc_ops *ops;
	struct snfc_property *sp;
};

struct spi_nor {
	const char *name;
	u8 manufact;
	u8 mem_type;
	u8 mem_density;
	u32 page_size;
	u32 sect_size;
	u32 chip_size;
	u32 read_cap;
};

struct snfc_property {
	unsigned long reg_base;
	unsigned long mem_base;
	int buf_size;
	int addr_width;
	int read_mode;
	int read_path;
	int erase_mode;
	int write_mode;
};

struct snfc_ops {
	int (*read_reg)(struct snfc_handler *sh, u8 opcode, u8 *buf, int len);
	int (*write_reg)(struct snfc_handler *sh, u8 opcode, u8 *buf, int len);

	int (*set_delsel)(struct snfc_handler *sh, struct snfc_delsel *ds);
	int (*set_irq_enable)(struct snfc_handler *sh, u32 value);
	int (*get_irq_status)(struct snfc_handler *sh, u32 *status);
	int (*clr_irq_status)(struct snfc_handler *sh);

	int (*set_addr_width)(struct snfc_handler *sh, int addr_width);
	int (*set_read_path)(struct snfc_handler *sh, int read_path);
	int (*set_read_mode)(struct snfc_handler *sh, int read_mode);
	int (*set_erase_mode)(struct snfc_handler *sh, int erase_mode);
	int (*set_write_mode)(struct snfc_handler *sh, int write_mode);

	int (*read)(struct snfc_handler *sh, u32 addr, u8 *buf, u32 len);
	int (*write)(struct snfc_handler *sh, u32 addr, const u8 *buf, u32 len);
	int (*erase)(struct snfc_handler *sh, u32 addr, u32 len);

	int (*nor_reset)(struct snfc_handler *sh);
	int (*nor_write_protect)(struct snfc_handler *sh, bool enable);
	int (*nor_get_status)(struct snfc_handler *sh, u8 *status);
	int (*nor_get_config)(struct snfc_handler *sh, u8 *config);
	int (*wait_nor_ready)(struct snfc_handler *sh, u32 tmo, u32 interval);
};

struct snfc_handler *snfc_nor_init(struct snfc_property *sp);

#endif /* __SNFC_H__ */
