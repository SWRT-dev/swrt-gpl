/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017-2020, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/cacheflush.h>
#include <linux/avalanche/generic/avalanche_pp_api.h>
#include "net/puma_datapath_api.h"
#include <linux/netip_mem_util.h>
#include <directconnect_dp_api.h>
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include <directconnect_dp_platform_cfg.h>
#include "cppp/cppp.h"
#include "cppp/libcppp.h"

/* satt configuration*/
extern int netss_sattn_iosf_prim_set(int idx, phys_addr_t brg_ba,
			    uint32_t sap_size);
extern void netss_sattn_get(int idx, uint32_t *ctrl, uint32_t *sap_ba,
			    uint32_t *sap_size, uint32_t *brg_ba);

/* acceleration hooks */
extern Int32 hil_egress_hook(struct sk_buff* skb);
extern Int32 hil_ingress_hook(struct sk_buff* skb);

/* module arguments */
static int pci_count = 2; /* Default */
module_param(pci_count, int, 0);

/* defines */
#define XRX750_DCMODE1_MAX_DEV_NUM       4
#define XRX750_DCMODE1_MAX_DEV_PORT_NUM  (1 + XRX750_DCMODE1_MAX_DEV_NUM + 2)
				/*! 0 - Reserved, {5,6} - Reserved as dc s/w port (dummy) */
#define XRX750_DCMODE1_MAX_SUBIF_PER_DEV 32
#define XRX750_DCMODE1_SUBIFID_OFFSET    8
#define XRX750_DCMODE1_SUBIFID_MASK      0x1F
#define XRX750_DCMODE1_GET_SUBIFIDX(subif_id) \
	((subif_id >> XRX750_DCMODE1_SUBIFID_OFFSET) & XRX750_DCMODE1_SUBIFID_MASK)
#define XRX750_DCMODE1_GET_BITS(var, pos, width) \
	(((var) >> (pos)) & ((0x1 << (width)) - 1))
#define XRX750_DCMODE1_SET_BITS(var, pos, width, val) \
	do { \
		(var) &= ~(((0x1 << (width)) - 1) << (pos)); \
		(var) |= (((val) & ((0x1 << (width)) - 1)) << (pos)); \
	} while (0)
#ifdef SUPPORT_32_VAPS_ENABLE
#define MULTIPORT_WORKAROUND_MAX_SUBIF_NUM 16
#else
#define MULTIPORT_WORKAROUND_MAX_SUBIF_NUM 8
#endif
#define MULTIPORT_WORKAROUND_SUBIFID_MASK \
	(MULTIPORT_WORKAROUND_MAX_SUBIF_NUM << XRX750_DCMODE1_SUBIFID_OFFSET)

#define DC_DP_MAX_RING_SIZE		4096
#define DC_DP_MAX_RX_BUFS		0xC000 /* 48K */
#define DC_DP_MAX_TX_BUFS		0x8800 /* 34K */
#define DC_DP_EXPECTED_NUM_DCCNTR	1
#define DC_DP_EXPECTED_DCCNTR_LEN	4
#define DC_DP_SOCW_DCCNTR_MODE \
	(DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN)

#define DC_DP_DEVW_DCCNTR_MODE \
	(DC_DP_F_DCCNTR_MODE_CUMULATIVE | DC_DP_F_DCCNTR_MODE_BIG_ENDIAN)

#define PP_BUFFER_HEADROOM		128
#define DC_DP_TOTAL_BUF_SIZE		2048
#define DC_DP_REAL_BUF_SIZE \
	(DC_DP_TOTAL_BUF_SIZE - PP_BUFFER_HEADROOM)

#define DC_DP_PID_BASE PP_WiFi_PORT0_PID_NUM
#define DC_DP_CID_BASE CPPP_ID_WIFI_0
#define DC_DP_PID_TO_CID(_p) (DC_DP_CID_BASE + ((_p) - DC_DP_PID_BASE))

#define DC_DP_SATT_BASE_IDX 5 /* ATOM controls SATT entries 5-8 */
#define DC_DP_PID_TO_SATT_IDX(_p) (DC_DP_SATT_BASE_IDX + ((_p) - DC_DP_PID_BASE))

#define DC_DP_DESC_NUM_DWORDS 4

#define DC_DP_DESC_MASK_DWORD0	0xFFFF		/* bits 0 - 15 */
#define DC_DP_DESC_MASK_DWORD1	0		/* none */
#define DC_DP_DESC_MASK_DWORD2	0		/* none */
#define DC_DP_DESC_MASK_DWORD3	0xF		/* bits 24 - 27 */

#define DC_DP_DESC_SHIFT_DWORD0	0
#define DC_DP_DESC_SHIFT_DWORD1	0
#define DC_DP_DESC_SHIFT_DWORD2	0
#define DC_DP_DESC_SHIFT_DWORD3	24


/* hard coded - should be used only for debug! */
#define NETIP_ARM_BASE 0xF0000000
#define NETIP_PCI_BASE 0x40000000
#define NETIP_P2V(p) \
	    (p ? (netip_mmio_to_virtual( \
	    (unsigned long)(((p) - NETIP_PCI_BASE) + NETIP_ARM_BASE))) : 0)

#define min4(a, b, c, d) min(a, min(b, min(c, d)))
#define max4(a, b, c, d) max(a, max(b, max(c, d)))
#define abs_offset(a, b) max(a, b) - min(a, b)

typedef struct xrx750_dcmode1_subif_info {
	struct net_device  *netif;   /* pointer to net_device */
} xrx750_dcmode1_subif_info_t;

typedef struct xrx750_dcmode1_dev_shared_info {
	int32_t status;
	int32_t ref_count;
	int32_t start_port_id; /* Multiport reference port id */
	int32_t alt_port_id; /* Multiport reference alternate port id */
	int32_t cid; /* cppp client-id */
	int32_t satt_idx; /* cppp client-id */
	void * handle; /* cppp handle */

	uint32_t soc2dev_enq_satt;
	uint32_t soc2dev_ret_deq_satt;
	uint32_t dev2soc_deq_satt;
	uint32_t dev2soc_ret_enq_satt;

	/* resources */
	struct dc_dp_ring_res rings;
	int32_t num_bufpools;
	struct dc_dp_buf_pool *buflist;
	int32_t tx_num_bufpools;
	struct dc_dp_buf_pool *tx_buflist;
	struct dc_dp_dccntr dccntr;

	/* Subif info */
	int32_t num_subif;
	xrx750_dcmode1_subif_info_t subif_info[XRX750_DCMODE1_MAX_SUBIF_PER_DEV];
} xrx750_dcmode1_dev_shared_info_t;

typedef struct xrx750_dcmode1_dev_info {
#define XRX750_DCMODE1_DEV_STATUS_FREE    0x0
#define XRX750_DCMODE1_DEV_STATUS_USED    0x1
	int32_t status;
	uint32_t alloc_flags;

	/* shared info */
	xrx750_dcmode1_dev_shared_info_t *shared_info;
} xrx750_dcmode1_dev_info_t;
static struct xrx750_dcmode1_dev_shared_info g_dcmode1_dev_shinfo[XRX750_DCMODE1_MAX_DEV_NUM];
static struct xrx750_dcmode1_dev_info g_dcmode1_dev[XRX750_DCMODE1_MAX_DEV_PORT_NUM];

char *dc_dp_subif_status_str[] = {
	"DC_DP_SUBIF_FREE",
	"DC_DP_SUBIF_REGISTERED",
};

char *dc_dp_port_status_str[] = {
	"DC_DP_DEV_FREE",
	"DC_DP_DEV_PORT_ALLOCATED",
	"DC_DP_DEV_REGISTERED",
	"DC_DP_DEV_SUBIF_REGISTERED",
};

#define DC_DP_DBG(fmt, args...) \
	do { \
		DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, fmt, ##args); \
	} while (0)

static inline bool is_valid_registered_port(int32_t port_id)
{
	if (port_id <= 0 ||
	    port_id >= XRX750_DCMODE1_MAX_DEV_PORT_NUM)
		return 0;

	if (g_dcmode1_dev[port_id].status == XRX750_DCMODE1_DEV_STATUS_FREE)
		return 0;

	return 1;
}

static inline bool is_multiport(uint32_t alloc_flags)
{
	return (0 != (alloc_flags & DC_DP_F_MULTI_PORT));
}

static inline bool is_multiport_main(uint32_t alloc_flags)
{
	return ( (0 != (alloc_flags & DC_DP_F_MULTI_PORT)) &&
		 (0 == (alloc_flags & DC_DP_F_SHARED_RES)) );
}

static inline bool is_multiport_sub(uint32_t alloc_flags)
{
	return ( (0 != (alloc_flags & DC_DP_F_MULTI_PORT)) &&
		 (0 != (alloc_flags & DC_DP_F_SHARED_RES)) );
}

static inline bool is_multiport_main_by_subif(uint16_t subif_id)
{
	return (XRX750_DCMODE1_GET_SUBIFIDX(subif_id) >= MULTIPORT_WORKAROUND_MAX_SUBIF_NUM);
}

