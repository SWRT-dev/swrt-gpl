/*******************************************************************
*                                                                   
*  MT_MAC_PAC_regs.h
*   Chip Name: 'Hyperion'                                   
*   Do not modify this file directly                                
*   To update the file, modify registers excel file                 
*   and run the macro 'Create H Files for MIPS SW'              
*   Written by: Avri Golan                                          
*                                                                   
*******************************************************************/

/* Production Date: 11/08/2013  */

#ifndef MAC_PAC_regs_H_FILE
#define MAC_PAC_regs_H_FILE

/* BYTE_FILTER_CONTROL_0 */
#define REG_BYTE_FILTER_CONTROL_0          0x0000
#define REG_BYTE_FILTER_CONTROL_0_MASK     0x000000ff
#define REG_BYTE_FILTER_CONTROL_0_SHIFT    0
#define REG_BYTE_FILTER_CONTROL_0_WIDTH    8

/* MASK_FILTER_CONTROL_0 */
#define REG_MASK_FILTER_CONTROL_0          0x0000
#define REG_MASK_FILTER_CONTROL_0_MASK     0x0000ff00
#define REG_MASK_FILTER_CONTROL_0_SHIFT    8
#define REG_MASK_FILTER_CONTROL_0_WIDTH    8

/* OFFSET_FILTER_CONTROL_0 */
#define REG_OFFSET_FILTER_CONTROL_0          0x0000
#define REG_OFFSET_FILTER_CONTROL_0_MASK     0x007f0000
#define REG_OFFSET_FILTER_CONTROL_0_SHIFT    16
#define REG_OFFSET_FILTER_CONTROL_0_WIDTH    7

/* OFFSET_WRAP_FILTER_CONTROL_0 */
#define REG_OFFSET_WRAP_FILTER_CONTROL_0          0x0000
#define REG_OFFSET_WRAP_FILTER_CONTROL_0_MASK     0x7f000000
#define REG_OFFSET_WRAP_FILTER_CONTROL_0_SHIFT    24
#define REG_OFFSET_WRAP_FILTER_CONTROL_0_WIDTH    7

/* BYTE_FILTER_CONTROL_1 */
#define REG_BYTE_FILTER_CONTROL_1          0x0004
#define REG_BYTE_FILTER_CONTROL_1_MASK     0x000000ff
#define REG_BYTE_FILTER_CONTROL_1_SHIFT    0
#define REG_BYTE_FILTER_CONTROL_1_WIDTH    8

/* MASKFILTER_CONTROL_1 */
#define REG_MASKFILTER_CONTROL_1          0x0004
#define REG_MASKFILTER_CONTROL_1_MASK     0x0000ff00
#define REG_MASKFILTER_CONTROL_1_SHIFT    8
#define REG_MASKFILTER_CONTROL_1_WIDTH    8

/* OFFSET_FILTER_CONTROL_1 */
#define REG_OFFSET_FILTER_CONTROL_1          0x0004
#define REG_OFFSET_FILTER_CONTROL_1_MASK     0x007f0000
#define REG_OFFSET_FILTER_CONTROL_1_SHIFT    16
#define REG_OFFSET_FILTER_CONTROL_1_WIDTH    7

/* OFFSET_WRAP_FILTER_CONTROL_1 */
#define REG_OFFSET_WRAP_FILTER_CONTROL_1          0x0004
#define REG_OFFSET_WRAP_FILTER_CONTROL_1_MASK     0x7f000000
#define REG_OFFSET_WRAP_FILTER_CONTROL_1_SHIFT    24
#define REG_OFFSET_WRAP_FILTER_CONTROL_1_WIDTH    7

/* BYTE_FILTER_CONTROL_2 */
#define REG_BYTE_FILTER_CONTROL_2          0x0008
#define REG_BYTE_FILTER_CONTROL_2_MASK     0x000000ff
#define REG_BYTE_FILTER_CONTROL_2_SHIFT    0
#define REG_BYTE_FILTER_CONTROL_2_WIDTH    8

/* MASK_FILTER_CONTROL_2 */
#define REG_MASK_FILTER_CONTROL_2          0x0008
#define REG_MASK_FILTER_CONTROL_2_MASK     0x0000ff00
#define REG_MASK_FILTER_CONTROL_2_SHIFT    8
#define REG_MASK_FILTER_CONTROL_2_WIDTH    8

/* OFFSET_FILTER_CONTROL_2 */
#define REG_OFFSET_FILTER_CONTROL_2          0x0008
#define REG_OFFSET_FILTER_CONTROL_2_MASK     0x007f0000
#define REG_OFFSET_FILTER_CONTROL_2_SHIFT    16
#define REG_OFFSET_FILTER_CONTROL_2_WIDTH    7

/* OFFSET_WRAP_FILTER_CONTROL_2 */
#define REG_OFFSET_WRAP_FILTER_CONTROL_2          0x0008
#define REG_OFFSET_WRAP_FILTER_CONTROL_2_MASK     0x7f000000
#define REG_OFFSET_WRAP_FILTER_CONTROL_2_SHIFT    24
#define REG_OFFSET_WRAP_FILTER_CONTROL_2_WIDTH    7

/* SF_MATCH_VECTOR */
#define REG_SF_MATCH_VECTOR          0x000c
#define REG_SF_MATCH_VECTOR_MASK     0x000007ff
#define REG_SF_MATCH_VECTOR_SHIFT    0
#define REG_SF_MATCH_VECTOR_WIDTH    11

/* SEL_LF_0 */
#define REG_SEL_LF_0          0x0010
#define REG_SEL_LF_0_MASK     0x00000003
#define REG_SEL_LF_0_SHIFT    0
#define REG_SEL_LF_0_WIDTH    2

/* SEL_LF_1 */
#define REG_SEL_LF_1          0x0010
#define REG_SEL_LF_1_MASK     0x0000000c
#define REG_SEL_LF_1_SHIFT    2
#define REG_SEL_LF_1_WIDTH    2

/* SEL_LF_2 */
#define REG_SEL_LF_2          0x0010
#define REG_SEL_LF_2_MASK     0x00000030
#define REG_SEL_LF_2_SHIFT    4
#define REG_SEL_LF_2_WIDTH    2

/* SEL_LF_3 */
#define REG_SEL_LF_3          0x0010
#define REG_SEL_LF_3_MASK     0x000000c0
#define REG_SEL_LF_3_SHIFT    6
#define REG_SEL_LF_3_WIDTH    2

/* SEL_LF_4 */
#define REG_SEL_LF_4          0x0010
#define REG_SEL_LF_4_MASK     0x00000300
#define REG_SEL_LF_4_SHIFT    8
#define REG_SEL_LF_4_WIDTH    2

/* SEL_LF_5 */
#define REG_SEL_LF_5          0x0010
#define REG_SEL_LF_5_MASK     0x00000c00
#define REG_SEL_LF_5_SHIFT    10
#define REG_SEL_LF_5_WIDTH    2

/* SEL_LF_6 */
#define REG_SEL_LF_6          0x0010
#define REG_SEL_LF_6_MASK     0x00003000
#define REG_SEL_LF_6_SHIFT    12
#define REG_SEL_LF_6_WIDTH    2

/* SEL_LF_7 */
#define REG_SEL_LF_7          0x0010
#define REG_SEL_LF_7_MASK     0x0000c000
#define REG_SEL_LF_7_SHIFT    14
#define REG_SEL_LF_7_WIDTH    2

/* SEL_LF_8 */
#define REG_SEL_LF_8          0x0010
#define REG_SEL_LF_8_MASK     0x00030000
#define REG_SEL_LF_8_SHIFT    16
#define REG_SEL_LF_8_WIDTH    2

/* SEL_LF_9 */
#define REG_SEL_LF_9          0x0010
#define REG_SEL_LF_9_MASK     0x000c0000
#define REG_SEL_LF_9_SHIFT    18
#define REG_SEL_LF_9_WIDTH    2

/* SEL_LF_10 */
#define REG_SEL_LF_10          0x0010
#define REG_SEL_LF_10_MASK     0x00300000
#define REG_SEL_LF_10_SHIFT    20
#define REG_SEL_LF_10_WIDTH    2

/* SEL_LF_11 */
#define REG_SEL_LF_11          0x0010
#define REG_SEL_LF_11_MASK     0x00c00000
#define REG_SEL_LF_11_SHIFT    22
#define REG_SEL_LF_11_WIDTH    2

/* SEL_LF_12 */
#define REG_SEL_LF_12          0x0010
#define REG_SEL_LF_12_MASK     0x03000000
#define REG_SEL_LF_12_SHIFT    24
#define REG_SEL_LF_12_WIDTH    2

/* SEL_LF_13 */
#define REG_SEL_LF_13          0x0010
#define REG_SEL_LF_13_MASK     0x0c000000
#define REG_SEL_LF_13_SHIFT    26
#define REG_SEL_LF_13_WIDTH    2

/* SEL_LF_14 */
#define REG_SEL_LF_14          0x0010
#define REG_SEL_LF_14_MASK     0x30000000
#define REG_SEL_LF_14_SHIFT    28
#define REG_SEL_LF_14_WIDTH    2

/* SEL_LF_15 */
#define REG_SEL_LF_15          0x0010
#define REG_SEL_LF_15_MASK     0xc0000000
#define REG_SEL_LF_15_SHIFT    30
#define REG_SEL_LF_15_WIDTH    2

/* SEL_LF_16 */
#define REG_SEL_LF_16          0x0014
#define REG_SEL_LF_16_MASK     0x00000003
#define REG_SEL_LF_16_SHIFT    0
#define REG_SEL_LF_16_WIDTH    2

/* SEL_LF_17 */
#define REG_SEL_LF_17          0x0014
#define REG_SEL_LF_17_MASK     0x0000000c
#define REG_SEL_LF_17_SHIFT    2
#define REG_SEL_LF_17_WIDTH    2

/* LONG_FILTER_0_CONTROL */
#define REG_LONG_FILTER_0_CONTROL          0x0018
#define REG_LONG_FILTER_0_CONTROL_MASK     0x0000003f
#define REG_LONG_FILTER_0_CONTROL_SHIFT    0
#define REG_LONG_FILTER_0_CONTROL_WIDTH    6

/* OFFSET_LONG_FILTER_0_CONTROL */
#define REG_OFFSET_LONG_FILTER_0_CONTROL          0x0018
#define REG_OFFSET_LONG_FILTER_0_CONTROL_MASK     0x00007f00
#define REG_OFFSET_LONG_FILTER_0_CONTROL_SHIFT    8
#define REG_OFFSET_LONG_FILTER_0_CONTROL_WIDTH    7

/* LONG_FILTER_1_CONTROL */
#define REG_LONG_FILTER_1_CONTROL          0x001c
#define REG_LONG_FILTER_1_CONTROL_MASK     0x0000003f
#define REG_LONG_FILTER_1_CONTROL_SHIFT    0
#define REG_LONG_FILTER_1_CONTROL_WIDTH    6

/* OFFSET_LONG_FILTER_1_CONTROL */
#define REG_OFFSET_LONG_FILTER_1_CONTROL          0x001c
#define REG_OFFSET_LONG_FILTER_1_CONTROL_MASK     0x00007f00
#define REG_OFFSET_LONG_FILTER_1_CONTROL_SHIFT    8
#define REG_OFFSET_LONG_FILTER_1_CONTROL_WIDTH    7

/* LONG_FILTER_2_CONTROL */
#define REG_LONG_FILTER_2_CONTROL          0x0020
#define REG_LONG_FILTER_2_CONTROL_MASK     0x0000003f
#define REG_LONG_FILTER_2_CONTROL_SHIFT    0
#define REG_LONG_FILTER_2_CONTROL_WIDTH    6

/* OFFSET_LONG_FILTER_2_CONTROL */
#define REG_OFFSET_LONG_FILTER_2_CONTROL          0x0020
#define REG_OFFSET_LONG_FILTER_2_CONTROL_MASK     0x00007f00
#define REG_OFFSET_LONG_FILTER_2_CONTROL_SHIFT    8
#define REG_OFFSET_LONG_FILTER_2_CONTROL_WIDTH    7

/* LONG_FILTER_0_REF1 */
#define REG_LONG_FILTER_0_REF1          0x0024
#define REG_LONG_FILTER_0_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_0_REF1_SHIFT    0
#define REG_LONG_FILTER_0_REF1_WIDTH    32

/* LONG_FILTER_0_REF2 */
#define REG_LONG_FILTER_0_REF2          0x0028
#define REG_LONG_FILTER_0_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_0_REF2_SHIFT    0
#define REG_LONG_FILTER_0_REF2_WIDTH    16

/* LONG_FILTER_1_REF1 */
#define REG_LONG_FILTER_1_REF1          0x002c
#define REG_LONG_FILTER_1_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_1_REF1_SHIFT    0
#define REG_LONG_FILTER_1_REF1_WIDTH    32

/* LONG_FILTER_1_REF2 */
#define REG_LONG_FILTER_1_REF2          0x0030
#define REG_LONG_FILTER_1_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_1_REF2_SHIFT    0
#define REG_LONG_FILTER_1_REF2_WIDTH    16

/* LONG_FILTER_2_REF1 */
#define REG_LONG_FILTER_2_REF1          0x0034
#define REG_LONG_FILTER_2_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_2_REF1_SHIFT    0
#define REG_LONG_FILTER_2_REF1_WIDTH    32

/* LONG_FILTER_2_REF2 */
#define REG_LONG_FILTER_2_REF2          0x0038
#define REG_LONG_FILTER_2_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_2_REF2_SHIFT    0
#define REG_LONG_FILTER_2_REF2_WIDTH    16

/* LONG_FILTER_3_REF1 */
#define REG_LONG_FILTER_3_REF1          0x003c
#define REG_LONG_FILTER_3_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_3_REF1_SHIFT    0
#define REG_LONG_FILTER_3_REF1_WIDTH    32

/* LONG_FILTER_3_REF2 */
#define REG_LONG_FILTER_3_REF2          0x0040
#define REG_LONG_FILTER_3_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_3_REF2_SHIFT    0
#define REG_LONG_FILTER_3_REF2_WIDTH    16

/* LONG_FILTER_4_REF1 */
#define REG_LONG_FILTER_4_REF1          0x0044
#define REG_LONG_FILTER_4_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_4_REF1_SHIFT    0
#define REG_LONG_FILTER_4_REF1_WIDTH    32

/* LONG_FILTER_4_REF2 */
#define REG_LONG_FILTER_4_REF2          0x0048
#define REG_LONG_FILTER_4_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_4_REF2_SHIFT    0
#define REG_LONG_FILTER_4_REF2_WIDTH    16

/* LONG_FILTER_5_REF1 */
#define REG_LONG_FILTER_5_REF1          0x004c
#define REG_LONG_FILTER_5_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_5_REF1_SHIFT    0
#define REG_LONG_FILTER_5_REF1_WIDTH    32

/* LONG_FILTER_5_REF2 */
#define REG_LONG_FILTER_5_REF2          0x0050
#define REG_LONG_FILTER_5_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_5_REF2_SHIFT    0
#define REG_LONG_FILTER_5_REF2_WIDTH    16

/* LONG_FILTER_6_REF1 */
#define REG_LONG_FILTER_6_REF1          0x0054
#define REG_LONG_FILTER_6_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_6_REF1_SHIFT    0
#define REG_LONG_FILTER_6_REF1_WIDTH    32

/* LONG_FILTER_6_REF2 */
#define REG_LONG_FILTER_6_REF2          0x0058
#define REG_LONG_FILTER_6_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_6_REF2_SHIFT    0
#define REG_LONG_FILTER_6_REF2_WIDTH    16

/* LONG_FILTER_7_REF1 */
#define REG_LONG_FILTER_7_REF1          0x005c
#define REG_LONG_FILTER_7_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_7_REF1_SHIFT    0
#define REG_LONG_FILTER_7_REF1_WIDTH    32

/* LONG_FILTER_7_REF2 */
#define REG_LONG_FILTER_7_REF2          0x0060
#define REG_LONG_FILTER_7_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_7_REF2_SHIFT    0
#define REG_LONG_FILTER_7_REF2_WIDTH    16

/* LONG_FILTER_8_REF1 */
#define REG_LONG_FILTER_8_REF1          0x0064
#define REG_LONG_FILTER_8_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_8_REF1_SHIFT    0
#define REG_LONG_FILTER_8_REF1_WIDTH    32

/* LONG_FILTER_8_REF2 */
#define REG_LONG_FILTER_8_REF2          0x0068
#define REG_LONG_FILTER_8_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_8_REF2_SHIFT    0
#define REG_LONG_FILTER_8_REF2_WIDTH    16

/* LONG_FILTER_9_REF1 */
#define REG_LONG_FILTER_9_REF1          0x006c
#define REG_LONG_FILTER_9_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_9_REF1_SHIFT    0
#define REG_LONG_FILTER_9_REF1_WIDTH    32

/* LONG_FILTER_9_REF2 */
#define REG_LONG_FILTER_9_REF2          0x0070
#define REG_LONG_FILTER_9_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_9_REF2_SHIFT    0
#define REG_LONG_FILTER_9_REF2_WIDTH    16

/* LONG_FILTER_10_REF1 */
#define REG_LONG_FILTER_10_REF1          0x0074
#define REG_LONG_FILTER_10_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_10_REF1_SHIFT    0
#define REG_LONG_FILTER_10_REF1_WIDTH    32

/* LONG_FILTER_10_REF2 */
#define REG_LONG_FILTER_10_REF2          0x0078
#define REG_LONG_FILTER_10_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_10_REF2_SHIFT    0
#define REG_LONG_FILTER_10_REF2_WIDTH    16

/* LONG_FILTER_11_REF1 */
#define REG_LONG_FILTER_11_REF1          0x007c
#define REG_LONG_FILTER_11_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_11_REF1_SHIFT    0
#define REG_LONG_FILTER_11_REF1_WIDTH    32

/* LONG_FILTER_11_REF2 */
#define REG_LONG_FILTER_11_REF2          0x0080
#define REG_LONG_FILTER_11_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_11_REF2_SHIFT    0
#define REG_LONG_FILTER_11_REF2_WIDTH    16

/* LONG_FILTER_12_REF1 */
#define REG_LONG_FILTER_12_REF1          0x0084
#define REG_LONG_FILTER_12_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_12_REF1_SHIFT    0
#define REG_LONG_FILTER_12_REF1_WIDTH    32

/* LONG_FILTER_12_REF2 */
#define REG_LONG_FILTER_12_REF2          0x0088
#define REG_LONG_FILTER_12_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_12_REF2_SHIFT    0
#define REG_LONG_FILTER_12_REF2_WIDTH    16

/* LONG_FILTER_13_REF1 */
#define REG_LONG_FILTER_13_REF1          0x008c
#define REG_LONG_FILTER_13_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_13_REF1_SHIFT    0
#define REG_LONG_FILTER_13_REF1_WIDTH    32

/* LONG_FILTER_13_REF2 */
#define REG_LONG_FILTER_13_REF2          0x0090
#define REG_LONG_FILTER_13_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_13_REF2_SHIFT    0
#define REG_LONG_FILTER_13_REF2_WIDTH    16

/* LONG_FILTER_14_REF1 */
#define REG_LONG_FILTER_14_REF1          0x0094
#define REG_LONG_FILTER_14_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_14_REF1_SHIFT    0
#define REG_LONG_FILTER_14_REF1_WIDTH    32

/* LONG_FILTER_14_REF2 */
#define REG_LONG_FILTER_14_REF2          0x0098
#define REG_LONG_FILTER_14_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_14_REF2_SHIFT    0
#define REG_LONG_FILTER_14_REF2_WIDTH    16

/* LONG_FILTER_15_REF1 */
#define REG_LONG_FILTER_15_REF1          0x009c
#define REG_LONG_FILTER_15_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_15_REF1_SHIFT    0
#define REG_LONG_FILTER_15_REF1_WIDTH    32

/* LONG_FILTER_15_REF2 */
#define REG_LONG_FILTER_15_REF2          0x00a0
#define REG_LONG_FILTER_15_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_15_REF2_SHIFT    0
#define REG_LONG_FILTER_15_REF2_WIDTH    16

/* LONG_FILTER_16_REF1 */
#define REG_LONG_FILTER_16_REF1          0x00a4
#define REG_LONG_FILTER_16_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_16_REF1_SHIFT    0
#define REG_LONG_FILTER_16_REF1_WIDTH    32

/* LONG_FILTER_16_REF2 */
#define REG_LONG_FILTER_16_REF2          0x00a8
#define REG_LONG_FILTER_16_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_16_REF2_SHIFT    0
#define REG_LONG_FILTER_16_REF2_WIDTH    16

/* LONG_FILTER_17_REF1 */
#define REG_LONG_FILTER_17_REF1          0x00ac
#define REG_LONG_FILTER_17_REF1_MASK     0xffffffff
#define REG_LONG_FILTER_17_REF1_SHIFT    0
#define REG_LONG_FILTER_17_REF1_WIDTH    32

/* LONG_FILTER_17_REF2 */
#define REG_LONG_FILTER_17_REF2          0x00b0
#define REG_LONG_FILTER_17_REF2_MASK     0x0000ffff
#define REG_LONG_FILTER_17_REF2_SHIFT    0
#define REG_LONG_FILTER_17_REF2_WIDTH    16

