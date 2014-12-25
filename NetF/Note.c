/*
 * In The Name Of God
 * ========================================
 * [] File Name : Note.c
 *
 * [] Creation Date : 25-12-2014
 *
 * [] Last Modified : Thu 25 Dec 2014 08:12:11 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/

/* IP Hooks */
/* After promisc drops, checksum checks. */
#define NF_IP_PRE_ROUTING       0
/* If the packet is destined for this box. */
#define NF_IP_LOCAL_IN          1
/* If the packet is destined for another interface. */
#define NF_IP_FORWARD           2
/* Packets coming from a local process. */
#define NF_IP_LOCAL_OUT         3
/* Packets about to hit the wire. */
#define NF_IP_POST_ROUTING      4
#define NF_IP_NUMHOOKS          5


struct nf_hook_ops;
struct sk_buff;

typedef unsigned int nf_hookfn(const struct nf_hook_ops *ops,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff *));
struct nf_hook_ops {
	struct list_head list;
	
	/* User fills in from here down. */
	nf_hookfn       *hook;
	struct module   *owner;
	void            *priv;
	u_int8_t        pf;
	unsigned int    hooknum;
	
	/* Hooks are ordered in ascending priority. */
	int             priority;
};
