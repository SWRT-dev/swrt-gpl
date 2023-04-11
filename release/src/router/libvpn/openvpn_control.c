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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <shutils.h>
#include <shared.h>
#include "openvpn_config.h"
#include "openvpn_control.h"
#include "openvpn_utils.h"

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 10000

#define PUSH_LAN_METRIC 500

#define OVPN_DIR_CONF	"/etc/openvpn"
#define OVPN_DIR_TMP	"/tmp/openvpn"
#define OVPN_DEFAULT_DH	"/rom/dh2048.pem"
#define OVPN_PRG_OPENVPN	"/usr/sbin/openvpn"
#define OVPN_PRG_OPENSSL	"/usr/sbin/openssl"
#define OVPN_PRG_OPENSSL_CONF	"/rom/easy-rsa/openssl-1.0.0.cnf"
#define OVPN_PRG_PKITOOL	"/rom/easy-rsa/pkitool"
#define OVPN_PRG_CROND_CRU	"/usr/sbin/cru"
#define OVPN_LIB_PAM	"/usr/lib/openvpn-plugin-auth-pam.so"
#define OVPN_RETRY_MIN	1

static int _ovpn_is_running(ovpn_type_t type, int unit)
{
	char name[16];

	if(type == OVPN_TYPE_CLIENT)
		snprintf(name, sizeof(name), "vpnclient%d", unit);
	else if(type == OVPN_TYPE_SERVER)
		snprintf(name, sizeof(name), "vpnserver%d", unit);

	if(pidof(name) > 0)
		return 1;
	return 0;
}

static void _ovpn_check_dir(ovpn_type_t type, int unit)
{
	char path[128];

	if(!d_exists(OVPN_DIR_CONF)) {
		mkdir(OVPN_DIR_CONF, 0700);
	}

	if(type == OVPN_TYPE_CLIENT)
		snprintf(path, sizeof(path), "%s/client%d", OVPN_DIR_CONF, unit);
	else if(type == OVPN_TYPE_SERVER)
		snprintf(path, sizeof(path), "%s/server%d", OVPN_DIR_CONF, unit);
	if(!d_exists(path)){
		mkdir(path, 0700);
	}

	// Make sure symbolic link exists
	if(type == OVPN_TYPE_CLIENT)
		snprintf(path, sizeof(path), "%s/vpnclient%d", OVPN_DIR_CONF, unit);
	else if(type == OVPN_TYPE_SERVER)
		snprintf(path, sizeof(path), "%s/vpnserver%d", OVPN_DIR_CONF, unit);
	unlink(path);
	symlink(OVPN_PRG_OPENVPN, path);
}

static void _ovpn_tunnel_create(ovpn_if_t if_type, char* if_name)
{
	char path[128];

	eval(OVPN_PRG_OPENVPN, "--mktun", "--dev", if_name);

	snprintf(path, sizeof(path), "/sys/class/net/%s/ifindex", if_name);
	f_wait_exists(path, 3);
	snprintf(path, sizeof(path), "/proc/sys/net/ipv6/conf/%s/disable_ipv6", if_name);
	f_write_string(path, "0", 0, 0);
	if(if_type == OVPN_IF_TAP)
		eval("brctl", "addif", "br0", if_name);

	eval("ifconfig", if_name, "up", "promisc");
}

static void _ovpn_tunnel_remove(ovpn_type_t type, int unit)
{
	char buf[128];
	char if_name[8];

	if(type == OVPN_TYPE_CLIENT)
		snprintf(if_name, sizeof(if_name), "tun%d", OVPN_CLIENT_BASE + unit);
	else
		snprintf(if_name, sizeof(if_name), "tun%d", OVPN_SERVER_BASE + unit);

	snprintf(buf, sizeof(buf), "/sys/class/net/%s", if_name);
	if(d_exists(buf)) {
		eval("ifconfig", if_name, "down");
		eval(OVPN_PRG_OPENVPN, "--rmtun", "--dev", if_name);
	}

	sleep(1);

	if(type == OVPN_TYPE_CLIENT)
		snprintf(if_name, sizeof(if_name), "tap%d", OVPN_CLIENT_BASE + unit);
	else
		snprintf(if_name, sizeof(if_name), "tap%d", OVPN_SERVER_BASE + unit);

	snprintf(buf, sizeof(buf), "/sys/class/net/%s", if_name);
	if(d_exists(buf)) {
		eval("ifconfig", if_name, "down");
		eval("brctl", "delif", "br0", if_name);
		eval(OVPN_PRG_OPENVPN, "--rmtun", "--dev", if_name);
	}
}

static int _ovpn_client_write_keys(int unit, ovpn_cconf_t *conf)
{
	FILE *fp;
	char fpath[128];
	char buf[BUF_SIZE];

	//TLS
	if(conf->auth_mode == OVPN_AUTH_TLS) {
		if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_CA, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/client%d/ca.crt", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -1;
		}
		else{
			logmessage_normal(conf->progname, "Get CA failed");
			return -1;
		}

		if (!conf->useronly) {
			if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_KEY, buf, sizeof(buf))) {
				snprintf(fpath, sizeof(fpath), "%s/client%d/client.key", OVPN_DIR_CONF, unit);
				fp = fopen(fpath, "w");
				if(fp) {
					fputs(buf, fp);
					fclose(fp);
					chmod(fpath, S_IRUSR|S_IWUSR);
				}
				else
					return -2;
			}
			else{
				logmessage_normal(conf->progname, "Get Key failed");
				return -2;
			}

			if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_CERT, buf, sizeof(buf))) {
				snprintf(fpath, sizeof(fpath), "%s/client%d/client.crt", OVPN_DIR_CONF, unit);
				fp = fopen(fpath, "w");
				if(fp) {
					fputs(buf, fp);
					fclose(fp);
					chmod(fpath, S_IRUSR|S_IWUSR);
				}
				else
					return -3;
			}
			else{
				logmessage_normal(conf->progname, "Get Certificate failed");
				return -3;
			}
			if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_EXTRA, buf, sizeof(buf))) {
				snprintf(fpath, sizeof(fpath), "%s/client%d/client_extra.crt", OVPN_DIR_CONF, unit);
				fp = fopen(fpath, "w");
				if(fp) {
					fputs(buf, fp);
					fclose(fp);
					chmod(fpath, S_IRUSR|S_IWUSR);
				}
				else
					return -7;
			}
		}

		if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_CRL, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/client%d/crl.pem", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -4;
		}

		if (conf->userauth) {
			snprintf(fpath, sizeof(fpath), "%s/client%d/up", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fprintf(fp, "%s\n%s\n", conf->username, conf->password);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -5;
		}
	}

	//static key
	if ( conf->auth_mode == OVPN_AUTH_STATIC
		|| (conf->auth_mode == OVPN_AUTH_TLS && (conf->direction >= 0 || conf->tlscrypt > 0))
	) {
		if(get_ovpn_key(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_STATIC, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/client%d/static.key", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -6;
		}
		else{
			logmessage_normal(conf->progname, "Get static key failed");
			return -6;
		}
	}

	return 0;
}

static int _ovpn_server_write_keys(int unit, ovpn_sconf_t *conf)
{
	FILE *fp;
	char fpath[128];
	char buf[BUF_SIZE];

	//TLS
	if(conf->auth_mode == OVPN_AUTH_TLS) {
		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA_KEY, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/ca.key", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -1;
		}
		else{
			logmessage_normal(conf->progname, "Get CA Key failed");
			return -1;
		}

		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/ca.crt", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -2;
		}
		else{
			logmessage_normal(conf->progname, "Get CA failed");
			return -2;
		}

		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_KEY, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/server.key", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -3;
		}
		else{
			logmessage_normal(conf->progname, "Get Key failed");
			return -3;
		}

		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CERT, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/server.crt", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -4;
		}
		else{
			logmessage_normal(conf->progname, "Get Certificate failed");
			return -4;
		}

		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CRL, buf, sizeof(buf))) {
			if(strlen(buf)) {
				snprintf(fpath, sizeof(fpath), "%s/server%d/crl.pem", OVPN_DIR_CONF, unit);
				fp = fopen(fpath, "w");
				if(fp) {
					fputs(buf, fp);
					fclose(fp);
					chmod(fpath, S_IRUSR|S_IWUSR);
				}
				else
					return -5;
			}
		}

		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_DH, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/dh.pem", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -6;
		}
		else{
			logmessage_normal(conf->progname, "Get DH parameters failed");
			return -6;
		}
	}

	//static key
	if ( conf->auth_mode == OVPN_AUTH_STATIC
		|| (conf->auth_mode == OVPN_AUTH_TLS && conf->direction >= 0)
	) {
		if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_STATIC, buf, sizeof(buf))) {
			snprintf(fpath, sizeof(fpath), "%s/server%d/static.key", OVPN_DIR_CONF, unit);
			fp = fopen(fpath, "w");
			if(fp) {
				fputs(buf, fp);
				fclose(fp);
				chmod(fpath, S_IRUSR|S_IWUSR);
			}
			else
				return -7;
		}
		else{
			logmessage_normal(conf->progname, "Get static key failed");
			return -7;
		}
	}

	return 0;
}

/***********************************************************
 * return value:
 * -1: error
 * 0: selfsigned and key size mismatch
 * 1: not selfsigned
 * 2: selfsigned and key size match
 ***********************************************************/
