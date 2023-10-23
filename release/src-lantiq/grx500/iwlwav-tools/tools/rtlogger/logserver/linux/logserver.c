/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 *
 *
 * Log server
 *
 * Written by: Andrey Fidrya
 *
 */
#include "mtlkinc.h"
#include "mtlk_osal.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <sys/syslog.h>

#include "logserver.h"
#include "logsrv_utils.h"
#include "cqueue.h"
#include "net.h"
#include "proto_drv.h"
#include "proto_lg.h"
#include "db.h"
#include "logsrv_protocol.h"
#include "mtlkerr.h"
#include "argv_parser.h"

#include "mtlknlink.h"
#include "nl.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app.h"
#endif

#define LOG_LOCAL_GID   GID_LOGSERVER
#define LOG_LOCAL_FID   1

// -------------
// Configuration
// -------------

#if 1
#define LOGSERVER_SINGLE_CLIENT
#endif

// Note: incoming data packets cannot be larger than IN_Q_MAX_SIZE
#define IN_Q_SIZE         512
#define IN_Q_MAX_SIZE     65536
#define IN_Q_RESIZE_STEP  4096

// Note: outgoing data packets cannot be larger than OUT_Q_MAX_SIZE
#define OUT_Q_SIZE        32768
#define OUT_Q_MAX_SIZE    524800
#define OUT_Q_RESIZE_STEP 65536

#define PARSE_EVENT_Q_SIZE 32768

#define LG_DEFAULT_PORT   2008

// ---------------
// Data structures
// ---------------

typedef struct _con_data_t
{
  int sock;
  struct sockaddr_in sockaddr;

  cqueue_t in_q;
  cqueue_t out_q;

  struct _con_data_t *next;
} con_data_t;

// ---------------
// Local variables
// ---------------
#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

// ----------------
// Global variables
// ----------------

int parse_events = 0;
cqueue_t parse_event_q;

int log_to_console = 0;
int log_to_syslog = 0;
int text_protocol = 0;

char *syslog_id = IWLWAV_RTLOG_APP_NAME_LOGSERVER;
int syslog_pri = LOG_NOTICE; // 5
int syslog_opt = LOG_PID;
int syslog_facility = LOG_DAEMON;

uint16 logserver_port = 0;

const char *log_cdev_name = NULL;
cdev_t *log_cdev = NULL;
int log_cdev_error_state = 0;

const char *scd_filename = NULL;

volatile int terminated = 0;
int deadlock_ctr = 0;
con_data_t *pcon_list = NULL;

BOOL read_enabled = TRUE;

/* ---------------------
   Forward declarations
  --------------------- */

static void update_datasource(void);
static void close_datasource(void);
static void con_close(con_data_t *pcon);
static int send_enqueue(con_data_t *pcon, unsigned char *data, size_t len);

// ---------------
// Local functions
// ---------------

static int
send_version_info (con_data_t *pcon)
{
  struct logsrv_info info;

  info.magic         = htonl(LOGSRV_INFO_MAGIC);
  info.size          = htonl(sizeof(info));
  info.log_ver_major = htons(RTLOGGER_VER_MAJOR);
  info.log_ver_minor = htons(RTLOGGER_VER_MINOR);

  return send_enqueue(pcon, (unsigned char*)&info, sizeof(info));
}

