/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <gsw_init.h>
#include <gsw_swmcast.h>

static MCAST_HASHTBL phtable[MCAST_TABLE_SIZE];

static int get_hashtable_empty_slot(void *cdev, MCAST_HASHTBL *phtable);
static u16 cal_hash(u32 src_ip_mode, MCAST_HASHTBL_PTN *pattern, u32 type);
static u16 crcmsb(const u8 *data, u32 len);
static int set_pce_hash_table(void *cdev, MCAST_HASHTBL *phtable, u32 loc);
static void update_bridge_pmap(u32 portId, MCAST_HASHTBL *table_ptr, u32 val);
static int search_hashtable_entry(u32 hashidx, MCAST_HASHTBL_PTN *pattern, u32 *loc);
static int remove_hashentry(void *cdev, u32 hashidx, u32 rm_idx, u32 portId);

static u16 crcmsb(const u8 *data, u32 len)
{
	u16 crc = 0;
	int i;

	if (len) do {
			crc ^= ((*data) << 8);
			data++;

			for (i = 0; i < 8; i++) {
				if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
				else crc <<= 1;

				crc = crc & 0xFFFF;
			}
		} while (--len);

	return crc;
}

static u16 cal_hash(u32 src_ip_mode, MCAST_HASHTBL_PTN *pattern, u32 type)
{
	u8  b[9];
	u16 crc;
	u32 len = 0;
	int i = 0, j = 0;
	u32 ip6data[4];
	u32 xdata;

	memset(b, 0, sizeof(b));

	/* Order is important here, fid, dip, sip */
	b[i] = pattern->fid;

	if (src_ip_mode == GSW_IGMP_MEMBER_INCLUDE) {
		if (type == GSW_IP_SELECT_IPV4) {
			for (i = 0; i < 4; i++)
				b[i + 1] = ((pattern->dstip.nIPv4 >> ((3 - i) * 8)) & 0xFF);

			for (i = 0; i < 4; i++)
				b[i + 5] = ((pattern->srcip.nIPv4 >> ((3 - i) * 8)) & 0xFF);
		} else if (type == GSW_IP_SELECT_IPV6) {

			for (i = 0, j = 0; i < 4; i++, j += 2) {
				ip6data[i] =
					((pattern->dstip.nIPv6[j] << 16) | ((pattern->dstip.nIPv6[j + 1])));
			}

			xdata = (ip6data[0] ^ ip6data[1] ^ ip6data[2] ^ ip6data[3]);

			for (i = 0; i < 4; i++)
				b[i + 1] = ((xdata >> ((3 - i) * 8)) & 0xFF);

			for (i = 0, j = 0; i < 4; i++, j += 2) {
				ip6data[i] =
					((pattern->srcip.nIPv6[j] << 16) | ((pattern->srcip.nIPv6[j + 1])));
			}

			xdata = (ip6data[0] ^ ip6data[1] ^ ip6data[2] ^ ip6data[3]);

			for (i = 0; i < 4; i++)
				b[i + 5] = ((xdata >> ((3 - i) * 8)) & 0xFF);

		}

		len = 9;
	} else {
		if (type == GSW_IP_SELECT_IPV4) {
			for (i = 0; i < 4; i++)
				b[i + 1] = ((pattern->dstip.nIPv4 >> ((3 - i) * 8)) & 0xFF);
		} else if (type == GSW_IP_SELECT_IPV6) {
			for (i = 0, j = 0; i < 4; i++, j += 2) {
				ip6data[i] =
					((pattern->dstip.nIPv6[j] << 16) | ((pattern->dstip.nIPv6[j + 1])));
			}

			xdata = (ip6data[0] ^ ip6data[1] ^ ip6data[2] ^ ip6data[3]);

			for (i = 0; i < 4; i++)
				b[i + 1] = ((xdata >> ((3 - i) * 8)) & 0xFF);
		}

		for (i = 0; i < 4; i++)
			b[i + 5] = 0;

		len = 9;
	}

	crc = crcmsb(b, len);

	crc &= 0x01FF;	/* To make sure it is withing 512 */

	return (crc);
}

