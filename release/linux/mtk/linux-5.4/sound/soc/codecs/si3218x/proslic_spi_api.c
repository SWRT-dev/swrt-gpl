// SPDX-License-Identifier: GPL-2.0
/*
 * spi.h
 * SPI driver implementation file
 *
 * Author(s): 
 * laj
 *
 * Distributed by: 
 * Silicon Laboratories, Inc
 *
 * File Description:
 * This is the implementation file for the SPI driver used 
 * in the ProSLIC demonstration code. It calls the library
 * that initializes and talks to the proslic motherboard.
 *
 * Dependancies:
 *
 */
//#include "../iowrapper.h" // This is the motherboard interface library
#include <linux/kernel.h>
#include <linux/delay.h>
#include "config_inc/si_voice_datatypes.h"
#include "inc/si_voice_ctrl.h"
#include "spi.h"
#include "proslic_sys.h"

/*
** Function: SPI_Init
**
** Description: 
** Initializes the SPI interface
**
** Input Parameters: 
** none
**
** Return:
** none
*/
int SPI_Init (ctrl_S *hSpi){
	int ret;

	ret = proslic_spi_setup();

	return 0;
}

/*
** Function: spiGci_ResetWrapper
**
** Description: 
** Sets the reset pin of the ProSLIC
*/
int ctrl_ResetWrapper (void *hSpiGci, int status){
	
	printk("ctrl_ResetWrapper");	
	proslic_reset(NULL, status);

	return 0;
}

/*
** SPI/GCI register read 
**
** Description: 
** Reads a single ProSLIC register
**
** Input Parameters: 
** channel: ProSLIC channel to read from
** regAddr: Address of register to read
** return data: data to read from register
**
** Return:
** none
*/
uInt8 ctrl_ReadRegisterWrapper (void *hSpiGci, uInt8 channel, uInt8 regAddr){
	

	return proslic_read_register(hSpiGci,channel,regAddr);
}


/*
** Function: spiGci_WriteRegisterWrapper 
**
** Description: 
** Writes a single ProSLIC register
**
** Input Parameters: 
** channel: ProSLIC channel to write to
** address: Address of register to write
** data: data to write to register
**
** Return:
** none
*/
int ctrl_WriteRegisterWrapper (void *hSpiGci, unsigned char channel, unsigned char regAddr, unsigned char data){

	proslic_write_register(hSpiGci,channel,regAddr, data);
	return 0;
}


/*
** Function: SPI_ReadRAMWrapper
**
** Description: 
** Reads a single ProSLIC RAM location
**
** Input Parameters: 
** channel: ProSLIC channel to read from
** address: Address of RAM location to read
** pData: data to read from RAM location
**
** Return:
** none
*/
ramData ctrl_ReadRAMWrapper (void *hSpiGci, unsigned char channel, unsigned short ramAddr){
	ramData data;

	data = proslic_read_ram(hSpiGci,channel,ramAddr);

	return data;
}


/*
** Function: SPI_WriteRAMWrapper
**
** Description: 
** Writes a single ProSLIC RAM location
**
** Input Parameters: 
** channel: ProSLIC channel to write to
** address: Address of RAM location to write
** data: data to write to RAM location
**
** Return:
** none
*/
int ctrl_WriteRAMWrapper (void *hSpiGci, unsigned char channel, unsigned short ramAddr, ramData data){

	proslic_write_ram(hSpiGci,channel,ramAddr,data);

	return 0;
}



/*
** $Log: proslic_spi_api.c,v $
** Revision 1.11  2008/07/24 21:06:16  lajordan
** no message
**
** Revision 1.10  2008/03/19 18:20:09  lajordan
** no message
**
** Revision 1.9  2007/06/04 16:35:51  lajordan
** added profiling
**
** Revision 1.8  2007/02/27 19:25:48  lajordan
** updated api
**
** Revision 1.7  2007/02/05 23:43:27  lajordan
** fixed register access func
**
** Revision 1.6  2006/11/22 21:38:19  lajordan
** broadcast added
**
** Revision 1.5  2006/07/21 20:31:26  lajordan
** fixed cant connect message
**
** Revision 1.4  2006/07/19 18:15:56  lajordan
** fixed spi init
**
** Revision 1.3  2006/07/18 21:50:16  lajordan
** removed extraneous endif
**
** Revision 1.2  2006/07/18 21:48:51  lajordan
** added 16 bit accesses example code
**
** Revision 1.1.1.1  2006/07/13 20:26:08  lajordan
** no message
**
** Revision 1.1.1.1  2006/07/06 22:06:23  lajordan
** no message
**
** Revision 1.3  2006/06/29 19:11:18  laj
** lpt driver completed
**
** Revision 1.2  2006/06/21 23:46:58  laj
** register reads/writes added
**
** Revision 1.1  2006/06/21 23:18:07  laj
** no message
**
** Revision 1.1  2006/06/21 22:42:26  laj
** new api style
**
** Revision 1.3  2005/11/14 17:42:34  laj
** added SPI_init
**
** Revision 1.2  2005/11/07 23:20:54  laj
** took out extra spaces
**
*/
