/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*****************************************************************************
* Copyright (c) 2008-2013 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

#include "device_mgmt.h"            // API to implement
#include "device_rw.h"              // API to implement

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_device_lkm.h"

// Driver Framework Device API
#include "device_swap.h"            // Device_SwapEndian32

// Logging API
#undef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX  HWPAL_LOG_SEVERITY
#include "log.h"                    // LOG_*

// Driver Framework C Run-Time Library API
#include "clib.h"                   // memcmp

// Driver Framework Basic Definitions API
#include "basic_defs.h"             // uint32_t, NULL, inline, bool,
                                    // IDENTIFIER_NOT_USED

// Linux Kernel API
#include <asm/io.h>                 // ioread32, iowrite32
#include <asm/barrier.h>
#include <linux/version.h>          // LINUX_VERSION_CODE, KERNEL_VERSION
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>

#include <asm/io.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <lantiq_soc.h>

#include "ltq_crypto_core.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define HWPAL_FLAG_READ     BIT_0   // 1
#define HWPAL_FLAG_WRITE    BIT_1   // 2
#define HWPAL_FLAG_SWAP     BIT_2   // 4
#define HWPAL_FLAG_HA       BIT_5   // 32

// Device administration structure
typedef struct
{
#ifdef HWPAL_DEVICE_MAGIC
    // Magic value for detecting valid handles
    unsigned int ValidHandle;
#endif

    // Name string used in Device_Find
    const char * DeviceName_p;

    // device offset range inside PCI device
    unsigned int StartByteOffset;
    unsigned int LastByteOffset;

    // Trace Read, Write flags,
    // Enable byte swap by the host processor flag,
    char Flags;
} HWPAL_Device_Administration_t;

// the c_device_lkm.h file defines a HWPAL_DEVICES that
// depends on the following HWPAL_DEVICE_ADD
#ifdef HWPAL_DEVICE_MAGIC
#define HWPAL_DEVICE_ADD(_name, _devrn, _start, _last, _flags) \
        { HWPAL_DEVICE_MAGIC, _name, _start, _last, _flags }
#else
#define HWPAL_DEVICE_ADD(_name, _devrn, _start, _last, _flags) \
        { _name, _start, _last, _flags }
#endif

// the c_device_lkm.h file defines a HWPAL_REMAP_ADDRESSES that
// depends on the following HWPAL_REMAP_ONE
#define HWPAL_REMAP_ONE(_old, _new) \
    case _old: \
        DeviceByteOffset = _new; \
        break;

// number of devices supported calculated on HWPAL_DEVICES defined
// in c_device_lkm.h
#define DEVICE_COUNT \
        (sizeof(HWPAL_Devices) \
         / sizeof(HWPAL_Device_Administration_t))

// checks that byte offset is in range
#define IS_INVALID_OFFSET(_ofs, _devp) \
    (((_devp)->StartByteOffset + (_ofs) > (_devp)->LastByteOffset) || \
     (((_ofs) & 3) != 0))

#ifdef HWPAL_DEVICE_MAGIC
// checks that device handle is valid
#define IS_INVALID_DEVICE(_devp) \
    ((_devp) < HWPAL_Devices || \
     (_devp) >= HWPAL_Devices + DEVICE_COUNT || \
     (_devp)->ValidHandle != HWPAL_DEVICE_MAGIC)
#endif /* HWPAL_DEVICE_MAGIC */

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif /* KERNEL_VERSION */


/* membase top:  0x1E100000 */
/* membase top2: 0x1E10F800 */
extern void __iomem *ltq_crypto_membase;
spinlock_t ltq_dev_lock;

/*----------------------------------------------------------------------------
 * Local variables
 */

static const HWPAL_Device_Administration_t HWPAL_Devices[] =
{
    HWPAL_DEVICES
};

/*----------------------------------------------------------------------------
 * HWPAL_Hexdump
 *
 * This function hex-dumps an array of uint32_t.
 */
