#if !defined(RTCONFIG_WLMODULE_MT7915D_AP) && !defined(RTCONFIG_MT798X)
int MCSMappingRateTable[] =
	{2,  4,   11,  22, // CCK
	12, 18,   24,  36, 48, 72, 96, 108, // OFDM
	13, 26,   39,  52,  78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260, // 20MHz, 800ns GI, MCS: 0 ~ 15
	39, 78,  117, 156, 234, 312, 351, 390,										  // 20MHz, 800ns GI, MCS: 16 ~ 23
	27, 54,   81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540, // 40MHz, 800ns GI, MCS: 0 ~ 15
	81, 162, 243, 324, 486, 648, 729, 810,										  // 40MHz, 800ns GI, MCS: 16 ~ 23
	14, 29,   43,  57,  87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288, // 20MHz, 400ns GI, MCS: 0 ~ 15
	43, 87,  130, 173, 260, 317, 390, 433,										  // 20MHz, 400ns GI, MCS: 16 ~ 23
	30, 60,   90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600, // 40MHz, 400ns GI, MCS: 0 ~ 15
	90, 180, 270, 360, 540, 720, 810, 900,
	13, 26,   39,  52,  78, 104, 117, 130, 156, /* 11ac: 20Mhz, 800ns GI, MCS: 0~8 */
	27, 54,   81, 108, 162, 216, 243, 270, 324, 360, /*11ac: 40Mhz, 800ns GI, MCS: 0~9 */
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /*11ac: 80Mhz, 800ns GI, MCS: 0~9 */
	14, 29,   43,  57,  87, 115, 130, 144, 173, /* 11ac: 20Mhz, 400ns GI, MCS: 0~8 */
	30, 60,   90, 120, 180, 240, 270, 300, 360, 400, /*11ac: 40Mhz, 400ns GI, MCS: 0~9 */
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867 /*11ac: 80Mhz, 400ns GI, MCS: 0~9 */
	};

int MCSMappingRateTable_5G[] = {
	2,  4, 11, 22, 12,  18,  24,  36, 48,  72,  96, 108, 109, 110, 111, 112,/* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */

	/*for 11ac:20 Mhz 800ns GI*/
	6,  13, 19, 26,  39,  52,  58,  65,  78,  0,     /*1ss mcs 0~8*/
	13, 26, 39, 52,  78,  104, 117, 130, 156, 0,     /*2ss mcs 0~8*/
	19, 39, 58, 78,  117, 156, 175, 195, 234, 260,   /*3ss mcs 0~9*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 0,     /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 800ns GI*/
	13,	27,	40,	54,	 81,  108, 121, 135, 162, 180,   /*1ss mcs 0~9*/
	27,	54,	81,	108, 162, 216, 243, 270, 324, 360,   /*2ss mcs 0~9*/
	40,	81,	121, 162, 243, 324, 364, 405, 486, 540,  /*3ss mcs 0~9*/
	54,	108, 162, 216, 324, 432, 486, 540, 648, 720, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 800ns GI*/
	29,	58,	87,	117, 175, 234, 263, 292, 351, 390,   /*1ss mcs 0~9*/
	58,	117, 175, 243, 351, 468, 526, 585, 702, 780, /*2ss mcs 0~9*/
	87,	175, 263, 351, 526, 702, 0,	877, 1053, 1170, /*3ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 800ns GI*/
	58,	117, 175, 234, 351, 468, 526, 585, 702, 780, /*1ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*2ss mcs 0~9*/
	175, 351, 526, 702, 1053, 1404, 1579, 1755, 2160, 0, /*3ss mcs 0~8*/
	234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120, /*4ss mcs 0~9*/

	/*for 11ac:20 Mhz 400ns GI*/
	7,	14,	21,	28,  43,  57,   65,	 72,  86,  0,    /*1ss mcs 0~8*/
	14,	28,	43,	57,	 86,  115,  130, 144, 173, 0,    /*2ss mcs 0~8*/
	21,	43,	65,	86,	 130, 173,  195, 216, 260, 288,  /*3ss mcs 0~9*/
	28,	57,	86,	115, 173, 231,  260, 288, 346, 0,    /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 400ns GI*/
	15,	30,	45,	60,	 90,  120,  135, 150, 180, 200,  /*1ss mcs 0~9*/
	30,	60,	90,	120, 180, 240,  270, 300, 360, 400,  /*2ss mcs 0~9*/
	45,	90,	135, 180, 270, 360,  405, 450, 540, 600, /*3ss mcs 0~9*/
	60,	120, 180, 240, 360, 480,  540, 600, 720, 800, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 400ns GI*/
	32,	65,	97,	130, 195, 260,  292, 325, 390, 433,  /*1ss mcs 0~9*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*2ss mcs 0~9*/
	97,	195, 292, 390, 585, 780,  0,	 975, 1170, 1300, /*3ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 400ns GI*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*1ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*2ss mcs 0~9*/
	195, 390, 585, 780, 1170, 1560,	1755, 1950, 2340, 0, /*3ss mcs 0~8*/
	260, 520, 780, 1040, 1560, 2080,	2340, 2600, 3120, 3466, /*4ss mcs 0~9*/

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37
}; /* 3*3 */

