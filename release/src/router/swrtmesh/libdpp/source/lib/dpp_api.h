/*
 * dpp_api.h - libdpp library header.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#ifndef DPP_API_H
#define DPP_API_H

#include "timer.h"

#ifndef foreach_token
#define foreach_token(t, tokens, delim)	\
	for ((t = strtok(tokens, delim)); t; (t = strtok(NULL, delim)))

#endif /* foreach_token */


#ifndef SHA256_MAC_LEN
#define SHA256_MAC_LEN	32
#endif

#ifndef PMKID_LEN
#define PMKID_LEN	32
#endif

#define WLAN_ACTION_PUBLIC 4
#define WLAN_PA_VENDOR_SPECIFIC 9
#define WLAN_PA_GAS_INITIAL_REQ 10
#define WLAN_PA_GAS_INITIAL_RESP 11
#define WLAN_PA_GAS_COMEBACK_REQ 12
#define WLAN_PA_GAS_COMEBACK_RESP 13


#define WLAN_EID_ADV_PROTO 108
#define WLAN_EID_VENDOR_SPECIFIC 221

#define OUI_WFA 0x506f9a

#define DPP_OUI_TYPE		0x1A
#define DPP_VERSION		2
#define DPP_BOOTSTRAP_QR_CODE	0
#define DPP_HDR_LEN		(4 + 2) /* OUI, OUI Type, Crypto Suite, DPP frame type */
#define WLAN_DPP_HDR_LEN	2 + DPP_HDR_LEN /* WLAN PA, WLAN PA-vendor-spec */

#define WLAN_STATUS_SUCCESS 0

#define DPP_BOOTSTRAP_MAX_FREQ 30

enum dpp_event {
	/* in events */
	DPP_EVENT_BOOTSTRAPPING = 0,
	DPP_EVENT_TRIGGER,
	DPP_EVENT_TIMEOUT,
	DPP_EVENT_RX_FRAME,

	/* out events */
	DPP_EVENT_NO_PEER,
	DPP_EVENT_BAD_AUTH,
	DPP_EVENT_TX_FRAME,
	DPP_EVENT_AUTHENTICATED,
	DPP_EVENT_PROVISIONED,
	DPP_EVENT_PROVISIONED_FAIL,
};

enum dpp_sm_state {
	DPP_INITIATOR_STATE_NOTHING,
	DPP_INITIATOR_STATE_BOOTSTRAPPED,
	DPP_INITIATOR_STATE_AUTHENTICATING,
	DPP_INITIATOR_STATE_AUTHENTICATED,
	DPP_INITIATOR_STATE_FINISHED,
	DPP_INITIATOR_STATE_TERMINATED,

	DPP_RESPONDER_STATE_AWAITING = 6,
	DPP_RESPONDER_STATE_BS_ACQUIRING,
	DPP_RESPONDER_STATE_AUTHENTICATING,
	DPP_RESPONDER_STATE_AUTHENTICATED,
	DPP_RESPONDER_STATE_PROVISIONING,
	DPP_RESPONDER_STATE_PROVISIONED,
	DPP_RESPONDER_STATE_TERMINATED,

	DPP_NUM_STATES,
};


enum dpp_public_action_frame_type {
	DPP_PA_AUTHENTICATION_REQ = 0,
	DPP_PA_AUTHENTICATION_RESP = 1,
	DPP_PA_AUTHENTICATION_CONF = 2,
	DPP_PA_PEER_DISCOVERY_REQ = 5,
	DPP_PA_PEER_DISCOVERY_RESP = 6,
	DPP_PA_PKEX_V1_EXCHANGE_REQ = 7,
	DPP_PA_PKEX_EXCHANGE_RESP = 8,
	DPP_PA_PKEX_COMMIT_REVEAL_REQ = 9,
	DPP_PA_PKEX_COMMIT_REVEAL_RESP = 10,
	DPP_PA_CONFIGURATION_RESULT = 11,
	DPP_PA_CONNECTION_STATUS_RESULT = 12,
	DPP_PA_PRESENCE_ANNOUNCEMENT = 13,
	DPP_PA_RECONFIG_ANNOUNCEMENT = 14,
	DPP_PA_RECONFIG_AUTH_REQ = 15,
	DPP_PA_RECONFIG_AUTH_RESP = 16,
	DPP_PA_RECONFIG_AUTH_CONF = 17,
	DPP_PA_PKEX_EXCHANGE_REQ = 18,
	DPP_PA_PB_PRESENCE_ANNOUNCEMENT = 19,
	DPP_PA_PB_PRESENCE_ANNOUNCEMENT_RESP = 20,
	DPP_PA_PRIV_PEER_INTRO_QUERY = 21,
	DPP_PA_PRIV_PEER_INTRO_NOTIFY = 22,
	DPP_PA_PRIV_PEER_INTRO_UPDATE = 23,
};

