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
 */
/*
 * Frontend command-line utility for Linux NVRAM layer
 *
 * Copyright 2004, ASUSTeK Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: nvram.c,v 1.1 2007/06/08 10:22:42 arthur Exp $
 */

#define _GNU_SOURCE	// for stpcpy()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>	// for sleep()
#include <typedefs.h>
#include <stdint.h>

#include <rtconfig.h>
#include <bcmnvram.h>
#include <shared.h>

#include "nvram_mode.h"

#define PROTECT_CHAR	'x'
#define DEFAULT_LOGIN_DATA	"xxxxxxxx"

static int export_mode(char* mode, char* buf_ap, char* buf)
{
	const char **conf = NULL, **hold = NULL;
	const wlc_conf_t *wlc = NULL;
	char *ptr, *item, *value;
	char name[128], nv[128];
	int i, len;

	if (!buf_ap || !buf)
		return -1;

	if (strcmp(mode, "save_ap") == 0) {
		conf = conf_ap;
		hold = hold_ap;
	} else if (strcmp(mode, "save_rp_2g") == 0) {
		conf = conf_rp_2g;
		hold = hold_rp_2g;
		wlc = wlc_rp_2g;
	} else if (strcmp(mode, "save_rp_5g") == 0) {
		conf = conf_rp_5g;
		hold = hold_rp_5g;
		wlc = wlc_rp_5g;
	} else if (strcmp(mode, "save_rp_5g2") == 0) {
		conf = conf_rp_5g;
		hold = hold_rp_5g;
		wlc = wlc_rp_5g2;
	} else
		return -1;

	ptr = buf_ap;

	for (i = 0; conf[i]; i++) {
#ifdef ASUS_DEBUG
		puts(conf[i]);
#endif
		ptr = stpcpy(ptr, conf[i]) + 1;
	}

	for (item = buf; *item; item += strlen(item) + 1) {
		value = strchr(item, '=');
		if (!value)
			continue;
		len = value - item;
		if (len < 0 || len > sizeof(name) - 1)
			continue;

		strncpy(name, item, len);
		name[len] = '\0';
		value++;

		for (i = 0; hold[i]; i++) {
			if (strcmp(name, hold[i]) == 0) {
				snprintf(nv, sizeof(nv), "%s=%s", name, value);
#ifdef ASUS_DEBUG
				puts(nv);
#endif
				ptr = stpcpy(ptr, nv) + 1;
				break;
			}
		}

		// save wlc settings
		if (wlc) {
			for (i = 0; wlc[i].key_assign; i++) {
				if (strcmp(name, wlc[i].key_from) == 0) {
					snprintf(nv, sizeof(nv), "%s=%s", wlc[i].key_assign, value);
#ifdef ASUS_DEBUG
					puts(nv);
#endif
					ptr = stpcpy(ptr, nv) + 1;
				}
			}
		}
	}

	return 0;
}

#ifdef RTCONFIG_NVRAM_ENCRYPT
static int nvram_dec_all(char* buf_ap, char* buf)
{
	struct nvram_tuple *t;
	extern struct nvram_tuple router_defaults[];
	char *ptr, *item, *value;
	char name[128], nv[65535];
	int len;
	char output[NVRAM_ENC_MAXLEN];
	memset(output, 0, sizeof(output));


	if (!buf_ap || !buf)
		return -1;

	ptr = buf_ap;

	for (item = buf; *item; item += strlen(item) + 1) {
		value = strchr(item, '=');
		if (!value)
			continue;
		len = value - item;
		if (len < 0 || len > sizeof(name) - 1)
			continue;

		strncpy(name, item, len);
		name[len] = '\0';
		value++;

		for (t = router_defaults; t->name; t++)
		{
			if (strcmp(name, t->name) == 0 && t->enc == 1) {
				dec_nvram(t->name, value, output);
				value = output;
			}
		}

		snprintf(nv, sizeof(nv), "%s=%s", name, value);
		ptr = stpcpy(ptr, nv) + 1;
#ifdef ASUS_DEBUG
				puts(nv);
#endif
	}

	return 0;
}
#endif

