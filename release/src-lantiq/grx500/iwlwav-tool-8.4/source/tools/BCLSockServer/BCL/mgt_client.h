/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* LICENSE_CODE_JUNGO */

#ifndef _MGT_CLIENT_LCL_H_
#define _MGT_CLIENT_LCL_H_

/* Open a connection to the local host, execute a command, receive the 
 * results and close the connection */
int mgt_command(char *cmd, char **result);

#endif
