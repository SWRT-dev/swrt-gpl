// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/init.h>


#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/delay.h>

#include <linux/regmap.h>
#include <linux/spi/spi.h>


#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/tlv.h>

#include "proslic_sys.h"
#include "si3218x.h"
#include "inc/proslic.h"
#include "timer.h"

#include "spi.h"

#ifdef SI3218X
#include "inc/si3218x.h"
#include "inc/si3218x_intf.h"
#endif

#define NUMBER_OF_DEVICES 1
#if defined(SI3218X)
#define CHAN_PER_DEVICE 1
#define NUMBER_OF_CHAN (NUMBER_OF_DEVICES*CHAN_PER_DEVICE)
#define NUMBER_OF_PROSLIC (NUMBER_OF_CHAN)
#define PROSLIC_DEVICE_TYPE SI3218X_TYPE
#endif

static int slic_init = 0;

ctrl_S spiGciObj; /* User¡¦s control interface object */
systemTimer_S timerObj; /* User¡¦s timer object */
chanState ports[NUMBER_OF_CHAN]; /* User¡¦s channel object, which has
				  ** a member defined as
				  ** proslicChanType_ptr ProObj;
				 */
/* Define ProSLIC control interface object */
controlInterfaceType *ProHWIntf;
/* Define array of ProSLIC device objects */
ProslicDeviceType *ProSLICDevices[NUMBER_OF_PROSLIC];
/* Define array of ProSLIC channel object pointers */
proslicChanType_ptr arrayOfProslicChans[NUMBER_OF_CHAN];

