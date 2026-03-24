/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <uci.h>
#include <stdlib.h>
#include "dwpal.h"
#include "dwpal_ext.h"
#include "stats.h"
#include "dwpal_log.h"	//Logging

#if defined YOCTO
#include <slibc/string.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#include "libsafec/safe_mem_lib.h"
#endif

/* list of banned SDL methods */
#define strlen(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strtok(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcat(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncat(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcpy(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncpy(...)   	SDL_BANNED_FUNCTION ERROR_TOKEN
#define snprintf(...)	SDL_BANNED_FUNCTION ERROR_TOKEN

/* global declaration */
unsigned char *goutData = NULL; // for traversing NL data
int gSpace = 0; // for indentation
int gEnum = 0; // to find enum stat
#define MAX_STR_SIZE 32
#define MAX_NUM_OF_RATES 2319 /* (OFDM 8 + CCK 7 + AX (12 MCS * 8 NSS * 4 BW * 6 GI)) = 2319 */

#ifdef CTL_SCAN_STATS
#define BITRATE_INVALID    (0) /* use zero value if rate is unknown/invalid */
#define _NV_            BITRATE_INVALID

//MCS
#define BITRATE_MCS32      32 /* BPSK */
#define NUM_RATE_MCS_N     33   /* 0 .. 32 */
#define NUM_RATE_MCS_AC    10   /* 0 ... 9 */
#define NUM_RATE_MCS_AX    12   /* 0 .. 11 */

//NSS
#define PHY_STAT_RATE_NSS_MIN      1
#define HT_PHY_STAT_RATE_NSS_MAX   4
#define PHY_STAT_RATE_NSS_MAX      8

//CBW
#define PHY_STAT_RATE_CBW_MAX   3 /* CW_160 */
#define PHY_STAT_RATE_CBW_NUM   (PHY_STAT_RATE_CBW_MAX + 1)

#define IEEE80211_HT_MCS_MASK_LEN         10
#define IEEE80211_HE_PPE_THRES_MAX_LEN    25

//Capabilities IE id
#define WLAN_EID_SUPP_RATES           1
#define WLAN_EID_EXT_SUPP_RATES       50
#define WLAN_EID_HT_CAP               45
#define WLAN_EID_VHT_CAP              191
#define WLAN_EID_EXTENSION            255
#define WLAN_EID_EXT_HE_CAPABILITIES  35

//Supported rates
#define MAX_NUM_SUPPORTED_RATES  32
#define BITRATE_FACTOR           10
#define SUPP_RATE_TO_BITRATE(x)  ((x) * BITRATE_FACTOR / 2)

//HT param masks
#define IEEE80211_HT_CAP_SUP_WIDTH_20_40            0x0002
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT           0x4000
#define IEEE80211_HT_CAP_SGI_20                     0x0020
#define IEEE80211_HT_CAP_SGI_40                     0x0040

//VHT param masks
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK      0x0000000C
#define IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ    0x00000004
#define IEEE80211_VHT_CAP_SHORT_GI_80               0x00000020
#define IEEE80211_VHT_CAP_SHORT_GI_160              0x00000040

//HE param masks
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G         0x02
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G   0x04
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G        0x08
#define IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G  0x10

#define HE_CP_TYPES_NUM    3    /* 0 .. 2 */
#define HE_CP_MODES_NUM    6

enum hw_band {
  HW_BAND_5_2_GHZ,
  HW_BAND_2_4_GHZ,
  HW_BAND_BOTH,
  HW_BAND_NONE
};

enum phy_type {
  PHY_MODE_HT,
  PHY_MODE_VHT,
  PHY_MODE_HE
};

/* Mode 2 (11n) HT, MCS 0...32, CBW 0/1, SCP 0/1 */
/* Bit0 - SCP, Bit1 - CBW, Bits2345 - MCS */
#define NUM_RATE_MCS_N_PER_NSS     8
static const uint16_t bitrates_11n[NUM_RATE_MCS_N_PER_NSS * 2 * 2] = {
      65,     72,    135,    150, /*  0 */
     130,    144,    270,    300, /*  1 */
     195,    217,    405,    450, /*  2 */
     260,    289,    540,    600, /*  3 */
     390,    433,    810,    900, /*  4 */
     520,    578,   1080,   1200, /*  5 */
     585,    650,   1215,   1350, /*  6 */
     650,    722,   1350,   1500, /*  7 */
};

static const uint16_t bitrates_11n_mcs32[4] = {
    _NV_,   _NV_,     60,     67  /* 32 */ /* BPSK */
};

/* Mode 3 (11ac) VHT, MCS 0...9, CBW 0/1/2/3, SCP 0/1 */
/* Bit0 - SCP, Bits12 - CBW, Bits3456 - MCS */
static const uint16_t bitrates_11ac[NUM_RATE_MCS_AC * 2 * 4] =
{  /* NSS = 1 */
     65,     72,    135,    150,    293,    325,    585,    650, /*  0 */
    130,    144,    270,    300,    585,    650,   1170,   1300, /*  1 */
    195,    217,    405,    450,    878,    975,   1755,   1950, /*  2 */
    260,    289,    540,    600,   1170,   1300,   2340,   2600, /*  3 */
    390,    433,    810,    900,   1755,   1950,   3510,   3900, /*  4 */
    520,    578,   1080,   1200,   2340,   2600,   4680,   5200, /*  5 */
    585,    650,   1215,   1350,   2633,   2925,   5265,   5850, /*  6 */
    650,    722,   1350,   1500,   2925,   3250,   5850,   6500, /*  7 */
    780,    867,   1620,   1800,   3510,   3900,   7020,   7800, /*  8 */
   _NV_,   _NV_,   1800,   2000,   3900,   4333,   7800,   8667  /*  9 */
};

