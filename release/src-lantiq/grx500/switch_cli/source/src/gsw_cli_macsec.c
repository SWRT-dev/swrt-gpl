/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ltq_cli_lib.h"

#ifndef IOCTL_PCTOOL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#endif

#include "macsec_ops.h"
#include "gsw_cli_macsec.h"

const GSW_CommandTable_t cmdTableMacsec[] = {
	{/* 0 */ "MACSEC_REG_RD", (command_fkt)gsw_macsec_reg_read},
	{/* 1 */ "MACSEC_REG_WR", (command_fkt)gsw_macsec_reg_write},
	{/* 2 */ "MACSEC_INIT", (command_fkt)gsw_macsec_init},
	{/* 3 */ "MACSEC_EXIT", (command_fkt)gsw_macsec_exit},
	{/* 4 */ "MACSEC_SA_READ", (command_fkt)gsw_macsec_sa_rd},
	{/* 5 */ "MACSEC_SPTAG_EN", (command_fkt)gsw_macsec_sptag},
	{/* 6 */ "MACSEC_MTINFO_EN", (command_fkt)gsw_macsec_mtinfo},
	{/* 7 */ "MACSEC_GET_MTINFO", (command_fkt)gsw_macsec_get_mtinfo},
	{/* 8 */ "MACSEC_GET_SPTAG", (command_fkt)gsw_macsec_get_sptag},
	{/* 9 */ "MACSEC_GET_RMON", (command_fkt)gsw_macsec_get_rmon},
	{/* 10 */ "MACSEC_GET_MTINFO", (command_fkt)gsw_macsec_get_sptag},
	{/* 11 */ "MACSEC_DBG_INIT_ENC", (command_fkt)gsw_macsec_dbg_init_enc},
	{/* 12 */ "MACSEC_DBG_RMON_ENC", (command_fkt)gsw_macsec_dbg_rmon_enc},
	{/* 13 */ "MACSEC_DBG_RMON_DEC", (command_fkt)gsw_macsec_dbg_rmon_dec},
	{/* 14 */ "MACSEC_RMON_CLEAR", (command_fkt)gsw_macsec_rmon_clr},
	{/* 15 */ "MACSEC_FLOW_READ", (command_fkt)gsw_macsec_flow_rd},
	{/* 16 */ "MACSEC_DBG_DIS", (command_fkt)gsw_macsec_dbg_init_dis},
	{/* 17 */ "MACSEC_SA_DBG", (command_fkt)gsw_macsec_sa_dbg},
	{/* 18 */ "MACSEC_GET_DBG_INFO", (command_fkt)gsw_macsec_dbg_info},
	{/* 19 */ "MACSEC_TESTCASE_KAT1", (command_fkt)gsw_macsec_testcase_kat1},
	{/* 20 */ "MACSEC_TESTSTS_KAT1", (command_fkt)gsw_macsec_teststs_kat1},
	{/* 21 */ "MACSEC_TESTCASE_KAT2", (command_fkt)gsw_macsec_testcase_kat2},
	{/* 22 */ "MACSEC_TESTSTS_KAT2", (command_fkt)gsw_macsec_teststs_kat2},
	{/* 23 */ "MACSEC_TESTCASE_KAT3", (command_fkt)gsw_macsec_testcase_kat3},
	{/* 24 */ "MACSEC_TESTSTS_KAT3", (command_fkt)gsw_macsec_teststs_kat3},
	{/* 25 */ "MACSEC_CP_STS", (command_fkt)gsw_macsec_cpsts},
	{/* 26 */ "MACSEC_PRINT_PDATA", (command_fkt)gsw_macsec_print_pdata},
};

int gsw_macsec_reg_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_REG_RD;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "nRegAddr", sizeof(param.nRegAddr),
			    &param.nRegAddr);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_reg_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_REG_WR;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "nRegAddr", sizeof(param.nRegAddr),
			    &param.nRegAddr);

	cnt += scanParamArg(argc, argv, "nData", sizeof(param.nData),
			    &param.nData);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_init(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_INIT;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_exit(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_EXIT;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_sa_rd(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_SA_READ;

	param.sa_st = 0;
	param.sa_end = 3;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "SaStart", sizeof(param.sa_st),
			    &param.sa_st);

	cnt += scanParamArg(argc, argv, "SaEnd", sizeof(param.sa_end),
			    &param.sa_end);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_flow_rd(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.sa_st = 0;
	param.sa_end = 3;

	param.cmdType = MACSEC_FLOW_READ;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "SaStart", sizeof(param.sa_st),
			    &param.sa_st);

	cnt += scanParamArg(argc, argv, "SaEnd", sizeof(param.sa_end),
			    &param.sa_end);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}


int gsw_macsec_sptag(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_SPTAG_EN;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "SpTagEn", sizeof(param.sptagen),
			    &param.sptagen);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_mtinfo(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_SPTAG_EN;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "MtInfoEn", sizeof(param.mtinfoen),
			    &param.mtinfoen);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_get_mtinfo(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_GET_MTINFO;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_get_sptag(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_GET_SPTAG;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_get_rmon(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.sa_st = 0;
	param.sa_end = 3;

	param.cmdType = MACSEC_GET_RMON;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "SaStart", sizeof(param.sa_st),
			    &param.sa_st);

	cnt += scanParamArg(argc, argv, "SaEnd", sizeof(param.sa_end),
			    &param.sa_end);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_rmon_clr(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_RMON_CLEAR;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_dbg_rmon_enc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_DBG_RMON_ENC;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_dbg_rmon_dec(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_DBG_RMON_DEC;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_dbg_init_enc(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_DBG_INIT_ENC;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_dbg_init_dis(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_DBG_DIS;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_sa_dbg(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.sa_st = 0;
	param.val = 24;

	param.cmdType = MACSEC_SA_DBG;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "SaIdx", sizeof(param.sa_st),
			    &param.sa_st);

	cnt += scanParamArg(argc, argv, "SaWords", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_dbg_info(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_GET_DBG_INFO;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_testcase_kat1(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTCASE_KAT1;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_teststs_kat1(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTSTS_KAT1;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_testcase_kat2(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTCASE_KAT2;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_teststs_kat2(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTSTS_KAT2;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_testcase_kat3(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTCASE_KAT3;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_teststs_kat3(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_TESTSTS_KAT3;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	cnt += scanParamArg(argc, argv, "testid", sizeof(param.val),
			    &param.val);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

int gsw_macsec_cpsts(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_CP_STS;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;
}

int gsw_macsec_print_pdata(int argc, char *argv[], void *fd, int numPar)
{
	GSW_MACSec_cfg_t param;
	unsigned int cnt = 0;

	memset(&param, 0x00, sizeof(param));

	param.cmdType = MACSEC_PRINT_PDATA;

	cnt += scanParamArg(argc, argv, "devid", sizeof(param.devid),
			    &param.devid);

	if (cli_ioctl(fd, GSW_MACSEC_CFG, &param) != 0)
		return (-1);

	return 0;

}

