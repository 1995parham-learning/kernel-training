/*
 * In The Name Of God
 * ========================================
 * [] File Name : lifo.h
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Sun 04 Jan 2015 07:58:00 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#ifndef LIFO_H
#define LIFO_H

/*
 * dynamic major by default
*/
#ifndef LIFO_MAJOR
#define LIFO_MAJOR 0
#endif

struct lifo_node {
	struct lifo_node *next;
	char value;
};

/*
 * @data: pointer to first fifo_node
 * @fset: the current fifo size
 * @cdev: char device structure
*/
struct lifo_dev {
	struct lifo_node *data;
	int fset;
	struct cdev cdev;
};

/*
 * The different configurable parameters
 */
extern int lifo_major;     /* main.c */


/*
 * Prototypes for shared functions
 */
int lifo_open(struct inode *inode, struct file *filp);
int lifo_release(struct inode *inode, struct file *filp);
ssize_t lifo_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos);
ssize_t lifo_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos);

#endif

