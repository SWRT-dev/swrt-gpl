/* BT ACS API header */

#ifndef __BT_ACS_H__
#define __BT_ACS_H__

#define NETLINK_BT_ACS                    21
#define NETLINK_BT_ACS_GROUP        (1L << 0)
#define NETLINK_BT_ACS_GROUP_LAST          1

#define BT_ACS_EVNT_ID_SCAN_REPORT 1
#define BT_ACS_EVNT_ID_INTERF_DET  2

#define BT_ACS_MAX_BSS_PER_CHANNEL 10

#define BT_ACS_BONDING_UPPER        1
#define BT_ACS_BONDING_LOWER       -1
#define BT_ACS_BONDING_NONE         0

struct bt_acs_bss_info_t
{ 
  uint8     bssid[6];
  char      ssid[33];       /* Null terminated string */
  uint8     padding0[1];  
  int8      rssi;
  uint8     padding2[3];
  int8      bonding;        /* Member of BT_ACS_BONDING_xxx */
  uint8     is_intolerant;
  uint8     padding3[2];
} __attribute__ ((aligned(1), packed));

struct bt_acs_channel_info_t
{
  uint32  num;
  int8    floor_noise;
  uint8   padding0[3];
  uint32  bss_num;
} __attribute__ ((aligned(1), packed));

struct bt_acs_scan_report_t
{
  uint32  event_id;           /* Member of BT_ACS_EVNT_ID_xxx */
  uint8   selected_channel;
  int8    selected_bonding;   /* Member of BT_ACS_BONDING_xxx */
  int8    selected_ch_floor_noise;
  uint8   padding0[1];
  uint8   channel_num;
  uint8   padding1[3];
} __attribute__ ((aligned(1), packed));

struct bt_acs_interf_event_t
{
  uint32  event_id;           /* Member of BT_ACS_EVNT_ID_xxx */
  uint8   channel;
  uint8   padding0[1];
  int8    floor_noise;
  uint8   padding1[1];
} __attribute__ ((aligned(1), packed));


#endif
