ccflags-y := -Werror

ifeq ($(CONFIG_PPA_MPE_HAL),m)
obj-m	+= ltq_mpe_hal_drv.o
ltq_mpe_hal_drv-objs	+= ltq_mpe_hal_proc.o ltq_mpe_tmpl_buf.o ltq_mpe_hal.o
endif
