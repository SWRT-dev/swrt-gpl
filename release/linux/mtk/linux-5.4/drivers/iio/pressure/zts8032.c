// SPDX-License-Identifier: GPL-2.0
/*
 * The ZTS8032 is a barometric pressure and temperature sensor.
 * Currently only reading a single temperature is supported by
 * this driver.
 */

#include <linux/i2c.h>
#include <linux/limits.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/regmap.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>

#define ZTS8032_DEV_NAME	"zts8032"

#define ZTS8032_PRS_B0		0x00
#define ZTS8032_PRS_B1		0x01
#define ZTS8032_PRS_B2		0x02
#define ZTS8032_TMP_B0		0x03
#define ZTS8032_TMP_B1		0x04
#define ZTS8032_TMP_B2		0x05
#define ZTS8032_PRS_CFG		0x06
#define  ZTS8032_PRS_RATE_BITS	GENMASK(6, 4)
#define  ZTS8032_PRS_PRC_BITS	GENMASK(3, 0)
#define ZTS8032_TMP_CFG		0x07
#define  ZTS8032_TMP_RATE_BITS	GENMASK(6, 4)
#define  ZTS8032_TMP_PRC_BITS	GENMASK(3, 0)
#define  ZTS8032_TMP_EXT	BIT(7)
#define ZTS8032_MEAS_CFG	0x08
#define  ZTS8032_MEAS_CTRL_BITS	GENMASK(2, 0)
#define   ZTS8032_PRS_EN	BIT(0)
#define   ZTS8032_TEMP_EN	BIT(1)
#define   ZTS8032_BACKGROUND	BIT(2)
#define  ZTS8032_PRS_RDY	BIT(4)
#define  ZTS8032_TMP_RDY	BIT(5)
#define  ZTS8032_SENSOR_RDY	BIT(6)
#define  ZTS8032_COEF_RDY	BIT(7)
#define ZTS8032_CFG_REG		0x09
#define  ZTS8032_INT_HL		BIT(7)
#define  ZTS8032_TMP_SHIFT_EN	BIT(3)
#define  ZTS8032_PRS_SHIFT_EN	BIT(4)
#define  ZTS8032_FIFO_EN	BIT(5)
#define  ZTS8032_SPI_EN		BIT(6)
#define ZTS8032_RESET		0x0c
#define  ZTS8032_RESET_MAGIC	0x09
#define ZTS8032_COEF_BASE	0x10

/* Make sure sleep time is <= 20ms for usleep_range */
#define ZTS8032_POLL_SLEEP_US(t)		min(20000, (t) / 8)
/* Silently handle error in rate value here */
#define ZTS8032_POLL_TIMEOUT_US(rc)	((rc) <= 0 ? 1000000 : 1000000 / (rc))

#define ZTS8032_PRS_BASE		ZTS8032_PRS_B0
#define ZTS8032_TMP_BASE		ZTS8032_TMP_B0

/*
 * These values (defined in the spec) indicate how to scale the raw register
 * values for each level of precision available.
 */
static const int scale_factors[] = {
	 524288,
	1572864,
	3670016,
	7864320,
	 253952,
	 516096,
	1040384,
	2088960,
};

struct zts8032_data {
	struct i2c_client *client;
	struct regmap *regmap;
	struct mutex lock;	/* Lock for sequential HW access functions */

	s32 c0, c1;
	s32 c00, c10, c20, c30, c40, c01, c11, c21, c31;
	s32 pressure_raw;
	s32 temp_raw;
};

static const struct iio_chan_spec zts8032_channels[] = {
	{
		.type = IIO_TEMP,
		.info_mask_separate = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO) |
			BIT(IIO_CHAN_INFO_SAMP_FREQ) |
			BIT(IIO_CHAN_INFO_PROCESSED),
	},
	{
		.type = IIO_PRESSURE,
		.info_mask_separate = BIT(IIO_CHAN_INFO_OVERSAMPLING_RATIO) |
			BIT(IIO_CHAN_INFO_SAMP_FREQ) |
			BIT(IIO_CHAN_INFO_PROCESSED),
	},
};

