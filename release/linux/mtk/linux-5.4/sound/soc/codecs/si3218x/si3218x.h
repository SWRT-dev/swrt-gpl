/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/spi/spi.h>
#include "inc/proslic.h"
#include "timer.h"
#include "spi.h"

#if 1
#include "inc/si3218x.h"
#include "inc/si3218x_intf.h"
#include "config_inc/si3218x_LCCB_constants.h"
#endif

typedef struct chanStatus chanState; //forward declaration

typedef void (*procState) (chanState *pState, ProslicInt eInput);

/*
** structure to hold state information for pbx demo
*/
struct chanStatus {
        proslicChanType *ProObj;
        SiVoiceChanType_ptr VoiceObj;
        timeStamp onHookTime;
        timeStamp offHookTime;
        procState currentState;
        uInt16 digitCount;
        uInt8 digits[20];
        uInt8 ringCount;
        uInt16 connectionWith;
        uInt16 powerAlarmCount;
        pulseDialType pulseDialData;
        BOOLEAN eventEnable;
}  __attribute__((aligned(32)));

struct si3218x_chip {
	struct spi_device *spi;
	struct device *dev;
	struct snd_soc_component *component;
};

/* Export function */
int si3218x_spi_probe(struct spi_device *spi, struct spi_driver *spi_drv);
int si3218x_spi_remove(struct spi_device *spi);
