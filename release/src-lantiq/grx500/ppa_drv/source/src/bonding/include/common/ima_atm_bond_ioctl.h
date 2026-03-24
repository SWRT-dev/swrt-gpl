
/****************************************************************************** 
         Copyright 2020 - 2021 MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_ATM_BOND_IOCTL_H__
#define __IMA_ATM_BOND_IOCTL_H__

/*
 * This file carries ioctl interfaces of the bonding driver
 */

/*******************************************************************************
 *                              Transmit side ioctls
 *******************************************************************************/
#define ATM_DRIVER_MAGIC_NUM 'x'

/*******************************************************************
 * Add a (configurable, signed) offset to the SID of N consecutive
 * Non-ASM cells (before distribution to both Tx queues)
 *******************************************************************/
#define IMA_IMAP_DEBUG_TX_SID_OFFSET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x01, int)

/*********************************************************************
 * Set the SID of next Non-ASM cell to configured value and (N-1)
 * consecutive Non-ASM cells (before distribution to both Tx queues)
 *********************************************************************/
#define IMA_IMAP_DEBUG_TX_SID_SET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x02, int)

/***************************************************************
 * Handle TX queue delay compensation - Queue frames with much
 * higher speed than the link rate
 ***************************************************************/
#define IMA_IMAP_DEBUG_TX_QDC _IOWR(ATM_DRIVER_MAGIC_NUM, 0x03, int)

/**************************************************************
 * Abort - Don't transmit - Empty buffer : Asynchronous event
 **************************************************************/
#define IMA_IMAP_DEBUG_TX_ABORT _IOWR(ATM_DRIVER_MAGIC_NUM, 0x04, int)

/********************************************
 * Flush out - transmit out - graceful exit
 ********************************************/
#define IMA_IMAP_DEBUG_TX_FLUSH _IOWR(ATM_DRIVER_MAGIC_NUM, 0x05, int)

/********************************
 * Force link specific Tx Delay
 ********************************/
#define IMA_IMAP_DEBUG_TX_DELAY _IOWR(ATM_DRIVER_MAGIC_NUM, 0x06, int)

/*******************************************************************************
 *                            End : Transmit side ioctls
 *******************************************************************************/

/*******************************************************************************
 *                           Receive side ioctls
 *******************************************************************************/

/*******************************************************************************
 * Add a (configurable, signed) offset value to the SID of N consecutive
 * Non-ASM cells in the selected receive queue(s)
 *******************************************************************************/
#define IMA_IMAP_DEBUG_RX_SID_OFFSET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x21, int)

/*******************************************************************************
 * Set the SID of next Non-ASM cell to configured value and (N-1) consecutive
 * Non-ASM cells to following (+1) values in the selected receive queue(s)
 *******************************************************************************/
#define IMA_IMAP_DEBUG_RX_SID_SET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x22, int)

/*******************************************************************************
 * Discard a configurable number of Non-ASM cells in the selected receive
 * queue(s)
 *******************************************************************************/
#define IMA_IMAP_DEBUG_RX_SID_SKIP _IOWR(ATM_DRIVER_MAGIC_NUM, 0x23, int)

/*******************************************************************************
 * Wait for configured time for arrival of next SID to arrive.
 *******************************************************************************/
#define IMA_IMAP_DEBUG_RX_TIMEOUT _IOWR(ATM_DRIVER_MAGIC_NUM, 0x24, int)

/*******************************************************************************
 * Bonding driver to not sent any frames to VRX driver and create the RX
 * buffer full scenario
 * Bonding driver - reassembly feature to detect buffer full
 *******************************************************************************/
#define IMA_IMAP_DEBUG_RX_FULL _IOWR(ATM_DRIVER_MAGIC_NUM, 0x25, int)

/*******************************************************************************
 *                           End : Receive side ioctls
 *******************************************************************************/

/*******************************************************************************
 *                                  General ioctls
 *******************************************************************************/

/*******************************************************************************
 * Get debug statistics & other counters/ state of driver in user space
 * Buffer allocated by user space driver - lock mechanism required
 *******************************************************************************/
#define IMA_IMAP_DEBUG_STATS _IOWR(ATM_DRIVER_MAGIC_NUM, 0x30,  char*)

/******************************
 * Output Rx ASM cell content
 ******************************/
#define IMA_IMAP_DEBUG_RX_ASM_GET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x31, int)

/******************************
 * Output Tx ASM cell content
 ******************************/
#define IMA_IMAP_DEBUG_TX_ASM_GET _IOWR(ATM_DRIVER_MAGIC_NUM, 0x32, int)

/**************************
 * Set link rate per link
 **************************/
