/* adapter_interrupts.c
 *
 * Adapter EIP-202 module responsible for interrupts.
 *
 */

/*****************************************************************************
* Copyright (c) 2008-2014 INSIDE Secure B.V. All Rights Reserved.
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

#include "adapter_interrupts.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default Adapter configuration
#include "c_adapter_eip202.h"      // ADAPTER_*INTERRUPT*

// Driver Framework Basic Definitions API
#include "basic_defs.h"            // bool, IDENTIFIER_NOT_USED

// Driver Framework C-RunTime Library API
#include "clib.h"                  // ZEROINIT

// EIP-201 Advanced interrupt Controller (AIC)
#include "eip201.h"

// Logging API
#include "log.h"

// Driver Framework Device API
#include "device_types.h"          // Device_Handle_t
#include "device_mgmt.h"           // Device_Find, Device_GetReference

// Linux Kernel API
#include <linux/interrupt.h>       // request_irq, free_irq,
                                   // DECLARE_TASKLET, tasklet_schedule,
                                   // IRQ_DISABLED
#include <linux/irq.h>             // IRQ_TYPE_LEVEL_HIGH
#include <linux/irqreturn.h>       // irqreturn_t
#include <linux/delay.h>
#include <linux/platform_device.h>

/* Switch the Handler to only use Ring 0 IRQ when MPE FW is loaded */
#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT 
#define IRQ_OFFSET				1
#define irq_handler_callback	AdapterINT_Ring0
#else
#define	IRQ_OFFSET				0
#define irq_handler_callback	AdapterINT_TopHalfHandler
#endif

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


// EIP-202 interrupt signals at the Global EIP-201 AIC
// assigned values represent interrupt source bit numbers
enum
{
    EIP201_INT_GLOBAL_DFE0          = (1 << ADAPTER_EIP202_PHY_DFE0_IRQ),
    EIP201_INT_GLOBAL_DSE0          = (1 << ADAPTER_EIP202_PHY_DSE0_IRQ),

    EIP201_INT_GLOBAL_RING0         = (1 << ADAPTER_EIP202_PHY_RING0_IRQ),
    EIP201_INT_GLOBAL_RING1         = (1 << ADAPTER_EIP202_PHY_RING1_IRQ),

    EIP201_INT_GLOBAL_PE0           = (1 << ADAPTER_EIP202_PHY_PE0_IRQ),
};


// EIP-202 interrupt signals at the Ring EIP-201 AIC
// assigned values represent interrupt source bit numbers
enum
{
    EIP201_INT_RING_CDR0           = (1 << ADAPTER_EIP202_PHY_CDR0_IRQ),
    EIP201_INT_RING_RDR0           = (1 << ADAPTER_EIP202_PHY_RDR0_IRQ),
    EIP201_INT_RING_CDR1           = (1 << ADAPTER_EIP202_PHY_CDR1_IRQ),
    EIP201_INT_RING_RDR1           = (1 << ADAPTER_EIP202_PHY_RDR1_IRQ),
};

// Define maximum number of supported interrupts
#define ADAPTER_MAX_INTERRUPTS  (IRQ_LAST_LINE)

