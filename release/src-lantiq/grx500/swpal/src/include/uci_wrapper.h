#ifndef __UCI_WRAPPER_API_H_
#define __UCI_WRAPPER_API_H_

#ifndef CONFIG_RPCD
#include <puma_safe_libc.h>
#else
#include <stddef.h>
#include "libsafec/safe_str_lib.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef u_int_32
#define u_int_32 unsigned int
#endif

#ifndef _cplusplus
#include <stdbool.h>
#endif

#define ERROR_TOKEN ..
#define SDL_BANNED_FUNCTION _Pragma("GCC error \"this function is banned by the SDL and should not be used. use one of the approved functions instead\"")

#define FILE_SIZE 1024
#define MAX_LEN_PARAM_VALUE 128
#define MAX_LEN_VALID_VALUE 1024
#define MAX_UCI_BUF_LEN 64
#define MAC_LENGTH 17
#define ATF_STA_GRANTS_LEN 22

#define MAX_NUM_OF_RADIOS 3
#define MAX_VAPS_PER_RADIO 32
#define VAP_RPC_IDX_OFFSET 10
#define MAC_ADDR_STR_LEN 18
#define MAX_RPC_VAP_IDX VAP_RPC_IDX_OFFSET + MAX_NUM_OF_RADIOS * MAX_VAPS_PER_RADIO
#define MAX_RDKB_VAP_IDX MAX_NUM_OF_RADIOS + MAX_VAPS_PER_RADIO * MAX_NUM_OF_RADIOS

#define RETURN_ERR_NOT_FOUND -2
#define RETURN_ERR -1
#define RETURN_OK 0

/*Log-Helper-Functions defined in liblishelper*/
#define CRIT(fmt, args...) LOGF_LOG_CRITICAL(fmt, ##args)
#define ERROR(fmt, args...) LOGF_LOG_ERROR(fmt, ##args)
#define WARN(fmt, args...) LOGF_LOG_WARNING(fmt, ##args)
#define INFO(fmt, args...) LOGF_LOG_INFO(fmt, ##args)
#define DEBUG(fmt, args...) LOGF_LOG_DEBUG(fmt, ##args)
/*END Log-Helper-Functions*/


/* use only even phy numbers since odd phy's are used for station interfaces */
#define RADIO_INDEX_SKIP 2

enum paramType
{
        TYPE_RADIO = 0,
        TYPE_RADIO_VAP,
        TYPE_VAP
};

enum uci_hwmode {
        UCI_HWMODE_11B = 0,
        UCI_HWMODE_11G,
        UCI_HWMODE_11BG,
        UCI_HWMODE_11NG,
        UCI_HWMODE_11BGN,
        UCI_HWMODE_11BGNAX,
        UCI_HWMODE_11N_24G,
        UCI_HWMODE_11N_5G,
        UCI_HWMODE_11A,
        UCI_HWMODE_11AN,
        UCI_HWMODE_11NAC,
        UCI_HWMODE_11AC,
        UCI_HWMODE_11ANAC,
        UCI_HWMODE_11ANACAX,

        UCI_HWMODE_LAST /* Keep last */
};

enum uci_htmode {
        UCI_HTMODE_HT20 = 0,
        UCI_HTMODE_HT40PLUS,
        UCI_HTMODE_HT40MINUS,
        UCI_HTMODE_VHT20,
        UCI_HTMODE_VHT40PLUS,
        UCI_HTMODE_VHT40MINUS,
        UCI_HTMODE_VHT80,
        UCI_HTMODE_VHT160,

        UCI_HTMODE_LAST /* Keep last */
};

static inline bool is_empty_str(const char *str)
{
	return (!str || !str[0]);
}


//UCI HELPER APIS
void set_uci_converter_fun(int (*callback)(char* path, const char* option, const char* value)); // __attribute__((deprecated));

