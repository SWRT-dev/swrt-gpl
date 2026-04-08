//#define WAVELOG_DEBUG

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "mtlkinc.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifdef WIN32

#include <io.h>
#include <fcntl.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr    SOCKADDR;
typedef struct in_addr     IN_ADDR;

#define closesocket(s) close(s);

#else

#error not defined for this platform

#endif

#include "logdefs.h"
#include "logsrv_protocol.h"
#include "logmacro_mixins.h"

#include "pcapdefs.h"

static void process(SOCKET sock);

#undef  USE_GETHOSTBYNAME

#ifndef USE_GETHOSTBYNAME

typedef struct my_inadrr_t {
    union {
        struct { unsigned char b1, b2, b3, b4; } s_b;
        long     s_l;
    };
} _my_inadrr_t;

static IN_ADDR *
get_inaddr_by_ipaddr (const char *ip_address, IN_ADDR *in_addr)
{
    int          num, b1, b2, b3, b4;
    _my_inadrr_t my_inaddr;

    num = sscanf(ip_address, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
    if (num != 4 || (~0xFF & (b1 | b2 | b3 | b4))) {
        fprintf(stderr, "Invalid ip_address '%s'\n", ip_address);
        return NULL;
    }

    my_inaddr.s_b.b1 = b1;
    my_inaddr.s_b.b2 = b2;
    my_inaddr.s_b.b3 = b3;
    my_inaddr.s_b.b4 = b4;

    *in_addr = *((IN_ADDR *)&my_inaddr);

    return in_addr;
}
#endif /* _USE_GETHOSTBYNAME */

int
#ifdef WIN32
__cdecl
#endif
main(int argc, char *argv[])
{
  char *ip_address;
  int   ip_port;
  SOCKET sock;
  SOCKADDR_IN sin = { 0 };

#ifdef USE_GETHOSTBYNAME
  struct hostent *hostinfo;
#else
  IN_ADDR   in_addr;
#endif

#ifdef WIN32
  WSADATA wsa_data;
#endif

  if (3 != argc) {
    fprintf(stderr,
            "Usage:\n"
            "\twavelog_cat <ip address> <port>\n"
      );
    return EXIT_FAILURE;
  }

  ip_address = argv[1];
  ip_port = atoi(argv[2]);

  fprintf(stderr, "Connecting to %s:%d ...\n", ip_address, ip_port);

#ifdef WIN32
  if (WSAStartup(MAKEWORD(2,2), &wsa_data)) {
    fprintf(stderr, "WSAStartup() failed\n");
    return EXIT_FAILURE;
  }
#endif

#if WIN32
  _setmode(1, _O_BINARY);
#endif

  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (INVALID_SOCKET == sock) {
    fprintf(stderr, "socket() failed\n");
    return EXIT_FAILURE;
  }

#ifdef USE_GETHOSTBYNAME
  hostinfo = gethostbyname(ip_address);

  if (NULL == hostinfo) {
    fprintf(stderr, "gethostbyname: cannot find host %s\n", ip_address);
    return EXIT_FAILURE;
  }

  sin.sin_addr   = *(IN_ADDR*) hostinfo->h_addr;

#else
  if (NULL == get_inaddr_by_ipaddr(ip_address, &in_addr)) {
    return EXIT_FAILURE;
  }

  sin.sin_addr   = in_addr;

#endif /* USE_GETHOSTBYNAME */

  sin.sin_family = AF_INET;
  sin.sin_port   = htons((unsigned short)ip_port);
  /* memcpy(&sin.sin_addr, hostinfo->h_addr, hostinfo->h_length); */
  
  if (connect(sock, (SOCKADDR*)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    fprintf(stderr, "connect() failed\n");
    return EXIT_FAILURE;
  }

  process(sock);
  closesocket(sock);

#ifdef WIN32
  WSACleanup();
#endif

  return EXIT_SUCCESS;
}

#define REVERSE8(x)        (x)
#define REVERSE16(x)       ( (uint16) ( ((x) & 0xFFFF) >> 8 ) | ( ((x) << 8) & 0xFFFF ) )
#define REVERSE32(x)       ( (uint32) (                       \
                               ( ((x) & 0xFF000000) >> 24 ) | \
                               ( ((x) & 0x00FF0000) >> 8  ) | \
                               ( ((x) & 0x0000FF00) << 8  ) | \
                               ( ((x) & 0x000000FF) << 24 )   \
                               )                              \
    )
