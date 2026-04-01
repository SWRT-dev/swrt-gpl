/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __PCAPDEFS_H__
#define __PCAPDEFS_H__

#if defined (_MSC_VER) /* MS CL */
#pragma pack(push, 1)
#define __MTLK_PACK1
#elif defined (__GNUC__) /* GCC */
#define __MTLK_PACK1 __attribute__((aligned(1), packed))
#else
#error Please define the __MTLK_PACK1 for your compiler!
#endif

typedef int32  gint32;
typedef uint32 guint32;
typedef uint16 guint16;

#define PCAP_MAGIC           0xa1b2c3d4
#define PCAP_VERSION_MAJOR   2
#define PCAP_VERSION_MINOR   4

typedef struct pcap_hdr_s {
        guint32 magic_number;   /* magic number */
        guint16 version_major;  /* major version number */
        guint16 version_minor;  /* minor version number */
        gint32  thiszone;       /* GMT to local correction */
        guint32 sigfigs;        /* accuracy of timestamps */
        guint32 snaplen;        /* max length of captured packets, in octets */
        guint32 network;        /* data link type */
} __MTLK_PACK1 pcap_hdr_t;

typedef struct pcaprec_hdr_s {
        guint32 ts_sec;         /* timestamp seconds */
        guint32 ts_usec;        /* timestamp microseconds */
        guint32 incl_len;       /* number of octets of packet saved in file */
        guint32 orig_len;       /* actual length of packet */
} __MTLK_PACK1 pcaprec_hdr_t;

#if defined (_MSC_VER) /* MS CL */
#pragma pack(pop)
#elif defined (__GNUC__) /* GCC */
#endif
#undef __MTLK_PACK

#endif /* __PCAPDEFS_H__ */