static inline bool is_multiport_sub_by_subif(uint16_t subif_id)
{
	return (XRX750_DCMODE1_GET_SUBIFIDX(subif_id) < MULTIPORT_WORKAROUND_MAX_SUBIF_NUM);
}

static inline void
multiport_wa_forward_map_subifid(xrx750_dcmode1_dev_info_t *dev_ctx, dp_subif_t *subif_id)
{
	/* Multiport 1st device? */
	if ( is_multiport_main(dev_ctx->alloc_flags) )
		subif_id->subif |= MULTIPORT_WORKAROUND_SUBIFID_MASK;
	/* Multiport sub-sequent device? */
	else
		subif_id->port_id = dev_ctx->shared_info->start_port_id;
}

static inline void
multiport_wa_reverse_map_subifid(xrx750_dcmode1_dev_info_t *dev_ctx, dp_subif_t *subif_id)
{
	/* Multiport 1st device? */
	if ( is_multiport_main_by_subif(subif_id->subif) )
		subif_id->subif &= ~MULTIPORT_WORKAROUND_SUBIFID_MASK;

	/* Multiport sub-sequent device? */
	else
		subif_id->port_id = dev_ctx->shared_info->alt_port_id;
}

static void inline dc_dp_dump_dccntr_mode(int32_t dccntr_mode, char *ret)
{
	ret[0] = '\0';

	if (dccntr_mode & DC_DP_F_DCCNTR_MODE_INCREMENTAL)
		strcat(ret, "INCREMENTAL:");
	if (dccntr_mode & DC_DP_F_DCCNTR_MODE_CUMULATIVE)
		strcat(ret, "CUMULATIVE:");
	if (dccntr_mode & DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN)
		strcat(ret, "LE:");
	if (dccntr_mode & DC_DP_F_DCCNTR_MODE_BIG_ENDIAN)
		strcat(ret, "BE:");

	ret[strlen(ret)] = '\0';
}

static void inline dc_dp_dump_dccntr(struct dc_dp_dccntr *dccntr)
{

	char soc_mode_str[128];
	char dev_mode_str[128];

	dc_dp_dump_dccntr_mode(dccntr->soc_write_dccntr_mode, soc_mode_str);
	dc_dp_dump_dccntr_mode(dccntr->dev_write_dccntr_mode, dev_mode_str);

	DC_DP_DBG("soc_write_dccntr_mode      [%s]\n", soc_mode_str);
	DC_DP_DBG("dev_write_dccntr_mode      [%s]\n", dev_mode_str);
	DC_DP_DBG("soc2dev_enq_phys_base      [0x%pK]\n", dccntr->soc2dev_enq_phys_base);
	DC_DP_DBG("soc2dev_enq_base           [0x%pK]\n", dccntr->soc2dev_enq_base);
	DC_DP_DBG("soc2dev_enq_dccntr_len     [%d]\n", dccntr->soc2dev_enq_dccntr_len);
	DC_DP_DBG("soc2dev_ret_deq_phys_base  [0x%pK]\n", dccntr->soc2dev_ret_deq_phys_base);
	DC_DP_DBG("soc2dev_ret_deq_base       [0x%pK]\n", dccntr->soc2dev_ret_deq_base);
	DC_DP_DBG("soc2dev_ret_deq_dccntr_len [%d]\n", dccntr->soc2dev_ret_deq_dccntr_len);
	DC_DP_DBG("dev2soc_deq_phys_base      [0x%pK]\n", dccntr->dev2soc_deq_phys_base);
	DC_DP_DBG("dev2soc_deq_base           [0x%pK]\n", dccntr->dev2soc_deq_base);
	DC_DP_DBG("dev2soc_deq_dccntr_len     [%d]\n", dccntr->dev2soc_deq_dccntr_len);
	DC_DP_DBG("dev2soc_ret_enq_phys_base  [0x%pK]\n", dccntr->dev2soc_ret_enq_phys_base);
	DC_DP_DBG("dev2soc_ret_enq_base       [0x%pK]\n", dccntr->dev2soc_ret_enq_base);
	DC_DP_DBG("dev2soc_ret_enq_dccntr_len [%d]\n", dccntr->dev2soc_ret_enq_dccntr_len);
	DC_DP_DBG("soc2dev_deq_phys_base      [0x%pK]\n", dccntr->soc2dev_deq_phys_base);
	DC_DP_DBG("soc2dev_deq_base           [0x%pK]\n", dccntr->soc2dev_deq_base);
	DC_DP_DBG("soc2dev_deq_dccntr_len     [%d]\n", dccntr->soc2dev_deq_dccntr_len);
	DC_DP_DBG("soc2dev_ret_enq_phys_base  [0x%pK]\n", dccntr->soc2dev_ret_enq_phys_base);
	DC_DP_DBG("soc2dev_ret_enq_base       [0x%pK]\n", dccntr->soc2dev_ret_enq_base);
	DC_DP_DBG("soc2dev_ret_enq_dccntr_len [%d]\n", dccntr->soc2dev_ret_enq_dccntr_len);
	DC_DP_DBG("dev2soc_enq_phys_base      [0x%pK]\n", dccntr->dev2soc_enq_phys_base);
	DC_DP_DBG("dev2soc_enq_base           [0x%pK]\n", dccntr->dev2soc_enq_base);
	DC_DP_DBG("dev2soc_enq_dccntr_len     [%d]\n", dccntr->dev2soc_enq_dccntr_len);
	DC_DP_DBG("dev2soc_ret_deq_phys_base  [0x%pK]\n", dccntr->dev2soc_ret_deq_phys_base);
	DC_DP_DBG("dev2soc_ret_deq_base       [0x%pK]\n", dccntr->dev2soc_ret_deq_base);
	DC_DP_DBG("dev2soc_ret_deq_dccntr_len [%d]\n", dccntr->dev2soc_ret_deq_dccntr_len);
	DC_DP_DBG("flags                      [0x%x]\n", dccntr->flags);
}

static void inline dc_dp_dump_ring(struct dc_dp_ring *ring, char *name)
{
	DC_DP_DBG("name         %s\n", name);
	DC_DP_DBG("base         0x%pK\n", ring->base);
	DC_DP_DBG("phys_base    0x%pK\n", ring->phys_base);
	DC_DP_DBG("size         %d\n", ring->size);
	DC_DP_DBG("flags        0x%x\n", ring->flags);
	DC_DP_DBG("ring         0x%pK\n\n", ring->ring);
}

static void inline dc_dp_dump_resources(char *prefix, struct dc_dp_res *res)
{
	DC_DP_DBG("%s\n", prefix);
	DC_DP_DBG("num_bufs_req %d\n", res->num_bufs_req);
	DC_DP_DBG("num_bufpools %d\n", res->num_bufpools);
	DC_DP_DBG("buflist      0x%pK\n", res->buflist);

	DC_DP_DBG("tx_num_bufs_req %d\n", res->tx_num_bufs_req);
	DC_DP_DBG("tx_num_bufpools %d\n", res->tx_num_bufpools);
	DC_DP_DBG("tx_buflist      0x%pK\n", res->tx_buflist);

	DC_DP_DBG("num_dccntr   %d\n", res->num_dccntr);
	DC_DP_DBG("dev_priv_res 0x%pK\n\n", res->dev_priv_res);

	dc_dp_dump_ring(&res->rings.soc2dev, "soc2dev");
	dc_dp_dump_ring(&res->rings.soc2dev_ret, "soc2dev_ret");
	dc_dp_dump_ring(&res->rings.dev2soc, "dev2soc");
	dc_dp_dump_ring(&res->rings.dev2soc_ret, "dev2soc_ret");

	dc_dp_dump_dccntr(res->dccntr);
}

