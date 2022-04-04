#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <bcmdevs.h>
#include "shared.h"

struct model_s {
	char *pid;
	int model;
};

static const struct model_s model_list[] = {
	{ "RT-ACRH18",		MODEL_RTACRH18		},
	{ "RT-AC85U",		MODEL_RTAC85U		},
	{ "RT-AC85P",		MODEL_RTAC85P		},
	{ "RT-ACRH26",		MODEL_RTACRH26		},
	{ "TUF-AC1750",		MODEL_TUFAC1750		},
	{ "RT-AC65U",		MODEL_RTAC85U		},
	{ "4G-AC86U",		MODEL_RT4GAC86U		},
	{ "4G-AX56",		MODEL_RT4GAX56		},
	{ "RT-AX53U",		MODEL_RTAX53U		},
	{ "RT-AX54",		MODEL_RTAX54		},
	{ "RT-AC59U",		MODEL_RTAC59U		},
	{ "RT-AC59U_V2",	MODEL_RTAC59U		},
	{ "PL-AX56_XP4",	MODEL_PLAX56XP4		},
	{ "RT-AC58U",		MODEL_RTAC58U		},
	{ "4G-AC53U",		MODEL_RT4GAC53U		},
	{ "4G-AC56",		MODEL_RT4GAC56		},
	{ "RT-AC82U",		MODEL_RTAC82U		},
	{ "MAP-AC1300",		MODEL_MAPAC1300		},
	{ "MAP-AC2200",		MODEL_MAPAC2200		},
	{ "VZW-AC1300",		MODEL_VZWAC1300		},
	{ "SH-AC1300",		MODEL_SHAC1300		},
	{ "MAP-AC1750",		MODEL_MAPAC1750		},
	{ "RT-AC59_CD6R",	MODEL_RTAC59CD6R	},
	{ "RT-AC59_CD6N",	MODEL_RTAC59CD6N	},
	{ "RT-AC95U",		MODEL_RTAC95U		},
	{ "RT-AC88N",		MODEL_RTAC88N		},
	{ "BRT-AC828",		MODEL_BRTAC828		},
	{ "RT-AC88S",		MODEL_RTAC88S		},
	{ "RT-AD7200",		MODEL_RTAD7200		},
	{ "GT-AXY16000",	MODEL_GTAXY16000	},
	{ "GT-AX6000N",		MODEL_GTAX6000N		},
	{ "RT-AX89U",		MODEL_RTAX89U		},
	{ "ETJ",			MODEL_ETJ			},
	{ "BLUECAVE",		MODEL_BLUECAVE		},
	{ "BLUE_CAVE",		MODEL_BLUECAVE		},
	{ "RT-AC68U",		MODEL_RTAC68U		},
	{ "RT-AC3200",		MODEL_RTAC3200		},
	{ "RT-AC88U",		MODEL_RTAC88U		},
	{ "RT-AC86U",		MODEL_RTAC86U		},
	{ "GT-AC2900",		MODEL_RTAC86U		},
	{ "RT-AC3100",		MODEL_RTAC3100		},
	{ "RT-AC5300",		MODEL_RTAC5300		},
	{ "GT-AC5300",		MODEL_GTAC5300		},
	{ "GT-AX11000",		MODEL_GTAX11000		},
	{ "RT-AX88U",		MODEL_RTAX88U		},
	{ "RT-AX92U",		MODEL_RTAX92U		},
	{ "RT-AX95Q",		MODEL_RTAX95Q		},
	{ "XT8PRO",		MODEL_XT8PRO		},
	{ "RT-AXE95Q",		MODEL_RTAXE95Q		},
	{ "ET8PRO",		MODEL_ET8PRO		},
	{ "RT-AX56_XD4",	MODEL_RTAX56_XD4	},
	{ "XD4PRO",	MODEL_XD4PRO	},
	{ "CT-AX56_XD4",	MODEL_CTAX56_XD4	},
	{ "RT-AX58U",		MODEL_RTAX58U		},
	{ "TUF-AX3000",		MODEL_RTAX58U		},
	{ "TUF-AX5400",     MODEL_RTAX58U		},
	{ "RT-AX82U",       MODEL_RTAX58U		},
	{ "RT-AX82_XD6",	MODEL_RTAX58U		},
	{ "GS-AX3000",		MODEL_RTAX58U		},
	{ "GS-AX5400",		MODEL_RTAX58U		},
	{ "RT-AX58U_V2",	MODEL_RTAX58U_V2	},
	{ "RT-AX56U",		MODEL_RTAX56U		},
	{ "RP-AX56",        MODEL_RPAX56        },
	{ "RP-AX58",            MODEL_RPAX58            },
	{ "RT-AX55",		MODEL_RTAX55		},
	{ "RT-AX1800",		MODEL_RTAX55		},
	{ "RT-AX86U",		MODEL_RTAX86U		},
	{ "RT-AX5700",		MODEL_RTAX86U		},
	{ "RT-AX86S",		MODEL_RTAX86U		},
	{ "RT-AX68U",		MODEL_RTAX68U		},
	{ "RT-AC68U_V4",	MODEL_RTAC68U_V4	},
	{ "GT-AX6000",		MODEL_GTAX6000		},
	{ "GT-AXE11000",	MODEL_GTAXE11000	},
	{ "GT-AX11000_PRO", MODEL_GTAX11000_PRO },
	{ "GT-AXE16000",	MODEL_GTAXE16000	},
	{ "ET12",			MODEL_ET12			},
	{ "XT12",			MODEL_XT12			},
	{ "DSL-AX82U",		MODEL_DSLAX82U		},
	{ "K3", 			MODEL_K3 			},
	{ "XWR3100", 		MODEL_XWR3100 		},
	{ "R7000P", 		MODEL_R7000P 		},
	{ "EA6700", 		MODEL_EA6700 		},
	{ "SBRAC1900P", 	MODEL_SBRAC1900P 	},
	{ "F9K1118", 		MODEL_F9K1118 		},
	{ "SBRAC3200P", 	MODEL_SBRAC3200P 	},
	{ "R8500", 			MODEL_R8500 		},
	{ "R8000P", 		MODEL_R8000P 		},
	{ "K3C", 			MODEL_K3C 			},
	{ "TY6201_RTK", 	MODEL_TY6201_RTK 	},
	{ "TY6201_BCM", 	MODEL_TY6201_BCM 	},
	{ "RAX120", 		MODEL_RAX120 		},
	{ "DIR868L", 		MODEL_DIR868L 		},
	{ "R6300V2", 		MODEL_R6300V2 		},
	{ "RM-AC2100", 		MODEL_RMAC2100 		},
	{ "R6800", 			MODEL_R6800 		},
	{ "RAX70", 			MODEL_RAX70 		},
	{ "MS60", 			MODEL_MS60 			},
	{ "MR60", 			MODEL_MR60 			},
	{ "PGB-M1", 		MODEL_PGBM1 		},
	{ NULL, 0 },
};

