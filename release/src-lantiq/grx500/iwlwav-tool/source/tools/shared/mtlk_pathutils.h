/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_PATHUTILS_H__
#define __MTLK_PATHUTILS_H__

#ifdef WIN32
#define MTLK_PATH_SEPARATOR '\\'
#elif defined(LINUX)
#define MTLK_PATH_SEPARATOR '/'
#define MAX_PATH (256)
#else
#error Unknown operating system
#endif

int __MTLK_IFUNC
mtlk_get_path_by_path_name(const char* path_name, char* path_buff, size_t path_buff_size);

int __MTLK_IFUNC
mtlk_get_current_executable_path(char* path_buff, size_t path_buff_size);

#endif /* __MTLK_PATHUTILS_H__ */