/*******************************************************************
* NAME: _secure_romfile
* AUTHOR: Andy Chiu
* CREATE DATE: 2015/06/08
* DESCRIPTION: replace account /password by PROTECT_CHAR with the same string length
* INPUT:  path: the rom file path
* OUTPUT:
* RETURN:  0: success, -1:failed
* NOTE: Andy Chiu, 2015/12/18. Add new tokens.
*           Andy Chiu, 2015/12/24. Add new tokens.
*	     Andy Chiu, 2016/02/18. Add new token, wtf_username.
*******************************************************************/
/* memset list field.
 * member mask is bitmask: 0 - 1st, 2 - 2nd, ... 0x80000000 - 31th member */
static char *nvlist_memset(char *list, int c, unsigned int member_mask)
{
	char *item, *end, *next;
	char *mitem, *mend, *mnext;
	unsigned int mask;

	if (!list || member_mask == 0)
		return list;

	for (item = list; *item; ) {
		end = strchr(item, '<');
		if (end)
			next = end + 1;
		else	next = end = strchr(item, 0);
		for (mitem = item, mask = member_mask;
		     mitem < end && mask; mask >>= 1) {
		        mend = memchr(mitem, '>', end - mitem);
			if (mend)
				mnext = mend + 1;
			else	mnext = mend = end;
			if (mask & 1)
				memset(mitem, c, mend - mitem);
			mitem = mnext;
		}
		item = next;
	}

	return list;
}

