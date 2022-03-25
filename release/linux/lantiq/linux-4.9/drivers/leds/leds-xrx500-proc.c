
#include "leds-xrx500-config.h"

#ifdef CONFIG_LEDS_CLASS
struct led_sysfs_help {
	char *cmd;		
	char *help;	
	char *usage;	
};
static struct led_sysfs_help help_entries[] = {
{"hw_blink_en",
 "\r\nA mask to enable or disable the HW blink for all the individual LEDs\r\n",
 "\r\ncat hw_blink_en -> prints the HW blink mask\r\n \
      echo 0x2A0 > hw_blink_en -> e.g)To enable HW blink for leds 5, 7, 9"},
{"auto_dimming_en",
 "\r\nEnable/Disable HW dimming function\r\n",
 "\r\ncat auto_dimming_en -> prints the current status of HW dimming feature\r\n \
      echo 0x1 > auto_dimming_en -> e.g)To enable HW dimming\r\n \
	  echo 0x0 > auto_dimming_en -> e.g)To disable HW dimming"},
{"blink_rate",
"\r\nSet this bit to have a blink rate different from update rate for LEDs[0-23]\r\n",
"\r\ncat blink_rate -> prints the Blink rate Update control\r\n \
echo 0x1 > blink_rate -> e.g)Blink rate is different from Update rate\r\n \
echo 0x0 > blink_rate -> e.g)Blink rate is same as Update rate" },
{"cpu_select",
"\r\nselect cpu for data source\r\n",
"\r\ncat cpu_select -> prints the data source \r\n \
echo 0 > cpu_select -> sets cpu 0 as the source\r\n \
echo 1 > cpu_select -> sets cpu 1 as the source\r\n"},
{"data_offset",
"\r\nset the offset of the LED shift clock before data out\r\n",
"\r\ncat data_offset -> prints the data_offset setting\r\n \
echo  0 > data_offset ->possible input values are 0,1,2,3"},
{"fpi_shift",
"\r\nFixed Divider Shift Clock\r\n",
"\r\ncat fpi_shift -> prints the current fpi_shift setting\r\n \
echo  0 > fpi_shift ->possible input values are 0,1,2,3"},
{"fpi_update",
"\r\nFSC Divider for LED [23:0]\r\n",
"\r\ncat fpi_update -> prints the current fpi_update setting\r\n \
echo  0 > fpi_update ->possible input values are 0,1,2,3"},
{"gptc_divider",
"\r\nGPT Divider for LED[23:0]\r\n",
"\r\ncat gptc_divider -> prints the current gptc_divider setting\r\n \
echo  0 > gptc_divider ->possible input values are 0,1,2,3"},
{"number_of_leds",
"\r\nLED group information (Read Only)\r\n",
"\r\ncat number_of_leds -> prints one of the following values 0, 8, 16, 24, 32\r\n"},
{"lux_dis", 
"\r\ndischarge time is this programmed 6 bitvalue x LUX_CNV_INT period programmed\r\n",
"\r\ncat lux_dis -> prints the current value\r\n \
echo 10 > lux_dis -> sets the current value to 10\r\n"},
{"lux_cnv_interval", 
"\r\nProgrammable divider for intervals used by LUX conversion\r\n",
"\r\ncat lux_cnv_interval ->prints the current LUX interval value\r\n \
echo 0 > lux_cnv_interval -> writes 0 to lux_cnv_interval, possible values are the following \
0  DO no division of 2560Hz\r\n \
1  D1 division of 2, equals to 1280Hz\r\n \
2  D2 Division of 4, equals to 640Hz\r\n \
3  D3 Division of 8, equals to 320Hz\r\n \
4  D4 Division of 16, equals to 160Hz\r\n \
5  D5 Division of 32, equals to 80Hz\r\n \
6  D6 Division of 64, equals to 40HZ\r\n \
7  D7 Division of 128, equals to 20Hz\r\n \
8  D8 Division of 256, equals to 10 Hz\r\n \
9  D9 Division of 512, equals to 5Hz\r\n \
A  D10 Division of 1024, equals to 2.5Hz\r\n \
B  D11 Division of 2048, equals to 1.25Hz\r\n \
"},
{"lux_lvl",
"\r\nLUX Level Converted by Hardware (Read Only)\r\n",
"\r\ncat lux_lvl -> prints the current calculated value of LUX level\r\n"},
{"store_mode",
"\r\nSetting to switch between LED single pulse mode or inverted shift clock during update",
"\r\ncat store_mode -> prints the current store _mode\r\n \
echo 1 > store_mode ->sets the value to 1, possible values are the following \
0 Single\r\n \
1 Clock \r\n"},
{"update_clock_source",
"\r\nUpdate Source for LEDS\r\n",
"\r\ncat update_clock_source -> prints the current update source\r\n \
echo SW > update_clock_source, possible values are the following \
SW\r\n\
GPT\r\n\
FPI\r\n \
"},
{"update_edge",
"\r\nData Clocking Edge\r\n",
"\r\ncat update_edge ->prints the data clocking edge\r\n \
echo 0 > update_edge -> to define if the data output is clocked at rising edge\r\n \
echo 1 > update_edge -> to define if the data output is clocked at falling edge\r\n"},
/*{"update_src",
"\r\nSwitch the LED source between LEDc and External hardware\r\n",
"\r\ncat update_src -> prints the current setting\r\n \
echo ledc > update_src ->LED to be controlled by LEDc\r\n\
echo ext > update_src ->LED to be controlled by the External HW"},*/
{"blink_src",
"\r\nselect the blink source corresponding to each LED in the range of 24 - 31\r\n",
"\r\ncat blink_src -> prints the current blink source for that LED\r\n \
echo FPID > blink_src ->sets the blink source to FPID, possible values are\r\n \
FPID ->blink at FPID output rate\r\n \
GPTD ->blink at GPT output rate\r\n \
V1M ->blink at GPT high speed output rate\r\n \
LEDS[24-31] have individual setting for blink source"},
{"blink_src_0_23",
"\r\nselect the blink source for LEDs in the range of LEDs 0 - 23\r\n",
"\r\ncat blink_src -> prints the current blink source for LEDs 0 - 23\r\n \
echo FPID > blink_src ->sets the blink source to FPID, possible values are\r\n \
FPID ->blink at FPID output rate\r\n \
GPTD ->blink at GPT output rate\r\n \
V1M ->blink at GPT high speed output rate\r\n \
LEDS[0-23] have a common setting for blink source which is only valid if blink_rate is set to 1"}
};
static struct xrx500_sso *g_chip;
static int cpu_select = 0;
static ssize_t grx500_blink_src_0_23_show(struct device *dev,
			struct device_attribute *attr,
			char *buf);	
