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
 * Copyright 2023-2025, SWRTdev
 * Copyright 2023-2025, paldier <paldier@hotmail.com>.
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

int le_acme_exit;

static void _le_jobs_install(int min, char *hostname)
{
	int jobs_min, le_time;
	uint64_t urandom1, urandom2;
	char jobs[128] = {0}, tmp[128] = {0};
	char *le_retry;
	char *le_jobs_argv[] = { "/usr/sbin/cru", "a", "LetsEncrypt", jobs, NULL };
	jobs_min = nvram_get_int("le_jobs_min");
	le_retry = nvram_get("le_retry");
	if(jobs_min == 0){
		if(min == 0){
			f_read("/dev/urandom", &urandom1, sizeof(urandom1));
			f_read("/dev/urandom", &urandom2, sizeof(urandom2));
#if defined(RTCONFIG_MT798X) || defined(RTCONFIG_SOC_IPQ50XX) || defined(RTCONFIG_SOC_IPQ60XX) || defined(RTCONFIG_SOC_IPQ8074)
			snprintf(jobs, sizeof(jobs), "%lu %lu */%d * * service restart_letsencrypt", urandom2 % 60, urandom1 % 24, 7);
#else
			snprintf(jobs, sizeof(jobs), "%llu %llu */%d * * service restart_letsencrypt", urandom2 % 60, urandom1 % 24, 7);
#endif
		}else if(nvram_get_int("le_auxstate_t") != LE_AUX_ACME)
			snprintf(jobs, sizeof(jobs), "*/%d * * * * service restart_letsencrypt", min);
		else if(le_retry && sscanf(le_retry, "%s %u", tmp, &le_time) == 2 && !strncmp(hostname, tmp, sizeof(tmp))){
			snprintf(tmp, sizeof(tmp), "%s %u", hostname, ++le_time);
			nvram_set("le_retry", tmp);
			if((le_time & 3) == 1)
				strlcpy(jobs, "*/1 * * * * service restart_letsencrypt", sizeof(jobs));
			else if((le_time & 3) == 2)
				strlcpy(jobs, "*/10 * * * * service restart_letsencrypt", sizeof(jobs));
			else if((le_time & 3) == 3)
				strlcpy(jobs, "*/59 * * * * service restart_letsencrypt", sizeof(jobs));
			else 
				strlcpy(jobs, "30 12 * * * service restart_letsencrypt", sizeof(jobs));
		}else{
			snprintf(tmp, sizeof(tmp), "%s 1", hostname);
			nvram_set("le_retry", tmp);
			strlcpy(jobs, "*/1 * * * * service restart_letsencrypt", sizeof(jobs));
		}
	}else
		snprintf(jobs, sizeof(jobs), "*/%d * * * * service restart_letsencrypt", jobs_min);
	_eval(le_jobs_argv, NULL, 0, NULL);
}

static void _le_add_fw_rule(le_conf_t *conf, uint16_t port)
{
	FILE *fp;
	fp = fopen("/tmp/.le/le-fw.sh", "w");
	if(fp){
		fprintf(fp, "#!/bin/sh\n\n");
		fprintf(fp, "iptables -I INPUT -m conntrack --ctstate DNAT -p tcp -d %s --dport %u -j ACCEPT\n", conf->lan_ipaddr, port);
		if(conf->authtype == LE_ACME_AUTH_TLS)
			fprintf(fp, "iptables-t nat -I VSERVER -p tcp --dport %d -j DNAT --to-destination %s:%u\n", 443, conf->lan_ipaddr, port);
		else
			fprintf(fp, "iptables-t nat -I VSERVER -p tcp --dport %d -j DNAT --to-destination %s:%u\n", 80, conf->lan_ipaddr, port);
		fclose(fp);
		chmod("/tmp/.le/le-fw.sh", 0700);
		eval("/tmp/.le/le-fw.sh");
	}
}

static void _le_del_fw_rule(void)
{
	if(f_exists("/tmp/.le/le-fw.sh")){
		eval("sed", "-i", "s/-I/-D/", "/tmp/.le/le-fw.sh");
		eval("/tmp/.le/le-fw.sh");
		unlink("/tmp/.le/le-fw.sh");
	}
}

