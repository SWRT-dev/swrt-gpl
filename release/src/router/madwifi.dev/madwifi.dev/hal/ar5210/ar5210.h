/*
 * Copyright (c) 2002-2006 Sam Leffler, Errno Consulting
 * Copyright (c) 2002-2006 Atheros Communications, Inc.
 * All rights reserved.
 *
 * $Id: //depot/sw/branches/sam_hal/ar5210/ar5210.h#8 $
 */
#ifndef _ATH_AR5210_H_
#define _ATH_AR5210_H_

#define	AR5210_MAGIC	0x19980124

#if 0
/*
 * RTS_ENABLE includes LONG_PKT because they essentially
 * imply the same thing, and are set or not set together
 * for this chip
 */
#define AR5210_TXD_CTRL_A_HDR_LEN(_val)         (((_val)      ) & 0x0003f)
#define AR5210_TXD_CTRL_A_TX_RATE(_val)         (((_val) <<  6) & 0x003c0)
#define AR5210_TXD_CTRL_A_RTS_ENABLE            (                 0x00c00)
#define AR5210_TXD_CTRL_A_CLEAR_DEST_MASK(_val) (((_val) << 12) & 0x01000)
#define AR5210_TXD_CTRL_A_ANT_MODE(_val)        (((_val) << 13) & 0x02000)
#define AR5210_TXD_CTRL_A_PKT_TYPE(_val)        (((_val) << 14) & 0x1c000)
#define AR5210_TXD_CTRL_A_INT_REQ               (                 0x20000)
#define AR5210_TXD_CTRL_A_KEY_VALID             (                 0x40000)
#define AR5210_TXD_CTRL_B_KEY_ID(_val)          (((_val)      ) & 0x0003f)
#define AR5210_TXD_CTRL_B_RTS_DURATION(_val)    (((_val) <<  6) & 0x7ffc0)
#endif

#define INIT_CONFIG_STATUS              0x00000000
#define INIT_ACKTOPS                    0x00000008
#define INIT_BCON_CNTRL_REG             0x00000000
#define INIT_SLOT_TIME                  0x00000168
#define INIT_SLOT_TIME_TURBO            0x000001e0 /* More aggressive turbo slot timing = 6 us */
#define INIT_ACK_CTS_TIMEOUT            0x04000400
#define INIT_ACK_CTS_TIMEOUT_TURBO      0x08000800

#define INIT_USEC                       0x27
#define INIT_USEC_TURBO                 0x4f
#define INIT_USEC_32                    0x1f
#define INIT_TX_LATENCY                 0x36
#define INIT_RX_LATENCY                 0x1D
#define INIT_TRANSMIT_LATENCY \
	((INIT_RX_LATENCY << AR_USEC_RX_LATENCY_S) | \
	 (INIT_TX_LATENCY << AR_USEC_TX_LATENCY_S) | \
	 (INIT_USEC_32 << 7) | INIT_USEC )
#define INIT_TRANSMIT_LATENCY_TURBO  \
	((INIT_RX_LATENCY << AR_USEC_RX_LATENCY_S) | \
	 (INIT_TX_LATENCY << AR_USEC_TX_LATENCY_S) | \
	 (INIT_USEC_32 << 7) | INIT_USEC_TURBO)

#define INIT_SIFS                       0x230 /* = 16 us - 2 us */
#define INIT_SIFS_TURBO                 0x1E0 /* More aggressive turbo SIFS timing - 8 us - 2 us */

/*
 * Various fifo fill before Tx start, in 64-byte units
 * i.e. put the frame in the air while still DMAing
 */
#define MIN_TX_FIFO_THRESHOLD           0x1
#define MAX_TX_FIFO_THRESHOLD           ((IEEE80211_MAX_LEN / 64) + 1)

#define INIT_NEXT_CFP_START             0xffffffff

#define INIT_BEACON_PERIOD              0xffff
#define INIT_BEACON_EN                  0 /* this should be set by AP only when it's ready */
#define INIT_BEACON_CONTROL \
	((INIT_RESET_TSF << 24) | (INIT_BEACON_EN << 23) | \
	 (INIT_TIM_OFFSET<<16)  | INIT_BEACON_PERIOD)