/* Mode 4 (11ax) HE, MCS 0...11, CBW 0/1/2/3, CP 0/1/2 */
static const uint16_t bitrates_11ax[NUM_RATE_MCS_AX * 4 * HE_CP_TYPES_NUM] =
{ /* NSS = 1 */
  /*  20     20     20      40     40     40      80     80     80     160    160    160     CBW */
  /* SCP    MCP    LCP     SCP    MCP    LCP     SCP    MCP    LCP     SCP    MCP    LCP *//* MCS*/
      86,    81,    73,    172,   163,   146,    360,   340,   306,    721,   681,   613,  /*  0 */
     172,   163,   146,    344,   325,   293,    721,   681,   613,   1441,  1361,  1225,  /*  1 */
     258,   244,   219,    516,   488,   439,   1081,  1021,   919,   2162,  2042,  1838,  /*  2 */
     344,   325,   293,    688,   650,   585,   1441,  1361,  1225,   2882,  2722,  2450,  /*  3 */
     516,   488,   439,   1032,   975,   878,   2162,  2042,  1838,   4324,  4083,  3675,  /*  4 */
     688,   650,   585,   1376,  1300,  1170,   2882,  2722,  2450,   5765,  5444,  4900,  /*  5 */
     774,   731,   658,   1549,  1463,  1316,   3243,  3063,  2756,   6485,  6125,  5513,  /*  6 */
     860,   813,   731,   1721,  1625,  1463,   3603,  3403,  3063,   7206,  6806,  6125,  /*  7 */
    1032,   975,   878,   2065,  1950,  1755,   4324,  4083,  3675,   8647,  8167,  7350,  /*  8 */
    1147,  1083,   975,   2294,  2167,  1950,   4804,  4537,  4083,   9607,  9074,  8166,  /*  9 */
    1290,  1219,  1097,   2581,  2438,  2194,   5404,  5104,  4594,  10809, 10208,  9188,  /* 10 */
    1434,  1354,  1219,   2868,  2708,  2438,   6004,  5671,  5104,  12010, 11342, 10208,  /* 11 */
};

void parse_ies(unsigned char *ie, int ielen,
        uint8_t *supp_rates,
        uint8_t **ht_cap,
        uint8_t **vht_cap,
        uint8_t **he_cap);
void parse_scan_result(struct nl_msg *msg);
void mac_addr_n2a(char *mac_addr, size_t addr_size, unsigned char *arg);
void calculate_phy_rate(int band, unsigned char *ie, int ielen);
bool is_valid_vht_rate_params(uint32_t cbw, uint32_t mcs, uint32_t nss);


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

static void convert_str_to_lower(char *src, char *dst)
{
	int i;
	for ( i = 0; i < (int)strnlen_s(src, MAX_STR_SIZE); i++ )
	{
		if( src[i] >= 'A' && src[i] <= 'Z' )
			dst[i] = src[i] - 'A' + 'a';
		else
			dst[i] = src[i];
	}
}

static void convertMac(char *sMac, unsigned char *cMac)
{
	int i;
	for (i = 0; i < 6; i++) {
		cMac[i] = (unsigned char) strtoul(sMac, &sMac, 16);
		sMac++;
	}
}

static unsigned int stat_ipow(unsigned int base, unsigned int power)
{
  unsigned int i;
  unsigned int res = 1;

  for (i = 0; i < power; i++)
  {
    res *= base;
  }

  return res;
}

