# Helper makefile for building Broadcom wps libaries
# This file maps wps feature flags (import) to WPSFLAGS and WPSFILES (export).
#
# Broadcom Proprietary and Confidential. Copyright (C) 2016,
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom.
# $Id: $

WPSFILES :=

ifeq ($(BLDTYPE), debug)
WPSFLAGS += -D_TUDEBUGTRACE
endif

ifeq ($(WCN_NET), 1)
WPSFLAGS += -DWCN_NET_SUPPORT   
endif

# Include external openssl path
ifeq ($(EXTERNAL_OPENSSL),1)
WPS_CRYPT = 0
WPSFLAGS += -DEXTERNAL_OPENSSL
WPSFILES += wps/common/shared/wps_openssl.c
endif

## wps common

## shared code 
WPSFILES += wps/common/shared/tutrace.c
WPSFILES += wps/common/shared/dev_config.c
WPSFILES += wps/common/shared/wps_sslist.c
WPSFILES += wps/common/enrollee/enr_reg_sm.c
WPSFILES += wps/common/registrar/reg_sm.c 
WPSFILES += wps/common/shared/reg_proto_utils.c
WPSFILES += wps/common/shared/reg_proto_msg.c
WPSFILES += wps/common/shared/tlv.c
WPSFILES += wps/common/shared/state_machine.c
WPSFILES += wps/common/shared/wps_utils.c
WPSFILES += wps/common/shared/ie_utils.c
WPSFILES += wps/common/shared/buffobj.c

# AP or APSTA
ifeq ($(WPS_AP), 1)
WPSFLAGS += -DBCMWPSAP
WPSFILES += wps/common/ap/ap_api.c
WPSFILES += wps/common/ap/ap_ssr.c
WPSFILES += wps/common/ap/ap_eap_sm.c
endif

# STA supports
ifeq ($(WPS_STA), 1)
WPSFLAGS += -DBCMWPSAPSTA
WPSFILES += wps/common/sta/sta_eap_sm.c
endif

# WPS monitor support
ifeq ($(WPS_ROUTER), 1)
WPSFILES += wps/brcm_apps/apps/wps_monitor.c
WPSFILES += wps/brcm_apps/apps/wps_aplockdown.c
WPSFILES += wps/brcm_apps/apps/wps_pb.c
WPSFILES += wps/brcm_apps/apps/wps_eap.c
WPSFILES += wps/brcm_apps/apps/wps_ie.c
WPSFILES += wps/brcm_apps/apps/wps_ui.c
WPSFILES += wps/brcm_apps/apps/wps_led.c

WPS_ROUTERHALFILES += wps/brcm_apps/arch/bcm947xx/wps_gpio.c
WPS_ROUTERHALFILES += wps/brcm_apps/arch/bcm947xx/wps_hal.c
WPS_ROUTERHALFILES += wps/brcm_apps/arch/bcm947xx/wps_wl.c

	# WFI supports
	ifeq ($(WPS_WFI),1)
	WPSFILES += wps/brcm_apps/apps/wps_wfi.c
	WPSFLAGS += -DBCMWFI
	endif

	ifeq ($(WPS_AP), 1)
	WPSFILES += wps/brcm_apps/apps/wps_ap.c 
		ifeq ($(WPS_UPNP_DEVICE),1)
			WPSFILES += wps/brcm_apps/upnp/WFADevice/soap_x_wfawlanconfig.c
			WPSFILES += wps/brcm_apps/upnp/WFADevice/WFADevice.c
			WPSFILES += wps/brcm_apps/upnp/WFADevice/WFADevice_table.c
			WPSFILES += wps/brcm_apps/upnp/WFADevice/xml_x_wfawlanconfig.c
			# Release xml_WFADevice.c for customization
			WPS_ROUTERHALFILES += wps/brcm_apps/upnp/WFADevice/xml_WFADevice.c
			WPSFILES += wps/common/ap/ap_upnp_sm.c
			WPSFILES += wps/brcm_apps/apps/wps_libupnp.c
			WPSFLAGS += -DWPS_UPNP_DEVICE
		endif
	endif

	ifeq ($(WPS_STA), 1)
	WPSFILES += wps/brcm_apps/apps/wps_sta.c
	WPS_ROUTERHALFILES += wps/brcm_apps/arch/bcm947xx/wps_sta_wl.c
	endif

	# NFC support
	ifeq ($(WPS_NFC_DEVICE), 1)
	WPSFILES += wps/brcm_apps/apps/wps_nfc.c
	WPSFILES += wps/brcm_apps/nfc/app_generic.c
	WPSFILES += wps/brcm_apps/nfc/app_mgt.c
	WPSFILES += wps/brcm_apps/nfc/app_nsa_utils.c
	endif
WPSFLAGS += -DWPS_ROUTER
endif # end WPS ROUTER

# Enrollee supports
ifeq ($(WPS_ENR),1)
WPSFILES += wps/common/enrollee/enr_api.c
endif

ifeq ($(WPS_CRYPT), 1)
CRYPTDIR = $(SRCBASE)/bcmcrypto
WPSFILES += bcmcrypto/aes.c
WPSFILES += bcmcrypto/rijndael-alg-fst.c
WPSFILES += bcmcrypto/dh.c
WPSFILES += bcmcrypto/bn.c
WPSFILES += bcmcrypto/sha256.c
WPSFILES += bcmcrypto/hmac_sha256.c
WPSFILES += bcmcrypto/random.c
endif

# NFC support
ifeq ($(WPS_NFC_DEVICE), 1)
WPSFILES += wps/common/shared/nfc_utils.c
WPSFLAGS += -DWPS_NFC_DEVICE
endif

export WPS_FLAGS = $(WPSFLAGS)
export WPS_FILES = $(WPSFILES)
export WPS_HALFILES = $(WPS_ROUTERHALFILES)