enum dpp_attribute_id {
	DPP_ATTR_STATUS = 0x1000,
	DPP_ATTR_I_BOOTSTRAP_KEY_HASH = 0x1001,
	DPP_ATTR_R_BOOTSTRAP_KEY_HASH = 0x1002,
	DPP_ATTR_I_PROTOCOL_KEY = 0x1003,
	DPP_ATTR_WRAPPED_DATA = 0x1004,
	DPP_ATTR_I_NONCE = 0x1005,
	DPP_ATTR_I_CAPABILITIES = 0x1006,
	DPP_ATTR_R_NONCE = 0x1007,
	DPP_ATTR_R_CAPABILITIES = 0x1008,
	DPP_ATTR_R_PROTOCOL_KEY = 0x1009,
	DPP_ATTR_I_AUTH_TAG = 0x100A,
	DPP_ATTR_R_AUTH_TAG = 0x100B,
	DPP_ATTR_CONFIG_OBJ = 0x100C,
	DPP_ATTR_CONNECTOR = 0x100D,
	DPP_ATTR_CONFIG_ATTR_OBJ = 0x100E,
	DPP_ATTR_BOOTSTRAP_KEY = 0x100F,
	DPP_ATTR_OWN_NET_NK_HASH = 0x1011,
	DPP_ATTR_FINITE_CYCLIC_GROUP = 0x1012,
	DPP_ATTR_ENCRYPTED_KEY = 0x1013,
	DPP_ATTR_ENROLLEE_NONCE = 0x1014,
	DPP_ATTR_CODE_IDENTIFIER = 0x1015,
	DPP_ATTR_TRANSACTION_ID = 0x1016,
	DPP_ATTR_BOOTSTRAP_INFO = 0x1017,
	DPP_ATTR_CHANNEL = 0x1018,
	DPP_ATTR_PROTOCOL_VERSION = 0x1019,
	DPP_ATTR_ENVELOPED_DATA = 0x101A,
	DPP_ATTR_SEND_CONN_STATUS = 0x101B,
	DPP_ATTR_CONN_STATUS = 0x101C,
	DPP_ATTR_RECONFIG_FLAGS = 0x101D,
	DPP_ATTR_C_SIGN_KEY_HASH = 0x101E,
	DPP_ATTR_CSR_ATTR_REQ = 0x101F,
	DPP_ATTR_A_NONCE = 0x1020,
	DPP_ATTR_E_PRIME_ID = 0x1021,
	DPP_ATTR_CONFIGURATOR_NONCE = 0x1022,
};

enum dpp_status {
	DPP_STATUS_OK = 0,
	DPP_STATUS_NOT_COMPATIBLE = 1,
	DPP_STATUS_AUTH_FAILURE = 2,
	DPP_STATUS_BAD_CODE = 3,
	DPP_STATUS_UNWRAP_FAILURE = 3,
	DPP_STATUS_BAD_GROUP = 4,
	DPP_STATUS_CONFIGURE_FAILURE = 5,
	DPP_STATUS_RESPONSE_PENDING = 6,
	DPP_STATUS_INVALID_CONNECTOR = 7,
	DPP_STATUS_NO_MATCH = 8,
	DPP_STATUS_CONFIG_REJECTED = 9,
	DPP_STATUS_NO_AP = 10,
	DPP_STATUS_CONFIGURE_PENDING = 11,
	DPP_STATUS_CSR_NEEDED = 12,
	DPP_STATUS_CSR_BAD = 13,
	DPP_STATUS_NEW_KEY_NEEDED = 14,
};

