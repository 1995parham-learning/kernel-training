#include <linux/module.h>
#include <linux/init.h>

static int __init hello_world_init(void) {
  pr_info("Hello world initiation\n");
}

static void __exit hello_world_exit(void) {
  pr_info("Hello world exit\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parham Alvani <parham.alvani@gmail.com>");
MODULE_DESCRIPTION("Linux kernel module skeleton");