/* LF0_MATCH_VECTOR */
#define REG_LF0_MATCH_VECTOR          0x00b4
#define REG_LF0_MATCH_VECTOR_MASK     0x0003ffff
#define REG_LF0_MATCH_VECTOR_SHIFT    0
#define REG_LF0_MATCH_VECTOR_WIDTH    18

/* LF1_MATCH_VECTOR */
#define REG_LF1_MATCH_VECTOR          0x00b8
#define REG_LF1_MATCH_VECTOR_MASK     0x0003ffff
#define REG_LF1_MATCH_VECTOR_SHIFT    0
#define REG_LF1_MATCH_VECTOR_WIDTH    18

/* LF2_MATCH_VECTOR */
#define REG_LF2_MATCH_VECTOR          0x00bc
#define REG_LF2_MATCH_VECTOR_MASK     0x0003ffff
#define REG_LF2_MATCH_VECTOR_SHIFT    0
#define REG_LF2_MATCH_VECTOR_WIDTH    18

/* WRAPPER_FRAME_ENABLE */
#define REG_WRAPPER_FRAME_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_ENABLE_MASK     0x00000001
#define REG_WRAPPER_FRAME_ENABLE_SHIFT    0
#define REG_WRAPPER_FRAME_ENABLE_WIDTH    1

/* WRAPPER_FRAME_LF_1_ENABLE */
#define REG_WRAPPER_FRAME_LF_1_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_LF_1_ENABLE_MASK     0x00000020
#define REG_WRAPPER_FRAME_LF_1_ENABLE_SHIFT    5
#define REG_WRAPPER_FRAME_LF_1_ENABLE_WIDTH    1

/* WRAPPER_FRAME_LF_2_ENABLE */
#define REG_WRAPPER_FRAME_LF_2_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_LF_2_ENABLE_MASK     0x00000040
#define REG_WRAPPER_FRAME_LF_2_ENABLE_SHIFT    6
#define REG_WRAPPER_FRAME_LF_2_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_0_ENABLE */
#define REG_WRAPPER_FRAME_SF_0_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_0_ENABLE_MASK     0x00000100
#define REG_WRAPPER_FRAME_SF_0_ENABLE_SHIFT    8
#define REG_WRAPPER_FRAME_SF_0_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_1_ENABLE */
#define REG_WRAPPER_FRAME_SF_1_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_1_ENABLE_MASK     0x00000200
#define REG_WRAPPER_FRAME_SF_1_ENABLE_SHIFT    9
#define REG_WRAPPER_FRAME_SF_1_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_2_ENABLE */
#define REG_WRAPPER_FRAME_SF_2_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_2_ENABLE_MASK     0x00000400
#define REG_WRAPPER_FRAME_SF_2_ENABLE_SHIFT    10
#define REG_WRAPPER_FRAME_SF_2_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_3_ENABLE */
#define REG_WRAPPER_FRAME_SF_3_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_3_ENABLE_MASK     0x00000800
#define REG_WRAPPER_FRAME_SF_3_ENABLE_SHIFT    11
#define REG_WRAPPER_FRAME_SF_3_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_4_ENABLE */
#define REG_WRAPPER_FRAME_SF_4_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_4_ENABLE_MASK     0x00001000
#define REG_WRAPPER_FRAME_SF_4_ENABLE_SHIFT    12
#define REG_WRAPPER_FRAME_SF_4_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_5_ENABLE */
#define REG_WRAPPER_FRAME_SF_5_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_5_ENABLE_MASK     0x00002000
#define REG_WRAPPER_FRAME_SF_5_ENABLE_SHIFT    13
#define REG_WRAPPER_FRAME_SF_5_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_6_ENABLE */
#define REG_WRAPPER_FRAME_SF_6_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_6_ENABLE_MASK     0x00004000
#define REG_WRAPPER_FRAME_SF_6_ENABLE_SHIFT    14
#define REG_WRAPPER_FRAME_SF_6_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_7_ENABLE */
#define REG_WRAPPER_FRAME_SF_7_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_7_ENABLE_MASK     0x00008000
#define REG_WRAPPER_FRAME_SF_7_ENABLE_SHIFT    15
#define REG_WRAPPER_FRAME_SF_7_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_8_ENABLE */
#define REG_WRAPPER_FRAME_SF_8_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_8_ENABLE_MASK     0x00010000
#define REG_WRAPPER_FRAME_SF_8_ENABLE_SHIFT    16
#define REG_WRAPPER_FRAME_SF_8_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_9_ENABLE */
#define REG_WRAPPER_FRAME_SF_9_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_9_ENABLE_MASK     0x00020000
#define REG_WRAPPER_FRAME_SF_9_ENABLE_SHIFT    17
#define REG_WRAPPER_FRAME_SF_9_ENABLE_WIDTH    1

/* WRAPPER_FRAME_SF_10_ENABLE */
#define REG_WRAPPER_FRAME_SF_10_ENABLE          0x00c0
#define REG_WRAPPER_FRAME_SF_10_ENABLE_MASK     0x00040000
#define REG_WRAPPER_FRAME_SF_10_ENABLE_SHIFT    18
#define REG_WRAPPER_FRAME_SF_10_ENABLE_WIDTH    1

/* EVENT_ENABLE */
/* Set each bit to enable event generation for the corresponding event */
#define REG_EVENT_ENABLE          0x0100
#define REG_EVENT_ENABLE_MASK     0x3fffffff
#define REG_EVENT_ENABLE_SHIFT    0
#define REG_EVENT_ENABLE_WIDTH    30

/* RX_ENABLE */
/* Set each bit to enable the corresponding receive condition to take part in rx_enable generation.  Bit 24 is for received frames which match none of the conditions. */
#define REG_RX_ENABLE          0x0104
#define REG_RX_ENABLE_MASK     0x7fffffff
#define REG_RX_ENABLE_SHIFT    0
#define REG_RX_ENABLE_WIDTH    31

/* ALL_EVENTS */
/* Each bit indicates that the corresponding event is active. */
#define REG_ALL_EVENTS          0x0108
#define REG_ALL_EVENTS_MASK     0x3fffffff
#define REG_ALL_EVENTS_SHIFT    0
#define REG_ALL_EVENTS_WIDTH    30

/* CRC_ERROR */
/* Generate frame abort on CRC error */
#define REG_CRC_ERROR          0x010c
#define REG_CRC_ERROR_MASK     0x00000004
#define REG_CRC_ERROR_SHIFT    2
#define REG_CRC_ERROR_WIDTH    1

/* PHY_ERROR */
/* Generate frame abort on error signalled by PHY */
#define REG_PHY_ERROR          0x010c
#define REG_PHY_ERROR_MASK     0x00000002
#define REG_PHY_ERROR_SHIFT    1
#define REG_PHY_ERROR_WIDTH    1

/* OVERFLOW_ABORT */
/* Generate frame abort on Input FIFO or Receive Buffer overflow */
#define REG_OVERFLOW_ABORT          0x010c
#define REG_OVERFLOW_ABORT_MASK     0x00000001
#define REG_OVERFLOW_ABORT_SHIFT    0
#define REG_OVERFLOW_ABORT_WIDTH    1

/* CONDITION_0 */
/* Which matrix columns contribute to event 0 */
#define REG_CONDITION_0          0x0110
#define REG_CONDITION_0_MASK     0xffffffff
#define REG_CONDITION_0_SHIFT    0
#define REG_CONDITION_0_WIDTH    32

/* CONDITION_1 */
/* Which matrix columns contribute to event 1 */
#define REG_CONDITION_1          0x0114
#define REG_CONDITION_1_MASK     0xffffffff
#define REG_CONDITION_1_SHIFT    0
#define REG_CONDITION_1_WIDTH    32

/* CONDITION_2 */
/* Which matrix columns contribute to event 2 */
#define REG_CONDITION_2          0x0118
#define REG_CONDITION_2_MASK     0xffffffff
#define REG_CONDITION_2_SHIFT    0
#define REG_CONDITION_2_WIDTH    32

/* CONDITION_3 */
/* Which matrix columns contribute to event 3 */
#define REG_CONDITION_3          0x011c
#define REG_CONDITION_3_MASK     0xffffffff
#define REG_CONDITION_3_SHIFT    0
#define REG_CONDITION_3_WIDTH    32

/* CONDITION_4 */
/* Which matrix columns contribute to event 4 */
#define REG_CONDITION_4          0x0120
#define REG_CONDITION_4_MASK     0xffffffff
#define REG_CONDITION_4_SHIFT    0
#define REG_CONDITION_4_WIDTH    32

/* CONDITION_5 */
/* Which matrix columns contribute to event 5 */
#define REG_CONDITION_5          0x0124
#define REG_CONDITION_5_MASK     0xffffffff
#define REG_CONDITION_5_SHIFT    0
#define REG_CONDITION_5_WIDTH    32

/* CONDITION_6 */
/* Which matrix columns contribute to event 6 */
#define REG_CONDITION_6          0x0128
#define REG_CONDITION_6_MASK     0xffffffff
#define REG_CONDITION_6_SHIFT    0
#define REG_CONDITION_6_WIDTH    32

/* CONDITION_7 */
/* Which matrix columns contribute to event 7 */
#define REG_CONDITION_7          0x012c
#define REG_CONDITION_7_MASK     0xffffffff
#define REG_CONDITION_7_SHIFT    0
#define REG_CONDITION_7_WIDTH    32

/* CONDITION_8 */
/* Which matrix columns contribute to event 8 */
#define REG_CONDITION_8          0x0130
#define REG_CONDITION_8_MASK     0xffffffff
#define REG_CONDITION_8_SHIFT    0
#define REG_CONDITION_8_WIDTH    32

/* CONDITION_9 */
/* Which matrix columns contribute to event 9 */
#define REG_CONDITION_9          0x0134
#define REG_CONDITION_9_MASK     0xffffffff
#define REG_CONDITION_9_SHIFT    0
#define REG_CONDITION_9_WIDTH    32

/* CONDITION_10 */
/* Which matrix columns contribute to event 10 */
#define REG_CONDITION_10          0x0138
#define REG_CONDITION_10_MASK     0xffffffff
#define REG_CONDITION_10_SHIFT    0
#define REG_CONDITION_10_WIDTH    32

/* CONDITION_11 */
/* Which matrix columns contribute to event 11 */
#define REG_CONDITION_11          0x013c
#define REG_CONDITION_11_MASK     0xffffffff
#define REG_CONDITION_11_SHIFT    0
#define REG_CONDITION_11_WIDTH    32

/* CONDITION_12 */
/* Which matrix columns contribute to event 12 */
#define REG_CONDITION_12          0x0140
#define REG_CONDITION_12_MASK     0xffffffff
#define REG_CONDITION_12_SHIFT    0
#define REG_CONDITION_12_WIDTH    32

/* CONDITION_13 */
/* Which matrix columns contribute to event 13 */
#define REG_CONDITION_13          0x0144
#define REG_CONDITION_13_MASK     0xffffffff
#define REG_CONDITION_13_SHIFT    0
#define REG_CONDITION_13_WIDTH    32

/* CONDITION_14 */
/* Which matrix columns contribute to event 14 */
#define REG_CONDITION_14          0x0148
#define REG_CONDITION_14_MASK     0xffffffff
#define REG_CONDITION_14_SHIFT    0
#define REG_CONDITION_14_WIDTH    32

/* CONDITION_15 */
/* Which matrix columns contribute to event 15 */
#define REG_CONDITION_15          0x014c
#define REG_CONDITION_15_MASK     0xffffffff
#define REG_CONDITION_15_SHIFT    0
#define REG_CONDITION_15_WIDTH    32

/* CONDITION_16 */
/* Which matrix columns contribute to event 16 */
#define REG_CONDITION_16          0x0150
#define REG_CONDITION_16_MASK     0xffffffff
#define REG_CONDITION_16_SHIFT    0
#define REG_CONDITION_16_WIDTH    32

/* CONDITION_17 */
/* Which matrix columns contribute to event 17 */
#define REG_CONDITION_17          0x0154
#define REG_CONDITION_17_MASK     0xffffffff
#define REG_CONDITION_17_SHIFT    0
#define REG_CONDITION_17_WIDTH    32

/* CONDITION_18 */
/* Which matrix columns contribute to event 18 */
#define REG_CONDITION_18          0x0158
#define REG_CONDITION_18_MASK     0xffffffff
#define REG_CONDITION_18_SHIFT    0
#define REG_CONDITION_18_WIDTH    32

/* CONDITION_19 */
/* Which matrix columns contribute to event 19 */
#define REG_CONDITION_19          0x015c
#define REG_CONDITION_19_MASK     0xffffffff
#define REG_CONDITION_19_SHIFT    0
#define REG_CONDITION_19_WIDTH    32

/* CONDITION_20 */
/* Which matrix columns contribute to event 20 */
#define REG_CONDITION_20          0x0160
#define REG_CONDITION_20_MASK     0xffffffff
#define REG_CONDITION_20_SHIFT    0
#define REG_CONDITION_20_WIDTH    32

/* CONDITION_21 */
/* Which matrix columns contribute to event 21 */
#define REG_CONDITION_21          0x0164
#define REG_CONDITION_21_MASK     0xffffffff
#define REG_CONDITION_21_SHIFT    0
#define REG_CONDITION_21_WIDTH    32

/* CONDITION_22 */
/* Which matrix columns contribute to event 22 */
#define REG_CONDITION_22          0x0168
#define REG_CONDITION_22_MASK     0xffffffff
#define REG_CONDITION_22_SHIFT    0
#define REG_CONDITION_22_WIDTH    32

/* CONDITION_23 */
/* Which matrix columns contribute to event 23 */
#define REG_CONDITION_23          0x016c
#define REG_CONDITION_23_MASK     0xffffffff
#define REG_CONDITION_23_SHIFT    0
#define REG_CONDITION_23_WIDTH    32

/* CONDITION_24 */
/* Which matrix columns contribute to event 24 */
#define REG_CONDITION_24          0x0170
#define REG_CONDITION_24_MASK     0xffffffff
#define REG_CONDITION_24_SHIFT    0
#define REG_CONDITION_24_WIDTH    32

/* CONDITION_25 */
/* Which matrix columns contribute to event 25 */
#define REG_CONDITION_25          0x0174
#define REG_CONDITION_25_MASK     0xffffffff
#define REG_CONDITION_25_SHIFT    0
#define REG_CONDITION_25_WIDTH    32

/* CONDITION_26 */
/* Which matrix columns contribute to event 26 */
#define REG_CONDITION_26          0x0178
#define REG_CONDITION_26_MASK     0xffffffff
#define REG_CONDITION_26_SHIFT    0
#define REG_CONDITION_26_WIDTH    32

/* CONDITION_27 */
/* Which matrix columns contribute to event 27 */
#define REG_CONDITION_27          0x017c
#define REG_CONDITION_27_MASK     0xffffffff
#define REG_CONDITION_27_SHIFT    0
#define REG_CONDITION_27_WIDTH    32

/* CONDITION_28 */
/* Which matrix columns contribute to event 28 */
#define REG_CONDITION_28          0x0180
#define REG_CONDITION_28_MASK     0xffffffff
#define REG_CONDITION_28_SHIFT    0
#define REG_CONDITION_28_WIDTH    32

/* CONDITION_29 */
/* Which matrix columns contribute to event 29 */
#define REG_CONDITION_29          0x0184
#define REG_CONDITION_29_MASK     0xffffffff
#define REG_CONDITION_29_SHIFT    0
#define REG_CONDITION_29_WIDTH    32

/* CONTROL_CONCAT_27 */
/* Connect "Concat 27" to column 27 to recognize aggregate */
#define REG_CONTROL_CONCAT_27          0x01a4
#define REG_CONTROL_CONCAT_27_MASK     0x00000002
#define REG_CONTROL_CONCAT_27_SHIFT    1
#define REG_CONTROL_CONCAT_27_WIDTH    1

/* CONTROL_NDPA_NDP_TIMER */
/* Verify "ndpa ndp timer" to column 37 to recognize NDP frame */
#define REG_CONTROL_NDPA_NDP_TIMER          0x01a4
#define REG_CONTROL_NDPA_NDP_TIMER_MASK     0x00000004
#define REG_CONTROL_NDPA_NDP_TIMER_SHIFT    2
#define REG_CONTROL_NDPA_NDP_TIMER_WIDTH    1

/* RX_SYNC_EVENT_ENABLE */
#define REG_RX_SYNC_EVENT_ENABLE          0x01a8
#define REG_RX_SYNC_EVENT_ENABLE_MASK     0x3fffffff
#define REG_RX_SYNC_EVENT_ENABLE_SHIFT    0
#define REG_RX_SYNC_EVENT_ENABLE_WIDTH    30

/* TXOP_CONDITION_0 */
/* Which matrix columns contribute to TXOP event 0 */
#define REG_TXOP_CONDITION_0          0x01ac
#define REG_TXOP_CONDITION_0_MASK     0xffffffff
#define REG_TXOP_CONDITION_0_SHIFT    0
#define REG_TXOP_CONDITION_0_WIDTH    32

/* TXOP_CONDITION_1 */
/* Which matrix columns contribute to TXOP event 1 */
#define REG_TXOP_CONDITION_1          0x01b0
#define REG_TXOP_CONDITION_1_MASK     0xffffffff
#define REG_TXOP_CONDITION_1_SHIFT    0
#define REG_TXOP_CONDITION_1_WIDTH    32

/* TXOP_CONDITION_2 */
/* Which matrix columns contribute to TXOP event 2 */
#define REG_TXOP_CONDITION_2          0x01b4
#define REG_TXOP_CONDITION_2_MASK     0xffffffff
#define REG_TXOP_CONDITION_2_SHIFT    0
#define REG_TXOP_CONDITION_2_WIDTH    32

/* TXOP_CONDITION_3 */
/* Which matrix columns contribute to TXOP event 3 */
#define REG_TXOP_CONDITION_3          0x01b8
#define REG_TXOP_CONDITION_3_MASK     0xffffffff
#define REG_TXOP_CONDITION_3_SHIFT    0
#define REG_TXOP_CONDITION_3_WIDTH    32

/* TXOP_CONDITION_4 */
/* Which matrix columns contribute to TXOP event 4 */
#define REG_TXOP_CONDITION_4          0x01bc
#define REG_TXOP_CONDITION_4_MASK     0xffffffff
#define REG_TXOP_CONDITION_4_SHIFT    0
#define REG_TXOP_CONDITION_4_WIDTH    32

/* TXOP_EVENT_ENABLE */
/* Set each bit to enable TXOP event generation for the corresponding event */
#define REG_TXOP_EVENT_ENABLE          0x01c0
#define REG_TXOP_EVENT_ENABLE_MASK     0x0000001f
#define REG_TXOP_EVENT_ENABLE_SHIFT    0
#define REG_TXOP_EVENT_ENABLE_WIDTH    5

/* TXOP_ALL_EVENTS */
/* Each bit indicates that the corresponding TXOP event is active. */
#define REG_TXOP_ALL_EVENTS          0x01c4
#define REG_TXOP_ALL_EVENTS_MASK     0x0000001f
#define REG_TXOP_ALL_EVENTS_SHIFT    0
#define REG_TXOP_ALL_EVENTS_WIDTH    5

/* TXOP_ADDRESS_SELECTOR */
/* Set each bit to select address 1 or 2 for the corresponding event (1'b0 - addr 1, 1'b1 - addr 2) */
#define REG_TXOP_ADDRESS_SELECTOR          0x01c8
#define REG_TXOP_ADDRESS_SELECTOR_MASK     0x0000001f
#define REG_TXOP_ADDRESS_SELECTOR_SHIFT    0
#define REG_TXOP_ADDRESS_SELECTOR_WIDTH    5

/* TXOP_GROUP_BIT_FRORCE2ZERO */
/* Set each bit to force zero on the group bit in the MAC address for the corresponding event */
#define REG_TXOP_GROUP_BIT_FRORCE2ZERO          0x01cc
#define REG_TXOP_GROUP_BIT_FRORCE2ZERO_MASK     0x0000001f
#define REG_TXOP_GROUP_BIT_FRORCE2ZERO_SHIFT    0
#define REG_TXOP_GROUP_BIT_FRORCE2ZERO_WIDTH    5

/* BASE_0 */
#define REG_BASE_0          0x0200
#define REG_BASE_0_MASK     0xffffffff
#define REG_BASE_0_SHIFT    0
#define REG_BASE_0_WIDTH    32

/* BASE_1 */
#define REG_BASE_1          0x0204
#define REG_BASE_1_MASK     0xffffffff
#define REG_BASE_1_SHIFT    0
#define REG_BASE_1_WIDTH    32

/* BASE_2 */
#define REG_BASE_2          0x0208
#define REG_BASE_2_MASK     0xffffffff
#define REG_BASE_2_SHIFT    0
#define REG_BASE_2_WIDTH    32

/* BASE_3 */
#define REG_BASE_3          0x020c
#define REG_BASE_3_MASK     0xffffffff
#define REG_BASE_3_SHIFT    0
#define REG_BASE_3_WIDTH    32

/* BASE_4 */
#define REG_BASE_4          0x0210
#define REG_BASE_4_MASK     0xffffffff
#define REG_BASE_4_SHIFT    0
#define REG_BASE_4_WIDTH    32

