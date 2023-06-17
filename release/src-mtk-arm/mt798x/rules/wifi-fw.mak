

wifi-fw:
	@true

wifi-fw-install:
	install -d $(INSTALLDIR)/wifi-fw/rom/firmware/
ifneq ($(wildcard $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_iPAiLNA_EEPROM_AX6000.bin),)
ifeq ($(or $(RTCONFIG_SOC_MT7986A),$(RTCONFIG_SOC_MT7986B),$(RTCONFIG_SOC_MT7986C)),y)
ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TUF-AX6000 RM-AX6000 TX-AX6000))
#ax6000 ax8400
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_iPAiLNA_EEPROM_AX6000.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX6000.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX8400.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TEST-AX7800))
#ax7800
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_iPAiLNA_EEPROM_AX7800.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX7800.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TUF-AX4200 RT-AX59U))
#ax4200
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_ONEADIE_DBDC.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
endif
#warp
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/warp_driver/warp/bin/7986_WOCPU0_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/warp_driver/warp/bin/7986_WOCPU1_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986_MT7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/mt7986_patch_e1_hdr.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/mt7986_patch_e1_hdr_mt7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7986/rebb/7986_WACPU_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(RTCONFIG_SOC_MT7981),y)
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7981/rebb/MT7981_iPAiLNA_EEPROM.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7981/rebb/MT7981_ePAeLNA_EEPROM.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/warp_driver/warp/bin/7981_WOCPU0_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7981/rebb/WIFI_RAM_CODE_MT7981.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7981/rebb/7981_WACPU_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt798x/7.6.7.0/bin/mt7981/rebb/mt7981_patch_e1_hdr.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
endif
else
ifeq ($(or $(RTCONFIG_SOC_MT7986A),$(RTCONFIG_SOC_MT7986B),$(RTCONFIG_SOC_MT7986C)),y)
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986_TESTMODE.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986_TESTMODE_MT7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/mt7986_patch_e1_hdr_testmode.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/mt7986_patch_e1_hdr_testmode_mt7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/WIFI_RAM_CODE_MT7986_MT7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/mt7986_patch_e1_hdr.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/mt7986_patch_e1_hdr_mt7975.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/7986_WACPU_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TUF-AX6000 RM-AX6000 TX-AX6000))
#ax6000 ax8400
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_iPAiLNA_EEPROM_AX6000.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX6000.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX8400.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TEST-AX7800))
#ax7800
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_iPAiLNA_EEPROM_AX7800.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_AX7800.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(BUILD_NAME), $(filter $(BUILD_NAME), TUF-AX4200 RT-AX59U))
#ax4200
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7986/rebb/MT7986_ePAeLNA_EEPROM_ONEADIE_DBDC.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
endif
#warp
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/warp_driver/warp/bin/7986_WOCPU0_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/warp_driver/warp/bin/7986_WOCPU1_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
else ifeq ($(RTCONFIG_SOC_MT7981),y)
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/WIFI_RAM_CODE_MT7981.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/7981_WACPU_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/mt7981_patch_e1_hdr.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/WIFI_RAM_CODE_MT7981_TESTMODE.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
#	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/mt7981_patch_e1_hdr_testmode.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/MT7981_iPAiLNA_EEPROM.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/bin/mt7981/rebb/MT7981_ePAeLNA_EEPROM.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
	install -D $(LINUXDIR)/drivers/net/wireless/mediatek/mt7986/warp_driver/warp/bin/7981_WOCPU0_RAM_CODE_release.bin $(INSTALLDIR)/wifi-fw/rom/firmware/
endif
endif

wifi-fw-clean:
	@true