int start_letsencrypt(void)
{
	FILE *fp = NULL;
	char path[256];
	le_conf_t conf;
	le_auxsts_t status;
	char *le_argv[] = { "le_acme", NULL };
	pid_t pid;
	update_le_sts(LE_STS_CHECKING);
	update_le_sbsts(LE_SB_NONE);
	update_le_auxsts(LE_AUX_NONE);
	memset(&conf, 0, sizeof(conf));
	get_le_conf(&conf);
	if(check_le_configure(&conf) == 0){
		if(!d_exists("/tmp/.le"))
			mkdir("/tmp/.le", 0600);
		if(conf.authtype == LE_ACME_AUTH_HTTP){
			snprintf(path, sizeof(path), "%s/www/.well-known/acme-challenge", "/tmp/.le");
			if(!d_exists(path)){
				snprintf(path, sizeof(path), "%s/www", "/tmp/.le");
				mkdir(path, 0600);
				strlcat(path, "/.well-known", sizeof(path));
				mkdir(path, 0600);
				strlcat(path, "/acme-challenge", sizeof(path));
				mkdir(path, 0600);
			}
		}
		if(!d_exists(LE_ACME_CERT_HOME))
			mkdir(LE_ACME_CERT_HOME, 0600);
		snprintf(path, sizeof(path), "%s/%s%s", LE_ACME_CERT_HOME, conf.ddns_hostname, conf.ecc ? "_ecc" : "");
		if(!d_exists(path))
			mkdir(path, 0600);
		if(!d_exists("/var/empty"))
			mkdir("/var/empty", 0600);
		status = check_le_status();
		update_le_auxsts(status);
		if(status){
			_le_jobs_install(5, conf.ddns_hostname);
			update_le_sts(LE_STS_WAIT_RETRY);
			if(get_path_le_domain_cert(path, sizeof(path)) && is_le_cert(path)){
				nvram_unset("le_retry");
				update_le_st(LE_ST_ISSUED);
			}
		}else{
			_dprintf("[%s(%d)] #### Execute le_acme ####\n", __func__, __LINE__);
			_eval(le_argv, NULL, 0, &pid);
			fp = fopen("/var/run/letsencrypt.pid", "w");
			if(fp){
				fprintf(fp, "%u", pid);
				fclose(fp);
			}
		}
	}
	return 0;
}

void _le_jobs_remove(void)
{
	eval("/usr/sbin/cru", "d", "LetsEncrypt");
}

int stop_letsencrypt(void)
{
	update_le_sts(LE_STS_STOPPING);
	kill_pidfile_s_rm("/var/run/letsencrypt.pid", SIGTERM, 1);
	_le_jobs_remove();
	update_le_sts(LE_STS_STOPPED);
	return 0;
}

static void _le_acme_term(int no)
{
	int n;
	le_acme_exit = 1;
	if (killall("/usr/sbin/acme.sh", SIGTERM) == 0) {
		n = 10;
		while ((killall("/usr/sbin/acme.sh", 0) == 0) && (n-- > 0)) {
			_dprintf("%s: waiting name=%s n=%d\n", __FUNCTION__, "/usr/sbin/acme.sh", n);
			sleep(1);
		}
		if (n < 0) {
			n = 10;
			while ((killall("/usr/sbin/acme.sh", SIGKILL) == 0) && (n-- > 0)) {
				_dprintf("%s: SIGKILL name=%s n=%d\n", __FUNCTION__, "/usr/sbin/acme.sh", n);
				sleep(1);
			}
		}
	}
}

static int _le_acme_need_revoke(le_conf_t *conf, char *path, size_t len)
{
	int ret = 0, ecc;
	char tmp[128];
	DIR *dir;
	struct dirent *dirent;
	memset(tmp, 0, sizeof(tmp));
	dir = opendir(LE_ACME_CERT_HOME);
	if(dir){
		while((dirent = readdir(dir)) != NULL){
			if((dirent->d_type == DT_DIR || d_exists(dirent->d_name)) && strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, "..") 
				&& strcmp(dirent->d_name, conf->ddns_hostname)){
				ecc = is_ecc(dirent->d_name, tmp, sizeof(tmp));
				if(!strcmp(tmp, conf->ddns_hostname)){
					if(ecc != conf->ecc){
						_dprintf("[%s(%d)] conf->ecc:[%d] ecc:[%d] host:[%s] ddns_host:[%s]\n", __func__, __LINE__, conf->ecc, ecc, tmp, conf->ddns_hostname);
						ret = 1;
						update_le_st(LE_ST_NONE);
						break;
					}
				}else{
					snprintf(path, len, "%s/%s/%s.cer", LE_ACME_CERT_HOME, dirent->d_name, dirent->d_name);
					if(is_le_cert(path)){
						ret = 1;
						update_le_st(LE_ST_NONE);
						break;
					}
					snprintf(path, len, "%s/%s", LE_ACME_CERT_HOME, dirent->d_name);
					_dprintf("[%s(%d)] rm :[%s]\n", __func__, __LINE__, path);
					eval("rm", "-rf", path);
				}
			}
		}
		closedir(dir);
	}
	return ret;
}

static void _le_acme_do(int action, le_conf_t *conf)
{
	int idx = 19;
	char acckey[256] = {0}, accconf[256] = {0}, fullchain[256] = {0}, domainkey[256] = {0}, revoke_hostname[256] = {0}, cmd[512];
	char *le_argv[] = { "/usr/sbin/acme.sh", "--home", "/tmp/.le", "--certhome", LE_ACME_CERT_HOME, "--accountkey", acckey, "--accountconf", accconf,
			"--domain", revoke_hostname, "--useragent", "asusrouter/0.2", "--fullchain-file", fullchain, "--key-file", domainkey, "--dnssleep", "10",
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	snprintf(acckey, sizeof(acckey), "%s/%s", LE_ACME_CERT_HOME, LE_ACME_ACC_KEY);
	snprintf(accconf, sizeof(accconf), "%s/%s", LE_ACME_CERT_HOME, LE_ACME_ACC_CONF);
	snprintf(fullchain, sizeof(fullchain), "%s/%s%s/%s", LE_ACME_CERT_HOME, conf->ddns_hostname, conf->ecc ? "_ecc" : "", LE_ACME_DOMAIN_FULLCHAIN);
	snprintf(domainkey, sizeof(domainkey), "%s/%s%s/%s", LE_ACME_CERT_HOME, conf->ddns_hostname, conf->ecc ? "_ecc" : "", LE_ACME_DOMAIN_KEY);
	if(action == 2){
		snprintf(fullchain, sizeof(fullchain), "%s/%s%s/%s", LE_ACME_CERT_HOME, conf->ddns_hostname, conf->re_ecc ? "_ecc" : "", LE_ACME_DOMAIN_FULLCHAIN);
		snprintf(domainkey, sizeof(domainkey), "%s/%s%s/%s", LE_ACME_CERT_HOME, conf->ddns_hostname, conf->re_ecc ? "_ecc" : "", LE_ACME_DOMAIN_KEY);
		snprintf(revoke_hostname, sizeof(revoke_hostname), "%s", conf->re_hostname);
		_dprintf("%s %d %s\n", __func__, action, conf->re_hostname);
		le_argv[idx] = "--revoke";
		idx++;
		if(conf->re_ecc){
			le_argv[idx] = "--ecc";
			idx++;
		}
	}else{
		snprintf(revoke_hostname, sizeof(revoke_hostname), "%s", conf->ddns_hostname);
		_dprintf("%s %d %s\n", __func__, action, conf->ddns_hostname);
		if(action){
			snprintf(cmd, sizeof(cmd), "sed -i '/Le_HTTPPort=/d' %s/%s%s/%s.conf", LE_ACME_CERT_HOME, conf->ddns_hostname, conf->ecc ? "_ecc" : "", conf->ddns_hostname);
			system(cmd);
			le_argv[idx] = "--renew";
			if(conf->ecc){
				idx++;
				le_argv[idx] = "--ecc";
				idx++;
			}
		}else{
			le_argv[idx] = "--issue";
			idx++;
			if(conf->ecc){
				le_argv[idx] = "--keylength";
				idx++;
				le_argv[idx] = "ec-256";
				idx++;
			}
		}
		if(conf->authtype == LE_ACME_AUTH_DNS){
			le_argv[idx] = "--dns";
			idx++;
			if(nvram_match("ddns_server_x", "WWW.ASUS.COM")){
				le_argv[idx] = "dns_asusapi";
				idx++;
			}else if(nvram_match("ddns_server_x", "WWW.CLOUDFLARE.COM")){
				le_argv[idx] = "dns_cf";
				idx++;
			}
		}else if(conf->authtype == LE_ACME_AUTH_TLS){
			snprintf(cmd, sizeof(cmd), "%u", conf->port);
			le_argv[idx] = "--alpn";
			idx++;
			le_argv[idx] = "--tlsport";
			idx++;
			le_argv[idx] = cmd;
			idx++;
			_dprintf("%s challenge with tls port %s\n", __func__, cmd);
		}else if(conf->authtype == LE_ACME_AUTH_HTTP){
			snprintf(cmd, sizeof(cmd), "%u", conf->port);
			le_argv[idx] = "--standalone";
			idx++;
			le_argv[idx] = "--httpport";
			idx++;
			le_argv[idx] = cmd;
			idx++;
			_dprintf("%s challenge with http port %s\n", __func__, cmd);
		}
	}
	if(conf->debug > 0){
		le_argv[idx] = "--debug";
		idx++;
		le_argv[idx] = nvram_safe_get("le_acme_debug");
		idx++;
	}
	if(conf->force || conf->renew_force)
		le_argv[idx++] = "--force";
	if(conf->staging)
		le_argv[idx] = "--staging";
  	idx = 0;
	_dprintf("\n=====\n");
	while(le_argv[idx]){
		_dprintf("%s ", le_argv[idx]);
		idx++;
	}
	_dprintf("\n=====\n");
	if(conf->acme_logpath[0])
		_eval(le_argv, conf->acme_logpath, 0, NULL);
	else
		_eval(le_argv, "/dev/kmsg", 0, NULL);
}

static int _le_acme_do_revoke(le_conf_t *conf, char *path, size_t len)
{
	int ecc;
	DIR *dir;
	char tmp[128];
	struct dirent *dirent;
	memset(tmp, 0, sizeof(tmp));
	dir = opendir(LE_ACME_CERT_HOME);
	if(dir){
		while((dirent = readdir(dir)) != NULL){
			if((dirent->d_type == DT_DIR || d_exists(dirent->d_name)) && strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, "..")){
				ecc = is_ecc(dirent->d_name, tmp, sizeof(tmp));
				snprintf(path, len, "%s/%s/%s.cer", LE_ACME_CERT_HOME, dirent->d_name, tmp);
				if(strcmp(tmp, conf->ddns_hostname) || conf->force){
					if(f_exists(path)){
						conf->re_ecc = ecc;
						snprintf(conf->re_hostname, sizeof(conf->re_hostname), "%s", tmp);
						_le_acme_do(2, conf);
						snprintf(path, len, "%s/%s", LE_ACME_CERT_HOME, dirent->d_name);
						_dprintf("[%s(%d)] rm :[%s]\n", __func__, __LINE__, path);
					}else{
						snprintf(path, len, "%s/%s", LE_ACME_CERT_HOME, dirent->d_name);
						_dprintf("[%s(%d)] rm :[%s]\n", __func__, __LINE__, path);
					}
					eval("rm", "-rf", path);
				}else if(ecc != conf->ecc){
					conf->re_ecc = ecc;
					snprintf(conf->re_hostname, sizeof(conf->re_hostname), "%s", tmp);
					_le_acme_do(2, conf);
					_dprintf("[%s(%d)][ECC Different] rm :[%s]\n", __func__, __LINE__, LE_ACME_CERT_HOME);
					eval("rm", "-rf", LE_ACME_CERT_HOME);
					break;
				}
			}
		}
		closedir(dir);
	}
	return 0;
}

static int expire_date(char *path, size_t len)
{
	FILE *fp;
	X509 *x509data = NULL;
	struct tm tm;
	fp = fopen(path, "r");
	if(fp == NULL)
		return 1;
	if(PEM_read_X509(fp, &x509data, NULL, NULL) == NULL){
		fclose(fp);
		return 1;
	}
	fclose(fp);
	ASN1_TimeToTM(X509_getm_notAfter(x509data), &tm);
	_dprintf("Expire Date: %s\n", asctime(&tm));
	if(x509data)
		X509_free(x509data);
	if((timegm(&tm) - 864000 - 1) <= time(NULL))//60*60*24*10
		return 1;
	return 0;
}