static int32_t dc_dp_validate_resources(struct dc_dp_res *res, xrx750_dcmode1_dev_info_t *dev_ctx)
{
	struct dc_dp_dccntr *cnt;

	if (!res->rings.dev2soc.size ||
	    !res->rings.dev2soc_ret.size ||
	    !res->rings.soc2dev.size||
	    !res->rings.soc2dev_ret.size) {
		DC_DP_ERROR("ERROR: did not get all ring sizes\n");
		return DC_DP_FAILURE;
	}

	if (res->rings.dev2soc.size > DC_DP_MAX_RING_SIZE) {
		DC_DP_ERROR("WARNING: dev2soc.size > max (%d), trimming to max\n",
			DC_DP_MAX_RING_SIZE);
		res->rings.dev2soc.size = DC_DP_MAX_RING_SIZE;
	}

	if (res->rings.dev2soc_ret.size > DC_DP_MAX_RING_SIZE) {
		DC_DP_ERROR("WARNING: dev2soc_ret.size > max (%d), trimming to max\n",
			DC_DP_MAX_RING_SIZE);
		res->rings.dev2soc_ret.size = DC_DP_MAX_RING_SIZE;
	}

	if (res->rings.soc2dev.size > DC_DP_MAX_RING_SIZE) {
		DC_DP_ERROR("WARNING: soc2dev.size > max (%d), trimming to max\n",
			DC_DP_MAX_RING_SIZE);
		res->rings.soc2dev.size = DC_DP_MAX_RING_SIZE;
	}

	if (res->rings.soc2dev_ret.size > DC_DP_MAX_RING_SIZE) {
		DC_DP_ERROR("WARNING: soc2dev_ret.size > max (%d), trimming to max\n",
			DC_DP_MAX_RING_SIZE);
		res->rings.soc2dev_ret.size = DC_DP_MAX_RING_SIZE;
	}

    if (dev_ctx->alloc_flags & DC_DP_F_FAST_DSL) {
        if (res->num_bufs_req != res->rings.dev2soc_ret.size - 1) {
            DC_DP_ERROR("WARNING: res->num_bufs_req != res->rings.soc2dev.size, aligning to ring size\n");
            res->num_bufpools = res->rings.dev2soc_ret.size - 1;
        }

        if (res->tx_num_bufs_req > res->rings.soc2dev.size - 1) {
            DC_DP_ERROR("WARNING: res->tx_num_bufs_req > res->rings.soc2dev.size, aligning to ring size\n");
            res->tx_num_bufpools = res->rings.soc2dev.size - 1;
        }
    }

	if (res->num_dccntr != DC_DP_EXPECTED_NUM_DCCNTR) {
		DC_DP_ERROR("ERROR: unexpected num_dccntr %d\n", res->num_dccntr);
		return DC_DP_FAILURE;
	}

	if (!res->dccntr) {
		DC_DP_ERROR("ERROR: missing dccntr!\n");
		return DC_DP_FAILURE;
	}

	cnt = res->dccntr;

	if (cnt->soc_write_dccntr_mode != DC_DP_SOCW_DCCNTR_MODE) {
		DC_DP_ERROR("ERROR: unexpected dccnte_mode %d\n",
			res->dccntr->soc_write_dccntr_mode);
		return DC_DP_FAILURE;
	}


	if (cnt->soc2dev_enq_dccntr_len     != DC_DP_EXPECTED_DCCNTR_LEN ||
	    cnt->soc2dev_ret_deq_dccntr_len != DC_DP_EXPECTED_DCCNTR_LEN ||
	    cnt->dev2soc_deq_dccntr_len     != DC_DP_EXPECTED_DCCNTR_LEN ||
	    cnt->dev2soc_ret_enq_dccntr_len != DC_DP_EXPECTED_DCCNTR_LEN) {
		DC_DP_ERROR("ERROR: unexpected dccnet_len %d,%d,%d,%d\n",
			cnt->soc2dev_enq_dccntr_len,
			cnt->soc2dev_ret_deq_dccntr_len,
			cnt->dev2soc_deq_dccntr_len,
			cnt->dev2soc_ret_enq_dccntr_len);
		return DC_DP_FAILURE;
	}

	if (!cnt->soc2dev_enq_phys_base || !cnt->soc2dev_enq_base ||
	    !cnt->soc2dev_ret_deq_phys_base || !cnt->soc2dev_ret_deq_base ||
	    !cnt->dev2soc_deq_phys_base || !cnt->dev2soc_deq_base ||
	    !cnt->dev2soc_ret_enq_phys_base || !cnt->dev2soc_ret_enq_base) {
		DC_DP_ERROR("ERROR: missing counter info!\n");
		return DC_DP_FAILURE;
	}

	return DC_DP_SUCCESS;
}

static int32_t dc_dp_get_satt_ba(xrx750_dcmode1_dev_shared_info_t *plat,
				   uint32_t brg_ba)
{
	uint32_t dummy, sap_ba;

	netss_sattn_get(plat->satt_idx, &dummy, &sap_ba, &dummy, &dummy);

	/* satt bridge-address has to be 16b aliged,
	   if needed add the offset */

	return sap_ba + (brg_ba & 0xF);

}

static int dc_dp_configure_satt(xrx750_dcmode1_dev_shared_info_t *plat,
				   uint32_t brg_ba, uint32_t size)
{
	uint32_t adj_size;

	/* satt bridge-address has to be 16b aliged,
	   if needed align the address and increase size */

	adj_size = (brg_ba & 0xF) ? 0x10 + size : size;
	brg_ba = (brg_ba & ~0xF);

	return netss_sattn_iosf_prim_set(plat->satt_idx, brg_ba, adj_size);
}

