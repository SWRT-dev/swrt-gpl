#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <libubox/list.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "util.h"
#include "dpp_api.h"

#define MAX 80
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


void *dpp;
struct dpp_peer *peer;
int sockfd;


int dpp_frame_handler(void *dpp, uint8_t *smac, enum dpp_event ev, uint8_t *frame, size_t framelen)
{
	int frametype = 0;

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

		write(sockfd, frame, framelen);
		break;
	case DPP_EVENT_AUTHENTICATED: {
		if (!smac) {
			fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
			return -1;
		}

		void *e = dpp_sm_create_event(dpp, smac, DPP_EVENT_TRIGGER, 0, NULL);

		if (e) {
			dpp_trigger(dpp, smac, e);
			dpp_sm_free_event(e);
		}
		break;
	}
	case DPP_EVENT_TIMEOUT:
		break;
	case DPP_EVENT_BOOTSTRAPPING:
		break;
	default:
		break;
	}

	fprintf(stderr, "%s: ----->\n", __func__);
#if 0
	if (ev != DPP_EVENT_TX_FRAME && ev != DPP_EVENT_TRIGGER)
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

	//TODO: get frametype from frame
	frametype = dpp_get_frame_type(frame + 1, framelen - 1);
	if (frametype == 255) {
		fprintf(stderr, "%s: Invalid frame type!\n", __func__);
		return -1;
	}


	fprintf(stderr, "DPP frametype %s\n", dpp_frame_type2str(frametype));

	switch(frametype) {
	case DPP_PA_AUTHENTICATION_RESP:
	case DPP_PUB_AF_GAS_INITIAL_REQ:
	case DPP_PA_CONFIGURATION_RESULT:
	case DPP_PA_PEER_DISCOVERY_REQ:
	case DPP_PA_CONNECTION_STATUS_RESULT:
		//generate_encap_dpp();
		if (ev == DPP_EVENT_TX_FRAME)
			write(sockfd, frame, framelen);
		else if (ev == DPP_EVENT_TRIGGER)

		break;
	default:
		//some error
		break;
	}
#endif
	return 0;
}


void func(int sockfd)
{
	//struct dpp_bootstrap_info *responder_bi;
	size_t msglen = 0;
	uint8_t *msg;
	int ret;
	uint8_t bcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	sleep(1);

	ret = dpp_build_presence_announcement(dpp, bcast, &msg, &msglen);
	if (ret)
		return;

	bufprintf("presence announcement", msg, msglen);

	ret = write(sockfd, msg, msglen);

	fprintf(stderr, "ret:%d\n", ret);

	for (;;) {
		uint8_t frametype;
		int framelen;
		uint8_t *frame = calloc(1, 3000);
		//uint8_t addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

		sleep(1); // Sleep to make output more easily readable

		framelen = read(sockfd, frame, 3000);
		dump(frame, framelen, "dpp msg");

		frametype = dpp_get_frame_type(frame + 1, framelen - 1);
		if (frametype == 255) {
			free(frame);
			continue;
		}

		fprintf(stderr, "|%s:%d| %s\n", __func__, __LINE__, dpp_frame_type2str(frametype));

		switch (frametype) {
		case DPP_PA_AUTHENTICATION_REQ:
		case DPP_PA_AUTHENTICATION_CONF:
		case DPP_PUB_AF_GAS_INITIAL_RESP:
		case DPP_PA_PEER_DISCOVERY_RESP: {
			void *ev = dpp_sm_create_event(dpp, bcast, DPP_EVENT_RX_FRAME, framelen, frame);

			if (ev) {
				dpp_trigger(dpp, bcast, ev);
				dpp_sm_free_event(ev);
			}
			break;
		}
		default:
			break;
		}
		free(frame);
	}
}

int main()
{
	struct sockaddr_in servaddr;
	int ret;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	ret = dpp_init(&dpp, 0, NULL);
	if (ret) {
		fprintf(stderr, "Failed to init dpp context\n");
		exit(0);
	}

	dpp_register_cb(dpp, dpp_frame_handler);
	peer = dpp_create_responder_instance(dpp);
	if (!peer) {
		fprintf(stderr, "Failed to create enrollee instance\n");
		exit(0);
	}

	//ret = dpp_gen_bootstrap_info("P-256", "81/1,115/36", NULL, NULL, &peer->own_bi);
	//if (ret) {
	//	fprintf(stderr, "%s: Failed to gen bootstrap info for self\n", __func__);
	//	exit(0);
	//}

	dpp_print_bootstrap_info(peer->own_bi);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))) {
		printf("connection with the server failed...\n");
		exit(0);
	} else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);

}
