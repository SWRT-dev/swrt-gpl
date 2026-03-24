#ifndef GENETLINK_H
#define GENETLINK_H

#include <stdbool.h>


typedef void (*genetlink_cb)(void *ctx, void *data);

struct genetlink_config {
	bool is_static;			/* if TRUE, then memory is allocated statically */
	void *ctx;				/* pointer to context */
	genetlink_cb rxmsg_cb;	/* pointer to receive call-back function */
	int genetlink_group;	/* use NETLINK_RTLOG_APPS_GROUP */
	u8 genetlink_cmd;		/* use NL_DRV_CMD_RTLOG_NOTIFY */
};


struct genetlink_data * genetlink_init(struct genetlink_config *cfg);
void genetlink_deinit(struct genetlink_data *genetlink);
int genetlink_send_msg(struct genetlink_data *genetlink, const void *data,
			   const int data_len);

#endif /* GENETLINK_H */
