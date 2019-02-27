#ifndef _I2C_BITOPS_H_
#define	_I2C_BITOPS_H_
#include "stdint.h"
#include "i2c_core.h"

//Ä£Äâi2cÉè±¸
struct ops_i2c_dev
{
	void (*set_sda)(int8_t state);
    void (*set_scl)(int8_t state);
    int8_t (*get_sda)(void);
    int8_t (*get_scl)(void);
    void (*delayus)(uint32_t us);
};

extern int i2c_bitops_bus_xfer(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message msgs[],unsigned long num);

#endif
