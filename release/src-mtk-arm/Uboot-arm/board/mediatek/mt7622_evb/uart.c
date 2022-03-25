#include <serial.h>
#include <common.h>
#include <config.h>

#include <asm/arch/typedefs.h>
#include <asm/arch/mt6735.h>
#include <asm/arch/uart.h>

#include <asm/arch/mt_gpio.h>

#if defined (CONFIG_MTK_SECOND_UART)
struct serial_device *mtk_2nd_serial_console(void);
#endif

#define UART_BASE(uart)             (uart)

#define UART_RBR(uart)              (UART_BASE(uart)+0x0)       /* Read only */
#define UART_THR(uart)              (UART_BASE(uart)+0x0)       /* Write only */
#define UART_IER(uart)              (UART_BASE(uart)+0x4)
#define UART_IIR(uart)              (UART_BASE(uart)+0x8)       /* Read only */
#define UART_FCR(uart)              (UART_BASE(uart)+0x8)       /* Write only */
#define UART_LCR(uart)              (UART_BASE(uart)+0xc)
#define UART_MCR(uart)              (UART_BASE(uart)+0x10)
#define UART_LSR(uart)              (UART_BASE(uart)+0x14)
#define UART_MSR(uart)              (UART_BASE(uart)+0x18)
#define UART_SCR(uart)              (UART_BASE(uart)+0x1c)
#define UART_DLL(uart)              (UART_BASE(uart)+0x0)
#define UART_DLH(uart)              (UART_BASE(uart)+0x4)
#define UART_EFR(uart)              (UART_BASE(uart)+0x8)
#define UART_XON1(uart)             (UART_BASE(uart)+0x10)
#define UART_XON2(uart)             (UART_BASE(uart)+0x14)
#define UART_XOFF1(uart)            (UART_BASE(uart)+0x18)
#define UART_XOFF2(uart)            (UART_BASE(uart)+0x1c)
#define UART_AUTOBAUD_EN(uart)      (UART_BASE(uart)+0x20)
#define UART_HIGHSPEED(uart)        (UART_BASE(uart)+0x24)
#define UART_SAMPLE_COUNT(uart)     (UART_BASE(uart)+0x28)
#define UART_SAMPLE_POINT(uart)     (UART_BASE(uart)+0x2c)
#define UART_AUTOBAUD_REG(uart)     (UART_BASE(uart)+0x30)
#define UART_RATE_FIX_AD(uart)      (UART_BASE(uart)+0x34)
#define UART_AUTOBAUD_SAMPLE(uart)  (UART_BASE(uart)+0x38)
#define UART_GUARD(uart)            (UART_BASE(uart)+0x3c)
#define UART_ESCAPE_DAT(uart)       (UART_BASE(uart)+0x40)
#define UART_ESCAPE_EN(uart)        (UART_BASE(uart)+0x44)
#define UART_SLEEP_EN(uart)         (UART_BASE(uart)+0x48)
#define UART_VFIFO_EN(uart)         (UART_BASE(uart)+0x4c)
#define UART_RXTRI_AD(uart)         (UART_BASE(uart)+0x50)

#define TOPRGU_STRAP_PAR            (RGU_BASE + 0x60)

#define UART_SET_BITS(BS,REG)       ((*(volatile U32*)(REG)) |= (U32)(BS))
#define UART_CLR_BITS(BS,REG)       ((*(volatile U32*)(REG)) &= ~((U32)(BS)))
#define UART_WRITE16(VAL, REG)      DRV_WriteReg(REG,VAL)
#define UART_READ32(REG)            DRV_Reg32(REG)
#define UART_WRITE32(VAL, REG)      DRV_WriteReg32(REG,VAL)

#if CFG_FPGA_PLATFORM
volatile unsigned int g_uart = UART1;
#define UART_SRC_CLK  FPGA_UART_CLOCK
#else
volatile unsigned int g_uart = UART1;
#define UART_SRC_CLK  EVB_UART_CLOCK   // 25000000
#endif

// output uart baudrate
unsigned int g_brg = CONFIG_BAUDRATE;

