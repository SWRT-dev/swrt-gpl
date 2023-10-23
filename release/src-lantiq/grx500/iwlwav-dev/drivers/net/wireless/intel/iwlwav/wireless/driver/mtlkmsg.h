/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLKMSG_H_
#define _MTLKMSG_H_

#if defined (CPTCFG_IWLWAV_BUS_PCI_PCIE)
#include "mtlk_mmb_msg.h"
#else
#error Unknown HW! Can not include suitable msg.h!
#endif

#endif /* ! _MTLKMSG_H_ */
