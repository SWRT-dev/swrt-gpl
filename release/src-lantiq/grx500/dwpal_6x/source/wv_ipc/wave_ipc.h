/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_IPC__H__
#define __WAVE_IPC__H__

#define WAVE_IPC_BUFF_SIZE	(16400)

typedef enum _wv_ipc_ret {
	WAVE_IPC_SUCCESS = 0,
	WAVE_IPC_DISCONNECTED = 1,
	WAVE_IPC_ERROR = 2,
	WAVE_IPC_CMD_FAILED = 3,
	WAVE_IPC_CMD_WOULD_BLOCK = 4,
} wv_ipc_ret;

#define WAVE_IPC_MSG_NULL	(0)
#define WAVE_IPC_MSG_CMD	(1)
#define WAVE_IPC_MSG_RESP	(2)
#define WAVE_IPC_MSG_REQ_FAIL	(4)
#define WAVE_IPC_MSG_EVENT	(5)

#endif /* __WAVE_IPC__H__ */