#define ADAPTERINT_MAKE_TASKLET(_name, _data) \
    DECLARE_TASKLET(AdapterINT_TaskLet_##_name, AdapterINT_CommonTasklet, _data)

#define ADAPTERINT_SCHEDULE_TASKLET(_name) \
    tasklet_schedule(&AdapterINT_TaskLet_##_name)

#define ADAPTERINT_KILL_TASKLET(_name) \
    tasklet_kill(&AdapterINT_TaskLet_##_name)



// configuration of all interrupt sources (polarity)
// "false" parameter ensure interrupts remain disabled  ----vv
#define ADD_IRQ(_irq, _pol) {(_irq), EIP201_CONFIG_##_pol, false}

#define ARRAY_ELEMCOUNT(_a)  (sizeof(_a) / sizeof(_a[0]))

#define ADAPTER_INT_SANITY_CHECK_MAX_WAIT_LOOPS     10000000

#define ADAPTER_INT_SANITY_CHECK_IRQ                IRQ_PE0

#define ADAPTERINT_REQUEST_IRQ_FLAGS    (IRQF_SHARED)


/*----------------------------------------------------------------------------
 * Local variables
 */

static Device_Handle_t Adapter_Device_EIP201_Global;
static Device_Handle_t Adapter_Device_EIP201_Ring0;
static Device_Handle_t Adapter_Device_EIP201_Ring1;

static void AdapterINT_SanityCheckHandler(const int nIRQ);
static void AdapterINT_From_EIP201_InterruptSource(
       						const EIP201_SourceBitmap_t EIP201IntSource,
        					int * const nIRQ);

static int AdapterINT_IRQ         = -1;

static Adapter_InterruptHandler_t AdapterINT_Handlers[ADAPTER_MAX_INTERRUPTS];

static const EIP201_SourceSettings_t
Adapter_InterruptSettings_Global[] =
{
    ADD_IRQ(EIP201_INT_GLOBAL_DFE0,    	 	ACTIVE_LOW),
    ADD_IRQ(EIP201_INT_GLOBAL_DSE0,    		ACTIVE_LOW),
    ADD_IRQ(EIP201_INT_GLOBAL_RING0,        ACTIVE_LOW),
    ADD_IRQ(EIP201_INT_GLOBAL_RING1,        ACTIVE_LOW),
};


static const EIP201_SourceSettings_t
Adapter_InterruptSettings_Ring0[] =
{
    ADD_IRQ(EIP201_INT_RING_CDR0,            ACTIVE_LOW),
    ADD_IRQ(EIP201_INT_RING_RDR0,            ACTIVE_LOW),
};

static const EIP201_SourceSettings_t
Adapter_InterruptSettings_Ring1[] =
{
    ADD_IRQ(EIP201_INT_RING_CDR1,            ACTIVE_LOW),
    ADD_IRQ(EIP201_INT_RING_RDR1,            ACTIVE_LOW),
};

static unsigned int AdapterINT_DFE0_Counter = 0;
static unsigned int AdapterINT_DSE0_Counter = 0;
static unsigned int AdapterINT_PE0_Counter = 0;
static unsigned int AdapterINT_CDR0_Counter = 0;
static unsigned int AdapterINT_RDR0_Counter = 0;
static unsigned int AdapterINT_CDR1_Counter = 0;
static unsigned int AdapterINT_RDR1_Counter = 0;
static volatile bool InterruptHasOccurred = false;


/*----------------------------------------------------------------------------
 * AdapterINT_GetActiveIntNr
 *
 * Returns 0..31 depending on the lowest '1' bit.
 * Returns 32 when all bits are zero
 *
 * Using binary break-down algorithm.
 */
static inline int
AdapterINT_GetActiveIntNr(
        uint32_t Sources)
{
    unsigned int IntNr = 0;
    unsigned int R16, R8, R4, R2;

    if (Sources == 0)
        return 32;

    // if the lower 16 bits are empty, look at the upper 16 bits
    R16 = Sources & 0xFFFF;
    if (R16 == 0)
    {
        IntNr += 16;
        R16 = Sources >> 16;
    }

    // if the lower 8 bits are empty, look at the high 8 bits
    R8 = R16 & 0xFF;
    if (R8 == 0)
    {
        IntNr += 8;
        R8 = R16 >> 8;
    }

    R4 = R8 & 0xF;
    if (R4 == 0)
    {
        IntNr += 4;
        R4 = R8 >> 4;
    }

    R2 = R4 & 3;
    if (R2 == 0)
    {
        IntNr += 2;
        R2 = R4 >> 2;
    }

    // last two bits are trivial
    // 00 => cannot happen
    // 01 => +0
    // 10 => +1
    // 11 => +0
    if (R2 == 2)
        IntNr++;

    return IntNr;
}


/*----------------------------------------------------------------------------
 * AdapterINT_Increment_InterruptCounter
 */
static void
AdapterINT_Increment_InterruptCounter(
        const int nIRQ)
{
    switch(nIRQ)
    {
        case IRQ_DFE0:
            AdapterINT_DFE0_Counter++;
            return;

        case IRQ_DSE0:
            AdapterINT_DSE0_Counter++;
            return;

        case IRQ_PE0:
            AdapterINT_PE0_Counter++;
            return;

        case IRQ_CDR0:
            AdapterINT_CDR0_Counter++;
            return;

        case IRQ_RDR0:
            AdapterINT_RDR0_Counter++;
            return;

        case IRQ_CDR1:
            AdapterINT_CDR1_Counter++;
            return;

        case IRQ_RDR1:
            AdapterINT_RDR1_Counter++;
            return;
#if 0
        case IRQ_CDR2:
            AdapterINT_CDR2_Counter++;
            return;

        case IRQ_RDR2:
            AdapterINT_RDR2_Counter++;
            return;

        case IRQ_CDR3:
            AdapterINT_CDR3_Counter++;
            return;

        case IRQ_RDR3:
            AdapterINT_RDR3_Counter++;
            return;
#endif
        default:
            return;
    }
}


/*----------------------------------------------------------------------------
 * AdapterINT_Report_InterruptCounters
 */
static void
AdapterINT_Report_InterruptCounters(void)
{
#define ADAPTER_INTERRUPT_REPORT_STR \
        "EIP-201 AIC interrupt source %d, adapter IRQ %d, counter %d\n"

    if(EIP201_INT_GLOBAL_DFE0 & ADAPTER_EIP202_INTERRUPTS_TRACEFILTER)
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_DFE0),
             IRQ_DFE0,
             AdapterINT_DFE0_Counter);

    if(EIP201_INT_GLOBAL_DSE0 & ADAPTER_EIP202_INTERRUPTS_TRACEFILTER)
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_DSE0),
             IRQ_DSE0,
             AdapterINT_DSE0_Counter);

    if(EIP201_INT_GLOBAL_PE0 & ADAPTER_EIP202_INTERRUPTS_TRACEFILTER)
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_PE0),
             IRQ_PE0,
             AdapterINT_PE0_Counter);

    if(EIP201_INT_GLOBAL_RING0 & ADAPTER_EIP202_INTERRUPTS_TRACEFILTER)
    {
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_RING0),
             IRQ_CDR0,
             AdapterINT_CDR0_Counter);
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_RING0),
             IRQ_RDR0,
             AdapterINT_RDR0_Counter);

        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_RING0),
             IRQ_CDR1,
             AdapterINT_CDR1_Counter);
        LOG_CRIT(ADAPTER_INTERRUPT_REPORT_STR,
             AdapterINT_GetActiveIntNr(EIP201_INT_GLOBAL_RING0),
             IRQ_RDR1,
             AdapterINT_RDR1_Counter);
    }
}


