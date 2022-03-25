/*
 * Copyright (c) 2012  MediaTek, Inc.
 * Author : Guochun.Mao@mediatek.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "snfc.h"

/* Serial NOF Flash Controller registers */
#define REG_SF_CMD		0x00
#define REG_SF_CNT		0x04
#define REG_SF_RDSR		0x08
#define REG_SF_RDATA		0x0C
#define REG_SF_RADR0		0x10
#define REG_SF_RADR1		0x14
#define REG_SF_RADR2		0x18
#define REG_SF_WDATA		0x1C
#define REG_SF_PRGDATA0		0x20
#define REG_SF_PRGDATA1		0x24
#define REG_SF_PRGDATA2		0x28
#define REG_SF_PRGDATA3		0x2C
#define REG_SF_PRGDATA4		0x30
#define REG_SF_PRGDATA5		0x34
#define REG_SF_SHREG0		0x38
#define REG_SF_SHREG1		0x3C
#define REG_SF_SHREG2		0x40
#define REG_SF_SHREG3		0x44
#define REG_SF_SHREG4		0x48
#define REG_SF_SHREG5		0x4C
#define REG_SF_SHREG6		0x50
#define REG_SF_SHREG7		0x54
#define REG_SF_SHREG8		0x58
#define REG_SF_SHREG9		0x5C
#define REG_SF_CFG1		0x60
#define REG_SF_CFG2		0x64
#define REG_SF_SHREG10		0x68
#define REG_SF_STATUS0		0x70
#define RREG_FLASH_TIME		0x80
#define RREG_FLASH_CFG		0x84
#define REG_SF_PP_DW_DATA	0x98
#define REG_SF_DELSEL0		0xA0
#define REG_SF_DELSEL1		0xA4
#define REG_SF_INTRSTUS		0xA8
#define REG_SF_INTREN		0xAC
#define REG_SF_AAICMD		0xC0
#define REG_SF_WRPROT		0xC4
#define REG_SF_RADR3		0xC8
#define REG_SF_DUAL		0xCC
#define REG_SF_DELSEL2		0xD0
#define REG_SF_DELSEL3		0xD4
#define REG_SF_DELSEL4		0xD8
#define REG_FDMA_CTL		0x718
#define REG_FDMA_FADR		0x71C
#define REG_FDMA_DADR		0x720
#define REG_FDMA_END_DADR	0x724
#define REG_CG_DIS		0x728
#define REG_SF_PAGECNT		0x72C
#define REG_SF_PAGESIZE		0x730
#define REG_MPP_EN		0x734
#define REG_FDMA_DADR_HB	0x738
#define REG_FDMA_END_DA		0x73C

/* Helpers for accessing the program data / shift data registers */
#define REG_SF_PRGDATA(n)	(REG_SF_PRGDATA0 + 4 * (n))
#define REG_SF_SHREG(n)		(REG_SF_SHREG0 + 4 * (n))

/* Command types which defined in REG_SF_CMD */
#define SF_CMD_RD		0x1
#define SF_CMD_RDSR		0x2
#define SF_CMD_PRG		0x4
#define SF_CMD_ERASE		0x8
#define SF_CMD_WR		0x10
#define SF_CMD_WRSR		0x20
#define SF_CMD_AUTOINC		0x80

/* control bit in REG_SF_DUAL */
#define SF_READ_MODE_MASK	0x0f
#define SF_LARGE_ADDR_EN	0x10
#define SF_READ_ADDR_QUAD	0x08
#define SF_READ_QUAD_EN		0x04
#define SF_READ_ADDR_DUAL	0x02
#define SF_READ_DUAL_EN		0x01

/* control valued defined in REG_SF_WRPROT */
#define SF_WP_TURN_OFF		0x30

/* control value defined in REG_SF_CFG2 */
#define SF_BUF_WRITE_EN		0x1
#define SF_BUF_WRITE_DIS	0x0

/* control value defined in REG_SF_CFG1 */
#define SF_FAST_READ_EN		0x1
#define SF_FAST_READ_DIS	0x0

/*control value defined in SF_FDMA_CTL */
#define SF_FDMA_CTL_CLEAR	0x0
#define SF_FDMA_CTL_DMA_TRIG	0x1
#define SF_FDMA_CTL_RST		0x2
#define SF_FDMA_CTL_WDLE_EN	0x4

/* Can shift up to 48 bits (6 bytes) of TX/RX */
#define SF_MAX_RX_TX_SHIFT	6
/* can shift up to 56 bits (7 bytes) transfer by PRG_CMD */
#define SF_MAX_SHIFT		7

#define ERASE_TIMEOUT		(500 * 1000 * 1000)
#define INTERVAL_100US		100
#define WAIT_READY_TIMEOUT	(500 * 1000 * 1000)
#define INTERVAL_10US		10
#define ACCESS_REG_TIMEOUT	(500 * 1000 * 1000)
#define INTERVAL_1US		1

#define ALIGNED_16(x)	(!((x) & (u32)(0x10 - 1)))
#define ALIGNED_4K(x)	(!((x) & (u32)(0x1000 - 1)))
#define ALIGNED_32K(x)	(!((x) & (u32)(0x8000 - 1)))
#define ALIGNED_64K(x)	(!((x) & (u32)(0x10000 - 1)))

