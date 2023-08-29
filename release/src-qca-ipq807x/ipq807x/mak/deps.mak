qca-install-header:
	mkdir -p header
	cp -rf $(SDK_DIR_PLATFORM)/header/* header/

libroxml:
	$(MAKE) -C libroxml
	$(MAKE) -C libroxml stage

iwinfo:
	$(MAKE) -C iwinfo
	$(MAKE) -C iwinfo stage

qca-cfg80211:iw libroxml
	$(MAKE) -C qca-cfg80211
	$(MAKE) -C qca-cfg80211 stage

libqcacommon:
	$(MAKE) -C libqcacommon
	$(MAKE) -C libqcacommon stage

qca-qmi-framework:
	$(MAKE) -C qca-qmi-framework
	$(MAKE) -C qca-qmi-framework stage

qca-cnss-daemon:qca-qmi-framework
	$(MAKE) -C qca-cnss-daemon
	$(MAKE) -C qca-cnss-daemon stage

qca-ssdk:
	$(MAKE) -C qca-ssdk
	$(MAKE) -C qca-ssdk stage

qca-nss-dp:qca-ssdk
	$(MAKE) -C qca-nss-dp
	$(MAKE) -C qca-nss-dp stage

qca-nss-crypto:qca-nss-drv
	$(MAKE) -C qca-nss-crypto
	$(MAKE) -C qca-nss-crypto stage

qca-ovsmgr:
#disable openvswitch
#	$(MAKE) -C qca-ovsmgr
#	$(MAKE) -C qca-ovsmgr stage

emesh-sp:
	$(MAKE) -C emesh-sp
	$(MAKE) -C emesh-sp stage

qca-hyfi-bridge:emesh-sp
	$(MAKE) -C qca-hyfi-bridge
	$(MAKE) -C qca-hyfi-bridge stage

qca-mcs-lkm:
	$(MAKE) -C qca-mcs-lkm
	$(MAKE) -C qca-mcs-lkm stage

qca-nss-ecm:qca-nss-drv qca-hyfi-bridge qca-mcs-lkm
	$(MAKE) -C qca-nss-ecm
	$(MAKE) -C qca-nss-ecm stage

qca-nss-clients:qca-nss-crypto qca-nss-ecm qca-nss-cfi-ocf qca-nss-cfi-cryptoapi qca-wifi
	$(MAKE) -C qca-nss-clients
	$(MAKE) -C qca-nss-clients stage

qca-nss-drv: qca-nss-dp
	$(MAKE) -C qca-nss-drv
	$(MAKE) -C qca-nss-drv stage

qca-acfg:qca-wifi
	$(MAKE) -C qca-acfg

qca-hostap:qca-acfg qca-wrapd athtestcmd-lith myftm qca-iface-mgr qca-wapid qca-lowi athdiag qca-spectral qca-wpc qca-cnss-daemon
	$(MAKE) -C qca-hostap

qca-wifi:qca-install-header libroxml qca-cfg80211 qca-cnss-daemon qca-nss-drv
	$(MAKE) -C qca-wifi
	$(MAKE) -C qca-wifi stage
