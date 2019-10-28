#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>

#define LED 14

//gpio setup
#define GPIO1_BASE_START		0x4804C000
#define GPIO1_BASE_STOP			0x4804CFFF

#define GPIO_DATAIN			0x138
#define GPIO_DATAOUT			0x13C
#define GPIO_SETDATAOUT			0x194
#define GPIO_CLEARDATAOUT		0x190
#define GPIO_OE				0x134

// control mode 
#define P8_16_MODE_OFFSET		0x838
#define CONTROL_MODE_START		0x44E10000
#define CONTROL_MODE_STOP		0x44E11FFF
#define INPUT_PULL_DOWN			0x27

static int gpio_setup(void);
static void __iomem *io;
static void led_on(void);
static void led_off(void);
static dev_t dev;
static struct cdev c_cdev;
static struct class *class_name;

static int dev_open(struct inode *, struct file *);
static int dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
char data[100];

//gpio setup function
static int gpio_setup(void)
{
	unsigned int temp = 0;
	io = ioremap(CONTROL_MODE_START, CONTROL_MODE_STOP - CONTROL_MODE_START);
	if(io == NULL)
	{
		pr_alert("error iomap control mode\n");
		return -1;
	}

	iowrite32(0x27, (io + P8_16_MODE_OFFSET));

	//set up gpio
	io = ioremap(GPIO1_BASE_START, GPIO1_BASE_STOP - GPIO1_BASE_START);
	if(io == NULL)
	{
		pr_alert("error iomap gpio setup\n");
		return -1;
	}

	iowrite32(~0u, (io + GPIO_OE));	
	temp &= ~(1 << LED);
	iowrite32(temp, (io + GPIO_OE));
	return 0;
}

static void led_on(void)
{
	iowrite32((1 << LED), (io + GPIO_SETDATAOUT));
}

static void led_off(void)
{
	iowrite32((1 << LED), (io + GPIO_CLEARDATAOUT));
}

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

static ssize_t dev_write(struct file *filep,const char __user *buf, size_t len, loff_t *offset)
{
	unsigned long cp;
	int r;
	int number;

	memset(data, 0, 100);
	cp = copy_from_user(data, buf, len);
	r = kstrtoint(data,10, &number);
	if(1 == number)
	{
		led_on();
	}
	else if(0 == number)
	{
		led_off();
	}
	printk("number: %d\n", number);
	return len;
}

static int __init exam_init(void)
{
	int ret;
	struct device *dev_ret;

	printk("init \n");
	//gpio setup
	gpio_setup();

	if((ret = alloc_chrdev_region(&dev, 0, 1, "Hoang")) < 0)
	{
		return ret;
	}

	if(IS_ERR(class_name = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(dev,1);
		return PTR_ERR(class_name);
	}
	
	if(IS_ERR(dev_ret = device_create(class_name, NULL, dev, NULL, "mycontrolLED")))
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
	iowrite32((1 << LED), (io + GPIO_CLEARDATAOUT));
	cdev_del(&c_cdev);
	device_destroy(class_name, dev);
	class_destroy(class_name);
	unregister_chrdev_region(dev, 1);
	iounmap(io);
	printk("end\n");
}

module_init(exam_init);
module_exit(exam_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Hoang");
MODULE_DESCRIPTION("Dev character driver");
