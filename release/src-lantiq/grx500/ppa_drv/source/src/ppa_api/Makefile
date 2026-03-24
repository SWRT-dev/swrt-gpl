ifeq ($(CONFIG_PPA),y)
obj-m += ppa_drv_stack_al.o
ppa_drv_stack_al-objs += ppa_stack_al.o ppa_api_tools.o ppa_stack_tnl_al.o

obj-m += ppa_api.o
ppa_api-objs += ppa_drv.o ppa_api_netif.o
ifeq ($(CONFIG_PPA_QOS),y)
ppa_api-objs += ppa_api_qos.o
endif

ccflags-y := -Werror

ifneq ($(CONFIG_PPA),)
ppa_api-objs += ppa_api_core.o ppa_api_misc.o ppa_api_mib.o ppa_api_proc.o 
ppa_api-objs += ppa_api_session.o ppa_api_sess_helper.o ppa_api_hal_selector.o
ppa_api-objs += ppa_sae_hal.o

ifeq ($(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS),y)
ppa_api-objs += ppa_api_session_limit.o
endif

ifeq ($(CONFIG_LTQ_PMCU),y)
   ppa_api-objs += ppa_api_pwm.o ppa_api_pwm_logic.o
endif

ifeq ($(CONFIG_LTQ_CPU_FREQ),y)
   ppa_api-objs += ppa_api_pwm.o ppa_api_pwm_logic.o
endif

ifeq ($(CONFIG_PPA_PP_LEARNING),y)
ppa_api-objs += ppa_api_pp_core.o
endif

# Stat Module
ifeq ($(CONFIG_SOC_GRX500),y)
ppa_api-objs += ppa_stat_helper_xrx500.o
endif

# CUT_HERE_FOR_CD
# Note: Do not add any thing below this line. Following the lines 
#       "CUT_HERE_FOR_CD" will be removed during CD creation.
ifeq ($(CONFIG_PPA_API_SW_FASTPATH),y)
   obj-m += ppa_api_sw_accel_mod.o
   ppa_api_sw_accel_mod-objs = ppa_api_sw_accel.o ppa_api_sw_accel_module.o 
endif

ifeq ($(CONFIG_PPA_MAC_VIOLATION_MIRROR),m)
	ccflags-y += -DCONFIG_PPA_MAC_VIOLATION_MIRROR
endif
endif
endif

ifneq ($(KERNELRELEASE),)

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
