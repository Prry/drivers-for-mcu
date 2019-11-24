#ifndef _AD5290_H_
#define	_AD5290_H_

#include <stdint.h>

extern void ad5290_init(void);
extern void ad5290_set_out(uint8_t set_value,uint8_t index);

#endif