static int get_hashtable_empty_slot(void *cdev, MCAST_HASHTBL *phtable)
{
	u32 location_counter = 0;
	MCAST_HASHTBL *pt;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	while (location_counter < gswdev->mctblsize) {
		pt  = &phtable[location_counter];

		if ((pt->nxt_idx == 0xFFFF) && (pt->valid == 0))
			return location_counter;

		location_counter++;
	}

	return TBL_FULL;
}

int gsw_check_hashtable_entry(void *cdev)
{
	u32 location_counter = 0;
	MCAST_HASHTBL *pt;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	while (location_counter < gswdev->mctblsize) {
		pt  = &phtable[location_counter];

		if (pt->valid == 1)
			return 1;

		location_counter++;
	}

	return 0;
}

static void update_bridge_pmap(u32 portId, MCAST_HASHTBL *table_ptr, u32 val)
{
	int i = 0, bit = 0;

	i = (portId / 16);
	bit = (portId % 16);

	if (val == SET)
		table_ptr->action.br_portmap[i] |= (1 << bit);
	else
		table_ptr->action.br_portmap[i] &= ~(1 << bit);

	return;
}

int gsw_init_hash_table(void *cdev)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	u16 pcindex;

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	memset(phtable, 0, (MCAST_HASHTBL_SIZE * gswdev->mctblsize));

	if (IS_VRSN_31(gswdev->gipver)) {

		for (pcindex = 0; pcindex < gswdev->mctblsize; pcindex++) {

			phtable[pcindex].valid  	= 0;
			phtable[pcindex].nxt_idx	= 0xFFFF;
			phtable[pcindex].first_idx	= 0xFFFF;
			phtable[pcindex].idx        = pcindex;

			pcetable.pcindex = pcindex;
			pcetable.table = PCE_MULTICAST_SW_INDEX;
		}

		return GSW_statusOk;
	}

	return GSW_statusNoSupport;
}

int gsw_get_swmcast_entry(void *cdev, GSW_multicastTableRead_t *parm, u32 loc)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	int i = 0;
	GSW_IP_t IP;

	if (gswdev == NULL)
		return 0;

	memset(&pcetable, 0, sizeof(pctbl_prog_t));
	pcetable.pcindex = loc;
	pcetable.table = PCE_MULTICAST_SW_INDEX;
	gsw_pce_table_read(cdev, &pcetable);

	if (pcetable.valid == 1) {
		parm->eModeMember = ((pcetable.key[2] >> 14) & 0x3);

		/* Pattern */
		parm->nFID = pcetable.key[2] & 0x3F;

		for (i = 0; i < 8; i++)
			parm->uIP_Gda.nIPv6[i] = pcetable.key[3 + i];

		for (i = 0; i < 8; i++)
			parm->uIP_Gsa.nIPv6[i] = pcetable.key[11 + i];

		/* Action */
		parm->nSubIfId = ((pcetable.val[1] >> 3) & 0x1FFF);

		for (i = 0; i <= gswdev->hitstatus_idx; i++)
			parm->nPortMap[i] = pcetable.val[2 + i];

		if ((parm->nPortMap[gswdev->hitstatus_idx]
		     & gswdev->hitstatus_mask)) {
			parm->hitstatus = LTQ_TRUE;

			pcetable.val[gswdev->hitstatus_idx + 2] &=
				~gswdev->hitstatus_mask;
			gsw_pce_table_write(cdev, &pcetable);
		} else
			parm->hitstatus = LTQ_FALSE;

		parm->eIPVersion = phtable[loc].ip_type;

#if CONFIG_CPU_BIG_ENDIAN

		if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
			parm->uIP_Gda.nIPv4 = ((parm->uIP_Gda.nIPv6[1] << 16) |
					       parm->uIP_Gda.nIPv6[0]);
			parm->uIP_Gsa.nIPv4 = ((parm->uIP_Gsa.nIPv6[1] << 16) |
					       parm->uIP_Gsa.nIPv6[0]);
		} else if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
			for (i = 0; i < 8; i++)
				IP.nIPv6[i] = parm->uIP_Gda.nIPv6[7 - i];
			
			for (i = 0; i < 8; i++)
				parm->uIP_Gda.nIPv6[i] = IP.nIPv6[i];

			for (i = 0; i < 8; i++)
				IP.nIPv6[i] = parm->uIP_Gsa.nIPv6[7 - i];
			
			for (i = 0; i < 8; i++)
				parm->uIP_Gsa.nIPv6[i] = IP.nIPv6[i];			
		}

