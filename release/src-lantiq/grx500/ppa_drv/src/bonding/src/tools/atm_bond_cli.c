
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <ima_atm_bond_ioctl.h>

struct cmd_options {
	const char *name; /* command name */
	int        (*handler)(int argc, char *argv[]); /* command handler */
};

static void atm_bond_cli_help(void)
{
	printf("Usage: atm_bond_cli <command> [<args>]\n");
	printf("  set_tx_sid_offset - Set Tx SID offset\n");
	printf("  set_tx_sid        - Set Tx SID\n");
	printf("  set_tx_qdc        - Set Tx Queue delay compensation\n");
	printf("  set_tx_abort      - Set Tx transmit abort\n");
	printf("  set_tx_flush      - Set Tx transmit flush\n");
	printf("  set_tx_delay      - Set Tx link-specific delay\n");
	printf("  set_rx_sid_offset - Set Rx SID offset\n");
	printf("  set_rx_sid        - Set Rx SID\n");
	printf("  set_rx_sid_skip   - Set Rx SID skip\n");
	printf("  set_rx_timeout    - Set Rx timeout\n");
	printf("  set_rx_full       - Set Rx full\n");
	printf("  get_stats         - Get stats\n");
	printf("  set_rx_asm        - Set Rx ASM dump\n");
	printf("  set_tx_asm        - Set Tx ASM dump\n");
	printf("  set_qos_link_rate - Set QoS link rate\n");
}

static int atm_bond_ioctl_cmd(int cmd, void *data)
{
	int ret = 0;
	int fd;

	fd = open(IMA_BONDING_DEVICE_NODE, O_RDWR);
	if (fd < 0) {
		printf("\nopen atm_bond device (%s) failed. (errno=%d)\n",
			IMA_BONDING_DEVICE_NODE, errno);
		ret = -1;
	} else {
		if (ioctl(fd, cmd, data) < 0) {
			printf("\nioctl failed for cmd %d. (errno=%d(system error:%s))\n",
				cmd, errno, strerror(errno));
			ret = -1;
		}
		close(fd);
	}

	return ret;
}

/* cmd: set_tx_sid_offset */
static const char set_tx_sid_offset_sopts[] = "o:c:h";
static struct option set_tx_sid_offset_lopts[] = {
	{ "sid-offset", required_argument, NULL, 'o' },
	{ "no-of-cells", required_argument, NULL, 'c' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_tx_sid_offset_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_sid_offset <args>\n");
	printf("  -o/--sid-offset  - SID offset\n");
	printf("  -c/--no-of-cells - Number of cells\n");
	printf("  -h/--help        - Command help\n");
}

static int set_tx_sid_offset_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_tx_sid_offset tx_sid_offset = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_tx_sid_offset_sopts,
                        set_tx_sid_offset_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'o':
			tx_sid_offset.offset = atoi(optarg);
			break;
		case 'c':
			if(atoi(optarg) <= 0) {
				err_help = 1;
				printf("No of cells should be +ve\n");
			}

			tx_sid_offset.no_of_cells = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_tx_sid_offset_cmd_help();
		return -1;
	}

	printf("tx_sid_offset: offset=%d no_of_cells=%d\n",
		tx_sid_offset.offset, tx_sid_offset.no_of_cells);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_SID_OFFSET, &tx_sid_offset);
}

/* cmd: set_tx_sid */
static const char set_tx_sid_sopts[] = "s:c:h";
static struct option set_tx_sid_lopts[] = {
	{ "sid-val", required_argument, NULL, 's' },
	{ "no-of-cells", required_argument, NULL, 'c' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_tx_sid_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_sid <args>\n");
	printf("  -s/--sid-val     - SID value\n");
	printf("  -c/--no-of-cells - Number of cells\n");
	printf("  -h/--help        - Command help\n");
}

static int set_tx_sid_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_tx_sid_set tx_sid_set = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_tx_sid_sopts,
                        set_tx_sid_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 's':
			if(atoi(optarg) < 0) {
				err_help = 1;
				printf("SID value should be +ve\n");
			}

			tx_sid_set.value = atoi(optarg);
			break;
		case 'c':
			if(atoi(optarg) <= 0) {
				err_help = 1;
				printf("No of cells should be +ve\n");
			}

			tx_sid_set.no_of_cells = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_tx_sid_cmd_help();
		return -1;
	}

	printf("tx_sid_set: value=%d no_of_cells=%d\n",
		tx_sid_set.value, tx_sid_set.no_of_cells);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_SID_SET, &tx_sid_set);
}

