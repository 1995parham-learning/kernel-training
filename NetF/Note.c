/*
 * In The Name Of God
 * ========================================
 * [] File Name : Note.c
 *
 * [] Creation Date : 25-12-2014
 *
 * [] Last Modified : Thu 25 Dec 2014 02:31:01 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/

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