#endif
		
		parm->nPortId |= 0x8000;
#ifdef __KERNEL__		
		pr_debug("Got Loc        %d\n", loc);
		pr_debug("Valid	         %d\n", pcetable.valid);
		pr_debug("ExclSrcIP      %d\n", parm->bExclSrcIP);
		pr_debug("nFID	         %d\n", parm->nFID);
		pr_debug("First Idx      %d\n", pcetable.key[0]);
		pr_debug("Next Idx       %d\n", pcetable.key[1]);

		for (i = 0; i < 8; i++)
			pr_debug("uIP_Gda.nIPv6[%d] %04x\n", i, parm->uIP_Gda.nIPv6[i]);

		for (i = 0; i < 8; i++)
			pr_debug("uIP_Gsa.nIPv6[%d] %04x\n", i, parm->uIP_Gsa.nIPv6[i]);

		pr_debug("nSubIfId       %d\n", parm->nSubIfId);

		for (i = 0; i < 16; i++)
			pr_debug("nPortMap[%d] %04x\n", i, parm->nPortMap[i]);

		pr_debug("nPortId        %d\n", parm->nPortId);

		pr_debug("ENTRY_FOUND %s:%s:%d\n", __FILE__, __func__, __LINE__);
#endif		
	}

	return pcetable.valid;
}

static int set_pce_hash_table(void *cdev, MCAST_HASHTBL *phtable, u32 loc)
{
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);
	pctbl_prog_t pcetable;
	int i = 0;

	if (gswdev == NULL)
		return 0;

	memset(&pcetable, 0, sizeof(pctbl_prog_t));
	pcetable.pcindex = loc;
	pcetable.table = PCE_MULTICAST_SW_INDEX;
	gsw_pce_table_read(cdev, &pcetable);

	pcetable.pcindex = loc;
	pcetable.table = PCE_MULTICAST_SW_INDEX;

	/* Cannot add 0x3FF to First Index, Msb Bit is used
	 * for stopping search
	 */
	if (phtable->first_idx == 0xFFFF)
		pcetable.key[0] = 0;
	else
		pcetable.key[0] = phtable->first_idx;
	
	pcetable.key[1] = phtable->nxt_idx;
	pcetable.valid = phtable->valid;
	pcetable.key[2] = ((phtable->src_ip_mode & 0x3) << 14);

	/* Pattern */
	pcetable.key[2] |= phtable->key.fid & 0x3F;

#if CONFIG_CPU_BIG_ENDIAN

	if (phtable->ip_type == GSW_IP_SELECT_IPV4) {
		pcetable.key[3] = phtable->key.dstip.nIPv6[1];
		pcetable.key[4] = phtable->key.dstip.nIPv6[0];
		pcetable.key[11] = phtable->key.srcip.nIPv6[1];
		pcetable.key[12] = phtable->key.srcip.nIPv6[0];
	} else if (phtable->ip_type == GSW_IP_SELECT_IPV6) {
		for (i = 0; i < 8; i++) {
			pcetable.key[3 + i] = phtable->key.dstip.nIPv6[7 - i];
		}

		for (i = 0; i < 8; i++) {
			pcetable.key[11 + i] = phtable->key.srcip.nIPv6[7 - i];
		}
	}

