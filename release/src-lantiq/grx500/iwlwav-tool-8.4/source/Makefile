export abs_top = $(shell pwd)
export abs_top_srcdir = $(abs_top)
export abs_top_builddir = $(abs_top_srcdir)
export abs_tools_srcdir = $(abs_top_srcdir)/tools
export STAGING_DIR = 

all: dutserver dump_handler logserver drvhlpr mtlk_cli mtlk_cli_dbg_srv BclSockServer;

clean:
	$(MAKE) -C tools/shared/linux/               clean
	$(MAKE) -C wireless/shared/                  clean
	$(MAKE) -C tools/mttools/shared/             clean
	$(MAKE) -C tools/shared/3rd_party/iniparser/ clean
	$(MAKE) -C tools/rtlogger/logserver/linux/   clean
	$(MAKE) -C tools/dutserver/linux/            clean
	$(MAKE) -C tools/mttools/drvhlpr/            clean
	$(MAKE) -C tools/dump_handler/               clean
	$(MAKE) -C tools/mttools/mtlk_cli/           clean
	$(MAKE) -C tools/mttools/mtlk_cli_dbg_srv/   clean
	$(MAKE) -C tools/BCLSockServer/              clean
	find $(abs_top) -name "logmacros.*"   | xargs rm -f
	find $(abs_top) -name "loggroups.h"   | xargs rm -f
	find $(abs_top) -name "Makefile.am"   | xargs rm -f
	find $(abs_top) -name "Makefile.in"   | xargs rm -f
	find $(abs_top) -name "*.scd"         | xargs rm -f
	find $(abs_top) -name "*.o.cmd"       | xargs rm -f 
	find $(abs_top) -name "*.stderr"      | xargs rm -f
	find $(abs_top) -name "*.pipe.result" | xargs rm -f
	find $(abs_top) -name ".logprep_ts"   | xargs rm -rf

libmtlkc:
	$(MAKE) -C tools/shared/linux/
	
libmtlkwls:
	$(MAKE) -C wireless/shared/
	
libmttools:
	$(MAKE) -C tools/mttools/shared/
	
libiniparser:
	$(MAKE) -C tools/shared/3rd_party/iniparser/
	
dutserver: libmtlkc libmtlkwls libmttools libiniparser
	$(MAKE) -C tools/dutserver/linux/

logserver: libmtlkc libmtlkwls
	$(MAKE) -C tools/rtlogger/logserver/linux/
	
drvhlpr: libmtlkc libmtlkwls libmttools libiniparser
	$(MAKE) -C tools/mttools/drvhlpr/

dump_handler: libmtlkc libmtlkwls libmttools
	$(MAKE) -C tools/dump_handler/

mtlk_cli: libmtlkc libmtlkwls libmttools
	$(MAKE) -C tools/mttools/mtlk_cli/
	
mtlk_cli_dbg_srv: libmtlkc libmtlkwls libmttools
	$(MAKE) -C tools/mttools/mtlk_cli_dbg_srv/

BclSockServer: libmtlkc
	$(MAKE) -C tools/BCLSockServer
