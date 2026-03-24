
###############################################################################
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
# 
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
# 
###############################################################################

#!/bin/bash


gcc -Wall -I ../../include -o ut_rx_bonding_testgroup_1 ut_rx_bonding_testgroup_1.c
gcc -Wall -I ../../include -o ut_rx_bonding_testgroup_2 ut_rx_bonding_testgroup_2.c
gcc -Wall -I ../../include -o ut_rx_bonding_testgroup_3 ut_rx_bonding_testgroup_3.c
gcc -Wall -I ../../include -o ut_rx_reassembly_testgroup_1 ut_rx_reassembly_testgroup_1.c
gcc -Wall -I ../../include -o ut_rx_bonding_and_reassembly_testgroup_1 ut_rx_bonding_and_reassembly_testgroup_1.c
