#ifndef _25XX_H_
#define	_25XX_H_

#include "spi_core.h"

/* extern function */
extern void ee_25xx_init(struct spi_bus_device *spi_bus);
extern void ee_25xx_write_bytes(uint16_t write_addr, uint8_t *write_buff, uint16_t write_bytes);
extern void ee_25xx_read_bytes(uint16_t read_addr,uint8_t *read_buff,uint16_t read_bytes);
	
#endif
