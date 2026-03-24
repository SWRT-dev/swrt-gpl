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
 * $Id$
 *
 *
 *
 * Core functionality
 *
 */

#ifdef CPTCFG_IWLWAV_ETH_PARSER

#include "mtlkinc.h"
#include "eth_parser.h"


#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/igmp.h>
#include <linux/if_arp.h>
#include <net/icmp.h>

#define LOG_LOCAL_GID   GID_ETH_PARSER
#define LOG_LOCAL_FID   1


#define MTLK_IP4_ALEN (4) /* TODO move to mtlkinc.h*/

#define   DHCP_DISCOVER   1
#define   DHCP_OFFER      2
#define   DHCP_REQUEST    3
#define   DHCP_DECLINE    4
#define   DHCP_ACK        5
#define   DHCP_NAK        6
#define   DHCP_RELEASE    7
#define   DHCP_INFORM     8

struct dhcpopt_hdr {
  u8      code;
#define DHCP_OPT_REQ_IP   50
#define DHCP_OPT_MSG_TYPE 53
#define DHCP_OPT_SRV_ID   54
#define DHCP_OPT_PAD      0
#define DHCP_OPT_END      255
  u8      len;
} __attribute__((aligned(1), packed));

struct dhcpopt_msg_type {
  struct dhcpopt_hdr hdr;
#define DHCPREQUEST     3
  u8                 type;
}__attribute__((aligned(1), packed));

struct dhcphdr {
  u8      op;
#define BOOTREQUEST   1
#define BOOTREPLY     2
  u8      htype;
  u8      hlen;
  u8      hops;
  u32     xid;
  u16     secs;
  u16     flags;
#define BOOTP_BRD_FLAG 0x8000
  u32     ciaddr;
  u32     yiaddr;
  u32     siaddr;
  u32     giaddr;
  u8      chaddr[16];
  u8      sname[64];
  u8      file[128];
  u32     magic; /* NB: actually magic is a part of options */
  u8      options[0];
} __attribute__((aligned(1), packed));

static inline
struct dhcpopt_hdr *dhcp_find_opt(uint8 *end,
  struct dhcpopt_hdr *hdr, u8 opt_code)
{
  ILOG4_D("Looking for %d", opt_code);
  while (hdr->code != DHCP_OPT_END && hdr->code != DHCP_OPT_PAD &&
         hdr->code != opt_code && (u8 *)hdr < end) {
    ILOG4_DDD("option %d (0x%02x), length %d", hdr->code, hdr->code, hdr->len);
    hdr = (struct dhcpopt_hdr *)((u8 *)hdr + hdr->len + sizeof(*hdr));
  }
  if (hdr->code == opt_code)
    return hdr;
  return NULL;
}


#define ANSI_COLOR_RESET    "\033[0m"
#define ANSI_COLOR_BLACK    "\033[30m"
#define ANSI_COLOR_RED      "\033[31m"
#define ANSI_COLOR_GREEN    "\033[32m"
#define ANSI_COLOR_YELLOW   "\033[33m"
#define ANSI_COLOR_BLUE     "\033[34m"
#define ANSI_COLOR_MAGENTA  "\033[35m"
#define ANSI_COLOR_CYAN     "\033[36m"
#define ANSI_COLOR_WHITE    "\033[37m"


