
all:
	$(MAKE) -C libeasy && $(MAKE) -C libeasy stage
	$(MAKE) -C libwifi && $(MAKE) -C libwifi stage
	$(MAKE) -C swrtmesh-utils && $(MAKE) -C swrtmesh-utils stage
	$(MAKE) -C wifimngr && $(MAKE) -C libwifi stage
	$(MAKE) -C ieee1905 && $(MAKE) -C ieee1905 stage
	$(MAKE) -C map-controller
	$(MAKE) -C map-agent
	$(MAKE) -C map-topology

install: libeasy-install libwifi-install swrtmesh-utils-install wifimngr-install ieee1905-install map-agent-install map-controller-install map-topology-install

clean: libeasy-clean libwifi-clean swrtmesh-utils-clean wifimngr-clean ieee1905-clean map-agent-clean map-controller-clean map-topology-clean

%-install:
	@echo $*
	@[ ! -d $* ] || $(MAKE) -C $* install INSTALLDIR=$(INSTALLDIR)

%-clean:
	-@[ ! -d $* ] || $(MAKE) -C $* clean

