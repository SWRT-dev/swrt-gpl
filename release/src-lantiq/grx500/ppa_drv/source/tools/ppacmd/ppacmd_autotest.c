/******************************************************************************
**
** FILE NAME  : ppacmd.c
** PROJECT    : PPA Configuration Utility
** MODULES    : Packet Acceleration
**
** DATE     : 28 Nov 2011
** AUTHOR     : Shao Guohua
** DESCRIPTION  : PPA (Routing Acceleration) User Configuration Utility
** COPYRIGHT  :        Copyright (c) 2009
**              Lantiq Deutschland GmbH
**           Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
**
**
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>

#include "ppacmd.h"


#if defined(PPA_TEST_AUTOMATION_ENABLE) && PPA_TEST_AUTOMATION_ENABLE
/******************************************************************************************************
Note, below is the code for PPA test automation only., not for PPA ioctl at all  --- start
*****************************************************************************************************/
#include <net/if.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <asm/ioctl.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
//#include <linux/filter.h>
#include <stdarg.h>
#include <sys/stat.h>

#define PPA_TEST_AUTOMATION_MAC_LEN 6
#define PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN  1200
#define PPA_TEST_AUTOMATION_MAX_VLAN_HEADER_ID  ( 4 * 2)


#define PPA_TEST_AUTOMATION_MAGIC 0x2345
#define PPA_TEST_AUTOMATION_TYPE 0xFEDC
#define PPA_TEST_AUTOMATION_VLAN 0x8100
#define PPA_TEST_AUTOMATION_MAJOR_VER  1
#define PPA_TEST_AUTOMATION_MINOR_VER  0

#define PPA_TEST_AUTOMATION_CMD_ACK                0
#define PPA_TEST_AUTOMATION_CMD_GET_FILE                1
#define PPA_TEST_AUTOMATION_CMD_GET_FILE_REPLY    2
#define PPA_TEST_AUTOMATION_CMD_PUT_FILE                3
#define PPA_TEST_AUTOMATION_CMD_PUT_FILE_REPLY    4   //for future if necessary
#define PPA_TEST_AUTOMATION_CMD_DEL_FILE                5
#define PPA_TEST_AUTOMATION_CMD_DEL_FILE_REPLY    6  //for future if necessary
#define PPA_TEST_AUTOMATION_CMD_RUN_FILE               7
#define PPA_TEST_AUTOMATION_CMD_RUN_FILE_REPLY   8  //for future if necessary
#define PPA_TEST_AUTOMATION_CMD_RUN_CMD              9
#define PPA_TEST_AUTOMATION_CMD_RUN_CMD_REPLY   10  //for future if necessary


#define PPA_TEST_AUTOMATION_RESULT_OK  0
#define PPA_TEST_AUTOMATION_RESULT_NO_MEM  1
#define PPA_TEST_AUTOMATION_RESULT_NO_SRC_MAC  2
#define PPA_TEST_AUTOMATION_RESULT_SEND_FAIL 3
#define PPA_TEST_AUTOMATION_RESULT_RECV_ACK_FAIL 4
#define PPA_TEST_AUTOMATION_RESULT_RECV_BUFFER_FAIL 5
#define PPA_TEST_AUTOMATION_RESULT_NO_FILE 6
#define PPA_TEST_AUTOMATION_RESULT_PARAM_WRONG 7
#define PPA_TEST_AUTOMATION_RESULT_FILE_NO_DATA 8

#define PPA_TEST_AUTOMATION_MILLION 1000000
#define PPA_TEST_AUTOMATION_BUFFER_TIMEOUT  (2 * PPA_TEST_AUTOMATION_MILLION  )

#define MAX_TEST_AUTOMATION_CMD_BUF_LEN  200

#define PPA_TEST_AUTOMATION_AUTO_ID_MANAGER 1
#define PPA_TEST_AUTOMATION_AUTO_ID_CPE 2
#define PPA_TEST_AUTOMATION_AUTO_ID_WAN 3

#define PPA_TEST_AUTOMATION_MAX_SESSION_NAME 8

#define PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT 1

typedef struct PPA_TEST_AUTOMATION_PKT_HDR
{
    unsigned char mac_dst[PPA_TEST_AUTOMATION_MAC_LEN];
    unsigned char mac_src[PPA_TEST_AUTOMATION_MAC_LEN];

    unsigned short type; //need check type
    unsigned short magic; //need check magic

    unsigned char major_ver;
    unsigned char minor_ver;
    unsigned char sender;  // 1 --manager  2-cpe, 3-wan
    unsigned char receiver;  // 1 --manager  2-cpe, 3-wan

    char session_name[PPA_TEST_AUTOMATION_MAX_SESSION_NAME];  //support multpile device automatic testing at the same time, use this variant to differentiate
    unsigned int    pid; //need check pid. for sender, need fill in its pid, but for ack reply, copy sender's pid, it should work  with id

    unsigned char id;//need check pid. for sender, need fill in its pid, but for ack reply, copy sender's id, it should work with pid
    unsigned char cmd;
    unsigned char tries;
    unsigned char second_receiver;/*must be none zero in order to be compatible with old version */

    unsigned short pkt_index;
    unsigned short pkts;
    unsigned short pkt_len; //current packet's real data length, not including header
    unsigned char reserved[2]; 

    unsigned char cmdbuf[MAX_TEST_AUTOMATION_CMD_BUF_LEN];
} __attribute__((packed)) PPA_TEST_AUTOMATION_PKT_HDR;

#define PPA_TEST_AUTOMATION_MAX_FILENAME_LEN (MAX_TEST_AUTOMATION_CMD_BUF_LEN)

typedef struct PPA_TEST_AUTOMATION_PKT
{
    struct PPA_TEST_AUTOMATION_PKT_HDR hdr;
    unsigned char buf[PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN];
} __attribute__((packed)) PPA_TEST_AUTOMATION_PKT;


int ppa_test_automation_get_if_mac(char *ifname, char *mac)
{
    int sock,res;
    struct ifreq ifr;

    memset (&ifr,0,sizeof(struct ifreq));

    /* Dummy socket, just to make ioctls with */
    sock=socket(PF_INET,SOCK_DGRAM,0);

    /* name of the card */
    /***** How are NIC devices named in SFU? *****/
    strcpy_s(ifr.ifr_name, 20, ifname); 

    res=ioctl(sock, SIOCGIFHWADDR, &ifr);
    if (res<0)
    {
        return -1;
    }

    if ( mac )
        memcpy( mac, ifr.ifr_ifru.ifru_hwaddr.sa_data, 6);

    close(sock);

    return 0;
}

#define USE_RAW_SOCKET 1

#ifdef USE_RAW_SOCKET

#define PCAP_ERRBUF_SIZE 1 //not used at all

typedef int pcap_t;  //raw sock id
typedef unsigned long bpf_u_int32;

struct bpf_program
{
    u_int bf_len;
    struct sock_filter *bfp_filter_code;
};

struct pcap_pkthdr
{
    struct timeval ts;  /* time stamp */
    int caplen; /* length of portion present */
    int len;    /* length this packet (off wire) */
};


pcap_t *pcap_open_live(const char *source, int snaplen, int promisc, int to_ms, char *errbuf);
const u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h);
int pcap_sendpacket(pcap_t *p, const u_char *buf, int size);
void pcap_close(pcap_t *p);
int pcap_lookupnet(const char *, bpf_u_int32 *, bpf_u_int32 *, char *);
int pcap_compile(pcap_t *p, struct bpf_program *program, const char *buf, int optimize, bpf_u_int32 mask);
int pcap_setfilter(pcap_t *p, struct bpf_program *fp);
#endif

#define PPA_AUTO_PROXY_CPE 1

int ppa_test_automation_vid = -1;

#define PPA_AUTO_DEBUG_RECV     0x1
#define PPA_AUTO_DEBUG_SEND     0x2
#define PPA_AUTO_DEBUG_CMD      0x4
#define PPA_AUTO_DEBUG_RECV_DETAIL     0x8
#define PPA_AUTO_DEBUG_SEND_DETAIL     0x10
#define PPA_AUTO_DEBUG_PARAM     0x20
#define PPA_AUTO_DEBUG_CFG          0x40
#define PPA_AUTO_DEBUG_OTHER 0xF0000000
static unsigned int enable_ppa_automation_debug =0;

