#include "precomp.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#define VERSION_STR "2.0.2"
#define SIGNAL
/******************************************************************************************************
*	Constants
*******************************************************************************************************/
#define NUM_DRIVER_IF 2
#define MAX_CLI_PARAMETERS 10
#define MAX_CLI_PARAMETERS_LEN 100
int ctrl_sock = -1;
/******************************************************************************************************
*	Prototype
*******************************************************************************************************/
static void RaCfg_Agent(void);	/* 2013.12.04.Common Procedure */
static void NetReceive(UINT8 *inpkt, int len);
static int GetOpt(int argc, char *const argv[], const char *optstring);	/* GetOpt - only used in main.c */
static void Usage(void);
static int processPktOldMagic(UINT8 *inpkt, int len, int if_index);
static int processPktNewMagic(UINT8 *inpkt, int len);
static int if_match(const char *if_name);
static int parse_and_kill_pid(char *line_buf, pid_t pid);
static void pkt_proc_logic(void *arg);
static void task_dispatcher(UINT8 *inpkt, int len, int if_index);
#ifdef CONFIG_PLATFORM_MODULE_CMD_PATH
static void PlatformModuleProcess(UINT8 *inpkt, int len, int if_index);
#endif

#ifdef DBG
static void ate_hexdump(int level, char *str, unsigned char *pSrcBufVA, unsigned long SrcBufLen)
{
	unsigned char *pt;
	int x;

	if (level < ate_debug_level)
		return;

	pt = pSrcBufVA;
	printf("%s: %p, len = %lu\n", str,  pSrcBufVA, SrcBufLen);

	for (x = 0; x < SrcBufLen; x++) {
		if (x % 16 == 0)
			printf("0x%04x : ", x);

		printf("%02x ", ((unsigned char)pt[x]));

		if (x % 16 == 15) printf("\n");
	}

	printf("\n");
}

/**
 * ate_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void ate_printf(int level, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	if (level >= ate_debug_level) {
		vprintf(fmt, ap);
		printf("\n");
	}

	va_end(ap);
}
#else /* DBG */
#define ate_printf(args...) do { } while (0)
#define ate_hexdump(l, t, b, le) do { } while (0)
#endif /* DBG */

#ifdef SIGNAL
#include <signal.h>
void init_signals(void);
void signup(int);
#endif /* SIGNAL */

int ate_debug_level = MSG_DEBUG; /* default : ate_debug_level == 2 */
/******************************************************************************************************
*	Private Data
*******************************************************************************************************/
static const char *ate_daemon_version = "ate daemon v" VERSION_STR "\n";
static int sock = -1;
/* respond to QA by unicast frames if bUnicast == TRUE */
static BOOLEAN bUnicast = FALSE;
/* ATED supports proprietary driver if propritary_driver_support == TRUE */
static BOOLEAN proprietary_driver_support = TRUE;
static BOOLEAN need_set_mac = FALSE;
static const char broadcast_addr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static int do_fork = 1;
static int signup_flag = 1;
static int optindex = 1;
static int optopts;
static char *optargs;
unsigned short ate_cmd_id_len_tbl[] = {
	0, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 6, 6, 6, 2, 2, 2, 0,
};
unsigned short cmd_id_len_tbl[] = {
	18, 2, 4, 0, 0xffff, 4, 8, 6, 2, 3, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0, 0, 0, 0, 2,
};
static char bridge_ifname[IFNAMSIZ];
static unsigned char bridge_mac[ETH_ALEN];
static char driver_ifname[NUM_DRIVER_IF][IFNAMSIZ];	/* Backward competibilty */
static int dri_if_num; /* Number of driver interface from user input */
/* Interface Abstraction */
struct HOST_IF *host_fd;
struct DRI_IF *dri_fd[NUM_DRIVER_IF];
static UINT16 version;
static int is_default_if = 1;
static unsigned char l1profile_path[100] = {"/etc/wireless/l1profile.dat"};
unsigned int e2p_size = 0;
static unsigned int e2p_flash_offset = 0;
static unsigned char e2p_binary_path[100] = {"/lib/firmware/"};

/******************************************************************************************************
*	Functions
*******************************************************************************************************/
static int wifiDownUp(char *if_name, int downUp);
static int changeConfigFile(UINT32 mode, UINT32 type);

#ifdef SIGNAL
void signup(int signum)
{
	int count;
	ate_printf(MSG_INFO, "===>%s, interface num(%d), signum: %d\n", __func__, dri_if_num, signum);

	/* It's time to terminate myself. */
	switch (signum) {
	case SIGUSR1:
		ate_debug_level++;

		if (ate_debug_level > 4)
			ate_debug_level = 4;

		printf("Debug level++, %d\n", ate_debug_level);
		break;

	case SIGUSR2:
		ate_debug_level--;

		if (ate_debug_level < 0)
			ate_debug_level = 0;

		printf("Debug level--, %d\n", ate_debug_level);
		break;

	case SIGPIPE:
		printf("Broken PIPE\n");

	case SIGHUP:
	case SIGTERM:
	case SIGABRT:
		if (signup_flag == 1) {
			/* Prepare Leave, free malloc */
			signup_flag = 0;

			if (host_fd) {
				host_fd->close();
				ate_printf(MSG_INFO, "%d, free host_fd-v1\n", getpid());
				free(host_fd);
				host_fd = NULL;
			}

			for (count = 0; count < dri_if_num; count++) {
				if (dri_fd[count]) {
					ate_printf(MSG_INFO, "%d, free dri_fd[%d]-v1\n", getpid(), count);
					dri_fd[count]->ops->multi_proc_close(dri_fd[count]);
					free(dri_fd[count]);
					dri_fd[count] = NULL;
				}
			}

			if(e2p_buffer)
				free(e2p_buffer);
		} else
			printf("Signup_flag is already 0\n");

		break;

	default:
		printf("Do nothing, %d\n", signum);
		break;
	}
}

void init_signals(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGABRT);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sigaddset(&sa.sa_mask, SIGPIPE);
	sa.sa_handler = signup;
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);
}
#endif /* SIGNAL */

static int wifiDownUp(char *if_name, int downUp)
{
	int ret = 0;
	int s;
	struct ifreq ifr;
	int ifname_len = os_strlen(if_name);

	printf("interface[%d]:%s, downUp:%d\n", ifname_len, if_name, downUp);
	os_memset(&ifr, 0, sizeof(ifr));
	os_memcpy(ifr.ifr_name, if_name , ifname_len);
	s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s < 0)
		goto err0;

	if (downUp)
		ifr.ifr_flags |= IFF_UP;
	else
		ifr.ifr_flags &= ~IFF_UP;

	ret = ioctl(s, SIOCSIFFLAGS, &ifr);

	if (ret)
		goto err0;

	return 0;
err0:
	printf("wifiDownUp %d fail, err %d\n", downUp, ret);
	return ret;
}