#if 0
static int _secure_conf(char* buf)
{
	char name[128], *value, *item;
	int len, i;

	//name contains token
	const char *keyword_token[] = {"http_username", "passwd", "password",
		NULL};	//Andy Chiu, 2015/12/18

	//name is token
	const char *token1[] = {"wan_pppoe_passwd", "modem_pass", "modem_pincode",
		"http_passwd", "wan0_pppoe_passwd", "wan1_pppoe_passwd", "dslx_pppoe_passwd", "ddns_passwd_x",
		"wl_wpa_psk",	"wlc_wpa_psk",  "wlc_wep_key",
		"wl0_wpa_psk", "wl0.1_wpa_psk", "wl0.2_wpa_psk", "wl0.3_wpa_psk",
		"wl1_wpa_psk", "wl1.1_wpa_psk", "wl1.2_wpa_psk", "wl1.3_wpa_psk",
		"wl0.1_key1", "wl0.1_key2", "wl0.1_key3", "wl0.1_key4",
		"wl0.2_key1", "wl0.2_key2", "wl0.2_key3", "wl0.2_key4",
		"wl0.3_key1", "wl0.3_key2", "wl0.3_key3", "wl0.3_key4",
		"wl0_key1", "wl0_key2", "wl0_key3", "wl0_key4",
		"wl1.1_key1", "wl1.1_key2", "wl1.1_key3", "wl1.1_key4",
		"wl1.2_key1", "wl1.2_key2", "wl1.2_key3", "wl1.2_key4",
		"wl1.3_key1", "wl1.3_key2", "wl1.3_key3", "wl1.3_key4",
		"wl_key1", "wl_key2", "wl_key3", "wl_key4",
		"wl1_key1", "wl1_key2", "wl1_key3", "wl1_key4",
		"wl0_phrase_x", "wl0.1_phrase_x", "wl0.2_phrase_x", "wl0.3_phrase_x",
		"wl1_phrase_x", "wl1.1_phrase_x", "wl1.2_phrase_x", "wl1.3_phrase_x",
		"wl_phrase_x", "vpnc_openvpn_pwd", "PM_SMTP_AUTH_USER", "PM_MY_EMAIL",
		"PM_SMTP_AUTH_PASS", "wtf_username", "ddns_hostname_x", "ddns_username_x",
		"shell_username", "shell_passwd",
		NULL};

	//name is token
	//value is [<]username>password<username...
	const char *token2[] = {"acc_list", "pptpd_clientlist", "vpn_serverx_clientlist",
		NULL};

	//name is token
	//valus is [<]desc>type>index>username>password<desc...
	const char vpnc_token[] = "vpnc_clientlist";

	//name is token
	//value is [<]type>username>password>url>rule>dir>enable<type...
	const char cloud_token[] = "cloud_sync";

	//name is token
	//value is username@domain.tld
	const char pppoe_username_token[] = "pppoe_username";

	if (!buf)
		return -1;

	for (item = buf; *item; item += strlen(item) + 1) {
		value = strchr(item, '=');
		if (!value)
			continue;
		len = value - item;
		if (len < 0 || len > sizeof(name) - 1)
			continue;

		strncpy(name, item, len);
		name[len] = '\0';
		value++;

		//check the password keyword token
		for (i = 0; keyword_token[i]; i++) {
			if (strstr(name, keyword_token[i]) != NULL) {
				memset(value, PROTECT_CHAR, strlen(value));
				goto next;
			}
		}

		//check the first token group
		for (i = 0; token1[i]; i++) {
			if (strcmp(name, token1[i]) == 0) {
				memset(value, PROTECT_CHAR, strlen(value));
				goto next;
			}
		}

		//check the 2nd token group
		//value is [<]username>password<username...
		for (i = 0; token2[i]; i++) {
			if (strcmp(name, token2[i]) == 0) {
				nvlist_memset(value, PROTECT_CHAR, (1u << 1));
				goto next;
			}
		}

		//check vpnc token
		//valus is [<]desc>type>index>username>password<desc...
		if (strcmp(name, vpnc_token) == 0) {
			nvlist_memset(value, PROTECT_CHAR, (1u << 4));
			goto next;
		}

		//check cloud sync token
		//value is [<]type>xxx1>xxx2>xxx3>xxx4>xxx5>xxx6<type...
		if (strcmp(name, cloud_token) == 0) {
			nvlist_memset(value, PROTECT_CHAR, (1u << 2)|(1u << 3)|(1u << 5));
			goto next;
		}

		//check
		//value is password@domain.tld
		if (strstr(name, pppoe_username_token) != NULL) {
			char *e = strchr(value, '@') ? : strchr(value, 0);
			memset(value, PROTECT_CHAR, e - value);
		}

	next:
		continue;
	}

	return 0;
}
#else
static int _convert_data(const char *name, char *value, size_t value_len)
{
	int i;

	//name contains token
	const char *http_token[] = {"http_username", "http_passwd", NULL};
	const char password_token[] = "password";
	const char acclist_token[] = "acc_list";

	//name is token
	const char *token1[] = {"wan_pppoe_passwd", "modem_pass", "modem_pincode",
		"http_passwd", "wan0_pppoe_passwd", "wan1_pppoe_passwd","dslx_pppoe_passwd", "ddns_passwd_x",
		"wl_wpa_psk",	"wlc_wpa_psk",  "wlc_wep_key",
		"wl0_wpa_psk", "wl0.1_wpa_psk", "wl0.2_wpa_psk", "wl0.3_wpa_psk",
		"wl1_wpa_psk", "wl1.1_wpa_psk", "wl1.2_wpa_psk", "wl1.3_wpa_psk",
		"wl2_wpa_psk", "wl2.1_wpa_psk", "wl2.2_wpa_psk", "wl2.3_wpa_psk",
		"wl0.1_key1", "wl0.1_key2", "wl0.1_key3", "wl0.1_key4",
		"wl0.2_key1", "wl0.2_key2", "wl0.2_key3", "wl0.2_key4",
		"wl0.3_key1", "wl0.3_key2", "wl0.3_key3", "wl0.3_key4",
		"wl0_key1", "wl0_key2", "wl0_key3", "wl0_key4",
		"wl1.1_key1", "wl1.1_key2", "wl1.1_key3", "wl1.1_key4",
		"wl1.2_key1", "wl1.2_key2", "wl1.2_key3", "wl1.2_key4",
		"wl1.3_key1", "wl1.3_key2", "wl1.3_key3", "wl1.3_key4",
		"wl1_key1", "wl1_key2", "wl1_key3", "wl1_key4",
		"wl2.1_key1", "wl2.1_key2", "wl2.1_key3", "wl2.1_key4",
		"wl2.2_key1", "wl2.2_key2", "wl2.2_key3", "wl2.2_key4",
		"wl2.3_key1", "wl2.3_key2", "wl2.3_key3", "wl2.3_key4",
		"wl2_key1", "wl2_key2", "wl2_key3", "wl2_key4",
		"wl_key1", "wl_key2", "wl_key3", "wl_key4",
		"wl0_phrase_x", "wl0.1_phrase_x", "wl0.2_phrase_x", "wl0.3_phrase_x",
		"wl1_phrase_x", "wl1.1_phrase_x", "wl1.2_phrase_x", "wl1.3_phrase_x",
		"wl2_phrase_x", "wl2.1_phrase_x", "wl2.2_phrase_x", "wl2.3_phrase_x",
		"wl_phrase_x", "vpnc_openvpn_pwd", "PM_SMTP_AUTH_USER", "PM_MY_EMAIL",
		"PM_SMTP_AUTH_PASS", "wtf_username", "ddns_hostname_x", "ddns_username_x",
		"shell_username", "shell_passwd",
		NULL};

	//name is token
	//value is [<]username>password<username...
	const char *token2[] = {"pptpd_clientlist", "vpn_serverx_clientlist",	NULL};

	//name is token
	//valus is [<]desc>type>index>username>password<desc...
	const char vpnc_token[] = "vpnc_clientlist";

	//name is token
	//value is [<]type>username>password>url>rule>dir>enable<type...
	const char cloud_token[] = "cloud_sync";

	//name is token
	//value is username@domain.tld
	const char pppoe_username_token[] = "pppoe_username";

#ifdef RTCONFIG_MULTILAN_CFG
	//value is  <band set>auth mode>crypto>psk>...
	const char apg_security_token[] = "apg%d_security";
	char token_name[128];
#endif

#if defined(RTCONFIG_QCA)
	const char *token3[] = {
		"wlc0_wpa_psk", "wlc1_wpa_psk",
#if defined(RTCONFIG_HAS_5G_2)
		"wlc2_wpa_psk",
#endif
#if defined(RTCONFIG_DWB) 
		"dwb_wlc0_wpa_psk", "dwb_wlc1_wpa_psk", 
#if defined(RTCONFIG_HAS_5G_2)
		"dwb_wlc2_wpa_psk",
#endif
#if defined(RTCONFIG_NO_TRY_DWB_PROFILE)
		"wsbh_wpa_psk", "wsfh_wpa_psk",
#endif
#endif
		NULL };
#endif

	if(!value)
		return 0;

	//change http login username and password as xxxxxxxx
	for(i = 0; http_token[i]; ++i)
	{
		if(!strcmp(name, http_token[i]))
		{
			strlcpy(value, DEFAULT_LOGIN_DATA, value_len);
			return 1;
		}		
	}

	//check the password keyword token
	if (strstr(name, password_token) != NULL) 
	{
		memset(value, PROTECT_CHAR, strlen(value));
		return 1;
	}

	//check the first token group
	for (i = 0; token1[i]; i++) 
	{
		if (strcmp(name, token1[i]) == 0) 
		{
			memset(value, PROTECT_CHAR, strlen(value));
			return 1;
		}
	}

	//check the 2nd token group
	//value is [<]username>password<username...
	for (i = 0; token2[i]; i++)
	{
		if (strcmp(name, token2[i]) == 0)
		{
			nvlist_memset(value, PROTECT_CHAR, (1u << 1));
			return 1;
		}
	}

	//check vpnc token
	//valus is [<]desc>type>index>username>password<desc...
	if (strcmp(name, vpnc_token) == 0) 
	{
		nvlist_memset(value, PROTECT_CHAR, (1u << 4));
		return 1;
	}

	//check cloud sync token
	//value is [<]type>xxx1>xxx2>xxx3>xxx4>xxx5>xxx6<type...
	if (strcmp(name, cloud_token) == 0) 
	{
		nvlist_memset(value, PROTECT_CHAR, (1u << 2)|(1u << 3)|(1u << 5));
		return 1;
	}

	//check
	//value is password@domain.tld
	if (strstr(name, pppoe_username_token) != NULL) 
	{
		char *e = strchr(value, '@') ? : strchr(value, 0);
		memset(value, PROTECT_CHAR, e - value);
		return 1;
	}
	//convert acc_list as DEFAULT_LOGIN_DATA>DEFAULT_LOGIN_DATA
	if(strcmp(name, acclist_token) == 0)
	{
		snprintf(value, value_len, "%s>%s", DEFAULT_LOGIN_DATA, DEFAULT_LOGIN_DATA);
		return 1;
	}

#if defined(RTCONFIG_QCA)
	//check the first token group
	for (i = 0; token3[i]; i++) {
		if (strcmp(name, token3[i]) == 0) {
			memset(value, PROTECT_CHAR, strlen(value));
			return 1;
		}
	}
#endif

#ifdef RTCONFIG_MULTILAN_CFG
	//check the 4th token group in each token with index
	//value is  <band set>auth mode>crypto>psk>...
	for (i = 0; i < MTLAN_MAXINUM; ++i)
	{
		snprintf(token_name, sizeof(token_name), apg_security_token, i);
		if (strcmp(name, token_name) == 0)
		{
			nvlist_memset(value, PROTECT_CHAR, (1u << 3));
			return 1;
		}
	}
#endif

	return 0;
}