#elif defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
int MCSMappingRateTable[] = {
	2,  4, 11, 22, 12,  18,  24,  36, 48,  72,  96, 108, 109, 110, 111, 112,/* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26,  52,  78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59,   87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */

	/*for 11ac:20 Mhz 800ns GI*/
	6,  13, 19, 26,  39,  52,  58,  65,  78,  90,     /*1ss mcs 0~8*/
	13, 26, 39, 52,  78,  104, 117, 130, 156, 180,     /*2ss mcs 0~8*/
	19, 39, 58, 78,  117, 156, 175, 195, 234, 260,   /*3ss mcs 0~9*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 360,     /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 800ns GI*/
	13,	27,	40,	54,	 81,  108, 121, 135, 162, 180,   /*1ss mcs 0~9*/
	27,	54,	81,	108, 162, 216, 243, 270, 324, 360,   /*2ss mcs 0~9*/
	40,	81,	121, 162, 243, 324, 364, 405, 486, 540,  /*3ss mcs 0~9*/
	54,	108, 162, 216, 324, 432, 486, 540, 648, 720, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 800ns GI*/
	29,	58,	87,	117, 175, 234, 263, 292, 351, 390,   /*1ss mcs 0~9*/
	58,	117, 175, 243, 351, 468, 526, 585, 702, 780, /*2ss mcs 0~9*/
	87,	175, 263, 351, 526, 702, 0,	877, 1053, 1170, /*3ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 800ns GI*/
	58,	117, 175, 234, 351, 468, 526, 585, 702, 780, /*1ss mcs 0~9*/
	117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560, /*2ss mcs 0~9*/
	175, 351, 526, 702, 1053, 1404, 1579, 1755, 2160, 0, /*3ss mcs 0~8*/
	234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120, /*4ss mcs 0~9*/

	/*for 11ac:20 Mhz 400ns GI*/
	7,	14,	21,	28,  43,  57,   65,	 72,  86,  100,    /*1ss mcs 0~8*/
	14,	28,	43,	57,	 86,  115,  130, 144, 173, 200,    /*2ss mcs 0~8*/
	21,	43,	65,	86,	 130, 173,  195, 216, 260, 288,  /*3ss mcs 0~9*/
	28,	57,	86,	115, 173, 231,  260, 288, 346, 400,    /*4ss mcs 0~8*/

	/*for 11ac:40 Mhz 400ns GI*/
	15,	30,	45,	60,	 90,  120,  135, 150, 180, 200,  /*1ss mcs 0~9*/
	30,	60,	90,	120, 180, 240,  270, 300, 360, 400,  /*2ss mcs 0~9*/
	45,	90,	135, 180, 270, 360,  405, 450, 540, 600, /*3ss mcs 0~9*/
	60,	120, 180, 240, 360, 480,  540, 600, 720, 800, /*4ss mcs 0~9*/

	/*for 11ac:80 Mhz 400ns GI*/
	32,	65,	97,	130, 195, 260,  292, 325, 390, 433,  /*1ss mcs 0~9*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*2ss mcs 0~9*/
	97,	195, 292, 390, 585, 780,  0,	 975, 1170, 1300, /*3ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*4ss mcs 0~9*/

	/*for 11ac:160 Mhz 400ns GI*/
	65,	130, 195, 260, 390, 520,  585, 650, 780, 866, /*1ss mcs 0~9*/
	130, 260, 390, 520, 780, 1040,	1170, 1300, 1560, 1733, /*2ss mcs 0~9*/
	195, 390, 585, 780, 1170, 1560,	1755, 1950, 2340, 0, /*3ss mcs 0~8*/
	260, 520, 780, 1040, 1560, 2080,	2340, 2600, 3120, 3466, /*4ss mcs 0~9*/

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37
}; /* 3*3 */
#endif

