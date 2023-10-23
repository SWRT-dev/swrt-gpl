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
 * LogGrabber protocol
 *
 * Written by: Andrey Fidrya
 *
 */

#ifndef __PROTO_LG_H__
#define __PROTO_LG_H__

int lg_process_next_pkt(cqueue_t *pqueue);

#endif // !__PROTO_LG_H__