#define INIT_RSSI_THR                   0x00000700 /* Missed beacon counter initialized to max value of 7 */
#define INIT_ProgIFS                    0x398      /* PIFS - 2us */
#define INIT_ProgIFS_TURBO              0x3C0
#define INIT_EIFS                       0xd70
#define INIT_EIFS_TURBO                 0x1ae0
#define INIT_CARR_SENSE_EN              1
#define INIT_PROTO_TIME_CNTRL           ( (INIT_CARR_SENSE_EN << 26) | (INIT_EIFS << 12) | \
                                          (INIT_ProgIFS) )
#define INIT_PROTO_TIME_CNTRL_TURBO     ( (INIT_CARR_SENSE_EN << 26) | (INIT_EIFS_TURBO << 12) | \
                                          (INIT_ProgIFS_TURBO) )

/*
 * EEPROM defines for Version 1 Crete EEPROM.
 *
 * The EEPROM is segmented into three sections:
 *
 *    PCI/Cardbus default configuration settings
 *    Cardbus CIS tuples and vendor-specific data
 *    Atheros-specific data
 *
 * EEPROM entries are read 32-bits at a time through the PCI bus
 * interface but are all 16-bit values.
 *
 * Access to the Atheros-specific data is controlled by protection
 * bits and the data is checksum'd.  The driver reads the Atheros
 * data from the EEPROM at attach and caches it in its private state.
 * This data includes the local regulatory domain, channel calibration
 * settings, and phy-related configuration settings.
 */
#define	AR_EEPROM_MAC(i)	(0x1f-(i))/* MAC address word */
#define	AR_EEPROM_MAGIC		0x3d	/* magic number */
#define AR_EEPROM_PROTECT	0x3f	/* Atheros segment protect register */
#define	AR_EEPROM_PROTOTECT_WP_128_191	0x80
#define AR_EEPROM_REG_DOMAIN	0xbf	/* Current regulatory domain register */
#define AR_EEPROM_ATHEROS_BASE	0xc0	/* Base of Atheros-specific data */
#define AR_EEPROM_ATHEROS_MAX	64	/* 64x2=128 bytes of EEPROM settings */
#define	AR_EEPROM_ATHEROS(n)	(AR_EEPROM_ATHEROS_BASE+(n))
#define	AR_EEPROM_VERSION	AR_EEPROM_ATHEROS(1)
#define AR_EEPROM_ATHEROS_TP_SETTINGS	0x09	/* Transmit power settings */
#define AR_REG_DOMAINS_MAX	4	/* # of Regulatory Domains */
#define AR_CHANNELS_MAX		5	/* # of Channel calibration groups */
#define AR_TP_SETTINGS_SIZE	11	/* # locations/Channel group */
#define AR_TP_SCALING_ENTRIES	11	/* # entries in transmit power dBm->pcdac */

/*
 * NB: we store the rfsilent select+polarity data packed
 *     with the encoding used in later parts so values
 *     returned to applications are consistent.
 */
#define AR_EEPROM_RFSILENT_GPIO_SEL	0x001c
#define AR_EEPROM_RFSILENT_GPIO_SEL_S	2
#define AR_EEPROM_RFSILENT_POLARITY	0x0002
#define AR_EEPROM_RFSILENT_POLARITY_S	1

#define AR_I2DBM(x)	((u_int8_t)((x * 2) + 3))

/*
 * Transmit power and channel calibration settings.
 */
struct tpcMap {
	u_int8_t	pcdac[AR_TP_SCALING_ENTRIES];
	u_int8_t	gainF[AR_TP_SCALING_ENTRIES];
	u_int8_t	rate36;
	u_int8_t	rate48;
	u_int8_t	rate54;
	u_int8_t	regdmn[AR_REG_DOMAINS_MAX];
};

/* NB: this is in ah_eeprom.h which isn't used for 5210 support */
#ifndef MAX_RATE_POWER
#define	MAX_RATE_POWER	60
#endif

#undef HAL_NUM_TX_QUEUES	/* from ah.h */
#define	HAL_NUM_TX_QUEUES	3

struct ath_hal_5210 {
	struct ath_hal_private ah_priv;	/* base definitions */

