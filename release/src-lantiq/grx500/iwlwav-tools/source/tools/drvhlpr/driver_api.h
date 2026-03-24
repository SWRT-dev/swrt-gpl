/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __DRIVER_API_H__
#define __DRIVER_API_H__

#include "compat.h"

int driver_connected(void);
int driver_setup_connection(const char *itrfname);
int driver_ioctl(int id, char *buf, size_t len);
void driver_close_connection(void);

#endif // !__DRIVER_API_H__
