/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "iniparser.h"
#include "mtlkerr.h"

#define INIPARSER_KEY_DELIM         ":"
#define INIPARSER_FULL_KEY_MAX_SIZE 128

/* Key creation as required by iniparser. 
 * NOTE: gkey means global key (i.e. out of sections key)
 */
#define iniparser_aux_key_const(section, key) (section INIPARSER_KEY_DELIM key)
#define iniparser_aux_gkey_const(key)         iniparser_aux_key_const("", key)

static __INLINE int
iniparser_aux_key (const char *section, 
                   const char *key, 
                   char       *buf,
                   uint32      buf_size)
{
  return sprintf_s(buf, buf_size, "%s" INIPARSER_KEY_DELIM "%s", section?section:"", key);
}

static __INLINE int
iniparser_aux_gkey (const char *key, 
                    char       *buf,
                    uint32      buf_size)
{
  return iniparser_aux_key(NULL, key, buf, buf_size);
}

static __INLINE int
iniparser_aux_fillstr (dictionary *dict,
                       const char *key,
                       char       *buf,
                       uint32      buf_size)
{
  const char *tmp = iniparser_getstr(dict, key);

  if (!tmp) {
    return MTLK_ERR_NOT_IN_USE;
  }

  if (!wave_strcopy(buf, tmp, buf_size)) {
    return MTLK_ERR_BUF_TOO_SMALL;
  }

  return MTLK_ERR_OK;
}

static __INLINE const char *
iniparser_aux_getstring (dictionary *dict,
                         const char *section,
                         const char *key,
                         char       *def)
{
  char full_key[INIPARSER_FULL_KEY_MAX_SIZE];
  int  n = iniparser_aux_key(section, key, full_key, sizeof(full_key));

#ifndef DEBUG
  MTLK_UNREFERENCED_PARAM(n);
#else
  MTLK_ASSERT(n < sizeof(full_key));
#endif

  return iniparser_getstring(dict, full_key, def);
}

static __INLINE const char *
iniparser_aux_getstr (dictionary *dict,
                      const char *section,
                      const char *key)
{
  return iniparser_aux_getstring(dict, section, key, NULL);
}

static __INLINE int
iniparser_aux_getint (dictionary *dict,
                      const char *section,
                      const char *key,
                      int         notfound)
{
  char full_key[INIPARSER_FULL_KEY_MAX_SIZE];
  int  n = iniparser_aux_key(section, key, full_key, sizeof(full_key));

#ifndef DEBUG
  MTLK_UNREFERENCED_PARAM(n);
#else
  MTLK_ASSERT(n < sizeof(full_key));
#endif

  return iniparser_getint(dict, full_key, notfound);
}