/* BASE_5 */
#define REG_BASE_5          0x0214
#define REG_BASE_5_MASK     0xffffffff
#define REG_BASE_5_SHIFT    0
#define REG_BASE_5_WIDTH    32

/* BASE_6 */
#define REG_BASE_6          0x0218
#define REG_BASE_6_MASK     0xffffffff
#define REG_BASE_6_SHIFT    0
#define REG_BASE_6_WIDTH    32

/* RETURN_ADDRESS */
#define REG_RETURN_ADDRESS          0x021c
#define REG_RETURN_ADDRESS_MASK     0x007fffff
#define REG_RETURN_ADDRESS_SHIFT    0
#define REG_RETURN_ADDRESS_WIDTH    23

/* RECIPE_POINTER */
#define REG_RECIPE_POINTER          0x0220
#define REG_RECIPE_POINTER_MASK     0x007fffff
#define REG_RECIPE_POINTER_SHIFT    0
#define REG_RECIPE_POINTER_WIDTH    23

/* DELIA_STATUS */
#define REG_DELIA_STATUS          0x0224
#define REG_DELIA_STATUS_MASK     0x00000001
#define REG_DELIA_STATUS_SHIFT    0
#define REG_DELIA_STATUS_WIDTH    1

/* DELIA_STATE */
#define REG_DELIA_STATE          0x0224
#define REG_DELIA_STATE_MASK     0x000000f0
#define REG_DELIA_STATE_SHIFT    4
#define REG_DELIA_STATE_WIDTH    4

/* IFS_COMPENSATION */
/* Delay from TX_PHY_READY fall to last bit on air */
#define REG_IFS_COMPENSATION          0x0228
#define REG_IFS_COMPENSATION_MASK     0x0000ffff
#define REG_IFS_COMPENSATION_SHIFT    0
#define REG_IFS_COMPENSATION_WIDTH    16

/* REDUCED_FREQ_IFS_COMPENSATION */
/* Delay from TX_PHY_READY fall to last bit on air */
#define REG_REDUCED_FREQ_IFS_COMPENSATION          0x0228
#define REG_REDUCED_FREQ_IFS_COMPENSATION_MASK     0xffff0000
#define REG_REDUCED_FREQ_IFS_COMPENSATION_SHIFT    16
#define REG_REDUCED_FREQ_IFS_COMPENSATION_WIDTH    16

/* CCA_COMPENSATION */
/* Delay from TX_PHY_READY fall to CCA refernce point; must be greater than ifs_compensation */
#define REG_CCA_COMPENSATION          0x022c
#define REG_CCA_COMPENSATION_MASK     0x0000ffff
#define REG_CCA_COMPENSATION_SHIFT    0
#define REG_CCA_COMPENSATION_WIDTH    16

/* REDUCED_FREQ_CCA_COMPENSATION */
/* Delay from TX_PHY_READY fall to CCA refernce point; must be greater than ifs_compensation */
#define REG_REDUCED_FREQ_CCA_COMPENSATION          0x022c
#define REG_REDUCED_FREQ_CCA_COMPENSATION_MASK     0xffff0000
#define REG_REDUCED_FREQ_CCA_COMPENSATION_SHIFT    16
#define REG_REDUCED_FREQ_CCA_COMPENSATION_WIDTH    16

/* PHY_AUTO_START */
#define REG_PHY_AUTO_START          0x0230
#define REG_PHY_AUTO_START_MASK     0x00000001
#define REG_PHY_AUTO_START_SHIFT    0
#define REG_PHY_AUTO_START_WIDTH    1

/* INDEXED_JUMP_ADDR */
#define REG_INDEXED_JUMP_ADDR          0x0234
#define REG_INDEXED_JUMP_ADDR_MASK     0x007ffffc
#define REG_INDEXED_JUMP_ADDR_SHIFT    2
#define REG_INDEXED_JUMP_ADDR_WIDTH    21

/* MP_TX_DATA_WORD_CTR */
#define REG_MP_TX_DATA_WORD_CTR          0x0238
#define REG_MP_TX_DATA_WORD_CTR_MASK     0x0003ffff
#define REG_MP_TX_DATA_WORD_CTR_SHIFT    0
#define REG_MP_TX_DATA_WORD_CTR_WIDTH    18

/* LAST_FETCHED_INSTRUCTION */
#define REG_LAST_FETCHED_INSTRUCTION          0x023c
#define REG_LAST_FETCHED_INSTRUCTION_MASK     0xffffffff
#define REG_LAST_FETCHED_INSTRUCTION_SHIFT    0
#define REG_LAST_FETCHED_INSTRUCTION_WIDTH    32

/* AUTO_POWER_MODE */
#define REG_AUTO_POWER_MODE          0x0240
#define REG_AUTO_POWER_MODE_MASK     0x00000001
#define REG_AUTO_POWER_MODE_SHIFT    0
#define REG_AUTO_POWER_MODE_WIDTH    1

/* TX_ADDR1_31TO0 */
#define REG_TX_ADDR1_31TO0          0x024c
#define REG_TX_ADDR1_31TO0_MASK     0xffffffff
#define REG_TX_ADDR1_31TO0_SHIFT    0
#define REG_TX_ADDR1_31TO0_WIDTH    32

/* TX_ADDR1_47TO32 */
#define REG_TX_ADDR1_47TO32          0x0250
#define REG_TX_ADDR1_47TO32_MASK     0x0000ffff
#define REG_TX_ADDR1_47TO32_SHIFT    0
#define REG_TX_ADDR1_47TO32_WIDTH    16

/* AUTO_FILL_ENABLE */
#define REG_AUTO_FILL_ENABLE          0x0254
#define REG_AUTO_FILL_ENABLE_MASK     0x00000001
#define REG_AUTO_FILL_ENABLE_SHIFT    0
#define REG_AUTO_FILL_ENABLE_WIDTH    1

/* AUTO_FILL_PACKET_LENGTH */
#define REG_AUTO_FILL_PACKET_LENGTH          0x0258
#define REG_AUTO_FILL_PACKET_LENGTH_MASK     0x000fffff
#define REG_AUTO_FILL_PACKET_LENGTH_SHIFT    0
#define REG_AUTO_FILL_PACKET_LENGTH_WIDTH    20

/* CPU_DEL_WR_EVENTS */
#define REG_CPU_DEL_WR_EVENTS          0x025c
#define REG_CPU_DEL_WR_EVENTS_MASK     0x0000003f
#define REG_CPU_DEL_WR_EVENTS_SHIFT    0
#define REG_CPU_DEL_WR_EVENTS_WIDTH    6

/* CPU_DEL_RD_EVENTS */
#define REG_CPU_DEL_RD_EVENTS          0x0260
#define REG_CPU_DEL_RD_EVENTS_MASK     0x0000003f
#define REG_CPU_DEL_RD_EVENTS_SHIFT    0
#define REG_CPU_DEL_RD_EVENTS_WIDTH    6

/* CPU_DEL_WR_CNTR0_EVENT */
#define REG_CPU_DEL_WR_CNTR0_EVENT          0x0264
#define REG_CPU_DEL_WR_CNTR0_EVENT_MASK     0x00000fff
#define REG_CPU_DEL_WR_CNTR0_EVENT_SHIFT    0
#define REG_CPU_DEL_WR_CNTR0_EVENT_WIDTH    12

/* CPU_DEL_RD_CNTR0_EVENT */
#define REG_CPU_DEL_RD_CNTR0_EVENT          0x0268
#define REG_CPU_DEL_RD_CNTR0_EVENT_MASK     0x00000fff
#define REG_CPU_DEL_RD_CNTR0_EVENT_SHIFT    0
#define REG_CPU_DEL_RD_CNTR0_EVENT_WIDTH    12

/* HOB_DEL_RD_CNTR0_EVENT */
#define REG_HOB_DEL_RD_CNTR0_EVENT          0x0268
#define REG_HOB_DEL_RD_CNTR0_EVENT_MASK     0x0fff0000
#define REG_HOB_DEL_RD_CNTR0_EVENT_SHIFT    16
#define REG_HOB_DEL_RD_CNTR0_EVENT_WIDTH    12

/* CPU_DEL_WR_CNTR1_EVENT */
#define REG_CPU_DEL_WR_CNTR1_EVENT          0x026c
#define REG_CPU_DEL_WR_CNTR1_EVENT_MASK     0x00000fff
#define REG_CPU_DEL_WR_CNTR1_EVENT_SHIFT    0
#define REG_CPU_DEL_WR_CNTR1_EVENT_WIDTH    12

/* CPU_DEL_RD_CNTR1_EVENT */
#define REG_CPU_DEL_RD_CNTR1_EVENT          0x0270
#define REG_CPU_DEL_RD_CNTR1_EVENT_MASK     0x00000fff
#define REG_CPU_DEL_RD_CNTR1_EVENT_SHIFT    0
#define REG_CPU_DEL_RD_CNTR1_EVENT_WIDTH    12

/* HOB_DEL_RD_CNTR1_EVENT */
#define REG_HOB_DEL_RD_CNTR1_EVENT          0x0270
#define REG_HOB_DEL_RD_CNTR1_EVENT_MASK     0x0fff0000
#define REG_HOB_DEL_RD_CNTR1_EVENT_SHIFT    16
#define REG_HOB_DEL_RD_CNTR1_EVENT_WIDTH    12

/* LAST_CPU_DEL_CNTR0_EVENT */
#define REG_LAST_CPU_DEL_CNTR0_EVENT          0x0274
#define REG_LAST_CPU_DEL_CNTR0_EVENT_MASK     0x00000fff
#define REG_LAST_CPU_DEL_CNTR0_EVENT_SHIFT    0
#define REG_LAST_CPU_DEL_CNTR0_EVENT_WIDTH    12

/* LAST_CPU_DEL_CNTR1_EVENT */
#define REG_LAST_CPU_DEL_CNTR1_EVENT          0x0274
#define REG_LAST_CPU_DEL_CNTR1_EVENT_MASK     0x00fff000
#define REG_LAST_CPU_DEL_CNTR1_EVENT_SHIFT    12
#define REG_LAST_CPU_DEL_CNTR1_EVENT_WIDTH    12

/* NOP_WAIT_EVENT_VECTOR */
#define REG_NOP_WAIT_EVENT_VECTOR          0x0278
#define REG_NOP_WAIT_EVENT_VECTOR_MASK     0x0000ffff
#define REG_NOP_WAIT_EVENT_VECTOR_SHIFT    0
#define REG_NOP_WAIT_EVENT_VECTOR_WIDTH    16

/* LAST_NOP_WAIT_EVENT_VECTOR */
#define REG_LAST_NOP_WAIT_EVENT_VECTOR          0x0278
#define REG_LAST_NOP_WAIT_EVENT_VECTOR_MASK     0xffff0000
#define REG_LAST_NOP_WAIT_EVENT_VECTOR_SHIFT    16
#define REG_LAST_NOP_WAIT_EVENT_VECTOR_WIDTH    16

/* LAST_RD_ADDR_TRANSFER */
#define REG_LAST_RD_ADDR_TRANSFER          0x027c
#define REG_LAST_RD_ADDR_TRANSFER_MASK     0x007fffff
#define REG_LAST_RD_ADDR_TRANSFER_SHIFT    0
#define REG_LAST_RD_ADDR_TRANSFER_WIDTH    23

/* MP_TX_TCR_WORD_CTR */
#define REG_MP_TX_TCR_WORD_CTR          0x0280
#define REG_MP_TX_TCR_WORD_CTR_MASK     0x00000003
#define REG_MP_TX_TCR_WORD_CTR_SHIFT    0
#define REG_MP_TX_TCR_WORD_CTR_WIDTH    2

/* MP_TX_BF_WORD_CTR */
#define REG_MP_TX_BF_WORD_CTR          0x0280
#define REG_MP_TX_BF_WORD_CTR_MASK     0x00003f00
#define REG_MP_TX_BF_WORD_CTR_SHIFT    8
#define REG_MP_TX_BF_WORD_CTR_WIDTH    6

/* MP_FIFO_FULL */
#define REG_MP_FIFO_FULL          0x0284
#define REG_MP_FIFO_FULL_MASK     0x00000001
#define REG_MP_FIFO_FULL_SHIFT    0
#define REG_MP_FIFO_FULL_WIDTH    1

/* MP_FIFO_EMPTY */
#define REG_MP_FIFO_EMPTY          0x0284
#define REG_MP_FIFO_EMPTY_MASK     0x00000002
#define REG_MP_FIFO_EMPTY_SHIFT    1
#define REG_MP_FIFO_EMPTY_WIDTH    1

/* AUTO_FILL_LATCH */
#define REG_AUTO_FILL_LATCH          0x0284
#define REG_AUTO_FILL_LATCH_MASK     0x00000004
#define REG_AUTO_FILL_LATCH_SHIFT    2
#define REG_AUTO_FILL_LATCH_WIDTH    1

/* MP_DATA_DONE */
#define REG_MP_DATA_DONE          0x0284
#define REG_MP_DATA_DONE_MASK     0x00000008
#define REG_MP_DATA_DONE_SHIFT    3
#define REG_MP_DATA_DONE_WIDTH    1

/* OVEN_ALIGN */
#define REG_OVEN_ALIGN          0x0284
#define REG_OVEN_ALIGN_MASK     0x00000030
#define REG_OVEN_ALIGN_SHIFT    4
#define REG_OVEN_ALIGN_WIDTH    2

/* WRITE_TO_UPPER_OVEN */
#define REG_WRITE_TO_UPPER_OVEN          0x0284
#define REG_WRITE_TO_UPPER_OVEN_MASK     0x00000040
#define REG_WRITE_TO_UPPER_OVEN_SHIFT    6
#define REG_WRITE_TO_UPPER_OVEN_WIDTH    1

/* MP_FIFO_NUM_BYTES */
#define REG_MP_FIFO_NUM_BYTES          0x0284
#define REG_MP_FIFO_NUM_BYTES_MASK     0x00007f00
#define REG_MP_FIFO_NUM_BYTES_SHIFT    8
#define REG_MP_FIFO_NUM_BYTES_WIDTH    7

/* MP_FIFO_RD_PTR */
#define REG_MP_FIFO_RD_PTR          0x0284
#define REG_MP_FIFO_RD_PTR_MASK     0x003f0000
#define REG_MP_FIFO_RD_PTR_SHIFT    16
#define REG_MP_FIFO_RD_PTR_WIDTH    6

/* MP_FIFO_WR_PTR */
#define REG_MP_FIFO_WR_PTR          0x0284
#define REG_MP_FIFO_WR_PTR_MASK     0x3f000000
#define REG_MP_FIFO_WR_PTR_SHIFT    24
#define REG_MP_FIFO_WR_PTR_WIDTH    6

/* TX_DATA_STRUCTURE_ADDR */
#define REG_TX_DATA_STRUCTURE_ADDR          0x0288
#define REG_TX_DATA_STRUCTURE_ADDR_MASK     0x00ffffff
#define REG_TX_DATA_STRUCTURE_ADDR_SHIFT    0
#define REG_TX_DATA_STRUCTURE_ADDR_WIDTH    24

/* EMU_PRBS_GCLK_BYPASS */
#define REG_EMU_PRBS_GCLK_BYPASS          0x028c
#define REG_EMU_PRBS_GCLK_BYPASS_MASK     0x00000001
#define REG_EMU_PRBS_GCLK_BYPASS_SHIFT    0
#define REG_EMU_PRBS_GCLK_BYPASS_WIDTH    1

/* BF_TX_MAX_THRESHOLD_TIMER_11AG */
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AG          0x0290
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AG_MASK     0x000003ff
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AG_SHIFT    0
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AG_WIDTH    10

/* BF_TX_MAX_THRESHOLD_TIMER_11N */
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11N          0x0290
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11N_MASK     0x000ffc00
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11N_SHIFT    10
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11N_WIDTH    10

/* BF_TX_MAX_THRESHOLD_TIMER_11AC */
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AC          0x0290
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AC_MASK     0x3ff00000
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AC_SHIFT    20
#define REG_BF_TX_MAX_THRESHOLD_TIMER_11AC_WIDTH    10

/* DEL2GENRISC_START_TX_INT */
#define REG_DEL2GENRISC_START_TX_INT          0x0294
#define REG_DEL2GENRISC_START_TX_INT_MASK     0x0000003f
#define REG_DEL2GENRISC_START_TX_INT_SHIFT    0
#define REG_DEL2GENRISC_START_TX_INT_WIDTH    6

/* DEL2GENRISC_START_TX_CLR_INT */
#define REG_DEL2GENRISC_START_TX_CLR_INT          0x0298
#define REG_DEL2GENRISC_START_TX_CLR_INT_MASK     0x0000003f
#define REG_DEL2GENRISC_START_TX_CLR_INT_SHIFT    0
#define REG_DEL2GENRISC_START_TX_CLR_INT_WIDTH    6

/* HOB_DEL_CNTR0_EVENT_AUTO_FILL */
#define REG_HOB_DEL_CNTR0_EVENT_AUTO_FILL          0x029c
#define REG_HOB_DEL_CNTR0_EVENT_AUTO_FILL_MASK     0x00000fff
#define REG_HOB_DEL_CNTR0_EVENT_AUTO_FILL_SHIFT    0
#define REG_HOB_DEL_CNTR0_EVENT_AUTO_FILL_WIDTH    12

/* HOB_DEL_CNTR1_EVENT_AUTO_FILL */
#define REG_HOB_DEL_CNTR1_EVENT_AUTO_FILL          0x029c
#define REG_HOB_DEL_CNTR1_EVENT_AUTO_FILL_MASK     0x0fff0000
#define REG_HOB_DEL_CNTR1_EVENT_AUTO_FILL_SHIFT    16
#define REG_HOB_DEL_CNTR1_EVENT_AUTO_FILL_WIDTH    12

/* AUTO_FILL_VALUE */
#define REG_AUTO_FILL_VALUE          0x02a0
#define REG_AUTO_FILL_VALUE_MASK     0xffffffff
#define REG_AUTO_FILL_VALUE_SHIFT    0
#define REG_AUTO_FILL_VALUE_WIDTH    32

/* AUTO_FILL_CTR_LIMIT */
#define REG_AUTO_FILL_CTR_LIMIT          0x02a4
#define REG_AUTO_FILL_CTR_LIMIT_MASK     0x0003ffff
#define REG_AUTO_FILL_CTR_LIMIT_SHIFT    0
#define REG_AUTO_FILL_CTR_LIMIT_WIDTH    18

/* AUTO_FILL_CTR */
#define REG_AUTO_FILL_CTR          0x02a8
#define REG_AUTO_FILL_CTR_MASK     0x0003ffff
#define REG_AUTO_FILL_CTR_SHIFT    0
#define REG_AUTO_FILL_CTR_WIDTH    18

/* AUTO_FILL_SUBFRAMES_CTR */
#define REG_AUTO_FILL_SUBFRAMES_CTR          0x02ac
#define REG_AUTO_FILL_SUBFRAMES_CTR_MASK     0x0000003f
#define REG_AUTO_FILL_SUBFRAMES_CTR_SHIFT    0
#define REG_AUTO_FILL_SUBFRAMES_CTR_WIDTH    6

/* AUTO_FILL_SUBFRAMES_STATUS_31TO0 */
#define REG_AUTO_FILL_SUBFRAMES_STATUS_31TO0          0x02b0
#define REG_AUTO_FILL_SUBFRAMES_STATUS_31TO0_MASK     0xffffffff
#define REG_AUTO_FILL_SUBFRAMES_STATUS_31TO0_SHIFT    0
#define REG_AUTO_FILL_SUBFRAMES_STATUS_31TO0_WIDTH    32

/* AUTO_FILL_SUBFRAMES_STATUS_63TO32 */
#define REG_AUTO_FILL_SUBFRAMES_STATUS_63TO32          0x02b4
#define REG_AUTO_FILL_SUBFRAMES_STATUS_63TO32_MASK     0xffffffff
#define REG_AUTO_FILL_SUBFRAMES_STATUS_63TO32_SHIFT    0
#define REG_AUTO_FILL_SUBFRAMES_STATUS_63TO32_WIDTH    32

/* RX_CONTROL */
/* Master Rx Enable switch */
#define REG_RX_CONTROL          0x0300
#define REG_RX_CONTROL_MASK     0x00000001
#define REG_RX_CONTROL_SHIFT    0
#define REG_RX_CONTROL_WIDTH    1

/* EARLY_ENABLE_DEASSERT */
/* Deassert PHY Enable for last word */
#define REG_EARLY_ENABLE_DEASSERT          0x0300
#define REG_EARLY_ENABLE_DEASSERT_MASK     0x00000002
#define REG_EARLY_ENABLE_DEASSERT_SHIFT    1
#define REG_EARLY_ENABLE_DEASSERT_WIDTH    1


/* RX_BUFFER_OUT_PTR_RX_DMA_INC_EN*/
/* Enable update RXC buffer out based on Rx DMA monitor address*/
#define REG_RX_BUFFER_OUT_PTR_RX_DMA_INC_EN          0x0300
#define REG_RX_BUFFER_OUT_PTR_RX_DMA_INC_EN_MASK     0x00000004
#define REG_RX_BUFFER_OUT_PTR_RX_DMA_INC_EN_SHIFT    2
#define REG_RX_BUFFER_OUT_PTR_RX_DMA_INC_EN_WIDTH    1


