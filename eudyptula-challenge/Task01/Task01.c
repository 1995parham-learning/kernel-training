/* 
 * In The Name Of God
 * ========================================
 * [] File Name : Task01.c
 *
 * [] Creation Date : 21-12-2014
 *
 * [] Last Modified : Sun 21 Dec 2014 01:03:27 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani");
MODULE_DESCRIPTION("This module does nothing .....");

static int __init hello_init(void){
	printk(KERN_INFO "Hello, i am loading\n");
	printk(KERN_DEBUG "Hello World\n");
	return 0;
}

static void __exit hello_exit(void){
	printk(KERN_INFO "Goodbye, i am unloading\n");
}

module_init(hello_init);
module_exit(hello_exit);
