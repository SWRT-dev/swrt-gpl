#include "librpc/rpc/dcerpc.h"
#include "autoconf/librpc/gen_ndr/dnsserver.h"
#ifndef _HEADER_RPC_dnsserver
#define _HEADER_RPC_dnsserver

extern const struct ndr_interface_table ndr_table_dnsserver;

struct tevent_req *dcerpc_DnssrvOperation_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvOperation *r);
NTSTATUS dcerpc_DnssrvOperation_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvOperation_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvOperation *r);
struct tevent_req *dcerpc_DnssrvOperation_send(TALLOC_CTX *mem_ctx,
					       struct tevent_context *ev,
					       struct dcerpc_binding_handle *h,
					       const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
					       const char *_pszZone /* [in] [unique,charset(UTF8)] */,
					       uint32_t _dwContext /* [in]  */,
					       const char *_pszOperation /* [in] [charset(UTF8),unique] */,
					       enum DNS_RPC_TYPEID _dwTypeId /* [in]  */,
					       union DNSSRV_RPC_UNION _pData /* [in] [switch_is(dwTypeId)] */);
NTSTATUS dcerpc_DnssrvOperation_recv(struct tevent_req *req,
				     TALLOC_CTX *mem_ctx,
				     WERROR *result);
NTSTATUS dcerpc_DnssrvOperation(struct dcerpc_binding_handle *h,
				TALLOC_CTX *mem_ctx,
				const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
				const char *_pszZone /* [in] [unique,charset(UTF8)] */,
				uint32_t _dwContext /* [in]  */,
				const char *_pszOperation /* [in] [charset(UTF8),unique] */,
				enum DNS_RPC_TYPEID _dwTypeId /* [in]  */,
				union DNSSRV_RPC_UNION _pData /* [in] [switch_is(dwTypeId)] */,
				WERROR *result);

struct tevent_req *dcerpc_DnssrvQuery_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvQuery *r);
NTSTATUS dcerpc_DnssrvQuery_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvQuery_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvQuery *r);
struct tevent_req *dcerpc_DnssrvQuery_send(TALLOC_CTX *mem_ctx,
					   struct tevent_context *ev,
					   struct dcerpc_binding_handle *h,
					   const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
					   const char *_pszZone /* [in] [charset(UTF8),unique] */,
					   const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
					   enum DNS_RPC_TYPEID *_pdwTypeId /* [out] [ref] */,
					   union DNSSRV_RPC_UNION *_ppData /* [out] [ref,switch_is(*pdwTypeId)] */);
NTSTATUS dcerpc_DnssrvQuery_recv(struct tevent_req *req,
				 TALLOC_CTX *mem_ctx,
				 WERROR *result);
NTSTATUS dcerpc_DnssrvQuery(struct dcerpc_binding_handle *h,
			    TALLOC_CTX *mem_ctx,
			    const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
			    const char *_pszZone /* [in] [charset(UTF8),unique] */,
			    const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
			    enum DNS_RPC_TYPEID *_pdwTypeId /* [out] [ref] */,
			    union DNSSRV_RPC_UNION *_ppData /* [out] [ref,switch_is(*pdwTypeId)] */,
			    WERROR *result);

struct tevent_req *dcerpc_DnssrvComplexOperation_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvComplexOperation *r);
NTSTATUS dcerpc_DnssrvComplexOperation_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvComplexOperation_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvComplexOperation *r);
struct tevent_req *dcerpc_DnssrvComplexOperation_send(TALLOC_CTX *mem_ctx,
						      struct tevent_context *ev,
						      struct dcerpc_binding_handle *h,
						      const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
						      const char *_pszZone /* [in] [unique,charset(UTF8)] */,
						      const char *_pszOperation /* [in] [charset(UTF8),unique] */,
						      enum DNS_RPC_TYPEID _dwTypeIn /* [in]  */,
						      union DNSSRV_RPC_UNION _pDataIn /* [in] [switch_is(dwTypeIn)] */,
						      enum DNS_RPC_TYPEID *_pdwTypeOut /* [out] [ref] */,
						      union DNSSRV_RPC_UNION *_ppDataOut /* [out] [switch_is(*pdwTypeOut),ref] */);
NTSTATUS dcerpc_DnssrvComplexOperation_recv(struct tevent_req *req,
					    TALLOC_CTX *mem_ctx,
					    WERROR *result);
NTSTATUS dcerpc_DnssrvComplexOperation(struct dcerpc_binding_handle *h,
				       TALLOC_CTX *mem_ctx,
				       const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
				       const char *_pszZone /* [in] [unique,charset(UTF8)] */,
				       const char *_pszOperation /* [in] [charset(UTF8),unique] */,
				       enum DNS_RPC_TYPEID _dwTypeIn /* [in]  */,
				       union DNSSRV_RPC_UNION _pDataIn /* [in] [switch_is(dwTypeIn)] */,
				       enum DNS_RPC_TYPEID *_pdwTypeOut /* [out] [ref] */,
				       union DNSSRV_RPC_UNION *_ppDataOut /* [out] [switch_is(*pdwTypeOut),ref] */,
				       WERROR *result);

