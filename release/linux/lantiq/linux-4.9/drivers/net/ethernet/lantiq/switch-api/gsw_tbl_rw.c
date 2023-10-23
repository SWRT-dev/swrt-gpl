/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <gsw_init.h>

GSW_return_t gsw_bm_table_read(void *cdev, bmtbl_prog_t *ptdata)
{
	GSW_return_t err = GSW_statusOk;
	int i, noOfValues;

	if (ptdata->b64bitMode)
		ptdata->tableID |= 0x40;

	noOfValues = ptdata->numValues;
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, (u32) ptdata->adr.raw);
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ((u32)ptdata->tableID) | 0x8000);
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);

	for (i = 0; (i < noOfValues) /*&& (GSW_statusOk == err)*/; i++)
		gsw_r32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET - i, &(ptdata->value[i]));

	return err;
}

GSW_return_t gsw_bm_table_write(void *cdev, bmtbl_prog_t *ptdata)
{
	GSW_return_t err = GSW_statusOk;
	int i, noOfValues;

	if (ptdata->b64bitMode)
		ptdata->tableID |= 0x40;

	noOfValues = ptdata->numValues;
	gsw_w32_raw(cdev, BM_RAM_ADDR_REG_OFFSET, (u32) ptdata->adr.raw);
	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ((u32)ptdata->tableID) | 0x0020);

	for (i = 0; (i < noOfValues) /*&& (GSW_statusOk == err)*/; i++)
		gsw_w32_raw(cdev, BM_RAM_VAL_0_VAL0_OFFSET - i, ptdata->value[i]);

	gsw_w32_raw(cdev, BM_RAM_CTRL_REG_OFFSET, ((u32)ptdata->tableID) | 0x8020);
	CHECK_BUSY(BM_RAM_CTRL_BAS_OFFSET, BM_RAM_CTRL_BAS_SHIFT,
		   BM_RAM_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	return err;
}

u32 pmac_addr_off(u32 off, u32 id)
{

	if (id == 1)
		off = off + 0x80;
	else if (id == 2)
		off = off + 0x200;

	return off;
}

int xwayflow_pmac_table_read(void *cdev, pmtbl_prog_t *ptdata)
{
	u32 value;

	CHECK_BUSY(pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		   PMAC_TBL_CTRL_BAS_SHIFT, PMAC_TBL_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_ADDR_ADDR_OFFSET, ptdata->pmacId),
		PMAC_TBL_ADDR_ADDR_SHIFT,
		PMAC_TBL_ADDR_ADDR_SIZE, ptdata->ptaddr);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_ADDR_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_ADDR_SHIFT,
		PMAC_TBL_CTRL_ADDR_SIZE, ptdata->ptcaddr);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_OPMOD_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_OPMOD_SHIFT,
		PMAC_TBL_CTRL_OPMOD_SIZE, 0 /* ptdata->op_mode */);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_BAS_SHIFT,
		PMAC_TBL_CTRL_BAS_SIZE, 1);
	CHECK_BUSY(pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		   PMAC_TBL_CTRL_BAS_SHIFT, PMAC_TBL_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_r32(cdev, pmac_addr_off(PMAC_TBL_VAL_4_VAL4_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_4_VAL4_SHIFT,
		PMAC_TBL_VAL_4_VAL4_SIZE, &value);
	ptdata->val[4] = value;
	gsw_r32(cdev, pmac_addr_off(PMAC_TBL_VAL_3_VAL3_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_3_VAL3_SHIFT,
		PMAC_TBL_VAL_3_VAL3_SIZE, &value);
	ptdata->val[3] = value;
	gsw_r32(cdev, pmac_addr_off(PMAC_TBL_VAL_2_VAL2_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_2_VAL2_SHIFT,
		PMAC_TBL_VAL_2_VAL2_SIZE, &value);
	ptdata->val[2] = value;
	gsw_r32(cdev, pmac_addr_off(PMAC_TBL_VAL_1_VAL1_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_1_VAL1_SHIFT,
		PMAC_TBL_VAL_1_VAL1_SIZE, &value);
	ptdata->val[1] = value;
	gsw_r32(cdev, pmac_addr_off(PMAC_TBL_VAL_0_VAL0_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_0_VAL0_SHIFT,
		PMAC_TBL_VAL_0_VAL0_SIZE, &value);
	ptdata->val[0] = value;
	return 0;
}

int xwayflow_pmac_table_write(void *cdev, pmtbl_prog_t *ptdata)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (!gswdev) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (IS_VRSN_31(gswdev->gipver)) {
		GSW_Freeze();
	}

	CHECK_BUSY(pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		   PMAC_TBL_CTRL_BAS_SHIFT, PMAC_TBL_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_ADDR_ADDR_OFFSET, ptdata->pmacId),
		PMAC_TBL_ADDR_ADDR_SHIFT,
		PMAC_TBL_ADDR_ADDR_SIZE, ptdata->ptaddr);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_ADDR_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_ADDR_SHIFT,
		PMAC_TBL_CTRL_ADDR_SIZE, ptdata->ptcaddr);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_OPMOD_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_OPMOD_SHIFT,
		PMAC_TBL_CTRL_OPMOD_SIZE, 1 /* ptdata->op_mode */);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_VAL_4_VAL4_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_4_VAL4_SHIFT,
		PMAC_TBL_VAL_4_VAL4_SIZE, ptdata->val[4]);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_VAL_3_VAL3_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_3_VAL3_SHIFT,
		PMAC_TBL_VAL_3_VAL3_SIZE, ptdata->val[3]);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_VAL_2_VAL2_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_2_VAL2_SHIFT,
		PMAC_TBL_VAL_2_VAL2_SIZE, ptdata->val[2]);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_VAL_1_VAL1_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_1_VAL1_SHIFT,
		PMAC_TBL_VAL_1_VAL1_SIZE, ptdata->val[1]);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_VAL_0_VAL0_OFFSET, ptdata->pmacId),
		PMAC_TBL_VAL_0_VAL0_SHIFT,
		PMAC_TBL_VAL_0_VAL0_SIZE, ptdata->val[0]);
	gsw_w32(cdev, pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		PMAC_TBL_CTRL_BAS_SHIFT,
		PMAC_TBL_CTRL_BAS_SIZE, 1);
	CHECK_BUSY(pmac_addr_off(PMAC_TBL_CTRL_BAS_OFFSET, ptdata->pmacId),
		   PMAC_TBL_CTRL_BAS_SHIFT, PMAC_TBL_CTRL_BAS_SIZE, RETURN_FROM_FUNCTION);

	if (IS_VRSN_31(gswdev->gipver)) {
		GSW_UnFreeze();
	}

	return 0;
}

