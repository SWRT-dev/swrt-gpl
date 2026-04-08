/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_osal.h"
#include "mtlk_cli_defs.h"
#include "mtlk_cli_client.h"
#include "mtlk_socket.h"
#include "mtlkstrtok.h"

#define LOG_LOCAL_GID   GID_CLI
#define LOG_LOCAL_FID   0

struct mtlk_cli_rsp_internal
{
  int           err;
  mtlk_strtok_t tokenizer;
  MTLK_DECLARE_INIT_STATUS;
};

MTLK_INIT_STEPS_LIST_BEGIN(cli_clt_rsp)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_clt_rsp, INIT_TOKENIZER)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_clt_rsp, UNPURGE_TOKENIZER) /* used for cleanup */
MTLK_INIT_INNER_STEPS_BEGIN(cli_clt_rsp)
MTLK_INIT_STEPS_LIST_END(cli_clt_rsp);

static void
_mtlk_cli_rsp_cleanup (mtlk_cli_rsp_t *rsp)
{
  MTLK_CLEANUP_BEGIN(cli_clt_rsp, MTLK_OBJ_PTR(rsp))
    MTLK_CLEANUP_STEP(cli_clt_rsp, UNPURGE_TOKENIZER, MTLK_OBJ_PTR(rsp),
                      mtlk_strtok_purge, (&rsp->tokenizer))
    MTLK_CLEANUP_STEP(cli_clt_rsp, INIT_TOKENIZER, MTLK_OBJ_PTR(rsp),
                      mtlk_strtok_cleanup, (&rsp->tokenizer))
  MTLK_CLEANUP_END(cli_clt_rsp, MTLK_OBJ_PTR(rsp))
}

static int
_mtlk_cli_rsp_init (mtlk_cli_rsp_t *rsp)
{
  rsp->err = MTLK_ERR_OK;
  MTLK_INIT_TRY(cli_clt_rsp, MTLK_OBJ_PTR(rsp))
    MTLK_INIT_STEP(cli_clt_rsp, INIT_TOKENIZER, MTLK_OBJ_PTR(rsp),
                   mtlk_strtok_init, (&rsp->tokenizer))
    MTLK_INIT_STEP_VOID(cli_clt_rsp, UNPURGE_TOKENIZER, MTLK_OBJ_PTR(rsp),
                        MTLK_NOACTION, ())
  MTLK_INIT_FINALLY(cli_clt_rsp, MTLK_OBJ_PTR(rsp))
  MTLK_INIT_RETURN(cli_clt_rsp, MTLK_OBJ_PTR(rsp), _mtlk_cli_rsp_cleanup, (rsp))
}

static int
_mtlk_cli_rsp_recv (mtlk_cli_rsp_t *rsp, SOCKET sd)
{
  int  res = MTLK_ERR_OK;
  char buf[32];
  int  nof_lines = 0;
  char *str = NULL;
  int tmp;

  tmp = sizeof(buf);
  res = mtlk_socket_recv_string(sd, buf, &tmp, MTLK_CLI_PROTOCOL_WORD_DELIM);
  if (res != MTLK_ERR_OK) {
    goto end;
  }
  rsp->err = atoi(buf);

  tmp = sizeof(buf);
  res = mtlk_socket_recv_string(sd, buf, &tmp, MTLK_CLI_PROTOCOL_EOL);
  if (res != MTLK_ERR_OK) {
    goto end;
  }
  nof_lines = atoi(buf);

  while (nof_lines) {
    res = mtlk_socket_recv_alloc_string(sd, &str, NULL, MTLK_CLI_PROTOCOL_EOL);
    if (res != MTLK_ERR_OK) {
      goto end;
    }

    /* Drop out empty tails of previous string caused by 
     * multichar protocol EOL ("\r\n")
     */
    if (str[0] != 0) { 
      res = mtlk_strtok_add_token(&rsp->tokenizer, str);
      if (res != MTLK_ERR_OK) {
        goto end;
      }

      --nof_lines;
    }

    mtlk_osal_mem_free(str);
    str = NULL;
  }

  res = MTLK_ERR_OK;

end:
  if (str) {
     mtlk_osal_mem_free(str);
  }
  return res;
}