#define MAX_NUM_HE_BANDWIDTHS 4
#define MAX_NUM_HE_SPATIAL_STREAMS 4
#define MAX_NUM_HE_MCS_ENTRIES 12
unsigned short he_mcs_phyrate_mapping_table[MAX_NUM_HE_BANDWIDTHS][MAX_NUM_HE_SPATIAL_STREAMS][MAX_NUM_HE_MCS_ENTRIES] = {
	{ /*20 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 8, 17, 25, 34, 51, 68, 77, 86, 103, 114, 129, 143
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 17, 34, 51, 68, 103, 137, 154, 172, 206, 229, 258, 286
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 25, 51, 77, 103, 154, 206, 232, 258, 309, 344, 387, 430
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 34, 68, 103, 137, 206, 275, 309, 344, 412, 458, 516, 573
		}
	},
	{ /*40 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 17, 34, 51, 68, 103, 137, 154, 172, 206, 229, 258, 286
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 34, 68, 103, 137, 206, 275, 309, 344, 412, 458, 516, 573
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 51, 103, 154, 206, 309, 412, 464, 516, 619, 688, 774, 860
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 68, 137, 206, 275, 412, 550, 619, 688, 825, 917, 1032, 1147
		}
	},
	{ /*80 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			36, 72, 108, 144, 216, 288, 324, 360, 432, 480, 540, 600
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 72, 144, 216, 288, 432, 576, 648, 720, 864, 960, 1080, 1201
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 108, 216, 324, 432, 648, 864, 972, 1080, 1297, 1441, 1621, 1801
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 144, 288, 432, 576, 864, 1152, 1297, 1141, 1729, 1921, 2161, 2401
		}
	},
	{ /*160 Mhz*/
		/* 1 SS */
		{
			/* DCM 0*/
			 72, 144, 216, 288, 432, 576, 648, 720, 864, 960, 1080, 1201
		},
		/* 2 SS */
		{
			/* DCM 0 */
			 144, 288, 432, 576, 864, 1152, 1297, 1441, 1729, 1921, 2161, 2401
		},
		/* 3 SS */
		{
			/* DCM 0 */
			 216, 432, 648, 864, 1297, 1729, 1945, 2161, 2594, 2882, 3242, 3602
		},
		/* 4 SS */
		{
			/* DCM 0 */
			 288, 576, 864, 1152, 1729, 2305, 2594, 2882, 3458, 3843, 4323, 4803
		},
	}
};

