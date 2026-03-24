/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "ParamInfo.h"

#include <sstream>

string CHelpScreen::GetHelp(void) const
{
    stringstream strm;

    for(vector<ParamInfo>::const_iterator it = m_Params.begin();  it != m_Params.end(); ++it)
    {
        strm << "[-" << it->Name.first << "|--" 
             << it->Name.second << "]";
         if(0 != it->ValueTemplate[0])
             strm << "<" << it->ValueTemplate << ">";
        strm << " ";
    }

    strm << endl << endl << "Supported command line parameters:" << endl;

    //Output detailed params description
    for(vector<ParamInfo>::const_iterator it = m_Params.begin();  it != m_Params.end(); ++it)
    {
        strm << "  " << "[-" << it->Name.first << "|--" 
             << it->Name.second << "]" << endl << "    " 
             << it->Description << endl;
    }

    return strm.str();
}

