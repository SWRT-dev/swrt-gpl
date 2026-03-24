
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <stdint.h>
#include "atm/ima_rx_bonding.h"

#if defined UNIT_TESTS_3 || defined UNIT_TESTS_4 || defined UNIT_TESTS_5
#include "atm/ima_rx_atm_cell.h"
#endif

//##################################
//

typedef struct {

    unsigned char      SIDFormat;
    unsigned int       SIDRange;
    unsigned int       ExpectedSID;
    RxBondingState_t state;
    void *             sidtimer;

//==================
// Needed only by Unit Tests 2:

    struct {
        unsigned int rx_size,
                     rx_head,
                     rx_tail,
                     rx_count;

#ifdef UNIT_TESTS_2
        int queue[1024];
#else
#if defined UNIT_TESTS_3 || defined UNIT_TESTS_5
        struct {
            unsigned char buffer[sizeof(ima_rx_atm_header_t) + 48];
            unsigned char flag;
        } queue[1024];
#endif
#endif

    } m_st_device_info[2];
//==================

#if defined UNIT_TESTS_4 || defined UNIT_TESTS_5
    struct {
        uint8_t rejectionmode;
        uint32_t cellcount;
        uint8_t  *frame;
        uint16_t lastsid;
        uint32_t maxcellcount;
    } reassemble;
#endif

    // Statistics & Debug
    struct {
        uint32_t           asmcount[2];
        uint32_t           nonasmcount[2];
        uint32_t           droppedcells;
        uint32_t           timedoutcells;
        uint32_t           missingcells;
        uint32_t           acceptedframecount;
        uint32_t           discardedframecount[MAXTYPES];

        uint32_t           ut_asmcount;
        uint32_t           ut_acceptedframecount;
        uint32_t           ut_discardedframecount;
        char               ut_reassembledcellslist[1024];
        char               ut_missingcellslist[1024];
        char               ut_droppedcellslist[1024];
        char               ut_copiedcellcount;
        char               ut_createdframecount;
    } stats;

} ut_RxPrivData_t;

ut_RxPrivData_t g_ut_privdata = { 8, 256, 0 };

#define __attribute__(x)

#define likely(x)  (x)
#define unlikely(x)  (x)
//##################################


struct {
    unsigned int sid1, sid2, fromsid;
    signed int expected;
} ut_smaller_sid_tests [] = {

       /* ExpectedSID = 0 */
               /*  1 */ { 1, 2, 0, 1 },
               /*  2 */ { 2, 1, 0, -1 },
               /*  3 */ { 2, 2, 0, 0 },
               /*  4 */ { 0, 0, 0, 0 },

       /* ExpectedSID = 1 */
               /*  5 */ { 2, 0, 1, 1 },
               /*  6 */ { 1, 255, 1, 1 },
               /*  7 */ { 255, 1, 1, -1 },
               /*  8 */ { 255, 255, 1, 0 },
               /*  9 */ { 255, 5, 1, -1 },
               /* 10 */ { 5, 255, 1, 1 },
               /* 11 */ { 0, 0, 1, 0 },
               /* 12 */ { 1, 1, 1, 0 },
               /* 13 */ { 2, 2, 1, 0 },
               /* 14 */ { 1, 1, 1, 0 },

       /* ExpectedSID = 128 */
               /* 15 */ { 129, 130, 128, 1 },
               /* 16 */ { 130, 129, 128, -1 },
               /* 17 */ { 129, 129, 128, 0 },
               /* 18 */ { 129, 127, 128, 1 },
               /* 19 */ { 127, 129, 128, -1 },
               /* 20 */ { 127, 127, 128, 0 },
               /* 21 */ { 128, 128, 128, 0 },
               /* 22 */ { 200, 0, 128, 1 },
               /* 23 */ { 0, 200, 128, -1 },

       /* ExpectedSID = 255 */
               /* 24 */ { 0, 6, 255, 1 },
               /* 25 */ { 6, 0, 255, -1 },
               /* 26 */ { 0, 0, 255, 0 },
               /* 27 */ { 200, 230, 255, 1 },
               /* 28 */ { 230, 200, 255, -1 },
               /* 29 */ { 230, 230, 255, 0 },

       /* ExpectedSID = 250 */
               /* 30 */ { 254, 0, 250, 1 },
               /* 31 */ { 0, 254, 250, -1 },
               /* 32 */ { 0, 0, 250, 0 },
               /* 33 */ { 254, 254, 250, 0 },

           };

