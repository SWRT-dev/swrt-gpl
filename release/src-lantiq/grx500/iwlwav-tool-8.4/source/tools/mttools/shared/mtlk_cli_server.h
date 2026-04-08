/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_CLI_SERVER_H__
#define __MTLK_CLI_SERVER_H__

/*! \file  mtlk_cli_server.h

    \brief MTLK CLI server module.

    The module allows developers to intergate MTLK CLI server functionality
    to their apllications.
*/

#define  MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*! \typedef struct mtlk_cli_srv_internal mtlk_cli_srv_t
    \brief   "Black box" MTLK CLI server object.
*/
typedef struct mtlk_cli_srv_internal mtlk_cli_srv_t;

/*! \typedef struct mtlk_cli_srv_cmd_internal mtlk_cli_srv_cmd_t
    \brief   "Black box" MTLK CLI server command object.
*/
typedef struct mtlk_cli_srv_cmd_internal mtlk_cli_srv_cmd_t;

/*! \typedef struct mtlk_cli_srv_rsp_internal mtlk_cli_srv_rsp_t
    \brief   "Black box" MTLK CLI server response object.
*/
typedef struct mtlk_cli_srv_rsp_internal mtlk_cli_srv_rsp_t;

/*! \typedef struct mtlk_cli_srv_clb_internal mtlk_cli_srv_clb_t
    \brief   "Black box" MTLK CLI server callback object.
*/
typedef struct mtlk_cli_srv_clb_internal mtlk_cli_srv_clb_t;

/*! \typedef void (* mtlk_cli_srv_cmd_clb_f)(mtlk_cli_srv_t           *srv,
                                             const mtlk_cli_srv_cmd_t *cmd,
                                             mtlk_cli_srv_rsp_t       *rsp,
                                             mtlk_handle_t             ctx)
    \brief   MTLK CLI server command handler function prototype.

    This function is called by MTLK CLI server to handle a command received
    from MTLK CLI client.

    \param   srv MTLK CLI server object
    \param   cmd MTLK CLI server command object to use during the parsing
    \param   rsp MTLK CLI server command object to fill with response
    \param   ctx user-defined context previously passed to the 
                 mtlk_cli_srv_register_cmd_clb()
*/
typedef void (__MTLK_IFUNC * mtlk_cli_srv_cmd_clb_f)(mtlk_cli_srv_t           *srv,
                                                     const mtlk_cli_srv_cmd_t *cmd,
                                                     mtlk_cli_srv_rsp_t       *rsp,
                                                     mtlk_handle_t             ctx);

/*! \fn      mtlk_cli_srv_t *mtlk_cli_srv_create(void)
    \brief   Creates MTLK CLI server object.

    This function creates a MTLK CLI server object and initializes its internals.

    \return  pointer to newly created MTLK CLI server object, NULL if failed.
*/

mtlk_cli_srv_t * __MTLK_IFUNC mtlk_cli_srv_create(void);

/*! \fn      int mtlk_cli_srv_start(mtlk_cli_srv_t *srv, 
                                    uint16          port)

    \brief   Starts MTLK CLI server object functionality.

    This function starts MTLK CLI server object functionality. Actually, it creates and 
    binds the MTLK CLI server's socket and starts listen()/accept() loop.

    \param   srv  MTLK CLI server object
    \param   port port for MTLK CLI server to use, 0 means dynamic port allocation

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_srv_start(mtlk_cli_srv_t *srv, 
                                    uint16          port);

/*! \fn      void mtlk_cli_srv_stop(mtlk_cli_srv_t *srv)

    \brief   Stops MTLK CLI server object functionality.

    This function stops MTLK CLI server object functionality. Actually, it cleans up,
    closes and deletes the MTLK CLI server object's socket.

    \param   srv  MTLK CLI server object
*/
void __MTLK_IFUNC mtlk_cli_srv_stop(mtlk_cli_srv_t *srv);

/*! \fn      void mtlk_cli_srv_delete(mtlk_cli_srv_t *srv)

    \brief   Delete MTLK CLI server object.

    This function deletes previously created MTLK CLI server object.

    \param   srv  MTLK CLI server object
*/
void __MTLK_IFUNC mtlk_cli_srv_delete(mtlk_cli_srv_t *srv);

/*! \fn      uint16 __MTLK_IFUNC mtlk_cli_srv_get_bound_port(mtlk_cli_srv_t *srv)

    \brief   Gets a port MTLK CLI server is bound on.

    This function gets a port MTLK CLI server is bound on that is highly usable
    in case of dynamic port allocation.

    \param   srv  MTLK CLI server object

    \warning in case of dynamic port allocation, this function will return the correct
             port number only after the server is succesfully started.

    \return  port the MTLK CLI server is bound on
*/
uint16 __MTLK_IFUNC mtlk_cli_srv_get_bound_port(mtlk_cli_srv_t *srv);

/*! \fn      mtlk_cli_srv_clb_t *mtlk_cli_srv_register_cmd_clb(mtlk_cli_srv_t        *srv,
                                                               const char            *cmd_name,
                                                               mtlk_cli_srv_cmd_clb_f clb,
                                                               mtlk_handle_t          ctx)

    \brief   Registers MTLK CLI command handler.

    This function registers MTLK CLI command handler with the MTLK CLI server.

    \param   srv      MTLK CLI server object
    \param   cmd_name 0-terminated name of command this handler is supposed to take care of.
    \param   clb      callback function will be called for command handling once the desired 
                      command arrives
    \param   ctx      user-defined context to be passed to clb

    \warning a few simultaneous handlers for the same command is allowed. They will be called in a
             order of registrations and each can add its strings to the response. Unfortunately, only
             one (latest) error is stored, but success code (MTLK_ERR_OK) can't override an error code.

    \return  MTLK CLI server callback object, NULL if failed.
*/
mtlk_cli_srv_clb_t * __MTLK_IFUNC mtlk_cli_srv_register_cmd_clb(mtlk_cli_srv_t        *srv,
                                                                const char            *cmd_name,
                                                                mtlk_cli_srv_cmd_clb_f clb,
                                                                mtlk_handle_t          ctx);
