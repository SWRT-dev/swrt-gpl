/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 *
 *
 * This file has common data for the driver and tools utilities
 * to implement and support data exchange
 *
 */

#include "mtlkinc.h"

#define LOG_LOCAL_GID   GID_DATAEX
#define LOG_LOCAL_FID   0

BOOL __MTLK_IFUNC
wave_check_size_exp (size_t data_size, size_t expected_size, const char *fn_name, int n_line)
{
  if (__UNLIKELY(data_size != expected_size)) {
    ELOG_SDDD("%s(%d): incorrect data size %u (expected %u)", fn_name, n_line, data_size, expected_size);
    return FALSE;
  }
  return TRUE;
}

BOOL __MTLK_IFUNC
wave_check_size_min (size_t data_size, size_t min_size, const char *fn_name, int n_line)
{
  if (__UNLIKELY(data_size < min_size)) {
    ELOG_SDDD("%s(%d): incorrect data size %u (expected %u or more)", fn_name, n_line, data_size, min_size);
    return FALSE;
  }
  return TRUE;
}

BOOL __MTLK_IFUNC
wave_check_size_max (size_t data_size, size_t max_size, const char *fn_name, int n_line)
{
  if (__UNLIKELY(data_size > max_size)) {
    ELOG_SDDD("%s(%d): incorrect data size %u (expected %u or less)", fn_name, n_line, data_size, max_size);
    return FALSE;
  }
  return TRUE;
}