struct {
    unsigned int sidfrom, sidto, sidrange;
    signed int expected;
} ut_is_sid_out_of_range_tests[] = {

       /* ExpectedSID = 0 , SIDRange = 256 */
               /*  1 */ {    0,    0,  256, 0 },
               /*  2 */ {    0,    1,  256, 0 },
               /*  3 */ {    0,  127,  256, 0 },
               /*  4 */ {    0,  128,  256, 1 },
               /*  5 */ {    0,  129,  256, 1 },
               /*  6 */ {    0,  254,  256, 1 },
               /*  7 */ {    0,  256,  256, 1 },

       /* ExpectedSID = 0 , SIDRange = 4096 */
               /*  1 */ {    0,    0, 4096, 0 },
               /*  2 */ {    0,    1, 4096, 0 },
               /*  3 */ {    0, 2047, 4096, 0 },
               /*  4 */ {    0, 2048, 4096, 1 },
               /*  5 */ {    0, 2049, 4096, 1 },
               /*  6 */ {    0, 4095, 4096, 1 },
               /*  7 */ {    0, 4096, 4096, 1 },

       /* ExpectedSID = 1 , SIDRange = 256 */
               /*  1 */ {    1,    0,  256, 1 },
               /*  2 */ {    1,    1,  256, 0 },
               /*  3 */ {    1,  127,  256, 0 },
               /*  4 */ {    1,  128,  256, 0 },
               /*  5 */ {    1,  129,  256, 1 },
               /*  6 */ {    1,  254,  256, 1 },
               /*  7 */ {    1,  256,  256, 1 },

       /* ExpectedSID = 1 , SIDRange = 4096 */
               /*  1 */ {    1,    0, 4096, 1 },
               /*  2 */ {    1,    1, 4096, 0 },
               /*  3 */ {    1, 2047, 4096, 0 },
               /*  4 */ {    1, 2048, 4096, 0 },
               /*  5 */ {    1, 2049, 4096, 1 },
               /*  6 */ {    1, 4095, 4096, 1 },
               /*  7 */ {    1, 4096, 4096, 1 },

       /* ExpectedSID = 127 , SIDRange = 256 */
               /*  1 */ {  127,    0,  256, 1 },
               /*  2 */ {  127,    1,  256, 1 },
               /*  3 */ {  127,  127,  256, 0 },
               /*  4 */ {  127,  128,  256, 0 },
               /*  5 */ {  127,  129,  256, 0 },
               /*  6 */ {  127,  254,  256, 0 },
               /*  7 */ {  127,  255,  256, 1 },
               /*  8 */ {  127,  256,  256, 1 },

       /* ExpectedSID = 2047 , SIDRange = 4096 */
               /*  1 */ { 2047,    0, 4096, 1 },
               /*  2 */ { 2047,    1, 4096, 1 },
               /*  3 */ { 2047, 2047, 4096, 0 },
               /*  4 */ { 2047, 2048, 4096, 0 },
               /*  5 */ { 2047, 2049, 4096, 0 },
               /*  6 */ { 2047, 4094, 4096, 0 },
               /*  7 */ { 2047, 4095, 4096, 1 },
               /*  8 */ { 2047, 4096, 4096, 1 },

       /* ExpectedSID = 255 , SIDRange = 256 */
               /*  1 */ {  255,    0,  256, 0 },
               /*  2 */ {  255,    1,  256, 0 },
               /*  3 */ {  255,  126,  256, 0 },
               /*  4 */ {  255,  127,  256, 1 },
               /*  5 */ {  255,  128,  256, 1 },
               /*  6 */ {  255,  254,  256, 1 },
               /*  7 */ {  255,  255,  256, 0 },
               /*  8 */ {  255,  256,  256, 0 },

       /* ExpectedSID = 250 , SIDRange = 256 */
               /*  1 */ {  250,    0,  256, 0 },
               /*  2 */ {  250,    1,  256, 0 },
               /*  3 */ {  250,  121,  256, 0 },
               /*  4 */ {  250,  122,  256, 1 },
               /*  5 */ {  250,  123,  256, 1 },
               /*  6 */ {  250,  249,  256, 1 },
               /*  7 */ {  250,  250,  256, 0 },
               /*  8 */ {  250,  256,  256, 0 },
};

#define ASM  -100
#define END  -1

struct {
    int queue0[100];
    int queue1[100];
    int expectedsids[100];