#define REVERSE64(x)       ( (uint64) (                                 \
                               ( ((x) & 0xFF00000000000000ULL) >> 56 ) | \
                               ( ((x) & 0x00FF000000000000ULL) >> 40 ) | \
                               ( ((x) & 0x0000FF0000000000ULL) >> 24 ) | \
                               ( ((x) & 0x000000FF00000000ULL) >> 8  ) | \
                               ( ((x) & 0x00000000FF000000ULL) << 8  ) | \
                               ( ((x) & 0x0000000000FF0000ULL) << 24 ) | \
                               ( ((x) & 0x000000000000FF00ULL) << 40 ) | \
                               ( ((x) & 0x00000000000000FFULL) << 56 )  \
                               )                                        \
    )

#define MTLK_LOGGER_NETWORK  148 /* Corresponds to wtap 45 - WTAP_ENCAP_USER1 */
#define MTLK_LOGGER_SNAPLEN  65535

static const pcap_hdr_t log_pcap_hdr = 
{
  PCAP_MAGIC,
  PCAP_VERSION_MAJOR,
  PCAP_VERSION_MINOR,
  0,
  0,
  MTLK_LOGGER_SNAPLEN,
  MTLK_LOGGER_NETWORK
};

#define BUFFER_SIZE 1024

#define CHECK_RC(rc)                            \
  if (rc < 0) {                                 \
    fprintf(stderr, "recv() failed\n");         \
    exit(EXIT_FAILURE);                         \
  } else if (rc == 0) {                         \
    fprintf(stderr, "connection closed\n");     \
    exit(EXIT_FAILURE);                         \
  }

#ifdef WIN32
bool IsWinXP(void)
{
  OSVERSIONINFO osvi;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  GetVersionEx(&osvi);

/*
http://msdn.microsoft.com/en-us/library/ms724834%28v=vs.85%29.aspx
  Operating system        Version number  dwMajorVersion  dwMinorVersion  Other
  Windows 8.1             6.3*            6               3               OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
  Windows Server 2012 R2  6.3*            6               3               OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
  Windows 8               6.2             6               2               OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
  Windows Server 2012     6.2             6               2               OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
  Windows 7               6.1             6               1               OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
  Windows Server 2008 R2  6.1             6               1               OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
  Windows Server 2008     6.0             6               0               OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
  Windows Vista           6.0             6               0               OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
  Windows Server 2003 R2  5.2             5               2               GetSystemMetrics(SM_SERVERR2) != 0
  Windows Server 2003     5.2             5               2               GetSystemMetrics(SM_SERVERR2) == 0
  Windows XP              5.1             5               1               Not applicable
  Windows 2000            5.0             5               0               Not applicable
*/

  /* Return TRUE for Windows XP or earlier */
  return ((osvi.dwMajorVersion < 6) ||
          ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion < 2)));
}
#endif

#ifdef WAVELOG_DEBUG
void hexdump(void *ptr, int buflen, int *offset)
{
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;

  for (i=0; i<buflen; i+=16, *offset+=16) {
    fprintf(stderr, "%04x: ", *offset);

    for (j=0; j<16; j++) {
      if (i+j < buflen) {
        fprintf(stderr, "%02x ", buf[i+j]);
      }
      else {
        fprintf(stderr, "   ");
      }
    }

    fprintf(stderr, " ");

    for (j=0; j<16; j++) {
      if (i+j < buflen) {
        fprintf(stderr, "%c", isprint(buf[i+j]) ? buf[i+j] : '.');
      }
    }

    fprintf(stderr, "\n");
  }
}
#endif