void mtlk_eth_parser (void *data, int length, const char *if_name, const char *f)
{
  struct ethhdr *ether_header = (struct ethhdr *)data;

  if (length < sizeof(*ether_header)) {
    ILOG1_V("Ethernet Frame length is wrong");
    return;
  }
  ILOG4_Y("802.3 tx DA: %Y", ether_header->h_dest);
  ILOG4_Y("802.3 tx SA: %Y", ether_header->h_source);

  /* ARP */
  if((ntohs(ether_header->h_proto) == ETH_P_ARP) || (ntohs(ether_header->h_proto) == ETH_P_RARP)) {
    struct arphdr *arp_hdr = (struct arphdr *)(data + sizeof(struct ethhdr));
    uint8         *arp_data = (uint8 *)(arp_hdr + 1);
    uint8         *sha = arp_data;                                       /* Sender hardware address */
    uint8         *spa = arp_data + ETH_ALEN;                            /* Sender IP address       */
    uint8         *tha = arp_data + ETH_ALEN + MTLK_IP4_ALEN;            /* Target hardware address */
    uint8         *tpa = arp_data + ETH_ALEN + MTLK_IP4_ALEN + ETH_ALEN; /* Target IP address       */

    if (length < sizeof(struct ethhdr) + sizeof(*arp_hdr) +
      ETH_ALEN + MTLK_IP4_ALEN + ETH_ALEN + 4) {
      ILOG1_V("ARP/RARP Ethernet Frame length is wrong");
      return;
    }
    MTLK_UNREFERENCED_PARAM(sha);
    MTLK_UNREFERENCED_PARAM(spa);
    MTLK_UNREFERENCED_PARAM(tha);
    MTLK_UNREFERENCED_PARAM(tpa);

    if (ntohs(arp_hdr->ar_op) == ARPOP_REQUEST) {
      ILOG2_SSYYDDDDDDDD("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_RED "ARP Who has" ANSI_COLOR_RESET " %d.%d.%d.%d " ANSI_COLOR_RED "Tell" ANSI_COLOR_RESET " %d.%d.%d.%d",
                        if_name, f, ether_header->h_source,
                        ether_header->h_dest, tpa[0], tpa[1], tpa[2], tpa[3], spa[0], spa[1], spa[2], spa[3]);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_REPLY) {
      ILOG2_SSYYDDDDY("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_RED "ARP" ANSI_COLOR_RESET " %d.%d.%d.%d " ANSI_COLOR_RED "is at" ANSI_COLOR_RESET " %Y",
                     if_name, f, ether_header->h_source, ether_header->h_dest,
                     spa[0], spa[1], spa[2], spa[3], sha);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_RREQUEST) {
      ILOG2_SSYYDDDDDDDD("%s: %s SRC:%Y DST:%Y RARP Who has %d.%d.%d.%d Tell %d.%d.%d.%d",
                        if_name, f, ether_header->h_source,
                        ether_header->h_dest, tpa[0], tpa[1], tpa[2], tpa[3], spa[0], spa[1], spa[2], spa[3]);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_RREPLY) {
      ILOG2_SSYYDDDDY("%s: %s SRC:%Y DST:%Y RARP %d.%d.%d.%d is at %Y",
                     if_name, f, ether_header->h_source, ether_header->h_dest,
                     spa[0], spa[1], spa[2], spa[3], sha);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_InREQUEST) {
      ILOG2_SSYYDDDDDDDD("%s: %s SRC:%Y DST:%Y InARP Who has %d.%d.%d.%d Tell %d.%d.%d.%d",
                        if_name, f, ether_header->h_source,
                        ether_header->h_dest, tpa[0], tpa[1], tpa[2], tpa[3], spa[0], spa[1], spa[2], spa[3]);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_InREPLY) {
      ILOG2_SSYYDDDDY("%s: %s SRC:%Y DST:%Y InARP %d.%d.%d.%d is at %Y",
                     if_name, f, ether_header->h_source, ether_header->h_dest,
                     spa[0], spa[1], spa[2], spa[3], sha);
    }
    else if (ntohs(arp_hdr->ar_op) == ARPOP_NAK) {
      ILOG2_SSYYDDDDY("%s: %s SRC:%Y DST:%Y ARP_NAK %d.%d.%d.%d is at %Y",
                     if_name, f, ether_header->h_source, ether_header->h_dest,
                     spa[0], spa[1], spa[2], spa[3], sha);
    }
    else {
      ILOG2_SSYYDDDDDDDD("%s: %s SRC:%Y DST:%Y ARP_UNKNOWN Trg %d.%d.%d.%d Src %d.%d.%d.%d",
                        if_name, f, ether_header->h_source,
                        ether_header->h_dest, tpa[0], tpa[1], tpa[2], tpa[3], spa[0], spa[1], spa[2], spa[3]);
    }
  }
  /* IPv4 */
  else if (ntohs(ether_header->h_proto) == ETH_P_IP) {
    struct in_addr ip_src_addr,ip_dst_addr;
    struct iphdr *iph = (struct iphdr *)(data + sizeof(struct ethhdr));
    unsigned iphdrlen;

    if (length < sizeof(struct ethhdr) + sizeof(*iph)) {
      ILOG1_V("IP Ethernet Frame length is wrong");
      return;
    }
    iphdrlen = iph->ihl*4;
    ip_src_addr.s_addr = ntohl(iph->saddr);
    ip_dst_addr.s_addr = ntohl(iph->daddr);

    if ((iph->protocol) == IPPROTO_TCP) {
      struct tcphdr *tcph=(struct tcphdr*)((uint8 *)iph + iphdrlen);

      if (length < sizeof(struct ethhdr) + iphdrlen + sizeof(*tcph)) {
        ILOG1_V("TCP Ethernet Frame length is wrong");
        return;
      }
      MTLK_UNREFERENCED_PARAM(tcph);

      ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y TCP src_ip %B dst_ip %B src_port %d dst_port %d",
                    if_name, f, ether_header->h_source,
                    ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(tcph->source), ntohs(tcph->dest));
    }
    else if ((iph->protocol) == IPPROTO_UDP) {
      struct udphdr *udph = (struct udphdr *)((uint8 *)iph + iphdrlen);

      if (length < sizeof(struct ethhdr) + iphdrlen + sizeof(*udph)) {
        ILOG1_V("UDP Ethernet Frame length is wrong");
        return;
      }
      if ((htons(udph->dest) == 67) || (htons(udph->dest) == 68)) {
        struct dhcphdr *dhcp_header= (struct dhcphdr *)((char *)udph + sizeof(struct udphdr));
        struct dhcpopt_hdr *opt_header;
        char const *msg_type;

        if (length < sizeof(struct ethhdr) + iphdrlen + sizeof(*udph) +
          sizeof(*dhcp_header)) {
          ILOG1_V("DHCP Ethernet Frame length is wrong");
          return;
        }
        opt_header = dhcp_find_opt(data + length , (struct dhcpopt_hdr *)dhcp_header->options, DHCP_OPT_MSG_TYPE);
        if (opt_header)
        {
           switch (((struct dhcpopt_msg_type *)opt_header)->type) {
             case DHCP_DISCOVER: msg_type = "discover"; break;
             case DHCP_OFFER:    msg_type = "offer"; break;
             case DHCP_REQUEST:  msg_type = "request"; break;
             case DHCP_ACK:      msg_type = "ack"; break;
             case DHCP_NAK:      msg_type = "nack"; break;
             case DHCP_DECLINE:  msg_type = "decline"; break;
             case DHCP_RELEASE:  msg_type = "release"; break;
             case DHCP_INFORM:   msg_type = "inform"; break;
             default:            msg_type = "type unknown"; break;
           }
           ILOG2_SSYYSDDDD("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_GREEN "DHCP %s" ANSI_COLOR_RESET " src_ip %B dst_ip %B src_port %d dst_port %d",
                          if_name, f, ether_header->h_source,
                          ether_header->h_dest, msg_type, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(udph->source), ntohs(udph->dest));
        }
        else {
           ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_GREEN "DHCP unknown" ANSI_COLOR_RESET " src_ip %B dst_ip %B src_port %d dst_port %d",
                         if_name, f, ether_header->h_source,
                         ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(udph->source), ntohs(udph->dest));
        }
      }
      else if ((htons(udph->dest) == 1900) || (htons(udph->source) == 1900)) {
        ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y SSDP src_ip %B dst_ip %B src_port %d dst_port %d",
                      if_name, f, ether_header->h_source,
                      ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(udph->source), ntohs(udph->dest));
      }
      else if (htons(udph->dest) == 53) {
        ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y DNS src_ip %B dst_ip %B src_port %d dst_port %d",
                      if_name, f, ether_header->h_source,
                      ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(udph->source), ntohs(udph->dest));
      }
      else {
        ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y UDP src_ip %B dst_ip %B src_port %d dst_port %d",
                      if_name, f, ether_header->h_source,
                      ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(udph->source), ntohs(udph->dest));
      }
    }
    else if ((iph->protocol) == IPPROTO_ICMP) {
      struct icmphdr *icmph = (struct icmphdr *)((uint8 *)iph + iphdrlen);
      if((unsigned int)(icmph->type) == ICMP_ECHOREPLY) {
        ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_BLUE "ICMP (ping) reply " ANSI_COLOR_RESET " src_ip %B dst_ip %B id %d sequence %d",
                      if_name, f, ether_header->h_source,
                      ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(icmph->un.echo.id), ntohs(icmph->un.echo.sequence));
      }
      else if((unsigned int)(icmph->type) == ICMP_ECHO) {
        ILOG2_SSYYDDDD("%s: %s SRC:%Y DST:%Y " ANSI_COLOR_BLUE "ICMP (ping) request" ANSI_COLOR_RESET " src_ip %B dst_ip %B id %d sequence %d",
                      if_name, f, ether_header->h_source,
                      ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr, ntohs(icmph->un.echo.id), ntohs(icmph->un.echo.sequence));
      }
      else if((unsigned int)(icmph->type) == ICMP_DEST_UNREACH) {
        ILOG2_SSYYDD("%s: %s SRC:%Y DST:%Y ICMP unreachable src_ip %B dst_ip %B",
                    if_name, f, ether_header->h_source,
                    ether_header->h_dest, ip_src_addr.s_addr, ip_dst_addr.s_addr);
      }
      else {
        ILOG2_SSYYD("%s: %s SRC:%Y DST:%Y ICMP unknown type %d",
                   if_name, f, ether_header->h_source,
                   ether_header->h_dest, icmph->type);
        mtlk_dump(2, data, length, "eth packet dump");

      }
    }
    else if ((iph->protocol) == IPPROTO_IGMP) {
      struct igmphdr *igmp_header = (struct igmphdr *) ((uint8 *)iph + iphdrlen);
      char const *msg_type;

      if (length < sizeof(struct ethhdr) + iphdrlen + sizeof(*igmp_header)) {
        ILOG1_V("IGMP Ethernet Frame length is wrong");
        return;
      }
      switch (igmp_header->type) {
        case IGMP_HOST_MEMBERSHIP_QUERY: msg_type = "Membership query"; break;
        case IGMP_HOST_MEMBERSHIP_REPORT:  msg_type = "Membership report"; break;
        case IGMPV2_HOST_MEMBERSHIP_REPORT: msg_type = "MembershipV2 report"; break;
        case IGMP_HOST_LEAVE_MESSAGE: msg_type = "Host leave"; break;
        case IGMPV3_HOST_MEMBERSHIP_REPORT: msg_type = "MembershipV3 report"; break;
        default: msg_type = "Unknown IGMP message"; break;
      }
      ILOG2_SSYYS("%s: %s SRC:%Y DST:%Y IGMP %s", if_name, f, ether_header->h_source, ether_header->h_dest, msg_type);
    }
    else {
      ILOG2_SSYYD("%s: %s SRC:%Y DST:%Y UNKNOWN IP protocol %d", if_name, f, ether_header->h_source, ether_header->h_dest, iph->protocol);
      mtlk_dump(2, data, length, "eth packet dump");
    }
  }
  /* IPv6 */
  else if (ntohs(ether_header->h_proto) == ETH_P_IPV6) {
    struct ipv6hdr *iph = (struct ipv6hdr *)(data + sizeof(struct ethhdr));

    if (length < sizeof(struct ethhdr) + sizeof(*iph)) {
      ILOG1_V("IPV6 Ethernet Frame length is wrong");
      return;
    }

    ILOG2_SSYYKK("%s: %s SRC:%Y DST:%Y IPv6 src_ip %K dst_ip %K", if_name, f, ether_header->h_source,
                  ether_header->h_dest, iph->saddr.s6_addr, iph->daddr.s6_addr);
  }
  /* All Others */
  else {
    ILOG2_SSYYD("%s: %s SRC:%Y DST:%Y UNKNOWN packet type (0x%04x)", if_name, f, ether_header->h_source, ether_header->h_dest, ntohs(ether_header->h_proto));
    mtlk_dump(2, data, length, "eth packet dump");
  }
}

#endif /* CPTCFG_IWLWAV_ETH_PARSER */
