/*
 * In The Name Of God
 * ========================================
 * [] File Name : fifo.h
 *
 * [] Creation Date : 29-12-2014
 *
 * [] Last Modified : Mon 29 Dec 2014 05:39:34 AM IRST
 *
 * [] Created By : Parham Alvani (parham.alvani@gmail.com)
 * =======================================
*/
#ifndef FIFO_H
#define FIFO_H

/*
 * dynamic major by default
*/
#ifndef FIFO_MAJOR
#define FIFO_MAJOR 0
#endif

/*
 * set default buffer size
*/
#ifndef MAX_BUFF
#define MAX_BUFF 1024
#endif

/*
 * Split minors in two parts
 */
#define TYPE(minor)     (((minor) >> 4) & 0xf)  /* high nibble */
#define NUM(minor)      ((minor) & 0xf)         /* low  nibble */

struct fifo_node {
	struct fifo_node *next;
	int value;
};

/*
 * @data: pointer to first fifo_node
 * @fset: the current fifo size
 * @cdev: char device structure
*/
struct fifo_dev {
	struct fifo_node *data;
	int fset;
	struct cdev cdev;
};

/*
 * The different configurable parameters
 */
extern int fifo_major;     /* main.c */


/*
 * Prototypes for shared functions
 */
int fifo_open(struct inode *inode, struct file *filp);
int fifo_release(struct inode *inode, struct file *filp);
ssize_t fifo_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos);
ssize_t fifo_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos);

#endif

