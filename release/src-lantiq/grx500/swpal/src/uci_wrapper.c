
/*##################################################################################################
# "Copyright (c) 2019 Intel Corporation                                                            #
# DISTRIBUTABLE AS SAMPLE SOURCE SOFTWARE                                                          #
# This Distributable As Sample Source Software is subject to the terms and conditions              #
# specified in the LICENSE file                                                                    #
##################################################################################################*/

/*! \file 	uci_wrapper.c
	\brief 	This file implements a wrapper over uci library.
	\todo 	Add license header
	\todo   Add null pointer verifications and clean-up
 */
#include <sys/param.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <paths.h>
#include <uci.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubus.h>
#include "uci_wrapper.h"
#include "help_logging.h"
#define NETIFD_RESP_TIMEOUT 10000


/* list of banned SDL methods */
#define strlen(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strtok(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcat(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncat(...) 	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strcpy(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN
#define strncpy(...) 	SDL_BANNED_FUNCTION ERROR_TOKEN
#define snprintf(...) 	SDL_BANNED_FUNCTION ERROR_TOKEN
#define sscanf(...)  	SDL_BANNED_FUNCTION ERROR_TOKEN

//Logging
#ifndef LOG_LEVEL
unsigned int LOGLEVEL = SYS_LOG_DEBUG + 1;
#else
unsigned int LOGLEVEL = LOG_LEVEL + 1;
#endif

#ifndef LOG_TYPE
unsigned int LOGTYPE = SYS_LOG_TYPE_CONSOLE | SYS_LOG_TYPE_FILE;
#else
unsigned int LOGTYPE = LOG_TYPE;
#endif

unsigned int LOGPROFILE;
//End Logging

static bool uci_converter_if_ap(struct uci_section *s);

enum ifType
{
	IFTYPE_AP = 0,
	IFTYPE_STA,
	IFTYPE_BOTH
};

//UCI HELPER APIS

/*! \file uci_wrapper.c
    \brief SWPAL library
    
	The UCI (Unified Configuration Interface) subsystem that we take from openwrt is intended to centralize 
	the configuration of the wireless configuration.
	The SWPAL scripts (mac80211.sh, and few other scripts taken from openwrt) shall be in charge of taking the parameters 
	from the UCI DB and apply them for configuring hostapd.
	The SWPAL can be used in many platforms. For example:
	UGW - where the WLAN SL will do the translation from TR181 to UCI and then call SWPAL (scripts/uci_wrapper.c) 
	RDKB - where the SWPAL will be called from the WIFI_HAL (wlan_config_server_api.c).
*/

static __thread struct uci_context *__ctx = NULL;
static __thread unsigned __ctx_ref_cnt = 0;

#define THEAD_LOCAL_CTX_ALLOC_LIMIT (24)

static struct uci_context* _thread_local_uci_get_context(void)
{
alloc:
	if (!__ctx)
		__ctx = uci_alloc_context();

	if (!__ctx)
		return NULL;

	if (++__ctx_ref_cnt >= THEAD_LOCAL_CTX_ALLOC_LIMIT) {
		ERROR("__ctx_ref_cnt reached %d ==> ctx mem leak detected\n", __ctx_ref_cnt);
		uci_free_context(__ctx);
		__ctx_ref_cnt = 0;
		__ctx = NULL;
		goto alloc;
	}

	return __ctx;
}

static void _thread_local_uci_put_context(struct uci_context* ctx)
{
	if (!__ctx_ref_cnt) {
		ERROR("__ctx_ref_cnt is equal to 0, __ctx=%p\n", __ctx);
		return;
	}

	if (ctx != __ctx) {
		ERROR("ctx (%p) != __ctx (%p)\n", ctx, __ctx);
		return;
	}

	__ctx_ref_cnt--;
	if (!__ctx_ref_cnt) {
		uci_free_context(__ctx);
		__ctx = NULL;
	}
}

int uci_converter_alloc_local_uci_context(void)
{
	if (!_thread_local_uci_get_context())
		return RETURN_ERR;

	return RETURN_OK;
}

void uci_converter_free_local_uci_context(void)
{
	_thread_local_uci_put_context(__ctx);
}

#ifndef UNUSED_PARAM
#define UNUSED_PARAM(param) (void)param
#endif

static inline const char* get_type_str(enum paramType type) {
	return (type == TYPE_RADIO) ? "radio" : "default_radio";
}

/* Internal macro which has to be called instead of direct call of uci_lookup_ptr().
 * This macro makes a local copy of "path" and prevents an external string buffer
 * from the modifications by uci_lookup_ptr()
 * Used mostly if "path" is passed as an input parameter of the API function. */
#define UCI_CONVERTER_LOOKUP_PTR(ctx,ptr,buffer,path,extended) ({ \
		int ret; \
		if (strcpy_s(buffer, sizeof(buffer), path)) { \
			ERROR("path is too long!\n"); \
			ret = UCI_ERR_NOTFOUND; \
		} else \
			ret = uci_lookup_ptr(ctx, ptr, buffer, extended); \
		ret; \
	})

#ifdef CONFIG_RPCD

static int _call_ubus_uci(struct blob_buf *blob, const char *operation)
{
	unsigned uci_id = (unsigned)-1;
	int ubus_ret;
	int ret = RETURN_ERR;
	struct ubus_context *ubus_ctx = NULL;
	const char *ubus_socket = NULL;

	ubus_ctx = ubus_connect(ubus_socket);
	if (!ubus_ctx) {
		CRIT("ubus_connect failed.\n");
		return RETURN_ERR;
	}

	if (ubus_lookup_id(ubus_ctx, "uci", &uci_id)) {
		CRIT("Requested server 'uci' not available \n");
		goto end;
	}

	/* DEBUG("uci id = %u\n", uci_id); */
	ubus_ret = ubus_invoke(ubus_ctx, uci_id, operation, blob->head, NULL, NULL, 30 * 1000);
	if(ubus_ret != 0) {
		ERROR("Ubus request to 'uci %s' failed (%d: %s)\n", operation, ubus_ret, ubus_strerror(ubus_ret));
		goto end;
	}
	DEBUG("Ubus request to 'uci %s': success\n", operation);
	ret = RETURN_OK;

end:
	ubus_free(ubus_ctx);
	return ret;
}

/* Write the parameter to the UCI database using UBUS service */
static int _ubus_uci_converter_set(const char* path, const char* option, const char* value)
{
	int ret;
	char buf_path[MAX_UCI_BUF_LEN] = "";
	char *config, *section, *start;
	struct blob_buf blob;
	void *tbl;

	if (is_empty_str(path) || is_empty_str(option) || is_empty_str(value)) {
		ERROR("At least one of received parameters is empty\n");
		return RETURN_ERR;
	}

	/* DEBUG("uci set operation called for path:'%s', option:'%s', value:'%s'\n", path, option, value); */
	if (strcpy_s(buf_path, sizeof(buf_path), path)) {
		ERROR("path is too long\n");
		return RETURN_ERR;
	}
	start = buf_path;

	/* Parse input path */
	config = start; strsep(&start, ".");
	if (!start) {
		ERROR("Can't find delimeter '.' in string '%s'\n", buf_path);
		return RETURN_ERR;
	}
	section = start;
	/* DEBUG("config:%s, section:%s\n", config, section); */

	memset(&blob, 0, sizeof(blob));
	blobmsg_buf_init(&blob);
	blobmsg_add_string(&blob, "config", config);
	blobmsg_add_string(&blob, "section", section);
	tbl = blobmsg_open_table(&blob, "values");
	blobmsg_add_string(&blob, option, value);
	blobmsg_close_table(&blob, tbl);

	ret = _call_ubus_uci(&blob, "set");
	if (blob.head != NULL)
		blob_buf_free(&blob);
	return ret;
}

/* Delete the parameter from the UCI database using UBUS */
static int _ubus_uci_converter_del(const char* path)
{
	int ret;
	char buf_path[MAX_UCI_BUF_LEN] = "";
	char *config = NULL, *section = NULL, *option = NULL, *start;
	struct blob_buf blob;

	if (is_empty_str(path)) {
		ERROR("Received parameters is empty\n");
		return RETURN_ERR;
	}

	/* DEBUG("uci del operation called for path:'%s'\n", path); */
	if (strcpy_s(buf_path, sizeof(buf_path), path)) {
		ERROR("Path is too long\n");
		return RETURN_ERR;
	}
	start = buf_path;

	/* Parse input path */
	config = start;
	strsep(&start, "."); if (start) section = start;
	strsep(&start, "."); if (start) option = start;

	/* DEBUG("config:%s, section:%s, option:%s\n", config, section, option); */

	memset(&blob, 0, sizeof(blob));
	blobmsg_buf_init(&blob);
	blobmsg_add_string(&blob, "config", config);
	if (section) blobmsg_add_string(&blob, "section", section);
	if (option) blobmsg_add_string(&blob, "option", option);

	ret = _call_ubus_uci(&blob, "delete");
	if (blob.head != NULL)
		blob_buf_free(&blob);
	return ret;
}

static int _ubus_uci_converter_commit(const char* config)
{
	int ret;
	struct blob_buf blob;

	if (is_empty_str(config)) {
		ERROR("Received parameters is empty\n");
		return RETURN_ERR;
	}

	memset(&blob, 0, sizeof(blob));
	blobmsg_buf_init(&blob);
	blobmsg_add_string(&blob, "config", config);

	ret = _call_ubus_uci(&blob, "commit");
	if (blob.head != NULL)
		blob_buf_free(&blob);

	return ret;
}
#endif /* CONFIG_RPCD */

