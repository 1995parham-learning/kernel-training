/*
 * In The Name Of God
 * ========================================
 * [] File Name : misc.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Fri 09 Jan 2015 05:57:08 AM IRST
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
#include <linux/cdev.h>
#include <asm/uaccess.h>        /* copy_*_user */

/*
 * Parameters which can be set at load time.
*/
int misc_major = 0;
int misc_minor = 0;

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

	retval = simple_write_to_buffer(input_id, idlen + 1,
			f_pos, buf, count);
	input_id[idlen] = 0;

	if (strcmp(idstr, input_id))
		return -EINVAL;
	else
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

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
*/
void __exit misc_cleanup_module(void)
{
	dev_t devno = MKDEV(misc_major, misc_minor);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);
}

int __init misc_init_module(void)
{
	misc_major = __register_chrdev(misc_major, misc_minor,
			1, "misc", &misc_fops);
	if (misc_major < 0) {
		pr_warn("MISC: can't get major %d\n", misc_major);
		return misc_major;
	}

	pr_info("MISC: major: %d , minor %d allocated\n",
			misc_major, misc_minor);

	return 0;
}

module_init(misc_init_module);
module_exit(misc_cleanup_module);