static int32_t dc_dp_prepare_resources(struct dc_dp_res *res,
				       xrx750_dcmode1_dev_info_t *dev_ctx,
				       uint32_t port_id)
{
	xrx750_dcmode1_dev_shared_info_t *plat = dev_ctx->shared_info;
	int i, bufs_recv = 0;
	cppp_bd_t *desc = NULL;
	cppp_create_t create = {0};
	uint32_t cppp_create_flags;
	struct dc_dp_buf_pool *pool;
	uint32_t satt_brg_base;
	struct dc_dp_dccntr *cnt = res->dccntr;
	struct sk_buff *dummy = NULL;

	uint32_t soc2dev_enq_satt;
	uint32_t soc2dev_ret_deq_satt;
	uint32_t dev2soc_deq_satt;
	uint32_t dev2soc_ret_enq_satt;

	uint32_t dev_cnt_min = min4((uint32_t)cnt->soc2dev_enq_phys_base,
				    (uint32_t)cnt->soc2dev_ret_deq_phys_base,
				    (uint32_t)cnt->dev2soc_deq_phys_base,
				    (uint32_t)cnt->dev2soc_ret_enq_phys_base);

	uint32_t dev_cnt_max = max4((uint32_t)cnt->soc2dev_enq_phys_base,
				    (uint32_t)cnt->soc2dev_ret_deq_phys_base,
				    (uint32_t)cnt->dev2soc_deq_phys_base,
				    (uint32_t)cnt->dev2soc_ret_enq_phys_base);

	/* ring sizes */
	create.recv.ents = res->rings.soc2dev.size;
	create.send.ents = res->rings.dev2soc.size;
	create.alloc.ents = res->rings.dev2soc_ret.size;
	create.free.ents = res->rings.soc2dev_ret.size;

	satt_brg_base = dc_dp_get_satt_ba(plat, dev_cnt_min);


	soc2dev_enq_satt = satt_brg_base +
	    abs_offset((uint32_t)cnt->soc2dev_enq_phys_base, dev_cnt_min);
	soc2dev_ret_deq_satt = satt_brg_base +
	    abs_offset((uint32_t)cnt->soc2dev_ret_deq_phys_base, dev_cnt_min);
	dev2soc_deq_satt = satt_brg_base +
	    abs_offset((uint32_t)cnt->dev2soc_deq_phys_base, dev_cnt_min);
	dev2soc_ret_enq_satt = satt_brg_base +
	    abs_offset((uint32_t)cnt->dev2soc_ret_enq_phys_base, dev_cnt_min);

	/* dev counters */
	create.send.cnt_caller  = dev2soc_deq_satt;
	create.free.cnt_caller  = soc2dev_ret_deq_satt;
	create.recv.cnt_caller  = soc2dev_enq_satt;
	create.alloc.cnt_caller = dev2soc_ret_enq_satt;

	DC_DP_DBG("satt dev counters 0x%x 0x%x 0x%x 0x%x\n",
		dev2soc_deq_satt, soc2dev_ret_deq_satt, soc2dev_enq_satt,
		dev2soc_ret_enq_satt);

	/* configure satt - set size as diff between higher and lowest addr +
	   4 bytes for highest reg */
	if (dc_dp_configure_satt(plat, dev_cnt_min,
				(dev_cnt_max - dev_cnt_min) + 4 )) {
		DC_DP_ERROR("failed to configure satt\n");
		goto satt_err;
	}

	cppp_create_flags = CPPP_FLAG_UMM | CPPP_FLAG_DIR;
#ifdef CONFIG_HIGH_PPS
	create.free.max_ents_in_process = min(res->tx_num_bufs_req, (uint32_t) DC_DP_MAX_RX_BUFS);
	create.send.max_ents_in_process = min(res->num_bufs_req, (uint32_t) DC_DP_MAX_TX_BUFS);
	cppp_create_flags |= CPPP_FLAG_RCU;
#endif
	plat->handle = cppp_create(plat->cid, cppp_create_flags,
				   &create);
	if (!plat->handle) {
		DC_DP_ERROR("cppp_create failed for id %d\n", plat->cid);
		goto cppp_create_err;
	}

#ifdef CONFIG_HIGH_PPS
	res->num_bufpools = 0;
	res->tx_num_bufpools = 0;
#endif

    if (dev_ctx->alloc_flags & DC_DP_F_FAST_DSL) {
        if (!res->num_bufpools)
            res->num_bufpools = res->num_bufs_req;

        if (!res->tx_num_bufpools)
            res->tx_num_bufpools = res->tx_num_bufs_req;
    }

	if (res->num_bufpools || res->tx_num_bufpools) {
		/* allocate buffer array for both tx and rx */
		desc = kmalloc(max(res->num_bufpools, res->tx_num_bufpools) *
			       sizeof(cppp_bd_t), GFP_KERNEL);
		if (!desc) {
			DC_DP_ERROR("failed to alloc desc array\n");
			goto desc_alloc_err;
		}

		if (res->num_bufpools) {
			res->buflist = kmalloc(res->num_bufpools *
					       sizeof(struct dc_dp_buf_pool), GFP_KERNEL);
			if (!res->buflist) {
				DC_DP_ERROR("failed to alloc bufflist\n");
				goto buf_alloc_err;
			}
		}

		if (res->tx_num_bufpools) {
			res->tx_buflist = kmalloc(res->tx_num_bufpools *
					       sizeof(struct dc_dp_buf_pool), GFP_KERNEL);
			if (!res->tx_buflist) {
				DC_DP_ERROR("failed to alloc tx_bufflist\n");
				goto tx_buf_alloc_err;
			}
		}
	}

	if (cppp_start(plat->handle)) {
		DC_DP_ERROR("failed to start cppp for id %d\n", plat->cid);
		goto cppp_start_err;
	}

	/* force PP to put buffers on the TX ring by pushing dummy skb's */
	if (res->tx_num_bufpools) {

		dp_subif_t dummy_subif;
		dummy_subif.port_id = port_id;

		for (i = 0; i < res->tx_num_bufpools; i++) {
			dummy = alloc_skb(16, GFP_KERNEL);

			if (!dummy) {
				DC_DP_ERROR("failed to allocate dummy skb\n");
				goto dummy_skb_alloc_err;
			}
			skb_put(dummy, 16);
			dp_xmit(NULL, &dummy_subif, dummy, dummy->len, DP_XMIT_F_FORCE);
		}
	}

	/* TODO: delay 300m to allow FW to fill the ring */
	mdelay(300);

	/* rx preallocated buffers */
	if (res->num_bufpools) {
		bufs_recv = cppp_alloc(plat->handle, desc, res->num_bufpools, 0);


		if (bufs_recv != res->num_bufpools) {
			DC_DP_ERROR("failed to alloc buffers, alloc: %d, req %d\n",
					bufs_recv, res->num_bufpools);
			goto cppp_alloc_err;
		}

		pool = res->buflist;

		for (i = 0; i < res->num_bufpools; i++) {
			pool->pool = (void *)bus_to_virt(be32_to_cpu(desc[i].addr));
			BUG_ON(!pool->pool);
			pool->phys_pool = (void *)be32_to_cpu(desc[i].addr);
			pool->size = DC_DP_REAL_BUF_SIZE;
			pool++;
		}
	}


	/* tx preallocated buffers */
	if (res->tx_num_bufpools) {
		memset(desc, 0 , max(res->num_bufpools, res->tx_num_bufpools) *
				     sizeof(cppp_bd_t));

		bufs_recv = cppp_recv(plat->handle, desc, res->tx_num_bufpools, 0);

		if (bufs_recv != res->tx_num_bufpools) {
			DC_DP_ERROR("failed to alloc tx buffers, recv: %d, req %d\n",
					bufs_recv, res->tx_num_bufpools);
			goto cppp_recv_err;
		}

		pool = res->tx_buflist;

		for (i = 0; i < res->tx_num_bufpools; i++) {
			pool->pool = (void *)bus_to_virt(be32_to_cpu(desc[i].addr));
			BUG_ON(!pool->pool);
			pool->phys_pool = (void *)be32_to_cpu(desc[i].addr);
			pool->size = DC_DP_REAL_BUF_SIZE;
			pool++;
		}

		/* Update soc2dev ring start index to #tx_num_bufpools, as tx buffers are part of the soc2dev ring.
		   FIXME : Assuming soc2dev ring size is always power of 2? */
		res->tx_desc_start_idx = (res->tx_num_bufs_req & (res->rings.soc2dev.size - 1));
	}

	/* Update returned 'resource' structure */
	res->rings.dev2soc.base = phys_to_virt(create.send.addr);
	res->rings.dev2soc.phys_base = (void *)create.send.addr;
	res->rings.dev2soc.desc_dwsz = 4;

	res->rings.soc2dev_ret.base = phys_to_virt(create.free.addr);
	res->rings.soc2dev_ret.phys_base = (void *)create.free.addr;
	res->rings.soc2dev_ret.desc_dwsz = 4;

	res->rings.soc2dev.base = phys_to_virt(create.recv.addr);
	res->rings.soc2dev.phys_base = (void *)create.recv.addr;
	res->rings.soc2dev.desc_dwsz = 4;

	res->rings.dev2soc_ret.base = phys_to_virt(create.alloc.addr);
	res->rings.dev2soc_ret.phys_base = (void *)create.alloc.addr;
	res->rings.dev2soc_ret.desc_dwsz = 4;

	/* dev counters */
	cnt->soc2dev_deq_base = NETIP_P2V(create.recv.cnt_callee);
	cnt->soc2dev_deq_phys_base = (void *)create.recv.cnt_callee;
	cnt->soc2dev_deq_dccntr_len = 4;

	cnt->soc2dev_ret_enq_base = NETIP_P2V(create.free.cnt_callee);
	cnt->soc2dev_ret_enq_phys_base = (void *)create.free.cnt_callee;
	cnt->soc2dev_ret_enq_dccntr_len = 4;

	cnt->dev2soc_enq_base = NETIP_P2V(create.send.cnt_callee);
	cnt->dev2soc_enq_phys_base = (void *)create.send.cnt_callee;
	cnt->dev2soc_enq_dccntr_len = 4;

	cnt->dev2soc_ret_deq_base = NETIP_P2V(create.alloc.cnt_callee);
	cnt->dev2soc_ret_deq_phys_base = (void *)create.alloc.cnt_callee;
	cnt->dev2soc_ret_deq_dccntr_len = 4;

	/* FW counter mode */
	cnt->dev_write_dccntr_mode = DC_DP_DEVW_DCCNTR_MODE;

	/* save satt entries for debug;*/
	plat->soc2dev_enq_satt     = soc2dev_enq_satt;
	plat->soc2dev_ret_deq_satt = soc2dev_ret_deq_satt;
	plat->dev2soc_deq_satt     = dev2soc_deq_satt;
	plat->dev2soc_ret_enq_satt = dev2soc_ret_enq_satt;

	if (desc)
		kfree(desc);

	return DC_DP_SUCCESS;

cppp_recv_err:
cppp_alloc_err:
dummy_skb_alloc_err:
	cppp_stop(plat->handle);
cppp_start_err:
	if (res->tx_num_bufpools)
		kfree(res->tx_buflist);
tx_buf_alloc_err:
	if (res->num_bufpools)
		kfree(res->buflist);
buf_alloc_err:
	if (desc)
		kfree(desc);
desc_alloc_err:
	cppp_destroy(plat->handle);
cppp_create_err:
satt_err:
	return DC_DP_FAILURE;
}

static int32_t
xrx750_dcmode1_rx_cb(struct net_device *rxif, struct net_device *txif,
			struct sk_buff *skb, int32_t len)
{
//	struct dma_rx_desc_0 *desc_0 = (struct dma_rx_desc_0 *)&skb->DW0;
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;
//	struct dma_rx_desc_2 *desc_2 = (struct dma_rx_desc_2 *)&skb->DW2;
//	struct dma_rx_desc_3 *desc_3 = (struct dma_rx_desc_3 *)&skb->DW3;

	dp_subif_t rx_subif = {0};
	struct wifi_proxy_psi_t *psi;
	struct xrx750_dcmode1_dev_info *dev_ctx = NULL;

	if (!skb) {
		DC_DP_ERROR("failed to receive as skb=%p!!!\n", skb);
		return -1;
	}

	if (!rxif) {
		DC_DP_ERROR("failed to receive as rxif=%p!!!\n", rxif);
		goto drop;
	}

	psi = pdp_skb_get_psi(skb);

	/* get the subif information from the skb, that was copied
	   to skb from the rx descriptor */
	rx_subif.subif = be16_to_cpu(psi->wifi_session_info);

	/* set class - TODO: might not be needed on RX */
	desc_1->field.classid = (psi->wifi_class & 0xF);

	/* set port id that was assigned in DP layer*/
	rx_subif.port_id = desc_1->field.ep;

	if ( !is_valid_registered_port(rx_subif.port_id) ) {
		DC_DP_ERROR("dropping, not a valid registered port!!!\n");
		goto drop;
	}

	/* Multiport device? */
	dev_ctx = &g_dcmode1_dev[rx_subif.port_id];
	if ( is_multiport(dev_ctx->alloc_flags) )
		multiport_wa_reverse_map_subifid(dev_ctx, &rx_subif);

	return dc_dp_rx(rxif, txif, &rx_subif, skb, skb->len, 0);

drop:
	if (skb)
		dev_kfree_skb_any(skb);

	return -1;
}

