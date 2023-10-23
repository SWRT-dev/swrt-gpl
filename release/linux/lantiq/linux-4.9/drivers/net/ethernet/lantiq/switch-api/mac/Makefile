#
# Makefile for MAC
#

EXTRA_CFLAGS += -I$(src)
obj-$(CONFIG_LTQ_ETHSW_API) += mac_driver.o

mac_driver-objs = macsec_drv.o 
mac_driver-objs += mac_drv.o 
mac_driver-objs += xgmac_main.o 
mac_driver-objs += xgmac_debug.o 
mac_driver-objs += xgmac_mtl_api.o 
mac_driver-objs += xgmac_ptp.o 
mac_driver-objs += xgmac_mac_api.o 
mac_driver-objs += mac_cli.o 
mac_driver-objs += xgmac_mdio.o 
mac_driver-objs += mac_cfg.o 
mac_driver-objs += gswss_api.o 
mac_driver-objs += gswss_mac_api.o 
mac_driver-objs += lmac_api.o
mac_driver-objs += mac_tx_fifo.o

