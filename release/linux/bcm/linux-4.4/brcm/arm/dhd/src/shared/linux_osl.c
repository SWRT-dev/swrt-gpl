/*
 * Linux OS Independent Layer
 *
 * Copyright (C) 2016, Broadcom. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: linux_osl.c 613984 2016-01-20 19:24:03Z $
 */

#define LINUX_PORT

#include <typedefs.h>
#include <bcmendian.h>
#include <linuxver.h>
#include <bcmdefs.h>

#ifdef mips
#include <asm/paccess.h>
#include <asm/cache.h>
#include <asm/r4kcache.h>
#undef ABS
#endif /* mips */

#if !defined(STBLINUX)
#if defined(__ARM_ARCH_7A__) && !defined(DHD_USE_COHERENT_MEM_FOR_RING)
#include <asm/cacheflush.h>
#endif /* __ARM_ARCH_7A__ && !DHD_USE_COHERENT_MEM_FOR_RING */
#endif /* STBLINUX */

#include <linux/random.h>

#include <osl.h>
#include <bcmutils.h>
#include <linux/delay.h>
#include <pcicfg.h>


#ifdef BCM_SECURE_DMA
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <asm/io.h>
#include <linux/skbuff.h>
#include <linux/vmalloc.h>
#include <stbutils.h>
#include <linux/highmem.h>
#include <linux/dma-mapping.h>
#include <asm/memory.h>
#if defined(__ARM_ARCH_7A__)
#include <arch/arm/include/asm/tlbflush.h>
#endif
#endif /* BCM_SECURE_DMA */

#include <linux/fs.h>
#ifdef BCM_NBUFF
#include <linux/netfilter_bridge/ebt_ftos_t.h>
#include <dhd_nbuff.h>
#endif
#ifdef BCM_BLOG
#include <dhd_blog.h>
#endif
#ifdef BCM47XX_CA9
#include <linux/spinlock.h>
extern spinlock_t l2x0_reg_lock;
#endif /* BCM47XX_CA9 */

#ifdef BCM_OBJECT_TRACE
#include <bcmutils.h>
#endif /* BCM_OBJECT_TRACE */

#define PCI_CFG_RETRY		10

#define OS_HANDLE_MAGIC		0x1234abcd	/* Magic # to recognize osh */
#define BCM_MEM_FILENAME_LEN	24		/* Mem. filename length */
#define DUMPBUFSZ 1024

#ifdef CONFIG_DHD_USE_STATIC_BUF
#define DHD_SKB_HDRSIZE		336
#define DHD_SKB_1PAGE_BUFSIZE	((PAGE_SIZE*1)-DHD_SKB_HDRSIZE)
#define DHD_SKB_2PAGE_BUFSIZE	((PAGE_SIZE*2)-DHD_SKB_HDRSIZE)
#define DHD_SKB_4PAGE_BUFSIZE	((PAGE_SIZE*4)-DHD_SKB_HDRSIZE)

#define STATIC_BUF_MAX_NUM	16
#define STATIC_BUF_SIZE	(PAGE_SIZE*2)
#define STATIC_BUF_TOTAL_LEN	(STATIC_BUF_MAX_NUM * STATIC_BUF_SIZE)

typedef struct bcm_static_buf {
	struct semaphore static_sem;
	unsigned char *buf_ptr;
	unsigned char buf_use[STATIC_BUF_MAX_NUM];
} bcm_static_buf_t;

static bcm_static_buf_t *bcm_static_buf = 0;

#define STATIC_PKT_MAX_NUM	8
#if defined(ENHANCED_STATIC_BUF)
#define STATIC_PKT_4PAGE_NUM	1
#define DHD_SKB_MAX_BUFSIZE	DHD_SKB_4PAGE_BUFSIZE
#else
#define STATIC_PKT_4PAGE_NUM	0
#define DHD_SKB_MAX_BUFSIZE DHD_SKB_2PAGE_BUFSIZE
#endif /* ENHANCED_STATIC_BUF */

typedef struct bcm_static_pkt {
	struct sk_buff *skb_4k[STATIC_PKT_MAX_NUM];
	struct sk_buff *skb_8k[STATIC_PKT_MAX_NUM];
#ifdef ENHANCED_STATIC_BUF
	struct sk_buff *skb_16k;
#endif
	struct semaphore osl_pkt_sem;
	unsigned char pkt_use[STATIC_PKT_MAX_NUM * 2 + STATIC_PKT_4PAGE_NUM];
} bcm_static_pkt_t;

static bcm_static_pkt_t *bcm_static_skb = 0;

void* wifi_platform_prealloc(void *adapter, int section, unsigned long size);
#endif /* CONFIG_DHD_USE_STATIC_BUF */

typedef struct bcm_mem_link {
	struct bcm_mem_link *prev;
	struct bcm_mem_link *next;
	uint	size;
	int	line;
	void 	*osh;
	char	file[BCM_MEM_FILENAME_LEN];
} bcm_mem_link_t;

struct osl_cmn_info {
	atomic_t malloced;
	atomic_t pktalloced;    /* Number of allocated packet buffers */
	spinlock_t dbgmem_lock;
	bcm_mem_link_t *dbgmem_list;
	spinlock_t pktalloc_lock;
	atomic_t refcount; /* Number of references to this shared structure. */
};
typedef struct osl_cmn_info osl_cmn_t;

struct osl_info {
	osl_pubinfo_t pub;
	uint32  flags;		/* If specific cases to be handled in the OSL */
#ifdef CTFPOOL
	ctfpool_t *ctfpool;
#endif /* CTFPOOL */
	uint magic;
	void *pdev;
	uint failed;
	uint bustype;
	osl_cmn_t *cmn; /* Common OSL related data shred between two OSH's */

	void *bus_handle;
#if defined(DSLCPE_DELAY)
	shared_osl_t *oshsh;		/* osh shared */
#endif
#ifdef BCMDBG_CTRACE
	spinlock_t ctrace_lock;
	struct list_head ctrace_list;
	int ctrace_num;
#endif /* BCMDBG_CTRACE */
#ifdef	BCM_SECURE_DMA
	struct sec_mem_elem *sec_list_512;
	struct sec_mem_elem *sec_list_base_512;
	struct sec_mem_elem *sec_list_2048;
	struct sec_mem_elem *sec_list_base_2048;
	struct sec_mem_elem *sec_list_4096;
	struct sec_mem_elem *sec_list_base_4096;
	phys_addr_t  contig_base;
	void *contig_base_va;
	phys_addr_t  contig_base_alloc;
	void *contig_base_alloc_va;
	phys_addr_t contig_base_alloc_coherent;
	void *contig_base_alloc_coherent_va;
	phys_addr_t contig_delta_va_pa;
	struct {
		phys_addr_t pa;
		void *va;
		bool avail;
	} sec_cma_coherent[SEC_CMA_COHERENT_MAX];
	int stb_ext_params;
#endif /* BCM_SECURE_DMA */
};

#ifdef BCM_SECURE_DMA
static void * osl_sec_dma_ioremap(osl_t *osh, struct page *page, size_t size,
	bool iscache, bool isdecr);
static void osl_sec_dma_iounmap(osl_t *osh, void *contig_base_va, size_t size);
static void osl_sec_dma_init_elem_mem_block(osl_t *osh, size_t mbsize, int max,
	sec_mem_elem_t **list);
static void osl_sec_dma_deinit_elem_mem_block(osl_t *osh, size_t mbsize, int max,
	void *sec_list_base);
static sec_mem_elem_t * osl_sec_dma_alloc_mem_elem(osl_t *osh, void *va, uint size,
	int direction, struct sec_cma_info *ptr_cma_info, uint offset);
static void osl_sec_dma_free_mem_elem(osl_t *osh, sec_mem_elem_t *sec_mem_elem);
static void osl_sec_dma_init_consistent(osl_t *osh);
static void *osl_sec_dma_alloc_consistent(osl_t *osh, uint size, uint16 align_bits,
	ulong *pap);
static void osl_sec_dma_free_consistent(osl_t *osh, void *va, uint size, dmaaddr_t pa);
#endif /* BCM_SECURE_DMA */

#ifdef BCM_OBJECT_TRACE
/* don't clear the first 4 byte that is the pkt sn */
#define OSL_PKTTAG_CLEAR(p) \
do { \
	struct sk_buff *s = (struct sk_buff *)(p); \
	ASSERT(OSL_PKTTAG_SZ == 32); \
	*(uint32 *)(&s->cb[4]) = 0; \
	*(uint32 *)(&s->cb[8]) = 0; *(uint32 *)(&s->cb[12]) = 0; \
	*(uint32 *)(&s->cb[16]) = 0; *(uint32 *)(&s->cb[20]) = 0; \
	*(uint32 *)(&s->cb[24]) = 0; *(uint32 *)(&s->cb[28]) = 0; \
} while (0)
#else
#define OSL_PKTTAG_CLEAR(p) \
do { \
	struct sk_buff *s = (struct sk_buff *)(p); \
	ASSERT(OSL_PKTTAG_SZ == 32); \
	*(uint32 *)(&s->cb[0]) = 0; *(uint32 *)(&s->cb[4]) = 0; \
	*(uint32 *)(&s->cb[8]) = 0; *(uint32 *)(&s->cb[12]) = 0; \
	*(uint32 *)(&s->cb[16]) = 0; *(uint32 *)(&s->cb[20]) = 0; \
	*(uint32 *)(&s->cb[24]) = 0; *(uint32 *)(&s->cb[28]) = 0; \
} while (0)
#endif /* BCM_OBJECT_TRACE */

/* PCMCIA attribute space access macros */
#if defined(CONFIG_PCMCIA) || defined(CONFIG_PCMCIA_MODULE)
struct pcmcia_dev {
	dev_link_t link;	/* PCMCIA device pointer */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
	dev_node_t node;	/* PCMCIA node structure */
#endif
	void *base;		/* Mapped attribute memory window */
	size_t size;		/* Size of window */
	void *drv;		/* Driver data */
};
#endif /* defined(CONFIG_PCMCIA) || defined(CONFIG_PCMCIA_MODULE) */

/* Global ASSERT type flag */
uint32 g_assert_type = 1;
module_param(g_assert_type, int, 0);

#ifdef	BCM_SECURE_DMA
#define	SECDMA_MODULE_PARAMS	0
#define	SECDMA_EXT_FILE	1

unsigned long secdma_addr = 0;
unsigned long secdma_addr2 = 0;
u32 secdma_size = 0;
u32 secdma_size2 = 0;
module_param(secdma_addr, ulong, 0);
module_param(secdma_size, int, 0);
module_param(secdma_addr2, ulong, 0);
module_param(secdma_size2, int, 0);
static int secdma_found = 0;
#endif /* BCM_SECURE_DMA */

static int16 linuxbcmerrormap[] =
{	0, 			/* 0 */
	-EINVAL,		/* BCME_ERROR */
	-EINVAL,		/* BCME_BADARG */
	-EINVAL,		/* BCME_BADOPTION */
	-EINVAL,		/* BCME_NOTUP */
	-EINVAL,		/* BCME_NOTDOWN */
	-EINVAL,		/* BCME_NOTAP */
	-EINVAL,		/* BCME_NOTSTA */
	-EINVAL,		/* BCME_BADKEYIDX */
	-EINVAL,		/* BCME_RADIOOFF */
	-EINVAL,		/* BCME_NOTBANDLOCKED */
	-EINVAL, 		/* BCME_NOCLK */
	-EINVAL, 		/* BCME_BADRATESET */
	-EINVAL, 		/* BCME_BADBAND */
	-E2BIG,			/* BCME_BUFTOOSHORT */
	-E2BIG,			/* BCME_BUFTOOLONG */
	-EBUSY, 		/* BCME_BUSY */
	-EINVAL, 		/* BCME_NOTASSOCIATED */
	-EINVAL, 		/* BCME_BADSSIDLEN */
	-EINVAL, 		/* BCME_OUTOFRANGECHAN */
	-EINVAL, 		/* BCME_BADCHAN */
	-EFAULT, 		/* BCME_BADADDR */
	-ENOMEM, 		/* BCME_NORESOURCE */
	-EOPNOTSUPP,		/* BCME_UNSUPPORTED */
	-EMSGSIZE,		/* BCME_BADLENGTH */
	-EINVAL,		/* BCME_NOTREADY */
	-EPERM,			/* BCME_EPERM */
	-ENOMEM, 		/* BCME_NOMEM */
	-EINVAL, 		/* BCME_ASSOCIATED */
	-ERANGE, 		/* BCME_RANGE */
	-EINVAL, 		/* BCME_NOTFOUND */
	-EINVAL, 		/* BCME_WME_NOT_ENABLED */
	-EINVAL, 		/* BCME_TSPEC_NOTFOUND */
	-EINVAL, 		/* BCME_ACM_NOTSUPPORTED */
	-EINVAL,		/* BCME_NOT_WME_ASSOCIATION */
	-EIO,			/* BCME_SDIO_ERROR */
	-ENODEV,		/* BCME_DONGLE_DOWN */
	-EINVAL,		/* BCME_VERSION */
	-EIO,			/* BCME_TXFAIL */
	-EIO,			/* BCME_RXFAIL */
	-ENODEV,		/* BCME_NODEVICE */
	-EINVAL,		/* BCME_NMODE_DISABLED */
	-ENODATA,		/* BCME_NONRESIDENT */
	-EINVAL,		/* BCME_SCANREJECT */
	-EINVAL,		/* BCME_USAGE_ERROR */
	-EIO,     		/* BCME_IOCTL_ERROR */
	-EIO,			/* BCME_SERIAL_PORT_ERR */
	-EOPNOTSUPP,	/* BCME_DISABLED, BCME_NOTENABLED */
	-EIO,			/* BCME_DECERR */
	-EIO,			/* BCME_ENCERR */
	-EIO,			/* BCME_MICERR */
	-ERANGE,		/* BCME_REPLAY */
	-EINVAL,		/* BCME_IE_NOTFOUND */
	-EINVAL,		/* BCME_DATA_NOTFOUND */

/* When an new error code is added to bcmutils.h, add os
 * specific error translation here as well
 */
/* check if BCME_LAST changed since the last time this function was updated */
#if BCME_LAST != -53
#error "You need to add a OS error translation in the linuxbcmerrormap \
	for new error code defined in bcmutils.h"
#endif
};
uint lmtest = FALSE;

