#include "autoconf/librpc/gen_ndr/ndr_dnsserver.h"
#ifndef __SRV_DNSSERVER__
#define __SRV_DNSSERVER__
WERROR _DnssrvOperation(struct pipes_struct *p, struct DnssrvOperation *r);
WERROR _DnssrvQuery(struct pipes_struct *p, struct DnssrvQuery *r);
WERROR _DnssrvComplexOperation(struct pipes_struct *p, struct DnssrvComplexOperation *r);
WERROR _DnssrvEnumRecords(struct pipes_struct *p, struct DnssrvEnumRecords *r);
WERROR _DnssrvUpdateRecord(struct pipes_struct *p, struct DnssrvUpdateRecord *r);
WERROR _DnssrvOperation2(struct pipes_struct *p, struct DnssrvOperation2 *r);
WERROR _DnssrvQuery2(struct pipes_struct *p, struct DnssrvQuery2 *r);
WERROR _DnssrvComplexOperation2(struct pipes_struct *p, struct DnssrvComplexOperation2 *r);
WERROR _DnssrvEnumRecords2(struct pipes_struct *p, struct DnssrvEnumRecords2 *r);
WERROR _DnssrvUpdateRecord2(struct pipes_struct *p, struct DnssrvUpdateRecord2 *r);
void dnsserver_get_pipe_fns(struct api_struct **fns, int *n_fns);
struct rpc_srv_callbacks;
NTSTATUS rpc_dnsserver_init(const struct rpc_srv_callbacks *rpc_srv_cb);
NTSTATUS rpc_dnsserver_shutdown(void);
#endif /* __SRV_DNSSERVER__ */
