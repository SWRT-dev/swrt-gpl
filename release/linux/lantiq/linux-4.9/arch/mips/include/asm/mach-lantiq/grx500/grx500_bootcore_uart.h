#ifndef __MT_UART_H__
#define __MT_UART_H__

#include "grx500_bootcore_chadr.h"
#include "grx500_bootcore_time.h"

#ifdef MT_GLOBAL
   #define MT_EXTERN
   #define MT_I(x) x
#else
   #define MT_EXTERN extern
   #define MT_I(x)
#endif

#define PORT_GRX500_BOOTCORE	82

struct uart_port;
extern void grx500_bootcore_uart_tx_isr(struct uart_port *port);
extern void grx500_bootcore_uart_rx_isr(struct uart_port *port);

#define UART_BASE	 			MT_LOCAL_MIPS_BASE_ADDRESS

/***********************************/
/*       UART  register MASK       */
/***********************************/

#define WR_ADDR					0x0c
#define GCLK_ADDR				0 

#define UART_WR_MASK                 			0x000000FF
#define UART_RD_DATA_MASK	 					0x000000FF
#define UART_RD_PERR_MASK	 					0x00000100
#define	UART_DIVR_MASK		 					0x00003FFF
#define	UART_FWL_RX_MASK	 					0x0000001F
#define	UART_FWL_TX_MASK	 					0x000003E0

/*	 UART	interrupts enable    */
#define	UART_INT_OVERRUN_IRQ_ENABLE		 		    	0x00000001
#define	UART_INT_FRAMING_ERROR_IRQ_ENABLE 	 			0x00000002
#define UART_INT_PARITY_ERROR_IRQ_ENABLE         		        0x00000004
#define UART_INT_READ_COLLAPSE_BUFFER_IRQ_ENABLE 	                0x00000008
#define UART_INT_WRITE_COLLAPSE_BUFFER_IRQ_ENABLE     	                0x00000010
#define UART_INT_READ_FIFO_WATER_LEVEL_IRQ_ENABLE 		        0x00000020
#define UART_INT_WRITE_FIFO_WATER_LEVEL_IRQ_ENABLE		        0x00000040
#define UART_INT_IDLE_IRQ_ENABLE		                        0x00000080
/*	 UART	interrupts event    */
#define	UART_INT_OVERRUN_IRQ		 		    		0x00000100
#define	UART_INT_FRAMING_ERROR_IRQ 	 				0x00000200
#define UART_INT_PARITY_ERROR_IRQ	 		    		0x00000400
#define UART_INT_READ_COLLAPSE_BUFFER_IRQ 	    		        0x00000800
#define UART_INT_WRITE_COLLAPSE_BUFFER_IRQ     			        0x00001000
#define UART_INT_READ_FIFO_WATER_LEVEL_IRQ 				0x00002000
#define UART_INT_WRITE_FIFO_WATER_LEVEL_IRQ				0x00004000
#define UART_INT_IDLE_IRQ       		        		0x00008000
#define UART_INT_IRQ_MASK_ALL						0x0000FF00

#define UART_CONTROL_TX_OUTPUT_ENABLE_MASK				0x00000001
#define UART_CONTROL_RX_PARITY_MODE_MASK				0x00000006	
#define UART_CONTROL_TX_PARITY_MODE_MASK				0x00000018
#define UART_CONTROL_RX_STOP_MASK					0x00000020
#define UART_CONTROL_TX_STOP_MASK					0x00000040
#define UART_CONTROL_LOOPBACK_MODE_MASK			    	        0x00000080

#define UART_NOISE_COUNT_MASK						0x10000000

#define UART_IDLE_CNT_MAX_MASK						0x000000FF

#define UART_COUNT_FIF_RX_MASK                  		        0x0000001F
#define UART_COUNT_FIF_TX_MASK  					0x000003E0
#define UART_INT_MODE_MASK						0x00000001

/**************************************************************************************/

#define UART_ENABLE_INTRPT   1
#define UART_DISABLE_INTRPT  0

#define UART_TX_OUT_ENABLE_NO      1 
#define UART_TX_OUT_ENABLE_YES     0
#define UART_TX_OUT_ENABLE_SHIFT   0

/*=============== Baud rate ====================*/
#define UART_RATE_9600             9600UL
#define UART_RATE_19200            19200UL
#define UART_RATE_38400            38400UL
#define UART_RATE_57600            57600UL
#define UART_RATE_115200           115200UL
#define UART_RATE_MAX              UART_RATE_115200
#define UART_RATE_DEFAULT          UART_RATE_9600

#define UART_BAUD_RATIO_DIV     16

typedef enum  _UART_BAUD{
   UART_BAUD115200,
   UART_BAUD38400,
   UART_BAUD19200,
   UART_BAUD9600
} _UART_BAUD;

/* Standard COM flags (except for COM4, because of the 8514 problem) */
#ifdef CONFIG_SERIAL_DETECT_IRQ
#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST | ASYNC_AUTO_IRQ)
#else
#define STD_COM_FLAGS (ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST)
#endif

