/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "stdio.h"
#include "qosal_inc.h"
#include "cpu_n_eng.h"
#include "qosal_ugw_debug.h"

char sEngName[10] = "CPU-N";
char sPrefix[10] = "cpu_n";
extern uint16_t LOGTYPE, LOGLEVEL;

/* =============================================================================
 *  Function Name : cpu_n_eng_reg
 *  Description   : Registration API for CPU-N engine. Fill the structure defined
		    by ES with capabilities and callbacks for supported operations.
 * ============================================================================*/
int32_t cpu_n_eng_reg(qos_engine_t *eng)
{
	/* read capabilities, fill in struct and return */
	ENTRY
	eng->init = cpu_n_eng_init;
	eng->fini = cpu_n_eng_fini;
	eng->queue_add = cpu_n_eng_queue_add;
	eng->queue_modify = cpu_n_eng_queue_modify;
	eng->queue_delete = cpu_n_eng_queue_delete;
	eng->queue_stats_get = cpu_n_eng_stats_get;
	eng->port_set = cpu_n_eng_port_set;
	eng->class_add = cpu_classAdd;
	eng->class_modify = cpu_classModify;
	eng->class_delete = cpu_classDel;

	eng->used_resource = QOS_RESOURCE_CPU;

	eng->engine_capab.q_max = CPU_N_MAX_Q;
	eng->engine_capab.q_len_max = CPU_N_MAX_Q_LEN;
	eng->engine_capab.cl_max = CPU_N_MAX_CL;
	eng->engine_capab.q_cap = CPU_N_Q_CAP; /* bitmap field indicating support for individual properties of queue */
	eng->engine_capab.cl_cap.cap = CPU_N_CL_CAP; /* bitmap field indicating support for individual properties of classifier */
	eng->engine_capab.cl_cap.cap_exclude = CPU_N_CL_CAP; /* bitmap field indicating support for excluding individual properties of classifier */
	eng->engine_capab.cl_cap.ext_cap = CPU_N_CL_EXT_CAP; /* bitmap field indicating support for individual properties of classifier */
	eng->engine_capab.cl_cap.ext_cap_exclude = CPU_N_CL_EXT_CAP; /* bitmap field indicating support for individual properties of classifier */

	EXIT
	return LTQ_SUCCESS;
}
