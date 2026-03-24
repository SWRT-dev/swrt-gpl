/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*****************************************************************************
*   Source File:
*       mt_cnfg.h
*   Product Name:
*       Metalink VDSL Software
*   Description:                               
*       Customer/User settings and Metalink EVM PPC860 settings.
*
*
*
*
*
*
*****************************************************************************/

#ifndef __MT_CNFG_H__                                 
#define __MT_CNFG_H__

/* Metalink's MT_EXTERN prefix should be used before variables definitions in this file */
#ifdef MT_GLOBAL
#define MT_EXTERN
#else
#define MT_EXTERN extern
#endif

/*****************************************************************************
*   Compiler dependant definitions - types.
*       The following type definitions should comply with the user host 
*   compiler requirements.
*****************************************************************************/
#ifndef _MS_SBR_CREATOR    

#define MT_CODE                         const              
#define MT_VOID                         void
#define MT_INLINE                       __inline__
#define MT_STATIC                       static
#define MT_INTERRUPT_PREFIX             void 
#define MT_INTERRUPT_SUFFIX 
#define MT_UBYTE                        uint8_t       /*  8 bit */
#define MT_BYTE                         int8_t        /*  8 bit */
#define MT_UINT16                       uint16_t      /* 16 bit */
#define MT_INT16                        int16_t       /* 16 bit */
#define MT_UINT32                       uint32_t      /* 32 bit */
#define MT_INT32                        int32_t       /* 32 bit */

#else /* support in MS browse info */

#define MT_CODE                         const              
#define MT_VOID                         void
#define MT_INLINE                       
#define MT_STATIC                       static
#define MT_INTERRUPT_PREFIX             void 
#define MT_INTERRUPT_SUFFIX 
#define MT_UBYTE                        uint8_t       /*  8 bit */
#define MT_BYTE                         int8_t        /*  8 bit */
#define MT_UINT16                       uint16_t      /* 16 bit */
#define MT_INT16                        int16_t       /* 16 bit */
#define MT_UINT32                       uint32_t      /* 32 bit */
#define MT_INT32                        int32_t       /* 32 bit */
#define __asm__(x)                      

#endif /* support in MS browse info */



/*****************************************************************************
*   OS dependant definition - terminal debug output.
*       The MT_TRACE macros are used in the Metalink driver for debug purposes.
*       Message are printed for errors, warnings and events.
*       We highly recommend to implement it for easy debug and better Metalink support.
*****************************************************************************/
#define MT_TRACE_ENABLE

#ifdef  MT_TRACE_ENABLE
#if !MT_EVM /* User:  */
     /* implement using your output function.
        for your own convenient, printouts are trunk oriented, so you can disable
        and enable printouts based on a trunk number.
        If a certain printout is not trunk oriented, the first argument is MT_DUMMY_ARG = 0xFF. */

/* for printouts with no arguments (just a string) */
#define MT_TRACE(MESSAGE)\
        mt_print(2,MESSAGE)    
/* for printouts with one argument  */
#define MT_TRACE_1(TRUNK,MESSAGE,ARG1)\
        mt_print(2,MESSAGE,ARG1)
/* for printouts with two arguments  */
#define MT_TRACE_2(TRUNK,MESSAGE,ARG1,ARG2)\
        mt_print(2,MESSAGE,ARG1,ARG2)    
/* for printouts with 3 arguments  */
#define MT_TRACE_3(TRUNK,MESSAGE,ARG1,ARG2,ARG3)\
        mt_print(2,MESSAGE,ARG1,ARG2,ARG3)    
/* for printouts with 4 arguments  */
#define MT_TRACE_4(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4)\
        mt_print(2,MESSAGE,ARG1,ARG2,ARG3,ARG4)   
/* for printouts with 5 arguments  */
#define MT_TRACE_5(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4,ARG5)\
        mt_print(2,MESSAGE,ARG1,ARG2,ARG3,ARG4,ARG5)    

#else /* Metalink PPC860 EVM.  Do not change: */
/* for printouts with no arguments (just a string) */
#define MT_TRACE(MESSAGE)\
        mt_mt_print(2,MESSAGE)    
/* for printouts with one argument  */
#define MT_TRACE_1(TRUNK,MESSAGE,ARG1)\
        mt_mt_print(2,MESSAGE,ARG1)
/* for printouts with two arguments  */
#define MT_TRACE_2(TRUNK,MESSAGE,ARG1,ARG2)\
        mt_mt_print(2,MESSAGE,ARG1,ARG2)    
/* for printouts with 3 arguments  */
#define MT_TRACE_3(TRUNK,MESSAGE,ARG1,ARG2,ARG3)\
        mt_mt_print(2,MESSAGE,ARG1,ARG2,ARG3)    
/* for printouts with 4 arguments  */
#define MT_TRACE_4(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4)\
        mt_mt_print(2,MESSAGE,ARG1,ARG2,ARG3,ARG4)    
/* for printouts with 5 arguments  */
#define MT_TRACE_5(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4,ARG5)\
        mt_mt_print(2,MESSAGE,ARG1,ARG2,ARG3,ARG4,ARG5)    

#endif

#else  /* MT_TRACE_ENABLE is undefined: */
       /* leave these following macros empty for both Metalink and user's code ! */
#define MT_TRACE(MESSAGE)          
#define MT_TRACE_1(TRUNK,MESSAGE,ARG1)          
#define MT_TRACE_2(TRUNK,MESSAGE,ARG1,ARG2)          
#define MT_TRACE_3(TRUNK,MESSAGE,ARG1,ARG2,ARG3)          
#define MT_TRACE_4(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4)  
#define MT_TRACE_5(TRUNK,MESSAGE,ARG1,ARG2,ARG3,ARG4,ARG5)

#endif



#undef MT_EXTERN
#endif /* __MT_CNFG_H__ */
