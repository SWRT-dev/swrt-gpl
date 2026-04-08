#!/usr/bin/env bash
#
# $Id$
#
# This script is intended to build all release binaries
# and create release directory structure.
#
# Copyright (c) 2006-2009 Metalink Broadband (Israel)
#

BUILD_PLATFORM_SH=./support/release/build-platform.sh
CREATE_DISTR_SH=./support/release/create-distr.sh

function info() {
  echo "`date` $(basename $0) [info]: $@"
}

function error() {
  local status=$1; shift
  echo "`date` $(basename $0) [error]: $@" >&2
  exit $status
}

function clean_release_dir() {
  info "Cleaning directory $RELEASE_DIRECTORY"

  rm -rf "$RELEASE_DIRECTORY" ||
    error 1 "Failed to remove files in '$RELEASE_DIRECTORY'."
  mkdir "$RELEASE_DIRECTORY"
}

function perform_platform_build() {
  local platform=$1
  local res_dir_tmp=$RELEASE_DIRECTORY/.$platform
  local res_dir_pub=$RELEASE_DIRECTORY/$platform

  echo "Performing platform $platform build in $res_dir_tmp for $COMPONENT_NAME"

  $BUILD_PLATFORM_SH $QUIET $platform $res_dir_tmp $COMPONENT_NAME ||
    return 4

  mkdir -p $res_dir_pub

  cp -a $res_dir_tmp/$COMPONENT_NAME/* $res_dir_pub/ ||
    return 6

  rm -rf $res_dir_tmp ||
    return 7
}

# 1.build all platforms
# 2.create tarball with library for each platform
# 3.create tarballs with all and GPL-only code
function perform_build() {
  for platform in $TARGET_PLATFORMS; do
    info "Performing platform build for $platform"
    perform_platform_build $platform || return 4
  done

  info "Creating dirstribution from $RELEASE_DIRECTORY for $COMPONENT_NAME [$PACKAGE_VERSION]"
  $CREATE_DISTR_SH $QUIET $RELEASE_DIRECTORY $COMPONENT_NAME $PACKAGE_VERSION $RELEASE_DIRECTORY || return 6
}

QUIET=""
if [[ $1 == @(-q|--quiet) ]]; then
  QUIET="--quiet"
  shift
fi

if [[ -n "$1" ]]; then
  COMPONENT_NAME=$1
else
  echo "Usage: build-reldir.sh <component name> [<release dir>] [<package version>] [<target platforms>]"
  exit 1
fi

RELEASE_DIRECTORY=release-dir
if [[ -n "$2" ]]; then
  RELEASE_DIRECTORY=$2
fi

PACKAGE_VERSION=unknown.version.number
if [[ -n "$3" ]]; then
   PACKAGE_VERSION=$3
fi

# reads default TARGET_PLATFORMS
. ./support/release/targets
if [[ -n $4 ]]; then
  if [[ $4 = "none" ]]; then
    # this skips building targets altogether and goes straight to 'make distcheck'
    TARGET_PLATFORMS=""
  else
    TARGET_PLATFORMS="$4"
  fi
fi

echo Building $RELEASE_DIRECTORY...

clean_release_dir && perform_build ||
  error 2 "$RELEASE_DIRECTORY build FAILED!"

echo $RELEASE_DIRECTORY build SUCCEEDED
