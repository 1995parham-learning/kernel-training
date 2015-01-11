/*
 * In The Name Of God
 * ========================================
 * [] File Name : misc.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Sun 11 Jan 2015 11:17:54 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/string.h>	/* strlen() */
#include <linux/fs.h>           /* everything... */
#include <linux/miscdevice.h>	/* misc things.. */
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

struct miscdevice misc_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &misc_fops,
	.nodename = "eudyptula",
};

/*
 * Finally, the module stuff
*/

void __exit misc_cleanup_module(void)
{
	misc_deregister(&misc_misc);
}

int __init misc_init_module(void)
{
	return misc_register(&misc_misc);
}

module_init(misc_init_module);
module_exit(misc_cleanup_module);