static int
accept_connections(int mother_socket)
{
  int result = -1;
  int s = INVALID_SOCKET;
  socklen_t sockaddr_len;
  struct sockaddr_in sockaddr = {0};
  con_data_t *pcon = NULL;
  int queues_initialized = 0;

  sockaddr_len = sizeof(sockaddr);
  for (;;) {
    s = accept(mother_socket, (struct sockaddr *) &sockaddr, &sockaddr_len);
    if (s == INVALID_SOCKET) {
      if (errno == EAGAIN) {
        // No more pending connections
        break;
      }
      ILOG1_S("accept: %s", strerror(errno));
      goto end;
    }

    // Errors aren't fatal for these options so return values aren't checked:
    SOCKET_SET_OPT(s, SO_SNDBUF, MAX_SND_BUF);
    SOCKET_SET_OPT(s, SO_RCVBUF, MAX_RCV_BUF);
    socket_set_linger(s, 0, 0);

    if (0 != socket_set_nonblock(s, 1)) {
      ELOG_V("Cannot switch socket to nonblocking mode");
      goto end;
    }

    pcon = (con_data_t *) malloc(sizeof(con_data_t));
    if (!pcon) {
      ELOG_V("Out of memory");
      goto end;
    }
    memset(pcon, 0, sizeof(con_data_t));

    pcon->sock = s;
    pcon->sockaddr = sockaddr;
    cqueue_init(&pcon->in_q);
    cqueue_init(&pcon->out_q);
    queues_initialized = 1;
    if (0 != cqueue_reset(&pcon->in_q, IN_Q_SIZE))
      goto end;
    if (0 != cqueue_reset(&pcon->out_q, OUT_Q_SIZE))
      goto end;

#ifdef LOGSERVER_SINGLE_CLIENT
    ILOG0_V("Closing active connections to accept a new one");
    close_datasource();
    if (pcon_list) {
      while (pcon_list)
        con_close(pcon_list);
    }
#endif

    ILOG0_S("Accepting a new connection from [%s]", inet_ntoa(sockaddr.sin_addr));

    if (log_cdev_error_state) {
      WLOG_V("Device is in error state");
    }

    if (0 != send_version_info(pcon)) {
      ELOG_V("Can not send Logger version!");
      goto end;
    }

    // TODO
    //send_enqueue(pcon, "HEADER", 6); // Enqueue header here
    //if (log_cdev_error_state)
    //  send_enqueue(pcon, "[CDEV_ERROR]", 13); // Notify of driver error state

    LOGSRV_LIST_PUSH_FRONT(pcon_list, pcon, next);
    /* Reset variables so they aren't freed if error occurs on next iteration: */
    s = 0;
    queues_initialized = 0;
    pcon = NULL;

#ifdef LOGSERVER_SINGLE_CLIENT
    update_datasource();
#endif
  }

  result = 0;

end:
  if (result != 0) {
    if (s != INVALID_SOCKET)
      close(s);
    if (pcon) {
      if (queues_initialized) {
        cqueue_cleanup(&pcon->in_q);
        cqueue_cleanup(&pcon->out_q);
      }
      free(pcon);
    }
  }
  return result;
}

static void
con_close(con_data_t *pcon)
{
  LOGSRV_LIST_REMOVE(pcon_list, pcon, con_data_t, next);

  close(pcon->sock);

  cqueue_cleanup(&pcon->in_q);
  cqueue_cleanup(&pcon->out_q);

  free(pcon);
}

static int
process_user_commands(con_data_t *pcon)
{
  int ret;
  CQUEUE_DEBUG_DUMP(&pcon->in_q, 1);

  for (;;) {
    ILOG9_V("Calling lg_process_next_pkt");
    ret = lg_process_next_pkt(&pcon->in_q);
    if (ret == -1) {
      ELOG_S("Unable to process a data packet from [%s]. Closing connection",
        inet_ntoa(pcon->sockaddr.sin_addr));
      return -1;
    } else if (ret == 0) {
      break;
    }
    ILOG9_D("Packet processed (%d)", ret);
  }

  // No packets recognized and no more space left in input queue
  if (cqueue_full(&pcon->in_q)) {
    ELOG_DS("Packet too long (%d byte(s) and still not recognized by "
        "parser). Closing connection [%s]", cqueue_size(&pcon->in_q),
        inet_ntoa(pcon->sockaddr.sin_addr));
    return -1;
  }

  return 0;
}

