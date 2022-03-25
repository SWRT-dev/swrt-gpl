/*
 * Driver for Cadence QSPI Controller
 *
 * Copyright (C) 2012 Altera Corporation
 * Copyright (C) 2018 Intel Corporation
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/spi/spi.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include "spi-cadence-qspi.h"
#include "spi-cadence-qspi-apb.h"

/* 1-beat single, 4-byte width = 4-byte single = 2**2 */
#define CQSPI_NUMSGLREQBYTES (2)
/* 16-beat burst, 4-byte width = 64-byte bursts = 2**6 */
#define CQSPI_NUMBURSTREQBYTES (6)

unsigned int start_print = 0;

#define debug_print(fmt, ...)	\
do {	\
	if (start_print)	\
		printk(fmt, ##__VA_ARGS__);	\
} while(0)

void SwapBytes(void *pv, unsigned int n)
{
	unsigned char *p = pv;
	unsigned int lo, hi;

	for(lo=0, hi=n-1; hi>lo; lo++, hi--) {
		unsigned char tmp = p[lo];
		p[lo] = p[hi];
		p[hi] = tmp;
	}
}

/**
 * Convert address buffer into word format (MSB first).
 */
static unsigned int cadence_qspi_apb_cmd2addr(const unsigned char *addr_buf,
	unsigned int addr_width)
{
	unsigned int addr = 0;
	int i;

	debug_print("[%s] addr_buf[0]=0x%x addr_buf[1]=0x%x addr_buf[2]=0x%x "
		"addr_buf[3]=0x%x addr_width %d\n", __func__,
		addr_buf[0], addr_buf[1], addr_buf[2], addr_buf[3], addr_width);

	if (addr_width > 4) {
		pr_err("Invalid address width %u\n", addr_width);
		return 0;
	}

	for (i = 0; i < addr_width; i++) {
		addr = addr << 8;
		addr |= addr_buf[i];
	}

	return addr;
}

void disable_qspi_direct_access(void *reg_base)
{
	u32 reg;

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(CQSPI_REG_CONFIG_DIRECT_MASK);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	return;
}

void enable_qspi_direct_access(void *reg_base)
{
	u32 reg;

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg |= CQSPI_REG_CONFIG_DIRECT_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	return;
}

/* Return 1 if idle, otherwise return 0 (busy). */
static unsigned int cadence_qspi_wait_idle(void *reg_base)
{
	unsigned int count = 0;
	unsigned timeout;

	timeout = cadence_qspi_init_timeout(CQSPI_TIMEOUT_MS);
	while (cadence_qspi_check_timeout(timeout)) {
		if (CQSPI_REG_IS_IDLE(reg_base)) {
			/* Read few times in succession to ensure it does
			not transition low again */
			count++;
			if (count >= CQSPI_POLL_IDLE_RETRY)
				return 1;
		} else {
			count = 0;
		}
	}
	/* Timeout, in busy mode. */
	pr_err("QSPI: QSPI is still busy after %dms timeout.\n",
		CQSPI_TIMEOUT_MS);
	return 0;
}
static void cadence_qspi_apb_readdata_capture(void *reg_base,
			unsigned int bypass, unsigned int delay)
{
	unsigned int reg;

	debug_print("%s %d %d\n", __func__, bypass, delay);
	cadence_qspi_apb_controller_disable(reg_base);

	reg = CQSPI_READL(reg_base + CQSPI_REG_READCAPTURE);
	if (bypass)
		reg |= (1 << CQSPI_REG_READCAPTURE_BYPASS_LSB);
	else
		reg &= ~(1 << CQSPI_REG_READCAPTURE_BYPASS_LSB);

	reg &= ~(CQSPI_REG_READCAPTURE_DELAY_MASK
		<< CQSPI_REG_READCAPTURE_DELAY_LSB);

	reg |= ((delay & CQSPI_REG_READCAPTURE_DELAY_MASK)
		<< CQSPI_REG_READCAPTURE_DELAY_LSB);

		CQSPI_WRITEL(reg, reg_base + CQSPI_REG_READCAPTURE);
	cadence_qspi_apb_controller_enable(reg_base);
	return;
}
static void cadence_qspi_apb_config_baudrate_div(void *reg_base,
		unsigned int ref_clk_hz, unsigned int sclk_hz)
{
	unsigned int reg;
	unsigned int div;

	debug_print("%s %d %d\n", __func__, ref_clk_hz, sclk_hz);
	cadence_qspi_apb_controller_disable(reg_base);

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(CQSPI_REG_CONFIG_BAUD_MASK << CQSPI_REG_CONFIG_BAUD_LSB);
	div = ref_clk_hz / sclk_hz;
	/* Recalculate the baudrate divisor based on QSPI specification. */
	if (div > 32)
		div = 32;
	/* Check if even number. */
	if (div & 1)
		div = (div / 2);
	else
		div = (div / 2) - 1;
	debug_print("QSPI: ref_clk %dHz sclk %dHz div 0x%x\n", ref_clk_hz,
		sclk_hz, div);
	div = (div & CQSPI_REG_CONFIG_BAUD_MASK) << CQSPI_REG_CONFIG_BAUD_LSB;
	reg |= div;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	cadence_qspi_apb_controller_enable(reg_base);
	return;
}
static void cadence_qspi_apb_chipselect(void *reg_base,
	unsigned int chip_select, unsigned int decoder_enable)
{
	unsigned int reg;

	debug_print("%s\n", __func__);
	debug_print("QSPI: chipselect %d decode %d\n", chip_select,
		decoder_enable);
	cadence_qspi_apb_controller_disable(reg_base);

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	/* docoder */
	if (decoder_enable)
		reg |= CQSPI_REG_CONFIG_DECODE_MASK;
	else {
		reg &= ~CQSPI_REG_CONFIG_DECODE_MASK;
		/* Convert CS if without decoder.
		 * CS0 to 4b'1110
		 * CS1 to 4b'1101
		 * CS2 to 4b'1011
		 * CS3 to 4b'0111
		 */
		chip_select = 0xF & ~(1 << chip_select);
	}
	reg &= ~(CQSPI_REG_CONFIG_CHIPSELECT_MASK
			<< CQSPI_REG_CONFIG_CHIPSELECT_LSB);
	reg |= (chip_select & CQSPI_REG_CONFIG_CHIPSELECT_MASK)
			<< CQSPI_REG_CONFIG_CHIPSELECT_LSB;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	cadence_qspi_apb_controller_enable(reg_base);
	return;
}
static int cadence_qspi_apb_exec_flash_cmd(void *reg_base, unsigned int reg)
{
	unsigned int timeout;

	debug_print("cadence_qspi_apb_exec_flash_cmd: reg_base 0x%p reg 0x%x\n",
		reg_base, reg);
	/* Write the CMDCTRL without start execution. */
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CMDCTRL);
	/* Start execute */
	reg |= CQSPI_REG_CMDCTRL_EXECUTE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CMDCTRL);
	debug_print("[%s] CQSPI_REG_CMDCTRL=0x%x\n", __func__, reg);
	/* Polling for completion. */
	timeout = cadence_qspi_init_timeout(CQSPI_TIMEOUT_MS);
	while (cadence_qspi_check_timeout(timeout)) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_CMDCTRL) &
			CQSPI_REG_CMDCTRL_INPROGRESS_MASK;
		if (!reg)
			break;
	}
	if (reg != 0) {
		pr_err("QSPI: flash cmd execute time out\n");
		return -EIO;
	}
	/* Polling QSPI idle status. */
	if (!cadence_qspi_wait_idle(reg_base))
		return -EIO;
	return 0;
}
/* For command RDID, RDSR. */
static int cadence_qspi_apb_command_read(void *reg_base,
	unsigned int txlen, const unsigned char *txbuf,
	unsigned addrlen, unsigned char *addrbuf,
	unsigned rxlen, unsigned char *rxbuf, unsigned int flash_type)
{
	unsigned int reg, i;
	unsigned int addr_value;
	int status;

	debug_print("%s txlen %d txbuf %p rxlen %d rxbuf %p\n",
		__func__, txlen, txbuf, rxlen, rxbuf);
#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi tx: ", DUMP_PREFIX_NONE,
		32, 1, txbuf, txlen, false);
