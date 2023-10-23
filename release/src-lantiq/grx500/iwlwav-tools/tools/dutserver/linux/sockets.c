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
#include "compat.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <signal.h>

#define LOG_LOCAL_GID   GID_DUT_SOCKETS
#define LOG_LOCAL_FID   1

#define _THREAD_RET_T void*
#define _THREAD_RET NULL
typedef unsigned int NAME_LEN_TYPE;

#define MT_SERVER_PORT_STREAM  22222  /* DUT server/client port */
#define MT_BUFFERSIZE          1024

/* TODO: Following variables are not thread safe */
/* Locking has to be introduced                  */
static int listenSocket = INVALID_SOCKET;
static int lNewMsgSocket = INVALID_SOCKET;
static int lCurrentMsgSocket = INVALID_SOCKET;

static mtlk_atomic_t numThreads;

void __MTLK_IFUNC
dut_comm_end_server()
{
  close(listenSocket);
  close(lCurrentMsgSocket);
  close(lNewMsgSocket);
  toEndServer = TRUE;
  mtlk_osal_msleep(100);
  while (mtlk_osal_atomic_get(&numThreads))
  {
    ELOG_V("Waiting for all threads to finish...");
    mtlk_osal_msleep(250);
  }
  dut_api_driver_stop(FALSE);
  ILOG0_V("Socket server stopped successfully");
}

void __MTLK_IFUNC
dut_comm_send_to_peer (char* data, int dataLength)
{
  if (INVALID_SOCKET == lCurrentMsgSocket)
    return;

  send(lCurrentMsgSocket, data, dataLength, 0);
}

/*****************************************************************************
*   Function Name:  _dut_comm_CompareIP
*   Description:
*           Compare IP address
*   Return Value:   MT_RET_OK - same string
                    MT_RET_FAIL - different string
*   Algorithm:  Gets the sa_data[] address (of the sockaddr struct). It holds the
                port number (two first bytes) and the IP address (the successive
                four bytes) of the host that is connected to current task. It starts
                bytes compare from the third byte i.e. compares addresses only.
*****************************************************************************/
static int
_dut_comm_CompareIP(const struct sockaddr* addr1, const struct sockaddr* addr2)
{
    return memcmp(addr1->sa_data + 2, addr2->sa_data + 2, MTLK_IP4_ALEN);
}

