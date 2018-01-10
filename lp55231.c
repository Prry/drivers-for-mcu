/*
 * File      : lp55231.c
 * 
 * This file is part of lp55231 driver.
 * COPYRIGHT (C) 2016
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-07-27     Acuity      first version.
 */
#include "lp55231.h"
#include "i2c_bitops.h"

#define USE_LP55231 1
#if USE_LP55231
//read lp55231 register
static char lp55231_read_reg(u8 lp55231_no,u8 read_addr,u8 *read_buff,u8 read_size)
{
		struct i2c_dev_message lp55231_msg[2];
		u8	buff[1];
		u8  slave_addr;
		char ret;
		
		slave_addr = LP55231_SLAVE0 + lp55231_no;
		buff[0] = read_addr;
		lp55231_msg[0].addr = slave_addr;
		lp55231_msg[0].flags = I2C_BUS_WR;
		lp55231_msg[0].buff  = buff;
		lp55231_msg[0].size  = 1;
		lp55231_msg[1].addr = slave_addr;
		lp55231_msg[1].flags = I2C_BUS_RD;
		lp55231_msg[1].buff  = read_buff;
		lp55231_msg[1].size  = read_size;
		ret = i2c_bitops_bus_xfer(&i2c1_dev,lp55231_msg,2);
	
		return ret;
}

//write lp55231 register
static char lp55231_write_reg0(u8 lp55231_no,u8 write_addr,u8	write_data)
{
		struct i2c_dev_message lp55231_msg[1];
		u8	buff[2];
		u8  slave_addr;
		char ret;
	
		slave_addr = LP55231_SLAVE0 + lp55231_no;
		buff[0] = write_addr;
		buff[1] = write_data;
		lp55231_msg[0].addr  = slave_addr;
		lp55231_msg[0].flags = I2C_BUS_WR;
		lp55231_msg[0].buff  = buff;
		lp55231_msg[0].size  = 2;
		ret = i2c_bitops_bus_xfer(&i2c1_dev,lp55231_msg,1);
	
		return ret;
}	

//write lp55231 register
static char lp55231_write_reg(u8 lp55231_no,u8 write_addr,u8 *write_buff,u8 write_size)
{
		struct i2c_dev_message ee24_msg[2];
		u8  slave_addr;
		char ret;

		slave_addr = LP55231_SLAVE0 + lp55231_no;
		ee24_msg[0].addr = slave_addr;
		ee24_msg[0].flags = I2C_BUS_WR;
		ee24_msg[0].buff  = &write_addr;
		ee24_msg[0].size  = 1;
		ee24_msg[1].addr = slave_addr;
		ee24_msg[1].flags = I2C_BUS_WR | I2C_BUS_NO_START;	//注意此标识
		ee24_msg[1].buff  = write_buff;
		ee24_msg[1].size  = write_size;
		ret = i2c_bitops_bus_xfer(&i2c1_dev,ee24_msg,2);
	
		return ret;
}

/****************外部函数********************/
//init
void lp55231_init(void)
{
		u8 i,j;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		//enable gpio
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
		GPIO_InitStructure.GPIO_Pin  	= GPIO_Pin_7;  			
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;	        //推挽输出 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		 GPIO_SetBits(GPIOC, GPIO_Pin_7);				//high enable lp55231																		 

		for(i = 0;i < 2;i++)
		{
				lp55231_write_reg0(i,REG_RESET,0xff);		//reset lp55231
				lp55231_write_reg0(i,REG_CNTRL1,0x40);	//enable engine
				//configure
				lp55231_write_reg0(i,REG_MISC,0x59);		//forced internal clock
				lp55231_write_reg0(i,REG_OUT_EN_MSB,0x01);	//turn on all led
				lp55231_write_reg0(i,REG_OUT_EN_LSB,0xff);
				//lp55231_write_reg0(i,REG_OUT_DIM_MSB,0x00);	//default enable	
				//lp55231_write_reg0(i,REG_OUT_DIM_LSB,0x00);
				for(j = 1;j < 10;j++)
				{		lp55231_set_led_current(i,j,0);			//max current 25.5mA
						lp55231_set_led_pwm(i,j,0);
				}
		}
}

//led_bit -> 注意每一位表示一个led,
void lp55231_set_led(u8 lp55231_no,u16 led_bit,u8 led_state)
{
		lp55231_write_reg0(lp55231_no,REG_OUT_EN_LSB,led_bit&0xff);	//D1-D8
		lp55231_write_reg0(lp55231_no,REG_OUT_EN_MSB,(led_bit>>8)&0x01);	//D9
}

/***********************************************************
 * Name			：lp55231_set_led_pwm
 * Function	：set led pwm to control the brightness of led
 * Input		：-
 * Output		: Null
 * notice   :led_no -> 1-9
 ***********************************************************/	
void lp55231_set_led_pwm(u8 lp55231_no,u8 led_no,u8 led_pwm)
{//REG_D1_PWM led1
		if(led_no > 9)
				led_no = 1;
		led_no -= 1;	//实际计算0——8
		lp55231_write_reg0(lp55231_no,REG_D1_PWM + led_no,led_pwm);
}

/***********************************************************
 * Name			：lp55231_set_led_current
 * Function	：set led current
 * Input		：-
 * Output		: Null
 * notice   :led_no -> 1-9
 ***********************************************************/
void lp55231_set_led_current(u8 lp55231_no,u8 led_no,u8 led_current)
{//REG_D1_CURRENT led1
		if(led_no > 9)
				led_no = 1;
		led_no -= 1;	//实际计算0——8
		lp55231_write_reg0(lp55231_no,REG_D1_CURRENT + led_no,led_current);
}


//
void light_control(u8 led_no,u8 bright)
{
		u8 led_dev,led_dev_no;
	
		led_dev = led_no/10;
		led_dev_no = (led_no % 10)+led_dev;
		if(bright == 0)
		{//close
				lp55231_set_led_current(led_dev,led_dev_no,bright);
				lp55231_set_led_current(led_dev,led_dev_no,bright);
				lp55231_set_led_pwm(led_dev,led_dev_no,bright);
				lp55231_set_led_pwm(led_dev,led_dev_no,bright);
		}
		else
		{//open
				lp55231_set_led_current(led_dev,led_dev_no,bright);
				lp55231_set_led_current(led_dev,led_dev_no,bright);
				lp55231_set_led_pwm(led_dev,led_dev_no,0xff);
				lp55231_set_led_pwm(led_dev,led_dev_no,0xff);
		}
}

#endif
