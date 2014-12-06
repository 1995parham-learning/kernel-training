#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
 
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>         /* kmalloc() */
#include <linux/fs.h>           /* everything... */
#include <linux/errno.h>        /* error codes */
#include <linux/types.h>        /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>

#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */
 
#include "scull.h"              /* local definitions */ 

/*
* Our parameters which can be set at load time.
*/
 
int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;      /* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;
 
module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Parham Alvani");
MODULE_LICENSE("GPL");

struct scull_dev *scull_devices;        /* allocated in scull_init_module */
 
 
/*
* Empty out the scull device; must be called with the device
* semaphore held.
*/
int scull_trim(struct scull_dev *dev){
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for(dptr = dev->data; dptr; dptr = next){ /* all the list items */
		if(dptr->data){
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
 		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}
 84 #ifdef SCULL_DEBUG /* use proc only if debugging */
 85 /*
 86  * The proc filesystem: function to read and entry
 87  */
 88 
 89 int scull_read_procmem(char *buf, char **start, off_t offset,
 90                    int count, int *eof, void *data)
 91 {
 92         int i, j, len = 0;
 93         int limit = count - 80; /* Don't print more than this */
 94 
 95         for (i = 0; i < scull_nr_devs && len <= limit; i++) {
 96                 struct scull_dev *d = &scull_devices[i];
 97                 struct scull_qset *qs = d->data;
 98                 if (down_interruptible(&d->sem))
 99                         return -ERESTARTSYS;
100                 len += sprintf(buf+len,"\nDevice %i: qset %i, q %i, sz %li\n",
101                                 i, d->qset, d->quantum, d->size);
102                 for (; qs && len <= limit; qs = qs->next) { /* scan the list */
103                         len += sprintf(buf + len, "  item at %p, qset at %p\n",
104                                         qs, qs->data);
105                         if (qs->data && !qs->next) /* dump only the last item */
106                                 for (j = 0; j < d->qset; j++) {
107                                         if (qs->data[j])
108                                                 len += sprintf(buf + len,
109                                                                 "    % 4i: %8p\n",
110                                                                 j, qs->data[j]);
111                                 }
112                 }
113                 up(&scull_devices[i].sem);
114         }
115         *eof = 1;
116         return len;
117 }
118 
119 
120 /*
121  * For now, the seq_file implementation will exist in parallel.  The
122  * older read_procmem function should maybe go away, though.
123  */
124 
125 /*
126  * Here are our sequence iteration methods.  Our "position" is
127  * simply the device number.
128  */
129 static void *scull_seq_start(struct seq_file *s, loff_t *pos)
130 {
131         if (*pos >= scull_nr_devs)
132                 return NULL;   /* No more to read */
133         return scull_devices + *pos;
134 }
135 
136 static void *scull_seq_next(struct seq_file *s, void *v, loff_t *pos)
137 {
138         (*pos)++;
139         if (*pos >= scull_nr_devs)
140                 return NULL;
141         return scull_devices + *pos;
142 }
143 
144 static void scull_seq_stop(struct seq_file *s, void *v)
145 {
146         /* Actually, there's nothing to do here */
147 }
148 
149 static int scull_seq_show(struct seq_file *s, void *v)
150 {
151         struct scull_dev *dev = (struct scull_dev *) v;
152         struct scull_qset *d;
153         int i;
154 
155         if (down_interruptible(&dev->sem))
156                 return -ERESTARTSYS;
157         seq_printf(s, "\nDevice %i: qset %i, q %i, sz %li\n",
158                         (int) (dev - scull_devices), dev->qset,
159                         dev->quantum, dev->size);
160         for (d = dev->data; d; d = d->next) { /* scan the list */
161                 seq_printf(s, "  item at %p, qset at %p\n", d, d->data);
162                 if (d->data && !d->next) /* dump only the last item */
163                         for (i = 0; i < dev->qset; i++) {
164                                 if (d->data[i])
165                                         seq_printf(s, "    % 4i: %8p\n",
166                                                         i, d->data[i]);
167                         }
168         }
169         up(&dev->sem);
170         return 0;
171 }
172         
173 /*
174  * Tie the sequence operators up.
175  */
176 static struct seq_operations scull_seq_ops = {
177         .start = scull_seq_start,
178         .next  = scull_seq_next,
179         .stop  = scull_seq_stop,
180         .show  = scull_seq_show
181 };
182 
183 /*
184  * Now to implement the /proc file we need only make an open
185  * method which sets up the sequence operators.
186  */
187 static int scull_proc_open(struct inode *inode, struct file *file)
188 {
189         return seq_open(file, &scull_seq_ops);
190 }
191 
192 /*
193  * Create a set of file operations for our proc file.
194  */
195 static struct file_operations scull_proc_ops = {
196         .owner   = THIS_MODULE,
197         .open    = scull_proc_open,
198         .read    = seq_read,
199         .llseek  = seq_lseek,
200         .release = seq_release
201 };
202         
203 
204 /*
205  * Actually create (and remove) the /proc file(s).
206  */
207 
208 static void scull_create_proc(void)
209 {
210         struct proc_dir_entry *entry;
211         create_proc_read_entry("scullmem", 0 /* default mode */,
212                         NULL /* parent dir */, scull_read_procmem,
213                         NULL /* client data */);
214         entry = create_proc_entry("scullseq", 0, NULL);
215         if (entry)
216                 entry->proc_fops = &scull_proc_ops;
217 }
218 
219 static void scull_remove_proc(void)
220 {
221         /* no problem if it was not registered */
222         remove_proc_entry("scullmem", NULL /* parent dir */);
223         remove_proc_entry("scullseq", NULL);
224 }
225 
226 
227 #endif /* SCULL_DEBUG */
228 
229 
230 
231 
232 
233 /*
234  * Open and close
235  */
236 
237 int scull_open(struct inode *inode, struct file *filp)
238 {
239         struct scull_dev *dev; /* device information */
240 
241         dev = container_of(inode->i_cdev, struct scull_dev, cdev);
242         filp->private_data = dev; /* for other methods */
243 
244         /* now trim to 0 the length of the device if open was write-only */
245         if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
246                 if (down_interruptible(&dev->sem))
247                         return -ERESTARTSYS;
248                 scull_trim(dev); /* ignore errors */
249                 up(&dev->sem);
250         }
251         return 0;          /* success */
252 }
253 
254 int scull_release(struct inode *inode, struct file *filp)
255 {
256         return 0;
257 }
258 /*
259  * Follow the list
260  */
261 struct scull_qset *scull_follow(struct scull_dev *dev, int n)
262 {
263         struct scull_qset *qs = dev->data;
264 
265         /* Allocate first qset explicitly if need be */
266         if (! qs) {
267                 qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
268                 if (qs == NULL)
269                         return NULL;  /* Never mind */
270                 memset(qs, 0, sizeof(struct scull_qset));
271         }
272 
273         /* Then follow the list */
274         while (n--) {
275                 if (!qs->next) {
276                         qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
277                         if (qs->next == NULL)
278                                 return NULL;  /* Never mind */
279                         memset(qs->next, 0, sizeof(struct scull_qset));
280                 }
281                 qs = qs->next;
282                 continue;
283         }
284         return qs;
285 }
286 
287 /*
288  * Data management: read and write
289  */
290 
291 ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
292                 loff_t *f_pos)
293 {
294         struct scull_dev *dev = filp->private_data; 
295         struct scull_qset *dptr;        /* the first listitem */
296         int quantum = dev->quantum, qset = dev->qset;
297         int itemsize = quantum * qset; /* how many bytes in the listitem */
298         int item, s_pos, q_pos, rest;
299         ssize_t retval = 0;
300 
301         if (down_interruptible(&dev->sem))
302                 return -ERESTARTSYS;
303         if (*f_pos >= dev->size)
304                 goto out;
305         if (*f_pos + count > dev->size)
306                 count = dev->size - *f_pos;
307 
308         /* find listitem, qset index, and offset in the quantum */
309         item = (long)*f_pos / itemsize;
310         rest = (long)*f_pos % itemsize;
311         s_pos = rest / quantum; q_pos = rest % quantum;
312 
313         /* follow the list up to the right position (defined elsewhere) */
314         dptr = scull_follow(dev, item);
315 
316         if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
317                 goto out; /* don't fill holes */
318 
319         /* read only up to the end of this quantum */
320         if (count > quantum - q_pos)
321                 count = quantum - q_pos;
322 
323         if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
324                 retval = -EFAULT;
325                 goto out;
326         }
327         *f_pos += count;
328         retval = count;
329 
330   out:
331         up(&dev->sem);
332         return retval;
333 }
334 
335 ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
336                 loff_t *f_pos)
337 {
338         struct scull_dev *dev = filp->private_data;
339         struct scull_qset *dptr;
340         int quantum = dev->quantum, qset = dev->qset;
341         int itemsize = quantum * qset;
342         int item, s_pos, q_pos, rest;
343         ssize_t retval = -ENOMEM; /* value used in "goto out" statements */
344 
345         if (down_interruptible(&dev->sem))
346                 return -ERESTARTSYS;
347 
348         /* find listitem, qset index and offset in the quantum */
349         item = (long)*f_pos / itemsize;
350         rest = (long)*f_pos % itemsize;
351         s_pos = rest / quantum; q_pos = rest % quantum;
352 
353         /* follow the list up to the right position */
354         dptr = scull_follow(dev, item);
355         if (dptr == NULL)
356                 goto out;
357         if (!dptr->data) {
358                 dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
359                 if (!dptr->data)
360                         goto out;
361                 memset(dptr->data, 0, qset * sizeof(char *));
362         }
363         if (!dptr->data[s_pos]) {
364                 dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
365                 if (!dptr->data[s_pos])
366                         goto out;
367         }
368         /* write only up to the end of this quantum */
369         if (count > quantum - q_pos)
370                 count = quantum - q_pos;
371 
372         if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
373                 retval = -EFAULT;
374                 goto out;
375         }
376         *f_pos += count;
377         retval = count;
378 
379         /* update the size */
380         if (dev->size < *f_pos)
381                 dev->size = *f_pos;
382 
383   out:
384         up(&dev->sem);
385         return retval;
386 }
387 
388 /*
389  * The ioctl() implementation
390  */
391 
392 int scull_ioctl(struct inode *inode, struct file *filp,
393                  unsigned int cmd, unsigned long arg)
394 {
395 
396         int err = 0, tmp;
397         int retval = 0;
398     
399         /*
400          * extract the type and number bitfields, and don't decode
401          * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
402          */
403         if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
404         if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;
405 
406         /*
407          * the direction is a bitmask, and VERIFY_WRITE catches R/W
408          * transfers. `Type' is user-oriented, while
409          * access_ok is kernel-oriented, so the concept of "read" and
410          * "write" is reversed
411          */
412         if (_IOC_DIR(cmd) & _IOC_READ)
413                 err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
414         else if (_IOC_DIR(cmd) & _IOC_WRITE)
415                 err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
416         if (err) return -EFAULT;
417 
418         switch(cmd) {
419 
420           case SCULL_IOCRESET:
421                 scull_quantum = SCULL_QUANTUM;
422                 scull_qset = SCULL_QSET;
423                 break;
424         
425           case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
426                 if (! capable (CAP_SYS_ADMIN))
427                         return -EPERM;
428                 retval = __get_user(scull_quantum, (int __user *)arg);
429                 break;
430 
431           case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
432                 if (! capable (CAP_SYS_ADMIN))
433                         return -EPERM;
434                 scull_quantum = arg;
435                 break;
436 
437           case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
438                 retval = __put_user(scull_quantum, (int __user *)arg);
439                 break;
440 
441           case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
442                 return scull_quantum;
443 
444           case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
445                 if (! capable (CAP_SYS_ADMIN))
446                         return -EPERM;
447                 tmp = scull_quantum;
448                 retval = __get_user(scull_quantum, (int __user *)arg);
449                 if (retval == 0)
450                         retval = __put_user(tmp, (int __user *)arg);
451                 break;
452 
453           case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
454                 if (! capable (CAP_SYS_ADMIN))
455                         return -EPERM;
456                 tmp = scull_quantum;
457                 scull_quantum = arg;
458                 return tmp;
459         
460           case SCULL_IOCSQSET:
461                 if (! capable (CAP_SYS_ADMIN))
462                         return -EPERM;
463                 retval = __get_user(scull_qset, (int __user *)arg);
464                 break;
465 
466           case SCULL_IOCTQSET:
467                 if (! capable (CAP_SYS_ADMIN))
468                         return -EPERM;
469                 scull_qset = arg;
470                 break;
471 
472           case SCULL_IOCGQSET:
473                 retval = __put_user(scull_qset, (int __user *)arg);
474                 break;
475 
476           case SCULL_IOCQQSET:
477                 return scull_qset;
478 
479           case SCULL_IOCXQSET:
480                 if (! capable (CAP_SYS_ADMIN))
481                         return -EPERM;
482                 tmp = scull_qset;
483                 retval = __get_user(scull_qset, (int __user *)arg);
484                 if (retval == 0)
485                         retval = put_user(tmp, (int __user *)arg);
486                 break;
487 
488           case SCULL_IOCHQSET:
489                 if (! capable (CAP_SYS_ADMIN))
490                         return -EPERM;
491                 tmp = scull_qset;
492                 scull_qset = arg;
493                 return tmp;
494 
495         /*
496          * The following two change the buffer size for scullpipe.
497          * The scullpipe device uses this same ioctl method, just to
498          * write less code. Actually, it's the same driver, isn't it?
499          */
500 
501           case SCULL_P_IOCTSIZE:
502                 scull_p_buffer = arg;
503                 break;
504 
505           case SCULL_P_IOCQSIZE:
506                 return scull_p_buffer;
507 
508 
509           default:  /* redundant, as cmd was checked against MAXNR */
510                 return -ENOTTY;
511         }
512         return retval;
513 
514 }
515 
516 
517 
518 /*
519  * The "extended" operations -- only seek
520  */
521 
522 loff_t scull_llseek(struct file *filp, loff_t off, int whence)
523 {
524         struct scull_dev *dev = filp->private_data;
525         loff_t newpos;
526 
527         switch(whence) {
528           case 0: /* SEEK_SET */
529                 newpos = off;
530                 break;
531 
532           case 1: /* SEEK_CUR */
533                 newpos = filp->f_pos + off;
534                 break;
535 
536           case 2: /* SEEK_END */
537                 newpos = dev->size + off;
538                 break;
539 
540           default: /* can't happen */
541                 return -EINVAL;
542         }
543         if (newpos < 0) return -EINVAL;
544         filp->f_pos = newpos;
545         return newpos;
546 }
 
 
 
struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	.llseek =   scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
	.ioctl =    scull_ioctl,
	.open =     scull_open,
	.release =  scull_release,
};

/*
* Finally, the module stuff
*/
 
/*
* The cleanup function is used to handle initialization failures as well.
* Thefore, it must be careful to work correctly even if some of the items
* have not been initialized
*/
void scull_cleanup_module(void){
571         int i;
572         dev_t devno = MKDEV(scull_major, scull_minor);
573 
574         /* Get rid of our char dev entries */
575         if (scull_devices) {
576                 for (i = 0; i < scull_nr_devs; i++) {
577                         scull_trim(scull_devices + i);
578                         cdev_del(&scull_devices[i].cdev);
579                 }
580                 kfree(scull_devices);
581         }
582 
583 #ifdef SCULL_DEBUG /* use proc only if debugging */
584         scull_remove_proc();
585 #endif
586 
587         /* cleanup_module is never called if registering failed */
588         unregister_chrdev_region(devno, scull_nr_devs);
589 
590         /* and call the cleanup functions for friend devices */
591         scull_p_cleanup();
592         scull_access_cleanup();
593 
594 }
595 
596 
/*
* Set up the char_dev structure for this device.
*/
static void scull_setup_cdev(struct scull_dev *dev, int index){
	int err, devno = MKDEV(scull_major, scull_minor + index);

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}
 
int scull_init_module(void){
	int result, i;
	dev_t dev = 0;
	/*
	* Get a range of minor numbers to work with, asking for a dynamic
	* major unless directed otherwise at load time.
	*/
	if(scull_major){
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	}else{
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
		scull_major = MAJOR(dev);
	}
	if(result < 0){
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	} 
	
	/* 
	* allocate the devices -- we can't have them static, as the number
	* can be specified at load time
	*/
	scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}
	memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));

	/* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
		scull_devices[i].quantum = scull_quantum;
		scull_devices[i].qset = scull_qset;
		init_MUTEX(&scull_devices[i].sem);
		scull_setup_cdev(&scull_devices[i], i);
	}
 
	/* At this point call the init function for any friend device */
	dev = MKDEV(scull_major, scull_minor + scull_nr_devs);
	dev += scull_p_init(dev);
	dev += scull_access_init(dev);

	#ifdef SCULL_DEBUG /* only when debugging */
		scull_create_proc();
	#endif

	return 0; /* succeed */

	fail:
		scull_cleanup_module();
		return result;
}
 
module_init(scull_init_module);
module_exit(scull_cleanup_module); 