/* PHY_RX_ABORT_RSSI_EN*/
/*Enable abort PHY-MAC Rxsession based on RSSI threshold */
#define REG_RX_PHY_RX_ABORT_RSSI_EN          0x0300
#define REG_RX_PHY_RX_ABORT_RSSI_EN_MASK     0x00000008
#define REG_RX_PHY_RX_ABORT_RSSI_EN_SHIFT    3
#define REG_RX_PHY_RX_ABORT_RSSI_EN_WIDTH    1


/* RESERVED_STORAGE_SIZE */
/* No of 32-bit words at start of Rx record */
#define REG_RESERVED_STORAGE_SIZE          0x0300
#define REG_RESERVED_STORAGE_SIZE_MASK     0x00003f00
#define REG_RESERVED_STORAGE_SIZE_SHIFT    8
#define REG_RESERVED_STORAGE_SIZE_WIDTH    6

/* KEEP_BAD_FRAMES */
/* Retain frames when PHY ERROR indicated */
#define REG_KEEP_BAD_FRAMES          0x0300
#define REG_KEEP_BAD_FRAMES_MASK     0x00008000
#define REG_KEEP_BAD_FRAMES_SHIFT    15
#define REG_KEEP_BAD_FRAMES_WIDTH    1

/* AUTO_DISCARD */
/* Discard frames selected by decision matrix (RXD) */
#define REG_AUTO_DISCARD          0x0300
#define REG_AUTO_DISCARD_MASK     0x00010000
#define REG_AUTO_DISCARD_SHIFT    16
#define REG_AUTO_DISCARD_WIDTH    1

/* AUTO_KEEP */
/* Retain only frames selected by decision matrix (RXD) */
#define REG_AUTO_KEEP          0x0300
#define REG_AUTO_KEEP_MASK     0x00020000
#define REG_AUTO_KEEP_SHIFT    17
#define REG_AUTO_KEEP_WIDTH    1

/* CCA */
/* PHY CCA */
#define REG_CCA          0x0300
#define REG_CCA_MASK     0x00040000
#define REG_CCA_SHIFT    18
#define REG_CCA_WIDTH    1

/* CCA_VALID */
/* PHY CCA is valid */
#define REG_CCA_VALID          0x0300
#define REG_CCA_VALID_MASK     0x00080000
#define REG_CCA_VALID_SHIFT    19
#define REG_CCA_VALID_WIDTH    1

/* RX_BUFFER_NEAR_FULL */
/* Space < near_full_threshold */
#define REG_RX_BUFFER_NEAR_FULL          0x0300
#define REG_RX_BUFFER_NEAR_FULL_MASK     0x00100000
#define REG_RX_BUFFER_NEAR_FULL_SHIFT    20
#define REG_RX_BUFFER_NEAR_FULL_WIDTH    1

/* RX_BUFFER_OVERFLOW */
/* RO, Write '1' to clear */
#define REG_RX_BUFFER_OVERFLOW          0x0300
#define REG_RX_BUFFER_OVERFLOW_MASK     0x00200000
#define REG_RX_BUFFER_OVERFLOW_SHIFT    21
#define REG_RX_BUFFER_OVERFLOW_WIDTH    1

/* RX_FIFO_OVERFLOW */
/* RO, Write '1' to clear */
#define REG_RX_FIFO_OVERFLOW          0x0300
#define REG_RX_FIFO_OVERFLOW_MASK     0x00400000
#define REG_RX_FIFO_OVERFLOW_SHIFT    22
#define REG_RX_FIFO_OVERFLOW_WIDTH    1

/* RECEIVE_BUSY */
/* Write PHY Status after received frame */
#define REG_RECEIVE_BUSY          0x0300
#define REG_RECEIVE_BUSY_MASK     0x00800000
#define REG_RECEIVE_BUSY_SHIFT    23
#define REG_RECEIVE_BUSY_WIDTH    1

/* RX_BUFFER_IN_POINTER */
/* maintained by H/W */
#define REG_RX_BUFFER_IN_POINTER          0x0304
#define REG_RX_BUFFER_IN_POINTER_MASK     0x003ffffc
#define REG_RX_BUFFER_IN_POINTER_SHIFT    2
#define REG_RX_BUFFER_IN_POINTER_WIDTH    20

/* RX_BUFFER_OUT_POINTER */
/* maintained by S/W */
#define REG_RX_BUFFER_OUT_POINTER          0x0308
#define REG_RX_BUFFER_OUT_POINTER_MASK     0x003ffffc
#define REG_RX_BUFFER_OUT_POINTER_SHIFT    2
#define REG_RX_BUFFER_OUT_POINTER_WIDTH    20

/* NEAR_FULL_THRESHOLD */
#define REG_NEAR_FULL_THRESHOLD          0x030c
#define REG_NEAR_FULL_THRESHOLD_MASK     0x000001fc
#define REG_NEAR_FULL_THRESHOLD_SHIFT    2
#define REG_NEAR_FULL_THRESHOLD_WIDTH    7

/* CCA_VALID_DELAY */
#define REG_CCA_VALID_DELAY          0x0310
#define REG_CCA_VALID_DELAY_MASK     0x000007ff
#define REG_CCA_VALID_DELAY_SHIFT    0
#define REG_CCA_VALID_DELAY_WIDTH    11

/* REDUCED_FREQ_CCA_VALID_DELAY */
#define REG_REDUCED_FREQ_CCA_VALID_DELAY          0x0310
#define REG_REDUCED_FREQ_CCA_VALID_DELAY_MASK     0x07ff0000
#define REG_REDUCED_FREQ_CCA_VALID_DELAY_SHIFT    16
#define REG_REDUCED_FREQ_CCA_VALID_DELAY_WIDTH    11

/* CCA_IFS_COMPENSATION */
#define REG_CCA_IFS_COMPENSATION          0x0314
#define REG_CCA_IFS_COMPENSATION_MASK     0x000007ff
#define REG_CCA_IFS_COMPENSATION_SHIFT    0
#define REG_CCA_IFS_COMPENSATION_WIDTH    11

/* REDUCED_FREQ_CCA_IFS_COMPENSATION */
#define REG_REDUCED_FREQ_CCA_IFS_COMPENSATION          0x0314
#define REG_REDUCED_FREQ_CCA_IFS_COMPENSATION_MASK     0x07ff0000
#define REG_REDUCED_FREQ_CCA_IFS_COMPENSATION_SHIFT    16
#define REG_REDUCED_FREQ_CCA_IFS_COMPENSATION_WIDTH    11

/* RXRDY_IFS_COMPENSATION */
#define REG_RXRDY_IFS_COMPENSATION          0x0318
#define REG_RXRDY_IFS_COMPENSATION_MASK     0x000007ff
#define REG_RXRDY_IFS_COMPENSATION_SHIFT    0
#define REG_RXRDY_IFS_COMPENSATION_WIDTH    11

/* REDUCED_FREQ_RXRDY_IFS_COMPENSATION */
#define REG_REDUCED_FREQ_RXRDY_IFS_COMPENSATION          0x0318
#define REG_REDUCED_FREQ_RXRDY_IFS_COMPENSATION_MASK     0x07ff0000
#define REG_REDUCED_FREQ_RXRDY_IFS_COMPENSATION_SHIFT    16
#define REG_REDUCED_FREQ_RXRDY_IFS_COMPENSATION_WIDTH    11

/* RXC_STATE */
#define REG_RXC_STATE          0x031c
#define REG_RXC_STATE_MASK     0x0000000f
#define REG_RXC_STATE_SHIFT    0
#define REG_RXC_STATE_WIDTH    4

/* MT_FORCE_HW_CCA */
#define REG_MT_FORCE_HW_CCA          0x0324
#define REG_MT_FORCE_HW_CCA_MASK     0x00000001
#define REG_MT_FORCE_HW_CCA_SHIFT    0
#define REG_MT_FORCE_HW_CCA_WIDTH    1

/* MT_FORCE_SW_CCA */
#define REG_MT_FORCE_SW_CCA          0x0324
#define REG_MT_FORCE_SW_CCA_MASK     0x00000002
#define REG_MT_FORCE_SW_CCA_SHIFT    1
#define REG_MT_FORCE_SW_CCA_WIDTH    1

/* MT_FORCED_SW_CCA */
#define REG_MT_FORCED_SW_CCA          0x0324
#define REG_MT_FORCED_SW_CCA_MASK     0x00000004
#define REG_MT_FORCED_SW_CCA_SHIFT    2
#define REG_MT_FORCED_SW_CCA_WIDTH    1

/* CRC_CALC_INIT */
#define REG_CRC_CALC_INIT          0x0398
#define REG_CRC_CALC_INIT_MASK     0x00000001
#define REG_CRC_CALC_INIT_SHIFT    0
#define REG_CRC_CALC_INIT_WIDTH    1

/* CRC_BYTE_ENABLE */
#define REG_CRC_BYTE_ENABLE          0x039c
#define REG_CRC_BYTE_ENABLE_MASK     0x0000000f
#define REG_CRC_BYTE_ENABLE_SHIFT    0
#define REG_CRC_BYTE_ENABLE_WIDTH    4

/* CRC_DATA_IN_OUT */
/* Write input data, read results */
#define REG_CRC_DATA_IN_OUT          0x03a0
#define REG_CRC_DATA_IN_OUT_MASK     0xffffffff
#define REG_CRC_DATA_IN_OUT_SHIFT    0
#define REG_CRC_DATA_IN_OUT_WIDTH    32

/* CRC_CALC_RESULT */
#define REG_CRC_CALC_RESULT          0x03a4
#define REG_CRC_CALC_RESULT_MASK     0x00000001
#define REG_CRC_CALC_RESULT_SHIFT    0
#define REG_CRC_CALC_RESULT_WIDTH    1

/* BUFFER_OVERFLOW_CTR */
/* Clear by read */
#define REG_BUFFER_OVERFLOW_CTR          0x03ac
#define REG_BUFFER_OVERFLOW_CTR_MASK     0x0000ffff
#define REG_BUFFER_OVERFLOW_CTR_SHIFT    0
#define REG_BUFFER_OVERFLOW_CTR_WIDTH    16

/* CFP_END_TIME */
/* The time for the end of the CFP expressed in Time Units  */
#define REG_CFP_END_TIME          0x0404
#define REG_CFP_END_TIME_MASK     0x0000ffff
#define REG_CFP_END_TIME_SHIFT    0
#define REG_CFP_END_TIME_WIDTH    16

/* TSF_START_TIME_ADJUST */
/* Value is in us and signed. */
#define REG_TSF_START_TIME_ADJUST          0x0408
#define REG_TSF_START_TIME_ADJUST_MASK     0x00000fff
#define REG_TSF_START_TIME_ADJUST_SHIFT    0
#define REG_TSF_START_TIME_ADJUST_WIDTH    12

/* POWER_MGMT */
/* Power management HOB bit */
#define REG_POWER_MGMT          0x040c
#define REG_POWER_MGMT_MASK     0x00000001
#define REG_POWER_MGMT_SHIFT    0
#define REG_POWER_MGMT_WIDTH    1

/* NAV_ADJUST */
/* Adjustment in microseconds to be subtracted from extracted Duration values which are loaded to th NAV timer. */
#define REG_NAV_ADJUST          0x0410
#define REG_NAV_ADJUST_MASK     0x0000007f
#define REG_NAV_ADJUST_SHIFT    0
#define REG_NAV_ADJUST_WIDTH    7

/* NO_ACK */
/* Bits 0-3 supress ACK for ACK policy 03 */
#define REG_NO_ACK          0x0418
#define REG_NO_ACK_MASK     0x0000001f
#define REG_NO_ACK_SHIFT    0
#define REG_NO_ACK_WIDTH    5

/* TSF_FIELD_TIME */
/* Offset within transmitted frame to start of TSF field */
#define REG_TSF_FIELD_TIME          0x041c
#define REG_TSF_FIELD_TIME_MASK     0x000001ff
#define REG_TSF_FIELD_TIME_SHIFT    0
#define REG_TSF_FIELD_TIME_WIDTH    9

/* AUTO_REPLY_PHY_MODE_TABLE */
#define REG_AUTO_REPLY_PHY_MODE_TABLE          0x0420
#define REG_AUTO_REPLY_PHY_MODE_TABLE_MASK     0x000000ff
#define REG_AUTO_REPLY_PHY_MODE_TABLE_SHIFT    0
#define REG_AUTO_REPLY_PHY_MODE_TABLE_WIDTH    8

/* AUTO_REPLY_BW_TABLE */
#define REG_AUTO_REPLY_BW_TABLE          0x0420
#define REG_AUTO_REPLY_BW_TABLE_MASK     0x0000ff00
#define REG_AUTO_REPLY_BW_TABLE_SHIFT    8
#define REG_AUTO_REPLY_BW_TABLE_WIDTH    8

/* AUTO_REPLY_MCS_11B_TABLE */
#define REG_AUTO_REPLY_MCS_11B_TABLE          0x0424
#define REG_AUTO_REPLY_MCS_11B_TABLE_MASK     0x00ffffff
#define REG_AUTO_REPLY_MCS_11B_TABLE_SHIFT    0
#define REG_AUTO_REPLY_MCS_11B_TABLE_WIDTH    24

/* AUTO_REPLY_MCS_OFDM_TABLE */
#define REG_AUTO_REPLY_MCS_OFDM_TABLE          0x0428
#define REG_AUTO_REPLY_MCS_OFDM_TABLE_MASK     0x3fffffff
#define REG_AUTO_REPLY_MCS_OFDM_TABLE_SHIFT    0
#define REG_AUTO_REPLY_MCS_OFDM_TABLE_WIDTH    30

/* AUTO_REPLY_POWER_11B_TABLE_0TO29 */
#define REG_AUTO_REPLY_POWER_11B_TABLE_0TO29          0x042c
#define REG_AUTO_REPLY_POWER_11B_TABLE_0TO29_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_11B_TABLE_0TO29_SHIFT    0
#define REG_AUTO_REPLY_POWER_11B_TABLE_0TO29_WIDTH    30

/* AUTO_REPLY_POWER_11B_TABLE_30TO47 */
#define REG_AUTO_REPLY_POWER_11B_TABLE_30TO47          0x0430
#define REG_AUTO_REPLY_POWER_11B_TABLE_30TO47_MASK     0x0003ffff
#define REG_AUTO_REPLY_POWER_11B_TABLE_30TO47_SHIFT    0
#define REG_AUTO_REPLY_POWER_11B_TABLE_30TO47_WIDTH    18

/* AUTO_REPLY_POWER_OFDM_TABLE_0TO29 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_0TO29          0x0434
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_0TO29_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_0TO29_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_0TO29_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_30TO59 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_30TO59          0x0438
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_30TO59_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_30TO59_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_30TO59_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_60TO89 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_60TO89          0x043c
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_60TO89_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_60TO89_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_60TO89_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_90TO119 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_90TO119          0x0440
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_90TO119_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_90TO119_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_90TO119_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_120TO149 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_120TO149          0x0444
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_120TO149_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_120TO149_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_120TO149_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_150TO179 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_150TO179          0x0448
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_150TO179_MASK     0x3fffffff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_150TO179_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_150TO179_WIDTH    30

/* AUTO_REPLY_POWER_OFDM_TABLE_180TO191 */
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_180TO191          0x044c
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_180TO191_MASK     0x000007ff
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_180TO191_SHIFT    0
#define REG_AUTO_REPLY_POWER_OFDM_TABLE_180TO191_WIDTH    11

/* TXOP_JUMP_TABLE_BASE */
#define REG_TXOP_JUMP_TABLE_BASE          0x0450
#define REG_TXOP_JUMP_TABLE_BASE_MASK     0x00ffffc0
#define REG_TXOP_JUMP_TABLE_BASE_SHIFT    6
#define REG_TXOP_JUMP_TABLE_BASE_WIDTH    18

/* TXOP_DURATION_COMPENSATE */
#define REG_TXOP_DURATION_COMPENSATE          0x045c
#define REG_TXOP_DURATION_COMPENSATE_MASK     0x00003fff
#define REG_TXOP_DURATION_COMPENSATE_SHIFT    0
#define REG_TXOP_DURATION_COMPENSATE_WIDTH    14

/* FRAME_DURATION */
#define REG_FRAME_DURATION          0x0460
#define REG_FRAME_DURATION_MASK     0x00003fff
#define REG_FRAME_DURATION_SHIFT    0
#define REG_FRAME_DURATION_WIDTH    14

/* TXOP_BUDGET_COMPENSATE */
#define REG_TXOP_BUDGET_COMPENSATE          0x0464
#define REG_TXOP_BUDGET_COMPENSATE_MASK     0x00003fff
#define REG_TXOP_BUDGET_COMPENSATE_SHIFT    0
#define REG_TXOP_BUDGET_COMPENSATE_WIDTH    14

/* TXOP_BUDGET_AVAILABLE */
#define REG_TXOP_BUDGET_AVAILABLE          0x0468
#define REG_TXOP_BUDGET_AVAILABLE_MASK     0x00000001
#define REG_TXOP_BUDGET_AVAILABLE_SHIFT    0
#define REG_TXOP_BUDGET_AVAILABLE_WIDTH    1

/* LATCHED_TXOP_LIMIT_COUNTER */
#define REG_LATCHED_TXOP_LIMIT_COUNTER          0x046c
#define REG_LATCHED_TXOP_LIMIT_COUNTER_MASK     0x00003fff
#define REG_LATCHED_TXOP_LIMIT_COUNTER_SHIFT    0
#define REG_LATCHED_TXOP_LIMIT_COUNTER_WIDTH    14

/* SNOOP_FIFO_ADDR */
#define REG_SNOOP_FIFO_ADDR          0x0470
#define REG_SNOOP_FIFO_ADDR_MASK     0x000f00ff
#define REG_SNOOP_FIFO_ADDR_SHIFT    0
#define REG_SNOOP_FIFO_ADDR_WIDTH    12

/* CRC_USAGE_MODE */
#define REG_CRC_USAGE_MODE          0x0474
#define REG_CRC_USAGE_MODE_MASK     0x00000001
#define REG_CRC_USAGE_MODE_SHIFT    0
#define REG_CRC_USAGE_MODE_WIDTH    1

/* AUTO_REPLY_PHY_MODE */
#define REG_AUTO_REPLY_PHY_MODE          0x0480
#define REG_AUTO_REPLY_PHY_MODE_MASK     0x00000003
#define REG_AUTO_REPLY_PHY_MODE_SHIFT    0
#define REG_AUTO_REPLY_PHY_MODE_WIDTH    2

/* AUTO_REPLY_BW */
#define REG_AUTO_REPLY_BW          0x0480
#define REG_AUTO_REPLY_BW_MASK     0x0000000c
#define REG_AUTO_REPLY_BW_SHIFT    2
#define REG_AUTO_REPLY_BW_WIDTH    2

/* AUTO_REPLY_MCS */
#define REG_AUTO_REPLY_MCS          0x0480
#define REG_AUTO_REPLY_MCS_MASK     0x000003f0
#define REG_AUTO_REPLY_MCS_SHIFT    4
#define REG_AUTO_REPLY_MCS_WIDTH    6

/* AUTO_REPLY_TX_POWER */
#define REG_AUTO_REPLY_TX_POWER          0x0480
#define REG_AUTO_REPLY_TX_POWER_MASK     0x0000fc00
#define REG_AUTO_REPLY_TX_POWER_SHIFT    10
#define REG_AUTO_REPLY_TX_POWER_WIDTH    6

/* TSF_FIELD_TIME1 */
#define REG_TSF_FIELD_TIME1          0x0484
#define REG_TSF_FIELD_TIME1_MASK     0x01ffffff
#define REG_TSF_FIELD_TIME1_SHIFT    0
#define REG_TSF_FIELD_TIME1_WIDTH    25

/* TSF_FIELD_TIME2 */
#define REG_TSF_FIELD_TIME2          0x0488
#define REG_TSF_FIELD_TIME2_MASK     0x01ffffff
#define REG_TSF_FIELD_TIME2_SHIFT    0
#define REG_TSF_FIELD_TIME2_WIDTH    25

/* TSF_FIELD_TIME3 */
#define REG_TSF_FIELD_TIME3          0x048c
#define REG_TSF_FIELD_TIME3_MASK     0x01ffffff
#define REG_TSF_FIELD_TIME3_SHIFT    0
#define REG_TSF_FIELD_TIME3_WIDTH    25

/* NAV_UPDATE_VALUE_AID */
#define REG_NAV_UPDATE_VALUE_AID          0x0494
#define REG_NAV_UPDATE_VALUE_AID_MASK     0x0000ffff
#define REG_NAV_UPDATE_VALUE_AID_SHIFT    0
#define REG_NAV_UPDATE_VALUE_AID_WIDTH    16

/* AUTO_REPLY_DUR */
#define REG_AUTO_REPLY_DUR          0x049c
#define REG_AUTO_REPLY_DUR_MASK     0x0000ffff
#define REG_AUTO_REPLY_DUR_SHIFT    0
#define REG_AUTO_REPLY_DUR_WIDTH    16

