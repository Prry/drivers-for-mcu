/*
 * File      : ad7684.c
 * 
 * This file is part of ad7684 driver for for stm32f1xx.
 * COPYRIGHT (C) 2016-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016-12-22     Acuity      first version.
 * 2017-04-06     Acuity      add three ad7684 device.
 */

#include <string.h>
#include "spi_hw.h"

/* ad7684 spi device */
static struct spi_dev_device ad7684_spi_dev[3];

static void ad7684_spi_cs0(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
	}
    else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
	}
}

static void ad7684_spi_cs1(uint8_t state)
{
    if (state)
    {    
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
	}
    else
    {
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	}
}

static void ad7684_spi_cs2(uint8_t state)
{
    if (state)
    {
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
	}
    else
    {
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	}
}

int8_t ad7684_init(struct spi_bus_device *spi_bus)
{	
    GPIO_InitTypeDef GPIO_InitStructure;
	
	if (NULL == spi_bus)
	{
		return -1;
	}
	
    /* spi cs */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOA ,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC,GPIO_Pin_6);
    GPIO_SetBits(GPIOC,GPIO_Pin_8);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_11);
	
    /* device init */
    ad7684_spi_dev[0].spi_cs  = ad7684_spi_cs0;
    ad7684_spi_dev[0].spi_bus = spi_bus;
    
    ad7684_spi_dev[1].spi_cs  = ad7684_spi_cs1;
    ad7684_spi_dev[1].spi_bus = spi_bus;
	
    ad7684_spi_dev[2].spi_cs  = ad7684_spi_cs2;
    ad7684_spi_dev[2].spi_bus = spi_bus;

	return 0;
}

/**
  * @brief  read ad data from ad7684.
  * @param  \index ad7684 device,1 or 2 or 3
  * @retval none
  */

uint16_t ad7684_read(uint8_t index)
{
    uint8_t send_buff[3] = {0};
	uint8_t recv_buff[3] = {0};

    if(index < 1 || index > 3)
    {
		return 0;
	}
    spi_send_recv(&ad7684_spi_dev[index-1], send_buff, recv_buff, 3);
    return ((((recv_buff[0] << 16) | (recv_buff[1] << 8) | recv_buff[2])>>2)&0xffff);
}