struct tevent_req *dcerpc_DnssrvEnumRecords_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvEnumRecords *r);
NTSTATUS dcerpc_DnssrvEnumRecords_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvEnumRecords_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvEnumRecords *r);
struct tevent_req *dcerpc_DnssrvEnumRecords_send(TALLOC_CTX *mem_ctx,
						 struct tevent_context *ev,
						 struct dcerpc_binding_handle *h,
						 const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
						 const char *_pszZone /* [in] [charset(UTF8),unique] */,
						 const char *_pszNodeName /* [in] [unique,charset(UTF8)] */,
						 const char *_pszStartChild /* [in] [charset(UTF8),unique] */,
						 enum dns_record_type _wRecordType /* [in]  */,
						 uint32_t _fSelectFlag /* [in]  */,
						 const char *_pszFilterStart /* [in] [unique,charset(UTF8)] */,
						 const char *_pszFilterStop /* [in] [charset(UTF8),unique] */,
						 uint32_t *_pdwBufferLength /* [out] [ref] */,
						 struct DNS_RPC_RECORDS_ARRAY **_pBuffer /* [out] [subcontext_size(*pdwBufferLength),subcontext(4),ref] */);
NTSTATUS dcerpc_DnssrvEnumRecords_recv(struct tevent_req *req,
				       TALLOC_CTX *mem_ctx,
				       WERROR *result);
NTSTATUS dcerpc_DnssrvEnumRecords(struct dcerpc_binding_handle *h,
				  TALLOC_CTX *mem_ctx,
				  const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
				  const char *_pszZone /* [in] [charset(UTF8),unique] */,
				  const char *_pszNodeName /* [in] [unique,charset(UTF8)] */,
				  const char *_pszStartChild /* [in] [charset(UTF8),unique] */,
				  enum dns_record_type _wRecordType /* [in]  */,
				  uint32_t _fSelectFlag /* [in]  */,
				  const char *_pszFilterStart /* [in] [unique,charset(UTF8)] */,
				  const char *_pszFilterStop /* [in] [charset(UTF8),unique] */,
				  uint32_t *_pdwBufferLength /* [out] [ref] */,
				  struct DNS_RPC_RECORDS_ARRAY **_pBuffer /* [out] [subcontext_size(*pdwBufferLength),subcontext(4),ref] */,
				  WERROR *result);

struct tevent_req *dcerpc_DnssrvUpdateRecord_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvUpdateRecord *r);
NTSTATUS dcerpc_DnssrvUpdateRecord_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvUpdateRecord_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvUpdateRecord *r);
struct tevent_req *dcerpc_DnssrvUpdateRecord_send(TALLOC_CTX *mem_ctx,
						  struct tevent_context *ev,
						  struct dcerpc_binding_handle *h,
						  const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
						  const char *_pszZone /* [in] [charset(UTF8),unique] */,
						  const char *_pszNodeName /* [in] [charset(UTF8),unique] */,
						  struct DNS_RPC_RECORD_BUF *_pAddRecord /* [in] [unique] */,
						  struct DNS_RPC_RECORD_BUF *_pDeleteRecord /* [in] [unique] */);
NTSTATUS dcerpc_DnssrvUpdateRecord_recv(struct tevent_req *req,
					TALLOC_CTX *mem_ctx,
					WERROR *result);
NTSTATUS dcerpc_DnssrvUpdateRecord(struct dcerpc_binding_handle *h,
				   TALLOC_CTX *mem_ctx,
				   const char *_pwszServerName /* [in] [charset(UTF16),unique] */,
				   const char *_pszZone /* [in] [charset(UTF8),unique] */,
				   const char *_pszNodeName /* [in] [charset(UTF8),unique] */,
				   struct DNS_RPC_RECORD_BUF *_pAddRecord /* [in] [unique] */,
				   struct DNS_RPC_RECORD_BUF *_pDeleteRecord /* [in] [unique] */,
				   WERROR *result);

struct tevent_req *dcerpc_DnssrvOperation2_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvOperation2 *r);
NTSTATUS dcerpc_DnssrvOperation2_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvOperation2_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvOperation2 *r);
struct tevent_req *dcerpc_DnssrvOperation2_send(TALLOC_CTX *mem_ctx,
						struct tevent_context *ev,
						struct dcerpc_binding_handle *h,
						enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
						uint32_t _dwSettingFlags /* [in]  */,
						const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
						const char *_pszZone /* [in] [unique,charset(UTF8)] */,
						uint32_t _dwContext /* [in]  */,
						const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
						enum DNS_RPC_TYPEID _dwTypeId /* [in]  */,
						union DNSSRV_RPC_UNION _pData /* [in] [switch_is(dwTypeId)] */);