static int _ovpn_server_verify_selfsigned_ca(char* buf, int match_keysize)
{
	BIO *bio = NULL;
	X509 *x509data = NULL;
	char issuer[128] = {0};
	EVP_PKEY *public_key = NULL;
	int real_keysize = 0 ;
	int ret = -1;

	if(!buf)
		return -1;

	bio = BIO_new(BIO_s_mem());

	if(BIO_write(bio, buf, strlen(buf)) <= 0)
		goto finish;

	x509data = PEM_read_bio_X509(bio, NULL, 0, NULL);

	X509_NAME_oneline(X509_get_issuer_name(x509data), issuer, sizeof(issuer));
	//cprintf("issuer: %s\n", issuer);
	if(!strstr(issuer, "ASUS") && !strstr(issuer, get_productid())) {
		ret = 1;
		goto finish;
	}

	public_key = X509_get_pubkey(x509data);
	real_keysize = EVP_PKEY_bits(public_key);
	//cprintf("key size: %d\n", real_keysize);
	if(real_keysize == match_keysize)
		ret = 2;
	else
		ret = 0;

finish:
	BIO_free(bio);
	return ret;
}

/***********************************************************
 * return value:
 * -1: error
 * 0: dh size < 768
 * 1: dh size >= 768
 ***********************************************************/
static int _ovpn_server_verify_dh(char* buf)
{
	BIO *bio = NULL;
	DH *dh = NULL;
	int dh_size;
	int ret = -1;

	if(!buf)
		return -1;

	bio = BIO_new(BIO_s_mem());

	if(BIO_write(bio, buf, strlen(buf)) <= 0)
		goto finish;

	dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
#if (OPENSSL_VERSION_NUMBER >= 0x1010000fL)
	dh_size = DH_size(dh);
#else
	dh_size = BN_num_bits(dh->p);
#endif

	//cprintf("dh size: %d\n", dh_size);
	if(dh_size < 768)
		ret = 0;
	else
		ret = 1;

finish:
	BIO_free(bio);
	return ret;
}

static void _ovpn_server_check_keys(int unit, ovpn_sconf_t *conf)
{
	char fpath[128];
	FILE *fp;
	char buf[BUF_SIZE];
	int dh_valid;

	mkdir(OVPN_DIR_TMP, 0700);

	//certificate and key
	if( !get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA, buf, sizeof(buf))
		|| !_ovpn_server_verify_selfsigned_ca(buf, conf->tls_keysize)
	) {
		//generate certificate and key
		snprintf(fpath, sizeof(fpath), "/tmp/genvpncert.sh");
		fp = fopen(fpath, "w");
		if(fp){
			fprintf(fp,
				"#!/bin/sh\n"
				"export OPENSSL=\"%s\"\n"	//openssl
				"export GREP=\"/bin/grep\"\n"
				"export KEY_CONFIG=\"%s\"\n"	//openssl conf
				"export KEY_DIR=\"%s\"\n"	//OVPN_DIR_TMP
				"export KEY_SIZE=%d\n"	//tls_keysize
				"export CA_EXPIRE=3650\n"
				"export KEY_EXPIRE=3650\n"
				"export KEY_COUNTRY=\"TW\"\n"
				"export KEY_PROVINCE=\"TW\"\n"
				"export KEY_CITY=\"Taipei\"\n"
				"export KEY_ORG=\"ASUS\"\n"
				"export KEY_EMAIL=\"me@myhost.mydomain\"\n"
				"export KEY_CN=\"%s\"\n"	//productid
				"touch %s/index.txt\n"	//OVPN_DIR_TMP
				"echo 01 >%s/serial\n"	//OVPN_DIR_TMP
				"%s --initca\n"	//pkitool
				"%s --server server\n"	//pkitool
				, OVPN_PRG_OPENSSL, OVPN_PRG_OPENSSL_CONF
				, OVPN_DIR_TMP, conf->tls_keysize, get_productid()
				, OVPN_DIR_TMP, OVPN_DIR_TMP
				, OVPN_PRG_PKITOOL, OVPN_PRG_PKITOOL
			);

			//undefined common name, default use username-as-common-name
			fprintf(fp, "\nexport KEY_CN=\"\"\n%s client\n", OVPN_PRG_PKITOOL);

			fclose(fp);
			chmod(fpath, 0700);
			system(fpath);
			unlink(fpath);
			sync_profile_update_time(1);
		}

		//set certification and key to nvram or jffs
		snprintf(fpath, sizeof(fpath), "%s/ca.key", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA_KEY, NULL, fpath);
		snprintf(fpath, sizeof(fpath), "%s/ca.crt", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA, NULL, fpath);

		snprintf(fpath, sizeof(fpath), "%s/server.key", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_KEY, NULL, fpath);
		snprintf(fpath, sizeof(fpath), "%s/server.crt", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CERT, NULL, fpath);

		snprintf(fpath, sizeof(fpath), "%s/client.key", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CLIENT_KEY, NULL, fpath);
		snprintf(fpath, sizeof(fpath), "%s/client.crt", OVPN_DIR_TMP);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CLIENT_CERT, NULL, fpath);
	}

	//DH
	dh_valid = 0;
	if(get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_DH, buf, sizeof(buf))){
		dh_valid = _ovpn_server_verify_dh(buf);
	}
	if(dh_valid == 0)
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_DH, NULL, OVPN_DEFAULT_DH);
	else if(dh_valid == -1)
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_ERROR, OVPN_ERRNO_DH);

	//STATIC KEY
	if((conf->auth_mode == OVPN_AUTH_STATIC || (conf->auth_mode == OVPN_AUTH_TLS && conf->direction >= 0))
		&& !ovpn_key_exists(OVPN_TYPE_SERVER, unit, OVPN_SERVER_STATIC)){
		snprintf(fpath, sizeof(fpath), "%s/static.key", OVPN_DIR_TMP);
		snprintf(buf, sizeof(buf), "%s --genkey --secret %s", OVPN_PRG_OPENVPN, fpath);
		system(buf);
		f_wait_exists(fpath, 3);
		set_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_STATIC, NULL, fpath);
	}

	eval("rm", "-rf", OVPN_DIR_TMP);
}

static int _ovpn_server_verify_client_crt(int unit)
{
	char buf[BUF_SIZE];
	char fpath[128];
	FILE *fp;
	int valid = 0;
	BIO *crtbio = NULL;
	X509 *x509data = NULL;
	X509_STORE *store = NULL;
	X509_STORE_CTX *ctx = NULL;
	int ret = 0;

	OpenSSL_add_all_algorithms();

	//Load the certificate
	get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CLIENT_CERT, buf, sizeof(buf));
	crtbio = BIO_new(BIO_s_mem());
	ret = BIO_write(crtbio, buf, strlen(buf));
	if(ret <= 0) {
		goto end;
	}
	x509data = PEM_read_bio_X509(crtbio, NULL, 0, NULL);
	if(x509data == NULL) {
		goto end;
	}

	//Load the CA
	mkdir(OVPN_DIR_TMP, 0700);
	snprintf(fpath, sizeof(fpath), "%s/ca.crt", OVPN_DIR_TMP);
	fp = fopen(fpath, "w");
	if(fp) {
		get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA, buf, sizeof(buf));
		fputs(buf, fp);
		fclose(fp);
	}
	else {
		goto end;
	}
	store = X509_STORE_new();
	if(store == NULL) {
		goto end;
	}
	ret = X509_STORE_load_locations(store, fpath, NULL);
	if (ret != 1) {
		goto end;
	}

	//Check the certficate
	ctx = X509_STORE_CTX_new();
	if(ctx == NULL) {
		goto end;
	}
	if(!X509_STORE_CTX_init(ctx, store, x509data, NULL)) {
		goto end;
	}
	ret = X509_verify_cert(ctx);
	if(ret > 0) {
		valid = 1;
	}
#if (OPENSSL_VERSION_NUMBER >= 0x1010000fL)
	else if(X509_STORE_CTX_get_error(ctx) == X509_V_ERR_CERT_NOT_YET_VALID) {
#else
	else if(ctx->error == X509_V_ERR_CERT_NOT_YET_VALID) {
#endif
		valid = 2;
	}
	else {
#if (OPENSSL_VERSION_NUMBER >= 0x1010000fL)
		_dprintf("error: %s\n", X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx)));
#else
		_dprintf("error: %s\n", X509_verify_cert_error_string(ctx->error));
#endif
	}

end:
	if(crtbio)
		BIO_free(crtbio);
	if(store)
		X509_STORE_free(store);
	if(ctx)
		X509_STORE_CTX_free(ctx);

	eval("rm", "-rf", OVPN_DIR_TMP);

	return valid;
}

/* e.g.
 * iptables -t nat -I PREROUTING -p tcp --dport 1194 -j ACCEPT
 * iptables -I INPUT -p tcp --dport 1194 -j ACCEPT
 * iptables -I INPUT -i tun21 -j ACCEPT
 * iptables -I FORWARD -i tun21 -j ACCEPT
 */
