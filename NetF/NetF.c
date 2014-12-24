/*
 * In The Name Of God
 * ========================================
 * [] File Name : Task01.c
 *
 * [] Creation Date : 21-12-2014
 *
 * [] Last Modified : Thu 25 Dec 2014 02:24:58 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
/*
 * NF_IP_PRE_ROUTING = 0
 * NF_IP_LOCAL_IN = 1
 * NF_IP_FORWARD = 2
 * NF_IP_LOCAL_OUT = 3
 * WE LOVE U Dr.Bakshi !!!
*/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani");
MODULE_DESCRIPTION("This module is netfilter patch");

static struct nf_hook_ops nfho;

unsigned int hook_func(const struct nf_hook_ops *ops,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	pr_info("[NetF] %s\n", out->name);
	return NF_ACCEPT;
}

static int __init NetF_init(void)
{
	pr_info("[Netf] Setting nf_hook_ops parameters\n");

	/* Fill in our hook structure */
	nfho.hook = hook_func;
	nfho.hooknum = 3;
	nfho.owner = THIS_MODULE;
	nfho.pf = PF_INET;
	nf_register_hook(&nfho);

	pr_info("[NetF] Our nf_hook_ops registered\n");
	return 0;
}

static void __exit NetF_exit(void)
{
	nf_unregister_hook(&nfho);
	pr_info("[NetF] Our nf_hook_ops unregistered\n");
}

module_init(NetF_init);
module_exit(NetF_exit);
