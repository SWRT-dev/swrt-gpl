/*
 * Non-physical true random number generator based on timing jitter - DebugFS
 *
 * Copyright Stephan Mueller <smueller@chronox.de>, 2013
 *
 * License
 * =======
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/compat.h>
#include <linux/export.h>
#include <linux/file.h>
#include <linux/debugfs.h>

#include "jitterentropy.h"
#include "jitterentropy-dbg.h"

struct jent_debugfs {
	struct dentry *jent_debugfs_root;
	struct dentry *jent_debugfs_entropy;
	struct dentry *jent_debugfs_noise;
};

struct jent_raw
{
	spinlock_t raw_lock;
	struct rand_data *entropy_collector;
};

static struct jent_debugfs jent_debugfs;
static struct jent_raw raw_entropy;

void jent_drng_cleanup_raw(struct jent_raw *raw)
{
	spin_lock_bh(&raw->raw_lock);
	if (NULL != raw->entropy_collector)
		jent_entropy_collector_free(raw->entropy_collector);
	raw->entropy_collector = NULL;
	spin_unlock_bh(&raw->raw_lock);
}

int jent_drng_get_bytes_raw(struct jent_raw *raw, char *data, size_t len)
{
	int ret = 0;

	spin_lock_bh(&raw->raw_lock);
	ret = jent_read_entropy(raw->entropy_collector, data, len);
	if (0 > ret) {
		printk(DRIVER_NAME": Unable to obtain %zu bytes of entropy\n", len);
		ret = -EAGAIN;
	}

	spin_unlock_bh(&raw->raw_lock);
	return ret;
}


static inline int jent_dbg_raw_bytes(char *data, size_t len)
{
        return jent_drng_get_bytes_raw(&raw_entropy, data, len);
}

static ssize_t jent_debugfs_read_func(struct file *file,
				      char __user *buf, size_t nbytes,
				      loff_t *ppos, size_t chunk,
				      int (*source)(char *out, size_t len))
{
	ssize_t total = 0;
	int ret = 0;
	loff_t pos = *ppos;
	char *out;

	if (!nbytes)
		return -EINVAL;

	out = kzalloc(chunk, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	while (nbytes > 0) {
		int copy = min_t(int, chunk, nbytes);
		ret = source(out, copy);
		if (0 > ret) {
			printk(DRIVER_NAME": could not obtain random data: %d\n", ret);
			ret = -EAGAIN;
			break;
		}
		if (copy_to_user(buf+pos+total, out, copy)) {
			ret = -EFAULT;
			break;
		}
		nbytes -= copy;
		total += copy;
	}
	kzfree(out);

	return ((0 > ret) ? ret : total);
}

static ssize_t jent_debugfs_noise_read(struct file *file, char __user *buf,
			      size_t nbytes, loff_t *ppos)
{
	struct rand_data *ec = raw_entropy.entropy_collector;
	int total = 0;
	loff_t pos = *ppos;
	char *out;

	out = kzalloc(8, GFP_KERNEL);

	while (nbytes > 0) {
		int len = min_t(int, 8, nbytes);
		jent_lfsr_time(ec, 0, 0, 0);
		memcpy(out, &ec->data, len);
		if (copy_to_user(buf+pos+total, out, len)) {
			break;
		}
		nbytes -= len;
		total += len;
	}
	kzfree(out);
	return total;

}
static ssize_t jent_debugfs_seed_read(struct file *file, char __user *buf,
			      size_t nbytes, loff_t *ppos)
{
	int ret = jent_debugfs_read_func(file, buf, nbytes, ppos, 8,
				      jent_dbg_raw_bytes);
	return ret;
}

int jent_drng_init_raw(struct jent_raw *raw, unsigned int flags)
{
	int ret = 0;

	raw->entropy_collector = jent_entropy_collector_alloc(1, flags);
	if (!raw->entropy_collector)
		ret = -ENOMEM;

	spin_lock_init(&raw->raw_lock);
	return ret;
}

static struct file_operations jent_seed_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_seed_read,
};

static struct file_operations jent_noise_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_noise_read,
};


int __init jent_dbg_init(void)
{
	int ret = -EINVAL;
	jent_debugfs.jent_debugfs_root = NULL;
	jent_debugfs.jent_debugfs_noise = NULL;
	jent_debugfs.jent_debugfs_entropy = NULL;

	ret = jent_drng_init_raw(&raw_entropy, JENT_DISABLE_STIR);
	if (ret) {
		printk(DRIVER_NAME": Raw entropy collector instantiation failed\n");
		return ret;
	}
	jent_debugfs.jent_debugfs_root =
		debugfs_create_dir(DRIVER_NAME, NULL);
	if (IS_ERR(jent_debugfs.jent_debugfs_root)) {
		printk(DRIVER_NAME": initialization of debugfs directory failed\n");
		goto cleandir;
	}

	jent_debugfs.jent_debugfs_entropy =
		debugfs_create_file("entropy", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_seed_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_entropy)) {
		printk(DRIVER_NAME": initialization of entropy file failed\n");
		goto cleandir;
	}

	jent_debugfs.jent_debugfs_noise =
		debugfs_create_file("noise", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_noise_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_noise)) {
		printk(DRIVER_NAME": initialization of noise file failed\n");
		goto cleandir;
	}
	return 0;

cleandir:
	debugfs_remove_recursive(jent_debugfs.jent_debugfs_root);
	jent_drng_cleanup_raw(&raw_entropy);

	return ret;
}

void jent_dbg_exit(void)
{
	debugfs_remove_recursive(jent_debugfs.jent_debugfs_root);
	jent_drng_cleanup_raw(&raw_entropy);
}