    struct {
        int nonemptylines;
        int nextsid;
        int lineid;
    } expectedoutput[100];

} ut_peek_rx_nextsid_tests[] = {
                {
                            { 0, ASM, 10, END },
                            { END             },
                            { 0, 1, 11, -1 },
                            {
                                { 1,  0,  0 },
                                { 1, 10,  0 },
                                { 0 },
                            }
                },
                {
                            { ASM, 0,  ASM, END },
                            { ASM, 255, END },
                            { 255, 0, 1, -1 },
                            {
                                { 2, 255,  1 },
                                { 1,   0,  0 },
                                { 0 },
                            }
                },
                {
                            { END },
                            { END },
                            { 0, -1 },
                            {
                                { 0 },
                            }
                },
                {
                            { END },
                            { ASM, END },
                            { 128, -1 },
                            {
                                { 0 },
                            }
                },
                {
                            { ASM, END },
                            { ASM, END },
                            { 255, -1 },
                            {
                                { 0 },
                            }
                },
                {
                            { 2, END },
                            { END },
                            { 2, 3, -1 },
                            {
                                { 1,  2,  0 },
                                { 0 },
                            }
                },
                {
                            { END },
                            { 128, ASM, END },
                            { 128, 129, -1 },
                            {
                                { 1, 128,  1 },
                                { 0 },
                            }
                },
                {
                            { 255, ASM, END },
                            { ASM, END },
                            { 255, 0, -1 },
                            {
                                { 1, 255,  0 },
                                { 0 },
                            }
                },
                {
                            { 200, END },
                            { ASM, END },
                            { 1,  1, -1 },
                            {
                                { 1, 200,  0 },
                                { 0 },
                            }
                },
                {
                            { END },
                            { 5, END },
                            { 128, 128, -1 },
                            {
                                { 1,   5,  1 },
                                { 0 },
                            }
                },
                {
                            { ASM, 250, ASM, END },
                            { ASM, END },
                            { 255, 255, -1 },
                            {
                                { 1, 250,  0 },
                                { 0 },
                            }
                },
                {
                            { 2, END },
                            { END },
                            { 1,  1, -1 },
                            {
                                { 1,  2,  0 },
                                { 0 },
                            }
                },
                {
                            { ASM, END },
                            { 129, ASM, END },
                            { 127, 127, -1 },
                            {
                                { 1, 129,  1 },
                                { 0 },
                            }
                },
                {
                            { ASM, 1, END },
                            { END },
                            { 255, 255, -1 },
                            {
                                { 1,  1,  0 },
                                { 0 },
                            }
                },
                {
                            { 4, 5, ASM, 6, END },
                            { END },
                            { 4, 5, 6, 7, -1 },
                            {
                                { 1,  4,  0 },
                                { 1,  5,  0 },
                                { 1,  6,  0 },
                                { 0 },
                            }
                },
                {
                            { END },
                            { 126, 127, 128, END },
                            { 126, 127, 128, 129, -1 },
                            {
                                { 1, 126,  1 },
                                { 1, 127,  1 },
                                { 1, 128,  1 },
                                { 0 },
                            }
                },
                {
                            { 254, 255, ASM, 0, 1, END },
                            { ASM, END },
                            { 254, 255, 0, 1, 2, -1 },
                            {
                                { 1, 254,  0 },
                                { 1, 255,  0 },
                                { 1,   0,  0 },
                                { 1,   1,  0 },
                                { 0 },
                            }
                },
                {
                            { 4, 5, ASM, 8, END },
                            { ASM, END },
                            { 4, 5, 6, 6, -1 },
                            {
                                { 1,  4,  0 },
                                { 1,  5,  0 },
                                { 1,  8,  0 },
                                { 0 },
                            },
                },
                {
                            { END },
                            { 126, 127, 130, ASM, END },
                            { 126, 127, 128, 128, -1 },
                            {
                                { 1, 126,  1 },
                                { 1, 127,  1 },
                                { 1, 130,  1 },
                                { 0 },
                            },
                },
                {
                            { 254, 255, 2, 3, END },
                            { END },
                            { 254, 255, 0, 0, 0, -1 },
                            {
                                { 1, 254,  0 },
                                { 1, 255,  0 },
                                { 1,   2,  0 },
                                { 1,   3,  0 },
                                { 0 },
                            },
                },
                {
                            { 1, 3, 4, 6, END },
                            { 2, 5, END },
                            { 1, 2, 3, 4, 5, 6, 7, -1 },
                            {
                                { 2,  1,  0 },
                                { 2,  2,  1 },
                                { 2,  3,  0 },
                                { 2,  4,  0 },
                                { 2,  5,  1 },
                                { 1,  6,  0 },
                                { 0 },
                            },
                },
                {
                            { 126, 129, ASM, END },
                            { 125, ASM, 127, 128, 130, ASM, END },
                            { 125, 126, 127, 128, 129, 130, 131, -1 },
                            {
                                { 2, 125,  1 },
                                { 2, 126,  0 },
                                { 2, 127,  1 },
                                { 2, 128,  1 },
                                { 2, 129,  0 },
                                { 1, 130,  1 },
                                { 0 },
                            },
                },
                {
                            { ASM, 254, 0, 1, 2, END },
                            { ASM, 253, 255, END },
                            { 253, 254, 255, 0, 1, 2, 3, -1 },
                            {
                                { 2, 253, 1 },
                                { 2, 254, 0 },
                                { 2, 255, 1 },
                                { 1,   0, 0 },
                                { 1,   1, 0 },
                                { 1,   2, 0 },
                                { 0 },
                            },
                },
                {
                            { 20, END },
                            { 10, END },
                            { 2, 20, 21, -1 },
                            {
                                { 2, 10, 1 },
                                { 1, 20, 0 },
                                { 0 },
                            },
                },
                {
                            { ASM, 130, END },
                            { 150, END },
                            { 123, 150, 151, -1 },
                            {
                                { 2, 130, 0 },
                                { 1, 150, 1 },
                                { 0 },
                            },
                },
                {
                            { 20, END },
                            { 6, END },
                            { 253, 7, 21, -1 },
                            {
                                { 2,   6, 1 },
                                { 1,  20, 0 },
                                { 0 },
                            },
                },
                {
                            { ASM, 20, END },
                            { 1, END },
                            { 2, 21, 21, -1 },
                            {
                                { 2, 20, 0 },
                                { 1,  1, 1 },
                                { 0 },
                            },
                },
                {
                            { 122, END },
                            { 150, END },
                            { 123, 151, 151, -1 },
                            {
                                { 2, 150, 1 },
                                { 1, 122, 0 },
                                { 0 },
                            },
                },
                {
                            {  20, ASM, END },
                            { 252, ASM, END },
                            { 253, 21, 253, -1 },
                            {
                                { 2, 20, 0 },
                                { 1, 252, 1 },
                                { 0 },
                            },
                },
                {
                            { 100, END },
                            { 200, END },
                            { 1, 100, 201, -1 },
                            {
                                { 2, 100, 0 },
                                { 1, 200, 1 },
                                { 0 },
                            },
                },
                {
                            { ASM,  80, ASM, END },
                            { ASM, 226, ASM, END },
                            { 127, 226, 81, -1 },
                            {
                                { 2, 226, 1 },
                                { 1,  80, 0 },
                                { 0 },
                            },
                },
                {
                            {  80, ASM, END },
                            { ASM, 180, END },
                            { 250, 81, 181, -1 },
                            {
                                { 2, 80, 0 },
                                { 1, 180, 1 },
                                { 0 },
                            },
                },
                {
                            { 2, END },
                            { END },
                            { 1, 3, -1 },
                            {
                                { 1, 2, 0 },
                                { 0 },
                            },
                },
                {
                            { ASM, END },
                            { 129, ASM, END },
                            { 127, 130, -1 },
                            {
                                { 1, 129, 1 },
                                { 0 },
                            },
                },
                {
                            { ASM, 1, END },
                            { END },
                            { 255, 2, -1 },
                            {
                                { 1, 1, 0 },
                                { 0 },
                            },
                },
                {
                            { 4, 5, ASM, 6, END },
                            { END },
                            { 4, 5, 6, 7, -1 },
                            {
                                { 1, 4, 0 },
                                { 1, 5, 0 },
                                { 1, 6, 0 },
                                { 0 },
                            },
                },
                {
                            { END },
                            { 126, 127, 128, END },
                            { 126, 127, 128, 129, -1 },
                            {
                                { 1, 126, 1 },
                                { 1, 127, 1 },
                                { 1, 128, 1 },
                                { 0 },
                            },
                },
                {
                            { 254, 255, ASM, 0, 1, END },
                            { ASM, END },
                            { 254, 255, 0, 1, 2, -1 },
                            {
                                { 1, 254, 0 },
                                { 1, 255, 0 },
                                { 1,   0, 0 },
                                { 1,   1, 0 },
                                { 0 },
                            },
                },
};

