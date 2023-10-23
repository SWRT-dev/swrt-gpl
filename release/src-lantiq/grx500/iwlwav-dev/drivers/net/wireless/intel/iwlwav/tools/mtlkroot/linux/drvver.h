/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DRV_VER_H__
#define __DRV_VER_H__

#ifdef CONFIG_WAVE_DEBUG
#define DRV_COMPILATION_TYPE ".Debug"
#else
#  ifdef CPTCFG_IWLWAV_SILENT
#    define DRV_COMPILATION_TYPE ".Silent"
#  else
#    define DRV_COMPILATION_TYPE ".Release"
#  endif
#endif

#define DRV_NAME        "mtlklog"
#define DRV_VERSION     MTLK_SOURCE_VERSION DRV_COMPILATION_TYPE
#define DRV_DESCRIPTION "MaxLinear Logger Driver"

#endif /* !__DRV_VER_H__ */
