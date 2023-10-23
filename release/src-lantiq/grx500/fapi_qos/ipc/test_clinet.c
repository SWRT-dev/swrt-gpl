/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <commondefs.h>
#include <qosal_ipc.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

main()
{
	x_ipc_msg_t xmsg;
	if(strncpy_s(xmsg.xhdr.aucfrom, MAX_CL_NAME_LEN, "client", strnlen_s("client", MAX_CL_NAME_LEN)) != EOK) {
		printf("Error copying string\n");
		return;
	}
	if(strncpy_s(xmsg.xhdr.aucto, MAX_CL_NAME_LEN, "server", strnlen_s("server", MAX_CL_NAME_LEN)) != EOK) {
                printf("Error copying string\n");
                return;
        }
	xmsg.xhdr.unmsgsize=strnlen_s("request", MAX_CL_NAME_LEN);
	if(strncpy_s(xmsg.acmsg, MAX_CL_NAME_LEN, "request", xmsg.xhdr.unmsgsize) != EOK) {
                printf("Error copying string\n");
                return;
        }
	printf("This is clinet\n");
	ipc_send_request(&xmsg);
	printf("From[%s]To[%s]Len[%d]Msg[%s]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize,xmsg.acmsg);
}
