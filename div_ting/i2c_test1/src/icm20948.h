#ifndef ICM20948_H
#define ICM20948_H

#include <zephyr/drivers/i2c.h>

/* REGISTERS */
#define REG_BANK_SEL 0x7F // Selects register bank
#define BANK_SEL_0			(0x00 << 4)
#define BANK_SEL_1			(0x01 << 4)
#define BANK_SEL_2			(0x02 << 4)
#define BANK_SEL_3			(0x03 << 4)

/* BANK 0 */
#define REG_WHO_AM_I		0x00
#define REG_USER_CTRL		0x03
#define REG_PWR_MGMT_1		0x06
	#define SLEEP_EN		(1 << 6)
	#define SLEEP_DIS		(0 << 6)
	#define CLKSEL_AUTO		0x01
#define REG_PWR_MGMT_2 		0x07
	#define ACCEL_EN 		(0x00 << 3)
	#define ACCEL_DIS 		(0x07 << 3)
	#define GYRO_EN 		(0x00 << 0)
	#define GYRO_DIS 		(0x07 << 0)
#define REG_GYRO_XOUT_H		0x33
#define REG_GYRO_XOUT_L		0x34
#define REG_GYRO_YOUT_H		0x35
#define REG_GYRO_YOUT_L		0x36
#define REG_GYRO_ZOUT_H		0x37
#define REG_GYRO_ZOUT_L		0x38

/* BANK 2 */
#define REG_GYRO_CONFIG_1 0x01
	#define GYRO_FS_SEL_250DPS 		(0x00 << 1)
	#define GYRO_FS_SEL_500DPS 		(0x01 << 1)
	#define GYRO_FS_SEL_1000DPS		(0x02 << 1)
	#define GYRO_FS_SEL_2000DPS		(0x03 << 1)
#define REG_GYRO_CONFIG_2 0x02

/* Nodes */
#define I2C0_NODE DT_NODELABEL(icm20948)

/* Prototypes */
int select_bank(const struct i2c_dt_spec *dev_i2c, uint8_t bank);
uint8_t read_reg(const struct i2c_dt_spec *dev_i2c, uint8_t bank, uint8_t reg);

#endif // ICM20948_H