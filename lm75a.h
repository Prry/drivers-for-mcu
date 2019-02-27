#ifndef 	_LM75A_H_
#define		_LM75A_H_

#define 	LM75A_ADDR		0X48   	//i2c slave address
#define		REG_TEMP		0X00	//温度，只读
#define		REG_CONF		0X01	//配置
#define		REG_THYST		0X02	//滞后控制
#define		REG_TOS			0X03	//超温关闭寄存器

extern void lm75a_init(void);
extern float lm75a_get_temp(void);

#endif
