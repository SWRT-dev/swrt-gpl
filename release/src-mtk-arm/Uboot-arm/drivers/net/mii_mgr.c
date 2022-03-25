#include <common.h>
#include <command.h>
#include "eth.h"

#define outw(address, value)    *((volatile uint32_t *)(address)) = cpu_to_le32(value)
#define inw(address)            le32_to_cpu(*(volatile u32 *)(address))

#define PHY_CONTROL_0 		0x0004   
#define PHY_CONTROL_1 		0x0000   
#define MDIO_PHY_CONTROL_0	(RALINK_ETH_SW_BASE + PHY_CONTROL_0)
#define MDIO_PHY_CONTROL_1 	(RALINK_ETH_SW_BASE + PHY_CONTROL_1)
#define enable_mdio(x)
#define GPIO_MDIO_BIT		(1<<12)
#define GPIO_PURPOSE_SELECT	0x60
#define GPIO_PRUPOSE		(RALINK_SYSCTL_BASE + GPIO_PURPOSE_SELECT)

#define CFG_HZ              100
#define MAX_PHY_RW_WAIT_CNT (100000)


u32 __mii_mgr_read(u32 phy_addr, u32 phy_register, u32 *read_data)
{
	u32 volatile  			status	= 0;
	u32 volatile  			data 	= 0;
	u32			  	rc		= 0;
	unsigned long volatile  t_start = get_timer(0);
	
	//printf("\n MDIO Read operation!!\n");
	// make sure previous read operation is complete
	while(1)
	{
		// 0 : Read/write operation complet
		if(!( inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) 
		{
			break;
		}else if(get_timer(t_start) > (5 * CFG_HZ)){
			printf("\n MDIO Read operation is ongoing !!\n");
			return rc;
		}
	}
	
	data  = (0x01 << 16) | (0x02 << 18) | (phy_addr << 20) | (phy_register << 25);
	outw(MDIO_PHY_CONTROL_0, data);
	data |= (1<<31);
	outw(MDIO_PHY_CONTROL_0, data);
	//printf("\n Set Command [0x%08X] to PHY 0x%8x!!\n", data, MDIO_PHY_CONTROL_0);

	
	// make sure read operation is complete
	t_start = get_timer(0);
	while(1)
	{
		if(!( inw(MDIO_PHY_CONTROL_0) & (0x1 << 31)))
		{
			status = inw(MDIO_PHY_CONTROL_0);
			*read_data = (u32)(status & 0x0000FFFF);
			//printf("\n MDIO_PHY_CONTROL_0: 0x%8x!!\n", status);

			return 1;
		}
		else if(get_timer(t_start) > (5 * CFG_HZ))
		{
			printf("\n MDIO Read operation is ongoing and Time Out!!\n");
			return 0;
		}
	}
}

u32 __mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data)
{
	unsigned long volatile  t_start=get_timer(0);
	u32 volatile  data;

	// make sure previous write operation is complete
	while(1)
	{
		if (!( inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) 
		{
			break;
		}
		else if(get_timer(t_start) > (5 * CFG_HZ))
		{
			printf("\n MDIO Write operation is ongoing !!\n");
			return 0;
		}
	}
	udelay(1);//make sequencial write more robust

	data = (0x01 << 16) | (1<<18) | (phy_addr << 20) | (phy_register << 25) | write_data;
	outw(MDIO_PHY_CONTROL_0, data);
	data |= (1<<31);
	outw(MDIO_PHY_CONTROL_0, data); //start operation
	//printf("\n Set Command [0x%08X] to PHY 0x%8x!!\n", data, MDIO_PHY_CONTROL_0);

	t_start = get_timer(0);

	// make sure write operation is complete
	while(1)
	{
		if(!( inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) // 0 : Read/write operation complete
		{
			return 1;
		}
		else if(get_timer(t_start) > (5 * CFG_HZ))
		{
			printf("\n MDIO Write operation is ongoing and Time Out!!\n");
			return 0;
		}
	}
}

u32 mii_mgr_read(u32 phy_addr, u32 phy_register, u32 *read_data)
{
	u32 low_word;
	u32 high_word;

	if(phy_addr==31) {
		//phase1: write page address phase
		if(__mii_mgr_write(phy_addr, 0x1f, ((phy_register >> 6) & 0x3FF))) {
			//phase2: write address & read low word phase
			if(__mii_mgr_read(phy_addr, (phy_register >> 2) & 0xF, &low_word)) {
				//phase3: write address & read high word phase
				if(__mii_mgr_read(phy_addr, (0x1 << 4), &high_word)) {
					*read_data = (high_word << 16) | (low_word & 0xFFFF);
				//	printf("%s: phy_addr=%x phy_register=%x *read_data=%x\n", __FUNCTION__, phy_addr, phy_register, *read_data);
					return 1;
				}
			}
		}
	} else {
		if(__mii_mgr_read(phy_addr, phy_register, read_data)) {
//			printf("%s: phy_addr=%x phy_register=%x *read_data=%x\n",__FUNCTION__, phy_addr, phy_register, *read_data);
			return 1;
		}
	}

	return 0;
}

u32 mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data)
{
//	printf("%s: phy_addr=%x phy_register=%x write_data=%x\n", __FUNCTION__, phy_addr, phy_register, write_data);
	if(phy_addr == 31) {
		//phase1: write page address phase
		if(__mii_mgr_write(phy_addr, 0x1f, (phy_register >> 6) & 0x3FF)) {
			//phase2: write address & read low word phase
			if(__mii_mgr_write(phy_addr, ((phy_register >> 2) & 0xF), write_data & 0xFFFF)) {
				//phase3: write address & read high word phase
				if(__mii_mgr_write(phy_addr, (0x1 << 4), write_data >> 16)) {
					return 1;
				}
			}
		}
	} else { 
		if(__mii_mgr_write(phy_addr, phy_register, write_data)) {
			return 1;
		}
	}

	return 0;
}

u32 mii_mgr_cl45_set_address(u32 port_num, u32 dev_addr, u32 reg_addr)
{
	u32 rc = 0;
	u32 data = 0;
	u32 count;

	count = 0;
	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			break;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			printf("\n MDIO Read CL45 operation is ongoing !!\n");
			return rc;
		}
		count++;
	}
	data =
	    (dev_addr << 25) | (port_num << 20) | (0x00 << 18) | (0x00 << 16) |
	    reg_addr;
	outw(MDIO_PHY_CONTROL_0, data);
	outw(MDIO_PHY_CONTROL_0, (data | (1 << 31)));

	count = 0;
	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			return 1;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			printf("\n MDIO Write CL45 operation Time Out\n");
			return 0;
		}
		count ++;
	}
}

