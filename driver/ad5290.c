
/*
 * File      : ad5290.c
 * 
 * This file is part of ad5290 driver for stm32f1xx.
 * COPYRIGHT (C) 2019-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-24     Acuity      first version.
 */
 
#include <string.h>
#include "ad5290.h"

/* ad9520 spi device */
static struct spi_dev_device  ad5290_spi_dev[3];


/* ad5290 cs function */
static void ad5290_spi_cs0(uint8_t state)
{
	if (state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	}
}
static void ad5290_spi_cs1(uint8_t state)
{
	if (state)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
	}
}
static void ad5290_spi_cs2(uint8_t state)
{
	if (state)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);
	}
}

int8_t ad5290_init(struct spi_bus_device *spi_bus)
{	
	GPIO_InitTypeDef GPIO_InitStructure;

	if (NULL == spi_bus)
	{
		return -1;
	}
	
	/* spi cs */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	GPIO_SetBits(GPIOC,GPIO_Pin_9);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	
	/* device init */
	/* device 0 */
	ad5290_spi_dev[0].spi_cs  = ad5290_spi_cs0;
	ad5290_spi_dev[0].spi_bus = spi_bus;
	
	/* device 1 */
	ad5290_spi_dev[1].spi_cs  = ad5290_spi_cs1;
	ad5290_spi_dev[1].spi_bus = spi_bus;
	
	/* device 2 */
	ad5290_spi_dev[2].spi_cs  = ad5290_spi_cs2;
	ad5290_spi_dev[2].spi_bus = spi_bus;
	
	return 0;
}

/**
  * @brief  set resistance out
  * @param  \set_value limits from 0x00 to 0xff
  *			\index sensor num,1 or 2 or 3
  * @retval none
  */
void ad5290_set_out(uint8_t set_value, uint8_t index)
{
	if (index < 1 || index > 3)
	{
		return;
	}
	spi_send(&ad5290_spi_dev[index-1], &set_value, 1);
}	
