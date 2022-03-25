#ifndef __MT_CNFG_H__
#define __MT_CNFG_H__

#ifdef MT_GLOBAL
#define MT_EXTERN
#define MT_I(x) x
#else
#define MT_EXTERN extern
#define MT_I(x)
#endif

#define MIPS_INITIAL_CLOCK_MHz 40

#define HW_CREG_U32(I)		*((volatile MT_UINT32*)(I))
#define HW_CREG_U32_NEW(I)	*((volatile MT_UINT32*)(I))

#define HW_CREG_U16(I)   	*((volatile MT_UINT16*)(I))

#define HW_CREG(I)		*((volatile MT_UINT32*)(I))
#define HW_C_U16(I)   	        *((volatile MT_UINT16*)(I))

#define HW_CREG_UNIT(UNIT, REG)  (((MT_UINT32)UNIT) | \
				  (((MT_UINT32)REG) << REG_MIPS_ADD_OFFSET) | \
				   ((MT_UINT32)0x2))

#define NONE_TRUNK_REG(UNIT, REG)	(HW_CREG_U32((MT_UINT32)UNIT | (MT_UINT32)REG ))



#define MT_UBYTE		unsigned char
#define MT_BYTE			char
#define MT_UINT16		unsigned short
#define MT_INT16		short
#define MT_UINT32		unsigned long
#define MT_INT32		long

#endif /* __MT_CNFG_H__ */