static int
process_input(con_data_t *pcon)
{
  ssize_t ret;
  int sz;

  for (;;) {
    ret = cqueue_read(&pcon->in_q, pcon->sock);
    if (ret == 0) {
      ILOG0_S("Connection closed by remote side [%s]",
        inet_ntoa(pcon->sockaddr.sin_addr));
      return -1;
    } else if (ret == -1) {
      if (errno == EINTR || // Interrupted by a system call
          errno == EAGAIN || // Socket write would block
          errno == EWOULDBLOCK) {
        return 0;
      } else if (errno == ECONNRESET) {
        ILOG0_S("Connection reset [%s]",
          inet_ntoa(pcon->sockaddr.sin_addr));
        return -1;
      }
      ELOG_SS("While reading data from remote side [%s]: %s",
          inet_ntoa(pcon->sockaddr.sin_addr), strerror(errno));
      return -1;
    }

    if (cqueue_space_left(&pcon->in_q) > 0)
      break;

    // Queue overflow: increase queue size and try to read more data
    sz = cqueue_max_size(&pcon->in_q);
    if (sz >= IN_Q_MAX_SIZE) {
      break;
    } else {
      unsigned int new_q_sz_nolim = sz + IN_Q_RESIZE_STEP;
      unsigned int new_q_sz = MIN(new_q_sz_nolim, IN_Q_MAX_SIZE);
      ILOG9_SD("Increasing [%s] input queue size to %u",
          inet_ntoa(pcon->sockaddr.sin_addr), new_q_sz);
      if (0 != cqueue_reserve(&pcon->in_q, new_q_sz)) {
        ELOG_V("Out of memory");
        break;
      }
    }
  }

  ILOG9_S("Processing user commands [%s]",
      inet_ntoa(pcon->sockaddr.sin_addr));
  if (0 != process_user_commands(pcon)) {
    return -1;
  }

  return 0;
}

static int
process_output(con_data_t *pcon)
{
  ssize_t ret;

  ASSERT(!cqueue_empty(&pcon->out_q));

  for (;;) {
    ret = cqueue_write(&pcon->out_q, pcon->sock);
    if (ret == 0) {
      ILOG0_S("Connection closed by remote side [%s]",
        inet_ntoa(pcon->sockaddr.sin_addr));
      return -1;
    } else if (ret == -1) {
      if (errno == EINTR || // Interrupted by a system call
          errno == EAGAIN || // Socket write would block
          errno == EWOULDBLOCK) {
        return 0;
      } else if (errno == ECONNRESET) {
        ILOG0_S("Connection reset [%s]",
          inet_ntoa(pcon->sockaddr.sin_addr));
        return -1;
      }
      ELOG_SS("While writing data to remote side [%s]: %s",
          inet_ntoa(pcon->sockaddr.sin_addr), strerror(errno));
      return -1;
    }

    if (cqueue_empty(&pcon->out_q))
      break;
  }
  return 0;
}

void
send_to_all(char *data, size_t len)
{
  con_data_t *pcon;
  for (pcon = pcon_list; pcon; pcon = pcon->next) {
    send_enqueue(pcon, (unsigned char*)data, len);
  }
}

static int
send_enqueue(con_data_t *pcon, unsigned char *data, size_t len)
{
  int space_left = cqueue_space_left(&pcon->out_q);

  if (space_left < len) {
    int need_space = len - space_left;
    int q_size = cqueue_max_size(&pcon->out_q);
    if (q_size + need_space > OUT_Q_MAX_SIZE) {
      ELOG_S("Output queue overflow [%s]",
          inet_ntoa(pcon->sockaddr.sin_addr));
      //cqueue_reset(&pcon->out_q, cqueue_max_size(&pcon->out_q));
      //TODO: report overflow
      return 1;
    }
    if (need_space < OUT_Q_RESIZE_STEP)
      need_space = OUT_Q_RESIZE_STEP;
    if (0 != cqueue_reserve(&pcon->out_q, q_size + need_space)) {
      return 1;
    }
  }

  ASSERT(cqueue_space_left(&pcon->out_q) >= len);

  cqueue_push_back(&pcon->out_q, data, len);

  return 0;
}

static int
process_drv_packets(cqueue_t *pq)
{
  int ret;

  for (;;) {
    ILOG9_V("Calling drv_process_next_pkt");
    ret = drv_process_next_pkt(pq);
    if (ret == -1) {
      ELOG_V("Unable to process a data packet from driver");
      return -1;
    } else if (ret == 0) {
      break;
    }
    ILOG9_D("Packet processed (%d)", ret);
  }

  // No packets recognized and no more space left in queue
  if (cqueue_full(pq)) {
    ELOG_D("Packet too long (%d byte(s) and still not recognized by "
        "parser). Closing data source", cqueue_size(pq));
    return -1;
  }

  return 0;
}

