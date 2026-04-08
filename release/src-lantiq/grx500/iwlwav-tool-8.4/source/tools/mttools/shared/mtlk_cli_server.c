/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_osal.h"
#include "mtlk_cli_server.h"
#include "mtlk_cli_defs.h"
#include "mtlklist.h"
#include "mtlkhash.h"
#include "mtlk_socket.h"
#include "mtlkstrtok.h"

#include <stdarg.h>

#define LOG_LOCAL_GID   GID_MTLK_CLI_SERVER
#define LOG_LOCAL_FID   1

#define MTLK_CLI_MAX_BACKLOG  10 /* the number of connections allowed on the incoming queue */
#define MTLK_CLI_HASH_MAX_KEY 10

struct mtlk_cli_srv_internal
{
  SOCKET              sd;
  uint16              port;
  mtlk_osal_thread_t  main_thread;
  volatile int        main_thread_stop;
  int                 error_detected;
  mtlk_mhash_t        clbs;
  mtlk_osal_mutex_t   clbs_lock;
  mtlk_cli_srv_clb_t *internal_ls_clb;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
};

/******************************************************************************
 * CLI Server's peer object API
 ******************************************************************************/
typedef struct mtlk_cli_peer_internal
{
  SOCKET             sd;
  mtlk_osal_thread_t thread;
  volatile int       thread_stop;
  volatile int       thread_running;
  mtlk_cli_srv_t    *srv;
  mtlk_slist_entry_t lentry;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
} mtlk_cli_peer_t;

static int  _mtlk_cli_peer_init(mtlk_cli_peer_t *peer, SOCKET sd, mtlk_cli_srv_t *srv);
static int  _mtlk_cli_peer_start(mtlk_cli_peer_t *peer);
static void _mtlk_cli_peer_stop(mtlk_cli_peer_t *peer);
static void _mtlk_cli_peer_cleanup(mtlk_cli_peer_t *peer);

static __INLINE int
_mtlk_cli_peer_is_running (mtlk_cli_peer_t *peer)
{
  return peer->thread_running;
}
/******************************************************************************/

/******************************************************************************
 * CLI Server's command callback related definitions
 ******************************************************************************/
typedef char * mtlk_cli_srv_key;

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

MTLK_MHASH_DECLARE_ENTRY_T(cli_srv, mtlk_cli_srv_key);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

struct mtlk_cli_srv_clb_internal
{
  char                       *cmd_name;
  mtlk_cli_srv_cmd_clb_f      clb;
  mtlk_handle_t               ctx;
  mtlk_cli_srv_t             *srv;
  MTLK_MHASH_ENTRY_T(cli_srv) hentry;
};

MTLK_MHASH_DECLARE_INLINE(cli_srv, mtlk_cli_srv_key);

static __INLINE uint32
_mtlk_cli_srv_hash_hash (const mtlk_cli_srv_key *key, uint32 nof_buckets)
{
  MTLK_UNREFERENCED_PARAM(nof_buckets);

  uint8 hash_char = (uint8)((*key)[0]);
  return (uint32)(hash_char % MTLK_CLI_HASH_MAX_KEY);
}

static __INLINE int
_mtlk_cli_srv_hash_key_cmp (const mtlk_cli_srv_key *key1,
                            const mtlk_cli_srv_key *key2)
{
  return strcmp(*key1, *key2);
}

MTLK_MHASH_DEFINE_INLINE(cli_srv, 
                         mtlk_cli_srv_key,
                         _mtlk_cli_srv_hash_hash,
                         _mtlk_cli_srv_hash_key_cmp);

/******************************************************************************/

/******************************************************************************
 * CLI Server object
 ******************************************************************************/
static void
_mtlk_cli_srv_cleanup_peers_list (mtlk_slist_t *peers_list,
                                  BOOL          only_stopped)
{

  while (1) {
    mtlk_slist_entry_t *peer_prev_entry = NULL;
    mtlk_cli_peer_t    *peer            = NULL;
    mtlk_slist_entry_t *entry;
    mtlk_slist_entry_t *prev_entry;

    mtlk_slist_foreach(peers_list, entry, prev_entry) {
      peer = MTLK_CONTAINER_OF(entry, mtlk_cli_peer_t, lentry);

      if (!only_stopped || !_mtlk_cli_peer_is_running(peer)) {
        /* WARNING: mtlk_slist_remove_next() can't be used under 
         *          mtlk_slist_foreach() since it breaks the loop.
         *          Thus, we must:
         *           -  first find an entry using mtlk_slist_foreach()
         *           -  exit the mtlk_slist_foreach() loop
         *           -  delete the element
         */
        peer_prev_entry = prev_entry;
        break;
      }  
    }

    if (!peer_prev_entry) {
      break;
    }

    mtlk_slist_remove_next(peers_list, peer_prev_entry);

    _mtlk_cli_peer_stop(peer);
    mtlk_osal_mem_free(peer);
  }
}