static char* _get_attr(const char *buf, char *name, size_t name_len, char *value, size_t value_len)
{
	char *p, *e,  *v;
	if(!buf || !name || !value)
		return NULL;

	memset(name, 0, name_len);
	memset(value, 0, value_len);
	
	p = strchr(buf, '=');
	if(p)
	{
		strlcpy(name, buf, ((p - buf + 1) > name_len)? name_len:  (p - buf + 1));

		v = p + 1;
		e = strchr(v, '\0');
		if(e)
		{
			strlcpy(value, v, ((e - v + 1) > value_len)? value_len:  (e - v + 1));
			return e + 1;
		}
		else	//last line
		{
			strlcpy(value, v, value_len);
			return 0;
		}
		
	}
	return NULL;
}

static int _secure_conf(char* buf, size_t len)
{
	char *tmp, *p = buf, *b;
	char name[256], *value;
	int tmp_len;

	if(!buf || !len)
		return -1;

	tmp = malloc(len);
	if(!tmp)
	{
		fprintf(stderr, "Can NOT alloc memory!!!");
		return -1;
	}

	value = malloc(CKN_STR_MAX);
	if(!value)
	{
		fprintf(stderr, "Can NOT alloc memory!!!");
		free(tmp);
		return -1;
	}

	memset(tmp, 0, len);
	b = tmp;
	
	while(p && *p != '\0')
	{
		p = _get_attr(p, name, sizeof(name), value, CKN_STR_MAX);

		{
			//handle data
			_convert_data(name, value, CKN_STR_MAX);

			//write data in the new buffer
			if(value)
				tmp_len = snprintf(b, len - (b - tmp), "%s=%s", name, value);
			else
				tmp_len = snprintf(b, len - (b - tmp), "%s=", name);
			
			b += (tmp_len + 1);	//Add NULL at the end of the value
		}
	}

	memcpy(buf, tmp, len);

	free(tmp);
	free(value);
	return 0;
}
#endif

