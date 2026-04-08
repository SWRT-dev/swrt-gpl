/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_socket.h"
#include "mtlk_osal.h"

#define LOG_LOCAL_GID   GID_MTLK_SOCKET
#define LOG_LOCAL_FID   1

int __MTLK_IFUNC
mtlk_socket_recv_string (SOCKET s, char *buf, int *size, const char *delim_str)
{
  int res  = MTLK_ERR_UNKNOWN;
  int offs = 0;

  memset(buf, 0, *size);

  while (TRUE) {
    int  tmp;

    if (offs >= *size) {
      res = MTLK_ERR_BUF_TOO_SMALL;
      break;
     }

    tmp = recv(s, &buf[offs], sizeof(buf[offs]), 0);
    if (tmp == 0) {
      /* socket has been disconnected */
      res = MTLK_ERR_NOT_IN_USE;
      break;
    }

    if (buf[offs] == 0 || strchr(delim_str, buf[offs])) {
      buf[offs] = 0;
      res       = MTLK_ERR_OK;
      break;
    }
    
    ++offs;
  }

  *size = offs;

  return res;
}

#define MTLK_RECV_SIZE_STEP  32

int __MTLK_IFUNC
mtlk_socket_recv_alloc_string (SOCKET s, char **psz, int *len, const char *delim_str)
{
  int   res  = MTLK_ERR_UNKNOWN;
  int   size = 0;
  char *buf  = NULL;
  int   offs = 0;

  while (TRUE) {
    /* Memory (re)allocation */
    if (size == offs) {
      char *tmp_buf;

      size += MTLK_RECV_SIZE_STEP;
      tmp_buf = mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_CLI_CLT);
      if (!tmp_buf) {
        res = MTLK_ERR_NO_MEM;
        break;
      }

      if (buf) {
        wave_memcpy(tmp_buf, size, buf, offs);
        mtlk_osal_mem_free(buf);
      }

      buf = tmp_buf;
    }

    if (recv(s, &buf[offs], sizeof(buf[offs]), 0) == 0) {
      /* socket has been disconnected */
      res = MTLK_ERR_NOT_IN_USE;
      break;
    }

    if (buf[offs] == 0 || strchr(delim_str, buf[offs])) {
      buf[offs] = 0;
      res       = MTLK_ERR_OK;
      break;
    }

    ++offs;
  }

  if (res != MTLK_ERR_OK) {
    mtlk_osal_mem_free(buf);
    buf  = NULL;
    offs = 0;
  }

  *psz = buf;
  if (len) {
    *len = offs;
  }

  return res;
}

int __MTLK_IFUNC
mtlk_socket_send_buf (SOCKET s, const void *buf, uint32 size)
{
  int res    = MTLK_ERR_OK;
  int offset = 0;

  while (offset < size) {
    int sres = send(s, ((const char*)buf) + offset, size - offset, 0);
    if (sres < 0) {
      ELOG_D("send error: %d", mtlk_socket_get_last_error());
      res = MTLK_ERR_NOT_READY;
      break;
    }

    offset += sres;
  }

  return res;
}

int __MTLK_IFUNC
mtlk_socket_send_string (SOCKET s, const char *str, BOOL send_trailing_zero)
{
  int size = strlen(str);
  if (send_trailing_zero) {
    ++size;
  }
  return mtlk_socket_send_buf(s, str, size);
}
