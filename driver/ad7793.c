
/*
 * File      : ad7793.c
 * 
 * This file is part of ad7793 driver for for stm32f1xx.
 * COPYRIGHT (C) 2016-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-03-09     Acuity      first version.
 */
 
#include <string.h>
#include "stm32f10x.h"
#include "ad7793.h"

/* 寄存器 */
#define REG_COMM_STA	0x00	 /* 8bit,写时为通讯寄存器/读时为状态寄存器 */  
#define REG_MODE		0x08	 /* 16bit,模式寄存器 */
#define REG_CONFIG		0x10	 /* 16bit,配置寄存器 */
#define	REG_DATA		0x18     /* 24bit,数据寄存器 */
#define REG_ID			0x20	 /* 8bit,芯片ID寄存器 */
#define	REG_IO			0x28	 /* 8bit,IO寄存器	*/
#define	REG_IMBL		0x30	 /* 24bit,失调寄存器 */
#define	REG_FULL		0x38	 /* 24bit,满量程寄存器 */

/* 读写指令 */
#define READ_EN			0x40	 /* 读使能 */
#define	CONTINUE_READ	0x20	 /* 连续读 */

/* 工作模式配置 */
#define	CONTINUE_CONVER	0x0000	 /* 连续转换,默认 */
#define	SINGLE_CONVER	0x2000	 /* 单次转换 */
#define	IDLE_MODE	    0x4000	 /* 空闲模式 */
#define	LOW_POWER_MODE	0x6000	 /* 省电模式 */
#define	IZL_CALIBRATION	0x8000	 /* 内部零电平校准 */
#define	IFC_CALIBRATION	0xA000	 /* 内部满量程校准 */
#define	SZL_CALIBRATION	0xC000	 /* 系统零电平校准 */
#define	SFC_CALIBRATION	0xE000	 /* 系统满量程校准 */

/* 恒流源配置 */
#define CURRENT_OUT1_1	0x00	/* 两路电流正常输出到IO1、IO2 */
#define CURRENT_OUT1_2	0x04	/* 两路电流输出IO1、IO2互换 */
#define CURRENT_OUT1	0x08	/* 两路电流叠加输出到IO1，只对10uA和210uA有效 */
#define CURRENT_OUT2	0x0C	/* 两路电流叠加输出到IO2，只对10uA和210uA有效 */
#define CURRENT_10UA	0x01	/* 10uA电流 */
#define CURRENT_210UA	0x02	/* 210uA电流 */
#define CURRENT_1MA	    0x03	/* 1mA电流 */

/* 放大器配置 */
#define	BIPOLAR		0x0000	 /* 双极性 */
#define	UNIPOLAR	0x1000	 /* 单极性 */
#define GAIN_1      0x0000	 /* 增益1~128 */
#define GAIN_2      0x0100	 
#define GAIN_4      0x0200	 
#define GAIN_8      0x0300	 
#define GAIN_16     0x0400	 
#define GAIN_32     0x0500	 	
#define GAIN_64     0x0600	   
#define GAIN_128    0x0700	 

/* ADC配置 */
#define OUTER_VREF  0x0000	 /* 外部参考源 */
#define INNER_VREF  0x0080 	 /* 内部参考源 */
#define NO_BUF		0x0000	 /* 无缓存 */
#define USE_BUF		0x0010	 /* 使用缓存 */
#define AIN_CH1     0x0000	 /* 检测通道1~3 */
#define AIN_CH2     0x0001	
#define AIN_CH3     0x0002	

/* ad7793 工作模式选择 */
#define MODE_NONE	    0x00    
#define MODE_BIPOLAR	0x01	/* 双极性模式 */
#define MODE_UNIPOLAR	0x02	/* 单极性模式 */
#define MODE_2LINE_RTD	0x04	/* 两线RTD */
#define MODE_3LINE_RTD	0x08	/* 三线RTD */
#define MODE_4LINE_RTD	0x10	/* 四线RTD */

struct _ad7793_info
{
	uint8_t mode;	/* 工作模式 */
	uint16_t ref;   /* 参考电阻阻值, Ω */
	uint16_t gain;  /* 增益 */
	uint8_t id;		/* 芯片id */
};

/* ad7793 spi device */
static struct spi_dev_device ad7793_spi_dev[1];

/* mode select */
static struct _ad7793_info s_ad7793_info =
{
	.mode = MODE_BIPOLAR|MODE_4LINE_RTD,/* 双精度/4线RTD */
	.ref = 2000,	/* 2kΩ */
	.gain = GAIN_1, /* no gain */
};

static void ad7793_spi_cs(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
    else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
}

