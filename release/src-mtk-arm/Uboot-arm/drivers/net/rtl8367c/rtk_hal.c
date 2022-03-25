#include  "./include/rtk_switch.h"
#include  "./include/vlan.h"
#include  "./include/port.h"
#include  "./include/rate.h"
#include  "./include/rtk_hal.h"
#include  "./include/l2.h"
#include  "./include/stat.h"
#include  "./include/igmp.h"
#include  "./include/rtl8367c_asicdrv_port.h"
#include  "./include/rtl8367c_asicdrv_mib.h"

extern rtk_int32 smi_read(rtk_uint32 mAddrs, rtk_uint32 *rData);
extern rtk_int32 smi_write(rtk_uint32 mAddrs, rtk_uint32 rData);


rtk_api_ret_t rtk_hal_switch_init(void)
{
    rtk_api_ret_t ret;
    unsigned int regValue;

    ret = rtk_switch_init();
    printf("rtk_switch_init ret = %d!!!!!!!!!!!!\n", ret);
    mdelay(500);

    regValue = 0;
    smi_write(0x13A0, 0x1234);
    mdelay(100);
    smi_read(0x13A0, &regValue);
    printf("rtk_switch reg = 0x%x !!!!!!!!!!!!\n", regValue);

	/*vlan init*/
	ret = rtk_vlan_init();
	printf("rtk_vlan_init ret = %d!!!!!!!!!!!!\n", ret);

    return ret;
}

int rtk_hal_dump_vlan(void)
{
    rtk_vlan_cfg_t vlan;
	int i;

	printf("vid    fid    portmap\n");
	for (i = 0; i < RTK_SW_VID_RANGE; i++) {
		rtk_vlan_get(i, &vlan);
		printf("RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 0): %d\n", RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 0));
		printf("RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 1): %d\n", RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 1));
		printf("RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 2): %d\n", RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 2));
		printf("RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 3): %d\n", RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 3));
		printf("RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 4): %d\n", RTK_PORTMASK_IS_PORT_SET(vlan.mbr, 4));
	}
	return 0;
}



void rtk_dump_unicast_table(void)
{
    rtk_uint32 address;
	rtk_l2_ucastAddr_t l2_data;
	rtk_api_ret_t retVal;

	printf("Unicast Table:\n");
	address = 0;
	printf("Port0:");
	while (1) {
		if ((retVal = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UCSPA, UTP_PORT0, &address, &l2_data)) != RT_ERR_OK) {
			if (!address)
				printf("empty!\n");
			break;
			}
		printf("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);
		address++;
	}
	printf("\n");

    address = 0;
	printf("Port1:");
	while (1) {
		if ((retVal = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UCSPA, UTP_PORT1, &address, &l2_data)) != RT_ERR_OK) {
			if (!address)
				printf("empty!\n");
			break;
			}
		printf("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);
		address++;
	}
	printf("\n");

    address = 0;
	printf("Port2:");
	while (1) {
		if ((retVal = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UCSPA, UTP_PORT2, &address, &l2_data)) != RT_ERR_OK)  {
			if (!address)
				printf("empty!\n");
			break;
			}
		printf("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);
		address++;
	}
	printf("\n");

    address = 0;
	printf("Port3:");
	while (1) {
		if ((retVal = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UCSPA, UTP_PORT3, &address, &l2_data)) != RT_ERR_OK) {
			if (!address)
				printf("empty!\n");
			break;
			}
		printf("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);
		address++;
	}
	printf("\n");

    address = 0;
	printf("Port4:");
	while (1) {
		if ((retVal = rtk_l2_addr_next_get(READMETHOD_NEXT_L2UCSPA, UTP_PORT4, &address, &l2_data)) != RT_ERR_OK) {
			if (!address)
				printf("empty!\n");
			break;
			}
		printf("  %02x%02x%02x%02x%02x%02x", l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2], l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5]);
		address++;
	}
	printf("\n");
	
}

#if 0
void rtk_dump_multicast_table(void)
{
/*TODO*/
}
#endif

int rtk_hal_dump_table(void)
{
    
	rtk_dump_unicast_table();
	//rtk_dump_multicast_table();	
    return 0;
}



void rtk_dump_mib_type(rtk_stat_port_type_t cntr_idx)
{
    rtk_port_t port;
	rtk_stat_counter_t Cntr;

    for (port = 0; port < 3; port++) {
		rtk_stat_port_get(port, cntr_idx, &Cntr);
		printf("%8llu", Cntr);
   	}
    for (port = 16; port < 18; port++) {
        rtk_stat_port_get(port, cntr_idx, &Cntr);
        printf("%8llu", Cntr);
    }
	printf("\n");
}

void rtk_hal_dump_mib(void)
{

	printf("  %8s %8s  %8s %8s %8s\n", "Port0", "Port1", "Port2", "Port3", "Port4");
	/* Get TX Unicast Pkts */
	printf("TX Unicast Pkts  :");
	rtk_dump_mib_type(STAT_IfOutUcastPkts);
	/* Get TX Multicast Pkts */
	printf("TX Multicast Pkts:");
	rtk_dump_mib_type(STAT_IfOutMulticastPkts);
	/* Get TX BroadCast Pkts */
	printf("TX BroadCast Pkts:");
	rtk_dump_mib_type(STAT_IfOutBroadcastPkts);
	/* Get TX Collisions */
	/* Get TX Puase Frames */
	printf("TX Pause Frames  :");
	rtk_dump_mib_type(STAT_Dot3OutPauseFrames);
	/* Get TX Drop Events */
	/* Get RX Unicast Pkts */
	printf("RX Unicast Pkts  :");
	rtk_dump_mib_type(STAT_IfInUcastPkts);
	/* Get RX Multicast Pkts */
	printf("RX Multicast Pkts:");
	rtk_dump_mib_type(STAT_IfInMulticastPkts);
	/* Get RX Broadcast Pkts */
	printf("RX Broadcast Pkts:");
	rtk_dump_mib_type(STAT_IfInBroadcastPkts);
	/* Get RX FCS Erros */
	printf("RX FCS Errors    :");
	rtk_dump_mib_type(STAT_Dot3StatsFCSErrors);
	/* Get RX Undersize Pkts */
	printf("RX Undersize Pkts:");
	rtk_dump_mib_type(STAT_EtherStatsUnderSizePkts);
	/* Get RX Fragments */
	printf("RX Fragments     :");
	rtk_dump_mib_type(STAT_EtherStatsFragments);
	/* Get RX Oversize Pkts */
	printf("RX Oversize Pkts :");
	rtk_dump_mib_type(STAT_EtherOversizeStats);
	/* Get RX Jabbers */
	printf("RX Jabbers       :");
	rtk_dump_mib_type(STAT_EtherStatsJabbers);
	/* Get RX Pause Frames */
	printf("RX Pause Frames  :");
	rtk_dump_mib_type(STAT_Dot3InPauseFrames);
	/* clear MIB */
	rtk_stat_global_reset();
}

void rtk_hal_disable_igmpsnoop(void)
{
    rtk_igmp_state_set(0);
}