/* interval should not be 0, and smaller than tmo */
#define poll_cond_tmo(condition, tmo, interval)\
	({\
		int __ret = ERR_BUSY;\
		u32 __tmo = tmo;\
		u32 __interval = interval;\
		if (__interval)\
			__tmo /= __interval;\
		while (__tmo--) {\
			if (condition) {\
				__ret = 0;\
				break;\
			} \
			udelay(__interval);\
		} \
		if (__ret) {\
			snfc_loge(">>NOTICE: polling condition time out.\n");\
			snfc_loge("          time out: %u\n", tmo);\
		} \
		__ret;\
	})

#define RD_SPT_ALL (BIT(SF_SINGLE_READ) | BIT(SF_FAST_READ) |\
		    BIT(SF_1_1_2_READ) | BIT(SF_2XIO_READ) |\
		    BIT(SF_1_1_4_READ) | BIT(SF_4XIO_READ))

#define RD_SPT_1_1_X (BIT(SF_SINGLE_READ) | BIT(SF_FAST_READ) |\
		      BIT(SF_1_1_2_READ) | BIT(SF_1_1_4_READ))

static const struct spi_nor nor_tbl[] = {
	{"S25FS064",  0x01, 0x02, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"S25FL004A", 0x01, 0x02, 0x12, 0x100, 0x1000, 0x80000,   RD_SPT_ALL},
	{"S25FL08A",  0x01, 0x02, 0x13, 0x100, 0x1000, 0x100000,  RD_SPT_ALL},
	{"S25FL016A", 0x01, 0x02, 0x14, 0x100, 0x1000, 0x200000,  RD_SPT_ALL},
	{"S25FL032A", 0x01, 0x02, 0x15, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"S25FL064A", 0x01, 0x02, 0x16, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"S25FL128P", 0x01, 0x20, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},
	{"S25FL216K", 0x01, 0x40, 0x15, 0x100, 0x1000, 0x200000,  RD_SPT_ALL},
	{"S25FL256S", 0x01, 0x02, 0x19, 0x100, 0x1000, 0x2000000, RD_SPT_ALL},

	{"MX25U256",   0xC2, 0x25, 0x39, 0x100, 0x1000, 0x2000000,  RD_SPT_ALL},
	{"MX66U2G",    0xC2, 0x25, 0x3C, 0x100, 0x1000, 0x10000000, RD_SPT_ALL},
	{"M25Q256",    0x20, 0xBA, 0x19, 0x100, 0x1000, 0x2000000,  RD_SPT_ALL},
	{"MX25L400",   0xC2, 0x20, 0x13, 0x100, 0x1000, 0x80000,    RD_SPT_ALL},
	{"MX25L800",   0xC2, 0x20, 0x14, 0x100, 0x1000, 0x100000,   RD_SPT_ALL},
	{"MX25L160",   0xC2, 0x20, 0x15, 0x100, 0x1000, 0x200000,   RD_SPT_ALL},
	{"MX25L1635D", 0xC2, 0x24, 0x15, 0x100, 0x1000, 0x200000,   RD_SPT_ALL},
	{"MX25L320",   0xC2, 0x20, 0x16, 0x100, 0x1000, 0x400000,   RD_SPT_ALL},
	{"MX25L640",   0xC2, 0x20, 0x17, 0x100, 0x1000, 0x800000,   RD_SPT_ALL},
	{"MX25L128",   0xC2, 0x20, 0x18, 0x100, 0x1000, 0x1000000,  RD_SPT_ALL},
	{"MX25L3235D", 0xC2, 0x5E, 0x16, 0x100, 0x1000, 0x400000,   RD_SPT_ALL},
	{"MX25L256",   0xC2, 0x20, 0x19, 0x100, 0x1000, 0x2000000,  RD_SPT_ALL},
	{"MX25L512",   0xC2, 0x20, 0x1a, 0x100, 0x1000, 0x4000000,  RD_SPT_ALL},
	{"MX25L1G",    0xC2, 0x20, 0x1b, 0x100, 0x1000, 0x8000000,  RD_SPT_ALL},

	{"GD25Q32",  0xC8, 0x40, 0x16, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"GD25Q64",  0xC8, 0x40, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"GD25L127", 0xC8, 0x40, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},
	{"GD25Q256", 0xC8, 0x40, 0x19, 0x100, 0x1000, 0x2000000, RD_SPT_ALL},
	{"GD25Q512", 0xC8, 0x40, 0x20, 0x100, 0x1000, 0x4000000, RD_SPT_ALL},

	{"M25P80",   0x20, 0x20, 0x14, 0x100, 0x1000, 0x100000,  RD_SPT_ALL},
	{"M25P16",   0x20, 0x20, 0x15, 0x100, 0x1000, 0x200000,  RD_SPT_ALL},
	{"M25P32",   0x20, 0x20, 0x16, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"M25P64",   0x20, 0x20, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"M25P128",  0x20, 0x20, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},
	{"M25PX32",  0x20, 0x71, 0x16, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"M25PX64",  0x20, 0x71, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"N25Q064A", 0x20, 0xBA, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"MT25Q128", 0x20, 0xBA, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},
	{"N25Q256",  0x20, 0xBA, 0x19, 0x100, 0x1000, 0x2000000, RD_SPT_ALL},

	{"W25X40", 0xEF, 0x30, 0x13, 0x100, 0x1000, 0x80000,  RD_SPT_ALL},
	{"W25X80", 0xEF, 0x30, 0x14, 0x100, 0x1000, 0x100000, RD_SPT_ALL},
	{"W25X16", 0xEF, 0x30, 0x15, 0x100, 0x1000, 0x200000, RD_SPT_ALL},
	{"W25X32", 0xEF, 0x30, 0x16, 0x100, 0x1000, 0x400000, RD_SPT_ALL},
	{"W25X64", 0xEF, 0x30, 0x17, 0x100, 0x1000, 0x800000, RD_SPT_ALL},

	{"W25Q128FW", 0xEF, 0x60, 0x18, 0x100, 0x1000, 0x1000000,  RD_SPT_ALL},
	{"W25Q16FV",  0xEF, 0x40, 0x15, 0x100, 0x1000, 0x200000,   RD_SPT_ALL},
	{"W25Q32BV",  0xEF, 0x40, 0x16, 0x100, 0x1000, 0x400000,   RD_SPT_ALL},
	{"W25Q64FV",  0xEF, 0x40, 0x17, 0x100, 0x1000, 0x800000,   RD_SPT_ALL},
	{"W25Q128BV", 0xEF, 0x40, 0x18, 0x100, 0x1000, 0x1000000,  RD_SPT_ALL},
	{"W25Q256",   0xEF, 0x40, 0x19, 0x100, 0x1000, 0x2000000,  RD_SPT_ALL},
	{"W25Q512",   0xEF, 0x40, 0x20, 0x100, 0x1000, 0x4000000,  RD_SPT_ALL},

	{"SST25VF020",  0xBF, 0x43, 0x10, 0x100, 0x1000, 0x40000,  RD_SPT_ALL},
	{"SST25VF040B", 0xBF, 0x25, 0x8D, 0x100, 0x1000, 0x80000,  RD_SPT_ALL},
	{"SST25VF080B", 0xBF, 0x25, 0x8E, 0x100, 0x1000, 0x100000, RD_SPT_ALL},
	{"SST25VF016B", 0xBF, 0x25, 0x41, 0x100, 0x1000, 0x200000, RD_SPT_ALL},
	{"SST25VF032B", 0xBF, 0x25, 0x4A, 0x100, 0x1000, 0x400000, RD_SPT_ALL},
	{"SST25VF064C", 0xBF, 0x25, 0x4B, 0x100, 0x1000, 0x800000, RD_SPT_ALL},

	{"AT25DF321", 0x1F, 0x47, 0x00, 0x100, 0x1000, 0x400000, RD_SPT_ALL},
	{"AT25DF641", 0x1F, 0x48, 0x00, 0x100, 0x1000, 0x800000, RD_SPT_ALL},

	{"EN25S80A",  0x1C, 0x38, 0x14, 0x100, 0x1000, 0x100000,  RD_SPT_ALL},
	{"EN25B40",   0x1C, 0x20, 0x13, 0x100, 0x1000, 0x80000,   RD_SPT_ALL},
	{"EN25F80",   0x1C, 0x31, 0x14, 0x100, 0x1000, 0x100000,  RD_SPT_ALL},
	{"EN25P16",   0x1C, 0x20, 0x15, 0x100, 0x1000, 0x200000,  RD_SPT_ALL},
	{"EN25NEW",   0x1C, 0x70, 0x15, 0x100, 0x1000, 0x200000,  RD_SPT_ALL},
	{"EN25QH32",  0x1C, 0x70, 0x16, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"EN25QH64",  0x1C, 0x70, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"EN25QH128", 0x1C, 0x70, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},
	{"EN25P32",   0x1C, 0x20, 0x16, 0x100, 0x1000, 0x400000,  RD_SPT_ALL},
	{"EN25P64",   0x1C, 0x20, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"EN25Q64",   0x1C, 0x30, 0x17, 0x100, 0x1000, 0x800000,  RD_SPT_ALL},
	{"EN25P128",  0x1C, 0x30, 0x18, 0x100, 0x1000, 0x1000000, RD_SPT_ALL},

	{"A25L40P", 0x7F, 0x37, 0x20, 0x100, 0x1000, 0x200000, RD_SPT_ALL},
	{"A25L040", 0x37, 0x30, 0x13, 0x100, 0x1000, 0x100000, RD_SPT_ALL},
	{"A25L032", 0x37, 0x30, 0x16, 0x100, 0x1000, 0x400000, RD_SPT_ALL},

	{"XT25F64B", 0x0B, 0x40, 0x17, 0x100, 0x1000, 0x800000, RD_SPT_ALL},
};

