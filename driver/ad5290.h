#ifndef _AD5290_H_
#define	_AD5290_H_

#include "spi_core.h"

extern int8_t ad5290_init(struct spi_bus_device *spi_bus);
extern void ad5290_set_out(uint8_t set_value, uint8_t index);

#endif
