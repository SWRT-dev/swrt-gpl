/*
 * Copyright 2022-2023, SWRTdev
 * Copyright 2022-2023, paldier <paldier@hotmail.com>.
 * Copyright 2022-2023, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(RTCONFIG_RALINK)
#include <ralink.h>
#elif defined(RTCONFIG_QCA)
#include <qca.h>
#elif defined(RTCONFIG_LANTIQ)
#include <lantiq.h>
#endif
#include <bcmnvram.h>
#include <shutils.h>
#include <swrtmesh.h>
#include <shared.h>

#if defined(RTCONFIG_RALINK)
#undef IS_SPACE
#define IS_SPACE(c) ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
char bh2gifname[] = "ra11";
char bh5gifname[] = "rai11";
char bh5g2ifname[] = "rai11";
char wificonf_buf[256];
#elif defined(RTCONFIG_QCA)
#error fixme
#elif defined(RTCONFIG_LANTIQ)
#error fixme
#endif

int get_easymesh_max_ver(void)
{
#if defined(RTCONFIG_MT798X)
	return EASYMESH_VER_R3;
#else
	return EASYMESH_VER_R2;
#endif
}

char *get_easymesh_ver_str(int ver)
{
	switch(ver){
		case EASYMESH_VER_R4:
			return "R4";
		case EASYMESH_VER_R3:
			return "R3";
		case EASYMESH_VER_R2:
			return "R2";
		default:
			return "R1";
	}
}

char *get_mesh_bh_ifname(int band)
{
#if defined(RTCONFIG_RALINK)
	int i, index = 0;
	char tmp[18];
//skip main ap and guest wifi
	for(i = 0; i < MAX_NO_MSSID; i++){
		snprintf(tmp, sizeof(tmp), "wl%d.%d_bss_enabled", band, i);
		if(nvram_match(tmp, "1"))
			index++;
	}

	if(band){
#if defined(RTCONFIG_MT798X)
		snprintf(bh5gifname, sizeof(bh5gifname), "rax%d", index);
#else
		snprintf(bh5gifname, sizeof(bh5gifname), "rai%d", index);
#endif
		return bh5gifname;
	}else{
		snprintf(bh2gifname, sizeof(bh2gifname), "ra%d", index);
		return bh2gifname;
	}
#elif defined(RTCONFIG_QCA)
#error fixme
#elif defined(RTCONFIG_LANTIQ)
#error fixme
#endif
}

void check_mssid_prelink_reset(uint32_t sf)
{
}

void swrtmesh_autoconf(void)
{

	int sw_mode = sw_mode();
	if(sw_mode == SW_MODE_AP || sw_mode == SW_MODE_ROUTER){
#if defined(RTCONFIG_EASYMESH)
		nvram_set("easymesh_enable", "1");
		nvram_set("easymesh_mode", "1");
		if(nvram_match("re_mode", "1")){
			nvram_set("easymesh_role", "2");//agent
		}else if(nvram_match("x_Setting", "0")/* || nvram_match("w_Setting", "0")*/){
			nvram_set("easymesh_role", "0");//auto
		}else{
			nvram_set("easymesh_role", "1");//controller
		}
#elif defined(RTCONFIG_SWRTMESH)
		nvram_set("swrtmesh_enable", "1");
		nvram_set("ieee1905_enable", "1");
		nvram_set("swrtmesh_steer_enable", "1");
		if(nvram_match("re_mode", "1")){
			nvram_set("swrtmesh_agent_enable", "1");//agent
			nvram_set("swrtmesh_controller_enable", "0");//controller
		}else{
			nvram_set("swrtmesh_agent_enable", "1");//agent
			nvram_set("swrtmesh_controller_enable", "1");//controller
		}
#endif
	}else{//like aimesh, controller/agent only works in ap/router mode
#if defined(RTCONFIG_EASYMESH)
		nvram_set("easymesh_enable", "0");
		nvram_set("easymesh_role", "0");
		nvram_set("easymesh_mode", "0");
#elif defined(RTCONFIG_SWRTMESH)
		nvram_set("swrtmesh_enable", "0");
		nvram_set("swrtmesh_steer_enable", "0");
		nvram_set("swrtmesh_agent_enable", "0");
		nvram_set("swrtmesh_controller_enable", "0");
#endif
		nvram_set("re_mode", "0");
	}
}

#if defined(RTCONFIG_RALINK) && defined(RTCONFIG_EASYMESH)
char *mesh_format_ssid(char *ssid, size_t len)
{
	int i;
	char tmp[128] = {0};
	char *pt = ssid;
	for(i = 0; i < sizeof(tmp) && *pt; i++, pt++){
		if(*pt == ' '){
			tmp[i] = '\\';
			i++;
			tmp[i] = ' ';
		}else if(*pt == '\\'){
			tmp[i] = '\\';
			i++;
			tmp[i] = '\\';
		}else
			tmp[i] = *pt;
	};
    SWRTMESH_DBG("ssid=%s, mesh ssid=%s\n", ssid, tmp);
	snprintf(ssid, len, "%s", tmp);
	return ssid;
}

