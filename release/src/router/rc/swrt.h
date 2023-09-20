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
 * Copyright 2018-2023, SWRT.
 * Copyright 2018-2023, paldier <paldier@hotmail.com>.
 * Copyright 2018-2023, lostlonger<lostlonger.g@gmail.com>.
 * All Rights Reserved.
 * 
 */

#ifndef __SWRT_H__
#define __SWRT_H__

#if defined(RTCONFIG_SOFTCENTER)
enum {
	SOFTCENTER_WAN=1,
	SOFTCENTER_NAT,
	SOFTCENTER_MOUNT,
	SOFTCENTER_SERVICES_START,
	SOFTCENTER_SERVICES_STOP,
	SOFTCENTER_CIFS_MOUNT,
	SOFTCENTER_UNMOUNT
};

extern void softcenter_trigger(int sig);
#endif
#if defined(RTCONFIG_BCMARM)
enum {
	SUFFIX_MACADDR=0,
	SUFFIX_TXPWR,
	SUFFIX_DISBAND5GRP,
	SUFFIX_CCODE,
	SUFFIX_REGREV
};
#endif
extern void swrt_init_pre(void);
extern void swrt_init_model(void) __attribute__((weak));
extern void swrt_init_post(void);
extern void gen_swrtid(void);
extern int swrt_toolbox(int argc, char **argv);
enum {
	CURL_FIRMWARE=0,
	CURL_OTHER
};
extern int curl_download_swrt(const char *url, const char *file_path, long timeout, const int feature);
#if defined(RTAC68U)
extern int swrt_set(const char *name, const char *value);
extern int swrt_unset(const char *name);
#else
extern int swrt_nvram_set(const char *name, const char *value, const int save);
extern int swrt_nvram_unset(const char *name, const int save);
#endif
#if defined(K3)
extern int start_k3screen(void);
extern void k3_nvram_patch(void);
#elif defined(XWR3100)
extern void xwr3100_nvram_patch(void);
#elif defined(R8500)
extern void r8500_nvram_patch(void);
#elif defined(RTAC68U) && !defined(SBRAC1900P) && !defined(EA6700) && !defined(DIR868L)
extern void update_cfe_tm1900(void);
#elif defined(EA6700)
extern void update_cfe_ea6700(void);
#elif defined(DIR868L)
extern void update_cfe_dir868l(void);
#elif defined(R6300V2)
extern void update_cfe_r6300v2(void);
#elif defined(SBRAC1900P)
extern void ac1900p_patch_cfe(void);
#elif defined(SBRAC3200P)
extern void ac3200p_patch_cfe(void);
#elif defined(R7000P)
extern void r7000p_nvram_patch(void);
#elif defined(RTCONFIG_RALINK) || defined(RTCONFIG_QCA)
extern void patch_Factory(void) __attribute__((weak));
#else
extern void swrt_patch_nvram(void);
#endif

extern void swrt_sys_hack(void) __attribute__((weak));
#if defined(TUFAX3000) || defined(RTAX58U) || defined(RTAX82U)
extern void enable_4t4r_ax58(void);
extern void enable_4t4r(void);
#elif defined(K3C)
extern void k3c_lan_led(void);
extern void k3c_init_led(void);
extern void init_mtd8(void);
#endif

#ifdef RTCONFIG_UUPLUGIN
extern void start_uu(void);
extern void stop_uu(void);
#if defined(RTCONFIG_SWRT_UU)
extern void exec_uu_swrt(void);
#endif
#endif
#if defined(RTCONFIG_FRS_LIVE_UPDATE) 
extern int swrt_firmware_check_update_main(int argc, char *argv[]);
#endif
extern void check_auth_code(void) __attribute__((weak));
#if defined(RTCONFIG_SWRT_LED)
extern void swrt_ledon(void);
#endif
#if defined(RTCONFIG_ENTWARE)
extern void start_entware(void);
extern void stop_entware(void);
extern void init_entware(void);
extern void gen_arch_conf(void);
#endif
#if defined(R6800)
extern void show_boraddata(void);
extern void fix_boraddata(char *key, char *value);
#endif
#if defined(RAX200)
extern void fan_watchdog(void);
#endif
#if defined(RTCONFIG_BCMARM)
extern void get_nvramstr(int unit, char *buf, size_t len, int which);
extern void auth_unlock_power(int *max2g, int *max5g, int *max5g2, int *max6g) __attribute__((weak));
#endif
#endif