#if ((defined(HWPAL_TRACE_DEVICE_READ)) || (defined(HWPAL_TRACE_DEVICE_WRITE)))
static void
HWPAL_Hexdump(
        const char * ArrayName_p,
        const char * DeviceName_p,
        const unsigned int ByteOffset,
        const uint32_t * WordArray_p,
        const unsigned int WordCount,
        bool fSwapEndianness)
{
    unsigned int i;

    Log_FormattedMessage(
        "%s: "
        "byte offsets 0x%x - 0x%x"
        " (%s)\n"
        "  ",
        ArrayName_p,
        ByteOffset,
        ByteOffset + WordCount*4 -1,
        DeviceName_p);

    for (i = 1; i <= WordCount; i++)
    {
        uint32_t Value = WordArray_p[i - 1];

        if (fSwapEndianness)
            Value = Device_SwapEndian32(Value);

        Log_FormattedMessage(" 0x%08x", Value);

        if ((i & 7) == 0)
            Log_Message("\n  ");
    }

    if ((WordCount & 7) != 0)
        Log_Message("\n");
}
#endif

/*----------------------------------------------------------------------------
 * Device_RemapDeviceAddress
 *
 * This function remaps certain device addresses (relative within the whole
 * device address map) to other addresses. This is needed when the integration
 * has remapped some EIP device registers to other addresses. The EIP Driver
 * Libraries assume the devices always have the same internal layout.
 */
static inline unsigned int
Device_RemapDeviceAddress(
        unsigned int DeviceByteOffset)
{
#ifdef HWPAL_REMAP_ADDRESSES
    switch(DeviceByteOffset)
    {
        // include the remap statements
        HWPAL_REMAP_ADDRESSES

        default:
            break;
    }
#endif

    return DeviceByteOffset;
}


/*------------------------------------------------------------------------------
 * device_mgmt API
 *
 * These functions support finding a device given its name.
 * A handle is returned that is needed in the device_rw API
 * to read or write the device
 */


/*------------------------------------------------------------------------------
 * Device_Initialize
 */
int
Device_Initialize(
        void * CustomInitData_p)
{
	spin_lock_init(&ltq_dev_lock);

    return true;
}


/*------------------------------------------------------------------------------
 * Device_UnInitialize
 */
void
Device_UnInitialize(void)
{
	return;
}


/*-----------------------------------------------------------------------------
 * Device_Find
 */
Device_Handle_t
Device_Find(
        const char * DeviceName_p)
{
    int i;
    unsigned int NameLen;

    if (DeviceName_p == NULL)
    {
        // not supported, thus not found
        return NULL;
    }

    // count the device name length, including the terminating zero
    NameLen = 0;
    while (DeviceName_p[NameLen++])
    {
        if (NameLen == HWPAL_MAX_DEVICE_NAME_LENGTH)
        {
            break;
        }
    }

    // walk through the defined devices and compare the name
    for (i = 0; i < DEVICE_COUNT; i++)
    {
        if (memcmp(
                DeviceName_p,
                HWPAL_Devices[i].DeviceName_p,
                NameLen) == 0)
        {
            // Return the device handle
            return (Device_Handle_t)(HWPAL_Devices + i);
        }
    }

    LOG_WARN("Device_Find: Could not find device '%s'", DeviceName_p);

    return NULL;
}


/*------------------------------------------------------------------------------
 * Device_GetReference
 */
Device_Reference_t
Device_GetReference(
        const Device_Handle_t Device)
{
    Device_Reference_t DevReference = {0};

    // There exists only one reference for this implementation
    IDENTIFIER_NOT_USED(Device);

    return DevReference;
}


/*------------------------------------------------------------------------------
 * device_rw API
 *
 * These functions can be used to transfer a single 32bit word or an array of
 * 32bit words to or from a device.
 * Endianess swapping is performed on the fly based on the configuration for
 * this device.
 *
 */

/*------------------------------------------------------------------------------
 * Device_Read32
 */