static ssize_t grx500_blink_src_0_23_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len);
static DEVICE_ATTR(blink_src_0_23, S_IRUGO | S_IWUSR,
		grx500_blink_src_0_23_show, grx500_blink_src_0_23_store);
	
int get_cpu_select() {
	return cpu_select;
}

static ssize_t grx500_led_usage_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	ssize_t status = 0, temp = 0;
	int i, n;
	char *str = buf;
	n = sprintf(str, "%s         %s\r\n","Command", "Description");	
	str += n;
	status += n;

	for (i = 0; i < sizeof(help_entries) / sizeof(help_entries[0]);
	     i++) {
		temp = sprintf(str, "%s  %s\r\n", help_entries[i].cmd, help_entries[i].help);
		str += temp;
		status += temp;
		
	}
	return status;
}

static ssize_t grx500_led_usage_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t len)
{
	ssize_t   status = 0;
	int i;
	char *buff = (char *)buf;
	char *token = NULL;
	token = strsep(&buff, " "); 
	if (token != 0) {
		for (i = 0; i < sizeof(help_entries) / sizeof(help_entries[0]);
	     i++) {
			//if(strncmp(help_entries[i].cmd, token, strlen(help_entries[i].cmd)) == 0) {
			if(strncmp(help_entries[i].cmd, token, strlen(token) - 1) == 0) {

				printk("%s\r\n", help_entries[i].usage);	
				break;
			}				
		}
	} else {
		status = -EINVAL;
		pr_err("Invalid Command\r\n");
	
	}
	return status ? : len;
}

