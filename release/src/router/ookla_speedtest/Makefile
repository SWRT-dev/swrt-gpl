include ../common.mak

#OOKLA_VER = 36521# updated in 2019.12.31
#OOKLA_VER = 371237# updated in 2021.10.19
OOKLA_VER = 38000# updated in 2022.08.16
OOKLA_PATH = OOKLA-$(OOKLA_VER)

prebuild:
	-@mkdir -p prebuild
# QCA
ifeq ($(or $(RTCONFIG_SOC_IPQ8064),$(RTCONFIG_SOC_IPQ40XX)),y)
	-cp -f $(OOKLA_PATH)/ookla-IPQ807-401X  prebuild/ookla # IPQ80XX / IPQ40XX
else ifeq ($(RTCONFIG_SOC_IPQ60XX),y)
	-cp -f $(OOKLA_PATH)/ookla-IPQ60XX      prebuild/ookla # IPQ60XX
else ifeq ($(RTCONFIG_QCN550X),y)
	-cp -f $(OOKLA_PATH)/ookla-QCN5502      prebuild/ookla # QCN550X (3.7.12.35)
else ifeq ($(or $(RTCONFIG_SOC_IPQ53XX),$(RTCONFIG_SOC_IPQ8074)),y)
	-cp -f $(OOKLA_PATH)/ookla-IPQ53XX      prebuild/ookla
else
	# nothing
endif

# BRCM
ifeq ($(RTCONFIG_BCMARM),y)
ifeq ($(RTCONFIG_HND_ROUTER),y)
	-cp -f $(OOKLA_PATH)/ookla-AXHND-A7 prebuild/ookla     # AXHND-A7
else
	-cp -f $(OOKLA_PATH)/ookla-BCM7.x-6.x prebuild/ookla   # BCM6 / BCM7
endif # RTCONFIG_HND_ROUTER
endif # RTCONFIG_BCMARM

# MTK
ifeq ($(RTCONFIG_RALINK),y)
ifeq ($(RTCONFIG_MT798X),y)
	-cp -f $(OOKLA_PATH)/ookla-MT7986A  prebuild/ookla     # MT7986A
else
	-cp -f $(OOKLA_PATH)/ookla-MTK7621A prebuild/ookla     # MTK7621A
endif # RTCONFIG_MT798X
endif # RTCONFIG_RALINK

all: prebuild

install: all
	-@install -d $(INSTALLDIR)/usr/sbin
	-@install -D $(TOP)/ookla_speedtest/prebuild/ookla $(INSTALLDIR)/usr/sbin/ookla

clean:
	-rm -rf prebuild