/*! \fn      void mtlk_cli_srv_unregister_cmd_clb(mtlk_cli_srv_clb_t *srv_clb)

    \brief   Un-registers MTLK CLI command handler.

    This function un-registers MTLK CLI command handler with the MTLK CLI server.

    \param   srv_clb  MTLK CLI server callback object received from 
             mtlk_cli_srv_register_cmd_clb() API.
*/
void __MTLK_IFUNC mtlk_cli_srv_unregister_cmd_clb(mtlk_cli_srv_clb_t *srv_clb);

/*! \fn      const char* mtlk_cli_srv_cmd_get_name(const mtlk_cli_srv_cmd_t *cmd)

    \brief   Gets command name.

    This function returns command name. 

    \param   rsp      MTLK CLI server response object

    \note    Usually, there is no need to ask for command name within the handler 
             function, since the MTLK CLI server itself takes care of calling the
             hanlers with appropriate commands only.
    \par     Moreover, MTLK CLI server uses a high performance mechanism that most
             likely acts the best while seraching for command's handler and, generally,
             it is not a good idea to create another seletor on top of it.
    \par     However, user can decide to register the same callback functions for 
             different commands and, in this case, the mtlk_cli_srv_cmd_get_name() API
             allows him to distinguish between the different commands.
    \par     Another option to use this API is to control the MTLK CLI server's selector
             by putting the ASSERT or some other control check inside the handler.

    \return  0-terminated command name.
*/
const char* __MTLK_IFUNC mtlk_cli_srv_cmd_get_name(const mtlk_cli_srv_cmd_t *cmd);

/*! \fn      uint32 mtlk_cli_srv_cmd_get_nof_params(const mtlk_cli_srv_cmd_t *cmd)

    \brief   Gets number of command params.

    This function returns a number of parameters received along with the command.

    \param   cmd      MTLK CLI server command object

    \return  number of parameters received
*/
uint32 __MTLK_IFUNC mtlk_cli_srv_cmd_get_nof_params(const mtlk_cli_srv_cmd_t *cmd);

/*! \fn      const char *mtlk_cli_srv_cmd_get_param(const mtlk_cli_srv_cmd_t *cmd, uint32 no)

    \brief   Gets string command parameter by index.

    This function returns a string parameter by its index.

    \param   cmd      MTLK CLI server command object
    \param   no       desired command parameter index

    \return  pointer to 0-terminated parameter string number, NULL if index is out-of-range.
*/
const char * __MTLK_IFUNC mtlk_cli_srv_cmd_get_param(const mtlk_cli_srv_cmd_t *cmd, uint32 no);

/*! \fn      int32 mtlk_cli_srv_cmd_get_param_int(const mtlk_cli_srv_cmd_t *cmd, uint32 no, int *err)

    \brief   Gets numerical command parameter by index.

    This function returns a numerical parameter by its index.

    \param   cmd      MTLK CLI server command object
    \param   no       desired command parameter index
    \param   err      optional, error code if required.

    \return  numerical value of parameter by index, 0 if not found. 
             err (if suplied) returns corresponding error value.
*/
int32 __MTLK_IFUNC mtlk_cli_srv_cmd_get_param_int(const mtlk_cli_srv_cmd_t *cmd, uint32 no, int *err);

/*! \fn      int mtlk_cli_srv_rsp_add_str(mtlk_cli_srv_rsp_t *rsp, const char *str)

    \brief   Adds a string to response.

    This function adds a 0-terminated string to response object.

    \param   rsp      MTLK CLI server response object
    \param   str      0-terminated response string to add.

    \note    Command handler is allowed to add a few strings to the same response.

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_srv_rsp_add_str(mtlk_cli_srv_rsp_t *rsp, const char *str);

/*! \fn      int mtlk_cli_srv_rsp_add_strf(mtlk_cli_srv_rsp_t *rsp, const char *fmt, ...)

    \brief   Formats and adds a string to response.

    This function formats a response string and adds it to response object.

    \param   rsp      MTLK CLI server response object
    \param   fmt      response string format

    \note    Command handler is allowed to add a few strings to the same response.

    \return  MTLK_ERR... code
*/
int __MTLK_IFUNC mtlk_cli_srv_rsp_add_strf(mtlk_cli_srv_rsp_t *rsp, const char *fmt, ...);

/*! \fn      void mtlk_cli_srv_rsp_set_error(mtlk_cli_srv_rsp_t *rsp, int err)

    \brief   Sets response error.

    This function sets error code for response object.

    \param   rsp      MTLK CLI server response object
    \param   err      error code (MTLK_ERR..) to set

    \note    There is no need to ser success code (MTLK_ERR_OK). 
             Moreover, MTLK_ERR_OK won't override a real error code once 
             such an error code has been set by this or an other handler.
*/
void __MTLK_IFUNC mtlk_cli_srv_rsp_set_error(mtlk_cli_srv_rsp_t *rsp, int err);

#define  MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_CLI_SERVER_H__ */