static void print_type(type t, char *description, int len)
{
	int i, tmp = gSpace;
	char strbuf[32];

	if ( t != BITFIELD  && t != ENUM )
		INDENTATION(tmp)
	switch (t)
	{
		case RESERVED:
		{
			goutData = (void *)(goutData + len);
		}
		break;

		case STRING:
		{
			unsigned char *str = (unsigned char *)goutData;
			if (len > 0) {
				str[len - 1] = '\0';
				printf("%20s : %s\n", str , description);
				goutData = (void *)(goutData + len);
			}
		}
		break;

		case MACADDR:
		{
			IEEE_ADDR *mac = (IEEE_ADDR *)goutData;
			printf("   " MAC_PRINTF_FMT " : %s\n", MAC_PRINTF_ARG(mac->au8Addr) , description);
			goutData = (void *)(goutData + sizeof(IEEE_ADDR));
		}
		break;

		case BYTE:
		{
			printf("%20u : %s\n", *((unsigned char *)goutData) , description);
			goutData = (void *)(goutData + sizeof(unsigned char));
		}
		break;

		case HEXBYTE:
		{
			sprintf_s(strbuf, sizeof(strbuf), "0x%02X", *((unsigned char *)goutData));
			printf("%20s : %s\n", strbuf , description);
			goutData = (void *)(goutData + sizeof(unsigned char));
		}
		break;

		case HEXLONG:
		{
			sprintf_s(strbuf, sizeof(strbuf), "0x%08X", *((unsigned int *)goutData));
			printf("%20s : %s\n", strbuf , description);
			goutData = (void *)(goutData + sizeof(unsigned int));
		}
		break;

		case SLONG:
		{
			printf("%20i : %s\n", *((signed int *) goutData), description);
			goutData = (void *)(goutData + sizeof(signed int));
		}
		break;

		case LONG:
		{
			printf("%20u : %s\n", *((unsigned int *) goutData), description);
			goutData = (void *)(goutData + sizeof(unsigned int));
		}
 		break;

		case BYTEARRAY:
		{
			printf("%20s : %s\n","",description);
			for (i = 0; i < len; i++)
			{
				tmp = gSpace;
				INDENTATION(tmp)
				printf("%20i : [%d]\n",*((unsigned char *) goutData),i);
				goutData = (void *)(goutData + sizeof(unsigned char));
			}
		}
		break;

		case SBYTEARRAY:
		{
			printf("%20s : %s\n","",description);
			for (i = 0; i < len; i++)
			{
				tmp = gSpace;
				INDENTATION(tmp)
				printf("%20i : [%d]\n",*((signed char *) goutData),i);
				goutData = (void *)(goutData + sizeof(signed char));
			}
		}
		break;

		case SLONGARRAY:
		{
			printf("%20s : %s\n","",description);
			for (i = 0; i < len; i++)
			{
				tmp = gSpace;
				INDENTATION(tmp)
				printf("%20i : [%d]\n",*((signed int *) goutData),i);
				goutData = (void *)(goutData + sizeof(signed int));
			}
		}
		break;

		case LONGARRAY:
		{
			printf("%20s : %s\n","",description);
			for (i = 0; i < len; i++)
			{
				tmp = gSpace;
				INDENTATION(tmp)
				printf("%20i : [%d]\n",*((unsigned int *) goutData),i);
				goutData = (void *)(goutData + sizeof(unsigned int));
			}
		}
		break;

		case BITFIELD:
		{
			tmp = gSpace;
			if ( len )
				goutData = (void *)(goutData - sizeof(unsigned int));
			if (  ( *((unsigned  int *) goutData) & (0x01<<len) )) {
				INDENTATION(tmp);
				printf("%20s : \n",description);
			}
		}
		break;

		case BOOLEAN:
		{
			printf("%20s : %s\n", *((bool *) goutData) ? "True" : "False", description);
			goutData = (void *)(goutData + sizeof(bool));
		}
		break;

		case FLAG:
		{
			printf("%20s : %s\n", *((unsigned  int *) goutData)? "True" : "False",description);
			goutData = (void *)(goutData + sizeof(unsigned int));
		}
		break;

		case ENUM:
		{
			if (len)
			{
				goutData = (void *)(goutData - sizeof(unsigned int));
			}

			if (*((unsigned  int *) goutData) == (unsigned int)len) { 
				tmp = gSpace;
				INDENTATION(tmp)
				gEnum = 1;
				printf("%20s : ", description);
			}
			
		}
		break;

		case TIMESTAMP:
		{
			tmp = gSpace;
			INDENTATION(tmp)
			printf("%11d msec ago : %s\n",*((unsigned int *)goutData),description);
			goutData = (void *)(goutData + sizeof(unsigned int));
		}
		break;

		case LONGFRACT:
		{
                	tmp = gSpace;
	                INDENTATION(tmp)
  			if (len != 0)
			{
				unsigned int base = stat_ipow(10, len);
				unsigned int  re_value =  *(( unsigned int *)goutData) / base;
				unsigned int fract_value = *(( unsigned int *)goutData) % base;
				printf("%*u.%0*u : %s\n", 20 - len - 1, re_value, len, fract_value, description);
			}
			else
      				printf("%20u : %s\n", *(( unsigned int *)goutData), description);

			goutData = (void *)(goutData + sizeof(unsigned int));
		}
		break;

		case SLONGFRACT:
		{
	                tmp = gSpace;
        	        INDENTATION(tmp)
		  	if (len != 0)
			{
				unsigned int base = stat_ipow(10, len);
				int  re_value =  *(( signed int *)goutData) / base;
				int fract_value = *(( signed int *)goutData) % base;
      				printf("%*i.%0*i : %s\n", 20 - len - 1, re_value, len, fract_value, description);
			}
			else
      				printf("%20i : %s\n", *(( signed int *)goutData), description);

			goutData = (void *)(goutData + sizeof(signed int));
		}
		break;

		case HUGE:
		{
	                tmp = gSpace;
        	        INDENTATION(tmp)
			printf("%20lli : %s\n", *((signed long long *)goutData), description);
			goutData = (void *)(goutData + sizeof(signed long long));
		}
		break;

		default:
		break;
	}
}

static int NlEventCallback(char *ifname, int event, int subevent, size_t len, unsigned char *data)
{
	int i;

	(void)ifname;
	(void)event;
	(void)subevent;

	for (i=0; i < (int)len; i++)
		printf("%x ",data[i]);

	return 0;
}