static __MTLK_IFUNC int32
_mtlk_cli_srv_main_thread_proc (mtlk_handle_t context)
{
  mtlk_cli_srv_t *srv = HANDLE_T_PTR(mtlk_cli_srv_t, context);
  mtlk_slist_t    peers_list;

  mtlk_slist_init(&peers_list);

  while (1) {
    SOCKET           sd;
    struct sockaddr  addr;
    socklen_t        addr_size = (socklen_t)sizeof(addr);
    int              res;
    mtlk_cli_peer_t *peer = NULL;
    int              peer_inited = 0;

    res = listen(srv->sd, MTLK_CLI_MAX_BACKLOG);
    if (res == -1) {
      srv->error_detected = mtlk_socket_get_last_error();
      break;
    }

    if (srv->main_thread_stop) {
      break;
    }

    sd = accept(srv->sd, &addr, &addr_size);
    if (sd == INVALID_SOCKET) {
      WLOG_D("Can't accept socket (err=%d)", mtlk_socket_get_last_error());
      goto loop;
    }

    peer = (mtlk_cli_peer_t *)mtlk_osal_mem_alloc(sizeof(*peer),
                                                  MTLK_MEM_TAG_CLI_SRV);
    if (!peer) {
      ELOG_V("Can't allocate peer");
      close(sd);
      sd = INVALID_SOCKET;
      goto loop;
    }

    res = _mtlk_cli_peer_init(peer, sd, srv);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Can't initialize peer (err=%d)", res);
      goto loop;
    }
    peer_inited = 1;

    res = _mtlk_cli_peer_start(peer);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Can't start peer (err=%d)", res);
      goto loop;
    }

    mtlk_slist_push(&peers_list, &peer->lentry);
    peer = NULL; /* peer is added to list => prevent peer cleanup */

loop:
    _mtlk_cli_srv_cleanup_peers_list(&peers_list, TRUE);

    if (peer) {
      /* peer was allocated but wasn't added to list => cleanup peer */
      if (peer_inited) {
        _mtlk_cli_peer_cleanup(peer);
      }
      mtlk_osal_mem_free(peer);
    }
  }

  _mtlk_cli_srv_cleanup_peers_list(&peers_list, FALSE);
  mtlk_slist_cleanup(&peers_list);

  return 0;
}

static int
_mtlk_cli_srv_handle_cmd (mtlk_cli_srv_t     *srv, 
                          mtlk_cli_srv_cmd_t *cmd,
                          mtlk_cli_srv_rsp_t *rsp)
{
  int                          res = MTLK_ERR_NOT_SUPPORTED;
  mtlk_mhash_find_t            f;
  MTLK_MHASH_ENTRY_T(cli_srv) *h;
  mtlk_cli_srv_key             key = (mtlk_cli_srv_key)mtlk_cli_srv_cmd_get_name(cmd); 

  mtlk_osal_mutex_acquire(&srv->clbs_lock);

  h = mtlk_mhash_find_first_cli_srv(&srv->clbs, 
                                    &key,
                                    &f);
  while (h) {
    mtlk_cli_srv_clb_t *srv_clb = 
      MTLK_CONTAINER_OF(h, mtlk_cli_srv_clb_t, hentry);
    srv_clb->clb(srv, cmd, rsp, srv_clb->ctx);
    res = MTLK_ERR_OK;
    h   = mtlk_mhash_find_next_cli_srv(&srv->clbs, 
                                       &key,
                                       &f);
  }

  mtlk_osal_mutex_release(&srv->clbs_lock);  

  return res;
}

static int
_mtlk_cli_srv_get_bound_port (mtlk_cli_srv_t *srv)
{
  int res = MTLK_ERR_OK;

  if (!srv->port) {
    struct sockaddr  bound_addr = {0};
    socklen_t        bound_addrlen = sizeof(bound_addr);
    int              tmp;

    tmp = getsockname(srv->sd, &bound_addr, &bound_addrlen);
    if (tmp == 0) {
      if (bound_addr.sa_family == AF_INET) {
        struct sockaddr_in *addr = (struct sockaddr_in *)&bound_addr;
        srv->port = ntohs(addr->sin_port);
      }
      else {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&bound_addr;
        srv->port = ntohs(addr->sin6_port);
      }
    }
    else {
      res = MTLK_ERR_UNKNOWN;
    }
  }
  return res;
}

