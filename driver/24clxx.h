#ifndef _24CLxx_H_
#define _24CLxx_H_

#include <stdint.h>
#include "i2c_core.h"

extern int8_t ee_24clxx_init(struct i2c_dev_device *i2c_bus);
extern void ee_24clxx_readbytes(uint16_t read_addr, char *pbuff, uint16_t read_size);
extern char ee_24clxx_writebyte(uint16_t write_addr, uint8_t data);
extern char ee_24clxx_writebytes(uint16_t write_addr, char *pbuff, uint16_t write_size);
extern char ee_24clxx_erasebytes(uint16_t erase_addr, char erase_data, uint16_t erase_size);

#endif