#define GRX500_BOOTCORE_BASE_BAUD (GRX500_BOOTCORE_SYSTEM_CLK / UART_BAUD_RATIO_DIV)
#define GRX500_BOOTCORE_UART_IRQ	1
//GRX500_BOOTCORE_SERIAL_IRQ_OUT_INDEX
#define SERIAL_PORT_DFNS			\
	/* UART CLK   PORT IRQ     FLAGS        */			\
	{ 0, GRX500_BOOTCORE_BASE_BAUD, UART_BASE, GRX500_BOOTCORE_UART_IRQ, STD_COM_FLAGS },	/* ttyS0 */	
#define MT_UARTRXIDLE ()


/*=============== Parity =======================*/
#define UART_PARITY_NONE           0           
#define UART_PARITY_ODD            1
#define UART_PARITY_EVEN	   3
#define UART_PARITY_DEFAULT        UART_PARITY_NONE
#define UART_PARITY_RX_SHIFT	   1 
#define UART_PARITY_TX_SHIFT	   3
	   
/*=============== Num of stop bits ==============*/
#define UART_STOP_ONEBIT           0
#define UART_STOP_TWOBIT           1
#define UART_STOP_DEFAULT          UART_STOP_ONEBIT
#define	UART_STOP_BIT_RX_SHIFT	   9
#define	UART_STOP_BIT_TX_SHIFT	   10
/*=============== Invert output siganl============*/
#define UART_SIGNAL_NO_INVERT      0
#define UART_SIGNAL_INVERT         1
#define UART_SIGNAL_DEFAULT        UART_SIGNAL_NO_INVERT

/*=============== Loop back on off ============*/
#define UART_LOOPBACK_ON           1
#define UART_LOOPBACK_OFF          0
#define UART_LOOPBACK_SHIFT	   11
/*=============== Reset uart ==================*/
#define UART_RESET_YES             1
#define UART_RESET_NO              0

/*=============== Enable disable uart cloocking */
#define UART_CLOCK_ON              1
#define UART_CLOCK_OFF             0

/*=============== Enable/Disable host interrupts from uart */
#define UART_HOST_INTRPT_ENABLE    0
#define UART_HOST_INTRPT_DISABLE   1


/*****************************************************************************
*
*       Module's Global Variables
*
*****************************************************************************/



/**********************************************************************
*			   MCOR	 of UART
***********************************************************************/

#define MCOR_UART_RX_NUM_MASK 0x1F
#define UART_MCOR_RD_RX_BYTES ((MT_UBYTE)((MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UART_BYTES_IN_RX_FIFO) & MCOR_UART_RX_NUM_MASK)))





/* Set which type of interrupts are masked */

#define MT_UART_MCOR_IDLE            UART_INT_IDLE_IRQ
#define MT_UART_MCOR_TX_WATER_LEVEL  UART_INT_WRITE_FIFO_WATER_LEVEL_IRQ
#define MT_UART_MCOR_RX_WATER_LEVEL  UART_INT_READ_FIFO_WATER_LEVEL_IRQ
#define MT_UART_MCOR_TX_COLLAPS_ERR  UART_INT_WRITE_COLLAPSE_BUFFER_IRQ
#define MT_UART_MCOR_RX_COLLAPS_ERR  UART_INT_READ_COLLAPSE_BUFFER_IRQ
#define MT_UART_MCOR_PARITY_ERR      UART_INT_PARITY_ERROR_IRQ
#define MT_UART_MCOR_FRAMING_ERR     UART_INT_FRAMING_ERROR_IRQ
#define MT_UART_MCOR_OVERRUN_ERR     UART_INT_OVERRUN_IRQ


/* Find out if UART interrupt */
#define UART_MCRO_INTRPT_ALL_EVENTS\
 ( MT_UART_MCOR_IDLE |\
   MT_UART_MCOR_TX_WATER_LEVEL |\
   MT_UART_MCOR_RX_WATER_LEVEL |\
   MT_UART_MCOR_TX_COLLAPS_ERR |\
   MT_UART_MCOR_RX_COLLAPS_ERR| \
   MT_UART_MCOR_PARITY_ERR |\
   MT_UART_MCOR_FRAMING_ERR |\
   MT_UART_MCOR_OVERRUN_ERR )



/* Define for global error in uart */
#define MT_UART_MCOR_ALL_RX_ERR\
  ( MT_UART_MCOR_PARITY_ERR | MT_UART_MCOR_FRAMING_ERR \
    | MT_UART_MCOR_OVERRUN_ERR |MT_UART_MCOR_RX_COLLAPS_ERR\
    | MT_UART_MCOR_TX_COLLAPS_ERR)

#define MT_UART_MCOR_RX_DATA_READY(STATUS) \
          (STATUS&(MT_UART_MCOR_IDLE|MT_UART_MCOR_RX_WATER_LEVEL))