static void
main_loop(int mother_socket)
{
  int max_fd;
  fd_set input_set;
  fd_set output_set;
  fd_set exc_set;
  con_data_t *pcon;
  con_data_t *pcon_next;
  int ret;

  while (!terminated) {

    /*
     * If no connections, sleep infinitely until a connection or
     * a signal arrives
     */
    if (!pcon_list && !parse_events) {
      ILOG0_V("No connections - going to sleep");

      close_datasource();

      FD_ZERO(&input_set);
      FD_SET(mother_socket, &input_set);
      if (select(mother_socket + 1, &input_set, NULL, NULL, NULL) < 0) {
        if (errno == EINTR)
          ILOG0_V("Waking up to process signal");
        else
          ILOG1_S("select: %s", strerror(errno));
      } else {
        ILOG0_V("New connection - waking up");
      }

      if (terminated)
        break;
    }

    /*
     * Poll for new connections, new input and exceptions
     */
    FD_ZERO(&input_set);
    FD_ZERO(&output_set);
    FD_ZERO(&exc_set);

    FD_SET(mother_socket, &input_set);
    max_fd = mother_socket;
    for (pcon = pcon_list; pcon; pcon = pcon->next) {
      if (pcon->sock > max_fd)
        max_fd = pcon->sock;

      if (!cqueue_full(&pcon->in_q)) {
        FD_SET(pcon->sock, &input_set);
      }

      if (!cqueue_empty(&pcon->out_q)) {
        FD_SET(pcon->sock, &output_set);
      }

      FD_SET(pcon->sock, &exc_set);
    }

    if (log_cdev) {
      if (parse_events && (log_to_console || log_to_syslog || pcon_list)) {
        if (!cqueue_full(&parse_event_q)) {
          FD_SET(log_cdev->fd, &input_set);
          if (log_cdev->fd > max_fd)
            max_fd = log_cdev->fd;
        }
      } else {
        if (pcon_list && !cqueue_full(&pcon_list->out_q)) {
          FD_SET(log_cdev->fd, &input_set);
          if (log_cdev->fd > max_fd)
            max_fd = log_cdev->fd;
        }
      }
    }

    if (read_enabled == FALSE)
    {
      if (log_cdev && pcon_list)
      {
        if (cqueue_empty(&pcon_list->out_q))
        {
          read_enabled = TRUE;
        }
        else
        {
          FD_ZERO(&input_set);
        }
      }
    }

    ILOG9_V("SELECT: calling");
    ret = select(max_fd + 1, &input_set, &output_set, &exc_set, NULL);
    if (ret < 0) {
      ILOG1_DS("select (%d): %s", errno, strerror(errno));
      if (errno == EINTR) {
        /* Signal caught, this doesn't necessarily means termination */
        continue;
      } else {
        terminated = 1;
        continue;
      }
    }
    ILOG9_D("SELECT: back from select (%d)", ret);

    /*
     * Accept new connections
     */
    if (FD_ISSET(mother_socket, &input_set)) {
      if (0 != accept_connections(mother_socket)) {
        ELOG_V("While accepting a new connection");
        terminated = 1;
        continue;
      }
    }

    /*
     * Get new data from driver
     */
    if (log_cdev && FD_ISSET(log_cdev->fd, &input_set)) {
      if (parse_events) {
        ASSERT(!cqueue_full(&parse_event_q));
        ASSERT(log_cdev != NULL);
        //int old_sz = cqueue_size(parse_event_q);
        if (0 >= cdev_read_to_q(&parse_event_q, log_cdev)) {
            ELOG_S("While queuing log data for console: %s", strerror(errno));
            log_cdev_error_state = 1;
            close_datasource();
        }
        // TODO: copy data to out_q
        if (0 != process_drv_packets(&parse_event_q)) {
          log_cdev_error_state = 1;
          close_datasource();
        }
      } else {
        // Read directly to out_q
        if (pcon_list) {
          pcon = pcon_list;
          ASSERT(!cqueue_full(&pcon->out_q));
          ASSERT(log_cdev != NULL);
          if (0 >= cdev_read_to_q(&pcon->out_q, log_cdev)) {
            ELOG_S("While queuing log data for [%s]",
              inet_ntoa(pcon->sockaddr.sin_addr));
            log_cdev_error_state = 1;
            close_datasource();
          } else if (cqueue_full(&pcon->out_q)) {
            // Next time try with larger buffer
            int cur_sz = cqueue_max_size(&pcon->out_q);
            int new_sz = cur_sz + OUT_Q_RESIZE_STEP;
            new_sz = MIN(new_sz, OUT_Q_MAX_SIZE);
            if (new_sz == cur_sz) {
              ELOG_DS("Internal buffer is FULL. Skip next %d bytes for [%s]",
                      cur_sz,
                      inet_ntoa(pcon->sockaddr.sin_addr));
              read_enabled = FALSE;
            }
            if (read_enabled)
            {
              if (0 != cqueue_reserve(&pcon->out_q, new_sz)) {
                ELOG_V("Out of memory");
              }
            }
          }
        }
      }
    }

    /*
     * Close sockets in exception state
     */
    for (pcon = pcon_list; pcon; pcon = pcon_next) {
      pcon_next = pcon->next;

      if (FD_ISSET(pcon->sock, &exc_set)) {
        FD_CLR(pcon->sock, &input_set);
        FD_CLR(pcon->sock, &output_set);
        con_close(pcon);
      }
    }

    /*
     * Process input
     */
    for (pcon = pcon_list; pcon; pcon = pcon_next) {
      pcon_next = pcon->next;

      if (FD_ISSET(pcon->sock, &input_set)) {
        if (0 != process_input(pcon))
          con_close(pcon);
      }
    }

    /*
     * Process output
     */
    for (pcon = pcon_list; pcon; pcon = pcon_next) {
      pcon_next = pcon->next;

      if (FD_ISSET(pcon->sock, &output_set)) {
        if (0 != process_output(pcon))
          con_close(pcon);
      }
    }

    deadlock_ctr = 1;
  }
}

