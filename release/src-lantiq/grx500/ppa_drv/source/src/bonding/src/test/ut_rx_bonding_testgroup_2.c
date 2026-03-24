
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 *************************************************************
 *     Unit Testing the IMA Bonding Driver Rx Module code    *
 *************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ut_smaller_sid( int *total, int *passed, int *failed );
void ut_is_sid_out_of_range( int *total, int *passed, int *failed );
void ut_peek_rx_nextsid( int *total, int *passed, int *failed );
void ut_rx_process_queues_READY( int *total, int *passed, int *failed );
void ut_rx_timeout_handler_READY( int *total, int *passed, int *failed );
void ut_rx_process_queues_LEARNING( int *total, int *passed, int *failed );
void ut_rx_timeout_handler_LEARNING( int *total, int *passed, int *failed );

void process_asm( unsigned char *cell, unsigned int lineid );
void process_nonasm( unsigned char *cell );
void report( char *msg, unsigned int sid1, unsigned int sid2);
void init_ut_queues();
void flush_ut_queues();
void populate_ut_queues( int *queue0, int *queue1 );

/////////////////////////////////////////////////////////////////////
#define PRIV (&g_ut_privdata)
#define IS_ASM(cell) (ASM == *((int *)(cell)))
#define GET_SID(cell) *((unsigned int *)(cell))
#define START_SID_TIMER( func )   ((void *)1)
#define DISABLE_SID_TIMER() ;
#define PROCESS_NONASM(cell, lineid) process_nonasm( (cell) );
#define PROCESS_ASM(cell, lineid) process_asm( (cell), (lineid) );
#define REPORT( msg, number1, number2 ) report( (msg), (number1), (number2) )
#define PRINT(format, ...) printf( "[%s[%d]: " format, __func__, __LINE__, ## __VA_ARGS__)

#define LINEQ_SIZE( lineid ) 1024
#define GET_ELEMENT( lineid, idx )  (unsigned char *)&(PRIV->m_st_device_info[(lineid)].queue[(idx)])
#define RELEASE_LINEQ_HEAD( lineid, idx ) PRIV->m_st_device_info[(lineid)].queue[(idx)] = END;
#define LINEQ_ELEMENT_IS_NEW( lineid, idx ) (PRIV->m_st_device_info[(lineid)].queue[(idx)] != END)

#define UNIT_TESTS_2
#include "ut_rx_definitions.c"
#include "../atm/ima_rx_bonding.c"
#include "../common/ima_rx_queue.c"
/////////////////////////////////////////////////////////////////////