int  __MTLK_IFUNC
mtlk_cli_rsp_get_err (mtlk_cli_rsp_t *rsp)
{
  MTLK_ASSERT(rsp != NULL);

  return rsp->err;
}

const char * __MTLK_IFUNC
mtlk_cli_rsp_get_string (mtlk_cli_rsp_t *rsp, uint32 no)
{
  MTLK_ASSERT(rsp != NULL);

  return mtlk_strtok_get_token(&rsp->tokenizer, no);
}

uint32 __MTLK_IFUNC
mtlk_cli_rsp_get_nof_strings (mtlk_cli_rsp_t *rsp)
{
  MTLK_ASSERT(rsp != NULL);

  return mtlk_strtok_get_nof_tokens(&rsp->tokenizer);

}

void __MTLK_IFUNC
mtlk_cli_rsp_release (mtlk_cli_rsp_t *rsp)
{
  MTLK_ASSERT(rsp != NULL);

  _mtlk_cli_rsp_cleanup(rsp);
  mtlk_osal_mem_free(rsp);
}


struct mtlk_cli_clt_internal
{
  SOCKET sd;
  BOOL   cmd_in_progress;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
};

MTLK_INIT_STEPS_LIST_BEGIN(cli_clt)
MTLK_INIT_INNER_STEPS_BEGIN(cli_clt)
MTLK_INIT_STEPS_LIST_END(cli_clt);

MTLK_START_STEPS_LIST_BEGIN(cli_clt)
  MTLK_START_STEPS_LIST_ENTRY(cli_clt, CREATE_SOCKET)
  MTLK_START_STEPS_LIST_ENTRY(cli_clt, CONNECT_SOCKET)
MTLK_START_INNER_STEPS_BEGIN(cli_clt)
  MTLK_START_STEPS_LIST_ENTRY(cli_clt, GET_SRV_ADDR)
MTLK_START_STEPS_LIST_END(cli_clt);

mtlk_cli_clt_t * __MTLK_IFUNC
mtlk_cli_clt_create (void)
{
  mtlk_cli_clt_t *clt = (mtlk_cli_clt_t *)mtlk_osal_mem_alloc(sizeof(*clt), 
                                                              MTLK_MEM_TAG_CLI_CLT);
  if (clt) {
    memset(clt, 0, sizeof(*clt));
  }

  return clt;
}

int __MTLK_IFUNC
mtlk_cli_clt_start (mtlk_cli_clt_t *clt,
                    uint16          port)
{
  int              tmp;
  struct addrinfo  hints;
  struct addrinfo *ares = NULL;
  char             buf[32];

  MTLK_ASSERT(clt  != NULL);
  MTLK_ASSERT(port != 0);

  snprintf(buf, sizeof(buf), "%u", (uint32)port);

  memset(&hints, 0, sizeof(hints));
#ifdef CPTCFG_IPV4_ONLY_CLI
  hints.ai_family   = AF_INET;   /* use IPv4 only */
#else
  hints.ai_family   = AF_UNSPEC; /* use IPv4 or IPv6, whichever */
#endif
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  MTLK_START_TRY(cli_clt,  MTLK_OBJ_PTR(clt))
    MTLK_START_STEP_EX(cli_clt,  GET_SRV_ADDR, MTLK_OBJ_NONE,
                       getaddrinfo, (MTLK_SOCKET_LOCALHOST, buf, &hints, &ares),
                       tmp,
                       tmp == 0,
                       MTLK_ERR_PARAMS);
    MTLK_START_STEP_EX(cli_clt,  CREATE_SOCKET, MTLK_OBJ_PTR(clt),
                       socket, (ares->ai_family, ares->ai_socktype, ares->ai_protocol),
                       clt->sd,
                       clt->sd != INVALID_SOCKET,
                       MTLK_ERR_PROHIB);
    MTLK_START_STEP_EX(cli_clt,  CONNECT_SOCKET, MTLK_OBJ_PTR(clt),
                       connect, (clt->sd, ares->ai_addr, ares->ai_addrlen),
                       tmp,
                       tmp >= 0,
                       MTLK_ERR_NOT_READY);
  MTLK_START_FINALLY(cli_clt, MTLK_OBJ_PTR(clt))
    MTLK_STOP_STEP(cli_clt,  GET_SRV_ADDR, MTLK_OBJ_NONE,
                   freeaddrinfo, (ares));
  MTLK_START_RETURN(cli_clt, MTLK_OBJ_PTR(clt), mtlk_cli_clt_stop, (clt))
}

