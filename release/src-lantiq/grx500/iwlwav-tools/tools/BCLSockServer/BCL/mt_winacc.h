/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

// Hyp.h: interface for the Hyp class.
//
//////////////////////////////////////////////////////////////////////


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BCLHYP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BCLHYP_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifdef BCLHYP_EXPORTS
#define BCLHYP_API __declspec(dllexport)
#else
#define BCLHYP_API __declspec(dllimport)
#endif

#define IN
#define OUT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _CARD_DESCRIPTION
{
    TCHAR _guid[64];
    TCHAR _descr[192];
    BOOLEAN _isWireless;
} CARD_DESCRIPTION, *PCARD_DESCRIPTION;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Initialize card. This function must be called first before all others.
 * Parameter cardGuid can be retrieved IPHELPER functions
 * #include <windows.h> must be inserted as well.
 */
BCLHYP_API void HypGetVersion(LPDWORD major, LPDWORD minor, LPTSTR csBuilt, size_t size);
BCLHYP_API void HypPrintMessage(IN LPCTSTR fmt, ...);

BCLHYP_API LONG HypInitialize(IN LPCTSTR cardGuid);
BCLHYP_API LONG HypCmd(IN UINT command, IN BYTE inSize, OUT PUINT inData, OUT LPBYTE outSize, OUT PUINT outData);
BCLHYP_API LONG HypRead(IN BYTE unit, IN UINT address, IN OUT LPBYTE pSize, OUT PUINT data);
BCLHYP_API LONG HypWrite(IN BYTE unit, IN UINT address, IN BYTE size, IN PUINT data);
BCLHYP_API LONG HypGetMibParams(IN UINT id, IN UINT type, IN UINT sizeIn, IN OUT LPBYTE data, OUT PUINT sizeOut);
BCLHYP_API LONG HypSetMibParams(IN UINT id, IN UINT type, IN UINT size, IN LPBYTE data);

BCLHYP_API LONG HypStopLowerMac();
BCLHYP_API LONG HypSetMacCalibration();

BCLHYP_API LONG HypGenericMacRequest(IN PVOID psRequest);

BCLHYP_API LONG HypEnumNetworkIntrfaces(IN PCARD_DESCRIPTION pCards, IN OUT PUINT pCount);

BCLHYP_API LONG HypBclStatCatInit(IN DWORD category, OUT LPDWORD counters);
BCLHYP_API LONG HypBclStatCatFree(IN DWORD category);
BCLHYP_API LONG HypBclStatGetName(IN DWORD category, IN DWORD index, OUT LPBYTE name);
BCLHYP_API LONG HypBclStatGetVal(IN DWORD category, IN DWORD index, OUT LPDWORD val);
BCLHYP_API LONG HypBclStatSetVal(IN DWORD category, IN DWORD index, IN DWORD val);


#ifdef __cplusplus
}
#endif

//
// Error codes
//
#define BCL_ERR_GENERAL_ERROR                       (-100)
#define BCL_ERR_CARD_NOT_INITIALIZED                (-101)
#define BCL_ERR_EMPTY_GUID                          (-102)
#define BCL_ERR_INVALID_UNIT_VALUE                  (-103)
#define BCL_ERR_INVALID_IN_SIZE_VALUE               (-104)
#define BCL_ERR_INVALID_OUT_SIZE_VALUE              (-105)
#define BCL_ERR_INVALID_DATA_PTR                    (-106)
#define BCL_ERR_INVALID_CARDS_BUFFER                (-107)
#define BCL_ERR_INVALID_CARDS_COUNT                 (-108)
#define BCL_ERR_CARDS_BUFFER_TOO_SMALL              (-109)
