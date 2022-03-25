/*
 * Copyright (c) 2012  MediaTek, Inc.
 * Author : Guochun.Mao@mediatek.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/**
 * spi nor flash uboot driver,
 * responsible for wrap snfc.
 */

#include "mtk_nor.h"
#include <malloc.h>

static struct snfc_handler *sh;

struct nor_pinmux {
	u32 reg;
	u32 mode;
	u32 shift;
	u32 mask;
};

static void clk_init(void)
{
#if defined(LEOPARD_ASIC_BOARD)
#define SNFC_CLOCK_REG 0x10210050
#define SNFC_CLOCK_MASK 0xF8FFFFFF
#define SNFC_CLOCK_SHIFT 24
	u32 val = READL(SNFC_CLOCK_REG);

	val &= SNFC_CLOCK_MASK;
	val |= 2 << SNFC_CLOCK_SHIFT;
	WRITEL(val, SNFC_CLOCK_REG);
#endif
}

static void io_init(void)
{
#ifdef NOR_NEED_DO_PINMUX_SETTING
	struct nor_pinmux np[] = {
		{NOR_CS_REG, NOR_CS_MODE, NOR_CS_SHIFT, NOR_CS_MASK},
		{NOR_CLK_REG, NOR_CLK_MODE, NOR_CLK_SHIFT, NOR_CLK_MASK},
		{NOR_SIO0_REG, NOR_SIO0_MODE, NOR_SIO0_SHIFT, NOR_SIO0_MASK},
		{NOR_SIO1_REG, NOR_SIO1_MODE, NOR_SIO1_SHIFT, NOR_SIO1_MASK},
		{NOR_SIO2_REG, NOR_SIO2_MODE, NOR_SIO2_SHIFT, NOR_SIO2_MASK},
		{NOR_SIO3_REG, NOR_SIO3_MODE, NOR_SIO3_SHIFT, NOR_SIO3_MASK},
	};
	int i;
	int array_size = sizeof(np) / sizeof(struct nor_pinmux);
	u32 tmp;

	for (i = 0; i < array_size; i++) {
		tmp = READL(np[i].reg) & (~np[i].mask);
		tmp |= np[i].mode << np[i].shift;
		WRITEL(tmp, np[i].reg);
	}
#endif
}

#define MEMCPY_THRESHOLD 0x2000
#define NOR_BUF_LEN 0x1000
#define NOR_DMA_ALIGN 0x40
static u8 nor_dma_buf[NOR_BUF_LEN]  __attribute__((aligned(NOR_DMA_ALIGN)));
int mtk_nor_read(u32 addr, u32 len, size_t *retlen, u8 *buf)
{
	int ret;
	u32 i, data_header;
	u32 buf_cnt, cur_addr, cur_index = 0;

	if (!len)
		return 0;

	if (len < MEMCPY_THRESHOLD) {
		sh->ops->set_read_path(sh, SF_DIRECT_PATH);
		ret = sh->ops->read(sh, addr, buf, len);
		if (!ret)
			*retlen = len;
		return ret;
	}

	sh->ops->set_read_path(sh, SF_DMA_PATH);

	data_header = addr % NOR_DMA_ALIGN;
	buf_cnt = (data_header + len + NOR_BUF_LEN - 1) / NOR_BUF_LEN;
	cur_addr = addr - data_header;

	ret = sh->ops->read(sh, cur_addr, nor_dma_buf, NOR_BUF_LEN);
	if (ret)
		return ret;
	memcpy(buf, nor_dma_buf + data_header, NOR_BUF_LEN - data_header);
	*retlen = NOR_BUF_LEN - data_header;
	cur_index = NOR_BUF_LEN - data_header;
	cur_addr += NOR_BUF_LEN;

	/* buf_cnt must larger than 2  here*/
	for (i = 0; i < buf_cnt - 2; i++) {
		ret = sh->ops->read(sh, cur_addr, nor_dma_buf,
				    NOR_BUF_LEN);
		if (ret)
			return ret;
		memcpy(buf + cur_index, nor_dma_buf, NOR_BUF_LEN);
		(*retlen) += NOR_BUF_LEN;
		cur_index += NOR_BUF_LEN;
		cur_addr += NOR_BUF_LEN;
	}

	ret = sh->ops->read(sh, cur_addr, nor_dma_buf, NOR_BUF_LEN);
	if (ret)
		return ret;
	memcpy(buf + cur_index, nor_dma_buf, (len + data_header) % NOR_BUF_LEN);
	*retlen = len;
	return 0;
}

