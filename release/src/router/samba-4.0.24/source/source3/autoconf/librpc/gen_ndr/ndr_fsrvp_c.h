#include "librpc/rpc/dcerpc.h"
#include "autoconf/librpc/gen_ndr/fsrvp.h"
#ifndef _HEADER_RPC_FileServerVssAgent
#define _HEADER_RPC_FileServerVssAgent

extern const struct ndr_interface_table ndr_table_FileServerVssAgent;

struct tevent_req *dcerpc_fss_GetSupportedVersion_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_GetSupportedVersion *r);
NTSTATUS dcerpc_fss_GetSupportedVersion_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_GetSupportedVersion_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_GetSupportedVersion *r);
struct tevent_req *dcerpc_fss_GetSupportedVersion_send(TALLOC_CTX *mem_ctx,
						       struct tevent_context *ev,
						       struct dcerpc_binding_handle *h,
						       uint8_t _magic[52] /* [in]  */,
						       uint32_t *_MinVersion /* [out] [ref] */,
						       uint32_t *_MaxVersion /* [out] [ref] */);
NTSTATUS dcerpc_fss_GetSupportedVersion_recv(struct tevent_req *req,
					     TALLOC_CTX *mem_ctx,
					     uint32_t *result);
NTSTATUS dcerpc_fss_GetSupportedVersion(struct dcerpc_binding_handle *h,
					TALLOC_CTX *mem_ctx,
					uint8_t _magic[52] /* [in]  */,
					uint32_t *_MinVersion /* [out] [ref] */,
					uint32_t *_MaxVersion /* [out] [ref] */,
					uint32_t *result);

struct tevent_req *dcerpc_fss_SetContext_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_SetContext *r);
NTSTATUS dcerpc_fss_SetContext_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_SetContext_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_SetContext *r);
struct tevent_req *dcerpc_fss_SetContext_send(TALLOC_CTX *mem_ctx,
					      struct tevent_context *ev,
					      struct dcerpc_binding_handle *h,
					      uint32_t _Context /* [in]  */);
NTSTATUS dcerpc_fss_SetContext_recv(struct tevent_req *req,
				    TALLOC_CTX *mem_ctx,
				    uint32_t *result);
NTSTATUS dcerpc_fss_SetContext(struct dcerpc_binding_handle *h,
			       TALLOC_CTX *mem_ctx,
			       uint32_t _Context /* [in]  */,
			       uint32_t *result);

struct tevent_req *dcerpc_fss_StartShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_StartShadowCopySet *r);
NTSTATUS dcerpc_fss_StartShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_StartShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_StartShadowCopySet *r);
struct tevent_req *dcerpc_fss_StartShadowCopySet_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      struct GUID _ClientShadowCopySetId /* [in]  */,
						      struct GUID *_pShadowCopySetId /* [out] [ref] */);
NTSTATUS dcerpc_fss_StartShadowCopySet_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *result);
NTSTATUS dcerpc_fss_StartShadowCopySet(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       struct GUID _ClientShadowCopySetId /* [in]  */,
				       struct GUID *_pShadowCopySetId /* [out] [ref] */,
				       uint32_t *result);

struct tevent_req *dcerpc_fss_AddToShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_AddToShadowCopySet *r);
NTSTATUS dcerpc_fss_AddToShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_AddToShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_AddToShadowCopySet *r);
struct tevent_req *dcerpc_fss_AddToShadowCopySet_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      struct GUID _ClientShadowCopyId /* [in]  */,
						      struct GUID _ShadowCopySetId /* [in]  */,
						      const char *_ShareName /* [in] [charset(UTF16),ref] */,
						      struct GUID *_pShadowCopyId /* [out] [ref] */);
NTSTATUS dcerpc_fss_AddToShadowCopySet_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *result);
NTSTATUS dcerpc_fss_AddToShadowCopySet(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       struct GUID _ClientShadowCopyId /* [in]  */,
				       struct GUID _ShadowCopySetId /* [in]  */,
				       const char *_ShareName /* [in] [charset(UTF16),ref] */,
				       struct GUID *_pShadowCopyId /* [out] [ref] */,
				       uint32_t *result);

struct tevent_req *dcerpc_fss_CommitShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_CommitShadowCopySet *r);
NTSTATUS dcerpc_fss_CommitShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_CommitShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_CommitShadowCopySet *r);
struct tevent_req *dcerpc_fss_CommitShadowCopySet_send(TALLOC_CTX *mem_ctx,
						       struct tevent_context *ev,
						       struct dcerpc_binding_handle *h,
						       struct GUID _ShadowCopySetId /* [in]  */,
						       uint32_t _TimeOutInMilliseconds /* [in]  */);