static DEVICE_ATTR(led_usage, S_IRUGO | S_IWUSR,
		grx500_led_usage_show, grx500_led_usage_store);
			
static ssize_t grx500_blink_rate_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	ssize_t status = 0;
	g_chip->blink_rate = (xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0) & XRX500_SSO_BLINK_RATE_MASK) >> XRX500_SSO_BLINK_RATE_SHIFT;
	status = sprintf(buf, "0x%x\n", g_chip->blink_rate);
	return status;
}

static ssize_t grx500_blink_rate_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t len)
{
	ssize_t   status = 0;
	int rc;
	long value;
	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 1)) {
			g_chip->blink_rate = value;
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_BLINK_RATE_MASK,
			g_chip->blink_rate << XRX500_SSO_BLINK_RATE_SHIFT,
			XRX500_SSO_CON0);
			if(value) {
				rc = device_create_file(dev, &dev_attr_blink_src_0_23);
				if (rc)
					pr_err("Error creating attribute blink_src_0_23");
			} else {
				device_remove_file(dev, &dev_attr_blink_src_0_23);
			}
		} else {
			status = -EINVAL;
			pr_err("Valid input is 0 or 1\r\n");
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(blink_rate, S_IRUGO | S_IWUSR,
		grx500_blink_rate_show, grx500_blink_rate_store);
		
static ssize_t grx500_blink_en_show(struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	ssize_t status = 0;
	status = sprintf(buf, "0x%x\n", g_chip->blink_mask);
	return status;
}

static ssize_t grx500_blink_en_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t len)
{
	ssize_t   status = 0;
	unsigned long value;
	status = kstrtoul(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0xffffffff)) {
			g_chip->blink_mask = value;
		} else {
			status = -EINVAL;
			pr_err("Valid input is from 0x0 - 0xffffffff\r\n");
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(hw_blink_en, S_IRUGO | S_IWUSR,
		grx500_blink_en_show, grx500_blink_en_store);
		
static ssize_t grx500_cpu_select_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	ssize_t status = 0;
	status = sprintf(buf, "%d\n", cpu_select);
	return status;
}

static ssize_t grx500_cpu_select_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t len)
{
	ssize_t   status = 0;
	long value;
	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 1)) {
			cpu_select = value;
		} else {
			status = -EINVAL;
			pr_err("Valid input is 0 or 1\r\n");
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(cpu_select, S_IRUGO | S_IWUSR,
		grx500_cpu_select_show, grx500_cpu_select_store);
		
static ssize_t grx500_gptc_divider_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	status = sprintf(buf, "%d\n",
	(reg & XRX500_SSO_GPTD_MASK)  >> XRX500_SSO_GPTD_SHIFT);
	return status;
}

static ssize_t grx500_gptc_divider_store(struct device *dev,
	struct device_attribute *attr,
	const char *buf, size_t len)
{
	ssize_t   status = 0;
	long value;
	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 3)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_GPTD_MASK,
			value << XRX500_SSO_GPTD_SHIFT, XRX500_SSO_CON1);
		} else {
			status = -EINVAL;
		}
	}
	return status ? : len;

}

static DEVICE_ATTR(gptc_divider, S_IRUGO | S_IWUSR,
		grx500_gptc_divider_show, grx500_gptc_divider_store);

static ssize_t grx500_fpi_shift_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	status = sprintf(buf, "%d\n",
	(reg & XRX500_SSO_FCDSC_MASK)  >> XRX500_SSO_FCDSC_SHIFT);
	return status;
}