unsigned int mtk_mcs_to_rate(unsigned char mcs, unsigned char phy_mode, unsigned char bw, unsigned char sgi, unsigned char vht_nss, int unit)
{
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
#define MCSMAPTAB_5G MCSMappingRateTable
#else
#define MCSMAPTAB_5G MCSMappingRateTable_5G
#endif
#define MCSMAPTAB MCSMappingRateTable

	unsigned char Antenna = 0;
	int rate_count;
	int rate_index = 0;
	int value = 0;

	if(unit)
		rate_count = sizeof(MCSMAPTAB_5G)/sizeof(int);
	else
		rate_count = sizeof(MCSMAPTAB)/sizeof(int);
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if (phy_mode >= MODE_HE){
		if (vht_nss == 0) {	
			vht_nss = 1;
		}
		if (mcs >= MAX_NUM_HE_MCS_ENTRIES)
			mcs = MAX_NUM_HE_MCS_ENTRIES - 1;
		if (vht_nss > MAX_NUM_HE_SPATIAL_STREAMS)
			vht_nss = MAX_NUM_HE_SPATIAL_STREAMS;
		if (bw >= MAX_NUM_HE_BANDWIDTHS)
			bw = MAX_NUM_HE_BANDWIDTHS - 1;
		vht_nss--;
		value = he_mcs_phyrate_mapping_table[bw][vht_nss][mcs];
		return value;
	}else 
#endif
	if (phy_mode >= MODE_VHT){
		if (bw == BW_20)
			rate_index = 112 + ((vht_nss - 1) * 10) + (sgi * 160) + mcs;
		else if (bw == BW_40)
			rate_index = 152 + ((vht_nss - 1) * 10) + (sgi * 160) + mcs;
		else if (bw == BW_80)
			rate_index = 192 + ((vht_nss - 1) * 10) + (sgi * 160) + mcs;
		else if (bw == BW_160)
			rate_index = 232 + ((vht_nss - 1) * 10) + (sgi * 160) + mcs;
	}else if (phy_mode >= MODE_HTMIX){
		if ((phy_mode == MODE_HTMIX) || (phy_mode == MODE_HTGREENFIELD))
			Antenna = (mcs >> 3) + 1;
			/* map back to 1SS MCS , multiply by antenna numbers later */
		if (mcs > 7)
			mcs %= 8;
		rate_index = 16 + (bw * 24) + (sgi * 48) + mcs;
	}else if (phy_mode == MODE_OFDM)
		rate_index = mcs + 4;
	else if (phy_mode == MODE_CCK)
		rate_index = mcs;
	if (rate_index < 0)
		rate_index = 0;
	if (rate_index >= rate_count)
		rate_index = rate_count-1;
	if(unit){
		if (phy_mode < MODE_VHT)
			value = (MCSMAPTAB_5G[rate_index] * 5) / 10;
		else
			value =  MCSMAPTAB_5G[rate_index];
	}else{
		if (phy_mode < MODE_VHT)
			value = (MCSMAPTAB[rate_index] * 5) / 10;
		else
			value =  MCSMAPTAB[rate_index];
	}
	if (phy_mode >= MODE_HTMIX && phy_mode < MODE_VHT)
		value *= Antenna;
	return value;
}

static void mtk_parse_ratedata(unsigned int ratedata, unsigned char *phymode, unsigned char *mcs, unsigned char *bw,
	unsigned char *vht_nss,	 unsigned char *sgi, unsigned char *stbc)
{
	*phymode = (ratedata >> 13) & 0x7;
	*mcs = ratedata & 0x3F;
	*bw = (ratedata >> 7) & 0x3;
	*sgi = (ratedata >> 9) & 0x1;
	*stbc = (ratedata >> 10) & 0x1;
	*vht_nss = 0;

	if ( *phymode >= MODE_VHT ) {
		*vht_nss = ((*mcs & (0x3 << 4)) >> 4) + 1;
		*mcs &= 0xF;
	}
}

