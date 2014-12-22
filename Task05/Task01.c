#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani");
MODULE_DESCRIPTION("This module does nothing .....");

static int __init hello_init(void)
{
	pr_info("Hello, i am loading\n");
	pr_debug(KERN_DEBUG "Hello World\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, i am unloading\n");
}

module_init(hello_init);
module_exit(hello_exit);
