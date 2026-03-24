
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * This file:
 * 1. Functions as the outer skin of Rx Module of IMA+ Bonding driver
 * 2. Glues the system-independent Line Queue Abstraction Library
 * with the actual HW RXOUT Rings below.
 * Within the IMA Bonding Driver, it occupies the layer shown below:
 *
 *    +-------+-------------------------------------+---------------------------+
 *    |       |  Rx Bonding Driver code components  |        Source code        |
 *    |       |        (sorted from UE to LE)       |                           |
 *    +-------+-------------------------------------+---------------------------+
 *    |       | T C  D R I V E R  (upper layer)     |                           |
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |                           |
 *    |   1   | ~ Reassembly FSM Library          ~ | ima_rx_reassembler.c(.h)  |
 *    |   2   | ~ Bonding Library                 ~ | ima_rx_bonding.c(.h)      |
 *    |   3   | ~ ATM Cell Decode Library         ~ | ima_rx_atm_cell.h         |
 *    |   4   | ~ Line Queue Abstraction Library  ~ | ima_rx_queue.c(.h)        |
 *    |       | ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |                           |
 *    |   5   | H/W RXOUT Ring Interface            | ima_atm_rx.c(.h)          |<=
 *    | - - - | - - - - - - - - - - - - - - - - - - | - - - - - - - - - - - - - |
 *    |       | H/W RXOUT RINGS   (lower layer)     |                           |
 *    +-------+-------------------------------------+---------------------------+
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/spinlock.h> // spinlock_t

#include "common/atm_bond.h"
#include "asm/ima_asm.h"
#include "atm/ima_atm_rx.h"
#include "atm/ima_rx_bonding.h"
#include "atm/ima_rx_atm_cell.h"

/* Rx Private Data Structure */
#ifdef __KERNEL__
RxBondingPrivData_t g_privdata;
#endif

/*
 * Task of this function is to initialize the atm receive module.
 */
s32 init_atm_rx(void)
{
	s32 ret = RS_SUCCESS;

	ret = init_rx_bonding();

	return ret;
}

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_atm_rx(void)
{
	s32 ret = RS_SUCCESS;

	cleanup_rx_bonding();

	return ret;
}

void dump_cell(uint32_t lineidx, char *data, int len, char *desc)
{
	ima_rx_atm_header_t *cell = (ima_rx_atm_header_t *)(data);
	char hdr[64];

	/* dump the packet */
	snprintf(hdr, sizeof(hdr), "%s data (lineidx:%d, vpi:%d, vci:%d, sid:%d):",
				desc, lineidx, GETVPI(cell), GETVCI(cell), GETSID(cell, 0));
	print_hex_dump(KERN_INFO, hdr, DUMP_PREFIX_OFFSET, 16, 1, data, len, false);
}

int init_device_rx_metadata(struct bond_drv_cfg_params *cfg)
{
	init_device_rx_bonding(0, cfg);
	init_device_rx_bonding(1, cfg);

	return 0;
}

void cleanup_device_rx_metadata(void)
{
	cleanup_device_rx_bonding(0);
	cleanup_device_rx_bonding(1);
}

//extern int vsprintf(char *buf, const char *fmt, va_list args);
void ImaPrint(const char *fmt, ...)
{
	va_list args;
	char msg[256];

	va_start(args, fmt);
	vsprintf(msg, fmt, args);
	va_end(args);

	pr_info("%s", msg);
}

void ProcessASM(uint8_t *cell, uint32_t lineidx)
{
	//ImaPrint("ASM received on line %d as 0x%px\n", lineidx, cell);
	//ImaPrint("Message Type: 0x%02X, ASM Identifier: 0x%02X, TxLink Number: %d, Rx Link Status: %c%c, %c%c, Tx Link Status: %c%c, %c%c\n",
	//							cell[5], cell[6], cell[7]&0x1f,
	//							cell[9]&0x80 ? '1' : '0', cell[9]&0x40 ? '1' : '0', cell[9]&0x20 ? '1' : '0', cell[9]&0x10 ? '1' : '0',
	//							cell[17]&0x80 ? '1' : '0', cell[17]&0x40 ? '1' : '0', cell[17]&0x20 ? '1' : '0', cell[17]&0x10 ? '1' : '0');
	asm_cell_received(cell, lineidx);
}
