/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __COMPAT_H__
#define __COMPAT_H__

#define __PACKED __attribute__((aligned(1), packed))

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#endif // !__COMPAT_H__

