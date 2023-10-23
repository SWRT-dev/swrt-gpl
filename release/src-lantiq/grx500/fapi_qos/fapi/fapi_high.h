/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "commondefs.h"
#include "qosal_queue_api.h"

int32_t
fapi_qos_cfg_set(qos_cfg_t *qoscfg, uint32_t flags);
int32_t
fapi_qos_cfg_get(qos_cfg_t *qoscfg, uint32_t flags);
int32_t
fapi_qos_queue_set(char	*ifname, qos_queue_cfg_t *q, uint32_t flags);
int32_t
fapi_qos_queue_get(char *ifname, char* queue_name, int32_t *num_queues,
		qos_queue_cfg_t	**q, uint32_t flags);
int32_t
fapi_qos_port_config_set(char *ifname, qos_shaper_t *shaper, int32_t weight,
			int32_t	priority, uint32_t flags);
int32_t
fapi_qos_port_config_get(char *ifname, qos_shaper_t *shaper, int32_t *weight,
		int32_t	*priority, uint32_t flags);
int32_t
fapi_qos_port_update(char *ifname, uint32_t flags);