int uci_converter_alloc_local_uci_context(void);
void uci_converter_free_local_uci_context(void);
int rpc_to_uci_index(enum paramType iftype, int index);
int uci_to_rpc_index(enum paramType iftype, int index);
bool uci_converter_is_dummy(int uci_index);
int uci_getIndexFromInterface(const char *interfaceName, int *rpc_index);
int uci_getIndexFromBssid(const char *bssid, int *rpc_index);
int uci_converter_system_print(const char *cmd);
int uci_converter_system(char *cmd);
int uci_converter_popen(char *cmd, char *output, int outputSize);
int uci_converter_get(const char* path, char* value, size_t length);
int uci_converter_get_str(enum paramType type, int index, const char param[], char *value); // __attribute__((deprecated));
int uci_converter_get_str_ext(enum paramType type, int index, const char param[], char *value, size_t size);
int uci_converter_get_str_by_uci_index(enum paramType type, int uciIndex, const char param[], char *value, size_t size);
void uci_converter_get_optional_str(enum paramType type, int index, const char param[], char *value, size_t size, char* default_val);
int uci_converter_get_int(enum paramType type, int index, const char param[], int *value);
void uci_converter_get_optional_int(enum paramType type, int index, const char param[], int *value, int default_val);
int uci_converter_get_uint(enum paramType type, int index, const char param[], unsigned int *value);
void uci_converter_get_optional_uint(enum paramType type, int index, const char param[], unsigned int *value, unsigned int default_val);
int uci_converter_get_ulong(enum paramType type, int index, const char param[], unsigned long *value);
void uci_converter_get_optional_ulong(enum paramType type, int index, const char param[], unsigned long *value, unsigned long default_val);
int uci_converter_get_ushort(enum paramType type, int index, const char param[], unsigned short *value);
void uci_converter_get_optional_ushort(enum paramType type, int index, const char param[], unsigned short *value, unsigned short default_val);
int uci_converter_get_bool(enum paramType type, int index, const char param[], bool *value);
void uci_converter_get_optional_bool(enum paramType type, int index, const char param[], bool *value, bool default_val);
int uci_converter_set(const char* path, const char* option, const char* value);
int uci_converter_add_device(const char* config_file, const char* device_name, const char* device_type);
int uci_converter_set_str(enum paramType type, int index, const char param[], const char *value);
int uci_converter_set_int(enum paramType type, int index, const char param[], int value);
int uci_converter_set_uint(enum paramType type, int index, const char param[], unsigned int value);
int uci_converter_set_ulong(enum paramType type, int index, const char param[], unsigned long value);
int uci_converter_set_ushort(enum paramType type, int index, const char param[], unsigned short value);
int uci_converter_set_bool(enum paramType type, int index, const char param[], bool value);
int uci_converter_del_elem(const char* path);
int uci_converter_del(enum paramType type, int index, const char* param);
int uci_converter_add_list(const char* path, const char* option, const char* value);
int uci_converter_add_list_str(enum paramType type, int index, const char param[], const char *value);
int uci_converter_del_list(const char* path, const char* option, const char* value);
int uci_converter_del_list_str(enum paramType type, int index, const char param[], const char *value);
int uci_converter_get_list(const char* list, char **output);
int uci_converter_get_list_str(enum paramType type, int index, const char param[], char **output);
int uci_converter_reorder(enum paramType type, int index, int pos);
int uci_converter_reorder_section(const char* path, int pos);
int uci_count_elements_list(const char *list, unsigned int *num_of_elements);
int uci_converter_count_elements_list(enum paramType type, int index, const char param[],
                                             unsigned int *num_of_elements);
int uci_converter_count_elems(const char *sec_type, const char *sec_name,
                              const char* opt_name, const char* opt_val,
                              int *num);
int uci_converter_count_sta_elems(const char *sec_type, const char *sec_name,
                              const char* opt_name, const char* opt_val,
                              int *num);
int uci_get_existing_interfaces(int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound);
int uci_get_existing_interfaces_in_radio(const char *radio, int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound);
int uci_get_existing_sta_interfaces(int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound);
int uci_get_existing_sta_interfaces_in_radio(const char *radio, int *ifacesArr, const unsigned int ifacesArrSize, unsigned int *numOfIfFound);
int uci_get_param_change(enum paramType ifType, int index, const char *paramName, char* paramChange, size_t size);
int uci_converter_revert_elem(const char* config, enum paramType type, int index, const char* param);
int uci_converter_revert_radio(const char* path, int index);
int uci_converter_commit(const char* path, const char* alt_dir);
int uci_converter_commit_wireless(void);
int set_htmode_enum(int index, enum uci_htmode uci_htmode);
int get_htmode_enum(int index, enum uci_htmode *uci_htmode);
int wlan_encryption_set(int index);
int set_hwmode_enum(int index, enum uci_hwmode uci_hwmode);
int get_hwmode_enum(int index, enum uci_hwmode *uci_hwmode);
void uci_converter_prepare_for_up(int radio_idx);
int uci_converter_netifd_connect(int radio_idx, bool initNetifd, char* command);
//UCI HELPER ENDS
#endif //__UCI_WRAPPER_API_H_
