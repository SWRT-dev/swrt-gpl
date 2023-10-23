/******************************************************************************
**
** FILE NAME  : ppacmd.c
** PROJECT    : PPA Configuration Utility
** MODULES    : Packet Acceleration
**
** DATE     : 10 JUN 2008
** AUTHOR     : Mark Smith
** DESCRIPTION  : PPA (Routing Acceleration) User Configuration Utility
** COPYRIGHT  : Copyright (c) 2020, MaxLinear, Inc.
**              Copyright (c) 2009, Lantiq Deutschland GmbH
**              Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
** 10 DEC 2012  Manamohan Shetty       Added the support for RTP,MIB mode  
**                                     Features 
**
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <uapi/asm-generic/ioctl.h>
#include <errno.h>
#include <ctype.h>

#include "ppacmd.h"
#include "ppacmd_autotest.h"

int enable_debug = 0;
char *debug_enable_file="/var/ppa_gdb_enable";

static int g_output_to_file = 0;
static int g_all = 0;
static char g_output_filename[PPACMD_MAX_FILENAME]= {0};
#define DEFAULT_OUTPUT_FILENAME   "/var/tmp.dat"

#define MAX_STR_IP	64
#define MAX_MEM_OFFSET	32

static void ppa_print_help(void);
static int ppa_parse_simple_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata);
static void ppa_print_get_qstatus_cmd(PPA_CMD_DATA *pdata);
static int ppa_get_qstatus_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata);

/*
int SaveDataToFile(char *FileName, char *data, int len )
return 0: means sucessful
return -1: fail to save data to file
*/
int SaveDataToFile(char *FileName, char *data, int len )
{
    FILE *pFile;

    if( data == NULL ) return  -1;
    
    if( FileName == NULL || strnlen_s(FileName,PPACMD_MAX_FILENAME) == 0 )
        FileName = DEFAULT_OUTPUT_FILENAME;

    if( strcmp(FileName, "null") == 0 ) return 0; //don't save, but remove console print also no file saving
    
	//IFX_PPACMD_DBG("SaveDataToFile  %s ( bytes: %d from buffer %p)\n", FileName, len, data );

    pFile = fopen(FileName, "wb");

    if( pFile == NULL )
    {
        IFX_PPACMD_PRINT("SaveDataToFile: fail to open file %s\n", FileName );
        return -1;
    }

    fwrite (data , 1 , len , pFile );
    fflush(pFile);
    fclose (pFile);

    return 0;
}

int32_t isValidMacAddress(const uint8_t* mac) {
    uint32_t i = 0;
    uint32_t s = 0;

    while (*mac) {
       if (isxdigit(*mac)) {
          i++;
       }
       else if (*mac == ':' || *mac == '-') {

          if (i == 0 || i / 2 - 1 != s)
            break;

          ++s;
       }
       else {
           s = -1;
       }

       ++mac;
    }

    return (i == 12 && (s == 5 || s == 0));
}
static void print_session_flags( uint32_t flags)
{
    static const char *str_flag[] =
    {
        "IS_REPLY",                 //  0x00000001
        "Reserved",
        "SESSION_IS_TCP",
        "STAMPING",
        "ADDED_IN_HW",              //  0x00000010
        "NOT_ACCEL_FOR_MGM",
        "STATIC",
        "DROP",
        "VALID_NAT_IP",             //  0x00000100
        "VALID_NAT_PORT",
        "VALID_NAT_SNAT",
        "NOT_ACCELABLE",
        "VALID_VLAN_INS",           //  0x00001000
        "VALID_VLAN_RM",
        "SESSION_VALID_OUT_VLAN_INS",
        "SESSION_VALID_OUT_VLAN_RM",
        "VALID_PPPOE",              //  0x00010000
        "VALID_NEW_SRC_MAC",
        "VALID_MTU",
        "VALID_NEW_DSCP",
        "SESSION_VALID_DSLWAN_QID", //  0x00100000
        "SESSION_TX_ITF_IPOA",
        "SESSION_TX_ITF_PPPOA",
        "Reserved",
        "SRC_MAC_DROP_EN",          //  0x01000000
        "SESSION_TUNNEL_6RD",
        "SESSION_TUNNEL_DSLITE",
        "Reserved",
        "LAN_ENTRY",                //  0x10000000
        "WAN_ENTRY",
        "IPV6",
        "Reserved",
    };

    int flag;
    unsigned long bit;
    int i;


    flag = 0;
    for ( bit = 1, i = 0; bit; bit <<= 1, i++ )
    {
        if ( (flags & bit) )
        {
            if ( flag++ )
                IFX_PPACMD_PRINT( "|");
            IFX_PPACMD_PRINT( "%s",str_flag[i]);
            //IFX_PPACMD_PRINT( " ");
        }
    }

}
int stricmp(const char * p1, const char * p2)
{
    int c1, c2;

    while ( *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
    }

    return *p1 - *p2;
}

int strincmp(const char *p1, const char *p2, int n)
{
    int c1 = 0, c2;

    while ( n && *p1 && *p2 )
    {
        c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
        c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
        if ( (c1 -= c2) )
            return c1;
        p1++;
        p2++;
        n--;
    }

    return n ? *p1 - *p2 : c1;
}


/*Note:
  if type == STRING_TYPE_INTEGER, directly return the value
  if type == STRING_TYPE_IP, return value definition as below:
                       IP_NON_VALID(0): means non-valid IP
                       IP_VALID_V4(1): means valid IPV4 address
                       IP_VALID_V6(2) :means valid IPV6 address
*/

uint32_t str_convert(int type, const char *nptr, void *buff )
{
    char *endPtr;
    uint32_t res;

    if( nptr == NULL )
    {
        IFX_PPACMD_PRINT("str_convert: nptr is NULL ??\n");
        return 0;
    }
    if( type == STRING_TYPE_IP )
    {
        if( (res = inet_pton(AF_INET, nptr, buff) ) == 0 )
        {
            if( (res = inet_pton(AF_INET6, nptr, buff)) == 0 )
            {
                return IP_NON_VALID;
            }
            else return IP_VALID_V6;
        }
        else return IP_VALID_V4;

        return 0;
    }
    else if( type == STRING_TYPE_INTEGER )
    {
        if( strlen(nptr) >= 2 && nptr[0] == '0' && ( nptr[1] == 'x' || nptr[1] == 'X') ) /*hex value start with 0x */
        {
            res = strtoul(nptr, &endPtr, 16);
		if (endPtr == NULL)
			res = PPA_CMD_ERR;
		else
			return res;
        }
        else  /*by default it is decimal value */
        {
            res = strtoul(nptr, &endPtr, 10);
		if (endPtr == NULL)
			res = PPA_CMD_ERR;
		else
			return res;
        }
    }
    else
    {
        IFX_PPACMD_PRINT("str_convert: wrong type parameter(%d)\n", type);
        return 0;
    }
	return PPA_CMD_OK;
}


static int is_digital_value(char *s)
{
    int i;
    
    if( !s ) return 0;

     if( (strlen(s) > 2 ) && ( s[0] == '0')  && ( s[1] == 'x') )
    { //hex
        for(i=2; i<strlen(s); i++ )
        { //
            if( ( s[i] >='0' &&  s[i] <='9' )  || ( s[i] >='a' &&  s[i] <='f' ) || ( s[i] >='A' &&  s[i] <='F' ) )
                continue;
            else 
                return 0;
        }
    }
    else
    { //normal value
        for(i=0; i<strlen(s); i++ )
            if( s[i] >='0' &&  s[i] <='9' )  continue;
            else
                return 0; 
    }
    return 1;
}

/*
===========================================================================================
   ppa_do_ioctl_cmd

===========================================================================================
*/
static int ppa_do_ioctl_cmd(int ioctl_cmd, void *data)
{
    int ret = PPA_CMD_OK;
    int fd  = 0;

    if ((fd = open (PPA_DEVICE, O_RDWR)) < 0)
    {
        IFX_PPACMD_PRINT ("\n [%s] : open PPA device (%s) failed. (errno=%d)\n", __FUNCTION__, PPA_DEVICE, errno);
        ret = PPA_CMD_ERR;
    }
    else
    {
        if (ioctl (fd, ioctl_cmd, data) < 0)
        {
            IFX_PPACMD_PRINT ("\n [%s] : ioctl failed for NR %d. (errno=%d(system error:%s))\n", __FUNCTION__, _IOC_NR(ioctl_cmd), errno, strerror(errno));
            ret = PPA_CMD_ERR;
        }
        close (fd);
    }
    
    return ret;
}

/*
====================================================================================
   Input conversion functions
   These sunctions convert input strings to the appropriate data types for
   the ioctl commands.
===================================================================================
*/
static void stomac(char *s,unsigned char mac_addr[])
{
    unsigned int mac[PPA_ETH_ALEN];

    sscanf(s,"%2x:%2x:%2x:%2x:%2x:%2x",&mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);

    mac_addr[5] = mac[5];
    mac_addr[4] = mac[4];
    mac_addr[3] = mac[3];
    mac_addr[2] = mac[2];
    mac_addr[1] = mac[1];
    mac_addr[0] = mac[0];
    return;
}

/*
====================================================================================
   Generic option sets
   These option sets are shared among several commands.
===================================================================================
*/

static const char ppa_no_short_opts[] = "h";
static struct option ppa_no_long_opts[] =
{
    { 0,0,0,0 }
};

static const char ppa_if_short_opts[] = "i:fl:wh";
static struct option ppa_if_long_opts[] =
{
    {"interface", required_argument,  NULL, 'i'},
    {"force", no_argument,  NULL, 'f'}, //-f is used for addlan and addwan only
    {"lower", required_argument,  NULL, 'l'}, //-l is used for manually configure its lower interface
    {"hardware", no_argument,  NULL, 'w'}, // -w is used to disable HW acceleration for interface (SW acceleration would still work for that interface)
    { 0,0,0,0 }
};

static const char ppa_mac_short_opts[] = "m:h";
static const struct option ppa_mac_long_opts[] =
{
    {"macaddr",   required_argument,  NULL, 'm'},
    { 0,0,0,0 }
};

static const char ppa_if_mac_short_opts[] = "i:m:h";
static const struct option ppa_if_mac_long_opts[] =
{
    {"interface", required_argument,  NULL, 'i'},
    {"macaddr",   required_argument,  NULL, 'm'},
    { 0,0,0,0 }
};

static const char ppa_output_short_opts[] = "o:h";

static const char ppa_if_output_short_opts[] = "i:o:h";

#if defined(MIB_MODE_ENABLE)
static const char ppa_init_short_opts[] = "f:l:w:b:m:n:i:h";
#else
static const char ppa_init_short_opts[] = "f:l:w:b:m:n:h";
#endif
static const struct option ppa_init_long_opts[] =
{
    {"file",   required_argument,  NULL, 'f'},
    {"lan",    required_argument,  NULL, 'l'},
    {"wan",    required_argument,  NULL, 'w'},
    {"bridge",   required_argument,  NULL, 'b'},
    {"multicast",required_argument,  NULL, 'm'},
    {"minimal-hit",    required_argument,  NULL, 'n'},
#if defined(MIB_MODE_ENABLE)
    {"mib-mode", required_argument,  NULL, 'i'},
#endif
    {"help",   no_argument,    NULL, 'h'},
    { 0,0,0,0 }
};

static const char ppa_if_mib_short_opts[] = "i:h";

static int ppa_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    return ppa_do_ioctl_cmd(pcmd->ioctl_cmd,pdata);
}

static int ppa_do_add_del_if_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    int idx = 0;
    for (idx = 0; idx < pdata->all_if_info.num_ifinfos; idx++)
        ppa_do_ioctl_cmd(pcmd->ioctl_cmd, &(pdata->all_if_info.ifinfo[idx]));
    return 0;
}

/*
====================================================================================
   command:   init
   description: Initialize the Packet Processing Acceleration Module
   options:   None
====================================================================================
*/

typedef enum
{
    INIT_CMD_INVALID = -1,
    INIT_CMD_COMMENT,
    INIT_CMD_SECTION,
    INIT_CMD_END,
    INIT_CMD_NUM_ENTRIES,
    INIT_CMD_MULTICAST_ENTRIES,
    INIT_CMD_BRIDGE_ENTRIES,
    INIT_CMD_TCP_HITS_TO_ADD,
    INIT_CMD_UDP_HITS_TO_ADD,
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
    INIT_CMD_LAN_COLLISION,
    INIT_CMD_WAN_COLLISION,
#endif
    INIT_CMD_INTERFACE
} INIT_CMD;

const char *cfg_names[] =
{
    "comment",
    "section",
    "end",
    "ip-header-check",
    "tcp-udp-header-check",
    "drop-on-error",
    "unicast-drop-on-miss",
    "multicast-drop-on-miss",
    "max-unicast-sessions",
    "max-multicast-sessions",
    "max-bridging-sessions",
    "tcp-threshold",
    "udp-threshold",
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
    "lan-collision",
    "wan-collision",
#endif
    "interface"
};

typedef enum
{
    SECTION_NONE,
    SECTION_WAN,
    SECTION_LAN
} SECTION_NAME;

typedef enum
{
    ERR_MULTIPLE_SECTIONS,
    ERR_INVALID_SECTION_NAME,
    ERR_INVALID_COMMAND,
    ERR_NOT_IN_SECTION,
    ERR_IN_SECTION,
    ERR_INVALID_RANGE
} INIT_FILE_ERR;

static void ppa_print_init_help(int summary)
{
    if( summary )
    {
#if defined(MIB_MODE_ENABLE)
        
        IFX_PPACMD_PRINT("init [-f <filename>] [-l <lan_num>] [-w <wan_num>] [-m <mc_num>] [-b <br_num>] [ -n minimal-hit ] [-i <mib-mode> ]\n");

#else
        IFX_PPACMD_PRINT("init [-f <filename>] [-l <lan_num>] [-w <wan_num>] [-m <mc_num>] [-b <br_num>] [ -n minimal-hit ] \n");
#endif
        IFX_PPACMD_PRINT("    -l/w/m/b: to set maximum LAN/WAN/Multicast/Bridge Acceeration entries\n");
        IFX_PPACMD_PRINT("    -n: to specify minimal hit before doing acceleration\n");
#if defined(MIB_MODE_ENABLE)
        IFX_PPACMD_PRINT("    -i: to set mibmode 0-Session MIB in terms of Byte,1- in terms of Packet\n");
#endif
        IFX_PPACMD_PRINT("    -f: note, if -f option is used, then no need to use other options\n");
        
    }
    else
        IFX_PPACMD_PRINT("init [-f <filename>]\n");
    return;
}

static INIT_CMD parse_init_config_line(char *buf, char **val)
{
    char *p_cmd, *p_end;
    int ndx;
    INIT_CMD ret_cmd = INIT_CMD_INVALID;
    int eol_seen = 0;

    if (buf[0] == '#')
        return INIT_CMD_COMMENT;

    p_cmd = buf;
    while (*p_cmd != '\n' && isspace(*p_cmd))   // skip leading white space while checking for eol
        p_cmd++;
    if (*p_cmd == '\n')
        return INIT_CMD_COMMENT;         // empty line
    p_end = p_cmd;                // null terminate the command
    while (!isspace(*p_end))
        p_end++;
    if (*p_end == '\n')
        eol_seen = 1;
    *p_end = '\0';

    for (ndx = 0; ndx < (sizeof(cfg_names)/sizeof(char *)); ndx++)
    {
        if ( strcasecmp(cfg_names[ndx], p_cmd ) == 0)
        {
            // return the following string if present
            if (!eol_seen)
            {
                p_cmd = p_end + 1;
                while (*p_cmd != '\n' && isspace(*p_cmd))
                    p_cmd++;
                p_end = p_cmd;
                while (!isspace(*p_end))
                    p_end++;
                *p_end = '\0';
                *val = p_cmd;
            }
            else
            {
                *val = NULL;   // no parameter present
            }
            ret_cmd = ndx;
            break;
        }
    }
    return ret_cmd;
}

#define	CMD_LINE_SIZE	128

static int parse_init_config_file(char *filename, PPA_CMD_INIT_INFO *pinfo)
{
    FILE *fd;
    char cmd_line[CMD_LINE_SIZE];
    char *val = NULL;
    int linenum = 0;
    INIT_CMD cmd;
    int num_entries, num_hits;
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
    int lan_collisions,wan_collisions;
#endif
    SECTION_NAME curr_section = SECTION_NONE;
    int seen_wan_section = 0, seen_lan_section = 0;
    //int if_index = 0;
    int num_wanifs = 0, num_lanifs = 0;
    INIT_FILE_ERR err;

    fd = fopen(filename,"r");
    if (fd != NULL)
    {
        while ( fgets(cmd_line, 128, fd) != NULL)
        {
            linenum++;
            cmd = parse_init_config_line(cmd_line, &val);
            if(val == NULL)
            {
		err = ERR_INVALID_SECTION_NAME;
                goto parse_error;
            }
            switch(cmd)
            {
            case INIT_CMD_COMMENT:
                break;

            case INIT_CMD_SECTION:
                if (!strcasecmp("wan", val))
                {
                    if(seen_wan_section)
                    {
                        err = ERR_MULTIPLE_SECTIONS;
                        goto parse_error;
                    }
                    curr_section = SECTION_WAN;
                }
                else if (!strcasecmp("lan", val))
                {
                    if (seen_lan_section)
                    {
                        err = ERR_MULTIPLE_SECTIONS;
                        goto parse_error;
                    }
                    curr_section = SECTION_LAN;
                }
                else
                {
                    err = ERR_INVALID_SECTION_NAME;
                    goto parse_error;
                }
                break;

            case INIT_CMD_END:
                if (curr_section == SECTION_NONE)
                {
                    err = ERR_NOT_IN_SECTION;
                    goto parse_error;
                }
                if (curr_section == SECTION_WAN)
                {
                    pinfo->num_wanifs = num_wanifs;
                    seen_wan_section = 1;
                }
                else
                {
                    pinfo->num_lanifs = num_lanifs;
                    seen_lan_section = 1;
                }
                curr_section = SECTION_NONE;
                break;
            case INIT_CMD_NUM_ENTRIES:
            case INIT_CMD_BRIDGE_ENTRIES:
            case INIT_CMD_MULTICAST_ENTRIES:
                num_entries = atoi(val);
                if (num_entries > 1000 || num_entries < 0)
                {
                    err = ERR_INVALID_RANGE;
                    goto parse_error;
                }
                if (cmd == INIT_CMD_NUM_ENTRIES)
                {
                    if (curr_section == SECTION_WAN)
                        pinfo->max_wan_source_entries = num_entries;
                    else if (curr_section == SECTION_LAN)
                        pinfo->max_lan_source_entries = num_entries;
                    else
                    {
                        err = ERR_NOT_IN_SECTION;
                        goto parse_error;
                    }
                }
                else
                {
                    if (curr_section != SECTION_NONE)
                    {
                        err = ERR_IN_SECTION;
                        goto parse_error;
                    }
                    if (cmd == INIT_CMD_BRIDGE_ENTRIES)
                        pinfo->max_bridging_entries = num_entries;
                    else
                        pinfo->max_mc_entries = num_entries;
                }
                break;

            case INIT_CMD_TCP_HITS_TO_ADD:
            case INIT_CMD_UDP_HITS_TO_ADD:
                num_hits = atoi(val);
                if (num_hits < 0)
                {
                    err = ERR_INVALID_COMMAND;
                    goto parse_error;
                }
                if (cmd == INIT_CMD_TCP_HITS_TO_ADD)
                    pinfo->add_requires_min_hits = num_hits;
                else
                    pinfo->add_requires_min_hits = num_hits;
                break;

            case INIT_CMD_INTERFACE:
                if (curr_section == SECTION_NONE)
                {
                    err = ERR_NOT_IN_SECTION;
                    goto parse_error;
                }
                if (curr_section == SECTION_WAN)
                {
                    if ( num_wanifs < sizeof(pinfo->p_wanifs) / sizeof(pinfo->p_wanifs[0]) )
                    {
                        strncpy_s(pinfo->p_wanifs[num_wanifs].ifname,PPA_IF_NAME_SIZE, val,PPA_IF_NAME_SIZE);
                        pinfo->p_wanifs[num_wanifs].if_flags = 0;
                        num_wanifs++;
                    }
                }
                else if (curr_section == SECTION_LAN)
                {
                    if ( num_wanifs < sizeof(pinfo->p_lanifs) / sizeof(pinfo->p_lanifs[0]) )
                    {
                        strncpy_s(pinfo->p_lanifs[num_lanifs].ifname,PPA_IF_NAME_SIZE, val,PPA_IF_NAME_SIZE);
                        pinfo->p_lanifs[num_lanifs].if_flags = PPA_F_LAN_IF;
                        num_lanifs++;
                    }
                }
                break;

            default:
                err = ERR_INVALID_COMMAND;
                goto parse_error;
            }
        }
    }

    if( fd != NULL )
    {
        fclose(fd);
        fd = NULL;
    }
    return PPA_CMD_OK;

    // error messages
parse_error:

    switch(err)
    {
    case ERR_MULTIPLE_SECTIONS:
        IFX_PPACMD_PRINT("error: multiple section definitions - line %d\n", linenum);
        break;
    case ERR_INVALID_SECTION_NAME:
        IFX_PPACMD_PRINT("error: invalid section name - line %d\n", linenum);
        break;
    case ERR_INVALID_COMMAND:
        IFX_PPACMD_PRINT("error: invalid command - line %d\n", linenum);
        break;
    case ERR_NOT_IN_SECTION:
        IFX_PPACMD_PRINT("error: command not within valid section - line %d\n", linenum);
        break;
    case ERR_IN_SECTION:
        IFX_PPACMD_PRINT("error: command within section - line %d\n", linenum);
        break;
    case ERR_INVALID_RANGE:
        IFX_PPACMD_PRINT("error: parameter outside allowed range - line %d\n", linenum);
        break;
    }
    if( fd != NULL )
    {
        fclose(fd);
        fd = NULL;
    }
    return PPA_CMD_ERR;
}

static int ppa_parse_init_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    unsigned int i;
    PPA_CMD_INIT_INFO *pinfo = &pdata->init_info;
    PPA_CMD_MAX_ENTRY_INFO max_entries;
	
#if defined(MIB_MODE_ENABLE)
    PPA_CMD_MIB_MODE_INFO    var_mib_mode;   /*!< MIB mode configuration parameter */  
#endif
    // Default PPA Settings
    pinfo->num_lanifs = 0;
    ppa_memset(pinfo->p_lanifs,sizeof(pinfo->p_lanifs),0); 
    pinfo->num_wanifs = 0;
    ppa_memset(pinfo->p_wanifs,sizeof(pinfo->p_wanifs),0);

    pinfo->max_lan_source_entries  = 0;
    pinfo->max_wan_source_entries  = 0;
    pinfo->max_mc_entries      = 0;
    pinfo->max_bridging_entries  = 0;
    pinfo->add_requires_min_hits   = 0;

    if( ppa_do_ioctl_cmd(PPA_CMD_GET_MAX_ENTRY, &max_entries) != PPA_CMD_OK )
    {
        return -EIO;
    }

    pinfo->max_lan_source_entries = max_entries.entries.max_lan_entries;
    pinfo->max_wan_source_entries = max_entries.entries.max_wan_entries;
    pinfo->max_mc_entries     = max_entries.entries.max_mc_entries;
    pinfo->max_bridging_entries   = max_entries.entries.max_bridging_entries;

    // Override any default setting from configuration file (if specified)
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'f':
            if ( parse_init_config_file( popts->optarg, &pdata->init_info) )
            {
                IFX_PPACMD_PRINT("%s: error reading PPA configuration file: %s\n", PPA_CMD_NAME, popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case 'l':
            if(  str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL ) < pinfo->max_lan_source_entries )
                pinfo->max_lan_source_entries = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            break;

        case 'w':
            if( str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL ) < pinfo->max_wan_source_entries )
                pinfo->max_wan_source_entries = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            break;

        case 'm':
            if( str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL ) < pinfo->max_mc_entries )
                pinfo->max_mc_entries = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            break;

        case 'b':
            if( str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL ) < pinfo->max_bridging_entries)
                pinfo->max_bridging_entries = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            break;

        case 'n':
            pinfo->add_requires_min_hits = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            break;

#if defined(MIB_MODE_ENABLE)
        case 'i':
            //var_mib_mode.mib_mode = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );
            pinfo->mib_mode = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL );

           /* if( ppa_do_ioctl_cmd(PPA_CMD_SET_MIB_MODE, &var_mib_mode) != PPA_CMD_OK )
            {
                return -EIO;
            }*/
            break;
#endif
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    IFX_PPACMD_DBG("INTERFACES\n");
    IFX_PPACMD_DBG("Number of LAN IF: %d\n", pinfo->num_lanifs);
    for ( i = 0; i < pinfo->num_lanifs; i++ )
        IFX_PPACMD_DBG("  %s (%08d)\n", pinfo->p_lanifs[i].ifname, pinfo->p_lanifs[i].if_flags);
    IFX_PPACMD_DBG("Number of WAN IF: %d\n", pinfo->num_wanifs);
    for ( i = 0; i < pinfo->num_wanifs; i++ )
        IFX_PPACMD_DBG("  %s %08X)\n", pinfo->p_wanifs[i].ifname, pinfo->p_wanifs[i].if_flags);

    IFX_PPACMD_DBG("OTHER\n");
    IFX_PPACMD_DBG("   Max. LAN Entries: %d\n", pinfo->max_lan_source_entries);
    IFX_PPACMD_DBG("   Max. WAN Entries: %d\n", pinfo->max_wan_source_entries);
    IFX_PPACMD_DBG("   Max. MC Entries: %d\n", pinfo->max_mc_entries);
    IFX_PPACMD_DBG("   Max. Bridge Entries: %d\n", pinfo->max_bridging_entries);
    IFX_PPACMD_DBG("   Min. Hits: %d\n", pinfo->add_requires_min_hits);

#ifdef CONFIG_PPA_PUMA7
    system("echo enable > /proc/net/ti_pp");
    system("echo 1 > /sys/devices/platform/toe/enable");
#endif

    return PPA_CMD_OK;
}

static void ppa_print_init_fake_cmd(PPA_CMD_DATA *pdata)
{
    /* By default, we will enable ppa LAN/WAN acceleratation */
    PPA_CMD_ENABLE_INFO  enable_info;

    ppa_memset( &enable_info, sizeof(enable_info), 0) ;

    enable_info.lan_rx_ppa_enable = 1;
    enable_info.wan_rx_ppa_enable = 1;

    if( ppa_do_ioctl_cmd(PPA_CMD_ENABLE, &enable_info ) != PPA_CMD_OK )
    {
        IFX_PPACMD_PRINT("ppacmd control to enable lan/wan failed\n");
        return ;
    }
}



/*
====================================================================================
   command:   exit
   description: Remove the Packet Processing Acceleration Module
   options:   None
====================================================================================
*/
static void ppa_print_exit_help(int summary)
{
    IFX_PPACMD_PRINT("exit\n");
    return;
}

static int ppa_parse_exit_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    if (popts->opt != 0)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("EXIT COMMAND\n");

#ifdef CONFIG_PPA_PUMA7
    system("echo disable > /proc/net/ti_pp");
    system("echo 0 > /sys/devices/platform/toe/enable");
#endif

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   control
   description: Enable and Disable Packet Processing Acceleration for WAN and/or LAN
        interfaces.
   options:   Enable and/or Disable parameters
====================================================================================
*/

static const struct option ppa_control_long_opts[] =
{
    {"enable-lan",  no_argument, NULL, OPT_ENABLE_LAN},
    {"disable-lan", no_argument, NULL, OPT_DISABLE_LAN},
    {"enable-wan",  no_argument, NULL, OPT_ENABLE_WAN},
    {"disable-wan", no_argument, NULL, OPT_DISABLE_WAN},
    { 0,0,0,0 }
};

static void ppa_print_control_help(int summary)
{
    IFX_PPACMD_PRINT("control {--enable-lan|--disable-lan} {--enable-wan|--disable-wan} \n");
    return;
}

static int ppa_parse_control_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    unsigned int lan_opt = 0, wan_opt = 0;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case OPT_ENABLE_LAN:
            pdata->ena_info.lan_rx_ppa_enable = 1;
            lan_opt++;
            break;

        case OPT_DISABLE_LAN:
            pdata->ena_info.lan_rx_ppa_enable = 0;
            lan_opt++;
            break;

        case OPT_ENABLE_WAN:
            pdata->ena_info.wan_rx_ppa_enable = 1;
            wan_opt++;
            break;

        case OPT_DISABLE_WAN:
            pdata->ena_info.wan_rx_ppa_enable = 0;
            wan_opt++;
            break;
        }
        popts++;
    }

    /* Allow only one of the parameters for LAN or WAN to be specified */
    if (wan_opt > 1 || lan_opt > 1)
        return PPA_CMD_ERR;

    if (wan_opt ==0 &&  lan_opt == 0) /*sgh add: without this checking, all lan/wan acceleration will be disabled if user run command "ppacmd control" without any parameter */
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA CONTROL: LAN = %s   WAN = %s\n", pdata->ena_info.lan_rx_ppa_enable ? "enable" : "disable",
            pdata->ena_info.wan_rx_ppa_enable ? "enable" : "disable");

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   status
   description: Display Packet Processing Acceleration status for WAN and/or LAN
        interfaces.

   options:   None
====================================================================================
*/

static void ppa_print_status_help(int summary)
{
    IFX_PPACMD_PRINT("status [-o outfile] \n");
    return;
}

static int ppa_parse_status_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int out_opt = 0;

    while(popts->opt)
    {
        if (popts->opt == 'o')
        {
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            out_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if (out_opt > 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA STATUS\n");

    return PPA_CMD_OK;
}

static void ppa_print_status(PPA_CMD_DATA *pdata)
{
    if( pdata->ena_info.flags == 0 )
        IFX_PPACMD_PRINT("PPA not initialized yet\n");
    else
    {
        PPA_CMD_MAX_ENTRY_INFO max_entries;
#if defined(MIB_MODE_ENABLE)
        PPA_CMD_MIB_MODE_INFO mode_info;
#endif
#if defined(CAP_WAP_CONFIG) && CAP_WAP_CONFIG
        PPA_CMD_DATA cmd_info;
#endif
        
        ppa_memset( &max_entries, sizeof(max_entries), 0 );
        IFX_PPACMD_PRINT("  LAN Acceleration: %s.\n", pdata->ena_info.lan_rx_ppa_enable ? "enabled": "disabled");
        IFX_PPACMD_PRINT("  WAN Acceleration: %s.\n", pdata->ena_info.wan_rx_ppa_enable ? "enabled": "disabled");

        if( ppa_do_ioctl_cmd(PPA_CMD_GET_MAX_ENTRY, &max_entries) == PPA_CMD_OK )
        {
            IFX_PPACMD_PRINT("  LAN max entries:%d(Collision:%d)\n", (unsigned int)max_entries.entries.max_lan_entries, (unsigned int)max_entries.entries.max_lan_collision_entries);
            IFX_PPACMD_PRINT("  WAN max entries:%d(Collision:%d)\n", (unsigned int)max_entries.entries.max_wan_entries, (unsigned int)max_entries.entries.max_wan_collision_entries);
            IFX_PPACMD_PRINT("  LAN hash index number:%d, bucket number per index:%d)\n", (unsigned int)max_entries.entries.max_lan_hash_index_num, (unsigned int)max_entries.entries.max_lan_hash_bucket_num);
            IFX_PPACMD_PRINT("  WAN hash index number:%d, bucket number per index:%d)\n", (unsigned int)max_entries.entries.max_wan_hash_index_num, (unsigned int)max_entries.entries.max_wan_hash_bucket_num);
            IFX_PPACMD_PRINT("  MC max entries:%d\n", (unsigned int)max_entries.entries.max_mc_entries);
            IFX_PPACMD_PRINT("  Bridge max entries:%d\n", (unsigned int)max_entries.entries.max_bridging_entries);
            IFX_PPACMD_PRINT("  IPv6 address max entries:%d\n", (unsigned int)max_entries.entries.max_ipv6_addr_entries);
            IFX_PPACMD_PRINT("  PPE FW max queue:%d\n", (unsigned int)max_entries.entries.max_fw_queue);
            IFX_PPACMD_PRINT("  6RD max entries:%d\n", (unsigned int)max_entries.entries.max_6rd_entries);
            IFX_PPACMD_PRINT("  MF Flow max entries:%d\n", (unsigned int)max_entries.entries.max_mf_flow);
        }

#if defined(MIB_MODE_ENABLE)
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_MIB_MODE, &mode_info) == PPA_CMD_OK )
        {
            if(mode_info.mib_mode == 1)
                  IFX_PPACMD_PRINT("  Unicast/Multicast Session Mib in Packet\n");
            else
                  IFX_PPACMD_PRINT("  Unicast/Multicast Session Mib in Byte\n");
        }
#endif

    }
}

/*
====================================================================================
   command:   setmac
   description: Set Ethernet MAC address
   options:   None
====================================================================================
*/

static void ppa_print_set_mac_help(int summary)
{
    IFX_PPACMD_PRINT("setmac -i <ifname> -m <macaddr> \n");
    return;
}

static int ppa_parse_set_mac_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int if_opt = 0, mac_opt = 0;

    while(popts->opt)
    {
        if (popts->opt == 'i')
        {
            strncpy_s(pdata->if_mac.ifname,PPA_IF_NAME_SIZE,popts->optarg,PPA_IF_NAME_SIZE);
            pdata->if_mac.ifname[PPA_IF_NAME_SIZE-1] = 0;
            if_opt++;
        }
        else if (popts->opt == 'm')
        {
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg,pdata->if_mac.mac);
		mac_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if (mac_opt != 1 || if_opt != 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA SET MAC: %s  =  %02x:%02x:%02x:%02x:%02x:%02x\n", pdata->if_mac.ifname,
            pdata->if_mac.mac[0],
            pdata->if_mac.mac[1],
            pdata->if_mac.mac[2],
            pdata->if_mac.mac[3],
            pdata->if_mac.mac[4],
            pdata->if_mac.mac[5]);
    return PPA_CMD_OK;
}

/*
====================================================================================
  Generic add/delete/get interface functions.
  The add/delete WAN interface commands share the same data structures and command
  options so they are combined into one set of functions and shared by each.
====================================================================================
*/

static int ppa_parse_add_del_if_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int  opt = 0;
    int check_f = 0, check_l = 0, check_w = 0;

    PPA_CMD_IFINFOS  *all_if_info;

    all_if_info = &(pdata->all_if_info);

    ppa_memset(all_if_info, sizeof(PPA_CMD_IFINFOS), 0);
    while (popts->opt)
    {
        if (popts->opt == 'i')
        {
            opt++;
            strncpy_s(all_if_info->ifinfo[opt-1].ifname,PPA_IF_NAME_SIZE,popts->optarg,PPA_IF_NAME_SIZE);
            all_if_info->ifinfo[opt-1].ifname[PPA_IF_NAME_SIZE-1] = 0;
            check_f = check_l = check_w = 0;
        }
        else if (popts->opt == 'f')
        {
            if (opt == 0)
                return PPA_CMD_ERR;
            all_if_info->ifinfo[opt-1].force_wanitf_flag=1;
            check_f++;
        }
        else if (popts->opt == 'l')
        {
            if (opt == 0)
                return PPA_CMD_ERR;
            strncpy_s(all_if_info->ifinfo[opt-1].ifname_lower,PPA_IF_NAME_SIZE-1,popts->optarg,PPA_IF_NAME_SIZE-1);
            all_if_info->ifinfo[opt-1].ifname_lower[PPA_IF_NAME_SIZE-1] = 0;
            check_l++;
        }
        else if (popts->opt == 'w')
        {
            if (opt == 0)
                return PPA_CMD_ERR;
            all_if_info->ifinfo[opt-1].hw_disable = 1;
            check_w++;
        }

        if (check_f > 1 || check_l > 1 || check_w > 1)
            return PPA_CMD_ERR;

        popts++;
    }

    if( opt < 1 )
        return PPA_CMD_ERR;

    all_if_info->num_ifinfos = opt;

    IFX_PPACMD_DBG("PPA ADD/DEL IF: %s with flag=%x\n", pdata->if_info.ifname, pdata->if_info.force_wanitf_flag);

    return PPA_CMD_OK;
}

static void ppa_print_lan_netif_cmd(PPA_CMD_DATA *pdata)
{
    int i;
    if( !g_output_to_file )
    {
        for(i=0; i<pdata->all_if_info.num_ifinfos; i++ )
            IFX_PPACMD_PRINT("[%2d] %15s with acc_rx/acc_tx %llu:%llu\n", i, pdata->all_if_info.ifinfo[i].ifname, pdata->all_if_info.ifinfo[i].acc_rx, pdata->all_if_info.ifinfo[i].acc_tx );
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->all_if_info), sizeof(pdata->all_if_info) );
    }

}
static void ppa_print_get_lan_netif_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
        IFX_PPACMD_PRINT("LAN IF: ---\n");
    ppa_print_lan_netif_cmd(pdata);
}

static void ppa_print_get_wan_netif_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
        IFX_PPACMD_PRINT("WAN IF: ---\n");
    ppa_print_lan_netif_cmd(pdata);
}

/*
====================================================================================
   command:   addwan
        delwan
        getwan
   description: Add WAN interface to PPA
   options:
====================================================================================
*/

static void ppa_print_add_wan_help(int summary)
{
    IFX_PPACMD_PRINT("addwan -i <ifname>\n");
    if( summary )
    {
       IFX_PPACMD_PRINT("addwan -i <ifname> -f\n");
       IFX_PPACMD_PRINT("    Note:  -f is used to force change WAN interface in PPE FW level. Be careful to use it !!\n");
       IFX_PPACMD_PRINT("           -l is used to manually configure its lower interface in case auto-searching failed !!\n");
       IFX_PPACMD_PRINT("           multiple interfaces can passed with -i <ifname>...\n");
    }
    return;
}

static void ppa_print_del_wan_help(int summary)
{
    IFX_PPACMD_PRINT("delwan -i <ifname>\n");
    IFX_PPACMD_PRINT("multiple interfaces can passed with -i <ifname>...\n");
    return;
}

static void ppa_print_get_wan_help(int summary)
{
    IFX_PPACMD_PRINT("getwan [-o outfile]\n");
    return;
}

/*
====================================================================================
   command:    addlan
         dellan
         getlan
   description:
   options:
====================================================================================
*/

static void ppa_print_add_lan_help(int summary)
{
    IFX_PPACMD_PRINT("addlan -i <ifname>\n");
    if( summary )
    {
        
        IFX_PPACMD_PRINT("addlan -i <ifname> -f\n");
        IFX_PPACMD_PRINT("    Note:  -f is used to force change LAN interface in PPE FW level. Be careful to use it !!\n");
        IFX_PPACMD_PRINT("           -l is used to manually configure its lower interface in case auto-searching failed !!\n");
    }
    return;
}

static void ppa_print_del_lan_help(int summary)
{
    IFX_PPACMD_PRINT("dellan -i <ifname>\n");
    return;
}

static void ppa_print_get_lan_help(int summary)
{
    IFX_PPACMD_PRINT("getlan [ -o outfile ]\n");
    return;
}

/*
====================================================================================
   command:   getmac
   description: Get Ethernet MAC address
   options:   interface for MAC address
====================================================================================
*/

static void ppa_print_get_mac_help(int summary)
{
    IFX_PPACMD_PRINT("getmac -i <ifname> [-o outfile] \n");
    return;
}

static int ppa_parse_get_mac_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int if_opt = 0, out_opt = 0;

    while (popts->opt)
    {
        if (popts->opt == 'i')
        {
            strncpy_s(pdata->if_info.ifname,PPA_IF_NAME_SIZE,popts->optarg,PPA_IF_NAME_SIZE);
            pdata->if_info.ifname[PPA_IF_NAME_SIZE-1] = 0;
            if_opt++;
        }
        else if (popts->opt == 'o')
        {
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            out_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    // Each parameter must be specified just once.
    if (out_opt  > 1 || if_opt != 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA GET MAC: %s\n", pdata->if_info.ifname);

    return PPA_CMD_OK;
}

static void ppa_print_get_mac_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("%02x:%02x:%02x:%02x:%02x:%02x\n", pdata->if_mac.mac[0],pdata->if_mac.mac[1],pdata->if_mac.mac[2],
                         pdata->if_mac.mac[3],pdata->if_mac.mac[4],pdata->if_mac.mac[5] );
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->if_mac), sizeof(pdata->if_mac) );
    }
}

/*
====================================================================================
   command:   addbr
   description:
   options:
====================================================================================
*/

static void ppa_add_mac_entry_help(int summary)
{
    IFX_PPACMD_PRINT("addbr -m <macaddr> -i <ifname> \n");
    return;
}

static int ppa_parse_add_mac_entry_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int if_opt = 0, mac_opt = 0;

    while (popts->opt)
    {
        if (popts->opt == 'i')
        {
            strncpy_s(pdata->mac_entry.ifname,PPA_IF_NAME_SIZE, popts->optarg,PPA_IF_NAME_SIZE);
            pdata->mac_entry.ifname[PPA_IF_NAME_SIZE-1] = 0;
            if_opt++;
        }
        else if (popts->opt == 'm')
        {
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg,pdata->mac_entry.mac_addr);
		mac_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    // Each parameter must be specified just once.
    if (mac_opt != 1 || if_opt != 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA ADD MAC: %s  =  %02x:%02x:%02x:%02x:%02x:%02x\n", pdata->mac_entry.ifname,
            pdata->mac_entry.mac_addr[0],
            pdata->mac_entry.mac_addr[1],
            pdata->mac_entry.mac_addr[2],
            pdata->mac_entry.mac_addr[3],
            pdata->mac_entry.mac_addr[4],
            pdata->mac_entry.mac_addr[5]);


    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   delbr
   description:
   options:
====================================================================================
*/

static void ppa_del_mac_entry_help(int summary)
{
    IFX_PPACMD_PRINT("delbr -m <macaddr> \n");
    return;
}

static int ppa_parse_del_mac_entry_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int mac_opt = 0;

    while (popts->opt)
    {
        if (popts->opt == 'm')
        {
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg,pdata->mac_entry.mac_addr);
		mac_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    // MAC parameter must be specified just once.
    if (mac_opt != 1)
        return PPA_CMD_ERR;


    IFX_PPACMD_DBG("PPA DEL MAC:  %02x:%02x:%02x:%02x:%02x:%02x\n", pdata->mac_entry.mac_addr[0],
            pdata->mac_entry.mac_addr[1],
            pdata->mac_entry.mac_addr[2],
            pdata->mac_entry.mac_addr[3],
            pdata->mac_entry.mac_addr[4],
            pdata->mac_entry.mac_addr[5]);
    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   setvif
   description: Set interface VLAN configuration.
   options:
====================================================================================
*/

static const char ppa_set_vlan_if_cfg_short_opts[] = "i:V:c:O:h";

static const struct option ppa_set_vlan_if_cfg_long_opts[] =
{
    {"interface",     required_argument,  NULL,  'i'},
    {"vlan-type",     required_argument,  NULL,  'V'},
    {"tag-control",     required_argument,  NULL,  'c'},
    {"outer-tag-control", required_argument,  NULL,  'O'},
    {"vlan-aware",    no_argument,    NULL,  OPT_VLAN_AWARE},
    {"outer-tag-control", no_argument,    NULL,  OPT_OUTER_VLAN_AWARE},
    { 0,0,0,0 }
};

static void ppa_set_vlan_if_cfg_help(int summary)
{
    IFX_PPACMD_PRINT("setvif -i <ifname> -V <vlan-type> -c <inner-tag-control> -O <outer-tag-control>\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("    <vlan-type>   := {src-ip-addr|eth-type|ingress-vid|port} \n");
        IFX_PPACMD_PRINT("    <tag-control> := {insert|remove|replace|none} \n");

        IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    }
    return;
}

static int ppa_parse_set_vlan_if_cfg_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int vlan_type_opt = 0, in_tag_opt = 0, out_tag_opt = 0;
    unsigned int in_aware_opt = 0, out_aware_opt = 0, if_opt = 0;

    while (popts->opt)
    {
        switch(popts->opt)
        {
        case 'V':
            if (strcmp("src-ip-addr",popts->optarg) == 0)
                pdata->br_vlan.vlan_cfg.src_ip_based_vlan = 1;
            else if (strcmp("eth-type",popts->optarg) == 0)
                pdata->br_vlan.vlan_cfg.eth_type_based_vlan = 1;
            else if (strcmp("ingress-vid",popts->optarg) == 0)
                pdata->br_vlan.vlan_cfg.vlanid_based_vlan = 1;
            else if (strcmp("port",popts->optarg) == 0)
                pdata->br_vlan.vlan_cfg.port_based_vlan = 1;
            else
                return PPA_CMD_ERR;
            vlan_type_opt++;
            break;

        case 'c':
            if (strcmp("insert",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.insertion = 1;
            else if (strcmp("remove",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.remove = 1;
            else if (strcmp("replace",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.replace = 1;
            else if (strcmp("none",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.unmodified = 1;
            else
                return PPA_CMD_ERR;
            in_tag_opt++;
            break;

        case 'O':
            if (strcmp("insert",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.out_insertion = 1;
            else if (strcmp("remove",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.out_remove = 1;
            else if (strcmp("replace",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.out_replace = 1;
            else if (strcmp("none",popts->optarg) == 0)
                pdata->br_vlan.vlan_tag_ctrl.out_unmodified = 1;
            else
                return PPA_CMD_ERR;
            out_tag_opt++;
            break;

        case 'i':
            strncpy_s(pdata->br_vlan.if_name,PPA_IF_NAME_SIZE,popts->optarg,PPA_IF_NAME_SIZE);
            pdata->br_vlan.if_name[PPA_IF_NAME_SIZE-1] = 0;
            if_opt++;
            break;

        case OPT_VLAN_AWARE:
            pdata->br_vlan.vlan_cfg.vlan_aware = 1;
            in_aware_opt++;
            break;

        case OPT_OUTER_VLAN_AWARE:
            pdata->br_vlan.vlan_cfg.out_vlan_aware = 1;
            out_aware_opt++;
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if (   /*vlan_type_opt > 1 ||*/ if_opt     != 1
                                    || in_tag_opt  > 1 || out_tag_opt   > 1
                                    || in_aware_opt  > 1 || out_aware_opt > 1)
        return PPA_CMD_ERR;


    // Set default values is not specified in command line
    if (vlan_type_opt == 0)
        pdata->br_vlan.vlan_cfg.port_based_vlan = 1;

    if (in_tag_opt == 0)
        pdata->br_vlan.vlan_tag_ctrl.unmodified = 1;

    if (out_tag_opt == 0)
        pdata->br_vlan.vlan_tag_ctrl.out_unmodified = 1;

    IFX_PPACMD_DBG("VLAN TYPE:%s\n", pdata->br_vlan.if_name);
    IFX_PPACMD_DBG("  SRC IP VLAN: %s\n", pdata->br_vlan.vlan_cfg.src_ip_based_vlan ? "enable" : "disable");
    IFX_PPACMD_DBG("  ETH TYPE VLAN: %s\n", pdata->br_vlan.vlan_cfg.eth_type_based_vlan ? "enable" : "disable");
    IFX_PPACMD_DBG("     VID VLAN: %s\n", pdata->br_vlan.vlan_cfg.vlanid_based_vlan ? "enable" : "disable");
    IFX_PPACMD_DBG("PORT BASED VLAN: %s\n", pdata->br_vlan.vlan_cfg.port_based_vlan ? "enable" : "disable");

    IFX_PPACMD_DBG("TAG CONTROL\n");
    IFX_PPACMD_DBG("    INSERT: %s\n", pdata->br_vlan.vlan_tag_ctrl.insertion ? "enable" : "disable");
    IFX_PPACMD_DBG("    REMOVE: %s\n", pdata->br_vlan.vlan_tag_ctrl.remove ? "enable" : "disable");
    IFX_PPACMD_DBG("     REPLACE: %s\n", pdata->br_vlan.vlan_tag_ctrl.replace ? "enable" : "disable");
    IFX_PPACMD_DBG("  OUT INSERT: %s\n",pdata->br_vlan.vlan_tag_ctrl.out_insertion ? "enable" : "disable");
    IFX_PPACMD_DBG("  OUT REMOVE: %s\n", pdata->br_vlan.vlan_tag_ctrl.out_remove ? "enable" : "disable");
    IFX_PPACMD_DBG("   OUT REPLACE: %s\n", pdata->br_vlan.vlan_tag_ctrl.out_replace ? "enable" : "disable");
    IFX_PPACMD_DBG("  VLAN AWARE: %s\n", pdata->br_vlan.vlan_cfg.vlan_aware ? "enable" : "disable");
    IFX_PPACMD_DBG("OUT VLAN AWARE: %s\n", pdata->br_vlan.vlan_cfg.out_vlan_aware ? "enable" : "disable");

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:
   description:
   options:
====================================================================================
*/

static void ppa_get_vlan_if_cfg_help(int summary)
{
    IFX_PPACMD_PRINT("getvif -i <ifname> [-o outfile] \n");
    if( summary ) IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    return;
}

static int ppa_parse_get_vlan_if_cfg_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int out_opts = 0, if_opts = 0;

    while (popts->opt)
    {
        if (popts->opt == 'i')
        {
            strcpy_s(pdata->br_vlan.if_name,PPA_IF_NAME_SIZE, popts->optarg);
            if_opts++;
        }
        else if (popts->opt == 'o')
        {
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            out_opts++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if (out_opts > 1 || if_opts != 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA GET VLAN CFG: %s\n", pdata->br_vlan.if_name);

    return PPA_CMD_OK;
}


static void ppa_print_get_vif(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        //vlan_tag_ctrl, &cmd_info.br_vlan.vlan_cfg, c
        IFX_PPACMD_PRINT("%s: ", pdata->br_vlan.if_name);
        if(  pdata->br_vlan.vlan_cfg.eth_type_based_vlan )
            IFX_PPACMD_PRINT(" ether-type based, ");
        if(  pdata->br_vlan.vlan_cfg.src_ip_based_vlan)
            IFX_PPACMD_PRINT(" src-ip based, ");
        if(  pdata->br_vlan.vlan_cfg.vlanid_based_vlan )
            IFX_PPACMD_PRINT(" vlan id based, ");
        if(  pdata->br_vlan.vlan_cfg.port_based_vlan )
            IFX_PPACMD_PRINT(" port based,");

        IFX_PPACMD_PRINT( "%s, ", pdata->br_vlan.vlan_cfg.vlan_aware ? "inner vlan aware":"inner vlan no");
        IFX_PPACMD_PRINT( "%s, ", pdata->br_vlan.vlan_cfg.out_vlan_aware ? "out vlan aware":"outlvan vlan no");

        if( pdata->br_vlan.vlan_tag_ctrl.unmodified )
            IFX_PPACMD_PRINT("inner-vlan unmodified, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.insertion )
            IFX_PPACMD_PRINT("inner-vlan insert, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.remove )
            IFX_PPACMD_PRINT("inner-vlan remove, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.replace )
            IFX_PPACMD_PRINT("inner-vlan replace, ");


        if( pdata->br_vlan.vlan_tag_ctrl.out_unmodified )
            IFX_PPACMD_PRINT("out-vlan unmodified, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.out_insertion )
            IFX_PPACMD_PRINT("out-vlan insert, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.out_remove )
            IFX_PPACMD_PRINT("out-vlan remove, ");
        else  if( pdata->br_vlan.vlan_tag_ctrl.out_replace )
            IFX_PPACMD_PRINT("out-vlan replace, ");

        IFX_PPACMD_PRINT("\n");

    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->br_vlan),  sizeof(pdata->br_vlan)  );
    }
}

/*
====================================================================================
   command:   addvfilter
   description:
   options:
====================================================================================
*/
typedef struct vlan_ctrl
{
    char* cmd_str;  //command
    char op; //qid
} vlan_ctrl;
vlan_ctrl vlan_ctrl_list[]= {{"none", 0},{"remove", 1},{"insert", 2},{"replace", 3} };

static const char ppa_add_vlan_filter_short_opts[] = "t:V:i:a:e:o:q:d:c:r:h";
static const struct option ppa_add_vlan_filter_long_opts[] =
{
    {"vlan-tag",  required_argument,  NULL, 't'},
    {"ingress-vid", required_argument,  NULL, 'V'},
    {"interface",  required_argument,  NULL, 'i'},
    {"src-ipaddr",  required_argument,  NULL, 'a'},
    {"eth-type",  required_argument,  NULL, 'e'},
    {"out-vlan-id", required_argument,  NULL, 'o'},
    {"dest_qos",  required_argument,  NULL, 'q'},
    {"dst-member",  required_argument,  NULL, 'd'},
    {"inner-vctrl",  required_argument,  NULL, 'r'},
    {"outer-vctrl",  required_argument,  NULL, 'c'},
    { 0,0,0,0 }
};

static void ppa_add_vlan_filter_help(int summary)
{
    if( !summary )
    {
        //only display part of parameter since there are too many parameters
        IFX_PPACMD_PRINT("addvfilter {-i <ifname>|-a <src-ip-addr>|-e <eth-type>|-V <vlan-id>} ...\n");
    }
    else
    {
        IFX_PPACMD_PRINT("addvfilter {-i <ifname>|-a <src-ip-addr>|-e <eth-type>|-V <vlan-id>} -t <vlan-tag>\n");
        IFX_PPACMD_PRINT("    -o <out_vlan_id> -q <queue_id> -d <member-list> -c <in-tag-control> -r <out-tag-control>\n");
        IFX_PPACMD_PRINT("    parameter c/r: for tag based vlan filter only\n");
        IFX_PPACMD_PRINT("    <tag-control: none | remove | insert | replace\n");
        IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    }
    return;
}

static int ppa_parse_add_vlan_filter_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int i, j, tag_opts = 0, match_opts = 0, out_vlan_id_opts=0, vlan_if_member_opts=0, qid_opts=0,inner_vlan_ctrl_opts=0, out_vlan_ctrl_opts=0;
    uint32_t vlan_ctrl;

    ppa_memset( &pdata->vlan_filter, sizeof(pdata->vlan_filter), 0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 't':  /*inner vlan: for all kinds of vlan filters */
            pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_vci = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            tag_opts++;
            break;

        case 'i': /*port based vlan filter: for comparing  */
            strncpy_s(pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ifname,PPA_IF_NAME_SIZE-1, popts->optarg, PPA_IF_NAME_SIZE-1);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_IFNAME;
            match_opts++;
            break;

        case 'a': /*ip based vlan filter: for comparing*/
            inet_aton(popts->optarg,&pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ip_src);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_IP_SRC;
            match_opts++;
            break;

        case 'e': /*protocol based vlan filter: for comparing */
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.eth_protocol = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_ETH_PROTO;
            match_opts++;
            break;

        case 'V': /*vlan tag based vlan filter: for comparing */
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ingress_vlan_tag = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_VLAN_TAG;
            match_opts++;
            break;

        case 'o': /*outer vlan: for all kinds of vlan filters */
            pdata->vlan_filter.vlan_filter_cfg.vlan_info.out_vlan_id= str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            out_vlan_id_opts++;
            break;

        case 'd': /*member list: for all kinds of vlan filters */
            if( vlan_if_member_opts < PPA_MAX_IFS_NUM )
            {
                strncpy_s(pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_if_membership[vlan_if_member_opts].ifname,sizeof(pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_if_membership[vlan_if_member_opts].ifname), popts->optarg, sizeof(pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_if_membership[vlan_if_member_opts].ifname) );

                vlan_if_member_opts++;
                pdata->vlan_filter.vlan_filter_cfg.vlan_info.num_ifs = vlan_if_member_opts;

            }
            break;

        case 'q': /*qid: for all kinds of vlan filters */
            pdata->vlan_filter.vlan_filter_cfg.vlan_info.qid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            qid_opts++;
            break;

        case 'c': //inner vlan ctrl: only for vlan tag based vlan filter
        case 'r': //out vlan ctrl:: only for vlan tag based vlan filter
            for(i=0; i<sizeof(vlan_ctrl_list)/sizeof(vlan_ctrl_list[0]); i++ )
            {
                if( strcmp( vlan_ctrl_list[i].cmd_str, popts->optarg ) == 0 )
                {
                    if( popts->opt == 'c' )
                    {
                        vlan_ctrl = vlan_ctrl_list[i].op << 2;
                        pdata->vlan_filter.vlan_filter_cfg.vlan_info.inner_vlan_tag_ctrl = 0;
                        for(j=0; j<8; j++)
                        {
                            pdata->vlan_filter.vlan_filter_cfg.vlan_info.inner_vlan_tag_ctrl |= vlan_ctrl << ( 4 * j );
                        }
                        inner_vlan_ctrl_opts++;
                        break;
                    }
                    else
                    {
                        vlan_ctrl = (vlan_ctrl_list[i].op);
                        pdata->vlan_filter.vlan_filter_cfg.vlan_info.out_vlan_tag_ctrl = 0;
                        for(j=0; j<8; j++)
                        {
                            pdata->vlan_filter.vlan_filter_cfg.vlan_info.out_vlan_tag_ctrl |= vlan_ctrl << ( 4 * j);
                        }
                        out_vlan_ctrl_opts++;
                        break;
                    }
                }
            }
            break;

        default:
            IFX_PPACMD_PRINT("not known parameter: %c\n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    /* Check that match field is not defined more than once and VLAN tag is specified */
    if ( ( match_opts != 1) || (tag_opts  != 1) || (out_vlan_id_opts !=1)  || (vlan_if_member_opts == 0)  )
    {
        if( match_opts != 1)
            IFX_PPACMD_PRINT( "match_opts wrong:%d\n", match_opts);
        else     if( tag_opts != 1)
            IFX_PPACMD_PRINT( "tag_opts wrong:%d\n", tag_opts);
        else     if( out_vlan_id_opts != 1)
            IFX_PPACMD_PRINT( "out_vlan_id_opts wrong:%d\n", out_vlan_id_opts);
        else     if( vlan_if_member_opts != 1)
            IFX_PPACMD_PRINT( "vlan_if_member_opts wrong:%d\n", vlan_if_member_opts);


        return PPA_CMD_ERR;
    }

    if( qid_opts == 0 )
    {
        pdata->vlan_filter.vlan_filter_cfg.vlan_info.qid = PPA_INVALID_QID;
    }


    if( pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_VLAN_TAG )
    {
        if( (inner_vlan_ctrl_opts != 1) || (out_vlan_ctrl_opts != 1) )
        {
            IFX_PPACMD_PRINT("vlan control wrong: inner_vlan_ctrl_opts=%d, out_vlan_ctrl_opts=%d\n", inner_vlan_ctrl_opts , out_vlan_ctrl_opts);
            return PPA_CMD_ERR;
        }
    }
    else
    {
        if(( inner_vlan_ctrl_opts != 0) ||( out_vlan_ctrl_opts != 0 ) )
        {
            IFX_PPACMD_PRINT("vlan control wrong 2: inner_vlan_ctrl_opts=%d, out_vlan_ctrl_opts=%d\n", inner_vlan_ctrl_opts , out_vlan_ctrl_opts);
            return PPA_CMD_ERR;
        }
    }


    IFX_PPACMD_DBG("INNER VLAN TAG: 0x%d\n", pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_vci);
    IFX_PPACMD_DBG("OUT VLAN TAG: 0x%d\n", pdata->vlan_filter.vlan_filter_cfg.vlan_info.out_vlan_id);

    IFX_PPACMD_DBG("MATCH FIELD\n");
    switch(pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags)
    {
    case PPA_F_VLAN_FILTER_VLAN_TAG:
        IFX_PPACMD_DBG("VLAN TAG: %04d\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ingress_vlan_tag);
        IFX_PPACMD_DBG("INNER VLAN CTRL: %s\n", vlan_ctrl_list[ (pdata->vlan_filter.vlan_filter_cfg.vlan_info.inner_vlan_tag_ctrl >> 2 ) & 0x3].cmd_str);
        IFX_PPACMD_DBG("OUT   VLAN CTRL: %s\n", vlan_ctrl_list[ (pdata->vlan_filter.vlan_filter_cfg.vlan_info.out_vlan_tag_ctrl ) & 0x3].cmd_str);
        break;
    case PPA_F_VLAN_FILTER_IFNAME:
        IFX_PPACMD_DBG( "IF NAME: %s\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ifname);
        break;
    case PPA_F_VLAN_FILTER_IP_SRC:
        IFX_PPACMD_DBG("IP SRC: %08d\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ip_src);
        break;
    case PPA_F_VLAN_FILTER_ETH_PROTO:
        IFX_PPACMD_DBG("ETH TYPE: %04d\n",pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.eth_protocol);
        break;
    }

    for(i=0; i< pdata->vlan_filter.vlan_filter_cfg.vlan_info.num_ifs; i++ )
    {
        IFX_PPACMD_DBG("Dest member[%d]=%s\n", i, pdata->vlan_filter.vlan_filter_cfg.vlan_info.vlan_if_membership[i].ifname);
    }
    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   delvfilter
   description:
   options:
====================================================================================
*/

static const char ppa_del_vlan_filter_short_opts[] = "V:i:a:e:h";

static const struct option ppa_del_vlan_filter_long_opts[] =
{
    {"ingress-vid", required_argument,  NULL, 'V'},
    {"interface",   required_argument,  NULL, 'i'},
    {"src-ipaddr",  required_argument,  NULL, 'a'},
    {"eth-type",  required_argument,  NULL, 'e'},
    { 0,0,0,0 }
};

static void ppa_del_vlan_filter_help(int summary)
{
    IFX_PPACMD_PRINT("delvfilter {-i <ifname>|-a <src-ip-addr>|-e <eth-type>|-V <vlan-id>} \n");
    if( summary )   IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    return;
}

static int ppa_parse_del_vlan_filter_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int match_opts = 0;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i': /*port based vlan filter: for comparing  */
            strncpy_s(pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ifname,PPA_IF_NAME_SIZE, popts->optarg, PPA_IF_NAME_SIZE);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_IFNAME;
            match_opts++;
            break;

        case 'a': /*ip based vlan filter: for comparing*/
            inet_aton(popts->optarg, &pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ip_src);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_IP_SRC;
            match_opts++;
            break;

        case 'e': /*protocol based vlan filter: for comparing */
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.eth_protocol = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_ETH_PROTO;
            match_opts++;
            break;

        case 'V': /*vlan tag based vlan filter: for comparing */
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ingress_vlan_tag = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags = PPA_F_VLAN_FILTER_VLAN_TAG;
            match_opts++;
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    /* Check that match field is not defined more than once and VLAN tag is specified */
    if ( match_opts != 1)
        return PPA_CMD_ERR;


    switch(pdata->vlan_filter.vlan_filter_cfg.match_field.match_flags)
    {
    case PPA_F_VLAN_FILTER_VLAN_TAG:
        IFX_PPACMD_DBG("VLAN TAG: %04d\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ingress_vlan_tag);
        break;
    case PPA_F_VLAN_FILTER_IFNAME:
        IFX_PPACMD_DBG(" IF NAME: %s\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ifname);
        break;
    case PPA_F_VLAN_FILTER_IP_SRC:
        IFX_PPACMD_DBG("  IP SRC: %08d\n", pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.ip_src);
        break;
    case PPA_F_VLAN_FILTER_ETH_PROTO:
        IFX_PPACMD_DBG("ETH TYPE: %04d\n",pdata->vlan_filter.vlan_filter_cfg.match_field.match_field.eth_protocol);
        break;
    }
    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   getvfiltercount
   description: get vlan fitlers counter
   options:
====================================================================================
*/
static void ppa_get_vfilter_count_help(int summary)
{
    IFX_PPACMD_PRINT("getvfilternum [-o file ]\n");
    if( summary ) IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    return;
}

static int ppa_parse_get_vfilter_count(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int res;

    res =   ppa_parse_simple_cmd( popts, pdata );

    if( res != PPA_CMD_OK ) return res;


    pdata->count_info.flag = 0;

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   getfilters
   description: get all vlan fitlers information
   options:
====================================================================================
*/

static void ppa_get_all_vlan_filter_help(int summary)
{
    IFX_PPACMD_PRINT("getvfilters [-o file ]\n");
    if( summary ) IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    return;
}

static int ppa_get_all_vlan_filter_cmd (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_VLAN_ALL_FILTER_CONFIG *psession_buffer;
    PPA_CMD_DATA cmd_info;
    int res = PPA_CMD_OK, i, j, size;
    uint32_t flag = PPA_CMD_GET_ALL_VLAN_FILTER_CFG;
    unsigned char bfCorrectType = 0;

    //get session count first before malloc memroy
    cmd_info.count_info.flag = 0;
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_VLAN_FILTER, &cmd_info ) != PPA_CMD_OK )
        return -EIO;

    if( cmd_info.count_info.count == 0 )
    {
        IFX_PPACMD_DBG("vfilter count=0\n");
        if( g_output_to_file )
            SaveDataToFile(g_output_filename, (char *)(&cmd_info.count_info),  sizeof(cmd_info.count_info)  );
        return PPA_CMD_OK;
    }

    //malloc memory and set value correctly
    size = sizeof(PPA_CMD_COUNT_INFO) + sizeof(PPA_CMD_VLAN_FILTER_CONFIG) * ( 1 + cmd_info.count_info.count ) ;
    psession_buffer = (PPA_CMD_VLAN_ALL_FILTER_CONFIG *) malloc( size );
    if( psession_buffer == NULL )
    {
        IFX_PPACMD_PRINT("Malloc %d bytes failed\n", size );
        return PPA_CMD_NOT_AVAIL;
    }

    ppa_memset( psession_buffer, sizeof(size), 0 );

    psession_buffer->count_info.count = cmd_info.count_info.count;
    psession_buffer->count_info.flag = 0;

    //get session information
    if( (res = ppa_do_ioctl_cmd(flag, psession_buffer ) != PPA_CMD_OK ) )
    {
        free( psession_buffer );
        return res;
    }

    IFX_PPACMD_DBG("Vfilter count=%u. \n", (unsigned int)psession_buffer->count_info.count);


    if( !g_output_to_file )
    {
        for(i=0; i<psession_buffer->count_info.count; i++ )
        {
            IFX_PPACMD_PRINT("[%02d] ", i );
            if( psession_buffer->filters[i].vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_VLAN_TAG )
            {
                bfCorrectType = 1;
                IFX_PPACMD_PRINT("Vlan tag based:vlan %u. Qos:%u. ",  (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.match_field.match_field.ingress_vlan_tag, (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.vlan_info.qid);
            }
            else  if( psession_buffer->filters[i].vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_IFNAME  )
            {
                bfCorrectType = 1;
                IFX_PPACMD_PRINT("Port based: %s. Qos:%d. ",  psession_buffer->filters[i].vlan_filter_cfg.match_field.match_field.ifname, psession_buffer->filters[i].vlan_filter_cfg.vlan_info.qid);
            }
            else if( psession_buffer->filters[i].vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_IP_SRC)
            {
                bfCorrectType = 1;
                IFX_PPACMD_PRINT("Src ip based: %u.%u.%u.%u. Qos:%d. ", NIPQUAD( psession_buffer->filters[i].vlan_filter_cfg.match_field.match_field.ip_src) , psession_buffer->filters[i].vlan_filter_cfg.vlan_info.qid);
            }
            else if( psession_buffer->filters[i].vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_ETH_PROTO)
            {
                bfCorrectType = 1;
                IFX_PPACMD_PRINT("Ether type based: %04x. Qos:%u. ",  (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.match_field.match_field.eth_protocol, (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.vlan_info.qid);
            }

            if( bfCorrectType )
            {
                IFX_PPACMD_PRINT("Inner/Out VLAN:%03X/%03x ", (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.vlan_info.vlan_vci, (unsigned int)psession_buffer->filters[i].vlan_filter_cfg.vlan_info.out_vlan_id );

                IFX_PPACMD_PRINT("Dst members:");
                for(j=0; j<psession_buffer->filters[i].vlan_filter_cfg.vlan_info.num_ifs ; j++ )
                {
                    if( i == 0 )
                        IFX_PPACMD_PRINT("%s", psession_buffer->filters[i].vlan_filter_cfg.vlan_info.vlan_if_membership[j].ifname);
                    else
                        IFX_PPACMD_PRINT(",%s", psession_buffer->filters[i].vlan_filter_cfg.vlan_info.vlan_if_membership[j].ifname);
                }
                IFX_PPACMD_PRINT(".");

                if (psession_buffer->filters[i].vlan_filter_cfg.match_field.match_flags == PPA_F_VLAN_FILTER_VLAN_TAG )
                {
                    IFX_PPACMD_PRINT("  Inner/Out vlan control:%s/%s ",
                                     vlan_ctrl_list[ (psession_buffer->filters[i].vlan_filter_cfg.vlan_info.inner_vlan_tag_ctrl >> 2) & 0x3].cmd_str,
                                     vlan_ctrl_list[ (psession_buffer->filters[i].vlan_filter_cfg.vlan_info.out_vlan_tag_ctrl >> 0) & 0x3].cmd_str);
                }
            }
            IFX_PPACMD_PRINT("\n");
        }
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(psession_buffer),  size  );
    }

    free( psession_buffer );
    return PPA_CMD_OK;
}

/*multicast bridging/routing */
static const char ppa_mc_add_short_opts[] = "s:g:l:w:f:t:i:h"; //need to further implement add/remove/modify vlan and enable new dscp and its value
static const struct option ppa_mc_add_long_opts[] =
{
    {"bridging_flag",  required_argument,  NULL, 's'},  /*0-means routing, 1 means bridging */
    /*{"multicat mac address", required_argument,  NULL, 'm'}, */  /*for bridging only as ritech suggest. I may not help at present */
    {"multicast group",   required_argument,  NULL, 'g'},
    {"down interface",  required_argument,  NULL, 'l'},
    {"up interface",  required_argument,  NULL, 'w'},
    {"source_ip",  required_argument,  NULL, 'i'},
    {"ssm_flag",  required_argument,  NULL, 'f'},
    {"qid",  required_argument,  NULL, 't'},
    { 0,0,0,0 }
};

static unsigned int is_ip_zero(IP_ADDR_C *ip)
{
    if(ip->f_ipv6){
		return ((ip->ip.ip6[0] | ip->ip.ip6[1] | ip->ip.ip6[2] | ip->ip.ip6[3]) == 0);
	}else{
		return (ip->ip.ip == 0);
	}
}

static unsigned int ip_equal(IP_ADDR_C *dst_ip, IP_ADDR_C *src_ip)
{
	if(dst_ip->f_ipv6){
		return (((dst_ip->ip.ip6[0] ^ src_ip->ip.ip6[0] ) |
			     (dst_ip->ip.ip6[1] ^ src_ip->ip.ip6[1] ) |
			     (dst_ip->ip.ip6[2] ^ src_ip->ip.ip6[2] ) |
			     (dst_ip->ip.ip6[3] ^ src_ip->ip.ip6[3] )) == 0);
	}else{
		return ( (dst_ip->ip.ip ^ src_ip->ip.ip) == 0);
	}
}


static void ppa_add_mc_help( int summary)
{
    if( !summary )
    {
        IFX_PPACMD_PRINT("addmc -g ip-group -l <down interface> -w<up-interface> -i< src-ip> -f<ssm_flag> ...\n");
    }
    else
    {
        IFX_PPACMD_PRINT("addmc [-s <bridging_flag>] -g ip-group -l <down interface> -w<up-interface> -i< src-ip> -f<ssm_flag>\n");
        IFX_PPACMD_PRINT("      option -s: must be 1 for multicast bridging. For multicast routing, it can be 0 or skipp this option \n");
        IFX_PPACMD_PRINT("      option -l: if not set this parameter, it means to delete the multicast group \n");
        IFX_PPACMD_PRINT("      option -f: only dummy parameter now for IGMP V3\n");
    }
    return;
}

static int ppa_parse_add_mc_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int lan_if_opts=0, g_opts=0;
	int ret;

    ppa_memset(pdata, sizeof(*pdata), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 's':
		pdata->mc_add_info.bridging_flag = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if ((pdata->mc_add_info.bridging_flag != 0) &&
			(pdata->mc_add_info.bridging_flag != 1))
			return PPA_CMD_ERR;
		IFX_PPACMD_DBG("addmc  mode: %s\n", pdata->mc_add_info.bridging_flag ? "bridging":"routing" );
		break;

#ifdef PPA_MC_FUTURE_USE
        case 'm':
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg, pdata->mc_add_info.mac);
		IFX_PPACMD_DBG("addmc mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		pdata->mc_add_info.mac[0], pdata->mc_add_info.mac[1], pdata->mc_add_info.mac[2],
		pdata->mc_add_info.mac[3], pdata->mc_add_info.mac[4], pdata->mc_add_info.mac[5]);
		break;
#endif

        case 'g':   /*Support IPv4 and IPv6 */
            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_add_info.mc.mcast_addr.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("MLD GROUP IP: "NIP6_FMT"\n", NIP6(pdata->mc_add_info.mc.mcast_addr.ip.ip6));
                pdata->mc_add_info.mc.mcast_addr.f_ipv6 = 1;
            }
            
            g_opts ++;
            break;

        case 'l':
            if( lan_if_opts > PPA_MAX_MC_IFS_NUM ) return PPA_CMD_ERR; //not to accelerated since too many lan interrace join the same group

            if( lan_if_opts < PPA_MAX_MC_IFS_NUM )
            {
                strncpy_s(pdata->mc_add_info.lan_ifname[lan_if_opts],PPA_IF_NAME_SIZE, popts->optarg, PPA_IF_NAME_SIZE);
                pdata->mc_add_info.lan_ifname[lan_if_opts][PPA_IF_NAME_SIZE-1] = 0;
                IFX_PPACMD_DBG("addmc lan if:%s,lan_if_opts:%d\n", pdata->mc_add_info.lan_ifname[lan_if_opts], lan_if_opts+1 );
                lan_if_opts++;
            }
            break;

        case 'w':
            strncpy_s(pdata->mc_add_info.src_ifname,sizeof(pdata->mc_add_info.src_ifname)-1, popts->optarg, sizeof(pdata->mc_add_info.src_ifname)-1);
            pdata->mc_add_info.src_ifname[sizeof(pdata->mc_add_info.src_ifname)-1] = 0;
            IFX_PPACMD_DBG("addmc wan if:%s\n", pdata->mc_add_info.src_ifname);
            break;

        case 'i': // Src IP, Support IPv4 & IPv6

            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_add_info.mc.source_ip.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("Source IP: "NIP6_FMT"\n", NIP6(pdata->mc_add_info.mc.mcast_addr.ip.ip6));
                pdata->mc_add_info.mc.source_ip.f_ipv6 = 1;
            }
            break;

        case 'f':
            pdata->mc_add_info.mc.SSM_flag = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);;
            IFX_PPACMD_DBG("addmc SSM_flag:%d \n",pdata->mc_add_info.mc.SSM_flag);
            break;

        case 't':     //future only
            pdata->mc_add_info.mc.mc_extra.dslwan_qid_remark = 1;
            pdata->mc_add_info.mc.mc_extra.dslwan_qid = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);
            IFX_PPACMD_DBG("addmc qid:%d (flag=%d)\n",pdata->mc_add_info.mc.mc_extra.dslwan_qid, pdata->mc_add_info.mc.mc_extra.dslwan_qid_remark);
            break;

        default:
            IFX_PPACMD_PRINT("mc_add not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }


    /* Check that match field is not defined more than once and VLAN tag is specified */
    if( g_opts != 1 )  return  PPA_CMD_ERR;

    pdata->mc_add_info.num_ifs = lan_if_opts;


    return PPA_CMD_OK;
}

/* rtp sampling */
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE

static const char ppa_rtp_set_short_opts[] = "g:i:r:"; 
static const struct option ppa_rtp_set_long_opts[] =
{
    {"multicast group",   required_argument,  NULL, 'g'},
    {"source_ip",  required_argument,  NULL, 'i'},
    {"rtp_flag",  required_argument,  NULL, 'r'},
    { 0,0,0,0 }
};


static void ppa_set_rtp_help( int summary)
{
    if( !summary )
    {
        IFX_PPACMD_PRINT("setrtp -g <mc-group> [-i<src-ip>] -r<rtp_flag> ...\n");
    }
    else
    {
        IFX_PPACMD_PRINT("setrtp -g <mc-group> [-i<src-ip>] -r<rtp_flag>\n");
        IFX_PPACMD_PRINT("      option -g: multicast group ip\n");
        IFX_PPACMD_PRINT("      option -i: source ip, this parameter is optional\n");
        IFX_PPACMD_PRINT("      option -r: RTP sequence number update by PPE FW, 1-enable 0-disable\n");
    }
    return;
}

static int ppa_parse_set_rtp_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int lan_if_opts=0, g_opts=0;
	int ret;

    ppa_memset(pdata, sizeof(*pdata), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'g':   /*Support IPv4 and IPv6 */
            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_entry.mcast_addr.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("MLD GROUP IP: "NIP6_FMT"\n", NIP6(pdata->mc_entry.mcast_addr.ip.ip6));
                pdata->mc_entry.mcast_addr.f_ipv6 = 1;
            }
            
            g_opts ++;
            break;

        case 'i': // Src IP, Support IPv4 & IPv6

            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_entry.source_ip.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("Source IP: "NIP6_FMT"\n",NIP6(pdata->mc_entry.source_ip.ip.ip6));
                pdata->mc_entry.source_ip.f_ipv6 = 1;
            }
            break;

        case 'r':
		ret = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (ret == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return ret;
		}
		pdata->mc_entry.RTP_flag = ret;
		IFX_PPACMD_DBG("RTP_flag:%d \n",pdata->mc_entry.RTP_flag);
		break;

        default:
            IFX_PPACMD_PRINT("setrtp does not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( g_opts != 1 )  return  PPA_CMD_ERR;

    return PPA_CMD_OK;
}


#endif

/* mib mode */
#if defined(MIB_MODE_ENABLE)

static const char ppa_mib_mode_short_opts[] = "i:"; 
static void ppa_set_mib_mode_help( int summary)
{
    if( !summary )
    {
        IFX_PPACMD_PRINT("setmibmode -i <mib-mode> ...\n");
    }
    else
    {
        IFX_PPACMD_PRINT("setmibmode -i <mib-mode>\n");
        IFX_PPACMD_PRINT("      option -i: Mib mode, 0-Byte 1-Packet\n");
    }
    return;
}

static int ppa_parse_set_mib_mode(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
	int ret;

    ppa_memset(pdata, sizeof(*pdata), 0 );

    switch(popts->opt)
    {
   
        case 'i':
            pdata->mib_mode_info.mib_mode = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);;
            IFX_PPACMD_DBG("mib_mode:%d \n",pdata->mib_mode_info.mib_mode);
            break;

        default:
            IFX_PPACMD_PRINT("setmibmode does not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
    }


    return PPA_CMD_OK;
}
#endif


/*
====================================================================================
   command:   getmcextra
   description: get multicast extra information, like vlan/dscp
   options:
====================================================================================
*/

static const char    ppa_get_mc_extra_short_opts[] = "g:o:h"; //need to further implement add/remove/modify vlan and enable new dscp and its value
static const struct option ppa_get_mc_extra_long_opts[] =
{
    {"multicast group",   required_argument,  NULL, 'g'},
    {"save-to-file",    required_argument,  NULL, 'o'},

    { 0,0,0,0 }
};
static void ppa_get_mc_extra_help( int summary)
{
    IFX_PPACMD_PRINT("getmcextra -g <multicast group > [-o file ]\n"); // [ -m <multicast-mac-address (for bridging only)>]
    return;
}

static int ppa_parse_get_mc_extra_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int g_opt=0;

    ppa_memset(pdata, sizeof(*pdata), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'g':
            inet_aton(popts->optarg, &pdata->mc_entry.mcast_addr);
            IFX_PPACMD_DBG("getmcextra  group ip: %d.%d.%d.%d\n", NIPQUAD( (pdata->mc_entry.mcast_addr.ip.ip)) );
            g_opt ++;
            break;

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg, PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("getmcextra not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( g_opt != 1)  return PPA_CMD_ERR;

    return PPA_CMD_OK;
}

static void ppa_print_get_mc_extra_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("multicast group %d.%d.%d.%d extra properties:\n", NIPQUAD(pdata->mc_entry.mcast_addr));
        if(pdata->mc_entry. mc_extra.dscp_remark )
            IFX_PPACMD_PRINT("  new_dscp=%04x. ", (unsigned int)pdata->mc_entry.mc_extra.new_dscp );
        else IFX_PPACMD_PRINT("Not new dscp editing. ");

        if(pdata->mc_entry.mc_extra.vlan_insert )
            IFX_PPACMD_PRINT("  New  inner vlan =%04x. ", (unsigned int)(pdata->mc_entry.mc_extra.vlan_prio<<13) + (pdata->mc_entry.mc_extra.vlan_cfi<<12) +  pdata->mc_entry.mc_extra.vlan_id);
        else if(pdata->mc_entry.mc_extra.vlan_remove )
            IFX_PPACMD_PRINT("Remove inner vlan.  ");
        else IFX_PPACMD_PRINT("No inner vlan editing. ");

        if(pdata->mc_entry.mc_extra.out_vlan_insert )
            IFX_PPACMD_PRINT("New outvlan=%04x.  ", (unsigned int)pdata->mc_entry.mc_extra.out_vlan_tag );
        else if(pdata->mc_entry.mc_extra.out_vlan_remove )
            IFX_PPACMD_PRINT("Remove out vlan.  ");
        else IFX_PPACMD_PRINT("No out vlan editing. ");

        if( pdata->mc_entry.mc_extra.dslwan_qid_remark )
            IFX_PPACMD_PRINT("qid: %d\n", pdata->mc_entry.mc_extra.dslwan_qid );

        IFX_PPACMD_PRINT("\n");

    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->mc_entry), sizeof(pdata->mc_entry) );
    }

}


/*
====================================================================================
   command:   getmcnum
   description: get multicast groups count
   options:
====================================================================================
*/

static void ppa_get_mc_count_help(int summary)
{
    IFX_PPACMD_PRINT("getmcnum [-o file ]\n");
    return;
}

static int ppa_parse_get_mc_count_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int res;

    res =   ppa_parse_simple_cmd( popts, pdata );

    if( res != PPA_CMD_OK ) return res;


    pdata->count_info.flag = 0;

    return PPA_CMD_OK;
}
/*
====================================================================================
   command:   getmcgroups
   description: get all multicast groups information
   options:
====================================================================================
*/

static void ppa_get_mc_groups_help( int summary)
{
    IFX_PPACMD_PRINT("getmcgroups -g <multicast group > -s <source ip> -f <ssm flag> [-o file ]\n"); // [ -m <multicast-mac-address (for bridging only)>]
    return;
}

static const char ppa_get_mc_group_short_opts[] = "g:s:f:o"; //need to further implement add/remove/modify vlan and enable new dscp and its value
static const struct option ppa_get_mc_group_long_opts[] =
{
    {"multicast group",   required_argument,  NULL, 'g'},
    {"source_ip",         required_argument,  NULL, 's'},
    {"ssm_flag",          required_argument,  NULL, 'f'},
    {"save-to-file",      required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};

static int ppa_parse_get_mc_group_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int ret;
    ppa_memset(pdata, sizeof(*pdata), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        
        case 'g':   /*Support IPv4 and IPv6 */
            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_entry.mcast_addr.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("MLD GROUP IP: "NIP6_FMT"\n", NIP6(pdata->mc_entry.mcast_addr.ip.ip6));
                pdata->mc_entry.mcast_addr.f_ipv6= 1;
            }
            
            break;

        case 's': // Src IP, Support IPv4 & IPv6

            ret = str_convert(STRING_TYPE_IP, popts->optarg, pdata->mc_entry.source_ip.ip.ip6);
            if(ret == IP_NON_VALID){
                IFX_PPACMD_DBG("Multicast group ip is not a valid IPv4 or IPv6 IP address: %s\n", popts->optarg);
				break;
            }else if(ret == IP_VALID_V6){
                IFX_PPACMD_DBG("Source IP: "NIP6_FMT"\n", NIP6(pdata->mc_entry.mcast_addr.ip.ip6));
                pdata->mc_entry.source_ip.f_ipv6 = 1;
            }
            break;

        case 'f':
            pdata->mc_entry.SSM_flag = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);;
            IFX_PPACMD_DBG("addmc SSM_flag:%d \n",pdata->mc_entry.SSM_flag);
            break;

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg, PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("mc_get_mc_group not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static int ppa_get_mc_groups_cmd (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_MC_GROUPS_INFO *psession_buffer;
    PPA_CMD_DATA cmd_info;
    int res = PPA_CMD_OK, i, j;
	uint32_t size;
    uint32_t flag = PPA_CMD_GET_MC_GROUPS;
    char str_srcip[MAX_STR_IP], str_dstip[MAX_STR_IP];

    //get session count first before malloc memroy
    cmd_info.count_info.flag = 0;
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_MC_GROUP, &cmd_info ) != PPA_CMD_OK )
        return -EIO;

    if( cmd_info.count_info.count == 0 )
    {
        IFX_PPACMD_PRINT("MC groups count=0\n");
        if( g_output_to_file )
            SaveDataToFile(g_output_filename, (char *)(&cmd_info.count_info),  sizeof(cmd_info.count_info)  );
        return PPA_CMD_OK;
    }

    //malloc memory and set value correctly
    size = sizeof(PPA_CMD_COUNT_INFO) + sizeof(PPA_CMD_MC_GROUP_INFO) * ( 1 + cmd_info.count_info.count ) ;
    psession_buffer = (PPA_CMD_MC_GROUPS_INFO *) malloc( size );
    if( psession_buffer == NULL )
    {
        IFX_PPACMD_PRINT("Malloc %d bytes failed\n", size );
        return PPA_CMD_NOT_AVAIL;
    }
    IFX_PPACMD_DBG("Get buffer size=%ul\n", size);
    ppa_memset( psession_buffer, size, 0);

    psession_buffer->count_info.count = cmd_info.count_info.count;
    psession_buffer->count_info.flag = 0;

    IFX_PPACMD_DBG("specified dst ip: %d.%d.%d.%d, src ip: %d.%d.%d.%d \n", 
        NIPQUAD(pdata->mc_entry.mcast_addr.ip.ip),
        NIPQUAD(pdata->mc_entry.source_ip.ip.ip));

    //get session information
    if( (res = ppa_do_ioctl_cmd(flag, psession_buffer ) != PPA_CMD_OK ) )
    {
        free( psession_buffer );
        return res;
    }

    IFX_PPACMD_DBG("MC groups total count=%u. \n", (unsigned int)psession_buffer->count_info.count);

#if defined(MIB_MODE_ENABLE)
            PPA_CMD_MIB_MODE_INFO mode_info;
            char str_mib[12];
            if( ppa_do_ioctl_cmd(PPA_CMD_GET_MIB_MODE, &mode_info) == PPA_CMD_OK )
            {
                if(mode_info.mib_mode == 1)
                        strcpy_s(str_mib,12," packet");
                else
                        strcpy_s(str_mib,12," byte");
            }
            else
            {
                free( psession_buffer );
                return PPA_CMD_ERR;
            }
#endif


    if( !g_output_to_file )
    {
        for(i=0; i<psession_buffer->count_info.count; i++ )
        { 
            if(!is_ip_zero(&pdata->mc_entry.mcast_addr)){
                if(!ip_equal(&pdata->mc_entry.mcast_addr,&psession_buffer->mc_group_list[i].mc.mcast_addr)
                    || (pdata->mc_entry.SSM_flag == 1 
                          && !ip_equal(&pdata->mc_entry.source_ip,&psession_buffer->mc_group_list[i].mc.source_ip)))
                          continue;
            }
            if(!psession_buffer->mc_group_list[i].mc.mcast_addr.f_ipv6){
                    if(psession_buffer->mc_group_list[i].mc.source_ip.ip.ip == 0){
                        snprintf(str_srcip,MAX_STR_IP, "%s", "ANY");
                    }else{
                        snprintf(str_srcip,MAX_STR_IP, NIPQUAD_FMT, NIPQUAD(psession_buffer->mc_group_list[i].mc.source_ip.ip.ip));
                    }
                    //format like: [002] Dst: 239.  2.  2.  3  Src: 1.1.1.1 \n\t (route) qid  0 vlan 0000/04x From  nas0 to  eth0 ADDED_IN_HW|VALID_PPPOE|VALID_NEW_SRC_MAC
                    
#if defined(MIB_MODE_ENABLE)
                    IFX_PPACMD_PRINT("[%03u] MC GROUP:%3u.%3u.%3u.%3u Src IP: %s \n\t (%s) qid(%2u) dscp(%2u) vlan (%04x/%04x) mib (%llu:%llu(cpu:hw mib in%s)) ", i,
#else
                    IFX_PPACMD_PRINT("[%03u] MC GROUP:%3u.%3u.%3u.%3u Src IP: %s \n\t (%s) qid(%2u) dscp(%2u) vlan (%04x/%04x) mib (%llu:%llu) ", i,
#endif
                             NIPQUAD((psession_buffer->mc_group_list[i].mc.mcast_addr.ip.ip)), str_srcip,
                             psession_buffer->mc_group_list[i].bridging_flag ? "bridge":"route ",
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.dslwan_qid,
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.new_dscp,
                             (unsigned int)( psession_buffer->mc_group_list[i].mc.mc_extra.vlan_prio << 13) |( psession_buffer->mc_group_list[i].mc.mc_extra.vlan_cfi << 12) | psession_buffer->mc_group_list[i].mc.mc_extra.vlan_id,
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.out_vlan_tag,
                             (unsigned long long)psession_buffer->mc_group_list[i].mc.host_bytes,
                             (unsigned long long)psession_buffer->mc_group_list[i].mc.hw_bytes
#if defined(MIB_MODE_ENABLE)
                             , str_mib
#endif
                             );

#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
                    if( psession_buffer->mc_group_list[i].mc.RTP_flag == 1)
                    {
                        IFX_PPACMD_PRINT("rtp pkt cnt(%u) rtp seq num(%u) ", 
                             psession_buffer->mc_group_list[i].mc.rtp_pkt_cnt,
                             psession_buffer->mc_group_list[i].mc.rtp_seq_num
                             );
                     }
#endif
            }else{
                if(is_ip_zero(&psession_buffer->mc_group_list[i].mc.source_ip)){
                    snprintf(str_srcip, MAX_STR_IP,"%s", "ANY");
                }else{
                    snprintf(str_srcip,MAX_STR_IP, NIP6_FMT, NIP6(psession_buffer->mc_group_list[i].mc.source_ip.ip.ip6));
                }
                snprintf(str_dstip,MAX_STR_IP, NIP6_FMT, NIP6(psession_buffer->mc_group_list[i].mc.mcast_addr.ip.ip6));

                IFX_PPACMD_PRINT("[%03u] MC GROUP:%s Src IP: %s \n\t (%s) qid(%2u) dscp(%2u) vlan (%04x/%04x) mib (%llu:%llu(host:hw)) ", i,
                             str_dstip, str_srcip,
                             psession_buffer->mc_group_list[i].bridging_flag ? "bridge":"route ",
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.dslwan_qid,
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.new_dscp,
                             (unsigned int)( psession_buffer->mc_group_list[i].mc.mc_extra.vlan_prio << 13) |( psession_buffer->mc_group_list[i].mc.mc_extra.vlan_cfi << 12) | psession_buffer->mc_group_list[i].mc.mc_extra.vlan_id,
                             (unsigned int)psession_buffer->mc_group_list[i].mc.mc_extra.out_vlan_tag,
                             (unsigned long long)psession_buffer->mc_group_list[i].mc.host_bytes,
                             (unsigned long long)psession_buffer->mc_group_list[i].mc.hw_bytes
                             );
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
                    if( psession_buffer->mc_group_list[i].mc.RTP_flag == 1)
                    {
                        IFX_PPACMD_PRINT("rtp pkt cnt(%u) rtp seq num(%u)", 
                             psession_buffer->mc_group_list[i].mc.rtp_pkt_cnt,
                             psession_buffer->mc_group_list[i].mc.rtp_seq_num
                             );
                     }
#endif

                
            }

            if( strnlen_s(psession_buffer->mc_group_list[i].src_ifname,PPA_IF_NAME_SIZE ) == 0 )
                IFX_PPACMD_PRINT("From N/A ");
            else
            {
                IFX_PPACMD_PRINT("From  %s ", psession_buffer->mc_group_list[i].src_ifname);
            }

            if( psession_buffer->mc_group_list[i].num_ifs ==0 ||psession_buffer->mc_group_list[i].lan_ifname[0] == 0 )
                IFX_PPACMD_PRINT("to N/A");
            else
            {
                IFX_PPACMD_PRINT("to  ");
                for(j=0; j<psession_buffer->mc_group_list[i].num_ifs; j++)
                {
                    if( j == 0 )
                        IFX_PPACMD_PRINT("%s", psession_buffer->mc_group_list[i].lan_ifname[j]);
                    else
                        IFX_PPACMD_PRINT("/%s", psession_buffer->mc_group_list[i].lan_ifname[j]);
                }
            }
            IFX_PPACMD_PRINT("\n\t ");
            print_session_flags( psession_buffer->mc_group_list[i].mc.flags );
            IFX_PPACMD_PRINT("\n");
        }
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(psession_buffer),  size  );
    }

    free( psession_buffer );
    return PPA_CMD_OK;
}


/*
====================================================================================
   command:   setmcextra
   description: set multicast extra information, like vlan/dscp
   options:
====================================================================================
*/

static const char    ppa_set_mc_extra_short_opts[] = "g:q:i:o:t:h"; //need to further implement add/remove/modify vlan and enable new dscp and its value
static const struct option ppa_set_mc_extra_long_opts[] =
{
    {"multicast group",   required_argument,  NULL, 'g'},
    {"dscp",   required_argument,  NULL, 'q'},
    {"inner vlan",   required_argument,  NULL, 'i'},
    {"outer vlan",   required_argument,  NULL, 'o'},
    {"qid",   required_argument,  NULL, 't'},

    { 0,0,0,0 }
};
static void ppa_set_mc_extra_help( int summary)
{
    if( !summary )
    {
        IFX_PPACMD_PRINT("setmcextra -g <multicast group> [-q <new dscp_value>] [-i <insert:<vlan_tag>|remove>] ...\n");
    }
    else
    {
        IFX_PPACMD_PRINT("setmcextra -g <multicast group> [-q <new dscp_value>] [-i <insert:<vlan_tag>|remove>] [-o <insert:<vlan_tag>|remove] \n");
        IFX_PPACMD_PRINT("      dscp: 4 bits ( Only for A5/D5)\n");
        IFX_PPACMD_PRINT("      vlan_tag:  16 bits: priority(3 bits), cfi(1 bit) and vlan id (10bits) \n");
    }
    return;
}

static int ppa_parse_set_mc_extra_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int g_opt=0, q_opt=0, inner_vlan_opt=0, out_vlan_opt=0;
    char *insert_format="insert:";
    char *remove_format="remove";
    uint32_t vlanid ;

    ppa_memset(pdata, sizeof(*pdata), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'g': //multicast group
            inet_aton(popts->optarg, ( struct in_addr *)&pdata->mc_entry.mcast_addr);
            g_opt ++;
            break;

        case 'q': //dscp
            pdata->mc_entry.mc_extra.new_dscp = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->mc_entry.mc_extra.dscp_remark = 1;
            pdata->mc_entry.flags |= PPA_F_SESSION_NEW_DSCP;
            q_opt ++;
            break;

        case 'i': //inner vlan
            if( strncmp( popts->optarg, insert_format, strlen(insert_format) ) == 0 )
            {
                char *p = popts->optarg +strlen(insert_format);
                vlanid = str_convert(STRING_TYPE_INTEGER,  p, NULL);
                if( vlanid == 0 ) return PPA_CMD_ERR;

                pdata->mc_entry.mc_extra.vlan_insert= 1;
                pdata->mc_entry.mc_extra.vlan_prio = ( vlanid  >> 13 ) & 7;
                pdata->mc_entry.mc_extra.vlan_cfi = ( vlanid  >> 12 ) & 1;
                pdata->mc_entry.mc_extra.vlan_id = vlanid & 0xFFF;
            }
            else if( strncmp( popts->optarg, remove_format, strlen(remove_format) ) == 0 )
            {
                pdata->mc_entry.mc_extra.vlan_remove = 1;
            }
            inner_vlan_opt ++;
            pdata->mc_entry.flags |= PPA_F_SESSION_VLAN;
            break;

        case 'o': //outer vlan
            if( strncmp( popts->optarg, insert_format, strlen(insert_format) ) == 0 )
            {
                char *p = popts->optarg +strlen(insert_format);
                vlanid = str_convert(STRING_TYPE_INTEGER,  p, NULL);
                if( vlanid == 0 ) return PPA_CMD_ERR;

                pdata->mc_entry.mc_extra.out_vlan_insert= 1;
                pdata->mc_entry.mc_extra.out_vlan_tag = vlanid;

            }
            else if( strncmp( popts->optarg, remove_format, strlen(remove_format) ) == 0 )
            {
                pdata->mc_entry.mc_extra.out_vlan_remove = 1;
            }
            out_vlan_opt ++;
            pdata->mc_entry.flags |= PPA_F_SESSION_OUT_VLAN;
            break;

        case 't':   //future only
        {
            pdata->mc_entry.mc_extra.dslwan_qid_remark = 1;
            pdata->mc_entry.mc_extra.dslwan_qid = str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);
        }

        default:
            IFX_PPACMD_PRINT("set_mc_extra not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( g_opt != 1 || inner_vlan_opt >1 || out_vlan_opt > 1 )
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("setmcextra: %d.%d.%d.%d\n", NIPQUAD( (pdata->mc_entry.mcast_addr.ip.ip)) );

    if( pdata->mc_entry.mc_extra.dscp_remark )
        IFX_PPACMD_DBG("  New dscp: 0x%x\n", pdata->mc_entry.mc_extra.new_dscp );

    if( pdata->mc_entry.mc_extra.vlan_insert )
    {
        vlanid = ( pdata->mc_entry.mc_extra.vlan_prio<< 13 ) + (pdata->mc_entry.mc_extra.vlan_cfi  << 12) + pdata->mc_entry.mc_extra.vlan_id;
        IFX_PPACMD_DBG("  New inner vlan: %04x\n", (unsigned int)vlanid );
    }
    else if( pdata->mc_entry.mc_extra.vlan_remove)
    {
        IFX_PPACMD_DBG("  Remove inner vlan. \n");
    }


    if( pdata->mc_entry.mc_extra.out_vlan_insert )
    {
        IFX_PPACMD_DBG("  New outer vlan: %04x\n", (unsigned int)pdata->mc_entry.mc_extra.out_vlan_tag);
    }
    else if( pdata->mc_entry.mc_extra.out_vlan_remove)
    {
        IFX_PPACMD_DBG("  Remove outer vlan. \n");
    }

    if( pdata->mc_entry.mc_extra.dslwan_qid_remark )
        IFX_PPACMD_DBG("  New dslwan_qid:%0u\n", (unsigned int)pdata->mc_entry.mc_extra.dslwan_qid);

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   getlansessionum/getwansessionnum
   description: get LAN/WAN session count
   options:
====================================================================================
*/
static void ppa_get_lan_session_count_help( int summary)
{
    IFX_PPACMD_PRINT("getlansessionnum [-w | -s | n | a ] [-o file ]\n");
    
    if( summary )
    {
        IFX_PPACMD_PRINT("  -w: all ppe accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -s: all software accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -n: all non-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -a: all sessions in PPA\n");
    }
    return;
}

static void ppa_get_wan_session_count_help( int summary)
{
    IFX_PPACMD_PRINT("getwansessionnum [-w | -s | n | a ] [-o file ]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("  -w: all ppe accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -s: all software accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -n: all non-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -a: all sessions in PPA\n");
    }
    return;
}

static const char ppa_get_simple_short_opts[] = "o:ah";
static const struct option ppa_get_simple_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    {"all",   no_argument,  NULL, 'a'},
    { 0,0,0,0 }
};

/*this is a simple template parse command. At most there is only one parameter for saving result to file */
static int ppa_parse_simple_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg, PPACMD_MAX_FILENAME);
            break;

        case 'a':
            g_all = 1;
            break;

        case 'w': //only for getlan/wassessionnum
            pdata->session_info.count_info.flag = SESSION_ADDED_IN_HW;
            break;
        
        case 'n': //only for getlan/wassessionnum
            pdata->session_info.count_info.flag = SESSION_NON_ACCE_MASK;
            break;
        
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static int ppa_parse_get_lan_session_count(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{

    pdata->count_info.flag =0;
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'w':
            pdata->count_info.flag = SESSION_ADDED_IN_HW;
            break;

        case 's':
            pdata->count_info.flag = SESSION_ADDED_IN_SW;
            break;

        case 'n':
            pdata->count_info.flag = SESSION_NON_ACCE_MASK;
            break;       

        case 'a':
            pdata->count_info.flag = 0;
            break;

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}


static int ppa_parse_get_wan_session_count(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{

    pdata->count_info.flag =0;
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'w':
            pdata->count_info.flag = SESSION_ADDED_IN_HW;
            break;
        
	case 's':
            pdata->count_info.flag = SESSION_ADDED_IN_SW;
            break;

        case 'n':
            pdata->count_info.flag = SESSION_NON_ACCE_MASK;
            break;       

        case 'a':
            pdata->count_info.flag = 0;
            break;

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}


/*
====================================================================================
   command:   getlansessions/getwansessions
   description: get LAN/WAN all session detail information
   options:
====================================================================================
*/

static const char ppa_get_session_short_opts[] = "wsnafd:o:h";
static void ppa_get_lan_sessions_help( int summary)
{
    IFX_PPACMD_PRINT("getlansessions [-w] [-s] [-a ] [-n ] [ -f ] [-o file ] [-d delay]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("  -w: all ppe-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -s: all software-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -n: all non-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -a: all sessions in PPA\n");
        IFX_PPACMD_PRINT("  -f: Set SESSION_BYTE_STAMPING flag for testing purpose\n");
        IFX_PPACMD_PRINT("  -d: sleep time in seconds. For testing purpose\n");
    }
    return;
}

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static const char ppa_get_session_criteria_short_opts[] = "a";
static const char ppa_swap_sessions_short_opts[] = "a";
#endif
static const char ppa_get_session_count_short_opts[] = "wsnao:h";
static int delay_in_second =0;

static void ppa_get_wan_sessions_help( int summary)
{
    IFX_PPACMD_PRINT("getwansessions [-w] [-s] [-a ] [-n ] [ -f ] [-o file ] [-d delay]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("  -w: all ppe-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -s: all software-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -n: all non-accelerated sessions in PPA\n");
        IFX_PPACMD_PRINT("  -a: all sessions in PPA\n");
        IFX_PPACMD_PRINT("  -f: Set SESSION_BYTE_STAMPING flag for testing purpose\n");
        IFX_PPACMD_PRINT(" -d: sleep time in seconds. For testing purpose\n");
    }
    return;
}

static int ppa_parse_get_session_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    pdata->session_info.count_info.flag = 0;  //default get all lan or wan sessions

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'w':
            pdata->session_info.count_info.flag = SESSION_ADDED_IN_HW;
            break;
        
	case 's':
            pdata->session_info.count_info.flag = SESSION_ADDED_IN_SW;
            break;

        case 'n':
            pdata->session_info.count_info.flag = SESSION_NON_ACCE_MASK;
            break;

        case 'a':
            pdata->session_info.count_info.flag = 0;
            break;

        case 'f':
            pdata->session_info.count_info.stamp_flag |= SESSION_BYTE_STAMPING;
            break;

       case 'd':
            delay_in_second= str_convert(STRING_TYPE_INTEGER,  popts->optarg, NULL);
            break;     

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static PPA_CMD_SESSIONS_INFO *ppa_get_sessions_malloc (uint32_t session_flag, uint32_t flag, uint32_t stamp_flag, uint32_t hash_index)
{
    PPA_CMD_SESSIONS_INFO *psession_buffer;
    PPA_CMD_DATA cmd_info;
    int res = PPA_CMD_OK, size;

    ppa_memset( &cmd_info, sizeof(cmd_info),0 );
    cmd_info.count_info.flag = session_flag;
    cmd_info.count_info.stamp_flag = 0;
    cmd_info.count_info.hash_index = hash_index;

    if( flag == PPA_CMD_GET_LAN_SESSIONS )
    {
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_LAN_SESSION, &cmd_info ) != PPA_CMD_OK )
            return NULL;
    }
    else if( flag == PPA_CMD_GET_WAN_SESSIONS )
    {
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_WAN_SESSION, &cmd_info ) != PPA_CMD_OK )
            return NULL;
    }    
    else if( flag == PPA_CMD_GET_LAN_WAN_SESSIONS )
    {
        IFX_PPACMD_DBG("PPA_CMD_GET_LAN_WAN_SESSIONS\n");
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_LAN_WAN_SESSION, &cmd_info ) != PPA_CMD_OK )
            return NULL;
    }
    
    if( cmd_info.count_info.count == 0 )
    {
        IFX_PPACMD_DBG("session count=0. \n");
        if( g_output_to_file )
            SaveDataToFile(g_output_filename, (char *)(&cmd_info.count_info),  sizeof(cmd_info.count_info)  );
        return NULL;
    }

    //malloc memory and set value correctly
    size = sizeof(PPA_CMD_SESSIONS_INFO) + sizeof(PPA_CMD_SESSION_ENTRY) * ( cmd_info.count_info.count + 1 );
    psession_buffer = (PPA_CMD_SESSIONS_INFO *) malloc( size );
    if( psession_buffer == NULL )
    {
        IFX_PPACMD_PRINT("ppa_get_sessions_malloc : Memory allocation Fail for psession_buffer\n" );
        return NULL;
    }
    
    ppa_memset( psession_buffer, sizeof(size), 0 );

    psession_buffer->count_info.count = cmd_info.count_info.count;
    psession_buffer->count_info.flag = session_flag;
    psession_buffer->count_info.stamp_flag = stamp_flag;
    psession_buffer->count_info.hash_index = hash_index;
    //get session information
    if( delay_in_second ) sleep(delay_in_second);
    if( (res = ppa_do_ioctl_cmd(flag, psession_buffer ) != PPA_CMD_OK ) )
    {
        free( psession_buffer );
        IFX_PPACMD_PRINT("ppa_get_sessions_malloc failed for ioctl not succeed.\n");
        return NULL;
    }

    
    return psession_buffer;
}


static int ppa_get_sessions (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata, uint32_t flag)
{
    PPA_CMD_SESSIONS_INFO *psession_buffer;
    int i=0, size=0;
    unsigned int hash_index = 0;
    unsigned int session_no=0;

    for(hash_index=0; hash_index<SESSION_LIST_HASH_TABLE_SIZE; hash_index++)
    {
        psession_buffer = ppa_get_sessions_malloc(pdata->session_info.count_info.flag, flag, pdata->session_info.count_info.stamp_flag, hash_index+1);
        if( !psession_buffer )
        {
            continue;
        }
        
        if( !g_output_to_file )
        {
            IFX_PPACMD_PRINT("\nSession Information: %5u in PPA Hash[%02u] -------\n", (unsigned int)psession_buffer->count_info.count, hash_index );
#if defined(MIB_MODE_ENABLE)
            PPA_CMD_MIB_MODE_INFO mode_info;
            char str_mib[12];
            if( ppa_do_ioctl_cmd(PPA_CMD_GET_MIB_MODE, &mode_info) == PPA_CMD_OK )
            {
                if(mode_info.mib_mode == 1)
                        strcpy_s(str_mib,12," packet");
                else
                        strcpy_s(str_mib,12," byte");
            }
            else
            {
                free( psession_buffer );
                return PPA_CMD_ERR;
            }
#endif /* MIB_MODE_ENABLE */

            for(i=0; i<psession_buffer->count_info.count; i++ )
            {
                //print format: <packet index> <packet-type>  <rx interface name> (source ip : port) -> <tx interface name> ( dst_ip : dst_port ) nat ( nat_ip: nat_port )
                IFX_PPACMD_PRINT("[%03d]", i + session_no);

                if( psession_buffer->session_list[i].ip_proto == 0x11 ) //UDP
                    IFX_PPACMD_PRINT("udp");
                else if( psession_buffer->session_list[i].ip_proto == 6)
                    IFX_PPACMD_PRINT("tcp");
                else
                    IFX_PPACMD_PRINT("---");
                if( psession_buffer->session_list[i].flags & SESSION_IS_IPV6 )

#if defined(MIB_MODE_ENABLE)
			IFX_PPACMD_PRINT(": %8s (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) -> %8s (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) NAT (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) (%llu:%llu(hw mib in %s )) @session0x%08x with coll %d pri %d\n",
					psession_buffer->session_list[i].rx_if_name,
					NIP6(psession_buffer->session_list[i].src_ip.ip6),
					psession_buffer->session_list[i].src_port,
					psession_buffer->session_list[i].tx_if_name,
					NIP6(psession_buffer->session_list[i].dst_ip.ip6),
					psession_buffer->session_list[i].dst_port,
					NIP6(psession_buffer->session_list[i].natip.ip6),
					psession_buffer->session_list[i].nat_port,
					psession_buffer->session_list[i].host_bytes,
					psession_buffer->session_list[i].hw_bytes,str_mib,
					(unsigned int)psession_buffer->session_list[i].session,
					psession_buffer->session_list[i].collision_flag,
					psession_buffer->session_list[i].priority);


#else
			IFX_PPACMD_PRINT(": %8s (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) -> %8s (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) NAT (%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d) (%llu:%llu(hw)) @session0x%08x with coll %d pri %d\n",
					psession_buffer->session_list[i].rx_if_name,
					NIP6(psession_buffer->session_list[i].src_ip.ip6),
					psession_buffer->session_list[i].src_port,
					psession_buffer->session_list[i].tx_if_name,
					NIP6(psession_buffer->session_list[i].dst_ip.ip6),
					psession_buffer->session_list[i].dst_port,
					NIP6(psession_buffer->session_list[i].natip.ip6), psession_buffer->session_list[i].nat_port,
					psession_buffer->session_list[i].host_bytes,
					psession_buffer->session_list[i].hw_bytes,
					(unsigned int)psession_buffer->session_list[i].session,
					psession_buffer->session_list[i].collision_flag,
					psession_buffer->session_list[i].priority);
		else
#endif

#if defined(MIB_MODE_ENABLE)
			IFX_PPACMD_PRINT(": %8s (%3d.%3d.%3d.%3d/%5d) -> %8s (%3d.%3d.%3d.%3d/%5d) NAT (%3d.%3d.%3d.%3d/%5d) (%llu:%llu(hw mib in %s))  @session0x%08x with coll %d pri %d\n",
					psession_buffer->session_list[i].rx_if_name,
					NIPQUAD(psession_buffer->session_list[i].src_ip),
					psession_buffer->session_list[i].src_port,
					psession_buffer->session_list[i].tx_if_name,
					NIPQUAD(psession_buffer->session_list[i].dst_ip),
					psession_buffer->session_list[i].dst_port,
					NIPQUAD(psession_buffer->session_list[i].natip),
					psession_buffer->session_list[i].nat_port,
					psession_buffer->session_list[i].host_bytes,
					psession_buffer->session_list[i].hw_bytes,str_mib,
					(unsigned int)psession_buffer->session_list[i].session,
					psession_buffer->session_list[i].collision_flag,
					psession_buffer->session_list[i].priority);

#else
			IFX_PPACMD_PRINT(": %8s (%3d.%3d.%3d.%3d/%5d) -> %8s (%3d.%3d.%3d.%3d/%5d) NAT (%3d.%3d.%3d.%3d/%5d) (%llu:%llu(hw))  @session0x%08x with coll %d pri %d\n",
					psession_buffer->session_list[i].rx_if_name,
					NIPQUAD(psession_buffer->session_list[i].src_ip),
					psession_buffer->session_list[i].src_port,
					psession_buffer->session_list[i].tx_if_name,
					NIPQUAD(psession_buffer->session_list[i].dst_ip),
					psession_buffer->session_list[i].dst_port,
					NIPQUAD(psession_buffer->session_list[i].natip),
					psession_buffer->session_list[i].nat_port,
					psession_buffer->session_list[i].host_bytes,
					psession_buffer->session_list[i].hw_bytes,
					(unsigned int)psession_buffer->session_list[i].session,
					psession_buffer->session_list[i].collision_flag,
					psession_buffer->session_list[i].priority);

#endif
            }
        }
        else
        {
            size = psession_buffer->count_info.count * sizeof(PPA_CMD_SESSIONS_INFO);
            SaveDataToFile(g_output_filename, (char *)(psession_buffer),  size  );
        }

        free( psession_buffer );
    }
    return PPA_CMD_OK;
}



static int ppa_get_lan_sessions_cmd (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    return ppa_get_sessions(pcmd, pdata, PPA_CMD_GET_LAN_SESSIONS);
}

static int ppa_get_wan_sessions_cmd (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    return ppa_get_sessions(pcmd, pdata, PPA_CMD_GET_WAN_SESSIONS);
}

#define MAX_SESSION_HASH_TABLE 2
typedef struct PPA_SESSION_STATISTIC_ITEM_LIST
{
    int32_t in_hash_num;
    int32_t acc_in_hash_num;  /* accelerated session number which put in the hash index's bucket */
    int32_t acc_in_collision_num;/* accelerated session number which have same hash index, but sit in collision table */
    int32_t not_acc_for_mgm_num; /* not accelerated session number for session managemnet purpuse */
    int32_t add_hw_fail_num;
    int32_t waiting_acc_sessions_num;
    struct list_head  add_hw_fail_list_head;
    int32_t other_not_acc_num; /* non_accelerated session number with unknow reasons */
    struct list_head  other_not_acc_list_head;
}PPA_SESSION_STATISTIC_ITEM_LIST;

typedef struct PPA_SESSION_STATISTIC
{
    int32_t hash_table_offset[MAX_SESSION_HASH_TABLE];
    int32_t hash_table_entries_size[MAX_SESSION_HASH_TABLE];
    int32_t max_table_hash_index_num[MAX_SESSION_HASH_TABLE];
    int32_t max_hash_bucket_per_index[MAX_SESSION_HASH_TABLE];
    int32_t max_collision[MAX_SESSION_HASH_TABLE];
    int32_t table_items_num;  //includes all tables items
    int32_t table_num;

    int32_t in_hash_num ;
    int32_t acc_in_hash_num ;
    int32_t waiting_session ;
    int32_t acc_in_collision_num ;
    int32_t add_hw_fail_num ;
    int32_t other_not_acc_num ;
    int32_t free_hash ; 
    uint32_t free_in_collision;

    int32_t valid_hash_num;
    int32_t failed_hash_num; //the session cannot be accelerated now for special reason, like cannot get its mac address and so on
    struct list_head  failed_hash_list_head;
    int32_t non_accelerable_sess_num; //canot acclerate session at all for PPA limitations
    struct list_head  non_accelerable_sess_list_head;
    int32_t not_acc_for_mgm_num;  //controlled by session management
    struct list_head  not_acc_for_mgm_list_head;


    PPA_SESSION_STATISTIC_ITEM_LIST table[1]; //dummy size. shared by all PPE FW session tables (LAN/WAN)
  
}PPA_SESSION_STATISTIC;


typedef struct PPA_SESSION_STATIS_ENTRY_INFO
{
    struct list_head hlist;
    void *session_addr;
}PPA_SESSION_STATIS_ENTRY_INFO;

static void ppa_free_session_mgm_list(struct list_head *hlist_head)
{       
    struct list_head *pos, *q;
    PPA_SESSION_STATIS_ENTRY_INFO *item;

    list_for_each_safe(pos, q, hlist_head)
    {
         item = list_entry(pos, struct PPA_SESSION_STATIS_ENTRY_INFO, hlist);
         list_del(pos);
         free(item);
    }
}

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static int ppa_swap_sessions(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
}
static int ppa_get_session_criteria(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
	PPA_CMD_SESSIONS_CRITERIA_INFO *session_criteria_info=NULL;
    	int res = PPA_CMD_OK;
    	PPA_CMD_DATA cmd_info;
	if( ppa_do_ioctl_cmd(PPA_CMD_GET_SESSIONS_CRITERIA, &cmd_info) != PPA_CMD_OK ) {
        	IFX_PPACMD_PRINT("PPA_CMD_GET_SESSIONS_CRITERIA failed\n");
        	return PPA_CMD_ERR;
    	}
    IFX_PPACMD_PRINT("low_prio_rate=%d def_prio_rate=%d low_prio_thresh=%d def_prio_thresh=%d\n",cmd_info.session_criteria_info.ppa_low_prio_data_rate,cmd_info.session_criteria_info.ppa_def_prio_data_rate,cmd_info.session_criteria_info.ppa_low_prio_thresh,cmd_info.session_criteria_info.ppa_def_prio_thresh);

    free(session_criteria_info);
    return PPA_CMD_OK;
}
#endif
static int display_non_lan_wan_session_num = 0;
static int ppa_acceleration_session_hash_summary()
{
    PPA_CMD_SESSION_ENTRY *pp_item=NULL;
    PPA_SESSION_STATISTIC *session_statistics = NULL;
    int32_t index;
    PPA_SESSION_STATIS_ENTRY_INFO *item=NULL;
    int32_t i,j, hash_index;
    uint32_t buf_size;
    PPA_CMD_SESSIONS_INFO *sessions=NULL;
    PPA_CMD_MAX_ENTRY_INFO max_entries={0};

    //Get PPE HASH property
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_MAX_ENTRY, &max_entries) != PPA_CMD_OK )
    {
        IFX_PPACMD_PRINT("PPA_CMD_GET_MAX_ENTRY failed\n");
        return PPA_CMD_ERR;
    }   
    IFX_PPACMD_DBG("max_lan_entries=%u\n", max_entries.entries.max_lan_entries);
    IFX_PPACMD_DBG("max_wan_entries=%u\n", max_entries.entries.max_wan_entries);
    IFX_PPACMD_DBG("max_lan_collision_entries=%u\n", max_entries.entries.max_lan_collision_entries);
    IFX_PPACMD_DBG("max_wan_collision_entries=%u\n", max_entries.entries.max_wan_collision_entries);
    IFX_PPACMD_DBG("session_hash_table_num=%u\n", max_entries.entries.session_hash_table_num);
    IFX_PPACMD_DBG("max_lan_hash_index_num=%u\n", max_entries.entries.max_lan_hash_index_num);
    IFX_PPACMD_DBG("max_wan_hash_index_num=%u\n", max_entries.entries.max_wan_hash_index_num);
    IFX_PPACMD_DBG("max_lan_hash_bucket_num=%u\n", max_entries.entries.max_lan_hash_bucket_num);
    IFX_PPACMD_DBG("max_wan_hash_bucket_num=%u\n", max_entries.entries.max_wan_hash_bucket_num);
    if( max_entries.entries.session_hash_table_num > MAX_SESSION_HASH_TABLE )
    {
        IFX_PPACMD_DBG("wrong session_hash_table_num %d\n", max_entries.entries.session_hash_table_num);
        goto END;
    }
    buf_size = sizeof(PPA_SESSION_STATISTIC) + sizeof(PPA_SESSION_STATISTIC_ITEM_LIST) * ( max_entries.entries.max_lan_hash_index_num + max_entries.entries.max_wan_hash_index_num) ;
    session_statistics = malloc( buf_size );
    if( !session_statistics ) 
    {
        IFX_PPACMD_PRINT( "ppa_malloc fail to allocat memory of %d bytes\n", buf_size);
        goto END;
    }
    IFX_PPACMD_DBG("Get session_statistics buffer buf_size=%d with hash index %d/%d support\n", buf_size, max_entries.entries.max_lan_hash_index_num , max_entries.entries.max_wan_hash_index_num );
    ppa_memset( session_statistics, buf_size, 0 );
   
    session_statistics->table_num = max_entries.entries.session_hash_table_num;
    for(i=0; i<session_statistics->table_num; i++) 
    {
        if( i == 0 )
        {
            session_statistics->max_table_hash_index_num[i] = max_entries.entries.max_lan_hash_index_num;
            session_statistics->max_hash_bucket_per_index[i] = max_entries.entries.max_lan_hash_bucket_num;
            session_statistics->max_collision[i] = max_entries.entries.max_lan_collision_entries;
            session_statistics->table_items_num += max_entries.entries.max_lan_entries;
            session_statistics->hash_table_offset[i] = 0;
        }
        else if( i == 1 )
        {
            session_statistics->max_table_hash_index_num[i] = max_entries.entries.max_wan_hash_index_num;            
            session_statistics->max_hash_bucket_per_index[i] = max_entries.entries.max_wan_hash_bucket_num;            
            session_statistics->max_collision[i] = max_entries.entries.max_wan_collision_entries;
            session_statistics->table_items_num += max_entries.entries.max_wan_entries;
            session_statistics->hash_table_offset[i] = session_statistics->max_table_hash_index_num[0];
        }
    }     
  
    IFX_PPACMD_PRINT(" Table  #Index   #Bucket  #Collision  Total\n");
    for(i=0; i<session_statistics->table_num; i++)
    {
        char table_flag[10]="     "; //for future GRX500
        
        if( session_statistics->table_num > 1 )
        {
            if(i== 0 ) 
                strcpy_s(table_flag,10, "(LAN)");
            else if (i== 1 ) 
                strcpy_s(table_flag,10, "(WAN)"); 
        }
        IFX_PPACMD_PRINT(" %d%s %4d     %4d      %4d      %4d(%d + %d)\n", 
            i, table_flag,
            session_statistics->max_table_hash_index_num[i], 
            session_statistics->max_hash_bucket_per_index[i], 
            session_statistics->max_collision[i],
            session_statistics->max_table_hash_index_num[i] * session_statistics->max_hash_bucket_per_index[i] + session_statistics->max_collision[i],
            session_statistics->max_table_hash_index_num[i] * session_statistics->max_hash_bucket_per_index[i], 
            session_statistics->max_collision[i]
            );
    }    
    //initialize list
    IFX_PPACMD_DBG( "initialize list\n");
    INIT_LIST_HEAD(&session_statistics->failed_hash_list_head);
    INIT_LIST_HEAD(&session_statistics->non_accelerable_sess_list_head);
    INIT_LIST_HEAD(&session_statistics->not_acc_for_mgm_list_head);
    for(i=0; i<session_statistics->table_num; i++) 
    {
        int table_offset = session_statistics->hash_table_offset[i];
       
        for(j=0; j<session_statistics->max_table_hash_index_num[i]; j++ )
        {
            INIT_LIST_HEAD(&session_statistics->table[table_offset + j].add_hw_fail_list_head);
            INIT_LIST_HEAD(&session_statistics->table[table_offset + j].other_not_acc_list_head);
        }
    }
    IFX_PPACMD_DBG( "initialize done\n");

    for(hash_index=0; hash_index<SESSION_LIST_HASH_TABLE_SIZE; hash_index++ )
    {
        sessions = ppa_get_sessions_malloc(0, PPA_CMD_GET_LAN_WAN_SESSIONS, 0, hash_index+1);

        IFX_PPACMD_DBG( "Get session list: %d\n", sessions ? sessions->count_info.count: 0);
        //loop the session list
        if( sessions ) 
        {
            for( i=0; i<sessions->count_info.count; i ++)
            {
                if( !item )
                {
                    item = malloc(sizeof(PPA_SESSION_STATIS_ENTRY_INFO) );
                    if( !item ) 
                    {
                        IFX_PPACMD_DBG("ppa_malloc fail\n");
                        goto END;
                    }
                    INIT_LIST_HEAD(&item->hlist);
                }

                pp_item = &sessions->session_list[i];        
                
                if( pp_item->flags & SESSION_NOT_ACCELABLE )
                {  //this session cannot be accelerated at all, for example, hairping NAT, CPE local session,...
                    session_statistics->non_accelerable_sess_num ++;                
                    item->session_addr = (void *)pp_item;
                    list_add(&item->hlist,  &session_statistics->non_accelerable_sess_list_head);
                    item = NULL;
                    continue;
                }
                
                if( ! pp_item->flag2 & SESSION_FLAG2_HASH_INDEX_DONE )
                {  //not valid hash, for example for IPV6 session, if IP6 table full already, then new IP6 cannot be added and this sessinon cannot get its hash index at all
                    session_statistics->failed_hash_num ++;                                
                    item->session_addr = (void *)pp_item; 
                    list_add(&item->hlist,  &session_statistics->failed_hash_list_head);
                    item = NULL;
                    continue;
                }

                //check tash index validility
                if( (pp_item->hash_table_id >= MAX_SESSION_HASH_TABLE) && (pp_item->hash_table_id >= session_statistics->table_num) )
                {
                    IFX_PPACMD_DBG("wrong hash_table_id(%d) for session 0x%p\n", pp_item->hash_table_id, pp_item);
                    goto END;
                }
                if( pp_item->hash_index >= session_statistics->max_table_hash_index_num[pp_item->hash_table_id])
                {
                    IFX_PPACMD_DBG("wrong hash_index(%d) for session 0x%p. It should less than %d\n", 
                        pp_item->hash_index, 
                        pp_item, 
                        session_statistics->max_table_hash_index_num[pp_item->hash_table_id]);
                    goto END;
                }            
                index = session_statistics->hash_table_offset[pp_item->hash_table_id] + pp_item->hash_index;
                if( index >= session_statistics->table_items_num )
                {
                    IFX_PPACMD_DBG("wrong index(%d=%d + %d) for session 0x%p\n", pp_item->hash_table_id,
                            session_statistics->hash_table_offset[pp_item->hash_table_id],
                            pp_item->hash_index,
                            pp_item);
                    goto END;
                }

                session_statistics->table[index].in_hash_num ++;
                
                if( pp_item->flags & SESSION_NOT_ACCEL_FOR_MGM )
                {  
                    session_statistics->table[index].not_acc_for_mgm_num ++;
                    item->session_addr = (void *)pp_item;                
                    list_add(&item->hlist,  &session_statistics->not_acc_for_mgm_list_head);
                    item = NULL;
                    continue;
                }            
                if ( pp_item->flags & SESSION_ADDED_IN_HW )
                {                          
                    if( pp_item->collision_flag ) 
                        session_statistics->table[index].acc_in_collision_num ++;
                    else
                        session_statistics->table[index].acc_in_hash_num ++;

                    continue;
                }
            
                //can be accelerated, but not in acceleration path
                session_statistics->table[index].waiting_acc_sessions_num ++;
                
                if( pp_item->flag2 & SESSION_FLAG2_ADD_HW_FAIL )
                {                
                    session_statistics->table[index].add_hw_fail_num ++;    
                    
                    item->session_addr = (void *)pp_item;
                    list_add(&item->hlist,  &session_statistics->table[index].add_hw_fail_list_head);
                    item = NULL;
                    IFX_PPACMD_DBG("session_statistics->table[%d].add_hw_fail_num=%d", index, session_statistics->table[index].add_hw_fail_num);
                    continue;
                }

                //other non-acceleation reason, like mac address fail or interface not added into ppa yet.
                session_statistics->table[index].other_not_acc_num ++;                
                item->session_addr = (void *)pp_item;
                list_add(&item->hlist,  &session_statistics->table[index].other_not_acc_list_head);
                item = NULL;           
            }
        }
        if( sessions ) 
        {
            free(sessions);
            sessions = NULL;
        }
    }

    for(i=0; i<session_statistics->table_num; i++)
    {
        int table_offset = session_statistics->hash_table_offset[i];
        int collision_num = 0;

        int in_hash_num = 0;
        int acc_in_hash_num = 0;
        int waiting_session = 0;
        int acc_in_collision_num = 0;
        int add_hw_fail_num = 0;
        int not_acc_for_mgm_num = 0;
        int other_not_acc_num = 0;
        int free_hash = 0;
        
        IFX_PPACMD_PRINT("\nTable %d\n", i);
        IFX_PPACMD_PRINT("   Index Sessions  hash_usage  coll_usage  sess_fail sess_waiting others_fail  free\n");
        for(j=0; j<session_statistics->max_table_hash_index_num[i]; j++ )
        {
            in_hash_num += session_statistics->table[table_offset+j].in_hash_num;
            acc_in_hash_num += session_statistics->table[table_offset+j].acc_in_hash_num;
            waiting_session += session_statistics->table[table_offset+j].add_hw_fail_num + session_statistics->table[table_offset+j].other_not_acc_num;
            acc_in_collision_num += session_statistics->table[table_offset+j].acc_in_collision_num;
            add_hw_fail_num += session_statistics->table[table_offset+j].add_hw_fail_num;
            not_acc_for_mgm_num += session_statistics->table[table_offset+j].not_acc_for_mgm_num;
            other_not_acc_num += session_statistics->table[table_offset+j].other_not_acc_num;
            free_hash += session_statistics->max_hash_bucket_per_index[i] - session_statistics->table[table_offset+j].acc_in_hash_num;
            
            
            IFX_PPACMD_PRINT("  [%5d]  %4d        %4d        %4d        %4d        %4d        %4d    %4d\n", j, 
                session_statistics->table[table_offset+j].in_hash_num,
                session_statistics->table[table_offset+j].acc_in_hash_num,
                session_statistics->table[table_offset+j].acc_in_collision_num,
                session_statistics->table[table_offset+j].add_hw_fail_num,
                session_statistics->table[table_offset+j].not_acc_for_mgm_num,
                session_statistics->table[table_offset+j].other_not_acc_num,
                session_statistics->max_hash_bucket_per_index[i] - session_statistics->table[table_offset+j].acc_in_hash_num ); 
            
            collision_num += session_statistics->table[table_offset+j].acc_in_collision_num;
            if( session_statistics->table[table_offset+j].acc_in_hash_num > session_statistics->max_hash_bucket_per_index[i] )
            {
                IFX_PPACMD_PRINT("why acceleated (%d) is more than its bucket(%d)\n", 
                                session_statistics->table[table_offset+j].acc_in_hash_num,
                                session_statistics->max_hash_bucket_per_index[i] );
                                
            }
        }
        
        IFX_PPACMD_PRINT("  [total]  %4d        %4d        %4d        %4d        %4d        %4d    %4d\n",
                in_hash_num,
                acc_in_hash_num,
                acc_in_collision_num,
                add_hw_fail_num,
                not_acc_for_mgm_num,
                other_not_acc_num,
                free_hash ); 
        IFX_PPACMD_PRINT("  collision(used/free): %d/%d\n", 
                        collision_num,  
                        session_statistics->max_collision[i] - collision_num );
        
        if( collision_num > session_statistics->max_collision[i] )
        {
            IFX_PPACMD_PRINT("why acceleated (%d) is more than its bucket(%d)\n", 
                            collision_num,
                            session_statistics->max_collision[i] );
                            
        }

        //update statistics
        session_statistics->in_hash_num += in_hash_num;
        session_statistics->acc_in_hash_num += acc_in_hash_num;
        session_statistics->waiting_session += waiting_session;
        session_statistics->acc_in_collision_num += acc_in_collision_num;
        session_statistics->add_hw_fail_num += add_hw_fail_num;
        session_statistics->not_acc_for_mgm_num += not_acc_for_mgm_num;
        session_statistics->other_not_acc_num += other_not_acc_num;
        session_statistics->free_hash += free_hash;
        session_statistics->free_in_collision += session_statistics->max_collision[i] - collision_num; 
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("   -sum-   %4d        %4d        %4d        %4d        %4d        %4d    %4d\n",
                session_statistics->in_hash_num,
                session_statistics->acc_in_hash_num,
                session_statistics->acc_in_collision_num,
                session_statistics->add_hw_fail_num,
                session_statistics->not_acc_for_mgm_num,
                session_statistics->other_not_acc_num,
                session_statistics->free_hash ); 
    IFX_PPACMD_PRINT("         Acclerated    %4d\n", session_statistics->acc_in_hash_num + session_statistics->acc_in_collision_num);
    IFX_PPACMD_PRINT("         Free          %4d\n", session_statistics->free_hash + session_statistics->free_in_collision);
    if( display_non_lan_wan_session_num )
    {
        PPA_CMD_DATA tmp_cmd_info;        
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_NONE_LAN_WAN_SESSION, &tmp_cmd_info ) == PPA_CMD_OK ) 
        {        
            IFX_PPACMD_PRINT("         Non LAN/WAN   %4d (unknown session for netif not added into PPA)\n", tmp_cmd_info.count_info.count );        
        }
        else
        {
            IFX_PPACMD_PRINT("Failed to get PPA_CMD_GET_COUNT_NONE_LAN_WAN_SESSION\n");
        }
    }

END: 
    if( session_statistics )
    {
        ppa_free_session_mgm_list(&session_statistics->failed_hash_list_head);
        ppa_free_session_mgm_list(&session_statistics->non_accelerable_sess_list_head);
        ppa_free_session_mgm_list(&session_statistics->not_acc_for_mgm_list_head);     
        for(i=0; i<session_statistics->table_num; i++) 
        {
            int table_offset = session_statistics->hash_table_offset[i];
            for(j=0; j<session_statistics->max_table_hash_index_num[i]; j++ )
            {
                ppa_free_session_mgm_list(&session_statistics->table[table_offset + j].add_hw_fail_list_head);
                ppa_free_session_mgm_list(&session_statistics->table[table_offset + j].other_not_acc_list_head);
            }
        }        
        free(session_statistics);
        session_statistics = NULL;
    }
    if( sessions )
    {
        free(sessions);
    }
	if ( item )
	{
		ppa_free_session_mgm_list(&item->hlist);
		free(item);
		item = NULL;
	}

    return PPA_CMD_OK;
}

static int ppa_session_hash_summary()
{
    PPA_CMD_SESSION_SUMMARY_INFO *ppa_sum;
    PPA_CMD_COUNT_INFO count_info={0};
    int i, j,len, num=0;

    if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_PPA_HASH, &count_info ) != PPA_CMD_OK )
        return PPA_CMD_ERR;
    if( count_info.count <= 0 )
    {
        IFX_PPACMD_PRINT("Why PPA Hash Max Index num is not valid:%d\n", count_info.count);
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("count_info.count: %d\n", count_info.count);
    len = sizeof(PPA_CMD_SESSION_SUMMARY_INFO) + sizeof(PPA_SESSION_SUMMARY) * count_info.count;
    ppa_sum =(PPA_CMD_SESSION_SUMMARY_INFO *) malloc( len );
    if( ppa_sum == NULL )
    {
        IFX_PPACMD_PRINT("Failed to malloc memory: %d bytes\n", len);
        return PPA_CMD_ERR;
    }
    ppa_memset(ppa_sum, len, 0);
    ppa_sum->hash_max_num = count_info.count;
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_PPA_HASH_SUMMARY, ppa_sum ) != PPA_CMD_OK )
    {
        free(ppa_sum);
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_PRINT("\nPPA Hash Usage with %d hash index\n", count_info.count);
    for(i=0; i<count_info.count;  )
    {
        #define NUM_PER_LINE 30
         if( (i % NUM_PER_LINE ) == 0 )
            IFX_PPACMD_PRINT("[%5d]:", i);
         for(j=0; j<NUM_PER_LINE; j++ )
         {
             if( j + i >= count_info.count ) break;
             IFX_PPACMD_PRINT("%2u ", ppa_sum->hash_info.count[i+j]);
             num += ppa_sum->hash_info.count[i+j];             
         }
         IFX_PPACMD_PRINT("\n");
         i+= NUM_PER_LINE;         
    }
    IFX_PPACMD_PRINT("[total]:%4u\n", num);
    if( ppa_sum ) 
        free(ppa_sum);
    return PPA_CMD_OK;
}

static const char ppa_summary_short_opts[] = "wpah";
static void ppa_summary_help( int summary)
{
    IFX_PPACMD_PRINT("summary [-w] [-p ]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("  -a: all summeries\n");
        IFX_PPACMD_PRINT("  -w: acceleration session hash summary ( default )\n");
        IFX_PPACMD_PRINT("  -p: ppa session hash summary\n");
    }
    return;
}

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static void ppa_get_session_criteria_help()
{
    IFX_PPACMD_PRINT("getsessioncriteria\n");
}
static int ppa_parse_get_session_criteria(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    return PPA_CMD_OK;
}
#endif
static int ppa_parse_summary_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
#define SUMMARY_PPE_SESSION 1
#define SUMMARY_PPA_SESSION 2

    uint32_t flag = 0;
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'a':
            flag = ~0;
            break;
            
        case 'w':
            flag |= SUMMARY_PPE_SESSION;            
            break;

        case 'p':
            flag |= SUMMARY_PPA_SESSION;            
            break;

        case 'n':
            display_non_lan_wan_session_num = 1;
            break;

        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( flag == 0 ) 
        flag = SUMMARY_PPE_SESSION;

    if( flag & SUMMARY_PPE_SESSION )
        ppa_acceleration_session_hash_summary();
    if( flag & SUMMARY_PPA_SESSION )
        ppa_session_hash_summary();       
    
        
    return PPA_CMD_OK;


}


/*
====================================================================================
   command:   delsession
   description: delete a routing session based on IP address (IPv4/v6) or MAC address
   options:
====================================================================================
*/
static const char ppa_del_session_short_opts[] = "i:m:";

static void ppa_del_session_help(void)
{
    IFX_PPACMD_PRINT("delsession [-i IPv4/v6_address(source/destination)] [-m MAC_address(source/destination)]\n");
    return;
}

static int ppa_parse_del_session_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int i_opt=0, m_opt=0, t_opt=0;
    unsigned int ip_type=0;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
            if( (ip_type = str_convert(STRING_TYPE_IP, popts->optarg, (void *)&pdata->del_session.u.ip)) == IP_NON_VALID )
            {
                IFX_PPACMD_PRINT("Wrong source ip:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            };
            if( ip_type == IP_VALID_V6 )
                pdata->del_session.type |= PPA_SESS_DEL_USING_IPv6;
            else
                pdata->del_session.type |= PPA_SESS_DEL_USING_IPv4;

            i_opt ++;
            break;

        case 'm':
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;

		}
		stomac(popts->optarg, pdata->del_session.u.mac_addr);
		pdata->del_session.type |= PPA_SESS_DEL_USING_MAC;
		m_opt ++;
		break;


        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }
    if( i_opt == 0 && m_opt==0 && t_opt==0 )  /*no parameter is provided */
    {
        ppa_del_session_help();
        return PPA_CMD_ERR;
    }
    if( i_opt > 1 || m_opt > 1 || t_opt > 1 )
    {
        IFX_PPACMD_PRINT("wrong input: -i/m.\n");
        ppa_del_session_help();
        return PPA_CMD_ERR;
    }

    if( m_opt == 1)
    {
        IFX_PPACMD_DBG("delete session via mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        pdata->del_session.u.mac_addr[0],
                        pdata->del_session.u.mac_addr[1],
                        pdata->del_session.u.mac_addr[2],
                        pdata->del_session.u.mac_addr[3],
                        pdata->del_session.u.mac_addr[4],
                        pdata->del_session.u.mac_addr[5]);
    }
    else
    {
        if( pdata->del_session.type & PPA_SESS_DEL_USING_IPv6)
        {
            IFX_PPACMD_DBG("delete session via ppa session with source/destination IPv6 address %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                    NIP6(pdata->del_session.u.ip.ip6));
        }
        else
        {
            IFX_PPACMD_DBG("delete session via ppa session with source/destination IPv4 address %u.%u.%u.%u",
                    NIPQUAD(pdata->del_session.u.ip.ip));
        }
    }
    return PPA_CMD_OK;

}

/*
====================================================================================
   command:   addsession
   description: delete a routing session
   options:
====================================================================================
*/
static const char ppa_add_session_short_opts[] = "f:t:s:d:p:i:w:x:y:n:m:o:r:c:q:z:h";
static const struct option ppa_add_session_long_opts[] =
{
    {"src-ip",   required_argument,  NULL, 'f'},
    {"dst-ip",   required_argument,  NULL, 't'},
    {"src-port",   required_argument,  NULL, 's'},
    {"dst-port",   required_argument,  NULL, 'd'},
    {"proto",   required_argument,  NULL, 'p'},
    {"dest_ifid",   required_argument,  NULL, 'i'},
    {"wan-flag",   required_argument,  NULL, 'w'},
    {"src-mac",   required_argument,  NULL, 'x'},
    {"dst-mac",   required_argument,  NULL, 'y'},


    {"nat-ip",   required_argument,  NULL, 'n'},
    {"nat-port",   required_argument,  NULL, 'm'},
    {"new_dscp",   required_argument,  NULL, 'o'},

    {"in_vlan_id",   required_argument,  NULL, 'r'},
    {"out_vlan_tag",   required_argument,  NULL, 'c'},
    {"qid",   required_argument,  NULL, 'q'},
    {"pppoe-id",   required_argument,  NULL, 'z'},

    { 0,0,0,0 }
};

#define DEFAULT_SRC_IP "192.168.168.100"
#define DEFAULT_DST_IP "192.168.0.100"
#define DEFAULT_SRC_PORT "1024"
#define DEFAULT_DST_PORT "1024"
#define DEFAULT_SRC_MAC "00:11:22:33:44:11"
#define DEFAULT_DST_MAC "00:11:22:33:44:22"
static void ppa_add_session_help( int summary)
{
    IFX_PPACMD_PRINT("addsession\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("addsession [-f src-ip] [-t dst-ip ] [-s src-port ] [-d dst-port] [-p proto] [-i dest_ifid] [-w wan-flag]\n");
        IFX_PPACMD_PRINT("         [-x src-mac] [-y dst-mac]\n");
        IFX_PPACMD_PRINT("         [-n nat-ip] [-m nat-port ] [-o new_dscp ] \n");
        IFX_PPACMD_PRINT("         [-r in_vlan_id] [-c out_vlan_tag ] [-q queue-id ] [-z pppoe-id] \n");
        IFX_PPACMD_PRINT("  Note:  This commands is only for test purpose !!!\n");
        IFX_PPACMD_PRINT("        a) [-f src-ip]: default is %s\n", DEFAULT_SRC_IP);
        IFX_PPACMD_PRINT("        b) [-t dst-ip]: default is %s\n", DEFAULT_DST_IP);
        IFX_PPACMD_PRINT("        c) [-s src-port ]: default is %s\n", DEFAULT_SRC_PORT);
        IFX_PPACMD_PRINT("        d) [-d dst-port ]: default is %s\n", DEFAULT_DST_PORT);
        IFX_PPACMD_PRINT("        e) [-p proto]: default is udp.  Supported value are  udp and tcp\n");
        IFX_PPACMD_PRINT("        f) [-i dest_ifid]: default is 1, ie, eth1\n");
        IFX_PPACMD_PRINT("        g) [-w wan-flag]: default is lan, supported value are lan and wan\n");
        IFX_PPACMD_PRINT("        h) [-i dest_ifid]: default is 1, ie, eth1\n");
        IFX_PPACMD_PRINT("        i) [-x src-mac]: default is %s\n", DEFAULT_SRC_MAC );
        IFX_PPACMD_PRINT("        j) [-y dst_mac]: default is %s\n", DEFAULT_DST_MAC);
        IFX_PPACMD_PRINT("        l) [-n nat-ip]: default is 0, ie no NAT\n");
        IFX_PPACMD_PRINT("        l) [-m nat-port]: default is 0, ie, no NAT\n");
        IFX_PPACMD_PRINT("        m) [-o new_dscp]: default is 0, ie, no dscp change\n");
        IFX_PPACMD_PRINT("        n) [-r in_vlan_id]: default is 0, ie, no inner vlan change\n");
        IFX_PPACMD_PRINT("        o) [-c out_vlan_id]: default is 0, ie, no out vlan change\n");
        IFX_PPACMD_PRINT("        p) [-q queue-id]: default is 0\n");
        IFX_PPACMD_PRINT("        q) [-q pppoe-id]: default is 0, ie, no pppoe header insert\n");
    }
    return;
}

static int ppa_parse_add_session_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int i_opt=0, f_opt=0, t_opt=0, s_opt=0,d_opt=0, p_opt=0, w_opt=0, x_opt=0, y_opt=0, n_opt=0,m_opt=0, o_opt=0, r_opt=0, c_opt=0,q_opt=0,z_opt=0;
    unsigned int ip1_type=0, ip2_type=0, ip3_type=0;

    ppa_memset( &pdata->detail_session_info, sizeof(pdata->detail_session_info), 0 );
    pdata->detail_session_info.ip_proto = PPA_IPPROTO_UDP;
    pdata->detail_session_info.flags = SESSION_LAN_ENTRY | SESSION_VALID_OUT_VLAN_RM | SESSION_VALID_OUT_VLAN_RM | SESSION_VALID_MTU | SESSION_VALID_NEW_SRC_MAC;

    ip1_type = str_convert(STRING_TYPE_IP, DEFAULT_SRC_IP, (void *)&pdata->detail_session_info.src_ip);
    if( ip1_type == IP_VALID_V6)
        pdata->detail_session_info.flags |= SESSION_IS_IPV6;
    ip2_type = str_convert(STRING_TYPE_IP, DEFAULT_DST_IP, (void *)&pdata->detail_session_info.dst_ip);
    if( ip2_type == IP_VALID_V6 )
        pdata->detail_session_info.flags |= SESSION_IS_IPV6;

    pdata->detail_session_info.src_port= str_convert(STRING_TYPE_INTEGER, DEFAULT_SRC_PORT, NULL);
    pdata->detail_session_info.dst_port = str_convert(STRING_TYPE_INTEGER, DEFAULT_DST_PORT, NULL);
    stomac(DEFAULT_SRC_MAC, pdata->detail_session_info.src_mac );
    stomac(DEFAULT_DST_MAC, pdata->detail_session_info.dst_mac );


    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'f':
            if( (ip1_type = str_convert(STRING_TYPE_IP, popts->optarg, (void *)&pdata->detail_session_info.src_ip)) == IP_NON_VALID )
            {
                IFX_PPACMD_PRINT("Wrong source ip:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            };
            if( ip1_type == IP_VALID_V6 )
                pdata->detail_session_info.flags |= SESSION_IS_IPV6;

            f_opt ++;
            break;

        case 't':
            if( (ip2_type = str_convert(STRING_TYPE_IP, popts->optarg, (void *)&pdata->detail_session_info.dst_ip)) == IP_NON_VALID )
            {
                IFX_PPACMD_PRINT("Wrong Dst ip:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            };
            if( ip2_type == IP_VALID_V6 )
                pdata->detail_session_info.flags |= SESSION_IS_IPV6;
            t_opt ++;
            break;

        case 's':
            pdata->detail_session_info.src_port=  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            s_opt ++;
            break;

        case 'd':
            pdata->detail_session_info.dst_port=  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            d_opt ++;
            break;

        case 'p':
            if( stricmp(popts->optarg, "tcp") == 0 )
            {
                pdata->detail_session_info.ip_proto= PPA_IPPROTO_TCP;
            }
            else
            {
                pdata->detail_session_info.ip_proto= PPA_IPPROTO_UDP;
            }
            p_opt ++;
            break;
        case 'i':
            pdata->detail_session_info.dest_ifid=  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            i_opt ++;
            break;
        case 'w':
            if( stricmp(popts->optarg, "wan") == 0 )
                pdata->detail_session_info.flags |= SESSION_WAN_ENTRY;
            else
                pdata->detail_session_info.flags |= SESSION_LAN_ENTRY;
            w_opt ++;
            break;
        case 'x':  //src-mac
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg, pdata->detail_session_info.src_mac );
		pdata->detail_session_info.flags |= SESSION_VALID_NEW_SRC_MAC;
		x_opt ++;
		break;
        case 'y': //dst_mac
		if (! isValidMacAddress(popts->optarg)){
			IFX_PPACMD_PRINT("Wrong mac address:%s\n", popts->optarg);
			return PPA_CMD_ERR;
		}
		stomac(popts->optarg, pdata->detail_session_info.dst_mac );
		y_opt ++;
		break;
        case 'n': //nat-ip
            if( (ip3_type = str_convert(STRING_TYPE_IP, popts->optarg, (void *)&pdata->detail_session_info.natip)) == IP_NON_VALID )
            {
                IFX_PPACMD_PRINT("Wrong nat ip:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            };
            if( ip3_type == IP_VALID_V6 )
                pdata->detail_session_info.flags |= SESSION_IS_IPV6;
            pdata->detail_session_info.flags |= SESSION_VALID_NAT_IP;
            n_opt ++;
            break;
        case 'm': //nat-port
            pdata->detail_session_info.nat_port=  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->detail_session_info.flags |= SESSION_VALID_NAT_PORT;
            m_opt ++;
            break;
        case 'o': //new_dscp
            pdata->detail_session_info.new_dscp =  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->detail_session_info.new_dscp = SESSION_VALID_NEW_DSCP;
            o_opt ++;
            break;
        case 'r':  //in_vlan_id
            pdata->detail_session_info.in_vci_vlanid =  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->detail_session_info.flags |= SESSION_VALID_VLAN_INS;
            r_opt ++;
            break;
        case 'c':  //out_vlan_tag
            pdata->detail_session_info.out_vlan_tag =  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->detail_session_info.flags |= SESSION_VALID_OUT_VLAN_INS;
            c_opt ++;
            break;
        case 'q': //qid
            pdata->detail_session_info.qid =  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            q_opt ++;
            break;
        case 'z': //pppoe-id
            pdata->detail_session_info.pppoe_session_id =  str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->detail_session_info.flags |= SESSION_VALID_PPPOE;
            z_opt ++;
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( f_opt>1|| t_opt>1|| s_opt>1||d_opt>1|| p_opt>1|| w_opt>1|| x_opt>1||
            y_opt>1|| n_opt>1||m_opt>1|| o_opt>1|| r_opt>1|| c_opt>1||q_opt>1||z_opt>1)  /*too many parameters are provided */
    {
        IFX_PPACMD_PRINT("too many same paramemter are provided\n");
        return PPA_CMD_ERR;
    }
    if( f_opt==0&& t_opt==0&& s_opt==0&&d_opt==0&& p_opt==0&& w_opt==0&& x_opt==0&&
            y_opt==0&& n_opt==0&&m_opt==0&& o_opt==0&& r_opt==0&& c_opt==0&&q_opt==0&&z_opt==0)  /*too many parameters are provided */
    {
        IFX_PPACMD_PRINT("At least provide one parameter\n");
        return PPA_CMD_ERR;
    }
    if( ip1_type != ip2_type || ( ip3_type && ip3_type != ip1_type )  )
    {
        IFX_PPACMD_PRINT("src-ip, dst-ip, nap-ip should match, ie, both are IPV4 or IPV6 address\n");
        return PPA_CMD_ERR;
    }

    if( ip1_type == IP_VALID_V6 )
    {
        IFX_PPACMD_PRINT("ppacmd not support IPV6. Pls recompile ppacmd\n");
        return PPA_CMD_ERR;
    }

    if( pdata->detail_session_info.flags & SESSION_IS_IPV6 )
    {
        IFX_PPACMD_DBG("add session via ppa session tuple:%s from %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d to %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%5d\n",
                (pdata->detail_session_info.ip_proto == PPA_IPPROTO_TCP)?"tcp":"udp",
                NIP6(pdata->detail_session_info.src_ip.ip6), pdata->detail_session_info.src_port,
                NIP6(pdata->detail_session_info.dst_ip.ip6), pdata->detail_session_info.dst_port);

    }
    else
    {
        IFX_PPACMD_DBG("add session via ppa session tuple:%s from %u.%u.%u.%u/%5d to %u.%u.%u.%u/%5d\n",
                (pdata->detail_session_info.ip_proto == PPA_IPPROTO_TCP)?"tcp":"udp",
                NIPQUAD(pdata->detail_session_info.src_ip), pdata->detail_session_info.src_port,
                NIPQUAD(pdata->detail_session_info.dst_ip), pdata->detail_session_info.dst_port);
    }

    return PPA_CMD_OK;

}

/**** addsession --- end */

/*
====================================================================================
   command:   modifysession
   description: modify a routing session
   options:
====================================================================================
*/
static const char ppa_modify_session_short_opts[] = "a:f:m:p:h";
static const struct option ppa_modify_session_long_opts[] =
{
    {"session address",   required_argument,  NULL, 'a'},
    {"session address",   required_argument,  NULL, 'f'},
    {"acceleration mode",   required_argument,  NULL, 'm'},
    {"pppoe",  required_argument,  NULL, 'p'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static void ppa_modify_session_help( int summary)
{
    IFX_PPACMD_PRINT("modifysession [-a ppa-session-address] [ -f 0 | 1 | 2 ] [-m 0 | 1] \n");
    if( summary )
    {
        IFX_PPACMD_PRINT("  Note 1: -f: 1 to match LAN session only, 2 to match WAN session only, and 0 match LAN/WAN both\n");
        IFX_PPACMD_PRINT("  Note 2: -m: 0 to disable acceleration for this specified session and 1 to enable acceleration\n");
        IFX_PPACMD_PRINT("  Note 3: -p: none zero id is to add or replace pppoe session id ( for test purpose only)\n");
        IFX_PPACMD_PRINT("            : otherwise no pppoe action ( for test purpose only)\n");

    }
    return;
}

static int ppa_parse_modify_session_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt=0, m_opt=0, p_opt=0;
    uint32_t tmp_f = 0;

    ppa_memset( &pdata->session_extra_info, sizeof(pdata->session_extra_info), 0 );
    pdata->session_extra_info.lan_wan_flags = SESSION_WAN_ENTRY | SESSION_LAN_ENTRY;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'a':
            pdata->session_extra_info.session = (void *)str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            a_opt ++;
            break;

        case 'm':
            pdata->session_extra_info.flags |= PPA_F_ACCEL_MODE;
            if( str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL ) ) {
                pdata->session_extra_info.session_extra.accel_enable = 1;
            } else {
                pdata->session_extra_info.session_extra.accel_enable = 0;
		pdata->session_extra_info.flags |=SESSION_NOT_ACCEL_FOR_MGM;
	    }
            m_opt ++;
            break;

        case 'f':
            tmp_f= str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL );
            if( tmp_f == 1 ) pdata->session_extra_info.lan_wan_flags = SESSION_LAN_ENTRY;
            else if( tmp_f == 2 ) pdata->session_extra_info.lan_wan_flags = SESSION_WAN_ENTRY;
            else if( tmp_f == 0 ) pdata->session_extra_info.lan_wan_flags = SESSION_WAN_ENTRY | SESSION_LAN_ENTRY;
            else
            {
                IFX_PPACMD_PRINT("Wrong flag:%d\n", tmp_f);
                return PPA_CMD_ERR;
            }
            break;
            
        case 'p':
            pdata->session_extra_info.flags |= PPA_F_PPPOE;
            pdata->session_extra_info.session_extra.pppoe_id = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL );
            p_opt ++;
            IFX_PPACMD_DBG("pppoe_id:%d\n", pdata->session_extra_info.session_extra.pppoe_id);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( ( a_opt != 1 ) || ( m_opt != 1 && p_opt != 1 ) )
    {
        ppa_modify_session_help(1);
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("session address=0x%x\n", (unsigned int)pdata->session_extra_info.session  );
    IFX_PPACMD_DBG("Flag=0x%x\n", (unsigned int)pdata->session_extra_info.flags );
    if( pdata->session_extra_info.flags & PPA_F_ACCEL_MODE)
    {
        IFX_PPACMD_DBG("  %s\n", pdata->session_extra_info.session_extra.accel_enable ?"/s acceleration":"disable acceleration" );
    }
    if( pdata->session_extra_info.flags & PPA_F_PPPOE)
    {
        IFX_PPACMD_DBG("  %s with id=0x%d\n", pdata->session_extra_info.session_extra.pppoe_id? "new pppoe session id":"remove pppoe", pdata->session_extra_info.session_extra.pppoe_id );
    }

    return PPA_CMD_OK;
}

/*
====================================================================================
   command:   setsessiontimer
   description: set routing session polling timer
   options:
====================================================================================
*/
static const char ppa_set_session_timer_short_opts[] = "a:t:h";
static const struct option ppa_set_session_timer_long_opts[] =
{
    {"session address",   required_argument,  NULL, 'a'},
    {"timter",   required_argument,  NULL, 't'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static void ppa_set_session_timer_help( int summary)
{
    IFX_PPACMD_PRINT("setsessiontimer [-a ppa-session-address] [-t polling_timer_in_seconds] \n");
    if( summary )
    {
        IFX_PPACMD_PRINT("Note 1: by default, session address is 0, ie, to set ppa routing session polling timer only\n");
    }
    return;
}

static int ppa_parse_set_session_timer_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt=0, t_opt=0;

    ppa_memset( &pdata->session_timer_info, sizeof(pdata->session_timer_info), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'a':
            pdata->session_timer_info.session = (void *)str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            a_opt ++;
            break;

        case 't':
		if (! is_digital_value(popts->optarg)){
			IFX_PPACMD_PRINT("ppa_parse_set_session_timer_cmd : Invalid input\n");
			return PPA_CMD_ERR;
		}
		pdata->session_timer_info.timer_in_sec = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL );
		t_opt ++;
		break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( t_opt !=1 )
    {
        ppa_set_session_timer_help(1);
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("session address=%x\n", (unsigned int) pdata->session_timer_info.session  );
    IFX_PPACMD_DBG("timer=%d\n", (unsigned int)pdata->session_timer_info.timer_in_sec);

    return PPA_CMD_OK;
}


/*
====================================================================================
   command:   getsessiontimer
   description: get routing session polling timer
   options:
====================================================================================
*/
static const char ppa_get_session_timer_short_opts[] = "s:h";
static const struct option ppa_get_session_timer_long_opts[] =
{
    {"save",   required_argument,  NULL, 'o'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static void ppa_get_session_timer_help( int summary)
{
    IFX_PPACMD_PRINT("getsessiontimer [-o save_file] \n");
    return;
}

static int ppa_parse_get_session_timer_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( &pdata->session_timer_info, sizeof(pdata->session_timer_info), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}


static void ppa_print_get_session_timer(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("PPA Routing poll timer:%d\n", (unsigned int)pdata->session_timer_info.timer_in_sec );
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->session_timer_info),  sizeof(pdata->session_timer_info)  );
    }
}


/*
====================================================================================
   command:   delvfilter
   description: delete one vlan filter
   options:
====================================================================================
*/

/*delete all vfilter */
static void ppa_del_all_vfilter_help( int summary)
{
    IFX_PPACMD_PRINT("delallvfilter\n"); // [ -m <multicast-mac-address (for bridging only)>]
    if( summary )
        IFX_PPACMD_PRINT("    This command is for A4/D4 only at present\n");
    return;
}

/*
====================================================================================
   command:   getversion
   description: get ppa/ppe driver/ppe fw version
   options:
====================================================================================
*/
static void ppa_get_version_help( int summary)
{
    IFX_PPACMD_PRINT("getversion [ -o file ] [ -a ]\n"); // [ -m <multicast-mac-address (for bridging only)>]
    return;
}

PPA_VERSION ppa_api_ver;
PPA_VERSION ppa_stack_al_ver;
PPA_VERSION ppe_hal_ver;
PPA_VERSION ppe_fw_ver;
char *ppe_fw_ver_family_str[] = /*Platfrom of PPE FW */
{
    "Reserved",
    "Danube",
    "Twinpass",
    "Amazon-SE",
    "Reserved",
    "AR9",
    "GR9",
    "VR9",
    "AR10",
    "VRX318",
};
char *ppe_fw_ver_interface_str[] =
{
    "D4", // 0
    "D5", // 1
    "A4", // 2
    "E4", // 3
    "A5", // 4
    "E5",  // 5
    "Res",  // 6: reserved
    "D5"  // 7: in VR9 D5+Bonding fw, it is set to this value. So here we set it to D5
};

char *ppe_fw_ver_package_str[] = 
{
	"Reserved - 0",
	"A1",
	"B1 - PTM_BONDING",
	"E1",
	"A5",
	"D5",
	"D5v2",
	"E5",
};

char *ppe_drv_ver_family_str[] =   /* Network processor platfrom */
{
    "Reserved",  //bit 0, ie, value 0x1
    "Danube",  //bit 1, ie, value 0x2
    "Twinpass",  //bit 2, ie, value 0x4
    "Amazon-SE", //bit 3, ie, value 0x8
    "Reserved",  //bit 4, ie, value 0x10
    "AR9",       //bit 5  , ie, value 0x20
    "VR9",      //bit 6, ie, value 0x40
    "AR10",     //bit 7, ie, value 080
};

//get the non-zero high bit index ( from zero )
static int ppa_get_platform_id(uint32_t family)
{
    /*very one bit repreents one platform */
    int i;

    for(i=31; i>=0; i--)
        if( family & (1<<i) ) return i;

    return 0;
}

typedef enum {
    DSL_ATM_MODE=1,
    DSL_PTM_MODE
}dsl_mode;

static void ppa_print_get_version_cmd(PPA_CMD_DATA *pdata)
{
#ifndef CONFIG_SOC_GRX500
    int i;
    int k, num,id, dsl_atm_ptm=0;
#endif
 
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("PPA SubSystem version info:v%u.%u.%u-%u\n", (unsigned int)pdata->ver.ppa_subsys_ver.major, (unsigned int)pdata->ver.ppa_subsys_ver.mid, (unsigned int)pdata->ver.ppa_subsys_ver.minor, (unsigned int)pdata->ver.ppa_subsys_ver.tag );
        IFX_PPACMD_PRINT("  PPA ppacmd utility tool info:%u.%u.%u\n", PPACMD_VERION_MAJOR, PPACMD_VERION_MID, PPACMD_VERION_MINOR);
        IFX_PPACMD_PRINT("  PPA API driver info:%u.%u.%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.ppa_api_ver.family, (unsigned int)pdata->ver.ppa_api_ver.type, (unsigned int)pdata->ver.ppa_api_ver.itf, (unsigned int)pdata->ver.ppa_api_ver.mode, (unsigned int)pdata->ver.ppa_api_ver.major, (unsigned int)pdata->ver.ppa_api_ver.mid, (unsigned int)pdata->ver.ppa_api_ver.minor);
        IFX_PPACMD_PRINT("  PPA Stack Adaption Layer driver info:%u.%u.%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.ppa_stack_al_ver.family, (unsigned int)pdata->ver.ppa_stack_al_ver.type, (unsigned int)pdata->ver.ppa_stack_al_ver.itf, (unsigned int)pdata->ver.ppa_stack_al_ver.mode, (unsigned int)pdata->ver.ppa_stack_al_ver.major, (unsigned int)pdata->ver.ppa_stack_al_ver.mid, (unsigned int)pdata->ver.ppa_stack_al_ver.minor);


#ifdef CONFIG_SOC_GRX500
        IFX_PPACMD_PRINT("  PAE HAL driver info:%u.%u.%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.ppe_hal_ver.family, (unsigned int)pdata->ver.ppe_hal_ver.type, (unsigned int)pdata->ver.ppe_hal_ver.itf, (unsigned int)pdata->ver.ppe_hal_ver.mode, (unsigned int)pdata->ver.ppe_hal_ver.major, (unsigned int)pdata->ver.ppe_hal_ver.mid, (unsigned int)pdata->ver.ppe_hal_ver.minor);
        
        IFX_PPACMD_PRINT("  MPE HAL driver info:%u.%u.%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.mpe_hal_ver.family, (unsigned int)pdata->ver.mpe_hal_ver.type, (unsigned int)pdata->ver.mpe_hal_ver.itf, (unsigned int)pdata->ver.mpe_hal_ver.mode, (unsigned int)pdata->ver.mpe_hal_ver.major, (unsigned int)pdata->ver.mpe_hal_ver.mid, (unsigned int)pdata->ver.mpe_hal_ver.minor);
	
	IFX_PPACMD_PRINT("  MPE FW version:%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.ppe_fw_ver[1].family, (unsigned int)pdata->ver.ppe_fw_ver[1].type, (unsigned int)pdata->ver.ppe_fw_ver[1].major, (unsigned int)pdata->ver.ppe_fw_ver[1].mid, (unsigned int)pdata->ver.ppe_fw_ver[1].minor);

#else
        IFX_PPACMD_PRINT("  PPE HAL driver info:%u.%u.%u.%u.%u.%u.%u\n", (unsigned int)pdata->ver.ppe_hal_ver.family, (unsigned int)pdata->ver.ppe_hal_ver.type, (unsigned int)pdata->ver.ppe_hal_ver.itf, (unsigned int)pdata->ver.ppe_hal_ver.mode, (unsigned int)pdata->ver.ppe_hal_ver.major, (unsigned int)pdata->ver.ppe_hal_ver.mid, (unsigned int)pdata->ver.ppe_hal_ver.minor);
        id=ppa_get_platform_id(pdata->ver.ppe_hal_ver.family);
        if( id < NUM_ENTITY(ppe_drv_ver_family_str) )
        {
            IFX_PPACMD_PRINT("  PPE Driver Platform:%s\n", ppe_drv_ver_family_str[id] );
        }
        else
        {
            IFX_PPACMD_PRINT("PPE Driver Platform :unknown (0x%x)\n",  (unsigned int)pdata->ver.ppa_stack_al_ver.family);
        }
        num = 0;
        for(k=0; k<2; k++ )
        {
            if( pdata->ver.ppe_fw_ver[k].family == 0 )
            {
                if( k ==0 && pdata->ver.ppe_fw_ver[1].family != 0 )
                { // For VR9 D5 Ethernet wan mode, only PPE FW 1 is working, PE FW 0 not running at all
                    //note, so far only VR9 E5 are using two PPE. So here we harded some information here for E5 ethernet WAN mode
                    IFX_PPACMD_PRINT("  PPE0 FW Mode: %s family %s (not running now)\n", ppe_fw_ver_family_str[7], ppe_fw_ver_interface_str[5]);
                }
                continue;
            }
            IFX_PPACMD_PRINT("  PPE%d FW version:%u.%u.%u.%u.%u\n", k, (unsigned int)pdata->ver.ppe_fw_ver[k].family, (unsigned int)pdata->ver.ppe_fw_ver[k].type, (unsigned int)pdata->ver.ppe_fw_ver[k].major, (unsigned int)pdata->ver.ppe_fw_ver[k].mid, (unsigned int)pdata->ver.ppe_fw_ver[k].minor);

            IFX_PPACMD_PRINT("  PPE%d FW Mode:", k);
            if( pdata->ver.ppe_fw_ver[k].family < NUM_ENTITY(ppe_fw_ver_family_str) )
            {
                IFX_PPACMD_PRINT("%s family ", ppe_fw_ver_family_str[pdata->ver.ppe_fw_ver[k].family] );
            }
            else
            {
                IFX_PPACMD_PRINT("unknown family(%d)",  (unsigned int)pdata->ver.ppe_fw_ver[k].family);
            }
			if(pdata->ver.ppe_fw_ver[k].type < NUM_ENTITY(ppe_fw_ver_package_str)){
				IFX_PPACMD_PRINT(" %s", ppe_fw_ver_package_str[pdata->ver.ppe_fw_ver[k].type] );
			}else{			     
				IFX_PPACMD_PRINT(" unknown package(%d) with ppe_fw_ver_package_str[%d] ", (unsigned int)pdata->ver.ppe_fw_ver[k].type,  NUM_ENTITY(ppe_fw_ver_package_str));
			}
            if( k == 0 ) { //adapt for VRX318 from ppe_fw_ver_package_str
                if( pdata->ver.ppe_fw_ver[k+1].type == 2 /*B1-PTM Bonding: E1 w/ Bonding */ || 
                    pdata->ver.ppe_fw_ver[k+1].type == 3 /*E1 w/o bonding */ )
                    dsl_atm_ptm=DSL_PTM_MODE; //PTM
                else if( pdata->ver.ppe_fw_ver[k+1].type == 1 /*A1*/||
                         pdata->ver.ppe_fw_ver[k+1].type == 10 /*support wrong value in current VRX318 */)
                    dsl_atm_ptm=DSL_ATM_MODE; //PTM                
            }    
            IFX_PPACMD_PRINT("\n");
            IFX_PPACMD_DBG("PPE%d package=%d with dsl_atm_ptm=%d\n", k, pdata->ver.ppe_fw_ver[k].type, dsl_atm_ptm );

            if( num ) continue; //only print once for below messages
            num ++;

            IFX_PPACMD_DBG("  wan_port_map=%u mixed=%u\n", (unsigned int)pdata->ver.ppa_wan_info.wan_port_map, (unsigned int)pdata->ver.ppa_wan_info.mixed );

            //display wan interfaces in PPE FW
            IFX_PPACMD_PRINT("  PPE WAN interfaces: ");
            for(i=0; i<8; i++)
            {
                if( pdata->ver.ppa_wan_info.wan_port_map & (1<<i) )
                {
                    char dsl_mode[10]={0};
                    if( dsl_atm_ptm == DSL_ATM_MODE ) strcpy_s(dsl_mode,10, "(ATM)");
                    else if( dsl_atm_ptm == DSL_PTM_MODE ) strcpy_s(dsl_mode,10, "(PTM)");
                    if( i == 0 || i== 1 ) IFX_PPACMD_PRINT("eth%d ", i);
                    else if( i >=2  && i<6 ) IFX_PPACMD_PRINT("ext%d ", i-2);
                    else if( i ==6 || i==7 ) IFX_PPACMD_PRINT("DSL%s ", dsl_mode);
                }
            }
            IFX_PPACMD_PRINT("\n");

        }
        IFX_PPACMD_PRINT("  PPA IPV6:%s(ppe:%s/ppa:%s)\n", pdata->ver.ppe_fw_feature.ipv6_en & pdata->ver.ppa_feature.ipv6_en ? "enabled":"disabled",
                         pdata->ver.ppe_fw_feature.ipv6_en ? "enabled":"disabled",
                         pdata->ver.ppa_feature.ipv6_en ? "enabled":"disabled" );
#ifdef PPACMD_DEBUG_QOS
        IFX_PPACMD_PRINT("  PPA QOS:%s(ppe:%s/ppa:%s)\n", pdata->ver.ppe_fw_feature.qos_en & pdata->ver.ppa_feature.qos_en ? "enabled":"disabled",
                         pdata->ver.ppe_fw_feature.qos_en ? "enabled":"disabled",
                         pdata->ver.ppa_feature.qos_en ? "enabled":"disabled" );
#endif


        if( g_all )
        {
            int ioctl_cmd;
            PPA_CMD_DATA tmp_pdata;
			
            ioctl_cmd =PPA_CMD_GET_STATUS;
            ppa_memset( &tmp_pdata, sizeof(tmp_pdata), 0 );
            if( ppa_do_ioctl_cmd( ioctl_cmd, &tmp_pdata) == PPA_SUCCESS ) ppa_print_status( &tmp_pdata);
            
#ifdef CONFIG_PPA_QOS
            ioctl_cmd =PPA_CMD_GET_QOS_STATUS;
            ppa_memset( &tmp_pdata, sizeof(tmp_pdata), 0 );
            ppa_get_qstatus_do_cmd(NULL, &tmp_pdata );
#endif
            system( "[ -e /proc/driver/ifx_cgu/clk_setting ] && cat /proc/driver/ifx_cgu/clk_setting > /tmp/clk.txt" );
            system( "[ -e /tmp/clk.txt ] && cat /tmp/clk.txt && rm /tmp/clk.txt" );


        }
#endif
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->ver),  sizeof(pdata->ver)  );
    }
}

/*get bridge mac count */
static void ppa_get_br_count_help( int summary)
{
    IFX_PPACMD_PRINT("getbrnum [-o file ]\n");
    return;
}

static int ppa_parse_get_br_count(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int res =   ppa_parse_simple_cmd( popts, pdata );

    if( res != PPA_CMD_OK ) return res;


    pdata->count_info.flag = 0;

    return PPA_CMD_OK;
}

static void ppa_print_get_count_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The count is %u\n", (unsigned int)pdata->count_info.count );
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->count_info), sizeof(pdata->count_info) );
    }

}


/*get all bridge mac information */
static void ppa_get_all_br_help( int summary)
{
    IFX_PPACMD_PRINT("getbrs  [-o file ]\n"); // [ -m <multicast-mac-address (for bridging only)>]
    return;
}

static int ppa_get_all_br_cmd (PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_ALL_MAC_INFO *psession_buffer;
    PPA_CMD_DATA cmd_info;
    int res = PPA_CMD_OK, i, size;
    uint32_t flag = PPA_CMD_GET_ALL_MAC;

    //get session count first before malloc memroy
    cmd_info.count_info.flag = 0;
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_COUNT_MAC, &cmd_info ) != PPA_CMD_OK )
        return -EIO;

    if( cmd_info.count_info.count == 0 )
    {
        IFX_PPACMD_DBG("bridge mac count=0\n");
        if( g_output_to_file )
            SaveDataToFile(g_output_filename, (char *)(&cmd_info.count_info),  sizeof(cmd_info.count_info)  );
        return PPA_CMD_OK;
    }

    //malloc memory and set value correctly
    size = sizeof(PPA_CMD_COUNT_INFO) + sizeof(PPA_CMD_MAC_ENTRY) * ( 1 + cmd_info.count_info.count ) ;
    psession_buffer = (PPA_CMD_ALL_MAC_INFO *) malloc( size );
    if( psession_buffer == NULL )
    {
        IFX_PPACMD_PRINT("Malloc %d bytes failed\n", size );
        return PPA_CMD_NOT_AVAIL;
    }
    ppa_memset( psession_buffer, sizeof(size), 0 );

    psession_buffer->count_info.count = cmd_info.count_info.count;
    psession_buffer->count_info.flag = 0;

    //get session information
    if( (res = ppa_do_ioctl_cmd(flag, psession_buffer ) != PPA_CMD_OK ) )
    {
        free( psession_buffer );
        return res;
    }

    IFX_PPACMD_DBG("bridge mac count=%u. \n", (unsigned int)psession_buffer->count_info.count);


    if( !g_output_to_file )
    {
        for(i=0; i<psession_buffer->count_info.count; i++ )
        {
            //format like: [002] 239.  2.  2.  3 (route) qid  0 vlan 0000/04x From  nas0 to  eth0 ADDED_IN_HW|VALID_PPPOE|VALID_NEW_SRC_MAC
            IFX_PPACMD_PRINT("[%03d] %02x:%02x:%02x:%02x:%02x:%02x %s\n", i, psession_buffer->session_list[i].mac_addr[0],
                             psession_buffer->session_list[i].mac_addr[1],
                             psession_buffer->session_list[i].mac_addr[2],
                             psession_buffer->session_list[i].mac_addr[3],
                             psession_buffer->session_list[i].mac_addr[4],
                             psession_buffer->session_list[i].mac_addr[5],
                             psession_buffer->session_list[i].ifname );
        }
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(psession_buffer),  size  );
    }

    free( psession_buffer );
    return PPA_CMD_OK;
}

static void ppa_get_sizeof_help(int summary)
{
    IFX_PPACMD_PRINT("size\n");
    if( summary)
        IFX_PPACMD_PRINT("note: get PPA main structure's internal size"); //purposely don't display this command since it is not for customer
    return;
}

static void ppa_print_get_sizeof_cmd( PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("one unicast session size =%d \n", (unsigned int)pdata->size_info.rout_session_size);
        IFX_PPACMD_PRINT("one unicast session size in ioctl =%d \n", sizeof(PPA_CMD_SESSION_ENTRY));
        IFX_PPACMD_PRINT("one multicast session size =%d\n", (unsigned int)pdata->size_info.mc_session_size );
        IFX_PPACMD_PRINT("one bridge session size =%d \n", (unsigned int)pdata->size_info.br_session_size );
        IFX_PPACMD_PRINT("netif size =%d\n", (unsigned int)pdata->size_info.netif_size );

    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->size_info), sizeof(pdata->size_info) );
    }

    return;
}

/*ppacmd setbr: set bridge mac address learning hook enable/disable---begin*/
static void ppa_set_br_help( int summary)
{
    IFX_PPACMD_PRINT("setbr [-f 0/1]\n");
    return;
}
static const char ppa_set_br_short_opts[] = "f:h";
static const struct option ppa_set_br_long_opts[] =
{
    {"flag",   required_argument,  NULL, 'f'},
    { 0,0,0,0 }
};

static int ppa_set_br_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_BRIDGE_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'f':
            pdata->br_enable_info.bridge_enable= str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);;
            break;

        default:
            IFX_PPACMD_PRINT("ppa_set_br_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

/*ppacmd setbr: set bridge mac address learning hook enable/disable ---end*/

/*ppacmd getbrstatus: get bridge mac address learning hook status: enabled/disabled---begin*/
static void ppa_get_br_status_help( int summary)
{
    IFX_PPACMD_PRINT("getbrstatus [-o outfile]\n");
    return;
}
static const char ppa_get_br_status_short_opts[] = "o:h";
static const struct option ppa_get_br_status_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_get_br_status_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_BRIDGE_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_get_br_status_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}
static void ppa_print_get_br_status_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The bridge mac learning hook is %s\n", pdata->br_enable_info.bridge_enable ? "enabled":"disabled");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->br_enable_info), sizeof(pdata->br_enable_info) );
    }
}

int get_portid(char *ifname)
{
    PPA_CMD_PORTID_INFO portid;

    ppa_memset( &portid, sizeof(portid), 0);
    strncpy_s( portid.ifname,PPA_IF_NAME_SIZE, ifname,PPA_IF_NAME_SIZE);

    if( ppa_do_ioctl_cmd(PPA_CMD_GET_PORTID, &portid ) != PPA_CMD_OK )
    {
        IFX_PPACMD_PRINT("ppacmd get portid failed\n");
        return -1;
    }

    return portid.portid;
}

/*ppacmd ppa_get_br_status_cmd:   ---end*/

#ifdef CONFIG_SOC_GRX500
/*ppacmd getbrfid: get bridge Fid */
static void ppa_get_br_fid_help(int summary)
{
    IFX_PPACMD_PRINT("getbrfid <-i ifname> (NOTE: 65535 represents invalid bridge name)\n");
    return;
}
static const char ppa_get_br_fid_short_opts[] = "i:h";
static const struct option ppa_get_br_fid_long_opts[] =
{
    {"ifname",   required_argument,  NULL, 'i'},
    { 0,0,0,0 }
};
static int ppa_get_br_fid_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int i_opt = 0;

    ppa_memset(pdata, sizeof(pdata->br_fid_info ), 0);

    while(popts->opt)
    {
        switch(popts->opt)
        {
        	case 'i':
            	strncpy_s(pdata->br_fid_info.ifname,sizeof(pdata->br_fid_info.ifname), popts->optarg, sizeof(pdata->br_fid_info.ifname) );
            	i_opt ++;
            	break;

        	default:
            	return PPA_CMD_ERR;
        }
        popts++;
    }

    if(i_opt != 1)
    {
        IFX_PPACMD_PRINT("Wrong parameter.\n");
        return PPA_CMD_ERR;
    }
    return PPA_CMD_OK;
}

static void ppa_print_get_br_fid_cmd(PPA_CMD_DATA *pdata)
{
    if(!g_output_to_file)
    {
        IFX_PPACMD_PRINT("The %s's fid is %d\n", pdata->br_fid_info.ifname, (unsigned int)pdata->br_fid_info.fid);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->br_fid_info.fid), sizeof(pdata->br_fid_info.fid));
    }
}

/*ppacmd getbrfid:   ---end*/
#endif /* CONFIG_SOC_GRX500 */

#ifdef CONFIG_PPA_QOS

static void ppa_get_qstatus_help( int summary)
{
    IFX_PPACMD_PRINT("getqstatus [-o outfile]\n");
    return;
}
static const char ppa_get_qstatus_short_opts[] = "o:h";
static int ppa_parse_get_qstatus_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA* pdata)
{
    int  out_opt=0;

    ppa_memset( pdata, sizeof(PPA_CMD_QOS_STATUS_INFO), 0 );

    //note, must set below max_buffer_size
    //pdata->qos_status_info.qstat.max_buffer_size = NUM_ENTITY( pdata->qos_status_info.qstat.mib);
    while(popts->opt)
    {
        if (popts->opt == 'o')
        {
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            out_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if (out_opt > 1)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("PPA getqstatus\n");

    return PPA_CMD_OK;
}

static int ppa_get_qstatus_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    int i;
    int res=PPA_CMD_ERR;

    for(i=0; i<PPA_MAX_PORT_NUM; i++ )
    {
        pdata->qos_status_info.qstat.qos_queue_portid = i;
        if( ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_STATUS, &pdata->qos_status_info ) == PPA_CMD_OK )
        {          
            if( pdata->qos_status_info.qstat.res == PPA_SUCCESS )
            {
                ppa_print_get_qstatus_cmd(pdata);
                res = PPA_CMD_OK;
            }            
        }
    }
    return res;
}
static void ppa_print_get_qstatus_cmd(PPA_CMD_DATA *pdata)
{
    unsigned long long rate[PPA_MAX_QOS_QUEUE_NUM];
    unsigned long long rate_L1[PPA_MAX_QOS_QUEUE_NUM];
    
    if( g_output_to_file)
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->qos_status_info), sizeof(pdata->qos_status_info) );
        return ;
    }

    if( pdata->qos_status_info.qstat.qos_queue_portid == -1 )
    {
        /*qos is not enabled */
        IFX_PPACMD_PRINT("Note: PPA QOS is not supported for wan_itf=%x  mixed_itf=%x\n", (unsigned int)pdata->qos_status_info.qstat.wan_port_map, (unsigned int)pdata->qos_status_info.qstat.wan_mix_map);
        return ;
    }

    IFX_PPACMD_PRINT("\nPort[%2d]\n  qos     : %s(For VR9 E5 VDSL WAN mode, this flag is not used)\n  wfq     : %s\n  Rate shaping: %s\n\n",
                     pdata->qos_status_info.qstat.qos_queue_portid,
                     pdata->qos_status_info.qstat.eth1_qss ?"enabled":"disabled",
                     pdata->qos_status_info.qstat.wfq_en?"enabled":"disabled",
                     pdata->qos_status_info.qstat.shape_en ?"enabled":"disabled");

    IFX_PPACMD_PRINT("  Ticks  =%u,  overhd  =%u,     qnum=%u  @0x%x\n",
                     (unsigned int)pdata->qos_status_info.qstat.time_tick,
                     (unsigned int)pdata->qos_status_info.qstat.overhd_bytes,
                     (unsigned int)pdata->qos_status_info.qstat.eth1_eg_qnum,
                     (unsigned int)pdata->qos_status_info.qstat.tx_qos_cfg_addr);

    IFX_PPACMD_PRINT("  PPE clk=%u MHz, basic tick=%u\n",(unsigned int)pdata->qos_status_info.qstat.pp32_clk/1000000, (unsigned int)pdata->qos_status_info.qstat.basic_time_tick );

//#ifdef CONFIG_PPA_QOS_WFQ
    IFX_PPACMD_PRINT("\n  wfq_multiple : %08u @0x%x", (unsigned int)pdata->qos_status_info.qstat.wfq_multiple, (unsigned int)pdata->qos_status_info.qstat.wfq_multiple_addr);
    IFX_PPACMD_PRINT("\n  strict_weight: %08u @0x%x\n", (unsigned int)pdata->qos_status_info.qstat.wfq_strict_pri_weight, (unsigned int)pdata->qos_status_info.qstat.wfq_strict_pri_weight_addr);
//#endif

    if ( pdata->qos_status_info.qstat.eth1_eg_qnum && pdata->qos_status_info.qstat.max_buffer_size  )
    {
        uint32_t i;
        uint32_t times = (pdata->qos_status_info.qstat.eth1_eg_qnum > pdata->qos_status_info.qstat.max_buffer_size) ? pdata->qos_status_info.qstat.max_buffer_size:pdata->qos_status_info.qstat.eth1_eg_qnum;

        IFX_PPACMD_PRINT("\n  Cfg :  T   R   S -->  Bit-rate(kbps)    Weight --> Level     Address     d/w    tick_cnt   b/S\n");

        for ( i = 0; i < times; i++ )
        {
            IFX_PPACMD_PRINT("\n    %2u:  %03u  %05u  %05u   %07u      %08u   %03u    @0x%x   %08u  %03u   %05u\n", (unsigned int)i,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].t,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].r,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].s,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].bit_rate_kbps,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].w,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].weight_level,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].reg_addr,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].d,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].tick_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.queue_internal[i].b);
        }

        //QOS Note: For ethernat wan mode only one port rateshaping.  For E5 ptm mode, we have 4 gamma interface port rateshaping configuration
        if( pdata->qos_status_info.qstat.qos_queue_portid == 7 ){//PTM wan mode
            for( i = 0; i < 4; i ++){
                IFX_PPACMD_PRINT("\n  p[%d]:  %03u  %05u  %05u   %07u                        @0x%x   %08u  %03u   %05u\n",
                             i, (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].t,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].r,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].s,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].bit_rate_kbps,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].reg_addr,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].d,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].tick_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.ptm_qos_port_rate_shaping[i].b);
            }
        }
        else if( pdata->qos_status_info.qstat.qos_queue_portid & 3){
            IFX_PPACMD_PRINT("\n  port:  %03u  %05u  %05u   %07u                        @0x%x   %08u  %03u   %05u\n",
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.t,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.r,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.s,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.bit_rate_kbps,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.reg_addr,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.d,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.tick_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.qos_port_rate_internal.b);

        }
        
        //print debug if necessary
        for ( i = 0; i <times ; i++ )
        {
            if( pdata->qos_status_info.qstat.mib[i].mib.tx_diff_jiffy == 0 ) 
            {
                IFX_PPACMD_PRINT("Error, why tx_diff_jiffy[%2u] is zero\n", i);                
                pdata->qos_status_info.qstat.mib[i].mib.tx_diff_jiffy = 1;
            }
            rate[i]=pdata->qos_status_info.qstat.mib[i].mib.tx_diff * 8 * pdata->qos_status_info.qstat.mib[i].mib.sys_hz/pdata->qos_status_info.qstat.mib[i].mib.tx_diff_jiffy;
            rate_L1[i]=pdata->qos_status_info.qstat.mib[i].mib.tx_diff_L1 * 8 * pdata->qos_status_info.qstat.mib[i].mib.sys_hz/pdata->qos_status_info.qstat.mib[i].mib.tx_diff_jiffy;
        }
        
        IFX_PPACMD_DBG("\n  Info : Rate  tx_diff_bytes tx_diff_jiffy   HZ\n");
        for ( i = 0; i < times ; i++ )
        {            
            IFX_PPACMD_DBG("  %2u:    %010llu %010llu %010llu %010u\n",(unsigned int)i,rate[i],
                             pdata->qos_status_info.qstat.mib[i].mib.tx_diff,
                             pdata->qos_status_info.qstat.mib[i].mib.tx_diff_jiffy,
                             pdata->qos_status_info.qstat.mib[i].mib.sys_hz);            
        }

        IFX_PPACMD_PRINT("\n  MIB : rx_pkt/rx_bytes      tx_pkt/tx_bytes         cpu_small/total_drop  fast_small/total_drop  tx rate/L1(bps/sec)   address\n");
        for ( i = 0; i < times ; i++ )
        {            
            IFX_PPACMD_PRINT("  %2u: %010u/%010u  %010u/%010u  %010u/%010u  %010u/%010u  %010u/%010u @0x%x\n",
                             (unsigned int)i,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.total_rx_pkt,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.total_rx_bytes,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.total_tx_pkt,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.total_tx_bytes,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.cpu_path_small_pkt_drop_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.cpu_path_total_pkt_drop_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.fast_path_small_pkt_drop_cnt,
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].mib.fast_path_total_pkt_drop_cnt,
                             (unsigned int)rate[i],
                             (unsigned int)rate_L1[i],
                             (unsigned int)pdata->qos_status_info.qstat.mib[i].reg_addr                            
                            ) ;

        }

        //QOS queue descriptor
        IFX_PPACMD_PRINT("\n  Desc: threshold  num  base_addr  rd_ptr   wr_ptr\n");
        for(i=0; i<times; i++)
        {
            IFX_PPACMD_PRINT("  %2u: 0x%02x     0x%02x   0x%04x   0x%04x   0x%04x  @0x%x\n",
                             (unsigned int)i,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].threshold,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].length,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].addr,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].rd_ptr,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].wr_ptr,
                             (unsigned int)pdata->qos_status_info.qstat.desc_cfg_interanl[i].reg_addr );
        }

    }
    else
    {
        IFX_PPACMD_PRINT("Note: PPA QOS is disabled for wan_itf=%x  mixed_itf=%x\n",
                         (unsigned int)pdata->qos_status_info.qstat.wan_port_map,
                         (unsigned int)pdata->qos_status_info.qstat.wan_mix_map);
    }


}


/*ppacmd getqnum: get eth1 queue number  ---begin*/
static void ppa_get_qnum_help( int summary)
{
    IFX_PPACMD_PRINT("getqnum [-p portid] [-i ifname] [-o outfile]\n");
    if (summary )
        IFX_PPACMD_PRINT("note: one or only one of portid or ifname must be specified\n");
    return;
}
static const char ppa_get_qnum_short_opts[] = "p:o:i:h";
static const struct option ppa_get_qnum_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"interface",   required_argument,  NULL, 'i'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_qnum_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA* pdata)
{
    int p_opt=0, i_opt=0;
    ppa_memset( pdata, sizeof(PPA_CMD_QUEUE_NUM_INFO),0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qnum_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            \
            p_opt ++;
            break;

        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qnum_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("ppa_parse_get_qnum_cmd: portid=%d, queue_num=%d\n", (unsigned int)pdata->qnum_info.portid, (unsigned int)pdata->qnum_info.queue_num);

    return PPA_CMD_OK;
}
static void ppa_print_get_qnum_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The queue number( of port id %d )  is %d\n", (unsigned int)pdata->qnum_info.portid, (unsigned int)pdata->qnum_info.queue_num);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->qnum_info), sizeof(pdata->qnum_info) );
    }
}
/*ppacmd getqnum: get eth1 queue number  ---end*/


/*ppacmd getmib: get qos mib counter  ---begin*/
static void ppa_get_qmib_help( int summary)
{
    IFX_PPACMD_PRINT("getqmib [-p portid] [-i ifname] <-q queuid> [-o outfile]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: one or only one of portid or ifname must be specified\n");
        IFX_PPACMD_PRINT("    if queueid is not provided, then it will get all queue's mib coutners\n");
    }
    return;
}
static const char ppa_get_qmib_short_opts[] = "p:i:q:o:h";
static const struct option ppa_get_qmib_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queueid",   required_argument,  NULL, 'q'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_qmib_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA* pdata)
{
    int p_opt=0, i_opt=0;

    ppa_memset( pdata, sizeof(PPA_CMD_QOS_MIB_INFO), 0 );
    pdata->qos_mib_info.queue_num = -1;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qos_mib_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_opt ++;
            break;

        case 'i':
            strcpy_s( pdata->qos_mib_info.ifname,PPA_IF_NAME_SIZE, popts->optarg );
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_mib_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case 'q':
            pdata->qos_mib_info.queue_num= str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;

        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_get_qmib_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_mib_info.portid, (unsigned int)pdata->qos_mib_info.queue_num);

    return PPA_CMD_OK;
}

static int ppa_get_qmib_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_QUEUE_NUM_INFO qnum_info;
    PPA_CMD_QOS_MIB_INFO mib_info;
    int i, start_i, end_i=0;
    int res;

    ppa_memset( &qnum_info, sizeof(qnum_info), 0 );
    qnum_info.portid = pdata->qos_mib_info.portid;
    if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_QUEUE_MAX_NUM, &qnum_info ) != PPA_CMD_OK ) )
    {
        IFX_PPACMD_PRINT("ioctl PPA_CMD_GET_QOS_QUEUE_MAX_NUM fail\n");
        return PPA_FAILURE;
    }
    if( pdata->qos_mib_info.queue_num != -1 ) // set queueid already, then use it
    {
        start_i = pdata->qos_mib_info.queue_num;
        if( start_i >= qnum_info.queue_num -1 )
            start_i = qnum_info.queue_num -1;
        end_i = start_i + 1;
        IFX_PPACMD_PRINT("Need to read queue %d's mib counter\n", start_i);
    }
    else
    {
        start_i = 0;
        end_i = qnum_info.queue_num;
        IFX_PPACMD_PRINT("Need to read mib counter from queue %d to %d\n", start_i, end_i-1 );
    }

    for(i=start_i; i<end_i; i++)
    {
        ppa_memset( &mib_info, sizeof(mib_info), 0 );
        mib_info.portid = pdata->qos_mib_info.portid;
        strcpy_s( mib_info.ifname,PPA_IF_NAME_SIZE, pdata->qos_mib_info.ifname);
        mib_info.queue_num = i;

        if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_MIB, &mib_info) == PPA_CMD_OK ) )
        {
            if( !g_output_to_file  )
            {
                if( i== start_i )
                    IFX_PPACMD_PRINT("MIB  rx_pkt/rx_bytes     tx_pkt/tx_bytes    cpu_small_drop/cpu_drop  fast_small_drop/fast_drop_cnt  q_occ(pkt)\n");

                IFX_PPACMD_PRINT("  %2d: 0x%08x/0x%08x  0x%08x/0x%08x  0x%08x/0x%08x  0x%08x/0x%08x  0x%08x\n", i,
                                 (unsigned int)mib_info.mib.total_rx_pkt, (unsigned int)mib_info.mib.total_rx_bytes,
                                 (unsigned int)mib_info.mib.total_tx_pkt, (unsigned int)mib_info.mib.total_tx_bytes,
                                 (unsigned int)mib_info.mib.cpu_path_small_pkt_drop_cnt, (unsigned int)mib_info.mib.cpu_path_total_pkt_drop_cnt,
                                 (unsigned int)mib_info.mib.fast_path_small_pkt_drop_cnt, (unsigned int)mib_info.mib.fast_path_total_pkt_drop_cnt,
                                 (unsigned int)mib_info.mib.q_occ );
            }
            else
            {
                /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                SaveDataToFile(g_output_filename, (char *)(&mib_info), sizeof(mib_info) );
            }
        }
    }
    return PPA_SUCCESS;
}

/*ppacmd getqmib: get eth1 queue number  ---end*/

//#ifdef CONFIG_PPA_QOS_WFQ
/*ppacmd setctrlwfq ---begin*/
static void ppa_set_ctrl_wfq_help( int summary)
{
    IFX_PPACMD_PRINT("setctrlwfq [-p portid] [-i ifname] <-c enable | disable> [-m manual_wfq]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        IFX_PPACMD_PRINT("note: manual_wfq 0      -- default weight\n");
        IFX_PPACMD_PRINT("note: manual_wfq 1      -- use user specified weight directly\n");
        IFX_PPACMD_PRINT("note: manual_wfq other_value-- use user specified mapping \n");
    }
    return;
}
static const char ppa_set_ctrl_wfq_short_opts[] = "p:c:i:m:h";
static const struct option ppa_set_ctrl_wfq_long_opts[] =
{
    {"portid",  required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"control",   required_argument,  NULL, 'c'},
    {"manual",   required_argument,  NULL, 'm'},
    { 0,0,0,0 }
};
static int ppa_parse_set_ctrl_wfq_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int c_opt=0;
    int p_opt=0, i_opt=0;
    uint32_t res = 0;

    ppa_memset( pdata, sizeof(PPA_CMD_QOS_CTRL_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_ctrl_info.portid = res;
		p_opt ++;
		break;

        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_ctrl_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case 'c':
            if( strcmp("enable" , popts->optarg) == 0 )
                pdata->qos_ctrl_info.enable = 1;
            else if ( strcmp("disable" , popts->optarg) == 0 )
                pdata->qos_ctrl_info.enable = 0;
	    else {
		IFX_PPACMD_PRINT("ERROR: invalid input\n");
		return PPA_CMD_ERR;
	    }
            c_opt = 1;
            break;

        case 'm':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_ctrl_info.flags= res; //use manual set WFQ weight
		break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( c_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter control option\n");
        return PPA_CMD_ERR;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_set_ctrl_wfq_cmd: portid=%d, ctrl=%s\n", (unsigned int)pdata->qos_ctrl_info.portid, pdata->qos_ctrl_info.enable ? "enabled":"disable");

    return PPA_CMD_OK;
}
/*ppacmd setctrlwfq ---end*/

/*ppacmd getctrlwfq ---begin*/
static void ppa_get_ctrl_wfq_help( int summary)
{
    IFX_PPACMD_PRINT("getctrlwfq [-p portid] [-i ifname] [-o outfile]\n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_get_ctrl_wfq_short_opts[] = "p:i:o:h";
static const struct option ppa_get_ctrl_wfq_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_ctrl_wfq_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int p_opt=0, i_opt=0;
    ppa_memset( pdata, sizeof(PPA_CMD_QOS_CTRL_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qos_ctrl_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_opt ++;
            break;

        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_ctrl_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("ppa_parse_get_ctrl_wfq_cmd: portid=%d, ctrl=%s\n", (unsigned int)pdata->qos_ctrl_info.portid, pdata->qos_ctrl_info.enable? "enabled":"disabled");

    return PPA_CMD_OK;
}
static void ppa_print_get_ctrl_wfq_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The wfq of port id %d is %s\n", (unsigned int)pdata->qos_ctrl_info.portid, pdata->qos_ctrl_info.enable ? "enabled":"disabled");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->qos_ctrl_info), sizeof(pdata->qos_ctrl_info) );
    }
}
/*ppacmd getctrlwfq ---end*/



/*ppacmd setwfq ---begin*/
static void ppa_set_wfq_help( int summary)
{
    IFX_PPACMD_PRINT("setwfq [-p portid] [-i ifname] <-q queuid> <-w weight-level>\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("weight-level is from 0 ~ 100. 0/100 means lowest/highest strict priority queue\n");
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    }
    return;
}
static const char ppa_set_wfq_short_opts[] = "p:q:w:i:h";
static const struct option ppa_set_wfq_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queuid",   required_argument,  NULL, 'q'},
    {"weight-level",   required_argument,  NULL, 'w'},
    { 0,0,0,0 }
};

static int ppa_parse_set_wfq_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA*pdata)
{
    unsigned int w_opt=0, q_opt=0;
    int p_opt=0, i_opt=0;
    uint32_t res;
    ppa_memset( pdata, sizeof(PPA_CMD_WFQ_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_wfq_info.portid = res;
		p_opt ++;
		break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_wfq_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_wfq_info.queueid = res;
		q_opt = 1;
		break;
        case 'w':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_wfq_info.weight = res;
		w_opt = 1;
		break;
        default:
            IFX_PPACMD_PRINT("ppa_parse_set_wfq_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( q_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter queueid\n");
        return PPA_CMD_ERR;
    }
    if( w_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter weight\n");
        return PPA_CMD_ERR;
    }
    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("ppa_parse_set_wfq_cmd: portid=%d, queueid=%d, weight=%d\n", (unsigned int)pdata->qos_wfq_info.portid, (unsigned int)pdata->qos_wfq_info.queueid, (unsigned int)pdata->qos_wfq_info.weight);

    return PPA_CMD_OK;
}
/*ppacmd setwfq ---end*/

/*ppacmd resetwfq ---begin*/
static void ppa_reset_wfq_help( int summary)
{
    IFX_PPACMD_PRINT("resetwfq [-p portid] [-i ifname] <-q queuid> \n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_reset_wfq_short_opts[] = "p:q:i:h";
static const struct option ppa_reset_wfq_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queuid",   required_argument,  NULL, 'q'},
    { 0,0,0,0 }
};
static int ppa_parse_reset_wfq_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int q_opt=0;
    int p_opt=0, i_opt=0;
    uint32_t res;
    ppa_memset( pdata, sizeof(PPA_CMD_WFQ_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_wfq_info.portid = res;
		p_opt ++;
		break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_wfq_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_wfq_info.queueid = res;
		q_opt = 1;
		break;
        default:
            IFX_PPACMD_PRINT("ppa_parse_reset_wfq_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( q_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter queueid\n");
        return PPA_CMD_ERR;
    }
    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_reset_wfq_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_wfq_info.portid, (unsigned int)pdata->qos_wfq_info.queueid);

    return PPA_CMD_OK;
}
/*ppacmd resetwfq ---end*/

/*ppacmd getwfq ---begin*/
static void ppa_get_wfq_help( int summary)
{
    IFX_PPACMD_PRINT("getwfq [-p portid] [-i ifname] <-q queuid> [-o outfile]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        IFX_PPACMD_PRINT("    if queueid is not provided, then it will get all queue's wfq\n");
    }
    return;
}
static const char ppa_get_wfq_short_opts[] = "p:q:o:i:h";
static const struct option ppa_get_wfq_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queuid",   required_argument,  NULL, 'q'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_wfq_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int q_opt=0;
    int p_opt=0, i_opt=0;

    ppa_memset( pdata, sizeof(PPA_CMD_WFQ_INFO), 0 );
    pdata->qos_wfq_info.queueid = -1;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qos_wfq_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_opt ++;
            break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_wfq_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
            pdata->qos_wfq_info.queueid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            q_opt = 1;
            break;
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_get_wfq_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_wfq_info.portid, (unsigned int)pdata->qos_wfq_info.queueid);

    return PPA_CMD_OK;
}

static int ppa_get_wfq_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_QUEUE_NUM_INFO qnum_info;
    PPA_CMD_WFQ_INFO  info;
    int i, start_i, end_i=0;
    int res;

    ppa_memset( &qnum_info, sizeof(qnum_info), 0 );
    qnum_info.portid = pdata->qos_wfq_info.portid;
    if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_QUEUE_MAX_NUM, &qnum_info ) != PPA_CMD_OK ) )
    {
        IFX_PPACMD_PRINT("ioctl PPA_CMD_GET_QOS_QUEUE_MAX_NUM fail\n");
        return PPA_FAILURE;
    }

    if( pdata->qos_wfq_info.queueid != -1 ) // set queuid already, then use it
    {
        start_i = pdata->qos_wfq_info.queueid;
        if( start_i >= qnum_info.queue_num -1 )
            start_i = qnum_info.queue_num -1;
        end_i = start_i + 1;
        IFX_PPACMD_DBG("Need to read wfq from queue %d \n", start_i);
    }
    else
    {
        start_i = 0;
        end_i = qnum_info.queue_num;
        IFX_PPACMD_DBG("Need to read wfq from queue %d to %d\n", start_i, end_i-1 );
    }

    for(i=start_i; i<end_i; i++)
    {
        ppa_memset( &info, sizeof(info), 0 );
        info.portid = pdata->qos_wfq_info.portid;
        info.queueid = i;

        if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_WFQ, &info) == PPA_CMD_OK ) )
        {
            if( !g_output_to_file  )
            {
                IFX_PPACMD_PRINT("  queue %2d wfq rate: %d\n", i, (unsigned int)info.weight );
            }
            else
            {
                /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                SaveDataToFile(g_output_filename, (char *)(&info), sizeof(info) );
            }
        }
    }
    return PPA_SUCCESS;
}

/*ppacmd getwfq ---end*/
//#endif  //enod of CONFIG_PPA_QOS_WFQ


//#ifdef CONFIG_PPA_QOS_RATE_SHAPING
/*ppacmd setctrlrate ---begin*/
static void ppa_set_ctrl_rate_help( int summary)
{
    IFX_PPACMD_PRINT("setctrlrate [-p portid] [-i ifname] <-c enable | disable>\n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_set_ctrl_rate_short_opts[] = "p:c:i:h";
static const struct option ppa_set_ctrl_rate_long_opts[] =
{
    {"portid",  required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"control",   required_argument,  NULL, 'c'},
    { 0,0,0,0 }
};
static int ppa_parse_set_ctrl_rate_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int c_opt=0;
    int p_opt=0, i_opt=0;
    uint32_t res;
    ppa_memset( pdata, sizeof(PPA_CMD_QOS_CTRL_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_ctrl_info.portid = res;
		p_opt ++;
		break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_ctrl_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'c':
		if( strcmp("enable" , popts->optarg) == 0 )
			pdata->qos_ctrl_info.enable = 1;
		else if( strcmp("enable" , popts->optarg) == 0 )
			pdata->qos_ctrl_info.enable = 0;
		else {
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return PPA_CMD_ERR;
		}
		c_opt = 1;
		break;
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( c_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter control option\n");
        return PPA_CMD_ERR;
    }
    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_set_ctrl_rate_cmd: portid=%d, ctrl=%s\n", (unsigned int)pdata->qos_ctrl_info.portid, pdata->qos_ctrl_info.enable ? "enabled":"disable");

    return PPA_CMD_OK;
}
/*ppacmd setctrlrate ---end*/

/*ppacmd getctrlrate ---begin*/
static void ppa_get_ctrl_rate_help( int summary)
{
    IFX_PPACMD_PRINT("getctrlrate [-p portid] [-i ifname] [-o outfile]\n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_get_ctrl_rate_short_opts[] = "p:i:h";
static const struct option ppa_get_ctrl_rate_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_ctrl_rate_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    int p_opt=0, i_opt=0;

    ppa_memset( pdata, sizeof(PPA_CMD_QOS_CTRL_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qos_ctrl_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_opt ++;
            break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_ctrl_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }
    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("ppa_parse_get_ctrl_rate_cmd: portid=%d\n", (unsigned int)pdata->qos_ctrl_info.portid);

    return PPA_CMD_OK;
}
static void ppa_print_get_ctrl_rate_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The rate of port id %d is %s\n", (unsigned int)pdata->qos_ctrl_info.portid, pdata->qos_ctrl_info.enable ? "enabled":"disabled");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->qos_ctrl_info), sizeof(pdata->qos_ctrl_info) );
    }
}
/*ppacmd getctrlrate ---end*/

/*ppacmd setrate ---begin*/
static void ppa_set_rate_help( int summary)
{
    IFX_PPACMD_PRINT("setrate [-p portid] [-i ifname] [-q queuid | -g gamma_itf_id ] [-s shaperid | -r rate ] <-b burst>\n");
    IFX_PPACMD_PRINT("set '-q = -1' for setting Port Rate Shaper \n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_set_rate_short_opts[] = "p:i:q:g:s:r:b:h";

static const struct option ppa_set_rate_long_opts[] =
{
    {"portid",   required_argument,  NULL,   'p'},
    {"ifname",   required_argument,  NULL,   'i'},
    {"queuid",   required_argument,  NULL,   'q'},
	{"gammaitf", required_argument,  NULL,   'g'},
    {"shaperid", required_argument,  NULL,	 's'},
    {"rate",     required_argument,  NULL,   'r'},
    {"burst",    required_argument,  NULL,   'b'},
    { 0,0,0,0 }
};

static int ppa_set_rate_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA*pdata)
{
    unsigned int r_opt=0, q_opt=0;
    unsigned int s_opt=0;
    int p_opt=0, i_opt=0, res = 0;
    ppa_memset( pdata, sizeof(PPA_CMD_RATE_INFO), 0 );

    q_opt = 1;  //by default if no queue id is specified, it will be regarded as port based rate shaping.
    pdata->qos_rate_info.queueid = -1;
    pdata->qos_rate_info.shaperid = -1;
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.portid = res;
		p_opt ++;
		break;
        case 'i':
	    strcpy_s(pdata->qos_rate_info.ifname,PPA_IF_NAME_SIZE,(char *)popts->optarg);
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_rate_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.queueid = res;
		q_opt ++;
		break;
	case 'g':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.queueid = res;
		pdata->qos_rate_info.queueid = ~ pdata->qos_rate_info.queueid; //if queueid is bigger than max allowed queueid, it is regarded as port id
		q_opt ++;
		break;
        case 's':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.shaperid = res;
		s_opt ++;
		break;
        case 'r':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.rate    = res;
		r_opt = 1;
		break;
        case 'b':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.burst   = res;
		r_opt = 1;
		break;
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( q_opt > 2 )
    {
        IFX_PPACMD_PRINT("Queue id and gamma interface id cannot both be set id\n");
        return PPA_CMD_ERR;
    }
	if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }   

    if( (s_opt == 1) && (r_opt == 1) )
    {
        IFX_PPACMD_PRINT(" Both Shaper id and Rate cannot be set\n");
        return PPA_CMD_ERR;
    }
    
    if( (s_opt == 0) && (r_opt == 0) )
    {
        IFX_PPACMD_PRINT("Either Shaper id or Rate should be set\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_set_rate_cmd: portid=%d, queueid=%d,rate=%d burst=%d\n", 
	         (unsigned int)pdata->qos_rate_info.portid, 
			 (unsigned int)pdata->qos_rate_info.queueid, 
			 (unsigned int)pdata->qos_rate_info.rate, 
			 (unsigned int)pdata->qos_rate_info.burst);

    return PPA_CMD_OK;
}
/*ppacmd setrate ---end*/

/*ppacmd resetrate ---begin*/
static void ppa_reset_rate_help( int summary)
{
    IFX_PPACMD_PRINT("resetrate [-p portid] [-i ifname] [ <-q queuid> | -g <gamma interface id> ] \n");
    IFX_PPACMD_PRINT("Use '-q = -1' to reset port rate shaper \n");
    if( summary )
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
    return;
}
static const char ppa_reset_rate_short_opts[] = "p:q:i:h";
static const struct option ppa_reset_rate_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queuid",   required_argument,  NULL, 'q'},
    { 0,0,0,0 }
};
static int ppa_parse_reset_rate_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int q_opt=0;
    int p_opt=0, i_opt=0, res = 0;
    ppa_memset( pdata, sizeof(PPA_CMD_RATE_INFO), 0 );
    pdata->qos_rate_info.burst = -1;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.portid = res;
		p_opt ++;
		break;
        case 'i':
	    strcpy_s(pdata->qos_rate_info.ifname,PPA_IF_NAME_SIZE,(char *)popts->optarg);
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_rate_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.queueid = res;
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.shaperid = res;
		pdata->qos_rate_info.shaper.enable = 1;
		q_opt ++ ;
		break;
            
        case 'g':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.queueid = res;
		pdata->qos_rate_info.queueid = ~ pdata->qos_rate_info.queueid; //if queueid is bigger than max allowed queueid, it is regarded as port id
		q_opt ++;
		break;
        default:
            IFX_PPACMD_PRINT("ppa_parse_reset_rate_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( q_opt > 2 )
    {
        IFX_PPACMD_PRINT("Queue id and gamma interface id cannot both be set id\n");
        return PPA_CMD_ERR;
    }

    if( q_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need enter queueid\n");
        return PPA_CMD_ERR;
    }
    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_reset_rate_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_rate_info.portid, (unsigned int)pdata->qos_rate_info.queueid);

    return PPA_CMD_OK;
}
/*ppacmd resetrate ---end*/

/*ppacmd getrate ---begin*/
static void ppa_get_rate_help( int summary)
{
    IFX_PPACMD_PRINT("getrate [-p portid] [-i ifname] <-q queuid> [-o outfile]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        IFX_PPACMD_PRINT("    if queueid is not provided, then get all queue's rate information\n");
    }
    return;
}
static const char ppa_get_rate_short_opts[] = "p:q:o:i:h";
static const struct option ppa_get_rate_long_opts[] =
{
    {"portid",   required_argument,  NULL, 'p'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"queuid",   required_argument,  NULL, 'q'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_rate_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int q_opt=0;
    int p_opt=0, i_opt=0;
    ppa_memset( pdata, sizeof(PPA_CMD_RATE_INFO), 0 );
    pdata->qos_rate_info.queueid = -1;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
            pdata->qos_rate_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_opt ++;
            break;
        case 'i':
            if( get_portid(popts->optarg) >= 0 )
            {
                pdata->qos_rate_info.portid = get_portid(popts->optarg);
                i_opt ++;
            }
            else
            {
                IFX_PPACMD_PRINT("The portid of %s is not exist.\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;
        case 'q':
            pdata->qos_rate_info.queueid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            q_opt = 1;
            break;
        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_parse_get_rate_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( p_opt + i_opt != 1)
    {
        IFX_PPACMD_PRINT("note: one or only one of portid and ifname must be specified\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_get_rate_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_rate_info.portid, (unsigned int)pdata->qos_rate_info.queueid);

    return PPA_CMD_OK;
}

static int ppa_get_rate_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_QUEUE_NUM_INFO qnum_info;
    PPA_CMD_RATE_INFO  info;
    int i=0, j=0, start_i, end_i=0;
    int res;

    ppa_memset( &qnum_info, sizeof(qnum_info), 0 );
    qnum_info.portid = pdata->qos_rate_info.portid;
    if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_QUEUE_MAX_NUM, &qnum_info ) != PPA_CMD_OK ) )
    {
        IFX_PPACMD_PRINT("ioctl PPA_CMD_GET_QOS_QUEUE_MAX_NUM fail\n");
        return PPA_FAILURE;
    }

    if( pdata->qos_rate_info.queueid!= -1 ) // set index already, then use it
    {
        ppa_memset( &info, 0, sizeof(info) );
        info.portid = pdata->qos_rate_info.portid;
        info.queueid = pdata->qos_rate_info.queueid;

        if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_RATE, &info) == PPA_CMD_OK ) )
        {
            if( !g_output_to_file  )
            {
                IFX_PPACMD_PRINT("              Rate           Burst      ShaperId\n");

                if( pdata->qos_rate_info.queueid < qnum_info.queue_num )
                    IFX_PPACMD_PRINT("   queue %2d:  %08d(kbps) 0x%04d       %d\n", (unsigned int)info.queueid, (unsigned int)info.rate, (unsigned int)info.burst, (unsigned int)info.shaperid );
                else
                    IFX_PPACMD_PRINT("   port    :  %08d(kbps) 0x%04d\n", (unsigned int)info.rate, (unsigned int)info.burst );
            }
            else
            {
                /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                SaveDataToFile(g_output_filename, (char *)(&info), sizeof(info) );
            }
            j++;
        }
    }
    else
    {
        start_i = 0;
        end_i = qnum_info.queue_num;
        IFX_PPACMD_DBG("Need to read rate shaping from queue %d to %d\n", start_i, end_i-1 );

        for(i=start_i; i<end_i; i++)
        {
            ppa_memset( &info, sizeof(info), 0 );
            info.portid = pdata->qos_rate_info.portid;
            info.queueid = i;

            if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_RATE, &info) == PPA_CMD_OK ) )
            {
                if( !g_output_to_file  )
                {
                    if( i == start_i )
                        IFX_PPACMD_PRINT("              Rate          Burst      ShaperId\n");
                        IFX_PPACMD_PRINT("   queue %2d:  %07d(kbps) 0x%04d      %d\n", i, (unsigned int)info.rate, (unsigned int)info.burst, (unsigned int)info.shaperid );
                }
                else
                {
                    /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                    SaveDataToFile(g_output_filename, (char *)(&info), sizeof(info) );
                }
                j++;
            }
        }

        if( pdata->qos_rate_info.queueid == -1 )
        {
            ppa_memset( &info, sizeof(info),0 );
            info.portid = pdata->qos_rate_info.portid;
            info.queueid = qnum_info.queue_num;

            if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_RATE, &info) == PPA_CMD_OK ) )
            {
                if( !g_output_to_file  )
                {
                    IFX_PPACMD_PRINT("   port   :   %07d(kbps) 0x%04d\n", (unsigned int)info.rate, (unsigned int)info.burst );
                }
                else
                {
                    /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                    SaveDataToFile(g_output_filename, (char *)(&info), sizeof(info) );
                }
                j++;
            }
        }
    }




    return PPA_SUCCESS;
}

/*ppacmd setshaper ---begin*/
static void ppa_set_shaper_help( int summary)
{
    IFX_PPACMD_PRINT("setshaper <-i ifname> <-s shaperid> <-r rate/PIR> <-c rate/CIR> <-b burst/PBS/CBS> <-m shapermode>\n");
    IFX_PPACMD_PRINT("      Use '-s = -1' to create Port rate shaper instance\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: shaperid and rate must be specified\n");
        IFX_PPACMD_PRINT("      option -s: Shaper id, valid range -1 to 7\n");
    }
    return;
}
static const char ppa_set_shaper_short_opts[] = "i:s:r:c:b:m";
static const struct option ppa_set_shaper_long_opts[] =
{
    {"ifname", required_argument,  NULL,   'i'},
    {"shaperid", required_argument,  NULL,   's'},
    {"rate",     required_argument,  NULL,   'r'},
    {"commitrate",     required_argument,  NULL,   'c'},
    {"burst",    required_argument,  NULL,   'b'},
    {"shapermode",     required_argument,  NULL,   'm'},
    { 0,0,0,0 }
};

static int ppa_set_shaper_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA*pdata)
{
    unsigned int r_opt=0, s_opt=0,b_opt=0;
    int res = 0;

    ppa_memset( pdata, sizeof(PPA_CMD_RATE_INFO), 0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
	    strcpy_s(pdata->qos_rate_info.ifname,PPA_IF_NAME_SIZE,(char *)popts->optarg);
            pdata->qos_rate_info.portid = 15;
	    break;
        case 's':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.shaperid = res;
		pdata->qos_rate_info.shaper.enable = 1;
		s_opt ++;
		break;
        case 'r':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.rate = pdata->qos_rate_info.shaper.pir   = res;
		r_opt = 1;
		break;
        case 'c':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.shaper.cir   = res;
		r_opt = 1;
		break;
        case 'b':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.burst = pdata->qos_rate_info.shaper.pbs = pdata->qos_rate_info.shaper.cbs = res;
		b_opt = 1;
		break;
        case 'm':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_rate_info.shaper.mode  = res;
		break;
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( r_opt == 0 )
    {
        IFX_PPACMD_PRINT("Need to enter rate\n");
        return PPA_CMD_ERR;
    }
        
    if( s_opt == 0 ) 
    {
        IFX_PPACMD_PRINT("Shaper id is required\n");
        return PPA_CMD_ERR;
    }
#if 0
    if( (pdata->qos_rate_info.shaperid < 0) || (pdata->qos_rate_info.shaperid > 7) ) 
    {
        IFX_PPACMD_PRINT("Shaper id valid range is 0 to 7\n");
        return PPA_CMD_ERR;
    }
#endif
    IFX_PPACMD_DBG("ppa_set_shaper_cmd: shaperid=%d,rate=%d burst=%d\n", 
	       (unsigned int)pdata->qos_rate_info.shaperid, 
			 (unsigned int)pdata->qos_rate_info.rate, 
			 (unsigned int)pdata->qos_rate_info.burst);

    return PPA_CMD_OK;
}
/*ppacmd setshaper ---end*/

/*ppacmd getshaper ---begin*/
static void ppa_get_shaper_help( int summary)
{
    IFX_PPACMD_PRINT("getshaper <-s shaperid> [-o outfile]\n");
    if( summary )
    {
        IFX_PPACMD_PRINT("note: Shaper id must be specified\n");
        IFX_PPACMD_PRINT("      option -s: Shaper id, valid range 0 to 7\n");
    }
    return;
}
static const char ppa_get_shaper_short_opts[] = "s:o";
static const struct option ppa_get_shaper_long_opts[] =
{
    {"shaperid",   required_argument,  NULL, 's'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_parse_get_shaper_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int s_opt=0;
    ppa_memset( pdata, sizeof(PPA_CMD_RATE_INFO), 0 );
    pdata->qos_rate_info.queueid = -1;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 's':
            pdata->qos_rate_info.shaperid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            s_opt = 1;
            break;
        case 'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_parse_get_shaper_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( s_opt != 1)
    {
        IFX_PPACMD_PRINT("note: Shaper id must be specified\n");
        return PPA_CMD_ERR;
    }

    if( (pdata->qos_rate_info.shaperid < 0) || (pdata->qos_rate_info.shaperid > 7) ) 
    {
        IFX_PPACMD_PRINT("Shaper id valid range is 0 to 7\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_get_shaper_cmd: shaperid=%d\n", (unsigned int)pdata->qos_rate_info.shaperid );

    return PPA_CMD_OK;
}

static int ppa_get_shaper_do_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_RATE_INFO  info;
    int res;

    {
        ppa_memset( &info, sizeof(info), 0 );
        info.shaperid = pdata->qos_rate_info.shaperid;

        if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_QOS_SHAPER, &info) == PPA_CMD_OK ) )
        {
            if( !g_output_to_file  )
            {
                IFX_PPACMD_PRINT("    ShaperId  Rate              Burst\n");

                IFX_PPACMD_PRINT("     %d        %08d(kbps)    0x%04d\n", (unsigned int)info.shaperid, (unsigned int)info.rate, (unsigned int)info.burst );
            }
            else
            {
                /*note, currently only last valid flow info is saved to file and all other flow informations are all overwritten */
                SaveDataToFile(g_output_filename, (char *)(&info), sizeof(info) );
            }
        }
    }
    
    return PPA_SUCCESS;
}
/*ppacmd getshaper ---end*/

/*ppacmd getrate ---end*/
//#endif  //enod of CONFIG_PPA_QOS_RATE_SHAPING


#endif /*end if CONFIG_PPA_QOS */


/*ppacmd gethook: get all exported hook number  ---begin*/
static void ppa_get_hook_count_help( int summary)
{
    IFX_PPACMD_PRINT("gethooknum [-o outfile]\n");
    return;
}

static const char ppa_get_hook_short_opts[] = "o:h";
static const struct option ppa_get_hook_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};

static void ppa_print_get_hook_count_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("There are %u exported hook \n", (unsigned int) pdata->count_info.count);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->count_info), sizeof(pdata->count_info) );
    }
}
/*ppacmd gethook: get all exported hook number  ---end*/

/*ppacmd gethooklist: get all exported hook list  ---begin*/
static void ppa_get_hook_list_help( int summary)
{
    IFX_PPACMD_PRINT("gethooklist [-o outfile]\n");
    return;
}

static const char ppa_get_hook_list_short_opts[] = "o:h";
static const struct option ppa_get_hook_list_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
static int ppa_get_hook_list_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_DATA cmd_info;
    PPA_CMD_HOOK_LIST_INFO *phook_list;
    int res = PPA_CMD_OK, i, size;

    //get session count first before malloc memroy
    cmd_info.count_info.flag = 0;
    cmd_info.count_info.count = 0;
    if( ppa_do_ioctl_cmd(PPA_CMD_GET_HOOK_COUNT, &cmd_info ) != PPA_CMD_OK )
    {
        IFX_PPACMD_DBG("ppa_do_ioctl_cmd: PPA_CMD_GET_HOOK_COUNT failed\n");
        return -EIO;
    }

    if( cmd_info.count_info.count <= 0 )
    {
        IFX_PPACMD_DBG("Note, there is no exported PPA hooks at all\n");
        return -EIO;
    }
    IFX_PPACMD_DBG("There are %u hook mapped\n", (unsigned int) cmd_info.count_info.count );

    //malloc memory and set value correctly
    size = sizeof(PPA_CMD_HOOK_LIST_INFO) + cmd_info.count_info.count * sizeof(PPA_HOOK_INFO);
    phook_list = (PPA_CMD_HOOK_LIST_INFO *) malloc( size );
    if( phook_list == NULL )
    {
        IFX_PPACMD_DBG("Malloc failed in ppa_get_hook_list_cmd\n");
        return PPA_CMD_NOT_AVAIL;
    }

    phook_list->hook_count = cmd_info.count_info.count;
    phook_list->flag = 0;

    //get session information
    if( (res = ppa_do_ioctl_cmd(PPA_CMD_GET_HOOK_LIST, phook_list ) != PPA_CMD_OK ) )
    {
        free( phook_list );
        IFX_PPACMD_DBG("ppa_get_hook_list_cmd ioctl  failed\n");
        return res;
    }

    if( !g_output_to_file )
    {
        for(i=0; i<phook_list->hook_count; i++)
        {
            if( phook_list->list[i].used_flag )
                IFX_PPACMD_PRINT("%02d: %-50s( hook address  0x%x: %s)\n", i,  phook_list->list[i].hookname,(unsigned int) phook_list->list[i].hook_addr,  phook_list->list[i].hook_flag?"enabled":"disabled");
            else
                IFX_PPACMD_PRINT("%02d: free item\n", i);
        }
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(phook_list), size );
    }

    free( phook_list );
    return PPA_CMD_OK;
}
/*ppacmd gethooklist: get all exported hook list  ---end*/

/*ppacmd sethook: set hook to enable/dsiable  ---begin*/
static void ppa_set_hook_help( int summary)
{
    IFX_PPACMD_PRINT("sethook <-n hookname > <-c enable | disable >\n");
    return;
}

static const char ppa_set_hook_short_opts[] = "n:c:h";
static const struct option ppa_set_hook_long_opts[] =
{
    {"hookname",   required_argument,  NULL, 'n'},
    {"control",   required_argument,  NULL, 'c'},
    { 0,0,0,0 }
};

static int ppa_parse_set_hook_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int hookname_opt = 0, control_opt = 0;

    ppa_memset( &pdata->hook_control_info, sizeof(pdata->hook_control_info), 0) ;

    while(popts->opt)
    {
        if (popts->opt == 'n')
        {
            strncpy_s((char * )pdata->hook_control_info.hookname,sizeof(pdata->hook_control_info.hookname),  popts->optarg, sizeof(pdata->hook_control_info.hookname) );
            pdata->hook_control_info.hookname[sizeof(pdata->hook_control_info.hookname)-1] = 0;
            hookname_opt++;
        }
        else if (popts->opt == 'c')
        {
            if( stricmp( popts->optarg, "enable" ) == 0 )
                pdata->hook_control_info.enable = 1;
            control_opt++;
        }
        else
        {
            return PPA_CMD_ERR;
        }

        popts++;
    }

    if (hookname_opt != 1 || control_opt >=2)
        return PPA_CMD_ERR;

    IFX_PPACMD_DBG("sethook: try to %s  %s \n", pdata->hook_control_info.enable ? "enable":"disable", pdata->hook_control_info.hookname);
    return PPA_CMD_OK;
}
/*ppacmd sethook: set hook to enable/dsiable  ---end*/

/*ppacmd r ( getmem ) ---begin*/
static void ppa_get_mem_help( int summary)
{
    if( !summary )
        IFX_PPACMD_PRINT("r <-a address> [-n repeat] [-f offset] [-s size] [-o outfile]\n");
    else
    {
        IFX_PPACMD_PRINT("r <-a address> [-n repeat] [-f offset] [-s size] [-o outfile]\n");
        IFX_PPACMD_PRINT("    Note: offset + size must equal to 32\n");
        IFX_PPACMD_PRINT("    Note: address must align to 4 bytes address and repeation is based on 4 bytes address\n");
    }
    return;
}

static const char ppa_get_mem_short_opts[] = "a:f:s:p:n:o:h";
static const struct option ppa_get_mem_long_opts[] =
{
    {"address",   required_argument,  NULL, 'a'},
    {"offset",   required_argument,  NULL, 'f'},
    {"size",   required_argument,  NULL, 's'},
    {"repeat",   required_argument,  NULL, 'n'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};

static int ppa_parse_get_mem_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt=0, f_opt=0, s_opt=0, n_opt=0;

    ppa_memset( &pdata->read_mem_info, sizeof(pdata->read_mem_info), 0 );
    pdata->read_mem_info.repeat = 1; /*default read only */
    pdata->read_mem_info.size = MAX_MEM_OFFSET;  /*default read 32 bits */
    pdata->read_mem_info.shift= MAX_MEM_OFFSET - pdata->read_mem_info.size ;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'a':
            pdata->read_mem_info.addr = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            a_opt ++;
            break;
        case 'f':
            pdata->read_mem_info.shift = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
			if (pdata->read_mem_info.shift > MAX_MEM_OFFSET) {
				IFX_PPACMD_PRINT("Invalid shift value\n");
				return PPA_CMD_ERR;
			}
            if( pdata->read_mem_info.size >= MAX_MEM_OFFSET - pdata->read_mem_info.shift )
                pdata->read_mem_info.size = MAX_MEM_OFFSET - pdata->read_mem_info.shift;
            f_opt ++;
            break;
        case 's':
            pdata->read_mem_info.size = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
			if (pdata->read_mem_info.size > MAX_MEM_OFFSET) {
				IFX_PPACMD_PRINT("Invalid size value\n");
				return PPA_CMD_ERR;
			}
            if( pdata->read_mem_info.shift > MAX_MEM_OFFSET - pdata->read_mem_info.size )
                pdata->read_mem_info.shift= MAX_MEM_OFFSET - pdata->read_mem_info.size;
            s_opt ++;
            break;
        case 'n':
            pdata->read_mem_info.repeat = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            n_opt ++;
            break;
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME,popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("r not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( a_opt != 1 || f_opt >= 2 || s_opt >= 2 || n_opt >= 2)
    {
        IFX_PPACMD_PRINT("Wrong parameter\n");
        return PPA_CMD_ERR;
    }
    if( pdata->read_mem_info.size == 0 )
    {
        pdata->read_mem_info.size = MAX_MEM_OFFSET - pdata->read_mem_info.shift;
    }
    if( pdata->read_mem_info.size > MAX_MEM_OFFSET )
    {
        IFX_PPACMD_PRINT("Wrong size value: %d ( it should be 1 ~ 32 )\n", (unsigned int)pdata->read_mem_info.size);
        return PPA_CMD_ERR;
    }
    if( pdata->read_mem_info.shift >= MAX_MEM_OFFSET )
    {
        IFX_PPACMD_PRINT("Wrong shift value: %d ( it should be 0 ~ 31 )\n", (unsigned int)pdata->read_mem_info.shift);
        return PPA_CMD_ERR;
    }

    if( pdata->read_mem_info.shift + pdata->read_mem_info.size > MAX_MEM_OFFSET )
    {
        IFX_PPACMD_PRINT("Wrong shift/size value: %d/%d (The sum should <= 32 ))\n", (unsigned int)pdata->read_mem_info.shift, (unsigned int)pdata->read_mem_info.size);
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("r: address=0x%x, offset=%d, size=%d, repeat=%d\n", (unsigned int )pdata->read_mem_info.addr, (unsigned int )pdata->read_mem_info.shift, (unsigned int )pdata->read_mem_info.size, (unsigned int )pdata->read_mem_info.repeat  );

    return PPA_CMD_OK;
}

static int ppa_get_mem_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
    PPA_CMD_READ_MEM_INFO *buffer;
    int i, size;
    int num_per_line=4;  //4 double words

    size = sizeof(pdata->read_mem_info) + sizeof(pdata->read_mem_info.buffer) * pdata->read_mem_info.repeat ;
    buffer = (PPA_CMD_READ_MEM_INFO *) malloc( size );
    if( buffer == NULL )
    {
        IFX_PPACMD_PRINT("Malloc %d bytes failed\n", size );
        return PPA_CMD_NOT_AVAIL;
    }
    ppa_memset( buffer, sizeof(size), 0 );

    memcpy_s( (void *)buffer,size, &pdata->read_mem_info, sizeof(pdata->read_mem_info) );
    buffer->flag = 0;

    //get session information
    if(  ppa_do_ioctl_cmd(PPA_CMD_READ_MEM, buffer ) != PPA_CMD_OK )
    {
        free( buffer );
        return PPA_FAILURE;
    }

    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The value from address 0x%x(0x%x):----------------------------", (unsigned int)buffer->addr, (unsigned int)buffer->addr_mapped);
        int tmp_filling_size = buffer->addr%16;
        if( tmp_filling_size != 0 )
        {
            IFX_PPACMD_PRINT("\n%08x: ", (unsigned int )buffer->addr/16 * 16 );
            for(i=0; i<tmp_filling_size/4; i++)
            {
                if( buffer->size <= 4 )
                {
                    IFX_PPACMD_PRINT( "  ");   // 2
                }
                else if( buffer->size <=8 )
                {
                    IFX_PPACMD_PRINT( "   " ); // 3
                }
                else if( buffer->size <=12 )
                {
                    IFX_PPACMD_PRINT( "    "); // 4
                }
                else if( buffer->size <=16 )
                {
                    IFX_PPACMD_PRINT( "     "); // 5
                }
                else if( buffer->size <=20 )
                {
                    IFX_PPACMD_PRINT( "      "); // 6
                }
                else if( buffer->size <=24 )
                {
                    IFX_PPACMD_PRINT( "       "); // 7
                }
                else if( buffer->size <=28 )
                {
                    IFX_PPACMD_PRINT( "         "); // 8
                }
                else
                {
                    IFX_PPACMD_PRINT( "         "); // 9
                }
            }
        }

        for(i=0;  i<buffer->repeat; i++ )
        {
            if( ( i+ tmp_filling_size/4) %num_per_line == 0 )
                IFX_PPACMD_PRINT("\n%08x: ", (unsigned int )buffer->addr + i * 4 );

            if( buffer->size <= 4 )
            {
                IFX_PPACMD_PRINT( "%01x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=8 )
            {
                IFX_PPACMD_PRINT( "%02x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=12 )
            {
                IFX_PPACMD_PRINT( "%03x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=16 )
            {
                IFX_PPACMD_PRINT( "%04x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=20 )
            {
                IFX_PPACMD_PRINT( "%05x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=24 )
            {
                IFX_PPACMD_PRINT( "%06x ", (unsigned int )buffer->buffer[i]  );
            }
            else if( buffer->size <=28 )
            {
                IFX_PPACMD_PRINT( "%07x ", (unsigned int )buffer->buffer[i] );
            }
            else
            {
                IFX_PPACMD_PRINT( "%08x ", (unsigned int )buffer->buffer[i]  );
            }
        }
        IFX_PPACMD_PRINT("\n" );
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->read_mem_info), sizeof(pdata->read_mem_info) );
    }

    free( buffer );
    return PPA_SUCCESS;
}
/*ppacmd r ( getmem ) ---end*/

/*ppacmd w ( setmem ) ---begin*/
static void ppa_set_mem_help( int summary)
{
    if( !summary )
        IFX_PPACMD_PRINT("w <-a address>  <-v value> [-n repeat] [-f offset] [-s size] \n");
    else
    {
        IFX_PPACMD_PRINT("w <-a address>  <-v value> [-n repeat] [-f offset] [-s size] \n");
        IFX_PPACMD_PRINT("    Note: offset + size must equal to 32\n");
        IFX_PPACMD_PRINT("    Note: address must align to 4 bytes address and repeation is based on 4 bytes address\n");
    }
    return;
}
static const char ppa_set_mem_short_opts[] = "a:f:s:v:n:p:h";
static const struct option ppa_set_mem_long_opts[] =
{
    {"address",   required_argument,  NULL, 'a'},
    {"offset",   required_argument,  NULL, 'f'},
    {"size",   required_argument,  NULL, 's'},
    {"repeat",   required_argument,  NULL, 'n'},
    {"value",   required_argument,  NULL, 'v'},
    { 0,0,0,0 }
};
static int ppa_parse_set_mem_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt=0, f_opt=0, s_opt=0, n_opt=0, v_opt=0;

    ppa_memset( &pdata->set_mem_info, sizeof(pdata->set_mem_info), 0 );
    pdata->set_mem_info.repeat = 1; /*default read only */
    pdata->set_mem_info.size = MAX_MEM_OFFSET;  /*default read 32 bits */
    pdata->set_mem_info.shift= 0;
    pdata->set_mem_info.value= 0;

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'a':
            pdata->set_mem_info.addr = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            a_opt ++;
            break;
        case 'f':
            pdata->set_mem_info.shift = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
			if (pdata->set_mem_info.shift > MAX_MEM_OFFSET) {
				IFX_PPACMD_PRINT("Invalid shift value\n");
				return PPA_CMD_ERR;
			}
            if( pdata->set_mem_info.size > MAX_MEM_OFFSET - pdata->set_mem_info.shift )
                pdata->set_mem_info.size = MAX_MEM_OFFSET - pdata->set_mem_info.shift;
            f_opt ++;
            break;
        case 's':
            pdata->set_mem_info.size = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
			if (pdata->set_mem_info.size > MAX_MEM_OFFSET) {
				IFX_PPACMD_PRINT("Invalid Size value\n");
				return PPA_CMD_ERR;
			}
            if( pdata->set_mem_info.shift > MAX_MEM_OFFSET - pdata->set_mem_info.size )
                pdata->set_mem_info.shift= MAX_MEM_OFFSET - pdata->set_mem_info.size;
            s_opt ++;
            break;
        case 'n':
            pdata->set_mem_info.repeat = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            n_opt ++;
            break;
        case  'v':
            pdata->set_mem_info.value = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            v_opt ++;
            break;
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( a_opt != 1 || f_opt >= 2 || s_opt >= 2 || n_opt >= 2 || v_opt != 1)
    {
        IFX_PPACMD_PRINT("Wrong parameter\n");
        return PPA_CMD_ERR;
    }
    if( pdata->set_mem_info.size == 0 )
    {
        pdata->set_mem_info.size = MAX_MEM_OFFSET - pdata->set_mem_info.shift;
    }
    if(  pdata->set_mem_info.size > MAX_MEM_OFFSET )
    {
        IFX_PPACMD_PRINT("Wrong size value: %d ( it should be 1 ~ 32 )\n", (unsigned int)pdata->set_mem_info.size);
        return PPA_CMD_ERR;
    }
    if( pdata->set_mem_info.shift >= MAX_MEM_OFFSET )
    {
        IFX_PPACMD_PRINT("Wrong shift value: %d ( it should be 0 ~ 31 )\n", (unsigned int)pdata->set_mem_info.shift);
        return PPA_CMD_ERR;
    }

    if( pdata->set_mem_info.shift + pdata->set_mem_info.size > MAX_MEM_OFFSET)
    {
        IFX_PPACMD_PRINT("Wrong shift/size value: %d/%d (The sum should <= 32 ))\n", (unsigned int)pdata->set_mem_info.shift, (unsigned int)pdata->set_mem_info.size);
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("w: address=0x%x, offset=%d, size=%d, value=0x%x, repeat=%d\n", (unsigned int )pdata->set_mem_info.addr, (unsigned int )pdata->set_mem_info.shift, (unsigned int )pdata->set_mem_info.size, (unsigned int ) pdata->set_mem_info.value , (unsigned int )  pdata->set_mem_info.repeat );

    return PPA_CMD_OK;
}

static void ppa_print_set_mem_cmd( PPA_CMD_DATA *pdata)
{
    IFX_PPACMD_PRINT("Write memory from address 0x%x(0x%x) to 0x%x(0x%x) with value 0x%x\n",
                     (unsigned int)pdata->set_mem_info.addr,
                     (unsigned int)pdata->set_mem_info.addr_mapped,
                     (unsigned int)(pdata->set_mem_info.addr+pdata->set_mem_info.repeat),
                     (unsigned int)(pdata->set_mem_info.addr_mapped+pdata->set_mem_info.repeat),
                     (unsigned int)pdata->set_mem_info.value );

    return;
}

/*ppacmd w ( setmem ) ---end*/


/*ppacmd sethaldbg (  ) ---begin*/
static void ppa_set_hal_dbg_help( int summary)
{
    IFX_PPACMD_PRINT("sethaldbg -f < 1 | 0>\n");
    return;
}
static const char ppa_set_hal_dbg_short_opts[] = "f:h";
static const struct option ppa_set_hal_dbg_long_opts[] =
{
    {"flag",   required_argument,  NULL, 'f'},
    { 0,0,0,0 }
};
static int ppa_parse_set_hal_flag_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int f_opt=0;
    int res = 0;

    ppa_memset( &pdata->genernal_enable_info, sizeof(pdata->genernal_enable_info), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'f':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		if (res != 0 && res != 1) {
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return PPA_CMD_ERR;
		}
		pdata->genernal_enable_info.enable = res;
		f_opt ++;
		break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( f_opt != 1 )
    {
        IFX_PPACMD_PRINT("Wrong parameter\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("sethaldbg: enable=0x%x\n",  (unsigned int )  pdata->genernal_enable_info.enable);

    return PPA_CMD_OK;
}

/*ppacmd sethaldbg ---end*/

/*ppacmd ppa_get_portid ---begin */
static const char ppa_get_portid_short_opts[] = "i:h";
static const struct option ppa_get_portid_long_opts[] =
{
    {"ifname",   required_argument,  NULL, 'i'},
    { 0,0,0,0 }
};
static void ppa_get_portid_help( int summary)
{
    IFX_PPACMD_PRINT("getportid  <-i ifname>\n");
    return;
}

static int ppa_parse_get_portid_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int i_opt = 0;

    ppa_memset( pdata, sizeof(pdata->portid_info ),0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
            strncpy_s(pdata->portid_info.ifname,sizeof(pdata->portid_info.ifname), popts->optarg, sizeof(pdata->portid_info.ifname) );
            i_opt ++;
            break;

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( i_opt != 1)
    {
        IFX_PPACMD_PRINT("Wrong parameter.\n");
        return PPA_CMD_ERR;
    }
    return PPA_CMD_OK;
}

static void ppa_print_get_portid_cmd( PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The %s's port id is %d\n", pdata->portid_info.ifname, (unsigned int)pdata->portid_info.portid);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->portid_info), sizeof(pdata->portid_info) );
    }

    return;
}
/*ppacmd ppa_get_portid ---end */


/*ppacmd ppa_get_dsl_mib ---begin */
static void ppa_get_dsl_mib_help( int summary)
{
    IFX_PPACMD_PRINT("getdslmib\n");
    return;
}

static void ppa_print_get_dsl_mib_cmd( PPA_CMD_DATA *pdata)
{
    int i;

    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The DSL MIB info\n");
        IFX_PPACMD_PRINT("  DSL Queue DROP MIB\n");
        for(i=0; i<pdata->dsl_mib_info.mib.queue_num; i++ )
        {
            IFX_PPACMD_PRINT("    queue%02d: %10u", i, (unsigned int)pdata->dsl_mib_info.mib.drop_mib[i]);
            if( (i%4) == 3 ) IFX_PPACMD_PRINT("\n");
        }
        IFX_PPACMD_PRINT("\n");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->dsl_mib_info), sizeof(pdata->dsl_mib_info) );
    }

    return;
}
/****ppa_get_dsl_mib ----end */

/*ppacmd ppa_clear_dsl_mib ---begin */
static void ppa_clear_dsl_mib_help( int summary)
{
    IFX_PPACMD_PRINT("cleardslmib\n");
    return;
}
/****ppa_clear_dsl_mib ----end */

/*ppacmd ppa_port_mib ---begin */
static void ppa_get_port_mib_help( int summary)
{
    IFX_PPACMD_PRINT("getportmib\n");
    return;
}

void print_port_mib(PPA_PORT_MIB *mib)
{
#define PORT_MIB_TITLE_SAPCE   32
#define PORT_MIB_NUMBER_SAPCE   10
    int i;

    if( !mib) return;

    IFX_PPACMD_DBG("print_port_mib:%d\n", (int)mib->port_num);

    //print format     space    eth0  eth1   dsl    ext1  ext2  ext3
    IFX_PPACMD_PRINT("%28s %10s %10s %10s %10s %10s %10s\n", "MIB:", "ETH0", "ETH1", "DSL", "EXT1", "EXT2","EXT3");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_brg_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_brg_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_brg_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_brg_bytes);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv4_udp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_udp_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv4_tcp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_tcp_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv4_mc_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_mc_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv4_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_bytes);
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv6_udp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_udp_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv6_tcp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_tcp_pkts);
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv6_mc_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_mc_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_fast_rt_ipv6_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_bytes);
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_cpu_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_cpu_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_cpu_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_cpu_bytes);
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_drop_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_drop_pkts);
    }
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "ig_drop_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].ig_drop_bytes);
    }
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("\n");

    IFX_PPACMD_PRINT("%28s ", "eg_fast_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_PRINT("%10u ", (unsigned int) mib->mib_info[i].eg_fast_pkts);
    }
    IFX_PPACMD_PRINT("\n");

}

void print_port_mib_dbg(PPA_PORT_MIB *mib)
{
    int i;

    if( !mib) return;

    IFX_PPACMD_DBG("print_port_mib:%d\n", (int)mib->port_num);

    IFX_PPACMD_DBG("%28s ", "ig_fast_brg_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_brg_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_brg_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_brg_bytes);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv4_udp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_udp_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv4_tcp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_tcp_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv4_mc_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_mc_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv4_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv4_bytes);
    }
    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv6_udp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_udp_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv6_tcp_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_tcp_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_fast_rt_ipv6_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_fast_rt_ipv6_bytes);
    }
    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_cpu_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_cpu_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_cpu_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_cpu_bytes);
    }
    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_drop_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_drop_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "ig_drop_bytes:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].ig_drop_bytes);
    }
    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "eg_fast_pkts:");
    for(i=0; i<mib->port_num; i++ )
    {
        IFX_PPACMD_DBG("%10u ", (unsigned int) mib->mib_info[i].eg_fast_pkts);
    }
    IFX_PPACMD_DBG("\n");

    IFX_PPACMD_DBG("%28s ", "port flag   :");
    for(i=0; i<mib->port_num; i++ )
    {
        if( mib->mib_info[i].port_flag == PPA_PORT_MODE_ETH )
        {
            IFX_PPACMD_DBG("%10s ", "ETH");
        }
        else if( mib->mib_info[i].port_flag == PPA_PORT_MODE_DSL)
        {
            IFX_PPACMD_DBG("%10s ", "DSL");
        }
        else if( mib->mib_info[i].port_flag == PPA_PORT_MODE_EXT)
        {
            IFX_PPACMD_DBG("%10s ", "EXT");
        }
        else if( mib->mib_info[i].port_flag == PPA_PORT_MODE_CPU)
        {
            IFX_PPACMD_DBG("%10s ", "CPU");
        }
        else
        {
            IFX_PPACMD_DBG("%10s ", "Unknown");
        }
    }
    IFX_PPACMD_DBG("\n");
    IFX_PPACMD_DBG("\n");

}

static void ppa_print_get_port_mib_cmd( PPA_CMD_DATA *pdata)
{
    int i, k, num;
    PPA_PORT_MIB tmp_info;

    print_port_mib_dbg(&pdata->port_mib_info.mib);

    if( !g_output_to_file )
    {
        ppa_memset( &tmp_info, sizeof(tmp_info), 0);
        IFX_PPACMD_DBG("ppa_print_get_port_mib_cmd: There are %d ports in PPE FW\n", (int)pdata->port_mib_info.mib.port_num);

        //Get first two ethernet port: for amazon_se and Danube A4, there is no eth1. But we still keep space for it
        num = 0;
        for(i=0, k=0; i<pdata->port_mib_info.mib.port_num; i++)
        {
            if( pdata->port_mib_info.mib.mib_info[i].port_flag == PPA_PORT_MODE_ETH )
            {
                tmp_info.mib_info[num] = pdata->port_mib_info.mib.mib_info[i];
                k++;
                num ++;

                if( k == 2 ) break; //only accept 2 ethernet port
            }
        }
        IFX_PPACMD_DBG("There are %d ethernet port in PPE FW\n", k);

        //Get DSL port ( note, we only print all DSL mib's sum, esp for ATM IPOA/PPPOA and ATM EOA here
        num = 2;
        for(i=0, k=0; i<pdata->port_mib_info.mib.port_num; i++)
        {
            if( pdata->port_mib_info.mib.mib_info[i].port_flag == PPA_PORT_MODE_DSL )
            {
                tmp_info.mib_info[num].ig_fast_brg_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_brg_pkts;
                tmp_info.mib_info[num].ig_fast_brg_bytes += pdata->port_mib_info.mib.mib_info[i].ig_fast_brg_bytes;
                tmp_info.mib_info[num].ig_fast_rt_ipv4_udp_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv4_udp_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv4_tcp_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv4_tcp_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv4_mc_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv4_mc_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv4_bytes += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv4_bytes;
                tmp_info.mib_info[num].ig_fast_rt_ipv6_udp_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv6_udp_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv6_mc_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv6_mc_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv6_tcp_pkts += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv6_tcp_pkts;
                tmp_info.mib_info[num].ig_fast_rt_ipv6_bytes += pdata->port_mib_info.mib.mib_info[i].ig_fast_rt_ipv6_bytes;
                tmp_info.mib_info[num].ig_cpu_pkts+= pdata->port_mib_info.mib.mib_info[i].ig_cpu_pkts;
                tmp_info.mib_info[num].ig_cpu_bytes += pdata->port_mib_info.mib.mib_info[i].ig_cpu_bytes;
                tmp_info.mib_info[num].ig_drop_pkts += pdata->port_mib_info.mib.mib_info[i].ig_drop_pkts;
                tmp_info.mib_info[num].ig_drop_bytes += pdata->port_mib_info.mib.mib_info[i].ig_drop_bytes;
                tmp_info.mib_info[num].eg_fast_pkts += pdata->port_mib_info.mib.mib_info[i].eg_fast_pkts;

                tmp_info.mib_info[num].port_flag = PPA_PORT_MODE_DSL;
                k++;
                if( k == 2 ) break; //at most only two DSL port
            }
        }
        IFX_PPACMD_DBG("There are %d DSL port in PPE FW\n", k);

        //Get extension port
        num=3;
        for(i=0, k=0; i<pdata->port_mib_info.mib.port_num; i++)
        {
            if( pdata->port_mib_info.mib.mib_info[i].port_flag == PPA_PORT_MODE_EXT)
            {
                tmp_info.mib_info[num] = pdata->port_mib_info.mib.mib_info[i];
                k++;
                num ++;

                if( k == 3) break; //at most accept three extension port/directpath
            }
        }
        IFX_PPACMD_DBG("There are %d extension port ( USB/WLAN) in PPE FW\n", k);

        tmp_info.port_num = num;
        IFX_PPACMD_DBG("There are %d port to be printed\n", (int)tmp_info.port_num);

        print_port_mib( &tmp_info );

    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->dsl_mib_info), sizeof(pdata->dsl_mib_info) );
    }

    return;
}
/****ppa_get_port_mib ----end */

/*ppacmd ppa_clear_port_mib ---begin */
static void ppa_clear_port_mib_help( int summary)
{
    IFX_PPACMD_PRINT("clearmib\n");
    return;
}
/****ppa_clear_port_mib ----end */

/*ppacmd ppa_get_iface_mib ---begin */
#if defined(PPA_IP_COUNTER) && PPA_IP_COUNTER
static void ppa_get_iface_mib_help( int summary)
{
	IFX_PPACMD_PRINT("getifacemib -i <interface name>\n");
	return;
}

static int ppa_parse_get_iface_mib_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
	int  opt = 0;
	PPA_CMD_IFINFO *if_info;

	if_info = &(pdata->if_info);

	ppa_memset(if_info, sizeof(PPA_CMD_IFINFO), 0);
	while (popts->opt)
	{
		if (popts->opt == 'i')
		{
			opt++;
			if (strncpy_s(if_info->ifname, PPA_IF_NAME_SIZE, popts->optarg, PPA_IF_NAME_SIZE) != EOK) {
				IFX_PPACMD_DBG("Invalid interfcae name!\n");
				return PPA_CMD_ERR;
			}
		}
		popts++;
	}

	if (opt != 1)
		return PPA_CMD_ERR;

	IFX_PPACMD_DBG("PPA IFACE: [%s]\n", pdata->if_info.ifname);

	return PPA_CMD_OK;
}

static void ppa_print_get_iface_mib_cmd( PPA_CMD_DATA *pdata)
{
	int strlen = 0;
	IFX_PPACMD_PRINT("Interface:[%s]\n\n", pdata->if_info.ifname);
	IFX_PPACMD_PRINT("IPv4 RX pkts:[%llu]\tTX pkts:[%llu]\n", pdata->if_info.acc_rx_ipv4, pdata->if_info.acc_tx_ipv4);
	IFX_PPACMD_PRINT("IPv6 RX pkts:[%llu]\tTX pkts:[%llu]\n", pdata->if_info.acc_rx_ipv6, pdata->if_info.acc_tx_ipv6);

	return;
}

static int ppa_do_cmd_get_iface_mib(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata)
{
	int ret = 0;

	ret = ppa_do_ioctl_cmd(pcmd->ioctl_cmd, pdata);

	return ret;
}
#endif /* PPA_IP_COUNTER */
/****ppa_get_iface_mib ----end */


/*ppacmd d( dbgtool ) ---begin*/
static void ppa_dbg_tool_help( int summary)
{
    if( !summary )
        IFX_PPACMD_PRINT("d [-m mem_size_in_kbytes]\n");
    else
    {
        IFX_PPACMD_PRINT("d [-m mem_size_in_kbytes for Low memory test]\n");
    }
    return;
}
static const char ppa_dbg_tool_short_opts[] = "m:h";
static int ppa_parse_dbg_tool_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{   
    ppa_memset( &pdata->dbg_tool_info, sizeof(pdata->dbg_tool_info), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'm':
            pdata->dbg_tool_info.mode = PPA_CMD_DBG_TOOL_LOW_MEM_TEST;
            pdata->dbg_tool_info.value.size = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            if( ppa_do_ioctl_cmd(PPA_CMD_DBG_TOOL, &pdata->dbg_tool_info ) == PPA_CMD_OK ) 
            {
                IFX_PPACMD_PRINT("Succeed to allocate %u Kbytes buffer for Low Memory endurance test\n", pdata->dbg_tool_info.value.size );
            }
            else
                IFX_PPACMD_PRINT("Failed to allocate %u Kbytes buffer for Low Memory endurance test\n", pdata->dbg_tool_info.value.size );
            
            break;
        
        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

/*ppacmd d( dbgtool ) ---end*/


/*ppacmd ppa_set_value ---begin */
struct variable_info variable_list[]={
{"early_drop_flag", PPA_VARIABLE_EARY_DROP_FLAG, 0, 1},
{"directpath_imq_flag", PPA_VARIABLE_PPA_DIRECTPATH_IMQ_FLAG, 0, 1},
{"lan_separate_flag", PPA_LAN_SEPARATE_FLAG, 0, 100},
{"wan_separate_flag", PPA_WAN_SEPARATE_FLAG, 0, 100},
};

static const char ppa_set_variable_value_short_opts[] = "n:v:h";
static const char ppa_get_variable_value_short_opts[] = "n:h";

static void ppa_set_variable_value_help( int summary)    
{
    int i;
    
    IFX_PPACMD_PRINT("setvalue  <-n variable_name > <-v value>\n");

    if( summary )
    {
        for( i=0; i<NUM_ENTITY(variable_list); i++)
        {
            if( i == 0 ) 
                IFX_PPACMD_PRINT("    %-20s           %-9s         %-9s\n", "Variable list", "min-value", "max-value");
            IFX_PPACMD_PRINT("    %-20s           %-9i         %-9i\n", variable_list[i].name, variable_list[i].min, variable_list[i].max );
        }
    }
    
    return;
}

static void ppa_get_variable_value_help( int summary)    
{
    int i;
    
    IFX_PPACMD_PRINT("getvalue  <-n variable_name >\n");

    if( summary )
    {
        for( i=0; i<NUM_ENTITY(variable_list); i++)
        {
            if( i == 0 ) 
                IFX_PPACMD_PRINT("    %-20s           %-9s         %-9s\n", "Variable list", "min-value", "max-value");
            IFX_PPACMD_PRINT("    %-20s           %-9i         %-9i\n", variable_list[i].name, variable_list[i].min, variable_list[i].max );
        }
    }
    
    return;
}

static int ppa_parse_set_variable_value_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int n_opt = 0;
    unsigned int v_opt = 0;
    int i, res = 0;

    ppa_memset( pdata, sizeof(pdata->var_value_info), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'n':
            for( i=0; i<NUM_ENTITY(variable_list); i++)
            {
                if( strcmp(popts->optarg, variable_list[i].name) == 0 )
                {
                    pdata->var_value_info.id = variable_list[i].id;                    
                    n_opt ++;
                }
            }
            if( !n_opt ) 
            {
                IFX_PPACMD_PRINT("Not supported variable name:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;

        case 'v':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->var_value_info.value = res;
		v_opt ++;
		break;    

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( n_opt != 1 || v_opt != 1 )
    {
        IFX_PPACMD_PRINT("Wrong parameter.\n");
        return PPA_CMD_ERR;
    }
    if( pdata->var_value_info.value < variable_list[pdata->var_value_info.id].min ||  pdata->var_value_info.value > variable_list[pdata->var_value_info.id].max )
        IFX_PPACMD_PRINT("Wrong value(%u), it should be %u ~ %u\n", pdata->var_value_info.value , variable_list[pdata->var_value_info.id].min, variable_list[pdata->var_value_info.id].max);
    
    return PPA_CMD_OK;
}


static int ppa_parse_get_variable_value_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int n_opt = 0;
    int i;

    ppa_memset( pdata, sizeof(pdata->var_value_info),0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'n':
            for( i=0; i<NUM_ENTITY(variable_list); i++)
            {
                if( strcmp(popts->optarg, variable_list[i].name) == 0 )
                {
                    pdata->var_value_info.id = variable_list[i].id;                    
                    n_opt ++;
                }
            }
            if( !n_opt ) 
            {
                IFX_PPACMD_PRINT("Not supported variable name:%s\n", popts->optarg);
                return PPA_CMD_ERR;
            }
            break;        

        default:
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( n_opt != 1)
    {
        IFX_PPACMD_PRINT("Wrong parameter.\n");
        return PPA_CMD_ERR;
    }
  
    return PPA_CMD_OK;
}

static void ppa_print_get_variable_value_cmd( PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The value is %i\n", pdata->var_value_info.value);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->var_value_info), sizeof(pdata->var_value_info) );
    }

    return;
}
/*ppacmd ppa_set_value ---end */

/*ppacmd setppefp ---begin*/
static void ppa_set_ppefp_help( int summary)
{
    IFX_PPACMD_PRINT("setppefp [-f 0/1]\n");
    return;
}

static int ppa_set_ppefp_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_PPA_FASTPATH_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'f':
            pdata->ppe_fastpath_enable_info.ppe_fastpath_enable = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);;
            break;

        default:
            IFX_PPACMD_PRINT("ppa_set_ppefp_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static const char ppa_set_ppefp_short_opts[] = "f:h";
static const struct option ppa_set_ppefp_long_opts[] =
{
    {"flag",   required_argument,  NULL, 'f'},
    { 0,0,0,0 }
};

static void ppa_get_ppefp_help( int summary)
{
    IFX_PPACMD_PRINT("getppefpstatus [-o outfile]\n");
    return;
}

static int ppa_get_ppefp_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_PPA_FASTPATH_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_get_ppefp_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static void ppa_print_get_ppefp_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The PPA - PPE Fastpath is %s\n", pdata->ppe_fastpath_enable_info.ppe_fastpath_enable ? "enabled":"disabled");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->ppe_fastpath_enable_info), sizeof(pdata->ppe_fastpath_enable_info) );
    }
}


static const char ppa_get_ppefp_short_opts[] = "o:h";
static const struct option ppa_get_ppefp_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
/*ppacmd setppefp ---end*/

#ifdef CONFIG_PPA_API_SW_FASTPATH
/*ppacmd setswfp ---begin*/
static void ppa_set_swfp_help( int summary)
{
    IFX_PPACMD_PRINT("setswfp [-f 0/1]\n");
    return;
}

static int ppa_set_swfp_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_SW_FASTPATH_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'f':
            pdata->sw_fastpath_enable_info.sw_fastpath_enable = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);;
            break;

        default:
            IFX_PPACMD_PRINT("ppa_set_swfp_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static const char ppa_set_swfp_short_opts[] = "f:h";
static const struct option ppa_set_swfp_long_opts[] =
{
    {"flag",   required_argument,  NULL, 'f'},
    { 0,0,0,0 }
};

static void ppa_get_swfp_help( int summary)
{
    IFX_PPACMD_PRINT("getswfpstatus [-o outfile]\n");
    return;
}

static int ppa_get_swfp_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_SW_FASTPATH_ENABLE_INFO),0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_get_swfp_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    return PPA_CMD_OK;
}

static void ppa_print_get_swfp_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The PPA Software Fastpath is %s\n", pdata->sw_fastpath_enable_info.sw_fastpath_enable ? "enabled":"disabled");
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->sw_fastpath_enable_info), sizeof(pdata->sw_fastpath_enable_info) );
    }
}


static const char ppa_get_swfp_short_opts[] = "o:h";
static const struct option ppa_get_swfp_long_opts[] =
{
    {"save-to-file",   required_argument,  NULL, 'o'},
    { 0,0,0,0 }
};
/*ppacmd setswfp ---end*/

/*ppacmd setswsession ---begin*/
static void ppa_set_sw_session_help( int summary)
{
    IFX_PPACMD_PRINT("setswsession [-a session-address] [-f 0/1]\n");
    return;
}

static int ppa_set_sw_session_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt = 0, f_opt = 0;
    ppa_memset( pdata, sizeof(PPA_CMD_SW_SESSION_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'a':
            pdata->sw_session_enable_info.session = (void *)str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            a_opt++;
            break;

        case  'f':
		pdata->sw_session_enable_info.sw_session_enable = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);;
		if ((pdata->sw_session_enable_info.sw_session_enable != 0) &&
			(pdata->sw_session_enable_info.sw_session_enable != 1)) {
			IFX_PPACMD_PRINT("ppa_set_sw_session_cmd : Invalid input\n");
			return PPA_CMD_ERR;
		}
		f_opt++;
		break;

        default:
            IFX_PPACMD_PRINT("ppa_set_sess_swfp_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( ( a_opt != 1 ) || ( f_opt != 1 ) )
    {
        ppa_set_sw_session_help(1);
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("session address=0x%x\n", (unsigned int)pdata->sw_session_enable_info.session  );
    IFX_PPACMD_DBG("Flag=0x%x\n", (unsigned int)pdata->sw_session_enable_info.sw_session_enable );

    return PPA_CMD_OK;
}

static const char ppa_set_sw_session_short_opts[] = "a:f:h";
static const struct option ppa_set_sw_session_long_opts[] =
{
    {"session address",   required_argument,  NULL, 'a'},
    {"flag",   required_argument,  NULL, 'f'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static void ppa_get_sw_session_help( int summary)
{
    IFX_PPACMD_PRINT("getswsessionstatus [-a <session-address>] [-o outfile]\n");
    return;
}

static int ppa_get_sw_session_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    unsigned int a_opt = 0;
    ppa_memset( pdata, sizeof(PPA_CMD_SW_SESSION_ENABLE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case  'a':
            pdata->sw_session_enable_info.session = (void *)str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);;
            a_opt++;
            break;

        case  'o':
            g_output_to_file = 1;
            strncpy_s(g_output_filename,PPACMD_MAX_FILENAME, popts->optarg,PPACMD_MAX_FILENAME);
            break;

        default:
            IFX_PPACMD_PRINT("ppa_get_sw_session_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( ( a_opt != 1 ) )
    {
        ppa_get_sw_session_help(1);
        return PPA_CMD_ERR;
    }

    return PPA_CMD_OK;
}

static void ppa_print_get_sw_session_cmd(PPA_CMD_DATA *pdata)
{
    if( !g_output_to_file )
    {
        IFX_PPACMD_PRINT("The PPA Software Fastpath is %s for session %x\n", pdata->sw_session_enable_info.sw_session_enable ? "enabled":"disabled", (unsigned int)pdata->sw_session_enable_info.session);
    }
    else
    {
        SaveDataToFile(g_output_filename, (char *)(&pdata->sw_session_enable_info), sizeof(pdata->sw_session_enable_info) );
    }
}


static const char ppa_get_sw_session_short_opts[] = "a:o:h";
static const struct option ppa_get_sw_session_long_opts[] =
{
    {"session address",   required_argument,  NULL, 'a'},
    {"save-to-file",   required_argument,  NULL, 'o'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};
/*ppacmd setswsession ---end*/
#endif

#if 0 //tc set/get for queue map
/*
===============================================================================
  Command : setQosTc
  discription:
===============================================================================
*/

static const char ppa_setQosTc_queue_map_short_opts[] = "i:p:q:t:f:h";
static const struct option ppa_setQosTc_queue_map_long_opts[] =
{
    {"ifname",  required_argument,  NULL, 'i'},
    {"portid",  required_argument,  NULL, 'p'},
    {"queue_id",   required_argument,  NULL, 'q'},
    {"tcmap",  required_argument,  NULL, 't'},
    {"flag",  required_argument,  NULL, 'f'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_setQosTc_queue_map_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
	
    int p_flg=0, q_flg=0, i_flg=0;
    memset( pdata, 0, sizeof(PPA_CMD_QOS_QUEUE_INFO) );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
    	    strcpy( pdata->qos_tc_queuemap_info.ifname, popts->optarg );
	    i_flg=1;
	    break;
        case 'p':
            pdata->qos_tc_queuemap_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_flg=1;
            break;
        case 'q':
            pdata->qos_tc_queuemap_info.queue_id = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 't':
            pdata->qos_tc_queuemap_info.tcmap = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;

        case 'f':
            pdata->qos_tc_queuemap_info.flags = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
	case 'h':
	    return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("%s not support parameter -%c \n", __FUNCTION__, popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if ((i_flg | p_flg) & q_flg & t_flg) { 
    	IFX_PPACMD_DBG("%s: queueid=%d tc=%d\n",  (unsigned int)pdata->qos_tc_queuemap_info.qid, (unsigned int)pdata->qos_tc_queuemap_info.tcmap);
    	return PPA_CMD_OK;
    }

    return PPA_CMD_ERR;
}


static void ppa_setQosTc_queue_map_help(int summary)
{
    IFX_PPACMD_PRINT("setQosTc {-p <port-id> -q <que-id> -t <tc-map> | -i <ifname> -q <que-id> -t <tc-map> } \
	{ --portid <port-id> --queue_id <que-id> --tcmap <tc-map> | --ifname <ifname> --queue_id <que-id> --tcmap <tc-map>} \n");
    IFX_PPACMD_PRINT("SetQosTc -h | --help");
    return;
}

/*
===============================================================================
  Command : getQosTc
  discription:
===============================================================================
*/

static const char ppa_getQosTc_queue_map_short_opts[] = "i:p:q:t:f:h";
static const struct option ppa_getQosTc_queue_map_long_opts[] =
{
    {"ifname",  required_argument,  NULL, 'i'},
    {"portid",  required_argument,  NULL, 'p'},
    {"queue_id",   required_argument,  NULL, 'q'},
    {"tcmap",  required_argument,  NULL, 't'},
    {"flag",  required_argument,  NULL, 'f'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_getQosTc_queue_map_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
	
    int p_flg=0, q_flg=0, i_flg=0;
    memset( pdata, 0, sizeof(PPA_CMD_QOS_QUEUE_INFO) );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
    	    strcpy_s(pdata->qos_tc_queuemap_info.ifname, PPA_IF_NAME_SIZE, popts->optarg );
	    i_flg=1;
	    break;
        case 'p':
            pdata->qos_tc_queuemap_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            p_flg=1;
            break;
        case 'q':
            pdata->qos_tc_queuemap_info.queue_id = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 't':
            pdata->qos_tc_queuemap_info.tcmap = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;

        case 'f':
            pdata->qos_tc_queuemap_info.flags = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
	case 'h':
	    return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("%s not support parameter -%c \n", __FUNCTION__, popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if ((i_flg | p_flg) & q_flg & t_flg) { 
    	IFX_PPACMD_DBG("%s: queueid=%d tc=%d\n",  (unsigned int)pdata->qos_tc_queuemap_info.qid, (unsigned int)pdata->qos_tc_queuemap_info.tcmap);
    	return PPA_CMD_OK;
    }

    return PPA_CMD_ERR;
}


static void ppa_setQosTc_queue_map_help(int summary)
{
    IFX_PPACMD_PRINT("setQosTc {-p <port-id> -q <que-id> -t <tc-map> | -i <ifname> -q <que-id> -t <tc-map> } \
	{ --portid <port-id> --queue_id <que-id> --tcmap <tc-map> | --ifname <ifname> --queue_id <que-id> --tcmap <tc-map>} \n");
    IFX_PPACMD_PRINT("SetQosTc -h | --help");
    return;
}

#endif

static void ppa_swap_sessions_help( int summary)
{
    IFX_PPACMD_PRINT("swapsessions\n");
    return;
}
static int ppa_swap_sessions_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata)
{
    return PPA_CMD_OK;
}

#ifdef CONFIG_SOC_GRX500
/*
===============================================================================
  Command : addclass
===============================================================================
*/
typedef enum {
pid=256,
pid_val,
pid_ex,
subif,
subif_id,
subif_ex,
dscp,
dscp_in,
dscp_val,
dscp_ex,
vlanpcp,
cvlan,
svlan,
vlanpcp_val,
vlanpcp_ex,
pktlen,
pktlen_val,
pktlen_range,
pktlen_ex,
mac,
mac_dst,
mac_src,
mac_addr,
mac_mask,
mac_ex,
appdata,
appdata_msb,
appdata_lsb,
appdata_val,
appdata_range,
appdata_ex,
ip,
ip_dst,
ip_src,
ip_dst_in,
ip_src_in,
ip_type,
ip_addr,
ip_mask,
ip_ex,
ethertype,
type_val,
type_mask,
type_ex,
ipproto,
proto_in,
proto_val,
proto_mask,
proto_ex,
pppoe,
sess_id,
sess_ex,
ppp,
ppp_val,
ppp_mask,
ppp_ex,
vlan,
vid,
vid_range,
vid_mask,
vid_ex,
svid,
svid_val,
svid_ex,
payload,
payld1,
payld2,
payld_val,
payld_mask_range_sel,
payld_mask,
payld_ex,
parserflg,
msbflg,
lsbflg,
flg_val,
flg_mask,
flg_ex,
interface,
interface_name,
} pattern_t;

typedef enum {
filter=1,
portfilter,
crossstate,
act_vlan,
act_vid,
fid,
act_svlan,
act_svid,
crossvlan,
crossvlan_ignore,
fwd,
fwd_crossvlan,
learning,
port_trunk,
portmap,
fwd_portmap,
fwd_subifid,
rtextid_enable,
rtextid,
rtdestportmaskcmp,
rtsrcportmaskcmp,
rtdstipmaskcmp,
rtsrcipmaskcmp,
rtinneripaskey,
rtaccelenable,
rtctrlenable,
processpath,
qos,
tc,
alt_tc,
meter,
meter_id,
criticalframe,
remark,
remark_pcp,
remark_stagpcp,
remark_stagdei,
remark_dscp,
remark_class,
flowid_action,
flowid,
mgmt,
irq,
timestamp,
rmon,
rmon_id
} action_t;

static const char ppa_addclass_short_opts[] = "d:c:s:o:p:a:h";
static const struct option ppa_addclass_long_opts[] =
{
   {"dev", required_argument, NULL, 'd'},
   {"category", required_argument, NULL, 'c'},
   {"subcategory", required_argument, NULL, 's'},
   {"order", required_argument, NULL, 'o'},
   {"pattern", no_argument,NULL, 'p'},
   {"action", no_argument,NULL, 'a'},
   {"help", required_argument, NULL, 'h'},
   {"port", no_argument, NULL, pid},
   {"pid",required_argument,NULL, pid_val},
   {"pid-excl",no_argument, NULL, pid_ex},
   {"subif",no_argument,NULL, subif},
   {"subifid",required_argument,NULL, subif_id},
   {"subif-excl",no_argument,NULL,subif_ex},
   {"dscp",no_argument,NULL, dscp},
   {"dscp-inner",no_argument,NULL, dscp_in},
   {"dscp-val",required_argument,NULL, dscp_val},
   {"dscp-excl",no_argument,NULL, dscp_ex},
   {"vlanpcp",no_argument,NULL, vlanpcp},
   {"cvlan",optional_argument,NULL,cvlan},
   {"svlan",optional_argument,NULL,svlan},
   {"vlanpcp-val",required_argument,NULL,vlanpcp_val},
   {"vlanpcp-excl",no_argument,NULL,vlanpcp_ex},
   {"pktlen",no_argument,NULL,pktlen},
   {"length",required_argument,NULL, pktlen_val},
   {"pktlen-range",required_argument,NULL,pktlen_range},
   {"pktlen-excl",no_argument ,NULL,pktlen_ex},
   {"mac",no_argument,NULL,mac},
   {"mac-dst",optional_argument,NULL,mac_dst},
   {"mac-src",optional_argument,NULL,mac_src},
   {"mac-addr",required_argument,NULL,mac_addr},
   {"mac-mask",required_argument,NULL,mac_mask},
   {"mac-excl",no_argument,NULL,mac_ex},
   {"appdata",no_argument, NULL, appdata},
   {"data-msb",required_argument,NULL,appdata_msb},
   {"data-lsb",required_argument,NULL,appdata_lsb},
   {"data-val",required_argument,NULL,appdata_val},
   {"data-range",required_argument,NULL,appdata_range},
   {"data-excl",no_argument, NULL ,appdata_ex},
   {"ip",no_argument,NULL,ip},
   {"ip-dst",required_argument,NULL,ip_dst},
   {"ip-src",required_argument,NULL,ip_src},
   {"ip-inner-dst",required_argument,NULL,ip_dst_in},
   {"ip-inner-src",required_argument,NULL,ip_src_in},
   {"ip-type",required_argument,NULL,ip_type},
   {"ip-addr", required_argument, NULL, ip_addr},
   {"ip-mask",required_argument,NULL,ip_mask},
   {"ip-excl",no_argument,NULL,ip_ex},
   {"ethertype",no_argument,NULL,ethertype},
   {"type-val",required_argument,NULL,type_val},
   {"type-mask",required_argument,NULL,type_mask},
   {"type-excl",no_argument, NULL,type_ex},
   {"protocol",no_argument,NULL,ipproto},
   {"proto-inner",required_argument, NULL, proto_in},
   {"proto-val",required_argument, NULL,proto_val},
   {"proto-mask",required_argument, NULL,proto_mask},
   {"proto-excl",no_argument, NULL, proto_ex},
   {"pppoe",no_argument,NULL,pppoe},
   {"sessionid",required_argument, NULL, sess_id},
   {"pppoe-excl", no_argument, NULL, sess_ex},
   {"ppp",no_argument,NULL,ppp},
   {"ppp-val", required_argument, NULL, ppp_val},
   {"ppp-mask", required_argument, NULL, ppp_mask},
   {"ppp-excl",no_argument, NULL, ppp_ex},
   {"vlan",no_argument,NULL,vlan},
   {"vlanid",required_argument, NULL, vid},
   {"vlan-range" , required_argument, NULL ,vid_range},
   {"vlan-mask", required_argument, NULL, vid_mask},
   {"vlan-excl", no_argument, NULL,vid_ex},
   {"svlan", no_argument,NULL,svid},
   {"svlanid", required_argument, NULL, svid_val},
   {"svlan-excl", optional_argument, NULL, svid_ex},
   {"payload", no_argument,NULL, payload},
   {"payld1", required_argument, NULL, payld1},
   {"payld2", required_argument, NULL, payld2},
   {"payld-val", required_argument,NULL, payld_val},
   {"mask-range", required_argument,NULL, payld_mask_range_sel},
   {"payld-mask", required_argument, NULL, payld_mask},
   {"payld-excl", no_argument, NULL, payld_ex},
   {"parserflg", no_argument, NULL, parserflg},
   {"parser-msb", no_argument, NULL, msbflg},
   {"parser-lsb", no_argument, NULL, lsbflg},
   {"parser-val", required_argument, NULL, flg_val},
   {"parser-mask", required_argument, NULL, flg_mask},
   {"parser-excl", no_argument, NULL, flg_ex},
   {"interface", no_argument, NULL, interface},
   {"interface_name", required_argument, NULL, interface_name},
   {"filter", no_argument, NULL, filter},
   {"port_filter",required_argument, NULL, portfilter},
   {"cross_state",required_argument, NULL,crossstate},
   {"vlan-action", no_argument,NULL, act_vlan},
   {"vid", required_argument , NULL, act_vid},
   {"fid", required_argument, NULL, fid},
   {"svlan-action", required_argument, NULL, act_svlan},
   {"svid", required_argument, NULL, act_svid},
   {"crossvlan",  required_argument, NULL,crossvlan},
   {"vlan-ignore", required_argument, NULL,crossvlan_ignore},
   {"forward", no_argument, NULL,fwd},
   {"fwd-crossvlan", no_argument, NULL,fwd_crossvlan},
   {"learning", required_argument, NULL, learning},
   {"port-trunk", required_argument, NULL, port_trunk},
   {"portmap-enable",required_argument, NULL , portmap},
   {"portmap", required_argument, NULL, fwd_portmap},
   {"subifid", required_argument, NULL, fwd_subifid},
   {"routextid-enable",required_argument, NULL, rtextid_enable},
   {"routextid",required_argument, NULL, rtextid},
   {"rtdestportmaskcmp",required_argument, NULL, rtdestportmaskcmp},
   {"rtsrcportmaskcmp",required_argument, NULL,rtsrcportmaskcmp},
   {"rtdstipmaskcmp",required_argument, NULL,rtdstipmaskcmp},
   {"rtsrcipmaskcmp",required_argument, NULL,rtsrcipmaskcmp},
   {"rtinneripaskey",required_argument, NULL,rtinneripaskey},
   {"rtaccelenable", required_argument, NULL,rtaccelenable},
   {"rtctrlenable", required_argument, NULL,rtctrlenable},
   {"processpath", required_argument, NULL,processpath},
   {"qos-action", no_argument, NULL, qos},
   {"tc-action", required_argument, NULL,tc},
   {"alt-tc", required_argument, NULL,alt_tc},
   {"meter",required_argument, NULL, meter},
   {"meter-id",required_argument, NULL, meter_id},
   {"criticalframe-action",required_argument, NULL,criticalframe},
   {"remark",required_argument, NULL,remark},
   {"pcp",required_argument, NULL,remark_pcp},
   {"stagpcp",required_argument, NULL,remark_stagpcp},
   {"stagdei",required_argument, NULL,remark_stagdei},
   {"dscpremark", required_argument, NULL,remark_dscp},
   {"class",required_argument, NULL,remark_class},
   {"flowid_action",required_argument, NULL,flowid_action},
   {"flowid",required_argument, NULL,flowid},
   {"mgmt",no_argument, NULL, mgmt},
   {"irq",required_argument, NULL,irq},
   {"timestamp",required_argument, NULL,timestamp},
   {"rmon",no_argument, NULL,rmon},
   {"rmon_id",required_argument, NULL,rmon_id},
   { 0,0,0,0}
};

static int ppa_parse_pattern_cmd(PPA_CMD_OPTS **popts,PPA_CMD_DATA **pdata)
{
    printf("In function Parse_pattern\n");
    int msb_field=0,dst_ip=0,src_ip=0,dst_inner_ip=0,src_inner_ip=0,inner_dscp=0,cvlan_pcp=0,svlan_pcp=0;    
    int dst_mac=0,src_mac=0,lsb_field=0,inner_ip_prot=0,payload_1=0,payload_2=0,MSW_Flag=0,LSW_Flag=0;
    int ret=0;
	(*popts)++;
	switch((*popts)->opt)
	{
	case pid:
	    printf("In PortId\n");
	    while((*popts)->opt) {
		printf( "Inside while loop");
	    	(*popts)++;
	    	switch ((*popts)->opt)
            	{
		case pid_val://port_id
		    printf("In case pid\n");
		    (*pdata)->class_info.pattern.bPortIdEnable=1;
		    (*pdata)->class_info.pattern.nPortId = str_convert(STRING_TYPE_INTEGER,(*popts)->optarg, NULL);
		    printf("PortId value is %d\n",(*pdata)->class_info.pattern.nPortId);
		    break;
		case pid_ex://port_exclude
		    (*pdata)->class_info.pattern.bPortId_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		}
		if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
		   break;
		}
	    }
	break;
	case subif:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case subif_id:
		    (*pdata)->class_info.pattern.bSubIfIdEnable=1;
		    (*pdata)->class_info.pattern.nSubIfId=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case subif_ex:
		    (*pdata)->class_info.pattern.bSubIfId_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
	    }
	    }
	    break;
	case dscp:
	    printf("In case DSCP\n");
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case dscp_in:
		    inner_dscp=1;
		    break;
		case dscp_val:
		    if(inner_dscp==1) {
			(*pdata)->class_info.pattern.bInner_DSCP_Enable=1;
			(*pdata)->class_info.pattern.nInnerDSCP=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else {
			printf( "In DSCP Value else part\n");
		    	(*pdata)->class_info.pattern.bDSCP_Enable=1;
		    	(*pdata)->class_info.pattern.nDSCP=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case dscp_ex:
		    if(inner_dscp==1) 
			(*pdata)->class_info.pattern.bInnerDSCP_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else
		    	(*pdata)->class_info.pattern.bDSCP_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg,NULL);
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
	    }
	    }
	    break;

	case vlanpcp:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case cvlan :
		    cvlan_pcp=1;
		    break;
		case svlan:
		    svlan_pcp=1;
		    break;
		case vlanpcp_val:
		    if(cvlan_pcp==1) {
		    	(*pdata)->class_info.pattern.bPCP_Enable=1;
		    	(*pdata)->class_info.pattern.nPCP=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else if (svlan_pcp==1) {
			(*pdata)->class_info.pattern.bSTAG_PCP_DEI_Enable=1;
			(*pdata)->class_info.pattern.nSTAG_PCP_DEI=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case vlanpcp_ex:
		    if(cvlan_pcp==1) 
		    	(*pdata)->class_info.pattern.bCTAG_PCP_DEI_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else if (svlan_pcp==1) 
			(*pdata)->class_info.pattern.bSTAG_PCP_DEI_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;

	case pktlen:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case pktlen_val:
		    (*pdata)->class_info.pattern.bPktLngEnable=1;
		    (*pdata)->class_info.pattern.nPktLng=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case pktlen_range:
		    (*pdata)->class_info.pattern.nPktLngRange=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case pktlen_ex:
		    (*pdata)->class_info.pattern.bPktLng_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;

	case mac:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case mac_dst:
		    dst_mac=1;
		    break;
		case mac_src:
		    src_mac=1;
		    break;
		case mac_addr:
		    if(dst_mac==1) {
		    	(*pdata)->class_info.pattern.bMAC_DstEnable=1;
			stomac((*popts)->optarg,(*pdata)->class_info.pattern.nMAC_Dst);
		    }
		    else {
		    	(*pdata)->class_info.pattern.bMAC_SrcEnable=1;
		    	stomac((*popts)->optarg,(*pdata)->class_info.pattern.nMAC_Src);
		    }
		    break;
		case mac_mask:
		    if(dst_mac==1) 
			(*pdata)->class_info.pattern.nMAC_DstMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else 
		    	(*pdata)->class_info.pattern.nMAC_SrcMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case mac_ex:
		    if(dst_mac==1)
			(*pdata)->class_info.pattern.bDstMAC_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else
		    	(*pdata)->class_info.pattern.bSrcMAC_Exclude=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		default:
		    printf("Pattern invalid or not related to specific pattern\n");
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;

	case appdata:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case appdata_msb:
		    msb_field=1;
		    break;
		case appdata_lsb:
		    lsb_field=1;
		    break;
		case appdata_val:
		    if(msb_field==1) {
			(*pdata)->class_info.pattern.bAppDataMSB_Enable=1;
			(*pdata)->class_info.pattern.nAppDataMSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else if(lsb_field==1) {
		    	(*pdata)->class_info.pattern.bAppDataLSB_Enable=1;
		    	(*pdata)->class_info.pattern.nAppDataLSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case appdata_range://range select
//		    msb_range_select=1;lsb_range_select=1;
		    if(msb_field==1) { 
			(*pdata)->class_info.pattern.bAppMaskRangeMSB_Select=1;
			(*pdata)->class_info.pattern.nAppMaskRangeMSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);	
		    }
		    else if(lsb_field==1) {
		    	(*pdata)->class_info.pattern.bAppMaskRangeLSB_Select=1;
		    	(*pdata)->class_info.pattern.nAppMaskRangeLSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case appdata_ex:
		    if(msb_field==1)
		    	(*pdata)->class_info.pattern.bAppMSB_Exclude=1;
		    else if (lsb_field==1)
		    	(*pdata)->class_info.pattern.bAppLSB_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;

	case ip:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case ip_dst:
		    dst_ip=1;
		    break;
		case ip_src:
		    src_ip=1;
		    break;
		case ip_dst_in:
		    dst_inner_ip=1;
		    break;
		case ip_src_in:
		    src_inner_ip=1;
		    break;
		case ip_type:
		    if(dst_ip==1) 
			(*pdata)->class_info.pattern.eDstIP_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    
		    else if(dst_inner_ip==1) 
			(*pdata)->class_info.pattern.eInnerDstIP_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    
		    else if(src_inner_ip==1)
			(*pdata)->class_info.pattern.eInnerSrcIP_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		       
		    else 
			(*pdata)->class_info.pattern.eSrcIP_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;

		case ip_addr:
		    if(dst_ip==1) {
		    	ret=str_convert(STRING_TYPE_IP, (*popts)->optarg,(void *)&(*pdata)->class_info.pattern.nDstIP);
		    	if(ret==IP_NON_VALID) {        
    			    printf("Not Valid IP\n");  
    			    break;                     
 		    	}
		    }                           
		    else if(dst_inner_ip==1) {
			ret=str_convert(STRING_TYPE_IP, (*popts)->optarg,(void *)&(*pdata)->class_info.pattern.nInnerDstIP);
			if(ret==IP_NON_VALID) {       
			    printf("Not Valid IP\n"); 
    			    break;                    
			}
		    }
		    else if(src_inner_ip==1) {
			ret=str_convert(STRING_TYPE_IP, (*popts)->optarg, (void *)&(*pdata)->class_info.pattern.nInnerSrcIP);
			if(ret==IP_NON_VALID) {
			    printf("Not Valid IP\n");
			    break;
		 	}
		    }
		    else if(src_ip==1) {
		    	ret=str_convert(STRING_TYPE_IP, (*popts)->optarg, (void *)&(*pdata)->class_info.pattern.nSrcIP);   
		    	if(ret==IP_NON_VALID) {     
			    printf("Not Valid IP\n");
			    break;
		        }
		    }
		    else
		    	break;
		
		
		case ip_mask:
		    if(dst_ip==1) 
			(*pdata)->class_info.pattern.nDstIP_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else if(dst_inner_ip==1)
			(*pdata)->class_info.pattern.nInnerDstIP_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else if(src_inner_ip==1)
			(*pdata)->class_info.pattern.nInnerSrcIP_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else
		    	(*pdata)->class_info.pattern.nSrcIP_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;

		case ip_ex:
		    if(dst_ip==1)
			(*pdata)->class_info.pattern.bDstIP_Exclude=1;
		    else if(dst_inner_ip==1)
			(*pdata)->class_info.pattern.bInnerDstIP_Exclude=1;		
		    else if(src_inner_ip==1)
			(*pdata)->class_info.pattern.bInnerSrcIP_Exclude=1;
		    else
		        (*pdata)->class_info.pattern.bSrcIP_Exclude=1;
		    break;
		}
		if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            	}
		}
	break;
		
	case ethertype:
	    while((*popts)->opt) {
  	    (*popts)++;                                
	    switch((*popts)->opt)
	    {
		case type_val:
		    (*pdata)->class_info.pattern.bEtherTypeEnable=1;
		    (*pdata)->class_info.pattern.nEtherType=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case type_mask:
		    (*pdata)->class_info.pattern.nEtherTypeMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case type_ex:
		    (*pdata)->class_info.pattern.bEtherType_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;
	
	case ipproto:
	    while ((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case proto_in:
		    inner_ip_prot=1;
		    break;
		case proto_val:
		    if(inner_ip_prot==1) {
			(*pdata)->class_info.pattern.bInnerProtocolEnable=1;
			(*pdata)->class_info.pattern.nInnerProtocol=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else {
		    	(*pdata)->class_info.pattern.bProtocolEnable=1;
		    	(*pdata)->class_info.pattern.nProtocol=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case proto_mask:
		    if(inner_ip_prot==1) 
			(*pdata)->class_info.pattern.nInnerProtocolMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else
		    	(*pdata)->class_info.pattern.nProtocolMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case proto_ex:
		    if(inner_ip_prot==1)
			(*pdata)->class_info.pattern.bInnerProtocol_Exclude=1;
		    else
		    	(*pdata)->class_info.pattern.bProtocol_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;
	
	
	case pppoe:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
		case sess_id:
		    (*pdata)->class_info.pattern.bSessionIdEnable=1;
		    (*pdata)->class_info.pattern.nSessionId=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case sess_ex:
		    (*pdata)->class_info.pattern.bSessionId_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;

	case ppp:
	    while((*popts)->opt) { 
	    (*popts)++;                               
	    switch((*popts)->opt)
	    {
		case ppp_val:
		    (*pdata)->class_info.pattern.bPPP_ProtocolEnable=1;
		    (*pdata)->class_info.pattern.nPPP_Protocol=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case ppp_mask:
		    (*pdata)->class_info.pattern.nPPP_ProtocolMask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case ppp_ex:
		    (*pdata)->class_info.pattern.bPPP_Protocol_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;
	case vlan:
	    while((*popts)->opt) { 
	    (*popts)++; 
	    switch((*popts)->opt)
	    {
		case vid:
		    (*pdata)->class_info.pattern.bVid=1;
		    (*pdata)->class_info.pattern.nVid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case vid_range://range select
		    (*pdata)->class_info.pattern.bVidRange_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case vid_mask://range or mask
		    (*pdata)->class_info.pattern.nVidRange=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case vid_ex:
		    (*pdata)->class_info.pattern.bVid_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;

		    
	case svid:
	    while((*popts)->opt) {    
	    (*popts)++;                                 
	    switch((*popts)->opt)
	    {
		case svid_val:
		    (*pdata)->class_info.pattern.bSLAN_Vid=1;
		    (*pdata)->class_info.pattern.nSLAN_Vid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case svid_ex:
		    (*pdata)->class_info.pattern.bSLANVid_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;
	
	case payload:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
	        case payld1:
		    payload_1=1;
		    break;
		case payld2:
		    payload_2=1;
		    break;
		case payld_val:
		    if(payload_1==1) {
			(*pdata)->class_info.pattern.bPayload1_SrcEnable=1;
			(*pdata)->class_info.pattern.nPayload1=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
			
		    }
		    else {
		    	(*pdata)->class_info.pattern.bPayload2_SrcEnable=1;
		   	(*pdata)->class_info.pattern.nPayload2=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    break;
		case payld_mask_range_sel:
		    if(payload_1==1) {
			(*pdata)->class_info.pattern.bPayload1MaskRange_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else
		        (*pdata)->class_info.pattern.bPayload2MaskRange_Select=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case payld_mask:
		    if(payload_1==1) {
			(*pdata)->class_info.pattern.nPayload1_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else
		        (*pdata)->class_info.pattern.nPayload2_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case payld_ex:
		    if(payload_1==1) 
			(*pdata)->class_info.pattern.bPayload1_Exclude=1;
		    else
		    	(*pdata)->class_info.pattern.bPayload2_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;

	case parserflg:
	    while((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
	    	case msbflg:
		    MSW_Flag=1;
		    break;
		case lsbflg:
		    LSW_Flag=1;
		    break;
		case flg_val:
		    if(MSW_Flag==1) {
		    	(*pdata)->class_info.pattern.bParserFlagMSB_Enable=1;
		    	(*pdata)->class_info.pattern.nParserFlagMSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    }
		    else {
			(*pdata)->class_info.pattern.bParserFlagLSB_Enable=1;
			(*pdata)->class_info.pattern.nParserFlagLSB=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);  
		    }
		    break;
		case flg_mask:
		    if(MSW_Flag==1) 
		    	(*pdata)->class_info.pattern.nParserFlagMSB_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    else
			(*pdata)->class_info.pattern.nParserFlagLSB_Mask=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		    break;
		case flg_ex:
		    if(MSW_Flag==1)
		    	(*pdata)->class_info.pattern.bParserFlagMSB_Exclude=1;
		    else
			(*pdata)->class_info.pattern.bParserFlagLSB_Exclude=1;
		    break;
	    }
	    if (((*popts)->opt =='p')||( (*popts)->opt == 'a')) {
                break;
            }
	    }
		break;
	case interface:
		while((*popts)->opt) {
		(*popts)++;
		switch((*popts)->opt)
			{
				case interface_name:
					ret = strncpy_s((*pdata)->class_info.rx_if, PPA_IF_NAME_SIZE, (*popts)->optarg, PPA_IF_NAME_SIZE);
					if (ret != PPA_CMD_OK) {
						printf("ERROR: Invalid interface name\n");
						return ret;
					}
					break;
			}
			if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
				break;
			}
		}
		break;
	default:
		printf("Deafult Argument\n");
	
			
    } 
return 0;
}

static int ppa_parse_action_cmd(PPA_CMD_OPTS **popts,PPA_CMD_DATA **pdata)
{
	printf("In action \n");
	(*popts)++;
	switch((*popts)->opt)
	{
	case filter:
	    printf("In case filter\n");
	    while ((*popts)->opt) {
	    (*popts)++;
	    switch((*popts)->opt)
	    {
	    case portfilter:
		printf("in case port_filter\n");
		(*pdata)->class_info.action.filter_action.portfilter=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
	 	break;
	    case crossstate:
		printf("In case cross_state\n");
		(*pdata)->class_info.action.filter_action.crossstate=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;
	case act_vlan:
	    while ((*popts)->opt) {
	    (*popts)++; 
	    switch((*popts)->opt)
	    {
	    case act_vid:
		printf("In case act_vid\n");
		(*pdata)->class_info.action.vlan_action.vlan_id=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case fid:
		(*pdata)->class_info.action.vlan_action.fid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case act_svlan:
		(*pdata)->class_info.action.vlan_action.cvlan=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case act_svid:
		(*pdata)->class_info.action.vlan_action.svlan=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case crossvlan:
		(*pdata)->class_info.action.vlan_action.cross_vlan=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case crossvlan_ignore:
		(*pdata)->class_info.action.vlan_action.cvlan_ignore=1;
		break;
	    }
	    if (((*popts)->opt =='p' )|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	    break;
	case fwd:
	    while ((*popts)->opt) { 
	    (*popts)++;
	    switch((*popts)->opt)
	    {
	    case learning:
		(*pdata)->class_info.action.fwd_action.learning=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case port_trunk:
		(*pdata)->class_info.action.fwd_action.port_trunk=1;
		break;
	    case portmap:
		(*pdata)->class_info.action.fwd_action.portmap=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case fwd_portmap:
		(*pdata)->class_info.action.fwd_action.forward_portmap=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case fwd_subifid:
		 (*pdata)->class_info.action.fwd_action.forward_subifid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtextid_enable:
		(*pdata)->class_info.action.fwd_action.routextid_enable=1;
		(*pdata)->class_info.action.fwd_action.routextid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtdestportmaskcmp:
		(*pdata)->class_info.action.fwd_action.rtdestportmaskcmp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtsrcportmaskcmp:
		(*pdata)->class_info.action.fwd_action.rtsrcportmaskcmp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtdstipmaskcmp:
		(*pdata)->class_info.action.fwd_action.rtdstipmaskcmp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtsrcipmaskcmp:
		(*pdata)->class_info.action.fwd_action.rtsrcipmaskcmp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtinneripaskey:
		(*pdata)->class_info.action.fwd_action.rtinneripaskey=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtaccelenable:
		(*pdata)->class_info.action.fwd_action.rtaccelenable=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case rtctrlenable:
		(*pdata)->class_info.action.fwd_action.rtctrlenable=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case processpath:
		(*pdata)->class_info.action.fwd_action.processpath=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;
	case qos:
	    while ((*popts)->opt) { 
	    (*popts)++;
	    switch((*popts)->opt)
	    {
	    case tc:	
		(*pdata)->class_info.action.qos_action.trafficclass=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case alt_tc:
		(*pdata)->class_info.action.qos_action.alt_trafficclass=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case meter:
		(*pdata)->class_info.action.qos_action.meter=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case meter_id:
		(*pdata)->class_info.action.qos_action.meterid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case criticalframe:
		(*pdata)->class_info.action.qos_action.criticalframe=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark:
		(*pdata)->class_info.action.qos_action.remark=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark_pcp:
		(*pdata)->class_info.action.qos_action.remarkpcp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark_stagpcp:
		 (*pdata)->class_info.action.qos_action.remark_stagpcp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark_stagdei:
		(*pdata)->class_info.action.qos_action.remark_stagdei=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark_dscp:
		(*pdata)->class_info.action.qos_action.remark_dscp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case remark_class:
		(*pdata)->class_info.action.qos_action.remark_class=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case flowid_action:
		(*pdata)->class_info.action.qos_action.flowid_enabled=1;
		(*pdata)->class_info.action.qos_action.flowid=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;

	case mgmt:
	    while ((*popts)->opt) {
            (*popts)++;
	    switch((*popts)->opt)
	    {
	    case irq:
		(*pdata)->class_info.action.mgmt_action.irq=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    case timestamp:
		(*pdata)->class_info.action.mgmt_action.timestamp=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    }
	    if (((*popts)->opt =='p')|| ((*popts)->opt == 'a')) {
                break;
            }
	    }
	break;
	case rmon:
	    while ((*popts)->opt) {
            (*popts)++;
            switch((*popts)->opt)
            {
	    case rmon_id:
		(*pdata)->class_info.action.rmon_action=1;
	 	(*pdata)->class_info.action.rmon_id=str_convert(STRING_TYPE_INTEGER, (*popts)->optarg, NULL);
		break;
	    }
	   if ((*popts)->opt =='p'|| (*popts)->opt == 'a') {
                break;
           }
	   }
	break;
	default:
	    IFX_PPACMD_PRINT("ppa_parse_add_class_cmd not support parameter -%c \n",(*popts)->opt);
            return PPA_CMD_ERR;	    
	}
	
return 0;
}
static int ppa_parse_addclass_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    uint32_t ret;
    ppa_memset( pdata, sizeof(PPA_CMD_CLASSIFIER_INFO), 0 );
    while(popts->opt)
    {
	switch(popts->opt)
	{
	case 'd':
		/* (default) 0 - GSWIP-R (dev=1)
		             1 - GSWIP-L (dev=0)  */
		ret = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (ret == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return ret;
		}
		pdata->class_info.in_dev=ret;
		break;
	case 'c':
		printf("in case 'c'\n");
		ret = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (ret == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return ret;
		}
		pdata->class_info.category=ret;
		printf("Category _id is %d\n",pdata->class_info.category);
		break;
	case 's':
		printf("in case 's'\n");
		ret = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (ret == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return ret;
		}
		pdata->class_info.subcategory=ret;
		break;
	case 'o' :
		printf("in case 'o'\n");
		ret = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (ret == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return ret;
		}
		pdata->class_info.order=ret;
		break;
	case 'p' :
	    printf( "In case 'p'\n");
	    ret =  ppa_parse_pattern_cmd(&popts,&pdata);
	    if (ret==0) {
			/* Enable pattern if parsing is successfull */
	    	pdata->class_info.pattern.bEnable = 1;
			popts--;
	    	break;
	    }
	case 'a':
	    ret= ppa_parse_action_cmd(&popts,&pdata);
	    if (ret==0) {
                popts--;
                break;
            }
	    else {
                printf( "Error\n");
                return PPA_CMD_ERR;
            }
	case 'h':
	    return PPA_CMD_HELP;
	    break;
	default:
	    IFX_PPACMD_PRINT("ppa_parse_add_class_cmd does not support parameter -%c \n",popts->opt);
            return PPA_CMD_ERR;
	}
	popts++;
    }
return PPA_CMD_OK;
	  
}
static void ppa_addclass_help(int summary)
{
    IFX_PPACMD_PRINT("addclass {-d <--device> -c <--category> -s <--subcategory> -o <--order> -p [--pattern] -a [--action>]\n");
    IFX_PPACMD_PRINT("pattern : 		parameters \n");
    IFX_PPACMD_PRINT("<--port> : <--pid  |  --pid-excl>\n<--subif> : <--subifid  |  --sifid-excl>\n<--dscp>  : <--dscp-inner  |  --dscp-val  |  --dscp-excl>\n<--vlanpcp> : <--cvlan>  |  --svlan  |  --vlanpcp-val  |  --vlanpcp-excl>\n<--pktlen> : <--length  |  --pktlen-range  |  --pktlen-excl>\n<--mac>: <--mac-dst  |  --mac-src  |  --mac-addr  |  --mac-mask  |  --mac_excl>\n<--appdata> : <--data-msb  | --data-lsb  |  --data-val  |  --data-range  |  --data-excl>\n<--ip>   :<--ip-dst  |  --ip-src  |  --ip-inner-dst  |  --ip-inner-src  |  --ip-type  |  --ip-addr  |  --ip-mask  |  --ip-excl>\n<--ethertype> : <--type-val  |  --type-mask  |  --type-excl>\n<--protocol> : <--proto-inner  |  --proto-val  |  --proto-mask  |  --proto-excl>\n<--pppoe>: <--sessionid  |  --pppoe-excl>\n<--ppp>: <--ppp-val  |  --ppp-mask  |  --ppp-excl>\n<--vlan>: <--vlanid  |  --vlan-range  |  --vlan-mask  |  --vlan-excl>\n<--svlan>:<--svlanid  |  --svlan-excl>\n<--payload>:<--payld1  |  --payld2  |  --payld-val  |  --mask-range  |  --payld-mask  |  --payld-excl>\n<--parserflg>: <--parser-msb  |  --parser-lsb  |  --parser-val  |  --parser-mask  |  --parser-excl>\n<--interface>: <--interface_name>\n");
   IFX_PPACMD_PRINT("action: \t parameters \n");
   IFX_PPACMD_PRINT("<--filter> : <--port_filter  |  --cross_state>\n<--vlan-action> : <--vid  | --fid  |  --svlan-action  |  --svid  |  --crossvlan  |  --vlan-ignore>\n<--forward> : <--fwd-crossvlan   |  --learning  |  --port-trunk  |  --portmap-enable  |  --portmap  |  --subifid  |  --routextid-enable  |  --routextid  |  --rtdestportmaskcmp  |  --rtsrcportmaskcmp  |  --rtdstipmaskcmp  |  --rtsrcipmaskcmp  |  -- rtinneripaskey  -- rtaccelenable  --rtctrlenable  --processpath>\n<--qos-action> : <--tc-action  |   --alt-tc  |  --meter  |  --meter-id  |  --criticalframe-action  |  --remark  |  --pcp  |  --stagpcp  |  --stagdei  |  --dscpremark  |  --class  |  --flowid_action  |  --flowid>\n<--mgmt> : <--irq  |  --timestamp>\n<--rmon> : <--rmon_id>");
    return;
}

static void ppa_print_addclass_cmd(PPA_CMD_DATA *pdata)
{
#define MAX_DUMP_STRLEN 256
	int category = pdata->class_info.category;
	char dumpStr[MAX_DUMP_STRLEN] = {'\0'};

	/* Dumping data based on category */
	switch(category) {
		case CAT_FWD:
			snprintf(dumpStr, MAX_DUMP_STRLEN, "Rule Parameters: device: %s catgeory: %d sub-catgeory: %d"
				" order: %d PCE rule idx: %d", pdata->class_info.in_dev ? "GSWIP-L" : "GSWIP-R",
				pdata->class_info.category, pdata->class_info.subcategory, pdata->class_info.order,
				pdata->class_info.pattern.nIndex);
			break;
		/* Future categories */
 		defualt:
			break;
	}
	if(!g_output_to_file) {
		IFX_PPACMD_PRINT("%s\n", dumpStr);
	} else {
        	SaveDataToFile(g_output_filename, dumpStr, sizeof(dumpStr) );
	}
#undef MAX_DUMP_STRLEN
	return;
}

/*
===============================================================================
  Command : modclass
===============================================================================
*/
static const char ppa_modclass_short_opts[]= "c:s:o:p:a";
static const struct option ppa_modclass_long_opts[] =
{
   {"categoryid", required_argument, NULL, 'c'},
   {"subcategoryid", optional_argument, NULL, 's'},
   {"orderno", required_argument, NULL, 'o'},
   {"pattern",required_argument,NULL, 'p'},
   {"action",required_argument,NULL, 'a'},
   {"help",no_argument, NULL, 'h'},
   { 0,0,0,0}
};
static int ppa_parse_modclass_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_CLASSIFIER_INFO),0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
	case 'd':
	    pdata->class_info.in_dev=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
	    break;
        case 'c':
            pdata->class_info.category=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 's':
            pdata->class_info.subcategory=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'o' :
            pdata->class_info.order=str_convert(STRING_TYPE_INTEGER, popts->optarg,NULL);
            break;
	case 'p' :
            if ((ppa_parse_pattern_cmd(&popts,&pdata))== PPA_CMD_OK)
            	break;
	    else
		return PPA_CMD_ERR;
        case 'a':
            if ((ppa_parse_action_cmd(&popts,&pdata))==PPA_CMD_OK)
            	break;
	    else
		return PPA_CMD_ERR;
        case 'h':
            return PPA_CMD_HELP;
            break;
        default:
            IFX_PPACMD_PRINT("ppa_parse_add_class_cmd not support parameter -%c \n",popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

return PPA_CMD_OK;
}

static void ppa_modclass_help(int summary)
{
    IFX_PPACMD_PRINT("modclass {-c <category-id> -s <subcategory-id> -o <order-no> -p <pattern> -a <action>\n");
    IFX_PPACMD_PRINT("modclass -h | --help");
    return;
}

/*
===============================================================================
  Command : delclass
===============================================================================
*/
static const char ppa_delclass_short_opts[] = "d:c:s:o:h";
static const struct option ppa_delclass_long_opts[] =
{
   {"dev", required_argument, NULL, 'd'},
   {"categoryid", required_argument, NULL, 'c'},
   {"subcategoryid", required_argument, NULL, 's'},
   {"orderno", required_argument, NULL, 'o'},
   {"help",required_argument, NULL, 'h'},
   { 0,0,0,0}
};
static int ppa_parse_delclass_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_CLASSIFIER_INFO),0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'd':
            pdata->class_info.in_dev=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'c':
            pdata->class_info.category=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 's':
            pdata->class_info.subcategory=str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'o' :
            pdata->class_info.order=str_convert(STRING_TYPE_INTEGER, popts->optarg,NULL);
            break;
	case 'h':
	    return PPA_CMD_HELP;
	default:
	    break;
	    
        }
    popts++;
    }

	return PPA_CMD_OK;
}
static void ppa_delclass_help(int summary)
{
    IFX_PPACMD_PRINT("delclass {-c <--category> -s <--subcategory> -o <--order> \n");
    IFX_PPACMD_PRINT("delclass -h | --help");
    return;
}
#endif //PPA classification

#ifdef CONFIG_SOC_GRX500
/*
===============================================================================
  Command : setgroup
  discription: Command for setting ingress group
===============================================================================
*/

static const char ppa_setgroup_short_opts[] = "g:i:h";
static const struct option ppa_setgroup_long_opts[] =
{
    {"ifname",  required_argument,  NULL, 'i'},
    {"group",  required_argument,  NULL, 'g'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};


static int ppa_parse_setgroup_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    int i_flg = 0, g_flg = 0;
    uint32_t res;

    ppa_memset(pdata, sizeof(PPA_CMD_QOS_INGGRP_INFO), 0);

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'i':
    	    strcpy_s(pdata->qos_inggrp_info.ifname,PPA_IF_NAME_SIZE, popts->optarg);
            i_flg=1;
            break;
        case 'g':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_inggrp_info.ingress_group = res;
		if (pdata->qos_inggrp_info.ingress_group < PPA_QOS_INGGRP0 || pdata->qos_inggrp_info.ingress_group >= PPA_QOS_INGGRP_MAX)
		{
			IFX_PPACMD_PRINT("Invalid Group %d (allowed [%d])\n", PPA_QOS_INGGRP0, PPA_QOS_INGGRP_MAX - 1);
			return PPA_CMD_ERR;
		}
		g_flg=1;
		break;
        case 'h':
            return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("ppa_parse_setgroup_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if(!(i_flg && g_flg))
    {
        IFX_PPACMD_PRINT("Manditory option missing:\n ppacmd setgroup [-i ifname] | [-g Ingress group]\n");
        return PPA_CMD_ERR;
    }

    IFX_PPACMD_DBG("ppa_parse_setgroup_cmd: ifname=%s, group=%d\n", pdata->qos_inggrp_info.ifname, (unsigned int)pdata->qos_inggrp_info.ingress_group); 
    return PPA_CMD_OK;
}

static void ppa_setgroup_help(int summary)
{
    IFX_PPACMD_PRINT("setgroup -i <ifname> -g <Ingress Group>\n");
    IFX_PPACMD_PRINT("  Ingress Group: INGGRP%d:INGGRP%d\n", PPA_QOS_INGGRP0, PPA_QOS_INGGRP_MAX - 1);
}
#endif //PPA ingress grouping

/*
===============================================================================
  Command : addque
  discription: Command for Queue Creation
===============================================================================
*/

char * strupr(char *str)
{
	int i;
	for(i=0;i<=strlen(str);i++){
            if(str[i]>=97&&str[i]<=122)
            	str[i]=str[i]-32;
  	}
	return str;
}

static const char ppa_addque_short_opts[] = "p:q:i:l:w:f:t:v:T:d:h";
static const struct option ppa_addque_long_opts[] =
{
    {"portid",  required_argument,  NULL, 'p'},
    {"que_num",   required_argument,  NULL, 'q'},
    {"ifname",  required_argument,  NULL, 'i'},
    {"priority",  required_argument,  NULL, 'l'},
    {"weight",  required_argument,  NULL, 'w'},
    {"tc",  required_argument,  NULL, 't'},
    {"intfid",  required_argument,  NULL, 'v'},
    {"flag",  required_argument,  NULL, 'f'},
    {"Type",  required_argument,  NULL, 'T'},
    {"defque",  required_argument,  NULL, 'd'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_addque_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    int p_flg=0, q_flg=0, i_flg=0, i ;
    uint32_t res;
    char *str;
    ppa_memset( pdata, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}

		pdata->qos_queue_info.portid = res;
		p_flg=1;
		break;
        case 'q':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_queue_info.queue_num = res;
		pdata->qos_queue_info.shaper.enable = 1;
		q_flg=1;
		break;
        case 'i':
    	    strcpy_s( pdata->qos_queue_info.ifname,PPA_IF_NAME_SIZE, popts->optarg );
	    i_flg=1;
	    break;
	case 'l':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_queue_info.priority = res;
		if (pdata->qos_queue_info.priority < 0 || pdata->qos_queue_info.priority > 15 ) {
			IFX_PPACMD_PRINT("ERROR: invalid level should be 0-15\n");
			return PPA_CMD_ERR;
		}
		break;
	case 'w':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_queue_info.weight = res;
		pdata->qos_queue_info.sched = PPA_QOS_SCHED_WFQ; 
		if (pdata->qos_queue_info.weight < 1 || pdata->qos_queue_info.weight > 100 ) {
			IFX_PPACMD_PRINT("ERROR: Invalid weight Should be 1-100\n");
			return PPA_CMD_ERR;
		}
	    break;
        case 't':
	    i = 0;
	    pdata->qos_queue_info.tc_map[i++] = str_convert(STRING_TYPE_INTEGER, strtok(popts->optarg, ","), NULL);
	    str = strtok(NULL, ",");
	    while( str ){
		pdata->qos_queue_info.tc_map[i++] = str_convert(STRING_TYPE_INTEGER, str, NULL);
		if(i >= MAX_TC_NUM)
		{
			IFX_PPACMD_PRINT("Error: Tc count= %d <= Max Tc count=%d\n ", i, MAX_TC_NUM);
			return PPA_CMD_ERR;
		}
	    str = strtok(NULL, ",");
	    }
	    pdata->qos_queue_info.tc_no = i ;
           break;
        case 'v':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_queue_info.flowId = res;
		break;
	case 'T':
	    if (PPA_QOS_Q_F_INGRESS == str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL))
	            pdata->qos_queue_info.flags |= PPA_QOS_Q_F_INGRESS;
	    else {
		IFX_PPACMD_PRINT("ERROR: invalid input\n");
		return PPA_CMD_ERR;
	    }

            break;
        case 'd':
	    if (str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL))
	            pdata->qos_queue_info.flags |= PPA_QOS_Q_F_DEFAULT;
	    else{
		IFX_PPACMD_PRINT("ERROR: invalid input\n");
		return PPA_CMD_ERR;
	    }
            break;
        case 'f':
            pdata->qos_queue_info.flags |= str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
	    break;
	case 'h':
	    return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("ppa_parse_addque_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( !(p_flg | i_flg ) & !q_flg)
    {
        IFX_PPACMD_PRINT("Manditory option missing:\n ppacmd addque [-p portid] | [-i ifname] & [-q Qid]\n");
        return PPA_CMD_ERR;
    }
    IFX_PPACMD_DBG("ppa_parse_addque_cmd: portid=%d, queueid=%d\n", (unsigned int)pdata->qos_queue_info.portid, (unsigned int)pdata->qos_queue_info.queue_num);
    return PPA_CMD_OK;
}


static void ppa_addque_help(int summary)
{
    IFX_PPACMD_PRINT("addque {-p <port-id> -q <que-id> -i <ifname> -l <priority> -w <weight> -t <Tc> -v <FlowId> -T <Type>\n");
    IFX_PPACMD_PRINT("  Type: 1 - INGRESS, 0 - EGRESS\n");
    IFX_PPACMD_PRINT("  flags: 1 - Default Queue \n");
}

/*
===============================================================================
  Command : delque
  discription: Delete queue 
===============================================================================
*/

static const char ppa_delque_short_opts[] = "i:q:p:T:h";
static const struct option ppa_delque_long_opts[] =
{
    {"queid",   required_argument,  NULL, 'q'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"portid",   required_argument,  NULL, 'p'},
    {"type",   required_argument,  NULL, 'T'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_delque_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    ppa_memset( pdata, sizeof(PPA_CMD_QOS_QUEUE_INFO),0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'q':
            pdata->qos_queue_info.queue_num = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            pdata->qos_queue_info.shaper_num = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'p':
            pdata->qos_queue_info.portid = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'i':
    	    strcpy_s( pdata->qos_queue_info.ifname,PPA_IF_NAME_SIZE,popts->optarg);
            break;
        case 'T':
            if (PPA_QOS_Q_F_INGRESS == str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL))
	            pdata->qos_queue_info.portid |= PPA_QOS_Q_F_INGRESS;
            break;
        case 'h':
            return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("ppa_parse_delque_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    IFX_PPACMD_DBG("ppa_parse_delque_cmd: queueid=%d\n", (unsigned int)pdata->qos_queue_info.queue_num);

    return PPA_CMD_OK;
}

static void ppa_delque_help(int summary)
{
    IFX_PPACMD_PRINT("delque -p <port-id> -q <que-id> -i <ifname> \n");
    return;
}

/*
===============================================================================
  Command : modque
  discription: Modify QOS Sub interface to Port Configuration 
===============================================================================
*/

static const char ppa_mod_queue_short_opts[] = "q:i:w:p:l:h";
static const struct option ppa_mod_queue_long_opts[] =
{
    {"queueid",   required_argument,  NULL, 'q'},
    {"ifname",   required_argument,  NULL, 'i'},
    {"weight",   required_argument,  NULL, 'w'},
    {"priority",   required_argument,  NULL, 'p'},
    {"qlen",   required_argument,  NULL, 'l'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_mod_queue_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
	
    ppa_memset( pdata, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0 );

    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'q':
            pdata->qos_queue_info.queue_num = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'p':
            pdata->qos_queue_info.priority = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'i':
    	    strcpy_s( pdata->qos_queue_info.ifname,PPA_IF_NAME_SIZE,popts->optarg);
            break;
        case 'w':
            pdata->qos_queue_info.weight = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
        case 'l':
            pdata->qos_queue_info.qlen = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
            break;
	case 'h':
	    return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("ppa_parse_mod_queue_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    IFX_PPACMD_DBG("ppa_parse_mod_queue_cmd: interface=%s\n", pdata->qos_queue_info.ifname);

    return PPA_CMD_OK;
}

static void ppa_mod_queue_help(int summary)
{ // need to write once conformed
    IFX_PPACMD_PRINT("modque -q <que-id> -i <ifname> -w <weight> -l <priority>\n");
    IFX_PPACMD_PRINT("modque -h | --help\n");
    return;
}


/*
===============================================================================
  Command : modsubif2port
  discription:
===============================================================================
*/

static const char ppa_modsubif2port_short_opts[] = "i:p:l:w:f:h";
static const struct option ppa_modsubif2port_long_opts[] =
{
    {"portid",  required_argument,  NULL, 'p'},
    {"ifname",  required_argument,  NULL, 'i'},
    {"priority",  required_argument,  NULL, 'l'},
    {"weight",  required_argument,  NULL, 'w'},
    {"flag",  required_argument,  NULL, 'f'},
    {"help",   no_argument,  NULL, 'h'},
    { 0,0,0,0 }
};

static int ppa_parse_modsubif2port_cmd(PPA_CMD_OPTS *popts,PPA_CMD_DATA *pdata)
{
    int p_flg=0, l_flg=0, i_flg=0, w_flg=0;
    uint32_t res;
    ppa_memset( pdata, sizeof(PPA_CMD_SUBIF_PORT_INFO), 0 );
    while(popts->opt)
    {
        switch(popts->opt)
        {
        case 'p':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->subif_port_info.port_id = res;
		p_flg=1;
		break;
        case 'i':
    	    strcpy_s( pdata->subif_port_info.ifname,PPA_IF_NAME_SIZE, popts->optarg );
	    i_flg=1;
	    break;
	case 'l':
		l_flg=1;
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->subif_port_info.priority_level = res;
		if (pdata->subif_port_info.priority_level > 15 ) {
			IFX_PPACMD_PRINT("ERROR: invalid level should be 0-15\n");
		return PPA_CMD_ERR;
		}
		break;
	case 'w':
		w_flg=1;
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->subif_port_info.weight = res;
		if (pdata->subif_port_info.weight < 1 || pdata->subif_port_info.weight > 100 ) {
			IFX_PPACMD_PRINT("ERROR: Invalid weight Should be 1-100\n");
			return PPA_CMD_ERR;
		}
	    break;
        case 'f':
		res = str_convert(STRING_TYPE_INTEGER, popts->optarg, NULL);
		if (res == PPA_CMD_ERR){
			IFX_PPACMD_PRINT("ERROR: invalid input\n");
			return res;
		}
		pdata->qos_queue_info.flags = res;
		break;
	case 'h':
	    return PPA_CMD_HELP;
        default:
            IFX_PPACMD_PRINT("ppa_parse_subif_port_info_cmd not support parameter -%c \n", popts->opt);
            return PPA_CMD_ERR;
        }
        popts++;
    }

    if( !(p_flg | i_flg)  & (l_flg | w_flg))
    {
        IFX_PPACMD_PRINT("Manditory option missing:\n ppacmd modsubif2port {[-p portid] | [-i ifname]} & {[-l priority] | [-w weight]}\n");
        return PPA_CMD_ERR;
    }
    return PPA_CMD_OK;
}


static void ppa_modsubif2port_help(int summary)
{
    IFX_PPACMD_PRINT("modsubif2port {-p <port-id> | -i <ifname>} & {-l <priority> | -w <weight>}\n");
    return;
}

/*
===============================================================================
  Command definitions
===============================================================================
*/

static PPA_COMMAND ppa_cmd[] =
{
    {
        "---PPA Initialization/Status commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },

    {
        "init",
        PPA_CMD_INIT,
        ppa_print_init_help,
        ppa_parse_init_cmd,
        ppa_do_cmd,
        ppa_print_init_fake_cmd,
        ppa_init_long_opts,
        ppa_init_short_opts
    },
    {
        "exit",
        PPA_CMD_EXIT,
        ppa_print_exit_help,
        ppa_parse_exit_cmd,
        ppa_do_cmd,
        NULL,
        ppa_no_long_opts,
        ppa_no_short_opts
    },
    {
        "control",
        PPA_CMD_ENABLE,
        ppa_print_control_help,
        ppa_parse_control_cmd,
        ppa_do_cmd,
        NULL,
        ppa_control_long_opts,
        ppa_no_short_opts
    },
    {
        "status",
        PPA_CMD_GET_STATUS,
        ppa_print_status_help,
        ppa_parse_status_cmd,
        ppa_do_cmd,
        ppa_print_status,
        ppa_no_long_opts,
        ppa_output_short_opts
    },
    {
        "getversion",
        PPA_CMD_GET_VERSION,
        ppa_get_version_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_version_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },

    {
        "---PPA LAN/WAN Interface control commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "addwan",
        PPA_CMD_ADD_WAN_IF,
        ppa_print_add_wan_help,
        ppa_parse_add_del_if_cmd,
        ppa_do_add_del_if_cmd,
        NULL,
        ppa_if_long_opts,
        ppa_if_short_opts
    },
    {
        "addlan",
        PPA_CMD_ADD_LAN_IF,
        ppa_print_add_lan_help,
        ppa_parse_add_del_if_cmd,
        ppa_do_add_del_if_cmd,
        NULL,
        ppa_if_long_opts,
        ppa_if_short_opts
    },
    {
        "delwan",
        PPA_CMD_DEL_WAN_IF,
        ppa_print_del_wan_help,
        ppa_parse_add_del_if_cmd,
        ppa_do_add_del_if_cmd,
        NULL,
        ppa_if_long_opts,
        ppa_if_short_opts
    },
    {
        "dellan",
        PPA_CMD_DEL_LAN_IF,
        ppa_print_del_lan_help,
        ppa_parse_add_del_if_cmd,
        ppa_do_add_del_if_cmd,
        NULL,
        ppa_if_long_opts,
        ppa_if_short_opts
    },
    {
        "getwan",
        PPA_CMD_GET_WAN_IF,
        ppa_print_get_wan_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_wan_netif_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getlan",
        PPA_CMD_GET_LAN_IF,
        ppa_print_get_lan_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_lan_netif_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "setmac",
        PPA_CMD_SET_IF_MAC,
        ppa_print_set_mac_help,
        ppa_parse_set_mac_cmd,
        ppa_do_cmd,
        NULL,
        ppa_if_mac_long_opts,
        ppa_if_mac_short_opts
    },
    {
        "getmac",
        PPA_CMD_GET_IF_MAC,
        ppa_print_get_mac_help,
        ppa_parse_get_mac_cmd,
        ppa_do_cmd,
        ppa_print_get_mac_cmd,
        ppa_if_long_opts,
        ppa_if_output_short_opts
    },

    {
        "---PPA bridging related commands(For A4/D4/E4 Firmware and A5 Firmware in DSL WAN mode)",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "addbr",
        PPA_CMD_ADD_MAC_ENTRY,
        ppa_add_mac_entry_help,
        ppa_parse_add_mac_entry_cmd,
        ppa_do_cmd,
        NULL,
        ppa_if_mac_long_opts,
        ppa_if_mac_short_opts
    },
    {
        "delbr",
        PPA_CMD_DEL_MAC_ENTRY,
        ppa_del_mac_entry_help,
        ppa_parse_del_mac_entry_cmd,
        ppa_do_cmd,
        NULL,
        ppa_mac_long_opts,
        ppa_mac_short_opts
    },
    {
        "getbrnum",
        PPA_CMD_GET_COUNT_MAC,
        ppa_get_br_count_help,
        ppa_parse_get_br_count,
        ppa_do_cmd,
        ppa_print_get_count_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getbrs",
        PPA_CMD_GET_ALL_MAC,
        ppa_get_all_br_help,
        ppa_parse_simple_cmd,
        ppa_get_all_br_cmd,
        NULL,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "setbr",  // to enable/disable bridge mac learning hook
        PPA_CMD_BRIDGE_ENABLE,
        ppa_set_br_help,
        ppa_set_br_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_br_long_opts,
        ppa_set_br_short_opts,
    },
    {
        "getbrstatus",  //get bridge mac learning hook status: enabled or disabled
        PPA_CMD_GET_BRIDGE_STATUS,
        ppa_get_br_status_help,
        ppa_get_br_status_cmd,
        ppa_do_cmd,
        ppa_print_get_br_status_cmd,
        ppa_get_br_status_long_opts,
        ppa_get_br_status_short_opts,
    },
#ifdef CONFIG_SOC_GRX500
    {
        "getbrfid",  //get bridge fid
        PPA_CMD_GET_BRIDGE_FID,
        ppa_get_br_fid_help,
        ppa_get_br_fid_cmd,
        ppa_do_cmd,
        ppa_print_get_br_fid_cmd,
        ppa_get_br_fid_long_opts,
        ppa_get_br_fid_short_opts,
    },
#endif

    {
        "---PPA unicast routing acceleration related commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "getlansessionnum",
        PPA_CMD_GET_COUNT_LAN_SESSION,
        ppa_get_lan_session_count_help,
        ppa_parse_get_lan_session_count,
        ppa_do_cmd,
        ppa_print_get_count_cmd,
        NULL,
        ppa_get_session_count_short_opts
    },
    {
        "getlansessions",
        PPA_CMD_GET_LAN_SESSIONS,
        ppa_get_lan_sessions_help,
        ppa_parse_get_session_cmd,
        ppa_get_lan_sessions_cmd,
        NULL,
        ppa_no_long_opts,
        ppa_get_session_short_opts
    },
    {
        "getwansessionnum",
        PPA_CMD_GET_COUNT_WAN_SESSION,
        ppa_get_wan_session_count_help,
        ppa_parse_get_wan_session_count,
        ppa_do_cmd,
        ppa_print_get_count_cmd,
        NULL,
        ppa_get_session_count_short_opts
    },
    {
        "getwansessions",
        PPA_CMD_GET_WAN_SESSIONS,
        ppa_get_wan_sessions_help,
        ppa_parse_get_session_cmd,
        ppa_get_wan_sessions_cmd,
        NULL,
        ppa_no_long_opts,
        ppa_get_session_short_opts
    },
    {
        "summary",
        0,
        ppa_summary_help,
        ppa_parse_summary_cmd,
        NULL,
        NULL,
        ppa_no_long_opts,
        ppa_summary_short_opts
    },
    {
        "addsession",  // get the dsl mib
        PPA_CMD_ADD_SESSION,
        ppa_add_session_help,
        ppa_parse_add_session_cmd,
        ppa_do_cmd,
        NULL,
        ppa_add_session_long_opts,
        ppa_add_session_short_opts
    },
    {
        "delsession",  // delet the session
        PPA_CMD_DEL_SESSION,
        (void (*)(int))ppa_del_session_help,
        ppa_parse_del_session_cmd,
        ppa_do_cmd,
        NULL,
        NULL,
        ppa_del_session_short_opts
    },
    {
        "modifysession",  // get the dsl mib
        PPA_CMD_MODIFY_SESSION,
        ppa_modify_session_help,
        ppa_parse_modify_session_cmd,
        ppa_do_cmd,
        NULL,
        ppa_modify_session_long_opts,
        ppa_modify_session_short_opts
    },
    {
        "getsessiontimer",  // get routing session polling timer
        PPA_CMD_GET_SESSION_TIMER,
        ppa_get_session_timer_help,
        ppa_parse_get_session_timer_cmd,
        ppa_do_cmd,
        ppa_print_get_session_timer,
        ppa_get_session_timer_long_opts,
        ppa_get_session_timer_short_opts
    },
    {
        "setsessiontimer",  // set routing session polling timer
        PPA_CMD_SET_SESSION_TIMER,
        ppa_set_session_timer_help,
        ppa_parse_set_session_timer_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_session_timer_long_opts,
        ppa_set_session_timer_short_opts
    },
    {
        "setppefp",  // to enable/disable ppe fastpath
        PPA_CMD_SET_PPA_FASTPATH_ENABLE,
        ppa_set_ppefp_help,
        ppa_set_ppefp_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_ppefp_long_opts,
        ppa_set_ppefp_short_opts,
    },    
    {
        "getppefpstatus",  //get status of ppe fastpath
        PPA_CMD_GET_PPA_FASTPATH_ENABLE,
        ppa_get_ppefp_help,
        ppa_get_ppefp_cmd,
        ppa_do_cmd,
        ppa_print_get_ppefp_cmd,
        ppa_get_ppefp_long_opts,
        ppa_get_ppefp_short_opts,
    },
#ifdef CONFIG_PPA_API_SW_FASTPATH
    {
        "setswfp",  // to enable/disable sw fastpath
        PPA_CMD_SET_SW_FASTPATH_ENABLE,
        ppa_set_swfp_help,
        ppa_set_swfp_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_swfp_long_opts,
        ppa_set_swfp_short_opts,
    },
    {
        "getswfpstatus",  //get status of sw fastpath
        PPA_CMD_GET_SW_FASTPATH_ENABLE,
        ppa_get_swfp_help,
        ppa_get_swfp_cmd,
        ppa_do_cmd,
        ppa_print_get_swfp_cmd,
        ppa_get_swfp_long_opts,
        ppa_get_swfp_short_opts,
    },
    {
        "setswsession",  // to enable/disable sw session
        PPA_CMD_SET_SW_SESSION_ENABLE,
        ppa_set_sw_session_help,
        ppa_set_sw_session_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_sw_session_long_opts,
        ppa_set_sw_session_short_opts,
    },
    {
        "getswsessionstatus",  //get enable/disable status of sw session
        PPA_CMD_GET_SW_SESSION_ENABLE,
        ppa_get_sw_session_help,
        ppa_get_sw_session_cmd,
        ppa_do_cmd,
        ppa_print_get_sw_session_cmd,
        ppa_get_sw_session_long_opts,
        ppa_get_sw_session_short_opts,
    },
#endif
    {
        "---PPA multicast acceleration related commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "addmc",
        PPA_CMD_ADD_MC,
        ppa_add_mc_help,
        ppa_parse_add_mc_cmd,
        ppa_do_cmd,
        NULL,
        ppa_mc_add_long_opts,
        ppa_mc_add_short_opts
    },
    {
        "getmcnum",
        PPA_CMD_GET_COUNT_MC_GROUP,
        ppa_get_mc_count_help,
        ppa_parse_get_mc_count_cmd,
        ppa_do_cmd,
        ppa_print_get_count_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getmcgroups",
        PPA_CMD_GET_MC_GROUPS,
        ppa_get_mc_groups_help,
        ppa_parse_get_mc_group_cmd,
        ppa_get_mc_groups_cmd,
        NULL,
        ppa_get_mc_group_long_opts,
        ppa_get_mc_group_short_opts
    },
    {
        "getmcextra",
        PPA_CMD_GET_MC_ENTRY,
        ppa_get_mc_extra_help,
        ppa_parse_get_mc_extra_cmd,
        ppa_do_cmd,
        ppa_print_get_mc_extra_cmd,
        ppa_get_mc_extra_long_opts,
        ppa_get_mc_extra_short_opts
    },
    {
        "setmcextra",
        PPA_CMD_MODIFY_MC_ENTRY,
        ppa_set_mc_extra_help,
        ppa_parse_set_mc_extra_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_mc_extra_long_opts,
        ppa_set_mc_extra_short_opts
    },

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
    {
        "---PPA session handling commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "getsessioncriteria",
        PPA_CMD_GET_SESSIONS_CRITERIA,
        ppa_get_session_criteria_help,
        ppa_parse_get_session_criteria, 
        ppa_do_cmd,
        ppa_get_session_criteria,
        NULL,
        ppa_get_session_criteria_short_opts
    },
    {
        "swapsessions", 
        PPA_CMD_SWAP_SESSIONS,
        ppa_swap_sessions_help,
        ppa_swap_sessions_cmd,
        ppa_do_cmd,
        NULL,
        ppa_no_long_opts,
        ppa_swap_sessions_short_opts
    },    
#endif
#if defined( CONFIG_IFX_VLAN_BR )  /*ONly supported in A4/D4 */
    {
        "---PPA VLAN bridging related commands(For PPE A4/D4/E4 only)",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "setvif",
        PPA_CMD_SET_VLAN_IF_CFG,
        ppa_set_vlan_if_cfg_help,
        ppa_parse_set_vlan_if_cfg_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_vlan_if_cfg_long_opts,
        ppa_set_vlan_if_cfg_short_opts
    },
    {
        "getvif",
        PPA_CMD_GET_VLAN_IF_CFG,
        ppa_get_vlan_if_cfg_help,
        ppa_parse_get_vlan_if_cfg_cmd,
        ppa_do_cmd,
        ppa_print_get_vif,
        ppa_if_long_opts,
        ppa_if_output_short_opts
    },
    {
        "addvfilter",
        PPA_CMD_ADD_VLAN_FILTER_CFG,
        ppa_add_vlan_filter_help,
        ppa_parse_add_vlan_filter_cmd,
        ppa_do_cmd,
        NULL,
        ppa_add_vlan_filter_long_opts,
        ppa_add_vlan_filter_short_opts
    },
    {
        "delvfilter",
        PPA_CMD_DEL_VLAN_FILTER_CFG,
        ppa_del_vlan_filter_help,
        ppa_parse_del_vlan_filter_cmd,
        ppa_do_cmd,
        NULL,
        ppa_del_vlan_filter_long_opts,
        ppa_del_vlan_filter_short_opts
    },
    {
        "getvfilternum",
        PPA_CMD_GET_COUNT_VLAN_FILTER,
        ppa_get_vfilter_count_help,
        ppa_parse_get_vfilter_count,
        ppa_do_cmd,
        ppa_print_get_count_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getvfilters",
        PPA_CMD_GET_ALL_VLAN_FILTER_CFG,
        ppa_get_all_vlan_filter_help,
        ppa_parse_simple_cmd,
        ppa_get_all_vlan_filter_cmd,
        NULL,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "delallvfilter",
        PPA_CMD_DEL_ALL_VLAN_FILTER_CFG,
        ppa_del_all_vfilter_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        NULL,
        ppa_no_long_opts,
        ppa_no_short_opts
    },
#endif
#ifdef CONFIG_SOC_GRX500
    {
	"--PPA Classification Related Commands",
	PPA_CMD_INIT,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
    },
    {
        "addclass",
        PPA_CMD_ADD_CLASSIFIER,
        ppa_addclass_help,
        ppa_parse_addclass_cmd,
        ppa_do_cmd,
        ppa_print_addclass_cmd,
        ppa_addclass_long_opts,
        ppa_addclass_short_opts
    },
    {
        "modclass",
        PPA_CMD_MOD_CLASSIFIER,
        ppa_modclass_help,
        ppa_parse_modclass_cmd,
        ppa_do_cmd,
        NULL,
        ppa_modclass_long_opts,
        ppa_modclass_short_opts
    },
    {
        "delclass",
        PPA_CMD_DEL_CLASSIFIER,
        ppa_delclass_help,
        ppa_parse_delclass_cmd,
        ppa_do_cmd,
        NULL,
        ppa_delclass_long_opts,
        ppa_delclass_short_opts
    },
#endif

#ifdef CONFIG_PPA_QOS
    {
        "---PPA QOS related commands ( not for A4/D4/E4 Firmware)",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "getqstatus",  // get maximum eth1 queue number
        PPA_CMD_GET_QOS_STATUS,
        ppa_get_qstatus_help,
        ppa_parse_get_qstatus_cmd,
        ppa_get_qstatus_do_cmd,
        NULL,
        NULL,
        ppa_get_qstatus_short_opts,
    },

    {
        "getqnum",  // get maximum eth1 queue number
        PPA_CMD_GET_QOS_QUEUE_MAX_NUM,
        ppa_get_qnum_help,
        ppa_parse_get_qnum_cmd,
        ppa_do_cmd,
        ppa_print_get_qnum_cmd,
        ppa_get_qnum_long_opts,
        ppa_get_qnum_short_opts,
    },
    {
        "getqmib",  // get maximum eth1 queue number
        PPA_CMD_GET_QOS_MIB,
        ppa_get_qmib_help,
        ppa_parse_get_qmib_cmd,
        ppa_get_qmib_do_cmd,
        NULL,
        ppa_get_qmib_long_opts,
        ppa_get_qmib_short_opts,
    },
    {
        "addque",  // //addque qos
        PPA_CMD_ADD_QOS_QUEUE,
        ppa_addque_help,
        ppa_parse_addque_cmd,
        ppa_do_cmd,
        NULL,
        ppa_addque_long_opts,
        ppa_addque_short_opts,
    },
    {
        "delque",  //delque qos
        PPA_CMD_DEL_QOS_QUEUE,
        ppa_delque_help,
        ppa_parse_delque_cmd,
        ppa_do_cmd,
        NULL,
        ppa_delque_long_opts,
        ppa_delque_short_opts,
    },
    {
        "modsubif2port",  // modsubif2port qos
        PPA_CMD_MOD_SUBIF_PORT,
        ppa_modsubif2port_help,
        ppa_parse_modsubif2port_cmd,
        ppa_do_cmd,
        NULL,
        ppa_modsubif2port_long_opts,
        ppa_modsubif2port_short_opts,
    },
#ifdef CONFIG_SOC_GRX500
    {
        "setgroup",  // //setgroup qos
        PPA_CMD_SET_QOS_INGGRP,
        ppa_setgroup_help,
        ppa_parse_setgroup_cmd,
        ppa_do_cmd,
        NULL,
        ppa_setgroup_long_opts,
        ppa_setgroup_short_opts,
    },
#endif
    {
        "setctrlwfq",  //set wfq to enable/disable
        PPA_CMD_SET_CTRL_QOS_WFQ,
        ppa_set_ctrl_wfq_help,
        ppa_parse_set_ctrl_wfq_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_ctrl_wfq_long_opts,
        ppa_set_ctrl_wfq_short_opts,
    },
    {
        "getctrlwfq",   //get  wfq control status---
        PPA_CMD_GET_CTRL_QOS_WFQ,
        ppa_get_ctrl_wfq_help,
        ppa_parse_get_ctrl_wfq_cmd,
        ppa_do_cmd,
        ppa_print_get_ctrl_wfq_cmd,
        ppa_get_ctrl_wfq_long_opts,
        ppa_get_ctrl_wfq_short_opts,
    },
    {
        "setwfq",  //set wfq weight
        PPA_CMD_SET_QOS_WFQ,
        ppa_set_wfq_help,
        ppa_parse_set_wfq_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_wfq_long_opts,
        ppa_set_wfq_short_opts,
    },
    {
        "resetwfq",  //reset WFQ weight
        PPA_CMD_RESET_QOS_WFQ,
        ppa_reset_wfq_help,
        ppa_parse_reset_wfq_cmd,
        ppa_do_cmd,
        NULL,
        ppa_reset_wfq_long_opts,
        ppa_reset_wfq_short_opts,
    },
    {
        "getwfq",   //get  WFQ weight
        PPA_CMD_GET_QOS_WFQ,
        ppa_get_wfq_help,
        ppa_parse_get_wfq_cmd,
        ppa_get_wfq_do_cmd,
        NULL,
        ppa_get_wfq_long_opts,
        ppa_get_wfq_short_opts,
    },
    {
        "setctrlrate",  //set rate shaping to enable/disable
        PPA_CMD_SET_CTRL_QOS_RATE,
        ppa_set_ctrl_rate_help,
        ppa_parse_set_ctrl_rate_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_ctrl_rate_long_opts,
        ppa_set_ctrl_rate_short_opts,
    },
    {
        "getctrlrate",   //get  rate shaping control status---
        PPA_CMD_GET_CTRL_QOS_RATE,
        ppa_get_ctrl_rate_help,
        ppa_parse_get_ctrl_rate_cmd,
        ppa_do_cmd,
        ppa_print_get_ctrl_rate_cmd,
        ppa_get_ctrl_rate_long_opts,
        ppa_get_ctrl_rate_short_opts,
    },
    {
        "setrate",  //set rate shaping
        PPA_CMD_SET_QOS_RATE,
        ppa_set_rate_help,
        ppa_set_rate_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_rate_long_opts,
        ppa_set_rate_short_opts,
    },
    {
        "resetrate",  //reset rate shaping
        PPA_CMD_RESET_QOS_RATE,
        ppa_reset_rate_help,
        ppa_parse_reset_rate_cmd,
        ppa_do_cmd,
        NULL,
        ppa_reset_rate_long_opts,
        ppa_reset_rate_short_opts,
    },
    {
        "getrate",   //get ate shaping
        PPA_CMD_GET_QOS_RATE,
        ppa_get_rate_help,
        ppa_parse_get_rate_cmd,
        ppa_get_rate_do_cmd,
        NULL,
        ppa_get_rate_long_opts,
        ppa_get_rate_short_opts,
    },
    {
        "setshaper",  //set shaper
        PPA_CMD_SET_QOS_SHAPER,
        ppa_set_shaper_help,
        ppa_set_shaper_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_shaper_long_opts,
        ppa_set_shaper_short_opts,
    },
    {
        "getshaper",   //get ate shaping
        PPA_CMD_GET_QOS_SHAPER,
        ppa_get_shaper_help,
        ppa_parse_get_shaper_cmd,
        ppa_get_shaper_do_cmd,
        NULL,
        ppa_get_shaper_long_opts,
        ppa_get_shaper_short_opts,
    },
#endif //end if CONFIG_PPA_QOS

    {
        "---PPA hook manipulation related commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "gethooknum",  //get all registered hook number
        PPA_CMD_GET_HOOK_COUNT,
        ppa_get_hook_count_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_hook_count_cmd,
        ppa_get_hook_long_opts,
        ppa_get_hook_short_opts,
    },
    {
        "gethooklist",  //get all registered hook list
        PPA_CMD_GET_HOOK_LIST,
        ppa_get_hook_list_help,
        ppa_parse_simple_cmd,
        ppa_get_hook_list_cmd,
        NULL,
        ppa_get_hook_list_long_opts,
        ppa_get_hook_list_short_opts,
    },
    {
        "sethook",  //enable/disable one registered hook
        PPA_CMD_SET_HOOK,
        ppa_set_hook_help,
        ppa_parse_set_hook_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_hook_long_opts,
        ppa_set_hook_short_opts,
    },

    {
        "---PPA MIB related commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "getdslmib",  // get the dsl mib
        PPA_CMD_GET_DSL_MIB,
        ppa_get_dsl_mib_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_dsl_mib_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "cleardslmib",  // get the dsl mib
        PPA_CMD_CLEAR_DSL_MIB,
        ppa_clear_dsl_mib_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        NULL,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getportmib",  // get the port mib
        PPA_CMD_GET_PORT_MIB,
        ppa_get_port_mib_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_port_mib_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "clearmib",  // get the dsl mib
        PPA_CMD_CLEAR_PORT_MIB,
        ppa_clear_port_mib_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        NULL,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
#if defined(PPA_IP_COUNTER) && PPA_IP_COUNTER
    {
		"getifacemib",  // get the interface mib
		PPA_CMD_GET_IFACE_MIB,
		ppa_get_iface_mib_help,
		ppa_parse_get_iface_mib_cmd,
		ppa_do_cmd_get_iface_mib,
		ppa_print_get_iface_mib_cmd,
		ppa_no_long_opts,
		ppa_if_mib_short_opts
    },
#endif /* PPA_IP_COUNTER */

    {
        "---PPA miscellaneous commands",
        PPA_CMD_INIT,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "sethaldbg",  // //set memory value  --hide the command
        PPA_CMD_SET_HAL_DBG_FLAG,
        ppa_set_hal_dbg_help,
        ppa_parse_set_hal_flag_cmd,
        ppa_do_cmd,
        NULL,
        ppa_set_hal_dbg_long_opts,
        ppa_set_hal_dbg_short_opts,
    },
    {
        "size",
        PPA_CMD_GET_SIZE,
        ppa_get_sizeof_help,
        ppa_parse_simple_cmd,
        ppa_do_cmd,
        ppa_print_get_sizeof_cmd,
        ppa_get_simple_long_opts,
        ppa_get_simple_short_opts
    },
    {
        "getportid",  // get the port id
        PPA_CMD_GET_PORTID,
        ppa_get_portid_help,
        ppa_parse_get_portid_cmd,
        ppa_do_cmd,
        ppa_print_get_portid_cmd,
        ppa_get_portid_long_opts,
        ppa_get_portid_short_opts
    },
    {
        "setvalue",  // set value
        PPA_CMD_SET_VALUE,
        ppa_set_variable_value_help,
        ppa_parse_set_variable_value_cmd,
        ppa_do_cmd,
        NULL,
        NULL,
        ppa_set_variable_value_short_opts
    },
    {
        "getvalue",  // get value
        PPA_CMD_GET_VALUE,
        ppa_get_variable_value_help,
        ppa_parse_get_variable_value_cmd,
        ppa_do_cmd,
        ppa_print_get_variable_value_cmd,
        NULL,
        ppa_get_variable_value_short_opts
    },
    {
        "r",
        PPA_CMD_READ_MEM,
        ppa_get_mem_help,
        ppa_parse_get_mem_cmd,
        ppa_get_mem_cmd,
        NULL,
        ppa_get_mem_long_opts,
        ppa_get_mem_short_opts,
    },
    {
        "w",
        PPA_CMD_SET_MEM,
        ppa_set_mem_help,
        ppa_parse_set_mem_cmd,
        ppa_do_cmd,
        ppa_print_set_mem_cmd,
        ppa_set_mem_long_opts,
        ppa_set_mem_short_opts,
    },
    {
        "d",
        PPA_CMD_DBG_TOOL,
        ppa_dbg_tool_help,
        ppa_parse_dbg_tool_cmd,
        NULL,
        NULL,
        NULL,
        ppa_dbg_tool_short_opts,
    },

#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
    {
        "setrtp",
        PPA_CMD_SET_RTP, 
        ppa_set_rtp_help,
        ppa_parse_set_rtp_cmd,
        ppa_do_cmd,
        NULL,
        ppa_rtp_set_long_opts,
        ppa_rtp_set_short_opts
    },
#endif

#if defined(MIB_MODE_ENABLE)
    {
        "setmibmode",
        PPA_CMD_SET_MIB_MODE, 
        ppa_set_mib_mode_help,
        ppa_parse_set_mib_mode,
        ppa_do_cmd,
        NULL,
        NULL,
        ppa_mib_mode_short_opts
    },
#endif
#if defined(PPA_TEST_AUTOMATION_ENABLE) && PPA_TEST_AUTOMATION_ENABLE
    /*Note, put all not discolsed command at the end of the array */
    {
        "automation",  // //set memory value  --hide the command
        PPA_CMD_INIT,
        ppa_test_automation_help,
        ppa_parse_test_automation_cmd,
        ppa_test_automation_cmd,
        NULL,
        ppa_test_automation_long_opts,
        ppa_test_automation_short_opts,
    },
#endif

    { NULL, 0, NULL, NULL, NULL, NULL, NULL }
};

/*
====================================================================================
  command:   ppa_cmd_help Function
  description: prints help text
  options:   argv
====================================================================================
*/
static void ppa_print_help(void)
{
    PPA_COMMAND *pcmd;
    int i;

    IFX_PPACMD_PRINT("Usage: %s <command> {options} \n", PPA_CMD_NAME);

    IFX_PPACMD_PRINT("Commands: \n");
    for(pcmd = ppa_cmd; pcmd->name != NULL; pcmd++)
    {
        if(pcmd->print_help)
        {
            //IFX_PPACMD_PRINT(" "); //it will cause wrong alignment for hidden internal commands
            (*pcmd->print_help)(0);
        }
        else  if( pcmd->name[0] == '-' || pcmd->name[0] == ' ')
        {
#define MAX_CONSOLE_LINE_LEN 80
            int filling=strlen(pcmd->name)>=MAX_CONSOLE_LINE_LEN ? 0 : MAX_CONSOLE_LINE_LEN-strlen(pcmd->name);
            IFX_PPACMD_PRINT("\n%s", pcmd->name);
            for(i=0; i<filling; i++ ) IFX_PPACMD_PRINT("-");
            IFX_PPACMD_PRINT("\n");
        }
    }

#if PPACMD_DEBUG
    IFX_PPACMD_PRINT("\n");
    IFX_PPACMD_PRINT("* Note: Create a file %s will enable ppacmd debug mode\n", debug_enable_file );
    IFX_PPACMD_PRINT("* Note: Any number inputs will be regarded as decial value without prefix 0x\n");
    IFX_PPACMD_PRINT("* Note: Please run \"ppacmd <command name> -h\" to get its detail usage\n");
    IFX_PPACMD_PRINT("\n");
#endif

    return;
}

/*
====================================================================================
  command:   ppa_cmd_help Function
  description: prints help text
  options:   argv
====================================================================================
*/
static void ppa_print_cmd_help(PPA_COMMAND *pcmd)
{
    if(pcmd->print_help)
    {
        IFX_PPACMD_PRINT("Usage: %s ", PPA_CMD_NAME);
        (*pcmd->print_help)(1);
    }
    return;
}

/*
===============================================================================
  Command processing functions
===============================================================================
*/

/*
===========================================================================================


===========================================================================================
*/
static int get_ppa_cmd(char *cmd_str, PPA_COMMAND **pcmd)
{
    int i;

    // Locate the command where the name matches the cmd_str and return
    // the index in the command array.
    for (i = 0; ppa_cmd[i].name; i++)
    {
        if (strcmp(cmd_str, ppa_cmd[i].name) == 0)
        {
            *pcmd = &ppa_cmd[i];
            return PPA_CMD_OK;
        }
    }
    return PPA_CMD_ERR;
}

/*
===========================================================================================


===========================================================================================
*/
static int ppa_parse_cmd(int ac, char **av, PPA_COMMAND *pcmd, PPA_CMD_OPTS *popts)
{
    int opt, opt_idx, ret = PPA_CMD_OK;
    int num_opts;


    // Fill out the PPA_CMD_OPTS array with the option value and argument for
    // each option that is found. If option is help, display command help and
    // do not process command.
    for (num_opts = 0; num_opts < PPA_MAX_CMD_OPTS; num_opts++)
    {
        opt = getopt_long(ac - 1, av + 1, pcmd->short_opts, pcmd->long_opts, &opt_idx);
        if (opt != -1)
        {
            if (opt == 'h')        // help
            {
                ret = PPA_CMD_HELP;
                return ret;
            }
            else if (opt == '?')      // missing argument or invalid option
            {
                ret = PPA_CMD_ERR;
                break;
            }
            popts->opt  = opt;
            popts->optarg = optarg;
            popts++;
        }
        else
            break;
    }
    return ret;
}

/*
===========================================================================================


===========================================================================================
*/
static int ppa_parse_cmd_line(int ac, char **av, PPA_COMMAND **pcmd, PPA_CMD_DATA **data)
{
    int ret = PPA_CMD_ERR;
    PPA_CMD_DATA *pdata = NULL;
    PPA_CMD_OPTS *popts = NULL;

    if ((ac <= 1) || (av == NULL))
    {
        return PPA_CMD_HELP;
    }

    pdata = malloc(sizeof(PPA_CMD_DATA));
    if (pdata == NULL)
        return PPA_CMD_NOT_AVAIL;
    ppa_memset(pdata, sizeof(PPA_CMD_DATA), 0 );

    popts = malloc(sizeof(PPA_CMD_OPTS)*PPA_MAX_CMD_OPTS);
    if (popts == NULL)
    {
        free(pdata);
        return PPA_CMD_NOT_AVAIL;
    }
    ppa_memset(popts, sizeof(PPA_CMD_OPTS)*PPA_MAX_CMD_OPTS , 0 );

    ret = get_ppa_cmd(av[1], pcmd);
    if (ret == PPA_CMD_OK)
    {
        ret = ppa_parse_cmd(ac, av, *pcmd, popts);
        if ( ret == PPA_CMD_OK )
        {
            ret = (*pcmd)->parse_options(popts,pdata);
            if ( ret == PPA_CMD_OK )
                *data = pdata;
        }
        else
        {
            IFX_PPACMD_PRINT("Wrong parameter\n");
            ret = PPA_CMD_HELP;
        }
    }
    else
    {
        IFX_PPACMD_PRINT("Unknown commands:  %s\n", av[1]);
    }
    free(popts);
    return ret;
}


/*
===========================================================================================


===========================================================================================
*/
int main(int argc, char** argv)
{
    int ret;
    PPA_CMD_DATA *pdata = NULL;
    PPA_COMMAND *pcmd=NULL;

    opterr = 0; //suppress option error messages

#if PPACMD_DEBUG
    {
        FILE *fp;

        fp = fopen(debug_enable_file, "r");
        if( fp != NULL )
        {
            enable_debug = 1;
            fclose(fp);
        }
    }
#endif

    if( argc == 1)
    {
        ppa_print_help();
        return 0;
    }
    ret = ppa_parse_cmd_line (argc, argv, &pcmd, &pdata);
    if (ret == PPA_CMD_OK)
    {
        if (pcmd->do_command)
        {
            ret = pcmd->do_command(pcmd,pdata);
            if (ret == PPA_CMD_OK && pcmd->print_data)
                pcmd->print_data(pdata);
        }
    }
    else if (ret == PPA_CMD_HELP)
    {
        ppa_print_cmd_help(pcmd);
    }
    if( pdata)
    {
        free(pdata);
        pdata=NULL;
    }
    return ret;
}