#endif
	if (!rxlen || rxlen > CQSPI_STIG_DATA_LEN_MAX || rxbuf == NULL) {
		pr_err("QSPI: Invalid input argument, len %d rxbuf 0x%08x\n",
			rxlen, (unsigned int)rxbuf);
		return -EINVAL;
	}
	reg = txbuf[0] << CQSPI_REG_CMDCTRL_OPCODE_LSB;
	reg |= (0x1 << CQSPI_REG_CMDCTRL_RD_EN_LSB);
	if (addrlen) {
		/* Command with address */
		reg |= (0x1 << CQSPI_REG_CMDCTRL_ADDR_EN_LSB);
		/* Number of bytes to write. */
		reg |= ((addrlen - 1) & CQSPI_REG_CMDCTRL_ADD_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_ADD_BYTES_LSB;
		/* Get address */
		addr_value = cadence_qspi_apb_cmd2addr(&addrbuf[0], addrlen);
		CQSPI_WRITEL(addr_value, reg_base + CQSPI_REG_CMDADDRESS);
		debug_print("[%s] CQSPI_REG_CMDADDRESS=0x%x\n", __func__, addr_value);
	}
	/* 0 means 1 byte. */
	reg |= (((rxlen - 1) & CQSPI_REG_CMDCTRL_RD_BYTES_MASK)
		<< CQSPI_REG_CMDCTRL_RD_BYTES_LSB);
	status = cadence_qspi_apb_exec_flash_cmd(reg_base, reg);
	if (status != 0)
		return status;

	/* Put read value into rxbuf. STIG data is in LE format.
	 * We should not use direct pointer cast to ensure it works
	 * regardless of CPU endianness.
	 */
	for (i = 0; i < rxlen; i++) {
		if (i == 0) {
			reg = readl(reg_base + CQSPI_REG_CMDREADDATALOWER);
			debug_print("[%s] CQSPI_REG_CMDREADDATALOWER=0x%x\n",
				    __func__, reg);
		} else if (i == 4) {
			reg = readl(reg_base + CQSPI_REG_CMDREADDATAUPPER);
			debug_print("[%s] CQSPI_REG_CMDREADDATAUPPER=0x%x\n",
				    __func__, reg);
		}
		rxbuf[i] = reg & 0xFF;
		reg = reg >> 8;
	}

#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi rx: ", DUMP_PREFIX_NONE,
		       32, 1, rxbuf, rxlen, false);
#endif
	return 0;
}

/* For commands: WRSR, WREN, WRDI, CHIP_ERASE, BE, etc. */
static int cadence_qspi_apb_command_write(void *reg_base,
	unsigned txlen, const unsigned char *txbuf,
	unsigned addrlen, const unsigned char *addrbuf,
	unsigned datalen, const unsigned char *databuf, unsigned int flash_type)
{
	unsigned int reg;
	unsigned int addr_value;
	unsigned int data = 0;
	int i;

	debug_print("%s txlen %d txbuf[0]=0x%x\n", __func__, txlen, txbuf[0]);
#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi tx: ", DUMP_PREFIX_NONE,
		32, 1, txbuf, txlen, false);