void dc_dp_dw_to_psi(struct wifi_proxy_psi_t *psi, struct dc_dp_fields_dw *dw)
{
	switch(dw->dw) {
	case 0:
		psi->wifi_session_info =
			cpu_to_be16((dw->desc_val & dw->desc_mask)
				    >> DC_DP_DESC_SHIFT_DWORD0);
		break;
	case 3:
		psi->wifi_class = (dw->desc_val & dw->desc_mask) >> DC_DP_DESC_SHIFT_DWORD3;
		break;
	default:
		DC_DP_ERROR("incorrect dw number %d", dw->dw);
	}
}

int32_t
dc_dp_set_pkt_psi(struct net_device *netif, struct sk_buff *skb,
		    dp_subif_t *subif, uint32_t flags)
{
	int32_t i;
	uint32_t mask;
	struct wifi_proxy_psi_t *psi = NULL;
	struct dp_subif subif_id = {0};
	xrx750_dcmode1_dev_info_t *dev_ctx = NULL;

	struct dc_dp_fields_dw dw[DC_DP_DESC_NUM_DWORDS];
	struct dc_dp_fields_value_dw vdw;

	if (!netif || !subif)
		return DC_DP_FAILURE;

	if ( !is_valid_registered_port(subif->port_id) ) {
		DC_DP_ERROR("failed, not a valid registered port!!!\n");
		return DC_DP_FAILURE;
	}

	vdw.num_dw = DC_DP_DESC_NUM_DWORDS;
	vdw.dw = dw;
	subif_id = *subif;

	dev_ctx = &g_dcmode1_dev[subif_id.port_id];
	if ( is_multiport(dev_ctx->alloc_flags) ) {
		struct xrx750_dcmode1_dev_shared_info *dev_shared_ctx = dev_ctx->shared_info;
		int32_t subif_idx;

		if (!dev_shared_ctx) {
			DC_DP_ERROR("failed, not a valid registered multiport datapath device=%s!!!\n",
					(netif ? netif->name : "NULL"));
			return DC_DP_FAILURE;
		}

		/* Some case, not a valid subif from DPLib in set_pkt_psi_fn CB */
		subif_idx = XRX750_DCMODE1_GET_SUBIFIDX(subif_id.subif);
		if (dev_shared_ctx->subif_info[subif_idx].netif != netif) {
			for (subif_idx = 0; subif_idx < XRX750_DCMODE1_MAX_SUBIF_PER_DEV; subif_idx++) {
				if (dev_shared_ctx->subif_info[subif_idx].netif == netif) {
					subif_id.subif &= ~(XRX750_DCMODE1_SUBIFID_MASK << XRX750_DCMODE1_SUBIFID_OFFSET);
					subif_id.subif |= (subif_idx << XRX750_DCMODE1_SUBIFID_OFFSET);
					break;
				}
			}

			if (subif_idx == XRX750_DCMODE1_MAX_SUBIF_PER_DEV) {
				DC_DP_ERROR("No matching netif=%p!!!\n", netif);
				return DC_DP_FAILURE;
			}
		}

		/* Multiport sub-sequent device? */
		if ( is_multiport_sub_by_subif(subif_id.subif) )
			subif_id.port_id = dev_ctx->shared_info->alt_port_id;
	}

	if (dc_dp_get_dev_specific_desc_info(subif_id.port_id, skb, &vdw, 0))
		return DC_DP_FAILURE;

	psi = pdp_skb_get_psi(skb);

	for (i = 0; i < vdw.num_dw; i++) {
		switch(dw[i].dw) {
		case 0:
			mask = (DC_DP_DESC_MASK_DWORD0 << DC_DP_DESC_SHIFT_DWORD0);
			/* Multiport 1st device? */
			if ( is_multiport(dev_ctx->alloc_flags) &&
			     is_multiport_main_by_subif(subif_id.subif) ) {
					dw[i].desc_val |= MULTIPORT_WORKAROUND_SUBIFID_MASK;
			}
			break;
		case 1:
			mask = (DC_DP_DESC_MASK_DWORD1 << DC_DP_DESC_SHIFT_DWORD1);
			break;
		case 2:
			mask = (DC_DP_DESC_MASK_DWORD2 << DC_DP_DESC_SHIFT_DWORD2);
			break;
		case 3:
			mask = (DC_DP_DESC_MASK_DWORD3 << DC_DP_DESC_SHIFT_DWORD3);
			break;
		default:
			DC_DP_ERROR("incorrect dw number %d", dw[i].dw);
			return DC_DP_FAILURE;
		}

		if (dw[i].desc_mask & ~mask) {
			DC_DP_ERROR("incorrect dw mask %d, 0x%x",
				dw[i].dw, dw[i].desc_mask);
			return DC_DP_FAILURE;
		}

		dc_dp_dw_to_psi(psi, &dw[i]);
	}

	dc_dp_set_mark_pkt(subif_id.port_id, skb, 0);

	return DC_DP_SUCCESS;
}

static int32_t
xrx750_dcmode1_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
				 void *subif_data, uint8_t dst_mac[MAX_ETH_ALEN],
				 dp_subif_t *subif, uint32_t flags)
{
	int32_t ret = 1;
	struct dp_subif subif_id = {0};
	struct xrx750_dcmode1_dev_info *dev_ctx;

	/* Validate input argument */
	if (!netif) {
		DC_DP_ERROR("failed to get subifid as netif=%p!!!\n", netif);
		goto out;
	}

	if (!subif) {
		DC_DP_ERROR("failed to get subifid as subif=%p!!!\n", subif);
		goto out;
	}

	if ( !is_valid_registered_port(subif->port_id)) {
		DC_DP_ERROR("failed, not a valid registered port!!!\n");
		goto out;
	}

	subif_id = *subif;

	dev_ctx = &g_dcmode1_dev[subif_id.port_id];
	if ( is_multiport(dev_ctx->alloc_flags) )
		multiport_wa_reverse_map_subifid(dev_ctx, &subif_id);

	ret = dc_dp_get_netif_subifid(netif, skb, subif_data, dst_mac, &subif_id, flags);
	if (ret)
		goto out;

	subif->subif &= ~0x00FF;
	subif->subif |= (subif_id.subif & 0x00FF);

out:
	return ret;
}

int32_t
xrx750_dcmode1_get_peripheral_config(int32_t port_id,
                                 enum dc_dp_dev_type dev_id,
                                 struct dc_dp_dev_config *dev_config)
{
    int32_t ret = DC_DP_SUCCESS;
    /* We dynamically allocate resources to 5G and 2.4G interfaces,
          According to Tri-Band or Dual-Band mode.*/
    if (dev_id == DC_DP_DEV_WAV_600_24G && pci_count == 3) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G;
    } else if ((dev_id == DC_DP_DEV_WAV_600_5G && pci_count == 3) ||
            (dev_id == DC_DP_DEV_WAV_600_24G && pci_count < 3)) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G;
    } else if (dev_id == DC_DP_DEV_WAV_600_6G ||
            (dev_id == DC_DP_DEV_WAV_600_5G && pci_count < 3)) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G;
    }
    return ret;
}

int32_t
xrx750_dcmode1_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
	int32_t ret = -1;

	if (cap) {
		cap->fastpath.support = 1;
		cap->fastpath.hw_dcmode = DC_DP_MODE_TYPE_1;

		cap->fastpath.hw_cmode.soc2dev_write = DC_DP_SOCW_DCCNTR_MODE;
		cap->fastpath.hw_cmode.dev2soc_write = DC_DP_DEVW_DCCNTR_MODE;

		ret = 0;
	}

	return ret;
}