int
main( int argc, char **argv )
{
    int grand_total = 0, grand_passed = 0, grand_failed = 0;
    int total, passed, failed;

    ut_rx_process_queues_READY( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_rx_timeout_handler_READY( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_rx_process_queues_LEARNING( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_rx_timeout_handler_LEARNING( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    printf( "====================================================================\n");
    printf( "TOTAL: %d      PASSED: %d               FAILED: %d\n", grand_total, grand_passed, grand_failed );
    printf( "====================================================================\n");

    return 0;
}

#define DUMP_RX_LINES() \
    { \
        int i,j; \
        printf( "\n        ------------------------------------------------------------------------------\n        "); \
        printf( "LINE-0: (HEAD: %d, TAIL: %d, COUNT: %d): ", PRIV->m_st_device_info[0].rx_head, PRIV->m_st_device_info[0].rx_tail, PRIV->m_st_device_info[0].rx_count ); \
                              if( 0 == PRIV->m_st_device_info[0].rx_count ) printf( "<empty>" ); \
                              else { for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { if( ASM == PRIV->m_st_device_info[0].queue[j] ) printf( " ASM " ); else printf( "% 4d ", PRIV->m_st_device_info[0].queue[j] ); } } \
        printf( "\n        ------------------------------------------------------------------------------\n        "); \
        printf( "LINE-1: (HEAD: %d, TAIL: %d, COUNT: %d): ", PRIV->m_st_device_info[1].rx_head, PRIV->m_st_device_info[1].rx_tail, PRIV->m_st_device_info[1].rx_count ); \
                              if( 0 == PRIV->m_st_device_info[1].rx_count ) printf( "<empty>" ); \
                              else { for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(1)) ) { if( ASM == PRIV->m_st_device_info[1].queue[j] ) printf( " ASM " ); else printf( "% 4d ", PRIV->m_st_device_info[1].queue[j] ); } } \
        printf( "\n        ------------------------------------------------------------------------------\n\n"); \
    }

void
process_nonasm( unsigned char *cell )
{
    //printf( "ASM PROCESSED in Line-%d\n", lineid );
    int len = strlen( PRIV->stats.ut_reassembledcellslist );
    snprintf( PRIV->stats.ut_reassembledcellslist + len, sizeof( PRIV->stats.ut_reassembledcellslist ) - len,
              "%s %u", len ? "," : "", GET_SID(cell) );
}

void
process_asm( unsigned char *cell, unsigned int lineid )
{
    PRIV->stats.ut_asmcount += 1;
}

void
report( char *msg, unsigned int sid1, unsigned int sid2)
{
    if( strcmp( msg, "Out of range" ) == 0 )
    {
        int len = strlen( PRIV->stats.ut_droppedcellslist );
        if( sid1 == sid2 )
            snprintf( PRIV->stats.ut_droppedcellslist + len, sizeof( PRIV->stats.ut_droppedcellslist ) - len,
                "%s %u", len ? "," : "", sid1);
        else
            snprintf( PRIV->stats.ut_droppedcellslist + len, sizeof( PRIV->stats.ut_droppedcellslist ) - len,
                "%s %u-%u", len ? "," : "", sid1, sid2 );
    } else if( strcmp( msg, "Missing cells" ) == 0 )
    {
        int len = strlen( PRIV->stats.ut_missingcellslist );
        if( sid1 == sid2 )
            snprintf( PRIV->stats.ut_missingcellslist + len, sizeof( PRIV->stats.ut_missingcellslist ) - len,
                "%s %u", len ? "," : "", sid1);
        else
            snprintf( PRIV->stats.ut_missingcellslist + len, sizeof( PRIV->stats.ut_missingcellslist ) - len,
                "%s %u-%u", len ? "," : "", sid1, sid2 );
    }
}

void
init_rx_reassembler( unsigned int maxcellcount )
{
}


void
init_ut_queues()
{
    int i;
    for( i=0; i<LINEQ_SIZE(0); ++i )
    {
        PRIV->m_st_device_info[0].queue[i] = END;
    }
    for( i=0; i<LINEQ_SIZE(1); ++i )
    {
        PRIV->m_st_device_info[1].queue[i] = END;
    }
}

void
flush_ut_queues()
{
    int line;

    for( line=0; line <2; ++line )
    {
        for( ; NULL != read_rx_lineq_head(line); dequeue_rx_lineq( line ) ) { }
    }
}

void
populate_ut_queues( int *queue0, int *queue1 )
{
    int i,j;

    /* Line-0 */
    for( i=0, j = PRIV->m_st_device_info[0].rx_tail; queue0[i] != END;
         ++i, j = (j + 1)%LINEQ_SIZE(0) )
    {
        PRIV->m_st_device_info[0].queue[j] = queue0[i];
    }
    enqueue_rx_lineq( 0 );

    /* Line-1 */
    for( i=0, j = PRIV->m_st_device_info[1].rx_tail; queue1[i] != END;
         ++i, j = (j + 1)%LINEQ_SIZE(0) )
    {
        PRIV->m_st_device_info[1].queue[j] = queue1[i];
    }
    enqueue_rx_lineq( 1 );
}

void
ut_rx_process_queues_READY( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: RX_PROCESS_QUEUES() in READY state\n" );
    printf( "------------------------------------------------\n" );

    /* Init both the lines */
    init_rx_lineq( 0 );
    init_rx_lineq( 1 );
    init_ut_queues();

    // For both the hw rings, Initialize every desc with OWN flag reset
    for( *total=0; *total < sizeof(ut_rx_process_queues_READY_tests)/sizeof(ut_rx_process_queues_READY_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_process_queues_READY_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* first flush both the lines */
        flush_ut_queues();

        /* enqueue new cells into both the lines */
        populate_ut_queues( ut_rx_process_queues_READY_tests[*total].queue0,
                            ut_rx_process_queues_READY_tests[*total].queue1 );

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = READY;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->ExpectedSID = ut_rx_process_queues_READY_tests[*total].expectedsid;
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = 0;

        process_rx_queues();

        if( ut_rx_process_queues_READY_tests[*total].expectedoutput.expectedsid != PRIV->ExpectedSID
                || ut_rx_process_queues_READY_tests[*total].expectedoutput.timer != PRIV->sidtimer
                || ut_rx_process_queues_READY_tests[*total].expectedoutput.asmcellcount != PRIV->stats.ut_asmcount )
        {
                success = 0;
        }

        printf( "process_rx_queues( %d ) RETURNS: {\n"
                "  - Reassembled Cell SIDs:%s\n"
                "  - ExpectedSID:%d (EXPECTED:%d)\n"
                "  - Timer: %s (EXPECTED:%s)\n"
                "  - Missing Cell SIDs:%s\n"
                "  - Out of range Cell SIDs:%s\n"
                "  - ASM Cell Count: %d (EXPECTED:%d)\n"
                "  - Lines:",
                               ut_rx_process_queues_READY_tests[*total].expectedsid,
                               strlen(PRIV->stats.ut_reassembledcellslist) ? PRIV->stats.ut_reassembledcellslist: "none",
                               PRIV->ExpectedSID, ut_rx_process_queues_READY_tests[*total].expectedoutput.expectedsid,
                               PRIV->sidtimer == 0 ? "OFF" : "ON",
                               ut_rx_process_queues_READY_tests[*total].expectedoutput.timer == 0 ? "OFF" : "ON",
                               strlen(PRIV->stats.ut_missingcellslist) ? PRIV->stats.ut_missingcellslist: "none",
                               strlen(PRIV->stats.ut_droppedcellslist) ? PRIV->stats.ut_droppedcellslist: "none",
                               PRIV->stats.ut_asmcount, ut_rx_process_queues_READY_tests[*total].expectedoutput.asmcellcount
                               );
        DUMP_RX_LINES();
        printf( "}\n" );

        printf( "\nUnit Test %s: ", ut_rx_process_queues_READY_tests[*total].testname );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "--------------------------------------------------\n" );
    printf( "Unit tests for RX_PROCESS_QUEUES() in READY state: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

void
ut_rx_process_queues_LEARNING( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: RX_PROCESS_QUEUES() in LEARNING state\n" );
    printf( "---------------------------------------------------\n" );

    /* Init both the lines */
    init_rx_lineq( 0 );
    init_rx_lineq( 1 );
    init_ut_queues();

    for( *total=0; *total < sizeof(ut_rx_process_queues_LEARNING_tests)/sizeof(ut_rx_process_queues_LEARNING_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_process_queues_LEARNING_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* first flush both the lines */
        flush_ut_queues();

        /* enqueue new cells into both the lines */
        populate_ut_queues( ut_rx_process_queues_LEARNING_tests[*total].queue0,
                            ut_rx_process_queues_LEARNING_tests[*total].queue1 );

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = LEARNING;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = 0;

        process_rx_queues();

        if( ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.state != PRIV->state
                || ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.expectedsid != PRIV->ExpectedSID
                || ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.timer != PRIV->sidtimer
                || ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.asmcellcount != PRIV->stats.ut_asmcount )
        {
                success = 0;
        }

        printf( "process_rx_queues( LEARNING ) RETURNS: {\n"
                "  - State:%s (EXPECTED:%s)\n"
                "  - Reassembled Cell SIDs:%s\n"
                "  - ExpectedSID:%d (EXPECTED:%d)\n"
                "  - Timer: %s (EXPECTED:%s)\n"
                "  - Missing Cell SIDs:%s\n"
                "  - Out of range Cell SIDs:%s\n"
                "  - ASM Cell Count: %d (EXPECTED:%d)\n"
                "  - Lines:",
                               PRIV->state == LEARNING ? "LEARNING" : "READY",
                               ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.state == LEARNING ? "LEARNING" : "READY",
                               strlen(PRIV->stats.ut_reassembledcellslist) ? PRIV->stats.ut_reassembledcellslist: "none",
                               PRIV->ExpectedSID, ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.expectedsid,
                               PRIV->sidtimer == 0 ? "OFF" : "ON",
                               ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.timer == 0 ? "OFF" : "ON",
                               strlen(PRIV->stats.ut_missingcellslist) ? PRIV->stats.ut_missingcellslist: "none",
                               strlen(PRIV->stats.ut_droppedcellslist) ? PRIV->stats.ut_droppedcellslist: "none",
                               PRIV->stats.ut_asmcount, ut_rx_process_queues_LEARNING_tests[*total].expectedoutput.asmcellcount
                               );
        DUMP_RX_LINES();
        printf( "}\n" );

        printf( "\nUnit Test %s: ", ut_rx_process_queues_LEARNING_tests[*total].testname );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "--------------------------------------------------\n" );
    printf( "Unit tests for RX_PROCESS_QUEUES() in LEARNING state: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

void
ut_rx_timeout_handler_READY( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: RX_TIMEOUT_HANDLER() in READY state\n" );
    printf( "-------------------------------------------------\n" );

    /* Init both the lines */
    init_rx_lineq( 0 );
    init_rx_lineq( 1 );
    init_ut_queues();

    for( *total=0; *total < sizeof(ut_rx_timeout_handler_READY_tests)/sizeof(ut_rx_timeout_handler_READY_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_timeout_handler_READY_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* first flush both the lines */
        flush_ut_queues();

        /* enqueue new cells into both the lines */
        populate_ut_queues( ut_rx_timeout_handler_READY_tests[*total].queue0,
                            ut_rx_timeout_handler_READY_tests[*total].queue1 );

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = READY;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->ExpectedSID = ut_rx_timeout_handler_READY_tests[*total].expectedsid;
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = 0;

        timeout_atm_rx();

        if( ut_rx_timeout_handler_READY_tests[*total].expectedoutput.expectedsid != PRIV->ExpectedSID
                || ut_rx_timeout_handler_READY_tests[*total].expectedoutput.timer != PRIV->sidtimer
                || ut_rx_timeout_handler_READY_tests[*total].expectedoutput.asmcellcount != PRIV->stats.ut_asmcount )
        {
                success = 0;
        }

        printf( "rx_timeout_handler( %d ) RETURNS: {\n"
                "  - Reassembled Cell SIDs:%s\n"
                "  - ExpectedSID:%d (EXPECTED:%d)\n"
                "  - Timer: %s (EXPECTED:%s)\n"
                "  - Missing Cell SIDs:%s\n"
                "  - Out of range Cell SIDs:%s\n"
                "  - ASM Cell Count: %d (EXPECTED:%d)\n"
                "  - Lines:",
                               ut_rx_timeout_handler_READY_tests[*total].expectedsid,
                               strlen(PRIV->stats.ut_reassembledcellslist) ? PRIV->stats.ut_reassembledcellslist: "none",
                               PRIV->ExpectedSID, ut_rx_timeout_handler_READY_tests[*total].expectedoutput.expectedsid,
                               PRIV->sidtimer == 0 ? "OFF" : "ON",
                               ut_rx_timeout_handler_READY_tests[*total].expectedoutput.timer == 0 ? "OFF" : "ON",
                               strlen(PRIV->stats.ut_missingcellslist) ? PRIV->stats.ut_missingcellslist: "none",
                               strlen(PRIV->stats.ut_droppedcellslist) ? PRIV->stats.ut_droppedcellslist: "none",
                               PRIV->stats.ut_asmcount, ut_rx_timeout_handler_READY_tests[*total].expectedoutput.asmcellcount
                               );
        DUMP_RX_LINES();
        printf( "}\n" );

        printf( "\nUnit Test %s: ", ut_rx_timeout_handler_READY_tests[*total].testname );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "--------------------------------------------------\n" );
    printf( "Unit tests for RX_TIMEOUT_HANDLER() in READY state: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );
}

void
ut_rx_timeout_handler_LEARNING( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: RX_TIMEOUT_HANDLER() in LEARNING state\n" );
    printf( "----------------------------------------------------\n" );

    /* Init both the lines */
    init_rx_lineq( 0 );
    init_rx_lineq( 1 );
    init_ut_queues();

    for( *total=0; *total < sizeof(ut_rx_timeout_handler_LEARNING_tests)/sizeof(ut_rx_timeout_handler_LEARNING_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_timeout_handler_LEARNING_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* first flush both the lines */
        flush_ut_queues();

        /* enqueue new cells into both the lines */
        populate_ut_queues( ut_rx_timeout_handler_LEARNING_tests[*total].queue0,
                            ut_rx_timeout_handler_LEARNING_tests[*total].queue1 );

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = LEARNING;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = 0;

        timeout_atm_rx();

        if( ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.state != PRIV->state
                || ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.expectedsid != PRIV->ExpectedSID
                || ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.timer != PRIV->sidtimer
                || ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.asmcellcount != PRIV->stats.ut_asmcount )
        {
                success = 0;
        }

        printf( "rx_timeout_handler( LEARNING ) RETURNS: {\n"
                "  - State:%s (EXPECTED:%s)\n"
                "  - Reassembled Cell SIDs:%s\n"
                "  - ExpectedSID:%d (EXPECTED:%d)\n"
                "  - Timer: %s (EXPECTED:%s)\n"
                "  - Missing Cell SIDs:%s\n"
                "  - Out of range Cell SIDs:%s\n"
                "  - ASM Cell Count: %d (EXPECTED:%d)\n"
                "  - Lines:",
                               PRIV->state == LEARNING ? "LEARNING" : "READY",
                               ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.state == LEARNING ? "LEARNING" : "READY",
                               strlen(PRIV->stats.ut_reassembledcellslist) ? PRIV->stats.ut_reassembledcellslist: "none",
                               PRIV->ExpectedSID, ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.expectedsid,
                               PRIV->sidtimer == 0 ? "OFF" : "ON",
                               ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.timer == 0 ? "OFF" : "ON",
                               strlen(PRIV->stats.ut_missingcellslist) ? PRIV->stats.ut_missingcellslist: "none",
                               strlen(PRIV->stats.ut_droppedcellslist) ? PRIV->stats.ut_droppedcellslist: "none",
                               PRIV->stats.ut_asmcount, ut_rx_timeout_handler_LEARNING_tests[*total].expectedoutput.asmcellcount
                               );
        DUMP_RX_LINES();
        printf( "}\n" );

        printf( "\nUnit Test %s: ", ut_rx_timeout_handler_LEARNING_tests[*total].testname );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "--------------------------------------------------\n" );
    printf( "Unit tests for RX_TIMEOUT_HANDLER() in LEARNING state: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}
