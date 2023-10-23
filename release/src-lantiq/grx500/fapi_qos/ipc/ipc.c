/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <commondefs.h>
#include <qosal_debug.h>
#include <qosal_ipc.h>
#include <errno.h>
#include <err.h>
#include <qosal_ugw_debug.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

//#define IPC_FILE_PATH "/opt/park/pramod/MsgTo"
#define IPC_FILE_PATH "/tmp/qosipc/MsgTo"

int32_t
ipc_send_request(x_ipc_msg_t *pxmsg)
{
	int16_t nbytes = 0;
	int32_t ifd,iret;
	char acbuf[MAX_NAME_LEN];
	struct sockaddr_un xaddr;

	iret=snprintf(acbuf,MAX_NAME_LEN, IPC_FILE_PATH"%s",pxmsg->xhdr.aucto);

	/* Create a socket */
	if((ifd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		LOGF_LOG_ERROR("IPC create failed\n");
		return LTQ_FAIL;
	}
	/* Construct name to connect the socket to */
	memset_s(&xaddr, sizeof(xaddr), 0);
	xaddr.sun_family = AF_UNIX;
	strncpy_s(xaddr.sun_path, MAX_NAME_LEN, acbuf, sizeof(xaddr.sun_path)-1);
	/* connect the socket */
	if(connect(ifd, (struct sockaddr*)&xaddr, sizeof(xaddr)) == -1) {
		LOGF_LOG_DEBUG("IPC listener missing\n");
		if(ifd >= 0)
			close(ifd);
		return LTQ_FAIL;
	}

	/* Write the message */
	if (write(ifd, (char *)pxmsg, pxmsg->xhdr.unmsgsize + IPC_HDR_SIZE) < 0)
	{
		/* Error Writing Message to the FIFO */
		/* Add Debugs if required */
		if(ifd >= 0)
			close(ifd);
		return IPC_FAIL;
	}
	/* block for reply */
	LOGF_LOG_DEBUG("Blocked to read the header\n");
	int32_t nAttempt = 0;
	while(1) {
		if((nbytes = read(ifd, (char *) &pxmsg->xhdr,
						sizeof(x_ipc_msghdr_t))) < 0)
		{
			/* Error Reading Header */
			LOGF_LOG_ERROR("Error reading response\n");
			if (nAttempt < 3) {
				nAttempt++;
				continue;
			}
			if(ifd >= 0)
				close(ifd);
			return IPC_FAIL;
		}
		else {
			break;
		}
	}
	if (nbytes != sizeof(x_ipc_msghdr_t))
	{
		if(ifd >= 0)
			close(ifd);
		return IPC_FAIL;
	}
	else
	{
		iret = pxmsg->xhdr.unmsgsize;
		if(iret > (int32_t)IPC_MAX_MSG_SIZE)
		{
			if(ifd >= 0)
				close(ifd);
			return IPC_FAIL;
		}
	}
	LOGF_LOG_DEBUG("Blocked to read the msg\n");
	if ((nbytes = read(ifd, pxmsg->acmsg, iret)) < 0)
	{
		/* Error Reading Header */
		/* Add Debugs if required */
		if(ifd >= 0)
			close(ifd);
		return IPC_FAIL;
	}

	if(ifd >= 0)
		close(ifd);
	return IPC_SUCCESS;
}

#define QOS_SHM_KEY_START 1234
#define QOS_SHM_KEY_END 1235
#define QOS_SHM_PERM 0666

int32_t ipc_get_memory(uint32_t *shm_key, int32_t *shm_id, char **data, uint32_t size, int32_t create)
{
	int32_t flags = QOS_SHM_PERM;

	if (create) {
		flags |= IPC_CREAT | IPC_EXCL;
		*shm_key = QOS_SHM_KEY_START;
	}

again:
	*shm_id = shmget(*shm_key, size, flags);
	if (*shm_id < 0) {
		if (create != 0 && errno == EEXIST && *shm_key < QOS_SHM_KEY_END){
			*shm_key += 1;
			goto again;
		}
		LOGF_LOG_CRITICAL("Could not get shared memory....\n");
		*shm_key = 0;
    return IPC_FAIL;
	}
	else {
		LOGF_LOG_DEBUG("shmget is sucessfull...\n");
	}

	/* attach to the segment to get a pointer to it: */
	*data = shmat(*shm_id, (void *) NULL, 0);
	if (*data == (char *)(-1)) {
		perror("shmat");
		*shm_key = 0;
		shmctl(*shm_id, IPC_RMID, NULL);
    return IPC_FAIL;
	}

	return IPC_SUCCESS;
}

int32_t ipc_delete_memory(int32_t shm_id, char **data, int32_t delete)
{
	/* deattach to the segment to get a pointer to it: */
	shmdt((void *) *data);

	if (delete)
		shmctl(shm_id, IPC_RMID, NULL);

	return IPC_SUCCESS;
}