enum dpp_akm {
	DPP_AKM_UNKNOWN,
	DPP_AKM_DPP,
	DPP_AKM_PSK,
	DPP_AKM_SAE,
	DPP_AKM_PSK_SAE,
	DPP_AKM_SAE_DPP,
	DPP_AKM_PSK_SAE_DPP,
	DPP_AKM_DOT1X,
};

enum dpp_netrole {
        DPP_NETROLE_STA,
        DPP_NETROLE_AP,
        DPP_NETROLE_CONFIGURATOR,
//#ifdef CONFIG_EASYMESH_DPP
        DPP_NETROLE_MAPAGENT,
        DPP_NETROLE_MAP_BH_STA,
        DPP_NETROLE_MAP_BH_BSS,
        DPP_NETROLE_MAPCONTROLLER,
//#endif
	DPP_NETROLE_INVALID,

};

//#ifdef CONFIG_EASYMESH_DPP
enum dpp_config_obj_type {
	DPP_CONFIG_TYPE_BSTA = 0,
	DPP_CONFIG_TYPE_1905 = 1,
};
//#endif

enum dpp_connector_key {
        DPP_CONFIG_REUSEKEY = 0,
        DPP_CONFIG_REPLACEKEY = 1,
};

struct dpp_attribute {
	uint16_t type;
	uint16_t len;
	uint8_t data[];
} __attribute__ ((packed));

struct dpp_action_frame {
	uint8_t oui[3];
	uint8_t oui_type;
	uint8_t cipher;
	uint8_t frame_type;
	struct dpp_attribute attr[];
} __attribute__ ((packed));

enum gas_query_ap_result {
        GAS_QUERY_AP_SUCCESS,
        GAS_QUERY_AP_FAILURE,
        GAS_QUERY_AP_TIMEOUT,
        GAS_QUERY_AP_PEER_ERROR,
        GAS_QUERY_AP_INTERNAL_ERROR,
        GAS_QUERY_AP_DELETED_AT_DEINIT
};


#define DPP_MAX_NONCE_LEN	32
#define DPP_NONCE_LEN		16

#define DPP_MAX_HASH_LEN		64
#define DPP_MAX_SHARED_SECRET_LEN	66

#define DPP_CAPAB_ENROLLEE BIT(0)
#define DPP_CAPAB_CONFIGURATOR BIT(1)
#define DPP_CAPAB_ROLE_MASK (BIT(0) | BIT(1))

struct dpp_curve_params {
	const char *name;
	size_t hash_len;
	size_t aes_siv_keylen;
	size_t nonce_len;
	size_t prime_len;
	uint16_t ike_grp;
};


struct dpp_bootstrap_info {
	char *uri;
	int type;
	unsigned int id;	/* TODO: set id = pubkey_hash*/
	uint16_t curve_ike_grp;
	uint8_t macaddr[6];
#define NUM_OPCLASS_CHANNEL_PAIR	4
	uint8_t opclass_channel[2 * NUM_OPCLASS_CHANNEL_PAIR];
	uint8_t num_channel;
	uint8_t version;
	uint8_t supported_curves;
	char *info;
	unsigned int port;
	int own;

	void *key;

	/* DER of ASN.1 SubjectPublicKeyInfo */
	uint8_t *pubkey;
	size_t pubkeylen;

	/* pk = base64(pubkey) = key part in URI */
	char *pk;

	uint8_t pubkey_hash[SHA256_MAC_LEN];        /* SHA256(pubkey) */
	uint8_t pubkey_hash_chirp[SHA256_MAC_LEN];  /* SHA256("chirp"|pubkey) */

	char *configurator_params;
	struct list_head list;
};


struct dpp_configuration {
	char *ssid;
	size_t ssid_len;
	enum dpp_akm akm;
	enum dpp_netrole netrole;

	enum wifi_band band;

	/* For legacy configuration */
	char *passphrase;
	size_t pass_len;
	struct list_head list;
};

