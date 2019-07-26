#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/types.h>
//#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>


MODULE_LICENSE("Dual BSD/GPL");

#define MODULE_NAME "scull"
dev_t dev;
struct cdev scull_cdev;

static char *whom = "world";
static int howmany = 1;

module_param(howmany, int , S_IRUGO);
module_param(whom, charp , S_IRUGO);

int scull_major = 0;
int scull_minor = 0;
int scull_nr_devs = 1;

loff_t scull_llseek(struct file *filp, loff_t offset, int whence) {

	printk(KERN_INFO"[%s]\n", __func__);
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos) {

	printk(KERN_INFO"[%s]\n", __func__);
	return size;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos) {

	printk(KERN_INFO"[%s]\n", __func__);
	return size;
}

long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

	printk(KERN_INFO"[%s]\n", __func__);
	return 0;
}

int scull_open(struct inode *inode, struct file *filp) {

	printk(KERN_INFO"[%s]\n", __func__);

	printk(KERN_INFO "imajor(): %u\n", imajor(inode));
	printk(KERN_INFO "iminor(): %u\n", iminor(inode));

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

static int __init scull_init(void)
{
	int i = 0;
	int ret = 0; 
	/*
	struct cdev *scull_cdev;
	*/

	printk(KERN_INFO"[%s]\n", __func__);
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Hello, %s.\n", whom);

	printk(KERN_INFO "The process is \"%s\" (pid %i) \n", current->comm, current->pid);
	printk(KERN_INFO "scull_major: %d.\n", scull_major);

	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);	
		ret = register_chrdev_region(dev, scull_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "scull: cannot get major %d\n", scull_major);
			goto err;
		}
	} else {
		ret = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "scull: cannot get major %d\n", scull_major);
			goto err;
		}

		scull_major = MAJOR(dev);
	}

	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(dev), MINOR(dev));

	/*
	 *
	scull_cdev = cdev_alloc();
	if(scull_cdev) {
		scull_cdev.ops = &scull_fops;
		scull_cdev.owner = THIS_MODULE;
	}
		
	*/
	cdev_init(&scull_cdev, &scull_fops);
	scull_cdev.owner = THIS_MODULE;
	ret = cdev_add(&scull_cdev, dev, 1);
	if(ret < 0) {
		printk(KERN_ERR"cdev_add() failed.\n");
		goto err;
	}

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
	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(dev), MINOR(dev));

	cdev_del(&scull_cdev);
	unregister_chrdev_region(dev, 1);
}

module_init(scull_init);
module_exit(scull_exit);
