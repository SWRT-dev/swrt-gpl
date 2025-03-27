// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 *   Author(s): Namjae Jeon (linkinjeon@kernel.org)
 */

#include <memory.h>
#include <endian.h>
#include <pwd.h>
#include <errno.h>
#include <linux/ksmbd_server.h>

#include <management/user.h>
#include <rpc.h>
#include <rpc_lsarpc.h>
#include <smbacl.h>
#include <ksmbdtools.h>

#define LSARPC_OPNUM_DS_ROLE_GET_PRIMARY_DOMAIN_INFO	0
#define LSARPC_OPNUM_OPEN_POLICY2			44
#define LSARPC_OPNUM_QUERY_INFO_POLICY			7
#define LSARPC_OPNUM_LOOKUP_SID2			57
#define LSARPC_OPNUM_LOOKUP_NAMES3			68
#define LSARPC_OPNUM_CLOSE				0

#define DS_ROLE_STANDALONE_SERVER	2
#define DS_ROLE_BASIC_INFORMATION	1

#define LSA_POLICY_INFO_ACCOUNT_DOMAIN	5

static struct LIST	*ph_table;
static pthread_rwlock_t ph_table_lock;
static char		*domain_name;
static void lsarpc_ph_free(struct policy_handle *ph)
{
	pthread_rwlock_wrlock(&ph_table_lock);
	list_remove(&ph_table, toid(ph->handle));
	pthread_rwlock_unlock(&ph_table_lock);

	free(ph);
}

static struct policy_handle *lsarpc_ph_lookup(unsigned char *handle)
{
	struct policy_handle *ph;

	pthread_rwlock_rdlock(&ph_table_lock);
	ph = list_get(&ph_table, toid(handle));
	pthread_rwlock_unlock(&ph_table_lock);

	return ph;
}

static struct policy_handle *lsarpc_ph_alloc(unsigned int id)
{
	struct policy_handle *ph;
	int ret;

	ph = calloc(1, sizeof(struct policy_handle));
	if (!ph)
		return NULL;

	id++;
	memcpy(ph->handle, &id, sizeof(unsigned int));
	pthread_rwlock_wrlock(&ph_table_lock);
	ret = list_add(&ph_table, ph, id);
	pthread_rwlock_unlock(&ph_table_lock);

	if (!ret) {
		lsarpc_ph_free(ph);
		ph = NULL;
	}

	return ph;
}

static int lsa_domain_account_rep(struct ksmbd_dcerpc *dce, char *domain_name)
{
	int len, ret;

	len = strlen(domain_name);
	ret = ndr_write_int16(dce, len*2); // length
	ret |= ndr_write_int16(dce, (len+1)*2); // size
	dce->num_pointers++;
	ret |= ndr_write_int32(dce, dce->num_pointers); /* ref pointer for domain name*/
	dce->num_pointers++;
	ret |= ndr_write_int32(dce, dce->num_pointers); /* ref pointer for sid*/
	return ret;
}

static int lsa_domain_account_data(struct ksmbd_dcerpc *dce, char *domain_name,
		struct smb_sid *sid)
{
	int ret;

	ret = ndr_write_lsa_string(dce, domain_name); // domain string
	ret |= ndr_write_int32(dce, sid->num_subauth); // count
	smb_write_sid(dce, sid); // sid
	return ret;
}

static int lsarpc_get_primary_domain_info_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	dce->lr_req.level = ndr_read_int16(dce);

	return KSMBD_RPC_OK;
}

static int lsarpc_get_primary_domain_info_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	int i;
	size_t max_len, actual_len;

	if (dce->lr_req.level != DS_ROLE_BASIC_INFORMATION)
		return KSMBD_RPC_EBAD_FUNC;

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, 1); // count
	ndr_write_int16(dce, 0);
	ndr_write_int16(dce, DS_ROLE_STANDALONE_SERVER); // role
	ndr_write_int16(dce, 0);
	ndr_write_int32(dce, 0); // flags
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 0); // NULL pointer : Pointer to Dns Domain
	ndr_write_int32(dce, 0); // NULL pointer : Pointer to Forest

	/* NULL Domain guid */
	for (i = 0; i < 16; i++)
		ndr_write_int8(dce, 0);

	ndr_write_vstring(dce, domain_name); // domain string

	return KSMBD_RPC_OK;
}

