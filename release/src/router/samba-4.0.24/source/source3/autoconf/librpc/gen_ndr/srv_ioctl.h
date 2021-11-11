#include "autoconf/librpc/gen_ndr/ndr_ioctl.h"
#ifndef __SRV_COPYCHUNK__
#define __SRV_COPYCHUNK__
void copychunk_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_copychunk_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_copychunk_shutdown(void);
#endif /* __SRV_COPYCHUNK__ */
