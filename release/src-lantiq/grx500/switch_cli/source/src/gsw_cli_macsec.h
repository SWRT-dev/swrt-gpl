/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#ifndef _MACSEC_CMD_DECLARE_H
#define _MACSEC_CMD_DECLARE_H
int gsw_macsec_reg_read(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_reg_write(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_init(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_exit(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_sa_rd(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_sptag(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_mtinfo(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_get_mtinfo(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_get_sptag(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_get_rmon(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_dbg_rmon_enc(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_dbg_rmon_dec(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_dbg_init_enc(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_rmon_clr(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_flow_rd(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_dbg_init_dis(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_sa_dbg(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_dbg_info(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_testcase_kat1(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_teststs_kat1(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_testcase_kat2(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_teststs_kat2(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_testcase_kat3(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_teststs_kat3(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_cpsts(int argc, char *argv[], void *fd, int numPar);
int gsw_macsec_print_pdata(int argc, char *argv[], void *fd, int numPar);


#endif /* _MACSEC_CMD_DECLARE_H*/
