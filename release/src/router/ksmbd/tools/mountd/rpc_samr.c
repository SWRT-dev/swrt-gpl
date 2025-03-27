// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Copyright (C) 2020 Samsung Electronics Co., Ltd.
 *
 *   Author(s): Namjae Jeon (linkinjeon@kernel.org)
 */

#include <memory.h>
#include <endian.h>
#include <errno.h>
#include <limits.h>
#include <linux/ksmbd_server.h>

#include <management/user.h>
#include <rpc.h>
#include <rpc_samr.h>
#include <smbacl.h>
#include <ksmbdtools.h>

#define SAMR_OPNUM_CONNECT5		64
#define SAMR_OPNUM_ENUM_DOMAIN		6
#define SAMR_OPNUM_LOOKUP_DOMAIN	5
#define SAMR_OPNUM_OPEN_DOMAIN		7
#define SAMR_OPNUM_LOOKUP_NAMES		17
#define SAMR_OPNUM_OPEN_USER		34
#define SAMR_OPNUM_QUERY_USER_INFO	36
#define SAMR_OPNUM_QUERY_SECURITY	3
#define SAMR_OPNUM_GET_GROUP_FOR_USER	39
#define SAMR_OPNUM_GET_ALIAS_MEMBERSHIP	16
#define SAMR_OPNUM_CLOSE		1

static struct LIST	*ch_table;
static pthread_rwlock_t	ch_table_lock;
static struct LIST	*domain_entries;
static char		*domain_name;
static int		num_domain_entries;

static void samr_ch_free(struct connect_handle *ch)
{
	pthread_rwlock_wrlock(&ch_table_lock);
	list_remove(&ch_table, toid(ch->handle));
	pthread_rwlock_unlock(&ch_table_lock);

	free(ch);
}

static struct connect_handle *samr_ch_lookup(unsigned char *handle)
{
	struct connect_handle *ch;

	pthread_rwlock_rdlock(&ch_table_lock);
	ch = list_get(&ch_table, toid(handle));
	pthread_rwlock_unlock(&ch_table_lock);

	return ch;
}

static struct connect_handle *samr_ch_alloc(unsigned int id)
{
	struct connect_handle *ch;
	int ret;

	ch = calloc(1, sizeof(struct connect_handle));
	if (!ch)
		return NULL;

	id++;
	memcpy(ch->handle, &id, sizeof(unsigned int));
	ch->refcount++;
	pthread_rwlock_wrlock(&ch_table_lock);
	ret = list_add(&ch_table, ch, id);
	pthread_rwlock_unlock(&ch_table_lock);

	if (!ret) {
		samr_ch_free(ch);
		ch = NULL;
	}

	return ch;
}

static int samr_connect5_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct ndr_uniq_char_ptr server_name;

	ndr_read_uniq_vsting_ptr(dce, &server_name);
	ndr_read_int32(dce); // Access mask
	dce->sm_req.level = ndr_read_int32(dce); // level in
	ndr_read_int32(dce); // Info in
	dce->sm_req.client_version = ndr_read_int32(dce);
	return 0;
}

static int samr_connect5_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ndr_write_union_int32(dce, dce->sm_req.level); //level out
	ndr_write_int32(dce, dce->sm_req.client_version); //client version
	ndr_write_int32(dce, 0); //reserved

	ch = samr_ch_alloc(pipe->id);
	if (!ch)
		return KSMBD_RPC_ENOMEM;

	/* write connect handle */
	ndr_write_bytes(dce, ch->handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_enum_domain_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	char *hostname, *builtin;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

int samr_ndr_write_domain_array(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	int i, ret = 0;

	for (i = 0; i < num_domain_entries; i++) {
		void * entry;
		size_t name_len;

		ret = ndr_write_int32(dce, i);
		entry = list_get(&domain_entries, i);
		name_len = strlen((char *)entry);
		ret |= ndr_write_int16(dce, name_len*2);
		ret |= ndr_write_int16(dce, name_len*2);

		/* ref pointer for name entry */
		dce->num_pointers++;
		ret |= ndr_write_int32(dce, dce->num_pointers);
	}

	for (i = 0; i < num_domain_entries; i++) {
		void * entry;

		entry = list_get(&domain_entries, i);
		ret |= ndr_write_string(dce, (char *)entry);
	}

	return ret;
}

static int samr_enum_domain_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;
	int status;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;
	/* Resume Handle */
	ndr_write_int32(dce, 0);

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, num_domain_entries); // Sam entry count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, num_domain_entries); // Sam max entry count

	status = samr_ndr_write_domain_array(pipe);

	/* [out] DWORD* Num Entries */
	ndr_write_int32(dce, num_domain_entries);

	return status;
}