#endif
	if (!txlen || txbuf == NULL) {
		pr_err("QSPI: Invalid input argument, cmdlen %d txbuf 0x%08x\n",
			txlen, (unsigned int)txbuf);
		return -EINVAL;
	}
	reg = txbuf[0] << CQSPI_REG_CMDCTRL_OPCODE_LSB;
	if (datalen != 0) {
		if (datalen <= 4) {
			/* Command with data only. */
			reg |= (0x1 << CQSPI_REG_CMDCTRL_WR_EN_LSB);
			reg |= ((datalen - 1) & CQSPI_REG_CMDCTRL_WR_BYTES_MASK)
				<< CQSPI_REG_CMDCTRL_WR_BYTES_LSB;
			for (i = 0; i < datalen; i++)
				data |= databuf[i] << (8 * i);

			/* Write the data */
			CQSPI_WRITEL(data,
				reg_base + CQSPI_REG_CMDWRITEDATALOWER);
			debug_print("[%s] CQSPI_REG_CMDWRITEDATALOWER=0x%x\n", __func__, data);
		} else {
			/* Command with data only. */
			reg |= (0x1 << CQSPI_REG_CMDCTRL_WR_EN_LSB);
			reg |= ((datalen - 1) & CQSPI_REG_CMDCTRL_WR_BYTES_MASK)
				<< CQSPI_REG_CMDCTRL_WR_BYTES_LSB;
			for (i = 0; i < 4; i++)
				data |= databuf[i] << (8 * i);

			/* Write the data */
			CQSPI_WRITEL(data,
				reg_base + CQSPI_REG_CMDWRITEDATALOWER);
			debug_print("[%s] CQSPI_REG_CMDWRITEDATALOWER=0x%x\n", __func__, data);

			data = 0;
			for (i = 0; i < datalen - 4; i++)
				data |= databuf[i + 4] << (8 * i);

			/* Write the data */
			CQSPI_WRITEL(data,
				reg_base + CQSPI_REG_CMDWRITEDATAUPPER);
			debug_print("[%s] CQSPI_REG_CMDWRITEDATAUPPER=0x%x\n", __func__, data);
		}
	}
	if (addrlen) {
		/* Command with address */
		reg |= (0x1 << CQSPI_REG_CMDCTRL_ADDR_EN_LSB);
		/* Number of bytes to write. */
		reg |= ((addrlen - 1) & CQSPI_REG_CMDCTRL_ADD_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_ADD_BYTES_LSB;
		/* Get address */
		addr_value = cadence_qspi_apb_cmd2addr(&addrbuf[0], addrlen);
		CQSPI_WRITEL(addr_value, reg_base + CQSPI_REG_CMDADDRESS);
		debug_print("[%s] CQSPI_REG_CMDADDRESS=0x%x\n", __func__, addr_value);
	}
#if 0
	else if ((txlen > 1) && (flash_type == QSPI_FLASH_TYPE_NOR)) {
		addrlen = txlen - 1;
		/* Command with address */
		reg |= (0x1 << CQSPI_REG_CMDCTRL_ADDR_EN_LSB);
		/* Number of bytes to write. */
		reg |= ((addrlen - 1) & CQSPI_REG_CMDCTRL_ADD_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_ADD_BYTES_LSB;
		/* Get address */
		addr_value = cadence_qspi_apb_cmd2addr(&txbuf[1],
			addrlen >= 5 ? 4 : 3);
		CQSPI_WRITEL(addr_value, reg_base + CQSPI_REG_CMDADDRESS);
		debug_print("[%s] CQSPI_REG_CMDADDRESS=0x%x\n", __func__, addr_value);
	}
#endif
	return cadence_qspi_apb_exec_flash_cmd(reg_base, reg);
}
static int cadence_qspi_apb_indirect_read_setup(void *reg_base,
	unsigned int ahb_phy_addr, unsigned txlen, const unsigned char *txbuf,
	unsigned addrlen, const unsigned char *addrbuf,
	unsigned int addr_bytes, unsigned int flash_type,
	unsigned int quad_mode)
{
	unsigned int reg;
	unsigned int addr_value;
	unsigned int dummy_clk;
	unsigned int dummy_bytes;

	debug_print("[%s]ahb_phy_addr=0x%x txlen=%d txbuf=0x%p "
		"addr_bytes=%d addrlen=%d\n",
		__func__, ahb_phy_addr, txlen, txbuf, addr_bytes, addrlen);
	if (txlen == 6)
		debug_print("[%s][0]=0x%x [1]=0x%x [2]=0x%x [3]=0x%x [4]=0x%x [5]=0x%x\n",
			__func__, *txbuf, *(txbuf + 1), *(txbuf + 2), *(txbuf + 3), *(txbuf + 4), *(txbuf + 5));

#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi tx: ", DUMP_PREFIX_NONE,
		32, 1, txbuf, txlen, false);
#endif
	CQSPI_WRITEL(ahb_phy_addr, reg_base + CQSPI_REG_INDIRECTTRIGGER);
	if (addrlen) {
		/* Get address */
		addr_value = cadence_qspi_apb_cmd2addr(&addrbuf[0], addr_bytes);
		CQSPI_WRITEL(addr_value,
			reg_base + CQSPI_REG_INDIRECTRDSTARTADDR);
		debug_print("[%s]CQSPI_REG_INDIRECTRDSTARTADDR=0x%x\n", __func__, addr_value);
	}
	/* The remaining lenght is dummy bytes. */
	if (txlen > addr_bytes)
		dummy_bytes = txlen - addr_bytes - 1;
	else
		dummy_bytes = 0;

	reg = CQSPI_READL(reg_base + CQSPI_REG_RD_INSTR);
	/* Configure the opcode */
	reg |= txbuf[0] << CQSPI_REG_RD_INSTR_OPCODE_LSB;
	/* Configure the quad */
	reg |= (quad_mode & CQSPI_REG_RD_INSTR_TYPE_DATA_MASK) << CQSPI_REG_RD_INSTR_TYPE_DATA_LSB;
	/* Setup dummy clock cycles */
	if (dummy_bytes) {
		if (dummy_bytes > CQSPI_DUMMY_BYTES_MAX)
			dummy_bytes = CQSPI_DUMMY_BYTES_MAX;
		reg |= (1 << CQSPI_REG_RD_INSTR_MODE_EN_LSB);
		/* Set all high to ensure chip doesn't enter XIP */
		CQSPI_WRITEL(0xFF, reg_base + CQSPI_REG_MODE_BIT);
		/* Convert to clock cycles. */
		dummy_clk = dummy_bytes * CQSPI_DUMMY_CLKS_PER_BYTE;
		/* Need to minus the mode byte (8 clocks). */
		dummy_clk -= CQSPI_DUMMY_CLKS_PER_BYTE;
		if (dummy_clk)
			reg |= (dummy_clk & CQSPI_REG_RD_INSTR_DUMMY_MASK)
				<< CQSPI_REG_RD_INSTR_DUMMY_LSB;
	} else if (flash_type == QSPI_FLASH_TYPE_NAND) {
		/* for qspi nand, we need at least 8 dummy cycles for rd */
		reg |= ((1 * CQSPI_DUMMY_CLKS_PER_BYTE)
				<< CQSPI_REG_RD_INSTR_DUMMY_LSB);
	}
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_RD_INSTR);
	debug_print("[%s]quad_mode=%d CQSPI_REG_RD_INSTR=0x%x\n", __func__, quad_mode, reg);
	/* Set device size */
	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (addr_bytes - 1);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_SIZE);
	debug_print("[%s]CQSPI_REG_SIZE=0x%x\n", __func__, reg);
	/* disable auto-polling */
	reg = CQSPI_READL(reg_base + CQSPI_REG_WRITE_COMPLETION_CTRL);
	reg |= (1 << WRITE_COMPLETION_CTRL_REG_DISABLE_POLLING_FLD_POS);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_WRITE_COMPLETION_CTRL);
	debug_print("[%s]CQSPI_REG_WRITE_COMPLETION_CTRL=0x%x\n", __func__, reg);

	return 0;
}
static int cadence_qspi_apb_indirect_read_execute(
	struct struct_cqspi *cadence_qspi, unsigned rxlen,
	unsigned char *rxbuf, unsigned int flash_type)
{
	unsigned int reg = 0;
	unsigned int timeout;
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	void *reg_base = cadence_qspi->iobase;
	void __iomem *ahb_base = cadence_qspi->qspi_ahb_virt;
	int remaining = (int)rxlen;
	int ret = 0;
	unsigned char *buf = rxbuf;

	debug_print("%s rxlen %d rxbuf %p\n", __func__, rxlen, rxbuf);
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_INDIRECTRDWATERMARK);

	CQSPI_WRITEL(0xa, reg_base + CQSPI_INDIRECT_TRIGGER_ADDR_RANGE_REG);

	CQSPI_WRITEL(remaining, reg_base + CQSPI_REG_INDIRECTRDBYTES);
	debug_print("[%s]CQSPI_REG_INDIRECTRDBYTES=0x%x\n", __func__, remaining);

	mb();

	CQSPI_WRITEL(pdata->fifo_depth - CQSPI_REG_SRAM_RESV_WORDS,
		reg_base + CQSPI_REG_SRAMPARTITION);
	/* Clear all interrupts. */
	CQSPI_WRITEL(CQSPI_IRQ_STATUS_MASK, reg_base + CQSPI_REG_IRQSTATUS);
	CQSPI_WRITEL(CQSPI_IRQ_MASK_RD, reg_base + CQSPI_REG_IRQMASK);

	/* Start the indirect read transfer */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTRD_START_MASK, reg_base + CQSPI_REG_INDIRECTRD);
	debug_print("[%s]CQSPI_REG_INDIRECTRD=0x%x\n", __func__, CQSPI_READL(reg_base + CQSPI_REG_INDIRECTRD));
	while (remaining > 0) {
			reg = CQSPI_GET_RD_SRAM_LEVEL(reg_base);
			/* convert to bytes */
			reg *= CQSPI_FIFO_WIDTH;
			reg = reg > remaining ? remaining : reg;

			/* Read data from FIFO in page burst */
			memcpy_fromio(buf, ahb_base, reg);

			buf += reg;
			remaining -= reg;
	}
	/* Check indirect done status */
	timeout = cadence_qspi_init_timeout(CQSPI_TIMEOUT_MS);
	while (cadence_qspi_check_timeout(timeout)) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_INDIRECTRD);
		if (reg & CQSPI_REG_INDIRECTRD_DONE_MASK)
			break;
	}
	if (!(reg & CQSPI_REG_INDIRECTRD_DONE_MASK)) {
		pr_err("QSPI : Indirect read completion status error with "
			"reg 0x%08x\n", reg);
		ret = -ETIMEDOUT;
		goto failrd;
	}
	/* Disable interrupt */
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_IRQMASK);
	/* Clear indirect completion status */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTRD_DONE_MASK,
		reg_base + CQSPI_REG_INDIRECTRD);