NTSTATUS dcerpc_DnssrvOperation2_recv(struct tevent_req *req,
				      TALLOC_CTX *mem_ctx,
				      WERROR *result);
NTSTATUS dcerpc_DnssrvOperation2(struct dcerpc_binding_handle *h,
				 TALLOC_CTX *mem_ctx,
				 enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
				 uint32_t _dwSettingFlags /* [in]  */,
				 const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
				 const char *_pszZone /* [in] [unique,charset(UTF8)] */,
				 uint32_t _dwContext /* [in]  */,
				 const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
				 enum DNS_RPC_TYPEID _dwTypeId /* [in]  */,
				 union DNSSRV_RPC_UNION _pData /* [in] [switch_is(dwTypeId)] */,
				 WERROR *result);

struct tevent_req *dcerpc_DnssrvQuery2_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvQuery2 *r);
NTSTATUS dcerpc_DnssrvQuery2_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvQuery2_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvQuery2 *r);
struct tevent_req *dcerpc_DnssrvQuery2_send(TALLOC_CTX *mem_ctx,
					    struct tevent_context *ev,
					    struct dcerpc_binding_handle *h,
					    enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
					    uint32_t _dwSettingFlags /* [in]  */,
					    const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
					    const char *_pszZone /* [in] [unique,charset(UTF8)] */,
					    const char *_pszOperation /* [in] [charset(UTF8),unique] */,
					    enum DNS_RPC_TYPEID *_pdwTypeId /* [out] [ref] */,
					    union DNSSRV_RPC_UNION *_ppData /* [out] [ref,switch_is(*pdwTypeId)] */);
NTSTATUS dcerpc_DnssrvQuery2_recv(struct tevent_req *req,
				  TALLOC_CTX *mem_ctx,
				  WERROR *result);
NTSTATUS dcerpc_DnssrvQuery2(struct dcerpc_binding_handle *h,
			     TALLOC_CTX *mem_ctx,
			     enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
			     uint32_t _dwSettingFlags /* [in]  */,
			     const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
			     const char *_pszZone /* [in] [unique,charset(UTF8)] */,
			     const char *_pszOperation /* [in] [charset(UTF8),unique] */,
			     enum DNS_RPC_TYPEID *_pdwTypeId /* [out] [ref] */,
			     union DNSSRV_RPC_UNION *_ppData /* [out] [ref,switch_is(*pdwTypeId)] */,
			     WERROR *result);

struct tevent_req *dcerpc_DnssrvComplexOperation2_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvComplexOperation2 *r);
NTSTATUS dcerpc_DnssrvComplexOperation2_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvComplexOperation2_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvComplexOperation2 *r);
struct tevent_req *dcerpc_DnssrvComplexOperation2_send(TALLOC_CTX *mem_ctx,
						       struct tevent_context *ev,
						       struct dcerpc_binding_handle *h,
						       enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
						       uint32_t _dwSettingFlags /* [in]  */,
						       const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
						       const char *_pszZone /* [in] [charset(UTF8),unique] */,
						       const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
						       enum DNS_RPC_TYPEID _dwTypeIn /* [in]  */,
						       union DNSSRV_RPC_UNION _pDataIn /* [in] [switch_is(dwTypeIn)] */,
						       enum DNS_RPC_TYPEID *_pdwTypeOut /* [out] [ref] */,
						       union DNSSRV_RPC_UNION *_ppDataOut /* [out] [ref,switch_is(*pdwTypeOut)] */);
NTSTATUS dcerpc_DnssrvComplexOperation2_recv(struct tevent_req *req,
					     TALLOC_CTX *mem_ctx,
					     WERROR *result);
NTSTATUS dcerpc_DnssrvComplexOperation2(struct dcerpc_binding_handle *h,
					TALLOC_CTX *mem_ctx,
					enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
					uint32_t _dwSettingFlags /* [in]  */,
					const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
					const char *_pszZone /* [in] [charset(UTF8),unique] */,
					const char *_pszOperation /* [in] [unique,charset(UTF8)] */,
					enum DNS_RPC_TYPEID _dwTypeIn /* [in]  */,
					union DNSSRV_RPC_UNION _pDataIn /* [in] [switch_is(dwTypeIn)] */,
					enum DNS_RPC_TYPEID *_pdwTypeOut /* [out] [ref] */,
					union DNSSRV_RPC_UNION *_ppDataOut /* [out] [ref,switch_is(*pdwTypeOut)] */,
					WERROR *result);

