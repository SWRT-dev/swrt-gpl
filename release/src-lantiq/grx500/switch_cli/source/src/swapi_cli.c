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
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "switch_cli_config.h"
#endif

unsigned int gswip_version = 0;
static const GSW_CommandTable_t *pCmdTable = NULL;
static int bHelpText = 0;
static int cmdTableIdx = 0;

static unsigned int t_olower(unsigned int ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch + 'a' - 'A';

	return ch;
}

static void find_sub_string(char *strfind)
{
	char *p1, *p2, *p3;
	unsigned int i = 0, j = 0, k = 0, count = 0,
		     Flag = 0, Index = 0;

	for (k = 0; cmdTable[k].Name != NULL ; k++) {
		p1 = cmdTable[k].Name;
		p2 = strfind;
		count = 0;

		for (i = 0; i < strlen(cmdTable[k].Name); i++) {
			if (t_olower(*p1) == t_olower(*p2)) {
				p3 = p1;

				for (j = 0; j < strlen(strfind); j++) {
					if (t_olower(*p3) == t_olower(*p2)) {
						p3++;
						p2++;
					} else
						break;
				}

				p2 = strfind;

				if (j == strlen(strfind)) {
					count++;

					if (count == 1) {
						Flag = 1;
						Index++;
						printf("%d. %s\n", Index, cmdTable[k].Name);
					}
				}
			}

			p1++;
		}
	}

	if (Flag) {
		printf("\n\nThe Above List are the Related Commands for the String which you have entered:\n");
		printf("For detail command Usage help:\n");
		printf("Usage: switch_cli <Command> <Arg1> <Arg2> ...\n");
		printf("Getting command help text: switch_cli <Command> --help\n");
	}
}

static int find_string(char *matchthis, unsigned int *Index)
{
	int i, match_found = 1, j;

	for (j = 0; cmdTable[j].Name != NULL ; j++) {
		match_found = 1;

		for (i = 0; cmdTable[j].Name[i] != '\0'; i++) {
			if (t_olower(cmdTable[j].Name[i]) != t_olower(matchthis[i])) {
				match_found = 0;
				break;
			}

			if (t_olower(matchthis[i]) == '\0')
				break;
		}

		if (match_found) {
			*Index = j;
			printf("%s\n", cmdTable[j].Name);
			return 1;
		}
	}

	return 0;
}

/* find the command within the programm arguments and return the string to
   the global command table. */
static void findCommandName(int argc, char *argv[])
{
	int i;
//   static char * const prefix = "GSW_";
	unsigned int Index = 0;

	/* search for all programm parameter for a command name */
	for (i = 0; i < argc; i++) {
		/* search for all supported commands */
		if (!find_string(argv[i], &Index)) {
			find_sub_string(argv[i]);
		} else {
			pCmdTable = &cmdTable[Index];
			cmdTableIdx = Index;
			return;
		}
	}
}

#ifdef MACSEC
static void find_sub_string_Macsec(char *strfind)
{
	char *p1, *p2, *p3;
	unsigned int i = 0, j = 0, k = 0, count = 0,
		     Flag = 0, Index = 0;

	for (k = 0; cmdTableMacsec[k].Name != NULL ; k++) {
		p1 = cmdTableMacsec[k].Name;
		p2 = strfind;
		count = 0;

		for (i = 0; i < strlen(cmdTableMacsec[k].Name); i++) {
			if (t_olower(*p1) == t_olower(*p2)) {
				p3 = p1;

				for (j = 0; j < strlen(strfind); j++) {
					if (t_olower(*p3) == t_olower(*p2)) {
						p3++;
						p2++;
					} else
						break;
				}

				p2 = strfind;

				if (j == strlen(strfind)) {
					count++;

					if (count == 1) {
						Flag = 1;
						Index++;
						printf("%d. %s\n", Index, cmdTableMacsec[k].Name);
					}
				}
			}

			p1++;
		}
	}

	if (Flag) {
		printf("\n\nThe Above List are the Related Commands for the String which you have entered:\n");
		printf("For detail command Usage help:\n");
		printf("switch_cli <Command> help\n\n");
	}
}

