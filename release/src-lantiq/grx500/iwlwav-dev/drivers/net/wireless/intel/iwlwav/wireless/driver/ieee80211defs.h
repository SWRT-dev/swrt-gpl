/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _IEEE80211DEFS_H_
#define _IEEE80211DEFS_H_

/*
    Excerpts from "IEEE P802.11e/D13.0, January 2005" p.p. 22-23

    Type          Subtype     Description
    -------------------------------------------------------------------------+
    00 Management 0000        Association request                            |
    00 Management 0001        Association response                           |
    00 Management 0010        Reassociation request                          |
    00 Management 0011        Reassociation response                         |
    00 Management 0100        Probe request                                  |
    00 Management 0101        Probe response                                 |
    00 Management 0110-0111   Reserved                                       |
    00 Management 1000        Beacon                                         |
    00 Management 1001        Announcement traffic indication message (ATIM) |
    00 Management 1010        Disassociation                                 |
    00 Management 1011        Authentication                                 |
    00 Management 1100        Deauthentication                               |
    00 Management 1101        Action                                         |
    00 Management 1110-1111   Reserved                                       |
    01 Control    0000-0111   Reserved                                       |
    01 Control    1000        Block Acknowledgement Request (BlockAckReq)    |
    01 Control    1001        Block Acknowledgement (BlockAck)               |
    01 Control    1010        Power Save Poll (PS-Poll)                      |
    01 Control    1011        Request To Send (RTS)                          |
    01 Control    1100        Clear To Send (CTS)                            |
    01 Control    1101        Acknowledgement (ACK)                          |
    01 Control    1110        Contention-Free (CF)-End                       |
    01 Control    1111        CF-End + CF-Ack                                |
    10 Data       0000        Data                                           |
    10 Data       0001        Data + CF-Ack                                  |
    10 Data       0010        Data + CF-Poll                                 |
    10 Data       0011        Data + CF-Ack + CF-Poll                        |
    10 Data       0100        Null function (no data)                        |
    10 Data       0101        CF-Ack (no data)                               |
    10 Data       0110        CF-Poll (no data)                              |
    10 Data       0111        CF-Ack + CF-Poll (no data)                     |
    10 Data       1000        QoS Data                                       |
    10 Data       1001        QoS Data + CF-Ack                              |
    10 Data       1010        QoS Data + CF-Poll                             |
    10 Data       1011        QoS Data + CF-Ack + CF-Poll                    |
    10 Data       1100        QoS Null (no data)                             |
    10 Data       1101        Reserved                                       |
    10 Data       1110        QoS CF-Poll (no data)                          |
    10 Data       1111        QoS CF-Ack + CF-Poll (no data)                 |
    11 Reserved   0000-1111   Reserved                                       |
*/

#define IEEE80211_1ADDR_LEN             10
#define IEEE80211_2ADDR_LEN             16
#define IEEE80211_3ADDR_LEN             24
#define IEEE80211_4ADDR_LEN             30

/* Frame control field constants */
#define IEEE80211_FCTL_VERS             0x0003
#define IEEE80211_FCTL_FTYPE            0x000c
#define IEEE80211_FCTL_STYPE            0x00f0
#define IEEE80211_FCTL_TODS             0x0100
#define IEEE80211_FCTL_FROMDS           0x0200
#define IEEE80211_FCTL_MOREFRAGS        0x0400
#define IEEE80211_FCTL_RETRY            0x0800
#define IEEE80211_FCTL_PM               0x1000
#define IEEE80211_FCTL_MOREDATA         0x2000
#define IEEE80211_FCTL_PROTECTED        0x4000
#define IEEE80211_FCTL_ORDER            0x8000

#define IEEE80211_FTYPE_MGMT            0x0000
#define IEEE80211_FTYPE_CTL             0x0004
#define IEEE80211_FTYPE_DATA            0x0008

/* management */
#define IEEE80211_STYPE_ASSOC_REQ       0x0000
#define IEEE80211_STYPE_ASSOC_RESP      0x0010
#define IEEE80211_STYPE_REASSOC_REQ     0x0020
#define IEEE80211_STYPE_REASSOC_RESP    0x0030
#define IEEE80211_STYPE_PROBE_REQ       0x0040
#define IEEE80211_STYPE_PROBE_RESP      0x0050
#define IEEE80211_STYPE_BEACON          0x0080
#define IEEE80211_STYPE_ATIM            0x0090
#define IEEE80211_STYPE_DISASSOC        0x00A0
#define IEEE80211_STYPE_AUTH            0x00B0
#define IEEE80211_STYPE_DEAUTH          0x00C0
#define IEEE80211_STYPE_ACTION          0x00D0

