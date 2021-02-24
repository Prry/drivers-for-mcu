
#include <stdint.h>
#include <string.h>
#include "spi_hw.h"
#include "25xx.h"
#include "ad7684.h"
#include "ad5290.h"

struct 	spi_bus_device 	spi_bus1;

/**
 * @brief  
 * @param  
 * @retval 
 */
int main(void)
{
	stm32f1xx_spi_init(0, 0, &spi_bus1, 8); /* spi bus init */
	
	ee_25xx_init(&spi_bus1);
	ad7684_init(&spi_bus1);
	ad5290_init(&spi_bus1);
	
	for(;;)
	{
		;
	}
}
