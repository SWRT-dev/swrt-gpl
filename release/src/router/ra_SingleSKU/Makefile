-include $(TOP)/.config
SKUDIR = $(INSTALLDIR)/ra_SKU

all clean:

install: all
	-rm -rf $(TARGETDIR)/ra_SKU
	-rm -rf $(SKUDIR)
	@install -d $(SKUDIR)
#7603+7615 or different eeprom offset
ifneq ($(wildcard $(BUILD_NAME)/l1profile.dat),)
	@install $(BUILD_NAME)/l1profile.dat $(SKUDIR)/l1profile.dat
	@install $(BUILD_NAME)/SingleSKU_mt7615e-sku.dat $(SKUDIR)/SingleSKU_mt7615e-sku.dat
	@install $(BUILD_NAME)/SingleSKU_mt7615e-sku-bf.dat $(SKUDIR)/SingleSKU_mt7615e-sku-bf.dat
else
ifeq ($(RTCONFIG_SOC_MT7981),y)
	@install MT7981/l1profile.dat $(SKUDIR)/l1profile.dat
	@install MT7981/mt7981-sku.dat $(SKUDIR)/mt7981-sku.dat
	@install MT7981/mt7981-sku-bf.dat $(SKUDIR)/mt7981-sku-bf.dat
else ifeq ($(RTCONFIG_MT798X),y)
	@install MT7986/l1profile.dat $(SKUDIR)/l1profile.dat
	@install MT7986/mt7986-sku.dat $(SKUDIR)/mt7986-sku.dat
	@install MT7986/mt7986-sku-bf.dat $(SKUDIR)/mt7986-sku-bf.dat
else ifeq ($(RTCONFIG_WLMODULE_MT7915D_AP),y)
	@install MT7915/l1profile.dat $(SKUDIR)/l1profile.dat
	@install MT7915/SingleSKU_mt7615e-sku.dat $(SKUDIR)/SingleSKU_mt7615e-sku.dat
	@install MT7915/SingleSKU_mt7615e-sku-bf.dat $(SKUDIR)/SingleSKU_mt7615e-sku-bf.dat
else ifeq ($(RTCONFIG_WLMODULE_MT7615E_AP),y)
	@install MT7615/l1profile.dat $(SKUDIR)/l1profile.dat
	@install MT7615/SingleSKU_mt7615e-sku.dat $(SKUDIR)/SingleSKU_mt7615e-sku.dat
	@install MT7615/SingleSKU_mt7615e-sku-bf.dat $(SKUDIR)/SingleSKU_mt7615e-sku-bf.dat
else
	$(error wifi chip is not defined)
endif
endif




.PHONY: all clean install
.PHONY: $(BUILD_NAME)
