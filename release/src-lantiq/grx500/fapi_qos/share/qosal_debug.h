/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_DEBUG_API_H /* [ */
#define __QOS_AL_DEBUG_API_H
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "help_logging.h"
#include "help_debug.h"
#include <commondefs.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define SCAPI_BLOCK 1

//char log_ena;
char log_type;
char log_file[MAX_NAME_LEN];

	#define ENTRY { \
		LOGF_LOG_DEBUG("Entry in %s\n", __func__); \
	}
	#define EXIT { \
		LOGF_LOG_DEBUG("Exit from %s\n", __func__); \
	}

	#define LOG_MSG(fmt, args...) { \
		LOGF_LOG_DEBUG(logstr, fmt, ##args); \
	}

	#define RUN_CMD(cmd)  { \
		LOGF_LOG_DEBUG("%s\n", cmd); \
		int32_t run_cmd_ret = LTQ_FAIL; \
		int32_t pnChildExitStatus = INT_MAX; \
		strncat_s(cmd, MAX_DATA_LEN, " > /dev/null 2> /dev/null", strnlen_s(" > /dev/null 2> /dev/null", MAX_DATA_LEN)); \
		SCAPI_SPAWN(cmd, SCAPI_BLOCK, &pnChildExitStatus, run_cmd_ret); \
		memset_s(cmd, sizeof(cmd), 0); \
	}

	#define RUN_CMD3(cmd, retval)  { \
		LOGF_LOG_DEBUG("%s\n", cmd); \
		int32_t pnChildExitStatus = INT_MAX; \
		strncat_s(cmd, MAX_DATA_LEN, " > /dev/null 2> /dev/null", strnlen_s(" > /dev/null 2> /dev/null", MAX_DATA_LEN)); \
		SCAPI_SPAWN(cmd, SCAPI_BLOCK, &pnChildExitStatus, retval); \
		memset_s(cmd, sizeof(cmd), 0); \
	}

#define PRINT_MSG(fmt, args...)

/*!<macro to call scapi spawn return UGW_FAILURE on failure*/
#define SCAPI_SPAWN(pcCommand, nBlockingFlag , pnChildExitStatus, retval) { \
	int nStatus = 0; \
	int nRet = 0; \
	pid_t pid = -1; \
	char *pArgList[] = {"/bin/sh", "-c", pcCommand, 0}; \
	*pnChildExitStatus = 0; \
	pid = fork(); \
	switch(pid) { \
		case -1: \
			nRet = -errno; \
			LOGF_LOG_ERROR( "ERROR = %d -> %s, PROCESS = QOS\n", nRet, strerror(-nRet)); \
			break; \
		case 0: \
			nRet = execve(pArgList[0], pArgList, NULL); \
			if(-1 == nRet) { \
				nRet = -errno; \
				LOGF_LOG_ERROR("ERROR = %d\n", nRet); \
			} \
			break; \
		default: \
			if(nBlockingFlag == SCAPI_BLOCK) \
			{ \
				if( waitpid(pid, &nStatus, 0) < 0 ) { \
					nRet = -errno; \
					*pnChildExitStatus = errno; \
					LOGF_LOG_ERROR("ERROR = %d\n", nRet); \
				} \
				if(WIFEXITED(nStatus)) { \
					nRet = LTQ_SUCCESS; \
					*pnChildExitStatus = WEXITSTATUS(nStatus); \
				} \
			} \
			break; \
	} \
	retval = nRet; \
	if (retval != LTQ_SUCCESS ) { \
		LOGF_LOG_ERROR( "Retval Error:[%d] Spawning :[%s]\n",retval, pcCommand); \
	} else if(*pnChildExitStatus != LTQ_SUCCESS) { \
		retval = -*pnChildExitStatus ; \
		LOGF_LOG_ERROR( "Child Exit Error:[%d] Spawning :[%s]\n",*pnChildExitStatus, pcCommand); \
	} \
	else { \
		LOGF_LOG_DEBUG( "Spawning Success[%s]\n",pcCommand); \
	} \
}

#define DBG_LEVEL SYS_LOG_DEBUG
#define DBG_TO_FILE 1
//#define MAX_DATA_LEN 1024
#define LOG_FILE "/tmp/log.txt"

#define PRINT(level, fmt, args...) { \
	if (level <= DBG_LEVEL) { \
		char logstr[MAX_DATA_LEN]; \
		snprintf(logstr, MAX_DATA_LEN, fmt, ##args); \
		if(DBG_TO_FILE == 1) { \
			FILE *fpd = fopen(LOG_FILE, "a"); \
			if(fpd != NULL ) { \
				fwrite(logstr, strnlen_s(logstr, MAX_DATA_LEN), 1, fpd); \
				fclose(fpd); \
			} \
		} \
		else { \
			fwrite(logstr, 1, strnlen_s(logstr, MAX_DATA_LEN), stdout); \
		} \
	} \
}

/* common error codes */
/*
  \brief QOS_ENG_QCNT_EQ_LT */
#define QOS_ENG_QCNT_EQ_LT		-2

/*
  \brief QOS_IF_ABS_SET_DEP_FAIL */
#define QOS_IF_ABS_SET_DEP_FAIL		-3

/*
  \brief QOS_ENTRY_EXISTS */
#define QOS_ENTRY_EXISTS		-4

/*
  \brief QOS_Q_PRIO_CLASH */
#define QOS_Q_PRIO_CLASH		-5

/*
  \brief QOS_Q_PRIO_NOT_SUPP */
#define QOS_Q_PRIO_NOT_SUPP		-6

/*
  \brief QOS_CLASSIFIER_ENTRY_EXISTS */
