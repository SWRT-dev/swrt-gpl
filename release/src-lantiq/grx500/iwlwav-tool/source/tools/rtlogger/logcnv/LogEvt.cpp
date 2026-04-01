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
#include "formats.h"
#include "LogEvt.h"
#include "Debug.h"

#include "logdefs.h"

#define REVERSE8(x)        (x)
#define REVERSE16(x)       ( (uint16) ( ((x) & 0xFFFF) >> 8 ) | ( ((x) << 8) & 0xFFFF ) )
#define REVERSE32(x)       ( (uint32) ( \
                                       ( ((x) & 0xFF000000) >> 24 ) |   \
                                       ( ((x) & 0x00FF0000) >> 8  ) |   \
                                       ( ((x) & 0x0000FF00) << 8  ) |   \
                                       ( ((x) & 0x000000FF) << 24 )     \
                                      )                                 \
                           )
#define REVERSE64(x)       ( (uint64) ( \
                                       ( ((x) & 0xFF00000000000000ULL) >> 56 ) |   \
                                       ( ((x) & 0x00FF000000000000ULL) >> 40 ) |   \
                                       ( ((x) & 0x0000FF0000000000ULL) >> 24 ) |   \
                                       ( ((x) & 0x000000FF00000000ULL) >> 8  ) |   \
                                       ( ((x) & 0x00000000FF000000ULL) << 8  ) |   \
                                       ( ((x) & 0x0000000000FF0000ULL) << 24 ) |   \
                                       ( ((x) & 0x000000000000FF00ULL) << 40 ) |   \
                                       ( ((x) & 0x00000000000000FFULL) << 56 )     \
                                      ) \
                           )

#define GetBuf() ((char *)&m_buffer[0])
 
CLogEvt::CLogEvt ()
  : m_has_data(false),
    m_ts(0),
    m_oid(0),
    m_gid(0),
    m_fid(0),
    m_lid(0),
    m_wlanif(0),
    m_reversed(false)
{

}

string
CLogEvt::GetSrcString (const CLogFmtDB &fmt_db) const
{
  string res;
  fmt_db.GetOrgName(m_oid, res);
  return res;
}


string
CLogEvt::GetDstString (const CLogFmtDB &fmt_db) const
{
  string res;
  fmt_db.GetGrpName(m_oid, m_gid, res);
  return res;
}

string
CLogEvt::GetMsgString (const CLogFmtDB &fmt_db) const
{
  CLogEvtFmt evt_fmt(*this);
  char      *buf = NULL;
  char      *p   = NULL;

  evt_fmt.BeginFormat(fmt_db);

  if (!m_buffer.size()) {
    goto end_format;
  }

  buf = (char *)&m_buffer[0];
  p   = buf;

  /* Parse log event data */
  while (p - buf < (int)m_buffer.size()) {
    mtlk_log_event_data_t *evt_data = (mtlk_log_event_data_t *)p;

    /* Skip log event data header */
    p += sizeof(*evt_data);

    switch (evt_data->datatype) {
    case LOG_DT_INT32:
      {
        /* Extract value */
        int32 val = *((int32 *)p);
        
        /* Reverse value if required*/
        if (m_reversed) {
          val = REVERSE32(val);
        }

        /* USE Value here */
        evt_fmt.PutParam(val);
        
        /* Skip the extracted value */
        p += sizeof(uint32);
      }
      break;
    case LOG_DT_INT64:
      {
        /* Extract value */
        int64 val = *((int64 *)p);
        
        /* Reverse value if required*/
        if (m_reversed) {
          val = REVERSE64(val);
        }

        /* USE Value here */
        evt_fmt.PutParam(val);
        
        /* Skip the extracted value */
        p += sizeof(uint64);
      }
      break;
    case LOG_DT_MACADDR:
      {
        /* Extract value */
        const void* val = (void*) p;
        
        /* USE Value here */
        evt_fmt.PutParam(val);
        
        /* Skip the extracted value */
        p += MAC_ADDR_LENGTH;
      }
      break;
    case LOG_DT_IP6ADDR:
      {
        /* Extract value */
        const void* val = (void*) p;
        
        /* USE Value here */
        evt_fmt.PutParam(val);
        
        /* Skip the extracted value */
        p += IP6_ADDR_LENGTH;
      }
      break;
    case LOG_DT_INT8:
      {
        /* Extract value */
        int8 val = *((int8 *)p);
        
        /* Reverse value if required*/
        if (m_reversed) {
          val = REVERSE8(val);
        }

        /* USE Value here */
        evt_fmt.PutParam(val);
        
        /* Skip the extracted value */
        p += sizeof(uint8);
      }
      break;
    case LOG_DT_LSTRING:
      {
        mtlk_log_lstring_t *log_str = (mtlk_log_lstring_t *)(p);
        
        /* Reverse log string header if required*/
        if (m_reversed) {
          log_str->len = REVERSE16(log_str->len);
        }
        
        /* Skip log string data header */
        p += sizeof(*log_str);
        
        /* USE String here */
        evt_fmt.PutParam(p);

        /* Skip the extracted value */
        p += log_str->len;
      }
      break;
    default:
      throw exc_bad_data_type(evt_data->datatype);
      break;
    }
  }

end_format:
  evt_fmt.EndFormat();

  return evt_fmt.GetFormatResult();
}