static void process(SOCKET sock)
{
  int rc;
  char buffer[BUFFER_SIZE];
  logsrv_info info;
  int size;

  rc = recv(sock, (char*)&info, sizeof(info), 0);
  CHECK_RC(rc);

  info.magic = ntohl(info.magic);

  if (LOGSRV_INFO_MAGIC != info.magic) {
    fprintf(stderr, "logsrv_info magic failed\n");
    exit(EXIT_FAILURE);
  }

  info.size = ntohl(info.size);
  if (info.size != sizeof(info)) {
    fprintf(stderr, "logsrv_info size failed\n");
    exit(EXIT_FAILURE);
  }

  info.log_ver_major = ntohs(info.log_ver_major);
  if (RTLOGGER_VER_MAJOR != info.log_ver_major) {
    fprintf(stderr, "logsrv_info version failed\n");
    exit(EXIT_FAILURE);
  }

  info.log_ver_minor = ntohs(info.log_ver_minor);

  fwrite((char *)&log_pcap_hdr, sizeof(log_pcap_hdr), 1, stdout);
  fflush(stdout);

  /* read/write loop */
  while(1) {
    mtlk_log_event_t log_event;
    mtlk_log_event_t log_event_orig;
    int log_event_hdr_size = LOGPKT_EVENT_HDR_SIZE;
    int reversed;
    int version;
#ifdef WAVELOG_DEBUG
    int hdr_offset = 0;
#endif

    rc = recv(sock, (char*)&log_event, log_event_hdr_size, 0);
    CHECK_RC(rc);

#ifdef WAVELOG_DEBUG
    hexdump(&log_event, log_event_hdr_size, &hdr_offset);
#endif
    if (!LOG_IS_CORRECT_INFO(log_event.info)) {
      fprintf(stderr, "log event header info failed\n");
      exit(EXIT_FAILURE);
    }

    memcpy((char *)&log_event_orig, (char *)&log_event, log_event_hdr_size);

    reversed = LOG_IS_INVERSED_ENDIAN(log_event.info);
    if (reversed) {
      log_event.info      = REVERSE16(log_event.info);
      log_event.info_ex   = REVERSE32(log_event.info_ex);
      log_event.timestamp = REVERSE32(log_event.timestamp);
    }

    version = LOG_INFO_GET_VERSION(log_event.info);
#ifdef WAVELOG_DEBUG
    {
      fprintf(stderr, "\nreversed: %u\n", reversed);
      fprintf(stderr, "\nver: %u\nOID: %u\nGID: %u\nFID: %u\nLID: %u",
              version,
              LOG_INFO_GET_OID(log_event.info),
              LOG_INFO_GET_GID(log_event.info),
              LOG_INFO_EX_GET_FID(log_event.info_ex),
              LOG_INFO_EX_GET_LID(log_event.info_ex));
      fprintf(stderr, "\nwlan: %u\nbe0: %u\nbe1: %u\nver: %u\n",
              LOG_INFO_EX_GET_WLAN_IF(log_event.info_ex),
              LOG_IS_INVERSED_ENDIAN(log_event.info),
              LOG_IS_STRAIGHT_ENDIAN(log_event.info),
              LOG_INFO_GET_VERSION(log_event.info));
      fprintf(stderr, "\n");
    }
#endif
    if (0 == version) {
      size = LOG_INFO_EX_GET_DSIZE(log_event.info_ex);
    }
    else if (2 == version) {
      rc = recv(sock, (char *)&log_event.size, sizeof(log_event.size), 0);
      CHECK_RC(rc);

      memcpy((char *)&log_event_orig.size, (char *)&log_event.size, sizeof(log_event.size));

      if (reversed) {
        log_event.size = REVERSE16(log_event.size);
      }
      size = log_event.size;
      log_event_hdr_size = LOGPKT_EVENT_HDR_SIZE_VER2;
    }
    else {
      fprintf(stderr, "unsupported version %d\n", version);
      exit(EXIT_FAILURE);
    }

#ifdef WAVELOG_DEBUG
    fprintf(stderr, "\nsize: %u\n", size);
#endif

    if (size >= 0) {
      pcaprec_hdr_t pcaprec_hdr;
      int remaining_size = size;
      int wait_all_flag = MSG_WAITALL;
#ifdef WAVELOG_DEBUG
      int packet_offset = 0;
#endif

#ifdef WIN32
      wait_all_flag = IsWinXP() ? 0 : MSG_WAITALL;
#endif

      pcaprec_hdr.ts_sec  = log_event.timestamp / 1000;
      pcaprec_hdr.ts_usec = (log_event.timestamp % 1000) * 1000;
      pcaprec_hdr.incl_len = pcaprec_hdr.orig_len
        = log_event_hdr_size + size;

      fwrite((char *)&pcaprec_hdr, sizeof(pcaprec_hdr), 1, stdout);
      fwrite((char *)&log_event_orig, log_event_hdr_size, 1, stdout);

      while (remaining_size > 0) {
        int packet_size = min(remaining_size, BUFFER_SIZE);
        rc = recv(sock, buffer, packet_size, wait_all_flag);

        if (rc == 0) {
          fprintf(stderr, "Connection closed\n");
        }
        else if (rc < 0) {
          fprintf(stderr, "recv() failed\n");
        }
        else {
#ifdef WAVELOG_DEBUG
          fprintf(stderr, "\nPacket received (lengh: %u, expected: %u):\n", rc, packet_size);
          hexdump(&buffer, rc, &packet_offset);
#endif
          fwrite(buffer, rc, 1, stdout);
          remaining_size -= rc;
        }
      }
    }

    fflush(stdout);
  }
}
