/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*****************************************************************************
*	MT_TFTP.C
*
*	TFTP server as part of the BCP project
*   Currently supported only in linux
*	Author: Ron Vainapel, 2008
*
*****************************************************************************/
#include "mtlkinc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>

#include "mt_cnfg.h"    // MTLK specific definitions
#include "mt_util.h"
#include "BCLSockServer.h"

#ifndef WIN32
#define LOG_LOCAL_GID   GID_MT_TFTP
#define LOG_LOCAL_FID   1
#endif /* !WIN32 */

extern volatile int BCL_nExit; /* if this variable sets to 1, BCL will exit */
extern unsigned int BCL_numOfThreads; /* the number of threads in the system */
int BCL_areWeLittleEndian; // TRUE when we are running on little endian system

#define TFTP_PORT 69
#define SOCKET_INVALID -1
#define TFTP_BUF_SIZE 576
#define TFTP_MAX_FILE_BLOCK_SIZE 512
#define TFTP_TEMP_WRITE_FILE "/tmp/tftp_temp_file"

typedef struct TFTP_t
{
	int			sock;
	MT_UBYTE*	inBuf;
	int			inBufLen;
	MT_UBYTE*	outBuf;
	MT_UBYTE*	filename;
	FILE*		pFile;
	int			outBufLen;
	int			blockNum;
	int			lastBlockSize;
	int			isSendingFile; // 1 for send, 0 for get
	struct sockaddr_in theiraddr_in;     /* for local socket address */

} TFTP_t;

#define TFTP_CLOSEFILE if (TFTP->pFile) { fclose(TFTP->pFile); TFTP->pFile = 0; }

enum OpCodes_t
{
	readRequestCode_e = 1,
	writeRequestCode_e,
	dataBlockCode_e,
	ackCode_e,
	errorCode_e,
};

enum errorCodes_t
{
	fileNotFound_e = 1,
	fileAccessViolation_e
};

void* TFTP_Error(TFTP_t* TFTP, const char* errStr)
{
	printf("%s", errStr);
	if (TFTP->sock != SOCKET_INVALID) close(TFTP->sock);
	BCL_numOfThreads--;
	BCL_nExit = 1;
	return MT_THREAD_RET;
}

static MT_UINT16 getRevWORD(MT_UINT16 word)
{
	if (BCL_areWeLittleEndian) return ((word & 0x00FF) << 8) | ((word & 0xFF00) >> 8);
	else return word; // WE ARE ALREADY IN A BIG-ENDIAN CPU !
}

static void putRevWORD(MT_UBYTE* buf, MT_UINT16 word)
{
	*((MT_UINT16*)buf) = getRevWORD(word);
}

static const char *getFileName(TFTP_t* TFTP)
{
	const char *fileName;
	size_t size;

	if (TFTP->inBufLen <= 2)
		return NULL;

	fileName = (const char*)(TFTP->inBuf + 2);
	size = strlen(fileName) + 1;

	if (size > (TFTP->inBufLen - 2))
		return NULL;

	/* TODO: add validation for file path (fileName) */

	return fileName;
}

void TFTP_SendBuffer(TFTP_t* TFTP)
{
	sendto(TFTP->sock, (const char*)TFTP->outBuf, TFTP->outBufLen, 0, (struct sockaddr *)&TFTP->theiraddr_in, sizeof(TFTP->theiraddr_in));
}

void TFTP_SendError(TFTP_t* TFTP, int errorIndex, const char* errStr)
{
	putRevWORD(TFTP->outBuf, errorCode_e);
	putRevWORD(TFTP->outBuf + 2, errorIndex);
	strcpy((char*) (TFTP->outBuf + 4), errStr);
	TFTP->outBufLen = strlen(errStr) + 5;
	TFTP_SendBuffer(TFTP);
}

void TFTP_SendNextBlock(TFTP_t* TFTP)
{
	if (!TFTP->isSendingFile || !TFTP->pFile)
	{
		printf("TFTP: Reached SendNextBlock when no file is sent.\n");
		TFTP_CLOSEFILE;
		return;
	}
	if (TFTP->blockNum > 0 && TFTP->lastBlockSize < TFTP_MAX_FILE_BLOCK_SIZE)
	{
		printf("TFTP: File has been sent successfully.\n");
		TFTP_CLOSEFILE;
		return;
	}
	++TFTP->blockNum;
	putRevWORD(TFTP->outBuf, dataBlockCode_e);
	putRevWORD(TFTP->outBuf + 2, TFTP->blockNum);
	TFTP->lastBlockSize = fread(TFTP->outBuf+4, 1, TFTP_MAX_FILE_BLOCK_SIZE, TFTP->pFile);
//	printf("TFTP: Sending file block %d, blockSize = %d\n", TFTP->blockNum, TFTP->lastBlockSize);
	TFTP->outBufLen = TFTP->lastBlockSize + 4;
	TFTP_SendBuffer(TFTP);
}