int32_t
xrx750_dcmode1_register_dev_ex(void *ctx, struct module *owner, uint32_t port_id,
		       struct net_device *dev, struct dc_dp_cb *datapathcb,
		       struct dc_dp_res *resources, struct dc_dp_dev *devspec,
		       int32_t start_port_id, uint32_t alloc_flags, uint32_t flags)
{
	int32_t ret, pid;
	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;
	dp_cb_t dp_cb = {0};
	int32_t dev_sh_idx = 0;

	DC_DP_DBG("dev_ctx=%p, owner=%p, port_id=%u, dev=%p, datapathcb=%p, \
		  resources=%p, dev_spec=%p, flags=0x%08X\n",
		  dev_ctx, owner, port_id, dev, datapathcb, resources, devspec, flags);

	/* De-register */
	if (flags & DC_DP_F_DEREGISTER) {
		if (!dev_ctx || (dev_ctx && !dev_ctx->shared_info)) {
			DC_DP_ERROR("failed to de-register device for the port_id %d!!!\n", port_id);
			return DC_DP_FAILURE;
		}

		/* De-register device from DC Common layer */
		dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, DC_DP_DCMODE_DEV_DEREGISTER);

		dev_ctx->shared_info->ref_count--;
		if (0 == dev_ctx->shared_info->ref_count) {
			/* De-register device from DP Lib */
			ret = dp_register_dev(owner, dev_ctx->shared_info->start_port_id, &dp_cb, DP_F_DEREGISTER);
			if (ret != DP_SUCCESS) {
				DC_DP_ERROR("failed to de-register device for the port_id %d!!!\n", port_id);
				return DC_DP_FAILURE;
			}

			cppp_stop(dev_ctx->shared_info->handle);

			cppp_destroy(dev_ctx->shared_info->handle);
			memset(dev_ctx->shared_info, 0, sizeof(xrx750_dcmode1_dev_shared_info_t));
		}

		memset(dev_ctx, 0, sizeof(xrx750_dcmode1_dev_info_t));

		DC_DP_DBG("Success, returned %d.\n", DC_DP_SUCCESS);
		return DC_DP_SUCCESS;
	}

	/* NOTE : here dev_ctx should be NULL */
	/* FIXME : any lock to protect private members across different call??? */

	/* Find a free device context */
	if ((port_id <= 0) || (port_id >= XRX750_DCMODE1_MAX_DEV_PORT_NUM)) {
		DC_DP_ERROR("failed to register dev as port_id=%d beyond limit [1-%d]!!!\n",
			    port_id, (XRX750_DCMODE1_MAX_DEV_PORT_NUM - 1));
		goto err_out;
	}

	if (g_dcmode1_dev[port_id].status != XRX750_DCMODE1_DEV_STATUS_FREE) {
		DC_DP_ERROR("failed to register dev as port_id=%d already registerd!!!\n", port_id);
		goto err_out;
	}
	dev_ctx = &g_dcmode1_dev[port_id];

	/* Find the device shared context, to be referenced */
	if ( is_multiport_sub(alloc_flags) ) {
		for (dev_sh_idx = 0; dev_sh_idx < XRX750_DCMODE1_MAX_DEV_NUM; dev_sh_idx++) {
			if ( (g_dcmode1_dev_shinfo[dev_sh_idx].status == XRX750_DCMODE1_DEV_STATUS_USED) &&
			     (g_dcmode1_dev_shinfo[dev_sh_idx].ref_count == 1) &&
			     (g_dcmode1_dev_shinfo[dev_sh_idx].start_port_id == start_port_id) ) {
				break;
			}
		}

		if (dev_sh_idx >= XRX750_DCMODE1_MAX_DEV_NUM) {
			DC_DP_ERROR("failed to register dev as it reaches maximum directconnect device limit - %d!!!\n", XRX750_DCMODE1_MAX_DEV_NUM);
			goto err_out;
		}
		g_dcmode1_dev_shinfo[dev_sh_idx].alt_port_id = port_id;
		g_dcmode1_dev_shinfo[dev_sh_idx].ref_count++;
	} else {
		for (dev_sh_idx = 0; dev_sh_idx < XRX750_DCMODE1_MAX_DEV_NUM; dev_sh_idx++) {
			if (g_dcmode1_dev_shinfo[dev_sh_idx].status != XRX750_DCMODE1_DEV_STATUS_USED) {
				break;
			}
		}

		if (dev_sh_idx >= XRX750_DCMODE1_MAX_DEV_NUM) {
			DC_DP_ERROR("failed to register dev as it reaches maximum directconnect device limit - %d!!!\n", XRX750_DCMODE1_MAX_DEV_NUM);
			goto err_out;
		}
		memset(&g_dcmode1_dev_shinfo[dev_sh_idx], 0, sizeof(struct xrx750_dcmode1_dev_shared_info));
		g_dcmode1_dev_shinfo[dev_sh_idx].start_port_id = port_id;
		g_dcmode1_dev_shinfo[dev_sh_idx].ref_count = 1;
	}

	/* Reset DC Mode1 device structure */
	memset(dev_ctx, 0, sizeof(struct xrx750_dcmode1_dev_info));
	dev_ctx->shared_info = &g_dcmode1_dev_shinfo[dev_sh_idx];

	/* Skip, iff sub-sequent multiport device */
	if ( !is_multiport_sub(alloc_flags) ) {
		/* map port-id to cppp client id and satt index */
		pid = dp_port_id_to_pid(port_id);
		dev_ctx->shared_info->cid = DC_DP_PID_TO_CID(pid);
		dev_ctx->shared_info->satt_idx = DC_DP_PID_TO_SATT_IDX(pid);

	/* allocate resources */
#ifdef DC_DP_DEBUG_RESOURCES
		dc_dp_dump_resources("PRE-REGISTRATION:", resources);
#endif

		if (dc_dp_validate_resources(resources, dev_ctx)) {
			DC_DP_ERROR("dc_dp_validate_resources failed\n");
			goto err_out;
		}

		if (dc_dp_prepare_resources(resources, dev_ctx, port_id)) {
			DC_DP_ERROR("dc_dp_prepare_resources failed\n");
			goto err_out;
		}

#ifdef DC_DP_DEBUG_RESOURCES
		dc_dp_dump_resources("POST-REGISTRATION:", resources);
#endif

		/* Datapath Library callback registration */
		dp_cb.rx_fn = xrx750_dcmode1_rx_cb;
		dp_cb.stop_fn = datapathcb->stop_fn;
		dp_cb.restart_fn = datapathcb->restart_fn;
		dp_cb.get_subifid_fn = dc_dp_get_netif_subifid;
		dp_cb.reset_mib_fn = datapathcb->reset_mib_fn;
		dp_cb.get_mib_fn = datapathcb->get_mib_fn;
		dp_cb.set_pkt_psi_fn = dc_dp_set_pkt_psi;
		/* Overwrites cb.get_subifid_fn, iff multiport device */
		if ( is_multiport(alloc_flags) )
			dp_cb.get_subifid_fn = xrx750_dcmode1_get_netif_subifid;

		ret = dp_register_dev(owner, port_id, &dp_cb, 0);
		if (ret != DP_SUCCESS) {
			DC_DP_ERROR("failed to register dev to Datapath Library/Core!!!\n");
			return DP_FAILURE;
		}
	}

	devspec->dc_accel_used = DC_DP_ACCEL_FULL_OFFLOAD;
	devspec->dc_tx_ring_used = DC_DP_RING_HW_MODE1;
	devspec->dc_rx_ring_used = DC_DP_RING_HW_MODE1;

	/* Register device to DC common layer */
	ret = dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, 0);
	if (ret) {
		DC_DP_ERROR("failed to register device to DC common layer!!!\n");
		goto err_out;
	}

	/* Keep all the resources */
	dev_ctx->status = XRX750_DCMODE1_DEV_STATUS_USED;
	dev_ctx->alloc_flags = alloc_flags;
	if ( !is_multiport_sub(alloc_flags) ) {
		dev_ctx->shared_info->status = XRX750_DCMODE1_DEV_STATUS_USED;
		dev_ctx->shared_info->num_bufpools = resources->num_bufpools;
		dev_ctx->shared_info->buflist = resources->buflist;
		dev_ctx->shared_info->tx_num_bufpools = resources->tx_num_bufpools;
		dev_ctx->shared_info->tx_buflist = resources->tx_buflist;
		memcpy(&dev_ctx->shared_info->dccntr, resources->dccntr,
			sizeof(struct dc_dp_dccntr));
		memcpy(&dev_ctx->shared_info->rings, &resources->rings,
			sizeof(struct dc_dp_ring_res));
	}

	DC_DP_DBG("Success, returned %d.\n", ret);
	return ret;
err_out:
	return DC_DP_FAILURE;
}

int32_t
xrx750_dcmode1_register_subif(void *ctx, struct module *owner,
			 struct net_device *dev, const uint8_t *subif_name, dp_subif_t *subif_id,
			 uint32_t flags)
{
	int32_t ret = DC_DP_FAILURE;
	struct dp_subif subif = {0};
	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;

	DC_DP_DBG("dev_ctx=%p, owner=%p, dev=%p, subif_id=%p, flags=0x%08X\n",
		  ctx, owner, dev, subif_id, flags);

	if (!dev_ctx || (dev_ctx && !dev_ctx->shared_info) ||
	    !subif_id) {
		DC_DP_ERROR("failed to register/de-register subif!!!\n");
		goto err_out;
	}

	subif = *subif_id;

	/* De-register */
	if (flags & DC_DP_F_DEREGISTER) {
		/* Multiport device? */
		if ( is_multiport(dev_ctx->alloc_flags) )
			multiport_wa_forward_map_subifid(dev_ctx, &subif);

		/* De-register subif from Datapath Library/Core */
		ret = dp_register_subif(owner, dev, (char *)subif_name, &subif, DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			DC_DP_ERROR("failed to de-register subif from Datapath Library/Core!!!\n");
			goto err_out;
		}

		/* Multiport device? */
		if ( is_multiport(dev_ctx->alloc_flags) ) {
			dev_ctx->shared_info->subif_info[XRX750_DCMODE1_GET_SUBIFIDX(subif.subif)].netif = NULL;
			dev_ctx->shared_info->num_subif--;
		}

		DC_DP_DBG("Success, returned %d.\n", DC_DP_SUCCESS);
		return DC_DP_SUCCESS;
	}

	/* Multiport device? */
	if ( is_multiport(dev_ctx->alloc_flags) ) {
		/* Single port should not have more than 16 Subif */
		if ( XRX750_DCMODE1_GET_SUBIFIDX(subif_id->subif) >= MULTIPORT_WORKAROUND_MAX_SUBIF_NUM ) {
			DC_DP_ERROR("failed to register subif, subif_id_num=0x%04x reaches maximum limit 16!!!\n", subif_id->subif);
			goto err_out;
		}

		multiport_wa_forward_map_subifid(dev_ctx, &subif);
	}

	/* Register subif to Datapath Library/Core */
	ret = dp_register_subif(owner, dev, (char *)subif_name, &subif, 0);
	if (ret != DP_SUCCESS) {
		DC_DP_ERROR("failed to register subif to Datapath Library/Core!!!\n");
		return DP_FAILURE;
	}

	/* Multiport device? */
	if ( is_multiport(dev_ctx->alloc_flags) ) {
		dev_ctx->shared_info->subif_info[XRX750_DCMODE1_GET_SUBIFIDX(subif.subif)].netif = dev;
		dev_ctx->shared_info->num_subif++;
	}

	/* Multiport 1st device? */
	if ( is_multiport_main(dev_ctx->alloc_flags) )
		subif_id->subif = (subif.subif & ~MULTIPORT_WORKAROUND_SUBIFID_MASK);
	else
		subif_id->subif = subif.subif;

	ret = DC_DP_SUCCESS;
	DC_DP_DBG("Success, returned %d.\n", ret);
	return ret;

err_out:
	DC_DP_DBG("Failure, returned %d.\n", ret);
	return ret;
}