#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi rx: ", DUMP_PREFIX_NONE,
		32, 1, rxbuf, rxlen, false);
#endif
	return 0;
failrd:
	/* Disable interrupt */
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_IRQMASK);
	/* Cancel the indirect read */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
			reg_base + CQSPI_REG_INDIRECTRD);
	return ret;
}
static int cadence_qspi_apb_indirect_write_setup(void *reg_base,
	unsigned int ahb_phy_addr, unsigned txlen, const unsigned char *txbuf,
	unsigned addrlen, const unsigned char *addrbuf, unsigned int flash_type,
	unsigned int quad_mode)
{
	unsigned int reg;
	unsigned int addr_bytes = txlen;

	debug_print("%s txlen %d txbuf %p addr_bytes %d\n",
		__func__, txlen, txbuf, addr_bytes);
#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi tx: ", DUMP_PREFIX_NONE,
		32, 1, txbuf, txlen, false);
#endif
	if (txbuf == NULL) {
		pr_err("QSPI: Invalid input argument, txlen %d txbuf 0x%08x\n",
			txlen, (unsigned int)txbuf);
		return -EINVAL;
	}
	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~(CQSPI_REG_CONFIG_DIRECT_MASK);
	reg &= ~(CQSPI_REG_CONFIG_DMA_MASK);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);

	CQSPI_WRITEL(ahb_phy_addr, reg_base + CQSPI_REG_INDIRECTTRIGGER);
	/* Set opcode. */
	reg = txbuf[0] << CQSPI_REG_WR_INSTR_OPCODE_LSB;
	reg |= (1 << DEV_INSTR_WR_CONFIG_REG_WEL_DIS_FLD_POS);
	if ((txbuf[0] == MACRONIX_4PP_CMD) && (flash_type == QSPI_FLASH_TYPE_NOR) && (quad_mode == QUAD_QIO)) {
		/* Configure the quad */
		reg |= (quad_mode & CQSPI_REG_WR_INSTR_TYPE_ADDR_MASK) << CQSPI_REG_WR_INSTR_TYPE_ADDR_LSB;
	}
	/* Configure the quad */
	reg |= (quad_mode & CQSPI_REG_WR_INSTR_TYPE_DATA_MASK) << CQSPI_REG_WR_INSTR_TYPE_DATA_LSB;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_WR_INSTR);
	debug_print("[%s]quad_mode=%d CQSPI_REG_WR_INSTR=0x%x\n", __func__, quad_mode, reg);
	/* Setup write address. */
	reg = cadence_qspi_apb_cmd2addr(&addrbuf[0], addrlen);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_INDIRECTWRSTARTADDR);
	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= ((addrlen - 1) & CQSPI_REG_SIZE_ADDRESS_MASK);
	CQSPI_WRITEL(reg, reg_base +  CQSPI_REG_SIZE);

	/* disable auto-polling */
	reg = CQSPI_READL(reg_base + CQSPI_REG_WRITE_COMPLETION_CTRL);
	reg |= (1 << 14);
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_WRITE_COMPLETION_CTRL);
	return 0;
}
static int cadence_qspi_apb_indirect_write_execute(
	struct struct_cqspi *cadence_qspi, unsigned txlen,
	const unsigned char *txbuf, unsigned int flash_type)
{
	int ret = 0;
	unsigned int timeout;
	unsigned int reg = 0;
	unsigned int *irq_status = &(cadence_qspi->irq_status);
	void *reg_base = cadence_qspi->iobase;
	void __iomem *ahb_base = cadence_qspi->qspi_ahb_virt;
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	struct cqspi_flash_pdata *f_pdata =
			&(pdata->f_pdata[cadence_qspi->current_cs]);
	unsigned int page_size = f_pdata->page_size;
	int remaining = (int)txlen;
	unsigned int write_bytes;
	uint16_t fill_level = 0;

	debug_print("%s txlen %d txbuf %p\n", __func__, txlen, txbuf);
#ifdef DEBUG
	/* Dump the QSPI command */
	print_hex_dump(KERN_DEBUG, "qspi tx: ", DUMP_PREFIX_NONE,
		32, 1, txbuf, txlen, false);
#endif
	CQSPI_WRITEL(0xa, reg_base + CQSPI_INDIRECT_TRIGGER_ADDR_RANGE_REG);
	CQSPI_WRITEL(0x0, reg_base + CQSPI_REG_INDIRECTWRWATERMARK);
	reg = CQSPI_READL(reg_base + CQSPI_REG_SIZE);
	reg &= ~(CQSPI_REG_SIZE_PAGE_MASK << CQSPI_REG_SIZE_PAGE_LSB);
	reg &= ~(CQSPI_REG_SIZE_BLOCK_MASK << CQSPI_REG_SIZE_BLOCK_LSB);
	reg |= (f_pdata->page_size << CQSPI_REG_SIZE_PAGE_LSB);
	reg |= (f_pdata->block_size << CQSPI_REG_SIZE_BLOCK_LSB);
	CQSPI_WRITEL(reg, reg_base +  CQSPI_REG_SIZE);

	CQSPI_WRITEL(remaining, reg_base + CQSPI_REG_INDIRECTWRBYTES);
	CQSPI_WRITEL(CQSPI_REG_SRAM_PARTITION_WR,
		     reg_base + CQSPI_REG_SRAMPARTITION);
	/* Clear all interrupts. */
	CQSPI_WRITEL(CQSPI_IRQ_STATUS_MASK, reg_base + CQSPI_REG_IRQSTATUS);
	CQSPI_WRITEL(CQSPI_IRQ_MASK_WR, reg_base + CQSPI_REG_IRQMASK);
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_START_MASK,
			reg_base + CQSPI_REG_INDIRECTWR);

	while (remaining > 0) {
		/* Calculate number of bytes to write. */
		write_bytes = remaining > page_size ? page_size : remaining;

		/* Write in page burst */
		memcpy_toio(ahb_base, txbuf, write_bytes);
		ret = wait_event_interruptible_timeout(cadence_qspi->waitqueue,
						       *irq_status & CQSPI_IRQ_MASK_WR,
						       CQSPI_TIMEOUT_MS);
		if (!ret) {
			pr_err("QSPI: Indirect write timeout\n");
			ret = -ETIMEDOUT;
			goto failwr;
		} else {
			ret = 0;
		}

		if (*irq_status & CQSPI_IRQ_STATUS_ERR) {
			/* Error occurred */
			pr_err("QSPI: Indirect write error IRQ status 0x%08x\n",
			       *irq_status);
			ret = -EPERM;
			goto failwr;
		}

		txbuf  += write_bytes;
		remaining -= write_bytes;
	}

	/* Check indirect done status */
	timeout = cadence_qspi_init_timeout(CQSPI_TIMEOUT_MS);
	while (cadence_qspi_check_timeout(timeout)) {
		reg = CQSPI_READL(reg_base + CQSPI_REG_INDIRECTWR);
		if (reg & CQSPI_REG_INDIRECTWR_DONE_MASK)
			break;
	}
	if (!(reg & CQSPI_REG_INDIRECTWR_DONE_MASK)) {
		pr_err("QSPI: Indirect write completion status error with "
			"reg 0x%08x\n", reg);
		ret = -ETIMEDOUT;
		goto failwr;
	}

	/* We observe issues in underrun condition (i.e. in high SPI clock
	 * or low CPU clock) in which write transfer fail in
	 * between, which eventually causes issues at higher layer (e.g. file
	 * system corruption). To workaround, we check the sram fill level
	 * after write. If it is not zero, we assume transfer failure, do hard
	 * recovery, and return -EAGAIN so that user layer can retry operation
	 * in a clean way.
	 */
	fill_level = (((CQSPI_READL(reg_base + CQSPI_REG_SDRAMLEVEL)) >>
		       CQSPI_REG_SDRAMLEVEL_WR_LSB) &
		      CQSPI_REG_SDRAMLEVEL_WR_MASK);
	if (fill_level) {
		pr_debug("%s fill level is %u\n", __func__, fill_level);

		CQSPI_WRITEL(0, reg_base + CQSPI_REG_IRQMASK);
		CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_DONE_MASK,
			     reg_base + CQSPI_REG_INDIRECTWR);
		CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
			     reg_base + CQSPI_REG_INDIRECTWR);

		cadence_qspi_apb_controller_init(cadence_qspi);
		return -EAGAIN;
	}