int route_table_read(void *cdev, pctbl_prog_t *rdata)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;

	do {
		gsw_r32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
			PCE_RTBL_CTRL_BAS_SHIFT,
			PCE_RTBL_CTRL_BAS_SIZE, &value);
	} while (value != 0);

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	gsw_w32(cdev, PCE_TBL_ADDR_ADDR_OFFSET,
		PCE_TBL_ADDR_ADDR_SHIFT,
		PCE_TBL_ADDR_ADDR_SIZE, rdata->pcindex);
	gsw_w32(cdev, PCE_RTBL_CTRL_ADDR_OFFSET,
		PCE_RTBL_CTRL_ADDR_SHIFT,
		PCE_RTBL_CTRL_ADDR_SIZE, rdata->table);
	gsw_w32(cdev, PCE_RTBL_CTRL_OPMOD_OFFSET,
		PCE_RTBL_CTRL_OPMOD_SHIFT,
		PCE_RTBL_CTRL_OPMOD_SIZE, rdata->op_mode);
	gsw_w32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
		PCE_RTBL_CTRL_BAS_SHIFT,
		PCE_RTBL_CTRL_BAS_SIZE, 1);

	do {
		gsw_r32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
			PCE_RTBL_CTRL_BAS_SHIFT,
			PCE_RTBL_CTRL_BAS_SIZE, &value);
	} while (value != 0);

	gsw_r32(cdev, PCE_TBL_KEY_15_KEY15_OFFSET,
		PCE_TBL_KEY_15_KEY15_SHIFT,
		PCE_TBL_KEY_15_KEY15_SIZE, &value);
	rdata->key[15] = value;
	gsw_r32(cdev, PCE_TBL_KEY_14_KEY14_OFFSET,
		PCE_TBL_KEY_14_KEY14_SHIFT,
		PCE_TBL_KEY_14_KEY14_SIZE, &value);
	rdata->key[14] = value;
	gsw_r32(cdev, PCE_TBL_KEY_13_KEY13_OFFSET,
		PCE_TBL_KEY_13_KEY13_SHIFT,
		PCE_TBL_KEY_13_KEY13_SIZE, &value);
	rdata->key[13] = value;
	gsw_r32(cdev, PCE_TBL_KEY_12_KEY12_OFFSET,
		PCE_TBL_KEY_12_KEY12_SHIFT,
		PCE_TBL_KEY_12_KEY12_SIZE, &value);
	rdata->key[12] = value;
	gsw_r32(cdev, PCE_TBL_KEY_11_KEY11_OFFSET,
		PCE_TBL_KEY_11_KEY11_SHIFT,
		PCE_TBL_KEY_11_KEY11_SIZE, &value);
	rdata->key[11] = value;
	gsw_r32(cdev, PCE_TBL_KEY_10_KEY10_OFFSET,
		PCE_TBL_KEY_10_KEY10_SHIFT,
		PCE_TBL_KEY_10_KEY10_SIZE, &value);
	rdata->key[10] = value;
	gsw_r32(cdev, PCE_TBL_KEY_9_KEY9_OFFSET,
		PCE_TBL_KEY_9_KEY9_SHIFT,
		PCE_TBL_KEY_9_KEY9_SIZE, &value);
	rdata->key[9] = value;
	gsw_r32(cdev, PCE_TBL_KEY_8_KEY8_OFFSET,
		PCE_TBL_KEY_8_KEY8_SHIFT,
		PCE_TBL_KEY_8_KEY8_SIZE, &value);
	rdata->key[8] = value;
	gsw_r32(cdev, PCE_TBL_KEY_7_KEY7_OFFSET,
		PCE_TBL_KEY_7_KEY7_SHIFT,
		PCE_TBL_KEY_7_KEY7_SIZE, &value);
	rdata->key[7] = value;
	gsw_r32(cdev, PCE_TBL_KEY_6_KEY6_OFFSET,
		PCE_TBL_KEY_6_KEY6_SHIFT,
		PCE_TBL_KEY_6_KEY6_SIZE, &value);
	rdata->key[6] = value;
	gsw_r32(cdev, PCE_TBL_KEY_5_KEY5_OFFSET,
		PCE_TBL_KEY_5_KEY5_SHIFT,
		PCE_TBL_KEY_5_KEY5_SIZE, &value);
	rdata->key[5] = value;
	gsw_r32(cdev, PCE_TBL_KEY_4_KEY4_OFFSET,
		PCE_TBL_KEY_4_KEY4_SHIFT,
		PCE_TBL_KEY_4_KEY4_SIZE, &value);
	rdata->key[4] = value;
	gsw_r32(cdev, PCE_TBL_KEY_3_KEY3_OFFSET,
		PCE_TBL_KEY_3_KEY3_SHIFT,
		PCE_TBL_KEY_3_KEY3_SIZE, &value);
	rdata->key[3] = value;
	gsw_r32(cdev, PCE_TBL_KEY_2_KEY2_OFFSET,
		PCE_TBL_KEY_2_KEY2_SHIFT,
		PCE_TBL_KEY_2_KEY2_SIZE, &value);
	rdata->key[2] = value;
	gsw_r32(cdev, PCE_TBL_KEY_1_KEY1_OFFSET,
		PCE_TBL_KEY_1_KEY1_SHIFT,
		PCE_TBL_KEY_1_KEY1_SIZE, &value);
	rdata->key[1] = value;
	gsw_r32(cdev, PCE_TBL_KEY_0_KEY0_OFFSET,
		PCE_TBL_KEY_0_KEY0_SHIFT,
		PCE_TBL_KEY_0_KEY0_SIZE, &value);
	rdata->key[0] = value;

	gsw_r32(cdev, PCE_TBL_VAL_15_VAL15_OFFSET,
		PCE_TBL_VAL_15_VAL15_SHIFT,
		PCE_TBL_VAL_15_VAL15_SIZE, &value);
	rdata->val[15] = value;
	gsw_r32(cdev, PCE_TBL_VAL_14_VAL14_OFFSET,
		PCE_TBL_VAL_14_VAL14_SHIFT,
		PCE_TBL_VAL_14_VAL14_SIZE, &value);
	rdata->val[14] = value;
	gsw_r32(cdev, PCE_TBL_VAL_13_VAL13_OFFSET,
		PCE_TBL_VAL_13_VAL13_SHIFT,
		PCE_TBL_VAL_13_VAL13_SIZE, &value);
	rdata->val[13] = value;
	gsw_r32(cdev, PCE_TBL_VAL_12_VAL12_OFFSET,
		PCE_TBL_VAL_12_VAL12_SHIFT,
		PCE_TBL_VAL_12_VAL12_SIZE, &value);
	rdata->val[12] = value;
	gsw_r32(cdev, PCE_TBL_VAL_11_VAL11_OFFSET,
		PCE_TBL_VAL_11_VAL11_SHIFT,
		PCE_TBL_VAL_11_VAL11_SIZE, &value);
	rdata->val[11] = value;
	gsw_r32(cdev, PCE_TBL_VAL_10_VAL10_OFFSET,
		PCE_TBL_VAL_10_VAL10_SHIFT,
		PCE_TBL_VAL_10_VAL10_SIZE, &value);
	rdata->val[10] = value;
	gsw_r32(cdev, PCE_TBL_VAL_9_VAL9_OFFSET,
		PCE_TBL_VAL_9_VAL9_SHIFT,
		PCE_TBL_VAL_9_VAL9_SIZE, &value);
	rdata->val[9] = value;
	gsw_r32(cdev, PCE_TBL_VAL_8_VAL8_OFFSET,
		PCE_TBL_VAL_8_VAL8_SHIFT,
		PCE_TBL_VAL_8_VAL8_SIZE, &value);
	rdata->val[8] = value;
	gsw_r32(cdev, PCE_TBL_VAL_7_VAL7_OFFSET,
		PCE_TBL_VAL_7_VAL7_SHIFT,
		PCE_TBL_VAL_7_VAL7_SIZE, &value);
	rdata->val[7] = value;
	gsw_r32(cdev, PCE_TBL_VAL_6_VAL6_OFFSET,
		PCE_TBL_VAL_6_VAL6_SHIFT,
		PCE_TBL_VAL_6_VAL6_SIZE, &value);
	rdata->val[6] = value;
	gsw_r32(cdev, PCE_TBL_VAL_5_VAL5_OFFSET,
		PCE_TBL_VAL_5_VAL5_SHIFT,
		PCE_TBL_VAL_5_VAL5_SIZE, &value);
	rdata->val[5] = value;
	gsw_r32(cdev, PCE_TBL_VAL_4_VAL4_OFFSET,
		PCE_TBL_VAL_4_VAL4_SHIFT,
		PCE_TBL_VAL_4_VAL4_SIZE, &value);
	rdata->val[4] = value;
	gsw_r32(cdev, PCE_TBL_VAL_3_VAL3_OFFSET,
		PCE_TBL_VAL_3_VAL3_SHIFT,
		PCE_TBL_VAL_3_VAL3_SIZE, &value);
	rdata->val[3] = value;
	gsw_r32(cdev, PCE_TBL_VAL_2_VAL2_OFFSET,
		PCE_TBL_VAL_2_VAL2_SHIFT,
		PCE_TBL_VAL_2_VAL2_SIZE, &value);
	rdata->val[2] = value;
	gsw_r32(cdev, PCE_TBL_VAL_1_VAL1_OFFSET,
		PCE_TBL_VAL_1_VAL1_SHIFT,
		PCE_TBL_VAL_1_VAL1_SIZE, &value);
	rdata->val[1] = value;
	gsw_r32(cdev, PCE_TBL_VAL_0_VAL0_OFFSET,
		PCE_TBL_VAL_0_VAL0_SHIFT,
		PCE_TBL_VAL_0_VAL0_SIZE, &value);
	rdata->val[0] = value;
	gsw_r32(cdev, PCE_TBL_MASK_0_MASK0_OFFSET,
		PCE_TBL_MASK_0_MASK0_SHIFT,
		PCE_TBL_MASK_0_MASK0_SIZE, &value);
	rdata->mask[0] = value;
	gsw_r32(cdev, PCE_RTBL_CTRL_VLD_OFFSET,
		PCE_RTBL_CTRL_VLD_SHIFT,
		PCE_RTBL_CTRL_VLD_SIZE, &value);
	rdata->valid = value;
	gsw_w32(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0, 16, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

int route_table_write(void *cdev, pctbl_prog_t *rdata)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	u32 value;
	u16 udata;

	do {
		gsw_r32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
			PCE_RTBL_CTRL_BAS_SHIFT,
			PCE_RTBL_CTRL_BAS_SIZE, &value);
	} while (value);

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	gsw_w32(cdev, PCE_TBL_ADDR_ADDR_OFFSET,
		PCE_TBL_ADDR_ADDR_SHIFT, PCE_TBL_ADDR_ADDR_SIZE,
		rdata->pcindex);

	udata = rdata->table;
	gsw_w32(cdev, PCE_RTBL_CTRL_ADDR_OFFSET,
		PCE_RTBL_CTRL_ADDR_SHIFT,
		PCE_RTBL_CTRL_ADDR_SIZE, udata);

	gsw_w32(cdev, PCE_RTBL_CTRL_OPMOD_OFFSET,
		PCE_RTBL_CTRL_OPMOD_SHIFT,
		PCE_RTBL_CTRL_OPMOD_SIZE, rdata->op_mode);

	gsw_w32(cdev, PCE_TBL_KEY_15_KEY15_OFFSET,
		PCE_TBL_KEY_15_KEY15_SHIFT,
		PCE_TBL_KEY_15_KEY15_SIZE, rdata->key[15]);
	gsw_w32(cdev, PCE_TBL_KEY_14_KEY14_OFFSET,
		PCE_TBL_KEY_14_KEY14_SHIFT,
		PCE_TBL_KEY_14_KEY14_SIZE, rdata->key[14]);
	gsw_w32(cdev, PCE_TBL_KEY_13_KEY13_OFFSET,
		PCE_TBL_KEY_13_KEY13_SHIFT,
		PCE_TBL_KEY_13_KEY13_SIZE, rdata->key[13]);
	gsw_w32(cdev, PCE_TBL_KEY_12_KEY12_OFFSET,
		PCE_TBL_KEY_12_KEY12_SHIFT,
		PCE_TBL_KEY_12_KEY12_SIZE, rdata->key[12]);
	gsw_w32(cdev, PCE_TBL_KEY_11_KEY11_OFFSET,
		PCE_TBL_KEY_11_KEY11_SHIFT,
		PCE_TBL_KEY_11_KEY11_SIZE, rdata->key[11]);
	gsw_w32(cdev, PCE_TBL_KEY_10_KEY10_OFFSET,
		PCE_TBL_KEY_10_KEY10_SHIFT,
		PCE_TBL_KEY_10_KEY10_SIZE, rdata->key[10]);
	gsw_w32(cdev, PCE_TBL_KEY_9_KEY9_OFFSET,
		PCE_TBL_KEY_9_KEY9_SHIFT,
		PCE_TBL_KEY_9_KEY9_SIZE, rdata->key[9]);
	gsw_w32(cdev, PCE_TBL_KEY_8_KEY8_OFFSET,
		PCE_TBL_KEY_8_KEY8_SHIFT,
		PCE_TBL_KEY_8_KEY8_SIZE, rdata->key[8]);
	gsw_w32(cdev, PCE_TBL_KEY_7_KEY7_OFFSET,
		PCE_TBL_KEY_7_KEY7_SHIFT,
		PCE_TBL_KEY_7_KEY7_SIZE, rdata->key[7]);
	gsw_w32(cdev, PCE_TBL_KEY_6_KEY6_OFFSET,
		PCE_TBL_KEY_6_KEY6_SHIFT,
		PCE_TBL_KEY_6_KEY6_SIZE, rdata->key[6]);
	gsw_w32(cdev, PCE_TBL_KEY_5_KEY5_OFFSET,
		PCE_TBL_KEY_5_KEY5_SHIFT,
		PCE_TBL_KEY_5_KEY5_SIZE, rdata->key[5]);
	gsw_w32(cdev, PCE_TBL_KEY_4_KEY4_OFFSET,
		PCE_TBL_KEY_4_KEY4_SHIFT,
		PCE_TBL_KEY_4_KEY4_SIZE, rdata->key[4]);
	gsw_w32(cdev, PCE_TBL_KEY_3_KEY3_OFFSET,
		PCE_TBL_KEY_3_KEY3_SHIFT,
		PCE_TBL_KEY_3_KEY3_SIZE, rdata->key[3]);
	gsw_w32(cdev, PCE_TBL_KEY_2_KEY2_OFFSET,
		PCE_TBL_KEY_2_KEY2_SHIFT,
		PCE_TBL_KEY_2_KEY2_SIZE, rdata->key[2]);
	gsw_w32(cdev, PCE_TBL_KEY_1_KEY1_OFFSET,
		PCE_TBL_KEY_1_KEY1_SHIFT,
		PCE_TBL_KEY_1_KEY1_SIZE, rdata->key[1]);
	gsw_w32(cdev, PCE_TBL_KEY_0_KEY0_OFFSET,
		PCE_TBL_KEY_0_KEY0_SHIFT,
		PCE_TBL_KEY_0_KEY0_SIZE, rdata->key[0]);

	gsw_w32(cdev, PCE_TBL_MASK_0_MASK0_OFFSET,
		PCE_TBL_MASK_0_MASK0_SHIFT,
		PCE_TBL_MASK_0_MASK0_SIZE, rdata->mask[0]);

	gsw_w32(cdev, PCE_TBL_VAL_15_VAL15_OFFSET,
		PCE_TBL_VAL_15_VAL15_SHIFT,
		PCE_TBL_VAL_15_VAL15_SIZE, rdata->val[15]);
	gsw_w32(cdev, PCE_TBL_VAL_14_VAL14_OFFSET,
		PCE_TBL_VAL_14_VAL14_SHIFT,
		PCE_TBL_VAL_14_VAL14_SIZE, rdata->val[14]);
	gsw_w32(cdev, PCE_TBL_VAL_13_VAL13_OFFSET,
		PCE_TBL_VAL_13_VAL13_SHIFT,
		PCE_TBL_VAL_13_VAL13_SIZE, rdata->val[13]);
	gsw_w32(cdev, PCE_TBL_VAL_12_VAL12_OFFSET,
		PCE_TBL_VAL_12_VAL12_SHIFT,
		PCE_TBL_VAL_12_VAL12_SIZE, rdata->val[12]);
	gsw_w32(cdev, PCE_TBL_VAL_11_VAL11_OFFSET,
		PCE_TBL_VAL_11_VAL11_SHIFT,
		PCE_TBL_VAL_11_VAL11_SIZE, rdata->val[11]);
	gsw_w32(cdev, PCE_TBL_VAL_10_VAL10_OFFSET,
		PCE_TBL_VAL_10_VAL10_SHIFT,
		PCE_TBL_VAL_10_VAL10_SIZE, rdata->val[10]);
	gsw_w32(cdev, PCE_TBL_VAL_9_VAL9_OFFSET,
		PCE_TBL_VAL_9_VAL9_SHIFT,
		PCE_TBL_VAL_9_VAL9_SIZE, rdata->val[9]);
	gsw_w32(cdev, PCE_TBL_VAL_8_VAL8_OFFSET,
		PCE_TBL_VAL_8_VAL8_SHIFT,
		PCE_TBL_VAL_8_VAL8_SIZE, rdata->val[8]);
	gsw_w32(cdev, PCE_TBL_VAL_7_VAL7_OFFSET,
		PCE_TBL_VAL_7_VAL7_SHIFT,
		PCE_TBL_VAL_7_VAL7_SIZE, rdata->val[7]);
	gsw_w32(cdev, PCE_TBL_VAL_6_VAL6_OFFSET,
		PCE_TBL_VAL_6_VAL6_SHIFT,
		PCE_TBL_VAL_6_VAL6_SIZE, rdata->val[6]);
	gsw_w32(cdev, PCE_TBL_VAL_5_VAL5_OFFSET,
		PCE_TBL_VAL_5_VAL5_SHIFT,
		PCE_TBL_VAL_5_VAL5_SIZE, rdata->val[5]);
	gsw_w32(cdev, PCE_TBL_VAL_4_VAL4_OFFSET,
		PCE_TBL_VAL_4_VAL4_SHIFT,
		PCE_TBL_VAL_4_VAL4_SIZE, rdata->val[4]);
	gsw_w32(cdev, PCE_TBL_VAL_3_VAL3_OFFSET,
		PCE_TBL_VAL_3_VAL3_SHIFT,
		PCE_TBL_VAL_3_VAL3_SIZE, rdata->val[3]);
	gsw_w32(cdev, PCE_TBL_VAL_2_VAL2_OFFSET,
		PCE_TBL_VAL_2_VAL2_SHIFT,
		PCE_TBL_VAL_2_VAL2_SIZE, rdata->val[2]);
	gsw_w32(cdev, PCE_TBL_VAL_1_VAL1_OFFSET,
		PCE_TBL_VAL_1_VAL1_SHIFT,
		PCE_TBL_VAL_1_VAL1_SIZE, rdata->val[1]);
	gsw_w32(cdev, PCE_TBL_VAL_0_VAL0_OFFSET,
		PCE_TBL_VAL_0_VAL0_SHIFT,
		PCE_TBL_VAL_0_VAL0_SIZE, rdata->val[0]);
	gsw_w32(cdev, PCE_RTBL_CTRL_VLD_OFFSET,
		PCE_RTBL_CTRL_VLD_SHIFT,
		PCE_RTBL_CTRL_VLD_SIZE, rdata->valid);
	gsw_w32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
		PCE_RTBL_CTRL_BAS_SHIFT,
		PCE_RTBL_CTRL_BAS_SIZE, 1);

	do {
		gsw_r32(cdev, PCE_RTBL_CTRL_BAS_OFFSET,
			PCE_RTBL_CTRL_BAS_SHIFT,
			PCE_RTBL_CTRL_BAS_SIZE, &value);
	} while (value != 0);

	gsw_w32(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0, 16, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

int gsw_pce_table_write(void *cdev, pctbl_prog_t *ptdata)
{
	u32 ctrlval;
	u16 i, j;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("\n%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	gsw_w32_raw(cdev, PCE_TBL_ADDR_ADDR_OFFSET, ptdata->pcindex);
	/*TABLE ADDRESS*/
	ctrlval = 0;
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_ADDR_SHIFT,
				PCE_TBL_CTRL_ADDR_SIZE, ptdata->table);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_OPMOD_SHIFT,
				PCE_TBL_CTRL_OPMOD_SIZE, PCE_OP_MODE_ADWR);
	/*KEY REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_key;


	if (ptdata->kformat)
		j *= 4;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.key[i], ptdata->key[i]);
	}

	/*MASK REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_mask;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.mask[i], ptdata->mask[i]);
	}

	/*VAL REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_val;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.value[i], ptdata->val[i]);
	}

	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_KEYFORM_SHIFT,
				PCE_TBL_CTRL_KEYFORM_SIZE, ptdata->kformat);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_TYPE_SHIFT,
				PCE_TBL_CTRL_TYPE_SIZE, ptdata->type);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_VLD_SHIFT,
				PCE_TBL_CTRL_VLD_SIZE, ptdata->valid);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_GMAP_SHIFT,
				PCE_TBL_CTRL_GMAP_SIZE, ptdata->group);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
				PCE_TBL_CTRL_BAS_SIZE, 1);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, ctrlval);

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

	gsw_w32_raw(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

static int __gsw_pce_table_read(void *cdev, ethsw_api_dev_t *gswdev,
				pctbl_prog_t *ptdata)
{
	u32 ctrlval, value;
	u16 i, j;

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	gsw_w32_raw(cdev, PCE_TBL_ADDR_ADDR_OFFSET, ptdata->pcindex);
	/*TABLE ADDRESS*/
	ctrlval = 0;
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_ADDR_SHIFT,
				PCE_TBL_CTRL_ADDR_SIZE, ptdata->table);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_OPMOD_SHIFT,
				PCE_TBL_CTRL_OPMOD_SIZE, PCE_OP_MODE_ADRD);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_KEYFORM_SHIFT,
				PCE_TBL_CTRL_KEYFORM_SIZE, ptdata->kformat);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
				PCE_TBL_CTRL_BAS_SIZE, 1);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, ctrlval);

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

	/*KEY REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_key;

	if (ptdata->kformat)
		j *= 4;

	for (i = 0; i < j; i++) {
		gsw_r32_raw(cdev, gswdev->pce_tbl_reg.key[i], &value);
		ptdata->key[i] = value;
	}

	/*MASK REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_mask;

	for (i = 0; i < j; i++) {
		gsw_r32_raw(cdev, gswdev->pce_tbl_reg.mask[i], &value);
		ptdata->mask[i] = value;
	}

	/*VAL REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_val;

	for (i = 0; i < j; i++) {
		gsw_r32_raw(cdev, gswdev->pce_tbl_reg.value[i], &value);
		ptdata->val[i] = value;
	}

	ptdata->type = gsw_field_r32(ctrlval, PCE_TBL_CTRL_TYPE_SHIFT,
				     PCE_TBL_CTRL_TYPE_SIZE);
	ptdata->valid = gsw_field_r32(ctrlval, PCE_TBL_CTRL_VLD_SHIFT,
				      PCE_TBL_CTRL_VLD_SIZE);
	ptdata->group = gsw_field_r32(ctrlval, PCE_TBL_CTRL_GMAP_SHIFT,
				      PCE_TBL_CTRL_GMAP_SIZE);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

int gsw_pce_table_read(void *cdev, pctbl_prog_t *ptdata)
{
	int ret;
	int retry;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("\n%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (!IS_VRSN_31(gswdev->gipver)
	    || ptdata->table != PCE_IGBGP_INDEX
	    || !gswdev->brdgeportconfig_idx[ptdata->pcindex].IndexInUse)
		return __gsw_pce_table_read(cdev, gswdev, ptdata);

	retry = 5;
	while (retry--) {
		u16 val;
		int i;

		ret = __gsw_pce_table_read(cdev, gswdev, ptdata);
		if (ret != GSW_statusOk)
			continue;

		/* Go through port state at val[0] and port map at val[10~18]
		   to ensure at least 1 valid non-zero value is available for
		   reading and table entry is completely loaded.
		 */
		val = ptdata->val[0];
		for (i = 10; i < 18; i++)
			val |= ptdata->val[i];
		if (val)
			break;
	}

	return ret;
}