static int changeConfigFile(UINT32 mode, UINT32 type)
{
	int ret = 0;
	int childpid;
	char *path = NULL;

	if (mode == 2)
		path = DBDC_CONFIG_PATH;
	else if (type == 2)
		path = A_BAND_CONFIG_PATH;
	else
		path = G_BAND_CONFIG_PATH;

	if (fork() == 0) {
		/* child process */
		char *execv_str[] = {"cp", path, CONFIG_PATH, NULL};

		if (execv("/bin/cp", execv_str) < 0) {
			perror("error on chageConfigFile");
			exit(0);
		}
	} else {
		/* parent process */
		wait(&childpid);
		printf("Change Config Done\n");
	}

	return ret;
}

static int hqa_set_band(struct DRI_IF *if_info, unsigned char *data, UINT16 len)
{
	int ret = 0;
	unsigned char *tmp = data;
	UINT32 band_mode = 0;
	UINT32 band_type = 0;

	os_memcpy(&band_mode, tmp, sizeof(band_mode));
	tmp += sizeof(band_mode);
	os_memcpy(&band_type, tmp, sizeof(band_type));
	band_mode = be2cpu32(band_mode);
	band_type = be2cpu32(band_type);

	if ((ret = changeConfigFile(band_mode, band_type)))
		goto err0;

	if ((ret = wifiDownUp(if_info->ifname, 0)))
		goto err0;

	if ((ret = wifiDownUp(if_info->ifname, 1)))
		goto err0;

	return 0;
err0:
	printf("hqa_set_band fail, band_mode:%d, band_type:%s err %d\n", band_mode, band_type, ret);
	return ret;
}

static int if_match(const char *if_name)
{
	int if_index = 0;
	struct DRI_IF *if_to_send = NULL;

	for (if_index = 0; if_index < dri_if_num; if_index++) {
		if_to_send = dri_fd[if_index];
		ate_printf(MSG_INFO, "if_to_send:%s, match name: %s\n", if_to_send->ifname, if_name);

		if (os_strcmp(if_to_send->ifname, if_name) == 0)
			break;
	}

	if (if_index == dri_if_num)
		return -1;

	return if_index;
}

static int parse_and_kill_pid(char *line_buf, pid_t pid)
{
	int distance = 0;
	int dif2 = 0;
	char *l;
	char ate_pid[16];
	ate_printf(MSG_DEBUG, "Parsing line: %s\n", line_buf);

repeat_parse:
	l = os_strchr(line_buf + distance, ' ');

	if (!l)
		return 0;

	ate_printf(MSG_DEBUG, "Line: 0x%x, l: 0x%x\n", line_buf, l);
	dif2 = ((int)(l - line_buf)) - distance;
	distance += dif2;

	/* The first char is space */
	if (dif2 == 0) {
		distance += 1;
		goto repeat_parse;
	}

	if ((dif2) > 16) {
		ate_printf(MSG_DEBUG, "String too long for pid, continue to parse, [%s]\n", ate_pid);
		goto repeat_parse;
	}

	os_memset(ate_pid, 0, 16);
	os_memcpy(ate_pid, l - dif2, dif2); /* For delete appending space */
	ate_printf(MSG_INFO, "ate_pid: %s\n", ate_pid);

	do {
		int pid_found = 0;
		int ret = -1;

		sscanf(ate_pid, "%d", &pid_found);

		if (pid_found != pid) {
			ate_printf(MSG_DEBUG, "!pid_found: %d\n", pid_found);
			ret = kill((pid_t)pid_found, SIGHUP);

			if (ret)
				ate_printf(MSG_ERROR, "kill process %d fail\n", pid_found);
			else
				ate_printf(MSG_INFO, "kill process %d success\n", pid_found);
		}
	} while (0);

	return 0;
}

static void clean_past_ated(void)
{
	pid_t pid = getpid();
	pid_t fork_pid;
	int pipefd[2];  /* 0: reading, 1: writing */
	if( pipe(pipefd) )
		return;
	ate_printf(MSG_INFO, "Pid of ated: %d\n", pid);
	fork_pid = fork();
	int dbg_cnt = 0;

	if (fork_pid == 0) {
		char *argv[] = {"ps", NULL};
		close(pipefd[0]);	/* children only DO write data. */
		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);
		execvp("ps", argv);
		close(pipefd[1]);
		exit(0);
	} else {
		/* Wait exec finish */
		char buffer[512];
		char line_buf[1024];
		unsigned int line_buf_pos = 0, str_len = 0;
		int buf_len = 0;
		char *buf_ptr = NULL;
		char *eol = NULL;

		close(pipefd[1]);
		os_memset(&buffer[0], 0, sizeof(buffer));
		os_memset(&line_buf[0], 0, sizeof(line_buf));

		while ((buf_len = read(pipefd[0], buffer, sizeof(buffer))) != 0) {
			int inner_cnt = 0;

			ate_printf(MSG_DEBUG, "Now go for Round %d\n", dbg_cnt);
			dbg_cnt++;
			buf_ptr = buffer;
			line_buf_pos = os_strlen(line_buf);
			inner_cnt = 0;

			do {
				ate_printf(MSG_DEBUG, "inner_cnt = %d\n", inner_cnt);
				inner_cnt++;
				ate_printf(MSG_DEBUG, "Preparing to parsing buffer[%d]=%s\n", buf_len, buf_ptr);
				ate_printf(MSG_DEBUG, "line_buf_pos=%d\n", line_buf_pos);
				eol = os_strchr(buf_ptr, '\n');

				if (eol == NULL) {
					if (buf_len > 0) {
						os_strncpy(&line_buf[line_buf_pos], buf_ptr, buf_len);
						line_buf_pos += buf_len;
						ate_printf(MSG_DEBUG, "Parsing done for one read, still have some string remaining\n");
						ate_printf(MSG_DEBUG, "Remaining buf[%d]=%s\n", buf_len, line_buf);
					}

					break;
				} else {
					/* if there's eol, append '\0' tail to statement. */
					*eol = '\0';
				}

				str_len = os_strlen(buf_ptr);   /* each row length, counting to '\0'. */

				if (str_len > ((sizeof(line_buf) - line_buf_pos))) {
					ate_printf(MSG_ERROR, "Error!!\n");
					break;
				} else {
					/* copy string to line_buf_pos with string length. */
					os_strncpy(&line_buf[line_buf_pos], buf_ptr, str_len);
					ate_printf(MSG_DEBUG, "Copied line[%d]=%s\n", os_strlen(line_buf), line_buf);

					/* capature ated key word in line_buf. */
					if (os_strstr(line_buf, "ated"))
						parse_and_kill_pid(line_buf, pid);

					buf_len -= (str_len + 1);
					buf_ptr += (str_len + 1);
					os_memset(&line_buf[0], 0, sizeof(line_buf));
					line_buf_pos = 0;
					ate_printf(MSG_DEBUG, "buf_len=%d\n", buf_len);

					if (buf_len <= 0) {
						ate_printf(MSG_DEBUG, "Break this inner_cnt here!str_len=%d, buf_len=%d, buf_ptr=%p\n", str_len, buf_len, buf_ptr);
						break;
					}
				}
			} while (eol);
		}

		close(pipefd[0]);
		waitpid(fork_pid, 0, 0);
		close(pipefd[1]);
	}
}


