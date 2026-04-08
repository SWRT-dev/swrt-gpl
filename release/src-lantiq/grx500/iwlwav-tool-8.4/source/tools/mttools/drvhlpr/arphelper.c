/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "arphelper.h"
#include "dataex.h"
#include "mtlkirba.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <arpa/inet.h>

#define LOG_LOCAL_GID   GID_ARPHELPER
#define LOG_LOCAL_FID   1

#define ETH_HW_ADDR_LEN ETH_ALEN
#define IP_ADDR_LEN     4
#define ARP_FRAME_TYPE  0x0806
#define ETHER_HW_TYPE   1
#define IP_PROTO_TYPE   0x0800
#define OP_ARP_REQUEST  1

extern mtlk_irba_t *mtlk_irba_wlan;

struct arp_packet {
  u_char targ_hw_addr[ETH_HW_ADDR_LEN];
  u_char src_hw_addr[ETH_HW_ADDR_LEN];
  u_short frame_type;
  u_short hw_type;
  u_short prot_type;
  u_char hw_addr_size;
  u_char prot_addr_size;
  u_short op;
  u_char sndr_hw_addr[ETH_HW_ADDR_LEN];
  u_char sndr_ip_addr[IP_ADDR_LEN];
  u_char rcpt_hw_addr[ETH_HW_ADDR_LEN];
  u_char rcpt_ip_addr[IP_ADDR_LEN];
  u_char padding[18];
};

struct arp_iface
{
  char *name;
  struct arp_iface *next;
};

static struct arp_iface *arp_iface_lst = NULL;
static int               arp_sock_fd   = -1;

static const mtlk_guid_t IRBE_ARP_REQ = MTLK_IRB_GUID_ARP_REQ;

static const uint8       BCAST_HW_ADDR[ETH_ALEN] = { 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
};

static int
cmd_do_arp(struct mtlk_arp_data *parp_data, const char *iface)
{
  struct arp_packet pkt;
  struct sockaddr_ll ll;
  struct ifreq ifr;
  int retval = 0;

  ASSERT(sizeof(pkt.src_hw_addr) == sizeof(parp_data->smac));
  ASSERT(sizeof(pkt.sndr_hw_addr) == sizeof(parp_data->smac));
  ASSERT(sizeof(pkt.sndr_ip_addr) == sizeof(parp_data->saddr));
  ASSERT(sizeof(pkt.rcpt_ip_addr) == sizeof(parp_data->daddr));

  memset(&pkt, 0, sizeof(pkt));

  memcpy(pkt.targ_hw_addr, BCAST_HW_ADDR, ETH_ALEN);
  memcpy(pkt.src_hw_addr, parp_data->smac, sizeof(parp_data->smac));
  pkt.frame_type = htons(ARP_FRAME_TYPE);
  pkt.hw_type = htons(ETHER_HW_TYPE);
  pkt.prot_type = htons(IP_PROTO_TYPE);
  pkt.hw_addr_size = ETH_HW_ADDR_LEN;
  pkt.prot_addr_size = IP_ADDR_LEN;
  pkt.op = htons(OP_ARP_REQUEST);
  memcpy(pkt.sndr_hw_addr, parp_data->smac, sizeof(parp_data->smac));
  memcpy(pkt.sndr_ip_addr, &parp_data->saddr, sizeof(parp_data->saddr));
  memcpy(pkt.rcpt_hw_addr, BCAST_HW_ADDR, ETH_ALEN);
  memcpy(pkt.rcpt_ip_addr, &parp_data->daddr, sizeof(parp_data->daddr));

  memset(&ifr, 0, sizeof(ifr));
  if (wave_strlen(iface, sizeof(ifr.ifr_name)) >= sizeof(ifr.ifr_name)) {
    ELOG_SD("Interface \"%s\" name is too long (maximum %d)",
      iface, sizeof(ifr.ifr_name)-1);
    retval = -1;
    goto end;
  }
  wave_strcopy(ifr.ifr_name, iface, sizeof(ifr.ifr_name));
  if (ioctl(arp_sock_fd, SIOCGIFINDEX, &ifr) < 0) {
    ELOG_S("SIOCGIFINDEX: %s", strerror(errno));
    retval = -1;
    goto end;
  }
  memset(&ll, 0, sizeof(ll));
  ll.sll_family = AF_PACKET;
  ll.sll_ifindex = ifr.ifr_ifindex;
  ll.sll_protocol = htons(ETH_P_RARP);
  ll.sll_halen = ETH_ALEN;
  memcpy(ll.sll_addr, BCAST_HW_ADDR, ETH_ALEN);

  if (sendto(arp_sock_fd, &pkt, sizeof(pkt), 0, (struct sockaddr *) &ll,
               sizeof(ll)) < 0) {
    ELOG_S("While sending ARP packet: %s", strerror(errno));
    retval = -1;
    goto end;
  }

end:
  return retval;
}

