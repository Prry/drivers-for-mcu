
#ifndef _LIS3DH_H_
#define _LIS3DH_H_

#include <stdint.h>
#include <stdbool.h>

extern uint8_t lis3dh_read_xyz(uint16_t xyz[3]);
extern uint8_t lis3dh_init(void);

#endif