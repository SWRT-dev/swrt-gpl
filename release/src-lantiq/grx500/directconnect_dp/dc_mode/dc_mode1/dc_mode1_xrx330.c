/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
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
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include <linux/dma-mapping.h>
#include <net/datapath_api.h>


/*Version:
 * 1.0.3:
 *  - single line ATM/PTM.
 * 	- QoS
 * 1.1.0:
 *  - Bonding PTM.
 * */
#define DCMODE1_XRX330_VERSION "1.1.0"
/* acceleration hooks */

/* defines */
#define XRX330_DCMODE1_MAX_DEV_NUM       2

#define DC_DP_MAX_RING_SIZE		4096
#define DC_DP_EXPECTED_NUM_DCCNTR	1
#define DC_DP_EXPECTED_DCCNTR_LEN	4
#define DC_DP_SOCW_DCCNTR_MODE \
	(DC_DP_F_DCCNTR_MODE_INCREMENTAL | DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN)

#define DC_DP_DEVW_DCCNTR_MODE \
	(DC_DP_F_DCCNTR_MODE_CUMULATIVE | DC_DP_F_DCCNTR_MODE_LITTLE_ENDIAN)

#define PP_BUFFER_HEADROOM		128
#define DC_DP_TOTAL_BUF_SIZE		2048
#define DC_DP_REAL_BUF_SIZE \
	(DC_DP_TOTAL_BUF_SIZE - PP_BUFFER_HEADROOM)


#define DC_DP_DESC_NUM_DWORDS 4

#define DC_DP_DESC_MASK_DWORD0	0xFFFF		/* bits 0 - 15 */
#define DC_DP_DESC_MASK_DWORD1	0		/* none */
#define DC_DP_DESC_MASK_DWORD2	0		/* none */
#define DC_DP_DESC_MASK_DWORD3	0xF		/* bits 24 - 27 */

#define DC_DP_DESC_SHIFT_DWORD0	0
#define DC_DP_DESC_SHIFT_DWORD1	0
#define DC_DP_DESC_SHIFT_DWORD2	0
#define DC_DP_DESC_SHIFT_DWORD3	24

#define DP_STATS_CPU    0x00000001
#define DP_STATS_ACCEL  0x00000002
#define DP_STATS_ALL    (DP_STATS_CPU | DP_STATS_ACCEL)
/* hard coded - should be used only for debug! */

#define min4(a, b, c, d) min(a, min(b, min(c, d)))
#define max4(a, b, c, d) max(a, max(b, max(c, d)))
#define abs_offset(a, b) max(a, b) - min(a, b)

typedef struct xrx330_dcmode1_dev_info {
#define XRX330_DCMODE1_DEV_STATUS_USED    0x1
	int32_t status;
	int32_t port_id; /* datapath port id */
	int32_t satt_idx; /* cppp client-id */
	void * handle; /* cppp handle */

	/* resources */
	struct dc_dp_ring_res rings;
	int32_t num_bufpools;
	struct dc_dp_buf_pool *buflist;
	int32_t tx_num_bufpools;
	struct dc_dp_buf_pool *tx_buflist;
	struct dc_dp_dccntr dccntr;
} xrx330_dcmode1_dev_info_t;
static struct xrx330_dcmode1_dev_info g_dcmode1_dev[XRX330_DCMODE1_MAX_DEV_NUM];

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
	DC_DP_DBG("tx_buflist      0x%pK\n", res->tx_buflist);
	DC_DP_DBG("num_dccntr   %d\n", res->num_dccntr);
	DC_DP_DBG("dev_priv_res 0x%pK\n\n", res->dev_priv_res);

	dc_dp_dump_ring(&res->rings.soc2dev, "soc2dev");
	dc_dp_dump_ring(&res->rings.soc2dev_ret, "soc2dev_ret");
	dc_dp_dump_ring(&res->rings.dev2soc, "dev2soc");
	dc_dp_dump_ring(&res->rings.dev2soc_ret, "dev2soc_ret");

	dc_dp_dump_dccntr(res->dccntr);
}

