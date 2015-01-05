/*
 * In The Name Of God
 * ========================================
 * [] File Name : misc.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Mon 05 Jan 2015 07:36:56 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/cdev.h>
#include <asm/uaccess.h>        /* copy_*_user */

/*
 * Parameters which can be set at load time.
 */
int misc_major = 0;
int misc_minor = 0;

struct cdev dev;

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

/*
 * Open and close
*/
int misc_open(struct inode *inode, struct file *filp)
{
	pr_info("MISC: device opened\n");

	return 0;
}

int misc_release(struct inode *inode, struct file *flip)
{
	return 0;
}

/*
 * Data management: read and write
*/
ssize_t misc_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	int retval = 0;
	char numstr[10];
	int numlen = 0;
	
	if (*f_pos != 0)
		return -EINVAL;

	numlen = sprintf(numstr, "%d\n", misc_major);
	if (copy_to_user(buf, numstr, numlen)) {
		retval = -EFAULT;
		goto out;
	}
	retval = numlen;
out:
	return retval;
}

ssize_t misc_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	int retval = 0;

	pr_info("MISC: start writing ...\n");
	/*if (copy_from_user(&new->value, buf + i, 1)) {
		retval = -EFAULT;
		goto out;
	}*/
out:
	return retval;
}

const struct file_operations misc_fops = {
	.owner =	THIS_MODULE,
	.read =		misc_read,
	.write =	misc_write,
	.open =		misc_open,
	.release =	misc_release
};

/*
 * Finally, the module stuff
*/

void misc_setup_cdev(void)
{
	int err;
	dev_t devno = MKDEV(misc_major, misc_minor);

	cdev_init(&dev, &misc_fops);
	dev.owner = THIS_MODULE;
	dev.ops = &misc_fops;
	err = cdev_add(&dev, devno, 1);
	if (err)
		pr_warn("MISC: error %d adding lifo", err);
	else
		pr_info("MISC: cdev registeration completed");
}

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
*/
void __exit misc_cleanup_module(void)
{
	dev_t devno = MKDEV(misc_major, misc_minor);

	cdev_del(&dev);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);
}

int __init misc_init_module(void)
{
	int result;
	dev_t devno = 0;

	result = alloc_chrdev_region(&devno, misc_minor, 1, "misc");
	misc_major = MAJOR(devno);

	if (result < 0) {
		pr_warn("MISC: can't get major %d\n", misc_major);
		return result;
	}

	pr_info("MISC: major: %d , minor %d allocated\n", misc_major, misc_minor);

	misc_setup_cdev();

	return 0;
}

module_init(misc_init_module);
module_exit(misc_cleanup_module);