struct dpp_config {
	uint8_t ssid[32];
	size_t ssid_len;
	int ssid_charset;
	enum dpp_akm akm;
	enum dpp_netrole netrole;

	/* For DPP configuration (connector) */
	time_t netaccesskey_expiry;

	/* TODO: groups */
	char *group_id;

	/* For legacy configuration */
	char *passphrase;
	uint8_t psk[32];
	int psk_set;

	char *csrattrs;
	char *extra_name;
	char *extra_value;
};

struct dpp_configurator {
	struct list_head list;
	unsigned int id;
	int own;
	void *csign_key;
	uint8_t kid_hash[SHA256_MAC_LEN];
	char *kid;
	uint16_t curve_ike_grp;
	//const struct dpp_curve_params *curve;
	const struct dpp_curve_params *net_access_key_curve;
	char *connector;	/* own Connector for reconfiguration */
	void *connector_key;
	void *pp_key;
};


struct dpp_signed_connector_info {
	unsigned char *payload;
	size_t payload_len;
};

struct dpp_config_obj {
	char *connector;	/* signed Connector */
	enum dpp_netrole netrole;

	uint8_t ssid[33];
	uint8_t ssid_len;
	int ssid_charset;
	char passphrase[64];
	uint8_t psk[32];
	int psk_set;
	enum dpp_akm akm;

	size_t csign_len;
	uint8_t *c_sign_key;

	size_t certbag_len;
	uint8_t *certbag;

	size_t certs_len;
	uint8_t *certs;

	size_t cacert_len;
	uint8_t *cacert;

	char *server_name;
	int pp_key_len;
	uint8_t *pp_key;

	struct list_head list;
};

struct dpp_conf_req {
	char tech[9];
	char dpp_name[32];
	size_t name_len;
	char *extra_name;
	char *extra_value;
	enum dpp_netrole netrole;

#define DPP_CONF_REQ_BSTA_MAX 3
	int num_bsta;
	struct bsta_list {
		enum dpp_netrole l2_netrole;
		enum dpp_akm akm;
		char *chan_list;
	} blist[DPP_CONF_REQ_BSTA_MAX];
};


/* Created for every new dpp enrollment attempt */
struct dpp_peer {
	struct list_head list;
	void *ctx;
	void *priv;	/* opaque private data */
	void *sm;
	bool initiator;
	uint8_t state;
	uint8_t role;		/* configurator or enrollee or both */
	uint8_t peer_version;
	const struct dpp_curve_params *curve;

	/* curve */
	size_t curve_hash_len;
	size_t curve_nonce_len;
	size_t curve_prime_len;
	uint16_t curve_ike_grp;

	uint8_t peer_macaddr[6];
	struct dpp_bootstrap_info *peer_bi;
	struct dpp_bootstrap_info *own_bi;
	struct dpp_bootstrap_info *tmp_own_bi;
	struct dpp_bootstrap_info *tmp_peer_bi;
	uint8_t i_nonce[DPP_NONCE_LEN];
	uint8_t r_nonce[DPP_NONCE_LEN];
	uint8_t e_nonce[DPP_NONCE_LEN];
	int i_nonce_len;
	int r_nonce_len;
	int e_nonce_len;

        uint8_t waiting_pubkey_hash[SHA256_MAC_LEN];
	int response_pending;

        unsigned int curr_freq;

	uint8_t *Mx;
	size_t Mx_len;

	uint8_t *Nx;
	size_t Nx_len;

	uint8_t *Lx;
	size_t Lx_len;

	uint8_t k1[DPP_MAX_HASH_LEN];
	uint8_t k2[DPP_MAX_HASH_LEN];
	uint8_t ke[DPP_MAX_HASH_LEN];
	uint8_t bk[DPP_MAX_HASH_LEN];

	uint8_t i_capab;
	uint8_t r_capab;

	void *own_protocol_key;
	void *peer_protocol_key;
	void *reconfig_old_protocol_key;

	uint8_t *auth_request;
	size_t auth_request_len;

	uint8_t *auth_response;
	size_t auth_response_len;

	uint8_t *auth_confirm;
	size_t auth_confirm_len;

	uint8_t *config_request;
	size_t config_request_len;

	uint8_t *config_response;
	size_t config_response_len;