static void help_print(stat_id c,bool original)
{
	int i,tmp=0;
	int status;

	for (i = 0; i < gStat[c].size; i++)
	{
		if ( !i && original )
			gSpace++;
		if ( gStat[c].sts[i].t != NONE )
		{
			print_type(gStat[c].sts[i].t, (char *)gStat[c].sts[i].description, gStat[c].sts[i].element);
			if (gStat[c].sts[i].t == BITFIELD || gStat[c].sts[i].t == ENUM)
				goutData = (void *)(goutData + sizeof(unsigned int));
		}
		else
		{
			tmp = gSpace;
				if ( gStat[c].sts[i].c == NETWORK_BITFIELD ) {
					INDENTATION(tmp)
					printf("%20s : %s\n","",gStat[c].sts[i].description);
				}
				else {
					if ( ( gStat[c].sts[i].c != PHY_ENUM ) && ( gStat[c].sts[i].c != VENDOR_ENUM ) ) {
						INDENTATION(tmp)
						PRINT_DESCRIPTION(gStat[c].sts[i])
					}
				}
		}

		if ( gStat[c].sts[i].c != c ) {
			if ( gStat[c].sts[i].c != NETWORK_BITFIELD )
				gSpace++;
			help_print(gStat[c].sts[i].c, false );
		}
	}
	if ( !original )
		gSpace--;
	i--;
	if ( ( gStat[c].sts[i].t == ENUM ) ) {
		char ptr[64]= {'\0'};
		switch ( gStat[c].sts[i].c )
		{
			case VENDOR_ENUM:
				status = sprintf_s(ptr, sizeof("Vendor"), "Vendor");
				if (status <= 0)
					console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
				break;
			case PHY_ENUM:
				status = sprintf_s(ptr, sizeof("Network (Phy) Mode"), "Network (Phy) Mode");
				if (status <= 0)
					console_printf("%s; sprintf_s failed!\n", __FUNCTION__);
				break;
			default:
				;
		}
		if ( !gEnum )
		{
			tmp = gSpace;
			INDENTATION(tmp); 
			printf("%20s : %s\n","Unknown value",ptr);
		}
		else {
			printf("%s\n",ptr);
			gEnum = 0;
		}
	}	
	
}

static void dump_sta_list(char *outData, unsigned int outLen)
{
  unsigned int sta_number = outLen / sizeof(peer_list_t);
  peer_list_t *sta = (peer_list_t *)outData;

  if (sta_number > 0)
  {
    unsigned int i;
    fprintf(stdout, "\n\n%u peer(s) connected:\n\n", sta_number);
    for (i = 0; i < sta_number; i++)
    {
      if (sta->is_sta_auth)
      {
        fprintf(stdout, "\t" MAC_PRINTF_FMT "\n", MAC_PRINTF_ARG(&sta->addr));
      }
      else
      {
        fprintf(stdout, "\t" MAC_PRINTF_FMT " (not authorized)\n", MAC_PRINTF_ARG(&sta->addr));
      }
	sta++;
    }
  }
  else
  {
    fprintf(stdout, "\n\nNo peers connected.\n\n");
  }

  fprintf(stdout, "\n");
}


static void print_cmd_help( char *cmd )
{
	int i, found = 0;

	if ( !cmd )
		printf("\n\t Help for supported statistics are:");

	for ( i=0; i < (int)(sizeof(gCmd)/sizeof(gCmd[0])); i++ )
	{
		if ( cmd )
		{
			if ( !strncmp(cmd, gCmd[i].cmd, strnlen_s(gCmd[i].cmd, MAX_STR_SIZE)) )
			{
				printf("\n\t Help for %s statistics is:",cmd);
				printf("\n\t %s",gCmd[i].usage);
				found = 1;
			}
		}
		else
			printf("\n\t %s",gCmd[i].usage);
	}

	if ( !cmd || found )
	{
		printf("\n\t Note:\n\t\t INTERFACENAME can be wlan0,wlan0.0,wlan2,wlan2.0...");
		printf("\n\t\t MACADDR corresponds to macaddr of connected station\n");
	}
}

int check_and_dump_packet_error_rate(char *cmd[])
{
	unsigned int outLen,i;
	char filename[MAX_FILE_NAME] = {'\0'};
	char input_cmd[MAX_STR_SIZE] = { '\0' };
	packet_error_rate_t *stats_array = NULL;
	int max_size = MAX_NUM_OF_RATES * sizeof(packet_error_rate_t), res;
	unsigned int output_array_size;
	char *gi_str[] = { "400", "800", "800(2*LTF)", "1600", "800(4*LTF)", "3200"};

	convert_str_to_lower(cmd[1],input_cmd);

	res = sprintf_s(filename, sizeof(filename), "%s/sta_per_rate_statistics/PER/%s", cmd[0], cmd[2]);
	if (res < 0 || res >= MAX_FILE_NAME)
		return DWPAL_FAILURE;

	stats_array = malloc(max_size);
	if ( (stats_array) == NULL )
		return DWPAL_FAILURE;

	if ( DWPAL_SUCCESS != dwpal_proc_file_copy(stats_array,
				&outLen, max_size, filename) ) {
		free(stats_array);
		return DWPAL_FAILURE;
	}

	output_array_size = (outLen)/sizeof(packet_error_rate_t);

	fprintf(stdout, "Packet Error Rate (PER): For each combination of MCS, NSS, GI and BW\n\n");
	fprintf(stdout, "|-----------------------------------------------------------|\n");
	fprintf(stdout, "|  MCS  |  NSS  | GI (In nsec) |   BW  | PER (In percent) |\n");
	for( i = 0; i < output_array_size; i++ )
	{
		fprintf(stdout, "|-----------------------------------------------------------|\n");
		fprintf(stdout, "|  %3u  |  %3u  | %12s |  %3u  | %16u |\n", stats_array[i].mcs, stats_array[i].nss,
				 gi_str[stats_array[i].gi], stats_array[i].bw, stats_array[i].per);
	}
	fprintf(stdout, "|-----------------------------------------------------------|\n");
	if (stats_array)
		free(stats_array);
	return DWPAL_SUCCESS;
}

