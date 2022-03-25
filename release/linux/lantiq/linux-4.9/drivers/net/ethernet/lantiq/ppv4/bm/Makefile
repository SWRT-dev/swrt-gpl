#
# Makefile for PPv4 BM driver.
#

obj-$(CONFIG_LTQ_PPV4) += ppv4_bm.o
ppv4_bm-y 	:= pp_bm_drv.o pp_bm_debugfs.o
#ccflags-y	+= -Iinclude/net -DNO_FW -DPP_QOS_DISABLE_CMDQ
ccflags-y	+= -Iinclude/net -DQOS_CPU_UC_SAME_ENDIANESS

#ccflags-$(CONFIG_LTQ_PPV4_QOS_TEST) += -DPP_QOS_TEST
