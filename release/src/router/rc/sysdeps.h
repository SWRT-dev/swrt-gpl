#ifndef _RC_SYSDEPS_H_
#define _RC_SYSDEPS_H_
#if !defined(__GLIBC__) && !defined(__UCLIBC__) && !defined(RTCONFIG_BCMARM) /* musl */
#include <net/ethernet.h>
#endif
#include <rtconfig.h>

/* sysdeps/init-PLATFORM.c */
extern void init_devs(void);
extern void generate_switch_para(void);
extern void init_switch();
extern void config_switch();
extern int switch_exist(void);
extern void init_wl(void);
#if defined(RTCONFIG_QCA)
extern void deinit_all_vaps(const int remove_vap);
extern int rebuild_main_vap(void);
extern void load_wifi_driver(void);
extern void load_testmode_wifi_driver(void);
#endif
#if defined(RTCONFIG_ALPINE)
extern void load_wifi_driver(void);
extern void load_testmode_wifi_driver(void);
extern char *__get_wlifname(int band, int subunit, char *buf);
extern char *get_staifname(int band);
extern char *get_vphyifname(int band);
#endif
#if defined(RTCONFIG_LANTIQ)
extern void load_wifi_driver(void);
extern void load_testmode_wifi_driver(void);
#endif
extern void fini_wl(void);
extern void init_syspara(void);
extern void post_syspara(void);
#ifndef CONFIG_BCMWL5
extern void generate_wl_para(int unit, int subunit);
#else
extern void generate_wl_para(char *ifname, int unit, int subunit);
#if !defined(RTCONFIG_HND_ROUTER)
extern char *brcm_to_swconfig(char *vlan, char *buf);
#endif
#endif
#if defined(RTCONFIG_RALINK)
extern void reinit_hwnat(int unit);
#elif defined(RTCONFIG_QCA)

#if defined(RTCONFIG_SOC_QCA9557) || defined(RTCONFIG_QCA953X) || defined(RTCONFIG_QCA956X) \
 || defined(RTCONFIG_QCN550X) || defined(RTCONFIG_SOC_IPQ40XX)
#define reinit_hwnat(unit) reinit_sfe(unit)
extern void reinit_sfe(int unit);
static inline void tweak_wifi_ps(const char *wif) { }
#elif defined(RTCONFIG_SOC_IPQ8064) || defined(RTCONFIG_SOC_IPQ8074) || defined(RTCONFIG_SOC_IPQ60XX) \
   || defined(RTCONFIG_SOC_IPQ50XX) || defined(RTCONFIG_SOC_IPQ53XX)
#define reinit_hwnat(unit) reinit_ecm(unit)
extern int ecm_selection(void);
extern void init_ecm(void);
extern void reinit_ecm(int unit);
extern void post_ecm(void);
extern void tweak_wifi_ps(const char *wif);
#else
#error select NAT acceleration type!
#endif
#endif
extern char *get_wlifname(int unit, int subunit, int subunit_x, char *buf);
extern int wl_exist(char *ifname, int band);
extern void set_wan_tag(char *interface);

extern int wlcconnect_core(void);
extern int wlcscan_core(char *ofile, char *wif);
#if defined(RTAC68U) || defined(R7000P)
extern int wlcscan_core_wl(char *ofile, char *wif);
#endif
extern void wps_oob_both(void);
extern void start_wsc(void);
extern const char *get_wifname(int band);
extern int get_channel_list_via_driver(int unit, char *buffer, int len);
extern int get_channel_list_via_country(int unit, const char *country_code, char *buffer, int len);
extern char *wlc_nvname(char *keyword);

#if defined(RTCONFIG_RALINK)
extern int getWscStatus(int unit);
#elif defined(RTCONFIG_QCA) || defined(RTCONFIG_ALPINE) || defined(RTCONFIG_LANTIQ)
extern char *getWscStatus(char *ifname, char *buf, int buflen);
#endif

#if defined(RTCONFIG_DSL)
extern void init_switch_dsl(void);
extern void config_switch_dsl(void);
extern void config_switch_dsl_set_lan(void);
#endif

// init-ralink.c
#if defined(RTCONFIG_RALINK)
extern int is_if_up(char *ifname);
extern void config_hwctl_led(void);
extern void gen_ra_sku(const char *reg_spec);
extern void generate_wl_para(int unit, int subunit);
extern void reset_ra_sku(const char *location, const char *country, const char *reg_spec);
extern void setup_smp(int interface);
#endif

#if defined(RTCONFIG_REALTEK)
#if defined(RTCONFIG_BT_CONN) || defined(RPAC55)
extern void gen_rtlbt_fw_config(void);
#endif
extern int rtk_check_nvram_partation(void);
#endif

#ifdef RTCONFIG_BCMFA
#include <etioctl.h>
#define	ARPHRD_ETHER		1	/* ARP Header */
#define	CTF_FA_DISABLED		0
#define	CTF_FA_BYPASS		1
#define	CTF_FA_NORMAL		2
#define	CTF_FA_SW_ACC		3
#define	FA_ON(mode)		(mode == CTF_FA_BYPASS || mode == CTF_FA_NORMAL)
int fa_mode;
void fa_mode_init();
#endif

/* conn_diag-PLATFORM.c */
extern int check_wifi_channel_by_idx(char *output, int len, int i, char *node, char *lan_ipaddr, char *lan_hwaddr);
extern int get_wifi_ssid(int idx, char *output, int outputlen);
extern int get_wifi_subif_info(int idx, char *output, int outputlen);
extern int get_wifi_status(char *ifname, char *output, int len);
extern int get_wifi_dfs_status(char *output, int len, char *node, char *lan_ipaddr, char *lan_hwaddr);
extern void init_check_wifi_channel(void);

/* roamast-PLATFORM.c */
extern void rast_send_beacon_request(int bssidx, int vifidx, struct ether_addr *sta);

/* wps-PLATFORM.c */
extern int start_wps_method_ob(void);
extern int stop_wps_method_ob(void);

/* amas-ssd-cd-PLATFORM.c */
extern void stop_site_survey_cd();

/* AiMesh sysdeps */
#if defined(RTCONFIG_AMAS_WGN)
extern void wgn_sysdep_swtich_unset(int vid);
extern void wgn_sysdep_swtich_set(int vid);
extern void wl_vlan_set(char* ifname, int allow);
extern void wgn_sysdep_wl_unset(int vid);
extern void wgn_sysdep_wl_set(int vid);
extern int is_wlif( char *ifname);
#endif
/* sysdeps/easymesh-PLATFORM.c */
#if defined(RTCONFIG_EASYMESH)
extern int config_agent_main(int argc, char *argv[]);
extern int connect_agent(char *ip, int port, char *cmd);
extern int easymesh_agentctrl_main(int argc, char *argv[]);
#endif
/* sysdeps/swrtmesh-PLATFORM.c */
#if defined(RTCONFIG_SWRTMESH)
extern int swrtmesh_sysdep_bh_start(void);
extern int swrtmesh_sysdep_bh_stop(void);
extern int swrtmesh_set_bh_bss_enabled(int unit, int subunit);
#endif
#endif

