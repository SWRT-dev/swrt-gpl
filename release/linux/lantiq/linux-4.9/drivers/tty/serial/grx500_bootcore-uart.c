/*
 *  linux/drivers/char/8250.c
 *
 *  Driver for grx500_bootcore serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *  $Id: grx500_bootcore.c
 *
 * A note about mapbase / membase
 *
 *  mapbase is the physical address of the IO port.
 *  membase is an 'ioremapped' cookie.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_reg.h>
//#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/nmi.h>
#include <linux/mutex.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/serial.h>
#include <asm/setup.h>

#include <linux/serial_core.h>
/*change name: ?*/
#include <grx500_bootcore_defs.h>
#include <grx500_bootcore_cnfg.h>
#include <grx500_bootcore_chadr.h>
#include <grx500_bootcore_chipreg.h>
#include <grx500_bootcore_emerald_env_regs.h>
#include <grx500_bootcore_uart.h>
#include <grx500_bootcore_time.h>
#include <grx500_bootcore_interrupt.h>


#define WWPATCH


#define UART_NR			1	/* only use one port */

int uart_done = 0;


static struct irqaction uart_irqaction = {
      .handler        = uart_interrupt,
      .flags          = 0x0, /* for disable nested interrupts */
    	/* Lior.H - when we need to use-> IRQF_NOBALANCING ? */
      .name           = "grx500_bootcore_uart",
};


static void grx500_bootcore_console_write(struct console *co, const char *s, unsigned count);
static void grx500_bootcore_start_tx(struct uart_port *port);

/*uart mux to host*/
#define REG_UART_CONTROL_MUX_MPU_BIT 0x100

#define uart_full_hw_buffer() (((MT_RdReg(UART_BASE, REG_UART_BYTES_IN_TX_FIFO) & UART_FWL_TX_MASK)>>5) == 16)
#define uart_get_char() (MT_RdReg(UART_BASE, REG_UART_READ_DATA) & UART_RD_DATA_MASK)
/****************************************************************************
* grx500_bootcore_config_hw
*
* REG_UART_FIFO_WATER_LEVEL (0x18):
* REG_UART_RX_FIFO_WATER_LEVEL - [mask = 0x1f]
* REG_UART_TX_FIFO_WATER_LEVEL - [mask = 0x3e0]
*
* REG_UART_INT (0x1c):
* Those  registers define the UART water level for RX and TX FIFOs
* The register is split to 2 section, mask_int and status int (clear write 1)
* interesting mask:
*   UART_read_FIFO_water_level_IRQ_enable - [mask =  0x20]
*   UART_write_FIFO_water_level_IRQ_enable - [mask =  0x40] 
* interesting status:
*   UART_read_FIFO_water_level_IRQ - [mask =  0x2000]
*   UART_write_FIFO_water_level_IRQ - [mask = 0x4000]
*/
void grx500_bootcore_config_hw(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
	uint32 temp = 0;
	
	/* disable interrupts */
    MT_WrReg(UART_BASE,REG_UART_INT, 0x00); 

	/* set up baud rate */
	{ 
		uint32 divisor;

		/* set divisor - clock 40mHz assumed */
//#ifndef CONFIG_VBG400_CHIPIT Orig
#ifdef CONFIG_VBG400_CHIPIT
		divisor = ((GRX500_BOOTCORE_SYSTEM_CLK) / (16*baud)) - 1;
#else
        divisor = ((GRX500_BOOTCORE_SYSTEM_CLK/GRX500_BOOTCORE_SCALE_VAL) / (16*baud)) - 1;
#endif
		MT_WrReg(UART_BASE,REG_UART_CLOCK_DIVISION_RATIO, divisor & 0x3fff);
 	}

	/* set data format */
	MT_WrReg(UART_BASE,REG_UART_CONTROL, data|parity|stop|REG_UART_CONTROL_MUX_MPU_BIT); 
	MT_WrReg(UART_BASE,REG_UART_RX_IDLE_COUNTER, 90); //10 time one byte duration

    /*============ Set WATER_L ======================================== */
    temp = 0 << REG_UART_TX_FIFO_WATER_LEVEL_SHIFT;  //TX
    temp |= (7 << REG_UART_RX_FIFO_WATER_LEVEL_SHIFT); //RX
    MT_WrReg(UART_BASE,REG_UART_RX_FIFO_WATER_LEVEL, temp); //same register as TX

	/*============ Set allowed interrupts ==================================== */
    temp = (1 << REG_UART_READ_FIFO_WATER_LEVEL_IRQ_ENABLE_SHIFT);   //RX
    temp |= 1 << REG_UART_WRITE_FIFO_WATER_LEVEL_IRQ_ENABLE_SHIFT; //TX
    temp |= (1 << REG_UART_IDLE_IRQ_ENABLE_SHIFT);                   //IDLE
	
	MT_WrReg(UART_BASE,REG_UART_INT,temp);

}


static void grx500_bootcore_stop_tx(struct uart_port *port)
{
}

static void grx500_bootcore_stop_rx(struct uart_port *port)
{
}

