/*
 * File      : tmc2660.c
 * 
 * This file is part of tmc2660 driver for stm32f1xx.
 * COPYRIGHT (C) 2019-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-08-07     Acuity      first version.
 */
 
#include <string.h>
#include "tmc2660.h"

/* tmc2660 register */
#define		REG_DRVCTRL			0x00000000
#define		REG_CHOPCONF		0x00080000
#define		REG_SMARTEN			0x000A0000
#define		REG_SGCSCONF		0x000C0000
#define		REG_DRVCONF			0x000E0000

/* register value */
#define		MICROSTEP_256		0x00
#define		MICROSTEP_128		0x01
#define		MICROSTEP_64		0x02
#define		MICROSTEP_32		0x03
#define		MICROSTEP_16		0x04
#define		MICROSTEP_8			0x05
#define		MICROSTEP_4			0x06
#define		MICROSTEP_2			0x07
#define		MICROSTEP_1			0x08

/* 默认配置 */
#define		SCG_DEFAULT			0x10000


/* 控制IO */
/* spi cs */
#define		TMC2660_PORT_CS		GPIOA
#define		TMC2660_GPIO_CS		GPIO_Pin_4		

/* enable */
#define		TMC2660_PORT_EN		GPIOC			
#define		TMC2660_GPIO_EN		GPIO_Pin_4

/* step */
#define		TMC2660_PORT_STEP	GPIOB			
#define		TMC2660_GPIO_STEP	GPIO_Pin_0

/* dir */
#define		TMC2660_PORT_DIR	GPIOC			
#define		TMC2660_GPIO_DIR	GPIO_Pin_5	

/* light sensor */
#define	  	LIGHT_IN			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)					 
#define		LIGHT_PORT			GPIOA
#define		LIGHT_GPIO			GPIO_Pin_8

/* tmc2660 spi device */
static struct spi_dev_device	tmc2660_spi_dev[1];

/* 片选信号 */
static void spi_cs0(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(TMC2660_PORT_CS, TMC2660_GPIO_CS);
	}
    else
    {
		GPIO_ResetBits(TMC2660_PORT_CS, TMC2660_GPIO_CS);
	}
}

/* 使能控制 */
static void enable_ctrl(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(TMC2660_PORT_EN, TMC2660_GPIO_EN);
	}
    else
    {
		GPIO_ResetBits(TMC2660_PORT_EN, TMC2660_GPIO_EN);
	}
}

/* 步进控制 */
static void step_ctrl(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(TMC2660_PORT_STEP, TMC2660_GPIO_STEP);
	}
    else
    {
		GPIO_ResetBits(TMC2660_PORT_STEP, TMC2660_GPIO_STEP);
	}
}

/* 方向切换控制 */
static void dir_ctrl(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(TMC2660_PORT_DIR, TMC2660_GPIO_DIR);
	}
    else
    {
		GPIO_ResetBits(TMC2660_PORT_DIR, TMC2660_GPIO_DIR);
	}
}

/* tmc2660 spi收发函数 */
static uint32_t tmc2660_spi_xfer(uint8_t spi_no,uint32_t write_data)
{
    uint8_t send_buff[3] = {0};
	uint8_t recv_buff[3] = {0};
    uint32_t recv_data= 0;
	
    send_buff[0] = (write_data>>16)&0xff;
    send_buff[1] = (write_data>>8)&0xff;
    send_buff[2] = (write_data&0xff);
    spi_send_recv(&tmc2660_spi_dev[spi_no], send_buff, recv_buff, 3);
    recv_data = recv_buff[0]<<16 | recv_buff[1]<<8 | recv_buff[2];
		
    return (recv_data&0x0fffff);
}