static void
update_datasource(void)
{
  if (!log_cdev && !log_cdev_error_state) {
    log_cdev = cdev_open(log_cdev_name);
    if (!log_cdev) {
      log_cdev_error_state = 1;
    }
    else {
      uint16 ver_major = cdev_get_ver_major(log_cdev);
      uint16 ver_minor = cdev_get_ver_minor(log_cdev);
      if (ver_major != RTLOGGER_VER_MAJOR) {
        ELOG_DDDD("Incompatible Logger version: %d.%d != %d.%d. Can't parse Log data! Aborting...",
              ver_major, ver_minor, RTLOGGER_VER_MAJOR, RTLOGGER_VER_MINOR);
        log_cdev_error_state = 1;
        close_datasource();
      }
      else if (ver_minor != RTLOGGER_VER_MINOR) {
        WLOG_DDDD("Logger version minor isn't same: %d.%d != %d.%d",
                ver_major, ver_minor, RTLOGGER_VER_MAJOR, RTLOGGER_VER_MINOR);
      }
    }
  }
}

static void
close_datasource(void)
{
  if (log_cdev) {
    cdev_close(log_cdev);
    log_cdev = NULL;
  }
}

static int
scd_load(void)
{
#define MAX_SCD_LINE_SIZE 8192

  FILE *fl = NULL;
  int rslt = 0;
  int retval;
  char *buf = NULL;
  char id_buf[64];
  int oid, gid, fid, lid;
  int eof = 0;
  char *p;

  fl = fopen(scd_filename, "rb");
  if (!fl) {
    ILOG2_SS("%s: unable to open for reading: %s",
        scd_filename, strerror(errno));
    rslt = ENOENT;
    goto cleanup;
  }

  buf = (char *) malloc(MAX_SCD_LINE_SIZE);
  if (!buf) {
    ELOG_V("Out of memory");
    goto cleanup;
  }


  for (;;) {
    retval = get_line(scd_filename, buf, MAX_SCD_LINE_SIZE, fl, 1, &eof);
    if (retval != 0) {
      rslt = -1;
      goto cleanup;
    }
    if (eof)
      break;
    ILOG2_S("read line: %s", buf);

    p = buf;

    /* parse string ID: O/G/S */
    retval = get_word(&p, id_buf, ARRAY_SIZE(id_buf));
    if (retval != 0) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }
    if (strcmp(id_buf, "S") != 0) {
      continue;
    }

    /* parse OID */
    retval = get_word(&p, id_buf, ARRAY_SIZE(id_buf));
    if (retval != 0) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }
    oid = atoi(id_buf);

    /* parse GID */
    retval = get_word(&p, id_buf, ARRAY_SIZE(id_buf));
    if (retval != 0) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }
    gid = atoi(id_buf);

    /* parse FID */
    retval = get_word(&p, id_buf, ARRAY_SIZE(id_buf));
    if (retval != 0) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }
    fid = atoi(id_buf);

    /* parse LID */
    retval = get_word(&p, id_buf, ARRAY_SIZE(id_buf));
    if (retval != 0) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }
    lid = atoi(id_buf);

    skip_spcrlf(&p);
    if (0 != str_decode(p)) {
      ELOG_S("%s: syntax error", scd_filename);
      rslt = -1;
      goto cleanup;
    }

    if (0 != db_register_scd_entry(oid, gid, fid, lid, p)) {
      rslt = -1;
      goto cleanup;
    }
  }

