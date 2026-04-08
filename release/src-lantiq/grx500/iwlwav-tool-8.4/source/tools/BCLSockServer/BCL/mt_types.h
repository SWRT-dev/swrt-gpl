/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*****************************************************************************
*   MT_TYPES.H
* 
*   Compiler dependant type definitions.
*   The following type definitions should comply with the customer host 
*   compiler requirements.
*	Author: Ron Zilberman
*
*****************************************************************************/

#define MT_UBYTE                        unsigned char      /*  8 bit */
#define MT_BYTE                         signed char        /*  8 bit */
#define MT_UINT16                       unsigned short int /* 16 bit */
#define MT_INT16                        signed short int   /* 16 bit */
#define MT_UINT32                       unsigned long int  /* 32 bit */
#define MT_INT32                        signed long int    /* 32 bit */
#define MT_CODE                         const              
#define MT_VOID                         void

#define MT_TRUE 1
#define MT_FALSE 0