/* cmd: set_tx_qdc */
static const char set_tx_qdc_sopts[] = "d:h";
static struct option set_tx_qdc_lopts[] = {
	{ "delay", required_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_tx_qdc_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_qdc <args>\n");
	printf("  -d/--delay - Delay\n");
	printf("  -h/--help  - Command help\n");
}

static int set_tx_qdc_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_tx_qdc tx_qdc = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_tx_sid_sopts,
                        set_tx_sid_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'd':
			tx_qdc.delay = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 4) || err_help) {
		set_tx_qdc_cmd_help();
		return -1;
	}

	printf("tx_qdc: delay=%lld\n", tx_qdc.delay);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_QDC, &tx_qdc);
}

/* cmd: set_tx_abort */
static void set_tx_abort_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_abort\n");
	printf("  -h/--help - Command help\n");
}

static int set_tx_abort_cmd(int argc, char *argv[])
{
	if (argc > 2) {
		set_tx_abort_cmd_help();
		return -1;
	}

	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_ABORT, NULL);
}

/* cmd: set_tx_flush */
static void set_tx_flush_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_flush\n");
	printf("  -h/--help - Command help\n");
}

int set_tx_flush_cmd(int argc, char *argv[])
{
	if (argc > 2) {
		set_tx_flush_cmd_help();
		return -1;
	}

	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_FLUSH, NULL);
}

/* cmd: set_tx_delay */
static const char set_tx_delay_sopts[] = "l:d:h";
static struct option set_tx_delay_lopts[] = {
	{ "link-id", required_argument, NULL, 'l' },
	{ "delay", required_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_tx_delay_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_flush <args>\n");
	printf("  -l/--link-id - Link index\n");
	printf("  -d/--delay   - Delay in microseconds\n");
	printf("  -h/--help    - Command help\n");
}

static int set_tx_delay_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_tx_delay tx_delay = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_tx_delay_sopts,
                        set_tx_delay_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'l':
			if(atoi(optarg) < 0) {
				err_help = 1;
				printf("Link index should be +ve\n");
			}

			tx_delay.link = atoi(optarg);
			break;
		case 'd':
			if(atoi(optarg) < 0) {
				err_help = 1;
				printf("Delay should be +ve\n");
			}

			tx_delay.delay = atoll(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_tx_delay_cmd_help();
		return -1;
	}

	printf("tx_delay: link=%d delay=%lld\n",
		tx_delay.link, tx_delay.delay);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_DELAY, &tx_delay);
}

/* cmd: set_rx_sid_offset */
static const char set_rx_sid_offset_sopts[] = "l:o:c:h";
static struct option set_rx_sid_offset_lopts[] = {
	{ "link-id", required_argument, NULL, 'l' },
	{ "sid-offset", required_argument, NULL, 'o' },
	{ "no-of-cells", required_argument, NULL, 'c' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_rx_sid_offset_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_sid_offset <args>\n");
	printf("  -l/--link-id     - Link index\n");
	printf("  -o/--sid-offset  - SID offset\n");
	printf("  -c/--no-of-cells - Number of cells\n");
	printf("  -h/--help - Command help\n");
}

static int set_rx_sid_offset_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_rx_sid_offset rx_sid_offset = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_sid_offset_sopts,
                        set_rx_sid_offset_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'l':
			if(atoi(optarg) < 0) {
				printf("Link id should be +ve\n");
				err_help = 1;
			}

			rx_sid_offset.link = atoi(optarg);
			break;
		case 'o':
			rx_sid_offset.offset = atoi(optarg);
			break;
		case 'c':
			if(atoi(optarg) <= 0) {
				printf("No of cells should ve +ve\n");
				err_help = 1;
			}

			rx_sid_offset.no_of_cells = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 8) || err_help) {
		set_rx_sid_offset_cmd_help();
		return -1;
	}

	printf("rx_sid_offset: link=%d offset=%d no_of_cells=%d\n",
		rx_sid_offset.link, rx_sid_offset.offset, rx_sid_offset.no_of_cells);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_SID_OFFSET, &rx_sid_offset);
}

