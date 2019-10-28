#include <linux/module.h> //needed by all modules
#include <linux/kernel.h> // needed for kernel info
#include <linux/init.h>	// needed for the macros
#include <linux/timer.h>

struct timer_list g_timer;

void _TimerHandler(unsigned long data)
{
	// restarting the timer
	mod_timer(&g_timer, jiffies + msecs_to_jiffies(1000));
	printk(KERN_INFO "Timer handler called\n");
}

static int __init my_init(void)
{
	printk(KERN_INFO "My module inserted into kernel\n ");

	// starting the timer
	setup_timer(&g_timer, _TimerHandler, 0);
	mod_timer(&g_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static void __exit my_exit(void)
{
	del_timer(&g_timer);
	printk(KERN_INFO "My module exited from kernel \n");
}

module_init(my_init);
module_exit(my_exit);