uint32_t
Device_Read32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset)
{
    HWPAL_Device_Administration_t * Device_p;
    uint32_t Value;
	unsigned long flag;

    Device_p = (HWPAL_Device_Administration_t *)Device;
    if (Device_p == NULL)
        return 0xEEEEEEEE;

#ifdef HWPAL_DEVICE_MAGIC
    if (IS_INVALID_DEVICE(Device_p))
    {
        LOG_WARN(
                "Device_Read32: "
                "Invalid device handle provided.\n");

        return 0xEEEEEEEE;
    }
#endif /* HWPAL_DEVICE_MAGIC */

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (IS_INVALID_OFFSET(ByteOffset, Device_p))
    {
        LOG_WARN(
                "Device_Read32: "
                "Invalid ByteOffset 0x%x (device %s)\n",
                ByteOffset,
                Device_p->DeviceName_p);

        return 0xEEEEEEEE;
    }
#endif /* HWPAL_STRICT_ARGS_CHECK */

#ifdef HWPAL_ENABLE_HA_SIMULATION
    if (Device_p->Flags & HWPAL_FLAG_HA)
    {
        // HA simulation mode
        // disable access to PKA_MASTER_SEQ_CTRL
        if (ByteOffset == 0x3FC8)
        {
            Value = 0;
            goto HA_SKIP;
        }
    }
#endif

	spin_lock_irqsave(&ltq_dev_lock, flag);

#ifdef HWPAL_DEVICE_DIRECT_MEMIO
	Value = ltq_crypto_r32(Device_p->StartByteOffset + ByteOffset);
#else
	Value = ioread32be(ltq_crypto_membase + Device_p->StartByteOffset + ByteOffset);
#endif
	spin_unlock_irqrestore(&ltq_dev_lock, flag);

#ifdef HWPAL_TRACE_DEVICE_READ
    if (Device_p->Flags & HWPAL_FLAG_READ)
    {
        unsigned int DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;
        {
            Log_FormattedMessage(
                    "Device_Read32: "
                    "0x%x = 0x%08x (%s)\n",
                    ByteOffset,
                    (unsigned int)Value,
                    Device_p->DeviceName_p);
        }
    }
#endif /* HWPAL_TRACE_DEVICE_READ */

    return Value;
}


/*------------------------------------------------------------------------------
 * Device_Write32
 */
void
Device_Write32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset,
        const uint32_t ValueIn)
{
    HWPAL_Device_Administration_t * Device_p;
    uint32_t Value = ValueIn;
	unsigned int DeviceByteOffset;
	unsigned long flag;

    Device_p = (HWPAL_Device_Administration_t *)Device;
    if (Device_p == NULL)
        return;

#ifdef HWPAL_DEVICE_MAGIC
    if (IS_INVALID_DEVICE(Device_p))
    {
        LOG_WARN(
                "Device_Write32 :"
                "Invalid device handle provided.\n");

        return;
    }
#endif /* HWPAL_DEVICE_MAGIC */

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (IS_INVALID_OFFSET(ByteOffset, Device_p))
    {
        LOG_WARN(
                "Device_Write32: "
                "Invalid ByteOffset 0x%x (device %s)\n",
                ByteOffset,
                Device_p->DeviceName_p);
        return;
    }
#endif /* HWPAL_STRICT_ARGS_CHECK */

#ifdef HWPAL_TRACE_DEVICE_WRITE
    if (Device_p->Flags & HWPAL_FLAG_WRITE)
    {
        Log_FormattedMessage(
                "Device_Write32: "
                "0x%x = 0x%08x (%s)\n",
                ByteOffset,
                (unsigned int)Value,
                Device_p->DeviceName_p);
    }
#endif /* HWPAL_TRACE_DEVICE_WRITE*/

#ifdef HWPAL_DEVICE_ENABLE_SWAP
	if (Device_p->Flags & HWPAL_FLAG_SWAP)
    	Value = Device_SwapEndian32(Value);
#endif

	DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;

	spin_lock_irqsave(&ltq_dev_lock, flag);
#ifdef HWPAL_DEVICE_DIRECT_MEMIO
	ltq_crypto_wr32(Value, DeviceByteOffset);
#else
	iowrite32be(Value, ltq_crypto_membase + DeviceByteOffset);
#endif
	spin_unlock_irqrestore(&ltq_dev_lock, flag);

	smp_wmb();
}