/*
	General cases, *key point to the string without INDEX0_. Traversal all lines if key point to NULL
	For example: get_from_l1profile("ra0", l1profile_path, "EEPROM_offset", value),
				 then value point to the string of eeprom offset from l1profile
*/
static int get_from_l1profile(unsigned char *ifname,
									  unsigned char *path,
									  unsigned char *key,
									  unsigned char *out)
{
	FILE *l1profile_f;
	unsigned int line = 0, retVal = -1;
	unsigned char buf[512], *pos = NULL, *param = NULL, *value = NULL, *dbdc_value = NULL, index[10] = "\0";

	l1profile_f = fopen(path, "r");
	if (l1profile_f == NULL) {
		printf("'%s' is not valid for reading.\n", path);
		goto error_out;
	}

	/* find index according to interface name */
	//printf("Find index of %s\n", ifname);
	while (fgets(buf, sizeof(buf), l1profile_f)) {
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

		pos = os_strchr(buf, '=');

		if (pos) {
			unsigned char *semicolon_ptr = NULL;
			/* if DBDC mode value then strip value after semicolon */
			if ( (semicolon_ptr = strstr(pos, ";")) != NULL ) {
				*semicolon_ptr = '\0';
				dbdc_value = semicolon_ptr+1;
			}

			*pos = '\0';
			param = buf;
			value = pos+1;

			if(strcmp(value, ifname) == 0) {
				snprintf(index, 7, "%s", param);
				//printf("found as %s\n", index);
				break;
			} else if (dbdc_value && (strcmp(dbdc_value, ifname) == 0)) {
				snprintf(index, 7, "%s", param);
				//printf("found as %s\n", index);
				break;
			}
		}
	}


	if (strlen(index) > 0) {
		fseek(l1profile_f, 0, SEEK_SET);
		/* match key */
		param = value = NULL;
		while (fgets(buf, sizeof(buf), l1profile_f)) {
			line++;
			pos = buf;
			while (*pos != '\0') {
				if (*pos == '\n') {
					*pos = '\0';
					break;
				}
				pos++;
			}
			if (buf[0] == '\0')
				continue;

			pos = os_strchr(buf, '=');

			/* match key with valid input */
			if (pos) {
				unsigned char *target[100];

				*pos = '\0';
				param = buf;
				value = pos+1;
				sprintf(target, "%s_%s", index, key);

				//printf("[%d]%s=%s match with %s\n", line, param, value, target);

				if (strcmp(target, param) == 0) {
					//printf("Found at [%d]%s=%s\n", line, param, value);
					break;
				} else
					param = NULL;
			}
		}

		if (param) {
			strcpy(out, value);
			retVal = 0;
		}
	} else {
		printf("\"%s\" is not found in %s, please check the environment.\n", ifname, path);
	}

error_out:
	if(l1profile_f)
		fclose(l1profile_f);

	return retVal;
}


static int memcpy_exs(unsigned char *dst, unsigned char *src, unsigned int len)
{
	unsigned int i;
	unsigned short *pDst, *pSrc;

	pDst = (unsigned short *) dst;
	pSrc = (unsigned short *) src;

	for (i = 0; i < (len >> 1); i++) {
		*pDst = be2cpu16(*pSrc);
		pDst++;
		pSrc++;
	}

	if ((len % 2) != 0) {
		memcpy(pDst, pSrc, (len % 2));
		*pDst = be2cpu16(*pDst);
	}

	return 0;
}


static int get_e2p_from_drv(unsigned char *buf, unsigned int size)
{
	unsigned short idx = 0x0;
	unsigned int retVal = -1;
	struct DRI_IF *p_if = dri_fd[0];

	if (p_if == NULL) {
		ate_printf(MSG_WARNING, "Driver socket is not initialized, do it now.");

		p_if = (struct DRI_IF *)malloc(sizeof(*p_if));
		memset(p_if, 0, sizeof(*p_if));

		if (p_if == NULL) {
			ate_printf(MSG_ERROR, "Socket error in IOCTL");
			return -1;
		}

		p_if->init_if = &init_if_ioctl;
		p_if->init_if(p_if, driver_ifname);
	}


	for (idx ; idx < size ; idx+=16) {
		struct racfghdr request_bulk;
		unsigned short *offset = (unsigned short *)request_bulk.data;
		unsigned short *size = (unsigned short *)(request_bulk.data+2);

		request_bulk.magic_no = cpu2be32(RACFG_MAGIC_NO);
		request_bulk.comand_id = cpu2be16(HQA_ReadBulkEEPROM);
		request_bulk.length = cpu2be16(0x4);

		*offset = cpu2be16(idx);
		*size = cpu2be16(0x10);

		retVal = p_if->send(p_if, &request_bulk, 4);

		if(retVal == 0) {
			int x=0;
			memcpy_exs(&buf[idx], request_bulk.data+2, 0x10);

#ifdef DBG
			for (x = 0; x < 0x10; x++) {
				if (x % 0x10 == 0)
					printf("0x%04x : ", idx);

				printf("%02x ", ((unsigned char)buf[idx+x]));

				if (x % 0x10 == 15) printf("\n");
			}
#endif
		} else
			goto error_out;
	}

	retVal = size;

error_out:
	return retVal;
}


static int update_eeprom_binary(unsigned char *path, unsigned char *input, unsigned int offset, unsigned int size)
{
	int retVal = -1;
	FILE *e2p_f = NULL;

	e2p_f = fopen(path, "rb+");	/* try to replace exist file fisrt */

	if(e2p_f == NULL)	{		/* file not exist, create it */
		if(strcmp(path, "/lib/firmware/")) {
			e2p_f = fopen(path, "wb+");
			printf("%s not eixst, try to create it.\n", path);
		} else {
			printf("eeprom binary is not supported, dismissed.\n");
			goto error_out;
		}
	}

	if (e2p_f) {
		fseek(e2p_f, offset, SEEK_SET);

		fwrite(input, 1, size, e2p_f);

		fclose(e2p_f);
		retVal = 0;
	} else {
		printf("Error! %s create failed!\n", path);
	}

error_out:
	return retVal;
}

static int cli_sync_eeprom(unsigned int start, unsigned int end, unsigned int offset)
{
	unsigned int length = end - start + 1;

	/* read all eeprom data from wireless driver */
	get_e2p_from_drv(e2p_buffer, e2p_size);
#ifdef DBG
	update_eeprom_binary("/tmp/e2p", (unsigned char *)e2p_buffer+start, offset+start, e2p_size);
#endif
	flash_write((char *)e2p_buffer+start, offset+start, length);
	ate_printf(MSG_INFO, "flash write to 0x%04x, length=0x%04x\n",offset+start, length);

	update_eeprom_binary(e2p_binary_path, (unsigned char *)e2p_buffer+start, offset+start, length);

	return 0;
}