struct {
    char *testname;
    int queue0[100];
    int queue1[100];
    int expectedsid;

    struct {
        int expectedsid;
        void *timer;
        int asmcellcount;
    } expectedoutput;

} ut_rx_process_queues_READY_tests[] = {
                {
                    "1A",
                            { END },
                            { END },
                            0,
                            {
                                0, NULL, 0,
                            },
                },
                {
                    "1B",
                            { END },
                            { ASM, END },
                            128,
                            {
                                128, NULL, 1,
                            },
                },
                {
                    "1C",
                            { ASM, END },
                            { ASM, END },
                            255,
                            {
                                255, NULL, 2,
                            },
                },
                {
                    "2A",
                            { 2, END },
                            { END },
                            2,
                            {
                                3, NULL, 0,
                            },
                },
                {
                    "2B",
                            { END },
                            { 128, ASM, END },
                            128,
                            {
                                129, NULL, 1,
                            },
                },
                {
                    "2C",
                            { ASM, 255, END },
                            { ASM, END },
                            255,
                            {
                                0, NULL, 2,
                            },
                },
                {
                    "3A",
                            { 200, END },
                            { ASM, END },
                            1,
                            {
                                1, NULL, 1,
                            },
                },
                {
                    "3B",
                            { END },
                            { 5, END },
                            128,
                            {
                                128, NULL, 0,
                            },
                },
                {
                    "3C",
                            { ASM, 250, ASM, END },
                            { ASM, END },
                            255,
                            {
                                255, NULL, 3,
                            },
                },
                {
                    "4A",
                            { 2, END },
                            { END },
                            1,
                            {
                                1, (void *)1, 0,
                            },
                },
                {
                    "4B",
                            { ASM, END },
                            { 129, ASM, END },
                            127,
                            {
                                127, (void *)1, 1,
                            },
                },
                {
                    "4C",
                            { ASM, 1, END },
                            { END },
                            255,
                            {
                                255, (void *)1, 1,
                            },
                },
                {
                    "5A",
                            { 4, 5, ASM, 6, END },
                            { END },
                            4,
                            {
                                7, NULL, 1,
                            },
                },
                {
                    "5B",
                            { END },
                            { 126, 127, 128, END },
                            126,
                            {
                                129, NULL, 0,
                            },
                },
                {
                    "5C",
                            { 254, 255, ASM, 0, 1, END },
                            { ASM, END },
                            254,
                            {
                                2, NULL, 2,
                            },
                },
                {
                    "6A",
                            { 4, 5, ASM, 8, END },
                            { ASM, END },
                            4,
                            {
                                6, (void *)1, 2,
                            },
                },
                {
                    "6B",
                            { END },
                            { 126, 127, 130, ASM, END },
                            126,
                            {
                                128, (void *)1, 0,
                            },
                },
                {
                    "6C",
                            { 254, 255, 2, 3, END },
                            { END },
                            254,
                            {
                                0, (void *)1, 0,
                            },
                },
                {
                    "7A",
                            { 1, 3, 4, 6, END },
                            { 2, 5, END },
                            1,
                            {
                                7, NULL, 0,
                            },
                },
                {
                    "7B",
                            { 126, 129, ASM, END },
                            { 125, ASM, 127, 128, 130, END },
                            125,
                            {
                                131, NULL, 2,
                            },
                },
                {
                    "7C",
                            { ASM, 254, 0, 1, 2, END },
                            { ASM, 253, 255, END },
                            253,
                            {
                                3, NULL, 2,
                            },
                },
                {
                    "8A",
                            { 20, END },
                            { 10, END },
                            2,
                            {
                                11, (void *)1, 0,
                            },
                },
                {
                    "8B",
                            { ASM, 130, END },
                            { 150, END },
                            123,
                            {
                                131, (void *)1, 1,
                            },
                },
                {
                    "8C",
                            { 20, END },
                            { 6, END },
                            253,
                            {
                                7, (void *)1, 0,
                            },
                },
                {
                    "9A",
                            { ASM, 20, END },
                            { 1, END },
                            2,
                            {
                                21, NULL, 1,
                            },
                },
                {
                    "9B",
                            { 122, END },
                            { 150, END },
                            123,
                            {
                                151, NULL, 0,
                            },
                },
                {
                    "9C",
                            {  20, ASM, END },
                            { 252, ASM, END },
                            253,
                            {
                                21, NULL, 2,
                            },
                },
                {
                    "10A",
                            { 100, END },
                            { 200, END },
                            1,
                            {
                                101, (void *)1, 0,
                            },
                },
                {
                    "10B",
                            { ASM,  80, ASM, END },
                            { ASM, 226, ASM, END },
                            127,
                            {
                                227, (void *)1, 3,
                            },
                },
                {
                    "10C",
                            {  80, ASM, END },
                            { ASM, 180, END },
                            250,
                            {
                                81, (void *)1, 2,
                            },
                },
};

