qca-install-header:
	mkdir -p header
	[ ! -d qca-wifi-fw-10.4 ] || cp -rf qca-wifi-fw-10.4/src/* header/


libroxml:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

iwinfo:
	$(MAKE) -C $@
	$(MAKE) -C $@ stage

libqcacommon:
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

nat46:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-wrapd:qca-hostap
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-hostap: qca-wifi
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-wifi:qca-install-header
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

libieee1905:libhyficommon
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

qca-whc-lbd:qca-wifi libqcacommon libieee1905 libwifisoncfg
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

libwifisoncfg:libhyficommon
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

qca-edma:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

qca-swconfig.ipq40xx:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

shortcut-fe:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	$(MAKE) -C $@ stage ; \
	fi