	struct dpp_configurator *conf;
	int configurator_set;

        int provision_configurator;

	char *e_mud_url;
	int *e_band_support;
//	enum dpp_netrole e_netrole;
//	enum dpp_netrole e_l2netrole;
//	enum dpp_akm e_akm;

	struct dpp_conf_req e_req;

	char *e_name;

	int akm_use_selector;

	bool use_config_query;
	bool waiting_config;

	int auth_success;
	int response_status;
	int waiting_auth_resp;		//TODO: remove
	int waiting_auth_confirm;	//TODO: remove
	int send_conn_status;
	int reconfig;

	enum dpp_status auth_status;
	enum dpp_status conf_status;
	enum dpp_status force_conf_status;

	uint8_t allowed_roles;
	int configurator;

	uint32_t num_conf_obj;
	struct list_head conf_objlist;	/* list of struct dpp_config_obj */

	uint8_t *net_access_key;
	int net_access_key_len;


        enum dpp_connector_key reconfig_connector_key;


	int conn_status_requested;

	uint8_t *csrattrs;
        size_t csrattrs_len;
        int connect_on_tx_status;
	uint8_t *map_peer_pub_key;
	size_t map_peer_pub_key_len;
};

struct dpp_context {
	void *ctx;
	int num_peers;
	int version;
	bool initiator;
	char *keyfile;
	uint32_t role;
	struct list_head peerlist;		/* list of struct dpp_peer */
	struct list_head configuratorlist;	/* list of struct dpp_configurator */
	struct list_head own_bilist;
	struct list_head peer_bilist;
	struct list_head conflist;		/* list of struct dpp_config */

	void *priv;

	/* output function from dpp s/mc that callers can override */
	int (*cb)(void *, uint8_t *smac, enum dpp_event e, uint8_t *data, size_t len);
};


int dpp_attr_ok(struct dpp_attribute *a, int rem);
struct dpp_attribute *dpp_attr_next(struct dpp_attribute *a, int *rem);

#define dpp_attr_for_each(attr, attrs, rem)			\
	for (attr = (struct dpp_attribute *)attrs;		\
	     dpp_attr_ok(attr, rem);				\
	     attr = dpp_attr_next(attr, &rem))


int dpp_init(void **ctx, int argc, char *argv[]);

int dpp_del_configuration(struct dpp_context *ctx);
int dpp_set_configuration(struct dpp_context *ctx, enum dpp_netrole netrole,
			  enum dpp_akm akm, char *ssid, char *pass,
			  enum wifi_band band);

struct dpp_peer *dpp_lookup_peer(struct dpp_context *ctx, uint8_t *macaddr);
void dpp_free_peer(struct dpp_context *ctx, struct dpp_peer *peer);

struct dpp_peer *dpp_create_initiator_instance(struct dpp_context *ctx);
struct dpp_peer *dpp_create_responder_instance(struct dpp_context *ctx);

int dpp_set_peer_private_data(void *ctx, uint8_t *peer_macaddr, void *priv);
void *dpp_get_peer_private_data(void *ctx, uint8_t *peer_macaddr);

int dpp_set_ctx_private_data(void *ctx, void *priv);
void *dpp_get_ctx_private_data(void *ctx);

/* Register callback to receive output events from DPP state-machines */
int dpp_register_cb(void *ctx, int (*cb)(void *ctx, uint8_t *smac, enum dpp_event e, uint8_t *data, size_t len));

/* Send event/trigger to DPP state-machine */
int dpp_trigger(void *ctx, uint8_t *peer_macaddr, void *event);
void dpp_sm_free_event(void *ev);
void *dpp_sm_create_event(void *ctx, uint8_t *peer_macaddr, int event_type,
			size_t event_len, uint8_t *event_data);


uint8_t *dpp_get_bootstrap_pubkey_hash(struct dpp_bootstrap_info *bs, uint16_t *outlen);
int dpp_read_bootstrap_keys(const char *file, struct dpp_bootstrap_info *bs);
int dpp_write_bootstrap_keys(const char *file, struct dpp_bootstrap_info *bs);

