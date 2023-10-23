/******************************************************************************
 *                Copyright (c) 2016, 2017, 2018, 2019, 2020  Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/
/* =========================================================================
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * The Synopsys DWC ETHER XGMAC Software Driver and documentation (hereinafter
 * "Software") is an unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto.  Permission is hereby granted,
 * free of charge, to any person obtaining a copy of this software annotated
 * with this license and the Software, to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * =========================================================================
 */

#include <xgmac.h>
#include <xgmac_ptp.h>

#define AUX_TRIG_0 1
#define AUX_TRIG_1 2

static int xgmac_adj_freq(struct ptp_clock_info *ptp, s32 ppb);
static int xgmac_adj_time(struct ptp_clock_info *ptp, s64 delta);
static int xgmac_get_time(struct ptp_clock_info *ptp,
			  struct timespec64 *ts);
static int xgmac_set_time(struct ptp_clock_info *ptp,
			  const struct timespec64 *ts);
static void *parse_ptp_packet(struct sk_buff *skb,
			      u16 *eth_type,
			      u8 *msg_type);
static void xgmac_get_rx_tstamp(struct mac_prv_data *pdata,
				struct sk_buff *skb);
static int xgmac_ptp_register(void *pdev);
static void xgmac_extts_isr_handler(struct mac_prv_data *pdata,
				    u32 tstamp_sts);

int xgmac_config_timer_reg(void *pdev, u32 mac_tscr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct timespec64 now;
	struct mac_ops *hw_if = &pdata->ops;
	u64 temp = 0;

	if (!pdata->systime_initialized) {
		/* Check Timestamp Enabled/Disabled
		 * Exit if Disabled, No need to Intialize the Timer
		 */
		if (!MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA))
			return 0;

		/* program Sub Second Increment Reg */
		hw_if->config_subsec_inc(pdev, pdata->ptp_clk);

		/* Calculate the def addend:
		 * addend = 2^32 / (PTP ref clock / CLOCK_UPDATE_FREQ)
		 *        = (2^32 * CLOCK_UPDATE_FREQ) / PTP ref clock
		 */
		temp = (u64)((CLOCK_UPDATE_FREQ * 1000000ULL) << 32);
		pdata->def_addend = div_u64(temp, pdata->ptp_clk);

		hw_if->config_addend(pdev, pdata->def_addend);

		/* initialize system time */
		getnstimeofday64(&now);
		hw_if->init_systime(pdev, now.tv_sec, now.tv_nsec);
		pdata->systime_initialized = 1;
	}

	return 0;
}

/* API to adjust the frequency of hardware clock.
 * delta - desired period change.
 */
static int xgmac_adj_freq(struct ptp_clock_info *ptp, s32 ppb)
{
	struct mac_prv_data *pdata =
		container_of(ptp, struct mac_prv_data, ptp_clk_info);
	struct mac_ops *hw_if = &pdata->ops;
	u64 adj, diff;
	u32 addend;

	pr_debug("Calling adjust_freq: %d\n", ppb);

	/* Frequency adjustment is feq_delta = ppb / 1.000.000.000
	 * addend = def_addend / ( 1 - ppb/1.000.000.000)
	 * So addend in integer arithmetic becomes
	 * addend = (def_addend * 1.000.000.000) / (1.000.000.000 - ppb)
	 */
	adj = ((u64)pdata->def_addend * NSEC_TO_SEC);
	diff = (NSEC_TO_SEC - ppb);

	addend = div_u64(adj, diff);

	spin_lock_bh(&pdata->ptp_lock);

	hw_if->config_addend(hw_if, addend);

	spin_unlock_bh(&pdata->ptp_lock);

	pr_debug("Adjust_freq: Done\n");

	return 0;
}

/* This function is used to shift/adjust the time of the
 * hardware clock.
 * delta - desired change in nanoseconds.
 */
