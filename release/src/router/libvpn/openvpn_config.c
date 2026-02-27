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
#include <shutils.h>
#include <shared.h>

#include "openvpn_config.h"
#include "openvpn_control.h"
#include "openvpn_utils.h"

extern struct nvram_tuple router_defaults[];

#ifdef RTCONFIG_MULTILAN_CFG
MTLAN_T *get_ovpn_mtlan(ovpn_type_t type, int unit, MTLAN_T *pmtl, size_t *mtl_sz)
{
	int idx;
	MTLAN_T *p;
	SDNFT_TYPE sdntype;

	if(type == OVPN_TYPE_CLIENT){
		idx = get_vpnc_idx_by_proto_unit(VPN_PROTO_OVPN, unit);
		sdntype = SDNFT_TYPE_VPNC;
	}else{
		idx = get_vpns_idx_by_proto_unit(VPN_PROTO_OVPN, unit);
		sdntype = SDNFT_TYPE_VPNS;
	}
	if(!get_mtlan_by_idx(sdntype, idx, pmtl, mtl_sz)){
		p = pmtl;
		pmtl = NULL;
		FREE_MTLAN(p);
		_dprintf("get mtlan by idx(%d) failed!!!\n", idx);
	}
	return pmtl;
}
#endif

ovpn_sconf_common_t* get_ovpn_sconf_common(ovpn_sconf_common_t* conf)
{
	char *p = NULL;
	char buf[32];

	memset(buf, 0, sizeof(buf));
	if(conf == NULL)
		return NULL;
	memset(conf, 0, sizeof(ovpn_sconf_common_t));
	if(nvram_match("VPNServer_enable", "1")){
		strlcpy(buf, nvram_safe_get("vpn_serverx_eas"), sizeof(buf));
		p = strtok(buf, ",");
		while(p != NULL){
			if(atoi(p) == 1)
				conf->enable[0] = 1;
			p = strtok(NULL, ",");
		}
		strlcpy(buf, nvram_safe_get("vpn_serverx_dns"), sizeof(buf));
		p = strtok(buf, ",");
		while(p != NULL){
			if(atoi(p) == 1)
				conf->dns[0] = 1;
			p = strtok(NULL, ",");
		}
	}
	return conf;
}

ovpn_cconf_common_t* get_ovpn_cconf_common(ovpn_cconf_common_t* conf)
{
	int i;
	char *p = NULL;
	char buf[32];
	memset(buf, 0, sizeof(buf));
	if(conf == NULL)
		return NULL;
	memset(conf, 0, sizeof(ovpn_sconf_common_t));
	strlcpy(buf, nvram_safe_get("vpn_clientx_eas"), sizeof(buf));
	p = strtok(buf, ",");
	while(p != NULL){
		i = atoi(p) - 1;
		if(i >= 0 && i < OVPN_CLIENT_MAX)
			conf->enable[i] = 1;
		p = strtok(NULL, ",");
	}
	return conf;
}

char *_filter_custom_config(char *buf, size_t len)
{
	int i;
	char *ptr = NULL, *pt = NULL, *pt2 = NULL;
	char *ovpn_script_option_list[] = {"ipchange", "route-up", "route-pre-down", "up", "down", "client-connect", "client-disconnect", "learn-address", 
		"auth-user-pass-verify", "tls-verify", ""};
	if(buf){
		ptr = calloc(1, len);
		if(ptr){
			pt = strtok(buf, "\n");
			while(pt != NULL){
				for(i = 0, pt2 = ovpn_script_option_list[0]; *pt2; i++, pt2 = ovpn_script_option_list[i]){
					if(strncmp(pt, pt2, strlen(pt2)))
						snprintf(&ptr[strlen(ptr)], len - strlen(ptr), "%s\n", pt);
				}
				pt = strtok(NULL, "\n");
			}
			strlcpy(buf, ptr, len);
			free(ptr);
			return buf;
		}
	}
	return NULL;
}

