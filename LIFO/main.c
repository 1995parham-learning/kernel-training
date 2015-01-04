/*
 * In The Name Of God
 * ========================================
 * [] File Name : main.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Sun 04 Jan 2015 08:04:38 AM IRST
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
#include <linux/slab.h>		/* kamalloc kfree */
#include <linux/cdev.h>
#include <asm/uaccess.h>        /* copy_*_user */

#include "lifo.h"              /* local definitions */

/*
 * Parameters which can be set at load time.
 */
int lifo_major = LIFO_MAJOR;
int lifo_minor = 0;

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

struct lifo_dev *main_device;

/*
 * Empty out lifo device.
*/
int lifo_trim(struct lifo_dev *dev)
{
	struct lifo_node *next, *ptr;

	for (ptr = dev->data; ptr; ptr = next) {
		next = ptr->next;
		kfree(ptr);
	}
	dev->fset = 0;
	return 0;
}

/*
 * Open and close
 */
int lifo_open(struct inode *inode, struct file *filp)
{
	struct lifo_dev *dev;

	pr_info("LIFO: device opened\n");
	dev = container_of(inode->i_cdev, struct lifo_dev, cdev);
	filp->private_data = dev;

	/*
	 * in user space we can get access flags directly
	 * but in here (kernel space) we must build it
	 * manually.
	*/
	if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
		lifo_trim(dev);

	return 0;
}

int lifo_release(struct inode *inode, struct file *flip)
{
	return 0;
}

/*
 * Data management: read and write
 */
ssize_t lifo_read(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct lifo_dev *dev = filp->private_data;
	struct lifo_node *ptr = dev->data;
	struct lifo_node *next = NULL;

	ssize_t retval = 0;
	int i = 0;
	if (*f_pos != 0)
		return -EINVAL;
	if (dev->fset < count)
		count = dev->fset;
	for (i = 0; i < count; i++) {
		if (copy_to_user(buf + i, &ptr->value, 1)) {
			retval = -EFAULT;
			goto out;
		}
		
		retval++;
		dev->fset--;
		next = ptr->next;
		kfree(ptr);
		ptr = next;
	}
	dev->data = ptr;
out:
	return retval;
}

ssize_t lifo_write(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{
	struct lifo_dev *dev = filp->private_data;
	struct lifo_node *ptr = dev->data;
	struct lifo_node *new = NULL;

	ssize_t retval = 0;
	int i = 0;

	pr_info("LIFO: start writing ...\n");
	for (i = 0; i < count; i++) {
		new = kmalloc(sizeof(struct lifo_node), GFP_KERNEL);

		if (copy_from_user(&new->value, buf + i, 1)) {
			retval = -EFAULT;
			goto out;
		}

		retval++;
		dev->fset++;
		new->next = ptr;
		ptr = new;
	}
	dev->data = ptr;
out:
	return retval;
}

const struct file_operations lifo_fops = {
	.owner =	THIS_MODULE,
	.read =		lifo_read,
	.write =	lifo_write,
	.open =		lifo_open,
	.release =	lifo_release
};

/*
 * Finally, the module stuff
*/

/*
 * Setup lifo_dev structure for device
 * we can use main_device instead of dev
 * but we don't !! ;-)))
*/
void lifo_setup_cdev(struct lifo_dev *dev)
{
	int err;
	dev_t devno = MKDEV(lifo_major, lifo_minor);

	cdev_init(&(dev->cdev), &lifo_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &lifo_fops;
	err = cdev_add(&(dev->cdev), devno, 1);
	if (err)
		pr_warn("LIFO: error %d adding lifo", err);
	else
		pr_info("LIFO: cdev registeration completed");
}

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
*/
void __exit lifo_cleanup_module(void)
{
	dev_t devno = MKDEV(lifo_major, lifo_minor);

	if (main_device) {
		cdev_del(&(main_device->cdev));
		lifo_trim(main_device);
	}

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);
}

int __init lifo_init_module(void)
{
	int result;
	dev_t dev = 0;

	/*
	 * Get a range of minor numbers to work with, asking for a dynamic
	 * major unless directed otherwise at load time.
	 */
	if (lifo_major) {
		dev = MKDEV(lifo_major, lifo_minor);
		result = register_chrdev_region(dev, 1, "lifo");
	} else {
		result = alloc_chrdev_region(&dev, lifo_minor, 1, "lifo");
		lifo_major = MAJOR(dev);
	}

	if (result < 0) {
		pr_warn("LIFO: can't get major %d\n", lifo_major);
		return result;
	}

	pr_info("KIFO: major: %d , minor %d allocated\n", lifo_major, lifo_minor);

	main_device = kmalloc(sizeof(struct lifo_dev), GFP_KERNEL);
	if (!main_device) {
		result = -ENOMEM;
		goto error;
	}
	memset(main_device, 0, sizeof(struct lifo_dev));

	lifo_setup_cdev(main_device);

	return 0;
error:
	lifo_cleanup_module();
	return result;
}

module_init(lifo_init_module);
module_exit(lifo_cleanup_module);
