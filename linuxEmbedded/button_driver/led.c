#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Hoang");
MODULE_DESCRIPTION("Led kernel module");
MODULE_VERSION("0.1");

/* P8.8 = GPIO 2.3 */
#define LED	3

// gpio setup

#define GPIO2_BASE_START 		0x481AC000
#define GPIO2_BASE_STOP			0x481ACFFF
#define GPIO_TRQSTATUS_SET_0 		0x34
#define GPIO_IRQSTATUS			0X2C
#define GPIO_RISINGDETECT		0x148

#define GPIO_DATAIN	 		0x138
#define GPIO_DATAOUT	 		0x13C
#define GPIO_SETDATAOUT 		0x194
#define GPIO_CLEARDATAOUT 		0x190
#define GPIO_OE				0x134

//control mode 
#define P8_7_MODE_OFFSET 		0x890		// Khong anh huong
#define P8_8_MODE_OFFSET		0x894

#define CONTROL_MODE_START		0x44E10000
#define CONTROL_MODE_STOP		0x44E11FFF
#define INPUT_PULL_DOWN			0x27		//Pad Control Registers

static int gpio_setup(void);
static void __iomem *io;
struct timer_list g_timer;
int i = 0;

void my_timer_callback(unsigned long data)
{
	/*
	if(ioread32(io + GPIO_DATAIN) & 0x4)
	{
		printk("Nhan nut\n");
		
		if(i==0)
		{
			i = 1;
			iowrite32((1<<LED), (io + GPIO_CLEARDATAOUT));
		}
	}
	else
	{
		if(i==1)
		{
			i = 0;
			iowrite32((1<<LED), (io + GPIO_SETDATAOUT));
		}
	}*/
	mod_timer(&g_timer, jiffies + msecs_to_jiffies(1000));
}

static int gpio_setup(void)
{
	unsigned int temp = 0;
	/* set mode ouput & input function for p8.7 */
	io = ioremap(CONTROL_MODE_START, CONTROL_MODE_STOP-CONTROL_MODE_START);
	if(io == NULL)
	{
		pr_alert("error ioremap control mode\n");
		return -1;
	}
	
	iowrite32(0x37, (io + P8_7_MODE_OFFSET));
	iowrite32(0x27, (io + P8_8_MODE_OFFSET));
	
	// setup gpio
	io = ioremap(GPIO2_BASE_START, GPIO2_BASE_STOP - GPIO2_BASE_START);
	if(io == NULL)
	{
		pr_alert("error ioremap gpio setup\n");
		return -1;
	}

	//set mode output led
	
	// save state of this port
//	iowrite32(~0u, (io + GPIO_OE));
	temp = ioread32(io + GPIO_OE);
	printk("%x\n", temp);
	// enable output at pin LED
	temp &= ~(1 << LED);
	printk("%x\n", temp);

	iowrite32(temp, (io + GPIO_OE));
	
	temp = ioread32(io + GPIO_DATAIN);
	printk("%x\n", temp);
	return 0;
}

static int __init hello_init(void)
{
	pr_info("START\n");
	gpio_setup();
	setup_timer(&g_timer, my_timer_callback, 0);
	mod_timer(&g_timer, jiffies + msecs_to_jiffies(1000));
	return 0;
}

static void __exit hello_exit(void)
{
	iounmap(io);
	del_timer(&g_timer);
	pr_info("END\n");
}

module_init(hello_init);
module_exit(hello_exit);