/* tmc2660 初始化 */
int8_t tmc2660_init(struct spi_bus_device *spi_bus)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if (NULL == spi_bus)
	{
		return -1;
	}
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	/* spi cs */
	spi_cs0(0x01);
	GPIO_InitStructure.GPIO_Pin = TMC2660_GPIO_CS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(TMC2660_PORT_CS, &GPIO_InitStructure);
	
	/* device init */
	tmc2660_spi_dev[0].spi_cs = spi_cs0;
	tmc2660_spi_dev[0].spi_bus = spi_bus;

	/* enable */
	enable_ctrl(0x00);
	GPIO_InitStructure.GPIO_Pin = TMC2660_GPIO_EN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    		
	GPIO_Init(TMC2660_PORT_EN,&GPIO_InitStructure);
	
	/* step	*/
	GPIO_InitStructure.GPIO_Pin = TMC2660_GPIO_STEP;								
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   		
	GPIO_Init(TMC2660_PORT_STEP,&GPIO_InitStructure);
	
	/* dir */
	enable_ctrl(0x00);
	GPIO_InitStructure.GPIO_Pin = TMC2660_GPIO_DIR;										
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		    		
	GPIO_Init(TMC2660_PORT_DIR,&GPIO_InitStructure);
					
	/* light */
	GPIO_InitStructure.GPIO_Pin =  LIGHT_GPIO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			
	GPIO_Init(LIGHT_PORT, &GPIO_InitStructure);
	
	/* 0X0010->305mV  0X0050->165mV 与电机电流相关	*/													
	//tmc2660_spi_xfer(MOTOR_INDEX_0, REG_DRVCONF | 0x0010); 
	tmc2660_spi_xfer(MOTOR_INDEX_0 ,REG_DRVCONF | 0x0050); 
	
	tmc2660_spi_xfer(MOTOR_INDEX_0,REG_DRVCTRL | MICROSTEP_16);
	tmc2660_spi_xfer(MOTOR_INDEX_0,0x901b4);	
	tmc2660_spi_xfer(MOTOR_INDEX_0,0xa0202);/* 0xa0202->1/2CS 0xa8202->1/4CS */
	tmc2660_spi_xfer(MOTOR_INDEX_0,REG_SGCSCONF | SCG_DEFAULT | 0x00);/* 后5位为电流大小 */
	tmc2660_set_force(0x00, MOTOR_INDEX_0);		/* 默认力矩 */
	tmc2660_set_subdivide(0x04, MOTOR_INDEX_0); /* 默认16细分 */
	
	return 0;
}

/* 电机转动方向 */
void tmc2660_set_direct(uint8_t direct, uint8_t motor_index)
{
	dir_ctrl(direct);
}

/* 电机转动一步 */
void tmc2660_move_step(uint8_t direct, uint8_t motor_index)
{
	/* 定时器翻转io产生脉冲输入step引脚，一个脉冲走一步 */
}

/* 设定电机力矩 */
void tmc2660_set_force(u8 force,u8 motor_index)
{
	uint8_t temp;
	uint32_t cmd = 0;
	
	temp = force/8;
	cmd = REG_SGCSCONF | SCG_DEFAULT | temp;
	tmc2660_spi_xfer(motor_index, cmd);
}

/* 获取原点位 */
uint8_t tmc2660_light_sw_state(uint8_t motor_index)
{
	return LIGHT_IN;
}

/* 设置tmc2660细分 */
void tmc2660_set_subdivide(uint8_t mode, uint8_t motor_index)
{
	uint8_t step = 0;
	uint8_t cmd = 0;
	
	switch(mode)
	{
		case 0:
			step = MICROSTEP_1;
		break;
		case 1:
			step = MICROSTEP_2;
		break;
		case 2:
			step = MICROSTEP_4;
		break;
		case 3:
			step = MICROSTEP_8;
		break;
		case 4:
			step = MICROSTEP_16;
		break;
		case 5:
			step = MICROSTEP_32;
		break;
		case 6:
			step = MICROSTEP_64;
		break;
		case 7:
			step = MICROSTEP_128;
		break;
		case 8:
			step = MICROSTEP_256;
		break;
		default:
			step = MICROSTEP_16;
		break;
	}
	cmd = REG_DRVCTRL | step;
	tmc2660_spi_xfer(motor_index,cmd);
}
	
/* 获取tmc2660状态信息 */
uint32_t tmc2660_read_state(uint8_t motor_index)
{
	return tmc2660_spi_xfer(motor_index, 0x00);
}