#define PPA_TEST_AUTOMATION_MAX_SEND_TRY 3
#define PPA_TEST_AUTOMATION_VLAN_TAG_LEN 4

//recv buffer status flag
#define PPA_TEST_AUTOMATION_BUFFER_NOT_READY 0
#define PPA_TEST_AUTOMATION_BUFFER_RECVING 1
#define PPA_TEST_AUTOMATION_BUFFER_OK  2

unsigned char ppa_auto_protocol_ack_per_pkt= 1;
unsigned char ppa_auto_capture_mode_only= 0;
static unsigned char ppa_test_automation_id = 0;

char *ppa_test_automation_tmp_file_fomat="/var/ppa_auto_tmpxx_%05d";
unsigned char second_receiver=0;

typedef struct ppa_test_automation_if_attr
{
    pcap_t* pcap_descr;
    char ifname[IFNAMSIZ];
} ppa_test_automation_if_attr;

#define PPA_TEST_AUTOMATION_MAX_BUFFER_NUM 2
unsigned int read_buffer_status[PPA_TEST_AUTOMATION_MAX_BUFFER_NUM];
struct PPA_TEST_AUTOMATION_PKT_HDR *read_buffer[PPA_TEST_AUTOMATION_MAX_BUFFER_NUM];
struct timeval read_buffer_last_time[PPA_TEST_AUTOMATION_MAX_BUFFER_NUM];

char ppa_test_automation_session_name[PPA_TEST_AUTOMATION_MAX_SESSION_NAME]="sgh";
struct ifreq ppa_test_automation_ifr;

ppa_test_automation_if_attr ppa_test_automation_if_listening= {NULL, ""};
unsigned char peer_mac_addr[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char local_mac_addr[]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


static int ppa_auto_recv_ack(PPA_TEST_AUTOMATION_PKT *orig_pkt, ppa_test_automation_if_attr *ifattr);
int send_ack(PPA_TEST_AUTOMATION_PKT *orig_pkt, char *ifname);

int send_secure_pkt(PPA_TEST_AUTOMATION_PKT *pkt, ppa_test_automation_if_attr *ifattr, int max_tries, int need_check_ack);
//static int send_buffer_via_if(unsigned char *buf, int nr, unsigned char cmd, char *mac_dst, ppa_test_automation_if_attr *ifattr, unsigned char receiver);
static int recv_buffer_via_if(ppa_test_automation_if_attr *ifattr, long max_ms, int wait_cmd);

int create_buffer( PPA_TEST_AUTOMATION_PKT *pkt);
int ppa_test_automation_put_to_buffer( PPA_TEST_AUTOMATION_PKT *pkt, int index);

long ppa_test_automation_get_time_diff( struct timeval *start, struct timeval *end );
int run_linux_cmd(char *script_cmd_file, char *start_in_folder, unsigned char run_fore_ground);
int time_expire(struct timeval* start, long duration);


void DEBUG_PRINT( int type, char* format, ...)
{
    va_list args;

    if (  ! (enable_ppa_automation_debug & type )  ) return ;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

int file_exist( char *filename)
{
    struct stat statBuf;

    if ( stat( filename, &statBuf ) < 0 )
        return 0;

    return !S_ISDIR( statBuf.st_mode );
}


unsigned char get_my_id()
{
#ifdef PPA_AUTO_PROXY_CPE
    return PPA_TEST_AUTOMATION_AUTO_ID_CPE;
#elif PPA_AUTO_PROXY_WAN
    return PPA_TEST_AUTOMATION_AUTO_ID_WAN;
#else
    return PPA_TEST_AUTOMATION_AUTO_ID_MANAGER;
#endif
}

/*
 * 1 --- from correct sender
 * 0-- not from correct sender
*/
int correct_sender_receiver(PPA_TEST_AUTOMATION_PKT *pkt )
{
    int correct_sender = 0;
    if( second_receiver != pkt->hdr.second_receiver ) 
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"2nd receiver not match:%u<->%u\n", pkt->hdr.second_receiver, second_receiver);
        return correct_sender;
    }
    
#ifdef PPA_AUTO_PROXY_CPE //cpe only receive ppa automation protocol from manager
    if ( pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_MANAGER && pkt->hdr.receiver == PPA_TEST_AUTOMATION_AUTO_ID_CPE)
        correct_sender = 1;
#elif PPA_AUTO_PROXY_WAN // wan only receive ppa automation protocol from manager
    if ( pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_MANAGER && pkt->hdr.receiver == PPA_TEST_AUTOMATION_AUTO_ID_WAN)
        correct_sender = 1;
#else
    if ( (pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_CPE || pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_WAN )  &&
            pkt->hdr.receiver == PPA_TEST_AUTOMATION_AUTO_ID_MANAGER )
        correct_sender = 1;
#endif
    return correct_sender;
}

void update_peer_mac_address(PPA_TEST_AUTOMATION_PKT *pkt )
{
    if( get_my_id() == PPA_TEST_AUTOMATION_AUTO_ID_CPE)
    {
        if ( pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_MANAGER )
        {
            memcpy(peer_mac_addr, pkt->hdr.mac_src, PPA_TEST_AUTOMATION_MAC_LEN );
        }
    }
    else if( get_my_id() == PPA_TEST_AUTOMATION_AUTO_ID_WAN)
    {
        if ( pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_MANAGER )
        {
            memcpy(peer_mac_addr, pkt->hdr.mac_src, PPA_TEST_AUTOMATION_MAC_LEN );
        }
    }
    else
    {
        if ( (pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_CPE)  || (pkt->hdr.sender == PPA_TEST_AUTOMATION_AUTO_ID_WAN ) )
            memcpy(peer_mac_addr, pkt->hdr.mac_src, PPA_TEST_AUTOMATION_MAC_LEN );
    }
}

void set_defaul_buffer_status(int i)
{
    if ( i<PPA_TEST_AUTOMATION_MAX_BUFFER_NUM && i >= 0 )
    {
        read_buffer_status[i]=PPA_TEST_AUTOMATION_BUFFER_NOT_READY;
        read_buffer[i] = NULL;
    }
}

void remove_vlan( unsigned char *p, unsigned int *len)
{
    int f_vlan=0;
    int enable_workaround = 1;

    while ( *(unsigned short *)(p+12) == htons( PPA_TEST_AUTOMATION_VLAN) )
    {
        //remove vlan header via moving data ahead
        memmove(p+12, p + 12 + PPA_TEST_AUTOMATION_VLAN_TAG_LEN,  *len - 12 - 4);
        *len -= PPA_TEST_AUTOMATION_VLAN_TAG_LEN;

        f_vlan=1;
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "remove vlan header 4 bytes\n");
    }

#if defined(PPA_AUTO_PROXY_CPE )  
//I don't know why in UGW 5.2 or 2.6.32.40 the linux kernel will have bug:
//if linux receive a packet with vlan tag, but there is no such vlan interface, then raw socket will receive wrong packet, for example
//the actual data is         FF FF FF FF FF FF 00 0C 29 79 EE 84 81 00 00 14
//but the receive data is FF FF FF FF FF FF FF FF FF FF 00 0C 29 79 EE 84
//OF course if the vlan interface are there, then it is ok.
//Manamohan
#if 0
    if ( !f_vlan )
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "Workaround used since raw socket will capture wrong data ??\n");
        memmove(p, p + 4,  *len - 4);
        *len -= 4;
    }
#endif

    if ( enable_workaround )
    {
        int k=0;

        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "Recv Data after remove vlan/fixing: len=%d \n", *len);
        for(k=0; k<*len; k+=2 )
        {
            if( ( k % 16 ) == 0 )
                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "\n\t");

            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "%02x %02x ", p[k], p[k+1]);
        }

        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "\n");
    }
#endif
}

