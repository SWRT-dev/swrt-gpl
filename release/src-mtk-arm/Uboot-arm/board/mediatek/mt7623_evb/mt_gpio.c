#include <asm/arch/mt_reg_base.h>
#include <asm/arch/mt_gpio.h>
#include <asm/arch/debug.h>
static GPIO_REGS *gpio_reg = (GPIO_REGS*)(GPIO_BASE);

/*-------for msdc pupd-----------*/

/************************************
**MSD0 PINS**
MSD0_DAT7  GPIO111, msd0_ctrl4, bit12:PUPD ,  bit13: R1 ,  bit14:R0     D00
MSD0_DAT6  GPIO112, msd0_ctrl4, bit8:PUPD ,  bit9: R1 ,  bit10:R0         D00
MSD0_DAT5  GPIO113, msd0_ctrl4, bit4:PUPD ,  bit5: R1 ,  bit6:R0           D00
MSD0_DAT4  GPIO114, msd0_ctrl4, bit0:PUPD ,  bit1: R1 ,  bit2:R0           D00
MSD0_RSTB  GPIO115, msd0_ctrl5, bit0:PUPD ,  bit1: R1 ,  bit2:R0           D10

MSD0_CMD   GPIO116, msd0_ctrl1, bit8:PUPD ,  bit9: R1 ,  bit10:R0         CD0

MSD0_CLK    GPIO117, msd0_ctrl0, bit8:PUPD ,  bit9: R1 ,  bit10:R0         CC0

MSD0_DAT3  GPIO118, msd0_ctrl3, bit12:PUPD ,  bit13: R1 ,  bit14:R0     CF0
MSD0_DAT2  GPIO119, msd0_ctrl3, bit8:PUPD ,  bit9: R1 ,  bit10:R0
MSD0_DAT1  GPIO120, msd0_ctrl3, bit4:PUPD ,  bit5: R1 ,  bit6:R0
MSD0_DAT0  GPIO121, msd0_ctrl3, bit0:PUPD ,  bit1: R1 ,  bit2:R0

**MSD1 PINS**
MSD1_CMD   GPIO105, msd1_ctrl1, bit8:PUPD ,  bit9: R1 ,  bit10:R0       D40

MSD1_CLK    GPIO106, msd1_ctrl0, bit8:PUPD ,  bit9: R1 ,  bit10:R0       D30

MSD1_DAT0  GPIO107, msd1_ctrl3, bit0:PUPD ,  bit1: R1 ,  bit2:R0         D60
MSD1_DAT1  GPIO108, msd1_ctrl3, bit4:PUPD ,  bit5: R1 ,  bit6:R0         D60
MSD1_DAT2  GPIO109, msd1_ctrl3, bit8:PUPD ,  bit9: R1 ,  bit10:R0       D60
MSD1_DAT3  GPIO110, msd1_ctrl3, bit12:PUPD ,  bit13: R1 ,  bit14:R0   D60

**MSD2 PINS**
MSD2_CMD   GPIO85, msd2_ctrl1, bit8:PUPD ,  bit9: R1 ,  bit10:R0        DA0
MSD2_CLK    GPIO86, msd2_ctrl0, bit8:PUPD ,  bit9: R1 ,  bit10:R0        D90
MSD2_DAT0  GPIO87, msd2_ctrl3, bit0:PUPD ,  bit1: R1 ,  bit2:R0          DC0
MSD2_DAT1  GPIO88, msd2_ctrl3, bit4:PUPD ,  bit5: R1 ,  bit6:R0          DC0
MSD2_DAT2  GPIO89, msd2_ctrl3, bit8:PUPD ,  bit9: R1 ,  bit10:R0        DC0
MSD2_DAT3  GPIO90, msd2_ctrl3, bit12:PUPD ,  bit13: R1 ,  bit14:R0    DC0

**MSD3 PINS**
MSDC0E_RSTB  GPIO249, msd3_ctrl5, bit0:PUPD ,  bit1: R1 ,  bit2:R0  140

MSDC0E_DAT7 GPIO250, msd3_ctrl4, bit12:PUPD ,  bit13: R1 ,  bit14:R0      130
MSDC0E_DAT6 GPIO251, msd3_ctrl4, bit8:PUPD ,  bit9: R1 ,  bit10:R0      130
MSDC0E_DAT5 GPIO252, msd3_ctrl4, bit4:PUPD ,  bit5: R1 ,  bit6:R0        130
MSDC0E_DAT4 GPIO253, msd3_ctrl4, bit0:PUPD ,  bit1: R1 ,  bit2:R0      130

MSDC0E_DAT3 GPIO254, msd3_ctrl3, bit12:PUPD ,  bit13: R1 ,  bit14:R0      F40
MSDC0E_DAT2 GPIO255, msd3_ctrl3, bit8:PUPD ,  bit9: R1 ,  bit10:R0      F40
MSDC0E_DAT1 GPIO256, msd3_ctrl3, bit4:PUPD ,  bit5: R1 ,  bit6:R0      F40
MSDC0E_DAT0 GPIO257, msd3_ctrl3, bit0:PUPD ,  bit1: R1 ,  bit2:R0      F40

MSDC0E_CMD GPIO258, msd3_ctrl1, bit8:PUPD ,  bit9: R1 ,  bit10:R0      CB0
MSDC0E_CLK  GPIO259, msd3_ctrl0, bit8:PUPD ,  bit9: R1 ,  bit10:R0      C90

MSDC1_INS  GPIO261, msd3_ctrl5, bit8:PUPD ,  bit9: R1 ,  bit10:R0 140
************************************/