static int lsarpc_open_policy2_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct policy_handle *ph;

	ph = lsarpc_ph_alloc(pipe->id);
	if (!ph)
		return KSMBD_RPC_ENOMEM;

	/* write connect handle */
	ndr_write_bytes(dce, ph->handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int lsarpc_query_info_policy_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	unsigned long long id;

	ndr_read_bytes(dce, dce->lr_req.handle, HANDLE_SIZE);
	dce->lr_req.level = ndr_read_int16(dce); // level

	return KSMBD_RPC_OK;
}

static int lsarpc_query_info_policy_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct smb_sid sid;
	struct policy_handle *ph;
	int len;

	ph = lsarpc_ph_lookup(dce->lr_req.handle);
	if (!ph)
		return KSMBD_RPC_EBAD_FID;

	if (dce->lr_req.level != LSA_POLICY_INFO_ACCOUNT_DOMAIN)
		return KSMBD_RPC_EBAD_FUNC;

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, LSA_POLICY_INFO_ACCOUNT_DOMAIN); // level
	ndr_write_int16(dce, 0);

	/* Domain, Sid ref pointer */
	lsa_domain_account_rep(dce, domain_name);

	/* Pointer to domain, Sid */
	smb_init_domain_sid(&sid);
	lsa_domain_account_data(dce, domain_name, &sid);

	return KSMBD_RPC_OK;
}

static int __lsarpc_entry_processed(struct ksmbd_rpc_pipe *pipe, int i)
{
	void *entry;

	entry = list_get(&pipe->entries, i);
	list_remove_dec(&pipe->entries, i);
	free(entry);
	return 0;
}

static int lsarpc_lookup_sid2_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	unsigned long long id;
	unsigned int num_sid, i;

	ndr_read_bytes(dce, dce->lr_req.handle, HANDLE_SIZE);

	num_sid = ndr_read_int32(dce);
	ndr_read_int32(dce); // ref pointer
	ndr_read_int32(dce); // max count

	for (i = 0; i < num_sid; i++)
		ndr_read_int32(dce); // ref pointer

	for (i = 0; i < num_sid; i++) {
		struct lsarpc_names_info *ni;
		struct passwd *passwd;
		int rid;

		ni = calloc(1, sizeof(struct lsarpc_names_info));
		if (!ni)
			break;

		ndr_read_int32(dce); // max count
		smb_read_sid(dce, &ni->sid); // sid
		ni->sid.num_subauth--;
		rid = ni->sid.sub_auth[ni->sid.num_subauth];
		passwd = getpwuid(rid);
		if (passwd)
			ni->user = usm_lookup_user(passwd->pw_name);

		ni->index = i + 1;
		if (set_domain_name(&ni->sid, ni->domain_str, &ni->type))
			ni->index = -1;
		list_append(&pipe->entries, ni);
		pipe->num_entries++;
	}

	pipe->entry_processed = __lsarpc_entry_processed;
	return KSMBD_RPC_OK;
}

static int lsarpc_lookup_sid2_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct policy_handle *ph;
	int i, rc = KSMBD_RPC_OK;

	ph = lsarpc_ph_lookup(dce->lr_req.handle);
	if (!ph)
		return KSMBD_RPC_EBAD_FID;

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, pipe->num_entries); // count

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 32); // max size
	ndr_write_int32(dce, pipe->num_entries); // max count

	for (i = 0; i < pipe->num_entries; i++) {
		struct lsarpc_names_info *ni;
		int len;

		ni = (struct lsarpc_names_info *)list_get(&pipe->entries, i);
		if (ni->type == -1)
			rc = KSMBD_RPC_SOME_NOT_MAPPED;
		lsa_domain_account_rep(dce, ni->domain_str);
	}

	for (i = 0; i < pipe->num_entries; i++) {
		struct lsarpc_names_info *ni;

		ni = (struct lsarpc_names_info *)list_get(&pipe->entries, i);
		lsa_domain_account_data(dce, ni->domain_str, &ni->sid);
	}

	/* Pointer to Names */
	ndr_write_int32(dce, pipe->num_entries); // count
	dce->num_pointers++; // ref pointer
	ndr_write_int32(dce, dce->num_pointers);
	ndr_write_int32(dce, pipe->num_entries); // max count

	for (i = 0; i < pipe->num_entries; i++) {
		struct lsarpc_names_info *ni;
		size_t len;

		ni = (struct lsarpc_names_info *)list_get(&pipe->entries, i);
		ndr_write_int16(dce, ni->type); // sid type
		ndr_write_int16(dce, 0);
		if (ni->user)
			len = strlen(ni->user->name);
		else
			len = strlen("None");
		ndr_write_int16(dce, len*2); // length
		ndr_write_int16(dce, len*2); // size
		dce->num_pointers++; // ref pointer
		ndr_write_int32(dce, dce->num_pointers);
		ndr_write_int32(dce, ni->index); // sid index
		ndr_write_int32(dce, 0); // unknown
	}

	for (i = 0; i < pipe->num_entries; i++) {
		struct lsarpc_names_info *ni;
		char *username = "None";

		ni = (struct lsarpc_names_info *)list_get(&pipe->entries, i);
		if (ni->user)
			username = ni->user->name;
		ndr_write_string(dce, username); // username
	}

	ndr_write_int32(dce, pipe->num_entries); // count
	if (pipe->entry_processed) {
		for (i = 0; i < pipe->num_entries; i++)
			pipe->entry_processed(pipe, 0);
		pipe->num_entries = 0;
	}

	return rc;
}

