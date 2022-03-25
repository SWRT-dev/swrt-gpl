#ifndef __RTK_HAL_H__
#define __RTK_HAL_H__

#define RTK_SW_VID_RANGE        16
extern rtk_api_ret_t rtk_hal_switch_init(void);
extern int rtk_hal_dump_vlan(void);
extern int rtk_hal_dump_table(void);
extern void rtk_hal_dump_mib(void);
extern void rtk_hal_disable_igmpsnoop(void);
#endif
