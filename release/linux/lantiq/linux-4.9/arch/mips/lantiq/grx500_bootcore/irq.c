/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 * Copyright (C) 2010 Thomas Langer <thomas.langer@lantiq.com>
 */

#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/slab.h>

#include <asm/bootinfo.h>
#include <asm/irq_cpu.h>

#include <lantiq_soc.h>
#include <irq.h>
#include <grx500_bootcore_emerald_env_regs.h>
#include <grx500_bootcore_cnfg.h>
#include <grx500_bootcore_chadr.h>
#include <grx500_bootcore_chipreg.h>
#include <grx500_bootcore_interrupt.h>
#include <grx500_bootcore_uart.h>

#define GET_REG_IRQ_MASK MT_RdReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK)
#define SET_REG_IRQ_MASK(irq_mask) MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK, irq_mask )

static uint32_t irq_mask_save = 0;

/*inline*/ void grx500_bootcore_disable_irq_all(void)
{
        uint32_t irq_mask = 0x0;

        irq_mask_save = MT_RdReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK);
        MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK, irq_mask );
}

/*inline*/ void grx500_bootcore_disable_irq(struct irq_data *d)
{
        uint32_t irq_mask = 0;

        irq_mask = MT_RdReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK);

        irq_mask &= ~(1 << (d->irq));

        MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK, irq_mask );

}

inline void grx500_bootcore_ack_irq(unsigned int irq_nr)
{
    /* ack the relevant interrupt */
    switch(irq_nr) {
#if 1
        case 4:
        case 3:
        case 2:
#endif
        case 1:
        /* UART will reset the interrupt in the handler */
                break;
        default:
        while(1);
        BUG();
                break;
        }
}

#if 0
/*inline*/ void grx500_bootcore_enable_irq_all(void)
{
        uint32_t irq_mask = 0;

        irq_mask = (1<<GRX500_BOOTCORE_SERIAL_IRQ_OUT_INDEX) |
                    (1<<GRX500_BOOTCORE_TIMER_IRQ_OUT_INDEX) |
                    (1<<GRX500_BOOTCORE_SYNOP_ETHER_IRQ_OUT_INDEX) |
                    (1<<GRX500_BOOTCORE_SYNOP_ETHER_IRQ_GMAC2_OUT_INDEX) |
                    (1<<GRX500_BOOTCORE_WIRELESS_IRQ_OUT_INDEX);
    

        SET_REG_IRQ_MASK(irq_mask/*irq_mask_save*/);
        //MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK, irq_mask );
}
#endif

/*inline*/ void grx500_bootcore_enable_irq(struct irq_data *d)
{
        uint32_t irq_mask = 0;

        //irq_mask = MT_RdReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK);
        irq_mask = GET_REG_IRQ_MASK;

    irq_mask |= (1 << (d->irq));

        SET_REG_IRQ_MASK(irq_mask);
        //MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, REG_IRQ_MASK, irq_mask );
}
#if 1
#define set_status(x)   __asm__ volatile("mtc0 %0,$12" :: "r" (x))

/*inline*/ void grx500_bootcore_enable_irq_all_mips(unsigned long val)
{
        set_status(val);
/*      __asm__(
        "       li      $10,0xe0000001          \n"
        "       mtc0    $10,$12                         \n"
        );
*/
}
#endif

#if 1 //HUANX 
/*static */irqreturn_t uart_interrupt(int irq, void *dev_id)
{
        uint32_t UartStatus;
#ifdef GRX500_BOOTCORE_DEBUG_UART
    uint32_t temp1,temp2;

    temp1 = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UNMAPPED_IRQS_TO_MAPPER);
    temp2 = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_MAPPED_IRQS_FROM_MAPPER);
#endif

        UartStatus = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UART_INT);
    //UartStatus |=0x7f00;
    
    //printk("enter: UNMAPPED_IRQS_TO_MAPPER = %x, MAPPED_IRQS_FROM_MAPPER = %x, stat b=%x ", temp1, temp2, UartStatus);

        // clear interrupt
        MT_WrReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UART_INT, UartStatus);
