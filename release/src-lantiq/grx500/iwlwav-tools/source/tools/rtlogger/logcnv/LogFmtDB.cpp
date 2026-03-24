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
#include "LogFmtDB.h"

#include <fstream>

using namespace std;

#define MAKE_ORGS_KEY(oid)           ((uint8)oid)
#define MAKE_GRPS_KEY(oid, gid)      ((((uint16)(uint8)(oid)) << 8) | ((uint16)(uint8)(gid)))
#define MAKE_FMTS_KEY(oid, gid, fid, lid) ((((uint64)(uint8)(oid)) << 40)  | \
                                           (((uint64)(uint8)(gid)) << 32)  | \
                                           (((uint64)(uint16)(fid)) << 16) | \
                                           ((uint64)(uint16)(lid)))

string
replace_all(const string &source, const string &victim, const string &replacement)
{
  string answer = source;
  string::size_type j = 0;

  while ((j = answer.find(victim, j)) != std::string::npos )
    answer.replace(j, victim.length(), replacement);
  return answer;
}

void 
CLogFmtDB::Read (string &fname)
{
  ifstream ifs;

  ifs.exceptions(ifstream::badbit); /* An error happened. */

  ifs.open(fname.c_str());

  while (!ifs.eof()) {
    char   record_type;
    uint8  oid;
    uint8  gid;
    uint16 fid;
    uint16 lid;
    char   buf[512];
    uint32 val;
    bool   fmt_read;
    string fmt_str;

    ifs >> record_type;

    switch (record_type) {
    case 'O':
      ifs >> val;
      oid = (uint8)val;
      ifs.getline(buf, sizeof(buf));
      if (!ifs.fail()) { /* if all the readings were OK */
        if (!orgs.insert(pair<uint8, string> (MAKE_ORGS_KEY(oid), buf)).second) {
          throw bad_scd_file(oid, buf);
        }
      }
      break;
    case 'G':
      ifs >> val;
      oid = (uint8)val;
      ifs >> val;
      gid = (uint8)val;
      ifs.getline(buf, sizeof(buf));

      if (!ifs.fail()) { /* if all the readings were OK */
        if (!grps.insert(pair<uint16, string> (MAKE_GRPS_KEY(oid, gid), buf)).second) {
          throw bad_scd_file(oid, gid, buf);
        }
      }
      break;
    case 'S': 
      ifs >> val;
      oid = (uint8)val;
      ifs >> val;
      gid = (uint8)val;
      ifs >> val;
      fid = (uint16)val;
      ifs >> val;
      lid = (uint16)val;

      /* read all strings before next record - we may have multiline format strings */
      fmt_read = false;
      fmt_str = "";
      while (!fmt_read) {
        ifs.getline(buf, sizeof(buf));
        if (ifs.fail())
          break;
        fmt_str.append(buf);
        /* check if we are done: logprep inserts spaces for all multiline formats to align them */
        ifs >> record_type;
        ifs.unget();
        if (record_type != '\"') {
          fmt_read = true;
          break;
        }
      }
      if (fmt_read) {
        fmt_str = replace_all(fmt_str, "\"\"", "");
        if (!fmts.insert(pair<uint64, string> (MAKE_FMTS_KEY(oid, gid, fid, lid), fmt_str.c_str())).second) {
          throw bad_scd_file(oid, gid, fid, lid, buf);
        }
      }
      break;
    default:
      throw bad_scd_file(record_type);
      break;
    }
  }
}

void
CLogFmtDB::GetFormat (uint8 oid, uint8 gid, uint16 fid, uint16 lid, string &res) const
{
  map<uint64, string>::const_iterator it = fmts.find(MAKE_FMTS_KEY(oid, gid, fid, lid));

  if (it != fmts.end()) {
    res = it->second;
  }
  else {
    res.clear();
  }
}

void
CLogFmtDB::GetOrgName (uint8 oid, string &res) const
{
  map<uint8, string>::const_iterator it = orgs.find(MAKE_ORGS_KEY(oid));

  if (it != orgs.end()) {
    res = it->second;
  }
  else {
    res.clear();
  }
}

void
CLogFmtDB::GetGrpName (uint8 oid, uint8 gid, string &res) const
{
  map<uint16, string>::const_iterator it = grps.find(MAKE_GRPS_KEY(oid, gid));

  if (it != grps.end()) {
    res = it->second;
  }
  else {
    res.clear();
  }
}
