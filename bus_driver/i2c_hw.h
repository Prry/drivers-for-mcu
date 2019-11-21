#ifndef _I2C_HW_H_
#define _I2C_HW_H_
#include "stm32f10x.h"

//i2c1 device
extern struct i2c_dev_device i2c1_dev;

#define	I2C1_SDA_PORT	GPIOB
#define	I2C1_SCL_PORT	GPIOB
#define	I2C1_SDA_PIN	GPIO_Pin_7
#define	I2C1_SCL_PIN	GPIO_Pin_6

extern void stm32f1xx_i2c_init(void);

#endif