struct {
    char *testname;
    int queue0[100];
    int queue1[100];

    struct {
        RxBondingState_t state;
        int expectedsid;
        void *timer;
        int asmcellcount;
    } expectedoutput;

} ut_rx_process_queues_LEARNING_tests[] = {
                {
                    "18A",
                            { 2, END },
                            { END },
                            {
                                LEARNING, 0, (void *)1, 0,
                            },
                },
                {
                    "18B",
                            { END },
                            { 128, ASM, END },
                            {
                                LEARNING, 0, (void *)1, 0,
                            },
                },
                {
                    "18C",
                            { ASM, 255, END },
                            { ASM, END },
                            {
                                LEARNING, 0, (void *)1, 2,
                            },
                },
                {
                    "19A",
                            { 20, END },
                            { 10, END },
                            {
                                READY, 11, (void *)1, 0,
                            },
                },
                {
                    "19B",
                            { ASM, 130, END },
                            { 150, END },
                            {
                                READY, 131, (void *)1, 1,
                            },
                },
                {
                    "19C",
                            { 255, END },
                            { 250, END },
                            {
                                READY, 251, (void *)1, 0,
                            },
                },
};

struct {
    char *testname;
    int queue0[100];
    int queue1[100];
    int expectedsid;

    struct {
        int expectedsid;
        void *timer;
        int asmcellcount;
    } expectedoutput;

} ut_rx_timeout_handler_READY_tests[] = {
                {
                    "11A",
                            { 2, END },
                            { END },
                            1,
                            {
                                3, NULL, 0,
                            },
                },
                {
                    "11B",
                            { ASM, END },
                            { 129, ASM, END },
                            127,
                            {
                                130, NULL, 2,
                            },
                },
                {
                    "11C",
                            { ASM, 1, END },
                            { END },
                            255,
                            {
                                2, NULL, 1,
                            },
                },
                {
                    "12A",
                            { 4, 5, ASM, 6, END },
                            { END },
                            4,
                            {
                                7, NULL, 1,
                            },
                },
                {
                    "12B",
                            { END },
                            { 126, 127, 128, END },
                            126,
                            {
                                129, NULL, 0,
                            },
                },
                {
                    "12C",
                            { 254, 255, ASM, 0, 1, END },
                            { ASM, END },
                            254,
                            {
                                2, NULL, 2,
                            },
                },
                {
                    "13A",
                            { 4, 5, ASM, 8, END },
                            { ASM, END },
                            4,
                            {
                                6, (void *)1, 2,
                            },
                },
                {
                    "13B",
                            { END },
                            { 126, 127, 130, ASM, END },
                            126,
                            {
                                128, (void *)1, 0,
                            },
                },
                {
                    "13C",
                            { 254, 255, 2, 3, END },
                            { END },
                            254,
                            {
                                0, (void *)1, 0,
                            },
                },
                {
                    "14A",
                            { 1, 3, 4, 6, END },
                            { 2, 5, END },
                            1,
                            {
                                7, NULL, 0,
                            },
                },
                {
                    "14B",
                            { 126, 129, ASM, END },
                            { 125, ASM, 127, 128, 130, END },
                            125,
                            {
                                131, NULL, 2,
                            },
                },
                {
                    "14C",
                            { ASM, 254, 0, 1, 2, END },
                            { ASM, 253, 255, END },
                            253,
                            {
                                3, NULL, 2,
                            },
                },
                {
                    "15A",
                            { 20, END },
                            { 10, END },
                            2,
                            {
                                11, (void *)1, 0,
                            },
                },
                {
                    "15B",
                            { ASM, 130, END },
                            { 150, END },
                            123,
                            {
                                131, (void *)1, 1,
                            },
                },
                {
                    "15C",
                            { 20, END },
                            { 6, END },
                            253,
                            {
                                7, (void *)1, 0,
                            },
                },
                {
                    "16A",
                            { ASM, 20, END },
                            { 1, END },
                            2,
                            {
                                21, NULL, 1,
                            },
                },
                {
                    "16B",
                            { 122, END },
                            { 150, END },
                            123,
                            {
                                151, NULL, 0,
                            },
                },
                {
                    "16C",
                            { 20, ASM, END },
                            { 252, ASM, END },
                            253,
                            {
                                21, NULL, 2,
                            },
                },
                {
                    "17A",
                            { 100, END },
                            { 200, END },
                            1,
                            {
                                101, (void *)1, 0,
                            },
                },
                {
                    "17B",
                            { ASM,  80, ASM, END },
                            { ASM, 226, ASM, END },
                            127,
                            {
                                227, (void *)1, 3,
                            },
                },
                {
                    "17C",
                            {  80, ASM, END },
                            { ASM, 180, END },
                            250,
                            {
                                81, (void *)1, 2,
                            },
                },
};

