/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/****************************************************************************
****************************************************************************
**
** MODULE:         File: ../develop/src/shared/mhi_mac_event.h
**
** DATED:          Date: 2006/3/26
**
** AUTHOR:         Alex Kalish
**
** DESCRIPTION:    MAC event definitions
**
** LAST MODIFIED BY:   Author:     Alex Kalish
**                     Modtime:    26/3/06
**
****************************************************************************
**
** 
**
****************************************************************************
****************************************************************************/

#ifndef __MHI_MAC_EVENT_INCLUDED
#define __MHI_MAC_EVENT_INCLUDED

#define  MTLK_PACK_ON
#include "mtlkpack.h"

/***************************************************************************/
/***                         Return codes                                ***/
/***************************************************************************/

#define RETURN_CODE_OK          0
#define RETURN_CODE_FAILED      1
#define RETURN_CODE_IGNORED     2

/***************************************************************************/
/***                        Event Opcodes                                ***/
/***************************************************************************/

#define EVENT_MASK              0xFF
#define GET_OPCODE(x)           ( (x) & EVENT_MASK )

/***************************************************************************/
/***                        General Definitions                          ***/
/***************************************************************************/

#define EVENT_TEXT_MAX_SIZE       40
#define APP_FATAL_MAX_FNAME_SIZE  20
#define GENERIC_DATA_SIZE         32

/*-------------------------------------------------------------------------*/

typedef enum _EVENT_OPCODES   /* Attention: don't remove (delete) enum entries
                                 even if they are not in use anymore */
{
    EVENT_EXCEPTION             = 0x00,
    EVENT_DUMMY                 = 0x01, /* For debugging */
    EVENT_EEPROM_FAILURE        = 0x02, /* EEPROM ILLEGAL VERSION */
    EVENT_CALIBR_ALGO_FAILURE   = 0x03, /* ALGO CALIBRATION FAIL ON STARTUP */
    EVENT_APP_FATAL             = 0x04, /* ASSERTS and FATAL calls */
    EVENT_GENERIC_EVENT         = 0x05, /* Just any kind of event passing some data */
    EVENT_STACKWL_CROSSED       = 0x06, /* Stack Water level is crossed */

    /***************** DEFINE YOUR OWN EVENT OPCODES HERE *******************/
    /* ...                                                                  */
    /* ...                                                                  */
    /************************** END OF YOUR CODE ****************************/

    NUM_OF_EVENTS /* This variable MUST be at the end of the enum - DO NOT MOVE IT */
} EVENT_OPCODES;


/***************************************************************************/
/***                        Event Sources                                ***/
/***************************************************************************/

#define SOURCE_OFFSET   ( 28 )
#define SOURCE_MASK     ( 0xF << SOURCE_OFFSET )
#define GET_SOURCE(x)   ( (x) >> SOURCE_OFFSET )

/*-------------------------------------------------------------------------*/

#define UMAC                    0x0
#define LMAC                    0x1

#define SOURCE_UMAC ( UMAC << SOURCE_OFFSET )
#define SOURCE_LMAC ( LMAC << SOURCE_OFFSET )

/***************************************************************************/
/***                        Event Messages                               ***/
/***************************************************************************/


/* For debugging */
typedef struct _DUMMY_EVENT
{
    uint32 u32Dummy1;
    uint32 u32Dummy2;
    uint32 u32Dummy3;
    uint32 u32Dummy4;
    uint32 u32Dummy5;
    uint32 u32Dummy6;
    uint32 u32Dummy7;
    uint32 u32Dummy8;
} __MTLK_PACKED DUMMY_EVENT;

/***************************************************************************
**
** NAME         EEPROM_FAILURE_EVENT
**
** PARAMETERS   u32ErrCode  Error code
**
** DESCRIPTION  EEPROM Failure Event. This message is sent by the
**              MAC ( upper or lower ) to indicate error in access to EEPROM.
**
****************************************************************************/
typedef struct _EEPROM_FAILURE_EVENT
{
    uint8 u8ErrCode; //Error code
    uint8 reserved[3];
} __MTLK_PACKED EEPROM_FAILURE_EVENT;