/* cmd: set_rx_sid */
static const char set_rx_sid_sopts[] = "l:s:c:h";
static struct option set_rx_sid_lopts[] = {
	{ "link-id", required_argument, NULL, 'l' },
	{ "sid-val", required_argument, NULL, 's' },
	{ "no-of-cells", required_argument, NULL, 'c' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_rx_sid_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_sid <args>\n");
	printf("  -l/--link-id     - Link index\n");
	printf("  -s/--sid-val     - SID value\n");
	printf("  -c/--no-of-cells - Number of cells\n");
	printf("  -h/--help - Command help\n");
}

static int set_rx_sid_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_rx_sid_set rx_sid_set = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_sid_sopts,
                        set_rx_sid_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'l':
			if(atoi(optarg) < 0) {
				printf("Link id should be +ve\n");
				err_help = 1;
			}

			rx_sid_set.link = atoi(optarg);
			break;
		case 's':
			if(atoi(optarg) < 0) {
				printf("SID value should ve +ve\n");
				err_help = 1;
			}

			rx_sid_set.value = atoi(optarg);
			break;
		case 'c':
			if(atoi(optarg) <= 0) {
				printf("No of cells should ve +ve\n");
				err_help = 1;
			}

			rx_sid_set.no_of_cells = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 8) || err_help) {
		set_rx_sid_cmd_help();
		return -1;
	}

	printf("rx_sid_set: link=%d value=%d no_of_cells=%d\n",
		rx_sid_set.link, rx_sid_set.value, rx_sid_set.no_of_cells);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_SID_SET, &rx_sid_set);
}

/* cmd: set_rx_sid_skip */
static const char set_rx_sid_skip_sopts[] = "l:c:h";
static struct option set_rx_sid_skip_lopts[] = {
	{ "link-id", required_argument, NULL, 'l' },
	{ "no-of-cells", required_argument, NULL, 'c' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_rx_sid_skip_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_sid_skip <args>\n");
	printf("  -l/--link-id     - Link index\n");
	printf("  -c/--no-of-cells - Number of cells\n");
	printf("  -h/--help        - Command help\n");
}

static int set_rx_sid_skip_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_rx_sid_skip rx_sid_skip = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_sid_skip_sopts,
                        set_rx_sid_skip_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'l':
			if(atoi(optarg) < 0) {
				printf("Link id should be +ve\n");
				err_help = 1;
			}

			rx_sid_skip.link = atoi(optarg);
			break;
		case 'c':
			if(atoi(optarg) <= 0) {
				printf("No of cells should ve +ve\n");
				err_help = 1;
			}

			rx_sid_skip.no_of_cells = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_rx_sid_skip_cmd_help();
		return -1;
	}

	printf("rx_sid_skip: link=%d no_of_cells=%d\n",
		rx_sid_skip.link, rx_sid_skip.no_of_cells);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_SID_SKIP, &rx_sid_skip);
}

/* cmd: set_rx_timeout */
static const char set_rx_timeout_sopts[] = "d:h";
static struct option set_rx_timeout_lopts[] = {
	{ "delay", required_argument, NULL, 'd' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_rx_timeout_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_timeout <args>\n");
	printf("  -d/--delay - Delay in microseconds\n");
	printf("  -h/--help  - Command help\n");
}

static int set_rx_timeout_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_rx_timeout rx_timeout = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_timeout_sopts,
                        set_rx_timeout_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'd':
			if(atoi(optarg) < 0) {
				printf("Delay should ve +ve\n");
				err_help = 1;
			}

			rx_timeout.delay = atoll(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 4) || err_help) {
		set_rx_timeout_cmd_help();
		return -1;
	}

	printf("rx_timeout: delay=%lld\n",
		rx_timeout.delay);
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_TIMEOUT, &rx_timeout);
}

/* cmd: set_rx_full */
static const char set_rx_full_sopts[] = "l:e:h";
static struct option set_rx_full_lopts[] = {
	{ "link-id", required_argument, NULL, 'l' },
	{ "enable", required_argument, NULL, 'e' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_rx_full_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_full <args>\n");
	printf("  -l/--link-id - Link index\n");
	printf("  -e/--enable  - Enable or disable\n");
	printf("  -h/--help    - Command help\n");
}

static int set_rx_full_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_rx_full rx_full = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_full_sopts,
                        set_rx_full_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case 'l':
			if(atoi(optarg) < 0) {
				printf("Link id should be +ve\n");
				err_help = 1;
			}

			rx_full.link = atoi(optarg);
			break;
		case 'e':
			rx_full.status = (0 == atoi(optarg)) ? disable : enable;
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_rx_full_cmd_help();
		return -1;
	}

	printf("rx_full: link=%d status=%s\n",
		rx_full.link, (enable == rx_full.status) ? "enabled" : "disabled");
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_FULL, &rx_full);
}

