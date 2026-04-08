#ifndef __MHI_DESC_INCLUDED_H
#define __MHI_DESC_INCLUDED_H

//#include "mtlkpack.h"
//#define __MTLK_PACKED
#define BYTE_OFFSET_MASK   0x3
#define PACKET_DESC_SIZE  36



typedef struct _HOST_DSC
{
    uint32    u32frameInfo0;
    uint32    u32frameInfo1;
    uint32    u32HostPayloadAddr;
    uint32    u32frameInfo3;
    uint32    u32BdIndex;
} __MTLK_PACKED HOST_DSC;










#endif // __MHI_DESC_INCLUDED_H


