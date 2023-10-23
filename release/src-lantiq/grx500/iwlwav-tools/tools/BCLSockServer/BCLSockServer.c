/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* BCL Socket Server, for running on Windows host.
   This should be platform independant.
   Based on SCL socket code.
 */

#include "mtlkinc.h"
#include <stdio.h>
#include <string.h>
extern void mt_print(char level,char *fmt, ...);

#include "mt_cnfg.h"    // MTLK specific definitions

#ifdef UNDER_CE
#pragma warning(disable: 4005)
#endif

/* ---- Windows-specific definitions ---- */
#ifdef WIN32
    #include <Winsock2.h>
#ifndef UNDER_CE
    #include <process.h>
#endif

#else /* !WIN32 --> Currently targeting linux specifically */
#define _STANDALONE
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <pthread.h>
    #include <strings.h>
    #include <semaphore.h>
    #include <signal.h>
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    #include "mtlk_rtlog_app.h"
#endif

#endif /* WIN32 */

#include "mt_bcl.h"
#include "mt_bcl_defs.h"
#include "BCLSockServer.h"    // MTLK specific definitions
#include "mt_util.h"

#ifndef WIN32
#define LOG_LOCAL_GID   GID_BCLSOCKSERVER
#define LOG_LOCAL_FID   1

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

#endif /* !WIN32 */

#define SOCKET_INVALID -1


void BCL_TxSocketsrv(void); 

/* socket variables */
int BCL_nListenSocket;
short BCL_nListenPort;
long lNewMsgSocket = SOCKET_INVALID;
long lCurrentMsgSocket = SOCKET_INVALID;

/* Save the last command that was issued */
char BCL_cLastCommand[MT_BUFFERSIZE];

/* Save the result of the last command 
TODO: Is this needed, or is there a more efficient, but safe way to store this in a shared buffer? */
char BCL_cLastResult[MT_BUFFERSIZE];
BCL_eAckOrNack BCL_LastAckOrNack;

volatile int BCL_nExit = 0; /* if this variable sets to 1, BCL will exit */
volatile int BCL_nWantToExit = 0; /* set this variable to 1 to exit the BCL via a BCL command */

volatile unsigned int BCL_numOfThreads = 0; /* the number of threads in the system */

/*** Forward declarations ***/
MT_INT32 BCL_RxBuffFreeSpace(void) 
{ 
    // TODO: This function is useless now.
    // It should return the free space in the buffer, and its usage moved to a more adequate place
    return MT_BUFFERSIZE; 
}

/*** Check if a new command is ready to be handled by BCL ***/
/* This checks the receive buffer, and makes sure that a newline was read */
MT_BYTE BCL_IsNewCommandReady(const MT_UBYTE* cDataArray)
{
    if (strchr((const char*)cDataArray, (char)MT_CR) > 0) {
        return 1;
    }
    //mt_print(1,"\nNot ready yet\n");
    //sleep(1);
    return 0;
}


/*** Functions to convert BCL to/from SCL protocol used by Broadband Studio ***/

void BCL_SclRemoveEOL(MT_UBYTE* cDataArray) 
{
    unsigned int nLastChar;
	MT_UBYTE* cCR;
    nLastChar = strlen((const char*)cDataArray) - 1;
	cCR = (MT_UBYTE*)strchr((const char*)cDataArray, MT_CR);
    // Safety checks
    if (nLastChar > MT_BUFFERSIZE) {
        mt_print(1,"ERROR: BCL_SclRemoveEOL: Illegal length");
        // TODO: signal to caller that an error occured
        return;
    }
    if (cCR != NULL) {
        *cCR = '\0'; 
    }
}

/* Echo is the last command, with a LF appended to it. */
void BCL_SclAddEcho(MT_UBYTE* cDataArray) 
{
    char LF[2] = {MT_LF,0};
    strncat((char*)cDataArray, (const char*)BCL_cLastCommand, (MT_BUFFERSIZE - strlen((const char*)cDataArray) - 1));
    strncat((char*)cDataArray, (const char*)LF, (MT_BUFFERSIZE - strlen((const char*)cDataArray) - 1));
    BCL_cLastCommand[0] = '\0'; 
}

