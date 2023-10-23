/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOSAL_IPC_H__
#define __QOSAL_IPC_H__

#define IPC_SUCCESS 0
#define IPC_FAIL -1
#define IPC_INVALID_CALLER -2

#define MAX_MODID_LEN 32

typedef struct
{
   /* ID of the module sending the message */
   unsigned char aucfrom[MAX_MODID_LEN];
   /* ID of the module to which the message is addressed */
   unsigned char aucto[MAX_MODID_LEN];
   /* ucTo may be just used for strict validation, since
      the message destined for a module will be in the 
      module's FIFO anyway
    */
   /* Size of the Message */
   uint16_t unmsgsize;
   /* Msg type can be passed here for receiver to know how to interpret acMsg */
   int16_t nMsgType;
   /* Reserved for future use */
   uint32_t uireserved;
   /* Shared memory key used for get q/cl  */
   uint32_t shm_key;
   int32_t respCode;
} x_ipc_msghdr_t;

#define IPC_HDR_SIZE sizeof(x_ipc_msghdr_t)
#define IPC_MAX_MSG_SIZE (4192 - IPC_HDR_SIZE)

typedef struct
{
   x_ipc_msghdr_t xhdr;
   /* Buffer to contain the message */
   char acmsg[IPC_MAX_MSG_SIZE];
   /* Each module is expected to write a union in the acMsg part */
} x_ipc_msg_t;

typedef struct{
	int32_t ifd;
	int32_t iconnfd;
}x_ipc_handle;

/* Error Codes */
typedef enum
{
   IPC_NO_ERR = 0,
   IPC_FIFO_READ_ERR = 1,
   IPC_HDR_ERR,
   IPC_FIFO_WRITE_ERR,
   IPC_MSG_ERR
} e_ipc_error_t;

/* Module Ids */
typedef enum
{
   IPC_APP_ID_FAPI = 1,
   IPC_APP_ID_QOSD = 2,
   IPC_APP_ID_MAX = 3
} e_ipc_appid_t;

/* PUBLIC Functions */
int32_t
ipc_send_request(x_ipc_msg_t *pxmsg);

int32_t
ipc_recv(void *pvhandle,
				 x_ipc_msg_t *pxmsg);

void*
ipc_create_listener(char *pucto);

int32_t
ipc_send_reply(void *pvhandle,
               x_ipc_msg_t *pxmsg);

int32_t ipc_get_memory(uint32_t *shm_key, int32_t *shm_id, char **data, uint32_t size, int32_t create);
int32_t ipc_delete_memory(int32_t shm_id, char **data, int32_t delete);

#endif /*__QOSAL_IPC_H__*/