	/*
	 * Information retrieved from EEPROM
	 */
	u_int16_t	ah_eeversion;		/* EEPROM Version field */
	u_int16_t	ah_eeprotect;		/* EEPROM protection settings */
	u_int16_t	ah_antenna;		/* Antenna Settings */
	u_int16_t	ah_biasCurrents;	/* OB, DB */
	u_int8_t	ah_thresh62;		/* thresh62 */
	u_int8_t	ah_xlnaOn;		/* External LNA timing */
	u_int8_t	ah_xpaOff;		/* Extern output stage timing */
	u_int8_t	ah_xpaOn;		/* Extern output stage timing */
	u_int8_t	ah_rfKill;		/* Single low bit signalling if RF Kill is implemented */
	u_int8_t	ah_devType;		/* Type: PCI, miniPCI, CB */
	u_int8_t	ah_regDomain[AR_REG_DOMAINS_MAX];
						/* calibrated reg domains */
	struct tpcMap	ah_tpc[AR_CHANNELS_MAX];
	u_int8_t	ah_macaddr[IEEE80211_ADDR_LEN];
	/*
	 * Runtime state.
	 */
	u_int32_t	ah_maskReg;		/* shadow of IMR+IER regs */
	u_int32_t	ah_txOkInterruptMask;
	u_int32_t	ah_txErrInterruptMask;
	u_int32_t	ah_txDescInterruptMask;
	u_int32_t	ah_txEolInterruptMask;
	u_int32_t	ah_txUrnInterruptMask;
	HAL_POWER_MODE	ah_powerMode;
	u_int8_t	ah_bssid[IEEE80211_ADDR_LEN];
	HAL_TX_QUEUE_INFO ah_txq[HAL_NUM_TX_QUEUES]; /* beacon+cab+data */
	/*
	 * Station mode support.
	 */
	u_int32_t	ah_staId1Defaults;	/* STA_ID1 default settings */
	u_int32_t	ah_rssiThr;		/* RSSI_THR settings */

	u_int		ah_sifstime;		/* user-specified sifs time */
	u_int		ah_slottime;		/* user-specified slot time */
	u_int		ah_acktimeout;		/* user-specified ack timeout */
	u_int		ah_ctstimeout;		/* user-specified cts timeout */
};
#define	AH5210(ah)	((struct ath_hal_5210 *)(ah))

struct ath_hal;

extern	struct ath_hal *ar5210Attach(u_int16_t, HAL_SOFTC,
	HAL_BUS_TAG, HAL_BUS_HANDLE, HAL_STATUS *);
extern	void ar5210Detach(struct ath_hal *);

extern	HAL_BOOL ar5210Reset(struct ath_hal *, HAL_OPMODE,
		HAL_CHANNEL *, HAL_RESET_TYPE resetType, HAL_STATUS *);
extern	void ar5210SetPCUConfig(struct ath_hal *);
extern	HAL_BOOL ar5210PhyDisable(struct ath_hal *);
extern	HAL_BOOL ar5210Disable(struct ath_hal *);
extern	HAL_BOOL ar5210ChipReset(struct ath_hal *, HAL_CHANNEL *);
extern	HAL_BOOL ar5210PerCalibration(struct ath_hal *, HAL_CHANNEL *, HAL_BOOL longCal, HAL_BOOL *);
extern	int16_t ar5210GetNoiseFloor(struct ath_hal *);
extern	int16_t ar5210GetNfAdjust(struct ath_hal *,
		const HAL_CHANNEL_INTERNAL *);