/* Prompt is CR/LF and 00> */
void BCL_SclAddPrompt(MT_UBYTE* cDataArray) 
{
    char prompt[] = {MT_CR, MT_LF, '0', '0', '>', '\0'};
    strncat((char*)cDataArray, (const char*)prompt, (MT_BUFFERSIZE - strlen((const char*)cDataArray) - 1));
}

/* Add ACK or NACK char */
void BCL_SclAddAckNack(MT_UBYTE* cDataArray, BCL_eAckOrNack AckOrNack) 
{
    char ack[2] = {0,0};
    ack[0] = (char)AckOrNack;
    strncat((char*)cDataArray, (const char*)ack, (MT_BUFFERSIZE - strlen((const char*)cDataArray) - 1));
}


/*** Handler functions that send commands to BCL and get the results ***/

void BCL_HandlerRx(MT_UBYTE* cDataArray, int lDataLength) 
{
    // Unreferenced formal parameter
    lDataLength = lDataLength;
    // Save command for echo.
    strncpy((char*)BCL_cLastCommand, (const char*)cDataArray, MT_BUFFERSIZE);
    BCL_cLastCommand[MT_BUFFERSIZE-1] = '\0';

    // Remove SCL CR from end of command before calling BCL interpretor
    BCL_SclRemoveEOL(cDataArray);

    // DEBUG:
    mt_print(1,"%s:\n", cDataArray);

    // Send command to BCL interpreter
    if (executeBCLCommand((char*)cDataArray, strlen((const char*)cDataArray))) {
        BCL_LastAckOrNack = BCL_ACK;
    } else {
        BCL_LastAckOrNack = BCL_NACK;
    }

    strncpy((char*)BCL_cLastResult, (const char*)cDataArray, MT_BUFFERSIZE);
    BCL_cLastResult[MT_BUFFERSIZE-1] = '\0';

    // DEBUG:
    mt_print(1,"%s\n", cDataArray);
}  

void BCL_HandlerTx(MT_UBYTE* cDataArray, int* nDataLength) 
{ 
    // TODO: 1. Get output from BCL interpreter
    //       2. Wrap with SCL protocol (e.g. ack, prompt, NL)

    *nDataLength = 0;

    cDataArray[0] = '\0';

    // Copy the last command for echo:
    BCL_SclAddEcho(cDataArray);

    // TODO: Ask BCL for ACK or NACK
    BCL_SclAddAckNack(cDataArray, BCL_LastAckOrNack);

    // TODO: 
    // The ack may need to arrive before the data, if this is a slow command.
    // In this case, calling BCL should be 2 stages.
    // 1. Get AckNack
    // 2. Get data when ready. (wait in this func until ready)
    // TODO: (not necessarily here), don't allow Rx until Tx is finished. (was this the purpose of Arad's task lock?)

    strncat((char*)cDataArray, (const char*)BCL_cLastResult, (MT_BUFFERSIZE - strlen((const char*)cDataArray) - 1)); 

    // Append prompt:
    BCL_SclAddPrompt(cDataArray);

    *nDataLength = strlen((const char*)cDataArray);
}  



MT_THREAD_RET_T BCL_RxMsgSocket (void* RxMsgSocketArgs);
//void BCL_RxMsgSocket (int their_addr, int addrlen, int lMsgSocketNew);



