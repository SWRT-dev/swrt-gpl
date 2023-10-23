/*
 * pp_debugfs_common.h
 * Description: PP debugfs common definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __PP_DEBUGFS_COMMON_H_
#define __PP_DEBUGFS_COMMON_H_

#include <linux/kernel.h>       /* for scnprintf */
#include <linux/debugfs.h>      /* struct dentry */
#include <linux/bitops.h>       /* BIT()         */
#include <linux/bitfield.h>     /* FIELD_GET/MOD */
#include <linux/ctype.h>        /* tolower       */
#include <linux/string.h>       /* strscpy       */
#include <linux/errno.h>
#include <linux/capability.h>
#include "pp_common.h"

#if IS_ENABLED(CONFIG_DEBUG_FS)
struct debugfs_file {
	const char                   *name;
	const struct file_operations *fops;
};

/**
 * @define Maximum buffer for read/write functions
 */
#define PP_DBGFS_WR_STR_MAX          (1024)

/**
 * @define Auto generated 'fops' structure name
 */
#define PP_DEBUGFS_FOPS(name) (dbg_##name##_fops)

/**
 * @brief Used internally in order to force read_fn to fail
 */
static inline u32 dummy_read_fn(char *cmd_buf)
{
	return 0;
}

/**
 * @brief Define Debugfs attribute functions and declare the
 *        'fops' structure
 * @param name base name for internal usage
 * @param get signature:
 *        s32 get(void *data, u64 *val)
 *        '@param data' user data
 *        '@param val' buffer to return the value
 *        '@return' 0 on success, error code otherwise
 * @param write_fn signature:
 *        s32 set(void *data, u64 *val)
 *        '@param data' user data
 *        '@param val' the value to set
 *        '@return' 0 on success, error code otherwise
 */
#define PP_DEFINE_DBGFS_ATT_FMT(name, get, set, fmt)                           \
	static s32 __dbg_##name##_get(void *data, u64 *val)                    \
	{                                                                      \
		s32 (*__get)(void *data, u64 *val);                            \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
									       \
		__get = get;                                                   \
		return __get ? __get(data, val) : -EPERM;                      \
	}                                                                      \
	static s32 __dbg_##name##_set(void *data, u64 val)                     \
	{                                                                      \
		s32 (*__set)(void *data, u64 val);                             \
									       \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
									       \
		__set = set;                                                   \
		return __set ? __set(data, val) : -EPERM;                      \
	}                                                                      \
	DEFINE_DEBUGFS_ATTRIBUTE(dbg_##name##_fops, __dbg_##name##_get,        \
				 __dbg_##name##_set, fmt)

#define PP_DEFINE_DBGFS_ATT(name, get, set)                                    \
	PP_DEFINE_DBGFS_ATT_FMT(name, get, set, "%llu\n")

/**
 * @brief Define Debugfs seq-read/write functions and declare
 *        the 'fops' structure
 * @param name base name for internal usage
 * @param read_fn signature:
 *        void READ_FN(struct seq_file *f)
 *        '@param f' seq_file ptr
 * @param write_fn signature:
 *        void WRITE_FN(char *cmd_buf, void *data)
 *        '@param cmd_buf' buffer contains the command from user
 *        '@param data' private data
 */
#define PP_DEFINE_DEBUGFS(name, read_fn, write_fn)                             \
	static int __dbg_##name##_rd(struct seq_file *f, void *offset)         \
	{                                                                      \
		void (*__read_fn)(struct seq_file *f);                         \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
									       \
		__read_fn = read_fn;                                           \
		if (!__read_fn)                                                \
			return -EPERM;                                         \
									       \
		if (unlikely(!f))                                              \
			return 0;                                              \
									       \
		__read_fn(f);                                                  \
									       \
		return 0;                                                      \
	}                                                                      \
	static int __dbg_##name##_open(struct inode *inode, struct file *file) \
	{                                                                      \
		return single_open(file, __dbg_##name##_rd, inode->i_private); \
	}                                                                      \
	static ssize_t __dbg_##name##_wr(struct file *file,                    \
					 const char __user *buf, size_t count, \
					 loff_t *pos)                          \
	{                                                                      \
		s32 ret;                                                       \
		char *cmd_buf;                                                 \
		void (*__write_fn)(char *buff, void *data);                    \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
									       \
		if ((PP_DBGFS_WR_STR_MAX - 1) < count)                         \
			return -E2BIG;                                         \
									       \
		__write_fn = write_fn;                                         \
		if (!__write_fn)                                               \
			return -EPERM;                                         \
									       \
		cmd_buf = kzalloc(PP_DBGFS_WR_STR_MAX, GFP_ATOMIC);            \
		if (!cmd_buf)                                                  \
			return count;                                          \
		*pos = 0;                                                      \
		ret = simple_write_to_buffer(cmd_buf, count, pos, buf, count); \
		if (unlikely(ret != count)) {                                  \
			kfree(cmd_buf);                                        \
			return ret >= 0 ? -EIO : ret;                          \
		}                                                              \
		cmd_buf[count - 1] = '\0';                                     \
		__write_fn(cmd_buf, file->f_inode->i_private);                 \
		kfree(cmd_buf);                                                \
		return count;                                                  \
	}                                                                      \
	static const struct file_operations dbg_##name##_fops = {              \
		.open = __dbg_##name##_open,                                   \
		.read = seq_read,                                              \
		.write = __dbg_##name##_wr,                                    \
		.llseek = seq_lseek,                                           \
		.release = single_release,                                     \
	}