static const struct spi_nor *get_nor_over_tbl(const u8 *id)
{
	int i;
	int array_size = sizeof(nor_tbl) / sizeof(struct spi_nor);

	for (i = 0; i < array_size; i++) {
		if (nor_tbl[i].manufact == id[0] &&
		    nor_tbl[i].mem_type == id[1] &&
		    nor_tbl[i].mem_density == id[2])
			return &nor_tbl[i];
	}

	return NULL;
}

/*
 * REG_SF_CMD[0:5] These bits will be auto-cleared when done,
 * so we should check these bits after written it
 */
static int execute_cmd(struct snfc_handler *sh, u8 cmd)
{
	u8 val = cmd & ~SF_CMD_AUTOINC;

	cmd |= READB(sh->sp->reg_base + REG_SF_CMD);

	WRITEB(cmd, sh->sp->reg_base + REG_SF_CMD);
	return poll_cond_tmo(!(READB(sh->sp->reg_base + REG_SF_CMD) & val),
		ACCESS_REG_TIMEOUT, INTERVAL_1US);
}

static int do_tx_rx(struct snfc_handler *sh, u8 op,
		    u8 *tx, int txlen, u8 *rx, int rxlen)
{
	int len = 1 + txlen + rxlen;
	int i, ret, idx;

	if (len > SF_MAX_SHIFT)
		return ERR_INVAL;

	WRITEB(len * 8, sh->sp->reg_base + REG_SF_CNT);

	/* start at PRGDATA5, go down to PRGDATA0 */
	idx = SF_MAX_RX_TX_SHIFT - 1;

	/* opcode */
	WRITEB(op, sh->sp->reg_base + REG_SF_PRGDATA(idx));
	idx--;

	/* program TX data */
	for (i = 0; i < txlen; i++, idx--)
		WRITEB(tx[i], sh->sp->reg_base + REG_SF_PRGDATA(idx));

	/* clear out rest of TX registers */
	while (idx >= 0) {
		WRITEB(0, sh->sp->reg_base + REG_SF_PRGDATA(idx));
		idx--;
	}

	ret = execute_cmd(sh, SF_CMD_PRG);
	if (ret)
		return ret;

	/* restart at first RX byte */
	idx = rxlen - 1;

	/* read out RX data */
	for (i = 0; i < rxlen; i++, idx--)
		rx[i] = READB(sh->sp->reg_base + REG_SF_SHREG(idx));

	return 0;
}