/* To be called after checking the COEF_RDY bit in MEAS_CFG */
static int zts8032_get_coefs(struct zts8032_data *data)
{
	int rc;
	u8 coef[22];
	u32 c0, c1;
	u32 c00, c10, c20, c30, c40, c01, c11, c21, c31;

	/* Read all sensor calibration coefficients from the COEF registers. */
	rc = regmap_bulk_read(data->regmap, ZTS8032_COEF_BASE, coef,
			      sizeof(coef));
	if (rc < 0)
		return rc;

	/*
	 * Calculate temperature calibration coefficients c0 and c1. The
	 * numbers are 12-bit 2's complement numbers.
	 */
	c0 = (coef[0] << 4) | (coef[1] >> 4);
	data->c0 = sign_extend32(c0, 11);

	c1 = ((coef[1] & GENMASK(3, 0)) << 8) | coef[2];
	data->c1 = sign_extend32(c1, 11);

	/*
	 * Calculate pressure calibration coefficients. c00 and c10 are 20 bit
	 * 2's complement numbers, while the rest are 16 bit 2's complement
	 * numbers.
	 */
	c00 = (coef[3] << 12) | (coef[4] << 4) | (coef[5] >> 4);
	data->c00 = sign_extend32(c00, 19);

	c10 = ((coef[5] & GENMASK(3, 0)) << 16) | (coef[6] << 8) | coef[7];
	data->c10 = sign_extend32(c10, 19);

	c01 = (coef[8] << 8) | coef[9];
	data->c01 = sign_extend32(c01, 15);

	c11 = (coef[10] << 8) | coef[11];
	data->c11 = sign_extend32(c11, 15);

	c20 = (coef[12] << 8) | coef[13];
	data->c20 = sign_extend32(c20, 15);

	c21 = (coef[14] << 8) | coef[15];
	data->c21 = sign_extend32(c21, 15);

	c30 = (coef[16] << 8) | coef[17];
	data->c30 = sign_extend32(c30, 15);

	c31 = (coef[18] << 8) | coef[19];
	data->c31 = sign_extend32(c31, 15);

	c40 = (coef[20] << 8) | coef[21];
	data->c40 = sign_extend32(c40, 15);

	return 0;
}

static int zts8032_get_pres_precision(struct zts8032_data *data)
{
	int rc;
	int val;

	rc = regmap_read(data->regmap, ZTS8032_PRS_CFG, &val);
	if (rc < 0)
		return rc;

	return BIT(val & GENMASK(2, 0));
}

static int zts8032_get_temp_precision(struct zts8032_data *data)
{
	int rc;
	int val;

	rc = regmap_read(data->regmap, ZTS8032_TMP_CFG, &val);
	if (rc < 0)
		return rc;

	/*
	 * Scale factor is bottom 4 bits of the register, but 1111 is
	 * reserved so just grab bottom three
	 */
	return BIT(val & GENMASK(2, 0));
}

/* Called with lock held */
static int zts8032_set_pres_precision(struct zts8032_data *data, int val)
{
	int rc;
	u8 shift_en;

	if (val < 0 || val > 128)
		return -EINVAL;

	shift_en = val >= 16 ? ZTS8032_PRS_SHIFT_EN : 0;
	rc = regmap_write_bits(data->regmap, ZTS8032_CFG_REG,
			       ZTS8032_PRS_SHIFT_EN, shift_en);
	if (rc)
		return rc;

	return regmap_update_bits(data->regmap, ZTS8032_PRS_CFG,
				  ZTS8032_PRS_PRC_BITS, ilog2(val));
}

/* Called with lock held */
static int zts8032_set_temp_precision(struct zts8032_data *data, int val)
{
	int rc;
	u8 shift_en;

	if (val < 0 || val > 128)
		return -EINVAL;

	shift_en = val >= 16 ? ZTS8032_TMP_SHIFT_EN : 0;
	rc = regmap_write_bits(data->regmap, ZTS8032_CFG_REG,
			       ZTS8032_TMP_SHIFT_EN, shift_en);
	if (rc)
		return rc;

	return regmap_update_bits(data->regmap, ZTS8032_TMP_CFG,
				  ZTS8032_TMP_PRC_BITS, ilog2(val));
}

/* Called with lock held */
static int zts8032_set_pres_samp_freq(struct zts8032_data *data, int freq)
{
	u8 val;

	if (freq < 0 || freq > 128)
		return -EINVAL;

	val = ilog2(freq) << 4;

	return regmap_update_bits(data->regmap, ZTS8032_PRS_CFG,
				  ZTS8032_PRS_RATE_BITS, val);
}

