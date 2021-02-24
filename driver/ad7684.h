#ifndef _AD7684_H_
#define	_AD7684_H_

#include "spi_core.h"

extern int8_t ad7684_init(struct spi_bus_device *spi_bus);
extern uint16_t ad7684_read(uint8_t index);

#endif
