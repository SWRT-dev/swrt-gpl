#include <linux/tty.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/serial.h>
#include <asm/setup.h>
#include <irq.h>
#include <grx500_bootcore_chadr.h>
#include <grx500_bootcore_time.h>
#include <grx500_bootcore_uart.h>
#include <grx500_bootcore_defs.h>

#include <grx500_bootcore_cnfg.h>
#include <grx500_bootcore_chipreg.h>
#include <grx500_bootcore_interrupt.h>
#include <lantiq.h>

#ifdef CONFIG_SOC_TYPE_GRX500_TEP

/* base addr of uart and clock timing */
#define         BASE                    MT_LOCAL_MIPS_BASE_ADDRESS
#define         MAX_BAUD                115200

/*
 * 0 - we need to do serial init
 * 1 - skip serial init
 */

static int serialPortInitialized = 0;

struct early_lntq_device {
	struct uart_port port;
};

static struct early_lntq_device early_device;

/*
 *  * the default baud rate *if* we do serial init
 *   */
#ifdef CONFIG_USE_EMULATOR
#define         BAUD_DEFAULT            UART16550_BAUD_9600 //UART16550_BAUD_115200
#else
#define         BAUD_DEFAULT            UART16550_BAUD_115200
#endif

/* === END OF CONFIG === */

static void __init early_lntq_write(struct console *console, const char *s, unsigned int count);

/* we need uint32 uint8 */
//typedef         unsigned char uint8;
//typedef         unsigned int  uint32;

#define         UART16550_BAUD_2400             2400
#define         UART16550_BAUD_4800             4800
#define         UART16550_BAUD_9600             9600
#define         UART16550_BAUD_19200            19200
#define         UART16550_BAUD_38400            38400
#define         UART16550_BAUD_57600            57600
#define         UART16550_BAUD_115200           115200

#define         UART16550_PARITY_NONE           0
#define         UART16550_PARITY_ODD            0x08
#define         UART16550_PARITY_EVEN           0x18
#define         UART16550_PARITY_MARK           0x28
#define         UART16550_PARITY_SPACE          0x38

#define         UART16550_TX_OUTPUT_ENABLE      0x0
#define         UART16550_TX_OUTPUT_DISABLE     0x1

#define         UART16550_STOP_1BIT             0x0
#define         UART16550_STOP_2BIT             0x4

/* register offset */
#define			REG_UART_INT						(0x1c)
#define			REG_UART_WRITE_DATA					(0xc)
#define			REG_UART_READ_DATA					(0x10)
#define			REG_UART_BYTES_IN_FIFO				(0x2c)
#define			REG_UART_RX_FIFO_BITS_MASK			(0x1f)
#define			REG_UART_TX_FIFO_BITS_MASK			(0x3e0)
#define			REG_UART_CONTROL					(0x20)
#define			REG_UART_CLOCK_DIVISION_RATIO		(0x14)
#define			REG_UART_RX_IDLE_COUNTER			(0x28)

/* memory-mapped read/write of the port */
#define         UART16550_READ(y)    (*((volatile uint32*)(BASE + y)))
#define         UART16550_WRITE(y, z)  ((*((volatile uint32*)(BASE + y))) = ((uint32)z))

/*uart mux to host*/
#define REG_UART_CONTROL_MUX_MPU_BIT 0x100

#undef GRX500_BOOTCORE_DEBUG_EARLY

void Uart16550Init_early(uint32 baud, uint8 data, uint8 parity, uint8 stop)
{
    serialPortInitialized = 1;
	/* disable interrupts */
    UART16550_WRITE(REG_UART_INT, 0x00); 

	/* set up baud rate */
	{ 
		uint32 divisor;

		/* set divisor */
#ifndef CONFIG_VBG400_CHIPIT //Orig
//#ifdef CONFIG_VBG400_CHIPIT

        divisor = ((GRX500_BOOTCORE_SYSTEM_CLK) / (UART_BAUD_RATIO_DIV*baud)) - 1;
#else
		divisor = ((GRX500_BOOTCORE_SYSTEM_CLK/10) / (UART_BAUD_RATIO_DIV*baud)) - 1;
#endif
		//UART16550_WRITE16(REG_UART_CLOCK_DIVISION_RATIO, divisor & 0x3fff);
		UART16550_WRITE(REG_UART_CLOCK_DIVISION_RATIO, divisor & 0x3fff);
 	}

	/* set data format */
	UART16550_WRITE(REG_UART_CONTROL, data|parity|stop|REG_UART_CONTROL_MUX_MPU_BIT); 
	UART16550_WRITE(REG_UART_RX_IDLE_COUNTER, 90); //10 time one byte duration
}