static void __MTLK_IFUNC
_mtlk_cli_srv_internal_ls_clb (mtlk_cli_srv_t           *srv,
                               const mtlk_cli_srv_cmd_t *cmd,
                               mtlk_cli_srv_rsp_t       *rsp,
                               mtlk_handle_t             ctx)
{
  mtlk_mhash_enum_t            e;
  MTLK_MHASH_ENTRY_T(cli_srv) *h;

  MTLK_UNREFERENCED_PARAM(ctx);

  /* NOTE: there is no need for lock (srv->clbs_lock) here, since 
   *       callbacks are called under it.
   */
  h = mtlk_mhash_enum_first_cli_srv(&srv->clbs, &e);
  while (h) {
    mtlk_cli_srv_clb_t *srv_clb = 
      MTLK_CONTAINER_OF(h, mtlk_cli_srv_clb_t, hentry);
    if (strcmp(srv_clb->cmd_name, MTLK_CLI_LS_CMD)) { /* skip internal LS command itself */
      mtlk_cli_srv_rsp_add_str(rsp, srv_clb->cmd_name);
    }
    h = mtlk_mhash_enum_next_cli_srv(&srv->clbs, &e);
  }
}


MTLK_INIT_STEPS_LIST_BEGIN(cli_srv)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv, INIT_MAIN_THREAD)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv, INIT_HASH)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv, INIT_HASH_LOCK)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv, REGISTER_INTERNAL_LS)
MTLK_INIT_INNER_STEPS_BEGIN(cli_srv)
MTLK_INIT_STEPS_LIST_END(cli_srv);

MTLK_START_STEPS_LIST_BEGIN(cli_srv)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv, CREATE_SOCKET)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv, BIND_SOCKET)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv, GET_BOUND_PORT)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv, RUN_MAIN_THREAD)
MTLK_START_INNER_STEPS_BEGIN(cli_srv)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv, GET_OWN_ADDR)
MTLK_START_STEPS_LIST_END(cli_srv);

static void
_mtlk_cli_srv_cleanup (mtlk_cli_srv_t *srv)
{
  MTLK_CLEANUP_BEGIN(cli_srv,  MTLK_OBJ_PTR(srv))
    MTLK_CLEANUP_STEP(cli_srv,  REGISTER_INTERNAL_LS, MTLK_OBJ_PTR(srv),
                      mtlk_cli_srv_unregister_cmd_clb, (srv->internal_ls_clb));
    MTLK_CLEANUP_STEP(cli_srv,  INIT_HASH_LOCK, MTLK_OBJ_PTR(srv),
                      mtlk_osal_mutex_cleanup, (&srv->clbs_lock));
    MTLK_CLEANUP_STEP(cli_srv,  INIT_HASH, MTLK_OBJ_PTR(srv),
                      mtlk_mhash_cleanup_cli_srv, (&srv->clbs));
    MTLK_CLEANUP_STEP(cli_srv,  INIT_MAIN_THREAD, MTLK_OBJ_PTR(srv),
                      mtlk_osal_thread_cleanup, (&srv->main_thread));
  MTLK_CLEANUP_END(cli_srv,  MTLK_OBJ_PTR(srv))
}

static int
_mtlk_cli_srv_init (mtlk_cli_srv_t *srv)
{
  MTLK_INIT_TRY(cli_srv,  MTLK_OBJ_PTR(srv))
    MTLK_INIT_STEP(cli_srv,  INIT_MAIN_THREAD, MTLK_OBJ_PTR(srv),
                   mtlk_osal_thread_init, (&srv->main_thread));
    MTLK_INIT_STEP(cli_srv,  INIT_HASH, MTLK_OBJ_PTR(srv),
                   mtlk_mhash_init_cli_srv, (&srv->clbs, MTLK_CLI_HASH_MAX_KEY));
    MTLK_INIT_STEP(cli_srv,  INIT_HASH_LOCK, MTLK_OBJ_PTR(srv),
                   mtlk_osal_mutex_init, (&srv->clbs_lock));
    MTLK_INIT_STEP_EX(cli_srv,  REGISTER_INTERNAL_LS, MTLK_OBJ_PTR(srv),
                      mtlk_cli_srv_register_cmd_clb, (srv, 
                                                      MTLK_CLI_LS_CMD, 
                                                      _mtlk_cli_srv_internal_ls_clb, 
                                                      HANDLE_T(srv)),
                      srv->internal_ls_clb,
                      srv->internal_ls_clb != NULL,
                      MTLK_ERR_NO_MEM);
  MTLK_INIT_FINALLY(cli_srv,  MTLK_OBJ_PTR(srv))
  MTLK_INIT_RETURN(cli_srv,  MTLK_OBJ_PTR(srv), _mtlk_cli_srv_cleanup, (srv))
}

