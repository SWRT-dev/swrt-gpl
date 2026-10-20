/*
 * Copyright 2021-2026, SWRTdev
 * Copyright 2021-2026, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <shared.h>

//#define SC_AUTH_DEBUG

static int show_help(void)
{
	printf("usage:\n");
	printf("sc_auth tcode\n");
	printf("\tset softcenter server\n");
	printf("sc_auth arch\n");
	printf("\tset softcenter arch\n");
	return 0;
}

int main(int argc, char **argv)
{
	int model;
	char buf[256];
	char cmd[40];
	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));
#if defined(SC_AUTH_DEBUG)
	printf("DEBUG MODE\n");
#endif
	if(argc == 2){
		if(!strcmp(argv[1], "tcode")){
			if(!strncmp(nvram_safe_get("territory_code"), "CN", 2))
				snprintf(cmd, sizeof(cmd), "dbus set softcenter_server_tcode=%s", "CN");
			else
				snprintf(cmd, sizeof(cmd), "dbus set softcenter_server_tcode=%s", "GB");
			if(is_swrt_mod()){
				model = get_modelname();
				if(model == SWRT_MODEL_R7000P || model == SWRT_MODEL_SBRAC1900P || model == SWRT_MODEL_SBRAC3200P
					|| model == SWRT_MODEL_R8000P || model == SWRT_MODEL_K3 || model == SWRT_MODEL_XWR3100)
					snprintf(cmd, sizeof(cmd), "dbus set softcenter_server_tcode=%s", "CN");
			}
#if defined(SC_AUTH_DEBUG)
			printf("%s\n", cmd);
#else
			system(cmd);
#endif
		}else if(!strcmp(argv[1], "arch")){
			snprintf(buf, sizeof(buf), "dbus set softcenter_arch=%s", nvram_get("sc_arch") ? : "armng");
#if defined(SC_AUTH_DEBUG)
			printf("%s\n", buf);
#else
			system(buf);
#endif
		}else
			show_help();
	}else
		show_help();
	return 0;
}

