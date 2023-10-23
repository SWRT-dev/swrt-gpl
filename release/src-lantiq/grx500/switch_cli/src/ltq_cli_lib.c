/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ltq_cli_lib.h"
#include "gsw_tbl.h"

#ifndef IOCTL_PCTOOL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#endif

#if ! defined (CLI_DUMP_CALL) && ! defined (IOCTL_PCTOOL)
int cli_ioctl(void *fd, unsigned long int request, void *par)
{
	return ioctl((int)fd, request, par);
}
#endif


#if defined(IOCTL_PCTOOL) && IOCTL_PCTOOL
int cli_ioctl(GSW_API_HANDLE fd, u32 request, void *par)
{
	/*For PC_tool Handle is stored as global variable*/
	fd = sdev;
	return gsw_api_kioctl(fd, request, par);
}

#endif

unsigned int t_olower(unsigned int ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch + 'a' - 'A';

	return ch;
}

unsigned int xstrncasecmp(const char *s1, const char *s2, unsigned int n)
{
	int c1, c2;

	if (!s1 || !s2) return 1;

	for (; n > 0; s1++, s2++, --n) {
		c1 = t_olower(*s1);
		c2 = t_olower(*s2);

		if (c1 != c2) {
			return (c1 - c2);
		} else if (c1 == '\0') {
			return 0;
		}
	}

	return 0;
}

static char *findArgParam(int argc, char *argv[], char *name)
{
	int i;
	size_t len;

	len = strlen(name);

	for (i = 0; i < argc; i++) {
		if (xstrncasecmp(name, argv[i], len) == 0) {
			if (strlen(argv[i]) > (len + 1)) {
				if ('=' == argv[i][len]) {
					return argv[i] + len + 1;
				}
			}
		}
	}

	return NULL;
}

int findStringParam(int argc, char *argv[], char *name)
{
	int i;

	/* search for all programm parameter for a command name */
	for (i = 1; i < argc; i++) {
		if (xstrncasecmp(argv[i], name, strlen(name)) == 0) {
			return 1;
		}
	}

	return 0;
}

static int convert_mac_adr_str(char *mac_adr_str, unsigned char *mac_adr_ptr)
{
	char *str_ptr = mac_adr_str;
	char *endptr;
	int i;
	unsigned long int val;

	if (strlen(mac_adr_str) != (12 + 5)) {
		printf("ERROR: Invalid length of address string!\n");
		return 0;
	}

	for (i = 0; i < 6; i++) {
		val = strtoul(str_ptr, &endptr, 16);

		if ((*endptr != 0) && (*endptr != ':') && (*endptr != '-'))
			return 0;

		*(mac_adr_ptr + i) = (unsigned char)(val & 0xFF);
		str_ptr = endptr + 1;
	}

	return 1;
}

static int convert_pmapper_adr_str(char *pmap_adr_str, unsigned char *pmap_adr_ptr)
{
	char *str_ptr = pmap_adr_str;
	char *endptr;
	int i;
	unsigned long int val;

	for (i = 0; i < 73; i++) {
		val = strtoul(str_ptr, &endptr, 10);

		if ((*endptr != 0) && (*endptr != ',') && (*endptr != '-'))
			return 0;

		*(pmap_adr_ptr + i) = (unsigned char)(val & 0xFF);
		str_ptr = endptr + 1;
	}

	return 1;
}

static int convert_pmac_adr_str(char *mac_adr_str, unsigned char *mac_adr_ptr)
{
	char *str_ptr = mac_adr_str;
	char *endptr;
	int i;
	unsigned long int val;

	if (strlen(mac_adr_str) != (16 + 7)) {
		printf("ERROR: Invalid length of address string!\n");
		return 0;
	}

	for (i = 0; i < 8; i++) {
		val = strtoul(str_ptr, &endptr, 16);

		if ((*endptr != 0) && (*endptr != ':') && (*endptr != '-'))
			return 0;

		*(mac_adr_ptr + i) = (unsigned char)(val & 0xFF);
		str_ptr = endptr + 1;
	}

	return 1;
}
static int convert_ipv4_str(char *ip_adr_str, unsigned int *ip_adr_ptr)
{
	char *endptr;
	int i, val = 0;
	unsigned int data = 0;

	if (strlen(ip_adr_str) > (12 + 3)) {
		printf("ERROR: Invalid length of IP address string!\n");
		return (-1);
	}

	for (i = 0; i < 4; i++) {
		val = strtoul(ip_adr_str, &endptr, 10);

		if (*endptr && (*endptr != '.') && (*endptr != '-'))
			return 0;

		//printf("String Value = %03d\n",val);
		data |= (val << ((3 - i) * 8));
		//printf("String Value = %08x\n",data);
		ip_adr_str = endptr + 1;
	}

	*(ip_adr_ptr) = data;
	return 1;
}

