/*
 * chlist.c - file implements valid wifi channel list
 *
 * Valid channels are dependant on a number of parameters -
 * regulatory domain, country, dfs, bandwidth.
 * The API wifi_get_valid_channels() returns valid channels for any
 * combination of these parameters.
 *
 * Copyright (C) 2018 Inteno Broadband Technology AB. All rights reserved.
 *
 * Author: anjan.chanda@inteno.se
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"

struct chlist {
	int start;
	int num;
};

struct region_to_chlist {
	int reg;
	struct chlist *list;
};

/* channel filter criteria */
#define FILTER_TDWR	0x1
#define FILTER_DFS	0x2

#define G_REG_0                  0	/* 1-11 */
#define G_REG_1                  1	/* 1-13 */
#define G_REG_2                  2	/* 10-11 */
#define G_REG_3                  3	/* 10-13 */
#define G_REG_4                  4	/* 14 */
#define G_REG_5                  5	/* 1-14 */
#define G_REG_6                  6	/* 3-9 */
#define G_REG_7                  7	/* 5-13 */

struct chlist g_reg0_chlist[] = {
	{1, 11}, {0, 0}
};

struct chlist g_reg1_chlist[] = {
	{1, 13}, {0, 0}
};

struct chlist g_reg2_chlist[] = {
	{10, 2}, {0, 0}
};

struct chlist g_reg3_chlist[] = {
	{10, 4}, {0, 0}
};

struct chlist g_reg4_chlist[] = {
	{14, 1}, {0, 0}
};

struct chlist g_reg5_chlist[] = {
	{1, 14}, {0, 0}
};

struct chlist g_reg6_chlist[] = {
	{3, 7}, {0, 0}
};

struct chlist g_reg7_chlist[] = {
	{5, 9}, {0, 0}
};

static struct region_to_chlist g_region_chlist[] = {
	{0, g_reg0_chlist},
	{1, g_reg1_chlist},
	{2, g_reg2_chlist},
	{3, g_reg3_chlist},
	{4, g_reg4_chlist},
	{5, g_reg5_chlist},
	{6, g_reg6_chlist},
	{7, g_reg7_chlist},
};

#define g_region_chlist_num (sizeof(g_region_chlist)/sizeof(g_region_chlist[0]))


/* 36 ~ 64, 149 ~ 165 */
#define A_REG_0                   0

/* 36 ~ 64, 100 ~ 140 */
#define A_REG_1                   1

/* 36 ~ 64 */
#define A_REG_2                   2

/* 52 ~ 64, 149 ~ 161 */
#define A_REG_3                   3

/* 149 ~ 165 */
#define A_REG_4                   4

/* 149 ~ 161 */
#define A_REG_5                   5

/* 36 ~ 48 */
#define A_REG_6                   6

/* 36 ~ 64, 100 ~ 140, 149 ~ 173 */
#define A_REG_7                   7

/* 52 ~ 64 */
#define A_REG_8                   8

/* 36 ~ 64, 100 ~ 140, 149 ~ 165 */
#define A_REG_9                   9

/* 36 ~ 48, 149 ~ 165 */
#define A_REG_10                  10

/* 36 ~ 64, 100 ~ 120, 149 ~ 161 */
#define A_REG_11                  11

/* 36 ~ 64, 100 ~ 140 */
#define A_REG_12                  12

/* 52 ~ 64, 100 ~ 140, 149 ~ 161 */
#define A_REG_13                  13

/* 36 ~ 64, 100 ~ 140, 149 ~ 165 */
#define A_REG_14                  14


struct chlist a_reg0_chlist[] = {
	{36, 8}, {149, 5}, {0, 0}
};

struct chlist a_reg1_chlist[] = {
	{36, 8}, {100, 11}, {0, 0}
};

struct chlist a_reg2_chlist[] = {
	{36, 8}, {0, 0}
};

struct chlist a_reg3_chlist[] = {
	{52, 4}, {149, 4}, {0, 0}
};

struct chlist a_reg4_chlist[] = {
	{149, 5}, {0, 0}
};

struct chlist a_reg5_chlist[] = {
	{149, 4}, {0, 0}
};

struct chlist a_reg6_chlist[] = {
	{36, 4}, {0, 0}
};

struct chlist a_reg7_chlist[] = {
	{36, 8}, {100, 11}, {149, 7}, {0, 0}
};

struct chlist a_reg8_chlist[] = {
	{52, 4}, {0, 0}
};

struct chlist a_reg9_chlist[] = {
	{36, 8}, {100, 5}, {132, 8}, {0, 0}
};

struct chlist a_reg10_chlist[] = {
	{36, 4}, {149, 5}, {0, 0}
};


static struct region_to_chlist a_region_chlist[] = {
	{0, a_reg0_chlist},
	{1, a_reg1_chlist},
	{2, a_reg2_chlist},
	{3, a_reg3_chlist},
	{4, a_reg4_chlist},
	{5, a_reg5_chlist},
	{6, a_reg6_chlist},
	{7, a_reg7_chlist},
	{8, a_reg8_chlist},
	{9, a_reg9_chlist},
	{10, a_reg10_chlist},
};

#define a_region_chlist_num (sizeof(a_region_chlist)/sizeof(a_region_chlist[0]))

static const uint32_t def_chlist_a[] = {36, 40, 44, 48,
					52, 56, 60, 64,
					100, 104, 108, 112,
					116, 120, 124, 128,
					132, 136, 140,
					149, 153, 157, 161, 165, 169, 173};

static const uint32_t def_chlist_a_num = sizeof(def_chlist_a)/sizeof(uint32_t);

static const uint32_t def_chlist_6[] = {1, 5, 9, 13,
					17, 21, 25, 29,
					33, 37, 41, 45,
					49, 53, 57, 61,
					65, 69, 73, 77,
					81, 85, 89, 93,
					97, 101, 105, 109,
					113, 117, 121, 125,
					129, 133, 137, 141,
					145, 149, 153, 157,
					161, 165, 169, 173,
					177, 181, 185, 189,
					193, 197, 201, 205,
					209, 213, 217, 221,
					225, 229, 233};

static const uint32_t def_chlist_6_num = sizeof(def_chlist_6)/sizeof(uint32_t);

static const uint32_t def_chlist_g[] = {1, 2, 3, 4, 5, 6, 7,
					8, 9, 10, 11, 12, 13, 14};

static const uint32_t def_chlist_g_num = sizeof(def_chlist_g)/sizeof(uint32_t);



struct country_chmap {
	char cc[3];
	char cname[64];
	int region_a;
	int region_g;
};