void TFTP_HandleReadRequest(TFTP_t* TFTP)
{
	const char* filename = getFileName(TFTP);
	if (!filename)
	{
		printf("TFTP: Invalid filename received.\n");
		TFTP_SendError(TFTP, fileNotFound_e, "MTLK TFTP: Invalid filename received !");
		return;
	}
	printf("TFTP: Client requests file: %s\n", filename);
	if (TFTP->pFile) printf("TFTP: Aborting previous file operation.\n");
	TFTP->pFile = fopen(filename, "rb");
	if (!TFTP->pFile)
	{
		printf("TFTP: Requested file not found.\n");
		TFTP_SendError(TFTP, fileNotFound_e, "MTLK TFTP: File not found !");
		return;
	}
	TFTP->blockNum = 0;
	TFTP->isSendingFile = 1;
	TFTP_SendNextBlock(TFTP);
}

void TFTP_HandleACK(TFTP_t* TFTP)
{
	MT_UINT16 ackBlock = getRevWORD(*((MT_UINT16*)(TFTP->inBuf + 2)));

	if (!TFTP->isSendingFile || !TFTP->pFile)
	{
		printf("TFTP: Reached ACK when no file is sent!\n");
		TFTP_CLOSEFILE;
	}

	if (ackBlock == TFTP->blockNum) TFTP_SendNextBlock(TFTP);
	else if (ackBlock == TFTP->blockNum - 1) TFTP_SendBuffer(TFTP); // Send the last block again
	else
	{
		printf("TFTP: Got ACK for block %d, when we already sent block %d\n", ackBlock, TFTP->blockNum);
		TFTP_CLOSEFILE;
	}
}

void TFTP_SendAck(TFTP_t* TFTP)
{
	putRevWORD(TFTP->outBuf, ackCode_e);
	putRevWORD(TFTP->outBuf + 2, TFTP->blockNum);
	TFTP->outBufLen = 4;
	TFTP_SendBuffer(TFTP);
}

void TFTP_HandleWriteRequest(TFTP_t* TFTP)
{
	const char* filename = getFileName(TFTP);
	if (!filename)
	{
		printf("TFTP: Invalid filename received.\n");
		TFTP_SendError(TFTP, fileAccessViolation_e, "MTLK TFTP: Invalid filename received !");
		return;
	}
	printf("TFTP: Client sends file: %s\n", filename);
	strcpy((char*) TFTP->filename, filename);
	if (TFTP->pFile) printf("TFTP: Aborting previous file operation.\n");
	unlink(TFTP_TEMP_WRITE_FILE);
	TFTP->pFile = fopen(TFTP_TEMP_WRITE_FILE, "wb");
	if (!TFTP->pFile)
	{
		printf("TFTP: Cannot create temporary file.\n");
		TFTP_SendError(TFTP, fileAccessViolation_e, "MTLK TFTP: Cannot create temporary file !");
		return;
	}
	TFTP->blockNum = 0;
	TFTP->isSendingFile = 0;
	TFTP_SendAck(TFTP);
}

void TFTP_HandleDataBlock(TFTP_t* TFTP)
{
	MT_UINT16 dataBlock;

	if (TFTP->isSendingFile || !TFTP->pFile)
	{
		printf("TFTP: Reached SendNextBlock when no file is sent.\n");
		TFTP_CLOSEFILE;
		return;
	}

	++TFTP->blockNum;
	dataBlock = getRevWORD(*((MT_UINT16*)(TFTP->inBuf + 2)));
	if (dataBlock != TFTP->blockNum) 
	{
		if (dataBlock == TFTP->blockNum - 1) TFTP_SendBuffer(TFTP); // Send the last ack again
		else
		{
			printf("TFTP: Got data block %d, when we already sent ack for block %d\n", dataBlock, TFTP->blockNum);
			TFTP_CLOSEFILE;
		}
		return;
	}

	TFTP->lastBlockSize = TFTP->inBufLen - 4;
	if (TFTP->lastBlockSize > 0) 
	{
		int writeSize = fwrite(TFTP->inBuf + 4, 1, TFTP->lastBlockSize, TFTP->pFile);
		if (writeSize != TFTP->lastBlockSize)
		{
			printf("Failed to write file block.\n");
			TFTP_CLOSEFILE;
		}
	}

//	printf("TFTP: Received file block %d, blockSize = %d\n", TFTP->blockNum, TFTP->lastBlockSize);
	if (TFTP->lastBlockSize < TFTP_MAX_FILE_BLOCK_SIZE)
	{
		struct stat fileStat = {0};
		int statRet = stat((char*) TFTP->filename, &fileStat);
		TFTP_CLOSEFILE;
		unlink((char*)TFTP->filename);
		snprintf((char*)TFTP->outBuf, TFTP_BUF_SIZE, "mv %s %s", TFTP_TEMP_WRITE_FILE, TFTP->filename);
		strip_unsafe_chars((char*)TFTP->outBuf, (const char*)TFTP->outBuf, TFTP_BUF_SIZE);
		system((char*)TFTP->outBuf);
		if (!statRet) chmod((char*)TFTP->filename, fileStat.st_mode);
		printf("TFTP: File has been received successfully.\n");
	}
	TFTP_SendAck(TFTP);
}

