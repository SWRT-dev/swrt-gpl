#!/bin/sh
#
# usage: this_program file1 file2
#
# Broadcom Proprietary and Confidential. Copyright (C) 2016,
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom.
#
#
# <<Broadcom-WL-IPTag/Proprietary:>>
#

if test -r $2 ; then
  if cmp $1 $2 > /dev/null ; then
#    echo $2 is unchanged
    rm -f $1
  else
    mv -f $1 $2
  fi
else
  mv -f $1 $2
fi