static ssize_t grx500_fpi_shift_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;
	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 3)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_FCDSC_MASK,
			value << XRX500_SSO_FCDSC_SHIFT, XRX500_SSO_CON1);
		} else {
			status = -EINVAL;
		}
	}
	return status ? : len;

}

static DEVICE_ATTR(fpi_shift, S_IRUGO | S_IWUSR,
		grx500_fpi_shift_show, grx500_fpi_shift_store);

static ssize_t grx500_fpi_update_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;

	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	status = sprintf(buf, "%d\n",
	(reg & XRX500_SSO_FPID_MASK) >> XRX500_SSO_FPID_SHIFT);
	return status;
}

static ssize_t grx500_fpi_update_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;
	status = kstrtol(buf, 0, &value);


	if (status == 0) {
		if ((value >= 0) && (value <= 3)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_FPID_MASK,
			value << XRX500_SSO_FPID_SHIFT, XRX500_SSO_CON1);
		} else {
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(fpi_update, S_IRUGO | S_IWUSR,
		grx500_fpi_update_show, grx500_fpi_update_store);

static ssize_t grx500_update_edge_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0);
	status = sprintf(buf, "%lu\n",
	(reg & XRX500_SSO_EDGE_MASK) >> XRX500_SSO_EDGE_SHIFT);
	return status;
}

static ssize_t grx500_update_edge_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;
	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 1)) {
			xrx500_sso_w32_mask(g_chip->virt, XRX500_SSO_EDGE_MASK,
			value << XRX500_SSO_EDGE_SHIFT, XRX500_SSO_CON0);
		} else {
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(update_edge, S_IRUGO | S_IWUSR,
		grx500_update_edge_show, grx500_update_edge_store);

static ssize_t grx500_led_number_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t   status = 0;

	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	if ((reg & 0xF) == 0xF)
		reg = 32;
	if ((reg & 0x7) == 0x7)
		reg = 24;
	if ((reg & 0x3) == 0x3)
		reg = 16;
	if ((reg & 0x1) == 0x1)
		reg = 8;
	status = sprintf(buf, "%d\n", reg);
	return status;
}
#if 0
static ssize_t grx500_led_number_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t   status = 0;
	u32 bits = 0, i = 0;
	long value;
	status = kstrtol(buf, 0, &value);


	if (status == 0) {
		if (!(((value%8) == 0) && ((value >= 0) && (value <= 32))))
			return -EINVAL;
		if (value) {
			while (value/8) {
				bits |= BIT(i);
				i++;
				value -= 8;
			}
			xrx500_sso_w32_mask(g_chip->virt, bits,
				bits, XRX500_SSO_CON1);
		} else {
			xrx500_sso_w32_mask(g_chip->virt, XRX500_SSO_GROUP_MASK,
				0, XRX500_SSO_CON1);
		}
	}
	return status ? : len;

}
#endif
static DEVICE_ATTR(number_of_leds, S_IRUGO | S_IWUSR,
		grx500_led_number_show, NULL);

static ssize_t grx500_update_clock_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	reg = reg >> XRX500_SSO_UPD_SHIFT;
	if (reg == 0)
		status = sprintf(buf, "%s\n", "SW");
	else if (reg == 1)
		status = sprintf(buf, "%s\n", "GPT");
	else if (reg == 2)
		status = sprintf(buf, "%s\n", "FPI");
	else
		status = sprintf(buf, "%s\n", "INVALID");
	return status;
}

static ssize_t grx500_update_clock_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	u32 value;
	if (sysfs_streq(buf, "SW"))
		value = 0;
	else if (sysfs_streq(buf, "GPT"))
		value = 1;
	else if (sysfs_streq(buf, "FPI"))
		value = 2;
	else
		status = -EINVAL;


	if (status == 0) {
		xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_UPD_MASK,
			value << XRX500_SSO_UPD_SHIFT, XRX500_SSO_CON1);
		g_chip->upd = value;
	}
	return status ? : len;
}
static DEVICE_ATTR(update_clock_source, S_IRUGO | S_IWUSR,
		grx500_update_clock_show, grx500_update_clock_store);

