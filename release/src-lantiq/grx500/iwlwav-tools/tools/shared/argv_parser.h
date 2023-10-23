/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __ARGV_PARSER_H__
#define __ARGV_PARSER_H__

/*! \file  arvg_parser.h
    \brief C-style command like parser

    Module that allows easy and convenient handling of C-style (int argc, char *argv[])
    command line parameters.
    It provides an ability to search for specific parameters, to make a basic command 
    line correctness chekcs, to get parameter values.
    Also, it provides auxiliary routines that could be used for automatic help screen
    creation.
*/

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \typedef struct mtlk_argv_parser_internal mtlk_argv_parser_t
    \brief   "Black box" MTLK ARGV parser object.

    \warning must not be used directly. 
             Use mtlk_argv_parser_...() API instead.
*/
typedef struct mtlk_argv_parser_internal
{
  int   argc;
  char **argv;
} __MTLK_IDATA mtlk_argv_parser_t;

/*! \def     _MTLK_ARGV_PINFO_FLAG_HAS_DATA
    \brief   MTLK ARGV parser's internal define

    \warning must not be used directly. 
             Use MTLK_ARGV_PINFO_FLAG_... defines instead.
*/
#define _MTLK_ARGV_PINFO_FLAG_HAS_DATA    0x00000001

/*! \def     MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
    \brief   MTLK ARGV parameter flag that means parameter has no data.
*/
#define MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA  0x00000000

/*! \def     MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
    \brief   MTLK ARGV parameter flag that means parameter has string data following it.
*/
#define MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA (0x00000000 | _MTLK_ARGV_PINFO_FLAG_HAS_DATA)

/*! \def     MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
    \brief   MTLK ARGV parameter flag that means parameter has numerical data following it.
*/
#define MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA (0x00000002 | _MTLK_ARGV_PINFO_FLAG_HAS_DATA)

/*! \struct mtlk_argv_param_info
    \brief  MTLK ARGV parameter info.

    Structure that describes the parameter for MTLK ARGV parser module.
*/
struct mtlk_argv_param_info
{
  const char *short_name; /*!< Short parameter name. 
                               For example, "o" for "-o <val>" parameter, 
                               Could be NULL for parameters that only have long_name.
                           */
  const char *long_name;  /*!< Long parameter name. 
                               For example, "output" for "--output <val>" parameter, 
                               Could be NULL for parameters that only have short_name.
                          */
  uint32      flags;      /*!< Additional parameter info, combination of MTLK_ARGV_PINFO_FLAG_...
                               flags
                          */
} __MTLK_IDATA;

/*! \typedef struct mtlk_argv_param_internal mtlk_argv_param_t
    \brief   "Black box" parameter object.
*/
typedef struct mtlk_argv_param_internal mtlk_argv_param_t;

/*! \fn      int mtlk_argv_parser_init(mtlk_argv_parser_t *parser, int argc, char *argv[])
    \brief   Initiates MTLK ARGV parser object.

    This function initializes internals of MTLK ARGV parser object.

    \param   parser MTLK ARGV parser object
    \param   argc   argument count as passed to the main() 
                    function on program invocation
    \param   argc   argument array as passed to the main() 
                    function on program invocation

    \return  MTLK_ERR... code
*/
int  __MTLK_IFUNC
mtlk_argv_parser_init(mtlk_argv_parser_t *parser, int argc, char *argv[]);

/*! \fn      void mtlk_argv_parser_cleanup(mtlk_argv_parser_t *parser)
    \brief   Cleans up MTLK ARGV parser object.

    This function cleans up internals of MTLK ARGV parser object.

    \param   parser MTLK ARGV parser object
*/
void __MTLK_IFUNC
mtlk_argv_parser_cleanup(mtlk_argv_parser_t *parser);

/*! \fn      mtlk_argv_param_t * mtlk_argv_parser_param_get(mtlk_argv_parser_t                *parser, 
                                                            const struct mtlk_argv_param_info *param_info)
    \brief   Searches for single parameter by info.

    This function searches single parameter by info and returns associated 
    MTLK ARGV parameter object.
    If a few same parameters are present in command line returns upon the for 1st occurrence.

    \param   parser     MTLK ARGV parser object
    \param   param_info parameter info structure

    \return  associated MTLK ARGV parameter object, NULL if failed.
*/
mtlk_argv_param_t * __MTLK_IFUNC
mtlk_argv_parser_param_get(mtlk_argv_parser_t                *parser, 
                           const struct mtlk_argv_param_info *param_info);