static int32_t dc_dp_validate_resources(struct dc_dp_res *res)
{
	struct dc_dp_dccntr *cnt;

	if (!res->rings.dev2soc.size ||
	    !res->rings.dev2soc_ret.size ||
	    !res->rings.soc2dev.size||
	    !res->rings.soc2dev_ret.size) {
		DC_DP_ERROR("ERROR: did not get all ring sizes %d %d %d %d\n",
				res->rings.dev2soc.size,
				res->rings.dev2soc_ret.size,
				res->rings.soc2dev.size,
				res->rings.soc2dev_ret.size);
		return DC_DP_FAILURE;
	}
	/* dc_dp_dump_resources("Valid resource", res); */
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

	if (res->num_bufs_req != res->rings.dev2soc_ret.size - 1) {
		DC_DP_ERROR("WARNING: res->num_bufs_req != res->rings.soc2dev.size, aligning to ring size\n");
		res->num_bufpools = res->rings.dev2soc_ret.size - 1;
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
		DC_DP_ERROR("ERROR: unexpected dccnte_mode %d#%d\n",
			res->dccntr->soc_write_dccntr_mode, DC_DP_SOCW_DCCNTR_MODE);
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

extern int32_t dp_create_dcdp_res(uint32_t port_id, struct dc_dp_res *dcdp_res);
extern void dc_dp_free_dma(unsigned int dataptr);
extern struct sk_buff * dc_dp_alloc_dma(int len);
static int32_t dc_dp_prepare_resources(struct dc_dp_res *res,
				   xrx330_dcmode1_dev_info_t *plat)
{
	struct dc_dp_dccntr *cnt = res->dccntr;

	/* dc_dp_dump_resources("Before prepare resource", res); */
	dp_create_dcdp_res(plat->port_id, res);
	/* ring sizes */

	/* dev counters */

	/* Update returned 'resource' structure */
	//res->rings.dev2soc.base = phys_to_virt(0);
	//res->rings.dev2soc.phys_base = (void *)0;

	//res->rings.soc2dev_ret.base = phys_to_virt(0);
	//res->rings.soc2dev_ret.phys_base = (void *)0;

	//res->rings.soc2dev.base = phys_to_virt(0);
	//res->rings.soc2dev.phys_base = (void *)0;

	//res->rings.dev2soc_ret.base = phys_to_virt(0);
	//res->rings.dev2soc_ret.phys_base = (void *)0;

	/* dev counters */
	//cnt->soc2dev_deq_base = 0;
	//cnt->soc2dev_deq_phys_base = (void *)0;
	cnt->soc2dev_deq_dccntr_len = 4;

	//cnt->soc2dev_ret_enq_base = 0;
	//cnt->soc2dev_ret_enq_phys_base = (void *)0;
	cnt->soc2dev_ret_enq_dccntr_len = 4;

	//cnt->dev2soc_enq_base = 0;
	//cnt->dev2soc_enq_phys_base = (void *)0;
	cnt->dev2soc_enq_dccntr_len = 4;

	//cnt->dev2soc_ret_deq_base = 0;
	//cnt->dev2soc_ret_deq_phys_base = (void *)0;
	cnt->dev2soc_ret_deq_dccntr_len = 4;

	/* FW counter mode */
	cnt->dev_write_dccntr_mode = DC_DP_DEVW_DCCNTR_MODE;

	/* save satt entries for debug;*/

	/* dc_dp_dump_resources("After preparing resource", res); */
	return DC_DP_SUCCESS;
}

static int32_t
xrx330_dcmode1_rx_cb(struct net_device *rxif, struct net_device *txif,
			struct sk_buff *skb, int32_t len)
{
	struct dp_subif tx_subif = {0};
	/* DC_DP_DBG("start\n");
	if (rxif)
		DC_DP_DBG("rxif: %s\n", rxif->name);
	if (txif)
		DC_DP_DBG("txif: %s\n", txif->name); */
	tx_subif.subif = skb->DW0 & 0x7F;
	tx_subif.port_id = (skb->DW1 >> 8) & 0xF;
	return dc_dp_rx(rxif, txif, &tx_subif, skb, skb->len, 0);;
}


int32_t
dc_dp_set_pkt_psi(struct net_device *netif, struct sk_buff *skb,
		    struct dp_subif *subif, uint32_t flags)
{
	return DC_DP_SUCCESS;
}

static int32_t find_dev_idx_for_port(uint32_t port_id, uint32_t flags)
{
	int32_t dev_idx;
	for (dev_idx = 0; dev_idx < XRX330_DCMODE1_MAX_DEV_NUM; dev_idx++) {
		/* handle multiple device registration, take current index */
		if (DC_DP_F_SHARED_RES & flags) {
			if (port_id == g_dcmode1_dev[dev_idx].port_id)
				break;
		} else
			if (XRX330_DCMODE1_DEV_STATUS_USED !=
				g_dcmode1_dev[dev_idx].status)
				break;
	}
	return dev_idx;
}
int32_t
xrx330_dcmode1_register_dev(void *ctx, struct module *owner, uint32_t port_id,
		       struct net_device *dev, struct dc_dp_cb *datapathcb,
		       struct dc_dp_res *resources, struct dc_dp_dev *devspec,
		       uint32_t flags)
{
	int32_t ret;
	xrx330_dcmode1_dev_info_t *dev_ctx;
	int32_t dev_idx = 0;
	dp_cb_t dp_cb = {0};
	DC_DP_DBG("owner=%p, port_id=%u, dev=%p, datapathcb=%p, \
			  resources=%p, dev_spec=%p, flags=0x%08X\n",
			  owner, port_id, dev, datapathcb, resources, devspec, flags);

	/* De-register */
	if (flags & DC_DP_F_DEREGISTER) {
		dev_ctx = (xrx330_dcmode1_dev_info_t *)ctx;
		/* De-register device from DC Common layer */
		dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, DC_DP_DCMODE_DEV_DEREGISTER);

		/* De-register device from DP Lib */
		ret = dp_register_dev(owner, port_id, &dp_cb, DP_F_DEREGISTER);
		ret = 0;
		if (ret) {
			DC_DP_ERROR("failed to de-register device for the port_id %d!!!\n", port_id);
			return DC_DP_FAILURE;
		}

		memset(dev_ctx, 0, sizeof(xrx330_dcmode1_dev_info_t));

		DC_DP_DBG("Success, returned %d.\n", DC_DP_SUCCESS);
		return DC_DP_SUCCESS;
	}

	/* NOTE : here dev_ctx should be NULL */
	/* FIXME : any lock to protect private members across different call??? */
	dev_idx = find_dev_idx_for_port(port_id, flags);

	if (dev_idx >= XRX330_DCMODE1_MAX_DEV_NUM) {
		DC_DP_ERROR("failed to register dev as it reaches maximum directconnect device limit - %d!!!\n", XRX330_DCMODE1_MAX_DEV_NUM);
		goto err_out;
	}
	dev_ctx = &g_dcmode1_dev[dev_idx];
	/* Reset DC Mode1 device structure */
	memset(dev_ctx, 0, sizeof(struct xrx330_dcmode1_dev_info));

	dev_ctx->port_id = port_id;
	if (dc_dp_prepare_resources(resources, dev_ctx)) {
		DC_DP_ERROR("dc_dp_prepare_resources failed\n");
		goto err_out;
	}
	if (dc_dp_validate_resources(resources)) {
		DC_DP_ERROR("dc_dp_validate_resources failed\n");
		goto err_out;
	}
#ifdef DC_DP_DEBUG_RESOURCES
	dc_dp_dump_resources("POST-REGISTRATION:", resources);
#endif
	/* Datapath Library callback registration */
	dp_cb.rx_fn = xrx330_dcmode1_rx_cb;
	dp_cb.stop_fn = datapathcb->stop_fn;
	dp_cb.restart_fn = datapathcb->restart_fn;
	dp_cb.get_subifid_fn = dc_dp_get_netif_subifid;
	dp_cb.reset_mib_fn = datapathcb->reset_mib_fn;
	dp_cb.get_mib_fn = datapathcb->get_mib_fn;
	//dp_cb.set_pkt_psi_fn = dc_dp_set_pkt_psi;
	ret = dp_register_dev(owner, port_id, &dp_cb, 0);
	if (ret != DP_SUCCESS) {
		DC_DP_ERROR("failed to register dev to Datapath Library/Core!!!\n");
		return DP_FAILURE;
	}

	/* Register DC Mode1 device to DC common layer */
	ret = dc_dp_register_dcmode_device(owner, port_id, dev, dev_ctx, 0);
	if (ret) {
		DC_DP_ERROR("failed to register device to DC common layer!!!\n");
		goto err_out_dereg_dev;
	}

	/* Keep all the resources */
	dev_ctx->status = XRX330_DCMODE1_DEV_STATUS_USED;
	dev_ctx->num_bufpools = resources->num_bufpools;
	dev_ctx->buflist = resources->buflist;
	dev_ctx->tx_num_bufpools = resources->tx_num_bufpools;
	dev_ctx->tx_buflist = resources->tx_buflist;
	memcpy(&dev_ctx->dccntr, resources->dccntr,
		sizeof(struct dc_dp_dccntr));
	memcpy(&dev_ctx->rings, &resources->rings,
		sizeof(struct dc_dp_ring_res));
	return DC_DP_SUCCESS;

err_out_dereg_dev:
    dp_register_dev(owner, port_id, &dp_cb, DP_F_DEREGISTER);

err_out:
	return DC_DP_FAILURE;
}

int32_t
xrx330_dcmode1_register_subif(void *ctx, struct module *owner,
		 struct net_device *dev, const uint8_t *subif_name, dp_subif_t *subif_id,
		 uint32_t flags)
{
	int32_t ret = DC_DP_FAILURE;
	xrx330_dcmode1_dev_info_t *dev_ctx = (xrx330_dcmode1_dev_info_t *)ctx;

	DC_DP_DBG("dev_ctx=%p, owner=%p, dev=%p, subif_id=%p, flags=0x%08X\n",
		  dev_ctx, owner, dev, subif_id, flags);

	/* De-register */
	if (flags & DC_DP_F_DEREGISTER) {

		/* De-register subif from Datapath Library/Core */
		ret = dp_register_subif(owner, dev, (char *)subif_name, subif_id, DP_F_DEREGISTER);
		if (ret != DP_SUCCESS) {
			DC_DP_ERROR("failed to de-register subif from Datapath Library/Core!!!\n");
			goto err_out;
		}

		DC_DP_DBG("Success, returned %d.\n", DC_DP_SUCCESS);
		return DC_DP_SUCCESS;
	}

	/* Register subif to Datapath Library/Core */
	ret = dp_register_subif(owner, dev, (char *)subif_name, subif_id, 0);
	if (ret != DP_SUCCESS) {
		DC_DP_ERROR("failed to register subif to Datapath Library/Core!!!\n");
		return DP_FAILURE;
	}

	ret = DC_DP_SUCCESS;
	DC_DP_DBG("Success, returned %d.\n", ret);
	return ret;

err_out:
	DC_DP_DBG("Failure, returned %d.\n", ret);
	return ret;
}

int32_t
xrx330_dcmode1_xmit(void *ctx, struct net_device *rx_if, struct dp_subif *rx_subif,
		struct dp_subif *tx_subif, struct sk_buff *skb, int32_t len, uint32_t flags)
{
	/* Send it to Datapath library for transmit */
	//DC_DP_DBG("Flags %x.\n", flags);
	return dp_xmit(skb->dev, tx_subif, skb, len, flags);
}

int32_t
xrx330_dcmode1_get_netif_stats(void *ctx, struct net_device *netif,
			  struct dp_subif *subif_id,
			  struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
	int ret;

	ret = dp_get_netif_stats(netif, subif_id, if_stats, DP_STATS_ALL);

	return ret ? DC_DP_FAILURE : DC_DP_SUCCESS; 
}

int32_t
xrx330_dcmode1_clear_netif_stats(void *ctx, struct net_device *netif,
			    struct dp_subif *subif_id, uint32_t flags)
{
	int ret;

		ret = dp_clear_netif_stats(netif, subif_id, DP_STATS_ALL);

		return ret ? DC_DP_FAILURE : DC_DP_SUCCESS;
}

int32_t
xrx330_dcmode1_register_qos_class2prio_cb(void *ctx,
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

void
xrx330_dcmode1_dump_proc(void *ctx, struct seq_file *seq)
{
	xrx330_dcmode1_dev_info_t *plat = (xrx330_dcmode1_dev_info_t *)ctx;
	struct dc_dp_dccntr *cnt;
	struct dc_dp_ring_res *ring = &plat->rings;
	if (!plat) {
		DC_DP_ERROR("Invalid private context\n");
		return;
	}
	cnt = &(plat->dccntr);
	if (!cnt) {
		DC_DP_ERROR("Invalid DCCNTR\n");
		return;
	}

	seq_printf(seq, "\nCurrent DCMODE1 XRX330 version:%s\n", DCMODE1_XRX330_VERSION);
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
	seq_printf(seq, "    recv_head | soc2dev_enq    : p:[0x%pK] v:[0x%pK]\n",
		cnt->soc2dev_enq_phys_base, cnt->soc2dev_enq_base
		);
	seq_printf(seq, "    free_tail | soc2dev_ret_deq: p:[0x%pK] v:[0x%pK]\n",
		cnt->soc2dev_ret_deq_phys_base, cnt->soc2dev_ret_deq_base
		);
	seq_printf(seq, "    send_tail | dev2soc_deq    : p:[0x%pK] v:[0x%pK]\n",
		cnt->dev2soc_deq_phys_base, cnt->dev2soc_deq_base
		);
	seq_printf(seq, "    alloc_head| dev2soc_ret_enq: p:[0x%pK] v:[0x%pK]\n",
		cnt->dev2soc_ret_enq_phys_base, cnt->dev2soc_ret_enq_base
		);
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
xrx330_dcmode1_disconn_if(void *ctx, struct net_device *netif, struct dp_subif *subif_id,
				 uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags)
{
	return DC_DP_SUCCESS;
}

int32_t
xrx330_dcmode1_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
    int32_t ret = -1;

    if (cap) {
        cap->fastpath.support = 1;
        cap->fastpath.hw_dcmode = DC_DP_MODE_TYPE_1;

        cap->fastpath.hw_cmode.soc2dev_write = DC_DP_SOCW_DCCNTR_MODE;
        cap->fastpath.hw_cmode.dev2soc_write = DC_DP_DEVW_DCCNTR_MODE;
        printk("%s: all little %x %x\n", __func__,
        		 cap->fastpath.hw_cmode.soc2dev_write,
				 cap->fastpath.hw_cmode.dev2soc_write);

        cap->fastpath.hw_cap = DC_DP_F_HOST_CAP_SG;

        ret = 0;
    }

    return ret;
}
struct sk_buff * xrx330_dcmode1_alloc_skb(void *dev_ctx,
	uint32_t len, struct dp_subif *subif, uint32_t flags)
{
	struct sk_buff *skb;

	skb = dc_dp_alloc_dma(len);
	return skb;
}
int32_t xrx330_dcmode1_free_skb(void *dev_ctx,
	struct dp_subif *subif, struct sk_buff *skb)
{
	unsigned int *data_ptr;

	/* free wrapper skb head */
	data_ptr = (unsigned int *)skb->head;
	dev_kfree_skb(skb);
	/* get original skb dma */
	dc_dp_free_dma(*data_ptr);
	return DC_DP_SUCCESS;
}
static struct dc_dp_dcmode_ops xrx330_dcmode1_ops = {
    .get_host_capability = xrx330_dcmode1_get_host_capability,
    .register_dev = xrx330_dcmode1_register_dev,
    .register_subif = xrx330_dcmode1_register_subif,
    .xmit = xrx330_dcmode1_xmit,
    .handle_ring_sw = NULL,
    .add_session_shortcut_forward = NULL,
    .disconn_if = xrx330_dcmode1_disconn_if,
    .get_netif_stats = xrx330_dcmode1_get_netif_stats,
    .clear_netif_stats = xrx330_dcmode1_clear_netif_stats,
    .register_qos_class2prio_cb = xrx330_dcmode1_register_qos_class2prio_cb,
    .map_class2devqos = NULL,
    .alloc_skb = xrx330_dcmode1_alloc_skb,
    .free_skb = xrx330_dcmode1_free_skb,
    .change_dev_status = NULL,
    .get_wol_cfg = NULL,
    .set_wol_cfg = NULL,
    .get_wol_ctrl_status = NULL,
    .set_wol_ctrl = NULL,
    .dump_proc = xrx330_dcmode1_dump_proc
};

static struct dc_dp_dcmode xrx330_dcmode1 = {
    .dcmode_cap = DC_DP_F_DCMODE_HW | DC_DP_F_DCMODE_1,
    .dcmode_ops = &xrx330_dcmode1_ops
};

static __init int xrx330_dcmode1_init_module(void)
{

        /* Register DCMODE1 */
        dc_dp_register_dcmode(&xrx330_dcmode1, 0);

	DC_DP_DBG("dc_dp GRX330 initialized\n");

	return 0;
}

static __exit void xrx330_dcmode1_exit_module(void)
{
	/* De-register DCMODE1 */
	dc_dp_register_dcmode(&xrx330_dcmode1, DC_DP_F_DCMODE_DEREGISTER);
}

module_init(xrx330_dcmode1_init_module);
module_exit(xrx330_dcmode1_exit_module);

MODULE_AUTHOR("Ho Nghia Duc");
MODULE_DESCRIPTION("DC Mode1 support for XRX330");
MODULE_LICENSE("GPL");