static int xgmac_adj_time(struct ptp_clock_info *ptp, s64 delta)
{
	struct mac_prv_data *pdata =
		container_of(ptp, struct mac_prv_data, ptp_clk_info);
	struct mac_ops *hw_if = &pdata->ops;

	u32 sec, nsec;
	u32 quotient, reminder;
	int neg_adj = 0;

	pr_debug("Calling adjust_time: %lld\n", delta);

	if (delta < 0) {
		neg_adj = 1;
		delta = -delta;
	}

	quotient = div_u64_rem(delta, NSEC_TO_SEC, &reminder);
	sec = quotient;
	nsec = reminder;

	spin_lock_bh(&pdata->ptp_lock);

	hw_if->adjust_systime(hw_if, sec, nsec, neg_adj,
			      pdata->one_nsec_accuracy);

	spin_unlock_bh(&pdata->ptp_lock);

	pr_debug("Adjust_time: Done\n");

	return 0;
}

/* This function is used to read the current time from the
 * hardware clock.
 */
static int xgmac_get_time(struct ptp_clock_info *ptp,
			  struct timespec64 *ts)
{
	struct mac_prv_data *pdata =
		container_of(ptp, struct mac_prv_data, ptp_clk_info);
	struct mac_ops *hw_if = &pdata->ops;
	u64 ns;
	u32 reminder;

	spin_lock_bh(&pdata->ptp_lock);

	ns = hw_if->get_systime(hw_if);

	spin_unlock_bh(&pdata->ptp_lock);

	ts->tv_sec = div_u64_rem(ns, NSEC_TO_SEC, &reminder);
	ts->tv_nsec = reminder;

	pr_debug("get_time: ts->tv_sec  = %ld,ts->tv_nsec = %ld\n",
		 (long int)ts->tv_sec, (long int)ts->tv_nsec);

	return 0;
}

/* This function is used to set the current time on the
 * hardware clock.
 */
static int xgmac_set_time(struct ptp_clock_info *ptp,
			  const struct timespec64 *ts)
{
	struct mac_prv_data *pdata =
		container_of(ptp, struct mac_prv_data, ptp_clk_info);
	struct mac_ops *hw_if = &pdata->ops;

	pr_debug("set_time: ts->tv_sec  = %lld, ts->tv_nsec = %lld\n",
		 (u64)ts->tv_sec, (u64)ts->tv_nsec);

	spin_lock_bh(&pdata->ptp_lock);

	hw_if->init_systime(hw_if, ts->tv_sec, ts->tv_nsec);

	spin_unlock_bh(&pdata->ptp_lock);

	return 0;
}

/*  Parse PTP packet to find out whether flag is 1 step or 2 step.
 *  The PTP header can be found in an IPv4, IPv6 or in an IEEE802.3
 *  ethernet frame. The function returns the position of the PTP packet
 *  or NULL, if no PTP found.
 */