struct {
    char *testname;
    int queue0[100];
    int queue1[100];

    struct {
        RxBondingState_t state;
        int expectedsid;
        void *timer;
        int asmcellcount;
    } expectedoutput;

} ut_rx_timeout_handler_LEARNING_tests[] = {
                {
                    "20A",
                            { 2, END },
                            { END },
                            {
                                READY, 3, NULL, 0,
                            },
                },
                {
                    "20B",
                            { END },
                            { 128, ASM, END },
                            {
                                READY, 129, NULL, 1,
                            },
                },
                {
                    "20C",
                            { ASM, 255, END },
                            { ASM, END },
                            {
                                READY, 0, NULL, 2,
                            },
                },
                {
                    "21A",
                            { 20, END },
                            { 10, END },
                            {
                                READY, 11, (void *)1, 0,
                            },
                },
                {
                    "21B",
                            { ASM, 130, END },
                            { 150, END },
                            {
                                READY, 131, (void *)1, 1,
                            },
                },
                {
                    "21C",
                            { 255, END },
                            { 250, END },
                            {
                                READY, 251, (void *)1, 0,
                            },
                },
};

#ifdef UNIT_TESTS_3
struct {
    unsigned char cell[100];

    struct {
        int vpi, vci, sid, pt3, pt2, pt1, clp;
    } expectedoutput;

} ut_read_atmcell_tests[] = {
                {
                    {
                        0x21,   // SID =      1111 1111 = 0x2FF = 255 [8-bit format, so ignore sid12_hi !!!]
                        0x1F,   // VPI =      0001 0001 =  0x11 =  17
                        0xF3,   // VCI =      0011 0100 =  0x34 =  52
                        0x42,   //  PT = 1 CLP = 0 0010 =  0x02 =   2
                        0x5D,   // HEX =      0101 1101 =  0x5D =  93
                        0x00, 0x00, 0x00, // Padding
                        0xAD, 0xDA, 0x12, 0xBE, 0xEF, 0x34,
                        0xAD, 0xDA, 0x56, 0xBE, 0xEF, 0x78,
                        0xAD, 0xDA, 0x87, 0xBE, 0xEF, 0x65,
                        0xAD, 0xDA, 0x43, 0xBE, 0xEF, 0x21,
                        0xAD, 0xDA, 0x12, 0xBE, 0xEF, 0x34,
                        0xAD, 0xDA, 0x56, 0xBE, 0xEF, 0x78,
                        0xAD, 0xDA, 0x00, 0xBE, 0xEF, 0x00,
                    },
                        { 17, 52, 255, 0, 0, 1, 0 },
                },
};

