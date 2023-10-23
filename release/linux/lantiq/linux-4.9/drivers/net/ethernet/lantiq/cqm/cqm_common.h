#ifndef __CQM_COMMON_H
#define __CQM_COMMON_H
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/dma/lantiq_dmax.h>
#include <linux/netdevice.h>
#include <net/lantiq_cbm_api.h>

#define CBM_PMAC_DYNAMIC 1000
#define CBM_PORT_INVALID 2000
#define CBM_PMAC_NOT_APPL 3000
#define CBM_PORT_NOT_APPL 255
#define LS_PORT_NUM             4
#define CBM_MAX_INT_LINES       8
#define CBM_DMA_DESC_OWN        1U/*belong to DMA*/
 /* DMA offset to be 128 byte aligned
  * CBM_DMA_DATA_OFFSET + NET_IP_ALIGN + NET_SKB_PAD +
  * CQM_POOL_METADATA = 128 + 6(byte offset for IP ALIGN)
  */
#define CBM_DMA_ALIGN 128
#define CBM_BYTE_OFFSET 6
#define CBM_DMA_DATA_OFFSET     \
(CBM_DMA_ALIGN + CBM_BYTE_OFFSET - NET_IP_ALIGN  - \
NET_SKB_PAD - CQM_POOL_METADATA)
/* TCP lite and LRO expects it to be 128 */
#define CBM_GRX550_DMA_DATA_OFFSET     128
#define DEFAULT_WAIT_CYCLES     20
#define DEFAULT_LS_QTHH         7
#define DEFAULT_LS_OVFL_CNT     0x2000
#define DEFAULT_LS_PORT_WEIGHT  2
#define CBM_SUCCESS             0
#define CBM_FAILURE             -1
#define CBM_EQM_DELAY_ENQ 0x10
#define CBM_PDU_TYPE 26

#define CBM_LS_PORT(idx, reg) \
(LS_DESC_DW0_PORT0 + ((idx) * 0x100) + offsetof(struct cbm_ls_reg, reg))

#define cbm_r32(m_reg)		readl(m_reg)
#define cbm_w32(m_reg, val)	writel(val, m_reg)
#define cbm_assert(cond, fmt, arg...) \
do { \
	if (!(cond)) \
		pr_err("%d:%s:" fmt "\n", __LINE__, __func__, ##arg); \
} while (0)
#define cbm_err(fmt, arg...) \
pr_err("%d:%s:"fmt "\n", __LINE__, __func__, ##arg)

#ifdef CBM_DEBUG
#define cbm_debug(fmt, arg...) \
pr_info(fmt, ##arg)
#else
#define cbm_debug(fmt, arg...)
#endif

#ifdef CBM_DEBUG_LVL_1
#define cbm_debug_1(fmt, arg...) \
pr_info(fmt, ##arg)
#else
#define cbm_debug_1(fmt, arg...)
#endif

#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))

enum {
	SPREAD_WRR = 0,
	SPREAD_FLOWID = 1,
	SPREAD_MAX,
};

struct cbm_ls_reg {
	struct cbm_desc desc;
	u32 ctrl;
	u32 status;
	u32 resv0[2];
	struct cbm_desc qdesc[7];
};

struct qidt_flag_done {
	u8 cls_done;
	u8 ep_done;
	u8 fl_done;
	u8 fh_done;
	u8 dec_done;
	u8 enc_done;
	u8 mpe1_done;
	u8 mpe2_done;
	u8 sub_if_id_done;
	u8 sub_if_dc_done;
};

struct cbm_q_info {
u16 refcnt; /* No of Queue Map table entries pointing to this q */
u16 qmap_idx_start; /* First index of Queue Map table pointing to q */
u16 qmap_idx_end; /* last index of Queue Map table pointing to q */
};

struct cbm_qidt_shadow {
u32 qidt_shadow;
u32 qidt_drop_flag;
};

static inline void set_val(void __iomem *reg, u32 val, u32 mask, u32 offset)
{
	u32 temp_val = cbm_r32(reg);

	temp_val &= ~(mask);
	temp_val |= (((val) << (offset)) & (mask));
	cbm_w32(reg, temp_val);
}

static inline int cqm_desc_data_len(u32 dw)
{
	return dw & 0x0000FFFF;
}

static inline int cqm_desc_data_off(u32 dw)
{
	return (dw & 0x3800000) >> 23;
}

static inline int cqm_desc_data_pool(u32 dw)
{
	return (dw & 0x70000) >> 16;
}

static inline int cqm_desc_data_policy(u32 dw)
{
	return (dw & 0x700000) >> 20;
}

static inline int get_is_bit_set(u32 flags)
{
	return ffs(flags) - 1;
}

void buf_addr_adjust(unsigned int buf_base_addr, unsigned int buf_size,
		     dma_addr_t buf_dma_addr,
		     unsigned int *adjusted_buf_base,
		     unsigned int *adjusted_buf_size,
		     dma_addr_t *adjusted_dma_addr,
		     unsigned int align);
int cbm_linearise_buf(struct sk_buff *skb, struct cbm_tx_data *data,
		      int buf_size, u32 new_buf);
void init_cbm_ls(void __iomem *cqm_ls_addr_base);
int cqm_dma_get_controller(char *ctrl);
void ls_intr_ctrl(u32 val, void __iomem *cqm_ls_addr_base);
void cbm_dw_memset(u32 *base, int val, u32 size);
void register_cbm(const struct cbm_ops *cbm_cb);
#endif