int mtk_nor_write(u32 addr, u32 len, size_t *retlen, const u8 *buf)
{
	int ret = 0;
	u32 wr_len = 0, rem_len, offset, loop;

	offset = addr % SNFC_BUF_SIZE;
	if (offset) {
		sh->ops->set_write_mode(sh, SF_BYTE_WRITE);
		ret = sh->ops->write(sh, addr, buf, SNFC_BUF_SIZE - offset);
		if (ret)
			return ret;
		wr_len = SNFC_BUF_SIZE - offset;
	}

	*retlen = wr_len;
	offset = addr + wr_len;
	rem_len = len - wr_len;
	loop = rem_len / SNFC_BUF_SIZE;
	if (loop) {
		sh->ops->set_write_mode(sh, SF_PAGE_WRITE);
		ret = sh->ops->write(sh, offset, buf + wr_len,
				     loop * SNFC_BUF_SIZE);
		if (ret)
			return ret;
		wr_len += loop * SNFC_BUF_SIZE;
	}

	*retlen = wr_len;
	offset += loop * SNFC_BUF_SIZE;
	rem_len -= loop * SNFC_BUF_SIZE;
	if (rem_len) {
		sh->ops->set_write_mode(sh, SF_BYTE_WRITE);
		ret = sh->ops->write(sh, offset, buf + wr_len, rem_len);
		if (ret)
			return ret;
	}

	*retlen = len;
	return ret;
}

int mtk_nor_erase(u32 addr, u32 len)
{
#define SIZE_64K 0x10000
	int ret = 0;
	u32 mask, size, end_addr = addr + len;

	if (len == 0)
		return ret;

	if (len == sh->nor->chip_size) {
		sh->ops->set_erase_mode(sh, SF_CHIP_ERASE);
		return sh->ops->erase(sh, addr, len);
	}

	mask = SIZE_64K - 1;
	if ((addr & mask) && ((end_addr & ~mask) != (addr & ~mask))) {
		size = SIZE_64K - (addr & mask);
		sh->ops->set_erase_mode(sh, SF_4K_ERASE);
		ret = sh->ops->erase(sh, addr, size);
		if (ret)
			return ret;
		addr += size;
		len -= size;
	}

	size = (end_addr - addr) & ~mask;
	if (size > 0) {
		sh->ops->set_erase_mode(sh, SF_64K_ERASE);
		ret = sh->ops->erase(sh, addr, size);
		if (ret)
			return ret;
		addr += size;
		len -= size;
	}

	if (len > 0) {
		sh->ops->set_erase_mode(sh, SF_4K_ERASE);
		ret = sh->ops->erase(sh, addr, len);
	}

	return ret;
}

int mtk_nor_init(void)
{
	struct snfc_property sp = {
		.reg_base   = SNFC_REG_BASE,
		.mem_base   = SNFC_MEM_BASE,
		.buf_size   = SNFC_BUF_SIZE,
		.addr_width = SF_ADDR_WIDTH_4B,
		.read_mode  = SF_FAST_READ,
		.read_path  = SF_DMA_PATH,
		.erase_mode = SF_4K_ERASE,
		.write_mode = SF_BYTE_WRITE
	};

	io_init();

	clk_init();

	sh = snfc_nor_init(&sp);
	if (!sh) {
		snfc_loge("init snfc & nor FAIL.\n");
		return ERR_FAULT;
	}

	return 0;
}

