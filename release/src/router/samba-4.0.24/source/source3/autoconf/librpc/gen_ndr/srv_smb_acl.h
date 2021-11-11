#include "autoconf/librpc/gen_ndr/ndr_smb_acl.h"
#ifndef __SRV_SMB_ACL__
#define __SRV_SMB_ACL__
void smb_acl_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_smb_acl_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_smb_acl_shutdown(void);
#endif /* __SRV_SMB_ACL__ */
