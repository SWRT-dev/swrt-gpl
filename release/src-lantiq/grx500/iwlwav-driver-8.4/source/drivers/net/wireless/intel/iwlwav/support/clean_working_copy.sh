#!/bin/sh
#
# $Id$
#
# This script renames all unversioned files in the working copy
# thus ensuring it won't compile in case some files were not
# added to repository
#
# Copyright (c) 2006-2009 Metalink Broadband (Israel)
#

#This script supports two optional commands:
# --purge means to remove unversioned files as opposite rename
# <path> means path to working copy root as opposite to default path ../../..
#        default path corresponds to working copy root when script is being
#        invoked from wireless/driver/linux

rename_file () {
  rm -vrf "$1.cleaned-out" && mv -vf "$1" "$1.cleaned-out"
}

purge_file () {
  rm -vrf "$1"
}

get_unversioned_file_list() {
  # retrieve "ignored" and "not tracked" files
  hg st -u $1 2>&1 | grep '^[\?I].*' \
                   | awk '/[\?I]/ {print $2}'
}

rename_unversioned_files() {
  get_unversioned_file_list $1 | grep -v '.cleaned-out$' \
                               | while read fname
                                 do
                                     rename_file $fname
                                 done
}

purge_unversioned_files() {
  get_unversioned_file_list $1 | while read fname
                                 do
                                     purge_file $fname
                                 done
}

is_operation_mode() {
  echo $1 | awk "/^--.*$/ { print 0; exit; }; {print 1;}"
}

OPERATION_MODE="--rename"
WC_ROOT="."

if ( [ "$1" != "" ] )
then
  if ( [ `is_operation_mode $1` != "1" ] )
  then
    OPERATION_MODE=$1
  else
    WC_ROOT=$1
  fi
fi

if ( [ "$2" != "" ] )
then
  if ( [ `is_operation_mode $2` != "1" ] )
  then
    OPERATION_MODE=$2
  else
    WC_ROOT=$2
  fi
fi

if ([ "$OPERATION_MODE" = "--purge" ])
then
    purge_unversioned_files $WC_ROOT
else
    rename_unversioned_files $WC_ROOT
fi
