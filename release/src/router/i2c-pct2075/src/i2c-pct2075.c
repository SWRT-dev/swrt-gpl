#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PCT2075_SENSOR_I2C  0
#define PCT2075_SENSOR_ADDR 0x48
#define PCT2075_SENSOR_NAME "pct2075"

#define PCT2075_REG_TEMP    0x00
#define PCT2075_REG_CONF    0x01
#define PCT2075_REG_THYST   0x02
#define PCT2075_REG_TOS     0x03
#define PCT2075_REG_TIDLE   0x04

static struct i2c_client *g_client = NULL;

int pct2075_read(u8 reg, u8 *val, u8 len)
{
	struct i2c_msg msgs[2];
	int i;
	int ret = 0;
	u8 *temp_buffer, *buffer;

	if(g_client == NULL)
	{
		printk(KERN_ERR "pct2075 i2c g_client null, exit\n");
		return -1;
	}

	temp_buffer = kzalloc(len, GFP_KERNEL);
	if (!temp_buffer)
		return -4;


	buffer = kzalloc(len, GFP_KERNEL);
	if (!buffer)
		return -4;

	msgs[0].addr = g_client->addr;
	msgs[0].flags = g_client->flags;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = g_client->addr;
	msgs[1].flags = g_client->flags | I2C_M_RD;
	msgs[1].len = len;
	msgs[1].buf = temp_buffer;

	ret = i2c_transfer(g_client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret < 0)
		printk(KERN_ERR "pct2075 i2c read failed\n");
	else
	{
		/* Note: buf[0]:MSB; MSB[1]:LSB */
		if(len == 1)
			memcpy(buffer, temp_buffer, len);
		else
		{
			for(i = 0; i < len; i++)
			{
				buffer[i] = temp_buffer[len - i - 1];
			}
		}
		memcpy(val, buffer, len);
		//for(i = 0; i < len; i++)
			//printk("pct2075_read val[%d]:0x%x, buffer[%d]:0x%x, len:%d\n", i, val[i], i, buffer[i], len);
	}

	kfree(temp_buffer);
	kfree(buffer);
	return ret;
}

int pct2075_write(u8 reg, u8 *val, u8 len)
{
	struct i2c_msg msgs[1];
	u8 *temp_buffer, *buffer;
	int i;
	int ret = 0;

	if(g_client == NULL)
	{
		printk(KERN_ERR "pct2075 i2c g_client null, exit\n");
		return -1;
	}

	temp_buffer = kzalloc(len, GFP_KERNEL);
	if (!temp_buffer)
		return -4;

	buffer = kzalloc(len + 1, GFP_KERNEL);
	if (!buffer)
		return -4;

	buffer[0] = reg;
	memcpy(temp_buffer, val, len);
	/* Note: buf[0]:MSB; MSB[1]:LSB */
	if(len == 1)
		memcpy(buffer + 1, temp_buffer, len);
	else
	{
		for(i = 0; i < len; i++)
		{
			buffer[i + 1] = temp_buffer[len - i - 1];
		}
	}
	for(i = 0; i < len; i++)
		printk("pct2075_write: reg:0x%x, val[%d]:0x%x, buffer[%d]:0x%x, len:%d\n", buffer[0], i, val[i], i, buffer[i+1], len);

	msgs[0].addr = g_client->addr;
	msgs[0].flags = g_client->flags;
	msgs[0].len = len + 1;
	msgs[0].buf = buffer;

	ret = i2c_transfer(g_client->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret < 0)
		printk(KERN_ERR "pct2075 i2c write failed\n");

	kfree(temp_buffer);
	kfree(buffer);
	return ret;
}

int pct2075_config(char *option, u8 reg, u8 *val)
{
	int ret = -1;

	if((strcmp(option, "read") == 0) || (strcmp(option, "READ") == 0))
	{
		/* For read */
		//printk("For read:\n");
		switch(reg)
		{
			case PCT2075_REG_CONF:
			case PCT2075_REG_TIDLE:
				ret = pct2075_read(reg, val, 1);
				break;
			case PCT2075_REG_TEMP:
			case PCT2075_REG_THYST:
			case PCT2075_REG_TOS:
				ret = pct2075_read(reg, val, 2);
				break;
			default:
				printk("Usage: option(read/READ or write/WRITE), reg(range:0x00 - 0x04), value\n");
				break;
		}
	}
	else if((strcmp(option, "write") == 0) || (strcmp(option, "WRITE") == 0))
	{
		/* For Write */
		printk("For write:\n");
		switch(reg)
		{
			case PCT2075_REG_CONF:
			case PCT2075_REG_TIDLE:
				ret = pct2075_write(reg, val, 1);
				break;
			case PCT2075_REG_TEMP:
			case PCT2075_REG_THYST:
			case PCT2075_REG_TOS:
				ret = pct2075_write(reg, val, 2);
				break;
			default:
				printk("Usage: option(read/READ or write/WRITE), reg(range:0x00 - 0x04), value\n");
				break;
		}
	}
	else
	{
		printk("Usage: option(read/READ or write/WRITE), reg(range:0x00 - 0x04), value\n");
	}

	return ret;
}