static int ppa_auto_recv_ack(PPA_TEST_AUTOMATION_PKT *orig_pkt, ppa_test_automation_if_attr *ifattr)
{

    PPA_TEST_AUTOMATION_PKT *p;
    unsigned char *packet;
    struct timeval currtime;
    struct pcap_pkthdr hdr;     /* pcap.h */
    unsigned int len;

    gettimeofday( &currtime, NULL);

    while (  !time_expire( &currtime, (long) PPA_TEST_AUTOMATION_MILLION))
    {
        //maybe there are some vlan tag inserted by swtich, so we need to remove vlan tag first
        if(  (packet = (unsigned char *)pcap_next(ppa_test_automation_if_listening.pcap_descr,&hdr) ) == NULL ) continue;
        if ( ppa_auto_capture_mode_only ) continue;
        
        if ( hdr.len <  sizeof(PPA_TEST_AUTOMATION_PKT_HDR) )
            continue;

        p = (PPA_TEST_AUTOMATION_PKT *)packet;
        len = hdr.len;

        remove_vlan((char *)p, &len );

        if ( p->hdr.type != htons(PPA_TEST_AUTOMATION_TYPE) )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND_DETAIL,"p->hdr.type not mach: %02x\n", ntohs(p->hdr.type ) );
            continue;
        }

        if ( len <  sizeof(PPA_TEST_AUTOMATION_PKT_HDR) )
        {
            printf("packet too short :%d\n", len );
            continue;
        }

        if ( p->hdr.magic != htons( PPA_TEST_AUTOMATION_MAGIC ) )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.magic not mach: %02x\n", ntohs(p->hdr.magic) );
            continue;
        }

        if ( p->hdr.cmd != PPA_TEST_AUTOMATION_CMD_ACK )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"Not ACK packet, drop it\n");
        }

        if ( ntohs(p->hdr.pkt_len) > PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"Wrong packet length: %d\n", ntohs(p->hdr.pkt_len) );
            continue;
        }
        if ( p->hdr.sender!= orig_pkt->hdr.receiver )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.sender not mach: %02x ( orig is %02x ) \n", ntohs(p->hdr.sender), ntohs(orig_pkt->hdr.receiver));
            continue;
        }
        if ( p->hdr.receiver!= orig_pkt->hdr.sender )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.receiver not mach: %02x ( orig is %02x ) \n", ntohs(p->hdr.receiver), ntohs(orig_pkt->hdr.sender));
            continue;
        }
        if( p->hdr.second_receiver != orig_pkt->hdr.second_receiver )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.second_receiver not mach: %02x ( orig is %02x ) \n", ntohs(p->hdr.second_receiver), ntohs(orig_pkt->hdr.second_receiver));
            continue;
        }
        if ( p->hdr.id != orig_pkt->hdr.id )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.id not mach: %02x ( orig is %02x ) \n", ntohs(p->hdr.id), ntohs(orig_pkt->hdr.id));
            continue;
        }
        if ( p->hdr.pid != orig_pkt->hdr.pid)
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.pid not mach: %02x( orig is %02x ) \n", ntohs(p->hdr.pid), ntohs(orig_pkt->hdr.pid));
            continue;
        }
        if ( p->hdr.pkts != orig_pkt->hdr.pkts )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.pkts not mach: %02x( orig is %02x ) \n", ntohs(p->hdr.pkts), ntohs(orig_pkt->hdr.pkts));
            continue;
        }
        if ( strcmp(p->hdr.session_name, ppa_test_automation_session_name) != 0 )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND,"p->hdr.session_name not mach: %s (orig is %s)\n", p->hdr.session_name, orig_pkt->hdr.session_name);
            continue;
        }

        update_peer_mac_address(p);
        return PPA_TEST_AUTOMATION_RESULT_OK;

    }

    return PPA_TEST_AUTOMATION_RESULT_RECV_ACK_FAIL;
}

int send_ack(PPA_TEST_AUTOMATION_PKT *orig_pkt, char *ifname)
{
    PPA_TEST_AUTOMATION_PKT_HDR *pkt = (PPA_TEST_AUTOMATION_PKT_HDR *) malloc( sizeof(PPA_TEST_AUTOMATION_PKT_HDR) ) ;
    if ( pkt == NULL ) return PPA_TEST_AUTOMATION_RESULT_NO_MEM;

    *pkt = orig_pkt->hdr; //copy all ppa automation protocol header, then update other necessary ones later
    memcpy( pkt->mac_dst, orig_pkt->hdr.mac_src, PPA_TEST_AUTOMATION_MAC_LEN);
    memcpy( pkt->mac_src, local_mac_addr, PPA_TEST_AUTOMATION_MAC_LEN );

    pkt->major_ver = PPA_TEST_AUTOMATION_MAJOR_VER; //use local version
    pkt->minor_ver = PPA_TEST_AUTOMATION_MINOR_VER;
    pkt->cmd = PPA_TEST_AUTOMATION_CMD_ACK; //ack packet
    pkt->pid = orig_pkt->hdr.pid;
    pkt->receiver = orig_pkt->hdr.sender;
    pkt->second_receiver = orig_pkt->hdr.second_receiver;
    pkt->sender = get_my_id();

    pcap_sendpacket (ppa_test_automation_if_listening.pcap_descr, (const u_char *)pkt, sizeof(PPA_TEST_AUTOMATION_PKT_HDR));

    free(pkt);

    return PPA_TEST_AUTOMATION_RESULT_OK;
}


int send_secure_pkt(PPA_TEST_AUTOMATION_PKT *pkt, ppa_test_automation_if_attr *ifattr, int max_tries, int need_check_ack)
{
    int i;
    int res = PPA_TEST_AUTOMATION_RESULT_SEND_FAIL;

    memcpy( pkt->hdr.mac_src, local_mac_addr, PPA_TEST_AUTOMATION_MAC_LEN );

    for (i=0; i<max_tries; i++ )
    {
        pkt->hdr.tries = i;
        if( pcap_sendpacket (ifattr->pcap_descr, (const u_char *)pkt, ntohs(pkt->hdr.pkt_len) + sizeof(PPA_TEST_AUTOMATION_PKT_HDR) )  != 0 )
        {
            //try again
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "sendto retry\n");
            usleep(1000);
            continue;
        }
        else
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "packet_send ok\n");
        }
        //wait confirmation
        if ( ppa_auto_protocol_ack_per_pkt && need_check_ack )
        {
            if ( ppa_auto_recv_ack(pkt, ifattr ) == PPA_TEST_AUTOMATION_RESULT_OK )
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "recv ack for pkt_index %d ok ****\n", ntohs(pkt->hdr.pkt_index ));
                return PPA_TEST_AUTOMATION_RESULT_OK;
            }
            else
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "recv ack fail for pkt_index %d ok\n", ntohs(pkt->hdr.pkt_index ));
            }

        }
        else
        {

            return PPA_TEST_AUTOMATION_RESULT_OK;
        }
    }



    return res;
}


