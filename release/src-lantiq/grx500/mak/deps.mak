ltq-wlan-wave_6x:dwpal_6x iwlwav-hostap iwlwav-iw iwlwav-tool iwlwav-driver iwlwav-firmware
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

ltq-wlan-wave_6x-8.4:dwpal_6x-8.4 iwlwav-hostap-8.4 iwlwav-iw-8.4 iwlwav-tool-8.4 iwlwav-driver-8.4 iwlwav-firmware-8.4
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-driver-8.4:directconnect_dp
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-hostap-8.4:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-iw-8.4:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-firmware-8.4:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

iwlwav-tool-8.4:dwpal_6x-8.4
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

dwpal_6x-8.4:iwlwav-hostap-8.4 iwlwav-iw-8.4
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

ppacmd:ppa_drv
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

libuuid:
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi

uuidgen:libuuid
	@$(SEP)
	if [ ! -f $@/stamp-h1 ];then \
		$(MAKE) -C $@ ; \
		$(MAKE) -C $@ stage ; \
	fi


