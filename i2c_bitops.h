#ifndef _I2C_BITOPS_H_
#define	_I2C_BITOPS_H_
#include "stm32f10x.h"

//模拟i2c设备
struct ops_i2c_dev
{
		void (*set_sda)(int8_t state);
    void (*set_scl)(int8_t state);
    int8_t (*get_sda)(void);
    int8_t (*get_scl)(void);
    void (*delayus)(uint32_t us);
};
 
//发送一帧i2c消息
struct i2c_dev_message
{
		unsigned short 	addr;
		unsigned short	flags;
		unsigned short	size;
		unsigned char		*buff;
		unsigned char   retries;		
};

//i2c1 device
extern struct ops_i2c_dev i2c1_dev;
#define	I2C1_SDA_PORT		GPIOB
#define	I2C1_SCL_PORT	  GPIOB
#define	I2C1_SDA_PIN		GPIO_Pin_7
#define	I2C1_SCL_PIN		GPIO_Pin_6

#define I2C_BUS_WR                0x0000
#define I2C_BUS_RD               (1u << 0)
#define I2C_BUS_ADDR_10BIT       (1u << 2)  /* this is a ten bit chip address */
#define I2C_BUS_NO_START         (1u << 4)
#define I2C_BUS_IGNORE_NACK      (1u << 5)
#define I2C_BUS_NO_READ_ACK      (1u << 6)  /* when I2C reading, we do not ACK */

extern void i2c_bitops_bus_init(void);
extern int i2c_bitops_bus_xfer(struct ops_i2c_dev *i2c_bus,struct i2c_dev_message msgs[],unsigned long num);

#endif