static int
arp_register_iface(const char *iface)
{
  int retval = MTLK_ERR_UNKNOWN;
  struct arp_iface *aif = malloc(sizeof(struct arp_iface));

  if (aif == NULL) {
    ELOG_V("Out of memory");
    retval = MTLK_ERR_NO_MEM;
    goto end;
  }
  memset(aif, 0, sizeof(struct arp_iface));
  aif->name = strdup(iface);
  if (!aif->name) {
    ELOG_V("Out of memory");
    retval = MTLK_ERR_NO_MEM;
    goto end;
  }
  aif->next = arp_iface_lst;
  arp_iface_lst = aif;

  retval = MTLK_ERR_OK;

end:
  if (retval != MTLK_ERR_OK) {
    if (aif) {
      if (aif->name)
        free(aif->name);
      free(aif);
    }
  }
  return retval;
}

static void __MTLK_IFUNC
_irb_arp_req_handler (mtlk_irba_t       *irba,
                      mtlk_handle_t      context,
                      const mtlk_guid_t *evt,
                      void              *buffer,
                      uint32            size)
{
  struct arp_iface     *aif;
  struct mtlk_arp_data *arp_data = (struct mtlk_arp_data *)buffer;

  MTLK_UNREFERENCED_PARAM(context);
  MTLK_ASSERT(mtlk_guid_compare(&IRBE_ARP_REQ, evt) == 0);
  MTLK_ASSERT(sizeof(struct mtlk_arp_data) == size);

  if (!arp_iface_lst) {
    WLOG_V("No interfaces to send ARP request to.");
  }

  for (aif = arp_iface_lst; aif; aif = aif->next) {
    ILOG1_S("ARP sending to %s", aif->name);
    if (0 != cmd_do_arp(arp_data, aif->name)) {
      WLOG_S("Send ARP to %s failed", aif->name);
    }
  }
}

static mtlk_handle_t
arp_helper_start (void)
{
  mtlk_handle_t res = HANDLE_T(0);

  arp_sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_RARP));
  if (arp_sock_fd < 0) {
    ELOG_S("Cannot open ARP packet interface: %s", strerror(errno));
    goto socket_open_failed;
  }

  MTLK_ASSERT(NULL != mtlk_irba_wlan);

  res = HANDLE_T(mtlk_irba_register(mtlk_irba_wlan, &IRBE_ARP_REQ, 1, _irb_arp_req_handler, 0));
  if (!res) {
    ELOG_V("Cannot register IRB handler");
    goto irb_register_failed;
  }
  return res;

irb_register_failed:
  close(arp_sock_fd);
  arp_sock_fd = 0;
socket_open_failed:
  return HANDLE_T(0);
}

static void
arp_helper_stop (mtlk_handle_t ctx)
{
  struct arp_iface *next;

  mtlk_irba_unregister(mtlk_irba_wlan, HANDLE_T_PTR(mtlk_irba_handle_t, ctx));
  close(arp_sock_fd);
  arp_sock_fd = 0;

  /* Cleanup iterface list */
  while (arp_iface_lst) {
    next = arp_iface_lst->next;
    free(arp_iface_lst);
    arp_iface_lst = next;
  }
}

const mtlk_component_api_t
irb_arp_helper_api = {
  arp_helper_start,
  NULL,
  arp_helper_stop
};

int
arp_helper_register_iface (const char *iface) {
  return arp_register_iface(iface);
}
