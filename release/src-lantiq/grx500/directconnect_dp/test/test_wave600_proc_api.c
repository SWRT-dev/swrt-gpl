#include <linux/fs.h>
#include<linux/slab.h>
#include <linux/kernel.h>
#include "test_wlan_proc_api.h"

static inline int lower_ch(int ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch + 'a' - 'A';
	return ch;
}

int ltq_strcmpi(char const *s1, char const *s2)
{
	int c1, c2;

	if (!s1 || !s2)
		return 1;
	while (*s1 && *s2) {	/*same length */
		c1 = lower_ch(*s1);
		c2 = lower_ch(*s2);
		s1++;
		s2++;

		if (c1 != c2)
			return c1 - c2;
	}
	return *s1 - *s2;
}
#if 0
EXPORT_SYMBOL(ltq_strcmpi);
#endif

int ltq_strncmpi(const char *s1, const char *s2, size_t n)
{
	int c1, c2;

	if (!s1 || !s2)
		return 1;
	for (; n > 0; s1++, s2++, --n) {
		c1 = lower_ch(*s1);
		c2 = lower_ch(*s2);
		if (c1 != c2)
			return c1 - c2;
		else if (c1 == '\0')
			return 0;
	}
	return 0;
}
#if 0
EXPORT_SYMBOL(ltq_strncmpi);
#endif

#if 0
char *ltq_strstri(char *string, char *substring)
{
	register char *a, *b;

/* First scan quickly through the two strings looking for a
* single-character match.  When it's found, then compare the
* rest of the substring.
*/
	if (!string || !substring)
		return (char *)0;
	b = substring;
	if (*b == 0)
		return string;

	for (; *string != 0; string += 1) {
		if (lower_ch(*string) != lower_ch(*b))
			continue;
		a = string;
		while (1) {
			if (*b == 0)
				return string;
			if (lower_ch(*a++) != lower_ch(*b++))
				break;
		}
		b = substring;
	}
	return (char *)0;
}
EXPORT_SYMBOL(ltq_strstri);
#endif

void ltq_replace_ch(char *p, int len, char orig_ch, char new_ch)
{
	int i;

	if (p)
		for (i = 0; i < len; i++) {
			if (p[i] == orig_ch)
				p[i] = new_ch;
		}
}
#if 0
EXPORT_SYMBOL(ltq_replace_ch);
#endif

static unsigned int btoi(char *str)
{
	unsigned int sum = 0;
	signed len = 0, i = 0;

	len = strlen(str);
	len = len - 1;
	while (len >= 0) {
		if (*(str + len) == '1')
			sum = (sum | (1 << i));
		i++;
		len--;
	}
	return sum;
}

int ltq_atoi(unsigned char *str)
{
	unsigned int n = 0;
	int i = 0;
	int nega_sign = 0;

	if (!str)
		return 0;
	ltq_replace_ch(str, strlen(str), ' ', 0);
	if (str[0] == 0)
		return 0;

	if (str[0] == 'b' || str[0] == 'B') {	/*binary format */
		n = btoi(str + 1);
	} else if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X'))) {
		/*hex format */
		str += 2;

		while (str[i]) {
			n = n * 16;
			if (('0' <= str[i] && str[i] <= '9')) {
				n += str[i] - '0';
			} else if (('A' <= str[i] && str[i] <= 'F')) {
				n += str[i] - 'A' + 10;
				;
			} else if (('a' <= str[i] && str[i] <= 'f')) {
				n += str[i] - 'a' + 10;
				;
			} else
				PRINTK(KERN_ERR "Wrong value:%u\n", str[i]);

			i++;
		}

	} else {
		if (str[i] == '-') {	/*negative sign */
			nega_sign = 1;
			i++;
		}
		while (str[i]) {
			n *= 10;
			n += str[i] - '0';
			i++;
		}
	}
	if (nega_sign)
		n = -(int)n;
	return n;
}
#if 0
EXPORT_SYMBOL(ltq_atoi);
#endif

static void ltq_remove_leading_whitespace(char **p, int *len)
{
	while (*len && ((**p == ' ') || (**p == '\r') || (**p == '\r'))) {
		(*p)++;
		(*len)--;
	}
}