int32_t
xrx750_dcmode1_xmit(void *ctx, struct net_device *rx_if, dp_subif_t *rx_subif,
		dp_subif_t *tx_subif, struct sk_buff *skb, int32_t len, uint32_t flags)
{
	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;
	dp_subif_t subif = {0};

	if (!tx_subif || !dev_ctx ||
	    (dev_ctx && !dev_ctx->shared_info)) {
		DC_DP_ERROR("failed to transmit!!!\n");
		goto drop;
	}

	subif = *tx_subif;

	/* Multiport device? */
	if ( is_multiport(dev_ctx->alloc_flags) )
		multiport_wa_forward_map_subifid(dev_ctx, &subif);

	/* Send it to Datapath library for transmit */
	return dp_xmit(skb->dev, &subif, skb, len, flags);

drop:
	if (skb)
		dev_kfree_skb_any(skb);
	return DC_DP_FAILURE;
}

int32_t
xrx750_dcmode1_get_netif_stats(void *ctx, struct net_device *netif,
			  dp_subif_t *subif_id,
			  struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
	int ret;
	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;
	dp_subif_t subif = {0};

	if (!subif_id || !dev_ctx ||
	    (dev_ctx && !dev_ctx->shared_info)) {
		DC_DP_ERROR("failed to get stats!!!\n");
		ret = DC_DP_FAILURE;
		goto out;
	}

	subif = *subif_id;

	/* Multiport device? */
	if ( is_multiport(dev_ctx->alloc_flags) )
		multiport_wa_forward_map_subifid(dev_ctx, &subif);

	ret = dp_get_netif_stats(netif, &subif, if_stats, DP_STATS_ALL);

out:
	return ret ? DC_DP_FAILURE : DC_DP_SUCCESS;
}

int32_t
xrx750_dcmode1_clear_netif_stats(void *ctx, struct net_device *netif,
			    dp_subif_t *subif_id, uint32_t flags)
{
	int ret;
	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;
	dp_subif_t subif = {0};

	if (!subif_id || !dev_ctx ||
	    (dev_ctx && !dev_ctx->shared_info)) {
		DC_DP_ERROR("failed to clear stats!!!\n");
		ret = DC_DP_FAILURE;
		goto out;
	}

	subif = *subif_id;

	/* Multiport device? */
	if ( is_multiport(dev_ctx->alloc_flags) )
		multiport_wa_forward_map_subifid(dev_ctx, &subif);

	ret = dp_clear_netif_stats(netif, &subif, DP_STATS_ALL);

out:
	return ret ? DC_DP_FAILURE : DC_DP_SUCCESS;
}

static int32_t
xrx750_dcmode1_get_property(void *ctx,
                            struct net_device *netif, struct dp_subif *subif_id,
                            struct dc_dp_prop *property)
{
	int32_t ret = DC_DP_FAILURE;
	uint32_t pos;
	unsigned long prop_id_bitmap;
	pp_wifi_prop_t pp_wifi_prop = {0};
	pp_wifi_prop.dev = netif;

	prop_id_bitmap = property->prop_id;
	for_each_set_bit(pos, &prop_id_bitmap, SZ_32) {
		switch (1 << pos) {
		case DC_DP_PROP_ID_DGAF:
			pp_wifi_prop.prop_id = PP_WIFI_PROP_ID_DGAF;
			avalanche_pp_get_dgaf_mode(&pp_wifi_prop);
			property->dgaf_disabled = pp_wifi_prop.prop_val;
			ret = DC_DP_SUCCESS;
			break;
		default:
			pr_debug("[%s] Unsupported property id=0x%x for dev=%s\n",
				__func__, (1 << pos), (netif ? netif->name : "NA"));
			break;
		}
	}

	return ret;
}

static int32_t
xrx750_dcmode1_set_property(void *ctx,
                            struct net_device *netif, struct dp_subif *subif_id,
                            struct dc_dp_prop *property)
{
	int32_t ret = DC_DP_FAILURE;
	uint32_t pos;
	unsigned long prop_id_bitmap;
	pp_wifi_prop_t pp_wifi_prop = {0};
	pp_wifi_prop.dev = netif;

	prop_id_bitmap = property->prop_id;
	for_each_set_bit(pos, &prop_id_bitmap, SZ_32) {
		switch (1 << pos) {
		case DC_DP_PROP_ID_DGAF:
			pp_wifi_prop.prop_id = PP_WIFI_PROP_ID_DGAF;
			pp_wifi_prop.prop_val = property->dgaf_disabled;
			ti_hil_pp_event(PP_UPDATE_WIFI_DEV_PROP, (void*)&pp_wifi_prop);
			ret = DC_DP_SUCCESS;
			break;
		default:
			pr_debug("[%s] Unsupported property id=0x%x for dev=%s\n",
				__func__, (1 << pos), (netif ? netif->name : "NA"));
			break;
		}
	}

	return ret;
}

#if 0
int32_t
xrx750_dcmode1_register_qos_class2prio_cb(void *ctx,
			int32_t port_id, struct net_device *netif,
			int (*cb)(int32_t port_id, struct net_device *netif, uint8_t class2prio[DC_DP_MAX_SOC_CLASS]),
			int32_t flags)
{
	int32_t ret;
	int32_t i;
	uint8_t class2prio[DC_DP_MAX_SOC_CLASS] = {0};

	/* FIXME : Ideally PPA registration should be done for GRX750 as well (from common layer???). Otherwise, .register_qos_class2prio_cb may be used. */
	/* Set default class2prio mapping, in case no mapping is registered */
	for (i = 0; i < DC_DP_MAX_SOC_CLASS; i++)
		class2prio[i] = (i >> 1);

	ret = cb(port_id, netif, class2prio);

	return ret;
}
#endif

int32_t
xrx750_dcmode1_register_qos_class2prio_cb(void *ctx,
			int32_t port_id, struct net_device *netif,
			int (*cb)(int32_t port_id, struct net_device *netif, uint8_t class2prio[DC_DP_MAX_SOC_CLASS]),
			int32_t flags)
{
	int32_t ret = DC_DP_FAILURE;
	int32_t i;
	uint8_t class2prio[DC_DP_MAX_SOC_CLASS] = {0};

	xrx750_dcmode1_dev_info_t *dev_ctx = (xrx750_dcmode1_dev_info_t *)ctx;

	if (!dev_ctx) {
		DC_DP_ERROR("failed to register qos_class2prio callback!!!\n");
		goto out;
	}

#if IS_ENABLED(CONFIG_LTQ_PPA_API) && defined(CONFIG_WMM_PPA_PUMA)
	/* Skip, iff multiport 2nd device? */
	if ( is_multiport_sub(dev_ctx->alloc_flags) ) {
		ret = DC_DP_SUCCESS;
		goto out;
	}

	if (ppa_register_qos_class2prio_hook_fn)
		ret = ppa_register_qos_class2prio_hook_fn(port_id, netif, cb, flags);
#else
	/* TODO: On Puma7 pass the callback to hil driver so it'll call it
	 * upon defining mapping in the QoS service. For now provide
	 * default class2prio mapping, since no mapping is registered */
	for (i = 0; i < DC_DP_MAX_SOC_CLASS; i++) {
        if(i <= 7) {
            class2prio[i] = i;
        } else {
            class2prio[i] = 7;
        }
    }
	ret = cb(port_id, netif, class2prio);
#endif

out:
	return ret;
}

static void
xrx750_dcmode1_get_subif_param(void *ctx, struct dc_dp_subif *dc_subif)
{
	uint8_t mcf;

	if (!dc_subif)
		return;

	dc_subif->vap_id = XRX750_DCMODE1_GET_BITS(dc_subif->subif, 8, 5);
	mcf = XRX750_DCMODE1_GET_BITS(dc_subif->subif, 13, 1);
	if (mcf) {
		dc_subif->mcf = mcf;
		dc_subif->mc_index = XRX750_DCMODE1_GET_BITS(dc_subif->subif, 0, 8);
	} else
		dc_subif->sta_id = XRX750_DCMODE1_GET_BITS(dc_subif->subif, 0, 8);
}

