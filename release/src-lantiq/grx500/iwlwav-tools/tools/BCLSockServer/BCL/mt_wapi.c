/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

// wls_chacc.cpp: implementation of the wireless chip access
//
//////////////////////////////////////////////////////////////////////

#include "mtlkinc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if _MSC_VER >= 1400
#pragma warning (disable: 4996)
#endif

#include "mt_types.h"
#include "mt_bcl.h"
#include "mt_util.h"
#include "mt_wapi.h"
#include "mt_bcl_funcs.h"
#include "mt_chacc.h"
#include "../BCLSockServer.h"    // MTLK specific definitions

#ifndef _WIN32

#include <unistd.h>

#endif

#if defined OPENRG
#include <mgt_client.h>
//#include "rg/pkg/util/mgt_client.h"  /* OpenRG management API*/
//#include "rg/pkg/samples/tutorial/process/mgt_utils.h"  /* Sample OpenRG rg_conf management API, using sets */
#endif /*OPENRG*/


#ifdef _WIN32
#include "Registry.h"
#endif

#ifdef UNDER_CE
#include <windows.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef  unsigned int MT_RET;
#define MT_RET_OK 0
#define MT_RET_FAIL 1


/* This is the unit number that passed to the MIPS for read/write  */
#define HYP_MEMORY  0  /* access the hyperion memory */
#define HYP_PIF_CS0 1  /* access to Prometheus chip 0 */
#define HYP_PIF_CS1 2  /* access to Prometheus chip 1 */
#define HYP_SPI     3  /* access to Athena chip via SPI */

#define HYP_MAX_MSG_SIZE 64



/*************************** SPI *****************************************/
#define BASE_ADD_PHY_RX_TD		 0xAF300000

#define MSPI_ADDR_WR_DATA					    (BASE_ADD_PHY_RX_TD + 0x10044)	
#define MSPI_ADDR_CLEAN_FIFO				    (BASE_ADD_PHY_RX_TD + 0x10048)	
#define MSPI_ADDR_RD_MSPI_STATUS			    (BASE_ADD_PHY_RX_TD + 0x10048)	
#define MSPI_ADDR_STATUS_PARAMETERS			    (BASE_ADD_PHY_RX_TD + 0x1004C)	
#define MSPI_RD_DATA						    (BASE_ADD_PHY_RX_TD + 0x10050)	
#define MSPI_ATHENA_READ                        0
#define MSPI_ATHENA_WRITE                       1

#define MSPI_DUMMY_READ	                        0x00
#define MSPI_READ_CMD 		                    0x8000

#define MSPI_ADDR_STATUS_RD_DATA_VALID		    0x01
#define MSPI_ADDR_STATUS_DONE				    0x02
#define MSPI_ADDR_STATUS_TXFIFO_EMPTY		    0x04
#define MSPI_ADDR_BYTES_IN_TX_FIFO			    0x38
#define MSPI_ADDR_RX_OVERRUN				    0x40
#define MSPI_ADDR_TX_OVERRUN				    0x80

void MSPI_Delay(MT_UINT32 statusCheck);
void MSPI_WritePage (MT_UBYTE page);
void MSPI_WriteAthena(MT_UBYTE page, MT_UBYTE address, MT_UBYTE data);
void MSPI_WriteAthena(MT_UBYTE page, MT_UBYTE address, MT_UBYTE data);

/************************************************************************/


/**************************** PIF ***************************************/


#define PIF_WRITE_VALUE        				    (BASE_ADD_PHY_RX_TD + 0x10054)	
#define PIF_CTRL        					    (BASE_ADD_PHY_RX_TD + 0x10058)	
#define PIF_CLEAR_OP         				    (BASE_ADD_PHY_RX_TD + 0x1005C)	
#define PIF_STATUS          				    (BASE_ADD_PHY_RX_TD + 0x10060)	
#define PIF_READ_VALUE         				    (BASE_ADD_PHY_RX_TD + 0x10064)	


#define RFIC_READ_CMD  0x00
#define RFIC_WRITE_CMD 0x80




/*************************** TEST32 wrappers **************************/

#define HIU_READ_TYPE 	        0
#define HIU_WRITE_TYPE	        1
#define HIU_READ_CON_TYPE       2
#define HIU_WRITE_CON_TYPE      3  /*For same data to all*/
#define HIU_WRITE_CONDATA_TYPE  4 /*changing Data*/


#define MT_API_HIU_PROM_WRITE                   7           
#define MT_API_HIU_PROM_READ                    8           
#define MT_TEST32_MIPS_VAR_WATCH_CRC		   15
#define MT_TEST32_HIU_READ_WRITE_1			   71
#define MT_TEST32_HIU_DOWNLOAD				   73
#define MT_TEST32_HIU_DOWNLOAD_SPECIAL		   74
#define MT_TEST32_HIU_READFROMFILE_SPECIAL	   75
#define MT_TEST32_ATHENA_READ_WRITE	           76
#define MT_TEST32_RFIB_READ_WRITE	           77
#define MT_TEST32_MSPI_ATHENA  	               78
#define MT_TEST32_HIU_REGISTER_MODE			   79


#define TEST32_MAX_PARAMS       10
#define TEST32_MAX_RET_VALUES   10

/************************* Address Mapping ********************************/
#define NUMBER_OF_UNITS 9
#define FGPA_CHIP_MODULE        0
#define HYPERION_CHIP_MODULE    1
#define HIU_BASE_ADDR           2
#define UMIPS_BASE_ADDR         3

    /*FPGA ba	    Hyperion ba	HostAddress	MipsAddress*/
MT_UINT32 gAddressMapping[9][4] = {
    {0x10000000,	0x10000000,	0x06000000, 0xA6000000},     /* MAC         	0 */
	{0x11000000,	0x10000000,	0x00000000, 0xA0000000},     /* Upper CPU   	1 */
	{0x12000000,	0x10000000,	0x10000000, 0xA0080000},     /* Lower CPU   	2 */
	{0x13000000,	0x10000000,	0x00000000, 0xAF000000},     /* PHY_LDPC    	3 */
	{0x13000010,	0x10000000,	0x40000000, 0xAF400000},     /* PHY_RXBE    	4 */
	{0x13000020,	0x10000000,	0x20000000, 0xA0000000},     /* PHY_RXSLISER	5 */
	{0x13000030,	0x10000000,	0x30000000, 0xAF300000},     /* PHY_RXTD    	6 */
	{0x13000040,	0x10000000,	0x60000000, 0xAF600000},     /* PHY_RXFD    	7 */
    {0x13000050,	0x10000000,	0x50000000, 0xAF500000}      /* PHY_TX      	8 */
    };							


/* local functions */
MT_UINT32  Test32_72(MT_UBYTE access_type,MT_UINT32 chip ,MT_UINT32 Address,MT_UBYTE data_size,MT_UINT32 Data,MT_UBYTE num_data_elements,MT_UINT32 * table);
MT_RET CheckMask(MT_UINT32 mask,MT_UBYTE * shift);

/* download functions */
void MIPS_SendBuffer( MT_UBYTE chip); 
MT_UINT32 SendFile(char * filename);
MT_UBYTE ReadFileParmterToReadfrom_WL(void);




