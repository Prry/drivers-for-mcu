/*
 * File      : lm75a.c
 * 
 * This file is part of lm75a driver.
 * COPYRIGHT (C) 2016-
 *
 * Depend on:
 * i2c_core.c,i2c_hw.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-27     Acuity      first version.
 */
#include "i2c_core.h"
#include "i2c_hw.h"
#include "lm75a.h"

//read lm75a register
static char lm75a_read_reg(u8 read_addr,u8 *read_buff,u8 read_size)
{
		struct i2c_dev_message lm75a_msg[2];
		char ret;
	
		lm75a_msg[0].addr  = LM75A_ADDR;
		lm75a_msg[0].flags = I2C_BUS_WR;
		lm75a_msg[0].buff  = &read_addr;
		lm75a_msg[0].size  = 1;
		lm75a_msg[1].addr  = LM75A_ADDR;
		lm75a_msg[1].flags = I2C_BUS_RD;
		lm75a_msg[1].buff  = read_buff;
		lm75a_msg[1].size  = read_size;
		ret = i2c_bus_xfer(&i2c1_dev,lm75a_msg,2);
	
		return ret;
}

//write lm75a register
static char lm75a_write_reg(u8 write_addr,u8 *write_buff,u8	write_size)
{
		struct i2c_dev_message lm75a_msg[2];
		char ret;

		lm75a_msg[0].addr  = LM75A_ADDR;
		lm75a_msg[0].flags = I2C_BUS_WR;
		lm75a_msg[0].buff  = &write_addr;
		lm75a_msg[0].size  = 1;
		lm75a_msg[1].addr  = LM75A_ADDR;
		lm75a_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;	
		lm75a_msg[1].buff  = write_buff;
		lm75a_msg[1].size  = write_size;
		ret = i2c_bus_xfer(&i2c1_dev,lm75a_msg,2);
	
		return ret;
}	

//init
void lm75a_init(void)
{
	//null		
}

//get temp from lm75a
float lm75a_get_temp(void)
{
		u8 temp_buff[2];
		short temp_data;
		float temp_out;
		
		lm75a_read_reg(REG_TEMP,temp_buff,2);
		temp_data = (temp_buff[0] << 3) | (temp_buff[1] >> 5);
		if(temp_data & 0x400)
		{//¸ºÎÂ¶È
				temp_data = ~temp_data + 1;
				temp_data &= 0x7ff;
				temp_data = -temp_data;
		}
		
		temp_out = temp_data * 0.125f;
		
		return temp_out;
}
