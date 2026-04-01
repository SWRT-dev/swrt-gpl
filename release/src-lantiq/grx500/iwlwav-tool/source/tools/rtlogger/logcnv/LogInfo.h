/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __LOG_INFO_H__
#define __LOG_INFO_H__

#include <string>
#include <istream>
#include <sstream>
#include <iomanip>

#include "logdefs.h"

using namespace std;

#include "aux_utils.h"
#include "logsrv_protocol.h"

class CLogInfo
{
  friend istream &operator>> (istream &is, CLogInfo &info);

  class exc_bad_info_magic : public exc_basic
  {
  public:
    exc_bad_info_magic (uint32 magic) {
      ostringstream ss;
      ss << "Incorrect Log Info magic:" << hex << showbase << setfill('0') << setw(8) << magic ;
      m_str = ss.str();
    }
  };
  class exc_bad_info_size : public exc_basic
  {
  public:
    exc_bad_info_size (uint32 size) {
      ostringstream ss;
      ss << "Incorrect Log Info size:" << hex << showbase << setfill('0') << setw(8) << size ;
      m_str = ss.str();
    }
  };
  class exc_bad_info_ver_major : public exc_basic
  {
  public:
    exc_bad_info_ver_major (uint16 major, uint16 supported) {
      ostringstream ss;
      ss << "Incorrect Log Info major: " << major << " (required: " << supported <<")";
      m_str = ss.str();
    }
  };

public:
  CLogInfo(void)
  { 
    memset(&m_info, 0, sizeof(m_info));
  }

  ~CLogInfo() 
  { }

protected:
  void Read (istream &in_s) {
    try {
      in_s.read((char *)&m_info, sizeof(m_info));
    }
    catch (const ios_base::failure &/*ex*/) {
      if (in_s.eof()) {
        /* EOF is not an error in our case */
        return;
      }
      /* Other stream error */
      throw; /* throw this exception up */
    }

    m_info.magic = ntohl(m_info.magic);
    if (m_info.magic != LOGSRV_INFO_MAGIC) {
      throw exc_bad_info_magic(m_info.magic);
    }

    m_info.size = ntohl(m_info.size);
    if (m_info.size != sizeof(m_info)) {
      throw exc_bad_info_size(m_info.size);
    }

    m_info.log_ver_major = ntohs(m_info.log_ver_major);
    if (m_info.log_ver_major != RTLOGGER_VER_MAJOR) {
      throw exc_bad_info_ver_major(m_info.log_ver_major, RTLOGGER_VER_MAJOR);
    }

    m_info.log_ver_minor = ntohs(m_info.log_ver_minor);
  }

  struct logsrv_info m_info;
};

istream &operator>> (istream &is, CLogInfo &info)
{
  info.Read(is);
  return is;
}

#endif /* __LOG_INFO_H__ */
