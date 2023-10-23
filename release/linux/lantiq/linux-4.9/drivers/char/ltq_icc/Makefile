#
# drivers/char/ltq_icc/Makefile
#

#
# Makefile for the Lantiq icc driver
#
obj-$(CONFIG_LTQ_ICC) += drv_icc.o icc_client.o

#obj-$(CONFIG_SOC_TYPE_GRX500_TEP) += proc_redirect_bootcore.o
# !<WW: FIXME!
# this is broken due to changes in proc function to use rb tree for search and
# traverse
# obj-$(CONFIG_SOC_GRX500) += proc_redirect.o

# needed for internal struct proc_dir_entry definiton
# CFLAGS_proc_redirect.o += -I./fs/proc