/* translate bcmerrors into linux errors */
int
osl_error(int bcmerror)
{
	if (bcmerror > 0)
		bcmerror = 0;
	else if (bcmerror < BCME_LAST)
		bcmerror = BCME_ERROR;

	/* Array bounds covered by ASSERT in osl_attach */
	return linuxbcmerrormap[-bcmerror];
}
#ifdef SHARED_OSL_CMN
osl_t *
osl_attach(void *pdev, uint bustype, bool pkttag, void **osl_cmn)
{
#else
osl_t *
osl_attach(void *pdev, uint bustype, bool pkttag)
{
	void **osl_cmn = NULL;
#endif /* SHARED_OSL_CMN */
	osl_t *osh;
	gfp_t flags;

#ifdef BCM_SECURE_DMA
	u32 secdma_memsize;
#endif

	flags = CAN_SLEEP() ? GFP_KERNEL: GFP_ATOMIC;
	if (!(osh = kmalloc(sizeof(osl_t), flags)))
		return osh;

	ASSERT(osh);

	bzero(osh, sizeof(osl_t));

	if (osl_cmn == NULL || *osl_cmn == NULL) {
		if (!(osh->cmn = kmalloc(sizeof(osl_cmn_t), flags))) {
			kfree(osh);
			return NULL;
		}
		bzero(osh->cmn, sizeof(osl_cmn_t));
		if (osl_cmn)
			*osl_cmn = osh->cmn;
		atomic_set(&osh->cmn->malloced, 0);
		osh->cmn->dbgmem_list = NULL;
		spin_lock_init(&(osh->cmn->dbgmem_lock));

		spin_lock_init(&(osh->cmn->pktalloc_lock));

	} else {
		osh->cmn = *osl_cmn;
	}
	atomic_add(1, &osh->cmn->refcount);

	bcm_object_trace_init();

	/* Check that error map has the right number of entries in it */
	ASSERT(ABS(BCME_LAST) == (ARRAYSIZE(linuxbcmerrormap) - 1));

	osh->failed = 0;
	osh->pdev = pdev;
	osh->pub.pkttag = pkttag;
	osh->bustype = bustype;
	osh->magic = OS_HANDLE_MAGIC;
#ifdef BCM_SECURE_DMA

	if ((secdma_addr != 0) && (secdma_size != 0)) {
		printk("linux_osl.c: Buffer info passed via module params, using it.\n");

		if (secdma_found == 0) {
			osh->contig_base_alloc = (phys_addr_t)secdma_addr;
			secdma_memsize = secdma_size;
		} else if (secdma_found == 1) {
			osh->contig_base_alloc = (phys_addr_t)secdma_addr2;
			secdma_memsize = secdma_size2;
		} else {
			printk("linux_osl.c secdma: secDMA instances %d \n", secdma_found);
			return NULL;
		}

		osh->contig_base = (phys_addr_t)osh->contig_base_alloc;

		printf("linux_osl.c: secdma_cma_size = 0x%x\n", secdma_memsize);
		printf("linux_osl.c: secdma_cma_addr = 0x%x \n",
			(unsigned int)osh->contig_base_alloc);
		osh->stb_ext_params = SECDMA_MODULE_PARAMS;
	}
	else if (stbpriv_init(osh) == 0) {
		printk("linux_osl.c: stbpriv.txt found. Get buffer info.\n");

		if (secdma_found == 0) {
			osh->contig_base_alloc =
				(phys_addr_t)bcm_strtoul(stbparam_get("secdma_cma_addr"), NULL, 0);
			secdma_memsize = bcm_strtoul(stbparam_get("secdma_cma_size"), NULL, 0);
		} else if (secdma_found == 1) {
			osh->contig_base_alloc =
				(phys_addr_t)bcm_strtoul(stbparam_get("secdma_cma_addr2"), NULL, 0);
			secdma_memsize = bcm_strtoul(stbparam_get("secdma_cma_size2"), NULL, 0);
		} else {
			printk("linux_osl.c secdma: secDMA instances %d \n", secdma_found);
			return NULL;
		}

		osh->contig_base = (phys_addr_t)osh->contig_base_alloc;

		printf("linux_osl.c: secdma_cma_size = 0x%x\n", secdma_memsize);
		printf("linux_osl.c: secdma_cma_addr = 0x%x \n",
			(unsigned int)osh->contig_base_alloc);
		osh->stb_ext_params = SECDMA_EXT_FILE;
	}
	else {
		printk("linux_osl.c: secDMA no longer supports internal buffer allocation.\n");
		kfree(osh);
		return NULL;
	}

	secdma_found++;
#ifdef BCM47XX_CA9
	osh->contig_base_alloc_coherent_va = osl_sec_dma_ioremap(osh,
		phys_to_page((u32)osh->contig_base_alloc),
		CMA_DMA_DESC_MEMBLOCK, TRUE, TRUE);
#else
	osh->contig_base_alloc_coherent_va = osl_sec_dma_ioremap(osh,
		phys_to_page((u32)osh->contig_base_alloc),
		CMA_DMA_DESC_MEMBLOCK, FALSE, TRUE);
#endif /* BCM47XX_CA9 */

	osh->contig_base_alloc_coherent = osh->contig_base_alloc;
	osl_sec_dma_init_consistent(osh);

	osh->contig_base_alloc += CMA_DMA_DESC_MEMBLOCK;

	osh->contig_base_alloc_va = osl_sec_dma_ioremap(osh,
		phys_to_page((u32)osh->contig_base_alloc), CMA_DMA_DATA_MEMBLOCK, TRUE, FALSE);
	osh->contig_base_va = osh->contig_base_alloc_va;

	/*
	* osl_sec_dma_init_elem_mem_block(osh, CMA_BUFSIZE_512, CMA_BUFNUM, &osh->sec_list_512);
	* osh->sec_list_base_512 = osh->sec_list_512;
	* osl_sec_dma_init_elem_mem_block(osh, CMA_BUFSIZE_2K, CMA_BUFNUM, &osh->sec_list_2048);
	* osh->sec_list_base_2048 = osh->sec_list_2048;
	*/
	osl_sec_dma_init_elem_mem_block(osh, CMA_BUFSIZE_4K, CMA_BUFNUM, &osh->sec_list_4096);
	osh->sec_list_base_4096 = osh->sec_list_4096;

#endif /* BCM_SECURE_DMA */

	switch (bustype) {
		case PCI_BUS:
		case SI_BUS:
		case PCMCIA_BUS:
			osh->pub.mmbus = TRUE;
			break;
		case JTAG_BUS:
		case SDIO_BUS:
		case USB_BUS:
		case SPI_BUS:
		case RPC_BUS:
			osh->pub.mmbus = FALSE;
			break;
		default:
			ASSERT(FALSE);
			break;
	}

#ifdef BCMDBG_CTRACE
	spin_lock_init(&osh->ctrace_lock);
	INIT_LIST_HEAD(&osh->ctrace_list);
	osh->ctrace_num = 0;
#endif /* BCMDBG_CTRACE */


	return osh;
}

int osl_static_mem_init(osl_t *osh, void *adapter)
{
#ifdef CONFIG_DHD_USE_STATIC_BUF
		if (!bcm_static_buf && adapter) {
			if (!(bcm_static_buf = (bcm_static_buf_t *)wifi_platform_prealloc(adapter,
				3, STATIC_BUF_SIZE + STATIC_BUF_TOTAL_LEN))) {
				printk("can not alloc static buf!\n");
				bcm_static_skb = NULL;
				ASSERT(osh->magic == OS_HANDLE_MAGIC);
				kfree(osh);
				return -ENOMEM;
			}
			else
				printk("alloc static buf at %x!\n", (unsigned int)bcm_static_buf);


			sema_init(&bcm_static_buf->static_sem, 1);

			bcm_static_buf->buf_ptr = (unsigned char *)bcm_static_buf + STATIC_BUF_SIZE;
		}

		if (!bcm_static_skb && adapter) {
			int i;
			void *skb_buff_ptr = 0;
			bcm_static_skb = (bcm_static_pkt_t *)((char *)bcm_static_buf + 2048);
			skb_buff_ptr = wifi_platform_prealloc(adapter, 4, 0);
			if (!skb_buff_ptr) {
				printk("cannot alloc static buf!\n");
				bcm_static_buf = NULL;
				bcm_static_skb = NULL;
				ASSERT(osh->magic == OS_HANDLE_MAGIC);
				kfree(osh);
				return -ENOMEM;
			}

			bcopy(skb_buff_ptr, bcm_static_skb, sizeof(struct sk_buff *) *
				(STATIC_PKT_MAX_NUM * 2 + STATIC_PKT_4PAGE_NUM));
			for (i = 0; i < STATIC_PKT_MAX_NUM * 2 + STATIC_PKT_4PAGE_NUM; i++)
				bcm_static_skb->pkt_use[i] = 0;

			sema_init(&bcm_static_skb->osl_pkt_sem, 1);
		}
#endif /* CONFIG_DHD_USE_STATIC_BUF */

	return 0;
}

void osl_set_bus_handle(osl_t *osh, void *bus_handle)
{
	osh->bus_handle = bus_handle;
}

void* osl_get_bus_handle(osl_t *osh)
{
	return osh->bus_handle;
}

void
osl_detach(osl_t *osh)
{
	if (osh == NULL)
		return;

#ifdef BCM_SECURE_DMA
	if (osh->stb_ext_params == SECDMA_EXT_FILE)
		stbpriv_exit(osh);

	osl_sec_dma_deinit_elem_mem_block(osh, CMA_BUFSIZE_512, CMA_BUFNUM, osh->sec_list_base_512);
	osl_sec_dma_deinit_elem_mem_block(osh, CMA_BUFSIZE_2K, CMA_BUFNUM, osh->sec_list_base_2048);
	osl_sec_dma_deinit_elem_mem_block(osh, CMA_BUFSIZE_4K, CMA_BUFNUM, osh->sec_list_base_4096);
	osl_sec_dma_iounmap(osh, osh->contig_base_va, CMA_MEMBLOCK);

	secdma_found--;
#endif /* BCM_SECURE_DMA */


	bcm_object_trace_deinit();

	ASSERT(osh->magic == OS_HANDLE_MAGIC);
	atomic_sub(1, &osh->cmn->refcount);
	if (atomic_read(&osh->cmn->refcount) == 0) {
			kfree(osh->cmn);
	}
	kfree(osh);
}

int osl_static_mem_deinit(osl_t *osh, void *adapter)
{
#ifdef CONFIG_DHD_USE_STATIC_BUF
	if (bcm_static_buf) {
		bcm_static_buf = 0;
	}
	if (bcm_static_skb) {
		bcm_static_skb = 0;
	}
#endif
	return 0;
}

/*
 * To avoid ACP latency, a fwder buf will be sent directly to DDR using
 * DDR aliasing into non-ACP address space. Such Fwder buffers must be
 * explicitly managed from a coherency perspective.
 */
static inline void BCMFASTPATH
osl_fwderbuf_reset(osl_t *osh, struct sk_buff *skb)
{
#if defined(BCM_GMAC3) && defined(BCM47XX_CA9)
	if (osl_is_flag_set(osh, OSL_FWDERBUF)) { /* Fwder OSH */
		if (!PKTISFWDERBUF(osh, skb)) { /* Initialize a Fwder Buf packet */
#if defined(__ARM_ARCH_7A__)
			uint8 *data = skb->head + NET_SKB_PAD;
#else
			uint8 *data = skb->head + 16;
#endif
			OSL_CACHE_INV_NOACP(data, osh->ctfpool->obj_size);
			PKTSETFWDERBUF(osh, skb);
		}
	}
#endif /* BCM_GMAC3 && BCM47XX_CA9 */
}

static struct sk_buff *osl_alloc_skb(osl_t *osh, unsigned int len)
{
	struct sk_buff *skb;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25)
	gfp_t flags = (in_atomic() || irqs_disabled()) ? GFP_ATOMIC : GFP_KERNEL;
#if defined(CONFIG_SPARSEMEM) && defined(CONFIG_ZONE_DMA)
	flags |= GFP_ATOMIC;
#endif
#if defined(CUSTOMER_HW4)
	flags = GFP_ATOMIC;
#endif
	skb = __dev_alloc_skb(len, flags);
#else
	skb = dev_alloc_skb(len);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 25) */
#ifdef CTFMAP
	if (skb) {
		_DMA_MAP(osh, PKTDATA(osh, skb), len, DMA_RX, NULL, NULL);
	}
