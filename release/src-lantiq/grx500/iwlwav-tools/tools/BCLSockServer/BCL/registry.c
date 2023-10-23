/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//
// Registry manipulation routines
//

#include "mtlkinc.h"
#include "Registry.h"
#include "mt_util.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

HKEY g_hRoot;

LONG Registry_Open(LPCTSTR lpRoot)
{
    LONG lError;
    g_hRoot = NULL;
#ifdef UNICODE
    mt_print(2, "Registry Leaf: HKLM\\%S\n", lpRoot);
#else
    mt_print(2, "Registry Leaf: HKLM\\%s\n", lpRoot);
#endif
    lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                          lpRoot,
                          0,
                          KEY_ALL_ACCESS,
                          &g_hRoot);

    return (lError == ERROR_SUCCESS);
}

VOID Registry_Close()
{
    RegCloseKey(g_hRoot);
}

#ifndef UNDER_CE // Driver index is included into driver's guid

INT Registry_FindDriverIndex(LPCTSTR lpGuid)
{
    DWORD dwType;
    TCHAR lpValueName[255];
    LONG lError;
    DWORD i;
    BOOL bContinue = TRUE;
    FILETIME sLastWriteTime;
    DWORD dwValueSize;

    if (g_hRoot == NULL) 
        return -1;

    for (i = 0; bContinue; i++)
    {
        dwValueSize = MAX_PATH;
        // Get next key
        lError = RegEnumKeyEx(g_hRoot,
                            i,
                            lpValueName,
                            &dwValueSize,
                            NULL,
                            NULL,
                            NULL,
                            &sLastWriteTime);

        bContinue = (lError == ERROR_SUCCESS);

        if ( ! ((lError == ERROR_SUCCESS) || (lError == ERROR_MORE_DATA)))
        {
            if (lError != ERROR_NO_MORE_ITEMS)
                mt_print(2, "Error %ld in enum value %s\n", lError, lpValueName);
            break;
        }

        // Inside the key look for NetCfgInstanceId value and compare with given guid
        {
            HKEY hNewRoot = NULL;
            lError = RegOpenKeyEx(g_hRoot, lpValueName, 0, KEY_ALL_ACCESS, &hNewRoot);

            if (lError == ERROR_SUCCESS)
            {
                DWORD dwLength = MAX_PATH;

                if (RegQueryValueEx(hNewRoot, TEXT("NetCfgInstanceId"), 0, &dwType, (LPBYTE)lpValueName, &dwLength) == ERROR_SUCCESS)
                {
                    if (_tcscmp(lpValueName, lpGuid) == 0)
                    {
                        g_hRoot = hNewRoot;
                        return i;                        
                    }
                }

                RegCloseKey(hNewRoot);
            }
        }
    }

    return -1;
}

#endif // !UNDER_CE


LONG Registry_SetKey(LPCTSTR lpKey, LPCTSTR lpValue)
{
    LONG lError = NO_ERROR;

    lError = RegSetValueEx(g_hRoot,
                           lpKey,
                           0,
                           REG_SZ,
                           (LPBYTE)lpValue,
                           _tcslen(lpValue));

    if (lError != ERROR_SUCCESS)
    {
        mt_print(2,"Error in set value %s\n",lpKey);
        return FALSE;
    }

    RegFlushKey(g_hRoot);

    return TRUE;
}

LONG Registry_GetKey(LPCTSTR lpKey, LPCTSTR lpValue, INT lpBufferLength) 
{
    LONG lError = NO_ERROR;
    DWORD dwType;
    DWORD length = lpBufferLength;

    if (g_hRoot == NULL)
        return FALSE;

    lError = RegQueryValueEx(g_hRoot,
                             lpKey,
                             NULL,
                             &dwType,
                             (LPBYTE)lpValue,
                             &length);

    return lError == ERROR_SUCCESS;
}