static int find_string_Macsec(char *matchthis, unsigned int *Index)
{
	int i, match_found = 1, j;

	for (j = 0; cmdTableMacsec[j].Name != NULL ; j++) {
		match_found = 1;

		for (i = 0; cmdTableMacsec[j].Name[i] != '\0'; i++) {
			if (t_olower(cmdTableMacsec[j].Name[i]) != t_olower(matchthis[i])) {
				match_found = 0;
				break;
			}

			if (t_olower(matchthis[i]) == '\0')
				break;
		}

		if (match_found) {
			*Index = j;
			printf("%s\n", cmdTableMacsec[j].Name);
			return 1;
		}
	}

	return 0;
}

/* find the command within the programm arguments and return the string to
   the global command table. */
static void findCommandNameMacsec(int argc, char *argv[])
{
	int i;
//   static char * const prefix = "GSW_";
	unsigned int Index = 0;

	/* search for all programm parameter for a command name */
	for (i = 0; i < argc; i++) {
		/* search for all supported commands */
		if (!find_string_Macsec(argv[i], &Index)) {
			find_sub_string_Macsec(argv[i]);
		} else {
			pCmdTable = &cmdTableMacsec[Index];
			cmdTableIdx = Index;
			return;
		}
	}
}

#endif
static void display_options(void)
{
	printf("\n");
	printf("0.  Quit\n");
	printf("1.  RMON Operation \n");
	printf("2.  MAC  Operations \n");
	printf("3.  VLAN Operation \n");
	printf("4.  Extended VLAN Operations \n");
	printf("5.  VLAN Filter Operations \n");
	printf("6.  Bridge Port and Bridge Operations \n");
	printf("7.  PCE Operations \n");
	printf("8.  QOS Operations \n");
	printf("9.  8021x Operations \n");
	printf("10. Multicast Operations \n");
	printf("11. Trunking Operations \n");
	printf("12. WOL Operations \n");
	printf("13. MonitorPort Operations \n");
	printf("14. PMAC Operations \n");
	printf("15. PAE Operation \n");
	printf("16. MDIO Operation \n");
	printf("17. MMD operations \n");
	printf("18. MAC Filter Operation \n");
	printf("19. Debug Operation \n");
	printf("20. Xgmac Operation \n");
	printf("21. GSW Sub-system Operation \n");
	printf("22. Display All Switch Operation \n");
	printf("\nEnter the option (0 to 22) to continue : \n");
}

static void display_alert(void)
{
	printf("\n############# ALERT #############\n\n");
	printf("Switch HW version : 0x%x\n", gswip_version);
	printf("NOTE : The above Switch CLI commands are not supported for this Switch HW version\n");
	printf("\n############# ALERT #############\n");

}

