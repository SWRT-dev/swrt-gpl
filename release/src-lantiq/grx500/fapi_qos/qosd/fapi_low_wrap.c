#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ulogging.h>
//#include <ugw_proto.h> /* TODO: Add dependency on libugwhelper in feed */
#include "qosal_inc.h"
#include "commondefs.h"

#include <netdb.h>
#include <ctype.h>

#define UGW_SUCCESS 0
#define UGW_FAILURE -1

/* Maximum permissible file size/ budffer len*/
#define MAX_FILE_SIZE		131072 // 128 * 1024

/* Flag to denote absence of Delta Section from cfg file. */
#define UGW_NO_DELTA_CFG	3

/* QoS daemon config file path */
#define QOS_DAEMON_FILE		"/tmp/qosd/qos.conf" /* This can be later put in the feed also. That will make it easier to change for customers */

/* maximum length for daemon config file lines */
#define MAX_OPTION_LEN		128

/* Common Strings */
#define DELTA_SECTION_START	"[deltacfg]"

#define HELP_FREE_BUF(buf) { \
	free(buf); \
	buf = NULL; \
}

/* Enum for type of entity */
typedef enum {
	QOS_ENTITY_QOSCFG = 0,		/* Entity corresonding to  QoS basic configuration section */
	QOS_ENTITY_QUEUE = 1,		/* Entity corresponding to Queue/Class configuration section */
	QOS_ENTITY_CLASSIFIER = 2,	/* Entity corresponding to Classifier/Classify configuration section */
	QOS_ENTITY_PORT_SHAPER = 3,	/* Entity corresponding to Port Shaper configuration section */
	QOS_ENTITY_DELTACFG = 4,	/* Entity corresponding to Deltacfg section */
	QOS_ENTITY_MAX = 5,
} qos_entity_t;

const char entityHeaders[QOS_ENTITY_MAX + 1][MAX_OPTION_LEN] = {
															"[qoscfg]",
															"[queue]",
															"[classifier]",
															"[shaper]",
															"[deltacfg]",
															"[invalid]"
															};

/* Enum for operations */
typedef enum {
	QOS_CFG_ADD = 1,
	QOS_CFG_DEL = 2,
	QOS_CFG_MOD = 3,
	QOS_CFG_NOOPER = 4,
} qos_cfg_oper_t;

extern es_tables_t es_tables;
extern uint16_t LOGTYPE, LOGLEVEL;

static int32_t read_cfg_file(char **buffer, size_t *buf_len);
static int32_t read_deltaSection(char **buffer, size_t buf_len, bool *deltaPresent);
static int32_t parse_deltaSection(char *delta_buffer, size_t buf_len, qos_entity_t *entity, uint32_t *entityId, char *old_entity, char *ifname, char *oldIfname, uint32_t *oper_flags);
static int32_t parse_singleLine(char *line, uint32_t *uInstance, char *sOption, char *sValue);
static int32_t read_deltaEntity(char *buffer, size_t buf_len, void **cfg, qos_entity_t entity, uint32_t entityId, uint32_t *flags);
static int32_t parse_qosSection(char *buffer, size_t buf_len, void **cfg, uint32_t *flags);
static int32_t parse_queueSection(char *buffer, size_t buf_len, void **cfg, char *entityName, char *ifname, uint32_t *flags);
static int32_t parse_classifierSection(char *buffer, size_t buf_len, void **cfg, char *entityName, char *ifname, uint32_t *flags);
static int32_t parse_shaperSection(char *buffer, size_t buf_len, void **cfg, char *entityName, char *ifname, uint32_t *flags);
static int32_t set_qos_entity(IN char *buffer, IN size_t buf_len, IN void **cfg, IN qos_entity_t entity, IN char *ifname, IN char *old_entity, IN char *oldIfName, IN uint32_t flags);
int32_t qosd_set_qos_config(IN void *cfg, IN qos_entity_t entity, IN char *ifname, IN uint32_t flags);
static int32_t qosd_set_base_abs(IN int32_t iftype, IN char *ifname, IN char *base_if, IN char *logical_base, IN uint32_t flags);
static int32_t qos_basic_set_wrap(char *buffer, size_t buf_len, qos_cfg_t *qoscfg, uint32_t flags);
static int32_t get_queue_cfg_node(char *ifname, char *q_name, qos_queue_node_t **q_list_node, uint32_t *flags);
static int32_t qos_queue_set_wrap(IN qos_queue_cfg_t *q, IN char *ifname, IN char *old_q_name, IN char *oldIfName, uint32_t flags);
static int32_t qos_classifier_set_wrap(IN qos_class_cfg_t *class_cfg, IN char *ifname, IN char *old_class_name, IN char *oldIfName, IN uint32_t flags);
static int32_t qos_shaper_set_wrap(IN qos_shaper_t *sh, IN char *ifname, IN char *old_sh_Name, IN char *oldIfName, IN uint32_t flags);
static void clcfg_setParamIncExc(int32_t exclude_req, int32_t param , cl_param_in_ex_t *pxparaminex);
static void clcfg_setPort(char *sPort, int32_t *start_port, int32_t *end_port);
static int32_t parse_Section(IN qos_entity_t entityType, OUT char *entity_buffer, OUT size_t buf_len, OUT void **cfg, OUT char *entityName, OUT char *ifname, OUT uint32_t *flags);
static int32_t set_all_entities_without_delta(char **buffer, size_t buf_len);
static int32_t isIPAddrValid(char *sIP);
static int32_t validate_string(char *str);
static int32_t isMacValid(char *mac_addr);

/* =============================================================================
 *  Function Name : qosd_set_qoscfg
 *  Description   : Parses the daemon config file and congigure the fapi and
			system accordingly.
 * ============================================================================*/
int32_t qosd_set_qoscfg(void) {
	ENTRY
    int32_t nRet = UGW_SUCCESS;
    char *cfg_buffer = NULL;
	size_t buffer_len = 0;
	bool deltaPresent = false;

    nRet = read_cfg_file(&cfg_buffer, &buffer_len);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Error in reading daemon config file.\n");
		goto exit_lbl;
	}

	nRet = read_deltaSection(&cfg_buffer, buffer_len, &deltaPresent);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Error in reading delta section.\n");
		goto exit_lbl;
	}

	if (deltaPresent == false) {
		LOGF_LOG_DEBUG("No delta section found in file, Configuring all entities in system!\n");
		nRet = set_all_entities_without_delta(&cfg_buffer, buffer_len);
		if (nRet != UGW_SUCCESS) {
			LOGF_LOG_ERROR("Error in configuring all entities in system!\n");
			goto exit_lbl;
		}
	}

exit_lbl:
	if (cfg_buffer != NULL)
		free(cfg_buffer);
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : read_cfg_file
 *  Description   : Reads the daemon config file into the buffer
 * ============================================================================*/
