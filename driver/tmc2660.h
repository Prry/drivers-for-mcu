#ifndef _TMC2660_H_
#define	_TMC2660_H_

#include "spi_core.h"

#define MOTOR_INDEX_0	(0x0u)

extern int8_t tmc2660_init(struct spi_bus_device *spi_bus);
extern void tmc2660_set_direct(uint8_t direct, uint8_t motor_index);
void tmc2660_move_step(uint8_t direct, uint8_t motor_index);
extern void tmc2660_set_force(uint8_t force,u8 motor_index);
extern uint8_t tmc2660_light_sw_state(uint8_t motor_index);
extern void tmc2660_set_subdivide(uint8_t mode,uint8_t motor_index);
extern uint32_t tmc2660_read_state(uint8_t motor_index);

#endif
