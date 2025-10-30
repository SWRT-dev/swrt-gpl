qca-install-header:
	mkdir -p header
	[ ! -d $(SDK_DIR_PLATFORM)/header ] || cp -rf $(SDK_DIR_PLATFORM)/header/* header/
	[ ! -d $(SDK_DIR_PLATFORM)/header ] || cp -rf header/wlan/$(PLATFORM_ROUTER) header/wlan/fwcommon

libroxml:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

iwinfo:
	$(MAKE) -C $@
	$(MAKE) -C $@ stage

qca-cfg80211:iw libroxml
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libqcacommon:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-qmi-framework:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-cnss:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-cnss-daemon:qca-qmi-framework
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-ssdk:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-dp:qca-ssdk
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-crypto:qca-nss-drv
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-ovsmgr:
#disable openvswitch
#	$(MAKE) -C qca-ovsmgr
#	$(MAKE) -C qca-ovsmgr stage

emesh-sp:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-hyfi-bridge:emesh-sp
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-mcs-lkm:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-ecm:qca-nss-drv qca-hyfi-bridge qca-mcs-lkm
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-clients:qca-nss-crypto qca-nss-ecm qca-nss-cfi qca-wifi
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-macsec:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-drv: qca-nss-dp
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

nat46:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-ppe: nat46 qca-ssdk
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-nss-cfi:qca-nss-drv qca-nss-crypto
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-acfg:qca-wifi
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-wrapd:qca-cfg80211
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-hostap: qca-acfg qca-cnss-daemon qca-nss-macsec
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-wifi:qca-install-header libroxml qca-cfg80211 qca-cnss qca-cnss-daemon qca-nss-drv
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-ssdk-shell:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-mcs-apps:qca-wifi qca-ssdk qca-mcs-lkm libhyficommon
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libieee1905:libhyficommon
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

breakpad-wrapper: breakpad
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

breakpad:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libathtestcmd:libtcmd libtlv2 libtlvutil
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libtcmd:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

myftm:libathtestcmd libtcmd
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

athtestcmd-lith:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

ftm:libtcmd qca-diag libprovision
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libtlv2:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libtlvutil:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libprovision:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

464xlat:nat46
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-diag:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-whc-lbd:qca-wifi libqcacommon libwifisoncfg qca-wifison-ext-lib libieee1905
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libwifisoncfg:qca-cfg80211 libhyficommon
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-wifison-ext-lib:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi
