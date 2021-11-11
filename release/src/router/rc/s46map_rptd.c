#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <wlutils.h>
#include <shutils.h>
#include <shared.h>
#include <wlioctl.h>
#include <rc.h>
#include <curl/curl.h>
#include <json.h>

#define JPNE_URL   "https://api.enabler.ne.jp/4027bf2bfdefca6d54b7650fbdeb41d0/"

static int CURRENT_STATE = S46_MAPSVR_INIT;
static int FORCE_TRIGGER = 1;
static int HGW_STATE_CHK = 1;
static int HGW_DHCP_WAIT = 0;
static int DEF_INTERVAL = 30;
static struct itimerval tick;

static void do_map_check(void);

/* safely check nvram value
 * returns:
 *  0 if not equal
 *  1 if equal */
static int
_nvram_check(const char *name, const char *value)
{
	char *nvalue = nvram_get(name);

	return (nvalue == value || strcmp(nvalue ? : "", value ? : "") == 0);
}

/* set nvram to value
 * returns:
 *  0 if not changed
 *  1 if new/changed/removed */
static int
_nvram_set_check(const char *name, const char *value)
{
	if (_nvram_check(name, value))
		return 0;

	nvram_set(name, value);
	return 1;
}

int check_s46map_rptd()
{
	if(pidof("s46map_rptd") == -1) {
		unlink("/var/run/s46map_rptd.pid");
		notify_rc("restart_s46map_rptd");
		return 1;
	}
	return 0;
}

static int getRandom(int min, int max)
{
	srand(time(0));
	return (rand() % (max - min + 1)) + min;
}

static int GetExeTime(int status)
{
	int t_unit;

	t_unit = nvram_get_int("s46map_rptd_debug");

	if (S46_MAPSVR_DATA_INVALID == status) {
		// 10-30 min
		if (!t_unit)
			return getRandom(10*60, 30*60);
		else //debug
			return getRandom(10*t_unit, 30*t_unit);
	} else if (S46_MAPSVR_ERR == status ||
		   S46_MAPSVR_OK  == status) {
		// 3-24 hr
		if (!t_unit)
			return getRandom(3*60*60, 24*60*60);
		else //debug
			return getRandom(3*t_unit, 24*t_unit);
	} else if (S46_MAPSVR_INIT == status ||
		   S46_MAPSVR_NO_RESPONSE == status) {
		// 1-10 min
		if (!t_unit)
			return getRandom(1*60, 10*60);
		else //debug
			return getRandom(1*t_unit, 10*t_unit);
	} else
		return 0;
}

static void setalarm_t(unsigned long sec, unsigned long usec)
{
	//Timeout to run first time
	tick.it_value.tv_sec = sec;
	tick.it_value.tv_usec = usec;

	//After first, the Interval time for clock
	tick.it_interval = tick.it_value;
	if (setitimer(ITIMER_REAL, &tick, NULL) < 0)
		_dprintf("[%s(%d)][Err] set alarm timer failed!\n", __FUNCTION__, __LINE__);
	else
		_dprintf("[%s(%d)][ALRM TIME] alarm set after %d sec\n", __FUNCTION__, __LINE__, sec);
}

static void handlesignal(int signum)
{
	if (signum == SIGUSR1) {
		_dprintf("[%s(%d)][Get SIGUSR1]\n", __FUNCTION__, __LINE__);
		FORCE_TRIGGER = 1;
		setalarm_t(1, 0);
	} else if (signum == SIGUSR2) {
		_dprintf("[%s(%d)][DEBUG][Get SIGUSR2]\n", __FUNCTION__, __LINE__);
		setalarm_t(1, 0);
	} else if (signum == SIGALRM) {
		do_map_check();
	} else if (signum == SIGTERM) {
		setalarm_t(0, 0);
		remove("/var/run/s46map_rptd.pid");
		nvram_set_int("s46_mapsvr_state", S46_MAPSVR_INIT);
		_dprintf("[%s(%d)]Exit!!!\n", __FUNCTION__, __LINE__);
		exit(0);
	} else
		_dprintf("Unknown SIGNAL\n");
}

