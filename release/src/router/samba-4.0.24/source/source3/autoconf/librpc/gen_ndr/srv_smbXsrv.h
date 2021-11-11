#include "autoconf/librpc/gen_ndr/ndr_smbXsrv.h"
#ifndef __SRV_SMBXSRV__
#define __SRV_SMBXSRV__
void _smbXsrv_version_global_decode(struct pipes_struct *p, struct smbXsrv_version_global_decode *r);
void _smbXsrv_session_global_decode(struct pipes_struct *p, struct smbXsrv_session_global_decode *r);
void _smbXsrv_session_decode(struct pipes_struct *p, struct smbXsrv_session_decode *r);
void _smbXsrv_session_close_decode(struct pipes_struct *p, struct smbXsrv_session_close_decode *r);
void _smbXsrv_tcon_global_decode(struct pipes_struct *p, struct smbXsrv_tcon_global_decode *r);
void _smbXsrv_tcon_decode(struct pipes_struct *p, struct smbXsrv_tcon_decode *r);
void _smbXsrv_open_global_decode(struct pipes_struct *p, struct smbXsrv_open_global_decode *r);
void _smbXsrv_open_decode(struct pipes_struct *p, struct smbXsrv_open_decode *r);
void smbXsrv_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_smbXsrv_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_smbXsrv_shutdown(void);
#endif /* __SRV_SMBXSRV__ */
