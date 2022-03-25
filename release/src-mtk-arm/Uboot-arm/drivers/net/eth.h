/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     register definition for MEDIATEK RT-series SoC
 *
 *  Copyright 2007 MEDIATEK Inc.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 **************************************************************************
 */

#ifndef __MEDIATEK_MMAP__
#define __MEDIATEK_MMAP__

#define HIFSYS_BASE			0xFA000000 //for PCIe/USB
#define ETHDMASYS_BASE			0xFB000000 //for I2S/PCM/GDMA/HSDMA/FE/GMAC

#define HIFSYS_PCI_BASE                 0xFA140000
#define HIFSYS_USB_HOST_BASE            0xFA1C0000
#define HIFSYS_USB_HOST2_BASE           0xFA240000


#define ETHDMASYS_SYSCTL_BASE           0x1B000000
#define ETHDMASYS_RBUS_MATRIXCTL_BASE   0x1B000400
#define ETHDMASYS_I2S_BASE              0x1B000A00
#define ETHDMASYS_PCM_BASE              0x1B002000
#define ETHDMASYS_GDMA_BASE             0x1B002800
#define ETHDMASYS_HS_DMA_BASE           0x1B007000
#define ETHDMASYS_FRAME_ENGINE_BASE     0x1B100000
#define ETHDMASYS_ETH_SW_BASE		    0x1B110000
#define ETHDMASYS_CRYPTO_ENGINE_BASE	0x1B240000


//for backward-compatible
#define RALINK_FRAME_ENGINE_BASE	    ETHDMASYS_FRAME_ENGINE_BASE
#define RALINK_SYSCTL_BASE		        ETHDMASYS_SYSCTL_BASE
#define RALINK_ETH_SW_BASE		        ETHDMASYS_ETH_SW_BASE
#define RALINK_GDMA_BASE                ETHDMASYS_GDMA_BASE
#define RALINK_HS_DMA_BASE              ETHDMASYS_HS_DMA_BASE
#define RALINK_11N_MAC_BASE		        0	//unused for rt_rdm usage

//Reset Control Register
#define RSTCTL_SYS_RST			        (1<<0)
#define RSTCTL_MCM_RST			        (1<<2)
#define RSTCTL_HSDMA_RST		        (1<<5)
#define RSTCTL_FE_RST			        (1<<6)
#define RSTCTL_SPDIF_RST		        (1<<7)
#define RSTCTL_TIMER_RST		        (1<<8)
#define RSTCTL_CIRQ_RST			        (1<<9)
#define RSTCTL_MC_RST			        (1<<10)
#define RSTCTL_PCM_RST			        (1<<11)
#define RSTCTL_GPIO_RST			        (1<<13)
#define RSTCTL_GDMA_RST			        (1<<14)
#define RSTCTL_NAND_RST			        (1<<15)
#define RSTCTL_I2C_RST			        (1<<16)
#define RSTCTL_I2S_RST			        (1<<17)
#define RSTCTL_SPI_RST			        (1<<18)
#define RSTCTL_UART0_RST		        (1<<19)
#define RSTCTL_UART1_RST		        (1<<20)
#define RSTCTL_UART2_RST		        (1<<21)
#define RSTCTL_UPHY_RST			        (1<<22)
#define RSTCTL_ETH_RST			        (1<<23)
#define RSTCTL_PCIE0_RST		        (1<<24)
#define RSTCTL_PCIE1_RST		        (1<<25)
#define RSTCTL_PCIE2_RST		        (1<<26)
#define RSTCTL_AUX_STCK_RST		        (1<<28)
#define RSTCTL_CRYPT_RST		        (1<<29)
#define RSTCTL_SDXC_RST			        (1<<30)
#define RSTCTL_PWM_RST			        (1<<31)

//for backward-compatible
#define RALINK_FE_RST			        RSTCTL_FE_RST
#define SYSCFG1			                (RALINK_SYSCTL_BASE + 0x14)

/*
 * System Controller
 *   Offset
 *   0x10  -- SYSCFG		System Configuration Register
 *   0x30  -- CLKCFG1		Clock Configuration Register
 *   0x34  -- RSTCTRL		Reset Control Register
 *   0x38  -- RSTSTAT		Reset Status Register 
 *   0x60  -- GPIOMODE		GPIO Mode Control Register 
 */
#define RT2880_SYS_CNTL_BASE			(RALINK_SYSCTL_BASE)
#define RT2880_CHIP_REV_ID_REG			(RT2880_SYS_CNTL_BASE+0x0c)
#define RT2880_SYSCFG_REG			(RT2880_SYS_CNTL_BASE+0x10)
#define RT2880_SYSCFG1_REG			(RT2880_SYS_CNTL_BASE+0x14)
#define RT2880_CLKCFG1_REG			    (RT2880_SYS_CNTL_BASE+0x30)
#define RT2880_RSTCTRL_REG			    (RT2880_SYS_CNTL_BASE+0x34)
#define RT2880_RSTSTAT_REG			    (RT2880_SYS_CNTL_BASE+0x38)
#define RT2880_SYSCLKCFG_REG			(RT2880_SYS_CNTL_BASE+0x3c)
#define RT2880_GPIOMODE_REG			    (RT2880_SYS_CNTL_BASE+0x60)

#define RT2880_PRGIO_ADDR               (RALINK_SYSCTL_BASE + 0x600) // Programmable I/O
#define RT2880_REG_PIOINT               (RT2880_PRGIO_ADDR + 0)
#define RT2880_REG_PIOEDGE              (RT2880_PRGIO_ADDR + 0x04)
#define RT2880_REG_PIORENA              (RT2880_PRGIO_ADDR + 0x08)
#define RT2880_REG_PIOFENA              (RT2880_PRGIO_ADDR + 0x0C)
#define RT2880_REG_PIODATA              (RT2880_PRGIO_ADDR + 0x20)
#define RT2880_REG_PIODIR               (RT2880_PRGIO_ADDR + 0x24)




#endif // __RALINK_MMAP__