/* AUTO_REPLY_BF_REPORT_DUR */
#define REG_AUTO_REPLY_BF_REPORT_DUR          0x049c
#define REG_AUTO_REPLY_BF_REPORT_DUR_MASK     0xffff0000
#define REG_AUTO_REPLY_BF_REPORT_DUR_SHIFT    16
#define REG_AUTO_REPLY_BF_REPORT_DUR_WIDTH    16

/* TSF_ADJUST_DATA_IN */
#define REG_TSF_ADJUST_DATA_IN          0x04a0
#define REG_TSF_ADJUST_DATA_IN_MASK     0x00ffffff
#define REG_TSF_ADJUST_DATA_IN_SHIFT    0
#define REG_TSF_ADJUST_DATA_IN_WIDTH    24

/* TXOP_HOLDER_ADDR_BITS0TO31 */
#define REG_TXOP_HOLDER_ADDR_BITS0TO31          0x04ac
#define REG_TXOP_HOLDER_ADDR_BITS0TO31_MASK     0xffffffff
#define REG_TXOP_HOLDER_ADDR_BITS0TO31_SHIFT    0
#define REG_TXOP_HOLDER_ADDR_BITS0TO31_WIDTH    32

/* TXOP_HOLDER_ADDR_BITS32TO47 */
#define REG_TXOP_HOLDER_ADDR_BITS32TO47          0x04b0
#define REG_TXOP_HOLDER_ADDR_BITS32TO47_MASK     0x0000ffff
#define REG_TXOP_HOLDER_ADDR_BITS32TO47_SHIFT    0
#define REG_TXOP_HOLDER_ADDR_BITS32TO47_WIDTH    16

/* TXOP_HOLDER_ADDR_VALID */
#define REG_TXOP_HOLDER_ADDR_VALID          0x04b0
#define REG_TXOP_HOLDER_ADDR_VALID_MASK     0x00010000
#define REG_TXOP_HOLDER_ADDR_VALID_SHIFT    16
#define REG_TXOP_HOLDER_ADDR_VALID_WIDTH    1

/* TSF_OFFSET0 */
#define REG_TSF_OFFSET0          0x04b4
#define REG_TSF_OFFSET0_MASK     0x01ffffff
#define REG_TSF_OFFSET0_SHIFT    0
#define REG_TSF_OFFSET0_WIDTH    25

/* TSF_OFFSET1 */
#define REG_TSF_OFFSET1          0x04b8
#define REG_TSF_OFFSET1_MASK     0x01ffffff
#define REG_TSF_OFFSET1_SHIFT    0
#define REG_TSF_OFFSET1_WIDTH    25

/* TSF_OFFSET2 */
#define REG_TSF_OFFSET2          0x04bc
#define REG_TSF_OFFSET2_MASK     0x01ffffff
#define REG_TSF_OFFSET2_SHIFT    0
#define REG_TSF_OFFSET2_WIDTH    25

/* TSF_OFFSET3 */
#define REG_TSF_OFFSET3          0x04c0
#define REG_TSF_OFFSET3_MASK     0x01ffffff
#define REG_TSF_OFFSET3_SHIFT    0
#define REG_TSF_OFFSET3_WIDTH    25

/* TSF_OFFSET4 */
#define REG_TSF_OFFSET4          0x04c4
#define REG_TSF_OFFSET4_MASK     0x01ffffff
#define REG_TSF_OFFSET4_SHIFT    0
#define REG_TSF_OFFSET4_WIDTH    25

/* TSF_OFFSET5 */
#define REG_TSF_OFFSET5          0x04c8
#define REG_TSF_OFFSET5_MASK     0x01ffffff
#define REG_TSF_OFFSET5_SHIFT    0
#define REG_TSF_OFFSET5_WIDTH    25

/* TSF_OFFSET6 */
#define REG_TSF_OFFSET6          0x04cc
#define REG_TSF_OFFSET6_MASK     0x01ffffff
#define REG_TSF_OFFSET6_SHIFT    0
#define REG_TSF_OFFSET6_WIDTH    25

/* TSF_OFFSET7 */
#define REG_TSF_OFFSET7          0x04d0
#define REG_TSF_OFFSET7_MASK     0x01ffffff
#define REG_TSF_OFFSET7_SHIFT    0
#define REG_TSF_OFFSET7_WIDTH    25

/* TSF_OFFSET8 */
#define REG_TSF_OFFSET8          0x04d4
#define REG_TSF_OFFSET8_MASK     0x01ffffff
#define REG_TSF_OFFSET8_SHIFT    0
#define REG_TSF_OFFSET8_WIDTH    25

/* TSF_OFFSET9 */
#define REG_TSF_OFFSET9          0x04d8
#define REG_TSF_OFFSET9_MASK     0x01ffffff
#define REG_TSF_OFFSET9_SHIFT    0
#define REG_TSF_OFFSET9_WIDTH    25

/* TSF_OFFSET10 */
#define REG_TSF_OFFSET10          0x04dc
#define REG_TSF_OFFSET10_MASK     0x01ffffff
#define REG_TSF_OFFSET10_SHIFT    0
#define REG_TSF_OFFSET10_WIDTH    25

/* TSF_OFFSET11 */
#define REG_TSF_OFFSET11          0x04e0
#define REG_TSF_OFFSET11_MASK     0x01ffffff
#define REG_TSF_OFFSET11_SHIFT    0
#define REG_TSF_OFFSET11_WIDTH    25

/* TSF_OFFSET12 */
#define REG_TSF_OFFSET12          0x04e4
#define REG_TSF_OFFSET12_MASK     0x01ffffff
#define REG_TSF_OFFSET12_SHIFT    0
#define REG_TSF_OFFSET12_WIDTH    25

/* TSF_OFFSET13 */
#define REG_TSF_OFFSET13          0x04e8
#define REG_TSF_OFFSET13_MASK     0x01ffffff
#define REG_TSF_OFFSET13_SHIFT    0
#define REG_TSF_OFFSET13_WIDTH    25

/* TSF_OFFSET14 */
#define REG_TSF_OFFSET14          0x04ec
#define REG_TSF_OFFSET14_MASK     0x01ffffff
#define REG_TSF_OFFSET14_SHIFT    0
#define REG_TSF_OFFSET14_WIDTH    25

/* TSF_OFFSET15 */
#define REG_TSF_OFFSET15          0x04f0
#define REG_TSF_OFFSET15_MASK     0x01ffffff
#define REG_TSF_OFFSET15_SHIFT    0
#define REG_TSF_OFFSET15_WIDTH    25

/* TSF_OFFSET_INDEX */
#define REG_TSF_OFFSET_INDEX          0x04f4
#define REG_TSF_OFFSET_INDEX_MASK     0x0000001f
#define REG_TSF_OFFSET_INDEX_SHIFT    0
#define REG_TSF_OFFSET_INDEX_WIDTH    5

/* DUR_TIMER_RXRDY_COMPENSATION */
#define REG_DUR_TIMER_RXRDY_COMPENSATION          0x04f8
#define REG_DUR_TIMER_RXRDY_COMPENSATION_MASK     0x0000001f
#define REG_DUR_TIMER_RXRDY_COMPENSATION_SHIFT    0
#define REG_DUR_TIMER_RXRDY_COMPENSATION_WIDTH    5

/* DUR_TIMER_TXEN_COMPENSATION */
#define REG_DUR_TIMER_TXEN_COMPENSATION          0x04f8
#define REG_DUR_TIMER_TXEN_COMPENSATION_MASK     0x00001f00
#define REG_DUR_TIMER_TXEN_COMPENSATION_SHIFT    8
#define REG_DUR_TIMER_TXEN_COMPENSATION_WIDTH    5

/* TSF_HW_EVENT0_ENABLE */
/* TSF HW event 0 response enable */
#define REG_TSF_HW_EVENT0_ENABLE          0x0500
#define REG_TSF_HW_EVENT0_ENABLE_MASK     0x00000001
#define REG_TSF_HW_EVENT0_ENABLE_SHIFT    0
#define REG_TSF_HW_EVENT0_ENABLE_WIDTH    1

/* TSF_HW_EVENT1_ENABLE */
/* TSF HW event 1 response enable */
#define REG_TSF_HW_EVENT1_ENABLE          0x0500
#define REG_TSF_HW_EVENT1_ENABLE_MASK     0x00000002
#define REG_TSF_HW_EVENT1_ENABLE_SHIFT    1
#define REG_TSF_HW_EVENT1_ENABLE_WIDTH    1

/* SW_TX_REQ0_SET */
/* SW Tx request 0 enable */
#define REG_SW_TX_REQ0_SET          0x0500
#define REG_SW_TX_REQ0_SET_MASK     0x00010000
#define REG_SW_TX_REQ0_SET_SHIFT    16
#define REG_SW_TX_REQ0_SET_WIDTH    1

/* SW_TX_REQ1_SET */
/* SW Tx request 1 enable */
#define REG_SW_TX_REQ1_SET          0x0500
#define REG_SW_TX_REQ1_SET_MASK     0x00020000
#define REG_SW_TX_REQ1_SET_SHIFT    17
#define REG_SW_TX_REQ1_SET_WIDTH    1

/* SW_TX_REQ0_RESET */
/* SW Tx request 0 enable */
#define REG_SW_TX_REQ0_RESET          0x0500
#define REG_SW_TX_REQ0_RESET_MASK     0x00100000
#define REG_SW_TX_REQ0_RESET_SHIFT    20
#define REG_SW_TX_REQ0_RESET_WIDTH    1

/* SW_TX_REQ1_RESET */
/* SW Tx request 1 enable */
#define REG_SW_TX_REQ1_RESET          0x0500
#define REG_SW_TX_REQ1_RESET_MASK     0x00200000
#define REG_SW_TX_REQ1_RESET_SHIFT    21
#define REG_SW_TX_REQ1_RESET_WIDTH    1

/* NO_TX_REQ0_SET */
/* No Tx request 0 enable */
#define REG_NO_TX_REQ0_SET          0x0500
#define REG_NO_TX_REQ0_SET_MASK     0x01000000
#define REG_NO_TX_REQ0_SET_SHIFT    24
#define REG_NO_TX_REQ0_SET_WIDTH    1

/* NO_TX_REQ1_SET */
/* No Tx request 1 enable */
#define REG_NO_TX_REQ1_SET          0x0500
#define REG_NO_TX_REQ1_SET_MASK     0x02000000
#define REG_NO_TX_REQ1_SET_SHIFT    25
#define REG_NO_TX_REQ1_SET_WIDTH    1

/* NO_TX_REQ0_RESET */
/* No Tx request 0 enable */
#define REG_NO_TX_REQ0_RESET          0x0500
#define REG_NO_TX_REQ0_RESET_MASK     0x10000000
#define REG_NO_TX_REQ0_RESET_SHIFT    28
#define REG_NO_TX_REQ0_RESET_WIDTH    1

/* NO_TX_REQ1_RESET */
/* No Tx request 1 enable */
#define REG_NO_TX_REQ1_RESET          0x0500
#define REG_NO_TX_REQ1_RESET_MASK     0x20000000
#define REG_NO_TX_REQ1_RESET_SHIFT    29
#define REG_NO_TX_REQ1_RESET_WIDTH    1

/* RX_EVENT_RESP_ENABLE */
/* Rx event response enables */
#define REG_RX_EVENT_RESP_ENABLE          0x0504
#define REG_RX_EVENT_RESP_ENABLE_MASK     0x3fffffff
#define REG_RX_EVENT_RESP_ENABLE_SHIFT    0
#define REG_RX_EVENT_RESP_ENABLE_WIDTH    30

/* RESP_TIMEOUTA */
#define REG_RESP_TIMEOUTA          0x0504
#define REG_RESP_TIMEOUTA_MASK     0x40000000
#define REG_RESP_TIMEOUTA_SHIFT    30
#define REG_RESP_TIMEOUTA_WIDTH    1

/* RESP_TIMEOUTB */
#define REG_RESP_TIMEOUTB          0x0504
#define REG_RESP_TIMEOUTB_MASK     0x80000000
#define REG_RESP_TIMEOUTB_SHIFT    31
#define REG_RESP_TIMEOUTB_WIDTH    1

/* RX_EVENT_RESP_MULTI_SHOT */
/* Rx event response enables */
#define REG_RX_EVENT_RESP_MULTI_SHOT          0x0508
#define REG_RX_EVENT_RESP_MULTI_SHOT_MASK     0x3fffffff
#define REG_RX_EVENT_RESP_MULTI_SHOT_SHIFT    0
#define REG_RX_EVENT_RESP_MULTI_SHOT_WIDTH    30

/* RESP_TIMEOUT_MULTI_SHOT */
#define REG_RESP_TIMEOUT_MULTI_SHOT          0x0508
#define REG_RESP_TIMEOUT_MULTI_SHOT_MASK     0x80000000
#define REG_RESP_TIMEOUT_MULTI_SHOT_SHIFT    31
#define REG_RESP_TIMEOUT_MULTI_SHOT_WIDTH    1

/* RX_EVENT_RESP_PRIORITY */
/* Rx event repsonse priorities */
#define REG_RX_EVENT_RESP_PRIORITY          0x050c
#define REG_RX_EVENT_RESP_PRIORITY_MASK     0x3fffffff
#define REG_RX_EVENT_RESP_PRIORITY_SHIFT    0
#define REG_RX_EVENT_RESP_PRIORITY_WIDTH    30

/* RESP_TIMEOUT__PRIORITY */
#define REG_RESP_TIMEOUT__PRIORITY          0x050c
#define REG_RESP_TIMEOUT__PRIORITY_MASK     0x80000000
#define REG_RESP_TIMEOUT__PRIORITY_SHIFT    31
#define REG_RESP_TIMEOUT__PRIORITY_WIDTH    1

/* RX_EVENT_RESP_REQUEUE_OVERRIDE */
/* Rx event re-queue overrides */
#define REG_RX_EVENT_RESP_REQUEUE_OVERRIDE          0x0510
#define REG_RX_EVENT_RESP_REQUEUE_OVERRIDE_MASK     0x3fffffff
#define REG_RX_EVENT_RESP_REQUEUE_OVERRIDE_SHIFT    0
#define REG_RX_EVENT_RESP_REQUEUE_OVERRIDE_WIDTH    30

/* RX_EVENT_NAV_LOADA */
/* Rx event NAV load, [1:0] are for Rx event 0, etc. */
#define REG_RX_EVENT_NAV_LOADA          0x0514
#define REG_RX_EVENT_NAV_LOADA_MASK     0xffffffff
#define REG_RX_EVENT_NAV_LOADA_SHIFT    0
#define REG_RX_EVENT_NAV_LOADA_WIDTH    32

/* RX_EVENT_NAV_LOADB */
/* Rx event NAV load */
#define REG_RX_EVENT_NAV_LOADB          0x0518
#define REG_RX_EVENT_NAV_LOADB_MASK     0x0fffffff
#define REG_RX_EVENT_NAV_LOADB_SHIFT    0
#define REG_RX_EVENT_NAV_LOADB_WIDTH    28

/* RX_SW_EVENT_ENABLE */
/* Rx event response enables, 0 and 1 are CRC error */
#define REG_RX_SW_EVENT_ENABLE          0x051c
#define REG_RX_SW_EVENT_ENABLE_MASK     0x3fffffff
#define REG_RX_SW_EVENT_ENABLE_SHIFT    0
#define REG_RX_SW_EVENT_ENABLE_WIDTH    30

/* UNRECOGNISED */
#define REG_UNRECOGNISED          0x051c
#define REG_UNRECOGNISED_MASK     0x40000000
#define REG_UNRECOGNISED_SHIFT    30
#define REG_UNRECOGNISED_WIDTH    1

/* RESP_TIMEOUT_ENABLE */
#define REG_RESP_TIMEOUT_ENABLE          0x051c
#define REG_RESP_TIMEOUT_ENABLE_MASK     0x80000000
#define REG_RESP_TIMEOUT_ENABLE_SHIFT    31
#define REG_RESP_TIMEOUT_ENABLE_WIDTH    1

/* RESP_RX_EVENT_STATUS */
#define REG_RESP_RX_EVENT_STATUS          0x0520
#define REG_RESP_RX_EVENT_STATUS_MASK     0x0000003f
#define REG_RESP_RX_EVENT_STATUS_SHIFT    0
#define REG_RESP_RX_EVENT_STATUS_WIDTH    6

/* RESP_RX_STORED */
#define REG_RESP_RX_STORED          0x0520
#define REG_RESP_RX_STORED_MASK     0x00000080
#define REG_RESP_RX_STORED_SHIFT    7
#define REG_RESP_RX_STORED_WIDTH    1

/* PTCS_REQ */
#define REG_PTCS_REQ          0x0520
#define REG_PTCS_REQ_MASK     0x00007f00
#define REG_PTCS_REQ_SHIFT    8
#define REG_PTCS_REQ_WIDTH    7

/* PTCS_EVENT_STATUS */
#define REG_PTCS_EVENT_STATUS          0x0520
#define REG_PTCS_EVENT_STATUS_MASK     0x00030000
#define REG_PTCS_EVENT_STATUS_SHIFT    16
#define REG_PTCS_EVENT_STATUS_WIDTH    2

/* CHAIN */
#define REG_CHAIN          0x0520
#define REG_CHAIN_MASK     0x000c0000
#define REG_CHAIN_SHIFT    18
#define REG_CHAIN_WIDTH    2

/* PTCS_ABORT_STATUS */
#define REG_PTCS_ABORT_STATUS          0x0520
#define REG_PTCS_ABORT_STATUS_MASK     0x00700000
#define REG_PTCS_ABORT_STATUS_SHIFT    20
#define REG_PTCS_ABORT_STATUS_WIDTH    3

/* RESP_RX_EVENT */
#define REG_RESP_RX_EVENT          0x0520
#define REG_RESP_RX_EVENT_MASK     0x01000000
#define REG_RESP_RX_EVENT_SHIFT    24
#define REG_RESP_RX_EVENT_WIDTH    1

/* PTCS_EVENT */
#define REG_PTCS_EVENT          0x0520
#define REG_PTCS_EVENT_MASK     0x02000000
#define REG_PTCS_EVENT_SHIFT    25
#define REG_PTCS_EVENT_WIDTH    1

/* TSF_HW_EVENT0 */
#define REG_TSF_HW_EVENT0          0x0520
#define REG_TSF_HW_EVENT0_MASK     0x04000000
#define REG_TSF_HW_EVENT0_SHIFT    26
#define REG_TSF_HW_EVENT0_WIDTH    1

/* TSF_HW_EVENT1 */
#define REG_TSF_HW_EVENT1          0x0520
#define REG_TSF_HW_EVENT1_MASK     0x08000000
#define REG_TSF_HW_EVENT1_SHIFT    27
#define REG_TSF_HW_EVENT1_WIDTH    1

/* TSF_REQ_SWITCH */
#define REG_TSF_REQ_SWITCH          0x0520
#define REG_TSF_REQ_SWITCH_MASK     0x10000000
#define REG_TSF_REQ_SWITCH_SHIFT    28
#define REG_TSF_REQ_SWITCH_WIDTH    1

/* TSF_REQ_SWITCH_STATUS */
#define REG_TSF_REQ_SWITCH_STATUS          0x0520
#define REG_TSF_REQ_SWITCH_STATUS_MASK     0x20000000
#define REG_TSF_REQ_SWITCH_STATUS_SHIFT    29
#define REG_TSF_REQ_SWITCH_STATUS_WIDTH    1

/* OVERFLOW */
/* NTD status FIFO overflow */
#define REG_OVERFLOW          0x0520
#define REG_OVERFLOW_MASK     0x40000000
#define REG_OVERFLOW_SHIFT    30
#define REG_OVERFLOW_WIDTH    1

/* EMPTY */
/* NTD status FIFO empty */
#define REG_EMPTY          0x0520
#define REG_EMPTY_MASK     0x80000000
#define REG_EMPTY_SHIFT    31
#define REG_EMPTY_WIDTH    1

/* RESP_RX_EVENT_STATUS_PEEK */
#define REG_RESP_RX_EVENT_STATUS_PEEK          0x0524
#define REG_RESP_RX_EVENT_STATUS_PEEK_MASK     0x0000003f
#define REG_RESP_RX_EVENT_STATUS_PEEK_SHIFT    0
#define REG_RESP_RX_EVENT_STATUS_PEEK_WIDTH    6

/* RESP_RX_STORED_PEEK */
#define REG_RESP_RX_STORED_PEEK          0x0524
#define REG_RESP_RX_STORED_PEEK_MASK     0x00000080
#define REG_RESP_RX_STORED_PEEK_SHIFT    7
#define REG_RESP_RX_STORED_PEEK_WIDTH    1

/* PTCS_REQ_PEEK */
#define REG_PTCS_REQ_PEEK          0x0524
#define REG_PTCS_REQ_PEEK_MASK     0x00007f00
#define REG_PTCS_REQ_PEEK_SHIFT    8
#define REG_PTCS_REQ_PEEK_WIDTH    7

/* PTCS_EVENT_STATUS_PEEK */
#define REG_PTCS_EVENT_STATUS_PEEK          0x0524
#define REG_PTCS_EVENT_STATUS_PEEK_MASK     0x00030000
#define REG_PTCS_EVENT_STATUS_PEEK_SHIFT    16
#define REG_PTCS_EVENT_STATUS_PEEK_WIDTH    2