#define MSDC0_MIN_PIN GPIO111
#define MSDC0_MAX_PIN GPIO121
#define MSDC1_MIN_PIN GPIO105
#define MSDC1_MAX_PIN GPIO110
#define MSDC2_MIN_PIN GPIO85
#define MSDC2_MAX_PIN GPIO90
#define MSDC3_MIN_PIN GPIO249
#define MSDC3_MAX_PIN GPIO261

#define MSDC0_PIN_NUM (MSDC0_MAX_PIN - MSDC0_MIN_PIN + 1)
#define MSDC1_PIN_NUM (MSDC1_MAX_PIN - MSDC1_MIN_PIN + 1)
#define MSDC2_PIN_NUM (MSDC2_MAX_PIN - MSDC2_MIN_PIN + 1)
#define MSDC3_PIN_NUM (MSDC3_MAX_PIN - MSDC3_MIN_PIN + 1)

static const struct msdc_pupd msdc_pupd_spec[]=
{
    /*msdc0 pin*/
    {0xD00, 12},
    {0xD00, 8},
    {0xD00, 4},
    {0xD00, 0},
    {0xD10, 0},
    {0xCD0, 8},
    {0xCC0, 8},
    {0xCF0, 12},
    {0xCF0, 8},
    {0xCF0, 4},
    {0xCF0, 0},
    /*msdc1 pin*/
    {0xD40, 8},
    {0xD30, 8},
    {0xD60, 0},
    {0xD60, 4},
    {0xD60, 8},
    {0xD60, 12},
    /*msdc2 pin*/
    {0xDA0, 8},
    {0xD90, 8},
    {0xDC0, 0},
    {0xDC0, 4},
    {0xDC0, 8},
    {0xDC0, 12},
    /*msdc3 pin*/
    {0x140, 0},
    {0x130, 12},
    {0x130, 8},
    {0x130, 4},
    {0x130, 0},
    {0xF40, 12},
    {0xF40, 8},
    {0xF40, 4},
    {0xF40, 0},
    {0xCB0, 8},
    {0xC90, 8},
    {0x3A0, 8},
    {0x140, 8},
};

#define PUPD_OFFSET_TO_REG(msdc_pupd_ctrl) ((VAL_REGS*)(GPIO_BASE + msdc_pupd_ctrl->reg))
/*---------------------------------------------------------------------------*/
#define IS_GPI_PIN(pin) ((pin >= GPIO91) && (pin <= GPIO100))