failwr:
	/* Disable interrupt. */
	CQSPI_WRITEL(0, reg_base + CQSPI_REG_IRQMASK);
	/* Clear indirect completion status */
	CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_DONE_MASK,
		reg_base + CQSPI_REG_INDIRECTWR);

	/* Cancel the indirect write */
	if (ret)
		CQSPI_WRITEL(CQSPI_REG_INDIRECTWR_CANCEL_MASK,
			     reg_base + CQSPI_REG_INDIRECTWR);

	return ret;
}
void cadence_qspi_apb_controller_enable(void *reg_base)
{
	unsigned int reg;

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg |= CQSPI_REG_CONFIG_ENABLE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	return;
}
void cadence_qspi_apb_controller_disable(void *reg_base)
{
	unsigned int reg;

	reg = CQSPI_READL(reg_base + CQSPI_REG_CONFIG);
	reg &= ~CQSPI_REG_CONFIG_ENABLE_MASK;
	CQSPI_WRITEL(reg, reg_base + CQSPI_REG_CONFIG);
	return;
}
unsigned int cadence_qspi_apb_is_controller_ready(void *reg_base)
{
	return cadence_qspi_wait_idle(reg_base);
}

void cadence_qspi_apb_controller_init(struct struct_cqspi *cadence_qspi)
{
	unsigned int reg;
	int ret;

	/* reset */
	reset_control_assert(cadence_qspi->reset);
	udelay(1);
	reset_control_deassert(cadence_qspi->reset);

	cadence_qspi_apb_controller_disable(cadence_qspi->iobase);

	/* phy mode initialization */
	CQSPI_WRITEL(0, cadence_qspi->iobase + CQSPI_PHY_CONFIG_REG);

	reg = 0x14 << CQSPI_PHY_MASTER_INITIAL_DELAY_LSB;
	CQSPI_WRITEL(reg, cadence_qspi->iobase + CQSPI_PHY_MASTER_CONTROL_REG);

	reg = 0x4 << CQSPI_PHY_CONFIG_TX_DLL_DELAY_LSB;
	reg |= 0x1 << CQSPI_PHY_CONFIG_RESYNC_LSB;
	reg |= 0x1 << CQSPI_PHY_CONFIG_RESET_LSB;
	CQSPI_WRITEL(reg, cadence_qspi->iobase + CQSPI_PHY_CONFIG_REG);

	ret = readl_poll_timeout(cadence_qspi->iobase +
				 CQSPI_DLL_OBSERVABLE_LOWER_REG, reg,
				 reg & CQSPI_DLL_OBSERVABLE_LOWER_REG,
				 1, 100);
	if (ret)
		dev_warn(&cadence_qspi->pdev->dev,
			 "timeout waiting for qspi dll lock\n");

	/* Configure the remap address register, no remap */
	CQSPI_WRITEL(0, cadence_qspi->iobase + CQSPI_REG_REMAP);
	/* Disable all interrupts. */
	CQSPI_WRITEL(0, cadence_qspi->iobase + CQSPI_REG_IRQMASK);

	cadence_qspi_apb_controller_enable(cadence_qspi->iobase);
	return;
}

