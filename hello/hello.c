#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

MODULE_AUTHOR("Nguyen Minh Hoang");

MODULE_DESCRIPTION("A simple hello world");

MODULE_VERSION("0.1");

static int __init hello_start(void)
{
	printk(KERN_INFO "Loading hello world...\n");
	printk(KERN_INFO "Hello world\n");
	return 0;
}

static void __exit hello_end(void)
{
	printk(KERN_INFO "Goodbye \n");
}

module_init(hello_start);
module_exit(hello_end);