static void __init console_config(void)
{
	Uart16550Init_early(BAUD_DEFAULT,
					UART16550_TX_OUTPUT_ENABLE,
					UART16550_PARITY_NONE, UART16550_STOP_1BIT);
#ifdef GRX500_BOOTCORE_DEBUG_EARLY
	{
	UART16550_WRITE(REG_UART_WRITE_DATA, 'e');
	UART16550_WRITE(REG_UART_WRITE_DATA, 'a');
	UART16550_WRITE(REG_UART_WRITE_DATA, 'r');
	UART16550_WRITE(REG_UART_WRITE_DATA, 'l');
	UART16550_WRITE(REG_UART_WRITE_DATA, 'y');
	UART16550_WRITE(REG_UART_WRITE_DATA, '\n');
    }
#endif
}

void __init  Uart16550Put(struct uart_port *port, /*char*/int byte)
{
	int dummy_count=0xfffff;

	if (!serialPortInitialized) {
			serialPortInitialized = 1;
			Uart16550Init_early(BAUD_DEFAULT,
							UART16550_TX_OUTPUT_ENABLE,
							UART16550_PARITY_NONE, UART16550_STOP_1BIT);
	}

	while(((UART16550_READ(REG_UART_BYTES_IN_FIFO) & REG_UART_TX_FIFO_BITS_MASK) != 0) && dummy_count)
	{
		dummy_count--;
	}
	UART16550_WRITE(REG_UART_WRITE_DATA, (char)byte);
}

#ifdef CONFIG_KGDB
int grx500_bootcore_kgdb_hook(int speed) 
{
	Uart16550Init_early(speed, UART16550_TX_OUTPUT_ENABLE, UART16550_PARITY_NONE, UART16550_STOP_1BIT);
	return speed;
}

int __init grx500_bootcore_putDebugChar(char byte)
{
	struct uart_port *port = &early_device.port;
	Uart16550Put(port, (int) byte);
	return 1; 
}

char grx500_bootcore_getDebugChar(void)
{
	char char_in;
	if (!serialPortInitialized) {
		serialPortInitialized = 1;
		Uart16550Init_early(BAUD_DEFAULT, UART16550_TX_OUTPUT_ENABLE,
				UART16550_PARITY_NONE, UART16550_STOP_1BIT);
	}

	while((!(UART16550_READ(REG_UART_BYTES_IN_FIFO) & REG_UART_RX_FIFO_BITS_MASK)))
		; //till there is no data in RX_FIFO
	char_in = (char) (UART16550_READ(REG_UART_READ_DATA) & UART_RD_DATA_MASK);
	return char_in;
}
#endif

static struct console early_lntq_console __initdata = {
	.name	= "early",
	.write	= early_lntq_write,
	.flags	= CON_PRINTBUFFER | CON_BOOT | CON_ENABLED,
	.index	= -1,
};



static void __init early_lntq_write(struct console *console,
					const char *s, unsigned int count)
{
	struct uart_port *port = &early_device.port;

	uart_console_write(port, s, count, Uart16550Put);
}

int __init setup_early_lntq_console(char *cmdline)
{
	UART16550_WRITE(REG_CPU_SUB_CLK_GATE, CPU_SUB_CLK_GATE_MASK_ALL);	// CPU_SUB_CLK_GATE all gated
	UART16550_WRITE(REG_CPU_SUB_RST, CPU_SUB_RST_MASK_RST);		// CPU_SUB_RST all reset
	UART16550_WRITE(REG_UART_CONTROL, UART16550_TX_OUTPUT_DISABLE);	// CPU_UART_CTL TX_OE tx out disable
	#ifdef CONFIG_SOC_PRX300_BOOTCORE
	ltq_w32(LTQ_MUX_TEP, LTQ_PORTMUXC30); // GPI0 30 MUX -> TEP UART RX
	ltq_w32(LTQ_MUX_TEP, LTQ_PORTMUXC31); // GPI0 31 MUX -> TEP UART TX
	#elif defined(CONFIG_SOC_GRX500_BOOTCORE) /* CONFIG_SOC_PRX300_BOOTCORE */
	ltq_w32_mask(IFMUX_CFG_BBTEPUART_EN_MASK, LTQ_MUX_TEP, LTQ_IFMUX_CFG); // IFMUX_CFG , BBTEPUART_EN=TEP_U
	#endif /* CONFIG_SOC_PRX300_BOOTCORE */

	//config HW
	console_config();

	register_console(&early_lntq_console);

	return 0;
}

early_param("console", setup_early_lntq_console);

#endif

