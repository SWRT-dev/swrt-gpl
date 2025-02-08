/*
 * (C) Copyright 2007 Michal Simek
 *
 * Michal  SIMEK <monstr@monstr.eu>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This is a board specific file.  It's OK to include board specific
 * header files */

#include <common.h>
#include <config.h>
#include <netdev.h>
#include <asm/processor.h>
#include <asm/microblaze_intc.h>
#include <asm/asm.h>
#include <asm/gpio.h>

#ifdef CONFIG_XILINX_GPIO
static int reset_pin = -1;
#endif

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#ifdef CONFIG_XILINX_GPIO
	if (reset_pin != -1)
		gpio_direction_output(reset_pin, 1);
#endif

#ifdef CONFIG_XILINX_TB_WATCHDOG
	hw_watchdog_disable();
#endif

	puts ("Reseting board\n");
	__asm__ __volatile__ ("	mts rmsr, r0;" \
				"bra r0");

	return 0;
}

int gpio_init (void)
{
#ifdef CONFIG_XILINX_GPIO
	reset_pin = gpio_alloc(CONFIG_SYS_GPIO_0_ADDR, "reset", 1);
	if (reset_pin != -1)
		gpio_request(reset_pin, "reset_pin");
#endif
	return 0;
}

void board_init(void)
{
	gpio_init();
}

int board_eth_init(bd_t *bis)
{
	int ret = 0;

#ifdef CONFIG_XILINX_AXIEMAC
	ret |= xilinx_axiemac_initialize(bis, XILINX_AXIEMAC_BASEADDR,
						XILINX_AXIDMA_BASEADDR);
#endif

#ifdef CONFIG_XILINX_EMACLITE
	u32 txpp = 0;
	u32 rxpp = 0;
# ifdef CONFIG_XILINX_EMACLITE_TX_PING_PONG
	txpp = 1;
# endif
# ifdef CONFIG_XILINX_EMACLITE_RX_PING_PONG
	rxpp = 1;
# endif
	ret |= xilinx_emaclite_initialize(bis, XILINX_EMACLITE_BASEADDR,
			txpp, rxpp);
#endif

#ifdef CONFIG_XILINX_LL_TEMAC
# ifdef XILINX_LLTEMAC_BASEADDR
#  ifdef XILINX_LLTEMAC_FIFO_BASEADDR
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR,
			XILINX_LL_TEMAC_M_FIFO, XILINX_LLTEMAC_FIFO_BASEADDR);
#  elif XILINX_LLTEMAC_SDMA_CTRL_BASEADDR
#   if XILINX_LLTEMAC_SDMA_USE_DCR == 1
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR,
			XILINX_LL_TEMAC_M_SDMA_DCR,
			XILINX_LLTEMAC_SDMA_CTRL_BASEADDR);
#   else
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR,
			XILINX_LL_TEMAC_M_SDMA_PLB,
			XILINX_LLTEMAC_SDMA_CTRL_BASEADDR);
#   endif
#  endif
# endif
# ifdef XILINX_LLTEMAC_BASEADDR1
#  ifdef XILINX_LLTEMAC_FIFO_BASEADDR1
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR1,
			XILINX_LL_TEMAC_M_FIFO, XILINX_LLTEMAC_FIFO_BASEADDR1);
#  elif XILINX_LLTEMAC_SDMA_CTRL_BASEADDR1
#   if XILINX_LLTEMAC_SDMA_USE_DCR == 1
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR1,
			XILINX_LL_TEMAC_M_SDMA_DCR,
			XILINX_LLTEMAC_SDMA_CTRL_BASEADDR1);
#   else
	ret |= xilinx_ll_temac_eth_init(bis, XILINX_LLTEMAC_BASEADDR1,
			XILINX_LL_TEMAC_M_SDMA_PLB,
			XILINX_LLTEMAC_SDMA_CTRL_BASEADDR1);
#   endif
#  endif
# endif
#endif

	return ret;
}