static void grx500_bootcore_enable_ms(struct uart_port *port)
{
	return;
}

/********************************************************
* Name: grx500_bootcore_uart_tx_isr
*
* Description: same handling as in start_tx clb.
* if there is data pending, send it out.
*
* TODO - if WL > 0, use timer in order to clear FIFO in polling mode.
*/
void grx500_bootcore_uart_tx_isr(struct uart_port *port)
{
/*    struct console *co = NULL;
    const char *s = NULL;
    unsigned count = 0;
    grx500_bootcore_start_tx(co, s, count); */
	 grx500_bootcore_start_tx(port);
}

/********************************************************
* Name: grx500_bootcore_uart_rx_isr
*
* Description: data in HW fifo.
* send it up to the core layer.
*
* UART_read_FIFO_water_level_IRQ_enable - [mask =  0x20]
*/
void grx500_bootcore_uart_rx_isr(struct uart_port *port)
{
	unsigned char index, len;
	unsigned char char_in;
    char flag;
	unsigned int status;

	len = (unsigned char)UART_MCOR_RD_RX_BYTES;
	flag = TTY_NORMAL;
	/* TODO- error handling here...
	if ()..
	  flag = TTY_BREAK;
      etc...
    */
    status = 0;
	for (index=0; index <len; index++)
	{
		//char_in = (unsigned char) MT_RdReg( UART_BASE, REG_UART_READ_DATA) & UART_RD_DATA_MASK;
		char_in = (unsigned char) uart_get_char();
		port->icount.rx++;
	    uart_insert_char(port, status, UART_LSR_OE, char_in, flag);
	}

//	tty_flip_buffer_push(port->info->tty);
//huanx	tty_flip_buffer_push(port->state->port.tty);
	tty_flip_buffer_push(&port->state->port);
}


static unsigned int grx500_bootcore_tx_empty(struct uart_port *port)
{
	return 0;
}

static unsigned int grx500_bootcore_get_mctrl(struct uart_port *port)
{
	return 0;
}

static void grx500_bootcore_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	return;
}

static void grx500_bootcore_break_ctl(struct uart_port *port, int break_state)
{
    return;
}

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

/*static*/ int grx500_bootcore_startup(struct uart_port *port)
{
	if (uart_done)
        return 0;

	/*config uart*/
	printk(KERN_INFO "Serial: grx500_bootcore driver startup\n");
    grx500_bootcore_config_hw(/*UART_RATE_9600,*/UART_RATE_115200,
                   UART_TX_OUT_ENABLE_YES/*0x1*/,
                   UART_PARITY_NONE/*0*/,
                   UART_STOP_ONEBIT/*0*/);

	/*config vector interrupt*/
	grx500_bootcore_register_static_irq(GRX500_BOOTCORE_SERIAL_IRQ_IN_INDEX, GRX500_BOOTCORE_SERIAL_IRQ_OUT_INDEX, &uart_irqaction, grx500_bootcore_uart_irq);
    //grx500_bootcore_enable_irq(GRX500_BOOTCORE_SERIAL_IRQ_OUT_INDEX); huanx
	uart_done = 1;

	return 0;
}

static void grx500_bootcore_shutdown(struct uart_port *port)
{
    /*TODO*/
}

static void
grx500_bootcore_set_termios(struct uart_port *port, struct ktermios *termios,
		       struct ktermios *old)
{
    return;
}

static void
grx500_bootcore_pm(struct uart_port *port, unsigned int state,
	      unsigned int oldstate)
{
	return;
}

static void grx500_bootcore_release_port(struct uart_port *port)
{
	return;
}

static int grx500_bootcore_request_port(struct uart_port *port)
{
	return 0;
}

static void grx500_bootcore_config_port(struct uart_port *port, int flags)
{
	return;
}

static int
grx500_bootcore_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static const char *
grx500_bootcore_type(struct uart_port *port)
{
	return "grx500_bootcoretty";
}

#define GRX500_BOOTCORE_CONSOLE	&grx500_bootcore_console

static struct uart_driver grx500_bootcore_uart_reg;

static struct console grx500_bootcore_console = {
	.name = "ttyLTQ",
	.write = grx500_bootcore_console_write,
	.device = uart_console_device,
	.data		= &grx500_bootcore_uart_reg,
	.flags = CON_PRINTBUFFER | CON_ENABLED | CON_CONSDEV,
	.index = -1,
};

static struct uart_ops grx500_bootcore_ops = {
	.tx_empty	    = grx500_bootcore_tx_empty,     /*NULL*/
	.set_mctrl	    = grx500_bootcore_set_mctrl,    /*NULL*/
	.get_mctrl	    = grx500_bootcore_get_mctrl,    /*NULL*/
	.stop_tx	    = grx500_bootcore_stop_tx,
	.start_tx	    = grx500_bootcore_start_tx,
	.stop_rx        = grx500_bootcore_stop_rx,
	.enable_ms	    = grx500_bootcore_enable_ms,    /*NULL*/
	.break_ctl	    = grx500_bootcore_break_ctl,    /*NULL*/
	.startup	    = grx500_bootcore_startup,
	.shutdown	    = grx500_bootcore_shutdown,
	.set_termios    = grx500_bootcore_set_termios,  /*NULL*/
	.pm		        = grx500_bootcore_pm,           /*NULL*/
	.type		    = grx500_bootcore_type,
	.release_port	= grx500_bootcore_release_port, /*NULL*/
	.request_port	= grx500_bootcore_request_port, /*NULL*/
	.config_port	= grx500_bootcore_config_port,  /*NULL*/
	.verify_port	= grx500_bootcore_verify_port,  /*NULL*/
};

