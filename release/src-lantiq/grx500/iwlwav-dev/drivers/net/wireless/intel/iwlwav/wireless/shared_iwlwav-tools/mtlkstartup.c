/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* $Id$ */
/* Helper code and data for startup and  */
/* shutdown control module               */
#include "mtlkinc.h"
#include "mtlkstartup.h"

#define LOG_LOCAL_GID   GID_MTLKSTARTUP
#define LOG_LOCAL_FID   1

/* Startup steps counter */
int g_CurrentStepNumber = 0;
void mtlk_startup_clear_step_counter(void)
{
  g_CurrentStepNumber = 0;
}

#ifdef MTLK_DEBUG

/* Failures simulation:                               */
/* This variable is set to the number of step that    */
/* have to fail (simulation of failure for testing)   */
/* Default value of 0 means do not simulate failures. */
int g_StepNumberToFail = 0;
void mtlk_startup_set_step_to_fail(uint32 step)
{
  g_StepNumberToFail = step;
}

#endif

void __MTLK_IFUNC
MTLK_STEPS_INFO_S(const char *str)
{
  ILOG4_S("%s", str);
}

void __MTLK_IFUNC
MTLK_STEPS_INFO_SD(const char *str, int var_01)
{
  ILOG4_SD("%s (%d)", str, var_01);
}

void __MTLK_IFUNC
MTLK_STEPS_INFO_SDD(const char *str, int var_01, int var_02)
{
  ILOG4_SDD("%s (%d) (%d)", str, var_01, var_02);
}

void __MTLK_IFUNC
MTLK_STEPS_WARNING_S(const char *str)
{
  WLOG_S("%s", str);
}

void __MTLK_IFUNC
MTLK_STEPS_ERROR_S(const char *str)
{
  ELOG_S("%s", str);
}

void __MTLK_IFUNC
MTLK_STEPS_ERROR_SD(const char *str, int var_01, void *res_var)
{
  ELOG_SDDP("%s (%d), error: %ld (0x%p)", str, var_01, (long) res_var, res_var);
}