static int temp_reg_show(struct seq_file *seq, void *v)
{
	u16 write_val, read_val;
	int ret;

	ret = pct2075_config("read", PCT2075_REG_TEMP, &read_val);
	read_val = (read_val >> 5);
	if((read_val & 0x400) == 0)
		seq_printf(seq, "%d.%d\n", (read_val*125)/1000, (read_val*125)%1000);
	else
		seq_printf(seq, "-%d.%d\n", ((0x7FF - read_val + 1)*125)/1000, ((0x7FF - read_val + 1)*125)%1000);

	return 0;
}

static int temp_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, temp_reg_show, PDE_DATA(inode));
}

static int temp_reg_write(struct file *file, const char __user * user_buffer, size_t count, loff_t * ppos)
{
	char line[64];
	char empty= ' ';
	char point= '.';
	char *ep  = &empty;
	char *ep2 = &point;
	char *p1, *p2;
	u16 write_val, read_val;
	u16 temp_val1, temp_val2;
	int flag = -1;
	int ret;

	ret = copy_from_user(line, user_buffer, count);
	if(ret)
		return -EFAULT;

	/* if(!strncmp(line, "write", 5))
	{
		p1 = strchr(line, ' ');
		if(p1 != NULL)
		{
			p1++;
			
			if(*p1 == '+')
				flag = 0;
			else if(*p1 == '-')
				flag = 1;
			else
				goto out;

			p1++;
			p2 = strchr(p1, '.');
			if(p2 != NULL)
			{
				p2++;
				temp_val1 = simple_strtoul(p1, &ep2, 0);
				temp_val2 = simple_strtoul(p2, &ep, 0);
			}
			else
			{
				temp_val1 = simple_strtoul(p1, &ep, 0);
				temp_val2 = 0;
			}

			if(!(((flag == 0) && (temp_val1 <= 127) && (((temp_val1*1000 + temp_val2)%125) == 0)) ||
				((flag == 1) && (temp_val1 <= 128) && (((temp_val1*1000 + temp_val2)%125) == 0))))
			{
				printk("VAL: Temperature: multiples of 0.125, range: From -128 to +127.875\n");
				goto out;
			}

			if(flag == 0)
			{
				printk("temperature value: +%d.%d\n", temp_val1, temp_val2);
				write_val = (temp_val1*1000 + temp_val2)/125;
				printk("Temp hex value: 0x%x\n", write_val);
				write_val = (write_val << 5);
				ret = pct2075_config("write", PCT2075_REG_TEMP, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
			else if(flag == 1)
			{
				printk("temperature value: -%d.%d\n", temp_val1, temp_val2);
				write_val = 0x7FF - (temp_val1*1000 + temp_val2)/125 + 1;
				printk("Temp hex value: 0x%x\n", write_val);
				write_val = (write_val << 5);
				ret = pct2075_config("write", PCT2075_REG_TEMP, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
		}
	}
	else */ if(!strncmp(line, "read", 4))
	{
		ret = pct2075_config("read", PCT2075_REG_TEMP, &read_val);
		printk("reg value: 0x%x\n", read_val);
		read_val = (read_val >> 5);
		printk("Temp hex value: 0x%x\n", read_val);
		if((read_val & 0x400) == 0)
			printk("Temperature: +%d.%d\n", (read_val*125)/1000, (read_val*125)%1000);
		else
			printk("Temperature: -%d.%d\n", ((0x7FF - read_val + 1)*125)/1000, ((0x7FF - read_val + 1)*125)%1000);
	}

out:
	return count;
}

static int conf_reg_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,
            "Usage:\n"
            "PCT2075 Conf Reg read:  echo read > /proc/pct2075/conf\n"
            "PCT2075 Conf Reg write: echo write val(Reg value, range: 0x00-0xFF) > /proc/pct2075/conf\n");

	return 0;
}

static int conf_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, conf_reg_show, PDE_DATA(inode));
}

