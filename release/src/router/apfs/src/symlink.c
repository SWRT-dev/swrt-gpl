// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2018 Ernesto A. Fernández <ernesto.mnd.fernandez@gmail.com>
 */

#include <linux/fs.h>
#include <linux/slab.h>
#include "apfs.h"

/**
 * apfs_get_link - Follow a symbolic link
 * @dentry:	dentry for the link
 * @inode:	inode for the link
 * @done:	delayed call to free the returned buffer after use
 *
 * Returns a pointer to a buffer containing the target path, or an appropriate
 * error pointer in case of failure.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
static const char *apfs_follow_link(struct dentry *dentry, void **cookie)
#else
static const char *apfs_get_link(struct dentry *dentry, struct inode *inode,
				 struct delayed_call *done)
#endif
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
	struct super_block *sb = d_inode(dentry)->i_sb;
#else
	struct super_block *sb = inode->i_sb;
#endif
	struct apfs_nxsb_info *nxi = APFS_NXI(sb);
	char *target = NULL;
	int err;
	int size;

	down_read(&nxi->nx_big_sem);

	if (!dentry) {
		err = -ECHILD;
		goto fail;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
	size = __apfs_xattr_get(d_inode(dentry), APFS_XATTR_NAME_SYMLINK,
#else
	size = __apfs_xattr_get(inode, APFS_XATTR_NAME_SYMLINK,
#endif
				NULL /* buffer */, 0 /* size */);
	if (size < 0) { /* TODO: return a better error code */
		apfs_err(sb, "symlink size read failed");
		err = size;
		goto fail;
	}

	target = kmalloc(size, GFP_KERNEL);
	if (!target) {
		err = -ENOMEM;
		goto fail;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
	size = __apfs_xattr_get(d_inode(dentry), APFS_XATTR_NAME_SYMLINK, target, size);
#else
	size = __apfs_xattr_get(inode, APFS_XATTR_NAME_SYMLINK, target, size);
#endif
	if (size < 0) {
		apfs_err(sb, "symlink read failed");
		err = size;
		goto fail;
	}
	if (size == 0 || *(target + size - 1) != 0) {
		/* Target path must be NULL-terminated */
		apfs_err(sb, "bad link target in inode 0x%llx",
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
			   apfs_ino(d_inode(dentry))
#else
			   apfs_ino(inode)
#endif
		);
		err = -EFSCORRUPTED;
		goto fail;
	}

	up_read(&nxi->nx_big_sem);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
	return *cookie = target;
#else
	set_delayed_call(done, kfree_link, target);
	return target;
#endif

fail:
	kfree(target);
	up_read(&nxi->nx_big_sem);
	return ERR_PTR(err);
}

const struct inode_operations apfs_symlink_inode_operations = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
	.follow_link = apfs_follow_link,
#else
	.get_link	= apfs_get_link,
#endif
	.getattr	= apfs_getattr,
	.listxattr	= apfs_listxattr,
	.update_time	= apfs_update_time,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0) /* Now this is the default */
	.readlink	= generic_readlink,
#endif
};
