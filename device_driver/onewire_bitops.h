#ifndef _ONEWIRE_BITOPS_H_
#define _ONEWIRE_BITOPS_H_
#include "stdint.h"

struct ops_onewire_dev
{
	void (*set_sdo)(int8_t state);
	uint8_t (*get_sdo)(void);
	void (*delayus)(uint32_t us);
};

extern uint8_t ops_onewire_reset(struct ops_onewire_dev *onewire);
extern int ops_onewire_read(struct ops_onewire_dev *onewire,void *buff,int size);
extern int ops_onewire_write(struct ops_onewire_dev *onewire,void *buff,int size);

#endif
