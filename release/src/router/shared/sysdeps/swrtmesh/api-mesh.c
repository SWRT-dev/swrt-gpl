/*
 * Copyright 2022, SWRTdev
 * Copyright 2022, paldier <paldier@hotmail.com>.
 * Copyright 2022, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 */

#include <stdio.h>
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
	char tmp[18], ifname[6];
//skip main ap and guest wifi
	for(i = 0; i < MAX_NO_MSSID; i++){
		snprintf(tmp, sizeof(tmp), "wl%d.%d_bss_enabled", band, i);
		if(nvram_match(tmp, "1"))
			index++;
	}

	if(band)
#if defined(RTCONFIG_MT798X)
		snprintf(ifname, sizeof(ifname), "rax%d", index);
#else
		snprintf(ifname, sizeof(ifname), "rai%d", index);
#endif
	else
		snprintf(ifname, sizeof(ifname), "ra%d", index);
#elif defined(RTCONFIG_QCA)
#error fixme
#elif defined(RTCONFIG_LANTIQ)
#error fixme
#endif
	return ifname;
}

void check_mssid_prelink_reset(uint32_t sf)
{
}