static int mtk_nor_sanity(void)
{
#define TST_ADDR 0x700000
#define TST_SIZE 0x1000
	int ret, i;
	size_t retlen;
	u8 *buf;

	buf = malloc(TST_SIZE);
	if (!buf)
		return ERR_NOMEM;

	ret = mtk_nor_erase(TST_ADDR, 0x10000);
	if (ret)
		goto out;

	for (i = 0; i < TST_SIZE; i++)
		buf[i] = i & 0xFF;

	ret = mtk_nor_write(TST_ADDR, TST_SIZE, &retlen, buf);
	if (ret)
		goto out;

	memset(buf, 0, TST_SIZE);
	ret = mtk_nor_read(TST_ADDR, TST_SIZE, &retlen, buf);
	if (ret)
		goto out;
	for (i = 0; i < TST_SIZE; i++) {
		if (buf[i] != (i & 0xFF)) {
			snfc_loge("check fail @0x%x + 0x%x\n", TST_ADDR, i);
			snfc_loge("	expect %x, cur %x\n",
				i & 0xFF, buf[i]);
			snfc_loge("buf address: 0x%x\n", (u32)buf);
			ret = ERR_FAULT;
			goto out;
		}
	}

	ret = mtk_nor_erase(TST_ADDR, 0x10000);
	if (ret)
		goto out;

	memset(buf, 0, TST_SIZE);
	ret = mtk_nor_read(TST_ADDR, TST_SIZE, &retlen, buf);
	if (ret)
		goto out;
	for (i = 0; i < TST_SIZE; i++) {
		if (buf[i] != 0xFF) {
			snfc_loge("check fail @0x%x + 0x%x\n", TST_ADDR, i);
			snfc_loge("	expect 0xFF, cur %x\n", buf[i]);
			snfc_loge("buf address: 0x%x\n", (u32)buf);
			ret = ERR_FAULT;
			goto out;
		}
	}
out:
	free(buf);
	return ret;
}

static int mtk_nor_command(cmd_tbl_t *cmd, int flag,
				int argc, char * const argv[])
{
	int ret;
	u32 addr, len, retlen = 0;
	u8 *buf = NULL;

	if (!strncmp(argv[1], "read", 5)) {
		buf = (u8 *)simple_strtoul(argv[2], NULL, 16);
		if (!buf) {
			snfc_loge("Buf address fault.\n");
			return ERR_INVAL;
		}
		addr = (u32)simple_strtoul(argv[3], NULL, 16);
		len = (u32)simple_strtoul(argv[4], NULL, 16);

		ret = mtk_nor_read(addr, len, &retlen, buf);
		if (ret) {
			snfc_loge("Nor flash read fail.\n");
			return ERR_FAULT;
		}
	} else if (!strncmp(argv[1], "erase", 6)) {
		addr = (u32)simple_strtoul(argv[2], NULL, 16);
		len = (u32)simple_strtoul(argv[3], NULL, 16);
		ret = mtk_nor_erase(addr, len);
		if (ret) {
			snfc_loge("Erase Fail\n");
			return ERR_FAULT;
		}
	} else if (!strncmp(argv[1], "write", 6)) {
		buf = (u8 *)simple_strtoul(argv[2], NULL, 16);
		if (!buf) {
			snfc_logi("Buf address fault.\n");
			return ERR_INVAL;
		}
		addr = (u32)simple_strtoul(argv[3], NULL, 16);
		len = (u32)simple_strtoul(argv[4], NULL, 16);
		ret = mtk_nor_write(addr, len, &retlen, buf);
		if (ret) {
			snfc_loge("Norflash write fail\n");
			return ERR_FAULT;
		}
	} else if (!strncmp(argv[1], "sanity", 7)) {
		ret = mtk_nor_sanity();
		if (ret) {
			snfc_loge("Nor flash sanity test fail\n");
			return ERR_FAULT;
		}
		printf("Nor flash sanity test pass.\n");
	} else {
		printf("Usage:\n%s\n", cmd->usage);
		printf("Use \"help snor\" for detail!\n");
		return ERR_INVAL;
	}

	return 0;
}

U_BOOT_CMD(
	snor, 5, 1, mtk_nor_command,
	"snor   - spi-nor flash command\n",
	"snor usage:\n"
	"	snor read  <load_addr> <addr> <len>\n"
	"	snor write <load_addr> <addr> <len>\n"
	"	snor erase <addr> <len>\n"
	"	snor sanity\n"
);