static void
usage(void)
{
	fprintf(stderr, 
		"usage: nvram [get name] [set name=value] [unset name] [erase] [show] [save file] [restore file] [fb_save file]\n"
		"usage: nvram [save_ap file] [save_rp_2g file] [save_rp_5g file]\n");
	exit(0);
}

#define PROFILE_HEADER		"HDR1"
#ifdef RTCONFIG_DSL
#define PROFILE_HEADER_NEW      "N55U"
#else
#ifdef RTCONFIG_QCA
#define PROFILE_HEADER_NEW      "AC55U"
#elif defined(RTCONFIG_LANTIQ)
#define PROFILE_HEADER_NEW      "BLUE"
#else
#define PROFILE_HEADER_NEW      "HDR2"
#endif
#endif


// save nvram to file
int nvram_save(char *file, char *buf)
{
	FILE *fp;
	char *name;
	unsigned long count, filelen, i;

   	if ((fp=fopen(file, "w"))==NULL) return -1;

	count = 0;
	for (name=buf;*name;name+=strlen(name)+1)
	{	
		puts(name);
		count = count+strlen(name)+1;
	}
   	
   	filelen = count + (1024 - count%1024);	
   	fwrite(PROFILE_HEADER, 1, 4, fp);
   	fwrite(&filelen, 1, 4, fp);
   	fwrite(buf, 1, count, fp);
   	for (i=count;i<filelen;i++) fwrite(name, 1, 1, fp);   	
   	fclose(fp);
	return 0;
}

unsigned char get_rand()
{
	unsigned char buf[1];
	FILE *fp;
//	size_t rc;

	fp = fopen("/dev/urandom", "r");
	if (fp == NULL) {
#ifdef ASUS_DEBUG
		fprintf(stderr, "Could not open /dev/urandom.\n");
#endif
		return 0;
	}
	fread(buf, 1, 1, fp);
	fclose(fp);

	return buf[0];
}