cleanup:
  if (fl) {
    retval = fclose(fl);
    fl = NULL;
    if (retval != 0) {
      ELOG_SS("%s: cannot close: %s", scd_filename, strerror(errno));
      rslt = -1;
    }
  }

  if (buf) {
    free(buf);
  }

  return rslt;
}

static void
on_sighup(int sig)
{
  ILOG0_V("Received SIGHUP: shutting down");
  terminated = 1;
}

static void
on_sigchld(int sig)
{
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
  signal(SIGCHLD, on_sigchld);
}

static void
on_sigint(int sig)
{
  ILOG0_V("Received SIGINT: shutting down");
  terminated = 1;
}

static void
on_sigterm(int sig)
{
  ILOG0_V("Received SIGTERM: shutting down");
  terminated = 1;
}

static void
on_sigvtalrm(int sig)
{
  ILOG9_V("Received sigvtalrm: deadlock check");
  if (deadlock_ctr == 0) {
    ELOG_V("Deadlock state, aborting");
    abort();
  } else {
    deadlock_ctr = 0;
  }
}

static void
setup_signals()
{
  struct itimerval itv;
  struct timeval tv;

  signal(SIGHUP,  on_sighup);
  signal(SIGCHLD, on_sigchld);
  signal(SIGINT,  on_sigint);
  signal(SIGTERM, on_sigterm);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGALRM, SIG_IGN);

  /*
   * Deadlock protection
   */
  tv.tv_sec = 30;
  tv.tv_usec = 0;
  itv.it_interval = itv.it_value = tv;
  setitimer(ITIMER_VIRTUAL, &itv, NULL);
  signal(SIGVTALRM, on_sigvtalrm);
}

