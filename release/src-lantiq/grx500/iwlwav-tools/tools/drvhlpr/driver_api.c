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
 * Driver API
 *
 */

#include "mtlkinc.h"

#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <linux/if.h>
#include <linux/wireless.h>

#include "compat.h"
#include "mtlkinc.h"

#include "driver_api.h"
#include "dataex.h"

#define LOG_LOCAL_GID   GID_DRIVER_API
#define LOG_LOCAL_FID   1

#define MAX_IF_CNT            32
#define MAX_FILE_NAME_SIZE    256

typedef struct _network_interface
{
    char name[IFNAMSIZ];
} network_interface;

typedef struct _drv_ctl
{
    int32 cmd;
    int32 datalen;
} drv_ctl;

int gsocket = -1;

int gifcount = 0;
network_interface gifs[MAX_IF_CNT];
int ifconnected = -1;

char itrfname[IFNAMSIZ];

// Taken from BCLSockServer
static BOOL is_wireless_interface (char * name)
{
    DIR *dirp;
    struct dirent *dp;
    DIR *dirphy;
    char filename[MAX_FILE_NAME_SIZE];
    struct dirent *dphy;
    if ((dirp = opendir("/sys/class/ieee80211/")) == NULL) {
        perror("couldn't open '/sys/class/ieee80211/'");
        return FALSE;
    }
    do {
        if ((dp = readdir(dirp)) != NULL) {
            int res = sprintf_s(filename, MAX_FILE_NAME_SIZE , "/sys/class/ieee80211/%s/device/net/", dp->d_name);
            if (res >= MAX_FILE_NAME_SIZE || res < 0) {
                perror("error in snprintf");
                (void) closedir(dirp);
                return FALSE;
            }
            if ((dirphy = opendir(filename)) == NULL) {
                continue;
            }
            do {
                if ((dphy = readdir(dirphy)) != NULL) {
                    if (strcmp(dphy->d_name, name) != 0)
                        continue;
                    (void) closedir(dirphy);
                    (void) closedir(dirp);
                    return TRUE;
                }
            } while (dphy != NULL);
            (void) closedir(dirphy);
        }
    } while (dp != NULL);

    (void) closedir(dirp);
    return FALSE;
}

static int find_interfaces( void )
{
  FILE *f;
  char buffer[1024];
//  char ifname[IFNAMSIZ];
  char *tmp, *tmp2;
  struct iwreq req;

  f = fopen("/proc/net/dev", "r");
  if (!f) {
    // FIXME: make errors more verbose
    ELOG_S("fopen(): %s", strerror(errno));
    return -1;
  }

  // Skipping first two lines...
  if (!fgets(buffer, sizeof(buffer), f)) {
    ELOG_S("fgets(): %s", strerror(errno));
    fclose(f);
    return -1;
  }

  if (!fgets(buffer, sizeof(buffer), f)) {
    perror("fgets()");
    fclose(f);
    return -1;
  }

  // Reading interface descriptions...
  for (gifcount = 0; (gifcount < MAX_IF_CNT) && fgets(buffer, sizeof(buffer), f);)
  {
    memset(itrfname, 0, IFNAMSIZ);

    // Skipping through leading space characters...
    for (tmp = buffer; (*tmp == ' ') && (tmp < (buffer + sizeof(buffer))); tmp++);

    // Getting interface name...
    tmp2 = strstr(tmp, ":");
    if (!tmp2) {
      perror("no ':' in device description");
      fclose(f);
      return -1;
    }
    wave_strncopy(itrfname, tmp, IFNAMSIZ, tmp2 - tmp);

    ILOG1_S("Found network interface \"%s\"", itrfname);

    // Checking if interface supports wireless extensions...
    wave_strcopy(req.ifr_name, itrfname, IFNAMSIZ);
    if (ioctl(gsocket, SIOCGIWNAME, &req) && !is_wireless_interface(itrfname)) {
      ILOG1_S("Interface \"%s\" has no wireless extensions...", itrfname);
      continue;
    } else {
      ILOG1_S("Interface \"%s\" has wireless extensions...", itrfname);
    }

    // Adding interface into our database...
    wave_strcopy(gifs[gifcount].name, itrfname, IFNAMSIZ);
    gifcount++;
  }

  fclose(f);

  return 0;
}

int
driver_connected(void)
{
  return ifconnected != -1;
}

int
driver_setup_connection(const char *itrfname)
{
  int soc;
  int i;
  int found = 0;

  ifconnected = -1;

  soc = socket(AF_INET, SOCK_DGRAM, 0);
  if (INVALID_SOCKET == soc) {
    ELOG_S("socket(): %s", strerror(errno));
    return -1;
  }

  gsocket = soc;

  if (find_interfaces() < 0) {
    goto finish;
  }

  for (i = 0; i < gifcount; ++i) {
    if (!strcmp(gifs[i].name, itrfname)) {
      ifconnected = i;
      found = 1;
      break;
    }
  }

finish:
  if (!found) {
    ELOG_S("Wireless interface \"%s\" not found", itrfname);
    close(gsocket);
    gsocket = -1;
    return -1;
  }

  return 0;
}

int
driver_ioctl(int id, char *buf, size_t len)
{
  int retval;

  ASSERT(ifconnected != -1);

  /*ILOG0_DDD("Calling ioctl %d (0x%X), data size %d", id, id, len);*/

  if ((id >= MTLK_WE_IOCTL_FIRST) && (id <= MTLK_WE_IOCTL_LAST)) {
    struct iwreq wreq;

    /* this is wireless extensions private ioctl */
    memset(&wreq, 0, sizeof(wreq));
    wave_strcopy(wreq.ifr_ifrn.ifrn_name, gifs[ifconnected].name, IFNAMSIZ);
    wreq.u.data.pointer = (caddr_t) buf;
    retval = ioctl(gsocket, id, &wreq);
  } else {
    struct ifreq req;

    memset(&req, 0, sizeof(req));
    wave_strcopy(req.ifr_name, gifs[ifconnected].name, IFNAMSIZ);
    req.ifr_data = (caddr_t) buf;
    retval = ioctl(gsocket, id, &req);
  }
  if (retval == -1) {
    ELOG_DS("Ioctl call failed (code %d): %s", retval, strerror(errno));
    return -1;
  }

  /*ILOG0_D("Ioctl call: success (code %d)", retval);*/
  return retval;
}

void
driver_close_connection(void)
{
  ILOG2_V("close the interface");
  close(gsocket);
}
