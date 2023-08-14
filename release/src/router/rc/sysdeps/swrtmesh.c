/*
 * Copyright 2023, SWRTdev
 * Copyright 2023, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 */

#include "rc.h"

#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <wlutils.h>
#include <bcmdevs.h>
#include <shared.h>
#include <ralink.h>
#include <flash_mtd.h>
#include <swrt.h>
#include <swrtmesh.h>
#include <swrtmesh-utils.h>

void auto_generate_config(void)
{
	if(!check_if_dir_exist("/var/run/multiap"))
		eval("mkdir", "-p", "/var/run/multiap");
	if(!check_if_file_exist("/etc/config/wireless"))
		swrtmesh_generate_wireless_config();
	if(!check_if_file_exist("/etc/config/ieee1905"))
		swrtmesh_generate_ieee1905_config();
	if(!check_if_file_exist("/etc/config/mapcontroller"))
		swrtmesh_generate_controller_config();
	if(!check_if_file_exist("/etc/config/mapagent"))
		swrtmesh_generate_agent_config();
	if(!check_if_file_exist("/etc/config/topology"))
		swrtmesh_generate_topology_config();
	if(!check_if_file_exist("/etc/config/hosts"))
		swrtmesh_generate_hosts_config();
}

#if defined(RTCONFIG_SWRTMESH)
int start_swrtmesh(void)
{
	pid_t pid;
	char *ubusd_argv[] = { "ubusd", NULL };
	char *ieee1905_argv[] = { "ieee1905d", "-o", "/tmp/ieee1905.log", /*"-f",*/ "-dddd", NULL };
	char *cntl_argv[] = { "mapcontroller", "-o", "/tmp/mapcontroller.log", "-d", "-vvvv", NULL, NULL };
	char *tp_argv[] = { "topologyd", NULL };
	char *dynbhd_argv[] = { "dynbhd", NULL };
	char *agent_argv[] = { "mapagent", "-o", "/tmp/mapagent.log", "-d", "-vvvv", NULL };
//	char *swrtmeshd_argv[] = { "swrtmeshd", NULL };

	if (getpid() != 1) {
		notify_rc("start_swrtmesh");
		return 0;
	}
	if(nvram_match("swrtmesh_enable", "0") || nvram_match("x_Setting", "0"))
		return 0;
	stop_swrtmesh();
	system("touch /tmp/SWRTMESHUTILS_DEBUG");
	auto_generate_config();
//	swrtmesh_resync_config();
	_eval(ubusd_argv, NULL, 0, &pid);
	_eval(ieee1905_argv, NULL, 0, &pid);
	_eval(tp_argv, NULL, 0, &pid);
	if(nvram_match("swrtmesh_controller_enable", "1")){
		if(nvram_match("swrtmesh_agent_enable", "1"))
			cntl_argv[5] = "-w";
		_eval(cntl_argv, NULL, 0, &pid);
		if(!check_if_file_exist("/proc/sys/net/netfilter/nf_conntrack_timestamp"))
			system("echo 1 >/proc/sys/net/netfilter/nf_conntrack_timestamp");
	}
	if(nvram_match("swrtmesh_agent_enable", "1")){
		unlink("/var/run/multiap/multiap.backhaul");
		_eval(dynbhd_argv, NULL, 0, &pid);
		_eval(agent_argv, NULL, 0, &pid);
	}
	//_eval(swrtmeshd_argv, NULL, 0, &pid);
	return 0;
}

void stop_swrtmesh(void)
{
	if(getpid() != 1){
		notify_rc("stop_swrtmesh");
		return;
	}
	if(pids("topologyd"))
		killall_tk("topologyd");
	if(pids("dynbhd"))
		killall_tk("dynbhd");
	if(pids("swrtmeshd"))
		killall_tk("swrtmeshd");
	if(pids("mapagent"))
		killall_tk("mapagent");
	if(pids("mapcontroller"))
		killall_tk("mapcontroller");
	if(pids("ieee1905d"))
		killall_tk("ieee1905d");
	if(pids("ubusd"))
		killall_tk("ubusd");
}

int start_mapcontroller(void)
{
	pid_t pid;
	char *cntl_argv[] = { "mapcontroller", "-o", "/tmp/mapcontroller.log", "-d", "-vvvv", NULL, NULL };
	if(nvram_match("swrtmesh_agent_enable", "1"))
		cntl_argv[5] = "-w";
	_eval(cntl_argv, NULL, 0, &pid);
	return 0;
}

void stop_mapcontroller(void)
{
	if(pids("mapcontroller"))
		killall_tk("mapcontroller");
}
#endif