int gsw_pce_table_key_read(void *cdev, pctbl_prog_t *ptdata)
{
	u32 ctrlval, value;
	u16 i, j;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("\n%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	/*KEY REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_key;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.key[i], ptdata->key[i]);
	}

	ctrlval = 0;
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_ADDR_SHIFT,
				PCE_TBL_CTRL_ADDR_SIZE, ptdata->table);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_OPMOD_SHIFT,
				PCE_TBL_CTRL_OPMOD_SIZE, PCE_OP_MODE_KSRD);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_KEYFORM_SHIFT,
				PCE_TBL_CTRL_KEYFORM_SIZE, 0);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
				PCE_TBL_CTRL_BAS_SIZE, 1);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, ctrlval);

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

	/*VAL REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_val;

	for (i = 0; i < j; i++) {
		gsw_r32_raw(cdev, gswdev->pce_tbl_reg.value[i], &value);
		ptdata->val[i] = value;
	}

	ptdata->type = gsw_field_r32(ctrlval, PCE_TBL_CTRL_TYPE_SHIFT,
				     PCE_TBL_CTRL_TYPE_SIZE);
	ptdata->valid = gsw_field_r32(ctrlval, PCE_TBL_CTRL_VLD_SHIFT,
				      PCE_TBL_CTRL_VLD_SIZE);
	ptdata->group = gsw_field_r32(ctrlval, PCE_TBL_CTRL_GMAP_SHIFT,
				      PCE_TBL_CTRL_GMAP_SIZE);
	gsw_r32_raw(cdev, PCE_TBL_ADDR_ADDR_OFFSET, &value);
	ptdata->pcindex = value;
	gsw_w32_raw(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

int gsw_pce_table_key_write(void *cdev, pctbl_prog_t *ptdata)
{
	u32 ctrlval;
	u16 i, j;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("\n%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	
	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

#ifdef __KERNEL__
	spin_lock_bh(&gswdev->lock_pce_tbl);
#endif

	ctrlval = 0;

	/*TABLE ADDRESS*/
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_ADDR_SHIFT,
				PCE_TBL_CTRL_ADDR_SIZE, ptdata->table);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_OPMOD_SHIFT,
				PCE_TBL_CTRL_OPMOD_SIZE, PCE_OP_MODE_KSWR);

	/*KEY REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_key;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.key[i], ptdata->key[i]);
	}

	/*MASK REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_mask;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.mask[i], ptdata->mask[i]);
	}

	/*VAL REG*/
	j = gswdev->pce_tbl_info[ptdata->table].num_val;

	for (i = 0; i < j; i++) {
		gsw_w32_raw(cdev, gswdev->pce_tbl_reg.value[i], ptdata->val[i]);
	}

	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_KEYFORM_SHIFT,
				PCE_TBL_CTRL_KEYFORM_SIZE, 0);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_TYPE_SHIFT,
				PCE_TBL_CTRL_TYPE_SIZE, ptdata->type);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_VLD_SHIFT,
				PCE_TBL_CTRL_VLD_SIZE, ptdata->valid);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_GMAP_SHIFT,
				PCE_TBL_CTRL_GMAP_SIZE, ptdata->group);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
				PCE_TBL_CTRL_BAS_SIZE, 1);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, ctrlval);

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

	gsw_w32_raw(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}

