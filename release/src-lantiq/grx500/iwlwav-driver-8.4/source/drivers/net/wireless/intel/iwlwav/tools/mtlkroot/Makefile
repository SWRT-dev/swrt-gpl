# common
obj-$(CPTCFG_IWLWAV)	+= mtlkroot.o

ifeq ($(CONFIG_BACKPORTED_WIRELESS),y)
CC:=$(src)/../../support/mtlkfront.sh $(CC)
endif

mtlkroot-objs += ../../wireless/shared/mtlkstartup.o
mtlkroot-objs += linux/logdrv_osdep.o
mtlkroot-objs += log_drv.o
mtlkroot-objs += linux/mtlk_assert.o
mtlkroot-objs += linux/mtlkcdev.o
mtlkroot-objs += linux/mtlkirbd_root.o
mtlkroot-objs += linux/nlmsgs.o
mtlkroot-objs += ../shared/mtlkirbhash.o
mtlkroot-objs += ../../wireless/shared/mtlk_algorithms.o
mtlkroot-objs += linux/mtlkndev.o
mtlkroot-objs += linux/mtlkroot.o
ifeq ($(CPTCFG_IWLWAV_ENABLE_OBJPOOL),y)
mtlkroot-objs += ../../wireless/shared/mem_leak.o
mtlkroot-objs += ../../wireless/shared/mtlk_objpool.o
endif
mtlkroot-objs += ../../wireless/driver/log_osdep.o
mtlkroot-objs += ../../wireless/driver/osal_defs.o
mtlkroot-objs += ../../wireless/driver/mtlk_snprintf.o

# Based on generated c file and therfore should be compiled last
mtlkroot-objs += shared/logmacro_database.o

ccflags-y += -DLOG_LOCAL_OID=7 -DMTLK_COPYRIGHT="\"\"" -Wno-multichar #TODO: To be fixed
ccflags-y += -I$(src)
ccflags-y += -I$(src)/linux
ccflags-y += -I$(src)/shared
ccflags-y += -I$(src)/../../wireless/driver
ccflags-y += -I$(src)/../../wireless/shared
ccflags-y += -I$(src)/../../wireless/shared_mbss_mac
ccflags-y += -I$(src)/../shared
ccflags-y += -I$(src)/../rtlogger/shared
ccflags-y += -I$(src)/../..

ifeq ($(CONFIG_BACKPORTED_WIRELESS),y)
clean-files := $(shell find $(obj) -name "*.scd")
clean-files += $(shell find $(obj) -name "*.o.cmd")
clean-files += $(shell find $(obj) -name "*.stderr")
clean-files += $(shell find $(obj) -name "*.pipe.result")

export abs_top=$(src)/../..
export abs_top_srcdir=$(src)/../..
export abs_top_builddir=$(abs_top_srcdir)
export abs_tools_srcdir=$(abs_top)/tools
export logger_origin_name = mtlkroot
export logger_binary_name = mtlkroot.o
export logger_origin_id = 7
endif