/***************************************************************************
**
** NAME         CALIBR_ALGO_EVENT
**
** PARAMETERS   u8calibrAlgoType
**              u8ErrCode
**
** DESCRIPTION  Calibration Algo Failure event. This message is sent by Lower
**              MAC with status of calibration.
**
****************************************************************************/
typedef struct _CALIBR_ALGO_EVENT
{
    uint32  u32calibrAlgoType; /* Type of calibration */
    uint32  u32ErrCode;        /* Type of error */
} __MTLK_PACKED CALIBR_ALGO_EVENT;

/***************************************************************************
**
** NAME         EVENT_APP_FATAL
**
** PARAMETERS   uCoreNum                according to CoreNum_e         
**              uTimeStamp              Time of the Error     
**              uCauseRegOrLineNum      Cause register in case of exception
**              uStatusReg              Status register in case of exception
**              uEpcReg                 Epc register in case of exception
**              FileName                Filename in case of Assert
**
** DESCRIPTION  Sent when there is a call to assert or Exception
**
****************************************************************************/
typedef struct _APP_FATAL
{
    uint32    uCoreNum;   
    uint32    uTimeStamp;
    uint32    uCauseRegOrLineNum;
    uint32    uStatusReg;
    uint32    uEpcReg;  
    uint32    FileId;
    uint32    GroupId;
	uint32    OriginId;
	uint32    FWinterface;
	uint32	  uFirstFreePdOffset;
} __MTLK_PACKED APP_FATAL;

/***************************************************************************
**
** NAME         EVENT_GENERIC_EVENT
**
** PARAMETERS   u32dataLength   Defines the length of data
**              u8data          Array holding generic data.
**
** DESCRIPTION  Sent when MAC wants to pass some information to driver.
**
****************************************************************************/
typedef struct _GENERIC_EVENT
{
    uint32 u32dataLength;
    uint8  u8data[MTLK_PAD4(GENERIC_DATA_SIZE)];
} __MTLK_PACKED GENERIC_EVENT;

/***************************************************************************
**
** NAME         STACKWL_CROSSED_EVENT
**
** PARAMETERS   u32ErrCode  Error code
**
** DESCRIPTION  Stack Water level crossed Event. This message is sent by the
**              MAC ( upper or lower ) to indicate that WL in stack is crossed.
**
****************************************************************************/
typedef struct _STACKWL_CROSSED_EVENT
{
    uint8 u8ErrCode; //Error code
    uint8 reserved[3];
} __MTLK_PACKED STACKWL_CROSSED_EVENT;

/****************** DEFINE YOUR OWN EVENT MESSAGES HERE ********************/

/***** DO NOT FORGET TO INSERT YOUR EVENTS TO THE UNION DEFINED BELOW ******/

/*
typedef struct
{
...
...
} XXX_EVENT;
*/



/***************************************************************************/
/***                      Common Message Structure                       ***/
/***************************************************************************/

/***************************************************************************
**
** NAME         MC_MAN_MAC_EVENT_IND
**
** PARAMETERS   u32EventID          Event identifier
**              u                   Union that includes all user defined events
**
** DESCRIPTION  MAC Event
**
****************************************************************************/
typedef struct _MAC_EVENT
{
    uint32 u32EventID;
    uint32 u32Timestamp;

    /* You should include your newly defined event message in this union */
    union
    {
        DUMMY_EVENT          sDummyEvent      ;/* For debugging */
        EEPROM_FAILURE_EVENT sEepromEvent     ;
        CALIBR_ALGO_EVENT    sCalibrationEvent;
        APP_FATAL            sAppFatalEvent   ;
        GENERIC_EVENT        sGenericData     ;
        STACKWL_CROSSED_EVENT sStackWLEvent   ;
        /*
        XXX_EVENT sXXXEvent;
        ...
        */
    } u;

} __MTLK_PACKED MAC_EVENT;

#define  MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* __MHI_MAC_EVENT_INCLUDED */
