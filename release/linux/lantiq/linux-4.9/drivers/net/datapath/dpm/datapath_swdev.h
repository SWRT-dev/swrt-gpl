#ifndef DATAPATH_SWDEV_H
#define DATAPATH_SWDEV_H

#include <linux/if.h>
//TODO added below header for local compilation
#include <linux/etherdevice.h>
#include <net/switchdev.h>
#include <net/switch_api/lantiq_gsw_api.h> /*Switch related structures */
#include <net/switch_api/lantiq_gsw.h>

#define ENABLE 1
#define DISABLE 0

#define BRIDGE_ID_ENTRY_HASH_SHIFT			0
#define BRIDGE_ID_ENTRY_HASH_LENGTH			6
#define BRIDGE_ID_ENTRY_HASH_MASK ((BIT(BRIDGE_ID_ENTRY_HASH_LENGTH)) - 1)
#define BR_ID_ENTRY_HASH_TABLE_SIZE BIT(BRIDGE_ID_ENTRY_HASH_LENGTH)
#define BRIDGE_ID_ENTRY_HASH_TABLE_VALUE(x) \
	(((u32)(x) >> BRIDGE_ID_ENTRY_HASH_SHIFT) & BRIDGE_ID_ENTRY_HASH_MASK)

#define ADD_BRENTRY	0x00000001
#define DEL_BRENTRY	0x00000002
#define BRIDGE_NO_ACTION 0x00000004
#define LOGIC_DEV_REGISTER 0x00000008

struct bridge_id_entry_item {
	u8 br_device_name[IFNAMSIZ];
	u16 fid;
	u32 flags;
	u32 portid; /*Bridge port*/
	u32 dp_port; /*DP port*/
	u32 inst;
	int alloc_flag; /* Same as alloc flag passed from dp_alloc_port */
	struct switchdev_trans_item tritem;
};

struct br_info {
	struct hlist_node br_hlist;
	u8 br_device_name[IFNAMSIZ];
	u8  cpu_port; /*To set CPU port as member or not*/
	u16 fid;
	u32 flag;
	u32 inst;
	int alloc_flag; /* Same as alloc flag passed from dp_alloc_port */
	struct list_head bp_list;
};

struct bridge_member_port {
	struct list_head list;
	u32 portid;
	u32 dev_reg_flag;
	u16 bport[8];
	int alloc_flag; /* Same as alloc flag passed from dp_alloc_port */
};

struct fdb_tbl {
	struct list_head fdb_list;
	struct net_device *port_dev;
	u8 addr[ETH_ALEN];
	__be16 vid;
};

extern struct list_head fdb_tbl_list;
extern struct hlist_head
	g_bridge_id_entry_hash_table[DP_MAX_INST][BR_ID_ENTRY_HASH_TABLE_SIZE];
int dp_swdev_bridge_id_entry_free(int instance);
int dp_swdev_bridge_id_entry_init(void);
struct br_info *dp_swdev_bridge_entry_lookup(char *br_name);
int dp_swdev_chk_bport_in_br(struct net_device *bp_dev, int bport, int inst);
u16 crc_cal(const u8 *data, u16 len);
u16 dp_swdev_cal_hash(u8 *dev_name);
void dp_switchdev_exit(void);

#endif /*DATAPATH_SWDEV_H*/