void mtk_uart_power_on(MT65XX_UART uart_base)
{
	/* UART Powr PDN and Reset*/
    //#define AP_PERI_GLOBALCON_RST0 (PERI_CON_BASE+0x0)
    #define AP_PERI_GLOBALCON_PDN0 (INFRACFG_AO_BASE+0x084)
    if (uart_base == UART1)
        UART_SET_BITS(1 << 22, AP_PERI_GLOBALCON_PDN0); /* Power on UART1 */
    else if (uart_base == UART2)
        UART_SET_BITS(1 << 23, AP_PERI_GLOBALCON_PDN0); /* Power on UART2 */
	else if (uart_base == UART3)
        UART_SET_BITS(1 << 24, AP_PERI_GLOBALCON_PDN0); /* Power on UART2 */
	else if (uart_base == UART4)
        UART_SET_BITS(1 << 25, AP_PERI_GLOBALCON_PDN0); /* Power on UART2 */
    return;
}

#if defined (CONFIG_MTK_SECOND_UART_IS_UART2)
/* HW UART +1 = SW_UART */
volatile unsigned int g_2nd_uart = UART3;
#endif
#if defined (CONFIG_MTK_SECOND_UART_IS_UART3)
/* HW UART +1 = SW_UART */
volatile unsigned int g_2nd_uart = UART4;
#endif

#if defined (CONFIG_MTK_SECOND_UART)
int mtk_uart_2nd_init (void)
{

/* UART2: GPIO51-52 */
#if defined (CONFIG_MTK_SECOND_UART_IS_UART2)
    // MDI_TP_P0: GPIO51 FUNC0: TXD2 (O)
    mt_set_gpio_mode(GPIO51, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO51, GPIO_DIR_OUT);
    // MDI_TN_P0: GPIO52 FUNC0: RXD2 (O)
    mt_set_gpio_mode(GPIO52, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO52, GPIO_DIR_IN);
    mt_set_gpio_pull_select(GPIO52, GPIO_PULL_UP);
    // MDI_RP_P9: GPIO53 FUNC0: RTS2_N (o)
    mt_set_gpio_mode(GPIO53, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO53, GPIO_DIR_OUT);
    // MDI_RN_P0: GPIO54 FUNC0: CTS2_N (I)
    mt_set_gpio_mode(GPIO54, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO54, GPIO_DIR_IN);
    mt_set_gpio_pull_select(GPIO54, GPIO_PULL_UP);

#endif

/* UART3: GPIO81-82 + 83/84 */
#if defined (CONFIG_MTK_SECOND_UART_IS_UART3)
    // TXD3: GPIO81 FUNC0: TXD3 (O)
    mt_set_gpio_mode(GPIO81, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO81, GPIO_DIR_OUT);
    // RXD3: GPIO82 FUNC0: RXD3 (I)
    mt_set_gpio_mode(GPIO82, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO82, GPIO_DIR_IN);
    mt_set_gpio_pull_select(GPIO82, GPIO_PULL_UP);
    // RTS3_N: GPIO79 FUNC0: RTS3_N (o)
    mt_set_gpio_mode(GPIO79, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO79, GPIO_DIR_OUT);
    // CTS3_N: GPIO80 FUNC0: CTS3_N (I)
    mt_set_gpio_mode(GPIO80, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO80, GPIO_DIR_IN);
    mt_set_gpio_pull_select(GPIO80, GPIO_PULL_UP);
#endif

    UART_SET_BITS(UART_FCR_FIFO_INIT, UART_FCR(g_2nd_uart)); /* clear fifo */
    UART_WRITE16(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, UART_LCR(g_2nd_uart));
    uart_2nd_setbrg();

    return 0;
}

void uart_2nd_setbrg(void)
{
    U32 uartclk = UART_SRC_CLK;
    U32 baudrate = CONFIG_MTK_SECOND_UART_BAUD;
    unsigned int byte,speed;
    unsigned int highspeed;
    unsigned int quot, divisor, remainder;
    unsigned short data, high_speed_div, sample_count, sample_point;
    unsigned int tmp_div;
    if (UART_READ32(TOPRGU_STRAP_PAR) & 2)
    	uartclk = 40000000;		/* 40M OSC case */
    speed = baudrate;
    highspeed = 3;
    quot = 1;
    data=(unsigned short)(uartclk/speed);
    high_speed_div = (data>>8) + 1; // divided by 256
    tmp_div=uartclk/(speed*high_speed_div);
    divisor =  (unsigned short)tmp_div;
    remainder = (uartclk)%(high_speed_div*speed);
    /*get (sample_count+1)*/
    if (remainder >= ((speed)*(high_speed_div))>>1)
        divisor =  (unsigned short)(tmp_div+1);
    else
        divisor =  (unsigned short)tmp_div;
    sample_count=divisor-1;
    /*get the sample point*/
    sample_point=(sample_count-1)>>1;
    /*configure register*/
    UART_WRITE32(highspeed, UART_HIGHSPEED(g_2nd_uart));
    byte = UART_READ32(UART_LCR(g_2nd_uart));    /* DLAB start */
    UART_WRITE32((byte | UART_LCR_DLAB), UART_LCR(g_2nd_uart));
    UART_WRITE32((high_speed_div & 0x00ff), UART_DLL(g_2nd_uart));
    UART_WRITE32(((high_speed_div >> 8)&0x00ff), UART_DLH(g_2nd_uart));
    UART_WRITE32(sample_count, UART_SAMPLE_COUNT(g_2nd_uart));
    UART_WRITE32(sample_point, UART_SAMPLE_POINT(g_2nd_uart));
    UART_WRITE32(byte, UART_LCR(g_2nd_uart));   /* DLAB end */
}