static int samr_lookup_domain_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);
	ndr_read_int16(dce); // name len
	ndr_read_int16(dce); // name size
	ndr_read_uniq_vsting_ptr(dce, &dce->sm_req.name); // domain name

	return KSMBD_RPC_OK;
}

static int samr_lookup_domain_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;
	struct smb_sid sid = {0};
	int i, j;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers);
	ndr_write_int32(dce, 4);

	for (i = 0; i < num_domain_entries; i++) {
		void * entry;

		entry = list_get(&domain_entries, i);
		if (!strcmp(STR_VAL(dce->sm_req.name), (char *)entry)) {
			smb_init_domain_sid(&sid);
			smb_write_sid(dce, &sid);
		}
	}

	return KSMBD_RPC_OK;
}

static int samr_open_domain_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	int i, j;
	struct smb_sid sid;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_open_domain_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;
	ch->refcount++;
	ndr_write_bytes(dce, ch->handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_lookup_names_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct passwd *passwd;
	int user_num;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	user_num = ndr_read_int32(dce);
	ndr_read_int32(dce); // max count
	ndr_read_int32(dce); // offset
	ndr_read_int32(dce); // actual count
	ndr_read_int16(dce); // name len
	ndr_read_int16(dce); // name size

	ndr_read_uniq_vsting_ptr(dce, &dce->sm_req.name); // names

	return KSMBD_RPC_OK;
}

static int samr_lookup_names_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;
	struct passwd *passwd;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	ch->user = usm_lookup_user(STR_VAL(dce->sm_req.name));
	if (!ch->user)
		return KSMBD_RPC_EACCESS_DENIED;

	ndr_write_int32(dce, 1); // count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 1); // count
	ndr_write_int32(dce, ch->user->uid); // RID

	ndr_write_int32(dce, 1);
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers);
	ndr_write_int32(dce, 1);
	ndr_write_int32(dce, 1);

	return KSMBD_RPC_OK;
}

static int samr_open_user_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	unsigned int req_rid;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	ndr_read_int32(dce);
	dce->sm_req.rid = ndr_read_int32(dce); // RID

	return KSMBD_RPC_OK;
}

