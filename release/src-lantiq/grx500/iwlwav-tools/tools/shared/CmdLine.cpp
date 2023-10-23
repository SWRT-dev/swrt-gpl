/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//$Id: CmdLine.cpp 5808 2009-01-18 13:37:27Z antonn $
#include "mtlkinc.h"
#include "CmdLine.h"
#include "aux_utils.h"

//#define LOCAL_DBG_TRACE(x) DBG_TRACE("CmdLine: " << x) 
#define LOCAL_DBG_TRACE(x)

const char CCmdLine::CMD_LINE_DELIMITER = '-';

CCmdLine::CCmdLine(int argc, char* const argv[])
    : m_argc(argc)
    , m_argv(argv)
{
	LOCAL_DBG_TRACE("CCmdLine object constructed, argc is " << m_argc << "...");
}

char* CCmdLine::isCmdLineParamInternal(const string& paramName)
{
	LOCAL_DBG_TRACE("Looking for param \"" << paramName << "\"...");

    for(int i = 1; i < m_argc; i++)
    {
    	LOCAL_DBG_TRACE("\tChecking command line param \"" << m_argv[i] << "\"...");
        const string arg(m_argv[i]);
        unsigned char nDelimitersLength;
        if( arg[0] == CMD_LINE_DELIMITER )
        {
            if ((arg.length() > 1) && (CMD_LINE_DELIMITER == arg[1]))
            {
                //Parameter name started with --, 
                //so we expect long parameter name
                //do not compare it if short 
                //parameter name provided
                if(paramName.length() == 1)
                {
                	LOCAL_DBG_TRACE("\tThis is a long param, while we are looking for short param.");
                    continue;
                }

                nDelimitersLength = 2;
            }
            else 
                nDelimitersLength = 1;

            if(arg.length() >= nDelimitersLength + paramName.length())
            {
                const string strArgName(arg.substr(nDelimitersLength, paramName.length()));
            	LOCAL_DBG_TRACE("\tArgument name is \"" << strArgName << "\"");
                
                if( strToUpper(strArgName) == strToUpper(paramName) )
                {
                	char* szParamValue = &m_argv[i][nDelimitersLength + paramName.length()]; 
                    
                    if((0 == szParamValue[0]) && (i < m_argc - 1) && 
                       (CMD_LINE_DELIMITER != m_argv[i+1][0]))
                    {
                        szParamValue = m_argv[i+1];
                    }
                	LOCAL_DBG_TRACE("\tArgument value is \"" << szParamValue << "\"");
                    return szParamValue;
                }
            }
        }
    }
    return NULL;
}

bool CCmdLine::isCmdLineParam(const CCmdLine::ParamName& paramName)
{
    return (NULL != isCmdLineParamInternal( paramName.first))
        || (NULL != isCmdLineParamInternal( paramName.second));
}

string CCmdLine::getParamValue(const CCmdLine::ParamName& paramName)
{
    char* szResult = isCmdLineParamInternal(paramName.first);
    if(NULL == szResult)
        szResult = isCmdLineParamInternal(paramName.second);

    return (NULL == szResult) ? string() : string(szResult);
}

int CCmdLine::getIntParamValue(const CCmdLine::ParamName& paramName)
{
    string strValue = getParamValue(paramName);

    return strValue.empty() ? 0 : stringToint(strValue);
}