static int parsing_ee_info(
	unsigned char *cmd,
	unsigned int *eeprom_start,
	unsigned int *eeprom_end,
	unsigned int *offset)
{
	int retVal = -1;
	unsigned char *ee_start_ptr = strstr(cmd, "[");
	unsigned char *ee_end_ptr = strstr(cmd, ":");
	unsigned char *ee_offset_ptr = NULL;

	if(ee_start_ptr == NULL || ee_start_ptr[1] == ':') {
		goto error_out;
	} else {
		if(ee_start_ptr != cmd)	/* declare offset, ignore l1profile.dat */
			ee_offset_ptr = cmd;

		*ee_start_ptr = '\0';
		ee_start_ptr++;
	}

	if(ee_end_ptr == NULL || ee_end_ptr[1] == ']') {
		goto error_out;
	} else {
		*ee_end_ptr = '\0';
		ee_end_ptr++;
	}

	if(ee_start_ptr)
		*eeprom_start = strtol(ee_start_ptr, NULL, 16);

	if(ee_end_ptr && strtol(ee_end_ptr, NULL, 16) < *eeprom_end)
		*eeprom_end = strtol(ee_end_ptr, NULL, 16);

	if(ee_offset_ptr)
		*offset = strtol(ee_offset_ptr, NULL, 16);

	retVal = 0;

error_out:
	return retVal;
}

static int process_cli_cmd(unsigned char *cmd)
{
	unsigned char cli_cmd_cnt = 0;
	unsigned char cli_cmds[MAX_CLI_PARAMETERS][MAX_CLI_PARAMETERS_LEN] = {0};
	unsigned char *cmd_tok = strtok(cmd, " ");

	while(cmd_tok) {
		cli_cmd_cnt++;
		if (cli_cmd_cnt > MAX_CLI_PARAMETERS) {
			printf("Syntax error, exceeding maximum parameters(%d)!\n", MAX_CLI_PARAMETERS);
			goto error_out;
		}

		if (strlen(cmd_tok) < MAX_CLI_PARAMETERS_LEN)
			strcpy(cli_cmds[cli_cmd_cnt-1], cmd_tok);
		else {
			printf("Syntax error, %d exceeding maximum length(%d)!\n", strlen(cmd_tok), MAX_CLI_PARAMETERS_LEN);
			goto error_out;
		}

		cmd_tok = strtok(NULL, " ");
	}
	
	if (strcmp(cli_cmds[0], "sync") == 0) {
		if (strcmp(cli_cmds[1], "eeprom") == 0 ) {
			if (proprietary_driver_support) {
				unsigned int offset = e2p_flash_offset;
				unsigned int eeprom_start = 0;
				unsigned int eeprom_end = e2p_size - 1;

				if (strcmp(cli_cmds[2], "all") != 0) {
					if (parsing_ee_info(cli_cmds[2], &eeprom_start, &eeprom_end, &offset) == 0)
						printf("sync eeprom[0x%x:0x%x] to 0x%x\n", eeprom_start, eeprom_end, offset);
					else {
						printf("Syntax error, %s(%s)\n", cli_cmds[2], strerror(errno));
						goto error_out;
					}
				}

				cli_sync_eeprom(eeprom_start, eeprom_end, offset);
			} else {
				printf("proprietary_driver_support is disabled, no need to write flash through ate daemon.\n");
			}
		}
	}

error_out:
	return 0;
}