/**************************************************************************/
/*! \fn int rpc_to_uci_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert rpc index to uci index using uci-db
 *  \param[in] enum paramType ifType: TYPE_RADIO/TYPE_RADIO_VAP/TYPE_VAP
 *  \param[in] int index - The rpc index
 *  \return the index if success, negative if error
 ***************************************************************************/
int rpc_to_uci_index(enum paramType iftype, int index)
{
	char index_path[MAX_UCI_BUF_LEN] = "";
	char *index_path_pre = NULL;
	char value_str[MAX_UCI_BUF_LEN] = "";
	int status = 0;
	switch(iftype)
	{
		case TYPE_RADIO:
			index_path_pre = "wireless.radio_rpc_indexes";
			break;
		case TYPE_RADIO_VAP:
			index_path_pre = "wireless.radio_vap_rpc_indexes";
			break;
		case TYPE_VAP:
			index_path_pre = "wireless.vap_rpc_indexes";
			break;
		default:
			ERROR("%s invalid iftype.", __func__);
			return RETURN_ERR;
	}

	status = sprintf_s(index_path, MAX_UCI_BUF_LEN, "%s.index%d",
			   index_path_pre, index);
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d", __func__, index);
		return RETURN_ERR;
	}

	status = uci_converter_get(index_path, value_str, MAX_UCI_BUF_LEN);
	if (status == RETURN_ERR)
		return RETURN_ERR;
	return atoi(value_str);
}

/**************************************************************************/
/*! \fn int uci_to_rpc_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert uci index to rpc index using uci-db
 *  \param[in] enum paramType ifType: TYPE_RADIO/TYPE_RADIO_VAP/TYPE_VAP
 *  \param[in] int index - The uci index
 *  \return the index if success, negative if error
 ***************************************************************************/
int uci_to_rpc_index(enum paramType iftype, int index)
{
	char index_path[MAX_UCI_BUF_LEN] = "";
	char *index_path_pre = NULL;
	char value_str[MAX_UCI_BUF_LEN] = "";
	int status = 0;
	switch(iftype)
	{
		case TYPE_RADIO:
			index_path_pre = "wireless.radio";
			break;
		case TYPE_RADIO_VAP:
			/* fall through */
		case TYPE_VAP:
			index_path_pre = "wireless.default_radio";
			break;
		default:
			ERROR("%s invalid iftype.", __func__);
			return RETURN_ERR;
	}

	status = sprintf_s(index_path, MAX_UCI_BUF_LEN, "%s%d.rpc_index",
			   index_path_pre, index);
	if (status <= 0) {
		ERROR("%s failed sprintf_s. index=%d", __func__, index);
		return RETURN_ERR;
	}

	status = uci_converter_get(index_path, value_str, MAX_UCI_BUF_LEN);
	if (status == RETURN_ERR)
		return RETURN_ERR;
	return atoi(value_str);
}

/**************************************************************************/
/*! \fn int dummy_to_radio_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert dummy vap uci index to radio uci index using uci-db
 *  \param[in] int index - The uci index of the dummy vap
 *  \return the index if success, negative if error
 ***************************************************************************/
static int dummy_to_radio_index(int index)
{
	int radio_idx = 0;
	char radio[MAX_UCI_BUF_LEN] = "";
	if (uci_converter_get_str_ext(TYPE_RADIO_VAP, uci_to_rpc_index(TYPE_RADIO_VAP, index), "device", radio, sizeof(radio)) == RETURN_ERR)
		return RETURN_ERR;

	if (sscanf_s(radio, "radio%d", &radio_idx) != 1) {
		return RETURN_ERR;
	}

	return radio_idx;
}

