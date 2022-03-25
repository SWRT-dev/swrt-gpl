/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2009~2013 Lei Chuanhua <chuanhua.lei@lantiq.com>
 */

#ifndef LANTIQ_SSC_H
#define LANTIQ_SSC_H

/*!
 * \defgroup IFX_SSC SSC bus driver module
 * \brief UEIP Project - SSC bus driver module, support all CPEs.
 */

/*!
 * \defgroup IFX_SSC_DEFINITIONS Definition and structions
 * \ingroup IFX_SSC
 * \brief definitions for ifx ssc driver
 */

/*!
 * \defgroup IFX_SSC_FUNCTIONS external APIs
 * \ingroup IFX_SSC
 * \brief IFX ssc external driver functions
 */

/*!
 * \defgroup IFX_SSC_INTERNAL Internal functions
 * \ingroup IFX_SSC
 * \brief IFX ssc internal driver functions
 */

/*!
 * \file ifx_ssc.h
 * \ingroup IFX_SSC
 * \brief header file for SSC bus driver external interface
 */

/*!
 * \addtogroup IFX_SSC_DEFINITIONS
 */
/* @{ */
/*! \enum  IFX_SSC_PRIO_t
 *   \brief Set communication priority of SSC connection
 *
 *   Three priority levels are defined. Low-level priority queue used for
 *   applications like FLASH driver (ifx_ssc_prio_low). normal priority
 *   queue used for applications like display (ifx_ssc_prio_mid).
 *   High priority queue used for applications like RTP packet transfer
 *   (ifx_ssc_prio_high).
 */
typedef enum {
	IFX_SSC_PRIO_LOW = 0,  /*!< Low Priority queue.
				*  For FLASH driver, etc.
				*/
	IFX_SSC_PRIO_MID,      /*!< Normal Priority queue.
				*  For LCD, display data, etc.
				*/
	IFX_SSC_PRIO_HIGH,     /*!< High priority queue.
				*  Ror RTP voice, etc.
				*/
	IFX_SSC_PRIO_ASYNC,    /*!< Tasklet priority (This is the highest
				*  supported priority) For this priority
				*  level only the asynchronous API set of
				*  the SSC driver can be called. These
				*  functions stay unblocked and a callback
				*  function is called when the request is
				*  processed. This allows that the APIs are
				*  called from tasklet level. The callback
				*  function is always called on tasklet
				*  level
				*/
} IFX_SSC_PRIO_t;

#define IFX_SSC_PRIO_MAX IFX_SSC_PRIO_ASYNC

/*! \enum  IFX_SSC_MODE_t
 *  \brief Defines the Ssc hardware mode settings supported
 *
 *  Because there is no official specification, what exactly SPI is and what
 *  not, it is necessary to consult the data sheets of the components one
 *  wants to use. Important are the permitted clock frequencies and the type
 *  of valid transitions. There are no general rules for transitions where
 *  data should be latched. Although not specified by Motorola, in practice
 *  four modes are used. These four modes are the combinations of CPOL and
 *  CPHA. In table 1, the four modes are listed.If the phase of the clock
 *  is zero, i.e. CPHA = 0, data is latched at the rising edge of the clock
 *  with CPOL = 0, and at the falling edge of the clock with CPOL = 1. If
 *  CPHA = 1, the polarities are reversed. CPOL = 0 means falling edge,
 *  CPOL = 1 rising edge.The micro controllers from Motorola allow the
 *  polarity and the phase of the clock to be adjusted. A positive polarity
 *  results in latching data at the rising edge of the clock. However data
 *  is put on the data line already at the falling edge in order to stabilize.
 *  Most peripherals which can only be slaves, work with this configuration.
 *  If it should become necessary to use the other polarity, transitions are
 *  reversed.
 */
typedef enum {
	IFX_SSC_MODE_0 = 0, /*!< CPOL=0,CPHA=0 */
	IFX_SSC_MODE_1, /*!< CPOL=0,CPHA=1 */
	IFX_SSC_MODE_2, /*!< CPOL=1,CPHA=0 */
	IFX_SSC_MODE_3, /*!< CPOL=1,CPHA=1 */
	IFX_SSC_MODE_UNKNOWN, /*!< Unknown SPI mode */
} IFX_SSC_MODE_t;

