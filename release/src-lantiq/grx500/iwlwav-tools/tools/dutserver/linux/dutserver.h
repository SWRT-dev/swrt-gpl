/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DUTSERVER_H__
#define __DUTSERVER_H__

extern BOOL toEndServer;

typedef struct _dut_hostif_funcs {
  void   (*pOnDutRequest) (const char* data, int length, int dutIndex);
  int   returned_message_id;
  int   response_length;
  char* response_data;
} dut_hostif_funcs;

#include "dataex.h"
#include "driver_api.h"

/**************************************************************************************
Functions exported by dut_host_if.c
**************************************************************************************/
void
dut_hostif_handle_packet (const char* inBuf, int packetLength);

/**************************************************************************************
Functions exported by dutmsg_clbk.c
**************************************************************************************/
void __MTLK_IFUNC
dut_msg_clbk_init();

dut_hostif_funcs*
dut_msg_clbk_get_handler(int msgID);

/**************************************************************************************
Functions exported by communication layer source file - currently socket.c
**************************************************************************************/
void __MTLK_IFUNC
dut_comm_send_to_peer (char* data, int dataLength);

int __MTLK_IFUNC
dut_comm_start_server (void);

void __MTLK_IFUNC
dut_comm_end_server (void);


#endif /* !__DUTSERVER_H__ */

