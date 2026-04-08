/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "argv_parser.h"
#include "mtlk_osal.h"

#define LOG_LOCAL_GID   GID_ARGV_PARSER
#define LOG_LOCAL_FID   1

struct mtlk_argv_param_internal
{
  uint32              flags;
  int                 index;
  mtlk_argv_parser_t *parser;
};

int  __MTLK_IFUNC
mtlk_argv_parser_init (mtlk_argv_parser_t *parser, int argc, char *argv[])
{
  MTLK_ASSERT(parser != NULL);

  parser->argc = argc;
  parser->argv = argv;

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_argv_parser_cleanup (mtlk_argv_parser_t *parser)
{
  MTLK_ASSERT(parser != NULL);

  memset(parser, 0, sizeof(*parser));
}

mtlk_argv_param_t * __MTLK_IFUNC
mtlk_argv_parser_param_get(mtlk_argv_parser_t                *parser, 
                           const struct mtlk_argv_param_info *param_info)
{
  return mtlk_argv_parser_param_get_ex(parser, param_info, 0);
}

mtlk_argv_param_t * __MTLK_IFUNC
mtlk_argv_parser_param_get_ex(mtlk_argv_parser_t                *parser, 
                              const struct mtlk_argv_param_info *param_info,
                              uint32                             no)
{
  mtlk_argv_param_t *param = NULL;
  int                i;
  uint32             param_no = 0;

  MTLK_ASSERT(parser != NULL);
  MTLK_ASSERT(param_info != NULL);
  MTLK_ASSERT(param_info->short_name != NULL || param_info->long_name != NULL);

  for (i = 1; i < parser->argc; i++) {
    int found = 0;

    if (parser->argv[i][0] != '-') {
      continue;
    }

    /* argv starts with '-' below this point */
    found = (
             /* argv starts with '--' and long_name matches */
             (parser->argv[i][1] == '-' &&
              param_info->long_name &&
              strcmp(param_info->long_name, &parser->argv[i][2]) == 0) ||
             /* argv starts with '-' and short_name matches */
             (param_info->short_name &&
              strcmp(param_info->short_name, &parser->argv[i][1]) == 0)
             );

    if (!found) {
      ;
    }
    else if (no == param_no) {
      break;
    }
    else {
      ++param_no;
    }
  }

  if (i == parser->argc) {
    /* no such parameter*/
    goto end;
  }

  param = (mtlk_argv_param_t *)mtlk_osal_mem_alloc(sizeof(*param), 
                                                   MTLK_MEM_TAG_ARGV_PARSER);

  if (!param) {
    ELOG_V("Can't allocate parameter object!");
    goto end;
  }

  param->flags  = param_info->flags;
  param->index  = i;
  param->parser = parser;

end:
  return param;
}

BOOL __MTLK_IFUNC
mtlk_argv_parser_param_val_ok (mtlk_argv_param_t *param)
{
  MTLK_ASSERT(param != NULL);

  return ((param->flags & _MTLK_ARGV_PINFO_FLAG_HAS_DATA) &&
          param->index + 1 < param->parser->argc &&
          param->parser->argv[param->index + 1][0] != '-');
}

const char * __MTLK_IFUNC
mtlk_argv_parser_param_get_str_val (mtlk_argv_param_t *param)
{
  MTLK_ASSERT(param != NULL);

  return mtlk_argv_parser_param_val_ok(param)?param->parser->argv[param->index + 1]:NULL;
}

uint32 __MTLK_IFUNC
mtlk_argv_parser_param_get_uint_val (mtlk_argv_param_t *param,
                                     uint32             def_val)
{
  uint32      res = def_val;
  const char *str;

  MTLK_ASSERT(param != NULL);

  str = mtlk_argv_parser_param_get_str_val(param);
  if (str) {
    res = (uint32)strtoul(str, NULL, 0);
  }
  return res;
}

void __MTLK_IFUNC
mtlk_argv_parser_param_release (mtlk_argv_param_t *param)
{
  MTLK_ASSERT(param != NULL);

  mtlk_osal_mem_free(param);
}

void __MTLK_IFUNC
mtlk_argv_print_help (FILE                                  *fd, 
                      const char                            *app_name, 
                      const char                            *app_description,
                      const struct mtlk_argv_param_info_ex **params,
                      uint32                                 nof_params)
{
  uint32 i;

  MTLK_ASSERT(fd != NULL);
  MTLK_ASSERT(app_name != NULL);
  MTLK_ASSERT(app_description != NULL);
  MTLK_ASSERT(params != NULL);
  MTLK_ASSERT(nof_params != 0);

  fprintf(fd, "Usage: %s [OPTIONS]\n", app_name);
  fprintf(fd, "%s\n", app_description);
  fprintf(fd, "\n");
  fprintf(fd, "Available OPTIONS ([MST] - mandatory, [OPT] - optional):");
  fprintf(fd, "\n");

  for (i = 0; i < nof_params; i++) {
    const struct mtlk_argv_param_info_ex *param = params[i];
    const char                           *val_str; 

    val_str = (param->info.flags & MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA)?"<value>":"";

    if (param->info.long_name) {
      fprintf(fd, "  --%s %s\n", param->info.long_name, val_str);
    }
    if (param->info.short_name) {
      fprintf(fd, "  -%s %s\n", param->info.short_name, val_str);
    }

    fprintf(fd, "      - [%s] %s\n", 
            (param->type == MTLK_ARGV_PTYPE_MANDATORY)?"MST":"OPT", param->desc);
  }
}

