#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/types.h>
//#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("Dual BSD/GPL");

#define MODULE_NAME "hello"
dev_t dev;

static char *whom = "world";
static int howmany = 1;

module_param(howmany, int , S_IRUGO);
module_param(whom, charp , S_IRUGO);

int hello_major = 0;
int hello_minor = 0;
int hello_nr_devs = 1;

static int __init hello_init(void)
{
	int i = 0;
	int ret = 0; 
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Hello, %s.\n", whom);

	printk(KERN_INFO "The process is \"%s\" (pid %i) \n", current->comm, current->pid);
	printk(KERN_INFO "hello_major: %d.\n", hello_major);

	if (hello_major) {
		dev = MKDEV(hello_major, hello_minor);	
		ret = register_chrdev_region(dev, hello_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "hello: cannot get major %d\n", hello_major);
			goto err;
		}
	} else {
		ret = alloc_chrdev_region(&dev, hello_minor, hello_nr_devs, MODULE_NAME);
		if( ret < 0 ) {
			printk(KERN_ERR "hello: cannot get major %d\n", hello_major);
			goto err;
		}

		hello_major = MAJOR(dev);
	}

	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(dev), MINOR(dev));
	ret = 0;

err:
	return ret;
}

static void __exit hello_exit(void)
{
	int i = 0;
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Goodbye, cruel %s.\n", whom);
	printk(KERN_INFO "major: %d, minor: %d.\n", MAJOR(dev), MINOR(dev));
	unregister_chrdev_region(dev, 1);
}

module_init(hello_init);
module_exit(hello_exit);
