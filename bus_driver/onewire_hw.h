#ifndef _ONEWIRE_HW_
#define _ONEWIRE_HW_

#include "stm32f10x.h"
#include "onewire_bitops.h"

//onewire1 device
extern struct ops_onewire_dev onewire1_dev;

#define	ONEWIRE1_RCC	RCC_APB2Periph_GPIOC
#define	ONEWIRE1_PORT	GPIOC
#define	ONEWIRE1_PIN	GPIO_Pin_13

extern void stm32f1xx_onewire1_init(void);

#endif