static int lsarpc_lookup_names3_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct ndr_uniq_char_ptr username;
	int num_names, i;

	ndr_read_bytes(dce, dce->lr_req.handle, HANDLE_SIZE);

	num_names = ndr_read_int32(dce); // num names
	ndr_read_int32(dce); // max count

	for (i = 0; i < num_names; i++) {
		struct lsarpc_names_info *ni;
		char *name = NULL;

		ni = malloc(sizeof(struct lsarpc_names_info));
		if (!ni)
			break;
		ndr_read_int16(dce); // length
		ndr_read_int16(dce); // size
		ndr_read_uniq_vsting_ptr(dce, &username);
		if (strstr(STR_VAL(username), "\\")) {
			strtok(STR_VAL(username), "\\");
			name = strtok(NULL, "\\");
		}

		ni->user = usm_lookup_user(name);
		if (!ni->user)
			break;
		list_append(&pipe->entries, ni);
		pipe->num_entries++;
		smb_init_domain_sid(&ni->sid);
	}
	pipe->entry_processed = __lsarpc_entry_processed;

	return KSMBD_RPC_OK;
}

static int lsarpc_lookup_names3_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct policy_handle *ph;
	struct smb_sid sid;
	int i, len;

	ph = lsarpc_ph_lookup(dce->lr_req.handle);
	if (!ph)
		return KSMBD_RPC_EBAD_FID;

	/* Domain list */
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer

	ndr_write_int32(dce, 1); // domain count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 32); // max size
	ndr_write_int32(dce, 1); // max count

	lsa_domain_account_rep(dce, domain_name);
	smb_init_domain_sid(&sid);
	lsa_domain_account_data(dce, domain_name, &sid);

	ndr_write_int32(dce, pipe->num_entries); // count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // sid ref id
	ndr_write_int32(dce, pipe->num_entries); // count

	for (i = 0; i < pipe->num_entries; i++) {
		ndr_write_int16(dce, SID_TYPE_USER); // sid type
		ndr_write_int16(dce, 0);
		dce->num_pointers++;
		ndr_write_int32(dce, dce->num_pointers); // ref pointer
		ndr_write_int32(dce, i); // sid index
		ndr_write_int32(dce, 0);
	}

	for (i = 0; i < pipe->num_entries; i++) {
		struct lsarpc_names_info *ni;

		ni = (struct lsarpc_names_info *)list_get(&pipe->entries, i);
		ni->sid.sub_auth[ni->sid.num_subauth++] = ni->user->uid;
		ndr_write_int32(dce, ni->sid.num_subauth); // sid auth count
		smb_write_sid(dce, &ni->sid); // sid
	}

	ndr_write_int32(dce, pipe->num_entries);
	if (pipe->entry_processed) {
		for (i = 0; i < pipe->num_entries; i++)
			pipe->entry_processed(pipe, 0);
		pipe->num_entries = 0;
	}

	return KSMBD_RPC_OK;
}