void uart_2nd_putc (const char c)
{
    while (!(UART_READ32 (UART_LSR(g_2nd_uart)) & UART_LSR_THRE));
    UART_WRITE32 ((unsigned int) c, UART_THR(g_2nd_uart));
}

int uart_2nd_getc(void)  /* returns -1 if no data available */
{
    while (!(DRV_Reg32(UART_LSR(g_2nd_uart)) & UART_LSR_DR));
    return (int)DRV_Reg32(UART_RBR(g_2nd_uart));
}

void uart_2nd_puts(const char *s)
{
    while (*s)
        uart_2nd_putc(*s++);
}

int uart_2nd_tstc(void)
{
    return (DRV_Reg32(UART_LSR(g_2nd_uart)) & UART_LSR_DR);
}
#endif

void uart_setbrg(void)
{
    U32 uartclk = UART_SRC_CLK;
    U32 baudrate = g_brg;

#if CFG_FPGA_PLATFORM
    #define MAX_SAMPLE_COUNT 256

    U16 tmp;
    U32 divisor;
    U32 sample_data;
    U32 sample_count;
    U32 sample_point;

    // Setup N81,(UART_WLS_8 | UART_NONE_PARITY | UART_1_STOP) = 0x03
    UART_WRITE32(0x0003, UART_LCR(g_uart));

   /*
    * NoteXXX: Below is the sample code to set UART baud rate.
    *          I assume that when system is reset, the UART clock rate is 26MHz
    *          and baud rate is 115200.
    *          use UART1_HIGHSPEED = 0x3 can get more sample count to get better UART sample rate
    *          based on baud_rate = uart clock frequency / (sampe_count * divisor)
    *          divisor = (DLH+DLL)
    */

    // In order to get better UART sample rate, set UART1_HIGHSPEED = 0x3.
    // And we can calculate sample count for reducing effect of UART sample rate variation
    UART_WRITE32(0x0003, UART_HIGHSPEED(g_uart));

    // calculate sample_data = sample_count*divisor
    // round off the result for approximating to the real baudrate
    sample_data = (uartclk+(baudrate/2))/baudrate;
    // calculate divisor
    divisor = (sample_data+(MAX_SAMPLE_COUNT-1))/MAX_SAMPLE_COUNT;
    // calculate sample count
    sample_count = sample_data/divisor;
    // calculate sample point (count from 0)
    sample_point = (sample_count-1)/2;
    // set sample count (count from 0)
    UART_WRITE32((sample_count-1), UART_SAMPLE_COUNT(g_uart));
    // set sample point
    UART_WRITE32(sample_point, UART_SAMPLE_POINT(g_uart));

    tmp = UART_READ32(UART_LCR(g_uart));        /* DLAB start */
    UART_WRITE32((tmp | UART_LCR_DLAB), UART_LCR(g_uart));

    UART_WRITE32((divisor&0xFF), UART_DLL(g_uart));
    UART_WRITE32(((divisor>>8)&0xFF), UART_DLH(g_uart));
    UART_WRITE32(tmp, UART_LCR(g_uart));

#else
    unsigned int byte,speed;
    unsigned int highspeed;
    unsigned int quot, divisor, remainder;
    unsigned short data, high_speed_div, sample_count, sample_point;
    unsigned int tmp_div;

	if (UART_READ32(TOPRGU_STRAP_PAR) & 2)
		uartclk = 40000000;		/* 40M OSC case */

    speed = baudrate;
    highspeed = 3;
    quot = 1;

    data=(unsigned short)(uartclk/speed);
    high_speed_div = (data>>8) + 1; // divided by 256

    tmp_div=uartclk/(speed*high_speed_div);
    divisor =  (unsigned short)tmp_div;

    remainder = (uartclk)%(high_speed_div*speed);
    /*get (sample_count+1)*/
    if (remainder >= ((speed)*(high_speed_div))>>1)
        divisor =  (unsigned short)(tmp_div+1);
    else
        divisor =  (unsigned short)tmp_div;

    sample_count=divisor-1;

    /*get the sample point*/
    sample_point=(sample_count-1)>>1;

    /*configure register*/
    UART_WRITE32(highspeed, UART_HIGHSPEED(g_uart));

    byte = UART_READ32(UART_LCR(g_uart));    /* DLAB start */
    UART_WRITE32((byte | UART_LCR_DLAB), UART_LCR(g_uart));
    UART_WRITE32((high_speed_div & 0x00ff), UART_DLL(g_uart));
    UART_WRITE32(((high_speed_div >> 8)&0x00ff), UART_DLH(g_uart));
    UART_WRITE32(sample_count, UART_SAMPLE_COUNT(g_uart));
    UART_WRITE32(sample_point, UART_SAMPLE_POINT(g_uart));
    UART_WRITE32(byte, UART_LCR(g_uart));   /* DLAB end */
#endif
}