extern	HAL_BOOL ar5210SetTxPowerLimit(struct ath_hal *, u_int32_t limit);
extern	HAL_BOOL ar5210SetTransmitPower(struct ath_hal *, HAL_CHANNEL *);
extern	HAL_BOOL ar5210CalNoiseFloor(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern	HAL_BOOL ar5210ResetDma(struct ath_hal *, HAL_OPMODE);

extern  HAL_BOOL ar5210SetTxQueueProps(struct ath_hal *ah, int q,
		const HAL_TXQ_INFO *qInfo);
extern	HAL_BOOL ar5210GetTxQueueProps(struct ath_hal *ah, int q,
		HAL_TXQ_INFO *qInfo);
extern	int ar5210SetupTxQueue(struct ath_hal *ah, HAL_TX_QUEUE type,
		const HAL_TXQ_INFO *qInfo);
extern	HAL_BOOL ar5210ReleaseTxQueue(struct ath_hal *ah, u_int q);
extern	HAL_BOOL ar5210ResetTxQueue(struct ath_hal *ah, u_int q);
extern	u_int32_t ar5210GetTxDP(struct ath_hal *, u_int);
extern	HAL_BOOL ar5210SetTxDP(struct ath_hal *, u_int, u_int32_t txdp);
extern	HAL_BOOL ar5210UpdateTxTrigLevel(struct ath_hal *, HAL_BOOL);
extern	u_int32_t ar5210NumTxPending(struct ath_hal *, u_int);
extern	HAL_BOOL ar5210StartTxDma(struct ath_hal *, u_int);
extern	HAL_BOOL ar5210StopTxDma(struct ath_hal *, u_int);
extern	HAL_BOOL ar5210SetupTxDesc(struct ath_hal *, struct ath_desc *,
		u_int pktLen, u_int hdrLen, HAL_PKT_TYPE type, u_int txPower,
		u_int txRate0, u_int txRetries0,
		u_int keyIx, u_int antMode, u_int flags,
		u_int rtsctsRate, u_int rtsctsDuration,
                u_int compicvLen, u_int compivLen, u_int comp);
extern	HAL_BOOL ar5210SetupXTxDesc(struct ath_hal *, struct ath_desc *,
		u_int txRate1, u_int txRetries1,
		u_int txRate2, u_int txRetries2,
		u_int txRate3, u_int txRetries3);
extern	HAL_BOOL ar5210FillTxDesc(struct ath_hal *, struct ath_desc *,
		u_int segLen, HAL_BOOL firstSeg, HAL_BOOL lastSeg,
		const struct ath_desc *ds0);
extern	HAL_STATUS ar5210ProcTxDesc(struct ath_hal *,
		struct ath_desc *, struct ath_tx_status *);
extern  void ar5210IntrReqTxDesc(struct ath_hal *ah, struct ath_desc *);

extern	u_int32_t ar5210GetRxDP(struct ath_hal *);
extern	void ar5210SetRxDP(struct ath_hal *, u_int32_t rxdp);
extern	void ar5210EnableReceive(struct ath_hal *);
extern	HAL_BOOL ar5210StopDmaReceive(struct ath_hal *);
extern	void ar5210StartPcuReceive(struct ath_hal *);
extern	void ar5210StopPcuReceive(struct ath_hal *);
extern	void ar5210SetMulticastFilter(struct ath_hal *,
		u_int32_t filter0, u_int32_t filter1);
extern	HAL_BOOL ar5210ClrMulticastFilterIndex(struct ath_hal *, u_int32_t);
extern	HAL_BOOL ar5210SetMulticastFilterIndex(struct ath_hal *, u_int32_t);
extern	u_int32_t ar5210GetRxFilter(struct ath_hal *);
extern	void ar5210SetRxFilter(struct ath_hal *, u_int32_t);
extern	HAL_BOOL ar5210SetupRxDesc(struct ath_hal *, struct ath_desc *,
		u_int32_t, u_int flags);
extern	HAL_STATUS ar5210ProcRxDesc(struct ath_hal *, struct ath_desc *,
		u_int32_t, struct ath_desc *, u_int64_t,
		struct ath_rx_status *);

extern	void ar5210GetMacAddress(struct ath_hal *, u_int8_t *);
extern	HAL_BOOL ar5210SetMacAddress(struct ath_hal *ah, const u_int8_t *);
extern	void ar5210GetBssIdMask(struct ath_hal *, u_int8_t *);
extern	HAL_BOOL ar5210SetBssIdMask(struct ath_hal *, const u_int8_t *);
extern	HAL_BOOL ar5210EepromRead(struct ath_hal *, u_int off, u_int16_t *data);
extern	HAL_BOOL ar5210EepromWrite(struct ath_hal *, u_int off, u_int16_t data);
extern	HAL_BOOL ar5210SetRegulatoryDomain(struct ath_hal *,
		u_int16_t, HAL_STATUS *);
extern	u_int ar5210GetWirelessModes(struct ath_hal *ah);
extern	void ar5210EnableRfKill(struct ath_hal *);
extern	HAL_BOOL ar5210GpioCfgInput(struct ath_hal *, u_int32_t gpio);
extern	HAL_BOOL ar5210GpioCfgOutput(struct ath_hal *, u_int32_t gpio);
extern	u_int32_t ar5210GpioGet(struct ath_hal *, u_int32_t gpio);
extern	HAL_BOOL ar5210GpioSet(struct ath_hal *, u_int32_t gpio, u_int32_t);
extern	void ar5210Gpio0SetIntr(struct ath_hal *, u_int, u_int32_t ilevel);
extern	void ar5210SetLedState(struct ath_hal *, HAL_LED_STATE);
extern	u_int ar5210GetDefAntenna(struct ath_hal *);
extern	void ar5210SetDefAntenna(struct ath_hal *, u_int);
extern	HAL_ANT_SETTING ar5210GetAntennaSwitch(struct ath_hal *);
extern	HAL_BOOL ar5210SetAntennaSwitch(struct ath_hal *, HAL_ANT_SETTING);
extern	void ar5210WriteAssocid(struct ath_hal *,
		const u_int8_t *bssid, u_int16_t assocId);
extern	u_int32_t ar5210GetTsf32(struct ath_hal *);
extern	u_int64_t ar5210GetTsf64(struct ath_hal *);
extern	void ar5210ResetTsf(struct ath_hal *);
extern	u_int32_t ar5210GetRandomSeed(struct ath_hal *);
extern	HAL_BOOL ar5210DetectCardPresent(struct ath_hal *);
extern	void ar5210UpdateMibCounters(struct ath_hal *, HAL_MIB_STATS *);
extern	void ar5210EnableHwEncryption(struct ath_hal *);
extern	void ar5210DisableHwEncryption(struct ath_hal *);
extern	HAL_RFGAIN ar5210GetRfgain(struct ath_hal *);
extern	HAL_BOOL ar5210SetSifsTime(struct ath_hal *, u_int);
extern	u_int ar5210GetSifsTime(struct ath_hal *);
extern	HAL_BOOL ar5210SetSlotTime(struct ath_hal *, u_int);
extern	u_int ar5210GetSlotTime(struct ath_hal *);
extern	HAL_BOOL ar5210SetAckTimeout(struct ath_hal *, u_int);
extern	u_int ar5210GetAckTimeout(struct ath_hal *);
extern	HAL_BOOL ar5210SetAckCTSRate(struct ath_hal *, u_int);
extern	u_int ar5210GetAckCTSRate(struct ath_hal *);
extern	HAL_BOOL ar5210SetCTSTimeout(struct ath_hal *, u_int);
extern	u_int ar5210GetCTSTimeout(struct ath_hal *);
extern	HAL_STATUS ar5210GetCapability(struct ath_hal *, HAL_CAPABILITY_TYPE,
		u_int32_t, u_int32_t *);
extern	HAL_BOOL ar5210SetCapability(struct ath_hal *, HAL_CAPABILITY_TYPE,
		u_int32_t, u_int32_t, HAL_STATUS *);
extern	HAL_BOOL ar5210GetDiagState(struct ath_hal *ah, int request,
		const void *args, u_int32_t argsize,
		void **result, u_int32_t *resultsize);

extern	u_int ar5210GetKeyCacheSize(struct ath_hal *);
extern	HAL_BOOL ar5210IsKeyCacheEntryValid(struct ath_hal *, u_int16_t);
extern	HAL_BOOL ar5210ResetKeyCacheEntry(struct ath_hal *, u_int16_t entry);
extern	HAL_BOOL ar5210SetKeyCacheEntry(struct ath_hal *, u_int16_t entry,
                       const HAL_KEYVAL *, const u_int8_t *mac, int xorKey);
extern	HAL_BOOL ar5210SetKeyCacheEntryMac(struct ath_hal *,
			u_int16_t, const u_int8_t *);

extern	HAL_BOOL ar5210SetPowerMode(struct ath_hal *, u_int32_t powerRequest,
		int setChip);
extern	HAL_POWER_MODE ar5210GetPowerMode(struct ath_hal *);

extern	void ar5210SetBeaconTimers(struct ath_hal *,
		const HAL_BEACON_TIMERS *);
extern	void ar5210BeaconInit(struct ath_hal *, u_int32_t, u_int32_t);
extern	void ar5210SetStaBeaconTimers(struct ath_hal *,
		const HAL_BEACON_STATE *);
extern	void ar5210ResetStaBeaconTimers(struct ath_hal *);

extern	HAL_BOOL ar5210IsInterruptPending(struct ath_hal *);
extern	HAL_BOOL ar5210GetPendingInterrupts(struct ath_hal *, HAL_INT *);
extern	HAL_INT ar5210GetInterrupts(struct ath_hal *);
extern	HAL_INT ar5210SetInterrupts(struct ath_hal *, HAL_INT ints);

extern	const HAL_RATE_TABLE *ar5210GetRateTable(struct ath_hal *, u_int mode);

#endif /* _ATH_AR5210_H_ */