static struct uart_port grx500_bootcore_port = {
	.ops		= &grx500_bootcore_ops,
   .type       = PORT_GRX500_BOOTCORE,
};

//static struct uart_driver grx500_bootcore_uart_reg;

static struct uart_driver grx500_bootcore_uart_reg = {
	.owner			= THIS_MODULE,
	.driver_name	= "serial",
	.dev_name		= "ttyLTQ",
	.major			= TTY_MAJOR,
	.minor			= 64,
	.nr			    = UART_NR,
	.cons			= GRX500_BOOTCORE_CONSOLE,
};

#ifdef WWPATCH

#define			REG_UART_BYTES_IN_FIFO				(0x2c)
#define			REG_UART_TX_FIFO_BITS_MASK			(0x3e0)

void wave400_console_tx_put(struct uart_port *port, /*char*/int byte)
{
	int dummy_count=0xfffff;

	while(((MT_RdReg(UART_BASE,REG_UART_BYTES_IN_FIFO) & REG_UART_TX_FIFO_BITS_MASK) != 0) && dummy_count)
	{
		dummy_count--;
	}
	MT_WrReg(UART_BASE ,WR_ADDR, (char)byte);
}


#endif /* WWPATCH */


/*static*/ void grx500_bootcore_console_write(struct console *co, const char *s, unsigned count)
{

	#ifdef WWPATCH
	/* i think start tx below fail since console init is before driver initcall */
	uart_console_write(&grx500_bootcore_port, s, count, wave400_console_tx_put);
	#else /* WWPATCH */
	struct uart_port *port = &grx500_bootcore_port;
	grx500_bootcore_start_tx(port);
	#endif
}

/*static*/ void grx500_bootcore_start_tx(struct uart_port *port)
{
//	struct uart_port *port = &grx500_bootcore_port;
//	struct circ_buf *xmit = &port->info->xmit; //ptr to xmit buff
	int count_local;
	struct circ_buf *xmit = NULL;

	if (!port->state)
		return;

	xmit = &port->state->xmit; //ptr to xmit buff

	if (port->x_char) {
		port->icount.tx++;
        printk("grx500_bootcore_start_tx: xon-xoff non-zero=%c\n", port->x_char); 
        port->x_char = 0;
	}

	/*buff handling:*/
	count_local = uart_circ_chars_pending(xmit);
	if (!count_local)
    {
	    return;
    }

	do {
		//if ( uart_circ_empty(xmit) || (((MT_RdReg( UART_BASE, REG_UART_BYTES_IN_TX_FIFO) & UART_FWL_TX_MASK)>>5) == 16) )
		if ( uart_circ_empty(xmit) || uart_full_hw_buffer() )
			break; //who trigger tx again?
	    MT_WrReg(UART_BASE ,WR_ADDR, (volatile unsigned long)xmit->buf[xmit->tail]);
	    //MT_WrReg(UART_BASE ,WR_ADDR, ((volatile unsigned long)xmit->buf[xmit->tail] & 0x000000FF));
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	} while (--count_local > 0);
    
    if (uart_circ_chars_pending(xmit))
        uart_write_wakeup(port);
    /*? can get port->x_char ??*/
    
}


static int __init grx500_bootcore_uart_init(void)
{
    struct irqaction *action = &uart_irqaction;
    struct uart_port *up = &grx500_bootcore_port;
    int ret;

	printk(KERN_INFO "Serial: grx500_bootcore driver init\n");
            
    /*register uart_port for use in interrupt handler*/
    action->dev_id = up;

	ret = uart_register_driver(&grx500_bootcore_uart_reg);
	if (ret)
		goto out;

    ret = uart_add_one_port(&grx500_bootcore_uart_reg, &grx500_bootcore_port);
	if (ret)
	    uart_unregister_driver(&grx500_bootcore_uart_reg);
	
out:
	return ret;
}

/*static*/ int __init grx500_bootcore_console_init(void)
{
	printk(KERN_INFO ": grx500_bootcore_console_init: call register\n");

	register_console(&grx500_bootcore_console);
    /*no need to unregister early console if set flag=CON_CONSDEV in new console,
     done in register_console().
    unregister_console(&early_lntq_console);*/

	return 0;
}

console_initcall(grx500_bootcore_console_init);


static void __exit grx500_bootcore_uart_exit(void)
{

	uart_unregister_driver(&grx500_bootcore_uart_reg);
}

module_init(grx500_bootcore_uart_init);
module_exit(grx500_bootcore_uart_exit);

