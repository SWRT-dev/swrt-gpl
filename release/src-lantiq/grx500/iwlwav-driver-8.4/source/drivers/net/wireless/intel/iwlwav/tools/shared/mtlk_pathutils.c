/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_pathutils.h"

int __MTLK_IFUNC
mtlk_get_path_by_path_name(const char* path_name, char* path_buff, size_t path_buff_size)
{
  size_t length_needed;
  char* last_slash = strrchr(path_name, MTLK_PATH_SEPARATOR);

  if(NULL == last_slash)
  {
    static const char local_dir[] = ".";
    length_needed = 1;
    wave_strncopy(path_buff, local_dir, path_buff_size, length_needed);
  }
  else
  {
    length_needed = last_slash - path_name;
    wave_strncopy(path_buff, path_name, path_buff_size, length_needed);
  }

  return (length_needed < path_buff_size) ? MTLK_ERR_OK : MTLK_ERR_BUF_TOO_SMALL;
};

int __MTLK_IFUNC
mtlk_get_current_executable_path(char* path_buff, size_t path_buff_size)
{
  int res = mtlk_get_current_executable_name(path_buff, path_buff_size);

  if(MTLK_ERR_OK == res)
    res = mtlk_get_path_by_path_name(path_buff, path_buff, path_buff_size);

  return res;
}

