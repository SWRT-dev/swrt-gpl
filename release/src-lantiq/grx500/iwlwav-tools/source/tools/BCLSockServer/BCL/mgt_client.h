/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* LICENSE_CODE_JUNGO */

#ifndef _MGT_CLIENT_LCL_H_
#define _MGT_CLIENT_LCL_H_

/* Open a connection to the local host, execute a command, receive the 
 * results and close the connection */
int mgt_command(char *cmd, char **result);

#endif