unsigned int calculate_ticks_for_ns(unsigned int ref_clk_hz,
	unsigned int ns_val)
{
	unsigned int ticks;

	ticks = ref_clk_hz;
	ticks /= 1000;
	ticks *= ns_val;
	ticks +=  999999;
	ticks /= 1000000;
	return ticks;
}
void cadence_qspi_apb_delay(struct struct_cqspi *cadence_qspi,
	unsigned int ref_clk, unsigned int sclk_hz)
{
	void __iomem *iobase = cadence_qspi->iobase;
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	struct cqspi_flash_pdata *f_pdata =
			&(pdata->f_pdata[cadence_qspi->current_cs]);
	unsigned int ref_clk_ns;
	unsigned int sclk_ns;
	unsigned int tshsl, tchsh, tslch, tsd2d;
	unsigned int reg;
	unsigned int tsclk;

	debug_print("%s %d %d\n", __func__, ref_clk, sclk_hz);

	cadence_qspi_apb_controller_disable(cadence_qspi->iobase);
	/* Convert to ns. */
	ref_clk_ns = (1000000000) / pdata->master_ref_clk_hz;
	/* Convert to ns. */
	sclk_ns = (1000000000) / sclk_hz;
	/* calculate the number of ref ticks for one sclk tick */
	tsclk = (pdata->master_ref_clk_hz + sclk_hz - 1) / sclk_hz;
	tshsl = calculate_ticks_for_ns(pdata->master_ref_clk_hz,
		f_pdata->tshsl_ns);
	/* this particular value must be at least one sclk */
	if (tshsl < tsclk)
		tshsl = tsclk;
	tchsh = calculate_ticks_for_ns(pdata->master_ref_clk_hz,
		f_pdata->tchsh_ns);
	tslch = calculate_ticks_for_ns(pdata->master_ref_clk_hz,
		f_pdata->tslch_ns);
	tsd2d = calculate_ticks_for_ns(pdata->master_ref_clk_hz,
		f_pdata->tsd2d_ns);
	debug_print("%s tshsl %d tsd2d %d tchsh %d tslch %d\n",
		__func__, tshsl, tsd2d, tchsh, tslch);
	reg = ((tshsl & CQSPI_REG_DELAY_TSHSL_MASK)
			<< CQSPI_REG_DELAY_TSHSL_LSB);
	reg |= ((tchsh & CQSPI_REG_DELAY_TCHSH_MASK)
			<< CQSPI_REG_DELAY_TCHSH_LSB);
	reg |= ((tslch & CQSPI_REG_DELAY_TSLCH_MASK)
			<< CQSPI_REG_DELAY_TSLCH_LSB);
	reg |= ((tsd2d & CQSPI_REG_DELAY_TSD2D_MASK)
			<< CQSPI_REG_DELAY_TSD2D_LSB);
	CQSPI_WRITEL(reg, iobase + CQSPI_REG_DELAY);
	cadence_qspi_apb_controller_enable(cadence_qspi->iobase);
	return;
}
void cadence_qspi_switch_cs(struct struct_cqspi *cadence_qspi,
	unsigned int cs)
{
	unsigned int reg;
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	struct cqspi_flash_pdata *f_pdata = &(pdata->f_pdata[cs]);
	void __iomem *iobase = cadence_qspi->iobase;

	debug_print("%s\n", __func__);
	cadence_qspi_apb_controller_disable(iobase);
	/* Configure page size and block size. */
	reg = CQSPI_READL(iobase + CQSPI_REG_SIZE);
	/* clear the previous value */
	reg &= ~(CQSPI_REG_SIZE_PAGE_MASK << CQSPI_REG_SIZE_PAGE_LSB);
	reg &= ~(CQSPI_REG_SIZE_BLOCK_MASK << CQSPI_REG_SIZE_BLOCK_LSB);
	reg |= (f_pdata->page_size << CQSPI_REG_SIZE_PAGE_LSB);
	reg |= (f_pdata->block_size << CQSPI_REG_SIZE_BLOCK_LSB);
	CQSPI_WRITEL(reg, iobase + CQSPI_REG_SIZE);
	debug_print("[%s]f_pdata->page_size=0x%x f_pdata->block_size=0x%x\n",
		__func__, f_pdata->page_size, f_pdata->block_size);
	debug_print("[%s]CQSPI_REG_SIZE=0x%x\n", __func__,
		CQSPI_READL(iobase + CQSPI_REG_SIZE));
	/* configure the chip select */
	cadence_qspi_apb_chipselect(iobase, cs, pdata->ext_decoder);
	cadence_qspi_apb_controller_enable(iobase);
	return;
}
int cadence_qspi_apb_process_queue(struct struct_cqspi *cadence_qspi,
				  struct spi_device *spi, unsigned int n_trans,
				  struct spi_transfer **spi_xfer)
{
	struct platform_device *pdev = cadence_qspi->pdev;
	struct cqspi_platform_data *pdata = pdev->dev.platform_data;
	struct spi_transfer *cmd_xfer = spi_xfer[0];
	struct spi_transfer *data_xfer = (n_trans >= 2) ? spi_xfer[1] : NULL;
	void __iomem *iobase = cadence_qspi->iobase;
	unsigned int sclk;
	/* 1 byte coomand, 3 bytes address, 1 byte dummy */
	unsigned int mode = DC_MODE;
	int ret = 0;
	struct cqspi_flash_pdata *f_pdata;