static void *parse_ptp_packet(struct sk_buff *skb,
			      u16 *eth_type,
			      u8 *msg_type)
{
	struct iphdr *iph;
	struct udphdr *udph;
	struct ipv6hdr *ipv6h;
	void *pos = skb->data + ETH_ALEN + ETH_ALEN;
	u8 *ptp_loc = NULL;

	*eth_type = *((u16 *)pos);

	/* Check if any VLAN tag is there */
	while (1) {
		if (*eth_type == ETH_P_8021Q) {
			pos += 4;
			*eth_type = *((u16 *)pos);
		} else {
			break;
		}
	}

	/* set pos after ethertype */
	pos += 2;

	switch (*eth_type) {
	/* Transport of PTP over Ethernet */
	case ETH_P_1588:
		ptp_loc = pos;
		pr_debug("Transport of PTP over Ethernet header\n");
		break;

	/* Transport of PTP over IPv4 */
	case ETH_P_IP:
		iph = (struct iphdr *)pos;

		if (ntohs(iph->protocol) != IPPROTO_UDP)
			ptp_loc = NULL;

		pos += iph->ihl * 4;
		udph = (struct udphdr *)pos;

		/* Update the Udp header checksum with 0 */
		udph->check = 0;

		/* check the destination port address
		 * ( 319 (0x013F) = PTP event port )
		 */
		if (ntohs(udph->dest) != 319) {
			pr_info(" Transport of PTP over IPv4 Error\n");
			ptp_loc = NULL;
		} else {
			pr_debug("Transport of PTP over IPv4 header\n");
			ptp_loc = pos + sizeof(struct udphdr);
		}

		break;

	/* Transport of PTP over IPv6 */
	case ETH_P_IPV6:
		ipv6h = (struct ipv6hdr *)pos;

		if (ntohs(ipv6h->nexthdr) != IPPROTO_UDP)
			ptp_loc = NULL;

		pos += sizeof(struct ipv6hdr);
		udph = (struct udphdr *)pos;

		/* check the destination port address
		 * ( 319 (0x013F) = PTP event port )
		 */
		if (ntohs(udph->dest) != 319) {
			pr_info(" Transport of PTP over IPv6 Error\n");
			ptp_loc = NULL;
		} else {
			pr_debug("Transport of PTP over IPv6 header\n");
			ptp_loc = pos + sizeof(struct udphdr);
		}

		break;

	default:
		ptp_loc = NULL;
		break;
	}

	if (!ptp_loc) {
		pr_info("PTP header is not found in the packet, Please check\n");
	} else {
		*msg_type = *((u8 *)(ptp_loc + PTP_OFFS_MSG_TYPE)) & 0xf;

		if ((*msg_type == PTP_MSGTYPE_SYNC) ||
		    (*msg_type == PTP_MSGTYPE_DELREQ) ||
		    (*msg_type == PTP_MSGTYPE_PDELREQ) ||
		    (*msg_type == PTP_MSGTYPE_PDELRESP)) {
			ptp_loc = pos;
		} else {
			pr_info("Error: Unknown PTP Message\n");
			ptp_loc = NULL;
		}
	}

	return ptp_loc;
}

static void tx_hwtstamp(struct mac_prv_data *pdata,
			struct skb_shared_hwtstamps *shhwtstamp)
{
	if (!pdata->ptp_tx_skb)
		return;

	/* Marking Tx Timestamp complete */
	if (!shhwtstamp->hwtstamp.tv64)
		shhwtstamp->hwtstamp = ktime_get_real();

	skb_complete_tx_timestamp(pdata->ptp_tx_skb, shhwtstamp);
	pdata->ptp_tx_skb = NULL;
}

/* =========================== TX TIMESTAMP =========================== */

/*  ptpd sends down 1-Step/2-Step sync packet on the event socket (i.e, one
 *  for which SO_TIMESTAMPING socket option is set).
 *  Network stack sets the SKBTX_HW_TSTAMP in skb since socket is
 *  marked for SO_TIMESTAMPING.
 */
int xgmac_tx_hwts(void *pdev, struct sk_buff *skb)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int rec_id = 0;
	u16 ethtype;
	u8 *ptp_loc = NULL;
	u8 msg_type = PTP_MSGTYPE_INVALID;

	/* check for hw tstamping */
	if (pdata->hw_feat.ts_src && pdata->ptp_flgs.ptp_tx_en &&
	    (skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP)) {
		/* declare that device is doing timestamping */
		skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;

		/* Currently supporting only Master Mode */
		if (IS_2STEP(pdata)) {
			mac_dbg("2-Step\n");

			if (pdata->ptp_tx_skb)
				return -1;

			pdata->ptp_tx_skb = skb_get(skb);

			/* PTP Sync if we are Master TTSE=1 OSTC=0, OSTPA=0 */
			rec_id = fifo_entry_add(pdev, 1, 0, 0, 0, 0, 0,
						&pdata->ptp_tx_skb);
		}

		if (IS_1STEP(pdata)) {
			mac_dbg("1-Step\n");
			ptp_loc = parse_ptp_packet(skb, &ethtype, &msg_type);

			if (!ptp_loc) {
				mac_printf("Parsing PTP packet Error\n");
				return -1;
			}

			/* PTP Sync if we are Master TTSE=0 OSTC=1, OSTPA=0 */
			rec_id = fifo_entry_add(pdev, 0, 1, 0, 0, 0, 0, NULL);
			/* SNAPTYPESEL=0 TSMSTREN=1 TSEVNTEN = 1 */
			xgmac_ptp_txtstamp_mode(pdev, 0, 1, 1);
		}

		if (rec_id == FIFO_FULL) {
			mac_printf("Fifo is Full\n");
			/* if -1, CPU should drop the packet */
			return -1;
		}
	}

	return rec_id;
}

