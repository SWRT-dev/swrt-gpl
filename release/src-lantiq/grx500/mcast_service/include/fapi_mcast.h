/*******************************************************************************
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/


#ifndef MCAST_FAPI_H
#define MCAST_FAPI_H

/*! \file fapi_mcast.h
    \brief This File contains the constants, enumerations, related data
           structures and API for using the functionality provided by
           Multicast FAPI
*/

/* header files*/
#include <linux/if_ether.h>
#include <linux/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <libsafec/safe_str_lib.h>

/** \addtogroup FAPI_MCAST */
/* @{ */

/*! \def IFSIZE          
    \brief  Maximum interface name size
 */

#define IFSIZE  20

/*! \def FAPI_SUCCESS 
	\brief  FAPI_SUCCESS return value 
*/

#define FAPI_SUCCESS  0x0

/*! \def FAPI_SUCCESS 
	\brief  FAPI_SUCCESS return value 
*/

#define FAPI_FAILURE  0x1


/*!
    \brief  enumerations for protocol types : IPV4,IPV6 and INVALID
*/
typedef enum {
/** IPv4 type  */
	IPV4 = 0,
/** IPv6 type  */
	IPV6 = 1,
/** Invalid type  */
	INVALID
} ptype_t;

/*!
  \brief Structure describing IP Address type
*/
typedef struct {
/** IP  type  */
	ptype_t type;
	union {
/** IPv4 Address  */
		struct in_addr ip4;
/** IPv6 Address  */
		struct in6_addr ip6;
	} addr;
}ipaddr_t;


/*!
    \brief Structure describing multicast membership configuration
*/

typedef struct {
        char intfName[IFSIZE]; /*!<  LAN/WLAN Member NetDevice */
        char rxIntfName[IFSIZE]; /*!< WAN Member NetDevice */
        ipaddr_t  groupIP; /*!< Destination IP (GA) - v4 or v6 */
        ipaddr_t  srcIP; /*!< Source IP - v4 or v6 (optional) */
        unsigned int  proto; /*!< Protocol Number */
        unsigned int  sPort; /*!< UDP Source Port */
        unsigned int  dPort; /*!< UDP Destination Port */	
	unsigned char macaddr[ETH_ALEN]; /*!< LAN/WLAN Client MAC Address */
} MCAST_MEMBER_t;

/*! \brief  This FAPI perform ADD i.e  it informs  MC-Helper  module  about  new  Join request  received  by  mcastd .
        \param[in] Structure containing multicast membership configuration
        \return FAPI_SUCCESS / FAPI_FAILURE
*/

extern int  fapi_mch_add_entry(MCAST_MEMBER_t *mch_mem);

/*! \brief  This FAPI perform Update i.e  it informs  MC-Helper  module  about Join request for already playing group is   received  by  mcastd .
        \param[in] Structure containing multicast membership configuration
        \return FAPI_SUCCESS / FAPI_FAILURE
*/

extern int  fapi_mch_update_entry(MCAST_MEMBER_t *mch_mem);


/*! \brief  This FAPI perform Get Lan server entry i.e  it gets the LAN server entry from MC-Helper  module  about stream playing group forwarded by bridge .
        \param[in] Structure containing multicast membership configuration
        \return FAPI_SUCCESS / FAPI_FAILURE
*/

extern int  fapi_mch_get_lanserver(MCAST_MEMBER_t *mch_mem);


/*! \brief  This FAPI perform DELETE i.e  it informs  MC-Helper  module  about Leave request  received  by  mcastd .
        \param[in] Structure containing multicast membership configuration
        \return FAPI_SUCCESS / FAPI_FAILURE
*/
extern int  fapi_mch_del_entry(MCAST_MEMBER_t *mch_mem);

/*! \brief  This FAPI perform initilization i.e  it insmod mcast_helper and create /dev/mcast device .
        \return FAPI_SUCCESS / FAPI_FAILURE
*/
extern int  fapi_mch_init(void);

/*! \brief  This FAPI perform uninitilization i.e  it rmmod mcast_helper .
        \return FAPI_SUCCESS / FAPI_FAILURE
*/
extern int  fapi_mch_uninit(void);
/* @} */

#endif
