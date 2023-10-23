obj-$(CONFIG_PPA_ACCEL)	+= ltq_pae_hal.o

obj-$(CONFIG_PPA_TMU_HAL) += ltq_tmu_hal_drv.o
ltq_tmu_hal_drv-objs	+= ltq_tmu_hal_proc.o ltq_tmu_hal.o

ccflags-y := -Werror

obj-$(CONFIG_PPA_MPE_HAL) += ltq_mpe_hal_drv.o
ltq_mpe_hal_drv-objs	+= ltq_mpe_hal_proc.o ltq_mpe_tmpl_buf.o ltq_mpe_hal.o

obj-$(CONFIG_LTQ_PPA_ETH_BOND)  += ltq_eth_bond_dp.o
obj-$(CONFIG_PPA_LITEPATH_HWACC) += hw_tcp_litepath.o
