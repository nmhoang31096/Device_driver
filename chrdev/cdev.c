#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

static dev_t dev;
static struct cdev c_cdev;
static struct class *class_name;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

static const struct file_operations pugs_fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write
};


static int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("open is called\n");
	return 0;
}

static int dev_close(struct inode *inodep, struct file *filep)
{
	pr_info("close is called\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buf, size_t len, loff_t *offset)
{
	printk("read\n");
	return 0;
}
char data[100];
static ssize_t dev_write(struct file *filep,const char __user *buf, size_t len, loff_t *offset)
{
	memset(data, 0, 100);
	copy_from_user(data, buf, len);
	printk("write = %s", data);
	return len;
}

static int __init exam_init(void)
{
	int ret;
	struct device *dev_ret;

	printk("init \n");
	if((ret = alloc_chrdev_region(&dev, 0, 1, "Hoang")) < 0)
	{
		return ret;
	}

	if(IS_ERR(class_name = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(dev,1);
		return PTR_ERR(class_name);
	}
	
	if(IS_ERR(dev_ret = device_create(class_name, NULL, dev, NULL, "mynull")))
	{
		class_destroy(class_name);
		unregister_chrdev_region(dev, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_cdev, &pugs_fops);
	if((ret = cdev_add(&c_cdev, dev, 1)) < 0)
	{
		device_destroy(class_name, dev);
		class_destroy(class_name);
		unregister_chrdev_region(dev, 1);
		return ret;
	}

	return 0;

}

static void __exit exam_exit(void)
{
	cdev_del(&c_cdev);
	device_destroy(class_name, dev);
	class_destroy(class_name);
	unregister_chrdev_region(dev, 1);
	printk("end\n");
}

module_init(exam_init);
module_exit(exam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Hoang");
MODULE_DESCRIPTION("Dev character driver");