/* Called with lock held */
static int zts8032_set_temp_samp_freq(struct zts8032_data *data, int freq)
{
	u8 val;

	if (freq < 0 || freq > 128)
		return -EINVAL;

	val = ilog2(freq) << 4;

	return regmap_update_bits(data->regmap, ZTS8032_TMP_CFG,
				  ZTS8032_TMP_RATE_BITS, val);
}

static int zts8032_get_pres_samp_freq(struct zts8032_data *data)
{
	int rc;
	int val;

	rc = regmap_read(data->regmap, ZTS8032_PRS_CFG, &val);
	if (rc < 0)
		return rc;

	return BIT((val & ZTS8032_PRS_RATE_BITS) >> 4);
}

static int zts8032_get_temp_samp_freq(struct zts8032_data *data)
{
	int rc;
	int val;

	rc = regmap_read(data->regmap, ZTS8032_TMP_CFG, &val);
	if (rc < 0)
		return rc;

	return BIT((val & ZTS8032_TMP_RATE_BITS) >> 4);
}

static int zts8032_get_pres_k(struct zts8032_data *data)
{
	int rc = zts8032_get_pres_precision(data);

	if (rc < 0)
		return rc;

	return scale_factors[ilog2(rc)];
}

static int zts8032_get_temp_k(struct zts8032_data *data)
{
	int rc = zts8032_get_temp_precision(data);

	if (rc < 0)
		return rc;

	return scale_factors[ilog2(rc)];
}

static int zts8032_read_pres_raw(struct zts8032_data *data)
{
	int rc;
	int rate;
	int ready;
	int timeout;
	s32 raw;
	u8 val[3];

	if (mutex_lock_interruptible(&data->lock))
		return -EINTR;

	rate = zts8032_get_pres_samp_freq(data);
	timeout = ZTS8032_POLL_TIMEOUT_US(rate);

	/* Poll for sensor readiness; base the timeout upon the sample rate. */
	rc = regmap_read_poll_timeout(data->regmap, ZTS8032_MEAS_CFG, ready,
				      ready & ZTS8032_PRS_RDY,
				      ZTS8032_POLL_SLEEP_US(timeout), timeout);
	if (rc)
		goto done;

	rc = regmap_bulk_read(data->regmap, ZTS8032_PRS_BASE, val, sizeof(val));
	if (rc < 0)
		goto done;

	raw = (val[0] << 16) | (val[1] << 8) | val[2];
	data->pressure_raw = sign_extend32(raw, 23);

done:
	mutex_unlock(&data->lock);
	return rc;
}

/* Called with lock held */
static int zts8032_read_temp_ready(struct zts8032_data *data)
{
	int rc;
	u8 val[3];
	s32 raw;

	rc = regmap_bulk_read(data->regmap, ZTS8032_TMP_BASE, val, sizeof(val));
	if (rc < 0)
		return rc;

	raw = (val[0] << 16) | (val[1] << 8) | val[2];
	data->temp_raw = sign_extend32(raw, 23);

	return 0;
}

static int zts8032_read_temp_raw(struct zts8032_data *data)
{
	int rc;
	int rate;
	int ready;
	int timeout;

	if (mutex_lock_interruptible(&data->lock))
		return -EINTR;

	rate = zts8032_get_temp_samp_freq(data);
	timeout = ZTS8032_POLL_TIMEOUT_US(rate);

	/* Poll for sensor readiness; base the timeout upon the sample rate. */
	rc = regmap_read_poll_timeout(data->regmap, ZTS8032_MEAS_CFG, ready,
				      ready & ZTS8032_TMP_RDY,
				      ZTS8032_POLL_SLEEP_US(timeout), timeout);
	if (rc < 0)
		goto done;

	rc = zts8032_read_temp_ready(data);

done:
	mutex_unlock(&data->lock);
	return rc;
}

static bool zts8032_is_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case ZTS8032_PRS_CFG:
	case ZTS8032_TMP_CFG:
	case ZTS8032_MEAS_CFG:
	case ZTS8032_CFG_REG:
	case ZTS8032_RESET:
	/* No documentation available on the registers below */
	case 0x0e:
	case 0x0f:
	case 0x62:
		return true;
	default:
		return false;
	}
}