int check_stats_cmd(int num_arg, char *cmd[])
{
	unsigned char outData[MAX_NL_REPLY] = { '\0' };
	unsigned int i;
	size_t outLen;
	unsigned char Vendordata[128] = {'\0'};
	int VendorDataLen = 0;
	char input_cmd[MAX_STR_SIZE] = { '\0' };
	size_t *pOutLen = &outLen;
	unsigned char *pOutData = outData;

	if ( !strncmp(cmd[0],"help",sizeof("help")-1) ) {
		if ( num_arg > 1 )
			print_cmd_help(cmd[1]);
		else
			print_cmd_help(NULL);
		return 0;
	}

	if ( num_arg <  2) {
		PRINT_ERROR(" Need atleaset 2 arguments ie interfacename and command\n");
		return -1;
	}
	
	convert_str_to_lower(cmd[1],input_cmd);
	
	for ( i = 0; i < sizeof(gCmd)/sizeof(gCmd[0]); i++ ) 
	{
		if ( !strncmp(input_cmd, gCmd[i].cmd, strnlen_s(gCmd[i].cmd,MAX_STR_SIZE)) ) {
			if ( num_arg -1 != gCmd[i].num_arg ) {
				printf("%s\n",(char *)&gCmd[i].usage);
				return -1;
			}
			if ( gCmd[i].c == PACKET_ERROR_RATE ) {
				if (check_and_dump_packet_error_rate(cmd) == DWPAL_FAILURE)
				{
					PRINT_ERROR("%s; process_packet_error_rate_cmd failed\n", __FUNCTION__);
					return DWPAL_FAILURE;
				}
                                return DWPAL_SUCCESS;
			}
			if (gCmd[i].id == LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_MSR_OFF_CHAN) {
				/* one param as unsigned int */
				unsigned int *pvd = (unsigned int *)Vendordata;
				unsigned int param = strtoul(cmd[2], NULL, 10);
				*pvd = param;
				VendorDataLen = sizeof(param);
			} else if (gCmd[i].id == LTQ_NL80211_VENDOR_SUBCMD_SET_START_CCA_MSR_OFF_CHAN) {
				/* two params as unsigned int */
				unsigned int *pvd = (unsigned int *)Vendordata;
				unsigned int param1 = strtoul(cmd[2], NULL, 10);
				unsigned int param2 = strtoul(cmd[3], NULL, 10);
				*pvd++ = param1;
				*pvd = param2;
				VendorDataLen = sizeof(param1)+sizeof(param2);
				/* no reply data */
				pOutLen = NULL;
				pOutData = NULL;
			} else if ( num_arg > 2 ) {
				/* 6 bytes of MAC address */
				convertMac(cmd[2],Vendordata);
				VendorDataLen = 6;
				/* PRINT_DEBUG("Vendordata %s\n",Vendordata); */
			}
			if (dwpal_ext_driver_nl_attach(NlEventCallback, NULL) == DWPAL_FAILURE)
			{
				PRINT_ERROR("%s; dwpal_driver_nl_attach returned ERROR ==> Abort!\n", __FUNCTION__);
				return DWPAL_FAILURE;
			}
			if (dwpal_ext_driver_nl_get(cmd[0], NL80211_CMD_VENDOR, DWPAL_NETDEV_ID, gCmd[i].id ,\
				 Vendordata, VendorDataLen, pOutLen, pOutData) != DWPAL_SUCCESS)
			{
				PRINT_ERROR("%s; dwpal_ext_driver_nl_get returned ERROR ==> Abort!\n", __FUNCTION__);
				dwpal_ext_driver_nl_detach();
				return -1;
			}

			if (gCmd[i].id == LTQ_NL80211_VENDOR_SUBCMD_SET_START_CCA_MSR_OFF_CHAN) {
				/* no reply data */
				goto end;
			}

			if ( outLen )
				goutData = outData;
			else
			{
				dwpal_ext_driver_nl_detach();
				return -1;
			}

			if ( gCmd[i].c != PEER_LIST )
				help_print( gCmd[i].c, true );
			else
				dump_sta_list((char *)outData, outLen);
			break;
		}
	}

end:
	dwpal_ext_driver_nl_detach();
	goutData = NULL;
	return 0;
}



#ifdef CTL_SCAN_STATS
bool
is_valid_vht_rate_params(uint32_t cbw, uint32_t mcs, uint32_t nss)
{
	if (((nss != 3 && nss != 6) && cbw == 0 && mcs == 9) ||
		(nss == 3 && cbw == 2 && mcs == 6) ||
		(nss == 7 && cbw == 2 && mcs == 6) ||
		(nss == 6 && cbw == 2 && mcs == 9) ||
		(nss == 3 && cbw == 3 && mcs == 9)) {
			return false;
	}
	return true;
}

