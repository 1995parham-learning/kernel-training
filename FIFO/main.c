/*
 * In The Name Of God
 * ========================================
 * [] File Name : main.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Sat 03 Jan 2015 09:02:21 AM IRST
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

#include "fifo.h"              /* local definitions */

/*
 * Parameters which can be set at load time.
 */
int fifo_major = FIFO_MAJOR;
int fifo_minor = 0;

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

struct fifo_dev *main_device;

/*
 * Empty out fifo device.
*/
int fifo_trim(struct fifo_dev *dev)
{
	struct fifo_node *next, *ptr;

	for(ptr = dev->data; ptr, ptr = next) {
		next = ptr->next;
		kfree(ptr);
	}
	dev->fset = 0;
	dev->data = NULL;
	
	return 0;
}

/*
 * Open and close
 */
int fifo_open(struct inode *inode, struct file *filp)
{
	struct fifo_dev *dev;

	printk(KERN_INFO "fifo: device opened\n");
	dev = container_of(inode->i_cdev, struct fifo_dev, cdev);
	filp->private_data = dev;

	/*
	 * in user space we can get access flags directly
	 * but in here (kernel space) we must build it
	 * manually.
	*/
	if ((filp->f_flags & O_ACCMODE) == O_WRONLY)
		fifo_trim();

	return 0;
}

int fifo_release(struct inode *inode, struct file *flip)
{
	return 0;
}

/*
 * Data management: read and write
 */
ssize_t fifo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct fifo_dev *dev = filp->private_data;
	struct fifo_node *ptr = dev->data;
	struct fifo_node *next = NULL;


	ssize_t retval = 0;

	if (*f_pos != 0)
	return -EINVAL;
	
	if (copy_to_user(buf, queue + head, count)){
		retval = -EFAULT;
		goto out;
	}
out:
		return retval;
}

ssize_t fifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	ssize_t retval = 0;

	printk(KERN_INFO "fifo: start writing ...\n");
	if (copy_from_user(queue + tail, buf, count)) {
		retval = -EFAULT;
		goto out;
	}
	tail += count;
	retval = count;
out:
		return retval;
}

struct file_operations fifo_fops = {
	.owner =	THIS_MODULE,
	.read =		fifo_read,
	.write =	fifo_write,
	.open =		fifo_open,
	.release =	fifo_release
};

/*
 * Finally, the module stuff
*/

/*
 * Setup fifo_dev structure for device
 * we can use main_device instead of dev
 * but we don't !! ;-)))
*/
void fifo_setup_cdev(struct fifo_dev *dev)
{
	int err;
	dev_t devno = MKDEV(fifo_major, fifo_minor);

	cdev_init(&(dev->cdev), &fifo_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &fifo_fops;
	err = cdev_add(&(dev->cdev), devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding fifo", err);
}

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
*/
void __exit fifo_cleanup_module(void)
{
	dev_t devno = MKDEV(fifo_major, fifo_minor);

	if (main_device)
		cdev_del(&(main_device->cdev));

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, 1);
}

int __init fifo_init_module(void)
{
	int result;
	dev_t dev = 0;

	/*
	 * Get a range of minor numbers to work with, asking for a dynamic
	 * major unless directed otherwise at load time.
	 */
	if (fifo_major) {
		dev = MKDEV(fifo_major, fifo_minor);
		result = register_chrdev_region(dev, 1, "fifo");
	} else {
		result = alloc_chrdev_region(&dev, fifo_minor, 1, "fifo");
	}

	if (result < 0){
		printk(KERN_WARNING "fifo: can't get major %d\n", fifo_major);
		return result;
	}

	main_device = kmalloc(sizeof(struct fifo_dev), GFP_KERNEL);
	if (!main_device) {
		result = -ENOMEM;
		goto error;
	}
	memset(main_device, 0, sizeof(struct fifo_dev));

	fifo_setup_cdev(main_device);

	return 0;
error:
	fifo_cleanup_module();
	return result;
}

module_init(fifo_init_module);
module_exit(fifo_cleanup_module);