#endif /* CTFMAP */
	return skb;
}

#ifdef CTFPOOL

#ifdef CTFPOOL_SPINLOCK
#define CTFPOOL_LOCK(ctfpool, flags)	spin_lock_irqsave(&(ctfpool)->lock, flags)
#define CTFPOOL_UNLOCK(ctfpool, flags)	spin_unlock_irqrestore(&(ctfpool)->lock, flags)
#else
#define CTFPOOL_LOCK(ctfpool, flags)	spin_lock_bh(&(ctfpool)->lock)
#define CTFPOOL_UNLOCK(ctfpool, flags)	spin_unlock_bh(&(ctfpool)->lock)
#endif /* CTFPOOL_SPINLOCK */
/*
 * Allocate and add an object to packet pool.
 */
void *
osl_ctfpool_add(osl_t *osh)
{
	struct sk_buff *skb;
#ifdef CTFPOOL_SPINLOCK
	unsigned long flags;
#endif /* CTFPOOL_SPINLOCK */

	if ((osh == NULL) || (osh->ctfpool == NULL))
		return NULL;

	CTFPOOL_LOCK(osh->ctfpool, flags);
	ASSERT(osh->ctfpool->curr_obj <= osh->ctfpool->max_obj);

	/* No need to allocate more objects */
	if (osh->ctfpool->curr_obj == osh->ctfpool->max_obj) {
		CTFPOOL_UNLOCK(osh->ctfpool, flags);
		return NULL;
	}

	/* Allocate a new skb and add it to the ctfpool */
	skb = osl_alloc_skb(osh, osh->ctfpool->obj_size);
	if (skb == NULL) {
		printf("%s: skb alloc of len %d failed\n", __FUNCTION__,
		       osh->ctfpool->obj_size);
		CTFPOOL_UNLOCK(osh->ctfpool, flags);
		return NULL;
	}

	/* Add to ctfpool */
	skb->next = (struct sk_buff *)osh->ctfpool->head;
	osh->ctfpool->head = skb;
	osh->ctfpool->fast_frees++;
	osh->ctfpool->curr_obj++;

	/* Hijack a skb member to store ptr to ctfpool */
	CTFPOOLPTR(osh, skb) = (void *)osh->ctfpool;

	/* Use bit flag to indicate skb from fast ctfpool */
	PKTFAST(osh, skb) = FASTBUF;

	/* If ctfpool's osh is a fwder osh, reset the fwder buf */
	osl_fwderbuf_reset(osh->ctfpool->osh, skb);

	CTFPOOL_UNLOCK(osh->ctfpool, flags);

	return skb;
}

/*
 * Add new objects to the pool.
 */
void
osl_ctfpool_replenish(osl_t *osh, uint thresh)
{
	if ((osh == NULL) || (osh->ctfpool == NULL))
		return;

	/* Do nothing if no refills are required */
	while ((osh->ctfpool->refills > 0) && (thresh--)) {
		osl_ctfpool_add(osh);
		osh->ctfpool->refills--;
	}
}

/*
 * Initialize the packet pool with specified number of objects.
 */
int32
osl_ctfpool_init(osl_t *osh, uint numobj, uint size)
{
	gfp_t flags;

	flags = CAN_SLEEP() ? GFP_KERNEL: GFP_ATOMIC;
	osh->ctfpool = kzalloc(sizeof(ctfpool_t), flags);
	ASSERT(osh->ctfpool);

	osh->ctfpool->osh = osh;

	osh->ctfpool->max_obj = numobj;
	osh->ctfpool->obj_size = size;

	spin_lock_init(&osh->ctfpool->lock);

	while (numobj--) {
		if (!osl_ctfpool_add(osh))
			return -1;
		osh->ctfpool->fast_frees--;
	}

	return 0;
}

/*
 * Cleanup the packet pool objects.
 */
void
osl_ctfpool_cleanup(osl_t *osh)
{
	struct sk_buff *skb, *nskb;
#ifdef CTFPOOL_SPINLOCK
	unsigned long flags;
#endif /* CTFPOOL_SPINLOCK */

	if ((osh == NULL) || (osh->ctfpool == NULL))
		return;

	CTFPOOL_LOCK(osh->ctfpool, flags);

	skb = osh->ctfpool->head;

	while (skb != NULL) {
		nskb = skb->next;
		dev_kfree_skb(skb);
		skb = nskb;
		osh->ctfpool->curr_obj--;
	}

	ASSERT(osh->ctfpool->curr_obj == 0);
	osh->ctfpool->head = NULL;
	CTFPOOL_UNLOCK(osh->ctfpool, flags);

	kfree(osh->ctfpool);
	osh->ctfpool = NULL;
}

void
osl_ctfpool_stats(osl_t *osh, void *b)
{
	struct bcmstrbuf *bb;

	if ((osh == NULL) || (osh->ctfpool == NULL))
		return;

#ifdef CONFIG_DHD_USE_STATIC_BUF
	if (bcm_static_buf) {
		bcm_static_buf = 0;
	}
	if (bcm_static_skb) {
		bcm_static_skb = 0;
	}
#endif /* CONFIG_DHD_USE_STATIC_BUF */

	bb = b;

	ASSERT((osh != NULL) && (bb != NULL));

	bcm_bprintf(bb, "max_obj %d obj_size %d curr_obj %d refills %d\n",
	            osh->ctfpool->max_obj, osh->ctfpool->obj_size,
	            osh->ctfpool->curr_obj, osh->ctfpool->refills);
	bcm_bprintf(bb, "fast_allocs %d fast_frees %d slow_allocs %d\n",
	            osh->ctfpool->fast_allocs, osh->ctfpool->fast_frees,
	            osh->ctfpool->slow_allocs);
}

static inline struct sk_buff *
osl_pktfastget(osl_t *osh, uint len)
{
	struct sk_buff *skb;
#ifdef CTFPOOL_SPINLOCK
	unsigned long flags;
#endif /* CTFPOOL_SPINLOCK */

	/* Try to do fast allocate. Return null if ctfpool is not in use
	 * or if there are no items in the ctfpool.
	 */
	if (osh->ctfpool == NULL)
		return NULL;

	CTFPOOL_LOCK(osh->ctfpool, flags);
	if (osh->ctfpool->head == NULL) {
		ASSERT(osh->ctfpool->curr_obj == 0);
		osh->ctfpool->slow_allocs++;
		CTFPOOL_UNLOCK(osh->ctfpool, flags);
		return NULL;
	}

	if (len > osh->ctfpool->obj_size) {
		CTFPOOL_UNLOCK(osh->ctfpool, flags);
		return NULL;
	}

	ASSERT(len <= osh->ctfpool->obj_size);

	/* Get an object from ctfpool */
	skb = (struct sk_buff *)osh->ctfpool->head;
	osh->ctfpool->head = (void *)skb->next;

	osh->ctfpool->fast_allocs++;
	osh->ctfpool->curr_obj--;
	ASSERT(CTFPOOLHEAD(osh, skb) == (struct sock *)osh->ctfpool->head);
	CTFPOOL_UNLOCK(osh->ctfpool, flags);

	/* Init skb struct */
	skb->next = skb->prev = NULL;
#if defined(__ARM_ARCH_7A__)
	skb->data = skb->head + NET_SKB_PAD;
	skb->tail = skb->head + NET_SKB_PAD;
#else
	skb->data = skb->head + 16;
	skb->tail = skb->head + 16;
#endif /* __ARM_ARCH_7A__ */
	skb->len = 0;
	skb->cloned = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
	skb->list = NULL;
#endif
	atomic_set(&skb->users, 1);

	PKTSETCLINK(skb, NULL);
	PKTCCLRATTR(skb);
#ifdef CTFMAP
	PKTFAST(osh, skb) &= ~(CTFBUF | SKIPCT | CHAINED);
#else
	PKTFAST(osh, skb) &= ~(SKIPCT | CHAINED);
#endif
	return skb;
}
#endif /* CTFPOOL */

#if defined(BCM_GMAC3)
/* Account for a packet delivered to downstream forwarder.
 * Decrement a GMAC forwarder interface's pktalloced count.
 */
void BCMFASTPATH
osl_pkt_tofwder(osl_t *osh, void *skbs, int skb_cnt)
{

	atomic_sub(skb_cnt, &osh->cmn->pktalloced);
}

/* Account for a downstream forwarder delivered packet to a WL/DHD driver.
 * Increment a GMAC forwarder interface's pktalloced count.
 */
#ifdef BCMDBG_CTRACE
void BCMFASTPATH
osl_pkt_frmfwder(osl_t *osh, void *skbs, int skb_cnt, int line, char *file)
#else
void BCMFASTPATH
osl_pkt_frmfwder(osl_t *osh, void *skbs, int skb_cnt)
#endif /* BCMDBG_CTRACE */
{
#if defined(BCMDBG_CTRACE)
	int i;
	struct sk_buff *skb;
#endif 

#if defined(BCMDBG_CTRACE)
	if (skb_cnt > 1) {
		struct sk_buff **skb_array = (struct sk_buff **)skbs;
		for (i = 0; i < skb_cnt; i++) {
			skb = skb_array[i];
#if defined(BCMDBG_CTRACE)
			ASSERT(!PKTISCHAINED(skb));
			ADD_CTRACE(osh, skb, file, line);
#endif /* BCMDBG_CTRACE */
		}
	} else {
		skb = (struct sk_buff *)skbs;
#if defined(BCMDBG_CTRACE)
		ASSERT(!PKTISCHAINED(skb));
		ADD_CTRACE(osh, skb, file, line);
#endif /* BCMDBG_CTRACE */
	}
#endif 

	atomic_add(skb_cnt, &osh->cmn->pktalloced);
}

#endif /* BCM_GMAC3 */

/* Convert a driver packet to native(OS) packet
 * In the process, packettag is zeroed out before sending up
 * IP code depends on skb->cb to be setup correctly with various options
 * In our case, that means it should be 0
 */
struct sk_buff * BCMFASTPATH
osl_pkt_tonative(osl_t *osh, void *pkt)
{
	struct sk_buff *nskb;
#ifdef BCMDBG_CTRACE
	struct sk_buff *nskb1, *nskb2;
#endif

	if (osh->pub.pkttag)
		OSL_PKTTAG_CLEAR(pkt);

	/* Decrement the packet counter */
	for (nskb = (struct sk_buff *)pkt; nskb; nskb = nskb->next) {
		atomic_sub(PKTISCHAINED(nskb) ? PKTCCNT(nskb) : 1, &osh->cmn->pktalloced);

#ifdef BCMDBG_CTRACE
		for (nskb1 = nskb; nskb1 != NULL; nskb1 = nskb2) {
			if (PKTISCHAINED(nskb1)) {
				nskb2 = PKTCLINK(nskb1);
			}
			else
				nskb2 = NULL;

			DEL_CTRACE(osh, nskb1);
		}
#endif /* BCMDBG_CTRACE */
	}
	return (struct sk_buff *)pkt;
}

/* Convert a native(OS) packet to driver packet.
 * In the process, native packet is destroyed, there is no copying
 * Also, a packettag is zeroed out
 */
#ifdef BCMDBG_CTRACE
void * BCMFASTPATH
osl_pkt_frmnative(osl_t *osh, void *pkt, int line, char *file)
#else
void * BCMFASTPATH
osl_pkt_frmnative(osl_t *osh, void *pkt)
#endif /* BCMDBG_CTRACE */
{
	struct sk_buff *nskb;
#ifdef BCMDBG_CTRACE
	struct sk_buff *nskb1, *nskb2;
#endif

	if (osh->pub.pkttag)
		OSL_PKTTAG_CLEAR(pkt);

	/* Increment the packet counter */
	for (nskb = (struct sk_buff *)pkt; nskb; nskb = nskb->next) {
		atomic_add(PKTISCHAINED(nskb) ? PKTCCNT(nskb) : 1, &osh->cmn->pktalloced);

#ifdef BCMDBG_CTRACE
		for (nskb1 = nskb; nskb1 != NULL; nskb1 = nskb2) {
			if (PKTISCHAINED(nskb1)) {
				nskb2 = PKTCLINK(nskb1);
			}
			else
				nskb2 = NULL;

			ADD_CTRACE(osh, nskb1, file, line);
		}
#endif /* BCMDBG_CTRACE */
	}
	return (void *)pkt;
}

