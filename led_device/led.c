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

/* P8.16 = GPIO 1[14] */
#define LED	14

// gpio setup

#define GPIO1_BASE_START 		0x4804C000
#define GPIO1_BASE_STOP			0x4804CFFF
#define GPIO_TRQSTATUS_SET_0 		0x34
#define GPIO_IRQSTATUS			0X2C
#define GPIO_RISINGDETECT		0x148

#define GPIO_DATAIN	 		0x138
#define GPIO_DATAOUT	 		0x13C
#define GPIO_SETDATAOUT 		0x194
#define GPIO_CLEARDATAOUT 		0x190
#define GPIO_OE				0x134

//control mode 
#define P8_16_MODE_OFFSET 		0x838		
#define CONTROL_MODE_START		0x44E10000
#define CONTROL_MODE_STOP		0x44E11FFF
#define INPUT_PULL_DOWN			0x27		//Pad Control Registers

static int gpio_setup(void);
static void __iomem *io;
static void led_on(void);
static void led_off(void);
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
	
	iowrite32(0x27, (io + P8_16_MODE_OFFSET));
	
	// setup gpio
	io = ioremap(GPIO1_BASE_START, GPIO1_BASE_STOP - GPIO1_BASE_START);
	if(io == NULL)
	{
		pr_alert("error ioremap gpio setup\n");
		return -1;
	}

	//set mode output led
	
	// save state of this port
//	iowrite32(~0u, (io + GPIO_OE));
	temp = ioread32(io + GPIO_OE);

	// enable output at pin LED
	temp &= ~(1 << LED);

	iowrite32(temp, (io + GPIO_OE));
	return 0;
}

static void led_on(void)
{
	iowrite32((1<<LED), (io + GPIO_SETDATAOUT));

}
static void led_off(void)
{
	iowrite32((1<<LED), (io + GPIO_CLEARDATAOUT));
}

static int __init hello_init(void)
{
	pr_info("Bat led P8.7\n");
	gpio_setup();
	
	led_on();
	//turn on led
//	iowrite32((1<<LED), (io + GPIO_SETDATAOUT));
	return 0;
}

static void __exit hello_exit(void)
{
	led_off();
	iounmap(io);
//	iowrite32((1<<LED), (io + GPIO_CLEARDATAOUT));
	pr_info("Tat led P8.7\n");
}

module_init(hello_init);
module_exit(hello_exit);