/*segment buffer to single packets and sent out one by one */
static int send_file_via_if(unsigned char *filename, unsigned char cmd, char *cmd_param, char *mac_dst, ppa_test_automation_if_attr *ifattr, unsigned char receiver)
{
    int last_pkt_len, curr_pkt_len;
    int index = 0, i;
    int res;
    int pkt_num;
    FILE *fp;
    unsigned char *p;
    int  nr;

    if ( filename == NULL || strlen(filename) == 0  || ifattr == NULL || ifattr->ifname == NULL ) return PPA_TEST_AUTOMATION_RESULT_PARAM_WRONG;

    fp = fopen(filename, "rb");

    if ( fp == NULL )
    {
        printf("ReadDataFromFile: fail to open file %s\n", filename );
        return PPA_TEST_AUTOMATION_RESULT_NO_FILE;
    }

    fseek(fp,0,SEEK_END);
    nr = ftell(fp);
    if (  nr <=0  )
    {
        nr = 0;
        fclose(fp);

        printf("no data in file %s\n", filename );
        return PPA_TEST_AUTOMATION_RESULT_FILE_NO_DATA;
    }
    fseek(fp,0,SEEK_SET);

    PPA_TEST_AUTOMATION_PKT *pkt = (PPA_TEST_AUTOMATION_PKT *) malloc( sizeof(PPA_TEST_AUTOMATION_PKT) + 1) ;

    if ( pkt == NULL )
    {
        fclose(fp);
        return PPA_TEST_AUTOMATION_RESULT_NO_MEM;
    }

    pkt->hdr.type = htons(PPA_TEST_AUTOMATION_TYPE);
    pkt->hdr.magic = htons(PPA_TEST_AUTOMATION_MAGIC);
    pkt->hdr.major_ver = PPA_TEST_AUTOMATION_MAJOR_VER;
    pkt->hdr.minor_ver = PPA_TEST_AUTOMATION_MINOR_VER;
    pkt->hdr.cmd = cmd;
    pkt->hdr.id = ppa_test_automation_id++;  //same id for all packets as along as belong to same buffer
    pkt->hdr.pid = htonl( getpid()) ;
    pkt->hdr.sender = get_my_id();
    pkt->hdr.receiver = receiver;
    pkt->hdr.second_receiver = second_receiver;
    strncpy(pkt->hdr.session_name, ppa_test_automation_session_name, sizeof(pkt->hdr.session_name) );
    pkt->hdr.cmdbuf[0] = 0;
    if ( cmd_param != NULL )
    {
        strncpy(pkt->hdr.cmdbuf, cmd_param, sizeof(pkt->hdr.cmdbuf) -1 );
    }

    if ( nr % PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN )
    {
        pkt_num = (nr / PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN) + 1;
        last_pkt_len = nr % PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN;
    }
    else
    {
        pkt_num = nr / PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN;
        last_pkt_len = PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN;
    }

    DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "pkt_num=%d, buffer len=%d, per packet len=%d, last pkt len=%d\n",pkt_num, nr, PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN, last_pkt_len);
    pkt->hdr.pkts = htons(pkt_num);

    for (i=0; i<pkt_num; i ++ )
    {
        memcpy( pkt->hdr.mac_dst, peer_mac_addr, PPA_TEST_AUTOMATION_MAC_LEN );

        if ( i != pkt_num -1 )
        {
            curr_pkt_len = PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN;
        }
        else
        {
            curr_pkt_len = last_pkt_len;
        }

        pkt->hdr.pkt_index = htons(index ++);
        pkt->hdr.pkt_len = htons(curr_pkt_len);

        fread(pkt->buf, 1, curr_pkt_len, fp );

        //note, mac_src and tries will be filled in sub-function according to the ifname later
        DEBUG_PRINT(PPA_AUTO_DEBUG_SEND_DETAIL, "\n----send pkt index=%d, packet len=%d\n", i, curr_pkt_len);

        res = send_secure_pkt( pkt, ifattr, PPA_TEST_AUTOMATION_MAX_SEND_TRY, 1);

        if ( res != PPA_TEST_AUTOMATION_RESULT_OK )
        {
            free(pkt);
            fclose(fp);
            return res;
        }
    }

    //wait for confirmation

    free(pkt);
    fclose(fp);
    return res;
}


static int send_simple_cmd_via_if( unsigned char cmd, char *cmd_param, char *mac_dst, ppa_test_automation_if_attr *ifattr, unsigned char receiver)
{
    int last_pkt_len, curr_pkt_len;
    int index = 0, i;
    int res;
    int pkt_num;
    FILE *fp;
    unsigned char *p;
    int  nr;

    if ( ifattr == NULL || ifattr->ifname == NULL )
    {
        printf("send_simple_cmd_via_if: wrong ifattr null \n");
        return PPA_TEST_AUTOMATION_RESULT_PARAM_WRONG;
    }

    PPA_TEST_AUTOMATION_PKT *pkt = (PPA_TEST_AUTOMATION_PKT *) malloc( sizeof(PPA_TEST_AUTOMATION_PKT) + 1) ;

    if ( pkt == NULL )
    {
        printf("send_simple_cmd_via_if: malloc fail\n");
        return PPA_TEST_AUTOMATION_RESULT_NO_MEM;
    }

    memcpy( pkt->hdr.mac_dst, peer_mac_addr, PPA_TEST_AUTOMATION_MAC_LEN);

    pkt->hdr.type = htons(PPA_TEST_AUTOMATION_TYPE);
    pkt->hdr.magic = htons(PPA_TEST_AUTOMATION_MAGIC);
    pkt->hdr.major_ver = PPA_TEST_AUTOMATION_MAJOR_VER;
    pkt->hdr.minor_ver = PPA_TEST_AUTOMATION_MINOR_VER;
    pkt->hdr.cmd = cmd;
    pkt->hdr.id = ppa_test_automation_id++;  //same id for all packets as along as belong to same buffer
    pkt->hdr.pid = htonl( getpid()) ;
    pkt->hdr.sender = get_my_id();
    pkt->hdr.receiver = receiver;
    pkt->hdr.second_receiver = second_receiver;
    strncpy(pkt->hdr.session_name, ppa_test_automation_session_name, sizeof(pkt->hdr.session_name) );
    pkt->hdr.cmdbuf[0] = 0;
    if ( cmd_param != NULL )
    {
        strncpy(pkt->hdr.cmdbuf, cmd_param, sizeof(pkt->hdr.cmdbuf) -1 );
    }
    pkt_num = 1;
    pkt->hdr.pkts = htons(pkt_num);

    curr_pkt_len = 0;
    pkt->hdr.pkt_index = htons(index ++);
    pkt->hdr.pkt_len = htons(curr_pkt_len);


    //pkt->buf[curr_pkt_len] = 0;
    res = send_secure_pkt( pkt, ifattr, PPA_TEST_AUTOMATION_MAX_SEND_TRY, 1);

    if ( res != PPA_TEST_AUTOMATION_RESULT_OK )
    {
        free(pkt);
        return res;
    }


    free(pkt);
    return res;
}


int find_free_buffer(PPA_TEST_AUTOMATION_PKT *pkt)
{
    int i;
    struct timeval currtime;

    gettimeofday( &currtime, NULL);

    for (i=0; i<PPA_TEST_AUTOMATION_MAX_BUFFER_NUM; i++ )
    {
        if ( read_buffer[i] == NULL ) return i;
    }

    //if cannot find free one, then find one buffer which is expire already
    for (i=0; i<PPA_TEST_AUTOMATION_MAX_BUFFER_NUM; i++ )
    {

        if ( ppa_test_automation_get_time_diff( &read_buffer_last_time[i], &currtime ) >=  PPA_TEST_AUTOMATION_BUFFER_TIMEOUT )
        {
            free(read_buffer[i] );
            read_buffer[i] = NULL;
            set_defaul_buffer_status(i);
            return i;
        }
    }

    return -1;
}

int match_buffer( PPA_TEST_AUTOMATION_PKT *pkt)
{
    PPA_TEST_AUTOMATION_PKT *p;
    int index, i;

    for (i=0; i<PPA_TEST_AUTOMATION_MAX_BUFFER_NUM; i++ )
    {
        if ( read_buffer_status[i] != PPA_TEST_AUTOMATION_BUFFER_NOT_READY )
        {
            p = (PPA_TEST_AUTOMATION_PKT *) read_buffer[i];

            //some important value should same with last packets if in same buffer
            if ( p ->hdr.id == pkt->hdr.id &&
                    p->hdr.pid == pkt->hdr.pid  &&
                    p->hdr.sender == pkt->hdr.sender &&
                    p->hdr.receiver == pkt->hdr.receiver &&
                    p->hdr.second_receiver == pkt->hdr.second_receiver)   //for session name and other itmes, it is already check outside here
            {
                return i;
            }
        }
    }

    return  -1;
}


void get_tmp_file_name(int index, char *filename_buf )
{
    sprintf(filename_buf, ppa_test_automation_tmp_file_fomat, index);
}

int savetoFile(int index, PPA_TEST_AUTOMATION_PKT *pkt)
{
    FILE *fp ;
    char filename[PPA_TEST_AUTOMATION_MAX_FILENAME_LEN];

    get_tmp_file_name( index, filename );

    if( htons(pkt->hdr.pkt_index) ==  0 )
        fp = fopen(filename, "wb");
    else
        fp = fopen(filename, "a+");

    if( fp == NULL )
    {
        printf("file open fail:%s\n", filename);
        return -1;
    }

    fwrite( pkt->buf, 1, ntohs(pkt->hdr.pkt_len) ,fp);
    fclose(fp);
}