NTSTATUS dcerpc_fss_CommitShadowCopySet_recv(struct tevent_req *req,
					     TALLOC_CTX *mem_ctx,
					     uint32_t *result);
NTSTATUS dcerpc_fss_CommitShadowCopySet(struct dcerpc_binding_handle *h,
					TALLOC_CTX *mem_ctx,
					struct GUID _ShadowCopySetId /* [in]  */,
					uint32_t _TimeOutInMilliseconds /* [in]  */,
					uint32_t *result);

struct tevent_req *dcerpc_fss_ExposeShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_ExposeShadowCopySet *r);
NTSTATUS dcerpc_fss_ExposeShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_ExposeShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_ExposeShadowCopySet *r);
struct tevent_req *dcerpc_fss_ExposeShadowCopySet_send(TALLOC_CTX *mem_ctx,
						       struct tevent_context *ev,
						       struct dcerpc_binding_handle *h,
						       struct GUID _ShadowCopySetId /* [in]  */,
						       uint32_t _TimeOutInMilliseconds /* [in]  */);
NTSTATUS dcerpc_fss_ExposeShadowCopySet_recv(struct tevent_req *req,
					     TALLOC_CTX *mem_ctx,
					     uint32_t *result);
NTSTATUS dcerpc_fss_ExposeShadowCopySet(struct dcerpc_binding_handle *h,
					TALLOC_CTX *mem_ctx,
					struct GUID _ShadowCopySetId /* [in]  */,
					uint32_t _TimeOutInMilliseconds /* [in]  */,
					uint32_t *result);

struct tevent_req *dcerpc_fss_RecoveryCompleteShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_RecoveryCompleteShadowCopySet *r);
NTSTATUS dcerpc_fss_RecoveryCompleteShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_RecoveryCompleteShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_RecoveryCompleteShadowCopySet *r);
struct tevent_req *dcerpc_fss_RecoveryCompleteShadowCopySet_send(TALLOC_CTX *mem_ctx,
								 struct tevent_context *ev,
								 struct dcerpc_binding_handle *h,
								 struct GUID _ShadowCopySetId /* [in]  */);
NTSTATUS dcerpc_fss_RecoveryCompleteShadowCopySet_recv(struct tevent_req *req,
						       TALLOC_CTX *mem_ctx,
						       uint32_t *result);
NTSTATUS dcerpc_fss_RecoveryCompleteShadowCopySet(struct dcerpc_binding_handle *h,
						  TALLOC_CTX *mem_ctx,
						  struct GUID _ShadowCopySetId /* [in]  */,
						  uint32_t *result);

struct tevent_req *dcerpc_fss_AbortShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_AbortShadowCopySet *r);
NTSTATUS dcerpc_fss_AbortShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_AbortShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_AbortShadowCopySet *r);
struct tevent_req *dcerpc_fss_AbortShadowCopySet_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      struct GUID _ShadowCopySetId /* [in]  */);
NTSTATUS dcerpc_fss_AbortShadowCopySet_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *result);
NTSTATUS dcerpc_fss_AbortShadowCopySet(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       struct GUID _ShadowCopySetId /* [in]  */,
				       uint32_t *result);

struct tevent_req *dcerpc_fss_IsPathSupported_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_IsPathSupported *r);
NTSTATUS dcerpc_fss_IsPathSupported_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_IsPathSupported_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_IsPathSupported *r);
struct tevent_req *dcerpc_fss_IsPathSupported_send(TALLOC_CTX *mem_ctx,
						   struct tevent_context *ev,
						   struct dcerpc_binding_handle *h,
						   const char *_ShareName /* [in] [charset(UTF16),ref] */,
						   uint8_t _magic[52] /* [in]  */,
						   uint32_t *_SupportedByThisProvider /* [out] [ref] */,
						   const char **_OwnerMachineName /* [out] [charset(UTF16),ref] */);
NTSTATUS dcerpc_fss_IsPathSupported_recv(struct tevent_req *req,
					 TALLOC_CTX *mem_ctx,
					 uint32_t *result);
NTSTATUS dcerpc_fss_IsPathSupported(struct dcerpc_binding_handle *h,
				    TALLOC_CTX *mem_ctx,
				    const char *_ShareName /* [in] [charset(UTF16),ref] */,
				    uint8_t _magic[52] /* [in]  */,
				    uint32_t *_SupportedByThisProvider /* [out] [ref] */,
				    const char **_OwnerMachineName /* [out] [charset(UTF16),ref] */,
				    uint32_t *result);