static void
xrx750_dcmode1_set_subif_param(void *ctx, struct dc_dp_subif *dc_subif)
{
	if (!dc_subif)
		return;

	XRX750_DCMODE1_SET_BITS(dc_subif->subif, 8, 5, dc_subif->vap_id);
	if (dc_subif->mcf) {
		XRX750_DCMODE1_SET_BITS(dc_subif->subif, 13, 1, dc_subif->mcf);
		XRX750_DCMODE1_SET_BITS(dc_subif->subif, 0, 8, dc_subif->mc_index);
	} else
		XRX750_DCMODE1_SET_BITS(dc_subif->subif, 0, 8, dc_subif->sta_id);
}

void
xrx750_dcmode1_dump_proc(void *ctx, struct seq_file *seq)
{

	xrx750_dcmode1_dev_info_t *plat = (xrx750_dcmode1_dev_info_t *)ctx;
	struct dc_dp_dccntr *cnt;
	struct dc_dp_ring_res *ring;

	if (!plat || (plat && !plat->shared_info))
		return;

	cnt = &plat->shared_info->dccntr;
	ring = &plat->shared_info->rings;

	seq_printf(seq, "\n");
	seq_printf(seq, "    cppp id:     %d\n", plat->shared_info->cid);
	seq_printf(seq, "    satt_idx:    %d\n", plat->shared_info->satt_idx);
	seq_printf(seq, "    cppp handle: 0x%x\n", (uint32_t)plat->shared_info->handle);

	seq_printf(seq, "    \n");
	seq_printf(seq, "    Ring resources:\n");
	seq_printf(seq, "    SoC2Dev: TX_IN\n");
	if (ring->soc2dev.base == NULL)
	{
		seq_printf(seq, "    Base adress of Soc2Dev  not allocated\n");
	}
	else{
		seq_printf(seq, "    base address :[0x%pK] |",ring->soc2dev.base);
	}
	if (ring->soc2dev.phys_base == NULL)
	{
		seq_printf(seq, "    \nPhysical Base adress of Soc2Dev  not allocated \n");
	}
	else{
		seq_printf(seq, " physical base adress:[0x%pk] |",ring->soc2dev.phys_base);
	}
	seq_printf(seq, " Ring Size :[%d] \n",ring->soc2dev.size);

	seq_printf(seq, "    SoC2Dev_ret: TX_OUT\n");
	seq_printf(seq, "    base address:[0x%pk] | physical base address:[0x%pK] | Ring Size:[%d]\n",
				ring->soc2dev_ret.base,ring->soc2dev_ret.phys_base,ring->soc2dev_ret.size);
	seq_printf(seq, "    Dev2SoC: RX_OUT  \n");
	seq_printf(seq, "    base address:[0x%pk] | physical base address:[0x%pK] | Ring Size:[%d]\n",
		ring->dev2soc.base,ring->dev2soc.phys_base,ring->dev2soc.size);
	seq_printf(seq, "    Dev2SoC_ret: RX_IN \n");
	seq_printf(seq, "    base address:[0x%pk] | physical base address:[0x%pK] | Ring Size:[%d]\n",
		ring->dev2soc_ret.base,ring->dev2soc_ret.phys_base,ring->dev2soc_ret.size);

	seq_printf(seq, "    \n");
	seq_printf(seq, "    dev counters address:\n");
	seq_printf(seq, "    recv_head | soc2dev_enq    : p:[0x%pK] v:[0x%pK], satt:[0x%x]\n",
		cnt->soc2dev_enq_phys_base, cnt->soc2dev_enq_base,
		plat->shared_info->soc2dev_enq_satt);
	seq_printf(seq, "    free_tail | soc2dev_ret_deq: p:[0x%pK] v:[0x%pK], satt:[0x%x]\n",
		cnt->soc2dev_ret_deq_phys_base, cnt->soc2dev_ret_deq_base,
		plat->shared_info->soc2dev_ret_deq_satt);
	seq_printf(seq, "    send_tail | dev2soc_deq    : p:[0x%pK] v:[0x%pK], satt:[0x%x]\n",
		cnt->dev2soc_deq_phys_base, cnt->dev2soc_deq_base,
		plat->shared_info->dev2soc_deq_satt);
	seq_printf(seq, "    alloc_head| dev2soc_ret_enq: p:[0x%pK] v:[0x%pK], satt:[0x%x]\n",
		cnt->dev2soc_ret_enq_phys_base, cnt->dev2soc_ret_enq_base,
		plat->shared_info->dev2soc_ret_enq_satt);
	seq_printf(seq, "    \n");
	seq_printf(seq, "    soc counters address:\n");
	seq_printf(seq, "    recv_tail | soc2dev_deq    : p:[0x%pK] v:[0x%pK]\n",
		cnt->soc2dev_deq_phys_base, cnt->soc2dev_deq_base);
	seq_printf(seq, "    free_head | soc2dev_ret_enq: p:[0x%pK] v:[0x%pK]\n",
		cnt->soc2dev_ret_enq_phys_base, cnt->soc2dev_ret_enq_base);
	seq_printf(seq, "    send_head | dev2soc_enq    : p:[0x%pK] v:[0x%pK]\n",
		cnt->dev2soc_enq_phys_base, cnt->dev2soc_enq_base);
	seq_printf(seq, "    alloc_tail| dev2soc_ret_deq: p:[0x%pK] v:[0x%pK]\n",
		cnt->dev2soc_ret_deq_phys_base, cnt->dev2soc_ret_deq_base);

	return;
}

int32_t
xrx750_dcmode1_add_session_shortcut_forward(void *ctx, struct dp_subif *subif, struct sk_buff *skb, uint32_t flags)
{
	switch (flags) {
	case DC_DP_F_PREFORWARDING:
		skb_reset_mac_header(skb);
		hil_ingress_hook(skb);
		break;
	case DC_DP_F_POSTFORWARDING:
		hil_egress_hook(skb);
		break;
	default:
		DC_DP_ERROR("unknown flag\n");
	}
	return DC_DP_SUCCESS;
}

int32_t
xrx750_dcmode1_disconn_if(void *ctx, struct net_device *netif, dp_subif_t *subif_id,
				 uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags)
{
	if (mac_addr)
		avalanche_pp_flush_sessions_per_mac(mac_addr, netif);

	return DC_DP_SUCCESS;
}

static struct dc_dp_dcmode_ops xrx750_dcmode1_ops = {
	.get_peripheral_config = xrx750_dcmode1_get_peripheral_config,
	.get_host_capability = xrx750_dcmode1_get_host_capability,
	.register_dev = NULL,
	.register_dev_ex = xrx750_dcmode1_register_dev_ex,
	.register_subif = xrx750_dcmode1_register_subif,
	.xmit = xrx750_dcmode1_xmit,
	.handle_ring_sw = NULL,
	.add_session_shortcut_forward = xrx750_dcmode1_add_session_shortcut_forward,
	.disconn_if = xrx750_dcmode1_disconn_if,
	.get_netif_stats = xrx750_dcmode1_get_netif_stats,
	.clear_netif_stats = xrx750_dcmode1_clear_netif_stats,
	.get_property = xrx750_dcmode1_get_property,
	.set_property = xrx750_dcmode1_set_property,
	.register_qos_class2prio_cb = xrx750_dcmode1_register_qos_class2prio_cb,
	.map_class2devqos = NULL,
	.alloc_skb = NULL,
	.free_skb = NULL,
	.change_dev_status = NULL,
	.get_wol_cfg = NULL,
	.set_wol_cfg = NULL,
	.get_wol_ctrl_status = NULL,
	.set_wol_ctrl = NULL,
	.get_subif_param = xrx750_dcmode1_get_subif_param,
	.set_subif_param = xrx750_dcmode1_set_subif_param,
	.dump_proc = xrx750_dcmode1_dump_proc
};

static struct dc_dp_dcmode xrx750_dcmode1 = {
	.dcmode_cap = DC_DP_F_DCMODE_HW | DC_DP_F_DCMODE_1,
	.dcmode_ops = &xrx750_dcmode1_ops
};

void xrx750_dcmode1_init(void)
{

	/* Register DCMODE1 */
	dc_dp_register_dcmode(&xrx750_dcmode1, 0);

	DC_DP_DBG("dc_dp Puma initialized\n");

}

void xrx750_dcmode1_exit(void)
{

	/* De-register DCMODE1 */
	dc_dp_register_dcmode(&xrx750_dcmode1, DC_DP_F_DCMODE_DEREGISTER);

}

#if 0
module_init(xrx750_dcmode1_init_module);
module_exit(xrx750_dcmode1_exit_module);

MODULE_AUTHOR("Weiss Yair");
MODULE_DESCRIPTION("DC Mode1 support for XRX750");
MODULE_LICENSE("GPL");
#endif