static void _ovpn_server_fw_rule_add(int unit, ovpn_sconf_t *conf)
{
	FILE* fp;
	char fpath[128];

	snprintf(fpath, sizeof(fpath), "/etc/openvpn/server%d/fw.sh", unit);
	fp = fopen(fpath, "w");
	if(fp){
		fprintf(fp, "#!/bin/sh\n\n");
		fprintf(fp, "iptables -t nat -I PREROUTING -p %s --dport %d -j ACCEPT\n"
			, strstr(conf->proto, "udp") ? "udp" : "tcp", conf->port);
		fprintf(fp, "iptables -I OVPNSI -p %s --dport %d -j ACCEPT\n"
			, strstr(conf->proto, "udp") ? "udp" : "tcp", conf->port);
		fprintf(fp, "ip6tables -I OVPNSI -p %s --dport %d -j ACCEPT\n"
			, strstr(conf->proto, "udp") ? "udp" : "tcp", conf->port
		);
		fprintf(fp, "iptables -I OVPNSI -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNSI -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "iptables -I OVPNSF -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNSF -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "iptables -I OVPNSF -o %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNSF -o %s -j ACCEPT\n", conf->if_name);
		if(conf->if_type == OVPN_IF_TUN && conf->ipv6_enable && conf->nat6)
			fprintf(fp, "ip6tables -t nat -I POSTROUTING -s %s -o %s -j MASQUERADE\n", conf->network6, conf->wan6_ifname);
		fclose(fp);
		chmod(fpath, S_IRUSR|S_IWUSR|S_IXUSR);
		eval(fpath);
	}
	if(conf->ipv6_enable && conf->if_type == OVPN_IF_TUN && conf->nat6){
		snprintf(fpath, sizeof(fpath), "/etc/openvpn/server%d/fw_nat6.sh", unit);
		fp = fopen(fpath, "w");
		if(fp){
			fprintf(fp, "#!/bin/sh\n\n");
			fprintf(fp, "ip6tables -t nat -I POSTROUTING -s %s -o %s -j MASQUERADE\n", conf->network6, conf->wan6_ifname);
			fclose(fp);
			chmod(fpath, S_IRUSR|S_IWUSR|S_IXUSR);
			eval(fpath);
		}
	}
}

static void _ovpn_server_fw_rule_del(int unit)
{
	char path[128];

	snprintf(path, sizeof(path), "/etc/openvpn/server%d/fw.sh", unit);
	if(f_exists(path)) {
		eval("sed", "-i", "s/-I/-D/", path);
		eval("sed", "-i", "s/-A/-D/", path);
		eval(path);
		unlink(path);
	}
	snprintf(path, sizeof(path), "/etc/openvpn/server%d/fw_nat6.sh", unit);
	if(f_exists(path)) {
		eval("sed", "-i", "s/-I/-D/", path);
		eval("sed", "-i", "s/-A/-D/", path);
		eval(path);
		unlink(path);
	}
}

static void _ovpn_client_fw_rule_add(int unit, ovpn_cconf_t *conf)
{
	FILE* fp;
	char fpath[128];

	snprintf(fpath, sizeof(fpath), "/etc/openvpn/client%d/fw.sh", unit);
	fp = fopen(fpath, "w");
	if(fp){
		fprintf(fp, "#!/bin/sh\n\n");
		fprintf(fp, "iptables -I OVPNCI -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "iptables -I OVPNCF -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "iptables -I OVPNCF -o %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNCI -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNCF -i %s -j ACCEPT\n", conf->if_name);
		fprintf(fp, "ip6tables -I OVPNCF -o %s -j ACCEPT\n", conf->if_name);
		fclose(fp);
		chmod(fpath, S_IRUSR|S_IWUSR|S_IXUSR);
		eval(fpath);
	}
}

static void _ovpn_client_fw_rule_del(int unit)
{
	char fpath[128];

	snprintf(fpath, sizeof(fpath), "/etc/openvpn/client%d/fw.sh", unit);
	if(f_exists(fpath)) {
		eval("sed", "-i", "s/-I/-D/", fpath);
		eval(fpath);
		unlink(fpath);
	}
}

static void _ovpn_cron_job_add(ovpn_type_t type, int unit, void *conf)
{
	char buf[256];
	int min = OVPN_RETRY_MIN;
	ovpn_sconf_t *sconf;
	ovpn_cconf_t *cconf;

	if(type == OVPN_TYPE_SERVER){
		if(conf){
			sconf = (ovpn_sconf_t *)conf;
			min = sconf->poll;
		}
		snprintf(buf, sizeof(buf),
			"%s a start_vpnserver%d \"*/%d * * * * service start_vpnserver%d\""
			, OVPN_PRG_CROND_CRU, unit, min, unit);
	}else{
		if(conf){
			cconf = (ovpn_cconf_t *)conf;
			min = cconf->poll;
		}
		snprintf(buf, sizeof(buf),
			"%s a start_vpnclient%d \"*/%d * * * * service start_vpnclient%d\""
			, OVPN_PRG_CROND_CRU, unit, min, unit);
	}

	if(min > 0)
		system(buf);
}

static void _ovpn_cron_job_del(ovpn_type_t type, int unit)
{
	char buf[256];

	if(type == OVPN_TYPE_SERVER) {
		snprintf(buf, sizeof(buf), "%s d start_vpnserver%d", OVPN_PRG_CROND_CRU, unit);
	}
	else {
		snprintf(buf, sizeof(buf), "%s d start_vpnclient%d", OVPN_PRG_CROND_CRU, unit);
	}

	system(buf);
}

