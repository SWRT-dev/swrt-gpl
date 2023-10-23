
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
#include <stdint.h>

void ut_reassemble_aal5( int *total, int *passed, int *failed );

void process_asm( unsigned char *cell, unsigned int lineid );
void process_nonasm( unsigned char *cell );
void report( char *msg, unsigned int sid1, unsigned int sid2);
void init_ut_queues();
void flush_ut_queues();
void populate_ut_queues( unsigned char cells[][56] );
uint32_t crc32( uint32_t seed, uint8_t * p_p_data, uint32_t p_data_length );
void aal5rcv( unsigned char *aal5, unsigned int aal5len );

/////////////////////////////////////////////////////////////////////
#define PRIV (&g_ut_privdata)
#define START_SID_TIMER( func )   ((void *)1)
#define DISABLE_SID_TIMER() ;
#define PROCESS_NONASM(cell, lineid) process_nonasm( (cell) );
#define PROCESS_ASM(cell, lineid) process_asm( (cell), (lineid) );
#define REPORT( msg, number1, number2 ) report( (msg), (number1), (number2) )
#define PRINT(format, ...) printf( "[%s[%d]: " format, __func__, __LINE__, ## __VA_ARGS__)

#define LINEQ_SIZE( lineid ) (sizeof(PRIV->m_st_device_info[(lineid)].queue)/sizeof(PRIV->m_st_device_info[(lineid)].queue[0]))
#define GET_ELEMENT( lineid, idx )  (PRIV->m_st_device_info[(lineid)].queue[(idx)].buffer)
#define RELEASE_LINEQ_HEAD( lineid, idx ) PRIV->m_st_device_info[(lineid)].queue[(idx)].flag = 0;
#define LINEQ_ELEMENT_IS_NEW( lineid, idx ) (PRIV->m_st_device_info[(lineid)].queue[(idx)].flag)

static  unsigned int aal5len;
#define AAL5FRAME_ALLOC( frame, cellcount )  (frame) = malloc( (cellcount) * 48 ); aal5len = 0; PRIV->stats.ut_createdframecount += 1;
#define AAL5FRAME_COPY_CELL( frame, data, len )   memcpy( (frame)+aal5len, (data), (len) ); aal5len += (len); PRIV->stats.ut_copiedcellcount += 1;
#define AAL5FRAME_DISCARD( frame )           free((frame))
#define AAL5FRAME_DATA( frame )              (frame)
#define CALL_TCDRIVER_CB( aal5frame, aal5framelen ) aal5rcv( (aal5frame), (aal5framelen) )

#define UNIT_TESTS_5
#include "ut_rx_definitions.c"
#include "../atm/ima_rx_bonding.c"
#include "../common/ima_rx_queue.c"
#include "atm/ima_rx_atm_cell.h"
#include "../atm/ima_rx_reassembler.c"
/////////////////////////////////////////////////////////////////////

int
main( int argc, char **argv )
{
    int grand_total = 0, grand_passed = 0, grand_failed = 0;
    int total, passed, failed;

    ut_reassemble_aal5( &total, &passed, &failed );
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
                              if( 0 == PRIV->m_st_device_info[0].rx_count ) printf( "\n         <empty>" ); \
                              else { \
                                  printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "+---------+ " ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| VPI:%-4u| ", GETVPI( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| VCI:%-4u| ", GETVCI( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| SID:%-4u| ", GET_SID( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT3:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) )->pt3 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT2:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) )->pt2 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT1:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) )->pt1 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| CLP:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) )->clp ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| HEC:0x%02x| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[0].queue[j].buffer) )->hec ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "+---------+ " ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[0].rx_head; i < PRIV->m_st_device_info[0].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( " Cell #%-4d ", i+1 ); \
                                  } \
                              } \
        printf( "\n        ------------------------------------------------------------------------------\n        "); \
        printf( "LINE-1: (HEAD: %d, TAIL: %d, COUNT: %d): ", PRIV->m_st_device_info[1].rx_head, PRIV->m_st_device_info[1].rx_tail, PRIV->m_st_device_info[1].rx_count ); \
                              if( 0 == PRIV->m_st_device_info[1].rx_count ) printf( "\n         <empty>" ); \
                              else { \
                                  printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "+---------+ " ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| VPI:%-4u| ", GETVPI( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| VCI:%-4u| ", GETVCI( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| SID:%-4u| ", GET_SID( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) ) ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT3:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) )->pt3 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT2:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) )->pt2 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| PT1:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) )->pt1 ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| CLP:%-4d| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) )->clp ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "| HEC:0x%02x| ", ( (ima_rx_atm_header_t *)(PRIV->m_st_device_info[1].queue[j].buffer) )->hec ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( "+---------+ " ); \
                                  } printf ("\n         " ); \
                                  for( i=0, j=PRIV->m_st_device_info[1].rx_head; i < PRIV->m_st_device_info[1].rx_count; ++i, j = (j+1)%(LINEQ_SIZE(0)) ) { \
                                      printf( " Cell #%-4d ", i+1 ); \
                                  } \
                              } \
        printf( "\n        ------------------------------------------------------------------------------\n\n"); \
    }

