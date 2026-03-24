/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

// mt_wchacc.cpp: implementation of the wireless WINDOWS driver chip access
//
//////////////////////////////////////////////////////////////////////

#include "mtlkinc.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#if _MSC_VER >= 1400
#pragma warning (disable: 4996)
#endif

#include "mt_types.h"
#include "mt_bcl.h"
#include "mt_util.h"
#include "mt_chacc.h"
#include "mt_bcl_funcs.h"
#include "mt_wapi.h"
#include "mt_winacc.h"
#include "Registry.h"
#include "..\BclSockServer.h"


#define MT_REQUEST_GET                          0
#define MT_REQUEST_SET                          1

typedef struct
{
	unsigned long opcode;
	unsigned long size;
	unsigned long action;
	unsigned long res0;
	unsigned long res1;
	unsigned long res2;
	unsigned long retStatus;
	unsigned long data[64];
} OID_GENERIC_MAC_REQUEST, *POID_GENERIC_MAC_REQUEST;

int g_driverIndex = 0;

/*********************************************************************************
***                                                                            ***
***                            Local Definitions                               ***
***                                                                            ***
**********************************************************************************/

#define VERCTL_DEFAULT_DLL _T("G:\\Software\\_PC_SW\\Latest_Versions\\CtlTool\\MtVerCtrl.dll")
#define VERCTL_DEFAULT_INI _T("G:\\Software\\_PC_SW\\Latest_Versions\\CtlTool\\mt_ver.ini")
typedef BOOL (CALLBACK*  LPFN_DLLFUNC_VERCTL)   ( const char *pcPath,
                                                  const char *pcAppName,
                                                  const char *pcCurrentVer);

/*********************************************************************************
***                                                                            ***
***                            Local Functions Definition                      ***
***                                                                            ***
**********************************************************************************/
MT_UINT32 HypInit(void);

#ifndef UNDER_CE
int       RemoteFnVerctl( const char *pcDllPath, const char *pcIniPath, const char *pcAppName,const char *pcCurrentVer);
#endif // !UNDER_CE

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


void PrintError(char * function,MT_INT32 errCode)
{
    
    char msg[50];
    char errors[10][30] = {
        "General error",
        "Card not initialized",
        "empty GUID"             ,
        "Invalid unit value"     ,
        "Invalid in size value"  ,
        "Invalid out size value" ,
        "Invalid data ptr"       ,
        "Invalid cards buffer"   ,
        "Invalid cards count"    ,
        "Cards buffer too small" };
    char * err = errors[0];

    if (errCode < -99 && errCode > -110)
        err = errors[-100 - errCode];
    sprintf(msg,"%s failed, error number: %d '%s'",function,errCode,err);
    error(msg);

}


// Function name	: HypPciRead
// Description	    : perform reading from the hyperion through the PCI.
// Return type		: MT_UINT32 -  MT_RET_OK / MT_RET_FAIL
// Argument         : MT_UBYTE unit - Hyperyon,Athena or Prometheus
// Argument         : MT_UINT32 address - start address to read from
// Argument         : MT_UBYTE size - amount of double words (32 bit) to read
// Argument         : MT_UINT32 * data - pointer to a buffer to read into
MT_UINT32 HypPciRead(MT_UBYTE unit, MT_UINT32 address, MT_UBYTE size, MT_UINT32 * data)
{

    MT_INT32 ret;
    mt_print(2,"reading unit = %d addr 0x%X size=%d ptr=0x%x\n",unit,address,size,data);
    ret = HypRead( unit,  address, &size, (unsigned int *) data);
    if (ret)
    {
        PrintError("HypPciRead",ret);
    }
    return ret;

}

// Function name	: HypPciWrite
// Description	    : perform writing to the hyperion through the PCI.
// Return type		: MT_UINT32 -  MT_RET_OK / MT_RET_FAIL
// Argument         : MT_UBYTE unit - Hyperyon,Athena or Prometheus
// Argument         : MT_UINT32 address - start address to write  to
// Argument         : MT_UBYTE  size - amount of double words (32 bit) to write
// Argument         : MT_UINT32 * data - pointer to a buffer with the data to write
MT_UINT32 HypPciWrite(MT_UBYTE unit, MT_UINT32 address, MT_UBYTE size, MT_UINT32 * data)
{
    MT_INT32 ret;
    mt_print(2,"writing unit = %d addr 0x%X size=%d ptr=0x%x\n",unit,address,size,data);
    ret = HypWrite( unit,  address, size,  (unsigned int *) data);
    if (ret)
    {
        PrintError("HypPciRead",ret);
    }
    return ret;

}