struct country_chmap chmaps[] = {
	{"DB",	"Debug",				A_REG_7, G_REG_5},
	{"AL",	"ALBANIA",				A_REG_0, G_REG_1},
	{"DZ",	"ALGERIA",				A_REG_0, G_REG_1},
	{"AR",	"ARGENTINA",				A_REG_3, G_REG_1},
	{"AM",	"ARMENIA",				A_REG_2, G_REG_1},
	{"AU",	"AUSTRALIA",				A_REG_0, G_REG_1},
	{"AT",	"AUSTRIA",				A_REG_1, G_REG_1},
	{"AZ",	"AZERBAIJAN",				A_REG_2, G_REG_1},
	{"BH",	"BAHRAIN",				A_REG_0, G_REG_1},
	{"BY",	"BELARUS",				A_REG_0, G_REG_1},
	{"BE",	"BELGIUM",				A_REG_1, G_REG_1},
	{"BZ",	"BELIZE",				A_REG_4, G_REG_1},
	{"BO",	"BOLIVIA",				A_REG_4, G_REG_1},
	{"BR",	"BRAZIL",				A_REG_1, G_REG_1},
	{"BN",	"BRUNEI DARUSSALAM",			A_REG_4, G_REG_1},
	{"BG",	"BULGARIA",				A_REG_1, G_REG_1},
	{"CA",	"CANADA",				A_REG_9, G_REG_0},
	{"CL",	"CHILE",				A_REG_0, G_REG_1},
	{"CN",	"CHINA",				A_REG_4, G_REG_1},
	{"CO",	"COLOMBIA",				A_REG_0, G_REG_0},
	{"CR",	"COSTA RICA",				A_REG_0, G_REG_1},
	{"HR",	"CROATIA",				A_REG_2, G_REG_1},
	{"CY",	"CYPRUS",				A_REG_1, G_REG_1},
	{"CZ",	"CZECH REPUBLIC",			A_REG_2, G_REG_1},
	{"DK",	"DENMARK",				A_REG_1, G_REG_1},
	{"DO",	"DOMINICAN REPUBLIC",			A_REG_0, G_REG_0},
	{"EC",	"ECUADOR",				A_REG_0, G_REG_1},
	{"EG",	"EGYPT",				A_REG_2, G_REG_1},
	{"SV",	"EL SALVADOR",				A_REG_0, G_REG_1},
	{"EE",	"ESTONIA",				A_REG_1, G_REG_1},
	{"FI",	"FINLAND",				A_REG_1, G_REG_1},
	{"FR",	"FRANCE",				A_REG_2, G_REG_1},
	{"GE",	"GEORGIA",				A_REG_2, G_REG_1},
	{"DE",	"GERMANY",				A_REG_1, G_REG_1},
	{"EU",	"EUROPEAN UNION",			A_REG_1, G_REG_1},
	{"GR",	"GREECE",				A_REG_1, G_REG_1},
	{"GT",	"GUATEMALA",				A_REG_0, G_REG_0},
	{"HN",	"HONDURAS",				A_REG_0, G_REG_1},
	{"HK",	"HONG KONG",				A_REG_0, G_REG_1},
	{"HU",	"HUNGARY",				A_REG_1, G_REG_1},
	{"IS",	"ICELAND",				A_REG_1, G_REG_1},
	{"IN",	"INDIA",				A_REG_9, G_REG_1},
	{"ID",	"INDONESIA",				A_REG_4, G_REG_1},
	{"IR",	"IRAN",					A_REG_4, G_REG_1},
	{"IE",	"IRELAND",				A_REG_1, G_REG_1},
	{"IL",	"ISRAEL",				A_REG_0, G_REG_1},
	{"IT",	"ITALY",				A_REG_1, G_REG_1},
	{"JP",	"JAPAN",				A_REG_9, G_REG_1},
	{"JO",	"JORDAN",				A_REG_0, G_REG_1},
	{"KZ",	"KAZAKHSTAN",				A_REG_0, G_REG_1},
	{"KP",	"KOREA DEMOCRATIC PEOPLE'S REPUBLIC OF", A_REG_5, G_REG_1},
	{"KR",	"KOREA REPUBLIC OF",			A_REG_5, G_REG_1},
	{"KW",	"KUWAIT",				A_REG_0, G_REG_1},
	{"LV",	"LATVIA",				A_REG_1, G_REG_1},
	{"LB",	"LEBANON",				A_REG_0, G_REG_1},
	{"LI",	"LIECHTENSTEIN",			A_REG_1, G_REG_1},
	{"LT",	"LITHUANIA",				A_REG_1, G_REG_1},
	{"LU",	"LUXEMBOURG",				A_REG_1, G_REG_1},
	{"MO",	"MACAU",				A_REG_0, G_REG_1},
	{"MK",	"MACEDONIA",				A_REG_0, G_REG_1},
	{"MY",	"MALAYSIA",				A_REG_0, G_REG_1},
	{"MX",	"MEXICO",				A_REG_0, G_REG_0},
	{"MC",	"MONACO",				A_REG_2, G_REG_1},
	{"MA",	"MOROCCO",				A_REG_0, G_REG_1},
	{"NL",	"NETHERLANDS",				A_REG_1, G_REG_1},
	{"NZ",	"NEW ZEALAND",				A_REG_0, G_REG_1},
	{"NO",	"NORWAY",				A_REG_0, G_REG_0},
	{"OM",	"OMAN",					A_REG_0, G_REG_1},
	{"PK",	"PAKISTAN",				A_REG_0, G_REG_1},
	{"PA",	"PANAMA",				A_REG_0, G_REG_0},
	{"PE",	"PERU",					A_REG_4, G_REG_1},
	{"PH",	"PHILIPPINES",				A_REG_4, G_REG_1},
	{"PL",	"POLAND",				A_REG_1, G_REG_1},
	{"PT",	"PORTUGAL",				A_REG_1, G_REG_1},
	{"PR",	"PUERTO RICO",				A_REG_0, G_REG_0},
	{"QA",	"QATAR",				A_REG_0, G_REG_1},
	{"RO",	"ROMANIA",				A_REG_0, G_REG_1},
	{"RU",	"RUSSIA FEDERATION",			A_REG_0, G_REG_1},
	{"SA",	"SAUDI ARABIA",				A_REG_0, G_REG_1},
	{"SG",	"SINGAPORE",				A_REG_0, G_REG_1},
	{"SK",	"SLOVAKIA",				A_REG_1, G_REG_1},
	{"SI",	"SLOVENIA",				A_REG_1, G_REG_1},
	{"ZA",	"SOUTH AFRICA",				A_REG_1, G_REG_1},
	{"ES",	"SPAIN",				A_REG_1, G_REG_1},
	{"SE",	"SWEDEN",				A_REG_1, G_REG_1},
	{"CH",	"SWITZERLAND",				A_REG_1, G_REG_1},
	{"SY",	"SYRIAN ARAB REPUBLIC",			A_REG_0, G_REG_1},
	{"TW",	"TAIWAN",				A_REG_3, G_REG_0},
	{"TH",	"THAILAND",				A_REG_0, G_REG_1},
	{"TT",	"TRINIDAD AND TOBAGO",			A_REG_2, G_REG_1},
	{"TN",	"TUNISIA",				A_REG_2, G_REG_1},
	{"TR",	"TURKEY",				A_REG_2, G_REG_1},
	{"UA",	"UKRAINE",				A_REG_0, G_REG_1},
	{"AE",	"UNITED ARAB EMIRATES",			A_REG_0, G_REG_1},
	{"GB",	"UNITED KINGDOM",			A_REG_1, G_REG_1},
	{"US",	"UNITED STATES",			A_REG_9, G_REG_0},
	{"UY",	"URUGUAY",				A_REG_5, G_REG_1},
	{"UZ",	"UZBEKISTAN",				A_REG_1, G_REG_0},
	{"VE",	"VENEZUELA",				A_REG_5, G_REG_1},
	{"VN",	"VIET NAM",				A_REG_0, G_REG_1},
	{"YE",	"YEMEN",				A_REG_0, G_REG_1},
	{"ZW",	"ZIMBABWE",				A_REG_0, G_REG_1},
	{"",	"", 0, 0}
};

#define num_countries	(sizeof(chmaps)/sizeof(struct country_chmap))

struct channelx {
	uint32_t ch;
	uint32_t grp;
};

int get_valid_channels(const char *ifname, int band, int bw, const char *cc,
				uint32_t *chs, int *n, uint32_t filter)
{
	int region = 0;
	struct chlist *cx;
	int i = 0;
	int nr = 0;
	int idx = 0;
	struct channelx chanlist[32] = {0};
	int bw_factor = bw / 20;
	int rem = 0;
	int grp = 0;
	int grp120 = -1, grp124 = -1, grp128 = -1;

	*n = 0;

	if (band == 6)
		/* TODO: chmaps & chlist for 6Ghz */
		return -1;

	if (cc && cc[0] != '\0') {
		for (i = 0; i < num_countries; i++) {
			if (!strncmp(cc, chmaps[i].cc, 2)) {
				region = band == 5 ?
					chmaps[i].region_a : chmaps[i].region_g;
				break;
			}
		}
	}

	if (!i || i == num_countries)
		return -1;

	if (band == 2) {
		if (region >= g_region_chlist_num)
			return -1;

		while (g_region_chlist[region].list[idx].start) {
			cx = g_region_chlist[region].list + idx;
			for (i = 0; i < cx->num; i++) {
				chs[*n] = cx->start + i;
				*n += 1;
			}

			idx++;
		}

		return 0;
	}

	if (region >= a_region_chlist_num)
		return -1;

	while (a_region_chlist[region].list[idx].start) {
		i = 0;
		cx = a_region_chlist[region].list + idx;
		rem = cx->num % bw_factor;
		cx->num -= rem;
		for (i = 0; i < cx->num; i++) {
			chanlist[nr].ch = cx->start + i * 4;
			if (i % bw_factor == 0)
				grp++;

			chanlist[nr].grp = grp;
			/* printf("%d{%d} ", chanlist[nr].ch, chanlist[nr].grp); */
			nr++;
		}

		idx++;
		/* printf("   "); */
	}

	/* exclude tdwr channel groups */
	for (i = 0; i < nr; i++) {
		if (filter & FILTER_TDWR) {
			if (chanlist[i].ch == 120)
				grp120 = chanlist[i].grp;
			else if (chanlist[i].ch == 124)
				grp124 = chanlist[i].grp;
			else if (chanlist[i].ch == 128)
				grp128 = chanlist[i].grp;
		}
	}

	for (i = 0; i < nr; i++) {
		if (chanlist[i].grp == grp120
			|| chanlist[i].grp == grp124
			|| chanlist[i].grp == grp128)
			continue;

		*(chs + *n) = chanlist[i].ch;
		*n += 1;
	}

	return 0;
}