void
CLogEvt::Read (istream &in_s)
{
  mtlk_log_event_t log_evt;
  uint32           dsize;

  m_has_data = false;

  /* Read next log event header from input file */
  try {
    in_s.read((char *)&log_evt, sizeof(log_evt));
  }
  catch (const ios_base::failure &/*ex*/) {
    if (in_s.eof()) {
      /* EOF is not an error in our case */
      return;
    }
    /* Other stream error */
    throw; /* throw this exception up */
  }

  /* Check the log event header is correct */    
  if (!LOG_IS_CORRECT_INFO(log_evt)) {
    throw exc_bad_evt_hdr(log_evt.info_w0);
  }
    
  /* Get endianness */
  m_reversed = LOG_IS_INVERSED_ENDIAN(log_evt);

  /* Reverse log event header fields if required */
  if (m_reversed) {
    log_evt.info_w0   = REVERSE32(log_evt.info_w0);
    log_evt.info_w1   = REVERSE16(log_evt.info_w1);
    log_evt.timestamp = REVERSE32(log_evt.timestamp);
  }

  /* Extract log event information */
  m_wlanif =  (uint8)LOG_INFO_GET_WLAN_IF(log_evt);
  dsize    =         LOG_INFO_GET_DSIZE(log_evt);
  m_oid    =  (uint8)LOG_INFO_GET_OID(log_evt);
  m_gid    =  (uint8)LOG_INFO_GET_GID(log_evt);
  m_fid    = (uint16)LOG_INFO_GET_FID(log_evt);
  m_lid    = (uint16)LOG_INFO_GET_LID(log_evt);
  m_ts     = log_evt.timestamp;

  /* Read the log event data from input file, if required */  
  if (dsize) {
    /* Alloc log event data buffer */
    m_buffer.resize(dsize);
    /* Handle log event with data attached */
    in_s.read(GetBuf(), dsize);
  }

  m_has_data = true;
}

istream &operator>> (istream &is, CLogEvt &evt)
{
  evt.Read(is);
  return is;
}

void
CLogEvtFmt::BeginFormat (const CLogFmtDB &fmt_db)
{
  EXC_ASSERT(!m_in_progress);

  fmt_db.GetFormat(m_evt.GetOID(),
                   m_evt.GetGID(),
                   m_evt.GetFID(),
                   m_evt.GetLID(),
                   m_format);

  if (!m_format.empty()) {
    do {
      m_offset = m_format.find('%', m_offset + 1);
    } while (m_offset != string::npos && 
             m_format[m_offset + 1] == '%'); // Handle "%%".
    m_res = m_format.substr(0, m_offset);
  }
  else {
    stringstream ss;
    ss << "{" << (int) m_evt.GetOID()
       << ":" << (int) m_evt.GetGID()
       << ":" << (int) m_evt.GetFID()
       << ":" << (int) m_evt.GetLID()
       << "} :";

    m_res    = ss.str();
    m_offset = string::npos;
  }

  m_in_progress = true;
}

