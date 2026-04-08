/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_IRB_OSDEP_LINUX_H__
#define __MTLK_IRB_OSDEP_LINUX_H__

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

#define MTLK_IRB_ROOT_DESC       "root"

#define MTLK_IRB_INI_NAME        "topology"

#define MTLK_IRB_INI_PARENT      "parent"
#define MTLK_IRB_INI_CDEV_MAJOR  "cdev_major"
#define MTLK_IRB_INI_CDEV_MINOR  "cdev_minor"
#define MTLK_IRB_INI_NOF_SONS    "nof_sons"
#define MTLK_IRB_INI_SON_PREFIX  "son"

#ifdef __KERNEL__
void __MTLK_IFUNC mtlk_irbd_print_topology(struct seq_file *s);
#endif

// {203101AB-E511-4bdf-AD92-2B404B955AF7}
#define MTLK_IRB_GUID_NODE_DISAPPEARS                                   \
  MTLK_DECLARE_GUID(0x203101ab, 0xe511, 0x4bdf, 0xad, 0x92, 0x2b, 0x40, 0x4b, 0x95, 0x5a, 0xf7)

#endif /*  __MTLK_IRB_OSDEP_LINUX_H__ */