/*  This API will get executed by workqueue only for 2 step timestamp
 *  Get the TX'ted Timestamp and pass it to upper app
 *  and free the skb
 */
int xgmac_ptp_tx_work(struct work_struct *work)
{
	struct mac_prv_data *pdata =
		container_of(work, struct mac_prv_data, ptp_tx_work);
	u64 tstamp;
	struct skb_shared_hwtstamps shhwtstamp;

	/* check tx tstamp status, if have, read the timestamp and sent to
	 * stack, otherwise reschedule later
	 * Get Tx Timestamp need to be done first, since it will clear the
	 * Xgmac Stored interrupt
	 */
	tstamp = pdata->ops.get_tx_tstamp(&pdata->ops);

	if (!pdata->ptp_tx_skb) {
		pr_debug("No PTP packet transferred out from device\n");
		return -1;
	}

	if (!tstamp) {
		pr_info("Tx tstamp is not captured or ignored for this pkt\n");
		goto schedule_later;
	}

	memset(&shhwtstamp, 0, sizeof(struct skb_shared_hwtstamps));
	shhwtstamp.hwtstamp = ns_to_ktime(tstamp);

	/* pass tstamp from HW to network stack fr 2 step */
	tx_hwtstamp(pdata, &shhwtstamp);

	return 1;

schedule_later:

	/* reschedule to check later */
	schedule_work(&pdata->ptp_tx_work);

	return 1;
}

/* =========================== RX TIMESTAMP =========================== */
/* This API will be called by Datapath library before calling netif_rx()
 */
int xgmac_rx_hwts(void *pdev, struct sk_buff *skb)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* No Rx packet will have timestamp added to it */
	if (pdata->ptp_flgs.ptp_rx_en == 0)
		return -1;

	/* All packets have timestamp appended to it, retrieve all
	 * timestamp and send to App
	 */
	if (pdata->ptp_flgs.ptp_rx_en & PTP_RX_EN_ALL)
		xgmac_get_rx_tstamp(pdata, skb);

	/* No other Rx filter currently supported in driver */
	return 0;
}

/* Update the received timestamp in skbhwtstamp
 * which will be used by PTP app
 */
static void xgmac_get_rx_tstamp(struct mac_prv_data *pdata,
				struct sk_buff *skb)
{
	__be64 regval;
	u32 ts_hdr_len = 8;
	u32 copy_hdr_len = 8;
	struct skb_shared_hwtstamps *shhwtstamp = NULL;
	u64 ns;

	/* External clk_src only have 8 bytes as tstamp header
	 * Internal clk_src have 10 bytes as tstamp header
	 * (8 bytes tstamp + 2 bytes Year)
	 * Get the register setting for clk src
	 */
	if (pdata->hw_feat.ts_src == 2) {
		ts_hdr_len = 8;
		copy_hdr_len = 8;
	} else if (pdata->hw_feat.ts_src == 1 || pdata->hw_feat.ts_src == 3) {
		/* Since 2 bytes is fixed as 0, copy only 8 bytes */
		copy_hdr_len = 8;
		/* Skb timestamp stripping should be still 10 bytes */
		ts_hdr_len = 10;
	}