#ifdef GRX500_BOOTCORE_DEBUG_UART
    temp1 = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UNMAPPED_IRQS_TO_MAPPER);
    temp2 = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_MAPPED_IRQS_FROM_MAPPER);

        UartStatus = MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, REG_UART_INT);
    printk("exit: UNMAPPED_IRQS_TO_MAPPER = %x, MAPPED_IRQS_FROM_MAPPER = %x, stat b=%x ", temp1, temp2, UartStatus);
#endif
        // TX Part
        if (UartStatus && REG_UART_WRITE_FIFO_WATER_LEVEL_IRQ_MASK)
            grx500_bootcore_uart_tx_isr(dev_id); /*needed???*/

        // RX Part
        if ((UartStatus && REG_UART_READ_FIFO_WATER_LEVEL_IRQ_MASK) ||
            (UartStatus && REG_UART_IDLE_IRQ_MASK))
        grx500_bootcore_uart_rx_isr(dev_id);
         
    return IRQ_HANDLED;
}
#endif //HUANX

static struct irq_chip external_irq_type = {
        .name     = "Lantiq GRX500_BOOTCORE Ext IRQ Controller",
        .irq_mask_ack = grx500_bootcore_disable_irq,
        .irq_mask     = grx500_bootcore_disable_irq,
        .irq_unmask   = grx500_bootcore_enable_irq,
};

#if 0 
static struct irqaction uart_irqaction = {
        .handler        = uart_interrupt,
        .flags          = IRQF_DISABLED, /* for disable nested interrupts */ 
        /* Lior.H - when we need to use-> IRQF_NOBALANCING ? */
        .name           = "grx500_bootcore_uart",
};
#endif

asmlinkage void plat_irq_dispatch(void)
{
        while(1);
}

static inline int clz(unsigned long x)
{
        __asm__(
        "       .set    push                                    \n"
        "       .set    mips32                                  \n"
        "       clz     %0, %1                                  \n"
        "       .set    pop                                     \n"
        : "=r" (x)
        : "r" (x));

        return x;
}

int __init icu_of_init(struct device_node *node, struct device_node *parent)
{

	mips_cpu_irq_init();

	grx500_bootcore_init_irq();

	return 0;
}

void MT_WrReg(MT_UINT32 unit, MT_UINT32 reg, MT_UINT32 data )
{
        NONE_TRUNK_REG(unit, reg)=data;
}

MT_UINT32 MT_RdReg(MT_UINT32 unit, MT_UINT32 reg)
{
        MT_UINT32 data;
        data = NONE_TRUNK_REG(unit, reg);
        return(data);
}

void MT_WrRegMask(MT_UINT32 unit, MT_UINT32 reg, MT_UINT32 mask,MT_UINT32 data )
{
                MT_UINT32 readVal,setData;      

                readVal=MT_RdReg(unit,reg);
                setData= (readVal & (~mask))|data;
                MT_WrReg(unit,reg, setData );
}

void EXCEP_remap_line( MT_UINT32 int_line, MT_UINT32 map_address, MT_UINT32 num)
{
        MT_UINT32 org_map, result_map, t1 = 0, t2 = 0;


    printk("EXCEP_remap_line: int_line = 0x%08lx, map_address = 0x%08lx, num = 0x%08lx \n",int_line,map_address, num);
        org_map = MT_RdReg( MT_LOCAL_MIPS_BASE_ADDRESS, map_address );
        t1 = ~(0x1f << (num * 5));
        t2 = int_line << (num * 5);
        result_map = org_map & t1;
        result_map |= t2;
    printk("EXCEP_remap_line: org_map = 0x%08lx, result_map = 0x%08lx \n",org_map,result_map);
        MT_WrReg( MT_LOCAL_MIPS_BASE_ADDRESS, map_address, result_map );
#if 0
{
        int i=0;
    for(i=REG_IRQ_SOURCE_01_MAPPING ; i <= REG_IRQ_SOURCE_31_MAPPING ; i+=4)
    {
        printk("EXCEP_remap_line: map[%d] = 0x%08lx \n",i,MT_RdReg(MT_LOCAL_MIPS_BASE_ADDRESS, i));
    }
}
#endif
}