struct tevent_req *dcerpc_fss_IsPathShadowCopied_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_IsPathShadowCopied *r);
NTSTATUS dcerpc_fss_IsPathShadowCopied_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_IsPathShadowCopied_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_IsPathShadowCopied *r);
struct tevent_req *dcerpc_fss_IsPathShadowCopied_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      const char *_ShareName /* [in] [ref,charset(UTF16)] */,
						      uint32_t *_ShadowCopyPresent /* [out] [ref] */,
						      int32_t *_ShadowCopyCompatibility /* [out] [ref] */);
NTSTATUS dcerpc_fss_IsPathShadowCopied_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *result);
NTSTATUS dcerpc_fss_IsPathShadowCopied(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       const char *_ShareName /* [in] [ref,charset(UTF16)] */,
				       uint32_t *_ShadowCopyPresent /* [out] [ref] */,
				       int32_t *_ShadowCopyCompatibility /* [out] [ref] */,
				       uint32_t *result);

struct tevent_req *dcerpc_fss_GetShareMapping_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_GetShareMapping *r);
NTSTATUS dcerpc_fss_GetShareMapping_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_GetShareMapping_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_GetShareMapping *r);
struct tevent_req *dcerpc_fss_GetShareMapping_send(TALLOC_CTX *mem_ctx,
						   struct tevent_context *ev,
						   struct dcerpc_binding_handle *h,
						   struct GUID _ShadowCopyId /* [in]  */,
						   struct GUID _ShadowCopySetId /* [in]  */,
						   const char *_ShareName /* [in] [ref,charset(UTF16)] */,
						   uint32_t _Level /* [in]  */,
						   union fssagent_share_mapping *_ShareMapping /* [out] [ref,switch_is(Level)] */);
NTSTATUS dcerpc_fss_GetShareMapping_recv(struct tevent_req *req,
					 TALLOC_CTX *mem_ctx,
					 uint32_t *result);
NTSTATUS dcerpc_fss_GetShareMapping(struct dcerpc_binding_handle *h,
				    TALLOC_CTX *mem_ctx,
				    struct GUID _ShadowCopyId /* [in]  */,
				    struct GUID _ShadowCopySetId /* [in]  */,
				    const char *_ShareName /* [in] [ref,charset(UTF16)] */,
				    uint32_t _Level /* [in]  */,
				    union fssagent_share_mapping *_ShareMapping /* [out] [ref,switch_is(Level)] */,
				    uint32_t *result);

struct tevent_req *dcerpc_fss_DeleteShareMapping_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_DeleteShareMapping *r);
NTSTATUS dcerpc_fss_DeleteShareMapping_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_DeleteShareMapping_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_DeleteShareMapping *r);
struct tevent_req *dcerpc_fss_DeleteShareMapping_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      struct GUID _ShadowCopySetId /* [in]  */,
						      struct GUID _ShadowCopyId /* [in]  */,
						      const char *_ShareName /* [in] [ref,charset(UTF16)] */);
NTSTATUS dcerpc_fss_DeleteShareMapping_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *result);
NTSTATUS dcerpc_fss_DeleteShareMapping(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       struct GUID _ShadowCopySetId /* [in]  */,
				       struct GUID _ShadowCopyId /* [in]  */,
				       const char *_ShareName /* [in] [ref,charset(UTF16)] */,
				       uint32_t *result);

struct tevent_req *dcerpc_fss_PrepareShadowCopySet_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct fss_PrepareShadowCopySet *r);
NTSTATUS dcerpc_fss_PrepareShadowCopySet_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_fss_PrepareShadowCopySet_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct fss_PrepareShadowCopySet *r);
struct tevent_req *dcerpc_fss_PrepareShadowCopySet_send(TALLOC_CTX *mem_ctx,
							struct tevent_context *ev,
							struct dcerpc_binding_handle *h,
							struct GUID _ShadowCopySetId /* [in]  */,
							uint32_t _TimeOutInMilliseconds /* [in]  */);
NTSTATUS dcerpc_fss_PrepareShadowCopySet_recv(struct tevent_req *req,
					      TALLOC_CTX *mem_ctx,
					      uint32_t *result);
NTSTATUS dcerpc_fss_PrepareShadowCopySet(struct dcerpc_binding_handle *h,
					 TALLOC_CTX *mem_ctx,
					 struct GUID _ShadowCopySetId /* [in]  */,
					 uint32_t _TimeOutInMilliseconds /* [in]  */,
					 uint32_t *result);

#endif /* _HEADER_RPC_FileServerVssAgent */