int main(int argc, char *argv[])
{
	pid_t pid;
	int c = 0;	/* For user input */
	int tmp = 0;	/* For temporary usage */
	unsigned char l1p_str_value[100] , *cli_cmd = NULL;

#ifdef SIGNAL
	init_signals();
#endif
	/* initialize interface */
	os_memset(bridge_ifname, 0, IFNAMSIZ);
	os_memset(bridge_mac, 0, 6);

	for (tmp = 0; tmp < NUM_DRIVER_IF; tmp++) {
		os_memset(driver_ifname[tmp], '\0', IFNAMSIZ);
		dri_fd[tmp] = NULL;
	}

	/* set default interface name */
#ifdef CONFIG_SUPPORT_OPENWRT
	os_memcpy(bridge_ifname, "br-lan", strlen("br-lan"));
#else
	os_memcpy(bridge_ifname, "br0", 4);
#endif
	os_memcpy(driver_ifname, "ra0", 4);	/*Act as old agent*/

	/* get interface name from arguments */
	for (;;) {
		c = GetOpt(argc, argv, "b:hufi:m:vdql:c:xe:");

		if (c < 0)
			break;

		switch (c) {
		case 'm': {
			unsigned char tmp_mac[6];

			need_set_mac = 1;
			bUnicast = TRUE;
			tmp = os_strlen(optargs) + 1;
			sscanf(optargs, "%2x:%2x:%2x:%2x:%2x:%2x", (unsigned int *)&tmp_mac[0], (unsigned int *)&tmp_mac[1], (unsigned int *)&tmp_mac[2], (unsigned int *)&tmp_mac[3], (unsigned int *)&tmp_mac[4], (unsigned int *)&tmp_mac[5]);
			os_memcpy(bridge_mac, tmp_mac, ETH_ALEN);
		}
		break;

		case 'b': {
			/* Default the host interface is set to Ethernet */
			tmp = os_strlen(optargs) + 1;
			os_memset(bridge_ifname, 0, IFNAMSIZ);
			os_memcpy(bridge_ifname, optargs, tmp);

			if (tmp > IFNAMSIZ)
				return -1;

			break;
		}

		case 'h':
			Usage();
			return -1;

		case 'u':
			bUnicast = TRUE;
			break;

		case 'f':
			do_fork = 1;
			break;

		case 'i': {
			printf("if_name: %s\n", optargs);
			tmp = os_strlen(optargs) + 1;
			int index = dri_if_num;
			os_memcpy(&driver_ifname[index], optargs, tmp);

			if (tmp > IFNAMSIZ)
				return -1;

			dri_if_num++;
			is_default_if = 0;
			break;
		}

		case 'v':
			printf("%s\n", ate_daemon_version);
			break;

		case 'd':
#ifndef DBG
			printf("Debugging disabled without "
				   "-DDBG compile time "
				   "option.\n");
			return -1;
#else /* !DBG */
			ate_debug_level--;
			break;
#endif /* !DBG */

		case 'q':
#ifndef DBG
			printf("Debugging disabled without "
				   "-DDBG compile time "
				   "option.\n");
			return -1;
#else /* !DBG */
			ate_debug_level++;
			break;
#endif /* !DBG */

		case 'l':
			if(strcmp(l1profile_path, optargs)) {
				strcpy(l1profile_path, optargs);

				ate_printf(MSG_INFO ,"l1profile.dat from %s to %s\n", l1profile_path, optargs);
			} else {
				ate_printf(MSG_INFO ,"l1profile.dat remain %s\n", l1profile_path);
			}
			break;

		case 'x':
			proprietary_driver_support = FALSE;
			break;

		case 'c':
			cli_cmd = (unsigned char *)malloc(strlen(optargs)+1);
			if(cli_cmd)
				strcpy(cli_cmd, optargs);
			else
				ate_printf(MSG_ERROR, "alloc memory for cli command failed.");
			break;

		case 'e':
			sscanf(optargs, "[%04x:%04x]", &e2p_flash_offset, &e2p_size);
			printf("EEPROM data locate at flash offset:0x%04x, length:0x%04x\n", e2p_flash_offset, e2p_size);
			break;

		default:
			/* error */
			Usage();
			return -1;
		}
	}

	if(access(l1profile_path, F_OK) == 0) {
		/* initialize e2p_buffer, e2p_offset, e2p_size,  e2p_binary_path*/
		if(get_from_l1profile(driver_ifname, l1profile_path, "EEPROM_size", l1p_str_value) == 0)
			e2p_size = strtol(l1p_str_value, NULL, 16);

		if(get_from_l1profile(driver_ifname, l1profile_path, "EEPROM_offset", l1p_str_value) == 0)
			e2p_flash_offset = strtol(l1p_str_value, NULL, 16);

		if(get_from_l1profile(driver_ifname, l1profile_path, "EEPROM_name", l1p_str_value) == 0)
			strcat(e2p_binary_path, l1p_str_value);
	} else if (e2p_size < 1 && proprietary_driver_support) {
		printf("%s is not valid, please declare eeprom information.\n", l1profile_path);
		printf("\neq. ated -ira0 -e[0:4000], means ated communicate with interface ra0,\n");
		printf("    and the eeprom data locate at flash offset 0x0 with length, 0x4000.\n");

		exit(0);
	}

	if(proprietary_driver_support) {
		e2p_buffer = (unsigned char *)malloc(e2p_size);
		if(e2p_buffer) {
			os_memset(e2p_buffer, 0x0, e2p_size);
			e2p_offset = 0;
		} else {
			ate_printf(MSG_ERROR, "Allocate memory for storing e2p failed!\n");
			exit(0);
		}
	}

	if(cli_cmd) {
		/* Utility mode */
		process_cli_cmd(cli_cmd);
		exit(0);
	}

	/* To Do: Put it into DBG mode */
	/* Clean ATED has been executed before */
	clean_past_ated();

	/* background ourself */
	if (do_fork)
		pid = fork();
	else
		pid = getpid();

	switch (pid) {
	case -1:
		/* error */
		perror("fork/getpid");
		return -1;

	case 0:
		/* child, success */
		break;

	default:

		/* parent, success */
		if (do_fork)
			return 0;

		break;
	}

	/* Default the host interface is set to Ethernet */
	if (is_default_if == 1)
		dri_if_num = 1;

	/* Initialize Host Interface */
	host_fd = malloc(sizeof(*host_fd));
	ate_printf(MSG_INFO, "%d, malloc host_fd\n", getpid());
	host_fd->init_if = &init_eth;

	if (bUnicast)
		host_fd->unicast = 1;

	if (need_set_mac) {
		host_fd->need_set_mac = 1;
		os_memcpy(&host_fd->ownmac, &bridge_mac, 6);
	}
	else
	{
		host_fd->need_set_mac = 0;
	}

	sock = host_fd->init_if(host_fd, bridge_ifname);

	/* Initialize Driver Interface */
	for (tmp = 0; tmp < dri_if_num; tmp++) {
		dri_fd[tmp] = (struct DRI_IF *)malloc(sizeof(*dri_fd[tmp]));
		ate_printf(MSG_INFO, "%d, malloc dri_fd[%d]\n", getpid(), tmp);
		memset(dri_fd[tmp], 0, sizeof(*dri_fd[tmp]));

		if (dri_fd[tmp] == NULL) {
			ate_printf(MSG_ERROR, "Socket error in IOCTL\n");
			return -1;
		}

		dri_fd[tmp]->init_if = &init_if_ioctl;
		dri_fd[tmp]->init_if(dri_fd[tmp], driver_ifname[tmp]);
		dri_fd[tmp]->status = 0;
#ifdef MT_ATED_THREAD
		dri_fd[tmp]->ops = &thread_ops;
#elif MT_ATED_SHM
		dri_fd[tmp]->ops = &fork_ops;
#elif MT_ATED_IPC_SOCK
		dri_fd[tmp]->ops = &ipc_sock_ops;
#endif
		/* Init Multi-Processing */
		dri_fd[tmp]->ops->multi_proc_init((void **)dri_fd, tmp, dri_if_num, pkt_proc_logic);
	}

	/* While loop util program finish */
	RaCfg_Agent();
	ate_printf(MSG_INFO, "Program is finished, If num[%d]\n", dri_if_num);

	for (tmp = 0; tmp < dri_if_num; tmp++) {
		if (dri_fd[tmp]) {
			ate_printf(MSG_INFO, "%d, free dri_fd[%d]\n", getpid(), tmp);
			dri_fd[tmp]->ops->multi_proc_close(dri_fd[tmp]);
			free(dri_fd[tmp]);
			dri_fd[tmp] = NULL;
		}
	}

	exit(0);
}

static void RaCfg_Agent(void)
{
	int n, count;
	fd_set readfds;

	if (sock < 0) {
		ate_printf(MSG_ERROR, "No socket, so return.\n");
		return;
	}

	ate_printf(MSG_INFO, "28xx ATE agent program start(%d)\n", getpid());

	while (signup_flag) {
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		count = select(sock + 1, &readfds, NULL, NULL, NULL);

		if (count < 0) {
			ate_printf(MSG_WARNING, "select failed():");
			continue;
		} else if (count == 0) {
			continue;
			/* usleep(1000); */
		} else {
			/* Use Thread to Process IOCTL on Each Interface */
			unsigned char *cmd = malloc(sizeof(char) * PKT_BUF_SIZE);
			os_memset(cmd, 0, PKT_BUF_SIZE);

			if (FD_ISSET(sock, &readfds)) {
				n = host_fd->rev_cmd(cmd, PKT_BUF_SIZE);

				if (n > 0) {
					if (cmd == NULL) {
						ate_printf(MSG_ERROR, "No space for packet from host(malloc)\n");
						break;
					}

					NetReceive(cmd + ETH_HLEN, n);
				}
			}

			free(cmd);
		}
	}

	ate_printf(MSG_DEBUG, "28xx ATE agent is closed.\n");
}

