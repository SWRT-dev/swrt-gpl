ifneq ($(CONFIG_PPA_ACCEL),)
obj-m += lgm_pp_hal_drv.o
lgm_pp_hal_drv-objs	+= lgm_pp_hal.o
ifneq ($(CONFIG_LGM_TOE),)
lgm_pp_hal_drv-objs     += lgm_lro_hal.o
endif
endif
ccflags-y := -Werror
ccflags-y += -DPP_INACTIVE_SCAN