/*! \enum  IFX_SSC_HANDL_TYPE_t
 *  \brief Defines the SPI handler type supported
 */
typedef enum {
	IFX_SSC_HANDL_TYPE_SYNC = 0,   /*!< Only SYNC handler which be used
					* by sync application
					*/
	IFX_SSC_HANDL_TYPE_ASYNC,      /*!< Only ASYNC handler which be used
					* by async application
					*/
} IFX_SSC_HANDL_TYPE_t;

/*! \enum  IFX_SSC_DLX_t
 *  \brief Set communication duplex mode of SSC connection
 *
 *  The duplex mode is used to notify SSC bus driver by SSC device driver
 *  about what kind of communication mode should be used. Which duplex mode
 *  will be used depends on the SSC device driver instead of SSC bus driver.
 */
typedef enum {
	IFX_SSC_HALF_DUPLEX = 0,/*!< Half Duplex. Interface is used in half
				 *   duplex when calling \ref ifx_sscTxRx or
				 *   \ref ifx_sscAsyncTxRx. The TX path is
				 *   servered before the RX path.
				 */
	IFX_SSC_FULL_DUPLEX,	/*!< Full Duplex. Interface is used in full
				 *   duplex when calling \ref ifx_sscTxRx or
				 *   \ref ifx_sscAsyncTxRx. The TX-and RX-
				 *   path is servered simultaneously.
				 */
} IFX_SSC_DLX_t;

/*!< \typedef IFX_CS_DATA
 *   \brief Definition of device specific data for chip select
 */
typedef int IFX_CS_DATA;
enum {
	IFX_SSC_CS_ON = 0,	IFX_SSC_CS_OFF,
};

#define IFX_SSC_WHBGPOSTAT_OUT0_POS  0
#define IFX_SSC_WHBGPOSTAT_OUT1_POS  1
#define IFX_SSC_WHBGPOSTAT_OUT2_POS  2
#define IFX_SSC_WHBGPOSTAT_OUT3_POS  3
#define IFX_SSC_WHBGPOSTAT_OUT4_POS  4
#define IFX_SSC_WHBGPOSTAT_OUT5_POS  5
#define IFX_SSC_WHBGPOSTAT_OUT6_POS  6
#define IFX_SSC_WHBGPOSTAT_OUT7_POS  7

/*! \typedef IFX_SSC_CS_CB_t
 *  \brief Chip Select Callback function type declaration
 *
 *   csq csq=0, i.e. CS low (active). csq=1, i.e.CS high (idle).
 *   cs_data This is the device number in case if more than one device is using
 *   the same registered driver. e.g. two VINETICs, each with it's own CS.
 */
typedef int (*IFX_SSC_CS_CB_t) (u32 csq, IFX_CS_DATA cs_data);

/*! typedef ifx_ssc_async_fkt_cb_t
 *  \brief Callback definition for asynchronous SSC API calls. This callback is
 *  called by the SSC driver on tasklet level after the request is completed or
 *  exit with an error.
 *
 *  \param handle Funktion handle that provided for callback registration
 *  during the SSC asynchronous API call.
 *  \param retvalue This return value describe if the asynchronous request
 *  identified an error and worked successfully.
 */
typedef void (*ifx_ssc_async_fkt_cb_t) (int handle, int retvalue);

/*! typedef IFX_SSC_ASYNC_CALLBACK_t
 *  \brief Parameter to specify the asynchronous callback. It is called
 *  by the SSC tasklet after the requested transmission or locking is done.
 */
typedef struct {
	ifx_ssc_async_fkt_cb_t pFunction;/*!< Callback function pointer,
					  * called in the SSC tasklet when
					  *the requested command is executed
					  */
	int functionHandle;	/*!< Callback function handle. This parameter
				 *   is transparently given to the callback
				 *   function without any modification by the
				 *   SSC driver
				 */
} IFX_SSC_ASYNC_CALLBACK_t;

