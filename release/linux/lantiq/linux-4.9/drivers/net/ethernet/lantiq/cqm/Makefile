#
# Makefile for CBM driver.
#

obj-$(CONFIG_LTQ_CBM) +=  cqm_dev.o cqm_common.o cbm_wrapper.o
ifneq ($(CONFIG_GRX500_CBM),)
obj-$(CONFIG_LTQ_CBM) += grx500/
endif
ifneq ($(CONFIG_PRX300_CQM),)
obj-$(CONFIG_LTQ_CBM) += prx300/
endif