// FIXME: enum band and bw
int wifi_get_valid_channels(const char *ifname, enum wifi_band b,
				enum wifi_bw bw, const char *cc,
				uint32_t *chlist, int *n)
{
	int ret;
	uint32_t filter = FILTER_TDWR;
	int band;
	int bandwidth = 20;

	switch (b) {
	case BAND_2:
		band = 2;
		break;
	case BAND_5:
		band = 5;
		break;
	case BAND_6:
		band = 6;
		break;
	default:
		return -1;
	}

	if (bw >= BW20 && bw <= BW160) {
		bandwidth = 20 << bw;
	} else {
		libwifi_err("Unhandled bw = %d\n", bw);
		return -1;	// TODO: handle 8080 and auto
	}

	ret = get_valid_channels(ifname, band, bandwidth, cc, chlist, n, filter);
	if (ret < 0) {
		if (band == 5) {
			memcpy(chlist, def_chlist_a,
					def_chlist_a_num * sizeof(uint32_t));
			*n = def_chlist_a_num;
		} else if (band == 6) {
			memcpy(chlist, def_chlist_6,
					def_chlist_6_num * sizeof(uint32_t));
			*n = def_chlist_6_num;
		} else {
			memcpy(chlist, def_chlist_g,
					def_chlist_g_num * sizeof(uint32_t));
			*n = def_chlist_g_num;
		}
	}

	return 0;
}

////// opclass related /////////////////////////////////////
// TODO: move to separate file

static struct wifi_opclass wifi_opclass_global[] = {
	{ 81, 81, BAND_2, BW20, EXTCH_NONE, {20, 13, {{1, {1}},
						      {2, {2}},
						      {3, {3}},
						      {4, {4}},
						      {5, {5}},
						      {6, {6}},
						      {7, {7}},
						      {8, {8}},
						      {9, {9}},
						      {10, {10}},
						      {11, {11}},
						      {12, {12}},
						      {13, {13}}}}},
	{ 82, 82, BAND_2, BW20, EXTCH_NONE, {20, 1, {{14, {14}}}}},
	{ 83, 83, BAND_2, BW40, EXTCH_ABOVE, {20, 9, {{1, {1}},
						      {2, {2}},
						      {3, {3}},
						      {4, {4}},
						      {5, {5}},
						      {6, {6}},
						      {7, {7}},
						      {8, {8}},
						      {9, {9}}}}},
	{ 84, 84, BAND_2, BW40, EXTCH_BELOW, {20, 9, {{5, {5}},
						      {6, {6}},
						      {7, {7}},
						      {8, {8}},
						      {9, {9}},
						      {10, {10}},
						      {11, {11}},
						      {12, {12}},
						      {13, {13}}}}},

	{ 115, 115, BAND_5, BW20, EXTCH_NONE, {23, 4, {{36, {36}},
						       {40, {40}},
						       {44, {44}},
						       {48, {48}}}}},
	{ 116, 116, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{36, {36}},
							{44, {44}}}}},
	{ 117, 117, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{40, {40}},
							{48, {48}}}}},
	{ 118, 118, BAND_5, BW20, EXTCH_NONE, {23, 4, {{52, {52}},
						       {56, {56}},
						       {60, {60}},
						       {64, {64}}}}},
	{ 119, 119, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{52, {52}},
							{60, {60}}}}},
	{ 120, 120, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{56, {56}},
							{64, {64}}}}},
	{ 121, 121, BAND_5, BW20, EXTCH_NONE, {30, 12, {{100, {100}},
							{104, {104}},
							{108, {108}},
							{112, {112}},
							{116, {116}},
							{120, {120}},
							{124, {124}},
							{128, {128}},
							{132, {132}},
							{136, {136}},
							{140, {140}},
							{144, {144}}}}},
	{ 122, 122, BAND_5, BW40, EXTCH_ABOVE, {30, 6, {{100, {100}},
							{108, {108}},
							{116, {116}},
							{124, {124}},
							{132, {132}},
							{140, {140}}}}},
	{ 123, 123, BAND_5, BW40, EXTCH_BELOW, {30, 6, {{104, {104}},
							{112, {112}},
							{120, {120}},
							{128, {128}},
							{136, {136}},
							{144, {144}}}}},
	{ 124, 124, BAND_5, BW20, EXTCH_NONE, {36, 4, {{149, {149}},
						       {153, {153}},
						       {157, {157}},
						       {161, {161}}}}},
	{ 125, 125, BAND_5, BW20, EXTCH_NONE, {36, 8, {{149, {149}},
						       {153, {153}},
						       {157, {157}},
						       {161, {161}},
						       {165, {165}},
						       {169, {169}},
						       {173, {173}},
						       {177, {177}}}}},
	{ 126, 126, BAND_5, BW40, EXTCH_ABOVE, {36, 4, {{149, {149}},
							{157, {157}},
						        {165, {165}},
							{173, {173}}}}},

	{ 127, 127, BAND_5, BW40, EXTCH_BELOW, {36, 4, {{153, {153}},
						        {161, {161}},
						        {169, {169}},
							{177, {177}}}}},

	{ 128, 128, BAND_5, BW80, EXTCH_AUTO, {23, 7, {{36, {36, 40, 44, 48}},
						       {52, {52, 56, 60, 64}},
						       {100, {100, 104, 108, 112}},
						       {116, {116, 120, 124, 128}},
						       {132, {132, 136, 140, 144}},
						       {149, {149, 153, 157, 161}},
						       {165, {165, 169, 173, 177}}}}},

	{ 129, 129, BAND_5, BW160, EXTCH_AUTO, {23, 3, {{36, {36, 40, 44, 48, 52, 56, 60, 64}},
						        {100, {100, 104, 108, 112, 116, 120, 124, 128}},
						        {149, {149, 153, 157, 161, 165, 169, 173, 177}}}}},

	{ 130, 130, BAND_5, BW8080, EXTCH_AUTO, {23, 7, {{36, {36, 40, 44, 48}},
							 {52, {52, 56, 60, 64}},
							 {100, {100, 104, 108, 112}},
							 {116, {116, 120, 124, 128}},
							 {132, {132, 136, 140, 144}},
							 {149, {149, 153, 157, 161}},
							 {165, {165, 169, 173, 177}}}}},

	{ 131, 131, BAND_6, BW20, EXTCH_NONE, {23, 59, {{1, {1}},
						       {5, {5}},
						       {9, {9}},
						       {13, {13}},
						       {17, {17}},
						       {21, {21}},
						       {25, {25}},
						       {29, {29}},
						       {33, {33}},
						       {37, {37}},
						       {41, {41}},
						       {45, {45}},
						       {49, {49}},
						       {53, {53}},
						       {57, {57}},
						       {61, {61}},
						       {65, {65}},
						       {69, {69}},
						       {73, {73}},
						       {77, {77}},
						       {81, {81}},
						       {85, {85}},
						       {89, {89}},
						       {93, {93}},
						       {97, {97}},
						       {101, {101}},
						       {105, {105}},
						       {109, {109}},
						       {113, {113}},
						       {117, {117}},
						       {121, {121}},
						       {125, {125}},
						       {129, {129}},
						       {133, {133}},
						       {137, {137}},
						       {141, {141}},
						       {145, {145}},
						       {149, {149}},
						       {153, {153}},
						       {157, {157}},
						       {161, {161}},
						       {165, {165}},
						       {169, {169}},
						       {173, {173}},
						       {177, {177}},
						       {181, {181}},
						       {185, {185}},
						       {189, {189}},
						       {193, {193}},
						       {197, {197}},
						       {201, {201}},
						       {205, {205}},
						       {209, {209}},
						       {213, {213}},
						       {217, {217}},
						       {221, {221}},
						       {225, {225}},
						       {229, {229}},
						       {233, {233}}}}},

	{ 132, 132, BAND_6, BW40, EXTCH_AUTO, {23, 29, {{1, {1 , 5}},
						       {9, {9, 13}},
						       {17, {17, 21}},
						       {25, {25, 29}},
						       {33, {33, 37}},
						       {41, {41, 45}},
						       {49, {49, 53}},
						       {57, {57, 61}},
						       {65, {65, 69}},
						       {73, {73, 77}},
						       {81, {81, 85}},
						       {89, {89, 93}},
						       {97, {97, 101}},
						       {105, {105, 109}},
						       {113, {113, 117}},
						       {121, {121, 125}},
						       {129, {129, 133}},
						       {137, {137, 141}},
						       {145, {145, 149}},
						       {153, {153, 157}},
						       {161, {161, 165}},
						       {169, {169, 173}},
						       {177, {177, 181}},
						       {185, {185, 189}},
						       {193, {193, 197}},
						       {201, {201, 205}},
						       {209, {209, 213}},
						       {217, {217, 221}},
						       {225, {225, 229}}}}},

	{ 133, 133, BAND_6, BW80, EXTCH_AUTO, {23, 14, {{1, {1, 5, 9, 13}},
						       {17, {17, 21, 25, 29}},
						       {33, {33, 37, 41, 45}},
						       {49, {49, 53, 57, 61}},
						       {65, {65, 69, 73, 77}},
						       {81, {81, 85, 89, 93}},
						       {97, {97, 101, 105, 109}},
						       {113, {113, 117, 121, 125}},
						       {129, {129, 133, 137, 141}},
						       {145, {145, 149, 153, 157}},
						       {161, {161, 165, 169, 173}},
						       {177, {177, 181, 185, 189}},
						       {193, {193, 197, 201, 205}},
						       {209, {209, 213, 217, 221}}}}},

	{ 134, 134, BAND_6, BW160, EXTCH_AUTO, {23, 7, {{1, {1, 5, 9, 13, 17, 21, 25, 29}},
						        {33, {33, 37, 41, 45, 49, 53, 57, 61}},
						        {65, {65, 69, 73, 77, 81, 85, 89, 93}},
						        {97, {97, 101, 105, 109, 113, 117, 121, 125}},
						        {129, {129, 133, 137, 141, 145, 149, 153, 157}},
						        {161, {161, 165, 169, 173, 177, 181, 185, 189}},
						        {193, {193, 197, 201, 205, 209, 213, 217, 221}}}}},

	{ 135, 135, BAND_6, BW8080, EXTCH_AUTO, {23, 14, {{1, {1, 5, 9, 13}},
						       {17, {17, 21, 25, 29}},
						       {33, {33, 37, 41, 45}},
						       {49, {49, 53, 57, 61}},
						       {65, {65, 69, 73, 77}},
						       {81, {81, 85, 89, 93}},
						       {97, {97, 101, 105, 109}},
						       {113, {113, 117, 121, 125}},
						       {129, {129, 133, 137, 141}},
						       {145, {145, 149, 153, 157}},
						       {161, {161, 165, 169, 173}},
						       {177, {177, 181, 185, 189}},
						       {193, {193, 197, 201, 205}},
						       {209, {209, 213, 217, 221}}}}},
	{ 136, 136, BAND_6, BW20, EXTCH_NONE, {23, 1, {{2, {2}}}}},
	{ 137, 137, BAND_6, BW320, EXTCH_AUTO, {23, 6, {{1, {1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61}},
						        {33, {33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93}},
						        {65, {65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125}},
						        {97, {97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157}},
						        {129, {129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189}},
						        {161, {161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221}}}}},

};