static void NetReceive(UINT8 *inpkt, int len)
{
	struct racfghdr *p_racfgh = NULL;
	UINT32 magic_no;
	UINT16 command_type, command_id;

	p_racfgh = (struct racfghdr *)inpkt;
	magic_no = be2cpu32(p_racfgh->magic_no);

	if (magic_no == RACFG_MAGIC_NO) {
		command_type = be2cpu16(p_racfgh->comand_type);
		command_id = be2cpu16(p_racfgh->comand_id);

		if (((command_type & RACFG_CMD_TYPE_PASSIVE_MASK) != RACFG_CMD_TYPE_ETHREQ) &&
			((command_type & RACFG_CMD_TYPE_PASSIVE_MASK) != RACFG_CMD_TYPE_PLATFORM_MODULE)) {
			ate_printf(MSG_ERROR, "Command_Type error = %x\n", command_type);
			return;
		}
#ifdef CONFIG_PLATFORM_MODULE_CMD_PATH
		else if (command_type == RACFG_CMD_TYPE_PLATFORM_MODULE) {
			ate_printf(MSG_DEBUG, "command_type = %x, it's a cmd for platform module.\n", command_type);
			PlatformModuleProcess(inpkt, len, 0);
		}
#endif
		task_dispatcher(inpkt, len, 0);
	} else if (magic_no == NEW_MAGIC_NO) {
		struct new_cfghdr *p_newhdr = (struct new_cfghdr *) inpkt;
		char *if_name = p_newhdr->if_name;
		int if_index;

		version = p_newhdr->ver_id;

		switch (version) {
		case 0:
			/* Old agent should not happen here*/
			break;

		case 1:
			ate_printf(MSG_INFO, "New Agent Version 1\n");

		default:
			/* Find Match MAC in interface */
			if_index = if_match(if_name);

			if (if_index != -1)
				task_dispatcher(inpkt, len, if_index);
			else {
				ate_printf(MSG_ERROR, "Unknwon Cmd: %04x", p_newhdr->comand_id);

				if (p_newhdr->comand_id == 0xffff) {
				}
			}

			break;
		}
	} else {
		ate_printf(MSG_ERROR, "Magic Error: %02x\n", magic_no);
		return;
	}
}

static void task_dispatcher(UINT8 *inpkt, int len, int if_index)
{
	struct DRI_IF *p_if = NULL;
	struct MULTI_PROC_OPS *q_ops;
	struct COMMON_PRIV *priv_data;
	struct cmd_queue *q;
	int ret = 0;

	p_if = dri_fd[if_index];

	if (p_if == NULL)
		ate_printf(MSG_ERROR, "NULL fd\n");

	priv_data = (struct COMMON_PRIV *)p_if->priv_data;
	q = &priv_data->q;
	q_ops = p_if->ops;
	q_ops->multi_lock_q(p_if);	/* Lock Queue */

	if (((q->un_served + 1) % CMD_QUEUE_SIZE) == q->served) {
		ate_printf(MSG_WARNING, "Interface[%s], queue Full, served:%d, unserved:%d\n", p_if->ifname, q->served, q->un_served);
		q_ops->multi_sig_data(p_if);	/* Singal pkt_proc_logc */
		q_ops->multi_unlock_q(p_if);
		p_if->status = 2;
	} else {
		ret = q_ops->multi_insert_q(p_if, inpkt - ETH_HLEN, len + ETH_HLEN);

		if (ret != (len + ETH_HLEN))
			ate_printf(MSG_ERROR, "Broken PIPE, ret: %d\n", ret);

		ate_printf(MSG_DEBUG, "Enqueue packet len %d\n", len + ETH_HLEN);
		q->cmd_len[q->un_served] = len + ETH_HLEN;
		q->un_served = (q->un_served + 1) % CMD_QUEUE_SIZE;
		ate_printf(MSG_DEBUG, "Interface[%s], queue inserted, served:%d, unserved:%d\n", p_if->ifname, q->served, q->un_served);
		q_ops->multi_sig_data(p_if);	/* Singal pkt_proc_logc */
		q_ops->multi_unlock_q(p_if);
	}
}

#ifdef CONFIG_PLATFORM_MODULE_CMD_PATH
static void PlatformModuleProcess(UINT8 *inpkt, int len, int if_index)
{
	struct racfghdr *p_racfgh = NULL;
	struct DRI_IF *p_if = NULL;
	UINT16 Command_Id;

	p_racfgh = (struct racfghdr *)inpkt;
	p_if = dri_fd[if_index];

	if (p_if == NULL)
		ate_printf(MSG_ERROR, "NULL fd.\n");

	ate_printf(MSG_DEBUG, "%s: Msg for flow check.\n", __func__);
	os_memcpy(&Command_Id, (unsigned char *)(&p_racfgh->comand_id), sizeof(Command_Id));
	Command_Id = be2cpu16(Command_Id);

	if (Command_Id == RACFG_CMD_TRIGGER_PLATFORM) {
		UINT32 start = 0;
		UINT32 platform_id = 0;
		unsigned char *tmp = p_racfgh->data;

		os_memcpy(&start, tmp, sizeof(start));
		tmp += sizeof(start);
		os_memcpy(&platform_id, tmp, sizeof(platform_id));
		start = be2cpu32(start);
		platform_id = be2cpu32(platform_id);

		if (start == 1) {
			switch (platform_id) {
			case 0x1:
				ate_printf(MSG_INFO, "Trigger platfrom function: DRAM\n");
				system("platform_module.sh DRAM &");
				break;

			case 0x2:
				ate_printf(MSG_INFO, "Trigger platfrom function: PCIe\n");
				system("platform_module.sh PCIE &");
				break;

			case 0x3:
				ate_printf(MSG_INFO, "Trigger platfrom function: SATA\n");
				system("platform_module.sh SATA &");
				break;

			case 0x4:
				ate_printf(MSG_INFO, "Trigger platfrom function: USB3\n");
				system("platform_module.sh USB3 &");
				break;

			case 0x5:
				ate_printf(MSG_INFO, "Trigger platfrom function: USB2\n");
				system("platform_module.sh USB2 &");
				break;

			case 0x6:
				ate_printf(MSG_INFO, "Trigger platfrom function: EPHY\n");
				system("platform_module.sh EPHY &");
				break;

			case 0x7:
				ate_printf(MSG_INFO, "Trigger platfrom function: RGMII\n");
				system("platform_module.sh RGMII &");
				break;

			case 0x8:
				ate_printf(MSG_INFO, "Trigger platfrom function: SGMII\n");
				system("platform_module.sh SGMII &");
				break;

			case 0x9:
				ate_printf(MSG_INFO, "Trigger platfrom function: NAND\n");
				system("platform_module.sh NAND &");
				break;

			case 0xA:
				ate_printf(MSG_INFO, "Trigger platfrom function: SDXC\n");
				system("platform_module.sh SDXC &");
				break;

			case 0xB:
				ate_printf(MSG_INFO, "Trigger platfrom function: ALL1(RGMII)\n");
				system("platform_module.sh ALL1 &");
				break;

			case 0xC:
				ate_printf(MSG_INFO, "Trigger platfrom function: ALL2(SGMII)\n");
				system("platform_module.sh ALL2 &");
				break;

			default:
				ate_printf(MSG_INFO, "Not supported platfrom function: %d.\n", platform_id);
				break;
			}
		} else if (start == 0) {
			ate_printf(MSG_INFO, "Stop platfrom function: %d\n", platform_id);
			system("platform_module.sh STOP &");
			system("sleep 2");
			system("platform_module.sh STOP &");
		}
	}
}
#endif

