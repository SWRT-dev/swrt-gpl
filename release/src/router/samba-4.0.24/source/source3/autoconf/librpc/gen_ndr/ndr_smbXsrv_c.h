#include "librpc/rpc/dcerpc.h"
#include "autoconf/librpc/gen_ndr/smbXsrv.h"
#ifndef _HEADER_RPC_smbXsrv
#define _HEADER_RPC_smbXsrv

extern const struct ndr_interface_table ndr_table_smbXsrv;

struct tevent_req *dcerpc_smbXsrv_version_global_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_version_global_decode *r);
NTSTATUS dcerpc_smbXsrv_version_global_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_version_global_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_version_global_decode *r);
struct tevent_req *dcerpc_smbXsrv_version_global_decode_send(TALLOC_CTX *mem_ctx,
							     struct tevent_context *ev,
							     struct dcerpc_binding_handle *h,
							     struct smbXsrv_version_globalB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_version_global_decode_recv(struct tevent_req *req,
						   TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_version_global_decode(struct dcerpc_binding_handle *h,
					      TALLOC_CTX *mem_ctx,
					      struct smbXsrv_version_globalB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_session_global_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_session_global_decode *r);
NTSTATUS dcerpc_smbXsrv_session_global_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_global_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_session_global_decode *r);
struct tevent_req *dcerpc_smbXsrv_session_global_decode_send(TALLOC_CTX *mem_ctx,
							     struct tevent_context *ev,
							     struct dcerpc_binding_handle *h,
							     struct smbXsrv_session_globalB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_session_global_decode_recv(struct tevent_req *req,
						   TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_global_decode(struct dcerpc_binding_handle *h,
					      TALLOC_CTX *mem_ctx,
					      struct smbXsrv_session_globalB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_session_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_session_decode *r);
NTSTATUS dcerpc_smbXsrv_session_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_session_decode *r);
struct tevent_req *dcerpc_smbXsrv_session_decode_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      struct smbXsrv_sessionB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_session_decode_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_decode(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       struct smbXsrv_sessionB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_session_close_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_session_close_decode *r);
NTSTATUS dcerpc_smbXsrv_session_close_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_close_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_session_close_decode *r);
struct tevent_req *dcerpc_smbXsrv_session_close_decode_send(TALLOC_CTX *mem_ctx,
							    struct tevent_context *ev,
							    struct dcerpc_binding_handle *h,
							    struct smbXsrv_session_closeB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_session_close_decode_recv(struct tevent_req *req,
						  TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_session_close_decode(struct dcerpc_binding_handle *h,
					     TALLOC_CTX *mem_ctx,
					     struct smbXsrv_session_closeB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_tcon_global_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_tcon_global_decode *r);
NTSTATUS dcerpc_smbXsrv_tcon_global_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_tcon_global_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_tcon_global_decode *r);
struct tevent_req *dcerpc_smbXsrv_tcon_global_decode_send(TALLOC_CTX *mem_ctx,
							  struct tevent_context *ev,
							  struct dcerpc_binding_handle *h,
							  struct smbXsrv_tcon_globalB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_tcon_global_decode_recv(struct tevent_req *req,
						TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_tcon_global_decode(struct dcerpc_binding_handle *h,
					   TALLOC_CTX *mem_ctx,
					   struct smbXsrv_tcon_globalB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_tcon_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_tcon_decode *r);
NTSTATUS dcerpc_smbXsrv_tcon_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_tcon_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_tcon_decode *r);
struct tevent_req *dcerpc_smbXsrv_tcon_decode_send(TALLOC_CTX *mem_ctx,
						   struct tevent_context *ev,
						   struct dcerpc_binding_handle *h,
						   struct smbXsrv_tconB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_tcon_decode_recv(struct tevent_req *req,
					 TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_tcon_decode(struct dcerpc_binding_handle *h,
				    TALLOC_CTX *mem_ctx,
				    struct smbXsrv_tconB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_open_global_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_open_global_decode *r);
NTSTATUS dcerpc_smbXsrv_open_global_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_open_global_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_open_global_decode *r);
struct tevent_req *dcerpc_smbXsrv_open_global_decode_send(TALLOC_CTX *mem_ctx,
							  struct tevent_context *ev,
							  struct dcerpc_binding_handle *h,
							  struct smbXsrv_open_globalB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_open_global_decode_recv(struct tevent_req *req,
						TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_open_global_decode(struct dcerpc_binding_handle *h,
					   TALLOC_CTX *mem_ctx,
					   struct smbXsrv_open_globalB _blob /* [in]  */);

struct tevent_req *dcerpc_smbXsrv_open_decode_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct smbXsrv_open_decode *r);
NTSTATUS dcerpc_smbXsrv_open_decode_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_open_decode_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct smbXsrv_open_decode *r);
struct tevent_req *dcerpc_smbXsrv_open_decode_send(TALLOC_CTX *mem_ctx,
						   struct tevent_context *ev,
						   struct dcerpc_binding_handle *h,
						   struct smbXsrv_openB _blob /* [in]  */);
NTSTATUS dcerpc_smbXsrv_open_decode_recv(struct tevent_req *req,
					 TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_smbXsrv_open_decode(struct dcerpc_binding_handle *h,
				    TALLOC_CTX *mem_ctx,
				    struct smbXsrv_openB _blob /* [in]  */);

#endif /* _HEADER_RPC_smbXsrv */