#define wifi_opclass_global_size	\
	sizeof(wifi_opclass_global)/sizeof(wifi_opclass_global[0])

static struct wifi_opclass wifi_opclass_eu[] = {
	{ 1, 115, BAND_5, BW20, EXTCH_NONE, {23, 4, {{36}, {40}, {44}, {48}}}},
	{ 2, 118, BAND_5, BW20, EXTCH_NONE, {23, 4, {{52}, {56}, {60}, {64}}}},
	{ 3, 121, BAND_5, BW20, EXTCH_NONE, {30, 11, {{100}, {104}, {108}, {112}, {116}, {120}, {124}, \
					     {128}, {132}, {136}, {140}}}},
	{ 4, 81, BAND_2, BW20, EXTCH_NONE, {20, 13, {{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}, {10}, {11}, {12}, {13}}}},
	{ 5, 116, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{36}, {44}}}},
	{ 6, 119, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{52}, {60}}}},
	{ 7, 122, BAND_5, BW40, EXTCH_ABOVE, {30, 5, {{100}, {108}, {116}, {124}, {132}}}},
	{ 8, 117, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{40}, {48}}}},
	{ 9, 120, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{56}, {64}}}},
	{ 10, 123, BAND_5, BW40, EXTCH_BELOW, {30, 5, {{104}, {112}, {120}, {128}, {136}}}},
	{ 11, 83, BAND_2, BW40, EXTCH_ABOVE, {20, 9, {{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}}}},
	{ 12, 84, BAND_2, BW40, EXTCH_BELOW, {20, 9, {{5}, {6}, {7}, {8}, {9}, {10}, {11}, {12}, {13}}}},
	{ 17, 125, BAND_5, BW20, EXTCH_NONE, {30, 6, {{149}, {153}, {157}, {161}, {165}, {169}}}},
	{ 128, 128, BAND_5, BW80, EXTCH_AUTO, {30, 4, {{36}, {52}, {100}, {116}}}},
	{ 129, 129, BAND_5, BW160, EXTCH_AUTO, {23, 2, {{36}, {100}}}},
	{ 130, 130, BAND_5, BW8080, EXTCH_AUTO, {23, 4, {{36}, {52}, {100}, {116}}}},
};

#define wifi_opclass_eu_size	\
		sizeof(wifi_opclass_eu)/sizeof(wifi_opclass_eu[0])


static struct wifi_opclass wifi_opclass_us[] = {
	{ 1, 115, BAND_5, BW20, EXTCH_NONE, {23, 4, {{36}, {40}, {44}, {48}}}},
	{ 2, 118, BAND_5, BW20, EXTCH_NONE, {23, 4, {{52}, {56}, {60}, {64}}}},
	{ 3, 124, BAND_5, BW20, EXTCH_NONE, {36, 6, {{149}, {153}, {157}, {161}}}},
	{ 4, 121, BAND_5, BW20, EXTCH_NONE, {30, 12, {{100}, {104}, {108}, {112}, {116}, {120}, {124}, \
					     {128}, {132}, {136}, {140}, {144}}}},
	{ 5, 125, BAND_5, BW20, EXTCH_NONE, {36, 5, {{149}, {153}, {157}, {161}, {165}}}},
	{ 12, 81, BAND_2, BW20, EXTCH_NONE, {30, 11, {{1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}, {10}, {11}}}},
	{ 22, 116, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{36}, {44}}}},
	{ 23, 119, BAND_5, BW40, EXTCH_ABOVE, {23, 2, {{52}, {60}}}},
	{ 24, 122, BAND_5, BW40, EXTCH_ABOVE, {30, 6, {{100}, {108}, {116}, {124}, {132}, {140}}}},
	{ 25, 126, BAND_5, BW40, EXTCH_ABOVE, {36, 2, {{149}, {157}}}},
	{ 26, 126, BAND_5, BW40, EXTCH_ABOVE, {36, 2, {{149}, {157}}}},
	{ 27, 117, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{40}, {48}}}},
	{ 28, 120, BAND_5, BW40, EXTCH_BELOW, {23, 2, {{56}, {64}}}},
	{ 29, 123, BAND_5, BW40, EXTCH_BELOW, {30, 6, {{104}, {112}, {120}, {128}, {136}, {144}}}},
	{ 30, 127, BAND_5, BW40, EXTCH_BELOW, {36, 2, {{153}, {161}}}},
	{ 31, 127, BAND_5, BW40, EXTCH_BELOW, {36, 2, {{153}, {161}}}},
	{ 32, 83, BAND_2, BW40, EXTCH_ABOVE, {30, 7, {{1}, {2}, {3}, {4}, {5}, {6}, {7}}}},
	{ 33, 84, BAND_2, BW40, EXTCH_BELOW, {30, 7, {{5}, {6}, {7}, {8}, {9}, {10}, {11}}}},

	{ 128, 128, BAND_5, BW80, EXTCH_AUTO, {23, 6, {{36}, {52}, {100}, {116}, {132}, {149}}}},
	{ 129, 129, BAND_5, BW160, EXTCH_AUTO, {23, 2, {{36}, {100}}}},
	{ 130, 130, BAND_5, BW8080, EXTCH_AUTO, {23, 6, {{36}, {52}, {100}, {116}, {132}, {149}}}},
};

#define wifi_opclass_us_size	\
		sizeof(wifi_opclass_us)/sizeof(wifi_opclass_us[0])

//TODO: JP and CN opclass table


enum wifi_regdomain {
	REG_GLOBAL = 0,
	REG_US,
	REG_EU,
	REG_JP,
	REG_CN,
	NUM_REG = 4,
};

struct country_regdomain {
	const char *cc[32];
};

#define eu_countries	"AL", "AM", "AT", "BE", "DE", "EE", "EU", "SE", \
			"NO", "FI", "FR", "IT", "IS", "LV", "LU", "NL"

#define us_countries	"CA", "US", "TW"
#define jp_countries	"JP"
#define cn_countries	"CN"

static struct country_regdomain creglist[] = {
	[REG_EU] = {{eu_countries, NULL}},
	[REG_US] = {{us_countries, NULL}},
	[REG_JP] = {{jp_countries, NULL}},
	[REG_CN] = {{cn_countries, NULL}},
};

int wifi_get_regdomain_from_country(const char *cc, enum wifi_regdomain *reg)
{
	int i, j;

	*reg = REG_GLOBAL;

	for (i = 1; i < NUM_REG; i++) {
		j = 0;
		while (creglist[i].cc[j]) {
			if (!strncmp(creglist[i].cc[j], cc, 2)) {
				*reg = i;
				return 0;
			}
			j++;
		}
	}

	return -1;
}

int wifi_get_opclass_internal(enum wifi_regdomain reg, enum wifi_band b,
			uint32_t ch, enum wifi_bw bw, enum wifi_chan_ext ext,
			struct wifi_opclass *o)
{
	int i, j;
	struct wifi_opclass *tab;
	int tabsize;