int nvram_save_new(char *file, char *buf)
{
	FILE *fp;
	char *name;
#if defined(RTCONFIG_SAVEJFFS)
	uint32_t filelen_le32;
#endif
	unsigned long count, filelen, i;
	unsigned char rand = 0, temp;

	if ((fp = fopen(file, "w")) == NULL) return -1;

	count = 0;
	for (name = buf; *name; name += strlen(name) + 1)
	{
#ifdef ASUS_DEBUG
		puts(name);
#endif
		count = count + strlen(name) + 1;
	}

	filelen = count + (1024 - count % 1024);
#if 1
	//workaround for do_upload_post waitfor() timeout
	filelen = filelen >= 1024*3 ? filelen : 1024*3;
#endif

	rand = get_rand() % 30;
#ifdef ASUS_DEBUG
	fprintf(stderr, "random number: %x\n", rand);
#endif
	fwrite(PROFILE_HEADER_NEW, 1, 4, fp);
#if defined(RTCONFIG_SAVEJFFS)
	filelen_le32 = cpu_to_le32(filelen);
	fwrite(&filelen_le32, 1, 3, fp);
#else
	fwrite(&filelen, 1, 3, fp);
#endif
	fwrite(&rand, 1, 1, fp);
#ifdef ASUS_DEBUG
	for (i = 0; i < 4; i++)
	{
		fprintf(stderr, "%2x ", PROFILE_HEADER_NEW[i]);
	}
	for (i = 0; i < 3; i++)
	{
		fprintf(stderr, "%2x ", ((char *)&filelen)[i]);
	}
	fprintf(stderr, "%2x ", ((char *)&rand)[0]);
#endif
	for (i = 0; i < count; i++)
	{
		if (buf[i] == 0x0)
			buf[i] = 0xfd + get_rand() % 3;
		else
			buf[i] = 0xff - buf[i] + rand;
	}
	fwrite(buf, 1, count, fp);
#ifdef ASUS_DEBUG
	for (i = 0; i < count; i++)
	{
		if (i % 16 == 0) fprintf(stderr, "\n");
		fprintf(stderr, "%2x ", (unsigned char) buf[i]);
	}
#endif
	for (i = count; i < filelen; i++)
	{
		temp = 0xfd + get_rand() % 3;
		fwrite(&temp, 1, 1, fp);
#ifdef ASUS_DEBUG
		if (i % 16 == 0) fprintf(stderr, "\n");
		fprintf(stderr, "%2x ", (unsigned char) temp);
#endif
	}
	fclose(fp);
	return 0;
}

int issyspara(char *p)
{
	struct nvram_tuple *t/*eric--, *u*/;
	extern struct nvram_tuple router_defaults[];
	extern struct nvram_tuple router_prefix_defaults[];

	/* pass runtime create prefix nvram */
	for (t = router_prefix_defaults; t->name; t++)
	{
		//if(!strncmp(p, t->name, strlen(t->name))){
		if(strstr(p, t->name)){
			if(!strcmp("wl", t->name) && !strncmp(p+3, "_failed", 7))
				break;
			return 1;
		}
	}

	for (t = router_defaults; t->name; t++)
	{
		if (strstr(p, t->name))
			break;
	
	}

	if (t->name) return 1;
	else return 0;
}

// restore nvram from file
int nvram_restore(char *file, char *buf)
{
   	FILE *fp;
   	char header[8], *p, *v;
  	unsigned long count, *filelen;

   	if ((fp=fopen(file, "r+"))==NULL) return -1;
    	   
   	count = fread(header, 1, 8, fp);
   	if (count>=8 && strncmp(header, PROFILE_HEADER, 4)==0)
   	{  
	    filelen = (unsigned long *)(header + 4);
   	    count = fread(buf, 1, *filelen, fp);
   	}   
   	fclose(fp);

   	p = buf;       		   
   
   	while (*p)
   	{       
		//printf("nv:%s\n", p);     	     			     	
       		v = strchr(p, '=');

		if (v!=NULL)
		{	
			*v++ = '\0' /*eric--NULL*/;

			if (issyspara(p))
			{
				nvram_set(p, v);
			}

       			p = v + strlen(v) + 1;			
		}
		else 
		{
			nvram_unset(p);
			p = p + 1;
		}
   	}

	nvram_set("x_Setting", "1");
	return 0;
}

