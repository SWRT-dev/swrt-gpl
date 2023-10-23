#ifndef TEST_WLAN_PROC_API_H
#define TEST_WLAN_PROC_API_H

#include <linux/kernel.h>	/*kmalloc */
#include <linux/ctype.h>
#include <linux/proc_fs.h>	/*file_operations */
#include <linux/seq_file.h>	/*seq_file */
#include <linux/uaccess.h>	/*copy_from_user */

#define PRINTK  printk         /*workaround for checkpatch issue */
#define SEQ_PRINTF  seq_printf

#define set_ltq_dbg_flag(v, e, f) do {;\
	if (e > 0)\
		v |= (uint32_t)(f);\
	else\
		v &= (uint32_t) (~f); } \
	while (0)

typedef void (*ltq_proc_single_callback_t) (struct seq_file *);
typedef int (*ltq_proc_callback_t) (struct seq_file *, int);
typedef int (*ltq_proc_init_callback_t) (void);
typedef ssize_t(*ltq_proc_write_callback_t) (struct file *file,
					     const char __user *input,
					     size_t size, loff_t *loff);

struct ltq_proc_file_entry {
	ltq_proc_callback_t callback;
	int pos;
};

struct ltq_proc_entry {
	char *name;
	ltq_proc_single_callback_t single_callback;
	ltq_proc_callback_t callback;
	ltq_proc_init_callback_t init_callback;
	ltq_proc_write_callback_t write_callback;
	struct file_operations ops;
};

void ltq_proc_entry_create(struct proc_dir_entry *parent_node,
			   struct ltq_proc_entry *proc_entry);

int ltq_atoi(unsigned char *str);
void ltq_remove_space(char **p, int *len);
char *ltq_strstri(char *string, char *substring);
int ltq_strncmpi(const char *s1, const char *s2, size_t n);
int ltq_strcmpi(char const *s1, char const *s2);
void ltq_replace_ch(char *p, int len, char orig_ch, char new_ch);

/*Split buffer to multiple segment with seperator space.
And put pointer to array[].
By the way, original buffer will be overwritten with '\0' at some place.
*/
int ltq_split_buffer(char *buffer, char *array[], int max_param_num);

#endif				/*TEST_WLAN_PROC_API_H*/
