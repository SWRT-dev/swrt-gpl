# libwlan source Makefile
PKG_NAME := libspal
 
bins := libspal.so

libspal.so_sources := $(wildcard src/*.c)
	
libspal.so_cflags := -I./src/include `pkg-config --cflags libnl-3.0`
libspal.so_ldflags := -L./src/ -luci

include make.inc