// Function name	: BCL_Init
// Description	    : called by the BCL engine when BCL is initialized
// Return type		: int - Non zero if init was successful
int BCL_Init()
{
// We don't need version check under CE
#ifndef UNDER_CE
    // first check for a new version
    if (!RemoteFnVerctl(VERCTL_DEFAULT_DLL, VERCTL_DEFAULT_INI,
    "BCL_HYP_WIN",
    BCL_VERSION_STRING))
    return 0;
#endif // UNDER_CE

    if (!HypInit())
    {
        mt_print(2,"************* ERROR **************\n");
        mt_print(2,"Can't initialize Hyperion chip !!!!\n");
        mt_print(2,"Please check driver installation and rerun the BCL\n");
        MT_SLEEP(3000);
#ifndef _DEBUG
        return 0;
#endif
    }
    return 1;
}



// Function name	: BCL_Exit
// Description	    : called by the BCL engine when BCL is exiting
// Return type		: none
void BCL_Exit()
{
}

#ifndef UNDER_CE

/***************** version control ********************************/


// A wrapper that calls the fnVerctl from a dll at a remote path:
//      pcDllPath    = Full path to remote version control DLL file
//      pcIniPath    = Full path to version control INI file
//      pcAppName    = Application Name (the section in the INI file)
//      pcCurrentVer = string of the version used at the moment, e.g. "1.3"
int  RemoteFnVerctl( const char *pcDllPath,      // Full path to remote version control DLL file
                     const char *pcIniPath,      // Full path to version control INI file
                     const char *pcAppName,      // Application Name (the section in the INI file)
                     const char *pcCurrentVer)  // string of the version used at the moment, e.g. "1.3"
{
    HINSTANCE    hDll_VerCtl = NULL;
    int nRet = -1;
    // Remote function pointer:
    LPFN_DLLFUNC_VERCTL pFnVerctl = NULL;

    // Attempt to load the verctl dll:
    if (hDll_VerCtl == NULL) {
        hDll_VerCtl = LoadLibrary(pcDllPath);
    }

    // Attempt to get a function ptr to fnVerctl:
    if (hDll_VerCtl != NULL) {
        pFnVerctl = (LPFN_DLLFUNC_VERCTL)GetProcAddress(hDll_VerCtl, "_fnVerctl@12");
    }

    // Perform version control if the load was successful:
    if (pFnVerctl) {
        nRet = pFnVerctl(pcIniPath, pcAppName, pcCurrentVer);
    }

    // Free the verctl dll:
    FreeLibrary(hDll_VerCtl);
    hDll_VerCtl = NULL;

    return nRet;
}

#endif // !UNDER_CE