static int ProSLIC_HWInit(void)
{
	int32 i, result= 0;

	printk ("%s()\n",__FUNCTION__);
	/*
	** Step 1: (required)
	** Create ProSLIC Control Interface Object
	*/
	ProSLIC_createControlInterface(&ProHWIntf);

	/*
	** Step 2: (required)
	** Create ProSLIC Device Objects
	*/
	for(i=0;i<NUMBER_OF_PROSLIC;i++)
	{
		ProSLIC_createDevice(&(ProSLICDevices[i]));
	}

	/*
	** Step 3: (required)
	** Create and initialize ProSLIC channel objects
	** Also initialize array pointers to user¡¦s proslic channel object
	** members to simplify initialization process.
	*/
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_createChannel(&(ports[i].ProObj));
		ProSLIC_SWInitChan(ports[i].ProObj,i,PROSLIC_DEVICE_TYPE,
		ProSLICDevices[i/CHAN_PER_DEVICE],ProHWIntf);
		arrayOfProslicChans[i] = ports[i].ProObj;
		ProSLIC_setSWDebugMode(ports[i].ProObj,TRUE); /* optional */
	}

	/*
	** Step 4: (required)
	** Establish linkage between host objects/functions and
	** ProSLIC API
	*/
	ProSLIC_setControlInterfaceCtrlObj (ProHWIntf, &spiGciObj);
	ProSLIC_setControlInterfaceReset (ProHWIntf, ctrl_ResetWrapper);
	ProSLIC_setControlInterfaceWriteRegister (ProHWIntf, ctrl_WriteRegisterWrapper);
	ProSLIC_setControlInterfaceReadRegister (ProHWIntf, ctrl_ReadRegisterWrapper);
	ProSLIC_setControlInterfaceWriteRAM (ProHWIntf, ctrl_WriteRAMWrapper);
	ProSLIC_setControlInterfaceReadRAM (ProHWIntf, ctrl_ReadRAMWrapper);
	ProSLIC_setControlInterfaceTimerObj (ProHWIntf, &timerObj);
	ProSLIC_setControlInterfaceDelay (ProHWIntf, time_DelayWrapper);
	ProSLIC_setControlInterfaceTimeElapsed (ProHWIntf, time_TimeElapsedWrapper);
	ProSLIC_setControlInterfaceGetTime (ProHWIntf, time_GetTimeWrapper);
	ProSLIC_setControlInterfaceSemaphore (ProHWIntf, NULL);

	/*
	** Step 5: (system dependent)
	** Assert hardware Reset ¡V ensure VDD, PCLK, and FSYNC are present and stable
	** before releasing reset
	*/
	ProSLIC_Reset(ports[0].ProObj);

	/*
	** Step 6: (required)
	** Initialize device (loading of general parameters, calibrations,
	** dc-dc powerup, etc.)
	*/
	ProSLIC_Init(arrayOfProslicChans,NUMBER_OF_CHAN);
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		if(arrayOfProslicChans[i]->error!=0)
		{
			printk("ProSLIC_Init[%d] ERR=%d\n",i,arrayOfProslicChans[i]->error);
			return 0;
		}
	}


	/*
	** Step 7: (design dependent)
	** Execute longitudinal balance calibration
	** or reload coefficients from factory LB cal
	**
	** Note: all batteries should be up and stable prior to
	** executing the lb cal
	*/
	ProSLIC_LBCal(arrayOfProslicChans,NUMBER_OF_CHAN);
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_GetLBCalResultPacked(arrayOfProslicChans[i], &result);
		printk("LBCal=0x%08X\n",result);
	}

	/*
	** Step 8: (design dependent)
	** Load custom configuration presets(generated using
	** ProSLIC API Config Tool)
	*/
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_PCMTimeSlotSetup(ports[i].ProObj,0,0);
		ProSLIC_DCFeedSetup(ports[i].ProObj,DCFEED_48V_20MA);
		ProSLIC_RingSetup(ports[i].ProObj,RING_F20_45VRMS_0VDC_LPR);
		ProSLIC_PCMSetup(ports[i].ProObj,PCM_16LIN_WB); /* PCM_DEFAULT_CONFIG */
		ProSLIC_ZsynthSetup(ports[i].ProObj,ZSYN_600_0_0_30_0);
		ProSLIC_ToneGenSetup(ports[i].ProObj,TONEGEN_FCC_DIAL);
	}

	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_PCMStart(ports[i].ProObj);
	}
	for(i=0;i<NUMBER_OF_CHAN;i++)
	{
		ProSLIC_SetLinefeedStatus(ports[i].ProObj,LF_FWD_ACTIVE);
	}

	/*
	** Step 9: (required)
	** SLIC settings - set TX_START and RX_START to 1 and
	** disable Free-Run mode.
	*/
	ProSLIC_WriteReg(ports[0].ProObj, 12, 1);
	ProSLIC_WriteReg(ports[0].ProObj, 14, 1);
	ProSLIC_WriteReg(ports[0].ProObj, 47, 1);

	return 1;
}

static int si3218x_component_probe(struct snd_soc_component *component)
{
	dev_info(component->dev, "%s\n", __func__);
	return 0;
}

static void si3218x_component_remove(struct snd_soc_component *component)
{
	struct si3218x_chip *chip = snd_soc_component_get_drvdata(component);

	dev_info(component->dev, "%s\n", __func__);

	chip->component = NULL;
}

static const struct snd_soc_dapm_widget si3218x_component_dapm_widgets[] = {
	SND_SOC_DAPM_INPUT("VINP"),
	SND_SOC_DAPM_OUTPUT("VOUTP"),
};

static const struct snd_soc_dapm_route si3218x_component_dapm_routes[] = {
	{ "VOUTP", NULL, "aif_playback"},
	{ "aif_capture", NULL, "VINP"},
};

static const char * const slic_control_str[] = {
	"off", "on"
};

static const struct soc_enum si3218x_slic_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(slic_control_str),
			    slic_control_str),
};

static int si3218x_init_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	ucontrol->value.integer.value[0] = slic_init;

	return 0;
}

static int si3218x_init_set(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	if (ucontrol->value.integer.value[0]) {
		ProSLIC_HWInit();
		slic_init = 1;
	}

	return 0;
}

static int si3218x_ring_get(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	if (!slic_init)
		return 0;

	ucontrol->value.integer.value[0] =
		(ProSLIC_ReadReg(ports[0].ProObj,30) == 4) ? 1 : 0;
	return 0;
}