mtlk_cli_srv_t * __MTLK_IFUNC
mtlk_cli_srv_create (void)
{
  mtlk_cli_srv_t *srv = 
    (mtlk_cli_srv_t *)mtlk_osal_mem_alloc(sizeof(*srv), 
                                          MTLK_MEM_TAG_CLI_SRV);
  if (srv) {
    int res;

    memset(srv, 0, sizeof(*srv));

    res = _mtlk_cli_srv_init(srv);
    if (res != MTLK_ERR_OK) {
      mtlk_osal_mem_free(srv);
      srv = NULL;
    }
  }

  return srv;
}

int __MTLK_IFUNC
mtlk_cli_srv_start (mtlk_cli_srv_t *srv, uint16 port)
{
  int                tmp;
  struct addrinfo    hints;
  struct addrinfo   *ares;
  char               buf[32];

  MTLK_ASSERT(srv  != NULL);

  snprintf(buf, sizeof(buf), "%u", (uint32)port);

  memset(&hints, 0, sizeof(hints));
#ifdef CPTCFG_IPV4_ONLY_CLI
  hints.ai_family   = AF_INET;    /* use IPv4 only */
#else
  hints.ai_family   = AF_UNSPEC;  /* use IPv4 or IPv6, whichever */
#endif
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE; /* fill in my IP */
  hints.ai_protocol = IPPROTO_TCP;

  srv->main_thread_stop = 0;
  srv->port             = port;

  MTLK_START_TRY(cli_srv,  MTLK_OBJ_PTR(srv))
    MTLK_START_STEP_EX(cli_srv,  GET_OWN_ADDR, MTLK_OBJ_NONE,
                       getaddrinfo, (MTLK_SOCKET_LOCALHOST, buf, &hints, &ares),
                       tmp,
                       tmp == 0,
                       MTLK_ERR_PARAMS);
    MTLK_START_STEP_EX(cli_srv,  CREATE_SOCKET, MTLK_OBJ_PTR(srv),
                       socket, (ares->ai_family, ares->ai_socktype, ares->ai_protocol),
                       srv->sd,
                       srv->sd != INVALID_SOCKET,
                       MTLK_ERR_PROHIB);
    MTLK_START_STEP_EX(cli_srv,  BIND_SOCKET, MTLK_OBJ_PTR(srv),
                       bind, (srv->sd, ares->ai_addr, ares->ai_addrlen),
                       tmp,
                       tmp == 0,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_IF(!port, cli_srv,  GET_BOUND_PORT, MTLK_OBJ_PTR(srv),
                       _mtlk_cli_srv_get_bound_port, (srv));
    MTLK_START_STEP(cli_srv,  RUN_MAIN_THREAD, MTLK_OBJ_PTR(srv),
                    mtlk_osal_thread_run, (&srv->main_thread, 
                                           _mtlk_cli_srv_main_thread_proc, 
                                           HANDLE_T(srv)))
  MTLK_START_FINALLY(cli_srv, MTLK_OBJ_PTR(srv))
    MTLK_STOP_STEP(cli_srv,  GET_OWN_ADDR, MTLK_OBJ_NONE,
                   freeaddrinfo, (ares));
  MTLK_START_RETURN(cli_srv, MTLK_OBJ_PTR(srv), mtlk_cli_srv_stop, (srv))
}

void __MTLK_IFUNC
mtlk_cli_srv_stop (mtlk_cli_srv_t *srv)
{
  MTLK_ASSERT(srv  != NULL);

  srv->port             = 0;
  srv->main_thread_stop = 1;
  shutdown(srv->sd, SD_BOTH); /* cancel any blocked socket I/O */

  MTLK_STOP_BEGIN(cli_srv,  MTLK_OBJ_PTR(srv))
    MTLK_STOP_STEP(cli_srv,  RUN_MAIN_THREAD, MTLK_OBJ_PTR(srv),
                   mtlk_osal_thread_wait, (&srv->main_thread, NULL))
    MTLK_STOP_STEP(cli_srv,  GET_BOUND_PORT, MTLK_OBJ_PTR(srv),
                   MTLK_NOACTION, ());
    MTLK_STOP_STEP(cli_srv,  BIND_SOCKET, MTLK_OBJ_PTR(srv),
                   MTLK_NOACTION, ());
    MTLK_STOP_STEP(cli_srv,  CREATE_SOCKET, MTLK_OBJ_PTR(srv),
                   closesocket, (srv->sd));
    MTLK_STOP_STEP(cli_srv,  GET_OWN_ADDR, MTLK_OBJ_PTR(srv),
                   MTLK_NOACTION, ());
  MTLK_STOP_END(cli_srv,  MTLK_OBJ_PTR(srv))
}

void __MTLK_IFUNC
mtlk_cli_srv_delete (mtlk_cli_srv_t *srv)
{
  MTLK_ASSERT(srv != NULL);
  _mtlk_cli_srv_cleanup(srv);
  mtlk_osal_mem_free(srv);
}

uint16 __MTLK_IFUNC
mtlk_cli_srv_get_bound_port (mtlk_cli_srv_t *srv)
{
  MTLK_ASSERT(srv != NULL);
  return srv->port;
}

mtlk_cli_srv_clb_t * __MTLK_IFUNC
mtlk_cli_srv_register_cmd_clb (mtlk_cli_srv_t        *srv,
                               const char            *cmd_name,
                               mtlk_cli_srv_cmd_clb_f clb,
                               mtlk_handle_t          ctx)
{
  int                 res     = MTLK_ERR_UNKNOWN;
  mtlk_cli_srv_clb_t *srv_clb = NULL;
  int                 size;

  MTLK_ASSERT(srv != NULL);
  MTLK_ASSERT(cmd_name != NULL);
  MTLK_ASSERT(clb != NULL);

  srv_clb = (mtlk_cli_srv_clb_t *)mtlk_osal_mem_alloc(sizeof(*srv_clb),
                                                      MTLK_MEM_TAG_CLI_SRV);

  if (!srv_clb) {
    goto end;
  }

  size              = strlen(cmd_name) + 1;
  srv_clb->cmd_name = (char *)mtlk_osal_mem_alloc(size,
                                                  MTLK_MEM_TAG_CLI_SRV);
  if (!srv_clb->cmd_name) {
    goto end;
  }
  wave_memcpy(srv_clb->cmd_name, size, cmd_name, size);

  srv_clb->clb = clb;
  srv_clb->ctx = ctx;
  srv_clb->srv = srv;

  mtlk_osal_mutex_acquire(&srv->clbs_lock);
  mtlk_mhash_insert_cli_srv(&srv->clbs, 
                            (mtlk_cli_srv_key *)&srv_clb->cmd_name,
                            &srv_clb->hentry);
  mtlk_osal_mutex_release(&srv->clbs_lock);

  res = MTLK_ERR_OK;

end:
  if (res != MTLK_ERR_OK) {
    if (srv_clb) {

      if (srv_clb->cmd_name) {
        mtlk_osal_mem_free(srv_clb->cmd_name);
      }

      mtlk_osal_mem_free(srv_clb);
      srv_clb = NULL;
    }
  }

  return srv_clb;
}

void __MTLK_IFUNC
mtlk_cli_srv_unregister_cmd_clb (mtlk_cli_srv_clb_t *srv_clb)
{
  MTLK_ASSERT(srv_clb != NULL);
  MTLK_ASSERT(srv_clb->srv != NULL);

  mtlk_osal_mutex_acquire(&srv_clb->srv->clbs_lock);
  mtlk_mhash_remove_cli_srv(&srv_clb->srv->clbs, &srv_clb->hentry);
  mtlk_osal_mutex_release(&srv_clb->srv->clbs_lock);

  mtlk_osal_mem_free(srv_clb->cmd_name);
  mtlk_osal_mem_free(srv_clb);
}
/******************************************************************************/

/******************************************************************************
 * CLI Server's received command object
 ******************************************************************************/
struct mtlk_cli_srv_cmd_internal
{
  mtlk_strtok_t tokenizer;
  MTLK_DECLARE_INIT_STATUS;
};

MTLK_INIT_STEPS_LIST_BEGIN(cli_srv_cmd)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv_cmd, INIT_TOKENIZER)
 MTLK_INIT_STEPS_LIST_ENTRY(cli_srv_cmd, PARSE_CMD_STR)