// Function name	: HypInit
// Description	    : find and open Metalink's wirless card
// Return type		: MT_UINT32 - non zero of successful
// Argument         : void
MT_UINT32 HypInit(void)
{
    LONG lError = NO_ERROR;
    UINT j;
    TCHAR guid[64];
    CARD_DESCRIPTION cards[15];
    UINT cardsCount = 15;
    MT_UINT32 ret = 0;
    guid[0] = 0;

    HypEnumNetworkIntrfaces(cards, &cardsCount);
    mt_print(2,("\nNetwork Intrfaces:\n"));
    for ( j = 0; j < cardsCount; j++)
    {
#ifdef UNICODE
		mt_print(2,("%d: %S %S\n\t%S\n\n")
                    , j + 1
                    , cards[j]._guid
                    , (cards[j]._isWireless ? _T("[*]") : _T(""))
                    , cards[j]._descr
                );
#else
        mt_print(2,("%d: %s %s\n\t%s\n\n")
                    , j + 1
                    , cards[j]._guid
                    , (cards[j]._isWireless ? _T("[*]") : _T(""))
                    , cards[j]._descr
                );
#endif // UNICODE
        if (_tcsstr(cards[j]._descr,_T("Metalink")) != NULL || /* For CE */ _tcsstr(cards[j]._descr,_T("MTLK")) != NULL)
        {
            _tcscpy(guid, cards[j]._guid);
            lError = HypInitialize(guid);
            mt_print(2,"HypInitialize() --> %d\n", lError);
            ret = 1;
        }
    }
    if (ret == 1)
    {
#ifdef UNDER_CE
        #define MTLK_DRV_REG_PATH _T("Comm\\")

        TCHAR szMetalinkEntry[MAX_PATH];
        _tcscpy(szMetalinkEntry, MTLK_DRV_REG_PATH);
        _tcscat(szMetalinkEntry, guid);
        szMetalinkEntry[_tcslen(guid) + _tcslen(MTLK_DRV_REG_PATH)] = _T('\0');

        if (!Registry_Open((LPTSTR)szMetalinkEntry))
        {
            ret = 0;
        }
        else
        {
            Registry_Close();
        }
#else // Windows XP & related
        if (!Registry_Open((LPTSTR)_T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}")))
        {
            ret = 0;
        }
        else
        {
            if (Registry_FindDriverIndex((LPTSTR)guid) == -1)
                ret = 0;

            Registry_Close();
        }
#endif
    }
    return ret;
}



// Function name	: GetMibParams
// Description	    : perform reading MIB parameter through the PCI.
// Return type		: MT_UINT32        -  MT_RET_OK / MT_RET_FAIL
// Argument         : MT_UBYTE ObjectID    - MIB parameter
// Argument         : MT_UBYTE ObjectType    - MIB type
// Argument         : MT_UBYTE size    - amount of double words (32 bit) to read
// Argument         : MT_UBYTE RequestStatus  - Return status
// Argument         : MT_UBYTE * data - pointer to a buffer to read into
// SCL command      : GETMIB ID Type Size Itemlist
MT_UINT32 GetMibParams(MT_UINT32 Id, MT_UBYTE type, MT_UINT32 size, MT_BYTE * data)
{
	MT_UINT32 i;
	MT_UINT32 sizeOut;

	mt_print(2,"Id = %d(0x%04X) type %d size=%d ptr=0x%X",Id,Id,type,size,data);
	HypGetMibParams(Id, type, size, data,&sizeOut);
	mt_print(2,"\t0x");
	for (i = 0; i < sizeOut; i++) mt_print(2,"%02X", data[i]);
	mt_print(2,"\n");
	return sizeOut;
}

// Function name	: SetMibParams
// Description	    : perform writing MIB parameter through the PCI.
// Return type		: MT_UINT32        -  MT_RET_OK / MT_RET_FAIL
// Argument         : MT_UBYTE ObjectID    - MIB parameter
// Argument         : MT_UBYTE ObjectType    - MIB type
// Argument         : MT_UBYTE size    - amount of bytes to read
// Argument         : MT_UBYTE RequestStatus  - Return status
// Argument         : MT_UBYTE * data - pointer to a buffer to read from
// SCL command      : SETMIB ID Type Size Itemlist
MT_UINT32 SetMibParams(MT_UINT32 Id, MT_UBYTE type, MT_UINT32 size, MT_BYTE * data)
{
	MT_UINT32 ret;
	MT_UINT32 i;

	mt_print(2,"Id = %d(0x%04X) type %d size=%d ptr=0x%X\t0x",Id,Id,type,size,data);
	for (i = 0; i < size; i++) mt_print(2,"%02X", data[i]);
	mt_print(2,"\n");
	ret = HypSetMibParams(Id, type, size, data);
	return ret;
}

// Function name	: SetMacCalibration
// Description	    : perform get ready to get MIB parameters. Followed "stop" lower MAC request.
// Return type		: MT_UINT32        -  MT_RET_OK / MT_RET_FAIL
// SCL command      : MACCALIBRATE - Enables MIB calibration on the fly
MT_UINT32 SetMacCalibration(void)
{
	MT_UINT32 ret;

	mt_print(2,"Set Stop Lower Mac");
	ret = HypStopLowerMac();
	if (ret != ERROR_SUCCESS)
	{
		mt_print(2,"\n");
		return 1;
	}

	mt_print(2,",\tSetMacCalibration\n");
	ret = HypSetMacCalibration();
	if (ret != ERROR_SUCCESS)
		return 1;

	return 0;
}


#define GENIOCTL_MAX_PARAMS 7

// Function name	: GenIoctl
// Description	    : perform reading MAC Variable through the PCI.
// Return type		: MT_UINT32        -  MT_RET_OK / MT_RET_FAIL
// Argument         : MT_UBYTE opcode    - requested command
// Argument         : MT_UBYTE size    - amount of bytes to read
// Argument         : MT_UBYTE action  - Set/Get
// Argument         : MT_UBYTE res0/1/2  - for data transfer (instead of the data array)
// Argument         : MT_UBYTE retStatus  - status of MAC operation
// Argument         : MT_UINT32 * data - pointer to a buffer
// SCL command      : GENIOCTL opcode size action Itemlist
MT_UINT32 GenIoctl()
{
	char *params;
	int numConvErr;
	MT_UINT32 i, numOfParams, parameters[GENIOCTL_MAX_PARAMS];
	MT_UINT32 * pItem = (MT_UINT32 *)MT_BCL_item_list[0];

	OID_GENERIC_MAC_REQUEST *sIoctl;
	int retVal = 0;

	params = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
	/*character right after the DMEM_read command. */
	/* get the number of parameters */
	numOfParams = wordsCount(params,' ');
	if (numOfParams > GENIOCTL_MAX_PARAMS || numOfParams == 0)
	{
		error("Wrong number of arguments");
		return 1;
	}

	for (i = 0; i < numOfParams; i++)
	{
		parameters[i] = asciiToUnsigned(params,' ', &numConvErr);
		if(numConvErr)
		{
			strCpy(MT_BCL_cmd_line, "Invalid value entered for argument ");
			unsignedToAscii(i + 1, &MT_BCL_cmd_line[strLen(MT_BCL_cmd_line)]);
			return 1;
		}
		params = skipDelimiters( skipWord(params,' '),' ' );
	}

	sIoctl = (OID_GENERIC_MAC_REQUEST *)malloc( sizeof(OID_GENERIC_MAC_REQUEST) );
	if (sIoctl == NULL)
	{
		mt_print(2, "No memory when reading data...\n" );
		return 1;
	}

	memset( sIoctl, 0, sizeof( OID_GENERIC_MAC_REQUEST ) );

	sIoctl->opcode = parameters[0];
	mt_print(2, "opcode = %d\n",sIoctl->opcode );
	if (numOfParams > 3)
	{
		sIoctl->size   = parameters[1];
		sIoctl->action = parameters[2];
		mt_print(2, "size = %d\n",sIoctl->size );
		mt_print(2, "action = %d\n",sIoctl->action );
		/* There is use of the optional fields */
		switch (numOfParams)
		{
		case 5:
			sIoctl->res0 = parameters[3];
			mt_print(2, "res0 = %d\n",sIoctl->res0 );
			break;
		case 6:
			sIoctl->res0 = parameters[3];
			sIoctl->res1 = parameters[4];
			mt_print(2, "res0 = %d, res1 = %d\n",sIoctl->res0,sIoctl->res1 );
			break;
		case 7:
			sIoctl->res0 = parameters[3];
			sIoctl->res1 = parameters[4];
			sIoctl->res2 = parameters[5];
			mt_print(2, "res0 = %d, res1 = %d, res2 = %d\n",sIoctl->res0,sIoctl->res1, sIoctl->res2 );
			break;
		}
	}
	else if (numOfParams > 1)
	{
		sIoctl->size   = parameters[1];
		sIoctl->action = parameters[2];
		mt_print(2, "size = %d, action = %d\n",sIoctl->size,sIoctl->action );
	}
	if ((sIoctl->action < 2) && (parameters[numOfParams-1] < MT_BCL_ITEM_LISTS_NUM))
	{
		pItem = (MT_UINT32 *)MT_BCL_item_list[parameters[numOfParams-1]];
		mt_print(2, "item = %d\n",parameters[numOfParams-1] );
	}

	if (sIoctl->action == MT_REQUEST_SET)
	{
		memcpy( &sIoctl->data, pItem, 256  ); /*Set all*/
	}

	retVal = HypGenericMacRequest(sIoctl);

	if (sIoctl->retStatus == 0)
		memcpy( pItem, &sIoctl->data, 256 ); /*Get all*/
	else
	{
		mt_print(2, "Error, retStatus = %x\n",sIoctl->retStatus );
		retVal = 1;
	}

	free( sIoctl );

	return retVal;
}

MT_UINT32 DebugTable(MT_UINT32 Id, MT_UINT32 action, MT_UINT32 value, MT_UINT32 size, MT_UINT32 offset, MT_BYTE * data)
{
    error("DebugTable - not supported under windows");
    return 0;
}

MT_UINT32 DrvCatInit(MT_UINT32 category, MT_UINT32 * cnt) {return HypBclStatCatInit(category,cnt);}
MT_UINT32 DrvCatFree(MT_UINT32 category)  {return HypBclStatCatFree(category);}
MT_UINT32 DrvNameGet(MT_UINT32 category, MT_UINT32 index, MT_UBYTE * name) {return HypBclStatGetName(category,index,name);}
MT_UINT32 DrvValGet(MT_UINT32 category, MT_UINT32 index, MT_UINT32 * val) {return HypBclStatGetVal(category,index,val);}
MT_UINT32 DrvValSet(MT_UINT32 category, MT_UINT32 index, MT_UINT32 val) {return HypBclStatSetVal(category,index,val);}
