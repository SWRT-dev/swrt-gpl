/*
 * pp_debugfs_common.c
 * Description: PP debugfs common infrastructure
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/bitops.h>
#include <linux/bitfield.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/capability.h>
#include "pp_debugfs_common.h"
#include "pp_common.h"

#define INVALID_ID (UINT_MAX)
#define MAX_FLD_VAL(sizeof_fld)  (((u64)1 << (BITS_PER_BYTE * sizeof_fld)) - 1)

void pp_dbg_props_set(char *cmd_buf, struct pp_dbg_props_cbs *cbs,
		      struct pp_dbg_prop props[], u16 num_props,
		      void *user_data)
{
	u8 *field;
	char *tok, *ptr, *pval;
	unsigned long res;
	u16 i, num_changed = 0;
	u32 id = INVALID_ID;
	u32 len, first_prop = 1;
	s32 ret;
	unsigned long val;

	field = kzalloc(PP_DBGFS_WR_STR_MAX, GFP_KERNEL);
	if (!field)
		return;

	ptr = (char *)cmd_buf;

	while ((tok = strsep(&ptr, " \t\n\r")) != NULL) {
		if (tok[0] == '\0')
			continue;

		strcpy(field, tok);
		pval = strchr(field, '=');
		if (!pval) {
			pr_err("Wrong format for prop %s\n", tok);
			goto out;
		}

		*pval = '\0';
		pval++;

		ret = kstrtoul(pval, 0, &res);
		if (ret) {
			pr_err("kstrtoul failure (%d)\n", ret);
			goto out;
		}

		if (first_prop) {
			first_prop = 0;
			id = res;
			if (cbs && cbs->first_prop_cb &&
			    cbs->first_prop_cb(field, res, user_data)) {
				pr_err("first_prop_cb failed\n");
				goto out;
			}
		}

		for (i = 0; i < num_props ; i++) {
			len = max(strlen(props[i].field), strlen(field));
			if (!strncmp(field, props[i].field, len)) {
				if (props[i].data_type == 0) {
					val = MAX_FLD_VAL(props[i].dst_sz);
					if (res > val) {
						pr_info("value is too big\n");
						goto out;
					} else {
						memcpy(props[i].dst,
						       (void *)&res,
						       props[i].dst_sz);
					}
				} else {
					*(props[i].pdst) = (void *)res;
				}
				num_changed++;
				break;
			}
		}

		if (i == num_props)
			pr_err("Not supported field %s", field);
	}

	if (id != INVALID_ID) {
		/* If only id was set, print current configuration */
		if (num_changed == 1) {
			pr_info("Current configuration:\n");

			for (i = 0; i < num_props ; i++) {
				if (props[i].data_type == 0) {
					val = 0;
					memcpy((void *)&val, props[i].dst,
					       props[i].dst_sz);
					pr_info("%-30s%lu\n",
						props[i].field, val);
				} else {
					pr_info("%-30s%#lx\n",
						props[i].field,
						(ulong)(*props[i].pdst));
				}
			}

			goto out;
		}

		if (cbs && cbs->done_props_cb) {
			if (cbs->done_props_cb(id, user_data)) {
				pr_err("done_props_cb failed\n");
				goto out;
			}
		}
	}

out:
	kfree(field);
}

void pp_dbg_props_help(struct seq_file *f, const char *name,
		       const char *format, struct pp_dbg_prop props[],
		       u16 num_props)
{
	u32 idx;

	seq_printf(f, "<---- %s---->\n", name);
	seq_printf(f, "[FORMAT] %s\n", format);
	seq_puts(f, "[FORMAT] If only id is set, operation is get conf\n");
	seq_puts(f, "Supported fields\n");

	for (idx = 0; idx < num_props ; idx++) {
		seq_printf(f, "%-30s%s\n", props[idx].field, props[idx].desc);
	}
}

void pp_dbg_add_prop(struct pp_dbg_prop *props, u16 *pos, u16 size,
		     const char *name, const char *desc, void *dst, u8 dst_sz)
{
	if (*pos >= size) {
		pr_err("pos %d >= size %d", *pos, size);
		return;
	}

	if (strscpy(props[*pos].field, name, sizeof(props[*pos].field)) <= 0) {
		pr_err("%s copying field %s failed\n", __func__, name);
		return;
	}

	if (strscpy(props[*pos].desc, desc, sizeof(props[*pos].desc)) <= 0) {
		pr_err("%s copying desc %s failed\n", __func__, desc);
		return;
	}

	props[*pos].data_type = 0;
	props[*pos].dst = dst;
	props[*pos].dst_sz = dst_sz;

	(*pos)++;
}

void pp_dbg_add_prop_ptr(struct pp_dbg_prop *props, u16 *pos, u16 size,
			 const char *name, const char *desc,
			 unsigned long **dst)
{
	if (*pos >= size) {
		pr_err("pos %d >= size %d", *pos, size);
		return;
	}

	if (strscpy(props[*pos].field, name, sizeof(props[*pos].field)) <= 0) {
		pr_err("%s copying field %s failed\n", __func__, name);
		return;
	}

	if (strscpy(props[*pos].desc, desc, sizeof(props[*pos].desc)) <= 0) {
		pr_err("%s copying desc %s failed\n", __func__, desc);
		return;
	}

	props[*pos].data_type = 1;
	props[*pos].pdst = dst;
	props[*pos].dst_sz = sizeof(dst);

	(*pos)++;
}
