ifeq ($(CONFIG_PPA),y)
obj-$(CONFIG_PPA)		+= platform/ ppa_api/ qos/ mac_violation_mirror/
else
ifeq ($(CONFIG_QOS_MGR),y)
obj-y		+= qos/
endif
endif
obj-$(CONFIG_DSL_DFE_LOOPBACK)	+= dsl_dfe_loopback/
obj-$(CONFIG_LOOP_ETH_DEV)	+= loop_eth_dev/
obj-$(CONFIG_LTQ_PORT_MIRROR)	+= portmirror/
obj-$(CONFIG_PTM_VECTORING)	+= vectoring/
obj-$(CONFIG_PPA_ATM_BONDING) += bonding/
ccflags-y := -Werror