static ssize_t grx500_store_mode_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	reg = (reg & XRX500_SSO_STORE_MODE_MASK) >> XRX500_SSO_STORE_MODE_SHIFT;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static ssize_t grx500_store_mode_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;

	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0x1)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_STORE_MODE_MASK,
			value << XRX500_SSO_STORE_MODE_SHIFT, XRX500_SSO_CON1);
		} else {
			pr_err("Valid input is 0 or 1\r\n");
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(store_mode, S_IRUGO | S_IWUSR,
		grx500_store_mode_show, grx500_store_mode_store);
		
static ssize_t grx500_data_offset_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON1);
	reg = (reg & XRX500_SSO_DATA_OFFSET_MASK)  >> XRX500_SSO_DATA_OFFSET_SHIFT;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static ssize_t grx500_data_offset_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;

	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0x3)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_DATA_OFFSET_MASK,
			value << XRX500_SSO_DATA_OFFSET_SHIFT, XRX500_SSO_CON1);
		} else {
			pr_err("Valid input range is 0x0 - 0x3\r\n");
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(data_offset, S_IRUGO | S_IWUSR,
		grx500_data_offset_show, grx500_data_offset_store);


static ssize_t grx500_lux_level_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0);
	reg = (reg >> XRX500_SSO_LUX_LEVEL_SHIFT)  & XRX500_SSO_LUX_LEVEL_MASK;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static DEVICE_ATTR(lux_lvl, S_IRUGO | S_IWUSR,
		grx500_lux_level_show, NULL);

static ssize_t grx500_lux_discharge_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0);
	reg = (reg & XRX500_SSO_LUX_DISCHARGE_MASK) >> XRX500_SSO_LUX_DISCHARGE_SHIFT;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static ssize_t grx500_lux_discharge_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;

	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0x3F)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_LUX_DISCHARGE_MASK,
			value << XRX500_SSO_LUX_DISCHARGE_SHIFT, XRX500_SSO_CON0);
		} else {
			pr_err("Valid input range is 0x0 - 0x3F\r\n");
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(lux_dis, S_IRUGO | S_IWUSR,
		grx500_lux_discharge_show, grx500_lux_discharge_store);

static ssize_t grx500_lux_interval_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0);
	reg = (reg & XRX500_SSO_LUX_INTERVAL_MASK) >> XRX500_SSO_LUX_INTERVAL_SHIFT;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static ssize_t grx500_lux_interval_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;

	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0xB)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_LUX_INTERVAL_MASK,
			value << XRX500_SSO_LUX_INTERVAL_SHIFT, XRX500_SSO_CON0);
		} else {
			status = -EINVAL;
			pr_err("Valid input range is 0x0 - 0xB\r\n");

		}
	}
	return status ? : len;
}

static DEVICE_ATTR(lux_cnv_interval, S_IRUGO | S_IWUSR,
		grx500_lux_interval_show, grx500_lux_interval_store);

static ssize_t grx500_dimm_enable_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_CON0);
	reg = (reg & XRX500_SSO_DIMM_ENABLE_MASK) >> XRX500_SSO_DIMM_ENABLE_SHIFT;
	status = sprintf(buf, "%d\n", reg);
	return status;
}

static ssize_t grx500_dimm_enable_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	long value;

	status = kstrtol(buf, 0, &value);
	if (status == 0) {
		if ((value >= 0) && (value <= 0x1)) {
			xrx500_sso_w32_mask(g_chip->virt,
			XRX500_SSO_DIMM_ENABLE_MASK,
			value << XRX500_SSO_DIMM_ENABLE_SHIFT, XRX500_SSO_CON0);
		} else {
			pr_err("Valid input is 0 or 1\r\n");
			status = -EINVAL;
		}
	}
	return status ? : len;
}