ovpn_sconf_t* get_ovpn_sconf(int unit, ovpn_sconf_t* conf)
{
	int port;
	char *ptr = NULL, *pt = NULL;
	char *enable = NULL, *name = NULL, *network = NULL, *netmask = NULL, *push = NULL;
	char buf[4096];
	char prefix[32];
#ifdef RTCONFIG_MULTILAN_CFG
	MTLAN_T *pmtl;
	size_t mtl_sz;
#endif

	memset(prefix, 0, sizeof(prefix));
	memset(buf, 0, sizeof(buf));
	if(conf == NULL)
		return NULL;
	memset(conf, 0, sizeof(ovpn_sconf_t));
	conf->enable = nvram_get_int("VPNServer_enable");
	snprintf(conf->progname, sizeof(conf->progname), "vpnserver%d", unit);
	snprintf(prefix, sizeof(prefix), "vpn_server%d_", unit);
	strlcpy(conf->proto, nvram_pf_safe_get(prefix, "proto"), sizeof(conf->proto));
	port = nvram_pf_get_int(prefix, "port");
	if(!port)
		port = 1194;
	conf->port = port;
	ptr = nvram_pf_get(prefix, "if");
	if(!ptr)
		ptr = "";
	snprintf(conf->if_name,  sizeof(conf->if_name), "%s%d", ptr, unit + OVPN_SERVER_BASE);
	conf->if_type = strncmp(conf->if_name, "tun", 3) != 0;
	strlcpy(conf->local, nvram_pf_safe_get(prefix, "local"), sizeof(conf->local));
	strlcpy(conf->remote, nvram_pf_safe_get(prefix, "remote"), sizeof(conf->remote));
	strlcpy(conf->local6, nvram_pf_safe_get(prefix, "local6"), sizeof(conf->local6));
	strlcpy(conf->remote6, nvram_pf_safe_get(prefix, "remote6"), sizeof(conf->remote6));
	ptr = nvram_pf_safe_get(prefix, "verb");
	if(*ptr)
		conf->verb = nvram_pf_get_int(prefix, "verb");
	else
		conf->verb = nvram_get_int("vpn_loglevel");
	strlcpy(conf->comp, nvram_pf_safe_get(prefix, "comp"), sizeof(conf->comp));
	if(nvram_pf_match(prefix, "crypt", "secret"))
		conf->auth_mode = OVPN_AUTH_STATIC;
	else
		conf->auth_mode = OVPN_AUTH_TLS;
	conf->useronly = nvram_pf_get_int(prefix, "igncrt");
	strlcpy(conf->network, nvram_pf_safe_get(prefix, "sn"), sizeof(conf->network));
	strlcpy(conf->netmask, nvram_pf_safe_get(prefix, "nm"), sizeof(conf->netmask));
	strlcpy(conf->network6, nvram_pf_safe_get(prefix, "sn6"), sizeof(conf->network6));
	conf->dhcp = nvram_pf_get_int(prefix, "dhcp");
	strlcpy(conf->pool_start, nvram_pf_safe_get(prefix, "r1"), sizeof(conf->pool_start));
	strlcpy(conf->pool_end, nvram_pf_safe_get(prefix, "r2"), sizeof(conf->pool_end));
	conf->redirect_gateway = nvram_pf_get_int(prefix, "rgw");
	conf->push_lan = nvram_pf_get_int(prefix, "plan");
	conf->push_dns = nvram_pf_get_int(prefix, "pdns");
	conf->ccd = nvram_pf_get_int(prefix, "ccd");
	conf->c2c = nvram_pf_get_int(prefix, "c2c");
	conf->ccd_excl = nvram_pf_get_int(prefix, "ccd_excl");
	strlcpy(buf, nvram_pf_safe_get(prefix, "ccd_val"), sizeof(buf));
	conf->ccd_info.count = 0;
	pt = buf;
	while((ptr = strsep(&pt, "<")) != NULL){
		if(vstrsep(ptr, ">", &enable, &name, &network, &netmask, &push) == 5){
			conf->ccd_info.ccd_val[conf->ccd_info.count].enable = atoi(enable);
			snprintf(conf->ccd_info.ccd_val[conf->ccd_info.count].name, sizeof(conf->ccd_info.ccd_val[0].name), name);
			snprintf(conf->ccd_info.ccd_val[conf->ccd_info.count].network, sizeof(conf->ccd_info.ccd_val[0].network), network);
			snprintf(conf->ccd_info.ccd_val[conf->ccd_info.count].netmask, sizeof(conf->ccd_info.ccd_val[0].netmask), netmask);
			conf->ccd_info.ccd_val[conf->ccd_info.count].push = atoi(push);
			++conf->ccd_info.count;
		}
	}
	conf->direction = nvram_pf_get_int(prefix, "hmac");
	strlcpy(conf->cipher, nvram_pf_safe_get(prefix, "cipher"), sizeof(conf->cipher));
	strlcpy(conf->digest, nvram_pf_safe_get(prefix, "digest"), sizeof(conf->digest));
	conf->ncp_enable = nvram_pf_get_int(prefix, "ncp_enable");
	conf->reneg = nvram_pf_get_int(prefix, "reneg");
	if(nvram_pf_get_int(prefix, "tls_keysize"))
		conf->tls_keysize = 2048;
	else
		conf->tls_keysize = 1024;
	strlcpy(conf->firewall, nvram_pf_safe_get(prefix, "firewall"), sizeof(conf->firewall));
	conf->poll = nvram_pf_get_int(prefix, "poll");
#ifdef RTCONFIG_MULTILAN_CFG
	pmtl = (MTLAN_T *)INIT_MTLAN(sizeof(MTLAN_T));
	if(get_ovpn_mtlan(OVPN_TYPE_SERVER, unit, pmtl, &mtl_sz)){
		if(mtl_sz != 1 && conf->if_type == OVPN_IF_TAP)
			_dprintf("%s: WARNING: MTLAN size %u\n", __func__, mtl_sz);
		strlcpy(conf->lan_ipaddr, pmtl->nw_t.addr, sizeof(conf->lan_ipaddr));
		strlcpy(conf->lan_netmask, pmtl->nw_t.netmask, sizeof(conf->lan_netmask));
		strlcpy(conf->lan_subnet, pmtl->nw_t.subnet, sizeof(conf->lan_subnet));
	}else
#endif
	{
		strlcpy(conf->lan_ipaddr, nvram_safe_get("lan_ipaddr"), sizeof(conf->lan_ipaddr));
		strlcpy(conf->lan_netmask, nvram_safe_get("lan_netmask"), sizeof(conf->lan_netmask));
		get_lan_subnet(conf->lan_subnet, sizeof(conf->lan_subnet));
	}
	if(get_ipv6_service())
		conf->ipv6_enable = nvram_pf_get_int(prefix, "ip6") != 0;
	else
		conf->ipv6_enable = 0;
	if(nvram_pf_get(prefix, "nat6"))
		conf->nat6 = nvram_pf_get_int(prefix, "nat6");
	else
		conf->nat6 = nvram_pf_get_int("vpn_server_", "nat6");
	snprintf(conf->wan6_ifname, sizeof(conf->wan6_ifname), "%s", get_wan6_ifname(wan_primary_ifunit()));
	strlcpy(conf->custom, nvram_pf_safe_get(prefix, "custom"), sizeof(conf->custom));

	if(!_filter_custom_config(conf->custom, sizeof(conf->custom)))
		strlcpy(conf->custom, "", sizeof(conf->custom));
	return conf;
}