void
process_nonasm( unsigned char *cell )
{
    //printf( "ASM PROCESSED in Line-%d\n", lineid );
    int len = strlen( PRIV->stats.ut_reassembledcellslist );
    snprintf( PRIV->stats.ut_reassembledcellslist + len, sizeof( PRIV->stats.ut_reassembledcellslist ) - len,
              "%s %u", len ? "," : "", GET_SID(cell) );
    reassemble_aal5( (ima_rx_atm_header_t *)cell ); // Ensured that reassembler is initiated before every test
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
    } else if( strcasecmp( msg, "Frame Discarded" ) == 0 )
    {
        PRIV->stats.ut_discardedframecount += 1;
    } else if( strcasecmp( msg, "Frame Accepted" ) == 0 )
    {
        PRIV->stats.ut_acceptedframecount += 1;
    }
}

uint32_t
crc32( uint32_t seed, uint8_t * p_p_data, uint32_t p_data_length )
{
    uint32_t field = 0xa1b2c3d4;
#if 0
    ((uint8_t *)(&field))[0] = 0xa1;
    ((uint8_t *)(&field))[1] = 0xb2;
    ((uint8_t *)(&field))[2] = 0xc3;
    ((uint8_t *)(&field))[3] = 0xd4;
#endif
    return ntohl(field) ^ seed;
}


