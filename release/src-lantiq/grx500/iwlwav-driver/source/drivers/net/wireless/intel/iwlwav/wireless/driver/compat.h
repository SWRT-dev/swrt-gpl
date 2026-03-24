/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 * 
 *
 * This file contains compartibility stuff
 * between different Linux kernels.
 *
 *  Written by Andriy Tkachuk
 *
 */

#ifndef _COMPAT_H_
#define _COMPAT_H_

#include <linux/pci.h>

#include <linux/etherdevice.h>
#include <linux/wireless.h>
#include <linux/icmpv6.h>

#define LOG_LOCAL_GID   GID_COMPAT
#define LOG_LOCAL_FID   0

#include <linux/kthread.h>
#include <linux/ieee80211.h>

// ------------------------------------------------------------------------------------------
// wait stuff

#ifndef wait_event_interruptible_timeout
#define __wait_event_interruptible_timeout(wq, condition, ret)     \
  do {                                                             \
    DEFINE_WAIT(__wait);                                           \
    for (;;) {                                                     \
      prepare_to_wait(&wq, &__wait, TASK_INTERRUPTIBLE);           \
      if (condition)                                               \
        break;                                                     \
      if (!signal_pending(current)) {                              \
        ret = schedule_timeout(ret);                               \
        if (!ret)                                                  \
          break;                                                   \
        continue;                                                  \
      }                                                            \
      ret = -ERESTARTSYS;                                          \
      break;                                                       \
    }                                                              \
    finish_wait(&wq, &__wait);                                     \
  } while (0)

#define wait_event_interruptible_timeout(wq, condition, timeout)   \
  ({                                                               \
    long __ret = timeout;                                          \
    if (!(condition))                                              \
      __wait_event_interruptible_timeout(wq, condition, __ret);    \
    __ret;                                                         \
})
#endif

#ifndef wait_event_timeout
#define __wait_event_timeout(wq, condition, ret)                   \
  do {                                                             \
    DEFINE_WAIT(__wait);                                           \
                                                                   \
    for (;;) {                                                     \
      prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);         \
      if (condition)                                               \
        break;                                                     \
      ret = schedule_timeout(ret);                                 \
      if (!ret)                                                    \
        break;                                                     \
    }                                                              \
    finish_wait(&wq, &__wait);                                     \
} while (0)

#define wait_event_timeout(wq, condition, timeout)                 \
  ({                                                               \
    long __ret = timeout;                                          \
    if (!(condition))                                              \
      __wait_event_timeout(wq, condition, __ret);                  \
     __ret;                                                        \
  })
#endif /* #ifndef wait_event_timeout */

// wait stuff
// -----------------------------------------------------------------------------------------

#include <linux/delay.h>

// ------------------------------------------------------------------------------------------
// Wireless extensions

#if WIRELESS_EXT < 18
/* WPA : Generic IEEE 802.11 informatiom element (e.g., for WPA/RSN/WMM).
 * This ioctl uses struct iw_point and data buffer that includes IE id and len
 * fields. More than one IE may be included in the request. Setting the generic
 * IE to empty buffer (len=0) removes the generic IE from the driver. Drivers
 * are allowed to generate their own WPA/RSN IEs, but in these cases, drivers
 * are required to report the used IE as a wireless event, e.g., when
 * associating with an AP. */
#define SIOCSIWGENIE    0x8B30          /* set generic IE */
#define SIOCGIWGENIE    0x8B31          /* get generic IE */

/* WPA : IEEE 802.11 MLME requests */
#define SIOCSIWMLME     0x8B16          /* request MLME operation; uses
                                         * struct iw_mlme */
/* WPA : Authentication mode parameters */
#define SIOCSIWAUTH     0x8B32          /* set authentication mode params */
#define SIOCGIWAUTH     0x8B33          /* get authentication mode params */

/* WPA : Extended version of encoding configuration */
#define SIOCSIWENCODEEXT 0x8B34         /* set encoding token & mode */
#define SIOCGIWENCODEEXT 0x8B35         /* get encoding token & mode */

/* WPA2 : PMKSA cache management */
#define SIOCSIWPMKSA    0x8B36          /* PMKSA cache operation */

#define IWEVGENIE       0x8C05          /* Generic IE (WPA, RSN, WMM, ..)
                                         * (scan results); This includes id and
                                         * length fields. One IWEVGENIE may
                                         * contain more than one IE. Scan
                                         * results may contain one or more
                                         * IWEVGENIE events. */
#define IWEVMICHAELMICFAILURE 0x8C06    /* Michael MIC failure
                                         * (struct iw_michaelmicfailure)
                                         */
#define IWEVASSOCREQIE  0x8C07          /* IEs used in (Re)Association Request.
                                         * The data includes id and length
                                         * fields and may contain more than one
                                         * IE. This event is required in
                                         * Managed mode if the driver
                                         * generates its own WPA/RSN IE. This
                                         * should be sent just before
                                         * IWEVREGISTERED event for the
                                         * association. */
