#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <ezxml.h>
#include "qosal_inc.h"
#include "ppa_eng.h"
#include "sysapi.h"
#include <net/ppa_api.h>
#include <sys/ioctl.h>

#define PPA_DEVICE   "/dev/ifx_ppa"
#define SWITCH_DEV_NODE_2   "/dev/switch_api/2"
char sEngName[10] = "PPA";
char sPrefix[10] = "ppa";

/* =============================================================================
 *  Function Name : ppa_eng_init
 *  Description   : Callback API of PPA engine to do engine initialization.
 * ============================================================================*/
int32_t ppa_eng_init(qos_module_type_t mtype)
{
	/* Initialize switch queue related config */
	int ret = LTQ_SUCCESS;
	GSW_HW_Init_t init_cfg;
	if ((mtype == QOS_MODULE_QUEUE) || (mtype == QOS_MODULE_BOTH))
	{
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ENG_QUEUE_INIT, NULL);
		if(ret != LTQ_SUCCESS) {
			LOGF_LOG_ERROR("PPA engine initialization failed. Ioctl for engine init returned [%d]\n", ret);
			ret = QOS_PPA_ENG_CFG_FAIL;
		}
		tc_q_map_init();
	}
	if (mtype == QOS_MODULE_WMM) {
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ENG_QUEUE_INIT, NULL);
		if (ret != LTQ_SUCCESS) {
			LOGF_LOG_ERROR("PPA engine initialization failed. Ioctl for engine init returned [%d]\n", ret);
			ret = QOS_PPA_ENG_CFG_FAIL;
		}
		def_dscp_map_init();
		init_wmm_queue_cfg(1);
		ppa_add_def_wmm_dscp_class();
		if (access(ETH0_5_DIR_PATH, F_OK) == LTQ_SUCCESS) {
			qos_call_ioctl(PPA_DEVICE, PPA_MEM_SET_PORT_TRUNKING, NULL);

			memset_s(&init_cfg, sizeof(init_cfg), 0);
			init_cfg.eInitMode = GSW_HW_INIT_WR;
			qos_call_ioctl(SWITCH_DEV_NODE_2, GSW_HW_INIT, &init_cfg);
		}
	}

	if((mtype == QOS_MODULE_CLASSIFIER || mtype == QOS_MODULE_BOTH) && ret == LTQ_SUCCESS) {
		ret = xml_parse_ingress_groups();
	}

	if (mtype == QOS_MODULE_DOS) {
		ret = dos_qos_cfg(QOSAL_ENABLE);
	}

	return ret;
}

/* =============================================================================
 *  Function Name : ppa_eng_fini
 *  Description   : Callback API of PPA engine to do engine un-initialization.
 * ============================================================================*/
int32_t ppa_eng_fini(qos_module_type_t mtype)
{
	/* Unintialize switch queue config */
	int ret = LTQ_SUCCESS;
	if ((mtype == QOS_MODULE_QUEUE) || (mtype == QOS_MODULE_BOTH))
	{
	    ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ENG_QUEUE_UNINIT, NULL);
		tc_q_map_fini();
	}
	if(ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("PPA engine configuration failed. Ioctl for engine fini returned [%d]\n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
	}

	if (mtype == QOS_MODULE_WMM) {
	    qos_call_ioctl(PPA_DEVICE, PPA_CMD_ENG_QUEUE_UNINIT, NULL);
		def_dscp_map_fini();
		init_wmm_queue_cfg(0);
		ppa_del_def_wmm_dscp_class();
	}
	return ret;
}

/* =============================================================================
 *  Function Name : ppa_eng_reg
 *  Description   : Callback API of PPA engine to do engine registration.
		    Fill the structure defined 
		    by ES with capabilities and callbacks for supported operations.
 * ============================================================================*/
int32_t ppa_eng_reg(qos_engine_t *eng)
{
	/* read capabilities, fill in struct and return */
	eng->init = ppa_eng_init;
	eng->fini = ppa_eng_fini;
	eng->queue_add = ppa_eng_queue_add;
	eng->queue_modify = ppa_eng_queue_modify;
	eng->queue_delete = ppa_eng_queue_delete;
	eng->queue_stats_get = ppa_eng_get_queue_stats;
	eng->port_set = ppa_eng_port_set;
	eng->class_add = ppa_eng_class_add;
	eng->class_modify = ppa_eng_class_modify;
	eng->class_delete = ppa_eng_class_delete;

	eng->used_resource = QOS_RESOURCE_PPA;

	eng->engine_capab.q_max = PPA_MAX_Q;
	eng->engine_capab.q_len_max = PPA_MAX_Q_LEN; 
	eng->engine_capab.q_cap = PPA_Q_CAP;  /* bitmap field indicating support for individual properties of queue */
	eng->engine_capab.cl_max = PPA_MAX_CL;
	eng->engine_capab.cl_cap.cap = PPA_CL_CAP; /* bitmap field indicating support for individual properties of classifier */
	eng->engine_capab.cl_cap.cap_exclude = PPA_CL_CAP;
	eng->engine_capab.cl_cap.ext_cap = PPA_CL_EXT_CAP;
	eng->engine_capab.cl_cap.ext_cap_exclude = PPA_CL_EXT_CAP; /*PPA_CL_CAP; bitmap field indicating support for excluding individual properties of classifier */
	
	return LTQ_SUCCESS;
}

int32_t dos_qos_cfg(uint32_t oper_flag) {
	ENTRY

	dos_q_cfg(oper_flag);
	dos_meter_cfg(oper_flag);
	dos_cl_cfg(oper_flag);

	EXIT
	return LTQ_SUCCESS;
}
