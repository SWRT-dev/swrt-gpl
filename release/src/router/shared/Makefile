include ../common.mak
include $(SRCBASE)/.config
include ../.config

CFLAGS += -Os -Wall $(EXTRACFLAGS) -fPIC
#CFLAGS += $(PRIVATE_EXTRACFLAGS)
CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include

ifneq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(TOP)/kernel_header/include
else
ifneq ($(HND_ROUTER),y)
CFLAGS += -I$(LINUXDIR)/include
else
CFLAGS += -DHND_ROUTER -DLINUX26 -DCONFIG_BCMWL5
CFLAGS += -I$(HND_SRC)/bcmdrivers/opensource/include/bcm963xx
CFLAGS += -I$(HND_SRC)/shared/opensource/include/bcm963xx
#CFLAGS += -I$(TOP)/ethctl -I$(TOP)/ethswctl
#CFLAGS += -I$(TOP)/ethswctl
CFLAGS += -I$(HND_SRC)/userspace/private/include
#LDFLAGS += -L$(TOP)/ethswctl_lib -lethswctl
ifeq ($(HND_ROUTER_AX),y)
CFLAGS  += -DWL11AC_80P80 -DWL11AC_160
CFLAGS  += -I$(SRCBASE)/include -I$(SRCBASE)/../components/wlioctl/include -I$(SRCBASE)/../components/proto/include
CFLAGS  += -I$(SRCBASE)/shared/bcmwifi/include -I $(SRCBASE)/../components/math/include/
CFLAGS += -I$(SRCBASE)/wl/sys
endif
endif
CFLAGS += -D__EXPORTED_HEADERS__
endif

CFLAGS += -DTEST1
ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
LDFLAGS += -lpthread
endif

ifeq ($(RTCONFIG_SHP),y)
CFLAGS += -DSHP
endif

ifeq ($(RTCONFIG_RALINK),y)
CFLAGS += -I$(TOP)/kernel_header/include
CFLAGS += -D_GNU_SOURCE -DTYPEDEF_FLOAT_T
CFLAGS += -I./sysdeps/ralink -I$(TOP)/wireless_tools/ -DWE_NOLIBM
endif

ifeq ($(RTCONFIG_QCA),y)
CFLAGS += -I./sysdeps/qca -I$(TOP)/wireless_tools/ -DWE_NOLIBM
CFLAGS += $(if $(or $(RTCONFIG_SOC_IPQ8064),$(RTCONFIG_SOC_IPQ8074)),-DSMP)
endif

ifeq ($(RTCONFIG_LANTIQ),y)
CFLAGS += -I./sysdeps/lantiq -I$(TOP)/wireless_tools/ -DWE_NOLIBM
CFLAGS += -I$(TOP)/shared/sysdeps/lantiq -I$(TOP)/wireless_tools -I$(TOP)/fapi_wlan_common-1.0.0.1/include -I$(TOP)/libhelper-1.4.0.2/include -DPACKAGE_ID=\"fapi_wlan_common\" -DLOGGING_ID="fapi_wlan_common"
CFLAGS += -I$(TOP)/wlc_nt
endif



