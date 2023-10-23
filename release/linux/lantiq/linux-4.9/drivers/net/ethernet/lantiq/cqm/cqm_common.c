#include "cqm_common.h"

#ifdef CONFIG_SOC_GRX500
#include "grx500/reg/cbm_ls.h"
#else
#include "prx300/reg/cbm_ls.h"
#endif

static void __iomem *cqm_ls_addr_base;

#define LS_BASE cqm_ls_addr_base
#define LS_TH_SET \
((DEFAULT_LS_QTHH << LS_CTRL_PORT0_QUEUE_THRSHLD_POS) \
& (LS_CTRL_PORT0_QUEUE_THRSHLD_MASK))
#define LS_OVFL_SET \
((DEFAULT_LS_OVFL_CNT << LS_CTRL_PORT0_CNT_THRSHLD_POS) \
& (LS_CTRL_PORT0_CNT_THRSHLD_MASK))

void buf_addr_adjust(unsigned int buf_base_addr,
		     unsigned int buf_size,
		     dma_addr_t buf_dma_addr,
		     unsigned int *adjusted_buf_base,
		     unsigned int *adjusted_buf_size,
		     dma_addr_t *adjusted_dma_addr,
		     unsigned int align)
{
	unsigned int base;
	unsigned int size;
	dma_addr_t dma_addr;

	pr_info("0x%x 0x%x 0x%x\n", buf_base_addr, buf_size, align);
	base = ALIGN(buf_base_addr, align);
	size = buf_base_addr + buf_size - base;
	dma_addr = ALIGN(buf_dma_addr, align);

	*adjusted_buf_base = base;
	*adjusted_buf_size = size;
	if (adjusted_dma_addr)
		*adjusted_dma_addr = dma_addr;
	pr_info("0x%x 0x%x\n", base, size);
}

int cbm_linearise_buf(struct sk_buff *skb, struct cbm_tx_data *data,
		      int buf_size, u32 new_buf)
{
	u32 tmp_buf;
	void *frag_addr;
	const skb_frag_t *frag;
	int i, len = 0, copy_len = 0;

	SKB_FRAG_ASSERT(skb);

	if (new_buf) {
		tmp_buf = (u32)new_buf;
		if (skb_is_nonlinear(skb))
			copy_len = skb_headlen(skb);
		else
			copy_len = skb->len;
		if (data && data->pmac) {
			memcpy((u8 *)tmp_buf, data->pmac, data->pmac_len);
			tmp_buf += data->pmac_len;
		}

		memcpy((u8 *)tmp_buf, skb->data, copy_len);
		tmp_buf += copy_len;
		/* If the packet has fragments, copy that also */
		for (i = 0; i < (skb_shinfo(skb)->nr_frags); i++) {
			frag = &skb_shinfo(skb)->frags[i];
			len = skb_frag_size(frag);
			frag_addr = skb_frag_address(frag);
			if (len <= (buf_size - copy_len)) {
				memcpy((u8 *)tmp_buf, (u8 *)frag_addr, len);
				tmp_buf += len;
				copy_len += len;
			} else {
				pr_err("%s:22:copied = %d\n",
				       __func__, copy_len);
				pr_err("remaining = %d and skb->head is %x\n",
				       len, (u32)(skb->head));
				return CBM_FAILURE;
			}
		}
	} else {
		pr_err("%s:33:Cannot alloc CBM Buffer !\n", __func__);
		return CBM_FAILURE;
	}
	return CBM_SUCCESS;
}

/*Load Spreader Initialization*/

