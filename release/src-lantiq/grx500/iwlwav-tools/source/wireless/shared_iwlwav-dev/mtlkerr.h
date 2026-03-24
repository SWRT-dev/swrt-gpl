/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#if !defined _MTLK_ERR_H_ || defined _MTLK_ERR_C_BUILD_

#ifndef _MTLK_ERR_C_BUILD_

#define _MTLK_ERR_H_
#define MTLK_SUCCESS(code)     ( (code) >= 0)
#define MTLK_FAILURE(code)     ( (code) < 0)

#define MTLK_NAMED_ENUM_START typedef enum {
#define MTLK_NAMED_ENUM_END(type) } type;
#define MTLK_NAMED_ENUM_ENTRY(name, value, descr) name=(value),

#endif

MTLK_NAMED_ENUM_START
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_OK,               (0), "Success")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_PARAMS,          (-1), "Parameters passed are invalid")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_UNKNOWN,         (-2), "Unknown error occurred")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NOT_SUPPORTED,   (-3), "Feature is not supported")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_TIMEOUT,         (-4), "Timeout reached while waiting for operation completion")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_EEPROM,          (-5), "EEPROM error occurred")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NO_MEM,          (-6), "Memory resources are exhausted")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_HW,              (-7), "Hardware initialization failed")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_FW,              (-8), "Firmware not found or failed to load")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_UMI,             (-9), "An unknown UMI error occurred")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_PENDING,        (-10), "Operation pending")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NOT_IN_USE,     (-11), "Object not in use")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NO_RESOURCES,   (-12), "System resources are exhausted")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_WRONG_CONTEXT,  (-13), "Unsuitable context for operation")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NOT_READY,      (-14), "System not ready to perform the operation requested")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_SCAN_FAILED,    (-15), "Scan process failed")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_AOCS_FAILED,    (-16), "AOCS activities failed")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_PROHIB,         (-17), "Requested action is currently prohibited")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_BUF_TOO_SMALL,  (-18), "Buffer passed to function is too small")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_PKT_DROPPED,    (-19), "Packet was dropped for unknown reason")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_FILEOP,         (-20), "File operation failed for unknown reason")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_VALUE,          (-21), "Value passed is invalid")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_BUSY,           (-22), "Object busy")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_MAC,            (-23), "Unknown FW error occurred")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_CANCELED,       (-24), "Action was canceled")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NOT_HANDLED,    (-25), "Request not handled")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_DATA_TOO_BIG,   (-26), "Data returned by request is to big to fit in buffer provided by user")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_ALREADY_EXISTS, (-27), "Object already exists")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_NO_ENTRY,       (-28), "Entry not found")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_SYSTEM,         (-29), "System error occurred")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_CORRUPTED,      (-30), "Corrupted data detected")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_DC_DP_XMIT,     (-31), "DC DP transmit error")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_RETRY,          (-32), "Notify that the call can be retried")
  MTLK_NAMED_ENUM_ENTRY(MTLK_ERR_CLPB_DATA,      (-33), "Clipboard data error")
MTLK_NAMED_ENUM_END(mtlk_error_t)

const char* __MTLK_IFUNC
mtlk_get_error_text(mtlk_error_t id);

#endif /* _MTLK_ERR_H_ */