static int conf_reg_write(struct file *file, const char __user * user_buffer, size_t count, loff_t * ppos)
{
	char line[64];
	char empty= ' ';
	char *ep = &empty;
	char *p1;
	u8 write_val, read_val;
	u16 temp_val;
	int ret;

	ret = copy_from_user(line, user_buffer, count);
	if(ret)
		return -EFAULT;

	if(!strncmp(line, "write", 5))
	{
		p1 = strchr(line, ' ');
		if(p1 != NULL)
		{
			p1++;
			temp_val = simple_strtoul(p1, &ep, 0);
			if((temp_val < 0) || (temp_val > 255))
			{
				printk("VAL: Reg value, range: 0x00-0xFF\n");
				goto out;
			}
			write_val=temp_val;
			ret = pct2075_config("write", PCT2075_REG_CONF, &write_val);
			printk("write value: 0x%x\n", write_val);
		}
	}
	else if(!strncmp(line, "read", 4))
	{
		ret = pct2075_config("read", PCT2075_REG_CONF, &read_val);
		printk("read value: 0x%x\n", read_val);
	}

out:
	return count;
}

static int thyst_reg_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,
            "Usage:\n"
            "PCT2075 Thyst read:  echo read > /proc/pct2075/thyst\n"
            "PCT2075 Thyst write: echo write val(Integer multiples of 0.5, range: from -128 to +127.5) > /proc/pct2075/thyst\n");

	return 0;
}

static int thyst_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, thyst_reg_show, PDE_DATA(inode));
}

static int thyst_reg_write(struct file *file, const char __user * user_buffer, size_t count, loff_t * ppos)
{
	char line[64];
	char empty= ' ';
	char point= '.';
	char *ep  = &empty;
	char *ep2 = &point;
	char *p1, *p2;
	u16 write_val, read_val;
	u16 temp_val1, temp_val2;
	int flag = -1;
	int ret;

	ret = copy_from_user(line, user_buffer, count);
	if(ret)
		return -EFAULT;

	if(!strncmp(line, "write", 5))
	{
		p1 = strchr(line, ' ');
		if(p1 != NULL)
		{
			p1++;
			
			if(*p1 == '+')
				flag = 0;
			else if(*p1 == '-')
				flag = 1;
			else
				goto out;

			p1++;
			p2 = strchr(p1, '.');
			if(p2 != NULL)
			{
				p2++;
				temp_val1 = simple_strtoul(p1, &ep2, 0);
				temp_val2 = simple_strtoul(p2, &ep, 0);
			}
			else
			{
				temp_val1 = simple_strtoul(p1, &ep, 0);
				temp_val2 = 0;
			}

			if(!(((flag == 0) && (temp_val1 <= 127) && (((temp_val1*10 + temp_val2)%5) == 0)) ||
				((flag == 1) && (temp_val1 <= 128) && (((temp_val1*10 + temp_val2)%5) == 0))))
			{
				printk("VAL: Thyst: multiples of 0.5, range: From -128 to +127.5\n");
				goto out;
			}

			if(flag == 0)
			{
				printk("thyst value: +%d.%d\n", temp_val1, temp_val2);
				write_val = (temp_val1*10 + temp_val2)/5;
				printk("Thyst hex value: 0x%x\n", write_val);
				write_val = (write_val << 7);
				ret = pct2075_config("write", PCT2075_REG_THYST, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
			else if(flag == 1)
			{
				printk("thyst value: -%d.%d\n", temp_val1, temp_val2);
				write_val = 0x1FF - (temp_val1*10 + temp_val2)/5 + 1;
				printk("Thyst hex value: 0x%x\n", write_val);
				write_val = (write_val << 7);
				ret = pct2075_config("write", PCT2075_REG_THYST, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
		}
	}
	else if(!strncmp(line, "read", 4))
	{
		ret = pct2075_config("read", PCT2075_REG_THYST, &read_val);
		printk("reg value: 0x%x\n", read_val);
		read_val = (read_val >> 7);
		printk("Thyst hex value: 0x%x\n", read_val);
		if((read_val & 0x100) == 0)
			printk("Thyst: +%d.%d\n", (read_val*5)/10, (read_val*5)%10);
		else
			printk("Thyst: -%d.%d\n", ((0x1FF - read_val + 1)*5)/10, ((0x1FF - read_val + 1)*5)%10);
	}

out:
	return count;
}

static int tos_reg_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,
            "Usage:\n"
            "PCT2075 Tos read:  echo read > /proc/pct2075/tos\n"
            "PCT2075 Tos write: echo write val(Integer multiples of 0.5, range: from -128 to +127.5) > /proc/pct2075/tos\n");

	return 0;
}

static int tos_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, tos_reg_show, PDE_DATA(inode));
}