static bool zts8032_is_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case ZTS8032_PRS_B0:
	case ZTS8032_PRS_B1:
	case ZTS8032_PRS_B2:
	case ZTS8032_TMP_B0:
	case ZTS8032_TMP_B1:
	case ZTS8032_TMP_B2:
	case ZTS8032_MEAS_CFG:
	case 0x32:	/* No documentation available on this register */
		return true;
	default:
		return false;
	}
}

static int zts8032_write_raw(struct iio_dev *iio,
			    struct iio_chan_spec const *chan, int val,
			    int val2, long mask)
{
	int rc;
	struct zts8032_data *data = iio_priv(iio);

	if (mutex_lock_interruptible(&data->lock))
		return -EINTR;

	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		switch (chan->type) {
		case IIO_PRESSURE:
			rc = zts8032_set_pres_samp_freq(data, val);
			break;

		case IIO_TEMP:
			rc = zts8032_set_temp_samp_freq(data, val);
			break;

		default:
			rc = -EINVAL;
			break;
		}
		break;

	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		switch (chan->type) {
		case IIO_PRESSURE:
			rc = zts8032_set_pres_precision(data, val);
			break;

		case IIO_TEMP:
			rc = zts8032_set_temp_precision(data, val);
			break;

		default:
			rc = -EINVAL;
			break;
		}
		break;

	default:
		rc = -EINVAL;
		break;
	}

	mutex_unlock(&data->lock);
	return rc;
}

static int zts8032_calculate_pressure(struct zts8032_data *data)
{
	int i;
	int rc;
	int t_ready;
	int kpi = zts8032_get_pres_k(data);
	int kti = zts8032_get_temp_k(data);
	s64 rem = 0ULL;
	s64 pressure = 0ULL;
	s64 p;
	s64 t;
	s64 denoms[9];
	s64 nums[9];
	s64 rems[9];
	s64 kp;
	s64 kt;

	if (kpi < 0)
		return kpi;

	if (kti < 0)
		return kti;

	kp = (s64)kpi;
	kt = (s64)kti;

	/* Refresh temp if it's ready, otherwise just use the latest value */
	if (mutex_trylock(&data->lock)) {
		rc = regmap_read(data->regmap, ZTS8032_MEAS_CFG, &t_ready);
		if (rc >= 0 && t_ready & ZTS8032_TMP_RDY)
			zts8032_read_temp_ready(data);

		mutex_unlock(&data->lock);
	}

	p = (s64)data->pressure_raw;
	t = (s64)data->temp_raw;

	/* Section 4.9.1 of the ZTS8032 spec; algebra'd to avoid underflow */
	nums[0] = (s64)data->c00;
	denoms[0] = 1LL;
	nums[1] = p * (s64)data->c10;
	denoms[1] = kp;
	nums[2] = p * p * (s64)data->c20;
	denoms[2] = kp * kp;
	nums[3] = p * p * p * (s64)data->c30;
	denoms[3] = kp * kp * kp;
	nums[4] = p * p * p * p * (s64)data->c40;
	denoms[4] = kp * kp * kp * kp;
	nums[5] = t * (s64)data->c01;
	denoms[5] = kt;
	nums[6] = t * p * (s64)data->c11;
	denoms[6] = kp * kt;
	nums[7] = t * p * p * (s64)data->c21;
	denoms[7] = kp * kp * kt;
	nums[8] = t * p * p * p * (s64)data->c31;
	denoms[8] = kp * kp * kp * kt;

	/* Kernel lacks a div64_s64_rem function; denoms are all positive */
	for (i = 0; i < 9; ++i) {
		u64 irem;

		if (nums[i] < 0LL) {
			pressure -= div64_u64_rem(-nums[i], denoms[i], &irem);
			rems[i] = -irem;
		} else {
			pressure += div64_u64_rem(nums[i], denoms[i], &irem);
			rems[i] = (s64)irem;
		}
	}

	/* Increase precision and calculate the remainder sum */
	for (i = 0; i < 9; ++i)
		rem += div64_s64((s64)rems[i] * 1000000000LL, denoms[i]);

	pressure += div_s64(rem, 1000000000LL);
	if (pressure < 0LL)
		return -ERANGE;

	return (int)min_t(s64, pressure, INT_MAX);
}