MTLK_INIT_INNER_STEPS_BEGIN(cli_srv_cmd)
MTLK_INIT_STEPS_LIST_END(cli_srv_cmd);

static void
_mtlk_cli_srv_cmd_cleanup (mtlk_cli_srv_cmd_t *cmd)
{
  MTLK_CLEANUP_BEGIN(cli_srv_cmd, MTLK_OBJ_PTR(cmd))
    MTLK_CLEANUP_STEP(cli_srv_cmd, PARSE_CMD_STR, MTLK_OBJ_PTR(cmd),
                      mtlk_strtok_purge, (&cmd->tokenizer))
    MTLK_CLEANUP_STEP(cli_srv_cmd, INIT_TOKENIZER, MTLK_OBJ_PTR(cmd),
                      mtlk_strtok_cleanup, (&cmd->tokenizer))
  MTLK_CLEANUP_END(cli_srv_cmd, MTLK_OBJ_PTR(cmd))
}

static int
_mtlk_cli_srv_cmd_init (mtlk_cli_srv_cmd_t *cmd,
                        const char         *cmd_str)
{
  MTLK_INIT_TRY(cli_srv_cmd, MTLK_OBJ_PTR(cmd))
    MTLK_INIT_STEP(cli_srv_cmd, INIT_TOKENIZER, MTLK_OBJ_PTR(cmd),
                   mtlk_strtok_init, (&cmd->tokenizer))
    MTLK_INIT_STEP(cli_srv_cmd, PARSE_CMD_STR, MTLK_OBJ_PTR(cmd),
                   mtlk_strtok_parse, (&cmd->tokenizer, cmd_str, ' '))
  MTLK_INIT_FINALLY(cli_srv_cmd, MTLK_OBJ_PTR(cmd))
  MTLK_INIT_RETURN(cli_srv_cmd, MTLK_OBJ_PTR(cmd), _mtlk_cli_srv_cmd_cleanup, (cmd))
}