/* Return a new packet. zero out pkttag */
#ifdef BCMDBG_CTRACE
void * BCMFASTPATH
osl_pktget(osl_t *osh, uint len, int line, char *file)
#else
#ifdef BCM_OBJECT_TRACE
void * BCMFASTPATH
osl_pktget(osl_t *osh, uint len, int line, const char *caller)
#else
void * BCMFASTPATH
osl_pktget(osl_t *osh, uint len)
#endif /* BCM_OBJECT_TRACE */
#endif /* BCMDBG_CTRACE */
{
	struct sk_buff *skb;
	uchar num = 0;
	if (lmtest != FALSE) {
		get_random_bytes(&num, sizeof(uchar));
		if ((num + 1) <= (256 * lmtest / 100))
			return NULL;
	}

#ifdef CTFPOOL
	/* Allocate from local pool */
	skb = osl_pktfastget(osh, len);
	if ((skb != NULL) || ((skb = osl_alloc_skb(osh, len)) != NULL)) {
#else /* CTFPOOL */
	if ((skb = osl_alloc_skb(osh, len))) {
#endif /* CTFPOOL */
#ifdef BCMDBG
		skb_put(skb, len);
#else
		skb->tail += len;
		skb->len  += len;
#endif
		skb->priority = 0;

#ifdef BCMDBG_CTRACE
		ADD_CTRACE(osh, skb, file, line);
#endif
		atomic_inc(&osh->cmn->pktalloced);
#ifdef BCM_OBJECT_TRACE
		bcm_object_trace_opr(skb, BCM_OBJDBG_ADD_PKT, caller, line);
#endif /* BCM_OBJECT_TRACE */
	}

	return ((void*) skb);
}

#ifdef CTFPOOL
static inline void
osl_pktfastfree(osl_t *osh, struct sk_buff *skb)
{
	ctfpool_t *ctfpool;
#ifdef CTFPOOL_SPINLOCK
	unsigned long flags;
#endif /* CTFPOOL_SPINLOCK */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 14)
	skb->tstamp = ktime_set(0, 0);
#else
	skb->stamp.tv_sec = 0;
#endif

	/* We only need to init the fields that we change */
	skb->dev = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
	skb->dst = NULL;
#endif
	OSL_PKTTAG_CLEAR(skb);
	skb->ip_summed = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	skb_orphan(skb);
#else
	skb->destructor = NULL;
#endif

	ctfpool = (ctfpool_t *)CTFPOOLPTR(osh, skb);
	if (ctfpool == NULL) {
		__kfree_skb(skb);
		return;
	}

	/* if osh is a fwder osh, reset the fwder buf */
	osl_fwderbuf_reset(ctfpool->osh, skb);

	/* Add object to the ctfpool */
	CTFPOOL_LOCK(ctfpool, flags);
	skb->next = (struct sk_buff *)ctfpool->head;
	ctfpool->head = (void *)skb;

	ctfpool->fast_frees++;
	ctfpool->curr_obj++;

	ASSERT(ctfpool->curr_obj <= ctfpool->max_obj);
	CTFPOOL_UNLOCK(ctfpool, flags);
}
#endif /* CTFPOOL */

/* Free the driver packet. Free the tag if present */
#ifdef BCM_OBJECT_TRACE
void BCMFASTPATH
osl_pktfree(osl_t *osh, void *p, bool send, int line, const char *caller)
#else
void BCMFASTPATH
osl_pktfree(osl_t *osh, void *p, bool send)
#endif /* BCM_OBJECT_TRACE */
{
#ifdef BCM_NBUFF
	if (IS_FKBUFF_PTR(p)) {
	    nbuff_free((pNBuff_t)p);
	} else {
#endif
	struct sk_buff *skb, *nskb;
	if (osh == NULL)
		return;

	skb = (struct sk_buff*) p;

	if (send && osh->pub.tx_fn)
		osh->pub.tx_fn(osh->pub.tx_ctx, p, 0);

	PKTDBG_TRACE(osh, (void *) skb, PKTLIST_PKTFREE);

	/* perversion: we use skb->next to chain multi-skb packets */
	while (skb) {
		nskb = skb->next;
		skb->next = NULL;

#ifdef BCMDBG_CTRACE
		DEL_CTRACE(osh, skb);
#endif

#ifdef CTFMAP
		/* Clear the map ptr before freeing */
		PKTCLRCTF(osh, skb);
		CTFMAPPTR(osh, skb) = NULL;
#endif

#ifdef BCM_OBJECT_TRACE
		bcm_object_trace_opr(skb, BCM_OBJDBG_REMOVE, caller, line);
#endif /* BCM_OBJECT_TRACE */

#ifdef CTFPOOL
		if (PKTISFAST(osh, skb)) {
			if (atomic_read(&skb->users) == 1)
				smp_rmb();
			else if (!atomic_dec_and_test(&skb->users))
				goto next_skb;
			osl_pktfastfree(osh, skb);
		} else
#endif
		{
			if (skb->destructor)
				/* cannot kfree_skb() on hard IRQ (net/core/skbuff.c) if
				 * destructor exists
				 */
				dev_kfree_skb_any(skb);
			else
				/* can free immediately (even in_irq()) if destructor
				 * does not exist
				 */
				dev_kfree_skb(skb);
		}
#ifdef CTFPOOL
next_skb:
#endif
		atomic_dec(&osh->cmn->pktalloced);
		skb = nskb;
	}

#ifdef BCM_NBUFF
	} /* endif IS_FKBUFF_PTR */
#endif
}

#ifdef CONFIG_DHD_USE_STATIC_BUF
void*
osl_pktget_static(osl_t *osh, uint len)
{
	int i = 0;
	struct sk_buff *skb;

	if (!bcm_static_skb)
		return osl_pktget(osh, len);

	if (len > DHD_SKB_MAX_BUFSIZE) {
		printk("%s: attempt to allocate huge packet (0x%x)\n", __FUNCTION__, len);
		return osl_pktget(osh, len);
	}

	down(&bcm_static_skb->osl_pkt_sem);

	if (len <= DHD_SKB_1PAGE_BUFSIZE) {
		for (i = 0; i < STATIC_PKT_MAX_NUM; i++) {
			if (bcm_static_skb->pkt_use[i] == 0)
				break;
		}

		if (i != STATIC_PKT_MAX_NUM) {
			bcm_static_skb->pkt_use[i] = 1;

			skb = bcm_static_skb->skb_4k[i];
#ifdef NET_SKBUFF_DATA_USES_OFFSET
			skb_set_tail_pointer(skb, len);
#else
			skb->tail = skb->data + len;
#endif /* NET_SKBUFF_DATA_USES_OFFSET */
			skb->len = len;

			up(&bcm_static_skb->osl_pkt_sem);
			return skb;
		}
	}

	if (len <= DHD_SKB_2PAGE_BUFSIZE) {
		for (i = 0; i < STATIC_PKT_MAX_NUM; i++) {
			if (bcm_static_skb->pkt_use[i + STATIC_PKT_MAX_NUM]
				== 0)
				break;
		}

		if (i != STATIC_PKT_MAX_NUM) {
			bcm_static_skb->pkt_use[i + STATIC_PKT_MAX_NUM] = 1;
			skb = bcm_static_skb->skb_8k[i];
#ifdef NET_SKBUFF_DATA_USES_OFFSET
			skb_set_tail_pointer(skb, len);
#else
			skb->tail = skb->data + len;
#endif /* NET_SKBUFF_DATA_USES_OFFSET */
			skb->len = len;

			up(&bcm_static_skb->osl_pkt_sem);
			return skb;
		}
	}

#if defined(ENHANCED_STATIC_BUF)
	if (bcm_static_skb->pkt_use[STATIC_PKT_MAX_NUM * 2] == 0) {
		bcm_static_skb->pkt_use[STATIC_PKT_MAX_NUM * 2] = 1;

		skb = bcm_static_skb->skb_16k;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
		skb_set_tail_pointer(skb, len);
#else
		skb->tail = skb->data + len;
#endif /* NET_SKBUFF_DATA_USES_OFFSET */
		skb->len = len;

		up(&bcm_static_skb->osl_pkt_sem);
		return skb;
	}
#endif /* ENHANCED_STATIC_BUF */

	up(&bcm_static_skb->osl_pkt_sem);
	printk("%s: all static pkt in use!\n", __FUNCTION__);
	return osl_pktget(osh, len);
}

void
osl_pktfree_static(osl_t *osh, void *p, bool send)
{
	int i;
	if (!bcm_static_skb) {
		osl_pktfree(osh, p, send);
		return;
	}

	down(&bcm_static_skb->osl_pkt_sem);
	for (i = 0; i < STATIC_PKT_MAX_NUM; i++) {
		if (p == bcm_static_skb->skb_4k[i]) {
			bcm_static_skb->pkt_use[i] = 0;
			up(&bcm_static_skb->osl_pkt_sem);
			return;
		}
	}

	for (i = 0; i < STATIC_PKT_MAX_NUM; i++) {
		if (p == bcm_static_skb->skb_8k[i]) {
			bcm_static_skb->pkt_use[i + STATIC_PKT_MAX_NUM] = 0;
			up(&bcm_static_skb->osl_pkt_sem);
			return;
		}
	}
#ifdef ENHANCED_STATIC_BUF
	if (p == bcm_static_skb->skb_16k) {
		bcm_static_skb->pkt_use[STATIC_PKT_MAX_NUM * 2] = 0;
		up(&bcm_static_skb->osl_pkt_sem);
		return;
	}
#endif
	up(&bcm_static_skb->osl_pkt_sem);
	osl_pktfree(osh, p, send);
}
#endif /* CONFIG_DHD_USE_STATIC_BUF */

uint32
osl_pci_read_config(osl_t *osh, uint offset, uint size)
{
	uint val = 0;
	uint retry = PCI_CFG_RETRY;

	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));

	/* only 4byte access supported */
	ASSERT(size == 4);

	do {
		pci_read_config_dword(osh->pdev, offset, &val);
		if (val != 0xffffffff)
			break;
	} while (retry--);

#ifdef BCMDBG
	if (retry < PCI_CFG_RETRY)
		printk("PCI CONFIG READ access to %d required %d retries\n", offset,
		       (PCI_CFG_RETRY - retry));
#endif /* BCMDBG */

	return (val);
}

void
osl_pci_write_config(osl_t *osh, uint offset, uint size, uint val)
{
	uint retry = PCI_CFG_RETRY;

	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));

	/* only 4byte access supported */
	ASSERT(size == 4);

	do {
		pci_write_config_dword(osh->pdev, offset, val);
		if (offset != PCI_BAR0_WIN)
			break;
		if (osl_pci_read_config(osh, offset, size) == val)
			break;
	} while (retry--);

#ifdef BCMDBG
	if (retry < PCI_CFG_RETRY)
		printk("PCI CONFIG WRITE access to %d required %d retries\n", offset,
		       (PCI_CFG_RETRY - retry));
#endif /* BCMDBG */
}

/* return bus # for the pci device pointed by osh->pdev */
uint
osl_pci_bus(osl_t *osh)
{
	ASSERT(osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

#if (defined(__ARM_ARCH_7A__) && LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)) || \
	defined(BCA_HNDROUTER)
	return pci_domain_nr(((struct pci_dev *)osh->pdev)->bus);
#else
	return ((struct pci_dev *)osh->pdev)->bus->number;
#endif
}

/* return slot # for the pci device pointed by osh->pdev */
uint
osl_pci_slot(osl_t *osh)
{
	ASSERT(osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);
#if (defined(__ARM_ARCH_7A__) && LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)) || \
	defined(BCA_HNDROUTER)
	return PCI_SLOT(((struct pci_dev *)osh->pdev)->devfn) + 1;
#else
	return PCI_SLOT(((struct pci_dev *)osh->pdev)->devfn);
#endif
}

/* return domain # for the pci device pointed by osh->pdev */
uint
osl_pcie_domain(osl_t *osh)
{
	ASSERT(osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

	return pci_domain_nr(((struct pci_dev *)osh->pdev)->bus);
}

/* return bus # for the pci device pointed by osh->pdev */
uint
osl_pcie_bus(osl_t *osh)
{
	ASSERT(osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

	return ((struct pci_dev *)osh->pdev)->bus->number;
}

/* return the pci device pointed by osh->pdev */
struct pci_dev *
osl_pci_device(osl_t *osh)
{
	ASSERT(osh && (osh->magic == OS_HANDLE_MAGIC) && osh->pdev);

	return osh->pdev;
}

static void
osl_pcmcia_attr(osl_t *osh, uint offset, char *buf, int size, bool write)
{
}

void
osl_pcmcia_read_attr(osl_t *osh, uint offset, void *buf, int size)
{
	osl_pcmcia_attr(osh, offset, (char *) buf, size, FALSE);
}

void
osl_pcmcia_write_attr(osl_t *osh, uint offset, void *buf, int size)
{
	osl_pcmcia_attr(osh, offset, (char *) buf, size, TRUE);
}

void *
osl_malloc(osl_t *osh, uint size)
{
	void *addr;
	gfp_t flags;

	/* only ASSERT if osh is defined */
	if (osh)
		ASSERT(osh->magic == OS_HANDLE_MAGIC);
#ifdef CONFIG_DHD_USE_STATIC_BUF
	if (bcm_static_buf)
	{
		int i = 0;
		if ((size >= PAGE_SIZE)&&(size <= STATIC_BUF_SIZE))
		{
			down(&bcm_static_buf->static_sem);

			for (i = 0; i < STATIC_BUF_MAX_NUM; i++)
			{
				if (bcm_static_buf->buf_use[i] == 0)
					break;
			}

			if (i == STATIC_BUF_MAX_NUM)
			{
				up(&bcm_static_buf->static_sem);
				printk("all static buff in use!\n");
				goto original;
			}

			bcm_static_buf->buf_use[i] = 1;
			up(&bcm_static_buf->static_sem);

			bzero(bcm_static_buf->buf_ptr+STATIC_BUF_SIZE*i, size);
			if (osh)
				atomic_add(size, &osh->cmn->malloced);

			return ((void *)(bcm_static_buf->buf_ptr+STATIC_BUF_SIZE*i));
		}
	}
original:
#endif /* CONFIG_DHD_USE_STATIC_BUF */

	flags = CAN_SLEEP() ? GFP_KERNEL: GFP_ATOMIC;
	if ((addr = kmalloc(size, flags)) == NULL) {
		if (osh)
			osh->failed++;
		return (NULL);
	}
	if (osh && osh->cmn)
		atomic_add(size, &osh->cmn->malloced);

	return (addr);
}

void *
osl_mallocz(osl_t *osh, uint size)
{
	void *ptr;

	ptr = osl_malloc(osh, size);

	if (ptr != NULL) {
		bzero(ptr, size);
	}

	return ptr;
}

void
osl_mfree(osl_t *osh, void *addr, uint size)
{
#ifdef CONFIG_DHD_USE_STATIC_BUF
	if (bcm_static_buf)
	{
		if ((addr > (void *)bcm_static_buf) && ((unsigned char *)addr
			<= ((unsigned char *)bcm_static_buf + STATIC_BUF_TOTAL_LEN)))
		{
			int buf_idx = 0;

			buf_idx = ((unsigned char *)addr - bcm_static_buf->buf_ptr)/STATIC_BUF_SIZE;

			down(&bcm_static_buf->static_sem);
			bcm_static_buf->buf_use[buf_idx] = 0;
			up(&bcm_static_buf->static_sem);

			if (osh && osh->cmn) {
				ASSERT(osh->magic == OS_HANDLE_MAGIC);
				atomic_sub(size, &osh->cmn->malloced);
			}
			return;
		}
	}
#endif /* CONFIG_DHD_USE_STATIC_BUF */
	if (osh && osh->cmn) {
		ASSERT(osh->magic == OS_HANDLE_MAGIC);

		ASSERT(size <= osl_malloced(osh));

		atomic_sub(size, &osh->cmn->malloced);
	}
	kfree(addr);
}

uint
osl_check_memleak(osl_t *osh)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	if (atomic_read(&osh->cmn->refcount) == 1)
		return (atomic_read(&osh->cmn->malloced));
	else
		return 0;
}

uint
osl_malloced(osl_t *osh)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	return (atomic_read(&osh->cmn->malloced));
}

uint
osl_malloc_failed(osl_t *osh)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	return (osh->failed);
}