static int send_opcode(struct snfc_handler *sh, u8 op)
{
	return do_tx_rx(sh, op, NULL, 0, NULL, 0);
}

static int nor_write_enable(struct snfc_handler *sh)
{
	return send_opcode(sh, NOR_OP_WREN);
}

static int read_reg(struct snfc_handler *sh, u8 op, u8 *buf, int len)
{
	return do_tx_rx(sh, op, NULL, 0, buf, len);
}

static int write_reg(struct snfc_handler *sh, u8 op, u8 *buf, int len)
{
	return do_tx_rx(sh, op, buf, len, NULL, 0);
}

static int rls_deep_pwr_down(struct snfc_handler *sh)
{
	int ret;

	ret = send_opcode(sh, NOR_OP_RDP);
	if (ret)
		return ret;

	/* wait device tRES time */
	udelay(50);

	return 0;
}

/* Reset NOR flash.*/
static int nor_reset(struct snfc_handler *sh)
{
	int ret;

	ret = send_opcode(sh, NOR_OP_RSTEN);
	if (ret)
		return ret;

	ret = send_opcode(sh, NOR_OP_RST);
	if (ret)
		return ret;

	/* wait device tRST time */
	udelay(100);

	snfc_logd(" >>reset nor flash.\n");

	return 0;
}

static int nor_get_status(struct snfc_handler *sh, u8 *status)
{
	int ret;

	ret = execute_cmd(sh, SF_CMD_RDSR);
	if (ret)
		return ret;
	*status = READB(sh->sp->reg_base + REG_SF_RDSR);
	return ret;
}

static int wait_nor_ready(struct snfc_handler *sh, u32 tmo, u32 interval)
{
	int ret;
	u8 status = SR_WIP;
	u32 cnt;

	cnt = interval ? tmo / interval : tmo;

	while (cnt--) {
		ret = nor_get_status(sh, &status);
		if (ret)
			return ret;

		if (!(status & SR_WIP))
			break;

		udelay(interval);
	}

	if (status & SR_WIP)
		snfc_loge("NOR is in busy state.\n");

	return (status & SR_WIP) ? ERR_BUSY : 0;
}

static int nor_set_status(struct snfc_handler *sh, u8 *status)
{
	int ret;

	ret = nor_write_enable(sh);
	if (ret)
		return ret;

	ret = write_reg(sh, NOR_OP_WRSR, status, 1);
	if (ret)
		return ret;

	return wait_nor_ready(sh, WAIT_READY_TIMEOUT, INTERVAL_10US);
}

static int nor_get_config(struct snfc_handler *sh, u8 *config)
{
	return read_reg(sh, NOR_OP_RDCR, config, 1);
}

/* NOR_OP_WRSR used to set status and config register,
 * before set config register, need to get status register value first.
 * then, set status and config together.
 */
static int nor_set_config(struct snfc_handler *sh, u8 *config)
{
	int ret;
	u8 status_config[2];

	ret = nor_get_status(sh, status_config);
	if (ret)
		return ret;

	status_config[1] = *config;

	ret = nor_write_enable(sh);
	if (ret)
		return ret;

	ret = write_reg(sh, NOR_OP_WRSR, status_config, 2);
	if (ret)
		return ret;

	return wait_nor_ready(sh, WAIT_READY_TIMEOUT, INTERVAL_10US);
}

static int read_jedec_id(struct snfc_handler *sh, u8 *id)
{
	return read_reg(sh, NOR_OP_RDID, id, NOR_JEDEC_ID_LEN);
}

static int set_addr_width(struct snfc_handler *sh, int addr_width)
{
	int ret;
	u8 op;
	u8 val = READB(sh->sp->reg_base + REG_SF_DUAL);

	/* Add WREN for Micron flash.
	 * Before sent EN4B/EX4B to Micron flash should excute WREN first.
	 * It is not specified in the specification.
	 */
	ret = nor_write_enable(sh);
	if (ret)
		return ret;

	if (addr_width == SF_ADDR_WIDTH_4B &&
	    sh->nor->chip_size <= 0x1000000) {
		addr_width = SF_ADDR_WIDTH_3B;
	}

	switch (addr_width) {
	case SF_ADDR_WIDTH_4B:
		val |= SF_LARGE_ADDR_EN;
		op = NOR_OP_EN4B;
		break;
	default:
		snfc_loge("unexpected address width %u.\n", addr_width);
		snfc_loge("replace it with SF_ADDR_WIDTH_3B\n");
		addr_width = SF_ADDR_WIDTH_3B;
	case SF_ADDR_WIDTH_3B:
		val &= ~SF_LARGE_ADDR_EN;
		op = NOR_OP_EX4B;
		break;
	}

	ret = send_opcode(sh, op);
	if (ret)
		return ret;

	WRITEB(val, sh->sp->reg_base + REG_SF_DUAL);

	sh->sp->addr_width = addr_width;

	return wait_nor_ready(sh, WAIT_READY_TIMEOUT, INTERVAL_10US);
}

