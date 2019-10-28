#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/reboot.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DRIVER_AUTHOR "Nguyen Minh Hoang <nmhoang31096@gmail.com>"
#define DRIVER_DECS "a sample character device driver"
#define DRIVER_VERSION "0.1"


static struct class *class_name;
static struct device *device_name;
static dev_t dev;// contains both major and minor number
static struct cdev *my_cdev;

// int(*open) (struct inode* , struct file*)
static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t  dev_read(struct file*, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

// first, is to fill in a file operation
static struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
};

static int dev_open(struct inode *inodep, struct file *filep)
{
	printk("open\n");
	return 0;
}
static int dev_close(struct inode *inodep, struct file *filep)
{
	printk("close\n");
	return 0;
}

static ssize_t dev_read(struct file* filep, char __user *buf, size_t len, loff_t *offset)
{
	printk("read\n");
	return 0;
}	


static ssize_t dev_write(struct file* filep, const char __user *buf, size_t len, loff_t *offset)
{
	printk("write\n");
	return 0;
}	

static int __init exam_init(void)
{
	int ret;
	ret = alloc_chrdev_region(&dev, 0, 1, "example");
	if(ret)
	{
		printk("can not register major no\n");
		return ret;
	}

	printk(KERN_INFO "register successfully major now is: %d\n", MAJOR(dev));

	my_cdev = cdev_alloc();
	my_cdev->owner = THIS_MODULE;
	my_cdev->ops = &fops;
	my_cdev->dev = dev;
	cdev_add(my_cdev, dev, 0);
	
	class_name = class_create(THIS_MODULE, "reboot");
	if(IS_ERR(class_name))
	{
		pr_info("create class failed\n");
		goto fail_reg;
	}

	pr_info("create successfully class\n");

	device_name = device_create(class_name, NULL, dev, NULL, "reboot");
	if(IS_ERR(device_name))
	{
		pr_info("create device failed\n");
		goto dev_fail;
	}
	pr_info("create device success\n");
	return 0;
dev_fail:
	cdev_del(my_cdev);
	class_destroy(class_name);
fail_reg:
	return -ENODEV;
}

static void __exit exam_exit(void)
{
	printk("goodbye\n");
	unregister_chrdev_region(dev, 0);
	cdev_del(my_cdev);
}

module_init(exam_init);
module_exit(exam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DECS);
MODULE_VERSION(DRIVER_VERSION);
MODULE_SUPPORTED_DEVICE("testdevice");
