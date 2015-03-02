/*
 * In The Name Of God
 * ========================================
 * [] File Name : HAL.c
 *
 * [] Creation Date : 02-03-2015
 *
 * [] Last Modified : Mon 02 Mar 2015 04:43:40 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/init.h>     /* Needed for the macros */
#include <linux/mm.h>       /* Needed for show_mem */
#include <asm/cacheflush.h>
#include <linux/mm.h>


static char *user_data1 __initdata = "Hello World";
static int *user_data2 __initdata = 2;

static int __init starter(void)
{
	printk(KERN_INFO "Loading module - %s %d\n", user_data1, *user_data2);
	show_mem(1);
	return 0;
}

static void __exit ending(void)
{
	printk(KERN_INFO "Exiting module - Goodbye World\n");
}

module_init(starter);
module_exit(ending);
