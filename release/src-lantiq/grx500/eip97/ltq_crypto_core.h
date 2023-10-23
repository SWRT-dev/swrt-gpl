/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <crypto/hash.h>
#include "api_dmabuf.h"

/* membase top:  0x1E100000 */
/* membase top2: 0x1E10F800 */
extern void __iomem *ltq_crypto_membase;

#define ltq_crypto_wr32(x, y)	writel(x, ltq_crypto_membase + (y))
#define ltq_crypto_r32(x)		readl(ltq_crypto_membase + (x))


u32 endian_swap(u32 input);

void dma_alloc_properties(DMABuf_Properties_t *dma,
                                int fcached, int size,
                                int alignment,
                                int bank);

int ltq_hmac_precompute(u32 *inner, u32 *outer,
                            struct crypto_shash *hash,
                            unsigned int base_hash_ds,
                            const u8 *key, unsigned int keylen,
                            bool need_hash,
                            void (*copykeys) (u32 *inner, u32 *outer,
                                    void *istate,
                                    void *ostate,
                                    unsigned int len));

struct crypto_shash *ltq_init_hmac(struct crypto_tfm *tfm,
                    char *base_hash_name);

void ltq_sha512_hmac_copy_keys(u32 *ipad, u32 *opad,
                            void *istate,
                            void *ostate,
                            unsigned int len);


void ltq_sha256_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len);

void ltq_sha224_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len);

void ltq_md5_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len);

void ltq_sha1_hmac_copy_keys(u32 *ipad, u32 *opad,
							void *istate,
							void *ostate,
							unsigned int len);


int ltq_gcm_precompute_keys(u8 *out, const u8 *key, unsigned int keylen);

int ltq_xcbc_precompute(u8 *key1, u8 *key2, u8 *key3, const u8 *key,
                unsigned int keylen);
int ltq_ipsec_init(unsigned int ring, int irq);
void ltq_ipsec_exit(unsigned int ring);

