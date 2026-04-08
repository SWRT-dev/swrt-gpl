/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __LOGFMTDB_H__
#define __LOGFMTDB_H__

#include <map>
#include <string>

using namespace std;

#include "aux_utils.h"

class CLogFmtDB
{
  class bad_scd_file : public exc_basic
  {
  public:
    bad_scd_file(char record_type) {
      ostringstream ss;
      ss << "Unknown record type: " << 
        record_type;
       m_str = ss.str();
    }
    bad_scd_file(uint8 oid, const char *str) {
      ostringstream ss;
      ss << "Originator already exists: " << 
        (int) oid << ":'" << str << "'";
       m_str = ss.str();
    }
    bad_scd_file(uint8 oid, uint8 gid, const char *str) {
      ostringstream ss;
      ss << "Group already exists: " << 
        (int) oid << ":" << (int) gid << ":'" << str << "'";
       m_str = ss.str();
    }
    bad_scd_file(uint8 oid, uint8 gid, uint16 fid, uint16 lid, const char *str) {
      ostringstream ss;
      ss << "Format already exists: " << 
        (int) oid << ":" << (int) gid << ":" << (int) fid << ":" << (int) lid << ":'" << str << "'";
       m_str = ss.str();
    }
  };
public:
  CLogFmtDB() { }
  virtual ~CLogFmtDB() { }
  void         Reset(void) {
    fmts.clear();
  }
  void Read(string &fname);
  void GetFormat(uint8 oid, uint8 gid, uint16 fid, uint16 lid, string &res) const;
  void GetOrgName(uint8 oid, string &res) const;
  void GetGrpName(uint8 oid, uint8 gid, string &res) const;

protected:
  map<uint8, string>  orgs;
  map<uint16, string> grps;
  map<uint64, string> fmts;
};

#endif // __LOGFMTDB_H__