static const struct mtlk_argv_param_info_ex param_dlevel = {
  {
    "d",
    "debug-level",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "debug level (DEBUG version only)",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_stderr_err = {
  {
    NULL,
    "stderr-err",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate ERROR printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_stderr_warn = {
  {
    NULL,
    "stderr-warn",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate ERROR and WARNING printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_stderr_all = {
  {
    "e",
    "stderr-all",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate all (ERROR, WARNING, LOG) printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_help = {
  {
    "h",
    "help",
     MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "print this help",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_console_log = {
  {
    "c",
    "console-log",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "log events to console",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_syslogd_log = {
  {
    "l",
    "syslogd-log",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "log events to syslogd with the priority specified",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_text = {
  {
    "t",
    "text",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "use text protocol",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_port = {
  {
    "p",
    "port",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "TCP/IP port to listen on",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_cdev_fname = {
  {
    "f",
    "cdev-fname",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "log character device file name",
  MTLK_ARGV_PTYPE_MANDATORY
};

static const struct mtlk_argv_param_info_ex param_scd_fname = {
  {
    "s",
    "scd-fname",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "string configuration data file",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static void
_print_help (const char *app_name)
{
  const struct mtlk_argv_param_info_ex *all_params[] = {
    &param_cdev_fname,
    &param_console_log,
    &param_syslogd_log,
    &param_text,
    &param_port,
    &param_scd_fname,
    &param_dlevel,
    &param_stderr_err,
    &param_stderr_warn,
    &param_stderr_all,
    &param_help
  };
  const char *app_fname = strrchr(app_name, '/');

  if (!app_fname) {
    app_fname = app_name;
  }
  else {
    ++app_fname; /* skip '/' */
  }

  mtlk_argv_print_help(stdout,
                       app_fname,
                       "Lantiq Log Server v." MTLK_SOURCE_VERSION,
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}

static int
process_commandline(int argc, char *argv[])
{
  int                res   = MTLK_ERR_UNKNOWN;
  BOOL               inited = FALSE;
  mtlk_argv_param_t *param  = NULL;
  mtlk_argv_parser_t argv_parser;

  if (argc < 2) {
    res  = MTLK_ERR_PARAMS;
    goto end;
  }

  res = mtlk_argv_parser_init(&argv_parser, argc, argv);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init argv parser (err=%d)", res);
    goto end;
  }
  inited = TRUE;

  param = mtlk_argv_parser_param_get(&argv_parser, &param_help.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_dlevel.info);
  if (param) {
    uint32 v = mtlk_argv_parser_param_get_uint_val(param, (uint32)-1);
    mtlk_argv_parser_param_release(param);
    if (v != (uint32)-1) {
      debug = v;
    }
    else {
      ELOG_V("Invalid debug-level");
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_err.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_warn.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_WARN, TRUE);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_all.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_WARN, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_INFO, TRUE);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_console_log.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    parse_events   = 1;
    log_to_console = 1;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_syslogd_log.info);
  if (param) {
    syslog_pri = (int)mtlk_argv_parser_param_get_uint_val(param, (uint32)-1);
    mtlk_argv_parser_param_release(param);
    parse_events  = 1;
    log_to_syslog = 1;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_text.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    parse_events  = 1;
    text_protocol = 1;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_port.info);
  if (param) {
    uint32 v = mtlk_argv_parser_param_get_uint_val(param, (uint32)-1);
    mtlk_argv_parser_param_release(param);
    if (v != (uint32)-1) {
      logserver_port = v;
    }
    else {
      ELOG_V("Invalid port");
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_cdev_fname.info);
  if (param) {
    log_cdev_name = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);
  }

  if (!log_cdev_name) {
    ELOG_S("%s must be specified", param_cdev_fname.desc);
    res = MTLK_ERR_VALUE;
    goto end;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_scd_fname.info);
  if (param) {
    scd_filename = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);
    if (!scd_filename) {
      ELOG_S("%s must be specified", param_scd_fname.desc);
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }

  res = MTLK_ERR_OK;

end:
  if (inited) {
    mtlk_argv_parser_cleanup(&argv_parser);
  }

  if (res != MTLK_ERR_OK) {
    _print_help(argv[0]);
  }

  return res;
}

int stop_pipe_fd[2];
pthread_t notification_thread;

static void
_parse_log (void *param, void *packet)
{
  mtlk_log_event_t *log_evt;
  char *scd_text = NULL;
  BOOL stderr_log = FALSE;
  int oid, gid, fid, lid, wlanif = 0;

  MTLK_UNREFERENCED_PARAM(param);
  MTLK_UNREFERENCED_PARAM(wlanif);

  log_evt = (mtlk_log_event_t *)packet;

  oid = LOG_INFO_GET_OID(*log_evt);
  gid = LOG_INFO_GET_GID(*log_evt);
  fid = LOG_INFO_GET_FID(*log_evt);
  lid = LOG_INFO_GET_LID(*log_evt);
  wlanif = LOG_INFO_GET_WLAN_IF(*log_evt);

  if (scd_data && (scd_filename))
    scd_text = scd_get_text(oid, gid, fid, lid);

  if (scd_filename) {
    if (scd_data) {
      stderr_log = _mtlk_osdep_log_is_enabled_stderr(MTLK_OSLOG_INFO);
      if (!stderr_log) {
        _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_INFO, TRUE);
      }
      if (!scd_text)
        ILOG0_DS("wlan%d: %s", wlanif, "SCD entry not found");
      else
        ILOG0_DS("wlan%d: %s", wlanif, scd_text);
      if (!stderr_log) {
        _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_INFO, FALSE);
      }
    }
  }
}

static void *
_notification_thread_proc (void *param)
{
  int res;
  mtlk_nlink_socket_t nl_socket;

  MTLK_UNREFERENCED_PARAM(param);

  res = mtlk_nlink_create(&nl_socket, MTLK_NETLINK_LOGSERVER_GROUP_NAME, _parse_log, NULL);

  if (res < 0) {
    ELOG_SD("Failed to create netlink socket: %s (%d)", strerror(res), res);
    goto end;
  }

  res = pipe(stop_pipe_fd);
  if (res) {
    ELOG_SD("Failed to create pipe: %s (%d)", strerror(res), res);
    goto end;
  }

  res = mtlk_nlink_receive_loop(&nl_socket, stop_pipe_fd[0]);
  if (res < 0) {
    ELOG_SD("Netlink socket receive failed: %s (%d)", strerror(res), res);
  }

  mtlk_nlink_cleanup(&nl_socket);

end:
  pthread_exit(&res);
}

static void
_notification_thread_stop (void)
{
  void *status;
  int res;

  /* Signal the notification thread to stop */
  write(stop_pipe_fd[1], "x", 1);
  /* Wait for the notification thread to process the signal */
  res = pthread_join(notification_thread, &status);
  if (0 != res) {
    ELOG_SD("Failed to terminate the notification thread: %s (%d)", strerror(res), res);
  }

  close(stop_pipe_fd[0]);
  close(stop_pipe_fd[1]);
}

static int
_create_nlink_thread (void)
{
  int res;

  pthread_attr_t attr;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  res = pthread_create(&notification_thread, &attr, _notification_thread_proc, NULL);
  if (res != 0) {
    ELOG_S("Failed create thread for NETLINK: %s", strerror(errno));
  }

  pthread_attr_destroy(&attr);

  return res;
}

int
main(int argc, char *argv[])
{
  int retval;
  int mother_socket = -1;
  int rslt = 0;
  int db_initialized = 0;

  if (_mtlk_osdep_log_init(syslog_id) != MTLK_ERR_OK) {
    rslt = 100;
    goto end_no_osal_return;
  }

  ILOG0_S("Lantiq Log Server v. %s", MTLK_SOURCE_VERSION);

  if (mtlk_osal_init() != MTLK_ERR_OK) {
    rslt = 200;
    goto end_return;
  }

#ifdef CONFIG_WAVE_RTLOG_REMOTE
  if (MTLK_ERR_OK != mtlk_rtlog_app_init(&rtlog_info_data, syslog_id)) {
    rslt = 1;
    goto end;
  }
#endif

  if (MTLK_ERR_OK != process_commandline(argc, argv)) {
    rslt = 1;
    goto end;
  }

  if (debug > 0) {
    ILOG0_D("Debug level set to: %d", debug);
  }
  ILOG0_S("Reading data from device: %s", log_cdev_name);

  if (!logserver_port) {
    logserver_port = LG_DEFAULT_PORT;
    ILOG0_D("Port not specified. Assuming default: %u", logserver_port);
  }

  if (0 != create_mother_socket(NULL, logserver_port, &mother_socket)) {
    ELOG_V("Unable to create a listening socket (make sure no other instances "
        "of logserver are running)");
    rslt = 1;
    goto end;
  }

  setup_signals();

  if (parse_events) {
    ILOG0_V("Events will be parsed by logserver");
    cqueue_init(&parse_event_q);
    cqueue_reset(&parse_event_q, PARSE_EVENT_Q_SIZE);
    if (log_to_console || log_to_syslog)
      update_datasource();
  }

  if (log_to_console) {
    ILOG0_V("Events will be shown on console");
  }

  if (text_protocol) {
    ILOG0_V("Logserver will use a text protocol");
  }

  if (0 != db_init()) {
    ELOG_V("Unable to initialize database");
    rslt = 1;
    goto end;
  }
  db_initialized = 1;

  if (scd_filename) {
    ILOG0_S("Using string configuration data file: %s", scd_filename);
    retval = scd_load();
    if (retval == ENOENT) {
      ELOG_S("%s: file not found", scd_filename);
      /* don't return error, run as usually without SCD file */
      scd_filename = NULL;
    } else if (retval != 0) {
      rslt = 1;
      goto end;
    }
    else {
      retval = _create_nlink_thread();
      if (retval) {
        rslt = 1;
        goto end;
      }
    }
  }

  main_loop(mother_socket);

  if (scd_filename) {
    _notification_thread_stop();
  }

  close_datasource();

  while (pcon_list)
    con_close(pcon_list);

  if (parse_events)
    cqueue_cleanup(&parse_event_q);

end:
  if (mother_socket >= 0) {
    close(mother_socket);
  }
  if (db_initialized) {
    if (0 != db_destroy()) {
      ELOG_V("Unable to destroy database");
    }
  }

  ILOG0_V("Terminated");
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  mtlk_rtlog_app_cleanup(&rtlog_info_data);
#endif
  _mtlk_osdep_log_cleanup();
end_return:
  mtlk_osal_cleanup();
end_no_osal_return:
  return rslt;
}