ifeq ($(RTCONFIG_BCMWL6),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include
CFLAGS += -I$(SRCBASE)/common/include
ifeq ($(RTCONFIG_BCM7)$(RTCONFIG_HSPOT),y)
CFLAGS += ${WLAN_StdIncPathA}
endif
else
CFLAGS += -I$(SRCBASE)/shared
endif

ifeq ($(RTCONFIG_BCMSMP),y)
CFLAGS += -DSMP
endif

ifeq ($(RTCONFIG_5301X),y)
CFLAGS += -DBCM5301X
endif

include $(SRCBASE)/.config

ifeq ($(RTCONFIG_4352_5G),y)
CFLAGS += -DBCM4352
endif

ifeq (2_6_36,$(LINUX_VERSION))
CFLAGS += -DAUTOFW_PORT_DEPRECATED
endif

CFLAGS += -DTRAFFIC_MGMT

ifeq ($(RTCONFIG_BCM7)$(RTCONFIG_HSPOT),y)
CFLAGS += -DD11AC_IOTYPES
endif

ifeq ($(RTCONFIG_BCM_CEVENTD),y)
CFLAGS += -DBCM_CEVENT
endif

ifeq ($(RTCONFIG_CONCURRENTREPEATER),y)
ifeq ($(RTCONFIG_RP_NEWSSID_REV3),y)
CFLAGS += -DDEF_SSID_2G=\"ASUS_RP\" -DDEF_SSID_5G=\"ASUS_RP\"
else
SSID = $(subst -,,$(BUILD_NAME))
CFLAGS += -DDEF_SSID_2G=\"ASUS_$(SSID)\" -DDEF_SSID_5G=\"ASUS_$(SSID)_5G\"
endif
endif

ifeq ($(or $(RTAC68A),$(RT4GAC68U)),y)
CFLAGS += -DRTAC68U
endif

ifeq ($(BUILD_NAME), RT-N10P_V3)
CFLAGS += -DRTN10P_V3
endif

ifeq ($(BUILD_NAME), RT-AC1200_V2)
CFLAGS += -DRTAC1200V2
endif

ifeq ($(BUILD_NAME), RT-ACRH18)
CFLAGS += -DRTACRH18
endif

ifeq ($(BUILD_NAME), RT-AX53U)
CFLAGS += -DRTAX53U
endif

ifeq ($(BUILD_NAME), RT-AX54)
CFLAGS += -DRTAX54
endif

ifeq ($(BUILD_NAME), 4G-AC86U)
CFLAGS += -DRT4GAC86U
endif

ifeq ($(BUILD_NAME), 4G-AX56)
CFLAGS += -DRT4GAX56
endif

ifeq ($(RTCONFIG_RGBLED),y)
CFLAGS += -I$(TOP)/i2c-tools-3.1.2/include/linux
endif

ifneq ($(findstring CONFIG_BCM_KF_NETFILTER=, $(shell cat $(LINUXDIR)/.config)),)
CFLAGS         += -DBCM_KF_NETFILTER
endif

OBJS = shutils.o defaults.o model.o rtstate.o boardapi.o
OBJS += network_utility.o
OBJS += misc.o version.o files.o strings.o process.o
OBJS += bin_sem_asus.o semaphore.o pids.o notify_rc.o
OBJS += discover.o
OBJS += base64.o
OBJS += nvparse.o
OBJS += bwdpi_utils.o
OBJS += bwdpi_utils.o
OBJS += mac_name_tbl.o
OBJS += stun.o
ifeq ($(RTCONFIG_AHS),y)
OBJS += notify_ahs.o
endif
ifeq ($(RTCONFIG_AMAS_WGN),y)
OBJS += amas_wgn_shared.o
endif
ifeq ($(RTCONFIG_RGBLED),y)
OBJS += aura_rgb.o
endif
OBJS += timer_utils.o
OBJS += scripts.o
ifeq ($(RTCONFIG_TRAFFIC_LIMITER),y)
OBJS += tld_utils.o
endif
ifeq ($(RTCONFIG_BCM7),y)
OBJS += et_linux.o bcmwifi_channels.o
else ifeq ($(RTCONFIG_HSPOT),y)
OBJS += bcmwifi_channels.o
endif
ifeq ($(or $(RTCONFIG_BCM_7114),$(HND_ROUTER)),y)
OBJS += confmtd_utils.o
ifeq ($(RTCONFIG_WBD),y)
OBJS += wbd_rc_shared.o
endif
ifeq ($(RTCONFIG_GMAC3),y)
OBJS += et_linux.o
endif
endif
ifeq ($(RTCONFIG_RALINK),y)
OBJS += $(TOP)/wireless_tools/iwlib.o
OBJS += flash_mtd.o api-ralink.o ralink.o
ifeq ($(RTCONFIG_RALINK_MT7621),y)
CFLAGS += -DRA_ESW
OBJS += mt7620.o
else ifeq ($(RTCONFIG_RALINK_MT7628),y)
OBJS += mt7628.o
else ifeq ($(RTCONFIG_RALINK_MT7629),y)
OBJS += mt7629.o
else ifeq ($(RTCONFIG_RALINK_MT7622),y)
OBJS += mt7622.o
else
OBJS += rtkswitch.o
endif

else ifeq ($(RTCONFIG_QCA),y)
# Qualcomm Atheros
OBJS += $(TOP)/wireless_tools/iwlib.o
OBJS += flash_mtd.o api-qca.o
ifeq ($(or $(RTCONFIG_SWITCH_RTL8370M_PHY_QCA8033_X2),$(RTCONFIG_SWITCH_RTL8370MB_PHY_QCA8033_X2)),y)
OBJS += qca8033_rtl8370m.o mdio.o
else ifeq ($(RTCONFIG_QCA8033),y)
OBJS += qca8033.o
else ifeq ($(RTCONFIG_SOC_IPQ40XX),y)
OBJS += ipq40xx.o
else ifeq ($(RTCONFIG_SWITCH_QCA8075_PHY_AQR107),y)
OBJS += qca8075_phy_aqr107.o
else ifeq ($(RTCONFIG_SWITCH_QCA8075_PHY_AQR111),y)
OBJS += qca8075_phy_aqr111.o mdio.o ethtool.o
else ifeq ($(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033),y)
OBJS += qca8075_qca8337_phy_aqr107_ar8035_qca8033.o mdio.o ethtool.o
else ifeq ($(RTCONFIG_QCN550X)$(RTCONFIG_SWITCH_QCA8337N),yy)
OBJS += qca8337_ssdk_sh.o
else ifeq ($(RTCONFIG_SOC_IPQ60XX),y)
OBJS += ipq60xx.o
else ifeq ($(RTCONFIG_SOC_IPQ50XX),y)
OBJS += ipq50xx.o
else
OBJS += qca8337.o
endif
OBJS += qca.o
ifeq ($(RTCONFIG_EXT_RTL8370MB),y)
CFLAGS += -I$(LINUXDIR)/drivers/char/rtl8370mb
OBJS += rtl8370mb.o mdio.o
endif
else ifeq ($(RTCONFIG_LANTIQ),y)
OBJS += flash_mtd.o
else
# Broadcom
OBJS += wl.o wl_linux.o
OBJS += broadcom.o api-broadcom.o
ifeq ($(RTCONFIG_DSL_TCLINUX),y)
OBJS += api-tclinux.o
endif
ifeq ($(RTCONFIG_EXT_RTL8365MB),y)
CFLAGS += -I$(LINUXDIR)/drivers/char/rtl8365mb
OBJS += rtl_8365mb.o
endif
ifeq ($(RTCONFIG_EXT_RTL8370MB),y)
CFLAGS += -I$(LINUXDIR)/drivers/char/rtl8365mb
OBJS += rtl_8365mb.o
endif
ifeq ($(RTCONFIG_STA_MONITOR),y)
OBJS += bcm_stamon.o
endif
ifeq ($(RTCONFIG_WBD),y)
OBJS += appevent_util.o
endif
ifeq ($(BCM_WLIMPL),69)
OBJS += event_utils.o
endif
endif

ifeq ($(RTCONFIG_USB),y)
OBJS += usb.o

ID_CFLAGS += -I$(TOP)/$(BUSYBOX_DIR)/include
ID_CFLAGS += -I$(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id
ID_CFLAGS += -include $(TOP)/$(BUSYBOX_DIR)/include/autoconf.h
ID_CFLAGS += -D_GNU_SOURCE -DNDEBUG -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
#ID_CFLAGS += -fdata-sections -ffunction-sections
ID_OBJS  = $(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/util.o
ID_OBJS += $(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/linux_swap.o
ID_OBJS += $(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/ext.o
ID_OBJS += $(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/fat.o
ID_OBJS += $(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/ntfs.o
ID_OBJS += $(if $(RTCONFIG_HFS),$(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/hfs.o)
ID_OBJS += $(if $(RTCONFIG_HFS),$(patsubst %.c,%.o,$(wildcard $(TOP)/$(BUSYBOX_DIR)/libbb/hash_md5.c)))
ID_OBJS += $(patsubst %.c,%.o,$(wildcard $(TOP)/$(BUSYBOX_DIR)/libbb/bb_bswap_64.c))
ID_OBJS += $(if $(RTCONFIG_USB_CDROM),$(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/udf.o)
ID_OBJS += $(if $(RTCONFIG_USB_CDROM),$(TOP)/$(BUSYBOX_DIR)/util-linux/volume_id/iso9660.o)
ID_OBJS += volume_id.o
#OBJS += volume_id.lib.o
OBJS += $(notdir $(ID_OBJS))

CFLAGS += -I$(TOP)/libdisk
ifeq ($(RTCONFIG_USB_MODEM),y)
OBJS += at_cmd.o
endif
endif

ifeq ($(RTCONFIG_BCMARM),y)
OBJS += ifaddrs.o
endif

ifeq ($(RTCONFIG_BLINK_LED),y)
CFLAGS	+= -I$(TOP)/bled
OBJS	+= bled.o
endif

ifeq ($(RTCONFIG_SAVEJFFS),y)
OBJS	+= jffs_cfgs.o
endif

ifeq ($(or $(RTCONFIG_QCA_PLC_UTILS),$(RTCONFIG_QCA_PLC2)),y)
OBJS	+= plc_utils.o
endif

ifeq ($(RTCONFIG_TCODE),y)
OBJS	+= tcode.o
endif

OBJS	+= shutils_private.o

ifeq ($(RTCONFIG_ISP_CUSTOMIZE),y)
OBJS	+=customize_shared.o
endif

OBJS += spwenc.o

ifeq ($(CONFIG_BCMWL5),y)
ifneq ($(RTCONFIG_DSL_REMOTE)$(HND_ROUTER),y)
CFLAGS += -DTRX_NEW
endif
OBJS += private.o
ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
OBJS += wlif_utils_ax.o linux_timer_ax.o
else
OBJS += wlif_utils.o linux_timer.o
endif
OBJS += UdpLib.o bcmutils.o
OBJS += bcmwifi_channels.o
endif

ifeq ($(HND_ROUTER),y)
OBJS += ethswctl.o ethctl_cmd.o
OBJS += memaccess.o
endif

ifeq ($(or $(RTCONFIG_BCM_7114),$(HND_ROUTER)),y)
OBJS += bcm_usched.o bcm_stamon.o
endif

ifeq ($(RTCONFIG_LP5523),y)
OBJS += lp5523led.o
endif

ifeq ($(RTCONFIG_AMAS),y)
CFLAGS += -I$(TOP)/shared/sysdeps/amas
ifeq ($(RTCONFIG_DWB),y)
ifeq ($(RTCONFIG_BCMBSD),y)
CFLAGS += -DSMART_CONNECT
endif
OBJS += amas_dwb.o
endif
OBJS += amas_utils.o
ifeq ($(RTCONFIG_AMASDB),y)
CFLAGS += -DAMASDB
endif
vpath %.c sysdeps/amas
endif

ifeq ($(RTCONFIG_SCHED_V2),y)
OBJS += sched_v2.o
endif

vpath %.c sysdeps $(SRCBASE)/shared/ $(WLAN_ComponentSrcDirs)
ifeq ($(RTCONFIG_RALINK),y)
ifneq ($(RTCONFIG_DSL_REMOTE),y)
CFLAGS += -DTRX_NEW
endif
OBJS += private.o
vpath %.c sysdeps/ralink
else ifeq ($(RTCONFIG_QCA),y)
CFLAGS += $(if $(IPQ40XX),,-DTRX_NEW)
OBJS += private.o
vpath %.c sysdeps/qca
else ifeq ($(RTCONFIG_LANTIQ),y)
OBJS	+= api-lantiq.o lantiq.o private.o
vpath %.c sysdeps/lantiq
else
vpath %.c sysdeps/broadcom  $(SRCBASE)/shared/bcmwifi/src
ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
OBJS += bcmxtlv.o bcmutils.o
ifeq ($(or $(RTAX55),$(RTAX1800),$(RTAX58U_V2)),y)
OBJS += rtkswitch.o
endif
endif
endif

ifeq ($(RTCONFIG_SW_HW_AUTH),y)
OBJS += sw_auth.o hw_auth.o
CFLAGS  += -I$(TOP)/sw-hw-auth
vpath %.c $(TOP)/sw-hw-auth
endif

ifeq ($(RTCONFIG_GEFORCENOW), y)
OBJS += nvgfn.o
endif
ifeq ($(RTCONFIG_BRCM_HOSTAPD),y)
OBJS += common_utils.o
endif

ifeq ($(or $(RTCONFIG_JFFS_NVRAM),$(RTCONFIG_VAR_NVRAM)),y)
OBJS += nvpriv.o
endif

ifeq ($(HND_ROUTER_AX_675X),y)
ifneq ($(RTCONFIG_SDK502L07P1_121_37),y)
OBJS += event_utils.o
endif
endif

ifeq ($(OPDBG),y)
CFLAGS += -DOPDBG
endif

ifeq ($(RT4GAC86U),y)
OBJS += $(LIBDIR)/libc_nonshared.a
endif

ifneq ($(CUSTOM_MODEL),)
CFLAGS += -DCUSTOM_MODEL
endif


.IGNORE:
ifneq ($(wildcard prebuild/$(BUILD_NAME)/*.o),)
	-cp -f prebuild/$(BUILD_NAME)/*.o .
endif
	-cp -f $(TOP)/wireless_tools/iwlib.c .
ifneq ($(wildcard ./tcode.c),)
	-rm -f ./tcode.o
endif
ifneq ($(wildcard ./sysdeps/ralink/private.c),)
	-rm -f ./private.o
endif
ifneq ($(wildcard ./notify_ahs.c),)
	-rm -f ./notify_ahs.o
endif
ifneq ($(wildcard ./notify_rc.c),)
	-rm -f ./notify_rc.o
endif
ifneq ($(wildcard ./shutils_private.c),)
	-rm -f ./shutils_private.o
endif
ifneq ($(wildcard ./spwenc.c),)
	-rm -f ./spwenc.o
endif

all: .IGNORE libshared.so libshared.a

libshared.so: $(OBJS)
	@echo " [shared] LD $@"
ifeq ($(HND_ROUTER),y)
	@$(CC) $(LDFLAGS) $(EXTRA_LD_FLAGS) -shared -o $@ $^
else
	@$(LD) $(LDFLAGS) -shared -o $@ $^
endif

	$(SIZECHECK)
	$(CPTMP)

libshared.a: $(OBJS)
	@echo " [shared] AR $@"
	@rm -f $@
	@$(AR) cru $@ $^

install: all
	@echo " [shared] Installing as $(INSTALLDIR)/usr/lib/libshared.so"
	@install -d $(INSTALLDIR)/usr/lib
	@install -m 755 libshared.so $(INSTALLDIR)/usr/lib
	@$(STRIP) $(INSTALLDIR)/usr/lib/libshared.so

clean:
	rm -f *.o *.so *.a .*.depend *.prep sysdeps/*.o sysdeps/broadcom/*.o sysdeps/ralink/*.o

sw_auth.o:
	cp -f $(TOP)/sw-hw-auth/sw_auth.o ./

hw_auth.o:
	cp -f $(TOP)/sw-hw-auth/hw_auth.o ./

ifeq ($(RTCONFIG_USB),y)
vpath %.c $(sort $(dir $(ID_OBJS)))

$(notdir $(ID_OBJS)): %.o: %.c .%.depend
	@echo " [shared] CC $@"
	@$(CC) $(CFLAGS) $(ID_CFLAGS) -o $@ -c $<

$(patsubst %.o,.%.depend,$(notdir $(ID_OBJS))): .%.depend: %.c
	@$(CC) $(CFLAGS) $(ID_CFLAGS) -M $< > $@

# ld -r --gc-sections is broken, so don't use it for now
#volume_id.lib.o: $(notdir $(ID_OBJS))
#	@echo " [shared] LD $@"
#	@$(LD) -r -e find_label_or_uuid --gc-sections -o $@ $(notdir $(ID_OBJS))
endif

%.o: %.c .%.depend
	@echo " [shared] CC $@"
	@$(CC) $(CFLAGS) -o $@ -c $<

prebuild/%.o:
	#nothing to do...

.depend: $(OBJS:%.o=%.c)
	@$(CC) $(CFLAGS) -M $^ > .depend

ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
ifneq ($(wildcard prebuild/bcmutils.o),)
bcmutils.o: prebuild/bcmutils.o
	cp prebuild/$@ .
else
.bcmutils.depend: bcmutils.c
	@$(CC) $(CFLAGS) -M $< > $@
endif
endif

ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
ifneq ($(wildcard prebuild/bcmwifi_channels.o),)
bcmwifi_channels.o: prebuild/bcmwifi_channels.o
	cp prebuild/$@ .
else
.bcmwifi_channels.depend: bcmwifi_channels.c
	@$(CC) $(CFLAGS) -M $< > $@
endif
endif

ifeq ($(RTCONFIG_HND_ROUTER_AX),y)
ifneq ($(wildcard prebuild/bcmxtlv.o),)
bcmxtlv.o: prebuild/bcmxtlv.o
	cp prebuild/$@ .
else
.bcmxtlv.depend: bcmxtlv.c
	@$(CC) $(CFLAGS) -M $< > $@
endif
endif

.%.depend: %.c
	@$(CC) $(CFLAGS) -M $< > $@

.defaults.depend .tcode.depend .hw_auth.depend: .%.depend: %.c %.prep
	@$(CC) $(CFLAGS) -M $< > $@

%.prep: %.c
	@$(CC) $(CFLAGS) -E $< > $@

ifneq ($(RT4GAC86U),y)
-include $(OBJS:%.o=.%.depend)
endif

