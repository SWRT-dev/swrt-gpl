/******************************************************************************

                               Copyright (c) 2006
                            Infineon Technologies AG
                     Am Campeon 1-12; 81726 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE,
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP,
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL
  PROPERTY INFRINGEMENT.

  EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

******************************************************************************/
#if defined(CONFIG_IFX_UDP_REDIRECT) || defined(CONFIG_IFX_UDP_REDIRECT_MODULE)
/* ============================= */
/* Includes                      */
/* ============================= */
#include <net/checksum.h>
#include <net/udp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/udp_redirect.h>

/* ============================= */
/* Global variable definition    */
/* ============================= */
int (*udpredirect_getfrag_fn) (void *p, char * to, int offset,
                               int fraglen, int odd, struct sk_buff *skb) = NULL;
int (*udp_do_redirect_fn)(struct sock *sk, struct sk_buff *skb) = NULL;

/* ============================= */
/* Local type definitions        */
/* ============================= */
struct udpfakehdr
{
  struct udphdr uh;
  u32 saddr;
  u32 daddr;
  struct iovec *iov;
  u32 wcheck;
};

/* ============================= */
/* Local function declaration    */
/* ============================= */
static int udpredirect_csum_partial_copy_fromiovecend(unsigned char *kdata,
              struct iovec *iov, int offset, unsigned int len, __wsum *csump);

static int udpredirect_memcpy_fromiovecend(unsigned char *kdata, struct iovec *iov, int offset,
                        int len);

/* ============================= */
/* Global function definition    */
/* ============================= */

/*
   Copy of udp_getfrag() from udp.c
   This function exists because no copy_from_user() is needed for udpredirect.
*/

int
udpredirect_getfrag(void *from, char *to, int offset, int len, int odd, struct sk_buff *skb)
{
   struct iovec *iov = from;

        if (skb->ip_summed == CHECKSUM_PARTIAL) {
                if (udpredirect_memcpy_fromiovecend(to, iov, offset, len) < 0)
                        return -EFAULT;
        } else {
                __wsum csum = 0;
                if (udpredirect_csum_partial_copy_fromiovecend(to, iov, offset, len, &csum) < 0)
                        return -EFAULT;
                skb->csum = csum_block_add(skb->csum, csum, odd);
        }
        return 0;
}

static int udpredirect_memcpy_fromiovecend(unsigned char *kdata, struct iovec *iov, int offset,
                        int len)
{
        /* Skip over the finished iovecs */
        while (offset >= iov->iov_len) {
                offset -= iov->iov_len;
                iov++;
        }

        while (len > 0) {
                u8 __user *base = iov->iov_base + offset;
                int copy = min_t(unsigned int, len, iov->iov_len - offset);

                offset = 0;
                memcpy(kdata, base, copy);
                len -= copy;
                kdata += copy;
                iov++;
        }

        return 0;
}

/*
   Copy of csum_partial_copy_fromiovecend() from iovec.c
   This function exists because no copy_from_user() is needed for udpredirect.
*/

int udpredirect_csum_partial_copy_fromiovecend(unsigned char *kdata, struct iovec *iov,
				 int offset, unsigned int len, __wsum *csump)
{
	__wsum csum = *csump;
	int partial_cnt = 0, err = 0;

	/* Skip over the finished iovecs */
	while (offset >= iov->iov_len) {
		offset -= iov->iov_len;
		iov++;
	}

	while (len > 0) {
		u8 __user *base = iov->iov_base + offset;
		int copy = min_t(unsigned int, len, iov->iov_len - offset);

		offset = 0;

		/* There is a remnant from previous iov. */
		if (partial_cnt) {
			int par_len = 4 - partial_cnt;

			/* iov component is too short ... */
			if (par_len > copy) {
				memcpy(kdata, base, copy);
				kdata += copy;
				base  += copy;
				partial_cnt += copy;
				len   -= copy;
				iov++;
				if (len)
					continue;
				*csump = csum_partial(kdata - partial_cnt,
							 partial_cnt, csum);
				goto out;
			}
			memcpy(kdata, base, par_len);
			csum = csum_partial(kdata - partial_cnt, 4, csum);
			kdata += par_len;
			base  += par_len;
			copy  -= par_len;
			len   -= par_len;
			partial_cnt = 0;
		}

		if (len > copy) {
			partial_cnt = copy % 4;
			if (partial_cnt) {
				copy -= partial_cnt;
				memcpy(kdata + copy, base + copy, partial_cnt);
			}
		}

		if (copy) {
			csum = csum_partial_copy_nocheck(base, kdata, copy, csum);
		}
		len   -= copy + partial_cnt;
		kdata += copy + partial_cnt;
		iov++;
	}
        *csump = csum;
out:
	return err;
}

EXPORT_SYMBOL(udpredirect_getfrag);
EXPORT_SYMBOL(udp_do_redirect_fn);
EXPORT_SYMBOL(udpredirect_getfrag_fn);
#endif /* CONFIG_IFX_UDP_REDIRECT* */
