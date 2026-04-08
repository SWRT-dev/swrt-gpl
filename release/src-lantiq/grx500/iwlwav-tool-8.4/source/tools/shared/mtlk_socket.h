/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __AUX_SOCKET_H__
#define __AUX_SOCKET_H__

/*! \file  mtlk_aux_socket.h

    \brief MTLK socket auxilliary module.

    The module provides a few wrappers around the Socket API
    designed for convenient socket usage and makes the socket
    available for CP development on the platforms support
    Berkley Sockets with small deviations.
*/

#include "mtlkerr.h"

#ifdef WIN32
#include <winsock.h>

#define SOCKET_GET_LAST_ERROR WSAGetLastError()

typedef int  socklen_t;
typedef BOOL reuseaddr_opt_t;

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <errno.h>

typedef int SOCKET;

#define INVALID_SOCKET (-1)
#define closesocket    close

#define SD_SEND    0 /* Shutdown send operations */
#define SD_RECEIVE 1 /* Shutdown receive operations */
#define SD_BOTH    2 /* Shutdown both send and receive operations */

#define SOCKET_GET_LAST_ERROR errno

typedef int reuseaddr_opt_t;

#endif

#define MTLK_SOCKET_IPV4_LO_IP "127.0.0.1"
#define MTLK_SOCKET_IPV6_LO_IP "::1"
#define MTLK_SOCKET_LOCALHOST  "localhost"

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \fn      int mtlk_socket_get_last_error(void)

    \brief   Returns last socket error.

    This function returns OS-dependent last socket error. Could be used for logs.

    \return  OS-dependent last socket error
*/
static __INLINE int
mtlk_socket_get_last_error (void)
{
  return SOCKET_GET_LAST_ERROR;
}

/*! \fn      int mtlk_socket_recv_string(SOCKET s, char *buf, int *size, const char *delim_str)

    \brief   Receives string until delimiter.

    This function receives string from socket until buffer is full or a delimiter received.

    \param   s         socket descriptior
    \param   buf       destination buffer
    \param   size      IN - destination buffer size, OUT - result buffer size
    \param   delim_str 0-terminated strings containing delimiters

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_socket_recv_string(SOCKET s, char *buf, int *size, const char *delim_str);

/*! \fn      int mtlk_socket_recv_alloc_string(SOCKET s, char **psz, int *len, const char *delim_str)

    \brief   Allocates the buffer and receives string until delimiter.

    This function allocates the buffer and receives string from socket until a delimiter received.
    Function re-allocates the buffer every time it becomes full.

    \param   s         socket descriptior
    \param   psz       OUT - variable to store destination buffer pointer in
    \param   len       OUT - result string length
    \param   delim_str 0-terminated strings containing delimiters

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_socket_recv_alloc_string(SOCKET s, char **psz, int *len, const char *delim_str);

/*! \fn      int mtlk_socket_send_buf(SOCKET s, const void *buf, uint32 size)

    \brief   Sends buffer synchronously.

    This function sends the whole buffer.

    \param   s         socket descriptior
    \param   buf       buffer to send
    \param   size      size data to send

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_socket_send_buf(SOCKET s, const void *buf, uint32 size);

/*! \fn      int mtlk_socket_send_string(SOCKET s, const char *str, BOOL send_trailing_zero)

    \brief   Sends 0-terminated string synchronously.

    This function sends the whole string.

    \param   s                  socket descriptior
    \param   str                string to send
    \param   send_trailing_zero TRUE if trailing 0 must be sent, FALSE otherwise

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_socket_send_string(SOCKET s, const char *str, BOOL send_trailing_zero);

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __AUX_SOCKET_H__ */