/*---------------------------------------------------------------------------*/
const struct msdc_pupd * mt_get_msdc_ctrl(unsigned long pin)
{
    unsigned int idx = 255;
    if ((pin >= MSDC0_MIN_PIN) && (pin <= MSDC0_MAX_PIN)){
        idx = pin - MSDC0_MIN_PIN;
    } else if ((pin >= MSDC1_MIN_PIN) && (pin <= MSDC1_MAX_PIN)){
        idx = MSDC0_PIN_NUM + pin-MSDC1_MIN_PIN;//pin-94
    } else if ((pin >= MSDC2_MIN_PIN) && (pin <= MSDC2_MAX_PIN)){
        idx = MSDC0_PIN_NUM + MSDC1_PIN_NUM + pin-MSDC2_MIN_PIN;//pin-68
    } else if ((pin >= MSDC3_MIN_PIN) && (pin <= MSDC3_MAX_PIN)){
        idx = MSDC0_PIN_NUM + MSDC1_PIN_NUM + MSDC2_PIN_NUM + pin-MSDC3_MIN_PIN;//pin-226
    } else {
        return NULL;
    }
    return &msdc_pupd_spec[idx];
}

/*---------------------------------------------------------------------------*/
int mt_set_gpio_dir_chip(unsigned long pin, unsigned long dir)
{
    unsigned long pos;
    unsigned long bit;
    GPIO_REGS *reg = gpio_reg;
    VAL_REGS *dir_grp = NULL;

    if (IS_GPI_PIN(pin) && (dir == GPIO_DIR_OUT))
    {
        return  -ERINVAL;
    }

    if(pin < GPIO_DIR_GRP2_PIN_BASE){
        dir_grp = &reg->dir1[0];
    }
    else{
        dir_grp = &reg->dir2[0];
        pin = pin - GPIO_DIR_GRP2_PIN_BASE;
    }

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (dir == GPIO_DIR_IN)
        GPIO_SET_BITS((1L << bit), &dir_grp[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &dir_grp[pos].set);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_dir_chip(unsigned long pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    GPIO_REGS *reg = gpio_reg;
    VAL_REGS *dir_grp = NULL;

    if (IS_GPI_PIN(pin))
    {
        return  GPIO_DIR_IN;
    }

    if(pin < GPIO_DIR_GRP2_PIN_BASE){
        dir_grp = &reg->dir1[0];
    }
    else{
        dir_grp = &reg->dir2[0];
        pin = pin - GPIO_DIR_GRP2_PIN_BASE;
    }

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    data = GPIO_RD32(&dir_grp[pos].val);
    return (((data & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_pull_enable_chip(unsigned long pin, unsigned long enable)
{
    unsigned long pos;
    unsigned long bit;
    GPIO_REGS *reg = gpio_reg;
    const struct msdc_pupd * msdc_pupd_ctrl;

	/*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
        if (enable == GPIO_PULL_DISABLE){
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 1)), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->rst));
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 2)), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->rst));
        }
        else {
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 1)), &((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->set));
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 2)), &((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->rst));
        }

        return RSUCCESS;
	}


	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	if (enable == GPIO_PULL_DISABLE)
		GPIO_SET_BITS((1L << bit), &reg->pullen[pos].rst);
	else
		GPIO_SET_BITS((1L << bit), &reg->pullen[pos].set);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull_enable_chip(unsigned long pin)
{
    unsigned long pos=0;
    unsigned long bit=0;
    unsigned long data=0;
    GPIO_REGS *reg = gpio_reg;
    const struct msdc_pupd * msdc_pupd_ctrl;

	/*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
        data = GPIO_RD32(&PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->val);
        return (((data & (3L << (msdc_pupd_ctrl->bit + 1))) == 0)? 0: 1);
    }

	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	data = GPIO_RD32(&reg->pullen[pos].val);

    return (((data & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_pull_select_chip(unsigned long pin, unsigned long select)
{
    unsigned long pos;
    unsigned long bit;
    GPIO_REGS *reg = gpio_reg;
    const struct msdc_pupd * msdc_pupd_ctrl;

	/*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
        if (select == GPIO_PULL_DOWN){
            GPIO_SET_BITS((1L << msdc_pupd_ctrl->bit), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->set);
        }
        else {
            GPIO_SET_BITS((1L << msdc_pupd_ctrl->bit), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->rst);
        }
        return RSUCCESS;
	}

    pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	if (select == GPIO_PULL_DOWN)
		GPIO_SET_BITS((1L << bit), &reg->pullsel[pos].rst);
	else
		GPIO_SET_BITS((1L << bit), &reg->pullsel[pos].set);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull_select_chip(unsigned long pin)
{
    unsigned long pos=0;
    unsigned long bit=0;
    unsigned long data=0;
    GPIO_REGS *reg = gpio_reg;
    const struct msdc_pupd * msdc_pupd_ctrl;

	/*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
        data = GPIO_RD32(&((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->val));
        return (((data & (1L << msdc_pupd_ctrl->bit)) != 0)? 0: 1);
	}

	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	data = GPIO_RD32(&reg->pullsel[pos].val);

    return (((data & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_out_chip(unsigned long pin, unsigned long output)
{
    unsigned long pos;
    unsigned long bit;
    GPIO_REGS *reg = gpio_reg;

    if(IS_GPI_PIN(pin))
    {
        return -ERINVAL;
    }

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (output == GPIO_OUT_ZERO)
        GPIO_SET_BITS((1L << bit), &reg->dout[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &reg->dout[pos].set);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_out_chip(unsigned long pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    GPIO_REGS *reg = gpio_reg;

    if(IS_GPI_PIN(pin))
    {
        return -ERINVAL;
    }

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    data = GPIO_RD32(&reg->dout[pos].val);
    return (((data & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_in_chip(unsigned long pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    GPIO_REGS *reg = gpio_reg;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    data = GPIO_RD32(&reg->din[pos].val);
    return (((data & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_mode_chip(unsigned long pin, unsigned long mode)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    unsigned long mask = (1L << GPIO_MODE_BITS) - 1;
    GPIO_REGS *reg = gpio_reg;

	pos = pin / MAX_GPIO_MODE_PER_REG;
	bit = pin % MAX_GPIO_MODE_PER_REG;

	data = GPIO_RD32(&reg->mode[pos].val);

	data &= ~(mask << (GPIO_MODE_BITS*bit));
	data |= (mode << (GPIO_MODE_BITS*bit));

	GPIO_WR32(&reg->mode[pos].val, data);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_mode_chip(unsigned long pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long data;
    unsigned long mask = (1L << GPIO_MODE_BITS) - 1;
    GPIO_REGS *reg = gpio_reg;

	pos = pin / MAX_GPIO_MODE_PER_REG;
	bit = pin % MAX_GPIO_MODE_PER_REG;

	data = GPIO_RD32(&reg->mode[pos].val);

	return ((data >> (GPIO_MODE_BITS*bit)) & mask);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_dir(unsigned long pin, unsigned long dir)
{
    return mt_set_gpio_dir_chip(pin,dir);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_dir(unsigned long pin)
{
    return mt_get_gpio_dir_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_pull_enable(unsigned long pin, unsigned long enable)
{
    return mt_set_gpio_pull_enable_chip(pin,enable);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull_enable(unsigned long pin)
{
    return mt_get_gpio_pull_enable_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_pull_select(unsigned long pin, unsigned long select)
{
    return mt_set_gpio_pull_select_chip(pin,select);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_pull_select(unsigned long pin)
{
    return mt_get_gpio_pull_select_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_out(unsigned long pin, unsigned long output)
{
    return mt_set_gpio_out_chip(pin,output);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_out(unsigned long pin)
{
    return mt_get_gpio_out_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_in(unsigned long pin)
{
    return mt_get_gpio_in_chip(pin);
}
/*---------------------------------------------------------------------------*/
int mt_set_gpio_mode(unsigned long pin, unsigned long mode)
{
    return mt_set_gpio_mode_chip(pin,mode);
}
/*---------------------------------------------------------------------------*/
int mt_get_gpio_mode(unsigned long pin)
{
    return mt_get_gpio_mode_chip(pin);
}
