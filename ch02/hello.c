#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>

MODULE_LICENSE("Dual BSD/GPL");

static char *whom = "world";
static int howmany = 1;

module_param(howmany, int , S_IRUGO);
module_param(whom, charp , S_IRUGO);

enum val_state{
	INIT = 0,
	HELLO,
	GOODBYE,
};

static enum val_state val_1 = INIT;
static enum val_state val_2 = INIT;
static enum val_state val_3 = INIT;

enum val_state hello_1(void)
{
	if(val_1 == HELLO) {
		printk(KERN_ERR"[%s]Wrong val_1.\n",__func__);
		printk(KERN_ERR"[%s]val_1 : %d.\n", __func__, val_1);
		goto err;
	}

	val_1 = HELLO;

	return val_1;
err:
	return -EINVAL;
}

enum val_state hello_2(void)
{
	if(val_1 != HELLO) {
		printk(KERN_ERR"val_1 is not HELLO.\n");
		printk(KERN_ERR"val_1 : %d.\n", val_1);
		goto err;

	}

	if(val_2 == HELLO) {
		printk(KERN_ERR"Wrong val_2.\n");
		printk(KERN_ERR"val_2 : %d.\n", val_2);
		goto err;

	}

	val_2 = HELLO;

	return val_2;
err:
	return -EINVAL;
}

enum val_state hello_3(void)
{
	if(val_1 != HELLO) {
		printk(KERN_ERR"val_1 is not HELLO.\n");
		printk(KERN_ERR"val_1 : %d.\n", val_1);
		goto err;

	}

	if(val_2 != HELLO) {
		printk(KERN_ERR"val_2 is not HELLO.\n");
		printk(KERN_ERR"val_2 : %d.\n", val_2);
		goto err;

	}

	if(val_3 == HELLO) {
		printk(KERN_ERR"Wrong val_3.\n");
		printk(KERN_ERR"val_3 : %d.\n", val_2);
		goto err;

	}

	val_3 = HELLO;

	return val_3;
err:
	return -EINVAL;
}

enum val_state goodbye_1(void)
{
	if(val_1 != HELLO) {
		printk(KERN_ERR"val_1 is not HELLO.\n");
		printk(KERN_ERR"val_1 : %d.\n", val_1);
		goto err;

	}

	val_1 = GOODBYE;

	return val_1;
err:
	return -EINVAL;
}

enum val_state goodbye_2(void)
{
	if(val_1 != HELLO) {
		printk(KERN_ERR"val_1 is not HELLO.\n");
		printk(KERN_ERR"val_1 : %d.\n", val_1);
		goto err;

	}

	if(val_2 != HELLO) {
		printk(KERN_ERR"val_2 is not HELLO.\n");
		printk(KERN_ERR"val_2 : %d.\n", val_2);
		goto err;

	}

	val_2 = GOODBYE;

	return val_2;
err:
	return -EINVAL;
}

enum val_state goodbye_3(void)
{
	if(val_1 != HELLO) {
		printk(KERN_ERR"val_1 is not HELLO.\n");
		printk(KERN_ERR"val_1 : %d.\n", val_1);
		goto err;

	}

	if(val_2 != HELLO) {
		printk(KERN_ERR"val_2 is not HELLO.\n");
		printk(KERN_ERR"val_2 : %d.\n", val_2);
		goto err;

	}

	if(val_3 != HELLO) {
		printk(KERN_ERR"Wrong val_3.\n");
		printk(KERN_ERR"val_3 : %d.\n", val_2);
		goto err;

	}

	val_3 = GOODBYE;

	return val_3;
err:
	return -EINVAL;
}

static int __init hello_init(void)
{
	int i = 0;
	int ret = 0;
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Hello, %s.\n", whom);

	printk(KERN_INFO "The process is \"%s\" (pid %i) \n", current->comm, current->pid);

	ret = hello_1();
	printk(KERN_INFO "hello_1() returns %d \n", ret);
	ret = hello_2();
	printk(KERN_INFO "hello_2() returns %d \n", ret);
	ret = hello_3();
	printk(KERN_INFO "hello_3() returns %d \n", ret);

	return 0;
}

static void __exit hello_exit(void)
{
	int i = 0;
	int ret = 0;
	
	for( i = 0; i < howmany; i++)
		printk(KERN_ALERT"Goodbye, cruel %s.\n", whom);
	
	printk(KERN_INFO "The process is \"%s\" (pid %i) \n", current->comm, current->pid);
	ret = goodbye_3();
	printk(KERN_INFO "goodbye_3() returns %d \n", ret);
	ret = goodbye_2();
	printk(KERN_INFO "goodbye_2() returns %d \n", ret);
	ret = goodbye_1();
	printk(KERN_INFO "goodbye_1() returns %d \n", ret);
}

module_init(hello_init);
module_exit(hello_exit);