static int findHelpMain(int *argc, char *argv[])
{
	int ret;
	int option = 0 ;

	if ((argv[1] == NULL) ||
	    (strcmp(argv[1], "help") == 0) ||
	    (strcmp(argv[1], "-help") == 0) ||
	    (strcmp(argv[1], "--help") == 0)
	   ) {

		printf("\n***** Switch CLI Help Menu *****\n\n");
		printf("Please select any of the below Switch Feature Operations,to display the related Switch CLI Commands\n");
		display_options();
		scanf("%d", &option);
		printf("\n\n");

		switch (option) {
		case 0:
			break;

		case 1:
			find_sub_string("RMON");
			break;

		case 2:
			find_sub_string("MAC");
			break;

		case 3:
			find_sub_string("_VLAN_");

			if (gswip_version == GSWIP_3_1) {
				display_alert();
				printf("\nFor this Switch HW version use : Extended Vlan and Vlan Filter Switch CLI commands\n\n");
			}

			break;

		case 4:
			find_sub_string("ExtendedVlan");

			if (gswip_version != GSWIP_3_1)
				display_alert();

			break;

		case 5:
			find_sub_string("VLANFilter");

			if (gswip_version != GSWIP_3_1)
				display_alert();

			break;

		case 6:
			find_sub_string("Bridge");

			if (gswip_version != GSWIP_3_1)
				display_alert();

			break;

		case 7:
			find_sub_string("PCE");
			break;

		case 8:
			find_sub_string("QOS");
			break;

		case 9:
			find_sub_string("8021x");
			break;

		case 10:
			find_sub_string("Multicast");
			break;

		case 11:
			find_sub_string("Trunking");
			break;

		case 12:
			find_sub_string("WOL");
			break;

		case 13:
			find_sub_string("MonitorPort");
			break;

		case 14:
			find_sub_string("PMAC");
			break;

		case 15:
			find_sub_string("ROUTE");

			if (gswip_version == GSWIP_3_1)
				display_alert();

			break;

		case 16:
			find_sub_string("MDIO");

			if (gswip_version == GSWIP_3_1)
				display_alert();

			break;

		case 17:
			find_sub_string("MMD");

			if (gswip_version == GSWIP_3_1)
				display_alert();

			break;

		case 18:
			find_sub_string("MAC_Filter");

			if (gswip_version != GSWIP_3_1)
				display_alert();

			break;

		case 19:
			find_sub_string("Debug");

			if (gswip_version != GSWIP_3_1)
				display_alert();

			break;

		case 20:
			argv[1] = "xgmac";
			argv[2] = "-help";
			*argc = 3;
			return 0;

		case 21:
			argv[1] = "gswss";
			argv[2] = "-help";
			*argc = 3;
			return 0;

		case 22:
			find_sub_string("GSW");
			break;

		default :
			find_sub_string("GSW");
			break;
		}

		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

/* search all program arguments for a help request. Return unequal zero in case
   a help request is found. Otherwise return zero */
static void findHelpText(int argc, char *argv[])
{
	int i;

	/* search for all programm parameter for a command name */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "help") == 0) {
			bHelpText = 1;
			return;
		}

		if (strcmp(argv[i], "-help") == 0) {
			bHelpText = 1;
			return;
		}

		if (strcmp(argv[i], "--help") == 0) {
			bHelpText = 1;
			return;
		}
	}

	return;
}

static int gswip_version_get(void *fd)
{
	GSW_register_t param;
	memset(&param, 0, sizeof(GSW_register_t));
	param.nRegAddr = 0x13;

	if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
		printf("ioctl returned with ERROR!\n");
		return (-1);
	}

	/*store in global variable*/
	gswip_version = param.nData;
	return 0;
}