uint
osl_dma_consistent_align(void)
{
	return (PAGE_SIZE);
}

void*
osl_dma_alloc_consistent(osl_t *osh, uint size, uint16 align_bits, uint *alloced, dmaaddr_t *pap)
{
	void *va;
	uint16 align = (1 << align_bits);
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));

	if (!ISALIGNED(DMA_CONSISTENT_ALIGN, align))
		size += align;
	*alloced = size;

#ifndef	BCM_SECURE_DMA
#if (defined(__ARM_ARCH_7A__) && !defined(DHD_USE_COHERENT_MEM_FOR_RING)) || \
	defined(BCA_HNDROUTER)
	va = kmalloc(size, GFP_ATOMIC | __GFP_ZERO);
	if (va)
		*pap = (ulong)__virt_to_phys((ulong)va);
#else
	{
		dma_addr_t pap_lin;
		struct pci_dev *hwdev = osh->pdev;
		gfp_t flags;
#ifdef DHD_ALLOC_COHERENT_MEM_FROM_ATOMIC_POOL
		flags = GFP_ATOMIC;
#else
		flags = GFP_KERNEL;
#endif /* DHD_ALLOC_COHERENT_MEM_FROM_ATOMIC_POOL */
		va = dma_alloc_coherent(&hwdev->dev, size, &pap_lin, flags);
		*pap = (dmaaddr_t)pap_lin;
	}
#endif /* __ARM_ARCH_7A__ && !DHD_USE_COHERENT_MEM_FOR_RING || BCA_HNDROUTER */
#else
	va = osl_sec_dma_alloc_consistent(osh, size, align_bits, pap);
#endif /* BCM_SECURE_DMA */
	return va;
}

void
osl_dma_free_consistent(osl_t *osh, void *va, uint size, dmaaddr_t pa)
{
	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));

#ifndef BCM_SECURE_DMA
#if (defined(__ARM_ARCH_7A__) && !defined(DHD_USE_COHERENT_MEM_FOR_RING)) || \
	defined(BCA_HNDROUTER)
	kfree(va);
#else
	pci_free_consistent(osh->pdev, size, va, (dma_addr_t)pa);
#endif /* __ARM_ARCH_7A__ && !DHD_USE_COHERENT_MEM_FOR_RING || BCA_HNDROUTER */
#else
	osl_sec_dma_free_consistent(osh, va, size, pa);
#endif /* BCM_SECURE_DMA */
}

dmaaddr_t BCMFASTPATH
osl_dma_map(osl_t *osh, void *va, uint size, int direction, void *p, hnddma_seg_map_t *dmah)
{
	int dir;

	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));
	dir = (direction == DMA_TX)? PCI_DMA_TODEVICE: PCI_DMA_FROMDEVICE;


#if defined(BCM47XX_CA9) && defined(BCMDMASGLISTOSL)
	if (dmah != NULL) {
		int32 nsegs, i, totsegs = 0, totlen = 0;
		struct scatterlist *sg, _sg[MAX_DMA_SEGS * 2];
		struct scatterlist *s;
		struct sk_buff *skb;

		for (skb = (struct sk_buff *)p; skb != NULL; skb = PKTNEXT(osh, skb)) {
			sg = &_sg[totsegs];
			if (skb_is_nonlinear(skb)) {
				nsegs = skb_to_sgvec(skb, sg, 0, PKTLEN(osh, skb));
				ASSERT((nsegs > 0) && (totsegs + nsegs <= MAX_DMA_SEGS));
				if (osl_is_flag_set(osh, OSL_ACP_COHERENCE)) {
					for_each_sg(sg, s, nsegs, i) {
						if (sg_phys(s) >= ACP_WIN_LIMIT) {
							dma_map_page(
							&((struct pci_dev *)osh->pdev)->dev,
							sg_page(s), s->offset, s->length, dir);
						}
					}
				} else
					pci_map_sg(osh->pdev, sg, nsegs, dir);
			} else {
				nsegs = 1;
				ASSERT(totsegs + nsegs <= MAX_DMA_SEGS);
				sg->page_link = 0;
				sg_set_buf(sg, PKTDATA(osh, skb), PKTLEN(osh, skb));

				/* not do cache ops */
				if (osl_is_flag_set(osh, OSL_ACP_COHERENCE) &&
					(virt_to_phys(PKTDATA(osh, skb)) < ACP_WIN_LIMIT))
					goto no_cache_ops;

#ifdef CTFMAP
				/* Map size bytes (not skb->len) for ctf bufs */
				pci_map_single(osh->pdev, PKTDATA(osh, skb),
				    PKTISCTF(osh, skb) ? CTFMAPSZ : PKTLEN(osh, skb), dir);
#else
				pci_map_single(osh->pdev, PKTDATA(osh, skb), PKTLEN(osh, skb), dir);

#endif
			}
no_cache_ops:
			totsegs += nsegs;
			totlen += PKTLEN(osh, skb);
		}

		dmah->nsegs = totsegs;
		dmah->origsize = totlen;
		for (i = 0, sg = _sg; i < totsegs; i++, sg++) {
			dmah->segs[i].addr = sg_phys(sg);
			dmah->segs[i].length = sg->length;
		}
		return dmah->segs[0].addr;
	}
#endif /* BCM47XX_CA9 && BCMDMASGLISTOSL */

#if defined(BCM47XX_CA9)
	if (osl_is_flag_set(osh, OSL_ACP_COHERENCE)) {
		uint pa = virt_to_phys(va);
		if (pa < ACP_WIN_LIMIT)
			return (pa);
	}
#endif /* BCM47XX_CA9 */

	return (pci_map_single(osh->pdev, va, size, dir));
}

void BCMFASTPATH
osl_dma_unmap(osl_t *osh, uint pa, uint size, int direction)
{
	int dir;

	ASSERT((osh && (osh->magic == OS_HANDLE_MAGIC)));

#if defined(BCM47XX_CA9)
	if (osl_is_flag_set(osh, OSL_ACP_COHERENCE) && (pa < ACP_WIN_LIMIT))
		return;
#endif /* BCM47XX_CA9 */

	dir = (direction == DMA_TX)? PCI_DMA_TODEVICE: PCI_DMA_FROMDEVICE;
	pci_unmap_single(osh->pdev, (uint32)pa, size, dir);
}

/* OSL function for CPU relax */
inline void BCMFASTPATH
osl_cpu_relax(void)
{
	cpu_relax();
}

#if defined(mips)
inline void BCMFASTPATH
osl_cache_flush(void *va, uint size)
{
	unsigned long l = ROUNDDN((unsigned long)va, L1_CACHE_BYTES);
	unsigned long e = ROUNDUP((unsigned long)(va+size), L1_CACHE_BYTES);
	while (l < e)
	{
		flush_dcache_line(l);                         /* Hit_Writeback_Inv_D  */
		l += L1_CACHE_BYTES;                          /* next cache line base */
	}
}

inline void BCMFASTPATH
osl_cache_inv(void *va, uint size)
{
	unsigned long l = ROUNDDN((unsigned long)va, L1_CACHE_BYTES);
	unsigned long e = ROUNDUP((unsigned long)(va+size), L1_CACHE_BYTES);
	while (l < e)
	{
		invalidate_dcache_line(l);                    /* Hit_Invalidate_D     */
		l += L1_CACHE_BYTES;                          /* next cache line base */
	}
}

inline void osl_prefetch(const void *ptr)
{
	__asm__ __volatile__(".set mips4\npref %0,(%1)\n.set mips0\n"::"i" (Pref_Load), "r" (ptr));
}

#elif (defined(__ARM_ARCH_7A__) && !defined(DHD_USE_COHERENT_MEM_FOR_RING))

inline void BCMFASTPATH
osl_cache_flush(void *va, uint size)
{
#ifdef BCM47XX_CA9
	if (ACP_WAR_ENAB() && (virt_to_phys(va) < ACP_WIN_LIMIT))
		return;
#endif /* BCM47XX_CA9 */

	if (size > 0)
		dma_sync_single_for_device(OSH_NULL, virt_to_dma(OSH_NULL, va), size, DMA_TX);
}

inline void BCMFASTPATH
osl_cache_inv(void *va, uint size)
{
#ifdef BCM47XX_CA9
	if (ACP_WAR_ENAB() && (virt_to_phys(va) < ACP_WIN_LIMIT))
		return;
#endif /* BCM47XX_CA9 */

	dma_sync_single_for_cpu(OSH_NULL, virt_to_dma(OSH_NULL, va), size, DMA_RX);
}

inline void osl_prefetch(const void *ptr)
{
	__asm__ __volatile__("pld\t%0" :: "o"(*(const char *)ptr) : "cc");
}

int osl_arch_is_coherent(void)
{
#ifdef BCM47XX_CA9
	return arch_is_coherent();
#else
	return 0;
#endif
}


inline int osl_acp_war_enab(void)
{
#ifdef BCM47XX_CA9
	return ACP_WAR_ENAB();
#else
	return 0;
#endif /* BCM47XX_CA9 */
}

#elif (defined(BCA_HNDROUTER) && defined(__aarch64__))

inline void BCMFASTPATH
osl_cache_flush(void *va, uint size)
{
	if (size > 0)
		dma_sync_single_for_device(OSH_NULL, virt_to_phys(va), size, DMA_TX);
}

inline void BCMFASTPATH
osl_cache_inv(void *va, uint size)
{
	dma_sync_single_for_cpu(OSH_NULL, virt_to_phys(va), size, DMA_RX);
}

inline void osl_prefetch(const void *ptr)
{
	asm volatile("prfm pldl1keep, %a0\n" : : "p" (ptr));
}

#endif /* mips */


void
osl_delay(uint usec)
{
	uint d;

	while (usec > 0) {
		d = MIN(usec, 1000);
		udelay(d);
		usec -= d;
	}
}

void
osl_sleep(uint ms)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	if (ms < 20)
		usleep_range(ms*1000, ms*1000 + 1000);
	else
#endif
	msleep(ms);
}

#if defined(DSLCPE_DELAY)
void
osl_oshsh_init(osl_t *osh, shared_osl_t* oshsh)
{
	extern unsigned long loops_per_jiffy;
	osh->oshsh = oshsh;
	osh->oshsh->MIPS = loops_per_jiffy / (500000/HZ);
}

int
in_long_delay(osl_t *osh)
{
	return osh->oshsh->long_delay;
}

void
osl_long_delay(osl_t *osh, uint usec, bool yield)
{
	uint d;
	bool yielded = TRUE;
	int usec_to_delay = usec;
	unsigned long tick1, tick2, tick_diff = 0;

	/* delay at least requested usec */
	while (usec_to_delay > 0) {
		if (!yield || !yielded) {
			d = MIN(usec_to_delay, 10);
			udelay(d);
			usec_to_delay -= d;
		}
		if (usec_to_delay > 0) {
			osh->oshsh->long_delay++;
			OSL_GETCYCLES(tick1);
			spin_unlock_bh(osh->oshsh->lock);
			if (usec_to_delay > 0 && !in_irq() && !in_softirq() && !in_interrupt()) {
				schedule();
				yielded = TRUE;
			} else {
				yielded = FALSE;
			}
			spin_lock_bh(osh->oshsh->lock);
			OSL_GETCYCLES(tick2);

			if (yielded) {
				tick_diff = TICKDIFF(tick2, tick1);
				tick_diff = (tick_diff * 2)/(osh->oshsh->MIPS);
				if (tick_diff) {
					usec_to_delay -= tick_diff;
				} else
					yielded = 0;
			}
			osh->oshsh->long_delay--;
			ASSERT(osh->oshsh->long_delay >= 0);
		}
	}
}
#endif /* DSLCPE_DELAY */