int serial_nonblock_getc(void)
{
    return (int)UART_READ32(UART_RBR(g_uart));
}

void mtk_serial_set_current_uart(MT65XX_UART uart_base)
{
    g_uart = uart_base;
}

int mtk_uart_init (void)
{
// Iverson 20150522 : GPIO setting is defined in preloader.
#if 0
#if !CFG_FPGA_PLATFORM
	#ifdef GPIO_UART_UTXD0_PIN
    mt_set_gpio_mode(GPIO_UART_UTXD0_PIN, GPIO_UART_UTXD0_PIN_M_UTXD);
    mt_set_gpio_dir(GPIO_UART_UTXD0_PIN, GPIO_DIR_OUT);
    #endif

    #ifdef GPIO_UART_URXD0_PIN
    mt_set_gpio_mode(GPIO_UART_URXD0_PIN, GPIO_UART_URXD0_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_URXD0_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_UART_URXD0_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_UART_URXD0_PIN, GPIO_PULL_UP);
    #endif

    #ifdef GPIO_UART_UTXD1_PIN
    mt_set_gpio_mode(GPIO_UART_UTXD1_PIN, GPIO_UART_UTXD1_PIN_M_UTXD);
    mt_set_gpio_dir(GPIO_UART_UTXD1_PIN, GPIO_DIR_OUT);
    #endif

    #ifdef GPIO_UART_URXD1_PIN
    mt_set_gpio_mode(GPIO_UART_URXD1_PIN, GPIO_UART_URXD1_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_URXD1_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_UART_URXD1_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_UART_URXD1_PIN, GPIO_PULL_UP);
    #endif

    #ifdef GPIO_UART_UTXD2_PIN
    mt_set_gpio_mode(GPIO_UART_UTXD2_PIN, GPIO_UART_UTXD2_PIN_M_UTXD);
    mt_set_gpio_dir(GPIO_UART_UTXD2_PIN, GPIO_DIR_OUT);
    #endif

    #ifdef GPIO_UART_URXD2_PIN
    mt_set_gpio_mode(GPIO_UART_URXD2_PIN, GPIO_UART_URXD2_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_URXD2_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_UART_URXD2_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_UART_URXD2_PIN, GPIO_PULL_UP);
    #endif

    #ifdef GPIO_UART_UTXD3_PIN
    mt_set_gpio_mode(GPIO_UART_UTXD3_PIN, GPIO_UART_UTXD3_PIN_M_UTXD);
    mt_set_gpio_dir(GPIO_UART_UTXD3_PIN, GPIO_DIR_OUT);
    #endif

    #ifdef GPIO_UART_URXD3_PIN
    mt_set_gpio_mode(GPIO_UART_URXD3_PIN, GPIO_UART_URXD3_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_URXD3_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_UART_URXD3_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_UART_URXD3_PIN, GPIO_PULL_UP);
    #endif
#endif

    /* uartclk != 0, means use custom bus clock; uartclk == 0, means use defaul bus clk */
    if(0 == uartclk){ // default bus clk
        //uartclk = mtk_get_bus_freq()*1000/4;
	uartclk = UART_SRC_CLK;
    }

#if (CFG_OUTPUT_PL_LOG_TO_UART1	&& !(defined(HW_INIT_ONLY) || defined(SLT) || defined(DUMMY_AP) || defined(TINY)))
	// init UART1
    mtk_serial_set_current_uart(UART2);

    UART_SET_BITS(UART_FCR_FIFO_INIT, UART_FCR(g_uart)); /* clear fifo */
    UART_WRITE16(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, UART_LCR(g_uart));
    serial_setbrg(uartclk, CFG_LOG_BAUDRATE);


	//set GPIO for UART1
    #ifdef GPIO_UART_UTXD1_PIN
    mt_set_gpio_mode(GPIO_UART_UTXD1_PIN, GPIO_UART_URXD1_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_UTXD1_PIN, GPIO_DIR_OUT);
    #endif

    #ifdef GPIO_UART_URXD1_PIN
    mt_set_gpio_mode(GPIO_UART_URXD1_PIN, GPIO_UART_URXD1_PIN_M_URXD);
    mt_set_gpio_dir(GPIO_UART_URXD1_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_UART_URXD1_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_UART_URXD1_PIN, GPIO_PULL_UP);
    #endif
#endif //#if CFG_OUTPUT_PL_LOG_TO_UART1

    mtk_serial_set_current_uart(CFG_UART_META);

    #if !(CFG_FPGA_PLATFORM)
    mtk_uart_power_on(CFG_UART_META);
    #endif

    UART_SET_BITS(UART_FCR_FIFO_INIT, UART_FCR(g_uart)); /* clear fifo */
    UART_WRITE16(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, UART_LCR(g_uart));
    serial_setbrg(uartclk, CFG_META_BAUDRATE);


    mtk_serial_set_current_uart(CFG_UART_LOG);

    #if !(CFG_FPGA_PLATFORM)
    mtk_uart_power_on(CFG_UART_LOG);
    #endif
#endif

    UART_SET_BITS(UART_FCR_FIFO_INIT, UART_FCR(g_uart)); /* clear fifo */
    UART_WRITE16(UART_NONE_PARITY | UART_WLS_8 | UART_1_STOP, UART_LCR(g_uart));
    uart_setbrg();

    return 0;
}

