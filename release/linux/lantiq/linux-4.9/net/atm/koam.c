// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2009~2015 Lantiq Deutschland GmbH
 * Copyright (C) 2016~2018 Intel Corporation.
 */

#include <generated/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/net.h>		/* struct socket, struct proto_ops */
#include <linux/atm.h>		/* ATM stuff */
#include <linux/atmdev.h>

struct koam_cell {
	unsigned char cell[52];
	unsigned int pid;
};
#define KOAM_CELL_SIZE	sizeof(struct koam_cell)

struct atm_cell_header {
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	struct {
		u32 clp 	:1;	// Cell Loss Priority
		u32 pti		:3;	// Payload Type Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 vpi		:8;	// Vitual Path Identifier
		u32 gfc 	:4;	// Generic Flow Control
	} bit;
#else
	struct {
		u32 gfc 	:4;	// Generic Flow Control
		u32 vpi		:8;	// Vitual Path Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 pti		:3;	// Payload Type Identifier
		u32 clp 	:1;	// Cell Loss Priority
	} bit;
#endif

};

#define KOAM_CELL_HEADER_SIZE	sizeof(struct atm_cell_header)
#define OAM_TYPE_FN             (KOAM_CELL_HEADER_SIZE+0)

static struct sock *koam_sock = NULL;
static unsigned int koam_upid = 0;

static int push_oam_internal(struct atm_vcc *atmvcc, void *cell);
static int oam_failure(void *cell, int cmdId);

static struct atm_vcc *find_atm_vcc(u32 vpi, u32 vci)
{
	struct atm_vcc *vcc = NULL;
	struct sock *atm_sock = NULL;
	struct hlist_head *head;
	int i;
	
	write_lock_irq(&vcc_sklist_lock);
	
	if (vci == 0x3 || vci == 0x4)
	{
		for (i = 0 ; i < VCC_HTABLE_SIZE; i++) 
		{
			if (!(atm_sock = sk_head(&vcc_hash[i])))
				continue;
			vcc = atm_sk(atm_sock);
			if(vcc->vpi == vpi)	
			    	break;
			else 
				vcc = NULL;
		}
	
		write_unlock_irq(&vcc_sklist_lock);
		return vcc;
	}	
	
	head = &vcc_hash[vci & (VCC_HTABLE_SIZE - 1)];
	atm_sock = sk_head(head);	
	for (; atm_sock != NULL; atm_sock=sk_next(atm_sock)) {
		vcc = atm_sk(atm_sock);
		if (vcc->dev == NULL)
			continue;
		if (vcc->vpi == (short)vpi)
			break;
	}
	write_unlock_irq(&vcc_sklist_lock);
	return atm_sk(atm_sock);
}

static void koam_rcv_skb(struct sk_buff *skb)
{
	struct koam_cell *pCell;
	struct atm_cell_header *cell_header;
	struct atm_vcc *vcc;
	u32 vpi, pti;
	u32 vci;

	pCell = (struct koam_cell *)skb->data;
	cell_header = (struct atm_cell_header*)pCell->cell;

	vpi = (u32)(cell_header->bit.vpi);
	vci = (u32)(cell_header->bit.vci);
	pti = (u32)(cell_header->bit.pti);
	
	koam_upid = pCell->pid;
	if (vpi == 0 && vci == 0) {
		printk(KERN_ERR "userspace registering PID\n");
		goto SEND_OAM_END;
	}
	vcc =find_atm_vcc(vpi, vci);
	if(!vcc){
		if (oam_failure(pCell, 8)) { /* cmdId 8: No vcc */
			printk(KERN_ERR "oam error: No vcc for vpi=%d, vci=%d\n", vpi, vci);
		}
		goto SEND_OAM_END;
	}
	if(vcc->dev->ops->send_oam(vcc, pCell->cell, 0)){
		if (oam_failure(pCell, 7)) { /* cmdId 7: No ppe conn */
			printk(KERN_ERR "oam error: send driver fail \n");
		}
	}
SEND_OAM_END:
	return;
}

static int push_oam_internal(struct atm_vcc *atmvcc, void *cell)
{
	struct sk_buff *skb;
	int status=0;

	if(!koam_upid){
		return -1;
	}

	skb = alloc_skb(KOAM_CELL_SIZE, GFP_ATOMIC);

	skb_put(skb, KOAM_CELL_SIZE);
	memcpy(skb->data, (u8 *)cell, KOAM_CELL_SIZE);

	status = netlink_unicast(koam_sock, skb, koam_upid, MSG_DONTWAIT);
	if (status < 0){
		printk(KERN_ERR " koam send fail \n");
		return -1;
	}
	return 0;
}

extern int (*push_oam_pfn)(struct atm_vcc *atmvcc, void *cell);

static __init int koam_init(void)
{
	struct netlink_kernel_cfg cfg = {
		.groups         = 0, 
		.input          = koam_rcv_skb,
		.cb_mutex       = NULL,
		.flags          = NL_CFG_F_NONROOT_RECV,
	};
	koam_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, &cfg);
	if (!koam_sock) {
		return -1;
	}
	push_oam_pfn = push_oam_internal;
	printk("KOAM is loaded successfully.\n");
	return 0;
}

static __exit void koam_exit(void)
{
	sock_release(koam_sock->sk_socket);
	push_oam_pfn = NULL;
	return;
}

static int oam_failure(void *cell, int cmdId)
{
	struct koam_cell *pCell;
	struct atm_cell_header *cell_header;
		
	pCell = (struct koam_cell *)cell;
	cell_header = (struct atm_cell_header*)pCell->cell;

	/* construct command failure */
	pCell->cell[OAM_TYPE_FN] = 0x18; /* oam cell */
	pCell->cell[1+OAM_TYPE_FN] = (u32)(cell_header->bit.vpi);  /* vpi*/
	pCell->cell[2+OAM_TYPE_FN] = (u32)(cell_header->bit.vci); /* vci */
	pCell->cell[5+OAM_TYPE_FN] = cmdId; /* command ID 7 or 8 */
	pCell->cell[7+OAM_TYPE_FN] = (u32)(cell_header->bit.pti); /* PTI value */

	cell_header->bit.vci = 0; /* 0 for command */

	if (push_oam_internal(NULL, (void *)pCell)) {
		printk(KERN_ERR "oam error: could not send failure\n");
		return -1;
	}
	return 0;
}

module_init(koam_init);
module_exit(koam_exit);
MODULE_LICENSE("GPL");
