/* SPDX-License-Identifier: GPL-2.0-only */
int fit_partition(struct parsed_partitions *);
int parse_fit_partitions(struct parsed_partitions *state, u64 start_sector, u64 nr_sectors, int *slot, int add_remain);