/*------------------------------------------------------------------------------
 * Device_Read32Array
 *
 * Not supported for PCI Configuration space!
 */
void
Device_Read32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // read starts here, +4 increments
        uint32_t * MemoryDst_p,         // writing starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;
	unsigned long flag;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    if (Device_p == NULL ||
        MemoryDst_p == NULL ||
        Count <= 0)
    {
        return;
    }

    if (IS_INVALID_OFFSET(Offset, Device_p))
    {
        LOG_WARN("Device_Read32Array: "
               "Invalid ByteOffset 0x%x (device %s)\n",
               Offset,
               Device_p->DeviceName_p);
        return;
    }


    DeviceByteOffset = Device_p->StartByteOffset + Offset;

    {
        uint32_t Value;
        int i;

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        bool fSwap = false;
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif
        for (i = 0; i < Count; i++)
        {
			spin_lock_irqsave(&ltq_dev_lock, flag);
#ifdef HWPAL_DEVICE_DIRECT_MEMIO
			Value = ltq_crypto_r32(DeviceByteOffset);
#else
			Value = ioread32be(ltq_crypto_membase + DeviceByteOffset);
#endif
			spin_unlock_irqrestore(&ltq_dev_lock, flag);
            smp_rmb();

#ifdef HWPAL_DEVICE_ENABLE_SWAP
            // swap endianness if required
            if (fSwap)
                Value = Device_SwapEndian32(Value);
#endif

            MemoryDst_p[i] = Value;
            DeviceByteOffset +=  4;
        } // for
    }

#ifdef HWPAL_TRACE_DEVICE_READ
    if (Device_p->Flags & HWPAL_FLAG_READ)
    {
        HWPAL_Hexdump(
            "Device_Read32Array",
            Device_p->DeviceName_p,
            Device_p->StartByteOffset + Offset,
            MemoryDst_p,
            Count,
            false);     // already swapped during read above
    }
#endif /* HWPAL_TRACE_DEVICE_READ */
}


/*----------------------------------------------------------------------------
 * Device_Write32Array
 *
 * Not supported for PCI Configuration space!
 */
void
Device_Write32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // write starts here, +4 increments
        const uint32_t * MemorySrc_p,   // reading starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;
	unsigned long flag;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    if (Device_p == NULL ||
        MemorySrc_p == NULL ||
        Count <= 0)
    {
        return;     // ## RETURN ##
    }

    if (IS_INVALID_OFFSET(Offset, Device_p))
    {
        LOG_WARN(
            "Device_Write32Array: "
            "Invalid ByteOffset 0x%x (device %s)\n",
            Offset,
            Device_p->DeviceName_p);
        return;
    }

    DeviceByteOffset = Device_p->StartByteOffset + Offset;

#ifdef HWPAL_TRACE_DEVICE_WRITE
    if (Device_p->Flags & HWPAL_FLAG_WRITE)
    {
        bool fSwap = false;
#ifdef HWPAL_DEVICE_ENABLE_SWAP
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif

        HWPAL_Hexdump(
            "Device_Write32Array",
            Device_p->DeviceName_p,
            DeviceByteOffset,
            MemorySrc_p,
            Count,
            fSwap);
    }
#endif /* HWPAL_TRACE_DEVICE_WRITE */

    {
        uint32_t Value;
        int i;

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        bool fSwap = false;
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif

        for (i = 0; i < Count; i++)
        {
            Value = MemorySrc_p[i];
#ifdef HWPAL_DEVICE_ENABLE_SWAP
            if (fSwap)
                Value = Device_SwapEndian32(Value);
#endif

			spin_lock_irqsave(&ltq_dev_lock, flag);
#ifdef HWPAL_DEVICE_DIRECT_MEMIO
			ltq_crypto_wr32(Value, DeviceByteOffset);
#else
			iowrite32be(Value, ltq_crypto_membase + DeviceByteOffset);
#endif
			spin_unlock_irqrestore(&ltq_dev_lock, flag);
            smp_wmb();

            DeviceByteOffset += 4;
        } // for
    }
}

/* end of file device_lkm.c */