/*! \brief Parameter structure used to configure an SSC connection "ConnId". */
typedef struct {
	IFX_SSC_MODE_t ssc_mode; /*!< Defines the hardware setup mode of
				  *   the SSC
				  */
	IFX_SSC_PRIO_t ssc_prio; /*!< The communication priority of SSC
				  *   connection. Three priority levels
				  *   are defined. Low-level priority
				  *   queue used for applications like
				  *   FLASH driver (ifx_ssc_prio_low).
				  *   Normal priority queue used for
				  *   applications like display
				  *   (ifx_ssc_prio_mid). High priority
				  *   queue used for applications like RTP
				  *   packet transfer (ifx_ssc_prio_high).
				  *   The here defined SSC priority
				  *   corresponds to the priority the SSC
				  *   driver application and should be set
				  *   just for information when the driver
				  *   is registered by the application.
				  */
	int baudrate;		/*!< Baudrate used for the ConnId.
				 * This parameter can be later be modified
				 * by a call of ifx_sscSetBaud
				 */
	int fragSize;		/*!< All transmitted and received packets
				 *   should be fragmented in this fragment
				 *   size. Size given in Bytes. A maximum
				 *   of 1024 Bytes is allowed. If the client
				 *   uses a bigger values here, SSC returns
				 *   with error. The client has to take care
				 *   of the fragmentation
				 */
	int maxFIFOSize;	/*!< Maximum packet size in FIFO mode.
				 * All transmitted and received packets are
				 * transmitted in DMA mode if the packet
				 * size is greater than this value. A value
				 * of 148 is recommended at first.Size given
				 * in Bytes
				 */
	IFX_SSC_CS_CB_t csset_cb;	/*!< Function Callback called by SSC
					 *   driver when it starts/stops to
					 *   receive or transmit
					 */
	IFX_CS_DATA cs_data;	/*!< Parameter used for the function
				 *   call of "csSet_cb".
				 */
	IFX_SSC_DLX_t duplex_mode;	/*!< Duplex Mode Selector. Connection
					 * used the SSC interface either in
					 * half- or full- duplex mode.
					 */
} IFX_SSC_CONFIGURE_t;

/*! \typedef IFX_SSC_HANDLE
 *  \brief Definition of the connection handle
 *
 *   as it is used by the client kernel module that use the SSC driver
 *   Inside of the SSC driver, this handle is mapped to an internal
 *   structure that contains the connection specific parameter
 *   (e.g. Baudrate, Chipselect Callbacks, etc.).
 */
typedef void *IFX_SSC_HANDLE;

/* @} */
int ifx_ssc_cs_low(u32 pin);
int ifx_ssc_cs_high(u32 pin);
int ifx_ssc_cs_low_port1(u32 pin);
int ifx_ssc_cs_high_port1(u32 pin);
int ifx_sscLock(IFX_SSC_HANDLE handler);
int ifx_sscUnlock(IFX_SSC_HANDLE handler);
int ifx_sscSetBaud(IFX_SSC_HANDLE handler, unsigned int baud);
int ifx_sscTxRx(IFX_SSC_HANDLE handler, char *tx_buf, u32 tx_len,
		char *rx_buf, u32 rx_len);
int ifx_sscRx(IFX_SSC_HANDLE handler, char *rx_buf, u32 rx_len);
int ifx_sscTx(IFX_SSC_HANDLE handler, char *tx_buf, u32 tx_len);
IFX_SSC_HANDLE ifx_sscAllocConnection(char *dev_name,
				      IFX_SSC_CONFIGURE_t *connid);
int ifx_sscFreeConnection(IFX_SSC_HANDLE handler);
int ifx_sscAsyncTxRx(IFX_SSC_HANDLE handler,
		     IFX_SSC_ASYNC_CALLBACK_t *pCallback,
		     char *txbuf, int txsize, char *rxbuf, int rxsize);
int ifx_sscAsyncTx(IFX_SSC_HANDLE handler,
		   IFX_SSC_ASYNC_CALLBACK_t *pCallback,
		   char *txbuf, int txsize);
int ifx_sscAsyncRx(IFX_SSC_HANDLE handler,
		   IFX_SSC_ASYNC_CALLBACK_t *pCallback,
		   char *rxbuf, int rxsize);
int ifx_sscAsyncLock(IFX_SSC_HANDLE handler,
		     IFX_SSC_ASYNC_CALLBACK_t *pCallback);
int ifx_sscAsyncUnLock(IFX_SSC_HANDLE handler);

#endif /* LANTIQ_SSC_H */

