PKG_NAME:=libmcastfapi
#PKG_VERSION:=$(shell cat VERSION)

bins := libmcastfapi.so


libmcastfapi.so_sources := $(wildcard *.c)
libmcastfapi.so_cflags := -I./include/ -I$(STAGING_DIR)/usr/include/
libmcastfapi.so_ldflags := -L$(STAGING_DIR)/usr/lib/ -lscapi -lhelper -L$(STAGING_DIR)/usr/lib/ -lsafec-3.3

include make.inc