int create_buffer( PPA_TEST_AUTOMATION_PKT *pkt)
{
    int i, index=-1;

    index = find_free_buffer( pkt);
    if ( index < 0 ) return PPA_TEST_AUTOMATION_RESULT_NO_MEM;

    DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "Find a recv_buffer[%d], its first packet len is %d, index=%d\n", index, ntohs (pkt->hdr.pkt_len) );

    read_buffer[index] = malloc(  sizeof(PPA_TEST_AUTOMATION_PKT_HDR) );
    if ( read_buffer[index] == NULL )
    {
        read_buffer_status[index]=PPA_TEST_AUTOMATION_BUFFER_NOT_READY;
        printf("create_buffer malloc fail to get %d bytes\n", sizeof(PPA_TEST_AUTOMATION_PKT_HDR) );
        return -PPA_TEST_AUTOMATION_RESULT_NO_MEM;
    }

    //save pkt header
    *read_buffer[index] = pkt->hdr;

    //save data to tmp file
    savetoFile(index, pkt);

    gettimeofday( &read_buffer_last_time[index], NULL);

    if ( ntohs( pkt->hdr.pkt_index)  + 1 == ntohs(pkt->hdr.pkts) ) //one buffer only one packet.
        read_buffer_status[index]=PPA_TEST_AUTOMATION_BUFFER_OK;
    else
    {
        read_buffer_status[index]=PPA_TEST_AUTOMATION_BUFFER_RECVING;
    }
    return index;


}
#if defined(PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT) && PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT
#define PPA_TEST_AUTOMATION_MAX_PID_NUM 30
int ppa_test_automation_zombie_pid[PPA_TEST_AUTOMATION_MAX_PID_NUM]= {0};
int ppa_test_automation_push_pid(int pid)
{
    int i;

    DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "Enter ppa_test_automation_push_pid\n");

    for(i=0; i<PPA_TEST_AUTOMATION_MAX_PID_NUM; i++ )
    {
        if( ppa_test_automation_zombie_pid[i] == 0 )
        {
            ppa_test_automation_zombie_pid[i] = pid;
            return 0;
        }
    }

    DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "Why zobmie_pid[%d] used up\n", PPA_TEST_AUTOMATION_MAX_PID_NUM);
    return -1;
}

int check_ppa_test_automation_zombie_pid()
{
    int i, ret, status;

    for(i=0; i<PPA_TEST_AUTOMATION_MAX_PID_NUM; i++ )
    {
        if( ppa_test_automation_zombie_pid[i]  )
        {
            ret = waitpid(ppa_test_automation_zombie_pid[i], &status, WNOHANG | WUNTRACED);
            switch ( ret )
            {
            case -1:   // error occurs then return -1
                printf("why waitpid return -1 for process %d\n", ppa_test_automation_zombie_pid[i]);
                ppa_test_automation_zombie_pid[i] = 0; //we have to set to 0 since error ?????
                return -1;

            case 0:    // child does not exited yet
                break;

            default:   // child does exit successfully
                ppa_test_automation_zombie_pid[i] = 0;
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "one zombie process is reclamined sucessfully\n");
            }
        }
    }
    return 0;
}

#endif

int ppa_test_automation_folder_exist( char *filename)
{
    struct stat statBuf;

    if ( stat( filename, &statBuf ) < 0 )
        return 0;

    return S_ISDIR( statBuf.st_mode );

}


int run_linux_cmd(char *script_cmd_file, char *start_in_folder, unsigned char run_fore_ground)
{
    int pid = 0, status = 0, counter = 0, ret = 0;
    extern char **environ;

    if ( script_cmd_file == 0 )      return 1;

#if 0
    if ( !run_fore_ground )
    {
        //to solve zombie process
        signal( SIGCHLD, SIG_IGN );
    }
#endif
    pid = fork();
    if ( pid == -1 )
        return -1;

    if ( pid == 0 )
    {
        /*child prorcess */
        char *argv[4];

        argv[0] = "sh";
#if 0        /*in brcmd, why need paramater -c, but in fedora 11, no support parameter */
        argv[1] = "-c";
        argv[2] = script_cmd_file;
#else
        argv[1] = script_cmd_file;
        argv[2] = 0;
#endif
        argv[3] = 0;

        if ( start_in_folder   )
        {
            if ( ppa_test_automation_folder_exist(start_in_folder) )
                chdir(start_in_folder);
        }

        execve("/bin/sh", argv, environ);
        exit(127);
    }

    if ( run_fore_ground )
    {
#define WAIT_LOOP_TIMES  300000
        do
        {
            // check the child is exited or not without hang
            ret = waitpid(pid, &status, WNOHANG | WUNTRACED);
            switch ( ret )
            {
            case -1:   // error occurs then return -1
                return -1;
            case 0:    // child does not exited yet
                usleep(20);
                if ( ++counter > WAIT_LOOP_TIMES )
                {
                    printf("app: child process cannot exits while executing script_cmd_file - %s\n", script_cmd_file);
                    kill(pid, SIGTERM);
                    return -1;
                }
                break;

            default:   // child does exit successfully
                return status;
            }
        }
        while ( 1 );
    }
    else
    {
#if defined(PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT) && PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT
        ppa_test_automation_push_pid(pid);
#endif
    }

}

int ppa_test_automation_put_to_buffer( PPA_TEST_AUTOMATION_PKT *pkt, int index)
{
    PPA_TEST_AUTOMATION_PKT *pkt_last;

    if ( read_buffer_status[index] == PPA_TEST_AUTOMATION_BUFFER_RECVING )
    {
        pkt_last =( PPA_TEST_AUTOMATION_PKT *)read_buffer[index];
        if ( pkt_last->hdr.pkt_index == pkt->hdr.pkt_index )
        {
            gettimeofday( &read_buffer_last_time[index], NULL);
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "repeating pkt_index:%d\n", ntohs(pkt->hdr.pkt_index) );
            return PPA_TEST_AUTOMATION_BUFFER_OK; //peer side are retrying
        }

        if ( ntohs(pkt_last->hdr.pkt_index) +1 != ntohs( pkt->hdr.pkt_index ) )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "wrong  pkt_index %d, but last index is %d\n", ntohs(pkt->hdr.pkt_index), ntohs(pkt_last->hdr.pkt_index) );
            return PPA_TEST_AUTOMATION_BUFFER_OK; //seems some packet drop, anyway, we just drop it
        }

        savetoFile(index, pkt );
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "put to buffer at offset:%d\n", ntohs (pkt->hdr.pkt_len) );

        pkt_last->hdr.pkt_index = pkt->hdr.pkt_index ; //update index
        gettimeofday( &read_buffer_last_time[index], NULL);

        if ( ntohs( pkt->hdr.pkt_index)  + 1 == ntohs(pkt->hdr.pkts) ) //one buffer only one packet.
        {
            read_buffer_status[index]=PPA_TEST_AUTOMATION_BUFFER_OK;
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "A buffer receving finished\n");
        }
        else
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "pkts %d left to assembly\n", ntohs(pkt->hdr.pkts) - ntohs( pkt->hdr.pkt_index)  -1);
        }
    }
    else
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "Drop the packet for wrong read_buffer_status=%d\n", read_buffer_status[index] );
    }


    return PPA_TEST_AUTOMATION_RESULT_OK;
}


int ppa_test_automation_assemble_packet( PPA_TEST_AUTOMATION_PKT *pkt)
{
    int index;
    struct timeval now;


    index = match_buffer( pkt);

    if ( index >= 0 &&  ntohs( pkt->hdr.pkt_index) == 0 )
    {
        gettimeofday( &now, NULL);
        if ( ppa_test_automation_get_time_diff( &read_buffer_last_time[index], &now ) >= PPA_TEST_AUTOMATION_BUFFER_TIMEOUT )
        {
            /*this buffer is too old and should be expire already */
            free( read_buffer[index] );
            set_defaul_buffer_status(index);

            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "Froce to free a receive buffer[%d] for expire\n", index);
            index =-1;
        }
    }

    if ( index < 0 )
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "No existing read_buffer for  pkt->hdr.id %x and pkt->hdr.pid %x\n", pkt->hdr.id, pkt->hdr.pid );
        if ( ntohs( pkt->hdr.pkt_index) == 0 )
        {
            index = create_buffer( pkt);
            if ( index >= 0 )
                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "created a read_buffer[%d]\n", index );
            else
                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "failed to create a read_buffer\n");

        }
        else
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "Drop this frame for not correct pkt_index:%d\n", ntohs( pkt->hdr.pkt_index) == 0);
        }

    }
    else
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "Find an existing buffer[ %d]\n", index );
        ppa_test_automation_put_to_buffer(pkt, index);
    }

    return index;
}

