/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//$Id: Debug.cpp 5808 2009-01-18 13:37:27Z antonn $

#include "mtlkinc.h"
#include "Debug.h"

#if defined(WIN32)
#include <Windows.h>
#include <iostream>

using namespace std;

static WORD g_wOriginalTextAttributes 
    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

void _set_debug_text_color()
{
    HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    if(INVALID_HANDLE_VALUE == hConsole)
    {
        cerr << "_set_debug_text_color: failed to obtain stderr handle" << endl;
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO ConsoleInfo;
    if(!GetConsoleScreenBufferInfo(hConsole, &ConsoleInfo))
    {
        cerr << "_set_debug_text_color: failed to get debug test color" << endl;
        ConsoleInfo.wAttributes;
        return;
    }

    g_wOriginalTextAttributes = ConsoleInfo.wAttributes;

    if(!SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN))
    {
        cerr << "_set_debug_text_color: failed to set debug text color" << endl;
        return;
    }
}

void _set_normal_text_color()
{
    HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    if(INVALID_HANDLE_VALUE == hConsole)
    {
        cerr << "_set_normal_text_color: failed to obtain stderr handle" << endl;
        return;
    }

    if(!SetConsoleTextAttribute(hConsole, g_wOriginalTextAttributes))
    {
        cerr << "_set_normal_text_color: failed to set normal text color" << endl;
        return;
    }
}

#endif //WIN32