static int _ovpn_server_gen_conf(int unit, ovpn_sconf_t *conf)
{
	char buf[BUF_SIZE];
	FILE *fp, *fp_client, *fp_cc;
	int i;
	int ret;

	snprintf(buf, sizeof(buf), "%s/server%d/config.ovpn", OVPN_DIR_CONF, unit);
	fp = fopen(buf, "w");
	snprintf(buf, sizeof(buf), "%s/server%d/client.ovpn", OVPN_DIR_CONF, unit);
	fp_client = fopen(buf, "w");

	if(!fp)
		return -1;
	if(!fp_client){
		fclose(fp);
		return -1;
	}

// Tunnel options
	fprintf(fp, "# Automatically generated configuration\n\n# Tunnel options\n");

	//remote (client)
	fprintf(fp_client, "remote %s %d\n", get_ovpn_sconf_remote(buf, sizeof(buf)), conf->port);

	//Allow remote peer to change its IP address and/or port number (client)
	fprintf(fp_client, "float\n");

	//Do not bind to local address and port (client)
	fprintf(fp_client, "nobind\n");

	//protocol
	if(strstr(conf->proto, "udp")){
		fprintf(fp, "proto udp6\n");
		fprintf(fp, "multihome\n");
		fprintf(fp_client, "proto udp\n");// udp:v4 first, udp6:v6 first
	}else{
		fprintf(fp, "proto tcp6-server\n");
		fprintf(fp_client, "proto tcp-client\n");// tcp-client:v4 first, tcp6-server:v6 first
	}

	//port
	fprintf(fp, "port %d\n", conf->port);

	//dev
	fprintf(fp, "dev %s\n", conf->if_name);
	if(conf->if_type == OVPN_IF_TUN){
		fprintf(fp_client, "dev tun\n");
	}else if(conf->if_type == OVPN_IF_TAP){
		fprintf(fp_client,
			"dev tap\n\n"
			"# Windows needs the TAP-Win32 adapter name\n"
			"# from the Network Connections panel\n"
			"# if you have more than one.  On XP SP2,\n"
			"# you may need to disable the firewall\n"
			"# for the TAP adapter.\n"
			";dev-node MyTap\n\n"
		);
	}

	//ip
	if(conf->auth_mode == OVPN_AUTH_STATIC){
		if(conf->if_type == OVPN_IF_TUN){
			fprintf(fp, "ifconfig %s %s\n", conf->local, conf->remote);
			fprintf(fp_client, "ifconfig %s %s\n", conf->remote, conf->local);
			if(conf->ipv6_enable){
				fprintf(fp, "ifconfig-ipv6 %s %s\n", conf->local6, conf->remote6);
				fprintf(fp_client, "ifconfig-ipv6 %s %s\n", conf->remote6, conf->local6);
			}
		}
	}
	if(conf->if_type == OVPN_IF_TUN && conf->ipv6_enable){
		fprintf(fp, "tun-mtu-extra 20\n");
		fprintf(fp_client, "tun-mtu-extra 20\n");
	}

	//OS socket buffer
	fprintf(fp, "sndbuf 0\n");
	fprintf(fp_client, "sndbuf 0\n");
	fprintf(fp, "rcvbuf 0\n");
	fprintf(fp_client, "rcvbuf 0\n");

	//Ping remote
	//keep alive
	fprintf(fp, "keepalive 10 30\n");
	fprintf(fp_client, "keepalive 10 30\n");

	snprintf(buf, sizeof(buf), "%s/ovpn-up", OVPN_DIR_CONF);
	symlink("/sbin/rc", buf);
	fprintf(fp, "up '%s'\n", buf);
	snprintf(buf, sizeof(buf), "%s/ovpn-down", OVPN_DIR_CONF);
	symlink("/sbin/rc", buf);
	fprintf(fp, "down '%s'\n", buf);
	fprintf(fp, "setenv ovpn_type %d\n", OVPN_TYPE_SERVER);
	fprintf(fp, "setenv unit %d\n", unit);
	//progname
	fprintf(fp, "daemon %s\n", conf->progname);
	fprintf(fp, "script-security 2\n");
	//log verbosity
	if(conf->verb >= 0)
		fprintf(fp, "verb %d\n", conf->verb);

	//client status
	fprintf(fp, "status-version 2\n");
	fprintf(fp, "status status 10\n");

	//compression
	if(strlen(conf->comp) && strcmp(conf->comp, "-1")){
		if(!strcmp(conf->comp, "adaptive")){
			fprintf(fp, "comp-lzo adaptive\n");
			fprintf(fp_client, "comp-lzo adaptive\n");
		}else if(!strcmp(conf->comp, "no")){
			fprintf(fp, "compress\n");
			fprintf(fp_client, "\n# for OpenVPN 2.4 or older\ncomp-lzo no\n# for OpenVPN 2.4 or newer\n;compress\n\n");
		}else if(!strcmp(conf->comp, "yes")){
			fprintf(fp, "compress lzo\n");
			fprintf(fp_client, "\n# for OpenVPN 2.4 or older\ncomp-lzo yes\n# for OpenVPN 2.4 or newer\n;compress\n\n");
		}else if(!strcmp(conf->comp, "lz4")){
			fprintf(fp, "compress lz4\n");
			fprintf(fp_client, "compress lz4\n");
		}
	}

	//authentication
	if(conf->auth_mode == OVPN_AUTH_TLS){
		fprintf(fp, "plugin %s openvpn\n", OVPN_LIB_PAM);
		fprintf(fp_client, "auth-user-pass\n");
	}

//Server Mode
	fprintf(fp, "\n# Server Mode\n");
	if(conf->auth_mode == OVPN_AUTH_TLS){
		if(conf->if_type == OVPN_IF_TUN){
			fprintf(fp, "server %s ", conf->network);
			fprintf(fp, "%s\n", conf->netmask);
			if(conf->ipv6_enable)
				fprintf(fp, "server-ipv6 %s\n", conf->network6);
		}else if(conf->if_type == OVPN_IF_TAP){
			fprintf(fp, "server-bridge");
			if(conf->dhcp){
				//--server-bridge
				//DHCP-proxy mode
				//will push "route-gateway dhcp"
				fprintf(fp, " nogw\n");
				//fprintf(fp, "push \"route 0.0.0.0 255.255.255.255 net_gateway\"");
			}
			else {
				//--server-bridge gateway netmask pool-start-IP pool-end-IP
				fprintf(fp, " %s %s %s %s\n"
					, conf->lan_ipaddr, conf->lan_netmask, conf->pool_start, conf->pool_end);
			}
		}

		fprintf(fp_client, "client\n");
	}

	//route, push message
	if(conf->auth_mode == OVPN_AUTH_TLS){
		//client config dir
		if(conf->ccd){
			fprintf(fp, "client-config-dir ccd\n");

			if(conf->c2c)
				fprintf(fp, "client-to-client\n");

			if(conf->ccd_excl)
				fprintf(fp, "ccd-exclusive\n");
			else
				fprintf(fp, "duplicate-cn\n");

			snprintf(buf, sizeof(buf), "%s/server%d/ccd", OVPN_DIR_CONF, unit);
			mkdir(buf, 0700);
			chdir(buf);

			for(i=0; i<conf->ccd_info.count; i++){
				if(conf->ccd_info.ccd_val[i].enable){
					fp_cc = fopen(conf->ccd_info.ccd_val[i].name, "w");
					if(fp_cc) {
						fprintf(fp_cc, "iroute %s %s\n"
							, conf->ccd_info.ccd_val[i].network, conf->ccd_info.ccd_val[i].netmask);
						fprintf(fp, "route %s %s\n"
							, conf->ccd_info.ccd_val[i].network, conf->ccd_info.ccd_val[i].netmask);

						if(conf->c2c && conf->ccd_info.ccd_val[i].push){
							fprintf(fp, "push \"route %s %s\"\n"
								, conf->ccd_info.ccd_val[i].network, conf->ccd_info.ccd_val[i].netmask);
						}

						fclose(fp_cc);
						chmod(conf->ccd_info.ccd_val[i].name, S_IRUSR|S_IWUSR);
					}
				}
			}
		}
		else {
			fprintf(fp, "duplicate-cn\n");
		}

		//router LAN
		if(conf->push_lan && conf->if_type == OVPN_IF_TUN){
			get_lan_cidr(buf, sizeof(buf));
			*strchr(buf, '/') = 0;
			fprintf(fp, "push \"route %s %s vpn_gateway %d\"\n",
				buf, conf->lan_netmask, PUSH_LAN_METRIC);
			if(conf->ipv6_enable){
				get_lan_cidr6(buf, sizeof(buf));
				if(strlen(buf) > 1)
					fprintf(fp, "push \"route-ipv6 %s\"\n", buf);
			}
		}

		//router as gateway
		if(conf->redirect_gateway){
			if(!conf->ipv6_enable || conf->if_type == OVPN_IF_TAP)
				fprintf(fp, "push \"redirect-gateway def1\"\n");
			else
				fprintf(fp, "push \"redirect-gateway ipv6 def1\"\n");
			//if ( conf->if_type == OVPN_IF_TAP )
				//fprintf(fp, "push \"route-gateway %s\"\n", conf->lan_ipaddr);
		}

		//DNS
		if(conf->push_dns){
			fprintf(fp, "push \"dhcp-option DNS %s\"\n", conf->lan_ipaddr);
			fprintf(fp, "push \"route %s\"\n", conf->lan_ipaddr);
			if(conf->if_type == OVPN_IF_TUN)
				fprintf(fp, "push \"block-outside-dns\"\n");
		}
	}

	//ignore client certificate
	if(conf->auth_mode == OVPN_AUTH_TLS && conf->useronly){
		fprintf(fp, "verify-client-cert none\n");
		fprintf(fp, "username-as-common-name\n");
	}

//Data Channel Encryption Options:
	fprintf(fp, "\n# Data Channel Encryption Options\n");

	//HMAC digest algorithm
	if(strlen(conf->digest) && strcmp(conf->digest, "default")){
		fprintf(fp, "auth %s\n", conf->digest);
		fprintf(fp_client, "auth %s\n", conf->digest);
	}

	//cipher
	if(strlen(conf->cipher) && strcmp(conf->cipher, "default")){
		fprintf(fp, "cipher %s\n", conf->cipher);
		fprintf(fp_client, "cipher %s\n", conf->cipher);
	}
	if(!conf->ncp_enable)
		fprintf(fp, "ncp-disable\n");
	if(conf->auth_mode == OVPN_AUTH_STATIC){
		fprintf(fp, "secret static.key\n");
	}

//TLS Mode Options:
	if(conf->auth_mode == OVPN_AUTH_TLS){
		fprintf(fp, "\n# TLS Mode Options\n");
		//certificate and keys
		fprintf(fp, "ca ca.crt\n");
		fprintf(fp, "dh dh.pem\n");
		fprintf(fp, "cert server.crt\n");
		fprintf(fp, "key server.key\n");

		if(ovpn_key_exists(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CRL))
			fprintf(fp, "crl-verify crl.pem\n");
		if(ovpn_key_exists(OVPN_TYPE_SERVER, unit, OVPN_SERVER_EXTRA))
			fprintf(fp, "extra-certs server_extra.crt\n");

		//TLS-Auth, HMAC
		if(conf->direction >= 0){
			fprintf(fp, "tls-auth static.key");
			if(conf->direction < 2)
				fprintf(fp, " %d", conf->direction);
			fprintf(fp, "\n");
		}

		//TLS Renegotiation Time
		if(conf->reneg >= 0){
			fprintf(fp, "reneg-sec %d\n", conf->reneg);
			fprintf(fp_client, "reneg-sec %d\n", conf->reneg);
		}

		//Require that peer certificate was signed with an explicit nsCertType designation of "client" or "server".
		fprintf(fp_client, "remote-cert-tls server\n");
	}

	//custom config
	fprintf(fp, "\n# Custom Configuration\n");
	fputs(conf->custom, fp);
	fclose(fp);

	// Write client inline certification and key
	if(conf->auth_mode == OVPN_AUTH_TLS){
		fprintf(fp_client, "<ca>\n%s\n</ca>\n\n"
			, get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CA, buf, sizeof(buf))
		);

		if(!conf->useronly) {
			ret = _ovpn_server_verify_client_crt(unit);
			if(ret){
				if(ret == 2){
					fprintf(fp_client, "#\n# Update client certificate and key if necessary\n#\n");
				}
				fprintf(fp_client, "<cert>\n%s\n</cert>\n\n"
					, get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CLIENT_CERT, buf, sizeof(buf))
				);
				fprintf(fp_client, "<key>\n%s\n</key>\n\n"
					, get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_CLIENT_KEY, buf, sizeof(buf))
				);
			}else{
				fprintf(fp_client, "<cert>\n    paste client certificate data here\n</cert>\n\n"
					"<key>\n    paste client key data here\n</key>\n\n"
				);
			}
		}

		if(conf->direction >= 0) {
			fprintf(fp_client, "<tls-auth>\n%s\n</tls-auth>\n\n"
				, get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_STATIC, buf, sizeof(buf))
			);
			if(conf->direction == 1)
				fprintf(fp_client, "key-direction 0\n\n");
			else if(conf->direction == 0)
				fprintf(fp_client, "key-direction 1\n\n");
		}
	}
	else if(conf->auth_mode == OVPN_AUTH_STATIC){
		fprintf(fp_client, "<secret>\n%s\n</secret>\n\n"
			, get_ovpn_key(OVPN_TYPE_SERVER, unit, OVPN_SERVER_STATIC, buf, sizeof(buf))
		);
	}
	fclose(fp_client);
	return 0;
}

