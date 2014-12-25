/*
 * In The Name Of God
 * ========================================
 * [] File Name : Task01.c
 *
 * [] Creation Date : 21-12-2014
 *
 * [] Last Modified : Thu 25 Dec 2014 06:42:02 PM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/

/*
 * This task is done just for FUN
 * and like always
 * We love you Dr.Bakshi
*/

#include <linux/string.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/printk.h>

#include "NetF.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani");
MODULE_DESCRIPTION("This module is netfilter patch");

static struct netF parham;

unsigned int filter(const struct nf_hook_ops *ops,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	if (!strcmp(out->name, "lo")) {
		pr_info("[NetF] %s\n", out->name);
		pr_info("[NetF] Packet Len is: %u\n", skb->len);
		pr_info("[NetF] Packet Data Len is: %u\n", skb->data_len);
		pr_info("[NetF] Packet MAC Data Len is: %hu\n", skb->mac_len);
		pr_info("[NetF] Lucky number: %d\n",
				container_of(ops, struct
					netF, nfho)->lucky_number);
	}
	return NF_ACCEPT;
}

static int __init NetF_init(void)
{
	pr_info("[Netf] Setting nf_hook_ops parameters\n");

	/* Fill in our hook structure */
	parham.lucky_number = 3;
	parham.nfho.hook = filter;
	parham.nfho.hooknum = 3;
	parham.nfho.owner = THIS_MODULE;
	parham.nfho.pf = PF_INET;
	nf_register_hook(&parham.nfho);

	pr_info("[NetF] Our nf_hook_ops registered\n");
	return 0;
}

static void __exit NetF_exit(void)
{
	nf_unregister_hook(&parham.nfho);
	pr_info("[NetF] Our nf_hook_ops unregistered\n");
}

module_init(NetF_init);
module_exit(NetF_exit);
