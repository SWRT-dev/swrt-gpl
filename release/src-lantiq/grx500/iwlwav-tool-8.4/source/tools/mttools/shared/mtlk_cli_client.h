/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_CLI_CLIENT_H__
#define __MTLK_CLI_CLIENT_H__

/*! \file  mtlk_cli_client.h

    \brief MTLK CLI client module.

    The module allows developers to create MTLK CLI client apllications.
*/

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \typedef struct mtlk_cli_clt_internal mtlk_cli_clt_t
    \brief   "Black box" MTLK CLI client object.
*/
typedef struct mtlk_cli_clt_internal mtlk_cli_clt_t;

/*! \typedef struct mtlk_cli_rsp_internal mtlk_cli_rsp_t
    \brief   "Black box" MTLK CLI client response object.
*/
typedef struct mtlk_cli_rsp_internal mtlk_cli_rsp_t;

/*! \fn      mtlk_cli_clt_t * mtlk_cli_clt_create(void)
    \brief   Creates MTLK CLI client object.

    This function creates a MTLK CLI client object and initializes its internals.

    \return  pointer to newly created MTLK CLI client object, NULL if failed
*/
mtlk_cli_clt_t * __MTLK_IFUNC mtlk_cli_clt_create(void);

/*! \fn      int mtlk_cli_clt_start(mtlk_cli_clt_t *clt,
                                    uint16          port)
    \brief   Starts MTLK CLI client object functionality.

    This function starts MTLK CLI client object functionality. Actually, it connects 
    the MTLK CLI client object to the appropriate MTLK CLI server.

    \param   clt  MTLK CLI client object
    \param   port port, the desired MTLK CLI server is bound on

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_clt_start(mtlk_cli_clt_t *clt,
                                    uint16          port);

/*! \fn      void mtlk_cli_clt_stop(mtlk_cli_clt_t *clt)

    \brief   Stops MTLK CLI client object functionality.

    This function stops MTLK CLI client object functionality. Actually, it disconnects 
    the MTLK CLI client object from the MTLK CLI server it was connected to.

    \param   clt  MTLK CLI client object
*/
void __MTLK_IFUNC mtlk_cli_clt_stop(mtlk_cli_clt_t *clt);

/*! \fn      void mtlk_cli_clt_delete(mtlk_cli_clt_t *clt)

    \brief   Delete MTLK CLI client object.

    This function deletes previously created MTLK CLI client object.

    \param   clt  MTLK CLI client object
*/
void __MTLK_IFUNC mtlk_cli_clt_delete(mtlk_cli_clt_t *clt);

/*! \fn      mtlk_cli_rsp_t *mtlk_cli_clt_send_cmd(mtlk_cli_clt_t *clt, 
                                                   const char     *cmd)
    \brief   Sends full command string, gets response.

    This function sends full command string to connected MTLK CLI server and
    gets a response received.

    \param   clt  MTLK CLI client object
    \param   cmd  0-terminated full command string

    \warning user is responsible for fomatting the command string as required
             by protocol (see mtlk_cli_defs.h for details), but the function
             takes care about sending the "command done" pattern.

    \return  response object pointer, NULL if failed
*/
mtlk_cli_rsp_t * __MTLK_IFUNC mtlk_cli_clt_send_cmd(mtlk_cli_clt_t *clt, 
                                                    const char     *cmd);

/*! \fn      int mtlk_cli_clt_send_cmd(mtlk_cli_clt_t *clt, 
                                       const char     *cmd_chunk)
    \brief   Sends a command chunk.

    This function sends a command chunk to connected MTLK CLI server. It also takes
    care of delimiters as required by protocol (see mtlk_cli_defs.h for details)

    \param   clt       MTLK CLI client object
    \param   cmd_chunk 0-terminated command chunk string

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_clt_send_cmd_chunk(mtlk_cli_clt_t *clt, 
                                             const char     *cmd_chunk);

/*! \fn      mtlk_cli_rsp_t *mtlk_cli_clt_send_cmd_done(mtlk_cli_clt_t *clt)

    \brief   Sends "command done" pattern.

    This function sends "command done" pattern to connected MTLK CLI server and
    gets a response received.

    \param   clt  MTLK CLI client object

    \warning user is responsible for sending the command itself using the
             mtlk_cli_clt_send_cmd_chunk() API.

    \return  response object pointer, NULL if failed
*/
mtlk_cli_rsp_t * __MTLK_IFUNC mtlk_cli_clt_send_cmd_done(mtlk_cli_clt_t *clt);

/*! \fn      int mtlk_cli_rsp_get_err(mtlk_cli_rsp_t *rsp)

    \brief   Gets response error code.

    This function returns a response error code as it was sent by MTLK CLI server.

    \param   rsp      MTLK CLI client response object

    \return  received MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_rsp_get_err(mtlk_cli_rsp_t *rsp);

/*! \fn      uint32 mtlk_cli_rsp_get_nof_strings(mtlk_cli_rsp_t *rsp)

    \brief   Gets number of response strings.

    This function returns a number of response strings as it was sent by MTLK CLI server.

    \param   rsp      MTLK CLI client response object

    \return  received number of response strings
*/
uint32 __MTLK_IFUNC mtlk_cli_rsp_get_nof_strings(mtlk_cli_rsp_t *rsp);

/*! \fn      const char *mtlk_cli_rsp_get_string(mtlk_cli_rsp_t *rsp, uint32 no)

    \brief   Gets response string by index.

    This function returns a response string by index as it was sent by MTLK CLI server.

    \param   rsp      MTLK CLI client response object
    \param   no       desired response string index

    \return  received response string, NULL if index is out-of-range
*/
const char * __MTLK_IFUNC mtlk_cli_rsp_get_string(mtlk_cli_rsp_t *rsp, uint32 no);

/*! \fn      void mtlk_cli_rsp_release(mtlk_cli_rsp_t *rsp)

    \brief   Releases MTLK CLI client response object.

    This function releases MTLK CLI client response object.

    \param   rsp      MTLK CLI client response object
*/
void __MTLK_IFUNC mtlk_cli_rsp_release(mtlk_cli_rsp_t *rsp);

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_CLI_CLIENT_H__ */