static int _ovpn_client_gen_conf(int unit, ovpn_cconf_t *conf)
{
	char buf[BUF_SIZE];
	FILE *fp;

	snprintf(buf, sizeof(buf), "%s/client%d/config.ovpn", OVPN_DIR_CONF, unit);
	fp = fopen(buf, "w");

	if(!fp)
		return -1;

// Tunnel options
	fprintf(fp,
		"# Automatically generated configuration\n\n# Tunnel options\n"
	);

	//remote
	fprintf(fp, "remote %s\n", conf->addr);

	//retry resolve hostname
	if(conf->retry >= 0)
		fprintf(fp, "resolv-retry %d\n", conf->retry);
	else
		fprintf(fp, "resolv-retry infinite\n");

	//Do not bind to local address and port
	fprintf(fp, "nobind\n");

	//protocol
	fprintf(fp, "proto %s\n", conf->proto);

	//port
	fprintf(fp, "port %d\n", conf->port);

	//dev
	fprintf(fp, "dev %s\n", conf->if_name);

	//ip pool
	if(conf->auth_mode == OVPN_AUTH_STATIC){
		if(conf->if_type == OVPN_IF_TUN){
			fprintf(fp, "ifconfig %s %s\n", conf->local, conf->remote);
		}
		//else if( conf->if_type == OVPN_IF_TAP ) {
			//fprintf(fp, "ifconfig %s %s\n", conf->remote, conf->netmask);
		//}
	}
	fprintf(fp, "route-noexec\n");
	//TODO: TLS recevie routing info, add/ignore rules for dual wan multi route table
	// redirect gateway
	if(conf->redirect_gateway){
		if(conf->if_type == OVPN_IF_TAP && strlen(conf->gateway))
			fprintf(fp, "route-gateway %s\n", conf->gateway);
		fprintf(fp, "redirect-gateway def1\n");
	}

	//OS socket buffer
	fprintf(fp, "sndbuf 0\n");
	fprintf(fp, "rcvbuf 0\n");

	//Don't close and reopen TUN/TAP device
	fprintf(fp, "persist-tun\n");

	//Don't re-read key files across SIGUSR1 or --ping-restart
	fprintf(fp, "persist-key\n");

	fprintf(fp, "setenv ovpn_type %d\n", 1);
	fprintf(fp, "setenv unit %d\n", unit);
	fprintf(fp, "setenv adns %d\n", conf->adns);
    if(get_default_gateway_dev(buf, sizeof(buf)))
      fprintf(fp, "setenv route_net_defdev %s\n", buf);
	fprintf(fp, "setenv nat %d\n", conf->nat);
	fprintf(fp, "script-security 2\n");

	//progname
	fprintf(fp, "daemon vpnclient%d\n", unit);

	//log verbosity
	if(conf->verb >= 0)
		fprintf(fp, "verb %d\n", conf->verb);

	//status
	fprintf(fp, "status-version 2\n");
	fprintf(fp, "status status 10\n");

	//compression
	if(strlen(conf->comp) && strcmp(conf->comp, "-1")){
		if(!strcmp(conf->comp, "adaptive"))
			fprintf(fp, "comp-lzo adaptive\n");
		else if(!strcmp(conf->comp, "no"))
			fprintf(fp, "compress\n");
		else if(!strcmp(conf->comp, "yes"))
			fprintf(fp, "compress lzo\n");
		else
			fprintf(fp, "compress %s\n", conf->comp);
	}

// Client Mode
	fprintf(fp, "\n# Client Mode\n");
	if(conf->auth_mode == OVPN_AUTH_TLS){
		fprintf(fp, "client\n");
		//authentication
		if(conf->userauth){
			fprintf(fp, "auth-user-pass up\n");
		}
	}

//Data Channel Encryption Options:
	fprintf(fp, "\n# Data Channel Encryption Options\n");

	//HMAC digest algorithm
	if(strlen(conf->digest) && strcmp(conf->digest, "default")){
		fprintf(fp, "auth %s\n", conf->digest);
	}

	//cipher
	if(strlen(conf->cipher) && strcmp(conf->cipher, "default")){
		fprintf(fp, "cipher %s\n", conf->cipher);
	}

	if(conf->auth_mode == OVPN_AUTH_STATIC){
		fprintf(fp, "secret static.key\n");
	}

//TLS Mode Options:
	if(conf->auth_mode == OVPN_AUTH_TLS ){
		fprintf(fp, "\n# TLS Mode Options\n");
		//certificate and keys
		fprintf(fp, "ca ca.crt\n");
		if(!conf->useronly){
			fprintf(fp, "cert client.crt\n");
			fprintf(fp, "key client.key\n");
			if(ovpn_key_exists(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_EXTRA))
				fprintf(fp, "extra-certs client_extra.crt\n");
		}

		if(ovpn_key_exists(OVPN_TYPE_CLIENT, unit, OVPN_CLIENT_CRL))
			fprintf(fp, "crl-verify crl.pem\n");

		if(conf->verify_x509_type)
			fprintf(fp, "verify-x509-name %s %s\n", conf->verify_x509_name, "name");

		if(conf->tlscrypt)
			fprintf(fp, "tls-crypt static.key\n");

		//TLS-Auth, HMAC
		else if(conf->direction >= 0){
			fprintf(fp, "tls-auth static.key");
			if(conf->direction < 2)
				fprintf(fp, " %d", conf->direction);
			fprintf(fp, "\n");
		}

		//TLS Renegotiation Time
		if(conf->reneg >= 0){
			fprintf(fp, "reneg-sec %d\n", conf->reneg);
		}
	}

	//custom config
	fprintf(fp, "\n# Custom Configuration\n");
	fputs(conf->custom, fp);
	fprintf(fp, "\n");
	//to update DNS info
	fprintf(fp, "up '/etc/openvpn/ovpnc-up %d'\n", unit);
	fprintf(fp, "down '/etc/openvpn/ovpnc-down %d'\n", unit);
	fprintf(fp, "route-up '/etc/openvpn/ovpnc-route-up %d'\n", unit);
	fprintf(fp, "route-pre-down '/etc/openvpn/ovpnc-route-pre-down %d'\n", unit);
	fclose(fp);
	return 0;
}

static void _ovpn_stop_wait(ovpn_type_t type, int unit)
{
	char name[16] = {0};
	int n = 0;

	if( _ovpn_is_running(type, unit) == 0 )
		return;

	if(type == OVPN_TYPE_CLIENT)
		snprintf(name, sizeof(name), "vpnclient%d", unit);
	else if(type == OVPN_TYPE_SERVER)
		snprintf(name, sizeof(name), "vpnserver%d", unit);

	if ( killall(name, SIGTERM) == 0) {
		n = 10;
		while ((killall(name, 0) == 0) && (n-- > 0)) {
			_dprintf("%s: waiting %s n=%d\n", __FUNCTION__, name, n);
			sleep(1);
		}
		if (n < 0) {
			n = 10;
			while ((killall(name, SIGKILL) != 0) && (n-- > 0)) {
				_dprintf("%s: SIGKILL %s n=%d\n", __FUNCTION__, name, n);
				sleep(1);
			}
		}
	}
}

static int _is_dup_ovpn_accnt(char *accnt_str)
{
	ovpn_accnt_info_t accnt_info;
	int i;
	int is_duplicate = 0;

	if( !accnt_str )
		return 0;

	get_ovpn_accnt(&accnt_info);

	for (i = 0; i < accnt_info.count; i++) {
		//_dprintf("%s-account(%s)-(%s)\n", __FUNCTION__, account_str, username);
		if( !strcmp(accnt_str, accnt_info.account[i].username) ) {
			is_duplicate = 1;
			break;
		}
	}

	return is_duplicate;
}

/*
	work around, OpenVpn server account would duplicate with system and samba
	need to deal with it before a formal account management function is
	implmented, or OpenVpn account won't work..
	Samba account only need an entry, so when account is duplicate with openvpn
	account, replaced with OpenVPN account
*/
void append_ovpn_accnt(const char *path, const char *ovpn_path)
{
	char tmpfile[256];
	char buf[512];
	char account[32];
	FILE *fp;
	FILE *fp_tmp;

	if(!path || !ovpn_path)
		return;

	if(!f_exists(path) || !f_exists(ovpn_path))
		return;

	//remove duplicate account
	snprintf(tmpfile, sizeof(tmpfile), "%s.tmp", path);
	if((fp = fopen(path, "r")) != NULL){
		if((fp_tmp = fopen(tmpfile, "w+")) != NULL){
			//first line for admin account
			if(fgets(buf, sizeof(buf), fp)){
				fputs(buf, fp_tmp);
			}

			while(fgets(buf, sizeof(buf), fp)){
				if(sscanf(buf, "%31[^:]:%*s", account) == 1){
					//_dprintf("%s-account(%s)\n", __FUNCTION__, account);
					if(!_is_dup_ovpn_accnt(account)){
						fputs(buf, fp_tmp);
					}
				}else{
					fputs(buf, fp_tmp);
				}
			}
			fclose(fp_tmp);

			unlink(path);
			rename(tmpfile, path);
			chmod(path, 0644);
			snprintf(buf, sizeof(buf), "cd /etc; cat %s >> %s", ovpn_path, path);
			system(buf);
		}
		fclose(fp);
	}
}

