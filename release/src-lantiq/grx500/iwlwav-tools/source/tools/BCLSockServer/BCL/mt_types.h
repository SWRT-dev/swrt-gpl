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
*   MT_TYPES.H
* 
*   Compiler dependant type definitions.
*   The following type definitions should comply with the customer host 
*   compiler requirements.
*	Author: Ron Zilberman
*
*****************************************************************************/

#define MT_UBYTE                        uint8_t       /*  8 bit */
#define MT_BYTE                         int8_t        /*  8 bit */
#define MT_UINT16                       uint16_t      /* 16 bit */
#define MT_INT16                        int16_t       /* 16 bit */
#define MT_UINT32                       uint32_t      /* 32 bit */
#define MT_INT32                        int32_t       /* 32 bit */
#define MT_CODE                         const              
#define MT_VOID                         void

#define MT_TRUE 1
#define MT_FALSE 0
