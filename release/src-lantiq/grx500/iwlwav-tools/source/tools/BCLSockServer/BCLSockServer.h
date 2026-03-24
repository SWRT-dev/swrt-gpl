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


#define BCL_VERSION_MAJOR   2
#define BCL_VERSION_MINOR   8
#define BCL_VERSION_BUILD   6
#define BCL_VERSION_STRING  "2.8.6"

#ifdef UNDER_CE
// NOTE - this value is not strongly correlated to the Windows CE OS version being targeted
#define WINVER _WIN32_WCE
#endif // UNDER_CE

#ifdef WIN32
    // Functions that are slightly different in Windows:
    #define close(fd) closesocket(fd)
    #define ioctl ioctlsocket


    #define MT_SLEEP(delay_in_mSec)   Sleep(delay_in_mSec)

#ifdef UNDER_CE // CE uses CreateThread instead of beginthread
    #define MT_THREAD_RET_T DWORD
    #define MT_THREAD_RET 1 // Nonzero indicates success
#else // !UNDER_CE
    #define MT_THREAD_RET_T void _cdecl
    #define MT_THREAD_RET
#endif

    // TODO: Define semaphores on Windows
    #define MT_SEM_TYPE             int
    #define MT_SEM_INIT(sem,count)
    #define MT_SEM_WAIT(sem)
    #define MT_SEM_POST(sem)
    
#else /* !WIN32 --> linux */

    #define MT_SLEEP(delay_in_mSec)   usleep((delay_in_mSec)*1000)
    #define MT_THREAD_RET_T void*
    #define MT_THREAD_RET NULL
/*
    #define MT_SEM_TYPE             sem_t
    #define MT_SEM_INIT(sem,count)  sem_init(&sem, 0, count)
    #define MT_SEM_WAIT(sem)        sem_wait(&sem)
    #define MT_SEM_POST(sem)        sem_post(&sem)
*/
    #define MT_SEM_TYPE             pthread_mutex_t
    #define MT_SEM_INIT(sem,count)  pthread_mutex_init(&sem, NULL)
    #define MT_SEM_WAIT(sem)        pthread_mutex_lock(&sem)
    #define MT_SEM_POST(sem)        pthread_mutex_unlock(&sem)


#endif /* WIN32 */



/* Socket server definitions */
#define MT_SERVER_DEFAULT_PORT   1111  // TODO: Change this to a legal (high) port. e.g. 20111 but then change BB Studio to be configurable
                                       // Well known ports:    http://www.iana.org/assignments/port-numbers   (20035-20201 Unassigned)

#define MT_COMMAND_MAX_SIZE 4096

#define MT_TCP_NODELAY  0x01
#define MT_BUFFERSIZE MT_BCL_ITEM_LIST_SIZE   //10240
#define MT_SOCK_DEBUG  0 /* For Metalink develop group use only */

#define MT_ACK          0x06    /* Acknowledge                         */
#define MT_BEL          0x07    /* Bell                                */
#define MT_LF           0x0A    /* LF                                  */
#define MT_CR           0x0D    /* CR                                  */
#define MT_NAK          0x15    /* Negative acknowledge                */

typedef enum { BCL_ACK = MT_ACK, BCL_NACK = MT_NAK } BCL_eAckOrNack;    // TODO: Should NACK be nack or bell?





/*** Functions to convert BCL to/from SCL protocol used by Broadband Studio ***/
void BCL_SclRemoveEOL(MT_UBYTE* cDataArray);
void BCL_SclAddEcho(MT_UBYTE* cDataArray);
void BCL_SclAddPrompt(MT_UBYTE* cDataArray);
void BCL_SclAddAckNack(MT_UBYTE* cDataArray, BCL_eAckOrNack AckOrNack);


/*** Handler functions that send commands to BCL and get the results ***/
void BCL_HandlerRx(MT_UBYTE* cDataArray, int lDataLength);
void BCL_HandlerTx(MT_UBYTE* cDataArray, int* nDataLength);


/*** Compare IP address ***/
MT_BYTE BCL_IpCompare(const MT_UBYTE* string1, const MT_UBYTE* string2, MT_UINT16 len);

/*** Check if a new command is ready to be handled by BCL ***/
MT_BYTE BCL_IsNewCommandReady(const MT_UBYTE* cDataArray);

/**** API for using BCLSockServer as a LIB ****/
#ifdef _STANDALONE
#ifdef WIN32
	int __cdecl main ();
#else
    int main (int argc, char* argv[]);
#endif // WIN32
#else  /* !_STANDALONE */

#ifdef __cplusplus
	extern "C" {
#endif

	int BCL_CreateSocketThreads();

#ifdef __cplusplus
	}
#endif

#endif /* _STANDALONE */
