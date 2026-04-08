#ifndef __BP_CRYPTO_SKCIPHER_H
#define __BP_CRYPTO_SKCIPHER_H
#include_next <crypto/skcipher.h>

#if LINUX_VERSION_IS_LESS(4,6,0)
static inline void skcipher_request_zero(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);

	memzero_explicit(req, sizeof(*req) + crypto_skcipher_reqsize(tfm));
}
#endif

#endif /* __BP_CRYPTO_SKCIPHER_H */
