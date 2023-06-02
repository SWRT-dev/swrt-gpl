#ifndef INTEL_H
#define INTEL_H

#include <stdint.h>

/* The following structs are used in the '/src/drivers/driver.h' file
 * of the hostapd package by Intel.
 * The same are used here.
 */

#ifndef ETH_ALEN
#define ETH_ALEN	6
#endif

typedef struct mtlk_blacklist_cfg {
	uint8_t addr[ETH_ALEN];
	uint16_t status;
	uint8_t remove;
} __attribute__ ((packed)) mtlk_blacklist_cfg_t;

typedef struct mtlk_steer_cfg {
	uint8_t addr[ETH_ALEN];
	uint8_t bssid[ETH_ALEN];
	uint8_t status;
} __attribute__ ((packed)) mtlk_steer_cfg_t;

struct mtlk_wssa_peer_traffic_stats {
	uint32_t BytesSent;
	uint32_t BytesReceived;
	uint32_t PacketsSent;
	uint32_t PacketsReceived;
};

struct mtlk_wssa_driver_traffic_stats {
	uint32_t BytesSent;
	uint32_t BytesReceived;
	uint32_t PacketsSent;
	uint32_t PacketsReceived;
	uint32_t UnicastPacketsSent;
	uint32_t UnicastPacketsReceived;
	uint32_t MulticastPacketsSent;
	uint32_t MulticastPacketsReceived;
	uint32_t BroadcastPacketsSent;
	uint32_t BroadcastPacketsReceived;
} __attribute__ ((packed));

struct mtlk_wssa_peer_error_stats {
	uint32_t ErrorsSent;
	uint32_t ErrorsReceived;
	uint32_t DiscardPacketsSent;
	uint32_t DiscardPacketsReceived;
} __attribute__ ((packed));

#define STA_INFO_PS_STATE 1

struct mtlk_wssa_drv_tr181_peer_stats {
	uint32_t StationId;
	uint32_t NetModesSupported;
	struct mtlk_wssa_peer_traffic_stats traffic_stats;
	uint32_t Retransmissions;
	uint32_t LastDataUplinkRate;
	uint32_t LastDataDownlinkRate;
	int32_t SignalStrength;
	int32_t ShortTermRSSIAverage[4];
} __attribute__ ((packed));

typedef struct mtlk_sta_info {
	struct mtlk_wssa_drv_tr181_peer_stats peer_stats;
	/* uint8_t stationInfo; */
	uint32_t TxMgmtPwr;
} __attribute__ ((packed)) mtlk_sta_info_t;

struct mtlk_wssa_drv_tr181_vap_stats {
	struct mtlk_wssa_driver_traffic_stats traffic_stats;
	struct mtlk_wssa_peer_error_stats error_stats;
	uint32_t RetransCount;
	uint32_t FailedRetransCount;
	uint32_t RetryCount;
	uint32_t MultipleRetryCount;
	uint32_t ACKFailureCount;
	uint32_t AggregatedPacketCount;
	uint32_t UnknownProtoPacketsReceived;
} __attribute__ ((packed));

struct mtlk_vap_info {
	struct mtlk_wssa_drv_tr181_vap_stats vap_stats;
	uint64_t TransmittedOctetsInAMSDUCount;
	uint64_t ReceivedOctetsInAMSDUCount;
	uint64_t TransmittedOctetsInAMPDUCount;
	uint64_t ReceivedOctetsInAMPDUCount;
	uint32_t RTSSuccessCount;
	uint32_t RTSFailureCount;
	uint32_t TransmittedAMSDUCount;
	uint32_t FailedAMSDUCount;
	uint32_t AMSDUAckFailureCount;
	uint32_t ReceivedAMSDUCount;
	uint32_t TransmittedAMPDUCount;
	uint32_t TransmittedMPDUsInAMPDUCount;
	uint32_t AMPDUReceivedCount;
	uint32_t MPDUInReceivedAMPDUCount;
	uint32_t ImplicitBARFailureCount;
	uint32_t ExplicitBARFailureCount;
	uint32_t TwentyMHzFrameTransmittedCount;
	uint32_t FortyMHzFrameTransmittedCount;
	uint32_t SwitchChannel20To40;
	uint32_t SwitchChannel40To20;
	uint32_t FrameDuplicateCount;
} __attribute__ ((packed));

struct mtlk_wssa_drv_tr181_hw {
	uint8_t Enable;
	uint8_t Channel;
} __attribute__ ((packed));

struct mtlk_wssa_drv_tr181_hw_stats {
	struct mtlk_wssa_driver_traffic_stats traffic_stats;
	struct mtlk_wssa_peer_error_stats error_stats;
	uint32_t FCSErrorCount;
	int32_t Noise;
} __attribute__ ((packed));

typedef struct mtlk_radio_info {
	struct mtlk_wssa_drv_tr181_hw hw;
	struct mtlk_wssa_drv_tr181_hw_stats hw_stats;
	uint64_t tsf_start_time;
	uint8_t load;
	uint32_t tx_pwr_cfg;
	uint8_t num_tx_antennas;
	uint8_t num_rx_antennas;
	uint32_t primary_center_freq; /* center frequency in MHz */
	uint32_t center_freq1;
	uint32_t center_freq2;
	uint32_t width; /* 20,40,80,... */
} __attribute__ ((packed)) mtlk_radio_info_t;


typedef struct mtlk_mac_addr_list_cfg {
	uint8_t addr[ETH_ALEN];
	uint8_t remove;
} mtlk_mac_addr_list_cfg_t;

#endif /* INTEL_H */
