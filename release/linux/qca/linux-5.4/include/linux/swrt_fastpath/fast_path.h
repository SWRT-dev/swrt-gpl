#ifndef __FAST_PATH_H__
#define __FAST_PATH_H__

#define SWRT_FASTPATH(skb) fast_path_nf_filter_port(skb)
int fast_path_nf_filter_port(struct sk_buff *skb);
#endif