const char* __MTLK_IFUNC
mtlk_cli_srv_cmd_get_name (const mtlk_cli_srv_cmd_t *cmd)
{
  MTLK_ASSERT(cmd != NULL);

  return mtlk_strtok_get_token(&cmd->tokenizer, 0);
}

uint32 __MTLK_IFUNC
mtlk_cli_srv_cmd_get_nof_params (const mtlk_cli_srv_cmd_t *cmd)
{
  MTLK_ASSERT(cmd != NULL);

  return mtlk_strtok_get_nof_tokens(&cmd->tokenizer) - 1;
}

const char* __MTLK_IFUNC
mtlk_cli_srv_cmd_get_param (const mtlk_cli_srv_cmd_t *cmd, uint32 no)
{
  MTLK_ASSERT(cmd != NULL);

  return mtlk_strtok_get_token(&cmd->tokenizer, (uint32)(no + 1));
}

int32 __MTLK_IFUNC
mtlk_cli_srv_cmd_get_param_int (const mtlk_cli_srv_cmd_t *cmd, uint32 no, int *err)
{
  int32       val = 0;
  const char *param;
  int         res = MTLK_ERR_NO_RESOURCES;

  MTLK_ASSERT(cmd != NULL);

  param = mtlk_cli_srv_cmd_get_param(cmd, no);
  if (param) {
    char *p = NULL;
    val = strtoul(param, &p, 0);
    res = (*p == 0)?MTLK_ERR_OK:MTLK_ERR_VALUE;
  }

  if (err) {
    *err = res;
  }

  return val;
}
/******************************************************************************/

/******************************************************************************
 * CLI Server's response to send object
 ******************************************************************************/

struct mtlk_cli_srv_rsp_internal
{
  mtlk_strtok_t tokenizer;
  int           error;
  MTLK_DECLARE_INIT_STATUS;
};

MTLK_INIT_STEPS_LIST_BEGIN(cli_srv_rsp)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_srv_rsp, INIT_TOKENIZER)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_srv_rsp, UNPURGE_TOKENIZER) /* used for cleanup only */
MTLK_INIT_INNER_STEPS_BEGIN(cli_srv_rsp)
MTLK_INIT_STEPS_LIST_END(cli_srv_rsp);

static void
_mtlk_cli_srv_rsp_cleanup (mtlk_cli_srv_rsp_t *rsp)
{
  MTLK_CLEANUP_BEGIN(cli_srv_rsp, MTLK_OBJ_PTR(rsp))
    MTLK_CLEANUP_STEP(cli_srv_rsp, UNPURGE_TOKENIZER, MTLK_OBJ_PTR(rsp),
                      mtlk_strtok_purge, (&rsp->tokenizer))
    MTLK_CLEANUP_STEP(cli_srv_rsp, INIT_TOKENIZER, MTLK_OBJ_PTR(rsp),
                      mtlk_strtok_cleanup, (&rsp->tokenizer))
  MTLK_CLEANUP_END(cli_srv_rsp, MTLK_OBJ_PTR(rsp))
}

