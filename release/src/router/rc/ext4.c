/*
	Copyright (C) 2022 paldier
	Copyright (C) 2022 SWRTdev

*/

#include "rc.h"

#define __packed __attribute__((__packed__))

#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <errno.h>
#ifndef MNT_DETACH
#define MNT_DETACH	0x00000002
#endif
#include <limits.h>

/* for example: 
 * emmc
 * mmcblk0p27 for jffs,  filesystem is ext4.
*/

#define EXT4_DEV_NUM	0
#define EXT4_PART_NUM	27
#define EXT4_MNT_DIR	"/jffs"
#define EXT4_FS_TYPE	"ext4"
#define EXT4_MTD_NAME	"storage"

void start_ext4(void)
{
	int format = 0, ret = 0;
	char s[256];
	int dev, part;
	unsigned long long int size;
	const char *p;
	struct statfs sf;
	int mtd_part = 0, mtd_size = 0;
	char dev_mtd[] = "/dev/mmcblk0pXXX";
	char *ext4_cmd[] = { "mke2fs", "-t", "ext4", "-L", "storage", "-T", "largefile", "-m", "1", "-F", dev_mtd, NULL };

	if (!nvram_match("ubifs_on", "1")) {
		notice_set("ubifs", "");
		return;
	}

	if (!wait_action_idle(10))
		return;

	ret = block_getinfo(EXT4_MTD_NAME, &dev, &part, &size);
	if (ret){
		_dprintf("*** ext4: (%d)\n", ret);
		return;
	}
	snprintf(dev_mtd, sizeof(dev_mtd), "/dev/mmcblk%dp%d", dev, part);
	_dprintf("*** ext4: %s (%lld)\n", dev_mtd, size);

	if (nvram_match("jffs2_format", "1") || nvram_match("ext4_format", "1") || nvram_match("ubifs_format", "1")) {
		nvram_set("jffs2_format", "0");
		nvram_set("ext4_format", "0");
		nvram_set("ubifs_format", "0");
		_eval(ext4_cmd, NULL, 0, NULL);
		format = 1;
	}
	sprintf(s, "%lld", size);
	p = nvram_get("ubifs_size");
	if ((p == NULL) || (strcmp(p, s) != 0)) {
		nvram_set("ubifs_size", s);
		nvram_commit_x();
	}

	if (mount(dev_mtd, EXT4_MNT_DIR, EXT4_FS_TYPE, MS_NOATIME, "") != 0) {
		_dprintf("*** ext4 mount error\n");
		return;
	}
	if (nvram_match("ext4_clean_fs", "1") || nvram_match("ubifs_clean_fs", "1")) {
		nvram_unset("ext4_clean_fs");
		nvram_unset("ubifs_clean_fs");
		doSystem("rm -rf /jffs/*");
	}
	userfs_prepare(EXT4_MNT_DIR);

	notice_set("ubifs", format ? "Formatted" : "Loaded");

#ifdef RTCONFIG_JFFS_NVRAM
	system("rm -rf /jffs/nvram_war");
	jffs_nvram_init();
	system("touch /jffs/nvram_war");
#endif
	if (!check_if_dir_exist("/jffs/scripts/")) mkdir("/jffs/scripts/", 0755);
	if (!check_if_dir_exist("/jffs/configs/")) mkdir("/jffs/configs/", 0755);
	if (!check_if_dir_exist("/jffs/.sys/")) mkdir("/jffs/.sys/", 0755);
#if defined(RTCONFIG_SWRTMESH)
	if (!check_if_dir_exist("/jffs/swrtmesh/")) mkdir("/jffs/swrtmesh/", 0755);
	if (!check_if_dir_exist("/jffs/multiap/")) mkdir("/jffs/multiap/", 0755);
	system("ln -sf /jffs/swrtmesh /etc/config");
	system("ln -sf /jffs/multiap /etc/multiap");
#endif
}

void stop_ext4(int stop)
{
	struct statfs sf;
#if defined(RTCONFIG_PSISTLOG)
	int restart_syslogd = 0;
#endif

	if (!wait_action_idle(10))
		return;

#if defined(RTCONFIG_SOFTCENTER)
	if(nvram_match("sc_mount","2")){
		if (umount("/jffs/softcenter"))
			umount2("/jffs/softcenter", MNT_DETACH);
	}
#endif

	if ((statfs(EXT4_MNT_DIR, &sf) == 0) && (sf.f_type != 0x73717368)) {
		// is mounted
		run_nvscript("script_autostop", EXT4_MNT_DIR, 5);
	}
#if defined(RTCONFIG_PSISTLOG)
	if (!stop && !strncmp(get_syslog_fname(0), EXT4_MNT_DIR "/", sizeof(EXT4_MNT_DIR) + 1)) {
		restart_syslogd = 1;
		stop_syslogd();
		eval("cp", EXT4_MNT_DIR "/syslog.log", EXT4_MNT_DIR "/syslog.log-1", "/tmp");
	}
#endif

	notice_set("ubifs", "Stopped");
	if (umount(EXT4_MNT_DIR))
		umount2(EXT4_MNT_DIR, MNT_DETACH);

#if defined(RTCONFIG_PSISTLOG)
	if (restart_syslogd)
		start_syslogd();
#endif
}

