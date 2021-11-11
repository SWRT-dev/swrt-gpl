#!/bin/sh

cmake . -DBUILD_LUA=OFF -DCMAKE_INSTALL_PREFIX=/opt/uci
make
sudo make install
