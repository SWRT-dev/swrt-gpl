/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"

struct _named_enum_entry
{
  int id;
  char* description;
};

#undef MTLK_NAMED_ENUM_START
#undef MTLK_NAMED_ENUM_END
#undef MTLK_NAMED_ENUM_ENTRY

#define _MTLK_ERR_C_BUILD_

#define MTLK_NAMED_ENUM_START static const struct \
                              _named_enum_entry error_descriptions[] = {
#define MTLK_NAMED_ENUM_END(type) {0, NULL} };
#define MTLK_NAMED_ENUM_ENTRY(name, value, descr) {value, descr},

#include "mtlkerr.h"

const char* __MTLK_IFUNC
mtlk_get_error_text(mtlk_error_t id)
{
  int i;
  /* FiXME should look up diretly by -id instead of a for loop */
  for(i = 0; NULL != error_descriptions[i].description; i++)
  {
    if(error_descriptions[i].id == id) return error_descriptions[i].description;
  }

  return "UNKNOWN";
}