/* control */
#define IEEE80211_STYPE_PSPOLL          0x00A0
#define IEEE80211_STYPE_RTS             0x00B0
#define IEEE80211_STYPE_CTS             0x00C0
#define IEEE80211_STYPE_ACK             0x00D0
#define IEEE80211_STYPE_CFEND           0x00E0
#define IEEE80211_STYPE_CFENDACK        0x00F0

/* data */
#define IEEE80211_STYPE_DATA            0x0000
#define IEEE80211_STYPE_DATA_CFACK      0x0010
#define IEEE80211_STYPE_DATA_CFPOLL     0x0020
#define IEEE80211_STYPE_DATA_CFACKPOLL  0x0030
#define IEEE80211_STYPE_NULLFUNC        0x0040
#define IEEE80211_STYPE_CFACK           0x0050
#define IEEE80211_STYPE_CFPOLL          0x0060
#define IEEE80211_STYPE_CFACKPOLL       0x0070
#define IEEE80211_STYPE_QOS_DATA        0x0080

#define IEEE80211_SCTL_FRAG             0x000F
#define IEEE80211_SCTL_SEQ              0xFFF0

#ifndef WAVE_BSS_TX_DPP_EASY_CONNECT
#define IEEE80211_PUBLIC_ACTION_FRAME_CATEGORY 0x04
#define IEEE80211_PUBLIC_ACTION_FRAME_ACTION   0x09
#endif

#define WLAN_FC_GET_TYPE(fc)            ((fc) & IEEE80211_FCTL_FTYPE)
#define WLAN_FC_GET_STYPE(fc)           ((fc) & IEEE80211_FCTL_STYPE)

#define WLAN_FC_IS_NULL_PKT(fc)         ((fc) & IEEE80211_STYPE_NULLFUNC)
#define WLAN_FC_IS_QOS_PKT(fc)          ((fc) & IEEE80211_STYPE_QOS_DATA)
#define WLAN_FC_GET_FROMDS(fc)          (((fc) & IEEE80211_FCTL_FROMDS) != 0)
#define WLAN_FC_GET_TODS(fc)            (((fc) & IEEE80211_FCTL_TODS) != 0)
#define WLAN_FC_GET_VERSION(fc)         ((fc) & IEEE80211_FCTL_VERS)
#define WLAN_FC_GET_PROTECTED(fc)       ((fc) & IEEE80211_FCTL_PROTECTED)
#define WLAN_FC_SET_PROTECTED(fc)       ((fc) | IEEE80211_FCTL_PROTECTED)
#define WLAN_FC_CLEAR_PROTECTED(fc)     ((fc) & ~IEEE80211_FCTL_PROTECTED)

#define WLAN_QOS_GET_PRIORITY(qc)       ((qc) & 0x0007)
#define WLAN_QOS_GET_MSDU(qc)           ((qc) & 0x0080)

#ifndef IEEE80211_STYPE_BAR
#define IEEE80211_STYPE_BAR             0x0080
#endif

#define IEEE80211_ADDR1_OFFSET          4
#define IEEE80211_ADDR2_OFFSET          (IEEE80211_ADDR1_OFFSET +6)
#define IEEE80211_ADDR3_OFFSET          (IEEE80211_ADDR2_OFFSET +6)
#define IEEE80211_ADDR4_OFFSET          (IEEE80211_ADDR3_OFFSET +6 +2) // 2 for Seq#
#define IEEE80211_SEQ_CTL_OFFSET        (IEEE80211_ADDR3_OFFSET +6)

#define WLAN_GET_ADDR1(data)            (data + IEEE80211_ADDR1_OFFSET)
#define WLAN_GET_ADDR2(data)            (data + IEEE80211_ADDR2_OFFSET)
#define WLAN_GET_ADDR3(data)            (data + IEEE80211_ADDR3_OFFSET)
#define WLAN_GET_ADDR4(data)            (data + IEEE80211_ADDR4_OFFSET)
#define WLAN_GET_SEQ_CTL(data)          (data + IEEE80211_SEQ_CTL_OFFSET)

#define IEEE80211_WEP_IV_LEN            4
#define IEEE80211_WEP_IV_WO_IDX_LEN     3
#define IEEE80211_WEP_ICV_LEN           4
#define IEEE80211_WEP_IV_IDX_BIT        6
#define IEEE80211_WEP_IV_IDX_POS        3


#endif // _IEEE80211DEFS_H_
