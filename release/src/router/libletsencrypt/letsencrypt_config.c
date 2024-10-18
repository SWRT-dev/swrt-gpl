/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/*
 * Copyright 2021-2022, ASUS
 * Copyright 2023, SWRTdev
 * Copyright 2023, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <rtconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <bcmparams.h>
#include <utils.h>
#include <shutils.h>
#include <shared.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/asn1.h>
#include "letsencrypt_config.h"
#include "letsencrypt_control.h"
#include "letsencrypt_utils.h"

void update_le_st(le_st_t status)
{
	nvram_set_int("le_state", status);
}

void update_le_sts(le_sts_t status)
{
	nvram_set_int("le_state_t", status);
}

void update_le_sbsts(le_sbsts_t status)
{
	nvram_set_int("le_sbstate_t", status);
}

void update_le_auxsts(le_auxsts_t status)
{
	nvram_set_int("le_auxstate_t", status);
}

le_auxsts_t check_le_status(void)
{
	int wanunit;

	wanunit = wan_primary_ifunit();
	if(nvram_match("wans_mode", "lb")){
		wanunit = nvram_get_int("ddns_wan_unit");
		if(wanunit > 1){
			if(get_first_connected_public_wan_unit() <= 1)
				wanunit = get_first_connected_public_wan_unit();
		}
	}
	if(is_wan_connect(wanunit)){
		if(nvram_match("ntp_ready", "1")){
			if(!nvram_match("le_acme_auth", "http") || nvram_match("ddns_status", "1"))
				return LE_AUX_NONE;
			logmessage("Let's Encrypt", "Err, DDNS update failed.");
			return LE_AUX_DDNS;
		}
		logmessage("Let's Encrypt", "Err, NTP time not sync.");
		return LE_AUX_NTP;
	}
	logmessage("Let's Encrypt", "Err, internet disconnected.");
	return LE_AUX_INTERNET;
}

le_conf_t* get_le_conf(le_conf_t* conf)
{
	conf->enable = nvram_match("le_enable", "1") != 0;
	if(nvram_match("le_acme_auth", "dns") || nvram_match("ddns_server_x", "WWW.ASUS.COM") || nvram_match("ddns_server_x", "WWW.ASUS.COM.CN"))
		conf->authtype = LE_ACME_AUTH_DNS;
	else if(nvram_match("le_acme_auth", "tls"))
		conf->authtype = LE_ACME_AUTH_TLS;
	else
		conf->authtype  = LE_ACME_AUTH_HTTP;
	conf->ecc = nvram_get_int("le_acme_ecc") != 0;
	conf->force = nvram_get_int("le_acme_force") != 0;
	conf->renew_force = nvram_get_int("le_acme_renew_force") != 0;
	conf->staging = nvram_get_int("le_acme_stage") != 0;
	if(nvram_get_int("le_acme_debug") == 1)
		conf->debug = 1;
	else if(nvram_get_int("le_acme_debug") < 1)
		conf->debug = 0;
	else
		conf->debug = 2;
	snprintf(conf->acme_logpath, sizeof(conf->acme_logpath), "%s", nvram_safe_get("le_acme_logpath"));
	conf->ddns_enabled = nvram_get_int("ddns_enable_x") != 0;
	snprintf(conf->ddns_hostname, sizeof(conf->ddns_hostname), "%s", nvram_safe_get("ddns_hostname_x"));
	conf->https_enabled = nvram_get_int("http_enable") > 0;
	if(nvram_get_int("enable_webdav") || nvram_get_int("webdav_aidisk") || nvram_get_int("webdav_proxy"))
		conf->webdav_enabled = 1;
	else
		conf->webdav_enabled = 0;
	snprintf(conf->lan_ipaddr, sizeof(conf->lan_ipaddr), "%s", nvram_safe_get("lan_ipaddr"));
	return conf;
}

int check_le_configure(le_conf_t* conf)
{
	if(conf->enable == 1){
		if(conf->ddns_enabled == 1 && nvram_get_int("sw_mode") == 1)
			return 0;
		update_le_auxsts(LE_AUX_CONFIG);
	}else
		update_le_sts(LE_STS_DISABLED);
	return -1;
}

char* get_path_le_domain_fullchain(char* path, size_t len)
{
	snprintf(path, len, "%s/%s%s/%s", LE_ACME_CERT_HOME, nvram_safe_get("ddns_hostname_x"), nvram_get_int("le_acme_ecc") ? "_ecc" : "", LE_ACME_DOMAIN_FULLCHAIN);
	return path;
}

char* get_path_le_domain_cert(char* path, size_t len)
{
	snprintf(path, len, "%s/%s%s/%s.cer", LE_ACME_CERT_HOME, nvram_safe_get("ddns_hostname_x"), nvram_get_int("le_acme_ecc") ? "_ecc" : "", nvram_safe_get("ddns_hostname_x"));
	return path;
}

char* get_path_le_domain_key(char* path, size_t len)
{
	snprintf(path, len, "%s/%s%s/%s", LE_ACME_CERT_HOME, nvram_safe_get("ddns_hostname_x"), nvram_get_int("le_acme_ecc") ? "_ecc" : "", LE_ACME_DOMAIN_KEY);
	return path;
}

