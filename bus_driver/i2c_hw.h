#ifndef _I2C_HW_H_
#define _I2C_HW_H_

#include "stm32f10x.h"  
#include "i2c_core.h"

extern void stm32f1xx_i2c_init(struct i2c_dev_device *i2c_bus);

#endif
