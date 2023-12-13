#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include <syslog.h>
#include "shared.h"

#define	NVSW_ERR_STRING_MAX_LEN	50
#define	JFFS_SYS		"/jffs/.sys"
#define	NVSW_PATH		JFFS_SYS"/NVSW"
#define	NVSW_CFGFILE		NVSW_PATH"/nvsw_cfg"
#define	NVSW_MAX_SNAPCOUNT	3
#define	NVSW_SNAP_PREFIX	"nvsw_"

#if defined(PRTAX57_GO)
#define	BTNSW_DISABLED		0
#define	BTNSW_NV_SWITCH		1
#define	BTNSW_LED_ONOFF		2
#define	BTNSW_WIFI_ONOFF	3
#define	BTNSW_VPN_ONOFF		4
#endif

static int nvsw_switching_val;	// this value is valid during init time

typedef struct {
	unsigned char nvsw_target_idx;	// 0: original BK, 1-NVSW_MAX_SNAPCOUNT: 1-N snapshot
					// for PRTAX57_GO, if BTNSW_NV_SWITCH is enabled, this value should be > 0
	unsigned char last_status;	// 0: OK, 1: file missing, 2: nvswid err, 3: system err
#if defined(PRTAX57_GO)
	unsigned char btnsw_meaning;    // BTNSW_DISABLED, BTNSW_NV_SWITCH, BTNSW_LED_ONOFF...
#endif
	char debug_message[NVSW_ERR_STRING_MAX_LEN+1]; // short string message
} __attribute__ ((__packed__)) nvswcfg, *nvswcfg_pt;

#if defined(PRTAX57_GO)
static int go_need_nv_switch_byBTNSW(unsigned char target_idx, unsigned char cur_idx, unsigned char *chgto)
/* input parameter :
 *    target_idx should be greather than 0
 *    cur_idx is current nvswid value
 *    chagto will be set to 0 or target_idx according to BTNSW
 * output :
 *    switch btn booton value 1(BTN ON) && current_idx == 0 , return true(1)
 *    switch btn booton value 0(BTN OFF) && current_idx > 0 , return true(1)
 * otherwise, return false(0) */
{
	char sw_btn;
	if (f_read("/proc/device-tree/sw_btn", &sw_btn, 1) == 1) {
		if (sw_btn == '1' && cur_idx != target_idx) {	// SWITCH ON && not snapshot now
			*chgto = target_idx;			// change to snapshot index
			return 1;
		}
		else if(sw_btn == '0' && cur_idx > 0) {		// SIWTCH OFF & is snapshot now
			*chgto = 0;				// change back to original
			return 1;
		}
		else
			return 0;

	} else {
		_dprintf("[NVSW]: read sw_btn fail!!!\n");
		return 0;
	}
}
#endif

static unsigned int nvsw_mtd_partsize(char *nvram_mtd_name)
{
	int mtd_fd;
	mtd_info_t mtd_info;

	if ((mtd_fd = open(nvram_mtd_name, O_RDWR|O_SYNC)) < 0)
		return 0;
	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0)
		return 0;
	close(mtd_fd);
	return mtd_info.size;
}