	switch (reg) {
	case REG_EU:
		tab = (struct wifi_opclass *)wifi_opclass_eu;
		tabsize = wifi_opclass_eu_size;
		break;
	case REG_US:
		tab = (struct wifi_opclass *)wifi_opclass_us;
		tabsize = wifi_opclass_us_size;
		break;
	case REG_JP:
		//TODO
		tab = NULL;
		tabsize = 0;
		break;
	case REG_CN:
		//TODO
#if !defined(RTCONFIG_SWRTMESH)
		tab = NULL;
		tabsize = 0;
		break;
#endif
	case REG_GLOBAL:
	default:
		tab = (struct wifi_opclass *)wifi_opclass_global;
		tabsize = wifi_opclass_global_size;
		break;
	}

	if (!tab)
		return -1;

	for (i = 0; i < tabsize; i++) {
		struct wifi_opclass *ptr = tab + i;

		if (ptr->band == b && ptr->bw == bw) {
			for (j = 0; j < ptr->opchannel.num; j++) {
				/* For BW20/BW40 match channel */
				switch (bw) {
				case BW20:
				case BW40:
					if (ptr->band == BAND_2 && bw == BW40) {
						/* Special case, we need to check chanext also */
						if (ptr->opchannel.ch[j].channel == ch && ptr->ext == ext) {
							memcpy(o, ptr, sizeof(struct wifi_opclass));
							return 0;
						}
					} else if (ptr->opchannel.ch[j].channel == ch) {
						memcpy(o, ptr, sizeof(struct wifi_opclass));
						return 0;
					}
					continue;

				default:
					if (ptr->opchannel.ch[j].channel <= ch &&
						ch <= ptr->opchannel.ch[j].channel + 4 + ptr->bw * 4) {

						memcpy(o, ptr, sizeof(struct wifi_opclass));
						return 0;
					}
				}
			}
		}
	}

	return -1;
}

int wifi_get_opclass_bw(const char *name, struct wifi_opclass *o, const enum wifi_bw *pbw)
{
	enum wifi_band band = BAND_5;
	struct wifi_radio radio = {};
	enum wifi_bw bw = BW20;
	enum wifi_chan_ext ext = EXTCH_AUTO;
	enum wifi_regdomain reg;
	uint32_t channel = 0;
	char cc[4] = {0};
	int ret = 0;

	ret |= wifi_get_channel(name, &channel, &bw);
	ret |= wifi_get_bandwidth(name, &bw);
	ret |= wifi_get_country(name, cc);
	ret |= wifi_get_oper_band(name, &band);

	if (band == BAND_2) {
		WARN_ON(wifi_radio_info(name, &radio));
		ext = radio.extch;
	}

	if (pbw)
		bw = *pbw;

	wifi_get_regdomain_from_country(cc, &reg);

	reg = REG_GLOBAL;
	ret |= wifi_get_opclass_internal(reg, band, channel, bw, ext, o);

	return ret;
}

int wifi_get_opclass(const char *name, struct wifi_opclass *o)
{
	return wifi_get_opclass_bw(name, o, NULL);
}

int wifi_opclass_to_channels(uint32_t opclass, int *num, uint32_t *channels)
{
	struct wifi_opclass *tab;
	struct wifi_opclass *ptr;
	int tabsize;
	int i;

	if (!num || !channels)
		return -1;

	tab = (struct wifi_opclass *)wifi_opclass_global;
	tabsize = wifi_opclass_global_size;

	for (i = 0; i < tabsize; i++) {
		ptr = tab + i;
		if (ptr->g_opclass == opclass)
			break;
	}

	if (i == tabsize)
		return -1;

	if (*num < ptr->opchannel.num)
		return -1;

	*num = 0;
	for (i = 0; i < ptr->opchannel.num; i++) {
		channels[i] = ptr->opchannel.ch[i].channel;
		*num += 1;
	}

	return 0;
}

int wifi_get_opclass_ht20(const char *name, struct wifi_opclass *o)
{
	enum wifi_bw bw = BW20;

	return wifi_get_opclass_bw(name, o, &bw);
}

int wifi_get_supported_opclass_internal(enum wifi_regdomain reg,
				uint32_t bands, enum wifi_bw bw,
				int *num, struct wifi_opclass *o)
{
	struct wifi_opclass *tab;
	int tabsize;
	int i;

	switch (reg) {
	case REG_EU:
		tab = (struct wifi_opclass *)wifi_opclass_eu;
		tabsize = wifi_opclass_eu_size;
		break;
	case REG_US:
		tab = (struct wifi_opclass *)wifi_opclass_us;
		tabsize = wifi_opclass_us_size;
		break;
	case REG_JP:
		//TODO
		tab = NULL;
		tabsize = 0;
		break;
	case REG_CN:
		//TODO
		tab = NULL;
		tabsize = 0;
		break;
	case REG_GLOBAL:
	default:
		tab = (struct wifi_opclass *)wifi_opclass_global;
		tabsize = wifi_opclass_global_size;
		break;
	}

	if (*num <= 0 || *num < tabsize)
		return -1;

	*num = 0;

	for (i = 0; i < tabsize; i++) {
		struct wifi_opclass *ptr = tab + i;

		if (ptr->band & bands) {
			memcpy(o + *num, ptr, sizeof(struct wifi_opclass));
			*num += 1;
		}
	}

	return 0;
}

static int _radio_get_supported_opclass(const char *name, int *num, struct wifi_opclass *o)
{
	uint32_t bands = 0;
	enum wifi_bw bw = BW20;
	enum wifi_regdomain reg;
	char cc[4] = {0};
	int ret = 0;

	if (*num <= 0)
		return -1;

	ret |= wifi_get_supp_band(name, &bands);
	wifi_get_bandwidth(name, &bw);
	ret |= wifi_get_country(name, cc);

	wifi_get_regdomain_from_country(cc, &reg);
	reg = REG_GLOBAL;
	ret |= wifi_get_supported_opclass_internal(reg, bands, bw, num, o);

	return ret;
}

int wifi_get_sideband(const char *name, enum wifi_chan_ext *sideband)
{
	int ht40plus[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 149, 157, 184, 192 };
	enum wifi_chan_ext ext = EXTCH_NONE;
	enum wifi_bw bw = BW20;
	uint32_t channel = 0;
	int i;

	if (WARN_ON(wifi_get_channel(name, &channel, &bw)))
		return -1;

	libwifi_dbg("[%s] %s channel %d bw %d\n", name, __func__, channel, bw);

	/* Don't set for 2GHz - get this from HW */
	if (channel <= 14)
		return 0;

	switch (bw) {
	case BW40:
	case BW80:
	case BW8080:
		for (i = 0; i < ARRAY_SIZE(ht40plus); i++) {
			if (channel == ht40plus[i]) {
				ext = EXTCH_ABOVE;
				break;
			}
		}

		if (i == ARRAY_SIZE(ht40plus))
			ext = EXTCH_BELOW;

		break;
	case BW_AUTO:
		ext = EXTCH_AUTO;
		break;
	default:
		break;
	}

	*sideband = ext;
	return 0;
}

bool wifi_is_dfs_channel(const char *name, int channel, int bandwidth)
{
	struct chan_entry channels[64] = {};
	int channels_num = ARRAY_SIZE(channels);
	const int *chans;
	int i;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (channel <= 14)
		return false;

	if (WARN_ON(wifi_channels_info(name, channels, &channels_num)))
		return false;

	/* Check if at least one DFS channel */
	chans = chan2list(channel, bandwidth);

	while (chans && *chans) {
		for (i = 0; i < channels_num; i++) {
			if (channels[i].channel == *chans && channels[i].dfs)
				return true;
		}
		chans++;
	}

	return false;
}

struct chanlist {
	int bw;
	enum wifi_chan_ext sb;
	int chans[16];
};

