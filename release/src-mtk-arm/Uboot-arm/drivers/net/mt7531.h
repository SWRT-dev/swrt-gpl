
#ifndef _MT7531_H_
#define _MT7531_H_

#include <asm/types.h>

struct mt7531_port_cfg {
	int phy_mode;
	u32 enabled: 1;
	u32 force_link: 1;
	u32 speed: 2;
	u32 duplex: 1;
};

/* Values of MAC_SPD_STS */
#define MAC_SPD_10			0
#define MAC_SPD_100			1
#define MAC_SPD_1000			2
#define MAC_SPD_2500			3

void mt7531_sw_init(struct mt7531_port_cfg *p5, struct mt7531_port_cfg *p6);

int mt7531_mii_read(int phy, int reg);
void mt7531_mii_write(int phy, int reg, u16 val);
int mt7531_mmd_read(int phy, int devad, u16 reg);
void mt7531_mmd_write(int phy, int devad, u16 reg, u16 val);
int mt7531_phy_calibration(u8 phyaddr);
#endif /* _MT7531_H_ */
