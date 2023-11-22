#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <libubox/list.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "util.h"
#include "dpp_api.h"

#define MAX 128
#define PORT 8080
#define SA struct sockaddr

#define PUB_AF_CATEGORY		0x04
#define DPP_PUB_AF_ACTION		0x09
#define DPP_PUB_AF_ACTION_OUI_TYPE	0x1A
#define DPP_PUB_AF_GAS_INITIAL_REQ	0x0A
#define DPP_PUB_AF_GAS_INITIAL_RESP	0x0B

#define FRAME_IS_DPP_PUB_AF(frame) \
		((*(frame) == DPP_PUB_AF_ACTION) && \
		(memcmp((frame + 1), "\x50\x6F\x9A", 3) == 0) && \
		((*(frame + 4)) == DPP_PUB_AF_ACTION_OUI_TYPE))

#define FRAME_IS_DPP_GAS_FRAME(frame) \
		(((*(frame) == DPP_PUB_AF_GAS_INITIAL_REQ) || \
		(*(frame) == DPP_PUB_AF_GAS_INITIAL_RESP)) && \
		memcmp((frame + 12), "\x50\x6F\x9A", 3))


uint8_t dpp_get_pub_af_type(const uint8_t *frame, uint16_t framelen)
{
	if (framelen < 7)
		return 255;

	return *(frame + 6);
}

uint8_t dpp_get_gas_frame_type(const uint8_t *frame, uint16_t framelen)
{
	if (framelen < 1)
		return 255;

	return *frame;
}

uint8_t dpp_get_frame_type(const uint8_t *frame, uint16_t framelen)
{
	if (FRAME_IS_DPP_PUB_AF(frame))
		return dpp_get_pub_af_type(frame, framelen);
	else if (FRAME_IS_DPP_GAS_FRAME(frame))
		return dpp_get_gas_frame_type(frame, framelen);
	return 255;
}

const char *dpp_frame_type2str(uint8_t type)
{
	if (type == DPP_PA_AUTHENTICATION_REQ)
		return "Authentication Request";
	else if (type == DPP_PA_AUTHENTICATION_RESP)
		return "Authentication Response";
	else if (type == DPP_PA_AUTHENTICATION_CONF)
		return "Authentication Confirm";
	else if (type == DPP_PA_PEER_DISCOVERY_REQ)
		return "Peer Discovery Request";
	else if (type == DPP_PA_PEER_DISCOVERY_RESP)
		return "Peer Discovery Response";
	else if (type == DPP_PA_PKEX_V1_EXCHANGE_REQ)
		return "PKEX Version 1 Exchange Request";
	else if (type == DPP_PA_PKEX_EXCHANGE_RESP)
		return "PKEX Exchange Response";
	else if (type == DPP_PA_PKEX_COMMIT_REVEAL_REQ)
		return "PKEX Commit-Reveal Request";
	else if (type == DPP_PA_PKEX_COMMIT_REVEAL_RESP)
		return "PKEX Commit-Reveal Response";
	else if (type == DPP_PA_CONFIGURATION_RESULT)
		return "Configuration Result";
	else if (type == DPP_PA_CONNECTION_STATUS_RESULT)
		return "Connection Status Result";
	else if (type == DPP_PA_PRESENCE_ANNOUNCEMENT)
		return "Presence Announcement";
	else if (type == DPP_PA_RECONFIG_ANNOUNCEMENT)
		return "Reconfiguration Announcement";
	else if (type == DPP_PA_RECONFIG_AUTH_REQ)
		return "Reconfiguration Authentication Request";
	else if (type == DPP_PA_RECONFIG_AUTH_RESP)
		return "Reconfiguration Authentication Response";
	else if (type == DPP_PA_RECONFIG_AUTH_CONF)
		return "Reconfiguration Authentication Confirm";
	else if (type == DPP_PA_PKEX_EXCHANGE_REQ)
		return "PKEX Exchange Request";
	else if (type == 0x13)
		return "Push Button Presence Announcement";
	else if (type == 0x14)
		return "Push Button Presence Announcement Response";
	else if (type == 0x15)
		return "Private Peer Introduction Query";
	else if (type == 0x16)
		return "Private Peer Introduction Notify";
	else if (type == 0x17)
		return "Private Peer Introduction Update";
	return "Unknown";
}

const char *dpp_gasframe_type2str(uint8_t type)
{
	if (type == WLAN_PA_GAS_INITIAL_REQ)
		return "GAS Initial Request";
	else if (type == WLAN_PA_GAS_INITIAL_RESP)
		return "GAS Initial Response";
	return "Unknown";
}


uint8_t addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
void *dpp;
int sockfd, connfd, len;