/*----------------------------------------------------------------------------
 * AdapterINT_To_EIP201_InterruptSource
 */
static void
AdapterINT_To_EIP201_InterruptSource(
        const int nIRQ,
        EIP201_SourceBitmap_t * EIP201IntSource)
{
    switch(nIRQ)
    {
        case IRQ_DFE0:
            *EIP201IntSource = EIP201_INT_GLOBAL_DFE0;
            return;

        case IRQ_DSE0:
            *EIP201IntSource = EIP201_INT_GLOBAL_DSE0;
            return;

        case IRQ_PE0:
            *EIP201IntSource = EIP201_INT_GLOBAL_PE0;
            return;

        case IRQ_CDR0:
            *EIP201IntSource = EIP201_INT_GLOBAL_RING0;
            return;

        case IRQ_RDR0:
            *EIP201IntSource = EIP201_INT_GLOBAL_RING0;
            return;

        case IRQ_CDR1:
            *EIP201IntSource = EIP201_INT_GLOBAL_RING1;
            return;

        case IRQ_RDR1:
            *EIP201IntSource = EIP201_INT_GLOBAL_RING1;
            return;

        default:
            LOG_INFO(
                "AdapterINT_To_EIP201_InterruptSource: "
                "Cannot translate IRQ number %d to EIP-201 interrupt source\n",
                nIRQ);
            return;
    }
}


/*----------------------------------------------------------------------------
 * AdapterINT_From_EIP201_InterruptSource
 */
static void
AdapterINT_From_EIP201_InterruptSource(
        const EIP201_SourceBitmap_t EIP201IntSource,
        int * const nIRQ)
{
    switch(EIP201IntSource)
    {
        case EIP201_INT_GLOBAL_DFE0:
            *nIRQ = IRQ_DFE0;
            return;

        case EIP201_INT_GLOBAL_DSE0:
            *nIRQ = IRQ_DSE0;
            return;

        case EIP201_INT_GLOBAL_PE0:
            *nIRQ = IRQ_PE0;
            return;

        default:
            LOG_INFO(
                "AdapterINT_From_EIP201_InterruptSource: "
                "Cannot translate EIP-201 interrupt source %d to IRQ number\n",
                EIP201IntSource);
            return;
    }
}


/*-----------------------------------------------------------------------------
 * AdapterINT_SanityCheckHandler
 */
static void
AdapterINT_SanityCheckHandler(
        const int nIRQ)
{
    LOG_INFO("Sanity check interrupt handler called\n");

    if (nIRQ == ADAPTER_INT_SANITY_CHECK_IRQ)
    {
        // Disable IRQ to avoid interrupt re-requesting
        Adapter_Interrupt_Disable(ADAPTER_INT_SANITY_CHECK_IRQ, 0);

        InterruptHasOccurred = true;
    }
}


/*----------------------------------------------------------------------------
 * Adapter_Interrupt_Enable
 */
