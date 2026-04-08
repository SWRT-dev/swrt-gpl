/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __DRIVER_API_H__
#define __DRIVER_API_H__

#include "compat.h"

int driver_connected(void);
int driver_setup_connection(const char *itrfname);
int driver_ioctl(int id, char *buf, size_t len);
void driver_close_connection(void);

#endif // !__DRIVER_API_H__