static int32_t read_cfg_file(char **buffer, size_t *buf_len) {
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	FILE *fp;
	long int file_size;

	fp = fopen(QOS_DAEMON_FILE, "rx");
	if (fp == NULL) {
		LOGF_LOG_ERROR("Daemon config file not present\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	if (fseek(fp, 0, SEEK_END) != 0) {
		LOGF_LOG_ERROR("Failure to traverse daemon config file\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	file_size = ftell(fp);
	if (file_size <= 0 || file_size > MAX_FILE_SIZE) {
		LOGF_LOG_ERROR("Invalid file size of daemon config file\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}
	LOGF_LOG_DEBUG("File size : [%ld]\n", file_size);
	if (fseek(fp, 0, SEEK_SET) != 0) {
		LOGF_LOG_ERROR("Failure to traverse daemon config file\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	*buffer = (char *)malloc(sizeof(char) * (file_size + 1));
	if (*buffer == NULL) {
		LOGF_LOG_ERROR("Memory allocation failed!\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	*buf_len = fread(*buffer, sizeof(char), (uint32_t)file_size, fp);
	if (*buf_len != (uint32_t)file_size) {
		LOGF_LOG_ERROR("Not able to read full file\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}
	(*buffer)[*buf_len] = '\0'; //For Safety

exit_lbl:
	if (fp != NULL)
		fclose(fp);
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : read_deltaSection
 *  Description   : Reads the daemon config file into the buffer
 * ============================================================================*/
static int32_t read_deltaSection(char **buffer, size_t buf_len, bool *deltaPresent)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	char *token, *next_token, *delta_buffer = NULL, *tmp_buffer = NULL;
	rsize_t strmax = buf_len;
	size_t delta_buf_len;

	char old_entity[MAX_Q_NAME_LEN] = {0};
	char ifname[MAX_IF_LEN] = {0}, oldIfname[MAX_IF_LEN] = {0};
	uint32_t entityId = 0;
	qos_entity_t entity = QOS_ENTITY_MAX;
	qos_cfg_oper_t oper = QOS_CFG_NOOPER;
	UNUSED(oper);
	uint32_t oper_flags = 0;
	void *cfg = NULL;

	if (*buffer == NULL || buf_len == 0) {
		LOGF_LOG_ERROR("Buffer is empty!\n");
		goto exit_lbl;
	}

	tmp_buffer = (char *)malloc(sizeof(char) * (buf_len + 1));
	if (tmp_buffer == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed!\n");
		goto exit_lbl;
	}
	memset_s(tmp_buffer, buf_len + 1, '\0');
	strncpy_s(tmp_buffer, buf_len + 1, *buffer, buf_len);

	*deltaPresent = false;

	token = strtok_s(tmp_buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		if ((strcmp(token , entityHeaders[QOS_ENTITY_DELTACFG]) == 0) && (*deltaPresent == false)) {
			*deltaPresent = true;
			delta_buffer = (char *)malloc(sizeof(char) * buf_len);
			if (delta_buffer == NULL) {
				nRet = UGW_FAILURE;
				LOGF_LOG_ERROR("Memory allocation failed\n");
				goto exit_lbl;
			}
			memset_s(delta_buffer, buf_len, '\0');
			token = strtok_s(NULL, &strmax, "\n", &next_token);
			continue;
		}
		if (*deltaPresent == true) {
			if (token[0] == '[') //Denotes start of another section
				break;

			strncat_s(delta_buffer, buf_len, token, strnlen_s(token, buf_len));
			strncat_s(delta_buffer, buf_len, "\n", 2);
		}
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	if (*deltaPresent) {
		delta_buf_len = strnlen_s(delta_buffer, buf_len);
		if (delta_buf_len == 0) {
			LOGF_LOG_ERROR("Failed to get the buffer length!\n");
			nRet = UGW_FAILURE;
			goto exit_lbl;
		}
		nRet = parse_deltaSection(delta_buffer, delta_buf_len, &entity, &entityId, old_entity, ifname, oldIfname, &oper_flags);

		memset_s(tmp_buffer, buf_len + 1, '\0');
		strncpy_s(tmp_buffer, buf_len + 1, *buffer, buf_len);
		if (oper_flags != QOS_OP_F_DELETE) {
			nRet = read_deltaEntity(tmp_buffer, buf_len, &cfg, entity, entityId, &oper_flags);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Error in reading delta entity!\n");
				goto exit_lbl;
			}
		}

		memset_s(tmp_buffer, buf_len + 1, '\0');
		strncpy_s(tmp_buffer, buf_len + 1, *buffer, buf_len);
		nRet = set_qos_entity(tmp_buffer, buf_len, &cfg, entity, ifname, old_entity, oldIfname, oper_flags);
	}

exit_lbl:
	if (delta_buffer != NULL)
		free(delta_buffer);
	if (cfg != NULL)
		free(cfg);
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : parse_deltaSection
 *  Description   : Parses the delta section options to appropriate variables.
 * ============================================================================*/
static int32_t parse_deltaSection(char *delta_buffer, size_t buf_len, qos_entity_t *entity, uint32_t *entityId, char *old_entity, char *ifname, char *oldIfname, uint32_t *oper_flags)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	char *token, *next_token;
	rsize_t strmax = buf_len;

	uint32_t uInstance = 0;
	char sOption[MAX_OPTION_LEN] = {0};
	char sValue[MAX_OPTION_LEN] = {0};

	token = strtok_s(delta_buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		memset_s(sOption, MAX_OPTION_LEN, '\0');
		memset_s(sValue, MAX_OPTION_LEN, '\0');
		uInstance = 0;
		nRet = parse_singleLine(token, &uInstance, sOption, sValue);
		if (strcmp(sOption, "entity") == 0) {
			if (strcmp(sValue, "qoscfg") == 0)
				*entity = QOS_ENTITY_QOSCFG;
			else if (strcmp(sValue, "class") == 0)
				*entity = QOS_ENTITY_QUEUE;
			else if (strcmp(sValue, "classify") == 0)
				*entity = QOS_ENTITY_CLASSIFIER;
			else if (strcmp(sValue, "interface") == 0)
				*entity = QOS_ENTITY_PORT_SHAPER;
			else if (strcmp(sValue, "deltacfg") == 0)
				*entity = QOS_ENTITY_DELTACFG;
			else
				*entity = QOS_ENTITY_MAX;
		}
		else if(strcmp(sOption, "entityid") == 0) {
			*entityId = (uint32_t)strtoul(sValue, NULL, 10);
		}
		else if(strcmp(sOption, "operation") == 0) {
			if (strcmp(sValue, "ADD") == 0)
				*oper_flags |= QOS_OP_ADD;
			else if (strcmp(sValue, "MOD") == 0)
				*oper_flags |= QOS_OP_MODIFY;
			else if (strcmp(sValue, "DEL") == 0)
				*oper_flags |= QOS_OP_DELETE;
			else
				*oper_flags |= QOS_OP_NOOPER;
		}
		else if(strcmp(sOption, "oldentityid") == 0) {
			strncpy_s(old_entity, MAX_Q_NAME_LEN, sValue, MAX_Q_NAME_LEN);
		}
		else if(strcmp(sOption, "interface") == 0) {
			strncpy_s(ifname, MAX_IF_LEN, sValue, MAX_IF_LEN);
		}
		else if(strcmp(sOption, "oldinterface") == 0) {
			strncpy_s(oldIfname, MAX_IF_LEN, sValue, MAX_IF_LEN);
		}

		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : parse_singleLine
 *  Description   : Parses single line and provides the instance, option and
 *				value present in that line.
 * ============================================================================*/
static int32_t parse_singleLine(char *line, uint32_t *uInstance, char *sOption, char *sValue)
{
	int32_t nRet = UGW_SUCCESS;
	uint32_t i, j;
	char sInstance[MAX_OPTION_LEN] = {0};
	char *from, *to;

	i = j = 0;
	from = to = NULL;

/*	nRet = validate_singleLine(line);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERORR("Validation failed!\n");
		nRet = UGW_FAILURE:
		goto exit_lbl;
	}
*/
	while (line[i] != '\0') {
		if (line[i] == '_') {
			from = line + i + 1;
			while (from[j] != '_') {
				j++;
			}
			if (uInstance != NULL) {
				strncpy_s(sInstance, MAX_OPTION_LEN, from, j);
				*uInstance = (uint32_t) strtoul(sInstance, NULL, 10);
			}

			from = from + j + 1;
			j = 0;
			while (from[j] != '=') {
				j++;
			}
			if (sOption != NULL) {
				strncpy_s(sOption, MAX_NAME_LEN, from, j);
			}

			from = from + j + 1;
			j = 0;
			while (from[j] != '\0') {
				j++;
			}
			if (sValue != NULL) {
				strncpy_s(sValue, MAX_NAME_LEN, from, j);
			}
			break;
		}
		i++;
	}

	return nRet;
}

static int32_t read_deltaEntity(char *buffer, size_t buf_len, void **cfg, qos_entity_t entity, uint32_t entityId, uint32_t *flags)
{
	ENTRY

	int32_t nRet = UGW_SUCCESS;
	char *token, *next_token;
	rsize_t strmax = buf_len;
	bool match = false;
	uint32_t instance = 0;
	char *entity_buffer = (char *)malloc(sizeof(char) * buf_len);
	if (entity_buffer == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed to buffer\n");
		goto exit_lbl;
	}
	memset_s(entity_buffer, buf_len, '\0');

	if (entity >= QOS_ENTITY_MAX) {
		LOGF_LOG_ERROR("Invalid Entity!\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		if ((strcmp(token, entityHeaders[entity]) == 0) && (match == false)) {
			match = true;
			token = strtok_s(NULL, &strmax, "\n", &next_token);
			continue;
		}

		if (match == true) {
			nRet = parse_singleLine(token, &instance, NULL, NULL);
			if (instance != entityId) {
				match = false;
				token = strtok_s(NULL, &strmax, "\n", &next_token);
				continue;
			}
			while (token != NULL) {
				if (token[0] == '[')
					break;
				strncat_s(entity_buffer, buf_len, token, strnlen_s(token, buf_len));
				strncat_s(entity_buffer, buf_len, "\n", 2);
				token = strtok_s(NULL, &strmax, "\n", &next_token);
			}
			break;

		}
		match = false;
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	nRet = parse_Section(entity, entity_buffer, buf_len, cfg, NULL, NULL, flags);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Failed to parse the given entity!\n");
		goto exit_lbl;
	}

exit_lbl:
	if (entity_buffer != NULL)
		free(entity_buffer);
	EXIT
	return nRet;
}

static int32_t parse_qosSection(char *buffer, size_t buf_len, void **cfg, uint32_t *flags) {
	ENTRY
	int32_t nRet = UGW_FAILURE;
	qos_cfg_t *qoscfg = (qos_cfg_t *)malloc(sizeof(qos_cfg_t));
	if (qoscfg == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory Allocation Failed!\n");
		goto exit_lbl;
	}
	char *token, *next_token;
	rsize_t strmax = strnlen_s(buffer, buf_len);
	uint32_t uInstance = 0;
	char sOption[MAX_OPTION_LEN] = {0}, sValue[MAX_OPTION_LEN] = {0};

	memset_s(qoscfg, sizeof(qos_cfg_t), 0);

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	LOGF_LOG_DEBUG("Token:[%s]\n", token);
	while (token != NULL) {
		memset_s(sOption, MAX_OPTION_LEN, '\0');
		memset_s(sValue, MAX_OPTION_LEN, '\0');
		uInstance = 0;

		nRet = parse_singleLine(token, &uInstance, sOption, sValue);
		LOGF_LOG_DEBUG("Token:[%s] Instance:[%u] Option:[%s] Value:[%s]\n", token, uInstance, sOption, sValue);

		if (strcmp(sOption, "enable") == 0) {
			qoscfg->ena = (int32_t)strtoul(sValue, NULL, 10);
		}
		else if (strcmp(sOption, "loglevel") == 0) {
			qoscfg->log_level = (int32_t)strtoul(sValue, NULL, 10);
		}
		else if (strcmp(sOption, "logredirect") == 0) {
			qoscfg->log_type = (log_type_t)strtoul(sValue, NULL, 10);
		}
		else if (strcmp(sOption, "logfile") == 0) {
			strncpy_s(qoscfg->log_file, MAX_NAME_LEN, sValue, MAX_NAME_LEN);
		}
		else if (strcmp(sOption, "defwandscp") == 0) {
			qoscfg->def_wan_dscp = (uint32_t)strtoul(sValue, NULL, 10);
		}
		else if (strcmp(sOption, "wmmenable") == 0) {
			qoscfg->wmm_ena = (int32_t)strtoul(sValue, NULL, 10);
		}
		else if (strcmp(sOption, "mgmtenable") == 0) {
			qoscfg->mgmt_ena = (int32_t)strtoul(sValue, NULL, 10);
		}

		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	if (qoscfg->ena == 1) {
		*flags |= QOS_OP_F_ADD;
	} else {
		*flags |= QOS_OP_F_DELETE;
	}
		
	*cfg = (void *)qoscfg;

exit_lbl:
	EXIT
	return nRet;
}

static int32_t parse_queueSection(char *buffer, size_t buf_len, void **cfg, char *qName, char *ifname, uint32_t *flags) {
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	qos_queue_cfg_t *qcfg = (qos_queue_cfg_t *)malloc(sizeof(qos_queue_cfg_t));
	if (qcfg == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed\n");
		goto exit_lbl;
	}
	char *token, *next_token;
	rsize_t strmax = strnlen_s(buffer, buf_len);
	uint32_t uInstance = 0;
	char sOption[MAX_OPTION_LEN] = {0}, sValue[MAX_OPTION_LEN] = {0};

	char ifname2[MAX_IF_NAME_LEN] = {0}, *endptr = NULL;
	char logical_ifname[MAX_IF_NAME_LEN] = {0}, base_ifname[MAX_IF_NAME_LEN] = {0};
	iftype_t ifgroup = QOS_IF_CATEGORY_MAX;

	memset_s(qcfg, sizeof(qos_queue_cfg_t), 0);

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		memset_s(sOption, MAX_OPTION_LEN, '\0');
		memset_s(sValue, MAX_OPTION_LEN, '\0');
		uInstance = 0;
		nRet = parse_singleLine(token, &uInstance, sOption, sValue);
		if (strcmp(sOption, "qname") == 0) {
			strncpy_s(qcfg->name, MAX_Q_NAME_LEN, sValue, MAX_Q_NAME_LEN);
			nRet = validate_string(qcfg->name);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Queue Name is not valid\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			if (qName != NULL) {
				strncpy_s(qName, MAX_Q_NAME_LEN, sValue, MAX_Q_NAME_LEN);
				LOGF_LOG_DEBUG("Queue Name is : [%s]\n", qName);
			}
		}
		else if (strcmp(sOption, "status") == 0) {
			qcfg->enable = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid queue length input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "owner") == 0) {
			qcfg->owner = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid queue length input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "maxsize") == 0) {
			qcfg->qlen = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(qcfg->qlen, 0, 102400))){
				LOGF_LOG_ERROR("Invalid queue length input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "qprio") == 0) {
			qcfg->priority = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(qcfg->priority, MIN_Q_PRIO_RANGE, MAX_Q_PRIO_RANGE))){
				LOGF_LOG_ERROR("Invalid queue priority input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "tc") == 0) {
			qcfg->tc_map[0] = (int8_t)strtoul(sValue, &endptr, 10); /* TODO: Handle multiple TC value */
			if(*endptr != 0 || (!VALIDATE_RANGE(qcfg->tc_map[0], 1, MAX_TC_NUM))){
				LOGF_LOG_ERROR("Invalid queue Traffic Class input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "weight") == 0) {
			qcfg->weight = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(qcfg->weight,0, MAX_Q_WEIGHT))){
				LOGF_LOG_ERROR("Invalid queue weight input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "drop") == 0) {
			if (strcmp(sValue, "DT") == 0)
				qcfg->drop.mode = QOS_DROP_TAIL;
			else if (strcmp(sValue, "RED") == 0)
				qcfg->drop.mode = QOS_DROP_RED;
			else if (strcmp(sValue, "WRED") == 0)
				qcfg->drop.mode = QOS_DROP_WRED;
		}
		else if (strcmp(sOption, "policy") == 0) {
			if (strcmp(sValue, "SP") == 0)
				qcfg->sched = QOS_SCHED_SP;
			else if (strcmp(sValue, "WFQ") == 0)
				qcfg->sched = QOS_SCHED_WFQ;
		}
		else if (strcmp(sOption, "peakrate") == 0) {
			qcfg->shaper.pir = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (qcfg->shaper.pir >MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid queue peakrate input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			qcfg->shaper.enable = 1;
			qcfg->shaper.mode = QOS_SHAPER_COLOR_BLIND;
		}
		else if (strcmp(sOption, "commitrate") == 0) {
			qcfg->shaper.cir = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (qcfg->shaper.cir > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid queue commitrate input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "peakburst") == 0) {
			qcfg->shaper.pbs = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (qcfg->shaper.pbs > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid queue peakburst input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			qcfg->shaper.enable = 1;
			qcfg->shaper.mode = QOS_SHAPER_COLOR_BLIND;
		}
		else if (strcmp(sOption, "commitburst") == 0) {
			qcfg->shaper.cbs = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (qcfg->shaper.cbs > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid queue commitburst input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "inggrp") == 0) {
			if (strcmp(sValue, "Group1") == 0)
				qcfg->inggrp = QOS_INGGRP_1;
			else if (strcmp(sValue, "Group2") == 0)
				qcfg->inggrp = QOS_INGGRP_2;
			else if (strcmp(sValue, "Group3") == 0)
				qcfg->inggrp = QOS_INGGRP_3;
			else if (strcmp(sValue, "Group4") == 0)
				qcfg->inggrp = QOS_INGGRP_4;
		}
		else if (strcmp(sOption, "flags") == 0) {
			qcfg->flags = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid queue flags input\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			if (((qcfg->flags & QOS_Q_F_INGRESS) == QOS_Q_F_INGRESS) && flags != NULL)
				*flags |= QOS_OP_F_INGRESS;
		}
		else if ((strcmp(sOption, "oper") == 0) && flags != NULL) {
			if (strcmp(sValue, "ADD") == 0)
				*flags |= QOS_OP_ADD;
			else if (strcmp(sValue, "MOD") == 0)
				*flags |= QOS_OP_MODIFY;
			else if (strcmp(sValue, "DEL") == 0)
				*flags |= QOS_OP_DELETE;
			else
				*flags |= QOS_OP_NOOPER;
		}
		else if (strcmp(sOption, "ifname") == 0) {
			nRet = validate_string(sValue);
			if (nRet != UGW_SUCCESS){
				LOGF_LOG_ERROR("Invalid queue interface Name\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(ifname2, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			if (ifname != NULL) {
				strncpy_s(ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			}
		}
		else if (strcmp(sOption, "logicalifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS){
				LOGF_LOG_ERROR("Invalid queue logical interface Name\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(logical_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "phyifname") == 0) {
			nRet = validate_string(sValue);
			if (nRet != UGW_SUCCESS){
				LOGF_LOG_ERROR("Invalid phyifname interface Name\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(base_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "iftype") == 0) {
			ifgroup = (iftype_t) strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(ifgroup, 0, QOS_IF_CATEGORY_MAX))){
				LOGF_LOG_ERROR("Invalid queue interface type\n");
				HELP_FREE_BUF(qcfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	*cfg = (void *)qcfg;

	nRet = qosd_set_base_abs(ifgroup, ifname2, base_ifname, logical_ifname, QOS_OP_F_ADD);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Setting interface group failed!\n");
		goto exit_lbl;
	}

	if (qcfg->flags & QOS_Q_F_INGRESS) {
		nRet = qosd_if_inggrp_set(qcfg->inggrp, ifname2, QOS_OP_F_ADD);
		if (nRet != UGW_SUCCESS && nRet != QOS_ENTRY_EXISTS) {
			LOGF_LOG_ERROR("Setting ingress group failed!\n");
			goto exit_lbl;
		}
		if (nRet == QOS_ENTRY_EXISTS) {
			LOGF_LOG_DEBUG("Ingress Group already set!\n");
			nRet = UGW_SUCCESS;
		}
	}

exit_lbl:
	EXIT
	return nRet;
}

static int32_t parse_classifierSection(char *buffer, size_t buf_len, void **cfg, char *entityName, char *ifname, uint32_t *flags) {
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	qos_class_cfg_t *clcfg = (qos_class_cfg_t *)malloc(sizeof(qos_class_cfg_t));
	if (clcfg == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed\n");
		goto exit_lbl;
	}
	char *token, *next_token, *endptr=NULL;
	rsize_t strmax = strnlen_s(buffer, buf_len);
	uint32_t uInstance = 0;
	char sOption[MAX_OPTION_LEN] = {0}, sValue[MAX_OPTION_LEN] = {0};

	char ifname2[MAX_IF_NAME_LEN] = {0};
	char logical_ifname[MAX_IF_NAME_LEN] = {0}, base_ifname[MAX_IF_NAME_LEN] = {0};
	iftype_t ifgroup = QOS_IF_CATEGORY_MAX;

	memset_s(clcfg, sizeof(qos_class_cfg_t), 0);

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		memset_s(sOption, MAX_OPTION_LEN, '\0');
		memset_s(sValue, MAX_OPTION_LEN, '\0');
		uInstance = 0;
		nRet = parse_singleLine(token, &uInstance, sOption, sValue);
		if (strcmp(sOption, "name") == 0) {
			strncpy_s(clcfg->filter.name, MAX_CL_NAME_LEN, sValue, MAX_CL_NAME_LEN);
			if (entityName != NULL)
				strncpy_s(entityName, MAX_CL_NAME_LEN, sValue, MAX_CL_NAME_LEN);
			nRet = validate_string(clcfg->filter.name);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Classifier Name is not valid\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "enable") == 0) {
			clcfg->filter.enable = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier enable param\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "owner") == 0) {
			clcfg->filter.owner = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier owner param\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "order") == 0) {
			clcfg->filter.order = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!(VALIDATE_RANGE(clcfg->filter.order, 1, MAX_RULES)))){
				LOGF_LOG_DEBUG("Invalid classifier order\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "trafficclass") == 0) {
			clcfg->action.tc = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!(VALIDATE_RANGE(clcfg->action.tc, 1, MAX_TC_NUM)))){
				LOGF_LOG_DEBUG("Invalid classifier traffic class\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg->action.flags |= CL_ACTION_TC;
		}
		else if (strcmp(sOption, "prio") == 0) {
			clcfg->action.priority = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(clcfg->action.priority, MIN_Q_PRIO_RANGE, MAX_Q_PRIO_RANGE))){
				LOGF_LOG_DEBUG("Invalid classifier priority\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "target") == 0) {
			strncpy_s(clcfg->action.q_name, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			nRet = validate_string(clcfg->action.q_name);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Queue Name is not valid for the attached classifier\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "clinterface") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid classifier interface name\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.rx_if, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "qInterface") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid queue interface name for classifier\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.tx_if, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_TX_IF, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "mptcp") == 0) {
			clcfg->filter.mptcp_flag = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier mptcp value\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.mptcp_flag == 1)
				clcfg->filter.flags |= CL_ACTION_CL_MPTCP;
		}
		else if ((strcmp(sOption, "flags") == 0) && flags != NULL) {
			clcfg->action.flags = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier flags\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "srcmac") == 0) {
			nRet = isMacValid(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid source mac address\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.src_mac.mac_addr, MAX_MACADDR_LEN, sValue, MAX_MACADDR_LEN);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_SRC_MAC, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_SRC_MAC_ADDRS, &(clcfg->filter.src_mac.param_in_ex));
		}
		else if (strcmp(sOption, "srcmacmask") == 0) {
			nRet = isMacValid(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid source mac address mask\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.src_mac.mac_addr_mask, MAX_MACADDR_LEN, sValue, MAX_MACADDR_LEN);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_SRC_MAC_MASK, &(clcfg->filter.src_mac.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_SRC_MAC_ADDRS, &(clcfg->filter.src_mac.param_in_ex));
		}
		else if (strcmp(sOption, "srcmacex") == 0) {
			clcfg->filter.src_mac.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier src mac exclude option\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.src_mac.exclude == 1)
				clcfg_setParamIncExc(1, CLSCFG_PARAM_SRC_MAC, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "dstmac") == 0) {
			nRet = isMacValid(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid destination mac address\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.dst_mac.mac_addr, MAX_MACADDR_LEN, sValue, MAX_MACADDR_LEN);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_DST_MAC, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_DST_MAC_ADDRS, &(clcfg->filter.dst_mac.param_in_ex));
		}
		else if (strcmp(sOption, "dstmacmask") == 0) {
			nRet = isMacValid(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid destination mac address mask\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.dst_mac.mac_addr_mask, MAX_MACADDR_LEN, sValue, MAX_MACADDR_LEN);
			clcfg_setParamIncExc(0, CLSCFG_PARAM_DST_MAC_ADDRS, &(clcfg->filter.dst_mac.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_DST_MAC_MASK, &(clcfg->filter.dst_mac.param_in_ex));
		}
		else if (strcmp(sOption, "dstmacex") == 0) {
			clcfg->filter.dst_mac.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier destination mac exclude option\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.dst_mac.exclude == 1)
				clcfg_setParamIncExc(1, CLSCFG_PARAM_DST_MAC, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "vlanid") == 0) {
			clcfg->filter.ethhdr.vlanid = (int16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(clcfg->filter.ethhdr.vlanid, 2, 4095))){
				LOGF_LOG_DEBUG("Invalid classifier order\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_VLANID, &(clcfg->filter.ethhdr.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_VLANID, &(clcfg->filter.ethhdr.param_in_ex));
		}
		else if (strcmp(sOption, "vlanidex") == 0) {
			clcfg->filter.ethhdr.vlanid_exclude = (uint16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier vlan id exclude option\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.ethhdr.vlanid_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR_VLANID, &(clcfg->filter.ethhdr.param_in_ex));
			}
		}
		else if (strcmp(sOption, "vlanprio") == 0) {
			clcfg->filter.ethhdr.prio = (int16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (!VALIDATE_RANGE(clcfg->filter.ethhdr.prio, 1, 8))){
				LOGF_LOG_DEBUG("Invalid classifier vlan prio should in range 1-8\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_PRIO, &(clcfg->filter.ethhdr.param_in_ex));
		}
		else if (strcmp(sOption, "vlanprioex") == 0) {
			clcfg->filter.ethhdr.prio_exclude = (uint16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier vlan prio exclude input\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.ethhdr.prio_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR_PRIO, &(clcfg->filter.ethhdr.param_in_ex));
			}
		}
		else if (strcmp(sOption, "srchost") == 0) {
			nRet = isIPAddrValid(sValue);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid source ip address\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.outer_ip.src_ip.ipaddr, IP_ADDRSTRLEN, sValue, IP_ADDRSTRLEN);
			if (strnlen_s(clcfg->filter.outer_ip.src_ip.ipaddr, IP_ADDRSTRLEN) > 0 && (strcmp((clcfg->filter.outer_ip.src_ip.ipaddr), " ") != 0)) {
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "srchostmask") == 0) {
			nRet = isIPAddrValid(sValue);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid ip address mask\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.outer_ip.src_ip.mask, IP_ADDRSTRLEN, sValue, IP_ADDRSTRLEN);
			if (strnlen_s(clcfg->filter.outer_ip.src_ip.mask, IP_ADDRSTRLEN) > 0 && (strcmp((clcfg->filter.outer_ip.src_ip.mask), " ") != 0)) {
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "srchostex") == 0) {
			clcfg->filter.outer_ip.src_ip.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier src host exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.outer_ip.src_ip.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_SRC_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "dsthost") == 0) {
			nRet = isIPAddrValid(sValue);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid dest ip address\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.outer_ip.dst_ip.ipaddr, IP_ADDRSTRLEN, sValue, IP_ADDRSTRLEN);
			if (strnlen_s(clcfg->filter.outer_ip.dst_ip.ipaddr, IP_ADDRSTRLEN) > 0 && (strcmp((clcfg->filter.outer_ip.dst_ip.ipaddr), " ") != 0)) {
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "dsthostmask") == 0) {
			nRet = isIPAddrValid(sValue);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Invalid destination ip address mask\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(clcfg->filter.outer_ip.dst_ip.mask, IP_ADDRSTRLEN, sValue, IP_ADDRSTRLEN);
			if (strnlen_s(clcfg->filter.outer_ip.dst_ip.mask, IP_ADDRSTRLEN) > 0 && (strcmp((clcfg->filter.outer_ip.dst_ip.mask), " ") != 0)) {
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "dsthostex") == 0) {
			clcfg->filter.outer_ip.dst_ip.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier destination exclude option\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.outer_ip.dst_ip.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_DST_IP, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "proto") == 0) {
			clcfg->filter.outer_ip.ip_p = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier proto value\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_PROTO, &(clcfg->filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "protoex") == 0) {
			clcfg->filter.outer_ip.ip_p_exclude = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid classifier proto exclude option\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.outer_ip.ip_p_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_PROTO, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "srcports") == 0) {
			clcfg_setPort(sValue, &(clcfg->filter.outer_ip.src_port.start_port), &(clcfg->filter.outer_ip.src_port.end_port));
			if ((!VALIDATE_RANGE(clcfg->filter.outer_ip.src_port.start_port, 0, 65535)) || (!VALIDATE_RANGE(clcfg->filter.outer_ip.src_port.end_port, 0, 65535))){
				LOGF_LOG_DEBUG("Invalid source port range\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_SRC_PORT, &(clcfg->filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "srcportsex") == 0) {
			clcfg->filter.outer_ip.src_port.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier source port exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.outer_ip.src_port.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_SRC_PORT, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "dstports") == 0) {
			clcfg_setPort(sValue, &(clcfg->filter.outer_ip.dst_port.start_port), &(clcfg->filter.outer_ip.dst_port.end_port));
			if ((!VALIDATE_RANGE(clcfg->filter.outer_ip.dst_port.start_port, 0, 65535)) || (!VALIDATE_RANGE(clcfg->filter.outer_ip.dst_port.end_port, 0, 65535))){
				LOGF_LOG_DEBUG("Invalid destination port range\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP_DST_PORT, &(clcfg->filter.outer_ip.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "dstportsex") == 0) {
			clcfg->filter.outer_ip.dst_port.exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier destination port exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.outer_ip.dst_port.exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP_DST_PORT, &(clcfg->filter.outer_ip.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
			}
		}
		else if (strcmp(sOption, "tcpACKen") == 0) {
			clcfg->filter.tcphdr.tcp_ack = (int8_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier tcp ack enable\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_TCP_HDR_ACK, &(clcfg->filter.tcphdr.param_in_ex));
		}
		else if (strcmp(sOption, "tcpACKenex") == 0) {
			clcfg->filter.tcphdr.tcp_ack_exclude = (int8_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier tcp ack enable exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.tcphdr.tcp_ack_exclude == 1)
				clcfg_setParamIncExc(1, CLSCFG_PARAM_TCP_HDR_ACK, &(clcfg->filter.tcphdr.param_in_ex));
		}
		else if (strcmp(sOption, "l7app") == 0) {
			strncpy_s(clcfg->filter.l7_proto, MAX_L7_PROTO_NAME_LEN, clcfg->filter.rx_if, MAX_L7_PROTO_NAME_LEN); 
			clcfg_setParamIncExc(0, CLSCFG_PARAM_L7_PROTO,  &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "dscp") == 0) {
			clcfg->filter.iphdr.dscp = (int16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier dscp value\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_IP_HDR,  &(clcfg->filter.iphdr.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_IP_HDR_DSCP,  &(clcfg->filter.iphdr.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_OUTER_IP, &(clcfg->filter.param_in_ex));
		}
		else if (strcmp(sOption, "dscpex") == 0) {
			clcfg->filter.iphdr.dscp_exclude = (int16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier dscp value\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.iphdr.dscp_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_IP_HDR,  &(clcfg->filter.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_IP_HDR_DSCP,  &(clcfg->filter.iphdr.param_in_ex));
			}
		}
		else if (strcmp(sOption, "actiondscp") == 0) {
			clcfg->action.dscp = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier dscp value\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->action.dscp != 0)
				clcfg->action.flags |= CL_ACTION_DSCP;
		}
		else if (strcmp(sOption, "actionacceldis") == 0) {
			clcfg->action.accl = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier action accel disable\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->action.accl != 0)
				clcfg->action.flags |= CL_ACTION_ACCELERATION;
		}
		else if (strcmp(sOption, "ethtype") == 0) {
			clcfg->filter.ethhdr.eth_type = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier ethtype\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_TYPE, &(clcfg->filter.ethhdr.param_in_ex));
		}
		else if (strcmp(sOption, "ethtypeex") == 0) {
			clcfg->filter.ethhdr.eth_type_exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier ethtype exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.ethhdr.eth_type_exclude == 1) {
                clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
                clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR_TYPE, &(clcfg->filter.ethhdr.param_in_ex));
			}
		}
		else if (strcmp(sOption, "ethprio") == 0) {
			clcfg->filter.ethhdr.prio = (int16_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier ethprio exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
			clcfg_setParamIncExc(0, CLSCFG_PARAM_ETH_HDR_PRIO, &(clcfg->filter.ethhdr.param_in_ex));
		}
		else if (strcmp(sOption, "ethprioex") == 0) {
			clcfg->filter.ethhdr.prio_exclude = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier ethprio exclude\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->filter.ethhdr.prio_exclude == 1) {
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR, &(clcfg->filter.param_in_ex));
				clcfg_setParamIncExc(1, CLSCFG_PARAM_ETH_HDR_PRIO, &(clcfg->filter.ethhdr.param_in_ex));
			}
		}
		else if (strcmp(sOption, "ethpriomark") == 0) {
			clcfg->action.vlan.pcp = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier ethprio mark\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg->action.flags |= CL_ACTION_VLAN;
		}
		else if (strcmp(sOption, "innerethpriomark") == 0) {
			clcfg->action.vlan.inner_pcp = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_DEBUG("Invalid input classifier inner ethprio mark\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			clcfg->action.flags |= CL_ACTION_VLAN;
		}
		else if (strcmp(sOption, "peakrate") == 0) {
			clcfg->action.policer.pr = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (clcfg->action.policer.pr > MAX_SHAPER_RANGE)){
				LOGF_LOG_DEBUG("Invalid input classifier peakrate\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->action.policer.pr > 0) {
				clcfg->action.policer.enable = 1;
				clcfg->action.flags |= CL_ACTION_POLICER;
			}
		}
		else if (strcmp(sOption, "commitrate") == 0) {
			clcfg->action.policer.cr = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (clcfg->action.policer.cr > MAX_SHAPER_RANGE)){
				LOGF_LOG_DEBUG("Invalid input classifier commit rate\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			if (clcfg->action.policer.cr > 0) {
				clcfg->action.policer.enable = 1;
				clcfg->action.flags |= CL_ACTION_POLICER;
			}
		}
		else if ((strcmp(sOption, "oper") == 0) && flags != NULL) {
			if (strcmp(sValue, "ADD") == 0)
				*flags |= QOS_OP_ADD;
			else if (strcmp(sValue, "MOD") == 0)
				*flags |= QOS_OP_MODIFY;
			else if (strcmp(sValue, "DEL") == 0)
				*flags |= QOS_OP_DELETE;
			else
				*flags |= QOS_OP_NOOPER;
		}
		else if (strcmp(sOption, "ifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Interface name for the classifier is not proper\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(ifname2, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			if (ifname != NULL) {
				strncpy_s(ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			}
		}
		else if (strcmp(sOption, "logicalifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("Logical interface name is not valid classifier\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(logical_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "phyifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_DEBUG("physical interface name is not valid classifier\n");
				nRet = UGW_FAILURE;
				HELP_FREE_BUF(clcfg);
				goto exit_lbl;
			}
			strncpy_s(base_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "iftype") == 0) {
			ifgroup = (iftype_t) strtoul(sValue, NULL, 10);
		}
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	*cfg = (void *)clcfg;

	nRet = qosd_set_base_abs(ifgroup, ifname2, base_ifname, logical_ifname, QOS_OP_F_ADD);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Setting interface group failed!\n");
		goto exit_lbl;
	}

exit_lbl:
	EXIT
	return nRet;
}

static int32_t parse_shaperSection(char *buffer, size_t buf_len, void **cfg, char *entityName, char *ifname, uint32_t *flags) {
	ENTRY
	UNUSED(entityName);
	int32_t nRet = UGW_SUCCESS;
	qos_shaper_t *shCfg = (qos_shaper_t *)malloc(sizeof(qos_shaper_t));
	if (shCfg == NULL) {
		LOGF_LOG_ERROR("Memory allocation failed!\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	char *token, *next_token;
	rsize_t strmax = strnlen_s(buffer, buf_len);
	uint32_t uInstance = 0;
	char sOption[MAX_OPTION_LEN] = {0}, sValue[MAX_OPTION_LEN] = {0};

	char ifname2[MAX_IF_NAME_LEN] = {0}, *endptr = NULL;
	char logical_ifname[MAX_IF_NAME_LEN] = {0}, base_ifname[MAX_IF_NAME_LEN] = {0};
	iftype_t ifgroup = QOS_IF_CATEGORY_MAX;

	memset_s(shCfg, sizeof(qos_shaper_t), 0);

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		memset_s(sOption, MAX_OPTION_LEN, '\0');
		memset_s(sValue, MAX_OPTION_LEN, '\0');
		uInstance = 0;
		nRet = parse_singleLine(token, &uInstance, sOption, sValue);

		if (strcmp(sOption, "shaperen") == 0) {
			shCfg->enable = (int32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid value for shaper enable\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shaperName") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Shaper Name is not valid\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(shCfg->name, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			if (entityName != NULL) {
				strncpy_s(entityName, MAX_Q_NAME_LEN, sValue, MAX_Q_NAME_LEN);
				LOGF_LOG_DEBUG("Shaper Name is : [%s]\n", entityName);
			}
		}
		else if (strcmp(sOption, "shapermode") == 0) {
			shCfg->mode = (qos_shaper_mode_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid value for shaper mode\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shapercir") == 0) {
			shCfg->cir = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (shCfg->cir > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid value for shaper cir rate\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shaperpir") == 0) {
			shCfg->pir = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (shCfg->pir > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid value for shaper pir rate\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shapercbs") == 0) {
			shCfg->cbs = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (shCfg->cbs > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid value for shapercbs\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shaperpbs") == 0) {
			shCfg->pbs = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0 || (shCfg->pbs > MAX_SHAPER_RANGE)){
				LOGF_LOG_ERROR("Invalid value for shaperpbs\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if (strcmp(sOption, "shaperinggrp") == 0) {
			shCfg->inggrp = (qos_inggrp_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid value for shaper ingress group\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		else if ((strcmp(sOption, "shaperflags") == 0) && (flags != NULL)) {
			/* TODO: Make changes in startup script and sl to pass shaper flags in daemon config file. */
			shCfg->flags = (uint32_t)strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid value for shaper flags\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			if ((shCfg->flags & QOS_OP_F_INGRESS) == QOS_OP_F_INGRESS)
				*flags |= QOS_OP_F_INGRESS;
		}
		else if ((strcmp(sOption, "shaperoper") == 0) && flags != NULL) {
			if (strcmp(sValue, "ADD") == 0)
				*flags |= QOS_OP_ADD;
			else if (strcmp(sValue, "MOD") == 0)
				*flags |= QOS_OP_MODIFY;
			else if (strcmp(sValue, "DEL") == 0)
				*flags |= QOS_OP_DELETE;
			else
				*flags |= QOS_OP_NOOPER;
		}
		else if (strcmp(sOption, "ifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Shaper interface not valid\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(ifname2, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
			if (ifname != NULL) {
				strncpy_s(ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
				LOGF_LOG_DEBUG("Shaper Ifname is :[%s]\n", ifname);
			}
		}
		else if (strcmp(sOption, "logicalifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Shaper logical interface not valid\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(logical_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "phyifname") == 0) {
			nRet = validate_string(sValue);
			if(nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Shaper physical interface not valid\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			strncpy_s(base_ifname, MAX_IF_NAME_LEN, sValue, MAX_IF_NAME_LEN);
		}
		else if (strcmp(sOption, "iftype") == 0) {
			ifgroup = (iftype_t) strtoul(sValue, &endptr, 10);
			if(*endptr != 0){
				LOGF_LOG_ERROR("Invalid shaper interface type\n");
				HELP_FREE_BUF(shCfg);
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
		}
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	if (shCfg->enable == QOSAL_DISABLE && flags != NULL) {
		*flags = (*flags & ~QOS_OP_ADD) | (*flags & ~QOS_OP_MODIFY);
		*flags |= QOS_OP_DELETE;
	}

	*cfg = (void *)shCfg;

	nRet = qosd_set_base_abs(ifgroup, ifname2, base_ifname, logical_ifname, QOS_OP_F_ADD);
	if (nRet != UGW_SUCCESS) {
		LOGF_LOG_ERROR("Setting interface group failed!\n");
		goto exit_lbl;
	}

	if (shCfg->flags & QOS_OP_F_INGRESS) {
		nRet = qosd_if_inggrp_set(shCfg->inggrp, ifname2, QOS_OP_F_ADD);
		if (nRet != UGW_SUCCESS && nRet != QOS_ENTRY_EXISTS) {
			LOGF_LOG_ERROR("Setting ingress group failed!\n");
			goto exit_lbl;
		}
		if (nRet == QOS_ENTRY_EXISTS) {
			LOGF_LOG_DEBUG("Ingress Group already set!\n");
			nRet = UGW_SUCCESS;
		}
	}

exit_lbl:
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : set_qos_entity
 *  Description   : Call the appropriate wraper API for the entity to set
 *  Input Params  : Pointer to entity structure, entity type, interface name, old
					old entity name, old interface name, flags
 *  Return value  : UGW_SUCCESS(0) and UGW_FAILURE if buff is empty or Delta section not
                    Present.
 * ============================================================================*/
static int32_t set_qos_entity(IN char *buffer, IN size_t buf_len, IN void **cfg, IN qos_entity_t entity, IN char *ifname, IN char *old_entity, IN char *oldIfName, IN uint32_t flags)
{
	ENTRY
	int32_t nRet = UGW_FAILURE;
    qos_class_cfg_t *cl = NULL;
    qos_queue_cfg_t *q = NULL;
    qos_shaper_t *sh =  NULL;
	qos_cfg_t *qoscfg = NULL;

	if ((*cfg == NULL) && ((flags & QOS_OP_ADD) || (flags & QOS_OP_MODIFY))) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("New configuration is not set!\n");
		goto exit_lbl;
	}
	switch (entity) {
		case QOS_ENTITY_QOSCFG:
			qoscfg = (qos_cfg_t *)(*cfg);
			nRet = qos_basic_set_wrap(buffer, buf_len, qoscfg, flags);
			break;

		case QOS_ENTITY_QUEUE:
			q = (qos_queue_cfg_t *)(*cfg);
			nRet = qos_queue_set_wrap(q, ifname, old_entity, oldIfName, flags);
			break;

		case QOS_ENTITY_CLASSIFIER:
			cl = (qos_class_cfg_t *)(*cfg);
			nRet = qos_classifier_set_wrap(cl, ifname, old_entity, oldIfName, flags);
			break;

		case QOS_ENTITY_PORT_SHAPER:
			sh = (qos_shaper_t *)(*cfg);
			nRet = qos_shaper_set_wrap(sh, ifname, old_entity, oldIfName, flags);
			break;

		default:
			LOGF_LOG_ERROR("Entity not supported~\n");
			nRet = UGW_FAILURE;
			break;
	}

exit_lbl:
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : qos_queue_set_wrap
 *  Description   : Wraper API to set the queue
 *  Input Params  : class_cfg -   		Queue Structure
					ifname -			Interface name
					old_class_name -	Old Queue Name
					oldIfName -			Old Interface Name
					flags -				Flags (CL_FILTER_ADD/..DEL/..MODIFY)
 * ============================================================================*/
static int32_t qos_queue_set_wrap(IN qos_queue_cfg_t *q, IN char *ifname, IN char *old_q_name, IN char *oldIfname, uint32_t flags)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	qos_queue_cfg_t *old_q = NULL;
	qos_queue_node_t *q_list_node = NULL;

	/* TODO: What about NOOPER ? */
	if (flags & QOS_OP_F_ADD) {
		qosd_set_qos_config(q, QOS_ENTITY_QUEUE, ifname, flags);
	}
	else if (flags & QOS_OP_F_DELETE) {
		/* Since from delta config we get only queue name, so fetch the qos_queue_cfg structure from the ES table based on the interface and queue name */
		nRet = get_queue_cfg_node(ifname, old_q_name, &q_list_node, &flags);
		if (nRet != UGW_SUCCESS) {
			LOGF_LOG_ERROR("Queue node not found\n");
			goto exit_lbl;
		}

		old_q = (qos_queue_cfg_t*)malloc(sizeof(qos_queue_cfg_t));
		if (old_q == NULL) {
			LOGF_LOG_ERROR("Queue memory allocation failed\n");
			goto exit_lbl;
		}

		memcpy_s(old_q, sizeof(qos_queue_cfg_t), &(q_list_node->queue_cfg), sizeof(qos_queue_cfg_t));
		qosd_set_qos_config(old_q, QOS_ENTITY_QUEUE, ifname, flags);
	}
	else if (flags & QOS_OP_F_MODIFY) {
		/* fetch the old qos_queue_cfg structure based on the old queue name */
		if (strcmp(ifname, oldIfname) || strcmp(old_q_name, q->name)) {
			nRet = get_queue_cfg_node(oldIfname, old_q_name, &q_list_node, &flags);
			if (nRet == UGW_SUCCESS) {
				old_q = (qos_queue_cfg_t*)malloc(sizeof(qos_queue_cfg_t));
				if (old_q == NULL) {
					LOGF_LOG_ERROR("Queue memory allocation failed\n");
					goto exit_lbl;
				}
				memcpy_s(old_q, sizeof(qos_queue_cfg_t), &(q_list_node->queue_cfg), sizeof(qos_queue_cfg_t));
				/* While parsing the config set the ABS and BASE for old Interface and the new interface */
				/* Delete that queue */
				/* reset the modify flag */
				flags &= ~QOS_OP_F_MODIFY;
				flags |= QOS_OP_F_DELETE;
				qosd_set_qos_config(old_q, QOS_ENTITY_QUEUE, oldIfname, flags);
			} else {
				LOGF_LOG_ERROR("Queue node not found!\n");
			}

			/* Add new queue */
			flags &= ~QOS_OP_F_DELETE;
			flags |= QOS_OP_F_ADD;
			qosd_set_qos_config(q, QOS_ENTITY_QUEUE, ifname, flags);
		}
		else {
			qosd_set_qos_config(q, QOS_ENTITY_QUEUE, ifname, flags);
		}
	}

exit_lbl:
	if (old_q != NULL) {
		free(old_q);
		old_q = NULL;
	}
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : qosd_set_qos_config
 *  Description   : Based on the entity will parse the control to fapi_low
 *  Input Params  : cfg -   			Entity Structure
                    entity -            qos config entity (queue/classifier/shaper)
                    ifname -            Interface Name
					flags -				Flags Used to configure the entity
 *  Return value  : UGW_SUCCESS(0) and UGW_FAIL if it fails
 * ============================================================================*/
int32_t qosd_set_qos_config(IN void *cfg, IN qos_entity_t entity, IN char *ifname, IN uint32_t flags)
{
	ENTRY
    qos_class_cfg_t *cl = NULL;
    qos_queue_cfg_t *q = NULL;
    qos_shaper_t *sh = NULL;
	int32_t nRet = UGW_SUCCESS;
    uint32_t weight, priority, order;

    switch (entity) {
        case QOS_ENTITY_QUEUE:
			q = (qos_queue_cfg_t *)cfg;
            nRet = qosd_queue_set(ifname, q, flags);
			if (nRet != UGW_SUCCESS)
				LOGF_LOG_ERROR("Queue Configuration Failed!");
            break;

        case QOS_ENTITY_CLASSIFIER:
			cl = (qos_class_cfg_t *)cfg;
			order = cl->filter.order;
            nRet = qosd_classifier_set(order, cl, flags);
			if (nRet != UGW_SUCCESS)
				LOGF_LOG_ERROR("Classifier Configuration Failed!");
            break;

        case QOS_ENTITY_PORT_SHAPER:
			sh = (qos_shaper_t *)cfg;
            weight = priority = 0;
			/* before setting the port do port update that will add a node to the interface   (to be done by parsing) */
            nRet = qosd_port_config_set(ifname, sh, weight, priority, flags);
			if (nRet != UGW_SUCCESS)
				LOGF_LOG_ERROR("Shaper Configuration Failed!");
            break;
/*
        case PORT_CONFIG_UPDATE:
        {
            nRet = qosd_port_update(ifname, flags);
			if (nRet != UGW_SUCCESS)
				LOGF_LOG_ERROR("Port update Configuration Failed!");
            break;
        }*/
		default:
			LOGF_LOG_ERROR("Entity not supported!\n");
			nRet = UGW_FAILURE;
    }
	return nRet;
}

/* =============================================================================
 *  Function Name : qosd_set_base_abs
 *  Description   : Set the ABS and Base
 *  Input Params  : iftype -   			Interface type
					ifname -			Interface name
					base_if -			Base Interface
					logical_base -		Logical interface
					flags -				Flags used to set the abs & base
 * ============================================================================*/

static int32_t qosd_set_base_abs(IN int32_t iftype, IN char *ifname, IN char *base_if, IN char *logical_base, IN uint32_t flags)
{
	int32_t nRet = UGW_SUCCESS;
	LOGF_LOG_DEBUG("Interface Name:[%s] Category::[%d] Base Interface:[%s] Logical Interface:[%s] flags:[%u]\n", ifname, iftype, base_if, logical_base, flags);
	nRet = qosd_if_abs_set(iftype, ifname, flags);
	if (nRet == UGW_SUCCESS) {
		if ((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN) || (iftype == QOS_IF_CATEGORY_ETHLAN)) {
            nRet = qosd_if_base_set(ifname, base_if, logical_base, flags);
		}
	}
	return nRet;
}

/* =============================================================================
 *  Function Name : get_queue_cfg_node
 *  Description   : Get the queue structure node from queue list
 *	Input Params  :	ifname -			Interface name
					q_name -			Queue name
 *	Output Params : q_list_node -		Queue node
 * ============================================================================*/
static int32_t get_queue_cfg_node(char *ifname, char *q_name, qos_queue_node_t **q_list_node, uint32_t *flags)
{
	ENTRY
	qos_if_node_t *if_list_node = NULL;
	struct list_head *if_list_head = NULL;
	int32_t nRet = UGW_FAILURE;

	if (ifname == NULL) {
		LOGF_LOG_ERROR("Interface is not valid!\n");
		nRet = UGW_FAILURE;
		goto exit;
	}
	LOGF_LOG_DEBUG("Check queues on Interface : [%s]\n", ifname);

/* Check if node is present for ingress interface list node */
	if_list_head = es_tables.ingress_if_list_head;
	if (if_list_head != NULL)
		if_list_node = qosd_get_if_node(if_list_head, ifname);
	if (if_list_node != NULL) {
		LOGF_LOG_DEBUG ("Check ingress queue [%s] configured on interface %s\n", q_name, ifname);
		*q_list_node = qosd_get_q_node(if_list_node->q_list_head, q_name);
		if (*q_list_node == NULL) {
			LOGF_LOG_ERROR("Given queue with name [%s] not configured on ingress interface %s\n", q_name, ifname);
		}
		else {
			*flags |= QOS_OP_F_INGRESS;
			LOGF_LOG_ERROR("Given queue with name [%s] configured on ingress interface %s\n", q_name, ifname);
			nRet = UGW_SUCCESS;
			goto exit;
		}
	}
/* If not then check in Egress interface list node */
	if (*q_list_node == NULL) {
		if_list_head = es_tables.egress_if_list_head;
		if (if_list_head != NULL)
			if_list_node = qosd_get_if_node(if_list_head, ifname);
		if(if_list_node != NULL) {
			LOGF_LOG_DEBUG ("Check egress queue [%s] configured on interface %s\n", q_name, ifname);
			*q_list_node = qosd_get_q_node(if_list_node->q_list_head, q_name);

			if (*q_list_node == NULL) {
				LOGF_LOG_ERROR("Given queue with name [%s] not configured on egress interface %s\n", q_name, ifname);
			}
			else {
				LOGF_LOG_ERROR("Given queue with name [%s] configured on egress interface %s\n", q_name, ifname);
				nRet = UGW_SUCCESS;
				goto exit;
			}
		}
	}

exit:
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : get_shaper_cfg_node
 *  Description   : Get the queue structure node from queue list
 *	Input Params  :	ifname -			Interface name
					q_name -			Queue name
 *	Output Params : q_list_node -		Queue node
 * ============================================================================*/
static int32_t get_shaper_cfg_node(IN char *ifname, IN char *shaperName, OUT qos_if_node_t **if_list_node, INOUT uint32_t *flags)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	struct list_head *if_list_head = NULL;
	qos_if_node_t *intf_node = NULL;

	if (shaperName != NULL) {
		LOGF_LOG_DEBUG("Get ifnode for shaper name:[%s] on :[%s]\n", shaperName, ifname);
	}
	else {
		LOGF_LOG_ERROR("Invalid shaper name!\n");
		nRet = UGW_FAILURE;
		goto exit_lbl;
	}

	/* Check if node is present for ingress interface list node */
	if_list_head = es_tables.ingress_if_list_head;
	if (if_list_head != NULL)
		intf_node = qosd_get_if_node(if_list_head, ifname);
	if (intf_node != NULL) {
		LOGF_LOG_DEBUG("Checking ingress interface list\n");
		if ( intf_node->port_shaper != NULL) {
			LOGF_LOG_DEBUG("Port shaper name:[%s]\n", intf_node->port_shaper->shaper.name);
			if (strcmp(intf_node->port_shaper->shaper.name, shaperName) == 0) {
				LOGF_LOG_DEBUG("Shaper node matched\n");
				*flags |= QOS_OP_F_INGRESS;
				goto exit_lbl;
			}
		}
	}
	/* If not then check in Egress interface list node */
	if_list_head = es_tables.egress_if_list_head;
	if (if_list_head != NULL)
		intf_node = qosd_get_if_node(if_list_head, ifname);
	if (intf_node != NULL) {
		LOGF_LOG_DEBUG("Checking egress interface list\n");
		if ( intf_node->port_shaper != NULL) {
			LOGF_LOG_DEBUG("Port shaper name:[%s]\n", intf_node->port_shaper->shaper.name);
			if (strcmp(intf_node->port_shaper->shaper.name, shaperName) == 0) {
				LOGF_LOG_DEBUG("Shaper node matched\n");
				goto exit_lbl;
			}
		}
	}

exit_lbl:
	if (intf_node == NULL || intf_node->port_shaper == NULL){
		LOGF_LOG_ERROR("No port shaper are configured on Interface %s \n",ifname);
		nRet = UGW_FAILURE;
	}
	else
		*if_list_node = intf_node;

	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : qos_shaper_set_wrap
 *  Description   : Wraper API to set the shaper
 *  Input Params  : class_cfg -   		Shaper Structure
					ifname -			Interface name
					oldIfName -			Old Interface Name
					flags -				Flags (CL_FILTER_ADD/..DEL/..MODIFY)
 * ============================================================================*/
static int32_t qos_shaper_set_wrap(IN qos_shaper_t *sh, IN char *ifname, IN char *old_sh_name, IN char *oldIfname, IN uint32_t flags)
{
	uint32_t nRet = UGW_SUCCESS;
	qos_shaper_t *old_sh = NULL;
	qos_if_node_t *if_list_node = NULL;

	/* WHAT ABOUT NOOPER? */
	if (flags & QOS_OP_F_ADD) {
		if (sh != NULL)
			nRet = qosd_set_qos_config(sh, QOS_ENTITY_PORT_SHAPER, ifname, flags);
	}
	else if (flags & QOS_OP_F_DELETE) {
		/* Get the shaper node from the ES tables in order to fetch the shaper structure */
		nRet = get_shaper_cfg_node(oldIfname, old_sh_name, &if_list_node, &flags);
		if( nRet != UGW_SUCCESS || if_list_node->port_shaper == NULL) {
			LOGF_LOG_ERROR("Shaper not found\n");
			return UGW_FAILURE;
		}

		sh = (qos_shaper_t *)malloc(sizeof(qos_shaper_t));
		if(sh == NULL) {
			LOGF_LOG_ERROR("Failed to allocate memory\n");
			return UGW_FAILURE;
		}

		memcpy_s(sh, sizeof(qos_shaper_t), &(if_list_node->port_shaper->shaper), sizeof(qos_shaper_t));
		nRet = qosd_set_qos_config(sh, QOS_ENTITY_PORT_SHAPER, ifname, flags);
	}
	else if (flags & QOS_OP_F_MODIFY) {
		if (strcmp(ifname, oldIfname)) {
			/* This condition if the interface is changed for the shaper then delete the old shaper and configure the new shaper */
			nRet = get_shaper_cfg_node(oldIfname, old_sh_name, &if_list_node, &flags);
			if (nRet != UGW_SUCCESS || if_list_node->port_shaper == NULL) {
				LOGF_LOG_ERROR("Shaper not found\n");
				return UGW_FAILURE;
			}
			old_sh = (qos_shaper_t *)malloc(sizeof(qos_shaper_t));
			if (old_sh == NULL) {
				LOGF_LOG_ERROR("Failed to allocate memory\n");
				return UGW_FAILURE;
			}
			memcpy_s(old_sh, sizeof(qos_shaper_t), &(if_list_node->port_shaper->shaper), sizeof(qos_shaper_t));
			/* Delete the old shaper that is configured on old interface*/
			flags &= ~QOS_OP_F_MODIFY;
			flags |= QOS_OP_F_DELETE;
			nRet = qosd_set_qos_config(old_sh, QOS_ENTITY_PORT_SHAPER, oldIfname, flags);

			/* ADD the new shaper that is configured on new interface*/
			flags &= ~QOS_OP_F_DELETE;
			flags |= QOS_OP_F_ADD;
			nRet = qosd_set_qos_config(sh, QOS_ENTITY_PORT_SHAPER, ifname, flags);
		}
		else {
			nRet = get_shaper_cfg_node(oldIfname, old_sh_name, &if_list_node, &flags);
			if (nRet != UGW_SUCCESS || if_list_node->port_shaper == NULL) {
				LOGF_LOG_ERROR("Shaper not found\n");
				flags &= ~QOS_OP_F_MODIFY;
				flags |= QOS_OP_F_ADD;
				goto setShaper;
			}
			old_sh = (qos_shaper_t *)malloc(sizeof(qos_shaper_t));
			if (old_sh == NULL) {
				LOGF_LOG_ERROR("Failed to allocate memory\n");
				return UGW_FAILURE;
			}
			memcpy_s(old_sh, sizeof(qos_shaper_t), &(if_list_node->port_shaper->shaper), sizeof(qos_shaper_t));
			if (old_sh->enable != sh->enable) {
				if (old_sh->enable == 1 && sh->enable == 0) {
					/* Delete the old shaper that is configured on interface*/
					flags &= ~QOS_OP_F_MODIFY;
					flags |= QOS_OP_F_DELETE;
				} else if (old_sh->enable == 0 && sh->enable == 1) {
					/* ADD the new shaper that is configured on interface*/
					flags &= ~QOS_OP_F_MODIFY;
					flags |= QOS_OP_F_ADD;
				}
			}
setShaper:
			nRet = qosd_set_qos_config(sh, QOS_ENTITY_PORT_SHAPER, ifname, flags);
		}
	}

	EXIT
	if (old_sh != NULL) {
		free(old_sh);
	}

	return nRet;
}

/* =============================================================================
 *  Function Name : qos_classifier_set_wrap
 *  Description   : Wraper API to set the classifier
 *  Input Params  : class_cfg -   		Classifier Structure
					ifname -			Interface name
					old_class_name -	Old Classifier Name
					oldIfName -			Old Interface Name
					flags -				Flags (CL_FILTER_ADD/..DEL/..MODIFY)
 * ============================================================================*/
static int32_t qos_classifier_set_wrap(IN qos_class_cfg_t *class_cfg, IN char *ifname, IN char *old_class_name, IN char *oldIfname, IN uint32_t flags)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	qos_class_cfg_t *old_class_cfg = NULL;
	qos_class_node_t *clNode = NULL;

	if (flags & CL_FILTER_ADD) {
		nRet = qosd_set_qos_config(class_cfg, QOS_ENTITY_CLASSIFIER, ifname, flags);
	}
	else if (flags & CL_FILTER_DELETE) {
		clNode = qosd_get_class_node(es_tables.class_list_head, old_class_name, ifname);
		if (clNode == NULL) {
			LOGF_LOG_ERROR("Given classifier with name [%s] not configured on specified interface %s\n", old_class_name, ifname);
			nRet = QOS_CLASSIFIER_NOT_FOUND;
			goto exit_lbl;
		}

		class_cfg = (qos_class_cfg_t *)malloc(sizeof(qos_class_cfg_t));
		if (class_cfg == NULL) {
			LOGF_LOG_ERROR("Memory allocation failed\n");
			nRet = UGW_FAILURE;
			goto exit_lbl;
		}
		memcpy_s(class_cfg, sizeof(qos_class_cfg_t), &(clNode->class_cfg), sizeof(qos_class_cfg_t));
		nRet = qosd_set_qos_config(class_cfg, QOS_ENTITY_CLASSIFIER, ifname, flags);
	}
	else if (flags & CL_FILTER_MODIFY) {
		/* fetch the old qos_queue_cfg structure based on the old queue name */
		if (strcmp(oldIfname, ifname) || strcmp(old_class_name, class_cfg->filter.name)) {
			clNode = qosd_get_class_node(es_tables.class_list_head, old_class_name, oldIfname);
			if (clNode == NULL) {
				LOGF_LOG_ERROR("Given classifier with name [%s] not configured on specified interface %s\n", old_class_name, ifname);
				nRet = QOS_CLASSIFIER_NOT_FOUND;
				goto exit_lbl;
			}

			/* Delete that classifier and reset the modify flag */
			flags &= ~CL_FILTER_MODIFY;
			flags |= CL_FILTER_DELETE;
			old_class_cfg = (qos_class_cfg_t *)malloc(sizeof(qos_class_cfg_t));
			if (old_class_cfg == NULL){
				LOGF_LOG_ERROR("Memory Allocation Failed\n");
				nRet = UGW_FAILURE;
				goto exit_lbl;
			}
			memcpy_s(old_class_cfg, sizeof(qos_class_cfg_t), &(clNode->class_cfg), sizeof(qos_class_cfg_t));
			qosd_set_qos_config(old_class_cfg, QOS_ENTITY_CLASSIFIER, oldIfname, flags);

			/* Add new classifier */
			flags &= ~CL_FILTER_DELETE;
			flags |= CL_FILTER_ADD;
			qosd_set_qos_config(class_cfg, QOS_ENTITY_CLASSIFIER, ifname, flags);
		}
		else {
			qosd_set_qos_config(class_cfg, QOS_ENTITY_CLASSIFIER, ifname, flags);
		}
	}

exit_lbl:
	if (old_class_cfg != NULL)
		free(old_class_cfg);
	EXIT
	return nRet;
}

/* ===================================================================================
 *  Function Name : clcfg_setParamIncExc
 *  Description   : API to set corresponding bit in bitmap including all parameters
 *  			indicating specified parameter has proper value.
 * ===================================================================================*/
static void clcfg_setParamIncExc(int32_t exclude_req, int32_t param , cl_param_in_ex_t *pxparaminex)
{
	pxparaminex->param_in |= param;

	if(exclude_req)
		pxparaminex->param_ex |= param ;

	return;
}

static void clcfg_setPort(char *sPort, int32_t *start_port, int32_t *end_port) {
	if (sPort == NULL)
		return;

	char *token, *next_token;
	rsize_t strmax = strnlen_s(sPort, MAX_OPTION_LEN);

	token = strtok_s(sPort, &strmax, ",", &next_token);
	if (token != NULL) {
		*start_port = (int32_t)strtoul(token, NULL, 10);
		*end_port = (int32_t)strtoul(next_token, NULL, 10);
	}
	return;
}


static int32_t qos_basic_set_wrap(char *buffer, size_t buf_len, qos_cfg_t *qoscfg, uint32_t flags)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	LOGF_LOG_DEBUG("qoscfg is set to:[%d] and flags:[%u]\n", qoscfg->ena, flags);
	UNUSED(buffer);
	UNUSED(buf_len);

	char *token, *next_token;
	rsize_t strmax = buf_len;

	qos_entity_t entityType = QOS_ENTITY_QOSCFG;
	bool match = false;
	uint32_t oper_flags = 0;
	char ifname[MAX_IF_NAME_LEN] = {0}, entityName[MAX_OPTION_LEN] = {0};
	void *cfg = NULL;

	char *entity_buffer = (char *)malloc(sizeof(char) * buf_len);
	if (entity_buffer == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed to buffer\n");
		goto exit_lbl;
	}
	memset_s(entity_buffer, buf_len, '\0');

	/* Enable QoS before starting to set queues/classifiers/shaper */
	if (qoscfg->ena == QOSAL_ENABLE) {
		nRet = qosd_cfg_set(qoscfg);
	}

	token = strtok_s(buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		for (entityType = QOS_ENTITY_QOSCFG; entityType < QOS_ENTITY_MAX; entityType++) {
			if (entityType == QOS_ENTITY_QOSCFG || entityType == QOS_ENTITY_DELTACFG) {
				match = false;
				continue;
			}
			if ((strcmp(token, entityHeaders[entityType]) == 0) && (match == false)) {
				match = true;
				break;
			}
		}
		
		if (match == true) {
			LOGF_LOG_DEBUG("Token:[%s] EntityType:[%u]\n", token, entityType);

			memset_s(entity_buffer, buf_len, '\0');
			memset_s(ifname, MAX_IF_NAME_LEN, '\0');
			memset_s(entityName, MAX_OPTION_LEN, '\0');
			oper_flags = 0;

			token = strtok_s(NULL, &strmax, "\n", &next_token);
			while (token != NULL) {
				if (token[0] == '[')
					break;
				strncat_s(entity_buffer, buf_len, token, strnlen_s(token, buf_len));
				strncat_s(entity_buffer, buf_len, "\n", 2);
				token = strtok_s(NULL, &strmax, "\n", &next_token);
			}
			/* Entity Buffer is ready to be parsed */
			nRet = parse_Section(entityType, entity_buffer, buf_len, &cfg, entityName, ifname, &oper_flags);
			if (nRet != UGW_SUCCESS) {
				LOGF_LOG_ERROR("Failed to parse the given entity!\n");
				goto exit_lbl;
			}
			/* Set the qos_entity then */
			if (flags & QOS_OP_F_ADD) {
				/* INIT case */
				oper_flags = (oper_flags & ~QOS_OP_F_MODIFY) | (oper_flags & ~QOS_OP_F_DELETE);
				oper_flags |= QOS_OP_F_ADD;
			} else if (flags & QOS_OP_F_DELETE) {
				/* UNINIT case */
				oper_flags &= ~(QOS_OP_F_ADD | QOS_OP_F_MODIFY | QOS_OP_NOOPER);
				oper_flags |= QOS_OP_F_DELETE;
			}
			nRet = set_qos_entity(buffer, buf_len, &cfg, entityType, ifname, entityName, ifname, oper_flags);
			LOGF_LOG_DEBUG("Now check for another entity in buffer\n");
			match = false;
			continue;
		}
		
		match = false;
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	/* Disable QoS only after all entities are disabled/deleted */
	if (qoscfg->ena == QOSAL_DISABLE) {
		nRet = qosd_cfg_set(qoscfg);
	}

exit_lbl:
	if (cfg != NULL) {
		free(cfg);
		cfg = NULL;
	}
	if (entity_buffer != NULL) {
		free(entity_buffer);
		entity_buffer = NULL;
	}
	EXIT
	return UGW_SUCCESS;
}

static int32_t parse_Section(IN qos_entity_t entityType, OUT char *entity_buffer, OUT size_t buf_len, OUT void **cfg, OUT char *entityName, OUT char *ifname, OUT uint32_t *flags)
{
	ENTRY
	int32_t nRet = UGW_FAILURE;

	switch(entityType) {
		case QOS_ENTITY_QOSCFG:
			nRet = parse_qosSection(entity_buffer, buf_len, cfg, flags);
			break;

		case QOS_ENTITY_QUEUE:
			nRet = parse_queueSection(entity_buffer, buf_len, cfg, entityName, ifname, flags);
			LOGF_LOG_DEBUG("Queue Name is : [%s]\n", entityName);
			break;

		case QOS_ENTITY_CLASSIFIER:
			nRet = parse_classifierSection(entity_buffer, buf_len, cfg, entityName, ifname, flags);
			break;

		case QOS_ENTITY_PORT_SHAPER:
			nRet = parse_shaperSection(entity_buffer, buf_len, cfg, entityName, ifname, flags);
			break;

		default:
			nRet = UGW_FAILURE;
			break;
	}

	EXIT
	return nRet;
}

static int32_t set_all_entities_without_delta(char **buffer, size_t buf_len)
{
	ENTRY
	int32_t nRet = UGW_SUCCESS;
	char *token, *next_token, *qoscfg_buffer = NULL, *tmp_buffer = NULL;
	rsize_t strmax = buf_len;
	bool match = false;
	qos_cfg_t *cfg = NULL;
	uint32_t entityFlags = 0;
	uint32_t qoscfg_buf_len = 0;

	if (*buffer == NULL || buf_len == 0) {
		LOGF_LOG_ERROR("Buffer is empty!\n");
		goto exit_lbl;
	}

	tmp_buffer = (char *)malloc(sizeof(char) * (buf_len + 1));
	if (tmp_buffer == NULL) {
		nRet = UGW_FAILURE;
		LOGF_LOG_ERROR("Memory allocation failed!\n");
		goto exit_lbl;
	}
	memset_s(tmp_buffer, buf_len + 1, '\0');
	strncpy_s(tmp_buffer, buf_len + 1, *buffer, buf_len);

	token = strtok_s(tmp_buffer, &strmax, "\n", &next_token);
	while (token != NULL) {
		if (strcmp(token , entityHeaders[QOS_ENTITY_QOSCFG]) == 0 && match == false) {
			match = true;
			qoscfg_buffer = (char *)malloc(sizeof(char) * buf_len);
			if (qoscfg_buffer == NULL) {
				nRet = UGW_FAILURE;
				LOGF_LOG_ERROR("Memory allocation failed\n");
				goto exit_lbl;
			}
			memset_s(qoscfg_buffer, buf_len, '\0');
			token = strtok_s(NULL, &strmax, "\n", &next_token);
			continue;
		}
		if (match == true) {
			if (token[0] == '[') //Denotes start of another section
				break;

			strncat_s(qoscfg_buffer, buf_len, token, strnlen_s(token, buf_len));
			strncat_s(qoscfg_buffer, buf_len, "\n", 2);
		}
		token = strtok_s(NULL, &strmax, "\n", &next_token);
	}

	if (match == true) {
		qoscfg_buf_len = strnlen_s(qoscfg_buffer, buf_len);
		if (qoscfg_buf_len == 0) {
			LOGF_LOG_ERROR("Failed to get the buffer length!\n");
			nRet = UGW_FAILURE;
			goto exit_lbl;
		}
		nRet = parse_Section(QOS_ENTITY_QOSCFG, qoscfg_buffer, qoscfg_buf_len, (void **)&cfg, NULL, NULL, &entityFlags);
		if (nRet != UGW_SUCCESS) {
			LOGF_LOG_ERROR("Error in parsing qoscfg section\n");
			goto exit_lbl;
		}

		if (cfg->ena == QOSAL_ENABLE)
			entityFlags = QOS_OP_F_ADD;
		else if (cfg->ena == QOSAL_DISABLE)
			entityFlags = QOS_OP_F_DELETE;

		memset_s(tmp_buffer, buf_len + 1, '\0');
		strncpy_s(tmp_buffer, buf_len + 1, *buffer, buf_len);
		nRet = set_qos_entity(tmp_buffer, buf_len, (void **)&cfg, QOS_ENTITY_QOSCFG, NULL, NULL, NULL, entityFlags);
	} else {
		LOGF_LOG_ERROR("QoS section not found in config file. No changes made!\n");
		nRet = UGW_SUCCESS;
	}

exit_lbl:
	if (cfg != NULL) {
		free(cfg);
		cfg = NULL;
	}
	if (qoscfg_buffer != NULL) {
		free(qoscfg_buffer);
		qoscfg_buffer = NULL;
	}
	EXIT
	return nRet;
}

static int32_t isIPAddrValid(char *sIP)
{
    struct addrinfo xAddr, *pxRes = NULL;
    int32_t nRet = UGW_SUCCESS;

    memset_s(&xAddr, sizeof(xAddr), 0);

    xAddr.ai_family = PF_UNSPEC;
    xAddr.ai_flags = AI_NUMERICHOST;

    nRet = getaddrinfo(sIP, NULL, &xAddr, &pxRes);

    freeaddrinfo(pxRes);
    return nRet;
}

static int32_t validate_string(char *str)
{
    size_t len;
    uint32_t i;
    len = strnlen_s(str, MAX_DATA_LEN);
    if (len == 0 || len > MAX_QUEUE_NAME_LEN)
        return UGW_FAILURE;
    for (i = 0; i< len; i++){
        if(!( (str[i] >= 'A' && str[i] <= 'Z') || (str[i] >= 'a' && str[i] <= 'z') || (str[i] >= '0' && str[i] <= '9') || (str[i] == '_') || (str[i] == '.') || (str[i] == '-') ))
            return UGW_FAILURE;
    }
    return UGW_SUCCESS;
}

static int32_t isMacValid(char *mac_addr)
{
    unsigned char  mac_idx= 0;

    while(mac_addr[mac_idx] != 0 && mac_idx < MAC_STR_LEN){
        if(mac_idx != 15 && mac_addr[mac_idx + 2] != ':')
            return UGW_FAILURE;
        if(!isxdigit(mac_addr[mac_idx]))
            return UGW_FAILURE;
        if(!isxdigit(mac_addr[mac_idx+1]))
            return UGW_FAILURE;
        mac_idx += 3;
    }

    if(mac_idx == MAC_STR_LEN)
        return UGW_SUCCESS;
 return UGW_FAILURE;
}