u32 mii_mgr_write_cl45(u32 port_num, u32 dev_addr, u32 reg_addr, u32 write_data)
{
	u32 count;
	u32 data = 0;

	/* set address first */
	mii_mgr_cl45_set_address(port_num, dev_addr, reg_addr);
	/* udelay(10); */
	count = 0;
	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			break;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			printf("\n MDIO CL45 Write operation ongoing\n");
			return 0;
		}
		count++;
	}

	data =
	    (dev_addr << 25) | (port_num << 20) | (0x01 << 18) | (0x00 << 16) |
	    write_data;
	outw(MDIO_PHY_CONTROL_0, data);
	outw(MDIO_PHY_CONTROL_0, (data | (1 << 31)));
	count = 0;
	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			return 1;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			printf("\n MDIO CL45 Write operation Time Out\n");
			return 0;
		}
		count++;
	}

}

u32 mii_mgr_read_cl45(u32 port_num, u32 dev_addr, u32 reg_addr, u32 *read_data)
{
	u32 rc = 0;
	u32 data = 0;
	u32 count;

	/* set address first */
	mii_mgr_cl45_set_address(port_num, dev_addr, reg_addr);
	/* udelay(10); */


	count = 0;
	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			break;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			rc = 0;
			printf("\n MDIO Read operation is ongoing !!\n");
			return rc;
		}
		count++;
	}
	data =
	    (dev_addr << 25) | (port_num << 20) | (0x03 << 18) | (0x00 << 16) |
	    reg_addr;
	outw(MDIO_PHY_CONTROL_0, data);
	outw(MDIO_PHY_CONTROL_0, (data | (1 << 31)));
	
	count = 0;

	while (1) {
		if (!(inw(MDIO_PHY_CONTROL_0) & (0x1 << 31))) {
			*read_data =
			    (inw(MDIO_PHY_CONTROL_0) & 0x0000FFFF);
			rc = 1;
			return rc;
		} else if (count > MAX_PHY_RW_WAIT_CNT) {
			rc = 0;
			printf
			    ("\n MDIO Read operation Time Out!!\n");
			return rc;
		}
		count++;
	}
	return rc;
}

void dump_phy_reg(int port_no, int from, int to, int is_local, int page_no)
{

	u32 i=0;
	u32 temp=0;
	u32 r31=0;


	if(is_local==0) {

	    printf("\n\nGlobal Register Page %d\n",page_no);
	    printf("===============");
	    r31 = 0 << 15; //global
	    r31 = page_no&0x7 << 12; //page no
	    mii_mgr_write(0, 31, r31); //select global page x
	    for(i=16;i<32;i++) {
		if(i%8==0) {
		    printf("\n");
		}
		mii_mgr_read(port_no,i, &temp);
		printf("%02d: %04X ",i, temp);
	    }
	}else {
	    printf("\n\nLocal Register Port %d Page %d\n",port_no, page_no);
	    printf("===============");
	    r31 = 1 << 15; //local
	    r31 = page_no&0x7 << 12; //page no
	    mii_mgr_write(0, 31, r31); //select local page x
	    for(i=16;i<32;i++) {
		if(i%8==0) {
		    printf("\n");
		}
		mii_mgr_read(port_no,i, &temp);
		printf("%02d: %04X ",i, temp);
	    }
	}
	printf("\n");
}