/* cmd: get_stats */
static void get_stats_cmd_help(void)
{
	printf("Usage: atm_bond_cli get_stats\n");
	printf("  -h/--help - Command help\n");
}

static int get_stats_cmd(int argc, char *argv[])
{
	int ret;
	struct st_stats stats = {0};

	if (argc > 2) {
		get_stats_cmd_help();
		return -1;
	}

	ret = atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_STATS, &stats);
	if (!ret) {

		printf("(TX)> AAL5 Frames => Waiting:%u, BackPressure:%llu, "
			"Cell Buffers => Malloc:%d, Free:%d, Lock:%d, Used:[SwQ:%d, 0:%d, 1:%d]\n",
			stats.host_txin_ring_count,
			stats.insufficient_tx_buffer_stack_backpressure,
			stats.cell_buff_mallocd,
			stats.cell_buff_free,
			stats.cell_buff_locked,
			stats.sched_ring_count,
			stats.tx_ep_count[0],
			stats.tx_ep_count[1]
		);

		printf("(TX)>   ATM Cells => ASM:[0:%llu, 1:%llu], "
			"Non-ASM:[0:%llu, 1:%llu], Dropped:[ASM0:%llu, "
			"ASM1:%llu, Non-ASM:%llu], BackPressure:[0:%llu, 1:%llu]\n",
			stats.tx_asm_count[0],
			stats.tx_asm_count[1],
			stats.tx_atm_count[0],
			stats.tx_atm_count[1],
			stats.tx_asm_dropped_count[0],
			stats.tx_asm_dropped_count[1],
			stats.tx_total_atm_dropped_count,
			stats.tx_no_ep_freespace_backpressure[0],
			stats.tx_no_ep_freespace_backpressure[1]
		);

		printf("(CO)>     TX:[0:%c, 1:%c], RX:[0:%c, 1:%c], Group lost cells:%llu\n", 
			stats.co_rx_link_status[0],
			stats.co_rx_link_status[1],
			stats.co_tx_link_status[0],
			stats.co_tx_link_status[1],
			stats.co_group_lost_cell
		);

		printf("(RX)>   ATM Cells => ASM:[0:%u, 1:%u], "
			"Non-ASM:[0:%u, 1:%u], Dropped:%u, Timed-out:%u, Missed:%u\n",
			stats.rx_asm_count[0],
			stats.rx_asm_count[1],
			stats.rx_atm_count[0],
			stats.rx_atm_count[1],
			stats.rx_dropped_cells,
			stats.rx_timedout_cells,
			stats.rx_missing_cells
		);

		printf("(RX)> AAL5 Frames => Accepted:%u, Discarded:[SID:%u, "
			"CRC/Length:%u, CLP/PT2:%u, Alloc-failure:%u, Size-Overflow:%u]\n", 
			stats.rx_accepted_frame_count,
			stats.rx_discarded_frame_count[SID_JUMP],
			stats.rx_discarded_frame_count[INVALID_LENGTH] + 
			stats.rx_discarded_frame_count[INVALID_CRC32],
			stats.rx_discarded_frame_count[INVALID_PT2_OR_CLP],
			stats.rx_discarded_frame_count[ALLOC_FAILURE],
			stats.rx_discarded_frame_count[SIZE_OVERFLOW]
		);
	}

	return ret;
}

/* cmd: set_rx_asm */
static const char set_rx_asm_sopts[] = "e:h";
static struct option set_rx_asm_lopts[] = {
	{ "enable", required_argument, NULL, 'e' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
static void set_rx_asm_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_rx_asm\n");
	printf("  -e/--enable  - Enable or disable\n");
	printf("  -h/--help - Command help\n");
}

static int set_rx_asm_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	st_print_asm_t print_asm = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_rx_asm_sopts,
                        set_rx_asm_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {

		case 'e':
			print_asm.status = (0 == atoi(optarg)) ? disable : enable;
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 4) || err_help) {
		set_rx_asm_cmd_help();
		return -1;
	}

	printf("rx_asm: status=%s\n",
		(enable == print_asm.status) ? "enabled" : "disabled");
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_RX_ASM_GET, &print_asm);
}

/* cmd: set_tx_asm */
static const char set_tx_asm_sopts[] = "e:h";
static struct option set_tx_asm_lopts[] = {
	{ "enable", required_argument, NULL, 'e' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};
static void set_tx_asm_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_tx_asm\n");
	printf("  -e/--enable  - Enable or disable\n");
	printf("  -h/--help - Command help\n");
}