void
Adapter_Interrupt_Enable(
        const int nIRQ,
        const unsigned int Flags)
{
    IDENTIFIER_NOT_USED(Flags);

    switch(nIRQ)
    {
        case IRQ_DFE0:
        case IRQ_DSE0:
        case IRQ_PE0:
        case IRQ_CDR0:
        case IRQ_RDR0:
        case IRQ_CDR1:
        case IRQ_RDR1:
            {
                EIP201_SourceBitmap_t Sources = 0;

                if(AdapterINT_IRQ == -1)
                {
                    LOG_INFO(
                            "Adapter_Interrupt_Enable: "
                            "Cannot enable IRQ %d "
                            "due to not installed handler\n",
                            nIRQ);
                    return;
                }

                AdapterINT_To_EIP201_InterruptSource(nIRQ,
                                                    &Sources);

                if (nIRQ == IRQ_CDR0)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_CDR0);
                else if (nIRQ == IRQ_RDR0)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_RDR0);

                else if (nIRQ == IRQ_CDR1)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_CDR1);
                else if (nIRQ == IRQ_RDR1)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_RDR1);

                EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Global,
                                               Sources);

                LOG_INFO(
                        "Adapter_Interrupt_Enable: "
                        "Enabled IRQ %d\n",
                        nIRQ);

            }
            break;
        default:
            LOG_INFO(
                    "Adapter_Interrupt_Enable: "
                    "IRQ %d not supported\n",
                    nIRQ);
            return;
    }
}


/*----------------------------------------------------------------------------
 * Adapter_Interrupt_Clear
 */
void
Adapter_Interrupt_Clear(
        const int nIRQ,
        const unsigned int Flags)
{
    IDENTIFIER_NOT_USED(Flags);

    switch(nIRQ)
    {
        case IRQ_DFE0:
        case IRQ_DSE0:
        case IRQ_PE0:
        case IRQ_CDR0:
        case IRQ_RDR0:
        case IRQ_CDR1:
        case IRQ_RDR1:
            {
                EIP201_SourceBitmap_t Sources = 0;

                if(AdapterINT_IRQ == -1)
                {
                    LOG_INFO(
                            "Adapter_Interrupt_Clear: "
                            "Cannot clear IRQ %d "
                            "due to not installed handler\n",
                            nIRQ);
                    return;
                }

                LOG_INFO("Adapter_Interrupt_Clear: Clearing IRQ %d\n", nIRQ);

                AdapterINT_To_EIP201_InterruptSource(nIRQ,
                                                    &Sources);

                 if (nIRQ == IRQ_CDR0)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_CDR0);
                 else if (nIRQ == IRQ_RDR0)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_RDR0);
                 else if (nIRQ == IRQ_CDR1)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_CDR1);
                 else if (nIRQ == IRQ_RDR1)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_RDR1);

                 EIP201_Acknowledge(Adapter_Device_EIP201_Global, Sources);
            }
            break;
        default:
            LOG_INFO(
                    "Adapter_Interrupt_Clear: "
                    "IRQ %d not supported\n",
                    nIRQ);
            return;
    }
}


/*----------------------------------------------------------------------------
 * Adapter_Interrupt_ClearAndEnable
 */
void
Adapter_Interrupt_ClearAndEnable(
        const int nIRQ,
        const unsigned int Flags)
{
    IDENTIFIER_NOT_USED(Flags);

    switch(nIRQ)
    {
        case IRQ_DFE0:
        case IRQ_DSE0:
        case IRQ_PE0:
        case IRQ_CDR0:
        case IRQ_RDR0:
        case IRQ_CDR1:
        case IRQ_RDR1:
            {
                EIP201_SourceBitmap_t Sources = 0;

                if(AdapterINT_IRQ == -1)
                {
                    LOG_INFO(
                            "Adapter_Interrupt_ClearAndEnable: "
                            "Cannot clear and enable IRQ %d "
                            "due to not installed handler\n",
                            nIRQ);
                    return;
                }


                AdapterINT_To_EIP201_InterruptSource(nIRQ, &Sources);

                if (nIRQ == IRQ_CDR0)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring0,
                                       EIP201_INT_RING_CDR0);
                else if (nIRQ == IRQ_RDR0)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring0,
                                       EIP201_INT_RING_RDR0);

                else if (nIRQ == IRQ_CDR1)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring1,
                                       EIP201_INT_RING_CDR1);
                else if (nIRQ == IRQ_RDR1)
                    EIP201_Acknowledge(Adapter_Device_EIP201_Ring1,
                                       EIP201_INT_RING_RDR1);

                EIP201_Acknowledge(Adapter_Device_EIP201_Global, Sources);

                if (nIRQ == IRQ_CDR0)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_CDR0);
                else if (nIRQ == IRQ_RDR0)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_RDR0);

                else if (nIRQ == IRQ_CDR1)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_CDR1);
                else if (nIRQ == IRQ_RDR1)
                    EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_RDR1);

                EIP201_SourceMask_EnableSource(Adapter_Device_EIP201_Global,
                                               Sources);

                LOG_INFO(
                        "Adapter_Interrupt_ClearAndEnable: "
                        "IRQ %d cleared and enabled\n",
                        nIRQ);
            }
            break;
        default:
            LOG_INFO(
                    "Adapter_Interrupt_ClearAndEnable: "
                    "IRQ %d not supported\n",
                    nIRQ);
            return;
    }
}


