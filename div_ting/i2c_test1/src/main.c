#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <errno.h>
#include <string.h>
#include "icm20948.h"

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 200
/* 3000 msec = 3 sec */
#define PAUSE_TIME_MS 250

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	
	static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);
	if (!device_is_ready(dev_i2c.bus))
	{
		printk("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
		return;
	}
	
	int ret;
	uint8_t reg_value;

	/* Read and print who_am_i register (should always print 234 (0xEA)) */
	printk("Reading who_am_i register...\n");
	reg_value = read_reg(&dev_i2c, 0, REG_WHO_AM_I);
	printk("Register value: %hhu\n", reg_value);

	// Select Bank 0
	ret = select_bank(&dev_i2c, 0);
	if (ret != 0)
	{
		printk("Failed to select bank 0: #%d %s\n", ret, strerror(-ret));
		return;
	}

	// Configure PWR_MGMT_1 to disable sleep and use auto clock select
	printk("Configuring PWR_MGMT_1 to disable sleep and use auto clock select...\n");
	uint8_t config_pwr_mgmt_1[] = {REG_PWR_MGMT_1, SLEEP_DIS | CLKSEL_AUTO};
	ret = i2c_write_dt(&dev_i2c, config_pwr_mgmt_1, sizeof(config_pwr_mgmt_1));
	if (ret != 0)
	{
		printk("Failed to write to I2C device address %x at Reg. %x: %s\n", dev_i2c.addr, config_pwr_mgmt_1[0], strerror(-ret));
		return;
	}
	reg_value = read_reg(&dev_i2c, 0, REG_PWR_MGMT_1);
	printk("Register value: %d\n", reg_value);
	k_msleep(PAUSE_TIME_MS);

	// Configure PWR_MGMT_2 to enable gyroscope and disable accelerometer
	printk("Configuring PWR_MGMT_2 to enable gyroscope and disable accelerometer...\n");
	uint8_t config_pwr_mgmt_2[] = {REG_PWR_MGMT_2, GYRO_EN | ACCEL_DIS};
	ret = i2c_write_dt(&dev_i2c, config_pwr_mgmt_2, sizeof(config_pwr_mgmt_2));
	if (ret != 0)
	{
		printk("Failed to write to I2C device address %x at Reg. %x: %s\n", dev_i2c.addr, config_pwr_mgmt_2[0], strerror(-ret));
		return;
	}
	reg_value = read_reg(&dev_i2c, 0, REG_PWR_MGMT_2);
	printk("Register value: %d\n", reg_value);
	k_msleep(PAUSE_TIME_MS);

	// Select Bank 2 to configure gyro sensitivity
	ret = select_bank(&dev_i2c, 2);
	if (ret != 0)
	{
		printk("Failed to select bank 2: %s\n", strerror(-ret));
		return;
	}

	// Set the desired gyro full-scale range (e.g., ±1000 dps)
	printk("Setting the desired gyro full-scale range (±1000 dps)...\n");
	uint8_t gyro_config[2] = {REG_GYRO_CONFIG_1, GYRO_FS_SEL_1000DPS};
	ret = i2c_write_dt(&dev_i2c, gyro_config, sizeof(gyro_config));
	if (ret != 0)
	{
		printk("Failed to write to I2C device address %x at Reg. %x: %s\n", dev_i2c.addr, gyro_config[0], strerror(-ret));
		return;
	}
	reg_value = read_reg(&dev_i2c, 2, REG_GYRO_CONFIG_1);
	printk("Register value: %d\n", reg_value);
	k_msleep(PAUSE_TIME_MS);
	
	while (1)
	{
		// Select Bank 0 to read gyroscope data
		ret = select_bank(&dev_i2c, 0);

		uint8_t gyro_data[6];

		// Read gyroscope data (6 bytes)
		printk("Reading gyroscope data...\n");
		ret = i2c_burst_read_dt(&dev_i2c, REG_GYRO_XOUT_H, gyro_data, sizeof(gyro_data));
		if (ret != 0)
		{
			printk("Failed to read gyroscope data: %s\n", strerror(-ret));
			return;
		}

		// Combine higher and lower bytes for each axis
		int16_t gyro_xout = (gyro_data[0] << 8) | gyro_data[1];
		int16_t gyro_yout = (gyro_data[2] << 8) | gyro_data[3];
		int16_t gyro_zout = (gyro_data[4] << 8) | gyro_data[5];
		
		/* Print raw data */
		printk("\nGyro X-axis raw data: %d\n", gyro_xout);
		printk("Gyro Y-axis raw data: %d\n", gyro_yout);
		printk("Gyro Z-axis raw data: %d\n\n", gyro_zout);

		// Calculate the angular rate for the X-axis
		float gyro_sensitivity = 16.4;
		float x_angular_rate = (float)gyro_xout / gyro_sensitivity;
		float y_angular_rate = (float)gyro_yout / gyro_sensitivity;
		float z_angular_rate = (float)gyro_zout / gyro_sensitivity;

		printk("Gyro X-axis angular rate: %f\n", x_angular_rate);
		printk("Gyro Y-axis angular rate: %f\n", y_angular_rate);
		printk("Gyro Z-axis angular rate: %f\n\n", z_angular_rate);
		k_msleep(SLEEP_TIME_MS);
	}
}
