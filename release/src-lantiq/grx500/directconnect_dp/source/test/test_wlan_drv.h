#ifndef TEST_WLAN_DRV_H
#define TEST_WLAN_DRV_H		/*! PMAC port informatin data */

#define IFNAMSIZ 16
#define PMAC_MAX_NUM  16

#define DP_LIB_LOCK    spin_lock_bh
#define DP_LIB_UNLOCK  spin_unlock_bh

#define MAX_SUBIF_PER_PORT 16

char *dp_port_flag_str[] = {
    "DP_F_DEREGISTER",  /*DP_F_DEREGISTER: not valid device type */
    "ETH_LAN",      /*DP_F_FAST_ETH_LAN */
    "ETH_WAN",      /*DP_F_FAST_ETH_WAN */
    "FAST_WLAN",        /*DP_F_FAST_WLAN */
    "FAST_DSL",     /* DP_F_FAST_DSL */
    "DIRECTPATH",       /* DP_F_DIRECT */
    "CAPWAP"        /* DP_F_CAPWAP */
};
#endif				/*DATAPATH_H */
