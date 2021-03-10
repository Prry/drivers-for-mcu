
#include <stdint.h>
#include <string.h>
#include "spi_hw.h"
#include "i2c_hw.h"
#include "25xx.h"
#include "ad7684.h"
#include "ad5290.h"
#include "tmc2660.h"
#include "24clxx.h"
#include "ad7793.h"

struct i2c_dev_device i2c0;
struct spi_bus_device spi_bus0;
struct spi_bus_device spi_bus1;

/**
 * @brief  
 * @param  
 * @retval 
 */
int main(void)
{
	stm32f1xx_spi_init(&spi_bus1, 8, &spi_bus1, 8); /* spi bus init */
	stm32f1xx_i2c_init(&i2c0);
	
	ee_25xx_init(&spi_bus1);
	ad7684_init(&spi_bus1);
	ad5290_init(&spi_bus1);
	ad7793_init(&spi_bus0);
	tmc2660_init(&spi_bus0);
	ee_24clxx_init(&i2c0);
	
	for(;;)
	{
		;
	}
}