static int convert_ipv6_str(char *ip_adr_str, unsigned short *ip_adr_ptr)
{
	char *endptr;
	int i ;
	unsigned int val = 0;

	if (strlen(ip_adr_str) > (32 + 7)) {
		printf("ERROR: Invalid length of IP V6 address string!\n");
		return (-1);
	}

	for (i = 0; i < 8; i++) {
		val = strtoul(ip_adr_str, &endptr, 16);

		if (*endptr && (*endptr != ':'))
			return 0;

		*ip_adr_ptr++ = val;
		ip_adr_str = endptr + 1;
	}

	return 1;
}

int scanParamArg(int argc, char *argv[], char *name, int size, void *param)
{
	unsigned long int tmp;
	char *endptr;
	char *ptr = findArgParam(argc, argv, name);

	if (ptr == NULL) return 0;

	/* check if the given parameter value are the boolean "LTQ_TRUE" / "LTQ_FALSE" */

	if (0 == xstrncasecmp(ptr, "LTQ_TRUE", strlen(ptr))) {

		tmp = 1;
	} else if (0 == xstrncasecmp(ptr, "LTQ_FALSE", strlen(ptr))) {

		tmp = 0;
	} else {
		/* scan for a number */
		tmp = strtoul(ptr, &endptr, 0);

		/* parameter detection does not work in case there are more character after the provided number */
		if (*endptr != '\0')
			return 0;
	}

	if (size <= 4)
		size *= 8;

	if (size == 8)
		*((char *)param) = (char)tmp;
	else if (size == 16)
		*((short *)param) = (short)tmp;
	else if (size == 32)
		*((int *)param) = (int)tmp;

	return 1;
}

int scanMAC_Arg(int argc, char *argv[], char *name, unsigned char *param)
{
	char *ptr = findArgParam(argc, argv, name);

	if (ptr == NULL) return 0;

	return convert_mac_adr_str(ptr, param);
}

int scanPMAP_Arg(int argc, char *argv[], char *name, unsigned char *param)
{
	char *ptr = findArgParam(argc, argv, name);

	if (ptr == NULL) return 0;

	return convert_pmapper_adr_str(ptr, param);
}

int scanPMAC_Arg(int argc, char *argv[], char *name, unsigned char *param)
{
	char *ptr = findArgParam(argc, argv, name);

	if (ptr == NULL) return 0;

	return convert_pmac_adr_str(ptr, param);
}

int scanIPv4_Arg(int argc, char *argv[], char *name, unsigned int *param)
{
	char *ptr = findArgParam(argc, argv, name);
	//printf("test 0x%08X\n", ptr);

	if (ptr == NULL) return 0;

	return convert_ipv4_str(ptr, param);
}

int scanIPv6_Arg(int argc, char *argv[], char *name, unsigned short *param)
{
	char *ptr = findArgParam(argc, argv, name);

	if (ptr == NULL) return 0;

	return convert_ipv6_str(ptr, param);
}

int checkValidMAC_Address(unsigned char *pMAC)
{
	if ((pMAC[0] == 0) &&
	    (pMAC[1] == 0) &&
	    (pMAC[2] == 0) &&
	    (pMAC[3] == 0) &&
	    (pMAC[4] == 0) &&
	    (pMAC[5] == 0))
		return (-1);

	return 0;
}

void printMAC_Address(unsigned char *pMAC)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x",
	       pMAC[0],
	       pMAC[1],
	       pMAC[2],
	       pMAC[3],
	       pMAC[4],
	       pMAC[5]);
}

int checkValidIPv6_Address(unsigned short *ip)
{
	if ((ip[0] == 0) && (ip[1] == 0) &&
	    (ip[2] == 0) && (ip[3] == 0) &&
	    (ip[4] == 0) && (ip[5] == 0) &&
	    (ip[6] == 0) && (ip[7] == 0))
		return (-1);

	return 0;
}

void printIPv6_Address(unsigned short *ip)
{
	printf("%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X |",
	       ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7]);
}

void printIPv4_Address(unsigned int ip)
{
	char buffer[25];

	sprintf(buffer, "%03d.%03d.%03d.%03d",
		(ip >> 24) & 0xFF,
		(ip >> 16) & 0xFF,
		(ip >> 8) & 0xFF,
		ip & 0xFF);

	printf(" %15s |", buffer);
}

void printHex32Value(char *name, unsigned int value, unsigned int bitmapIndicator)
{
	if (bitmapIndicator == 0) {
		printf("\t%40s:\t%u", name, (unsigned int)value);

		if (value > 9) {
			/* Make an additional hex printout for larger values */
			printf(" (0x%0x)", (unsigned int)value);
		}
	} else {
		int i;
		int bitset = 0;

		if ((1 << (bitmapIndicator - 1)) & value) {

			/* Make an additional hex printout for larger values */
			unsigned int tmp = (1 << bitmapIndicator) - 1;

			if (tmp == 0) tmp -= 1;

			value = value & tmp;
			printf("\t%40s:\t0x%0x (Bits: ", name, (unsigned int)value);

			/* The highest data bit is set and is used as bitmap indicator, therefore
			   represent the data as bitmap as well. */
			for (i = bitmapIndicator - 2; i >= 0; i--) {
				if ((1 << i) & value) {
					if (bitset) printf(",");

					printf("%d", i);
					bitset = 1;
				}
			}
		}

		printf(")");
	}

	printf("\n");
}

