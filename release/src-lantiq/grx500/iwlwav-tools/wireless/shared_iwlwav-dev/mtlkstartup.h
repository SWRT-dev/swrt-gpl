/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_STARTUP_H__
#define __MTLK_STARTUP_H__

/* $Id$ */
#include "mtlkbfield.h"

#define LOG_LOCAL_GID   GID_MTLKSTARTUP
#define LOG_LOCAL_FID   0

/* Macro used by following code to print debug traces */
#define STEPS_PFX "STARTCTRL: "
#define STEPS_TAB "  " /* We use this tab instead of '\t' since
                        * '\t' is handled incorrect by syslog
                        */

/* NOP helper function */
static __INLINE
void MTLK_NOACTION(void)
{};

void __MTLK_IFUNC
MTLK_STEPS_INFO_S(const char *str);

void __MTLK_IFUNC
MTLK_STEPS_INFO_SD(const char *str, int var_01);

void __MTLK_IFUNC
MTLK_STEPS_INFO_SDD(const char *str, int var_01, int var_02);

void __MTLK_IFUNC
MTLK_STEPS_WARNING_S(const char *str);

void __MTLK_IFUNC
MTLK_STEPS_ERROR_S(const char *str);

void __MTLK_IFUNC
MTLK_STEPS_ERROR_SD(const char *str, int var_01, void *res_var);

/* Type of bit fields used to track   */
/* which steps were performed/skipped */
#define __BFIELDS_TYPE uint32

/* Step numbers for inner steps to be */
/* started from INNER_STEPS_OFFSET    */
#define INNER_STEPS_OFFSET (1000)

/* Internal names in steps enum */
#define __STEP_NUMBER(module_name, step_name, suffix)             \
  module_name##_##step_name##_##suffix

#define __LAST_OUTER_STEP_NUMBER(module_name, suffix)             \
  module_name##_##suffix##_LAST_OUTER

#define __FIRST_INNER_STEP_NUMBER(module_name, suffix)            \
  module_name##_##suffix##_FIRST_INNER

#define __LAST_INNER_STEP_NUMBER(module_name, suffix)             \
  module_name##_##suffix##_LAST_INNER

/* Internal macro to obtain bitmask bit number by step number */
#define __BIT_NUMBER(module_name, step_name, suffix)                           \
  ((__STEP_NUMBER(module_name, step_name, suffix) < INNER_STEPS_OFFSET) ?      \
      (__STEP_NUMBER(module_name, step_name, suffix))                          \
      :                                                                        \
      (__STEP_NUMBER(module_name, step_name, suffix) - INNER_STEPS_OFFSET))

#define __LAST_OUTER_BIT_NUMBER(module_name, suffix)                           \
  __LAST_OUTER_STEP_NUMBER(module_name, suffix)

#define __LAST_INNER_BIT_NUMBER(module_name, suffix)                           \
  (__LAST_INNER_STEP_NUMBER(module_name, suffix) - INNER_STEPS_OFFSET)

/* Macros for object definition */
#define MTLK_OBJ_PTR(x) (x)->
#define MTLK_OBJ_NONE

/* Macros to define ordered set of steps */
#define _MTLK_STEPS_LIST_BEGIN(module_name, suffix)               \
  enum module_name##_##suffix                                     \
  {

#define _MTLK_STEPS_LIST_ENTRY(module_name, step_name, suffix)    \
    __STEP_NUMBER(module_name, step_name, suffix),

#define _MTLK_STEPS_INNER_BEGIN(module_name, suffix)                        \
  __LAST_OUTER_STEP_NUMBER(module_name, suffix),                            \
  __FIRST_INNER_STEP_NUMBER(module_name, suffix)= INNER_STEPS_OFFSET - 1,

#define _MTLK_STEPS_LIST_END(module_name, suffix)                 \
    __LAST_INNER_STEP_NUMBER(module_name, suffix)                 \
  }

/* Macros to define ordered set of init steps */
#define MTLK_INIT_STEPS_LIST_BEGIN(module_name)                   \
  _MTLK_STEPS_LIST_BEGIN(module_name, init_steps)

#define MTLK_INIT_STEPS_LIST_ENTRY(module_name, step_name)        \
  _MTLK_STEPS_LIST_ENTRY(module_name, step_name, INIT)

#define MTLK_INIT_INNER_STEPS_BEGIN(module_name)                  \
  _MTLK_STEPS_INNER_BEGIN(module_name, INIT)

