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
#include "dutserver.h"

#define LOG_LOCAL_GID   GID_DUT_SRV_HOST_IF
#define LOG_LOCAL_FID   1

#define DUT_MSG_ID_RESPONSE_FLAG          0x80
#define DUT_MSG_HEADER_LENGTH             (8)

#define DUT_MSG_VERIFY_SIGNATURE(hdr)     ((hdr)[0] == 'M' && (hdr)[1] == 'T')
#define DUT_MSG_GET_HEADER_BY_DATA(data)  ((uint8*)(data) - DUT_MSG_HEADER_LENGTH)
#define DUT_MSG_SET_ID(hdr, id)           (((uint8*)(hdr))[3] = id)
#define DUT_MSG_SET_LENGTH(hdr, len)      (((uint32*)(hdr))[1] = HOST_TO_DUT32(len))
#define DUT_MSG_GET_ID(hdr)               (((uint8*)(hdr))[3] & 0xF)
#define DUT_MSG_GET_HW_IDX(hdr)           ((((uint8*)(hdr))[3] >> 4) & 0xF)
static __INLINE uint32
DUT_MSG_GET_LENGTH(const char* hdr)
{
    uint32 raw_length;
    wave_memcpy(&raw_length, sizeof(uint32), hdr + sizeof(uint32), sizeof(uint32));
    return DUT_TO_HOST32(raw_length);
}

static void
_dut_hostif_send_response(int msgID, const char* data, int dataLength)
{
  void *hdr = DUT_MSG_GET_HEADER_BY_DATA(data);

  ILOG1_DPD("Sending response msgID 0x%X, data %p, length %d",
            msgID, data, DUT_MSG_HEADER_LENGTH + dataLength);

  DUT_MSG_SET_ID(hdr, msgID | DUT_MSG_ID_RESPONSE_FLAG);
  DUT_MSG_SET_LENGTH(hdr, dataLength);
  dut_comm_send_to_peer(hdr, DUT_MSG_HEADER_LENGTH + dataLength);
}

void
dut_hostif_handle_packet (const char* inBuf, int packetLength)
{
  int dataLength;
  int msgID;
  int dutIndex;
  dut_hostif_funcs* msgHandler;

  // Check packet header validity
  if (packetLength < DUT_MSG_HEADER_LENGTH)
  {
    ELOG_D("Wrong packet received. Packet length = %d", packetLength);
    return;
  }

  if (!DUT_MSG_VERIFY_SIGNATURE(inBuf))
  {
    ELOG_D("Wrong packet received. MT signature not found. Packet length =  %d",
           packetLength);
    return;
  }

  dataLength = DUT_MSG_GET_LENGTH(inBuf);
  if (packetLength != dataLength + DUT_MSG_HEADER_LENGTH)
  {
    ELOG_DD("The length of the received packet (%d) is inconsistent with data length (%d)",
            packetLength, dataLength);
    return;
  }

  // Parse the packet and try to process
  msgID = DUT_MSG_GET_ID(inBuf);
  dutIndex = DUT_MSG_GET_HW_IDX(inBuf);

  ILOG1_DDD("Received message ID 0x%X for HW #%d, length is %d",
            msgID, dutIndex, dataLength);

  msgHandler = dut_msg_clbk_get_handler(msgID);
  if(NULL == msgHandler)
  {
    ELOG_D("Incorrect message ID 0x%X", msgID);
    return;
  }

  msgHandler->response_data = NULL;

  if (msgHandler->pOnDutRequest)
  {
    msgHandler->pOnDutRequest(inBuf + DUT_MSG_HEADER_LENGTH, dataLength, dutIndex);
  }
  else
  {
    ILOG0_V("Message handler not registered");
  }

  if(NULL != msgHandler->response_data)
  {
    _dut_hostif_send_response(msgHandler->returned_message_id,
                              msgHandler->response_data,
                              msgHandler->response_length);
    ILOG2_V("Message is processed. And response is sent.");
  }
  else
  {
    WLOG_V("Message processing failed. Response will NOT be sent.");
  }
}