void create_ovpn_passwd()
{
	FILE *fps, *fpp;
	unsigned char s[512];
	char salt[32], *p;
	ovpn_accnt_info_t account_info;
	int gid = 200; /* OpenVPN GID */
	int uid = 200;
	int i;

	strcpy(salt, "$1$");
	f_read("/dev/urandom", s, 6);
	base64_encode(s, salt + 3, 6);
	salt[3 + 8] = 0;
	p = salt;
	while(*p){
		if(*p == '+') *p = '.';
		++p;
	}

	fps = fopen("/etc/shadow.openvpn", "w");
	fpp = fopen("/etc/passwd.openvpn", "w");
	if(fps == NULL || fpp == NULL)
		goto error;

	get_ovpn_accnt(&account_info);

	for(i = 0; i < account_info.count; i++){
		p = crypt(account_info.account[i].password, salt);
		fprintf(fps, "%s:%s:0:0:99999:7:0:0:\n", account_info.account[i].username, p);
		fprintf(fpp, "%s:x:%d:%d::/dev/null:/dev/null\n", account_info.account[i].username, uid, gid);
		uid++;
	}

error:
	if(fps)
		fclose(fps);
	if(fpp)
		fclose(fpp);

	chmod("/etc/shadow.openvpn", S_IRUSR|S_IWUSR);
	chmod("/etc/passwd.openvpn", S_IRUSR|S_IWUSR);
}

void run_ovpn_fw_nat_scripts()
{
	char buf[128];
	int unit;
	int lock;
	lock = file_lock("openvpn_server");
	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++){
		snprintf(buf, sizeof(buf), "%s/server%d/fw_nat6.sh", OVPN_DIR_CONF, unit);
		if(f_exists(buf))
			eval(buf);
	}
	file_unlock(lock);
	lock = file_lock("openvpn_client");
	for(unit = 1; unit <= OVPN_CLIENT_MAX; unit++){
		snprintf(buf, sizeof(buf), "%s/client%d/fw_nat.sh", OVPN_DIR_CONF, unit);
		if(f_exists(buf))
			eval(buf);
		snprintf(buf, sizeof(buf), "%s/client%d/fw_nat6.sh", OVPN_DIR_CONF, unit);
		if(f_exists(buf))
			eval(buf);
	}
	file_unlock(lock);
}

void run_ovpn_fw_scripts()
{
	char buf[128];
	int unit;
	int lock;
	lock = file_lock("openvpn_server");
	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++){
		snprintf(buf, sizeof(buf), "%s/server%d/fw.sh", OVPN_DIR_CONF, unit);
		if(f_exists(buf))
			eval(buf);
	}
	file_unlock(lock);
	lock = file_lock("openvpn_client");
	for(unit = 1; unit <= OVPN_CLIENT_MAX; unit++){
		snprintf(buf, sizeof(buf), "%s/client%d/fw.sh", OVPN_DIR_CONF, unit);
		if(f_exists(buf))
			eval(buf);
	}
	file_unlock(lock);
}

void start_ovpn_serverall()
{
	int lock, unit;
	ovpn_sconf_common_t sconf_common;
	lock = file_lock("openvpn_server");
	get_ovpn_sconf_common(&sconf_common);
	// start servers
	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++){
		if(sconf_common.enable[unit - 1] == 1 ){
			if(_ovpn_is_running(OVPN_TYPE_SERVER, unit))
				stop_ovpn_server(unit);
			start_ovpn_server(unit);
		}
	}
	file_unlock(lock);
}

void start_ovpn_clientall()
{
	int lock, unit;
	ovpn_cconf_common_t cconf_common;
	lock = file_lock("openvpn_client");
	get_ovpn_cconf_common(&cconf_common);
	// start clients
	for(unit = 1; unit <= OVPN_CLIENT_MAX; unit++){
		if(cconf_common.enable[unit - 1] == 1){
			if(_ovpn_is_running(OVPN_TYPE_CLIENT, unit))
				stop_ovpn_client(unit);
			start_ovpn_client(unit);
		}
	}
	file_unlock(lock);
}


void start_ovpn_eas()
{
	start_ovpn_serverall();
	start_ovpn_clientall();
}

void start_ovpn_client(int unit)
{
	char buf[256];
	ovpn_cconf_t conf;
	int ret;

	if(unit == 0 || unit > OVPN_CLIENT_MAX)
		return;

	if(_ovpn_is_running(OVPN_TYPE_CLIENT, unit)){
		_dprintf("OpenVPN Client %d is running\n", unit);
		return;
	}

	//get config
	if(!get_ovpn_cconf(unit, &conf)){
		_dprintf("get_ovpn_cconf failed\n");
		update_ovpn_status(OVPN_TYPE_CLIENT, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_CLIENT, unit, NULL);
		return;
	}

	if(conf.enable == 0)
		return;

    if(wait_time_sync(10) < 0)
    {
		conf.poll = 1;
		_ovpn_cron_job_add(OVPN_TYPE_CLIENT, unit, &conf);
		logmessage_normal(conf.progname, "Time not sync yet. Retry later");
		return;
    }
	//initializing
	update_ovpn_status(OVPN_TYPE_CLIENT, unit, OVPN_STS_INIT, OVPN_ERRNO_NONE);

	//work directory
	_ovpn_check_dir(OVPN_TYPE_CLIENT, unit);

	//load module
	eval("modprobe", "tun");
	f_wait_exists("/dev/net/tun", 5);

	//setup interface
	_ovpn_tunnel_create(conf.if_type, conf.if_name);

	//generate config and script files
	if(_ovpn_client_gen_conf(unit, &conf) < 0){
		logmessage_normal(conf.progname, "config failed");
		update_ovpn_status(OVPN_TYPE_CLIENT, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_CLIENT, unit, NULL);
		return;
	}
	if( (ret = _ovpn_client_write_keys(unit, &conf)) < 0 ) {
		_dprintf("_ovpn_client_write_keys return %d\n", ret);
		update_ovpn_status(OVPN_TYPE_CLIENT, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_CLIENT, unit, NULL);
		return;
	}

	//start
	//[<CPUx>] openvpn --cd <dir> --config <file> NULL
	char *argv[] = { "", "", "", "", "", "", NULL};
	int argc = 1, cpumask = 0;
	snprintf(buf, sizeof(buf), "%s/vpnclient%d --cd %s/client%d --config config.ovpn"
		, OVPN_DIR_CONF, unit, OVPN_DIR_CONF, unit);
	for (argv[argc] = strtok(buf, " "); argv[argc] != NULL; argv[++argc] = strtok(NULL, " "));
	if((cpumask = adjust_smp_affinity(OVPN_TYPE_CLIENT, unit))) {
		_cpu_mask_eval(argv, NULL, 0, NULL, cpumask);
	}
	else {
		_eval(argv+1, NULL, 0, NULL);
	}

	//firewall
	_ovpn_client_fw_rule_add(unit, &conf);

	//cron job
	_ovpn_cron_job_del(OVPN_TYPE_CLIENT, unit);
	_ovpn_cron_job_add(OVPN_TYPE_CLIENT, unit, &conf);

}

void start_ovpn_server(int unit)
{
	char buf[256];
	ovpn_sconf_t conf;
	int ret;

	if(unit == 0 || unit > OVPN_SERVER_MAX)
		return;

	if( _ovpn_is_running(OVPN_TYPE_SERVER, unit) ) {
		_dprintf("OpenVPN Server %d is running\n", unit);
		return;
	}

	//get config
	if( !get_ovpn_sconf(unit, &conf) ) {
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_SERVER, unit, NULL);
		return;
	}

	if(conf.enable == 0)
		return;

	//initializing
	update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_INIT, OVPN_ERRNO_NONE);

	//work directory
	_ovpn_check_dir(OVPN_TYPE_SERVER, unit);

	//load module
	eval("modprobe", "tun");
	f_wait_exists("/dev/net/tun", 5);

	//setup interface
	_ovpn_tunnel_create(conf.if_type, conf.if_name);

	//generate config and script files
	_ovpn_server_check_keys(unit, &conf);
	if((ret = _ovpn_server_write_keys(unit, &conf)) < 0){
		_dprintf("_ovpn_server_write_keys return %d\n", ret);
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_SERVER, unit, NULL);
		return;
	}
	if(_ovpn_server_gen_conf(unit, &conf) < 0){
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		_ovpn_cron_job_add(OVPN_TYPE_SERVER, unit, NULL);
		return;
	}

	//start
	//[<CPUx>] openvpn --cd <dir> --config <file> NULL
	char *argv[] = { "", "", "", "", "", "", NULL};
	int argc = 0, cpumask = 0;
	snprintf(buf, sizeof(buf),
		"%s/vpnserver%d --cd %s/server%d --config config.ovpn"
		, OVPN_DIR_CONF, unit, OVPN_DIR_CONF, unit);
	for (argv[argc] = strtok(buf, " "); argv[argc] != NULL; argv[++argc] = strtok(NULL, " "));
	if((cpumask = adjust_smp_affinity(OVPN_TYPE_SERVER, unit))) {
		ret = _cpu_mask_eval(argv, NULL, 0, NULL, cpumask);
	}
	else {
		ret = _eval(argv, NULL, 0, NULL);
	}
	if(ret){
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_ERROR, OVPN_ERRNO_CONF);
		return;
	}

	//firewall
	_ovpn_server_fw_rule_add(unit, &conf);

	//cron job
	_ovpn_cron_job_del(OVPN_TYPE_SERVER, unit);
	_ovpn_cron_job_add(OVPN_TYPE_SERVER, unit, &conf);

	//running
	if(conf.auth_mode == OVPN_AUTH_STATIC){
		update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_RUNNING, OVPN_ERRNO_NONE);
	}
}

