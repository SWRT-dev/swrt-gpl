#include "autoconf/librpc/gen_ndr/ndr_fsrvp.h"
#ifndef __SRV_FILESERVERVSSAGENT__
#define __SRV_FILESERVERVSSAGENT__
uint32 _fss_GetSupportedVersion(struct pipes_struct *p, struct fss_GetSupportedVersion *r);
uint32 _fss_SetContext(struct pipes_struct *p, struct fss_SetContext *r);
uint32 _fss_StartShadowCopySet(struct pipes_struct *p, struct fss_StartShadowCopySet *r);
uint32 _fss_AddToShadowCopySet(struct pipes_struct *p, struct fss_AddToShadowCopySet *r);
uint32 _fss_CommitShadowCopySet(struct pipes_struct *p, struct fss_CommitShadowCopySet *r);
uint32 _fss_ExposeShadowCopySet(struct pipes_struct *p, struct fss_ExposeShadowCopySet *r);
uint32 _fss_RecoveryCompleteShadowCopySet(struct pipes_struct *p, struct fss_RecoveryCompleteShadowCopySet *r);
uint32 _fss_AbortShadowCopySet(struct pipes_struct *p, struct fss_AbortShadowCopySet *r);
uint32 _fss_IsPathSupported(struct pipes_struct *p, struct fss_IsPathSupported *r);
uint32 _fss_IsPathShadowCopied(struct pipes_struct *p, struct fss_IsPathShadowCopied *r);
uint32 _fss_GetShareMapping(struct pipes_struct *p, struct fss_GetShareMapping *r);
uint32 _fss_DeleteShareMapping(struct pipes_struct *p, struct fss_DeleteShareMapping *r);
uint32 _fss_PrepareShadowCopySet(struct pipes_struct *p, struct fss_PrepareShadowCopySet *r);
void FileServerVssAgent_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_FileServerVssAgent_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_FileServerVssAgent_shutdown(void);
#endif /* __SRV_FILESERVERVSSAGENT__ */