static int
_mtlk_cli_srv_rsp_init (mtlk_cli_srv_rsp_t *rsp)
{
  MTLK_INIT_TRY(cli_srv_rsp, MTLK_OBJ_PTR(rsp))
    MTLK_INIT_STEP(cli_srv_rsp, INIT_TOKENIZER, MTLK_OBJ_PTR(rsp),
                   mtlk_strtok_init, (&rsp->tokenizer))
    MTLK_INIT_STEP_VOID(cli_srv_rsp, UNPURGE_TOKENIZER, MTLK_OBJ_PTR(rsp),
                        MTLK_NOACTION, ())
  MTLK_INIT_FINALLY(cli_srv_rsp, MTLK_OBJ_PTR(rsp))
  MTLK_INIT_RETURN(cli_srv_rsp, MTLK_OBJ_PTR(rsp), _mtlk_cli_srv_rsp_cleanup, (rsp))
}

static int
_mtlk_cli_srv_rsp_send (mtlk_cli_srv_rsp_t *rsp, SOCKET sd)
{
  int    res = MTLK_ERR_OK;
  uint32 i   = 0;
  char   err_buf[32];
  uint32 nof_strs = mtlk_strtok_get_nof_tokens(&rsp->tokenizer);

  snprintf(err_buf, sizeof(err_buf), "%d %u\n", rsp->error, nof_strs);
  res = mtlk_socket_send_string(sd, err_buf, FALSE);
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  for (i = 0; i < nof_strs; i++) {
    const char *str = mtlk_strtok_get_token(&rsp->tokenizer, i);

    res = mtlk_socket_send_string(sd, str, FALSE);
    if (res != MTLK_ERR_OK) {
      goto end;
    }

    res = mtlk_socket_send_string(sd, MTLK_CLI_PROTOCOL_EOL, FALSE);
    if (res != MTLK_ERR_OK) {
      goto end;
    }
  }

  res = MTLK_ERR_OK;

end:
  return res;
}

int __MTLK_IFUNC
mtlk_cli_srv_rsp_add_str (mtlk_cli_srv_rsp_t *rsp, const char *str)
{
  MTLK_ASSERT(rsp != NULL);
  MTLK_ASSERT(str != NULL);

  return (*str)?mtlk_strtok_add_token(&rsp->tokenizer, str):MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_cli_srv_rsp_add_strf (mtlk_cli_srv_rsp_t *rsp, const char *fmt, ...)
{
  char    buffer[256];
  int     len    = 0;
  va_list va;

  MTLK_ASSERT(rsp != NULL);
  MTLK_ASSERT(fmt != NULL);

  va_start(va, fmt);
  len = vsnprintf(buffer, sizeof(buffer), fmt, va);
  va_end(va);

  return (len >= sizeof(buffer)) ? 
    MTLK_ERR_BUF_TOO_SMALL : mtlk_cli_srv_rsp_add_str(rsp, buffer);
}

void __MTLK_IFUNC
mtlk_cli_srv_rsp_set_error (mtlk_cli_srv_rsp_t *rsp, int err)
{
  MTLK_ASSERT(rsp != NULL);

  if (err != MTLK_ERR_OK) {
    rsp->error = err;
  }
}
/******************************************************************************/

/******************************************************************************
 * CLI Server's peer object
 ******************************************************************************/
static int32 __MTLK_IFUNC
_mtlk_cli_peert_hread_proc (mtlk_handle_t context)
{
  mtlk_cli_peer_t *peer = HANDLE_T_PTR(mtlk_cli_peer_t, context);

  while (1) {
    char              *str  = NULL;
    int                len  = 0;
    int                res;
    mtlk_cli_srv_cmd_t cmd;
    mtlk_cli_srv_rsp_t rsp;
    int                cmd_initialized = 0;
    int                rsp_initialized = 0;

    res = mtlk_socket_recv_alloc_string(peer->sd, &str, &len,  MTLK_CLI_PROTOCOL_EOL);
    if (res == MTLK_ERR_NOT_IN_USE) {
      /* socket has been disconnected */
      goto cleanup;
    }
    else if (res != MTLK_ERR_OK) {
      ELOG_D("Can't get command string. Err#%d", res);
      goto cleanup;
    }

    if (peer->thread_stop) {
      res = MTLK_ERR_NOT_IN_USE;
      goto cleanup;
    }

    res = _mtlk_cli_srv_cmd_init(&cmd, str);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Can't initiate cmd object. Err#%d", res);
      goto cleanup;
    }
    cmd_initialized = 1;

    if (mtlk_cli_srv_cmd_get_name(&cmd) == NULL) {
      ILOG9_V("Empty cmd arrived. Skipping..."); /* looks like we got tail of previous command */
      res = MTLK_ERR_OK;
      goto cleanup;
    }

    res = _mtlk_cli_srv_rsp_init(&rsp);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Can't initiate rsp object. Err#%d", res);
      goto cleanup;
    }
    rsp_initialized = 1;
    
    res = _mtlk_cli_srv_handle_cmd(peer->srv, &cmd, &rsp);
    if (res != MTLK_ERR_OK) { /* unknown command */
      mtlk_cli_srv_rsp_set_error(&rsp, MTLK_ERR_NOT_SUPPORTED);
    }

    res = _mtlk_cli_srv_rsp_send(&rsp, peer->sd);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Can't send responce. Err#%d", res);
      goto cleanup;
    }
    res = MTLK_ERR_OK;