/**
 * gsw_pce_rule_set()- Enable/Disable PCE Rule
 * @cdev: a pointer to the target &ethsw_api_dev_t
 * @pctbl_prog_t: a pointer to PCE Table target &pctbl_prog_t
 *
 * Parse the valid bit,Table Address and set the valid bit.
 */
GSW_return_t gsw_pce_rule_set(void *cdev, pctbl_prog_t *ptdata)
{
	u32 ctrlval;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (!gswdev || !cdev)
		return GSW_statusErr;

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

#ifdef __KERNEL__
	spin_lock(&gswdev->lock_pce_tbl);
#endif

	gsw_w32_raw(cdev, PCE_TBL_ADDR_ADDR_OFFSET, ptdata->pcindex);
	/*TABLE ADDRESS*/
	ctrlval = 0;
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_ADDR_SHIFT,
				PCE_TBL_CTRL_ADDR_SIZE, ptdata->table);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_OPMOD_SHIFT,
				PCE_TBL_CTRL_OPMOD_SIZE, PCE_OP_MODE_ADWR);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_VLD_SHIFT,
				PCE_TBL_CTRL_VLD_SIZE, ptdata->valid);
	ctrlval = gsw_field_w32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
				PCE_TBL_CTRL_BAS_SIZE, 1);
	gsw_w32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, ctrlval);

	do {
		gsw_r32_raw(cdev, PCE_TBL_CTRL_BAS_OFFSET, &ctrlval);
	} while (gsw_field_r32(ctrlval, PCE_TBL_CTRL_BAS_SHIFT,
			       PCE_TBL_CTRL_BAS_SIZE));

	gsw_w32_raw(cdev, PCE_TBL_CTRL_ADDR_OFFSET, 0);