static int check_reg_set_success(struct snfc_handler *sh, u8 reg, u8 bit)
{
#define CHECK_QUAD_CNT 20
	int ret, cnt = CHECK_QUAD_CNT;
	u8 val;

	do {
		if (reg)
			ret = nor_get_config(sh, &val);
		else
			ret = nor_get_status(sh, &val);
		if (ret)
			return ret;

		if (val & bit)
			break;

		udelay(20);
	} while (--cnt);

	if (!cnt)
		return ERR_FAULT;

	return 0;
}

static int set_mxic_quad(struct snfc_handler *sh, bool enable)
{
	int ret;
	u8 status;

	ret =  nor_get_status(sh, &status);
	if (ret)
		return ret;

	if (enable)
		status |= SR_QUAD_EN_MX;
	else
		status &= ~SR_QUAD_EN_MX;

	ret = nor_set_status(sh, &status);
	if (ret)
		return ret;

	if (enable)
		return check_reg_set_success(sh, 0, SR_QUAD_EN_MX);

	return 0;
}

/* for some nor, we should check if QE bit set success after sending opcode. */
static int set_spansion_quad(struct snfc_handler *sh, bool enable)
{
	int ret;
	u8 config, tmp;

	ret = nor_get_config(sh, &config);
	if (ret)
		return ret;

	if (enable)
		config |= CR_QUAD_EN_SPAN;
	else
		config &= ~CR_QUAD_EN_SPAN;

	ret = nor_set_config(sh, &config);
	if (ret)
		return ret;

	ret = nor_get_config(sh, &tmp);
	if (ret)
		return ret;

	if (enable)
		return check_reg_set_success(sh, 1, CR_QUAD_EN_SPAN);

	return 0;
}

static int set_nor_quad_mode(struct snfc_handler *sh, bool enable)
{
	switch (sh->nor->manufact) {
	case NOR_MF_MACRONIX:
	case NOR_MF_ISSI:
		return set_mxic_quad(sh, enable);
	case NOR_MF_WINBOND:
	case NOR_MF_SPANSION:
	case NOR_MF_SST:
	case NOR_MF_GIGADEVICE:
	case NOR_MF_XTX:
		return set_spansion_quad(sh, enable);
	case NOR_MF_MICRON:
	case NOR_MF_ESMT:
		/* micron no need to config qe, it depends on cmd. */
		return 0;
	default:
		snfc_loge("Invalid manufact value(%u).\n", sh->nor->manufact);
		snfc_loge("It's strange, maybe occur memory overlay,\n");
		snfc_loge("or else, shouldn't be here.\n");
		return ERR_INVAL;
	}
}

static int __set_read_mode(struct snfc_handler *sh,
			   int read_mode, bool skip_nor_qe)
{
	int ret = 0;
	u8 val = READB(sh->sp->reg_base + REG_SF_DUAL) &
		(~SF_READ_MODE_MASK);

	if (!(BIT(read_mode) & sh->nor->read_cap))
		read_mode = SF_SINGLE_READ;

	/* clean fast read bit. */
	WRITEB(SF_FAST_READ_DIS, sh->sp->reg_base + REG_SF_CFG1);
	/* if not set quad mode, should clean NOR qaud bit.*/
	if (read_mode != SF_4XIO_READ && read_mode != SF_1_1_4_READ &&
	    !skip_nor_qe)
		set_nor_quad_mode(sh, false);

	switch (read_mode) {
	case SF_FAST_READ:
		WRITEB(SF_FAST_READ_EN, sh->sp->reg_base + REG_SF_CFG1);
		break;
	case SF_1_1_2_READ:
		WRITEB(NOR_OP_READ_1_1_2, sh->sp->reg_base + REG_SF_PRGDATA3);
		val |= SF_READ_DUAL_EN;
		break;
	case SF_2XIO_READ:
		WRITEB(NOR_OP_READ_2IO, sh->sp->reg_base + REG_SF_PRGDATA3);
		val |= SF_READ_DUAL_EN | SF_READ_ADDR_DUAL;
		break;
	case SF_1_1_4_READ:
		if (!skip_nor_qe) {
			ret = set_nor_quad_mode(sh, true);
			if (ret)
				return ret;
		}
		WRITEB(NOR_OP_READ_1_1_4, sh->sp->reg_base + REG_SF_PRGDATA4);
		val |= SF_READ_QUAD_EN;
		break;
	case SF_4XIO_READ:
		if (!skip_nor_qe) {
			ret = set_nor_quad_mode(sh, true);
			if (ret)
				return ret;
		}
		WRITEB(NOR_OP_READ_4IO, sh->sp->reg_base + REG_SF_PRGDATA4);
		val |= SF_READ_QUAD_EN | SF_READ_ADDR_QUAD;
		break;
	default:
		snfc_logi("invalid read mode(%u).\n", read_mode);
		snfc_logi("replace it with SF_SINGLE_READ\n");
		read_mode = SF_SINGLE_READ;
	case SF_SINGLE_READ:
		break;
	}

	WRITEB(val, sh->sp->reg_base + REG_SF_DUAL);

	sh->sp->read_mode = read_mode;

	return ret;
}

static int set_read_mode(struct snfc_handler *sh, int read_mode)
{
	return __set_read_mode(sh, read_mode, false);
}