static DEVICE_ATTR(auto_dimming_en, S_IRUGO | S_IWUSR,
		grx500_dimm_enable_show, grx500_dimm_enable_store);

#if 0

static DEVICE_ATTR(blink_src, S_IRUGO | S_IWUSR,
		grx500_blink_src_show, grx500_blink_src_store);
#endif		
static ssize_t grx500_blink_src_0_23_store(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t len)
{
	ssize_t status = 0;
	u32 value;
	int i;
	struct xrx500_sso_led *led = (struct xrx500_sso_led *) dev;
	
	if (sysfs_streq(buf, "FPID"))
		value = 0;
	else if (sysfs_streq(buf, "GPTD"))
		value = 1;
	else if (sysfs_streq(buf, "V1M"))
		value = 2;
	else
		status = -EINVAL;


	if (status == 0) {
		if((led->phys_id >= 0) && (led->phys_id <= 23)) {
			xrx500_sso_w32_mask(g_chip->virt,
			0x3 << XRX500_SSO_BLINK_SOURCE_SHIFT,
			value << XRX500_SSO_BLINK_SOURCE_SHIFT,
			XRX500_SSO_BLINK_H8_1);
		} else {
			i = led->phys_id % 24;
			xrx500_sso_w32_mask(g_chip->virt,
			0x3 << (((i%5) * 6) + 4),
			value << (((i%5) * 6) + 4),
			((i < 5) ?
			XRX500_SSO_BLINK_H8_0 : XRX500_SSO_BLINK_H8_1));	
		}
	}
	return status ? : len;
}

static ssize_t grx500_blink_src_0_23_show(struct device *dev,
			struct device_attribute *attr,
			char *buf)
{
	u32 reg;
	ssize_t status = 0;
	int i;

	struct xrx500_sso_led *led = (struct xrx500_sso_led *) dev;
	if((led->phys_id >= 0) && (led->phys_id <= 23)) {
		reg = xrx500_sso_r32(g_chip->virt, XRX500_SSO_BLINK_H8_1);
		reg = (reg >> XRX500_SSO_BLINK_SOURCE_SHIFT) & 0x3;
	} else {
		i = led->phys_id % 24;
		reg = xrx500_sso_r32(g_chip->virt, ((i < 5) ? XRX500_SSO_BLINK_H8_0 : XRX500_SSO_BLINK_H8_1));
		reg = (reg >> (((i%5) * 6) + 4)) & 0x3;
	}
	switch (reg) {
		case 0:
			status = sprintf(buf, "%s\n", "FPID");
		break;
		case 1:
			status = sprintf(buf, "%s\n", "GPTD");
		break;
		case 2:
			status = sprintf(buf, "%s\n", "V1M");
		break;
	}
	return status;
}


static struct attribute *grx500_led_attributes[] = {
	&dev_attr_update_clock_source.attr,
	&dev_attr_number_of_leds.attr,
	&dev_attr_update_edge.attr,
	&dev_attr_fpi_update.attr,
	&dev_attr_fpi_shift.attr,
	&dev_attr_gptc_divider.attr,
	&dev_attr_auto_dimming_en.attr,
	&dev_attr_lux_cnv_interval.attr,
	&dev_attr_lux_dis.attr,
	&dev_attr_lux_lvl.attr,
	&dev_attr_data_offset.attr,
	&dev_attr_store_mode.attr,
	&dev_attr_cpu_select.attr,
	&dev_attr_hw_blink_en.attr,
	&dev_attr_blink_rate.attr,
	&dev_attr_led_usage.attr,
	NULL,
};

static const struct attribute_group grx500_attr_group = {
	.attrs = grx500_led_attributes,
};

int grx500_register_sysfs(struct platform_device *pdev)
{
	int ret = 0;
	struct device *dev;
	g_chip = (struct xrx500_sso *)platform_get_drvdata(pdev);
	dev = g_chip->gc.parent;
	ret = sysfs_create_group(&dev->kobj, &grx500_attr_group);
	return ret;

}
#endif

