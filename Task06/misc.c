/*
 * In The Name Of God
 * ========================================
 * [] File Name : misc.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Sat 10 Jan 2015 06:28:56 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/string.h>	/* strlen() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */

int misc_major = 0;

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

/*
 * Data management: read and write
*/
ssize_t misc_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	return simple_read_from_buffer(buf, count, f_pos,
			"8d7990499d47\n", strlen("8d7990499d47\n"));
}

ssize_t misc_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	int retval = 0;
	const char idstr[] = "8d7990499d47";
	const int idlen = strlen(idstr);
	char input_id[idlen + 1];

	*f_pos = 0;
	retval = simple_write_to_buffer(input_id, idlen + 1,
			f_pos, buf, count);
	if (retval < 0)
		return retval;

	input_id[retval] = 0;

	if (strcmp(idstr, input_id))
		return -EINVAL;

	return retval;
}

const struct file_operations misc_fops = {
	.owner =	THIS_MODULE,
	.read =		misc_read,
	.write =	misc_write,
};

/*
 * Finally, the module stuff
*/

void __exit misc_cleanup_module(void)
{
	/* cleanup_module is never called if registering failed */
	unregister_chrdev(misc_major, "misc");
}

int __init misc_init_module(void)
{
	misc_major = register_chrdev(misc_major, "misc", &misc_fops);
	if (misc_major < 0) {
		pr_warn("MISC: can't get major %d\n", misc_major);
		return misc_major;
	}

	pr_info("MISC: major: %d allocated\n", misc_major);

	return 0;
}

module_init(misc_init_module);
module_exit(misc_cleanup_module);