#if defined(RTCONFIG_RALINK)
#elif defined(RTCONFIG_QCA)
#else
static int get_model_by_hw(void)
{
	char *hw_version = nvram_safe_get("hardware_version");

	if (strncmp(hw_version, "RTN12", 5) == 0) {
		if (strncmp(hw_version, "RTN12B1", 7) == 0)
			return MODEL_RTN12B1;
		if (strncmp(hw_version, "RTN12C1", 7) == 0)
			return MODEL_RTN12C1;
		if (strncmp(hw_version, "RTN12D1", 7) == 0)
			return MODEL_RTN12D1;
		if (strncmp(hw_version, "RTN12VP", 7) == 0)
			return MODEL_RTN12VP;
		if (strncmp(hw_version, "RTN12HP_B1", 10) == 0)
			return MODEL_RTN12HP_B1;
		if (strncmp(hw_version, "RTN12HP", 7) == 0)
			return MODEL_RTN12HP;
		if (strncmp(hw_version, "APN12HP", 7) == 0)
			return MODEL_APN12HP;
		return MODEL_RTN12;
	} else if (strncmp(hw_version, "APN12", 5) == 0) {
		if (strncmp(hw_version, "APN12HP", 7) == 0)
			return MODEL_APN12HP;
		return MODEL_APN12;
	}
	return MODEL_UNKNOWN;
}
#endif

#define BLV_MAX 4
#define BL_VERSION(a,b,c,d) (((a) << 12) + ((b) << 8) + ((c) << 4) + (d))