static int samr_open_user_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;
	ch->refcount++;

	if (dce->sm_req.rid != ch->user->uid)
		return KSMBD_RPC_EBAD_FID;

	ndr_write_bytes(dce, ch->handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_query_user_info_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_query_user_info_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;
	char *home_dir, *profile_path;
	char hostname[NAME_MAX];
	int home_dir_len, i;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	gethostname(hostname, NAME_MAX);
	home_dir_len = 2 + strlen(hostname) + 1 + strlen(ch->user->name);

	home_dir = calloc(1, home_dir_len);
	if (!home_dir)
		return KSMBD_RPC_ENOMEM;

	/* Make Home dir string */
	strcpy(home_dir, "\\\\");
	strcat(home_dir, hostname);
	strcat(home_dir, "\\");
	strcat(home_dir, ch->user->name);

	profile_path = calloc(1, home_dir_len + strlen("profile"));
	if (!profile_path) {
		free(home_dir);
		return KSMBD_RPC_ENOMEM;
	}

	/* Make Profile path string */
	strcat(profile_path, "\\\\");
	strcat(profile_path, hostname);
	strcat(profile_path, "\\");
	strcat(profile_path, ch->user->name);
	strcat(profile_path, "\\");
	strcat(profile_path, "profile");

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, 0x15); // info
	ndr_write_int16(dce, 0);

	/*
	 * Last Logon/Logoff/Password change, Acct Expiry,
	 * Allow Passworkd Change, Force Password Change.
	 */
	for (i = 0; i < 6; i++)
		ndr_write_int64(dce, 0);

	ndr_write_int16(dce, strlen(ch->user->name)*2); // account name length
	ndr_write_int16(dce, strlen(ch->user->name)*2);

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, strlen(ch->user->name)*2); // full name length
	ndr_write_int16(dce, strlen(ch->user->name)*2);

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, strlen(home_dir)*2); // home directory length
	ndr_write_int16(dce, strlen(home_dir)*2);

	/* Home Drive, Logon Script */
	for (i = 0; i < 2; i++) {
		dce->num_pointers++;
		ndr_write_int32(dce, dce->num_pointers); // ref pointer
		ndr_write_int16(dce, 0);
		ndr_write_int16(dce, 0);
	}

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int16(dce, strlen(profile_path)*2); //profile path length
	ndr_write_int16(dce, strlen(profile_path)*2);

	/* Description, Workstations, Comments, Parameters */
	for (i = 0; i < 4; i++) {
		dce->num_pointers++;
		ndr_write_int32(dce, dce->num_pointers); // ref pointer
		ndr_write_int16(dce, 0);
		ndr_write_int16(dce, 0);
	}

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers);
	/* Lm, Nt, Password and Private*/
	for (i = 0; i < 3; i++) {
		ndr_write_int16(dce, 0);
		ndr_write_int16(dce, 0);
		ndr_write_int32(dce, 0);
	}

	ndr_write_int32(dce, 0); // buf count
	/* Pointer to Buffer */
	ndr_write_int32(dce, 0);
	ndr_write_int32(dce, ch->user->uid); // rid
	ndr_write_int32(dce, 513); // primary gid
	ndr_write_int32(dce, 0x00000010); // Acct Flags : Acb Normal
	ndr_write_int32(dce, 0x00FFFFFF); // Fields Present
	ndr_write_int16(dce, 168); // logon hours
	ndr_write_int16(dce, 0);

	/* Pointers to Bits */
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); //ref pointer

	/* Bad Password/Logon Count/Country Code/Code Page */
	for (i = 0; i < 4; i++)
		ndr_write_int16(dce, 0);

	/* Lm/Nt Password Set, Password Expired/etc */
	ndr_write_int8(dce, 0);
	ndr_write_int8(dce, 0);
	ndr_write_int8(dce, 0);
	ndr_write_int8(dce, 0);

	ndr_write_string(dce, ch->user->name);
	ndr_write_string(dce, ch->user->name);
	ndr_write_string(dce, home_dir);

	/* Home Drive, Logon Script */
	for (i = 0; i < 2; i++) {
		ndr_write_int32(dce, 0);
		ndr_write_int32(dce, 0);
		ndr_write_int32(dce, 0);
	}

	ndr_write_string(dce, profile_path);

	/* Description, Workstations, Comments, Parameters */
	for (i = 0; i < 4; i++) {
		ndr_write_int32(dce, 0);
		ndr_write_int32(dce, 0);
		ndr_write_int32(dce, 0);
	}

	/* Logon Hours */
	ndr_write_int32(dce, 1260);
	ndr_write_int32(dce, 0);
	ndr_write_int32(dce, 21);

	for (i = 0; i < 21; i++)
		ndr_write_int8(dce, 0xff);

	free(home_dir);
	free(profile_path);
	return KSMBD_RPC_OK;
}