#else

	for (i = 0; i < 8; i++) {
		pcetable.key[3 + i] = phtable->key.dstip.nIPv6[i];
	}

	for (i = 0; i < 8; i++) {
		pcetable.key[11 + i] = phtable->key.srcip.nIPv6[i];
	}

#endif

	/* Action */
	pcetable.val[1] = ((phtable->action.subifid & 0x1FFF) << 3);

	/*Set Bridge Port Map*/
	for (i = 0; i < 16; i++)
		pcetable.val[2 + i] = phtable->action.br_portmap[i];

	gsw_pce_table_write(cdev, &pcetable);

	return GSW_statusOk;
}

int gsw_insert_hashtable_entry(void *cdev, GSW_multicastTable_t *parm)
{
	u32 loc = 0, found_loc = 0;
	u32 hashidx;
	int i = 0, ret = 0;
	int new_hash_slot = 0;
	u32 portId;
	MCAST_HASHTBL  *table_ptr = NULL, *new_table_ptr = NULL;
	MCAST_HASHTBL_PTN pattern;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->nPortId > gswdev->num_of_bridge_port) {
		pr_err("PortId only upto %d is supported\n", gswdev->num_of_bridge_port);
		return FAIL;
	}

	portId = parm->nPortId;

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {

		pattern.srcip.nIPv4 = parm->uIP_Gsa.nIPv4;
		pattern.dstip.nIPv4 = parm->uIP_Gda.nIPv4;

		pr_debug("portId = %d\n parm->eIPVersion = %d\n nSubIfId = %d\n uIP_Gsa.nIPv4 = %08x\n uIP_Gda.nIPv4 = %08x\n fid = %d\n bExclSrcIP = %d\n eModeMember = %d\n", parm->nPortId, parm->eIPVersion, parm->nSubIfId, pattern.srcip.nIPv4, pattern.dstip.nIPv4,
			 parm->nFID,  parm->bExclSrcIP, parm->eModeMember);
	} else if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {

		pr_debug("portId = %d\n parm->eIPVersion = %d\n nSubIfId = %d\n fid = %d\n bExclSrcIP = %d\n eModeMember = %d\n",
			 parm->nPortId, parm->eIPVersion, parm->nSubIfId, parm->nFID,  parm->bExclSrcIP, parm->eModeMember);

#if CONFIG_CPU_BIG_ENDIAN

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[i];
		}

#else

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[7 - i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[7 - i];
		}