/*static */void grx500_bootcore_uart_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_SERIAL_IRQ_OUT_INDEX/*0x1*/);
}

/*static */void grx500_bootcore_net_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_SYNOP_ETHER_IRQ_OUT_INDEX/*0x3*/);
}
EXPORT_SYMBOL(grx500_bootcore_net_irq);

/*static */void grx500_bootcore_net_g2_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_SYNOP_ETHER_IRQ_GMAC2_OUT_INDEX/*0x4*/);
}
EXPORT_SYMBOL(grx500_bootcore_net_g2_irq);

void grx500_bootcore_wls_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_WIRELESS_IRQ_OUT_INDEX);
}

EXPORT_SYMBOL(grx500_bootcore_wls_irq);

/*static */void grx500_bootcore_gptc2_timer2_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_GPTC2_TIMER2_OUT_INDEX);
}

/*static */void grx500_bootcore_gptc2_timer1_irq(void)
{
        do_IRQ(GRX500_BOOTCORE_GPTC2_TIMER1_OUT_INDEX);
}

void grx500_bootcore_mps_irq(void)
{
			do_IRQ(GRX500_BOOTCORE_MPS2_OUT_INDEX);
}

void grx500_bootcore_ngi_irq(void)
{
	do_IRQ(GRX500_BOOTCORE_NGIERROR_IRQ_OUT_INDEX);
} 
EXPORT_SYMBOL(grx500_bootcore_ngi_irq);

#if 1
void grx500_bootcore_register_static_irq_dummy(unsigned int irq_in, unsigned int irq_out)
{
  uint32_t mapper_reg;
  uint32_t mapper_offset;

  mapper_reg = REG_IRQ_SOURCE_01_MAPPING + (((irq_in - 1) / 6) * 4);
  mapper_offset = ((irq_in - 1) % 6);
  printk("grx500_bootcore_register_static_irq: irq_in = 0x%08x, irq_out = 0x%08x\n",irq_in,irq_out);
  printk("grx500_bootcore_register_static_irq: mapper_reg = 0x%08lx, mapper_offset = 0x%08lx\n",(long unsigned int)mapper_reg,(long unsigned int)mapper_offset);
  EXCEP_remap_line( irq_out, mapper_reg, mapper_offset);
}
#endif

