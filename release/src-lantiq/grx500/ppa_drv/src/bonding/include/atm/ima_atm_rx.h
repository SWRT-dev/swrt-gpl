
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file exposes the functions of Rx Module to IMA+ Bonding driver
 */

#ifndef __IMA_ATM_RX_H__
#define __IMA_ATM_RX_H__

#include "common/ima_common.h"
#include "atm/ima_rx_atm_cell.h"

#ifndef PRINT
extern void ImaPrint(const char *fmt, ...);
#if 1
#define PRINT(format, ...)
#else
#define PRINT(format, ...) ImaPrint("[%s[%d]: " format, __func__, __LINE__, ## __VA_ARGS__)
#endif
#endif

#ifndef REPORT
//  For debugging purpose only by the unit tests
#define REPORT(msg, number1, number2)
#endif

#ifndef PROCESS_ASM
extern void ProcessASM(uint8_t *cell, uint32_t lineidx);  // Processes the ASM cell pointer by cell-arg, received on the line-queue lineidx
#define PROCESS_ASM(cell, lineid)  ProcessASM((cell), (lineid))
#endif

///////////////////////////////////////////////////////////////////////

typedef enum {
	LEARNING,
	READY,
} RxBondingState_t;

typedef enum {
	SIDJUMP = 0,
	INVALIDLENGTH,
	INVALIDCRC32,
	INVALIDPT2ORCLP,
	SIZEOVERFLOW,
	ALLOCFAILURE,
	MAXTYPES
} RxBondingFrameDiscardType_t;

typedef struct {

	struct {
		spinlock_t spinlock;
		bool sid_format_is_8bit;
		u32  sid_range;
		bool active[2];
		bool data_accept[2];
		void (*flush_cb)(void);
		void (*process_cb)(void);
	} config;

	bool sid_format_is_8bit;
	u32  sid_range;
	bool data_accept[2];
	unsigned int       ExpectedSID;
	RxBondingState_t   state;

#ifdef BADFRAME_THEN_DUMPITSCELLS
	u8 celldumpbuffer[200*64];
#endif /* BADFRAME_THEN_DUMPITSCELLS */

#if defined RESEQUENCING_TEST
	struct {
		bool valid;
		union {
			ima_rx_atm_header_t cell;
			u8 buff[56];
		} data;
	} cache[2];
#endif /* RESEQUENCING_TEST */

	struct {
		u32 prev_sid0;
		u32 prev_sid1;
		u32 ttl;
	} sidtimer;

	struct {
#ifdef __KERNEL__
		spinlock_t   rx_spinlock;
#endif
		u32 start;
		u32 start_phys;
		unsigned int rx_size,
					 rx_head,
					 rx_tail,
					 rx_count;
		u32 umt_out_addr;
	} m_st_device_info[2];

	struct {
		uint8_t rejectionmode;
		uint32_t cellcount;
		uint8_t  *frame;
		uint16_t lastsid;
		uint32_t maxcellcount;
	} reassemble;

	// Statistics & Debug
	struct {
#ifdef __KERNEL__
		spinlock_t     spinlock;
#endif
		uint64_t       asmcount[2]; // per link
		uint64_t       nonasmcount[2]; // per link
		uint64_t       droppedcells[2]; // per link
		uint64_t       timedoutcells;
		uint64_t       missingcells;
		uint64_t       acceptedframecount;
		uint64_t       discardedframecount[MAXTYPES];

#ifdef MEASURE_JIFFIES
		u64            start_jiffies; // starting time for first process_rx
		u64            jiffies; // total jiffies spent on process_rx()
#endif

	} stats;

} RxBondingPrivData_t;

extern RxBondingPrivData_t g_privdata;

#ifdef __KERNEL__
#define UPDATE_STATS(field, quantity) \
	do { \
		spin_lock_bh(&(PRIV->stats.spinlock)); \
		PRIV->stats.field += (quantity); \
		spin_unlock_bh(&(PRIV->stats.spinlock)); \
	} while (0)
#else
#define UPDATE_STATS(field, quantity)  (PRIV->stats.field += (quantity))
#endif

/*
 * Task of this function is to initialize the atm receive module.
 */
s32 init_atm_rx(void);

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_atm_rx(void);

int init_device_rx_metadata(struct bond_drv_cfg_params *cfg);
void cleanup_device_rx_metadata(void);
void process_rx(void);

#endif /* __IMA_ATM_RX_H__ */
