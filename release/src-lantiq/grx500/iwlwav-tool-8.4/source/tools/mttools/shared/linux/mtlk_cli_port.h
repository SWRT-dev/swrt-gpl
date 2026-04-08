/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_CLI_PORT_H__
#define __MTLK_CLI_PORT_H__

#include "mtlkerr.h"

/*! \file  mtlk_cli_port.h

    \brief MTLK CLI port number import/export.

    MTLK CLI port number import/export auxiliary module. 
    The module allows MTLK CLI server to export the port it bound on, and MTLK CLI client - 
    to get (import) it by a server name.
*/

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \fn      int mtlk_cli_port_export_init(uint16      port_no, 
                                           const char *name)

    \brief   Exports MTLK CLI server port number.

    This function exposes MTLK CLI server port number in a manner it 
    could be then imported by a MTLK CLI client using MTLK CLI server
    name.

    \param   port_no   MTLK CLI Server port
    \param   name      0-treminated MTLK CLI Server name

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_port_export_init(uint16      port_no, 
                                           const char *name);

/*! \fn      int mtlk_cli_port_export_cleanup(void)

    \brief   Cleans up exported MTLK CLI server port number.

    This function finishes exposing of MTLK CLI server port number.
*/
void __MTLK_IFUNC mtlk_cli_port_export_cleanup(void);

/*! \fn      int mtlk_cli_port_import(const char *name, 
                                      uint16     *port_no)

    \brief   Imports MTLK CLI server port number by name.

    This function imports the port number a MTLK CLI server bound on 
    by name. MTLK CLI clients using this function to find out the 
    desired MTLK CLI Server's port number.

    \param   name      0-treminated MTLK CLI Server name
    \param   port_no   pointer to be filled with MTLK CLI Server port

    \return  MTLK_ERR... code
*/
int  __MTLK_IFUNC mtlk_cli_port_import(const char *name, 
                                       uint16     *port_no);

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_CLI_PORT_H__ */