cleanup:
    if (rsp_initialized) {
      _mtlk_cli_srv_rsp_cleanup(&rsp);
    }
    if (cmd_initialized) {
      _mtlk_cli_srv_cmd_cleanup(&cmd);
    }

    if (str) {
      mtlk_osal_mem_free(str);
    }

    if (res != MTLK_ERR_OK) {
      break;
    }
  }

  /* Set to 0 for following cleanup */
  peer->thread_running = 0;

  return 0;
}

MTLK_INIT_STEPS_LIST_BEGIN(cli_srv_peer)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_srv_peer, INIT_THREAD)
MTLK_INIT_INNER_STEPS_BEGIN(cli_srv_peer)
MTLK_INIT_STEPS_LIST_END(cli_srv_peer);

MTLK_START_STEPS_LIST_BEGIN(cli_srv_peer)
  MTLK_START_STEPS_LIST_ENTRY(cli_srv_peer, RUN_THREAD)
MTLK_START_INNER_STEPS_BEGIN(cli_srv_peer)
MTLK_START_STEPS_LIST_END(cli_srv_peer);

static void
_mtlk_cli_peer_stop (mtlk_cli_peer_t *peer)
{
  peer->thread_stop = 1;
  shutdown(peer->sd, SD_BOTH); /* cancel any blocked socket I/O */

  MTLK_STOP_BEGIN(cli_srv_peer,  MTLK_OBJ_PTR(peer))
    MTLK_STOP_STEP(cli_srv_peer,  RUN_THREAD, MTLK_OBJ_PTR(peer),
                   mtlk_osal_thread_wait, (&peer->thread, NULL));
  MTLK_STOP_END(cli_srv_peer,  MTLK_OBJ_PTR(peer))

  _mtlk_cli_peer_cleanup(peer);
}

static int
_mtlk_cli_peer_start (mtlk_cli_peer_t *peer)
{
  MTLK_START_TRY(cli_srv_peer,  MTLK_OBJ_PTR(peer))
    MTLK_START_STEP(cli_srv_peer,  RUN_THREAD, MTLK_OBJ_PTR(peer),
                    mtlk_osal_thread_run, (&peer->thread,
                                           _mtlk_cli_peert_hread_proc,
                                           HANDLE_T(peer)));
    peer->thread_running = 1;
  MTLK_START_FINALLY(cli_srv_peer,  MTLK_OBJ_PTR(peer))
  MTLK_START_RETURN(cli_srv_peer,  MTLK_OBJ_PTR(peer), _mtlk_cli_peer_stop, (peer))
}

static void
_mtlk_cli_peer_cleanup (mtlk_cli_peer_t *peer)
{
  if (INVALID_SOCKET != peer->sd) {
    close(peer->sd);
    peer->sd = INVALID_SOCKET;
  }

  MTLK_CLEANUP_BEGIN(cli_srv_peer,  MTLK_OBJ_PTR(peer))
    MTLK_CLEANUP_STEP(cli_srv_peer,  INIT_THREAD, MTLK_OBJ_PTR(peer),
                      mtlk_osal_thread_cleanup, (&peer->thread));
  MTLK_CLEANUP_END(cli_srv_peer,  MTLK_OBJ_PTR(peer))
}

static int
_mtlk_cli_peer_init (mtlk_cli_peer_t *peer, SOCKET sd, mtlk_cli_srv_t *srv)
{
  peer->sd             = sd;
  peer->srv            = srv;
  peer->thread_stop    = 0;
  peer->thread_running = 0;

  MTLK_INIT_TRY(cli_srv_peer,  MTLK_OBJ_PTR(peer))
    MTLK_INIT_STEP(cli_srv_peer,  INIT_THREAD, MTLK_OBJ_PTR(peer),
                   mtlk_osal_thread_init, (&peer->thread));
  MTLK_INIT_FINALLY(cli_srv_peer,  MTLK_OBJ_PTR(peer))
  MTLK_INIT_RETURN(cli_srv_peer,  MTLK_OBJ_PTR(peer), _mtlk_cli_peer_cleanup, (peer))
}
/******************************************************************************/