/*----------------------------------------------------------------------------
 * Adapter_Interrupt_Disable
 *
 */
void
Adapter_Interrupt_Disable(
        const int nIRQ,
        const unsigned int Flags)
{
    IDENTIFIER_NOT_USED(Flags);

    switch(nIRQ)
    {
        case IRQ_DFE0:
        case IRQ_DSE0:
        case IRQ_PE0:
        case IRQ_CDR0:
        case IRQ_RDR0:
        case IRQ_CDR1:
        case IRQ_RDR1:
            {
                EIP201_SourceBitmap_t Sources = 0;

                if(AdapterINT_IRQ == -1)
                {
                    LOG_INFO(
                            "Adapter_Interrupt_Disable: "
                            "Cannot disable IRQ %d "
                            "due to not installed handler\n",
                            nIRQ);
                    return;
                }

                AdapterINT_To_EIP201_InterruptSource(nIRQ, &Sources);


                if (nIRQ == IRQ_CDR0)
                    EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_CDR0);
                else if (nIRQ == IRQ_RDR0)
                    EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Ring0,
                                               EIP201_INT_RING_RDR0);
                else if (nIRQ == IRQ_CDR1)
                    EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_CDR1);
                else if (nIRQ == IRQ_RDR1)
                    EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Ring1,
                                               EIP201_INT_RING_RDR1);

                EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Global,
                                                Sources);


                LOG_INFO(
                        "Adapter_Interrupt_Disable: "
                        "IRQ %d disabled\n",
                        nIRQ);
            }
            break;
        default:
            LOG_INFO(
                    "Adapter_Interrupt_Disable: "
                    "IRQ %d not supported\n",
                    nIRQ);
            return;
    }
}

/*----------------------------------------------------------------------------
 * Adapter_Interrupt_SetHandler
 */
void
Adapter_Interrupt_SetHandler(
        const int nIRQ,
        Adapter_InterruptHandler_t HandlerFunction)
{
    if (nIRQ < 0 || nIRQ >= ADAPTER_MAX_INTERRUPTS)
        return;

    LOG_INFO(
            "Adapter_Interrupt_SetHandler: "
            "HandlerFnc=%p for IRQ %d\n",
            HandlerFunction,
            nIRQ);

    /*
     * The way this function is implemented depends a lot on the enumeration
     * of the interrupt sources in adapter_interrupt.h!
     *
     * The interrupt handlers for the CDR and RDR interrupts will be
     * executed in the context of Linux bottom-half a.k.a. softirq!
     *
     *  The interrupt handlers for the other interrupts will be
     *  executed in the context of Linux top-half!
     *
     */
    AdapterINT_Handlers[nIRQ] = HandlerFunction;
}


/*----------------------------------------------------------------------------
 * AdapterINT_CommonTasklet
 *
 * This handler is scheduled in the top-halve interrupt handler when it
 * decodes one of the CDR or RDR interrupt sources.
 * The data parameter is the IRQ value (from adapter_interrupts.h) for that
 * specific interrupt source.
 */
static void
AdapterINT_CommonTasklet(
        unsigned long data)
{
    const unsigned int IntNr = (unsigned int)data;
    Adapter_InterruptHandler_t H;


    LOG_INFO("Tasklet invoked intnr=%d\n",IntNr);

    // verify we have a handler
    H = AdapterINT_Handlers[IntNr];

    if (H)
    {
        // invoke the handler
        H(IntNr);
    }
    else
    {
        EIP201_SourceBitmap_t Sources = 0;

        AdapterINT_To_EIP201_InterruptSource(IntNr, &Sources);

        LOG_CRIT(
            "AdapterINT_CommonTasklet: "
            "Disabling IRQ %d with missing handler\n",
            IntNr);

        // no reentrance protection!
        EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Global, Sources);
    }
}

ADAPTERINT_MAKE_TASKLET(EIP202_RESULT_NOTIFY0, IRQ_RDR0);
ADAPTERINT_MAKE_TASKLET(EIP202_COMMAND_NOTIFY0, IRQ_CDR0);
ADAPTERINT_MAKE_TASKLET(EIP202_RESULT_NOTIFY1, IRQ_RDR1);
ADAPTERINT_MAKE_TASKLET(EIP202_COMMAND_NOTIFY1, IRQ_CDR1);

#ifndef CONFIG_LTQ_MPE_IPSEC_SUPPORT
/*----------------------------------------------------------------------------
 * AdapterINT_TopHalfHandler
 *
 * This is the interrupt handler function call by the kernel when our hooked
 * interrupt is active.
 * We ask the AIC which sources are active.
 * For CDR and RDR interrupts we schedule a bottom-half handler.
 * These BHs then invoke the installed handlers.
 */