static int nvsw_init(nvswcfg_pt cfgpt, unsigned char *nvswid_pt, char *nvram_mtd_name)
{
	unsigned long fsize;
	char msg[NVSW_ERR_STRING_MAX_LEN+1] = {0};
	unsigned char status = 0;
	unsigned char changed_to_idx;
	fsize = f_size(NVSW_CFGFILE);
	*nvswid_pt = (unsigned char)nvram_get_int("nvswid");
	// check cfg file status
	if (fsize == sizeof(nvswcfg) && f_read(NVSW_CFGFILE, cfgpt, fsize) == fsize) {
#if defined(PRTAX57_GO)
		int need_change = 0;
		if (cfgpt->btnsw_meaning == BTNSW_NV_SWITCH && cfgpt->nvsw_target_idx > 0 &&
				go_need_nv_switch_byBTNSW(cfgpt->nvsw_target_idx, *nvswid_pt, &changed_to_idx)) {
			need_change = 1;
		} else if (cfgpt->btnsw_meaning != BTNSW_NV_SWITCH && *nvswid_pt != 0) {
			need_change = 1;
			changed_to_idx = 0; // set to original bakup
		}
		if (need_change)
#else
		changed_to_idx = cfgpt->nvsw_target_idx; // always set to target_idx, target_idx used as GO's switch value
		if (cfgpt->nvsw_target_idx != *nvswid_pt)
#endif
		{
			// sanity check
			char snapshot_name[PATH_MAX];
			unsigned int nvram_mtd_size;
			if (cfgpt->nvsw_target_idx > NVSW_MAX_SNAPCOUNT) {
				snprintf(msg, NVSW_ERR_STRING_MAX_LEN, "id err, target:[%d],nvsw:[%d]!", cfgpt->nvsw_target_idx, *nvswid_pt);
				status = 2;
				goto bad;
			}
			nvram_mtd_size = nvsw_mtd_partsize(nvram_mtd_name);
			if (nvram_mtd_size == 0) {
				snprintf(msg, NVSW_ERR_STRING_MAX_LEN, "err open nvram mtd[%s]", nvram_mtd_name);
				status = 3;
				goto bad;
			}

			sprintf(snapshot_name, "%s/%s%d.img", NVSW_PATH, NVSW_SNAP_PREFIX, changed_to_idx);
			fsize = f_size(snapshot_name);
			if (fsize != nvram_mtd_size) {
				snprintf(msg, NVSW_ERR_STRING_MAX_LEN, "err snapshot[%d] size[%lu][%u]", changed_to_idx, fsize, nvram_mtd_size);
				status = 1;
				goto bad;
			}
		}
		return 0;
	}
bad:
	// destroy all inavlid & create an empty cfg
	if (!check_if_dir_exist(JFFS_SYS))
		system("mkdir -p "JFFS_SYS);
	_dprintf("NVSW: init empty cfg...\n");
	system("rm -rf "NVSW_PATH);
	system("mkdir "NVSW_PATH);
	memset(cfgpt, 0x00, sizeof(nvswcfg));
	cfgpt->last_status = status;
	strncpy(cfgpt->debug_message, msg, NVSW_ERR_STRING_MAX_LEN);
	f_write(NVSW_CFGFILE, cfgpt, sizeof(nvswcfg), 0, 0);
	nvram_set_int("nvswid", 0);
	*nvswid_pt = 0;
	return -1;
}

#define	NVRAM_MTD_TMP_FILE	"/tmp/nvram_mtd"
static int got_nvram_mtd(char *buf, int max_len)
{
	int len;
	char *rpt;

	if (max_len < 11) return -1;
	system("cat /proc/mtd  | grep nvram | awk '{print $1}' | sed s/://g > "NVRAM_MTD_TMP_FILE);
	len = sprintf(buf, "/dev/");
	rpt = &buf[len];
	len = f_read(NVRAM_MTD_TMP_FILE, rpt, max_len-len);
	unlink(NVRAM_MTD_TMP_FILE);
	if (len == 0) {
		return -1;
	}
	rpt[len] = '\0';
	while (len && rpt[len-1] == '\n') {
		len--;
		rpt[len] = '\0';
	}
	return 0;
}

// only for MT798X now
int reload_nvram(void)
{
	int ret;
	int nvram_fd;

	if ((nvram_fd = open("/dev/nvram", O_RDWR)) < 0)
		return -1;

	ret = ioctl(nvram_fd, 0x0003, 0);     // get the real nvram space size
	close(nvram_fd);
	if (ret) {
		_dprintf("[NVSW]:ioctl nvram reload fail!!\n");
		return -1;
	}
	return 0;
}

