KCOV_INSTRUMENT:=$(CONFIG_PON_QOS_KCOV)

obj-$(CONFIG_PON_QOS) = pon_qos.o

pon_qos-objs := \
	pon_qos_tc_main.o \
	pon_qos_tc_qos.o  \
	pon_qos_tc_qmap.o \
	pon_qos_tc_drr.o \
	pon_qos_tc_debugfs.o \
	pon_qos_tc_red.o \
	pon_qos_tc_prio.o \
	pon_qos_tc_mqprio.o \
	pon_qos_tc_flower.o \
	pon_qos_tc_vlan_prepare.o \
	pon_qos_tc_vlan_storage.o \
	pon_qos_tc_ext_vlan.o \
	pon_qos_tc_vlan_filter.o \
	pon_qos_tc_lct.o \
	pon_qos_tc_pce.o \
	pon_qos_tc_parser.o \
	pon_qos_tc_mirred.o \
	pon_qos_tc_ds_tc_remap.o \
	pon_qos_tc_police.o \
	pon_qos_tc_tbf.o \
	pon_qos_tc_trap.o \
	pon_qos_tc_ip_drop.o

pon_qos-$(CONFIG_PON_QOS_TRACING) += pon_qos_trace.o

# otherwise trace header is not found
CFLAGS_pon_qos_trace.o += -I$(src)