ovpn_cconf_t* get_ovpn_cconf(int unit, ovpn_cconf_t* conf)
{
	char *ptr = NULL;
	char prefix[32] = {0}, vpn_clientx_list[32] = {0};

	if(conf == NULL)
		return NULL;
	memset(conf, 0, sizeof(ovpn_cconf_t));
	snprintf(prefix, sizeof(prefix), "vpn_client%d_", unit);
	nvram_get_r("vpn_clientx_eas", vpn_clientx_list, sizeof(vpn_clientx_list));
	ptr = strtok(vpn_clientx_list, ",");
	while(ptr != NULL){
		if(atoi(ptr) == unit)
			conf->enable = 1;
		ptr = strtok(NULL, ",");
	}
	snprintf(conf->progname, sizeof(conf->progname), "vpnclient%d", unit);
	strlcpy(conf->addr, nvram_pf_safe_get(prefix, "addr"), sizeof(conf->addr));
	conf->retry = nvram_pf_get_int(prefix, "retry");
	strlcpy(conf->proto, nvram_pf_safe_get(prefix, "proto"), sizeof(conf->proto));
	conf->port = nvram_pf_get_int(prefix, "port");
	snprintf(conf->if_name,  sizeof(conf->if_name), "%s%d", nvram_pf_safe_get(prefix, "if"), unit + OVPN_CLIENT_BASE);
	conf->if_type = strncmp(conf->if_name, "tun", 3) != 0;
	strlcpy(conf->local, nvram_pf_safe_get(prefix, "local"), sizeof(conf->local));
	strlcpy(conf->remote, nvram_pf_safe_get(prefix, "remote"), sizeof(conf->remote));
	strlcpy(conf->netmask, nvram_pf_safe_get(prefix, "nm"), sizeof(conf->netmask));
	conf->redirect_gateway = nvram_pf_get_int(prefix, "rgw");
	strlcpy(conf->gateway, nvram_pf_safe_get(prefix, "gw"), sizeof(conf->gateway));
	ptr = nvram_pf_safe_get(prefix, "verb");
	if(*ptr)
		conf->verb = nvram_pf_get_int(prefix, "verb");
	else
		conf->verb = nvram_get_int("vpn_loglevel");
	strlcpy(conf->comp, nvram_pf_safe_get(prefix, "comp"), sizeof (conf->comp));
	if(nvram_pf_match(prefix, "crypt", "secret"))
		conf->auth_mode = OVPN_AUTH_STATIC;
	else
		conf->auth_mode = OVPN_AUTH_TLS;
	conf->userauth = nvram_pf_get_int(prefix, "userauth");
	conf->useronly = nvram_pf_get_int(prefix, "useronly");
	strlcpy(conf->username, nvram_pf_safe_get(prefix, "username"), sizeof(conf->username));
	strlcpy(conf->password, nvram_pf_safe_get(prefix, "password"), sizeof(conf->password));
	conf->direction = nvram_pf_get_int(prefix, "hmac");
	strlcpy(conf->cipher, nvram_pf_safe_get(prefix, "cipher"), sizeof(conf->cipher));
	strlcpy(conf->digest, nvram_pf_safe_get(prefix, "digest"), sizeof(conf->digest));
	conf->reneg = nvram_pf_get_int(prefix, "reneg");
	conf->tlscrypt = nvram_pf_get_int(prefix, "tlscrypt");
	conf->verify_x509_type = nvram_pf_get_int(prefix, "tlsremote");
	strlcpy(conf->verify_x509_name, nvram_pf_safe_get(prefix, "cn"), sizeof(conf->verify_x509_name));
	strlcpy(conf->firewall, nvram_pf_safe_get(prefix, "firewall"), sizeof(conf->firewall));
	conf->poll = nvram_pf_get_int(prefix, "poll");
	conf->bridge = nvram_pf_get_int(prefix, "bridge");
	conf->nat = nvram_pf_get_int(prefix, "nat");
	conf->adns = 1;
	strlcpy(conf->custom, nvram_pf_safe_get(prefix, "custom"), sizeof(conf->custom));
	if(!_filter_custom_config(conf->custom, sizeof(conf->custom)))
		strlcpy(conf->custom, "", sizeof(conf->custom));
	return conf;
}

