/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __DRV_VER_H__
#define __DRV_VER_H__

#ifdef CPTCFG_IWLWAV_DEBUG
#define DRV_COMPILATION_TYPE ".Debug"
#else
#  ifdef CPTCFG_IWLWAV_SILENT
#    define DRV_COMPILATION_TYPE ".Silent"
#  else
#    define DRV_COMPILATION_TYPE ".Release"
#  endif
#endif

# define MTLK_PCIEG6 ".PcieG6"

#define MTLK_PLATFORMS MTLK_PCIEG6

#define DRV_NAME        "mtlk"
#define DRV_VERSION     MTLK_SOURCE_VERSION \
                        MTLK_PLATFORMS DRV_COMPILATION_TYPE
#define DRV_DESCRIPTION "Metalink 802.11n WiFi Network Driver"

#endif /* !__DRV_VER_H__ */