	/* copy the bits out of the skb, and then trim the skb length */
	skb_copy_bits(skb, skb->len - copy_hdr_len, &regval, copy_hdr_len);
	__pskb_trim(skb, skb->len - ts_hdr_len);


	/* The timestamp is recorded in below order from HW,
	 * and is stored at the end of the packet.
	 *
	 * DWORD: Year(2 byte)  Sec(4 Byte)	Nsec(4 Byte)
	 * Field: 00  		SYSTIMH    	SYSTIML
	 */

	regval = be64_to_cpu(regval);

	ns = ((((regval & 0xffffffff00000000) >> 32) * NSEC_TO_SEC) +
	      (regval & 0x00000000ffffffff));

	shhwtstamp = skb_hwtstamps(skb);
	memset(shhwtstamp, 0, sizeof(struct skb_shared_hwtstamps));
	shhwtstamp->hwtstamp = ns_to_ktime(ns);
}

/* Configuring the HW Timestamping feature
 * This api will be called from the IOCTL SIOCSHWTSTAMP
 */
int xgmac_set_hwts(void *pdev, struct ifreq *ifr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct hwtstamp_config config;
	int err;
	struct mac_ops *hw_if = &pdata->ops;

	if (copy_from_user(&config, ifr->ifr_data, sizeof(config)))
		return -EFAULT;

	err = hw_if->config_hw_time_stamping(pdev,
					     config.tx_type,
					     config.rx_filter);

	if (err)
		return 0;

	/* save these settings for future reference */
	memcpy(&pdata->tstamp_config, &config, sizeof(pdata->tstamp_config));

	return 1;
}

/* Configuring the HW Timestamping feature
 * This api will be called from the IOCTL SIOCGHWTSTAMP
 */
int xgmac_get_hwts(void *pdev, struct ifreq *ifr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	return copy_to_user(ifr->ifr_data, &pdata->tstamp_config,
			    sizeof(pdata->tstamp_config)) ? -EFAULT : 0;
}

int xgmac_ptp_isr_hdlr(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 tstamp_sts;
	struct mac_ops *hw_if = &pdata->ops;
	u64 tstamp;
	int ret = -1;

	/* Clear/Acknowledge interrupt by reading */
	tstamp_sts = XGMAC_RGRD(pdata, MAC_TSTAMP_STSR);

	/* Timestamp stored interrupt */
	if (tstamp_sts & 0x8000) {
		if (IS_2STEP(pdata))
			xgmac_ptp_tx_work(&pdata->ptp_tx_work);

		if (IS_1STEP(pdata))
			tstamp = hw_if->get_tx_tstamp(pdev);

		ret = 0;
	}

	/* Auxilairy Timestamp stored interrupt */
	if (tstamp_sts & 0x4) {
		if (XGMAC_RGRD(pdata, MAC_AUX_CTRL) & 0x30)
			xgmac_extts_isr_handler(pdata, tstamp_sts);
	}

	return ret;
}

static int xgmac_extts_enable(struct ptp_clock_info *ptp,
			      struct ptp_clock_request *rq, int on)
{
	struct mac_prv_data *pdata =
		container_of(ptp, struct mac_prv_data, ptp_clk_info);
	int i;
	u32 aux_ctrl;

	switch (rq->type) {
	case PTP_CLK_REQ_EXTTS:
		i = rq->extts.index;
		if (i < N_EXT_TS) {
			mac_dbg("ATSEN%d enabled %u\n", i, on);
			pdata->exts_enabled[i] = on ? 1 : 0;

			aux_ctrl = XGMAC_RGRD(pdata, MAC_AUX_CTRL);
			SET_N_BITS(aux_ctrl,
				   MAC_AUX_CTRL_ATSEN0_POS + i,
				   1,
				   on ? 1 : 0);
			XGMAC_RGWR(pdata, MAC_AUX_CTRL, aux_ctrl);
		} else {
			mac_dbg("Invalid request\n");
			return -EINVAL;
		}
		return 0;
	default:
		break;
	}

