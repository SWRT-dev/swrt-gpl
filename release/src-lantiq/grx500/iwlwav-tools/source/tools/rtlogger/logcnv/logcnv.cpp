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
#include <iostream>

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#include <winsock2.h>
#endif

#include "CmdLine.h"
#include "ParamInfo.h"
#include "Debug.h"
#include "aux_utils.h"

#include "LogInfo.h"
#include "LogEvt.h"
#include "LogFmtDB.h"

#include "pcapdefs.h"
#include <stdexcept>
#include <fstream>

using namespace std;

#define MTLK_LOGGER_NETWORK  147 /* Corresponds to wtap 45 - WTAP_ENCAP_USER0 */
#define MTLK_LOGGER_SNAPLEN  65535

#define SCD_FILES_DELIM      ","

static const pcap_hdr_t log_pcap_hdr = 
{
  PCAP_MAGIC,
  PCAP_VERSION_MAJOR,
  PCAP_VERSION_MINOR,
  0,
  0,
  MTLK_LOGGER_SNAPLEN,
  MTLK_LOGGER_NETWORK
};

#pragma pack(push,1)
typedef struct {
  uint8  oid;
  uint8  gid;
  uint16 fid;
  uint16 lid;
  uint8  wlanif;
  /* Followed by:
  uint16 src_len;
  char   src_str[];
  uint16 dst_len;
  char   dst_str[];
  uint16 msg_len;
  char   msg_str[];
  */
} mtlklog_hdr_t;
#pragma pack(pop)

#if 1
#define LOCAL_DBG_TRACE(x) DBG_TRACE("logcnv: " << x) 
#else
#define LOCAL_DBG_TRACE(x)
#endif

static const ParamInfo paramShowHelp(CCmdLine::ParamName("h", "help"),
                                     "Show this help.");
static const ParamInfo paramDebugLevel(CCmdLine::ParamName("d", "debug"),
                                     "Application debug level (for debugging purposes only)",
                                     "dlevel");
static const ParamInfo paramStringFiles(CCmdLine::ParamName("s", "str-file"),
                                        "Compilation extracted string-file (.scd)",
                                        "file1[,file2[,...]]");
static const ParamInfo paramPCapOut(CCmdLine::ParamName("p", "pcap"),
                                        "Output in PCAP format");
static const ParamInfo paramInFile(CCmdLine::ParamName("i", "input"),
                                   "Log file to read (default: stdin)",
                                   "file");
static const ParamInfo paramOutFile(CCmdLine::ParamName("o", "output"),
                                    "Output file (default: stdout)",
                                    "file");

#define stream_enable_exceptions(s) (s).exceptions(ios::badbit | ios::failbit)

static void
ShowUsage (void)
{
  static const ParamInfo* AllParams[] = {&paramShowHelp, 
                                         &paramDebugLevel, 
                                         &paramStringFiles,
                                         &paramPCapOut,
                                         &paramInFile,
                                         &paramOutFile};

  CHelpScreen HelpScreen;

  cerr << "Metalink Log Convertor Utility v." MTLK_SOURCE_VERSION 
       << endl << endl
       << "Usage:" << endl << "\tlogcnv ";

  for (size_t i = 0 ; i < ARRAY_SIZE(AllParams); i++) {
    HelpScreen.AddParam(*AllParams[i]);
  }

  cerr << HelpScreen.GetHelp();
}

static void
WriteString (string& str, ostream &out_s)
{
  uint16 size = str.length() + 1;
  uint16 sz_h = HOST_TO_NET16(size);
  out_s.write((const char *)&sz_h, sizeof(sz_h));
  out_s.write(str.c_str(), size);
}