static int samr_query_security_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_query_security_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;
	int sec_desc_len, curr_offset, payload_offset;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	curr_offset = dce->offset;
	dce->offset += 16;
	build_sec_desc(dce, &sec_desc_len, ch->user->uid);
	payload_offset = dce->offset;

	dce->offset = curr_offset;
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers);
	ndr_write_int32(dce, sec_desc_len);

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers);
	ndr_write_int32(dce, sec_desc_len);

	dce->offset = payload_offset;

	return KSMBD_RPC_OK;
}

static int samr_get_group_for_user_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_get_group_for_user_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 1); // count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 1); // max count
	ndr_write_int32(dce, 513); // group rid
	ndr_write_int32(dce, 0x00000007); // attributes

	return KSMBD_RPC_OK;
}

static int samr_get_alias_membership_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_get_alias_membership_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;

	ndr_write_int32(dce, 0); // count
	dce->num_pointers++;
	ndr_write_int32(dce, dce->num_pointers); // ref pointer
	ndr_write_int32(dce, 0); // max count

	return KSMBD_RPC_OK;
}

static int samr_close_invoke(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;

	ndr_read_bytes(dce, dce->sm_req.handle, HANDLE_SIZE);

	return KSMBD_RPC_OK;
}

static int samr_close_return(struct ksmbd_rpc_pipe *pipe)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	struct connect_handle *ch;

	ch = samr_ch_lookup(dce->sm_req.handle);
	if (!ch)
		return KSMBD_RPC_EBAD_FID;
	else if (ch->refcount > 1)
		ch->refcount--;
	else
		samr_ch_free(ch);

	/* write connect handle */
	ndr_write_int64(dce, 0);
	ndr_write_int64(dce, 0);
	ndr_write_int32(dce, 0);

	return KSMBD_RPC_OK;
}

static int samr_invoke(struct ksmbd_rpc_pipe *pipe)
{
	int ret = KSMBD_RPC_ENOTIMPLEMENTED;

	switch (pipe->dce->req_hdr.opnum) {
	case SAMR_OPNUM_CONNECT5:
		ret = samr_connect5_invoke(pipe);
		break;
	case SAMR_OPNUM_ENUM_DOMAIN:
		ret = samr_enum_domain_invoke(pipe);
		break;
	case SAMR_OPNUM_LOOKUP_DOMAIN:
		ret = samr_lookup_domain_invoke(pipe);
		break;
	case SAMR_OPNUM_OPEN_DOMAIN:
		ret = samr_open_domain_invoke(pipe);
		break;
	case SAMR_OPNUM_LOOKUP_NAMES:
		ret = samr_lookup_names_invoke(pipe);
		break;
	case SAMR_OPNUM_OPEN_USER:
		ret = samr_open_user_invoke(pipe);
		break;
	case SAMR_OPNUM_QUERY_USER_INFO:
		ret = samr_query_user_info_invoke(pipe);
		break;
	case SAMR_OPNUM_QUERY_SECURITY:
		ret = samr_query_security_invoke(pipe);
		break;
	case SAMR_OPNUM_GET_GROUP_FOR_USER:
		ret = samr_get_group_for_user_invoke(pipe);
		break;
	case SAMR_OPNUM_GET_ALIAS_MEMBERSHIP:
		ret = samr_get_alias_membership_invoke(pipe);
		break;
	case SAMR_OPNUM_CLOSE:
		ret = samr_close_invoke(pipe);
		break;
	default:
		pr_err("SAMR: unsupported INVOKE method %d\n",
		       pipe->dce->req_hdr.opnum);
		break;
	}

	return ret;
}