int nvram_restore_new(char *file, char *buf)
{
	FILE *fp;
	char header[8], *p, *v;
	uint32_t *filelenptr;
	unsigned long count, filelen, i;
	unsigned char rand, *randptr;

	if ((fp = fopen(file, "r+")) == NULL) return -1;

	count = fread(header, 1, 8, fp);
	if (count>=8 && strncmp(header, PROFILE_HEADER, 4) == 0)
	{
		filelenptr = (uint32_t *)(header + 4);
#ifdef ASUS_DEBUG
		fprintf(stderr, "restoring original text cfg of length %x\n", *filelenptr);
#endif
		fread(buf, 1, *filelenptr, fp);
	}
	else if (count>=8 && strncmp(header, PROFILE_HEADER_NEW, 4) == 0)
	{
		filelenptr = (uint32_t *)(header + 4);
#if defined(RTCONFIG_SAVEJFFS)
		filelen = le32_to_cpu(*filelenptr) & 0xffffff;
#else
		filelen = *filelenptr & 0xffffff;
#endif
#ifdef ASUS_DEBUG
		fprintf(stderr, "restoring non-text cfg of length %x\n", filelen);
#endif
		randptr = (unsigned char *)(header + 7);
		rand = *randptr;
#ifdef ASUS_DEBUG
		fprintf(stderr, "non-text cfg random number %x\n", rand);
#endif
		count = fread(buf, 1, filelen, fp);
#ifdef ASUS_DEBUG
		fprintf(stderr, "non-text cfg count %x\n", count);
#endif
		for (i = 0; i < count; i++)
		{
			if ((unsigned char) buf[i] > ( 0xfd - 0x1)){
				/* e.g.: to skip the case: 0x61 0x62 0x63 0x00 0x00 0x61 0x62 0x63 */
				if(i > 0 && buf[i-1] != 0x0)
					buf[i] = 0x0;
			}
			else
				buf[i] = 0xff + rand - buf[i];
		}
#ifdef ASUS_DEBUG
		for (i = 0; i < count; i++)
		{
			if (i % 16 == 0) fprintf(stderr, "\n");
			fprintf(stderr, "%2x ", (unsigned char) buf[i]);
		}

		for (i = 0; i < count; i++)
		{
			if (i % 16 == 0) fprintf(stderr, "\n");
			fprintf(stderr, "%c", buf[i]);
		}
#endif
	}
	else
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);

	p = buf;

	while (*p || p-buf<=count)
	{
#if 1
		/* e.g.: to skip the case: 00 2e 30 2e 32 38 00 ff 77 61 6e */
		if (!isprint(*p)) {
			p = p + 1;
			continue;
		}
#endif
		v = strchr(p, '=');

		if (v != NULL)
		{
			*v++ = '\0';

			if (issyspara(p))
				nvram_set(p, v);

			p = v + strlen(v) + 1;
		}
		else
		{
			nvram_unset(p);
			p = p + 1;
		}
	}
	return 0;
}