/* Clone a packet.
 * The pkttag contents are NOT cloned.
 */
#ifdef BCMDBG_CTRACE
void *
osl_pktdup(osl_t *osh, void *skb, int line, char *file)
#else
#ifdef BCM_OBJECT_TRACE
void *
osl_pktdup(osl_t *osh, void *skb, int line, const char *caller)
#else
void *
osl_pktdup(osl_t *osh, void *skb)
#endif /* BCM_OBJECT_TRACE */
#endif /* BCMDBG_CTRACE */
{
	void * p;

	ASSERT(!PKTISCHAINED(skb));

	/* clear the CTFBUF flag if set and map the rest of the buffer
	 * before cloning.
	 */
	PKTCTFMAP(osh, skb);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	if ((p = pskb_copy((struct sk_buff *)skb, GFP_ATOMIC)) == NULL)
#else
	if ((p = skb_clone((struct sk_buff *)skb, GFP_ATOMIC)) == NULL)
#endif
		return NULL;

#ifdef CTFPOOL
	if (PKTISFAST(osh, skb)) {
		ctfpool_t *ctfpool;

		/* if the buffer allocated from ctfpool is cloned then
		 * we can't be sure when it will be freed. since there
		 * is a chance that we will be losing a buffer
		 * from our pool, we increment the refill count for the
		 * object to be alloced later.
		 */
		ctfpool = (ctfpool_t *)CTFPOOLPTR(osh, skb);
		ASSERT(ctfpool != NULL);
		PKTCLRFAST(osh, p);
		PKTCLRFAST(osh, skb);
		ctfpool->refills++;
	}
#endif /* CTFPOOL */

	/* Clear PKTC  context */
	PKTSETCLINK(p, NULL);
	PKTCCLRFLAGS(p);
	PKTCSETCNT(p, 1);
	PKTCSETLEN(p, PKTLEN(osh, skb));

	/* skb_clone copies skb->cb.. we don't want that */
	if (osh->pub.pkttag)
		OSL_PKTTAG_CLEAR(p);

	/* Increment the packet counter */
	atomic_inc(&osh->cmn->pktalloced);
#ifdef BCM_OBJECT_TRACE
	bcm_object_trace_opr(p, BCM_OBJDBG_ADD_PKT, caller, line);
#endif /* BCM_OBJECT_TRACE */

#ifdef BCMDBG_CTRACE
	ADD_CTRACE(osh, (struct sk_buff *)p, file, line);
#endif
	return (p);
}

#ifdef BCMDBG_CTRACE
int osl_pkt_is_frmnative(osl_t *osh, struct sk_buff *pkt)
{
	unsigned long flags;
	struct sk_buff *skb;
	int ck = FALSE;

	spin_lock_irqsave(&osh->ctrace_lock, flags);

	list_for_each_entry(skb, &osh->ctrace_list, ctrace_list) {
		if (pkt == skb) {
			ck = TRUE;
			break;
		}
	}

	spin_unlock_irqrestore(&osh->ctrace_lock, flags);
	return ck;
}

void osl_ctrace_dump(osl_t *osh, struct bcmstrbuf *b)
{
	unsigned long flags;
	struct sk_buff *skb;
	int idx = 0;
	int i, j;

	spin_lock_irqsave(&osh->ctrace_lock, flags);

	if (b != NULL)
		bcm_bprintf(b, " Total %d sbk not free\n", osh->ctrace_num);
	else
		printk(" Total %d sbk not free\n", osh->ctrace_num);

	list_for_each_entry(skb, &osh->ctrace_list, ctrace_list) {
		if (b != NULL)
			bcm_bprintf(b, "[%d] skb %p:\n", ++idx, skb);
		else
			printk("[%d] skb %p:\n", ++idx, skb);

		for (i = 0; i < skb->ctrace_count; i++) {
			j = (skb->ctrace_start + i) % CTRACE_NUM;
			if (b != NULL)
				bcm_bprintf(b, "    [%s(%d)]\n", skb->func[j], skb->line[j]);
			else
				printk("    [%s(%d)]\n", skb->func[j], skb->line[j]);
		}
		if (b != NULL)
			bcm_bprintf(b, "\n");
		else
			printk("\n");
	}

	spin_unlock_irqrestore(&osh->ctrace_lock, flags);

	return;
}
#endif /* BCMDBG_CTRACE */


/*
 * OSLREGOPS specifies the use of osl_XXX routines to be used for register access
 */
#ifdef OSLREGOPS
uint8
osl_readb(osl_t *osh, volatile uint8 *r)
{
	osl_rreg_fn_t rreg	= ((osl_pubinfo_t*)osh)->rreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	return (uint8)((rreg)(ctx, (volatile void*)r, sizeof(uint8)));
}


uint16
osl_readw(osl_t *osh, volatile uint16 *r)
{
	osl_rreg_fn_t rreg	= ((osl_pubinfo_t*)osh)->rreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	return (uint16)((rreg)(ctx, (volatile void*)r, sizeof(uint16)));
}

uint32
osl_readl(osl_t *osh, volatile uint32 *r)
{
	osl_rreg_fn_t rreg	= ((osl_pubinfo_t*)osh)->rreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	return (uint32)((rreg)(ctx, (volatile void*)r, sizeof(uint32)));
}

void
osl_writeb(osl_t *osh, volatile uint8 *r, uint8 v)
{
	osl_wreg_fn_t wreg	= ((osl_pubinfo_t*)osh)->wreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	((wreg)(ctx, (volatile void*)r, v, sizeof(uint8)));
}


void
osl_writew(osl_t *osh, volatile uint16 *r, uint16 v)
{
	osl_wreg_fn_t wreg	= ((osl_pubinfo_t*)osh)->wreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	((wreg)(ctx, (volatile void*)r, v, sizeof(uint16)));
}

void
osl_writel(osl_t *osh, volatile uint32 *r, uint32 v)
{
	osl_wreg_fn_t wreg	= ((osl_pubinfo_t*)osh)->wreg_fn;
	void *ctx		= ((osl_pubinfo_t*)osh)->reg_ctx;

	((wreg)(ctx, (volatile void*)r, v, sizeof(uint32)));
}
#endif /* OSLREGOPS */

/*
 * BINOSL selects the slightly slower function-call-based binary compatible osl.
 */
#ifdef BINOSL

uint32
osl_sysuptime(void)
{
	return ((uint32)jiffies * (1000 / HZ));
}

int
osl_printf(const char *format, ...)
{
	va_list args;
	static char printbuf[1024];
	int len;

	/* sprintf into a local buffer because there *is* no "vprintk()".. */
	va_start(args, format);
	len = vsnprintf(printbuf, 1024, format, args);
	va_end(args);

	if (len > sizeof(printbuf)) {
		printk("osl_printf: buffer overrun\n");
		return (0);
	}

	return (printk("%s", printbuf));
}

int
osl_sprintf(char *buf, const char *format, ...)
{
	va_list args;
	int rc;

	va_start(args, format);
	rc = vsprintf(buf, format, args);
	va_end(args);
	return (rc);
}

int
osl_snprintf(char *buf, size_t n, const char *format, ...)
{
	va_list args;
	int rc;

	va_start(args, format);
	rc = vsnprintf(buf, n, format, args);
	va_end(args);
	return (rc);
}

int
osl_vsprintf(char *buf, const char *format, va_list ap)
{
	return (vsprintf(buf, format, ap));
}

int
osl_vsnprintf(char *buf, size_t n, const char *format, va_list ap)
{
	return (vsnprintf(buf, n, format, ap));
}

int
osl_strcmp(const char *s1, const char *s2)
{
	return (strcmp(s1, s2));
}

int
osl_strncmp(const char *s1, const char *s2, uint n)
{
	return (strncmp(s1, s2, n));
}

int
osl_strlen(const char *s)
{
	return (strlen(s));
}

char*
osl_strcpy(char *d, const char *s)
{
	return (strcpy(d, s));
}

char*
osl_strncpy(char *d, const char *s, uint n)
{
	return (strncpy(d, s, n));
}

char*
osl_strchr(const char *s, int c)
{
	return (strchr(s, c));
}

char*
osl_strrchr(const char *s, int c)
{
	return (strrchr(s, c));
}

void*
osl_memset(void *d, int c, size_t n)
{
	return memset(d, c, n);
}

void*
osl_memcpy(void *d, const void *s, size_t n)
{
	return memcpy(d, s, n);
}

void*
osl_memmove(void *d, const void *s, size_t n)
{
	return memmove(d, s, n);
}

int
osl_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n);
}

uint32
osl_readl(volatile uint32 *r)
{
	return (readl(r));
}

uint16
osl_readw(volatile uint16 *r)
{
	return (readw(r));
}

uint8
osl_readb(volatile uint8 *r)
{
	return (readb(r));
}

void
osl_writel(uint32 v, volatile uint32 *r)
{
	writel(v, r);
}

void
osl_writew(uint16 v, volatile uint16 *r)
{
	writew(v, r);
}

void
osl_writeb(uint8 v, volatile uint8 *r)
{
	writeb(v, r);
}

void *
osl_uncached(void *va)
{
#ifdef mips
	return ((void*)KSEG1ADDR(va));
#else
	return ((void*)va);
#endif /* mips */
}

void *
osl_cached(void *va)
{
#ifdef mips
	return ((void*)KSEG0ADDR(va));
#else
	return ((void*)va);
#endif /* mips */
}

uint
osl_getcycles(void)
{
	uint cycles;

#if defined(mips)
	cycles = read_c0_count() * 2;
#elif defined(__i386__)
	rdtscl(cycles);
#else
	cycles = 0;
#endif /* defined(mips) */
	return cycles;
}

void *
osl_reg_map(uint32 pa, uint size)
{
	return (ioremap_nocache((unsigned long)pa, (unsigned long)size));
}

void
osl_reg_unmap(void *va)
{
	iounmap(va);
}

int
osl_busprobe(uint32 *val, uint32 addr)
{
#ifdef mips
	return get_dbe(*val, (uint32 *)addr);
#else
	*val = readl((uint32 *)(uintptr)addr);
	return 0;
#endif /* mips */
}

bool
osl_pktshared(void *skb)
{
	return (((struct sk_buff*)skb)->cloned);
}

uchar*
osl_pktdata(osl_t *osh, void *skb)
{
	return (((struct sk_buff*)skb)->data);
}

uint
osl_pktlen(osl_t *osh, void *skb)
{
	return (((struct sk_buff*)skb)->len);
}

uint
osl_pktheadroom(osl_t *osh, void *skb)
{
	return (uint) skb_headroom((struct sk_buff *) skb);
}

uint
osl_pkttailroom(osl_t *osh, void *skb)
{
	return (uint) skb_tailroom((struct sk_buff *) skb);
}

void*
osl_pktnext(osl_t *osh, void *skb)
{
	return (((struct sk_buff*)skb)->next);
}

void
osl_pktsetnext(void *skb, void *x)
{
	((struct sk_buff*)skb)->next = (struct sk_buff*)x;
}

void
osl_pktsetlen(osl_t *osh, void *skb, uint len)
{
	__skb_trim((struct sk_buff*)skb, len);
}

uchar*
osl_pktpush(osl_t *osh, void *skb, int bytes)
{
	return (skb_push((struct sk_buff*)skb, bytes));
}

uchar*
osl_pktpull(osl_t *osh, void *skb, int bytes)
{
	return (skb_pull((struct sk_buff*)skb, bytes));
}

void*
osl_pkttag(void *skb)
{
	return ((void*)(((struct sk_buff*)skb)->cb));
}

void*
osl_pktlink(void *skb)
{
	return (((struct sk_buff*)skb)->prev);
}

void
osl_pktsetlink(void *skb, void *x)
{
	((struct sk_buff*)skb)->prev = (struct sk_buff*)x;
}

uint
osl_pktprio(void *skb)
{
	return (((struct sk_buff*)skb)->priority);
}

void
osl_pktsetprio(void *skb, uint x)
{
	((struct sk_buff*)skb)->priority = x;
}
#endif	/* BINOSL */

uint
osl_pktalloced(osl_t *osh)
{
	if (atomic_read(&osh->cmn->refcount) == 1)
		return (atomic_read(&osh->cmn->pktalloced));
	else
		return 0;
}

uint32
osl_rand(void)
{
	uint32 rand;

	get_random_bytes(&rand, sizeof(rand));

	return rand;
}

/* Linux Kernel: File Operations: start */
void *
osl_os_open_image(char *filename)
{
	struct file *fp;

	fp = filp_open(filename, O_RDONLY, 0);
	/*
	 * 2.6.11 (FC4) supports filp_open() but later revs don't?
	 * Alternative:
	 * fp = open_namei(AT_FDCWD, filename, O_RD, 0);
	 * ???
	 */
	 if (IS_ERR(fp))
		 fp = NULL;

	 return fp;
}

int
osl_os_get_image_block(char *buf, int len, void *image)
{
	struct file *fp = (struct file *)image;
	int rdlen;

	if (!image)
		return 0;

	rdlen = kernel_read(fp, fp->f_pos, buf, len);
	if (rdlen > 0)
		fp->f_pos += rdlen;

	return rdlen;
}

void
osl_os_close_image(void *image)
{
	if (image)
		filp_close((struct file *)image, NULL);
}