void __MTLK_IFUNC
mtlk_cli_clt_stop (mtlk_cli_clt_t *clt)
{
  MTLK_ASSERT(clt != NULL);

  MTLK_STOP_BEGIN(cli_clt,  MTLK_OBJ_PTR(clt))
    MTLK_STOP_STEP(cli_clt,  CONNECT_SOCKET, MTLK_OBJ_PTR(clt),
                   shutdown, (clt->sd, SD_BOTH)); /* close for both send & receive */
    MTLK_STOP_STEP(cli_clt,  GET_SRV_ADDR, MTLK_OBJ_PTR(clt),
                   MTLK_NOACTION, ());
    MTLK_STOP_STEP(cli_clt,  CREATE_SOCKET, MTLK_OBJ_PTR(clt),
                   closesocket, (clt->sd));
  MTLK_STOP_END(cli_clt, MTLK_OBJ_PTR(clt))
}

void __MTLK_IFUNC
mtlk_cli_clt_delete (mtlk_cli_clt_t *clt)
{
  MTLK_ASSERT(clt != NULL);

  mtlk_osal_mem_free(clt);
}

static int  _mtlk_cli_rsp_init(mtlk_cli_rsp_t *rsp);
static void _mtlk_cli_rsp_cleanup(mtlk_cli_rsp_t *rsp);
static int  _mtlk_cli_rsp_recv(mtlk_cli_rsp_t *rsp, SOCKET sd);

mtlk_cli_rsp_t * __MTLK_IFUNC
mtlk_cli_clt_send_cmd (mtlk_cli_clt_t *clt, 
                       const char     *cmd)
{
  int res = mtlk_cli_clt_send_cmd_chunk(clt, cmd);
  return (res == MTLK_ERR_OK)?mtlk_cli_clt_send_cmd_done(clt):NULL;
}

int __MTLK_IFUNC
mtlk_cli_clt_send_cmd_chunk (mtlk_cli_clt_t *clt, 
                             const char     *cmd_chunk)
{
  int res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(clt != NULL);
  MTLK_ASSERT(cmd_chunk != NULL);

  if (clt->cmd_in_progress) {
    res = mtlk_socket_send_string(clt->sd, MTLK_CLI_PROTOCOL_WORD_DELIM, FALSE);
    if (res != MTLK_ERR_OK) {
      goto end;
    }
  }

  clt->cmd_in_progress = TRUE;
  res = mtlk_socket_send_string(clt->sd, cmd_chunk, FALSE);
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  res = MTLK_ERR_OK;

end:
  return res;
}

mtlk_cli_rsp_t * __MTLK_IFUNC
mtlk_cli_clt_send_cmd_done (mtlk_cli_clt_t *clt)
{
  int             res  = MTLK_ERR_UNKNOWN;
  mtlk_cli_rsp_t *resp = NULL;
  int             resp_inited = 0;

  clt->cmd_in_progress = FALSE;

  res = mtlk_socket_send_string(clt->sd, MTLK_CLI_PROTOCOL_EOL, FALSE); /* send END-OF-CMD char */
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  resp = mtlk_osal_mem_alloc(sizeof(*resp), MTLK_MEM_TAG_CLI_CLT);
  if (!resp) {
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  res = _mtlk_cli_rsp_init(resp);
  if (res != MTLK_ERR_OK) {
    goto end;
  }
  resp_inited = 1;

  res = _mtlk_cli_rsp_recv(resp, clt->sd);
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  res = MTLK_ERR_OK;
  
end:
  if (res != MTLK_ERR_OK && resp) {
    if (resp_inited) {
      _mtlk_cli_rsp_cleanup(resp);
    }
    mtlk_osal_mem_free(resp);
    resp = NULL;
  }

  return resp;
}