struct {

    int vpi, vci, sid, pt3, pt2, pt1, clp, hec;

    struct {
        unsigned char cell[8];
    } expectedoutput;

} ut_write_atmcell_tests[] = {
                {
                    0, 20, 0, 0, 0, 1, 0, 0x22,
                    {
                        {
                            0x00, // upper-nibble(SID) + upper-nibble(VPI)
                            0x00, // lower-nibble(VPI) + middle-nibble(SID)
                            0x01, // lowest-nibble(SID) + upper-nibble(VCI)
                            0x42, // lower-nibble(VCI) + pt.3 + pt.2 + pt.1 + clp
                            0x22, // HEC
                            0x00, 0x00, 0x00,
                        }
                    },
                },
};
#endif

#if defined UNIT_TESTS_4 || defined UNIT_TESTS_5
struct {
    char *purpose;
    unsigned char cells[30][56];

    struct {
        unsigned int createdframecount;
        unsigned int copiedcellcount;
        unsigned int acceptedframecount;
        unsigned int discardedframecount;
    } expectedoutput;

} ut_reassemble_aal5_tests[] = {
                {
                    .purpose = "Accepts the first cell, creates and appends to frame, keeps waiting for more",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 1, 0, 0 }
                },
                {
                    .purpose = "In continuation to earlier test, accepts the second cell, appends to frame, keeps waiting for more",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=6, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x60, 0x00,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 2, 0, 0 }
                },
                {
                    .purpose = "In continuation to earlier test, accepts the third cell which has PT1 flag set, appends to frame, passes frame "
                               "to TC driver. The size of this frame is 96, i.e. last cell has only padding",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=6, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x60, 0x00,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=7, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x00, 0x70, 0x02,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,   96, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 3, 1, 0 }
                },
                {
                    .purpose = "In continuation to earlier test, accepts the third cell which has PT1 flag set, appends to frame, passes frame "
                               "to TC driver. The size of this frame is 90, i.e. padding starts from 2nd cell, 3rd cell is only padding",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=6, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x60, 0x00,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=7, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x00, 0x70, 0x02,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,   90, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 3, 1, 0 }
                },
                {
                    .purpose = "In continuation to earlier tests, accepts the third cell which has PT1 flag set, appends to frame, passes frame "
                               "to TC driver. The size of this frame is 136, i.e. last cell has no padding",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=6, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x60, 0x00,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=7, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x00, 0x70, 0x02,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  136, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 3, 1, 0 }
                },
                {
                    .purpose = "To check whether reassembler can reassemble two frames in sequence", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=5, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x50, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=6, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x60, 0x00,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=7, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x00, 0x70, 0x02,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  136, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=8, .vci_hi=4, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x84, 0x00,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=9, .vci_hi=4, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x94, 0x02,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 5, 2, 0 }
                },
                {
                    .purpose = "To check whether reassembler reassembles frame made of only one cell",
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD0, 0x02,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    0,    0,    0,   40, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 1, 1, 1, 0 }
                },
                {
                    .purpose = "To check whether reassembler drops the first frame because the second cell has CLP flag set", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD0, 0x00,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=E, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=1, .hec=1
                                  { 0x00, 0x0F, 0xE0, 0x01,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=F, .vci_hi=0, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x0F, 0xF0, 0x02,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  136, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=0, .vci_hi=4, .vci_lo=0, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x04, 0x00,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=1, .vci_hi=4, .vci_lo=0, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x14, 0x02,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 3, 1, 1 }
                },
                {
                    .purpose = "To check whether reassembler drops the first frame because the second cell has PT2 flag set", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD2, 0x20,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=E, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=1, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xE2, 0x24,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=F, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x0F, 0xF2, 0x22,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  136, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=0, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x04, 0x20,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=1, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x14, 0x22,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 3, 1, 1 }
                },
                {
                    .purpose = "To check whether reassembler drops the first frame because the Length in its trailer is too large", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD2, 0x20,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=E, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xE2, 0x20,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=F, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x0F, 0xF2, 0x22,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  200, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=0, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x04, 0x20,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=1, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x14, 0x22,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 5, 1, 1 }
                },
                {
                    .purpose = "To check whether reassembler drops the first frame because the Length in its trailer is too small", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD2, 0x20,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=E, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xE2, 0x20,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=F, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x0F, 0xF2, 0x22,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,   48, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=0, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x04, 0x20,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=1, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x14, 0x22,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 5, 1, 1 }
                },
                {
                    .purpose = "To check whether reassembler drops the first frame because the CRC in its trailer incorrect", 
                    .cells = {
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=D, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xD2, 0x20,    1,    0,    0,    0, 
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                       1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=E, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x0F, 0xE2, 0x20,    1,    0,    0,    0,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                       2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,    2,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=F, .sid8_lo=F, .vci_hi=2, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1,
                                  { 0x00, 0x0F, 0xF2, 0x22,    1,    0,    0,    0,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
                                       3,    3,    3,    3,    3,    3,    3,    3,    0,    0,    0,  100, 0xa1, 0x00, 0xc3, 0xd4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=0, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=0, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x04, 0x20,    1,    0,    0,    0, 
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                       4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,    4,
                                  },
                                  // sid12_hi=0, .vpi_hi=0, .vpi_lo=0, .sid8_hi=0, .sid8_lo=1, .vci_hi=4, .vci_lo=2, .pt3=0, .pt2=0, .pt1=1, .clp=0, .hec=1
                                  { 0x00, 0x00, 0x14, 0x22,    1,    0,    0,    0,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,
                                       5,    5,    5,    5,    5,    5,    5,    5, 0xAA, 0xBB,    0,   80, 0xa1, 0xb2, 0xc3, 0xd4,
                                  },
                                  //.hec=0
                                  { 0x00, 0x00, 0x00, 0x00, 0, },
                             },
                    .expectedoutput = { 2, 5, 1, 1 }
                },
};
#endif