/* Translate from the chip module+ address from the 860 point of view to an address in the upper mips */
MT_UINT32 Host2MipsAdd(MT_UINT32 chipModule,MT_UINT32 address)
{
    MT_UINT32 i,baseAddr,offset;
    
    /* take special care of lower cpu system interface */
    if ((address & 0xFFFF0000) ==  0x10180000)
        return 0xA0190000 | (address & 0x0000FFFF);
    
    baseAddr = address & 0xFF000000;
    offset   = address & 0x00FFFFFF;
    /* check if this is a logical unit */
    if (chipModule < NUMBER_OF_UNITS)
        return gAddressMapping[chipModule][UMIPS_BASE_ADDR] + offset;
    /* check if this is an HIU address */
    for (i = 0; i < NUMBER_OF_UNITS; i++)
    {
        if (gAddressMapping[i][HIU_BASE_ADDR] == baseAddr)
            return gAddressMapping[i][UMIPS_BASE_ADDR] + offset;
    }
    /* this is a MIPS address */
    return address;    
}




MT_UINT32 HypVersion(void)
{
#if 0
    MT_UINT32 pos = 0;
    MT_UINT32 version[2];
    char str[1024];
    //HypGetVersion(version,version+1,str,1024);
    BCL_SkipReturnValue();
    pos += strCpy(&MT_BCL_cmd_line[pos],str);
    pos += strCpy(&MT_BCL_cmd_line[pos]," Major=");
    pos +=unsignedToAscii(version[0], MT_BCL_cmd_line+pos);    
    pos += strCpy(&MT_BCL_cmd_line[pos]," Minor=");
    pos +=unsignedToAscii(version[1], MT_BCL_cmd_line+pos);    
#endif
    return 0;

}


MT_UINT32  version(MT_UBYTE type)
{
    MT_UINT32 ver[]= {BCL_VERSION_MAJOR,BCL_VERSION_MINOR,BCL_VERSION_BUILD};
    if (type == 2)
    {
        return dmem(4, ver, 3);
    }
    return HypVersion();

}



MT_UINT32 Test32()
{
	char *params;
	int numConvErr;
    char retAsHex = 1;
    MT_UINT32 i, numOfParams, parameters[TEST32_MAX_PARAMS];
    MT_UINT32 numOfRetValues=0,RetValues[TEST32_MAX_RET_VALUES];
    MT_UINT32 pos = 0;
    

    params = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
	            												/*character right after the DMEM_read command. */
	/* get the number of parameters */
    numOfParams = wordsCount(params,' ');
    if (numOfParams > TEST32_MAX_PARAMS || numOfParams == 0)
    {
		error("Wrong number of arguments");
		return 0;
    }
	
	for (i=0;i<numOfParams;i++)
    {
	    parameters[i] = asciiToUnsigned(params,' ', &numConvErr);
        if(numConvErr)
        {
            MT_BCL_error_occured = MT_TRUE;
            strCpy(MT_BCL_cmd_line, "Invalid value entered for argument ");
            unsignedToAscii(i + 1, &MT_BCL_cmd_line[strLen(MT_BCL_cmd_line)]);
            return MT_FALSE;
        }
		params = skipDelimiters( skipWord(params,' '),' ' );
    }

    /* defualt values: */
    numOfRetValues = 1;
    RetValues[0] = 0xFFFFFFFF;

    /* test32 switch case */
    switch (parameters[0])
    {
    
    case MT_API_HIU_PROM_WRITE:
        RetValues[0]  = PromWrite( parameters[3],parameters[4], parameters[5], parameters[6]);
        break;
    case MT_API_HIU_PROM_READ:
        RetValues[0]  = PromRead( parameters[3],parameters[4], parameters[5]);
        break;
    case MT_TEST32_MIPS_VAR_WATCH_CRC:
        RetValues[0] = (MT_UINT32)-1;
        break;
    case MT_TEST32_HIU_READ_WRITE_1:
        if (numOfParams == 8)
            RetValues[0] = Test32_72((MT_UBYTE)parameters[1],parameters[2],parameters[3],(MT_UBYTE)parameters[4],parameters[5],(MT_UBYTE)parameters[6],MT_BCL_item_list[parameters[7]]);
        else
            error("Wrong number of arguments for test32 72");
        break;
    case MT_TEST32_HIU_DOWNLOAD:
        MIPS_SendBuffer( (MT_UBYTE) parameters[1]);
        RetValues[0] = MT_RET_OK;
        break;
    case MT_TEST32_HIU_DOWNLOAD_SPECIAL:
        SendFile(__FTP_PATH__"/ssram_wr.bin");
        break;
    case MT_TEST32_HIU_READFROMFILE_SPECIAL:
        RetValues[0] =  ReadFileParmterToReadfrom_WL();
        break;
    case MT_TEST32_MSPI_ATHENA:
        if (parameters[1] == MSPI_ATHENA_WRITE)
        {
            AthenaWrite((MT_UBYTE)parameters[2], (MT_UBYTE)parameters[3], (MT_UBYTE)parameters[4]);
            RetValues[0] = MT_RET_OK;
        } else 
        {
            if (parameters[1] == MSPI_ATHENA_READ) 
            {
                RetValues[0] =  AthenaRead((MT_UBYTE)parameters[2], (MT_UBYTE)parameters[3]);
            } else {
                error("ERROR: at MSPI ATHENA test32 the first paramete");
            }
        }
        
        break;
    case MT_TEST32_HIU_REGISTER_MODE:
        RetValues[0] = 0;
        break;
    default:
        error("Command is not supported under PCI interface");
    }

    if (!MT_BCL_error_occured)
    {
        for (i = 0; i< numOfRetValues;i++)
        {
            if (retAsHex)
                unsignedToAsciiHex(RetValues[i], MT_BCL_cmd_line+pos);    /*return signed hexdecimal */
            else
                unsignedToAscii(RetValues[i], MT_BCL_cmd_line+pos);    /*return signed decimal */
            pos += strLen(&MT_BCL_cmd_line[pos]);	/*Update the position on the output buffer MT_BCL_cmd_line. */
            MT_BCL_cmd_line[pos++] = ' ';	/*Plant the seperating space. */
        }
        if (pos == 0) pos++;
        MT_BCL_cmd_line[--pos] = '\0';	/*Plant the string termination suffix. */
    }
    return 1;
}

MT_UINT32  Test32_72(MT_UBYTE access_type,MT_UINT32 chip ,MT_UINT32 Address,MT_UBYTE data_size,MT_UINT32 Data,MT_UBYTE num_data_elements,MT_UINT32 * table)
{
    
    /* we support only 32 bit */
    if (data_size != 4)
    {
        error("Test32 72 : only 4 byte data type is supported");
        return MT_FALSE;
    }
    /* translate address */
    Address = Host2MipsAdd(chip,Address);

    switch (access_type)
    {
    case HIU_READ_TYPE:
        return HypPciRead(HYP_MEMORY, Address,1, table);
        break;
    case HIU_WRITE_TYPE:
        return HypPciWrite(HYP_MEMORY, Address,1, &Data);
        break;
    case HIU_READ_CON_TYPE:
        return HypPciRead(HYP_MEMORY, Address,num_data_elements, table);
        break;
    case HIU_WRITE_CON_TYPE:
        return HypPciWrite(HYP_MEMORY, Address,num_data_elements, table);
        break;
    case HIU_WRITE_CONDATA_TYPE:
        return HypPciWrite(HYP_MEMORY, Address,num_data_elements, table);
        break;
    default:
        error("Test32 72: wrong type");

    }
    return MT_TRUE;
    
}

