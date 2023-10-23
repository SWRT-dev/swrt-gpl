/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

//$Id: Debug.h 5783 2009-01-15 15:11:27Z antonn $
#ifndef _DEBUG_H_INCLUDED_
#define _DEBUG_H_INCLUDED_

#if defined(UTR_DEBUG)

#if defined(WIN32)

void _set_debug_text_color();
void _set_normal_text_color();

#define DBG_TRACE(msg) { \
                            _set_debug_text_color(); \
                            cerr << msg << endl; \
                            _set_normal_text_color(); \
                       }

#elif defined(LINUX)

#define DBG_TRACE(msg) cerr << "\033[22;32m" << msg << "\033[0m" << endl;

#else
#   error Unknown operating system
#endif


#else //UTR_DEBUG

#define DBG_TRACE(msg)

#endif //UTR_DEBUG

#endif //_DEBUG_H_INCLUDED_