#define IMA_IMAP_QOS_SET_LINK_RATE _IOWR(ATM_DRIVER_MAGIC_NUM, 0x33, int)

/*******************************************************************************
 *                               End : General ioctls
 *******************************************************************************/

/********************
 * Character device
 ********************/
#define IMA_BONDING_DEVICE_NODE "/dev/ima_bond"

typedef enum en_status {
	enable = 0,
	disable
} en_status_t;

/*******************
 * ioctl parameter
 *******************/
typedef enum {
	IMA_IMAP_QOS_BONDING_LINK_MODE_DUAL = 0, /* "Dual link mode" operation (default) */
	IMA_IMAP_QOS_BONDING_LINK_MODE_SINGLE = 1 /* "Single link mode" operation */
} IMA_IMAP_QOS_BondingLinkMode_t;

/* IMA_IMAP_QOS_SET_LINK_RATE */
typedef struct st_link_rate {
	unsigned int link0Rate;
	unsigned int link1Rate;
	IMA_IMAP_QOS_BondingLinkMode_t bondingLinkMode;
	unsigned int reserved;
} st_link_rate_t;

/* IMA_IMAP_DEBUG_TX_SID_OFFSET */
typedef struct st_tx_sid_offset {
	short offset;
	unsigned int no_of_cells;
} st_tx_sid_offset_t;

/* IMA_IMAP_DEBUG_TX_SID_SET */
typedef struct st_tx_sid_set {
	unsigned short value;
	unsigned int no_of_cells;
} st_tx_sid_set_t;

/* IMA_IMAP_DEBUG_TX_QDC */
typedef struct st_tx_qdc {
	/* In nano seconds */
	unsigned long long delay;
} st_tx_qdc_t;

/* IMA_IMAP_DEBUG_TX_DELAY */
typedef struct st_tx_delay {
	unsigned short link;

	/* In micro seconds */
	unsigned long long delay;
} st_tx_delay_t;

/* IMA_IMAP_DEBUG_RX_SID_OFFSET */
typedef struct st_rx_sid_offset {
	short offset;
	unsigned int no_of_cells;
	unsigned short link;
} st_rx_sid_offset_t;

/* IMA_IMAP_DEBUG_RX_SID_SET */
typedef struct st_rx_sid_set {
	unsigned short value;
	unsigned int no_of_cells;
	unsigned short link;
} st_rx_sid_set_t;

/* IMA_IMAP_DEBUG_RX_SID_SKIP */
typedef struct st_rx_sid_skip {
	unsigned int no_of_cells;
	unsigned short link;
} st_rx_sid_skip_t;

/* IMA_IMAP_DEBUG_RX_TIMEOUT */
typedef struct st_rx_timeout {
	/* In micro seconds */
	unsigned long long delay;
} st_rx_timeout_t;

/* IMA_IMAP_DEBUG_RX_FULL */
typedef struct st_rx_full {
	en_status_t status;
	unsigned short link;
} st_rx_full_t;

/* IMA_IMAP_DEBUG_TX_ASM_GET, IMA_IMAP_DEBUG_RX_ASM_GET */
typedef struct st_print_asm {
	en_status_t status;
} st_print_asm_t;

typedef enum en_rx_frame_discard_type {
	SID_JUMP = 0,
	INVALID_LENGTH,
	INVALID_CRC32,
	INVALID_PT2_OR_CLP,
	SIZE_OVERFLOW,
	ALLOC_FAILURE,
	MAX_TYPES
} en_rx_frame_discard_type_t;;

typedef struct st_stats {
	unsigned int host_txin_ring_count;
	unsigned long long int insufficient_tx_buffer_stack_backpressure;
	unsigned long long int ep_total_dropped;
	int cell_buff_mallocd;
	int cell_buff_free;
	int cell_buff_locked;
	unsigned int sched_ring_count;

	unsigned int tx_ep_count[2];
	unsigned long long int tx_asm_count[2];
	unsigned long long int tx_atm_count[2];
	unsigned long long int tx_asm_dropped_count[2];
	unsigned long long int tx_total_atm_dropped_count;
	unsigned long long int tx_no_ep_freespace_backpressure[2];

	char co_rx_link_status[2];
	char co_tx_link_status[2];
	unsigned long long int co_group_lost_cell;

	unsigned long long int rx_asm_count[2];
	unsigned long long int rx_atm_count[2];
	unsigned long long int rx_dropped_cells;
	unsigned long long int rx_timedout_cells;
	unsigned long long int rx_missing_cells;
	unsigned long long int rx_accepted_frame_count;
	unsigned long long int rx_discarded_frame_count[MAX_TYPES];
} st_stats_t;

#endif /* __IMA_ATM_BOND_IOCTL_H__ */

