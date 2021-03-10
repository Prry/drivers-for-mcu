
#ifndef _AD7793_H_
#define	_AD7793_H_

#include "spi_core.h"

extern int8_t ad7793_init(struct spi_bus_device *spi_bus);
extern int8_t ad7793_read_resistance(float *resistance);

#endif