static void
ProcessLog (istream &in_s, ostream &out_s, vector<string> &scd_files, bool pcap_out)
{
  CLogInfo  log_info;
  CLogFmtDB fmt_db;
  pcaprec_hdr_t pcaprec_hdr;
  mtlklog_hdr_t mtlklog_hdr;

  in_s >> log_info;

  for (vector<string>::iterator it = scd_files.begin(); it != scd_files.end(); ++it) {
    fmt_db.Read(*it);
  }

  /* Put the PCAP file header */
  if (pcap_out) {
    out_s.write((const char *)&log_pcap_hdr, sizeof(log_pcap_hdr));
  }

  while (!in_s.eof()) {
    CLogEvt log_evt;

    in_s >> log_evt;
    if (log_evt.HasData()) {
      if (!pcap_out) {
        out_s << "[" << setw(10) << setfill('0') << log_evt.GetTS() << setfill(' ') << setw(0) << "] " << log_evt.GetMsgString(fmt_db) << "'" << endl;
      }
      else {
        string src = log_evt.GetSrcString(fmt_db);
        string dst = log_evt.GetDstString(fmt_db);
        string msg = log_evt.GetMsgString(fmt_db);

        pcaprec_hdr.ts_sec = log_evt.GetTS()/1000;
        pcaprec_hdr.ts_usec = (log_evt.GetTS()%1000) * 1000;
        pcaprec_hdr.incl_len = 
          pcaprec_hdr.orig_len = sizeof(mtlklog_hdr_t) + 
          sizeof(uint16) + src.length() + 1 + 
          sizeof(uint16) + dst.length() + 1 + 
          sizeof(uint16) + msg.length() + 1;
        out_s.write((const char *)&pcaprec_hdr, sizeof(pcaprec_hdr_t));
        mtlklog_hdr.oid = log_evt.GetOID();
        mtlklog_hdr.gid = log_evt.GetGID();
        mtlklog_hdr.fid = HOST_TO_NET16(log_evt.GetFID());
        mtlklog_hdr.lid = HOST_TO_NET16(log_evt.GetLID());
        mtlklog_hdr.wlanif = log_evt.GetWLANIF();
        out_s.write((const char *)&mtlklog_hdr, sizeof(mtlklog_hdr));
        WriteString(src, out_s);
        WriteString(dst, out_s);
        WriteString(msg, out_s);
      }
    }
  }
}

enum MAIN_RETVALS
{
    MAIN_SUCCESS        = 0,
    MAIN_KNOWN_ERROR    = -1,
    MAIN_UNKNOWN_ERROR  = -2
};

#ifdef WIN32
int __cdecl main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
  try {
    CCmdLine cmdLine(argc, argv);
    string strStringFiles = cmdLine.getParamValue(paramStringFiles);

    if (cmdLine.isCmdLineParam(paramShowHelp)) {
      LOCAL_DBG_TRACE("Entering help mode...");
      ShowUsage();
    }
    
    vector<string> scd_files;
    CStrTokenizer  tok(strStringFiles);
    for(CStrTokenizer::token_itr t = tok.begin(SCD_FILES_DELIM); t; ++t)  {
      LOCAL_DBG_TRACE("\tTokenizer returned path \"" << t.get() << "\"");
      
      if(t.get().empty())
        continue;

      scd_files.push_back(t.get());
    }

    if (scd_files.empty()) {
      ShowUsage();
      throw logic_error("At least one string-file (.scd) must be specified!");
    }

    istream *in_s  = &cin;
    ostream *out_s = &cout;
    ifstream in_f;
    ofstream out_f;
    string   fName;
    bool     pcap_out = cmdLine.isCmdLineParam(paramPCapOut);

    fName = cmdLine.getParamValue(paramInFile);
    if (!fName.empty()) {
      stream_enable_exceptions(in_f);
      in_f.open(fName.c_str(), ios::in | ios::binary);
      in_s = &in_f;
    }
    else {
      stream_enable_exceptions(cin);
#ifdef WIN32
      /* Without _setmode() Windows replaces end-of-lines and breaks the data */
      if (_setmode(_fileno(stdin), _O_BINARY) == -1) {
        throw logic_error("Can't set STDIN to BINARY mode!");
      }
#endif
    }

    fName = cmdLine.getParamValue(paramOutFile);
    if (!fName.empty()) {
      stream_enable_exceptions(out_f);
      out_f.open(fName.c_str(), pcap_out?(ios::out | ios::binary):ios::out);
      out_s = &out_f;
    }
    else {
      stream_enable_exceptions(cout);
#ifdef WIN32
      /* Without _setmode() Windows replaces end-of-lines and breaks the data */
      if (_setmode(_fileno(stdout), _O_BINARY) == -1) {
        throw logic_error("Can't set STDOUT to BINARY mode!");
      }
#endif
    }

    ProcessLog(*in_s, *out_s, scd_files, pcap_out);
  }
  catch (const exception& ex) {
    cerr << "Error occurred:" << endl << "\t" 
         << ex.what() << endl;
    return MAIN_KNOWN_ERROR;
  }
  catch(...) {
    cerr << "Unknown fatal error occurred." << endl;
    return MAIN_UNKNOWN_ERROR;
  }

  return MAIN_SUCCESS;
}
