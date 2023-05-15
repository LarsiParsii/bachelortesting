#include <zephyr/drivers/i2c.h>
#include "icm20948.h"

int select_bank(const struct i2c_dt_spec *dev_i2c, uint8_t bank)
{
	uint8_t bank_value;
	//printk("Switching to Bank %u\n", bank);
	
	switch (bank)
	{
	case 0:
		bank_value = BANK_SEL_0;
		break;
	case 1:
		bank_value = BANK_SEL_1;
		break;
	case 2:
		bank_value = BANK_SEL_2;
		break;
	case 3:
		bank_value = BANK_SEL_3;
		break;
	default:
		return -EINVAL; // Invalid bank number
	}
	
	uint8_t config_bank_sel[] = {REG_BANK_SEL, bank_value};
	int ret = i2c_write_dt(dev_i2c, config_bank_sel, sizeof(config_bank_sel));
		if (ret != 0)
		{
			printk("Failed to select bank %d: %s\n", bank, strerror(-ret));
			return;
		}
	
	return ret;
}

uint8_t read_reg(const struct i2c_dt_spec *dev, uint8_t bank, uint8_t reg)
{
    // Select the bank
    int ret = select_bank(dev, bank);
    if (ret != 0)
    {
        printk("Failed to select bank %d: %s\n", bank, strerror(-ret));
        return 0;
    }
	
    // Read the register value
    uint8_t reg_value;
	ret = i2c_write_read_dt(dev, &reg, 1, &reg_value, 1);
	if (ret != 0)
    {
        printk("Failed to read register 0x%02x in bank %d: %s\n", reg, bank, strerror(-ret));
        return 0;
    }
	
    return reg_value;
}