#if defined(IOCTL_PCTOOL) && IOCTL_PCTOOL
int gsw_swcli_cmdProcessor(int argc, char *argv[])
{
#else
int main(int argc, char *argv[])
{
#endif
	command_fkt pFkt;
	int ret, fd, cnt;
	int devIdx = 0;

#ifndef CLI_DUMP_CALL
	char buffer[32];
#endif

#if ! defined (IOCTL_PCTOOL)
	system("echo 0 > /sys/module/printk/parameters/time");
#endif

	/* scan for device node index */
	cnt = scanParamArg(argc, argv, "dev", 32, &devIdx);
#if ! defined (CLI_DUMP_CALL) && ! defined (IOCTL_PCTOOL)
	sprintf(buffer, "/dev/switch_api/%d", devIdx);
	fd = open(buffer, O_RDONLY);

	if (fd < 0)
		goto end;

#else
	/*PC tool*/
	fd = 0;
#endif

	/*Get Switch HW version*/
	ret = gswip_version_get((void *)fd);

	if (ret != 0) {
		printf("Command returns with error %d\n\n", ret);
		printf("GSWIP Hardware Revision Identification get failed\n");
		goto end;;
	}

	if (findStringParam(argc, argv, "xgmac")) {
		if (argc > 12 || argc < 3) {
			printf("Invalid xgmac command, if specify *, please use double quotes or single quotes around *.\n"
			       "Shell will conside * as a wild space character and misbehave\n");
			argv[2] = "-help";
			argc = argc + 1;
		} else if (findStringParam(argc, argv, "help") ||
			   findStringParam(argc, argv, "-help") ||
			   findStringParam(argc, argv, "--help")) {
			argv[2] = "-help";
			argc = argc + 1;
		}
	} else if (findStringParam(argc, argv, "gswss") ||
		   findStringParam(argc, argv, "lmac")) {

		if (argc < 3) {
			argv[2] = "-help";
			argc = argc + 1;
		}
	} else {
		ret = findHelpMain(&argc, argv);

		if (ret)
			goto end;
	}

	/* search for a SWITCH API command in the program arguments */
	bHelpText = 0;
	pCmdTable = NULL;
	findCommandName(argc, argv);
#ifdef MACSEC

	if ((!findStringParam(argc, argv, "xgmac")) &&
	    (!findStringParam(argc, argv, "gswss"))) {
		findCommandNameMacsec(argc, argv);
	}

#endif

	if (pCmdTable == NULL) {
		printf("ERROR : Command Not Found\n\n");
		/* command not given, therefore printout help */
		printf("Usage: switch_cli <Command> <Arg1> <Arg2> ...\n");
		printf("Getting command help text: switch_cli <Command> --help\n");
		ret = -1;
		goto end;
	}

	if (findStringParam(argc, argv, "gswss") ||
	    findStringParam(argc, argv, "lmac") || findStringParam(argc, argv, "xgmac")) {
	} else {
		/* check if one parameter requests help printout */
		findHelpText(argc, argv);

		if (bHelpText == 1) {
			/* print out command help */
			printf("\nCommand Description:\n--------------------\n\n");
			printf("%s", HelpText[cmdTableIdx]);

			if (gswip_version == GSWIP_3_0) {
				printf(
					"\n\nCommon Parameter:\n"
					"-----------------\n"
					"dev:\n"
					"\tDevice selection:\n"
					"\t- \"0\" for \"/dev/switch/0\" (e.g. internal switch / GSWIP-L)\n"
					"\t- \"1\" for \"/dev/switch/1\" (e.g. external switch / GSWIP-R) and so on\n"
					"\tDefault is device \"0\" in case this parameter is not set.\n");
			}

#ifdef CLI_DUMP_CALL
			printf("\n!!!Parameter Dump Version!!!\n");
#endif
			ret = 0;
			goto end;
		}
	}


	/*Store the function Pointer*/
	pFkt = pCmdTable->fkt;

	if (pFkt == NULL) {
		printf("Command not supported (%d)!\n", cmdTableIdx);
		ret = -1;
		goto end;
	}

	/*call function pointer*/
	ret = pFkt(argc, argv, fd, argc - (2 + cnt));

	if (ret != 0) {
		printf("Command returns with error %d\n\n", ret);
		printf("Command Description:\n--------------------\n");
		printf("%s\n", HelpText[cmdTableIdx]);
#ifdef CLI_DUMP_CALL
		printf("Parameter Dump Version\n");
#endif
	}

end:

#if ! defined (CLI_DUMP_CALL) && ! defined (IOCTL_PCTOOL)

	if (fd < 0) {
		printf("ERROR: Could not open dev node \"%s\"\n", buffer);
		ret = -1;
	} else {
		close(fd);
	}

#endif

#if ! defined (IOCTL_PCTOOL)
	system("echo 1 > /sys/module/printk/parameters/time");
#endif

	return ret;
}