static int uci_get_existing_interfaces_imp(enum ifType if_type, const char *radioName,
		int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound)
{
	int itemsRead = 0;
	int index_read = 0;
	struct uci_ptr ptr;
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	struct uci_element *e;
	struct uci_section *s;
	char lookup_str[MAX_UCI_BUF_LEN] = "wireless";

	if ((ifacesArr == NULL) || (numOfIfFound == NULL)) {
		ERROR("%s, ifacesArr or numOfIfFound is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	*numOfIfFound = 0;

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	p = ptr.p;

	uci_foreach_element(&p->sections, e) {
		struct uci_element *n = 0;

		s = uci_to_section(e);
		if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
			continue;

		switch (if_type) {
			case IFTYPE_AP:
				if (!uci_converter_if_ap(s)) continue;
				break;
			case IFTYPE_STA:
				if (uci_converter_if_ap(s)) continue;
				break;
			default:
				break;
		}

		uci_foreach_element(&s->options, n) {

			if (strncmp(n->name, "device", MAX_UCI_BUF_LEN))
				continue;

			if (radioName && strncmp(uci_to_option(n)->v.string, radioName, MAX_UCI_BUF_LEN))
				continue;/*the vap does not belong to the requested radio*/

			itemsRead = sscanf_s(s->e.name, "default_radio%d", &index_read);
			if (uci_converter_is_dummy(index_read))
				continue;

			if (itemsRead == 1) {
				if(ifacesArrSize <= *numOfIfFound) {
					_thread_local_uci_put_context(ctx);
					return RETURN_ERR;
				}

				ifacesArr[*numOfIfFound] = index_read;
				(*numOfIfFound)++;
			}
		}
	}

	_thread_local_uci_put_context(ctx);
	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_get_existing_interfaces(int *ifacesArr, const unsigned int ifacesArrSize)
 **************************************************************************
 *  \brief return an array containing the indexes of all AP interfaces
 *  \param[out] int *ifacesArr - buffer to contain the ouput array
 *  \param[in] const unsigned int ifacesArrSize - max size of ifacesArr
 *	\param[out] int *numOfIfFound - the number of interfaces found
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_get_existing_interfaces(int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound)
{
	return uci_get_existing_interfaces_imp(IFTYPE_AP, NULL, ifacesArr, ifacesArrSize, numOfIfFound);
}

/**************************************************************************/
/*! \fn int uci_get_existing_interfaces_in_radio(int *ifacesArr, const unsigned int ifacesArrSize)
 **************************************************************************
 *  \brief return an array containing the indexes of all AP interfaces in the current radio
 *  \param[in] const char *radio - radio name
 *  \param[out] int *ifacesArr - buffer to contain the ouput array
 *  \param[in] const unsigned int ifacesArrSize - max size of ifacesArr
 *	\param[out] int *numOfIfFound - the number of interfaces found
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_get_existing_interfaces_in_radio(const char *radio, int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound)
{
	return uci_get_existing_interfaces_imp(IFTYPE_AP, radio, ifacesArr, ifacesArrSize, numOfIfFound);
}

/**************************************************************************/
/*! \fn int uci_get_existing_sta_interfaces(int *ifacesArr, const unsigned int ifacesArrSize)
 **************************************************************************
 *  \brief return an array containing the indexes of all STA interfaces
 *  \param[out] int *ifacesArr - buffer to contain the ouput array
 *  \param[in] const unsigned int ifacesArrSize - max size of ifacesArr
 *	\param[out] int *numOfIfFound - the number of interfaces found
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_get_existing_sta_interfaces(int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound)
{
	return uci_get_existing_interfaces_imp(IFTYPE_STA, NULL, ifacesArr, ifacesArrSize, numOfIfFound);
}

/**************************************************************************/
/*! \fn int uci_get_existing_sta_interfaces_in_radio(int *ifacesArr, const unsigned int ifacesArrSize)
 **************************************************************************
 *  \brief return an array containing the indexes of all STA interfaces in the current radio
 *  \param[in] const char *radio - radio name
 *  \param[out] int *ifacesArr - buffer to contain the ouput array
 *  \param[in] const unsigned int ifacesArrSize - max size of ifacesArr
 *	\param[out] int *numOfIfFound - the number of interfaces found
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_get_existing_sta_interfaces_in_radio(const char *radio, int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound)
{
	return uci_get_existing_interfaces_imp(IFTYPE_STA, radio, ifacesArr, ifacesArrSize, numOfIfFound);
}

/**************************************************************************/
/*! \fn int uci_get_param_change(enum paramType ifType, int index, const char *paramName, char* paramChange)
 **************************************************************************
 *  \brief get the new value of the parameter as it will be pushed in the new uci commit
 *  \param[in] enum paramType ifType: TYPE_RADIO/TYPE_VAP
 *  \param[in] int index - The index of the entry
 *  \param[in] const char *paramName - parameter name to get the value of
 *  \param[out] char* paramChange - the new parameter value if a new value is found
 *  \param[in] size_t size - size of the output string buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_get_param_change(enum paramType ifType, int index, const char *paramName, char* paramChange, size_t size)
{
	struct uci_ptr ptr = {0};
	struct uci_element *e = NULL;
	struct uci_context *ctx = NULL;
	int status = 0;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = NULL;

	if ((paramName == NULL) || (paramChange == NULL)) {
		ERROR("%s, paramName or paramChange is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if (ifType != TYPE_VAP && ifType != TYPE_RADIO) {
		ERROR("%s, ifType %d is not supported!\n", __FUNCTION__, ifType);
		return RETURN_ERR;
	}

	radio_str = get_type_str(ifType);

	status = sprintf_s(path, sizeof(path), "wireless.%s%d", radio_str, rpc_to_uci_index(ifType, index));
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d\n", __func__, index);
		return RETURN_ERR;
	}

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((uci_lookup_ptr(ctx, &ptr, path, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	if (ptr.p == NULL) {
		ERROR("%s, uci lookup returned a NULL ptr in ptr.p!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	status = sprintf_s(path, sizeof(path), "%s%d", radio_str, rpc_to_uci_index(ifType, index));
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d\n", __func__, index);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	uci_foreach_element(&ptr.p->saved_delta, e) {
		struct uci_delta *h = uci_to_delta(e);
		if (h && !strncmp(e->name, paramName, MAX_UCI_BUF_LEN)) {
			if (!strncmp(h->section, path, MAX_UCI_BUF_LEN)) {
				status = strcpy_s(paramChange, size, h->value);
				_thread_local_uci_put_context(ctx);
				if (status) {
					ERROR("%s, output buffer is too small!\n", __FUNCTION__);
					return RETURN_ERR;
				}
				return RETURN_OK;
			}
		}
	}

	_thread_local_uci_put_context(ctx);
	return RETURN_OK;
}

static bool uci_converter_if_ap(struct uci_section *s)
{
	bool res = false;
	struct uci_element *n;

	uci_foreach_element(&s->options, n) {
		struct uci_option *o = uci_to_option(n);

		if (strncmp(n->name, "mode", MAX_UCI_BUF_LEN))
			continue;

		if (o->type != UCI_TYPE_STRING)
			continue;

		if (!strncmp(o->v.string, "ap", MAX_UCI_BUF_LEN))
			res = true;

		break;
	}

	return res;
}

static bool uci_converter_is_sta(int uci_index)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	char uci_ret[MAX_UCI_BUF_LEN] = "";

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.default_radio%d.mode", uci_index);
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d\n", __func__, uci_index);
		return false;
	}

	status = uci_converter_get(path, uci_ret, MAX_UCI_BUF_LEN);
	if ((status == RETURN_OK) && !strncmp(uci_ret, "sta", MAX_UCI_BUF_LEN))
		return true;
	return false;
}

static int sta_to_radio_index(int index)
{
	int radio_idx = 0;
	char radio[MAX_UCI_BUF_LEN] = "";
	if (uci_converter_get_str_by_uci_index(TYPE_VAP, index, "device", radio, sizeof(radio)) == RETURN_ERR)
		return RETURN_ERR;

	if (sscanf_s(radio, "radio%d", &radio_idx) != 1) {
		return RETURN_ERR;
	}

	return radio_idx;
}

/**************************************************************************/
/*! \fn int uci_converter_is_dummy(int uci_index)
 **************************************************************************
 *  \brief get wheter vap is dummy or not based on uci index
 *  \param[in] int uci_index - The index of the vap entry
 *  \return true if dummy, false if not dummy / error
 ***************************************************************************/
bool uci_converter_is_dummy(int uci_index)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	char uci_ret[MAX_UCI_BUF_LEN] = "";

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.radio_vap_rpc_indexes.index%d", uci_to_rpc_index(TYPE_RADIO_VAP, uci_index));
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d\n", __func__, uci_index);
		return false;
	}

	status = uci_converter_get(path, uci_ret, MAX_UCI_BUF_LEN);
	if ((status == RETURN_OK) && (atoi(uci_ret) == uci_index))
		return true;
	return false;
}

static int uci_converter_count_elems_by_type(enum ifType if_type, const char *sec_type, const char *sec_name,
		const char* opt_name, const char* opt_val, int *num)
{
	int wireless_idx;
	struct uci_ptr ptr;
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	struct uci_element *e;
	struct uci_section *s;
	char lookup_str[MAX_UCI_BUF_LEN] = "wireless";

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	p = ptr.p;
	*num = 0;

	uci_foreach_element(&p->sections, e) {
		struct uci_element *n;

		s = uci_to_section(e);

		if (strncmp(s->type, sec_type, MAX_UCI_BUF_LEN))
			continue;


		if (sec_name && strncmp(s->e.name, sec_name, MAX_UCI_BUF_LEN))
			continue;

		if (sscanf_s(s->e.name, "default_radio%d", &wireless_idx) == 1) {
			if (uci_converter_is_dummy(wireless_idx))
				continue;
		}

		if (!strncmp(sec_type, "wifi-iface", MAX_UCI_BUF_LEN)) {
			switch (if_type) {
				case IFTYPE_AP:
					if (!uci_converter_if_ap(s)) continue;
					break;
				case IFTYPE_STA:
					if (uci_converter_if_ap(s)) continue;
					break;
				default:
					break;
			}
		}

		if (opt_name){
			uci_foreach_element(&s->options, n) {
				struct uci_option *o = uci_to_option(n);

				if (strncmp(n->name, opt_name, MAX_UCI_BUF_LEN))
					continue;

				if (o->type != UCI_TYPE_STRING)
					continue;

				if (opt_val && strncmp(opt_val, o->v.string, MAX_UCI_BUF_LEN))
					continue;
				(*num)++;
			}
		} else {
			(*num)++;
		}
	}

	_thread_local_uci_put_context(ctx);

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_count_elems(const char *sec_type, const char *sec_name, const char* opt_name, const char* opt_val,int *num)
 **************************************************************************
 *  \brief count elements in UCI DB (for iface count ap only)
 *  \param[in] const char *sec_type - section type, mandatory
 *  \param[in] const char *sec_name - section name, optional, used for extra filtering
 *  \param[in] const char* opt_name - option name, optional, used for extra filtering
 *  \param[in] const char* opt_val - option value, optional, used for extra filtering
 *  \param[out] int *num - return the element count
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_count_elems(const char *sec_type, const char *sec_name, const char* opt_name, const char* opt_val, int *num)
{
	return uci_converter_count_elems_by_type(IFTYPE_AP, sec_type, sec_name, opt_name, opt_val, num);
}

/**************************************************************************/
/*! \fn int uci_converter_count_sta_elems(const char *sec_type, const char *sec_name, const char* opt_name, const char* opt_val,int *num)
 **************************************************************************
 *  \brief count elements in UCI DB (for iface count stations only)
 *  \param[in] const char *sec_type - section type, mandatory
 *  \param[in] const char *sec_name - section name, optional, used for extra filtering
 *  \param[in] const char* opt_name - option name, optional, used for extra filtering
 *  \param[in] const char* opt_val - option value, optional, used for extra filtering
 *  \param[out] int *num - return the element count
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_count_sta_elems(const char *sec_type, const char *sec_name, const char* opt_name, const char* opt_val, int *num)
{
	return uci_converter_count_elems_by_type(IFTYPE_STA, sec_type, sec_name, opt_name, opt_val, num);
}

/**************************************************************************/
/*! \fn int uci_getIndexFromInterface(const char *interfaceName, int *rpc_index)
 **************************************************************************
 *  \brief get UCI index from the interface name
 *  \param[in] const char *interfaceName - interface name string
 *  \param[out] int *rpc_index - output the UCI index
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_getIndexFromInterface(const char *interfaceName, int *rpc_index)
{
	struct uci_ptr ptr;
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	struct uci_element *e;
	struct uci_section *s;
	char lookup_str[MAX_UCI_BUF_LEN] = "wireless";

	if ((interfaceName == NULL) || (rpc_index == NULL)) {
		ERROR("%s, interfaceName or rpc_index is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	p = ptr.p;

	uci_foreach_element(&p->sections, e) {
		struct uci_element *n;
		int scanf_res;

		s = uci_to_section(e);

		if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
			continue;

		uci_foreach_element(&s->options, n) {
			struct uci_option *o = uci_to_option(n);

			if (strncmp(n->name, "ifname", MAX_UCI_BUF_LEN))
				continue;

			if (o->type != UCI_TYPE_STRING)
				continue;

			if (strncmp(interfaceName, o->v.string, MAX_UCI_BUF_LEN))
				continue;

			scanf_res = sscanf_s(s->e.name, "default_radio%d", rpc_index);

			if (scanf_res < 1 || *rpc_index < 0) {
				*rpc_index = -1;
				_thread_local_uci_put_context(ctx);
				return RETURN_ERR;
			}
			/* if it is dummy we want the radio index, not the dummy one */
			if (uci_converter_is_dummy(*rpc_index)) {
				*rpc_index = dummy_to_radio_index(*rpc_index);
				if (*rpc_index == RETURN_ERR) {
					_thread_local_uci_put_context(ctx);
					return RETURN_ERR;
				}
			}

			/* if it is sta we want the radio index, not the sta one */
			if (uci_converter_is_sta(*rpc_index)) {
				*rpc_index = sta_to_radio_index(*rpc_index);
				if (*rpc_index == RETURN_ERR) {
					_thread_local_uci_put_context(ctx);
					return RETURN_ERR;
				}
			}

			_thread_local_uci_put_context(ctx);
			return RETURN_OK;
		}
	}
	*rpc_index = -1;
	_thread_local_uci_put_context(ctx);
	return RETURN_ERR;

}