struct tevent_req *dcerpc_DnssrvEnumRecords2_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvEnumRecords2 *r);
NTSTATUS dcerpc_DnssrvEnumRecords2_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvEnumRecords2_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvEnumRecords2 *r);
struct tevent_req *dcerpc_DnssrvEnumRecords2_send(TALLOC_CTX *mem_ctx,
						  struct tevent_context *ev,
						  struct dcerpc_binding_handle *h,
						  enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
						  uint32_t _dwSettingFlags /* [in]  */,
						  const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
						  const char *_pszZone /* [in] [unique,charset(UTF8)] */,
						  const char *_pszNodeName /* [in] [charset(UTF8),unique] */,
						  const char *_pszStartChild /* [in] [unique,charset(UTF8)] */,
						  enum dns_record_type _wRecordType /* [in]  */,
						  uint32_t _fSelectFlag /* [in]  */,
						  const char *_pszFilterStart /* [in] [unique,charset(UTF8)] */,
						  const char *_pszFilterStop /* [in] [unique,charset(UTF8)] */,
						  uint32_t *_pdwBufferLength /* [out] [ref] */,
						  struct DNS_RPC_RECORDS_ARRAY **_pBuffer /* [out] [subcontext_size(*pdwBufferLength),ref,subcontext(4)] */);
NTSTATUS dcerpc_DnssrvEnumRecords2_recv(struct tevent_req *req,
					TALLOC_CTX *mem_ctx,
					WERROR *result);
NTSTATUS dcerpc_DnssrvEnumRecords2(struct dcerpc_binding_handle *h,
				   TALLOC_CTX *mem_ctx,
				   enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
				   uint32_t _dwSettingFlags /* [in]  */,
				   const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
				   const char *_pszZone /* [in] [unique,charset(UTF8)] */,
				   const char *_pszNodeName /* [in] [charset(UTF8),unique] */,
				   const char *_pszStartChild /* [in] [unique,charset(UTF8)] */,
				   enum dns_record_type _wRecordType /* [in]  */,
				   uint32_t _fSelectFlag /* [in]  */,
				   const char *_pszFilterStart /* [in] [unique,charset(UTF8)] */,
				   const char *_pszFilterStop /* [in] [unique,charset(UTF8)] */,
				   uint32_t *_pdwBufferLength /* [out] [ref] */,
				   struct DNS_RPC_RECORDS_ARRAY **_pBuffer /* [out] [subcontext_size(*pdwBufferLength),ref,subcontext(4)] */,
				   WERROR *result);

struct tevent_req *dcerpc_DnssrvUpdateRecord2_r_send(TALLOC_CTX *mem_ctx,
	struct tevent_context *ev,
	struct dcerpc_binding_handle *h,
	struct DnssrvUpdateRecord2 *r);
NTSTATUS dcerpc_DnssrvUpdateRecord2_r_recv(struct tevent_req *req, TALLOC_CTX *mem_ctx);
NTSTATUS dcerpc_DnssrvUpdateRecord2_r(struct dcerpc_binding_handle *h, TALLOC_CTX *mem_ctx, struct DnssrvUpdateRecord2 *r);
struct tevent_req *dcerpc_DnssrvUpdateRecord2_send(TALLOC_CTX *mem_ctx,
						   struct tevent_context *ev,
						   struct dcerpc_binding_handle *h,
						   enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
						   uint32_t _dwSettingFlags /* [in]  */,
						   const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
						   const char *_pszZone /* [in] [unique,charset(UTF8)] */,
						   const char *_pszNodeName /* [in] [charset(UTF8),ref] */,
						   struct DNS_RPC_RECORD_BUF *_pAddRecord /* [in] [unique] */,
						   struct DNS_RPC_RECORD_BUF *_pDeleteRecord /* [in] [unique] */);
NTSTATUS dcerpc_DnssrvUpdateRecord2_recv(struct tevent_req *req,
					 TALLOC_CTX *mem_ctx,
					 WERROR *result);
NTSTATUS dcerpc_DnssrvUpdateRecord2(struct dcerpc_binding_handle *h,
				    TALLOC_CTX *mem_ctx,
				    enum DNS_RPC_CLIENT_VERSION _dwClientVersion /* [in]  */,
				    uint32_t _dwSettingFlags /* [in]  */,
				    const char *_pwszServerName /* [in] [unique,charset(UTF16)] */,
				    const char *_pszZone /* [in] [unique,charset(UTF8)] */,
				    const char *_pszNodeName /* [in] [charset(UTF8),ref] */,
				    struct DNS_RPC_RECORD_BUF *_pAddRecord /* [in] [unique] */,
				    struct DNS_RPC_RECORD_BUF *_pDeleteRecord /* [in] [unique] */,
				    WERROR *result);

#endif /* _HEADER_RPC_dnsserver */