static void set_addr(struct snfc_handler *sh, u32 addr)
{
	int i;

	for (i = 0; i < 3; i++) {
		WRITEB(addr & 0xff, sh->sp->reg_base + REG_SF_RADR0 + i * 4);
		addr >>= 8;
	}
	/* Last register is non-contiguous */
	WRITEB(addr & 0xff, sh->sp->reg_base + REG_SF_RADR3);
}

static int dma(struct snfc_handler *sh, u32 src, u32 dest, u32 len)
{
	if (!ALIGNED_16(src) || !ALIGNED_16(dest) || !ALIGNED_16(len)) {
		snfc_loge("Address or size is not 16-byte alignment!\n");
		snfc_loge("src: %x, dest: %x, len: %x\n", src, dest, len);
		return ERR_INVAL;
	}

	/* reset */
	WRITEL(SF_FDMA_CTL_CLEAR, sh->sp->reg_base + REG_FDMA_CTL);
	WRITEL(SF_FDMA_CTL_RST, sh->sp->reg_base + REG_FDMA_CTL);
	WRITEL(SF_FDMA_CTL_WDLE_EN, sh->sp->reg_base + REG_FDMA_CTL);

	/* flash source address */
	WRITEL(src, sh->sp->reg_base + REG_FDMA_FADR);

	/* DRAM destination address */
	WRITEL(dest, sh->sp->reg_base + REG_FDMA_DADR);
	WRITEL(dest + len, sh->sp->reg_base + REG_FDMA_END_DADR);

	/* Enable */
	WRITEL(SF_FDMA_CTL_WDLE_EN | SF_FDMA_CTL_DMA_TRIG,
	       sh->sp->reg_base + REG_FDMA_CTL);

	return poll_cond_tmo((READL(sh->sp->reg_base + REG_FDMA_CTL) &
		SF_FDMA_CTL_DMA_TRIG) == 0,
		ACCESS_REG_TIMEOUT, INTERVAL_1US);
}

static int read(struct snfc_handler *sh, u32 addr, u8 *buf, u32 len)
{
	int ret = 0;
	u32 i;
	u8 cmd;

	if (!len)
		return 0;

	if ((addr + len) > sh->nor->chip_size) {
		snfc_loge("access out of range.\n");
		return ERR_INVAL;
	}

	__set_read_mode(sh, sh->sp->read_mode, true);

	switch (sh->sp->read_path) {
	case SF_DIRECT_PATH:
		memcpy(buf, (u8 *)(sh->sp->mem_base + addr), len);
		break;

	case SF_DMA_PATH:
		os_invalidate_cache_range(buf, len);
		ret = dma(sh, addr, (u32)buf, len);
		os_invalidate_cache_range(buf, len);
		break;

	default:
		snfc_logi("invalid read_path value(%u).\n", sh->sp->read_path);
		snfc_logi("replace it with SF_PIO_PATH.\n");
		sh->sp->read_path = SF_PIO_PATH;
	case SF_PIO_PATH:
		set_addr(sh, addr);

		for (i = 0; i < len; i++) {
			cmd = SF_CMD_AUTOINC | SF_CMD_RD;
			ret = execute_cmd(sh, cmd);
			if (ret)
				return ret;
			buf[i] = READB(sh->sp->reg_base + REG_SF_RDATA);
		}
	}

	return ret;
}

static int trigger_write(struct snfc_handler *sh)
{
	int ret;
	u8 cmd;

	ret = nor_write_enable(sh);
	if (ret)
		return ret;

	cmd = SF_CMD_WR | SF_CMD_AUTOINC;
	ret = execute_cmd(sh, cmd);
	if (ret)
		return ret;

	return wait_nor_ready(sh, WAIT_READY_TIMEOUT, INTERVAL_10US);
}

static int write_byte(struct snfc_handler *sh, u32 addr,
		      const u8 *buf, u32 len)
{
	int ret = 0;
	u32 i;

	set_addr(sh, addr);

	for (i = 0; i < len; i++, buf++) {
		WRITEB(*buf, sh->sp->reg_base + REG_SF_WDATA);
		ret = trigger_write(sh);
		if (ret)
			return ret;
	}

	return ret;
}

static int write_page(struct snfc_handler *sh, u32 addr,
		      const u8 *buf, u32 len)
{
	u32 i, loop;
	u32 *__buf = (u32 *)buf;
	int ret;

	if ((addr & (sh->nor->page_size - 1)) ||
	    (len & (sh->nor->page_size - 1))) {
		snfc_loge("addr or len not aligned with page_size\n");
		return ERR_INVAL;
	}

	loop = len / sh->nor->page_size;

	WRITEB(1, sh->sp->reg_base + REG_SF_CFG2); /* Enable buffer */

	while (loop) {
		set_addr(sh, addr);

		/* write data DWORD-by-DWORD to REG_SF_PP_DW_DATA until
		 * fulfill the write buffer(HW internal buffer)
		 */
		for (i = 0; i < (sh->nor->page_size >> 2); i++)
			WRITEL(*__buf++, sh->sp->reg_base + REG_SF_PP_DW_DATA);

		ret = trigger_write(sh);
		if (ret)
			return ret;

		addr += sh->nor->page_size;
		loop--;
	}

	WRITEB(0, sh->sp->reg_base + REG_SF_CFG2); /* Disable buffer */

	return 0;
}