void reset_ovpn_setting(ovpn_type_t type, int unit){
	int len;
	struct nvram_tuple *t = NULL;
	char prefix[32];
	char *service = NULL;

	if (type == OVPN_TYPE_CLIENT) {
		service = "vpn_client_";
		snprintf(prefix, sizeof(prefix), "vpn_client%d_", unit);
		nvram_pf_set(prefix, "username", "");
		nvram_pf_set(prefix, "password", "");
	}
	else {
		service = "vpn_server_";
		snprintf(prefix, sizeof(prefix), "vpn_server%d_", unit);
	}

	logmessage("openvpn", "Resetting VPN %s %d to default settings", service, unit);
	len = strlen(service);
	for (t = router_defaults; t->name; t++) {
		if (len > strlen(t->name) && strncmp(t->name, service, len) == 0 && !strstr(t->name, "unit"))
		{
			_dprintf("reset %s%s=%s\n", prefix, (t->name + len), t->value);
			nvram_pf_set(prefix, (t->name + len), t->value);
		}
	}

	if (type == OVPN_TYPE_CLIENT) {
		set_ovpn_key(type, unit, OVPN_CLIENT_CA, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_CLIENT_CERT, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_CLIENT_EXTRA, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_CLIENT_KEY, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_CLIENT_STATIC, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_CLIENT_CRL, NULL, NULL);
	} else {
		set_ovpn_key(type, unit, OVPN_SERVER_CA, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_CA_KEY, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_CERT, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_EXTRA, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_KEY, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_STATIC, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_DH, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_CRL, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_CLIENT_CERT, NULL, NULL);
		set_ovpn_key(type, unit, OVPN_SERVER_CLIENT_KEY, NULL, NULL);
	}
}