/* CHAIN_PEEK */
#define REG_CHAIN_PEEK          0x0524
#define REG_CHAIN_PEEK_MASK     0x000c0000
#define REG_CHAIN_PEEK_SHIFT    18
#define REG_CHAIN_PEEK_WIDTH    2

/* PTCS_ABORT_STATUS_PEEK */
#define REG_PTCS_ABORT_STATUS_PEEK          0x0524
#define REG_PTCS_ABORT_STATUS_PEEK_MASK     0x00700000
#define REG_PTCS_ABORT_STATUS_PEEK_SHIFT    20
#define REG_PTCS_ABORT_STATUS_PEEK_WIDTH    3

/* RESP_RX_EVENT_PEEK */
#define REG_RESP_RX_EVENT_PEEK          0x0524
#define REG_RESP_RX_EVENT_PEEK_MASK     0x01000000
#define REG_RESP_RX_EVENT_PEEK_SHIFT    24
#define REG_RESP_RX_EVENT_PEEK_WIDTH    1

/* PTCS_EVENT_PEEK */
#define REG_PTCS_EVENT_PEEK          0x0524
#define REG_PTCS_EVENT_PEEK_MASK     0x02000000
#define REG_PTCS_EVENT_PEEK_SHIFT    25
#define REG_PTCS_EVENT_PEEK_WIDTH    1

/* TSF_HW_EVENT0_PEEK */
#define REG_TSF_HW_EVENT0_PEEK          0x0524
#define REG_TSF_HW_EVENT0_PEEK_MASK     0x04000000
#define REG_TSF_HW_EVENT0_PEEK_SHIFT    26
#define REG_TSF_HW_EVENT0_PEEK_WIDTH    1

/* TSF_HW_EVENT1_PEEK */
#define REG_TSF_HW_EVENT1_PEEK          0x0524
#define REG_TSF_HW_EVENT1_PEEK_MASK     0x08000000
#define REG_TSF_HW_EVENT1_PEEK_SHIFT    27
#define REG_TSF_HW_EVENT1_PEEK_WIDTH    1

/* TSF_REQ_SWITCH_PEEK */
#define REG_TSF_REQ_SWITCH_PEEK          0x0524
#define REG_TSF_REQ_SWITCH_PEEK_MASK     0x10000000
#define REG_TSF_REQ_SWITCH_PEEK_SHIFT    28
#define REG_TSF_REQ_SWITCH_PEEK_WIDTH    1

/* TSF_REQ_SWITCH_STATUS_PEEK */
#define REG_TSF_REQ_SWITCH_STATUS_PEEK          0x0524
#define REG_TSF_REQ_SWITCH_STATUS_PEEK_MASK     0x20000000
#define REG_TSF_REQ_SWITCH_STATUS_PEEK_SHIFT    29
#define REG_TSF_REQ_SWITCH_STATUS_PEEK_WIDTH    1

/* OVERFLOW_PEEK */
/* NTD status FIFO overflow */
#define REG_OVERFLOW_PEEK          0x0524
#define REG_OVERFLOW_PEEK_MASK     0x40000000
#define REG_OVERFLOW_PEEK_SHIFT    30
#define REG_OVERFLOW_PEEK_WIDTH    1

/* EMPTY_PEEK */
/* NTD status FIFO empty */
#define REG_EMPTY_PEEK          0x0524
#define REG_EMPTY_PEEK_MASK     0x80000000
#define REG_EMPTY_PEEK_SHIFT    31
#define REG_EMPTY_PEEK_WIDTH    1

/* AUTO_NO_TX0_ENA */
#define REG_AUTO_NO_TX0_ENA          0x0528
#define REG_AUTO_NO_TX0_ENA_MASK     0x00000001
#define REG_AUTO_NO_TX0_ENA_SHIFT    0
#define REG_AUTO_NO_TX0_ENA_WIDTH    1

/* AUTO_NO_TX1_ENA */
#define REG_AUTO_NO_TX1_ENA          0x0528
#define REG_AUTO_NO_TX1_ENA_MASK     0x00000002
#define REG_AUTO_NO_TX1_ENA_SHIFT    1
#define REG_AUTO_NO_TX1_ENA_WIDTH    1

/* ENABLE_EXTRA_ROWS */
#define REG_ENABLE_EXTRA_ROWS          0x0528
#define REG_ENABLE_EXTRA_ROWS_MASK     0x00000004
#define REG_ENABLE_EXTRA_ROWS_SHIFT    2
#define REG_ENABLE_EXTRA_ROWS_WIDTH    1

/* ENABLE_TXC_PTCS_STATUS */
#define REG_ENABLE_TXC_PTCS_STATUS          0x0528
#define REG_ENABLE_TXC_PTCS_STATUS_MASK     0x00000008
#define REG_ENABLE_TXC_PTCS_STATUS_SHIFT    3
#define REG_ENABLE_TXC_PTCS_STATUS_WIDTH    1

/* NTD_STATE */
#define REG_NTD_STATE          0x0528
#define REG_NTD_STATE_MASK     0x00001f00
#define REG_NTD_STATE_SHIFT    8
#define REG_NTD_STATE_WIDTH    5

/* RX_LOW_REQ */
#define REG_RX_LOW_REQ          0x0528
#define REG_RX_LOW_REQ_MASK     0x001f0000
#define REG_RX_LOW_REQ_SHIFT    16
#define REG_RX_LOW_REQ_WIDTH    5

/* RESP_TIMEOUT_REQ */
#define REG_RESP_TIMEOUT_REQ          0x0528
#define REG_RESP_TIMEOUT_REQ_MASK     0x00200000
#define REG_RESP_TIMEOUT_REQ_SHIFT    21
#define REG_RESP_TIMEOUT_REQ_WIDTH    1

/* TSF_HW1_REQ */
#define REG_TSF_HW1_REQ          0x0528
#define REG_TSF_HW1_REQ_MASK     0x00400000
#define REG_TSF_HW1_REQ_SHIFT    22
#define REG_TSF_HW1_REQ_WIDTH    1

/* TSF_HW0_REQ */
#define REG_TSF_HW0_REQ          0x0528
#define REG_TSF_HW0_REQ_MASK     0x00800000
#define REG_TSF_HW0_REQ_SHIFT    23
#define REG_TSF_HW0_REQ_WIDTH    1

/* RX_HIGH_REQ */
#define REG_RX_HIGH_REQ          0x0528
#define REG_RX_HIGH_REQ_MASK     0x1f000000
#define REG_RX_HIGH_REQ_SHIFT    24
#define REG_RX_HIGH_REQ_WIDTH    5

/* RX_EVENT_RESP_AUTO_DISABLE */
#define REG_RX_EVENT_RESP_AUTO_DISABLE          0x052c
#define REG_RX_EVENT_RESP_AUTO_DISABLE_MASK     0x3fffffff
#define REG_RX_EVENT_RESP_AUTO_DISABLE_SHIFT    0
#define REG_RX_EVENT_RESP_AUTO_DISABLE_WIDTH    30

/* RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTA */
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTA          0x052c
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTA_MASK     0x40000000
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTA_SHIFT    30
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTA_WIDTH    1

/* RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTB */
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTB          0x052c
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTB_MASK     0x80000000
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTB_SHIFT    31
#define REG_RX_EVENT_RESP_AUTO_DISABLE_TIMEOUTB_WIDTH    1

/* EVENT_STATUS */
#define REG_EVENT_STATUS          0x0530
#define REG_EVENT_STATUS_MASK     0x00000001
#define REG_EVENT_STATUS_SHIFT    0
#define REG_EVENT_STATUS_WIDTH    1

/* STATUS_FIFO_WRITE_ADDR */
#define REG_STATUS_FIFO_WRITE_ADDR          0x0534
#define REG_STATUS_FIFO_WRITE_ADDR_MASK     0x0000000f
#define REG_STATUS_FIFO_WRITE_ADDR_SHIFT    0
#define REG_STATUS_FIFO_WRITE_ADDR_WIDTH    4

/* STATUS_FIFO_READ_ADDR */
#define REG_STATUS_FIFO_READ_ADDR          0x0534
#define REG_STATUS_FIFO_READ_ADDR_MASK     0x000000f0
#define REG_STATUS_FIFO_READ_ADDR_SHIFT    4
#define REG_STATUS_FIFO_READ_ADDR_WIDTH    4

/* STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER */
#define REG_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER          0x0538
#define REG_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER_MASK     0x000fffff
#define REG_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER_SHIFT    0
#define REG_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER_WIDTH    20

/* PTCS_POINTER_TABLE_BASE */
/* Start address of a table in shared memory, comprising one 32-bit pointer to a PTCS for each event or request code from the NTD block. */
#define REG_PTCS_POINTER_TABLE_BASE          0x0600
#define REG_PTCS_POINTER_TABLE_BASE_MASK     0x003fff00
#define REG_PTCS_POINTER_TABLE_BASE_SHIFT    8
#define REG_PTCS_POINTER_TABLE_BASE_WIDTH    14

/* TXC_STATE */
/* Diagnostic observation of the state machine */
#define REG_TXC_STATE          0x0604
#define REG_TXC_STATE_MASK     0x0000001f
#define REG_TXC_STATE_SHIFT    0
#define REG_TXC_STATE_WIDTH    5

/* IFS_COMPLETE */
/* IFS_complete flag */
#define REG_IFS_COMPLETE          0x0604
#define REG_IFS_COMPLETE_MASK     0x00000040
#define REG_IFS_COMPLETE_SHIFT    6
#define REG_IFS_COMPLETE_WIDTH    1

/* BACKOFF_COMPLETE */
/* Backoff_complete flag (non EDCF) */
#define REG_BACKOFF_COMPLETE          0x0604
#define REG_BACKOFF_COMPLETE_MASK     0x00000080
#define REG_BACKOFF_COMPLETE_SHIFT    7
#define REG_BACKOFF_COMPLETE_WIDTH    1

/* PTCS_INDEX */
/* Which PTCS is currently being processed or most recently completed */
#define REG_PTCS_INDEX          0x0604
#define REG_PTCS_INDEX_MASK     0x00003f00
#define REG_PTCS_INDEX_SHIFT    8
#define REG_PTCS_INDEX_WIDTH    6

/* LAST_RX_CRC_ERROR */
/* Last received frame had a CRC error */
#define REG_LAST_RX_CRC_ERROR          0x0604
#define REG_LAST_RX_CRC_ERROR_MASK     0x00004000
#define REG_LAST_RX_CRC_ERROR_SHIFT    14
#define REG_LAST_RX_CRC_ERROR_WIDTH    1

/* MAC_CLEAR_CHAN_ASSESS */
/* Set when MAC believes the channel to be busy.  Derived from NAV, RXC, TXC and PHY CCA. */
#define REG_MAC_CLEAR_CHAN_ASSESS          0x0604
#define REG_MAC_CLEAR_CHAN_ASSESS_MASK     0x00008000
#define REG_MAC_CLEAR_CHAN_ASSESS_SHIFT    15
#define REG_MAC_CLEAR_CHAN_ASSESS_WIDTH    1

/* TXC_PRIORITY */
/* Priority currently being serviced */
#define REG_TXC_PRIORITY          0x0604
#define REG_TXC_PRIORITY_MASK     0x00030000
#define REG_TXC_PRIORITY_SHIFT    16
#define REG_TXC_PRIORITY_WIDTH    2

/* EDCF_BACKOFF_COMPLETE */
/* Backoff complete for each EDCF priority */
#define REG_EDCF_BACKOFF_COMPLETE          0x0604
#define REG_EDCF_BACKOFF_COMPLETE_MASK     0x00f00000
#define REG_EDCF_BACKOFF_COMPLETE_SHIFT    20
#define REG_EDCF_BACKOFF_COMPLETE_WIDTH    4

/* AIFS_COMPLETE */
/* AIFS complete for each EDCF priority */
#define REG_AIFS_COMPLETE          0x0604
#define REG_AIFS_COMPLETE_MASK     0x3c000000
#define REG_AIFS_COMPLETE_SHIFT    26
#define REG_AIFS_COMPLETE_WIDTH    4

/* EDCF_PRIORITY_ENABLE */
/* Selects which EDCF priorities use the acitive PTCS (rather than default) */
#define REG_EDCF_PRIORITY_ENABLE          0x0608
#define REG_EDCF_PRIORITY_ENABLE_MASK     0x0000000f
#define REG_EDCF_PRIORITY_ENABLE_SHIFT    0
#define REG_EDCF_PRIORITY_ENABLE_WIDTH    4

/* ALT_BACKOFF_0 */
/* Backoff to be used when priority 0 defers because of an internal collision */
#define REG_ALT_BACKOFF_0          0x060c
#define REG_ALT_BACKOFF_0_MASK     0x000003ff
#define REG_ALT_BACKOFF_0_SHIFT    0
#define REG_ALT_BACKOFF_0_WIDTH    10

/* ALT_BACKOFF_1 */
/* Backoff to be used when priority 1 defers because of an internal collision */
#define REG_ALT_BACKOFF_1          0x0610
#define REG_ALT_BACKOFF_1_MASK     0x000003ff
#define REG_ALT_BACKOFF_1_SHIFT    0
#define REG_ALT_BACKOFF_1_WIDTH    10

/* ALT_BACKOFF_2 */
/* Backoff to be used when priority 2 defers because of an internal collision */
#define REG_ALT_BACKOFF_2          0x0614
#define REG_ALT_BACKOFF_2_MASK     0x000003ff
#define REG_ALT_BACKOFF_2_SHIFT    0
#define REG_ALT_BACKOFF_2_WIDTH    10

/* EDCF_CONTEXT */
#define REG_EDCF_CONTEXT          0x0618
#define REG_EDCF_CONTEXT_MASK     0x0000000f
#define REG_EDCF_CONTEXT_SHIFT    0
#define REG_EDCF_CONTEXT_WIDTH    4

/* EDCF_PRIORITY_ENABLE_SET */
#define REG_EDCF_PRIORITY_ENABLE_SET          0x061c
#define REG_EDCF_PRIORITY_ENABLE_SET_MASK     0x0000000f
#define REG_EDCF_PRIORITY_ENABLE_SET_SHIFT    0
#define REG_EDCF_PRIORITY_ENABLE_SET_WIDTH    4

/* EDCF_PRIORITY_ENABLE_CLEAR */
#define REG_EDCF_PRIORITY_ENABLE_CLEAR          0x0620
#define REG_EDCF_PRIORITY_ENABLE_CLEAR_MASK     0x0000000f
#define REG_EDCF_PRIORITY_ENABLE_CLEAR_SHIFT    0
#define REG_EDCF_PRIORITY_ENABLE_CLEAR_WIDTH    4

/* WME_REVISION_MINOR */
#define REG_WME_REVISION_MINOR          0x062c
#define REG_WME_REVISION_MINOR_MASK     0x0000000f
#define REG_WME_REVISION_MINOR_SHIFT    0
#define REG_WME_REVISION_MINOR_WIDTH    4

/* WME_REVISION_MAJOR */
#define REG_WME_REVISION_MAJOR          0x062c
#define REG_WME_REVISION_MAJOR_MASK     0x000000f0
#define REG_WME_REVISION_MAJOR_SHIFT    4
#define REG_WME_REVISION_MAJOR_WIDTH    4

/* RXC_DEBUG_OVERRIDE_TXC */
#define REG_RXC_DEBUG_OVERRIDE_TXC          0x0630
#define REG_RXC_DEBUG_OVERRIDE_TXC_MASK     0x00000003
#define REG_RXC_DEBUG_OVERRIDE_TXC_SHIFT    0
#define REG_RXC_DEBUG_OVERRIDE_TXC_WIDTH    2

/* IFS_CRC_ERR_FIX_ENABLE */
#define REG_IFS_CRC_ERR_FIX_ENABLE          0x0634
#define REG_IFS_CRC_ERR_FIX_ENABLE_MASK     0x00000001
#define REG_IFS_CRC_ERR_FIX_ENABLE_SHIFT    0
#define REG_IFS_CRC_ERR_FIX_ENABLE_WIDTH    1

/* TSF_FAST_DIVIDE */
/* Number of fast clock periods per microsecond - 1 */
#define REG_TSF_FAST_DIVIDE          0x0700
#define REG_TSF_FAST_DIVIDE_MASK     0x000000ff
#define REG_TSF_FAST_DIVIDE_SHIFT    0
#define REG_TSF_FAST_DIVIDE_WIDTH    8

/* TSF_SLOW_FRAC_INCR */
/* Slow prescaler integer increment per clock */
#define REG_TSF_SLOW_FRAC_INCR          0x0700
#define REG_TSF_SLOW_FRAC_INCR_MASK     0x0001ff00
#define REG_TSF_SLOW_FRAC_INCR_SHIFT    8
#define REG_TSF_SLOW_FRAC_INCR_WIDTH    9

/* TSF_SLOW_INT_INCR */
/* Slow prescaler fractional increment per clock */
#define REG_TSF_SLOW_INT_INCR          0x0700
#define REG_TSF_SLOW_INT_INCR_MASK     0x007e0000
#define REG_TSF_SLOW_INT_INCR_SHIFT    17
#define REG_TSF_SLOW_INT_INCR_WIDTH    6

/* NAV_FAST_DIVIDE */
/* Number of fast clock periods per microsecond - 1 */
#define REG_NAV_FAST_DIVIDE          0x0700
#define REG_NAV_FAST_DIVIDE_MASK     0xff000000
#define REG_NAV_FAST_DIVIDE_SHIFT    24
#define REG_NAV_FAST_DIVIDE_WIDTH    8

/* NAV_SLOW_FRAC_INCR */
/* Timers slow prescaler fractional increment per clock */
#define REG_NAV_SLOW_FRAC_INCR          0x0704
#define REG_NAV_SLOW_FRAC_INCR_MASK     0x000001ff
#define REG_NAV_SLOW_FRAC_INCR_SHIFT    0
#define REG_NAV_SLOW_FRAC_INCR_WIDTH    9

/* NAV_SLOW_INT_INCR */
/* Timers slow prescaler integer increment per clock */
#define REG_NAV_SLOW_INT_INCR          0x0704
#define REG_NAV_SLOW_INT_INCR_MASK     0x00007e00
#define REG_NAV_SLOW_INT_INCR_SHIFT    9
#define REG_NAV_SLOW_INT_INCR_WIDTH    6

/* IFS_DIVIDE */
/* IFS timer prescaler divide ratio */
#define REG_IFS_DIVIDE          0x0704
#define REG_IFS_DIVIDE_MASK     0x00ff0000
#define REG_IFS_DIVIDE_SHIFT    16
#define REG_IFS_DIVIDE_WIDTH    8

/* SLOT_DIVIDE */
/* Slot timer prescaler divide ratio */
#define REG_SLOT_DIVIDE          0x0704
#define REG_SLOT_DIVIDE_MASK     0x1f000000
#define REG_SLOT_DIVIDE_SHIFT    24
#define REG_SLOT_DIVIDE_WIDTH    5

/* TIME_TSF_HW_EVENT0 */
/* TSF time at which TSF HW event 0 occurs, up to 2^23 us after current TSF */
#define REG_TIME_TSF_HW_EVENT0          0x0708
#define REG_TIME_TSF_HW_EVENT0_MASK     0x00ffffff
#define REG_TIME_TSF_HW_EVENT0_SHIFT    0
#define REG_TIME_TSF_HW_EVENT0_WIDTH    24

/* HW_ENABLETSF_HW_EVENT0 */
/* Enable hardware event 0 */
#define REG_HW_ENABLETSF_HW_EVENT0          0x0708
#define REG_HW_ENABLETSF_HW_EVENT0_MASK     0x40000000
#define REG_HW_ENABLETSF_HW_EVENT0_SHIFT    30
#define REG_HW_ENABLETSF_HW_EVENT0_WIDTH    1

/* SW_ENABLE_TSF_HW_EVENT0 */
/* Enable software event 0 */
#define REG_SW_ENABLE_TSF_HW_EVENT0          0x0708
#define REG_SW_ENABLE_TSF_HW_EVENT0_MASK     0x80000000
#define REG_SW_ENABLE_TSF_HW_EVENT0_SHIFT    31
#define REG_SW_ENABLE_TSF_HW_EVENT0_WIDTH    1

/* TIME_TSF_HW_EVENT1 */
/* TSF time at which TSF HW event 1 occurs, up to 2^23 us after current TSF */
#define REG_TIME_TSF_HW_EVENT1          0x070c
#define REG_TIME_TSF_HW_EVENT1_MASK     0x00ffffff
#define REG_TIME_TSF_HW_EVENT1_SHIFT    0
#define REG_TIME_TSF_HW_EVENT1_WIDTH    24

/* HW_ENABLETSF_HW_EVENT1 */
/* Enable hardware event 1 */
#define REG_HW_ENABLETSF_HW_EVENT1          0x070c
#define REG_HW_ENABLETSF_HW_EVENT1_MASK     0x40000000
#define REG_HW_ENABLETSF_HW_EVENT1_SHIFT    30
#define REG_HW_ENABLETSF_HW_EVENT1_WIDTH    1