char *ppa_test_automation_ppa_id_name[] = {"N/A", "manager", "CPE", "WAN" };

int time_expire(struct timeval* start, long duration)
{
    struct timeval currtime;

    if ( duration == 0 ) return 0; //duration 0 means no timeout

    gettimeofday( &currtime, NULL);

    if ( ppa_test_automation_get_time_diff( start, &currtime ) >= duration )
    {
        printf("Time expire:%ld >= %ld\n", ppa_test_automation_get_time_diff( start, &currtime ), duration);
        return 1;
    }
    else return 0;
}

/* return value:
*/
static int recv_buffer_via_if(ppa_test_automation_if_attr *ifattr, long max_ms, int wait_cmd)
{
    PPA_TEST_AUTOMATION_PKT *pkt;
    unsigned char *packet, *p;
    unsigned int len;
    int index;
    struct timeval currtime;
    struct pcap_pkthdr hdr;     /* pcap.h */

    gettimeofday( &currtime, NULL);

    while ( !time_expire( &currtime, max_ms ) )
    {
#if defined(PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT) && PPA_TEST_AUTOMATION_ZOMBIE_SUPPORT
        check_ppa_test_automation_zombie_pid();
#endif
        if(  (packet = (unsigned char *)pcap_next(ppa_test_automation_if_listening.pcap_descr,&hdr) ) == NULL ) continue;

        if ( hdr.len <  sizeof(PPA_TEST_AUTOMATION_PKT_HDR) )
            continue;

        p = packet;
        len = hdr.len;

        {
            int k=0;

            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "Recv Data: len=%d hdr.caplen=%d, hdr.len=%d\n", len, hdr.caplen,hdr.len );
            for(k=0; k<hdr.len; k+=2 )
            {
                if( ( k % 16 ) == 0 )
                    DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "\n\t");

                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "%02x %02x ", packet[k], packet[k+1]);
            }

            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL, "\n");
        }

        remove_vlan( (char *)p, &len );

        pkt = (PPA_TEST_AUTOMATION_PKT * )p;

        //check ppa automation type and magic
        if ( pkt->hdr.type != htons(PPA_TEST_AUTOMATION_TYPE) )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV_DETAIL,"pkt->hdr.type not mach: %02x\n", ntohs(pkt->hdr.type ) );
            continue;
        }

        if ( len <  sizeof(PPA_TEST_AUTOMATION_PKT_HDR) )
        {
            printf("packet too short :%d\n", len );
            continue;
        }


        if ( pkt->hdr.magic != htons( PPA_TEST_AUTOMATION_MAGIC ) )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"pkt->hdr.magic not mach: %02x\n", ntohs(pkt->hdr.magic) );
            continue;
        }

        if ( ntohs(pkt->hdr.pkt_len) > PPA_TEST_AUTOMATION_MAX_PKT_DATA_LEN )
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"Wrong packet length: %d\n", ntohs(pkt->hdr.pkt_len) );
            continue;
        }

        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "----recv ppa auto protocol---- to %02x%02x%02x%02x%02x%02x from %02x%02x%02x%02x%02x%02x -- packet len=%d, header=%d, data=%d\n",
                    len, sizeof(PPA_TEST_AUTOMATION_PKT_HDR),  ntohs(pkt->hdr.pkt_len),
                    pkt->hdr.mac_dst[0], pkt->hdr.mac_dst[1], pkt->hdr.mac_dst[2], pkt->hdr.mac_dst[3], pkt->hdr.mac_dst[4], pkt->hdr.mac_dst[5], pkt->hdr.mac_dst[6],
                    pkt->hdr.mac_src[0], pkt->hdr.mac_src[1], pkt->hdr.mac_src[2], pkt->hdr.mac_src[3], pkt->hdr.mac_src[4], pkt->hdr.mac_src[5], pkt->hdr.mac_src[6] );

        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "\ttype=%02x magic=%02x version=%d.%d\n", ntohs( pkt->hdr.type), ntohs( pkt->hdr.magic), pkt->hdr.major_ver, pkt->hdr.minor_ver );
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "\tsession=%s send=%s, receive=%s, pid=%x, id=%x\n", pkt->hdr.session_name, ppa_test_automation_ppa_id_name[pkt->hdr.sender], ppa_test_automation_ppa_id_name[pkt->hdr.receiver], ntohl(pkt->hdr.pid) , ntohl(pkt->hdr.id) );
        DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "\tcmd=%x: index=%d pkts=%d, pkt_len=%d \n", pkt->hdr.cmd, ntohs(pkt->hdr.pkt_index), ntohs(pkt->hdr.pkts), ntohs(pkt->hdr.pkt_len) );


        if ( strncmp(pkt->hdr.session_name, ppa_test_automation_session_name, sizeof(pkt->hdr.session_name) ) != 0 ) //not in the same sessin. So discard it
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"Recv a different session: %s_%s\n", pkt->hdr.session_name, ppa_test_automation_session_name );
            continue;
        }
        //no need to respond and no action if it is a ack packet since we receive it at wrong time
        if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_ACK ) continue;

        if ( !correct_sender_receiver(pkt) ) //from correct sender
        {
            DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"Not from corect sender=%d, receive=%d\n", pkt->hdr.sender, pkt->hdr.receiver);
            continue;
        }

        if ( wait_cmd != 0 ) //only receive one specific cmd, mainly for PPA_TEST_AUTOMATION_CMD_GET_FILE_REPLY and so on at present
        {
            if ( pkt->hdr.cmd != wait_cmd )
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV,"Not the cmd we want: %d_%d\n", pkt->hdr.cmd, wait_cmd);
                continue;
            }
        }

        update_peer_mac_address(pkt); //once have learned the mac address, ppa automation protocol will use unicast mac address


        //it is a valid ppa automation protocol packet.
        if ( send_ack(pkt,ifattr->ifname) != PPA_TEST_AUTOMATION_RESULT_OK ) continue; //drop since we cannot reply the ack

        index = ppa_test_automation_assemble_packet( pkt );

        if ( index >= 0 )
        {
            if ( read_buffer_status[index] == PPA_TEST_AUTOMATION_BUFFER_OK )
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "recv_buffer_via_if exit \n");
                return index;
            }
        }
    }

    return -1;
}

/*get the time difference in million seconds.
   use below to get time:
   struct timeval t;
   gettimeofday(&t, NULL);
*/
long ppa_test_automation_get_time_diff( struct timeval *start, struct timeval *end )
{
    return PPA_TEST_AUTOMATION_MILLION * ( end->tv_sec - start->tv_sec ) + (end->tv_usec - start->tv_usec );
}

static struct option long_options[] =
{
    /* These options set a flag. */
    {"ack-per-pkt",     no_argument,                   0, 'a'},
    {"debug",              required_argument,           0, 'g'},
    {"interface",          required_argument,           0, 'i'},
    {"session-name",  required_argument,           0, 'n'},
    {"vlan-id",            required_argument,           0, 'v'},
    {0, 0, 0, 0}
};

#if defined(PPA_AUTO_PROXY_CPE    )  | defined(PPA_AUTO_PROXY_CPE )
char *short_options="ag:i:n:p:v:h"; //note, if no need to follow argument, then don't add common ( : )
#else
char *short_options="ag:i:n:p:v:c:s:d:t:h"; //note, if no need to follow argument, then don't add common ( : )
#endif

