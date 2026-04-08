#!/usr/bin/env bash
#
# $Id$
#
# This script is intended to build all release binaries
# for specific platform.
#
# Copyright (c) 2006-2009 Metalink Broadband (Israel)
#

function info() {
  echo "`date` $(basename $0) [info]: $@"
}

function error() {
  local status=$1; shift
  echo "`date` $(basename $0) [error]: $@" >&2
  exit $status
}

function limit_output() {
  if [[ $QUIET == "no" ]]; then
    # default (verbose) mode, simply run the program
    "$@"
  else
    # quiet mode: hide stdout completely, show stderr only if error occurs
    local err_log=$(mktemp)
    if "$@" >/dev/null 2>$err_log; then
      # no errors occured
      rm $err_log
    else
      # some errors occured
      echo "Error occured, exit code was $?." >&2
      echo "Here are last 40 lines, full error log saved at '$err_log':" >&2
      echo
      tail -n 40 $err_log >&2
      exit 1
    fi
  fi
}

function build_binary() {
  local kw_info=""
  if [[ $KLOCWORK == "yes" ]]; then
    kw_info=" (+ klocwork)"
  fi
  info "Building binary for $PLATFORM - $COMPONENT $kw_info"

  # get the right paths
  local templates=./support/release/configs
  local mini_config=./mini.config
  cat $templates/$PLATFORM.config $templates/$COMPONENT.complist > $mini_config ||
    error 1 "Failed to generate mini config '$mini_config' for platform '$PLATFORM'."

  limit_output make defconfig MINICONFIG=$mini_config ||
    error 1 "Failed to generate config for '$PLATFORM', based on '$mini_config'."

  local build_dir; build_dir=$(./support/cfghlpr.sh .config get_bld_dir) ||
    error 1 "./support/cfghlpr.sh failed to get_bld_dir"
  local bin_dir=$build_dir/binaries

  rm -rf $bin_dir

  local make="make"
  if [[ $KLOCWORK == "yes" ]]; then
    make="kwshell make"
  fi

  # actually run 'make'
  limit_output $make -C $build_dir install ||
    error 1 "Failed to build platform '$PLATFORM' in '$build_dir'."

  info "Done building, now copy stuff around"

  # copy binaries to $RES_DIR
  for file in $(cd $bin_dir && find . -type f); do
    local destination=$RES_DIR/$(dirname $file)

    mkdir -p $destination
    cp $bin_dir/$file $destination ||
      error 2 "Failed to copy file '$file' into directory '$destination'."
  done

  info "Done copying stuff around"
}

KLOCWORK="no"
if [[ $1 == "--klocwork" ]]; then
  KLOCWORK="yes"
  shift
fi

QUIET="no"
if [[ $1 == @(-q|--quiet) ]]; then
  QUIET="yes"
  shift
fi

PLATFORM=$1
RES_DIR=$2

if [[ -z "$PLATFORM" || -z "$RES_DIR" ]]; then
  echo "Usage: "`basename $0`" <-q|--quiet> <platform> <res_dir> [<component_name>]"
  echo "       if --quiet (or -q) is specified, output will be less verbose"
  exit 2
fi

COMPONENT="all"
if [[ -n "$3" ]]; then
  COMPONENT=$3
fi

mkdir -p "$RES_DIR" || exit 1
build_binary || exit 2
