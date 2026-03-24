#!/bin/bash

TARGET_DIR=$1
INFO_TYPE=$2

test x"$TARGET_DIR" = x"" && TARGET_DIR="."

if [ -d "$TARGET_DIR/.hg" ]; then
  # Under source control => get Mercurial info

  get_revision() {
    if hash hg &>/dev/null; then
      hg identify --id "$1"
    else
      echo "unknown"
    fi
  }

  get_branch() {
    if hash hg &>/dev/null; then
      hg identify --branch "$1"
    else
      echo "unknown"
    fi
  }

  get_fw_api_info() {
    if hash hg &>/dev/null; then
      hg identify --id "$1/wireless/shared_mbss_mac"
    else
      echo "unknown"
    fi
  }

  get_state() {
    echo ""
  }

elif [[ -d $TARGET_DIR/.git ]]; then

  get_revision() {
    pushd $TARGET_DIR >/dev/null

    git rev-parse HEAD

    popd >/dev/null
  }

  get_branch() {
    pushd $TARGET_DIR >/dev/null

    git rev-parse --abbrev-ref HEAD

    popd >/dev/null
  }

  get_fw_api_info() {
    pushd $TARGET_DIR/wireless/shared_mbss_mac/ >/dev/null

    git rev-parse --short=12 HEAD

    popd >/dev/null
  }

  # check if clean working dir or with local changes
  get_state() {
    pushd $TARGET_DIR >/dev/null

    [ -n "$(git status --untracked-files=no --porcelain)" ] && echo ".With_local_changes"

    popd >/dev/null
  }

else
  # Source tarball => mark as 'exported' + base version if exists

  # get base version from 'revision' file if exists BEFORE we output (probaly to the same file)
  BASE_REVISION="$TARGET_DIR/revision_info"

  if [ -f "${BASE_REVISION}" ]; then
    BASE_INFO=$(cat "${BASE_REVISION}" | awk '{ print $1; }')
    BASE_FW_MBSS_API_INFO=$(cat "${BASE_REVISION}" | awk '{ print $2; }')
  fi

  get_revision() {
    echo "exported"
  }

  get_branch() {
    if [ "$BASE_INFO" ]; then
      echo "$BASE_INFO"
    else
      echo "unknown"
    fi
  }

  get_fw_api_info() {
    if [ "$BASE_FW_MBSS_API_INFO" ]; then
      echo "$BASE_FW_MBSS_API_INFO"
    else
      echo "unknown"
    fi
  }

  get_state() {
    echo ""
  }
fi

get_info() {
  echo "$(get_branch "$1").$(get_revision "$1")$(get_state)"
}

SHOW_BASE=0
SHOW_FW_MBSS_API=0
test x"${INFO_TYPE}" = x"" && SHOW_BASE=1 && SHOW_FW_MBSS_API=1
test x"${INFO_TYPE}" = x"base" && SHOW_BASE=1
test x"${INFO_TYPE}" = x"fw_mbss_api" && SHOW_FW_MBSS_API=1

test x"$SHOW_BASE" = x"1" && INFO="$INFO $(get_info "$TARGET_DIR")"
test x"$SHOW_FW_MBSS_API" = x"1" && INFO="$INFO $(get_fw_api_info "$TARGET_DIR")"

echo $INFO