char *_get_ovpn_key_fpath(ovpn_key_t key_type, int unit, char *filename, size_t len)
{
	switch(key_type){
	case OVPN_CLIENT_STATIC:
		snprintf(filename, len, "%s/vpn_crt_client%d_static", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_CLIENT_CA:
		snprintf(filename, len, "%s/vpn_crt_client%d_ca", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_CLIENT_CERT:
		snprintf(filename, len, "%s/vpn_crt_client%d_crt", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_CLIENT_EXTRA:
		snprintf(filename, len, "%s/vpn_crt_client%d_extra", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_CLIENT_KEY:
		snprintf(filename, len, "%s/vpn_crt_client%d_key", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_CLIENT_CRL:
		snprintf(filename, len, "%s/vpn_crt_client%d_crl", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_STATIC:
		snprintf(filename, len, "%s/vpn_crt_server%d_static", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CA:
		snprintf(filename, len, "%s/vpn_crt_server%d_ca", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CA_KEY:
		snprintf(filename, len, "%s/vpn_crt_server%d_ca_key", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CERT:
		snprintf(filename, len, "%s/vpn_crt_server%d_crt", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_EXTRA:
		snprintf(filename, len, "%s/vpn_crt_server%d_extra", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_KEY:
		snprintf(filename, len, "%s/vpn_crt_server%d_key", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_DH:
		snprintf(filename, len, "%s/vpn_crt_server%d_dh", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CRL:
		snprintf(filename, len, "%s/vpn_crt_server%d_crl", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CLIENT_CERT:
		snprintf(filename, len, "%s/vpn_crt_server%d_client_crt", OVPN_DIR_SAVE, unit);
		break;
	case OVPN_SERVER_CLIENT_KEY:
		snprintf(filename, len, "%s/vpn_crt_server%d_client_key", OVPN_DIR_SAVE, unit);
		break;
	default:
      return NULL;
	}
	return filename;
}

int _set_ovpn_key_to_file(const char *buf, char *path)
{
	char *pt = strstr(buf, "-----BEGIN");
	if(pt)
		return f_write_string(path, pt, 0, 0600);
	return -1;
}

char *get_ovpn_key(ovpn_type_t type, int unit, ovpn_key_t key_type, char *buf, size_t len)
{
	FILE *fp = NULL;
	int n, count;
	char *keyStr = NULL;
	char path[128];
	char prefix[32];

	memset(prefix, 0, sizeof(prefix));
	memset(path, 0, sizeof(path));
	if(type == OVPN_TYPE_CLIENT)
		snprintf(prefix, sizeof(prefix), "vpn_crt_client%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "vpn_crt_server%d_", unit);
	switch(key_type){
	case OVPN_CLIENT_STATIC:
	case OVPN_SERVER_STATIC:
		keyStr = "static";
		break;
    case OVPN_CLIENT_CA:
    case OVPN_SERVER_CA:
		keyStr = "ca";
		break;
    case OVPN_CLIENT_CERT:
    case OVPN_SERVER_CERT:
		keyStr = "crt";
		break;
    case OVPN_CLIENT_EXTRA:
    case OVPN_SERVER_EXTRA:
		keyStr = "extra";
		break;
    case OVPN_CLIENT_KEY:
    case OVPN_SERVER_KEY:
		keyStr = "key";
		break;
    case OVPN_CLIENT_CRL:
    case OVPN_SERVER_CRL:
		keyStr = "crl";
		break;
    case OVPN_SERVER_CA_KEY:
		keyStr = "ca_key";
		break;
    case OVPN_SERVER_DH:
		keyStr = "dh";
		break;
    case OVPN_SERVER_CLIENT_CERT:
		keyStr = "client_crt";
		break;
    case OVPN_SERVER_CLIENT_KEY:
		keyStr = "client_key";
		break;
	}
	memset(buf, 0, len);
	snprintf(buf, len, nvram_pf_safe_get(prefix, keyStr));
	if(!d_exists("/jffs/openvpn"))
		mkdir("/jffs/openvpn", 0700);
	if(!_get_ovpn_key_fpath(key_type, unit, path, sizeof(path)))
		return NULL;
	if(*buf){
		if(_set_ovpn_key_to_file(buf, path) >= 0){
			nvram_pf_set(prefix, keyStr, "");
			return buf;
		}
		return NULL;
	}
	fp = fopen(path, "r");
	if(fp == NULL)
		return NULL;
	while(fgets(buf, len, fp) && !strncmp(buf, "-----BEGIN", 10))
        break;
	if(!feof(fp)){
		n = strlen(buf);
        count = fread(&buf[n], 1, len - n - 1, fp);
        if(count > 0){
			buf[n + count] = 0x0;
			fclose(fp);
			return buf;
		}
	}
	memset(buf, 0, len);
	fclose(fp);
	return NULL;
}

int set_ovpn_key(ovpn_type_t type, int unit, ovpn_key_t key_type, char *buf, char *path)
{
	char filename[256];
	memset(filename, 0, sizeof(filename));
	if(!d_exists("/jffs/openvpn"))
		mkdir("/jffs/openvpn", 0700);
	if(!_get_ovpn_key_fpath(key_type, unit, filename, sizeof(filename)))
		return -1;
	if(buf && *buf)
		return _set_ovpn_key_to_file(buf, filename);
	if(!path || !f_exists(path))
		return unlink(filename);
	eval("cp", path, filename);
	return 0;
}

int ovpn_key_exists(ovpn_type_t type, int unit, ovpn_key_t key_type)
{
	char buf[4096];
	memset(buf, 0, sizeof(buf));
	if(get_ovpn_key(type, unit, key_type, buf, sizeof(buf)))
		return buf[0] != 0x0;
	else
		return 0;
}

int need_dnsmasq_serverfile()
{
	return 1;
}

char* get_lan_cidr(char* buf, size_t len)
{
	uint32_t ip, mask;
	struct in_addr in;
	int count = 0;

	ip = (uint32_t)inet_addr(nvram_safe_get("lan_ipaddr"));
	mask = (uint32_t)inet_addr(nvram_safe_get("lan_netmask"));

	in.s_addr = ip & mask;

	mask = ntohl(mask);
	while(mask) {
		mask <<= 1;
		count++;
	}

	snprintf(buf, len, "%s/%d", inet_ntoa(in), count);

	return buf;
}

char* get_lan_cidr6(char* buf, size_t len)
{
	if(get_ipv6_service() == IPV6_PASSTHROUGH)
		snprintf(buf, len, "%s", getifaddr(nvram_safe_get("lan_ifname"), AF_INET6, GIF_PREFIX) ? : "");
	else
		snprintf(buf, len, "%s/%d", nvram_safe_get("ipv6_prefix"), nvram_get_int("ipv6_prefix_length"));
	return buf;
}

char* get_ovpn_sconf_remote(char* buf, size_t len)
{
	const char *address;

	address = get_ddns_hostname();
	if (!nvram_get_int("ddns_enable_x") || !nvram_get_int("ddns_status") || *address == 0x0)
	{
		address = get_wanip();
		if (inet_addr_(address) == INADDR_ANY)
			address = "0.0.0.0"; /* error */
	}

	snprintf(buf, len, address);
	return buf;
}

void update_ovpn_status(ovpn_type_t type, int unit, ovpn_status_t status_type, ovpn_errno_t err_no)
{
	char prefix[32];

	memset(prefix, 0, sizeof(prefix));
	if(type)
		snprintf(prefix, sizeof(prefix), "vpn_client%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "vpn_server%d_", unit);
	nvram_pf_set_int(prefix, "state", status_type);
	nvram_pf_set_int(prefix, "errno", err_no);
}

ovpn_status_t get_ovpn_status(ovpn_type_t type, int unit)
{
	char prefix[32];
	memset(prefix, 0, sizeof(prefix));
	if(type)
		snprintf(prefix, sizeof(prefix), "vpn_client%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "vpn_server%d_", unit);
	return nvram_pf_get_int(prefix, "state");
}

ovpn_errno_t get_ovpn_errno(ovpn_type_t type, int unit)
{
	char prefix[32];
	memset(prefix, 0, sizeof(prefix));
	if(type)
		snprintf(prefix, sizeof(prefix), "vpn_client%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "vpn_server%d_", unit);
	return nvram_pf_get_int(prefix, "errno");
}

int wait_time_sync(int max)
{
	while(max > 0){
		if(nvram_match("ntp_ready", "1"))
			return 0;
		sleep(1);
		max--;
	}
	return -1;
}

unsigned int adjust_smp_affinity(ovpn_type_t type, int unit)
{
	int model = get_model();

	switch(model)
	{
		case MODEL_RTAC86U:
			return unit ? 2 : 1;
		case MODEL_DSLAX82U:
			return 2;
		case MODEL_GTAC5300:
			return unit ? 2 : 1;
		default:
			return 0;
	}
	return 0;
}

ovpn_accnt_info_t* get_ovpn_accnt(ovpn_accnt_info_t *accnt_info)
{
	char *ptr = NULL, *password = NULL, *username = NULL, *buf = NULL;
 	char passwd[256];

	if(accnt_info){
		memset(accnt_info, 0, sizeof(ovpn_accnt_info_t));
		accnt_info->count = 0;
		buf = strdup(nvram_safe_get("vpn_serverx_clientlist"));
		while((ptr = strsep(&buf, "<")) != NULL){
			if(vstrsep(ptr, ">", &username, &password) == 2){
				if(*username && *password){
					memset(passwd, 0, sizeof(passwd));
					pw_dec(password, passwd, sizeof(passwd), 1);
					password = passwd;
					snprintf(accnt_info->account[accnt_info->count].username, sizeof(accnt_info->account[0].username), "%s", username);
					snprintf(accnt_info->account[accnt_info->count].password, sizeof(accnt_info->account[0].password), "%s", password);
					++accnt_info->count;
					if(accnt_info->count >= OVPN_ACCNT_MAX)
						break;
				}
			}
		}
		free(buf);
	}
	return accnt_info;
}

void ovpn_defaults()
{
	char buf[64];
	char tmp[32];
	char prefix[16];
	unsigned char rand_bytes[5];
	memset(tmp, 0, sizeof(tmp));
	memset(buf, 0, sizeof(buf));
	f_read("/dev/urandom", &rand_bytes, sizeof(rand_bytes));
	snprintf(prefix, sizeof(prefix), "vpn_server%d_", 1);
	if(!nvram_pf_get(prefix, "sn6") || !nvram_pf_get(prefix, "local6") || !nvram_pf_get(prefix, "remote6")){
		snprintf(tmp, sizeof(tmp), "fd%02x:%02x%02x:%02x%02x:%04x::", rand_bytes[0], rand_bytes[1], rand_bytes[2], rand_bytes[3], 0, 4369);
		snprintf(buf, sizeof(buf), "%s/64", tmp);
		nvram_pf_set(prefix, "sn6", buf);
		snprintf(buf, sizeof(buf), "%s1", tmp);
		nvram_pf_set(prefix, "local6", buf);
		snprintf(buf, sizeof(buf), "%s2", tmp);
		nvram_pf_set(prefix, "remote6", buf);
	}
}

char *get_lan_subnet(char *buf, size_t len)
{
	uint32_t ip, mask;
	struct in_addr in;

	ip = (uint32_t)inet_addr(nvram_safe_get("lan_ipaddr"));
	mask = (uint32_t)inet_addr(nvram_safe_get("lan_netmask"));

	in.s_addr = ip & mask;
	snprintf(buf, len, "%s", inet_ntoa(in));
	return buf;
}

#ifdef RTCONFIG_MULTILAN_CFG
char* ovpn_get_sdn_subnet_mask(ovpn_type_t type, int unit, char *buf, size_t len)
{
	int i;
	size_t mtl_sz;
	char *ret = NULL;
	MTLAN_T *pmtl = (MTLAN_T *)INIT_MTLAN(sizeof(MTLAN_T));
	if(get_ovpn_mtlan(type, unit, pmtl, &mtl_sz)){
		memset(buf, 0, len);
		for(i = 0; i < mtl_sz; i++){
			strlcat(buf, pmtl[i].nw_t.subnet, len);
			strlcat(buf, " ", len);
			strlcat(buf, pmtl[i].nw_t.netmask, len);
			strlcat(buf, ",", len);
		}
		FREE_MTLAN(pmtl);
		ret = buf;
		_dprintf("%s: %s\n", __func__, buf);
	}
	FREE_MTLAN(pmtl);
	return ret;
}
#endif