static uint32_t
_hw_rate_params_to_rate (uint32_t mode, uint32_t cbw, uint32_t scp, uint32_t mcs, uint32_t nss)
{
	uint32_t		rate = BITRATE_INVALID;
	uint32_t		idx;

	switch(mode) {
	case PHY_MODE_HT:
		if (cbw < 2 && mcs < NUM_RATE_MCS_N && nss <= HT_PHY_STAT_RATE_NSS_MAX) { /* CBW 0/1, MCS <= 32 */
			if (mcs == BITRATE_MCS32) {
				idx	= scp | (cbw << 1);
				rate = bitrates_11n_mcs32[idx];
			} else if (mcs <= 7) {
				idx	= scp | (cbw << 1) | (mcs << 2);
				rate = bitrates_11n[idx];
			}
		}
		break;

	case PHY_MODE_VHT: /* CBW 0/1/2/3 */
		if (mcs < NUM_RATE_MCS_AC && nss <= PHY_STAT_RATE_NSS_MAX && nss >= PHY_STAT_RATE_NSS_MIN) {
			if (is_valid_vht_rate_params(cbw, mcs, nss))	{
				idx	= scp | (cbw << 1) | (mcs << 3);
				rate = bitrates_11ac[idx];
			}
		}
		break;

	case PHY_MODE_HE: /* CBW 0/1/2/3 */
		/* TODO: scp to cp_mode conversion */
		if ((scp < HE_CP_MODES_NUM) && (mcs < NUM_RATE_MCS_AX) && (nss > 0) && (nss <= PHY_STAT_RATE_NSS_MAX)) {
			//uint8 cp_type = he_cp_mode_to_cp_type_table[scp];
			idx = scp + (HE_CP_TYPES_NUM * (cbw + (PHY_STAT_RATE_CBW_NUM * mcs)));
			rate = bitrates_11ax[idx];
		}
		break;
	default:
		break;
	};

	return rate;
}

void parse_scan_result(struct nl_msg *msg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *bss[NL80211_BSS_MAX + 1];
	char mac_addr[20];
	int scan_band = 0;
	static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
		[NL80211_BSS_TSF] = { .type = NLA_U64 },
		[NL80211_BSS_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_BSS_BSSID] = { },
		[NL80211_BSS_BEACON_INTERVAL] = { .type = NLA_U16 },
		[NL80211_BSS_CAPABILITY] = { .type = NLA_U16 },
		[NL80211_BSS_INFORMATION_ELEMENTS] = { },
		[NL80211_BSS_SIGNAL_MBM] = { .type = NLA_U32 },
		[NL80211_BSS_SIGNAL_UNSPEC] = { .type = NLA_U8 },
		[NL80211_BSS_STATUS] = { .type = NLA_U32 },
		[NL80211_BSS_SEEN_MS_AGO] = { .type = NLA_U32 },
		[NL80211_BSS_BEACON_IES] = { },
		[NL80211_BSS_NOISE] = { .type = NLA_U8 },
	};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_BSS]) {
		fprintf(stderr, "bss info missing!\n");
		return;
	}

	if (nla_parse_nested(bss, NL80211_BSS_MAX,
					 tb[NL80211_ATTR_BSS],
					 bss_policy)) {
		fprintf(stderr, "failed to parse nested attributes!\n");
		return;
	}

	if (!bss[NL80211_BSS_BSSID])
		return;

	mac_addr_n2a(mac_addr, sizeof(mac_addr), nla_data(bss[NL80211_BSS_BSSID]));
	printf("BSS %s", mac_addr);

	if (bss[NL80211_BSS_NOISE]) {
		char n = nla_get_u8(bss[NL80211_BSS_NOISE]);
		printf("\tnoise: %d.00 dBm", n);
	}

	if (bss[NL80211_BSS_FREQUENCY]) {
		int freq = nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
		if (freq < 5035)
			scan_band = HW_BAND_2_4_GHZ;
		else
			scan_band = HW_BAND_5_2_GHZ;
	}


	if (bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		calculate_phy_rate(scan_band, nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]),
				 nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]));
	} else if (bss[NL80211_BSS_BEACON_IES]) {
		calculate_phy_rate(scan_band, nla_data(bss[NL80211_BSS_BEACON_IES]),
				nla_len(bss[NL80211_BSS_BEACON_IES]));
	}

	printf("\n\n");
	return;
}

struct ieee80211_mcs_info {
	u8 rx_mask[IEEE80211_HT_MCS_MASK_LEN];
	__le16 rx_highest;
	u8 tx_params;
	u8 reserved[3];
} __attribute__ ((packed));

struct ieee80211_ht_cap {
	u16 cap; /* use IEEE80211_HT_CAP_ */
	u8 ampdu_params;
	struct ieee80211_mcs_info mcs;
	__le16 ht_extended_capabilities;
	__le32 tx_BF_cap_info;
	u8 asel_capabilities;
} __attribute__ ((packed));

struct ieee80211_vht_mcs_info {
	__le16 rx_mcs_map;
	__le16 rx_highest;
	__le16 tx_mcs_map;
	__le16 tx_highest;
} __attribute__ ((packed));

struct ieee80211_vht_cap {
	__le32 cap; /* use IEEE80211_VHT_CAP_ */
	struct ieee80211_vht_mcs_info vht_mcs;
} __attribute__ ((packed));

struct ieee80211_he_cap_elem {
	u8 mac_cap_info[6];
	u8 phy_cap_info[11];
	/*
	 * Followed by 4, 8, or 12 octets of Supported HE-MCS And NSS
	 * Set field and optional variable length PPE Thresholds field.
	 */
	u8 optional[0];
} __attribute__ ((packed));

struct ieee80211_he_mcs_nss_supp {
	u16 rx_mcs_80;
	u16 tx_mcs_80;
	u16 rx_mcs_160;
	u16 tx_mcs_160;
	u16 rx_mcs_80p80;
	u16 tx_mcs_80p80;
} __attribute__ ((packed));

struct ieee80211_he_cap {
	struct ieee80211_he_cap_elem he_cap_elem;
	struct ieee80211_he_mcs_nss_supp he_mcs_nss_supp;
	u8 ppe_thres[IEEE80211_HE_PPE_THRES_MAX_LEN];
} __attribute__ ((packed));

enum ieee80211_rx_bandwidth {
	IEEE80211_BW_20 = 0,
	IEEE80211_BW_40,
	IEEE80211_BW_80,
	IEEE80211_BW_160,
};

