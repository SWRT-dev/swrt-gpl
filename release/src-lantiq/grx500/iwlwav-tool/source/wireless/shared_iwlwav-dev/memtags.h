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
* This file contains memory tags for driver's 
* memory allocations. Basic rule is to have 
* separate tag for every logically connected 
* set of allocations. Ideally, separate tag 
* for every place that allocates memory.
*
* 
*
* Written by: Dmitry Fleytman
*
*/

#ifndef _MTLK_MEMTAGS_H_
#define _MTLK_MEMTAGS_H_

#define MTLK_MEM_TAG_BSS_CACHE          'cbTM'
#define MTLK_MEM_TAG_BSS_COUNTRY_IE     'ciTM'
#define MTLK_MEM_TAG_SCAN_RESULT        'scrs'
#define MTLK_MEM_TAG_SCAN_CACHE         'csTM'
#define MTLK_MEM_TAG_SCAN_VECTOR        'vsTM'
#define MTLK_MEM_TAG_TXMM_STORAGE       'stTM'
#define MTLK_MEM_TAG_TXMM_QUEUE         'qtTM'
#define MTLK_MEM_TAG_TXMM_HISTORY       'htTM'
#define MTLK_MEM_TAG_CANDIDATE          'ncTM'
#define MTLK_MEM_TAG_DPWI_CONTEXT       'cdTM'
#define MTLK_MEM_TAG_CMD                'mcTM'
#define MTLK_MEM_TAG_CMD_ARGS           'acTM'
#define MTLK_MEM_TAG_CMD_DATA           'dcTM'
#define MTLK_MEM_TAG_CMD_TEMP           'pcTM'
#define MTLK_MEM_TAG_PROC_CMD           'cpTM'
#define MTLK_MEM_TAG_REG_LIMIT          'lrTM'
#define MTLK_MEM_TAG_REG_DOMAIN         'drTM'
#define MTLK_MEM_TAG_REG_CLASS          'crTM'
#define MTLK_MEM_TAG_REG_CHANNEL        'hrTM'
#define MTLK_MEM_TAG_REG_HW_LIMITS      'shTM'
#define MTLK_MEM_TAG_REG_HW_LIMIT       'lhTM'
#define MTLK_MEM_TAG_REG_RC_LIMIT       'mrTM'
#define MTLK_MEM_TAG_ANTENNA_GAIN       'gaTM'
#define MTLK_MEM_TAG_EEPROM             'eeTM'
#define MTLK_MEM_TAG_DUT_CORE           'dcTM'
#define MTLK_MEM_TAG_CORE               'ocTM'
#define MTLK_MEM_TAG_HW                 'whTM'
#define MTLK_MEM_TAG_PSDB               'spTM'
#define MTLK_MEM_TAG_TPC_DATA           'dtTM'
#define MTLK_MEM_TAG_TPC_ANTENNA        'anTM'
#define MTLK_MEM_TAG_TPC_POINT          'ptTM'
#define MTLK_MEM_TAG_AOCS               'aocs'
#define MTLK_MEM_TAG_AOCS_ENTRY         'eaTM'
#define MTLK_MEM_TAG_AOCS_TABLE_ENTRY1  '1aTM'
#define MTLK_MEM_TAG_AOCS_TABLE_ENTRY2  '2aTM'
#define MTLK_MEM_TAG_AOCS_TABLE_ENTRY3  '3aTM'
#define MTLK_MEM_TAG_AOCS_TABLE_ENTRY4  '4aTM'
#define MTLK_MEM_TAG_AOCS_PENALTY       'paTM'
#define MTLK_MEM_TAG_AOCS_CHANNELS      'caTM'
#define MTLK_MEM_TAG_AOCS_TABLE         'taTM'
#define MTLK_MEM_TAG_AOCS_RESTR_CHNL    'raTM'
#define MTLK_MEM_TAG_AOCS_INTERF_CHNL   'ifTM'
#define MTLK_MEM_TAG_AOCS_40MHZ_INT     'iaTM'
#define MTLK_MEM_TAG_BDR                'dbTM'
#define MTLK_MEM_TAG_FILE1              '1fTM'
#define MTLK_MEM_TAG_FILE2              '2fTM'
#define MTLK_MEM_TAG_RESOURCES          'srTM'
#define MTLK_MEM_TAG_MINIPORT           'pmTM'
#define MTLK_MEM_TAG_WORKITEM           'iwTM'
#define MTLK_MEM_TAG_PCI                'pci '
#define MTLK_MEM_TAG_MIB_VALUES         'mibV'
#define MTLK_MEM_TAG_MIB                'mib '
#define MTLK_MEM_TAG_PROC               'proc'
#define MTLK_MEM_TAG_L2NAT              'l2na'
#define MTLK_MEM_TAG_IOCTL              'ioct'
#define MTLK_MEM_TAG_MCAST              'mcas'
#define MTLK_MEM_TAG_DEBUG_DATA         'dbg '
#define MTLK_MEM_TAG_COMPAT             'comp'
#define MTLK_MEM_TAG_LOGGER             'log '
#define MTLK_MEM_TAG_EXTENSION          'cemp'
#define MTLK_MEM_TAG_AOCS_HISTORY       'haTM'
#define MTLK_MEM_TAG_OBJPOOL            'poTM'
#define MTLK_MEM_TAG_IRB                'irbs'
#define MTLK_MEM_TAG_IRBCALL            'irbc'
#define MTLK_MEM_TAG_IRBNOTIFY          'irbn'
#define MTLK_MEM_TAG_IRB_DATA           'irbd'
#define MTLK_MEM_TAG_IRB_DESC           'irbe'
#define MTLK_MEM_TAG_IRB_OBJ            'irbo'
#define MTLK_MEM_TAG_HASH               'hash'
#define MTLK_MEM_TAG_RFMGMT             'rfmg'
#define MTLK_MEM_TAG_CONTAINER          'cont'
#define MTLK_MEM_TAG_EQ                 'eq  '
#define MTLK_MEM_TAG_PROGMODEL          'prgm'
#define MTLK_MEM_TAG_CLI_CLT            'clic'
#define MTLK_MEM_TAG_CLI_SRV            'clis'
#define MTLK_MEM_TAG_STRTOK             'stok'
#define MTLK_MEM_TAG_ARGV_PARSER        'argv'
#define MTLK_MEM_TAG_TPC4               'tpc4'
#define MTLK_MEM_TAG_CDEV               'cdev'
#define MTLK_MEM_TAG_DF                 'dfhw'
#define MTLK_MEM_TAG_DF_SLOW            'dfsl'
#define MTLK_MEM_TAG_DF_FW              'dffw'
#define MTLK_MEM_TAG_STADB_ITER         'stdi'
#define MTLK_MEM_TAG_HSTDB_ITER         'hsdi'
#define MTLK_MEM_TAG_SERIALIZER         'mszr'
#define MTLK_MEM_TAG_UI_DATA_BUF        'uibf'
#define MTLK_MEM_TAG_CLPB               'clpb'
#define MTLK_MEM_TAG_ASYNC_CTX          'mcTM'
#define MTLK_MEM_TAG_DFS                'dfs '
#define MTLK_MEM_TAG_CORE_INVOKE        'icTM'
#define MTLK_MEM_TAG_PARAM_DB           'bdTM'
#define MTLK_MEM_TAG_CHANNEL            'chnl'
#define MTLK_MEM_TAG_FFAREA             'ffar'
#define MTLK_MEM_TAG_FW_LOGGER          'fwlo'
#define MTLK_MEM_TAG_BSS_MGMT           'bssm'
#define MTLK_MEM_TAG_VAP_MANAGER        'mvTM'
#define MTLK_MEM_TAG_VAP_NODE           'vnTM'
#define MTLK_MEM_TAG_CORE_CFG           'ccTM'
#define MTLK_MEM_TAG_FLCTRL             'flcl'
#define MTLK_MEM_TAG_NBUF_STATS         'nbst'
#define MEM_TAG_ABILITY_RECORD          'raTM'
#define MTLK_MEM_TAG_ABMGR              'maTM'
#define MTLK_MEM_TAG_WSS                'mwss'
#define MTLK_MEM_TAG_COEX_20_40         '2040'
#define MTLK_MEM_TAG_MTDUMP_STATS       'sdTM'
#define MTLK_MEM_TAG_MTIDL              'liTM'
#define LQLA_MEM_TAG_WDS                'wds '
#define LQLA_MEM_TAG_TA                 'ta  '
#define MTLK_MEM_TAG_NVM_ACCESS         'dnvm'
#define LQLA_MEM_TAG_FW_RECOVERY        'rcvr'
#define MTLK_MEM_TAG_BEACON_DATA        'bdTM'
#define MTLK_MEM_TAG_BEACON_MNGR        'bmTM'
#define MTLK_MEM_TAG_CFG80211           'cfg8'
#define MTLK_MEM_TAG_MAC80211           'mac8'
#define MTLK_MEM_TAG_CALIB_DATA         'clbr'
#define MTLK_MEM_TAG_SCAN_DATA          'scan'
#define MTLK_MEM_TAG_VERSION_PRINTOUT   'verp'
#define MTLK_MEM_TAG_MCAST_GROUPS       'mcgr'
#define MTLK_MEM_TAG_MCAST_VAP          'mcva'
#define MTLK_MEM_TAG_MCAST_RANGE        'mcrn'
#define MTLK_MEM_TAG_PPA                'ppa '
#define LQLA_MEM_TAG_PMCU               'pmcu'
#define LQLA_MEM_TAG_WIDAN_BLACKLIST    'wdbl'
#define MTLK_MEM_TAG_US_STATS           'usst'
#define MTLK_MEM_TAG_DCDP               'dcdp'
#define LQLA_MEM_TAG_FOUR_ADDR_LIST     '4adl'
#define MTLK_MEM_TAG_IEEE_ADDR_LIST     'ieal'
#define MTLK_MEM_TAG_PROP_DFS_STATE     'pdfs'
#define MTLK_MEM_TAG_VENDOR_CMD_DATA    'vcmd'
#define MTLK_MEM_TAG_TIMER              'timr'
#define MTLK_MEM_TAG_CSA_DEAUTH_DATA    'csde'
#define MTLK_MEM_TAG_USER_DF            'fmdf'
#define MTLK_MEM_TAG_USER_CORE          'fmuc'
#define MTLK_MEM_TAG_USER_MMBDRV        'fmum'
#define MTLK_MEM_TAG_USER_VAPMGR        'fmuv'
#define MTLK_MEM_TAG_USER_RADIO         'fmur'
#define MTLK_MEM_TAG_FW_LOGS            'fwlg'

#endif //_MTLK_MEMTAGS_H_

