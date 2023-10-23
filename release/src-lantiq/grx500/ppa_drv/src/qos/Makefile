TARGET := qos_mgr_drv

# Target objects
$(TARGET)-y += qos_mgr.o
$(TARGET)-objs += qos_mgr.o qos_hal_proc.o qos_hal_ll_dp.o qos_hal_dp.o

$(TARGET)-objs += qos_mgr_stack_al.o
$(TARGET)-objs += qos_mgr_api.o
$(TARGET)-objs += qos_mgr_ioctl.o

ifneq ($(CONFIG_QOS_MGR_TC_SUPPORT),)
$(TARGET)-objs += qos_mgr_tc.o qos_mgr_tc_lct.o qos_mgr_tc_pce.o qos_mgr_tc_ds_qos.o qos_mgr_tc_mirred.o
ifneq ($(CONFIG_NET_SCH_DRR),)
$(TARGET)-objs += qos_mgr_tc_drr.o
ifneq ($(CONFIG_NET_SCH_PRIO),)
$(TARGET)-objs += qos_mgr_tc_prio.o
endif
endif
ifneq ($(CONFIG_NET_SCH_TBF),)
$(TARGET)-objs += qos_mgr_tc_tbf.o
endif
ifneq ($(CONFIG_NET_SCH_MQPRIO),)
$(TARGET)-objs += qos_mgr_tc_mqprio.o
endif #CONFIG_NET_SCH_MQPRIO
ifneq ($(CONFIG_NET_CLS_FLOWER),)
$(TARGET)-objs += qos_mgr_tc_flower.o
endif #CONFIG_NET_CLS_FLOWER
endif #CONFIG_QOS_MGR_TC_SUPPORT

ifneq ($(KERNELRELEASE),)
obj-m += $(TARGET).o
else
SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

endif