int dpp_frame_handler(void *dpp, uint8_t *smac, enum dpp_event ev, uint8_t *frame, size_t framelen)
{
	int frametype = 0;

	fprintf(stderr, "%s: ----->\n", __func__);

	switch (ev) {
	case DPP_EVENT_TX_FRAME:
		if (!smac) {
			fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
			return -1;
		}

		if (!frame) {
			fprintf(stderr, "%s: Error: frame = NULL\n", __func__);
			return -1;
		}

		if (!framelen) {
			fprintf(stderr, "%s: Err: framelen = 0\n", __func__);
			return -1;
		}

		frametype = dpp_get_frame_type(frame + 1, framelen - 1);
		if (frametype == 255) {
			fprintf(stderr, "%s: Invalid frame type!\n", __func__);
			return -1;
		}

		write(connfd, frame, framelen);
		break;
	case DPP_EVENT_AUTHENTICATED: {
		if (!smac) {
			fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
			return -1;
		}

		void *e = dpp_sm_create_event(dpp, smac, DPP_EVENT_TRIGGER, 0, NULL);

		if (e)
			dpp_trigger(dpp, smac, e);
		break;
	}
	case DPP_EVENT_TIMEOUT:
		struct dpp_peer *peer = dpp_lookup_peer(dpp, smac);

		if (peer) {
			list_del(&peer->list);
			dpp_free_peer(dpp, peer);
		}
		break;
	case DPP_EVENT_BOOTSTRAPPING:
		break;
	default:
		break;
	}

#if 0
	if (ev != DPP_EVENT_TX_FRAME)
		return -1;

	if (!smac) {
		fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
		return -1;
	}

	if (!frame) {
		fprintf(stderr, "%s: Error: frame = NULL\n", __func__);
		return -1;
	}

	if (!framelen) {
		fprintf(stderr, "%s: Err: framelen = 0\n", __func__);
		return -1;
	}


	fprintf(stderr, "%s: DPP_EVENT_TX_FRAME: smac = " MACFMT", frame = %p, len = %zu\n",
		__func__, MAC2STR(smac), frame, framelen);

	frametype = dpp_get_frame_type(frame + 1, framelen - 1);
	if (frametype == 255) {
		fprintf(stderr, "Invalid frametype\n");
		return -1;
	}

	fprintf(stderr, "DPP frametype %s\n", dpp_frame_type2str(frametype));

	switch(frametype) {
	case DPP_PA_AUTHENTICATION_REQ:
	case DPP_PA_AUTHENTICATION_CONF:
	case DPP_PUB_AF_GAS_INITIAL_RESP:
	case DPP_PA_PEER_DISCOVERY_RESP:
		//generate_encap_dpp();
		write(connfd, frame, framelen);
		break;
	default:
		//some error
		break;
	}
#endif
	return 0;
}


// Function designed for chat between client and server.
void func(int connfd)
{
	struct dpp_bootstrap_info *enrollee_bi;
	char buff[MAX] = {0};
	int ret = 0;
	int n = 0;

	fprintf(stderr, "enter enrollee URI : \n");

	while ((buff[n++] = getchar()) != '\n')
		;

	enrollee_bi = calloc(1, sizeof(*enrollee_bi));
	if (!enrollee_bi) {
		fprintf(stderr, "Failed to allocate enrollee_bi\n");
		return;
	}

	ret = dpp_build_bootstrap_info_from_uri(buff, enrollee_bi);
	if (ret) {
		fprintf(stderr, "Failed to build bootstrap info\n");
		return;
	}

	ret = dpp_bootstrap_add(dpp, enrollee_bi);
	if (ret) {
		fprintf(stderr, "Failed to add bootstrap\n");
		return;
	}

	for (;;) {
		uint8_t frametype;
		uint8_t *frame = calloc(1, 3000);
		int framelen;

		sleep(1); // Sleep to make output more easily readable

		framelen = read(connfd, frame, 3000);
		dump(frame, framelen, "dpp msg");

		frametype = dpp_get_frame_type(frame + 1, framelen - 1);
		if (frametype == 255) {
			free(frame);
			continue;
		}

		switch (frametype) {
		case DPP_PA_PRESENCE_ANNOUNCEMENT: {
			ret = dpp_process_presence_announcement(dpp, addr,
								frame,
								framelen);
			if (ret) {
				fprintf(stderr, "Failed to process presence announcement frame!\n");
				free(frame);
				continue;
			}

			fprintf(stderr, "Succeeded processing PA\n");
			break;
		}
		case DPP_PA_AUTHENTICATION_RESP:
		case DPP_PUB_AF_GAS_INITIAL_REQ:
		case DPP_PA_CONFIGURATION_RESULT:
		case DPP_PA_PEER_DISCOVERY_REQ:
		case DPP_PA_CONNECTION_STATUS_RESULT: {
			void *event =
				dpp_sm_create_event(dpp, addr, DPP_EVENT_RX_FRAME, framelen, frame);

			if (event) {
				dpp_trigger(dpp, addr, event);
				dpp_sm_free_event(event);
			}

			//fprintf(stderr, "---- NETWORK INTRODUCTION SUCCESS ----\n");
			break;

		}
		default:
			fprintf(stderr, "Unknown frame!\n");
			break;
		}

		free(frame);
	}
}

// Driver function
int main()
{
	struct sockaddr_in servaddr, cli;
	char *argv[] = {"-I", "-C", "-V", "2"};
	int argc = 4;
	int ret;

	ret = dpp_init(&dpp, argc, argv);
	if (ret) {
		fprintf(stderr, "Failed to init dpp context\n");
		exit(0);
	}

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	} else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	} else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	} else
		printf("Server listening..\n");
	len = sizeof(cli);

	dpp_register_cb(dpp, dpp_frame_handler);
	dpp_set_configuration(dpp, DPP_NETROLE_MAP_BH_STA, DPP_AKM_SAE,
			     "test--5g", "1234567890", BAND_5);
	dpp_set_configuration(dpp, DPP_NETROLE_MAP_BH_STA, DPP_AKM_SAE,
			     "test--2g", "1234567890", BAND_2);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, (socklen_t * restrict)&len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	} else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}