static int set_tx_asm_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	st_print_asm_t print_asm = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_tx_asm_sopts,
                        set_tx_asm_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {

		case 'e':
			print_asm.status = (0 == atoi(optarg)) ? disable : enable;
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 4) || err_help) {
		set_tx_asm_cmd_help();
		return -1;
	}

	printf("tx_asm: status=%s\n",
		(enable == print_asm.status) ? "enabled" : "disabled");
	return atm_bond_ioctl_cmd(IMA_IMAP_DEBUG_TX_ASM_GET, &print_asm);
}

/* cmd: set_qos_link_rate */
static const char set_qos_link_rate_sopts[] = "0:1:h";
static struct option set_qos_link_rate_lopts[] = {
	{ "link0-rate", required_argument, NULL, '0' },
	{ "link1-rate", required_argument, NULL, '1' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

static void set_qos_link_rate_cmd_help(void)
{
	printf("Usage: atm_bond_cli set_qos_link_rate\n");
	printf("  -0/--link0-rate - Link0 rate\n");
	printf("  -1/--link1-rate - Link1 rate\n");
	printf("  -h/--help       - Command help\n");
}

static int set_qos_link_rate_cmd(int argc, char *argv[])
{
	int opt;
	int opt_idx;
	int err_help = 0;
	struct st_link_rate link_rate = {0};

	while (1) {
		opt = getopt_long(argc, argv, set_qos_link_rate_sopts,
                        set_qos_link_rate_lopts, &opt_idx);
		if (opt == -1)
			break;

		switch(opt) {
		case '0':
			if(atoi(optarg) <= 0) {
				printf("Line rate should be +ve\n");
				err_help = 1;
			}

			link_rate.link0Rate = atoi(optarg);
			break;
		case '1':
			if(atoi(optarg) <= 0) {
				printf("Line rate should be +ve\n");
				err_help = 1;
			}

			link_rate.link1Rate = atoi(optarg);
			break;
		case 'h':
			err_help = 1;
			break;
		default:
			printf("unknown option: opt=%d optarg=%s\n", opt, optarg);
			err_help = 1;
		}
	}

	if ((argc < 6) || err_help) {
		set_qos_link_rate_cmd_help();
		return -1;
	}

	printf("qos_link_rate: link0Rate=%d link1Rate=%d\n",
		link_rate.link0Rate, link_rate.link1Rate);
	return atm_bond_ioctl_cmd(IMA_IMAP_QOS_SET_LINK_RATE, &link_rate);
}

static const struct cmd_options atm_bond_cmds[] =
{
	{ "set_tx_sid_offset", set_tx_sid_offset_cmd },
	{ "set_tx_sid",        set_tx_sid_cmd },
	{ "set_tx_qdc",        set_tx_qdc_cmd },
	{ "set_tx_abort",      set_tx_abort_cmd },
	{ "set_tx_flush",      set_tx_flush_cmd },
	{ "set_tx_delay",      set_tx_delay_cmd },
	{ "set_rx_sid_offset", set_rx_sid_offset_cmd },
	{ "set_rx_sid",        set_rx_sid_cmd },
	{ "set_rx_sid_skip",   set_rx_sid_skip_cmd },
	{ "set_rx_timeout",    set_rx_timeout_cmd },
	{ "set_rx_full",       set_rx_full_cmd },
	{ "get_stats",         get_stats_cmd },
	{ "set_rx_asm",        set_rx_asm_cmd },
	{ "set_tx_asm",        set_tx_asm_cmd },
	{ "set_qos_link_rate", set_qos_link_rate_cmd },
	{ NULL, NULL }
};

int main(int argc, char *argv[])
{
	int ret;
	int idx = 0;
	int cmd_found = 0;

	if ((argc == 1) ||
	    !strcmp(argv[1], "-h") ||
	    !strcmp(argv[1], "--help")) {
		atm_bond_cli_help();
		return -1;
	}

	for (idx = 0; atm_bond_cmds[idx].name &&
		      atm_bond_cmds[idx].handler; idx++) {
		if (!strcmp(atm_bond_cmds[idx].name, argv[1])) {
			ret = atm_bond_cmds[idx].handler(argc, argv);
			cmd_found = 1;
			break;
		}
	}

	if (!cmd_found) {
		atm_bond_cli_help();
		ret = -1;
	}

	return ret;
}