static int zts8032_read_pressure(struct zts8032_data *data, int *val, int *val2,
				long mask)
{
	int rc;

	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		rc = zts8032_get_pres_samp_freq(data);
		if (rc < 0)
			return rc;

		*val = rc;
		return IIO_VAL_INT;

	case IIO_CHAN_INFO_PROCESSED:
		rc = zts8032_read_pres_raw(data);
		if (rc)
			return rc;

		rc = zts8032_calculate_pressure(data);
		if (rc < 0)
			return rc;

		*val = rc;
		*val2 = 1000; /* Convert Pa to KPa per IIO ABI */
		return IIO_VAL_FRACTIONAL;

	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		rc = zts8032_get_pres_precision(data);
		if (rc < 0)
			return rc;

		*val = rc;
		return IIO_VAL_INT;

	default:
		return -EINVAL;
	}
}

static int zts8032_calculate_temp(struct zts8032_data *data)
{
	s64 c0;
	s64 t;
	int kt = zts8032_get_temp_k(data);

	if (kt < 0)
		return kt;

	/* Obtain inverse-scaled offset */
	c0 = div_s64((s64)kt * (s64)data->c0, 2);

	/* Add the offset to the unscaled temperature */
	t = c0 + ((s64)data->temp_raw * (s64)data->c1);

	/* Convert to milliCelsius and scale the temperature */
	return (int)div_s64(t * 1000LL, kt);
}

static int zts8032_read_temp(struct zts8032_data *data, int *val, int *val2,
			    long mask)
{
	int rc;

	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		rc = zts8032_get_temp_samp_freq(data);
		if (rc < 0)
			return rc;

		*val = rc;
		return IIO_VAL_INT;

	case IIO_CHAN_INFO_PROCESSED:
		rc = zts8032_read_temp_raw(data);
		if (rc)
			return rc;

		rc = zts8032_calculate_temp(data);
		if (rc < 0)
			return rc;

		*val = rc;
		return IIO_VAL_INT;

	case IIO_CHAN_INFO_OVERSAMPLING_RATIO:
		rc = zts8032_get_temp_precision(data);
		if (rc < 0)
			return rc;

		*val = rc;
		return IIO_VAL_INT;

	default:
		return -EINVAL;
	}
}

static int zts8032_read_raw(struct iio_dev *iio,
			   struct iio_chan_spec const *chan,
			   int *val, int *val2, long mask)
{
	struct zts8032_data *data = iio_priv(iio);

	switch (chan->type) {
	case IIO_PRESSURE:
		return zts8032_read_pressure(data, val, val2, mask);

	case IIO_TEMP:
		return zts8032_read_temp(data, val, val2, mask);

	default:
		return -EINVAL;
	}
}

static void zts8032_reset(void *action_data)
{
	struct zts8032_data *data = action_data;

	regmap_write(data->regmap, ZTS8032_RESET, ZTS8032_RESET_MAGIC);
}

static const struct regmap_config zts8032_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.writeable_reg = zts8032_is_writeable_reg,
	.volatile_reg = zts8032_is_volatile_reg,
	.cache_type = REGCACHE_RBTREE,
	.max_register = 0x62, /* No documentation available on this register */
};

static const struct iio_info zts8032_info = {
	.read_raw = zts8032_read_raw,
	.write_raw = zts8032_write_raw,
};

/*
 * Some verions of chip will read temperatures in the ~60C range when
 * its actually ~20C. This is the manufacturer recommended workaround
 * to correct the issue. The registers used below are undocumented.
 */
static int zts8032_temp_workaround(struct zts8032_data *data)
{
	int rc;
	int reg;

	rc = regmap_read(data->regmap, 0x32, &reg);
	if (rc < 0)
		return rc;

	/*
	 * If bit 1 is set then the device is okay, and the workaround does not
	 * need to be applied
	 */
	if (reg & BIT(1))
		return 0;

	rc = regmap_write(data->regmap, 0x0e, 0xA5);
	if (rc < 0)
		return rc;

	rc = regmap_write(data->regmap, 0x0f, 0x96);
	if (rc < 0)
		return rc;

	rc = regmap_write(data->regmap, 0x62, 0x02);
	if (rc < 0)
		return rc;

	rc = regmap_write(data->regmap, 0x0e, 0x00);
	if (rc < 0)
		return rc;

	return regmap_write(data->regmap, 0x0f, 0x00);
}