static int write_multi_page(struct snfc_handler *sh, u32 addr,
			    const u8 *buf, u32 len)
{
	u32 loop, sf_page_cnt, sf_page_size;
	u32 wr_page_cnt = 0;
	u32 *__buf = (u32 *)buf;
	int i, ret;

	/* this condition should be thought again */
	if ((addr & (sh->sp->buf_size - 1)) ||
	    (len & (sh->sp->buf_size - 1))) {
		snfc_loge("addr or len not aligned with buf_size\n");
		return ERR_INVAL;
	}

	/* page size of unit mtk snfc register is 16 bytes.
	 * NOR flash's page size is always aligned 16 so far.
	 */
	sf_page_size = sh->nor->page_size / 16;

	/* buf_size of mtk snfc is 512, and page_size of NOR is smaller than
	 * snfc's buf_size now. If NOR's page size increased in future,
	 * these code should be refactored.
	 */
	sf_page_cnt = sh->sp->buf_size / sh->nor->page_size;

	loop = len / sh->sp->buf_size;

	WRITEB(1, sh->sp->reg_base + REG_SF_CFG2);

	WRITEL(sf_page_size, sh->sp->reg_base + REG_SF_PAGESIZE);
	WRITEL(1, sh->sp->reg_base + REG_MPP_EN); /* Enable mpp */

	while (loop) {
		set_addr(sh, addr + wr_page_cnt * sh->nor->page_size);
		WRITEL(sf_page_cnt, sh->sp->reg_base + REG_SF_PAGECNT);
		for (i = 0; i < (sh->sp->buf_size >> 2); i++)
			WRITEL(*__buf++, sh->sp->reg_base + REG_SF_PP_DW_DATA);

		ret = trigger_write(sh);
		if (ret) {
			WRITEL(0, sh->sp->reg_base + REG_MPP_EN);
			WRITEB(0, sh->sp->reg_base + REG_SF_CFG2);
			return ret;
		}

		loop--;
		wr_page_cnt += sf_page_cnt;
	}

	WRITEL(0, sh->sp->reg_base + REG_MPP_EN); /* Disable mpp */
	WRITEB(0, sh->sp->reg_base + REG_SF_CFG2);

	return 0;
}

static int write(struct snfc_handler *sh, u32 addr, const u8 *buf, u32 len)
{
	int ret = 0;

	if (!len)
		return 0;

	if ((addr + len) > sh->nor->chip_size) {
		snfc_loge("access out of range.\n");
		return ERR_INVAL;
	}

	switch (sh->sp->write_mode) {
	case SF_PAGE_WRITE:
		ret = write_page(sh, addr, buf, len);
		break;
	case SF_MULTI_PAGE_WRITE:
		ret = write_multi_page(sh, addr, buf, len);
		break;
	default:
		snfc_logi("unexpected write_mode %d.\n", sh->sp->write_mode);
		snfc_logi("replace it with SF_BYTE_WRITE\n");
		sh->sp->write_mode = SF_BYTE_WRITE;
	case SF_BYTE_WRITE:
		ret = write_byte(sh, addr, buf, len);
		break;
	}

	return ret;
}

static int erase_single_unit(struct snfc_handler *sh, u32 addr, u8 cmd)
{
	int ret, i;
	u8 buf[SF_ADDR_WIDTH_4B];

	ret = nor_write_enable(sh);
	if (ret)
		return ret;

	if (cmd != NOR_OP_CHIP_ERASE) {
		for (i = sh->sp->addr_width - 1; i >= 0; i--) {
			buf[i] = addr & 0xff;
			addr >>= 8;
		}

		ret = write_reg(sh, cmd, buf, sh->sp->addr_width);
		if (ret)
			return ret;
	} else {
		ret = execute_cmd(sh, SF_CMD_ERASE);
		if (ret)
			return ret;
	}

	return wait_nor_ready(sh, ERASE_TIMEOUT, INTERVAL_100US);
}

static int erase(struct snfc_handler *sh, u32 addr, u32 len)
{
	int ret = 0;
	u32 loop, step, cmd;

	if (len == 0)
		return 0;

	if ((addr + len) > sh->nor->chip_size)
		return ERR_INVAL;

	switch (sh->sp->erase_mode) {
	case SF_32K_ERASE:
		if (ALIGNED_32K(addr) && ALIGNED_32K(len)) {
			step = 0x8000;
			cmd = NOR_OP_BE_32K;
			break;
		}
		return ERR_INVAL;
	case SF_64K_ERASE:
		if (ALIGNED_64K(addr) && ALIGNED_64K(len)) {
			step = 0x10000;
			cmd = NOR_OP_BE_64K;
			break;
		}
		return ERR_INVAL;
	case SF_CHIP_ERASE:
		step = len;
		cmd = NOR_OP_CHIP_ERASE;
		break;
	default:
		snfc_logi("unexpected erase_mode %d.\n", sh->sp->erase_mode);
		snfc_logi("replace it with SF_4K_ERASE\n");
		sh->sp->erase_mode = SF_4K_ERASE;
	case SF_4K_ERASE:
		if (ALIGNED_4K(addr) && ALIGNED_4K(len)) {
			step = 0x1000;
			cmd = NOR_OP_BE_4K;
			break;
		}
		return ERR_INVAL;
	}

	loop = len / step;
	while (loop) {
		ret = erase_single_unit(sh, addr, cmd);
		if (ret)
			return ret;

		addr += step;
		loop--;
	}

	return ret;
}