static _THREAD_RET_T
_dut_comm_listenSocketThread(void* unusedArg)
{
  long lMsgSocketTemp;
  int nOptVal = 1;
  long lResultStatus;
  struct sockaddr_in myaddr_in, their_addr;    /* for local socket address */
  fd_set rfds;
  struct timeval tv;
  struct sockaddr hostAddrNew, hostAddrOld;    /* for local socket address */
  struct linger sock_linger_params;

  memset(hostAddrNew.sa_data, 0, sizeof(hostAddrNew.sa_data));
  memset(hostAddrOld.sa_data, 0, sizeof(hostAddrOld.sa_data));

  mtlk_osal_atomic_inc(&numThreads);
  ILOG0_D("dutserver listen thread started, pid = %d", (int)getpid());

  /* Clear out address structures */
  memset((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
  memset((char *)&their_addr, 0, sizeof(struct sockaddr_in));

  /* Create the socket */
  listenSocket = socket(AF_INET,SOCK_STREAM,0);
  if (listenSocket == INVALID_SOCKET)
  {
    ELOG_S("Failed to create listening socket: %s", strerror(errno));
    toEndServer = TRUE;
    mtlk_osal_atomic_dec(&numThreads);
    return _THREAD_RET;
  }

  /* Reuse the socket address, so bind doesn't fail*/
  setsockopt (listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptVal, sizeof(int));

  /* Setup for bind */
  myaddr_in.sin_family = AF_INET;
  myaddr_in.sin_port = htons(MT_SERVER_PORT_STREAM);
  myaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

  /* Bind the socket */
  lResultStatus = bind(listenSocket,(struct sockaddr *)&myaddr_in, sizeof(myaddr_in));
  if (lResultStatus < 0)
  {
    ELOG_S("Failed to bind listening socket: %s", strerror(errno));
    close(listenSocket);
    mtlk_osal_atomic_dec(&numThreads);
    toEndServer = TRUE;
    return _THREAD_RET;
  }

  /* Wait for data */
  ILOG0_V("listening socket bound successfully. Waiting for connections...");
  lResultStatus = listen(listenSocket,5);
  if (lResultStatus < 0)
  {
    ELOG_S("Failed to listen for connections: %s", strerror(errno));
    close(listenSocket);
    mtlk_osal_atomic_dec(&numThreads);
    toEndServer = TRUE;
    return _THREAD_RET;
  }

  sock_linger_params.l_onoff = TRUE;
  sock_linger_params.l_linger = 0;

  setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, &sock_linger_params, sizeof(sock_linger_params));

  while(!toEndServer)
  {
    NAME_LEN_TYPE addrlen;
    int sock_select_result;
    /*************************************
    * In a loop test for connect request.
    * In case connect was requested it creates task for the connect session.
    * At the BCL_RxMsgSocket(), in case there is an active message socket task, the IP of the host
    * connected is test:
    * If from the same host as the active message socket task -
    *  connection might disconnected without informing to target, therefore close old, open new.
    * Else
    *  connection from other board is not allow when another one is active, therefore close the new.
    */
    FD_ZERO(&rfds);
    FD_SET(listenSocket, &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    sock_select_result = select(listenSocket+1, &rfds, NULL, NULL, &tv);
    if (-1 == sock_select_result) {
      break;
    }
    else
      if (0 == sock_select_result) {
        continue;
      }
    else {
      if (!FD_ISSET (listenSocket, &rfds))
        continue; /* False alarm - connection has been removed before establishment */
    }

    addrlen =  sizeof(struct sockaddr_in);
    lMsgSocketTemp = accept(listenSocket, (struct sockaddr *)&their_addr, &addrlen);
    if (lMsgSocketTemp != INVALID_SOCKET)
    {
      NAME_LEN_TYPE nHostNameLength;
      if (lNewMsgSocket != INVALID_SOCKET)
      {
        /*
          If we got here - it means that we already have a communication path with the DLL.
        */
        nHostNameLength = sizeof (hostAddrNew);
        getpeername(lMsgSocketTemp, &hostAddrNew, &nHostNameLength);

        /* Linux Bugfix: if the old socket was closed, the old address gets NULLs, so kill the old thread. */
        if (!_dut_comm_CompareIP(&hostAddrNew, &hostAddrOld))
        {
          long lMsgSocketPrev = lNewMsgSocket;
          WLOG_D ("Socket server is closing old connection with %ld", lMsgSocketPrev);
          lNewMsgSocket = INVALID_SOCKET; /* mark that we have to replace the socket to be used */
          /* close old socket */
          close(lMsgSocketPrev);
        }
        else
        {
          WLOG_V ("Socket server is closing new connection");
          close(lMsgSocketTemp);
          lMsgSocketTemp = INVALID_SOCKET;
        }
      }

      /* if we got here, we either already closed old socket, or it is the first time we establish a communication path - just set the new one */
      if (lMsgSocketTemp != INVALID_SOCKET)
      {
        char* IP;

        MTLK_UNREFERENCED_PARAM(IP);

        nHostNameLength = sizeof (hostAddrNew);
        getpeername(lMsgSocketTemp, &hostAddrOld, &nHostNameLength);

        IP = (char *)hostAddrOld.sa_data + 2;
        ILOG0_DDDD("DUT: %d.%d.%d.%d - ", IP[0],IP[1],IP[2],IP[3]);
        ILOG0_D("DUT: open new socket %ld\n",lMsgSocketTemp);
        /*** Set socket options ***/

        /* Signal receiving socket upon close request */
        setsockopt(lMsgSocketTemp, SOL_SOCKET, SO_LINGER, &sock_linger_params, sizeof(sock_linger_params));

        /* Force small messages to be sent with no delay */
        setsockopt (lMsgSocketTemp, IPPROTO_TCP, TCP_NODELAY, (char*)&nOptVal, sizeof(int));

        /* Close connection in case connection lost (default - 2 hours quiet to send keepalive message) */
        setsockopt (lMsgSocketTemp, SOL_SOCKET, SO_KEEPALIVE, (char*)&nOptVal, sizeof(int));
        /*ioctl (lMsgSocketTemp, FIONBIO, &nOptVal);*/

        /* set the active socket for the threads to use */
        lNewMsgSocket = lMsgSocketTemp;
        lMsgSocketTemp = INVALID_SOCKET;
      }
    }
    mtlk_osal_msleep(10);
  }
  mtlk_osal_atomic_dec(&numThreads);
  return _THREAD_RET;
}

/*****************************************************************************
*   Function Name:  _dut_comm_msgThread
*   Description:
*           Enables Socket communication
*   Algorithm:
            Clear all buffers.
            Test if there is additional (old) message socket task (lNewMsgSocket = INVALID_SOCKET if no task is active).
            If there is old active task:
                Get the IP of the old and the new message socket tasks.
                If new and old IP are the same:
                    Close the old task as it is not valid any more (probably was closed with no target notification).
                Else:
                    Other PC is not allow to connect in case PC is already connected. Close the new task and socket.
               lNewMsgSocket (and local lMsgSocketOld) is updated with the active task.
               Due to real time problem and IP constrains (IP sequence can change) lMsgSocketOld is used in this function
               (local variable) in addition to the golbal lNewMsgSocket variable.
               Get the task ID for all the operations required it and for future task delete when needed.
               Handle Rx data flow. Calls parsePacket when data is received
*****************************************************************************/
static _THREAD_RET_T
_dut_comm_msgThread (void* arg)
{
  long lResultStatus;
  char inBuf[MT_BUFFERSIZE];
  void (*dut_msg_parser_ptr)(const char* inBuf, int packetLength) = arg;

  mtlk_osal_atomic_inc(&numThreads);
  ILOG0_D("Communication socket thread is running..., pid = %d", (int)getpid());

  while(!toEndServer)
  {
    if (lCurrentMsgSocket == INVALID_SOCKET)
    {
      mtlk_osal_msleep(10);
    }

    lResultStatus = -2;
    while (lResultStatus == -2 && lCurrentMsgSocket == lNewMsgSocket && lCurrentMsgSocket != INVALID_SOCKET && !toEndServer)
    {
      lResultStatus = recv(lCurrentMsgSocket, inBuf, MT_BUFFERSIZE, 0);
      if (lResultStatus == 0)
      {
        WLOG_V ("Connection closed");
        /* it may be that the msgsocket was closed from the outside */
        close(lCurrentMsgSocket);
        /* if this is also the last socket, clear it */
        if (lCurrentMsgSocket == lNewMsgSocket)
          lNewMsgSocket = INVALID_SOCKET;
        /* set current to invalid */
        lCurrentMsgSocket = INVALID_SOCKET;
      }
      else if (lResultStatus != -1)
      {
        dut_msg_parser_ptr(inBuf, lResultStatus);
      }
    }

    /* check if the socket was replaced */
    if (lCurrentMsgSocket != lNewMsgSocket)
    {
      WLOG_DD("Socket replaced: %d with %d", lCurrentMsgSocket, lNewMsgSocket);
      /* try to close old socket */
      if (lCurrentMsgSocket != INVALID_SOCKET)
      {
        close(lCurrentMsgSocket);
        WLOG_D("Old socket %d closed", lCurrentMsgSocket);
      }

      /* replace current socket */
      lCurrentMsgSocket = lNewMsgSocket;
    }
  }

  ILOG0_V ("Communication thread is finishing...");
  mtlk_osal_atomic_dec(&numThreads);
  return _THREAD_RET;
}

static void
_dut_comm_on_terminate(int sig)
{
  ILOG0_V("Received termination signal: shutting down");
  toEndServer = TRUE;
}

int dut_comm_start_server()
{
  struct sigaction sa;
  pthread_t listenSocketThreadID;
  pthread_t commSocketThreadID;
  mtlk_osal_atomic_set(&numThreads, 0);
  signal(SIGPIPE, SIG_IGN);

  memset(&sa, 0, sizeof(struct sigaction));
  sa.sa_handler = _dut_comm_on_terminate;
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  pthread_create(&listenSocketThreadID, NULL, _dut_comm_listenSocketThread, (void*)0);
  pthread_create(&commSocketThreadID, NULL, _dut_comm_msgThread, (void*)dut_hostif_handle_packet);
  return MTLK_ERR_OK;
}