#endif
	}

	pattern.fid = parm->nFID;

	hashidx = cal_hash(GSW_IGMP_MEMBER_EXCLUDE, &pattern, parm->eIPVersion);

	ret = search_hashtable_entry(hashidx, &pattern, &found_loc);

	if (ret == TABLE_ERROR) {
		pr_err(":::::: IMPOSSIBLE ::::: TABLE ERROR\n");
		return ret;
	} else if (ret ==  MATCH_FOUND) {

		/* Existing Entry Found, update the bridge portmap with the new portid and exit */

		pr_debug("Existing Entry Found, update the bridge portmap with the new portid and exit\n");
		table_ptr = &phtable[found_loc];
		update_bridge_pmap(portId, table_ptr, SET);
		set_pce_hash_table(cdev, table_ptr, found_loc);

		return found_loc;
	}

	/* Get a new location, this is needed for 2 cases,
	   1. New hashidx entry
	   2. Existing hashidx with a new entry
	 */
	new_hash_slot = get_hashtable_empty_slot(cdev, phtable);
	if (new_hash_slot == TBL_FULL) {
		pr_err("Hash Table FULL\n");
		return TBL_FULL;
	} else if (new_hash_slot > MCAST_TABLE_SIZE) {
		pr_err("Location got is wrong\n");
		return FAIL;
	}

	loc = (u32)new_hash_slot;
	pr_debug("Got new location %d\n", loc);

	new_table_ptr = &phtable[loc];

	/* No session yet in this hashidx, add the first entry */
	if (ret == NO_VALID_HASHENTRY) {
		pr_debug("ADDING new hash idx = %x first loc %d\n", hashidx, loc);
		phtable[hashidx].first_idx = loc;
		set_pce_hash_table(cdev, &phtable[hashidx], hashidx);
	}
	/* There is already some entry in the hash index, add new one */
	else if (ret == MATCH_NOT_FOUND) {

		pr_debug("MATCH NOT FOUND in hash idx = %x new entry loc %d\n", hashidx, loc);

		table_ptr = &phtable[hashidx];
		table_ptr = &phtable[table_ptr->first_idx];

		/* Find the last entry of the current hash index */
		while (table_ptr->idx != table_ptr->nxt_idx)
			table_ptr = &phtable[table_ptr->nxt_idx];

		/* Update the existing hash last entry's nxt pointer to the new loc. */
		table_ptr->nxt_idx = loc;

		set_pce_hash_table(cdev, table_ptr, table_ptr->idx);
	}

	memset(&new_table_ptr->key, 0, sizeof(MCAST_HASHTBL_PTN));
	memset(&new_table_ptr->action, 0, sizeof(MCAST_HASHTBL_ACT));

	/* New entry will be the last entry in this hashidx so update the loc to the nxt ptr */
	new_table_ptr->nxt_idx = loc;

	new_table_ptr->valid        = 1;

	if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
		for (i = 0; i < 8; i++) {
			new_table_ptr->key.srcip.nIPv6[i] = pattern.srcip.nIPv6[i];
			new_table_ptr->key.dstip.nIPv6[i] = pattern.dstip.nIPv6[i];
		}
	} else {
		new_table_ptr->key.srcip.nIPv4 = pattern.srcip.nIPv4;
		new_table_ptr->key.dstip.nIPv4 = pattern.dstip.nIPv4;
	}

	new_table_ptr->ip_type = parm->eIPVersion;
	new_table_ptr->key.fid = pattern.fid;
	new_table_ptr->src_ip_mode = parm->eModeMember;
	new_table_ptr->excl_src_ip = parm->bExclSrcIP;

	if (ret == NO_VALID_HASHENTRY) {
		/* Need to update sub if id only for the first entry */
		new_table_ptr->action.subifid = parm->nSubIfId;
	}

	update_bridge_pmap(portId, new_table_ptr, SET);
	set_pce_hash_table(cdev, new_table_ptr, loc);

	return loc;
}

/* If external modules need to search */
int gsw_search_hashtable_entry(void *cdev, GSW_multicastTable_t *parm, GSW_multicastTableRead_t *read_parm, u32 *loc)
{
	u32 found_loc = 0, hashidx;
	int i = 0, ret = 0;
	u32 portId;
	MCAST_HASHTBL_PTN pattern;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return GSW_statusErr;
	}

	if (parm->nPortId > gswdev->num_of_bridge_port) {
		pr_err("PortId only upto %d is supported\n", gswdev->num_of_bridge_port);
		return FAIL;
	}

	portId = parm->nPortId;

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {

		pattern.srcip.nIPv4 = parm->uIP_Gsa.nIPv4;
		pattern.dstip.nIPv4 = parm->uIP_Gda.nIPv4;

	} else if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
#if CONFIG_CPU_BIG_ENDIAN

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[i];
		}

#else

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[7 - i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[7 - i];
		}