#ifdef __KERNEL__
	spin_unlock(&gswdev->lock_pce_tbl);
#endif

	return GSW_statusOk;
}


GSW_return_t GSW_DumpTable(void *cdev, GSW_table_t *parm)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	struct core_ops *gsw_ops;


	gsw_ops = cdev;

	if (parm->tbl_id == 1) {
		memset(&parm->ptdata, 0, sizeof(parm->ptdata));

		parm->ptdata.pcindex = parm->tbl_entry;
		parm->ptdata.table = parm->tbl_addr;
#ifdef __KERNEL__
		spin_lock_bh(&gswdev->lock_pce);
#endif
		gsw_pce_table_read(gsw_ops, &parm->ptdata);

#ifdef __KERNEL__
		spin_unlock_bh(&gswdev->lock_pce);
#endif

	}

	if (parm->tbl_id == 2) {
		memset(&parm->ptdata, 0, sizeof(parm->ptdata));

		parm->ptdata.pcindex = parm->tbl_entry;
		parm->ptdata.table = parm->tbl_addr;

#ifdef __KERNEL__
		spin_lock_bh(&gswdev->lock_pae);
#endif
		route_table_read(gsw_ops, &parm->ptdata);

#ifdef __KERNEL__
		spin_unlock_bh(&gswdev->lock_pae);
#endif

	}

	if (parm->tbl_id == 3) {
		memset(&parm->bmtable, 0, sizeof(parm->bmtable));

		parm->bmtable.adr.raw = parm->tbl_entry;
		parm->bmtable.tableID = parm->tbl_addr;
		parm->bmtable.numValues = parm->bm_numValues;

#ifdef __KERNEL__
		spin_lock_bh(&gswdev->lock_bm);
#endif
		gsw_bm_table_read(gsw_ops, &parm->bmtable);

#ifdef __KERNEL__
		spin_unlock_bh(&gswdev->lock_bm);
#endif

	}

	if (parm->tbl_id == 4) {
		memset(&parm->pmactable, 0, sizeof(parm->pmactable));

		parm->pmactable.pmacId = parm->tbl_entry;
		parm->pmactable.ptcaddr = parm->tbl_addr;

#ifdef __KERNEL__
		spin_lock_bh(&gswdev->lock_pmac);
#endif
		xwayflow_pmac_table_read(gsw_ops, &parm->pmactable);

#ifdef __KERNEL__
		spin_unlock_bh(&gswdev->lock_pmac);
#endif
	}

	return GSW_statusOk;
}