static int tos_reg_write(struct file *file, const char __user * user_buffer, size_t count, loff_t * ppos)
{
	char line[64];
	char empty= ' ';
	char point= '.';
	char *ep  = &empty;
	char *ep2 = &point;
	char *p1, *p2;
	u16 write_val, read_val;
	u16 temp_val1, temp_val2;
	int flag = -1;
	int ret;

	ret = copy_from_user(line, user_buffer, count);
	if(ret)
		return -EFAULT;

	if(!strncmp(line, "write", 5))
	{
		p1 = strchr(line, ' ');
		if(p1 != NULL)
		{
			p1++;
			
			if(*p1 == '+')
				flag = 0;
			else if(*p1 == '-')
				flag = 1;
			else
				goto out;

			p1++;
			p2 = strchr(p1, '.');
			if(p2 != NULL)
			{
				p2++;
				temp_val1 = simple_strtoul(p1, &ep2, 0);
				temp_val2 = simple_strtoul(p2, &ep, 0);
			}
			else
			{
				temp_val1 = simple_strtoul(p1, &ep, 0);
				temp_val2 = 0;
			}

			if(!(((flag == 0) && (temp_val1 <= 127) && (((temp_val1*10 + temp_val2)%5) == 0)) ||
				((flag == 1) && (temp_val1 <= 128) && (((temp_val1*10 + temp_val2)%5) == 0))))
			{
				printk("VAL: Tos: multiples of 0.5, range: From -128 to +127.5\n");
				goto out;
			}

			if(flag == 0)
			{
				printk("tos value: +%d.%d\n", temp_val1, temp_val2);
				write_val = (temp_val1*10 + temp_val2)/5;
				printk("Tos hex value: 0x%x\n", write_val);
				write_val = (write_val << 7);
				ret = pct2075_config("write", PCT2075_REG_TOS, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
			else if(flag == 1)
			{
				printk("tos value: -%d.%d\n", temp_val1, temp_val2);
				write_val = 0x1FF - (temp_val1*10 + temp_val2)/5 + 1;
				printk("Tos hex value: 0x%x\n", write_val);
				write_val = (write_val << 7);
				ret = pct2075_config("write", PCT2075_REG_TOS, &write_val);
				printk("write value: 0x%x\n", write_val);
			}
		}
	}
	else if(!strncmp(line, "read", 4))
	{
		ret = pct2075_config("read", PCT2075_REG_TOS, &read_val);
		printk("reg value: 0x%x\n", read_val);
		read_val = (read_val >> 7);
		printk("Tos hex value: 0x%x\n", read_val);
		if((read_val & 0x100) == 0)
			printk("Tos: +%d.%d\n", (read_val*5)/10, (read_val*5)%10);
		else
			printk("Tos: -%d.%d\n", ((0x1FF - read_val + 1)*5)/10, ((0x1FF - read_val + 1)*5)%10);
	}

out:
	return count;
}

static int tidle_reg_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,
            "Usage:\n"
            "PCT2075 Tidle Time read:  echo read > /proc/pct2075/tidle\n"
            "PCT2075 Tidle Time write: echo write val(Integer multiples of 100, Unit: ms, range: 100-3100) > /proc/pct2075/tidle\n");

	return 0;
}

static int tidle_reg_open(struct inode *inode, struct file *file)
{
	return single_open(file, tidle_reg_show, PDE_DATA(inode));
}

static int tidle_reg_write(struct file *file, const char __user * user_buffer, size_t count, loff_t * ppos)
{
	char line[64];
	char empty= ' ';
	char *ep = &empty;
	char *p1;
	u8 write_val, read_val;
	u16 temp_val;
	int ret;

	ret = copy_from_user(line, user_buffer, count);
	if(ret)
		return -EFAULT;

	if(!strncmp(line, "write", 5))
	{
		p1 = strchr(line, ' ');
		if(p1 != NULL)
		{
			p1++;
			temp_val = simple_strtoul(p1, &ep, 0);
			if((temp_val < 100) || (temp_val > 3100) || ((temp_val % 100) != 0))
			{
				printk("VAL: Integer multiples of 100, Unit: ms, range: 100-3100\n");
				goto out;
			}
			printk("tidle time value: %dms\n", temp_val);
			write_val = temp_val/100;
			ret = pct2075_config("write", PCT2075_REG_TIDLE, &write_val);
			printk("write value: 0x%x\n", write_val);
		}
	}
	else if(!strncmp(line, "read", 4))
	{
		ret = pct2075_config("read", PCT2075_REG_TIDLE, &read_val);
		printk("reg value: 0x%x\n", read_val);
		if((read_val*100) > 1000)
			printk("tidle time value: %d.%ds\n", (read_val*100)/1000, ((read_val*100)%1000)/100);
		else
			printk("tidle time value: %dms\n", read_val*100);
	}

out:
	return count;
}

