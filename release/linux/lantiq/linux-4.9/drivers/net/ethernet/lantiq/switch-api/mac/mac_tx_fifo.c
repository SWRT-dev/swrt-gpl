/******************************************************************************
 *                Copyright (c) 2016, 2017, 2018 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <xgmac.h>
#include <gswss_mac_api.h>
#include <mac_tx_fifo.h>

static int fifo_freeid_get(void *pdev, u8 ttse);
void fifo_entry_timeout(unsigned long data);
static void fifo_update_hw_clrsts(void *pdev,
				  struct mac_fifo_entry *f_entry);

u32 fifo_init(void *pdev)
{
	int i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	memset(pdata->ts_fifo, 0, sizeof(pdata->ts_fifo));

	for (i = START_FIFO; i < MAX_FIFO_ENTRY; i++) {
		pdata->ts_fifo[i].rec_id = i;
#ifdef __KERNEL__
		init_timer(&pdata->ts_fifo[i].timer);
#endif
	}

	return 0;
}

int fifo_entry_add(void *pdev, u8 ttse, u8 ostc, u8 ostpa, u8 cic,
		   u32 ttsl, u32 ttsh)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int id;
	struct mac_fifo_entry *f_entry;

	id = fifo_freeid_get(pdev, ttse);

	if (id == FIFO_FULL)
		return id;

	f_entry = &pdata->ts_fifo[id];

#ifdef __KERNEL__

	if (f_entry->ttse || f_entry->ostc || f_entry->ostpa || f_entry->cic) {
		/* if previously there was a timer running, and timeout */
		del_timer_sync(&f_entry->timer);
	}

#endif
	f_entry->ttse = ttse;
	f_entry->ostc = ostc;
	f_entry->ostpa = ostpa;
	f_entry->cic = cic;
	f_entry->ttsl = ttsl;
	f_entry->ttsh = ttsh;
	f_entry->timeout = FIFO_TIMEOUT_IN_SEC;
	f_entry->is_used = 1;

#ifdef __KERNEL__
	setup_timer(&f_entry->timer, fifo_entry_timeout,
		    (unsigned long)f_entry);
	f_entry->timer.expires = jiffies + (f_entry->timeout * HZ) - 1;
	add_timer(&f_entry->timer);
	f_entry->jiffies = jiffies;
#endif

	gswss_set_txtstamp_fifo(pdev, f_entry);

	return f_entry->rec_id;
}

int fifo_entry_del(void *pdev, u32 rec_id)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_fifo_entry *f_entry = &pdata->ts_fifo[rec_id];

#ifdef __KERNEL__
	/* Delete timer */
	del_timer_sync(&f_entry->timer);
	f_entry->timer.expires = 0;
#endif
	f_entry->ttse = 0;
	f_entry->ostc = 0;
	f_entry->ostpa = 0;
	f_entry->cic = 0;
	f_entry->ttsl = 0;
	f_entry->ttsh = 0;

	/* Fifo available for the next packet */
	f_entry->is_used = 0;

	return 0;
}

static void fifo_update_hw_clrsts(void *pdev,
				  struct mac_fifo_entry *f_entry)
{
	gswss_get_txtstamp_fifo(pdev, f_entry->rec_id, f_entry);

	/* HW has cleared the entry before timeout
	 * Delete the timeout and the entry
	 */
	if (f_entry->ostc == 0 &&
	    f_entry->ostpa == 0 &&
	    f_entry->ttse == 0 &&
	    f_entry->cic == 0 &&
	    f_entry->ttsl == 0 &&
	    f_entry->ttsh == 0) {
		fifo_entry_del(pdev, f_entry->rec_id);
	}
}

void fifo_isr_handler(void *pdev)
{
	int i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	mac_printf("Fifo isr handler\n");

	for (i = START_FIFO; i < MAX_FIFO_ENTRY; i++) {
		if (pdata->ts_fifo[i].is_used)
			fifo_update_hw_clrsts(pdev, &pdata->ts_fifo[i]);
	}
}

u32 timer_in_sec(u32 jiffies)
{
#ifdef __KERNEL__
	return ((jiffies + HZ / 2) / HZ);
#else
	return 0;
#endif
}

int print_fifo(void *pdev)
{
	int i = 0;
	struct mac_fifo_entry f_entry;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	mac_printf("%12s %12s %12s %12s %12s %12s %12s %12s\n",
		   "REC_ID", "TTSE", "OSTC", "OSTPA", "CIC", "TTSL", "TTSH", "TIMEOUT");

	for (i = START_FIFO; i < MAX_FIFO_ENTRY; i++) {
		gswss_get_txtstamp_fifo(pdev, i, &f_entry);

		if (!pdata->ts_fifo[i].is_used)
			continue;

#ifdef __KERNEL__
		mac_printf("%12d %12d %12d %12d %12d %12d %12d %12d\n",
			   f_entry.rec_id, f_entry.ttse, f_entry.ostc,
			   f_entry.ostpa, f_entry.cic,
			   f_entry.ttsl, f_entry.ttsh,
			   (pdata->ts_fifo[i].timer.expires ?
			    (pdata->ts_fifo[i].timeout -
			     (timer_in_sec(jiffies) -
			      timer_in_sec(pdata->ts_fifo[i].jiffies))) : 0));
#else
		mac_printf("%12d %12d %12d %12d %12d %12d %12d %12d\n",
			   f_entry.rec_id, f_entry.ttse, f_entry.ostc,
			   f_entry.ostpa, f_entry.cic,
			   f_entry.ttsl, f_entry.ttsh,
			   pdata->ts_fifo[i].jiffies);
#endif
	}

	return 0;
}

static int fifo_freeid_get(void *pdev, u8 ttse)
{
	int i = FIFO_FULL;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	for (i = START_FIFO; i < MAX_FIFO_ENTRY; i++) {
		if (!pdata->ts_fifo[i].is_used) {
			return pdata->ts_fifo[i].rec_id;
		} else {
			fifo_update_hw_clrsts(pdev, &pdata->ts_fifo[i]);
		}
	}

	return FIFO_FULL;
}

/* Timeout handler, Called when Fifo entry timeout
 * This happens when packet is dropped by cqem or some other reason packet lost
 */
void fifo_entry_timeout(unsigned long data)
{
	struct mac_fifo_entry *f_entry  = (struct mac_fifo_entry *)data;

	//mac_printf("RecordID in timeout %d \n", f_entry->rec_id);

	/* Mark this Fifo entry can be used again */
	f_entry->is_used = 0;
}