/* the following code implements the SOCKET SUPPORT for the SCL communication */
/*****************************************************************************
*   Function Name:  BCL_RxSocketsrv 
*   Description:   
*           Enables Socket comunication
*   Input Parameters: 
*        None
*   Output Parameters: 
*       None
*   Return Value: 
*   Algorithm:
            Create listen socket, bind it to address and listen for connect request.
            Non blocking is set in order to continues monitoring connection.
            In a loop test for connect request:
                In case connect was requested it creates task for the connect session.
                At the BCL_RxMsgSocket(), in case there is an active message socket task, the IP of the host
                connected is test:
                If from the same host as the active message socket task
                  connection might disconnected without informing to target, therefore close old, open new.
                Else
                  connection from other board is not allow when another one is active, therefore close the new.
*****************************************************************************/
MT_THREAD_RET_T BCL_RxSocketsrv(void* ignored) 
//void BCL_RxSocketsrv(void) 
{
    long lMsgSocketTemp;
    socklen_t addrlen;
    int nOptVal = 1;
    long lResultStatus;
    int i;
    struct sockaddr_in myaddr_in, their_addr;     /* for local socket address */
    char cDataArray[MT_BUFFERSIZE];
    
    socklen_t nHostNameLength;
    struct sockaddr hostAddrNew, hostAddrOld;     /* for local socket address */

    // unreferenced formal parameter
    ignored = ignored;

    memset(hostAddrNew.sa_data, 0, 14);
    memset(hostAddrOld.sa_data, 0, 14);

    BCL_numOfThreads++;
#ifndef UNDER_CE
    mt_print(1,"BCL_RxSocketsrv thread = %d\n",getpid());
#endif
    
    /* Clear out address structures */
    memset((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
    memset((char *)&their_addr, 0, sizeof(struct sockaddr_in));

    /* clear out buffer */
    memset(cDataArray, 0, MT_BUFFERSIZE);

    /* Create the socket */
    BCL_nListenSocket = socket(AF_INET,SOCK_STREAM,0);

    myaddr_in.sin_port = htons(BCL_nListenPort);

    if (BCL_nListenSocket == SOCKET_INVALID)
    {
        mt_print(1,"BCL Server Error --> can't create Socket!\n");
        BCL_nExit = 1;
        BCL_numOfThreads--;
        return MT_THREAD_RET;
    }
    
#ifndef WIN32
    /* Reuse the socket address, so bind doesn't fail*/
    setsockopt (BCL_nListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOptVal, sizeof(int));
#endif /*WIN32*/

    /* Setup for bind */
    myaddr_in.sin_family = AF_INET;
    myaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    for (i=0; i<8; i++)
        myaddr_in.sin_zero[i] = 0;

    /* Bind the socket */
    lResultStatus = bind(BCL_nListenSocket,(struct sockaddr *)&myaddr_in, sizeof(myaddr_in));
    if (lResultStatus == -1)
    {
        mt_print(1,"BCL Server Error --> can't bind Socket!\n");
        close(BCL_nListenSocket);
        BCL_numOfThreads--;
        BCL_nExit = 1;
        return MT_THREAD_RET;
    }                    

    addrlen =  sizeof (struct sockaddr_in);

    /* Wait for data */
    mt_print(0,"Your connection was done. Waiting for client connection\n");
    lResultStatus = listen(BCL_nListenSocket,5);
    if (lResultStatus == -1) 
    {
        mt_print(1,"BCL Server Error --> can't listen on Socket!\n");
        close(BCL_nListenSocket);
        BCL_numOfThreads--;
        BCL_nExit = 1;
        return MT_THREAD_RET;
    }
    /* Non blocking socket.
       The accept() and recv() are not blocking, but imediatelly get the status and continue in the function flow */
    /* ioctl (BCL_nListenSocket, FIONBIO, &nOptVal); */

    while(!BCL_nExit)
    {
        /*************************************
        * In a loop test for connect request.
        * In case connect was requested it creates task for the connect session.
        * At the BCL_RxMsgSocket(), in case there is an active message socket task, the IP of the host
        * connected is test:
        * If from the same host as the active message socket task -
        *   connection might disconnected without informing to target, therefore close old, open new.
        * Else
        *   connection from other board is not allow when another one is active, therefore close the new.
        */
        lMsgSocketTemp = accept(BCL_nListenSocket,(struct sockaddr *)&their_addr,&addrlen);

        if (lMsgSocketTemp != SOCKET_INVALID)
        {
            //mt_print(1,"Comparing Their address:\n" );
            //BCL_IpCompare((MT_UBYTE *)hostAddrNew.sa_data, (MT_UBYTE *)(their_addr->sa_data), /*13*/sizeof (hostAddrNew.sa_data));
            if (lNewMsgSocket != SOCKET_INVALID)
            {
                nHostNameLength = sizeof (hostAddrNew);
                getpeername(lMsgSocketTemp, &hostAddrNew, &nHostNameLength);

                /* Linux Bugfix: if the old socket was closed, the old address gets NULLs, so kill the old thread. */
                if ((BCL_IpCompare((MT_UBYTE *)hostAddrNew.sa_data, (MT_UBYTE *)hostAddrOld.sa_data, /*13*/sizeof (hostAddrNew.sa_data))) == 0 )
                {                    
                    long lMsgSocketPrev = lNewMsgSocket;
                    mt_print(0,"Socket -> Server closing old connection %ld\n",lMsgSocketPrev);
                    //print++;
                    lNewMsgSocket = SOCKET_INVALID; /* replace the socket to use */
                    /* close old socket */
                    close (lMsgSocketPrev);
                   
                }
                else
                {
                    mt_print (0,"Socket -> Server closing new connection \n");
                    close (lMsgSocketTemp);
                    lMsgSocketTemp = SOCKET_INVALID;
                }
            }
            
            /* if we got here, we already closed old socket, just set the new one */
            if (lMsgSocketTemp != SOCKET_INVALID)
            {
                char* IP;
                struct sockaddr_in *ip_sockaddr_in;
                struct in_addr *ip_in_addr;

 #ifndef WIN32
                FILE* fileIP;
#endif                           
                nHostNameLength = sizeof (hostAddrNew);
                getpeername(lMsgSocketTemp, &hostAddrOld, &nHostNameLength);
 
#ifndef WIN32
                IP = (char *)hostAddrOld.sa_data + 2;
                /* Save the connected IP to a file, so that it can be used by BCL tftp/ftp commands */
                fileIP = fopen(__FTP_PATH__"/connected_ip.txt", "w");
                if (fileIP)
                {
                    fprintf(fileIP, "%d.%d.%d.%d\n",
                        (unsigned char)IP[0],(unsigned char)IP[1],(unsigned char)IP[2],(unsigned char)IP[3]);
                    fclose(fileIP);
                }
                else 
                {
                    /* File wasn't created, but ignore this here... */
                }

#endif /*WIN32*/
                ip_sockaddr_in = (struct sockaddr_in *)&hostAddrOld;
                ip_in_addr = (struct in_addr *)&ip_sockaddr_in->sin_addr.s_addr;
                IP = inet_ntoa(*ip_in_addr);
                mt_print(0,"\n[%s] - ", IP);
                mt_print(0,"open new socket %ld\n",lMsgSocketTemp);
                /*** Set socket options ***/
    
                /* Force small messages to be sent with no delay */
                setsockopt (lMsgSocketTemp, IPPROTO_TCP, MT_TCP_NODELAY, (char*)&nOptVal, sizeof(int));
    
                /* Close connection in case connection lost (default - 2 hours quiet to send keepalive message) */
                setsockopt (lMsgSocketTemp, SOL_SOCKET, SO_KEEPALIVE, (char*)&nOptVal, sizeof(int));
                /*ioctl (lMsgSocketTemp, FIONBIO, &nOptVal);*/
                
                /* set the active socket for the threads to use */
                lNewMsgSocket = lMsgSocketTemp;
                lMsgSocketTemp = SOCKET_INVALID;


            }
        }
        /* Delay 10mSec */
        MT_SLEEP(10);
    }
    
    BCL_numOfThreads--;
    
    return MT_THREAD_RET;
}


/*****************************************************************************
*   Function Name:  BCL_RxMsgSocket 
*   Description:   
*           Enables Socket comunication
*   Algorithm: 
            Clear all buffers.
            Test if there is additional (old) message socket task (lNewMsgSocket = SOCKET_INVALID if no task is active).
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
               Handle Rx data flow.
*****************************************************************************/
MT_THREAD_RET_T BCL_RxMsgSocket (void* arg)
{
    int  unRxFreeSpace;
    long lResultStatus;
    long lDataLength;
    char cDataArray[MT_BUFFERSIZE];
    char *pNextData = cDataArray;                /* The command might arrive in several chunks, so use a ptr */

    // unreferenced formal parameter;
    arg = arg;

    /* clear out buffer */
    memset(cDataArray, 0, MT_BUFFERSIZE);
    BCL_numOfThreads++;
#ifndef UNDER_CE
    mt_print(1,"BCL_RxMsgSocket thread = %d\n",getpid());
#endif

    while(!BCL_nExit)
    {   
        /* Get the free space in Rx buffer */
        // TODO: Fix this func for the new ptr, 
        // and it shouldn't be called here: here the buffer is always empty...
        unRxFreeSpace = BCL_RxBuffFreeSpace(); 
        /* Reset the data ptr for a new command */
        pNextData = cDataArray;
        lDataLength = 0;
        if (lCurrentMsgSocket == SOCKET_INVALID)
        {
            MT_SLEEP(10);
        }
        
          lResultStatus = -2;
          while (lResultStatus == -2 && lCurrentMsgSocket == lNewMsgSocket && lCurrentMsgSocket != SOCKET_INVALID && !BCL_nExit) 
          {
              

              lResultStatus = recv(lCurrentMsgSocket, pNextData, unRxFreeSpace, 0);
             
              if (lResultStatus == 0)
              {
                  mt_print (0,"Socket -> Client closed connection \n");
                  /* it may be that the msgsocket was closed from the outside */
                  close (lCurrentMsgSocket);
                  /* if this is also the last socket, clear it */
                  if (lCurrentMsgSocket == lNewMsgSocket) 
                    lNewMsgSocket = SOCKET_INVALID;
                  /* set current to invalid */
                  lCurrentMsgSocket = SOCKET_INVALID;
              }
              else if (lResultStatus != -1) 
              {
                  /* Network communication type is active.
                  Call to put char in the Rx buffer */
                  
                  // Terminate the string
                  lDataLength += lResultStatus;
                  cDataArray[lDataLength] = '\0';
                  
                  mt_print(1,"[%ld,%ld]" , lDataLength, lResultStatus);

                  /* Advance the data ptr for the rest of this command */
                  pNextData = pNextData + lResultStatus;
                  
                  /* subtract from unRxFreeSpace the datasize from the previous recv */
                  unRxFreeSpace -= lResultStatus;
                  
                  /* Handle the command if it is ready, i.e. it contains a newline */
                  if (BCL_IsNewCommandReady((const MT_UBYTE*)cDataArray)) 
                  {
                      BCL_HandlerRx((MT_UBYTE *)cDataArray, (int)lDataLength);
                          BCL_TxSocketsrv();
                  } 
                  else 
                  {
                      /* Stay in this inner loop to try to read the end of the command. */
                      mt_print(1,"\n\n\nRestarting recv (received %s)\n\n\n   ", cDataArray);
                      lResultStatus = -2;
                  }
                  
                  
              }

          }
          

        /* check if the socket was replaced */
        if (lCurrentMsgSocket != lNewMsgSocket)
        {
            mt_print(1,"Replacing socket %ld with %ld\n",lCurrentMsgSocket,lNewMsgSocket);
            /* try to close old socket */
            close(lCurrentMsgSocket);
            mt_print(1,"closed old socket %ld\n",lCurrentMsgSocket);
            
            /* replace current socket */
            lCurrentMsgSocket = lNewMsgSocket;
            /* clear out buffer */
            memset(cDataArray, 0, MT_BUFFERSIZE);
        }
        
    }
    
    BCL_numOfThreads--;
    return MT_THREAD_RET;
}

/*****************************************************************************
*   Function Name:  BCL_IpCompare 
*   Description:   
*           Compare IP address
*   Return Value:   0 - same string
                    1 - different string 
*   Algorithm:  Gets the sa_data[] address (of the sockaddr struct). It holds the
                port number (two first bytes) and the IP address (the successive
                four bytes) of the host that is connected to current task. It starts
                bytes compare from the third bytes on (the IP address).
*****************************************************************************/
MT_BYTE BCL_IpCompare(const MT_UBYTE* string1, const MT_UBYTE* string2, MT_UINT16 len)
{
    MT_UINT16  i;
	char strDbg1[256];
	char strDbg2[256];
	char strTmp[256];

    /* Advance to point at the IP address */
    string1 += 2;
    string2 += 2;
	
	/* Debug code - remove all these s/printfs */
	sprintf (strDbg1,  "New IP: ");
	sprintf (strDbg2,  "Old IP: ");

    for(i = 0; i < (len-2); i++)
    {
        /*mt_print ("i++ %d %d\n",*string1,*string2);*/

		sprintf (strTmp,  "%d.", *string1);
		strcat (strDbg1, strTmp);
		sprintf (strTmp,  "%d.", *string2);
		strcat (strDbg2, strTmp);
        if(*(string1++) != *(string2++)) {
            break; /* Exit from loop is not equal */
		}

		/* Only look at first 4 bytes of IP. */
		if (i == 3) {
			i = (len-2);
			break;
		}
    }
	mt_print(1,"%s \t %s\n", strDbg1, strDbg2);

    if(i == (len-2))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*****************************************************************************
*   Function Name:  BCL_TxSocketsrv 
*   Description:   
*           Enables Socket comunication
*   Input Parameters: 
*        None
*   Output Parameters: 
*       None
*   Return Value:
*       None 
*****************************************************************************/
void BCL_TxSocketsrv(void) 
{
    int  nDataLength = 0;
    long lResultStatus;
    char cDataArray[MT_BUFFERSIZE];

    /*
    * Call to get data from the Tx buffer.
    */
    BCL_HandlerTx((MT_UBYTE *)cDataArray, &nDataLength);
    
    
    if (nDataLength)
    {
        /*
        * There is data in the buffer. Test for active Rx message socket task before handling Tx.
        * Lock tasks to protect socket from Read/Write access together.
        * Send it to the terminal.
        */
        if (lCurrentMsgSocket >= 0)
        {
            mt_print(1,"send msg: '%s'\n",cDataArray);
            /* There is active Rx message socket task */
            lResultStatus = send(lCurrentMsgSocket, cDataArray, nDataLength, 0);
            if (lResultStatus == -1)
            {
                /* The socket was probably killed by the client */
                mt_print(1,"BCL Server Error --> can't send on Socket!\n");
                /* close(lNewMsgSocket); */
            } 
        }
        /* Clear to avoid re-enter in case the UART is active */
        nDataLength = 0;
        /* after we sent the data, check if we were asked to exit */
        if (BCL_nWantToExit)
            BCL_nExit = 1; 
    }
}

/*************************************************************** 
 * The main thread creation function. 
 * This is main in the standalone app, 
 * and a function call when BCLSockServer is built as a library.
 ***************************************************************/
#ifdef _STANDALONE
#ifdef WIN32
int __cdecl main ()
#else
// TFTP server now supported only in Linux
extern void StartTFTPServer(void);
int main (int argc, char* argv[])
#endif // WIN32
#else  /* !_STANDALONE */
int BCL_CreateSocketThreads()
#endif /* _STANDALONE */
{

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    mt_print(1,"BCL Ver. %d.%d.%d\n", BCL_VERSION_MAJOR, BCL_VERSION_MINOR, BCL_VERSION_BUILD);
#else
    // Begin linux socket server threads
    pthread_t BCL_threadIdRxSocketsrv;
    pthread_t BCL_threadIdRxMsgSocket;

    ILOG0_DDD("BCL Ver. %d.%d.%d", BCL_VERSION_MAJOR, BCL_VERSION_MINOR, BCL_VERSION_BUILD);

    if (MTLK_ERR_OK != _mtlk_osdep_log_init(IWLWAV_RTLOG_APP_NAME_BCLSOCKSERVER)) {
      return 1;
    }
    mtlk_osdep_log_enable_stderr_all();

#ifdef CONFIG_WAVE_RTLOG_REMOTE
    if (MTLK_ERR_OK != mtlk_rtlog_app_init(&rtlog_info_data, IWLWAV_RTLOG_APP_NAME_BCLSOCKSERVER)) {
      goto end;
    }
#endif
#endif

#ifndef UNDER_CE
    mt_print(1,"BCL main thread = %d\n",getpid());
#endif

#if defined(WIN32) || !defined(_STANDALONE)
    if (!BCL_Init())
#else
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    if (!BCL_Init( (argc > 1) ? argv[1] : NULL, &rtlog_info_data))
#else
    if (!BCL_Init( (argc > 1) ? argv[1] : NULL))
#endif
#endif /* defined(WIN32) || !defined(_STANDALONE) */
      return 1;

#if defined(WIN32) || !defined(_STANDALONE)
    BCL_nListenPort = MT_SERVER_DEFAULT_PORT;
#else
    BCL_nListenPort = (argc > 2) ? atoi(argv[2]) : MT_SERVER_DEFAULT_PORT;
#endif /* defined(WIN32) || !defined(_STANDALONE) */

    PrintLevel(0);
    /* Initialize semaphore */
        
    BCL_nExit = 0;

#ifdef WIN32

    // Init WinSock
    wVersionRequested = MAKEWORD( 2, 0 );
 
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        mt_print(1,"Winsock couldn't be initialized");
        return 1;
    }

#ifdef UNDER_CE
    CreateThread(NULL,
                 0,
                 BCL_RxSocketsrv,
                 (LPVOID)0,
                 (DWORD)NULL,
                 (LPDWORD)NULL);

    CreateThread(NULL,
                 0,
                 BCL_RxMsgSocket,
                 (LPVOID)0,
                 (DWORD)NULL,
                 (LPDWORD)NULL);

#else // !UNDER_CE
    // Begin windows socket server threads
    _beginthread(BCL_RxSocketsrv, 0, (void*)0);
    _beginthread(BCL_RxMsgSocket, 0, (void*)0);
#endif // UNDER_CE

#else // !WIN32
    /* Disable broken pipe signals - this allows error handling, 
       instead of killing the program on broken pipe*/
    signal(SIGPIPE, SIG_IGN);    
    
    pthread_create(&BCL_threadIdRxSocketsrv, NULL, BCL_RxSocketsrv, (void*)0);
    pthread_create(&BCL_threadIdRxMsgSocket, NULL, BCL_RxMsgSocket, (void*)0);
	StartTFTPServer();

#endif /* WIN32 */

   
#ifdef _STANDALONE
	/* In standalone mode, don't return */
    while (!BCL_nExit ) {
#ifdef UNDER_CE // in CE we can't use Control
        TCHAR c;
        _tprintf(_T("Use <ENTER> key to exit.\n"));
        _tscanf(_T("%c"), &c);
        BCL_nExit = 1;
#else
        MT_SLEEP(10);
#ifndef WIN32
#ifdef CONFIG_WAVE_RTLOG_REMOTE
        if (MTLK_RTLOG_APP_RUNNING != mtlk_rtlog_app_get_terminated_status()) {
          BCL_nExit = 1;
        }
#endif
#endif /* !WIN32 */
#endif // UNDER_CE
    }
    BCL_Exit();
    /* close all sockets to insure exis from sleeping */
    close(BCL_nListenSocket);
    close(lCurrentMsgSocket);
    close(lNewMsgSocket);
    while (BCL_numOfThreads)
    {
        MT_SLEEP(1000);
        mt_print(1,"waiting for threads to die ...\n");
    }
    mt_print(1,"Exiting BCL server\n");

#endif /* _STANDALONE */

#ifndef WIN32
#ifdef CONFIG_WAVE_RTLOG_REMOTE
end:
    mtlk_rtlog_app_cleanup(&rtlog_info_data);
#endif
    _mtlk_osdep_log_cleanup();
#endif /* !WIN32 */

    return 0;
}
