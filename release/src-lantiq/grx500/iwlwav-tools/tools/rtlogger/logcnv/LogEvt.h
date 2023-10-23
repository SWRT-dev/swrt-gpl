/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __LOGEVT_H__
#define __LOGEVT_H__

#include <string>
#include <istream>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

#include "LogFmtDB.h"
#include "aux_utils.h"

class CLogEvt
{
  friend istream &operator>> (istream &is, CLogEvt &evt);

  class exc_bad_evt_hdr : public exc_basic
  {
  public:
    exc_bad_evt_hdr (uint32 info) {
      ostringstream ss;
      ss << "Incorrect LOG event header info:" << hex << showbase << setfill('0') << setw(8) << info ;
      m_str = ss.str();
    }
  };

  class exc_bad_data_type : public exc_basic
  {
  public:
    exc_bad_data_type (uint8 dtype) {
      ostringstream ss;
      ss << "Unknown LOG event data type: " << dtype;
      m_str = ss.str();
    }
  };

public:
  CLogEvt();
  virtual ~CLogEvt() {;}

  string   GetSrcString(const CLogFmtDB &fmt_db) const;
  string   GetDstString(const CLogFmtDB &fmt_db) const;
  string   GetMsgString(const CLogFmtDB &fmt_db) const;
  uint32   GetTS(void) const {
    return m_ts;
  }
  uint8    GetOID(void) const {
    return m_oid;
  }
  uint8    GetGID(void) const {
    return m_gid;
  }
  uint16   GetFID(void) const {
    return m_fid;
  }
  uint16   GetLID(void) const {
    return m_lid;
  }
  uint8    GetWLANIF(void) const {
    return m_wlanif;
  }
  bool    HasData(void) const {
    return m_has_data;
  }

protected:
  void     Read(istream &in_s);

  bool                 m_has_data;
  uint32               m_ts;
  uint8                m_oid;
  uint8                m_gid;
  uint16               m_fid;
  uint16               m_lid;
  uint8                m_wlanif;
  bool                 m_reversed;
  
  mutable vector<int8> m_buffer;
};

istream &operator>> (istream &is, CLogEvt &evt);

class CLogEvtFmt
{
  class exc_bad_format : public exc_basic
  {
  public:
    exc_bad_format (string &fmt, size_t offs, uint32 param) {
      ostringstream ss;
      ss << "Invalid format string= '" << fmt << "' offset=" << offs <<" param=" << param;
      m_str = ss.str();
    }
    exc_bad_format (string &fmt, size_t offs, const char *str) {
      ostringstream ss;
      ss << "Invalid format string= '" << fmt << "' offset=" << offs <<" param='" << str << "'";
      m_str = ss.str();
    }
    exc_bad_format (string &fmt, size_t offs, const void *) {
      ostringstream ss;
      ss << "Invalid format string= '" << fmt << "' offset=" << offs <<" param is binary data";
      m_str = ss.str();
    }
  };
public:
  CLogEvtFmt(const CLogEvt &evt)
    : m_evt(evt)
    , m_in_progress(false)
    , m_offset(0)
  { }

  ~CLogEvtFmt() 
  { }
  
  void BeginFormat(const CLogFmtDB &fmt_db);
  template<typename T> 
    void PutParam(const T& val);
  void EndFormat(void);
  
  string &GetFormatResult(void) {
    return m_res;
  }

protected:
  bool TryProcessOwnExtension(const string& val_format, const int8 &val);
  bool TryProcessOwnExtension(const string& val_format, const int32 &val);
  bool TryProcessOwnExtension(const string& val_format, const int64 &val);
  bool TryProcessOwnExtension(const string& val_format, const void *val);
 
  const CLogEvt &m_evt;
  string         m_format;
  bool           m_in_progress;
  size_t         m_offset;
  string         m_res;
};

#endif // __LOGEVT_H__