void grx500_bootcore_init_irq(void)
{
        uint32_t irq;
        /*
        31 - SI_TimerInt
        30 - SI_SWInt[1]
        29 - SI_SWInt[0]
        28 - timer0_irq
        27 - timer1_irq
        26 - timer2_irq
        25 - timer3_irq
        24 - N/A
        23 - N/A
        ...
        05 - N/A 
        04 - N/A
        03 - N/A
        02 - UART
        01 - ETH
        00 - N/A - (Never used)
        */

        /* Remap all interrupts to an unused line - then open only those that are used */
#if 1
    for(irq=REG_IRQ_SOURCE_01_MAPPING ; irq <= REG_IRQ_SOURCE_31_MAPPING ; irq+=4)
        {
        MT_WrReg(MT_LOCAL_MIPS_BASE_ADDRESS, irq, 0 );
        }

        for (irq = 1; irq <= 31; irq++) {
//                set_irq_chip_and_handler(irq, &external_irq_type,
//                                         handle_level_irq);
		irq_set_chip_and_handler(irq, &external_irq_type,
						handle_level_irq);
//huanx fix                grx500_bootcore_disable_irq(irq);
#else
        for (irq = 1; irq <= 31; irq++) {
                if ((irq!= GRX500_BOOTCORE_SYNOP_ETHER_IRQ_IN_INDEX) |
                        (irq!= GRX500_BOOTCORE_SERIAL_IRQ_IN_INDEX) |
                            (irq!= GRX500_BOOTCORE_SYNOP_ETHER_IRQ_GMAC2_IN_INDEX) |
                                (irq!= GRX500_BOOTCORE_WIRELESS_IRQ_IN_INDEX) |
                                    (irq!= GRX500_BOOTCORE_TIMER_IRQ_IN_INDEX))
        {
                grx500_bootcore_register_static_irq_dummy(irq,0);

                    set_irq_chip_and_handler(irq, &external_irq_type,
                                             handle_level_irq);

                    grx500_bootcore_disable_irq(irq);
        }
#endif
        }

#if 0
    grx500_bootcore_register_irq(2 /* in */,1 /* out */,&uart_irqaction, grx500_bootcore_uart_irq); 
#endif

//grx500_bootcore_enable_irq_all_mips(/*0xe0000001*/0x00000001);

        grx500_bootcore_register_static_irq_dummy(31, 31);//(31,2);
//	grx500_bootcore_enable_irq(31);
}

int grx500_bootcore_register_static_irq(unsigned int irq_in, unsigned int irq_out,
                             struct irqaction *action, vi_handler_t addr)
{
  uint32_t mapper_reg;
  uint32_t mapper_offset;
  int res;

  mapper_reg = REG_IRQ_SOURCE_01_MAPPING + (((irq_in - 1) / 6) * 4);
  mapper_offset = ((irq_in - 1) % 6);
  printk("grx500_bootcore_register_static_irq: irq_in = 0x%08x, irq_out = 0x%08x\n",irq_in,irq_out);
  printk("grx500_bootcore_register_static_irq: mapper_reg = 0x%08lx, mapper_offset = 0x%08lx\n",(long unsigned int)mapper_reg,(long unsigned int)mapper_offset);
  EXCEP_remap_line( irq_out, mapper_reg, mapper_offset);

  set_vi_handler(irq_out, addr);

  res = setup_irq(irq_out, action);
  if(0 != res) {
         printk(KERN_INFO "grx500_bootcore_irq: setup_irq() failed!!! **********\n");
    return res;
  }
#if 0
  grx500_bootcore_enable_irq(irq_out);
#endif
  return 0;
}

EXPORT_SYMBOL(grx500_bootcore_register_static_irq);
EXPORT_SYMBOL(grx500_bootcore_enable_irq);
EXPORT_SYMBOL(grx500_bootcore_disable_irq);

int grx500_bootcore_register_irq(unsigned int irq_in, unsigned int irq_out,
                       irq_handler_t handler, unsigned long flags,
                       const char *name, void *dev_id,
                       vi_handler_t addr)
{
    struct irqaction *action;
    int res;

    action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
    if (!action)
      return -ENOMEM;

    action->handler = handler;
    action->flags = flags;
    action->name = name;
    action->dev_id = dev_id;

    res = grx500_bootcore_register_static_irq(irq_in, irq_out, action, addr);

    if(0 != res)
      kfree(action);

    return res;
}

EXPORT_SYMBOL(grx500_bootcore_register_irq);

void grx500_bootcore_unregister_irq(unsigned int irq_in, unsigned int irq_out, void* dev_id)
{
    uint32_t mapper_reg;
    uint32_t mapper_offset;

//fix huanx    grx500_bootcore_disable_irq(irq_out);

    mapper_reg = REG_IRQ_SOURCE_01_MAPPING + (((irq_in - 1) / 6) * 4);
    mapper_offset = ((irq_in - 1) % 6);
    EXCEP_remap_line(0, mapper_reg, mapper_offset);

    set_vi_handler(irq_out, NULL);
    free_irq(irq_out, dev_id);
}

EXPORT_SYMBOL(grx500_bootcore_unregister_irq);

static struct of_device_id __initdata of_irq_ids[] = {
	{ .compatible = "lantiq,icu", .data = icu_of_init },
	{},
};

void __init arch_init_irq(void)
{
	of_irq_init(of_irq_ids);
	grx500_bootcore_init_irq();	
}