#endif
	}

	pattern.fid = parm->nFID;

	hashidx = cal_hash(GSW_IGMP_MEMBER_EXCLUDE, &pattern, parm->eIPVersion);

	ret = search_hashtable_entry(hashidx, &pattern, &found_loc);

	if (ret == TABLE_ERROR) {
		pr_err(":::::: IMPOSSIBLE ::::: TABLE ERROR\n");
	} else if (ret ==  MATCH_FOUND) {
		*loc = found_loc;
		gsw_get_swmcast_entry(cdev, read_parm, found_loc);
	} else if (ret == NO_VALID_HASHENTRY) {
		pr_err("No Valid Entry to the HASHIDX %x\n", hashidx);
		/* There is already some entry in the hash index, add new one */
	} else if (ret == MATCH_NOT_FOUND) {
		pr_err("MATCH_NOT_FOUND to the HASHIDX %x\n", hashidx);
	}

	return ret;
}

static int search_hashtable_entry(u32 hashidx, MCAST_HASHTBL_PTN *pattern, u32 *loc)
{
	int  i = 0;
	MCAST_HASHTBL  *table_ptr;

	pr_debug("SEARCHING session hash idx = %x \n", hashidx);

	table_ptr = &phtable[hashidx];

	if (table_ptr->first_idx == 0xFFFF)
		return NO_VALID_HASHENTRY;

	/* Go Inside the First Ptr */
	table_ptr = &phtable[table_ptr->first_idx];

	for (i = 0; i < MAX_STEP_CNTR; i++) {
		pr_debug("Searching Idx %d\n", table_ptr->idx);

		if (table_ptr->valid) {

			if (table_ptr->ip_type == GSW_IP_SELECT_IPV6) {

				if ((!memcmp(pattern->srcip.nIPv6, table_ptr->key.srcip.nIPv6, sizeof(pattern->srcip))) &&
				    (!memcmp(pattern->dstip.nIPv6, table_ptr->key.dstip.nIPv6, sizeof(pattern->dstip))) &&
				    (pattern->fid == table_ptr->key.fid)) {

					*loc = table_ptr->idx;
					pr_debug("MATCH_FOUND %s:%s:%d\n", __FILE__, __func__, __LINE__);
					return MATCH_FOUND;
				}
			} else {
				if ((pattern->srcip.nIPv4 == table_ptr->key.srcip.nIPv4) &&
				    (pattern->dstip.nIPv4 == table_ptr->key.dstip.nIPv4) &&
				    (pattern->fid == table_ptr->key.fid)) {

					*loc = table_ptr->idx;
					pr_debug("MATCH_FOUND %s:%s:%d\n", __FILE__, __func__, __LINE__);
					return MATCH_FOUND;
				}

			}
		}

		pr_debug("Current Idx %d Nxt Ptr %d\n", table_ptr->idx, table_ptr->nxt_idx);

		if (table_ptr->idx == table_ptr->nxt_idx) {
			pr_err("No match is found\n");
			return MATCH_NOT_FOUND;
		}

		table_ptr = &phtable[table_ptr->nxt_idx];
	}

	pr_debug("MATCH_NOT_FOUND %s:%s:%d\n", __FILE__, __func__, __LINE__);

	return TABLE_ERROR;
}