/* SW_ENABLE_TSF_HW_EVENT1 */
/* Enable software event 1 */
#define REG_SW_ENABLE_TSF_HW_EVENT1          0x070c
#define REG_SW_ENABLE_TSF_HW_EVENT1_MASK     0x80000000
#define REG_SW_ENABLE_TSF_HW_EVENT1_SHIFT    31
#define REG_SW_ENABLE_TSF_HW_EVENT1_WIDTH    1

/* TIME_TSF_REQ_SWITCH */
/* TSF time at which the request switch occurs. */
#define REG_TIME_TSF_REQ_SWITCH          0x0710
#define REG_TIME_TSF_REQ_SWITCH_MASK     0x00ffffff
#define REG_TIME_TSF_REQ_SWITCH_SHIFT    0
#define REG_TIME_TSF_REQ_SWITCH_WIDTH    24

/* TO_1 */
/* 0 - Switch to queue 0; 1 - Switch to queue 1 */
#define REG_TO_1          0x0710
#define REG_TO_1_MASK     0x20000000
#define REG_TO_1_SHIFT    29
#define REG_TO_1_WIDTH    1

/* HW_ENABLE_TSF_REQ_SWITCH */
/* Enable hardware event */
#define REG_HW_ENABLE_TSF_REQ_SWITCH          0x0710
#define REG_HW_ENABLE_TSF_REQ_SWITCH_MASK     0x40000000
#define REG_HW_ENABLE_TSF_REQ_SWITCH_SHIFT    30
#define REG_HW_ENABLE_TSF_REQ_SWITCH_WIDTH    1

/* SW_ENABLE_TSF_REQ_SWITCH */
/* Enable software event 4 */
#define REG_SW_ENABLE_TSF_REQ_SWITCH          0x0710
#define REG_SW_ENABLE_TSF_REQ_SWITCH_MASK     0x80000000
#define REG_SW_ENABLE_TSF_REQ_SWITCH_SHIFT    31
#define REG_SW_ENABLE_TSF_REQ_SWITCH_WIDTH    1

/* TIME_TSF_SW_EVENT2 */
/* TSF time at which TSF SW event 2 occurs. */
#define REG_TIME_TSF_SW_EVENT2          0x0714
#define REG_TIME_TSF_SW_EVENT2_MASK     0x00ffffff
#define REG_TIME_TSF_SW_EVENT2_SHIFT    0
#define REG_TIME_TSF_SW_EVENT2_WIDTH    24

/* ENABLE_TSF_SW_EVENT2 */
/* Enable software event 2 */
#define REG_ENABLE_TSF_SW_EVENT2          0x0714
#define REG_ENABLE_TSF_SW_EVENT2_MASK     0x80000000
#define REG_ENABLE_TSF_SW_EVENT2_SHIFT    31
#define REG_ENABLE_TSF_SW_EVENT2_WIDTH    1

/* TIME_TSF_SW_EVENT3 */
/* TSF time at which TSF SW event 3 occurs. */
#define REG_TIME_TSF_SW_EVENT3          0x0718
#define REG_TIME_TSF_SW_EVENT3_MASK     0x00ffffff
#define REG_TIME_TSF_SW_EVENT3_SHIFT    0
#define REG_TIME_TSF_SW_EVENT3_WIDTH    24

/* ENABLE_TSF_SW_EVENT3 */
/* Enable software event 3 */
#define REG_ENABLE_TSF_SW_EVENT3          0x0718
#define REG_ENABLE_TSF_SW_EVENT3_MASK     0x80000000
#define REG_ENABLE_TSF_SW_EVENT3_SHIFT    31
#define REG_ENABLE_TSF_SW_EVENT3_WIDTH    1

/* TIME_TSF_NAV_LOAD */
/* TSF time at which TSF NAV load event occurs. */
#define REG_TIME_TSF_NAV_LOAD          0x071c
#define REG_TIME_TSF_NAV_LOAD_MASK     0x00ffffff
#define REG_TIME_TSF_NAV_LOAD_SHIFT    0
#define REG_TIME_TSF_NAV_LOAD_WIDTH    24

/* HW_ENABLE_TSF_NAV_LOAD */
/* Enable hardware event */
#define REG_HW_ENABLE_TSF_NAV_LOAD          0x071c
#define REG_HW_ENABLE_TSF_NAV_LOAD_MASK     0x40000000
#define REG_HW_ENABLE_TSF_NAV_LOAD_SHIFT    30
#define REG_HW_ENABLE_TSF_NAV_LOAD_WIDTH    1

/* RX_NAV_LOAD_DURATION */
/* Duration loaded into the NAV timer when a Rx event with "load NAV from Rx NAV load duration register" enabled. */
#define REG_RX_NAV_LOAD_DURATION          0x0720
#define REG_RX_NAV_LOAD_DURATION_MASK     0x0000ffff
#define REG_RX_NAV_LOAD_DURATION_SHIFT    0
#define REG_RX_NAV_LOAD_DURATION_WIDTH    16

/* TSF_NAV_LOAD_DURATION */
/* Value to be loaded into the NAV timer when the TSF NAV load event occurs or when a Rx event occurs with load NAV from TSF NAV load duration register . */
#define REG_TSF_NAV_LOAD_DURATION          0x0724
#define REG_TSF_NAV_LOAD_DURATION_MASK     0x0000ffff
#define REG_TSF_NAV_LOAD_DURATION_SHIFT    0
#define REG_TSF_NAV_LOAD_DURATION_WIDTH    16

/* TIME_RXD_TSF_COMP */
/* TSF time to compare with TSF Timer */
#define REG_TIME_RXD_TSF_COMP          0x0728
#define REG_TIME_RXD_TSF_COMP_MASK     0x00ffffff
#define REG_TIME_RXD_TSF_COMP_SHIFT    0
#define REG_TIME_RXD_TSF_COMP_WIDTH    24

/* ENABLE_RXD_TSF_COMP */
/* Set for enabled. Reset for disabled. Set and reset by software. */
#define REG_ENABLE_RXD_TSF_COMP          0x0728
#define REG_ENABLE_RXD_TSF_COMP_MASK     0x80000000
#define REG_ENABLE_RXD_TSF_COMP_SHIFT    31
#define REG_ENABLE_RXD_TSF_COMP_WIDTH    1

/* PERIOD_TICK_TIMER */
/* Tick timer period in microseconds */
#define REG_PERIOD_TICK_TIMER          0x072c
#define REG_PERIOD_TICK_TIMER_MASK     0x0000ffff
#define REG_PERIOD_TICK_TIMER_SHIFT    0
#define REG_PERIOD_TICK_TIMER_WIDTH    16

/* ENABLE_TICK_TIMER */
/* Set for enabled. Reset for disabled. Set and reset by software. */
#define REG_ENABLE_TICK_TIMER          0x072c
#define REG_ENABLE_TICK_TIMER_MASK     0x80000000
#define REG_ENABLE_TICK_TIMER_SHIFT    31
#define REG_ENABLE_TICK_TIMER_WIDTH    1

/* PERIOD_SLOT_TIMER */
/* Set to (8 * slot period) - 1, slot period in microseconds */
#define REG_PERIOD_SLOT_TIMER          0x0730
#define REG_PERIOD_SLOT_TIMER_MASK     0x000003ff
#define REG_PERIOD_SLOT_TIMER_SHIFT    0
#define REG_PERIOD_SLOT_TIMER_WIDTH    10

/* NAV_ENABLE */
/* When set the NAV timer is enabled and it will decrement every 1 us until it reaches zero. When reset the NAV timer is disabled and the value will not change. */
#define REG_NAV_ENABLE          0x0734
#define REG_NAV_ENABLE_MASK     0x00000001
#define REG_NAV_ENABLE_SHIFT    0
#define REG_NAV_ENABLE_WIDTH    1

/* TSF_ENABLE */
/* When set the TSF timer is enabled and it will increment every 1 us, wrapping (resetting to zero) when it reaches it's maximum value. When reset the TSF timer is disabled and the software can load the TSF timer by writing to the TSF timer low and TSF timer high registers. */
#define REG_TSF_ENABLE          0x0734
#define REG_TSF_ENABLE_MASK     0x00000002
#define REG_TSF_ENABLE_SHIFT    1
#define REG_TSF_ENABLE_WIDTH    1

/* TSF_NUDGE */
/* Setting this bit causes the TSF timer to be adjusted (nudged) by TSF adjustment (a small signed value). This bit set by the software and is reset by the hardware when the adjustment is complete. */
#define REG_TSF_NUDGE          0x0734
#define REG_TSF_NUDGE_MASK     0x00000004
#define REG_TSF_NUDGE_SHIFT    2
#define REG_TSF_NUDGE_WIDTH    1

/* INTEGRATE_TX_TIME_TO_QBSS */
#define REG_INTEGRATE_TX_TIME_TO_QBSS          0x0734
#define REG_INTEGRATE_TX_TIME_TO_QBSS_MASK     0x00000020
#define REG_INTEGRATE_TX_TIME_TO_QBSS_SHIFT    5
#define REG_INTEGRATE_TX_TIME_TO_QBSS_WIDTH    1

/* INTEGRATE_NAV */
#define REG_INTEGRATE_NAV          0x0734
#define REG_INTEGRATE_NAV_MASK     0x00000040
#define REG_INTEGRATE_NAV_SHIFT    6
#define REG_INTEGRATE_NAV_WIDTH    1

/* INTEGRATE_CCA */
#define REG_INTEGRATE_CCA          0x0734
#define REG_INTEGRATE_CCA_MASK     0x00000080
#define REG_INTEGRATE_CCA_SHIFT    7
#define REG_INTEGRATE_CCA_WIDTH    1

/* TSF_ADJ */
/* Signed TSF adjustment value, a negative value puts the TSF time back, a positive value puts the TSF time forwards. */
#define REG_TSF_ADJ          0x0734
#define REG_TSF_ADJ_MASK     0x0001ff00
#define REG_TSF_ADJ_SHIFT    8
#define REG_TSF_ADJ_WIDTH    9

/* TSF_SW_EVENT0 */
/* Set by hardware when tsf_sw_event0 occurs.  Write '1' to clear the sw_enable bit in the tsf_hw_event0 register, thus clearing the event. */
#define REG_TSF_SW_EVENT0          0x0734
#define REG_TSF_SW_EVENT0_MASK     0x01000000
#define REG_TSF_SW_EVENT0_SHIFT    24
#define REG_TSF_SW_EVENT0_WIDTH    1

/* TSF_SW_EVENT1 */
/* Set by hardware when tsf_sw_event1 occurs.  Write '1' to clear the sw_enable bit in the tsf_hw_event1 register, thus clearing the event. */
#define REG_TSF_SW_EVENT1          0x0734
#define REG_TSF_SW_EVENT1_MASK     0x02000000
#define REG_TSF_SW_EVENT1_SHIFT    25
#define REG_TSF_SW_EVENT1_WIDTH    1

/* TSF_SW_EVENT2 */
/* Set by hardware when tsf_sw_event2 occurs.  Write '1' to clear the sw_enable bit in the tsf_sw_event2 register, thus clearing the event. */
#define REG_TSF_SW_EVENT2          0x0734
#define REG_TSF_SW_EVENT2_MASK     0x04000000
#define REG_TSF_SW_EVENT2_SHIFT    26
#define REG_TSF_SW_EVENT2_WIDTH    1

/* TSF_SW_EVENT3 */
/* Set by hardware when tsf_sw_event3 occurs.  Write '1' to clear the sw_enable bit in the tsf_sw_event3 register, thus clearing the event. */
#define REG_TSF_SW_EVENT3          0x0734
#define REG_TSF_SW_EVENT3_MASK     0x08000000
#define REG_TSF_SW_EVENT3_SHIFT    27
#define REG_TSF_SW_EVENT3_WIDTH    1

/* TSF_SW_EVENT4 */
/* Set by hardware when tsf_sw_event4 (TSF request switch) occurs.  Write '1' to clear the sw_enable bit in the tsf_req_switch register, thus clearing the event. */
#define REG_TSF_SW_EVENT4          0x0734
#define REG_TSF_SW_EVENT4_MASK     0x10000000
#define REG_TSF_SW_EVENT4_SHIFT    28
#define REG_TSF_SW_EVENT4_WIDTH    1

/* TSF_TIMER_LOW_RPORT */
/* Bits 31:0 of the TSF Timer.  Can only be written when timer_control.tsf_enable is zero. */
#define REG_TSF_TIMER_LOW_RPORT          0x0738
#define REG_TSF_TIMER_LOW_RPORT_MASK     0xffffffff
#define REG_TSF_TIMER_LOW_RPORT_SHIFT    0
#define REG_TSF_TIMER_LOW_RPORT_WIDTH    32

/* TSF_TIMER_HIGH_RPORT */
/* Bits 63:32 of the TSF Timer.  Can only be written when timer_control.tsf_enable is zero. */
#define REG_TSF_TIMER_HIGH_RPORT          0x073c
#define REG_TSF_TIMER_HIGH_RPORT_MASK     0xffffffff
#define REG_TSF_TIMER_HIGH_RPORT_SHIFT    0
#define REG_TSF_TIMER_HIGH_RPORT_WIDTH    32

/* NAV_TIMER */
/* NAV timer */
#define REG_NAV_TIMER          0x0740
#define REG_NAV_TIMER_MASK     0x0000ffff
#define REG_NAV_TIMER_SHIFT    0
#define REG_NAV_TIMER_WIDTH    16

/* TIMER_TICK */
/* Tick timer */
#define REG_TIMER_TICK          0x0744
#define REG_TIMER_TICK_MASK     0x0000ffff
#define REG_TIMER_TICK_SHIFT    0
#define REG_TIMER_TICK_WIDTH    16

/* TIMER_SLOT */
/* Slot timer */
#define REG_TIMER_SLOT          0x0748
#define REG_TIMER_SLOT_MASK     0x000003ff
#define REG_TIMER_SLOT_SHIFT    0
#define REG_TIMER_SLOT_WIDTH    10

/* BACKOFF_COUNTER */
/* Backoff counter */
#define REG_BACKOFF_COUNTER          0x074c
#define REG_BACKOFF_COUNTER_MASK     0x000003ff
#define REG_BACKOFF_COUNTER_SHIFT    0
#define REG_BACKOFF_COUNTER_WIDTH    10

/* IFS_TIMER */
/* IFS timer */
#define REG_IFS_TIMER          0x0750
#define REG_IFS_TIMER_MASK     0x00001fff
#define REG_IFS_TIMER_SHIFT    0
#define REG_IFS_TIMER_WIDTH    13

/* RESP_TIMER */
/* Response timer */
#define REG_RESP_TIMER          0x0754
#define REG_RESP_TIMER_MASK     0x00001fff
#define REG_RESP_TIMER_SHIFT    0
#define REG_RESP_TIMER_WIDTH    13

/* TSF_HW_EVENT2 */
#define REG_TSF_HW_EVENT2          0x0768
#define REG_TSF_HW_EVENT2_MASK     0x00ffffff
#define REG_TSF_HW_EVENT2_SHIFT    0
#define REG_TSF_HW_EVENT2_WIDTH    24

/* QBSS_LOAD_INTEGRATOR */
#define REG_QBSS_LOAD_INTEGRATOR          0x076c
#define REG_QBSS_LOAD_INTEGRATOR_MASK     0x000fffff
#define REG_QBSS_LOAD_INTEGRATOR_SHIFT    0
#define REG_QBSS_LOAD_INTEGRATOR_WIDTH    20

/* TXOP_LIMIT_COUNTER */
#define REG_TXOP_LIMIT_COUNTER          0x0770
#define REG_TXOP_LIMIT_COUNTER_MASK     0x00003fff
#define REG_TXOP_LIMIT_COUNTER_SHIFT    0
#define REG_TXOP_LIMIT_COUNTER_WIDTH    14

/* FAST_CCA_BASED_RESPONSE_TIMEOUT_COUNT */
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_COUNT          0x0774
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_COUNT_MASK     0x00003fff
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_COUNT_SHIFT    0
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_COUNT_WIDTH    14

/* FAST_CCA_BASED_RESPONSE_TIMEOUT_ENABLE */
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_ENABLE          0x0774
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_ENABLE_MASK     0x00004000
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_ENABLE_SHIFT    14
#define REG_FAST_CCA_BASED_RESPONSE_TIMEOUT_ENABLE_WIDTH    1

/* LSP_ENABLE_MODE_MCS */
#define REG_LSP_ENABLE_MODE_MCS          0x0778
#define REG_LSP_ENABLE_MODE_MCS_MASK     0x00000001
#define REG_LSP_ENABLE_MODE_MCS_SHIFT    0
#define REG_LSP_ENABLE_MODE_MCS_WIDTH    1

/* LSP_ENABLE_MODE_FCS */
#define REG_LSP_ENABLE_MODE_FCS          0x0778
#define REG_LSP_ENABLE_MODE_FCS_MASK     0x00000002
#define REG_LSP_ENABLE_MODE_FCS_SHIFT    1
#define REG_LSP_ENABLE_MODE_FCS_WIDTH    1

/* LSP_MCS_EVENT_CTR */
/* Counter is cleared by read */
#define REG_LSP_MCS_EVENT_CTR          0x077c
#define REG_LSP_MCS_EVENT_CTR_MASK     0x0000ffff
#define REG_LSP_MCS_EVENT_CTR_SHIFT    0
#define REG_LSP_MCS_EVENT_CTR_WIDTH    16

/* LSP_FCS_EVENT_CTR */
/* Counter is cleared by read */
#define REG_LSP_FCS_EVENT_CTR          0x0780
#define REG_LSP_FCS_EVENT_CTR_MASK     0x0000ffff
#define REG_LSP_FCS_EVENT_CTR_SHIFT    0
#define REG_LSP_FCS_EVENT_CTR_WIDTH    16

/* LSP_DURATION */
#define REG_LSP_DURATION          0x0784
#define REG_LSP_DURATION_MASK     0x0000ffff
#define REG_LSP_DURATION_SHIFT    0
#define REG_LSP_DURATION_WIDTH    16

/* REDUCED_FREQ_PRESCALERS */
#define REG_REDUCED_FREQ_PRESCALERS          0x0788
#define REG_REDUCED_FREQ_PRESCALERS_MASK     0xffffffff
#define REG_REDUCED_FREQ_PRESCALERS_SHIFT    0
#define REG_REDUCED_FREQ_PRESCALERS_WIDTH    32

/* PRESCALERS_EXTENSION_BIT */
#define REG_PRESCALERS_EXTENSION_BIT          0x078c
#define REG_PRESCALERS_EXTENSION_BIT_MASK     0x0000000f
#define REG_PRESCALERS_EXTENSION_BIT_SHIFT    0
#define REG_PRESCALERS_EXTENSION_BIT_WIDTH    4

/* REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW */
#define REG_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW          0x0790
#define REG_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW_MASK     0xffffffff
#define REG_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW_SHIFT    0
#define REG_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW_WIDTH    32

/* FIX_NAV_LOAD_AGG */
/* NAV load fix at aggregates with last bad subframe */
#define REG_FIX_NAV_LOAD_AGG          0x0794
#define REG_FIX_NAV_LOAD_AGG_MASK     0x00000001
#define REG_FIX_NAV_LOAD_AGG_SHIFT    0
#define REG_FIX_NAV_LOAD_AGG_WIDTH    1

/* RX_DUR_TIMER */
#define REG_RX_DUR_TIMER          0x0798
#define REG_RX_DUR_TIMER_MASK     0x000000ff
#define REG_RX_DUR_TIMER_SHIFT    0
#define REG_RX_DUR_TIMER_WIDTH    8

/* DELIA_TIMER */
#define REG_DELIA_TIMER          0x079c
#define REG_DELIA_TIMER_MASK     0x00ffffff
#define REG_DELIA_TIMER_SHIFT    0
#define REG_DELIA_TIMER_WIDTH    24

/* DELIA_MAX_LIMIT_TIMER */
/* delia max limit timer for compare */
#define REG_DELIA_MAX_LIMIT_TIMER          0x07a0
#define REG_DELIA_MAX_LIMIT_TIMER_MASK     0x00ffffff
#define REG_DELIA_MAX_LIMIT_TIMER_SHIFT    0
#define REG_DELIA_MAX_LIMIT_TIMER_WIDTH    24

/* BF_ENABLE */
#define REG_BF_ENABLE          0x0800
#define REG_BF_ENABLE_MASK     0x00000001
#define REG_BF_ENABLE_SHIFT    0
#define REG_BF_ENABLE_WIDTH    1

/* AGING_NONSTANDARD_BF_ENABLE */
#define REG_AGING_NONSTANDARD_BF_ENABLE          0x0800
#define REG_AGING_NONSTANDARD_BF_ENABLE_MASK     0x00000002
#define REG_AGING_NONSTANDARD_BF_ENABLE_SHIFT    1
#define REG_AGING_NONSTANDARD_BF_ENABLE_WIDTH    1

/* AGING_STANDARD_BF_ENABLE */
#define REG_AGING_STANDARD_BF_ENABLE          0x0800
#define REG_AGING_STANDARD_BF_ENABLE_MASK     0x00000004
#define REG_AGING_STANDARD_BF_ENABLE_SHIFT    2
#define REG_AGING_STANDARD_BF_ENABLE_WIDTH    1

