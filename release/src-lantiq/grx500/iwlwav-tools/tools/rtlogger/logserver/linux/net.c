/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * 
 *
 * Networking
 *
 * Written by: Andrey Fidrya
 *
 */

#include "mtlkinc.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "compat.h"

#include "logsrv_utils.h"
#include "net.h"

#define LOG_LOCAL_GID   GID_NET
#define LOG_LOCAL_FID   1

/*****************************************************************************
  **
  ** NAME         create_mother_socket
  **
  ** PARAMETERS   port                Port to listen on
  **              psocket             Pointer to a socket created
  **
  ** RETURNS      0                   Success
  **             -1                   Failure
  **
  ** DESCRIPTION  Creates a TCP/IP socket to listen for incoming connections
  **
  ******************************************************************************/
int
create_mother_socket(const char *addr, uint16 port, int *psocket)
{
  int result = -1; // failure
  int s = -1;
  struct sockaddr_in sa;
  
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    ILOG1_S("Error creating socket: %s", strerror(errno));
    goto end;
  }

  // Errors aren't fatal for these options so return values aren't checked:
  SOCKET_SET_OPT(s, SO_REUSEADDR, 1);
  SOCKET_SET_OPT(s, SO_SNDBUF, MAX_SND_BUF);
  SOCKET_SET_OPT(s, SO_RCVBUF, MAX_RCV_BUF);
  socket_set_linger(s, 0, 0);
  
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  if (!addr) {
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    ILOG0_D("Binding to all interfaces on port %u", port);
  } else {
    if (0 == inet_aton(addr, &sa.sin_addr)) {
      ELOG_S("Invalid address to bind to: %s", addr);
      goto end;
    }
    ILOG0_SD("Binding to %s on port %u", inet_ntoa(sa.sin_addr), port);
  }

  if (bind(s, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    ILOG1_S("bind: %s", strerror(errno));
    goto end;
  }

  if (0 != socket_set_nonblock(s, 1)) {
    ELOG_V("Cannot switch socket to nonblocking mode");
    goto end;
  }

  if (0 != listen(s, 5)) {
    ILOG1_S("listen: %s", strerror(errno));
    goto end;
  }

  result = 0; // success
                                    
end:
  if (result != 0) {
    if (s >= 0) {
      close(s);
      s = -1;
    }
  }

  *psocket = s;
  return result;
}

int
socket_set_opt(int s, int opt, const char *opt_name, int val)
{
  if (setsockopt(s, SOL_SOCKET, opt, (char *) &val, sizeof(val)) < 0){
    ILOG1_SS("setsockopt %s: %s", opt_name, strerror(errno));
    return -1;
  }
  return 0;
}

int
socket_set_linger(int s, int onoff, int linger)
{
  struct linger ld;

  ld.l_onoff = onoff;
  ld.l_linger = linger;
  if (setsockopt(s, SOL_SOCKET, SO_LINGER, (char *) &ld, sizeof(ld)) < 0) {
    ILOG1_S("setsockopt SO_LINGER: %s", strerror(errno));
    return -1;
  }
  return 0;
}

int
socket_set_nonblock(int s, int nonblock)
{
  int flags;

  flags = fcntl(s, F_GETFL, 0);
  
  if (nonblock)
    flags |= O_NONBLOCK;
  else
    flags &= ~(O_NONBLOCK);

  if (fcntl(s, F_SETFL, flags) < 0) {
    ILOG1_S("Setting O_NONBLOCK on a socket: %s", strerror(errno));
    return -1;
  }
  return 0;
}


