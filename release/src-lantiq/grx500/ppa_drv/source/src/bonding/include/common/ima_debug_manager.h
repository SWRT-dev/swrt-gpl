
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#ifndef __IMA_DEBUG_MANAGER_H__
#define __IMA_DEBUG_MANAGER_H__

#ifdef ENABLE_IOCTL_DEBUG
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include "common/ima_atm_bond_ioctl.h"

#define TX 0
#define RX 1

typedef enum debug_type {
	debug_type_none = 0,
	debug_type_tx_sid_offset,
	debug_type_tx_sid_set,
	debug_type_tx_qdc,
	debug_type_tx_abort,
	debug_type_tx_flush,
	debug_type_tx_delay,
	debug_type_rx_sid_offset,
	debug_type_rx_sid_set,
	debug_type_rx_sid_skip,
	debug_type_rx_timeout,
	debug_type_rx_full
} debug_type_t;

struct bond_debug_priv {
	struct {
		spinlock_t lock;
		bool xmit_enable;
		struct {
			union {
				u16 value;
				s16 offset;
			} u;
			s16 no_of_cells;
			debug_type_t type;
		} sid;
		u64 delay[2];
	} tx_dbg;

	struct {
		spinlock_t lock;
		struct {
			union {
				u16 value;
				s16 offset;
			} u;
			s16 no_of_cells;
			u32 last_sid;
			debug_type_t type;
		} sid[2];
		en_status_t full[2];
		struct {
			s16 no_of_cells;
		} sid_skip[2];
		u64 timeout_delay;
	} rx_dbg;

	struct {
		spinlock_t lock;
		en_status_t asm_print[2]; /* index 0 for TX and index 1 for RX */
	} asm_dbg;
};
extern struct bond_debug_priv g_dbg_privdata;
#define DBGPRIV (&g_dbg_privdata)

/* APIs exported to core modules, like Tx, Rx and ASM */
extern u32 debug_get_tx_next_sid(u32 next_sid);
#define debug_tx_sid_condn(next_sid)				\
{								\
	u32 new_next_sid = 0;					\
								\
	new_next_sid = debug_get_tx_next_sid(next_sid);		\
	SETSID(cell, new_next_sid, sid_format_is_8bit);		\
}
#define debug_tx_disable_condn() 				\
{ 								\
	bool is_tx_dis = false;					\
								\
	spin_lock_bh(&DBGPRIV->tx_dbg.lock); 			\
	is_tx_dis = (DBGPRIV->tx_dbg.xmit_enable == false);	\
	spin_unlock_bh(&DBGPRIV->tx_dbg.lock); 			\
								\
	if (is_tx_dis) 						\
		continue; 					\
}
extern void debug_tx_delay_condn(int linkid);

extern void debug_rx_sid_condn(void);
extern void debug_rx_sid_skip_condn(void);
extern void debug_rx_timeout_condn(void);
extern bool debug_is_rx_link_full(int linkid);
#define debug_rx_full_condn(linkid)		\
{						\
	if (debug_is_rx_link_full(linkid))	\
		break;				\
}

extern void dump_asm_cell(uint32_t lineidx, u8 *cell, char *desc, int direction);

/* APIs exported to debug interface, like IOCTL */
extern u8 ima_debug_tx_sid_offset(s16 offset, u32 no);
extern u8 ima_debug_tx_sid_set(u16 value, u32 no);
extern u8 ima_debug_tx_qdc(u64 delay);
extern u8 ima_debug_tx_abort(void);
extern u8 ima_debug_tx_flush(void);
extern u8 ima_debug_tx_delay(u16 link, u64 delay);

extern u8 ima_debug_rx_sid_offset(s16 offset, u32 no_of_cells, u16 link);
extern u8 ima_debug_rx_sid_set(u16 value, u32 no_of_cells, u16 link);
extern u8 ima_debug_rx_sid_skip(u32 no_of_cells, u16 link);
extern u8 ima_debug_rx_timeout(u64 delay);
extern u8 ima_debug_rx_full(u16 link, en_status_t status);

extern u8 ima_debug_stats(st_stats_t *user_stats);
extern u8 ima_debug_rx_asm_get(en_status_t status);
extern u8 ima_debug_tx_asm_get(en_status_t status);

#else /* ENABLE_IOCTL_DEBUG */
#define debug_tx_sid_condn(...)
#define debug_tx_disable_condn(...)
#define debug_tx_delay_condn(...)

#define debug_rx_sid_condn(...)
#define debug_rx_sid_skip_condn(...)
#define debug_rx_timeout_condn(...)
#define debug_rx_full_condn(...)
#endif /* ENABLE_IOCTL_DEBUG */

extern int init_debug_manager(void);
extern void cleanup_debug_manager(void);

#endif /* __IMA_DEBUG_MANAGER_H__ */