/* BF_DB_BASE_ADDR */
#define REG_BF_DB_BASE_ADDR          0x0804
#define REG_BF_DB_BASE_ADDR_MASK     0x00ffffff
#define REG_BF_DB_BASE_ADDR_SHIFT    0
#define REG_BF_DB_BASE_ADDR_WIDTH    24

/* TX_BFEE_BUFFER_ADDR */
#define REG_TX_BFEE_BUFFER_ADDR          0x0808
#define REG_TX_BFEE_BUFFER_ADDR_MASK     0x00ffffff
#define REG_TX_BFEE_BUFFER_ADDR_SHIFT    0
#define REG_TX_BFEE_BUFFER_ADDR_WIDTH    24

/* MAX_NR_SUPPORTED_20_40_80 */
#define REG_MAX_NR_SUPPORTED_20_40_80          0x080c
#define REG_MAX_NR_SUPPORTED_20_40_80_MASK     0x00000007
#define REG_MAX_NR_SUPPORTED_20_40_80_SHIFT    0
#define REG_MAX_NR_SUPPORTED_20_40_80_WIDTH    3

/* MAX_NR_SUPPORTED_160 */
#define REG_MAX_NR_SUPPORTED_160          0x080c
#define REG_MAX_NR_SUPPORTED_160_MASK     0x00000038
#define REG_MAX_NR_SUPPORTED_160_SHIFT    3
#define REG_MAX_NR_SUPPORTED_160_WIDTH    3

/* STA_DB_BF_WORDS_OFFSET */
#define REG_STA_DB_BF_WORDS_OFFSET          0x0810
#define REG_STA_DB_BF_WORDS_OFFSET_MASK     0x0000001f
#define REG_STA_DB_BF_WORDS_OFFSET_SHIFT    0
#define REG_STA_DB_BF_WORDS_OFFSET_WIDTH    5

/* AGING_MAX_TIMER_THRESHOLD */
#define REG_AGING_MAX_TIMER_THRESHOLD          0x0814
#define REG_AGING_MAX_TIMER_THRESHOLD_MASK     0x0000ffff
#define REG_AGING_MAX_TIMER_THRESHOLD_SHIFT    0
#define REG_AGING_MAX_TIMER_THRESHOLD_WIDTH    16

/* AGING_TIMER_THRESHOLD_FAST */
#define REG_AGING_TIMER_THRESHOLD_FAST          0x0818
#define REG_AGING_TIMER_THRESHOLD_FAST_MASK     0x0000ffff
#define REG_AGING_TIMER_THRESHOLD_FAST_SHIFT    0
#define REG_AGING_TIMER_THRESHOLD_FAST_WIDTH    16

/* AGING_TIMER_THRESHOLD_MEDIUM1 */
#define REG_AGING_TIMER_THRESHOLD_MEDIUM1          0x0818
#define REG_AGING_TIMER_THRESHOLD_MEDIUM1_MASK     0xffff0000
#define REG_AGING_TIMER_THRESHOLD_MEDIUM1_SHIFT    16
#define REG_AGING_TIMER_THRESHOLD_MEDIUM1_WIDTH    16

/* AGING_TIMER_THRESHOLD_MEDIUM2 */
#define REG_AGING_TIMER_THRESHOLD_MEDIUM2          0x081c
#define REG_AGING_TIMER_THRESHOLD_MEDIUM2_MASK     0x0000ffff
#define REG_AGING_TIMER_THRESHOLD_MEDIUM2_SHIFT    0
#define REG_AGING_TIMER_THRESHOLD_MEDIUM2_WIDTH    16

/* AGING_TIMER_THRESHOLD_SLOW */
#define REG_AGING_TIMER_THRESHOLD_SLOW          0x081c
#define REG_AGING_TIMER_THRESHOLD_SLOW_MASK     0xffff0000
#define REG_AGING_TIMER_THRESHOLD_SLOW_SHIFT    16
#define REG_AGING_TIMER_THRESHOLD_SLOW_WIDTH    16

/* AGING_TIMER */
#define REG_AGING_TIMER          0x0820
#define REG_AGING_TIMER_MASK     0x00ffffff
#define REG_AGING_TIMER_SHIFT    0
#define REG_AGING_TIMER_WIDTH    24

/* BFEE_MAC_HDR_LENGTH */
#define REG_BFEE_MAC_HDR_LENGTH          0x0824
#define REG_BFEE_MAC_HDR_LENGTH_MASK     0x0000003f
#define REG_BFEE_MAC_HDR_LENGTH_SHIFT    0
#define REG_BFEE_MAC_HDR_LENGTH_WIDTH    6

/* PHY_RX_DEBUG_MODE */
#define REG_PHY_RX_DEBUG_MODE          0x0828
#define REG_PHY_RX_DEBUG_MODE_MASK     0x00000001
#define REG_PHY_RX_DEBUG_MODE_SHIFT    0
#define REG_PHY_RX_DEBUG_MODE_WIDTH    1

/* PHY_DEBUG_MODE_OVERRIDE_MAC_PHY_SIGNALS */
#define REG_PHY_DEBUG_MODE_OVERRIDE_MAC_PHY_SIGNALS          0x0828
#define REG_PHY_DEBUG_MODE_OVERRIDE_MAC_PHY_SIGNALS_MASK     0x00000002
#define REG_PHY_DEBUG_MODE_OVERRIDE_MAC_PHY_SIGNALS_SHIFT    1
#define REG_PHY_DEBUG_MODE_OVERRIDE_MAC_PHY_SIGNALS_WIDTH    1

/* PHY_DEBUG_STA_TX_MAX_NSS */
#define REG_PHY_DEBUG_STA_TX_MAX_NSS          0x0828
#define REG_PHY_DEBUG_STA_TX_MAX_NSS_MASK     0x00000300
#define REG_PHY_DEBUG_STA_TX_MAX_NSS_SHIFT    8
#define REG_PHY_DEBUG_STA_TX_MAX_NSS_WIDTH    2

/* GROUPING_SRC_SM */
#define REG_GROUPING_SRC_SM          0x082c
#define REG_GROUPING_SRC_SM_MASK     0x00000003
#define REG_GROUPING_SRC_SM_SHIFT    0
#define REG_GROUPING_SRC_SM_WIDTH    2

/* GROUPING_DEST_SM */
#define REG_GROUPING_DEST_SM          0x082c
#define REG_GROUPING_DEST_SM_MASK     0x0000001c
#define REG_GROUPING_DEST_SM_SHIFT    2
#define REG_GROUPING_DEST_SM_WIDTH    3

/* PHY_BF_MATRICES_SM */
#define REG_PHY_BF_MATRICES_SM          0x082c
#define REG_PHY_BF_MATRICES_SM_MASK     0x000000e0
#define REG_PHY_BF_MATRICES_SM_SHIFT    5
#define REG_PHY_BF_MATRICES_SM_WIDTH    3

/* AGING_SM */
#define REG_AGING_SM          0x082c
#define REG_AGING_SM_MASK     0x00000700
#define REG_AGING_SM_SHIFT    8
#define REG_AGING_SM_WIDTH    3

/* BF_MAIN_RX_SM */
#define REG_BF_MAIN_RX_SM          0x082c
#define REG_BF_MAIN_RX_SM_MASK     0x00007800
#define REG_BF_MAIN_RX_SM_SHIFT    11
#define REG_BF_MAIN_RX_SM_WIDTH    4

/* NDPA_NDP_SM */
#define REG_NDPA_NDP_SM          0x082c
#define REG_NDPA_NDP_SM_MASK     0x00018000
#define REG_NDPA_NDP_SM_SHIFT    15
#define REG_NDPA_NDP_SM_WIDTH    2

/* STA_DB_INDEX */
#define REG_STA_DB_INDEX          0x0830
#define REG_STA_DB_INDEX_MASK     0x000000ff
#define REG_STA_DB_INDEX_SHIFT    0
#define REG_STA_DB_INDEX_WIDTH    8

/* BF_DB_TEMP_INDEX */
#define REG_BF_DB_TEMP_INDEX          0x0830
#define REG_BF_DB_TEMP_INDEX_MASK     0x0001ff00
#define REG_BF_DB_TEMP_INDEX_SHIFT    8
#define REG_BF_DB_TEMP_INDEX_WIDTH    9

/* AGING_STA_DB_INDEX */
#define REG_AGING_STA_DB_INDEX          0x0834
#define REG_AGING_STA_DB_INDEX_MASK     0x000000ff
#define REG_AGING_STA_DB_INDEX_SHIFT    0
#define REG_AGING_STA_DB_INDEX_WIDTH    8

/* AGING_BF_DB_INDEX */
#define REG_AGING_BF_DB_INDEX          0x0834
#define REG_AGING_BF_DB_INDEX_MASK     0x0001ff00
#define REG_AGING_BF_DB_INDEX_SHIFT    8
#define REG_AGING_BF_DB_INDEX_WIDTH    9

/* AGING_TIMER_EXPIRED */
#define REG_AGING_TIMER_EXPIRED          0x0834
#define REG_AGING_TIMER_EXPIRED_MASK     0x01000000
#define REG_AGING_TIMER_EXPIRED_SHIFT    24
#define REG_AGING_TIMER_EXPIRED_WIDTH    1

/* TSF_AGING_THRESHOLD_EXPIRED */
#define REG_TSF_AGING_THRESHOLD_EXPIRED          0x0834
#define REG_TSF_AGING_THRESHOLD_EXPIRED_MASK     0x02000000
#define REG_TSF_AGING_THRESHOLD_EXPIRED_SHIFT    25
#define REG_TSF_AGING_THRESHOLD_EXPIRED_WIDTH    1

/* NDP_HEADER_FRAME_CONTROL */
#define REG_NDP_HEADER_FRAME_CONTROL          0x0838
#define REG_NDP_HEADER_FRAME_CONTROL_MASK     0x0000ffff
#define REG_NDP_HEADER_FRAME_CONTROL_SHIFT    0
#define REG_NDP_HEADER_FRAME_CONTROL_WIDTH    16

/* MAX_NDPA_NDP_TIMER */
#define REG_MAX_NDPA_NDP_TIMER          0x083c
#define REG_MAX_NDPA_NDP_TIMER_MASK     0x000000ff
#define REG_MAX_NDPA_NDP_TIMER_SHIFT    0
#define REG_MAX_NDPA_NDP_TIMER_WIDTH    8

/* BF_INDEXES_TABLE_BASE_ADDR */
#define REG_BF_INDEXES_TABLE_BASE_ADDR          0x0840
#define REG_BF_INDEXES_TABLE_BASE_ADDR_MASK     0x00ffffff
#define REG_BF_INDEXES_TABLE_BASE_ADDR_SHIFT    0
#define REG_BF_INDEXES_TABLE_BASE_ADDR_WIDTH    24

/* CONDITION_EXPAND0 */
#define REG_CONDITION_EXPAND0          0x0900
#define REG_CONDITION_EXPAND0_MASK     0x0000003f
#define REG_CONDITION_EXPAND0_SHIFT    0
#define REG_CONDITION_EXPAND0_WIDTH    6

/* CONDITION_EXPAND1 */
#define REG_CONDITION_EXPAND1          0x0904
#define REG_CONDITION_EXPAND1_MASK     0x0000003f
#define REG_CONDITION_EXPAND1_SHIFT    0
#define REG_CONDITION_EXPAND1_WIDTH    6

/* CONDITION_EXPAND2 */
#define REG_CONDITION_EXPAND2          0x0908
#define REG_CONDITION_EXPAND2_MASK     0x0000003f
#define REG_CONDITION_EXPAND2_SHIFT    0
#define REG_CONDITION_EXPAND2_WIDTH    6

/* CONDITION_EXPAND3 */
#define REG_CONDITION_EXPAND3          0x090c
#define REG_CONDITION_EXPAND3_MASK     0x0000003f
#define REG_CONDITION_EXPAND3_SHIFT    0
#define REG_CONDITION_EXPAND3_WIDTH    6

/* CONDITION_EXPAND4 */
#define REG_CONDITION_EXPAND4          0x0910
#define REG_CONDITION_EXPAND4_MASK     0x0000003f
#define REG_CONDITION_EXPAND4_SHIFT    0
#define REG_CONDITION_EXPAND4_WIDTH    6

/* CONDITION_EXPAND5 */
#define REG_CONDITION_EXPAND5          0x0914
#define REG_CONDITION_EXPAND5_MASK     0x0000003f
#define REG_CONDITION_EXPAND5_SHIFT    0
#define REG_CONDITION_EXPAND5_WIDTH    6

/* CONDITION_EXPAND6 */
#define REG_CONDITION_EXPAND6          0x0918
#define REG_CONDITION_EXPAND6_MASK     0x0000003f
#define REG_CONDITION_EXPAND6_SHIFT    0
#define REG_CONDITION_EXPAND6_WIDTH    6

/* CONDITION_EXPAND7 */
#define REG_CONDITION_EXPAND7          0x091c
#define REG_CONDITION_EXPAND7_MASK     0x0000003f
#define REG_CONDITION_EXPAND7_SHIFT    0
#define REG_CONDITION_EXPAND7_WIDTH    6

/* CONDITION_EXPAND8 */
#define REG_CONDITION_EXPAND8          0x0920
#define REG_CONDITION_EXPAND8_MASK     0x0000003f
#define REG_CONDITION_EXPAND8_SHIFT    0
#define REG_CONDITION_EXPAND8_WIDTH    6

/* CONDITION_EXPAND9 */
#define REG_CONDITION_EXPAND9          0x0924
#define REG_CONDITION_EXPAND9_MASK     0x0000003f
#define REG_CONDITION_EXPAND9_SHIFT    0
#define REG_CONDITION_EXPAND9_WIDTH    6

/* CONDITION_EXPAND10 */
#define REG_CONDITION_EXPAND10          0x0928
#define REG_CONDITION_EXPAND10_MASK     0x0000003f
#define REG_CONDITION_EXPAND10_SHIFT    0
#define REG_CONDITION_EXPAND10_WIDTH    6

/* CONDITION_EXPAND11 */
#define REG_CONDITION_EXPAND11          0x092c
#define REG_CONDITION_EXPAND11_MASK     0x0000003f
#define REG_CONDITION_EXPAND11_SHIFT    0
#define REG_CONDITION_EXPAND11_WIDTH    6

/* CONDITION_EXPAND12 */
#define REG_CONDITION_EXPAND12          0x0930
#define REG_CONDITION_EXPAND12_MASK     0x0000003f
#define REG_CONDITION_EXPAND12_SHIFT    0
#define REG_CONDITION_EXPAND12_WIDTH    6

/* CONDITION_EXPAND13 */
#define REG_CONDITION_EXPAND13          0x0934
#define REG_CONDITION_EXPAND13_MASK     0x0000003f
#define REG_CONDITION_EXPAND13_SHIFT    0
#define REG_CONDITION_EXPAND13_WIDTH    6

/* CONDITION_EXPAND14 */
#define REG_CONDITION_EXPAND14          0x0938
#define REG_CONDITION_EXPAND14_MASK     0x0000003f
#define REG_CONDITION_EXPAND14_SHIFT    0
#define REG_CONDITION_EXPAND14_WIDTH    6

/* CONDITION_EXPAND15 */
#define REG_CONDITION_EXPAND15          0x093c
#define REG_CONDITION_EXPAND15_MASK     0x0000003f
#define REG_CONDITION_EXPAND15_SHIFT    0
#define REG_CONDITION_EXPAND15_WIDTH    6

/* CONDITION_EXPAND16 */
#define REG_CONDITION_EXPAND16          0x0940
#define REG_CONDITION_EXPAND16_MASK     0x0000003f
#define REG_CONDITION_EXPAND16_SHIFT    0
#define REG_CONDITION_EXPAND16_WIDTH    6

/* CONDITION_EXPAND17 */
#define REG_CONDITION_EXPAND17          0x0944
#define REG_CONDITION_EXPAND17_MASK     0x0000003f
#define REG_CONDITION_EXPAND17_SHIFT    0
#define REG_CONDITION_EXPAND17_WIDTH    6

/* CONDITION_EXPAND18 */
#define REG_CONDITION_EXPAND18          0x0948
#define REG_CONDITION_EXPAND18_MASK     0x0000003f
#define REG_CONDITION_EXPAND18_SHIFT    0
#define REG_CONDITION_EXPAND18_WIDTH    6

/* CONDITION_EXPAND19 */
#define REG_CONDITION_EXPAND19          0x094c
#define REG_CONDITION_EXPAND19_MASK     0x0000003f
#define REG_CONDITION_EXPAND19_SHIFT    0
#define REG_CONDITION_EXPAND19_WIDTH    6

/* CONDITION_EXPAND20 */
#define REG_CONDITION_EXPAND20          0x0950
#define REG_CONDITION_EXPAND20_MASK     0x0000003f
#define REG_CONDITION_EXPAND20_SHIFT    0
#define REG_CONDITION_EXPAND20_WIDTH    6

/* CONDITION_EXPAND21 */
#define REG_CONDITION_EXPAND21          0x0954
#define REG_CONDITION_EXPAND21_MASK     0x0000003f
#define REG_CONDITION_EXPAND21_SHIFT    0
#define REG_CONDITION_EXPAND21_WIDTH    6

/* CONDITION_EXPAND22 */
#define REG_CONDITION_EXPAND22          0x0958
#define REG_CONDITION_EXPAND22_MASK     0x0000003f
#define REG_CONDITION_EXPAND22_SHIFT    0
#define REG_CONDITION_EXPAND22_WIDTH    6

/* CONDITION_EXPAND23 */
#define REG_CONDITION_EXPAND23          0x095c
#define REG_CONDITION_EXPAND23_MASK     0x0000003f
#define REG_CONDITION_EXPAND23_SHIFT    0
#define REG_CONDITION_EXPAND23_WIDTH    6

/* CONDITION_EXPAND24 */
#define REG_CONDITION_EXPAND24          0x0960
#define REG_CONDITION_EXPAND24_MASK     0x0000003f
#define REG_CONDITION_EXPAND24_SHIFT    0
#define REG_CONDITION_EXPAND24_WIDTH    6

/* CONDITION_EXPAND25 */
#define REG_CONDITION_EXPAND25          0x0964
#define REG_CONDITION_EXPAND25_MASK     0x0000003f
#define REG_CONDITION_EXPAND25_SHIFT    0
#define REG_CONDITION_EXPAND25_WIDTH    6

/* CONDITION_EXPAND26 */
#define REG_CONDITION_EXPAND26          0x0968
#define REG_CONDITION_EXPAND26_MASK     0x0000003f
#define REG_CONDITION_EXPAND26_SHIFT    0
#define REG_CONDITION_EXPAND26_WIDTH    6

/* CONDITION_EXPAND27 */
#define REG_CONDITION_EXPAND27          0x096c
#define REG_CONDITION_EXPAND27_MASK     0x0000003f
#define REG_CONDITION_EXPAND27_SHIFT    0
#define REG_CONDITION_EXPAND27_WIDTH    6

/* CONDITION_EXPAND28 */
#define REG_CONDITION_EXPAND28          0x0970
#define REG_CONDITION_EXPAND28_MASK     0x0000003f
#define REG_CONDITION_EXPAND28_SHIFT    0
#define REG_CONDITION_EXPAND28_WIDTH    6

/* CONDITION_EXPAND29 */
#define REG_CONDITION_EXPAND29          0x0974
#define REG_CONDITION_EXPAND29_MASK     0x0000003f
#define REG_CONDITION_EXPAND29_SHIFT    0
#define REG_CONDITION_EXPAND29_WIDTH    6

/* TXOP_CONDITION_EXPAND0 */
#define REG_TXOP_CONDITION_EXPAND0          0x0978
#define REG_TXOP_CONDITION_EXPAND0_MASK     0x0000003f
#define REG_TXOP_CONDITION_EXPAND0_SHIFT    0
#define REG_TXOP_CONDITION_EXPAND0_WIDTH    6

/* TXOP_CONDITION_EXPAND1 */
#define REG_TXOP_CONDITION_EXPAND1          0x097c
#define REG_TXOP_CONDITION_EXPAND1_MASK     0x0000003f
#define REG_TXOP_CONDITION_EXPAND1_SHIFT    0
#define REG_TXOP_CONDITION_EXPAND1_WIDTH    6

/* TXOP_CONDITION_EXPAND2 */
#define REG_TXOP_CONDITION_EXPAND2          0x0980
#define REG_TXOP_CONDITION_EXPAND2_MASK     0x0000003f
#define REG_TXOP_CONDITION_EXPAND2_SHIFT    0
#define REG_TXOP_CONDITION_EXPAND2_WIDTH    6

/* TXOP_CONDITION_EXPAND3 */
#define REG_TXOP_CONDITION_EXPAND3          0x0984
#define REG_TXOP_CONDITION_EXPAND3_MASK     0x0000003f
#define REG_TXOP_CONDITION_EXPAND3_SHIFT    0
#define REG_TXOP_CONDITION_EXPAND3_WIDTH    6

/* TXOP_CONDITION_EXPAND4 */
#define REG_TXOP_CONDITION_EXPAND4          0x0988
#define REG_TXOP_CONDITION_EXPAND4_MASK     0x0000003f
#define REG_TXOP_CONDITION_EXPAND4_SHIFT    0
#define REG_TXOP_CONDITION_EXPAND4_WIDTH    6
#endif 