	f_pdata = &(pdata->f_pdata[cadence_qspi->current_cs]);
	debug_print("[%s] flash_type=%d cadence_qspi->current_cs=%d\n", __func__, f_pdata->flash_type, cadence_qspi->current_cs);
	debug_print("[%s] page_size=%d block_size=%d rd quad=%d, wr quad=%d\n",
		    __func__, f_pdata->page_size, f_pdata->block_size,
		    f_pdata->rd_quad, f_pdata->wr_quad);
	debug_print("[%s] n_trans=%d\n", __func__, n_trans);
	debug_print("[%s] cmd_xfer->len=%d\n", __func__, cmd_xfer->len);
	debug_print("[%s] cmd_xfer->tx_buf=0x%p\n", __func__, cmd_xfer->tx_buf);
	debug_print("[%s] cmd_xfer->rx_buf=0x%p\n", __func__, cmd_xfer->rx_buf);
	debug_print("[%s] data_xfer=0x%p\n", __func__, data_xfer);

	if (!cmd_xfer->len) {
		pr_err("QSPI: SPI transfer length is 0.\n");
		return -EINVAL;
	}

	/* Setup baudrate divisor and delays */
	sclk = cmd_xfer->speed_hz ?
		cmd_xfer->speed_hz : spi->max_speed_hz;
	cadence_qspi_apb_controller_disable(iobase);
	cadence_qspi_apb_config_baudrate_div(iobase,
		pdata->master_ref_clk_hz, sclk);
	/* this set delay function need work with device tree */
	cadence_qspi_apb_delay(cadence_qspi,
		pdata->master_ref_clk_hz, sclk);
	cadence_qspi_apb_readdata_capture(iobase, 1,
		f_pdata->read_delay);
	cadence_qspi_apb_controller_enable(iobase);

