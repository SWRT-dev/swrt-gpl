/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*! \file              mtlkirb_osdep.h
 * \brief              OS dependent IRB functionality
 */
#ifndef __MTLK_IRB_OSDEP_LINUX_H__
#define __MTLK_IRB_OSDEP_LINUX_H__

#include "mtlkwlanirbdefs.h"

#define MTLK_CDEV_IRB_IOCTL SIOCDEVPRIVATE

struct mtlk_irb_call_hdr
{
  mtlk_guid_t evt;
  uint32      data_size;
} __attribute__((aligned(1), packed));

struct mtlk_irb_ntfy_hdr
{
  mtlk_guid_t evt;
  uint32      seq_no;
  uint32      cdev_minor; /* used as IRB node ID */
  uint32      data_size;
} __attribute__((aligned(1), packed));

/* Maximal length of names*/
#define MTLK_IRB_STR_MAX_LEN    (127)

#define MTLK_IRB_ROOT_DESC       "root"

#define MTLK_IRB_INI_NAME        "topology"

#define MTLK_IRB_INI_PARENT      "parent"
#define MTLK_IRB_INI_CDEV_MAJOR  "cdev_major"
#define MTLK_IRB_INI_CDEV_MINOR  "cdev_minor"
#define MTLK_IRB_INI_NOF_SONS    "nof_sons"
#define MTLK_IRB_INI_SON_PREFIX  "son"

#ifdef __KERNEL__
/******************************************************************************/
/*!  \brief             Print IRB topology to proc file system
*    \param[in]         s seq_file object.
*    \return            void
*/
void __MTLK_IFUNC mtlk_irbd_print_topology(struct seq_file *s);
#endif

// {203101AB-E511-4bdf-AD92-2B404B955AF7}
#define MTLK_IRB_GUID_NODE_DISAPPEARS                                   \
  MTLK_DECLARE_GUID(0x203101ab, 0xe511, 0x4bdf, 0xad, 0x92, 0x2b, 0x40, 0x4b, 0x95, 0x5a, 0xf7)

#define MTLK_IRB_VAP_NAME_MAX_LEN (sizeof(MTLK_IRB_VAP_NAME) + 3) /* 3 chars for card index */
#define MTLK_IRB_ROOT_DESC_LEN    sizeof(MTLK_IRB_ROOT_DESC)

#define MTLK_IRB_MAX_UNIQ_DESC_LEN (MAX(IFNAMSIZ, MAX(MTLK_IRB_VAP_NAME_MAX_LEN, MTLK_IRB_ROOT_DESC_LEN)))

#endif /*  __MTLK_IRB_OSDEP_LINUX_H__ */