/* check mask validation and find the shift*/
MT_RET CheckMask(MT_UINT32 mask,MT_UBYTE * shift)
{
    MT_UINT32 tempMask = mask;
    MT_UBYTE maskShift = 0;
    /* find the first '1' in the mask */
    while (tempMask && ((tempMask & 1) == 0))
    {
        tempMask = tempMask >> 1;
        maskShift++;
    }
    
    /* make sure that the mask is valid */
    while (tempMask && (tempMask & 1))
    {
        tempMask = tempMask >> 1;
    }

    if (tempMask != 0)
    {
	 	error("Wrong mask\n");
        return MT_RET_FAIL;
    }

    *shift = maskShift;
    return MT_RET_OK;

}



MT_UINT32 Hyp_Read(MT_UINT32 address, MT_UINT32 mask, MT_UBYTE size)
{
    MT_UINT32 buffer[HYP_MAX_MSG_SIZE];
    MT_UBYTE maskShift;
    MT_UINT32 i;
    if (CheckMask(mask,&maskShift) != MT_RET_OK)
       return MT_RET_FAIL;

    if (size > HYP_MAX_MSG_SIZE)
    {
        error("Hyp_Read: size over limit");
        return MT_RET_FAIL;
    }

	address = Host2MipsAdd(NUMBER_OF_UNITS,address);	
	// read data
    HypPciRead(HYP_MEMORY, address, size, buffer);
    // apply mask
    if (mask != 0xFFFFFFFF)
    {
        for (i = 0 ; i < size; i++)
            buffer[i] = (buffer[i] & mask) >> maskShift;
    }

    
    if (size > 1)
    {
        /* print memory to output */
        dmem(4, buffer,size);
    }
    
    return buffer[0];
}

MT_UINT32 Hyp_Write (MT_UINT32 address,  MT_UINT32 mask , MT_UINT32 data)
{
    MT_UINT32 prevData;
    MT_UBYTE maskShift;
    if (CheckMask(mask,&maskShift) != MT_RET_OK)
       return MT_RET_FAIL;
	address = Host2MipsAdd(NUMBER_OF_UNITS,address);	
    
    // apply mask
    if (mask != 0xFFFFFFFF)
    {
        HypPciRead(HYP_MEMORY, address, 1, &prevData);
        data = (prevData & ~mask) | ((data << maskShift) & mask);
    }

    // read data
    HypPciWrite(HYP_MEMORY, address, 1, &data);
    return MT_RET_OK;
}

MT_UINT32 Hyp_WriteBuffer (MT_UINT32 address,  MT_UINT32 mask ,MT_UBYTE size, MT_UINT32 *pData)
{
    MT_UINT32 buffer[HYP_MAX_MSG_SIZE];
    MT_UBYTE maskShift;
    MT_UINT32 i;
	address = Host2MipsAdd(NUMBER_OF_UNITS,address);	

    if (CheckMask(mask,&maskShift) != MT_RET_OK)
       return MT_RET_FAIL;
    
    // apply mask
    if (mask != 0xFFFFFFFF)
    {
        HypPciRead(HYP_MEMORY, address, size, buffer);
        for (i = 0; i < size; i++)
        {
            pData[i] = (buffer[i] & ~mask) | ((pData[i] << maskShift) & mask);
        }
    }

    // read data
    HypPciWrite(HYP_MEMORY, address, size, pData);
    return MT_RET_OK;
}





/********************** Promothous Access functions **************************/
MT_UINT32 PromRead( MT_UINT32 chip_select,MT_UINT32 prom_address, MT_UINT32 mask)
{
    MT_UINT32 control_word;
    MT_UINT32 status;
    MT_UINT32 counter = 0;
    MT_UINT32 value;
    MT_UBYTE maskShift;
    
    if (CheckMask(mask,&maskShift) != MT_RET_OK)
       return MT_RET_FAIL;

    /* Write to PIF cntrl word */
    control_word = RFIC_READ_CMD | (chip_select << 6) | prom_address;
    HypPciWrite(HYP_MEMORY,PIF_CTRL,1,&control_word);
 
    /* Read status done     */
	do
	{
	    if (HypPciRead(HYP_MEMORY,PIF_STATUS,1,&status) != MT_RET_OK)
        {
            error("PromRead: PIF status error");
            return MT_RET_FAIL;
        }
        counter++;
	    if (counter > 1000)
        {
            error("PromRead: PIF s status register not ready");
            return MT_RET_FAIL;

        }
    } while ((status & 0x1) == 0);
    
    
    /* Read reg value */
    HypPciRead(HYP_MEMORY,PIF_READ_VALUE,1,&value);    
    /* Clear operation   */
    control_word = 0x2;
    HypPciWrite(HYP_MEMORY,PIF_CLEAR_OP,1,&control_word);
    
    /* enforce mask */
    value = (value & mask) >>  maskShift;

    return value;
}

MT_UINT32 PromWrite(MT_UINT32 chip_select,MT_UINT32 prom_address, MT_UINT32 mask,MT_UINT32 data)
{
    MT_UINT32 control_word;
    MT_UINT32 status;
    MT_UINT32 counter = 0;
    MT_UINT32 prevData = 0;
    MT_UBYTE maskShift;

    if (CheckMask(mask,&maskShift) != MT_RET_OK)
        return MT_RET_FAIL;

    /* handle mask if any */
    if (mask != 0xFFFFFFFF)
    {
        prevData = PromRead(chip_select,prom_address,0xFFFFFFFF);
	    data = ((data << maskShift) & mask) | (prevData & ~mask);
    
    }
 
    /* Write data */
    HypPciWrite(HYP_MEMORY,PIF_WRITE_VALUE,1,&data);

    /* Write to PIF cntrl word  */
    control_word = RFIC_WRITE_CMD | (chip_select << 6) | prom_address;
    HypPciWrite(HYP_MEMORY,PIF_CTRL,1,&control_word);

    /* Read status done     */
	do
	{
	    if (HypPciRead(HYP_MEMORY,PIF_STATUS,1,&status) != MT_RET_OK)
        {
            error("PromRead: PIF status error");
            return MT_RET_FAIL;
        }
        counter++;
	    if (counter > 1000)
        {
            error("PromRead: PIF s status register not ready");
            return MT_RET_FAIL;

        }
    } while ((status & 0x2) == 0);
    
            
    /* Clear operation */
    control_word = 0x2;
    HypPciWrite(HYP_MEMORY,PIF_CLEAR_OP,1,&control_word);
    return MT_RET_OK;
}

 