	/* Switch chip select. */
	if (cadence_qspi->current_cs != spi->chip_select) {
		cadence_qspi->current_cs = spi->chip_select;
		cadence_qspi_switch_cs(cadence_qspi, spi->chip_select);
	}
	if (f_pdata->flash_type == QSPI_FLASH_TYPE_NOR) {
		if (n_trans == 2) {
			if ((data_xfer->len > 6) && (data_xfer->rx_buf))
				mode = IDC_READ_MODE;
			else if ((data_xfer->len > 6) && (data_xfer->tx_buf))
				mode = IDC_WRITE_MODE;
			else if (data_xfer->tx_buf)
				mode = STIG_WRITE_MODE;
			else if (data_xfer->rx_buf)
				mode = STIG_READ_MODE;
		} else if (n_trans == 1)
			mode = STIG_WRITE_MODE;
	} else if (f_pdata->flash_type == QSPI_FLASH_TYPE_NAND) {
		/*
		 * Use STIG command to send if the transfer length is less than
		 * 4 or if only one transfer.
		 */
		if (n_trans >= 3) {
			if ((spi_xfer[2]->len > 4) && (spi_xfer[2]->rx_buf))
				mode = IDC_READ_MODE;
			else if ((spi_xfer[2]->len > 4) && (spi_xfer[2]->tx_buf))
				mode = IDC_WRITE_MODE;
			else if ((spi_xfer[2]->len < 4) && (spi_xfer[2]->tx_buf))
				mode = STIG_WRITE_MODE;
			else if ((spi_xfer[2]->len < 4) && (spi_xfer[2]->rx_buf))
				mode = STIG_READ_MODE;
		} else if (n_trans == 2) {
			if ((data_xfer->len > 4) && (data_xfer->rx_buf))
				mode = IDC_READ_MODE;
			else if ((data_xfer->len > 4) && (data_xfer->tx_buf))
				mode = IDC_WRITE_MODE;
			else if (data_xfer->tx_buf)
				mode = STIG_WRITE_MODE;
			else if (data_xfer->rx_buf)
				mode = STIG_READ_MODE;
		} else if (n_trans == 1)
			mode = STIG_WRITE_MODE;
	}

	if (mode == STIG_WRITE_MODE) {
		/* STIG write */
		if ((n_trans == 3) && data_xfer && spi_xfer[2])
			ret = cadence_qspi_apb_command_write(iobase,
				cmd_xfer->len, cmd_xfer->tx_buf,
				data_xfer->len, data_xfer->tx_buf,
				spi_xfer[2]->len, spi_xfer[2]->tx_buf, f_pdata->flash_type);
		else if ((n_trans == 2) && data_xfer)
			ret = cadence_qspi_apb_command_write(iobase,
				cmd_xfer->len, cmd_xfer->tx_buf,
				data_xfer->len, data_xfer->tx_buf, 0, NULL, f_pdata->flash_type);
		else
			ret = cadence_qspi_apb_command_write(iobase,
				cmd_xfer->len, cmd_xfer->tx_buf, cmd_xfer->len - 1,
				cmd_xfer->tx_buf + 1, 0, NULL, f_pdata->flash_type);
	} else if (mode == STIG_READ_MODE) {
		/* STIG read */
		if ((n_trans == 3) && data_xfer && spi_xfer[2])
			ret = cadence_qspi_apb_command_read(iobase,
				cmd_xfer->len, (const unsigned char *)cmd_xfer->tx_buf,
				data_xfer->len, (unsigned char *)data_xfer->tx_buf,
				spi_xfer[2]->len, spi_xfer[2]->rx_buf, f_pdata->flash_type);
		else if ((n_trans == 2) && data_xfer)
			ret = cadence_qspi_apb_command_read(iobase,
				cmd_xfer->len, cmd_xfer->tx_buf,
				0, NULL, data_xfer->len, data_xfer->rx_buf, f_pdata->flash_type);
	} else if (mode == IDC_READ_MODE) {
		/* Indirect operation */
		/* write command and address byte */
		/* Indirect read */
		if (f_pdata->flash_type == QSPI_FLASH_TYPE_NOR)
			ret = cadence_qspi_apb_indirect_read_setup(iobase,
				pdata->qspi_ahb_phy & pdata->qspi_ahb_mask,
				cmd_xfer->len, cmd_xfer->tx_buf,
				cmd_xfer->len - 1, cmd_xfer->tx_buf + 1, 3, f_pdata->flash_type,
				f_pdata->rd_quad);
		else
			/* read data from cache len should be 2 */
			ret = cadence_qspi_apb_indirect_read_setup(iobase,
				pdata->qspi_ahb_phy & pdata->qspi_ahb_mask,
				cmd_xfer->len, cmd_xfer->tx_buf,
				data_xfer->len, data_xfer->tx_buf, 2, f_pdata->flash_type,
				f_pdata->rd_quad);

		if (n_trans == 3) {
			ret = cadence_qspi_apb_indirect_read_execute(
			cadence_qspi, spi_xfer[2]->len, spi_xfer[2]->rx_buf, f_pdata->flash_type);
		} else {
			ret = cadence_qspi_apb_indirect_read_execute(
			cadence_qspi, data_xfer->len,
			data_xfer->rx_buf, f_pdata->flash_type);
		}
	} else if (mode == IDC_WRITE_MODE) {
		/* Indirect write */
		if (f_pdata->flash_type == QSPI_FLASH_TYPE_NOR) {
			ret = cadence_qspi_apb_indirect_write_setup(
				iobase,
				pdata->qspi_ahb_phy & pdata->qspi_ahb_mask,
				data_xfer->len, cmd_xfer->tx_buf,
				cmd_xfer->len - 1, cmd_xfer->tx_buf + 1, f_pdata->flash_type,
				f_pdata->wr_quad);

			ret = cadence_qspi_apb_indirect_write_execute(
				cadence_qspi, spi_xfer[1]->len,
				spi_xfer[1]->tx_buf, f_pdata->flash_type);
		} else {
			ret = cadence_qspi_apb_indirect_write_setup(
				iobase,
				pdata->qspi_ahb_phy & pdata->qspi_ahb_mask,
				cmd_xfer->len, cmd_xfer->tx_buf,
				data_xfer->len, data_xfer->tx_buf,
				f_pdata->flash_type, f_pdata->wr_quad);
			ret = cadence_qspi_apb_indirect_write_execute(
				cadence_qspi, spi_xfer[2]->len,
				spi_xfer[2]->tx_buf, f_pdata->flash_type);
		}
	} else {
		pr_err("QSPI : Unknown SPI transfer.\n");
		return -EINVAL;
	}
	return ret;
}
MODULE_LICENSE("Dual BSD/GPL");