static irqreturn_t
AdapterINT_TopHalfHandler(
        int irq,
        void * dev_id)
{
    int IntNr=0;
    EIP201_SourceBitmap_t Sources = 0;
    EIP201_SourceBitmap_t RingSources = 0;

    int nIRQ = ADAPTER_MAX_INTERRUPTS;

    LOG_INFO("Top Half Handler called, IRQ: %d\n", irq);

    if (irq != AdapterINT_IRQ)
        return IRQ_NONE;

    LOG_INFO("Top Half Handler got correct IRQ\n");

    /*
     * Read the active interrupt sources at the AIC
     */

    Sources = EIP201_SourceStatus_ReadAllEnabled(Adapter_Device_EIP201_Global);

    if (Sources == 0) 
        return IRQ_NONE;        // ## RETURN ##

    LOG_INFO("Interrupt sources non-zero %x\n",Sources);

    /*
     * Acknowledgeinterrupt source at the AIC
     */

    EIP201_Acknowledge(
            Adapter_Device_EIP201_Global,
            Sources);

    if (Sources & ADAPTER_EIP202_INTERRUPTS_TRACEFILTER)
    {
        LOG_CRIT(
            "AdapterINT_TopHalfHandler: "
            "Interrupt sources=0x%08x\n",
            Sources);
    }

    if (Sources &
        (EIP201_INT_GLOBAL_RING0))
    {
        Sources &= ~EIP201_INT_GLOBAL_RING0;

        RingSources = EIP201_SourceStatus_ReadAllEnabled(
            Adapter_Device_EIP201_Ring0);


        LOG_INFO("Ring interrupt occurred ring sources %x\n",RingSources);

        EIP201_Acknowledge(
            Adapter_Device_EIP201_Ring0,
            RingSources);

        if (RingSources & EIP201_INT_RING_CDR0)
        {
            AdapterINT_Increment_InterruptCounter(IRQ_CDR0);
            Adapter_Interrupt_Disable(IRQ_CDR0, 0);
            LOG_INFO("CDR bottomhalf scheduled\n");
            ADAPTERINT_SCHEDULE_TASKLET(EIP202_COMMAND_NOTIFY0);
        }
        else if (RingSources & EIP201_INT_RING_RDR0)
        {
            AdapterINT_Increment_InterruptCounter(IRQ_RDR0);
            Adapter_Interrupt_Disable(IRQ_RDR0, 0);
            LOG_INFO("RDR bottomhalf scheduled\n");
            ADAPTERINT_SCHEDULE_TASKLET(EIP202_RESULT_NOTIFY0);
        }
	}

    if (Sources &
        (EIP201_INT_GLOBAL_RING1))
    {
        Sources &= ~EIP201_INT_GLOBAL_RING1;

        RingSources = EIP201_SourceStatus_ReadAllEnabled(
            Adapter_Device_EIP201_Ring1);

        LOG_INFO("Ring interrupt occurred ring sources %x\n",RingSources);

        EIP201_Acknowledge(
            Adapter_Device_EIP201_Ring1,
            RingSources);

        if (RingSources & EIP201_INT_RING_CDR1)
        {
            AdapterINT_Increment_InterruptCounter(IRQ_CDR1);
            Adapter_Interrupt_Disable(IRQ_CDR1, 0);
            LOG_INFO("CDR bottomhalf scheduled\n");
            ADAPTERINT_SCHEDULE_TASKLET(EIP202_COMMAND_NOTIFY1);
        }
        else if (RingSources & EIP201_INT_RING_RDR1)
        {
            AdapterINT_Increment_InterruptCounter(IRQ_RDR1);
            Adapter_Interrupt_Disable(IRQ_RDR1, 0);
            LOG_INFO("RDR bottomhalf scheduled\n");
            ADAPTERINT_SCHEDULE_TASKLET(EIP202_RESULT_NOTIFY1);
        }
    }

    if (Sources)
    {
        IntNr = AdapterINT_GetActiveIntNr(Sources);

        AdapterINT_From_EIP201_InterruptSource(1<<IntNr, &nIRQ);

        AdapterINT_Increment_InterruptCounter(nIRQ);
    }

    // now figure out which sources are active and call
    // the appropriate interrupt handlers that are installed
    while(Sources)
    {
        // now remove that bit from Sources
        Sources ^= (1 << IntNr);

        // verify we have a handler
        {
            Adapter_InterruptHandler_t H;



            if (nIRQ >= 0 && nIRQ < ADAPTER_MAX_INTERRUPTS &&
                AdapterINT_Handlers[nIRQ] != NULL)
            {
                H = AdapterINT_Handlers[nIRQ];
                // invoke the handler
                H(nIRQ);
            }
            else
            {
                EIP201_SourceBitmap_t FailedSources = 0;

                AdapterINT_To_EIP201_InterruptSource(nIRQ, &FailedSources);

                LOG_CRIT(
                    "AdapterINT_TopHalfHandler: "
                    "Failed, IRQ %d with missing handler\n",
                    nIRQ);

                // no reentrance protection!
                EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Global,
                                                FailedSources);
            }
        }

        if(Sources)
        {
            IntNr = AdapterINT_GetActiveIntNr(Sources);

            AdapterINT_From_EIP201_InterruptSource(1<<IntNr, &nIRQ);

            AdapterINT_Increment_InterruptCounter(nIRQ);
        }
    } // while

    IDENTIFIER_NOT_USED(dev_id);
    return IRQ_HANDLED;
}
#else
static irqreturn_t
AdapterINT_Ring0(int irq, void * dev_id)
{
    EIP201_SourceBitmap_t RingSources = 0;

    LOG_INFO("Top Half Handler called, IRQ: %d\n", irq);

    if (irq != AdapterINT_IRQ)
        return IRQ_NONE;

	RingSources = EIP201_SourceStatus_ReadAllEnabled(
            Adapter_Device_EIP201_Ring0);

	if (RingSources == 0)
		return IRQ_NONE;

    LOG_INFO("Ring interrupt occurred ring sources %x\n",RingSources);

    EIP201_Acknowledge(
		Adapter_Device_EIP201_Ring0,
        RingSources);

    if (RingSources & EIP201_INT_RING_CDR0)
    {
		AdapterINT_Increment_InterruptCounter(IRQ_CDR0);
		Adapter_Interrupt_Disable(IRQ_CDR0, 0);
		LOG_INFO("CDR bottomhalf scheduled\n");
        ADAPTERINT_SCHEDULE_TASKLET(EIP202_COMMAND_NOTIFY0);
   	}
	else if (RingSources & EIP201_INT_RING_RDR0)
    {
    	AdapterINT_Increment_InterruptCounter(IRQ_RDR0);
    	Adapter_Interrupt_Disable(IRQ_RDR0, 0);
    	LOG_INFO("RDR bottomhalf scheduled\n");
    	ADAPTERINT_SCHEDULE_TASKLET(EIP202_RESULT_NOTIFY0);
    }

	return IRQ_HANDLED;
}

