/*
 * File      : onewire_hw.c
 * 
 * This file is part of onewire_hw.c.
 * COPYRIGHT (C) 2018-
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-10     Acuity      first version.
 */
 
#include "onewire_hw.h"

struct ops_onewire_dev onewire1_dev;

static void gpio_set_sdo(int8_t state)
{
	if (state)
		GPIO_SetBits(ONEWIRE1_PORT,ONEWIRE1_PIN); 
	else
		GPIO_ResetBits(ONEWIRE1_PORT,ONEWIRE1_PIN); 
}

static uint8_t gpio_get_sdo(void)
{
	return (GPIO_ReadInputDataBit(ONEWIRE1_PORT,ONEWIRE1_PIN));
}

static void gpio_delayus(uint32_t us)
{
#if 1  /* 不用系统延时时开启这个！！*/
    volatile int32_t i;
	
    for (; us > 0; us--)
    {
        i = 30;  //mini 17
        while(i--);
    }
#else
		Delayus(us);
#endif
}

/**
  * @brief  初始化单总线
  * @param  none
  * @retval none
*/
void stm32f1xx_onewire1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										
	RCC_APB2PeriphClockCmd(ONEWIRE1_RCC,ENABLE);		

	GPIO_InitStructure.GPIO_Pin = ONEWIRE1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   		
	GPIO_Init(ONEWIRE1_PORT, &GPIO_InitStructure);						   				
	ONEWIRE1_PORT->BSRR = ONEWIRE1_PIN;		 

	/* device init */
	onewire1_dev.get_sdo = gpio_get_sdo;
	onewire1_dev.set_sdo = gpio_set_sdo;
	onewire1_dev.delayus = gpio_delayus;
}