void stop_ovpn_eas()
{
	stop_ovpn_serverall();
	stop_ovpn_clientall();
}

void stop_ovpn_client(int unit)
{
	char buf[256];
	ovpn_cconf_t conf;

	if(unit == 0 || unit > OVPN_CLIENT_MAX)
		return;

	get_ovpn_cconf(unit, &conf);

	// Remove cron
	_ovpn_cron_job_del(OVPN_TYPE_CLIENT, unit);

	// Remove firewall rules
	_ovpn_client_fw_rule_del(unit);

	// Stop the VPN client
	_ovpn_stop_wait(OVPN_TYPE_CLIENT, unit);

	// remove interface
	_ovpn_tunnel_remove(OVPN_TYPE_CLIENT, unit);

	eval("modprobe", "-r", "tun");

	// Delete config
	snprintf(buf, sizeof(buf), "%s/client%d", OVPN_DIR_CONF, unit);
	eval("rm", "-rf", buf);

	update_ovpn_status(OVPN_TYPE_CLIENT, unit, OVPN_STS_STOP, OVPN_ERRNO_NONE);
}

void stop_ovpn_server(int unit)
{
	char buf[256];
	ovpn_sconf_t conf;

	if(unit == 0 || unit > OVPN_SERVER_MAX)
		return;

	update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_STOPPING, OVPN_ERRNO_NONE);

	get_ovpn_sconf(unit, &conf);

	// Remove cron
	_ovpn_cron_job_del(OVPN_TYPE_SERVER, unit);

	// Remove firewall rules
	_ovpn_server_fw_rule_del(unit);

	// Stop the VPN server
	_ovpn_stop_wait(OVPN_TYPE_SERVER, unit);

	// Remove interface
	_ovpn_tunnel_remove(OVPN_TYPE_SERVER, unit);

	// Remove module
	eval("modprobe", "-r", "tun");

	// Delete config
	snprintf(buf, sizeof(buf), "%s/server%d", OVPN_DIR_CONF, unit);
	eval("rm", "-rf", buf);

	update_ovpn_status(OVPN_TYPE_SERVER, unit, OVPN_STS_STOP, OVPN_ERRNO_NONE);
}

void stop_ovpn_clientall()
{
	int lock, unit;

	lock = file_lock("openvpn_client");
	for(unit = 1; unit <= OVPN_CLIENT_MAX; unit++) {
		if( _ovpn_is_running(OVPN_TYPE_CLIENT, unit) )
			stop_ovpn_client(unit);
	}
	file_unlock(lock);
}

void stop_ovpn_serverall() {
	int lock, unit;

	lock = file_lock("openvpn_server");
	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++) {
		if( _ovpn_is_running(OVPN_TYPE_SERVER, unit) )
			stop_ovpn_server(unit);
	}
	file_unlock(lock);
}

void write_ovpn_dnsmasq_config(FILE* f)
{
	ovpn_sconf_common_t sconf_common;
	ovpn_sconf_t sconf;
	int unit;

	get_ovpn_sconf_common(&sconf_common);

	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++){
		if(sconf_common.dns[unit - 1]){
			get_ovpn_sconf(unit, &sconf);
			fprintf(f, "interface=%s\n", sconf.if_name);
		}
	}

	//TODO: VPN client recevie DNS info, update DNS policy/server info
}

int write_ovpn_resolv_dnsmasq(FILE* fp_servers)
{
	int unit, ret = 0;
	FILE *fp;
	char buf[128];
	char path[128];

	for(unit = 1; unit <= OVPN_CLIENT_MAX; unit++){
		snprintf(path, sizeof(path), "%s/client%d/resolv.dnsmasq", "/etc/openvpn", unit);
		fp = fopen(path, "r");
		if(fp){
			while(!feof(fp)){
				if(fgets(buf, sizeof(buf), fp)){
					if(strlen(buf) > 7){
						fputs(buf, fp_servers);
 						ret = 1;
					}
				}
			}
			fclose(fp);
		}
	}
	return ret;
}

void update_ovpn_profie_remote()
{
	ovpn_sconf_common_t sconf_common;
	int unit;
	char file_path[128];
	char address[64];
	char cmd[256];

	get_ovpn_sconf_common(&sconf_common);
	for(unit = 1; unit <= OVPN_SERVER_MAX; unit++){
		if(sconf_common.enable[unit - 1]){
			snprintf(file_path, sizeof(file_path), "%s/server%d/client.ovpn", OVPN_DIR_CONF, unit);
			if(f_exists(file_path) && f_size(file_path) > 0){
				get_ovpn_sconf_remote(address, sizeof(address));
				snprintf(cmd, sizeof(cmd), "sed -i 's/remote[ ]\\+[^ ]*/remote %s/' %s", address, file_path);
				system(cmd);
			}
		}
	}
}

static void _ovpn_client_fw_rule_add_nat(int unit, const char *dev, const char *ip, int isipv6)
{
	FILE *fp;
	char path[256];
	if(isipv6)
		snprintf(path, sizeof(path), "/etc/openvpn/client%d/fw_nat6.sh", unit);
	else
		snprintf(path, sizeof(path), "/etc/openvpn/client%d/fw_nat.sh", unit);
	if((fp = fopen(path, "w")) != NULL){
		fprintf(fp, "#!/bin/sh\n\n");
		fprintf(fp, "%s -t nat -I POSTROUTING ! -s %s -o %s -j MASQUERADE", isipv6 ? "ip6tables" : "iptables", ip, dev);
		fclose(fp);
		chmod(path, 0700u);
		eval(path);
	}
}

static void _ovpn_client_fw_rule_del_nat(int unit, const char *dev, const char *ip, int isipv6)
{
	char path[256];
	if(isipv6)
		snprintf(path, sizeof(path), "/etc/openvpn/client%d/fw_nat6.sh", unit);
	else
		snprintf(path, sizeof(path), "/etc/openvpn/client%d/fw_nat.sh", unit);
	if(f_exists(path)){
		eval("sed", "-i", "s/-I/-D/", path);
		eval(path);
		unlink(path);
	}
}

static void ovpn_env_foreign_option_handler(int unit, int adns)
{
	char env[32] = "foreign_option_XXX";
	char* foreign_option_x = NULL, *next, *next2;
	int i = 1;
	char server_path[128], dns_buf[1024], dm_buf[1024], word[128], word2[128];
	FILE* fp;
	memset(dns_buf, 0, sizeof(dns_buf));
	memset(dm_buf, 0, sizeof(dm_buf));
	snprintf(server_path, sizeof(server_path), "%s/client%d/resolv.dnsmasq", OVPN_DIR_CONF, unit);
	unlink(server_path);

	while(1){
		snprintf(env, sizeof(env), "foreign_option_%d", i++);
		foreign_option_x = getenv(env);
		if(!foreign_option_x)
			break;
		else{
			_dprintf("foreign option: %s\n", foreign_option_x);
			if(!strncmp(foreign_option_x, "dhcp-option DNS ", 16)){
				if(adns){
					strlcat(dns_buf, &foreign_option_x[16], sizeof(dns_buf));
					strlcat(dns_buf, " ", sizeof(dns_buf));
					fp = fopen(server_path, "a");
					if(fp){
						fprintf(fp, "server=%s\n", &foreign_option_x[16]);
						fclose(fp);
					}
				}
			}
			else if(!strncmp(foreign_option_x, "dhcp-option DOMAIN ", 19)){
				if(adns){
					strlcat(dm_buf, &foreign_option_x[19], sizeof(dm_buf));
					strlcat(dm_buf, " ", sizeof(dm_buf));
				}
			}
		}
	}
	foreach(word, dm_buf, next){
		foreach(word2, dns_buf, next2){
			if((fp = fopen(server_path, "a")) != NULL){
				fprintf(fp, "server=/%s/%s\n", word, word2);
				fclose(fp);
			}
		}
	}
}

void ovpn_up_handler()
{
	int unit = atoi(safe_getenv("unit"));
	ovpn_type_t ovpn_type = strcmp(safe_getenv("ovpn_type"), "0") ? OVPN_TYPE_CLIENT : OVPN_TYPE_SERVER;
	int adns = atoi(safe_getenv("adns"));
	int nat = atoi(safe_getenv("nat"));

	if(ovpn_type == OVPN_TYPE_CLIENT){
		ovpn_env_foreign_option_handler(unit, adns);
		if(nat){
			if(!strcmp(safe_getenv("dev_type"), "tun")){
				char *local = safe_getenv("ifconfig_local");
				char *local6 = safe_getenv("ifconfig_ipv6_local");
				char *dev = safe_getenv("dev");
				if(*local)
					_ovpn_client_fw_rule_add_nat(unit, dev, local, 0);
				if(*local6)
					_ovpn_client_fw_rule_add_nat(unit, dev, local6, 1);
			}
		}
	}
	if(get_ovpn_status(ovpn_type, unit) != OVPN_STS_ERROR || (get_ovpn_errno(ovpn_type, unit) & 3) == 0)
		update_ovpn_status(ovpn_type, unit, OVPN_STS_RUNNING, OVPN_ERRNO_NONE);
}

