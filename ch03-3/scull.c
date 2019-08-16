#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/types.h>
//#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include <linux/slab.h>

#define SCULL_QUANTUM 4000
#define SCULL_QSET    1000

MODULE_LICENSE("Dual BSD/GPL");

#define MODULE_NAME "scull"
dev_t devno;

static char *whom = "world";
static int howmany = 1;

module_param(howmany, int , S_IRUGO);
module_param(whom, charp , S_IRUGO);

struct scull_qset {
	void **data;
	struct scull_qset *next;
};

struct scull_dev {
	struct scull_qset *data;
	int quantum;
	int qset;
	unsigned long size;
	unsigned int access_key;
	struct semaphore sem;
	struct cdev cdev;
};

struct scull_dev scull_dev;
int scull_major = 0;
int scull_minor = 0;
int scull_nr_devs = 1;
int scull_quantum = SCULL_QUANTUM;
int scull_qset = SCULL_QSET;

struct scull_qset *scull_follow(struct scull_dev *dev, int item)
{
	struct scull_qset *qs = dev->data;

	if(!qs) {
		qs = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
		if(!qs)
			return NULL;

		memset(qs, 0, sizeof(struct scull_qset));
	}

	while(item--) {
		if(!qs->next) {
			qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
			if(qs->next == NULL)
				return NULL;

			memset(qs->next, 0, sizeof(struct scull_qset));
		}
		qs = qs->next;
	}

	return qs;
}

int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;
	int i;

	for(dptr = dev->data; dptr; dptr = next) {
		if(dptr->data) {
			for(i = 0; i < qset; i++)
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

loff_t scull_llseek(struct file *filp, loff_t offset, int whence) {

	printk(KERN_INFO"[%s]\n", __func__);
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {

	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	printk(KERN_INFO"[%s]\n", __func__);

	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if(*f_pos >= dev->size)
		goto out;
	if(*f_pos + count > dev->size)
		count = dev->size - *f_pos;

	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = scull_follow(dev, item);

	if(dptr == NULL || !dptr->data || !dptr->data[s_pos])
		goto out;

	if(count > quantum - q_pos)
		count = quantum - q_pos;

	if(copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;
out:
	up(&dev->sem);
	return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {

	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM;

	printk(KERN_INFO"[%s]\n", __func__);

	if(down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	
	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos % itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = scull_follow(dev, item);
	if(dptr == NULL)
		goto out;
	if(!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
		if(!dptr->data)
			goto out;
		memset(dptr->data, 0, qset * sizeof(char *));
	}

	if(!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
		if(!dptr->data[s_pos])
			goto out;
	}

	if(count > quantum - q_pos)
		count = quantum - q_pos;
	
	if(copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
		retval = -EFAULT;
		goto out;
	}

	*f_pos += count;
	retval = count;

	if(dev->size < *f_pos)
		dev->size = *f_pos;
out:
	up(&dev->sem);
	return retval;
}

long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

	printk(KERN_INFO"[%s]\n", __func__);
	return 0;
}

int scull_open(struct inode *inode, struct file *filp) {

	struct scull_dev *sdev;

	printk(KERN_INFO"[%s]\n", __func__);

	printk(KERN_INFO "imajor(): %u\n", imajor(inode));
	printk(KERN_INFO "iminor(): %u\n", iminor(inode));

	sdev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = sdev;

	if((filp->f_flags & O_ACCMODE) == O_WRONLY)
		scull_trim(sdev);

	return 0;
}

int scull_release(struct inode *inode, struct file *filp) {

	printk(KERN_INFO"[%s]\n", __func__);
	return 0;
}

struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.compat_ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};


static int scull_setup_cdev(struct scull_dev *pdev, int index) {
	
	int ret = 0;
	int local_devno;

	if( index >= scull_nr_devs ) {
		printk(KERN_ERR"invalid index.\n");
		ret = -EINVAL;
		goto err;
	}

	local_devno = MKDEV(scull_major, scull_minor+index);
	cdev_init(&pdev->cdev, &scull_fops);
	pdev->cdev.owner = THIS_MODULE;
	ret = cdev_add(&pdev->cdev, local_devno, 1);
	if(ret < 0) {
		printk(KERN_ERR"cdev_add() failed.\n");
		goto err;
	}
err:
	return ret;	
}

static int __init scull_init(void)
{
	int i = 0;
	int ret = 0; 

	printk(KERN_INFO"[%s]\n", __func__);
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Hello, %s.\n", whom);

	printk(KERN_INFO "The process is \"%s\" (pid %i) \n", current->comm, current->pid);
	printk(KERN_INFO "scull_major: %d.\n", scull_major);

	if (scull_major) {
		devno = MKDEV(scull_major, scull_minor);	
		ret = register_chrdev_region(devno, scull_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "scull: cannot get major %d\n", scull_major);
			goto err;
		}
	} else {
		ret = alloc_chrdev_region(&devno, scull_minor, scull_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "scull: cannot get major %d\n", scull_major);
			goto err;
		}

		scull_major = MAJOR(devno);
	}

	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(devno), MINOR(devno));
	ret = scull_setup_cdev(&scull_dev, 0);

	ret = 0;

err:
	return ret;
}

static void __exit scull_exit(void)
{
	int i = 0;
	
	printk(KERN_INFO"[%s]\n", __func__);

	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Goodbye, cruel %s.\n", whom);
	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(devno), MINOR(devno));

	scull_trim(&scull_dev);
	cdev_del(&scull_dev.cdev);
	unregister_chrdev_region(devno, scull_nr_devs);
}

module_init(scull_init);
module_exit(scull_exit);