static void pkt_proc_logic(void *arg)
{
	struct DRI_IF *dri_if = arg;
	struct COMMON_PRIV *priv_data;
	struct cmd_queue *q;
	int unserved = 0;
	int served = 0;
	static int num_pkt;
	struct MULTI_PROC_OPS *q_ops;
	struct racfghdr *cmd_hdr = NULL;
	struct DRI_IF *p_if = NULL;
	UINT16 Command_Type;
	UINT16 Command_Id;
	UINT16 Len;

	ate_printf(MSG_INFO, "Multi-processing for serving command packet has started for dri_if[%s],PId:%d\n", dri_if->ifname, getpid());
	q_ops = dri_if->ops;
	priv_data = (struct COMMON_PRIV *)dri_if->priv_data;
	q = &priv_data->q;

	/* Get unserved number */
	while (signup_flag) {
		unsigned char *pkt;
		struct racfghdr *p_tmp = NULL;
		int len = -1;
		int len_to_host = ETH_HLEN;

		/* Lock queue */
		q_ops->multi_lock_q(dri_if);
		unserved = q->un_served;
		served = q->served;

		while (served == unserved && signup_flag) {
			ate_printf(MSG_DEBUG, "[%d]Wait for cond SERVED/UN-SERVED(%d:%d)\n", getpid(), served, unserved);
			dri_if->status = 0;
			q_ops->multi_wait_data(dri_if);

			if (!signup_flag)
				goto clean_multi_proc;

			unserved = q->un_served;
			served = q->served;
		}

		dri_if->status = 1;
		pkt = q->cmd_arr[served];
		p_tmp = (struct racfghdr *)(pkt + ETH_HLEN);
		len = q->cmd_len[served] - ETH_HLEN;

		if (pkt == NULL) {
			ate_printf(MSG_ERROR, "Null pkt pointer\n");
			break;
		}

		/* Get flash partition offset from l1profile and write value into flash */
		if (proprietary_driver_support) {
			UINT16 command_id;
			unsigned char offset_char[100];

			command_id = be2cpu16(p_tmp->comand_id);

			switch (command_id)
			{
				case HQA_WriteBufferDone:
				case HQA_WriteBulkEEPROM:
					mt_e2p_write(p_tmp, e2p_flash_offset);
					update_eeprom_binary(e2p_binary_path, e2p_buffer, e2p_flash_offset, EEPROM_SIZE);
					break;

				default:
					break;
			}
		}

		/* Unlock queue */
		q_ops->multi_unlock_q(dri_if);

		if (be2cpu32/*ntohl*/(p_tmp->magic_no) == RACFG_MAGIC_NO) {
			ate_printf(MSG_INFO, "Old Magic Packet(%d)\n", len);
			len_to_host += processPktOldMagic((UINT8 *)p_tmp, len, 0);
		} else if (be2cpu32/*ntohl*/(p_tmp->magic_no) == NEW_MAGIC_NO)
			len_to_host += processPktNewMagic((UINT8 *)p_tmp, len);

		host_fd->rsp2host(pkt, len_to_host);
		cmd_hdr = (struct racfghdr *) p_tmp;
		os_memcpy(&Command_Type, (unsigned char *)(&cmd_hdr->comand_type), sizeof(Command_Type));
		os_memcpy(&Command_Id, (unsigned char *)(&cmd_hdr->comand_id), sizeof(Command_Id));
		os_memcpy(&Len, (unsigned char *)(&cmd_hdr->length), sizeof(Len));
		Command_Type = be2cpu16(Command_Type);
		Command_Id = be2cpu16(Command_Id);
		Len	= be2cpu16(Len);
		p_if = dri_fd[0];

		if (Command_Id == HQA_SetBandMode) {
			ate_printf(MSG_INFO, "%s: HQA_SetBandMode\n", __func__);
			sleep(3);
			hqa_set_band(p_if, cmd_hdr->data, Len);
		}

		/* Read eeprom value response from driver */
		if (proprietary_driver_support) {
			if (Command_Id == HQA_ReadBulkEEPROM)
				mt_e2p_read(p_tmp);
		}
		num_pkt++;
		/* Update Ring Buffer */
		ate_printf(MSG_DEBUG, "Lock in pkt_process_logic\n");
		q_ops->multi_lock_q(dri_if);
		os_memset(q->cmd_arr[q->served], 0, PKT_BUF_SIZE);
		q->cmd_len[q->served] = -1;
		q->served = (q->served + 1) % CMD_QUEUE_SIZE;
		ate_printf(MSG_DEBUG, "Un-lock in pkt_process_logic\n");
		q_ops->multi_unlock_q(dri_if);
	}

clean_multi_proc:
	ate_printf(MSG_DEBUG, "Clean dri_cmd_q(%s)\n", dri_if->ifname);
	q_ops->multi_proc_close(dri_if);
	dri_if->close(dri_if);
	free(dri_if);
	dri_if_num--;
}

static int processPktOldMagic(UINT8 *inpkt, int len, int if_index)
{
	struct racfghdr *cmd_hdr = NULL;
	struct DRI_IF *p_if = NULL;
	int ret = 0;
#ifdef DBG /*DEBUG FER*/
	static int reset_cnt;
	static int start_rx_cnt;
#endif
	UINT16 Command_Type;
	UINT16 Command_Id;
	UINT16 Len;

	cmd_hdr = (struct racfghdr *) inpkt;
	os_memcpy(&Command_Type, (unsigned char *)(&cmd_hdr->comand_type), sizeof(Command_Type));
	os_memcpy(&Command_Id, (unsigned char *)(&cmd_hdr->comand_id), sizeof(Command_Id));
	os_memcpy(&Len, (unsigned char *)(&cmd_hdr->length), sizeof(Len));
	Command_Type = be2cpu16(Command_Type);
	Command_Id = be2cpu16(Command_Id);
	Len	= be2cpu16(Len);
	/* Default old agent use 1 interface*/
	p_if = dri_fd[if_index];

	if (p_if == NULL) {
		ate_printf(MSG_ERROR, "NULL fd\n");
		return -1;
	}

#ifdef DBG
	ate_printf(MSG_INFO, "Command_Id :%04x\n", Command_Id);
	ate_printf(MSG_INFO, "Len          :%04x\n", Len);
	{
		int j = 0;

		for (j = 0; j < Len; j++)
			ate_printf(MSG_INFO, "%x", cmd_hdr->data[j]);
	}
#endif
	ret = p_if->send(p_if, inpkt, Len);

	if (ret)
		ate_printf(MSG_ERROR, "%s::ioctl err %d\n", __func__, ret);

#if 0

	if (Command_Id == HQA_SetBandMode) {
		ate_printf(MSG_ERROR, "%s::HQA_SetBandMode\n", __func__);
		hqa_set_band(p_if, cmd_hdr->data, Len);
	}

#endif
	
	/* add ack bit to command type */
	Command_Type = Command_Type | (~RACFG_CMD_TYPE_PASSIVE_MASK);
	Command_Type = cpu2be16(Command_Type);
	os_memcpy((unsigned char *)&cmd_hdr->comand_type, &Command_Type, sizeof(Command_Type));
	os_memcpy(&Len, (unsigned char *)&cmd_hdr->length, sizeof(Len));
	Len	= be2cpu16(Len);
	return Len + RA_CFG_HLEN;
}
/*
*	Support Unicast ONLY
*/
static int processPktNewMagic(UINT8 *inpkt, int len)
{
	struct new_cfghdr *p_newhdr = (struct new_cfghdr *) inpkt;
	struct DRI_IF *if_to_send = NULL;
	char *if_name = p_newhdr->if_name;
	int if_index;
	int ret_len = NEW_MAGIC_ADDITIONAL_LEN;
	version = p_newhdr->ver_id;
	unsigned char *old_format_head = inpkt + NEW_MAGIC_ADDITIONAL_LEN;

	switch (version) {
	case 0:
		/* Old agent should not happen here*/
		break;

	case 1:
		ate_printf(MSG_ERROR, "New Agent Version 1\n");

	default:
		/* Find Match MAC in interface */
		if_index = if_match(if_name);

		if (if_index != -1) {
			if_to_send = dri_fd[if_index];

			if (if_to_send != NULL) {
				ate_printf(MSG_ERROR, "Find Corresponding MAC Address\n");
				ret_len += processPktOldMagic(old_format_head, len - NEW_MAGIC_ADDITIONAL_LEN, if_index);
			} else
				ate_printf(MSG_ERROR, "if_to_send NULL Pointer\n");
		} else {
			/* Fail to find the same MAC, PLZ Error Handle, OR its first time */
			ate_printf(MSG_ERROR, "Cmd: %04x", p_newhdr->comand_id);

			if (p_newhdr->comand_id == 0xffff) {
			}
		}

		break;
	}

	return ret_len;
}

