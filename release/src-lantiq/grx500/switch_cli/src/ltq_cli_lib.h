/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _IFX_CLI_LIB_H
#define _IFX_CLI_LIB_H
typedef int (*command_fkt)(int argc, char *argv[], int fd, int numPar);

#define CONFIG_LTQ_TEST 0

typedef struct {
	char *Name;
	command_fkt fkt;
} GSW_CommandTable_t;

#if defined(IOCTL_PCTOOL) && IOCTL_PCTOOL
#include <gsw_init.h>
extern GSW_API_HANDLE sdev;
int cli_ioctl(GSW_API_HANDLE fd, u32 request, void *par);

#ifdef SWITCH_SERVER
extern FILE *g_fp;
FILE *get_fp(void);

#define printf(...)		\
	do { FILE *fp;			\
		fp = (FILE *) get_fp();	\
		fprintf(fp, __VA_ARGS__); \
	} while (0)
#else
#define printf printf
#endif

#else
#include <linux/types.h>
#include <asm/byteorder.h>
#include <lantiq_gsw_api.h>
int cli_ioctl(void *fd, unsigned long int request, void *par);
#endif

#ifndef BIT
#define BIT(nr)		(1UL << nr)
#endif
#ifndef GENMASK
#define BITS_PER_LONG	(sizeof(0UL) * 8)
#define GENMASK(h, l)	\
	(((~0UL) - (1UL << (l)) + 1) & (~0UL >> (BITS_PER_LONG - 1 - (h))))
#endif

#define GSWIP_3_0 0x30
#define GSWIP_3_1 0x31
#define GSWIP_3_2 0x32
#define VRSN_CMP(x, y)	((int)((x) & 0xFF) - (int)((y) & 0xFF))

extern unsigned int gswip_version;

int scanParamArg(int argc, char *argv[], char *name, int size, void *param);
int findStringParam(int argc, char *argv[], char *name);
int scanMAC_Arg(int argc, char *argv[], char *name, unsigned char *param);
int scanIPv4_Arg(int argc, char *argv[], char *name, unsigned int *param);
int scanIPv6_Arg(int argc, char *argv[], char *name, unsigned short *param);
int checkValidMAC_Address(unsigned char *pMAC);
void printMAC_Address(unsigned char *pMAC);
int checkValidIPv6_Address(unsigned short *ip);
void printIPv6_Address(unsigned short *ip);
void printIPv4_Address(unsigned int ip);
void printHex32Value(char *name, unsigned int value, unsigned int bitmapIndicator);
int scanPMAC_Arg(int argc, char *argv[], char *name, unsigned char *param);
int scanPMAP_Arg(int argc, char *argv[], char *name, unsigned char *param);



command_fkt fktGet(char *name);

#endif /* _IFX_CLI_LIB_H */