static int zts8032_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct zts8032_data *data;
	struct iio_dev *iio;
	int rc, ready;

	iio = devm_iio_device_alloc(&client->dev,  sizeof(*data));
	if (!iio)
		return -ENOMEM;

	data = iio_priv(iio);
	data->client = client;
	mutex_init(&data->lock);

	iio->name = id->name;
	iio->channels = zts8032_channels;
	iio->num_channels = ARRAY_SIZE(zts8032_channels);
	iio->info = &zts8032_info;
	iio->modes = INDIO_DIRECT_MODE;

	data->regmap = devm_regmap_init_i2c(client, &zts8032_regmap_config);
	if (IS_ERR(data->regmap))
		return PTR_ERR(data->regmap);

	/* Register to run the device reset when the device is removed */
	rc = devm_add_action_or_reset(&client->dev, zts8032_reset, data);
	if (rc)
		return rc;

	/*
	 * Set up pressure sensor in single sample, one measurement per second
	 * mode
	 */
	rc = regmap_write(data->regmap, ZTS8032_PRS_CFG, 0);

	/*
	 * Set up external (MEMS) temperature sensor in single sample, one
	 * measurement per second mode
	 */
	rc = regmap_write(data->regmap, ZTS8032_TMP_CFG, ZTS8032_TMP_EXT);
	if (rc < 0)
		return rc;

	/* Temp and pressure shifts are disabled when PRC <= 8 */
	rc = regmap_write_bits(data->regmap, ZTS8032_CFG_REG,
			       ZTS8032_PRS_SHIFT_EN | ZTS8032_TMP_SHIFT_EN, 0);
	if (rc < 0)
		return rc;

	/* MEAS_CFG doesn't update correctly unless first written with 0 */
	rc = regmap_write_bits(data->regmap, ZTS8032_MEAS_CFG,
			       ZTS8032_MEAS_CTRL_BITS, 0);
	if (rc < 0)
		return rc;

	/* Turn on temperature and pressure measurement in the background */
	rc = regmap_write_bits(data->regmap, ZTS8032_MEAS_CFG,
			       ZTS8032_MEAS_CTRL_BITS, ZTS8032_PRS_EN |
			       ZTS8032_TEMP_EN | ZTS8032_BACKGROUND);
	if (rc < 0)
		return rc;

	/*
	 * Calibration coefficients required for reporting temperature.
	 * They are available 40ms after the device has started
	 */
	rc = regmap_read_poll_timeout(data->regmap, ZTS8032_MEAS_CFG, ready,
				      ready & ZTS8032_COEF_RDY, 10000, 40000);
	if (rc < 0)
		return rc;

	rc = zts8032_get_coefs(data);
	if (rc < 0)
		return rc;

	rc = zts8032_temp_workaround(data);
	if (rc < 0)
		return rc;

	rc = devm_iio_device_register(&client->dev, iio);
	if (rc)
		return rc;

	i2c_set_clientdata(client, iio);

	return 0;
}

static int zts8032_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id zts8032_id[] = {
	{ ZTS8032_DEV_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, zts8032_id);

static const struct acpi_device_id zts8032_acpi_match[] = {
	{ "ZTS8032" },
	{}
};
MODULE_DEVICE_TABLE(acpi, zts8032_acpi_match);

static const struct of_device_id zts8032_of_match[] = {
	{ .compatible = "zilltek,zts8032", },
	{ }
};
MODULE_DEVICE_TABLE(of, zts8032_of_match);

static struct i2c_driver zts8032_driver = {
	.driver = {
		.name = ZTS8032_DEV_NAME,
		.acpi_match_table = zts8032_acpi_match,
		.of_match_table = zts8032_of_match,
	},
	.probe = zts8032_probe,
	.remove = zts8032_remove,
	.id_table = zts8032_id,
};
module_i2c_driver(zts8032_driver);

MODULE_AUTHOR("Knight Kuo <knight@zilltek.com>");
MODULE_DESCRIPTION("Zilltek ZTS8032 pressure and temperature sensor");
MODULE_LICENSE("GPL v2");