/*
*	Use optstring as a pattern, and comparing argv[x] with optstring to parse variables
*/
static int GetOpt(int argc, char *const argv[], const char *optstring)
{
	static int optchr = 1;
	char *cp;

	if (optchr == 1) {
		if (optindex >= argc) {
			/* all arguments processed */
			return EOF;
		}

		if (argv[optindex][0] != '-' || argv[optindex][1] == '\0') {
			/* no option characters */
			return EOF;
		}
	}

	if (os_strcmp(argv[optindex], "--") == 0) {
		/* no more options */
		optindex++;
		return EOF;
	}

	optopts = argv[optindex][optchr];
	cp = os_strchr(optstring, optopts);

	if (cp == NULL || optopts == ':') {
		if (argv[optindex][++optchr] == '\0') {
			optchr = 1;
			optindex++;
		}

		return '?';
	}

	if (cp[1] == ':') {
		/* Argument required */
		optchr = 1;

		if (argv[optindex][optchr + 1]) {
			/* No space between option and argument */
			optargs = &argv[optindex++][optchr + 1];
		} else if (++optindex >= argc) {
			/* option requires an argument */
			return '?';
		} else {
			/* Argument in the next argv */
			optargs = argv[optindex++];
		}
	} else {
		/* No argument */
		if (argv[optindex][++optchr] == '\0') {
			optchr = 1;
			optindex++;
		}

		optargs = NULL;
	}

	return *cp;
}

static void Usage(void)
{
	printf("%s\n\n\n"
		"usage:\n"
		"  ated [-huvd]"
		"[-b<br_ifname>] \\\n"
		"[-i<driver_ifname>] \\\n"
		"\n",
		ate_daemon_version);
	printf("options:\n"
		"  -b = bridge interface name\n"
		"  -h = show this help text\n"
		"  -u = respond to QA by unicast frame\n"
		"  -f = daemonize ATED\n"
		"  -i = driver interface name\n"
		"  -v = show version\n"
		"  -d = increase debugging verbosity (-dd even more)\n"
		"  -q = decrease debugging verbosity (-qq even less)\n"
		"  -l = path of l1profile.dat\n"
		"  -x = disable mtd flash read/write by ATED feature\n"
		"  -c = CLI mode to process predefined command(s)\n");
	printf("example 1:\n"
		"  ated -h\n");
	printf("example 2:\n"
		"  ated -b br1 -i ra1 -v\n");
	printf("example 3:\n"
		"  ated -u\n");
	printf("example 4:\n"
		"  ated -d\n");
	printf("example 5 for Dual Adapter and QA support Dual Adapter:\n"
		"  ated -i ra0 -i ra1\n");
	printf("example 6 change dev mac address:\n"
		"  ated -b br0 -i ra0 -m mac_addr\n");
	printf("example 7 change path of l1profile:\n"
		"  ated -l/etc/wireless/l1profile.dat\n");
	printf("example 8 disable mtd flash read/write by ATED feature\n"
		"  ated -x\n");
	printf("example 9 cli mode with interface:\n"
		"  ated -irai0 -c\"sync eeprom all\"\n"
		"  ated -irai0 -c\"sync eeprom 2f00[20:2f]\" (w/\o l1profile.dat)\n");
	printf("example 10 with interface and eeprom infomation: (w\/o l1profile.dat)\n"
		"  ated -ira0 -e[0:4000], eeprom locate at flash offset 0x0 with length, 0x4000\n");
}
#if 0	/* Not used in ate-ext.c */
static void sendATESTOP(int index)
{
	unsigned char *pkt = malloc(sizeof(struct racfghdr));
	struct racfghdr *hdr = (struct racfghdr *)pkt;
	/* Send ATESTOP command to driver before I am killed by command line(not by GUI). */
	bzero(hdr, sizeof(hdr));
	hdr->magic_no =  cpu2be32(RACFG_MAGIC_NO);
	hdr->comand_type = cpu2be16(RACFG_CMD_TYPE_ETHREQ);
	hdr->comand_id = cpu2be16(RACFG_CMD_ATE_STOP);
	hdr->length = 0;
	hdr->sequence = 0;

	/* Conditional Branch to Check if its new DLL or old DLL*/
	if (0) {
		/* Parse */
	} else {
		/* Default old agent use 1 interface*/
		dri_fd[index]->send(dri_fd[index], pkt, 0);
	}
}

static void sendATESTART(int index)
{
	unsigned char *pkt = malloc(sizeof(struct racfghdr));
	struct racfghdr *hdr = (struct racfghdr *)pkt;
	/* Send ATESTOP command to driver before I am killed by command line(not by GUI). */
	bzero(hdr, sizeof(hdr));
	hdr->magic_no =  cpu2be32(RACFG_MAGIC_NO);
	hdr->comand_type = cpu2be16(RACFG_CMD_TYPE_ETHREQ);
	hdr->comand_id = cpu2be16(RACFG_CMD_ATE_START);
	hdr->length = 0;
	hdr->sequence = 0;

	/* Conditional Branch to Check if its new DLL or old DLL*/
	if (0) {
		/* Parse */
	} else {
		/* Default old agent use 1 interface*/
		dri_fd[index]->send(dri_fd[index], pkt, 0);
	}
}
#endif