static int samr_return(struct ksmbd_rpc_pipe *pipe,
			 struct ksmbd_rpc_command *resp,
			 int max_resp_sz)
{
	struct ksmbd_dcerpc *dce = pipe->dce;
	int status;

	/*
	 * Reserve space for response NDR header. We don't know yet if
	 * the payload buffer is big enough. This will determine if we
	 * can set DCERPC_PFC_FIRST_FRAG|DCERPC_PFC_LAST_FRAG or if we
	 * will have a multi-part response.
	 */
	dce->offset = sizeof(struct dcerpc_header);
	dce->offset += sizeof(struct dcerpc_response_header);

	switch (dce->req_hdr.opnum) {
	case SAMR_OPNUM_CONNECT5:
		status = samr_connect5_return(pipe);
		break;
	case SAMR_OPNUM_ENUM_DOMAIN:
		status = samr_enum_domain_return(pipe);
		break;
	case SAMR_OPNUM_LOOKUP_DOMAIN:
		status = samr_lookup_domain_return(pipe);
		break;
	case SAMR_OPNUM_OPEN_DOMAIN:
		status = samr_open_domain_return(pipe);
		break;
	case SAMR_OPNUM_LOOKUP_NAMES:
		status = samr_lookup_names_return(pipe);
		break;
	case SAMR_OPNUM_OPEN_USER:
		status = samr_open_user_return(pipe);
		break;
	case SAMR_OPNUM_QUERY_USER_INFO:
		status = samr_query_user_info_return(pipe);
		break;
	case SAMR_OPNUM_QUERY_SECURITY:
		status = samr_query_security_return(pipe);
		break;
	case SAMR_OPNUM_GET_GROUP_FOR_USER:
		status = samr_get_group_for_user_return(pipe);
		break;
	case SAMR_OPNUM_GET_ALIAS_MEMBERSHIP:
		status = samr_get_alias_membership_return(pipe);
		break;
	case SAMR_OPNUM_CLOSE:
		status = samr_close_return(pipe);
		break;
	default:
		pr_err("SAMR: unsupported RETURN method %d\n",
			dce->req_hdr.opnum);
		status = KSMBD_RPC_EBAD_FUNC;
		break;
	}

	if (rpc_restricted_context(dce->rpc_req))
		status = KSMBD_RPC_EACCESS_DENIED;

	/*
	 * [out] DWORD Return value/code
	 */
	ndr_write_int32(dce, status);
	dcerpc_write_headers(dce, status);

	dce->rpc_resp->payload_sz = dce->offset;
	return status;
}

int rpc_samr_read_request(struct ksmbd_rpc_pipe *pipe,
			    struct ksmbd_rpc_command *resp,
			    int max_resp_sz)
{
	return samr_return(pipe, resp, max_resp_sz);
}

int rpc_samr_write_request(struct ksmbd_rpc_pipe *pipe)
{
	return samr_invoke(pipe);
}

static int rpc_samr_add_domain_entry(char *name)
{
	char *domain_string;

	domain_string = strdup(name);
	if (!domain_string)
		return KSMBD_RPC_ENOMEM;
	list_append(&domain_entries, domain_string);
	num_domain_entries++;

	return 0;
}

static void rpc_samr_remove_domain_entry(unsigned int eidx)
{
	void * entry;

	entry = list_get(&domain_entries, eidx);
	list_remove_dec(&domain_entries, eidx);
	free(entry);
}

static void domain_entry_free(void *s, unsigned long long id, void *user_data)
{
	char **entry = s;
	free(*entry);
	free(s);
}

int rpc_samr_init(void)
{
	char hostname[NAME_MAX];
	pthread_rwlock_init(&ch_table_lock, NULL);

	list_init(&ch_table);
	if (!ch_table)
		return -ENOMEM;
	list_init(&domain_entries);
	if (!domain_entries)
		return -ENOMEM;

//	g_array_set_clear_func(domain_entries, domain_entry_free);

	/*
	 * ksmbd supports the standalone server and
	 * uses the hostname as the domain name.
	 */
	gethostname(hostname, NAME_MAX);
	domain_name = strup(hostname);
	rpc_samr_add_domain_entry(domain_name);
	rpc_samr_add_domain_entry("Builtin");
	return 0;
}

void rpc_samr_destroy(void)
{
	if (ch_table)
		list_clear(&ch_table);
	pthread_rwlock_destroy(&ch_table_lock);
	num_domain_entries = 0;
	free(domain_name);
	if (domain_entries) {
		list_foreach(&domain_entries, domain_entry_free, NULL);
		list_clear(&domain_entries);
	}
}