static void _le_acme_need_issue(char *path, size_t len, le_conf_t *conf)
{
	snprintf(path, len, "%s/%s", LE_ACME_CERT_HOME, conf->ddns_hostname);
	_dprintf("[%s(%d)] rm :[%s]\n", __func__, __LINE__, path);
	eval("rm", "-rf", path);
}

static void _le_acme_result_handler(char *path, size_t len, le_conf_t *conf)
{
	_dprintf("[%s(%d)] #### ACME ERROR ####\n", __func__, __LINE__);
	update_le_auxsts(LE_AUX_ACME);
	if(f_read_string("/tmp/acme_err", path, len) && strstr(path, "firewall"))
		update_le_sbsts(LE_SB_CHA_FW);
	_le_jobs_install(5, conf->ddns_hostname);
	update_le_sts(LE_STS_WAIT_RETRY);
}

static void _le_acme_do_issue(le_conf_t *conf)
{
	conf->port = pick_random_port();
	if(conf->port == 0)
		_dprintf("%s: pick random port failed", __func__);
	else{
		_le_add_fw_rule(conf, conf->port);
		_le_acme_do(0, conf);
		_le_del_fw_rule();
	}
}

static void _le_acme_do_renew(le_conf_t *conf)
{
	conf->port = pick_random_port();
	if(conf->port == 0)
		_dprintf("%s: pick random port failed", __func__);
	else{
		_le_add_fw_rule(conf, conf->port);
		_le_acme_do(1, conf);
		_le_del_fw_rule();
	}
}

int le_acme_main(int argc, char **argv)
{
	char path[256], domain_cert[256], domain_key[256];
	le_conf_t conf;

	memset(&conf, 0, sizeof(conf));
	get_le_conf(&conf);
	le_acme_exit = 0;
	signal(SIGTERM, _le_acme_term);
	if(le_acme_exit == 0){
		memset(path, 0, sizeof(path));
		if(_le_acme_need_revoke(&conf, path, sizeof(path)) || conf.force){
			update_le_sts(LE_STS_REVOKE);
			_dprintf("[%s(%d)] #### need revokation ####\n", __func__, __LINE__);
			memset(path, 0, sizeof(path));
			_le_acme_do_revoke(&conf, path, sizeof(path));
		}
	}
	if(le_acme_exit == 0){
		memset(domain_cert, 0, sizeof(domain_cert));
		memset(domain_key, 0, sizeof(domain_key));
		memset(path, 0, sizeof(path));
		get_path_le_domain_cert(domain_cert, sizeof(domain_cert));
		get_path_le_domain_key(domain_key, sizeof(domain_key));
		if(is_le_cert(domain_cert) && cert_key_match(domain_cert, domain_key)){
			nvram_unset("le_retry");
			update_le_st(LE_ST_ISSUED);
			if(conf.force == 0){
				memset(domain_cert, 0, sizeof(domain_cert));
				memset(path, 0, sizeof(path));
				get_path_le_domain_cert(path, sizeof(path));
				if(expire_date(path, sizeof(path)) || conf.renew_force){
					update_le_sts(LE_STS_RENEW);
					_dprintf("[%s(%d)] #### need renew ####\n", __func__, __LINE__);
					if(conf.authtype == LE_ACME_AUTH_DNS){
						_le_acme_do(1, &conf);
					}else
						_le_acme_do_renew(&conf);
				}
			}else{
				update_le_sts(LE_STS_ISSUE);
				_dprintf("[%s(%d)] #### need issue ####\n", __func__, __LINE__);
				if(conf.authtype == LE_ACME_AUTH_DNS)
					_le_acme_do(0, &conf);
				else if(conf.authtype == LE_ACME_AUTH_HTTP)
					_le_acme_do_issue(&conf);
			}
		}else{
			_le_acme_need_issue(path, sizeof(path), &conf);
			update_le_sts(LE_STS_ISSUE);
			_dprintf("[%s(%d)] #### need issue ####\n", __func__, __LINE__);
			if(conf.authtype == LE_ACME_AUTH_DNS)
				_le_acme_do(0, &conf);
			else if(conf.authtype == LE_ACME_AUTH_HTTP)
				_le_acme_do_issue(&conf);
		}
		if(le_acme_exit == 0){
			memset(domain_cert, 0, sizeof(domain_cert));
			memset(domain_key, 0, sizeof(domain_key));
			_le_jobs_remove();
			get_path_le_domain_cert(domain_cert, sizeof(domain_cert));
			if(is_le_cert(domain_cert)){
				nvram_unset("le_retry");
				update_le_st(LE_ST_ISSUED);
				update_le_sts(LE_STS_WAIT_RENEW);
				update_le_auxsts(LE_AUX_NONE);
				get_path_le_domain_fullchain(path, sizeof(path));
				if(f_diff("/etc/cert.pem", path) && !cp_le_cert(LE_FULLCHAIN, "/etc/cert.pem") && !cp_le_cert(LE_KEY, "/etc/key.pem")){
					if(conf.https_enabled){
						nvram_set("le_restart_httpd", "1");
						sleep(6);
						notify_rc("restart_httpd");
					}
					if(conf.webdav_enabled)
						notify_rc("restart_webdav");
				}
			}
			_le_jobs_install(0, conf.ddns_hostname);
		}else
			_le_acme_result_handler(path, sizeof(path), &conf);
		eval("rm", "-rf", "/tmp/.le");
	}
	return 0;
}