#define MTLK_INIT_STEPS_LIST_END(module_name)                     \
  _MTLK_STEPS_LIST_END(module_name, INIT);

/* Macros to define ordered set of start steps */
#define MTLK_START_STEPS_LIST_BEGIN(module_name)                   \
  _MTLK_STEPS_LIST_BEGIN(module_name, start_steps)

#define MTLK_START_STEPS_LIST_ENTRY(module_name, step_name)        \
  _MTLK_STEPS_LIST_ENTRY(module_name, step_name, START)

#define MTLK_START_INNER_STEPS_BEGIN(module_name)                  \
  _MTLK_STEPS_INNER_BEGIN(module_name, START)

#define MTLK_START_STEPS_LIST_END(module_name)                     \
  _MTLK_STEPS_LIST_END(module_name, START);

#define __BFIELD_NAME(prefix, suffix) __##prefix##_bit_field_##suffix

/* Macro that declares init status variable in module's data structure */
#define MTLK_DECLARE_INIT_STATUS __BFIELDS_TYPE __BFIELD_NAME(INIT, MAIN);      \
                                 __BFIELDS_TYPE __BFIELD_NAME(INIT, SKIPPED);
/* Macro that declares init status variable in module's data structure */
#define MTLK_DECLARE_START_STATUS __BFIELDS_TYPE __BFIELD_NAME(START, MAIN);    \
                                  __BFIELDS_TYPE __BFIELD_NAME(START, SKIPPED);
/* Macro that declares init loop counter variable in module's data structure */
#define MTLK_DECLARE_INIT_LOOP(step_name)  uint32 __loop_init_##step_name;
/* Macro that declares start loop counter variable in module's data structure */
#define MTLK_DECLARE_START_LOOP(step_name) uint32 __loop_start_##step_name;

/* Macro that returns number of cleanup iterations pending for given cleanup step */
#define MTLK_CLEANUP_ITERATONS_LEFT(object, step_name) ((const uint32)(object __loop_init_##step_name))
/* Macro that returns number of stop iterations pending for given stop step */
#define MTLK_STOP_ITERATIONS_LEFT(object, step_name)   ((const uint32)(object __loop_start_##step_name))

/* This macro is to ensure that on init/start or cleanup/stop of some submodule */
/* all previous/successive submodules are inited/started or cleaned up/stopped, */
/* i.e. this macro ensures proper order of operations as defined by steps list  */
#define _ASSERT_PREVIOUS_STEPS_DONE(mask, module_idx)                          \
  MTLK_ASSERT( (mask == MTLK_U_BITS_VALUE(0, module_idx, -1, __BFIELDS_TYPE))  \
               || !"ERROR: Order of steps is broken." )

#ifdef MTLK_DEBUG
/* Failures simulation:                               */
/* This variable is set externally (for example as    */
/* kernel module's parameter) the number of step that */
/* have to fail (simulation of failure for testing)   */
/* Default value of 0 means do not simulate failures. */
extern int g_StepNumberToFail;

/* Function to set the initial value of step number   */
/* to fail.                                           */
extern void mtlk_startup_set_step_to_fail(uint32 step);

/* This internal macro performs simulation of init/start failure */
/* This logic is debug-only                                      */
#define _PERFORM_FAILURES_SIMULATION(error_val)                     \
  if( (g_StepNumberToFail != 0) && (g_StepNumberToFail-- == 1) )    \
  {                                                                 \
    MTLK_STEPS_WARNING_S(STEPS_PFX "*** Simulating failure ***");     \
    __res = (error_val);                                            \
    goto __ON_FWD_ERROR__;                                          \
  }
#else  /* MTLK_DEBUG */
#define _PERFORM_FAILURES_SIMULATION(error_val)
#endif /* MTLK_DEBUG */

/* Steps counter for debugging output */
extern int g_CurrentStepNumber;
void mtlk_startup_clear_step_counter(void);

