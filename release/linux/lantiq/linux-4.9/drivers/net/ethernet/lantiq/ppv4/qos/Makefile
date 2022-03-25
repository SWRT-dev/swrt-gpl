#
# Makefile for PPv4 QoS driver.
#

obj-$(CONFIG_LTQ_PPV4_QOS) += pp_qos_drv.o
pp_qos_drv-y 	:= pp_qos_linux.o pp_qos_main.o pp_qos_utils.o pp_qos_fw.o pp_qos_elf.o pp_qos_debugfs.o
ccflags-y	+= -Iinclude/net -Iinclude/uapi/linux -DQOS_CPU_UC_SAME_ENDIANESS