static struct file_operations pct2075_proc_temp_reg_fops =
{
	.owner = THIS_MODULE,
	.open = temp_reg_open,
	.read = seq_read,
	.write = temp_reg_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations pct2075_proc_conf_reg_fops =
{
	.owner = THIS_MODULE,
	.open = conf_reg_open,
	.read = seq_read,
	.write = conf_reg_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations pct2075_proc_thyst_reg_fops =
{
	.owner = THIS_MODULE,
	.open = thyst_reg_open,
	.read = seq_read,
	.write = thyst_reg_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations pct2075_proc_tos_reg_fops =
{
	.owner = THIS_MODULE,
	.open = tos_reg_open,
	.read = seq_read,
	.write = tos_reg_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations pct2075_proc_tidle_reg_fops =
{
	.owner = THIS_MODULE,
	.open = tidle_reg_open,
	.read = seq_read,
	.write = tidle_reg_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int pct2075_proc_fs_create(void)
{
	struct proc_dir_entry *proc_pct2075_dir = NULL;

	proc_pct2075_dir = proc_mkdir("pct2075", NULL);
	if(!proc_pct2075_dir)
		return -ENOMEM;

	proc_create("temp", 0644, proc_pct2075_dir, &pct2075_proc_temp_reg_fops);
	proc_create("conf", 0644, proc_pct2075_dir, &pct2075_proc_conf_reg_fops);
	proc_create("thyst", 0644, proc_pct2075_dir, &pct2075_proc_thyst_reg_fops);
	proc_create("tos", 0644, proc_pct2075_dir, &pct2075_proc_tos_reg_fops);
	proc_create("tidle", 0644, proc_pct2075_dir, &pct2075_proc_tidle_reg_fops);

	return 0;
}

static int pct2075_proc_fs_destroy(void)
{
	remove_proc_entry("pct2075/temp", NULL);
	remove_proc_entry("pct2075/conf", NULL);
	remove_proc_entry("pct2075/thyst", NULL);
	remove_proc_entry("pct2075/tos", NULL);
	remove_proc_entry("pct2075/tidle", NULL);
	remove_proc_entry("pct2075", NULL);

	return 0;
}

static int pct2075_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("pct2075 probe\n");
	
	g_client = client;
	pct2075_proc_fs_create();
	
	return 0;
}

static int pct2075_remove(struct i2c_client *client)
{
	printk("pct2075_remove\n!");
	pct2075_proc_fs_destroy();
	return 0;
}

static const struct i2c_device_id pct2075_id[] = {
	{PCT2075_SENSOR_NAME, PCT2075_SENSOR_ADDR},
	{}
};
static struct i2c_driver pct2075_driver = {
	.driver = {
	 	.name = PCT2075_SENSOR_NAME,
	},
	.probe = pct2075_probe,
	.remove = pct2075_remove,
	.id_table = pct2075_id,
};

static struct i2c_board_info pct2075_device = {
	I2C_BOARD_INFO(PCT2075_SENSOR_NAME, PCT2075_SENSOR_ADDR),
};

static int __init pct2075_init(void)
{
	struct i2c_adapter *adap;
	struct i2c_client *client;

	printk("pct2075 init!\n");
	adap = i2c_get_adapter(0);
	if(!adap)
	{
		printk("get i2c adapter %d fail\n", PCT2075_SENSOR_I2C);
		return -ENODEV;
	}
	else
	{
		printk("get i2c adapter %d ok\n", PCT2075_SENSOR_I2C);
		client = i2c_new_device(adap, &pct2075_device);
	}
	if(!client)
	{
		printk("get i2c client %s @ 0x%02x fail\n", pct2075_device.type, pct2075_device.addr);
		return -ENODEV;
	}
	else
	{
		printk("get i2c client %s @ 0x%02x ok\n", pct2075_device.type, pct2075_device.addr);
	}
	i2c_put_adapter(adap);
	i2c_add_driver(&pct2075_driver);
	printk("pct2075 init success!\n");
	return 0;
}

static void __exit pct2075_exit(void)
{
	printk("pct2075 exit!\n");
	i2c_del_driver(&pct2075_driver);
	if(g_client != NULL)
		i2c_unregister_device(g_client);
	printk("pct2075 exit success!\n");
}

module_init(pct2075_init);
module_exit(pct2075_exit);

MODULE_DESCRIPTION("pct2075 driver");
MODULE_LICENSE("GPL");