void PutUARTByte (const char c)
{
    while (!(UART_READ32 (UART_LSR(g_uart)) & UART_LSR_THRE))
    {
    }

    if (c == '\n')
        UART_WRITE32 ((unsigned int) '\r', UART_THR(g_uart));

    UART_WRITE32 ((unsigned int) c, UART_THR(g_uart));
}

int uart_getc(void)  /* returns -1 if no data available */
{
	while (!(DRV_Reg32(UART_LSR(g_uart)) & UART_LSR_DR));
 	return (int)DRV_Reg32(UART_RBR(g_uart));
}

void uart_puts(const char *s)
{
	while (*s)
		PutUARTByte(*s++);
}

int uart_tstc(void)
{
    return (DRV_Reg32(UART_LSR(g_uart)) & UART_LSR_DR);
}

static struct serial_device mt7622_uart_drv = {
	.name	= "mt7622_uart",
	.start	= mtk_uart_init,
	.stop	= NULL,
	.setbrg	= uart_setbrg,
	.putc	= PutUARTByte,
	.puts	= uart_puts,
	.getc	= uart_getc,
	.tstc	= uart_tstc,
};

#if defined (CONFIG_MTK_SECOND_UART)
static struct serial_device mt7622_2nd_uart_drv = {
	.name	= "mt7622_2nd_uart",
	.start	= mtk_uart_2nd_init,
	.stop	= NULL,
	.setbrg	= uart_2nd_setbrg,
	.putc	= uart_2nd_putc,
	.puts	= uart_2nd_puts,
	.getc	= uart_2nd_getc,
	.tstc	= uart_2nd_tstc,
};
#endif

void pl01x_serial_initialize(void)
{
	serial_register(&mt7622_uart_drv);
}

struct serial_device *default_serial_console(void)
{
	return &mt7622_uart_drv;
}

#if defined (CONFIG_MTK_SECOND_UART)
struct serial_device *mtk_2nd_serial_console(void)
{
	return &mt7622_2nd_uart_drv;
}
#endif

