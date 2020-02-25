
#include <stddef.h>
#include <string.h>
#include "lis3dh.h"
#include "i2c_core.h"

#define		LIS3DH_ADDRESS	0x18

#define 	WHO_AM_I		0x0F
#define		REG_CTRL1		0x20
#define		REG_CTRL2		0x21
#define		REG_CTRL3		0x22
#define		REG_CTRL4		0x23
#define		REG_CTRL5		0x24
#define		REG_CTRL6		0x25
#define		REG_STATUS		0x27
#define		REG_X_L			0x28
#define		REG_X_H			0x29
#define		REG_Y_L			0x2A
#define		REG_Y_H			0x2B
#define		REG_Z_L			0x2C
#define		REG_Z_H			0x2D


#define 	WHOAMI_LIS3DH_ACC	0x33	/* 器件ID */

extern struct i2c_dev_device i2c1_dev;	/* i2c总线设备 */

static char lis3dh_read_regs(uint8_t read_addr,uint8_t *read_buff,uint8_t read_size)
{
	struct i2c_dev_message lis3dh_msg[2];
	char ret;

	lis3dh_msg[0].addr  = LIS3DH_ADDRESS;
	lis3dh_msg[0].flags = I2C_BUS_WR;
	lis3dh_msg[0].buff  = &read_addr;
	lis3dh_msg[0].size  = 1;
	lis3dh_msg[1].addr  = LIS3DH_ADDRESS;
	lis3dh_msg[1].flags = I2C_BUS_RD;
	lis3dh_msg[1].buff  = read_buff;
	lis3dh_msg[1].size  = read_size;
	ret = i2c_bus_xfer(&i2c1_dev, lis3dh_msg, 2);

	return ret;
}

static char lm75a_write_regs(uint8_t write_addr,uint8_t *write_buff,uint8_t write_size)
{
	struct i2c_dev_message lis3dh_msg[2];
	char ret;

	lis3dh_msg[0].addr  = LIS3DH_ADDRESS;
	lis3dh_msg[0].flags = I2C_BUS_WR;
	lis3dh_msg[0].buff  = &write_addr;
	lis3dh_msg[0].size  = 1;
	lis3dh_msg[1].addr  = LIS3DH_ADDRESS;
	lis3dh_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;	
	lis3dh_msg[1].buff  = write_buff;
	lis3dh_msg[1].size  = write_size;
	ret = i2c_bus_xfer(&i2c1_dev,lis3dh_msg,2);

	return ret;
}	

static char lm75a_write_reg(uint8_t write_addr,uint8_t data)
{
	return lm75a_write_regs(write_addr, &data, 1);
}

static uint8_t lis3dh_config(void)
{	
	lm75a_write_reg(REG_CTRL1, 0x57);	/* 100Hz */
        
	lm75a_write_reg(REG_CTRL2, 0x00);
   
	lm75a_write_reg(REG_CTRL3, 0x00);
 
	lm75a_write_reg(REG_CTRL4, 0x28);	/* ±8g，小端模式 */

	lm75a_write_reg(REG_CTRL5, 0x00);

    return 0;
}

uint8_t lis3dh_read_xyz(uint16_t xyz[3])
{
	uint8_t	buff[6] = {0};
	
	lis3dh_read_regs(REG_X_L|0x80, buff, 6); /* 连续读时，寄存器最高位需置1 */

	xyz[0] = (buff[1]<<8) | buff[0];		 /* 最高位为符号位 */
	xyz[1] = (buff[3]<<8) | buff[2];
	xyz[2] = (buff[5]<<8) | buff[4];
	
	return 0;
}

uint8_t lis3dh_init(void)
{
	uint8_t	ret = false;	
	uint8_t	value = 0x00;
	
	lis3dh_read_regs(WHO_AM_I, &value, 1);
	if (value == WHOAMI_LIS3DH_ACC)
	{
		ret = lis3dh_config();
	}
	
	return ret;
}
