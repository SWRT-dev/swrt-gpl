#ifndef __CQM_PROC_H_
#define __CQM_PROC_H_

#include <asm/div64.h>
#include <linux/ctype.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/sort.h>
#include <linux/version.h>

#define get_val(val, mask, offset) (((val) & (mask)) >> (offset))
void print_reg64(char *name, void *addr);
uint64_t REG64(void *addr);
#endif