/* NVRAM utility */
int
main(int argc, char **argv)
{
	char *name, *value, *buf;
	int size;
	unsigned int nvram_space;
#ifdef RTCONFIG_CFE_NVRAM_CHK
	int ret = 0;
	FILE *fp;
#endif

	/* Skip program name */
	--argc;
	++argv;

	if (!*argv) 
		usage();

	extern unsigned int get_nvram_space(void);
	nvram_space = get_nvram_space();
	if(nvram_space == 0)
		nvram_space = MAX_NVRAM_SPACE;

	buf = malloc (nvram_space);
	if (buf == NULL)	{
		perror ("Out of memory!\n");
		return -1;
	}
	
	/* Process the remaining arguments. */
	for (; *argv; argv++) {
		if (!strncmp(*argv, "get", 3)) {
#ifdef RTCONFIG_NVRAM_ENCRYPT
			if(invalid_nvram_get_name(*argv))
				return 0;
#endif
			if (*++argv) {
				if ((value = nvram_get(*argv)))
					puts(value);
			}
		}
		else if (!strncmp(*argv, "set", 3)) {
			if (*++argv) {
				strncpy(value = buf, *argv, nvram_space);
				name = strsep(&value, "=");
				
#ifdef RTCONFIG_CFE_NVRAM_CHK
				ret = nvram_set(name, value);
				if(ret == 2) {
					fp = fopen("/var/log/cfecommit_ret", "w");
					if(fp!=NULL) {
		                                fprintf(fp,"Illegal nvram\n");
						fclose(fp);
					}
					puts("Illegal nvram format!");
				}
#else
				nvram_set(name, value);
#endif
			}
		}
		else if (!strncmp(*argv, "unset", 5)) {
			if (*++argv)
				nvram_unset(*argv);
		}
		else if (!strncmp(*argv, "commit", 5)) {
			nvram_commit();
		}
		else if (!strcmp(*argv, "save"))
		{
			if (*++argv)
			{
				nvram_getall(buf, nvram_space);
#ifdef RTCONFIG_NVRAM_ENCRYPT
				char *tmp_dnv = malloc(nvram_space);
				if (!tmp_dnv) {
					fprintf(stderr, "Can NOT alloc memory!!!");
					return 0;
				}
				memset(tmp_dnv, 0, nvram_space);
				nvram_dec_all(tmp_dnv, buf);
				nvram_save_new(*argv, tmp_dnv);
				free(tmp_dnv);
#else
				nvram_save_new(*argv, buf);
#endif
			}
		}
		else if (!strncmp(*argv, "save_ap", 7) ||
			 !strncmp(*argv, "save_rp", 7))
		{
			char *mode = *argv;
			if (*++argv) {
				char *tmp_export = malloc(nvram_space);
				if (!tmp_export) {
					fprintf(stderr, "Can NOT alloc memory!!!");
					return 0;
				}
				memset(tmp_export, 0, nvram_space);
				nvram_getall(buf, NVRAM_SPACE);
#ifdef RTCONFIG_NVRAM_ENCRYPT
				char *tmp_dnv = malloc(nvram_space);
				if (!tmp_dnv) {
					fprintf(stderr, "Can NOT alloc memory!!!");
					return 0;
				}
				memset(tmp_dnv, 0, nvram_space);
				nvram_dec_all(tmp_dnv, buf);
				export_mode(mode, tmp_export, tmp_dnv);
				free(tmp_dnv);
#else
				export_mode(mode, tmp_export, buf);
#endif
				nvram_save_new(*argv, tmp_export);
				free(tmp_export);
			}
		}
		//Andy Chiu, 2015/06/09
		else if (!strncmp(*argv, "fb_save", 7))
		{
			if (*++argv) {
				char *tmpbuf = malloc(nvram_space);
				if (!tmpbuf) {
					fprintf(stderr, "Can NOT alloc memory!!!");
					return 0;
				}
				nvram_getall(buf, nvram_space);
#ifdef RTCONFIG_NVRAM_ENCRYPT
				memset(tmpbuf, 0, nvram_space);
				nvram_dec_all(tmpbuf, buf);
#else
				memcpy(tmpbuf, buf, nvram_space);
#endif
				_secure_conf(tmpbuf, nvram_space);
				nvram_save_new(*argv, tmpbuf);
				free(tmpbuf);
			}
		}
		else if (!strncmp(*argv, "restore", 7)) 
		{
			if (*++argv) 
			{
				nvram_restore_new(*argv, buf);
			}
			
		}
		else if (!strcmp(*argv, "erase"))
		{
			system("nvram_erase");
		}
		else if (!strncmp(*argv, "show", 4) || !strncmp(*argv, "getall", 6)) {
#ifdef RTCONFIG_NVRAM_ENCRYPT
			char name_tmp[128] = {0};
			char *name_t = NULL, *value;
#endif
			nvram_getall(buf, nvram_space);
			for (name = buf; *name; name += strlen(name) + 1){
#ifdef RTCONFIG_NVRAM_ENCRYPT
				strlcpy(name_tmp, name, sizeof(name_tmp));
				name_t = value = name_tmp;
				name_t = strsep(&value, "=");

				if(invalid_nvram_get_name(name_t))
					continue;
#endif
				puts(name);
			}
			size = sizeof(struct nvram_header) + (long) (name - buf);
			fprintf(stderr, "size: %d bytes (%d left)\n", size, nvram_space - size);
		}
		if (!*argv)
			break;
	}

	if (buf != NULL)
		free (buf);
	return 0;
}	