int dpp_gen_key(const char *curve_name, void **key);	/* generate EC keypair */
int dpp_read_key(const char *keyfile, void *eckey);	/* in DER format */
int dpp_write_key(const char *keyfile, void *eckey);	/* in DER format */

int dpp_pubkey_hash(const uint8_t *der, size_t der_len, uint8_t *digest);
int dpp_pubkey_hash_with_chirp(const uint8_t *der, size_t der_len, uint8_t *digest);

int dpp_gen_uri(int argc, char **argv, char **out);
int dpp_gen_bootstrap_info(const char *curve, char *channel_list, char *mac,
			   char *information, struct dpp_bootstrap_info **bi);

/* function dumps bootstrap info in console */
void dpp_print_bootstrap_info(struct dpp_bootstrap_info *b);
char *dpp_get_bootstrap_uri(struct dpp_bootstrap_info *b);


int dpp_parse_uri(const char *uri, int *argc, char **argv);
int dpp_build_bootstrap_info_from_uri(const char *uri, struct dpp_bootstrap_info *b);

int dpp_is_peer_bootstrap_hash_known(void *context,
				     const uint8_t *bootstrap_hash,
				     uint8_t *peer_macaddr);
int dpp_bootstrap_add(void *ctx, struct dpp_bootstrap_info *b);
void dpp_bootstrap_free(struct dpp_bootstrap_info *b);

int dpp_gen_csign_key(const char *curve_name, void **csign_key);
int dpp_read_csign_key(const char *csign_keyfile, void **key);
int dpp_read_ppkey(const char *pp_keyfile, void **key);


int dpp_add_config(struct dpp_context *ctx, struct dpp_config *cfg, void *cfgid);
int dpp_del_config(struct dpp_context *ctx, void *cfgid);
int dpp_sign_config(struct dpp_context *ctx, void *cfgid, void *csign_key);

struct dpp_config_obj *dpp_config_obj_get_netrole(struct dpp_peer *peer,
					  enum dpp_netrole netrole, int idx);
/* for debugging */
void dpp_handle_config_obj(struct dpp_peer *auth, struct dpp_config_obj *conf);


int dpp_build_presence_announcement(struct dpp_context *ctx,
				    uint8_t *peer_macaddr,
				    uint8_t **frame, size_t *framelen);

int dpp_process_presence_announcement(struct dpp_context *ctx, uint8_t *src_macaddr,
				      uint8_t *frame, size_t framelen);

int dpp_process_virt_presence_announcement(struct dpp_context *ctx,
					   uint8_t *src_macaddr,
					   uint8_t *hash, size_t hashlen);

int dpp_build_auth_request(struct dpp_context *ctx,
			   uint8_t *peer_macaddr,
			   uint8_t **frame, size_t *framelen);

int dpp_process_auth_request(struct dpp_context *ctx,
			     uint8_t *peer_macaddr,
			     uint8_t *frame, size_t framelen);

int dpp_build_auth_response(struct dpp_context *ctx,
			    uint8_t *peer_macaddr,
			    uint8_t **frame, size_t *framelen);

int dpp_process_auth_response(struct dpp_context *ctx,
			      uint8_t *peer_macaddr,
			      uint8_t *frame, size_t framelen);

int dpp_build_auth_confirm(struct dpp_context *ctx,
			   uint8_t *peer_macaddr,
			   uint8_t **frame, size_t *framelen);

int dpp_process_auth_confirm(struct dpp_context *ctx,
			     uint8_t *peer_macaddr,
			     uint8_t *frame, size_t framelen);


int dpp_build_config_request(struct dpp_context *ctx,
			     uint8_t *peer_macaddr,
			     uint8_t **frame, size_t *framelen);

int dpp_process_config_request(struct dpp_context *ctx,
			       uint8_t *peer_macaddr,
			       uint8_t *frame, size_t framelen);

int dpp_build_config_response(struct dpp_context *ctx,
			      uint8_t *peer_macaddr,
			      uint8_t **frame, size_t *framelen);

int dpp_process_config_response(struct dpp_context *ctx,
				uint8_t *peer_macaddr,
				uint8_t *frame, size_t framelen);

#endif /* DPP_API_H */