/*Split buffer to multiple segment with seperator space.
And put pointer to array[].
By the way, original buffer will be overwritten with '\0' at some place.
*/
int ltq_split_buffer(char *buffer, char *array[], int max_param_num)
{
	int i, set_copy = 0;
	int res = 0;
	int len;

	for (i = 0; i < max_param_num; i++)
		array[i] = NULL;
	if (!buffer)
		return 0;
	len = strlen(buffer);
	for (i = 0; i < max_param_num;) {
		ltq_remove_leading_whitespace(&buffer, &len);
		for (;
		     *buffer != ' ' && *buffer != '\0' && *buffer != '\r'
		     && *buffer != '\n' && *buffer != '\t'; buffer++, len--) {
			/*Find first valid charactor */
			set_copy = 1;
			if (!array[i])
				array[i] = buffer;
		}

		if (set_copy == 1) {
			i++;
			if (*buffer == '\0' || *buffer == '\r'
			    || *buffer == '\n') {
				*buffer = 0;
				break;
			}
			*buffer = 0;
			buffer++;
			len--;
			set_copy = 0;

		} else {
			if (*buffer == '\0' || *buffer == '\r'
			    || *buffer == '\n')
				break;
			buffer++;
			len--;
		}
	}
	res = i;

	return res;
}
#if 0
EXPORT_SYMBOL(ltq_split_buffer);
#endif

static void *ltq_seq_start(struct seq_file *s, loff_t *pos)
{
	struct ltq_proc_file_entry *p = s->private;

	if (p->pos < 0)
		return NULL;

	return p;
}

static void *ltq_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	struct ltq_proc_file_entry *p = s->private;

	*pos = p->pos;

	if (p->pos >= 0)
		return p;
	else
		return NULL;
}

static void ltq_seq_stop(struct seq_file *s, void *v)
{
}

static int ltq_seq_show(struct seq_file *s, void *v)
{
	struct ltq_proc_file_entry *p = s->private;

	if (p->pos >= 0)
		p->pos = p->callback(s, p->pos);

	return 0;
}

static int ltq_proc_open(struct inode *inode, struct file *file);

static const struct seq_operations dp_seq_ops = {
	.start = ltq_seq_start,
	.next = ltq_seq_next,
	.stop = ltq_seq_stop,
	.show = ltq_seq_show
};

static int ltq_proc_open(struct inode *inode, struct file *file)
{
	struct seq_file *s;
	struct ltq_proc_file_entry *p;
	struct ltq_proc_entry *entry;
	int ret;

	ret = seq_open(file, &dp_seq_ops);
	if (ret)
		return ret;

	s = file->private_data;
	p = (struct ltq_proc_file_entry *)kmalloc(sizeof(*p), GFP_KERNEL);

	if (!p) {
		(void)seq_release(inode, file);
		return -ENOMEM;
	}

	entry = PDE_DATA(inode);

	p->callback = entry->callback;
	if (entry->init_callback)
		p->pos = entry->init_callback();
	else
		p->pos = 0;

	s->private = p;

	return 0;
}

static int ltq_proc_release(struct inode *inode, struct file *file)
{
	struct seq_file *s;

	s = file->private_data;
	kfree(s->private);

	return seq_release(inode, file);
}

static int ltq_seq_single_show(struct seq_file *s, void *v)
{
	struct ltq_proc_entry *p = s->private;

	p->single_callback(s);
	return 0;
}

static int ltq_proc_single_open(struct inode *inode, struct file *file)
{
	return single_open(file, ltq_seq_single_show, PDE_DATA(inode));
}

void ltq_proc_entry_create(struct proc_dir_entry *parent_node,
			   struct ltq_proc_entry *proc_entry)
{
	memset(&proc_entry->ops, 0, sizeof(struct file_operations));
	proc_entry->ops.owner = THIS_MODULE;

	if (proc_entry->single_callback) {
		proc_entry->ops.open = ltq_proc_single_open;
		proc_entry->ops.release = single_release;
	} else {
		proc_entry->ops.open = ltq_proc_open;
		proc_entry->ops.release = ltq_proc_release;
	}

	proc_entry->ops.read = seq_read;
	proc_entry->ops.llseek = seq_lseek;
	proc_entry->ops.write = proc_entry->write_callback;
	proc_create_data(proc_entry->name,
			 (S_IFREG | S_IRUGO),
			 parent_node, &proc_entry->ops, proc_entry);
}
#if 0 /* FIXME */
EXPORT_SYMBOL(ltq_proc_entry_create);
#endif