/**************************************************************************/
/*! \fn int uci_getIndexFromBssid(const char *bssid, int *rpc_index)
 **************************************************************************
 *  \brief get UCI index from the bssid
 *  \param[in] const char *bssid - bssid string
 *  \param[out] int *rpc_index - output the UCI index
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_getIndexFromBssid(const char *bssid, int *rpc_index)
{
	struct uci_ptr ptr;
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	struct uci_element *e;
	struct uci_section *s;
	char lookup_str[MAX_UCI_BUF_LEN] = "wireless";

	if ((bssid == NULL) || (rpc_index == NULL)) {
		ERROR("%s, bssid or rpc_index is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	p = ptr.p;

	uci_foreach_element(&p->sections, e) {
		struct uci_element *n;
		int scanf_res;

		s = uci_to_section(e);

		if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
			continue;

		uci_foreach_element(&s->options, n) {
			struct uci_option *o = uci_to_option(n);

			if (strncmp(n->name, "macaddr", MAX_UCI_BUF_LEN))
				continue;

			if (o->type != UCI_TYPE_STRING)
				continue;

			if (strncmp(bssid, o->v.string, MAX_UCI_BUF_LEN))
				continue;

			scanf_res = sscanf_s(s->e.name, "default_radio%d", rpc_index);

			if (scanf_res < 1 || *rpc_index < 0) {
				*rpc_index = -1;
				_thread_local_uci_put_context(ctx);
				return RETURN_ERR;
			}
			/* if it is dummy we want the radio index, not the dummy one */
			if (uci_converter_is_dummy(*rpc_index)) {
				*rpc_index = dummy_to_radio_index(*rpc_index);
				if (*rpc_index == RETURN_ERR) {
					_thread_local_uci_put_context(ctx);
					return RETURN_ERR;
				}
			}

			_thread_local_uci_put_context(ctx);
			return RETURN_OK;
		}
	}
	*rpc_index = -1;
	_thread_local_uci_put_context(ctx);
	return RETURN_ERR_NOT_FOUND;

}


extern char **environ;
int uci_converter_system_print(const char *cmd)
{
	int st;
	char print_cmd[MAX_LEN_PARAM_VALUE];

	if (cmd == NULL) {
		ERROR("%s, cmd is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}
	st = sprintf_s(print_cmd, MAX_LEN_PARAM_VALUE, "%s >> /dev/console 2>&1", cmd);
	if (st <= 0) {
		ERROR("%s failed sprintf_s", __func__);
		return RETURN_ERR;
	}
	return uci_converter_system(print_cmd);
}

/**************************************************************************/
/*! \fn int uci_converter_system(char *cmd)
 **************************************************************************
 *  \brief run command using system - not recommended to use (plus - need root permissions!)
 *  \param[in] char *cmd - command string to run via system
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_system(char *cmd)
{
	return uci_converter_popen(cmd, NULL, -1);
}

/**************************************************************************/
/*! \fn int uci_converter_popen(char *cmd, char *output, int outputSize)
 **************************************************************************
 *  \brief run command using system, saving result - not recommended to use (plus - need root permissions!)
 *  \param[in] char *cmd - command string to run via system
 *  \param[out] char *output - save result
 *  \param[in] int outputSize - size of output buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_popen(char *cmd, char *output, int outputSize)
{
	int link[2];
	pid_t pid;
	int pstat = 0;
	int ret = 0;
	int res = RETURN_OK;
	char *argp[] = {"sh", "-c", cmd, NULL};
	int stringSize = outputSize - 1;

	if (cmd == NULL) {
		ERROR("%s, cmd is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if (pipe(link)==-1) {
		ERROR("%s, pipe error\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if ((pid = fork()) == -1) {
		ERROR("%s, fork error\n", __FUNCTION__);
		close(link[0]);
		close(link[1]);
		return RETURN_ERR;
	}

	if (pid == 0) {
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		execve(_PATH_BSHELL, argp, environ);
		return RETURN_ERR;
	} else {
		int bytesRead = 0;
		close(link[1]);
		if (output) {
			while (bytesRead < stringSize) {
				ret = read(link[0], output + bytesRead, stringSize - bytesRead);

				if (ret == 0) {
					break;
				}
				if (ret == -1) {
					ERROR("%s, read error\n", __FUNCTION__);
					break;
				}
				bytesRead += ret;
			}
			output[bytesRead] = '\0';
		}
		do {
			ret = waitpid(pid, &pstat, 0);
			if (ret == -1 && errno == EINTR) {
				DEBUG("%s: EINTR recieved, restarting cur_pid=%d\n", __FUNCTION__, pid);
				continue;
			}

			if (WIFEXITED(pstat)) {
				int status = WEXITSTATUS(pstat);
				/* if there is a pipe the status is of the last cmd,
				 * so it is meaningless in term of success or failure */
				if (status && !strstr(cmd, "|") && !strstr(cmd, ">> /dev/console")) {
					ERROR("%s: cmd:%s - exited, status=%d\n", __FUNCTION__, cmd, status);
					res = RETURN_ERR;
					goto end;
				}
				DEBUG("%s: exited, status=%d\n", __FUNCTION__, status);
				break;
			} else if (WIFSIGNALED(pstat)) {
				ERROR("%s: killed by signal %d\n", __FUNCTION__, WTERMSIG(pstat));
				res = RETURN_ERR;
				goto end;
			} else if (WIFSTOPPED(pstat)) {
				ERROR("%s: stopped by signal %d\n", __FUNCTION__, WSTOPSIG(pstat));
				res = RETURN_ERR;
				goto end;
			}
		} while (ret != -1);
	}

	/* Remove trailing new line character if needed */
	if (output && (output[strnlen_s(output, outputSize)-1] == '\n'))
		output[strnlen_s(output, outputSize)-1] = '\0';

end:
	close(link[0]);
	return res;
}

/**************************************************************************/
/*! \fn int uci_converter_get(const char* path, char* value, size_t size)
 **************************************************************************
 *  \brief generic get from UCI database
 *  \param[in] const char *path - variable full name to get (config file.device name.option) for example wireless.default_radio45.wps_state
 *  \param[out] char *value - value from UCI DB 
 *  \param[in] size_t size - size of the value buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get(const char* path, char* value, size_t size)
{
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((path == NULL) || (value == NULL)) {
		ERROR("%s, path or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK || !ptr.o) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (strcpy_s(value, size, ptr.o->v.string)) {
		ERROR("%s, output buffer is too small!\n", __FUNCTION__);
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	_thread_local_uci_put_context(cont);
	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_get_str_ext(enum paramType type, int index, const char param[], char *value, size_t size)
 **************************************************************************
 *  \brief get a specific radio or VAP param string from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP UCI index
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] char *value - string value from UCI DB 
 *  \param[in] size_t size - size of the output string buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_str_by_uci_index(enum paramType type, int uciIndex, const char param[], char *value, size_t size)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str, uciIndex, param);
	if (status <= 0) {
		ERROR("%s failed sprintf_s. uciIndex=%d param=%s\n", __func__, uciIndex, param);
		return RETURN_ERR;
	}

	status = uci_converter_get(path, value, size);
	if (status == RETURN_OK) {
		DEBUG("%s uciIndex=%d %s=%s\n", __func__, uciIndex, param, value);
	} else {
		INFO("%s option N/A. uciIndex=%d param=%s\n", __func__, uciIndex, param);
	}

	return status;
}

/**************************************************************************/
/*! \fn int uci_converter_get_str_ext(enum paramType type, int index, const char param[], char *value, size_t size)
 **************************************************************************
 *  \brief get a specific radio or VAP param string from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] char *value - string value from UCI DB
 *  \param[in] size_t size - size of the output string buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_str_ext(enum paramType type, int index, const char param[], char *value, size_t size)
{
	int ret;

	if (uci_converter_alloc_local_uci_context()) {
		ERROR("alloc local context returned err!\n");
		return RETURN_ERR;
	}

	ret = uci_converter_get_str_by_uci_index(type, rpc_to_uci_index(type, index), param, value, size);

	uci_converter_free_local_uci_context();
	return ret;
}

/**************************************************************************/
/*! \fn int uci_converter_get_str(enum paramType type, int index, const char param[], char *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param string from UCI database.
 *  THIS IS OBSOLETE FUNCTION and MUST NOT BE USED!
 *  Use uci_converter_get_str_ext() insted.
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] char *value - string value from UCI DB
  *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_str(enum paramType type, int index, const char param[], char *value)
{
       return uci_converter_get_str_ext(type, index, param, value, MAX_UCI_BUF_LEN);
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_str(enum paramType type, int index, const char param[], char *value, char* default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param string from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] char *value - string value from UCI DB 
 *  \param[in] size_t *size - size of the value buffer
 *  \param[in] char *default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_str(enum paramType type, int index, const char param[], char *value, size_t size, char* default_val)
{
	int status;

	if ((param == NULL) || (value == NULL) || (default_val == NULL)) {
		ERROR("%s, param or value or default_val is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_str_ext(type, index, param, value, size);
	if (status == RETURN_ERR)
		strncpy_s(value, size, default_val, strnlen_s(default_val, size - 1));
}

/**************************************************************************/
/*! \fn int uci_converter_get_int(enum paramType type, int index, const char param[], int *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param integer from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] int *value - integer value from UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_int(enum paramType type, int index, const char param[], int *value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = uci_converter_get_str_ext(type, index, param, val, sizeof(val));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = sscanf_s(val, "%d", value);
	if (status != 1)
		return RETURN_ERR;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_int(enum paramType type, int index, const char param[], int *value, int default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param integer from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] int *value - integer value from UCI DB 
 *  \param[in] int default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_int(enum paramType type, int index, const char param[], int *value, int default_val)
{
	int status;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_int(type, index, param, value);
	if (status == RETURN_ERR)
		*value = default_val;
}

/**************************************************************************/
/*! \fn int uci_converter_get_uint(enum paramType type, int index, const char param[], unsigned int *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned integer from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned int *value - unsigned integer value from UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_uint(enum paramType type, int index, const char param[], unsigned int *value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = uci_converter_get_str_ext(type, index, param, val, sizeof(val));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = sscanf_s(val, "%u", value);
	if (status != 1)
		return RETURN_ERR;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_uint(enum paramType type, int index, const char param[], unsigned int *value, unsigned int default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned integer from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned int *value - unsigned integer value from UCI DB 
 *  \param[in] unsigned int default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_uint(enum paramType type, int index, const char param[], unsigned int *value, unsigned int default_val)
{
	int status;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_uint(type, index, param, value);
	if (status == RETURN_ERR)
		*value = default_val;
}

/**************************************************************************/
/*! \fn int uci_converter_get_ulong(enum paramType type, int index, const char param[], unsigned long *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned long from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned long *value - unsigned long value from UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_ulong(enum paramType type, int index, const char param[], unsigned long *value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = uci_converter_get_str_ext(type, index, param, val, sizeof(val));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = sscanf_s(val, "%lu", value);
	if (status < 1)
		return RETURN_ERR;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_ulong(enum paramType type, int index, const char param[], unsigned long *value, unsigned long default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned long from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned long *value - unsigned long value from UCI DB 
 *  \param[in] unsigned long default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_ulong(enum paramType type, int index, const char param[], unsigned long *value, unsigned long default_val)
{
	int status;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_ulong(type, index, param, value);
	if (status == RETURN_ERR)
		*value = default_val;
}

/**************************************************************************/
/*! \fn int uci_converter_get_ushort(enum paramType type, int index, const char param[], unsigned short *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned short from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned short *value - unsigned short value from UCI DB
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_ushort(enum paramType type, int index, const char param[], unsigned short *value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = uci_converter_get_str_ext(type, index, param, val, sizeof(val));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = sscanf_s(val, "%hu", value);
	if (status < 1)
		return RETURN_ERR;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_ushort(enum paramType type, int index, const char param[], unsigned short *value, unsigned short default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param unsigned short from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] unsigned short *value - unsigned short value from UCI DB
 *  \param[in] unsigned short default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_ushort(enum paramType type, int index, const char param[], unsigned short *value, unsigned short default_val)
{
	int status;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_ushort(type, index, param, value);
	if (status == RETURN_ERR)
		*value = default_val;
}

/**************************************************************************/
/*! \fn int uci_converter_get_bool(enum paramType type, int index, const char param[], bool *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param boolean from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] bool *value - boolean value from UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_bool(enum paramType type, int index, const char param[], bool *value)
{
	int status;
	int res;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = uci_converter_get_int(type, index, param, &res);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	*value = (res != 0) ? true : false;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn void uci_converter_get_optional_bool(enum paramType type, int index, const char param[], bool *value, bool default_val)
 **************************************************************************
 *  \brief get a specific radio or VAP param boolean from UCI database, if not found return a default value
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] bool *value - boolean value from UCI DB 
 *  \param[in] bool default_val - in case not found return this value as default
 ***************************************************************************/
