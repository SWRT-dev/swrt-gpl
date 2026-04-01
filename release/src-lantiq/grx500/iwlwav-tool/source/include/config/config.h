/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* Define to 1 if gcc supports builin atomics */
#define HAVE_BUILTIN_ATOMIC 1

/* Define as 1 to tell that we are building on Linux */
#define LINUX 1

/* Following defines have been moved here from various makefiles */
#define CPTCFG_IWLWAV_RT_LOGGER_FUNCTIONS

#ifdef CONFIG_WAVE_DEBUG
#define CPTCFG_IWLWAV_MAX_DLEVEL 3
#else
#define CPTCFG_IWLWAV_MAX_DLEVEL 0
#endif

#define CPTCFG_IWLWAV_USE_LIBNL_NG

/* Variables referenced in this repo but not set by default */
#undef CPTCFG_IPV4_ONLY_CLI
#undef CPTCFG_IWLWAV_DEBUG
#undef CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL
#undef CPTCFG_IWLWAV_ENABLE_OBJPOOL
#undef CPTCFG_IWLWAV_IRB_DEBUG
#undef CPTCFG_IWLWAV_RT_LOGGER_INLINES
#undef CPTCFG_IWLWAV_STAT_CNTRS_DEBUG
#undef CPTCFG_IWLWAV_SILENT