#endif /* CONFIG_LTQ_MPE_IPSEC_SUPPORT */

/*-----------------------------------------------------------------------------
 * AdapterINT_SanityCheck
 *
 * Install a temporary handler for an interrupt and force that interrupt.
 * Check that the interrupt does occur.
 *
 * Return true if the sanity check passes, false if it fails.
 *
 */
static bool
AdapterINT_SanityCheck(void)
{

    int i;
    EIP201_SourceBitmap_t Sources = 0;

    AdapterINT_To_EIP201_InterruptSource(ADAPTER_INT_SANITY_CHECK_IRQ,
                                         &Sources);

    Adapter_Interrupt_SetHandler(ADAPTER_INT_SANITY_CHECK_IRQ,
                                 AdapterINT_SanityCheckHandler);
    Adapter_Interrupt_Enable(ADAPTER_INT_SANITY_CHECK_IRQ, 0);

    InterruptHasOccurred = false;

    // By changing from rising to falling edge, the edge detection logic
    // will detect an edge, so the interrupt will be triggered.
    EIP201_Config_Change(
        Adapter_Device_EIP201_Global,
        Sources,
        EIP201_CONFIG_RISING_EDGE);

    // Restore the interrupt polarity to normal.
    EIP201_Config_Change(
        Adapter_Device_EIP201_Global,
        Sources,
        EIP201_CONFIG_FALLING_EDGE);

    // Restore the interrupt polarity to normal.
    EIP201_Config_Change(
        Adapter_Device_EIP201_Global,
        Sources,
        EIP201_CONFIG_ACTIVE_LOW);


    // Wait for the interrupt to occur
    for (i=0; i<ADAPTER_INT_SANITY_CHECK_MAX_WAIT_LOOPS; i++)
    {
        if(InterruptHasOccurred)
            break;
    }

    Adapter_Interrupt_Disable(ADAPTER_INT_SANITY_CHECK_IRQ, 0);
    Adapter_Interrupt_SetHandler(ADAPTER_INT_SANITY_CHECK_IRQ, NULL);

    return InterruptHasOccurred;
}

/*----------------------------------------------------------------------------
 * Adapter_Interrupts_Init
 */
