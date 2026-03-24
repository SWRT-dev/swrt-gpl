
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

#include "ut_rx_definitions.c"
#include "../atm/ima_rx_bonding.c"
/////////////////////////////////////////////////////////////////////

int
main( int argc, char **argv )
{
    int grand_total = 0, grand_passed = 0, grand_failed = 0;
    int total, passed, failed;

    ut_smaller_sid( &total, &passed, &failed);
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_is_sid_out_of_range( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_peek_rx_nextsid( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

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

void
ut_smaller_sid( int *total, int *passed, int *failed )
{
    signed int result;

    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: SMALLER_SID()\n" );
    printf( "---------------------------\n" );
    for( *total=0; *total < sizeof(ut_smaller_sid_tests)/sizeof(ut_smaller_sid_tests[0]); ++(*total) ) {

        result = smaller_sid( ut_smaller_sid_tests[*total].fromsid, ut_smaller_sid_tests[*total].sid1, ut_smaller_sid_tests[*total].sid2 );

        printf( "% 3d: ", *total+1 );
        if( result == ut_smaller_sid_tests[*total].expected ) { printf( "PASSED: " ); ++(*passed); }
        else { printf( "FAILED: " ); ++(*failed); }
        printf( "smaller_sid( from: %4u, to1: %4u, to2: %4u ) returns % 2d. Expected: % 2d\n", ut_smaller_sid_tests[*total].fromsid, 
                ut_smaller_sid_tests[*total].sid1, ut_smaller_sid_tests[*total].sid2,
                result, ut_smaller_sid_tests[*total].expected );
    }

    printf( "--------------------------------------------------\n" );
    printf( "SMALLER_SID() Unit Tests: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

void
ut_is_sid_out_of_range( int *total, int *passed, int *failed )
{
    signed int result;

    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: IS_SID_OUT_OF_RANGE()\n" );
    printf( "----------------------------------\n" );
    for( *total=0; *total < sizeof(ut_is_sid_out_of_range_tests)/sizeof(ut_is_sid_out_of_range_tests[0]); ++(*total) ) {

        result = is_sid_out_of_range( ut_is_sid_out_of_range_tests[*total].sidfrom, ut_is_sid_out_of_range_tests[*total].sidto, ut_is_sid_out_of_range_tests[*total].sidrange );

        printf( "% 3d: ", *total+1 );
        if( result == ut_is_sid_out_of_range_tests[*total].expected ) { printf( "PASSED: " ); ++(*passed); }
        else { printf( "FAILED: " ); ++(*failed); }
        printf( "is_sid_out_of_range( from: %-4u, to: %-4u, range: %-4u ) returns %d. Expected: %d\n", 
                ut_is_sid_out_of_range_tests[*total].sidfrom, ut_is_sid_out_of_range_tests[*total].sidto,
                ut_is_sid_out_of_range_tests[*total].sidrange, result, ut_is_sid_out_of_range_tests[*total].expected );
    }

    printf( "--------------------------------------------------\n" );
    printf( "IS_SID_OUT_OF_RANGE() Unit Tests: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

#define DUMP_RX_LINES() \
    { \
        int i; \
        printf( "\n        ------------------------------------------------------------------------------\n        "); \
        printf( "LINE-0: " ); if( END == g_lines[0].queue[g_lines[0].head] ) printf( "<empty>" ); \
                              else { for( i = g_lines[0].head; END != g_lines[0].queue[i]; ++i ) { if( ASM == g_lines[0].queue[i] ) printf( " ASM " ); else printf( "% 4d ", g_lines[0].queue[i] ); } } \
        printf( "\n        ------------------------------------------------------------------------------\n        "); \
        printf( "LINE-1: " ); if( END == g_lines[1].queue[g_lines[1].head] ) printf( "<empty>" ); \
                              else { for( i = g_lines[1].head; END != g_lines[1].queue[i]; ++i ) { if( ASM == g_lines[1].queue[i] ) printf( " ASM " ); else printf( "% 4d ", g_lines[1].queue[i] ); } } \
        printf( "\n        ------------------------------------------------------------------------------\n"); \
    }

struct {
    int *queue;
    int head;
} g_lines[2];

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

unsigned char *
read_rx_lineq_head( unsigned int lineid )
{
    if( END == g_lines[lineid].queue[ g_lines[lineid].head ] ) {
        return NULL;
    }

    return (unsigned char *)( g_lines[lineid].queue + g_lines[lineid].head );
}

void
dequeue_rx_lineq( unsigned int lineid )
{
    //printf( "Removing cell from head of line-%d\n", lineid ); fflush( stdout );
    g_lines[lineid].head += 1;
}

unsigned int
enqueue_rx_lineq( unsigned int lineid )
{
    return 0;
}

void
init_rx_lineq( unsigned int lineid )
{
    return;
}

void
init_rx_reassembler( unsigned int maxcellcount )
{
}

void
ut_peek_rx_nextsid( int *total, int *passed, int *failed )
{
    signed int result;
    unsigned int nextsid;
    unsigned int lineid;
    unsigned char *cell;
    int count;

    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: PEEK_RX_NEXTSID()\n" );
    printf( "-------------------------------\n" );

    for( *total=0; *total < sizeof(ut_peek_rx_nextsid_tests)/sizeof(ut_peek_rx_nextsid_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\nStarting Unit Test %d: ", *total + 1 );

        /* enqueue cells into the 2 lines */
        g_lines[0].queue = ut_peek_rx_nextsid_tests[*total].queue0;
        g_lines[0].head = 0;
        g_lines[1].queue = ut_peek_rx_nextsid_tests[*total].queue1;
        g_lines[1].head = 0;

        /* Keep calling (NextSID, dequeue) pairs until the expectedsid becomes -1 */
        for( count = 0; 0 <= ut_peek_rx_nextsid_tests[*total].expectedsids[count]; ++count ) {

            DUMP_RX_LINES(); 
            result = peek_rx_nextsid( ut_peek_rx_nextsid_tests[*total].expectedsids[count], &nextsid, &lineid, &cell );

            if( ut_peek_rx_nextsid_tests[*total].expectedoutput[count].nonemptylines != result
                || ( result > 0 && ( ut_peek_rx_nextsid_tests[*total].expectedoutput[count].nextsid != nextsid
                                     || ut_peek_rx_nextsid_tests[*total].expectedoutput[count].lineid != lineid ) ) )
            {
                success = 0;
            }

            printf( "     NextSID( ExpectedSID:%d ) RETURNS: { NonEmptyLineCount:%d (EXPECTED:%d)",
                               ut_peek_rx_nextsid_tests[*total].expectedsids[count], result, ut_peek_rx_nextsid_tests[*total].expectedoutput[count].nonemptylines );

            if( result > 0 ) {
                printf( ", NextSID:%d (EXPECTED:%d), LineID:%d (EXPECTED:%d) }\n",
                                                                nextsid, ut_peek_rx_nextsid_tests[*total].expectedoutput[count].nextsid,
                                                                lineid, ut_peek_rx_nextsid_tests[*total].expectedoutput[count].lineid );
                printf( "        Dequeuing head of line-%d\n", lineid );
                dequeue_rx_lineq(lineid); // release the cage
            } else {
                printf( " }\n" );
            }
        }

        printf( "\nUnit Test %d: ", *total + 1 );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
    }

    printf( "--------------------------------------------------\n" );
    printf( "PEEK_RX_NEXTSID() Unit Tests: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

void
ut_rx_process_queues_READY( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: RX_PROCESS_QUEUES() in READY state\n" );
    printf( "------------------------------------------------\n" );

    for( *total=0; *total < sizeof(ut_rx_process_queues_READY_tests)/sizeof(ut_rx_process_queues_READY_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_process_queues_READY_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* enqueue cells into the 2 lines */
        g_lines[0].queue = ut_rx_process_queues_READY_tests[*total].queue0;
        g_lines[0].head = 0;
        g_lines[1].queue = ut_rx_process_queues_READY_tests[*total].queue1;
        g_lines[1].head = 0;

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = READY;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->ExpectedSID = ut_rx_process_queues_READY_tests[*total].expectedsid;
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = NULL;

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
                               PRIV->sidtimer == NULL ? "OFF" : "ON",
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

    for( *total=0; *total < sizeof(ut_rx_process_queues_LEARNING_tests)/sizeof(ut_rx_process_queues_LEARNING_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_process_queues_LEARNING_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* enqueue cells into the 2 lines */
        g_lines[0].queue = ut_rx_process_queues_LEARNING_tests[*total].queue0;
        g_lines[0].head = 0;
        g_lines[1].queue = ut_rx_process_queues_LEARNING_tests[*total].queue1;
        g_lines[1].head = 0;

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = LEARNING;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = NULL;

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
                               PRIV->sidtimer == NULL ? "OFF" : "ON",
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

    for( *total=0; *total < sizeof(ut_rx_timeout_handler_READY_tests)/sizeof(ut_rx_timeout_handler_READY_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_timeout_handler_READY_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* enqueue cells into the 2 lines */
        g_lines[0].queue = ut_rx_timeout_handler_READY_tests[*total].queue0;
        g_lines[0].head = 0;
        g_lines[1].queue = ut_rx_timeout_handler_READY_tests[*total].queue1;
        g_lines[1].head = 0;

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = READY;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->ExpectedSID = ut_rx_timeout_handler_READY_tests[*total].expectedsid;
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = NULL;

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
                               PRIV->sidtimer == NULL ? "OFF" : "ON",
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

    for( *total=0; *total < sizeof(ut_rx_timeout_handler_LEARNING_tests)/sizeof(ut_rx_timeout_handler_LEARNING_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %s: ", ut_rx_timeout_handler_LEARNING_tests[*total].testname );
        printf( "\n----------------------\n" );

        /* enqueue cells into the 2 lines */
        g_lines[0].queue = ut_rx_timeout_handler_LEARNING_tests[*total].queue0;
        g_lines[0].head = 0;
        g_lines[1].queue = ut_rx_timeout_handler_LEARNING_tests[*total].queue1;
        g_lines[1].head = 0;

        DUMP_RX_LINES(); 

        /* Set up the requirements */
        PRIV->state = LEARNING;
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0';
        PRIV->stats.ut_asmcount = 0;
        PRIV->sidtimer = NULL;

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
                               PRIV->sidtimer == NULL ? "OFF" : "ON",
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
