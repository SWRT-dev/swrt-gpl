
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_ASM_H__
#define __IMA_ASM_H__

#include "atm/ima_rx_atm_cell.h"

/* ASM FSM Private Data */
typedef struct {

	u32 asmcount;

	struct {
		/*ASMFSMState_e*/ u32 state;
		/*ASMTxLinkStatusFSMState_e*/ u32 txlinkstatus[2];
		/*ASMRxLinkStatusFSMState_e*/ u32 rxlinkstatus[2];
	} fsm;

	struct {
		spinlock_t spinlock;

		struct {

			/*ASMFSMEvent_e*/ u32 event;
			u32 linkid;
			
		} elements[32];

		u32 head;
		u32 tail;
		u32 size;
		u32 count;
		bool prev_asm_received[2];
		u8   lastasmcellbuff[56];
		u32  lastasmlinkid;
		bool lastasmcellconsumed;
		bool asm_id_registered;
		u8   last_rx_asm_id;
		u8   last_group_lost_cells;
	} eventqueue;

	struct {
		u32 next_tx_asm_id;

		struct {
			bool enabled;
			u64  jiffies;
			u8   *asmcell;
		} timers[2];
	} asm_transmit;

	struct {
		u16 grpid;
		u8  msg_type;
		u8  num_links;
		u8  link_map[2]; // Mapping from Key: CPE's link-id; to Value: CO's link-idx
		u8  link_rmap[2]; // Mapping from Key: CO's link-idx; to Value: CPE's link-id
	} bonding_grp_cfg;

	struct {
		spinlock_t spinlock;

		u64 asmsent[2];
		u64 asmdropped[2];
		u64 asmdiscarded[2];
		u64 showtimeentry[2];
		u64 showtimeexit[2];
		u64 grouplostcells;
	} stats;

} ASMPrivData_t;

extern ASMPrivData_t g_asm_privdata;

int init_device_asm_metadata(struct bond_drv_cfg_params *cfg);
void process_asm(void);
void cleanup_device_asm_metadata(void);
void asm_showtime_enter(u32 linkid);
void asm_showtime_exit(u32 linkid);
void asm_cell_received(u8 *buff, u32 linkid);

#endif /* __IMA_ASM_H__*/