bool
Adapter_Interrupts_Init(
        const int nIRQ, struct platform_device *pdev)
{
    int IRQ_n_offset = IRQ_OFFSET;
    EIP201_Status_t res;
    struct device *Device_p;

    Device_p = &pdev->dev;

    ZEROINIT(AdapterINT_Handlers);

    Adapter_Device_EIP201_Global = Device_Find("EIP201_GLOBAL");
    if (Adapter_Device_EIP201_Global == NULL)
    {
        LOG_CRIT("Adapter_Interrupts_Init: Failed to find EIP201 device\n");
        return false;
    }

    Adapter_Device_EIP201_Ring0 = Device_Find("EIP201_RING0");
    if (Adapter_Device_EIP201_Ring0 == NULL)
    {
        LOG_CRIT("Adapter_Interrupts_Init: Failed to find EIP201 device\n");
        return false;
    }

#ifndef CONFIG_LTQ_MPE_IPSEC_SUPPORT
    Adapter_Device_EIP201_Ring1 = Device_Find("EIP201_RING1");
    if (Adapter_Device_EIP201_Ring1 == NULL)
    {
        LOG_CRIT("Adapter_Interrupts_Init: Failed to find EIP201 device\n");
        return false;
	}
#endif

    // initialize driver and EIP201
    // but leave all interrupts disabled
    res = EIP201_Initialize(
                Adapter_Device_EIP201_Global,
                Adapter_InterruptSettings_Global,
                ARRAY_ELEMCOUNT(Adapter_InterruptSettings_Global));

    if (res != EIP201_STATUS_SUCCESS)
    {
        LOG_CRIT(
            "Adapter_Interrupts_Init: "
            "EIP201_Initialized returned %d\n",
            res);

        return false;
    }

    // initialize driver and EIP201
    // but leave all interrupts disabled
    res = EIP201_Initialize(
                Adapter_Device_EIP201_Ring0,
                Adapter_InterruptSettings_Ring0,
                ARRAY_ELEMCOUNT(Adapter_InterruptSettings_Ring0));

    if (res != EIP201_STATUS_SUCCESS)
    {

        LOG_CRIT(
            "Adapter_Interrupts_Init: "
            "EIP201_Initialized returned %d\n",
            res);

        return false;
    }

#ifndef CONFIG_LTQ_MPE_IPSEC_SUPPORT
    // initialize driver and EIP201
    // but leave all interrupts disabled
    res = EIP201_Initialize(
                Adapter_Device_EIP201_Ring1,
                Adapter_InterruptSettings_Ring1,
                ARRAY_ELEMCOUNT(Adapter_InterruptSettings_Ring1));

    if (res != EIP201_STATUS_SUCCESS)
    {

        LOG_CRIT(
            "Adapter_Interrupts_Init: "
            "EIP201_Initialized returned %d\n",
            res);

        return false;
    }
#endif

    if (nIRQ != -1)
    {
        AdapterINT_IRQ = nIRQ + IRQ_n_offset;

        res = request_irq(
                        AdapterINT_IRQ,
                        irq_handler_callback,
                        0,
                        ADAPTER_EIP202_DRIVER_NAME,
                        Device_p);

        if (res)
        {
            LOG_CRIT(
                "Adapter_Interrupts_Init: "
                "request_irq  returned %d\n",
                res);

            EIP201_SourceMask_DisableSource(Adapter_Device_EIP201_Global,
                                            EIP201_SOURCE_ALL);

            AdapterINT_IRQ = -1;
            return false;
        }

        LOG_INFO(
                "Adapter_Interrupts_Init: "
                "Successfully hooked IRQ %d\n",
                nIRQ);
    }

    return true;
}


/*----------------------------------------------------------------------------
 * Adapter_Interrupts_UnInit
 */
void
Adapter_Interrupts_UnInit(const int nIRQ)
{
    IDENTIFIER_NOT_USED(nIRQ);

    if (AdapterINT_IRQ != -1)
    {
        struct device * Device_p;

        // Get device reference for this resource
        Device_p = Device_GetReference(NULL);

        // disable all interrupts
        EIP201_SourceMask_DisableSource(
                Adapter_Device_EIP201_Global,
                EIP201_SOURCE_ALL);

        // disable all interrupts
        EIP201_SourceMask_DisableSource(
                Adapter_Device_EIP201_Ring0,
                EIP201_SOURCE_ALL);

#ifndef CONFIG_LTQ_MPE_IPSEC_SUPPORT
        EIP201_SourceMask_DisableSource(
                Adapter_Device_EIP201_Ring1,
                EIP201_SOURCE_ALL);
#endif 

        ADAPTERINT_KILL_TASKLET(EIP202_COMMAND_NOTIFY0);
        ADAPTERINT_KILL_TASKLET(EIP202_RESULT_NOTIFY0);
        ADAPTERINT_KILL_TASKLET(EIP202_COMMAND_NOTIFY1);
        ADAPTERINT_KILL_TASKLET(EIP202_RESULT_NOTIFY1);
        // unhook the interrupt
        free_irq(AdapterINT_IRQ, Device_p);

        AdapterINT_Report_InterruptCounters();

        AdapterINT_IRQ = -1;
    }
}


/* end of file adapter_interrupts.c */
