
hostapd:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

8021xd:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

bndstrg_plus:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

iw:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

mhz:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

mii_mgr:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

regs:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

switch:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
	$(MAKE) -C $@ ; \
	fi