static int si3218x_ring_set(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	if (!slic_init)
		return 0;

	if (ucontrol->value.integer.value[0])
		ProSLIC_WriteReg(ports[0].ProObj, 30, 4);
	else
		ProSLIC_WriteReg(ports[0].ProObj, 30, 1);

	return 0;
}

static const struct snd_kcontrol_new si3218x_component_snd_controls[] = {
	SOC_ENUM_EXT("proslic_init", si3218x_slic_enum[0],
				     si3218x_init_get, si3218x_init_set),
	SOC_ENUM_EXT("proslic_ring", si3218x_slic_enum[0],
				     si3218x_ring_get, si3218x_ring_set),
};

static const struct snd_soc_component_driver si3218x_component_driver = {
	.probe = si3218x_component_probe,
	.remove = si3218x_component_remove,

	.controls = si3218x_component_snd_controls,
	.num_controls = ARRAY_SIZE(si3218x_component_snd_controls),
	.dapm_widgets = si3218x_component_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(si3218x_component_dapm_widgets),
	.dapm_routes = si3218x_component_dapm_routes,
	.num_dapm_routes = ARRAY_SIZE(si3218x_component_dapm_routes),

	.idle_bias_on = false,
};

static int si3218x_component_aif_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *hw_params, struct snd_soc_dai *dai)
{
	int word_len = params_physical_width(hw_params);
	int aud_bit = params_width(hw_params);

	dev_dbg(dai->dev, "format: 0x%08x\n", params_format(hw_params));
	dev_dbg(dai->dev, "rate: 0x%08x\n", params_rate(hw_params));
	dev_dbg(dai->dev, "word_len: %d, aud_bit: %d\n", word_len, aud_bit);
	if (word_len > 32 || word_len < 16) {
		dev_err(dai->dev, "not supported word length\n");
		return -ENOTSUPP;
	}

	dev_dbg(dai->dev, "%s: --\n", __func__);
	return 0;
}

static const struct snd_soc_dai_ops si3218x_component_aif_ops = {
	.hw_params = si3218x_component_aif_hw_params,
};

#define STUB_RATES	SNDRV_PCM_RATE_8000_192000
#define STUB_FORMATS	(SNDRV_PCM_FMTBIT_S16_LE | \
			SNDRV_PCM_FMTBIT_U16_LE | \
			SNDRV_PCM_FMTBIT_S24_LE | \
			SNDRV_PCM_FMTBIT_U24_LE | \
			SNDRV_PCM_FMTBIT_S32_LE | \
			SNDRV_PCM_FMTBIT_U32_LE)

static struct snd_soc_dai_driver si3218x_codec_dai = {
	.name = "proslic_spi-aif",
	.playback = {
		.stream_name	= "aif_playback",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates		= STUB_RATES,
		.formats	= STUB_FORMATS,
	},
	.capture = {
		.stream_name	= "aif_capture",
		.channels_min	= 1,
		.channels_max	= 2,
		.rates = STUB_RATES,
		.formats = STUB_FORMATS,
	},
	/* dai properties */
	.symmetric_rates = 1,
	.symmetric_channels = 1,
	.symmetric_samplebits = 1,
	/* dai operations */
	.ops = &si3218x_component_aif_ops,
};

int si3218x_spi_probe(struct spi_device *spi, struct spi_driver *spi_drv)
{
	int ret;

	printk(KERN_INFO "PROSLIC si3218x_spi_probe\n");

	ret = proslic_spi_probe(spi,spi_drv);

	return snd_soc_register_component(&spi->dev, &si3218x_component_driver,
				      &si3218x_codec_dai, 1);
}
EXPORT_SYMBOL(si3218x_spi_probe);

int si3218x_spi_remove(struct spi_device *spi)
{
	proslic_spi_remove(spi);
	snd_soc_unregister_component(&spi->dev);

	return 0;
}
EXPORT_SYMBOL(si3218x_spi_remove);