int nvsw_switching(int *outval)
{
	nvswcfg cfg;
	unsigned char cur_nvswid;
	char nvram_mtd_name[40];
	unsigned char changed_to_idx;
	int need_change = 0;

	if (got_nvram_mtd(nvram_mtd_name, sizeof(nvram_mtd_name)) != 0) {
		*outval = 0xffff;
		nvsw_switching_val = *outval;
		return -2;
	}
	// _dprintf("[NVSW]: nvram_mtd is %s\n", nvram_mtd_name);

	if (nvsw_init(&cfg, &cur_nvswid, nvram_mtd_name) != 0) {
		_dprintf("[NVSW] init:%d (%s)\n", cfg.last_status, cfg.debug_message);
		*outval = 0;
		nvsw_switching_val = *outval;
		return -1;
	}

	_dprintf("[NVSW]: btnsw_val:%u, target_id:%u\n", cfg.btnsw_meaning, cfg.nvsw_target_idx);
#if defined(PRTAX57_GO)
	if (cfg.btnsw_meaning == BTNSW_NV_SWITCH && cfg.nvsw_target_idx > 0) {
		if (go_need_nv_switch_byBTNSW(cfg.nvsw_target_idx, cur_nvswid, &changed_to_idx) && cur_nvswid != changed_to_idx)
			need_change = 1;
	}
	else if (cur_nvswid != 0) { // change back to original
		need_change = 1;
		changed_to_idx = 0;
	}
#else
	if (cfg.nvsw_target_idx != cur_nvswid) {
		need_change = 1;
		changed_to_idx = cfg.nvsw_target_idx; // for NON-GO case
	}
#endif
	_dprintf("[NVSW]: cur_nvswid:%d, %s:%u\n", cur_nvswid, need_change?"change-to":"keep", need_change?changed_to_idx:cur_nvswid);

	if (need_change) {
		unsigned char chg_nvswid;
		// 1. make sure nvram module not operating
		// modprobe_r("nvram_linux");
		//stop_nvram(nvram_mtd_name);
		// 2. backup current nvram to snapshot image
#if 0 //!defined(NVSW_ALWAYS_BACKUP_ALL_SNAP)
		if (cur_nvswid == 0) // default backup non-snapshot nvram image
#endif
		doSystem("dd if=%s of=%s/%s%d.img", nvram_mtd_name, NVSW_PATH, NVSW_SNAP_PREFIX, cur_nvswid);
		// 3. erase nvram partition
		eval("mtd-erase", "-d", "nvram");
		// 4. write target nvram image to nvram partition
		//doSystem("dd if=%s/%s%d.img of=%s", NVSW_PATH, NVSW_SNAP_PREFIX, changed_to_idx, nvram_mtd_name);
		doSystem("mtd-write -i %s/%s%d.img -d nvram", NVSW_PATH, NVSW_SNAP_PREFIX, changed_to_idx);
		// 5. react nvram module
                // modprobe("nvram_linux");
		reload_nvram();
		// 6. verify nvswid & reset some nvram if needed
		if (!nvram_get("nvswid")) // come frome reset default
			nvram_set_int("nvswid", changed_to_idx);
		else {
			chg_nvswid = (unsigned char)nvram_get_int("nvswid");
			if (chg_nvswid != changed_to_idx) { // come frome upload config?
				_dprintf("[NVSW]: nvswid different in image, target:[%u], nv:[%u]!\n", changed_to_idx, chg_nvswid);
				nvram_set_int("nvswid", changed_to_idx); // overwrite
			}
		}
		*outval = cur_nvswid;
		*outval = (*outval << 8) | changed_to_idx;
		nvsw_switching_val = *outval;
	}
#if defined(PRTAX57_GO)
	// overwrite btnsw by btnsw_meaning
	if (!nvram_get("btnsw_onoff") || ((unsigned char)nvram_get_int("btnsw_onoff") != cfg.btnsw_meaning))
		nvram_set_int("btnsw_onoff", cfg.btnsw_meaning);
#endif
	return 0;
}

int nvsw_set(unsigned char idx, unsigned char btnsw_meaning)
{
	nvswcfg cfg;
	unsigned long fsize;

	if (idx > NVSW_MAX_SNAPCOUNT)
		return -2;
#if defined(PRTAX57_GO)
	if (btnsw_meaning == BTNSW_NV_SWITCH && idx == 0) // must give target idx if enabled
		return -2;
#endif
	fsize = f_size(NVSW_CFGFILE);
	if (fsize == sizeof(nvswcfg) && f_read(NVSW_CFGFILE, &cfg, fsize) == fsize) {
		int changed = 0;
		if (cfg.nvsw_target_idx != idx) {
			cfg.nvsw_target_idx = idx;
			changed++;
		}
#if defined(PRTAX57_GO)
		if (cfg.btnsw_meaning != btnsw_meaning) {
			cfg.btnsw_meaning = btnsw_meaning;
			changed++;
		}
#endif
		if (changed)
			f_write(NVSW_CFGFILE, &cfg, sizeof(nvswcfg), 0, 0);
		return 0;
	}
	return -1;
}

int nvsw_cfg_get(unsigned char *target_idx, unsigned char *max_idx, unsigned char *btnsw_meaning)
{
	nvswcfg cfg;
	unsigned long fsize;

	fsize = f_size(NVSW_CFGFILE);
	if (fsize == sizeof(nvswcfg) && f_read(NVSW_CFGFILE, &cfg, fsize) == fsize) {
		*target_idx = cfg.nvsw_target_idx;
		*max_idx = NVSW_MAX_SNAPCOUNT;
#if defined(PRTAX57_GO)
		*btnsw_meaning = cfg.btnsw_meaning;
#endif
		return 0;
	}
	return -1;
}