int cp_le_cert(int type, char* dst_path)
{
	char path[256];
	memset(path, 0, sizeof(path));

	if(type == LE_CERT)
		get_path_le_domain_cert(path, sizeof(path));
	else if(type == LE_FULLCHAIN)
		get_path_le_domain_fullchain(path, sizeof(path));
	else if(type == LE_KEY)
    	get_path_le_domain_key(path, sizeof(path));
	else
		return -1;
	eval("cp", "-af", path, dst_path);
	return 0;
}

int is_le_cert(const char *cert_path)
{
	int ret = 0;
	FILE *fp;
	char buf[256] = {0}, subject[64];
	X509 *x509data = NULL;
	le_conf_t conf;
	if(!f_exists(cert_path))
		return 0;
	fp = fopen(cert_path, "r");
	if(fp == NULL)
		return 0;
	if(!PEM_read_X509(fp, &x509data, NULL, NULL)){
		fseek(fp, 0, SEEK_SET);
		d2i_X509_fp(fp, &x509data);
	}
	fclose(fp);
	if(x509data == NULL)
		return 0;
	get_le_conf(&conf);
	X509_NAME_oneline(X509_get_issuer_name(x509data), buf, sizeof(buf));
	_dprintf("[Issuer Info]: %s\n", buf);
	if(strstr(buf, "Let's Encrypt"))
		ret = 1;
	else if(conf.staging && strstr(buf, "Fake LE Intermediate"))
		ret = 1;
	X509_NAME_oneline(X509_get_subject_name(x509data), buf, sizeof(buf));
	_dprintf("[Subject Info]: %s\n", buf);
	parse_cert_cn(buf, subject, sizeof(subject));
	_dprintf("subject: %s\n", subject);
	if(strcasecmp(conf.ddns_hostname, subject) && ret == 1)
		ret = 2;
	X509_free(x509data);
	if(ret != 1)
		_dprintf("[Error] Issuer or Subject not matched.\n");
	return ret;
}

