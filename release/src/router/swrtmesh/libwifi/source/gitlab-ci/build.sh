#!/bin/bash
set -e
echo "build stage"
pwd
CFLAGS="-DHAS_WIFI=1" make