int rt2880_mdio_access(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 addr;
	u32 phy_addr;
	u32 value = 0,bit_offset,temp;
	u32 i=0, j=0;

	if(!memcmp(argv[0],"mdio.anoff",sizeof("mdio.anoff")))
	{
		value = inw(MDIO_PHY_CONTROL_1);
		value |= (1<<15);
		outw(MDIO_PHY_CONTROL_1,value);
		puts("\n GMAC1 Force link status enable !! \n");
	}
	else if(!memcmp(argv[0],"mdio.anon",sizeof("mdio.anon")))
	{
		value = inw(MDIO_PHY_CONTROL_1);
		value &= ~(1 << 15);
		outw(MDIO_PHY_CONTROL_1,value);
		puts("\n GMAC1 Force link status disable !! \n");
	}
	else if(!memcmp(argv[0],"mdio.r",sizeof("mdio.r")))
	{
		if (argc != 3) {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
	    	}
		phy_addr = simple_strtoul(argv[1], NULL, 10);
#if defined (MAC_TO_MT7530_MODE)
		if(phy_addr == 31) {
			addr = simple_strtoul(argv[2], NULL, 16);
		} else {
			addr = simple_strtoul(argv[2], NULL, 10);
		}
#else
		addr = simple_strtoul(argv[2], NULL, 10);
#endif
		phy_addr &=0x1f;

		if(mii_mgr_read(phy_addr, addr, &value))
			printf("\n mdio.r addr[0x%08X]=0x%08X\n",addr,value);
		else {
			printf("\n Read addr[0x%08X] is Fail!!\n",addr);
		}
	}
	else if(!memcmp(argv[0],"mdio.w",sizeof("mdio.w")))
	{
		if (argc != 4) {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
	    	}
		phy_addr = simple_strtoul(argv[1], NULL, 10);
#if defined (MAC_TO_MT7530_MODE)
		if(phy_addr == 31) {
			addr = simple_strtoul(argv[2], NULL, 16);
		} else {
			addr = simple_strtoul(argv[2], NULL, 10);
		}
#else
		addr = simple_strtoul(argv[2], NULL, 10);
#endif
		value = simple_strtoul(argv[3], NULL, 16);
		phy_addr &=0x1f;

		if(mii_mgr_write(phy_addr, addr,value)) {
			printf("\n mdio.w addr[0x%08X]  value[0x%08X]\n",addr,value);
		}
		else {
			printf("\n Write[0x%08X] is Fail!!\n",addr);
		}
	}
	else if(!memcmp(argv[0],"mdio.wb",sizeof("mdio.wb")))
	{
		if (argc != 4) {
			printf ("Usage:\n%s\n", cmdtp->usage);
			return 1;
		}
		addr = simple_strtoul(argv[1], NULL, 10);
		bit_offset = simple_strtoul(argv[2], NULL, 10);
		value = simple_strtoul(argv[3], NULL, 10);

		if(mii_mgr_read(31, addr,&temp)) {
		    
		}
		else {
			printf("\n Rasd PHY fail while mdio.wb was called\n");
			return 1;
		}

		if(value) {
			printf("\n Set bit[%d] to '1' \n",bit_offset);
			temp |= (1<<bit_offset);
		}
		else {
			printf("\n Set bit[%d] to '0' \n",bit_offset);
			temp &= ~(1<<bit_offset);
		}

		if(mii_mgr_write(31, addr,temp)) {
			printf("\n mdio.wb addr[0x%08X]  value[0x%08X]\n",addr,temp);
		}
		else {
			printf("\n Write[0x%08X] is Fail!!\n",addr);
		}
	}
	else if(!memcmp(argv[0],"mdio.d",sizeof("mdio.d")))
	{
		if (argc == 2) {
		    addr = simple_strtoul(argv[1], NULL, 10);

		    for(i=0;i<0x100;i+=4) {
			    if(i%16==0) {
				    printf("\n%04X: ",0x4000 + addr*0x100 + i);
			    }
			    mii_mgr_read(31, 0x4000 + addr*0x100 + i, &temp);
			    printf("%08X ", temp);
		    }
		    printf("\n");
		} else {
		    for(i=0;i<7;i++) {
			    for(j=0;j<0x100;j+=4) {
				    if(j%16==0) {
					    printf("\n%04X: ",0x4000 + i*0x100 + j);
				    }
				    mii_mgr_read(31, 0x4000 + i*0x100 + j, &temp);
				    printf("%08X ", temp);
			    }
			    printf("\n");
		    }
		}
	}
	return 0;
}

U_BOOT_CMD(
 	mdio,	4,	1,	rt2880_mdio_access,
 	"mdio   - Mediatek PHY register R/W command !!\n",
 	"mdio.r [phy_addr(dec)] [reg_addr(dec)] \n"
 	"mdio.w [phy_addr(dec)] [reg_addr(dec)] [data(HEX)] \n"
 	"mdio.anoff GMAC1 Force link status enable !!  \n"
 	"mdio.anon GMAC1 Force link status disable !!  \n"
 	"mdio.wb [phy register(dec)] [bit offset(Dec)] [Value(0/1)]  \n"
 	"mdio.d - dump all Phy registers \n"
 	"mdio.d [phy register(dec)] - dump Phy registers \n"
);