/*****************************************************************************
*   Athena SPI access functions
*****************************************************************************/
void MSPI_Delay(MT_UINT32 statusCheck)
{
	int max	 =0;
	
	int val_delay=50;
	MT_UINT32 readData=0;
	
	/* wait for the spi ready flag */
    while(max<val_delay)
    {
        
        HypPciRead(HYP_MEMORY, MSPI_ADDR_STATUS_PARAMETERS,1, &readData);
        if (statusCheck  & (readData))
            break;
        max++;
    }
    if (max == val_delay)
        MT_TRACE("Mspi not ready !!!!!!!!\n \n");		
}



void MSPI_WritePage (MT_UBYTE page)
{
	
	MT_UINT32 DataWrite=(0x7e00 ) |  (MT_UINT16)page;
	
	MSPI_Delay(MSPI_ADDR_STATUS_DONE);
    HypPciWrite(HYP_MEMORY, MSPI_ADDR_WR_DATA,1, &DataWrite);

}



MT_UBYTE AthenaWrite(MT_UBYTE page, MT_UBYTE address, MT_UBYTE data)
{
    MT_UINT32 DataWrite= ((MT_UINT16)(address<<1)<<8) | (MT_UINT16)data;
    MSPI_WritePage(page);
    MSPI_Delay(MSPI_ADDR_STATUS_DONE);
    HypPciWrite(HYP_MEMORY, MSPI_ADDR_WR_DATA,1, &DataWrite);
    return MT_RET_OK;
}



MT_UBYTE  AthenaRead(MT_UBYTE page, MT_UBYTE address)
{
	MT_UINT32 DataWrite=0;
	MT_UINT32 readData=0;
	
	DataWrite =   ((MT_UINT16)(address<<1)<<8) | MSPI_READ_CMD;
	
	/* write page */
	MSPI_WritePage(page);
	MSPI_Delay(MSPI_ADDR_STATUS_DONE);	
    HypPciWrite(HYP_MEMORY, MSPI_ADDR_WR_DATA,1, &DataWrite);
	
	MSPI_Delay(MSPI_ADDR_STATUS_RD_DATA_VALID);	
    HypPciRead(HYP_MEMORY, MSPI_RD_DATA,1, &readData);
	return (MT_UBYTE)readData;
}


MT_UINT32 ReadWord(FILE* file,MT_UINT32 * pValue)
{
    MT_UBYTE temp[4];;
    if (fread(temp,1,4,file) == 4)
    {
        *pValue =(temp[0])|(temp[1]<<8)|(temp[2]<<16)|(temp[3]<<24);
        return MT_RET_OK;
    }
    return MT_RET_FAIL;
}

 
MT_UINT32 SendFile(char * fileName) 
{
    FILE*    file;
    MT_UINT32 TypeModule_chip=0;
    MT_UINT32 addressToWrite=0;
    MT_UINT32 lenToWrite=0;
    MT_UINT32 ulWordCount = 0;
    MT_UINT32 buffer[HYP_MAX_MSG_SIZE];
    /* Send the entire bin file, one page at a time: */
    
    mt_print(2,"file name '%s'\n",fileName);
    file = fopen(fileName, "rb");
    
    if (!file)
    {
        error("SendBinFile: failed to open file");
        return MT_RET_FAIL;
    }
    
    /* Get module,address and length */
    while ( ReadWord(file,&TypeModule_chip) == MT_RET_OK &&
            ReadWord(file,&addressToWrite)  == MT_RET_OK &&
            ReadWord(file,&lenToWrite)      == MT_RET_OK)
    {
        
        mt_print(2,"Send File: ChipModule=0x%lX, address=0x%lX, length=%ld\n",TypeModule_chip,addressToWrite,lenToWrite);
        /* Translate addresses */
        addressToWrite = Host2MipsAdd( TypeModule_chip, addressToWrite);
        mt_print(2,"Send File: translated address=0x%lX\n",addressToWrite);

        ulWordCount = 0;
        while(ReadWord(file,buffer+ulWordCount) == MT_RET_OK)
        {
            ulWordCount++;
            if (ulWordCount == lenToWrite) 
            {
                HypPciWrite(HYP_MEMORY,addressToWrite,(MT_UBYTE)ulWordCount,buffer);
                ulWordCount = 0;
                /* exit the while loop */
                break;
                
            }
            else if(ulWordCount == HYP_MAX_MSG_SIZE ) 
            {
                /* Write the Data and clculet the Addres*/
                HypPciWrite(HYP_MEMORY,addressToWrite,(MT_UBYTE)ulWordCount,buffer);
                addressToWrite+= ulWordCount*4;
                lenToWrite -= ulWordCount;					
                ulWordCount = 0;
           }
        } /* finshed section writing */
    } /* finished all */
      
    fclose(file);
    unsignedToAsciiHex(0, MT_BCL_cmd_line);
    return MT_RET_OK;
    
}


void MIPS_SendBuffer( MT_UBYTE chip) 
{
    FILE*    file;
    char * fileName;
    MT_UINT32 ulWordCount = 0;
    MT_UINT32 ulPageCount = 0;
	MT_UINT32 addressToWrite = 0;   /* Default value for FPGA, and UM on chip */
    MT_UINT32 buffer[HYP_MAX_MSG_SIZE];

   	 
    
    /* Send the entire bin file, one page at a time: */
    if (chip == 1) 
    {
        fileName = __FTP_PATH__"/contr_UM.bin";
        addressToWrite = 0x00000000; //0xA0000000;
    } 
    else 
    {
        if (chip == 2) 
        {
            fileName = __FTP_PATH__"/contr_LM.bin";
            addressToWrite = 0x10000000; //0xA0080000;
        } 
        else
        {
            error("MIPS_SendBuffer: ERROR CPU Number .\n");
            return;
        }
    }

    // Convert address
    addressToWrite = Host2MipsAdd(NUMBER_OF_UNITS, addressToWrite);
    
    file = fopen(fileName, "rb");
    if (!file) 
    {
        error("MIPS_SendBuffer: can't open file");
        return;
    }

    for(;;)
    {
        ulWordCount=0;
        while (ulWordCount < HYP_MAX_MSG_SIZE ) 
        {
            // TODO: Is endianness correct in ReadWord?
            if (ReadWord(file,buffer+ulWordCount) != MT_RET_OK) {
                mt_print(2,"ReadWord returned != MT_RET_OK\n");
                break;
            }
            ulWordCount++;
        }
        /* Send a non-empty buffer */
        if (ulWordCount > 0) {
            /* Send the page to To MIPS . */
            mt_print(2,"Sending page %lud to MIPS. \n", ulPageCount++);
            HypPciWrite(HYP_MEMORY,addressToWrite,(MT_UBYTE)ulWordCount,buffer);
            mt_print(2,"HypPciWrite %08lx finished \n", addressToWrite);
            addressToWrite += (ulWordCount * 4);
            ulWordCount=0;
        }
        else
        {
            break;
        }
        
    } 
    fclose(file);
    
    mt_print(2," Finished sending SW To MIPS CPU .\n");
}

static int _mt_fwrite(const void *data, size_t size, size_t count, FILE *stream)
{
    return (count == fwrite(data, size, count, stream));
}