static inline s32 pp_debugfs_create(struct dentry *parent,
				    const char *sub_dir_name,
				    struct dentry **sub_dir,
				    const struct debugfs_file *files, u32 n,
				    void *data)
{
	struct dentry *dent, *__sub_dir;
	unsigned char tmp[] = "debugfs";
	char *pname = parent ? parent->d_iname : tmp;
	const struct debugfs_file *file;

	__sub_dir = parent;
	if (!sub_dir_name)
		goto create_files;

	__sub_dir = debugfs_create_dir(sub_dir_name, parent);
	if (unlikely(!__sub_dir)) {
		pr_err("Failed to create %s/%s debugfs dir\n", pname,
		       sub_dir_name);
		return -ENOMEM;
	}

	pr_debug("folder '%s/%s' created\n", pname, sub_dir_name);
	if (sub_dir)
		*sub_dir = __sub_dir;

create_files:
	for_each_arr_entry(file, files, n) {
		dent = debugfs_create_file(file->name, 0600, __sub_dir, data,
					   file->fops);
		if (unlikely(IS_ERR_OR_NULL(dent))) {
			pr_err("Failed to create '%s' file\n", file->name);
			return (s32)PTR_ERR(dent);
		}
		pr_debug("file '%s/%s/%s' created\n", pname, sub_dir_name,
			 file->name);
	}

	return 0;
}

#define DBG_MAX_PROPS	(32)

/**
 * @struct pp_dbg_prop
 * @brief pp dbg prop configuration
 */
struct pp_dbg_prop {
	char          field[32];
	char          desc[128];
	u8            data_type; /* 0 - normal, 1 - pointer */
	void          *dst;
	u8            dst_sz;
	unsigned long **pdst;
};

/**
 * @struct pp_dbg_props_cbs
 * @brief pp dbg prop cbs
 */
struct pp_dbg_props_cbs {
	s32 (*first_prop_cb)(char *field, u32 val, void *user_data);
	s32 (*done_props_cb)(u32 val, void *user_data);
};

/**
 * @brief Add field to the debugfs
 * @param props    properties array
 * @param pos      Position to add the new prop
 * @param size     size of the props array
 * @param name     Property name
 * @param desc     description (Will be printed when user is doing 'cat file')
 * @param dst      Pointer to save the new value of this parameter
 * @param dst_sz   Size of the field
 */
void pp_dbg_add_prop(struct pp_dbg_prop *props, u16 *pos, u16 size,
		     const char *name, const char *desc, void *dst, u8 dst_sz);

/**
 * @brief Add ptr field to the debugfs
 * @param props    properties array
 * @param pos      Position to add the new prop
 * @param size     size of the props array
 * @param name     Property name
 * @param desc     description (Will be printed when user is doing 'cat file')
 * @param dst      Pointer to save the new value of this parameter
 */
void pp_dbg_add_prop_ptr(struct pp_dbg_prop *props, u16 *pos, u16 size,
			 const char *name, const char *desc,
			 unsigned long **dst);

/**
 * @brief Sets all props to the debugfs. Should be called from set debugfs
 * @param cmd_buf      Command buffer from user
 * @param cbs          Callbacks structure
 * @param props        Properties array
 * @param num_props    Size of the props array
 * @param user_data    User data sent in the CBs (Typically the conf struct)
 */
void pp_dbg_props_set(char *cmd_buf, struct pp_dbg_props_cbs *cbs,
		      struct pp_dbg_prop props[], u16 num_props,
		      void *user_data);

/**
 * @brief Prints props help for the debugfs. Should be called from help debugfs
 * @param buff         Command buffer from user
 * @param name         Debugfs name
 * @param format       Format for this debugfs
 * @param props        Properties array
 * @param num_props    Size of the props array
 */
void pp_dbg_props_help(struct seq_file *f, const char *name,
		       const char *format, struct pp_dbg_prop props[],
		       u16 num_props);

#endif /* CONFIG_DEBUG_FS */

#endif /* __PP_DEBUGFS_COMMON_H_ */
