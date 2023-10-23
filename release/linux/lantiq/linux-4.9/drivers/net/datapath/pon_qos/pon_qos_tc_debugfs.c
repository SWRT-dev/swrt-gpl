#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <uapi/linux/stat.h> /* S_IRUSR */

#include "pon_qos_tc_ext_vlan.h"

struct pon_qos_debugfs_entry {
	const char *filename;
	void (*read)(struct seq_file *file, void *ctx);
};

static int pon_qos_read(struct seq_file *file, void *unused)
{
	const struct pon_qos_debugfs_entry *entry = file->private;

	entry->read(file, NULL);
	return 0;
}

static int pon_qos_open(struct inode *inode, struct file *file)
{
	return single_open(file, pon_qos_read, inode->i_private);
}

static const struct file_operations pon_qos_fops = {
	.llseek = seq_lseek,
	.open = pon_qos_open,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static struct pon_qos_debugfs_entry entries[] = {
	{ .filename = "ext_vlan", .read = pon_qos_tc_ext_vlan_debugfs },
};

#define DEBUGFS_DIR "pon_qos"
static struct dentry *dir;
static struct dentry *debugfs_files[ARRAY_SIZE(entries)];

int pon_qos_tc_debugfs_init(void)
{
	unsigned int i = 0;

	dir = debugfs_create_dir(DEBUGFS_DIR, 0);
	if (!dir)
		return -ENODEV;

	for (i = 0; i < ARRAY_SIZE(entries); ++i) {
		struct pon_qos_debugfs_entry *entry = &entries[i];
		struct dentry **file = &debugfs_files[i];

		*file = debugfs_create_file(entry->filename, S_IRUSR, dir,
					    entry, &pon_qos_fops);
		if (!*file)
			goto cleanup;
	}

	return 0;

cleanup:
	debugfs_remove_recursive(dir);
	return -ENOENT;
}

void pon_qos_tc_debugfs_exit(void)
{
	debugfs_remove_recursive(dir);
}