int get_blver(char *bls)
{
	int bv[BLV_MAX], blver=0, i=0;
	char *tok, *delim = ".";
	char buf[32], *bp=NULL;

	memset(buf, 0, sizeof(buf));
	if(bls)
		strlcpy(buf, bls, sizeof(buf));
	bp = buf;
	while((tok = strsep((char**)&bp, delim)))
		if(i < BLV_MAX)
			bv[i++] = atoi(tok);

	blver = BL_VERSION(bv[0], bv[1], bv[2], bv[3]);

	return blver;
}

int get_fwver(char *buildno, char *extendno)
{
	
	return atoi(buildno)*100000 + atoi(extendno);
}

/* returns MODEL ID
 * result is cached for safe multiple use */
int get_model(void)
{
	static int model = MODEL_UNKNOWN;
	char *pid;
	const struct model_s *p;

	if (model != MODEL_UNKNOWN)
		return model;

	pid = nvram_safe_get("productid");
	for (p = &model_list[0]; p->pid; ++p) {
		if (!strcmp(pid, p->pid)) {
			model = p->model;
			break;
		}
	}
	
#if defined(RTCONFIG_RALINK)
#elif defined(RTCONFIG_QCA)
#else
	if (model == MODEL_RTN12)
		model = get_model_by_hw();
	if (model == MODEL_APN12)
		model = get_model_by_hw();
#endif
	return model;
}

#if defined(RTCONFIG_RALINK)
#elif defined(RTCONFIG_QCA)
#else
/* returns product id */
char *get_modelid(int model)
{
	char *pid = "unknown";
	const struct model_s *p;

	for (p = &model_list[0]; p->pid; ++p) {
		if (model == p->model) {
			pid = p->pid;
			break;
		}
	}
	return pid;
}
#endif

/* returns SWITCH ID
 * result is cached for safe multiple use */
int get_switch(void)
{
	static int sw_model = SWITCH_UNKNOWN;

	if (sw_model != SWITCH_UNKNOWN)
		return sw_model;

	sw_model = get_switch_model();
	return sw_model;
}

static const struct model_s modelname_list[] = {
	{ "K3", 		SWRT_MODEL_K3 },
	{ "XWR3100", 	SWRT_MODEL_XWR3100 },
	{ "R7000P", 	SWRT_MODEL_R7000P },
	{ "EA6700", 	SWRT_MODEL_EA6700 },
	{ "SBRAC1900P", SWRT_MODEL_SBRAC1900P },
	{ "F9K1118", 	SWRT_MODEL_F9K1118 },
	{ "SBRAC3200P", SWRT_MODEL_SBRAC3200P },
	{ "R8500", 		SWRT_MODEL_R8500 },
	{ "R8000P", 	SWRT_MODEL_R8000P },
	{ "K3C", 		SWRT_MODEL_K3C },
	{ "TY6201_RTK", SWRT_MODEL_TY6201_RTK },
	{ "TY6201_BCM", SWRT_MODEL_TY6201_BCM },
	{ "TY6202", 	SWRT_MODEL_TY6202 },
	{ "RAX120", 	SWRT_MODEL_RAX120 },
	{ "DIR868L", 	SWRT_MODEL_DIR868L },
	{ "R6300V2", 	SWRT_MODEL_R6300V2 },
	{ "MR60", 		SWRT_MODEL_MR60 },
	{ "MS60", 		SWRT_MODEL_MS60 },
	{ "RAX70", 		SWRT_MODEL_RAX70 },
	{ "360V6", 		SWRT_MODEL_360V6 },
	{ "GLAX1800", 	SWRT_MODEL_GLAX1800 },
	{ "RMAC2100", 	SWRT_MODEL_RMAC2100 },
	{ "R6800", 		SWRT_MODEL_R6800 },
	{ "PGBM1", 		SWRT_MODEL_PGBM1 },
	{ NULL, 0 },
};

int get_modelname(void)
{
	static int model = SWRT_MODEL_SWRTMIN;
	char *pid;
	const struct model_s *p;

	if (model != SWRT_MODEL_SWRTMIN)
		return model;

	pid = nvram_safe_get("modelname");
	for (p = &modelname_list[0]; p->pid; ++p) {
		if (!strcmp(pid, p->pid)) {
			model = p->model;
			break;
		}
	}
	return model;
}

char *get_modelnameid(int model)
{
	char *pid = "unknown";
	const struct model_s *p;

	for (p = &modelname_list[0]; p->pid; ++p) {
		if (model == p->model) {
			pid = p->pid;
			break;
		}
	}
	return pid;
}