bool
CLogEvtFmt::TryProcessOwnExtension(const string& val_format, const int8 &val)
{
  return false;
}

bool 
CLogEvtFmt::TryProcessOwnExtension(const string& val_format, const int32 &val)
{
  char buf[64];
  if(val_format[1] == 'p') {
    // overwrite the %p for the 32 bit pointer
    snprintf(buf, sizeof(buf), "%08x", val);
  }
  else if(val_format[1] == MTLK_LOG_FMT_IP4) {
    snprintf(buf, sizeof(buf), IP4_PRINTF_FMT, IP4_PRINTF_ARG(&val));
  }
  else return false;

  
  /* Printf extension recognized and processed */
  /* Add result string to result */
  m_res += buf + val_format.substr(2);
  /* Tell caller that value was recognized as extension and processed */
  return true;
}

bool
CLogEvtFmt::TryProcessOwnExtension(const string& val_format, const int64 &val)
{
  char buf[64];

  if(val_format[1] == 'p') {
    // overwrite the %p for the 32 bit pointer
    uint32 hdw = (uint32)(val >> 32);
    uint32 ldw = (uint32)(val);
    snprintf(buf, sizeof(buf), "%08x%08x", hdw, ldw);
  }
  else return false;

  /* Printf extension recognized and processed */
  /* Add result string to result */
  m_res += buf + val_format.substr(2);
  /* Tell caller that value was recognized as extension and processed */
  return true;
}

bool
CLogEvtFmt::TryProcessOwnExtension(const string& val_format, const void *val)
{
  char buf[64];
  if(val_format[1] == MTLK_LOG_FMT_MAC) {
    snprintf(buf, sizeof(buf), MAC_PRINTF_FMT, MAC_PRINTF_ARG(val));
  }
  else if(val_format[1] == MTLK_LOG_FMT_IP6) {
    snprintf(buf, sizeof(buf), IP6_PRINTF_FMT, IP6_PRINTF_ARG(val));
  }
  else return false;

  /* Printf extension recognized and processed */
  /* Add result string to result */
  m_res += buf + val_format.substr(2);
  /* Tell caller that value was recognized as extension and processed */
  return true;
}

template<typename T> 
void
CLogEvtFmt::PutParam (const T& val)
{
  EXC_ASSERT(m_in_progress);

  /* No format string case => just print value */
  if (m_format.empty()) {
    stringstream ss;
    ss << " '" << val << "'";
    m_res += ss.str();
    return;
  }

  /* There is no %... specifier for this parameter */
  if (m_offset == string::npos) {
    throw exc_bad_format(m_format, m_offset, val);
  }

  /* Find next % or endl */
  size_t p_next = m_offset;
  do {
    p_next = m_format.find('%', p_next + 1);
  } while (p_next != string::npos && 
           m_format[p_next + 1] == '%'); // Handle "%%".

  /* Extract val format specifier */
  string val_fmt;
  if (p_next != string::npos) {
    val_fmt = m_format.substr(m_offset, p_next -  m_offset);
  }
  else {
    val_fmt = m_format.substr(m_offset);
  }

  /* Advance offset */
  m_offset = p_next;

  if(!TryProcessOwnExtension(val_fmt, val))
  {
    /* Format val for print */
    char buf[512];
    snprintf(buf, sizeof(buf), val_fmt.c_str(), val);

    /* Add result string to result */
    m_res += buf;
  }
}

void
CLogEvtFmt::EndFormat (void)
{
  EXC_ASSERT(m_in_progress);

  if (m_offset != string::npos) {
    m_res      += m_format.substr(m_offset);
  }
  m_offset      = 0;
  m_in_progress = false;
}