void ovpn_down_handler()
{
	char path[128];
	int unit = atoi(safe_getenv("unit"));
	ovpn_type_t ovpn_type = strcmp(safe_getenv("ovpn_type"), "0") ? OVPN_TYPE_CLIENT : OVPN_TYPE_SERVER;
	int nat = atoi(safe_getenv("nat"));

	if(ovpn_type == OVPN_TYPE_CLIENT){
		snprintf(path, sizeof(path), "%s/client%d/resolv.dnsmasq", "/etc/openvpn", unit);
		unlink(path);
		if(nat){
			if(!strcmp(safe_getenv("dev_type"), "tun")){
				char *local = safe_getenv("ifconfig_local");
				char *local6 = safe_getenv("ifconfig_ipv6_local");
				char *dev = safe_getenv("dev");
				if(*local)
					_ovpn_client_fw_rule_del_nat(unit, dev, local, 0);
				if(*local6)
					_ovpn_client_fw_rule_del_nat(unit, dev, local6, 1);
			}
		}
	}
}

void ovpn_route_up_handler()
{
	FILE *fp;
	int i = 1;
	int unit = atoi(safe_getenv("unit"));
	ovpn_type_t ovpn_type = strcmp(safe_getenv("ovpn_type"), "0") ? OVPN_TYPE_CLIENT : OVPN_TYPE_SERVER;
	int routes_flags = atoi(safe_getenv("routes_flags"));
	int rgi_flags = atoi(safe_getenv("rgi_flags"));
	int rt_table = atoi(safe_getenv("rt_table"));
	char progname[32], cmd[256], prefix[32], buf[128];
	char *pnw, *pnm, *pgw, *pnw6, *pgw6;
	struct sockaddr_in sa_network;
	struct sockaddr_in sa_netmask;
	memset(buf, 0, sizeof(buf));
	if(ovpn_type == OVPN_TYPE_CLIENT)
		snprintf(progname, sizeof(progname), "vpnclient%d", unit);
	else
		snprintf(progname, sizeof(progname), "vpnserver%d", unit);
	if(rt_table){
		snprintf(cmd, sizeof(cmd), "ip route flush table %d", rt_table);
		system(cmd);
		system("ip route show table main > /tmp/route_tmp");
		if((fp = fopen("/tmp/route_tmp", "r"))){
			while(fgets(buf, sizeof(buf), fp)){
				snprintf(cmd, sizeof(cmd), "ip route add %s table %d ", trimNL(buf), rt_table);
				system(cmd);
			}
			fclose(fp);
		}
		unlink("/tmp/route_tmp");
	}
	while(1){
		snprintf(prefix, sizeof(prefix), "route_network_%d", i);
		pnw = safe_getenv(prefix);
		if(!*pnw)
			break;
		snprintf(prefix, sizeof(prefix), "route_netmask_%d", i);
		pnm = safe_getenv(prefix);
		snprintf(prefix, sizeof(prefix), "route_gateway_%d", i);
		pgw = safe_getenv(prefix);
		if(inet_pton(AF_INET,pnw, &(sa_network.sin_addr)) > 0 && inet_pton(AF_INET,pnm, &(sa_netmask.sin_addr)) > 0){
			_dprintf("route: %s / %s via %s\n", pnw, pnm, pgw);
			if(current_route(sa_network.sin_addr.s_addr, sa_netmask.sin_addr.s_addr) || current_addr(sa_network.sin_addr.s_addr)){
				if((sa_network.sin_addr.s_addr & sa_netmask.sin_addr.s_addr) != 0){
					_dprintf("route conflict: %s/%s\n", pnw, pnm);
					logmessage_normal(progname, "WARNING: Ignore conflicted routing rule: %s %s gw %s", pnw, pnm, pgw);
				}else{
					_dprintf("Detect default gateway: %s/%s\n", pnw, pnm);
					logmessage_normal(progname, "WARNING: Replace default vpn gateway by using 0.0.0.0/1 and 128.0.0.0/1");
					routes_flags |= 36;
				}
				update_ovpn_status(ovpn_type, unit, OVPN_STS_ERROR, OVPN_ERRNO_ROUTE);
			}else{
				snprintf(cmd, sizeof(cmd), "ip route add %s/%d via %s", pnw, convert_subnet_mask_to_cidr(pnm), pgw);
				if(rt_table)
					snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
				if(system(cmd))
					_dprintf("cmd failed: %s\n", cmd);
			}
		}
		++i;
	}
	if((routes_flags & 0x20) != 0){
		char *dev = safe_getenv("dev");
		char *route_vpn_gateway = safe_getenv("route_vpn_gateway");
		char *local = safe_getenv("ifconfig_local");
		if((rgi_flags & 8) != 0)
			snprintf(cmd, sizeof(cmd), "ip route add %s/32 via %s", safe_getenv("trusted_ip"), safe_getenv("route_net_gateway"));
		else
			snprintf(cmd, sizeof(cmd), "ip route add %s/32 dev %s", safe_getenv("trusted_ip"), safe_getenv("route_net_defdev"));
		if(rt_table)
			snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
		_dprintf("rgi_flags: %d\n", rgi_flags);
		_dprintf("%s\n", cmd);
		system(cmd);
		if(*route_vpn_gateway){
			snprintf(cmd, sizeof(cmd), "ip route add 0.0.0.0/1 via %s dev %s", route_vpn_gateway, dev);
			if(rt_table)
				snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
			system(cmd);
			snprintf(cmd, sizeof(cmd), "ip route add 128.0.0.0/1 via %s dev %s", route_vpn_gateway, dev);
			if(rt_table)
				snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
			system(cmd);
			if(!strcmp(safe_getenv("dev_type"), "tap") && *local){
				eval("brctl", "delif", "br0", dev);
				_ovpn_client_fw_rule_add_nat(unit, dev, local, 0);
			}
		}
	}
	i = 1;
	while(1){
		snprintf(prefix, sizeof(prefix), "route_ipv6_network_%d", i);
		pnw6 = safe_getenv(prefix);
		if(!*pnw6)
			break;
		snprintf(prefix, sizeof(prefix), "route_ipv6_gateway_%d", i);
		pgw6 = safe_getenv(prefix);
		snprintf(cmd, sizeof(cmd), "ip -6 route add %s via %s", pnw6, pgw6);
		if(rt_table)
			snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
		system(cmd);
		i++;
	}
	if((routes_flags & 0x20) != 0){
		char *ip6 = safe_getenv("trusted_ip6");
		char *gw6 = safe_getenv("net_gateway_ipv6");
		char *defdev = safe_getenv("route_net_defdev");
		if((rgi_flags & 8) != 0)
			snprintf(cmd, sizeof(cmd), "ip -6 route add %s/128 via %s", ip6, gw6);
		else
			snprintf(cmd, sizeof(cmd), "ip -6 route add %s/128 dev %s", ip6, defdev);
		if(rt_table)
			snprintf(&cmd[strlen(cmd)], sizeof(cmd) - strlen(cmd), " table %d", rt_table);
		_dprintf("%s\n", cmd);
		system(cmd);
	}
}

void _ovpn_route_down_handler()
{
	int rt_table = atoi(safe_getenv("rt_table"));
	int i;
	char cmd[256];
	char word[32];
	char *pnw, *pnm, *pgw, *pnw6, *pgw6;
	struct sockaddr_in sa_network;
	struct sockaddr_in sa_netmask;

	if(rt_table){
		snprintf(cmd, sizeof(cmd), "ip route flush table %d", rt_table);
	system(cmd);
	}else{
		snprintf(cmd, sizeof(cmd), "ip route del %s", safe_getenv("trusted_ip"));
		if(system(cmd))
			_dprintf("cmd failed: %s\n", cmd);
		i = 1;
		while(1){
			snprintf(word, sizeof(word), "route_network_%d", i);
			pnw = safe_getenv(word);
			if(!*pnw)
				break;
			snprintf(word, sizeof(word), "route_netmask_%d", i);
			pnm = safe_getenv(word);
			snprintf(word, sizeof(word), "route_gateway_%d", i);
			pgw = safe_getenv(word);
			if(inet_pton(AF_INET,pnw, &(sa_network.sin_addr)) > 0 && inet_pton(AF_INET,pnm, &(sa_netmask.sin_addr)) > 0){
				_dprintf("route: %s / %s via %s\n", pnw, pnm, pgw);
				snprintf(cmd, sizeof(cmd), "ip route del %s/%d via %s", pnw, convert_subnet_mask_to_cidr(pnm), pgw);
				if(system(cmd))
					_dprintf("cmd failed: %s\n", cmd);
			}
			i++;
		}
		i = 1;
		while(1){
			snprintf(word, sizeof(word), "route_ipv6_network_%d", i);
			pnw6 = safe_getenv(word);
			if(!*pnw6)
				break;
			snprintf(word, sizeof(word), "route_ipv6_gateway_%d", i);
			pgw6 = safe_getenv(word);
			snprintf(cmd, sizeof(cmd), "ip -6 route del %s via %s", pnw6, pgw6);
			if(system(cmd))
				_dprintf("cmd failed: %s\n", cmd);
			i++;
		}
	}
}

void ovpn_route_pre_down_handler()
{
	_ovpn_route_down_handler();
}
