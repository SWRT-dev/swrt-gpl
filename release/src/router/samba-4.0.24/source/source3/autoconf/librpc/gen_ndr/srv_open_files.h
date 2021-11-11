#include "autoconf/librpc/gen_ndr/ndr_open_files.h"
#ifndef __SRV_OPEN_FILES__
#define __SRV_OPEN_FILES__
void open_files_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_open_files_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_open_files_shutdown(void);
#endif /* __SRV_OPEN_FILES__ */
