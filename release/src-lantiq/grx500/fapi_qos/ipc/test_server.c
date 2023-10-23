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

int32_t
main()
{
	void *pvhandle;
	int32_t iret;
	x_ipc_msg_t xmsg;
	printf("This is server\n");
	printf("Creating Listerner\n");
	pvhandle=ipc_create_listener("server");
	while(1){
		printf("Blocking on rcv\n");
		iret=ipc_recv(pvhandle,&xmsg);
		if(iret<0){
			printf("Receive failed\n");
			return -1;
		}
		printf("From[%s]To[%s]Len[%d]Msg[%s]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize,xmsg.acmsg);
		sleep(5);
		printf("Sending reply\n");
		xmsg.xhdr.unmsgsize=strnlen_s("response", MAX_CL_NAME_LEN);
		if(strncpy_s(xmsg.acmsg, MAX_CL_NAME_LEN, "response", xmsg.xhdr.unmsgsize) != EOK) {
			printf("Error copying string\n");
			return -1;
		}
		ipc_send_reply(pvhandle,&xmsg);
	}
}