/*! \fn      mtlk_argv_param_t * mtlk_argv_parser_param_get_ex(mtlk_argv_parser_t                *parser, 
                                                               const struct mtlk_argv_param_info *param_info,
                                                               uint32                             no)
    \brief   Searches for single parameter by info and occurrence index.

    This function searches single parameter by info and occurrence index and 
    returns associated MTLK ARGV parameter object.

    \param   parser     MTLK ARGV parser object
    \param   param_info parameter info structure
    \param   no         desired parameter occurrence index

    \return  associated MTLK ARGV parameter object, NULL if failed.
*/
mtlk_argv_param_t * __MTLK_IFUNC
mtlk_argv_parser_param_get_ex(mtlk_argv_parser_t                *parser, 
                              const struct mtlk_argv_param_info *param_info,
                              uint32                             no);

/*! \fn      BOOL mtlk_argv_parser_param_val_ok(mtlk_argv_param_t *param)
    \brief   Makes basic parameter correctness checks.

    This function makes basic parameter correctness checks like: 
    if parameter has a data it must be followed by parameter that doesn't start 
    with '-' and it can't be last.

    \param   parameter  MTLK ARGV parameter object

    \return  TRUE if parameter seems to be OK, FALSE otherwise.
*/
BOOL __MTLK_IFUNC
mtlk_argv_parser_param_val_ok(mtlk_argv_param_t *param);

/*! \fn      const char *mtlk_argv_parser_param_get_str_val(mtlk_argv_param_t *param)
    \brief   Gets parameter string value.

    This function gets a parameter string value. For example, for "-o file" command line
    it brings the pointer to "file" string.

    \param   parameter  MTLK ARGV parameter object

    \return  pointer to a parameter string value. NULL in one of the following cases:
             - parameter is defined as MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
             - parameter correctness checks fail

    \warning this function return a pointer stored in original argv array. 
             Thus argv pointers must be valid at this point.
*/
const char * __MTLK_IFUNC
mtlk_argv_parser_param_get_str_val(mtlk_argv_param_t *param);

/*! \fn      uint32 mtlk_argv_parser_param_get_uint_val(mtlk_argv_param_t *param,
                                                        uint32             def_val)
    \brief   Gets parameter UINT32 value.

    This function gets a parameter UINT32 value. For example, for "-o 22" command line
    it brings the 22 value.

    \param   parameter  MTLK ARGV parameter object
    \param   def_val    a default value to be returned in case of error.

    \return  parameter's value, def_val in case of error.

    \warning this function use pointers stored in original argv array. 
             Thus argv pointers must be valid at this point.
*/
uint32 __MTLK_IFUNC
mtlk_argv_parser_param_get_uint_val(mtlk_argv_param_t *param,
                                    uint32             def_val);

/*! \fn      void mtlk_argv_parser_param_release(mtlk_argv_param_t *param)
    \brief   releases associated MTLK ARGV parameter object.

    This function releases associated MTLK ARGV parameter object. Object becomes unavailable.
    However, pointers received from mtlk_argv_parser_param_get_str_val() API are still valid
    as long as the original argv array is.

    \param   parameter  MTLK ARGV parameter object
*/
void __MTLK_IFUNC
mtlk_argv_parser_param_release(mtlk_argv_param_t *param);

/*! \enum  mtlk_argv_param_type
    \brief parameter's type

    Parameter's type used for help screen generation.
*/
enum mtlk_argv_param_type {
  MTLK_ARGV_PTYPE_OPTIONAL, /*!< Parameter is optional */
  MTLK_ARGV_PTYPE_MANDATORY /*!< Parameter is mandatory */
};

/*! \struct mtlk_argv_param_info_ex
    \brief  MTLK ARGV extra parameter info.

    Structure that describes the parameter for help screen generation purpose.
*/
struct mtlk_argv_param_info_ex
{
  struct mtlk_argv_param_info info; /*!< Basic parameter info */
  const char                 *desc; /*!< Human readable parameter description */
  enum mtlk_argv_param_type   type; /*!< Parameter type */
};

/*! \fn      void mtlk_argv_print_help(FILE                                  *fd, 
                                       const char                            *app_name, 
                                       const char                            *app_description,
                                       const struct mtlk_argv_param_info_ex **params,
                                       uint32                                 nof_params);
    \brief   Generates help screen.

    This function generates full application help screen and prints it out to a file.

    \param   fd              output file descriptor
    \param   app_name        human readable application name
    \param   app_description human readable application description
    \param   params          array of parameter extra info structures
    \param   nof_params      size of parameter extra info structures array
*/
void __MTLK_IFUNC
mtlk_argv_print_help(FILE                                  *fd, 
                     const char                            *app_name, 
                     const char                            *app_description,
                     const struct mtlk_argv_param_info_ex **params,
                     uint32                                 nof_params);

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __ARGV_PARSER_H__ */