	return -EOPNOTSUPP;
}

#ifdef CONFIG_PTP_1588_CLOCK
static u64 xgmac_get_auxtimestamp(struct mac_prv_data *pdata)
{
	u64 nsec;

	/* First read nanoseconds and then seconds.
	 * On reading seconds register the FIFO top
	 * entry gets removed.
	 */
	nsec = XGMAC_RGRD(pdata, MAC_AUX_NSEC);
	nsec += (u64)XGMAC_RGRD(pdata, MAC_AUX_SEC) * NSEC_TO_SEC;

	return nsec;
}
#endif

static void xgmac_extts_isr_handler(struct mac_prv_data *pdata,
				    u32 tstamp_sts)
{
#ifdef CONFIG_PTP_1588_CLOCK
	u8 val, i;
	struct ptp_clock_event event;
	u64 ts[N_EXT_TS] = {0};
	u8 ts_valid[N_EXT_TS] = {0};
	u8 cnt = MAC_GET_VAL(tstamp_sts, MAC_TSTAMP_STSR, ATSNS);

	while (cnt--) {
		val = MAC_GET_VAL(tstamp_sts, MAC_TSTAMP_STSR, ATSSTN);
		if (val && val <= N_EXT_TS) {
			ts[val - 1] = xgmac_get_auxtimestamp(pdata);
			ts_valid[val - 1] = 1;
		} else if (val > N_EXT_TS) {
			xgmac_get_auxtimestamp(pdata);
		}
	}

	for (i = 0; i < N_EXT_TS; i++) {
		if (ts_valid[i]) {
			if (pdata->exts_enabled[i]) {
				event.timestamp = ts[i];
				event.type = PTP_CLOCK_EXTTS;
				event.index = i;
				ptp_clock_event(pdata->ptp_clock, &event);
			}
		}
	}
#endif
}

/* This API performs the required steps for enabling PTP support.
 * This api will be called by MAC driver when initializing MAC
 */
int xgmac_ptp_init(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	int ret = 0;

	if (!pdata->hw_feat.ts_src || !pdata->hw_feat.osten) {
		ret = -1;
		pdata->ptp_clock = NULL;
		pr_info("No PTP support in HW Aborting PTP clk drv register\n");
		return ret;
	}

	/* do nothing if we already have a clock device */
	if (pdata->ptp_clock)
		return -1;

	/* Initialize the 64 Entry Tx FIFO */
	fifo_init(pdev);

	/* Initialize the spin lock first since we can't control when a user
	 * will call the entry functions once we have initialized the clock
	 * device
	 */

	spin_lock_init(&pdata->ptp_lock);

	/* Initialize the work queue, this is needed for 2 step timestamp
	 * process
	 */
	INIT_WORK(&pdata->ptp_tx_work, (work_func_t)xgmac_ptp_tx_work);

	ret = xgmac_ptp_register(pdev);

	if (ret == -1)
		pr_info("Already have a PTP clock device\n");
	
	if (pdata->ptp_clock) {
		/* Start internal clock */
		xgmac_config_tstamp(&pdata->ops, BIT(MAC_TSTAMP_CR_TSENA_POS));
		xgmac_config_timer_reg(&pdata->ops, BIT(MAC_TSTAMP_CR_TSENA_POS));

		/* Enable timestamp interrupt for ExtTS */
		mac_int_enable(pdev);
		xgmac_set_mac_int(pdev, XGMAC_TSTAMP_EVNT, 1);
	}

	return ret;
}

