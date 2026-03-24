
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h> /* malloc */

void ut_reassemble_aal5( int *total, int *passed, int *failed );
void ut_reassemble_too_large_aal5( int *total, int *passed, int *failed );
void report( char *msg, int dummy1, int dummy2 );
static  unsigned int aal5len;
uint32_t crc32( uint32_t seed, uint8_t * p_p_data, uint32_t p_data_length );
void aal5rcv( unsigned char *aal5, unsigned int aal5len );

/////////////////////////////////////////////////////////////////////
#define PRIV (&g_ut_privdata)
#define REPORT( msg, number1, number2 ) report( (msg), (number1), (number2) )
//#define PRINT(msg) printf( "%s[%d]: %s\n", __func__, __LINE__, (msg) )
#define PRINT(format, ...) printf( "[%s[%d]: " format, __func__, __LINE__, ## __VA_ARGS__)
#define AAL5FRAME_ALLOC( frame, cellcount )  (frame) = malloc( (cellcount) * 48 ); aal5len = 0; PRIV->stats.ut_createdframecount += 1;
#define AAL5FRAME_COPY_CELL( frame, data, len )   memcpy( (frame)+aal5len, (data), (len) ); aal5len += (len); PRIV->stats.ut_copiedcellcount += 1;
#define AAL5FRAME_DISCARD( frame )           free((frame))
#define AAL5FRAME_DATA( frame )              (frame)
#define CALL_TCDRIVER_CB( aal5frame, aal5framelen ) aal5rcv( (aal5frame), (aal5framelen) )

#define UNIT_TESTS_4
#include "ut_rx_definitions.c"
#include "../atm/ima_rx_reassembler.c"
/////////////////////////////////////////////////////////////////////

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
report( char *msg, int dummy1, int dummy2 )
{
    if( strcasecmp( msg, "Frame Discarded" ) == 0 )
    {
        PRIV->stats.ut_discardedframecount += 1;
    } else if( strcasecmp( msg, "Frame Accepted" ) == 0 )
    {
        PRIV->stats.ut_acceptedframecount += 1;
    }
}