static void init_cbm_ls_port(int idx, void __iomem *cqm_ls_addr_base)
{
	int lsport = CBM_LS_PORT(idx, ctrl);
#ifdef CONFIG_CBM_LS_ENABLE
	/*if ((1 << idx) & g_cpu_port_alloc) {*/
	if (cpu_online(idx)) {
	/*Enable spreading only for port 0*/
#else
	if (!idx) {
#endif
		cbm_w32((LS_BASE + lsport), 0xF
			| LS_TH_SET
			| LS_OVFL_SET);
	} else {
		cbm_w32((LS_BASE + lsport), 0xD
			| LS_TH_SET
			| LS_OVFL_SET);
	}
}

void cbm_add_ls_port(int idx, int flag, void __iomem *cqm_ls_addr_base)
{
	int lsport = CBM_LS_PORT(idx, ctrl);

	if (flag) {
		cbm_w32((LS_BASE + lsport), 0xF
			| LS_TH_SET
			| LS_OVFL_SET);
	} else {
		cbm_w32((LS_BASE + lsport), 0xD
			| LS_TH_SET
			| LS_OVFL_SET);
	}
}

void cbm_ls_spread_alg_set(u32 alg, void __iomem *cqm_ls_addr_base)
{
	cbm_assert(alg < SPREAD_MAX, "illegal cbm load spread alg");
	set_val((LS_BASE + LS_SPR_CTRL), alg, LS_SPR_CTRL_SPR_SEL_MASK,
		LS_SPR_CTRL_SPR_SEL_POS);
}

void cbm_ls_port_weight_set(u32 port_id, u32 weight,
			    void __iomem *cqm_ls_addr_base)
{
	u32 pos, mask;

	cbm_assert(port_id < LS_PORT_NUM, "illegal cbm ls port id");

	pos  = LS_SPR_CTRL_WP0_POS + 2 * port_id;
	mask = (LS_SPR_CTRL_WP0_MASK >> LS_SPR_CTRL_WP0_POS) << pos;
	set_val((LS_BASE + LS_SPR_CTRL), weight, mask, pos);
}

void cbm_ls_flowid_map_set(u32 col, u32 val,
			   void __iomem *cqm_ls_addr_base)
{
	cbm_w32((LS_BASE + LS_FLOWID_MAP_COL0 + (col * 4)), val);
}

u32 cbm_ls_flowid_map_get(u32 col, void __iomem *ls_base)
{
	return cbm_r32(LS_BASE + LS_FLOWID_MAP_COL0 + (col * 4));
}

u32 cbm_ls_port_weight_get(u32 port_id,
			   void __iomem *cqm_ls_addr_base)
{
	u32 pos, mask;

	cbm_assert(port_id < LS_PORT_NUM, "illegal cbm ls port id");

	pos  = LS_SPR_CTRL_WP0_POS + 2 * port_id;
	mask = (LS_SPR_CTRL_WP0_MASK >> LS_SPR_CTRL_WP0_POS) << pos;
	return get_val(cbm_r32(LS_BASE + LS_SPR_CTRL), mask, pos);
}

void ls_intr_ctrl(u32 val, void __iomem *cqm_ls_addr_base)
{
	cbm_w32(LS_BASE + IRNEN_LS, val);
}

void init_cbm_ls(void __iomem *cqm_ls_addr_base)
{
	int i;

	for (i = 0; i < LS_PORT_NUM; i++)
		init_cbm_ls_port(i, LS_BASE);

	cbm_ls_spread_alg_set(SPREAD_WRR, LS_BASE);
	for (i = 0; i < LS_PORT_NUM; i++)
		cbm_ls_port_weight_set(i, DEFAULT_LS_PORT_WEIGHT, LS_BASE);

	cbm_w32((LS_BASE + IRNEN_LS), 0xFF0000);
	cbm_w32((LS_BASE + LS_GLBL_CTRL), (0x01 << LS_GLBL_CTRL_EN_POS));

	pr_info("Load spreader init successfully\n");
}

void cbm_dw_memset(u32 *base, int val, u32 size)
{
	int i;

	for (i = 0; i < size; i++)
		base[i] = val;
}

int cqm_dma_get_controller(char *ctrl)
{
	if (!strcmp(ctrl, "DMA1TX"))
		return DMA1TX;
	else if (!strcmp(ctrl, "DMA1RX"))
		return DMA1RX;
	else if (!strcmp(ctrl, "DMA2TX"))
		return DMA2TX;
	else if (!strcmp(ctrl, "DMA2RX"))
		return DMA2RX;
	else if (!strcmp(ctrl, "DMA3"))
		return DMA3;
	else
		return DMAMAX;
}

