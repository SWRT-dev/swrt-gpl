/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

