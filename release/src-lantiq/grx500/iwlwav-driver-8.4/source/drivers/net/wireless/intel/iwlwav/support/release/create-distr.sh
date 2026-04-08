#!/bin/sh
#
# $Id: build-platform.sh 9138 2010-06-20 14:06:40Z dmytrof $
#
# This script is intended to create source tarball distributions
#
# Copyright (c) 2006-2009 Metalink Broadband (Israel)
#

QUIET="no"
if [[ $1 == @(-q|--quiet) ]]; then
  QUIET="yes"
  shift
fi

function limit_output() {
  if [[ $QUIET == no ]]; then
    # default (verbose) mode, simply run the program
    "$@"
  else
    # quiet mode: hide stdout completely, show stderr only if error occurs
    err_log=$(mktemp)
    if "$@" >/dev/null 2>$err_log; then
      # no errors occured
      rm $err_log
    else
      # some errors occured
      echo "Error occured, exit code was $?." >&2
      echo "Here are the last 40 lines, full error log saved at '$err_log':" >&2
      echo
      tail -n 40 $err_log >&2
      exit 1
    fi
  fi
}

# Test if we have all required parameters
if [ -z $3 ]; then
  echo "Usage: $0 <release dir> <component name> <package version> <prebuilt components location>"
  exit 1
fi

RELEASE_DIRECTORY=$1
COMPONENT_NAME=$2
PACKAGE_VERSION=$3

echo "`date` Removing previous archives"
limit_output make defconfig MINICONFIG=./support/release/configs/${COMPONENT_NAME}.complist && \
rm -f `./support/cfghlpr.sh .config get_bld_dir`/*.tar.bz2 && \
echo "`date` Running distcheck" && \
limit_output make $DISTCHECK_FLAGS distcheck && \
mkdir -p ${RELEASE_DIRECTORY}

if [ $? -ne 0 ]; then exit 11; fi

DISTR_DIR=lq-wave-300-${PACKAGE_VERSION}.${COMPONENT_NAME}.src
RFLIB_DIR=wireless/driver/rflib
DISTR_NAME_GPL=lq-wave-300-${PACKAGE_VERSION}.gpl.${COMPONENT_NAME}.src
DISTR_NAME_RFLIB=lq-wave-300-${PACKAGE_VERSION}.rflib.${COMPONENT_NAME}.src

if [ $? -ne 0 ]; then exit 10; fi

function move_if_exists() {
if [[ -f $1 ]]; then
  mv -f $1 $2
fi
}

pushd `./support/cfghlpr.sh .config get_bld_dir`/

for file in *.tar.bz2; do

SRC_ROOT_DIR=../../..

  echo "`date` repacking $file"
  mkdir _repacking_`basename $file .tar.bz2` && \
  cd _repacking_`basename $file .tar.bz2` && \
  tar xjf ../$file && \
  rm -f ../$file && \
  cp -R `basename $file .tar.bz2` ${DISTR_DIR} && \
  cp -R ${SRC_ROOT_DIR}/LICENSE ${DISTR_DIR}/ && \
  move_if_exists ${DISTR_DIR}/wireless/rnotes.REL_* ${DISTR_DIR}/wireless/rnotes.txt && \
  rm -fr ${DISTR_DIR}/${RFLIB_DIR} && \
  mkdir ${DISTR_DIR}/${RFLIB_DIR} && \
  touch ${DISTR_DIR}/${RFLIB_DIR}/Makefile.am && \
  touch ${DISTR_DIR}/${RFLIB_DIR}/Makefile.in && \
  tar cjf ../${DISTR_NAME_GPL}.tar.bz2 ${DISTR_DIR} && \
  rm -fr ${DISTR_DIR} && \
  mkdir -p ${DISTR_DIR}/${RFLIB_DIR} && \
  cp -R `basename $file .tar.bz2`/${RFLIB_DIR}/* ${DISTR_DIR}/${RFLIB_DIR}/ && \
  cp -R ${SRC_ROOT_DIR}/${RFLIB_DIR}/LICENSE ${DISTR_DIR}/${RFLIB_DIR}/ && \
  tar cjf ../${DISTR_NAME_RFLIB}.tar.bz2 ${DISTR_DIR} && \
  cd .. && \
  rm -rf _repacking_`basename $file .tar.bz2` && \
  continue;

  echo "Failed to repack the archive"
  exit 112;

done

popd

mv -fv `./support/cfghlpr.sh .config get_bld_dir`/*.tar.bz2 ${RELEASE_DIRECTORY}

exit 0;