#ifdef CONFIG_PTP_1588_CLOCK
static int get_ext_ref_time(struct mac_prv_data *pdata)
{
	struct gswss *gswss;
	struct adap_ops *ops;
	struct platform_device *mac_pdev;
	struct mac_ops *mac_ops;
	struct mac_prv_data *ext_pdata;
	u32 ref, dig, bin, pps;

	gswss = dev_get_drvdata(pdata->pdev->dev.parent);
	ops = gswss->adap_ops;

	ops->ss_get_cfg0_1588(ops, &ref, &dig, &bin, &pps);
	if (ref < 3 || ref > 5 || ref >= 3 + gswss->mac_subdevs_cnt)
		return -1;

	mac_pdev = gswss->mac_dev[ref - 3];
	mac_ops = platform_get_drvdata(mac_pdev);
	ext_pdata = GET_MAC_PDATA(mac_ops);
	if (ext_pdata->ptp_clock)
		return ptp_clock_index(ext_pdata->ptp_clock);
	else
		return -1;
}
#endif

int xgmac_get_ts_info(void *pdev,
		      struct ethtool_ts_info *ts_info)
{
#ifdef CONFIG_PTP_1588_CLOCK
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (pdata->ext_ref_time)
		ts_info->phc_index = get_ext_ref_time(pdata);
	else if (pdata->ptp_clock)
		ts_info->phc_index = ptp_clock_index(pdata->ptp_clock);
	else
		ts_info->phc_index = -1;

#endif

	ts_info->so_timestamping = SOF_TIMESTAMPING_TX_SOFTWARE |
				   SOF_TIMESTAMPING_RX_SOFTWARE |
				   SOF_TIMESTAMPING_SOFTWARE |
				   SOF_TIMESTAMPING_TX_HARDWARE |
				   SOF_TIMESTAMPING_RX_HARDWARE |
				   SOF_TIMESTAMPING_RAW_HARDWARE;

	ts_info->tx_types = (1 << HWTSTAMP_TX_OFF) | (1 << HWTSTAMP_TX_ON);
	ts_info->rx_filters = (1 << HWTSTAMP_FILTER_NONE) |
			      (1 << HWTSTAMP_FILTER_PTP_V1_L4_EVENT) |
			      (1 << HWTSTAMP_FILTER_PTP_V1_L4_SYNC) |
			      (1 << HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_L4_EVENT) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_L4_SYNC) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_EVENT) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_SYNC) |
			      (1 << HWTSTAMP_FILTER_PTP_V2_DELAY_REQ) |
			      (1 << HWTSTAMP_FILTER_ALL);

	return 0;
}

static int xgmac_ptp_register(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct ptp_clock_info *info = &pdata->ptp_clk_info;
	int ret = 0;

	/* do nothing if we already have a clock device */
	if (pdata->ptp_clock)
		return -1;

	snprintf(info->name, sizeof(info->name),
		 "%s", "xgmac_clk");

	info->owner = THIS_MODULE;
	info->max_adj = MAX_FREQ_ADJUSTMENT;
	info->n_ext_ts = N_EXT_TS;
	info->adjfreq = xgmac_adj_freq;
	info->adjtime = xgmac_adj_time;
	info->gettime64 = xgmac_get_time;
	info->settime64 = xgmac_set_time;
	info->enable	= xgmac_extts_enable;
#ifdef CONFIG_PTP_1588_CLOCK
	if (!pdata->ext_ref_time) {
		pdata->ptp_clock = ptp_clock_register(info, pdata->dev);
		if (IS_ERR(pdata->ptp_clock)) {
			pdata->ptp_clock = NULL;
			pr_err("ptp_clock_register() failed\n");
			return -1;
		}
	}
#endif

	mac_dbg("Added PTP HW clock successfully\n");

	/* Disable all timestamping to start */
	pdata->tstamp_config.tx_type = HWTSTAMP_TX_OFF;
	pdata->tstamp_config.rx_filter = HWTSTAMP_FILTER_NONE;

	return ret;
}

void xgmac_ptp_remove(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	cancel_work_sync(&pdata->ptp_tx_work);

#ifdef CONFIG_PTP_1588_CLOCK
	if (pdata->ptp_clock) {
		ptp_clock_unregister(pdata->ptp_clock);
		pr_info("Removed PTP HW clock successfully\n");
	}
#endif
}

