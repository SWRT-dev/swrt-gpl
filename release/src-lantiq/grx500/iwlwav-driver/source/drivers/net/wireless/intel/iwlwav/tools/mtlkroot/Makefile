#common
obj-$(CPTCFG_IWLWAV)	+= mtlkroot.o

abs_top			:= $(src)/../..
abs_builddir		:= $(src)
abs_logprep_srcdir	:= $(abs_top)/wireless/shared_iwlwav-tools
logger_origin_id	:= 7
logger_origin_name	:= mtlkroot
logger_binary_name	:= mtlkroot.o

ifeq ($(CPTCFG_BACKPORTED_WIRELESS),y)
CC := abs_top_srcdir=$(abs_top) abs_builddir=$(abs_builddir) \
  abs_logprep_srcdir=$(abs_logprep_srcdir) logger_origin_name=$(logger_origin_name) \
  logger_binary_name=$(logger_binary_name) logger_origin_id=$(logger_origin_id) \
  $(abs_top)/support/mtlkfront.sh `echo $(CC) |sed -e 's/.*mtlkfront.sh //g'`
endif

mtlkroot-objs += shared_iwlwav-dev/mtlkstartup.o

mtlkroot-objs += linux/logdrv_osdep.o
mtlkroot-objs += log_drv.o
mtlkroot-objs += linux/mtlk_assert.o
mtlkroot-objs += linux/mtlkcdev.o
mtlkroot-objs += linux/mtlkirbd_root.o
mtlkroot-objs += linux/nlmsgs.o
mtlkroot-objs += shared_iwlwav-dev/mtlkirbhash.o
mtlkroot-objs += shared_iwlwav-dev/mtlk_algorithms.o
mtlkroot-objs += linux/mtlkndev.o
mtlkroot-objs += linux/mtlkroot.o
ifeq ($(CPTCFG_IWLWAV_ENABLE_OBJPOOL),y)
mtlkroot-objs += shared_iwlwav-dev/mem_leak.o
mtlkroot-objs += shared_iwlwav-dev/mtlk_objpool.o
endif
mtlkroot-objs += shared_iwlwav-dev/log_osdep.o
mtlkroot-objs += shared_iwlwav-dev/osal_defs.o
mtlkroot-objs += shared_iwlwav-dev/mtlk_snprintf.o

# Based on generated c file and therfore should be compiled last
logdb-obj := shared_iwlwav-dev/logmacro_database.o

$(obj)/$(logdb-obj) : $(src)/logmacros.ts

$(src)/logmacros.ts : $(mtlkroot-objs:%.o=$(obj)/%.o) # depends on all of the above objs
	touch -r $(src)/logmacros.c $@

# Add log_db to module
mtlkroot-objs += $(logdb-obj)

ccflags-y += -DLOG_LOCAL_OID=7 -DMTLK_COPYRIGHT="\"\"" -Wno-multichar #TODO: To be fixed
ccflags-y += -I$(src)
ccflags-y += -I$(src)/linux
ccflags-y += -I$(src)/shared
ccflags-y += -I$(abs_top)/wireless/driver
ccflags-y += -I$(abs_top)/wireless/shared_iwlwav-tools
ccflags-y += -I$(abs_top)/wireless/shared_mbss_mac
ccflags-y += -I$(abs_top)

obj ?= $(shell pwd)

ifeq ($(CPTCFG_BACKPORTED_WIRELESS),y)
clean-files := logmacros.*
clean-files += *.scd
clean-files += $(shell find $(obj) -name "*.o.cmd")
clean-files += $(shell find $(obj) -name "*.stderr")
clean-files += $(shell find $(obj) -name "*.pipe.result")
endif
