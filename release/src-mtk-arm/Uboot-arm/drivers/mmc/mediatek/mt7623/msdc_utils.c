#include "msdc_types.h"
#include "msdc_utils.h"

#ifndef MSDC_INLINE_UTILS
unsigned int msdc_uffs(unsigned int x)
{
	unsigned int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

unsigned int msdc_ntohl(unsigned int n)
{
    unsigned int t;
    unsigned char *b = (unsigned char*)&t;
    *b++ = ((n >> 24) & 0xFF);
    *b++ = ((n >> 16) & 0xFF);
    *b++ = ((n >> 8) & 0xFF);
    *b   = ((n) & 0xFF);
    return t;
}

void msdc_set_field(volatile u32 *reg, u32 field, u32 val)
{
    u32 tv = (u32)*reg;
    tv &= ~(field);
    tv |= ((val) << (msdc_uffs((unsigned int)field) - 1));
    *reg = tv;
}

void msdc_get_field(volatile u32 *reg, u32 field, u32 *val)
{
    u32 tv = (u32)*reg;
    *val = ((tv & (field)) >> (msdc_uffs((unsigned int)field) - 1));
}
#endif