#define IWEVASSOCRESPIE 0x8C08          /* IEs used in (Re)Association
                                         * Response. The data includes id and
                                         * length fields and may contain more
                                         * than one IE. This may be sent
                                         * between IWEVASSOCREQIE and
                                         * IWEVREGISTERED events for the
                                         * association. */
#define IWEVPMKIDCAND   0x8C09          /* PMKID candidate for RSN
                                         * pre-authentication
                                         * (struct iw_pmkid_cand) */

/* MLME requests (SIOCSIWMLME / struct iw_mlme) */
#define IW_MLME_DEAUTH          0
#define IW_MLME_DISASSOC        1

/* SIOCSIWAUTH/SIOCGIWAUTH struct iw_param flags */
#define IW_AUTH_INDEX           0x0FFF
#define IW_AUTH_FLAGS           0xF000
/* SIOCSIWAUTH/SIOCGIWAUTH parameters (0 .. 4095)
 * (IW_AUTH_INDEX mask in struct iw_param flags; this is the index of the
 * parameter that is being set/get to; value will be read/written to
 * struct iw_param value field) */
#define IW_AUTH_WPA_VERSION             0
#define IW_AUTH_CIPHER_PAIRWISE         1
#define IW_AUTH_CIPHER_GROUP            2
#define IW_AUTH_KEY_MGMT                3
#define IW_AUTH_TKIP_COUNTERMEASURES    4
#define IW_AUTH_DROP_UNENCRYPTED        5
#define IW_AUTH_80211_AUTH_ALG          6
#define IW_AUTH_WPA_ENABLED             7
#define IW_AUTH_RX_UNENCRYPTED_EAPOL    8
#define IW_AUTH_ROAMING_CONTROL         9
#define IW_AUTH_PRIVACY_INVOKED         10

/* IW_AUTH_WPA_VERSION values (bit field) */
#define IW_AUTH_WPA_VERSION_DISABLED    0x00000001
#define IW_AUTH_WPA_VERSION_WPA         0x00000002
#define IW_AUTH_WPA_VERSION_WPA2        0x00000004

/* IW_AUTH_PAIRWISE_CIPHER and IW_AUTH_GROUP_CIPHER values (bit field) */
#define IW_AUTH_CIPHER_NONE     0x00000001
#define IW_AUTH_CIPHER_WEP40    0x00000002
#define IW_AUTH_CIPHER_TKIP     0x00000004
#define IW_AUTH_CIPHER_CCMP     0x00000008
#define IW_AUTH_CIPHER_WEP104   0x00000010

/* IW_AUTH_KEY_MGMT values (bit field) */
#define IW_AUTH_KEY_MGMT_802_1X 1
#define IW_AUTH_KEY_MGMT_PSK    2

/* IW_AUTH_80211_AUTH_ALG values (bit field) */
#define IW_AUTH_ALG_OPEN_SYSTEM 0x00000001
#define IW_AUTH_ALG_SHARED_KEY  0x00000002
#define IW_AUTH_ALG_LEAP        0x00000004

/* IW_AUTH_ROAMING_CONTROL values */
#define IW_AUTH_ROAMING_ENABLE  0       /* driver/firmware based roaming */
#define IW_AUTH_ROAMING_DISABLE 1       /* user space program used for roaming
                                         * control */
/* SIOCSIWENCODEEXT definitions */
#define IW_ENCODE_SEQ_MAX_SIZE  8
/* struct iw_encode_ext ->alg */
#define IW_ENCODE_ALG_NONE      0
#define IW_ENCODE_ALG_WEP       1
#define IW_ENCODE_ALG_TKIP      2
#define IW_ENCODE_ALG_CCMP      3
#define IW_ENCODE_ALG_AES_CMAC  5
#define IW_ENCODE_ALG_GCMP      6
#define IW_ENCODE_ALG_GCMP_256  7
#define IW_ENCODE_ALG_BIP_GMAC_128  8
#define IW_ENCODE_ALG_BIP_GMAC_256  9

/* struct iw_encode_ext ->ext_flags */
#define IW_ENCODE_EXT_TX_SEQ_VALID      0x00000001
#define IW_ENCODE_EXT_RX_SEQ_VALID      0x00000002
#define IW_ENCODE_EXT_GROUP_KEY         0x00000004
#define IW_ENCODE_EXT_SET_TX_KEY        0x00000008

/* IWEVMICHAELMICFAILURE : struct iw_michaelmicfailure ->flags */
#define IW_MICFAILURE_KEY_ID    0x00000003 /* Key ID 0..3 */
#define IW_MICFAILURE_GROUP     0x00000004
#define IW_MICFAILURE_PAIRWISE  0x00000008
#define IW_MICFAILURE_STAKEY    0x00000010
#define IW_MICFAILURE_COUNT     0x00000060 /* 1 or 2 (0 = count not supported)
                                            */

