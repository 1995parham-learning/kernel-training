/*
 * In The Name Of God
 * ========================================
 * [] File Name : NetF.h
 *
 * [] Creation Date : 25-12-2014
 *
 * [] Last Modified : Thu 25 Dec 2014 08:56:46 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>


struct netF {
	int lucky_number;
	struct nf_hook_ops nfho;
};

unsigned int filter(const struct nf_hook_ops *ops,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *));