static uint8_t ad7793_spi_write(uint8_t addr, uint8_t *pbuf, uint8_t size)
{
	spi_send_then_send(&ad7793_spi_dev[0], &addr, 1, pbuf, size);
	return size;
}

static uint8_t ad7793_spi_write_byte(uint8_t addr, uint8_t data)
{
	ad7793_spi_write(addr, &data, 1);
	return 0x01;
}

static uint8_t ad7793_spi_write_word(uint8_t addr, uint16_t data)
{
  	uint8_t buf[2] = {0};
	
	buf[0] = (data>>8)&0xff;
	buf[1] = data&0xff;
	ad7793_spi_write(addr, &buf[0], 2);
	
	return 0x02;
}

static uint8_t ad7793_spi_write_3byte(uint8_t addr, uint32_t data)
{
  	uint8_t buf[3] = {0};
	
	buf[0] = (data>>16)&0xff;
	buf[1] = (data>>8)&0xff;
	buf[2] = data&0xff;
	ad7793_spi_write(addr, &buf[0], 3);
	
	return 0x03;
}

static uint8_t ad7793_spi_read(uint8_t addr, uint8_t *pbuf, uint8_t size)
{
	spi_send_then_recv(&ad7793_spi_dev[0], &addr, 1, pbuf, size);
	
	return size;	
}

static void ad7793_soft_reset(void)
{
  	uint8_t buf[] ={0xff, 0xff, 0xff,0xff};
	
	ad7793_spi_write(REG_COMM_STA, &buf[0], 4);
}

static void ad7793_register_init(void)
{
  	uint16_t mode = 0x00;
	
	/* read chip id */
	ad7793_spi_read(REG_ID, &s_ad7793_info.id, 1);
	
	/* 65db抑制 */
  	ad7793_spi_write_word(REG_MODE, 0x000A);

	if (s_ad7793_info.mode&MODE_UNIPOLAR)
	{
		mode |= MODE_UNIPOLAR;
	}
	else if (s_ad7793_info.mode&MODE_BIPOLAR)
	{
		mode |= MODE_BIPOLAR;
	}
	
	if (s_ad7793_info.gain > GAIN_128)
	{
		s_ad7793_info.gain = GAIN_128;
	}
	mode |= s_ad7793_info.gain;

	/* 双极性/无增益/外部参考源/无缓存/通道AIN1 */
	ad7793_spi_write_word(REG_CONFIG, mode|OUTER_VREF|NO_BUF|AIN_CH1);

	/* 210uA恒流源输出 */
	ad7793_spi_write_byte(REG_IO, CURRENT_OUT1_1|CURRENT_210UA);
}

int8_t ad7793_read_data(int32_t *ad_value)
{
  	int8_t ret = 0;
	uint8_t temp = 0;
	uint8_t buf[3] = {0};
	
	ad7793_spi_read(REG_COMM_STA|READ_EN, &temp ,1);/* 读取转换完成状态 */
	
	if (0xbf == (temp|0xbf))
	{
		ad7793_spi_read(REG_DATA|READ_EN, &buf[0] ,3);
		*ad_value = ((buf[0]<<16) | (buf[1]<<8) | buf[2])&0xffffff;
	}
	else
	{
		/* todo，出错？ */
	  	ret = -1;
	}
	return ret;
}

int8_t ad7793_init(struct spi_bus_device *spi_bus)
{	
    GPIO_InitTypeDef GPIO_InitStructure;
	
	if (NULL == spi_bus)
	{
		return -1;
	}
	
    /* spi cs */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /* device init */
    ad7793_spi_dev[0].spi_cs  = ad7793_spi_cs;
    ad7793_spi_dev[0].spi_bus = spi_bus;

	ad7793_soft_reset();
	ad7793_register_init();
	
	return 0;
}

int8_t ad7793_read_resistance(float *resistance)
{
  	int8_t ret = 0;
	int32_t ad_value = 0;
	uint8_t gain = 0;
	
	if (0 != ad7793_read_data(&ad_value))
	{
		return -1;
	}
	
	gain = 0x01<<(s_ad7793_info.gain>>8);	/* 计算增益 */
	
	/* 根据不同模式计算 */
	if (s_ad7793_info.mode&MODE_BIPOLAR)
	{
		*resistance = (float)s_ad7793_info.ref *(ad_value/0x7fffff-1) / gain;
	}
	else if (s_ad7793_info.mode&MODE_UNIPOLAR)
	{
		*resistance = (float)s_ad7793_info.ref * ad_value / 0xffffff / gain;
	}
	
	if (s_ad7793_info.mode&MODE_3LINE_RTD)
	{/* 三线RTD需要两路恒流源 */
		*resistance *= 2;
	}
	
	return ret;
}