static int lsarpc_close_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->lr_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int lsarpc_close_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct policy_handle *ph;

	ph = lsarpc_ph_lookup(dce->lr_req.handle);
	if (!ph)
		return KSMBD_RPC_EBAD_FID;
	lsarpc_ph_free(ph);

	ndr_write_int64(dce, 0);
	ndr_write_int64(dce, 0);
	ndr_write_int32(dce, 0);
	return KSMBD_RPC_OK;
}

static int lsarpc_invoke(struct ksmbd_rpc_pipe *pipe)
{
	int ret = KSMBD_RPC_ENOTIMPLEMENTED;

	switch (pipe->dce->req_hdr.opnum) {
	case LSARPC_OPNUM_DS_ROLE_GET_PRIMARY_DOMAIN_INFO || LSARPC_OPNUM_CLOSE:
		if (pipe->dce->hdr.frag_length == 26)
			ret = lsarpc_get_primary_domain_info_invoke(pipe);
		else
			ret = lsarpc_close_invoke(pipe);
		break;
	case LSARPC_OPNUM_OPEN_POLICY2:
		ret = KSMBD_RPC_OK;
		break;
	case LSARPC_OPNUM_QUERY_INFO_POLICY:
		ret = lsarpc_query_info_policy_invoke(pipe);
		break;
	case LSARPC_OPNUM_LOOKUP_SID2:
		ret = lsarpc_lookup_sid2_invoke(pipe);
		break;
	case LSARPC_OPNUM_LOOKUP_NAMES3:
		ret = lsarpc_lookup_names3_invoke(pipe);
		break;
	default:
		pr_err("LSARPC: unsupported INVOKE method %d, alloc_hint : %d\n",
		       pipe->dce->req_hdr.opnum, pipe->dce->req_hdr.alloc_hint);
		break;
	}

	return ret;
}

static int lsarpc_return(struct ksmbd_rpc_pipe *pipe,
			 struct ksmbd_rpc_command *resp,
			 int max_resp_sz)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	int status = KSMBD_RPC_ENOTIMPLEMENTED;

	dce->offset = sizeof(struct dcerpc_header);
	dce->offset += sizeof(struct dcerpc_response_header);

	switch (dce->req_hdr.opnum) {
	case LSARPC_OPNUM_DS_ROLE_GET_PRIMARY_DOMAIN_INFO || LSARPC_OPNUM_CLOSE:
		if (dce->hdr.frag_length == 26)
			status = lsarpc_get_primary_domain_info_return(pipe);
		else
			status = lsarpc_close_return(pipe);
		break;
	case LSARPC_OPNUM_OPEN_POLICY2:
		status = lsarpc_open_policy2_return(pipe);
		break;
	case LSARPC_OPNUM_QUERY_INFO_POLICY:
		status = lsarpc_query_info_policy_return(pipe);
		break;
	case LSARPC_OPNUM_LOOKUP_SID2:
		status = lsarpc_lookup_sid2_return(pipe);
		break;
	case LSARPC_OPNUM_LOOKUP_NAMES3:
		status = lsarpc_lookup_names3_return(pipe);
		break;
	default:
		pr_err("LSARPC: unsupported RETURN method %d\n",
			dce->req_hdr.opnum);
		status = KSMBD_RPC_EBAD_FUNC;
		break;
	}

	/*
	 * [out] DWORD Return value/code
	 */
	ndr_write_int32(dce, status);
	dcerpc_write_headers(dce, status);

	dce->rpc_resp->payload_sz = dce->offset;
	return status;
}

int rpc_lsarpc_read_request(struct ksmbd_rpc_pipe *pipe,
			    struct ksmbd_rpc_command *resp,
			    int max_resp_sz)
{
	return lsarpc_return(pipe, resp, max_resp_sz);
}

int rpc_lsarpc_write_request(struct ksmbd_rpc_pipe *pipe)
{
	return lsarpc_invoke(pipe);
}

int rpc_lsarpc_init(void)
{
	char domain_string[NAME_MAX];
	pthread_rwlock_init(&ph_table_lock, NULL);

	/*
	 * ksmbd supports the standalone server and
	 * uses the hostname as the domain name.
	 */
	gethostname(domain_string, NAME_MAX);
	domain_name = strup(domain_string);
	list_init(&ph_table);
	if (!ph_table)
		return -ENOMEM;
	return 0;
}

void rpc_lsarpc_destroy(void)
{
	free(domain_name);
	if (ph_table)
		list_clear(&ph_table);
	pthread_rwlock_destroy(&ph_table_lock);
}