const int *chan2list(int chan, int bw)
{
	static const struct chanlist chanlist[] = {
		{.bw = 20, .chans = {36, 0}},
		{.bw = 20, .chans = {40, 0}},
		{.bw = 20, .chans = {44, 0}},
		{.bw = 20, .chans = {48, 0}},
		{.bw = 20, .chans = {52, 0}},
		{.bw = 20, .chans = {56, 0}},
		{.bw = 20, .chans = {60, 0}},
		{.bw = 20, .chans = {64, 0}},
		{.bw = 20, .chans = {100, 0}},
		{.bw = 20, .chans = {104, 0}},
		{.bw = 20, .chans = {108, 0}},
		{.bw = 20, .chans = {112, 0}},
		{.bw = 20, .chans = {116, 0}},
		{.bw = 20, .chans = {120, 0}},
		{.bw = 20, .chans = {124, 0}},
		{.bw = 20, .chans = {128, 0}},
		{.bw = 20, .chans = {132, 0}},
		{.bw = 20, .chans = {136, 0}},
		{.bw = 20, .chans = {140, 0}},
		{.bw = 20, .chans = {144, 0}},
		{.bw = 20, .chans = {149, 0}},
		{.bw = 20, .chans = {153, 0}},
		{.bw = 20, .chans = {157, 0}},
		{.bw = 20, .chans = {161, 0}},
		{.bw = 20, .chans = {165, 0}},
		{.bw = 40, .chans = {36, 40, 0}},
		{.bw = 40, .chans = {44, 48, 0}},
		{.bw = 40, .chans = {52, 56, 0}},
		{.bw = 40, .chans = {60, 64, 0}},
		{.bw = 40, .chans = {100, 104, 0}},
		{.bw = 40, .chans = {108, 112, 0}},
		{.bw = 40, .chans = {116, 120, 0}},
		{.bw = 40, .chans = {124, 128, 0}},
		{.bw = 40, .chans = {132, 136, 0}},
		{.bw = 40, .chans = {140, 144, 0}},
		{.bw = 40, .chans = {149, 153, 0}},
		{.bw = 40, .chans = {157, 161, 0}},
		{.bw = 80, .chans = {36, 40, 44, 48, 0}},
		{.bw = 80, .chans = {52, 56, 60, 64, 0}},
		{.bw = 80, .chans = {100, 104, 108, 112, 0}},
		{.bw = 80, .chans = {116, 120, 124, 128, 0}},
		{.bw = 80, .chans = {132, 136, 140, 144, 0}},
		{.bw = 80, .chans = {149, 153, 157, 161, 0}},
		{.bw = 160, .chans = {36, 40, 44, 48, 52, 56, 60, 64, 0}},
		{.bw = 160, .chans = {100, 104, 108, 112, 116, 120, 124, 128, 0}},
	};
	const int *chans;
	const int *start;
	int i;

	for (i = 0; i < ARRAY_SIZE(chanlist); i++) {
		if (chanlist[i].bw == bw) {
			chans = chanlist[i].chans;
			start = chans;
			while (*start) {
				if (*start == chan)
					return chans;
				start++;
			}
		}
	}

	return NULL;
}

