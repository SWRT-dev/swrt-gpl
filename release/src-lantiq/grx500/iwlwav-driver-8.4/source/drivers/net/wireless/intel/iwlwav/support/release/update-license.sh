#!/bin/bash
#
# This script is intended to update the wave driver license.
#

SCRIPT_NAME=$(basename $0)

SRC_PATH="${PWD}"
LICENSE_PATH=${SRC_PATH}/license

function cleanup() {
  local exit_code=$?
  cd "${SRC_PATH}"
  exit $exit_code
}
trap "cleanup" EXIT

echo ${SCRIPT_NAME}
echo ${SRC_PATH}
echo ${LICENSE_PATH}
echo
#exit 0

NEED_TO_UPDATE="no"

error()
{
  echo "$SCRIPT_NAME: error: $@" >&2
  exit 1
}

success()
{
  echo "$SCRIPT_NAME: $@" >&2
  exit 0
}

info()
{
  echo "$SCRIPT_NAME: $@" >&2
}

# Verify that license is under version control
pushd "${LICENSE_PATH}" >/dev/null
if git status >/dev/null 2>&1; then
  GIT=true
elif hg id >/dev/null; then
  GIT=false
else
  error "License is not under version control"
fi
popd >/dev/null

# check incoming
pushd "${LICENSE_PATH}" >/dev/null
if $GIT; then
  [[ $(git branch -l) =~ '(no branch)' ]] &&
    error "'license' repo does not have any branch selected. 'master' *might* be the right one. Or maybe not."
  git fetch sw_ugw && git log HEAD..FETCH_HEAD --exit-code &&
    success "License is not changed. Ok"
else
  hg incoming >/dev/null ||
    success "License is not changed. Ok"
fi
popd >/dev/null
# note that the rest only executes if there are changes in the upstream license repo

# pull and update
pushd "${LICENSE_PATH}" >/dev/null
if $GIT; then
  echo
  echo "There have been updates in the license repo. They have been fetched, but not merged."
  echo "Please look at these updates and if the merge is needed, carry it out manually & rerun this command."
  echo
else
  hg pull && hg update &&
    cd "${SRC_PATH}" && hg commit -S -m "Updated license files tag"
fi
popd >/dev/null
(( $? )) && error "Updating the license has failed."

# create local copy
info "Update license files if needed"

diff -q   ${LICENSE_PATH}/GPL_BSD/LICENSE ${SRC_PATH}/ \
|| cp -pv ${LICENSE_PATH}/GPL_BSD/LICENSE ${SRC_PATH}/ \
|| error "Update GPL_BSD LICENESE is failed"

diff -q   ${LICENSE_PATH}/wlan_wrapper_rflib/LICENSE ${SRC_PATH}/wireless/driver/rflib/ \
|| cp -pv ${LICENSE_PATH}/wlan_wrapper_rflib/LICENSE ${SRC_PATH}/wireless/driver/rflib/ \
|| error "Update RFLIB LICENESE is failed"

if $GIT; then
  git commit LICENSE -m "Updated license file" >/dev/null
fi

# return Ok
exit 0
