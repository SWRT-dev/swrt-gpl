/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