const int *chan2list_2g(int chan, int bw, enum wifi_chan_ext sideband)
{
	static const struct chanlist chanlist_2g[] = {
		{.bw = 20, .sb = EXTCH_NONE, .chans = {1, 0}},
		{.bw = 20, .sb = EXTCH_NONE, .chans = {6, 0}},
		{.bw = 20, .sb = EXTCH_NONE, .chans = {11, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {1, 6, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {6, 1, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {6, 11, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {11, 6, 0}},
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(chanlist_2g); i++) {
		if (chanlist_2g[i].bw == bw &&
		    chanlist_2g[i].sb == sideband &&
		    chanlist_2g[i].chans[0] == chan) {
			return chanlist_2g[i].chans;
		}
	}

	return NULL;
}

const int *chan2list_2g_adj(int chan, int bw, enum wifi_chan_ext sideband)
{
	static const struct chanlist chanlist_2g_adj[] = {
		{.bw = 20, .chans = {1, 2, 3, 4, 0}},
		{.bw = 20, .chans = {2, 3, 4, 5, 1, 0}},
		{.bw = 20, .chans = {3, 4, 5, 6, 1, 2, 0}},
		{.bw = 20, .chans = {4, 5, 6, 7, 1, 2, 3, 0}},
		{.bw = 20, .chans = {5, 6, 7, 8, 2, 3, 4, 0}},
		{.bw = 20, .chans = {6, 7, 8, 9, 3, 4, 5, 0}},
		{.bw = 20, .chans = {7, 8, 9, 10, 4, 5, 6, 0}},
		{.bw = 20, .chans = {8, 9, 10, 11, 5, 6, 7, 0}},
		{.bw = 20, .chans = {9, 10, 11, 12, 6, 7, 8, 0}},
		{.bw = 20, .chans = {10, 11, 12, 13, 7, 8, 9, 0}},
		{.bw = 20, .chans = {11, 12, 13, 14, 8, 9, 10, 0}},
		{.bw = 20, .chans = {12, 13, 14, 9, 10, 11, 0}},
		{.bw = 20, .chans = {13, 14, 10, 11, 12, 0}},
		{.bw = 20, .chans = {14, 11, 12, 13, 0}},

		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {1, 2, 3, 4, 5, 6, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {2, 3, 4, 5, 6, 7, 1, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {3, 4, 5, 6, 7, 8, 1, 2, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {4, 5, 6, 7, 8, 9, 1, 2, 3, 0}},

		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {5, 6, 7, 8,  2, 3, 4, 9, 10, 11, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {6, 7, 8, 9, 3, 4, 5, 10, 11, 12, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {7, 8, 9, 10, 4, 5, 6, 11, 12, 13, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {8, 9, 10, 11, 5, 6, 7, 12, 13, 14, 0}},
		{.bw = 40, .sb = EXTCH_ABOVE, .chans = {9, 10, 11, 12, 6, 7, 8, 13, 14, 0}},

		{.bw = 40, .sb = EXTCH_BELOW, .chans = {5, 6, 7, 8, 1, 2, 3, 4, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {6, 7, 8, 9, 1, 2, 3, 4, 5, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {8, 9, 10, 11, 2, 3, 4, 5, 6, 7, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {9, 10, 11, 12, 3, 4, 5, 6, 7, 8, 0}},

		{.bw = 40, .sb = EXTCH_BELOW, .chans = {10, 11, 12, 13, 14, 5, 6, 7, 8, 9, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {11, 12, 13, 14, 6, 7, 8, 9, 10, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {12, 13, 14, 7, 8, 9, 10, 11, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {13, 14, 8, 9, 10, 11, 12, 0}},
		{.bw = 40, .sb = EXTCH_BELOW, .chans = {14, 9, 10, 11, 12, 13, 0}},
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(chanlist_2g_adj); i++) {
		if (chanlist_2g_adj[i].bw == bw &&
		    chanlist_2g_adj[i].sb == sideband &&
		    chanlist_2g_adj[i].chans[0] == chan) {
			return chanlist_2g_adj[i].chans;
		}
	}

	return NULL;
}

static const int *radio_adjacent_channels(int chan, int bw, enum wifi_chan_ext sideband)
{
	if (chan >= 36)
		return chan2list(chan, bw);

	return chan2list_2g_adj(chan, bw, sideband);
}

static const int *radio_chan2list(int chan, int bw, enum wifi_chan_ext sideband)
{
	if (chan >= 36)
		return chan2list(chan, bw);

	return chan2list_2g(chan, bw, sideband);
}

static bool radio_opclass_dfs_required(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *chans;
	int j;

	chans = chan2list(channel, wifi_bw_enum2MHz(bw));

	while (chans && *chans) {
		for (j = 0; j < chan_num; j++) {
			if (*chans == chan[j].channel) {
				if (chan[j].dfs)
					return true;
			}
		}
		chans++;
	}

	return false;
}

static bool radio_opclass_dfs_unavailable(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *chans;
	int j;

	chans = chan2list(channel, wifi_bw_enum2MHz(bw));

	/* Check any channel in NOP state (unavailable) */
	while (chans && *chans) {
		for (j = 0; j < chan_num; j++) {
			if (*chans == chan[j].channel) {
				if (!chan[j].dfs)
					continue;
				if (chan[j].dfs_state == WIFI_DFS_STATE_UNAVAILABLE)
					return true;
			}
		}
		chans++;
	}

	return false;
}

static bool radio_opclass_dfs_available(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *chans;
	int j;

	chans = chan2list(channel, wifi_bw_enum2MHz(bw));

	/* Check all DFS channels pass CAC (available) */
	while (chans && *chans) {
		for (j = 0; j < chan_num; j++) {
			if (*chans == chan[j].channel) {
				if (!chan[j].dfs)
					continue;
				if (chan[j].dfs_state != WIFI_DFS_STATE_AVAILABLE)
					return false;
			}
		}
		chans++;
	}

	return true;
}

static bool radio_opclass_dfs_cac(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *chans;
	int j;

	chans = chan2list(channel, wifi_bw_enum2MHz(bw));

	/* Check all DFS channels CAC ongoing (cac) */
	while (chans && *chans) {
		for (j = 0; j < chan_num; j++) {
			if (*chans == chan[j].channel) {
				if (!chan[j].dfs)
					continue;
				if (chan[j].dfs_state != WIFI_DFS_STATE_CAC)
					return false;
			}
		}
		chans++;
	}

	return true;
}

static bool radio_opclass_dfs_usable(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *chans;
	int j;

	chans = chan2list(channel, wifi_bw_enum2MHz(bw));

	/* Check all DFS channels require CAC (usable) */
	while (chans && *chans) {
		for (j = 0; j < chan_num; j++) {
			if (*chans == chan[j].channel) {
				if (!chan[j].dfs)
					continue;
				if (chan[j].dfs_state != WIFI_DFS_STATE_USABLE)
					return false;
			}
		}
		chans++;
	}

	return true;
}

static enum dfs_state radio_opclass_dfs_state(int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	if (radio_opclass_dfs_unavailable(channel, bw, chan, chan_num))
		return WIFI_DFS_STATE_UNAVAILABLE;

	if (radio_opclass_dfs_available(channel, bw, chan, chan_num))
		return WIFI_DFS_STATE_AVAILABLE;

	if (radio_opclass_dfs_cac(channel, bw, chan, chan_num))
		return WIFI_DFS_STATE_CAC;

	if (radio_opclass_dfs_usable(channel, bw, chan, chan_num))
		return WIFI_DFS_STATE_USABLE;

	return WIFI_DFS_STATE_USABLE;
}

static uint32_t radio_dfs_cac_time(const char *name, int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *channels;
	uint32_t cac_time = 0;
	int bandwidth;
	int i;

	/* Get adjacent channels we should care */
	bandwidth = wifi_bw_enum2MHz(bw);
	channels = chan2list(channel, bandwidth);

	/* Get worst (highest) CAC value */
	while (channels && *channels) {
		for (i = 0; i < chan_num; i++) {
			if (chan[i].channel != *channels)
				continue;

			if (chan[i].cac_time > cac_time)
				cac_time = chan[i].cac_time;
			libwifi_dbg("[%s] cac_time chan %d cac_time %d new cac_time %d\n", name, *channels, chan[i].cac_time, cac_time);
			break;
		}
		channels++;
	}

	return cac_time;
}

static uint32_t radio_dfs_nop_time(const char *name, int channel, enum wifi_bw bw, struct chan_entry *chan, int chan_num)
{
	const int *channels;
	uint32_t nop_time = 0;
	int bandwidth;
	int i;

	/* Get adjacent channels we should care */
	bandwidth = wifi_bw_enum2MHz(bw);
	channels = chan2list(channel, bandwidth);

	/* Get worst (highest) NOP value */
	while (channels && *channels) {
		for (i = 0; i < chan_num; i++) {
			if (chan[i].channel != *channels)
				continue;

			if (chan[i].nop_time > nop_time)
				nop_time = chan[i].nop_time;
			libwifi_dbg("[%s] nop_time chan %d noptime %d new nop_time %d\n", name, *channels, chan[i].nop_time, nop_time);
			break;
		}
		channels++;
	}

	return nop_time;
}

static int radio_opclass_recalc_noise(const char *name, int channel, enum wifi_bw bw,
				      enum wifi_chan_ext sideband, struct chan_entry *chan,
				      int chan_num)
{
	int noise = -255;
	const int *channels;
	int bandwidth;
	int i;

	/* Get adjacent channels we should care */
	bandwidth = wifi_bw_enum2MHz(bw);
	channels = radio_chan2list(channel, bandwidth, sideband);

	/* Get worst (highest) noise value */
	while (channels && *channels) {
		for (i = 0; i < chan_num; i++) {
			if (chan[i].channel != *channels)
				continue;

			if (chan[i].noise > noise)
				noise = chan[i].noise;
			break;
		}
		channels++;
	}

	libwifi_dbg("[%s] score chan %d/%d new noise %d\n", name, channel, bandwidth, noise);
	return noise;
}

static uint8_t radio_opclass_recalc_busy(const char *name, int channel, enum wifi_bw bw,
					 enum wifi_chan_ext sideband, struct chan_entry *chan,
					 int chan_num)
{
	const int *channels;
	int bandwidth;
	int busy = 0;
	int busy_num = 0;
	int i;

	/* Get adjacent channels we should care */
	bandwidth = wifi_bw_enum2MHz(bw);
	channels = radio_chan2list(channel, bandwidth, sideband);

	/* Get average value */
	while (channels && *channels) {
		for (i = 0; i < chan_num; i++) {
			if (chan[i].channel != *channels)
				continue;

			/* If invalid value for one channel - mark bw invalid */
			if (chan[i].busy > 100)
				return 255;

			busy += chan[i].busy;
			busy_num++;
			break;
		}
		channels++;
	}

	if (!busy_num)
		return 255;

	busy = busy/busy_num;
	libwifi_dbg("[%s] avg busy %d\n", name, busy);
	return (uint8_t)((busy > 255) ? 255 : busy);
}

static uint8_t radio_opclass_bss_num(const char *name, int channel, enum wifi_bw bw,
				     enum wifi_chan_ext sideband, struct chan_entry *chan,
				     int chan_num, struct wifi_bss *bss, int bss_num)
{
	const int *chans, *channels;
	int bandwidth;
	uint8_t num = 0;
	int i;

	bandwidth = wifi_bw_enum2MHz(bw);
	channels = radio_adjacent_channels(channel, bandwidth, sideband);
	for (i = 0; i < bss_num; i++) {
		chans = channels;
		while (chans && *chans) {
			if (bss[i].channel == *chans) {
				num++;
				libwifi_dbg("[%s] chan %d/%d new bss (bss %s chan %d) num: %d\n",
					    name, channel, bandwidth, bss[i].ssid, *chans, num);
			}
			chans++;
		}
	}

	return num;
}

static uint8_t radio_opclass_recalc_score(const char *name, int channel, enum wifi_bw bw,
					  enum wifi_chan_ext sideband, struct chan_entry *chan,
					  int chan_num, struct wifi_bss *bss, int bss_num,
					  struct chan_entry *cur)
{
	const int *channels;
	int score_num = 0;
	int bandwidth;
	int score = 0;
	int delta = 0;
	int i;

	/* Get adjacent channels we should care */
	bandwidth = wifi_bw_enum2MHz(bw);
	channels = radio_chan2list(channel, bandwidth, sideband);

	for (i = 0; i < chan_num; i++) {
		if (chan[i].channel == cur->channel) {
			cur->score = chan[i].score;
			break;
		}
	}

	libwifi_dbg("[%s] stage1: chan %d/20 ctrl chan score %d\n",
		    name, channel, cur->score);

	/* Get average score value */
	while (channels && *channels) {
		for (i = 0; i < chan_num; i++) {
			if (chan[i].channel != *channels)
				continue;

			/* If any score invalid - report invalid also */
			if (chan[i].score == 255)
				return 255;

			score += chan[i].score;
			score_num++;
			break;
		}
		channels++;
	}

	if (!score_num)
		return 255;

	score = score/score_num;

	libwifi_dbg("[%s] stage2: chan %d/%d avg score %d\n",
		    name, channel, bandwidth, score);

	/* Now check noise */
	if (cur->noise > -95)
		delta = abs(-95 - cur->noise);
	else
		delta = 0;

	if (WARN_ON(score - delta < 0))
		score = 1;
	else
		score -= delta;

	libwifi_dbg("[%s] stage3: chan %d/%d noise %d (delta %d) new score: %d\n",
		    name, channel, bandwidth, cur->noise, delta, score);

	/*
	 * For 80/80+/160 MHz we don't report cntrl channel, so upper layer
	 * should check 20MHz opclass also when choose best controll channel.
	 */
	if (bw == BW80 || bw == BW160 || bw == BW8080)
		return (uint8_t)(score > 255 ? 255 : score);

	/* Now check BSSes */
	if (bss_num < 20)
		delta = cur->bss_num * 3;
	else
		delta = cur->bss_num * 50 / bss_num;

	if (WARN_ON(score - delta < 0))
		score = 1;
	else
		score -= delta;


	libwifi_dbg("[%s] stage4: chan %d/%d bss_num %d (all %d) new score: %d\n",
		    name, channel, bandwidth, cur->bss_num, bss_num, score);

	/* TODO more factors we should check? */

	return (uint8_t)(score > 255 ? 255 : score);
}

static bool radio_chan_supported(uint32_t channel, enum wifi_band band, struct chan_entry *chan, int chan_num)
{
	int i;

	for (i = 0; i < chan_num; i++) {
		if (channel == chan[i].channel &&
		    band == chan[i].band)
			return true;
	}

	return false;
}

static bool radio_channel_supported(struct chan_entry *channel, enum wifi_band band,
				    struct chan_entry *chan, int chan_num)
{
	uint32_t ctrl_chan;
	int i;

	/* Check all ctrl channels supported */
	for (i = 0; i < ARRAY_SIZE(channel->ctrl_channels); i++) {
		ctrl_chan = channel->ctrl_channels[i];

		/* end of table */
		if (!ctrl_chan)
			break;

		if (!radio_chan_supported(ctrl_chan, band, chan, chan_num))
			return false;
	}

	return true;
}

static int radio_update_opclass(const char *name, struct wifi_opclass *opclass, struct chan_entry *chan,
				int chan_num, struct wifi_bss *scan_bss, int scan_bss_num, uint32_t supp_bw)
{
	enum wifi_chan_ext sideband = EXTCH_NONE;
	struct chan_entry *chan_entry;
	uint8_t channel;
	int i;

	for (i = 0; i < opclass->opchannel.num; i++) {
		chan_entry = &opclass->opchannel.ch[i];
		channel = (uint8_t)chan_entry->channel;

		if (!(BIT(opclass->bw) & supp_bw)) {
			chan_entry->score = 0;
			continue;
		}

		if (!radio_channel_supported(chan_entry, opclass->band, chan, chan_num)) {
			chan_entry->score = 0;
			continue;
		}

		/* Prefer 1, 6, 11 for 2.4GHz */
		if (opclass->band == BAND_2 && channel != 1 && channel != 6 && channel != 11) {
			/* Setup lowest preference */
			chan_entry->score = 1;
			continue;
		}

		/* BW8080 mean upper 80MHz (80+) */
		if (opclass->bw == BW8080)
			opclass->bw = BW80;

		/* Check if dfs required for channel/bw */
		chan_entry->dfs = radio_opclass_dfs_required(
						chan_entry->channel,
						opclass->bw,
						chan,
						chan_num);

		/* Recalc busy/noise/bss_num/score for channel/bw */
		switch (opclass->g_opclass) {
		/* We need this for 2.4GHz only */
		case 83:
			sideband = EXTCH_ABOVE;
			break;
		case 84:
			sideband = EXTCH_BELOW;
			break;
		default:
			sideband = EXTCH_NONE;
			break;
		}

		chan_entry->noise = radio_opclass_recalc_noise(
							name,
							chan_entry->channel,
							opclass->bw,
							sideband,
							chan,
							chan_num);

		chan_entry->busy = radio_opclass_recalc_busy(
							name,
							chan_entry->channel,
							opclass->bw,
							sideband,
							chan,
							chan_num);

		chan_entry->bss_num = radio_opclass_bss_num(
							name,
							chan_entry->channel,
							opclass->bw,
							sideband,
							chan,
							chan_num,
							scan_bss,
							scan_bss_num);

		chan_entry->score = radio_opclass_recalc_score(
							name,
							chan_entry->channel,
							opclass->bw,
							sideband,
							chan,
							chan_num,
							scan_bss,
							scan_bss_num,
							chan_entry);

		chan_entry->dfs_state = radio_opclass_dfs_state(
							chan_entry->channel,
							opclass->bw,
							chan,
							chan_num);

		chan_entry->cac_time = radio_dfs_cac_time(
							name,
							chan_entry->channel,
							opclass->bw,
							chan,
							chan_num);

		chan_entry->nop_time = radio_dfs_nop_time(
							name,
							chan_entry->channel,
							opclass->bw,
							chan,
							chan_num);
	}

	/* EasyMesh/MultiAP expect center channel for 80/80+/160/320 MHz */
	switch (opclass->opclass) {
	case 132:
		for (i = 0; i < opclass->opchannel.num ; i++)
			opclass->opchannel.ch[i].channel += 2;
		break;
	case 128:
	case 130:
	case 133:
	case 135:
		for (i = 0; i < opclass->opchannel.num ; i++)
			opclass->opchannel.ch[i].channel += 6;
		break;
	case 129:
	case 134:
		for (i = 0; i < opclass->opchannel.num ; i++)
			opclass->opchannel.ch[i].channel += 14;
		break;
	case 137:
		for (i = 0; i < opclass->opchannel.num ; i++)
			opclass->opchannel.ch[i].channel += 30;
		break;
	}

	return 0;
}

static int radio_update_opclass_channels(const char *name, struct wifi_opclass *opclass,
					 struct chan_entry *chan, int chan_num)
{
	struct chan_entry opclass_ch[64] = {};
	struct chan_entry *chan_entry;
	int opclass_ch_num = 0;
	int i;

	for (i = 0; i < opclass->opchannel.num; i++) {
		chan_entry = &opclass->opchannel.ch[i];
		/* TODO check chan/bw also here */
		if (!radio_channel_supported(chan_entry, opclass->band, chan, chan_num))
			continue;

		if (WARN_ON(opclass_ch_num >= ARRAY_SIZE(opclass_ch)))
			break;

		/* Copy chan_entry */
		memcpy(&opclass_ch[opclass_ch_num], chan_entry, sizeof(*chan_entry));
		opclass_ch_num++;
	}


	if (WARN_ON(opclass_ch_num >= ARRAY_SIZE(opclass->opchannel.ch)))
		return -1;

	memset(opclass->opchannel.ch, 0x0, sizeof(opclass->opchannel.ch));
	memcpy(opclass->opchannel.ch, opclass_ch, opclass_ch_num * sizeof(struct chan_entry));
	opclass->opchannel.num = (uint8_t)opclass_ch_num;

	return (opclass_ch_num ? 0 : -1);
}

int wifi_get_supported_opclass(const char *name, int *num_opclass, struct wifi_opclass *o)
{
	struct chan_entry channel[64];
	int channel_num = ARRAY_SIZE(channel);
	struct wifi_opclass *rd_opclass = NULL;
	struct wifi_opclass *opclass;
	int i, rd_num_opclass;
	uint32_t supp_bw = 0;
	int num, max;
	int ret = 0;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (WARN_ON(!o))
		return -1;

	if (WARN_ON(!num_opclass))
		return -1;

	if (WARN_ON(*num_opclass <= 0))
		return -1;

	max = *num_opclass;
	*num_opclass = 0;

	ret = wifi_get_supp_bandwidths(name, &supp_bw);
	if (WARN_ON(ret))
		return ret;

	ret = wifi_channels_info(name, channel, &channel_num);
	if (WARN_ON(ret))
		return ret;

	rd_opclass = calloc(max, sizeof(struct wifi_opclass));
	WARN_ON(!rd_opclass);
	if (!rd_opclass)
		return -1;

	rd_num_opclass = max;
	ret  = _radio_get_supported_opclass(name, &rd_num_opclass, rd_opclass);
	if (WARN_ON(ret))
		goto end;

	/* filter out opclass got from regdomain by driver supported bandwidhts/channels */
	num = 0;
	for (i = 0; i < rd_num_opclass; i++) {
		opclass = &rd_opclass[i];

		if (WARN_ON(num >= max))
			break;

		if (!(BIT(opclass->bw) & supp_bw))
			continue;

		if (radio_update_opclass_channels(name, opclass, channel, channel_num))
			continue;

		/* Finally if bw/channel(s) supported */
		memcpy(&o[num], opclass, sizeof(*opclass));
		num++;
	}

	*num_opclass = num;
end:
	free(rd_opclass);
	return ret;
}

bool wifi_is_dfs_usable(const char *name, int chan, enum wifi_bw bw)
{
	struct chan_entry channel[64];
	int channel_num = ARRAY_SIZE(channel);
	int ret;

	ret = wifi_channels_info(name, channel, &channel_num);
	if (WARN_ON(ret))
		return false;

	return radio_opclass_dfs_usable(chan, bw, channel, channel_num);
}

int wifi_get_opclass_pref(const char *name, int *num_opclass, struct wifi_opclass *o)
{
	struct chan_entry channel[64];
	int channel_num = ARRAY_SIZE(channel);
	struct wifi_bss scan_bss[128]={0};
	int scan_bss_num = ARRAY_SIZE(scan_bss);
	struct wifi_opclass *rd_opclass = NULL;
	struct wifi_opclass *opclass;
	int i, rd_num_opclass;
	uint32_t supp_bw = 0;
	int num, max;
	int ret = 0;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (WARN_ON(!o))
		return -1;

	if (WARN_ON(!num_opclass))
		return -1;

	if (WARN_ON(*num_opclass <= 0))
		return -1;

	max = *num_opclass;
	*num_opclass = 0;

	ret = wifi_get_supp_bandwidths(name, &supp_bw);
	if (WARN_ON(ret))
		return ret;

	ret = wifi_channels_info(name, channel, &channel_num);
	if (WARN_ON(ret))
		return ret;

	WARN_ON(wifi_get_scan_results(name, scan_bss, &scan_bss_num));

	rd_opclass = calloc(max, sizeof(struct wifi_opclass));
	WARN_ON(!rd_opclass);
	if (!rd_opclass)
		return -1;

	rd_num_opclass = max;
	ret  = _radio_get_supported_opclass(name, &rd_num_opclass, rd_opclass);
	if (WARN_ON(ret))
		goto end;

	/* filter out opclass got from regdomain by driver supported bandwidhts/channels */
	num = 0;
	for (i = 0; i < rd_num_opclass; i++) {
		opclass = &rd_opclass[i];

		if (WARN_ON(num >= max))
			break;

		radio_update_opclass(name, opclass, channel, channel_num,
				     scan_bss, scan_bss_num, supp_bw);

		/* Finally if bw/channel(s) supported */
		memcpy(&o[num], opclass, sizeof(*opclass));
		num++;
	}

	*num_opclass = num;
end:
	free(rd_opclass);
	return ret;
}