/* return it's line number, if not found, return -1 */
static int __locate_key(FILE *fp, char *key)
{
    int offset = -1;
    char buffer[1024] = {0};
    char *p = NULL;
    char *q = NULL;

    if(fp == NULL || key == NULL)
    	return offset;

    fseek(fp, 0, SEEK_SET);
    do
    {
        offset = ftell(fp); /* record current pos */
        memset(buffer, 0, sizeof(buffer));
        p = fgets(buffer, sizeof(buffer), fp);
        if(!p) break;
        q = strstr(buffer, "=");
        if(!q) continue;
        *q = 0; /* cut off the part from '=' */
        while(IS_SPACE(*p)) p++; /* trim leading spaces */
        if(*p == 0) continue; /* skip empty line */
        if(*p == '#') continue; /* skip comment line */
        if (0 == strcmp(key, p))
            break;
    }
    while(1);
    fseek(fp, offset, SEEK_SET);

    return offset;
}

char *wificonf_get(char *key, char *path)
{
    int offset = 0;
    FILE *fp = NULL;
    char *p = NULL;
    char *q = NULL;

    if((fp = fopen(path, "rb")) == NULL)
		return NULL;

    offset = __locate_key(fp, key);
    if(offset < 0)
		return NULL;

    memset(wificonf_buf, 0, sizeof(wificonf_buf));
    fseek(fp, offset, SEEK_SET);
    p = fgets(wificonf_buf, sizeof(wificonf_buf)-1, fp);
    if(!p) return NULL;
    p = strstr(wificonf_buf, "=");
    if(!p) return NULL;
    p++;

    while(IS_SPACE(*p)) p++; /* trim head spaces */
    q = p;
    while(*q != 0) q++;
    q--;/* q points to the last character */
    while(q > p && IS_SPACE(*q))
    {
        *q = 0;
        q--;
    };  /* trim tail spaces */
    SWRTMESH_DBG("%s\n", p);

    if(fp) fclose(fp);
    return p;
}

int wificonf_set(char *key, char *value, char *path)
{
    int ret = 0;
    FILE *fp = NULL;
    char *buffer = NULL;
    int nbytes = 0;
    char *p = NULL;
    struct stat sb;
    size_t len = strlen(value);

    if((fp = fopen(path, "rb")) == NULL)
		return -1;

    nbytes = __locate_key(fp, key);
    if(nbytes < 0)
		return -1;

    if(stat(path, &sb) != 0)
		return -1;
    buffer = (char *)malloc(sb.st_size + 1);
    if(buffer == NULL)
		return -1;
    buffer[sb.st_size] = 0;

    fseek(fp, 0, SEEK_SET);
    ret = fread(buffer, 1, sb.st_size, fp);
    if(ret != sb.st_size)
		return -1;
    fclose(fp);

    if((fp = fopen(path, "wb")) == NULL)
		return -1;

    ret = fwrite(buffer, 1, nbytes, fp);
    if(ret != nbytes)
		return -1;
    fprintf(fp, "%s=", key);
    for(ret = 0; ret < len; ret++)
        fprintf(fp, "%c", value[ret]);
    fprintf(fp, "\n");

    p = buffer + nbytes;
    while('\n' != *p && 0 != *p) p++;
    if (0 != *p) p++; /* now p is the next line */

    nbytes = buffer + sb.st_size - p; /* remain data */
    ret = fwrite(p, 1, nbytes, fp);
    if(ret != nbytes)
		return -1;
    if(fp) fclose(fp);
    return 0;
}

char *wificonf_token_get(char *key, int idx, char *path)
{
    FILE *fp = NULL;
	char cmd[128];
	snprintf(cmd, sizeof(cmd), "wificonf -f %s get %s %d", path, key, idx);
    memset(wificonf_buf, 0, sizeof(wificonf_buf));
	if((fp = popen(cmd, "r")) == NULL)
		return NULL;
    fread(wificonf_buf, 1, sizeof(wificonf_buf), fp);
    SWRTMESH_DBG("%s\n", wificonf_buf);
	fclose(fp);
	return wificonf_buf;
}
int wificonf_token_set(char *key, int idx, char *value, char *path, int base64)
{
	char cmd[128];
	if(base64)
		snprintf(cmd, sizeof(cmd), "wificonf -e -f %s set %s %d %s", path, key, idx, value);
	else
		snprintf(cmd, sizeof(cmd), "wificonf -f %s set %s %d %s", path, key, idx, value);
	system(cmd);
	return 0;
}
#endif

int mesh_get_chanlist(int unit, char *chList, size_t len)
{
	unsigned char countryCode[3];
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	memset(countryCode, 0, sizeof(countryCode));
	strncpy(countryCode, nvram_safe_get(strcat_r(prefix, "country_code", tmp)), 2);

	if(get_channel_list_via_driver(unit, chList, len) > 0)
		return 1;
	else if(countryCode[0] != 0xff && countryCode[1] != 0xff){
		if(get_channel_list_via_country(unit, countryCode, chList, len) > 0)
			return 1;
	}
	return 0;
}
