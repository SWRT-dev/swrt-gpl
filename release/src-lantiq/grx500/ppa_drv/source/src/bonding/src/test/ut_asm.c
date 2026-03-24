
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <stdio.h> // fprintf
#include <stdint.h> // uint8_t, uint16_t, etc
#include <stdbool.h> // bool
#include <string.h> // memcpy, memset
#include <arpa/inet.h> // ntohs, ntohl, htons, htonl

typedef uint8_t u8;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef struct {} spinlock_t;
#define spin_lock_init(s)
#define spin_lock_bh(s)
#define spin_unlock_bh(s)
#define get_jiffies_64() 0L
struct bond_drv_cfg_params {};
#define pr_crit(...) fprintf( stderr, __VA_ARGS__ )
#define usecs_to_jiffies(x) 100*(x)
#define jiffies64_to_nsecs(x) 10*(x)
#define do_div(a64,b) (a64)
#define crc32_be(seed,buff,bufflen) 0
#define print_hex_dump(...)

void first_asm_received(u8 linkid);
void update_tx_config(bool sid_is_8bit);
void update_rx_config(bool sid_is_8bit);
void activate_tx_link(u32 linkid);
void deactivate_tx_link(u32 linkid);
bool xmt_cell(u8 *cellbuff, u32 linkid);

//#include "common/atm_bond.h"
#include "atm/ima_rx_atm_cell.h"
//#include "atm/ima_tx_module.h"
#include "atm/ima_tx_cell_buffer.h"
//#include "atm/ima_rx_bonding.h"
#include "asm/ima_asm.h"

#include "../asm/ima_asm.c"

struct {

	bool tx_module_updated;
	bool tx_sid_is_8bit;

	bool rx_module_updated;
	bool rx_sid_is_8bit;

	bool tx_link0_activated;
	bool tx_link0_deactivated;
	bool tx_link1_activated;
	bool tx_link1_deactivated;

	bool tx_link0_xmt;
	bool tx_link1_xmt;

	u8 cell[56];

} ut_asm_context;

#define INIT_UT_CONTEXT() ut_asm_context.tx_module_updated = ut_asm_context.rx_module_updated = false; \
                          ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false; \
                          ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; \
                          ut_asm_context.tx_link0_xmt = ut_asm_context.tx_link1_xmt = false; 

void
first_asm_received(u8 linkid)
{
}

void
update_tx_config(bool sid_is_8bit) 
{
	ut_asm_context.tx_module_updated = true;
	ut_asm_context.tx_sid_is_8bit = sid_is_8bit;
}

void
update_rx_config(bool sid_is_8bit) 
{
	ut_asm_context.rx_module_updated = true;
	ut_asm_context.rx_sid_is_8bit = sid_is_8bit;
}

void
activate_tx_link(u32 linkid)
{
	if( linkid == 0 ) {
		ut_asm_context.tx_link0_activated = true;
	} else if( linkid == 1 ) {
		ut_asm_context.tx_link1_activated = true;
	} else {
		fprintf( stderr, "activate_tx_link called with invalid linkid(%u)\n", linkid );
	}
}

void
deactivate_tx_link(u32 linkid)
{
	if( linkid == 0 ) {
		ut_asm_context.tx_link0_deactivated = true;
	} else if( linkid == 1 ) {
		ut_asm_context.tx_link1_deactivated = true;
	} else {
		fprintf( stderr, "deactivate_tx_link called with invalid linkid(%u)\n", linkid );
	}
}

bool
reserve_from_txbuff_stack(u32 count)
{
	return true;
}

void *
pop_from_txbuff_stack(void)
{
	return (void *)ut_asm_context.cell;
}

void
push_into_txbuff_stack(void *buff)
{
}

bool
xmt_cell(u8 *cellbuff, u32 linkid)
{
	memcpy(ut_asm_context.cell, cellbuff, 56);

	if( linkid == 0 ) {
		ut_asm_context.tx_link0_xmt = true;
	} else if( linkid == 1 ) {
		ut_asm_context.tx_link1_xmt = true;
	} else {
		fprintf( stderr, "xmt_cell() called with invalid linkid(%u)\n", linkid );
	}
	return true;
}

/*===================================================================*
 *  UNIT TESTS                                                       *
 *                                                                   *
 *  To run:                                                          *
 *    $> cd source/ppa_drv/src/bonding                               *
 *    $> gcc -Wall -I./include src/asm/ima_asm_tmp.c                 *
 *    $> ./a.out                                                     *
 *===================================================================*/

#include <stdio.h>

#define BEGIN_UNIT_TESTS() int main( int argc, char *argv[] ) { int total=0, passed=0;
#define END_UNIT_TESTS() \
		printf( "===========================================\n" ); \
		printf( "TOTAL TESTS: %d	PASSED: %d	FAILED: %d\n", total, passed, total - passed ); \
		printf( "===========================================\n" ); \
		return 0; \
	}\
	typedef int DUMMY;
#define BEGIN_TEST(desc) { printf( "Unit test %d: %s\n{\n", total+1, (desc) ); int pass=1; INIT_UT_CONTEXT()
#define END_TEST() ++total; printf( "}\nUnit test %d ", total ); if( pass ) { ++passed; printf( "passed\n" ); } else printf( "failed\n" ); }
#define PASSED() (1==pass)
#define PRINTF(format, ...) printf( "	" format, ## __VA_ARGS__);
#define NULL_STRINGIFY(ptr) (NULL==(void *)(ptr) ? "NULL" : "NOT NULL")
#define ASSERT_BOOL(str, expected, actual) PRINTF( "%s: Expected: %s, Actual: %s\n", (str), (expected)?"true":"false", (actual)?"true":"false" );\
				if( (expected) != (actual) ) pass=0;
#define ASSERT_NUM(str, expected, actual) PRINTF( "%s: Expected: %u, Actual: %u\n", (str), (expected), (actual) );\
				if( (expected) != (actual) ) pass=0;
#define ASSERT_HEX(str, expected, actual) PRINTF( "%s: Expected: 0x%lx, Actual: 0x%x\n", (str), (expected), (actual) );\
				if( (expected) != (actual) ) pass=0;
#define ASSERT_MEMCMP(str, binstr1, binstr2, len) PRINTF( "%s: they are %s\n", (str), memcmp((void *)(binstr1),(void *)(binstr2), (len)) ? "not identical" : "identical" );\
				if( memcmp((void *)(binstr1), (void *)(binstr2), (len)) != 0 ) pass=0;
#define ASSERT_NULL(str, actual) PRINTF( "%s: Expected: NULL, Actual: %s\n", (str), NULL_STRINGIFY((actual)) );\
				if( NULL != (actual) ) pass=0;
#define ASSERT_NOT_NULL(str, actual) PRINTF( "%s: Expected: NOT NULL, Actual: %s\n", (str), NULL_STRINGIFY((actual)) );\
				if( NULL == (actual) ) pass=0;