int
osl_os_image_size(void *image)
{
	int len = 0, curroffset;

	if (image) {
		/* store the current offset */
		curroffset = generic_file_llseek(image, 0, 1);
		/* goto end of file to get length */
		len = generic_file_llseek(image, 0, 2);
		/* restore back the offset */
		generic_file_llseek(image, curroffset, 0);
	}
	return len;
}

/* Linux Kernel: File Operations: end */

#ifdef BCM47XX_CA9
inline void osl_pcie_rreg(osl_t *osh, ulong addr, void *v, uint size)
{
	unsigned long flags = 0;
	int pci_access = 0;

	if (osh && BUSTYPE(osh->bustype) == PCI_BUS)
		pci_access = 1;

	if (pci_access && ACP_WAR_ENAB())
		spin_lock_irqsave(&l2x0_reg_lock, flags);

	switch (size) {
	case sizeof(uint8):
		*(uint8*)v = readb((volatile uint8*)(addr));
		break;
	case sizeof(uint16):
		*(uint16*)v = readw((volatile uint16*)(addr));
		break;
	case sizeof(uint32):
		*(uint32*)v = readl((volatile uint32*)(addr));
		break;
	case sizeof(uint64):
		*(uint64*)v = *((volatile uint64*)(addr));
		break;
	}

	if (pci_access && ACP_WAR_ENAB())
		spin_unlock_irqrestore(&l2x0_reg_lock, flags);
}
#endif /* BCM47XX_CA9 */

/* APIs to set/get specific quirks in OSL layer */
void
osl_flag_set(osl_t *osh, uint32 mask)
{
	osh->flags |= mask;
}

#ifdef BCM47XX_CA9
inline bool BCMFASTPATH
#else
bool
#endif /* BCM47XX_CA9 */
osl_is_flag_set(osl_t *osh, uint32 mask)
{
	return (osh->flags & mask);
}

#ifdef BCM_SECURE_DMA
static void *
osl_sec_dma_ioremap(osl_t *osh, struct page *page, size_t size, bool iscache, bool isdecr)
{

	struct page **map;
	int order, i;
	void *addr = NULL;

	size = PAGE_ALIGN(size);
	order = get_order(size);

	map = kmalloc(sizeof(struct page *) << order, GFP_ATOMIC);

	if (map == NULL)
		return NULL;

	for (i = 0; i < (size >> PAGE_SHIFT); i++)
		map[i] = page + i;

	if (iscache) {
		addr = vmap(map, size >> PAGE_SHIFT, VM_MAP, __pgprot(PAGE_KERNEL));
		if (isdecr) {
			osh->contig_delta_va_pa = (phys_addr_t)(addr - page_to_phys(page));
		}
	}
	else {

#if defined(__ARM_ARCH_7A__)
		addr = vmap(map, size >> PAGE_SHIFT, VM_MAP,
			pgprot_noncached(__pgprot(PAGE_KERNEL)));
#endif
		if (isdecr) {
			osh->contig_delta_va_pa = (phys_addr_t)(addr - page_to_phys(page));
		}
	}

	kfree(map);
	return (void *)addr;
}

static void
osl_sec_dma_iounmap(osl_t *osh, void *contig_base_va, size_t size)
{
	vunmap(contig_base_va);
}

static void
osl_sec_dma_init_elem_mem_block(osl_t *osh, size_t mbsize, int max, sec_mem_elem_t **list)
{
	int i;
	sec_mem_elem_t *sec_mem_elem;

	if ((sec_mem_elem = kmalloc(sizeof(sec_mem_elem_t)*(max), GFP_ATOMIC)) != NULL) {

		*list = sec_mem_elem;
		bzero(sec_mem_elem, sizeof(sec_mem_elem_t)*(max));
		for (i = 0; i < max-1; i++) {
			sec_mem_elem->next = (sec_mem_elem + 1);
			sec_mem_elem->size = mbsize;
			sec_mem_elem->pa_cma = (u32)osh->contig_base_alloc;
			sec_mem_elem->vac = osh->contig_base_alloc_va;

			osh->contig_base_alloc += mbsize;
			osh->contig_base_alloc_va += mbsize;

			sec_mem_elem = sec_mem_elem + 1;
		}
		sec_mem_elem->next = NULL;
		sec_mem_elem->size = mbsize;
		sec_mem_elem->pa_cma = (u32)osh->contig_base_alloc;
		sec_mem_elem->vac = osh->contig_base_alloc_va;

		osh->contig_base_alloc += mbsize;
		osh->contig_base_alloc_va += mbsize;

	}
	else
		printf("%s sec mem elem kmalloc failed\n", __FUNCTION__);
}


static void
osl_sec_dma_deinit_elem_mem_block(osl_t *osh, size_t mbsize, int max, void *sec_list_base)
{
	if (sec_list_base)
		kfree(sec_list_base);
}

static sec_mem_elem_t * BCMFASTPATH
osl_sec_dma_alloc_mem_elem(osl_t *osh, void *va, uint size, int direction,
	struct sec_cma_info *ptr_cma_info, uint offset)
{
	sec_mem_elem_t *sec_mem_elem = NULL;

	if (size <= 512 && osh->sec_list_512) {
		sec_mem_elem = osh->sec_list_512;
		osh->sec_list_512 = sec_mem_elem->next;
	}
	else if (size <= 2048 && osh->sec_list_2048) {
		sec_mem_elem = osh->sec_list_2048;
		osh->sec_list_2048 = sec_mem_elem->next;
	}
	else if (osh->sec_list_4096) {
		sec_mem_elem = osh->sec_list_4096;
		osh->sec_list_4096 = sec_mem_elem->next;
	} else {
		printf("%s No matching Pool available size=%d \n", __FUNCTION__, size);
		return NULL;
	}

	if (sec_mem_elem != NULL) {
		sec_mem_elem->next = NULL;

	if (ptr_cma_info->sec_alloc_list_tail) {
		ptr_cma_info->sec_alloc_list_tail->next = sec_mem_elem;
	}

	ptr_cma_info->sec_alloc_list_tail = sec_mem_elem;
	if (ptr_cma_info->sec_alloc_list == NULL)
		ptr_cma_info->sec_alloc_list = sec_mem_elem;
	}
	return sec_mem_elem;
}

static void BCMFASTPATH
osl_sec_dma_free_mem_elem(osl_t *osh, sec_mem_elem_t *sec_mem_elem)
{
	sec_mem_elem->dma_handle = 0x0;
	sec_mem_elem->va = NULL;

	if (sec_mem_elem->size == 512) {
		sec_mem_elem->next = osh->sec_list_512;
		osh->sec_list_512 = sec_mem_elem;
	}
	else if (sec_mem_elem->size == 2048) {
		sec_mem_elem->next = osh->sec_list_2048;
		osh->sec_list_2048 = sec_mem_elem;
	}
	else if (sec_mem_elem->size == 4096) {
		sec_mem_elem->next = osh->sec_list_4096;
		osh->sec_list_4096 = sec_mem_elem;
	}
	else
	printf("%s free failed size=%d \n", __FUNCTION__, sec_mem_elem->size);
}

static sec_mem_elem_t * BCMFASTPATH
osl_sec_dma_find_rem_elem(osl_t *osh, struct sec_cma_info *ptr_cma_info, dma_addr_t dma_handle)
{
	sec_mem_elem_t *sec_mem_elem = ptr_cma_info->sec_alloc_list;
	sec_mem_elem_t *sec_prv_elem = ptr_cma_info->sec_alloc_list;

	if (!sec_mem_elem) {
		printk("osl_sec_dma_find_rem_elem ptr_cma_info->sec_alloc_list is NULL \n");
		return NULL;
	}

	if (sec_mem_elem->dma_handle == dma_handle) {

		ptr_cma_info->sec_alloc_list = sec_mem_elem->next;

		if (sec_mem_elem == ptr_cma_info->sec_alloc_list_tail) {
			ptr_cma_info->sec_alloc_list_tail = NULL;
			ASSERT(ptr_cma_info->sec_alloc_list == NULL);
		}

		return sec_mem_elem;
	}

	while (sec_mem_elem != NULL) {

		if (sec_mem_elem->dma_handle == dma_handle) {

			sec_prv_elem->next = sec_mem_elem->next;
			if (sec_mem_elem == ptr_cma_info->sec_alloc_list_tail)
				ptr_cma_info->sec_alloc_list_tail = sec_prv_elem;

			return sec_mem_elem;
		}
		sec_prv_elem = sec_mem_elem;
		sec_mem_elem = sec_mem_elem->next;
	}
	return NULL;
}

static sec_mem_elem_t *
osl_sec_dma_rem_first_elem(osl_t *osh, struct sec_cma_info *ptr_cma_info)
{
	sec_mem_elem_t *sec_mem_elem = ptr_cma_info->sec_alloc_list;

	if (sec_mem_elem) {

		ptr_cma_info->sec_alloc_list = sec_mem_elem->next;

		if (ptr_cma_info->sec_alloc_list == NULL)
			ptr_cma_info->sec_alloc_list_tail = NULL;

		return sec_mem_elem;

	} else
		return NULL;
}

static void * BCMFASTPATH
osl_sec_dma_last_elem(osl_t *osh, struct sec_cma_info *ptr_cma_info)
{
	return ptr_cma_info->sec_alloc_list_tail;
}

dma_addr_t BCMFASTPATH
osl_sec_dma_map_txmeta(osl_t *osh, void *va, uint size, int direction, void *p,
	hnddma_seg_map_t *dmah, void *ptr_cma_info)
{
	sec_mem_elem_t *sec_mem_elem;
	struct page *pa_cma_page;
	uint loffset;
	void *vaorig = va + size;
	dma_addr_t dma_handle = 0x0;
	/* packet will be the one added with osl_sec_dma_map() just before this call */

	sec_mem_elem = osl_sec_dma_last_elem(osh, ptr_cma_info);

	if (sec_mem_elem && sec_mem_elem->va == vaorig) {

		pa_cma_page = phys_to_page(sec_mem_elem->pa_cma);
		loffset = sec_mem_elem->pa_cma -(sec_mem_elem->pa_cma & ~(PAGE_SIZE-1));

		dma_handle = dma_map_page(OSH_NULL, pa_cma_page, loffset, size,
			(direction == DMA_TX ? DMA_TO_DEVICE:DMA_FROM_DEVICE));

	} else {
		printf("%s: error orig va not found va = 0x%p \n",
			__FUNCTION__, vaorig);
	}
	return dma_handle;
}

dma_addr_t BCMFASTPATH
osl_sec_dma_map(osl_t *osh, void *va, uint size, int direction, void *p,
	hnddma_seg_map_t *dmah, void *ptr_cma_info, uint offset)
{

	sec_mem_elem_t *sec_mem_elem;
	struct page *pa_cma_page;
	void *pa_cma_kmap_va = NULL;
	int *fragva;
	uint buflen = 0;
	struct sk_buff *skb;
	dma_addr_t dma_handle = 0x0;
	uint loffset;
	int i = 0;

	sec_mem_elem = osl_sec_dma_alloc_mem_elem(osh, va, size, direction, ptr_cma_info, offset);

	if (sec_mem_elem == NULL) {
		printk("linux_osl.c: osl_sec_dma_map - cma allocation failed\n");
		return 0;
	}
	sec_mem_elem->va = va;
	sec_mem_elem->direction = direction;
	pa_cma_page = phys_to_page(sec_mem_elem->pa_cma);

	loffset = sec_mem_elem->pa_cma -(sec_mem_elem->pa_cma & ~(PAGE_SIZE-1));
	/* pa_cma_kmap_va = kmap_atomic(pa_cma_page);
	* pa_cma_kmap_va += loffset;
	*/

	pa_cma_kmap_va = sec_mem_elem->vac;

	if (direction == DMA_TX) {

		if (p == NULL) {

			memcpy(pa_cma_kmap_va+offset, va, size);
			buflen = size;
		}
		else {
			for (skb = (struct sk_buff *)p; skb != NULL; skb = PKTNEXT(osh, skb)) {
				if (skb_is_nonlinear(skb)) {


					for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
						skb_frag_t *f = &skb_shinfo(skb)->frags[i];
						fragva = kmap_atomic(skb_frag_page(f));
						memcpy((pa_cma_kmap_va+offset+buflen),
						(fragva + f->page_offset), skb_frag_size(f));
						kunmap_atomic(fragva);
						buflen += skb_frag_size(f);
					}
				}
				else {
					memcpy((pa_cma_kmap_va+offset+buflen), skb->data, skb->len);
					buflen += skb->len;
				}
			}

		}
		if (dmah) {
			dmah->nsegs = 1;
			dmah->origsize = buflen;
		}
	}

	else if (direction == DMA_RX)
	{
		buflen = size;
		if ((p != NULL) && (dmah != NULL)) {
			dmah->nsegs = 1;
			dmah->origsize = buflen;
		}
	}
#ifdef WLC_HIGH

	if (direction == DMA_RX) {


		*(uint32 *)(pa_cma_kmap_va) = 0x0;
		flush_kernel_vmap_range(pa_cma_kmap_va, sizeof(int));

	}