static int remove_hashentry(void *cdev, u32 hashidx, u32 rm_idx, u32 portId)
{
	u32 i = 0;
	MCAST_HASHTBL *table_ptr = &phtable[hashidx];

	update_bridge_pmap(portId, &phtable[rm_idx], CLR);

	for (i = 0; i < 16; i++) {
		if (phtable[rm_idx].action.br_portmap[i])
			return  UPDATED_BR_PMAP;
	}

	/* Going to remove the entry */

	/* Go Inside the table */
	table_ptr = &phtable[table_ptr->first_idx];

	if (phtable[rm_idx].nxt_idx == rm_idx) { /* we are going to remove the last entry */
		/* first entry to the hash is the last entry */
		if (phtable[hashidx].first_idx == rm_idx) {
			/* Indicate there is no valid session to the particular hashidx */
			phtable[hashidx].first_idx = 0xFFFF;
			set_pce_hash_table(cdev, &phtable[hashidx], hashidx);
		} else {	/* we have atleast 2 entries to the hash */

			/* Find the previous entry to the current remove entry */
			while (rm_idx != table_ptr->nxt_idx)
				table_ptr = &phtable[table_ptr->nxt_idx];

			/* Update the previous entry as the last entry */
			table_ptr->nxt_idx = table_ptr->idx;
			set_pce_hash_table(cdev, table_ptr, table_ptr->idx);
		}
	}
	/* we are going to remove something in middle */
	else {

		/* firt entry to the hash we are going to remove */
		if (phtable[hashidx].first_idx == rm_idx) {
			/* Update the hashidx first ptr entry as the next entry */
			phtable[hashidx].first_idx = table_ptr->nxt_idx;
			set_pce_hash_table(cdev, &phtable[hashidx], hashidx);
		} else {
			/* Find the previous entry to the current remove entry */
			while (rm_idx != table_ptr->nxt_idx)
				table_ptr = &phtable[table_ptr->nxt_idx];

			/* Update the previous entry nxt ptr to the next entry after remove entry */
			table_ptr->nxt_idx = phtable[rm_idx].nxt_idx;
			set_pce_hash_table(cdev, table_ptr, table_ptr->idx);
		}
	}

	phtable[rm_idx].valid = 0;
	phtable[rm_idx].nxt_idx = 0xFFFF;

	return REMOVED_ENTRY;
}

int gsw_remove_hashtable_entry(void *cdev, GSW_multicastTable_t *parm)
{
	u32 hashidx;
	u32 loc = 0, ret = 0;
	int  i = 0;
	MCAST_HASHTBL  *table_ptr = NULL;
	MCAST_HASHTBL_PTN pattern;
	ethsw_api_dev_t *gswdev = GSW_PDATA_GET(cdev);

	if (gswdev == NULL) {
		pr_err("%s:%s:%d", __FILE__, __func__, __LINE__);
		return FAIL;
	}

	if (parm->nPortId > gswdev->num_of_bridge_port) {
		pr_err("PortId only upto %d is supported\n", gswdev->num_of_bridge_port);
		return FAIL;
	}

	if (parm->eIPVersion == GSW_IP_SELECT_IPV4) {
		pattern.srcip.nIPv4 = parm->uIP_Gsa.nIPv4;
		pattern.dstip.nIPv4 = parm->uIP_Gda.nIPv4;

	} else if (parm->eIPVersion == GSW_IP_SELECT_IPV6) {
#if CONFIG_CPU_BIG_ENDIAN

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[i];
		}

#else

		for (i = 0; i < 8; i++) {
			pattern.srcip.nIPv6[i] = parm->uIP_Gsa.nIPv6[7 - i];
			pattern.dstip.nIPv6[i] = parm->uIP_Gda.nIPv6[7 - i];
		}

#endif
	}

	pattern.fid = parm->nFID;

	hashidx = cal_hash(GSW_IGMP_MEMBER_EXCLUDE, &pattern, parm->eIPVersion);

	if (MATCH_FOUND != search_hashtable_entry(hashidx, &pattern, &loc))
		return ENTRY_CANNOT_REMOVE;

	table_ptr = &phtable[loc];

	pr_debug("REMOVING hash idx %x Found loc %d table idx %d\n", hashidx, loc, table_ptr->idx);

	ret = remove_hashentry(cdev, hashidx, loc, parm->nPortId);
	set_pce_hash_table(cdev, table_ptr, loc);

	if (ret == REMOVED_ENTRY)
		pr_debug("REMOVED_ENTRY %s:%s:%d\n", __FILE__, __func__, __LINE__);
	else
		pr_debug("UPDATED BR PMAP ENTRY %s:%s:%d\n", __FILE__, __func__, __LINE__);

	return ret;
}