void uci_converter_get_optional_bool(enum paramType type, int index, const char param[], bool *value, bool default_val)
{
	int status;

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return;
	}

	status = uci_converter_get_bool(type, index, param, value);
	if (status == RETURN_ERR)
		*value = default_val;
}

#ifdef CONFIG_RPCD

/* "Set" for compatibility with old API */
static int (*g_uci_converter_cb_set_compat)(char* path,
		const char* option, const char* value) = NULL;

/* Obsolete function */
void set_uci_converter_fun(int (*callback)(char* path, const char* option, const char* value))
{
        INFO("%s:%d\n", __FUNCTION__, __LINE__);
        /* DEBUG("%s:%d uci_converter_set: %p cb: %p\n", __FUNCTION__, __LINE__, (void*)uci_converter_set_str, (void*)callback); */
        g_uci_converter_cb_set_compat = callback;
        return;
}

#else

void set_uci_converter_fun(int (*callback)(char* path, const char* option, const char* value))
{
	/* Do nothing */
	UNUSED_PARAM(callback);
}
#endif /* CONFIG_RPCD */

/**************************************************************************/
/*! \fn int uci_converter_set(const char* path, const char* option, const char* value)
 **************************************************************************
 *  \brief generic set to UCI database
 *  \param[in] const char *path - config name to set (config file.device name) for example wireless.default_radio45
 *  \param[in] const char *option - option (param) name to set
 *  \param[in] const char *value - value to write tom UCI DB
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set(const char* path, const char* option, const char* value)
#ifdef CONFIG_RPCD
{
	if (g_uci_converter_cb_set_compat) {
		/* Call the old API*/
		if (path) {
			char path_buf[MAX_UCI_BUF_LEN] = "";

			if (strcpy_s(path_buf, sizeof(path_buf), path)) {
				ERROR("%s, path is too long!\n", __FUNCTION__);
				return RETURN_ERR;
			}
			return g_uci_converter_cb_set_compat(path_buf, option, value);
		}
		else {
			return g_uci_converter_cb_set_compat(NULL, option, value);
		}
	} else {
		return _ubus_uci_converter_set(path, option, value);
	}
}
#else
{
	int status;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((path == NULL) || (option == NULL) || (value == NULL)) {
		ERROR("%s, path or option or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if ((UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK) || (!ptr.s)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	ptr.option = option;
	ptr.value = value;

	if (uci_set(cont, &ptr) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}
#endif /* CONFIG_RPCD */

/**************************************************************************/
/*! \fn int uci_converter_add_device(const char* config_file, const char* device_name, const char* device_type)
 **************************************************************************
 *  \brief add a device to UCI database (like a new VAP)
 *  \param[in] const char* config_file - config file name for example wireless
 *  \param[in] const char* device_name - device name for example default_radio45
 *  \param[in] const char* device_type - device type for example wifi-iface
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_add_device(const char* config_file, const char* device_name, const char* device_type)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	struct uci_ptr ptr;
	struct uci_context *cont;

	if (!config_file || !device_name || !device_type)
		return RETURN_ERR;

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "%s.%s=%s", config_file, device_name, device_type);
	if (status <= 0)
		return RETURN_ERR;

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if ((uci_lookup_ptr(cont, &ptr, path, true) != UCI_OK)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (uci_set(cont, &ptr) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}

/**************************************************************************/
/*! \fn int uci_converter_set_str(enum paramType type, int index, const char param[], const char *value)
 **************************************************************************
 *  \brief set a specific radio or VAP param string to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] const char *value - string value to set in UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_str(enum paramType type, int index, const char param[], const char *value)
{
	int ret = RETURN_ERR;
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);
	char get_value[MAX_LEN_VALID_VALUE] = "";

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if (uci_converter_alloc_local_uci_context()) {
		ERROR("alloc local context returned err!\n");
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d", radio_str, rpc_to_uci_index(type, index));
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d param=%s\n", __func__, index, param);
		goto end;
	}

	uci_converter_get_optional_str(type, index, param, get_value, sizeof(get_value), "");
	if (!strncmp(value, get_value, sizeof(get_value))) {
		DEBUG("%s same value being set index=%d %s=%s\n", __func__, index, param, value);
		ret = RETURN_OK;
		goto end;
	}

	status = uci_converter_set(path, param, value);
	if (status == RETURN_OK) {
		DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
	} else {
		ERROR("%s failed. index=%d %s=%s\n", __func__, index, param, value);
		goto end;
	}

	ret = RETURN_OK;

end:
	uci_converter_free_local_uci_context();
	return ret;
}

/**************************************************************************/
/*! \fn int uci_converter_set_int(enum paramType type, int index, const char param[], int value)
 **************************************************************************
 *  \brief set a specific radio or VAP param integer to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] int value - integer value to set in UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_int(enum paramType type, int index, const char param[], int value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if (param == NULL) {
		ERROR("%s, param is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(val, MAX_UCI_BUF_LEN, "%d", value);
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_set_str(type, index, param, val);

	return status;
}

/**************************************************************************/
/*! \fn int uci_converter_set_uint(enum paramType type, int index, const char param[], unsigned int value)
 **************************************************************************
 *  \brief set a specific radio or VAP param unsigned integer to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] unsigned int value - unsigned integer value to set in UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_uint(enum paramType type, int index, const char param[], unsigned int value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if (param == NULL) {
		ERROR("%s, param is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(val, MAX_UCI_BUF_LEN, "%u", value);
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_set_str(type, index, param, val);

	return status;
}

/**************************************************************************/
/*! \fn int uci_converter_set_ulong(enum paramType type, int index, const char param[], unsigned long value)
 **************************************************************************
 *  \brief set a specific radio or VAP param unsigned long to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] unsigned long value - unsigned long value to set in UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_ulong(enum paramType type, int index, const char param[], unsigned long value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	if (param == NULL) {
		ERROR("%s, param is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(val, MAX_UCI_BUF_LEN, "%lu", value);
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_set_str(type, index, param, val);

	return status;
}

/**************************************************************************/
/*! \fn int uci_converter_set_ushort(enum paramType type, int index, const char param[], unsigned short value)
 **************************************************************************
 *  \brief set a specific radio or VAP param unsigned short to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] unsigned short value - unsigned short value to set in UCI DB
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_ushort(enum paramType type, int index, const char param[], unsigned short value)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	status = sprintf_s(val, MAX_UCI_BUF_LEN, "%u", value);
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_set_str(type, index, param, val);

	return status;
}

/**************************************************************************/
/*! \fn int uci_converter_set_bool(enum paramType type, int index, const char param[], bool value)
 **************************************************************************
 *  \brief set a specific radio or VAP param boolean to UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[in] bool value - boolean value to set in UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_set_bool(enum paramType type, int index, const char param[], bool value)
{
	if (param == NULL) {
		ERROR("%s, param is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	return uci_converter_set_int(type, index, param, (value ? 1 : 0));
}

/**************************************************************************/
/*! \fn int uci_converter_del_elem(const char* path)
 **************************************************************************
 *  \brief remove an element from the UCI database (using full path)
 *  \param[in] const char *path - complete path of the element to delete
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_del_elem(const char* path)
#ifdef CONFIG_RPCD
{
	return _ubus_uci_converter_del(path);
}
#else
{
	int status = RETURN_ERR;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if (path == NULL) {
		ERROR("%s, path is NULL!\n", __FUNCTION__);
		goto out;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		goto out;

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK)
		goto out_after_cont;

	if (!ptr.o) { /* Param not found, nothing to do apprently */
		status = RETURN_OK;
		goto out_after_cont;
	}

	if (uci_delete(cont, &ptr) != UCI_OK)
		goto out_after_cont;

	if (uci_save(cont, ptr.p) != UCI_OK)
		goto out_after_cont;

	status = RETURN_OK;

out_after_cont:
	_thread_local_uci_put_context(cont);
out:
	return status;
}
#endif /* CONFIG_RPCD */

/**************************************************************************/
/*! \fn int uci_converter_del(enum paramType type, int index, const char* param)
 **************************************************************************
 *  \brief remove an element from the UCI database (using param name)
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to delete (not full path)
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_del(enum paramType type, int index, const char* param)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if (param == NULL) {
		ERROR("%s, param is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if (sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s",
		radio_str, rpc_to_uci_index(type, index), param) <= 0) {
		ERROR("%s failed sprintf_s.  index=%d param=%s", __func__, index, param);
		return RETURN_ERR;
	}

	status = uci_converter_del_elem(path);
	if (status == RETURN_ERR) {
		ERROR("%s failed: delete %s for index %d\n", __func__, param, index);
		return status;
	}

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_add_list(const char* path, const char* option, const char* value)
 **************************************************************************
 *  \brief append a string to a UCI element list (using path and param)
 *  \param[in] const char *path - element path (config file.device name)
 *  \param[in] const char *option - param name to append string to
 *  \param[in] const char *value - string value to append
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_add_list(const char* path, const char* option, const char* value)
{
	int status;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((path == NULL) || (option == NULL) || (value == NULL)) {
		ERROR("%s, path or option or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if ((UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK) || (!ptr.s)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	ptr.option = option;
	ptr.value = value;

	if (uci_add_list(cont, &ptr) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}

/**************************************************************************/
/*! \fn int uci_converter_add_list_str(enum paramType type, int index, const char param[], const char *value)
 **************************************************************************
 *  \brief append a string to a UCI element list (using param)
 *  \param[in] char *path - element path (config file.device name) 
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to append string to
 *  \param[in] const char *value - string value to append
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_add_list_str(enum paramType type, int index, const char param[], const char *value)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d", radio_str, rpc_to_uci_index(type, index));
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_add_list(path, param, value);
	if (status == RETURN_OK) {
		DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
	} else {
		ERROR("%s failed. index=%d %s=%s\n", __func__, index, param, value);
		return RETURN_ERR;
	}

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_del_list(const char* path, const char* option, const char* value)
 **************************************************************************
 *  \brief delete a string from a UCI element list (using path and param)
 *  \param[in] const char *path - element path (config file.device name)
 *  \param[in] const char *option - param name to remove string from
 *  \param[in] const char *value - string value to remove
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_del_list(const char* path, const char* option, const char* value)
{
	int status;
	char full_path[MAX_LEN_VALID_VALUE] = "";
	struct uci_ptr ptr;
	struct uci_context *cont;

	if ((path == NULL) || (option == NULL) || (value == NULL)) {
		ERROR("%s, path or option or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(full_path, (MAX_LEN_VALID_VALUE - 1), "%s.%s=%s", path, option, value);
	if (status <= 0) {
		ERROR("%s sprintf_s failed. full path, option=%s value=%s\n", __func__, option, value);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if ((uci_lookup_ptr(cont, &ptr, full_path, true) != UCI_OK) || (!ptr.s)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (uci_del_list(cont, &ptr) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}

/**************************************************************************/
/*! \fn int uci_converter_del_list_str(enum paramType type, int index, const char param[], const char *value)
 **************************************************************************
 *  \brief delete a string from a UCI element list (using param)
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to remove string from
 *  \param[in] const char *value - string value to remove
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_del_list_str(enum paramType type, int index, const char param[], const char *value)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d", radio_str, rpc_to_uci_index(type, index));
	if (status <= 0)
		return RETURN_ERR;

	status = uci_converter_del_list(path, param, value);
	if (status == RETURN_OK) {
		DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
	} else {
		ERROR("%s failed. index=%d %s=%s\n", __func__, index, param, value);
		return RETURN_ERR;
	}

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_get_list(const char* list, char **output)
 **************************************************************************
 *  \brief get a list of strings from UCI element list (using full path)
 *  \param[in] const char *list - full path of list to get (config file.device name,param)
 *  \param[out] char **output - an array of strings returned
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_list(const char* list, char **output)
{
	unsigned int index = 0;
	struct uci_element *e, *tmp;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((list == NULL) || (output == NULL) || (*output == NULL)) {
		ERROR("%s, list or output or *output is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, list, true) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (!(ptr.o && ptr.option)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if ((ptr.o->type != UCI_TYPE_LIST)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	uci_foreach_element_safe(&ptr.o->v.list, tmp, e) {
		size_t len = strnlen_s(uci_to_option(e)->e.name, MAX_UCI_BUF_LEN - 1);
		strncpy_s((output)[index++], len + 1, uci_to_option(e)->e.name, len);
	}

	_thread_local_uci_put_context(cont);

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_get_list_str(enum paramType type, int index, const char param[], char **output)
 **************************************************************************
 *  \brief get a list of strings from UCI element list (using param)
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name of string list to return
 *  \param[out] char **output - an array of strings returned
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_list_str(enum paramType type, int index,
		const char param[], char **output)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if ((param == NULL) || (output == NULL) || (*output == NULL)) {
		ERROR("%s, param or output or *output is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str, rpc_to_uci_index(type, index), param);
	if (status <= 0)
		return RETURN_ERR;

	return uci_converter_get_list(path, output);
}

/**************************************************************************/
/*! \fn int uci_count_elements_list(const char *list, unsigned int *num_of_elements)
 **************************************************************************
 *  \brief count the number of elements in UCI element list (using full path)
 *  \param[in] const char *list - full path of list to count elements in (config file.device name,param)
 *  \param[out] unsigned int *num_of_elements - return number of elements
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_count_elements_list(const char *list, unsigned int *num_of_elements)
{
	unsigned int count = 0;
	struct uci_element *e, *tmp;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((list == NULL) || (num_of_elements == NULL)) {
		ERROR("%s, list or num_of_elements is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, list, true) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		*num_of_elements = count;
		return RETURN_ERR;
	}

	if (!(ptr.o && ptr.option)) {
		_thread_local_uci_put_context(cont);
		*num_of_elements = count;
		return RETURN_ERR;
	}

	if ((ptr.o->type != UCI_TYPE_LIST)) {
		_thread_local_uci_put_context(cont);
		*num_of_elements = count;
		return RETURN_ERR;
	}

	uci_foreach_element_safe(&ptr.o->v.list, tmp, e) {
		count++;
	}

	_thread_local_uci_put_context(cont);

	*num_of_elements = count;

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_count_elements_list(enum paramType type, int index, const char param[], unsigned int *num_of_elements)
 **************************************************************************
 *  \brief count the number of elements in UCI element list (using param)
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name of string list to count elements in
 *  \param[out] unsigned int *num_of_elements - return number of elements
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_count_elements_list(enum paramType type, int index, const char param[],
		unsigned int *num_of_elements)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if ((param == NULL) || (num_of_elements == NULL)) {
		ERROR("%s, param or num_of_elements is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str, rpc_to_uci_index(type, index), param);
	if (status <= 0)
		return RETURN_ERR;

	return uci_count_elements_list(path, num_of_elements);
}

int uci_converter_reorder(enum paramType type, int index, int pos)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d", radio_str, rpc_to_uci_index(type, index));
	if (status <= 0)
		return RETURN_ERR;

	return uci_converter_reorder_section(path, pos);
}

int uci_converter_reorder_section(const char* path, int pos)
{
	int status;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if ((path == NULL)) {
		ERROR("%s, path NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if ((UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK) || (!ptr.s)) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (uci_reorder_section(cont, ptr.s, pos) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}

/**************************************************************************/
/*! \fn int uci_converter_revert_vaps(const char* path, const char* device)
 **************************************************************************
 *  \brief remove uncommited changes for all vaps of a radio
 *  \param[in] const char* path - file name of the db
 *  \param[in] const char* device - name of the radio
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
static int uci_converter_revert_vaps(const char* path, const char* device)
{
	int wireless_idx;
	struct uci_ptr ptr;
	struct uci_context *ctx;
	struct uci_package *p = NULL;
	struct uci_element *e;
	struct uci_section *s;
	char lookup_path[MAX_UCI_BUF_LEN] = "";
	char vap_type[MAX_UCI_BUF_LEN] = "wifi-iface";

	if ((path == NULL) || (device == NULL)) {
		ERROR("%s, path or device is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	ctx = _thread_local_uci_get_context();
	if (!ctx)
		return RETURN_ERR;

	if ((UCI_CONVERTER_LOOKUP_PTR(ctx, &ptr, lookup_path, path, true) != UCI_OK)) {
		ERROR("%s, uci lookup failed!\n", __FUNCTION__);
		_thread_local_uci_put_context(ctx);
		return RETURN_ERR;
	}

	p = ptr.p;
	uci_foreach_element(&p->sections, e) {
		struct uci_element *n;
		s = uci_to_section(e);

		if (sscanf_s(s->e.name, "default_radio%d", &wireless_idx) != 1)
			continue;

		if (!strncmp(s->type, vap_type, MAX_UCI_BUF_LEN)) {
			uci_foreach_element(&s->options, n) {
				struct uci_option *o = uci_to_option(n);

				if (strncmp(n->name, "device", MAX_UCI_BUF_LEN))
					continue;

				if (o->type != UCI_TYPE_STRING)
					continue;

				if (wireless_idx < 0) {
					_thread_local_uci_put_context(ctx);
					return RETURN_ERR;
				}

				if (!strncmp(device, o->v.string, MAX_UCI_BUF_LEN))
					uci_converter_revert_elem(path, TYPE_VAP, uci_to_rpc_index(TYPE_VAP, wireless_idx), NULL);

				break;
			}
		}
	}
	_thread_local_uci_put_context(ctx);
	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_revert(const char* path)
 **************************************************************************
 *  \brief remove uncommited changes for a giver path
 *  \param[in] const char* path - config<.device><.parameter>
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
static int uci_converter_revert(const char* path)
{
	int status;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if (path == NULL) {
		ERROR("%s, path is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK) {
        ERROR("%s: lookup failed\n", __FUNCTION__);
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	if (uci_revert(cont, &ptr) != UCI_OK) {
        ERROR("%s: revert failed\n", __FUNCTION__);
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_save(cont, ptr.p);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
}

/**************************************************************************/
/*! \fn int uci_converter_revert_elem(const char* config, enum paramType type,
 *          int index, const char* param)
 **************************************************************************
 *  \brief remove uncommited changes using the given path
 *  \param[in] const char* config - name of config file (default "wireless")
 *  \param[in] enum paramType type - radio or vap (if someting else, revert the
 *                                   whole config file
 *  \param[in] int index - index of the device (if less the 0, revert the whole
 *                         config file
 *  \param[in] const char* param - parameter name (if not given, revert the whole device)
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_revert_elem(const char* config, enum paramType type, int index, const char* param)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	char device[MAX_UCI_BUF_LEN] = "";
	const char *radio_str = get_type_str(type);

	if (is_empty_str(config)) {
		strncat_s(path, MAX_UCI_BUF_LEN, "wireless", sizeof("wireless")-1);
	} else {
		strncat_s(path, MAX_UCI_BUF_LEN, config, strnlen_s(config, MAX_UCI_BUF_LEN - 1));
	}

	if (index < 0 || (type != TYPE_RADIO && type != TYPE_VAP && type != TYPE_RADIO_VAP)) {
		goto revert;
	} else {
		status = sprintf_s(device, MAX_UCI_BUF_LEN, "%s%d", radio_str, rpc_to_uci_index(type, index));
		if (status <= 0) {
			ERROR("%s sprintf_s failed: device, radio_str=%s index=%d\n", __func__, radio_str, index);
			return RETURN_ERR;
		}
		strncat_s(path, MAX_UCI_BUF_LEN, ".", 1);
		strncat_s(path, MAX_UCI_BUF_LEN, device, strnlen_s(device, MAX_UCI_BUF_LEN - 1));
	}

	if (!is_empty_str(param)) {
		strncat_s(path, MAX_UCI_BUF_LEN, ".", 1);
		strncat_s(path, MAX_UCI_BUF_LEN, param, strnlen_s(param, MAX_UCI_BUF_LEN - 1));
	}

revert:
	return uci_converter_revert(path);
}

/**************************************************************************/
/*! \fn int uci_converter_revert_radio(const char* path, int index)
 **************************************************************************
 *  \brief remove uncommited changes for the given radio and all its vaps
 *  \param[in] const char* path - name of the config file
 *  \param[in] int index - index of the radio
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_revert_radio(const char* path, int index)
{
	int status;
	char radio[MAX_UCI_BUF_LEN] = "";

	if (path == NULL) {
		ERROR("%s, path is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	uci_converter_revert_elem(path, TYPE_RADIO, index, NULL);

	status = sprintf_s(radio, MAX_UCI_BUF_LEN, "radio%d", rpc_to_uci_index(TYPE_RADIO, index));
	if (status <= 0) {
		ERROR("%s sprintf_s failed: radio, index=%d\n", __func__, index);
		return RETURN_ERR;
	}

	return uci_converter_revert_vaps(path, radio);
}

/**************************************************************************/
/*! \fn int uci_converter_commit(const char* path, const char* alt_dir)
 **************************************************************************
 *  \brief commit all the changes made to the UCI database
 *  \param[in] const char* path - name of the config file (for example wireless)
 *  \param[in] const char* alt_dir - alternate directory (default is /etc/config)
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_commit(const char* path, const char* alt_dir)
{
#ifdef CONFIG_RPCD
	(void) alt_dir;
	return _ubus_uci_converter_commit(path);
#else
	int status;
	struct uci_ptr ptr;
	struct uci_context *cont;
	char lookup_path[MAX_UCI_BUF_LEN] = "";

	if (path == NULL) {
		ERROR("%s, path is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	cont = _thread_local_uci_get_context();
	if (!cont)
		return RETURN_ERR;

	if (alt_dir)
		uci_set_confdir(cont, alt_dir);

	if (UCI_CONVERTER_LOOKUP_PTR(cont, &ptr, lookup_path, path, true) != UCI_OK) {
		_thread_local_uci_put_context(cont);
		return RETURN_ERR;
	}

	status = uci_commit(cont, &ptr.p, false);

	_thread_local_uci_put_context(cont);

	return (status == UCI_OK) ? RETURN_OK : RETURN_ERR;
#endif
}

/**************************************************************************/
/*! \fn int uci_converter_commit_wireless(void)
 **************************************************************************
 *  \brief commit all the changes made to the UCI database in /etc/config/wireless package
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_commit_wireless(void)
{
	return uci_converter_commit("wireless", NULL);
}

/**************************************************************************/
/*! \fn int get_htmode_enum(int index, enum uci_htmode *uci_htmode) {
 **************************************************************************
 *  \brief get htmode for a specific radio from UCI database
 *  \param[in] int index - radio index
 *  \param[out] enum uci_htmode *uci_htmode - output the radio htmode
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int get_htmode_enum(int index, enum uci_htmode *uci_htmode) {
	int status;
	char htmode_str[MAX_UCI_BUF_LEN];

	if (uci_htmode == NULL) {
		ERROR("%s, uci_htmode is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	*uci_htmode = UCI_HTMODE_LAST;

	status = uci_converter_get_str_ext(TYPE_RADIO, index, "htmode", htmode_str, sizeof(htmode_str));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	if (!strncmp(htmode_str, "HT20", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_HT20;
	else if (!strncmp(htmode_str, "HT40+", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_HT40PLUS;
	else if (!strncmp(htmode_str, "HT40-", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_HT40MINUS;
	else if (!strncmp(htmode_str, "VHT20", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_VHT20;
	else if (!strncmp(htmode_str, "VHT40+", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_VHT40PLUS;
	else if (!strncmp(htmode_str, "VHT40-", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_VHT40MINUS;
	else if (!strncmp(htmode_str, "VHT80", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_VHT80;
	else if (!strncmp(htmode_str, "VHT160", MAX_UCI_BUF_LEN))
		*uci_htmode = UCI_HTMODE_VHT160;
	else {
		ERROR("Failed with htmode_str=%s\n", htmode_str);
		return RETURN_ERR;
	}

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int set_htmode_enum(int index, enum uci_htmode uci_htmode) {
 **************************************************************************
 *  \brief set htmode for a specific radio
 *  \param[in] int index - radio index
 *  \param[in] enum uci_htmode uci_htmode - the radio htmode to set
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int set_htmode_enum(int index, enum uci_htmode uci_htmode)
{
	const char *htmode_str = "";

	if (uci_htmode == UCI_HTMODE_HT20)
		htmode_str = "HT20";
	else if (uci_htmode == UCI_HTMODE_HT40PLUS)
		htmode_str = "HT40+";
	else if (uci_htmode == UCI_HTMODE_HT40MINUS)
		htmode_str = "HT40-";
	else if (uci_htmode == UCI_HTMODE_VHT20)
		htmode_str = "VHT20";
	else if (uci_htmode == UCI_HTMODE_VHT40PLUS)
		htmode_str = "VHT40+";
	else if (uci_htmode == UCI_HTMODE_VHT40MINUS)
		htmode_str = "VHT40-";
	else if (uci_htmode == UCI_HTMODE_VHT80)
		htmode_str = "VHT80";
	else if (uci_htmode == UCI_HTMODE_VHT160)
		htmode_str = "VHT160";
	else {
		ERROR("Failed with uci_htmode=%d\n", uci_htmode);
		return RETURN_ERR;
	}

	return uci_converter_set_str(TYPE_RADIO, index, "htmode", htmode_str);
}

/**************************************************************************/
/*! \fn int get_hwmode_enum(int index, enum uci_hwmode *uci_hwmode) {
 **************************************************************************
 *  \brief get hwmode for a specific radio from UCI database
 *  \param[in] int index - radio index
 *  \param[out] enum uci_hwmode *uci_hwmode - output the radio hwmode
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int get_hwmode_enum(int index, enum uci_hwmode *uci_hwmode) {
	int status;
	char band[MAX_UCI_BUF_LEN];
	char hwmode_str[MAX_UCI_BUF_LEN];

	if (uci_hwmode == NULL) {
		ERROR("%s, uci_hwmode is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	*uci_hwmode = UCI_HWMODE_LAST;

	status = uci_converter_get_str_ext(TYPE_RADIO, index, "hwmode", hwmode_str, sizeof(hwmode_str));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = uci_converter_get_str_ext(TYPE_RADIO, index, "band", band, sizeof(band));
	if (status == RETURN_ERR)
		return RETURN_ERR;

	if (!strncmp(hwmode_str, "11b", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11B;
	else if (!strncmp(hwmode_str, "11bg", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11BG;
	else if (!strncmp(hwmode_str, "11g", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11G;
	else if (!strncmp(hwmode_str, "11ng", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11NG;
	else if (!strncmp(hwmode_str, "11bgn", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11BGN;
	else if (!strncmp(hwmode_str, "11bgnax", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11BGNAX;
	else if (!strncmp(hwmode_str, "11n", MAX_UCI_BUF_LEN) && !strncmp(band, "2.4GHz", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11N_24G;
	else if (!strncmp(hwmode_str, "11n", MAX_UCI_BUF_LEN) && !strncmp(band, "5GHz", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11N_5G;
	else if (!strncmp(hwmode_str, "11a", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11A;
	else if (!strncmp(hwmode_str, "11an", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11AN;
	else if (!strncmp(hwmode_str, "11nac", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11NAC;
	else if (!strncmp(hwmode_str, "11ac", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11AC;
	else if (!strncmp(hwmode_str, "11anac", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11ANAC;
	else if (!strncmp(hwmode_str, "11anacax", MAX_UCI_BUF_LEN))
		*uci_hwmode = UCI_HWMODE_11ANACAX;
	else {
		ERROR("Failed with hwmode_str=%s, band=%s\n", hwmode_str, band);
		return RETURN_ERR;
	}

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int set_hwmode_enum(int index, enum uci_hwmode uci_hwmode) {
 **************************************************************************
 *  \brief set hwmode for a specific radio
 *  \param[in] int index - radio index
 *  \param[in] enum uci_hwmode uci_hwmode - the radio hwmode to set
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int set_hwmode_enum(int index, enum uci_hwmode uci_hwmode)
{
	const char *hwmode_str = "";

	if (uci_hwmode == UCI_HWMODE_11B)
		hwmode_str = "11b";
	else if (uci_hwmode == UCI_HWMODE_11BG)
		hwmode_str = "11bg";
	else if (uci_hwmode == UCI_HWMODE_11G)
		hwmode_str = "11g";
	else if (uci_hwmode == UCI_HWMODE_11NG)
		hwmode_str = "11ng";
	else if (uci_hwmode == UCI_HWMODE_11BGN)
		hwmode_str = "11bgn";
	else if (uci_hwmode == UCI_HWMODE_11BGNAX)
		hwmode_str = "11bgnax";
	else if (uci_hwmode == UCI_HWMODE_11N_24G || uci_hwmode == UCI_HWMODE_11N_5G)
		hwmode_str = "11n";
	else if (uci_hwmode == UCI_HWMODE_11A)
		hwmode_str = "11a";
	else if (uci_hwmode == UCI_HWMODE_11AN)
		hwmode_str = "11an";
	else if (uci_hwmode == UCI_HWMODE_11NAC)
		hwmode_str = "11nac";
	else if (uci_hwmode == UCI_HWMODE_11AC)
		hwmode_str = "11ac";
	else if (uci_hwmode == UCI_HWMODE_11ANAC)
		hwmode_str = "11anac";
	else if (uci_hwmode == UCI_HWMODE_11ANACAX)
		hwmode_str = "11anacax";
	else {
		ERROR("Failed with uci_hwmode=%d\n", uci_hwmode);
		return RETURN_ERR;
	}

	return uci_converter_set_str(TYPE_RADIO, index, "hwmode", hwmode_str);
}

/**************************************************************************/
/*! \fn void uci_converter_prepare_for_up(int radio_idx)
 **************************************************************************
 *  \brief prepare uci db to be precessed by netifd
 *  \param[in] int radio_idx - the index of the radio to prepare
 ***************************************************************************/
void uci_converter_prepare_for_up(int radio_idx)
{
	UNUSED_PARAM(radio_idx);
	if (uci_converter_commit_wireless() != RETURN_OK)
		ERROR("%s uci_converter_commit_wireless failed\n", __FUNCTION__);
}

enum {
	HAL_RADIO,
	HAL_CHANGE_STATUS,
	__HAL_MAX
};

static const struct blobmsg_policy confirmation_policy[] = {
	[HAL_RADIO] = { .name = "radio", .type = BLOBMSG_TYPE_STRING },
	[HAL_CHANGE_STATUS] = { .name = "status", .type = BLOBMSG_TYPE_INT32 },
};

static void radio_cb_timeout(struct uloop_timeout *t)
{
	(void)t;
	DEBUG("%s: TIMEOUT received", __FUNCTION__);
	uloop_end();
}

static int confirmation_func(struct ubus_context *ctx, struct ubus_object *obj,
              struct ubus_request_data *req, const char *method,
              struct blob_attr *msg)
{
	static struct blob_buf resp_buf;
	struct blob_attr *hb[__HAL_MAX];
	char *radio;
	unsigned int radio_index;
	(void)obj;
	(void)method;

	blobmsg_parse(confirmation_policy, __HAL_MAX, hb, blob_data(msg), blob_len(msg));
	if (!hb[HAL_RADIO] || !hb[HAL_CHANGE_STATUS]) {
		ERROR("%s: UBUS_STATUS_INVALID_ARGUMENT", __FUNCTION__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	radio = blobmsg_get_string(hb[HAL_RADIO]);

	if (!radio) {
		ERROR("%s: UBUS_STATUS_UNKNOWN_ERROR", __FUNCTION__);
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	blob_buf_init(&resp_buf, 0);
	ubus_send_reply(ctx, req, resp_buf.head);

	if (sscanf_s(radio, "radio%d", (int*)&radio_index) != 1) {
		ERROR("%s: Invalid radio index\n", __FUNCTION__);
		uloop_end();
		return 0;
	}

	uloop_end();
	return 0;
}

static const struct ubus_method hal_methods[] = {
	UBUS_METHOD("confirmation", confirmation_func, confirmation_policy),
};

static struct ubus_object_type hal_object_type =
	UBUS_OBJECT_TYPE("hal", hal_methods);

static struct ubus_object hal_object = {
	.name = "hal",
	.type = &hal_object_type,
	.methods = hal_methods,
	.n_methods = ARRAY_SIZE(hal_methods),
};

static void uci_converter_run_uloop(void)
{
	int ret;
	const char *ubus_socket = NULL;
	static struct ubus_context *ctx = NULL;
	struct uloop_timeout resp_timeout;
	resp_timeout.pending = false;
	resp_timeout.cb = radio_cb_timeout;

	uloop_init();
	signal(SIGPIPE, SIG_IGN);

	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		ERROR("%s: Failed to connect to ubus\n", __FUNCTION__);
		return;
	}

	ubus_add_uloop(ctx);
	ret = ubus_add_object(ctx, &hal_object);
	if (ret) {
		ERROR("%s: Failed to add object: %s\n", __FUNCTION__, ubus_strerror(ret));
		return;
	}

	uloop_timeout_set(&resp_timeout, NETIFD_RESP_TIMEOUT);
	uloop_run();

	ubus_remove_object(ctx, &hal_object);
	ubus_free(ctx);
	uloop_done();
	ctx = NULL;
}

static int uci_converter_start_uloop(struct ubus_context **ctx)
{
	int ret = RETURN_OK;
	const char *ubus_socket = NULL;

	if (ctx == NULL)
		return RETURN_ERR;

	uloop_init();
	signal(SIGPIPE, SIG_IGN);

	*ctx = ubus_connect(ubus_socket);
	if (!*ctx) {
		ERROR("%s: Failed to connect to ubus\n", __FUNCTION__);
        ret = RETURN_ERR;
	} else {
		ubus_add_uloop(*ctx);
		ret = ubus_add_object(*ctx, &hal_object);
		if (ret)
			ERROR("%s: Failed to add object: %s\n", __FUNCTION__, ubus_strerror(ret));
	}

    return ret;
}

static void uci_converter_clean_uloop(struct ubus_context **ctx)
{
	if (ctx == NULL)
		return;

	ubus_remove_object(*ctx, &hal_object);
	ubus_free(*ctx);
	uloop_done();
	*ctx = NULL;
}

static void uci_converter_finish_uloop(struct ubus_context **ctx)
{
	struct uloop_timeout resp_timeout;
	resp_timeout.pending = false;
	resp_timeout.cb = radio_cb_timeout;

	uloop_timeout_set(&resp_timeout, NETIFD_RESP_TIMEOUT);
	uloop_run();

	uci_converter_clean_uloop(ctx);
}

int uci_converter_netifd_connect(int radio_idx, bool initNetifd, char* command)
{
	static struct ubus_context *ctx = NULL;
	int ret = uci_converter_start_uloop(&ctx);

	uci_converter_prepare_for_up(radio_idx);

	if (uci_converter_system(command) == RETURN_OK) {
		DEBUG("%s completed successfully\n", __FUNCTION__);
	} else {
		ERROR("%s failed\n", __FUNCTION__);
		uci_converter_clean_uloop(&ctx);
		return RETURN_ERR;
	}

	if (!ret) {
		uci_converter_finish_uloop(&ctx);
	}

	/* on init run uloop twice*/
	if (initNetifd)
		uci_converter_run_uloop();

	return RETURN_OK;
}

//UCI HELPER ENDS