#ifdef USE_RAW_SOCKET
/*source -- the interface to open
   snaplen -- not used now.
   promisc -- not use now.
   to_ms -- not use now.
   errbuf --not use now.
*/
pcap_t *pcap_open_live(const char *source, int snaplen, int promisc, int to_ms, char *errbuf)
{
    struct sockaddr_ll addr = { 0,0,0,0,0,0,{0,} };
    int MACSIZE = 6;
    int protocol=ETH_P_ALL;
    pcap_t netfd;

    printf("pcap_open_live...\n");
    netfd = socket(PF_PACKET, SOCK_RAW, htons(protocol));
    if (netfd == -1)
    {
        printf("Socket opening error!");
        if(errbuf) strcpy_s(errbuf, 25, "Socket opening error!\n"); 
        return NULL;
    }

    printf("socket ok\n");
    /* First Get the Interface Index  */
    memset( &ppa_test_automation_ifr, 0, sizeof(ppa_test_automation_ifr) );
    strncpy(ppa_test_automation_ifr.ifr_name, source, sizeof(ppa_test_automation_ifr.ifr_name));
    if((ioctl(netfd, SIOCGIFINDEX, &ppa_test_automation_ifr)) == -1)
    {
        printf("Error getting Interface index for interface %s!\n", ppa_test_automation_ifr.ifr_name);
        if(errbuf) strcpy_s(errbuf, 50, "Error getting Interface index for interface!\n"); 
        return NULL;
    }

    //bind to the interface
    memset( &addr, 0, sizeof(addr) );
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(protocol);
    addr.sll_ifindex = ppa_test_automation_ifr.ifr_ifindex;

    if (bind(netfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        printf("Bind Error\n");
        if(errbuf) strcpy_s(errbuf, 15, "bind error!\n"); 
        close(netfd);
        return NULL;
    }
    printf("bind...\n");
    return (pcap_t *)netfd;
}

const u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h)
{
    int c;

    static buf[sizeof(PPA_TEST_AUTOMATION_PKT) + 20];
    static PPA_TEST_AUTOMATION_PKT *pkt = (PPA_TEST_AUTOMATION_PKT *)buf;
    struct sockaddr_ll info;           // struttura utilizzata in recvfrom
    int info_size = sizeof(info);
    fd_set read_fd;
    struct timeval tv;
    int retval;

    tv.tv_usec = 500000;
    tv.tv_sec = 0;

    FD_ZERO(&read_fd);
    FD_SET( (int) p, &read_fd); /* add to master set */

    if( (retval=select( (int)p + 1, &read_fd, NULL, NULL, &tv) ) <= 0)
    {
        if( h)
        {
            h->len = 0;
            h->caplen = 0;
        }
        FD_CLR( (int) p, &read_fd); 
        FD_ZERO(&read_fd);

        return NULL;
    }

    if ( ! FD_ISSET( (int)p, &read_fd))
    {
        if( h)
        {
            h->len = 0;
            h->caplen = 0;
        }
        FD_CLR( (int) p, &read_fd); 
        FD_ZERO(&read_fd);

        return NULL;
    }

    c = recvfrom((int)p, buf, sizeof(buf), 0, (struct sockaddr*)&info, &info_size);

    if( c < 0 )
    {
        if( errno == EBADF || errno == ENOTSOCK )
        {
            printf("Why here \n");
        }
        if( h)
        {
            h->len = 0;
            h->caplen = 0;
        }
        FD_CLR( (int) p, &read_fd); 
        FD_ZERO(&read_fd);

        return NULL;
    }
    else
    {
        if( h )
        {
            h->len = c;
            h->caplen = c;
        }
        FD_CLR( (int) p, &read_fd); 
        FD_ZERO(&read_fd);

        return (const u_char *)buf;
    }

}
/*return value: 0 --succeed to send packet
                        otherwise return -1;
*/
int pcap_sendpacket(pcap_t *p, const u_char *buf, int size)
{
    int res;
    //note, since we have call ioctl to bind the interface to one specific socket. in fact there is no need to provide sa again.
    struct sockaddr_ll sa;

    memset(&sa, 0, sizeof (sa));
    sa.sll_family    = AF_PACKET;
    sa.sll_ifindex   = ppa_test_automation_ifr.ifr_ifindex;
    sa.sll_protocol  = htons(ETH_P_ALL);
    sa.sll_halen = 6;
    memcpy(sa.sll_addr, buf+6, 6);

    if( ppa_test_automation_vid <0 )
    {
        res = sendto((int)p, (char *)buf, size, 0, (struct sockaddr*) &sa, sizeof(sa) ); // (struct sockaddr *)&sa, sizeof (sa));
    }
    else
    {
        int i;
        char tmp[sizeof(PPA_TEST_AUTOMATION_PKT) + 4];

        for(i=0; i<12; i++) memcpy( tmp, buf, 12); //copy mac
        //fill vid
        tmp[12] = 0x81;
        tmp[13] = 0x0;
        tmp[14] = 0x0;
        tmp[15] = ppa_test_automation_vid;
        //copy other data
        memcpy( tmp+16, buf+12, size -12);
        res = sendto((int)p, (char *)tmp, size+4, 0, (struct sockaddr*) &sa, sizeof(sa) ); // (struct sockaddr *)&sa, sizeof (sa));
    }

    DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "pcap_sendpacket: %s\n", (res > 0 )?"ok":"fail");
    if ( res< 0 )
    {
        DEBUG_PRINT(PPA_AUTO_DEBUG_SEND, "pcap_sendpacket errno: %d\n", errno);
        return errno;
    }


    return 0;

}
void pcap_close(pcap_t *p)
{
    close((int)p);
}

/*dummy function only */
int pcap_lookupnet (const char *device, bpf_u_int32 *localnet,
                    bpf_u_int32 *netmask, char *errbuf)
{
    return 0;
}

/*dummy here only */
int pcap_compile(pcap_t *p, struct bpf_program *program, const char *buf, int optimize, bpf_u_int32 mask)
{
    return 0;
}
/*Note, here use hardcoded filter. so
struct bpf_program not used at all
*/
int pcap_setfilter(pcap_t *p, struct bpf_program *fp)
{
    //sudo tcpdump -i eth0 '(ether[12]=0xfe and ether[13]=0xdc) || (ether[12]=0x81 and ether[16]=0xfe and ether[17]=0xdc ) || (ether[12]=0x81 and ether[16]=0X81 and ether[20]=0xfe and ether[21]=0xdc )' -s 1514 -dd
    struct sock_filter bpfcode[]=
    {
        { 0x30, 0, 0, 0x0000000c },
        { 0x15, 0, 2, 0x000000fe },
        { 0x30, 0, 0, 0x0000000d },
        { 0x15, 10, 11, 0x000000dc },
        { 0x15, 0, 10, 0x00000081 },
        { 0x30, 0, 0, 0x00000010 },
        { 0x15, 0, 2, 0x000000fe },
        { 0x30, 0, 0, 0x00000011 },
        { 0x15, 5, 6, 0x000000dc },
        { 0x15, 0, 5, 0x00000081 },
        { 0x30, 0, 0, 0x00000014 },
        { 0x15, 0, 3, 0x000000fe },
        { 0x30, 0, 0, 0x00000015 },
        { 0x15, 0, 1, 0x000000dc },
        { 0x6, 0, 0, 0x000005ea },
        { 0x6, 0, 0, 0x00000000 } //There is no comma at the end of this line
    };

   
    struct sockaddr_ll sa;
    struct sock_fprog filter;

    if( ppa_auto_capture_mode_only ) return 0; //not filter.
    
    filter.len = sizeof(bpfcode)/sizeof(bpfcode[0]);
    filter.filter = bpfcode;

    
    if (setsockopt((int)p, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) == -1)
    {
        printf( "setsockopt call failed for SO_ATTACH_FILTER setting\n");
        return -1;
    }

    return 0;
}


#endif



