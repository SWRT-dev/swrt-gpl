#ifndef __MT_CHIPREG_H__
#define __MT_CHIPREG_H__

extern MT_UINT32 MT_RdReg(MT_UINT32 unit, MT_UINT32 reg);
extern void MT_WrReg(MT_UINT32 unit, MT_UINT32 reg, MT_UINT32 data);
extern void MT_WrRegMask(MT_UINT32 unit, MT_UINT32 reg, MT_UINT32 mask,MT_UINT32 data);


#endif /* __MT_CHIPREG_H__ */