enum ieee80211_vht_mcs_support {
	IEEE80211_VHT_MCS_SUPPORT_0_7 = 0,
	IEEE80211_VHT_MCS_SUPPORT_0_8 = 1,
	IEEE80211_VHT_MCS_SUPPORT_0_9 = 2,
	IEEE80211_VHT_MCS_NOT_SUPPORTED = 3,
};

enum ieee80211_he_mcs_support {
	IEEE80211_HE_MCS_SUPPORT_0_7	= 0,
	IEEE80211_HE_MCS_SUPPORT_0_9	= 1,
	IEEE80211_HE_MCS_SUPPORT_0_11 = 2,
	IEEE80211_HE_MCS_NOT_SUPPORTED	= 3,
};

void parse_ies(unsigned char *ie, int ielen,
				uint8_t *supp_rates,
				uint8_t **ht_cap,
				uint8_t **vht_cap,
				uint8_t **he_cap)
{
	int supp_rates_len = 0;
	while (ielen >= 2 && ielen >= ie[1]) {
		if (ie[0] == WLAN_EID_SUPP_RATES || ie[0] == WLAN_EID_EXT_SUPP_RATES) {
			int remain_len = (MAX_NUM_SUPPORTED_RATES - supp_rates_len);
			int len = (remain_len > ie[1]) ? ie[1]: remain_len;
			if (len > 0) {
				memcpy_s(supp_rates + supp_rates_len, remain_len, ie + 2, len);
				supp_rates_len = len;
			}
		} else if (ie[0] == WLAN_EID_HT_CAP &&
					ie[1] == sizeof(struct ieee80211_ht_cap)) {
			*ht_cap = &ie[2];
		} else if (ie[0] == WLAN_EID_VHT_CAP &&
					ie[1] == sizeof(struct ieee80211_vht_cap)) {
			*vht_cap = &ie[2];
		} else if (ie[0] == WLAN_EID_EXTENSION &&
					 ie[2] == WLAN_EID_EXT_HE_CAPABILITIES) {
			if (ie[1] >= (sizeof(struct ieee80211_he_cap_elem) +
				((sizeof(struct ieee80211_he_mcs_nss_supp))/3) + 1)) {
				*he_cap = &ie[3];
				return;
			}

		}
		ielen -= ie[1] + 2;
		ie += ie[1] + 2;
	}
}