void TFTP_ParsePacket(TFTP_t* TFTP)
{
	int opCode = (int)getRevWORD(*((MT_UINT16*)TFTP->inBuf));
	switch (opCode)
	{
	case readRequestCode_e: TFTP_HandleReadRequest(TFTP); break;
	case writeRequestCode_e: TFTP_HandleWriteRequest(TFTP); break;
	case dataBlockCode_e: TFTP_HandleDataBlock(TFTP); break;
	case ackCode_e: TFTP_HandleACK(TFTP); break;
	default: printf("Received a wrong opcode: %04X", opCode); break;
	}
}

// Called when a client is connected.
// Parameter: sock - the connected socket
void TFTP_HandleSession(TFTP_t* TFTP)
{
	socklen_t sockAddrLen = sizeof(TFTP->theiraddr_in);

	memset((char *)&(TFTP->theiraddr_in), 0, sizeof(struct sockaddr_in));
	TFTP->theiraddr_in.sin_port = htons(TFTP_PORT);
	TFTP->theiraddr_in.sin_family = AF_INET;
	TFTP->theiraddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	TFTP->inBufLen = recvfrom(TFTP->sock, (char*)(TFTP->inBuf), TFTP_BUF_SIZE-2, 0, (struct sockaddr *)&(TFTP->theiraddr_in), &sockAddrLen);

	if (TFTP->inBufLen >= 2) 
	{
		TFTP->inBuf[TFTP->inBufLen] = 0;
		TFTP->inBuf[TFTP->inBufLen+1] = 0; // Terminate UNICODE string
		TFTP_ParsePacket(TFTP);
	}
	else printf("TFTP: Received a packet with %d bytes\n", TFTP->inBufLen);
}

// The TFTP Server function (main thread function)
// Starts the server socket, and handle its listen/accept stuff
MT_THREAD_RET_T TFTP_Server_Thread_Func(void* pParam)
{
	struct sockaddr_in myaddr_in;     /* for local socket address */
	long lResultStatus;

	TFTP_t TFTPobj;
	TFTP_t* TFTP = &TFTPobj;
	memset(TFTP, 0, sizeof(TFTP_t));
	TFTP->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (TFTP->sock == SOCKET_INVALID) return TFTP_Error(TFTP, "BCL Server Error --> can't create TFTP Socket!\n"); 

	/* Setup for bind */
	memset((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
	myaddr_in.sin_port = htons(TFTP_PORT);
	myaddr_in.sin_family = AF_INET;
	myaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Bind the socket */
	lResultStatus = bind(TFTP->sock,(struct sockaddr *)&myaddr_in, sizeof(myaddr_in));

	if (lResultStatus == -1)
  {
#ifdef WIN32
      printf("BCL: Failed to start TFTP server. Working without it.\n");
#else /* !WIN32 */
      ILOG0_V("BCL: Failed to start TFTP server. Working without it.");
#endif /* WIN32 */
      BCL_numOfThreads--;
      close(TFTP->sock);
      return MT_THREAD_RET;
  }
	//	setsockopt (TFTP_listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptVal, sizeof(int));

	TFTP->inBuf = (MT_UBYTE*)malloc(TFTP_BUF_SIZE);
	TFTP->outBuf = (MT_UBYTE*)malloc(TFTP_BUF_SIZE);
	TFTP->filename = (MT_UBYTE*)malloc(TFTP_MAX_FILE_BLOCK_SIZE);

    if (!TFTP->inBuf || !TFTP->outBuf || !TFTP->filename) {
      TFTP_Error(TFTP, "TFTP: NOT ENOUGH MEMORY!!!\n");
      goto FINISH;
    }

	while(!BCL_nExit)
	{
		TFTP_HandleSession(TFTP);
	}

FINISH:
    close(TFTP->sock);
    TFTP_CLOSEFILE;

    if (TFTP->inBuf)
      free(TFTP->inBuf);
    if (TFTP->outBuf)
      free(TFTP->outBuf);
    if (TFTP->filename)
      free(TFTP->filename);
	BCL_numOfThreads--;
	return MT_THREAD_RET;
}

void StartTFTPServer(void)
{
	pthread_t TFTP_ThreadID;

	// Check if we are little endian or big endian
	int testInt = 0x12345678;
	MT_UBYTE* pTestByte = (MT_UBYTE*)&testInt;
	BCL_areWeLittleEndian = (*pTestByte == 0x78);

	printf("BCL: Starting TFTP Server (system is %s-endian)\n", BCL_areWeLittleEndian ? "little" : "big");
	++BCL_numOfThreads;
	pthread_create(&TFTP_ThreadID, NULL, TFTP_Server_Thread_Func, (void*)0);
}