MT_UBYTE ReadFileParmterToReadfrom_WL()
{
    
    FILE*    fp;
    FILE*    fpout;
    MT_UINT32 chip_mode=0;
    MT_UINT32 Add_val=0;
    MT_UINT32 len=0;
    MT_UINT32 read=0;
    MT_UINT32 val_countread=0;
    MT_UINT32 buffer[HYP_MAX_MSG_SIZE];
    MT_UINT32 is_written = FALSE;
    
    fp = fopen(__FTP_PATH__"/ReadFrom.txt", "r");
    
    if (!fp)
    {
        error("could not open '"__FTP_PATH__"/ReadFrom.txt' for reading");
        return 0;
    }

    fpout = fopen(__FTP_PATH__"/dataread.bin","wb");
    if (!fpout)
    {
        error("could not open '"__FTP_PATH__"/dataread.bin' for writing");
        fclose(fp);
        return 0;
    }
    
    while ((read = fscanf(fp,"%08lx %08lx %08lx",&chip_mode,&Add_val,&len)) == 3) 
    {
        /*Write to file the chip add len*/
        if (!_mt_fwrite(&chip_mode, sizeof(MT_UINT32), 1, fpout)) goto exit;
        if (!_mt_fwrite(&Add_val, sizeof(MT_UINT32), 1, fpout)) goto exit;
        if (!_mt_fwrite(&len, sizeof(MT_UINT32), 1, fpout)) goto exit;

        /* Translate addresses */
        Add_val = Host2MipsAdd(chip_mode, Add_val);
        
        /*start to read data HW*/
        while (len) 
        {
            val_countread = MIN(len, HYP_MAX_MSG_SIZE);
            HypPciRead(HYP_MEMORY, Add_val, (MT_UBYTE)val_countread, buffer);
            len -= val_countread;
            Add_val += HYP_MAX_MSG_SIZE * sizeof(MT_UINT32);

            if (!_mt_fwrite(buffer, sizeof(MT_UINT32), val_countread, fpout)) goto exit;
        }
    }

    is_written = TRUE;

exit:
    fclose(fp);
    fclose(fpout);

    if (!is_written)
    {
        mt_print(2,"failed to write parameters into 'dataread.bin'\n");
        error("failed to write parameters into 'dataread.bin'");
        return MT_RET_FAIL;
    }

    if (read != EOF)
    {
        mt_print(2,"failed to read parameters from 'ReadFrom.txt'\n");
        error("failed to read parameters from 'ReadFrom.txt'");
        return MT_RET_FAIL;
    }

    return MT_RET_OK;
}


/****** OpenRG Support: RG_conf access functions ******/


/***********************************************************************
                         RG_conf_get
***********************************************************************/
/**
 * @brief           Get the value of a parameter from openrg and return it 
 *                  in the same itemlist used for the path.
 * @return          
 * @param           path - (in) full path to the parameter, (out) paramter value
 * 
 * @remarks         
***********************************************************************/
MT_UINT32 RG_conf_get(MT_BYTE* path)
{
#if OPENRG
    /* Get the value from openrg. */
    char* result = NULL;
    int   nResult;
    char  cmd[1024];
    
    //mt_print(2,"%08x: %s\n", (int)path, path);
   
    sprintf(cmd, "conf_get \"%s\"", path);
    mt_print(2,"%s\n", cmd);
    //fflush(stdout);
    
    nResult = mgt_command(cmd, &result);
    /* DEBUG: */
    mt_print(2,"RESULT: >%s<\n", result);
    //mt_print(2,"RESULT: >[%d]\n", nResult);
    
    
    if (nResult != 0) {
        //error("rg_conf_get returned failure status.");
        strcpy(path, "Error: rg_conf_get returned failure status.");
        return MT_RET_FAIL;
    }
    /* Copy the result back to the original variable (which is an itemlist) */
    if (result != NULL) {
        strcpy(path, result);
        
        /* Free memory allocated to result */ 
        free(result);
    } else {
        strcpy(path, "Error: String not found");
        return MT_RET_FAIL;
    }
        
    return MT_RET_OK;
#elif _WIN32
    char response[255];
    if (Registry_GetKey((LPCTSTR)path, (LPCTSTR)response, MAX_PATH))
    {
        strcpy(path,response);
        return MT_RET_OK;
    }
    return MT_RET_FAIL;
#else
    char cmd[1024];
    int len;
    
    FILE * file;
    sprintf(cmd,"../bcl/bcl_util.tcl getparam \"%s\" 0 > /tmp/param.tmp",path);
    mt_print(2,"command %s\n",cmd);
    system(cmd);
    len = 0;
    file = fopen("/tmp/param.tmp","r");
    if (file)
    {
        len = fread(path,1,128,file);
        fclose(file);
    }
    if (len > 0) 
    {
        path[len] = 0; // set end of string 
    }
    else
    {
       path[0] = 0; /* item not found ...*/	
    }

    return MT_RET_OK;
#endif /*OPENRG*/
}



/***********************************************************************
                         RG_conf_set
***********************************************************************/
/**
 * @brief           Set a value to an openrg rg_conf parameter
 * @return          
 * @param           path - the full path to the parameter: e.g. /Metalink/Ndis/MyParam
 * @param           value - The value, as a string
 * 
 * @remarks         Requires reconf and reboot for changes to be seen.
 * @TODO            Add a different func for int or string parameters? Or implement in wrapper?
***********************************************************************/
MT_UINT32 RG_conf_set(MT_BYTE* path, MT_BYTE* value)
{
#if OPENRG
    /* Set the value to openrg. */
    char* result = NULL;
    int   nResult;
    /* TODO: Use Jungo's str_sprintf allocate functions? */ 
    char  cmd[1024];
    sprintf(cmd, "conf_replace \"%s\" \"%s\"", path, value);
    nResult = mgt_command(cmd, &result);
    if (nResult != 0) {
        //error("rg_conf_set returned failure status.");
        strcpy(path, "Error: rg_conf_set returned failure status.");
        return MT_RET_FAIL;
    }

    if (result != NULL) {
        /* Free memory allocated to result */ 
        free(result);
    }
    return MT_RET_OK;
#elif _WIN32
    
    return Registry_SetKey((LPCTSTR)path, (LPCTSTR)value) ? MT_RET_OK : MT_RET_FAIL;
#else 
    char  cmd[1024];
    sprintf(cmd, "echo %s = %s >> /tmp/bcl_params", path, value);
	system(cmd);
	return MT_RET_OK;
#endif 
}


/***********************************************************************
                         OpenRG_reboot
***********************************************************************/
/**
 * @brief           Call openrg reboot API
 * @return          Function shouldn't return, because host reboots.
***********************************************************************/
MT_UINT32 OpenRG_reboot()
{
#if _WIN32      
    error("Command is not supported without OpenRG based Host.");
    return MT_RET_FAIL;
#else
    system("reboot &");
    /* 16/5/06 Avri - we can't ignore the response. The BBS will try to send the command again and again .... */
    /* Ignore return status, we should be rebooting now... */
    //while (1) {
    //    sleep(1);
    //    mt_print(2,"BCL is waiting for reboot.\n");
    //}
    return MT_RET_OK;
#endif /*OPENRG*/
}


