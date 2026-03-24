/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _PARAMINFO_H_INCLUDED_
#define _PARAMINFO_H_INCLUDED_

#if defined(WIN32)
#pragma warning(push,3)
#endif

#include <string>
#include <vector>

#if defined(WIN32)
#pragma warning(pop)
#endif

using namespace std;

#include "CmdLine.h"

class ParamInfo
{
public:
    ParamInfo(const CCmdLine::ParamName& paramName, 
              const char* paramDescr, const char* valueTemplate = "")
              : Name(paramName)
              , Description(paramDescr)
              , ValueTemplate(valueTemplate)
    {}
    
    operator const CCmdLine::ParamName&() const { return Name; }
    operator const CCmdLine::ParamName&() { return Name; }
    CCmdLine::ParamName Name;
    string              Description;
    string              ValueTemplate;
};

class CHelpScreen
{
public:
    void   AddParam(const ParamInfo &Param)
    {
      m_Params.push_back(Param);
    }
    string GetHelp(void) const;
protected:
    vector<ParamInfo> m_Params;
};

#endif /* _PARAMINFO_H_INCLUDED_ */