#define MT_UART_MCOR_IS_TX_INTRPT(STATUS)\
          (STATUS&MT_UART_MCOR_TX_WATER_LEVEL)

#define MT_UART_MCOR_IS_RX_ERR(STATUS)\
          (STATUS&MT_UART_MCOR_ALL_RX_ERR)

#define MT_UART_MCOR_IS_RX_PARITY_ERR(STATUS)\
           (STATUS&MT_UART_MCOR_PARITY_ERR)

#define MT_UART_MCOR_IS_RX_OVERRUN_ERR(STATUS)\
           (STATUS&MT_UART_MCOR_OVERRUN_ERR)

#define MT_UART_MCOR_IS_RX_FRAMING_ERR(STATUS)\
           (STATUS&MT_UART_MCOR_FRAMING_ERR )

#define MT_UART_MCOR_IS_TX_COLLAPSE_ERR(STATUS)\
           (STATUS&MT_UART_MCOR_TX_COLLAPS_ERR)

#define MT_UART_MCOR_IS_RX_COLLAPSE_ERR(STATUS)\
           (STATUS&MT_UART_MCOR_RX_COLLAPS_ERR)



#define  MT_RX_INTERRUPT   0x01
#define  MT_TX_INTERRUPT   0x02
#define  MT_BSY_EVENT      0x04
/*
*  Communications status bit flags
* ---------------------------------
*/
#define UART_INBUF_EMPTY_MASK       0x01 /* Set when the input buffer is empty        */
#define UART_INBUF_FULL_MASK        0x02 /* Set when the input buffer is full         */
/* #define MT_INBUF_ALMOST_FULL_MASK 0x04 */ /* deleted ....                              */
#define UART_OUTBUF_EMPTY_MASK      0x08 /* Set when the output buffer is empty       */
#define UART_OUTBUF_FULL_MASK       0x10 /* Set when the output buffer is full        */
#define UART_ADDRESSED_MASK         0x20 /* Set when the modem is addressed           */
/*#define UART_GLOBAL_ADDRESSED_MASK  0x40 *//* Set when the modem is globally addressed  */

#define UART_COMMS_ERROR_MASK       0x80 /* Set when a comms error is detected.       */

/*
* Communications Debugging options bit flags
* ----------------------------------------------
*/
#define UART_TRANSMIT_STOPPED    0x01 /* Set when there is no more Data in Output Buffer */
#define UART_XON_IS_ACTIVE       0x02
#define UART_XOFF_TRIGGER        0x04
#define UART_XOFF_IS_ACTIVE      0x08
#define UART_FIRST_XON           0x10
#define UART_XON_TRIGGER         0x20
/*#define UART_NOT_USED_6          0x40*/
/*#define UART_ALMOST_FULL_FLAG    0x80*/

/*
*  Communications error flags
* -----------------------------
*/
#define MT_UART_COM_ERR_OVERRUN_MASK      0x01
#define MT_UART_COM_ERR_BUSY_MASK         0x02
#define MT_UART_COM_ERR_FRAMING_MASK      0x04
#define MT_UART_COM_ERR_PARITY_MASK       0x08
#define MT_UART_COM_ERR_READ_EMPTY_MASK   0x10
#define MT_UART_COM_ERR_WRITE_FULL_MASK   0x20
#define MT_UART_COM_MIPS_BUF_OVERRUN_MASK 0x40






#define MT_COMMS_BUFFER_SIZE  20   //80  ttpcom used 16 
#define UART_MAX_TXFIFO		  14   /*The buffer tx 16 byte*/

typedef struct {

           unsigned UartOverRunIntrpt:1;
           unsigned UartFramingIntrpt:1;
           unsigned UartParityIntrpt:1;
           unsigned UartRXFIFOCollapseIntrpt:1;
           unsigned UartTxFIFOCollapseIntrpt:1;
           unsigned UartRxWaterLevelIntrpt:1;
           unsigned UartTxWaterLevelIntrpt:1;
           unsigned UartIdleIntrpt:1;

}MT_UART_STATUS;


typedef struct {
           unsigned UartTxOutEnable:1;
           unsigned UartRxParity:2;
           unsigned UartTxParity:2;
           unsigned UartTxStopBits:1;
           unsigned UartRxStopBits:1;
           unsigned UartTxLogic:1;
           unsigned UartRxLogic:1;
           unsigned UartLoopBack: 1;

           unsigned char UartRxIdle;

           unsigned UartTxWaterLevel:4;
           unsigned UartRxWaterLevel:4;


           MT_UART_STATUS Status;

           unsigned long UartRate;

           unsigned UartReset:1;
           unsigned UartClockOnOff:1;
           unsigned UartEnableHostIntrpt:1;

           unsigned reserve1:5;

           unsigned reserve2:8;        /* Align to word boundry */
} MT_UART_CNFG;

/*****************************************************************************
*
*   Module's Global Function Prototypes
*
*****************************************************************************/

#undef MT_EXTERN
#undef MT_I
#endif /* __MT_UART_H__ */