#define QOS_CLASSIFIER_ENTRY_EXISTS	-7

/*
  \brief QOS_IFMAP_LOOKUP_FAIL */
#define QOS_IFMAP_LOOKUP_FAIL		-8

/*
  \brief QOS_IF_ADD_FAIL */
#define QOS_IF_ADD_FAIL			-9

/*
  \brief QOS_ENG_CFG_FAIL */
#define QOS_ENG_CFG_FAIL		-10

/*
  \brief QOS_SWITCH_ENG_CFG_FAIL */
#define QOS_SWITCH_ENG_CFG_FAIL		-11

/*
  \brief QOS_PPA_ENG_CFG_FAIL */
#define QOS_PPA_ENG_CFG_FAIL		-12

/*
  \brief QOS_CPU_ENG_CFG_FAIL */
#define QOS_CPU_ENG_CFG_FAIL		-13

/*
  \brief QOS_CL_ORDER_CLASH */
#define QOS_CL_ORDER_CLASH		-14

/*
  \brief QOS_MEM_ALLOC_FAIL */
#define QOS_MEM_ALLOC_FAIL		-15

/*
  \brief QOS_IF_NOT_FOUND */
#define QOS_IF_NOT_FOUND		-16

/*
  \brief QOS_QUEUE_NOT_FOUND */
#define QOS_QUEUE_NOT_FOUND		-17

/*
  \brief QOS_CLASSIFIER_NOT_FOUND */
#define QOS_CLASSIFIER_NOT_FOUND	-18

/*
  \brief QOS_ENTRY_NOT_FOUND */
#define QOS_ENTRY_NOT_FOUND		-19

/*
  \brief QOS_DEF_Q_DEP_IN_ADD_FAIL */
#define QOS_DEF_Q_DEP_IN_ADD_FAIL	-20

/*
  \brief QOS_DEF_Q_DEP_IN_DEL_FAIL */
#define QOS_DEF_Q_DEP_IN_DEL_FAIL	-21

/*
  \brief QOS_RATE_VALID_FAIL */
#define QOS_RATE_VALID_FAIL        	-22

/*
  \brief QOS_CLASSIFIER_FOUND */
#define QOS_CLASSIFIER_FOUND        	-23

/*
  \brief QOS_Q_RESERVED */
#define QOS_Q_RESERVED			-24

/*
  \brief QOS_CL_POLICER_UNSUP_PARAM */
#define QOS_CL_POLICER_UNSUP_PARAM	-25

/*
  \brief QOS_Q_MOD_FAIL_CL_DEP*/
#define QOS_Q_MOD_FAIL_CL_DEP		-26

/*
  \brief QOS_Q_WLAN_PRIO_INVALID*/
#define QOS_Q_WLAN_PRIO_INVALID		-27

/*
  \brief QOS_CFG_DENY_FROM_LINK_CHK*/
#define QOS_CFG_DENY_FROM_LINK_CHK	-28

/*
  \brief QOS_CL_SIP_ADDR_INVALID */
#define QOS_CL_SIP_ADDR_INVALID		-29

/*
  \brief QOS_CL_SIP_MASK_INVALID */
#define QOS_CL_SIP_MASK_INVALID		-30

/*
  \brief QOS_CL_DIP_ADDR_INVALID */
#define QOS_CL_DIP_ADDR_INVALID		-31

/*
  \brief QOS_CL_DIP_MASK_INVALID */
#define QOS_CL_DIP_MASK_INVALID		-32

/*
  \brief QOS_Q_TC_CLASH */
#define QOS_Q_TC_CLASH		-33

/*
  \brief QOS_Q_RATE_INVALID */
#define QOS_Q_RATE_INVALID		-34

/*
  \brief QOS_TC_ZERO_INVALID */
#define QOS_Q_TC_ZERO_INVALID		-35

/*
  \brief QOS_IF_NOT_DEF_WAN */
#define QOS_IF_NOT_DEF_WAN		-36

#define QOS_INVALID_PARAM		-37

#define QOS_INVALID_Q_SH_PARAM		-38
#define QOS_INVALID_Q_SCHED			-39
#define QOS_INVALID_CL_ORDER_RANGE	-40
#define QOS_INVALID_SPORT			-41
#define QOS_INVALID_SPORT_RANGE		-42
#define QOS_INVALID_DPORT			-43
#define QOS_INVALID_DPORT_RANGE		-44
#define QOS_INVALID_SMAC_RANGE		-45
#define QOS_INVALID_SMAC_MASK		-46
#define QOS_INVALID_DMAC_RANGE		-47
#define QOS_INVALID_DMAC_MASK		-48
#define QOS_CL_PROTO_NOT_SET		-49
#define QOS_CL_INVALID_TC		-50
/*
  \brief QOS_SHAPER_NAME_CLASH */
#define QOS_SHAPER_NAME_CLASH		-51
/*
  \brief QOS_SHAPER_ENTRY_EXISTS */
#define QOS_SHAPER_ENTRY_EXISTS		-52

/*
  \brief QOS_INVALID_DSCP_MARK */
#define QOS_INVALID_DSCP_MARK		-53

/*
  \brief QOS_INVALID_DSCP */
#define QOS_INVALID_DSCP		-54

#define os_calloc(iNum,iSize)		help_calloc(iNum,iSize,__FILE__,__LINE__)

#define os_malloc(iSize)		help_malloc(iSize,__FILE__,__LINE__)

#define os_realloc(ptr, iSize)		help_realloc(ptr, iSize,__FILE__,__LINE__)

#define os_free(pcFree)			help_free(pcFree,__FILE__,__LINE__)

#endif
