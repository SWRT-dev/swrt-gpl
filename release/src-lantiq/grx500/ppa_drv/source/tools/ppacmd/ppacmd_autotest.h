/******************************************************************************
**
** FILE NAME  : ppacmd_autotest.h
** PROJECT    : PPA Configuration Utility
** MODULES    : Packet Acceleration
**
** DATE     : 10 JUN 2008
** AUTHOR     : Shao Guohua
** DESCRIPTION  : PPA (Routing Acceleration) User Configuration Utility
** COPYRIGHT  :        Copyright (c) 2009
**              Lantiq Deutschland GmbH
**           Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
**
**
*******************************************************************************/
#ifndef PPACMD_AUTOTEST_H
#define PPACMD_AUTOTEST_H

extern void ppa_test_automation_help( int summary);
extern int ppa_parse_test_automation_cmd(PPA_CMD_OPTS *popts, PPA_CMD_DATA *pdata);
extern int ppa_test_automation_cmd(PPA_COMMAND *pcmd, PPA_CMD_DATA *pdata);
extern const char ppa_test_automation_short_opts[];
extern const struct option ppa_test_automation_long_opts[];

#endif //PPACMD_AUTOTEST_H