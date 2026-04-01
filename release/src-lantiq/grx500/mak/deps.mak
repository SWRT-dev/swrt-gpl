ltq-wlan-wave_6x:dwpal_6x wav_api_6x iwlwav-hostap iwlwav-iw iwlwav-tool iwlwav-driver iwlwav-firmware libuuid uuidgen
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-driver:directconnect_dp
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-hostap:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-iw:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-firmware:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-tool:dwpal_6x
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

dwpal_6x:iwlwav-hostap iwlwav-iw
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

swpal_6x:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

directconnect_dp:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

ppa_drv:eip97
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

eip97:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

ltq_fw_phy_ip:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

switch_cli:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

upgrade_utils:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