/* Bit field values for enc_capa in struct iw_range */
#define IW_ENC_CAPA_WPA             0x00000001
#define IW_ENC_CAPA_WPA2            0x00000002
#define IW_ENC_CAPA_CIPHER_TKIP     0x00000004
#define IW_ENC_CAPA_CIPHER_CCMP     0x00000008
#define IW_ENC_CAPA_CIPHER_GCMP     0x00000010
#define IW_ENC_CAPA_CIPHER_GCMP_256 0x00000020

/* ------------------------- WPA SUPPORT ------------------------- */

/*
 *      Extended data structure for get/set encoding (this is used with
 *      SIOCSIWENCODEEXT/SIOCGIWENCODEEXT. struct iw_point and IW_ENCODE_*
 *      flags are used in the same way as with SIOCSIWENCODE/SIOCGIWENCODE and
 *      only the data contents changes (key data -> this structure, including
 *      key data).
 *
 *      If the new key is the first group key, it will be set as the default
 *      TX key. Otherwise, default TX key index is only changed if
 *      IW_ENCODE_EXT_SET_TX_KEY flag is set.
 *
 *      Key will be changed with SIOCSIWENCODEEXT in all cases except for
 *      special "change TX key index" operation which is indicated by setting
 *      key_len = 0 and ext_flags |= IW_ENCODE_EXT_SET_TX_KEY.
 *
 *      tx_seq/rx_seq are only used when respective
 *      IW_ENCODE_EXT_{TX,RX}_SEQ_VALID flag is set in ext_flags. Normal
 *      TKIP/CCMP operation is to set RX seq with SIOCSIWENCODEEXT and start
 *      TX seq from zero whenever key is changed. SIOCGIWENCODEEXT is normally
 *      used only by an Authenticator (AP or an IBSS station) to get the
 *      current TX sequence number. Using TX_SEQ_VALID for SIOCSIWENCODEEXT and
 *      RX_SEQ_VALID for SIOCGIWENCODEEXT are optional, but can be useful for
 *      debugging/testing.
 */
struct  iw_encode_ext
{
        __u32           ext_flags; /* IW_ENCODE_EXT_* */
        __u8            tx_seq[IW_ENCODE_SEQ_MAX_SIZE]; /* LSB first */
        __u8            rx_seq[IW_ENCODE_SEQ_MAX_SIZE]; /* LSB first */
        struct sockaddr addr; /* ff:ff:ff:ff:ff:ff for broadcast/multicast
                               * (group) keys or unicast address for
                               * individual keys */
        __u16           alg; /* IW_ENCODE_ALG_* */
        __u16           key_len;
        __u8            key[0];
};

/* SIOCSIWMLME data */
struct  iw_mlme
{
        __u16           cmd; /* IW_MLME_* */
        __u16           reason_code;
        struct sockaddr addr;
};

/* SIOCSIWPMKSA data */
#define IW_PMKSA_ADD            1
#define IW_PMKSA_REMOVE         2
#define IW_PMKSA_FLUSH          3

#define IW_PMKID_LEN    16

struct  iw_pmksa
{
        __u32           cmd; /* IW_PMKSA_* */
        struct sockaddr bssid;
        __u8            pmkid[IW_PMKID_LEN];
};

/* IWEVMICHAELMICFAILURE data */
struct  iw_michaelmicfailure
{
        __u32           flags;
        struct sockaddr src_addr;
        __u8            tsc[IW_ENCODE_SEQ_MAX_SIZE]; /* LSB first */
};
#endif    //  WIRELESS_EXT < 18

#if WIRELESS_EXT < 17
#define IW_QUAL_QUAL_UPDATED    0x01
#define IW_QUAL_LEVEL_UPDATED   0x02
#define IW_QUAL_NOISE_UPDATED   0x04
#define IW_QUAL_QUAL_INVALID    0x10
#define IW_QUAL_LEVEL_INVALID   0x20
#define IW_QUAL_NOISE_INVALID   0x40
#endif /* WIRELESS_EXT < 17 */

#if WIRELESS_EXT < 19
#define IW_QUAL_ALL_UPDATED     0x07
#define IW_QUAL_ALL_INVALID     0x70
#define IW_QUAL_DBM             0x08
#endif /* WIRELESS_EXT < 19 */


#define mtlk_iwe_stream_add_point  iwe_stream_add_point
#define mtlk_iwe_stream_add_event  iwe_stream_add_event
#define mtlk_iwe_stream_add_value  iwe_stream_add_value

// Wireless extensions 
// ------------------------------------------------------------------------------------------

#define WEP_KEYS 4

// ------------------------------------------------------------------------------------------
// Ethernet

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)
static inline unsigned compare_ether_addr(const u8 *addr1, const u8 *addr2)
{
  return !ether_addr_equal(addr1, addr2);
}
#endif

// Ethernet
// ------------------------------------------------------------------------------------------

#define mtlk_try_module_get try_module_get
#define mtlk_module_put     module_put

// ------------------------------------------------------------------------------------------
// hash32

/* hash_32 is needed for l2nat. find details on it in include/linux/hash.h */
#include <linux/hash.h>

// hash32
// ------------------------------------------------------------------------------------------

# define FUNCTION func

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* _COMPAT_H_ */