/* Generic macros for Init/Cleanup and Start/Stop */
#define _MTLK_FWD_STEPS_TRY(module_name, object, suffix)            \
  for(;;) {                                                         \
    int __macro_mix_guard_fwd_##suffix = 0;                         \
    int __object_mix_guard_fwd_##module_name = 0;                   \
    int __res = MTLK_ERR_OK;                                        \
    __BFIELDS_TYPE __BFIELD_NAME(suffix, MAIN) = 0;                 \
    __BFIELDS_TYPE __BFIELD_NAME(suffix, SKIPPED) = 0;              \
    (object __BFIELD_NAME(suffix, MAIN)) = 0;                       \
    (object __BFIELD_NAME(suffix, SKIPPED)) = 0;

#define _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, success_op, iter_ctr, step_func,                        \
                       func_args, suffix, res_var, success_cond, error_val)                                               \
  {                                                                                                                       \
    uint32 __exec_counter = 0;                                                                                            \
    MTLK_UNREFERENCED_PARAM(__macro_mix_guard_fwd_##suffix);                                                              \
    MTLK_UNREFERENCED_PARAM(__object_mix_guard_fwd_##module_name);                                                        \
    ++g_CurrentStepNumber;                                                                                                \
    MTLK_STEPS_INFO_SDD(STEPS_PFX STEPS_TAB #module_name " " #suffix                                                          \
                       ", step " #step_name " iteration ", iter_ctr, g_CurrentStepNumber);                    \
    _PERFORM_FAILURES_SIMULATION(error_val);                                                                              \
    if(exec_cond) {                                                                                                       \
      (res_var) = step_func func_args;                                                                                   \
      if (success_cond) {                                                                                                 \
         __exec_counter = success_op;                                                                                     \
      }                                                                                                                   \
      else {                                                                                                              \
        __res = (error_val);                                                                                              \
        MTLK_STEPS_ERROR_SD(STEPS_PFX #module_name ":" #step_name "_" #suffix " FAILED! step" , g_CurrentStepNumber, (void*) (long) res_var); \
        goto __ON_FWD_ERROR__;                                                                                            \
      }                                                                                                                   \
    } else {                                                                                                              \
      __res = MTLK_ERR_OK;                                                                                                \
      MTLK_STEPS_INFO_S(STEPS_PFX STEPS_TAB STEPS_TAB "Step SKIPPED because its precondition is false");                    \
      MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, SKIPPED),                                                             \
                       __BIT_NUMBER(module_name, step_name, suffix), 1, 1);                                               \
    }                                                                                                                     \
    if (__exec_counter == 0) { /* regular step or 1st loop iteration */                                                   \
      _ASSERT_PREVIOUS_STEPS_DONE(object __BFIELD_NAME(suffix, MAIN),                                                    \
                                   __BIT_NUMBER(module_name, step_name, suffix));                                         \
      MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),   \
                       __BIT_NUMBER(module_name, step_name, suffix), 1, 1);                                               \
    }                                                                                                                     \
  }

#define _MTLK_FWD_STEP_VOID(exec_cond, module_name, step_name, object, post_run_op, iter_ctr, step_func,                  \
                            func_args, suffix)                                                                            \
  {                                                                                                                       \
    uint32 __exec_counter = 0;                                                                                            \
    MTLK_UNREFERENCED_PARAM(__macro_mix_guard_fwd_##suffix);                                                              \
    MTLK_UNREFERENCED_PARAM(__object_mix_guard_fwd_##module_name);                                                        \
    MTLK_STEPS_INFO_SD(STEPS_PFX STEPS_TAB #module_name " " #suffix                                                               \
                      ", step " #step_name " (VOID STEP) iteration ", iter_ctr);                                   \
    if(exec_cond) {                                                                                                       \
      step_func func_args;                                                                                               \
       __exec_counter = post_run_op;                                                                                      \
    } else {                                                                                                              \
    	MTLK_STEPS_INFO_S(STEPS_PFX STEPS_TAB STEPS_TAB "Step SKIPPED because its precondition is false");                    \
      MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, SKIPPED),                                                             \
                       __BIT_NUMBER(module_name, step_name, suffix), 1, 1);                                               \
    }                                                                                                                     \
    if (__exec_counter == 0) { /* regular step or 1st loop iteration */                                                   \
      _ASSERT_PREVIOUS_STEPS_DONE(object __BFIELD_NAME(suffix, MAIN),                                                    \
                                   __BIT_NUMBER(module_name, step_name, suffix));                                         \
      MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),                                                                \
                       __BIT_NUMBER(module_name, step_name, suffix), 1, 1);                                               \
    }                                                                                                                     \
  }

