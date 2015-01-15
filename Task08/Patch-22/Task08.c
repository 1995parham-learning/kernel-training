/*
 * In The Name Of God
 * ========================================
 * [] File Name : Task08-2.c
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Thu 15 Jan 2015 08:13:26 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h>       /* printk() */
#include <linux/string.h>	/* strlen() */
#include <linux/fs.h>           /* everything... */
#include <linux/debugfs.h>	/* debugfs things.. */
#include <linux/jiffies.h>	/* jiffies timer things.. */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

static struct dentry *root;

/*
 * ID data management: read and write
*/
ssize_t task08_read_id(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	return simple_read_from_buffer(buf, count, f_pos,
			"8d7990499d47\n", strlen("8d7990499d47\n"));
}

ssize_t task08_write_id(struct file *filp, const char __user *buf, size_t count,
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

const struct file_operations task08_fops_id = {
	.owner =	THIS_MODULE,
	.read =		task08_read_id,
	.write =	task08_write_id,
};

/*
 * Jiffies data management: read and write
*/
ssize_t task08_read_jiffies(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
	char timestr[30];
	sprintf(timestr, "%lld\n", get_jiffies_64());
	return simple_read_from_buffer(buf, count, f_pos,
			timestr, strlen(timestr));
}

const struct file_operations task08_fops_jiffies = {
	.owner =	THIS_MODULE,
	.read =		task08_read_jiffies,
};

/*
 * Foo data management: read and write
*/
ssize_t task08_read_foo(struct file *filp, char __user *buf, size_t count,
		loff_t *f_pos)
{
}

ssize_t task08_write_foo(struct file *filp, const char __user *buf, size_t count,
		loff_t *f_pos)
{	
}

const struct file_operations task08_fops_id = {
	.owner =	THIS_MODULE,
	.read =		task08_read_foo,
	.write =	task08_write_foo,
};


/*
 * Finally, the module stuff
*/

void __exit task08_cleanup_module(void)
{
	debugfs_remove_recursive(root);
}

int __init task08_init_module(void)
{
	struct dentry *id = NULL;
	struct dentry *jiffies = NULL;
	struct dentry *foo = NULL;

	/* Create root directory */
	root = debugfs_create_dir("eudyptula", NULL);
	if (!root)
		return -1;
	if (root == -ENODEV)
		return -ENODEV;
	/* Create id file */
	id = debugfs_create_file("id", 0666, root, NULL, &task08_fops_id);
	if (!id)
		goto sub_error;
	/* Create jiffies file */
	jiffies = debugfs_create_file("jiffies", 0444, root, NULL, &task08_fops_jiffies);
	if (!jiffies)
		goto sub_error;
	/* Create foo file */
	foo = debugfs_create_file("foo", 0644, root, NULL, &task08_fops_foo);
	if (!foo)
		goto sub_error;

	return 0;

sub_error:
	task08_cleanup_module();
	return -1;
}

module_init(task08_init_module);
module_exit(task08_cleanup_module);