BEGIN_UNIT_TESTS()
{
	u8 sample_asm[56] = {0x0,0x0,0x1,0x42,0x0,0x0,0x0,0x0,
				0x0,0x4,0x1,0x2,0x70,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x90,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf,0x22,0x0,0x0,0x0,0x0,0x0,0x0,0x8a,0x78,0x7,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x28,0x68,0x82,0x3c,0x76
				//0x0,0x1,0x1,0x2,0xa0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xa0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf,0x22,0x0,0x0,0x0,0x0,0x0,0x0,0x00,0xa2,0xcc,0x8b,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x28,0x78,0x52,0x40,0x55
				//0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x28,0x78,0x52,0x40,0x55
		};

	/* 1. Unit Test #1.a */
	BEGIN_TEST( "ASM-Cell C-struct: Correctness in mapping from byte array to C-struct" ) {

		asm_cell_t* ptr = (asm_cell_t*)sample_asm;
		PRINTF( "mapped sample asm to struct\n" );

		ASSERT_NUM( "msg type", 0, ptr->msg_type );
		ASSERT_NUM( "asm id", 4, ptr->asm_id );
		ASSERT_NUM( "tx lnk no", 1, ptr->tx_lnk_no );
		ASSERT_NUM( "no of lnk", 2, ptr->num_links );
		ASSERT_NUM( "rx lnk0 status", 1, ptr->rx_link0_status );
		ASSERT_NUM( "rx lnk1 status", 3, ptr->rx_link1_status );
		ASSERT_NUM( "tx lnk0 status", 2, ptr->tx_link0_status );
		ASSERT_NUM( "tx lnk1 status", 1, ptr->tx_link1_status );
		ASSERT_NUM( "grp id", 3874, ntohs(ptr->grpid));
		ASSERT_NUM( "lnk0 rx asm status", 0, ptr->lnk0_rx_asm_status );
		ASSERT_NUM( "lnk1 rx asm status", 0, ptr->lnk1_rx_asm_status );
		ASSERT_NUM( "grp cell lost", 0, ptr->grp_lost_cell );
		ASSERT_NUM( "req tx delay", 0, ntohs(ptr->req_tx_delay));
		ASSERT_NUM( "act tx delay", 0, ntohs(ptr->act_tx_delay));
		ASSERT_NUM( "len", 40, ntohs(ptr->len));
		ASSERT_NUM( "crc", 1753365622, ntohl(ptr->crc));

	} END_TEST();

	/* 2. Unit Test #1.b */
	BEGIN_TEST( "ASM-Cell C-struct: Correctness in mapping from struct to byte array" ) {

		asm_cell_t cell;
		u8 buff[56];

		memset(&cell, 0, sizeof(cell));
		cell.msg_type = 0x0;
		cell.asm_id = 0x4;
		cell.tx_lnk_no = 0x1;
		cell.num_links = 0x2;
		cell.rx_link0_status = 1;
		cell.rx_link1_status = 3;
		cell.tx_link0_status = 2;
		cell.tx_link1_status = 1;
		cell.grpid = htonl(3874);
		cell.lnk0_rx_asm_status = 0x0;
		cell.lnk1_rx_asm_status = 0x0;
		cell.grp_lost_cell = 0x0;
		cell.req_tx_delay = 0x0;
		cell.act_tx_delay = 0x0;
		cell.len = htons(0x28);

		memcpy((void*)buff, (void*)&cell, 56);

		ASSERT_MEMCMP("memcmp till before ts-field", buff + 8, sample_asm + 8, 28);
		ASSERT_MEMCMP("memcmp after ts-field", buff + 40, sample_asm + 40, 12);

	} END_TEST();

	/* 3. Unit Test #2.1 */
	BEGIN_TEST( "FSM: Verify that init() function sets the state of ASM-FSM, Tx-Link-Status FSMs and Rx-Link Status FSMs correctly to LEARNING, TX-NON-PROVISIONED and RX-NON-PROVISIONED respectively" ) {

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is LEARNING", LEARNING, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[1] );

	} END_TEST();

	/* 4. Unit Test #2.2 */
	BEGIN_TEST( "FSM: Verify transition from LEARNING to LEARNING state for event: SHOWTIME_ENTRY for link-0; TX & RX Link Status of only link-0 changes to TX-ACCEPTABLE & RX-SHOULDNT-BE-USED" ) {

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is LEARNING", LEARNING, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-SHOULDNT-BE-USED", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[1] );

	} END_TEST();

	/* 5. Unit Test #2.3 */
	BEGIN_TEST( "FSM: Verify transition from LEARNING to LEARNING state for event: SHOWTIME_ENTRY for link-1; TX & RX Link Status of only link-1 changes to TX-ACCEPTABLE & RX-SHOULDNT-BE-USED" ) {
		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is LEARNING", LEARNING, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-SHOULDNT-BE-USED", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[1] );

	} END_TEST();

	/* 6. Unit Test #2.4 */
	BEGIN_TEST( "FSM: Verify transition from LEARNING to STEADY state for event: NORMAL_ASM for link-0; without prior SHOWTIME_ENTRY on any link" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 1;
		cell.rx_link0_status = 0; // CO proposes to CPE that link-0 is unusable for reception
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 0; // CO notifies CPE that link-0 is unusable for transmission
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[1] );
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);

	} END_TEST();

	/* 7. Unit Test #2.5 */
	BEGIN_TEST( "FSM: Verify transition from LEARNING to STEADY state for event: NORMAL_ASM for link-0; after SHOWTIME_ENTRY on link-0" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 1; // CO proposes to CPE not to xmt in link-0
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 1; // CO notifies CPE it is not ready to accept in link-0
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-SHOULDNT-BE-USED", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-NON-PROVISIONED", RX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[1] );
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);

	} END_TEST();

	/* 8. Unit Test #2.6 */
	BEGIN_TEST( "FSM: Verify transition from LEARNING to STEADY state for event: NORMAL_ASM for link-0; after SHOWTIME_ENTRY on link-0 & link-1" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 1; // CO proposes to CPE not to xmt in link-0
		cell.rx_link1_status = 1; // CO proposes to CPE not to xmt in link-1
		cell.tx_link0_status = 1; // CO notifies CPE it is not ready to accept in link-0
		cell.tx_link1_status = 1; // CO notifies CPE it is not ready to accept in link-1
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );

		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_NUM("Rx Link Status of Link-0 is RX-SHOULDNT-BE-USED", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[0] );
		ASSERT_NUM("Rx Link Status of Link-1 is RX-SHOULDNT-BE-USED", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[1] );
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);

	} END_TEST();

	/* 9. Unit Test #2.7 */
	BEGIN_TEST( "FSM: Verify during transition from LEARNING to STEADY state for event: NORMAL_ASM for link-0; the Tx and Rx modules get correctly updated that SID-format is 8-bits" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 1; // CO proposes to CPE not to xmt in link-0
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 8-bit SID format", true, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 8-bit SID format", true, ut_asm_context.rx_sid_is_8bit);

	} END_TEST();

	/* 10. Unit Test #2.8 */
	BEGIN_TEST( "FSM: Verify during transition from LEARNING to STEADY state for event: NORMAL_ASM for link-1; the Tx and Rx modules get correctly updated that SID-format is 8-bits" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 0; // CO proposes to CPE that link-0 is unusable for reception
		cell.rx_link1_status = 1; // CO proposes to CPE not to xmt in link-1
		cell.tx_link0_status = 0; // CO notifies CPE that link-0 is unusable for transmission
		cell.tx_link1_status = 1; // CO notifies CPE it is not ready to accept in link-1
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 8-bit SID format", true, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 8-bit SID format", true, ut_asm_context.rx_sid_is_8bit);

	} END_TEST();

	/* 11. Unit Test #2.9 */
	BEGIN_TEST( "FSM: Verify during transition from LEARNING to STEADY state for event: NORMAL_ASM for link-0; the Tx and Rx modules get correctly updated that SID-format is 12-bits" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );

		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 1; // CO proposes to CPE not to xmt in link-0
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 1; // CO notifies CPE it is not ready to accept in link-0
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 12-bit SID format", false, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 12-bit SID format", false, ut_asm_context.rx_sid_is_8bit);

	} END_TEST();

	/* 12. Unit Test #2.10 */
	BEGIN_TEST( "FSM: Verify during transition from LEARNING to STEADY state for event: NORMAL_ASM for link-1; the Tx and Rx modules get correctly updated that SID-format is 12-bits" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );

		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 0; // CO proposes to CPE that link-0 is unusable for reception
		cell.rx_link1_status = 1; // CO proposes to CPE not to xmt in link-1
		cell.tx_link0_status = 0; // CO notifies CPE that link-0 is unusable for transmission
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 12-bit SID format", false, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 12-bit SID format", false, ut_asm_context.rx_sid_is_8bit);

	} END_TEST();

	/* 13. Unit Test #2.11 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state for event: NORMAL_ASM for link-0 with RX-Link Status SELECTED; the Tx module gets correctly updated to activate Link-0 for bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 0;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 0; // CO proposes to CPE that link-1 is unusable for reception
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 0; // CO notifies CPE that link-1 is unusable for transmission
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 14. Unit Test #2.12 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state for event: NORMAL_ASM for link-1 with RX-Link Status SELECTED; the Tx module gets correctly updated to activate Link-1 for bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x1; // SID format 12-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 0; // CO proposes to CPE that link-0 is unusable for reception
		cell.rx_link1_status = 3; // CO proposes to CPE start xmission in link-0
		cell.tx_link0_status = 0; // CO notifies CPE that link-0 is unusable for transmission
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );

		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 15. Unit Test #2.13 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state for event: NORMAL_ASM for link-0 with RX-Link Status SELECTED for both links; the Tx module gets correctly updated to activate both links for bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 0;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 16. Unit Test #2.14 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state with Tx-Link-Status of both links already in TX-ACTIVE state, for event: NORMAL_ASM for link-1 with RX-Link Status SELECTED for both links; the Tx module correctly does not get updated to activate both links for bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 1;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

		ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false;
        ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; 

		cell.tx_lnk_no = 1;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 17. Unit Test #2.15 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state with Tx-Link-Status of both links already in TX-ACTIVE state, for event: SHOWTIME_EXIT for link-0; the Tx module correctly gets updated to deactivate link-0 from bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 0;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 1;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

		ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false;
        ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; 

		printf( "Firing SHOWTIME_EXIT for link-0\n" );
		asm_fsm( SHOWTIME_EXIT, 0, NULL );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was deactivated", true, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 18. Unit Test #2.16 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state with Tx-Link-Status of both links already in TX-ACTIVE state, for event: NORMAL_ASM for link-1 with RX-Link Status SHOULDNT-BE-USED for link-1; the Tx module correctly gets updated to deactivate link-1 from bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		memset((void *)&cell, 0, sizeof(cell));
		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 1;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

		ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false;
        ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; 

		cell.tx_lnk_no = 1;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 1; // CO proposes to CPE not to use link-1 for transmitting bonded traffic
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was deactivated", true, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 19. Unit Test #2.17 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to STEADY state with Tx-Link-Status of both links already in TX-ACTIVE state, for event: NORMAL_ASM for link-0 with RX-Link Status SELECTED for both links; the Tx module correctly gets updated to deactivate both links from bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 0;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

		ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false;
        ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; 

		cell.tx_lnk_no = 0;
		cell.rx_link0_status = 2; // CO proposes to CPE not to pause transmitting bonded traffic from link-0
		cell.rx_link1_status = 2; // CO proposes to CPE not to pause transmitting bonded traffic from link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was deactivated", true, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was deactivated", true, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 20. Unit Test #2.18 */
	BEGIN_TEST( "FSM: Verify during transition from STEADY to LEARNING state with Tx-Link-Status of both links already in TX-ACTIVE state, for event: RESET_ASM for link-0; the Tx module correctly gets updated to deactivate both links from bonded traffic" ) {

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		cell.msg_type = 0x0; // SID format 8-bit
		cell.tx_lnk_no = 1;
		cell.num_links = 2;
		cell.rx_link0_status = 2; // CO proposes to CPE prepare to xmt in link-0
		cell.rx_link1_status = 2; // CO proposes to CPE prepare to xmt in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-1\n" );
		asm_fsm( NORMAL_ASM, 1, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-SELECTED", TX_SELECTED, PRIV->fsm.txlinkstatus[1] );

		cell.tx_lnk_no = 0;
		cell.rx_link0_status = 3; // CO proposes to CPE to start xmt-ing in link-0
		cell.rx_link1_status = 3; // CO proposes to CPE to start xmt-ing in link-1
		cell.tx_link0_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-0
		cell.tx_link1_status = 2; // CO notifies CPE to consider accepting bonding traffic in link-1
		printf( "Firing NORMAL_ASM for link-0\n" );
		asm_fsm( NORMAL_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is STEADY", STEADY, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACTIVE", TX_ACTIVE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was activated", true, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was activated", true, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was not deactivated", false, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was not deactivated", false, ut_asm_context.tx_link1_deactivated);

		ut_asm_context.tx_link0_activated = ut_asm_context.tx_link1_activated = false;
        ut_asm_context.tx_link0_deactivated = ut_asm_context.tx_link1_deactivated = false; 

		cell.msg_type = 0xFF; // RESET message type
		cell.tx_lnk_no = 0;
		printf( "Firing RESET_ASM for link-0\n" );
		asm_fsm( RESET_ASM, 0, (void *)&cell );
		ASSERT_NUM("ASM-FSM State is LEARNING", LEARNING, PRIV->fsm.state);
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );
		ASSERT_BOOL("Tx Link-0 was not activated", false, ut_asm_context.tx_link0_activated);
		ASSERT_BOOL("Tx Link-1 was not activated", false, ut_asm_context.tx_link1_activated);
		ASSERT_BOOL("Tx Link-0 was deactivated", true, ut_asm_context.tx_link0_deactivated);
		ASSERT_BOOL("Tx Link-1 was deactivated", true, ut_asm_context.tx_link1_deactivated);

	} END_TEST();

	/* 21. Unit Test #3.1.1 */
	BEGIN_TEST( "Invalid ASM: Set consumed flag to true, write valid cell and verify that process_asm generates no event" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to true
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x00; // 8-bit
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.lastasmcellconsumed = true;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_BOOL("Previous received flag remains false", false, PRIV->eventqueue.prev_asm_received );

	} END_TEST();

	/* 22. Unit Test #3.1.2 */
	BEGIN_TEST( "Invalid ASM: Verify for ASM cell received with incorrect CRC, it gets discarded" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x00000000
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to false
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x00; // 8-bit
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = ~((crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0)); // one-'s complement of crc

		PRIV->eventqueue.lastasmcellconsumed = false;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_BOOL("Previous received flag remains false", false, PRIV->eventqueue.prev_asm_received );

	} END_TEST();

	/* 23. Unit Test #3.1.3 */
	BEGIN_TEST( "Invalid ASM: Verify for ASM cell received with incorrect length field, it gets discarded" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x27, CRC: 0x12345678
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to false
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x00; // 8-bit
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x27);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.lastasmcellconsumed = false;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_BOOL("Previous received flag remains false", false, PRIV->eventqueue.prev_asm_received );

	} END_TEST();

	/* 24. Unit Test #3.1.4 */
	BEGIN_TEST( "Invalid ASM: Verify for ASM cell received with incorrect msg type, it gets discarded" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Update ASMCell field of eventqueue section:
		 *    - msg_type: 0xEE, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to false
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0xEE; // invalid
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.lastasmcellconsumed = false;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_BOOL("Previous received flag remains false", false, PRIV->eventqueue.prev_asm_received );

	} END_TEST();

	/* 25. Unit Test #3.1.5 */
	BEGIN_TEST( "Invalid ASM: Verify for ASM cell received with out of range ASM-Id, it gets discarded. Last ASM ID was 4, this one's ASM ID is 3." ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Set PrevASMReceived field of eventqueue section to true
		 * 4. Set last_rx_asm_id field of EventQueue to 4
		 * 5. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-12-bit, asm_id: 3, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to false
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x01; // 12-bit
		cell->asm_id = 3;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.prev_asm_received = true;
		PRIV->eventqueue.lastasmcellconsumed = false;
		PRIV->eventqueue.last_rx_asm_id = 4;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));

	} END_TEST();

	/* 26. Unit Test #3.1.6 */
	BEGIN_TEST( "Invalid ASM: Verify for ASM cell received with out of range ASM-Id, it gets discarded. Last ASM ID was 100, this one's ASM ID is 100." ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set enabled field of ASMTransmit section for both links to false
		 * 3. Set PrevASMReceived field of eventqueue section to true
		 * 4. Set last_rx_asm_id field of EventQueue to 100
		 * 5. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-12-bit, asm_id: 100, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 4. Inject RECEIVE, linkid:0 event into eventqueue
		 * 5. Set Consumed flag to false
		 * 6. call process_asm
		 * 7. Verify from stats (asmdiscarded count) that this cell was discarded
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x01; // 12-bit
		cell->asm_id = 100;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.prev_asm_received = true;
		PRIV->eventqueue.lastasmcellconsumed = false;
		PRIV->eventqueue.last_rx_asm_id = 100;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was discarded", 1, (unsigned int)(PRIV->stats.asmdiscarded[0]));

	} END_TEST();

	/* 27. Unit Test #3.2.1 */
	BEGIN_TEST( "NORMAL_ASM: Verify for ASM cell received for the first time, it generates NORMAL_ASM event" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Fire SHOWTIME_ENTRY on both links
		 * 3. Set enabled field of ASMTransmit section for both links to false
		 * 4. Set PrevASMReceived field of eventqueue section to false
		 * 5. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 6. Inject RECEIVE, linkid:0 event into eventqueue
		 * 7. Set Consumed flag to false
		 * 8. call process_asm
		 * 9. Verify Tx and Rx modules get correctly updated with SID-format as 8-bits
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x00; // 8-bit
		cell->asm_id = 5;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.prev_asm_received = false;
		PRIV->eventqueue.lastasmcellconsumed = false;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was not discarded", 0, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_NUM("FSM has reached STEADY state", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 8-bit SID format", true, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 8-bit SID format", true, ut_asm_context.rx_sid_is_8bit);
		ASSERT_BOOL("Previous received flag becomes true", true, PRIV->eventqueue.prev_asm_received );
		ASSERT_NUM("Previous received ASM ID", 5, PRIV->eventqueue.last_rx_asm_id );

	} END_TEST();

	/* 28. Unit Test #3.2.2 */
	BEGIN_TEST( "NORMAL_ASM: Verify for Valid ASM cell received not for the first time, it generates NORMAL_ASM event if (a) Bonding group config matches, and (b) ASM Id is within range" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Fire SHOWTIME_ENTRY on both links
		 * 3. Set enabled field of ASMTransmit section for both links to false
		 * 4. Set PrevASMReceived field of eventqueue section to true
		 * 5. Set last_rx_asm_id field of EventQueue to 10
		 * 6. Set BondingGroupConfig section to {msg-type: is-8-bit, group-id: 1000, num_links: 2}
		 * 7. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, asm_id: 11, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 8. Inject RECEIVE, linkid:0 event into eventqueue
		 * 9. Set Consumed flag to false
		 *10. call process_asm
		 *11. Verify Tx and Rx modules get correctly updated with SID-format as 8-bits
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		printf( "Firing SHOWTIME_ENTRY for link-0\n" );
		asm_fsm( SHOWTIME_ENTRY, 0, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );
		printf( "Firing SHOWTIME_ENTRY for link-1\n" );
		asm_fsm( SHOWTIME_ENTRY, 1, NULL );
		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );
		ASSERT_NUM("Tx Link Status of Link-1 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1] );

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0x00; // 8-bit
		cell->asm_id = 11;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.last_rx_asm_id = 10;
		PRIV->eventqueue.prev_asm_received = true;
		PRIV->eventqueue.lastasmcellconsumed = false;

		PRIV->bonding_grp_cfg.grpid = 1000;
		PRIV->bonding_grp_cfg.msg_type = 0x00;
		PRIV->bonding_grp_cfg.num_links = 2;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was not discarded", 0, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_NUM("FSM has reached STEADY state", STEADY, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was called", true, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Tx Config Update with 8-bit SID format", true, ut_asm_context.tx_sid_is_8bit);
		ASSERT_BOOL("Rx Config Update was called", true, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Rx Config Update with 8-bit SID format", true, ut_asm_context.rx_sid_is_8bit);
		ASSERT_BOOL("Previous received flag becomes true", true, PRIV->eventqueue.prev_asm_received );
		ASSERT_NUM("Previous received ASM ID", 11, PRIV->eventqueue.last_rx_asm_id );

	} END_TEST();

	/* 29. Unit Test #3.3.1 */
	BEGIN_TEST( "RESET_ASM: Verify for Valid ASM cell received for the first time, it generates RESET_ASM event" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set TxLinkStatus fields of both links in FSM section to TX-ACCEPTABLE,
		 *    RXLinkStatus fields of both links in FSM section to RX-ACCEPTABLE
		 * 3. Set enabled field of ASMTransmit section for both links to false
		 * 4. Set PrevASMReceived field of eventqueue section to false
		 * 5. Update ASMCell field of eventqueue section:
		 *    - msg_type: FF, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 6. Inject RECEIVE, linkid:0 event into eventqueue
		 * 7. Set Consumed flag to false
		 * 8. call process_asm
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->fsm.txlinkstatus[0] = PRIV->fsm.txlinkstatus[1] = TX_ACCEPTABLE;
		PRIV->fsm.rxlinkstatus[0] = PRIV->fsm.rxlinkstatus[1] = RX_ACCEPTABLE;

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;
		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;
		
		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;

		cell->msg_type = 0xFF;
		cell->asm_id = 5;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.prev_asm_received = false;
		PRIV->eventqueue.lastasmcellconsumed = false;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was not discarded", 0, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_NUM("FSM is in LEARNING state", LEARNING, PRIV->fsm.state);
		ASSERT_BOOL("Tx Config Update was not called", false, ut_asm_context.tx_module_updated);
		ASSERT_BOOL("Rx Config Update was not called", false, ut_asm_context.rx_module_updated);
		ASSERT_BOOL("Previous received flag becomes true", true, PRIV->eventqueue.prev_asm_received );
		ASSERT_NUM("Previous received ASM ID", 5, PRIV->eventqueue.last_rx_asm_id );

	} END_TEST();

	/* 30. Unit Test #3.3.2 */
	BEGIN_TEST( "RESET_ASM: Verify for Valid ASM cell received not for the first time, it generates RESET_ASM event if Bonding group id mismatch" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set state field of FSM section to STEADY, TxLinkStatus fields of both links in FSM section to TX-ACCEPTABLE,
		 *    RXLinkStatus fields of both links in FSM section to RX-ACCEPTABLE
		 * 3. Set enabled field of ASMTransmit section for both links to false
		 * 4. Set PrevASMReceived field of eventqueue section to true
		 * 5. Set last_rx_asm_id field of EventQueue to 10
		 * 6. Set BondingGroupConfig section to {msg-type: is-8-bit, group-id: 1001, num_links: 2}
		 * 7. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, asm_id: 11, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 8. Inject RECEIVE, linkid:0 event into eventqueue
		 * 9. Set Consumed flag to false
		 * 10. call process_asm
		 * 11. Verify FSM is still in LEARNING mode
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->fsm.state = STEADY;
		PRIV->fsm.txlinkstatus[0] = PRIV->fsm.txlinkstatus[1] = TX_ACCEPTABLE;
		PRIV->fsm.rxlinkstatus[0] = PRIV->fsm.rxlinkstatus[1] = RX_ACCEPTABLE;

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;

		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;

		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;
		cell->msg_type = 0x00; // 8-bit
		cell->asm_id = 11;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.last_rx_asm_id = 10;
		PRIV->eventqueue.prev_asm_received = true;
		PRIV->eventqueue.lastasmcellconsumed = false;

		PRIV->bonding_grp_cfg.grpid = 1001;
		PRIV->bonding_grp_cfg.msg_type = 0x00;
		PRIV->bonding_grp_cfg.num_links = 2;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was not discarded", 0, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_NUM("FSM is in LEARNING state", LEARNING, PRIV->fsm.state);
		ASSERT_BOOL("Previous received flag becomes true", true, PRIV->eventqueue.prev_asm_received );
		ASSERT_NUM("Previous received ASM ID", 11, PRIV->eventqueue.last_rx_asm_id );

	} END_TEST();

	/* 31. Unit Test #3.3.3 */
	BEGIN_TEST( "RESET_ASM: Verify for Valid ASM cell received not for the first time, it generates RESET_ASM event if msg type mismatch" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set state field of FSM section to STEADY, TxLinkStatus fields of both links in FSM section to TX-ACCEPTABLE,
		 *    RXLinkStatus fields of both links in FSM section to RX-ACCEPTABLE
		 * 3. Set enabled field of ASMTransmit section for both links to false
		 * 4. Set PrevASMReceived field of eventqueue section to true
		 * 5. Set last_rx_asm_id field of EventQueue to 10
		 * 6. Set BondingGroupConfig section to {msg-type: is-12-bit, group-id: 1000, num_links: 2}
		 * 7. Update ASMCell field of eventqueue section:
		 *    - msg_type: is-8-bit, asm_id: 11, tx_lnk_no: 1, num_links: 2, rx_link0_status: SHOULDNT_BE_USED, rx_link0_status: SHOULDNT_BE_USED,
		 *    - tx_link0_status: ACCEPTED, tx_link1_status: ACCEPTED, grpid: 1000, len: 0x28, CRC: 0x12345678
		 * 8. Inject RECEIVE, linkid:0 event into eventqueue
		 * 9. Set Consumed flag to false
		 * 10. call process_asm
		 * 11. Verify FSM is still in LEARNING mode
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->fsm.state = STEADY;
		PRIV->fsm.txlinkstatus[0] = PRIV->fsm.txlinkstatus[1] = TX_ACCEPTABLE;
		PRIV->fsm.rxlinkstatus[0] = PRIV->fsm.rxlinkstatus[1] = RX_ACCEPTABLE;

		PRIV->asm_transmit.timers[0].enabled = false;
		PRIV->asm_transmit.timers[1].enabled = false;

		memset( PRIV->eventqueue.lastasmcellbuff, 0, sizeof(PRIV->eventqueue.lastasmcellbuff) );
		cell = (asm_cell_t *)(PRIV->eventqueue.lastasmcellbuff);
		PRIV->eventqueue.lastasmlinkid = 0;

		SETSID(&cell->hdr, 0, false);
		SETVPI(&cell->hdr, 0);
		SETVCI(&cell->hdr, 20);
		cell->hdr.pt1 = 1;
		cell->msg_type = 0x00; // 8-bit
		cell->asm_id = 11;
		cell->tx_lnk_no = 1; cell->num_links = 2; cell->rx_link0_status = cell->rx_link1_status = 1;
		cell->tx_link0_status = cell->tx_link1_status = 2;
		cell->grpid = htons(1000);
		cell->len = htons(0x28);
		cell->crc = crc32_be(~0, ((u8 *)cell)+sizeof(ima_rx_atm_header_t), 44) ^ ~0;

		PRIV->eventqueue.last_rx_asm_id = 10;
		PRIV->eventqueue.prev_asm_received = true;
		PRIV->eventqueue.lastasmcellconsumed = false;

		PRIV->bonding_grp_cfg.grpid = 1000;
		PRIV->bonding_grp_cfg.msg_type = 0x01;
		PRIV->bonding_grp_cfg.num_links = 2;

		// Inject RECEIVE event
		PRIV->eventqueue.elements[0].event = RECEIVE;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Cell was not discarded", 0, (unsigned int)(PRIV->stats.asmdiscarded[0]));
		ASSERT_NUM("FSM is in LEARNING state", LEARNING, PRIV->fsm.state);
		ASSERT_BOOL("Previous received flag becomes true", true, PRIV->eventqueue.prev_asm_received );
		ASSERT_NUM("Previous received ASM ID", 11, PRIV->eventqueue.last_rx_asm_id );

	} END_TEST();

	/* 32. Unit Test #3.4.1 */
	BEGIN_TEST( "SHOWTIME_ENTRY: Verify process_asm() relays SHOWTIME_ENTRY event" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Inject SHOWTIME_ENTRY, linkid:0 event into eventqueue
		 * 3. call process_asm
		 * 4. Verify Tx Link Status of link-0 is TX-ACCEPTABLE
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		ASSERT_NUM("Tx Link Status of Link-0 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0] );

		// Inject SHOWTIME_ENTRY event
		PRIV->eventqueue.elements[0].event = SHOWTIME_ENTRY;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Tx Link Status of Link-0 is TX-ACCEPTABLE", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[0] );

	} END_TEST();

	/* 33. Unit Test #3.4.2 */
	BEGIN_TEST( "SHOWTIME_EXIT: Verify process_asm() relays SHOWTIME_EXIT event" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Set TxLinkStatus of Link-1 to TX-ACCEPTABLE
		 * 3. Inject SHOWTIME_EXIT, linkid:1 event into eventqueue
		 * 4. call process_asm
		 * 5. Verify Tx Link Status of link-1 is TX-NON-PROVISIONED
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->fsm.txlinkstatus[1] = TX_ACCEPTABLE;

		// Inject SHOWTIME_ENTRY event
		PRIV->eventqueue.elements[0].event = SHOWTIME_EXIT;
	    PRIV->eventqueue.elements[0].linkid = 1;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_NUM("Tx Link Status of Link-1 is TX-NON-PROVISIONED", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[1] );

	} END_TEST();

	/* 34. Unit Test #3.4.3 */
	BEGIN_TEST( "TRANSMIT: Verify process_asm() relays TRANSMIT event: by checking tx-timer for the link gets enabled" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Disable tx timer of both links
		 * 3. Inject TRANSMIT, linkid:0 event into eventqueue
		 * 4. call process_asm
		 * 5. Verify tx timer of link-0 gets enabled, but tx-timer of link-1 remains disabled
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled
			= PRIV->asm_transmit.timers[1].enabled = false;

		// Inject TRANSMIT event
		PRIV->eventqueue.elements[0].event = TRANSMIT;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_BOOL("Tx timer of Link-0 gets enabled",  true, PRIV->asm_transmit.timers[0].enabled );
		ASSERT_BOOL("Tx timer of Link-1 remains disabled", false, PRIV->asm_transmit.timers[1].enabled );

	} END_TEST();

	/* 35. Unit Test #3.4.5 */
	BEGIN_TEST( "STOP: Verify process_asm() relays STOP event: By checking tx-timer for both links getting disabled" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Enable tx timer of both links
		 * 3. Inject STOP, linkid:0 event into eventqueue
		 * 4. call process_asm
		 * 5. Verify tx timer of both links get disabled
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->asm_transmit.timers[0].enabled
			= PRIV->asm_transmit.timers[1].enabled = true;

		// Inject STOP event
		PRIV->eventqueue.elements[0].event = STOP;
	    PRIV->eventqueue.elements[0].linkid = 0;
	    PRIV->eventqueue.tail = 1;
	    PRIV->eventqueue.count = 1;

		process_asm();

		ASSERT_BOOL("Tx timer of Link-0 gets disabled", false, PRIV->asm_transmit.timers[0].enabled );
		ASSERT_BOOL("Tx timer of Link-1 gets disabled", false, PRIV->asm_transmit.timers[1].enabled );

	} END_TEST();

	/* 36. Unit Test #4.1 */
	BEGIN_TEST( "TRANSMIT: Verify asm_fsm_create_and_transmit() correctly creates ASM cell" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. For bonding-group-config section of private data:
		 *    - set msg-type field to 0x1
		 *    - set num-links to 2
		 *    - set link-map to {link-0: 1, link-1: 0}
		 *    - set grpid to 342
		 * 3. Set next_ts_asm_id field of ASMTransmit section of private data to 57
		 * 4. For FSM section of private data:
		 *    - set txlink-status of link-0 to TX-ACCEPTABLE
		 *    - set txlink-status of link-1 to TX-ACTIVE
		 *    - set rxlink-status of link-0 to RX-ACCEPTABLE
		 *    - set rxlink-status of link-1 to RX-SELECTED
		 * 5. call asm_fsm_create_and_transmit from link-0
		 * 6. Verify the cell gets created properly 
		 * 7. Verify that xmt_cell gets called with correct linkid
		 */

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->bonding_grp_cfg.msg_type = 0x1;
		PRIV->bonding_grp_cfg.num_links = 2;
		PRIV->bonding_grp_cfg.link_map[0] = 0;
		PRIV->bonding_grp_cfg.link_map[1] = 1;
		PRIV->bonding_grp_cfg.link_rmap[0] = 0;
		PRIV->bonding_grp_cfg.link_rmap[1] = 1;
		PRIV->bonding_grp_cfg.grpid = 342;

		PRIV->asm_transmit.next_tx_asm_id = 57;

		PRIV->fsm.txlinkstatus[0] = TX_ACCEPTABLE;
		PRIV->fsm.txlinkstatus[1] = TX_ACTIVE;
		PRIV->fsm.rxlinkstatus[0] = RX_SHOULDNT_BE_USED;
		PRIV->fsm.rxlinkstatus[1] = RX_SELECTED;

		asm_fsm_create_and_transmit(0);

		ASSERT_NUM("ASM Cell created for link-0 with VPI: 0", 0, GETVPI((ima_rx_atm_header_t *)ut_asm_context.cell));
		ASSERT_NUM("ASM Cell created for link-0 with VCI: 20", 20, GETVCI((ima_rx_atm_header_t *)ut_asm_context.cell));
		ASSERT_NUM("ASM Cell created for link-0 with PT1: 1", 1, ((ima_rx_atm_header_t *)(ut_asm_context.cell))->pt1);;
		ASSERT_NUM("ASM Cell created for link-0 with msg-type: SID format 12-bit", 1, ((asm_cell_t *)ut_asm_context.cell)->msg_type);
		ASSERT_NUM("ASM Cell created for link-0 with asm-id: 57", 57, ((asm_cell_t *)ut_asm_context.cell)->asm_id);
		ASSERT_NUM("next_ts_asm_id field of ASMTransmit section of private data gets incremented to 58", 58, PRIV->asm_transmit.next_tx_asm_id);
		ASSERT_NUM("ASM Cell created for link-0 with tx-link-no: 0", 0, ((asm_cell_t *)ut_asm_context.cell)->tx_lnk_no);
		ASSERT_NUM("ASM Cell created for link-0 with num-links: 2", 2, ((asm_cell_t *)ut_asm_context.cell)->num_links);
		ASSERT_NUM("ASM Cell created for link-0 with TX Link Status for link-0: ACCEPTABLE", 2, ((asm_cell_t *)ut_asm_context.cell)->tx_link0_status);
		ASSERT_NUM("ASM Cell created for link-0 with TX Link Status for link-1: SELECTED", 3, ((asm_cell_t *)ut_asm_context.cell)->tx_link1_status);
		ASSERT_NUM("ASM Cell created for link-0 with RX Link Status for link-0: SHOULDNT_BE_USED", 1, ((asm_cell_t *)ut_asm_context.cell)->rx_link0_status);
		ASSERT_NUM("ASM Cell created for link-0 with RX Link Status for link-1: SELECTED", 3, ((asm_cell_t *)ut_asm_context.cell)->rx_link1_status);
		ASSERT_NUM("ASM Cell created for link-0 with group-id: 342", 342, ntohs(((asm_cell_t *)ut_asm_context.cell)->grpid));
		ASSERT_BOOL("xmt_cell() was called for link-0", true, ut_asm_context.tx_link0_xmt);
		ASSERT_BOOL("xmt_cell() was not called for link-1", false, ut_asm_context.tx_link1_xmt);

	} END_TEST();

	/* 37. Unit Test #4.2 */
	BEGIN_TEST( "TRANSMIT: Verify asm_fsm_create_and_transmit() correctly creates ASM cell with ASM ID rollover" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. For bonding-group-config section of private data:
		 *    - set msg-type field to 0x1
		 *    - set num-links to 2
		 *    - set link-map to {link-0: 1, link-1: 0}
		 *    - set grpid to 342
		 * 3. Set next_ts_asm_id field of ASMTransmit section of private data to 0xFF
		 * 4. For FSM section of private data:
		 *    - set txlink-status of link-0 to TX-ACCEPTABLE
		 *    - set txlink-status of link-1 to TX-ACTIVE
		 *    - set rxlink-status of link-0 to RX-ACCEPTABLE
		 *    - set rxlink-status of link-1 to RX-SELECTED
		 * 5. call asm_fsm_create_and_transmit from link-0
		 * 6. Verify the cell gets created properly with next_ts_asm_id field of ASMTransmit section of private data set to 0
		 * 7. Verify that xmt_cell gets called with correct linkid
		 */

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->bonding_grp_cfg.msg_type = 0x1;
		PRIV->bonding_grp_cfg.num_links = 2;
		PRIV->bonding_grp_cfg.link_map[0] = 1;
		PRIV->bonding_grp_cfg.link_map[1] = 0;
		PRIV->bonding_grp_cfg.link_rmap[0] = 1;
		PRIV->bonding_grp_cfg.link_rmap[1] = 0;
		PRIV->bonding_grp_cfg.grpid = 1015;

		PRIV->asm_transmit.next_tx_asm_id = 0xFF;

		PRIV->fsm.txlinkstatus[0] = TX_ACCEPTABLE;
		PRIV->fsm.txlinkstatus[1] = TX_ACTIVE;
		PRIV->fsm.rxlinkstatus[0] = RX_SHOULDNT_BE_USED;
		PRIV->fsm.rxlinkstatus[1] = RX_SELECTED;

		asm_fsm_create_and_transmit(1);

		ASSERT_NUM("ASM Cell created for link-0 with VPI: 0", 0, GETVPI((ima_rx_atm_header_t *)ut_asm_context.cell));
		ASSERT_NUM("ASM Cell created for link-0 with VCI: 20", 20, GETVCI((ima_rx_atm_header_t *)ut_asm_context.cell));
		ASSERT_NUM("ASM Cell created for link-0 with PT1: 1", 1, ((ima_rx_atm_header_t *)(ut_asm_context.cell))->pt1);;
		ASSERT_NUM("ASM Cell created for link-0 with msg-type: SID format 12-bit", 1, ((asm_cell_t *)ut_asm_context.cell)->msg_type);
		ASSERT_NUM("ASM Cell created for link-0 with asm-id: 255", 255, ((asm_cell_t *)ut_asm_context.cell)->asm_id);
		ASSERT_NUM("next_ts_asm_id field of ASMTransmit section of private data gets incremented to 0", 0, PRIV->asm_transmit.next_tx_asm_id);
		ASSERT_NUM("ASM Cell created for link-0 with tx-link-no: 0", 0, ((asm_cell_t *)ut_asm_context.cell)->tx_lnk_no);
		ASSERT_NUM("ASM Cell created for link-0 with num-links: 2", 2, ((asm_cell_t *)ut_asm_context.cell)->num_links);
		ASSERT_NUM("ASM Cell created for link-0 with TX Link Status for link-0: SELECTED", 3, ((asm_cell_t *)ut_asm_context.cell)->tx_link0_status);
		ASSERT_NUM("ASM Cell created for link-0 with TX Link Status for link-1: ACCEPTABLE", 2, ((asm_cell_t *)ut_asm_context.cell)->tx_link1_status);
		ASSERT_NUM("ASM Cell created for link-0 with RX Link Status for link-0: SELECTED", 3, ((asm_cell_t *)ut_asm_context.cell)->rx_link0_status);
		ASSERT_NUM("ASM Cell created for link-0 with RX Link Status for link-1: SHOULDNT-BE-USED", 1, ((asm_cell_t *)ut_asm_context.cell)->rx_link1_status);
		ASSERT_NUM("ASM Cell created for link-0 with group-id: 1015", 1015, ntohs(((asm_cell_t *)ut_asm_context.cell)->grpid));
		ASSERT_BOOL("xmt_cell() was not called for link-0", false, ut_asm_context.tx_link0_xmt);
		ASSERT_BOOL("xmt_cell() was called for link-1", true, ut_asm_context.tx_link1_xmt);

	} END_TEST();

	/* 38. Unit Test #5.a */
	BEGIN_TEST( "EVENT-QUEUE: Verify asm_cell_received() injects into the queue" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. call asm_cell_received()
		 * 3. Verify EventQueue of private data contains a single event, of type RECEIVE
		 */

		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t *cell;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->eventqueue.lastasmcellconsumed = true;

		asm_cell_received( sample_asm, 1);

		ASSERT_NUM("Used count of eventqueue is 1", 1, PRIV->eventqueue.count);
		ASSERT_NUM("Head of eventqueue contains event: RECEIVE", RECEIVE, PRIV->eventqueue.elements[PRIV->eventqueue.head].event);
		ASSERT_BOOL("ASM Cell consumed flag is reset", false, PRIV->eventqueue.lastasmcellconsumed );
		ASSERT_MEMCMP("ASM cell copied into ASMCell field of eventqueue section of priv data", PRIV->eventqueue.lastasmcellbuff, sample_asm, sizeof(sample_asm));

	} END_TEST();

	/* 39. Unit Test #5.b */
	BEGIN_TEST( "EVENT-QUEUE: Verify asm_showtime_enter() injects into the queue. Also check rollover of the event-queue" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Adjust the head and tail to point to the end of eventqueue
		 * 3. call asm_showtime_enter()
		 * 4. Verify EventQueue of private data contains a single event, of type SHOWTIME_ENTRY
		 * 5. Verify tail of EventQueue of private data now points to the first element of the eventqueue
		 */

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->eventqueue.head 
			= PRIV->eventqueue.tail = PRIV->eventqueue.size - 1;

		asm_showtime_enter(0);

		ASSERT_NUM("Used count of eventqueue is 1", 1, PRIV->eventqueue.count);
		ASSERT_NUM("Head of eventqueue contains event: SHOWTIME_ENTRY", SHOWTIME_ENTRY, PRIV->eventqueue.elements[PRIV->eventqueue.head].event);
		ASSERT_NUM("Tail of eventqueue has moved to beginning of eventqueue", 0, PRIV->eventqueue.tail );

	} END_TEST();

	/* 40. Unit Test #5.c */
	BEGIN_TEST( "EVENT-QUEUE: Verify asm_showtime_exit() injects into the queue" ) {

		/*
		 * 1. Initialize the ASM module
		 * 2. Adjust the head and tail to point to middle of eventqueue
		 * 3. call asm_showtime_exit()
		 * 4. Verify EventQueue of private data contains a single event, of type SHOWTIME_EXIT
		 * 5. Verify tail of EventQueue of private data now points to element next to head
		 */

		int rval;
		struct bond_drv_cfg_params tmp;

		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		PRIV->eventqueue.head 
			= PRIV->eventqueue.tail = PRIV->eventqueue.size / 2;

		asm_showtime_exit(1);

		ASSERT_NUM("Used count of eventqueue is 1", 1, PRIV->eventqueue.count);
		ASSERT_NUM("Head of eventqueue contains event: SHOWTIME_EXIT", SHOWTIME_EXIT, PRIV->eventqueue.elements[PRIV->eventqueue.head].event);
		ASSERT_NUM("Tail of eventqueue has moved to element next to head of eventqueue", PRIV->eventqueue.head + 1, PRIV->eventqueue.tail );

	} END_TEST();

	/* 41. Unit Test #6 */
	BEGIN_TEST( "CO, CPE link id map verification" ) {

		int id = 1;
		int rval;
		struct bond_drv_cfg_params tmp;
		asm_cell_t* tmp_cell = NULL;
		int i = 0;

		u8 rxcell[56] = {
							0x00, 0x00, 0x01, 0x42, 0x89, 0x00, 0x00, 0x00, /* ATM Header */
							0x00 /*msg-type*/, 0x01 /*msg-id*/, 0x00 /*tx_lnk_no*/, 0x02 /*num-links*/,
							0x40 /*rx-link-status[0][1][2][3]*/, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x80 /*tx-link-status[0][1][2][3]*/, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x0e, 0x0d /*grp-id*/, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x9b, 0x9a, 0xcc, 0x8e /*timestamp*/, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							//0x00, 0x28, 0xd2, 0x42, 0x9b, 0x95 /*AAL5 trailer*/
							0x00, 0x28, 0xFF, 0xFF, 0xFF, 0xFF /*AAL5 trailer*/
						};

		/*
		 * 1. Initialize the ASM module
		 * 2. call asm_showtime_enter() for link-1
		 * 3. call asm_cell_received() on link-1, where the CO represents idx of link-1 as zero
		 * 4. call asm_fsm_create_and_transmit() on link-1
		 * 5. Verify xmt_cell is called, and verify the generated rx-link-status of co-linkidx-0 is ACCEPTED,
		 *    co-linkidx-1 is NON-PROVISIONED, tx-link-status of co-linkidx-0 is SELECTED, co-linkidx-1 is NON-PROVISIONED
		 */
		rval = init_device_asm_metadata(&tmp);
		ASSERT_NUM("init_device_asm_metadata returns 0", 0, rval);

		asm_showtime_enter(1);
		process_asm();
		ASSERT_NUM("Regular FSM is in LEARNING state", PRIV->fsm.state, LEARNING);
		ASSERT_NUM("Tx-Link-Status of Link-0 is in NON-PROVISIONED state", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0]);
		ASSERT_NUM("Tx-Link-Status of Link-1 is in ACCEPTABLE state", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1]);
		ASSERT_NUM("Rx-Link-Status of Link-0 is in NON-PROVISIONED state", TX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[0]);
		ASSERT_NUM("Rx-Link-Status of Link-1 is in SHOULDNT-BE-USED state", RX_SHOULDNT_BE_USED, PRIV->fsm.rxlinkstatus[1]);

		asm_cell_received( rxcell, 1);
		process_asm();
		ASSERT_NUM("Regular FSM is in STEADY state", PRIV->fsm.state, STEADY);
		ASSERT_NUM("Tx-Link-Status of Link-0 is in NON-PROVISIONED state", TX_NON_PROVISIONED, PRIV->fsm.txlinkstatus[0]);
		ASSERT_NUM("Tx-Link-Status of Link-1 is in ACCEPTABLE state", TX_ACCEPTABLE, PRIV->fsm.txlinkstatus[1]);
		ASSERT_NUM("Rx-Link-Status of Link-0 is in NON-PROVISIONED state", TX_NON_PROVISIONED, PRIV->fsm.rxlinkstatus[0]);
		ASSERT_NUM("Rx-Link-Status of Link-1 is in RX-ACCEPTABLE state", RX_ACCEPTABLE, PRIV->fsm.rxlinkstatus[1]);

		memset(ut_asm_context.cell, 0, sizeof(ut_asm_context.cell));
		PRIV->asm_transmit.next_tx_asm_id = 30;
		asm_fsm_create_and_transmit(1);
		ASSERT_BOOL("xmt_cell() was not called for link-0", false, ut_asm_context.tx_link0_xmt);
		ASSERT_BOOL("xmt_cell() was called for link-1", true, ut_asm_context.tx_link1_xmt);
		ASSERT_NUM("ASM Cell created for link-1 with asm-id: 30", 30, ((asm_cell_t *)ut_asm_context.cell)->asm_id);
		ASSERT_NUM("next_ts_asm_id field of ASMTransmit section of private data gets incremented to 31", 31, PRIV->asm_transmit.next_tx_asm_id);
		ASSERT_NUM("ASM Cell created for link-1 with tx-link-no: 0", 0, ((asm_cell_t *)ut_asm_context.cell)->tx_lnk_no);
		ASSERT_NUM("ASM Cell created for link-1 with num-links: 2", 2, ((asm_cell_t *)ut_asm_context.cell)->num_links);
		ASSERT_NUM("ASM Cell created for link-1 with TX Link Status for link-0: ACCEPTABLE", 2, ((asm_cell_t *)ut_asm_context.cell)->tx_link0_status);
		ASSERT_NUM("ASM Cell created for link-1 with TX Link Status for link-1: NON-PROVISIONED", 0, ((asm_cell_t *)ut_asm_context.cell)->tx_link1_status);
		ASSERT_NUM("ASM Cell created for link-1 with RX Link Status for link-0: ACCEPTABLE", 2, ((asm_cell_t *)ut_asm_context.cell)->rx_link0_status);
		ASSERT_NUM("ASM Cell created for link-1 with RX Link Status for link-1: NON-PROVISIONED", 0, ((asm_cell_t *)ut_asm_context.cell)->rx_link1_status);

	} END_TEST();
}
END_UNIT_TESTS();

