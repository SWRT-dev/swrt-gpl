#ifndef _MMC_HAL_H_
#define _MMC_HAL_H_

#include "mmc_types.h"
#include "mmc_core.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int  msdc_init(struct mmc_host *host, int id);
extern int  msdc_cmd(struct mmc_host *host, struct mmc_command *cmd);
extern int  msdc_card_protected(struct mmc_host *host);
extern int  msdc_card_avail(struct mmc_host *host);
extern void msdc_power(struct mmc_host *host, u8 mode);
extern void msdc_config_bus(struct mmc_host *host, u32 width);
extern void msdc_config_clock(struct mmc_host *host, int state, u32 hz);
extern void msdc_set_timeout(struct mmc_host *host, u32 ns, u32 clks);
extern int  msdc_switch_volt(struct mmc_host *host, int volt);
extern int  msdc_tune_uhs1(struct mmc_host *host, struct mmc_card *card);
extern int  msdc_pio_read(struct mmc_host *host, u32 *ptr, u32 size);
extern void msdc_set_blklen(struct mmc_host *host, u32 blklen);
extern void msdc_set_blknum(struct mmc_host *host, u32 blknum);

extern int  msdc_emmc_boot_reset(struct mmc_host *host, int reset);
extern int  msdc_emmc_boot_start(struct mmc_host *host, u32 hz, int ddr, int mode, int ackdis);
extern int  msdc_emmc_boot_read(struct mmc_host *host, u32 size, u32 *to);
extern int  msdc_emmc_boot_stop(struct mmc_host *host, int mode);

/* unused in download agent */
extern void msdc_sdioirq(struct mmc_host *host, int enable);
extern void msdc_sdioirq_register(struct mmc_host *host, hw_irq_handler_t handler);
extern void msdc_set_blksz(struct mmc_host *host, u32 sz); /* FIXME */
extern int  msdc_fifo_read(struct mmc_host *host, u32 *ptr, u32 size);
extern int  msdc_fifo_write(struct mmc_host *host, u32 *ptr, u32 size);    

#ifdef __cplusplus
}
#endif

#endif /* _MMC_HAL_H_ */