void MTK_stainfo(int unit)
{
	struct iwreq wrq;
	int i, firstRow;
#if defined(RTCONFIG_MT798X)
	char data[sizeof(RT_802_11_MAC_TABLE_5G)]; // 2G, 5G use same structure
#else
	char data[16384];
#endif
	char mac[18];
	RT_802_11_MAC_TABLE_5G *mp;
	char *value;
	char rssi, cnt;
	int xTxR;
	char *ifname = NULL;
	char prefix[] = "wlXXXXXXXXXX_", tmp[128];
	STA_INFO_TABLE *sta_info_tab = NULL;
	unsigned int wireless_type = 1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	strcpy(stpcpy(tmp, prefix), "ifname");
	ifname = nvram_safe_get(tmp);

	/* query wl for authenticated sta list */
	memset(data, 0, sizeof(data));
	wrq.u.data.pointer = data;
	wrq.u.data.length = sizeof(data);
	wrq.u.data.flags = 0;
	if(wl_ioctl(ifname, RTPRIV_IOCTL_GET_MAC_TABLE, &wrq) >= 0){
		mp = (RT_802_11_MAC_TABLE_5G *)wrq.u.data.pointer;
		strcpy(stpcpy(tmp, prefix), "HT_RxStream");
		xTxR = nvram_get_int(tmp);
		if(!strcmp(ifname, WIF_2G))
			wireless_type = 1;
		else if(!strcmp(ifname, WIF_5G))
			wireless_type = 2;
//		else if(!strcmp(ifname, WIF_5G2))
//			wireless_type = 3;
		if(mp->Num > 0){
			int hr, min, sec;
			unsigned int tx_ratedata = 0;
			unsigned int rx_ratedata = 0;
			unsigned char phy, mcs, bw, vht_nss, sgi, stbc;
			unsigned char r_phy, r_mcs, r_bw, r_vht_nss, r_sgi, r_stbc;
			for(i = 0; i < mp->Num; i++){
				sta_info_tab = (STA_INFO_TABLE *)calloc(1, sizeof(STA_INFO_TABLE));
				if(sta_info_tab == NULL){
					_dprintf("%s: calloc failed.\n", __func__);
					return;
				}
				rssi = cnt = 0;
				memcpy(sta_info_tab->mac_addr, mp->Entry[i].Addr, sizeof(sta_info_tab->mac_addr));
				sta_info_tab->wireless = wireless_type;
				if(mp->Entry[i].AvgRssi0 && cnt < xTxR){
					rssi += mp->Entry[i].AvgRssi0;
					cnt++;
				}
				if(mp->Entry[i].AvgRssi1 && cnt < xTxR){
					rssi += mp->Entry[i].AvgRssi1;
					cnt++;
				}
				if(mp->Entry[i].AvgRssi2 && cnt < xTxR){
					rssi += mp->Entry[i].AvgRssi2;
					cnt++;
				}
				if(cnt == 0)
					continue;	//skip this sta info
				sta_info_tab->rssi = ~((unsigned int)rssi / cnt) + 1;
				tx_ratedata = (unsigned int)mp->Entry[i].TxRate.word;
#if !defined(RTCONFIG_MT798X)
				rx_ratedata = mp->Entry[i].LastRxRate;
#endif
				mtk_parse_ratedata(tx_ratedata, &phy, &mcs, &bw, &vht_nss, &sgi, &stbc);
				if(rx_ratedata)
					mtk_parse_ratedata(rx_ratedata, &r_phy, &r_mcs, &r_bw, &r_vht_nss, &r_sgi, &r_stbc);
				hr = (mp->Entry[i].ConnectedTime) / 3600;
				min = (mp->Entry[i].ConnectedTime) % 3600 / 60;
				sec = mp->Entry[i].ConnectedTime - hr * 3600 - min * 60;
				snprintf(sta_info_tab->conn_time, sizeof(sta_info_tab->conn_time), "%02d:%02d:%02d", hr, min, sec);
				if(wireless_type == 1){
					snprintf(sta_info_tab->txrate, sizeof(sta_info_tab->txrate), "%dM", mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 0));
#if !defined(RTCONFIG_MT798X)
					if(rx_ratedata)
						snprintf(sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate), "%dM", mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 0));
#endif
				}else{
					snprintf(sta_info_tab->txrate, sizeof(sta_info_tab->txrate), "%dM", mtk_mcs_to_rate(mcs, phy, bw, sgi, vht_nss, 1));
#if !defined(RTCONFIG_MT798X)
					if(rx_ratedata)
						snprintf(sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate), "%dM", mtk_mcs_to_rate(r_mcs, r_phy, r_bw, r_sgi, r_vht_nss, 1));
#endif
				}
#if defined(RTCONFIG_MT798X)
				snprintf(sta_info_tab->rxrate, sizeof(sta_info_tab->rxrate), "%ldM", mp->Entry[i].LastRxRate);
#endif
				if(g_show_sta_info && f_exists("/tmp/conn_debug"))
					_dprintf("%s[%3d,MTK] %02X%02X%02X%02X%02X%02X, rssi: %d\n", "[connection log]", i, sta_info_tab->mac_addr[0], sta_info_tab->mac_addr[1],
						sta_info_tab->mac_addr[2], sta_info_tab->mac_addr[3], sta_info_tab->mac_addr[4], sta_info_tab->mac_addr[5], sta_info_tab->rssi);
				sta_info_tab->next = NULL;
				if(g_sta_info_tab == NULL){
					g_sta_info_tab = sta_info_tab;
				}else{
					sta_info_tab->next = g_sta_info_tab;
					g_sta_info_tab = sta_info_tab;
				}
			}
		}
	}
}