#define _MTLK_FWD_STEPS_FINALLY(module_name, object, suffix)                                                                          \
  MTLK_UNREFERENCED_PARAM(__macro_mix_guard_fwd_##suffix);                                                                            \
  MTLK_UNREFERENCED_PARAM(__object_mix_guard_fwd_##module_name);                                                                      \
  _ASSERT_PREVIOUS_STEPS_DONE(object __BFIELD_NAME(suffix, MAIN),                                                                     \
                               __LAST_OUTER_BIT_NUMBER(module_name, suffix));                                                         \
  MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),                                                                                 \
                   __LAST_OUTER_BIT_NUMBER(module_name, suffix), 1, 1);                                                               \
  _ASSERT_PREVIOUS_STEPS_DONE(__BFIELD_NAME(suffix, MAIN),                                                                            \
                               __LAST_INNER_BIT_NUMBER(module_name, suffix));                                                         \
__ON_FWD_ERROR__:                                                                                                                     \
  MTLK_STEPS_INFO_S(STEPS_PFX "Inner steps of " #module_name " un-" #suffix " started...");                                             \
  {                                                                                                                                   \
    int __macro_mix_guard_bwd_##suffix = 0;                                                                                           \
    int __object_mix_guard_bwd_##module_name = 0;

/*
 * NOTE: The __LAST_OUTER_STEP_NUMBER(module_name, suffix) bit is always set at end of Init/Start, thus it can be
 *       checked and unset on Stop/Cleanup, that allows these macros to catch redundant Stop/Cleanup
 *       calls.
 */
#define _MTLK_FWD_STEPS_RETURN(module_name, object, suffix, rollback_func, rollback_params)                             \
      MTLK_UNREFERENCED_PARAM(__macro_mix_guard_bwd_##suffix);                                                          \
      MTLK_UNREFERENCED_PARAM(__object_mix_guard_bwd_##module_name);                                                    \
    }                                                                                                                   \
    MTLK_UNREFERENCED_PARAM(__macro_mix_guard_fwd_##suffix);                                                            \
    MTLK_UNREFERENCED_PARAM(__object_mix_guard_fwd_##module_name);                                                      \
    MTLK_UNREFERENCED_PARAM(__BFIELD_NAME(suffix, MAIN));                                                               \
    MTLK_UNREFERENCED_PARAM(__BFIELD_NAME(suffix, SKIPPED));                                                            \
    MTLK_ASSERT((0 == __BFIELD_NAME(suffix, MAIN)) || !"Some rollback steps missed");                                   \
    MTLK_ASSERT((0 == __BFIELD_NAME(suffix, SKIPPED)) || !"Some (skipped) rollback steps missed");                      \
    MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),                                                                 \
                     __LAST_OUTER_BIT_NUMBER(module_name, suffix), 1, 1);                                               \
    if(MTLK_ERR_OK == __res)                                                                                            \
    {                                                                                                                   \
      return MTLK_ERR_OK;                                                                                               \
      goto __ON_FWD_ERROR__; /* To eliminate warning "label __ON_FWD_ERROR__ defined but not used" in case of empty list of steps */          \
    }                                                                                                                   \
    MTLK_STEPS_ERROR_S(STEPS_PFX #module_name " " #suffix " FAILED! Initiating rollback...");                           \
    rollback_func rollback_params;                                                                                      \
    return __res;                                                                                                       \
  }                                                                                                                     \
  return MTLK_ERR_OK;

#define _MTLK_BWD_STEPS_BEGIN(module_name, object, suffix)                                                                        \
  {                                                                                                                               \
    int __macro_mix_guard_bwd_##suffix = 0;                                                                                       \
    int __object_mix_guard_bwd_##module_name = 0;                                                                                 \
    MTLK_STEPS_INFO_S(STEPS_PFX #module_name " un-" #suffix " started...");                                                         \
    MTLK_ASSERT(MTLK_U_BITS_GET(object __BFIELD_NAME(suffix, MAIN),                                                              \
                                __LAST_OUTER_BIT_NUMBER(module_name, suffix), 1) != 0 || !"Unexpected un" #suffix);                 \
    MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),                                                                          \
                     __LAST_OUTER_BIT_NUMBER(module_name, suffix), 1, 0);

#define _MTLK_BWD_STEP(module_name, step_name, object, post_op, step_func, func_args, suffix)                                \
  {                                                                                                                          \
    uint32 __exec_counter = 0;                                                                                               \
    MTLK_UNREFERENCED_PARAM(__macro_mix_guard_bwd_##suffix);                                                                 \
    MTLK_UNREFERENCED_PARAM(__object_mix_guard_bwd_##module_name);                                                           \
    if(!MTLK_U_BITS_GET(object __BFIELD_NAME(suffix, SKIPPED),                                                              \
                         __BIT_NUMBER(module_name, step_name, suffix), 1)) {                                                 \
      if(MTLK_U_BITS_GET(object __BFIELD_NAME(suffix, MAIN),                                                                \
                          __BIT_NUMBER(module_name, step_name, suffix), 1)) {                                                \
        step_func func_args;                                                                                                \
        __exec_counter = post_op;                                                                                            \
      }                                                                                                                      \
    } else {                                                                                                                 \
    	MTLK_STEPS_INFO_S(STEPS_PFX STEPS_TAB STEPS_TAB "Step was SKIPPED because precondition was false...");                   \
    }                                                                                                                        \
    if(MTLK_U_BITS_GET(object __BFIELD_NAME(suffix, MAIN),                                                                  \
                        __BIT_NUMBER(module_name, step_name, suffix), 1)) {                                                  \
    	MTLK_STEPS_INFO_SD(STEPS_PFX STEPS_TAB #module_name " un-" #suffix ", step " #step_name                                   \
                        " done iteration ", __exec_counter);                                                         \
      if (__exec_counter == 0 /* regular step or last loop iteration */) {                                                   \
        MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, MAIN),                                                                 \
                         __BIT_NUMBER(module_name, step_name, suffix), 1, 0);                                                \
        MTLK_U_BITS_SET(object __BFIELD_NAME(suffix, SKIPPED),                                                              \
                         __BIT_NUMBER(module_name, step_name, suffix), 1, 0);                                                \
        _ASSERT_PREVIOUS_STEPS_DONE(object __BFIELD_NAME(suffix, MAIN), __BIT_NUMBER(module_name, step_name, suffix));      \
      }                                                                                                                      \
    }                                                                                                                        \
  }

#define _MTLK_BWD_STEPS_END(module_name, object, suffix)                                                                \
    MTLK_UNREFERENCED_PARAM(__macro_mix_guard_bwd_##suffix);                                                            \
    MTLK_UNREFERENCED_PARAM(__object_mix_guard_bwd_##module_name);                                                      \
    MTLK_ASSERT(0 == object __BFIELD_NAME(suffix, MAIN) || !"Some rollback steps missed!");                             \
    MTLK_ASSERT(0 == object __BFIELD_NAME(suffix, SKIPPED) || !"Some (skipped) rollback steps missed!");                \
  }

/* Init/Cleanup macros */
#define MTLK_INIT_TRY(module_name, object)                                                                              \
	MTLK_STEPS_INFO_S(STEPS_PFX #module_name " init started...");                                                           \
  _MTLK_FWD_STEPS_TRY(module_name, object, INIT)

#define MTLK_INIT_STEP(module_name, step_name, object, init_step_func, func_args)                                       \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, 0, 0, init_step_func, func_args,                                 \
                 INIT, __res, __res == MTLK_ERR_OK, __res);

#define MTLK_INIT_STEP_VOID(module_name, step_name, object, init_step_func, func_args)                                  \
  _MTLK_FWD_STEP_VOID(TRUE, module_name, step_name, object, 0, 0, init_step_func, func_args, INIT);

#define MTLK_INIT_STEP_EX(module_name, step_name, object, init_step_func, func_args,                                    \
                          res_var, success_cond, error_val)                                                             \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, 0, 0, init_step_func, func_args,                                 \
                 INIT, res_var, success_cond, error_val);

#define MTLK_INIT_STEP_IF(exec_cond, module_name, step_name, object, init_step_func, func_args)                         \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, 0, 0, init_step_func, func_args,                            \
                 INIT, __res, __res == MTLK_ERR_OK, __res);

#define MTLK_INIT_STEP_VOID_IF(exec_cond, module_name, step_name, object, init_step_func, func_args)                    \
  _MTLK_FWD_STEP_VOID(exec_cond, module_name, step_name, object, 0, 0, init_step_func, func_args, INIT);

#define MTLK_INIT_STEP_EX_IF(exec_cond, module_name, step_name, object, init_step_func, func_args,                      \
                          res_var, success_cond, error_val)                                                             \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, 0, 0, init_step_func, func_args,                            \
                 INIT, res_var, success_cond, error_val);