//In order to save some memory for ppa_auto_proxy will be downloaded into cpe, so we define a macro to compile different
int ppa_test_automation_main()
{
    int index, i;
    int res=0;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 net;               /* Our IP address            */
    bpf_u_int32 mask;              /* Our netmask               */
    struct bpf_program filter;     /* The compiled filter       */
    char filterbuf[100];

    static int loop_cnt =0;
    static int loop_cnt_cmd =0;

    if ( getuid() && geteuid() )
    {
        perror("Sorry  need the su rights for raw socket!\n");
        return -2;
    }

    printf("ppa_test_automation_if_listening.ifname=%s\n", ppa_test_automation_if_listening.ifname );
    //Note, cannot just use sizeof(PPA_TEST_AUTOMATION_PKT) + PPA_TEST_AUTOMATION_MAX_VLAN_HEADER_ID + 4. The reason is that linux kernel will add some offset before L2 Mac header, esp in Infineon solution
    ppa_test_automation_if_listening.pcap_descr =  pcap_open_live(ppa_test_automation_if_listening.ifname,/*sizeof(PPA_TEST_AUTOMATION_PKT) + PPA_TEST_AUTOMATION_MAX_VLAN_HEADER_ID + 4*/1514,0,10,errbuf);

    if(ppa_test_automation_if_listening.pcap_descr == NULL)
    {
        printf("pcap_open_live(): %s\n",errbuf);
        return -2;
    }
    printf("pcap_open_live ok\n");
    DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "pcap_open_live ok\n");

    pcap_lookupnet(ppa_test_automation_if_listening.ifname, &net, &mask, errbuf);

    sprintf(filterbuf, "ether proto 0x%x", /*htons*/(PPA_TEST_AUTOMATION_TYPE) ); //why no need to add htons
    DEBUG_PRINT(PPA_AUTO_DEBUG_RECV, "pcap filter:%s\n", filterbuf );

    if (pcap_compile(ppa_test_automation_if_listening.pcap_descr, &filter, filterbuf, 0, /*net*/ 0) == -1)
    {
        fprintf(stderr,"Error compiling pcap\n");
        return  -2;
    }
    if (pcap_setfilter(ppa_test_automation_if_listening.pcap_descr, &filter))
    {
        fprintf(stderr, "Error setting pcap filter\n");
        return  -2;
    }
    DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "pcap_setfilter ok\n");
    if ( ppa_test_automation_get_if_mac(ppa_test_automation_if_listening.ifname, local_mac_addr )  )
    {
        printf("Cannot get mac address from interface %s\n", ppa_test_automation_if_listening.ifname );
        pcap_close(ppa_test_automation_if_listening.pcap_descr);
        return -2;
    }
    DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "ppa_test_automation_get_if_mac ok\n");

    printf("Listening on port %s with session %s\n", ppa_test_automation_if_listening.ifname, ppa_test_automation_session_name);

    while (1)
    {
        PPA_TEST_AUTOMATION_PKT *pkt;
        if ( (index=recv_buffer_via_if( &ppa_test_automation_if_listening, 0, 0) ) >= 0 )
        {
            pkt = (PPA_TEST_AUTOMATION_PKT *) read_buffer[index];

            if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_GET_FILE )
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "recv command get file %s\n", pkt->hdr.cmdbuf );
                send_file_via_if(pkt->hdr.cmdbuf,PPA_TEST_AUTOMATION_CMD_GET_FILE_REPLY, NULL, NULL, &ppa_test_automation_if_listening, PPA_TEST_AUTOMATION_AUTO_ID_MANAGER );
            }
            else if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_PUT_FILE)
            {
                char buf[ 2 * PPA_TEST_AUTOMATION_MAX_FILENAME_LEN];
                char tmp_filename[PPA_TEST_AUTOMATION_MAX_FILENAME_LEN];

                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "recv command put to filename %s\n", pkt->hdr.cmdbuf );

                get_tmp_file_name( index, tmp_filename );
                //move tmp to its destination
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "move file from %s to %s\n", tmp_filename,  pkt->hdr.cmdbuf);
                rename( tmp_filename, pkt->hdr.cmdbuf );

                sprintf(buf, "chmod +rwx %s", pkt->hdr.cmdbuf );
                system(buf);

            }
            else if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_DEL_FILE)
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "recv command delete file %s\n", pkt->hdr.cmdbuf );
                remove(pkt->hdr.cmdbuf );
            }
            else if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_RUN_FILE)
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "recv command run file %s\n", pkt->hdr.cmdbuf );
 		//printf("\nLoop count =%d\n",loop_cnt);
       		if( loop_cnt == 0)
        	{
			loop_cnt = 1;
                	run_linux_cmd( pkt->hdr.cmdbuf, NULL, 0 );
        	}
                else
        		loop_cnt = 0; 


                //run_linux_cmd( pkt->hdr.cmdbuf, NULL, 0 );
            }
            else if ( pkt->hdr.cmd == PPA_TEST_AUTOMATION_CMD_RUN_CMD)
            {
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "recv command run cmd: %s\n", pkt->hdr.cmdbuf );
                DEBUG_PRINT(PPA_AUTO_DEBUG_OTHER, "\t It's length is %d\n", strlen(pkt->hdr.cmdbuf) );
                if( loop_cnt_cmd == 0)
        	{
			loop_cnt_cmd = 1;
			system( pkt->hdr.cmdbuf);
        	}
                else
        		loop_cnt_cmd = 0; 
		//system( pkt->hdr.cmdbuf);
            }
            else
            {
                printf("Unknow command:%d\n", pkt->hdr.cmd );
            }

            free( read_buffer[index] );
            set_defaul_buffer_status(index);

        }

    }

    pcap_close(ppa_test_automation_if_listening.pcap_descr);
    return res;
}



void ppa_test_automation_help( int summary)
{
    if( summary )
    {
        IFX_PPACMD_PRINT("automation [-i interface] [-n session_name] [-g debug-type] [-v vlan-id] [-r 2nd-receiver] [-a (ack per packet)]\n");
        IFX_PPACMD_PRINT("automation [-i interface] [-c capture only \n");
        IFX_PPACMD_PRINT("Notes:\n");
        IFX_PPACMD_PRINT("\t-i: listening interface \n");
        IFX_PPACMD_PRINT("\t-n: ppa test automation session name, like 0000, 0001, .... \n");
        IFX_PPACMD_PRINT("\t-r: The 2nd receive id is 0 ~ 255 only\n");
        IFX_PPACMD_PRINT("\tdebug-type: all/rx/rx_detail/tx/tx_detail/cmd/option/other\n");
        IFX_PPACMD_PRINT("\tc: capture mode only, ie, not for PPA test automation but for tcpdump\n");
    }
    return;
}

const char ppa_test_automation_short_opts[] = "i:g:v:n:ar:c";
const struct option ppa_test_automation_long_opts[] =
{
    {"interface",   required_argument,  NULL, 'i'},
    {"debug",   required_argument,  NULL, 'g'},
    {"vlan",   required_argument,  NULL, 'v'},
    {"session",   required_argument,  NULL, 'n'},
    {"ack",   no_argument,  NULL, 'a'},
    {"capture-only",   no_argument,  NULL, 'c'},
    {"second receiver id",   required_argument,  NULL, 'r'},
    { 0,0,0,0 }
};

int ppa_parse_test_automation_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    memset( &ppa_test_automation_if_listening, 0, sizeof(ppa_test_automation_if_listening));
    printf("ppa_parse_test_automation_cmd...\n");

    while(popts->opt)
    {
        printf("popts->opt=%s\n", popts->optarg );

        switch(popts->opt)
        {
        case 'a':
            ppa_auto_protocol_ack_per_pkt ^= 1;
            break;
            
        case 'c':
            ppa_auto_capture_mode_only=1;
            enable_ppa_automation_debug =( unsigned int)  -1;
            break;

        case 'g':
            if ( strcmp(popts->optarg, "all" ) == 0 )
                enable_ppa_automation_debug =( unsigned int)  -1;
            else if ( strcmp(popts->optarg, "rx" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_RECV;
            else if ( strcmp(popts->optarg, "rx_detail" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_RECV_DETAIL;
            else if ( strcmp(popts->optarg, "tx" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_SEND;
            else if ( strcmp(popts->optarg, "tx_detail" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_SEND_DETAIL;
            else if ( strcmp(popts->optarg, "cmd" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_CMD;
            else if ( strcmp(popts->optarg, "param" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_PARAM;
            else if ( strcmp(popts->optarg, "other" ) == 0 )
                enable_ppa_automation_debug |= PPA_AUTO_DEBUG_OTHER;
            break;

        case 'v':
            ppa_test_automation_vid=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;

        case 'n':
            strncpy( ppa_test_automation_session_name, popts->optarg, sizeof(ppa_test_automation_session_name)-1 );
            break;

        case  'i':
            strncpy( ppa_test_automation_if_listening.ifname, popts->optarg, sizeof(ppa_test_automation_if_listening.ifname) -1);
            break;

        case  'r':
            second_receiver=atoi(popts->optarg);
            printf("second_receiver=%u\n", second_receiver );
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }


    printf("ppa_parse_test_automation_cmd ok\n");
    return PPA_CMD_OK;
}

int ppa_test_automation_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    ppa_test_automation_main();
    return PPA_CMD_OK;
}

/******************************************************************************************************
Note, below is the code for PPA test automation only., not for PPA ioctl at all  --- end
*****************************************************************************************************/
#endif //PPA_TEST_AUTOMATION_ENABLE