void
calculate_phy_rate(int band, unsigned char *ie, int ielen)
{
	struct ieee80211_ht_cap *ht_cap_ie = NULL;
	struct ieee80211_vht_cap *vht_cap_ie = NULL;
	struct ieee80211_he_cap *he_cap_ie = NULL;
	uint8_t	rates[MAX_NUM_SUPPORTED_RATES];

	uint32_t max_rate = 0;
	uint32_t ht_bw = IEEE80211_BW_20;
	uint32_t ht_scp = 0;

	memset(rates, 0, sizeof(rates));
	parse_ies(ie, ielen, rates, (uint8_t**) &ht_cap_ie, (uint8_t**) &vht_cap_ie,
					 (uint8_t**) &he_cap_ie);

	/* Get max non-HT rate */
	if (rates[0] > 0) {
		int i;
		unsigned	supp_rate = 0;
		for (i = 0; i < MAX_NUM_SUPPORTED_RATES; i++) {
			supp_rate = MAX(supp_rate, rates[i] & 0x7f);
		}
		max_rate = SUPP_RATE_TO_BITRATE(supp_rate);
	}

	/* Get max HT rate */
	if (ht_cap_ie) {
		int i,y;
		uint32_t ht_nss = 0;
		uint32_t ht_mcs = 0;
		uint16_t ht_cap;
		uint32_t ht_max_phy_rate = 0;
		uint32_t ht_1ss_phy_rate = 0;

		ht_cap = le16toh(ht_cap_ie->cap);

		if ((ht_cap & IEEE80211_HT_CAP_SUP_WIDTH_20_40) &&
			!(ht_cap & IEEE80211_HT_CAP_40MHZ_INTOLERANT)) {
			ht_bw = IEEE80211_BW_40;
			if (ht_cap & IEEE80211_HT_CAP_SGI_40)
				ht_scp = 1;
		} else {
			ht_bw = 0;
			if (ht_cap & IEEE80211_HT_CAP_SGI_20)
				ht_scp = 1;
		}

		/* BITRATE_MCS32 for BPSK */
		if (ht_cap_ie->mcs.rx_mask[4] & 0x01) {
			if (ht_bw == IEEE80211_BW_40) {
				ht_nss = 1;
				ht_mcs = BITRATE_MCS32;

				ht_1ss_phy_rate = _hw_rate_params_to_rate(PHY_MODE_HT, ht_bw, ht_scp,
						 BITRATE_MCS32,	1);
				ht_max_phy_rate = ht_1ss_phy_rate * ht_nss;
				if (max_rate < ht_max_phy_rate) {
					max_rate = ht_max_phy_rate;
				}
			}
		}

		for (i = HT_PHY_STAT_RATE_NSS_MAX - 1; i >= 0; i--) { /* we support max 4 nss */
			uint8_t rx_mask = ht_cap_ie->mcs.rx_mask[i];
			if (rx_mask) {
				ht_nss = i + 1;
				for (y = 7; y >= 0; y--) {
					if (((rx_mask >> y) & 0x1) != 0) {
						ht_mcs = y + i*8;
						break;
					}
				}
				break;
			}
		}

		ht_1ss_phy_rate = _hw_rate_params_to_rate(PHY_MODE_HT, ht_bw, ht_scp,
				(ht_mcs == BITRATE_MCS32) ? BITRATE_MCS32 : (ht_mcs & 7), 1);
		ht_max_phy_rate = ht_1ss_phy_rate * ht_nss;
		if (max_rate < ht_max_phy_rate) {
			max_rate = ht_max_phy_rate;
		}
	}

	/* Get max VHT rate */
	if (vht_cap_ie) {
		uint32_t vht_nss = 0;
		uint32_t vht_mcs = 0;
		uint16_t vht_tx_mcs_map = 0;
		uint32_t vht_max_phy_rate = 0;
		uint32_t vht_1ss_phy_rate = 0;

		vht_tx_mcs_map = le16toh(vht_cap_ie->vht_mcs.rx_mcs_map);
		for (vht_nss = PHY_STAT_RATE_NSS_MAX; vht_nss > 0; vht_nss--) {
			vht_mcs = ((vht_tx_mcs_map >> (2 * (vht_nss - 1))) & 3);
			if (vht_mcs != IEEE80211_VHT_MCS_NOT_SUPPORTED)
				break;
		}

		if (vht_mcs != IEEE80211_VHT_MCS_NOT_SUPPORTED) {
			uint32_t vht_cap;
			uint32_t vht_bw;
			uint32_t vht_scp = 0;
			uint32_t mcs_1ss;

			vht_mcs = 7 + vht_mcs;
			vht_cap = le32toh(vht_cap_ie->cap);

			if (HW_BAND_2_4_GHZ == band) {
				vht_bw = ht_bw;
				vht_scp = ht_scp;
			} else {
				switch (vht_cap & IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK) {
				case IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ:
					vht_bw = IEEE80211_BW_160;
					if (vht_cap & IEEE80211_VHT_CAP_SHORT_GI_160)
						vht_scp = 1;
					break;
				default:
					vht_bw = IEEE80211_BW_80;
					if (vht_cap & IEEE80211_VHT_CAP_SHORT_GI_80)
						vht_scp = 1;
				}
			}

			/* Look for max bitrate supported */
			for (mcs_1ss = vht_mcs; mcs_1ss >= 7; mcs_1ss--) {
				if ((vht_1ss_phy_rate = _hw_rate_params_to_rate(PHY_MODE_VHT, vht_bw, vht_scp, mcs_1ss, 1))
						!= BITRATE_INVALID)
					break;
			}

			vht_max_phy_rate = vht_1ss_phy_rate * vht_nss;
			if (max_rate < vht_max_phy_rate) {
				max_rate = vht_max_phy_rate;
			}
		}
	}

	/* Get max HE rate */
	if (he_cap_ie) {
		uint32_t he_nss = 0;
		uint32_t he_mcs = 0;
		uint16_t he_tx_mcs_map = 0;
		uint8_t	*he_phy_cap_info;
		uint32_t he_max_phy_rate = 0;
		uint32_t he_1ss_phy_rate = 0;
		uint32_t he_bw= IEEE80211_BW_20;
		uint32_t he_scp = 0;

		he_phy_cap_info = he_cap_ie->he_cap_elem.phy_cap_info;

		/* Process BW */
		if (HW_BAND_2_4_GHZ == band) {
			if (he_phy_cap_info[0] & IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G)
			 he_bw = IEEE80211_BW_40;
			else
			 he_bw = IEEE80211_BW_20;
		} else {
			if (he_phy_cap_info[0] & IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G ||
					he_phy_cap_info[0] & IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G)
				he_bw = IEEE80211_BW_160;
			else if (he_phy_cap_info[0] & IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G)
				he_bw = IEEE80211_BW_80;
			else
				he_bw = IEEE80211_BW_20;
		}

		if (he_bw == IEEE80211_BW_160)
			he_tx_mcs_map = le16toh(he_cap_ie->he_mcs_nss_supp.rx_mcs_160);
		else
			he_tx_mcs_map = le16toh(he_cap_ie->he_mcs_nss_supp.rx_mcs_80);

		for (he_nss = PHY_STAT_RATE_NSS_MAX; he_nss > 0; he_nss--) {
			he_mcs = ((he_tx_mcs_map >> (2 * (he_nss - 1))) & 3);
			 if (he_mcs != IEEE80211_HE_MCS_NOT_SUPPORTED)
				 break;
		 }

		if (he_mcs != IEEE80211_HE_MCS_NOT_SUPPORTED) {
			he_mcs = 7 + he_mcs * 2;
			he_1ss_phy_rate = _hw_rate_params_to_rate(PHY_MODE_HE, he_bw, he_scp, he_mcs, 1);
			he_max_phy_rate = he_1ss_phy_rate * he_nss;

			if (max_rate < he_max_phy_rate) {
				max_rate = he_max_phy_rate;
			}
		}

	}

	printf("\tphy_rate (max): %u.%u Mbps\n", max_rate/10, max_rate%10);
}

void mac_addr_n2a(char *mac_addr, size_t addr_size, unsigned char *arg)
{
#define ETH_ALEN 6
	int i, l;

	l = 0;
	for (i = 0; i < ETH_ALEN ; i++) {
		if (i == 0) {
			sprintf_s(mac_addr + l, addr_size - l, "%02x", arg[i]);
			l += 2;
		} else {
			sprintf_s(mac_addr + l, addr_size - l, ":%02x", arg[i]);
			l += 3;
		}
	}
}
#endif