#define MTLK_INIT_STEP_LOOP(module_name, step_name, object, init_step_func, func_args)                                  \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, object __loop_init_##step_name++,                               \
                 object __loop_init_##step_name, init_step_func, func_args, INIT,                                      \
                 __res, __res == MTLK_ERR_OK, __res);

#define MTLK_INIT_STEP_VOID_LOOP(module_name, step_name, object, init_step_func, func_args)                             \
  _MTLK_FWD_STEP_VOID(TRUE, module_name, step_name, object, object __loop_init_##step_name++,                          \
                      object __loop_init_##step_name, init_step_func, func_args, INIT);

#define MTLK_INIT_STEP_LOOP_EX(module_name, step_name, object, init_step_func, func_args,                               \
                          res_var, success_cond, error_val)                                                             \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, object __loop_init_##step_name++,                               \
                 object __loop_init_##step_name, init_step_func, func_args, INIT,                                      \
                 res_var, success_cond, error_val);

#define MTLK_INIT_STEP_LOOP_IF(exec_cond, module_name, step_name, object, init_step_func, func_args)                    \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, object __loop_init_##step_name++,                          \
                 object __loop_init_##step_name , init_step_func, func_args, INIT,                                      \
                 __res, __res == MTLK_ERR_OK, __res);

#define MTLK_INIT_STEP_VOID_LOOP_IF(exec_cond, module_name, step_name, object, init_step_func, func_args)               \
  _MTLK_FWD_STEP_VOID(exec_cond, module_name, step_name, object, object __loop_init_##step_name++,                     \
                      object __loop_init_##step_name, init_step_func, func_args, INIT);

#define MTLK_INIT_STEP_LOOP_EX_IF(exec_cond, module_name, step_name, object, init_step_func, func_args,                 \
                          res_var, success_cond, error_val)                                                             \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object,  object __loop_init_##step_name++,                         \
                 object __loop_init_##step_name, init_step_func, func_args, INIT,                                      \
                 res_var, success_cond, error_val);