int cert_key_match(const char *cert_path, const char *key_path)
{
	int ret = 0, pem = 1;
	FILE *fp;
	X509 *x509data = NULL;
	EVP_PKEY *pkey = NULL;
	RSA *rsa_pub = NULL;
	RSA *rsa_pri = NULL;
	DSA *dsa_pub = NULL;
	DSA *dsa_pri = NULL;
	EC_KEY *ec_pub = NULL;
	EC_KEY *ec_pri = NULL;
	if(!f_exists(cert_path) || !f_exists(key_path))
		return 0;
	//get x509 from cert file
	fp = fopen(cert_path, "r");
	if(!fp)
		return 0;
	if(!PEM_read_X509(fp, &x509data, NULL, NULL))
	{
		_dprintf("Try to read DER format certificate\n");
		pem = 0;
		fseek(fp, 0, SEEK_SET);
		d2i_X509_fp(fp, &x509data);
	}
	else
	{
		_dprintf("PEM format certificate\n");
	}
	fclose(fp);
	if(x509data == NULL)
	{
		_dprintf("Load certificate failed\n");
		ret = 0;
		goto end;
	}
	//get pubic key from x509
	pkey = X509_get_pubkey(x509data);
	if(pkey == NULL)
	{
		ret = 0;
		goto end;
	}
	X509_free(x509data);
	x509data = NULL;

	if(EVP_PKEY_id(pkey) == EVP_PKEY_RSA)
	{
		//_dprintf("RSA public key\n");
		rsa_pub = EVP_PKEY_get1_RSA(pkey);
	}
	else if(EVP_PKEY_id(pkey) == EVP_PKEY_DSA)
	{
		//_dprintf("DSA public key\n");
		dsa_pub = EVP_PKEY_get1_DSA(pkey);
	}
	else if(EVP_PKEY_id(pkey) == EVP_PKEY_EC)
	{
		//_dprintf("EC public key\n");
		ec_pub = EVP_PKEY_get1_EC_KEY(pkey);
	}
	EVP_PKEY_free(pkey);
	pkey = NULL;
	//get private key from key file
	fp = fopen(key_path, "r");
	if(!fp)
	{
		ret = 0;
		goto end;
	}
	if(pem)
	{
		//_dprintf("PEM format private key\n");
		pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
	}
	else
	{
		//_dprintf("DER format private key\n");
		pkey = d2i_PrivateKey_fp(fp, NULL);
	}
	fclose(fp);
	if(pkey == NULL)
	{
		_dprintf("Load private key failed\n");
		ret = 0;
		goto end;
	}
	if(EVP_PKEY_id(pkey) == EVP_PKEY_RSA)
	{
		//_dprintf("RSA private key\n");
		rsa_pri = EVP_PKEY_get1_RSA(pkey);
	}
	else if(EVP_PKEY_id(pkey) == EVP_PKEY_DSA)
	{
		//_dprintf("DSA private key\n");
		dsa_pri = EVP_PKEY_get1_DSA(pkey);
	}
	else if(EVP_PKEY_id(pkey) == EVP_PKEY_EC)
	{
		//_dprintf("EC private key\n");
		ec_pri = EVP_PKEY_get1_EC_KEY(pkey);
	}
	EVP_PKEY_free(pkey);
	pkey = NULL;
	//compare modulus
	if(rsa_pub && rsa_pri)
	{
		if(BN_cmp(RSA_get0_n(rsa_pub), RSA_get0_n(rsa_pri)))
		{
			_dprintf("rsa n not match\n");
			ret = 0;
		}
		else
		{
			_dprintf("rsa n match\n");
			ret = 1;
		}
	}
	else if(dsa_pub && dsa_pri)
	{
		if(BN_cmp(DSA_get0_pub_key(dsa_pub), DSA_get0_pub_key(dsa_pri)))
		{
			_dprintf("dsa modulus not match\n");
			ret = 0;
		}
		else
		{
			_dprintf("dsa modulus match\n");
			ret = 1;
		}
	}
	else if(ec_pub && ec_pri)
	{
		if(EC_POINT_cmp(EC_KEY_get0_group(ec_pub), EC_KEY_get0_public_key(ec_pub), EC_KEY_get0_public_key(ec_pri), NULL))
		{
			_dprintf("[mssl] ec modulus not match\n");
			ret = 0;
		}
		else
		{
			_dprintf("[mssl] ec modulus match\n");
			ret = 1;
		}
	}
	else
	{
		_dprintf("compare failed");
	}

end:
	if(x509data)
		X509_free(x509data);
	if(pkey)
		EVP_PKEY_free(pkey);
	if(rsa_pub)
		RSA_free(rsa_pub);
	if(dsa_pub)
		DSA_free(dsa_pub);
	if(ec_pub)
		EC_KEY_free(ec_pub);
	if(rsa_pri)
		RSA_free(rsa_pri);
	if(dsa_pri)
		DSA_free(dsa_pri);
	if(ec_pri)
		EC_KEY_free(ec_pri);

	return ret;
}

void run_le_fw_script(void)
{
	if(f_exists("/tmp/.le/le-fw.sh"))
		eval("/tmp/.le/le-fw.sh");
}