static void signal_register(void) {

	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler =  &handlesignal;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGALRM, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

void restart_wan_if(void)
{
	char buf[32];
	snprintf(buf, sizeof(buf), "restart_wan_if %d", 0);
	notify_rc_and_wait(buf);
}

int s46_jpne_hgw(void)
{
	CURL *curl;
	CURLcode res;
	json_object *json, *obj;
	char *buf, *p, *s;
	size_t bufsz;
	FILE *fp;
	int ret = -1;
	long http_code = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl) {
		if (asprintf(&buf, "http://ntt.setup:8888/enabler.ipv4/check?callback=%s", "hgw") < 0)
			goto cleanup;

		_dprintf("[%s(%d)][JPNE_HGW_URL]%s\n", __FUNCTION__, __LINE__, buf);
		curl_easy_setopt(curl, CURLOPT_URL, buf);
		free(buf);

		fp = open_memstream(&buf, &bufsz);
		if (fp == NULL)
			goto cleanup;

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		_dprintf("[%s(%d)][HTTP_RESPONSE_CODE]:%d\n", __FUNCTION__, __LINE__, http_code);
		fclose(fp);

		json = NULL;
		if (res == CURLE_OK) {
			for (p = buf; (p = s = strstr(p, "hgw"));) {
				p += sizeof("hgw") - 1;
				if (s == buf || isspace(s[-1])) {
					while (*p && isspace(*p)) p++;
					if (*p == '(') {
						*strchrnul(++p, ')') = '\0';
						json = json_tokener_parse(p);
						break;
					}
				}
			}
		} else {
			_dprintf("[%s(%d)][Err] curl_easy_perform() failed: %s\n", __FUNCTION__, __LINE__, curl_easy_strerror(res));
			goto cleanup;
		}
		_dprintf("[%s(%d)][JSON]%s\n", __FUNCTION__, __LINE__, json_object_to_json_string(json));
		if (json) {
			if (json_object_object_get_ex(json, "name", &obj))
				nvram_set("s46_hgw_name", json_object_get_string(obj));
			if (json_object_object_get_ex(json, "version", &obj))
				nvram_set("s46_hgw_ver", json_object_get_string(obj));
			if (json_object_object_get_ex(json, "status", &obj)) {
				if (!strcmp(json_object_get_string(obj), "ON"))
					ret = 1;
				else if (!strcmp(json_object_get_string(obj), "OFF"))
					ret = 0;
			}
			json_object_put(json);
		}
	cleanup:
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return ret;
}

void s46_jpne_report(char *id, int act, int reason)
{
	CURL *curl;
	CURLcode res;
	json_object *json, *obj;
	char *buf, *p, *s;
	char ret[4] = {0};
	size_t bufsz;
	FILE *fp;
	long http_code = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl) {
		if (asprintf(&buf, "%sacct_report?callback=%s&action=%d&reason=%d%s%s", JPNE_URL, "rpt",
			     act, reason,
			     (id && *id) ? "&id=" : "", (id && *id) ? id : "") < 0)
			goto cleanup;

		_dprintf("[%s(%d)][JPNE_REPLY_URL]%s\n", __FUNCTION__, __LINE__, buf);
		curl_easy_setopt(curl, CURLOPT_URL, buf);
		free(buf);

		fp = open_memstream(&buf, &bufsz);
		if (fp == NULL)
			goto cleanup;

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		_dprintf("[%s(%d)][HTTP_RESPONSE_CODE]:%d\n", __FUNCTION__, __LINE__, http_code);
		fclose(fp);

		json = NULL;
		if (res == CURLE_OK) {
			for (p = buf; (p = s = strstr(p, "rpt"));) {
				p += sizeof("rpt") - 1;
				if (s == buf || isspace(s[-1])) {
					while (*p && isspace(*p)) p++;
					if (*p == '(') {
						*strchrnul(++p, ')') = '\0';
						json = json_tokener_parse(p);
						break;
					}
				}
			}
		} else
			goto cleanup;
		_dprintf("[%s(%d)][JSON]%s\n", __FUNCTION__, __LINE__, json_object_to_json_string(json));
		if (json) {
			if (json_object_object_get_ex(json, "status", &obj))
				strlcpy(ret, json_object_get_string(obj), sizeof(ret));
			json_object_put(json);
		}
	cleanup:
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return;
}

char *s46_jpne_maprules(char *id, char *idbuf, size_t idlen, long *rsp_code)
{
	CURL *curl;
	CURLcode res;
	json_object *json, *obj, *list_obj, *entry_obj;
	char addrbuf[INET6_ADDRSTRLEN], *buf, *p, *s, *ret = NULL;
	size_t bufsz;
	FILE *fp;
	int MapSvrState = S46_MAPSVR_INIT;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if (curl) {
		if (asprintf(&buf, "%sget_rules?callback=%s%s%s", JPNE_URL, "v6plus",
			     (id && *id) ? "&id=" : "", (id && *id) ? id : "") < 0)
			goto cleanup;

		_dprintf("[%s(%d)][GET_JPNE_URL]%s\n", __FUNCTION__, __LINE__, buf);
		curl_easy_setopt(curl, CURLOPT_URL, buf);
		free(buf);

		fp = open_memstream(&buf, &bufsz);
		if (fp == NULL)
			goto cleanup;

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, rsp_code);
		_dprintf("[%s(%d)][HTTP_RESPONSE_CODE]:%d\n", __FUNCTION__, __LINE__, *rsp_code);
		fclose(fp);

		if (*rsp_code >= 400 && *rsp_code <= 499) {
			MapSvrState = S46_MAPSVR_ERR;
		} else if (*rsp_code >= 500) {  /* rsp_code >= 500 or other error*/
			MapSvrState = S46_MAPSVR_NO_RESPONSE;
		}

		json = NULL;
		if (res == CURLE_OK) {
			for (p = buf; (p = s = strstr(p, "v6plus"));) {
				p += sizeof("v6plus") - 1;
				if (s == buf || isspace(s[-1])) {
					while (*p && isspace(*p)) p++;
					if (*p == '(') {
						*strchrnul(++p, ')') = '\0';
						json = json_tokener_parse(p);
						break;
					}
				}
			}
		} else {
			MapSvrState = S46_MAPSVR_NO_RESPONSE;
			_dprintf("[%s(%d)][Err] curl_easy_perform() failed: %s\n", __FUNCTION__, __LINE__, curl_easy_strerror(res));
		}
		free(buf);
		_dprintf("[%s(%d)][JSON]%s\n", __FUNCTION__, __LINE__, json_object_to_json_string(json));

		if (json) {
			const char *dmr = NULL;
			int i, len, pdlen = 0;

			fp = open_memstream(&ret, &bufsz);
			if (fp == NULL)
				goto json_put;

			if (json_object_object_get_ex(json, "id", &obj) && idbuf)
				strlcpy(idbuf, json_object_get_string(obj), idlen);
			else
				MapSvrState = S46_MAPSVR_DATA_INVALID;
			if (json_object_object_get_ex(json, "dmr", &obj))
				dmr = json_object_get_string(obj);
			else
				MapSvrState = S46_MAPSVR_DATA_INVALID;
			if (json_object_object_get_ex(json, "ipv6_fixlen", &obj))
				pdlen = json_object_get_int(obj);
			else
				MapSvrState = S46_MAPSVR_DATA_INVALID;
			if (json_object_object_get_ex(json, "fmr", &list_obj)) {
				for (i = 0; i < json_object_array_length(list_obj); i++) {
					entry_obj = json_object_array_get_idx(list_obj, i);
					if (entry_obj) {
						fprintf(fp, "fmr,type=map-e");
						if (json_object_object_get_ex(entry_obj, "ipv6", &obj) &&
						    sscanf(json_object_get_string(obj), "%45[^/]/%d", addrbuf, &len) == 2)
							fprintf(fp, ",ipv6prefix=%s,prefix6len=%d", addrbuf, len);
						if (json_object_object_get_ex(entry_obj, "ipv4", &obj) &&
						    sscanf(json_object_get_string(obj), "%15[^/]/%d", addrbuf, &len) == 2)
							fprintf(fp, ",ipv4prefix=%s,prefix4len=%d", addrbuf, len);
						if (json_object_object_get_ex(entry_obj, "psid_offset", &obj))
							fprintf(fp, ",offset=%d", json_object_get_int(obj));
						if (json_object_object_get_ex(entry_obj, "ea_length", &obj))
							fprintf(fp, ",ealen=%d", json_object_get_int(obj));
						if (dmr)
							fprintf(fp, ",br=%s", dmr);
						fprintf(fp, " ");
					}
				}
			} else
				MapSvrState = S46_MAPSVR_DATA_INVALID;
			fclose(fp);
		json_put:
			json_object_put(json);
		} else { /* json parser err */
			if (MapSvrState == S46_MAPSVR_INIT) {
				MapSvrState = S46_MAPSVR_DATA_INVALID;
			}
		}
	cleanup:
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	if (MapSvrState == S46_MAPSVR_INIT) {
		nvram_set_int("s46_mapsvr_state", S46_MAPSVR_OK);
	} else {
		nvram_set_int("s46_mapsvr_state", MapSvrState);
		idbuf = NULL;
	}

	_dprintf("[%s(%d)]s46_mapsvr_state=[%d]\n", __FUNCTION__, __LINE__, nvram_get_int("s46_mapsvr_state"));
	_dprintf("[%s(%d)]%s\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

void fmrs2file(char *path) {
	char *nv, *nvp, *item, *nextp;
	FILE *fp;
	int cnt = 0;

	if (!f_exists(path)) {
		if ((fp = fopen(path, "w"))) {
			nvp = nv = strdup(nvram_safe_get("ipv6_s46_fmrs"));
			for (item = strtok_r(nvp, " ", &nextp); item; item = strtok_r(NULL, " ", &nextp)) {
				cnt++;
				if (cnt%4 == 0)
					fprintf(fp, "%s\n", item);
				else
					fprintf(fp, "%s ", item);
			}
			free(nv);
			fclose(fp);
		}
	}
}

int s46_mapcalc_chcek(char *id, char *rules, int draft)
{
	char peerbuf[INET6_ADDRSTRLEN];
	char addr6buf[INET6_ADDRSTRLEN];
	char addr4buf[INET_ADDRSTRLEN + sizeof("/32")];
	char *fmrs, ports[512] = {0}, range[16];
	int offset, psidlen, psid;
	int k, start, end, strcnt = 0;
	int s46_changed;
	int state = 0;

	state = nvram_get_int("s46_mapsvr_state");
	if (state > S46_MAPSVR_OK) {
		_dprintf("[%s(%d)][ERR]\n", __FUNCTION__, __LINE__);
		s46_jpne_report(id, 2, 0); //stop, Normal operation
		return state;
	}

	if (s46_mapcalc(rules, peerbuf, sizeof(peerbuf), addr6buf, sizeof(addr6buf),
			addr4buf, sizeof(addr4buf), &offset, &psidlen, &psid, &fmrs, draft) <= 0) {
		peerbuf[0] = addr6buf[0] = addr4buf[0] = '\0';
		offset = 0, psidlen = 0, psid = 0;
		fmrs = NULL;
	}
	//Check have matched rule
	if (!strcmp(peerbuf, "") || !strcmp(addr6buf, "") || !strcmp(addr4buf, "")) {
		s46_jpne_report(id, 2, 3); //stop, Map rule mismatch
		_dprintf("[%s(%d)][MAP RULE MISMATCH]\n", __FUNCTION__, __LINE__);
		state = S46_MAPSVR_DATA_INVALID;
		goto end;
	} else {
		if ((nvram_invmatch(ipv6_nvname("ipv6_s46_peer"), peerbuf)    ||
		    nvram_invmatch(ipv6_nvname("ipv6_s46_addr6"), addr6buf)   ||
		    nvram_invmatch(ipv6_nvname("ipv6_s46_addr4"), addr4buf)   ||
		    nvram_get_int(ipv6_nvname("ipv6_s46_offset")) != offset   ||
		    nvram_get_int(ipv6_nvname("ipv6_s46_psidlen")) != psidlen ||
		    nvram_get_int(ipv6_nvname("ipv6_s46_psid")) != psid) &&
		    (strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_peer")), "") &&
		    strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_addr6")), "") &&
		    strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_addr4")), "")))
		{
			s46_jpne_report(id, 2, 2); //stop, Address change
			_dprintf("[%s(%d)][MAP RULE CHANGE]\n", __FUNCTION__, __LINE__);
			logmessage("[MAP-E]", "Detected map rule changed.");
		} else {
			_dprintf("[%s(%d)][Normal OK]\n", __FUNCTION__, __LINE__);
			s46_jpne_report(id, 1, 0); //start, Normal operation
		}
	}

	// Delete the last blank character
	fmrs[strlen(fmrs)-1] = '\0';
	s46_changed = _nvram_set_check(ipv6_nvname("ipv6_s46_peer"), peerbuf);
	s46_changed += _nvram_set_check(ipv6_nvname("ipv6_s46_addr6"), addr6buf);
	s46_changed += _nvram_set_check(ipv6_nvname("ipv6_s46_addr4"), addr4buf);
	s46_changed += _nvram_set_check(ipv6_nvname("ipv6_s46_fmrs"), fmrs ? : "");
	if (s46_changed || FORCE_TRIGGER) {
		nvram_set_int(ipv6_nvname("ipv6_s46_offset"), offset);
		nvram_set_int(ipv6_nvname("ipv6_s46_psidlen"), psidlen);
		nvram_set_int(ipv6_nvname("ipv6_s46_psid"), psid);
		//v6plus prot range
		start = end = 0;
		for (k = offset ? 1 : 0; k < (1 << offset); k++) {
			start = (k << (16 - offset)) | (psid << (16 - offset - psidlen));
			end = start + (1 << (16 - offset - psidlen)) - 1;
			if (start == 0)
				start = 1;
			if (start <= end) {
				snprintf(range, sizeof(range), "%d-%d ", start, end);
				strcnt = sizeof(ports) - strlen(ports);
                                strncat(ports, range, strcnt-1);
			}
		}
		ports[strlen(ports)-1] = '\0';
		nvram_set(ipv6_nvname("ipv6_s46_ports"), ports);
		_dprintf("[%s(%d)][Ports][%s]\n", __FUNCTION__, __LINE__, ports);

		_dprintf("[%s(%d)][Create s46 tunnel interface]\n", __FUNCTION__, __LINE__);
		stop_s46_tunnel(wan_primary_ifunit(), 0);
		start_s46_tunnel(wan_primary_ifunit());
	}
	state = S46_MAPSVR_OK;
end:
	free(fmrs);
	nvram_set_int("s46_mapsvr_state", state);
	return state;
}

int ce_dad_check(void)
{
	FILE *fp;
	char buf[32];
	char tmp[100];
	int  ret = 0;
	snprintf(buf, sizeof(buf), "ip addr show %s | grep dadfailed  2>/dev/null", nvram_safe_get("wan_ifname"));
	if ((fp = popen(buf, "r")) != NULL) {
		while(fgets(tmp, sizeof(tmp), fp) != NULL) {
			if (strlen(tmp) > sizeof("dadfailed"))
				ret = 1;
		}
		pclose(fp);
	}
	return ret;
}

void do_map_check(void)
{
	FILE *fp;
	int interval_t = DEF_INTERVAL;
	int state = 0;
	int size;
	long rsp_code = 0;
	char *rules, *rulebuf;
	char maprule_id[64] = {0};
	char prefix[sizeof("wanXXXXXXXXXX_")];
	char wan_ifname[16];
	char tmp[100];
	char addr[INET6_ADDRSTRLEN + 1];
	char cmdbuf[256];

	if (HGW_STATE_CHK) {
		if (nvram_get_int("s46_hgw_case") == S46_CASE_INIT) {
			setalarm_t(5, 0);
			if (HGW_DHCP_WAIT == 1) {
				logmessage("[MAP-E]", "Checking..., takes about 30 sec.");
			}
			if (HGW_DHCP_WAIT == 6) {
				setalarm_t(DEF_INTERVAL, 0);
				HGW_DHCP_WAIT = 0;
				HGW_STATE_CHK = 0;
				nvram_set_int("s46_hgw_case", S46_CASE_MAP_CE_ON);
				snprintf(prefix, sizeof(prefix), "wan%d_", wan_primary_ifunit());
				snprintf(wan_ifname, sizeof(wan_ifname), "%s", nvram_safe_get(strcat_r(prefix, "ifname", tmp)));
				_dprintf("[%s(%d)] ### HGW OFF ###\n", __FUNCTION__, __LINE__);
				logmessage("[MAP-E]", "v6plus starts.");
				wan6_up(wan_ifname);
				return;
			}
			HGW_DHCP_WAIT +=1;
			return;
		} else if (nvram_get_int("s46_hgw_case") == S46_CASE_MAP_HGW_OFF) {
			setalarm_t(15, 0);
			HGW_STATE_CHK = 0;
			snprintf(prefix, sizeof(prefix), "wan%d_", wan_primary_ifunit());
			snprintf(wan_ifname, sizeof(wan_ifname), "%s", nvram_safe_get(strcat_r(prefix, "ifname", tmp)));
			_dprintf("[%s(%d)] ### HGW OFF ###\n", __FUNCTION__, __LINE__);
			logmessage("[MAP-E]", "v6plus starts.(HGW v6plus is not activated)");
			wan6_up(wan_ifname);
			//FIXME
			sleep(2);
			snprintf(cmdbuf, sizeof(cmdbuf), "ip a s %s | grep \"scope global mngtmpaddr dynamic\" | awk -F \" \" '{printf $2}' 2>/dev/null", wan_ifname);
			 _dprintf("[%s(%d)]cmd:[%s]\n", __FUNCTION__, __LINE__, cmdbuf);
			if ((fp = popen(cmdbuf, "r")) != NULL) {
				if (fscanf(fp, "%[^/]/%d", addr, &size) == 2) {
					nvram_set(ipv6_nvname("ipv6_ra_addr"), addr);
					nvram_set_int(ipv6_nvname("ipv6_ra_length"), size);
					_dprintf("[%s(%d)]ipv6_ra_addr:[%s/%d]\n", __FUNCTION__, __LINE__, addr, size);
				} else {
					nvram_unset(ipv6_nvname("ipv6_ra_addr"));
					nvram_unset(ipv6_nvname("ipv6_ra_length"));
					_dprintf("[%s(%d)]ipv6_ra_addr:[NULL]\n", __FUNCTION__, __LINE__);
				}
				pclose(fp);
			}
			return;
		} else if (nvram_get_int("s46_hgw_case") == S46_CASE_MAP_HGW_ON) {
			_dprintf("[%s(%d)] ### HGW ON ###\n", __FUNCTION__, __LINE__);
			logmessage("[MAP-E]", "v6plus is active on HGW");
			s46_jpne_report(NULL, 2, 4); //stop, HGW
			stop_s46map_rptd();
			return;
		}
	}

	if (FORCE_TRIGGER) {
		CURRENT_STATE = S46_MAPSVR_INIT;
		_dprintf("[%s(%d)] ### Current State Force Change: [%d] ###\n", __FUNCTION__, __LINE__, CURRENT_STATE);
		nvram_set_int("s46_mapsvr_state", S46_MAPSVR_INIT);
		if (strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_fmrs")), "")) {
			interval_t = GetExeTime(S46_MAPSVR_INIT);
			logmessage("[MAP-E]", "The map rule has exist. Reconfirm after %d min.", interval_t/60);
			_dprintf("[%s(%d)][MAP rule has exist, create s46 tunnel interface.]\n", __FUNCTION__, __LINE__);
			fmrs2file("/tmp/v6maps");
			stop_s46_tunnel(wan_primary_ifunit(), 0);
			start_s46_tunnel(wan_primary_ifunit());
			goto setime;
		}
	} else {
		_dprintf("[%s(%d)] ### Current State: [%d] ###\n", __FUNCTION__, __LINE__, CURRENT_STATE);
	}

	rules = rulebuf = s46_jpne_maprules(nvram_safe_get("s46_mapsvr_id"), maprule_id, sizeof(maprule_id), &rsp_code);
	if (nvram_match("s46_mapsvr_id", "")) {
		nvram_set("s46_mapsvr_id", maprule_id);
		_dprintf("[%s(%d)]Save ID:[%s]\n", __FUNCTION__, __LINE__, maprule_id);
	}
	state = s46_mapcalc_chcek(maprule_id, rules, 1);

	if (state == S46_MAPSVR_INIT) {
		if (CURRENT_STATE != S46_MAPSVR_INIT) {
			_dprintf("[%s(%d)]Current State: [%d] -> [%d](S46_MAPSVR_INIT)\n", __FUNCTION__, __LINE__, CURRENT_STATE, S46_MAPSVR_INIT);
			CURRENT_STATE = S46_MAPSVR_INIT;
		}
	} else if (state == S46_MAPSVR_OK) {
		interval_t = GetExeTime(S46_MAPSVR_OK);
		logmessage("[MAP-E]", "Receiving a rule is successfully(%d). Reconfirm after %d min.", rsp_code, interval_t/60);
		if (CURRENT_STATE != S46_MAPSVR_OK) {
			_dprintf("[%s(%d)]Current State: [%d] -> [%d](S46_MAPSVR_OK)\n", __FUNCTION__, __LINE__, CURRENT_STATE, S46_MAPSVR_OK);
			if (CURRENT_STATE > 1) { //[Error] -> [OK]
				if (!strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_fmrs")), "")) {
					//FIXME Need to check current connection is online/offline.
					restart_wan_if();
				} else if (CURRENT_STATE != S46_MAPSVR_NO_RESPONSE) {
					start_s46_tunnel(wan_primary_ifunit());
				}
			}
			CURRENT_STATE = S46_MAPSVR_OK;
		}
		if (ce_dad_check())
			logmessage("[MAP-E]", "Detect duplicate CE address.");
	} else if (state == S46_MAPSVR_DATA_INVALID) {
		interval_t = GetExeTime(S46_MAPSVR_DATA_INVALID);
		logmessage("[MAP-E]", "Receiveing invalid rule. Reconfirm after %d min.", interval_t/60);
		if (CURRENT_STATE != S46_MAPSVR_DATA_INVALID) {
			_dprintf("[%s(%d)]Current State: [%d] -> [%d](S46_MAPSVR_DATA_INVALID)\n", __FUNCTION__, __LINE__, CURRENT_STATE, S46_MAPSVR_DATA_INVALID);
			stop_s46_tunnel(wan_primary_ifunit(), 0);
			CURRENT_STATE = S46_MAPSVR_DATA_INVALID;
		}
	} else if (state == S46_MAPSVR_ERR) {
		interval_t = GetExeTime(S46_MAPSVR_ERR);
		logmessage("[MAP-E]", "Map server error(%d). Reconfirm after %d min.", rsp_code, interval_t/60);
		if (CURRENT_STATE != S46_MAPSVR_ERR) {
			_dprintf("[%s(%d)]Current State: [%d] -> [%d](S46_MAPSVR_ERR)\n", __FUNCTION__, __LINE__, CURRENT_STATE, S46_MAPSVR_ERR);
			stop_s46_tunnel(wan_primary_ifunit(), 0);
			CURRENT_STATE = S46_MAPSVR_ERR;
		}
	} else if (state == S46_MAPSVR_NO_RESPONSE) {
		interval_t = GetExeTime(S46_MAPSVR_NO_RESPONSE);
		logmessage("[MAP-E]", "Map server did not reply(%d). Reconfirm after %d min.", rsp_code, interval_t/60);
		if (CURRENT_STATE != S46_MAPSVR_NO_RESPONSE) {
			_dprintf("[%s(%d)]Current State: [%d] -> [%d](S46_MAPSVR_NO_RESPONSE)\n", __FUNCTION__, __LINE__, CURRENT_STATE, S46_MAPSVR_NO_RESPONSE);
			if (CURRENT_STATE > 1) { //[Error] -> [NO_RESPONSE]
				if (strcmp(nvram_safe_get(ipv6_nvname("ipv6_s46_fmrs")), "")) {
					stop_s46_tunnel(wan_primary_ifunit(), 0);
					start_s46_tunnel(wan_primary_ifunit());
				}
			}
			CURRENT_STATE = S46_MAPSVR_NO_RESPONSE;
		}
	}

	if (rulebuf)
		free(rulebuf);
setime:
	FORCE_TRIGGER = 0;
	setalarm_t(interval_t, 0);
}

int s46map_rptd_main(int argc, char *argv[])
{
	FILE *fp;

	/* write pid */
	if ((fp = fopen("/var/run/s46map_rptd.pid", "w")) != NULL) {
	    fprintf(fp, "%d", getpid());
	    fclose(fp);
	}

	signal_register();

	memset(&tick, 0, sizeof(tick));

	do_map_check();

	/* When get a SIGALRM, the main process will enter another loop for pause() */
	while(1) {
		pause();
	}

	return 0;
}