#define MTLK_INIT_FINALLY(module_name, object)                                                                          \
  _MTLK_FWD_STEPS_FINALLY(module_name, object, INIT)

#define MTLK_INIT_RETURN(module_name, object, cleanup_func, cleanup_params)                                             \
	MTLK_STEPS_INFO_S(STEPS_PFX #module_name " init finished");                                                             \
  _MTLK_FWD_STEPS_RETURN(module_name, object, INIT, cleanup_func, cleanup_params);

#define MTLK_CLEANUP_BEGIN(module_name, object)                                                                         \
  _MTLK_BWD_STEPS_BEGIN(module_name, object, INIT);

#define MTLK_CLEANUP_STEP(module_name, step_name, object, cleanup_step_func, func_args)                                 \
  _MTLK_BWD_STEP(module_name, step_name, object, 0, cleanup_step_func, func_args, INIT);

#define MTLK_CLEANUP_STEP_LOOP(module_name, step_name, object, cleanup_step_func, func_args)                            \
  _MTLK_BWD_STEP(module_name, step_name, object, --(object __loop_init_##step_name), cleanup_step_func,                \
                 func_args, INIT);                                                                                      \


#define MTLK_CLEANUP_END(module_name, object)                                                                           \
  _MTLK_BWD_STEPS_END(module_name, object, INIT);                                                                       \
  MTLK_STEPS_INFO_S(STEPS_PFX #module_name " cleanup finished");

/* Start/Stop macros */
#define MTLK_START_TRY(module_name, object)                                                                             \
	MTLK_STEPS_INFO_S(STEPS_PFX #module_name " startup started...");                                                        \
  _MTLK_FWD_STEPS_TRY(module_name, object, START)

#define MTLK_START_STEP(module_name, step_name, object, start_step_func, func_args)                                     \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, 0, 0, start_step_func, func_args,                                \
                 START, __res, __res == MTLK_ERR_OK, __res);

#define MTLK_START_STEP_VOID(module_name, step_name, object, init_step_func, func_args)                                 \
  _MTLK_FWD_STEP_VOID(TRUE, module_name, step_name, object, 0, 0, init_step_func, func_args, START);

#define MTLK_START_STEP_EX(module_name, step_name, object, start_step_func, func_args,                                  \
                           res_var, success_cond, error_val)                                                            \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, 0, 0, start_step_func, func_args,                                \
                 START, res_var, success_cond, error_val);

#define MTLK_START_STEP_IF(exec_cond, module_name, step_name, object, start_step_func, func_args)                       \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, 0, 0, start_step_func, func_args,                           \
                 START, __res, __res == MTLK_ERR_OK, __res);