#endif
	if (direction == DMA_RX || direction == DMA_TX) {

		dma_handle = dma_map_page(OSH_NULL, pa_cma_page, loffset+offset, buflen,
			(direction == DMA_TX ? DMA_TO_DEVICE:DMA_FROM_DEVICE));

	}
	if (dmah) {
		dmah->segs[0].addr = dma_handle;
		dmah->segs[0].length = buflen;
	}
	sec_mem_elem->dma_handle = dma_handle;
	/* kunmap_atomic(pa_cma_kmap_va-loffset); */
	return dma_handle;
}

dma_addr_t BCMFASTPATH
osl_sec_dma_dd_map(osl_t *osh, void *va, uint size, int direction, void *p, hnddma_seg_map_t *map)
{

	struct page *pa_cma_page;
	phys_addr_t pa_cma;
	dma_addr_t dma_handle = 0x0;
	uint loffset;

	pa_cma = (phys_addr_t)(va - osh->contig_delta_va_pa);
	pa_cma_page = phys_to_page(pa_cma);
	loffset = pa_cma -(pa_cma & ~(PAGE_SIZE-1));

	dma_handle = dma_map_page(OSH_NULL, pa_cma_page, loffset, size,
		(direction == DMA_TX ? DMA_TO_DEVICE:DMA_FROM_DEVICE));

	return dma_handle;
}

void BCMFASTPATH
osl_sec_dma_unmap(osl_t *osh, dma_addr_t dma_handle, uint size, int direction,
void *p, hnddma_seg_map_t *map,	void *ptr_cma_info, uint offset)
{
	sec_mem_elem_t *sec_mem_elem;
	struct page *pa_cma_page;
	void *pa_cma_kmap_va = NULL;
	uint buflen = 0;
	dma_addr_t pa_cma;
	void *va;
	uint loffset = 0;
	int read_count = 0;
	BCM_REFERENCE(buflen);
	BCM_REFERENCE(read_count);

	sec_mem_elem = osl_sec_dma_find_rem_elem(osh, ptr_cma_info, dma_handle);
	if (sec_mem_elem == NULL) {
		printf("%s sec_mem_elem is NULL and dma_handle =0x%lx and dir=%d\n",
			__FUNCTION__, (ulong)dma_handle, direction);
		return;
	}

	va = sec_mem_elem->va;
	va -= offset;
	pa_cma = sec_mem_elem->pa_cma;

	pa_cma_page = phys_to_page(pa_cma);
	loffset = sec_mem_elem->pa_cma -(sec_mem_elem->pa_cma & ~(PAGE_SIZE-1));

	if (direction == DMA_RX) {

		if (p == NULL) {

			/* pa_cma_kmap_va = kmap_atomic(pa_cma_page);
			* pa_cma_kmap_va += loffset;
			*/

			pa_cma_kmap_va = sec_mem_elem->vac;

#ifdef WLC_HIGH
			for (read_count = 200; read_count; read_count--) {

				invalidate_kernel_vmap_range(pa_cma_kmap_va, sizeof(int));

				buflen = *(uint *)(pa_cma_kmap_va);
				if (buflen)
					break;

				OSL_DELAY(1);
			}
#endif
			dma_unmap_page(OSH_NULL, pa_cma, size, DMA_FROM_DEVICE);
			memcpy(va, pa_cma_kmap_va, size);
			/* kunmap_atomic(pa_cma_kmap_va); */
		}
#ifdef NOT_YET
		else {
			buflen = 0;
			for (skb = (struct sk_buff *)p; (buflen < size) &&
				(skb != NULL); skb = skb->next) {
				if (skb_is_nonlinear(skb)) {
					pa_cma_kmap_va = kmap_atomic(pa_cma_page);
					for (i = 0; (buflen < size) &&
						(i < skb_shinfo(skb)->nr_frags); i++) {
						skb_frag_t *f = &skb_shinfo(skb)->frags[i];
						cpuaddr = kmap_atomic(skb_frag_page(f));
						memcpy((cpuaddr + f->page_offset),
							(pa_cma_kmap_va+buflen), skb_frag_size(f));
						kunmap_atomic(cpuaddr);
						buflen += skb_frag_size(f);
					}
						kunmap_atomic(pa_cma_kmap_va);
				}
				else {
					pa_cma_kmap_va = kmap_atomic(pa_cma_page);
					memcpy(skb->data, (pa_cma_kmap_va + buflen), skb->len);
					kunmap_atomic(pa_cma_kmap_va);
					buflen += skb->len;
				}

			}

		}
#endif /* NOT YET */
	} else {
		dma_unmap_page(OSH_NULL, pa_cma, size+offset, DMA_TO_DEVICE);
	}

	osl_sec_dma_free_mem_elem(osh, sec_mem_elem);
}

void
osl_sec_dma_unmap_all(osl_t *osh, void *ptr_cma_info)
{

	sec_mem_elem_t *sec_mem_elem;

	sec_mem_elem = osl_sec_dma_rem_first_elem(osh, ptr_cma_info);

	while (sec_mem_elem != NULL) {

		dma_unmap_page(OSH_NULL, sec_mem_elem->pa_cma, sec_mem_elem->size,
			sec_mem_elem->direction == DMA_TX ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
		osl_sec_dma_free_mem_elem(osh, sec_mem_elem);

		sec_mem_elem = osl_sec_dma_rem_first_elem(osh, ptr_cma_info);
	}
}

static void
osl_sec_dma_init_consistent(osl_t *osh)
{
	int i;
	void *temp_va = osh->contig_base_alloc_coherent_va;
	phys_addr_t temp_pa = osh->contig_base_alloc_coherent;

	for (i = 0; i < SEC_CMA_COHERENT_MAX; i++) {
		osh->sec_cma_coherent[i].avail = TRUE;
		osh->sec_cma_coherent[i].va = temp_va;
		osh->sec_cma_coherent[i].pa = temp_pa;
		temp_va += SEC_CMA_COHERENT_BLK;
		temp_pa += SEC_CMA_COHERENT_BLK;
	}
}

static void *
osl_sec_dma_alloc_consistent(osl_t *osh, uint size, uint16 align_bits, ulong *pap)
{

	void *temp_va = NULL;
	ulong temp_pa = 0;
	int i;

	if (size > SEC_CMA_COHERENT_BLK) {
		printf("%s unsupported size\n", __FUNCTION__);
		return NULL;
	}

	for (i = 0; i < SEC_CMA_COHERENT_MAX; i++) {
		if (osh->sec_cma_coherent[i].avail == TRUE) {
			temp_va = osh->sec_cma_coherent[i].va;
			temp_pa = osh->sec_cma_coherent[i].pa;
			osh->sec_cma_coherent[i].avail = FALSE;
			break;
		}
	}

	if (i == SEC_CMA_COHERENT_MAX)
		printf("%s:No coherent mem: va = 0x%p pa = 0x%lx size = %d\n", __FUNCTION__,
			temp_va, (ulong)temp_pa, size);

	*pap = (unsigned long)temp_pa;
	return temp_va;
}

static void
osl_sec_dma_free_consistent(osl_t *osh, void *va, uint size, dmaaddr_t pa)
{
	int i = 0;

	for (i = 0; i < SEC_CMA_COHERENT_MAX; i++) {
		if (osh->sec_cma_coherent[i].va == va) {
			osh->sec_cma_coherent[i].avail = TRUE;
			break;
		}
	}
	if (i == SEC_CMA_COHERENT_MAX)
		printf("%s:Error: va = 0x%p pa = 0x%lx size = %d\n", __FUNCTION__,
			va, (ulong)pa, size);
}

#endif /* BCM_SECURE_DMA */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0) && defined(TSQ_MULTIPLIER)
#include <linux/kallsyms.h>
#include <net/sock.h>
void
osl_pkt_orphan_partial(struct sk_buff *skb)
{
	uint32 fraction;
	static void *p_tcp_wfree = NULL;

	if (!skb->destructor || skb->destructor == sock_wfree)
		return;

	if (unlikely(!p_tcp_wfree)) {
		char sym[KSYM_SYMBOL_LEN];
		sprint_symbol(sym, (unsigned long)skb->destructor);
		sym[9] = 0;
		if (!strcmp(sym, "tcp_wfree"))
			p_tcp_wfree = skb->destructor;
		else
			return;
	}

	if (unlikely(skb->destructor != p_tcp_wfree || !skb->sk))
		return;

	/* abstract a certain portion of skb truesize from the socket
	 * sk_wmem_alloc to allow more skb can be allocated for this
	 * socket for better cusion meeting WiFi device requirement
	 */
	fraction = skb->truesize * (TSQ_MULTIPLIER - 1) / TSQ_MULTIPLIER;
	skb->truesize -= fraction;
	atomic_sub(fraction, &skb->sk->sk_wmem_alloc);
}
#endif /* LINUX_VERSION >= 3.6.0 && TSQ_MULTIPLIER */

#ifdef BCM_NBUFF

void inline *osl_pkt_get_tag(void *pkt) {

	if (IS_SKBUFF_PTR(pkt)) {
		return  ((void *)(((struct sk_buff*)(pkt))->cb));
	} else {
		FkBuff_t *fkb_p = PNBUFF_2_FKBUFF(pkt);
		if (_is_fkb_cloned_pool_(fkb_p))
			return (void *)fkb_p->dhd_pkttag_info_p;
		else
			return ((void *)(PFKBUFF_TO_PHEAD(PNBUFF_2_FKBUFF(pkt))));
	}
}

void inline osl_pkt_clear_tag(void *pkt) {
	void *tag = osl_pkt_get_tag(pkt);
	*(uint32 *)(tag) = 0; *(uint32 *)(tag+4) = 0;
	*(uint32 *)(tag+8) = 0; *(uint32 *)(tag+12) = 0;
	*(uint32 *)(tag+16) = 0; *(uint32 *)(tag+20) = 0;
	*(uint32 *)(tag+24) = 0; *(uint32 *)(tag+28) = 0;
}

uint
osl_pktprio(void *p)
{
	uint32 prio = 0;
	if (IS_SKBUFF_PTR(p)) {
		prio = ((struct sk_buff*)p)->mark>>PRIO_LOC_NFMARK & 0x7;
	} else { /* manuplated in dhd, 3bit prio + 10bit flowid */
		FkBuff_t *fkb_p = PNBUFF_2_FKBUFF(p);
		prio = fkb_p->wl.ucast.dhd.wl_prio; /* Same bit pos used for mcast */
	}

	if (prio > 7) {
		printk("osl_pktprio: wrong prio (0x%x) !!!\n", prio);
		prio = 0;
	}

	return prio;
}

void
osl_pktsetprio(void *p, uint x)
{
	if (IS_SKBUFF_PTR(p)) {
		((struct sk_buff*)p)->mark &= ~(0x7 << PRIO_LOC_NFMARK);
		((struct sk_buff*)p)->mark |= (x & 0x7) << PRIO_LOC_NFMARK;
	}
	else
	{
		FkBuff_t *fkb_p = PNBUFF_2_FKBUFF(p);
		fkb_p->wl.ucast.dhd.wl_prio = x; /* Same bit pos used for mcast */
	}
}


uint
osl_pktflowid(void *p)
{
	if (IS_SKBUFF_PTR(p)) {
		return ((struct sk_buff *)p)->wl.ucast.dhd.flowring_idx;
	}
	else
	{
		FkBuff_t *fkb_p = PNBUFF_2_FKBUFF(p);
		/* for both ucast and mcast, flowring_idx is at the same position */
		return fkb_p->wl.ucast.dhd.flowring_idx;
	}
}

void
osl_pktsetflowid(void *p, uint x)
{
	if (IS_SKBUFF_PTR(p)) {
		((struct sk_buff *)p)->wl.ucast.dhd.flowring_idx = x;
	}
	else
	{
		FkBuff_t *fkb_p = PNBUFF_2_FKBUFF(p);
		/* for both ucast and mcast, flowring_idx is at the same position */
		fkb_p->wl.ucast.dhd.flowring_idx = x;
	}
}


uchar*
osl_pktdata(osl_t *osh, void *p)
{
	BCM_REFERENCE(osh);
	return nbuff_get_data((pNBuff_t)p);
}

uint
osl_pktlen(osl_t *osh, void *p)
{
	BCM_REFERENCE(osh);
	return nbuff_get_len((pNBuff_t)p);
}

void
osl_pktsetlen(osl_t *osh, void *p, uint len)
{
	BCM_REFERENCE(osh);
	if (IS_SKBUFF_PTR((pNBuff_t)p))
	    __skb_trim((struct sk_buff*)p, len);
	    /* else if IS_FPBUFF_PTR, else if IS_TGBUFF_PTR */
	else
	    nbuff_set_len((pNBuff_t)p, len);
}

uchar*
osl_pktpush(osl_t *osh, void *skb, int bytes)
{
	BCM_REFERENCE(osh);
	return (nbuff_push(skb, bytes));
}

uchar*
osl_pktpull(osl_t *osh, void *skb, int bytes)
{
	BCM_REFERENCE(osh);
	return (nbuff_pull(skb, bytes));
}

struct pci_dev*
osh_get_pdev(osl_t *osh)
{
	return osh->pdev;
}

void *
osl_pktlink(void *p)
{
	if (IS_FKBUFF_PTR(p))
		return nbuff_get_queue(p);
	else
		return (((struct sk_buff*)(p))->prev);
}

void
osl_pktsetlink(void *p, void *x)
{
	if (IS_FKBUFF_PTR(p))
		nbuff_set_queue(p, x);
	else
		(((struct sk_buff*)(p))->prev = (struct sk_buff*)(x));
}
#endif /* BCM_NBUFF */
