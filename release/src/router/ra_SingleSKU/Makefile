
SKUDIR = $(INSTALLDIR)/ra_SKU

all clean:
	@if [ ! -d $(BUILD_NAME) ]; then \
		echo "No SingleSKU for $(BUILD_NAME)"; \
		exit 1; \
	fi

install: all
	-rm -rf $(TARGETDIR)/ra_SKU
	-rm -rf $(SKUDIR)
	@install -d $(SKUDIR)
ifneq ($(wildcard $(BUILD_NAME)),)
	@install $(BUILD_NAME)/l1profile.dat $(SKUDIR)/l1profile.dat
ifeq ($(or $(MT7986A),$(MT7986B),$(MT7986C)),y)
	@install $(BUILD_NAME)/mt7986-sku.dat $(SKUDIR)/mt7986-sku.dat
	@install $(BUILD_NAME)/mt7986-sku-bf.dat $(SKUDIR)/mt7986-sku-bf.dat
else ifeq ($(MT7981),y)
	@install $(BUILD_NAME)/mt7981-sku.dat $(SKUDIR)/mt7981-sku.dat
	@install $(BUILD_NAME)/mt7981-sku-bf.dat $(SKUDIR)/mt7981-sku-bf.dat
else
	@install $(BUILD_NAME)/SingleSKU_mt7615e-sku.dat $(SKUDIR)/SingleSKU_mt7615e-sku.dat
	@install $(BUILD_NAME)/SingleSKU_mt7615e-sku-bf.dat $(SKUDIR)/SingleSKU_mt7615e-sku-bf.dat
endif
else
	@install l1profile.dat $(SKUDIR)/l1profile.dat
ifeq ($(or $(MT7986A),$(MT7986B),$(MT7986C)),y)
	@install mt7986-sku.dat $(SKUDIR)/mt798x-sku.dat
	@install mt7986-sku-bf.dat $(SKUDIR)/mt798x-sku-bf.dat
else ifeq ($(MT7981),y)
	@install mt7981-sku.dat $(SKUDIR)/mt798x-sku.dat
	@install mt7981-sku-bf.dat $(SKUDIR)/mt798x-sku-bf.dat
endif
endif




.PHONY: all clean install
.PHONY: $(BUILD_NAME)