void
init_ut_queues()
{
    int i;
    for( i=0; i<LINEQ_SIZE(0); ++i )
    {
        PRIV->m_st_device_info[0].queue[i].flag = 0;
    }
    for( i=0; i<LINEQ_SIZE(1); ++i )
    {
        PRIV->m_st_device_info[1].queue[i].flag = 0;
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
populate_ut_queues( unsigned char cells[][56] )
{
    int line0tail,line1tail;
    int cellcount;
    ima_rx_atm_header_t *cell;

    line0tail = PRIV->m_st_device_info[0].rx_tail;
    line1tail = PRIV->m_st_device_info[1].rx_tail;
    for( cellcount = 0; ; ++cellcount )
    {
        cell = (ima_rx_atm_header_t *)cells[cellcount];

        // no more cells
        if( 0 == cell->hec ) break;

        // assign it to one of the two line queues
        if( cellcount % 2 ) 
        {
printf( "Adding %d-th cell to line-0 at tail %d\n", cellcount+1, line0tail );
            memcpy( PRIV->m_st_device_info[0].queue[line0tail].buffer, cells[cellcount], 56 );
            PRIV->m_st_device_info[0].queue[line0tail].flag = 1;
            line0tail = (line0tail + 1)%LINEQ_SIZE(0);
        } else {
printf( "Adding %d-th cell to line-1 at tail %d\n", cellcount+1, line1tail );
            memcpy( PRIV->m_st_device_info[1].queue[line1tail].buffer, cells[cellcount], 56 );
            PRIV->m_st_device_info[1].queue[line1tail].flag = 1;
            line1tail = (line1tail + 1)%LINEQ_SIZE(1);
        }
    }

    /* This MUST be done (by poller) */
    //enqueue_rx_lineq( 0 );
    //enqueue_rx_lineq( 1 );
    poll_rx_bonding();
}

void
aal5rcv( unsigned char *aal5, unsigned int aal5len )
{
    int i;
    printf( "Reassembled an AAL5 frame of %d bytes\n", aal5len );

    for( i=0; i<aal5len; ++i )
    {
        printf( "%02X ", aal5[i] );
        if( (i+1)%24 == 0 ) printf( "\n" );
        else if( (i+1)%12 == 0 ) printf( "  " );
    }
    
    free(aal5);
}

void
ut_reassemble_aal5( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 0;

    printf( "Unit Testing: FULL-STACK\n" );
    printf( "------------------------\n" );

    /* Init both the lines */
    init_rx_lineq( 0 );  /* OK */
    init_rx_lineq( 1 );  /* OK */
    init_ut_queues();    /* OK */

    // For both the hw rings, Initialize every desc with OWN flag reset
    for( *total=0; *total < sizeof(ut_reassemble_aal5_tests)/sizeof(ut_reassemble_aal5_tests[0]); ++(*total) ) {

        int success = 1;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %d: %s", *total+1, ut_reassemble_aal5_tests[*total].purpose );
        printf( "\n----------------------\n" );

        /* first flush both the lines */
        flush_ut_queues();  /* OK */

        // TODO: 2. Ensure in the test case definitions, none of these cells have VPI/VCI 0/20
        /* enqueue new cells into both the lines */
        populate_ut_queues( ut_reassemble_aal5_tests[*total].cells );

        DUMP_RX_LINES(); /* OK */

        /* Set up the requirements */
        PRIV->state = READY; /* OK */
        PRIV->stats.ut_reassembledcellslist[0] = PRIV->stats.ut_missingcellslist[0] = PRIV->stats.ut_droppedcellslist[0] = '\0'; /* OK */
        //TODO: Set this expectedSID to the first SID of cell-stream defined for this test case
        PRIV->ExpectedSID = GET_SID((ima_rx_atm_header_t *)(ut_reassemble_aal5_tests[*total].cells[0]));
        PRIV->stats.ut_asmcount = 0; /* OK */
        PRIV->sidtimer = 0; /* OK */

        /* Init Reassembler */
        init_rx_reassembler( 50 );
        PRIV->stats.ut_discardedframecount = 0;
        PRIV->stats.ut_copiedcellcount = 0;
        PRIV->stats.acceptedframecount = 0;
        PRIV->stats.ut_createdframecount = 0;

        process_rx_queues();

        if( ut_reassemble_aal5_tests[*total].expectedoutput.createdframecount != PRIV->stats.ut_createdframecount
                || ut_reassemble_aal5_tests[*total].expectedoutput.copiedcellcount != PRIV->stats.ut_copiedcellcount
                || ut_reassemble_aal5_tests[*total].expectedoutput.acceptedframecount != PRIV->stats.acceptedframecount
                || ut_reassemble_aal5_tests[*total].expectedoutput.discardedframecount != PRIV->stats.ut_discardedframecount )
        {
                success = 0;
        }

        printf( "process_rx_queues( %d ) RETURNS: {\n"
                "  - Reassembled Cell SIDs:%s\n"
                "  - ExpectedSID:%d\n"
                "  - Timer: %s\n"
                "  - Missing Cell SIDs:%s\n"
                "  - Out of range Cell SIDs:%s\n"
                "  - ASM Cell Count: %d\n"
                "  - Created AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Copied Cell Count:%d (EXPECTED:%d)\n"
                "  - Accepted AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Discarded AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Lines:",
                               GET_SID(((ima_rx_atm_header_t *)(ut_reassemble_aal5_tests[*total].cells[0]))),
                               strlen(PRIV->stats.ut_reassembledcellslist) ? PRIV->stats.ut_reassembledcellslist: "none",
                               PRIV->ExpectedSID,
                               PRIV->sidtimer == 0 ? "OFF" : "ON",
                               strlen(PRIV->stats.ut_missingcellslist) ? PRIV->stats.ut_missingcellslist: "none", /* OK */
                               strlen(PRIV->stats.ut_droppedcellslist) ? PRIV->stats.ut_droppedcellslist: "none", /* OK */
                               PRIV->stats.ut_asmcount,
                               PRIV->stats.ut_createdframecount, ut_reassemble_aal5_tests[*total].expectedoutput.createdframecount,
                               PRIV->stats.ut_copiedcellcount, ut_reassemble_aal5_tests[*total].expectedoutput.copiedcellcount,
                               PRIV->stats.acceptedframecount, ut_reassemble_aal5_tests[*total].expectedoutput.acceptedframecount,
                               PRIV->stats.ut_discardedframecount, ut_reassemble_aal5_tests[*total].expectedoutput.discardedframecount
                               );
        DUMP_RX_LINES(); /* OK */
        printf( "}\n" ); /* OK */

        printf( "\nUnit Test %d: ", *total + 1 );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "-------------------------\n" );
    printf( "Unit Testing: FULL-STACK: Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "-------------------------\n\n\n" );

}

