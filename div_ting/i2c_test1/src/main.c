#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
}