int nvsw_clone_nvimg(unsigned char idx, char *comment)
{
	unsigned char cur_nvswid;
	char nvram_mtd_name[40];
	int ret = 0;

	if (idx > NVSW_MAX_SNAPCOUNT || idx == 0)
		return -2;

	cur_nvswid = (unsigned char)nvram_get_int("nvswid");
	if (got_nvram_mtd(nvram_mtd_name, sizeof(nvram_mtd_name)) != 0) {
		return -3;
	}
	// 1. change nvswid to target idx
	nvram_set_int("nvswid", idx);
	nvram_commit();
	// 2. snapshot nvram image
	ret = doSystem("dd if=%s of=%s/%s%d.img", nvram_mtd_name, NVSW_PATH, NVSW_SNAP_PREFIX, idx);
	// 3. write comment file
	if (comment) {
		char snapshot_name[PATH_MAX];
		sprintf(snapshot_name, "%s/%s%d.txt", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
		if (f_write(snapshot_name, comment, strlen(comment)+1, 0, 0) < 0)
			ret |= 1;
	}
	// 4. write back nvswid
	nvram_set_int("nvswid", cur_nvswid);
	nvram_commit();
	if (ret == 0)
		return 0;
	return -1;
}

int nvsw_def_nvimg(unsigned char idx, char *comment)
{
	char nvram_mtd_name[40];
	unsigned int nvram_mtd_size;
	int ret = 0;

	if (idx > NVSW_MAX_SNAPCOUNT)
		return -2;

	if (got_nvram_mtd(nvram_mtd_name, sizeof(nvram_mtd_name)) != 0)
		return -3;

	nvram_mtd_size = nvsw_mtd_partsize(nvram_mtd_name);
	if (nvram_mtd_size == 0)
		return -4;

	ret = doSystem("tr '\\000' '\\377' < /dev/zero | dd of=%s/%s%d.img bs=1 count=%u", NVSW_PATH, NVSW_SNAP_PREFIX, idx, nvram_mtd_size);
	if (comment) {
		char snapshot_name[PATH_MAX];
		sprintf(snapshot_name, "%s/%s%d.txt", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
		if (f_write(snapshot_name, comment, strlen(comment)+1, 0, 0) < 0)
			ret |= 1;
	}
	if (ret == 0)
		return 0;
	return -1;
}

int nvsw_get_comment(unsigned char idx, char *buf, int buf_len)
{
	char snapshot_name[PATH_MAX];
	int len;
	if (idx > NVSW_MAX_SNAPCOUNT || idx == 0)
		return -2;
	sprintf(snapshot_name, "%s/%s%d.txt", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
	len = f_read(snapshot_name, buf, buf_len);
	if (len >= 0) {
		buf[len]='\0';
		return 0;
	}
	return -1;
}

int nvsw_new_comment(unsigned char idx, char *comment)
{
	if (idx > NVSW_MAX_SNAPCOUNT || idx == 0)
		return -2;

	if (comment) {
		unsigned long fsize;
		char snapshot_name[PATH_MAX];
		sprintf(snapshot_name, "%s/%s%d.img", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
		fsize = f_size(snapshot_name);
		if ((fsize != (unsigned long)-1) && fsize > 0) {
			sprintf(snapshot_name, "%s/%s%d.txt", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
			if (f_write(snapshot_name, comment, strlen(comment)+1, 0, 0) < 0)
				return -4;
			return 0;
		}
		return -3;
	}
	return -1;
}

int nvsw_statuslog_clear(void)
{
	nvswcfg cfg;
	unsigned long fsize;
	int ret;

	fsize = f_size(NVSW_CFGFILE);
	openlog("[NVSW]", 0, 0);
	if (fsize == sizeof(nvswcfg) && f_read(NVSW_CFGFILE, &cfg, fsize) == fsize) {
#if defined(PRTAX57_GO)
		syslog(LOG_NOTICE, "btnsw:%u", cfg.btnsw_meaning);
#endif
		syslog(LOG_NOTICE, "target:%u, last_status:%u", cfg.nvsw_target_idx, cfg.last_status);
		if (cfg.last_status) {
			syslog(LOG_INFO, "err msg(%s)", cfg.debug_message);
			cfg.last_status = 0;
			memset(cfg.debug_message, 0x00, NVSW_ERR_STRING_MAX_LEN);
			f_write(NVSW_CFGFILE, &cfg, sizeof(nvswcfg), 0, 0);
		} else if (nvsw_switching_val) {
			syslog(LOG_NOTICE, "SWITCH from %d to %d", (nvsw_switching_val >> 8) & 0xff, nvsw_switching_val & 0xff);
		}
		ret = 0;
	} else {
		syslog(LOG_INFO, "[NVSW]read cfg fail!!");
		ret = -1;
	}
	closelog();
	return ret;
}

int nvsw_rm_nvimg(unsigned char idx)
{
	nvswcfg cfg;
	unsigned long fsize;

	if (idx > NVSW_MAX_SNAPCOUNT || idx == 0)
		return -2;

	fsize = f_size(NVSW_CFGFILE);
	if (fsize == sizeof(nvswcfg) && f_read(NVSW_CFGFILE, &cfg, fsize) == fsize) {
		char snapshot_name[PATH_MAX];
		sprintf(snapshot_name, "%s/%s%d.img", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
		unlink(snapshot_name);
		sprintf(snapshot_name, "%s/%s%d.txt", NVSW_PATH, NVSW_SNAP_PREFIX, idx);
		unlink(snapshot_name);
#if defined(PRTAX57_GO)
		if (cfg.btnsw_meaning == BTNSW_NV_SWITCH && cfg.nvsw_target_idx == idx) {
			cfg.btnsw_meaning = BTNSW_DISABLED;
			cfg.nvsw_target_idx = 0;
			f_write(NVSW_CFGFILE, &cfg, sizeof(nvswcfg), 0, 0);
		}
#else
		if (cfg.nvsw_target_idx == idx) {
			cfg.nvsw_target_idx = 0;
			f_write(NVSW_CFGFILE, &cfg, sizeof(nvswcfg), 0, 0);
		}
#endif
		return 0;
	} else {
		return -1;
	}
}

static void nvsw_dump_all(void)
{
	unsigned char target_idx, max_idx, btnsw_meaning;
	if (nvsw_cfg_get(&target_idx, &max_idx, &btnsw_meaning)==0) {
		unsigned char i;
		unsigned char comment_buf[50];
		int ret;
		printf("btnsw_val:%u, cur_id:%d, target id:%u (max:%u)\n", btnsw_meaning, nvram_get_int("nvswid"), target_idx, max_idx);
		printf("============================================\n");
		for (i = 1; i <= max_idx; i++) {
			ret = nvsw_get_comment(i, comment_buf, sizeof(comment_buf));
			if (ret) sprintf(comment_buf, "N/A");
			printf("%cprofile %u: %s [%s]\n", (i==target_idx)? '*':' ', i, (ret==0)? "OK.":"N/A", comment_buf);
		}
	}
}

int nvsw_cmd(int argc, char **argv)
/* cmd format:
 * [nvsw] dump
 * [nvsw] set TARGET_IDX BTNSW_VAL
 * [nvsw] img clone IDX [COMMENT]
 * [nvsw] img default IDX [COMMENT]
 * [nvsw] img comment IDX NEW_COMMENT
 * [nvsw] img del idx
*/
{
	int ret = -1;
	if (argc < 1)
		goto usage_out;

	if (!strcmp(argv[0], "set")) {
		if (argc < 3)
			goto usage_out;
		ret = nvsw_set(atoi(argv[1]), atoi(argv[2]));
	} else if (!strcmp(argv[0], "dump")) {
		nvsw_dump_all();
		ret = 0;
	} else if (!strcmp(argv[0], "img")) {
		if (!strcmp(argv[1], "clone") || !strcmp(argv[1], "default")) {
			if (argc < 3)
				goto usage_out;
			if (argv[1][0] == 'c')
				ret = nvsw_clone_nvimg(atoi(argv[2]), (argc > 3)? argv[3]:NULL);
			else if (argv[1][0] == 'd')
				ret = nvsw_def_nvimg(atoi(argv[2]), (argc > 3)? argv[3]:NULL);
		} else if (!strcmp(argv[1], "comment")) {
			if (argc < 4)
				goto usage_out;
			ret = nvsw_new_comment(atoi(argv[2]), argv[3]);
		} else if (!strcmp(argv[1], "del")) {
			if (argc < 3)
				goto usage_out;
			ret = nvsw_rm_nvimg(atoi(argv[2]));
		}
	} else
		goto usage_out;

	if (ret) {
		printf("ERR: cmd ret %d\n", ret);
		return ret;
	}
usage_out:
	if (ret) {
		printf("Usage:\n");
		printf(" nvsw dump\n");
		printf(" nvsw set TARGET_IDX BTNSW_VAL\n");
		printf(" nvsw img clone IDX [COMMENT]\n");
		printf(" nvsw img default IDX [COMMENT]\n");
		printf(" nvsw img comment IDX NEW_COMMENT\n");
		printf(" nvsw img del idx\n");
		printf("==============================\n");
		printf("Note:\n");
		printf(" BTNSW_VAL=> 0:disabled, 1:profile switch, 2: LED ON/OFF...\n");
		printf(" IDX, must between 1-max\n\n");
	}
	return 0;
}