/***********************************************************************
                         OpenRG_reconf
***********************************************************************/
/**
 * @brief           Make the rg_conf_set changes permanent on flash.
 * @return          
 * @param           TimeFrame - Usually use 1: reconf now.
 * 
 * @remarks         This must be called after RG_conf_set and before reboot.
***********************************************************************/
MT_UINT32 OpenRG_reconf(MT_UINT32 TimeFrame)
{
#if OPENRG
    /* Call openrg reconf with the passed parameter. */
    char* result = NULL;
    int   nResult;
    char  cmd[128];
    sprintf(cmd, "reconf %ld", TimeFrame);
    nResult = mgt_command(cmd, &result);
    if (nResult != 0) {
        error("Command returned failure status.");
        return MT_RET_FAIL;
    }

    /* Free memory allocated to result */ 
    free(result);

    return OpenRG_reboot();

#elif UNDER_CE
#define CE_PROCESS_TIMEOUT 5000

    BOOLEAN bResult;    
    PROCESS_INFORMATION sProcessInfo;

    //
    // Stop:
    //  wzctool -disablewzcsvc devices -stop ZCF1:
    //  ndisconfig adapter del PCI\WLANPLUS1
    //  
    // Start:
    //  ndisconfig adapter add WLANPLUS PCI\WLANPLUS1
    //  devices -start Drivers\BuiltIn\ZeroConfig 
    //  wzctool -enablewzcsvc
    // 

    // wzctool -disablewzcsvc devices -stop ZCF1:
    bResult =  CreateProcess(_T("wzctool"), _T("-disablewzcsvc devices -stop ZCF1:"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &sProcessInfo);
    WaitForSingleObject(sProcessInfo.hProcess, CE_PROCESS_TIMEOUT);

    if (!bResult)
    {
        error("Couldn't disable wireless zero config service!\n");
        return MT_RET_FAIL;
    }
    
    // ndisconfig adapter del PCI\WLANPLUS1
    bResult =  CreateProcess(_T("ndisconfig"), _T("adapter del PCI\\WLANPLUS1"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &sProcessInfo);
    WaitForSingleObject(sProcessInfo.hProcess, CE_PROCESS_TIMEOUT);

    if (!bResult)
    {
        error("Couldn't disable Metalink wireless card driver!\n");
        return MT_RET_FAIL;
    }

    // ndisconfig adapter add WLANPLUS PCI\WLANPLUS1
    bResult =  CreateProcess(_T("ndisconfig"), _T("adapter add WLANPLUS PCI\\WLANPLUS1"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &sProcessInfo);
    WaitForSingleObject(sProcessInfo.hProcess, CE_PROCESS_TIMEOUT);

    if (!bResult)
    {
        error("Couldn't enable Metalink wireless card driver!\n");
        return MT_RET_FAIL;
    }

    // devices -start Drivers\BuiltIn\ZeroConfig
    bResult =  CreateProcess(_T("devices"), _T("-start Drivers\\BuiltIn\\ZeroConfig"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &sProcessInfo);
    WaitForSingleObject(sProcessInfo.hProcess, CE_PROCESS_TIMEOUT);

    if (!bResult)
    {
        error("Couldn't run wireless zero config service!\n");
        return MT_RET_FAIL;
    }

    // wzctool -enablewzcsvc
    bResult =  CreateProcess(_T("wzctool"), _T("-enablewzcsvc"), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &sProcessInfo);
    WaitForSingleObject(sProcessInfo.hProcess, CE_PROCESS_TIMEOUT);

    if (!bResult)
    {
        error("Couldn't enable wireless zero config service!\n");
        return MT_RET_FAIL;
    }

#elif _WIN32
	system("devcon disable PCI\\VEN_1A30");
	system("devcon enable PCI\\VEN_1A30");
#else
    FILE * file;
    char buffer[1024]={0};
    int len = 0;
    system("../bcl/bcl_util.tcl loadconfig /tmp/bcl_params 0 > /tmp/bcl_out");
    file = fopen("/tmp/bcl_out","r");
    if (file)
    {
        len = fread(buffer,1,128,file);
        fclose(file);
    }
    if (len > 0) 
    {
        BCL_SkipReturnValue();      
        strcpy(MT_BCL_cmd_line, buffer);
        
    }
	return MT_RET_OK;	
#endif /*OPENRG*/
    return MT_RET_OK;

}

/***********************************************************************
                         TFTP_Get
***********************************************************************/
/**
 * @brief           Get a file through tftp
 * @return          
 * @param           source_path - format: "filename"
 * @param           dest_path   - format: "/tmp/filename"
 * 
 * @remarks         Only works on openrg host.
 *                  Use the IP of the PC connected to BCL, and append filename
 *                  (Get the IP from /tmp/connected_ip.txt)
***********************************************************************/
MT_UINT32 TFTP_Get(MT_BYTE* source_path, MT_BYTE* dest_path)
{
#ifndef  _WIN32
    int   nResult;

    char  cmd[1024];
    char  destPath[1024];
    char  ip[256], safe_ip[256];
    FILE* fileIP;
    fileIP = fopen(__FTP_PATH__"/connected_ip.txt", "r");
    if (!fileIP)
    {
        strcpy((char*) source_path, "Error: Couldn't get IP of connected target.");
        return MT_RET_FAIL;
    }

    memset(ip, 0, sizeof(ip));
    nResult = fscanf(fileIP, "%255s\n", ip);
    fclose(fileIP);

    if (nResult != 1) {
        strcpy((char*) source_path, "Error: Couldn't get IP of connected target.");
        return MT_RET_FAIL;
    }

    strip_unsafe_chars(safe_ip, ip, sizeof(safe_ip));

    /* Use the /tmp path unless a full path is given */
    if (dest_path[0] != '/') {
        snprintf(destPath, sizeof(destPath), "%s/%s", __FTP_PATH__, dest_path);
    } else {
        snprintf(destPath, sizeof(destPath), "%s", dest_path);
    }

    snprintf(cmd, sizeof(cmd), "tftp -g -r \"%s\" -l \"%s\" \"%s\" ", source_path, destPath, safe_ip);

    mt_print(2,"Executing: %s\n", cmd);
    nResult = system(cmd);
    if (nResult != 0) {
        strcpy((char*) source_path, "Error: tftp get returned failure status.");
        return MT_RET_FAIL;
    }

    return MT_RET_OK;
#else
    error("Command is not supported in windows.");
    return MT_RET_FAIL;
#endif /*#ifndef  _WIN32*/
}

/***********************************************************************
                         TFTP_Put
***********************************************************************/
/**
 * @brief           Put a file through tftp
 * @return          
 * @param           source_path (param is filename) - format (after manipuation): "/tmp/filename"
 * @param           dest_path   (param is filename) - format (after manipuation): "192.168.1.100:filename"
 * 
 * @remarks         Only works on openrg host.
 *                  Use the IP of the PC connected to BCL, and append filename
 *                  (Get the IP from /tmp/connected_ip.txt)
***********************************************************************/
MT_UINT32 TFTP_Put(MT_BYTE* source_path, MT_BYTE* dest_path)
{
#ifndef  _WIN32
    int   nResult;

    char  cmd[1024];
    char  srcPath[1024];
    char  ip[256], safe_ip[256];
    FILE* fileIP;
    fileIP = fopen(__FTP_PATH__"/connected_ip.txt", "r");
    if (!fileIP)
    {
        strcpy((char*) source_path, "Error: Couldn't get IP of connected target.");
        return MT_RET_FAIL;
    }

    memset(ip, 0, sizeof(ip));
    nResult = fscanf(fileIP, "%255s\n", ip);
    fclose(fileIP);

    if (nResult != 1) {
        strcpy((char*) source_path, "Error: Couldn't get IP of connected target.");
        return MT_RET_FAIL;
    }

    strip_unsafe_chars(safe_ip, ip, sizeof(safe_ip));

    /* Use the /tmp path unless a full path is given */
    if (source_path[0] != '/') {
        snprintf(srcPath, sizeof(srcPath), "%s/%s", __FTP_PATH__, source_path);
    } else {
        snprintf(srcPath, sizeof(srcPath), "%s", source_path);
    }
    
    snprintf(cmd, sizeof(cmd), "tftp -p -l \"%s\" -r \"%s\" \"%s\" ", srcPath, dest_path, safe_ip);
    mt_print(2,"Executing: %s\n", cmd);
    nResult = system(cmd);
    if (nResult != 0) {
        strcpy((char*) source_path, "Error: tftp put returned failure status.");
        return MT_RET_FAIL;
    }

    return MT_RET_OK;
#else
    error("Command is not supported in windows");
    return MT_RET_FAIL;
#endif 
}

// Executes a command, without returning result to the client
MT_UINT32 Shell_Command(void)
{
    int   nResult = -1;
    char * command;
#ifdef UNDER_CE
    WCHAR wcCommand[MT_COMMAND_MAX_SIZE];
    PROCESS_INFORMATION sProcessInfo;
#endif

    command = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */

#ifdef UNDER_CE
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, command, strlen(command), (LPWSTR)wcCommand, MT_COMMAND_MAX_SIZE);
    wcCommand[strlen(command)] = L'\0';

    // XXX Need to specife command as a parameter of "cmd /C "
    nResult =  CreateProcess(wcCommand,
                              NULL,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_NEW_CONSOLE,
                              NULL,
                              NULL,
                              NULL,
                              &sProcessInfo);
#else
    nResult =system(command);
#endif
    return nResult;   
}

#ifdef _WIN32
#define SYSTEM_CMD_TMP_FILE "c:\\bcl_system_cmd.tmp"
#else
#define SYSTEM_CMD_TMP_FILE "/tmp/bcl_system_cmd.tmp"
#endif

// Executes a command, and sends the stdout to the client
MT_UINT32 System_Command(void)
{
    int nResult;
	size_t bytesRead;
	FILE* file;
	char* fullCmd;
#ifdef UNDER_CE
    WCHAR wcCommand[MT_COMMAND_MAX_SIZE];
    PROCESS_INFORMATION sProcessInfo;
#endif

    char * command = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */;
	fullCmd = (char *)malloc(strlen(command)+32);
    if (!fullCmd) {
      error("error can't allocate memory");
      return MT_RET_FAIL;
    }

#ifdef UNDER_CE
    DeleteFile(_T(SYSTEM_CMD_TMP_FILE));
#else
	remove(SYSTEM_CMD_TMP_FILE);
#endif

	sprintf(fullCmd, "%s >%s", command, SYSTEM_CMD_TMP_FILE);

#ifdef UNDER_CE
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, command, strlen(command), (LPWSTR)wcCommand, MT_COMMAND_MAX_SIZE);
    wcCommand[strlen(command)] = L'\0';

    /// XXXX Need to specife command as a parameter of "cmd /C "
    nResult =  CreateProcess(wcCommand,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        NULL,
        &sProcessInfo);
#else
    nResult = system(fullCmd);
#endif

    free(fullCmd);

	if (nResult == -1)
	{
		error("system call returned an error");
		return MT_RET_FAIL;
	}
    
	file = fopen(SYSTEM_CMD_TMP_FILE, "r");
    if (!file)
    {
        error("System - can't open stdout temporary file");
        return MT_RET_FAIL;
    }
	bytesRead = fread(MT_BCL_cmd_line, 1, 255, file);
	MT_BCL_cmd_line[bytesRead] = 0; // Add null termination
	fclose(file);

#ifdef UNDER_CE
    DeleteFile(_T(SYSTEM_CMD_TMP_FILE));
#else
    remove(SYSTEM_CMD_TMP_FILE);
#endif

	return MT_RET_OK;
}

MT_UINT32 RG_Command(void)
{
#if OPENRG
    int   nResult = -1;
    char * cmd;
    char* result = NULL;

    cmd = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
    nResult = mgt_command(cmd, &result);
    
    BCL_SkipReturnValue();    
    
    if (nResult != 0) {
        strcpy(MT_BCL_cmd_line, "Error: RG_Command returned failure status.");
        return MT_RET_FAIL;
    }

    /* Copy the result back to the original variable (which is an itemlist) */
    if (result != NULL) {
        strcpy(MT_BCL_cmd_line, result);
        
        /* Free memory allocated to result */ 
        free(result);

    } 
    else 
    {
        strcpy(MT_BCL_cmd_line, "");
    }
        
    return MT_RET_OK;
#else
    error("Command is not supported without OpenRG based Host.");
    return MT_RET_FAIL;
#endif /*OPENRG*/
}

MT_UINT32 Read_File(void)
{
    char * params;
    int intParams[2];
    MT_UINT32 numOfParams;
    int numConvErr,i,actualRead;
    FILE * file;
    char usage[] = "Wrong usage: ReadFile offset length fileNameString\nLength 0 will read the maximum";

    params = skipDelimiters(skipWord(MT_BCL_cmd_line,' '),' ');	/*params will now point to the first non-space  */
    /*character right after the DMEM_read command. */
    /* get the number of parameters */
    numOfParams = wordsCount(params,' ');
    
    /* we should get at least three params: offset length file_name */
    if (numOfParams < 3)
    {
        error(usage);
        return MT_RET_FAIL;
    }
    
    for (i=0;i<2;i++)
    {
        intParams[i] = asciiToUnsigned(params, ' ', &numConvErr);
        params = skipWord(params, ' ');
        params = skipDelimiters(params, ' ');
        if (numConvErr)
        {
            error(usage);
            return MT_RET_FAIL;
        }
        
    }
    
    /* params - hold the file name
       intParams[0] - offset
       intParams[1] - length 
        put data into BCL_SkipReturnValue */
    if (intParams[1] > 255 || intParams[1] == 0)
        intParams[1] = 255;
    BCL_SkipReturnValue();
    file = fopen(params, "rt");
    if (!file)
    {
        error("ReadFile - can't open file");
        return MT_RET_FAIL;
    }

    if (fseek(file,intParams[0],SEEK_SET))
    {
        error("ReadFile - can't seek in file");
        fclose(file);
        return MT_RET_FAIL;
    }
    
    actualRead = fread(MT_BCL_cmd_line,1,intParams[1],file);
    MT_BCL_cmd_line[actualRead] = 0; /* close the string */
    fclose(file);
    return MT_RET_OK;
}

#define CPU_UPPER 0
#define CPU_LOWER 1
#define MT_NO_MASK 0xFFFFFFFF
static const MT_UINT32 ChipvarAnchorAddr[2] = {0xA601FEDC, 0xA601FED8};
#define RETURN_ERROR(x) { error(x); return MT_RET_FAIL; }

// Convert LMAC address to UMAC if needed
static MT_UINT32 GetCPUAddr(MT_UBYTE cpu, MT_UINT32 addr)
{
	MT_UINT32 prefix = addr & 0xFF000000;
	if (prefix == 0xA0000000 || prefix == 0x80000000)
	{
		MT_UINT32 suffix = addr & 0x00FFFFFF;
		if (cpu == CPU_UPPER) return addr;
		else return (0xA0080000 + suffix);
	}
	else if (prefix == 0xA6000000) return addr;
	else
	{
		error("Unknown address space.\n");
		return 0xA6000000; // prevent errors
	}
}

#define MAX_DWORDS_IN_BCL_COMMAND 64

// Reverse back buffer of 16-bits or 8-bits values
static void ReverseBuffer(MT_UINT32* buffer, MT_UINT32 count, MT_UINT32 size)
{
	MT_UINT32 i, dword=10, tmp;
	MT_UINT32* ptr = buffer;
	if (size == 4) return; // No need to do anything with 32-bits, there's already reversed from driver
	if (*((MT_UBYTE*)(&dword)) == 10) return; // We're in little-endian, there's no need to do reversing

	if (size == 1)
	{
		for (i=0; i<count; i+=4)
		{
			tmp = *ptr;
			*ptr = ((tmp&0xFF)<<24) | ((tmp&0xFF00)<<8) | ((tmp&0xFF0000)>>8) | ((tmp&0xFF000000)>>24);
			mt_print(2,"In index %d: converted %08X to %08X\n", i, tmp, *ptr);
			ptr++;
		}
	}
	else if (size == 2)
	{
		for (i=0; i<count; i+=2)
		{
			tmp = *ptr;
			*ptr = ((tmp&0xFFFF)<<16) | ((tmp&0xFFFF0000)>>16);
			mt_print(2,"In index %d: converted %08X to %08X\n", i, tmp, *ptr);
			ptr++;
		}
	}
	else error("Wrong size in chipvar structure");
}

// Do a smart HypPciRead, allowing non-aligned addresses and big buffers
static MT_UINT32 HypReadNonAligned(MT_UINT32 baseAddr, MT_UINT32 numBytes, MT_UBYTE* outBuf, MT_UINT32 elemSize)
{
	MT_UINT32 alignedStartAddr, numDWORDs, alignedEndAddr, prefixBytes, i;
	MT_UINT32 curAddr, dwordsToRead, bytesRead, bytesCopied;
	MT_UINT32 tmpBuffer[MAX_DWORDS_IN_BCL_COMMAND];
	MT_UBYTE* pTmpBufBytes = (MT_UBYTE*)tmpBuffer;
	alignedStartAddr = baseAddr;
	prefixBytes = baseAddr % 4;
	mt_print(2,"prefixbytes = %d\n",prefixBytes);
	if (prefixBytes > 0) alignedStartAddr -= prefixBytes;
	alignedEndAddr = baseAddr + numBytes + prefixBytes;
	if (alignedEndAddr % 4) alignedEndAddr += 4 - (alignedEndAddr % 4);
	numDWORDs = (alignedEndAddr - alignedStartAddr) / 4;
	curAddr = alignedStartAddr;
	bytesCopied = 0;
	for (i=0; i<numDWORDs; i += MAX_DWORDS_IN_BCL_COMMAND)
	{
		dwordsToRead = numDWORDs - i;
		if (dwordsToRead > MAX_DWORDS_IN_BCL_COMMAND) dwordsToRead = MAX_DWORDS_IN_BCL_COMMAND;
		if (HypPciRead(HYP_MEMORY, curAddr, (MT_UBYTE)dwordsToRead, tmpBuffer) != MT_RET_OK) return MT_RET_FAIL;
		bytesRead = dwordsToRead << 2;
		ReverseBuffer(tmpBuffer, bytesRead/elemSize, elemSize);

		// In last chunk, we might need to memcpy less bytes than what we read:
		if (i == 0 && prefixBytes > 0) bytesRead -= prefixBytes;
		if (bytesCopied + bytesRead > numBytes) bytesRead = numBytes - bytesCopied;
		mt_print(2,"bytesRead = %d\n", bytesRead);
		if (i == 0 && prefixBytes > 0) memcpy(outBuf, pTmpBufBytes+prefixBytes, bytesRead);
		else memcpy(outBuf, pTmpBufBytes, bytesRead);
		curAddr += bytesRead;
		outBuf += bytesRead;
		bytesCopied += bytesRead;
	}
	return MT_RET_OK;
}

// Read one DWORD from MIPS memory (LCPU/UCPU)
static MT_UINT32 HypReadMipsDWORD(MT_UBYTE cpu, MT_UINT32 address)
{
	MT_UINT32 data;
	HypPciRead(HYP_MEMORY, GetCPUAddr(cpu, address), 1, &data);
	return data;
}

typedef struct
{
	MT_UINT32 addr;
	MT_UINT32 maxOffset;
	MT_UINT32 size;
} chipVar_t;

// ChipVarGet - read chipvar value from LCPU/UCPU.
// First check that the CPU supports chipvar.
// Find the chipvar base address.
// Check that the index given is not too big
// Then, read the chipvar using the lowlevel function
//
MT_UINT32 Chipvar_Get(MT_UBYTE cpu, MT_UINT32 index, MT_UINT32 offset, MT_UINT32 count)
{
	MT_UINT32 isValid, baseAddr, countAddr, chipVarCount, startAddr;
	MT_UINT32 buffer[256];
	chipVar_t chipVar;

	if (cpu > 1) RETURN_ERROR("Wrong CPU - 0 for UMAC and 1 for LMAC");
	// First, we check if the CPU supports chipvar:

 	isValid = HypReadMipsDWORD(CPU_UPPER, ChipvarAnchorAddr[cpu]+12);
	if (isValid != 1) RETURN_ERROR("CPU doesn't support chipvar.");

	baseAddr = HypReadMipsDWORD(CPU_UPPER, ChipvarAnchorAddr[cpu]);

	countAddr = HypReadMipsDWORD(cpu, baseAddr);
	chipVarCount = HypReadMipsDWORD(cpu, countAddr);
	mt_print(2,"Chipvar count is: %d\n", chipVarCount);
	if (index >= chipVarCount) RETURN_ERROR("Illegal chipvar index");

	HypPciRead(HYP_MEMORY, GetCPUAddr(cpu, baseAddr + index*12), 3, (MT_UINT32*)&chipVar);

	if (offset + count > chipVar.maxOffset) RETURN_ERROR("Offset + count is too large");
	// Do the chipvar read !
	startAddr = GetCPUAddr(cpu, chipVar.addr) + offset*chipVar.size;

	HypReadNonAligned(startAddr, count * chipVar.size, (MT_UBYTE*)buffer, chipVar.size);

	dmem((MT_UBYTE)chipVar.size,buffer,(MT_UBYTE)count);

	return MT_RET_OK;
}