int
main( int argc, char **argv )
{
    int grand_total = 0, grand_passed = 0, grand_failed = 0;
    int total, passed, failed;

    ut_reassemble_aal5( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    ut_reassemble_too_large_aal5( &total, &passed, &failed );
    grand_total += total, grand_passed += passed, grand_failed += failed;

    printf( "====================================================================\n");
    printf( "TOTAL: %d      PASSED: %d               FAILED: %d\n", grand_total, grand_passed, grand_failed );
    printf( "====================================================================\n");

    return 0;
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

    printf( "Unit Testing: REASSEMBLE_AAL5()\n" );
    printf( "-------------------------------\n" );

    // Run the tests now
    for( *total=0; *total < sizeof(ut_reassemble_aal5_tests)/sizeof(ut_reassemble_aal5_tests[0]); ++(*total) ) {

        int success = 1;
        int cellcount;
        ima_rx_atm_header_t *cell;

        printf( "\n======================\n" );
        printf( "Starting Unit Test %d: %s", *total + 1, ut_reassemble_aal5_tests[*total].purpose);
        printf( "\n----------------------\n" );

        /* Init Reassembler */
        init_rx_reassembler( 50 );
        PRIV->stats.ut_discardedframecount = 0;
        PRIV->stats.ut_copiedcellcount = 0;
        PRIV->stats.acceptedframecount = 0;
        PRIV->stats.ut_createdframecount = 0;

        for( cellcount = 0; cellcount < sizeof( ut_reassemble_aal5_tests[*total].cells )/sizeof( ut_reassemble_aal5_tests[*total].cells[0] ); ++cellcount )
        {
            cell = (ima_rx_atm_header_t *)ut_reassemble_aal5_tests[*total].cells[cellcount];
            if( 0 == cell->hec ) break;

            reassemble_aal5( cell );
        }

        if( ut_reassemble_aal5_tests[*total].expectedoutput.createdframecount != PRIV->stats.ut_createdframecount
                || ut_reassemble_aal5_tests[*total].expectedoutput.copiedcellcount != PRIV->stats.ut_copiedcellcount
                || ut_reassemble_aal5_tests[*total].expectedoutput.acceptedframecount != PRIV->stats.acceptedframecount
                || ut_reassemble_aal5_tests[*total].expectedoutput.discardedframecount != PRIV->stats.ut_discardedframecount )
        {
                success = 0;
        }

        printf( "%d calls to reassemble_aal5( ) resulted in: {\n"
                "  - Created AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Copied Cell Count:%d (EXPECTED:%d)\n"
                "  - Accepted AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Discarded AAL5 Frames:%d (EXPECTED:%d)\n",
                                cellcount,
                                PRIV->stats.ut_createdframecount, ut_reassemble_aal5_tests[*total].expectedoutput.createdframecount,
                                PRIV->stats.ut_copiedcellcount, ut_reassemble_aal5_tests[*total].expectedoutput.copiedcellcount,
                                PRIV->stats.acceptedframecount, ut_reassemble_aal5_tests[*total].expectedoutput.acceptedframecount,
                                PRIV->stats.ut_discardedframecount, ut_reassemble_aal5_tests[*total].expectedoutput.discardedframecount );

        printf( "\nUnit Test %d: ", *total+1 );
        if( success ) { printf( "PASSED\n" ); ++(*passed); }
        else { printf( "FAILED\n" ); ++(*failed); }
        printf( "======================\n" );
    }

    printf( "--------------------------------------------------\n" );
    printf( "Unit tests for REASSEMBLE_AAL5(): Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}

void
ut_reassemble_too_large_aal5( int *total, int *passed, int *failed )
{
    *passed = 0, *failed = 0, *total = 1;

    printf( "Unit Testing: REASSEMBLE_TOOLARGE_AAL5()\n" );
    printf( "----------------------------------------\n" );

    unsigned char buffer[56] = // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                               { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                               };

    int success = 1;
    int cellcount;
    ima_rx_atm_header_t *cell;

    printf( "\n======================\n" );
    printf( "Starting Unit Test %d: %s", 1, "To verify reassembler discards frame due to frame buffer overflow" );
    printf( "\n----------------------\n" );

    /* Init Reassembler */
    init_rx_reassembler( 10 );
    PRIV->stats.ut_discardedframecount = 0;
    PRIV->stats.ut_copiedcellcount = 0;
    PRIV->stats.acceptedframecount = 0;
    PRIV->stats.ut_createdframecount = 0;

    for( cellcount = 0; cellcount < 10; ++cellcount )
    {
            cell = (ima_rx_atm_header_t *)buffer;
            SETSID( cell, (GETSID(cell, (8 == PRIV->SIDFormat)) + 1), (8 == PRIV->SIDFormat) );

            reassemble_aal5( cell );
    }

    if( 1 != PRIV->stats.ut_createdframecount
        || 9 != PRIV->stats.ut_copiedcellcount
        || 0 != PRIV->stats.acceptedframecount
        || 1 != PRIV->stats.ut_discardedframecount )
        {
                success = 0;
        }

    printf( "%d calls to reassemble_aal5( ) resulted in: {\n"
                "  - Created AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Copied Cell Count:%d (EXPECTED:%d)\n"
                "  - Accepted AAL5 Frames:%d (EXPECTED:%d)\n"
                "  - Discarded AAL5 Frames:%d (EXPECTED:%d)\n",
                                cellcount,
                                PRIV->stats.ut_createdframecount, 1,
                                PRIV->stats.ut_copiedcellcount, 9,
                                PRIV->stats.acceptedframecount, 0,
                                PRIV->stats.ut_discardedframecount, 1 );

    printf( "\nUnit Test 1: " );
    if( success ) { printf( "PASSED\n" ); ++(*passed); }
    else { printf( "FAILED\n" ); ++(*failed); }
    printf( "======================\n" );

    printf( "--------------------------------------------------\n" );
    printf( "Unit Testing: REASSEMBLE_TOOLARGE_AAL5(): Passed=%d, Failed=%d, Total=%d\n", *passed, *failed, *total );
    printf( "--------------------------------------------------\n\n\n" );

}