static int nor_write_protect(struct snfc_handler *sh, bool enable)
{
	int ret;
	u8 status;

	ret = nor_get_status(sh, &status);
	if (ret)
		return ret;

	switch (sh->nor->manufact) {
	case NOR_MF_MICRON:
		if (enable)
			status |= (SR_BP0 | SR_BP1 | SR_BP2 | SR_BP3_MT);
		else
			status &= ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_BP3_MT);
		break;
	default:
		if (enable)
			status |= (SR_BP0 | SR_BP1 | SR_BP2 | SR_BP3);
		else
			status &= ~(SR_BP0 | SR_BP1 | SR_BP2 | SR_BP3);
	}

	return nor_set_status(sh, &status);
}

static int set_irq_enable(struct snfc_handler *sh, u32 value)
{
	WRITEB(value, sh->sp->reg_base + REG_SF_INTREN);

	return poll_cond_tmo(READB(sh->sp->reg_base + REG_SF_INTREN) == value,
		ACCESS_REG_TIMEOUT, INTERVAL_1US);
}

static int get_irq_status(struct snfc_handler *sh, u32 *status)
{
	*status = READB(sh->sp->reg_base + REG_SF_INTRSTUS);

	return 0;
}

static int clr_irq_status(struct snfc_handler *sh)
{
	WRITEB(0xff, sh->sp->reg_base + REG_SF_INTRSTUS);

	return 0;
}

static int set_read_path(struct snfc_handler *sh, int read_path)
{
	sh->sp->read_path = read_path;

	return 0;
}

static int set_erase_mode(struct snfc_handler *sh, int erase_mode)
{
	sh->sp->erase_mode = erase_mode;

	return 0;
}

static int set_write_mode(struct snfc_handler *sh, int write_mode)
{
	sh->sp->write_mode = write_mode;

	return 0;
}

static int set_delsel(struct snfc_handler *sh, struct snfc_delsel *ds)
{
	u32 val;

	val = ds->sio_0_in | ds->sck_out << 4;
	WRITEL(val, sh->sp->reg_base + REG_SF_DELSEL0);

	/*bit5 in REG_SF_DELSEL1 indicates SMPCK_INV, should not change it.*/
	val = READL(sh->sp->reg_base + REG_SF_DELSEL1) & ~0xF;
	val |= ds->sio_1_out;
	WRITEL(val, sh->sp->reg_base + REG_SF_DELSEL1);

	val = ds->sio_0_out | ds->sio_1_in << 4;
	WRITEL(val, sh->sp->reg_base + REG_SF_DELSEL2);

	val = ds->sio_2_out | ds->sio_2_in << 4;
	WRITEL(val, sh->sp->reg_base + REG_SF_DELSEL3);

	val = ds->sio_3_out | ds->sio_3_in << 4;
	WRITEL(val, sh->sp->reg_base + REG_SF_DELSEL4);

	return 0;
}

static int build_default_state(struct snfc_handler *sh)
{
	int ret;

	ret = nor_write_protect(sh, false);
	if (ret)
		return ret;

	ret = set_addr_width(sh, sh->sp->addr_width);
	if (ret)
		return ret;

	return set_read_mode(sh, sh->sp->read_mode);
}

static const struct snfc_ops snfc_ops = {
	.read_reg = read_reg,
	.write_reg = write_reg,

	.set_delsel = set_delsel,
	.set_irq_enable = set_irq_enable,
	.get_irq_status = get_irq_status,
	.clr_irq_status = clr_irq_status,

	.set_addr_width = set_addr_width,
	.set_read_path = set_read_path,
	.set_read_mode = set_read_mode,
	.set_erase_mode = set_erase_mode,
	.set_write_mode = set_write_mode,

	.read = read,
	.write = write,
	.erase = erase,

	.nor_reset = nor_reset,
	.nor_write_protect = nor_write_protect,
	.nor_get_status = nor_get_status,
	.nor_get_config = nor_get_config,
	.wait_nor_ready = wait_nor_ready,
};

/* malloc cannot used in ctp, just use global variable. */
static struct snfc_handler sh_g;
static struct snfc_property sp_g;
struct snfc_handler *snfc_nor_init(struct snfc_property *sp)
{
	int ret;
	u8 id[NOR_JEDEC_ID_LEN];

	struct snfc_handler *sh;

	if (!sp) {
		snfc_loge("user give invalid sflash property, init fail!\n");
		return NULL;
	}

	sh = &sh_g;
	sh->sp = &sp_g;

	memcpy(sh->sp, sp, sizeof(struct snfc_property));
	sh->ops = &snfc_ops;

	/* initialize controller to accept commands */
	WRITEB(SF_WP_TURN_OFF, sp->reg_base + REG_SF_WRPROT);

	rls_deep_pwr_down(sh);

	nor_reset(sh);

	ret = read_jedec_id(sh, id);
	if (ret) {
		snfc_loge("read flash id fail.\n");
		return NULL;
	}

	sh->nor = get_nor_over_tbl(id);
	if (!sh->nor) {
		snfc_loge("the flash has not been verified yet!\n");
		snfc_loge("current id(%x,%x,%x)\n", id[0], id[1], id[2]);
		return NULL;
	}

	snfc_logi("%s(0x%x 0x%x 0x%x)\n",
		  sh->nor->name, sh->nor->manufact,
		  sh->nor->mem_type, sh->nor->mem_density);
	snfc_logi("pagesize=0x%x\nsectsize=0x%x\n",
		  sh->nor->page_size, sh->nor->sect_size);
	snfc_logi("chipsize=0x%x\nreadcap=0x%x\n",
		  sh->nor->chip_size, sh->nor->read_cap);

	ret = build_default_state(sh);
	if (ret) {
		snfc_loge("build default state fail, init fail.\n");
		return NULL;
	}

	return sh;
}