#define MTLK_START_STEP_VOID_IF(exec_cond, module_name, step_name, object, init_step_func, func_args)                   \
  _MTLK_FWD_STEP_VOID(exec_cond, module_name, step_name, object, 0, 0, init_step_func, func_args, START);

#define MTLK_START_STEP_EX_IF(exec_cond, module_name, step_name, object, start_step_func, func_args,                    \
                           res_var, success_cond, error_val)                                                            \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, 0, 0, start_step_func, func_args,                           \
                 START, res_var, success_cond, error_val);

#define MTLK_START_STEP_LOOP(module_name, step_name, object, start_step_func, func_args)                                \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, (object __loop_start_##step_name)++,                              \
                 object __loop_start_##step_name, start_step_func, func_args, START,                                   \
                 __res, __res == MTLK_ERR_OK, __res);

#define MTLK_START_STEP_VOID_LOOP(module_name, step_name, object, init_step_func, func_args)                            \
  _MTLK_FWD_STEP_VOID(TRUE, module_name, step_name, object, (object __loop_init_##step_name)++,                          \
  object __loop_init_##step_name, init_step_func, func_args, START);

#define MTLK_START_STEP_LOOP_EX(module_name, step_name, object, start_step_func, func_args,                             \
                           res_var, success_cond, error_val)                                                            \
  _MTLK_FWD_STEP(TRUE, module_name, step_name, object, (object __loop_start_##step_name)++,                              \
                 (object __loop_start_##step_name), start_step_func, func_args, START,                                   \
                 res_var, success_cond, error_val);

#define MTLK_START_STEP_LOOP_IF(exec_cond, module_name, step_name, object, start_step_func, func_args)                  \
  __res = MTLK_ERR_UNKNOWN; /* Each step starts from the scratch */                                                     \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, (object __loop_start_##step_name)++,                         \
                 (object __loop_start_##step_name), start_step_func, func_args, START,                                   \
                 __res, __res == MTLK_ERR_OK, __res);

#define MTLK_START_STEP_VOID_LOOP_IF(exec_cond, module_name, step_name, object, start_step_func, func_args)             \
  _MTLK_FWD_STEP_VOID(exec_cond, module_name, step_name, object, (object __loop_start_##step_name)++,                    \
  (object __loop_start_##step_name), start_step_func, func_args, START);

#define MTLK_START_STEP_LOOP_EX_IF(exec_cond, module_name, step_name, object, start_step_func, func_args,               \
                           res_var, success_cond, error_val)                                                            \
  _MTLK_FWD_STEP(exec_cond, module_name, step_name, object, (object __loop_start_##step_name)++,                         \
                 (object __loop_start_##step_name), start_step_func, func_args, START,                                   \
                 res_var, success_cond, error_val);

#define MTLK_START_FINALLY(module_name, object)                                                                         \
  _MTLK_FWD_STEPS_FINALLY(module_name, object, START)

#define MTLK_START_RETURN(module_name, object, stop_func, stop_params)                                                  \
	MTLK_STEPS_INFO_S(STEPS_PFX #module_name " start finished");                                                            \
  _MTLK_FWD_STEPS_RETURN(module_name, object, START, stop_func, stop_params);

#define MTLK_STOP_BEGIN(module_name, object)                                                                            \
  _MTLK_BWD_STEPS_BEGIN(module_name, object, START);

#define MTLK_STOP_STEP(module_name, step_name, object, stop_step_func, func_args)                                       \
  _MTLK_BWD_STEP(module_name, step_name, object, 0, stop_step_func, func_args, START);

#define MTLK_STOP_STEP_LOOP(module_name, step_name, object, stop_step_func, func_args)                                  \
  _MTLK_BWD_STEP(module_name, step_name, object, --(object __loop_start_##step_name), stop_step_func,                    \
                 func_args, START);

#define MTLK_STOP_END(module_name, object)                                                                              \
  _MTLK_BWD_STEPS_END(module_name, object, START);                                                                      \
  MTLK_STEPS_INFO_S(STEPS_PFX #module_name " stop finished");

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif //__MTLK_STARTUP_H__
